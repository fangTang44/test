#include <arpa/inet.h>
#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

// 封装创建绑定监听
int c_socket() {
  // 创建套接字
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  assert(sockfd != -1);
  if (sockfd == -1) {
    return -1;
  }

  // 绑定套接字
  struct sockaddr_in saddr;
  bzero(&saddr, sizeof(saddr));
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(6000);
  // saddr.sin_addr.s_addr = inet_addr("192.168.31.173");
  saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

  int res = bind(sockfd, (struct sockaddr *)&saddr, sizeof(saddr));
  assert(res != -1);
  if (res == -1) {
    return -1;
  }

  // 监听套接字
  res = listen(sockfd, 5);
  assert(res != -1);
  if (res == -1) {
    return -1;
  }

  return sockfd;
}

// 获取请求方法和请求资源
void getMethodAndResou(char *buff, char *mentod, char *resou) {
  char *s = strtok(buff, " ");
  if (s == NULL) {
    return;
  }
  strcpy(mentod, s);
  printf("请求的方法: %s\n", mentod);

  s = strtok(NULL, " ");

  if (strcmp(s, "/") == 0) {
    s = "/index.html";
  }
  if (s == NULL) {
    return;
  }
  strcpy(resou, s);
  printf("请求的资源: %s\n", resou);
}

// 封装HTTP报头
void packageHeaders(char *sendBuff, int size) {
  strcpy(sendBuff, "HTTP/1.1 200 ok\r\n");
  strcat(sendBuff, "Server:myWeb_fangtang\r\n");
  sprintf(sendBuff + strlen(sendBuff), "Content-Lenth:%d\r\n", size);
  strcat(sendBuff, "\r\n");
}

// 发送报头以及文件
void sendPackage(int c, int fd, char *sendBuff) {
  send(c, sendBuff, strlen(sendBuff), 0);
  char data[512] = {0};
  int num = 0;
  while ((num = read(fd, data, 512)) > 0) {
    send(c, data, num, 0);
  }
}

// 请求处理
void requestProcessing(int c, char *mentod, char *resou) {
  // if (mentod == NULL || resou == NULL) {
  //   return;
  // }
  char path[128] = {"."};
  if (strncmp(mentod, "GET", 3) == 0) {
    strcat(path, resou);

    int fd = open(path, O_RDONLY);

    // 未找到资源发送404界面
    if (fd == -1) {
      fd = open("./404.html", O_RDONLY);
    }

    // 获取文件大小
    int size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    char *sendBuff = (char *)malloc(sizeof(char) * 512);
    packageHeaders(sendBuff, size);

    sendPackage(c, fd, sendBuff);

    free(sendBuff);
    close(fd);
  }
}

// 工作线程函数
void *worker(void *arg) {
  int c = *(int *)arg;

  char buff[1024] = {0};
  int n = recv(c, buff, 1023, 0);
  char *mentod = (char *)malloc(sizeof(char) * 256);
  char *resou = (char *)malloc(sizeof(char) * 256);

  // 获取请求方法
  getMethodAndResou(buff, mentod, resou);
  // 请求处理
  requestProcessing(c, mentod, resou);

  close(c);
  free(mentod);
  free(resou);
  return NULL;
}

int main() {
  // 创建
  int sockfd = c_socket();

  while (1) {
    // 绑定服务器ip以及端口号
    struct sockaddr_in caddr;
    unsigned int len = sizeof(caddr);
    bzero(&caddr, sizeof(caddr));
    int c = accept(sockfd, (struct sockaddr *)&caddr, &len);
    if (c < 0) {
      continue;
    }

    // 多线程
    pthread_t tid;
    pthread_create(&tid, NULL, worker, (void *)&c);
  }

  close(sockfd);
  exit(0);
}
