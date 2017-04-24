#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define N 10
#define MAX_STRING 100

int in = 0, out = 0;
char buffer[MAX_STRING][N];
pthread_t producer[3];
pthread_t consumer[4];

pthread_mutex_t mutex;
sem_t empty, full;

void produce(int i) {
  FILE *fp;
  while (1) {
    sem_wait(&empty);
    pthread_mutex_lock(&mutex);
    fp = fopen("source.txt", "r");
    fgets(buffer[in], MAX_STRING, fp);
    fclose(fp);
    printf("[Producer %d] Wrote %d\n", i, in + 1);
    in = (in + 1) % N;
    pthread_mutex_unlock(&mutex);
    sem_post(&full);
  }
}

void consume(int i) {
  while (1) {
    sem_wait(&full);
    pthread_mutex_lock(&mutex);
    printf("[Consumer %d] Get %d: %s\n", i, out + 1, buffer[out]);
    out = (out + 1) % N;
    pthread_mutex_unlock(&mutex);
    sem_post(&empty);
  }
}

int main() {
  int i;
  sem_init(&empty, 0, N);
  sem_init(&full, 0, 0);
  pthread_mutex_init(&mutex, NULL);

  for (i = 0; i < 3; i++) {
    pthread_create(&producer[i], NULL, (void *)produce, (void *)i);
  }

  for (i = 0; i < 4; i++) {
    pthread_create(&consumer[i], NULL, (void *)consume, (void *)i);
  }

  for (i = 0; i < 3; i++) {
    pthread_join(producer[i], NULL);
  }

  for (i = 0; i < 4; i++) {
    pthread_join(consumer[i], NULL);
  }

  pthread_mutex_destroy(&mutex);
  sem_destroy(&empty);
  sem_destroy(&full);

  return 0;
}
