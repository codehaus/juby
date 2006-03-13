
require 'rubygems'

spec = Gem::Specification.new do | s | 
  s.name       = "Juby"
  s.version    = "1.0.0"
  s.author     = "The Juby Project"
  s.email      = "dev@juby.rubyhaus.org"
  s.homepage   = "http://juby.rubyhaus.org"
  s.platform   = Gem::Platform::RUBY
  s.summary    = "A Ruby/Java Binding"
  s.files      = [
    'README',
    'ext/extconf.rb',
    'ext/depend',
    'ext/juby.h',
    'ext/juby.c',
    'ext/juby_rni.h',
    'ext/juby_rni.c',
    'ext/rniutil.h',
    'ext/rniutil.c',
    'ext/juby_jni.c',
    'ext/jniutil.h',
    'ext/jniutil.c',
    'ext/lib/juby/juby.rb',
    'java/Makefile',
    'java/extconf.rb',
    'java/org/rubyhaus/juby/Juby.java',
    'java/org/rubyhaus/juby/Ary.java',
    'java/org/rubyhaus/juby/Value.java',
    'java/org/rubyhaus/juby/Accessor.java',
    'java/org/rubyhaus/juby/JavaBeanPropertyAccessor.java',
    'java/org/rubyhaus/juby/GetWithArgAccessor.java',
    'java/org/rubyhaus/juby/FieldAccessor.java',
    'java/org/rubyhaus/juby/NoArgMethodAccessor.java',
  ]
  s.extensions = [ 'java/extconf.rb', 'ext/extconf.rb' ]
  s.test_files   = [ 
    'tests/tc_misc.rb'
  ]
end

if $0 == __FILE__
  Gem::manage_gems
  Gem::Builder.new( spec ).build
end
