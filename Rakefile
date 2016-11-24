require "bundler/gem_tasks"
require "rspec/core/rake_task"
require "rake/extensiontask"

EXT_ROOT_DIR      = 'ext'
EXT_PARSER_DIR    = "#{EXT_ROOT_DIR}/luaparser"

RAGEL_PATH        = "#{EXT_PARSER_DIR}/luaparser.rl"
EXT_PARSER_SRC    = "#{EXT_PARSER_DIR}/luaparser.c"


spec = Gem::Specification.load('luatable.gemspec')
Rake::ExtensionTask.new("luaparser", spec)

RSpec::Core::RakeTask.new(:spec)
task :default => :spec

task :ragel => EXT_PARSER_SRC

file EXT_PARSER_SRC  => RAGEL_PATH do
	sh "ragel #{RAGEL_PATH} -G2 -o #{EXT_PARSER_SRC}"
end



desc "Generate diagrams of ragel parser (png)"
task :ragel_dot_png do
	root = 'diagrams'
	ragel_specs = []
	File.new(RAGEL_PATH).grep(/^\s*machine\s*(\S+);\s*$/) { ragel_specs << $1 }
	for s in ragel_specs
		sh "ragel #{RAGEL_PATH} -S#{s} -p -V | dot -Tpng -o#{root}/#{s}.png"
	end
end

namespace :test do
  desc "run test suite under gdb"
  task :gdb => :compile do
    system "lldb  -- ruby  -Ilib boot.rb"
  end
end