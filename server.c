//
//	Author: Navneet Singh Gill, Mohammed Sarfaraz Ali Khan
//
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<netdb.h>

void servicePlayers(int client1,int client2, int gameCount);

int main(int argc, char *argv[]) {
	int socketDescriptor, client1,client2;
	int portNumber, numberOfConcurrentGamesPossible, gamesCounter;
	struct sockaddr_in serverAddress;

	//Verify required port
	if(argc != 2 && argc !=3 ) {
		fprintf(stderr, "Call model #1: %s <PortNumber>\n", argv[0]);
		fprintf(stderr, "Call model #2: %s <PortNumber> <NumberOfConcurrentGamesPossible>\n", argv[0]);
		exit(0);
	}
	//Create socket
	if( (socketDescriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "Socket could not be created\n");
		exit(0);
	}
	if(argc == 3) {
		sscanf(argv[2], "%d", &numberOfConcurrentGamesPossible);
	} else {
		numberOfConcurrentGamesPossible = 10;
	}

	sscanf(argv[1], "%d", &portNumber);
	serverAddress.sin_port = htons(portNumber);
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(socketDescriptor, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
	listen(socketDescriptor, numberOfConcurrentGamesPossible*2);

	gamesCounter = 0;
	while(gamesCounter != (numberOfConcurrentGamesPossible) ) {
		gamesCounter++;
		printf("Waiting for players to start Game#%d..\n", gamesCounter);

		client1 = accept(socketDescriptor,(struct sockaddr*) NULL, NULL);
		printf("TOTO signed in for Game#%d\n", gamesCounter);

		client2 = accept(socketDescriptor,(struct sockaddr*) NULL, NULL);
		printf("TITI signed in for Game#%d\n", gamesCounter);

		printf("Game #%d began\n", gamesCounter);

		if( fork() == 0 )
			servicePlayers(client1, client2, gamesCounter);
	}
}
void servicePlayers(int client1,int client2, int gameCount) {
	int pointsToWin = 100, client1_points = 0, client2_points = 0;
	int bufferLength = 50;
	int32_t client1_buffer, client2_buffer;	
	char *winnerName;

	char *playMessage = "You can now play";
	char *winnerMessage = "Game over: You won the game";
	char *loserMessage = "Game over: You lost the game";

	while(1) {
		sleep(1);
		write(client1, playMessage, bufferLength);
		if( read(client1, &client1_buffer , bufferLength) < 0)
			printf("Error reading from Client1 (Game#%d)\n", gameCount);

		client1_points += ntohl(client1_buffer);
		if( client1_points >= pointsToWin) {
			write(client1, winnerMessage, bufferLength);
			write(client2, loserMessage, bufferLength);
			winnerName = "TOTO";
			break;
		}

		sleep(1);
		write(client2, playMessage, bufferLength);
		if( read(client2, &client2_buffer , bufferLength) < 0)
			printf("Error reading from Client2 (Game#%d)\n", gameCount);

		client2_points += ntohl(client2_buffer);
		if( client2_points >= pointsToWin) {
			write(client2, winnerMessage, bufferLength);
			write(client1, loserMessage, bufferLength);
			winnerName = "TITI";
			break;
		}
	}
	printf("%s won Game#%d\n", winnerName, gameCount);

	close(client1);
	close(client2);
	exit(0);
}


