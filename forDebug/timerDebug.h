#include<sys/time.h>
#include<stdio.h>

#define DEBUG_TIME_BEGIN()\
{\
	struct timeval _start = {0,0}, _cur = {0,0};\
	struct timeval _result = {0,0};\
	float _tmp;\
	printf("=====DEBUG TIME START=====\n");

#define DEBUG_TIME_DUR_BEGIN()\
	gettimeofday( &_start, NULL);

#define DEBUG_TIME_DUR_END()\
	gettimeofday( &_cur, NULL);\
	timersub(&_cur, &_start, &_result);\
	{\
		_tmp = _result.tv_sec+(float)_result.tv_usec/1000000;\
		printf("=====From DUR_BEGIN to DUR_END:%03fs=====\n", _tmp);\
	}

#define DEBUG_TIME_END()\
	printf("=====DEBUG TIME END=====\n");\
}
