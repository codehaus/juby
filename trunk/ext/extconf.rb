
require 'mkmf'

Logging::logfile( 'extconf.log' )

is_macosx = (/darwin/ =~ RUBY_PLATFORM)

if ( is_macosx ) 
	$DLDFLAGS='-framework JavaVM'
	$LIBPATH << '/System/Library/Frameworks/JavaVM.framework/Libraries'
	$CPPFLAGS << ' -I"/System/Library/Frameworks/JavaVM.framework/Headers/"'
end

#topdir = arg_config( '--topdir' )
#juby_jar_path = File.dirname( topdir ) + File::SEPARATOR + "java" + File::SEPARATOR + "juby.jar"

install_dir = arg_config( '--install-dir' )
juby_jar_path = install_dir + File::SEPARATOR + "gems" + File::SEPARATOR + "Juby-1.0.0" + File::SEPARATOR + "java" + File::SEPARATOR + "juby.jar"

puts "========================="
puts "juby_jar_path is #{juby_jar_path}"
puts "========================="


$defs << "-DJUBY_JAR_PATH=\\\"#{juby_jar_path}\\\""

create_makefile( 'juby' )
