
require 'pp'

require 'juby'

Java::initialize_vm

props = Java::java.lang.System.properties

#puts props

#puts props.getProperty( "user.dir" )

puts "user.dir = #{props["user.dir"]}"

props = Java::java.util.Properties.new

props["cheese", "gouda"]
props["state",  "melty"]

## does not work yet
props["size",  4]

props.each{|k,v|
  puts "#{k} = #{v}" 
}


file = Java::java.io.File.new( "juby.bundle" )

puts file.toURL
