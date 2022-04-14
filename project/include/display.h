#ifndef DISPLAY_H
#define DISPLAY_H

#include "battleship_types.h"

// Functions for display handling

// Start the curses mode and colors (if applicable)
int init_display(int use_color);

// End the curses window for clean program exit
void end_display();

// Display the battleship grids on the screen
void display_grids(const struct game *btlshp);

// Display a message depending on if its the user turn or enemy
// Message on top row
void display_status(const char *msg);

// Display a message on the second row of the screen
void display_message(const char *msg);

// Getting input from the user
// buf is assumed to be size + 1 bytes
void get_user_input(const char *prompt, char *buf, int size);

#endif /* DISPLAY_H */
