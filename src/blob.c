/*
Boilerplate 28: GreaseMonkey's boilerplate code for Ludum Dare #28
2013, Ben "GreaseMonkey" Russell - Public Domain
*/

#include "common.h"

// Generates a blob VA.
// Also returns a userdata on the Lua stack.
blob_t *blob_new(lua_State *L, float *data, GLenum mode, int dims, int points)
{
	blob_t *bl = lua_newuserdata(L, sizeof(blob_t)
		+ (dims * points * sizeof(GLfloat)));
	
	// no __gc needed.
	
	bl->mode = mode;
	bl->dims = dims;
	bl->points = points;

	if(data != NULL)
		memcpy(bl->data, data, (dims * points * sizeof(GLfloat)));

	return bl;
}

// blob.new((int)dims, (list)data): Creates a blob from a list.
int lf_blob_new(lua_State *L)
{
	int i;

	int top = lua_gettop(L);
	if(top < 3) return luaL_error(L, "not enough arguments for blob.new");

	GLenum mode = lua_tointeger(L, 1);
	int dims = lua_tointeger(L, 2);
	int ent_count = (int)lua_objlen(L, 3);

	if(dims < 2 || dims > 4) return luaL_error(L, "dims size invalid");
	if(ent_count % dims != 0) return luaL_error(L, "length invalid for dims");

	blob_t *bl = blob_new(L, NULL, mode, dims, ent_count / dims);
	
	for(i = 0; i < ent_count; i++)
	{
		lua_pushinteger(L, i+1);
		lua_gettable(L, 3);
		float v = lua_tonumber(L, -1);
		lua_remove(L, -1);
		bl->data[i] = v;
	}

	return 1;
}

// blob.render((blob)bl, (float)r, (float)g, (float)b [, (float)a]): Renders a blob.
int lf_blob_render(lua_State *L)
{
	int top = lua_gettop(L);
	if(top < 4) return luaL_error(L, "not enough arguments for blob.render");

	blob_t *bl = lua_touserdata(L, 1);
	double r = lua_tonumber(L, 2);
	double g = lua_tonumber(L, 3);
	double b = lua_tonumber(L, 4);
	double a = (top < 5 ? 1.0 : lua_tonumber(L, 5));

	glEnableClientState(GL_VERTEX_ARRAY);
	glColor4d(r, g, b, a);
	glVertexPointer(bl->dims, GL_FLOAT, 0, bl->data);
	glDrawArrays(bl->mode, 0, bl->points);
	glDisableClientState(GL_VERTEX_ARRAY);

	return 0;
}
