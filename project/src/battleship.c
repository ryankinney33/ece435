#include <stddef.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <inttypes.h>

#include "battleship_types.h"
#include "display.h"
#include "network.h"
#include <ncurses.h>


// Private functions
static int set_ship_location(struct ship *btl, int row, int col,
		enum ship_direction dir, enum tile_state grid[10][10]);
static int decode_location(const char buf[3], int *row, int *col);
static void setup_ship(struct ship *btl, enum ship_type type, enum tile_state grid[10][10]);



/************************************* Initialization functions **************************/

/*
 * Set up the battlship game
 * Gets input from the user to set up the grid
 * Also displays the grid
 */
struct game *init_game(const char *hostname, uint16_t port, int use_color)
{
	struct game *btlshp = malloc(sizeof(struct game));
	if (btlshp == NULL) {
		perror("malloc");
		return NULL;
	}

	// Perform network connections
	if (hostname == NULL) { // hosting
		printf("Hosting on port %"PRIu16"\n", port);
		if (host_game(port, btlshp) < 0) {
			free(btlshp);
			return NULL;
		}
	} else {
		printf("Connecting to %s on port %"PRIu16"\n", hostname, port);
		if (join_game(hostname, port, btlshp) < 0) {
			free(btlshp);
			return NULL;
		}
	}
	printf("\nConnection received.\n");

	// Make sure both grids are set to the default state
	memset(btlshp->enemy, null, 100 * sizeof(enum tile_state));
	memset(btlshp->yours, null, 100 * sizeof(enum tile_state));

	// Initialize the display for showing the grid and getting input
	init_display(use_color);

	// Initialize the ships
	display_grids(btlshp);
	setup_ship(&btlshp->car, carrier, btlshp->yours);
	display_grids(btlshp);
	setup_ship(&btlshp->bat, battleship, btlshp->yours);
	display_grids(btlshp);
	setup_ship(&btlshp->des, destroyer, btlshp->yours);
	display_grids(btlshp);
	setup_ship(&btlshp->sub, submarine, btlshp->yours);
	display_grids(btlshp);
	setup_ship(&btlshp->pat, patrol_boat, btlshp->yours);

	display_grids(btlshp);

	// Tell the other player we are ready
	if (send_to_enemy("done", btlshp) < 0) {
		perror("send_to_enemy");
		free(btlshp);
		return NULL;
	}

	printw("Waiting for other player...");
	refresh();

	// Wait for the other player to be ready
	char *buf = read_from_enemy(btlshp);
	if (buf == NULL) {
		perror("read_from_enemy");
		free(btlshp);
		return NULL;
	} else if (strcmp(buf, "done")) { // sanity check
		fprintf(stderr, "Received unexpected message...\n");
		free(btlshp);
		free(buf);
		return NULL;
	}
	free(buf);

	return btlshp; // No more work to be done
}

// Gets user input to eventually set the ship location
static void setup_ship(struct ship *btl, enum ship_type type, enum tile_state grid[10][10])
{
	// Set the health and id of the ship
	btl->id = type;
	btl->health = (type == submarine) ? destroyer : type; // submarine and destroyer sizes are the same

	// Get user input for the ship location and orientation
	char buf[6]; // buffer to hold the string from the user
	const char *names[5] = {"submarine", "patrol boat", "destroyer", "battleship", "carrier"};
	char prompt[55];
	snprintf(prompt, 55, "Please enter the location of your %s: ", names[type - 1]);

	while (1) {
		get_user_input(prompt, buf, 3);

		// Decode the input
		int row, col;
		if (decode_location(buf, &row, &col)) {
			// There was an invalid input
			snprintf(prompt, 55, "Invalid input, please try again: ");
			continue;
		}

		snprintf(prompt, 55, "Please enter the orientation (up, down, left, right): ");
		get_user_input(prompt, buf, 6);

		// Convert buf to lowercase
		for (int i = 0; i < strlen(buf); ++i) {
		buf[i] = tolower(buf[i]);
		}

		enum ship_direction dir;

		if (!strcmp(buf, "up") || !strcmp(buf, "u")) {
			dir = up;
		} else if (!strcmp(buf, "down") || !strcmp(buf, "d")) {
			dir = down;

		} else if (!strcmp(buf, "left") || !strcmp(buf, "l")) {
			dir = left;
		} else if (!strcmp(buf, "right") || !strcmp(buf, "r")) {
			dir = right;
		} else {
			snprintf(prompt, 55, "Invalid input, enter new location ");
			continue;
		}


		// Determine if the location is valid
		int rslt = set_ship_location(btl, row, col, dir, grid);
		if (rslt == -1) {
			snprintf(prompt, 55, "Location/orientation invalid, enter new location: ");
		} else if (rslt == -2) {
			snprintf(prompt, 55, "Location already in use, please try again: ");
		} else {
			return;
		}
	}
}

