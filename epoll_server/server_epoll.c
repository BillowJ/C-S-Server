#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <errno.h>
#include <ctype.h>

#include "wrap.h"

#define MAXLINE 8192
#define SERV_PORT 8000
#define OPEN_MAX 5000

int main(int argc, char* argv[]){

    int i;
    int num = 0;    //client num
    int listenfd, connfd, sockfd;

    struct sockaddr_int Serv_Addr, Clie_Addr;
    struct epoll_event tep; 
    struct epoll_event ep[OPEN_MAX];
    char buf[MAXLINE], str
    socklen_t Clie_Addr_len;

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    bzero(&Serv_Addr, sizeof(Serv_Addr));
    Serv_Addr.sin_family = AF_INET;
    Serv_Addr.sin_port = htons(SERV_PORT);
    Serv_Addr.sin_addr.s_addr = htonl(INADDR_ANY);

    Bind(listenfd,(struct sockaddr *)&Serv_Addr, sizeof(Serv_Addr));
    Listen(listenfd, 20);

    efd = epoll_create(OPEN_MAX); //创建epoll模型, efd指向红黑树根节点
    if(efd == -1){
        perr_exit("epoll_create error");
    }

    tep.events = EPOLLIN; 
    tep.data.fd = listenfd;
    res = epoll_ctl(efd, EPOLL_CTL_ADD, listenfd, &tep);
    if(res == -1){
        perr_exit("epoll_ctl error");
    }

    for( ; ; ){
        //ep为传出数组 OPEN_MAX为每次能处理的事件, -1:阻塞
        nready = epoll_wait(efd, ep, OPEN_MAX, -1)
        if(nready == -1){
            perr_exit("opoll_wait error");
        }
        for(i = 0; i < nready; i++){
            //不是读事件就跳过.
            if(!(ep[i].events & EPOLLIN)){
                continue;
            }

            if(ep[i].data.fd == listenfd){
                Clie_Addr_len = sizeof(Clie_Addr);
                connfd = Accept(listenfd,
                (stuct sockaddr *)&Clie_Addr, Clie_Addr_len);

                printf("received from %s at %d\n",
                        inet_ntop(AF_INET, &Clie_Addr.sin_addr,
                         str, sizeof(str)));
                printf("cfd %d --> client %d\n", connfd, ++num);
                tep.events = EPOLLIN;
                tep.data.fd = connfd;
                res = epoll_ctl(efd, EPOLL_CTL_ADD,connfd, &tep);
                if(res == -1){
                    perr_exit("epoll_ctl error");
                }
            } else {
                sockfd = ep[i].data.fd;
                n = Read(sockfd, buf, MAXLINE);
                //消息判断
                if(n == 0){
                    res = epoll_ctl(efd, EPOLL_CTL_DEL, sockfd, NULL);
                    if(res == -1)
                    perr_exit("EPOLL_CTL_DEL error");
                    Close(sockfd);
                    printf("client[%d] close connetion", sockfd);
                }else if(n < 0){
                    perror("read n < 0 error:");
                    res = epoll_ctl(efd, EPOLL_CTL_DEL, sockfd, NULL);
                    if(res == -1)
                    perr_exit("EPOLL_CTL_DEL error");
                    Close(sockfd);
                }else{
                    for(i = 0; i < n; i++){
                        buf[i] = toupper(buf[i]);
                    }
                    Write(STDOUT_FILENO, buf, n);
                    Write(sockfd, buf, n);
                }
            }
        }
    }
    Close(listenfd);
    Close(efd);
    return 0;
}