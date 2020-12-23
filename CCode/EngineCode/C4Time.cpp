 

#include "C4Time.h"


using namespace C4;


TimeMgr *C4::TheTimeMgr = nullptr;


namespace C4
{
	template <> TimeMgr Manager<TimeMgr>::managerObject(0);
	template <> TimeMgr **Manager<TimeMgr>::managerPointer = &TheTimeMgr;

	template <> const char *const Manager<TimeMgr>::resultString[] =
	{
		nullptr
	};

	template <> const unsigned_int32 Manager<TimeMgr>::resultIdentifier[] =
	{
		0
	};

	template class Manager<TimeMgr>;
}


C4::DeferredTask::DeferredTask(CompletionProc *proc, void *cookie)
{
	SetCompletionProc(proc, cookie);
	taskFlags = 0;
}

C4::DeferredTask::~DeferredTask()
{
}

bool C4::DeferredTask::GetTriggerState(void)
{
	return (true);
}


Timer::Timer(CompletionProc *proc, void *cookie) : DeferredTask(proc, cookie)
{
}

Timer::Timer(int32 time, CompletionProc *proc, void *cookie) : DeferredTask(proc, cookie)
{
	remainingTime = time;
}

Timer::~Timer()
{
}

bool Timer::GetTriggerState(void)
{
	int32 time = remainingTime - TheTimeMgr->GetDeltaTime();
	remainingTime = time;
	return (time <= 0);
}


Interpolator::Interpolator()
{
	interpolatorValue = 0.0F;
	interpolatorRate = 0.0F;
	interpolatorMode = kInterpolatorStop;
	interpolatorRange.Set(0.0F, 1.0F);

	loopProc = nullptr;
}

Interpolator::Interpolator(float value, float rate, unsigned_int32 mode)
{
	interpolatorValue = value;
	interpolatorRate = rate;
	interpolatorMode = mode;
	interpolatorRange.Set(0.0F, 1.0F);

	loopProc = nullptr;
}

Interpolator::~Interpolator()
{
}

void Interpolator::Pack(Packer& data, unsigned_int32 packFlags) const
{
	data << interpolatorValue;
	data << interpolatorRate;
	data << interpolatorMode;
	data << interpolatorRange;
}

void Interpolator::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	data >> interpolatorValue;
	data >> interpolatorRate; 
	data >> interpolatorMode;
	data >> interpolatorRange;
} 

float Interpolator::UpdateValue(SubrangeData *subrangeData) 
{
	float previous = interpolatorValue;
 
	int32 mode = interpolatorMode;
	if ((mode & (kInterpolatorForward | kInterpolatorBackward)) != 0) 
	{ 
		float value = previous;
		float dt = TheTimeMgr->GetFloatDeltaTime();

		if (mode & kInterpolatorForward) 
		{
			value += interpolatorRate * dt;
			if (value > interpolatorRange.max)
			{
				if (mode & kInterpolatorOscillate)
				{
					interpolatorMode = mode ^ (kInterpolatorForward | kInterpolatorBackward);
					value = Fmax(interpolatorRange.max * 2.0F - value, interpolatorRange.min);
					interpolatorValue = value;

					if (subrangeData)
					{
						subrangeData->subrangeCount = 2;
						subrangeData->subrange[0].Set(previous, interpolatorRange.max);
						subrangeData->subrange[1].Set(interpolatorRange.max, value);
					}
				}
				else if (mode & kInterpolatorLoop)
				{
					value = Fmin(value - interpolatorRange.max + interpolatorRange.min, interpolatorRange.max);
					interpolatorValue = value;

					if (subrangeData)
					{
						subrangeData->subrangeCount = 2;
						subrangeData->subrange[0].Set(previous, interpolatorRange.max);
						subrangeData->subrange[1].Set(interpolatorRange.min, value);
					}

					if (loopProc)
					{
						(*loopProc)(this, loopCookie);
					}
				}
				else
				{
					interpolatorMode = kInterpolatorStop;
					value = interpolatorRange.max;
					interpolatorValue = value;

					if (subrangeData)
					{
						subrangeData->subrangeCount = 1;
						subrangeData->subrange[0].Set(previous, value);
					}

					CallCompletionProc();
				}
			}
			else
			{
				interpolatorValue = value;

				if (subrangeData)
				{
					subrangeData->subrangeCount = 1;
					subrangeData->subrange[0].Set(previous, value);
				}
			}
		}
		else
		{
			value -= interpolatorRate * dt;
			if (value < interpolatorRange.min)
			{
				if (mode & kInterpolatorLoop)
				{
					if (mode & kInterpolatorOscillate)
					{
						interpolatorMode = mode ^ (kInterpolatorForward | kInterpolatorBackward);
						value = Fmin(interpolatorRange.min * 2.0F - value, interpolatorRange.max);
						interpolatorValue = value;

						if (subrangeData)
						{
							subrangeData->subrangeCount = 2;
							subrangeData->subrange[0].Set(previous, interpolatorRange.min);
							subrangeData->subrange[1].Set(interpolatorRange.min, value);
						}
					}
					else
					{
						value = Fmax(value + interpolatorRange.max - interpolatorRange.min, interpolatorRange.min);
						interpolatorValue = value;

						if (subrangeData)
						{
							subrangeData->subrangeCount = 2;
							subrangeData->subrange[0].Set(previous, interpolatorRange.min);
							subrangeData->subrange[1].Set(interpolatorRange.max, value);
						}
					}

					if (loopProc)
					{
						(*loopProc)(this, loopCookie);
					}
				}
				else
				{
					interpolatorMode = kInterpolatorStop;
					value = interpolatorRange.min;
					interpolatorValue = value;

					if (subrangeData)
					{
						subrangeData->subrangeCount = 1;
						subrangeData->subrange[0].Set(previous, value);
					}

					CallCompletionProc();
				}
			}
			else
			{
				interpolatorValue = value;

				if (subrangeData)
				{
					subrangeData->subrangeCount = 1;
					subrangeData->subrange[0].Set(previous, value);
				}
			}
		}

		return (value);
	}

	if (subrangeData)
	{
		subrangeData->subrangeCount = 0;
	}

	return (previous);
}


