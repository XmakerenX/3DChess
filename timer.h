#ifndef  _TIMER_H
#define  _TIMER_H

#include <sys/time.h>
#include <inttypes.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <iostream>

const unsigned long MAX_SAMPLE_COUNT	= 50;  // Maximum frame time sample count

// Helpful conversion constants.
static const unsigned usec_per_sec = 1000000;
static const unsigned usec_per_msec = 1000;

// These functions are written to match the win32
// signatures and behavior as closely as possible.
bool  QueryPerformanceFrequency(int64_t* frequency);
/*{
    // Sanity check.
    assert(frequency != nullptr);

    // gettimeofday reports to microsecond accuracy.
    *frequency = usec_per_sec;

    return true;
} */

bool  QueryPerformanceCounter(int64_t* performance_count);
/*{
    struct timeval time;

    // Sanity check.
    assert(performance_count != nullptr);

    // Grab the current time.
    gettimeofday(&time, nullptr);
    *performance_count = time.tv_usec + // Microseconds. 
                         time.tv_sec * usec_per_sec; // Seconds. 

    return true;
} */

class Timer
{
public:
	Timer(void);
	virtual ~Timer(void);

	void                   frameAdvanced   ();
	float                  getTimeElapsed  ();
	float                  getCurrentTime  ();
	float                  getLastTime     ();
	unsigned long          getFPS          ();
        bool                   isCap           ();

private:
	int64_t                m_lastTime;
	int64_t                m_PerfFreq;
	float                  m_TimeScale;
	float                  m_avgTimeDelta;
	float                  m_timeDeltas[MAX_SAMPLE_COUNT];
	unsigned long          m_SampleCount;

	unsigned long          m_FrameRate;       // Stores current framerate
	unsigned long          m_FPSFrameCount;   // Elapsed frames in any given second
	float                  m_FPSTimeElapsed;  // How much time has passed during FPS sample
	bool                   m_cap;

};

#endif  //_TIMER_H
