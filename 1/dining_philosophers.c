#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


pthread_t philosopher[5];
pthread_mutex_t chopstick[5];

void random_sleep(void) {
  int t = rand() % 3;
  sleep(t + 1);
}

void eat(int n) {
  do {
    printf("Philosopher %d is thinking\n", n);
    random_sleep();

    printf("Philosopher %d is going to eat\n", n);
    if (n % 2 != 0) {
      pthread_mutex_lock(&chopstick[n]);
      pthread_mutex_lock(&chopstick[(n + 1) % 5]);
      printf("Philosopher %d is eating\n", n);
      pthread_mutex_unlock(&chopstick[n]);
      pthread_mutex_unlock(&chopstick[(n + 1) % 5]);
      printf("Philosopher %d finished eating\n", n);
    } else {
      pthread_mutex_lock(&chopstick[(n + 1) % 5]);
      pthread_mutex_lock(&chopstick[n]);
      printf("Philosopher %d is eating\n", n);
      pthread_mutex_unlock(&chopstick[(n + 1) % 5]);
      pthread_mutex_unlock(&chopstick[n]);
      printf("Philosopher %d finished eating\n", n);
    }

    random_sleep();
  } while (1);
}

int main(void) {
  srand((unsigned)time(NULL));
  int i;
  for (i = 0; i < 5; i++) {
    pthread_mutex_init(&chopstick[i], NULL);
  }

  for (i = 0; i < 5; i++) {
    pthread_create(&philosopher[i], NULL, (void *)eat, (void *)i);
  }

  for (i = 0; i < 5; i++) {
    pthread_join(philosopher[i], NULL);
  }

  for (i = 0; i < 5; i++) {
    pthread_mutex_destroy(&chopstick[i]);
  }

  return 0;
}
