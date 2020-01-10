CC = gcc
CFLAGS += -O2 -std=c11
CFLAGS += -Iinclude
LDFLAGS += -Llib_win
LDLIBS += -lm

src = $(wildcard *.c)
resgen: $(src)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^