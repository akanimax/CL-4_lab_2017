#include <mpi.h>
#include <stdio.h>
#include <unistd.h>
#define MSG_SIZE 40
int main(int argc, char **argv)
{
	int rank;
	char hostname[256];
	char message[MSG_SIZE];
	char incoming[MSG_SIZE];
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	gethostname(hostname,255);

	printf("Hello world!  I am process number: %d on host %s\n", rank, hostname);
	if(rank==0) {
		// send a message here!
		printf("What message would you like to send to slave?\n");
		scanf("%s", message);
		MPI_Send(message, MSG_SIZE, MPI_BYTE, 1, 0, MPI_COMM_WORLD);
	}
	else if(rank==1) {
		// display the message here
		MPI_Recv(incoming, MSG_SIZE, MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		printf("The master sent: %s\n", incoming);
		printf("The message's value is %s\n", message);
	}

	MPI_Finalize();

	return 0;
}
