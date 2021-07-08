#pragma once

#include <ctime>
#include <string>

#ifdef _WIN32
#include <windows.h>

#define sleep_ms(x) Sleep(x)
#else
#include <unistd.h>
#include <cstdint> //for uintptr_t

// Use standard Posix macro for Linux
#if (defined(linux) || defined(__linux)) && !defined(__linux__)
#define __linux__
#endif
#if (defined(__linux__) || defined(__GNU__)) && !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif

#define sleep_ms(x) usleep(x*1000)
#endif

#if defined(__APPLE__)
#include <mach-o/dyld.h>
#include <CoreFoundation/CoreFoundation.h>
#endif

#ifndef _WIN32 // Posix
#include <sys/time.h>
#include <ctime>

#if defined(__MACH__) && defined(__APPLE__)
#include <mach/clock.h>
#include <mach/mach.h>
#endif
#endif


enum TimePrecision
{
	PRECISION_SECONDS,
	PRECISION_MILLI,
	PRECISION_MICRO,
	PRECISION_NANO
};

inline std::string getTimestamp()
{
	time_t t = time(NULL);
	// This is not really thread-safe but it won't break anything
	// except its own output, so just go with it.
	struct tm *tm = localtime(&t);
	char cs[20]; // YYYY-MM-DD HH:MM:SS + '\0'
	strftime(cs, 20, "%Y-%m-%d %H:%M:%S", tm);
	return cs;
}

#ifdef _WIN32 // Windows

extern double perf_freq;

inline uint64_t os_get_time(double mult)
{
	LARGE_INTEGER t;
	QueryPerformanceCounter(&t);
	return static_cast<double>(t.QuadPart) / (perf_freq / mult);
}

// Resolution is <1us.
inline uint64_t getTimeS() { return os_get_time(1); }
inline uint64_t getTimeMs() { return os_get_time(1000); }
inline uint64_t getTimeUs() { return os_get_time(1000 * 1000); }
inline uint64_t getTimeNs() { return os_get_time(1000 * 1000 * 1000); }

#else // Posix

inline void os_get_clock(struct timespec *ts)
{
#if defined(__MACH__) && defined(__APPLE__)
	// From http://stackoverflow.com/questions/5167269/clock-gettime-alternative-in-mac-os-x
	// OS X does not have clock_gettime, use clock_get_time
	clock_serv_t cclock;
	mach_timespec_t mts;
	host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
	clock_get_time(cclock, &mts);
	mach_port_deallocate(mach_task_self(), cclock);
	ts->tv_sec = mts.tv_sec;
	ts->tv_nsec = mts.tv_nsec;
#elif defined(CLOCK_MONOTONIC_RAW)
	clock_gettime(CLOCK_MONOTONIC_RAW, ts);
#elif defined(_POSIX_MONOTONIC_CLOCK)
	clock_gettime(CLOCK_MONOTONIC, ts);
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	TIMEVAL_TO_TIMESPEC(&tv, ts);
#endif
}

inline uint64_t getTimeS()
{
	struct timespec ts;
	os_get_clock(&ts);
	return ts.tv_sec;
}

inline uint64_t getTimeMs()
{
	struct timespec ts;
	os_get_clock(&ts);
	return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

inline uint64_t getTimeUs()
{
	struct timespec ts;
	os_get_clock(&ts);
	return ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
}

inline uint64_t getTimeNs()
{
	struct timespec ts;
	os_get_clock(&ts);
	return ts.tv_sec * 1000000000 + ts.tv_nsec;
}

#endif

uint64_t getTime(TimePrecision prec);
