#ifndef _XLOG_H
#define _XLOG_H

#define XLOGD( FORMAT, args... ) \
	xlog( "->%s(%d):"FORMAT, __FUNCTION__, __LINE__, ##args );
void xlog(const char * fmt, ...);

#endif
