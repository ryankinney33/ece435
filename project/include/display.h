#ifndef DISPLAY_H
#define DISPLAY_H

#include "chess_types.h"

// Functions for display handling
int init_display();
void display_board(const struct chess_piece board[][8]);
void end_display();

#endif /* DISPLAY_H */
