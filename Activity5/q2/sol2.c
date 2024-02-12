#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 10

// Thread function
void *display_number(void *arg)
{
  int thread_number = *((int *)arg);
  printf("This is thread %d\n", thread_number);
  return NULL;
}

int main()
{
  pthread_t threads[NUM_THREADS];
  int thread_args[NUM_THREADS];
  int i;

  // Create threads
  for (i = 0; i < NUM_THREADS; i++)
  {
    thread_args[i] = i + 1; // Thread numbers start from 1
    pthread_create(&threads[i], NULL, display_number, &thread_args[i]);
    pthread_join(threads[i], NULL); // Wait for this thread to complete before starting the next one
  }

  return 0;
}
