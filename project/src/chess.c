#include <ncurses.h>

int main () {
	// Start curses mode
	initscr();
	printw("AAAAAAAAAAAA");
	refresh(); // display on the real screen
	getch(); // wait for user input
	endwin(); // end curses mode

	return 0;
}
