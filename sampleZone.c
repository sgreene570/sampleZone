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

void *playFile(void *fd) {
    playback(44100, 2, 1, (int)fd);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    int numFiles = argc - 1;
    char *args[numFiles];
    for(int i = 1; i < argc; i++) {
        args[i - 1] = argv[i];
        printf("%s\n", args[i - 1]);
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
