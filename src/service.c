#include "../inc/main.h"

/**
 * 广播
*/
/*
void *broadcast_msg(void *arg)
{
  //接收参数
  struct arg *args = (struct arg *)arg;

  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd != -1)
  {
    printf("广播sockfd是%d\n", sockfd);
    //设置广播属性给套接字
    int optval = 1;
    int ret = setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval));
    if (ret != -1)
    {
      struct sockaddr_in service_addr;
      service_addr.sin_family = AF_INET;
      service_addr.sin_port = htons(atoi(args->arg2));        //接收端口号
      service_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST); //广播地址

      //发送广播消息
    }
    else
    {
      perror("广播设置失败\n");
    }
  }
  else
  {
    perror("广播初始化失败(sockfd failed)");
  }
  //4.关闭套接字 close
  close(sockfd);
}
*/

/**
 * 聊天
*/
void *chat(void *arg)
{
  char port[6];
  //接收参数
  struct arg *args = (struct arg *)arg;
  strcpy(port, args->arg3);
  //printf("port %s\n", port);
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0); //udp
  if (sockfd != -1)
  {
    perror("聊天 服务启动");
    printf("聊天 服务端口 %s\n", port);
    //printf("sockfd :%d\n", sockfd);
    //地址初始化
    struct sockaddr_in servies_addr;
    bzero(&servies_addr, sizeof(servies_addr));
    servies_addr.sin_family = AF_INET;
    servies_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    servies_addr.sin_port = htons(atoi(port));
    //绑定地址
    bind(sockfd, (struct sockaddr *)&servies_addr, sizeof(servies_addr));

    fd_set collec_fd; //文件描述符集合
    struct timeval timeout;

    //客户端IP信息
    char ip[INET_ADDRSTRLEN];
    struct sockaddr_in client_addr;
    bzero(&client_addr, sizeof(client_addr));
    socklen_t len = sizeof(client_addr);

    char buf[1024];
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
        printf("超时60s 无人发送数据\n");

      if (FD_ISSET(sockfd, &collec_fd))
      {
        bzero(buf, sizeof(buf));
        recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&client_addr, &len);
        inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip));
        printf("客户端(%s)：%s\n", ip, buf);
        // if (!strncmp(buf, "bye", 3))
        //   break;
      }
      if (FD_ISSET(STDIN_FILENO, &collec_fd))
      {
        bzero(buf, sizeof(buf));
        printf("服务端：");
        scanf("%s", buf);
        sendto(sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&client_addr, len);
        if (!strncmp(buf, "bye", 3))
          break;
      }
    }
  }
  else
  {
    perror("服务器启动失败(sockfd failed)");
  }
  //4.关闭套接字 close
  printf("聊天关闭\n");
  close(sockfd);
}

