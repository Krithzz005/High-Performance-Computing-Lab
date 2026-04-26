[23bcs074@mepcolinux e4]$cat comm.c
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

int main(void)
{
    int rank, size;
    int N;
    int search_num;
    int *array = NULL;
    int *sub_array;
    int *counts = NULL;
    int local_count = 0,total_count=0;
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (rank == 0) {
        printf("Enter total number of elements: ");
        fflush(stdout);
        scanf("%d", &N);
        printf("Enter number to find frequency: ");
        fflush(stdout);
        scanf("%d", &search_num);
    }
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&search_num, 1, MPI_INT, 0, MPI_COMM_WORLD);
    int elements_per_process = N / size;
    sub_array = (int *)malloc(elements_per_process * sizeof(int));
    if (rank == 0) {
        array = (int *)malloc(N * sizeof(int));

        printf("Generated Array: ");
        for (int i = 0; i < N; i++) {
            array[i] = rand() % 10;
            printf("%d ", array[i]);
        }
        printf("\n");

        counts = (int *)malloc(size * sizeof(int));
    }
    MPI_Scatter(array, elements_per_process, MPI_INT,
                sub_array, elements_per_process, MPI_INT,
                0, MPI_COMM_WORLD);
    for (int i = 0; i < elements_per_process; i++) {
        if (sub_array[i] == search_num) {
            local_count++;
        }
    }
    MPI_Reduce(&local_count,&total_count,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
    if (rank == 0) {
        /*int total_count = 0;
        for (int i = 0; i < size; i++) {
            total_count += counts[i];
        }*/
        printf("Frequency of %d is %d\n", search_num, total_count);
    }
    free(sub_array);
    if (rank == 0) {
        free(array);
        free(counts);
    }
    MPI_Finalize();
    return 0;
}

student@cil30:~$ mpicc p2.c
student@cil30:~$ mpiexec -n 4 ./a.out
Enter total number of elements: 8
Enter number to find frequency: 4
Generated Array: 3 6 7 5 3 5 6 2
Frequency of 4 is 0
student@cil30:~$ mpiexec -n 4 ./a.out
Enter total number of elements: 8
Enter number to find frequency: 3
Generated Array: 3 6 7 5 3 5 6 2
Frequency of 3 is 2

[23bcs074@mepcolinux e4]$exit
exit
