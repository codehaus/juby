#!/bin/sh 

VERSION=1.5

#ruby ./ext/extconf.rb --with-jdk-include=/System/Library/Frameworks/JavaVM.framework/Versions/$VERSION/Headers/ --with-jdk-lib=/System/Library/Frameworks/JavaVM.framework/Versions/$VERSION/Libraries    
ruby ./ext/extconf.rb --with-jdk-include=/System/Library/Frameworks/JavaVM.framework/Headers/ --with-jdk-lib=/System/Library/Frameworks/JavaVM.framework/Libraries    
