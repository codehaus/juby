
require 'mkmf'

Logging::logfile( 'extconf.log' )

is_macosx = (/darwin/ =~ RUBY_PLATFORM)

if ( is_macosx ) 
	$DLDFLAGS='-framework JavaVM'
	$LIBPATH << '/System/Library/Frameworks/JavaVM.framework/Libraries'
	$CPPFLAGS << '-I"/System/Library/Frameworks/JavaVM.framework/Headers/"'
end

create_makefile( 'juby' )
