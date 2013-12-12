/*
Boilerplate 28: GreaseMonkey's boilerplate code for Ludum Dare #28
2013, Ben "GreaseMonkey" Russell - Public Domain
*/

#include "common.h"

#define WRF_1I(name) \
	int lf_##name(lua_State *L) \
	{ \
		GLint i1 = lua_tointeger(L, 1); \
		name(i1); \
		return 0; \
	}

#define WRF_4D(name) \
	int lf_##name(lua_State *L) \
	{ \
		GLdouble d1 = lua_tonumber(L, 1); \
		GLdouble d2 = lua_tonumber(L, 2); \
		GLdouble d3 = lua_tonumber(L, 3); \
		GLdouble d4 = lua_tonumber(L, 4); \
		name(d1, d2, d3, d4); \
		return 0; \
	}

WRF_1I(glClear)
WRF_4D(glClearColor)

void load_gl_lua_state(lua_State *L)
{
	// load constants
	lua_pushinteger(L, GL_DEPTH_BUFFER_BIT); lua_setfield(L, -2, "DEPTH_BUFFER_BIT");
	lua_pushinteger(L, GL_ACCUM_BUFFER_BIT); lua_setfield(L, -2, "ACCUM_BUFFER_BIT");
	lua_pushinteger(L, GL_STENCIL_BUFFER_BIT); lua_setfield(L, -2, "STENCIL_BUFFER_BIT");
	lua_pushinteger(L, GL_COLOR_BUFFER_BIT); lua_setfield(L, -2, "COLOR_BUFFER_BIT");

	// 
	lua_pushcfunction(L, lf_glClearColor); lua_setfield(L, -2, "glClearColor");
	lua_pushcfunction(L, lf_glClear); lua_setfield(L, -2, "glClear");
}

