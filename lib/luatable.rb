require "luatable/version"
module LuaTable
	NaN = 0.0 / 0;

	  # The base exception for JSON errors.
	class LuaTableError < StandardError
		def self.wrap(exception)
		  obj = new("Wrapped(#{exception.class}): #{exception.message.inspect}")
		  obj.set_backtrace exception.backtrace
		  obj
		end
	end

	# This exception is raised if a parser error occurs.
	class ParserError < LuaTableError; end
	class NestingError < LuaTableError; end
end


require "luaparser"