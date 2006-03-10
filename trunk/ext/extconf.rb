
require 'mkmf'

Logging::logfile( 'extconf.log' )

is_macosx = (/darwin/ =~ RUBY_PLATFORM)

if ( is_macosx ) 
	$DLDFLAGS='-framework JavaVM'
	$LIBPATH << '/System/Library/Frameworks/JavaVM.framework/Libraries'
	$CPPFLAGS << ' -I"/System/Library/Frameworks/JavaVM.framework/Headers/"'
end

topdir = arg_config( '--topdir' )

puts "========================="
puts "topdir is #{topdir}"
puts "configure_args is #{$configure_args}"
puts "========================="

juby_jar_path = File.dirname( topdir ) + File::SEPARATOR + "java" + File::SEPARATOR + "juby.jar"

$defs << "-DJUBY_JAR_PATH=\\\"#{juby_jar_path}\\\""

create_makefile( 'juby' )
