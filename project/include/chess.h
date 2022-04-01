#ifndef CHESS_H
#define CHESS_H

#include "chess_types.h"

struct chess_board *init_board(int has_color, int unicode, enum team_color player);
struct chess_board *destroy_board(struct chess_board *board);

#endif /* CHESS_H */
