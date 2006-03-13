package org.rubyhaus.juby;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

public class NoArgMethodAccessor implements Accessor {
	
	private static final Object[] EMPTY_OBJECT_ARRAY = new Object[0];
	
	private Object object;
	private Method method;

	public NoArgMethodAccessor(Object object, Method method) {
		this.object = object;
		this.method = method;
	}

	public Object access() throws IllegalArgumentException, IllegalAccessException, InvocationTargetException {
		return method.invoke( object, EMPTY_OBJECT_ARRAY );
	}

}
