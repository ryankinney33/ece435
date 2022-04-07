#include <stddef.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "battleship_types.h"
#include "display.h"



// Set the ship location in the grid
// Returns 0 on success
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
				return -1;
		}
	} else {
		for (int i = 0; i < btl->health; ++i) {
			enum tile_state temp = grid[row][col + sign * i];
			if (temp != null)
				return -1;
		}
	}

	// No collision; set the values
	if (dir == up || dir == down) {
		for (int i = 0; i < btl->health; ++i) {
			grid[row + sign * i][col] = ship;
		}
	} else {
		for (int i = 0; i < btl->health; ++i) {
			grid[row - 1][col + sign * i] = ship;
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
	snprintf(prompt, 55, "Please enter the location for your %s: ", names[type - 1]);

	while (1) {
		get_user_input(prompt, buf, 4);

		// Decode the input
		if ((buf[0] < 'a' || buf[0] > 'j') // first character should be a-j
		    || (buf[1] < '1' || buf[1] > '9') // 2nd should be 1-9
		    || (buf[2] && (buf[1] != '1' || buf[2] != '0'))) { // third may be terminator OR '0' if 2nd is '1'
			// There was an invalid input
			snprintf(prompt, 55, "Invalid input, please try again: ");
			continue;
		}

		// Extract the row and column
		int row = buf[0] - 'a';
		int col = atoi(buf + 1) - 1;

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
		if (!set_ship_location(btl, row, col, dir, grid))
			return; // input was good

		snprintf(prompt, 55, "Location/orientation invalid, enter new location: ");
	}
}

struct team *init_team(void)
{
	struct team *btlshp = malloc(sizeof(struct team));
	if (btlshp == NULL)
		return NULL;

	// Make sure both grids are set to the default state
	memset(btlshp->attempts, null, 100 * sizeof(enum tile_state));
	memset(btlshp->local, null, 100 * sizeof(enum tile_state));

	// Initialize the ships
	setup_ship(&btlshp->car, carrier, btlshp->local);
	setup_ship(&btlshp->bat, battleship, btlshp->local);
	setup_ship(&btlshp->des, destroyer, btlshp->local);
	setup_ship(&btlshp->sub, submarine, btlshp->local);
	setup_ship(&btlshp->pat, patrol_boat, btlshp->local);

	return btlshp; // No more work to be done
}
