
#include <ruby.h>
#include <jni.h>

#include "juby.h"
#include "juby_jni.h"
#include "juby_rni.h"
#include "jniutil.h"

void mixin(JNIEnv *env, VALUE self, jclass javaClass) {
	
	jstring javaClassName = (*env)->CallObjectMethod( env, javaClass, CLASS_GETNAME_METHOD );
	checkException( env );
	
	jsize len = (*env)->GetStringUTFLength( env, javaClassName );
	checkException( env );
	
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
	
	(*env)->ReleaseStringUTFChars( env, javaClassName, javaClassNameChars );

	char *jubyFqModuleName = malloc( strlen( JUBY_MIXIN_PREFIX ) + len + 1);
	sprintf( jubyFqModuleName, JUBY_MIXIN_PREFIX "%s\0", jubyModuleName );
	
	free( jubyModuleName );
 
	VALUE jubyModule = rb_eval_string_protect( jubyFqModuleName, 0 );
	
	free( jubyFqModuleName );

	if ( jubyModule != Qnil ) {
		rb_extend_object( self, jubyModule );
	}
	
}

VALUE extract_ruby_object(JNIEnv *env, jobject javaObject) {
	jlong value = (*env)->CallLongMethod( env, javaObject, VALUE_GETVALUE_METHOD );
	return (VALUE) value;
}

VALUE coerce_to_ruby_type(JNIEnv *env, jobject javaObject) {
	
	VALUE value = 0;
	
	if ( ! javaObject ) {
		value = Qnil;
	} else if ( isInstance( env, javaObject, &JAVA_STRING ) ) {
		const char *strValue = (*env)->GetStringUTFChars( env, javaObject, JNI_FALSE );
		value = rb_str_new2( strValue );
		(*env)->ReleaseStringUTFChars( env, javaObject, strValue );
	} else if ( isInstance( env, javaObject, &JAVA_BOOLEAN ) ) {
		value = extractBoolean( env, javaObject ) ? Qtrue : Qfalse;
	} else if ( isInstance( env, javaObject, &JAVA_SHORT ) ) {
		value = INT2FIX( extractShort( env, javaObject ) );
	} else if ( isInstance( env, javaObject, &JAVA_INTEGER ) ) {
		value = INT2FIX( extractInteger( env, javaObject ) );
	} else if ( isInstance( env, javaObject, &JAVA_LONG ) ) {
		value = LONG2FIX( extractLong( env, javaObject ) );
	} else if ( isInstance( env, javaObject, &JAVA_FLOAT ) ) {
		value = rb_float_new( extractFloat( env, javaObject ) );
	} else if ( isInstance( env, javaObject, &JAVA_DOUBLE ) ) {
		value = rb_float_new( extractDouble( env, javaObject ) );
	} else {
		value = wrap_with_ruby_class( env, javaObject );
	}
	
	return value;
}

VALUE wrap_with_ruby_class(JNIEnv *env, jobject javaObject) {
	
	DEBUG_ENTER( "wrap_with_ruby_class(...)" );

	javaObject = (*env)->NewGlobalRef( env, javaObject );
	
	checkException( env );
	
	VALUE rubyResult = Data_Wrap_Struct( cJavaObject, 0, object_free, javaObject );

	jclass javaClass = (*env)->GetObjectClass( env, javaObject );

	checkException( env );
	
	while ( javaClass != 0 ) {

		mixin( env, rubyResult, javaClass );
		
		jobjectArray javaInterfaces = (*env)->CallObjectMethod( env, javaClass, CLASS_GETINTERFACES_METHOD );
		checkException( env );
		
		int numInterfaces = (*env)->GetArrayLength( env, javaInterfaces );
		checkException( env );
		
		int i;
		for ( i = 0 ; i < numInterfaces ; ++i ) {
			jobject javaInterface = (*env)->GetObjectArrayElement( env, javaInterfaces, i );
			checkException( env );
			
			mixin( env, rubyResult, javaInterface );
		}

		javaClass = (*env)->CallObjectMethod( env, javaClass, CLASS_GETSUPERCLASS_METHOD );
		checkException( env );
	}
  
	DEBUG_EXIT( "wrap_with_ruby_class(...)" );

	return rubyResult;
}

jobject wrap_for_java(JNIEnv *env, VALUE rubyObject) {
	// TODO: How will Ruby know when this object is freed?  
	// Will it free it from under us?  Does passing it to Java pin it?
	return (*env)->NewObject( env, VALUE_CLASS, VALUE_CONSTRUCTOR, (jlong) rubyObject );
}

void object_free(void *p) {
	JNIEnv *env = attachJNIEnv();
	(*env)->DeleteGlobalRef(env, (jobject)p);
	detachJNIEnv();
}

void Init_juby() {
	DEBUG_ENTER( "Init_juby" );
	
	setUpRNI();
	
	DEBUG_EXIT( "Init_juby" );
}
