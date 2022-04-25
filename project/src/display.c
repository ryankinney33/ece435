#include <ncurses.h>
#include <string.h>

#include "display.h"
#include "battleship_types.h"

// Macros for the colors
#define COL_NULL	18 // A darkish blue
#define COL_SHIP	236 // A gray color
#define COL_MISS	6 // A much lighter blue
#define COL_HIT 	9 // Darker red

// 8 terminal colors
#define COL_NULL8	COLOR_BLUE
#define COL_SHIP8	COLOR_BLACK
#define COL_MISS8	COLOR_MAGENTA
#define COL_HIT8	COLOR_RED

//  Color pairs for the 4 tile states:
#define NULL_TILE 1
#define SHIP_TILE 2
#define MISS_TILE 3
#define HIT_TILE  4

static short orig[3]; // Original black color:
static int using_colors = 0; // Flag for using colors or not

// Private functions
static int init_colors(void);

// Initialize the screen for displaying the chess board
int init_display(int use_color)
{
	// Start curses mode
	initscr();

	// Turn off cbreak and the cursor
	nocbreak();
	curs_set(0);

	// Start color mode (if applicable)
	if (use_color && init_colors()) {
		fprintf(stderr, "Error: your terminal does not support color.\n");
		return -1;
	}

	return 0;
}

static int init_colors(void)
{
	// Check if colors are supported
	if (!has_colors()) {
		endwin();
		return -1;
	}
	start_color();
	using_colors = 1;

	// possibly change black to look better?
	if (can_change_color()) {
		color_content(COLOR_BLACK, &orig[0], &orig[1], &orig[2]);
		init_color(COLOR_BLACK, 0, 0, 0);
	}

	// Initialize the color pairs
	if (COLORS >= 256) {
		// revert to older colors
		// use 256 color mode
		init_pair(NULL_TILE, COLOR_WHITE, COL_NULL);
		init_pair(SHIP_TILE, COLOR_WHITE, COL_SHIP);
		init_pair(MISS_TILE, COLOR_BLACK, COL_MISS);
		init_pair(HIT_TILE, COLOR_BLACK, COL_HIT);

	} else {
		// revert to default 8 color mode
		init_pair(NULL_TILE, COLOR_WHITE, COL_NULL8);
		init_pair(SHIP_TILE, COLOR_WHITE, COL_SHIP8);
		init_pair(MISS_TILE, COLOR_WHITE, COL_MISS8);
		init_pair(HIT_TILE, COLOR_WHITE, COL_HIT8);
	}

	return 0;
}

void display_grids(const struct game *btlshp)
{
	if (btlshp == NULL) {
		return; // sanity check
	}

	const char tile_map[4] = {'~', 'S', 'X', 'H'};
	const char tile_map_c[4] = {'~', ' ', 'X', 'X'};
	move(2, 0);
	clrtobot();
	printw("  Your Ships\t Enemy Ships\n");
	printw("  0123456789\t  0123456789\n");
	for (int row = 0; row < 10; ++row) {
		printw("%c ", row + 'A');
		for (int col = 0; col < 10; ++col) {
			enum tile_state tstate = btlshp->yours[row][col];
			char tmp = tile_map[tstate];
			if (using_colors) {
				tmp = tile_map_c[tstate];
				attron(COLOR_PAIR(tstate + 1));
				addch(tmp);
				attroff(COLOR_PAIR(tstate + 1));
			} else {
				addch(tmp);
			}
		}
		printw(" %c\t%c ", row + 'A', row + 'A');
		for (int col = 0; col < 10; ++col) {
			enum tile_state tstate = btlshp->enemy[row][col];
			char tmp = tile_map[tstate];
			if (using_colors) {
				tmp = tile_map_c[tstate];
				attron(COLOR_PAIR(tstate + 1));
				addch(tmp);
				attroff(COLOR_PAIR(tstate + 1));
			} else {
				addch(tmp);
			}
		}
		printw(" %c\n", row + 'A');
	}
	printw("  0123456789\t  0123456789\n\n");
	refresh();
}

void get_user_input(const char *prompt, char *buf, int size)
{
	// Show the cursor
	curs_set(1);

	// clear the buffer
	memset(buf, 0, size);

	// Display the prompt
	addstr(prompt);
	refresh();

	// read the bytes from the user
	getnstr(buf, size - 1);

	// Hide the cursor
	curs_set(0);
}

// A function to cleanly end the ncurses window
void end_display(void)
{
	// restore colors?
	if (using_colors && can_change_color()) {
		init_color(COLOR_BLACK, orig[0], orig[1], orig[2]);
		refresh();
	}
	// exit the window
	endwin();
}

// Print a message on the first row; used for printing game status
void display_status(const char *msg)
{
	if (msg == NULL)
		return; // sanity check

	// Move to the first row and clear the line
	move(0,0);
	clrtoeol();

	// display the message on the screen
	addstr(msg);
	refresh();
}

// Display a message on the second row of the screen
// Prints the message depending on if your shot was successful
void display_message(const char *msg)
{
	if (msg == NULL)
		return; // sanity check

	// Move to the second row and clear the line
	move(1,0);
	clrtoeol();

	// display the message on the screen
	addstr(msg);
	refresh();
}

// Display the game over message
void display_game_over(int winner)
{
	move(0,0);
	clear();
	printw("You %s.\n", (winner) ? "win" : "lose");
	refresh();
}

// Wait for a user to press a button
void wait_key(void)
{
	curs_set(1);
	cbreak();
	printw("Press any key to continue...");
	refresh();
	getch();
}

// Attempt to handle window resizing
void winch_h(int signum)
{
	refresh();
}
