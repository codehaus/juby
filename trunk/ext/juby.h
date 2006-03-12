
#ifndef JUBY_H
#define JUBY_H   

#undef JUBY_EXTERN
#if defined _WIN32 && !defined __GNUC__
# ifndef JUBY_EXPORT
#  define JUBY_EXTERN extern __declspec(dllimport)
# endif
#endif

#ifndef JUBY_EXTERN
#define JUBY_EXTERN extern
#endif     

#include <ruby.h>
#include <jni.h>

VALUE extract_ruby_object(JNIEnv *env, jobject javaObject);
VALUE coerce_to_ruby_type(JNIEnv *env, jobject javaObject);
VALUE wrap_with_ruby_class(JNIEnv *env, jobject javaObject);

jobject wrap_for_java(JNIEnv *env, VALUE rubyObject);

void object_free(void *p);
void mixin(JNIEnv *env, VALUE self, jclass javaClass); 
void Init_juby();


#define JUBY_MIXIN_PREFIX "Juby_"

#define JUBY_DEBUG 0

#define DEBUG_MSG(msg) { if ( JUBY_DEBUG ) { printf( msg ); } }

#define DEBUG_ENTER(func) { if ( JUBY_DEBUG ) { printf( "debug: enter %s\n", func ); } }
#define DEBUG_EXIT(func)  { if ( JUBY_DEBUG ) { printf( "debug: exit %s\n", func ); } }

#endif /* JUBY_H */
