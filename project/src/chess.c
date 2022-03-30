#include <ncurses.h>

static int display_grid() {
	clear();
	printw("   A  B  C  D  E  F  G\n\n");
	printw("8                        8\n");
	printw("7                        7\n");
	printw("6                        6\n");
	printw("5                        5\n");
	printw("4                        4\n");
	printw("3                        3\n");
	printw("2                        2\n");
	printw("1                        1\n\n");
	printw("   A  B  C  D  E  F  G");

	refresh();
}

int main () {
	// Start curses mode
	initscr();
	start_color();
	display_grid();

	getch(); // wait for user input
	endwin(); // end curses mode

	return 0;
}
