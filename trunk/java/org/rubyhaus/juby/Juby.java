package org.rubyhaus.juby;

import java.lang.reflect.Constructor;
import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Member;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;
import java.util.Collection;
import java.util.Iterator;
import java.util.Map;

public class Juby {

	private static final Object[] EMPTY_OBJECT_ARRAY = new Object[0];
	private static final Value[] EMPTY_VALUE_ARRAY = new Value[0];
	private static final Class[] EMPTY_CLASS_ARRAY = new Class[0];
	private static final Class[] STRING_CLASS_ARRAY = new Class[] { String.class };
	
	public Juby() {
		// intentionally left blank
	}

	public Class getClass(String name) {
		ClassLoader cl = Thread.currentThread().getContextClassLoader();
		
		if ( cl == null ) {
			cl = getClass().getClassLoader();
		}
		
		try {
			return cl.loadClass( name );
		} catch (Exception e) {
			if (name.indexOf(".") < 0) {
				try {
					return cl.loadClass("java.lang." + name);
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

	public Object bridge(Object object, String sym, Value[] args) throws Exception {
		Object value = null;
		
		if ( sym.endsWith( "=" ) ) {
			value = bridgeAssignment( object, sym, args );
		} else {
			value = bridgeAccess( object, sym, args );
		}
		
		return value;
	}
	
	protected Object bridgeAssignment(Object object, String sym, Value[] args) throws Exception {
		return null;
	}
	
	protected Object bridgeAccess(Object object, String sym, Value[] args) throws Exception {
		Object value = null;
		
		if ( args.length == 0 ) {
			value = bridgeSimpleAccess( object, sym );
		}  else {
			value = bridgeComplexAccess( object, sym, args );
		}
		
		return value;
	}
	
	protected Object bridgeSimpleAccess(Object self, String sym) throws Exception {

		if ( "class".equals( sym ) ) {
			return self.getClass();
		}

		// Special cases are required here due to some oddness (bug?)
		// about how accessing anonymous inner-class implementations
		// of Iterator that some Java collections return from iterator().

		if ( "iterator".equals( sym ) && self instanceof Collection ) {
			return ((Collection)self).iterator();
		}

		if ( "hasNext".equals( sym ) && self instanceof Iterator ) {
			return ((Iterator)self).hasNext() ? Boolean.TRUE : Boolean.FALSE;
		}

		if ( "next".equals( sym ) && self instanceof Iterator ) {
			return ((Iterator)self).next();
		}

		if ( "key".equals( sym ) && self instanceof Map.Entry ) {
			return ((Map.Entry)self).getKey();
		}

		if ( "value".equals( sym ) && self instanceof Map.Entry ) {
			return ((Map.Entry)self).getValue();
		}
		
		Accessor accessor = findAccessor( self, sym );
		
		if ( accessor != null ) {
			return accessor.access();
		}
		
		// TODO: throw something to demonstrate that we weren't
		// able to find diddly to call.
		return null;
	}
	
	private Accessor findAccessor(Object self, String sym) {
		Accessor accessor = findJavaBeanPropertyAccessor( self, sym );
		
		if ( accessor == null ) {
			accessor = findFieldAccessor( self, sym );
		}
		
		if ( accessor == null ) {
			accessor = findNoArgMethodAccessor( self, sym );
		}
		
		if ( accessor == null ) {
			accessor = findGetWithArgAccessor( self, sym );
		}
		
		return accessor;
	}
	
	private Accessor findJavaBeanPropertyAccessor(Object self, String sym) {
		Class selfClass = self.getClass();
		
		String javaName = makeJavaName( sym, true );
		
		String accessorMethodName = "get" + javaName;
		
		try {
			Method m = selfClass.getMethod( accessorMethodName, EMPTY_CLASS_ARRAY );
			if ( ! isStatic( m ) ) {
				return new JavaBeanPropertyAccessor( self, m );
			}
		} catch (SecurityException e) {
		} catch (NoSuchMethodException e) {
		}
		
		accessorMethodName = "is" + javaName;
		
		try {
			Method m = selfClass.getMethod( accessorMethodName, EMPTY_CLASS_ARRAY );
			if ( ! isStatic( m ) ) {
				return new JavaBeanPropertyAccessor( self, m );
			}
		} catch (SecurityException e) {
		} catch (NoSuchMethodException e) {
		}
		
		if ( self instanceof Class ) {
		
			accessorMethodName = "get" + javaName;
			
			try {
				Method m = ((Class)self).getMethod( accessorMethodName, EMPTY_CLASS_ARRAY );
				if ( isStatic( m ) ) {
					return new JavaBeanPropertyAccessor( self, m );
				}
			} catch (SecurityException e) {
			} catch (NoSuchMethodException e) {
			}
			
			accessorMethodName = "is" + javaName ;
			
			try {
				Method m = ((Class)self).getMethod( accessorMethodName, EMPTY_CLASS_ARRAY );
				if ( isStatic( m ) ) {
					return new JavaBeanPropertyAccessor( self, m );
				}
			} catch (SecurityException e) {
			} catch (NoSuchMethodException e) {
			}
		}
		
		return null;
	}
	
	private Accessor findGetWithArgAccessor(Object self, String sym) {
		Class selfClass = self.getClass();
		
		try {
			Method m = selfClass.getMethod( "get", STRING_CLASS_ARRAY );
			if ( ! isStatic( m ) ) {
				return new GetWithArgAccessor( self, m, sym );
			}
		} catch (SecurityException e) {
		} catch (NoSuchMethodException e) {
		}
		
		if ( self instanceof Class ) {
			try {
				Method m = ((Class)self).getMethod( "get", STRING_CLASS_ARRAY );
				if ( isStatic( m ) ) {
					return new GetWithArgAccessor( self, m, sym );
				}
			} catch (SecurityException e) {
			} catch (NoSuchMethodException e) {
			}
		}
		
		return null;
	}
	
	private Accessor findFieldAccessor(Object self, String sym) {
		Class selfClass = self.getClass();
		
		String fieldName = makeJavaName( sym, false );
		
		try {
			Field f = selfClass.getField( fieldName );
			if ( ! isStatic( f ) ) {
				return new FieldAccessor( self, f );
			}
		} catch (SecurityException e) {
		} catch (NoSuchFieldException e) {
		}
		
		if ( self instanceof Class ) {
			try {
				Field f = ((Class)self).getField( fieldName );
				if ( isStatic( f ) ) {
					return new FieldAccessor( self, f );
				}
			} catch (SecurityException e) {
			} catch (NoSuchFieldException e) {
			}
		}
		
		return null;
	}
	
	private Accessor findNoArgMethodAccessor(Object self, String sym) {
		Class selfClass = self.getClass();
		
		String methodName = makeJavaName( sym, false );
		
		try {
			Method m = selfClass.getMethod( methodName, EMPTY_CLASS_ARRAY );
			if ( ! isStatic( m ) ) {
				return new NoArgMethodAccessor( self, m );
			}
		} catch (SecurityException e) {
		} catch (NoSuchMethodException e) {
		}
		
		if ( self instanceof Class ) {
			try {
				Method m = ((Class)self).getMethod( methodName, EMPTY_CLASS_ARRAY );
				if ( isStatic( m ) ) {
					return new NoArgMethodAccessor( self, m );
				}
			} catch (SecurityException e) {
			} catch (NoSuchMethodException e) {
			}
		}
		
		return null;
	}
	
	private boolean isStatic(Member member) {
		int modifiers = member.getModifiers();
		
		return Modifier.isPublic( modifiers) && Modifier.isStatic( modifiers);
	}
	
	private String makeJavaName(String name, boolean initialCapital) {
		int len = name.length();
		
		StringBuffer javaName = new StringBuffer();
		
		boolean upperCaseNext = initialCapital;
		
		for ( int i = 0 ; i < len ; ++i ) {
			char c = name.charAt( i );
			
			if ( name.charAt( i ) == '_' ) {
				upperCaseNext = true;
				continue;
			} 
			
			if ( upperCaseNext ) {
				javaName.append( Character.toUpperCase( c ) );
				upperCaseNext = false;
			} else {
				javaName.append( c );
			}
		}
		
		return javaName.toString();
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
