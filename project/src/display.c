#include <ncurses.h>
#include "display.h"

// Macros for the colors
#define LIGHT_FG	255 // Bright white
#define DARK_FG 	9 // A bright red
#define LIGHT_BG	236 // A gray color
#define DARK_BG 	0 // Black

// 8 color terminal colors
#define LIGHT_FG8	COLOR_WHITE
#define DARK_FG8	COLOR_RED
#define LIGHT_BG8	COLOR_GREEN
#define DARK_BG8	COLOR_BLACK

/*
 * Color pairs
 * 6 states:
 *   1. Black square no piece
 *   2. Black square white piece
 *   3. Black square black piece
 *   4. White square no piece
 *   5. White square black piece
 *   6. White square white piece
 * Colors pair number bits are the following:
 * LFG, DFG, LBG, DBG;
 * L = light, D = dark, BG = background, FG = foreground
 * Used in the color selection logic in the display_grid function
 * Even row and odd column = LBG, otherwise, DFG
 * Foreground color depends on the piece
 */
#define BLACK_NO	0x01 // dark bg, no piece
#define WHITE_NO	0x02 // light bg, no piece
#define BLACK_BL	0x05 // dark bg, dark piece
#define WHITE_BL	0x06 // light bg, dark piece
#define BLACK_WH	0x09 // dark bg, light piece
#define WHITE_WH	0x0A // light bg, light piece

// Private functions
static void display_grid();
static int init_colors();

static void display_grid() {
	clear();
	for (int i = 0; i < 8; ++i) {
		move(3*i + 1, 0);
		printw("%d", 8 - i);
		for (int j = 0; j < 8; ++j) {
			int color = ((i + j + 1) & 1) + 1;
			// Set cursor position
			// y position is 3*row
			// x position is 5*col
			move(3*i, 5*j + 3);
			attron(COLOR_PAIR(color));
			printw("AAAAA");
			move(3*i + 1, 5*j + 3);
			printw("AAAAA");
			move(3*i + 2, 5*j + 3);
			printw("AAAAA");

			attroff(COLOR_PAIR(color));
		}
		move(3*i + 1, 45); // 2 space after the edge of the grid
		printw("%d", 8 - i);
	}

	move(25, 5);
	printw("A    B    C    D    E    F    G    H\n\n");
	printw("Please enter your move: ");
	refresh();
}

static int init_colors() {
	if (!has_colors()) {
		endwin();
		return -1;
	}

	start_color();
	if (COLORS >= 256) {
		// revert to older colors
		// use 256 color mode
		init_pair(WHITE_NO, LIGHT_BG, LIGHT_BG); // FG color doesnt matter
		init_pair(BLACK_NO, DARK_BG, DARK_BG); // FG color doesnt matter
		init_pair(WHITE_BL, DARK_FG, LIGHT_BG);
		init_pair(BLACK_BL, DARK_FG, DARK_BG);
		init_pair(WHITE_WH, LIGHT_FG, LIGHT_BG);
		init_pair(BLACK_WH, LIGHT_FG, DARK_BG);
	} else {
		// revert to default 8 color mode
		init_pair(WHITE_NO, LIGHT_BG8, LIGHT_BG8); // FG color doesnt matter
		init_pair(BLACK_NO, DARK_BG8, DARK_BG8); // FG color doesnt matter
		init_pair(WHITE_BL, DARK_FG8, LIGHT_BG8);
		init_pair(BLACK_BL, DARK_FG8, DARK_BG8);
		init_pair(WHITE_WH, LIGHT_FG8, LIGHT_BG8);
		init_pair(BLACK_WH, LIGHT_FG8, DARK_BG8);
	}

	return 0;
}

int init_display() {
	// Start curses mode
	initscr();

	//curs_set(0);
	if (init_colors()) {
		fprintf(stderr, "Error: your terminal does not support color.\n");
		return -1;
	}

	display_grid();
	getch(); // wait for user input
	endwin(); // end curses mode

	return 0;
}

void end_display() {
	// Simply cleans up the window to fix the terminal in case of some deadly
	// signals
	endwin();
}
