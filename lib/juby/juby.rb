
module Java

  def self.initialize_vm(classpath=nil)
    Java::juby_initialize_vm( "bob.jar" )
  end

  def self.method_missing(sym)
    return Package.new( sym )
  end

  class Package
    def initialize(package_name)
      @package_name = package_name
    end
    def method_missing(segment)
      fq = "#{@package_name}.#{segment}"
      java_class = Java::juby_get_class( fq )
      if ( java_class ) 
        return java_class
      else
        return Package.new( fq )
      end
    end
  end

  class Object
    def method_missing(name,*args)
      if ( args.size == 0 ) 
        access_property( name.to_s )
      else
        call_method( name.to_s, args )
      end
    end
    def [](arg)
      access_property( arg.to_s )
    end
  end

  class Class < Object
    def new(*args) 
      new_instance( args )
    end
  end

end

module Juby_java_util_Map
  def each(&block)
    entrySet.each{|entry|
      block.call(entry.key,entry.value)
    }
  end
end

module Juby_java_util_Collection
  def each(&block)
    iter = iterator
    while ( iter.hasNext )
      entry = iter.next 
      block.call( entry );
    end
  end
end

module Juby_java_util_Properties
  def [](key, value=nil)
    if ( value != nil )
      setProperty( key, value )
    end
    getProperty( key )
  end
end
