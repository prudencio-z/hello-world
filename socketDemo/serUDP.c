#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

int port = 8080;
const char *p = "127.0.0.1";

int main( int argc, char * argv[] )
{
	int ret = 0;
	int sockfd = 0;
	struct sockaddr_in addr;
	struct sockaddr_in cli;
	socklen_t len = 0;
	char buf[1024];

	if( argc == 1 )
	{
		printf("Could input port and IPV4 address\nPlease run like:ser 8080 127.0.0.1\n");
		printf( "Default port:%d, address:%s\n", port, p  );
	}
	else if( argc == 3 )
	{
		port = atoi(argv[1]);
		p = argv[2];
		printf( "Input server port:%d, address:%s\n", port, p  );
	}



	sockfd = socket( AF_INET, SOCK_DGRAM, 0 );

	addr.sin_family = AF_INET;
	addr.sin_port = htons( port );
	addr.sin_addr.s_addr = inet_addr(p);

	ret = bind( sockfd, (struct sockaddr*)&addr, sizeof(addr) );

	if( ret != 0 )
	{
		printf("bind error!\n");	
		return -1;
	}

	len = sizeof(cli);

	while(1)
	{
		recvfrom( sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&cli, &len );
		printf( "server recive %s from %s\n", buf, inet_ntoa(cli.sin_addr) );

		memset(buf, 0, sizeof(buf));
		buf[0] = 0x31;
		buf[1] = 0x32;
		buf[2] = 0x33;
		sendto( sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&cli, len );
		printf( "server send %s over\n", buf);
	}
	close(sockfd);
}
