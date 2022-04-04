#ifndef CHESS_H
#define CHESS_H

#include "chess_types.h"

// Functions related to the beginning and end of the game
struct chess_board *init_board(int unicode, enum team_color player);
struct chess_board *destroy_board(struct chess_board *board);

#endif /* CHESS_H */
