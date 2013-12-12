/*
Boilerplate 28: GreaseMonkey's boilerplate code for Ludum Dare #28
2013, Ben "GreaseMonkey" Russell - Public Domain
*/

#include "common.h"

lua_State *Lg = NULL;

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
	
	return 0;
}

