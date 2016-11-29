
#line 1 "ext/luaparser/luaparser.rl"
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

#line 119 "ext/luaparser/luaparser.rl"


// "


#line 99 "ext/luaparser/luaparser.c"
static const int LuaTable_value_start = 1;
static const int LuaTable_value_first_final = 13;
static const int LuaTable_value_error = 0;

static const int LuaTable_value_en_main = 1;


#line 183 "ext/luaparser/luaparser.rl"



static char* LuaTable_parse_value(Lua_Parser *lua, char *p, char *pe, VALUE *result, int current_nesting) {
	int cs;

	
#line 115 "ext/luaparser/luaparser.c"
	{
	cs = LuaTable_value_start;
	}

#line 190 "ext/luaparser/luaparser.rl"
	
#line 122 "ext/luaparser/luaparser.c"
	{
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
case 1:
	switch( (*p) ) {
		case 34: goto tr0;
		case 45: goto tr2;
		case 78: goto st2;
		case 102: goto st4;
		case 110: goto st8;
		case 116: goto st10;
		case 123: goto tr7;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr2;
	goto st0;
st0:
cs = 0;
	goto _out;
tr0:
#line 159 "ext/luaparser/luaparser.rl"
	{
        char *np = LuaTable_parse_string(lua, p, pe, result);
        if (np == NULL){
        	p--; {p++; cs = 13; goto _out;}
        }
        else {
        	{p = (( np))-1;}
        }
    }
	goto st13;
tr2:
#line 145 "ext/luaparser/luaparser.rl"
	{
        char *np;
        bool number_found = false;
        np = LuaTabble_parse_float(lua, p, pe, result);
        if(np != NULL) { {p = (( np))-1;} number_found = true; }
        else {

		  np = LuaTable_parse_integer(lua, p, pe, result);

		  if(np != NULL) { {p = (( np))-1;} number_found = true; }
        }
		if(!number_found) { p--; {p++; cs = 13; goto _out;} }
	}
	goto st13;
tr7:
#line 169 "ext/luaparser/luaparser.rl"
	{
        char *np = LuaTable_parse_table(lua, p, pe, result, current_nesting + 1);
        if(np != NULL) { {p = (( np))-1;} }
        else { p--; {p++; cs = 13; goto _out;} }
    }
	goto st13;
tr9:
#line 141 "ext/luaparser/luaparser.rl"
	{
		*result = CNaN;
	}
	goto st13;
tr13:
#line 133 "ext/luaparser/luaparser.rl"
	{
		*result = Qfalse;
	}
	goto st13;
tr15:
#line 129 "ext/luaparser/luaparser.rl"
	{
		*result = Qnil;
	}
	goto st13;
tr18:
#line 137 "ext/luaparser/luaparser.rl"
	{
		*result = Qtrue;
	}
	goto st13;
st13:
	if ( ++p == pe )
		goto _test_eof13;
case 13:
#line 118 "ext/luaparser/luaparser.rl"
	{ p--; {p++; cs = 13; goto _out;} }
#line 210 "ext/luaparser/luaparser.c"
	goto st0;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
	if ( (*p) == 97 )
		goto st3;
	goto st0;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
	if ( (*p) == 78 )
		goto tr9;
	goto st0;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
	if ( (*p) == 97 )
		goto st5;
	goto st0;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
	if ( (*p) == 108 )
		goto st6;
	goto st0;
st6:
	if ( ++p == pe )
		goto _test_eof6;
case 6:
	if ( (*p) == 115 )
		goto st7;
	goto st0;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
	if ( (*p) == 101 )
		goto tr13;
	goto st0;
st8:
	if ( ++p == pe )
		goto _test_eof8;
case 8:
	if ( (*p) == 105 )
		goto st9;
	goto st0;
st9:
	if ( ++p == pe )
		goto _test_eof9;
case 9:
	if ( (*p) == 108 )
		goto tr15;
	goto st0;
st10:
	if ( ++p == pe )
		goto _test_eof10;
case 10:
	if ( (*p) == 114 )
		goto st11;
	goto st0;
st11:
	if ( ++p == pe )
		goto _test_eof11;
case 11:
	if ( (*p) == 117 )
		goto st12;
	goto st0;
st12:
	if ( ++p == pe )
		goto _test_eof12;
case 12:
	if ( (*p) == 101 )
		goto tr18;
	goto st0;
	}
	_test_eof13: cs = 13; goto _test_eof; 
	_test_eof2: cs = 2; goto _test_eof; 
	_test_eof3: cs = 3; goto _test_eof; 
	_test_eof4: cs = 4; goto _test_eof; 
	_test_eof5: cs = 5; goto _test_eof; 
	_test_eof6: cs = 6; goto _test_eof; 
	_test_eof7: cs = 7; goto _test_eof; 
	_test_eof8: cs = 8; goto _test_eof; 
	_test_eof9: cs = 9; goto _test_eof; 
	_test_eof10: cs = 10; goto _test_eof; 
	_test_eof11: cs = 11; goto _test_eof; 
	_test_eof12: cs = 12; goto _test_eof; 

	_test_eof: {}
	_out: {}
	}

#line 191 "ext/luaparser/luaparser.rl"

	if (cs >= LuaTable_value_first_final) {
        return p;
    } else {
        return NULL;
    }
}


