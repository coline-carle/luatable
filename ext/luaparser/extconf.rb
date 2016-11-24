require 'mkmf'
makefile_config = RbConfig::MAKEFILE_CONFIG

makefile_config["CFLAGS"] << ' -ggdb -g3 -O0' if ENV['DEBUG']

with_cflags(makefile_config['CFLAGS']) { create_makefile("luaparser") }