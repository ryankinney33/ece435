#include <ncurses.h>
#include "display.h"

static void display_grid() {
	clear();
	for (int i = 8; i > 0; --i) {
		printw("%d ", i);
		for (int j = 0; j < 8; ++j) {
			int color = ((j+i) & 1) + 1;
			attron(COLOR_PAIR(color));
			printw("~");
			attroff(COLOR_PAIR(color++));
		}
		printw(" %d\n", i);
	}

	printw("  ABCDEFGH\n\n");
	refresh();
}

static int init_colors() {
	if (!has_colors()) {
		endwin();
		return -1;
	}

	start_color();
	init_pair(WHITE_NO, LIGHT_BG, LIGHT_BG); // FG color doesnt matter
	init_pair(BLACK_NO, DARK_BG, DARK_BG); // FG color doesnt matter
	init_pair(WHITE_BL, DARK_FG, LIGHT_BG);
	init_pair(BLACK_BL, DARK_FG, DARK_BG);
	init_pair(WHITE_WH, LIGHT_FG, LIGHT_BG);
	init_pair(BLACK_WH, LIGHT_FG, DARK_BG);

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

int end_display() {
	// Simply cleans up the window to fix the terminal in case of some deadly
	// signals
	endwin();
}