/**
 * 聊天室
*/
void chat_room_serice()
{
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd != -1)
  {
    printf("聊天室 sockfd: %d 组播地址：224.0.0.100 端口：8888\n", sockfd);

    struct sockaddr_in service_addr;
    service_addr.sin_family = AF_INET;
    //service_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    inet_pton(AF_INET, "224.0.0.100", &service_addr.sin_addr);
    service_addr.sin_port = htons(8888);
    socklen_t len = sizeof(service_addr);

    // bind(sockfd, (struct sockaddr *)&service_addr, sizeof(service_addr));

    // struct sockaddr_in mcast_addr;
    // mcast_addr.sin_family = AF_INET; /*设置协议族类行为AF*/
    // //mcast_addr.sin_addr.s_addr = inet_addr("224.0.0.100");
    // inet_pton(AF_INET, "224.0.0.100", &mcast_addr.sin_addr); /*设置多播IP地址*/
    // mcast_addr.sin_port = htons(8888);                       /*设置多播端口*/

    // struct ip_mreqn mcast_addr;
    // inet_pton(AF_INET, "224.0.0.100", &mcast_addr.imr_multiaddr);
    // inet_pton(AF_INET, "0.0.0.0", &mcast_addr.imr_address);

    // setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_IF, &mcast_addr, sizeof(mcast_addr));

    // struct sockaddr_in client_addr;
    // client_addr.sin_family = AF_INET;
    // client_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    // client_addr.sin_port = htons(8888);

    char msg[1024];
    int size;
    while (1)
    {
      bzero(msg, sizeof(msg));
      printf("服务端: ");
      scanf("%s", msg);
      size = sendto(sockfd, msg, sizeof(msg), 0, (struct sockaddr *)&service_addr, sizeof(service_addr));
      perror("");
      printf("发送长度%d\n", size);
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
 * 文件传输进度
*/
void progress(int num, int filesize)
{
  int prog; //进度百分比
  prog = floor(((double)num / (double)filesize) * 100.0);
  if (prog >= 100)
    prog = 100;
  printf("%-14s" GR "[%d%%]\r" NONE, "文件传输进度: ", prog);
  fflush(stdout);
}

/**
 * 接收客户端上传的文件
*/
void *recv_upload(void *arg)
{
  //接收参数
  struct arg *args = (struct arg *)arg;
  char ip[INET_ADDRSTRLEN];
  char port[5];
  strcpy(port, args->arg3);
  printf("port %s \n", port);

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd != -1)
  {
    printf("服务端 sockfd: %d\n", sockfd);
    struct sockaddr_in service_addr;
    bzero(&service_addr, sizeof(service_addr));
    service_addr.sin_family = AF_INET;
    service_addr.sin_port = htons(atoi(port));
    service_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(sockfd, (struct sockaddr *)&service_addr, sizeof(service_addr));

    listen(sockfd, 1);

    struct sockaddr_in client_addr;
    bzero(&client_addr, sizeof(client_addr));
    socklen_t len = sizeof(client_addr);

    int connfd = accept(sockfd, (struct sockaddr *)&client_addr, &len);

    if (connfd != -1)
    {
      fd_set collective;
      int max_fd = connfd > 2 ? connfd + 1 : 2 + 1;
      struct timeval timeout;

      // printf("connfd %d\n", connfd);
      // printf("max_fd %d\n", max_fd);

      int rets;
      while (1)
      {
        FD_ZERO(&collective);
        FD_SET(connfd, &collective);

        timeout.tv_sec = 60;
        timeout.tv_usec = 0;

        rets = select(max_fd, &collective, NULL, NULL, &timeout);
        if (rets == 0)
        {
          printf("超时 断开连接\n");
          break;
        }
        //printf("rets %d \n", rets);

        if (FD_ISSET(connfd, &collective))
        {
          inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip));
          printf("当前连接对象是%s\n", ip);

          //接收文件名
          char filename[256];
          char recv_datas[1500];
          bzero(filename, sizeof(filename));
          bzero(recv_datas, sizeof(recv_datas));

          if (recv(connfd, recv_datas, sizeof(recv_datas), 0) > 0)
          {
            strcpy(filename, recv_datas);
            printf("接收文件：%s\n", filename);

            //创建服务端接收文件存储 文件目录
            if (access("./recv_upload", F_OK) != 0)
            {
              mkdir("./recv_upload", 0777);
            }
            char filepath[512];
            sprintf(filepath, "./recv_upload/%s", filename);
            int fd = open(filepath, O_CREAT | O_RDWR | O_APPEND);
            if (fd != -1)
            {
              //接收文件大小
              int filesize;
              recv(connfd, &filesize, sizeof(filesize), 0);
              printf("文件大小：%.2fK\n", (double)filesize / 1024.0);

              //接收数据
              int recv_size;
              int size;
              int total = 0;
              while (1)
              {
                bzero(recv_datas, sizeof(recv_datas));
                recv_size = recv(connfd, recv_datas, sizeof(recv_datas), 0);

                if (recv_size > 0)
                {
                  total += recv_size;
                  progress(total, filesize);
                  // printf("接收数据: %d\n", recv_size);
                }
                if (recv_size < 0)
                {
                  perror("接收文件失败");
                  close(fd);
                  break;
                }
                if (recv_size == 0)
                  break;
                write(fd, recv_datas, recv_size);
              }
              printf("文件接收成功\n");
              system("ls ./recv_upload/");
              close(fd);
              break;
            }
            else
            {
              perror("创建文件失败");
              break;
            }
          }
          else
          {
            perror("接收文件名失败");
            break;
          }
        }
      }
      //退出接受文件的上传
      close(connfd);
      close(sockfd);
    }
    else
    {
      close(sockfd);
      perror("接收客户端失败\n");
    }
  }
  else
    perror("接收客户端上传的文件 初始化失败(socket failed)");
}

