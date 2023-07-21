CC = gcc
CFLAGS = -Iinclude/SDL2
LDFLAGS = -Llib
LIBS = -lSDL2 -LSDL2_ttf

all: testing_with_sdl

testing_with_sdl: testing_with_sdl.c
	$(CC) $(CFLAGS) $(LDFLAGS) testing_with_sdl.c -o test $(LIBS)


pong: pong.c
	$(CC) $(CFLAGS) $(LDFLAGS) pong.c -o pong $(LIBS)


.PHONY:
clean:
	rm -f pong test


