#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <pthread.h>
#include "log.h"

int watch_inotify_ents( int fd )
{
	char event_buf[512];
	int ret;
	int evt_pos = 0;
	int evt_size = 0;

/*==========================================================================*/
//	struct inotify_event {
//            __s32        wd; 		/*watch descriptor*/
//            __u32        mask; 	/*watch mask,IN_CREATE,IN_DELETE,IN_OPEN,IN_CLOSE*/
//            __u32        cookie; 	/*cookie to sychronize two events*/
//            __u32        len; 	/*length (including nulls) of name*/
//            char         name[0]; /*stub for possibale name*/
//	};
/*===========================================================================*/
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

void * watch_thread_func(void *arg)
{
	while(1)
	{
		watch_inotify_ents( *(int*)arg );
	}
	return NULL;
}

int main(int argc, char * argv[])
{
	int inotifyFd = 0;
	int ret = 0;
	pthread_t watchThread;

	if(argc != 2)
	{
		XLOGD("Usage:%s {<dir>}\n",argv[0]);
		return -1;
	}

	inotifyFd = inotify_init();
	if( inotifyFd <= 0)
	{
		XLOGD("inotify_init error\n");
		return -1;
	}
	ret = inotify_add_watch(inotifyFd, argv[1], IN_CREATE|IN_DELETE);

	pthread_create(&watchThread, NULL, watch_thread_func, &inotifyFd);
	pthread_join(watchThread, NULL);

	inotify_rm_watch(inotifyFd, ret);

	close(inotifyFd);

	return 0;
}
