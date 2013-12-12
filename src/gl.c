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

	lua_pushinteger(L, GL_POINTS); lua_setfield(L, -2, "POINTS");
	lua_pushinteger(L, GL_LINES); lua_setfield(L, -2, "LINES");
	lua_pushinteger(L, GL_LINE_LOOP); lua_setfield(L, -2, "LINE_LOOP");
	lua_pushinteger(L, GL_LINE_STRIP); lua_setfield(L, -2, "LINE_STRIP");
	lua_pushinteger(L, GL_TRIANGLES); lua_setfield(L, -2, "TRIANGLES");
	lua_pushinteger(L, GL_TRIANGLE_STRIP); lua_setfield(L, -2, "TRIANGLE_STRIP");
	lua_pushinteger(L, GL_TRIANGLE_FAN); lua_setfield(L, -2, "TRIANGLE_FAN");
	lua_pushinteger(L, GL_QUADS); lua_setfield(L, -2, "QUADS");
	lua_pushinteger(L, GL_QUAD_STRIP); lua_setfield(L, -2, "QUAD_STRIP");
	lua_pushinteger(L, GL_POLYGON); lua_setfield(L, -2, "POLYGON");

	// load functions
	lua_pushcfunction(L, lf_glClearColor); lua_setfield(L, -2, "glClearColor");
	lua_pushcfunction(L, lf_glClear); lua_setfield(L, -2, "glClear");
}

