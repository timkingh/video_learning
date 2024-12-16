#include "common_ff.h"

#if _WIN32
#include <sys/types.h>
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif

int64_t time_mdate(void)
{
#if _WIN32
	struct timeb tb;
	ftime(&tb);
	return ((int64_t)tb.time * 1000 + (int64_t)tb.millitm) * 1000;
#else
	struct timeval tv_date;
	gettimeofday(&tv_date, NULL);
	return (int64_t)tv_date.tv_sec * 1000000 + (int64_t)tv_date.tv_usec;
#endif
}