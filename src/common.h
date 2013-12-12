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

#include "linmath.h"

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
int lf_blob_render(lua_State *L);

// gl.c
void load_gl_lua_state(lua_State *L);

// lua.c
int init_lua(void);
int render_lua(int64_t sec_current);
int tick_lua(int64_t sec_current);

// matrix.c
int lf_matrix_new(lua_State *L);
int lf_matrix_load_modelview(lua_State *L);
int lf_matrix_load_projection(lua_State *L);
int lf_matrix_identity(lua_State *L);
int lf_matrix_translate(lua_State *L);
int lf_matrix_scale(lua_State *L);
int lf_matrix_rotate(lua_State *L);

// main.c
extern int sys_width;
extern int sys_height;
void eprintf(const char *fmt, ...);
int64_t get_time(void);

