[23bcs074@mepcolinux ex2]$cat s1.c
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

double determinant(int n, double A[n][n]) {
    int i, j, k;
    double det = 1.0;
    int swapCount = 0;
    for (i = 0; i < n; i++) {
        if (A[i][i] == 0) {
            int swapped = 0;
            for (j = i + 1; j < n; j++) {
                if (A[j][i] != 0) {
                    for (k = 0; k < n; k++) {
                        double temp = A[i][k];
                        A[i][k] = A[j][k];
                        A[j][k] = temp;
                    }
                    swapCount++;
                    swapped = 1;
                    break;
                }
            }
            if (!swapped) { det = 0; break; }
        }
        for (j = i + 1; j < n; j++) {
            double factor = A[j][i] / A[i][i];
            for (k = i; k < n; k++) A[j][k] -= factor * A[i][k];
        }
    }
    if (det != 0) {
        for (i = 0; i < n; i++) det *= A[i][i];
        if (swapCount % 2 != 0) det = -det;
    }
    return det;
}

int main() {
    int m1, n1, m2, n2, i, j, k;
    printf("Enter rows and columns of Matrix A: ");
    scanf("%d %d", &m1, &n1);
    printf("Enter rows and columns of Matrix B: ");
    scanf("%d %d", &m2, &n2);

    double A[m1][n1], B[m2][n2];

    printf("Enter elements of Matrix A:\n");
    for (i = 0; i < m1; i++) for (j = 0; j < n1; j++) scanf("%lf", &A[i][j]);
    printf("Enter elements of Matrix B:\n");
    for (i = 0; i < m2; i++) for (j = 0; j < n2; j++) scanf("%lf", &B[i][j]);

    struct timeval start, end;
    gettimeofday(&start, NULL);

    if (m1 == m2 && n1 == n2) {
        double add[m1][n1];
        printf("\nMatrix Addition:\n");
        for (i = 0; i < m1; i++) {
            for (j = 0; j < n1; j++) {
                add[i][j] = A[i][j] + B[i][j];
                printf("%.2lf ", add[i][j]);
            }
            printf("\n");
        }
    }

    if (m1 == m2 && n1 == n2) {
        double sub[m1][n1];
        printf("\nMatrix Subtraction:\n");
        for (i = 0; i < m1; i++) {
            for (j = 0; j < n1; j++) {
                sub[i][j] = A[i][j] - B[i][j];
                printf("%.2lf ", sub[i][j]);
            }
            printf("\n");
        }
    }

    if (n1 == m2) {
        double mul[m1][n2];
        printf("\nMatrix Multiplication:\n");
        for (i = 0; i < m1; i++) {
            for (j = 0; j < n2; j++) {
                mul[i][j] = 0;
                for (k = 0; k < n1; k++) mul[i][j] += A[i][k] * B[k][j];
                printf("%.2lf ", mul[i][j]);
            }
            printf("\n");
        }
    } else printf("\nMatrix multiplication not possible\n");

    if (m1 == n1) {
        double copy[m1][n1];
        for (i = 0; i < m1; i++) for (j = 0; j < n1; j++) copy[i][j] = A[i][j];
        printf("\nDeterminant of Matrix A = %.2lf\n", determinant(m1, copy));
    } else printf("\nDeterminant not possible for Matrix A\n");

    if (m2 == n2) {
        double copy[m2][n2];
        for (i = 0; i < m2; i++) for (j = 0; j < n2; j++) copy[i][j] = B[i][j];
        printf("\nDeterminant of Matrix B = %.2lf\n", determinant(m2, copy));
    } else printf("\nDeterminant not possible for Matrix B\n");

    gettimeofday(&end, NULL);
    double elapsed = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_usec - start.tv_usec) / 1000.0;
    printf("\nSerial execution CPU time: %.3lf milliseconds\n", elapsed);

    return 0;
}


