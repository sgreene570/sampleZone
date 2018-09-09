/*
* Hghway to the sample zone.
* Work in progress
* @sgreene570
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <fcntl.h>
#include <ncurses.h>
#include <unistd.h>
#include "playback.h"

#define WINDOW_HEIGHT 20
#define WINDOW_WIDTH 30
#define OFFSET 1
#define SAMPLE_MARKERS "0123456789"

typedef struct audioFile_t {
    char *fileName;
    int pitchAdjust;
} audioFile;

double wavLength(u_int32_t wavSize, u_int32_t byteRate) {
    return (double) wavSize / byteRate;
}

void *playFile(void *file) {
    char *fileName = ((audioFile *)file)->fileName;
    int fd = open(fileName, O_RDONLY);
    // Read wav file header (44 bytes long)
    wavHeader *header =  calloc(1, 44);
    int out = read(fd, header, 44);
    if (out != 44) {
        printf("Error reading wav header\n");
        return NULL;
    }
    double length = wavLength(header->subChunk2Size, header->byteRate);
    // Debug wav header
    // printf("%u %u %f\n", header->subChunk2Size, header->byteRate, length);
    playback(header->sampleRate + (((audioFile *) file)->pitchAdjust * 500), header->numChannels, length, fd);
    pthread_exit(NULL);
    close(fd);
}

static WINDOW *create_newwin(int height, int width, int starty, int startx) {
    WINDOW *local_win = newwin(height, width, starty, startx);
    box(local_win, 0, 0);
    keypad(local_win, TRUE);
    return (local_win);
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
    for (int y = OFFSET; y < WINDOW_HEIGHT + OFFSET; y++) {
        for (int x = OFFSET; x < WINDOW_WIDTH + OFFSET - 1; x++) {
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

// Ncurses startup calls
void initCurses() {
    initscr();
    cbreak();
    keypad(stdscr, TRUE);
    noecho();
}

int main(int argc, char *argv[]) {
    int numFiles = argc - 1;
    char *fileNames[numFiles];
    // Parse file name input
    for(int i = 1; i < argc; i++) {
        fileNames[i - 1] = argv[i];
        fprintf(stderr, "%s\n", argv[i]);
    }

    // Audio file structs
    audioFile *files = initFiles(numFiles, fileNames);

    // Ncurses code
    initCurses();
    int x = OFFSET;
    int y = OFFSET;
    char ch;
    refresh();
    WINDOW *win = create_newwin(WINDOW_HEIGHT, WINDOW_WIDTH, y, x);
    wrefresh(win);

    // Print file names and symbols
    mvprintw(WINDOW_HEIGHT + 1, 1, "Loaded Samples:\n");
    for(int i = 0; i < numFiles; i++) {
        mvprintw(WINDOW_HEIGHT + i  + 2, 1, "%d: %s (Pitch Adjust: %d)",
            i, fileNames[i], files[i].pitchAdjust);
    }

    // Play back vars
    int tempo = 120;

    // Print usage info left of the game grid
    mvprintw(2, WINDOW_WIDTH + 3, "Start/Stop sequence: Spacebar\n");
    mvprintw(3, WINDOW_WIDTH + 3, "Inc/Dec tempo: +/-\n");
    mvprintw(4, WINDOW_WIDTH + 3, "Tempo: %d\n", tempo);

    refresh();
    wmove(win, y, x);
    wrefresh(win);
    while((ch = wgetch(win)) != 'q') {
        if (checkSymbol(ch, SAMPLE_MARKERS, sizeof(SAMPLE_MARKERS))) {
            mvwaddch(win, y, x, ch);
            if(x + 1 < OFFSET + WINDOW_WIDTH - 2) {
                x++;
            } else {
                y++;
                x = OFFSET;
            }
        } else if (ch == ' ') {
            wmove(win, y, x);
            playPattern(win, files, tempo);
        } else {
            // Vim arrow controls with grid boundaries in mind
            switch(ch) {
                case 'h':
                    if(x - 1 > OFFSET - 1) {
                        x--;
                    }
                    break;
                case 'l':
                    if(x + 1 < OFFSET + WINDOW_WIDTH - 2) {
                        x++;
                    }
                    break;
                case 'j':
                    if(y + 1 < OFFSET + WINDOW_HEIGHT - 2) {
                        y++;
                    }
                    break;
                case 'k':
                    if(y - 1 > OFFSET - 1) {
                        y--;
                    }
                    break;
                case '+':
                    tempo++;
                    mvprintw(4, WINDOW_WIDTH + 10, "%d\n", tempo);
                    refresh();
                    break;
                case '-':
                    tempo--;
                    mvprintw(4, WINDOW_WIDTH + 10, "%d\n", tempo);
                    refresh();
                    break;
                }
            wmove(win, y, x);
        }
        // Refresh grid after each cursor move or screen print (important)
        wrefresh(win);
    }

    endwin();
    return 0;
}
