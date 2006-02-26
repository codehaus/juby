
#include <ruby.h>
#include <jni.h>

#include "juby.h"
#include "juby_jni.h"

VALUE juby_initialize_vm(VALUE self, VALUE classpath) {

  DOUT( "juby_intialize_vm" );

  JavaVMInitArgs vm_args;
  JavaVM* vm;

  char *jvmClasspath;

  if ( classpath == Qnil ) {
    jvmClasspath = CLASSPATH_PREFIX JUBY_JAR;
  } else {
    jvmClasspath = malloc( strlen( CLASSPATH_PREFIX JUBY_JAR ) + RSTRING( classpath )->len  + 1 );
    sprintf( jvmClasspath, CLASSPATH_PREFIX "%s:" JUBY_JAR, StringValuePtr( classpath ) );
  }

  JavaVMOption options[2];
  options[0].optionString = jvmClasspath;
  options[1].optionString = "-Djava.compiler=NONE";
  //options[1].optionString = "-verbose:jni";

  vm_args.version            = JNI_VERSION_1_2;
  vm_args.options            = options;
  vm_args.nOptions           = 2;
  vm_args.ignoreUnrecognized = JNI_FALSE; 

  JNI_CreateJavaVM(&vm,(void**)&env,&vm_args); 

  DOUT( "vm created" );

  JAVA_STRING.javaClass = (*env)->FindClass( env, "java/lang/String" );

  setup_java_primitive( JAVA_BOOLEAN, "java/lang/Boolean", "booleanValue", "()Z" );
  setup_java_primitive( JAVA_SHORT,   "java/lang/Short",   "shortValue",   "()S" );
  setup_java_primitive( JAVA_INTEGER, "java/lang/Integer", "intValue",     "()I" );
  setup_java_primitive( JAVA_LONG,    "java/lang/Long",    "longValue",    "()J" );
  setup_java_primitive( JAVA_FLOAT,   "java/lang/Float",   "floatValue",   "()F" );
  setup_java_primitive( JAVA_DOUBLE,  "java/lang/Double",  "doubleValue",  "()D" );

  DOUT( "primitives setup" );

  CLASS_CLASS = (*env)->FindClass( env, "java/lang/Class" );
  CLASS_GETNAME_METHOD = (*env)->GetMethodID( env, CLASS_CLASS, "getName", "()Ljava/lang/String;" );
  CLASS_GETSUPERCLASS_METHOD = (*env)->GetMethodID( env, CLASS_CLASS, "getSuperclass", "()Ljava/lang/Class;" );
  CLASS_GETINTERFACES_METHOD = (*env)->GetMethodID( env, CLASS_CLASS, "getInterfaces", "()[Ljava/lang/Class;" );

  DOUT( "java.lang.Class setup" );

  JUBY_CLASS = (*env)->FindClass( env, "com/radarnetworks/juby/Juby" );

  JUBY_ISCLASS_METHOD        = (*env)->GetStaticMethodID( env, JUBY_CLASS, "isClass",        "(Ljava/lang/String;)Z" );
  JUBY_GETCLASS_METHOD       = (*env)->GetStaticMethodID( env, JUBY_CLASS, "getClass",       "(Ljava/lang/String;)Ljava/lang/Class;" );
  JUBY_CALLMETHOD_METHOD     = (*env)->GetStaticMethodID( env, JUBY_CLASS, "callMethod",     "(Ljava/lang/Object;Ljava/lang/String;[Lcom/radarnetworks/juby/Value;)Ljava/lang/Object;" );
  JUBY_NEWINSTANCE_METHOD    = (*env)->GetStaticMethodID( env, JUBY_CLASS, "newInstance",     "(Ljava/lang/Class;[Lcom/radarnetworks/juby/Value;)Ljava/lang/Object;" );
  JUBY_ACCESSPROPERTY_METHOD = (*env)->GetStaticMethodID( env, JUBY_CLASS, "accessProperty", "(Ljava/lang/Object;Ljava/lang/String;)Ljava/lang/Object;" );
  JUBY_OBJECTTOS_METHOD      = (*env)->GetStaticMethodID( env, JUBY_CLASS, "objectToS",      "(Ljava/lang/Object;)Ljava/lang/String;" );

  DOUT( "com.radarnetworks.juby.Juby setup" );

  VALUE_CLASS = (*env)->FindClass( env, "com/radarnetworks/juby/Value" );

  if ( (*env)->ExceptionOccurred( env ) ) {
    (*env)->ExceptionDescribe( env );
  }

  VALUE_CONSTRUCTOR = (*env)->GetMethodID( env, VALUE_CLASS, "<init>", "(J)V" );
  VALUE_GETVALUE_METHOD = (*env)->GetMethodID( env, VALUE_CLASS, "getValue", "()J" );


  DOUT( "com.radarnetworks.juby.Value setup" );

  JNINativeMethod valueNativeMethods[6];

  valueNativeMethods[0].name = "callMethod";
  valueNativeMethods[0].signature = "(Ljava/lang/String;[Ljava/lang/Object;)Ljava/lang/Object;";
  valueNativeMethods[0].fnPtr = Java_com_radarnetworks_juby_Value_callMethod;

  DOUT( "callMethod registered" );

  valueNativeMethods[1].name = "callStringMethod";
  valueNativeMethods[1].signature = "(Ljava/lang/String;[Ljava/lang/Object;)Ljava/lang/String;";
  valueNativeMethods[1].fnPtr = Java_com_radarnetworks_juby_Value_callStringMethod;

  DOUT( "callStringMethod registered" );

  valueNativeMethods[2].name = "callIntegerMethod";
  valueNativeMethods[2].signature = "(Ljava/lang/String;[Ljava/lang/Object;)I";
  valueNativeMethods[2].fnPtr = Java_com_radarnetworks_juby_Value_callIntegerMethod;

  DOUT( "callIntegerMethod registered" );

  valueNativeMethods[3].name = "callBooleanMethod";
  valueNativeMethods[3].signature = "(Ljava/lang/String;[Ljava/lang/Object;)Z";
  valueNativeMethods[3].fnPtr = Java_com_radarnetworks_juby_Value_callBooleanMethod;

  DOUT( "callBooleanMethod registered" );

  valueNativeMethods[4].name = "getRubyTypeCode";
  valueNativeMethods[4].signature = "()I";
  valueNativeMethods[4].fnPtr = Java_com_radarnetworks_juby_Value_getRubyTypeCode;

  DOUT( "getRubyTypeCode registered" );

  valueNativeMethods[5].name = "unwrapJavaObject";
  valueNativeMethods[5].signature = "()Ljava/lang/Object;";
  valueNativeMethods[5].fnPtr = Java_com_radarnetworks_juby_Value_unwrapJavaObject;

  DOUT( "unwrapJavaObject registered" );

  (*env)->RegisterNatives( env, VALUE_CLASS, valueNativeMethods, 6 );

  if ( (*env)->ExceptionOccurred( env ) ) {
    (*env)->ExceptionDescribe( env );
  }

  DOUT( "~juby_intialize_vm" );
}

