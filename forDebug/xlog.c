/*================================================================
 * For log print test by Prudencio
================================================================*/
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/time.h>

#define XLOG_BUFFER_SIZE        1024


#define XLOGD( FORMAT, args... ) \
	xlog( "->%s(%d):"FORMAT, __FUNCTION__, __LINE__, ##args );


void xlog( const char * fmt, ... )
{
    va_list arg;
    char buf[XLOG_BUFFER_SIZE];

    struct timeval _t = { 0, 0 };
    gettimeofday( &_t, NULL );

    int n = 0;
    n= snprintf( buf, XLOG_BUFFER_SIZE, "XLOG[%ld.%ld]", _t.tv_sec, _t.tv_usec/1000 );
    va_start( arg, fmt );
    n += vsnprintf( buf+n, XLOG_BUFFER_SIZE-n, fmt, arg );
    va_end( arg );

    n += snprintf( buf+n, XLOG_BUFFER_SIZE-n, "\n" );

    write( 1, buf, n );
}

int main(int argc, char * argv[])
{
    int fd = open( "./logInfo", O_RDWR|O_CREAT );
    ftruncate( fd, 0 );
    dup2( fd, 1 );
    close( fd );

    int i = 10;
    char * a = "Test";
    char * p = "Hello world!";
    XLOGD( "%d %s %s\n", i, a, p );
}
