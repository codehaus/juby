
#ifndef JUBY_RNI_H
#define JUBY_RNI_H

#include <ruby.h>

void setUpRNI();

JUBY_EXTERN VALUE mJava;
JUBY_EXTERN VALUE cJavaPackage;
JUBY_EXTERN VALUE cJavaClass;
JUBY_EXTERN VALUE cJavaObject;

VALUE juby_initialize_vm(VALUE self, VALUE classpath);
VALUE juby_get_class(VALUE self, VALUE name);
	
VALUE object_access_property(VALUE self, VALUE propertyName);
VALUE object_call_method(VALUE self, VALUE methodName, VALUE args);
VALUE object_to_s(VALUE self);
VALUE class_new_instance(VALUE self, VALUE args);

#endif /* JUBY_RNI_H */
