#include <ncurses.h>
#include <locale.h>

#include "display.h"
#include "chess_types.h"

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

// Original black color:
static short orig[3];

// Private functions
static int init_colors();

void display_board(const struct chess_board *board) {
	clear();
	int i_start, j_start; // starting bound
	int i_end, j_end; // end bound
	int i_dir, j_dir; // increment/decrement direction

	// determine the direction
	if (board->player_color == white) { // white goes on bottom
		// count up from 0 to 7
		i_start = j_start = 0;
		i_end = j_end = 8;
		i_dir = j_dir = 1;
	} else { // black goes on bottom
		// count down from 7 to	0
		i_start = j_start = 7;
		i_end = j_end = -1;
		i_dir = j_dir = -1;
	}

	for (int i = i_start; i != i_end; i += i_dir) {
		//move(3*i + 1, 0);
		printw("%d ", 8 - i);
		for (int j = j_start; j != j_end; j += j_dir) {
			int color = ((i + j + 1) & 1) + 1; // background color
			color |= board->grid[7 - i][7 - j].color << 2; // add foreground color

			// Set cursor position
			// y position is 3*row
			// x position is 5*col
			if (board->use_color) {
				attron(COLOR_PAIR(color));
				printw("%s", board->grid[i][j].print_char);
				attroff(COLOR_PAIR(color));
			} else {
				printw("%c%s", board->grid[i][j].prefix, board->grid[i][j].print_char);
			//	printw("%s", board->grid[i][j].print_char);
			}
		}
		printw(" %d\n", 8 - i);
	}

	if (board->use_color) {
		if (board->player_color == white)
			printw("  ABCDEFGH\n\n");
		else
			printw("  HGFEDCBA\n\n");
	} else {
		if (board->player_color == white)
			printw("   A B C D E F G H\n\n");
		else
			printw("   H G F E D C B A\n\n");
	}

	printw("Please enter your move: ");
	refresh();
}

static int init_colors() {
	if (!has_colors()) {
		endwin();
		return -1;
	}

	start_color();

	// possibly change black to look better?
	if (can_change_color()) {
		color_content(DARK_BG, &orig[0], &orig[1], &orig[2]);
		init_color(DARK_BG, 0, 0, 0);
	}

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
	// Set the locale
	setlocale(LC_ALL, "");

	// Start curses mode
	initscr();

	//curs_set(0);
	if (init_colors()) {
		fprintf(stderr, "Error: your terminal does not support color.\n");
		return -1;
	}

	return 0;
}

void end_display() {
	// A function to cleanly end the ncurses window
	// restore colors?
	if (can_change_color()) {
		init_color(DARK_BG, orig[0], orig[1], orig[2]);
		refresh();
	}
	// exit the window
	endwin();
}