void *recv_file(void *arg)
{
  //接收参数
  struct arg *args = (struct arg *)arg;
  int connfd = args->arg1;

  //创建服务端接收文件存储 文件目录
  if (access("./recv_upload", F_OK) != 0)
  {
    mkdir("./recv_upload", 0777);
  }

  //接收文件名
  char filename[256];
  bzero(filename, sizeof(filename));
  recv(connfd, filename, sizeof(filename), 0);
  perror("");
  printf("filename %s\n", filename);

  char filepath[512];
  sprintf(filepath, "./recv_upload/%s", filename);
  int fd = open(filepath, O_CREAT | O_RDWR | O_APPEND);
  if (fd != -1)
  {
    //接收文件大小
    int filesize;
    recv(connfd, &filesize, sizeof(filesize), 0);
    printf("文件名：%s 文件大小：%.2fK\n", filename, (double)filesize / 1024.0);

    //接收数据
    char recv_datas[1500];
    int recv_size;
    int size;
    int total = 0;
    while (1)
    {
      bzero(recv_datas, sizeof(recv_datas));
      recv_size = recv(connfd, recv_datas, sizeof(recv_datas), 0);

      if (recv_size > 0)
      {
        write(fd, recv_datas, recv_size);
        total += recv_size;
        progress(total, filesize);
      }
      if (recv_size < 0)
      {
        perror("接收文件失败");
        close(fd);
      }
      if (recv_size == 0)
        break;
    }
    printf("文件接收成功\n");
    system("ls ./recv_upload/");
    close(fd);
  }
  else
  {
    perror("创建文件失败");
  }
}

/**
 * 发送客户端下载文件
*/
void *send_download(void *arg)
{
  //接收参数
  struct arg *args = (struct arg *)arg;
  char ip[INET_ADDRSTRLEN];
  char port[5];
  strcpy(port, args->arg3);
  printf("port %s \n", port);

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd != -1)
  {
    printf("服务端 sockfd: %d\n", sockfd);
    struct sockaddr_in service_addr;
    bzero(&service_addr, sizeof(service_addr));
    service_addr.sin_family = AF_INET;
    service_addr.sin_port = htons(atoi(port));
    service_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(sockfd, (struct sockaddr *)&service_addr, sizeof(service_addr));

    listen(sockfd, 1);

    struct sockaddr_in client_addr;
    bzero(&client_addr, sizeof(client_addr));
    socklen_t len = sizeof(client_addr);

    int connfd = accept(sockfd, (struct sockaddr *)&client_addr, &len);

    if (connfd != -1)
    {

      fd_set collective;
      int max_fd = connfd > 2 ? connfd + 1 : 2 + 1;
      struct timeval timeout;

      // printf("connfd %d\n", connfd);
      // printf("max_fd %d\n", max_fd);

      int rets;
      while (1)
      {
        FD_ZERO(&collective);
        FD_SET(connfd, &collective);

        timeout.tv_sec = 60;
        timeout.tv_usec = 0;

        rets = select(max_fd, &collective, NULL, NULL, &timeout);
        //printf("rets  %d \n", rets);

        if (rets == 0)
        {
          printf("超时 断开连接\n");
          break;
        }
        //printf("rets %d \n", rets);

        if (FD_ISSET(connfd, &collective))
        {

          inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip));
          //printf("当前连接对象是%s\n", ip);

          char test[2];
          recv(connfd, test, sizeof(test), 0);
          printf("客户端(%s)：%s \n", ip, test);

          //文件名
          char filename[256];
          bzero(filename, sizeof(filename));

          /*向客户端发送文件列表*/
          if (access("./recv_upload", F_OK) != 0)
          {
            mkdir("./recv_upload", 0777);
          }
          DIR *upload_dp = opendir("./recv_upload");
          if (upload_dp != NULL)
          {
            struct dirent *upload_ep = NULL;
            while (1)
            {
              upload_ep = readdir(upload_dp);
              if (upload_ep == NULL)
                break;
              if (upload_ep->d_type == DT_REG)
              {
                if (send(connfd, upload_ep->d_name, sizeof(filename), 0) < 0)
                  perror("send");
              }
            }
            if (send(connfd, "end", sizeof(filename), 0) < 0)
              perror("send end");
          }

          /*接收文件名*/
          bzero(filename, sizeof(filename));
          if (recv(connfd, filename, sizeof(filename), 0) > 0)
          {
            printf("接收文件名: %s \n", filename);
          }
          else
          {
            perror("文件名接收失败");
            return NULL;
          }

          /*发送文件*/
          char filepath[512];
          bzero(filepath, sizeof(filepath));
          sprintf(filepath, "./recv_upload/%s", filename);
          int fd = open(filepath, O_RDONLY); //打开文件
          if (fd != -1)
          {
            //大小计算
            int filesize = lseek(fd, 0, SEEK_END);
            printf("%s 文件大小：%.2fK\n", filename, (double)filesize / 1024.0);
            lseek(fd, 0, SEEK_SET);
            char datas[1500]; //数据缓存
            int size;
            int send_size; //发送数据大小
            //发送文件大小
            send(connfd, &filesize, sizeof(filesize), 0);
            //发送
            int total = 0;
            while (1)
            {
              bzero(datas, sizeof(datas));
              size = read(fd, datas, sizeof(datas));

              if (size == 0)
                break;
              if (size < 0)
              {
                perror("读取文件错误");
                break;
              }
              send_size = send(connfd, datas, size, 0);
              if (send_size > 0)
              {
                //printf("发送数据%d\n", send_size);
                total += send_size;
                progress(total, filesize);
              }
              if (send_size < 0)
              {
                perror("发送文件失败");
                close(fd);
                return NULL;
              }
            }
            printf("\n发送文件成功\n");
            //3.关闭套接字
            close(connfd);
            close(fd);
            break;
          }
          else
          {
            perror("打开文件失败");
            send(connfd, "0", 1, 0);
            close(connfd);
            break;
          }
        }
      }
      //退出接受文件的上传
      close(sockfd);
    }
    else
      perror("连接客户端失败");
  }
  else
    perror("发送客户端下载文件 初始化失败(socket failed)");
}

