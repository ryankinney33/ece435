#ifndef BATTLESHIP_H
#define BATTLESHIP_H

#include "battleship_types.h"

// Functions related to the beginning and end of the game
struct chess_board *init_board(int unicode, enum team_color player);
struct chess_board *destroy_board(struct chess_board *board);

#endif /* BATTLESHIP_H */
