#ifndef DISPLAY_H
#define DISPLAY_H

#include "chess_types.h"

// Functions for display handling
int init_display(int use_color);
void display_board(const struct chess_board *board);
void end_display();

#endif /* DISPLAY_H */
