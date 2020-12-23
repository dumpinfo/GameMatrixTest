#pragma once

#include "Leadwerks.h"

#ifdef _WIN32
namespace win32
{
//#include <windows.h>
#include <mmsystem.h>
}
#else
	#include <time.h>
#endif

namespace Leadwerks
{
    class Time//lua
    {
    public:
        static void Update(const int framePerSecond=60);//lua
        static long GetCurrent();//lua
        static float UPS(const int framesPerSecond=60);//lua
        static long Millisecs();//lua
        static void Reset();
        static void Resume();//lua
        static void Pause();//lua
        static float GetSpeed();//lua
        static void Delay(const int milliseconds);//lua
		static void Step(const int framesPerSecond=60);//lua
    };
    
    extern int stepmode__;
	extern long time__;
	extern float speed__;
	extern long systemtime__;
	extern long timeoffset__;
	extern long pausetime__;
	extern bool timepausestate;
}
