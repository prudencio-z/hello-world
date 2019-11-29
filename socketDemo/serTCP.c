#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

int port = 8080;
//const char *p = "192.168.56.101";
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

	sockfd = socket( AF_INET, SOCK_STREAM, 0 );

	addr.sin_family = AF_INET;
	addr.sin_port = htons( port );
	addr.sin_addr.s_addr = inet_addr(p);

	ret = bind( sockfd, (struct sockaddr*)&addr, sizeof(addr) );
	if( ret != 0 )
	{
		printf("bind error!\n");	
		return -1;
	}

	listen(sockfd, 5);

	len = sizeof(cli);
	while(1)
	{
		int sock = accept( sockfd, (struct sockaddr*)&cli, &len );	
		printf( "server accept sock:%d from %s\n", sock, inet_ntoa(cli.sin_addr) );

		recv( sock, buf, sizeof(buf), 0);
		printf( "server receive:%s\n", buf );

		send(sock, "123", 4, 0);
		printf( "server send 123\n" );
	}
	close(sockfd);
}
