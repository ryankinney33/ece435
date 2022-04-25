#ifndef BATTLESHIP_H
#define BATTLESHIP_H

#include <stdint.h>
#include "battleship_types.h"

// Functions related to the beginning and end of the game
struct game *init_game(const char *hostname, uint16_t port, int use_color);

// Get a move from the user and send it to the remote client
int get_move_user(struct game *btlshp);

// Get a move from the remote client
int get_move_enemy(struct game *btlshp);

// Process a turn of the game
// Returns 0 if the game is over
// Returns negative if error
int process_turn(struct game *btlshp);

// Closes socket connections and cleans up the curses window
void game_cleanup(struct game *btlshp);

#endif /* BATTLESHIP_H */
