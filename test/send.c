#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <stdbool.h>
#include <strings.h>
#include <string.h>
#include <syslog.h>
#include <sys/file.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <pthread.h>
#include <semaphore.h>
#include <dirent.h>
#include <fcntl.h>
#include <math.h>
#include <sys/mman.h>
#include <linux/input.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define HELLO_PORT 12345
#define HELLO_GROUP "225.0.0.37"
int main(int argc, char *argv[])
{
  struct sockaddr_in addr;
  int fd, cnt;
  struct ip_mreq mreq;
  char *message = "Hello, World!";

  if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
  {
    perror("socket");
    exit(1);
  }

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(HELLO_GROUP);
  addr.sin_port = htons(HELLO_PORT);

  while (1)
  {
    if (sendto(fd, message, strlen(message), 0, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
      perror("sendto");
      exit(1);
    }
    sleep(1);
  }
}