#line 317 "ext/luaparser/luaparser.c"
static const int LuaTable_table_start = 1;
static const int LuaTable_table_first_final = 12;
static const int LuaTable_table_error = 0;

static const int LuaTable_table_en_main = 1;


#line 222 "ext/luaparser/luaparser.rl"


static char *LuaTable_parse_table(Lua_Parser *lua, char *p, char *pe, VALUE *result, int current_nesting) {
    int cs;

    *result = rb_hash_new();
    long index = 1;

    
#line 335 "ext/luaparser/luaparser.c"
	{
	cs = LuaTable_table_start;
	}

#line 231 "ext/luaparser/luaparser.rl"
    
#line 342 "ext/luaparser/luaparser.c"
	{
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
case 1:
	if ( (*p) == 123 )
		goto st2;
	goto st0;
st0:
cs = 0;
	goto _out;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
	switch( (*p) ) {
		case 13: goto st2;
		case 32: goto st2;
		case 34: goto tr2;
		case 45: goto tr3;
		case 78: goto tr2;
		case 91: goto tr2;
		case 102: goto tr2;
		case 110: goto tr2;
		case 116: goto tr2;
		case 123: goto tr2;
		case 125: goto tr4;
	}
	if ( (*p) > 10 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr2;
	} else if ( (*p) >= 9 )
		goto st2;
	goto st0;
tr2:
#line 205 "ext/luaparser/luaparser.rl"
	{
        VALUE key_name = LONG2FIX(index);
        VALUE v = Qnil;
        char *np;

        np = LuaTable_parse_element(lua, p, pe, result, &index, current_nesting);

        if (np == NULL) {
            p--; {p++; cs = 3; goto _out;}
        } else {
            {p = (( np))-1;}
        }

    }
	goto st3;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
#line 399 "ext/luaparser/luaparser.c"
	switch( (*p) ) {
		case 13: goto st3;
		case 32: goto st3;
		case 44: goto st2;
		case 45: goto st4;
	}
	if ( 9 <= (*p) && (*p) <= 10 )
		goto st3;
	goto st0;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
	if ( (*p) == 45 )
		goto st5;
	goto st0;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
	if ( (*p) == 10 )
		goto st3;
	goto st5;
tr3:
#line 205 "ext/luaparser/luaparser.rl"
	{
        VALUE key_name = LONG2FIX(index);
        VALUE v = Qnil;
        char *np;

        np = LuaTable_parse_element(lua, p, pe, result, &index, current_nesting);

        if (np == NULL) {
            p--; {p++; cs = 6; goto _out;}
        } else {
            {p = (( np))-1;}
        }

    }
	goto st6;
st6:
	if ( ++p == pe )
		goto _test_eof6;
case 6:
#line 444 "ext/luaparser/luaparser.c"
	switch( (*p) ) {
		case 13: goto st3;
		case 32: goto st3;
		case 44: goto st2;
		case 45: goto st7;
	}
	if ( 9 <= (*p) && (*p) <= 10 )
		goto st3;
	goto st0;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
	switch( (*p) ) {
		case 10: goto st2;
		case 45: goto st9;
	}
	goto st8;
st8:
	if ( ++p == pe )
		goto _test_eof8;
case 8:
	if ( (*p) == 10 )
		goto st2;
	goto st8;
st9:
	if ( ++p == pe )
		goto _test_eof9;
case 9:
	if ( (*p) == 10 )
		goto st10;
	goto st9;
st10:
	if ( ++p == pe )
		goto _test_eof10;
case 10:
	switch( (*p) ) {
		case 13: goto st10;
		case 32: goto st10;
		case 34: goto tr2;
		case 44: goto st2;
		case 45: goto tr12;
		case 78: goto tr2;
		case 91: goto tr2;
		case 102: goto tr2;
		case 110: goto tr2;
		case 116: goto tr2;
		case 123: goto tr2;
		case 125: goto tr4;
	}
	if ( (*p) > 10 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr2;
	} else if ( (*p) >= 9 )
		goto st10;
	goto st0;
tr12:
#line 205 "ext/luaparser/luaparser.rl"
	{
        VALUE key_name = LONG2FIX(index);
        VALUE v = Qnil;
        char *np;

        np = LuaTable_parse_element(lua, p, pe, result, &index, current_nesting);

        if (np == NULL) {
            p--; {p++; cs = 11; goto _out;}
        } else {
            {p = (( np))-1;}
        }

    }
	goto st11;
st11:
	if ( ++p == pe )
		goto _test_eof11;
case 11:
#line 522 "ext/luaparser/luaparser.c"
	switch( (*p) ) {
		case 13: goto st3;
		case 32: goto st3;
		case 44: goto st2;
		case 45: goto st9;
	}
	if ( 9 <= (*p) && (*p) <= 10 )
		goto st3;
	goto st0;
tr4:
#line 118 "ext/luaparser/luaparser.rl"
	{ p--; {p++; cs = 12; goto _out;} }
	goto st12;
st12:
	if ( ++p == pe )
		goto _test_eof12;
case 12:
#line 540 "ext/luaparser/luaparser.c"
	goto st0;
	}
	_test_eof2: cs = 2; goto _test_eof; 
	_test_eof3: cs = 3; goto _test_eof; 
	_test_eof4: cs = 4; goto _test_eof; 
	_test_eof5: cs = 5; goto _test_eof; 
	_test_eof6: cs = 6; goto _test_eof; 
	_test_eof7: cs = 7; goto _test_eof; 
	_test_eof8: cs = 8; goto _test_eof; 
	_test_eof9: cs = 9; goto _test_eof; 
	_test_eof10: cs = 10; goto _test_eof; 
	_test_eof11: cs = 11; goto _test_eof; 
	_test_eof12: cs = 12; goto _test_eof; 

	_test_eof: {}
	_out: {}
	}

