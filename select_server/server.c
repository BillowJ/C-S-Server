#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <ctype.h>
#include <arpa/inet.h>

#include "wrap.h"

#define SERV_PORT 6666

int main(int argc, char** argv){
    int i, j,connfd, sockfd, n, listenfd;
    int nready;
    char buf[BUFSIZ];
    char str[BUFSIZ];
    int client[1024];
    struct sockaddr_in Clie_Addr;
    struct sockaddr_in Serv_Addr;
    fd_set rset, allset;
    
    listenfd = Socket(AF_INET,SOCK_STREAM, 0);
    bzero(&Serv_Addr, sizeof(Serv_Addr));
    Serv_Addr.sin_family = AF_INET;
    Serv_Addr.sin_port = htons(SERV_PORT);
    Serv_Addr.sin_addr.s_addr = htonl(INADDR_ANY);

    Bind(listenfd, (struct sockaddr *)&Serv_Addr, sizeof(Serv_Addr));
    Listen(listenfd, 20);
    int maxfd = listenfd;
    int maxi = -1;

    for(int i = 0; i < FD_SETSIZE; i++){
        client[i] = -1;
    }

    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);

    while(1){
        rset = allset;
        nready = select(maxfd+1, &rset, NULL, NULL, NULL);
        if(nready < 0) perr_exit("select error");

        if(FD_ISSET(listenfd, &rset)){
            socklen_t Clie_Addr_len = sizeof(Clie_Addr);
            connfd = Accept(listenfd, (struct sockaddr *)& Clie_Addr, &Clie_Addr_len);
            printf("rece from %s at port %d",
                    inet_ntop(AF_INET,&Clie_Addr.sin_addr, str, sizeof(str)),
                    ntohs(Clie_Addr.sin_port));
            for(i = 0; i < FD_SETSIZE; i++)
                if(client[i] < 0){
                    client[i] = connfd;
                    break;
                }
            if(i == FD_SETSIZE){
                fputs("too many clients\n", stderr);
                exit(1);
            }
            FD_SET(connfd, &allset);
            if(connfd > maxfd)
                maxfd = connfd;
            if(i > maxi)
                maxi = i;
            if(--nready == 0)
                continue;
        }
        for(i = 0; i <= maxi; i++){
            if((sockfd = client[i]) < 0)
                continue;
            if(FD_ISSET(sockfd, &rset)){
                if((n = Read(sockfd, buf, sizeof(buf))) == 0){
                    Close(sockfd);
                    FD_CLR(sockfd, &allset);
                    client[i] = -1;
                }else if(n > 0){
                    for(j = 0; j < n; j++)
                        buf[j] = toupper(buf[j]);
                    //sleep(10);
                    Write(sockfd, buf, n);
                }
                if(--nready == 0)
                    break;
                }
            }
        }
        Close(listenfd);
        return 0;
}


                    






