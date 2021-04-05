#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>


#define BUFSIZE 16384

int main(int argc, char **argv)
{

  void *buf;
  int sf,df,rb,wb;
  int going = 1;
  int e = 0;

  if (argc != 3) {
    printf("usage: cpnt <sourcefile> <destfile>\n");
    printf("       sorry, only one file at a time yet.\n");
    return(1);
  }

#if 0
  printf("input : %s\n",argv[1]);
  printf("output: %s\n",argv[2]);
#endif

  buf = malloc(BUFSIZE);
  if (!buf) {
    printf("cpnt: could not allocate buffer\n");
    return(1);
  }

  sf = open(argv[1],O_RDONLY);
  if (sf < 0) {
    e = errno;
    printf("cpnt: %s: %s\n",argv[1],strerror(e));
    return(1);
  }

  df = open(argv[2],O_WRONLY|O_CREAT,00666);
  if (df < 0) {
    e = errno;
    printf("cpnt: %s: %s\n",argv[2],strerror(e));
    return(1);
  }

  while (going) {
    rb = read(sf,buf,BUFSIZE);
    if (rb < 0) {
      e = errno;
      printf("cpnt: error while reading: %s\n",strerror(e));
      going = 0;
      break;
    }
    if (rb == 0) going = 0;
    wb = write(df,buf,rb);
    if (wb < 0) {
      e = errno;
      printf("cpnt: error while writing: %s\n",strerror(e));
      going = 0;
    }
    
  }

  close(sf);
  close(df);
  free(buf);

  return(e ? 1 : 0);
}
