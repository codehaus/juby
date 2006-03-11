
require 'test/unit'

require 'juby'

class TestStatic < Test::Unit::TestCase
  def test_simple
    Java::initialize_vm
    props = Java::java.lang.System.properties   
    assert_not_nil( props[ 'java.version' ] )
    assert_nil( props['something.cheesy.this.way.comes'] )
    puts "java.version #{props['java.version']}"
  end
end
