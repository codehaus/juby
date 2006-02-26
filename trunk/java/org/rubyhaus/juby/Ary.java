
package org.rubyhaus.juby;

public class Ary extends Value {

	public Ary(long value) {
		super( value );
	}

	public native int size();
}
