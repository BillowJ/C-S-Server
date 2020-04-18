#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <errno.h>
#include <ctype.h>

#include "wrap.h"

#define MAXLINE 80
#define SERV_PORT 8000
#define OPEN_MAX 1024

int main(int argc, char *argv[]){

    int nready;
    int i, maxi;
    int listenfd, connfd, sockfd;

    struct pollfd client[OPEN_MAX];
    struct sockaddr_in Serv_Addr, Clie_Addr;
    socklen_t Clie_Addr_Len;

    char buf[BUFSIZ], str[INET_ADDRSTRLEN];

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    
    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    bzero(&Serv_Addr, sizeof(Serv_Addr));
    Serv_Addr.sin_family = AF_INET;
    Serv_Addr.sin_port = htons(SERV_PORT);
    Serv_Addr.sin_addr.s_addr = htonl(INADDR_ANY);

    Bind(listenfd, (struct sockadd *)&Serv_Addr, sizeof(Serv_Addr));
    Listen(listenfd, 128);

    client[0].fd = listenfd;
    client[0].events = POLLIN //监听普通读事件

    for(i = 1; i < OPEN_MAX; i++){
        client[i].fd = -1;
    }

    maxi = 0; //下标

    for( ; ; ){
        nready = poll(client, maxi + 1, -1); //timeout = -1 ：阻塞等待 maxi + 1是监听数
        //POLLIN | POLLOUT | POLLERR
        if(client[0].revents & POLLIN){ //监听客户端请求Accept不阻塞
            Clie_Addr_Len = sizeof(Clie_Addr)
            connfd = Accept(listenfd, (struct sockadd *)&Clie_Addr, &Clie_Addr_Len);
            
            printf("received from %s at port %d",
                    inet_ntop(AF_INET, &Clie_Addr.sin_addr, str, sizeof(str)),
                    ntohs(Clie_Addr.sin_port));

            for(i = 1; i < OPEN_MAX; i++){
                if(client[i].fd < 0) {
                    client[i].fd = connfd;
                    break;   
                }
            }

            if( i == OPEN_MAX){
                perr_exit("too many clients");
            }

            if(i > maxi){
                maxi = i;
            }
            if(--nready <= 0){
                continue;
            }
        }

        for(i = 1; i < maxi; i++){
            if((sockfd = client[i].fd) < 0){
                continue;
            }
            
            if(client[i].revents & POLLIN){

                if((n = Read(sockfd, buf, BUFSIZ)) < 0){
                //错误处理
                    if(errno == ECONNRESET){ /*收到RST*/
                        printf("client[%d] aborted connetion\n", i);
                        Close(sockfd);
                        client[i].fd = -1;
                    }else{
                        perr_exit("read error");
                    }
                }else if(n == 0){   //正常退出
                    printf("client[%d] closed connetion", i);
                    Close(sockfd);
                    client[i].fd = -1;
                }else{
                    //处理读出的数据 并写回要求数据
                    for(j = 0; j < n; j++){
                        buf[j] = toupper(buf[j]);
                    }
                    Writen(sockfd, buf, n);
                }

                if(--nready <= 0){
                    break;
                }
            }
        }

    }
    return 0;
}