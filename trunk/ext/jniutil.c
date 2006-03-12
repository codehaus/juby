#include <jni.h>
#include <stdlib.h>
#include <string.h>

#include "juby.h"
#include "jniutil.h"
#include "juby_jni.h"

JavaPrimitiveClass JAVA_STRING;
JavaPrimitiveClass JAVA_BOOLEAN;
JavaPrimitiveClass JAVA_SHORT;
JavaPrimitiveClass JAVA_INTEGER;
JavaPrimitiveClass JAVA_LONG;
JavaPrimitiveClass JAVA_FLOAT;
JavaPrimitiveClass JAVA_DOUBLE;

jclass CLASS_CLASS;
jmethodID CLASS_GETNAME_METHOD;
jmethodID CLASS_GETSUPERCLASS_METHOD;
jmethodID CLASS_GETINTERFACES_METHOD;

jclass JUBY_CLASS;

jmethodID JUBY_CONSTRUCTOR;
jmethodID JUBY_BRIDGE_METHOD;
jmethodID JUBY_GETCLASS_METHOD;
jmethodID JUBY_NEWINSTANCE_METHOD;
jmethodID JUBY_OBJECTTOS_METHOD;

jobject JUBY_INSTANCE;

jclass VALUE_CLASS;
jmethodID VALUE_CONSTRUCTOR;
jmethodID VALUE_GETVALUE_METHOD;

void setUpJNIUtils() {
	DEBUG_ENTER( "setUpJNIUtils()" );
	JNIEnv *env = setUpJVM( 0 );
	
	setUpJavaReflection( env );
	registerNativeMethods( env );
	DEBUG_EXIT( "setUpJNIUtils()" );
}

JNIEnv *setUpJVM(const char *classpath) {
	
	DEBUG_ENTER( "setUpJVM(...)" );

	JavaVMOption options[2];
	
	char *customClasspath = 0;
	
	if ( classpath ) {
		customClasspath = malloc( strlen( CLASSPATH_PREFIX JUBY_JAR_PATH ) + strlen( classpath ) + 1 );
		sprintf( customClasspath, CLASSPATH_PREFIX JUBY_JAR_PATH ":%s", classpath );
		options[0].optionString = customClasspath;
	} else {
		options[0].optionString = CLASSPATH_PREFIX JUBY_JAR_PATH;
	}

	options[1].optionString = "-Djava.compiler=NONE";

	JavaVMInitArgs jvm_args;
	jvm_args.version            = JNI_VERSION_1_2;
	jvm_args.options            = options;
	jvm_args.nOptions           = 2;
	jvm_args.ignoreUnrecognized = JNI_FALSE; 
	
	JNIEnv *env;
	
	JNI_CreateJavaVM( &jvm ,(void**)&env, &jvm_args ); 
	
	if ( customClasspath ) {
		free( customClasspath );
	}
	
	DEBUG_EXIT( "setUpJVM(...)" );
	return env;
}

void setUpJavaReflection(JNIEnv *env) {
	setUpJavaPrimitives( env );
	
	setUpJavaClassClass( env );
	
	setUpJavaJubyClass(  env );
	setUpJubyInstance( env );
	
	setUpJavaValueClass( env );
}

void setUpJavaPrimitives(JNIEnv *env) {
	setUpJavaPrimitiveClass( env, &JAVA_STRING, "java/lang/String",   0,              0 );
	setUpJavaPrimitiveClass( env, &JAVA_BOOLEAN, "java/lang/Boolean", "booleanValue", "()Z" );
	setUpJavaPrimitiveClass( env, &JAVA_SHORT,   "java/lang/Short",   "shortValue",   "()S" );
	setUpJavaPrimitiveClass( env, &JAVA_INTEGER, "java/lang/Integer", "intValue",     "()I" );
	setUpJavaPrimitiveClass( env, &JAVA_LONG,    "java/lang/Long",    "longValue",    "()J" );
	setUpJavaPrimitiveClass( env, &JAVA_FLOAT,   "java/lang/Float",   "floatValue",   "()F" );
	setUpJavaPrimitiveClass( env, &JAVA_DOUBLE,  "java/lang/Double",  "doubleValue",  "()D" );	
	
}

