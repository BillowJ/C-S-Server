#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <ctype.h>
#include "wrap.h"
#define SERV_PORT 6666

int main(int argc, char *argv[]){

    int i, j, n, maxi;
    int nready;
    int listenfd, connfd, sockfd;
    int client[FD_SETSIZE]  //listenfd是不在里面的，里面只存客户端请求的sockfd
    char buf[BUFSIZ];
    str[INET_ADDRSTRLEN];

    struct sockadd_in Client_Addr, Serv_Addr;
    socklen_t Client_Addr_Len;  //sock结构体的长度
    fd_set rset, allset;

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSERADDR
    ,&opt, sizeof(opt));

    bzero(&Serv_Addr, sizeof(Serv_Addr));
    Serv_Addr.sin_family = AF_INET;
    Serv_Addr.sin_port = htons(SERV_PORT);
    Serv_Addr.sin_addr.s_addr = htonl(INADDR_ANY);

    Bind(listenfd, (struct sockaddr *)&Serv_Addr,
     sizeof(Serv_Addr));

    Liste(listenfd, 128);

    maxfd = listenfd;
    maxi = -1;

    //将存储clientfd的数组全部设成-1
    for(i = 0; i < FD_SETSIZE; i++){
        client[i] = -1;
    }

    //构造select监听描述符的集合，allset里是所以要监听的fd
    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);

    while(1){
        rset = allset;
        nready = select(maxfd + 1, &rset, NULL, NULL, NULL);
        if(nready < 0){
            perr_exit("select error");
        }
        //说明有新的客户端链接请求
        if(FD_ISSET(listenfd, &rset)){
            Client_Addr_Len = sizeof(Client_Addr);
            connfd = Accept(listenfd, (struct sockaddr *)&Client_Addr,
            &Client_Addr_Len);
            printf("received from %s at port %d\n",
            inet_ntop(AF_INET, &Client_Addr.sin_addr, str, sizeof(str),
                      ntohs(Client_Addr.sin_port)));
            for(i = 0; i < FD_SETSIZE; i++){
                if(client[i] < 0){  //将读出的客户端套接字写到数组里
                    client[i] = connfd;
                    break;
                }
            }
            //判断客户端是否超出上限
            if(i == FD_SETSIZE){
                fputs("too many clients\n", stderr);
                exit(1);
            }
            if(i > maxi){
                maxi = i;
            }
            if(--nready == 0){
                continue;
            }
        }

        for(i = 0; i <= maxi; i++){
            if((sockfd = client[i] < 0)){
                continue;
            }

            if(FD_ISSET(sockfd, &rset)){

                //n = 0 读完了
                if((n = Read(sockfd, buf, sizeof(buf))) == 0){
                    Close(sockfd);
                    FD_CLR(sockfd, &allset);    //接触select对当前sockfd的监控
                    client[i] = -1;
                }else if(n > 0){
                    //读出的数据 做出相应处理 然后写数据给客户端
                    for(j = 0; j < n; j++){
                        buf[j] = toupper(buf[j]);
                    }
                    Write(sockfd, buf, n);
                    Write(STDOUT_FILENO, buf, n); //在服务端打印
                }
                if(--nready == 0){
                    break;
                }
            }

        }

    }
    Close(listenfd);
    return 0;
}