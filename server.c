#include "unp.h"
#define UNIX_DOMAIN "/tmp/UNIX.domain

void send_fd(int sfd, int fd)
{
    char buf[2] = {0};
    struct iovec iov[1];
	struct msghdr msg;
    struct cmsghdr *cmptr= NULL;
    union
    {
      struct cmsghdr cm;
      char control[CMSG_SPACE(sizeof(int))];
     }control_un;

    msg.msg_control = control_un.control;//
    msg.msg_controllen = sizeof(control_un.control);
    cmptr = CMSG_FIRSTHDR(&msg);
    cmptr->cmsg_len = CMSG_LEN(sizeof(int)); 
    cmptr->cmsg_level = SOL_SOCKET;
    cmptr->cmsg_type = SCM_RIGHTS;
      
    iov[0].iov_base = buf;
    iov[0].iov_len = 2;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;
    msg.msg_name = NULL;         
    msg.msg_namelen = 0;    
    *(int *)CMSG_DATA(cmptr) = fd;

    if(sendmsg(sfd,&msg,0)== -1)
      printf("send error");   
	exit(0);
}

void str_echo(int sockfd)
{
    ssize_t n;
    char buf[MAXLINE];
    n = read(sockfd, buf, MAXLINE);

    if(n>0)
    {
        printf("Received file path is %s\n",buf);
		/* open the file with the file path sent by client */
		int fd = open(buf,O_RDONLY|O_CREAT,S_IRWXO);
	    
	    if(fd == -1)
	    {
	        printf("file is not exist.\n");
            exit(1);
	    }
	    else
	    {
	        printf("file was opened.\n");              
		printf("send the file descriptor to the client.\n");
		send_fd(sockfd,fd);
	        close(fd); 
        }
    }
    else 
        printf("str_echo: read error");
    exit(1);
}


int main(int argc, char **argv)
{
    ssize_t nbytes;
    int sfd, cfd, *fds;
    pid_t childpid;
    char buf[MAXLINE];
    int fd;
    struct sockaddr_un addr, cliaddr;
    sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1)
    printf ("Failed to create socket");

    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, UNIX_DOMAIN,sizeof(addr.sun_path)-1);
    unlink (UNIX_DOMAIN);			
				
    if (bind(sfd, (struct sockaddr *) &addr, sizeof(addr)) == -1)
        printf ("Failed to bind to socket");
    if (listen(sfd, 5) == -1)
        printf ("Failed to listen on socket");
    printf("listening...\n");
    do
    {
        int clilen = sizeof(cliaddr);
        int connfd = accept(sfd, (struct sockaddr *)&cliaddr, &clilen);
	    printf("Connected with client.\n");

        if((childpid = fork()) == 0)
        {
            str_echo(connfd);
            exit(0);
        }
        close(connfd);
    }while(1);
}
