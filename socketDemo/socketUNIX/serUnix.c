#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <string.h>

#include <sys/epoll.h>
#include <sys/inotify.h>
#include <pthread.h>

#define EPOLL_FD_MAX_COUNT 	10
#define READ_WIRTE_BUF_SIZE 1024
char * sockPath = "server.socket";

typedef struct _stFdName
{
	int fd;
	char name[100];
}stFdName;

static stFdName fdList[EPOLL_FD_MAX_COUNT] = 
{
};

char * get_fd_name(int fd)
{
	int i = 0;
	for(i = 0; i < EPOLL_FD_MAX_COUNT; i++ )
	{
		if( fdList[i].fd == fd )
		{
			break;
		}
	}
	if( i < EPOLL_FD_MAX_COUNT)
	{
		return fdList[i].name;
	}
	else
	{
		return NULL;
	}
}


void * epoll_socket_thread(void * data)
{
	char buf[READ_WIRTE_BUF_SIZE];
	int evtCount = 0;
	int epollFd = *((int *)data);
	struct epoll_event pendingEventItems[EPOLL_FD_MAX_COUNT];

	printf("thread to check socket ...\n");

	while(1)
	{
		evtCount = epoll_wait( epollFd, pendingEventItems, EPOLL_FD_MAX_COUNT, -1 );
		for(int i = 0; i<evtCount; i++)
		{
			int tmpFd = pendingEventItems[i].data.fd;
			if(pendingEventItems[i].events & EPOLLIN)
			{
				char * name = get_fd_name(tmpFd);
				if(name != NULL)
				{
					printf("===%s===\n",name);
				}
				int n = read(tmpFd, buf, sizeof(buf));
				if(n > 0)
				{
					printf("%s\n",buf);
					write(tmpFd, "OK", 3);
				}
				else
				{
					epoll_ctl(epollFd, EPOLL_CTL_DEL, tmpFd, NULL);
					printf("Connection may disconnected\n");
				}
			}
		}
	}
}

int main(int argc, char *argv[])
{
	struct sockaddr_un cliUn;
	struct sockaddr_un serUn;
	socklen_t len = 0;
	int listenFd, connFd, size;

	pthread_t epollThread;
	int epollFd = 0;
	struct epoll_event eventItem;

	epollFd = epoll_create( EPOLL_FD_MAX_COUNT ); 

	listenFd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(listenFd < 0)
	{
		printf("socket error\n");
		return -1;
	}
	
	size = sizeof(serUn);
	memset(&serUn, 0, size);
	serUn.sun_family = AF_UNIX;
	strcpy(serUn.sun_path, sockPath);
	unlink(sockPath);

	if( (bind(listenFd, (struct sockaddr *)&serUn, size)) < 0 )
	{
		printf("bind error\n");
		return -1;
	}

	listen(listenFd, 5); 
	printf("UNIX socket server listening ...\n");

	/*thread for watch socket which to read*/
	pthread_create( &epollThread, NULL, epoll_socket_thread, &epollFd);

	size = sizeof(cliUn);
	while(1)
	{
		static int i = 0;
		connFd = accept(listenFd, (struct sockaddr *)&cliUn, &size );
		printf("server accept %s\n", cliUn.sun_path);

		eventItem.events = EPOLLIN;	/*fd could be read*/
		eventItem.data.fd = connFd;
		epoll_ctl(epollFd, EPOLL_CTL_ADD, connFd, &eventItem);

		fdList[i].fd = connFd;
		strcpy(fdList[i].name, cliUn.sun_path);
		if(i < EPOLL_FD_MAX_COUNT)
		{
			i++;
		}
	}
	pthread_join(epollThread, NULL);

	printf("==>exit\n");
	return 0;
}
