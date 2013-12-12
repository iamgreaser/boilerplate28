/*
Boilerplate 28: GreaseMonkey's boilerplate code for Ludum Dare #28
2013, Ben "GreaseMonkey" Russell - Public Domain
*/

#include "common.h"

// M.new(): Creates a new 4x4 matrix and preloads the identity.
int lf_matrix_new(lua_State *L)
{
	mat4x4 *m = lua_newuserdata(L, sizeof(mat4x4));
	mat4x4_identity(*m);
	return 1;
}

// M.load_modelview((matrix)m): Loads the GL_MODELVIEW matrix.
int lf_matrix_load_modelview(lua_State *L)
{
	mat4x4 *m = lua_touserdata(L, 1);
	// TODO: check if this needs to be transposed!
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf((GLfloat *)m);
	return 0;
}

// M.load_projection((matrix)m): Loads the GL_PROJECTION matrix.
int lf_matrix_load_projection(lua_State *L)
{
	mat4x4 *m = lua_touserdata(L, 1);
	// TODO: check if this needs to be transposed!
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf((GLfloat *)m);
	return 0;
}

// M.identity((matrix)m): Loads the identity into a matrix.
int lf_matrix_identity(lua_State *L)
{
	mat4x4 *m = lua_touserdata(L, 1);
	mat4x4_identity(*m);
	return 0;
}

// M.dup((matrix)dst, (matrix)src): Duplicates a matrix.
int lf_matrix_dup(lua_State *L)
{
	mat4x4 *dst = lua_touserdata(L, 1);
	mat4x4 *src = lua_touserdata(L, 2);
	mat4x4_dup(*dst, *src);
	return 0;
}

// M.apply((matrix)dst, (matrix)src): Applies a matrix to another.
int lf_matrix_apply(lua_State *L)
{
	mat4x4 *dst = lua_touserdata(L, 1);
	mat4x4 *src = lua_touserdata(L, 2);
	mat4x4 tmp;
	mat4x4_dup(tmp, *dst);
	mat4x4_mul(*dst, tmp, *src);
	return 0;
}

// M.translate((matrix)m, (float)x, (float)y, (float)z): Translates the matrix.
int lf_matrix_translate(lua_State *L)
{
	mat4x4 *m = lua_touserdata(L, 1);
	float x = lua_tonumber(L, 2);
	float y = lua_tonumber(L, 3);
	float z = lua_tonumber(L, 4);
	mat4x4_translate_in_place(*m, x, y, z);
	return 0;
}

// M.scale((matrix)m, (float)x, (float)y, (float)z): Scales the matrix.
int lf_matrix_scale(lua_State *L)
{
	mat4x4 *m = lua_touserdata(L, 1);
	float x = lua_tonumber(L, 2);
	float y = lua_tonumber(L, 3);
	float z = lua_tonumber(L, 4);
	mat4x4_scale_aniso(*m, *m, x, y, z);
	return 0;
}

// M.rotate((matrix)m, (float)angle, (float)x, (float)y, (float)z): Rotates the matrix.
int lf_matrix_rotate(lua_State *L)
{
	mat4x4 *m = lua_touserdata(L, 1);
	float angle = lua_tonumber(L, 2);
	float x = lua_tonumber(L, 3);
	float y = lua_tonumber(L, 4);
	float z = lua_tonumber(L, 5);
	mat4x4 tmp;
	mat4x4_dup(tmp, *m);
	mat4x4_rotate(*m, tmp, x, y, z, angle);
	return 0;
}

