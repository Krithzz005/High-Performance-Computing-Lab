[23bcs074@mepcolinux ex9]$cat add.c
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

int main() {
    int n, i;

    printf("Enter size of arrays: ");
    scanf("%d", &n);

    int *A = (int*) malloc(n * sizeof(int));
    int *B = (int*) malloc(n * sizeof(int));
    int *C = (int*) malloc(n * sizeof(int));

    // Seed for random numbers
    srand(time(NULL));

    // Initialize arrays with random values (0–99)
    for (i = 0; i < n; i++) {
        A[i] = rand() % 100;
        B[i] = rand() % 100;
    }

    // Parallel addition
    #pragma omp parallel for
    for (i = 0; i < n; i++) {
        C[i] = A[i] + B[i];
    }

    // Print only if size < 10
    if (n > 10) {
        printf("\nArray A:\n");
        for (i = 0; i < n; i++) {
            printf("%d ", A[i]);
        }

        printf("\nArray B:\n");
        for (i = 0; i < n; i++) {
            printf("%d ", B[i]);
        }

        printf("\nResult Array C (A + B):\n");
        for (i = 0; i < n; i++) {
            printf("%d ", C[i]);
        }
        printf("\n");
    } else {
        printf("\nArrays are small. Result not displayed.\n");
    }

    // Free memory
    free(A);
    free(B);
    free(C);

    return 0;


}
[23bcs074@mepcolinux ex9]$./add
Enter size of arrays: 15

Array A:
36 4 82 7 9 18 35 86 86 76 4 30 63 57 14
Array B:
4 32 53 31 51 37 6 98 73 32 65 49 62 95 69
Result Array C (A + B):
40 36 135 38 60 55 41 184 159 108 69 79 125 152 83
[23bcs074@mepcolinux ex9]$cat const.c
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

int main() {
    int n, i, constant;

    printf("Enter size of array: ");
    scanf("%d", &n);

    printf("Enter constant value: ");
    scanf("%d", &constant);

    int *A = (int*) malloc(n * sizeof(int));

    // Seed random numbers
    srand(time(NULL));

    // Initialize array
    for (i = 0; i < n; i++) {
        A[i] = rand() % 100;
        printf("%d\t",A[i]);
    }

    // Print initial array if n < 10
    if (n < 10) {
        printf("\nInitial Array:\n");
        for (i = 0; i < n; i++) {
            printf("%d ", A[i]);
        }
        printf("\n");
    }

    // Parallel addition
    #pragma omp parallel for
    for (i = 0; i < n; i++) {
        A[i] = A[i] + constant;
    }

    // Print updated array if n < 10
    if (n > 10) {
        printf("\nUpdated Array:\n");
        for (i = 0; i < n; i++) {
            printf("%d ", A[i]);
        }
        printf("\n");
    } else {
        printf("\nArray is small. Output not displayed.\n");
    }

    free(A);
    return 0;
}
[23bcs074@mepcolinux ex9]$./c
Enter size of array: 15
Enter constant value: 5
33      90      85      23      59      55      77      66      17      92      25      9       65      84      38
Updated Array:
38 95 90 28 64 60 82 71 22 97 30 14 70 89 43
[23bcs074@mepcolinux ex9]$cat h.c
#include <stdio.h>
#include <omp.h>

int main() {

    int r, c, i, j;

    printf("Enter number of rows: ");
    scanf("%d", &r);

    printf("Enter number of columns: ");
    scanf("%d", &c);

    int A[r][c], B[r][c], C[r][c];

    // Input matrix
    printf("Enter matrix elements:\n");
    for (i = 0; i < r; i++) {
        for (j = 0; j < c; j++) {
            scanf("%d", &A[i][j]);
        }
    }

    // Parallel horizontal mirror (store in B)
    #pragma omp parallel for private(j)
    for (i = 0; i < r; i++) {
        for (j = 0; j < c; j++) {
            B[i][j] = A[i][c - j - 1];
        }
    }

    // Copy B into C
    for (i = 0; i < r; i++) {
        for (j = 0; j < c; j++) {
            C[i][j] = B[i][j];
        }
    }

    // Print original matrix
    printf("\nOriginal Matrix:\n");
    for (i = 0; i < r; i++) {
        for (j = 0; j < c; j++) {
            printf("%d ", A[i][j]);
        }
        printf("\n");
    }

    // Print mirrored matrix from C
    printf("\nHorizontally Mirrored Matrix:\n");
    for (i = 0; i < r; i++) {
        for (j = 0; j < c; j++) {
            printf("%d ", C[i][j]);
        }
        printf("\n");
    }

    return 0;
}
[23bcs074@mepcolinux ex9]$./h
Enter number of rows: 3
Enter number of columns: 3
Enter matrix elements:
1 2 3
4 5 6
7 8 9

Original Matrix:
1 2 3
4 5 6
7 8 9

Horizontally Mirrored Matrix:
3 2 1
6 5 4
9 8 7
[23bcs074@mepcolinux ex9]$cat sliding.c
#include <stdio.h>
#include <omp.h>

int main() {
    int n, w, i, j;

    printf("Enter size of array (n): ");
    scanf("%d", &n);

    int A[n];

    printf("Enter array elements:\n");
    for (i = 0; i < n; i++) {
        scanf("%d", &A[i]);
    }

    printf("Enter window size: ");
    scanf("%d", &w);

    int result_size = n - w + 1;

    // Validate window size
    if (w > n || w <= 0) {
        printf("Invalid window size!\n");
        return 0;
    }

    int sum[result_size];

    // Parallel sliding window computation
    #pragma omp parallel for private(j)
    for (i = 0; i < result_size; i++) {
        sum[i] = 0;
        for (j = 0; j < w; j++) {
            sum[i] += A[i + j];
        }
    }

    // Print result
    printf("\nSliding Window Sums:\n");
    for (i = 0; i < result_size; i++) {
        printf("%d ", sum[i]);
    }
    printf("\n");

    return 0;
}
[23bcs074@mepcolinux ex9]$./sliding
Enter size of array (n): 7
Enter array elements:
1 2 3 4 5 6 7
Enter window size: 2

Sliding Window Sums:
3 5 7 9 11 13
[23bcs074@mepcolinux ex9]$exit
exit