void mixin(VALUE self, jclass javaClass) {
  jstring javaClassName = (*env)->CallObjectMethod( env, javaClass, CLASS_GETNAME_METHOD );
  jsize len = (*env)->GetStringUTFLength( env, javaClassName );
  const char* javaClassNameChars = (*env)->GetStringUTFChars( env, javaClassName, JNI_FALSE );

  char *jubyModuleName = malloc( len + 1 );

  int i;
  for( i = 0 ; i < len ; ++i ) {
    if ( javaClassNameChars[i] == '.' ) {
      jubyModuleName[i] = '_';
    } else {
      jubyModuleName[i] = javaClassNameChars[i];
    }
  }
  jubyModuleName[i] = 0;

  char *jubyFqModuleName = malloc( strlen( JUBY_MIXIN_PREFIX ) + len );
  sprintf( jubyFqModuleName, JUBY_MIXIN_PREFIX "%s\0", jubyModuleName );
 
  VALUE jubyModule = rb_eval_string_protect( jubyFqModuleName, 0 );

  if ( jubyModule != Qnil ) {
    rb_extend_object( self, jubyModule );
  }
}


VALUE juby_is_class(VALUE self, VALUE name) {
  jstring nameUtf = (*env)->NewStringUTF( env, RSTRING( name )->ptr );

  int result = (*env)->CallStaticBooleanMethod( env, JUBY_CLASS, JUBY_ISCLASS_METHOD, nameUtf );

  if ( result ) {
    return Qtrue;
  }

  return Qfalse;
}

