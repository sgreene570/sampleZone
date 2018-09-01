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
    // Debug wav header
    // printf("%u %u %f\n", header->subChunk2Size, header->byteRate, length);
    playback(header->sampleRate, header->numChannels, length, file);
    pthread_exit(NULL);
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

void playPattern(WINDOW *win, char *files[], int tempo) {
    nodelay(win, TRUE);
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
            int fd;
            if (checkSymbol(ch, SAMPLE_MARKERS, sizeof(SAMPLE_MARKERS))) {
                wrefresh(win);
                fd = open(files[(int) ch - '0'], O_RDONLY);
                pthread_create(&thread, NULL, playFile, &fd);
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
    char *files[numFiles];
    // Parse file name input
    for(int i = 1; i < argc; i++) {
        files[i - 1] = argv[i];
        fprintf(stderr, "%s\n", argv[i]);
    }

    pthread_t threads[numFiles];
    int fd;

    /*
    for(int i = 0; i < numFiles; i++) {
        fd = open(argv[i], O_RDONLY);
        pthread_create(&threads[i], NULL, playFile, &fd);
    }
    pthread_exit(NULL);
    */

    // Ncurses code
    initCurses();
    int x = OFFSET;
    int y = OFFSET;
    char ch;
    refresh();
    WINDOW *win = create_newwin(WINDOW_HEIGHT, WINDOW_WIDTH, y, x);
    wrefresh(win);
    // Print file names and symbols
    mvprintw(WINDOW_HEIGHT + 1, 0, "Loaded Samples:\n");
    for(int i = 1; i <= numFiles; i++) {
        mvprintw(WINDOW_HEIGHT + i  + 1, 0, "%d: %s", i - 1, argv[i]);
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
        } else if (ch == ' ') {
            playPattern(win, files, tempo);
            wmove(win, y, x);
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