void setUpJavaClassClass(JNIEnv *env) {
	CLASS_CLASS = (*env)->NewGlobalRef( env, (*env)->FindClass( env, "java/lang/Class" ) );
	checkException( env );
	
	CLASS_GETNAME_METHOD = (*env)->GetMethodID( env, CLASS_CLASS, "getName", "()Ljava/lang/String;" );
	checkException( env );
	
	CLASS_GETSUPERCLASS_METHOD = (*env)->GetMethodID( env, CLASS_CLASS, "getSuperclass", "()Ljava/lang/Class;" );
	checkException( env );
	
	CLASS_GETINTERFACES_METHOD = (*env)->GetMethodID( env, CLASS_CLASS, "getInterfaces", "()[Ljava/lang/Class;" );
	checkException( env );
}

void setUpJavaJubyClass(JNIEnv* env) {
	JUBY_CLASS = (*env)->NewGlobalRef( env, (*env)->FindClass( env, "org/rubyhaus/juby/Juby" ) );
	checkException( env );

	JUBY_CONSTRUCTOR           = (*env)->GetMethodID( env, JUBY_CLASS, "<init>",         "()V" );
	checkException( env );
	
	JUBY_BRIDGE_METHOD         = (*env)->GetMethodID( env, JUBY_CLASS, "bridge",     "(Ljava/lang/Object;Ljava/lang/String;[Lorg/rubyhaus/juby/Value;)Ljava/lang/Object;" );
	checkException( env );
	
	JUBY_GETCLASS_METHOD       = (*env)->GetMethodID( env, JUBY_CLASS, "getClass",       "(Ljava/lang/String;)Ljava/lang/Class;" );
	checkException( env );
	
	JUBY_NEWINSTANCE_METHOD    = (*env)->GetMethodID( env, JUBY_CLASS, "newInstance",     "(Ljava/lang/Class;[Lorg/rubyhaus/juby/Value;)Ljava/lang/Object;" );
	checkException( env );
	
	JUBY_OBJECTTOS_METHOD      = (*env)->GetMethodID( env, JUBY_CLASS, "objectToS",      "(Ljava/lang/Object;)Ljava/lang/String;" );
	checkException( env );
}

void setUpJubyInstance(JNIEnv *env) {
	JUBY_INSTANCE = (*env)->NewGlobalRef( env, (*env)->NewObject( env, JUBY_CLASS, JUBY_CONSTRUCTOR ) );
	checkException( env );
}

void setUpJavaValueClass(JNIEnv *env) {
	VALUE_CLASS = (*env)->NewGlobalRef( env, (*env)->FindClass( env, "org/rubyhaus/juby/Value" ) );
	checkException( env );

	VALUE_CONSTRUCTOR = (*env)->GetMethodID( env, VALUE_CLASS, "<init>", "(J)V" );
	checkException( env );
	
	VALUE_GETVALUE_METHOD = (*env)->GetMethodID( env, VALUE_CLASS, "getValue", "()J" );
	checkException( env );
}

void registerNativeMethods(JNIEnv *env) {

	JNINativeMethod valueNativeMethods[6];

	valueNativeMethods[0].name = "callMethod";
	valueNativeMethods[0].signature = "(Ljava/lang/String;[Ljava/lang/Object;)Ljava/lang/Object;";
	valueNativeMethods[0].fnPtr = Java_org_rubyhaus_juby_Value_callMethod;

	valueNativeMethods[1].name = "callStringMethod";
	valueNativeMethods[1].signature = "(Ljava/lang/String;[Ljava/lang/Object;)Ljava/lang/String;";
	valueNativeMethods[1].fnPtr = Java_org_rubyhaus_juby_Value_callStringMethod;

	valueNativeMethods[2].name = "callIntegerMethod";
	valueNativeMethods[2].signature = "(Ljava/lang/String;[Ljava/lang/Object;)I";
	valueNativeMethods[2].fnPtr = Java_org_rubyhaus_juby_Value_callIntegerMethod;

	valueNativeMethods[3].name = "callBooleanMethod";
	valueNativeMethods[3].signature = "(Ljava/lang/String;[Ljava/lang/Object;)Z";
	valueNativeMethods[3].fnPtr = Java_org_rubyhaus_juby_Value_callBooleanMethod;

	valueNativeMethods[4].name = "getRubyTypeCode";
	valueNativeMethods[4].signature = "()I";
	valueNativeMethods[4].fnPtr = Java_org_rubyhaus_juby_Value_getRubyTypeCode;

	valueNativeMethods[5].name = "unwrapJavaObject";
	valueNativeMethods[5].signature = "()Ljava/lang/Object;";
	valueNativeMethods[5].fnPtr = Java_org_rubyhaus_juby_Value_unwrapJavaObject;

	(*env)->RegisterNatives( env, VALUE_CLASS, valueNativeMethods, 6 );
	
	checkException( env );
}

