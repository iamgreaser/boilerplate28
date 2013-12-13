/*
Boilerplate 28: GreaseMonkey's boilerplate code for Ludum Dare #28
2013, Ben "GreaseMonkey" Russell - Public Domain
*/

#include "common.h"

int wav_dev = 0;

SDL_mutex *mtx_play = NULL;
SDL_mutex *mtx_sackit = NULL;
sackit_playback_t *sackit = NULL;
it_module_t *it_mod_play = NULL;
volatile int it_mod_luafree = 0;
voice_t *voice_chain = NULL;

// SDL audio callback.

SDL_AudioSpec wav_want, wav_have;
void cb_wav_update(void *userdata, Uint8 *stream, int len)
{
	int i;
	int slen = len / 4;
	int16_t *d = (int16_t *)stream;

	printf("slen %i\n", slen);
	for(i = 0; i < slen; i++)
		d[2*i+0] = d[2*i+1] = 0;
	
	if(SDL_LockMutex(mtx_play) == 0)
	{
		if(sackit == NULL)
		{
			// Check if we need to free the module
			if(it_mod_play != NULL)
			{
				if(it_mod_luafree)
				{
					free(it_mod_play);
					it_mod_luafree = 0;
				}

				it_mod_play = NULL;
			}
		} else {
			// TODO: update buffer
		}

		voice_t *v;
		for(v = voice_chain; v != NULL; v = v->ptail)
		{
			// TODO: mix in voices
		}

		SDL_UnlockMutex(mtx_play);
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
	wav_want.samples = WAV_SAMPLES;
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

// wav __gc metatable function.
int lmf_wav_gc(lua_State *L)
{
	voice_t *v, *v2;
	wav_t *wav = lua_touserdata(L, 1);

	if(SDL_LockMutex(mtx_play) == 0)
	{
		for(v = v2 = wav->svtail; v2 != NULL; v = v2)
		{
			v2 = v->svtail;

			if(v->pprev != NULL) v->pprev->ptail = v->ptail;
			if(v->ptail != NULL) v->ptail->pprev = v->pprev;
			// svprev and svtail don't matter - we're clearing that whole chain

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
	return wav;
}

// Plays a .wav sound.
// Returns the voice userdata on the Lua stack provided the result isn't NULL.
voice_t *wav_play(lua_State *L, wav_t *wav)
{
	if(wav == NULL) return NULL;

	// TODO!
	return NULL;
}

