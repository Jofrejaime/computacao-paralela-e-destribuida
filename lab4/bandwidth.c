/*
	Name: Latency and Bandwidth Benchmark
	Description: 
	Modified sendReceive.c to measure message latency and network bandwidth.
	Measures communication costs for different message sizes.
*/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>

int main(int argc, char *argv[]) {

    MPI_Status status;
    int id, p, i, rounds, msg_size;
    double secs, latency, bandwidth;
    char *buffer;
    const int MAX_MSG_SIZE = 1048576;  /* 1 MB */

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    if(argc != 3){
        if (!id) printf("Command line: %s <n-rounds> <message-size-bytes>\n", argv[0]);
        MPI_Finalize();
        exit(1);
    }

    rounds = atoi(argv[1]);
    msg_size = atoi(argv[2]);

    if(msg_size > MAX_MSG_SIZE){
        if(!id) printf("Message size too large. Max: %d bytes\n", MAX_MSG_SIZE);
        MPI_Finalize();
        exit(1);
    }

    buffer = (char *)malloc(msg_size);
    if(!buffer){
        if(!id) printf("Memory allocation failed\n");
        MPI_Finalize();
        exit(1);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    secs = -MPI_Wtime();

    /* Ring communication with message_size bytes per message */
    for(i = 0; i < rounds; i++){
        if(!id){
            MPI_Send(buffer, msg_size, MPI_BYTE, 1, i, MPI_COMM_WORLD);
            MPI_Recv(buffer, msg_size, MPI_BYTE, p-1, i, MPI_COMM_WORLD, &status);
        }
        else{
            MPI_Recv(buffer, msg_size, MPI_BYTE, id-1, i, MPI_COMM_WORLD, &status);
            MPI_Send(buffer, msg_size, MPI_BYTE, (id+1)%p, i, MPI_COMM_WORLD);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    secs += MPI_Wtime();

    if(!id){
        /* Calculate latency: time for a single message round-trip divided by 2 */
        latency = (secs * 1e6) / (2.0 * rounds * p);
        
        /* Calculate bandwidth: bytes transferred per second */
        double total_bytes = (double)msg_size * 2.0 * rounds * p;
        bandwidth = (total_bytes / (1024*1024)) / secs;  /* MB/s */

        printf("Message Size: %d bytes\n", msg_size);
        printf("Rounds: %d, Processes: %d\n", rounds, p);
        printf("Total Time: %12.6f sec\n", secs);
        printf("Latency: %10.2f us\n", latency);
        printf("Bandwidth: %10.2f MB/s\n\n", bandwidth);
    }

    free(buffer);
    MPI_Finalize();
    return 0;
}
