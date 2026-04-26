[23bcs074@mepcolinux ex6]$cat p1.c
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<time.h>

int thread_count;

void *Hello(void *rank)
{
   long my_rank=(long)rank;

   struct timespec start,end;
   clock_gettime(CLOCK_MONOTONIC,&start);

   printf("Hello from thread %ld of %d\n",my_rank,thread_count);

   clock_gettime(CLOCK_MONOTONIC,&end);

   double time_taken = (end.tv_sec - start.tv_sec) +
                       (end.tv_nsec - start.tv_nsec)/1e9;

   printf("Thread %ld Execution Time = %f seconds\n",my_rank,time_taken);

   return NULL;
}

int main(int argc,char *argv[])
{
   long thread;
   pthread_t *thread_handles;

   struct timespec start_main,end_main;
   clock_gettime(CLOCK_MONOTONIC,&start_main);

   thread_count=strtol(argv[1],NULL,10);
   thread_handles=malloc(thread_count*sizeof(pthread_t));

   for(thread=0;thread<thread_count;thread++)
   {
      pthread_create(&thread_handles[thread],NULL,Hello,(void *)thread);
   }

   printf("Hello from the main thread\n");

   for(thread=0;thread<thread_count;thread++)
   {
      pthread_join(thread_handles[thread],NULL);
   }

   clock_gettime(CLOCK_MONOTONIC,&end_main);

   double total_time = (end_main.tv_sec - start_main.tv_sec) +
                       (end_main.tv_nsec - start_main.tv_nsec)/1e9;

   printf("Main Thread Execution Time = %f seconds\n",total_time);

   free(thread_handles);
   return 0;
}
[23bcs074@mepcolinux ex6]$./p1
Segmentation fault (core dumped)
[23bcs074@mepcolinux ex6]$./p1 4
Hello from thread 0 of 4
Thread 0 Execution Time = 0.000177 seconds
Hello from the main thread
Hello from thread 2 of 4
Thread 2 Execution Time = 0.000302 seconds
Hello from thread 1 of 4
Thread 1 Execution Time = 0.000437 seconds
Hello from thread 3 of 4
Thread 3 Execution Time = 0.000482 seconds
Main Thread Execution Time = 0.001010 seconds
[23bcs074@mepcolinux ex6]$cat p2.c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX 1000

int arr[MAX];
int n,i, thread_count;
long global_sum = 0;
long thread;
pthread_mutex_t mutex;

void *partial_sum(void *rank)
{
    long my_rank = (long) rank;

    struct timespec start,end;
    clock_gettime(CLOCK_MONOTONIC,&start);

    int local_n = n / thread_count;
    int first = my_rank * local_n;
    int last = (my_rank == thread_count-1) ? n-1 : first + local_n - 1;

    long local_sum = 0;

    printf("Thread %ld: First index=%d, Last index=%d\n",my_rank,first,last);

    for(i = first; i <= last; i++)
        local_sum += arr[i];

    pthread_mutex_lock(&mutex);
    global_sum += local_sum;
    pthread_mutex_unlock(&mutex);

    clock_gettime(CLOCK_MONOTONIC,&end);

    double time_taken = (end.tv_sec - start.tv_sec) +
                        (end.tv_nsec - start.tv_nsec)/1e9;

    printf("Thread %ld Execution Time = %f seconds\n",my_rank,time_taken);

    pthread_exit(NULL);
}

int main()
{
    pthread_t *thread_handles;

    pthread_mutex_init(&mutex,NULL);

    printf("Enter number of elements: ");
    scanf("%d", &n);

    printf("Enter the elements:\n");
    for(i = 0; i < n; i++)
        scanf("%d", &arr[i]);

    printf("Enter number of threads: ");
    scanf("%d", &thread_count);

    thread_handles = malloc(thread_count * sizeof(pthread_t));

    struct timespec start_total,end_total;
    clock_gettime(CLOCK_MONOTONIC,&start_total);

    for(thread = 0; thread < thread_count; thread++)
        pthread_create(&thread_handles[thread],NULL,partial_sum,(void*) thread);

    for(thread = 0; thread < thread_count; thread++)
        pthread_join(thread_handles[thread], NULL);

    clock_gettime(CLOCK_MONOTONIC,&end_total);

    double total_time = (end_total.tv_sec - start_total.tv_sec) +
                        (end_total.tv_nsec - start_total.tv_nsec)/1e9;

    printf("\nTotal Sum = %ld\n", global_sum);
    printf("Total Execution Time = %f seconds\n",total_time);

    pthread_mutex_destroy(&mutex);
    free(thread_handles);

    return 0;
}
[23bcs074@mepcolinux ex6]$./p2
Enter number of elements: 10
Enter the elements:
1 1 2 3 4 5 6  7 7 8 9
Enter number of threads: 3
Thread 0: First index=0, Last index=2
Thread 1: First index=3, Last index=5
Thread 2: First index=6, Last index=9
Thread 2 Execution Time = 0.000035 seconds
Thread 0 Execution Time = 0.000088 seconds
Thread 1 Execution Time = 0.000069 seconds

