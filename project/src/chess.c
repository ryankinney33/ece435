#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

#include "display.h"

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

	// Create the display
	if (init_display()) {
		return 1;
	}

	return 0;
}
