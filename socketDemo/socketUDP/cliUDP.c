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
	int sockfd = 0;
	struct sockaddr_in addr;
	struct sockaddr_in src;
	socklen_t len = 0;
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


	sockfd = socket( AF_INET, SOCK_DGRAM, 0 );

	addr.sin_family = AF_INET;
	addr.sin_port = htons( port );
	addr.sin_addr.s_addr = inet_addr(p);

	len = sizeof( addr);	
	memcpy(buf, "test", 5);
	sendto( sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&addr, len );
	printf( "client send %s over\n", buf);

	len = sizeof( src );
	recvfrom( sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&src, &len );
	printf( "client recive %s from %s\n", buf, inet_ntoa(addr.sin_addr) );

	close(sockfd);
}
