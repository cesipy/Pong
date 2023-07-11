CC = gcc
CFLAGS = -Iinclude/SDL2
LDFLAGS = -Llib
LIBS = -lSDL2 

all: testing_with_sdl

testing_with_sdl: testing_with_sdl.c
	$(CC) $(CFLAGS) $(LDFLAGS) testing_with_sdl.c -o test $(LIBS)


