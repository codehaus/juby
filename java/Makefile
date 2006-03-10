

juby.jar: ./org/rubyhaus/juby/Juby.java ./org/rubyhaus/juby/Value.java ./org/rubyhaus/juby/Ary.java
	javac -source 1.2 -sourcepath ./ ./org/rubyhaus/juby/*.java
	jar cvf ./juby.jar org/rubyhaus/juby/*.class


clean: javaclean 

javaclean:
	rm -f ./juby.jar
	rm -f ./org/rubyhaus/juby/*.class
