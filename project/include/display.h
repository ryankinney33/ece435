#ifndef DISPLAY_H
#define DISPLAY_H

#include "chess_types.h"

// Functions for display handling
int init_display(int use_color);
void end_display();
void display_board(const struct chess_board *board);

// Getting input from the user
void get_user_input(char *buffer);

#endif /* DISPLAY_H */
