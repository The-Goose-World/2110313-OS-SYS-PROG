#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    printf("Please enter UNIX command\n");
    exit(1);
  }

  pid_t pid;
  pid = fork();

  if (pid < 0)
  {
    perror("Error: cannot fork\n");
    exit(1);
  }
  else if (pid == 0)
  {
    execvp(argv[1], &argv[1]);
    // If execvp fails, it will reach here
    perror("Error executing command");
    exit(1);
  }
  else
  {
    wait(NULL);
    return 0;
  }
}