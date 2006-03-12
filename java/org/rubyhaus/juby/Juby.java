package org.rubyhaus.juby;

import java.lang.reflect.Constructor;
import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;
import java.util.Collection;
import java.util.Iterator;
import java.util.Map;

public class Juby {

	private static final Object[] EMPTY_OBJECT_ARRAY = new Object[0];
	
	public Juby() {
		// intentionally left blank
	}

	public Class getClass(String name) {
		try {
			return Class.forName(name);
		} catch (Exception e) {
			if (name.indexOf(".") < 0) {
				try {
					return Class.forName("java.lang." + name);
				} catch (Exception e2) {
				}
			}
		}
		return null;
	}

	public String objectToS(Object object) {
		if (object != null) {
			return object.toString();
		}

		return null;
	}

	public Object newInstance(Class javaClass, Value[] args) {
		Constructor[] constructors = javaClass.getConstructors();
		CONSTRUCTORS: for (int i = 0; i < constructors.length; ++i) {
			int modifiers = constructors[i].getModifiers();
			if (Modifier.isPublic(modifiers)) {
				Class[] paramTypes = constructors[i].getParameterTypes();
				if (paramTypes.length == args.length) {
					for (int j = 0; j < args.length; ++j) {
						if (!paramTypes[j].isAssignableFrom(args[j]
								.getJavaType())) {
							continue CONSTRUCTORS;
						}
					}
					Object result = null;
					Object[] javaArgs = coerceParameters(args, paramTypes);
					try {
						result = constructors[i].newInstance(javaArgs);
					} catch (IllegalAccessException e) {
						e.printStackTrace();
					} catch (InvocationTargetException e) {
						e.getTargetException().printStackTrace();
					} catch (InstantiationException e) {
						e.printStackTrace();
					}
					return result;
				}
			}
		}
		return null;
	}

	
	// ----------------------------------------
	// ----------------------------------------

	public Object bridge(Object object, String sym, Value[] args) {
		Object value = null;
		
		if ( sym.endsWith( "=" ) ) {
			value = bridgeAssignment( object, sym, args );
		} else {
			value = bridgeAccess( object, sym, args );
		}
		
		return value;
	}
	
	protected Object bridgeAssignment(Object object, String sym, Value[] args) {
		return null;
	}
	
	protected Object bridgeAccess(Object object, String sym, Value[] args) {
		Object value = null;
		
		if ( args.length == 0 ) {
			value = bridgeSimpleAccess( object, sym );
		}  else {
			value = bridgeComplexAccess( object, sym, args );
		}
		
		return value;
	}
	
	protected Object bridgeSimpleAccess(Object object, String sym) {

		if ("class".equals(sym)) {
			return object.getClass();
		}

		Class objectClass = null;

		if (object instanceof Class) {
			objectClass = (Class) object;
		} else {
			objectClass = object.getClass();
		}
		
		// Special cases are required here due to some oddness (bug?)
		// about how accessing anonymous inner-class implementations
		// of Iterator that some Java collections return from iterator().

		if ("iterator".equals(sym) && object instanceof Collection) {
			return ((Collection) object).iterator();
		}

		if ("hasNext".equals(sym) && object instanceof Iterator) {
			return ((Iterator) object).hasNext() ? Boolean.TRUE : Boolean.FALSE;
		}

		if ("next".equals(sym) && object instanceof Iterator) {
			return ((Iterator) object).next();
		}

		if ("key".equals(sym) && object instanceof Map.Entry) {
			return ((Map.Entry) object).getKey();
		}

		if ("value".equals(sym) && object instanceof Map.Entry) {
			return ((Map.Entry) object).getValue();
		}

		try {
			Field field = objectClass.getField(sym);
			return field.get(object);
		} catch (Exception e) {
			try {
				Method method = objectClass.getMethod(sym, new Class[0]);
				Object value = method.invoke(object, EMPTY_OBJECT_ARRAY);
				return value;
			} catch (Exception e2) {
				try {
					String accessorName = "get"
							+ sym.substring(0, 1).toUpperCase()
							+ sym.substring(1);
					Method method = objectClass.getMethod(accessorName,
							new Class[0]);
					return method.invoke(object, EMPTY_OBJECT_ARRAY);
				} catch (Exception e3) {
					try {
						Method method = objectClass.getMethod("get",
								new Class[] { Object.class });
						return method.invoke(object, new Object[] { sym });
					} catch (Exception e4) {
						if (objectClass == object) {
							try {
								objectClass = object.getClass();
								String accessorName = "get"
										+ sym.substring(0, 1).toUpperCase()
										+ sym.substring(1);
								Method method = objectClass.getMethod(
										accessorName, new Class[0]);
								return method
										.invoke(object, EMPTY_OBJECT_ARRAY);
							} catch (Exception e5) {
								// ignore
							}
						}
					}
				}
			}
		}
		return null;	
	}
	
