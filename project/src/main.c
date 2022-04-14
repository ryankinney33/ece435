#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

#include "display.h"
#include "battleship.h"
#include "network.h"
#include <ncurses.h>

// Global variable for game structure
static struct game *btlshp = NULL;

static void crash_cleanup(int signum)
{
	end_display();
	fprintf(stderr, "Deadly signal arrived. Dyin...\n");
	exit(EXIT_FAILURE);
}

static void winch_h(int signum)
{
	refresh();
}

int main(int argc, char *argv[])
{

	// Install signal handlers for some common deadly signals
	struct sigaction act;
	struct sigaction ign;
	memset(&act, 0, sizeof(act));
	memset(&ign, 0, sizeof(ign));

	/*act.sa_handler = crash_cleanup;
	ign.sa_handler = winch_h;
	sigaction(SIGINT, &act, 0);
	sigaction(SIGABRT, &act, 0);
	sigaction(SIGILL, &act, 0);
	sigaction(SIGFPE, &act, 0);
	sigaction(SIGSEGV, &act, 0);
	sigaction(SIGBUS, &act, 0);
	sigaction(SIGTERM, &act, 0);
	sigaction(SIGHUP, &act, 0);
	sigaction(SIGWINCH, &ign, 0);*/

	// Read the command line arguments and set the flags
	uint16_t port = 0;
	int use_color = 1;
	char *hostname = NULL; // Should be NULL if hosting

	// Decode command line arguments
	for (int i = 1; i < argc; ++i) {
		if (*argv[i] == '-') { // check for options
			// The first character of an option should be -
			// Figure out which option is wanted
			char *tmp = argv[i];
			if (!strcmp(tmp, "--no-color") || !strcmp(tmp, "-n")) {
				// User wants to turn color off
				use_color = 0;
			} else if (!strcmp(tmp, "--help") || !strcmp(tmp, "-h")) {
				// Display the help message
				printf("Usage: %s [-options] [hostname] [port]\n", argv[0]);
				printf("Play a game of battleship.\n");
				printf("If no hostname or port is specified, hosts a game on port 31337\n");
				printf("If a port is passed with no hostname, hosts a game on the specified port.\n");
				printf("If a hostname is passed with no port, connects to the host on port 31337.\n");
				printf("Options:\n");
				printf("  -h, --help    \tdisplay this message\n");
				printf("  -n, --no-color\tturn off colors\n");
				exit(EXIT_SUCCESS);
			} else {
				// Invalid option
				fprintf(stderr, "Inavlid option -- '%s'\n", argv[i]);
				fprintf(stderr, "Try '%s --help' for more information.\n", argv[0]);
				exit(EXIT_FAILURE);
			}
		} else {
			// Check for a port
			// The port should just be a number
			char *tmp;
			long int tmpPort = strtol(argv[i], &tmp, 10);
			if (*tmp == '\0') {
				// The entire string was a number; it's a port
				if (port) {
					fprintf(stderr, "Error: multiple ports entered!\n");
					fprintf(stderr, "Try '%s --help' for more information.\n", argv[0]);
					exit(EXIT_FAILURE);
				}

				if (tmpPort < 1 || tmpPort > 65535) {
					// Invalid host port number
					fprintf(stderr, "Error: invalid port number!\n");
					fprintf(stderr, "Should be a number from 1-65535.\n");
					exit(EXIT_FAILURE);
				}

				port = (uint16_t)tmpPort; // Port was valid
			} else {
				// The argument is the hostname
				if (hostname != NULL) {
					fprintf(stderr, "Error: multiple hosts entered!\n");
					fprintf(stderr, "Try '%s --help' for more information.\n", argv[0]);
					exit(EXIT_FAILURE);
				}

				// Hostname should be set before the port
				if (port) {
					fprintf(stderr, "%s is an invalid hostname!\n", argv[i - 1]);
					fprintf(stderr, "Try '%s --help' for more information.\n", argv[0]);
					exit(EXIT_FAILURE);
				}

				// Set the hostname
				hostname = argv[i];
			}
		}
	}

	// Set the port number to 31337 if unset
	if (!port)
		port = 31337;

	btlshp = init_game(hostname, port, use_color);
	if (btlshp == NULL) {
		end_display();
		return 1;
	}

	// main game loop
	while (1) {
		display_grids(btlshp);
		int result = process_turn(btlshp);
		if (result < 0)
			break;
	}

//	display_grids(btlshp);
//	getch();
	end_display();

	return 0;
}

