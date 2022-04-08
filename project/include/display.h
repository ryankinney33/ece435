#ifndef DISPLAY_H
#define DISPLAY_H

#include "battleship_types.h"

// Functions for display handling
int init_display(int use_color);
void end_display();
void display_grids(const struct game *btlshp);

// Getting input from the user
// buf is assumed to be size + 1 bytes
void get_user_input(const char *prompt, char *buf, int size);

#endif /* DISPLAY_H */
