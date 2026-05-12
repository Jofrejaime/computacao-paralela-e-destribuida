/*
	Name: Broadcast vs Individual Send Comparison
	Description: 
	Compares performance of MPI_Bcast vs individual MPI_Send for distributing a large array.
*/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>

int main(int argc, char *argv[]) {

    MPI_Status status;
    int id, p, i, rounds, array_size;
    double secs_bcast, secs_send, rate_bcast, rate_send;
    int *array;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    if(argc != 3){
        if (!id) printf("Command line: %s <n-rounds> <array-size>\n", argv[0]);
        MPI_Finalize();
        exit(1);
    }

    rounds = atoi(argv[1]);
    array_size = atoi(argv[2]);

    array = (int *)malloc(array_size * sizeof(int));
    if(!array){
        if(!id) printf("Memory allocation failed\n");
        MPI_Finalize();
        exit(1);
    }

    /* Initialize array with values (only on process 0) */
    if(!id){
        for(i = 0; i < array_size; i++){
            array[i] = i;
        }
    }

    /* ============================================ */
    /* Test 1: MPI_Bcast                           */
    /* ============================================ */
    MPI_Barrier(MPI_COMM_WORLD);
    secs_bcast = -MPI_Wtime();

    for(i = 0; i < rounds; i++){
        MPI_Bcast(array, array_size, MPI_INT, 0, MPI_COMM_WORLD);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    secs_bcast += MPI_Wtime();

    /* ============================================ */
    /* Test 2: Individual MPI_Send from process 0  */
    /* ============================================ */
    MPI_Barrier(MPI_COMM_WORLD);
    secs_send = -MPI_Wtime();

    for(i = 0; i < rounds; i++){
        if(!id){
            int j;
            for(j = 1; j < p; j++){
                MPI_Send(array, array_size, MPI_INT, j, i, MPI_COMM_WORLD);
            }
        }
        else{
            MPI_Recv(array, array_size, MPI_INT, 0, i, MPI_COMM_WORLD, &status);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    secs_send += MPI_Wtime();

    /* ============================================ */
    /* Print Results (only process 0)              */
    /* ============================================ */
    if(!id){
        double bytes_per_round = (double)array_size * sizeof(int) * (p - 1);
        double total_bytes_bcast = bytes_per_round * rounds;
        double total_bytes_send = bytes_per_round * rounds;

        rate_bcast = (total_bytes_bcast / (1024*1024)) / secs_bcast;  /* MB/s */
        rate_send = (total_bytes_send / (1024*1024)) / secs_send;      /* MB/s */

        printf("\n========== Broadcast vs Individual Send Comparison ==========\n\n");
        printf("Array Size: %d integers (%.2f MB per element distribution)\n", array_size, 
               (double)(array_size * sizeof(int)) / (1024*1024));
        printf("Rounds: %d, Number of Processes: %d\n\n", rounds, p);

        printf("--- MPI_Bcast Method ---\n");
        printf("  Total Time: %12.6f sec\n", secs_bcast);
        printf("  Throughput: %10.2f MB/s\n", rate_bcast);
        printf("  Time per broadcast: %10.6f sec\n\n", secs_bcast / rounds);

        printf("--- Individual MPI_Send Method ---\n");
        printf("  Total Time: %12.6f sec\n", secs_send);
        printf("  Throughput: %10.2f MB/s\n", rate_send);
        printf("  Time per send round: %10.6f sec\n\n", secs_send / rounds);

        printf("--- Performance Comparison ---\n");
        if(secs_bcast < secs_send){
            printf("  MPI_Bcast is FASTER by: %6.2f%% (%.6f sec saved)\n", 
                   ((secs_send - secs_bcast) / secs_send) * 100, secs_send - secs_bcast);
        }
        else if(secs_send < secs_bcast){
            printf("  Individual Send is FASTER by: %6.2f%% (%.6f sec saved)\n",
                   ((secs_bcast - secs_send) / secs_bcast) * 100, secs_bcast - secs_send);
        }
        else{
            printf("  Performance is equivalent\n");
        }
        printf("  Ratio (Bcast/Send): %.4f\n\n", secs_bcast / secs_send);
    }

    free(array);
    MPI_Finalize();
    return 0;
}
