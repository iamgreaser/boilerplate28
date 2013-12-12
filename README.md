Some boilerplate code I wrote for Ludum Dare #28.

TODO:
- Expose more OpenGL stuff to Lua
- Support keyboard/mouse
- Add sound
- Actually use sackit

C side contains:
- libsackit: For .it playback.
- linmath.h: Available here: https://github.com/datenwolf/linmath.h

Lua API contains:
- blob: Triangle fans of a single colour.
- GL: Part of the OpenGL API.
- M: Matrix manipulation + loading into OpenGL.
- sys: Little miscellaneous tidbits.

Lua side contains:
- Nothing fancy at the moment.

Needed extra libraries:
- Lua 5.1 (**NOT 5.2!**)
- SDL 2.0 (**NOT 1.2!** although support for that might happen)
- GLEW (pretty much any version should do, I'm using 1.6.0)

