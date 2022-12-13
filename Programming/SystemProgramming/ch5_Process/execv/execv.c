#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>  //for exit

pid_t pid;
const char *args[] = {"ls", NULL};

int main(int argc, char *argv[])
{
  pid = fork();
  //child process
  if (!pid)
  {
	int ret;

	ret = execv("/bin/ls", (char* const*)args);
	if (ret == -1)
	{
	  perror("execv");
	  exit(EXIT_FAILURE);
	}

  }
}
