#include <sys/time.h>
#include <time.h>


typedef struct{
	struct timeval start;
	struct timeval end;
}timer_et;

timer_et timer;

double timediff()
{
	double et;
	et = (timer.end.tv_sec - timer.start.tv_sec) * 1000.0;      // sec to ms
	et += (timer.end.tv_usec - timer.start.tv_usec) / 1000.0;   // us to ms
	return et;
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