void *send_files(void *arg)
{
  printf("111111111111111111111111\n");
  //接收参数
  struct arg *args = (struct arg *)arg;
  int connfd = args->arg1;

  //文件名
  char filename[256];
  bzero(filename, sizeof(filename));

  /*向客户端发送文件列表*/
  if (access("./recv_upload", F_OK) != 0)
  {
    mkdir("./recv_upload", 0777);
  }
  DIR *upload_dp = opendir("./recv_upload");
  if (upload_dp != NULL)
  {
    struct dirent *upload_ep = NULL;
    while (1)
    {
      upload_ep = readdir(upload_dp);
      if (upload_ep == NULL)
        break;
      if (upload_ep->d_type == DT_REG)
      {
        if (send(connfd, upload_ep->d_name, sizeof(filename), 0) < 0)
          perror("send");
      }
    }
    if (send(connfd, "end", sizeof(filename), 0) < 0)
      perror("send end");
  }

  /*接收文件名*/
  char filepath[512];
  while (1)
  {
    bzero(filename, sizeof(filename));
    if (recv(connfd, filename, sizeof(filename), 0) > 0)
      printf("接收文件名: %s \n", filename);
    else
    {
      perror("文件名接收失败");
      return NULL;
    }

    /*检测文件是否存在*/
    bzero(filepath, sizeof(filepath));
    sprintf(filepath, "./recv_upload/%s", filename);
    if (access(filepath, F_OK | R_OK) == -1)
    {
      perror("检测文件");
      send(connfd, "err:下载文件有误，请重新输入", 100, 0);
    }
    else
    {
      send(connfd, "Success", 100, 0);
      break;
    }
  }

  /*发送文件*/
  int fd = open(filepath, O_RDONLY); //打开文件
  if (fd != -1)
  {
    //大小计算
    int filesize = lseek(fd, 0, SEEK_END);
    printf("%s 文件大小：%.2fK\n", filename, (double)filesize / 1024.0);
    lseek(fd, 0, SEEK_SET);

    char datas[1500]; //数据缓存
    int size;
    int send_size; //发送数据大小
    //发送文件大小
    send(connfd, &filesize, sizeof(filesize), 0);

    //发送
    int total = 0;
    while (1)
    {
      bzero(datas, sizeof(datas));
      size = read(fd, datas, sizeof(datas));

      if (size == 0)
        break;
      if (size < 0)
      {
        perror("读取文件错误");
        break;
      }
      send_size = send(connfd, datas, size, 0);
      if (send_size > 0)
      {
        total += send_size;
        progress(total, filesize);
      }
      if (send_size < 0)
      {
        perror("发送文件失败");
        close(fd);
        return NULL;
      }
    }
    printf("\n发送文件成功\n");
    //3.关闭套接字
    close(connfd);
    close(fd);
  }
  else
  {
    perror("打开文件失败");
    send(connfd, "0", 1, 0);
    close(connfd);
  }
}

