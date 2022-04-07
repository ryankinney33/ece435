#include <stddef.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "battleship_types.h"
#include "display.h"



// Set the ship location in the grid
// Returns:
//   0 on success
//  -1 on out-of-bounds
//  -2 on collision
static int set_ship_location(struct ship *btl, int row, int col, enum ship_direction dir, enum tile_state grid[10][10])
{
	// Sanity check
	if (grid == NULL || btl == NULL || row < 0 || row > 9 || col < 0 || col > 9) {
		return -1;
	}

	// Make sure the location and orientation are in-bounds
	switch (dir) {
		case up:
			if (row - btl->health < 0)
				return -1;
			break;
		case down:
			if (row + btl->health > 9)
				return -1;
			break;
		case left:
			if (col - btl->health < 0)
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
		buf[0] = tolower(buf[0]);

		if ((buf[0] < 'a' || buf[0] > 'j') // first character should be a-j
		    || (buf[1] < '0' || buf[1] > '9')) { // 2nd should be 0-9
			// There was an invalid input
			snprintf(prompt, 55, "Invalid input, please try again: ");
			continue;
		}

		// Extract the row and column
		int row = buf[0] - 'a';
		int col = buf[1] - '0';

		snprintf(prompt, 55, "Please enter the orientation (up, down, left, right): ");
		get_user_input(prompt, buf, 6);

		enum ship_direction dir;

		if (!strcmp(buf, "up")) {
			dir = up;
		} else if (!strcmp(buf, "down")) {
			dir = down;
		} else if (!strcmp(buf, "left")) {
			dir = left;
		} else if (!strcmp(buf, "right")) {
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

struct team *init_team(void)
{
	struct team *btlshp = malloc(sizeof(struct team));
	if (btlshp == NULL)
		return NULL;

	// Make sure both grids are set to the default state
	memset(btlshp->enemy, null, 100 * sizeof(enum tile_state));
	memset(btlshp->yours, null, 100 * sizeof(enum tile_state));

	// Initialize the display for showing the grid and getting input
	init_display(1);

	/*
	enum tile_state TALLY_HO_LADS = 0;
	for (int i = 0; i < 10; ++i) {
		for (int j = 0; j < 10; ++j) {
			btlshp->yours[i][j] = btlshp->enemy[i][j] = TALLY_HO_LADS++;
			if (TALLY_HO_LADS > 3) {
				TALLY_HO_LADS = 0;
			}
		}
	}
	*/

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

	return btlshp; // No more work to be done
}
