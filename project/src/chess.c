#include <ncurses.h>

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

int main () {
	// Start curses mode
	initscr();
	//curs_set(0);
	if (init_colors()) {
		fprintf(stderr, "Error: your terminal does not support color.\n");
		return 1;
	}

	display_grid();
	getch(); // wait for user input
	endwin(); // end curses mode

	return 0;
}
