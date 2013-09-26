#include <sys/time.h>
#include <time.h>


typedef struct{
	struct timeval start;
	struct timeval end;
}timer_et;

timer_et timer;

void get_start_time_secs(timer_t *secs)
{
	*secs = (timer_t)timer.start.tv_sec;
}

void get_start_time_usecs(timer_t *usecs)
{
	*usecs = (timer_t)timer.start.tv_usec;
}

void utimediff(double *et)
{
	*et = (double)(timer.end.tv_usec - timer.start.tv_usec);
}

void timediff(double *et)
{
	*et = (double)(timer.end.tv_sec - timer.start.tv_sec);
}

int start_timer()
{
	gettimeofday(&timer.start, NULL);
	return 0;
}

int stop_timer()
{
	gettimeofday(&timer.end, NULL);
	return 0;
}
