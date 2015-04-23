CC=gcc
SDL_FLAGS=-lSDL -lSDL_image -lSDL_ttf
CFLAGS=${SDL_FLAGS} -std=c99 -Wall -pedantic -ggdb3

main: main.c

clean:
	rm main

test:
	./main & sleep 1 && killall -9 main