#line 232 "ext/luaparser/luaparser.rl"


    if (cs >= LuaTable_table_first_final) {
        return p + 1;   
    } else {
        rb_enc_raise(rb_utf8_encoding(), eParserError, "%u: unexpected token at '%s'", __LINE__, p);
        return NULL;
    }
}


#line 571 "ext/luaparser/luaparser.c"
static const int LuaTable_element_start = 1;
static const int LuaTable_element_first_final = 7;
static const int LuaTable_element_error = 0;

static const int LuaTable_element_en_main = 1;


#line 277 "ext/luaparser/luaparser.rl"


static char *LuaTable_parse_element(Lua_Parser *lua, char *p, char *pe, VALUE *table, long *pIndex, int current_nesting)
{
    int cs;
    VALUE v = Qnil;
    char *np;

    
#line 589 "ext/luaparser/luaparser.c"
	{
	cs = LuaTable_element_start;
	}

#line 286 "ext/luaparser/luaparser.rl"
    
#line 596 "ext/luaparser/luaparser.c"
	{
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
st1:
	if ( ++p == pe )
		goto _test_eof1;
case 1:
	switch( (*p) ) {
		case 13: goto st1;
		case 32: goto st1;
		case 34: goto tr2;
		case 45: goto tr3;
		case 78: goto tr2;
		case 91: goto tr4;
		case 102: goto tr2;
		case 110: goto tr2;
		case 116: goto tr2;
		case 123: goto tr2;
	}
	if ( (*p) > 10 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr2;
	} else if ( (*p) >= 9 )
		goto st1;
	goto st0;
st0:
cs = 0;
	goto _out;
tr2:
#line 249 "ext/luaparser/luaparser.rl"
	{
        VALUE key_name = LONG2FIX(*pIndex);
        char *np;
        np = LuaTable_parse_value(lua, p, pe, &v, current_nesting);
        if (np == NULL) {
            p--; {p++; cs = 7; goto _out;}
        } else if (np > p) {
            rb_hash_aset(*table, key_name, v);
            (*pIndex)++;
            {p = (( np))-1;}
        }

    }
	goto st7;
tr4:
#line 263 "ext/luaparser/luaparser.rl"
	{

        np = LuaTable_parse_assignation(lua, p, pe, table, current_nesting);

        if (np == NULL) {
            p--; {p++; cs = 7; goto _out;}
        } else {
            {p = (( np))-1;}
        }
    }
	goto st7;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
#line 118 "ext/luaparser/luaparser.rl"
	{ p--; {p++; cs = 7; goto _out;} }
#line 662 "ext/luaparser/luaparser.c"
	switch( (*p) ) {
		case 13: goto st7;
		case 32: goto st7;
		case 45: goto st2;
	}
	if ( 9 <= (*p) && (*p) <= 10 )
		goto st7;
	goto st0;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
	if ( (*p) == 45 )
		goto st3;
	goto st0;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
	if ( (*p) == 10 )
		goto st7;
	goto st3;
tr3:
#line 249 "ext/luaparser/luaparser.rl"
	{
        VALUE key_name = LONG2FIX(*pIndex);
        char *np;
        np = LuaTable_parse_value(lua, p, pe, &v, current_nesting);
        if (np == NULL) {
            p--; {p++; cs = 8; goto _out;}
        } else if (np > p) {
            rb_hash_aset(*table, key_name, v);
            (*pIndex)++;
            {p = (( np))-1;}
        }

    }
	goto st8;
st8:
	if ( ++p == pe )
		goto _test_eof8;
case 8:
#line 118 "ext/luaparser/luaparser.rl"
	{ p--; {p++; cs = 8; goto _out;} }
#line 707 "ext/luaparser/luaparser.c"
	switch( (*p) ) {
		case 13: goto st7;
		case 32: goto st7;
		case 45: goto st4;
	}
	if ( 9 <= (*p) && (*p) <= 10 )
		goto st7;
	goto st0;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
	switch( (*p) ) {
		case 10: goto st1;
		case 45: goto st6;
	}
	goto st5;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
	if ( (*p) == 10 )
		goto st1;
	goto st5;
st6:
	if ( ++p == pe )
		goto _test_eof6;
case 6:
	if ( (*p) == 10 )
		goto st9;
	goto st6;
st9:
	if ( ++p == pe )
		goto _test_eof9;
case 9:
#line 118 "ext/luaparser/luaparser.rl"
	{ p--; {p++; cs = 9; goto _out;} }
#line 745 "ext/luaparser/luaparser.c"
	switch( (*p) ) {
		case 13: goto st9;
		case 32: goto st9;
		case 34: goto tr2;
		case 45: goto tr12;
		case 78: goto tr2;
		case 91: goto tr4;
		case 102: goto tr2;
		case 110: goto tr2;
		case 116: goto tr2;
		case 123: goto tr2;
	}
	if ( (*p) > 10 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr2;
	} else if ( (*p) >= 9 )
		goto st9;
	goto st0;
tr12:
#line 249 "ext/luaparser/luaparser.rl"
	{
        VALUE key_name = LONG2FIX(*pIndex);
        char *np;
        np = LuaTable_parse_value(lua, p, pe, &v, current_nesting);
        if (np == NULL) {
            p--; {p++; cs = 10; goto _out;}
        } else if (np > p) {
            rb_hash_aset(*table, key_name, v);
            (*pIndex)++;
            {p = (( np))-1;}
        }

    }
	goto st10;
st10:
	if ( ++p == pe )
		goto _test_eof10;
case 10:
#line 118 "ext/luaparser/luaparser.rl"
	{ p--; {p++; cs = 10; goto _out;} }
#line 786 "ext/luaparser/luaparser.c"
	switch( (*p) ) {
		case 13: goto st7;
		case 32: goto st7;
		case 45: goto st6;
	}
	if ( 9 <= (*p) && (*p) <= 10 )
		goto st7;
	goto st0;
	}
	_test_eof1: cs = 1; goto _test_eof; 
	_test_eof7: cs = 7; goto _test_eof; 
	_test_eof2: cs = 2; goto _test_eof; 
	_test_eof3: cs = 3; goto _test_eof; 
	_test_eof8: cs = 8; goto _test_eof; 
	_test_eof4: cs = 4; goto _test_eof; 
	_test_eof5: cs = 5; goto _test_eof; 
	_test_eof6: cs = 6; goto _test_eof; 
	_test_eof9: cs = 9; goto _test_eof; 
	_test_eof10: cs = 10; goto _test_eof; 

	_test_eof: {}
	_out: {}
	}

