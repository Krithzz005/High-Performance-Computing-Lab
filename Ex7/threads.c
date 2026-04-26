[23bcs074@mepcolinux ex8]$cat spell.c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include<unistd.h>

#define BUFFER_SIZE 10
#define MAX_WORD 50
#define NUM_THREADS 10

/* Word buffer */
char word_buffer[BUFFER_SIZE][MAX_WORD];
int in = 0;
int out = 0;
int count = 0;

/* Result buffer */
char result_buffer[BUFFER_SIZE][MAX_WORD];
int result_index = 0;

/* Dictionary */
char *dictionary[] = {
"ability","able","about","above","accept","access","accident","account","across","action",
"active","activity","actor","actual","add","address","adult","advance","advice","affect",
"after","again","against","age","agency","agent","agree","agreement","ahead","air",
"all","allow","almost","alone","along","already","also","although","always","among",
"amount","analysis","animal","another","answer","any","anyone","anything","appear","apply",
"approach","area","argue","arm","around","arrive","art","article","artist","ask",
"attack","attention","attorney","audience","author","authority","available","avoid","away","baby",
"back","bad","bag","ball","bank","bar","base","beat","beautiful","because",
"become","bed","before","begin","behavior","behind","believe","benefit","best","better",
"between","beyond","big","bill","billion","bit","black","blood","blue","board",
"body","book","born","both","box","boy","break","bring","brother","build",
"building","business","buy","call","camera","campaign","can","cancer","candidate","capital",
"car","card","care","career","carry","case","catch","cause","cell","center",
"central","century","certain","certainly","chair","challenge","chance","change","character","charge",
"check","child","choice","choose","church","citizen","city","civil","claim","class",
"clear","clearly","close","coach","cold","collection","college","color","come","commercial",
"common","community","company","compare","computer","concern","condition","conference","consider","consumer",
"contain","continue","control","cost","could","country","couple","course","court","cover",
"create","crime","culture","cup","current","customer","cut","dark","data","daughter",
"day","dead","deal","death","debate","decade","decide","decision","linux","program"};

int dict_size = 200;
/* Synchronization */
pthread_mutex_t lock;
pthread_cond_t not_empty;
pthread_cond_t not_full;

/* Function to check dictionary */
int check_dictionary(char *word) {
    for (int i = 0; i < dict_size; i++) {
        if (strcmp(word, dictionary[i]) == 0)
            return 1;
    }
    return 0;
}

/* Spell check thread */
void *spell_check(void *arg) {
    char word[MAX_WORD];

    while (1) {
        pthread_mutex_lock(&lock);

        while (count == 0)
            pthread_cond_wait(&not_empty, &lock);

        strcpy(word, word_buffer[out]);
        out = (out + 1) % BUFFER_SIZE;
        count--;

        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&lock);

        if (strcmp(word, "END") == 0)
            break;

        int correct = check_dictionary(word);
        sleep(1);
        pthread_mutex_lock(&lock);
        pthread_t thread_id = pthread_self();
        unsigned long thread_num = (unsigned long)thread_id;
        if (result_index < BUFFER_SIZE) {
            if (correct)
                sprintf(result_buffer[result_index++], "[THREAD %lu] %s -> CORRECT", thread_num, word);
            else
                sprintf(result_buffer[result_index++], "[THREAD %lu] %s -> INCORRECT", thread_num, word);
        } else {
            printf("[SERVER] Result buffer is full, cannot store more results.\n");
        }

        pthread_mutex_unlock(&lock);
    }

    pthread_exit(NULL);
}

/* Master thread */
int main() {
    pthread_t threads[NUM_THREADS];
    pthread_t main_thread_id = pthread_self(); // Get the main thread ID

    printf("[MAIN THREAD %lu] Starting spell check server...\n", (unsigned long)main_thread_id);
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&not_empty, NULL);
    pthread_cond_init(&not_full, NULL);

    /* Words to check */
    char *words[] = {
        "hello", "world", "apple", "thread", "banana",
        "linux", "check", "car", "program", "spell"
    };

    int total_words = 10;

    /* Create spell check threads */
    for (int i = 0; i < NUM_THREADS; i++)
        pthread_create(&threads[i], NULL, spell_check, NULL);

    /* Master inserting words into buffer */
    for (int i = 0; i < total_words; i++) {
        pthread_mutex_lock(&lock);

        while (count == BUFFER_SIZE)
            pthread_cond_wait(&not_full, &lock);

        strcpy(word_buffer[in], words[i]);
        in = (in + 1) % BUFFER_SIZE;
        count++;

        printf("[SERVER] Word inserted: %s\n", words[i]);
        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&lock);
    }

    /* Send termination signal */
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_mutex_lock(&lock);
        strcpy(word_buffer[in], "END");
        in = (in + 1) % BUFFER_SIZE;
        count++;
        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&lock);
    }

    /* Join threads */
    for (int i = 0; i < NUM_THREADS; i++)
        pthread_join(threads[i], NULL);

    /* Print results */
    printf("\n---- Spell Check Results ----\n");
    for (int i = 0; i < result_index; i++)
        printf("%s\n", result_buffer[i]);

    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&not_empty);
    pthread_cond_destroy(&not_full);

    return 0;
}
[23bcs074@mepcolinux ex8]$./spell
[MAIN THREAD 139768002910016] Starting spell check server...
[SERVER] Word inserted: hello
[SERVER] Word inserted: world
[SERVER] Word inserted: apple
[SERVER] Word inserted: thread
[SERVER] Word inserted: banana
[SERVER] Word inserted: linux
[SERVER] Word inserted: check
[SERVER] Word inserted: car
[SERVER] Word inserted: program
[SERVER] Word inserted: spell

---- Spell Check Results ----
[THREAD 139767986202368] hello -> INCORRECT
[THREAD 139767919060736] apple -> INCORRECT
[THREAD 139767994595072] world -> INCORRECT
[THREAD 139767977809664] thread -> INCORRECT
[THREAD 139767961024256] banana -> INCORRECT
[THREAD 139767952631552] linux -> CORRECT
[THREAD 139767969416960] check -> CORRECT
[THREAD 139767944238848] spell -> INCORRECT
[THREAD 139767927453440] program -> CORRECT
[THREAD 139767935846144] car -> CORRECT

[23bcs074@mepcolinux ex8]$exit
exit
