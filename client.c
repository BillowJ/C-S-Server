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
#include "wrap.h"

#define SERV_PORT 6666
#define SERV_IP "127.0.0.1"

int main(int argc, char* argv[]){
    int cfd;
    int n;
    //uint16_t port =(uint16_t)*argv[1];
    struct sockaddr_in serv_addr;  //server's port ip
    cfd = Socket(AF_INET, SOCK_STREAM, 0);
    char buf[BUFSIZ];

    memset(&serv_addr, 0, sizeof(serv_addr));
      
    //addr.sin_addr.s_addr = htonl(INADDR_ANY);
    inet_pton(AF_INET, SERV_IP, &serv_addr.sin_addr.s_addr);
    serv_addr.sin_port = htons(SERV_PORT);
    //serv_addr.sin_port = htons(port);
    serv_addr.sin_family = AF_INET;

    while(1){
        Connect(cfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
        fgets(buf, sizeof(buf), stdin); //"hello \n\0"
        Write(cfd, buf, strlen(buf));
        n = Read(cfd, buf, sizeof(buf));
        Write(STDOUT_FILENO, buf, n);
    }
    close(cfd);
    return 0;
}
