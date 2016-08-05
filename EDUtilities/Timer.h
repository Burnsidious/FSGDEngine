#ifndef _ED_TIMER_H_
#define _ED_TIMER_H_
#pragma once


#define WIN32_LEAN_AND_MEAN
#include "windows.h"

#include "InternalOutput.h"

namespace EDUtilities
{
    class Timer
    {
    private:
        LARGE_INTEGER lastTime;

        LARGE_INTEGER frequency;

    public:

        Timer(void)
        {
            QueryPerformanceFrequency(&frequency);
        }

        void Reset(void)
        {
            QueryPerformanceCounter(&lastTime);
        }

        float GetElapsedTime(void)
        {
            LARGE_INTEGER curTime;
            QueryPerformanceCounter(&curTime);
            return (float)(curTime.QuadPart - lastTime.QuadPart)/frequency.QuadPart;
        }
    };
}
#endif