	public Object bridgeComplexAccess(Object self, String sym, Value[] args) {
		
		Method method = findMethod(self.getClass(), sym, args);

		Object result = null;

		if (method != null) {
			Object[] javaArgs = coerceParameters(args, method
					.getParameterTypes());
			try {
				result = method.invoke(self, javaArgs);
			} catch (IllegalAccessException e) {
				e.printStackTrace();
			} catch (InvocationTargetException e) {
				e.getTargetException().printStackTrace();
			}
		}

		return result;
	}
	
	protected Method findMethod(Class selfClass, String name, Value[] args) {
		Method[] methods = selfClass.getMethods();

		METHODS: 
			for (int i = 0; i < methods.length; ++i) {
				if (methods[i].getName().equals(name)) {
					int modifiers = methods[i].getModifiers();
					if (!Modifier.isStatic(modifiers) && Modifier.isPublic(modifiers)) {
						Class[] paramTypes = methods[i].getParameterTypes();
						if (paramTypes.length == args.length) {
							for (int j = 0; j < args.length; ++j) {
								if (!paramTypes[j].isAssignableFrom(args[j] .getJavaType())) {
									continue METHODS;
								}
							}
							return methods[i];
						}
					}
				}
			} 
		
		return null;
	}
	

	/** Examine the parameters from Ruby to determine which are originally Java objects.
	 * 
	 * <p>This method extracts the types of any inherently Java objects passed
	 * as parameters.  For ruby types, which are flexible, no type is extracted
	 * and the position in the resulting <code>Class[]</code> array contains a null.
	 * </p>
	 * 
	 * @param parameters The ruby value parameters.
	 * 
	 * @return An array of <code>Class</code> the same size as the passed in parameters.
	 */
	private Class[] getPhaseOneParameterTypes(Value[] parameters) {
		Class[] types = new Class[ parameters.length ];
		
		for ( int i = 0 ; i < parameters.length ; ++i ) {
			if ( parameters[i].isInherentlyJava() ) {
				types[i] = parameters[i].getJavaType();
			}
		}
		return types;
	}
	
	/** Given the true parameter types, and phase one partial parameter types, determine if
	 *  a match is possible.
	 *  
	 *  <p>For each non-null phase-one parameter type, assignability is tested against the
	 *  true parameter type.  Each null parameter is ignored for this comparison.</p>
	 *  
	 * @param parameterTypes The true parameters of the candidate method or constructor.
	 * @param phaseOneParameterTypes The parameter types determined from phase-one analysis.
	 * 
	 * @return <code>true</code> if the parameter types match as far as possible, otherwise
	 *         <code>false</code>.
	 */
	private boolean isPhaseOneMatch(Class[] parameterTypes, Class[] phaseOneParameterTypes) {
		for ( int i = 0 ; i < parameterTypes.length ; ++i ) {
			if ( phaseOneParameterTypes[i] != null ) {
				if ( ! parameterTypes[i].isAssignableFrom( phaseOneParameterTypes[i] ) ) {
					return false;
				}
			}
		}
		
		return true;
	}
	
	private boolean isPhaseTwoMatch(Class[] parameterTypes, Class[] phaseOneParameterTypes) {
		for ( int i = 0 ; i < parameterTypes.length ; ++i ) {
			if ( phaseOneParameterTypes[i] == null ) {
				if ( ! isCoercibleFromRuby( parameterTypes[i] ) ) {
					return false;
				}
			}
		}
		
		return true;
	}
	
	private boolean isCoercibleFromRuby(Class javaType) {
		// if string, double, float, etc, true!
		return false;
	}

	private Object[] coerceParameters(Value[] args, Class[] paramTypes) {
		Object[] javaArgs = new Object[args.length];

		for (int i = 0; i < javaArgs.length; ++i) {
			if (paramTypes[i] == String.class) {
				javaArgs[i] = args[i].toString();
			}
		}

		return javaArgs;
	}

}
