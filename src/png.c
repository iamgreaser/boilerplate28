/*
  PNG loader
  Copyright (C) 2013 Ben "GreaseMonkey" Russell & contributors

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#include "common.h"

// Lua metatable __gc function for img_t userdata.
int lmf_gc_png(lua_State *L)
{
	img_t *img = lua_touserdata(L, 1);
	printf("Freeing image %p\n", img);

	if(img->gltex != 0) glDeleteTextures(1, &(img->gltex));
	
	return 0;
}

// Loads a png from a file.
// Returns the image userdata on the Lua stack.
// Modified from the seabase version to load from a file.
img_t *img_load_png(lua_State *L, FILE *fp)
{
	char hdrtag[8];
	hdrtag[7] = 'X';
	fread(hdrtag, 8, 1, fp);
	if(memcmp(hdrtag, "\x89PNG\x0D\x0A\x1A\x0A", 8))
	{
		eprintf("img_load_png: PNG signature mismatch\n");
		return NULL;
	}

	char *idat_buf = NULL;
	int idat_len = 0;

	int x, y, i;

	int img_w = 0, img_h = 0;
	int img_bpc = 0, img_ct = 0;
	int img_cm = 0, img_fm = 0, img_im = 0;

	char *cbuf = NULL;

	for(;;)
	{
		// Get chunk region
		uint32_t clen;
		uint8_t bebuf[4];
		char cname[4];
		fread(bebuf, 4, 1, fp);
		clen = ((uint32_t)(bebuf[3]))
			| (((uint32_t)(bebuf[2])) << 8)
			| (((uint32_t)(bebuf[1])) << 16)
			| (((uint32_t)(bebuf[0])) << 24);
		
		if(fread(cname, 4, 1, fp) != 1)
		{
			eprintf("img_load_png: file truncated\n");
			if(idat_buf != NULL) free(idat_buf);
			if(cbuf != NULL) free(cbuf);
			return NULL;
		}

		cbuf = realloc(cbuf, clen + 4);
		memcpy(cbuf, cname, 4);
		fread(cbuf+4, 1, clen, fp);

		uint32_t csum;
		fread(bebuf, 4, 1, fp);
		csum = ((uint32_t)(bebuf[3]))
			| (((uint32_t)(bebuf[2])) << 8)
			| (((uint32_t)(bebuf[1])) << 16)
			| (((uint32_t)(bebuf[0])) << 24);
		uint32_t realcsum = crc32(0, (const Bytef *)(cbuf), (uInt)(clen + 4));

		if(csum != realcsum)
		{
			eprintf("img_load_png: checksum fail %08X %08X\n", csum, realcsum);
			if(idat_buf != NULL) free(idat_buf);
			if(cbuf != NULL) free(cbuf);
			return NULL;
		}
		/*
		printf("%c%c%c%c: checksum %08X - calculated %08X\n"
			, cname[0], cname[1], cname[2], cname[3]
			, csum, realcsum);
		*/

		// Parse chunk
		if(!memcmp(cname, "IEND", 4))
		{
			break;
		} else if(!memcmp(cname, "IHDR", 4)) {
			if(clen != 13)
			{
				eprintf("img_load_png: invalid IHDR length\n");
				if(idat_buf != NULL) free(idat_buf);
				if(cbuf != NULL) free(cbuf);
				return NULL;
			}

			img_w = ((uint32_t)(cbuf[7]))
				| (((uint32_t)(cbuf[6])) << 8)
				| (((uint32_t)(cbuf[5])) << 16)
				| (((uint32_t)(cbuf[4])) << 24);
			img_h = ((uint32_t)(cbuf[11]))
				| (((uint32_t)(cbuf[10])) << 8)
				| (((uint32_t)(cbuf[9])) << 16)
				| (((uint32_t)(cbuf[8])) << 24);
			img_bpc = (int)(uint8_t)cbuf[12];
			img_ct = (int)(uint8_t)cbuf[13];
			img_cm = (int)(uint8_t)cbuf[14];
			img_fm = (int)(uint8_t)cbuf[15];
			img_im = (int)(uint8_t)cbuf[16];

			// sanity checks
			if(img_w <= 0 || img_h <= 0 || img_w > 32768 || img_h > 32768
				|| img_w * img_h > (2<<26))
			{
				eprintf("img_load_png: image too large or contains a 0 dimension\n");
				if(idat_buf != NULL) free(idat_buf);
				if(cbuf != NULL) free(cbuf);
				return NULL;
			}

			if(img_bpc != 8)
			{
				eprintf("img_load_png: only 8bpc images supported\n");
				if(idat_buf != NULL) free(idat_buf);
				if(cbuf != NULL) free(cbuf);
				return NULL;
			}

			if(img_ct != 6)
			{
				eprintf("img_load_png: only RGBA images supported\n");
				if(idat_buf != NULL) free(idat_buf);
				if(cbuf != NULL) free(cbuf);
				return NULL;
			}

			if(img_cm != 0)
			{
				eprintf("img_load_png: invalid compression method\n");
				if(idat_buf != NULL) free(idat_buf);
				if(cbuf != NULL) free(cbuf);
				return NULL;
			}

			if(img_im != 0)
			{
				eprintf("img_load_png: interlacing not supported\n");
				if(idat_buf != NULL) free(idat_buf);
				if(cbuf != NULL) free(cbuf);
				return NULL;
			}

			if(img_fm != 0)
			{
				eprintf("img_load_png: invalid filter method\n");
				if(idat_buf != NULL) free(idat_buf);
				if(cbuf != NULL) free(cbuf);
				return NULL;
			}

		} else if(!memcmp(cname, "IDAT", 4)) {
			// Concatenate
			idat_buf = realloc(idat_buf, idat_len + clen);
			memcpy(idat_buf + idat_len, cbuf + 4, clen);
			idat_len += clen;
		} else if(!(cname[0] & 0x20)) {
			eprintf("img_load_png: unhandled critical chunk %c%c%c%c\n"
				, cname[0], cname[1], cname[2], cname[3]);
			if(idat_buf != NULL) free(idat_buf);
			if(cbuf != NULL) free(cbuf);
			return NULL;
		}
	}

	if(cbuf != NULL) free(cbuf);

	if(idat_buf == NULL)
	{
		eprintf("img_load_png: no IDAT chunks in image\n");
		return NULL;
	}

	int unsize = (img_w*4+1)*img_h;
	uint8_t *unpackbuf = malloc(unsize);
	uLongf unlen = (uLongf)unsize;

	if(uncompress((Bytef *)unpackbuf, &unlen, (Bytef *)idat_buf, idat_len)
		|| unlen != (uLongf)unsize)
	{
		eprintf("img_load_png: uncompressed size incorrect or unpack failure\n");
		free(unpackbuf);
		free(idat_buf);
		return NULL;
	}

	// filter image
	uint8_t *fx = unpackbuf + 1;
	uint8_t *fa = NULL;
	uint8_t *fb = NULL;
	uint8_t *fc = NULL;

	for(y = 0; y < img_h; y++)
	{
		uint8_t *nfb = fx;
		uint8_t typ = fx[-1];
		fa = fc = NULL;

		// we're not loading PNG files all the time
		// so it's a good time to rank purity over speed
		for(x = 0; x < img_w; x++)
		{
			for(i = 0; i < 4; i++)
			{
				switch(typ)
				{
					case 0: // None
						break;
					case 1: // Sub
						fx[i] += (fa == NULL ? 0 : fa[i]);
						break;
					case 2: // Up
						fx[i] += (fb == NULL ? 0 : fb[i]);
						break;
					case 3: // Average
						fx[i] += (((int)(fa == NULL ? 0 : fa[i]))
							+ (int)(fb == NULL ? 0 : fb[i]))
							/ 2;
						break;
					case 4: // Paeth
					{
						int pia = (int)(fa == NULL ? 0 : fa[i]);
						int pib = (int)(fb == NULL ? 0 : fb[i]);
						int pic = (int)(fc == NULL ? 0 : fc[i]);

						int p = pia + pib - pic;
						int pa = (p > pia ? p - pia : pia - p);
						int pb = (p > pib ? p - pib : pib - p);
						int pc = (p > pic ? p - pic : pic - p);

						int pr = 0;
						if(pa <= pb && pa <= pc) pr = pia;
						else if(pb <= pc) pr = pib;
						else pr = pic;

						fx[i] += pr;
					} break;
					default:
						eprintf("img_load_png: invalid filter\n");
						free(unpackbuf);
						free(idat_buf);
						return NULL;
				}
			}

			fa = fx; fx += 4;
			if(fb != NULL) { fc = fb; fb += 4; }
		}

		fb = nfb;
		fx++; // Skip filter mode byte
	}

	// Create image
	// we need to expand it for the pre-2.0 cards
	int gw, gh;
	gw = 64;
	gh = 64;
	while(gw < img_w) gw <<= 1;
	while(gh < img_h) gh <<= 1;
	//gw = img_w; gh = img_h;
	img_t *img = lua_newuserdata(L, sizeof(img_t)
		+ sizeof(uint32_t) * gw * gh);
	img->w = img_w;
	img->h = img_h;
	img->gw = gw;
	img->gh = gh;

	img->gltex = 0;

	lua_newtable(L);
	lua_pushcfunction(L, lmf_gc_png);
	lua_setfield(L, -2, "__gc");
	lua_setmetatable(L, -2);

	for(y = 0; y < img_h; y++)
	{
		uint8_t *p = (unpackbuf + y*(4*img_w+1) + 1);
		uint8_t *d = (uint8_t *)(img->data + gw*y);
		for(x = 0; x < img_w; x++)
		{
			d[0] = p[0];
			d[1] = p[1];
			d[2] = p[2];
			d[3] = p[3];
			p += 4;
			d += 4;
		}
	}

	free(unpackbuf);
	free(idat_buf);
	return img;
}