#line 287 "ext/luaparser/luaparser.rl"

    if(cs >= LuaTable_element_first_final) {
        return p;
    } else {
        return NULL;
    }
}




#line 823 "ext/luaparser/luaparser.c"
static const int LuaTable_assignation_start = 1;
static const int LuaTable_assignation_first_final = 7;
static const int LuaTable_assignation_error = 0;

static const int LuaTable_assignation_en_main = 1;


#line 322 "ext/luaparser/luaparser.rl"




static char *LuaTable_parse_assignation(Lua_Parser *lua, char *p, char *pe, VALUE *context, int current_nesting)
{
    int cs;
    VALUE key_name = Qnil;

    if (lua->max_nesting && current_nesting > lua->max_nesting) {
        rb_raise(eNestingError, "nesting of %d is too deep", current_nesting);
    }

    
#line 846 "ext/luaparser/luaparser.c"
	{
	cs = LuaTable_assignation_start;
	}

#line 336 "ext/luaparser/luaparser.rl"
    
#line 853 "ext/luaparser/luaparser.c"
	{
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
case 1:
	if ( (*p) == 95 )
		goto tr0;
	if ( (*p) > 91 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr0;
	} else if ( (*p) >= 65 )
		goto tr0;
	goto st0;
st0:
cs = 0;
	goto _out;
tr0:
#line 314 "ext/luaparser/luaparser.rl"
	{
		char *np;

        np = LuaTable_parse_name(lua, p, pe, &key_name);
        if (np == NULL) { p--; {p++; cs = 2; goto _out;} } else {p = (( np))-1;}
	}
	goto st2;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
#line 884 "ext/luaparser/luaparser.c"
	switch( (*p) ) {
		case 13: goto st2;
		case 32: goto st2;
		case 45: goto st3;
		case 61: goto st5;
	}
	if ( 9 <= (*p) && (*p) <= 10 )
		goto st2;
	goto st0;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
	if ( (*p) == 45 )
		goto st4;
	goto st0;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
	if ( (*p) == 10 )
		goto st2;
	goto st4;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
	switch( (*p) ) {
		case 13: goto st5;
		case 32: goto st5;
		case 34: goto tr6;
		case 45: goto tr7;
		case 78: goto tr6;
		case 102: goto tr6;
		case 110: goto tr6;
		case 116: goto tr6;
		case 123: goto tr6;
	}
	if ( (*p) > 10 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr6;
	} else if ( (*p) >= 9 )
		goto st5;
	goto st0;
tr6:
#line 303 "ext/luaparser/luaparser.rl"
	{
        VALUE v = Qnil;
        char *np = LuaTable_parse_value(lua, p, pe, &v, current_nesting);
        if (np == NULL) {
            p--; {p++; cs = 7; goto _out;}
        } else {
            rb_hash_aset(*context, key_name, v);
            {p = (( np))-1;}
        }
    }
#line 118 "ext/luaparser/luaparser.rl"
	{ p--; {p++; cs = 7; goto _out;} }
	goto st7;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
#line 948 "ext/luaparser/luaparser.c"
	goto st0;
tr7:
#line 303 "ext/luaparser/luaparser.rl"
	{
        VALUE v = Qnil;
        char *np = LuaTable_parse_value(lua, p, pe, &v, current_nesting);
        if (np == NULL) {
            p--; {p++; cs = 8; goto _out;}
        } else {
            rb_hash_aset(*context, key_name, v);
            {p = (( np))-1;}
        }
    }
#line 118 "ext/luaparser/luaparser.rl"
	{ p--; {p++; cs = 8; goto _out;} }
	goto st8;
st8:
	if ( ++p == pe )
		goto _test_eof8;
case 8:
#line 969 "ext/luaparser/luaparser.c"
	if ( (*p) == 45 )
		goto st6;
	goto st0;
st6:
	if ( ++p == pe )
		goto _test_eof6;
case 6:
	if ( (*p) == 10 )
		goto st5;
	goto st6;
	}
	_test_eof2: cs = 2; goto _test_eof; 
	_test_eof3: cs = 3; goto _test_eof; 
	_test_eof4: cs = 4; goto _test_eof; 
	_test_eof5: cs = 5; goto _test_eof; 
	_test_eof7: cs = 7; goto _test_eof; 
	_test_eof8: cs = 8; goto _test_eof; 
	_test_eof6: cs = 6; goto _test_eof; 

	_test_eof: {}
	_out: {}
	}

