#include <sys/mman.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char message1[] = "Child process 1 is sending a message!\n";
char message2[] = "Child process 2 is sending a message!\n";
pthread_mutex_t *mutex;

int main(void) {
  int fd[2];
  pid_t child1, child2;
  char readbuffer[80];

  int size1 = strlen(message1) + 1,
    size2 = strlen(message2) + 1;
  pthread_mutexattr_t mattr;

	mutex = mmap(0, sizeof(pthread_mutex_t), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
	pthread_mutexattr_init(&mattr);
	pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
	pthread_mutex_init(mutex, &mattr);

  pipe(fd);
  pthread_mutex_lock(mutex);

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
    sleep(1);
    write(fd[1], message1, size1); 
    return 0;
  }

  // Parent
  if ((child2 = fork()) == -1) {
    printf("Error creating second child.\n");
    exit(1);
  }
  
  if (child2 == 0) {
    // Child 2
    close(fd[0]);
    pthread_mutex_lock(mutex);
    
    write(fd[1], message2, size2);
    return 0;
  }

  close(fd[1]);
  waitpid(child1, NULL, 0);
  read(fd[0], readbuffer, sizeof(readbuffer));
  printf("%s", readbuffer);
  pthread_mutex_unlock(mutex);
  waitpid(child2, NULL, 0);
  read(fd[0], readbuffer, sizeof(readbuffer));
  printf("%s", readbuffer);
  return 0;
}