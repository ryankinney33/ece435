#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "battleship_types.h"
#include "network.h"

// Opens a TCP socket on the port and waits for a connection
int host_game(uint16_t port, struct game *btlshp)
{
	if (btlshp == NULL) {
		return -1; // Sanity check
	}

	struct sockaddr_in server_addr;

	/*
	 * Open a socket to listen on;
	 * AF_INET = IPv4
	 * SOCK_STREAM is a reliable two-way connection (TCP)
	 */
	btlshp->serv_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (btlshp->serv_fd < 0) {
		perror("socket");
		return -1;
	}

	/*
	 * Set SO_REUSEADDR so the socket does not enter TIME_WAIT on
	 * early exit
	 */
	char on = 1;
	if (setsockopt(btlshp->serv_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) {
		perror("setsockopt");
		return -1;
	}

	// Set the server address to listen to 0.0.0.0 (any interface)
	memset(&server_addr, 0, sizeof(struct sockaddr_in));
	server_addr.sin_family = AF_INET;

	// Convert the port to network byte order
	server_addr.sin_port = htons(port);

	// Bind to the port
	if (bind(btlshp->serv_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		perror("bind");
		return -1;
	}

	// Listen on the port, only allow one connection
	if (listen(btlshp->serv_fd, 1) < 0) {
		perror("listen");
		return -1;
	}

	// Get a connection from a client
	struct sockaddr client_addr;
	socklen_t client_len;
	btlshp->enemy_fd = accept(btlshp->serv_fd, (struct sockaddr *)&client_addr, &client_len);
	if (btlshp->enemy_fd < 0) {
		perror("accept");
	}

	// Done!
	return 0;
}
