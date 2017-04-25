#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

pthread_mutex_t *mutex;
pthread_mutexattr_t mattr;

char message1[] = "Child process 1 is sending a message!\n";
char message2[] = "Child process 2 is sending a message!\n";

sem_t *child_two_can_start;

int main(void) {
  int fd[2];
  pid_t child1, child2;
  char readbuffer[80];


  pipe(fd);

  if (pipe(fd) == -1) {
    printf("Error creating pipes.\n");
    exit(1);
  }

  if ((child1 = fork()) == -1) {
    printf("Error creating first child.\n");
    exit(1);
  }
  
  if (child1 == 0) {
    // Child 1
    close(fd[0]);
    write(fd[1], message1, (strlen(message1) + 1));
    return
  }

  // Parent
  if ((child2 = fork()) == -1) {
    printf("Error creating second child.\n");
    exit(1);
  }
  
  if (child2 == 0) {
    // Child 2
    close(fd[0]);
    waitpid(child1, NULL, 0);
    write(fd[1], message2, (strlen(message2) + 1));
    exit(0);
  } else {
    close(fd[1]);
    read(fd[0], readbuffer, sizeof(readbuffer));
    printf("%s", readbuffer);
    waitpid(child2, NULL, 0);
    read(fd[0], readbuffer, sizeof(readbuffer));
    printf("%s", readbuffer);
    pthread_mutex_destroy(mutex);
    return 0;
  }
}