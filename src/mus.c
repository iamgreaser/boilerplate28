/*
Boilerplate 28: GreaseMonkey's boilerplate code for Ludum Dare #28
2013, Ben "GreaseMonkey" Russell - Public Domain
*/

#include "common.h"

// itmodule gc metatable method.
int lmf_itmodule_gc(lua_State *L)
{
	it_module_t **p = lua_touserdata(L, 1);
	it_module_t *mod = *p;

	//printf("mus gc: %p\n", p);

	if(SDL_LockMutex(mtx_sackit) == 0)
	{
		if(mod == mod_play)
			mod_luafree = 1;
		else
			free(mod);
	} else {
		printf("PANIC: Could not lock global sackit mutex!\n"); 
		fflush(stdout);
		abort();
	}

	SDL_UnlockMutex(mtx_sackit);

	return 0;
}

// mus.load((str)fname): Load an .it module.
//
// XXX: the userdata used here is an it_module_t **.
// If you wish to free the memory of an .it module,
// it is advised that you call collectgarbage() afterwards.
//
// Returns a module pointer userdata on success.
int lf_mus_load(lua_State *L)
{
	int top = lua_gettop(L);
	if(top < 1) return luaL_error(L, "not enough arguments for mus.load");

	const char *fname = lua_tostring(L, 1);

	it_module_t *mod = sackit_module_load(fname);
	if(mod == NULL)
		return luaL_error(L, "failed to parse ImpulseTracker module file %s", fname);
	
	it_module_t **ret = lua_newuserdata(L, sizeof(it_module_t *));
	*ret = mod;

	lua_newtable(L);
	lua_pushcfunction(L, lmf_itmodule_gc);
	lua_setfield(L, -2, "__gc");
	lua_setmetatable(L, -2);

	return 1;
}

// mus.stop(): Stops playing music.
int lf_mus_stop(lua_State *L)
{
	if(SDL_LockMutex(mtx_sackit) == 0)
	{
		if(sackit != NULL)
		{
			sackit_playback_free(sackit);
			sackit = NULL;
		}

		if(mod_play != NULL)
		{
			if(mod_luafree != 0)
			{
				sackit_module_free(mod_play);
				mod_luafree = 0;
			}

			mod_play = NULL;
		}
	} else {
		printf("PANIC: Could not lock global sackit mutex!\n"); 
		fflush(stdout);
		abort();
	}

	SDL_UnlockMutex(mtx_sackit);

	return 0;
}

// mus.play((itmodule)mod): Plays the specified .it module.
// Stops any currently playing module.
int lf_mus_play(lua_State *L)
{
	int top = lua_gettop(L);
	if(top < 1) return luaL_error(L, "not enough arguments for mus.play");

	it_module_t **p = lua_touserdata(L, 1);
	it_module_t *mod = *p;

	lf_mus_stop(L); // Problem, Officer?

	if(SDL_LockMutex(mtx_sackit) == 0)
	{
		mod_play = mod;
		sackit = sackit_playback_new(mod, WAV_SAMPLES, 256, MIXER_IT214FS);
	} else {
		printf("PANIC: Could not lock global sackit mutex!\n"); 
		fflush(stdout);
		abort();
	}

	SDL_UnlockMutex(mtx_sackit);

	return 0;
}

