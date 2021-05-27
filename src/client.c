#include "../inc/main.h"

void progress(int num, int filesize)
{
  int prog; //进度百分比
  prog = floor(((double)num / (double)filesize) * 100.0);
  if (prog >= 100)
    prog = 100;
  printf("%-14s" GR "[%d%%]\r" NONE, "传输进度：", prog);
  fflush(stdout);
}

/**
 * 聊天
*/
void *chat_client(void *arg)
{
  char ip[INET_ADDRSTRLEN];
  char port[6];

  //接收参数
  struct arg *args = (struct arg *)arg;
  strcpy(ip, args->arg2);
  strcpy(port, args->arg3);

  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd != -1)
  {
    perror("客户端启动");
    printf("sockfd :%d\n", sockfd);

    //IP地址初始化
    struct sockaddr_in servic_addr;
    bzero(&servic_addr, sizeof(servic_addr));
    servic_addr.sin_family = AF_INET;
    servic_addr.sin_port = htons(atoi(port));
    inet_pton(AF_INET, ip, &servic_addr.sin_addr);
    socklen_t len = sizeof(servic_addr);

    fd_set collec_fd; //文件描述符集合
    struct timeval timeout;

    char msg[1024];
    int send_size;
    int ret;
    while (1)
    {
      FD_ZERO(&collec_fd);
      FD_SET(sockfd, &collec_fd);
      FD_SET(STDIN_FILENO, &collec_fd);
      timeout.tv_sec = 60;
      timeout.tv_usec = 0;
      ret = select(sockfd + 1, &collec_fd, NULL, NULL, &timeout);

      if (ret == 0)
      {
        printf("超时 与服务器断开\n");
        break;
      }

      if (FD_ISSET(STDIN_FILENO, &collec_fd))
      {
        bzero(msg, sizeof(msg));
        printf("客户端：");
        scanf("%s", msg);
        send_size = sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr *)&servic_addr, len);
        //退出
        if (!strncmp(msg, "bye", 3))
          break;
      }

      if (FD_ISSET(sockfd, &collec_fd))
      {
        bzero(msg, sizeof(msg));
        recvfrom(sockfd, msg, sizeof(msg), 0, (struct sockaddr *)&servic_addr, &len);
        printf("服务端：%s\n", msg);
        if (!strncmp(msg, "bye", 3))
          break;
      }
    }
    close(sockfd);
  }
  else
  {
    perror("客户端启动失败(sockfd failed)");
  }
}

/**
 * 聊天室 组播
*/
//void *chat_room(void *arg)
void chat_room_client()
{
  char ip[INET_ADDRSTRLEN];
  char port[6];

  printf("请输入聊天室ip：\n");
  scanf("%s", ip);
  printf("请您输入服务器端口号\n");
  scanf("%s", port);

  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd != -1)
  {
    printf("聊天室 sockfd是%d 组播地址：%s\n", sockfd, ip);

    /*初始化地址*/
    struct sockaddr_in local_addr; //本地地址
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    local_addr.sin_port = htons(atoi(port));

    /*绑定socket*/
    bind(sockfd, (struct sockaddr *)&local_addr, sizeof(local_addr));

    /*设置回环许可*/
    // int loop = 1;
    // int rets = setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop));

    //定义组播地址
    struct ip_mreq servies_addr;
    bzero(&servies_addr, sizeof(servies_addr));
    inet_pton(AF_INET, ip, &servies_addr.imr_multiaddr);   /*组播播地址*/
    servies_addr.imr_interface.s_addr = htonl(INADDR_ANY); /*网络接口为默认*/

    //将组播属性加入到套接字
    int ret = setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &servies_addr, sizeof(servies_addr));
    if (ret != -1)
    {
      //接收客户端的消息   recvfrom
      struct sockaddr_in addr;
      socklen_t len = sizeof(addr);
      char msg[1024];
      char ip[INET_ADDRSTRLEN];
      while (1)
      {
        bzero(msg, sizeof(msg));
        recvfrom(sockfd, msg, sizeof(msg), 0, (struct sockaddr *)&addr, &len);
        inet_ntop(AF_INET, &addr.sin_addr, ip, sizeof(ip));
        printf("服务端(%s)：%s\n", ip, msg);
      }
    }
  }
  else
  {
    perror("聊天室 初始化失败(sockfd failed)");
  }
  //4.关闭套接字 close
  close(sockfd);
}

