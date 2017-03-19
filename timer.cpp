#include "timer.h"

//-----------------------------------------------------------------------------
// Name : QueryPerformanceFrequency
//-----------------------------------------------------------------------------
bool  QueryPerformanceFrequency(int64_t* frequency)
{
    // Sanity check.
    assert(frequency != nullptr);

    // gettimeofday reports to microsecond accuracy.
    *frequency = usec_per_sec;

    return true;
}

//-----------------------------------------------------------------------------
// Name : QueryPerformanceCounter
//-----------------------------------------------------------------------------
bool  QueryPerformanceCounter(int64_t* performance_count)
{
    struct timeval time;

    // Sanity check.
    assert(performance_count != nullptr);

    // Grab the current time.
    gettimeofday(&time, nullptr);
    *performance_count = time.tv_usec + // Microseconds. 
                         time.tv_sec * usec_per_sec; // Seconds. 

    return true;
}

//-----------------------------------------------------------------------------
// Name : Timer (constructor)
//-----------------------------------------------------------------------------
Timer::Timer(void)
{
	QueryPerformanceFrequency  ( &m_PerfFreq);
	QueryPerformanceCounter    ( &m_lastTime); 
	m_TimeScale		= 1.0f / m_PerfFreq;
	m_avgTimeDelta		= 0.0;
	m_SampleCount		= 0;

	m_FrameRate		= 0;
	m_FPSFrameCount		= 0;
	m_FPSTimeElapsed	= 0.0f;
    m_cap = false;
}

//-----------------------------------------------------------------------------
// Name : Timer (destructor)
//-----------------------------------------------------------------------------
Timer::~Timer(void)
{
}

//-----------------------------------------------------------------------------
// Name : frameAdvanced
// Desc : signals that a frame has advanced and there is a need to recalculate the time delta and the FPS
//-----------------------------------------------------------------------------
void Timer::frameAdvanced()
{
	int64_t currTime;

	QueryPerformanceCounter( &currTime);
	float curTimeDelta=(currTime - m_lastTime) * m_TimeScale;//the current time delta between frames
	m_lastTime = currTime;

        if (!m_cap)
        {
            if ( fabsf(curTimeDelta - m_avgTimeDelta) < 1.0f  )
            {
                    // Wrap FIFO frame time buffer.
                    memmove( &m_timeDeltas[1], m_timeDeltas, (MAX_SAMPLE_COUNT - 1) * sizeof(float) );
                    m_timeDeltas[ 0 ] = curTimeDelta;
                    if ( m_SampleCount < MAX_SAMPLE_COUNT ) 
                            m_SampleCount++;

            } // End if

            // Count up the new average elapsed time
            m_avgTimeDelta = 0.0f;

            for ( unsigned long i = 0; i < m_SampleCount; i++ )
                    m_avgTimeDelta += m_timeDeltas[ i ];

            if ( m_SampleCount > 0 )
                    m_avgTimeDelta /= m_SampleCount;

            // Calculate Frame Rate
            m_FPSFrameCount++;
            //std::cout << "m_FPSFrameCount++ \n";
           //if (m_FPSFrameCount == 600)
           //    m_cap = true;
        }
        //m_FPSFrameCount++;
        m_FPSTimeElapsed += curTimeDelta;
        //std::cout << "TimeElapsed: " << m_FPSTimeElapsed << "\n";

	if ( m_FPSTimeElapsed > 1.0f) 
	{
        m_FrameRate		= m_FPSFrameCount;
        //std::cout << "frameCount: " << m_FPSFrameCount << "\n";
		m_FPSFrameCount		= 0;
		m_FPSTimeElapsed	= 0.0f;
        m_cap = false;
	} // End If Second Elapsed

}

//-----------------------------------------------------------------------------
// Name : getTimeElapsed
//-----------------------------------------------------------------------------
float Timer::getTimeElapsed()
{
	return m_avgTimeDelta;
}

//-----------------------------------------------------------------------------
// Name : getFPS
//-----------------------------------------------------------------------------
unsigned long Timer::getFPS()
{
	return m_FrameRate;
}

//-----------------------------------------------------------------------------
// Name : getCurrentTime
//-----------------------------------------------------------------------------
float Timer::getCurrentTime()
{
	int64_t currTime;

	QueryPerformanceCounter( &currTime);

	return currTime * m_TimeScale;
}

//-----------------------------------------------------------------------------
// Name : getLastTime
//-----------------------------------------------------------------------------
float Timer::getLastTime()
{
	return m_lastTime * m_TimeScale;
} 

bool Timer::isCap()
{
	return m_cap;
} 
