/*
Boilerplate 28: GreaseMonkey's boilerplate code for Ludum Dare #28
2013, Ben "GreaseMonkey" Russell - Public Domain
*/

#include "common.h"

int wav_dev = 0;

SDL_mutex *mtx_play = NULL;
SDL_mutex *mtx_sackit = NULL;
sackit_playback_t *sackit = NULL;
it_module_t *mod_play = NULL;
volatile int mod_luafree = 0;
voice_t *voice_chain = NULL;

// SDL audio callback.

SDL_AudioSpec wav_want, wav_have;
void cb_wav_update(void *userdata, Uint8 *stream, int len)
{
	int i;
	int slen = len / 4;
	int16_t *d = (int16_t *)stream;

	//printf("slen %i\n", slen);
	for(i = 0; i < slen; i++)
		d[2*i+0] = d[2*i+1] = 0;
	
	if(SDL_LockMutex(mtx_play) == 0)
	{
		voice_t *v;
		voice_t fv; // fake voice used to make the loop not crash on voice kill

		for(v = voice_chain; v != NULL; v = v->ptail)
		{
			wav_t *w = v->wav;

			double vspd = v->freq / (double)(wav_have.freq);
			double vm0 = v->lvol;
			double vm1 = v->rvol;

			for(i = 0; i < slen; i++)
			{
				int vp0, vp1;
				int vp = v->offs;

				if(vp >= w->len)
				{
					if(w->lplen == 0)
					{
						// kill voice
						//printf("voice kill: %p\n", v);

						if(w->svtail == v) w->svtail = v->svtail;
						if(v->svprev != NULL) v->svprev->svtail = v->svtail;
						if(v->svtail != NULL) v->svtail->svprev = v->svprev;
						if(v->pprev != NULL) v->pprev->ptail = v->ptail;
						if(v->ptail != NULL) v->ptail->pprev = v->pprev;

						if(voice_chain == v) voice_chain = v->ptail;

						if(v->ud != NULL) *(v->ud) = NULL;
						fv.ptail = v->ptail;
						fv.pprev = NULL;
						free(v);
						v = &fv;
						break;
					} else {
						vp = (vp - w->len) % w->lplen
							+ (w->len - w->lplen);
					}
				}

				if(vp >= 0)
				{
					if(w->chns == 1)
					{
						vp0 = vp1 = vp;
					} else {
						vp0 = 2 * vp + 0;
						vp1 = 2 * vp + 1;
					}

					int v0 = ((int)(d[2 * i + 0])) + (vm0*(int)(w->data[vp0]));
					int v1 = ((int)(d[2 * i + 1])) + (vm1*(int)(w->data[vp1]));

					if(v0 < -0x8000) v0 = -0x8000;
					if(v0 >  0x7FFF) v0 =  0x7FFF;
					if(v1 < -0x8000) v1 = -0x8000;
					if(v1 >  0x7FFF) v1 =  0x7FFF;

					d[2 * i + 0] = v0;
					d[2 * i + 1] = v1;
				}
				
				v->offs += vspd;
			}
		}

		SDL_UnlockMutex(mtx_play);
	}

	if(SDL_LockMutex(mtx_sackit) == 0)
	{
		if(sackit == NULL)
		{
			// Check if we need to free the module
			if(mod_play != NULL)
			{
				if(mod_luafree)
				{
					free(mod_play);
					mod_luafree = 0;
				}

				mod_play = NULL;
			}
		} else {
			// TODO: update buffer
			sackit_playback_update(sackit);

			double mvol = 1.0;

			for(i = 0; i < slen * 2; i++)
			{
				int v0 = ((int)(d[i])) + (mvol*(int)(sackit->buf[i]));

				if(v0 < -0x8000) v0 = -0x8000;
				if(v0 >  0x7FFF) v0 =  0x7FFF;

				d[i] = v0;
			}
		}

		SDL_UnlockMutex(mtx_sackit);
	}
}

// Initialises the sound subsystem.
int init_wav(void)
{
	// It's been a while since I've touched these.
	mtx_play = SDL_CreateMutex();
	mtx_sackit = SDL_CreateMutex();

	SDL_zero(wav_want);

	// TODO: fix the bug in sackit where it always assumes 44100Hz
	wav_want.freq = 44100;
	// TODO: big-endian support for sackit
	wav_want.format = AUDIO_S16SYS;
	// we're using a lightweight build,
	// and only building the stereo 2.14 filter nointerp mixer,
	wav_want.channels = 2;
	// check common.h for this setting. might make it a user setting somehow.
#ifdef WIN32
	// Fuck you Windows.
	// (Technically it's SDL's fault for this subtle difference,
	//  but Windows still sucks.)
	wav_want.samples = WAV_SAMPLES;
#else
	wav_want.samples = WAV_SAMPLES * 2;
#endif
	// and, uh, yeah.
	wav_want.callback = cb_wav_update;

	wav_dev = SDL_OpenAudioDevice(NULL, 0, &wav_want, &wav_have, 0);

	if(wav_dev == 0)
	{
		eprintf("init_wav: couldn't open sound (nonfatal, but there's no sound)\n");
		return 0;
	}

	SDL_PauseAudioDevice(wav_dev, 0);

	return 0;
}

