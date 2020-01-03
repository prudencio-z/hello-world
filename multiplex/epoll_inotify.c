#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <sys/epoll.h>
#include <pthread.h>
#include "log.h"

/*==========================================================================*/
//	struct inotify_event {													//
//            __s32  wd; 		/*watch descriptor*/						//
//            __u32  mask; /*IN_CREATE,IN_DELETE,IN_OPEN,IN_CLOSE*/         //
//            __u32  cookie; 	/*cookie to sychronize two events*/         //
//            __u32  len; 	/*length (including nulls) of name*/            //
//            char   name[0]; /*stub for possibale name*/                   //
//	};                                                                      //
/*==========================================================================*/

/*==========================================================================*/
//																			//
//	typedef union epoll_data {                                              //
//	    void *ptr;                                                          //
//	    int fd;                                                             //
//	    __uint32_t u32;                                                     //
//	    __uint64_t u64;                                                     //
//	} epoll_data_t;                                                         //
//	                                                                        //
//	struct epoll_event {                                                    //
//	    __uint32_t events; /* Epoll events */                               //
//	    epoll_data_t data; /* User data variable */                         //
//	};                                                                      //
/*==========================================================================*/

#define EPOLL_FD_MAX_COUNT 	10

typedef struct _stFD_Name
{
	int fd;
	char * name;
}stFD_Name;

static stFD_Name fdList[EPOLL_FD_MAX_COUNT] = 
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

int watch_inotify_ents( int fd )
{
	char event_buf[512];
	int ret;
	int evt_pos = 0;
	int evt_size = 0;

	struct inotify_event *evt;

	ret = read(fd, event_buf, sizeof(event_buf));

	if( ret < (int)sizeof(struct inotify_event) )
	{
		XLOGD("Couldn't get event\n");
		return -1;
	}

	while(ret > (int)sizeof(struct inotify_event) )
	{
		evt = (struct inotify_event*)(event_buf + evt_pos);
		if(evt->len)
		{
			if(evt->mask & IN_CREATE)
			{
				XLOGD("Create file %s successfully\n", evt->name);
			}
			else if(evt->mask & IN_DELETE)
			{
				XLOGD("Delete file %s successfully\n", evt->name);
			}
			else
			{
				XLOGD("Watch file %s successfully\n", evt->name);
			}
		}

		/*The actualy size of this event*/
		evt_size = sizeof(struct inotify_event) + evt->len;
		ret -= evt_size;
		evt_pos += evt_size;
	}
	return 0;
}


int main(int argc, char * argv[])
{
	int inotifyFdBuf[EPOLL_FD_MAX_COUNT] = {0};
	int ret = 0;

	int epollFd = 0;
	struct epoll_event eventItem;
	struct epoll_event pendingEventItems[EPOLL_FD_MAX_COUNT];
	
	if(argc < 2)
	{
		XLOGD("Usage:%s {<dir>} [<dir>...]\n",argv[0]);
		return -1;
	}

	epollFd = epoll_create(EPOLL_FD_MAX_COUNT);
	if( epollFd <= 0 )
	{
		XLOGD("epoll_create error\n");
		return -1;
	}

	for(int i = 1; i < argc; i++)
	{
		inotifyFdBuf[i-1] = inotify_init();
		ret = inotify_add_watch(inotifyFdBuf[i-1], argv[i], IN_CREATE|IN_DELETE);

		eventItem.events = EPOLLIN;/*fd could be read*/
		eventItem.data.fd = inotifyFdBuf[i-1];
		epoll_ctl(epollFd, EPOLL_CTL_ADD, inotifyFdBuf[i-1], &eventItem);
		fdList[i-1].fd = inotifyFdBuf[i-1];
		fdList[i-1].name = argv[i];
	}

	while(1)
	{
		
		int pollFdCount = epoll_wait(epollFd, pendingEventItems, EPOLL_FD_MAX_COUNT, -1);
		for(int i = 0; i < pollFdCount; i++ )
		{
			if( pendingEventItems[i].events & EPOLLIN )
			{
				char * name = get_fd_name(pendingEventItems[i].data.fd);
				if( name != NULL )
				{
					printf("===%s===:\n",name);
				}
				watch_inotify_ents(pendingEventItems[i].data.fd);
			}
		}
	}

	for(int i = 1; i < argc; i++)
	{
		inotify_rm_watch(inotifyFdBuf[i-1], ret);
		close(inotifyFdBuf[i-1]);
	}


	return 0;
}
