#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define BUFFER_SIZE	256

/* Default port to listen on */
#define DEFAULT_PORT	31337

int main(int argc, char **argv) {

	int socket_fd;
	struct sockaddr_in server_addr, client_addr;
	int port=DEFAULT_PORT;
	int n;
	socklen_t client_len;
	char buffer[BUFFER_SIZE];
	int optval;
	struct hostent *hostp;
	char *hostaddrp; /* IP address */
	unsigned short int client_port; /* Client port */

	printf("Starting UDP server on port %d\n",port);

	/* Open a socket to listen on */
	/* AF_INET means an IPv4 connection */
	/* SOCK_DGRAM is UDP */
	socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (socket_fd<0) {
		fprintf(stderr,"Error opening socket! %s\n",
			strerror(errno));
	}

	/* setsockopt: avoid TIME WAIT?? */
	optval = 1;
	setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR,
		(const void *)&optval , sizeof(int));

	/* Set up the server address to listen on */
	memset(&server_addr,0,sizeof(struct sockaddr_in));
	server_addr.sin_family=AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	/* Convert the port we want to network byte order */
	server_addr.sin_port=htons(port);

	/* Bind to the port */
	if (bind(socket_fd, (struct sockaddr *) &server_addr,
		sizeof(server_addr)) <0) {
		fprintf(stderr,"Error binding! %s\n", strerror(errno));
	}

	client_len=sizeof(client_addr);

	while(1) {

		/* Wait until we receive a UDP packet */
		memset(buffer,0,BUFFER_SIZE);


		/* Read from socket */
		n=recvfrom(socket_fd,buffer,(BUFFER_SIZE-1), 0,
				(struct sockaddr*)&client_addr, &client_len);

		if (n==0) {
			fprintf(stderr,"Connection to client lost\n\n");
			break;
		}
		else if (n<0) {
			fprintf(stderr,"Error reading from socket %s\n",
				strerror(errno));
			break;
		}

		/* Get the host and port of the incoming connection */
		hostp = gethostbyaddr((const char*)&client_addr.sin_addr.s_addr,
				sizeof(client_addr.sin_addr.s_addr), AF_INET);
		hostaddrp = inet_ntoa(client_addr.sin_addr);
		client_port = ntohs(client_addr.sin_port);

		/* Print the client address and port */
		printf("Host: %s - %s\nPort: %u\n",
				hostp->h_name, hostaddrp, client_port);

		/* Print the message we received */
		printf("Message received: %s\n", buffer);

		/* Convert the message to uppercase */
		for (int i = 0; buffer[i] && (i < BUFFER_SIZE-1); ++i) {
			buffer[i] = toupper(buffer[i]);
		}

		/* Finally, send the uppercased message back */
		n = sendto(socket_fd, buffer, strlen(buffer), 0,
				(struct sockaddr*)&client_addr, client_len);
		if (n < 0) {
			fprintf(stderr, "Error writing to socket %s\n",
					strerror(errno));
			break;
		}
	}

	printf("Exiting server\n\n");

	/* Close the sockets */
	close(socket_fd);

	return 0;
}
