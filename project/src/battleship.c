#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <inttypes.h>

#include "battleship_types.h"
#include "display.h"
#include "network.h"


// Private functions
static int set_ship_location(struct ship *btl, int row, int col,
		enum ship_direction dir, enum tile_state grid[10][10]);
static int decode_location(const char buf[3], int *row, int *col);
static void setup_ship(struct ship *btl, enum ship_type type, enum tile_state grid[10][10]);
static enum ship_type determine_hit(int row, int col, struct game *btlshp);
static int was_ship_hit(const struct ship *btl, int row, int col);

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

		// Host moves first
		btlshp->turn = 1;
	} else {
		printf("Connecting to %s on port %"PRIu16"\n", hostname, port);
		if (join_game(hostname, port, btlshp) < 0) {
			free(btlshp);
			return NULL;
		}

		// Client moves second
		btlshp->turn = 0;
	}

	// Make sure both grids are set to the default state
	memset(btlshp->enemy, null, 100 * sizeof(enum tile_state));
	memset(btlshp->yours, null, 100 * sizeof(enum tile_state));

	// Initialize the display for showing the grid and getting input
	init_display(use_color);

	// Initialize the ships
	display_grids(btlshp);
	setup_ship(&btlshp->boats[0], carrier, btlshp->yours);
	display_grids(btlshp);
	setup_ship(&btlshp->boats[1], battleship, btlshp->yours);
	display_grids(btlshp);
	setup_ship(&btlshp->boats[2], destroyer, btlshp->yours);
	display_grids(btlshp);
	setup_ship(&btlshp->boats[3], submarine, btlshp->yours);
	display_grids(btlshp);
	setup_ship(&btlshp->boats[4], patrol_boat, btlshp->yours);
	btlshp->game_over = 0;

	display_grids(btlshp);

	// Tell the other player we are ready
	if (send_to_enemy("done", btlshp) < 0) {
		perror("send_to_enemy");
		free(btlshp);
		return NULL;
	}

	display_status("Waiting for other player...");

	// Wait for the other player to be ready
	char buf[5];
	int res = read_from_enemy(buf, btlshp);
	if (res < 0) {
		perror("read_from_enemy");
		free(btlshp);
		return NULL;
	} else if (/*res == 0 ||*/ strncmp(buf, "done", 4)) { // sanity check
		free(btlshp);
		return NULL;
	}

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
			if (row < (btl->health - 1))
				return -1;
			break;
		case down:
			if ((row + btl->health - 1) > 9)
				return -1;
			break;
		case left:
			if (col < (btl->health - 1))
				return -1;
			break;
		case right:
			if ((col + btl->health - 1) > 9)
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
int get_move_user(struct game *btlshp)
{
	if (btlshp == NULL)
		return -1;

	display_status("Entering your shot...");
	display_grids(btlshp);

	// First get the move from the user
	char *prompt;
	char buf[3];
	int row, col;
	prompt = "Enter your shot: ";
	while (1) {
		get_user_input(prompt, buf, 3);

		// decode the input
		if (decode_location(buf, &row, &col)) {
			// There was an error
			prompt = "Invalid input, please try again: ";
			continue;
		}

		break;
	}

	// Send to other client
	if (send_to_enemy(buf, btlshp) < 0) {
		// there was an error
		return -1;
	}

	// Get response and update the grid
	char msg[5];
	int res = read_from_enemy(msg, btlshp);
	if (res < 0) {
		// an error occurred
		return -1;
	} else if (res == 0) {
		// user disconnected
		return 1;
	}

	// Decode message
	if (msg[0] == 'X') {
		// shot missed
		display_message("Your shot missed!");
		btlshp->enemy[row][col] = miss;
	} else if (msg[0] == 'H') {
		// shot hit
		btlshp->enemy[row][col] = hit;

		// see if the enemy ship has sunk
		if (msg[1] == 'C') {
			// display carrier sunk message
			display_message("Enemy carrier destroyed");
		} else if (msg[1] == 'B') {
			// display battleship sunk message
			display_message("Enemy battleship destroyed");
		} else if (msg[1] == 'D') {
			// display destroyer sunk message
			display_message("Enemy destroyer destroyed");
		} else if (msg[1] == 'S') {
			// display submarine sunk message
			display_message("Enemy submarine destroyed");
		} else if (msg[1] == 'P') {
			// display patrol boat sunk message
			display_message("Enemy patrol boat destroyed");
		} else if (msg[1] == 'A') {
			// game is over
			btlshp->game_over = 1;
			// display game over message
			display_game_over(1);
			return 1;
		} else if (msg[1] == '\0') {
			display_message("Direct hit!");
		}
	}

	return 0;
}

