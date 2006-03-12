
module Java

  def self.initialize_vm(classpath=nil)
    Java::juby_initialize_vm( classpath )
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
    def method_missing(sym, *args)
      bridge( sym.to_s, args )
    end
    def [](sym)
      bridge( sym.to_s, [] )
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