#line 337 "ext/luaparser/luaparser.rl"

    if (cs >= LuaTable_assignation_first_final) {
        return p + 1;
    } else {
        return NULL;
    }
}



#line 1004 "ext/luaparser/luaparser.c"
static const int LuaTable_name_start = 1;
static const int LuaTable_name_first_final = 2;
static const int LuaTable_name_error = 0;

static const int LuaTable_name_en_main = 1;


#line 369 "ext/luaparser/luaparser.rl"


static char *LuaTable_parse_name(Lua_Parser *lua, char *p, char *pe, VALUE *result)
{
	int cs;

	*result = Qnil;
	
#line 1021 "ext/luaparser/luaparser.c"
	{
	cs = LuaTable_name_start;
	}

#line 377 "ext/luaparser/luaparser.rl"
	
#line 1028 "ext/luaparser/luaparser.c"
	{
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
case 1:
	switch( (*p) ) {
		case 91: goto tr2;
		case 95: goto tr0;
	}
	if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr0;
	} else if ( (*p) >= 65 )
		goto tr0;
	goto st0;
st0:
cs = 0;
	goto _out;
tr0:
#line 352 "ext/luaparser/luaparser.rl"
	{
        char *np;
        np = LuaTable_parse_variable_name(lua, p, pe, result);
        if (np == NULL) { p--; {p++; cs = 2; goto _out;} } else {p = (( np))-1;}
    }
	goto st2;
tr2:
#line 358 "ext/luaparser/luaparser.rl"
	{
        char *np;
        np = LuaTable_parse_key_name(lua, p, pe, result);
        if (np == NULL) { p--; {p++; cs = 2; goto _out;} } else {
            {p = (( np))-1;}
        }
    }
	goto st2;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
#line 118 "ext/luaparser/luaparser.rl"
	{ p--; {p++; cs = 2; goto _out;} }
#line 1072 "ext/luaparser/luaparser.c"
	goto st0;
	}
	_test_eof2: cs = 2; goto _test_eof; 

	_test_eof: {}
	_out: {}
	}

#line 378 "ext/luaparser/luaparser.rl"

	if(cs >= LuaTable_name_first_final) {
		return p;
	}
	else {
		return NULL;
	}
}



