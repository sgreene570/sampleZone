#ifndef NCURSESUTILS_H
#define NCURSESUTILS_H

//Window size
#define DEFAULT_WINDOW_HEIGHT 28
#define DEFAULT_WINDOW_WIDTH 42

// Ncurses init function
void initCurses();

// Creates ncurses window for main program grid
// Returns window as an ncurses window pointer
WINDOW *createWindow(int height, int width, int starty, int startx);

// Prints measure markers at top of terminal
void printMeasureMarkers(int currMeasure, int windowWidth);

// Prints file information
void printSamples(audioFile *files, char *fileNames[], int highlightIndex, int numFiles, int windowHeight);

// Prints an error on a file when stuff goes whack
void sampleError(char* fileName, int windowHeight);

// Clears sample errors off the screen (used when playing the file again)
void clearErrors(int windowHeight);

#endif
