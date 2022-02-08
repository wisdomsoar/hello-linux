#include <unistd.h>	//for read/write
#include <stdio.h>
#include <stdlib.h>	//for malloc
#include <string.h>	//for strlen
#include <errno.h>      //for EINTR
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc, char *argv[])
{
  int fd;
  char filename[] = "test_file.txt";
  char * buf = malloc(20);
  //int errno = 0;
  int nr = 0;

  fd = open(filename, O_RDWR | O_NONBLOCK, 0644);

start:
  nr = read(fd, buf, BUFSIZ);
  if (nr == -1)
  {
	if (errno == EINTR)
		goto start;  //oh, shi...
	if (errno == EAGAIN)
		printf("try again later\n");
		
	else
		perror("read");
	
  }

  printf("%s", buf);
  free(buf);
  close(fd);
	  
  return 0;
}