#line 1093 "ext/luaparser/luaparser.c"
static const int LuaTable_float_start = 1;
static const int LuaTable_float_first_final = 8;
static const int LuaTable_float_error = 0;

static const int LuaTable_float_en_main = 1;


#line 398 "ext/luaparser/luaparser.rl"



static char *LuaTabble_parse_float(Lua_Parser *lua, char *p, char *pe, VALUE *result)
{
    int cs;

    
#line 1110 "ext/luaparser/luaparser.c"
	{
	cs = LuaTable_float_start;
	}

#line 406 "ext/luaparser/luaparser.rl"
    lua->memo = p;
    
#line 1118 "ext/luaparser/luaparser.c"
	{
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
case 1:
	switch( (*p) ) {
		case 45: goto st2;
		case 48: goto st3;
	}
	if ( 49 <= (*p) && (*p) <= 57 )
		goto st7;
	goto st0;
st0:
cs = 0;
	goto _out;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
	if ( (*p) == 48 )
		goto st3;
	if ( 49 <= (*p) && (*p) <= 57 )
		goto st7;
	goto st0;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
	switch( (*p) ) {
		case 46: goto st4;
		case 69: goto st5;
		case 101: goto st5;
	}
	goto st0;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st8;
	goto st0;
st8:
	if ( ++p == pe )
		goto _test_eof8;
case 8:
	switch( (*p) ) {
		case 69: goto st5;
		case 101: goto st5;
	}
	if ( (*p) > 46 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st8;
	} else if ( (*p) >= 45 )
		goto st0;
	goto tr9;
tr9:
#line 118 "ext/luaparser/luaparser.rl"
	{ p--; {p++; cs = 9; goto _out;} }
	goto st9;
st9:
	if ( ++p == pe )
		goto _test_eof9;
case 9:
#line 1183 "ext/luaparser/luaparser.c"
	goto st0;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
	switch( (*p) ) {
		case 43: goto st6;
		case 45: goto st6;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st10;
	goto st0;
st6:
	if ( ++p == pe )
		goto _test_eof6;
case 6:
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st10;
	goto st0;
st10:
	if ( ++p == pe )
		goto _test_eof10;
case 10:
	switch( (*p) ) {
		case 69: goto st0;
		case 101: goto st0;
	}
	if ( (*p) > 46 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st10;
	} else if ( (*p) >= 45 )
		goto st0;
	goto tr9;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
	switch( (*p) ) {
		case 46: goto st4;
		case 69: goto st5;
		case 101: goto st5;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st7;
	goto st0;
	}
	_test_eof2: cs = 2; goto _test_eof; 
	_test_eof3: cs = 3; goto _test_eof; 
	_test_eof4: cs = 4; goto _test_eof; 
	_test_eof8: cs = 8; goto _test_eof; 
	_test_eof9: cs = 9; goto _test_eof; 
	_test_eof5: cs = 5; goto _test_eof; 
	_test_eof6: cs = 6; goto _test_eof; 
	_test_eof10: cs = 10; goto _test_eof; 
	_test_eof7: cs = 7; goto _test_eof; 

	_test_eof: {}
	_out: {}
	}

#line 408 "ext/luaparser/luaparser.rl"

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




#line 428 "ext/luaparser/luaparser.rl"



#line 1265 "ext/luaparser/luaparser.c"
static const int LuaTable_integer_start = 1;
static const int LuaTable_integer_first_final = 3;
static const int LuaTable_integer_error = 0;

static const int LuaTable_integer_en_main = 1;


#line 431 "ext/luaparser/luaparser.rl"

static char *LuaTable_parse_integer(Lua_Parser *lua, char *p, char *pe, VALUE *result)
{
	int cs;

	
#line 1280 "ext/luaparser/luaparser.c"
	{
	cs = LuaTable_integer_start;
	}

#line 437 "ext/luaparser/luaparser.rl"
	lua->memo = p;
	
#line 1288 "ext/luaparser/luaparser.c"
	{
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
case 1:
	switch( (*p) ) {
		case 45: goto st2;
		case 48: goto st3;
	}
	if ( 49 <= (*p) && (*p) <= 57 )
		goto st5;
	goto st0;
st0:
cs = 0;
	goto _out;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
	if ( (*p) == 48 )
		goto st3;
	if ( 49 <= (*p) && (*p) <= 57 )
		goto st5;
	goto st0;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st0;
	goto tr4;
tr4:
#line 118 "ext/luaparser/luaparser.rl"
	{ p--; {p++; cs = 4; goto _out;} }
	goto st4;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
#line 1329 "ext/luaparser/luaparser.c"
	goto st0;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st5;
	goto tr4;
	}
	_test_eof2: cs = 2; goto _test_eof; 
	_test_eof3: cs = 3; goto _test_eof; 
	_test_eof4: cs = 4; goto _test_eof; 
	_test_eof5: cs = 5; goto _test_eof; 

	_test_eof: {}
	_out: {}
	}

#line 439 "ext/luaparser/luaparser.rl"

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


#line 1364 "ext/luaparser/luaparser.c"
static const int LuaTable_variable_name_start = 1;
static const int LuaTable_variable_name_first_final = 2;
static const int LuaTable_variable_name_error = 0;

static const int LuaTable_variable_name_en_main = 1;


#line 464 "ext/luaparser/luaparser.rl"



static char *LuaTable_parse_variable_name(Lua_Parser *lua, char *p, char *pe, VALUE *result)
{
    int cs;

    *result = rb_str_buf_new(0);
    
#line 1382 "ext/luaparser/luaparser.c"
	{
	cs = LuaTable_variable_name_start;
	}

#line 473 "ext/luaparser/luaparser.rl"
    lua->memo = p;
    
#line 1390 "ext/luaparser/luaparser.c"
	{
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
case 1:
	if ( (*p) == 95 )
		goto st2;
	if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st2;
	} else if ( (*p) >= 65 )
		goto st2;
	goto st0;
st0:
cs = 0;
	goto _out;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
	if ( (*p) == 95 )
		goto st2;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st2;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st2;
	} else
		goto st2;
	goto tr2;
tr2:
#line 118 "ext/luaparser/luaparser.rl"
	{ p--; {p++; cs = 3; goto _out;} }
	goto st3;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
#line 1431 "ext/luaparser/luaparser.c"
	goto st0;
	}
	_test_eof2: cs = 2; goto _test_eof; 
	_test_eof3: cs = 3; goto _test_eof; 

	_test_eof: {}
	_out: {}
	}

