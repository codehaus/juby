#include <jni.h>

#include "juby.h"
#include "jniutil.h"
#include "juby_rni.h"

VALUE mJava;
VALUE mJubyJavaLangClass;
VALUE cJavaPackage;
//VALUE cJavaClass;
VALUE cJavaObject;

static juby_initialized;

void setUpRNI() {
	
	DEBUG_ENTER( "setUpRNI()" );

	mJava = rb_define_module( "Java" );

	cJavaPackage = rb_define_class_under( mJava, "Package", rb_cObject );
	cJavaObject  = rb_define_class_under( mJava, "Object",  rb_cObject );
	//cJavaClass   = rb_define_class_under( mJava, "Class",   cJavaObject );

	rb_define_module_function( mJava, "juby_initialize_vm", juby_initialize_vm, 1 );
	rb_define_module_function( mJava, "juby_get_class",     juby_get_class, 1 );

	rb_define_method( cJavaObject, "bridge",          juby_bridge,     2 );
	
	rb_define_method( cJavaObject, "to_s",            object_to_s,            0 );
	//rb_define_method( cJavaClass,  "new_instance",    class_new_instance,     1 );
	
	mJubyJavaLangClass = rb_define_module( "Juby_java_lang_Class" );
	rb_define_module_function( mJubyJavaLangClass, "new_instance", class_new_instance, 1 );

	rb_require( "juby/juby" );
	
	DEBUG_EXIT( "setUpRNI()" );
}

VALUE juby_initialize_vm(VALUE self, VALUE classpath) {
	DEBUG_ENTER( "juby_initialize_vm(...)" );
	
	if ( juby_initialized ) {
		return;
	}
	
	const char *classpathChars = 0;
	
	if ( classpath != Qnil ) {
		classpathChars = RSTRING( classpath )->ptr;
	}
	
	setUpJNIUtils( classpathChars );
	
	juby_initialized = 1;
	DEBUG_EXIT( "juby_initialize_vm(...)" );
}


VALUE juby_get_class(VALUE self, VALUE name) {
	
	DEBUG_ENTER( "juby_get_class(...)" );
	
	JNIEnv *env = attachJNIEnv();
	
	jstring nameUtf = (*env)->NewStringUTF( env, RSTRING( name )->ptr );
	checkException( env );

	jobject javaClass = (*env)->CallObjectMethod( env, JUBY_INSTANCE, JUBY_GETCLASS_METHOD, nameUtf );
	
	if ( checkException( env ) || ! javaClass ) {
		detachJNIEnv();
		DEBUG_EXIT( "juby_get_class(...)" );
		return Qnil;
	}
	
	javaClass = (*env)->NewGlobalRef( env, javaClass );
	checkException( env );
	
	VALUE rubyObj = wrap_with_ruby_class( env, javaClass );
	
	detachJNIEnv();
	DEBUG_EXIT( "juby_get_class(...)" );
	
	return rubyObj;
}

VALUE juby_bridge(VALUE self, VALUE sym, VALUE args) {
	
	JNIEnv *env = attachJNIEnv();

	jstring symUtf = (*env)->NewStringUTF( env, RSTRING( sym )->ptr );
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

	jobject result = (*env)->CallObjectMethod( env, JUBY_INSTANCE, JUBY_BRIDGE_METHOD, javaObject, symUtf, javaArgs );
	checkException( env );	
	
	VALUE value = coerce_to_ruby_type( env, result );
	
	detachJNIEnv();
	
	return value;
}

VALUE object_to_s(VALUE self) {
	
	JNIEnv *env = attachJNIEnv();

	jobject javaObject; 

	Data_Get_Struct( self, struct _jobject, javaObject );

	jstring result = (*env)->CallObjectMethod( env, JUBY_INSTANCE, JUBY_OBJECTTOS_METHOD, javaObject );
	
	if ( checkException( env ) || ! result ) {
		detachJNIEnv();
		return rb_str_new2( "" );
	}

	const char *resultChars = (*env)->GetStringUTFChars( env, result, JNI_FALSE );
	checkException( env );
	
	VALUE value = rb_str_new2( resultChars );
	
	(*env)->ReleaseStringUTFChars( env, result, resultChars );
	checkException( env );
	
	detachJNIEnv();
	
	return value;	
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

