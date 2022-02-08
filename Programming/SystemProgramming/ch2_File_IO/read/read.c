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
  int len = 18;
  int ret = 0;
  int errorno = 0;
  off_t off_ret;
  ssize_t nr;

  const char *w_buf = "my ship is solid\n";


  fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0644); 
 //fd = creat(filename, 0644);
  if (fd == -1)
	  perror("creat");

  nr = write(fd, w_buf, strlen(w_buf));
  if ( nr == -1 )
	  perror("write");
  else if (nr != strlen(w_buf))
	  printf("nr != string length\n");

  off_ret = lseek(fd, 0, SEEK_SET);
  if (off_ret == (off_t) -1)
	  perror("lseek");

  while (len != 0 && (ret == read(fd, buf, len)) != 0) {
	  if (ret == -1) {
		if (errorno == EINTR)
			continue;
		perror("read");
		break;
	  }
	len -= ret;
	buf += ret;
  }

  printf("%s", buf);
  free(buf);
  close(fd);
	  
  return 0;
}