#line 475 "ext/luaparser/luaparser.rl"


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


#line 1458 "ext/luaparser/luaparser.c"
static const int LuaTable_key_name_start = 1;
static const int LuaTable_key_name_first_final = 4;
static const int LuaTable_key_name_error = 0;

static const int LuaTable_key_name_en_main = 1;


#line 515 "ext/luaparser/luaparser.rl"


//"'
static char *LuaTable_parse_key_name(Lua_Parser *lua, char *p, char *pe, VALUE *result)
{
    int cs;
    VALUE match_string;

    *result = rb_str_buf_new(0);
    
#line 1477 "ext/luaparser/luaparser.c"
	{
	cs = LuaTable_key_name_start;
	}

#line 525 "ext/luaparser/luaparser.rl"
    lua->memo = p;
    
#line 1485 "ext/luaparser/luaparser.c"
	{
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
case 1:
	if ( (*p) == 91 )
		goto st2;
	goto st0;
st0:
cs = 0;
	goto _out;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
	switch( (*p) ) {
		case 34: goto tr2;
		case 45: goto tr3;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr3;
	goto st0;
tr2:
#line 496 "ext/luaparser/luaparser.rl"
	{
        char *np = LuaTable_parse_string(lua, p, pe, result);

        if (np == NULL) {
            p--;
            {p++; cs = 3; goto _out;}
        } else {
            {p = (( np))-1;}
        }
    }
	goto st3;
tr3:
#line 507 "ext/luaparser/luaparser.rl"
	{
        char *np = LuaTable_parse_integer(lua, p, pe, result);

        if(np == NULL) { p--; {p++; cs = 3; goto _out;} }
        else { {p = (( np))-1;} }
    }
	goto st3;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
#line 1535 "ext/luaparser/luaparser.c"
	if ( (*p) == 93 )
		goto tr4;
	goto st0;
tr4:
#line 118 "ext/luaparser/luaparser.rl"
	{ p--; {p++; cs = 4; goto _out;} }
	goto st4;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
#line 1547 "ext/luaparser/luaparser.c"
	goto st0;
	}
	_test_eof2: cs = 2; goto _test_eof; 
	_test_eof3: cs = 3; goto _test_eof; 
	_test_eof4: cs = 4; goto _test_eof; 

	_test_eof: {}
	_out: {}
	}

#line 527 "ext/luaparser/luaparser.rl"

    if (cs >= LuaTable_key_name_first_final) {
        return p + 1;
    } else {
        return NULL;
    }
}



#line 1569 "ext/luaparser/luaparser.c"
static const int LuaTable_string_start = 1;
static const int LuaTable_string_first_final = 8;
static const int LuaTable_string_error = 0;

static const int LuaTable_string_en_main = 1;


#line 555 "ext/luaparser/luaparser.rl"