void setUpJavaPrimitiveClass(JNIEnv *env, JavaPrimitiveClass *primitiveHolder, const char *classSpec, const char *name, const char *signature)	{ 
	DEBUG_ENTER( "setUpJavaPrimitiveClass(...)" );
	
	primitiveHolder->javaClass = (*env)->FindClass( env, classSpec );  
	checkException( env );
	
	if ( name && signature ) {
		primitiveHolder->primitiveExtractor = (*env)->GetMethodID( env, primitiveHolder->javaClass, name, signature ); 
		checkException( env );
	}
	
	DEBUG_EXIT( "setUpJavaPrimitiveClass(...)" );
}

JNIEnv *attachJNIEnv() {
	DEBUG_ENTER( "attachJNIEnv()" );
	JNIEnv *env;
	
	if ( (*jvm)->AttachCurrentThread( jvm, (void**) &env, NULL ) ) {
		printf( "unable to retrieve JVM for thread\n" );
		DEBUG_EXIT( "attachJNIEnv()" );
		return 0;
	}
	
	DEBUG_EXIT( "attachJNIEnv()" );
	return env;
}

void detachJNIEnv() {
	(*jvm)->DetachCurrentThread( jvm );	
}

jboolean extractBoolean(JNIEnv *env, jobject object) {
	jboolean result = (*env)->CallBooleanMethod( env, object, JAVA_BOOLEAN.primitiveExtractor );
	checkException( env );
	return result;
}

jshort extractShort(JNIEnv *env, jobject object) {
	jshort result = (*env)->CallShortMethod( env, object, JAVA_SHORT.primitiveExtractor );	
	checkException( env );
	return result;
}

jint extractInteger(JNIEnv *env, jobject object) {
	jint result = (*env)->CallIntMethod( env, object, JAVA_INTEGER.primitiveExtractor );	
	checkException( env );
	return result;
}

jlong extractLong(JNIEnv *env, jobject object) {
	jlong result = (*env)->CallLongMethod( env, object, JAVA_LONG.primitiveExtractor );	
	checkException( env );
	return result;
}

jdouble extractDouble(JNIEnv *env, jobject object) {
	jdouble result = (*env)->CallDoubleMethod( env, object, JAVA_DOUBLE.primitiveExtractor );	
	checkException( env );
	return result;
}

jfloat extractFloat(JNIEnv *env, jobject object) {
	jfloat result = (*env)->CallFloatMethod( env, object, JAVA_FLOAT.primitiveExtractor );	
	checkException( env );
	return result;
}

jboolean isInstance(JNIEnv *env, jobject javaObject, JavaPrimitiveClass *primitiveHolder) {
	DEBUG_ENTER( "isInstance(...)" );
	
	jboolean result = (*env)->IsInstanceOf( env, javaObject, primitiveHolder->javaClass );
	checkException( env );
	
	DEBUG_EXIT( "isInstance(...)" );
	return result;
}

int checkException(JNIEnv *env) {
	if ( (*env)->ExceptionOccurred( env ) ) {
		(*env)->ExceptionDescribe( env );
		return 1;
	}
	
	return 0;
}

void dumpJavaClass(JNIEnv *env, jclass javaClass) {
	jstring name = (*env)->CallObjectMethod( env, javaClass, CLASS_GETNAME_METHOD );
	checkException( env );
	
	const char *nameChars = (*env)->GetStringUTFChars( env, name, JNI_FALSE );
	checkException( env );
	
	printf( "[class: %s]\n", nameChars );
	
	(*env)->ReleaseStringUTFChars( env, name, nameChars );
	checkException( env );
}