VALUE juby_get_class(VALUE self, VALUE name) {

  DOUT( "juby_get_class" );

  jstring nameUtf = (*env)->NewStringUTF( env, RSTRING( name )->ptr );

  jobject result = (*env)->CallStaticObjectMethod( env, JUBY_CLASS, JUBY_GETCLASS_METHOD, nameUtf );

  if ( (*env)->ExceptionOccurred( env ) ) {
    (*env)->ExceptionDescribe( env );
  }

  if ( ! result ) {
    return Qnil;
  }

  result = (*env)->NewGlobalRef( env, result );

  return Data_Wrap_Struct( cJavaClass, 0, 0, &result );
}

VALUE extract_ruby_object(jobject javaObject) {
  DOUT( "extract_ruby_object" );
	jlong value = (*env)->CallLongMethod( env, javaObject, VALUE_GETVALUE_METHOD );
	return (VALUE) value;
  DOUT( "~extract_ruby_object" );
}

VALUE coerce_to_ruby_type(jobject javaObject) {
  DOUT( "coerce_to_ruby_type" );

	if ( ! javaObject ) {
		return Qnil;
	} else if ( is_instance( javaObject, JAVA_STRING ) ) {
		const char *strValue = (*env)->GetStringUTFChars( env, javaObject, JNI_FALSE );
		return rb_str_new2( strValue );
	} else if ( is_instance( javaObject, JAVA_BOOLEAN ) ) {
		return extract_boolean( javaObject ) ? Qtrue : Qfalse;
	} else if ( is_instance( javaObject, JAVA_SHORT ) ) {
		return INT2FIX( extract_short( javaObject ) );
	} else if ( is_instance( javaObject, JAVA_INTEGER ) ) {
		return INT2FIX( extract_integer( javaObject ) );
	} else if ( is_instance( javaObject, JAVA_LONG ) ) {
		return LONG2FIX( extract_long( javaObject ) );
	} else if ( is_instance( javaObject, JAVA_FLOAT ) ) {
		return rb_float_new( extract_float( javaObject ) );
	} else if ( is_instance( javaObject, JAVA_DOUBLE ) ) {
		return rb_float_new( extract_double( javaObject ) );
	} else {
		return wrap_with_ruby_class( javaObject );
	}
  DOUT( "~coerce_to_ruby_type" );
}

VALUE wrap_with_ruby_class(jobject javaObject) {
  DOUT( "wrap_with_ruby_class" );

  javaObject = (*env)->NewGlobalRef( env, javaObject );

  jobject *heapResult = malloc( sizeof( javaObject ) );

  memcpy( heapResult, &javaObject, sizeof( javaObject ) ); 

  VALUE rubyResult = Data_Wrap_Struct( cJavaObject, 0, 0, heapResult );

  jclass javaClass = (*env)->GetObjectClass( env, javaObject );

  while ( javaClass != 0 ) {

    mixin( rubyResult, javaClass );

    jobjectArray javaInterfaces = (*env)->CallObjectMethod( env, javaClass, CLASS_GETINTERFACES_METHOD );

    int numInterfaces = (*env)->GetArrayLength( env, javaInterfaces );

    int i;
    for ( i = 0 ; i < numInterfaces ; ++i ) {
      jobject javaInterface = (*env)->GetObjectArrayElement( env, javaInterfaces, i );
      mixin( rubyResult, javaInterface );
    }

    javaClass = (*env)->CallObjectMethod( env, javaClass, CLASS_GETSUPERCLASS_METHOD );
  }

  DOUT( "~wrap_with_ruby_class" );
  return rubyResult;
}

jobject wrap_for_java(VALUE rubyObject) {
  DOUT( "wrap_for_java" );
  return (*env)->NewObject( env, VALUE_CLASS, VALUE_CONSTRUCTOR, (jlong) rubyObject );
}

VALUE object_access_property(VALUE self, VALUE property) {
  DOUT( "object_access_property" );

  jstring propertyUtf = (*env)->NewStringUTF( env, RSTRING( property )->ptr );

  jobject *javaObject; 
  Data_Get_Struct( self, jobject, javaObject );

  jobject result = (*env)->CallStaticObjectMethod( env, JUBY_CLASS, JUBY_ACCESSPROPERTY_METHOD, *javaObject, propertyUtf );

  DOUT( "~object_access_property" );
  return coerce_to_ruby_type( result );
}

