#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdarg.h>



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
