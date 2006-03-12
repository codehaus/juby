
require 'mkmf'

Logging::logfile( 'extconf.log' )

case RUBY_PLATFORM
  when /darwin/
    $DLDFLAGS='-framework JavaVM'
    $LIBPATH << '/System/Library/Frameworks/JavaVM.framework/Libraries'
    $CPPFLAGS << ' -I"/System/Library/Frameworks/JavaVM.framework/Headers/"'
  when /linux/
    $CPPFLAGS << " -I\"#{ENV['JAVA_HOME']}/include\" -I\"#{ENV['JAVA_HOME']}/include/linux\""
		# TODO: for i386 the lib path should be /jre/lib/i386/server
    $LIBPATH << "#{ENV['JAVA_HOME']}/jre/lib/amd64/server"
		$LIBS << " -ljvm"
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
