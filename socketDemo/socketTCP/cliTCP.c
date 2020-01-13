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
	char buf[1024];

	if( argc == 1 )
	{
		printf("Could input port and IPV4 address\nPlease run like:cli 8080 127.0.0.1\n");
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

	ret = connect( sockfd, (struct sockaddr*)&addr, sizeof( addr) );
	if(ret < 0)
	{
		printf( "client connect error\n" );	
		return -1;
	}
	printf("client connect ret:%d\n",ret);

	send( sockfd, "test", 5, 0 );
	printf( "client send test\n" );

	recv(sockfd, buf, sizeof(buf), 0 );
	printf( "client receive:%s\n", buf );

	close(sockfd);
	return 0;
}
