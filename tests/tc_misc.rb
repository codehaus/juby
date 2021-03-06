
require 'test/unit'

require 'juby'

class TestStatic < Test::Unit::TestCase
  def test_class
    Java::initialize_vm
    jls = Java::java.lang.String
    assert_not_nil( jls );
    assert_equal( "class java.lang.String", jls.to_s )
  end
  def test_java_version_property
    Java::initialize_vm
    props = Java::java.lang.System.properties   
    assert_not_nil( props[ 'java.version' ] )
    assert_nil( props['something.cheesy.this.way.comes'] )
    puts "  ------------------------------------"
    puts "  Your installation of Juby has been"
    puts "  deemed to be sane." 
    puts ""
    puts "  java.version #{props['java.version']}"
    puts "  ------------------------------------"
  end
end