void servies()
{
  //线程池初始化
  thread_pool *pool = (thread_pool *)malloc(sizeof(thread_pool));
  init_pool(pool, 4);

  /*启动聊天服务*/
  struct arg *task_arg1 = (struct arg *)malloc(sizeof(struct arg));
  strcpy(task_arg1->arg3, "8080");
  add_task(pool, chat, (void *)task_arg1);

  /*聊天室*/
  //chat_room_serice();

  pid_t send_pid;
  send_pid = fork();
  /*启动文件下载服务*/
  if (send_pid == 0)
  {
    int sockfd_send = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd_send != -1)
    {
      perror("文件下载服务开启");
      printf("文件下载服务 端口：8082\n");

      char ip[INET_ADDRSTRLEN];
      //初始化服务端IP
      struct sockaddr_in service_addr;
      bzero(&service_addr, sizeof(service_addr));
      service_addr.sin_family = AF_INET;
      service_addr.sin_port = htons(atoi("8082"));
      service_addr.sin_addr.s_addr = htonl(INADDR_ANY);

      bind(sockfd_send, (struct sockaddr *)&service_addr, sizeof(service_addr));
      listen(sockfd_send, 1);

      //客户端初始化
      struct sockaddr_in client_addr;
      bzero(&client_addr, sizeof(client_addr));
      socklen_t len = sizeof(client_addr);

      int connfd;
      struct arg *task_arg3 = (struct arg *)malloc(sizeof(struct arg));
      while (1)
      {
        //检测接入
        connfd = accept(sockfd_send, (struct sockaddr *)&client_addr, &len);
        if (connfd != -1)
        {
          inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip));
          printf("下载服务: 当前连接的客户端(%s)\n", ip);

          /*启动文件上传服务*/
          task_arg3->arg1 = connfd;
          //add_task(pool, send_files, (void *)task_arg3);
          // printf("22222222222222222222222\n");
          send_files((void *)task_arg3);
        }
      }
    }
  }

  /*启动文件上传服务*/
  int sockfd_recv = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd_recv != -1)
  {
    printf("\n===================================\n");
    perror("文件上传服务开启");
    printf("文件上传服务 端口：8081\n");

    char ip[INET_ADDRSTRLEN];
    //初始化服务端IP
    struct sockaddr_in service_addr;
    bzero(&service_addr, sizeof(service_addr));
    service_addr.sin_family = AF_INET;
    service_addr.sin_port = htons(atoi("8081"));
    service_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(sockfd_recv, (struct sockaddr *)&service_addr, sizeof(service_addr));
    listen(sockfd_recv, 1);

    //客户端初始化
    struct sockaddr_in client_addr;
    bzero(&client_addr, sizeof(client_addr));
    socklen_t len = sizeof(client_addr);

    int connfd;
    struct arg *task_arg2 = (struct arg *)malloc(sizeof(struct arg));
    while (1)
    {
      //检测接入
      connfd = accept(sockfd_recv, (struct sockaddr *)&client_addr, &len);
      if (connfd != -1)
      {
        inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip));
        printf("上传服务: 当前连接的客户端(%s)\n", ip);

        /*启动文件上传服务*/
        task_arg2->arg1 = connfd;
        add_task(pool, recv_file, (void *)task_arg2);
      }
    }
  }

  wait(NULL);
  //销毁线程池
  destroy_pool(pool);
}

int main(int argc, char const *argv[])
{
  servies();
  return 0;
}
