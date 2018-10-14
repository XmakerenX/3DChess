#ifndef  _TIMER_H
#define  _TIMER_H

#ifdef _WIN32
	#include <Windows.h>
	#undef max
	#undef min
#elif
	#include <sys/time.h>
	// Helpful conversion constants.
	static const unsigned usec_per_sec = 1000000;
	static const unsigned usec_per_msec = 1000;
#endif

#include <inttypes.h>
#include <assert.h>
#include <string.h>
#include <iostream>

const unsigned long MAX_SAMPLE_COUNT = 50;  // Maximum frame time sample count

class Timer
{
public:
	Timer(void);
	virtual ~Timer(void);

	// These functions are written to match the win32
	// signatures and behavior as closely as possible.
	static bool  getPerformanceFrequency(int64_t* frequency);
	static bool  getPerformanceCounter(int64_t* performance_count);

	void                   frameAdvanced   ();
    double                 getTimeElapsed  ();
    double                 getCurrentTime  ();
    double                 getLastTime     ();
	unsigned long          getFPS          ();
    bool                   isCap           ();

private:
	int64_t                m_lastTime;
	int64_t                m_PerfFreq;
    double                 m_TimeScale;
    double                 m_avgTimeDelta;
    double                 m_timeDeltas[MAX_SAMPLE_COUNT];
	unsigned long          m_SampleCount;

	unsigned long          m_FrameRate;       // Stores current framerate
	unsigned long          m_FPSFrameCount;   // Elapsed frames in any given second
	float                  m_FPSTimeElapsed;  // How much time has passed during FPS sample
	bool                   m_cap;

};

#endif  //_TIMER_H
