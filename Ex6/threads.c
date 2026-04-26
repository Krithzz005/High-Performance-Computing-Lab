[23bcs074@mepcolinux ex7]$cat calc.c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define BUFFER_SIZE 5
#define TOTAL_TASKS 20

typedef struct {
    int a;
    int b;
    char op;
} Task;

Task buffer[BUFFER_SIZE];

int in = 0;
int out = 0;
int produced = 0;
int consumed = 0;

sem_t empty, full;
pthread_mutex_t mutex;

FILE *fp;

/* Function to compute operations */
void compute(Task t)
{
    int result;
    pthread_t thread_id = pthread_self();
    unsigned long thread_num = (unsigned long)thread_id;
    switch(t.op)
    {
        case '+':
            result = t.a + t.b;
            printf("[ADD THREAD:%lu] %d + %d = %d\n",thread_num, t.a, t.b, result);
            fprintf(fp,"[ADD THREAD:%lu] %d + %d = %d\n",thread_num,t.a,t.b,result);
            break;

        case '-':
            result = t.a - t.b;
            printf("[SUB THREAD:%lu] %d - %d = %d\n",thread_num, t.a, t.b, result);
            fprintf(fp,"[SUB THREAD:%lu] %d - %d = %d\n",thread_num,t.a,t.b,result);
            break;

        case '*':
            result = t.a * t.b;
            printf("[MUL THREAD:%lu] %d * %d = %d\n",thread_num, t.a, t.b, result);
            fprintf(fp,"[MUL THREAD:%lu] %d * %d = %d\n",thread_num,t.a,t.b,result);
            break;

        case '/':
            if(t.b == 0)
            {
                printf("[DIV THREAD] ERROR: Division by zero (%d / %d)\n",t.a,t.b);
                return;
            }
            result = t.a / t.b;
            printf("[DIV THREAD:%lu] %d / %d = %d\n",thread_num, t.a, t.b, result);
            fprintf(fp,"[DIV THREAD:%lu] %d / %d = %d\n",thread_num,t.a,t.b,result);
            break;
    }

    fflush(fp);
}

/* Producer thread */
void *producer(void *arg)
{
    char ops[] = {'+','-','*','/'};

    while(produced < TOTAL_TASKS)
    {
        Task t;

        t.a = rand()%20 + 1;
        t.b = rand()%10;
        t.op = ops[rand()%4];

        sem_wait(&empty);
        pthread_mutex_lock(&mutex);

        buffer[in] = t;
        in = (in + 1) % BUFFER_SIZE;
        produced++;

        printf("\n[PRODUCER] Generated -> %d %c %d\n",t.a,t.op,t.b);
        printf("[BUFFER] Task inserted\n");

        pthread_mutex_unlock(&mutex);
        sem_post(&full);

        sleep(1);
    }

    pthread_exit(NULL);
}

/* Consumer thread */
void *consumer(void *arg)
{
    while(1)
    {
        sem_wait(&full);
        pthread_mutex_lock(&mutex);

        if(consumed >= TOTAL_TASKS)
        {
            pthread_mutex_unlock(&mutex);
            sem_post(&full);
            break;
        }

        Task t = buffer[out];
        out = (out + 1) % BUFFER_SIZE;
        consumed++;

        printf("[CONSUMER] Task taken -> %d %c %d\n",t.a,t.op,t.b);

        pthread_mutex_unlock(&mutex);
        sem_post(&empty);

        compute(t);
    }

    pthread_exit(NULL);
}

int main()
{
    srand(time(NULL));

    pthread_t prod;
    pthread_t c1,c2,c3,c4;

    fp = fopen("result.txt","w");

    sem_init(&empty,0,BUFFER_SIZE);
    sem_init(&full,0,0);
    pthread_mutex_init(&mutex,NULL);

    pthread_create(&prod,NULL,producer,NULL);

    pthread_create(&c1,NULL,consumer,NULL);
    pthread_create(&c2,NULL,consumer,NULL);
    pthread_create(&c3,NULL,consumer,NULL);
    pthread_create(&c4,NULL,consumer,NULL);

    pthread_join(prod,NULL);
    pthread_join(c1,NULL);
    pthread_join(c2,NULL);
    pthread_join(c3,NULL);
    pthread_join(c4,NULL);

    fclose(fp);

    printf("\nExecution finished. Check result.txt\n");

    return 0;
}
[23bcs074@mepcolinux ex7]$./calc

[PRODUCER] Generated -> 13 / 9
[BUFFER] Task inserted
[CONSUMER] Task taken -> 13 / 9
[DIV THREAD:140602000873216] 13 / 9 = 1

[PRODUCER] Generated -> 4 / 4
[BUFFER] Task inserted
[CONSUMER] Task taken -> 4 / 4
[DIV THREAD:140601992480512] 4 / 4 = 1

[PRODUCER] Generated -> 6 / 7
[BUFFER] Task inserted
[CONSUMER] Task taken -> 6 / 7
[DIV THREAD:140601984087808] 6 / 7 = 0