/**
 * 上传文件 tcp
*/
void *upload(void *arg)
{
  char ip[INET_ADDRSTRLEN];
  char port[5];
  //接收参数
  struct arg *args = (struct arg *)arg;
  strcpy(ip, args->arg2);
  strcpy(port, args->arg3);
  printf("ip %s \n", ip);
  printf("port %s \n", port);

  char filepath[512];
  printf("请输入需要上传的文件路径\n");
  scanf("%s", filepath);
  char filename[256];
  while (1)
  {
    if (strrchr(filepath, '/') == NULL)
    {
      printf("文件路径有误\n");
      printf("请输入需要上传的文件路径,例如 “./a.txt” \n");
      scanf("%s", filepath);
    }
    if (access(filepath, F_OK | R_OK) != -1)
      break;
  }

  strcpy(filename, strrchr(filepath, '/') + 1);

  //1.创建套接字 socket
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd != -1)
  {
    printf("客户端 上传文件 sockfd: %d\n", sockfd);

    struct sockaddr_in service_addr;
    bzero(&service_addr, sizeof(service_addr));
    service_addr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &service_addr.sin_addr);
    service_addr.sin_port = htons(atoi(port));

    int ret = connect(sockfd, (struct sockaddr *)&service_addr, sizeof(service_addr));
    if (ret != -1)
    {
      //发送文件名
      if (send(sockfd, filename, sizeof(filename), 0) > 0)
      {
        //打开文件
        int fd = open(filepath, O_RDONLY);
        if (fd != -1)
        {
          //大小计算
          int filesize = lseek(fd, 0, SEEK_END);
          printf("文件名：%s 文件大小：%.2fK\n", filename, (double)filesize / 1024.0);
          lseek(fd, 0, SEEK_SET);

          //发送文件大小
          send(sockfd, &filesize, sizeof(filesize), 0);

          char datas[1500]; //数据缓存
          int size;
          int rets;
          int total;
          //发送
          while (1)
          {
            bzero(datas, sizeof(datas));
            size = read(fd, datas, sizeof(datas));
            rets = send(sockfd, datas, size, 0);
            if (rets > 0)
            {
              //printf("发送数据%d\n", rets);
              total += rets;
              progress(total, filesize);
            }
            else if (rets < 0)
            {
              perror("发送失败");
              close(sockfd);
              close(fd);
              return NULL;
            }

            if (size < 0)
            {
              perror("读取文件错误");
              break;
            }
            if (size == 0)
              break;
          }
          printf("\n上传文件成功\n");
          sleep(2);
          //3.关闭套接字
          close(sockfd);
          close(fd);
        }
        else
        {
          close(sockfd);
          perror("打开文件失败");
        }
      }
      else
      {
        close(sockfd);
        perror("发送文件名失败");
      }
    }
    else
    {
      close(sockfd);
      perror("连接服务器失败");
    }
  }
  else
    perror("上传初始化失败(socket failed)");
}

