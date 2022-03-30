#ifndef DISPLAY_H
#define DISPLAY_H

// Macros for colors
#define WHITE_NO 1 // light bg, no piece
#define BLACK_NO 2 // dark bg, no piece
#define WHITE_BL 3 // light bg, dark piece
#define BLACK_BL 4 // dark bg, dark piece
#define WHITE_WH 5 // light bg, light piece
#define BLACK_WH 6 // dark bg, light piece

#define LIGHT_FG COLOR_BLUE
#define DARK_FG COLOR_RED

#define LIGHT_BG COLOR_WHITE
#define DARK_BG COLOR_BLACK

int init_display();
int end_display();

#endif /* DISPLAY_H */
