#include "timer.h"
#include <cmath>

//-----------------------------------------------------------------------------
// Name : QueryPerformanceFrequency
//-----------------------------------------------------------------------------
bool Timer::getPerformanceFrequency(int64_t* frequency)
{
#ifdef _WIN32
    QueryPerformanceFrequency((LARGE_INTEGER*)frequency);
#else
    // Sanity check.
    assert(frequency != nullptr);
    // gettimeofday reports to microsecond accuracy.
    *frequency = usec_per_sec;
#endif
    return true;
}

//-----------------------------------------------------------------------------
// Name : QueryPerformanceCounter
//-----------------------------------------------------------------------------
bool Timer::getPerformanceCounter(int64_t* performance_count)
{
#ifdef _WIN32
    QueryPerformanceCounter((LARGE_INTEGER*)performance_count);
#else
    struct timeval time;
    // Sanity check.
    assert(performance_count != nullptr);
    // Grab the current time.
    gettimeofday(&time, nullptr);
    *performance_count = time.tv_usec + // Microseconds. 
                         time.tv_sec * usec_per_sec; // Seconds. 
#endif
    return true;
}

//-----------------------------------------------------------------------------
// Name : Timer (constructor)
//-----------------------------------------------------------------------------
Timer::Timer(void)
{
    getPerformanceFrequency(&m_PerfFreq);
    getPerformanceCounter(&m_lastTime);

    m_TimeScale = 1.0f / m_PerfFreq;
    m_avgTimeDelta = 0.0;
    m_SampleCount = 0;

    m_FrameRate	= 0;
    m_FPSFrameCount	= 0;
    m_FPSTimeElapsed = 0.0f;
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
    getPerformanceCounter(&currTime);
    //the current time delta between frames
    double curTimeDelta=(currTime - m_lastTime) * m_TimeScale;
    m_lastTime = currTime;

    for (std::list<alarm>::iterator it = --m_alarms.end(); it != --m_alarms.begin(); --it)
    {
        if (it->fireTime <= currTime)
        {
            std::cout << "fireing callback\n";
            it->callback();
            if (it->repeat > 0)
                it->fireTime = currTime + it->repeat;
            else
                m_alarms.erase(it);
        }
    }
    
    if (!m_cap)
    {
        if ( fabs(curTimeDelta - m_avgTimeDelta) < 1.0f  )
        {
            // Wrap FIFO frame time buffer.
            for (int i = MAX_SAMPLE_COUNT - 1; i > 0; i--)
            {
                m_timeDeltas[i] = m_timeDeltas[i - 1];
            }

            m_timeDeltas[ 0 ] = curTimeDelta;
            if (m_SampleCount < MAX_SAMPLE_COUNT)
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
        //if (m_FPSFrameCount == 600)
            //    m_cap = true;
    }

    m_FPSTimeElapsed += curTimeDelta;
    //std::cout << "TimeElapsed: " << m_FPSTimeElapsed << "\n";

    if ( m_FPSTimeElapsed > 1.0f) 
    {
        m_FrameRate = m_FPSFrameCount;
        m_FPSFrameCount = 0;
        m_FPSTimeElapsed = 0.0f;
        m_cap = false;
    } // End If Second Elapsed
}

//-----------------------------------------------------------------------------
// Name : getTimeElapsed
//-----------------------------------------------------------------------------
double Timer::getTimeElapsed()
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
double Timer::getCurrentTime()
{
    int64_t currTime;
    getPerformanceCounter(&currTime);

    return currTime * m_TimeScale;
}

//-----------------------------------------------------------------------------
// Name : getLastTime
//-----------------------------------------------------------------------------
double Timer::getLastTime()
{
    return m_lastTime * m_TimeScale;
} 

//-----------------------------------------------------------------------------
// Name : isCap
//-----------------------------------------------------------------------------
bool Timer::isCap()
{
    return m_cap;
} 

//-----------------------------------------------------------------------------
// Name : addAlram
//-----------------------------------------------------------------------------
std::list<Timer::alarm>::iterator Timer::addAlram(int64_t duration, const std::function<void (void)>& callback ,int64_t repeated)
{
    int64_t currentTime;
    getPerformanceCounter(&currentTime);
    m_alarms.emplace_back(currentTime, currentTime + duration, callback, repeated);
    return --m_alarms.end();
}

//-----------------------------------------------------------------------------
// Name : addAlram
//-----------------------------------------------------------------------------
std::list<Timer::alarm>::iterator Timer::addAlram(int64_t duration, std::function<void (void)>&& callback ,int64_t repeated)
{
    int64_t currentTime;
    getPerformanceCounter(&currentTime);
    m_alarms.emplace_back(currentTime, currentTime + duration, callback, repeated);
    return --m_alarms.end();
}

//-----------------------------------------------------------------------------
// Name : removeAlarm
//-----------------------------------------------------------------------------
void Timer::removeAlarm(std::list<alarm>::iterator itemIndex)
{
    m_alarms.erase(itemIndex);
}

//-----------------------------------------------------------------------------
// Name : convertTime
//-----------------------------------------------------------------------------
int64_t Timer::convertTime(unsigned int seconds, unsigned int microseconds)
{
    return microseconds + seconds * m_PerfFreq;
}