/**
 * 下载文件
*/
void *download(void *arg)
{
  char ip[INET_ADDRSTRLEN];
  char port[5];
  //接收参数
  struct arg *args = (struct arg *)arg;
  strcpy(ip, args->arg2);
  strcpy(port, args->arg3);
  printf("ip %s \n", ip);
  printf("port %s \n", port);

  //1.创建套接字 socket
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd != -1)
  {
    printf("客户端 下载文件 sockfd: %d\n", sockfd);

    struct sockaddr_in service_addr;
    bzero(&service_addr, sizeof(service_addr));
    service_addr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &service_addr.sin_addr);
    service_addr.sin_port = htons(atoi(port));

    int ret = connect(sockfd, (struct sockaddr *)&service_addr, sizeof(service_addr));

    if (ret != -1)
    {
      char filename[256];

      //接收服务端文件列表
      printf("-------------服务端文件列表-------------\n");
      while (1)
      {
        bzero(filename, sizeof(filename));
        if (recv(sockfd, filename, sizeof(filename), 0) < 0)
          perror("recv");
        if (!strncmp(filename, "end", 3))
          break;
        printf("%s \n", filename);
      }

      /*检测文件无误*/
      while (1)
      {
        char buf[100];
        //发送下载文件名
        printf("请输入下载文件\n");
        bzero(filename, sizeof(filename));
        scanf("%s", filename);
        if (send(sockfd, filename, sizeof(filename), 0) < 0)
        {
          perror("send");
          close(sockfd);
          printf("正在退出(....2s)\n");
          sleep(2);
          return NULL;
        }
        recv(sockfd, buf, sizeof(buf), 0);
        if (!strncmp(buf, "err", 3))
        {
          printf("%s \n", buf);
        }
        else if (!strcmp(buf, "Success"))
          break;
      }

      /*下载数据*/
      if (access("./download", F_OK) != 0)
      { //文件目录
        mkdir("./download", 0777);
      }
      char filepath[512];
      sprintf(filepath, "./download/%s", filename);

      //接收文件大小
      int filesize;
      recv(sockfd, &filesize, sizeof(filesize), 0);
      printf("%s 文件大小：%.2fK\n", filename, (double)filesize / 1024.0);

      //打开文件
      int fd = open(filepath, O_CREAT | O_RDWR | O_APPEND);
      if (fd != -1)
      {
        char recv_datas[1500];
        int recv_size;
        int total;
        //接收
        while (1)
        {
          bzero(recv_datas, sizeof(recv_datas));
          recv_size = recv(sockfd, recv_datas, sizeof(recv_datas), 0);

          if (recv_size > 0)
          {
            total += recv_size;
            progress(total, filesize);
          }
          if (recv_size < 0)
          {
            perror("下载失败");
            close(sockfd);
            close(fd);
            char cmd[256];
            sprintf(cmd, "rm ./download/%s", filename);
            system(cmd);
            return NULL;
          }
          if (recv_size == 0)
            break;
          write(fd, recv_datas, recv_size);
        }
        printf("\n下载文件成功\n");
        system("ls ./download");
        close(fd);
        close(sockfd);
        printf("正在退出(....2s)\n");
        sleep(2);
      }
      else
      {
        close(sockfd);
        perror("打开文件失败");
        printf("正在退出(....2s)\n");
        sleep(2);
      }
    }
    else
    {
      close(sockfd);
      perror("连接服务器失败");
      printf("正在退出(....2s)\n");
      sleep(2);
    }
  }
  else
    perror("下载初始化失败(socket failed)");
  printf("正在退出(....2s)\n");
  sleep(2);
}

int main(int argc, char const *argv[])
{
  // 线程池初始化
  thread_pool *pool = (thread_pool *)malloc(sizeof(thread_pool));
  init_pool(pool, 1);

  char ip[INET_ADDRSTRLEN];
  char port[5];
  printf("请输入服务端ip地址\n");
  scanf("%s", ip);

  int num = 0;
  char c;
  while (1)
  {
    system("clear");
    printf("\n=================请选择服务项目=================\n");
    printf("服务端ip: %s\n", ip);
    printf("1、聊天 \n");
    printf("2、上传文件\n");
    printf("3、下载文件\n");
    printf("4、退出\n");
    printf("请输入序号\n");
    printf("================================================\n");
    while (1)
    {
      c = getchar();
      num = atoi(&c);
      if (num < 5 && num > 0)
        break;
    }

    struct arg *task_arg1 = (struct arg *)malloc(sizeof(struct arg));
    struct arg *task_arg2 = (struct arg *)malloc(sizeof(struct arg));
    struct arg *task_arg3 = (struct arg *)malloc(sizeof(struct arg));
    pid_t chat_pid;
    pid_t upload_pid;
    pid_t download_pid;

    switch (num)
    {
    case 1:
      chat_pid = fork();
      if (chat_pid == 0)
      {
        strcpy(task_arg1->arg2, ip);
        strcpy(task_arg1->arg3, "8080");
        //add_task(pool, chat_client, (void *)task_arg1);
        chat_client((void *)task_arg1);
      }
      wait(NULL);
      break;
    case 2:
      upload_pid = fork();
      if (upload_pid == 0)
      {
        strcpy(task_arg2->arg2, ip);
        strcpy(task_arg2->arg3, "8081");
        //add_task(pool, upload, (void *)task_arg2);
        upload((void *)task_arg2);
      }
      wait(NULL);
      break;
    case 3:
      download_pid = fork();
      if (download_pid == 0)
      {
        strcpy(task_arg3->arg2, ip);
        strcpy(task_arg3->arg3, "8082");
        //add_task(pool, download, (void *)task_arg3);
        download((void *)task_arg3);
      }
      wait(NULL);
      break;
    }
    if (num == 4)
      break;
  }
  //销毁线程池
  destroy_pool(pool);
  return 0;
}