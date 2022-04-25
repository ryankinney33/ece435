#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

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
	int on = 1;
	if (setsockopt(btlshp->serv_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on))) {
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
	socklen_t client_len = sizeof(client_addr);
	while (1) {
		btlshp->enemy_fd = accept(btlshp->serv_fd, &client_addr, &client_len);
		if (errno == EINTR) {
			continue;
		} else if (btlshp->enemy_fd < 0) {
			perror("accept");
			return -1;
		} else {
			return 0;
		}
	}

	// Done!
	return 0;
}

// Open a TCP connection to the host
int join_game(const char *hostname, uint16_t port, struct game *btlshp)
{
	if (btlshp == NULL) {
		return -1; // Sanity check
	}
	btlshp->serv_fd = -1;

	/*
	 * Open a socket to connect to
	 * AF_INET = IPv4
	 * SOCK_STREAM is a reliable two-way connection (TCP)
	 */
	btlshp->enemy_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (btlshp->enemy_fd < 0) {
		perror("socket");
		return -1;
	}

	// Look up the server info based on its name
	struct hostent *server = gethostbyname(hostname);
	if (server == NULL) {
		fprintf(stderr, "Error: No such host.\n");
		return -1;
	}

	// Set the address and port of the server
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	memcpy(server->h_addr, &server_addr.sin_addr.s_addr, server->h_length);
	server_addr.sin_port = htons(port);

	// Connect to the server
	if (connect(btlshp->enemy_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		perror("connect");
		return -1;
	}

	return 0;
}

// Send a message to the enemy
int send_to_enemy(const char *message, struct game *btlshp)
{
	// sanity check
	if (message == NULL) {
		errno = EFAULT;
		return -1;
	}

	// Send the message to the enemy
	return send(btlshp->enemy_fd, message, strlen(message) + 1, 0);
}

/*
 * Read the message from enemy
 * The message is allocated by the caller and should
 * hold 5 bytes
 */
int read_from_enemy(char *msg, struct game *btlshp)
{
	if (msg == NULL || btlshp == NULL) {
		errno = EFAULT;
		return -1;
	}

	msg[4] = '\0';
	return recv(btlshp->enemy_fd, msg, 5, 0);
}

