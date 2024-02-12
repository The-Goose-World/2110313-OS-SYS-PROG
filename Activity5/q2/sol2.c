#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 10

void *display_number(void *data)
{
  printf("This is thread %d\n", *((int *)data));
}

int main()
{
  pthread_t threads[NUM_THREADS];
  int thread_args[NUM_THREADS];

  for (int i = 0; i < NUM_THREADS; i++)
  {
    thread_args[i] = i + 1;
    pthread_create(&threads[i], NULL, display_number, &thread_args[i]);
    pthread_join(threads[i], NULL);
  }

  return 0;
}
