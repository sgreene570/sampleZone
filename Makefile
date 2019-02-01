CC=gcc
CFLAGS=-I.

sampleZone: sampleZone.c playback.c playback.h ncursesUtils.c ncursesUtils.h
	$(CC) -Wall -Wextra -pedantic -o sampleZone sampleZone.c playback.c ncursesUtils.c -lasound -lncurses -pthread -std=gnu99

clean:
	rm sampleZone
