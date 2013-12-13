/*
Boilerplate 28: GreaseMonkey's boilerplate code for Ludum Dare #28
2013, Ben "GreaseMonkey" Russell - Public Domain
*/

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>

#include <zlib.h>

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
	GLenum mode;
	int dims, points;
	GLfloat data[];
} blob_t;

typedef struct img
{
	int w, h;
	int gw, gh;
	GLuint gltex;
	uint32_t data[];
} img_t;

typedef struct wav wav_t;
typedef struct voice voice_t;
struct voice
{
	wav_t *wav;
	voice_t **ud;
	voice_t *svprev, *svtail; // used for when wav gets garbage collected
	voice_t *pprev, *ptail; // used for the actual playing of stuff
	float freq;
	float offs;
	float lvol, rvol;
};

struct wav
{
	voice_t *svtail; // used for when this wav_t gets garbage collected
	float freq;
	int chns; // 1 == mono, 2 == stereo
	int lplen; // 0 == no loop
	int len;
	int16_t data[];
};

// blob.c
blob_t *blob_new(lua_State *L, float *data, GLenum mode, int dims, int points);
int lf_blob_new(lua_State *L);
int lf_blob_render(lua_State *L);

// gl.c
void load_gl_lua_state(lua_State *L);

// lua.c
extern lua_State *Lg;
int init_lua(void);
int render_lua(int64_t sec_current);
int tick_lua(int64_t sec_current);

// matrix.c
int lf_matrix_new(lua_State *L);
int lf_matrix_load_modelview(lua_State *L);
int lf_matrix_load_projection(lua_State *L);
int lf_matrix_identity(lua_State *L);
int lf_matrix_dup(lua_State *L);
int lf_matrix_apply(lua_State *L);
int lf_matrix_translate(lua_State *L);
int lf_matrix_scale(lua_State *L);
int lf_matrix_rotate(lua_State *L);

// mus.c
int lf_mus_load(lua_State *L);
int lf_mus_play(lua_State *L);
int lf_mus_stop(lua_State *L);

// png.c
int lf_png_load(lua_State *L);
int lf_png_get_dims(lua_State *L);
int lf_png_render(lua_State *L);

// wav.c
#define WAV_SAMPLES 2048
extern SDL_mutex *mtx_sackit;
extern sackit_playback_t *sackit;
extern it_module_t *mod_play;
extern volatile int mod_luafree;
int init_wav(void);
int lf_wav_load(lua_State *L);
int lf_wav_play(lua_State *L);

// main.c
extern int mouse_x;
extern int mouse_y;
extern int sys_width;
extern int sys_height;
void eprintf(const char *fmt, ...);
int64_t get_time(void);

