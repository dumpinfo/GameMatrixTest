 

#ifndef C4Time_h
#define C4Time_h


//# \component	Time Manager
//# \prefix		TimeMgr/


#include "C4Packing.h"


namespace C4
{
	#if C4WINDOWS

		typedef LONGLONG			RawTimeValue;

	#elif C4MACOS || C4IOS

		typedef uint64_t			RawTimeValue;

	#elif C4LINUX

		typedef int64				RawTimeValue;

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]


	//# \enum	TaskFlags

	enum
	{
		kTaskNonpersistent			= 1 << 0,		//## The task is nonpersistent. If this flag is set, then the task is automatically destroyed after it is triggered.
		kTaskRepeating				= 1 << 1		//## The task will be triggered repeatedly (at most once per frame). If this flag is set, then the task is not removed from the Time Manager when it is triggered.
	};


	//# \enum	InterpolatorMode

	enum
	{
		kInterpolatorStop			= 0,			//## The interpolator is stopped.
		kInterpolatorForward		= 1 << 0,		//## The interpolator is moving forward. Only one of $kInterpolatorForward$ and $kInterpolatorBackward$ may be set.
		kInterpolatorBackward		= 1 << 1,		//## The interpolator is moving backward. Only one of $kInterpolatorForward$ and $kInterpolatorBackward$ may be set.
		kInterpolatorLoop			= 1 << 2,		//## The interpolator is loops in the same direction when it reaches the maximum or minimum value.
		kInterpolatorOscillate		= 1 << 3		//## The interpolator reverses direction when it reaches the maximum value. If $kInterpolatorLoop$ is also set, the interpolator reverses direction when it reaches the minimum value as well.
	};


	//# \struct	SubrangeData	Contains information about what ranges were covered during an interpolator update.
	//
	//# The $SubrangeData$ structure contains information about what ranges were covered during a call
	//# to the $@Interpolator::UpdateValue@$ function.
	//
	//# \def	struct SubrangeData
	//
	//# \data	SubrangeData
	//
	//# \also	$@Interpolator::UpdateValue@$
	//# \also	$@Utilities/Range@$


	//# \member		SubrangeData

	struct SubrangeData
	{
		int32			subrangeCount;		//## The number of subranges that were covered. This is always 0, 1, or 2.
		Range<float>	subrange[2];		//## The minimum and maximum values for each subrange.
	};


	//# \struct	DateTime		Contains information about a date and time.
	//
	//# The $DateTime$ structure contains information about a date and time.
	//
	//# \def	struct DateTime
	//
	//# \data	DateTime


	//# \member		DateTime

	struct DateTime
	{
		unsigned_int32	year;		//## The year.
		unsigned_int32	month;		//## The month, in the range 1&ndash;12.
		unsigned_int32	day;		//## The day of the month, in the range 1&ndash;31.
		unsigned_int32	hour;		//## The hour of the day, in the range 0&ndash;23.
		unsigned_int32	minute;		//## The minute, in the range 0&ndash;59.
		unsigned_int32	second;		//## The second, in the range 0&ndash;59.
	};

 
	//# \class	DeferredTask	General base class for objects which can be scheduled to perform a task after a specific amount of time.
	//
	//# \def	class DeferredTask : public ListElement<DeferredTask>, public Completable<DeferredTask> 
	//
	//# \ctor	DeferredTask(CompletionProc *proc, void *cookie = nullptr); 
	//
	//# \param	proc	The procedure to invoke when the task is triggered.
	//# \param	cookie	The cookie that is passed to the trigger procedure as its last parameter. 
	//
	//# \desc 
	//# The $DeferredTask$ class encapsulates a callback procedure that is invoked at some future time when the deferred task object is 
	//# triggered by some means. Once a deferred task or one of its subclasses has been created, it should be added to the Time Manager
	//# using the $@TimeMgr::AddTask@$ function. The callback procedure specified by the $proc$ parameter should have the following prototype.
	//
	//# \code	typedef void CompletionProc(DeferredTask *, void *); 
	//
	//# When the deferred task is triggered, the callback is invoked with the pointer specified by the $cookie$ parameter passed to it as its
	//# second argument. By default, a deferred task is triggered only once and is then removed from the Time Manager. A deferred task can be
	//# made to be triggered repeatedly by setting the $kTaskRepeating$ flag with the $@DeferredTask::SetTaskFlags@$ function.
	//#
	//# The $DeferredTask$ base class is always triggered, so the callback procedure will be invoked at the beginning of the next frame once
	//# the deferred task object has been registered using the $@TimeMgr::AddTask@$ function. See the $@Timer@$ class for a deferred task that
	//# is triggered after a specified period of time.
	//#
	//# Deferred tasks can be persistent or nonpersistent. A persistent task continues to exist after it has been triggered, but a nonpersistent
	//# task is automatically destroyed after being triggered. By default, a deferred task is persistent. The persistence state can be changed
	//# using the $@DeferredTask::SetTaskFlags@$ function.
	//
	//# \base	Utilities/ListElement<DeferredTask>		Used internally by the Time Manager.
	//# \base	Utilities/Completable<DeferredTask>		The completion procedure is called when the task is triggered.
	//
	//# \also	$@Timer@$
	//# \also	$@TimeMgr::AddTask@$
	//# \also	$@TimeMgr::RemoveTask@$


	//# \function	DeferredTask::GetTaskFlags		Returns the task flags.
	//
	//# \proto	unsigned_int32 GetTaskFlags(void) const;
	//
	//# \desc
	//# The $GetTaskFlags$ function returns the task flags, which can be a combination (through logical OR) of the following constants.
	//
	//# \table	TaskFlags
	//
	//# The initial value of the task flags is 0.
	//
	//# \also	$@DeferredTask::SetTaskFlags@$


	//# \function	DeferredTask::SetTaskFlags		Sets the task flags.
	//
	//# \proto	void SetTaskFlags(unsigned_int32 flags);
	//
	//# \param	flags		The new task flags.
	//
	//# \desc
	//# The $SetTaskFlags$ function sets the task flags, which can be a combination (through logical OR) of the following constants.
	//
	//# \table	TaskFlags
	//
	//# The initial value of the task flags is 0.
	//
	//# \also	$@DeferredTask::GetTaskFlags@$


	class DeferredTask : public ListElement<DeferredTask>, public Completable<DeferredTask>
	{
		friend class TimeMgr;

		private:

			unsigned_int32		taskFlags;

		protected:

			C4API virtual bool GetTriggerState(void);

		public:

			C4API DeferredTask(CompletionProc *proc, void *cookie = nullptr);
			C4API virtual ~DeferredTask();

			unsigned_int32 GetTaskFlags(void) const
			{
				return (taskFlags);
			}

			void SetTaskFlags(unsigned_int32 flags)
			{
				taskFlags = flags;
			}
	};


	//# \class	Timer	A deferred task which signals the expiration of a specific amount of time.
	//
	//# \def	class Timer : public DeferredTask
	//
	//# \ctor	Timer(int32 time, CompletionProc *proc, void *cookie = nullptr);
	//
	//# \param	time	The length of time, in milliseconds, which should pass before the timer is triggered.
	//# \param	proc	The procedure to invoke when the task is triggered.
	//# \param	cookie	The cookie that is passed to the trigger procedure as its last parameter.
	//
	//# \desc
	//# The $Timer$ class encapsulates a deferred task which is triggered after the time specified by the $time$ parameter.
	//# Once a timer is registered using the $@TimeMgr::AddTask@$ function, its remaining time is reduced each frame until
	//# it reaches zero, at which point its callback procedure is invoked. The callback procedure specified by the $proc$
	//# parameter should have the following prototype.
	//
	//# \code	typedef void CompletionProc(DeferredTask *, void *);
	//
	//# When the timer is triggered, the callback is invoked with the pointer specified by the $cookie$ parameter passed to it as its
	//# second argument. A timer is triggered only once and must be resubmitted with the $@TimeMgr::AddTask@$ function in order to
	//# be triggered again.
	//
	//# \base	DeferredTask	$Timer$ objects provide a trigger for $DeferredTask$ objects.


	//# \function	Timer::GetTime		Returns the length of time until the timer is triggered.
	//
	//# \proto	int32 GetTime(void) const;
	//
	//# \desc
	//# The $GetTime$ function returns the length of time remaining, in milliseconds, before a timer object is triggered.
	//
	//# \also	$@Timer::SetTime@$


	//# \function	Timer::SetTime		Sets the length of time which should pass before the timer is triggered.
	//
	//# \proto	void SetTime(int32 time);
	//
	//# \param	time	The length of time, in milliseconds, which should pass before the timer is triggered.
	//
	//# \desc
	//# The $SetTime$ function sets the length of time remaining, in milliseconds, before a timer object is triggered.
	//# If the time is zero or negative, then the timer will be triggered on the next frame.
	//
	//# \also	$@Timer::GetTime@$


	class Timer : public DeferredTask
	{
		private:

			int32	remainingTime;

			C4API bool GetTriggerState(void);

		public:

			C4API Timer(CompletionProc *proc, void *cookie = nullptr);
			C4API Timer(int32 time, CompletionProc *proc, void *cookie = nullptr);
			C4API ~Timer();

			int32 GetTime(void) const
			{
				return (remainingTime);
			}

			void SetTime(int32 time)
			{
				remainingTime = time;
			}
	};


	//# \class	Interpolator	Encapsulates a general interpolator.
	//
	//# \def	class Interpolator : public Completable<Interpolator>
	//
	//# \ctor	Interpolator();
	//# \ctor	Interpolator(float value, float rate = 0.0F, unsigned_int32 mode = kInterpolatorStop);
	//
	//# \param	value	The initial value of the interpolator.
	//# \param	rate	The rate at which the interpolated value changes. This is measured in value change per millisecond.
	//# \param	mode	The initial interpolation mode. See below for possible values.
	//
	//# \desc
	//# The $Interpolator$ class is used to represent a generic value that is being interpolated between two other values.
	//# An $Interpolator$ object stores a current value, the range of values through which it is interpolated, the rate at which
	//# it moves from one end of the range to the other, and a mode that determinates what happens when an endpoint is reached.
	//#
	//# The $value$ parameter specifies the initial value of the interpolator. If the default constructor is used, then the
	//# initial value of the interpolator is zero. The initial value can be outside the minimum and maximum values that are
	//# initially assigned as the range of the interpolator.
	//#
	//# The $rate$ parameter specifies the rate at which the interpolated value changes as a difference applied for each
	//# millisecond of time. For example, if a rate of 0.005 is specified, it would take 200&nbsp;ms to interpolate from an
	//# initial value of 0.0 to a final value of 1.0. If the default constructor is used, then the initial rate is 0.0,
	//# meaning that the interpolator's value will never change.
	//#
	//# The $mode$ parameter specifies the initial mode of the interpolator, which can be a combination (through logical OR)
	//# of the following constants.
	//
	//# \table	InterpolatorMode
	//
	//# If the default constructor is used, then the initial mode is $kInterpolatorStop$.
	//#
	//# The initial range for an interpolator object is [0.0,&nbsp;1.0].
	//#
	//# The interpolated value is changed based on the current rate and mode when the $@Interpolator::UpdateValue@$ function is called.
	//
	//# \base	Utilities/Completable<Interpolator>		The completion procedure is called when an interpolator goes into the stopped state.


	//# \function	Interpolator::GetValue		Returns the current value of an interpolator.
	//
	//# \proto	float GetValue(void) const;
	//
	//# \desc
	//# The $GetValue$ function returns the current value stored inside an interpolator object. The current
	//# value can be set explicitly by calling the $@Interpolator::SetValue@$ function.
	//#
	//# The current value of an interpolator is updated according to the rate and mode state by calling the
	//# $@Interpolator::UpdateValue@$ function.
	//
	//# \also	$@Interpolator::SetValue@$
	//# \also	$@Interpolator::UpdateValue@$


	//# \function	Interpolator::SetValue		Sets the current value of an interpolator.
	//
	//# \proto	void SetValue(float value);
	//
	//# \param	value	The new interpolator value.
	//
	//# \desc
	//# The $SetValue$ function sets the current value stored inside an interpolator object to that specified
	//# by the $value$ parameter.
	//#
	//# The current value of an interpolator is updated according to the rate and mode state by calling the
	//# $@Interpolator::UpdateValue@$ function.
	//
	//# \also	$@Interpolator::GetValue@$


	//# \function	Interpolator::GetRange		Returns the range of an interpolator.
	//
	//# \proto	const Range<float>& GetRange(void) const;
	//
	//# \desc
	//# The $GetRange$ function returns the current range of an interpolator. The interpolated value is changed
	//# at the current rate until it reaches the end of the range or the beginning of the range, depending on
	//# whether $kInterpolatorForward$ or $kInterpolatorBackward$ is part of the current mode.
	//#
	//# It is possible for an interpolator's current value to be outside the interpolator's range. This can
	//# happen if the current value is set to a value less than the range's minimum value and the current mode
	//# includes $kInterpolatorForward$, or it can happen if the current value is set to a value greater than
	//# the range's maximum value and the current mode includes $kInterpolatorBackward$.
	//
	//# \also	$@Interpolator::SetRange@$
	//# \also	$@Interpolator::SetMinValue@$
	//# \also	$@Interpolator::SetMaxValue@$
	//# \also	$@Utilities/Range@$


	//# \function	Interpolator::SetMinValue		Sets the minimum value of an interpolator.
	//
	//# \proto	void SetMinValue(float min);
	//
	//# \param	min		The new minimum interpolator value.
	//
	//# \desc
	//# The $SetMinValue$ function sets the minimum value of an interpolator's range without affecting the
	//# maximum value. Calling this function does not affect the current value of the interpolator.
	//
	//# \also	$@Interpolator::SetMaxValue@$
	//# \also	$@Interpolator::GetRange@$
	//# \also	$@Interpolator::SetRange@$


	//# \function	Interpolator::SetMaxValue		Sets the maximum value of an interpolator.
	//
	//# \proto	void SetMaxValue(float max);
	//
	//# \param	max		The new maximum interpolator value.
	//
	//# \desc
	//# The $SetMaxValue$ function sets the maximum value of an interpolator's range without affecting the
	//# minimum value. Calling this function does not affect the current value of the interpolator.
	//
	//# \also	$@Interpolator::SetMinValue@$
	//# \also	$@Interpolator::GetRange@$
	//# \also	$@Interpolator::SetRange@$


	//# \function	Interpolator::SetRange			Sets the minimum and maximum values of an interpolator.
	//
	//# \proto	void SetRange(float min, float max);
	//
	//# \param	min		The new minimum interpolator value.
	//# \param	max		The new maximum interpolator value.
	//
	//# \desc
	//# The $SetRange$ function sets the range for an interpolator to the values specified by the $min$ and
	//# $max$ parameters. Calling this function does not affect the current value of the interpolator.
	//
	//# \also	$@Interpolator::GetRange@$
	//# \also	$@Interpolator::SetMinValue@$
	//# \also	$@Interpolator::SetMaxValue@$


	//# \function	Interpolator::GetRate		Returns the rate at which an interpolator changes.
	//
	//# \proto	float GetRate(void) const;
	//
	//# \desc
	//# The $GetRate$ function returns the current rate at which an interpolator changes. The rate is measured
	//# in the change in value per millisecond.
	//
	//# \also	$@Interpolator::SetRate@$


	//# \function	Interpolator::SetRate		Sets the rate at which an interpolator changes.
	//
	//# \proto	void SetRate(float rate);
	//
	//# \param	rate	The new interpolator rate.
	//
	//# \desc
	//# The $SetRate$ function sets the current rate at which an interpolator changes to that specified by the
	//# $rate$ parameter. The rate is measured in the change in value per millisecond. For example, if a rate
	//# of 0.005 is specified, it would take 200&nbsp;ms to interpolate from an initial value of 0.0 to a final
	//# value of 1.0.
	//
	//# \also	$@Interpolator::GetRate@$


	//# \function	Interpolator::GetMode		Returns the current mode for an interpolator.
	//
	//# \proto	unsigned_int32 GetMode(void) const;
	//
	//# \desc
	//# The $GetMode$ function returns the current mode for an interpolator, which can be a combination
	//# (through logical OR) of the following constants.
	//
	//# \table	InterpolatorMode
	//
	//# \also	$@Interpolator::SetMode@$


	//# \function	Interpolator::SetMode		Sets the current mode for an interpolator.
	//
	//# \proto	void SetMode(unsigned_int32 mode);
	//
	//# \param	mode	The new interpolator mode.
	//
	//# \desc
	//# The $SetMode$ function sets the interpolator mode to the value specified by the $mode$ parameter.
	//# This value can be a combination (through logical OR) of the following constants.
	//
	//# \table	InterpolatorMode
	//
	//# \also	$@Interpolator::GetMode@$


	//# \function	Interpolator::SetLoopProc		Sets the loop callback procedure for an interpolator.
	//
	//# \proto	void SetLoopProc(LoopProc *proc, void *cookie = nullptr);
	//
	//# \param	proc	The new loop callback procedure.
	//# \param	cookie	The cookie that is passed to the callback procedure.
	//
	//# \desc
	//# The $SetLoopProc$ function installs a callback procedure that is called when a looping interpolator
	//# reaches the end of one cycle and loops. An interpolator is looping if its current mode includes the
	//# $kInterpolatorLoop$ flag. The callback procedure specified by the $proc$ parameter should point to
	//# a function having the following prototype.
	//
	//# \code	typedef void LoopProc(Interpolator *, void *);
	//
	//# A pointer to the interpolator that has looped is passed as the first parameter to the callback, and
	//# the value of the $cookie$ parameter is passed as the second parameter.
	//#
	//# An interpolator initially has no loop callback procedure. If the $proc$ parameter is $nullptr$,
	//# then any existing loop callback procedure is removed from the interpolator.
	//#
	//# If an interpolator's mode is equal to $kInterpolatorForward | kInterpolatorLoop$, then the loop callback
	//# procedure is called when the current value reaches the maximum value of the interpolator's range.
	//# If an interpolator's mode is equal to $kInterpolatorBackward | kInterpolatorLoop$, then the loop callback
	//# procedure is called when the current value reaches the minimum value of the interpolator's range.
	//# If the $kInterpolatorOscillate$ flag is set in the current mode, then the loop callback procedure is
	//# called only when the current value is decreasing and reaches the minimum value of the interpolator's range.
	//#
	//# It is safe to destroy an interpolator from inside the loop callback procedure.
	//
	//# \also	$@Interpolator::GetMode@$
	//# \also	$@Interpolator::SetMode@$


	//# \function	Interpolator::Set		Sets the current value, rate, and mode for an interpolator.
	//
	//# \proto	void Set(float value, float rate, unsigned_int32 mode);
	//
	//# \param	value	The new interpolator value.
	//# \param	rate	The new interpolator rate.
	//# \param	mode	The new interpolator mode.
	//
	//# \desc
	//# The $Set$ function sets the current value, rate, and mode for an interpolator to those specified
	//# by the $value$, $rate$, and $mode$ parameters.
	//
	//# \also	$@Interpolator::GetValue@$
	//# \also	$@Interpolator::SetValue@$
	//# \also	$@Interpolator::GetRate@$
	//# \also	$@Interpolator::SetRate@$
	//# \also	$@Interpolator::GetMode@$
	//# \also	$@Interpolator::SetMode@$


	//# \function	Interpolator::UpdateValue		Updates an interpolator.
	//
	//# \proto	float UpdateValue(SubrangeData *subrangeData = nullptr);
	//
	//# \param	subrangeData	An optional pointer to a structure that receives information about what subranges were covered.
	//
	//# \desc
	//# The $UpdateValue$ function changes the current value of an interpolator based on the current rate, mode, and range.
	//# The current mode must include either the $kInterpolatorForward$ flag or $kInterpolatorBackward$ flag in order
	//# for any change to be made.
	//#
	//# The current mode may be changed if the current value reaches either the minimum or maximum value of the
	//# interpolator's range. For example, if the current mode is $kInterpolateForward$ and the current value reaches
	//# the maximum value of the range, then the value is pinned at the maximum value and the mode is changed to
	//# $kInterpolatorStop$.
	//#
	//# If the interpolator mode is changed to $kInterpolatorStop$, which happens when a non-looping interpolator reaches
	//# one of the ends of the current range, then the completion procedure is invoked for the interpolator if one
	//# is installed. See the $@Utilities/Completable@$ base class. It is safe to destroy an interpolator from inside the
	//# completion procedure.
	//#
	//# If the current mode includes the $kInterpolatorLoop$ flag and the interpolator loops by passing one end of the
	//# current range and wrapping around to the other, then the loop callback procedure is invoked if one is installed.
	//# See the $@Interpolator::SetLoopProc@$ function. It is safe to destroy an interpolator from inside the loop
	//# callback procedure.
	//#
	//# If the $subrangeData$ parameter is not $nullptr$, then the structure it points to is filled in with information
	//# about what ranges of values were covered during the update. For a stopped interpolator, this structure will
	//# always indicate no subranges. Otherwise, this structure will contain either one or two subranges depending on
	//# whether any looping or oscillation occurred. The minimum value in each subrange always corresponds to the value
	//# attained by the interpolator earlier in time. For any subrange over which the interpolator increased in value,
	//# the minimum value will be less than or equal to the maximum value, but for any subrange over which the interpolator
	//# decreased in value, the minimum value will be greater than or equal to the maximum value.
	//
	//# \also	$@Interpolator::GetValue@$
	//# \also	$@Interpolator::SetValue@$
	//# \also	$@Interpolator::Set@$
	//# \also	$@SubrangeData@$


	class Interpolator : public Completable<Interpolator>, public Packable
	{
		public:

			typedef void LoopProc(Interpolator *, void *);

		private:

			float				interpolatorValue;
			float				interpolatorRate;
			unsigned_int32		interpolatorMode;
			Range<float>		interpolatorRange;

			LoopProc			*loopProc;
			void				*loopCookie;

		public:

			C4API Interpolator();
			C4API Interpolator(float value, float rate = 0.0F, unsigned_int32 mode = kInterpolatorStop);
			C4API ~Interpolator();

			float GetValue(void) const
			{
				return (interpolatorValue);
			}

			void SetValue(float value)
			{
				interpolatorValue = value;
			}

			float GetRate(void) const
			{
				return (interpolatorRate);
			}

			void SetRate(float rate)
			{
				interpolatorRate = rate;
			}

			unsigned_int32 GetMode(void) const
			{
				return (interpolatorMode);
			}

			void SetMode(unsigned_int32 mode)
			{
				interpolatorMode = mode;
			}

			const Range<float>& GetRange(void) const
			{
				return (interpolatorRange);
			}

			void SetMinValue(float min)
			{
				interpolatorRange.min = min;
			}

			void SetMaxValue(float max)
			{
				interpolatorRange.max = max;
			}

			void SetRange(float min, float max)
			{
				interpolatorRange.Set(min, max);
			}

			void SetLoopProc(LoopProc *proc, void *cookie = nullptr)
			{
				loopProc = proc;
				loopCookie = cookie;
			}

			void Set(float value, float rate, unsigned_int32 mode)
			{
				interpolatorValue = value;
				interpolatorRate = rate;
				interpolatorMode = mode;
			}

			C4API void Pack(Packer& data, unsigned_int32 packFlags) const override;
			C4API void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			C4API float UpdateValue(SubrangeData *subrangeData = nullptr);
	};


	//# \class	TimeMgr		The Time Manager class.
	//
	//# \def	class TimeMgr : public Manager<TimeMgr>
	//
	//# \desc
	//# The $TimeMgr$ class encapsulates the low-level timing functionality of the C4 Engine.
	//# The single instance of the Time Manager is constructed during an application's initialization
	//# and destroyed at termination.
	//#
	//# The Time Manager's member functions are accessed through the global pointer $TheTimeMgr$.


	//# \function	TimeMgr::GetWorldTimeMultiplier		Returns the multiplier that converts normal time into world time.
	//
	//# \proto	float GetWorldTimeMultiplier(void) const;
	//
	//# \desc
	//#
	//
	//# \also	$@TimeMgr::SetWorldTimeMultiplier@$


	//# \function	TimeMgr::SetWorldTimeMultiplier		Sets the multiplier that converts normal time into world time.
	//
	//# \proto	void SetWorldTimeMultiplier(float multiplier);
	//
	//# \param	multiplier		The world time multiplier. The default value is 1.0.
	//
	//# \desc
	//#
	//
	//# \also	$@TimeMgr::GetWorldTimeMultiplier@$


	//# \function	TimeMgr::GetDeltaTime		Returns the time difference between the current frame and the previous frame as an integer.
	//
	//# \proto	int32 GetDeltaTime(void) const;
	//
	//# \desc
	//# The $GetDeltaTime$ function returns the integer difference in time between the current frame and the previous
	//# frame. The value returned is in milliseconds.
	//
	//# \also	$@TimeMgr::GetFloatDeltaTime@$
	//# \also	$@TimeMgr::GetAbsoluteTime@$


	//# \function	TimeMgr::GetFloatDeltaTime		Returns the time difference between the current frame and the previous frame as a floating point number.
	//
	//# \proto	float GetFloatDeltaTime(void) const;
	//
	//# \desc
	//# The $GetFloatDeltaTime$ function returns the floating-point difference in time between the current frame and
	//# the previous frame. The value returned is in milliseconds.
	//
	//# \also	$@TimeMgr::GetDeltaTime@$
	//# \also	$@TimeMgr::GetAbsoluteTime@$


	//# \function	TimeMgr::GetAbsoluteTime		Returns the current absolute millisecond count.
	//
	//# \proto	unsigned_int32 GetAbsoluteTime(void) const;
	//
	//# \desc
	//
	//# \also	$@TimeMgr::GetDeltaTime@$
	//# \also	$@TimeMgr::GetFloatDeltaTime@$


	//# \div
	//# \function	TimeMgr::AddTask		Adds a deferred task to the Time Manager task list.
	//
	//# \proto	void AddTask(DeferredTask *task);
	//
	//# \param	task	The deferred task to add.
	//
	//# \desc
	//# The $AddTask$ function registers a deferred task object with the Time Manager. A deferred task can be
	//# triggered only after it has been registered.
	//#
	//# A deferred task can be unregistered explicitly using the $@TimeMgr::RemoveTask@$, or it can simply be deleted.
	//
	//# \also	$@TimeMgr::RemoveTask@$
	//# \also	$@DeferredTask@$


	//# \function	TimeMgr::RemoveTask		Removes a deferred task from the Time Manager task list.
	//
	//# \proto	void RemoveTask(DeferredTask *task);
	//
	//# \param	task	The deferred task to remove.
	//
	//# \desc
	//# The $RemoveTask$ function unregisters a deferred task object with the Time Manager.
	//
	//# \also	$@TimeMgr::AddTask@$
	//# \also	$@DeferredTask@$


	//# \div
	//# \function	TimeMgr::GetDateTime		Returns a structure containing the current date and time.
	//
	//# \proto	static void GetDateTime(DateTime *dateTime);
	//
	//# \param	dateTime	The structure that receives the current date and time.
	//
	//# \desc
	//# The $GetDateTime$ function fills in the $@DateTime@$ structure specified by the $dateTime$ parameter with the
	//# current date and time. The date and time correspond to the time zone set for the local machine.
	//
	//# \also	$@TimeMgr::GetDateTimeStrings@$
	//# \also	$@DateTime@$


	//# \function	TimeMgr::GetDateTimeStrings		Returns strings containing the current date and time.
	//
	//# \proto	static void GetDateTimeStrings(String<127> *date, String<127> *time);
	//
	//# \param	date	A pointer to a string that receives the date.
	//# \param	time	A pointer to a string that receives the time.
	//
	//# \desc
	//# The $GetDateTimeStrings$ function returns strings containing the current date and time in the strings
	//# specified by the $date$ and $time$ parameters. The date and time correspond to the time zone set for the local machine.
	//
	//# \also	$@TimeMgr::GetDateTime@$


	class TimeMgr : public Manager<TimeMgr>
	{
		private:

			#if C4WINDOWS

				LARGE_INTEGER				counterFrequency;

			#elif C4MACOS || C4IOS

				mach_timebase_info_data_t	timebaseInfo;

			#endif

			RawTimeValue			currentTimeValue;
			RawTimeValue			previousTimeValue;

			float					worldTimeMultiplier;

			int32					worldDeltaTime;
			float					worldFloatDeltaTime;
			float					worldDeltaSeconds;

			float					worldResidualDeltaTime;
			unsigned_int32			worldAbsoluteTime;

			int32					systemDeltaTime;
			float					systemFloatDeltaTime;
			unsigned_int32			systemAbsoluteTime;

			unsigned_int32			systemMicrosecondTime;

			List<DeferredTask>		taskList;

			static RawTimeValue GetRawTimeValue(void)
			{
				#if C4WINDOWS

					LARGE_INTEGER	counter;

					QueryPerformanceCounter(&counter);
					return (counter.QuadPart);

				#elif C4MACOS || C4IOS

					return (mach_absolute_time());

				#elif C4LINUX

					timespec		spec;

					clock_gettime(CLOCK_MONOTONIC, &spec);
					return (RawTimeValue(spec.tv_sec) * 1000000000U + spec.tv_nsec);

				#elif C4PS4 //[ PS4

					// -- PS4 code hidden --

				#elif C4PS3 //[ PS3

					// -- PS3 code hidden --

				#endif //]
			}

			int32 ConvertRawTimeValueToMilliseconds(RawTimeValue time) const
			{
				#if C4WINDOWS

					return ((int32) (time * 1000U / counterFrequency.QuadPart));

				#elif C4MACOS || C4IOS

					return ((int32) (time * timebaseInfo.numer / (timebaseInfo.denom * 1000000U)));

				#elif C4LINUX

					return ((int32) (time / 1000000U));

				#elif C4PS4 //[ PS4

					// -- PS4 code hidden --

				#elif C4PS3 //[ PS3

					// -- PS3 code hidden --

				#endif //]
			}

			int64 ConvertRawTimeValueToMicroseconds(RawTimeValue time) const
			{
				#if C4WINDOWS

					return (time * 1000000U / counterFrequency.QuadPart);

				#elif C4MACOS || C4IOS

					return (time * timebaseInfo.numer / (timebaseInfo.denom * 1000U));

				#elif C4LINUX

					return ((int32) (time / 1000U));

				#elif C4PS4 //[ PS4

					// -- PS4 code hidden --

				#elif C4PS3 //[ PS3

					// -- PS3 code hidden --

				#endif //]
			}

		public:

			TimeMgr(int);
			~TimeMgr();

			EngineResult Construct(void);
			void Destruct(void);

			float GetWorldTimeMultiplier(void) const
			{
				return (worldTimeMultiplier);
			}

			void SetWorldTimeMultiplier(float multiplier)
			{
				worldTimeMultiplier = multiplier;
			}

			int32 GetDeltaTime(void) const
			{
				return (worldDeltaTime);
			}

			float GetFloatDeltaTime(void) const
			{
				return (worldFloatDeltaTime);
			}

			float GetDeltaSeconds(void) const
			{
				return (worldDeltaSeconds);
			}

			unsigned_int32 GetAbsoluteTime(void) const
			{
				return (worldAbsoluteTime);
			}

			int32 GetSystemDeltaTime(void) const
			{
				return (systemDeltaTime);
			}

			float GetSystemFloatDeltaTime(void) const
			{
				return (systemFloatDeltaTime);
			}

			unsigned_int32 GetSystemAbsoluteTime(void) const
			{
				return (systemAbsoluteTime);
			}

			unsigned_int32 GetSystemMicrosecondTime(void) const
			{
				return (systemMicrosecondTime);
			}

			unsigned_int32 GetMillisecondCount(void) const
			{
				return (ConvertRawTimeValueToMilliseconds(GetRawTimeValue()));
			}

			unsigned_int64 GetMicrosecondCount(void) const
			{
				return (ConvertRawTimeValueToMicroseconds(GetRawTimeValue()));
			}

			void AddTask(DeferredTask *task)
			{
				taskList.Append(task);
			}

			void RemoveTask(DeferredTask *task)
			{
				taskList.Remove(task);
			}

			C4API static void GetDateTime(DateTime *dateTime);
			C4API static void GetDateTimeStrings(String<127> *date, String<127> *time);

			C4API void ResetTime(void);

			C4API void TimeTask(void);
	};


	C4API extern TimeMgr *TheTimeMgr;
}


#endif

// ZYUQURM