// voice __gc metatable function.
int lmf_voice_gc(lua_State *L)
{
	voice_t **p = lua_touserdata(L, 1);
	//printf("voice gc: %p\n", p);

	if(SDL_LockMutex(mtx_play) == 0)
	{
		if(*p != NULL)
			(*p)->ud = NULL;
	} else {
		printf("PANIC: Could not lock global wav mutex!\n"); 
		fflush(stdout);
		abort();
	}

	SDL_UnlockMutex(mtx_play);

	return 0;
}

// wav __gc metatable function.
int lmf_wav_gc(lua_State *L)
{
	voice_t *v, *v2;
	wav_t *wav = lua_touserdata(L, 1);

	//printf("wav gc: %p\n", wav);

	if(SDL_LockMutex(mtx_play) == 0)
	{
		for(v = v2 = wav->svtail; v2 != NULL; v = v2)
		{
			v2 = v->svtail;

			if(v->pprev != NULL) v->pprev->ptail = v->ptail;
			if(v->ptail != NULL) v->ptail->pprev = v->pprev;
			if(voice_chain == v) voice_chain = v->ptail;
			// svprev and svtail don't matter - we're clearing that whole chain

			if(v->ud != NULL) *(v->ud) = NULL;

			free(v);
		}
	} else {
		printf("PANIC: Could not lock global wav mutex!\n"); 
		fflush(stdout);
		abort();
	}

	SDL_UnlockMutex(mtx_play);

	return 0;
}

// Loads a .wav file.
// Returns the userdata on the Lua stack provided the result isn't NULL.
// (It should be more tolerant than Iceball's loader.)
wav_t *wav_load(lua_State *L, const char *fname)
{
	FILE *fp = fopen(fname, "rb");

	if(fp == NULL)
	{
		int err = errno;
		eprintf("wav_load: could not open: %s (%i)\n",
			strerror(err), err);
		return NULL;
	}

	char hbuf[12];
	hbuf[11] = '\x00';
	fread(hbuf, 12, 1, fp);
	if(memcmp(hbuf + 0, "RIFF", 4))
	{
		eprintf("wav_load: not a RIFF WAVE file\n");
		fclose(fp);
		return NULL;
	}

	if(memcmp(hbuf + 8, "WAVE", 4))
	{
		eprintf("wav_load: not a WAVE file\n");
		fclose(fp);
		return NULL;
	}

	uint8_t tbuf[8];
	wav_t *wav = NULL;
	int freq = 0;
	int len = 0;
	int dlen = 0;
	uint8_t *data = NULL;
	int w_fmt_balign = 0;
	int w_fmt_chns = 0;
	int w_fmt_bps = 0;
	int w_fmt_codec = 0;
	//int w_fmt_bytesec = 0;

	// read tags until the cows come home
	while(fread(tbuf, 8, 1, fp) == 1)
	{
		int tlen = ((int)tbuf[4])
			| (((int)tbuf[5]) << 8)
			| (((int)tbuf[6]) << 16)
			| (((int)tbuf[7]) << 24);

		if(!memcmp(tbuf, "fmt ", 4))
		{
			if(tlen < 0x10)
			{
				eprintf("wav_load: fmt tag too small\n");
				fclose(fp);
				if(data != NULL) free(data);
				return NULL;
			}

			uint8_t c_fmt[16];
			fread(c_fmt, 1, 16, fp);
			fseek(fp, tlen - 16, SEEK_CUR);
			w_fmt_codec = ((int)(c_fmt[0]))
				| (((int)(c_fmt[1])) << 8);
			w_fmt_chns = ((int)(c_fmt[2]))
				| (((int)(c_fmt[3])) << 8);
			freq = ((int)(c_fmt[4]))
				| (((int)(c_fmt[5])) << 8)
				| (((int)(c_fmt[6])) << 16)
				| (((int)(c_fmt[7])) << 24);
			/*w_fmt_bytesec = ((int)(c_fmt[8]))
				| (((int)(c_fmt[9])) << 8)
				| (((int)(c_fmt[10])) << 16)
				| (((int)(c_fmt[11])) << 24);*/
			w_fmt_balign = ((int)(c_fmt[12]))
				| (((int)(c_fmt[13])) << 8);
			w_fmt_bps = ((int)(c_fmt[14]))
				| (((int)(c_fmt[15])) << 8);
		} else if(!memcmp(tbuf, "data", 4)) {
			data = realloc(data, len + tlen);
			fread(data + len, 1, tlen, fp);
			len += tlen;
		} else {
			fseek(fp, tlen, SEEK_CUR);
		}
	}

	fclose(fp);

	if(freq == 0)
	{
		eprintf("wav_load: no fmt tag\n");
		if(data != NULL) free(data);
		return NULL;
	}

	if(data == NULL)
	{
		eprintf("wav_load: no data tag\n");
		return NULL;
	}

	if(w_fmt_codec != 1)
	{
		eprintf("wav_load: unsupported codec 0x%04X\n", w_fmt_codec);
		if(data != NULL) free(data);
		return NULL;
	}

	if(w_fmt_bps != 8 && w_fmt_bps != 16)
	{
		eprintf("wav_load: unsupported bit depth %i\n", w_fmt_bps);
		if(data != NULL) free(data);
		return NULL;
	}

	if(w_fmt_chns != 1 && w_fmt_chns != 2)
	{
		eprintf("wav_load: unsupported channel count %i\n", w_fmt_chns);
		if(data != NULL) free(data);
		return NULL;
	}

	if(w_fmt_balign == 0 || len % w_fmt_balign != 0)
	{
		eprintf("wav_load: data does not conform to block alignment\n");
		if(data != NULL) free(data);
		return NULL;
	}

	dlen = len / w_fmt_balign;
	wav = lua_newuserdata(L, sizeof(wav_t)
		+ dlen * w_fmt_chns * sizeof(uint16_t));
	wav->svtail = NULL;
	wav->freq = freq;
	wav->chns = w_fmt_chns;
	wav->lplen = 0;
	wav->len = dlen;
	
	int i, j;
	for(i = 0; i < dlen; i++)
	{
		for(j = 0; j < w_fmt_chns; j++)
		{
			uint8_t *src_smp = data
				+ i * w_fmt_balign
				+ (j * w_fmt_balign) / w_fmt_chns;
			int16_t *dst_smp = wav->data
				+ i * w_fmt_chns
				+ j;

			switch(w_fmt_bps)
			{
				case 8:
					*dst_smp = (((int)*src_smp) * 0x0101) - 0x8000;
					break;
				case 16:
					*dst_smp = ((int)src_smp[0])
						| (((int)src_smp[1]) << 8);
					break;
				default:
					eprintf("EDOOFUS: w_fmt_bps should be filtered out by now!\n");
					fflush(stdout);
					abort();
					break;
			}
		}
	}

	free(data);

	lua_newtable(L);
	lua_pushcfunction(L, lmf_wav_gc);
	lua_setfield(L, -2, "__gc");
	lua_setmetatable(L, -2);

	return wav;
}

