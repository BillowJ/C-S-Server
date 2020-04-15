#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <strings.h>
#include "wrap.h"

#define SERV_PORT 8880

int main(void){
    int lfd, cfd;
    pid_t pid;
    struct sockaddr_in serv_addr, clie_addr;
    socklen_t clie_addr_len;
    lfd = socket(AF_INET, SOCK_STREAM, 0);
    char buf[BUFSIZ];
    int i, n;

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    //inet_pton(AF_INET, "128.123.123.123", serv_addr.sin_addr.s_addr));
    Bind(lfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr));

    Listen(lfd, 128);
    while(1){   
        clie_addr_len = sizeof(clie_addr);
        cfd = Accept(lfd, (struct sockaddr *)&clie_addr, &clie_addr_len);
        
        pid = fork();
        if(pid < 0){
            perror("fork");
            exit(1);
        }else if (pid == 0){
            close(lfd);//子线程 先关闭父进程的lfd
            break;
        } else{
            close(cfd);
        }
    }
    if(pid == 0){
        while(1){
            n = read(cfd, buf, sizeof(buf));
            if(n == 0){ //client close
                close(cfd);
                return 0;
            }else if(n == -1){
                perror("read error");
                exit(1);
            }else{
                for(i = 0; i < n; i++){
                    buf[i] = toupper(buf[i]);
                    write(cfd, buf, n);
                }
            }
            }
        }

    return 0;
}