// png.load((str)fname): Loads a 32bpp RGBA PNG image.
int lf_png_load(lua_State *L)
{
	int top = lua_gettop(L);
	if(top < 1) return luaL_error(L, "not enough arguments for png.load");

	const char *fname = lua_tostring(L, 1);
	FILE *fp = fopen(fname, "rb");
	if(fp == NULL) return luaL_error(L, "could not open PNG file %s", fname);

	img_t *img = img_load_png(L, fp);
	if(img == NULL)
	{
		fclose(fp);
		return luaL_error(L, "failed to parse PNG file %s", fname);
	}

	return 1;
}

// png.get_dims((img)img): Returns the dimensions of an image ((int)w, (int)h).
int lf_png_get_dims(lua_State *L)
{
	int top = lua_gettop(L);
	if(top < 1) return luaL_error(L, "not enough arguments for png.get_dims");

	img_t *img = lua_touserdata(L, 1);

	lua_pushinteger(L, img->w);
	lua_pushinteger(L, img->h);

	return 2;
}

// png.render((img)img, (float)x1, (float)y1, (float)x2, (float)y2,
// 	(float)tx1, (float)ty1, (float)tx2, (float)ty2,
//	(float)r=1, (float)g=1, (float)b=1, (float)a=1):
// Renders an image given a set of box coordinates and an optional colour.
int lf_png_render(lua_State *L)
{
	int top = lua_gettop(L);
	if(top < 9) return luaL_error(L, "not enough arguments for png.render");

	img_t *img = lua_touserdata(L, 1);
	double x1 = lua_tonumber(L, 2);
	double y1 = lua_tonumber(L, 3);
	double x2 = lua_tonumber(L, 4);
	double y2 = lua_tonumber(L, 5);
	double tx1 = lua_tonumber(L, 6);
	double ty1 = lua_tonumber(L, 7);
	double tx2 = lua_tonumber(L, 8);
	double ty2 = lua_tonumber(L, 9);
	double r = (top < 12 ? 1.0 : lua_tonumber(L, 10));
	double g = (top < 12 ? 1.0 : lua_tonumber(L, 11));
	double b = (top < 12 ? 1.0 : lua_tonumber(L, 12));
	double a = (top < 13 ? 1.0 : lua_tonumber(L, 13));

	tx1 = (tx1*img->w)/img->gw;
	ty1 = (ty1*img->h)/img->gh;
	tx2 = (tx2*img->w)/img->gw;
	ty2 = (ty2*img->h)/img->gh;

	glEnable(GL_TEXTURE_2D);

	if(img->gltex == 0)
	{
		// load texture!
		glGenTextures(1, &(img->gltex));
		glBindTexture(GL_TEXTURE_2D, img->gltex);
		glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RGBA,
			img->gw,
			img->gh,
			0, // who the hell uses the border anyway?!
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			img->data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	} else {
		glBindTexture(GL_TEXTURE_2D, img->gltex);
	}

	glColor4d(r, g, b, a);
	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2d(tx1, ty1); glVertex2d(x1, y1);
		glTexCoord2d(tx2, ty1); glVertex2d(x2, y1);
		glTexCoord2d(tx1, ty2); glVertex2d(x1, y2);
		glTexCoord2d(tx2, ty2); glVertex2d(x2, y2);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	return 0;
}