// Plays a .wav sound.
// Returns the voice pointer userdata on the Lua stack provided the result isn't NULL.
voice_t *wav_play(lua_State *L, wav_t *wav, float freq, float offs, float lvol, float rvol)
{
	if(wav == NULL) return NULL;

	voice_t *v = malloc(sizeof(voice_t));

	v->svprev = NULL;
	v->svtail = wav->svtail;
	if(wav->svtail != NULL) wav->svtail->svprev = v;
	wav->svtail = v;

	v->pprev = NULL;
	v->ptail = voice_chain;
	if(voice_chain != NULL) voice_chain->pprev = v;
	voice_chain = v;

	v->wav = wav;
	v->freq = freq;
	v->offs = offs;
	v->lvol = lvol;
	v->rvol = rvol;

	voice_t **ret = lua_newuserdata(L, sizeof(voice_t *));
	v->ud = ret;
	*ret = v;

	lua_newtable(L);
	lua_pushcfunction(L, lmf_voice_gc);
	lua_setfield(L, -2, "__gc");
	lua_setmetatable(L, -2);

	return v;
}

// wav.load((str)fname): Loads a RIFF WAVE file.
int lf_wav_load(lua_State *L)
{
	int top = lua_gettop(L);
	if(top < 1) return luaL_error(L, "not enough arguments for wav.load");

	const char *fname = lua_tostring(L, 1);

	wav_t *wav = wav_load(L, fname);
	if(wav == NULL)
		return luaL_error(L, "failed to parse wav file %s", fname);

	return 1;
}

// wav.play((wav)w, (float)lvol=1, (float)rvol=lvol, (float)freq=1, (float)offs=0):
// Plays a sound. freq is a frequency multiplier.
// offs is in seconds relative to freq=1. It can be negative to add some leading silence.
// Returns a voice pointer userdata.
int lf_wav_play(lua_State *L)
{
	int top = lua_gettop(L);
	if(top < 1) return luaL_error(L, "not enough arguments for wav.play");

	if(SDL_LockMutex(mtx_play) != 0)
	{
		eprintf("wav.play: could not lock mutex!\n");
		return luaL_error(L, "could not lock mutex for wav.play");
	}

	wav_t *wav = lua_touserdata(L, 1);
	if(wav == NULL) return luaL_error(L, "expected userdata for wav.play");
	double lvol = (top < 2 ? 1.0 : lua_tonumber(L, 2));
	double rvol = (top < 3 ? lvol : lua_tonumber(L, 3));
	double freq = (top < 4 ? 1.0 : lua_tonumber(L, 4));
	double offs = (top < 5 ? 0.0 : lua_tonumber(L, 5));

	offs *= (double)(wav->freq);
	freq *= (double)(wav->freq);

	//printf("v %f %f %f %f\b", offs, freq, lvol, rvol);
	voice_t *v = wav_play(L, wav, freq, offs, lvol, rvol);
	if(v == NULL)
		return 0; // don't return a voice pointer

	SDL_UnlockMutex(mtx_play);

	return 1;
}
