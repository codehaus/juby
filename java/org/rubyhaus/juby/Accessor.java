package org.rubyhaus.juby;

import java.lang.reflect.InvocationTargetException;

public interface Accessor {
	
	Object access() throws IllegalArgumentException, IllegalAccessException, InvocationTargetException;

}
