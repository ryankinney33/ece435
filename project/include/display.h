#ifndef DISPLAY_H
#define DISPLAY_H

#include <ncurses.h>

// Macros for the colors
#define BLACK_NO 1 // dark bg, no piece
#define WHITE_NO 2 // light bg, no piece
#define BLACK_BL 3 // dark bg, dark piece
#define WHITE_BL 4 // light bg, dark piece
#define BLACK_WH 5 // dark bg, light piece
#define WHITE_WH 6 // light bg, light piece

#define LIGHT_FG COLOR_WHITE
#define DARK_FG COLOR_RED
#define LIGHT_BG COLOR_GREEN
#define DARK_BG COLOR_BLACK

// Functions for display handling
int init_display();
void end_display();

#endif /* DISPLAY_H */
