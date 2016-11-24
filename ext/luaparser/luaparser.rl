#include "luaparser.h"


#define EXC_ENCODING rb_utf8_encoding()

static VALUE mLuaTable;
static VALUE CNaN;
static VALUE eParserError, eNestingError;

/* unicode */

static const char digit_values[256] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1,
    -1, -1, -1, -1, -1, -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1
};

static UTF32 unescape_unicode(const unsigned char *p)
{
    char b;
    UTF32 result = 0;
    b = digit_values[p[0]];
    if (b < 0) return UNI_REPLACEMENT_CHAR;
    result = (result << 4) | (unsigned char)b;
    b = digit_values[p[1]];
    if (b < 0) return UNI_REPLACEMENT_CHAR;
    result = (result << 4) | (unsigned char)b;
    b = digit_values[p[2]];
    if (b < 0) return UNI_REPLACEMENT_CHAR;
    result = (result << 4) | (unsigned char)b;
    b = digit_values[p[3]];
    if (b < 0) return UNI_REPLACEMENT_CHAR;
    result = (result << 4) | (unsigned char)b;
    return result;
}

static int convert_UTF32_to_UTF8(char *buf, UTF32 ch)
{
    int len = 1;
    if (ch <= 0x7F) {
        buf[0] = (char) ch;
    } else if (ch <= 0x07FF) {
        buf[0] = (char) ((ch >> 6) | 0xC0);
        buf[1] = (char) ((ch & 0x3F) | 0x80);
        len++;
    } else if (ch <= 0xFFFF) {
        buf[0] = (char) ((ch >> 12) | 0xE0);
        buf[1] = (char) (((ch >> 6) & 0x3F) | 0x80);
        buf[2] = (char) ((ch & 0x3F) | 0x80);
        len += 2;
   	} else if (ch <= 0x1fffff) {
        buf[0] =(char) ((ch >> 18) | 0xF0);
        buf[1] =(char) (((ch >> 12) & 0x3F) | 0x80);
        buf[2] =(char) (((ch >> 6) & 0x3F) | 0x80);
        buf[3] =(char) ((ch & 0x3F) | 0x80);
        len += 3;
    } else {
        buf[0] = '?';
    }
    return len;
}

static void d(VALUE v) {
    ID sym_puts = rb_intern("puts");
    ID sym_inspect = rb_intern("inspect");
    rb_funcall(rb_mKernel, sym_puts, 1,
        rb_funcall(v, sym_inspect, 0));
}

