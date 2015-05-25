/*
** Library for binary operations
** Copyright bel 2015, MIT license
*/


#include <stdlib.h>
#include <math.h>

#define lbinarylib_c
#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"


#ifndef LUA_BINARY_DEFAULT_DIG
#define LUA_BINARY_DEFAULT_DIG    ((lua_Unsigned)(32))
#endif
#ifndef LUA_BINARY_BITS
#define LUA_BINARY_BITS unsigned long long
#endif
#define luabin_bits LUA_BINARY_BITS

#define num2bits(x) ((luabin_bits)(x))
#define bits2num(x) ((lua_Unsigned)(x))
#define dig2filter(d) (~((~(luabin_bits)0) << (int)(d)))



static int binary_and (lua_State *L) {
  lua_Number a = luaL_checknumber(L, 1);
  lua_Number b = luaL_checknumber(L, 2);
  lua_Unsigned dig = luaL_optunsigned(L, 3, LUA_BINARY_DEFAULT_DIG);
  luabin_bits xa = num2bits(a);
  luabin_bits xb = num2bits(b);
  luabin_bits xdig = dig2filter(dig);
  luabin_bits xr = (xa & xb) & xdig;
  lua_pushunsigned(L, bits2num(xr));
  return 1;
}

static int binary_nand (lua_State *L) {
  lua_Number a = luaL_checknumber(L, 1);
  lua_Number b = luaL_checknumber(L, 2);
  lua_Unsigned dig = luaL_optunsigned(L, 3, LUA_BINARY_DEFAULT_DIG);
  luabin_bits xa = num2bits(a);
  luabin_bits xb = num2bits(b);
  luabin_bits xdig = dig2filter(dig);
  luabin_bits xr = ~(xa & xb) & xdig;
  lua_pushunsigned(L, bits2num(xr));
  return 1;
}

static int binary_or (lua_State *L) {
  lua_Number a = luaL_checknumber(L, 1);
  lua_Number b = luaL_checknumber(L, 2);
  lua_Unsigned dig = luaL_optunsigned(L, 3, LUA_BINARY_DEFAULT_DIG);
  luabin_bits xa = num2bits(a);
  luabin_bits xb = num2bits(b);
  luabin_bits xdig = dig2filter(dig);
  luabin_bits xr = (xa | xb) & xdig;
  lua_pushunsigned(L, bits2num(xr));
  return 1;
}

static int binary_nor (lua_State *L) {
  lua_Number a = luaL_checknumber(L, 1);
  lua_Number b = luaL_checknumber(L, 2);
  lua_Unsigned dig = luaL_optunsigned(L, 3, LUA_BINARY_DEFAULT_DIG);
  luabin_bits xa = num2bits(a);
  luabin_bits xb = num2bits(b);
  luabin_bits xdig = dig2filter(dig);
  luabin_bits xr = ~(xa | xb) & xdig;
  lua_pushunsigned(L, bits2num(xr));
  return 1;
}

static int binary_xor (lua_State *L) {
  lua_Number a = luaL_checknumber(L, 1);
  lua_Number b = luaL_checknumber(L, 2);
  lua_Unsigned dig = luaL_optunsigned(L, 3, LUA_BINARY_DEFAULT_DIG);
  luabin_bits xa = num2bits(a);
  luabin_bits xb = num2bits(b);
  luabin_bits xdig = dig2filter(dig);
  luabin_bits xr = (xa ^ xb) & xdig;
  lua_pushunsigned(L, bits2num(xr));
  return 1;
}

static int binary_eq (lua_State *L) {
  lua_Number a = luaL_checknumber(L, 1);
  lua_Number b = luaL_checknumber(L, 2);
  lua_Unsigned dig = luaL_optunsigned(L, 3, LUA_BINARY_DEFAULT_DIG);
  luabin_bits xa = num2bits(a);
  luabin_bits xb = num2bits(b);
  luabin_bits xdig = dig2filter(dig);
  luabin_bits xr = ~(xa ^ xb) & xdig;
  lua_pushunsigned(L, bits2num(xr));
  return 1;
}

static int binary_not (lua_State *L) {
  lua_Number a = luaL_checknumber(L, 1);
  lua_Unsigned dig = luaL_optunsigned(L, 2, LUA_BINARY_DEFAULT_DIG);
  luabin_bits xa = num2bits(a);
  luabin_bits xdig = dig2filter(dig);
  luabin_bits xr = ~xa & xdig;
  lua_pushunsigned(L, bits2num(xr));
  return 1;
}

