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

void str_cli( int sockfd)
{
    int nrecv;
    char recvline[MAXLINE];
    char sendline[]="hello";

    printf("send string to server.\n");
    write(sockfd,sendline, strlen(sendline));
        nrecv = read(sockfd, recvline, strlen(recvline));
    if(nrecv >=0){
    	printf("Receive from server:%s\n",recvline);
        memset(recvline,0,sizeof(recvline));
        }
    else{
        printf("receive error.\n");
        exit(0);}
    memset(recvline,0,sizeof(recvline));
}


int main(int argc, char **argv)
{
    int sockfd;
    char buff[MAXLINE];

    struct sockaddr_in cliaddr, servaddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    const int flag = 1;
    int ret;

    ret = setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (const char *)&flag, sizeof(flag));
    if (ret == -1)
	printf("Couldn't setsockopt(TCP_NODELAY)\n");
    else
        printf("Disabled Nagle algorithm.\n");
    //bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(7979);
    while(1){
    if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))<0)
	printf("connect error");
    printf("connected with server.\n");
    str_cli( sockfd);
    exit(0);
    }
    return 0;
}
