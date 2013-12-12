Some boilerplate code I wrote for Ludum Dare #28.

TODO:
- Allow rendering of blobs
- Expose more OpenGL stuff to Lua
- Support hooks
- Actually use sackit

C side contains:
- blobs:
- libsackit: For .it playback.
- linmath.h: Available here: https://github.com/datenwolf/linmath.h

Lua side contains:
- Not much, really.

Needed extra libraries:
- Lua 5.1 (**NOT 5.2!**)
- SDL 2.0 (**NOT 1.2!** although support for that might happen)
- GLEW (pretty much any version should do, I'm using 1.6.0)

