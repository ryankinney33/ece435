#ifndef BATTLESHIP_H
#define BATTLESHIP_H

#include "battleship_types.h"

// Functions related to the beginning and end of the game
struct game *init_game(int use_color);


// Get a move from the user and send it to the remote client
void get_move_user(struct game *btlshp);

// Get a move from the remote client
void get_move_enemy(struct game *btlshp);

#endif /* BATTLESHIP_H */
