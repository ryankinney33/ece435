#include <ncurses.h>

#define WHITE_BG 1
#define BLACK_BG 2

static int display_grid() {
	clear();
/*	printw("8         8\n");
	printw("7         7\n");
	printw("6         6\n");
	printw("5         5\n");
	printw("4         4\n");
	printw("3         3\n");
	printw("2         2\n");
	printw("1         1\n");
	printw("  ABCDEFGH");
*/
	for (int i = 8; i > 0; --i) {
		printw("%d ", i);
		for (int j = 0; j < 8; ++j) {
			int color = ((j+i) & 1) + 1;
			printf("color: %d\r\n", color);
			attron(COLOR_PAIR(color));
			printw(" ");
			attroff(COLOR_PAIR(color));
		}
		printw(" %d\n", i);
	}

	printw("  ABCDEFGH");
	refresh();
	return 0;
}

static int init_colors() {
	if (!has_colors()) {
		endwin();
		return -1;
	}

	start_color();
	init_pair(WHITE_BG, COLOR_WHITE, COLOR_WHITE);
	init_pair(BLACK_BG, COLOR_BLACK, COLOR_BLACK);

	return 0;
}

int main () {
	// Start curses mode
	initscr();
	curs_set(0);
	if (init_colors()) {
		fprintf(stderr, "Error: your terminal does not support color.\n");
		return 1;
	}

	display_grid();
	getch(); // wait for user input
	endwin(); // end curses mode

	return 0;
}
