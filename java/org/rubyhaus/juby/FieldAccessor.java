package org.rubyhaus.juby;

import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;

public class FieldAccessor implements Accessor {
	
	private Object object;
	private Field field;

	public FieldAccessor(Object object, Field field) {
		this.object = object;
		this.field  = field;
	}

	public Object access() throws IllegalArgumentException, IllegalAccessException, InvocationTargetException {
		return field.get( object );
	}

}
