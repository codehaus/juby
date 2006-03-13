package org.rubyhaus.juby;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

public class GetWithArgAccessor implements Accessor {
	
	private Object object;
	private Method method;
	private Object parameter;

	public GetWithArgAccessor(Object object, Method method, Object parameter) {
		this.object    = object;
		this.method    = method;
		this.parameter = parameter;
	}

	public Object access() throws IllegalArgumentException, IllegalAccessException, InvocationTargetException {
		return method.invoke( object, new Object[] { parameter } );
	}

}
