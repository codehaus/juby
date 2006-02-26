
require 'mkmf'

Logging::logfile( 'extconf.log' )

$DLDFLAGS='-framework JavaVM'
dir_config( 'jdk' )
#find_library( 'jvm', nil )
find_library( 'jvm_compat', nil )
create_makefile( 'juby' )
