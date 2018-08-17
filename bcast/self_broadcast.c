#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<time.h>
#define N 32

void broadcast(char *buffer, int number, MPI_Datatype datatype, int source_process, MPI_Comm communicator)
{
    MPI_Status status;
    int rank, size;
    int tag = 1;
    int src, dst;
    int relative_rank, mask;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

//    if (rank == source_process)
//    {
//        for (int i = 0; i < N; i++)
//        {
//            buffer[i] = 'b';
//        }
//        for(int dest = 0; dest < size; dest++)
//        {
//            if (dest == source_process)
//                continue;
//            MPI_Send(buffer, number, datatype, dest, tag, communicator);
//            printf("my rank is %d, char is %c, send to rank %d\n", rank, buffer[0], dest);
//        }
//    }
//    else
//    {
//        MPI_Recv(buffer, number, datatype, source_process,tag, communicator, &status);
//        printf("my rank is %d, char is %c, receive from source %d\n", rank, buffer[0], source_process);
//    }
    if (rank == source_process) {
        for (int i = 0; i < number; i++) {
            buffer[i] = 'b';
        }
    }
    relative_rank = (rank >= source_process) ? rank - source_process : rank - source_process + size;
    mask = 0x1;
    while (mask < size) {
        if (relative_rank & mask) {
            src = rank - mask;
            if (src < 0) 
                src += size;
            MPI_Recv(buffer, number, datatype, src, tag, communicator, &status);
            printf("my rank is %d, char is %c, receive from source %d\n", rank, buffer[0], src);
            break;
        }
        mask <<= 1;
    }
    
    mask >>= 1;
    while (mask > 0) {
        if (relative_rank + mask < size) {
            dst = rank + mask;
            if (dst >= size)
                dst -= size;
            MPI_Send(buffer, number, datatype, dst, tag, communicator);
            printf("my rank is %d, char is %c, sendt to %d\n", rank, buffer[0], dst);
        }
        mask >>= 1;
    }
    MPI_Barrier(communicator);    
    printf("my rank is %d, buffer[0] is %c\n", rank, buffer[0]);
}

int main (int argc, char *argv[])
{
    int size, rank;
    int tag = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    char *buffer = malloc(sizeof(char)*N);

    broadcast(buffer, N, MPI_BYTE, 4, MPI_COMM_WORLD);
//    printf("hello my rank is %d in %d process\n", rank, size);
    MPI_Finalize();
    return 0;
}
