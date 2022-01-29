#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stddef.h>

#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER_SIZE	256

/* Default port to listen on */
#define DEFAULT_PORT	8080

int main(int argc, char **argv) {

	int socket_fd,new_socket_fd;
	struct sockaddr_in server_addr, client_addr;
	int port=DEFAULT_PORT;
	int n,on=1;
	socklen_t client_len;
	char buffer[BUFFER_SIZE];

	printf("Starting server on port %d\n",port);

	/* Open a socket to listen on */
	/* AF_INET means an IPv4 connection */
	/* SOCK_STREAM means reliable two-way connection (TCP) */
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd<0) {
		fprintf(stderr,"Error opening socket! %s\n",
			strerror(errno));
		exit(1);
	}

	/* avoid TIME_WAIT */
	setsockopt(socket_fd,SOL_SOCKET,SO_REUSEADDR,(char *)&on,sizeof(on));

	/* Set up the server address to listen on */
	/* The memset stes the address to 0.0.0.0 which means */
	/* listen on any interface. */
	memset(&server_addr,0,sizeof(struct sockaddr_in));
	server_addr.sin_family=AF_INET;
	/* Convert the port we want to network byte order */
	server_addr.sin_port=htons(port);

	/* Bind to the port */
	if (bind(socket_fd, (struct sockaddr *) &server_addr,
		sizeof(server_addr)) <0) {
		fprintf(stderr,"Error binding! %s\n", strerror(errno));
		fprintf(stderr,"Probably in time wait, have to wait 60s if you ^C to close\n");
		exit(1);
	}

	/* Tell the server we want to listen on the port */
	/* Second argument is backlog, how many pending connections can */
	/* build up */
	listen(socket_fd,5);

wait_for_connection:

	/* Call accept to create a new file descriptor for an incoming */
	/* connection.  It takes the oldest one off the queue */
	/* We're blocking so it waits here until a connection happens */
	client_len=sizeof(client_addr);
	new_socket_fd = accept(socket_fd,
			(struct sockaddr *)&client_addr,&client_len);
	if (new_socket_fd<0) {
		fprintf(stderr,"Error accepting! %s\n",strerror(errno));
		exit(1);
	}

	char *filename; /* The name of the requested file */
	long int fname_length = 0; /* The size of the filename array */
	int getting_name = 0; /* Flag for long filenames */

	while(1) {
		/* Someone connected!  Let's try to read BUFFER_SIZE-1 bytes */
		memset(buffer,0,BUFFER_SIZE);
		n = read(new_socket_fd,buffer,(BUFFER_SIZE-1));
		if (n==0) {
			fprintf(stderr,"Connection to client lost\n\n");
			break;
		}
		else if (n<0) {
			fprintf(stderr,"Error reading from socket %s\n",
				strerror(errno));
		}

		/* Print the message we received */
		printf("Message received: %s\n",buffer);


		/* Find the "GET" in the request */
		char *get = strstr(buffer, "GET /");

		/* Check if "GET" was found */
		if (get != NULL) {
			/* Look for a space after the filename */
			char *space = strstr(get+5, " ");

			if (space == NULL) {
				/*
				 * Space not found; the requested file name does not fit in the buffer
				 * Save what was found into the filename buffer
				 */
				fname_length = BUFFER_SIZE-5;

				filename = malloc(fname_length * sizeof(char));
				if (filename == NULL) {
					fprintf(stderr, "Memory allocation error: %s\n", strerror(errno));
					break;
				}

				strcpy(filename, buffer+5);

				getting_name = 1;
			} else {
				
//				/* End of the filename detected; get the length */
//				if (getting_name) {
//					/* Handle long file names */
//					ptrdiff_t size = space - buffer;
//					fname_length += size;
//
//					filename = realloc(filename, fname_length);
//					if (filename == NULL) {
//						fprintf(stderr, "Memory allocation error:%s\n", strerror(errno));
//						break;
//					}
//
//					/* Append the rest of the filename to the string */
//					strncat(filename, buffer, size);
					fname_length = space - get - 5;
				filename = malloc(fname_length * sizeof(char));
				if (filename == NULL) {
					fprintf(stderr, "Memory allocation error: %s\n", strerror(errno));
					break;
				}

				strncpy(filename, get+5, fname_length);

				fprintf(stderr, "File name = '%s'\n",filename);

				/* Reset the flags and deallocate the filename string */
				getting_name = 0;
				fname_length = 0;
				free(filename);
			}
		}


//		ptrdiff_t size = space-get+5;

//		printf("Filename length = %li\n", size);

		/* Send a response */
	/*	n = write(new_socket_fd,"Got your message, thanks!\r\n\r\n",29);
		if (n<0) {
			fprintf(stderr,"Error writing. %s\n",
				strerror(errno));
		}*/


	}

	close(new_socket_fd);

	printf("Done connection, go back and wait for another\n\n");

	goto wait_for_connection;

	/* Close the sockets */
	close(socket_fd);

	return 0;
}
