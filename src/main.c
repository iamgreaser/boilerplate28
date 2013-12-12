/*
Boilerplate 28: GreaseMonkey's boilerplate code for Ludum Dare #28
2013, Ben "GreaseMonkey" Russell - Public Domain
*/

#include "common.h"

int sys_width = 800;
int sys_height = 600;
SDL_Window *window = NULL;
SDL_GLContext context;

// This is only necessary because Windows. Fuck you Windows.
void eprintf(const char *fmt, ...)
{
	va_list va;
	va_start(va, fmt);

#ifdef WIN32
	// Fuck you Windows.
	vprintf(fmt, va);
#else
	vfprintf(stderr, fmt, va);
#endif

	va_end(va);
}

// Gets the current time relative to some point.
int64_t get_time(void)
{
#ifdef WIN32
	// Fuck you Windows.
	return (int64_t)SDL_GetTicks();
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	int64_t sec = tv.tv_sec;
	int64_t usec = tv.tv_usec;
	sec *= (int64_t)1000000;
	return sec + usec;
#endif
}

// Set up SDL.
int init_sdl(void)
{
	window = SDL_CreateWindow("Boilerplate 28",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		sys_width,
		sys_height,

		0
		| SDL_WINDOW_OPENGL
		| SDL_WINDOW_SHOWN);
	
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 1); // go with whatever
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	context = SDL_GL_CreateContext(window);
	
	// blatantly stolen from here: http://glew.sourceforge.net/basic.html
	{
		GLenum err = glewInit();

		if (GLEW_OK != err)
		{
			eprintf("GLEW Error: %s\n", glewGetErrorString(err));
			return 1;
		}

		eprintf("Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
	}

	return 0;
}

int run_game(void)
{
	int quitflag = 0;

	while(!quitflag)
	{
		tick_lua(get_time());
		render_lua(get_time());
		SDL_GL_SwapWindow(window);

		SDL_Event ev;
		while(SDL_PollEvent(&ev) && !quitflag)
		switch(ev.type)
		{
			case SDL_QUIT:
				quitflag = 1;
				break;
		}
	}

	return 0;
}

// Main function.
int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_TIMER
		|SDL_INIT_VIDEO);

	atexit(SDL_Quit);
	
	return init_sdl() || init_lua() || run_game();
}