Total Sum = 46
Total Execution Time = 0.000721 seconds
[23bcs074@mepcolinux ex6]$cat p3.c
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<time.h>

#define MAX 10

int A[MAX][MAX],B[MAX][MAX],add[MAX][MAX],mul[MAX][MAX];
int r1,r2,c1,c2;

void *mat_add(void *arg)
{
   struct timespec start,end;
   clock_gettime(CLOCK_MONOTONIC,&start);

   int i,j;
   for(i=0;i<r1;i++)
      for(j=0;j<c1;j++)
         add[i][j]=A[i][j]+B[i][j];

   clock_gettime(CLOCK_MONOTONIC,&end);

   double time_taken = (end.tv_sec - start.tv_sec) +
                       (end.tv_nsec - start.tv_nsec)/1e9;

   printf("Addition completed by thread\n");
   printf("Addition Thread Execution Time = %f seconds\n",time_taken);

   pthread_exit(NULL);
}

void *mat_mul(void *arg)
{
   struct timespec start,end;
   clock_gettime(CLOCK_MONOTONIC,&start);

   int i,j,k;
   for(i=0;i<r2;i++)
      for(j=0;j<c2;j++)
      {
         mul[i][j]=0;
         for(k=0;k<c1;k++)
            mul[i][j]+=A[i][k]*B[k][j];
      }

   clock_gettime(CLOCK_MONOTONIC,&end);

   double time_taken = (end.tv_sec - start.tv_sec) +
                       (end.tv_nsec - start.tv_nsec)/1e9;

   printf("Multiplication completed by thread\n");
   printf("Multiplication Thread Execution Time = %f seconds\n",time_taken);

   pthread_exit(NULL);
}

int main()
{
   pthread_t t1,t2;
   int i,j;

   printf("Enter the no of rows and columns of Matrix A:");
   scanf("%d%d",&r1,&c1);

   printf("Enter the no of rows and columns of Matrix B:");
   scanf("%d%d",&r2,&c2);

   if(c1!=r2)
   {
      printf("Matrix multiplication not possible");
      return 0;
   }

   if(r1!=r2 || c1!=c2)
   {
      printf("Matrix addition not possible");
      return 0;
   }

   printf("Enter elements of matrixA:");
   for(i=0;i<r1;i++)
      for(j=0;j<c1;j++)
         scanf("%d",&A[i][j]);

   printf("Enter elements of matrixB:");
   for(i=0;i<r2;i++)
      for(j=0;j<c2;j++)
         scanf("%d",&B[i][j]);

   struct timespec start_total,end_total;
   clock_gettime(CLOCK_MONOTONIC,&start_total);

   pthread_create(&t1,NULL,mat_add,NULL);
   pthread_create(&t2,NULL,mat_mul,NULL);

   pthread_join(t1,NULL);
   pthread_join(t2,NULL);

   clock_gettime(CLOCK_MONOTONIC,&end_total);

   double total_time = (end_total.tv_sec - start_total.tv_sec) +
                       (end_total.tv_nsec - start_total.tv_nsec)/1e9;

   printf("Matrix addition result:\n");
   for(i=0;i<r1;i++)
   {
      for(j=0;j<c1;j++)
         printf(" %d ",add[i][j]);
      printf("\n");
   }

   printf("Matrix multiplication result:\n");
   for(i=0;i<r2;i++)
   {
      for(j=0;j<c2;j++)
         printf(" %d ",mul[i][j]);
      printf("\n");
   }

   printf("Total Execution Time = %f seconds\n",total_time);

   return 0;
}
[23bcs074@mepcolinux ex6]$./p3
Enter the no of rows and columns of Matrix A:5 5
Enter the no of rows and columns of Matrix B:5 5
Enter elements of matrixA:
1 2 3 4 5
2 3 4 5 6
4 4 7 6 1
1 4 7 8 9
3 1 5 6 7
Enter elements of matrixB:
3 4 1 2 3
6 6 4 2 5
1 5 4 1 5
3 3 4 4 5
4 2 7 6 3
Addition completed by thread
Addition Thread Execution Time = 0.000001 seconds
Multiplication completed by thread
Multiplication Thread Execution Time = 0.000004 seconds
Matrix addition result:
 4  6  4  6  8
 8  9  8  7  11
 5  9  11  7  6
 4  7  11  12  14
 7  3  12  12  10
Matrix multiplication result:
 50  53  72  55  63
 67  73  92  70  84
 65  95  79  53  100
 94  105  140  103  125
 66  75  100  79  90
Total Execution Time = 0.001132 seconds
[23bcs074@mepcolinux ex6]$exit
exit