[PRODUCER] Generated -> 2 * 9
[BUFFER] Task inserted
[CONSUMER] Task taken -> 2 * 9
[MUL THREAD:140601975695104] 2 * 9 = 18

[PRODUCER] Generated -> 12 * 6
[BUFFER] Task inserted
[CONSUMER] Task taken -> 12 * 6
[MUL THREAD:140602000873216] 12 * 6 = 72

[PRODUCER] Generated -> 2 - 9
[BUFFER] Task inserted
[CONSUMER] Task taken -> 2 - 9
[SUB THREAD:140601992480512] 2 - 9 = -7

[PRODUCER] Generated -> 2 - 3
[BUFFER] Task inserted
[CONSUMER] Task taken -> 2 - 3
[SUB THREAD:140601984087808] 2 - 3 = -1

[PRODUCER] Generated -> 11 + 2
[BUFFER] Task inserted
[CONSUMER] Task taken -> 11 + 2
[ADD THREAD:140601975695104] 11 + 2 = 13

[PRODUCER] Generated -> 7 - 0
[BUFFER] Task inserted
[CONSUMER] Task taken -> 7 - 0
[SUB THREAD:140602000873216] 7 - 0 = 7

[PRODUCER] Generated -> 6 - 9
[BUFFER] Task inserted
[CONSUMER] Task taken -> 6 - 9
[SUB THREAD:140601992480512] 6 - 9 = -3

[PRODUCER] Generated -> 15 * 1
[BUFFER] Task inserted
[CONSUMER] Task taken -> 15 * 1
[MUL THREAD:140601984087808] 15 * 1 = 15

[PRODUCER] Generated -> 2 * 7
[BUFFER] Task inserted
[CONSUMER] Task taken -> 2 * 7
[MUL THREAD:140601975695104] 2 * 7 = 14

[PRODUCER] Generated -> 9 - 2
[BUFFER] Task inserted
[CONSUMER] Task taken -> 9 - 2
[SUB THREAD:140602000873216] 9 - 2 = 7

[PRODUCER] Generated -> 12 - 3
[BUFFER] Task inserted
[CONSUMER] Task taken -> 12 - 3
[SUB THREAD:140601992480512] 12 - 3 = 9

[PRODUCER] Generated -> 15 / 6
[BUFFER] Task inserted
[CONSUMER] Task taken -> 15 / 6
[DIV THREAD:140601984087808] 15 / 6 = 2

[PRODUCER] Generated -> 17 * 7
[BUFFER] Task inserted
[CONSUMER] Task taken -> 17 * 7
[MUL THREAD:140601975695104] 17 * 7 = 119

[PRODUCER] Generated -> 10 + 8
[BUFFER] Task inserted
[CONSUMER] Task taken -> 10 + 8
[ADD THREAD:140602000873216] 10 + 8 = 18

[PRODUCER] Generated -> 7 * 8
[BUFFER] Task inserted
[CONSUMER] Task taken -> 7 * 8
[MUL THREAD:140601992480512] 7 * 8 = 56

[PRODUCER] Generated -> 11 + 4
[BUFFER] Task inserted
[CONSUMER] Task taken -> 11 + 4
[ADD THREAD:140601984087808] 11 + 4 = 15

[PRODUCER] Generated -> 4 - 1
[BUFFER] Task inserted
[CONSUMER] Task taken -> 4 - 1
[SUB THREAD:140601975695104] 4 - 1 = 3
^C
[23bcs074@mepcolinux ex7]$cat result.txt
[DIV THREAD:140602000873216] 13 / 9 = 1
[DIV THREAD:140601992480512] 4 / 4 = 1
[DIV THREAD:140601984087808] 6 / 7 = 0
[MUL THREAD:140601975695104] 2 * 9 = 18
[MUL THREAD:140602000873216] 12 * 6 = 72
[SUB THREAD:140601992480512] 2 - 9 = -7
[SUB THREAD:140601984087808] 2 - 3 = -1
[ADD THREAD:140601975695104] 11 + 2 = 13
[SUB THREAD:140602000873216] 7 - 0 = 7
[SUB THREAD:140601992480512] 6 - 9 = -3
[MUL THREAD:140601984087808] 15 * 1 = 15
[MUL THREAD:140601975695104] 2 * 7 = 14
[SUB THREAD:140602000873216] 9 - 2 = 7
[SUB THREAD:140601992480512] 12 - 3 = 9
[DIV THREAD:140601984087808] 15 / 6 = 2
[MUL THREAD:140601975695104] 17 * 7 = 119
[ADD THREAD:140602000873216] 10 + 8 = 18
[MUL THREAD:140601992480512] 7 * 8 = 56
[ADD THREAD:140601984087808] 11 + 4 = 15
[SUB THREAD:140601975695104] 4 - 1 = 3
[23bcs074@mepcolinux ex7]$exit
exit
