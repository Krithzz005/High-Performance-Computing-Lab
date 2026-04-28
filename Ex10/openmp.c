[23bcs074@mepcolinux ex11]$cat token.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#define QUEUE_SIZE 20
#define MAX_LINE 1024
#define NUM_PRODUCERS 1
#define NUM_CONSUMERS 2

char queue[QUEUE_SIZE][MAX_LINE];
int head = 0, tail = 0, count = 0;
int done_producing = 0;
omp_lock_t queue_lock;

void enqueue(char *line) {
    int success = 0;
    while (!success) {
        omp_set_lock(&queue_lock);
        if (count < QUEUE_SIZE) {
            strncpy(queue[tail], line, MAX_LINE);
            tail = (tail + 1) % QUEUE_SIZE;
            count++;
            success = 1;
        }
        omp_unset_lock(&queue_lock);
    }
}

int dequeue(char *line) {
    int success = 0;
    omp_set_lock(&queue_lock);
    if (count > 0) {
        strncpy(line, queue[head], MAX_LINE);
        head = (head + 1) % QUEUE_SIZE;
        count--;
        success = 1;
    }
    omp_unset_lock(&queue_lock);
    return success;
}

int main() {
    omp_init_lock(&queue_lock);
    #pragma omp parallel num_threads(NUM_PRODUCERS + NUM_CONSUMERS)
    {
        int tid = omp_get_thread_num();
        if (tid < NUM_PRODUCERS) {
            // PRODUCER LOGIC
            char filename[20];
            sprintf(filename, "input.txt");
            FILE *file = fopen(filename, "r");
            if (file) {
                char line[MAX_LINE];
                while (fgets(line, MAX_LINE, file)) {
                    enqueue(line);
                }
                fclose(file);
            } else {
                printf("Producer %d: Error opening %s\n", tid, filename);
            }
            #pragma omp atomic
            done_producing++;
        }
        else {
            // CONSUMER LOGIC
            char line[MAX_LINE];
            char *saveptr;

            while (1) {
                if (dequeue(line)) {
                    char *token = strtok_r(line, " \t\n\r", &saveptr);
                    while (token != NULL) {
                        #pragma omp critical
                        printf("Thread %d token: %s\n", tid, token);
                        token = strtok_r(NULL, " \t\n\r", &saveptr);
                    }
                } else {
                   int producers_finished;
                    #pragma omp atomic read
                    producers_finished = done_producing;

                    if (producers_finished == NUM_PRODUCERS) break;
                }
            }
        }
    }
    omp_destroy_lock(&queue_lock);
    return 0;
}

[23bcs074@mepcolinux ex11]$cat input.txt
Hello world from OpenMP
This is producer one
Parallel programming is powerful
Consumers will tokenize this text
[23bcs074@mepcolinux ex11]$./token
Thread 1 token: Hello
Thread 1 token: world
Thread 1 token: from
Thread 1 token: OpenMP
Thread 1 token: Parallel
Thread 1 token: programming
Thread 1 token: is
Thread 1 token: powerful
Thread 1 token: Consumers
Thread 1 token: will
Thread 1 token: tokenize
Thread 1 token: this
Thread 1 token: text
Thread 2 token: This
Thread 2 token: is
Thread 2 token: producer
Thread 2 token: one
[23bcs074@mepcolinux ex11]$exit
exit
