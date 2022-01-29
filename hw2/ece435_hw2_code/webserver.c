#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stddef.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER_SIZE	256

/* Default port to listen on */
#define DEFAULT_PORT	8080

int send_response(int socket_fd, char* filename);

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
	/* The memset sets the address to 0.0.0.0 which means */
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


	char *filename = calloc(1,sizeof(char)); /* The name of the requested file */
	long int fname_length = 1; /* The size of the filename array */
	int long_name = 0; /* Flag for long filenames */


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

	filename = calloc(1,sizeof(char));
	fname_length = 1;
	long_name = 0;

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
		printf("%s",buffer);

		/* Look for stuff */

		/* Parse the string for a file name */
		char *get;
		if (long_name)
			get = buffer - 5;
		else
			get = strstr(buffer, "GET /");

		if (get != NULL) {
			char* space = strstr(get+5, " "); /* Look for the end of the file name */

			long int size;

			if (space == NULL) {
				/* End not found */
				long_name = 1;
				size = BUFFER_SIZE - 1;
			} else {
				/* End found; get the length */
				long_name = 0;
				size = (space - get - 5);
			}

			fname_length += BUFFER_SIZE-1;

			/* Allocate memory for the file name */
			char *tmp = realloc(filename, fname_length);
			if (tmp == NULL) {
				fprintf(stderr, "Memory allocation error: %s\n", strerror(errno));
				free(filename);
				break;
			}
			filename = tmp;
			strncat(filename, get + 5, size); /* Append the data to the filename string */
		}

		/* Look for the end of the HTTP header */
		char *CRLF = strstr(buffer, "\n\r\n");
		if (n < BUFFER_SIZE - 1 || CRLF != NULL) { /* HTTP request complete; send response */
			send_response (new_socket_fd, filename);

			/* Reset flags and deallocate the filename string */
			char *tmp = realloc(filename, sizeof(char));
			if (tmp == NULL) {
				fprintf(stderr, "Memory allocation error: %s\n", strerror(errno));
				free(filename);
				break;
			}
			filename = tmp;
			filename[0] = 0;
			fname_length = 1;
			long_name = 0;
		}
	}

	free(filename);

	close(new_socket_fd);

	printf("Done connection, go back and wait for another\n\n");

	goto wait_for_connection;

	/* Close the sockets */
	close(socket_fd);

	return 0;
}

int send_response(int socket_fd, char *filename) {

	/* Get the time */
	time_t tmp = time(NULL);
	struct tm *current_time = gmtime(&tmp);
	if (current_time == NULL) {
		fprintf(stderr, "Error getting time: %s\n", strerror(errno));
		return -1;
	}

	/* Convert the time into the required format */
	char time_str[150];
	strftime(time_str, sizeof(time_str), "%a, %d %b %Y %T %Z", current_time);

	/* Check the filename */
	if (filename == NULL) {
		/* Respond with internal server error message */
		dprintf(socket_fd, "HTTP/1.1 500 Internal Server Error\r\n");
		dprintf(socket_fd, "Date: %s\r\nServer: ECE435\r\n", time_str);
		dprintf(socket_fd, "Content-Length: 216\r\nConnection: close\r\n");
		dprintf(socket_fd, "Content-Type: text/html; charset=iso-8859-1\r\n\r\n");
		dprintf(socket_fd, "<html><head>\r\n<title>500 Internal Server Error</title>\r\n</head><body>\r\n");
		dprintf(socket_fd, "<h1>Internal Server Error</h1>\r\n<p>The server encountered an internal error and was unable");
		dprintf(socket_fd, " to complete the request.<br />\r\n</p>\r\n</body></html>\r\n");

		return -1;
	}

	int fd;

	if (*filename == '\0') { /* Check if filename is an empty string */
		printf("File requested: 'index.html'\n");

		/* Attempt to open index.html */
		fd = open("index.html", O_RDONLY);
	} else {
		printf("File requested: '%s'\n",filename);

		/* Attempt to open the desired file */
		fd = open(filename, O_RDONLY);
	}

	if (fd < 0) { /* Could not open the file */
		/* Send error 404 response */
		dprintf(socket_fd, "HTTP/1.1 404 Not Found\r\nDate: %s\r\n", time_str);
		dprintf(socket_fd, "Server: ECE435\r\nContent-Length: 158\r\n");
		dprintf(socket_fd, "Connection: close\r\nContent-Type: text/html; charset=iso-8859-1\r\n\r\n");
		dprintf(socket_fd, "<html><head>\r\n<title>404 Not Found</title>\r\n</head><body>\r\n");
		dprintf(socket_fd, "<h1>Not Found</h1>\r\n<p>The requested URL was not found on the server.");
		dprintf(socket_fd, "<br />\r\n</p>\r\n</body></html>\r\n");
	} else {
		/* Get the file stats */
		struct stat filestat;
		fstat(fd, &filestat);

		/* Extract the necessary file stats */
		struct tm *mod_time = gmtime(&filestat.st_mtim.tv_sec);
		long int size = filestat.st_size;

		/* Send the HTTP header */
		dprintf(socket_fd, "HTTP/1.1 200 OK\r\nDate: %s\r\nServer: ECE435\r\n", time_str);
		strftime(time_str, sizeof(time_str), "%a, %d %b %Y %T %Z", mod_time);
		dprintf(socket_fd, "Last-Modified: %s\r\nContent-Length: %li\r\n", time_str, size);
		dprintf(socket_fd, "Content-Type: text/html\r\n\r\n");

		/* Buffer for holding data from the file */
		char buffer[BUFFER_SIZE];
		buffer[BUFFER_SIZE-1] = 0;

		int n = 0;
		do {
			/* Read the data from the file */
			n = read(fd, buffer, BUFFER_SIZE-1);
			if (n < 0) {
				fprintf(stderr, "Error reading from file: %s\n", strerror(errno));
				close(fd);
				return -1;
			}

			/* Write the data to the socket */
			if (write(socket_fd, buffer, strlen(buffer)) < 0) {
				fprintf(stderr, "Error writing to the socket: %s\n", strerror(errno));
				close(fd);
				return -1;
			}
		} while (n > 0);
	}

	return 0;
}
