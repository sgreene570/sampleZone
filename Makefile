CC=gcc
CFLAGS=-I.

sampleZone: sampleZone.c playback.c playback.h
	$(CC) -Wall -Wextra -pedantic -lasound -pthread -lncurses -std=gnu99 -o sampleZone sampleZone.c playback.c ncursesUtils.c

clean:
	rm sampleZone
