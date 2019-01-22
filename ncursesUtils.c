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
    for(int i = 0; i < windowWidth - 1; i += 4) {
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
        mvprintw(windowHeight + i  + 2, 1, "%d: %s (Pitch Adjust: %d)",
            i, fileNames[i], files[i].pitchAdjust);
        attroff(A_STANDOUT);
    }
}