[23bcs074@mepcolinux ex2]$cc -std=c99 s1.c
[23bcs074@mepcolinux ex2]$./a.out
Enter rows and columns of Matrix A: 2 2
Enter rows and columns of Matrix B: 2 2
Enter elements of Matrix A:
1 2 3 4
Enter elements of Matrix B:
6 4 3 3

Matrix Addition:
7.00 6.00
6.00 7.00

Matrix Subtraction:
-5.00 -2.00
0.00 1.00

Matrix Multiplication:
12.00 10.00
30.00 24.00

Determinant of Matrix A = -2.00

Determinant of Matrix B = 6.00

Serial execution CPU time: 0.122 milliseconds
[23bcs074@mepcolinux ex2]$cat s2.c
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

double determinant(int n, double **A) {
    double det = 1.0;
    int swapCount = 0;
    for (int i = 0; i < n; i++) {
        if (A[i][i] == 0) {
            int swapped = 0;
            for (int j = i + 1; j < n; j++) {
                if (A[j][i] != 0) {
                    double *temp = A[i];
                    A[i] = A[j];
                    A[j] = temp;
                    swapCount++;
                    swapped = 1;
                    break;
                }
            }
            if (!swapped) return 0;
        }
        for (int j = i + 1; j < n; j++) {
            double factor = A[j][i] / A[i][i];
            for (int k = i; k < n; k++) A[j][k] -= factor * A[i][k];
        }
    }
    for (int i = 0; i < n; i++) det *= A[i][i];
    if (swapCount % 2 != 0) det = -det;
    return det;
}

int main() {
    int m1, n1, m2, n2;
    printf("Enter rows and columns of Matrix A: ");
    scanf("%d %d", &m1, &n1);
    printf("Enter rows and columns of Matrix B: ");
    scanf("%d %d", &m2, &n2);

    double **A = malloc(m1 * sizeof(double *));
    double **B = malloc(m2 * sizeof(double *));
    for (int i = 0; i < m1; i++) A[i] = malloc(n1 * sizeof(double));
    for (int i = 0; i < m2; i++) B[i] = malloc(n2 * sizeof(double));

    for (int i = 0; i < m1; i++)
        for (int j = 0; j < n1; j++)
            A[i][j] = rand() % 10;

    for (int i = 0; i < m2; i++)
        for (int j = 0; j < n2; j++)
            B[i][j] = rand() % 10;

    struct timeval start, end;
    gettimeofday(&start, NULL);

    if (m1 == m2 && n1 == n2) {
        double **add = malloc(m1 * sizeof(double *));
        for (int i = 0; i < m1; i++) add[i] = malloc(n1 * sizeof(double));
        for (int i = 0; i < m1; i++)
            for (int j = 0; j < n1; j++)
                add[i][j] = A[i][j] + B[i][j];
        for (int i = 0; i < m1; i++) free(add[i]);
        free(add);
    }

    if (m1 == m2 && n1 == n2) {
        double **sub = malloc(m1 * sizeof(double *));
        for (int i = 0; i < m1; i++) sub[i] = malloc(n1 * sizeof(double));
        for (int i = 0; i < m1; i++)
            for (int j = 0; j < n1; j++)
                sub[i][j] = A[i][j] - B[i][j];
        for (int i = 0; i < m1; i++) free(sub[i]);
        free(sub);
    }

    if (n1 == m2) {
        double **mul = malloc(m1 * sizeof(double *));
        for (int i = 0; i < m1; i++) mul[i] = malloc(n2 * sizeof(double));
        for (int i = 0; i < m1; i++)
            for (int j = 0; j < n2; j++) {
                mul[i][j] = 0;
                for (int k = 0; k < n1; k++)
                    mul[i][j] += A[i][k] * B[k][j];
            }
        for (int i = 0; i < m1; i++) free(mul[i]);
        free(mul);
    }

    if (m1 == n1) {
        double **copyA = malloc(m1 * sizeof(double *));
        for (int i = 0; i < m1; i++) {
            copyA[i] = malloc(n1 * sizeof(double));
            for (int j = 0; j < n1; j++) copyA[i][j] = A[i][j];
        }
        determinant(m1, copyA);
        for (int i = 0; i < m1; i++) free(copyA[i]);
        free(copyA);
    }

    if (m2 == n2) {
        double **copyB = malloc(m2 * sizeof(double *));
        for (int i = 0; i < m2; i++) {
            copyB[i] = malloc(n2 * sizeof(double));
            for (int j = 0; j < n2; j++) copyB[i][j] = B[i][j];
        }
        determinant(m2, copyB);
        for (int i = 0; i < m2; i++) free(copyB[i]);
        free(copyB);
    }

    gettimeofday(&end, NULL);
    double serial_time = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_usec - start.tv_usec) / 1000.0;
    printf("Serial execution time: %.3lf ms\n", serial_time);

    for (int i = 0; i < m1; i++) free(A[i]);
    for (int i = 0; i < m2; i++) free(B[i]);
    free(A);
    free(B);

    return 0;
}

