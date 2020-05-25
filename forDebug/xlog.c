/*================================================================
 * For log print test by Prudencio
================================================================*/
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdarg.h>
#include <time.h>

#define XLOGD( FORMAT, args... ) \
	xlog( "->%s(%d):"FORMAT, __FUNCTION__, __LINE__, ##args );


void xlog( const char * fmt, ... )
{
    va_list arg;
    char buf[1024];
    int n = 0;
    n= snprintf( buf, 1024, "[%s]","XLOG:" );
    va_start( arg, fmt );
    n += vsnprintf( buf+n, 1024-n, fmt, arg );
    va_end( arg );

    write( 1, buf, n );
}

void print_elapsed_time(void)
{
   static struct timespec start;
   struct timespec curr;
   static int first_call = 1;
   int secs, nsecs;

   if (first_call) {
       first_call = 0;
       if (clock_gettime(CLOCK_MONOTONIC, &start) == -1)
       {
           XLOGD("clock_gettime failed!!\n");
       }
   }

   if (clock_gettime(CLOCK_MONOTONIC, &curr) == -1)
   {
       XLOGD("clock_gettime failed!!\n");
   }

   secs = curr.tv_sec - start.tv_sec;
   nsecs = curr.tv_nsec - start.tv_nsec;
   if (nsecs < 0) {
       secs--;
       nsecs += 1000000000;
   }
   XLOGD("=====>%d.%03d:\n", secs, (nsecs + 500000) / 1000000);
}

int main(int argc, char * argv[])
{
    print_elapsed_time();
    int fd = open( "./logInfo", O_RDWR|O_CREAT, 0766 );
    ftruncate( fd, 0 );
    dup2( fd, 1 );
    close( fd );

    sleep(1);
    int i = 10;
    char * a = "Test";
    char * p = "Hello world!";
    XLOGD( "%d %s %s\n", i, a, p );
    print_elapsed_time();
}
