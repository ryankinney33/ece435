#ifndef CHESS_H
#define CHESS_H

#include "chess_types.h"


// Team is assumed to be an array of 16 chess_piece stuctures
int init_team(struct chess_piece *team, int color_enabled, enum team_color color);


#endif /* CHESS_H */
