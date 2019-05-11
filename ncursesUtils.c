/*
 * Sample Zone ncurses utils
 * @sgreene570
*/


#include <stdlib.h>
#include <ncurses.h>
#include "playback.h"

// Ncurses startup calls
void initCurses() {
    initscr();
    cbreak();
    keypad(stdscr, TRUE);
    noecho();
}

WINDOW *createWindow(int height, int width, int starty, int startx) {
    WINDOW *local_win = newwin(height, width, starty, startx);
    box(local_win, 0, 0);
    keypad(local_win, TRUE);
    return (local_win);
}

// Prints measure markers in terminal ignoring windows
void printMeasureMarkers(int currMeasure, int windowWidth) {
    for(int i = 0; i < windowWidth - 2; i += 4) {
        if(i == currMeasure * 4) {
            attron(A_STANDOUT);
        }
        mvaddch(0, 2 + i, 'v');
        attroff(A_STANDOUT);
    }
    refresh();
}

void printSamples(audioFile *files, char *fileNames[], int highlightIndex, int numFiles, int windowHeight) {
    mvprintw(windowHeight + 1, 1, "Loaded samples:\n");
    for(int i = 0; i < numFiles; i++) {
        if (i == highlightIndex) {
            attron(A_STANDOUT);
        }
        move(windowHeight + i + 2, 0);
        clrtoeol();

        if (i > 9) {
            mvprintw(windowHeight + i + 2, 1, "%0x: %s (Pitch Adjust: %d)", 
                i, fileNames[i], files[i].pitchAdjust);
        } else {
            mvprintw(windowHeight + i  + 2, 1, "%d: %s (Pitch Adjust: %d)",
                i, fileNames[i], files[i].pitchAdjust);
        }
        attroff(A_STANDOUT);
    }
}

//You're probably going to need an index
void sampleError(char* errorMessage, int windowHeight) {
    move(windowHeight - 1, 45);
    clrtoeol();
    attron(A_STANDOUT);
    mvprintw(windowHeight - 2, 45, "--ERROR--");
    mvprintw(windowHeight - 1, 45, errorMessage);
    attroff(A_STANDOUT);
}

void clearErrors(int windowHeight) {
    move(windowHeight - 1, 45);
    clrtoeol();
    move(windowHeight - 2, 45);
    clrtoeol();
}
