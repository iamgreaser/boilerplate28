OBJDIR_ROOT = build
OBJDIR = build/posix
SRCDIR = src

OBJDIR_LIST = $(OBJDIR) $(OBJDIR)/sackit

INCLUDES = src/common.h src/sackit/sackit.h
SACKIT_INCLUDES = src/sackit/sackit.h src/sackit/sackit_internal.h
OBJS = \
	$(OBJDIR)/sackit/effects.o \
	$(OBJDIR)/sackit/fixedmath.o \
	$(OBJDIR)/sackit/litemixer.o \
	$(OBJDIR)/sackit/objects.o \
	$(OBJDIR)/sackit/playroutine.o \
	$(OBJDIR)/sackit/playroutine_effects.o \
	$(OBJDIR)/sackit/playroutine_nna.o \
	$(OBJDIR)/sackit/tables.o \
	\
	$(OBJDIR)/blob.o \
	$(OBJDIR)/gl.o \
	$(OBJDIR)/lua.o \
	$(OBJDIR)/matrix.o \
	$(OBJDIR)/png.o \
	\
	$(OBJDIR)/main.o

CFLAGS_SDL=`sdl2-config --cflags`
CFLAGS = -g -O2 -Isrc/ -Isrc/sackit/ $(CFLAGS_SDL) -Wall -Wextra -Wno-unused-parameter
LDFLAGS = -g
LIBS_SDL = `sdl2-config --libs`
LIBS_Lua = -llua-5.1
LIBS_GL = -lGL -lGLEW
LIBS = -lm $(LIBS_SDL) $(LIBS_Lua) -lz $(LIBS_GL)

BINNAME = boilerplate28

MKDIR = mkdir
MKDIR_F = mkdir -p
RM = rm
RM_F = rm -f
TOUCH = touch

all: $(BINNAME)

clean:
	$(RM_F) $(OBJS)

$(BINNAME): $(OBJDIR_LIST) $(OBJS)
	@# hack to stop it mkdiring directories each time
	$(TOUCH) $(OBJDIR)
	$(TOUCH) $(OBJDIR)/sackit
	@# actual command
	$(CC) -o $(BINNAME) $(LDFLAGS) $(OBJS) $(LIBS)

$(OBJDIR)/sackit: $(OBJDIR)
	$(MKDIR_F) $(OBJDIR)/sackit

$(OBJDIR): $(OBJDIR_ROOT)
	$(MKDIR_F) $(OBJDIR)

$(OBJDIR_ROOT):
	$(MKDIR_F) $(OBJDIR_ROOT)

$(OBJDIR)/sackit/%.o: $(SRCDIR)/sackit/%.c $(SACKIT_INCLUDES)
	$(CC) -c -o $@ $(CFLAGS) $<

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(INCLUDES)
	$(CC) -c -o $@ $(CFLAGS) $<

