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

	struct chess_board *board = init_board(1, 1, white);
	if (board == NULL) {
		perror("init_board");
		end_display();
		return 1;
	}
	init_display();
	display_board(board->grid);
	getch();
	end_display();
	board = destroy_board(board);
//	init_team(team_white, 1, white);
//	init_team(team_black, 1, black);

	return 0;
}
