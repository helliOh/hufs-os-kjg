#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <pthread.h>

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX 1024	// max client data length
#define PORT 30000	// server port #
#define BACKLOG 5	// queue length

void *clientHandler(void *arg);

int main (int argc, char *argv[]){

	int sd, cliaddsize;
	struct sockaddr_in cliaddr, servaddr;

	if (( sd = socket (AF_INET, SOCK_STREAM, 0)) < 0){
		fprintf( stderr, "can’t open socket.\n");
		exit(1);
	}
     // to bind the server itself to the socket
	bzero ((char*) &servaddr, sizeof( servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl (INADDR_ANY);
	servaddr.sin_port = htons (PORT);
	if (bind (sd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
		fprintf (stderr, "can’t bind to socket.\n");
		exit(1);
	}  // bind itself to the socket

	listen (sd, BACKLOG);		// declare the client-queue length

	while (1){	// a typical server waiting loop
		int cliaddrsize = sizeof (cliaddr);
		int nsd;
		int *nsdp = &nsd;

		if (( nsd = accept (sd, (struct sockaddr *) &cliaddr, &cliaddrsize)) < 0){
			fprintf (stderr, "can’t accept connection.\n");
			exit(1);
		}  // upon return: client addr. is known and a new socket is created

		pthread_t tid;
		pthread_create(&tid, NULL, clientHandler, (void *)nsdp);
	}
} /* main */

void *clientHandler(void *arg){
  int nsd = *(int *)arg;
	char data[MAX];
	int bytes;

	while(1){
		bytes = recv (nsd, data, MAX, 0);  // from client
		if (bytes == 0)	//  client quit
					break;
		else if (bytes < 0) {  // error
					fprintf (stderr, "can’t receive data.\n"); exit(1);
		}
		if (send (nsd, data, bytes, 0) != bytes) { // echo back
					fprintf (stderr, "can’t send data.\n"); exit(1);
		}
	}

	close (nsd);
}
