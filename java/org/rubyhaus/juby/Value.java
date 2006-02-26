package org.rubyhaus.juby;

import java.util.Map;
import java.util.HashMap;
import java.math.BigInteger;
import java.io.File;

public class Value {

  public static final class Type {

    private static final Map mappings = new HashMap();

    public static Type get(int typeCode) {
      return (Type) mappings.get( new Integer( typeCode ) );
    }

    private int typeCode;
    private String description;
    private Class javaType;

    public Type(int typeCode, String description, Class javaType) {
      this.typeCode    = typeCode;
      this.description = description;
      this.javaType    = javaType;
      mappings.put( new Integer( typeCode ), this );
    }

    public Class getJavaType() {
      return javaType;
    }

    public String toString() {
      return description;
    }

    public static final Type NONE   = new Type( 0x00, "T_NONE",   Void.TYPE );
    public static final Type NIL    = new Type( 0x01, "T_NIL",    Void.TYPE );
    public static final Type OBJECT = new Type( 0x02, "T_OBJECT", Object.class );
    public static final Type CLASS  = new Type( 0x03, "T_CLASS",  Class.class );
    public static final Type ICLASS = new Type( 0x04, "T_ICLASS", null );
    public static final Type MODULE = new Type( 0x05, "T_MODULE", null  );
    public static final Type FLOAT  = new Type( 0x06, "T_FLOAT",  Float.class );
    public static final Type STRING = new Type( 0x07, "T_STRING", String.class );
    public static final Type REGEXP = new Type( 0x08, "T_REGEXP", null );
    public static final Type ARRAY  = new Type( 0x09, "T_ARRAY",  null );
    public static final Type FIXNUM = new Type( 0x0a, "T_FIXNUM", Long.class );
    public static final Type HASH   = new Type( 0x0b, "T_HASH",   Map.class );
    public static final Type STRUCT = new Type( 0x0c, "T_STRUCT", Object.class );
    public static final Type BIGNUM = new Type( 0x0d, "T_BIGNUM", BigInteger.class );
    public static final Type FILE   = new Type( 0x0e, "T_FILE",   File.class );
  
    public static final Type TRUE   = new Type( 0x20, "T_TRUE",   Boolean.class );
    public static final Type FALSE  = new Type( 0x21, "T_FALSE",  Boolean.class );
    public static final Type DATA   = new Type( 0x22, "T_DATA",   Object.class );
    public static final Type MATCH  = new Type( 0x23, "T_MATCH",  null );
    public static final Type SYMBOL = new Type( 0x24, "T_SYMBOL", String.class );
  
    public static final Type BLKTAG = new Type( 0x3b, "T_BLKTAG", null );
    public static final Type UNDEF  = new Type( 0x3c, "T_UNDEF",  null );
    public static final Type VARMAP = new Type( 0x3d, "T_VARMAP", null );
    public static final Type SCOPE  = new Type( 0x3e, "T_SCOPE",  null );
    public static final Type NODE   = new Type( 0x3f, "T_NODE",   null );
  
    public static final Type MASK   = new Type( 0x3f, "T_MASK",   null );
    

  }

  private static final Object[] EMPTY_OBJECT_ARRAY = new Object[0];

  private long value;

  public Value(long value) {
    this.value = value;
  }

  public long getValue() {
    return value;
  }

  public Type getRubyType() {
    int typeCode = getRubyTypeCode();
    return Type.get( typeCode );
  }

  public Class getJavaType() {
    Type rubyType = getRubyType();
    if ( rubyType == null ) {
      return null;
    }
    return rubyType.getJavaType();
  }

  public boolean isInherentlyJava() {
    return getRubyType() == Type.DATA;
  }

  public Object getJavaObject() {
    if ( isInherentlyJava() ) {
      return unwrapJavaObject();
    }
    return null;
  }

  private native int getRubyTypeCode();

  public native Object callMethod(String name, Object[] args);
  public native String callStringMethod(String name, Object[] args);
  public native int callIntegerMethod(String name, Object[] args);
  public native boolean callBooleanMethod(String name, Object[] args);
  private native Object unwrapJavaObject();

  public String to_s() {
    return callStringMethod( "to_s", EMPTY_OBJECT_ARRAY );
  }

  public String toString() {
    return to_s();
  }
}
