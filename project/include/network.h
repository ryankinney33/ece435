#ifndef NETWORK_H
#define NETWORK_H

#include <stdint.h>
#include "battleship_types.h"

// Opens a TCP socket on the port and waits for a connection
int host_game(uint16_t port, struct game *btlshp);


#endif
