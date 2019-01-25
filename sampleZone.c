/*
* Hghway to the sample zone.
* Work in progress
* @sgreene570
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <fcntl.h>
#include <ncurses.h>
#include <unistd.h>
#include "playback.h"
#include "ncursesUtils.h"

#define DEFAULT_WINDOW_HEIGHT 28
#define DEFAULT_WINDOW_WIDTH 42
#define WINDOW_OFFSET 1
#define SAMPLE_MARKERS "0123456789"

static int height = DEFAULT_WINDOW_HEIGHT;
static int width = DEFAULT_WINDOW_WIDTH;

double wavLength(u_int32_t wavSize, u_int32_t byteRate) {
    return (double) wavSize / byteRate;
}

void *playFile(void *file) {
    char *fileName = ((audioFile *)file)->fileName;
    int fd = open(fileName, O_RDONLY);
    // Read wav file header (44 bytes long)
    wavHeader *header = calloc(1, 44);
    int out = read(fd, header, 44);
    if (out != 44) {
        //Put sampleError here
        sampleError(fileName, 0, DEFAULT_WINDOW_HEIGHT);
        //printf("Error reading wav header\n");
        return NULL;
    }
    double length = wavLength(header->subChunk2Size, header->byteRate);
    // Debug wav header
    // printf("%u %u %f\n", header->subChunk2Size, header->byteRate, length);
    playback(header->sampleRate + (((audioFile *) file)->pitchAdjust * 500), header->numChannels, length, fd);
    pthread_exit(NULL);
    close(fd);
}

bool checkSymbol(char input, char *symbols, int numSymbols) {
    for (int i = 0; i < numSymbols; i++) {
        if (symbols[i] == input) {
            return true;
        }
    }
    return false;
}

audioFile *initFiles(int numFiles, char *fileNames[]) {
    audioFile *files = calloc(numFiles, sizeof(audioFile));
    for(int i = 0; i < numFiles; i++) {
        files[i].fileName = fileNames[i];
        files[i].pitchAdjust = 0;
    }
    return files;
}

void playPattern(WINDOW *win, audioFile *files, int tempo) {
    // Make wgetch a non-blocking call
    nodelay(win, TRUE);
    // Play through grid
    for (int y = WINDOW_OFFSET; y < height + WINDOW_OFFSET; y++) {
        for (int x = WINDOW_OFFSET; x < width + WINDOW_OFFSET - 1; x++) {
            printMeasureMarkers((x - WINDOW_OFFSET - 1) / 4, width);
            wrefresh(win);
            if (wgetch(win) == ' ') {
                return;
            }
            // Calculates seconds per beat
            double spb = 60 / (1.0 * tempo);
            usleep(spb * 1000000);
            wmove(win, y, x);
            char ch = winch(win) & A_CHARTEXT;
            wrefresh(win);
            pthread_t thread;
            if (checkSymbol(ch, SAMPLE_MARKERS, sizeof(SAMPLE_MARKERS))) {
                wrefresh(win);
                pthread_create(&thread, NULL, playFile, &files[(int) ch - '0']);
            }
        }

    }
    pthread_exit(NULL);
}

static void printUsage() {
    printf("Usage: sampleZone [-w] [-h] <file1.wav> <file2.wav> ...\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    int firstArgument = 1;
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--height")) {
            if (i == argc - 1) {
                printUsage();
            }
            else {
                height = atoi(argv[i + 1]);
                if (!height) {
                    printUsage();
                }
                firstArgument = i + 2;
            }
        }
        else if (!strcmp(argv[i], "-w") || !strcmp(argv[i], "--width")) {
            if (i == argc - 1) {
                printUsage();
            }
            else {
                width = atoi(argv[i + 1]);
                if (!width) {
                    printUsage();
                }
                firstArgument = i + 2;
            }
        }
    }
    int numFiles = argc - firstArgument;
    char *fileNames[numFiles];

    if (numFiles == 0) {
        printf("No input files specified\n");
        return 0;
    }
    // Parse file name input
    for(int i = firstArgument; i < argc; i++) {
        fileNames[i - firstArgument] = argv[i];
    }

    //TODO: add inout for WINDOW_SIZE and stuff  

    // Audio file structs
    audioFile *files = initFiles(numFiles, fileNames);

    // Ncurses code
    initCurses();
    int x = WINDOW_OFFSET;
    int y = WINDOW_OFFSET;
    char ch;

    // Print initial grid
    refresh();
    WINDOW *win = createWindow(height, width, y, x);
    wrefresh(win);

    // Print measure markers
    printMeasureMarkers(0, width);

    // Print sample info
    printSamples(files, fileNames, 0, numFiles, height);

    // Play back vars
    int tempo = 120;

    // Print usage info left of the game grid
    mvprintw(2, width + 3, "Start/Stop sequence: Spacebar\n");
    mvprintw(3, width + 3, "Inc/Dec tempo: +/-\n");
    mvprintw(4, width + 3, "Tempo: %d\n", tempo);
    mvprintw(5, width + 3, "Adjust pitch: </>\n");
    mvprintw(6, width + 3, "Select file: Up/Down arrow\b");

    // Initial select file for tempo adjust
    int selectedFileIndex = 0;
    audioFile *selectedFile = &files[selectedFileIndex];

    //Check files for errors (TODO: More robust error checking code. Make this not broken)
//    char badFiles[numFiles];
//    for (int i = 1; i <= numFiles; i++) {
//        int fd = open(fileNames[i], O_RDONLY);
//        // Read wav file header (44 bytes long)
//        wavHeader *header = calloc(1, 44);
//        int out = read(fd, header, 44);
//        if (out != 44) {
//            badFiles[i-1] = fileNames[i];
//            //Put sampleError here: sampleError(fileName, 0, WINDOW_HEIGHT);
//            //printf("Error reading wav header\n");
//            return NULL;
//        }
//        //sampleError(badFiles, 0, WINDOW_HEIGHT);
//    }

    refresh();
    wmove(win, y, x);
    wrefresh(win);
    while((ch = wgetch(win)) != 'q') {
        if (checkSymbol(ch, SAMPLE_MARKERS, sizeof(SAMPLE_MARKERS))) {
            mvwaddch(win, y, x, ch);
            if(x + 1 < WINDOW_OFFSET + width - 2) {
                x++;
            } else {
                y++;
                x = WINDOW_OFFSET;
            }
        } else if (ch == ' ') {
            wmove(win, y, x);
            playPattern(win, files, tempo);
        } else {
            // Vim arrow controls with grid boundaries in mind
            switch(ch) {
                case 'h':
                    if(x - 1 > WINDOW_OFFSET - 1) {
                        x--;
                    }
                    break;
                case 'l':
                    if(x + 1 < WINDOW_OFFSET + width - 2) {
                        x++;
                    }
                    break;
                case 'j':
                    if(y + 1 < WINDOW_OFFSET + width - 2) {
                        y++;
                    }
                    break;
                case 'k':
                    if(y - 1 > WINDOW_OFFSET - 1) {
                        y--;
                    }
                    break;
                case '+':
                    tempo++;
                    mvprintw(4, width + 10, "%d\n", tempo);
                    refresh();
                    break;
                case '-':
                    tempo--;
                    mvprintw(4, width + 10, "%d\n", tempo);
                    refresh();
                    break;
                case '<':
                    selectedFile->pitchAdjust -= 1;
                    printSamples(files, fileNames, selectedFileIndex, numFiles, height);
                    refresh();
                    break;
                case '>':
                    selectedFile->pitchAdjust += 1;
                    printSamples(files, fileNames, selectedFileIndex, numFiles, height);
                    refresh();
                    break;
                case (char)KEY_UP:
                    // Up arrow handling
                    if(selectedFileIndex != 0){
                        selectedFileIndex--;
                        selectedFile = &files[selectedFileIndex];
                        printSamples(files, fileNames, selectedFileIndex, numFiles, height);
                        refresh();
                    }
                    break;
                case (char)KEY_DOWN:
                    // Down arrow handling
                    if(selectedFileIndex + 1 < numFiles){
                        selectedFileIndex++;
                        selectedFile = &files[selectedFileIndex];
                        printSamples(files, fileNames, selectedFileIndex, numFiles, height);
                        refresh();
                    }
                    break;
                }
            wmove(win, y, x);
        }
        // Refresh grid after each cursor move or screen print (important)
        wrefresh(win);
    }
    delwin(win);
    endwin();
    return EXIT_SUCCESS;
}
