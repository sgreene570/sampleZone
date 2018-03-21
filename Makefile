sampleZone: sampleZone.c playback.c playback.h
	gcc -Wall -Wextra -pedantic -lasound -pthread -o sampleZone sampleZone.c playback.c -I.

clean:
	rm sampleZone