/*
 * Decodes the passed location string
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

// Waits until the enemy sends their move
// Returns negative on error
// Return 0 on success
// Return 1 on game end
int get_move_enemy(struct game *btlshp)
{
	if (btlshp == NULL) {
		return -1;
	}

	display_status("Waiting for opponent...");
	display_grids(btlshp);

	// First, read the move from the enemy
	char msg[5];
	int res = read_from_enemy(msg, btlshp);
	if (res < 0) {
		return -1;
	} else if(res == 0) {
		// user disconnected
		return 1;
	}

	// The string is simply the row column pair, such as j0
	int row, col;

	// The location has already been checked for validity by
	// the enemy, no need to do it here
	decode_location(msg, &row, &col);

	// Determine if there was a hit, miss, or repeat shot
	if (btlshp->yours[row][col] == ship) {
		// there was a hit
		btlshp->yours[row][col] = hit;

		// Figure out which ship was hit
		enum ship_type typ = determine_hit(row, col, btlshp);

		// See if all the ships are dead
		int dead_count = 0;
		int ship_died = 0; // flag if the ship hit just sank
		for (int i = 0; i < 5; ++i) {
			if (btlshp->boats[i].health == dead) {
				dead_count++;
				if (btlshp->boats[i].id == typ)
					ship_died = 1;
			}
		}

		// Send the response message
		if (dead_count == 5) {
			btlshp->game_over = 1;
			// display game over message
			display_game_over(0);
			return send_to_enemy("HA", btlshp);
		}

		if (ship_died) {
			switch (typ) {
				case carrier:
					display_message("Your carrier sunk!");
					return send_to_enemy("HC", btlshp);
				case battleship:
					display_message("Your battleship sunk!");
					return send_to_enemy("HB", btlshp);
				case destroyer:
					display_message("Your destroyer sunk!");
					return send_to_enemy("HD", btlshp);
				case submarine:
					display_message("Your submarine sunk!");
					return send_to_enemy("HS", btlshp);
				case patrol_boat:
					display_message("Your patrol boat sunk!");
					return send_to_enemy("HP", btlshp);
				default:
					return -1; // this should not be executed
			}
		}

		// No ships died, just send hit message
		display_message("Hit!");
		return send_to_enemy("H", btlshp);
	} else if (btlshp->yours[row][col] == null) {
		// Miss
		btlshp->yours[row][col] = miss;

		// Send the miss response
		display_message("Miss!");
		return send_to_enemy("X", btlshp);
	} else {
		// A repeat shot?
		// Send the repeat message
		return send_to_enemy("R", btlshp);
	}
}

// Return the type of the ship that was hit. Also decrements the health of the hit ship
static enum ship_type determine_hit(int row, int col, struct game *btlshp)
{
	if (btlshp == NULL) {
		return dead; // Sanity check
	}

	for (int i = 0; i < 5; ++i) {
		struct ship *tmp = &btlshp->boats[i];
		if (tmp->health == dead)
			continue; // ship is already downed

		// Now check if the ship was hit
		if (was_ship_hit(tmp, row, col)) {
			tmp->health--; // decrement the ship health
			return tmp->id;
		}
	}

	return dead;
}

// Returns true if the ship was hit
static int was_ship_hit(const struct ship *btl, int row, int col)
{
	if (btl == NULL)
		return 0; // sanity check

	// Determine the row and column range
	int min_row = btl->row, max_row = btl->row;
	int min_col = btl->row, max_col = btl->col;

	// Get max ship health from type
	int len = (btl->id == submarine) ? 2 : (btl->id - 1);

	// Set the row/column range for the ship
	switch (btl->dir) {
		case up:
			min_row -= len;
			break;
		case down:
			max_row += len;
			break;
		case left:
			min_col -= len;
			break;
		default:
			max_col += len;
	}

	// Returns true if collision
	return (min_row <= row && row <= max_row)
		&& (min_col <= col && col <= max_col);
}

// Process a turn of the game
// Returns 0 if the game is over
// Returns negative if error
int process_turn(struct game *btlshp)
{
	if (btlshp == NULL)
		return -1; // sanity check

	int res;

	if (btlshp->turn) { // player's turn
		res = get_move_user(btlshp);
	} else {
		res = get_move_enemy(btlshp);
	}

	// Handle the result
	if (res < 0) { // errors
		return -1;
	} if (res == 1) { // game over
		return 0;
	}

	// Switch turn
	btlshp->turn = !btlshp->turn;
	return 1;
}

// Closes socket connections and cleans up the curses window
void game_cleanup(struct game *btlshp)
{
	// Clear the curses window
	end_display();

	if (btlshp == NULL)
		return; // sanity check

	// Close the socket connections
	close(btlshp->enemy_fd);
	close(btlshp->serv_fd);

	// Finally, free the dynamic memory
	free(btlshp);
}
