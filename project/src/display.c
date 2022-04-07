#include <ncurses.h>
#include <string.h>

#include "display.h"
#include "battleship_types.h"


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

static short orig[3]; // Original black color:
static int using_colors = 0; // Flag for using colors or not

// Private functions
static int init_colors();

// Initialize the screen for displaying the chess board
int init_display(int use_color)
{
	// Start curses mode
	initscr();

	// Turn off cbreak
	nocbreak();

	// Start color mode (if applicable)
	if (use_color && init_colors()) {
		fprintf(stderr, "Error: your terminal does not support color.\n");
		return -1;
	}

	return 0;
}

static int init_colors()
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
		color_content(DARK_BG, &orig[0], &orig[1], &orig[2]);
		init_color(DARK_BG, 0, 0, 0);
	}

	// Initialize the color pairs
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

void display_grids(const struct team *btlshp)
{
	clear();
	printw(" Your Ships\t Enemy Ships\n");
	printw("  0123456789\t  0123456789\n");
	printw("A ~~~~~~~~~~\tA ~~~~~~~~~~\n");
	printw("B ~~~~~~~~~~\tB ~~~~~~~~~~\n");
	printw("C ~~~~~~~~~~\tC ~~~~~~~~~~\n");
	printw("D ~~~~~~~~~~\tD ~~~~~~~~~~\n");
	printw("E ~~~~~~~~~~\tE ~~~~~~~~~~\n");
	printw("F ~~~~~~~~~~\tF ~~~~~~~~~~\n");
	printw("G ~~~~~~~~~~\tG ~~~~~~~~~~\n");
	printw("H ~~~~~~~~~~\tH ~~~~~~~~~~\n");
	printw("I ~~~~~~~~~~\tI ~~~~~~~~~~\n");
	printw("J ~~~~~~~~~~\tJ ~~~~~~~~~~\n");
	refresh();
}

void get_user_input(const char *prompt, char *buf, int size)
{
	// clear the buffer
	memset(buf, 0, size);

	// Display the prompt
	addstr(prompt);
	refresh();

	// read the bytes from the user
	getnstr(buf, size - 1);
}

// A function to cleanly end the ncurses window
void end_display()
{
	// restore colors?
	if (using_colors && can_change_color()) {
		init_color(DARK_BG, orig[0], orig[1], orig[2]);
		refresh();
	}
	// exit the window
	endwin();
}