//"'
static char *LuaTable_parse_string(Lua_Parser *lua, char *p, char *pe, VALUE *result)
{
    int cs;
    VALUE match_string;

    *result = rb_str_buf_new(0);
    
#line 1588 "ext/luaparser/luaparser.c"
	{
	cs = LuaTable_string_start;
	}

#line 565 "ext/luaparser/luaparser.rl"
    lua->memo = p;
    
#line 1596 "ext/luaparser/luaparser.c"
	{
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
case 1:
	if ( (*p) == 34 )
		goto st2;
	goto st0;
st0:
cs = 0;
	goto _out;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
	switch( (*p) ) {
		case 34: goto tr2;
		case 92: goto st3;
	}
	goto st2;
tr2:
#line 542 "ext/luaparser/luaparser.rl"
	{
        *result =  LuaTable_string_unescape(*result, lua->memo + 1, p);
        if (NIL_P(*result)) {
            p--;
            {p++; cs = 8; goto _out;}
        } else {
        	FORCE_UTF8(*result);
            {p = (( p + 1))-1;}
        }
    }
#line 118 "ext/luaparser/luaparser.rl"
	{ p--; {p++; cs = 8; goto _out;} }
	goto st8;
st8:
	if ( ++p == pe )
		goto _test_eof8;
case 8:
#line 1637 "ext/luaparser/luaparser.c"
	goto st0;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
	switch( (*p) ) {
		case 34: goto st2;
		case 47: goto st2;
		case 92: goto st2;
		case 98: goto st2;
		case 102: goto st2;
		case 110: goto st2;
		case 114: goto st2;
		case 116: goto st2;
		case 117: goto st4;
	}
	goto st0;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st5;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st5;
	} else
		goto st5;
	goto st0;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st6;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st6;
	} else
		goto st6;
	goto st0;
st6:
	if ( ++p == pe )
		goto _test_eof6;
case 6:
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st7;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st7;
	} else
		goto st7;
	goto st0;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st2;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st2;
	} else
		goto st2;
	goto st0;
	}
	_test_eof2: cs = 2; goto _test_eof; 
	_test_eof8: cs = 8; goto _test_eof; 
	_test_eof3: cs = 3; goto _test_eof; 
	_test_eof4: cs = 4; goto _test_eof; 
	_test_eof5: cs = 5; goto _test_eof; 
	_test_eof6: cs = 6; goto _test_eof; 
	_test_eof7: cs = 7; goto _test_eof; 

	_test_eof: {}
	_out: {}
	}

#line 567 "ext/luaparser/luaparser.rl"

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



#line 1820 "ext/luaparser/luaparser.c"
static const int LuaTable_start = 1;
static const int LuaTable_first_final = 6;
static const int LuaTable_error = 0;

static const int LuaTable_en_main = 1;


#line 682 "ext/luaparser/luaparser.rl"



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

	
#line 1847 "ext/luaparser/luaparser.c"
	{
	cs = LuaTable_start;
	}

#line 700 "ext/luaparser/luaparser.rl"
	p = lua->source;
	pe = p + lua->len;
	
#line 1856 "ext/luaparser/luaparser.c"
	{
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
st1:
	if ( ++p == pe )
		goto _test_eof1;
case 1:
	switch( (*p) ) {
		case 13: goto st1;
		case 32: goto st1;
		case 45: goto st2;
		case 95: goto tr3;
	}
	if ( (*p) < 65 ) {
		if ( 9 <= (*p) && (*p) <= 10 )
			goto st1;
	} else if ( (*p) > 91 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr3;
	} else
		goto tr3;
	goto st0;
st0:
cs = 0;
	goto _out;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
	if ( (*p) == 45 )
		goto st3;
	goto st0;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
	if ( (*p) == 10 )
		goto st1;
	goto st3;
tr3:
#line 671 "ext/luaparser/luaparser.rl"
	{
    	char *np = LuaTable_parse_assignation(lua, p, pe, &result, 0);
        if(np == NULL) { p--; {p++; cs = 6; goto _out;} } else {p = (( np))-1;}
    }
	goto st6;
st6:
	if ( ++p == pe )
		goto _test_eof6;
case 6:
#line 1909 "ext/luaparser/luaparser.c"
	switch( (*p) ) {
		case 13: goto st6;
		case 32: goto st6;
		case 45: goto st4;
		case 95: goto tr3;
	}
	if ( (*p) < 65 ) {
		if ( 9 <= (*p) && (*p) <= 10 )
			goto st6;
	} else if ( (*p) > 91 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr3;
	} else
		goto tr3;
	goto st0;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
	if ( (*p) == 45 )
		goto st5;
	goto st0;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
	if ( (*p) == 10 )
		goto st6;
	goto st5;
	}
	_test_eof1: cs = 1; goto _test_eof; 
	_test_eof2: cs = 2; goto _test_eof; 
	_test_eof3: cs = 3; goto _test_eof; 
	_test_eof6: cs = 6; goto _test_eof; 
	_test_eof4: cs = 4; goto _test_eof; 
	_test_eof5: cs = 5; goto _test_eof; 

	_test_eof: {}
	_out: {}
	}

#line 703 "ext/luaparser/luaparser.rl"


  if (cs >= LuaTable_first_final && p == pe) {
    return result;
  } else {
    rb_enc_raise(rb_utf8_encoding(), eParserError, "%u: unexpected token at '%s'", __LINE__, p);
    return Qnil;
  }
}