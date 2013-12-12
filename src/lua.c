/*
Boilerplate 28: GreaseMonkey's boilerplate code for Ludum Dare #28
2013, Ben "GreaseMonkey" Russell - Public Domain
*/

#include "common.h"

lua_State *Lg = NULL;

int64_t ltime_render = 0;
int64_t ltime_tick = 0;

// Converts the given time to a float.
double time_to_float(int64_t time)
{
	return ((double)time)/(double)TIME_PER_SECOND;
}

// Set up Lua.
int init_lua(void)
{
	lua_State *L = Lg = luaL_newstate();
	luaL_openlibs(L);

	// blob
	lua_newtable(L);
	lua_pushcfunction(L, lf_blob_new); lua_setfield(L, -2, "new");
	lua_setglobal(L, "blob");

	// RUN THE DAMN THING
	if(luaL_dofile(L, "game/main.lua"))
	{
		eprintf("lua: %s\n", lua_tostring(L, -1));
		lua_remove(L, -1);
		return 1;
	}
	
	ltime_render = ltime_tick = get_time();
	return 0;
}

int render_lua(int64_t sec_current)
{
	lua_State *L = Lg;

	lua_getglobal(L, "hook_render");
	lua_pushnumber(L, time_to_float(sec_current));
	lua_pushnumber(L, time_to_float(sec_current - ltime_render));
	ltime_render = sec_current;
	lua_call(L, 2, 0); // TODO? lua_pcall?

	return 0;
}

int tick_lua(int64_t sec_current)
{
	lua_State *L = Lg;

	lua_getglobal(L, "hook_tick");
	lua_pushnumber(L, time_to_float(sec_current));
	lua_pushnumber(L, time_to_float(sec_current - ltime_tick));
	ltime_tick = sec_current;
	lua_call(L, 2, 0); // TODO? lua_pcall?

	return 0;
}

