all: pong

pong: pong.c
	gcc -Iinclude $(shell pkg-config --cflags sdl2 SDL2_ttf SDL2_mixer) pong.c -o pong $(shell pkg-config --libs sdl2 SDL2_ttf SDL2_mixer)

.PHONY: clean
clean:
	rm -f pong test
