#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

#include "display.h"
#include "chess.h"
#include <ncurses.h>

static void crash_cleanup() {
	end_display();
	fprintf(stderr, "Deadly signal arrived. Dyin...\n");
	exit(EXIT_FAILURE);
}

int main() {
	// Install signal handlers for some common deadly signals
	struct sigaction act;
	memset(&act, 0, sizeof(act));

	act.sa_handler = crash_cleanup;
	sigaction(SIGINT, &act, 0);
	sigaction(SIGABRT, &act, 0);
	sigaction(SIGILL, &act, 0);
	sigaction(SIGFPE, &act, 0);
	sigaction(SIGSEGV, &act, 0);
	sigaction(SIGBUS, &act, 0);
	sigaction(SIGTERM, &act, 0);

	struct chess_board *board = init_board(1, 0, white);
	if (board == NULL) {
		fprintf(stderr, "Error: could not create the chess board!\n");
		end_display();
		return 1;
	}
	init_display(board->use_color);
	display_board(board);
	getch();
	end_display();
	board = destroy_board(board);

	return 0;
}