[23bcs074@mepcolinux ex2]$cat s2.c
[23bcs074@mepcolinux ex2]$./a.out
Enter rows and columns of Matrix A: 5 5
Enter rows and columns of Matrix B: 5 5
Serial execution time: 0.014 ms
[23bcs074@mepcolinux ex2]$./a.out
Enter rows and columns of Matrix A: 50 50
Enter rows and columns of Matrix B: 50 50
Serial execution time: 2.290 ms
[23bcs074@mepcolinux ex2]$./a.out
Enter rows and columns of Matrix A: 500 500
Enter rows and columns of Matrix B: 500 500
Serial execution time: 3836.983 ms
[23bcs074@mepcolinux ex2]$./a.out
[23bcs074@mepcolinux ex2]$./a.out
Enter rows and columns of Matrix A: 2 2
Enter rows and columns of Matrix B: 2 2
Enter elements of Matrix A:
1 2 3 4
Enter elements of Matrix B:
5 6 7 8

[Process 1] Addition:
6.00 8.00
10.00 12.00

[Process 2] Subtraction:
-4.00 -4.00
-4.00 -4.00

[Process 3] Multiplication:
19.00 22.00
43.00 50.00

[Process 4] Determinant of A = -2.00
[Process 4] Determinant of B = -2.00

Child execution times (ms):
Process 1: 8.114 ms
Process 2: 7.788 ms
Process 3: 0.179 ms
Process 4: 2.996 ms

Parallel execution time: 8.114 ms
[23bcs074@mepcolinux ex2]$cc -std=c99 p1.c
[23bcs074@mepcolinux ex2]$./a.out
Enter rows and columns of Matrix A: 5 5
Enter rows and columns of Matrix B: 5 5
Process 1 execution time: 0.000 ms
Process 2 execution time: 0.000 ms
Process 3 execution time: 0.003 ms
Process 4 execution time: 0.023 ms
Parallel execution time: 0.023 ms
[23bcs074@mepcolinux ex2]$./a.out
Enter rows and columns of Matrix A: 50 50
Enter rows and columns of Matrix B: 50 50
Process 1 execution time: 0.008 ms
Process 2 execution time: 0.009 ms
Process 3 execution time: 1.300 ms
Process 4 execution time: 1.240 ms
Parallel execution time: 1.300 ms
[23bcs074@mepcolinux ex2]$./a.out
Enter rows and columns of Matrix A: 500 500
Enter rows and columns of Matrix B: 500 500
Process 1 execution time: 0.846 ms
Process 2 execution time: 0.846 ms
Process 3 execution time: 2116.511 ms
Process 4 execution time: 1051.954 ms
Parallel execution time: 2116.511 ms
[23bcs074@mepcolinux ex2]$exit

