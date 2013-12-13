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

// sys.get_screen_dims(): Gets the screen dimensions ((int)w, (int)h).
int lf_sys_get_screen_dims(lua_State *L)
{
	lua_pushinteger(L, sys_width);
	lua_pushinteger(L, sys_height);

	return 2;
}

// sys.get_mouse(): Gets the mouse position ((int)x, (int)y).
// Returns -1, -1 if the mouse is unfocused.
int lf_sys_get_mouse(lua_State *L)
{
	lua_pushinteger(L, mouse_x);
	lua_pushinteger(L, mouse_y);

	return 2;
}

// Set up Lua.
int init_lua(void)
{
	lua_State *L = Lg = luaL_newstate();
	luaL_openlibs(L);

	// sys
	lua_newtable(L);
	lua_pushcfunction(L, lf_sys_get_screen_dims); lua_setfield(L, -2, "get_screen_dims");
	lua_pushcfunction(L, lf_sys_get_mouse); lua_setfield(L, -2, "get_mouse");
	lua_setglobal(L, "sys");

	// blob
	lua_newtable(L);
	lua_pushcfunction(L, lf_blob_new); lua_setfield(L, -2, "new");
	lua_pushcfunction(L, lf_blob_render); lua_setfield(L, -2, "render");
	lua_setglobal(L, "blob");

	// GL
	lua_newtable(L);
	load_gl_lua_state(L);
	lua_setglobal(L, "GL");

	// M (matrix)
	lua_newtable(L);
	lua_pushcfunction(L, lf_matrix_new); lua_setfield(L, -2, "new");
	lua_pushcfunction(L, lf_matrix_load_modelview); lua_setfield(L, -2, "load_modelview");
	lua_pushcfunction(L, lf_matrix_load_projection); lua_setfield(L, -2, "load_projection");
	lua_pushcfunction(L, lf_matrix_identity); lua_setfield(L, -2, "identity");
	lua_pushcfunction(L, lf_matrix_dup); lua_setfield(L, -2, "dup");
	lua_pushcfunction(L, lf_matrix_apply); lua_setfield(L, -2, "apply");
	lua_pushcfunction(L, lf_matrix_translate); lua_setfield(L, -2, "translate");
	lua_pushcfunction(L, lf_matrix_scale); lua_setfield(L, -2, "scale");
	lua_pushcfunction(L, lf_matrix_rotate); lua_setfield(L, -2, "rotate");
	lua_setglobal(L, "M");

	// mus
	lua_newtable(L);
	lua_pushcfunction(L, lf_mus_load); lua_setfield(L, -2, "load");
	lua_pushcfunction(L, lf_mus_play); lua_setfield(L, -2, "play");
	lua_pushcfunction(L, lf_mus_stop); lua_setfield(L, -2, "stop");
	lua_setglobal(L, "mus");

	// png
	lua_newtable(L);
	lua_pushcfunction(L, lf_png_load); lua_setfield(L, -2, "load");
	lua_pushcfunction(L, lf_png_get_dims); lua_setfield(L, -2, "get_dims");
	lua_pushcfunction(L, lf_png_render); lua_setfield(L, -2, "render");
	lua_setglobal(L, "png");

	// wav
	lua_newtable(L);
	lua_pushcfunction(L, lf_wav_load); lua_setfield(L, -2, "load");
	lua_pushcfunction(L, lf_wav_play); lua_setfield(L, -2, "play");
	lua_setglobal(L, "wav");

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