TimeMgr::TimeMgr(int)
{
}

TimeMgr::~TimeMgr()
{
}

EngineResult TimeMgr::Construct(void)
{
	#if C4WINDOWS

		QueryPerformanceFrequency(&counterFrequency);

	#elif C4MACOS || C4IOS

		mach_timebase_info(&timebaseInfo);

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]

	worldTimeMultiplier = 1.0F;
	worldAbsoluteTime = 0;

	systemAbsoluteTime = 0;
	systemMicrosecondTime = 0;

	ResetTime();
	return (kEngineOkay);
}

void TimeMgr::Destruct(void)
{
	taskList.Purge();

	#if C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]
}

void TimeMgr::GetDateTime(DateTime *dateTime)
{
	#if C4WINDOWS

		SYSTEMTIME		systemTime;

		GetLocalTime(&systemTime);

		dateTime->year = systemTime.wYear;
		dateTime->month = systemTime.wMonth;
		dateTime->day = systemTime.wDay;
		dateTime->hour = systemTime.wHour;
		dateTime->minute = systemTime.wMinute;
		dateTime->second = systemTime.wSecond;

	#elif C4MACOS || C4IOS

		int		year, month, day, hour, minute, second;

		CFAbsoluteTime absoluteTime = CFAbsoluteTimeGetCurrent();
		CFTimeZoneRef timeZone = CFTimeZoneCopySystem();
		CFCalendarRef calendar = CFCalendarCreateWithIdentifier(kCFAllocatorDefault, kCFGregorianCalendar);
		CFCalendarSetTimeZone(calendar, timeZone);
		CFCalendarDecomposeAbsoluteTime(calendar, absoluteTime, "yMdHms", &year, &month, &day, &hour, &minute, &second);

		dateTime->year = year;
		dateTime->month = month;
		dateTime->day = day;
		dateTime->hour = hour;
		dateTime->minute = minute;
		dateTime->second = second;

		CFRelease(calendar);
		CFRelease(timeZone);

	#elif C4LINUX

		tm		localTime;

		time_t timeValue = time(nullptr);
		localtime_r(&timeValue, &localTime);

		dateTime->year = localTime.tm_year + 1900;
		dateTime->month = localTime.tm_mon + 1;
		dateTime->day = localTime.tm_mday;
		dateTime->hour = localTime.tm_hour;
		dateTime->minute = localTime.tm_min;
		dateTime->second = localTime.tm_sec;

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]
}

