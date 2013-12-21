#include "nano_timer.h"

#include <time.h>
#include <sys/time.h>
#include <assert.h>

double nano_time_d(void)
{
    struct timespec ts;
#ifndef NDEBUG
    int h = clock_gettime(CLOCK_MONOTONIC, &ts);
    assert(!h);
#else
    clock_gettime(CLOCK_MONOTONIC, &ts);
#endif
    //int h = clock_gettime(CLOCK_REALTIME, &ts);
    return (double)ts.tv_sec+(double)ts.tv_nsec*1e-9;
}

long long nano_time_i(void)
{
    struct timespec ts;
#ifndef NDEBUG
    int h = clock_gettime(CLOCK_MONOTONIC, &ts);
    assert(!h);
#else
    clock_gettime(CLOCK_MONOTONIC, &ts);
#endif
    //int h = clock_gettime(CLOCK_REALTIME, &ts);
    assert(!h);
    return (long long)ts.tv_sec*1000000000+ts.tv_nsec;
}

double milli_time_d(void)
{
    struct timeval tv;
#ifndef NDEBUG
    int h = gettimeofday(&tv, NULL);
    assert(!h);
#else
    gettimeofday(&tv, NULL);
#endif
    return (double)tv.tv_sec+(double)tv.tv_usec*1e-6;
}

long long milli_time_i(void)
{
    struct timeval tv;
#ifndef NDEBUG
    int h = gettimeofday(&tv, NULL);
    assert(!h);
#else
    gettimeofday(&tv, NULL);
#endif
    return (long long)tv.tv_sec*1000000+tv.tv_usec;
}
