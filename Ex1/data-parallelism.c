#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <time.h>

int main() {
    int m, n, p;
    int A[10][10], B[10][10], C[10][10];

    struct timespec start, end;

    printf("Enter m n p: ");
    scanf("%d %d %d", &m, &n, &p);

    printf("Enter matrix A:\n");
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            scanf("%d", &A[i][j]);

    printf("Enter matrix B:\n");
    for (int i = 0; i < n; i++)
        for (int j = 0; j < p; j++)
            scanf("%d", &B[i][j]);

    /* Start high-resolution timer */
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Serial matrix multiplication
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++) {
            C[i][j] = 0;
            for (int k = 0; k < n; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    /* Stop timer */
    clock_gettime(CLOCK_MONOTONIC, &end);

    printf("Result Matrix:\n");
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++)
            printf("%d ", C[i][j]);
        printf("\n");
    }

    double time_taken;
    time_taken = (end.tv_sec - start.tv_sec)
               + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("Serial Execution Time: %.9f seconds\n", time_taken);

    return 0;
}
[23bcs074@mepcolinux e1]$cc -std=c99 s.c
[23bcs074@mepcolinux e1]$./a.out
Enter m n p: 2 2 2
Enter matrix A:
1 2
3 4
Enter matrix B:
5 6
7 8
Result Matrix:
19 22
43 50
Serial Execution Time: 0.000000534 seconds
[23bcs074@mepcolinux e1]$cat p.c
#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    int m, n, p;

    printf("Enter m n p: ");
    scanf("%d %d %d", &m, &n, &p);
    int **A = (int **)malloc(m * sizeof(int *));
    int **B = (int **)malloc(n * sizeof(int *));
    int **C = (int **)malloc(m * sizeof(int *));

    for (int i = 0; i < m; i++)
        A[i] = (int *)malloc(n * sizeof(int));

    for (int i = 0; i < n; i++)
        B[i] = (int *)malloc(p * sizeof(int));

    for (int i = 0; i < m; i++)
        C[i] = (int *)malloc(p * sizeof(int));

    srand(time(NULL));

    /*printf("Matrix A:\n");
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            A[i][j] = rand() % 10;
            printf("%d ", A[i][j]);
        }
        printf("\n");
    }

    printf("Matrix B:\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < p; j++) {
            B[i][j] = rand() % 10;
            printf("%d ", B[i][j]);
        }
        printf("\n");
    }*/
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++) {
            C[i][j] = 0;
            for (int k = 0; k < n; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

  /* printf("Result Matrix:\n");
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++)
            printf("%d ", C[i][j]);
        printf("\n");
    }*/
    double time_taken;
    time_taken = (end.tv_sec - start.tv_sec)
               + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("Serial Execution Time: %.9f seconds\n", time_taken);
    for (int i = 0; i < m; i++) free(A[i]);
    for (int i = 0; i < n; i++) free(B[i]);
    for (int i = 0; i < m; i++) free(C[i]);
    free(A);
    free(B);
    free(C);

    return 0;
}

[23bcs074@mepcolinux e1]$cc -std=c99 p.c
[23bcs074@mepcolinux e1]$./a.out
Enter m n p: 50 50 50
Serial Execution Time: 0.001856549 seconds
[23bcs074@mepcolinux e1]$./a.out
Enter m n p: 500 500 500
Serial Execution Time: 2.363736925 seconds
[23bcs074@mepcolinux e1]$./a.out
Enter m n p: 1000 1000 1000
Serial Execution Time: 27.755955384 seconds
[23bcs074@mepcolinux e1]$cat npg.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/time.h>

#define MAX 10

struct Matrix {
    int C[MAX][MAX];
    double exe_time[MAX];
};

int main() {
    int m, n, p, i, j, k;
    int A[MAX][MAX], B[MAX][MAX];
    double max;

    printf("Enter m n p: ");
    scanf("%d %d %d", &m, &n, &p);

    printf("Enter matrix A:\n");
    for (i = 0; i < m; i++)
        for (j = 0; j < n; j++)
            scanf("%d", &A[i][j]);

    printf("Enter matrix B:\n");
    for (i = 0; i < n; i++)
        for (j = 0; j < p; j++)
            scanf("%d", &B[i][j]);

    /* As asked in the question */
    int shmid = shmget(7652, sizeof(int) * m * p,
                       IPC_CREAT | 0666);

    for (i = 0; i < m; i++) {
        int pid = fork();

        if (pid == 0) {
            struct Matrix *res = shmat(shmid, NULL, 0);
            struct timeval t1, t2;

            gettimeofday(&t1, NULL);

            /* One row computation */
            for (j = 0; j < p; j++) {
                res->C[i][j] = 0;
                for (k = 0; k < n; k++) {
                    res->C[i][j] += A[i][k] * B[k][j];
                }
            }

            gettimeofday(&t2, NULL);

            res->exe_time[i] =
                (t2.tv_sec - t1.tv_sec) +
                (t2.tv_usec - t1.tv_usec) / 1000000.0;

            shmdt(res);
            exit(0);
        }
    }

    /* Parent waits for all children */
    for (i = 0; i < m; i++)
        wait(NULL);

    /* Parent attaches shared memory */
    struct Matrix *res = shmat(shmid, NULL, 0);

    /* Find maximum execution time */
    max = res->exe_time[0];
    for (i = 1; i < m; i++) {
        if (res->exe_time[i] > max)
            max = res->exe_time[i];
    }

    printf("Result Matrix:\n");
    for (i = 0; i < m; i++) {
        for (j = 0; j < p; j++)
            printf("%d ", res->C[i][j]);
        printf("\n");
    }

    printf("Parallel Execution Time: %f seconds\n", max);

    shmdt(res);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}

[23bcs074@mepcolinux e1]$cc -std=c99 npg.c
[23bcs074@mepcolinux e1]$./a.out
Enter m n p: 2 2 3
Enter matrix A:
1 2
6
7
Enter matrix B:
1 4 5
2 3 4
Result Matrix:
5 10 13
20 45 58
Parallel Execution Time: 0.000009 seconds
[23bcs074@mepcolinux e1]$cat pran.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/time.h>

/* Structure for shared memory */
struct Matrix {
    int *C;            // Result matrix (flattened)
    double *exe_time;  // Execution time per process
};

int main() {
    int m, n, p, i, j, k;
    int **A, **B;
    double max;

    printf("\nEnter m n p: ");
    scanf("%d %d %d", &m, &n, &p);

    /* -------- Dynamic allocation for A -------- */
    A = (int **)malloc(m * sizeof(int *));
    for (i = 0; i < m; i++)
        A[i] = (int *)malloc(n * sizeof(int));

    /* -------- Dynamic allocation for B -------- */
    B = (int **)malloc(n * sizeof(int *));
    for (i = 0; i < n; i++)
        B[i] = (int *)malloc(p * sizeof(int));

    /* Seed random numbers */
    srand(getpid());

    /* Generate random matrix A */
    for (i = 0; i < m; i++)
        for (j = 0; j < n; j++)
            A[i][j] = rand() % 10;

    /* Generate random matrix B */
    for (i = 0; i < n; i++)
        for (j = 0; j < p; j++)
            B[i][j] = rand() % 10;

    /* Print Matrix A
    printf("\nMatrix A:\n");
    for (i = 0; i < m; i++) {
        for (j = 0; j < n; j++)
            printf("%d ", A[i][j]);
        printf("\n");
    }

    /* Print Matrix B
    printf("\nMatrix B:\n");
    for (i = 0; i < n; i++) {
        for (j = 0; j < p; j++)
            printf("%d ", B[i][j]);
        printf("\n");
    }

    /* Shared memory allocation */
    int shmid = shmget(IPC_PRIVATE,
                       sizeof(int) * m * p + sizeof(double) * m,
                       IPC_CREAT | 0666);

    int *shm = (int *)shmat(shmid, NULL, 0);
    int *C = shm;                    // Result matrix
    double *exe_time = (double *)(shm + m * p);

    /* Fork m processes */
    for (i = 0; i < m; i++) {
        int pid = fork();

        if (pid == 0) {
            struct timeval t1, t2;
            gettimeofday(&t1, NULL);

            for (j = 0; j < p; j++) {
                C[i * p + j] = 0;
                for (k = 0; k < n; k++) {
                    C[i * p + j] += A[i][k] * B[k][j];
                }
            }

            gettimeofday(&t2, NULL);

            exe_time[i] =
                (t2.tv_sec - t1.tv_sec) +
                (t2.tv_usec - t1.tv_usec) / 1000000.0;

            shmdt(shm);
            exit(0);
        }
    }

    /* Parent waits */
    for (i = 0; i < m; i++)
        wait(NULL);

    /* Find max execution time */
    max = exe_time[0];
    for (i = 1; i < m; i++)
        if (exe_time[i] > max)
            max = exe_time[i];

    /* Print result matrix
    printf("\nResult Matrix:\n");
    for (i = 0; i < m; i++) {
        for (j = 0; j < p; j++)
            printf("%d ", C[i * p + j]);
        printf("\n");
    }
*/
    printf("\nParallel Execution Time: %f seconds\n\n", max);

    /* Cleanup */
    shmdt(shm);
    shmctl(shmid, IPC_RMID, NULL);

    for (i = 0; i < m; i++)
        free(A[i]);
    free(A);

    for (i = 0; i < n; i++)
        free(B[i]);
    free(B);

    return 0;
}


[23bcs074@mepcolinux e1]$cc -std=c99 pran.c
[23bcs074@mepcolinux e1]$./a.out

Enter m n p: 50 50 50

Parallel Execution Time: 0.000047 seconds

[23bcs074@mepcolinux e1]$./a.out

Enter m n p: 500 500 500

Parallel Execution Time: 0.023234 seconds

[23bcs074@mepcolinux e1]$./a.out

Enter m n p: 1000 1000 1000

Parallel Execution Time: 0.101800 seconds

[23bcs074@mepcolinux e1]$exit
exit

