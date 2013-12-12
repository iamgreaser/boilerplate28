/*
Boilerplate 28: GreaseMonkey's boilerplate code for Ludum Dare #28
2013, Ben "GreaseMonkey" Russell - Public Domain
*/

#include "common.h"

// Generates a blob VA.
// Also returns a userdata on the Lua stack.
blob_t *blob_new(lua_State *L, float *data, int dims, int points)
{
	blob_t *blob = lua_newuserdata(L, sizeof(blob_t)
		+ (dims * points * sizeof(GLfloat)));
	
	// no __gc needed.
	
	blob->dims = dims;
	blob->points = points;

	if(data != NULL)
		memcpy(blob->data, data, (dims * points * sizeof(GLfloat)));

	return blob;
}

// blob.new((int)dims, (list)data): Creates a blob from a list.
int lf_blob_new(lua_State *L)
{
	int i;

	int top = lua_gettop(L);
	if(top < 2) return luaL_error(L, "not enough arguments for blob_new");

	int dims = lua_tointeger(L, 1);
	int ent_count = (int)lua_objlen(L, 2);

	if(dims < 2 || dims > 4) return luaL_error(L, "dims size invalid");
	if(ent_count % dims != 0) return luaL_error(L, "length invalid for dims");

	blob_t *blob = blob_new(L, NULL, dims, ent_count);
	
	for(i = 0; i < ent_count; i++)
	{
		lua_pushinteger(L, i+1);
		lua_gettable(L, 2);
		float v = lua_tonumber(L, -1);
		lua_remove(L, -1);
		blob->data[i] = v;
	}

	return 1;
}

