# Compiler stuff
CC = gcc
CFLAGS += -O2 -std=c11
CFLAGS += -Iinclude
LDFLAGS += -Llib_win
LDLIBS += -lm -lglfw3 -lgdi32 -lopengl32

OUTPUT_OPTION += -MMD
WARNING_OPTIONS = -Wpedantic -Werror -Wfatal-errors

srcdir = src
builddir = build

# Default to build all
all:

# Track files
bin = ksweeper

src = $(wildcard $(srcdir)/*.c)
obj = $(src:src/%.c=$(builddir)/%.o)
dep = $(obj:.o=.d)

# Default is build all the things (just binary for now)
all: $(bin)

# Build .o from .c with strict warnings and errors
$(builddir)/%.o: $(srcdir)/%.c
	$(CC) -c $< $(CPPFLAGS) $(CFLAGS) $(WARNING_OPTIONS) $(OUTPUT_OPTION)

# Don't be so strict when building glad
$(builddir)/glad.o: $(srcdir)/glad.c
	$(CC) -c $< $(CPPFLAGS) $(CFLAGS) -Wno-stringop-overflow $(OUTPUT_OPTION)

# Link the binary
$(bin): $(obj)
	$(CC) $^ $(LDLIBS) $(LDFLAGS) -o $@

# Remove generated files
.PHONY: clean
clean:
	rm -f $(bin) $(obj) $(dep)

# Shortcut to build and run because I hate hate HATE the Windows commmand
# prompt it is so gosh-darn hard to run an executable and the stupid
# backslashes AAAAGGGGHHHH
.PHONY: run
run: $(bin)
	ksweeper

-include $(dep)
