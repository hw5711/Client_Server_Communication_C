#include "unp.h"
#define UNIX_DOMAIN "/tmp/UNIX.domain"

int recv_fd(int sockfd)
{
	int newfd, nr;
	char buf[2];//
	ssize_t n;
    struct iovec iov[1];
    struct msghdr msg;
    struct cmsghdr *cmptr;
    
    union
    {
      struct cmsghdr cm;
      char control[CMSG_SPACE(sizeof(int))];
     }control_un;

    msg.msg_control = control_un.control;
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
	
    if((nr=recvmsg(sockfd, &msg, 0))<0)
	{
	    printf("recv error");
	    exit(1);
	}
	else
	{
	    newfd = *(int *)CMSG_DATA(cmptr);//newfd to fp
	    printf("Receive file descriptor successfully.\n");
		
       	    /* output content of the test file. but I failed.  */
	    char *buffer;
	    long lSize;
            size_t result;
		
            /* transfer file descriptor to file structure */
	    FILE *fp;
	    fp = fdopen(newfd, "r");
	    printf("transfer file descriptor to file structure\n");
		
	    fseek (fp , 0 , SEEK_END);	      
            lSize = ftell (fp);
       	    rewind (fp);
            buffer = (char*) malloc (sizeof(char)*lSize);
    
            if (buffer == NULL)
            {
            	fputs ("Memory error",stderr); 
            	exit (1);
            }
	}
 
    result = fread (buffer,1,lSize,fp);
    if (result != lSize)
    {
        fputs ("Reading error",stderr);
        exit (1);
        printf("%s", buffer); 
        fclose (fp);
        free (buffer);
  	}
	return 0;
}

void str_cli(FILE *fp, int sockfd)
{
    char sendline[MAXLINE], recvline[MAXLINE];
    printf("Which file you want to open? (type the path)\n");
    while(fgets(sendline, MAXLINE, fp) != NULL)
    {    
        write(sockfd, sendline, strlen(sendline));
        printf("Server is trying to open the file...\n");
    	recv_fd(sockfd);
    	printf("finished\n");
    }
}

int main(int argc, char **argv)
{
    int sfd, fd;
    FILE *fp;
    ssize_t lenth;
    struct sockaddr_un addr;
    sfd = socket(AF_UNIX, SOCK_STREAM, 0); 
    if (sfd == -1)
   	printf ("Failed to create socket");	

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, UNIX_DOMAIN);

    connect(sfd, (struct sockaddr *)&addr, sizeof(addr));
    printf("connected with server.\n");

    str_cli(stdin, sfd);
    exit(0);
}


