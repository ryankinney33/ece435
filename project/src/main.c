#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

#include "display.h"
#include "chess.h"

extern struct chess_piece board[8][8]; // the chess grid

static void cleanup() {
	end_display();
	fprintf(stderr, "Deadly signal arrived. Dyin...\n");
	exit(EXIT_FAILURE);
}

int main() {
	// Install signal handlers for some common deadly signals
	struct sigaction act;
	memset(&act, 0, sizeof(act));

	act.sa_handler = cleanup;
	sigaction(SIGINT, &act, 0);
	sigaction(SIGABRT, &act, 0);
	sigaction(SIGILL, &act, 0);
	sigaction(SIGFPE, &act, 0);
	sigaction(SIGSEGV, &act, 0);
	sigaction(SIGBUS, &act, 0);
	sigaction(SIGTERM, &act, 0);

	init_display();
//	init_team(team_white, 1, white);
//	init_team(team_black, 1, black);

	return 0;
}