VALUE class_new_instance(VALUE self, VALUE args) {
  jclass *javaClass; 
  Data_Get_Struct( self, jclass, javaClass );

  int numArgs = NUM2INT( rb_funcall( args,  rb_intern( "size" ), 0 ) );
  jobjectArray javaArgs = (*env)->NewObjectArray( env, numArgs, VALUE_CLASS, 0 );

  int i;
  for ( i = 0 ; i < numArgs; ++i ) {
    (*env)->SetObjectArrayElement( env, javaArgs, i, wrap_for_java( rb_ary_entry( args, i ) ) );
  }

  jobject result = (*env)->CallStaticObjectMethod( env, JUBY_CLASS, JUBY_NEWINSTANCE_METHOD, *javaClass, javaArgs );

  if ( (*env)->ExceptionOccurred( env ) ) {
    (*env)->ExceptionDescribe( env );
  }

  DOUT( "~class_new_instance" );

  return coerce_to_ruby_type( result );
}


VALUE object_call_method(VALUE self, VALUE methodName, VALUE args) {
  DOUT( "object_call_method" );

  jstring methodNameUtf = (*env)->NewStringUTF( env, RSTRING( methodName )->ptr );

  jobject *javaObject; 
  Data_Get_Struct( self, jobject, javaObject );

  int numArgs = NUM2INT( rb_funcall( args,  rb_intern( "size" ), 0 ) );
  jobjectArray javaArgs = (*env)->NewObjectArray( env, numArgs, VALUE_CLASS, 0 );

  int i;
  for ( i = 0 ; i < numArgs; ++i ) {
    (*env)->SetObjectArrayElement( env, javaArgs, i, wrap_for_java( rb_ary_entry( args, i ) ) );
  }

  jobject result = (*env)->CallStaticObjectMethod( env, JUBY_CLASS, JUBY_CALLMETHOD_METHOD, *javaObject, methodNameUtf, javaArgs );

  if ( (*env)->ExceptionOccurred( env ) ) {
    (*env)->ExceptionDescribe( env );
  }

  DOUT( "~object_call_method" );

  return coerce_to_ruby_type( result );
}

VALUE object_to_s(VALUE self) {
  DOUT( "object_to_s" );

  jobject *javaObject; 

  Data_Get_Struct( self, jobject, javaObject );

  jstring result = (*env)->CallStaticObjectMethod( env, JUBY_CLASS, JUBY_OBJECTTOS_METHOD, *javaObject );

  if ( (*env)->ExceptionOccurred( env ) ) {
    (*env)->ExceptionDescribe( env );
    return rb_str_new2( "" );
  }

  if ( ! result ) {
    return rb_str_new2( "" );
  }

  const char *strValue = (*env)->GetStringUTFChars( env, result, JNI_FALSE );

  DOUT( "~object_to_s" );

  return rb_str_new2( strValue );
}

void Init_juby() {

  DOUT( "Init_juby" );
  
  mJava = rb_define_module( "Java" );

  cJavaPackage = rb_define_class_under( mJava, "Package", rb_cObject );
  cJavaObject  = rb_define_class_under( mJava, "Object",  rb_cObject );
  cJavaClass   = rb_define_class_under( mJava, "Class",   cJavaObject );

  rb_define_module_function( mJava, "juby_initialize_vm", juby_initialize_vm, 1 );
  rb_define_module_function( mJava, "juby_is_class",      juby_is_class, 1 );
  rb_define_module_function( mJava, "juby_get_class",     juby_get_class, 1 );

  rb_define_method( cJavaObject, "access_property", object_access_property, 1 );
  rb_define_method( cJavaObject, "call_method",     object_call_method,     2 );
  rb_define_method( cJavaObject, "[",               object_access_property, 1 );
  rb_define_method( cJavaObject, "to_s",            object_to_s,            0 );
  rb_define_method( cJavaClass,  "new_instance",    class_new_instance,     1 );

  rb_require( "juby/juby" );

  DOUT( "~Init_juby" );
}
