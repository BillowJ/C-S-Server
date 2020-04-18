#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <strings.h> //bzero
#include "wrap.h"
#define SERV_IP "127.0.0.1"
#define SERV_PORT 6666

int main(int argc, char** argv){
    int lfd, cfd;
    struct sockaddr_in serv_addr, clie_addr;
    socklen_t clie_addr_len, clie_ip_len;
    
    char buf[BUFSIZ];
    char clie_ip[BUFSIZ];
    int i, n;
    lfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&serv_addr, sizeof(serv_addr)); //memset
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
   
    Bind(lfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    Listen(lfd, 128); 
    
    clie_addr_len = sizeof(clie_addr);
    cfd = Accept(lfd, (struct sockaddr*)&clie_addr, &clie_addr_len);
    printf("cfd : %d\n", cfd);

    printf("client IP: %s, client port: %d\n",
            inet_ntop(AF_INET, &clie_addr.sin_addr.s_addr, clie_ip, sizeof(clie_ip)),
            ntohs(clie_addr.sin_port));
    
    while(1){
        n = Read(cfd, buf, sizeof(buf));
        for(i = 0; i < n; i++){
            buf[i] = toupper(buf[i]);
        }
        Write(cfd, buf, n);
    }
    close(lfd);
    close(cfd);

}