void TimeMgr::GetDateTimeStrings(String<127> *date, String<127> *time)
{
	#if C4WINDOWS

		SYSTEMTIME		systemTime;

		GetLocalTime(&systemTime);

		GetDateFormatA(LOCALE_SYSTEM_DEFAULT, 0, &systemTime, nullptr, &(*date)[0], 127);
		GetTimeFormatA(LOCALE_SYSTEM_DEFAULT, 0, &systemTime, nullptr, &(*time)[0], 127);

	#elif C4MACOS || C4IOS

		CFLocaleRef locale = CFLocaleCopyCurrent();
		CFDateFormatterRef dateFormatter = CFDateFormatterCreate(kCFAllocatorDefault, locale, kCFDateFormatterShortStyle, kCFDateFormatterNoStyle);
		CFDateFormatterRef timeFormatter = CFDateFormatterCreate(kCFAllocatorDefault, locale, kCFDateFormatterNoStyle, kCFDateFormatterFullStyle);

		CFAbsoluteTime absoluteTime = CFAbsoluteTimeGetCurrent();
		CFStringRef dateString = CFDateFormatterCreateStringWithAbsoluteTime(kCFAllocatorDefault, dateFormatter, absoluteTime);
		CFStringRef timeString = CFDateFormatterCreateStringWithAbsoluteTime(kCFAllocatorDefault, timeFormatter, absoluteTime);

		CFStringGetCString(dateString, *date, 127, kCFStringEncodingUTF8);
		CFStringGetCString(timeString, *time, 127, kCFStringEncodingUTF8);

		CFRelease(timeString);
		CFRelease(dateString);
		CFRelease(timeFormatter);
		CFRelease(dateFormatter);
		CFRelease(locale);

	#elif C4LINUX

		time_t timeValue = ::time(nullptr);
		ctime_r(&timeValue, &(*date)[0]);
		(*time)[0] = 0;

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]
}

void TimeMgr::ResetTime(void)
{
	currentTimeValue = previousTimeValue = GetRawTimeValue();

	worldDeltaTime = 0;
	worldFloatDeltaTime = 0.0F;
	worldDeltaSeconds = 0.0F;
	worldResidualDeltaTime = 0.0F;

	systemDeltaTime = 0;
	systemFloatDeltaTime = 0.0F;
}

void TimeMgr::TimeTask(void)
{
	previousTimeValue = currentTimeValue;

	RawTimeValue timeValue = GetRawTimeValue();
	RawTimeValue rawDelta = timeValue - previousTimeValue;

	int64 us = MaxZero64(ConvertRawTimeValueToMicroseconds(rawDelta));
	systemMicrosecondTime = (unsigned_int32) (systemMicrosecondTime + us);

	int32 ms = (int32) (us / 1000);
	systemDeltaTime = ms;
	systemFloatDeltaTime = (float) ms;
	systemAbsoluteTime += ms;

	#if C4WINDOWS

		rawDelta = (RawTimeValue) ms * counterFrequency.QuadPart / 1000U;

	#elif C4MACOS || C4IOS

		rawDelta = (RawTimeValue) ms * 1000000U * timebaseInfo.denom / timebaseInfo.numer;

	#elif C4LINUX

		rawDelta = (RawTimeValue) ms * 1000000U;

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]

	currentTimeValue = previousTimeValue + rawDelta;

	float time = Fmin(systemFloatDeltaTime, 250.0F) * worldTimeMultiplier + worldResidualDeltaTime;
	float integerTime = PositiveFloor(time);
	worldResidualDeltaTime = time - integerTime;
	worldFloatDeltaTime = integerTime;
	worldDeltaSeconds = integerTime * 0.001F;

	worldDeltaTime = (int32) worldFloatDeltaTime;
	worldAbsoluteTime += worldDeltaTime;

	DeferredTask *task = taskList.First();
	while (task)
	{
		DeferredTask *next = task->Next();

		if (task->GetTriggerState())
		{
			unsigned_int32 flags = task->GetTaskFlags();
			if (!(flags & kTaskRepeating))
			{
				taskList.Remove(task);
			}

			task->CallCompletionProc();

			if (flags & kTaskNonpersistent)
			{
				delete task;
			}
		}

		task = next;
	}
}

// ZYUQURM
