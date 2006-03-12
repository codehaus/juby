#include <jni.h>

#include "juby.h"
#include "jniutil.h"
#include "juby_rni.h"

VALUE mJava;
VALUE cJavaPackage;
VALUE cJavaClass;
VALUE cJavaObject;

void setUpRNI() {
	
	DEBUG_ENTER( "setUpRNI()" );

	mJava = rb_define_module( "Java" );

	cJavaPackage = rb_define_class_under( mJava, "Package", rb_cObject );
	cJavaObject  = rb_define_class_under( mJava, "Object",  rb_cObject );
	cJavaClass   = rb_define_class_under( mJava, "Class",   cJavaObject );

	rb_define_module_function( mJava, "juby_initialize_vm", juby_initialize_vm, 1 );
	rb_define_module_function( mJava, "juby_get_class",     juby_get_class, 1 );

	rb_define_method( cJavaObject, "access_property", object_access_property, 1 );
	rb_define_method( cJavaObject, "call_method",     object_call_method,     2 );
	rb_define_method( cJavaObject, "[",               object_access_property, 1 );
	rb_define_method( cJavaObject, "to_s",            object_to_s,            0 );
	rb_define_method( cJavaClass,  "new_instance",    class_new_instance,     1 );

	rb_require( "juby/juby" );
	
	DEBUG_EXIT( "setUpRNI()" );
}

VALUE juby_initialize_vm(VALUE self, VALUE classpath) {
	DEBUG_ENTER( "juby_initialize_vm(...)" );
	// TODO: pass in custom classpath
	setUpJNIUtils();
	DEBUG_EXIT( "juby_initialize_vm(...)" );
}


VALUE juby_get_class(VALUE self, VALUE name) {
	
	DEBUG_ENTER( "juby_get_class(...)" );
	
	JNIEnv *env = attachJNIEnv();
	
	jstring nameUtf = (*env)->NewStringUTF( env, RSTRING( name )->ptr );
	checkException( env );

	jobject result = (*env)->CallObjectMethod( env, JUBY_INSTANCE, JUBY_GETCLASS_METHOD, nameUtf );
	checkException( env );

	if ( ! result ) {
		detachJNIEnv();
		DEBUG_EXIT( "juby_get_class(...)" );
		return Qnil;
	}

	result = (*env)->NewGlobalRef( env, result );
	checkException( env );

	detachJNIEnv();
	
	DEBUG_EXIT( "juby_get_class(...)" );
	
	return Data_Wrap_Struct( cJavaClass, 0, object_free, result );
}

VALUE object_call_method(VALUE self, VALUE methodName, VALUE args) {
	
	JNIEnv *env = attachJNIEnv();

	jstring methodNameUtf = (*env)->NewStringUTF( env, RSTRING( methodName )->ptr );
	checkException( env );

	jobject javaObject; 
	Data_Get_Struct( self, struct _jobject, javaObject );

	int numArgs = NUM2INT( rb_funcall( args,  rb_intern( "size" ), 0 ) );
	jobjectArray javaArgs = (*env)->NewObjectArray( env, numArgs, VALUE_CLASS, 0 );
	checkException( env );

	int i;
	for ( i = 0 ; i < numArgs; ++i ) {
		(*env)->SetObjectArrayElement( env, javaArgs, i, wrap_for_java( env, rb_ary_entry( args, i ) ) );
		checkException( env );
	}

	jobject result = (*env)->CallObjectMethod( env, JUBY_INSTANCE, JUBY_CALLMETHOD_METHOD, javaObject, methodNameUtf, javaArgs );
	checkException( env );	

	VALUE value = coerce_to_ruby_type( env, result );
	
	detachJNIEnv();
	
	return value;
}


VALUE object_access_property(VALUE self, VALUE propertyName) {
	
	DEBUG_ENTER( "object_access_property(...)" );
	
	JNIEnv *env = attachJNIEnv();
	
	jstring propertyNameUtf = (*env)->NewStringUTF( env, RSTRING( propertyName )->ptr );
	checkException( env );

	jobject javaObject; 
	Data_Get_Struct( self, struct _jobject, javaObject );
	
	jobject result = (*env)->CallObjectMethod( env, JUBY_INSTANCE, JUBY_ACCESSPROPERTY_METHOD, javaObject, propertyNameUtf, 0 );
	checkException( env );
	
	VALUE value = coerce_to_ruby_type( env, result );
	
	detachJNIEnv();
	
	DEBUG_EXIT( "object_access_property(...)" );
	
	return value;
}

VALUE object_to_s(VALUE self) {
	
	JNIEnv *env = attachJNIEnv();

	jobject javaObject; 

	Data_Get_Struct( self, struct _jobject, javaObject );

	jstring result = (*env)->CallObjectMethod( env, JUBY_INSTANCE, JUBY_OBJECTTOS_METHOD, javaObject );

	if ( (*env)->ExceptionOccurred( env ) ) {
		(*env)->ExceptionDescribe( env );
		detachJNIEnv();
		return rb_str_new2( "" );
	}

	if ( ! result ) {
		detachJNIEnv();
		return rb_str_new2( "" );
	}

	//TODO: Free these UTF Chars
	const char *strValue = (*env)->GetStringUTFChars( env, result, JNI_FALSE );

	detachJNIEnv();
	return rb_str_new2( strValue );
}

VALUE class_new_instance(VALUE self, VALUE args) {
	
	JNIEnv *env = attachJNIEnv();
	
	jclass javaClass; 
	Data_Get_Struct( self, struct _jobject, javaClass );

	int numArgs = NUM2INT( rb_funcall( args,  rb_intern( "size" ), 0 ) );
	
	jobjectArray javaArgs = (*env)->NewObjectArray( env, numArgs, VALUE_CLASS, 0 );
	checkException( env );

	int i;
	for ( i = 0 ; i < numArgs; ++i ) {
		(*env)->SetObjectArrayElement( env, javaArgs, i, wrap_for_java( env, rb_ary_entry( args, i ) ) );
		checkException( env );
	}

	jobject result = (*env)->CallObjectMethod( env, JUBY_INSTANCE, JUBY_NEWINSTANCE_METHOD, javaClass, javaArgs );
	checkException( env );

	VALUE value = coerce_to_ruby_type( env, result );
	
	detachJNIEnv();
	
	return value;
}

