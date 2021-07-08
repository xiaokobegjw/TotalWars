#include "time_utils.h"
#include "debug.h"

#ifdef _WIN32

inline double get_perf_freq()
{
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	return freq.QuadPart;
}

double perf_freq = get_perf_freq();

#endif

uint64_t getTime(TimePrecision prec)
{
	switch (prec) {
	case PRECISION_SECONDS: return getTimeS();
	case PRECISION_MILLI:   return getTimeMs();
	case PRECISION_MICRO:   return getTimeUs();
	case PRECISION_NANO:    return getTimeNs();
	}
	FATAL_ERROR("Called getTime with invalid time precision");
}