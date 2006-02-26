#include <ruby.h>
#include <jni.h>


VALUE juby_initialize_vm(VALUE self, VALUE classpath);
VALUE juby_is_class(VALUE self, VALUE name);
VALUE juby_get_class(VALUE self, VALUE name);
VALUE extract_ruby_object(jobject javaObject);
VALUE coerce_to_ruby_type(jobject javaObject);
VALUE wrap_with_ruby_class(jobject javaObject);
VALUE object_access_property(VALUE self, VALUE property);
VALUE object_call_method(VALUE self, VALUE method, VALUE args);
VALUE object_to_s(VALUE self);
VALUE class_new_instance(VALUE self, VALUE args);
void mixin(VALUE self, jclass javaClass); 
void Init_juby();

#define DEBUG 0
#define DOUT(msg) { if (DEBUG) { printf( msg "\n" ); } }
#define CLASSPATH_PREFIX "-Djava.class.path="
#define JUBY_JAR "./juby.jar"
#define JUBY_MIXIN_PREFIX "Juby_"

static JNIEnv* env;
static VALUE mJava;
static VALUE cJavaPackage;
static VALUE cJavaClass;
static VALUE cJavaObject;

typedef struct JavaPrimitiveClass {
	jclass javaClass;
	jmethodID primitiveExtractor;
} JavaPrimitiveClass;

#define setup_java_primitive(var,classSpec,name,signature)	{ \
	var.javaClass = (*env)->FindClass( env, classSpec );  \
	var.primitiveExtractor = (*env)->GetMethodID( env, var.javaClass, name, signature ); \
}

#define is_instance(javaObject,classVar) ( (*env)->IsInstanceOf( env, javaObject, classVar.javaClass ) )

#define extract_boolean(javaObject) ( (*env)->CallBooleanMethod( env, javaObject, JAVA_BOOLEAN.primitiveExtractor ) )
#define extract_short(javaObject)   ( (*env)->CallShortMethod(   env, javaObject, JAVA_SHORT.primitiveExtractor ) )
#define extract_integer(javaObject) ( (*env)->CallIntMethod(     env, javaObject, JAVA_INTEGER.primitiveExtractor ) )
#define extract_long(javaObject)    ( (*env)->CallLongMethod(    env, javaObject, JAVA_LONG.primitiveExtractor ) )
#define extract_float(javaObject)   ( (*env)->CallFloatMethod(   env, javaObject, JAVA_FLOAT.primitiveExtractor ) )
#define extract_double(javaObject)  ( (*env)->CallDoubleMethod(  env, javaObject, JAVA_DOUBLE.primitiveExtractor ) )

static JavaPrimitiveClass JAVA_STRING;
static JavaPrimitiveClass JAVA_BOOLEAN;
static JavaPrimitiveClass JAVA_SHORT;
static JavaPrimitiveClass JAVA_INTEGER;
static JavaPrimitiveClass JAVA_LONG;
static JavaPrimitiveClass JAVA_FLOAT;
static JavaPrimitiveClass JAVA_DOUBLE;

static jclass CLASS_CLASS;
static jmethodID CLASS_GETNAME_METHOD;
static jmethodID CLASS_GETSUPERCLASS_METHOD;
static jmethodID CLASS_GETINTERFACES_METHOD;

static jmethodID BOOLEAN_BOOLEANVALUE_METHOD;

static jclass JUBY_CLASS;

static jmethodID JUBY_ISCLASS_METHOD;
static jmethodID JUBY_GETCLASS_METHOD;
static jmethodID JUBY_CALLMETHOD_METHOD;
static jmethodID JUBY_NEWINSTANCE_METHOD;
static jmethodID JUBY_ACCESSPROPERTY_METHOD;
static jmethodID JUBY_OBJECTTOS_METHOD;

static jclass VALUE_CLASS;
static jmethodID VALUE_CONSTRUCTOR;
static jmethodID VALUE_GETVALUE_METHOD;

