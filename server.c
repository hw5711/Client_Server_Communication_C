#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#define MAXLINE 1024
void str_echo(int sockfd)
{
    ssize_t n;
    char buf[MAXLINE];
    n = read(sockfd, buf, strlen(buf));
   while(1)
   {
     if(n > 0)
        {
	  printf("clien send: %s\n",buf);
          write(sockfd, buf,strlen(buf));
          memset(buf,0,sizeof(buf));
          exit(0);
        }
    else {
       printf("str_echo: read error");
       exit(0);}
    }
}


int main(int argc, char **argv)
{
    int listenfd, connfd;
    pid_t childpid;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    if(listenfd<0)
        {
            printf("Failed to create socket.");
            exit(1);
        }
    printf("Socket created.\n");
    const int flag = 1;
    int ret;

    ret = setsockopt(listenfd, IPPROTO_TCP, TCP_NODELAY, (const char *)&flag, sizeof(flag));
    if (ret == -1)
	printf("Couldn't setsockopt(TCP_NODELAY)\n");
    else
        printf("Disabled Nagle algorithm.\n");

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(7979);

    if(bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)))
    {
	printf("bind failed");
        exit(1);
    }

    listen(listenfd, 64);
    printf("Listen...\n");
    do
    {
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
	printf("Connected with client.\n");

        if((childpid = fork()) == 0)
        {
            str_echo(connfd);
            exit(0);
        }
        close(connfd);
    }while(1);
}
