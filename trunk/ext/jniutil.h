
#ifndef JNIUTIL_H
#define JNIUTIL_H

#include <jni.h>
#include "juby.h"

#define CLASSPATH_PREFIX "-Djava.class.path="

typedef struct JavaPrimitiveClass {
	jclass javaClass;
	jmethodID primitiveExtractor;
} JavaPrimitiveClass;

void setUpJNIUtils();

JNIEnv *setUpJVM(const char *classpath);

void setUpJavaReflection(JNIEnv *env);
void setUpJavaPrimitiveClass(JNIEnv *env,  JavaPrimitiveClass *var,  const char *classSpec,  const char *name,  const char *signature);
void registerNativeMethods(JNIEnv *env);

jboolean extractBoolean(JNIEnv *env, jobject object);
jshort extractShort(JNIEnv *env, jobject object);
jint extractInteger(JNIEnv *env, jobject object);
jlong extractLong(JNIEnv *env, jobject object);
jfloat extractFloat(JNIEnv *env, jobject object);
jdouble extractDouble(JNIEnv *env, jobject object);
jboolean isInstance(JNIEnv *env, jobject javaObject, JavaPrimitiveClass *primitiveHolder);

void checkException(JNIEnv *env);
							
JNIEnv *attachJNIEnv();
void detachJNIEnv();
	
static JavaVM *jvm;

JUBY_EXTERN JavaPrimitiveClass JAVA_STRING;
JUBY_EXTERN JavaPrimitiveClass JAVA_BOOLEAN;
JUBY_EXTERN JavaPrimitiveClass JAVA_SHORT;
JUBY_EXTERN JavaPrimitiveClass JAVA_INTEGER;
JUBY_EXTERN JavaPrimitiveClass JAVA_LONG;
JUBY_EXTERN JavaPrimitiveClass JAVA_FLOAT;
JUBY_EXTERN JavaPrimitiveClass JAVA_DOUBLE;

JUBY_EXTERN jclass CLASS_CLASS;
JUBY_EXTERN jmethodID CLASS_GETNAME_METHOD;
JUBY_EXTERN jmethodID CLASS_GETSUPERCLASS_METHOD;
JUBY_EXTERN jmethodID CLASS_GETINTERFACES_METHOD;

JUBY_EXTERN jclass JUBY_CLASS;

JUBY_EXTERN jmethodID JUBY_ISCLASS_METHOD;
JUBY_EXTERN jmethodID JUBY_GETCLASS_METHOD;
JUBY_EXTERN jmethodID JUBY_CALLMETHOD_METHOD;
JUBY_EXTERN jmethodID JUBY_NEWINSTANCE_METHOD;
JUBY_EXTERN jmethodID JUBY_ACCESSPROPERTY_METHOD;
JUBY_EXTERN jmethodID JUBY_OBJECTTOS_METHOD;

JUBY_EXTERN jclass VALUE_CLASS;
JUBY_EXTERN jmethodID VALUE_CONSTRUCTOR;
JUBY_EXTERN jmethodID VALUE_GETVALUE_METHOD;

void dumpJavaClass(JNIEnv* env, jclass javaClass);

#endif /* JNIUTIL_H */
