
#include <ruby.h>
#include "juby.h"
#include "juby_jni.h"

VALUE _call_method(JNIEnv *env, jobject self, jstring methodName, jobjectArray args);

JNIEXPORT jobject JNICALL Java_com_radarnetworks_juby_Value_unwrapJavaObject(JNIEnv *env, jobject self) {
  VALUE rubySelf = extract_ruby_object( self );

  jobject *javaObject; 

  Data_Get_Struct( rubySelf, jobject, javaObject );

  return *javaObject;
}

JNIEXPORT jint JNICALL Java_com_radarnetworks_juby_Value_getRubyTypeCode(JNIEnv *env, jobject self) {
  VALUE rubySelf = extract_ruby_object( self );
  return (jint) TYPE( rubySelf );
}

JNIEXPORT jobject JNICALL Java_com_radarnetworks_juby_Value_callMethod(JNIEnv *env, jobject self, jstring methodName, jobjectArray args) {
  VALUE result = _call_method( env, self, methodName, args );
  jobject javaResult = (jobject) wrap_for_java( result );
  return javaResult;
}

JNIEXPORT jstring JNICALL Java_com_radarnetworks_juby_Value_callStringMethod(JNIEnv *env, jobject self, jstring methodName, jobjectArray args) {
  VALUE result = _call_method( env, self, methodName, args );
  jstring javaString = (*env)->NewStringUTF( env, RSTRING( result )->ptr );
  return javaString;
}

JNIEXPORT jint JNICALL Java_com_radarnetworks_juby_Value_callIntegerMethod(JNIEnv *env, jobject self, jstring methodName, jobjectArray args) {
  VALUE result = _call_method( env, self, methodName, args );
  jint javaInt = rb_num2int( result );
  return javaInt;
}

JNIEXPORT jboolean JNICALL Java_com_radarnetworks_juby_Value_callBooleanMethod(JNIEnv *env, jobject self, jstring methodName, jobjectArray args) {
  VALUE result = _call_method( env, self, methodName, args );

  if ( !result || result == Qnil || result == Qfalse ) {
    return JNI_FALSE;
  }

  return JNI_TRUE;
}


JNIEXPORT jint JNICALL Java_com_radarnetworks_juby_Ary_size(JNIEnv *env, jobject self) {
  return 0;
}

VALUE _call_method(JNIEnv *env, jobject self, jstring methodName, jobjectArray args) {

  jsize numArgs = (*env)->GetArrayLength( env, args );

  VALUE *rubyArgs = malloc( sizeof( VALUE ) * numArgs );

  int i;
  for ( i = 0 ; i < numArgs ; ++i ) {
	rubyArgs[i] = coerce_to_ruby_type( (*env)->GetObjectArrayElement( env, args, i ) );
  }

  const char* methodNameChars = (*env)->GetStringUTFChars( env, methodName, JNI_FALSE );
  ID methodSym = rb_intern( methodNameChars );
  (*env)->ReleaseStringUTFChars( env, methodName, methodNameChars );

  VALUE rubySelf = extract_ruby_object( self );

  VALUE result = rb_funcall2( rubySelf, methodSym, numArgs, rubyArgs );

  return result;
}
