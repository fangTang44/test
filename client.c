#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <string.h>

int main(int argc, char *argv[]) {
    //1.创建通信的套接字
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket");
        return -1;
    }

    //2.连接服务器IP port
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(9999);
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    int ret = connect(fd, (struct sockaddr*)&saddr, sizeof(saddr));
    if (ret == -1) {
        perror("connect");
        return -1;
    }

    //3.通信
    while (1) {
        //发送数据
        char buff[1024];
        fgets(buff, sizeof(buff), stdin);
        if (strcmp(buff, "end\n") == 0) break;
        send(fd, buff, strlen(buff) + 1, 0);

        //接收数据
        memset(buff, 0, sizeof(buff));
        int len = recv(fd, buff, sizeof(buff), 0);
        if (len > 0) {
            printf("server say: %s\n", buff);
        }
        else if(len == 0) {
            printf("服务器已经断开连接\n");
            break;
        }
        else {
            perror("recv");
            break;
        }
    }

    //关闭文件描述符
    close(fd);

    exit(0);
}
