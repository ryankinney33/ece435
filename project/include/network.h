#ifndef NETWORK_H
#define NETWORK_H

#include <stdint.h>
#include "battleship_types.h"

// Opens a TCP socket on the port and waits for a connection
int host_game(uint16_t port, struct game *btlshp);

// Open a TCP connection to the host
int join_game(const char *hostname, uint16_t port, struct game *btlshp);

// Send a message to the enemy
int send_to_enemy(const char *message, struct game *btlshp);

// Read the message from enemy
int read_from_enemy(char *msg, struct game *btlshp);

#endif
