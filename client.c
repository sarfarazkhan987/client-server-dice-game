//
//	Author: Navneet Singh Gill, Mohammed Sarfaraz Ali Khan
//
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<sys/time.h>
#include<sys/sysinfo.h>
#include<time.h>

int main(int argc, char *argv[]) {
	int socketDescriptor;
	int portNumber;
	struct sockaddr_in serverAddress;
	int dice;
	int bufferLength = 50;
	char serverMessage[100];
	int32_t convertedDiceValue;
	int size=sizeof(convertedDiceValue);
	struct timespec timeInterval;
	char *playMessageExpected = "You can now play";
	char *winnerMessageExpected = "Game over: You won the game";
	char *loserMessageExpected = "Game over: You lost the game";

	if(argc != 3) {
		printf("Call model: %s <IP> <PortNumber>\n", argv[0]);
		exit(0);
	}
	if ( (socketDescriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
		fprintf(stderr, "Socket could not be created\n");
		exit(0);
	}
	serverAddress.sin_family = AF_INET;
	sscanf(argv[2], "%d", &portNumber);
	serverAddress.sin_port = htons(portNumber);

	//convert IPv4 and IPv6 addresses from text to binary form
	if( inet_pton(AF_INET, argv[1], &serverAddress.sin_addr) < 0 ) {
		fprintf(stderr, "inet_pton() failed\n");
		exit(0);
	}
	if( connect(socketDescriptor, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0 ) {
		fprintf(stderr, "Error connecting to server\n");
		exit(0);
	}
	while(1) {
		if( read(socketDescriptor, serverMessage, bufferLength) < 0 ) {
			fprintf(stderr, "Error reading from server\n");
		}

		if( !strcmp(serverMessage, playMessageExpected) ) {
			clock_gettime(CLOCK_MONOTONIC_RAW, &timeInterval);
			dice = (int)(timeInterval.tv_nsec + timeInterval.tv_sec)%10 + 1;
			printf("I got: %d\n",dice);

			convertedDiceValue = htonl(dice);
			write(socketDescriptor, &convertedDiceValue, size);
		} else if( !strcmp(serverMessage, winnerMessageExpected) ) {
			printf("I won the game\n");
			break;
		} else if( !strcmp(serverMessage, loserMessageExpected) ) {
			printf("I lost the game\n");
			break;
		}
	}
	close(socketDescriptor);
	exit(0);
}


