#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define TIMEOUT 5    // wait time of select (unit: sec)
#define BUF_LEN 1024

int main(void)
{
  struct timeval tv;
  fd_set readfds;
  int ret;

  FD_ZERO(&readfds);
  FD_SET(STDIN_FILENO, &readfds);

  tv.tv_sec = TIMEOUT;
  tv.tv_usec = 0;

  printf("select (timeout: %d s)\n", TIMEOUT);
  ret = select(STDIN_FILENO + 1,
		  &readfds,
		  NULL,
		  NULL,
		  &tv);
  if (ret == -1)
  {
	perror("select");
	return 1;
  }
  else if (!ret)
  {
	printf("%d seconds elapsed.\n", TIMEOUT);
  }

  if (FD_ISSET(STDIN_FILENO, &readfds))
  {
	char buf[BUF_LEN + 1];
	int len;
	
	len = read(STDIN_FILENO, buf, BUF_LEN);
	if (len == -1)
	{
		perror("read");
		return 1;
	}
	
	if (len)
	{
		buf[len] = '\0';
		printf("read: %s\n", buf);
	}
	return 0;
  }

  fprintf(stderr, "This should not happen!\n");
  return -1;

}
