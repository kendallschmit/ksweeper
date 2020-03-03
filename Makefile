# Compiler stuff
CC ?= gcc
CFLAGS += -O2 -std=c11
CFLAGS += -Iinclude -Ibuild
LDFLAGS +=
LDLIBS += -lm -lglfw3

OUTPUT_OPTION += -MMD
WARNING_OPTIONS = -Wpedantic -Werror -Wfatal-errors

srcdir = src
builddir = build

resgen = ./resgen/resgen

# Track files
bin := ksweeper

-include config.mk

ifeq "$(platform)" "windows"
LDFLAGS += -mwindows -Llib_windows
LDLIBS += -lgdi32 -lopengl32
bin := $(bin).exe
endif

ifeq "$(platform)" "macos"
LDFLAGS += -Llib_macos -framework OpenGL -framework Cocoa -framework IOKit
endif

ifeq "$(build_type)" "release"
LDFLAGS += -s
endif

tga = $(wildcard res/tga/*.tga)
tga_c = $(builddir)/resgen_tga.c
tga_h = $(builddir)/resgen_tga.h
resource_src += $(tga_c)
resource_h += $(tga_h)

glsl = $(wildcard res/shader/*.glsl)
glsl_c = $(builddir)/resgen_shader.c
glsl_h = $(builddir)/resgen_shader.h
resource_src += $(glsl_c)
resource_h += $(glsl_h)

resource_gen = $(resource_src) $(resource_h)

src = $(wildcard $(srcdir)/*.c)
obj = $(src:src/%.c=$(builddir)/%.o) $(resource_src:%.c=%.o)
dep = $(obj:.o=.d)

# Default build binary
all: $(bin)

# Windows icon
ifeq "$(platform)" "windows"
ksweeper_res = $(builddir)/ksweeper.res.o
obj += $(ksweeper_res)
$(ksweeper_res): ksweeper.rc ksweeper.ico
	windres ksweeper.rc -O coff -o $(ksweeper_res)
endif

# Build .o from .c with strict warnings and errors
$(builddir)/%.o: $(srcdir)/%.c | $(resource_gen)
	$(CC) -c $< $(CPPFLAGS) $(CFLAGS) $(WARNING_OPTIONS) $(OUTPUT_OPTION)

# Build .o from .c in build dir  with strict warnings and errors
$(builddir)/%.o: $(build_dir)/%.c | $(resource_gen)
	$(CC) -c $< $(CPPFLAGS) $(CFLAGS) $(WARNING_OPTIONS) $(OUTPUT_OPTION)

# Don't be so strict when building glad
$(builddir)/glad.o: $(srcdir)/glad.c
	$(CC) -c $< $(CPPFLAGS) $(CFLAGS) -Wno-stringop-overflow $(OUTPUT_OPTION)

# Build tga resources
$(tga_h) $(tga_c): $(tga)
	$(resgen) $(tga_h) $(tga_c) resgen_tga $^

# Build glsl resources
$(glsl_h) $(glsl_c): $(glsl)
	$(resgen) $(glsl_h) $(glsl_c) resgen_shader $^

# Link the binary
$(bin): $(obj)
	$(CC) $^ $(LDLIBS) $(LDFLAGS) -o $@

# Remove generated files
.PHONY: clean
clean:
	rm -f $(resource_gen) $(bin) $(obj) $(dep)

# Shortcut to build and run because I hate hate HATE the Windows commmand
# prompt it is so gosh-darn hard to run an executable and the stupid
# backslashes AAAAGGGGHHHH
.PHONY: run
run: $(bin)
	./ksweeper

-include $(dep)
