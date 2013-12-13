Some boilerplate code I wrote for Ludum Dare #28.

TODO:
- Expose OpenGL's stenciling API

C side contains:
- libsackit: For .it playback.
- linmath.h: Available here: https://github.com/datenwolf/linmath.h

Lua API contains:
- blob: Triangle fans of a single colour.
- GL: Part of the OpenGL API.
- png: PNG loader + renderer.
- M: Matrix manipulation + loading into OpenGL.
- mus: ImpulseTracker module loading + playing.
- sys: Little miscellaneous tidbits.
- wav: Sound loading + playback.
- Hooks for: tick, render, keys, mouse buttons.

Lua side contains:
- Lollipop spam.

Needed extra libraries:
- Lua 5.1 (**NOT 5.2!**)
- SDL 2.0 (**NOT 1.2!** although support for that might happen)
- GLEW (pretty much any version should do, I'm using 1.6.0)
- ZLib (if you don't have this, you should probably get it)

Licensing:
- The PNG loader is licensed under a ZLib-style licence.
- The sdlkey library for Lua is licensed under a ZLib-style licence.
- Everything else where explicitly marked is public domain.
- The font is in the public domain.

