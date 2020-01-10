# Compiler stuff
CC = gcc
CFLAGS += -O2 -std=c11
CFLAGS += -Iinclude -Ibuild
LDFLAGS += -Llib_win
LDLIBS += -lm -lglfw3 -lgdi32 -lopengl32

OUTPUT_OPTION += -MMD
WARNING_OPTIONS = -Wpedantic -Werror -Wfatal-errors

srcdir = src
builddir = build

resgen = ./resgen/resgen

# Track files
bin := ksweeper

-include config.mk

ifeq "$(platform)" "windows"
LDFLAGS += -mwindows
bin := $(bin).exe
endif

ifeq "$(build_type)" "release"
LDFLAGS += -s
endif

tga = $(wildcard res/tga/*.tga)
tga_h = $(builddir)/tga_names.h $(builddir)/tga.h
resource_src = $(tga_h)

src = $(wildcard $(srcdir)/*.c)
obj = $(src:src/%.c=$(builddir)/%.o)
dep = $(obj:.o=.d)

# Default build binary
all: $(bin)

# Build .o from .c with strict warnings and errors
$(builddir)/%.o: $(srcdir)/%.c | $(resource_src)
	$(CC) -c $< $(CPPFLAGS) $(CFLAGS) $(WARNING_OPTIONS) $(OUTPUT_OPTION)

# Build .o from .c in build dir  with strict warnings and errors
$(builddir)/%.o: $(build_dir)/%.c | $(resource_src)
	$(CC) -c $< $(CPPFLAGS) $(CFLAGS) $(WARNING_OPTIONS) $(OUTPUT_OPTION)

# Don't be so strict when building glad
$(builddir)/glad.o: $(srcdir)/glad.c
	$(CC) -c $< $(CPPFLAGS) $(CFLAGS) -Wno-stringop-overflow $(OUTPUT_OPTION)

# Build tga.h and tga.c from .tga
$(tga_h): $(tga)
	$(resgen) $(tga_h) tga $^

# Link the binary
$(bin): $(obj)
	$(CC) $^ $(LDLIBS) $(LDFLAGS) -o $@

# Remove generated files
.PHONY: clean
clean:
	rm -f $(resource_src) $(bin) $(obj) $(dep)

# Shortcut to build and run because I hate hate HATE the Windows commmand
# prompt it is so gosh-darn hard to run an executable and the stupid
# backslashes AAAAGGGGHHHH
.PHONY: run
run: $(bin)
	ksweeper

-include $(dep)
