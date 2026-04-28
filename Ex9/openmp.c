[23bcs074@mepcolinux ex10]$cat trap.c
#include <stdio.h>
#include <omp.h>


double f(double x) {
    return x * x;
}

int main() {
    int n, i;
    double a, b, h, sum = 0.0, x;

    printf("Enter lower limit (a): ");
    scanf("%lf", &a);

    printf("Enter upper limit (b): ");
    scanf("%lf", &b);

    printf("Enter number of trapezoids (n): ");
    scanf("%d", &n);

    h = (b - a) / n;

    sum = (f(a) + f(b)) / 2.0;

    #pragma omp parallel for private(x) reduction(+:sum)
    for (i = 1; i < n; i++) {
        x = a + i * h;
        sum += f(x);
    }

    double result = h * sum;

    printf("\n[Using Reduction] Integral = %lf\n", result);


    sum = (f(a) + f(b)) / 2.0;

    #pragma omp parallel for private(x)
    for (i = 1; i < n; i++) {
        x = a + i * h;

        #pragma omp critical
        {
            sum += f(x);
        }
    }

    result = h * sum;

    printf("[Using Critical] Integral = %lf\n", result);

    return 0;
}

[23bcs074@mepcolinux ex10]$./t
tar   trap
[23bcs074@mepcolinux ex10]$./t
bash: ./t: No such file or directory
[23bcs074@mepcolinux ex10]$./trap
Enter lower limit (a): 0
Enter upper limit (b): 5
Enter number of trapezoids (n): 100

[Using Reduction] Integral = 41.668750
[Using Critical] Integral = 41.668750
[23bcs074@mepcolinux ex10]$cat large.c
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <limits.h>

int main() {
    int n;
    printf("Enter the number of elements: ");
    scanf("%d", &n);
    int *array = (int *)malloc(n * sizeof(int));
    printf("Enter %d elements:\n", n);
    for (int i = 0; i < n; i++) {
        scanf("%d", &array[i]);
    }
    int max_value = array[0];
    #pragma omp parallel for reduction(max:max_value)
    for (int i = 0; i < n; i++) {
        if (array[i] > max_value) max_value = array[i];
    }
    printf("The largest number in the array is: %d\n", max_value);
    free(array);
    return 0;
}

[23bcs074@mepcolinux ex10]$./l
Enter the number of elements: 10
Enter 10 elements:
32 21 56 43 88 43 22 91 31 11 2 10
The largest number in the array is: 91
[23bcs074@mepcolinux ex10]$cat target.c
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main()
{
    int n;
    printf("Enter the number of elements in the array: ");
    scanf("%d", &n);
    int* array = (int*)malloc(n * sizeof(int));
    printf("Enter %d elements:\n", n);
    for (int i = 0; i < n; i++)
    {
        scanf("%d", &array[i]);
    }
    int target;
    printf("Enter the target value: ");
    scanf("%d", &target);
    int local_count = 0;
    #pragma omp parallel for
    //int local_count = 0;
        for (int i = 0; i < n; i++)
        {
            if (array[i] < target)
            {
            #pragma omp critical
            {
                local_count++;
            }
            }
        }
        //count += local_count;

    printf("The count of elements less than %d is: %d\n", target, local_count);
    free(array);

    return 0;

}

[23bcs074@mepcolinux ex10]$./tar
Enter the number of elements in the array: 10
Enter 10 elements:
1 3 5 4 2 6 7 4 3 1
Enter the target value: 5
The count of elements less than 5 is: 7
[23bcs074@mepcolinux ex10]$exit
exit
