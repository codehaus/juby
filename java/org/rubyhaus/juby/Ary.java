
package com.radarnetworks.juby;

public class Ary extends Value {

	public Ary(long value) {
		super( value );
	}

	public native int size();
}