void Init_luaparser(void)
{
	mLuaTable = rb_define_module("LuaTable");
	rb_define_singleton_method(mLuaTable, "parse", cParser_parse, 1);
	CNaN = rb_const_get(mLuaTable, rb_intern("NaN"));
	eParserError = rb_path2class("LuaTable::ParserError");
	eNestingError = rb_path2class("LuaTable::NestingError");
}
%%{
	machine LuaTable_common;

	cr = '\n';
	cr_neg = ^[\n];
	ws =[ \t\r\n];
	value_separator = ',';

	comment = '--' cr_neg* cr;
	ignore = ws | comment;
	begin_number = digit | '-';
	Vnil = 'nil';
	Vfalse = 'false';
	Vtrue = 'true';
	VNaN = 'NaN';
	begin_string = '"';
	begin_variable_name = alpha | '_';
	begin_key = "[";
	begin_name = begin_key | begin_variable_name;
	begin_assignation = begin_name;
	valid_variable_character = alnum | '_';
    begin_table = '{';
    operator = '=';
    separator = ws | operator | value_separator;
    string_character = ^[\"\\] | '\\'[\"\\/bfnrt] | '\\u'[0-9a-fA-F]{4};

    begin_value = [Nnft\"\-{] | digit;

    # common actions
    action exit { fhold; fbreak; }
}%%

// "

%%{
	machine LuaTable_value;
	include LuaTable_common;

	write data;

	action parse_nil {
		*result = Qnil;
	}

	action parse_false {
		*result = Qfalse;
	}

	action parse_true {
		*result = Qtrue;
	}

	action parse_nan {
		*result = CNaN;
	}

	action parse_number {
        char *np;
        bool number_found = false;
        np = LuaTabble_parse_float(lua, fpc, pe, result);
        if(np != NULL) { fexec np; number_found = true; }
        else {

		  np = LuaTable_parse_integer(lua, fpc, pe, result);

		  if(np != NULL) { fexec np; number_found = true; }
        }
		if(!number_found) { fhold; fbreak; }
	}

	action parse_string {
        char *np = LuaTable_parse_string(lua, fpc, pe, result);
        if (np == NULL){
        	fhold; fbreak;
        }
        else {
        	fexec np;
        }
    }

    action parse_table {
        char *np = LuaTable_parse_table(lua, fpc, pe, result, current_nesting + 1);
        if(np != NULL) { fexec np; }
        else { fhold; fbreak; }
    }

	main := ignore*  (  Vnil @parse_nil |
						Vtrue @parse_true |
						Vfalse @parse_false |
						VNaN @parse_nan |
						begin_number >parse_number |
						begin_string >parse_string |
                        begin_table >parse_table
						) ignore* %*exit;
}%%


static char* LuaTable_parse_value(Lua_Parser *lua, char *p, char *pe, VALUE *result, int current_nesting) {
	int cs;

	%% write init;
	%% write exec;

	if (cs >= LuaTable_value_first_final) {
        return p;
    } else {
        return NULL;
    }
}

%%{
    machine LuaTable_table;
    include LuaTable_common;

    write data;

    action parse_element {
        VALUE key_name = LONG2FIX(index);
        VALUE v = Qnil;
        char *np;

        np = LuaTable_parse_element(lua, fpc, pe, result, &index, current_nesting);

        if (np == NULL) {
            fhold; fbreak;
        } else {
            fexec np;
        }

    }

    main := '{' ignore* ((begin_value|begin_key) >parse_element ignore* ',' ignore*)* ignore* '}' @exit;

}%%

static char *LuaTable_parse_table(Lua_Parser *lua, char *p, char *pe, VALUE *result, int current_nesting) {
    int cs;

    *result = rb_hash_new();
    long index = 1;

    %%write init;
    %%write exec;


    if (cs >= LuaTable_table_first_final) {
        return p + 1;   
    } else {
        rb_enc_raise(rb_utf8_encoding(), eParserError, "%u: unexpected token at '%s'", __LINE__, p);
        return NULL;
    }
}

%%{
    machine LuaTable_element;
    include LuaTable_common;

    write data;


     action parse_value {
        VALUE key_name = LONG2FIX(*pIndex);
        char *np;

        np = LuaTable_parse_value(lua, fpc, pe, &v, current_nesting);

        if (np == NULL) {
            fhold; fbreak;
        } else {
            rb_hash_aset(*table, key_name, v);
            (*pIndex)++;
            fexec np;
        }

    }

    action parse_assignation {

        np = LuaTable_parse_assignation(lua, fpc, pe, table, current_nesting);

        if (np == NULL) {
            fhold; fbreak;
        } else {
            fexec np;
        }
    }

    main = ignore* ( begin_value > parse_value |
             begin_key  > parse_assignation ) ignore*
             %*exit;
}%%

static char *LuaTable_parse_element(Lua_Parser *lua, char *p, char *pe, VALUE *table, long *pIndex, int current_nesting)
{
    int cs;
    VALUE v = Qnil;
    char *np;

    %%write init;
    %%write exec;

    if(cs >= LuaTable_element_first_final) {
        return p;
    } else {
        return NULL;
    }
}



%%{
	machine LuaTable_assignation;
	include LuaTable_common;

	write data;

    action parse_value {
        VALUE v = Qnil;
        char *np = LuaTable_parse_value(lua, fpc, pe, &v, current_nesting);
        if (np == NULL) {
            fhold; fbreak;
        } else {
            rb_hash_aset(*context, key_name, v);
            fexec np;
        }
    }

	action parse_name {
		char *np;

        np = LuaTable_parse_name(lua, fpc, pe, &key_name);
        if (np == NULL) { fhold; fbreak; } else fexec np;
	}

	main := begin_name >parse_name ignore* '=' ignore* begin_value >parse_value @exit; 
}%%



static char *LuaTable_parse_assignation(Lua_Parser *lua, char *p, char *pe, VALUE *context, int current_nesting)
{
    int cs;
    VALUE key_name = Qnil;

    if (lua->max_nesting && current_nesting > lua->max_nesting) {
        rb_raise(eNestingError, "nesting of %d is too deep", current_nesting);
    }

    %% write init;
    %% write exec;

    if (cs >= LuaTable_assignation_first_final) {
        return p + 1;
    } else {
        return NULL;
    }
}


%%{
	machine LuaTable_name;
	include LuaTable_common;

	write data;

    action parse_variable_name {
        char *np;
        np = LuaTable_parse_variable_name(lua, fpc, pe, result);
        if (np == NULL) { fhold; fbreak; } else fexec np;
    }

    action parse_key_name {
        char *np;
        np = LuaTable_parse_key_name(lua, fpc, pe, result);
        if (np == NULL) { fhold; fbreak; } else {
            fexec np;
        }
    }

    main := (begin_variable_name >parse_variable_name |
    		begin_key >parse_key_name)
            %*exit;
}%%

static char *LuaTable_parse_name(Lua_Parser *lua, char *p, char *pe, VALUE *result)
{
	int cs;

	*result = Qnil;
	%% write init;
	%% write exec;

	if(cs >= LuaTable_name_first_final) {
		return p;
	}
	else {
		return NULL;
	}
}


%%{
    machine LuaTable_float;
    include LuaTable_common;

    write data;

    main := '-'? (
              (('0' | [1-9][0-9]*) '.' [0-9]+ ([Ee] [+\-]?[0-9]+)?)
              | (('0' | [1-9][0-9]*) ([Ee] [+\-]?[0-9]+))
             )  (^[0-9Ee.\-]? @exit );
}%%


static char *LuaTabble_parse_float(Lua_Parser *lua, char *p, char *pe, VALUE *result)
{
    int cs;

    %% write init;
    lua->memo = p;
    %% write exec;

    if (cs >= LuaTable_float_first_final) {
        long len = p - lua->memo;
        char *buf = malloc(len + 1);
        memcpy(buf, lua->memo, len);
        buf[len] = '\0';
        *result = rb_float_new(rb_cstr_to_dbl(buf, 1));
        return p;
    } else {
        return NULL;
    }
}



%%{
	machine LuaTable_integer;
    include LuaTable_common;

	main := '-' ? ('0' | [1-9][0-9]*) (^[0-9]? @exit);
}%%

%% write data;

static char *LuaTable_parse_integer(Lua_Parser *lua, char *p, char *pe, VALUE *result)
{
	int cs;

	%% write init;
	lua->memo = p;
	%% write exec;

	if(cs > LuaTable_integer_first_final) {
		long len = p - lua->memo;
		char * buf = malloc(len + 1);
		memcpy(buf, lua->memo, len);
		buf[len] = '\0';
		*result =  rb_cstr2inum(buf, 10);
		return p;
	}
	else {
		return NULL;
	}
}

%%{
    machine LuaTable_variable_name;
    include LuaTable_common;

    write data;

    action parse_variable_name {
        *result =  LuaTable_string_unescape(*result, lua->memo, p);
    }

    main := begin_variable_name valid_variable_character* ^valid_variable_character? @exit;
}%%


static char *LuaTable_parse_variable_name(Lua_Parser *lua, char *p, char *pe, VALUE *result)
{
    int cs;

    *result = rb_str_buf_new(0);
    %% write init;
    lua->memo = p;
    %% write exec;


    if (cs >= LuaTable_variable_name_first_final) {
        LuaTable_string_unescape(*result, lua->memo, p);
        if (NIL_P(*result)) {
            return NULL;
        } else {
            FORCE_UTF8(*result);
            return p;
        }
    } 

    return NULL;
}

%%{
    machine LuaTable_key_name;
    include LuaTable_common;

    write data;

    action parse_string {
        char *np = LuaTable_parse_string(lua, fpc, pe, result);

        if (np == NULL) {
            fhold;
            fbreak;
        } else {
            fexec np;
        }
    }

    action parse_integer {
        char *np = LuaTable_parse_integer(lua, fpc, pe, result);

        if(np == NULL) { fhold; fbreak; }
        else { fexec np; }
    }

    main := '[' (begin_string  >parse_string | begin_number >parse_integer) ']' @exit;
}%%

//"'
static char *LuaTable_parse_key_name(Lua_Parser *lua, char *p, char *pe, VALUE *result)
{
    int cs;
    VALUE match_string;

    *result = rb_str_buf_new(0);
    %% write init;
    lua->memo = p;
    %% write exec;

    if (cs >= LuaTable_key_name_first_final) {
        return p + 1;
    } else {
        return NULL;
    }
}


%%{
    machine LuaTable_string;
    include LuaTable_common;

    write data;

    action parse_string {
        *result =  LuaTable_string_unescape(*result, lua->memo + 1, p);
        if (NIL_P(*result)) {
            fhold;
            fbreak;
        } else {
        	FORCE_UTF8(*result);
            fexec p + 1;
        }
    }


    main := '"' string_character* %parse_string '"' @exit;
}%%

//"'
static char *LuaTable_parse_string(Lua_Parser *lua, char *p, char *pe, VALUE *result)
{
    int cs;
    VALUE match_string;

    *result = rb_str_buf_new(0);
    %% write init;
    lua->memo = p;
    %% write exec;

    if (cs >= LuaTable_string_first_final) {
        return p + 1;
    } else {
        return NULL;
    }
}


static VALUE LuaTable_string_unescape(VALUE result, char *string, char *stringEnd)
{
    char *p = string, *pe = string, *unescape;
    int unescape_len;
    char buf[4];

    while (pe < stringEnd) {
        if (*pe == '\\') {
            unescape = (char *) "?";
            unescape_len = 1;
            if (pe > p) rb_str_buf_cat(result, p, pe - p);
            switch (*++pe) {
                case 'n':
                    unescape = (char *) "\n";
                    break;
                case 'r':
                    unescape = (char *) "\r";
                    break;
                case 't':
                    unescape = (char *) "\t";
                    break;
                case '"':
                    unescape = (char *) "\"";
                    break;
                case '\\':
                    unescape = (char *) "\\";
                    break;
                case 'b':
                    unescape = (char *) "\b";
                    break;
                case 'f':
                    unescape = (char *) "\f";
                    break;
                case 'u':
                    if (pe > stringEnd - 4) {
                        return Qnil;
                    } else {
                        UTF32 ch = unescape_unicode((unsigned char *) ++pe);
                        pe += 3;
                        if (UNI_SUR_HIGH_START == (ch & 0xFC00)) {
                            pe++;
                            if (pe > stringEnd - 6) return Qnil;
                            if (pe[0] == '\\' && pe[1] == 'u') {
                                UTF32 sur = unescape_unicode((unsigned char *) pe + 2);
                                ch = (((ch & 0x3F) << 10) | ((((ch >> 6) & 0xF) + 1) << 16)
                                        | (sur & 0x3FF));
                                pe += 5;
                            } else {
                                unescape = (char *) "?";
                                break;
                            }
                        }
                        unescape_len = convert_UTF32_to_UTF8(buf, ch);
                        unescape = buf;
                    }
                    break;

                default:
                    p = pe;
                    continue;
            }
            rb_str_buf_cat(result, unescape, unescape_len);
            p = ++pe;
        } else {
            pe++;
        }
    }
    rb_str_buf_cat(result, p, pe - p);
    return result;
}


static VALUE convert_encoding(VALUE source)
{

  rb_encoding *enc = rb_enc_get(source);
  if (enc == rb_ascii8bit_encoding()) {
    if (OBJ_FROZEN(source)) {
      source = rb_str_dup(source);
    }
    FORCE_UTF8(source);
  } else {
    source = rb_str_conv_enc(source, NULL, rb_utf8_encoding());
  }

  return source;
}


%%{
    machine LuaTable;
    include LuaTable_common;

    write data;

    action parse_assignation {
    	char *np = LuaTable_parse_assignation(lua, fpc, pe, &result, 0);
        if(np == NULL) { fhold; fbreak; } else fexec np;
    }



    main := ignore* (
            begin_assignation >parse_assignation
            ignore*
            )+;
}%%


static VALUE cParser_parse(VALUE self, VALUE source)
{
	source = convert_encoding(StringValue(source));

	Lua_Parser *lua = malloc(sizeof(Lua_Parser));
	lua->source = RSTRING_PTR(source);
	lua->len = RSTRING_LEN(source);
    lua->max_nesting = 100;

	VALUE result = rb_hash_new();

	int cs;
	char *p, *pe;

	%% write init;
	p = lua->source;
	pe = p + lua->len;
	%% write exec;


  if (cs >= LuaTable_first_final && p == pe) {
    return result;
  } else {
    rb_enc_raise(rb_utf8_encoding(), eParserError, "%u: unexpected token at '%s'", __LINE__, p);
    return Qnil;
  }
}