

RUBYARCHDIR   = $(sitearchdir)$(target_prefix)                                                                                                                                                                                                 
MAKEDIRS = $(RUBY) -run -e mkdir -- -p
INSTALL_PROG = $(RUBY) -run -e install -- -vpm 0755

juby.jar: ./org/rubyhaus/juby/Juby.java ./org/rubyhaus/juby/Value.java ./org/rubyhaus/juby/Ary.java ./org/rubyhaus/juby/Accessor.java ./org/rubyhaus/juby/JavaBeanPropertyAccessor.java ./org/rubyhaus/juby/GetWithArgAccessor.java ./org/rubyhaus/juby/FieldAccessor.java ./org/rubyhaus/juby/NoArgMethodAccessor.java
	javac -source 1.2 -sourcepath ./ ./org/rubyhaus/juby/*.java
	jar cvf ./juby.jar org/rubyhaus/juby/*.class

install:

clean: javaclean 

javaclean:
	rm -f ./juby.jar
	rm -f ./org/rubyhaus/juby/*.class
