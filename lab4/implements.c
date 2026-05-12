include <unistd.h>
#include <stdio.h>

// incluir a biblioteca MPI e a biblioteca de entrada e saída padrão
#include <mpi.h>

void MPI_Scatter(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
                 void *recvbuf, int recvcount, MPI_Datatype recvtype,
                 int root, MPI_Comm comm)
{
    int rank;
    int p;
    int i;

    MPI_Comm_size(comm, &p);
    MPI_Comm_rank(comm, &rank);
    if (rank == root)
    {
        chunk = sendcount / p;
        for (i = 0; i < p; i++)
            MPI_Send(sendbuf + i * chunk, chunk, sendtype, i, 25, comm);
    }
    MPI_Recv(recvbuf, recvcount, recvtype, root, 25, comm, MPI_STATUS_IGNORE);
}

void MPI_Gather(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
                 void *recvbuf, int recvcount, MPI_Datatype recvtype,
                 int root, MPI_Comm comm)
{
    int rank;
    int p;
    int i;

    MPI_Comm_size(comm, &p);
    MPI_Comm_rank(comm, &rank);
    if (rank == root)
    {
        chunk = recvcount / p;
        for (i = 0; i < p; i++)
            MPI_Recv(recvbuf + i * chunk, chunk, recvtype, i, 25, comm, MPI_STATUS_IGNORE);
    }
    MPI_Send(sendbuf, sendcount, sendtype, root, 25, comm);
}

void MPI_Allgather(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
                 void *recvbuf, int recvcount, MPI_Datatype recvtype,
                 MPI_Comm comm)
{
    int rank;
    int p;
    int i;

    MPI_Comm_size(comm, &p);
    MPI_Comm_rank(comm, &rank);
    chunk = recvcount / p;
    for (i = 0; i < p; i++)
        if (i == rank)
            memcpy(recvbuf + i * chunk, sendbuf, chunk);
        else
            MPI_Recv(recvbuf + i * chunk, chunk, recvtype, i, 25, comm, MPI_STATUS_IGNORE);
    for (i = 0; i < p; i++)
        if (i != rank)
            MPI_Send(sendbuf, sendcount, sendtype, i, 25, comm);
}

void MPI_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm)
{
    int rank;
    int p;
    int i;

    MPI_Comm_size(comm, &p);
    MPI_Comm_rank(comm, &rank);
    if (rank == root)
        for (i = 0; i < p; i++)
            if (i != root)
                MPI_Send(buffer, count, datatype, i, 25, comm);
    else
        MPI_Recv(buffer, count, datatype, root, 25, comm, MPI_STATUS_IGNORE);
}

void MPI_Reduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype,
                 MPI_Op op, int root, MPI_Comm comm)
{
    int rank;
    int p;
    int i;

    MPI_Comm_size(comm, &p);
    MPI_Comm_rank(comm, &rank);
    if (rank == root)
    {
        memcpy(recvbuf, sendbuf, count);
        for (i = 0; i < p; i++)
            if (i != root)
            {
                MPI_Recv(buffer, count, datatype, i, 25, comm, MPI_STATUS_IGNORE);
                // aplicar a operação de redução entre recvbuf e buffer
            }
    }
    else
        MPI_Send(sendbuf, count, datatype, root, 25, comm);
}

void MPI_Allreduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype,
                 MPI_Op op, MPI_Comm comm)
{
    int rank;
    int p;
    int i;

    MPI_Comm_size(comm, &p);
    MPI_Comm_rank(comm, &rank);
    memcpy(recvbuf, sendbuf, count);
    for (i = 0; i < p; i++)
        if (i != rank)
        {
            MPI_Recv(buffer, count, datatype, i, 25, comm, MPI_STATUS_IGNORE);
            // aplicar a operação de redução entre recvbuf e buffer
        }
    for (i = 0; i < p; i++)
        if (i != rank)
            MPI_Send(recvbuf, count, datatype, i, 25, comm);
}
void MPI_Alltoall(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
                 void *recvbuf, int recvcount, MPI_Datatype recvtype,
                 MPI_Comm comm)
{
    int rank;
    int p;
    int i;

    MPI_Comm_size(comm, &p);
    MPI_Comm_rank(comm, &rank);
    chunk = recvcount / p;
    for (i = 0; i < p; i++)
        if (i == rank)
            memcpy(recvbuf + i * chunk, sendbuf, chunk);
        else
            MPI_Recv(recvbuf + i * chunk, chunk, recvtype, i, 25, comm, MPI_STATUS_IGNORE);
    for (i = 0; i < p; i++)
        if (i != rank)
            MPI_Send(sendbuf, sendcount, sendtype, i, 25, comm);
}

void MPI_Barrier(MPI_Comm comm)
{
    int rank;
    int p;
    int i;

    MPI_Comm_size(comm, &p);
    MPI_Comm_rank(comm, &rank);
    for (i = 0; i < p; i++)
        if (i != rank)
            MPI_Send(NULL, 0, MPI_BYTE, i, 25, comm);
    for (i = 0; i < p; i++)
        if (i != rank)
            MPI_Recv(NULL, 0, MPI_BYTE, i, 25, comm, MPI_STATUS_IGNORE);
}
 
int main(int ac, char **av)
{
    int a;
    MPI_Init(&ac, &av);
    MPI_Finalize();
}
