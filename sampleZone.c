/*
* Hghway to the sample zone.
* Work in progress
* @sgreene570
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include "playback.h"

double wavLength(u_int32_t wavSize, u_int32_t byteRate) {
    return (double) wavSize / byteRate;
}

void *playFile(void *fd) {
    int file = *((int *) fd);
    // Read wav file header (44 bytes long)
    wavHeader *header =  calloc(1, 44);
    int out = read(file, header, 44);
    if (out != 44) {
        printf("Error reading wav header\n");
        return NULL;
    }
    double length = wavLength(header->subChunk2Size, header->byteRate);
    printf("%u %u %f\n", header->subChunk2Size, header->byteRate, length);
    playback(header->sampleRate, header->numChannels, length, file);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    int numFiles = argc - 1;
    char *args[numFiles];
    // Parse file name input
    for(int i = 1; i < argc; i++) {
        args[i - 1] = argv[i];
    }

    pthread_t threads[numFiles];
    int fd;

    for(int i = 0; i < numFiles; i++) {
        fd = open(args[i], O_RDONLY);
        pthread_create(&threads[i], NULL, playFile, &fd);
    }
    pthread_exit(NULL);
    return 0;
}
