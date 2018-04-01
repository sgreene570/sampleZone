/*
* Hghway to the sample zone.
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include "playback.h"

#define PLAYBACK_RATE 44100
#define AUDIO_CHANNELS 2

typedef struct wavHeader_t {
    u_int32_t chunkID;
    u_int32_t chunkSize;
    u_int32_t format;
    u_int32_t subChunk1ID;
    u_int32_t subChunk1Size;
    u_int16_t audioFormat;
    u_int16_t numChannels;
    u_int32_t sampleRate;
    u_int32_t byteRate;
    u_int16_t blockAlign;
    u_int16_t bitsPerSample;
    u_int32_t subChunk2ID;
    u_int32_t subChunk2Size;
} wavHeader;

double wavLength(u_int32_t wavSize, u_int32_t byteRate) {
    return (double) wavSize / byteRate;
}

void *playFile(void *fd) {
    int file = (int) fd;
    // Read wav file header (44 bytes long)
    wavHeader *header =  calloc(1, 44);
    int out = read(file, header, 44);
    if (out != 44) {
        printf("Error reading wav header\n");
        return;
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
        pthread_create(&threads[i], NULL, playFile, (void *)fd);
    }
    pthread_exit(NULL);
    return 0;
}
