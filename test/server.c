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
#define MSGBUFSIZE 256
int main(int argc, char *argv[])
{
  struct sockaddr_in addr;
  int fd, nbytes, addrlen;
  struct ip_mreq mreq;
  char msgbuf[MSGBUFSIZE];
  u_int yes = 1;

  if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
  {
    perror("socket");
    exit(1);
  }

  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
  {
    perror("Reusing ADDR failed");
    exit(1);
  }

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(HELLO_PORT);

  if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
  {
    perror("bind");
    exit(1);
  }

  mreq.imr_multiaddr.s_addr = inet_addr(HELLO_GROUP);
  mreq.imr_interface.s_addr = htonl(INADDR_ANY);
  if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
  {
    perror("setsockopt");
    exit(1);
  }

  while (1)
  {
    addrlen = sizeof(addr);
    if ((nbytes = recvfrom(fd, msgbuf, MSGBUFSIZE, 0, (struct sockaddr *)&addr, (socklen_t *)&addrlen)) < 0)
    {
      perror("recvfrom");
      exit(1);
    }
    puts(msgbuf);
  }
  return 0;
}