/*
Boilerplate 28: GreaseMonkey's boilerplate code for Ludum Dare #28
2013, Ben "GreaseMonkey" Russell - Public Domain
*/

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>

#include <SDL.h>
#include <GL/glew.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <sackit.h>

#ifdef WIN32
// Fuck you Windows.
#define TIME_PER_SECOND (1000)
#else
#include <sys/time.h>
#define TIME_PER_SECOND (1000000)
#endif

typedef struct blob
{
	int dims, points;
	GLfloat data[];
} blob_t;

// blob.c
blob_t *blob_new(lua_State *L, float *data, int dims, int points);
int lf_blob_new(lua_State *L);

// lua.c
int init_lua(void);
int render_lua(int64_t sec_current);
int tick_lua(int64_t sec_current);

// main.c
void eprintf(const char *fmt, ...);
int64_t get_time(void);