/*
 * Set the ship location in the grid
 * Returns:
 *   0 on success
 *  -1 on out-of-bounds
 *  -2 on collision
 */
static int set_ship_location(struct ship *btl, int row, int col, enum ship_direction dir, enum tile_state grid[10][10])
{
	// Sanity check
	if (grid == NULL || btl == NULL || row < 0 || row > 9 || col < 0 || col > 9) {
		return -1;
	}

	// Make sure the location and orientation are in-bounds
	switch (dir) {
		case up:
			if (row < btl->health)
				return -1;
			break;
		case down:
			if (row + btl->health > 9)
				return -1;
			break;
		case left:
			if (col < btl->health)
				return -1;
			break;
		case right:
			if (col + btl->health > 9)
				return -1;
	}

	// Next, check for collisions
	int sign = (dir == down || dir == right) ? 1 : -1;

	if (dir == up || dir == down) {
		for (int i = 0; i < btl->health; ++i) {
			enum tile_state temp = grid[row + sign * i][col];
			if (temp != null)
				return -2;
		}
	} else {
		for (int i = 0; i < btl->health; ++i) {
			enum tile_state temp = grid[row][col + sign * i];
			if (temp != null)
				return -2;
		}
	}

	// No collision; set the values
	btl->row = row;
	btl->col = col;
	btl->dir = dir;
	if (dir == up || dir == down) {
		for (int i = 0; i < btl->health; ++i) {
			grid[row + sign * i][col] = ship;
		}
	} else {
		for (int i = 0; i < btl->health; ++i) {
			grid[row][col + sign * i] = ship;
		}
	}

	return 0;
}


/******************************** Gameplay-related functions ******************************/

/*
 * Get a move from the user
 * Calls a function to process the move
 * Sends the move to the remote client
 * Receives the result from the remote client
 */
void get_move_user(struct game *btlshp)
{
	if (btlshp == NULL)
		return;

	// First get the move from the user
	char prompt[34];
	char buf[3];
	snprintf(prompt, 34, "Enter your shot: ");
	while (1) {
		get_user_input(prompt, buf, 3);

		// decode the input
		int row, col;
		if (decode_location(buf, &row, &col)) {
			// There was an error
			snprintf(prompt, 34, "Invalid input, please try again: ");
			continue;
		}
	}

	// Send to other client

	// Get response and update the grid
}

/* Decodes the passed location string
 * Locations are a letter followed by a number
 * Letters are from a-j and number from 0-9
 * Letter may be capital
 * Returns 0 on success
 */
static int decode_location(const char buf[3], int *row, int *col)
{
	if (buf == NULL)
		return -1;

	char tmp = tolower(buf[0]);

	if (tmp < 'a' || tmp > 'j' || buf[1] < '0' || buf[1] > '9')
		return -2;

	*row = tmp - 'a';
	*col = buf[1] - '0';
	return 0;
}

void get_move_enemy(struct game *btlshp)
{



}
