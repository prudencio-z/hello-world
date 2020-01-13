#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <string.h>

#define READ_WIRTE_BUF_SIZE 1024
char * sockSerPath = "server.socket";

int main(int argc, char *argv[])
{
	struct sockaddr_un serUn,cliUn;
	int sockFd = 0;
	char buf[READ_WIRTE_BUF_SIZE];

	if( argc != 2 )
	{
		printf("usage:%s [client name]\n",argv[0]);
		return 0;
	}

	sockFd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(sockFd < 0)
	{
		printf("socket error\n");
		return -1;
	}

	memset(&cliUn, 0, sizeof(cliUn));
	cliUn.sun_family = AF_UNIX;
	strcpy(cliUn.sun_path, argv[1]);
	unlink(argv[1]);
	/*Generally call bind explicitly so that the server distinguishes between different clients*/
	if(bind(sockFd, (struct sockaddr *)&cliUn, sizeof(cliUn)) < 0 )
	{
		printf("client bind error\n");
		return -1;
	}

	memset(&serUn, 0, sizeof(serUn));
	serUn.sun_family = AF_UNIX;
	strcpy(serUn.sun_path, sockSerPath);
	if( connect(sockFd, (struct sockaddr *)&serUn, sizeof(serUn)) < 0 )
	{
		printf("client connect error\n");
		return -1;
	}

	while(fgets(buf, READ_WIRTE_BUF_SIZE, stdin) != NULL)
	{
		write(sockFd, buf, READ_WIRTE_BUF_SIZE);
		read(sockFd, buf, sizeof(buf));
		printf("client get:%s from server\n",buf);
	}
	return 1;
}
