#ifndef _LUA_PARSER_H_
#define _LUA_PARSER_H_

#include <stdbool.h>

#include "ruby.h"
#include "ruby/st.h"
#include "ruby/encoding.h"

#define FORCE_UTF8(obj) rb_enc_associate((obj), rb_utf8_encoding())

typedef unsigned long UTF32;  /* at least 32 bits */
typedef unsigned short UTF16; /* at least 16 bits */
typedef unsigned char UTF8;   /* typically 8 bits */


#define UNI_REPLACEMENT_CHAR (UTF32)0x0000FFFD
#define UNI_SUR_HIGH_START  (UTF32)0xD800
#define UNI_SUR_HIGH_END    (UTF32)0xDBFF
#define UNI_SUR_LOW_START   (UTF32)0xDC00
#define UNI_SUR_LOW_END     (UTF32)0xDFFF


typedef struct Lua_ParserStruct {
	VALUE Vsource;
	char *source;
	long len;
	char *memo;
	int max_nesting;
} Lua_Parser;

static void d(VALUE v);

static VALUE cParser_parse(VALUE self, VALUE source);
static char *LuaTable_parse_integer(Lua_Parser *lua, char *p, char *pe, VALUE *result);
static char *LuaTabble_parse_float(Lua_Parser *lua, char *p, char *pe, VALUE *result);
static char *LuaTable_parse_string(Lua_Parser *lua, char *p, char *pe, VALUE *result);
static char* LuaTable_parse_value(Lua_Parser *lua, char *p, char *pe, VALUE *result, int current_nesting);

static char *LuaTable_parse_assignation(Lua_Parser *lua, char *p, char *pe, VALUE *context, int current_nesting);
static char *LuaTable_parse_name(Lua_Parser *lua, char *p, char *pe, VALUE *result);
static char *LuaTable_parse_variable_name(Lua_Parser *lua, char *p, char *pe, VALUE *result);
static char *LuaTable_parse_key_name(Lua_Parser *lua, char *p, char *pe, VALUE *result);
static char *LuaTable_parse_table(Lua_Parser *lua, char *p, char *pe, VALUE *result, int current_nesting);
static char *LuaTable_parse_element(Lua_Parser *lua, char *p, char *pe, VALUE *table, long *pIndex, int current_nesting);

static int convert_UTF32_to_UTF8(char *buf, UTF32 ch);
static UTF32 unescape_unicode(const unsigned char *p);

static VALUE convert_encoding(VALUE source);

static VALUE LuaTable_string_unescape(VALUE result, char *string, char *stringEnd);
#endif