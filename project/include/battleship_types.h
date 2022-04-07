#ifndef BATTLESHIP_TYPES_H
#define BATTLESHIP_TYPES_H

// The 4 different tile states
enum tile_state {
	null,	// not occupied, not shot
	ship,	// occupied, not shot
	miss,	// not occupied, shot hit
	hit	// occupied, shot hit
};

// The different types of ships (holds the size (except submarine))
enum ship_type {
	carrier = 5,
	battleship = 4,
	destroyer = 3,
	submarine = 1,
	patrol_boat = 2,
	dead = 0
};

enum ship_direction {
	up,
	down,
	left,
	right
};

struct ship {
	// Information about id and "health"
	enum ship_type id;
	enum ship_type health;
};

// Contains the ship locations and two grids.
// The first grid contains information about where
// the user's shots success and the second grid
// contains information about where the opponent
// has shot
struct team {
	// Each team has 1 of each type of ship
	struct ship car;
	struct ship bat;
	struct ship des;
	struct ship sub;
	struct ship pat;

	// Holds information about where the player has shot
	enum tile_state enemy[10][10];

	// The player's grid;
	enum tile_state yours[10][10];
};

#endif /* BATTLESHIP_TYPES_H */
