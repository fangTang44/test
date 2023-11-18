#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>

//信息结构体
typedef struct sockInfo{
    struct sockaddr_in addr;
    int fd;
}sockInfo;

struct sockInfo infos[512];

void* working(void* arg) {
    struct sockInfo* pinfo = (struct sockInfo*)arg;
    //建立连接成功,打印客户端的IP和端口信息
    char ip[32];
    printf("客户端IP: %s, 端口: %d\n",
           inet_ntop(AF_INET, &pinfo->addr.sin_addr.s_addr, ip, sizeof(ip)),
           ntohs(pinfo->addr.sin_port));

    //通信
    while (1) {
        //接收数据
        char buff[1024];
        int len = recv(pinfo->fd, buff, sizeof(buff), 0);
        if (len > 0) {
            printf("clinet:%s say: %s", inet_ntop(AF_INET, &pinfo->addr.sin_addr.s_addr, ip, sizeof(ip)), buff);
            send(pinfo->fd, "ok", 3, 0);
        }
        else if(len == 0) {
            printf("客户端IP: %s, 端口: %d 已断开连接\n",
                inet_ntop(AF_INET, &pinfo->addr.sin_addr.s_addr, ip, sizeof(ip)),
                ntohs(pinfo->addr.sin_port));
            break;
        }
        else {
            perror("recv");
            break;
        }
    }
    //关闭文件描述符
    close(pinfo->fd);
    pinfo->fd = -1;

    return NULL;
}

int main(int argc, char *argv[]) {
    //创建监听的套接字
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket");
        return -1;
    }

    //绑定本地IP port
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(9999);
    saddr.sin_addr.s_addr = INADDR_ANY; //0 = 0.0.0.0
    int ret = bind(fd, (struct sockaddr*)&saddr, sizeof(saddr));
    if (ret == -1) {
        perror("bind");
        return -1;
    }

    //设置监听
    ret = listen(fd, 128);
    if (ret == -1) {
        perror("listen");
        return -1;
    }

    // 初始化结构体数组
    int max = sizeof(infos) / sizeof(infos[0]);
    for (int i = 0; i < max; ++i) {
        bzero(&infos[i], sizeof(infos[i]));
        infos[i].fd = -1;
    }

    //阻塞并等待客户端连接
    struct sockaddr_in caddr;
    unsigned int addrlen = sizeof(struct sockaddr_in);
    while (1) {
        struct sockInfo* pinfo;
        for (int i = 0; i < max; i++) {
            if (infos[i].fd == -1) {
                pinfo = &infos[i];
                break;
            }
        }
        int cfd = accept(fd, (struct sockaddr*)&pinfo->addr, &addrlen);
        pinfo->fd = cfd;
        if (pinfo->fd == -1) {
            perror("accept");
            break;
        }

        //创建工作线程
        pthread_t tid;
        pthread_create(&tid, NULL, working, pinfo);
        pthread_detach(tid);
    }

    //关闭文件描述符
    close(fd);
    exit(0);
}