static luabin_bits binextract (const unsigned char * src, size_t offs, size_t len) {
  luabin_bits xr = 0;
  luabin_bits xdig = dig2filter(len);
  size_t split = ((offs+len-1)%8) < (offs%8) ? 1 : 0;
  memcpy(&xr, src + (offs/8), (len+7)/8 + split);
  xr >>= (offs%8);
  return xr & xdig;
}

static int binary_get (lua_State *L) {
  size_t n = 0;
  const char * src = luaL_checklstring(L, 1, &n);
  lua_Unsigned offs = luaL_optunsigned(L, 2, 0);
  lua_Unsigned dig = luaL_optunsigned(L, 3, LUA_BINARY_DEFAULT_DIG);
  luabin_bits xr;

  if (n*8 < offs + dig) {
    return luaL_error(L, "Index out of range");
  }

  xr = binextract(src, offs, dig);
  lua_pushunsigned(L, bits2num(xr));
  return 1;
}

static int binary_poke (lua_State *L) {
  size_t n = 0;
  size_t total;
  const char * src = luaL_checklstring(L, 1, &n);
  lua_Unsigned dta = luaL_checkunsigned(L, 2);
  lua_Unsigned offs = luaL_optunsigned(L, 3, 0);
  lua_Unsigned dig = luaL_optunsigned(L, 4, LUA_BINARY_DEFAULT_DIG);
  luabin_bits xdig = dig2filter(dig);
  luabin_bits xdta = num2bits(dta) & xdig;
  luabin_bits xtmp;

  if (n*8 > offs + dig) {
    total = offs + dig;
  } else {
    total = n*8;
  }

  #ifdef LUA_BINARY_AS_LUA_PATCH
/* TODO: using internal Lua functions allows to do this without a temporary copy */
  #else
  {
    char * stmp = (char*)calloc(1, (total+7)/8);
    if (!stmp) return luaL_error(L, "Out of memory");
    memcpy(stmp, src, n);

    /* TODO: xtmp */

    lua_pushlstring(L, stmp, (total+7)/8);
    free(stmp);
  }
  #endif

  return 1;
}

static int binary_toarray (lua_State *L) {
  size_t i, tl, n = 0;
  const unsigned char * src = (const unsigned char *)luaL_checklstring(L, 1, &n);
  lua_Unsigned bits = luaL_optunsigned(L, 2, 0);

  tl = (bits>0) ? ((n*8+bits-1)/bits) : (n*8);
  lua_createtable(L, tl, 0);
  for (i=0; i<tl; i++) {
    if (bits==0) {
      lua_pushboolean(L, binextract(src, i, 1));
    } else {
      lua_pushunsigned(L, binextract(src, i*bits, bits));
    }
    lua_rawseti(L, -2, i+1);
  }
  return 1;
}

static int binary_fromarray (lua_State *L) {
  size_t i, tl, n;
  lua_Unsigned bits;
  char *ret;

  luaL_checktype(L, 1, LUA_TTABLE);
  bits = luaL_optunsigned(L, 2, 0);
  tl = lua_objlen(L, 1);

  n = (tl * 8 + 7) / bits;
  ret = (char*)malloc(n);
  if (ret == NULL) return luaL_error(L, "Out of memory");

  /* TODO convert array to string */

  lua_pushlstring(L, ret, n);
  free(ret);

  return 1;
}

static const luaL_Reg binarylib[] = {
/* bit field operations on numbers */
  {"band",  binary_and},
  {"bnand", binary_nand},
  {"bor",   binary_or},
  {"bnor",  binary_nor},
  {"bxor",  binary_xor},
  {"beq",   binary_eq},
  {"bnot",  binary_not},

/* casts between strings and bit fields */
  {"bget", binary_get},
  {"poke", binary_poke},

/* casts between strings and bit/byte/word/.. arrays */
  {"toarray",   binary_toarray},
  {"fromarray", binary_fromarray},

  {NULL, NULL}
};


/*
** Open binary library
*/
LUAMOD_API int luaopen_binary (lua_State *L) {
  luaL_newlib(L, binarylib);
  return 1;
}

/*
LUAMOD_API int luaopen_binary (lua_State *L) {
  luaL_newlibtable(L, binarylib);
  luaL_setfuncs(L, binarylib, 0);
  lua_setglobal(L, "binary");
  return 1;
}
*/

