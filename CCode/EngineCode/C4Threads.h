 

#ifndef C4Threads_h
#define C4Threads_h


//# \component	System Utilities
//# \prefix		System/


#include "C4Types.h"


namespace C4
{
	enum
	{
		kMaxSignalCount		= 4,
		kSignalForever		= -1,
		kSignalTimeout		= -1
	};


	//# \enum	ThreadPriority

	enum
	{
		kThreadPriorityLow,				//## The thread has low priority.
		kThreadPriorityNormal,			//## The thread has normal priority.
		kThreadPriorityHigh,			//## The thread has high priority. If this priority is used, then the thread should block often so that lower-priority threads can run.
		kThreadPriorityCritical			//## The thread has critical priority. If this priority is used, then the thread should be in the blocked state most of the time.
	};


	//# \enum	JobFlags

	enum
	{
		kJobNonpersistent		= 1 << 0	//## The job is nonpersistent. If this flag is set, then the job is automatically destroyed when its batch is finished.
	};


	enum
	{
		kJobExecuting			= 1 << 0,
		kJobComplete			= 1 << 1,
		kJobCancelled			= 1 << 2
	};


	class Batch;


	//# \class	Lock	Encapsulates a shared lock object for multithreaded synchronization.
	//
	//# The $Lock$ class encapsulates a shared lock object for multithreaded synchronization.
	//
	//# \def	class Lock
	//
	//# \ctor	Lock();
	//
	//# \desc
	//# The $Lock$ class defines a platform-independent shared lock object that can be used for
	//# multithreaded synchronization. A lock object can be acquired for exclusive access by a
	//# single thread, or it can be acquired for shared access by multiple threads simultaneously.
	//# Exclusive access is ordinarily required when a thread is going to write to a shared
	//# resource, and shared access is ordinarily sufficient when multiple threads only need to
	//# read from a shared resource.
	//
	//# \warning
	//# If a lock object is destroyed while a thread is waiting on it, then the
	//# behavior is undefined.
	//
	//# \also	$@Mutex@$
	//# \also	$@Signal@$
	//# \also	$@Thread@$


	//# \function	Lock::AcquireExclusive		Acquires exclusive access to a lock.
	//
	//# \proto	void AcquireExclusive(void);
	//
	//# \desc
	//# The $AcquireExclusive$ function acquires exclusive access to a lock.
	//# If exclusive or shared access to the lock has already been acquired by another thread at the time
	//# this function is called, then the calling thread blocks until all access to the lock has been
	//# released by other threads.
	//#
	//# During the time that a lock is acquired with exclusive access, all other attempts to acquire the
	//# lock with either exclusive or shared access will block.
	//#
	//# Exclusive access to a lock must be relinquished by calling the $@Lock::ReleaseExclusive@$ function
	//# when the calling thread no longer needs the lock.
	//
	//# \also	$@Lock::ReleaseExclusive@$
	//# \also	$@Lock::AcquireShared@$
	//# \also	$@Lock::ReleaseShared@$


	//# \function	Lock::ReleaseExclusive		Releases exclusive access to a lock.
	//
	//# \proto	void ReleaseExclusive(void);
	// 
	//# \desc
	//# The $ReleaseExclusive$ function relinquishes exclusive access to a lock that was previously
	//# acquired using the $@Lock::AcquireExclusive@$ function. Once the $ReleaseExclusive$ 
	//# function has been called, the lock can once again be acquired with shared access.
	// 
	//# \also	$@Lock::AcquireExclusive@$
	//# \also	$@Lock::AcquireShared@$
	//# \also	$@Lock::ReleaseShared@$ 

 
	//# \function	Lock::AcquireShared		Acquires shared access to a lock. 
	//
	//# \proto	void AcquireShared(void);
	//
	//# \desc 
	//# The $AcquireShared$ function acquires shared access to a lock.
	//# If exclusive to the lock has already been acquired by another thread at the time
	//# this function is called, then the calling thread blocks until the exclusive access has been
	//# released. However, all other shared acquisitions of the lock will succeed without blocking.
	//#
	//# To balance each call to the $AcquireShared$ function, shared access to a lock must be relinquished
	//# by calling the $@Lock::ReleaseShared@$ function when the calling thread no longer needs the lock.
	//
	//# \also	$@Lock::ReleaseShared@$
	//# \also	$@Lock::AcquireExclusive@$
	//# \also	$@Lock::ReleaseExclusive@$


	//# \function	Lock::ReleaseShared		Releases shared access to a lock.
	//
	//# \proto	void ReleaseShared(void);
	//
	//# \desc
	//# The $ReleaseShared$ function relinquishes shared access to a lock that was previously
	//# acquired using the $@Lock::AcquireShared@$ function. Each previous call to the $@Lock::AcquireShared@$
	//# function must be balanced with a call to the $ReleaseShared$ function. A lock cannot be acquired with
	//# exclusive access until all threads have released shared access to the lock.
	//
	//# \also	$@Lock::AcquireShared@$
	//# \also	$@Lock::AcquireExclusive@$
	//# \also	$@Lock::ReleaseExclusive@$


	class Lock
	{
		private:

			Mutex				lockMutex;
			volatile int32		lockCount;

		public:

			C4API Lock();

			void ReleaseExclusive(void)
			{
				lockMutex.Release();
			}

			C4API void AcquireExclusive(void);

			C4API void AcquireShared(void);
			C4API void ReleaseShared(void);
	};


	//# \class	Signal		Encapsulates a signal object for multithreaded synchronization.
	//
	//# The $Signal$ class encapsulates a signal object for multithreaded synchronization.
	//
	//# \def	class Signal
	//
	//# \ctor	explicit Signal(int32 count = 1);
	//
	//# \param	count	The number of separate signals that can be triggered.
	//#					This must be a positive number less than $kMaxSignalCount$.
	//
	//# \desc
	//# The $Signal$ class defines a platform-independent signal object that can be used for
	//# multithreaded synchronization. A signal object encapsulates up to $kMaxSignalCount$
	//# separate binary signals that can be individually triggered. A thread can wait on a
	//# signal object using the $@Signal::Wait@$ function in order to block itself until another
	//# thread activates the signal using the $@Signal::Trigger@$ function.
	//
	//# \warning
	//# If a signal object is destroyed while a thread is waiting on it, then the
	//# behavior is undefined.
	//
	//# \also	$@Lock@$
	//# \also	$@Mutex@$
	//# \also	$@Thread@$


	//# \function	Signal::Trigger		Triggers a signal.
	//
	//# \proto	void Trigger(int32 index = 0);
	//
	//# \param	index	The index of the signal to trigger.
	//
	//# \desc
	//# The $Trigger$ function activates a specific signal inside a signal object. The signal
	//# specified by the $index$ parameter must be in the range [0, $count$ - 1], where $count$
	//# is the number of signals specified when the signal object was constructed. When the
	//# $Trigger$ function is called, any thread waiting on the signal object using the
	//# $@Signal::Wait@$ function is unblocked, and the return value of the $Signal::Wait$
	//# function is the same as the $index$ value passed to the $Trigger$ function.
	//#
	//# It is possible to assign a signal to a thread using the $@Thread::SetThreadSignal@$
	//# function. If this is done, then the destructor for the $@Thread@$ class will trigger
	//# signal 0 of the signal object to indicate to the thread's code that it should exit.
	//
	//# \also	$@Signal::Wait@$
	//# \also	$@Thread@$


	//# \function	Signal::Wait		Waits for a signal to be triggered.
	//
	//# \proto	int32 Wait(int32 time = kSignalForever);
	//
	//# \param	time	The time, in milliseconds, to wait for a signal. This should be a nonnegative integer or
	//#					the special constant $kSignalForever$ indicating that the function should not time out.
	//
	//# \desc
	//# The $Wait$ function waits for a signal to be triggered or for a timeout interval to expire. If no trigger occurs
	//# before the timeout interval expires, then the return value is $kSignalTimeout$. Otherwise, the return value is
	//# the index value that was passed to the $@Signal::Trigger@$ function to trigger the signal.
	//
	//# \also	$@Signal::Trigger@$


	class Signal
	{
		private:

			#if C4WINDOWS

				int32						signalCount;
				HANDLE						signalEvent[kMaxSignalCount];

			#elif C4POSIX

				pthread_cond_t				condID;
				pthread_mutex_t				mutexID;
				volatile unsigned_int32		signalValue;

			#elif C4PS4 //[ PS4

				// -- PS4 code hidden --

			#elif C4PS3 //[ PS3

				// -- PS3 code hidden --

			#endif //]

		public:

			C4API explicit Signal(int32 count = 1);
			C4API ~Signal();

			#if C4WINDOWS

				void Trigger(int32 index = 0)
				{
					SetEvent(signalEvent[index]);
				}

				HANDLE GetEventHandle(int32 index = 0)
				{
					return (signalEvent[index]);
				}

			#elif C4POSIX

				C4API void Trigger(int32 index = 0);

			#elif C4PS4 //[ PS4

				// -- PS4 code hidden --

			#elif C4PS3 //[ PS3

				// -- PS3 code hidden --

			#endif //]

			C4API int32 Wait(int32 time = kSignalForever);
	};


	//# \class	Thread		Encapsulates a thread of execution.
	//
	//# The $Thread$ class encapsulates a thread of execution.
	//
	//# \def	class Thread
	//
	//# \ctor	Thread(ThreadProc *proc, void *cookie, unsigned_int32 stack = 0, Signal *signal = nullptr);
	//
	//# \param	proc		The entry point for the thread.
	//# \param	cookie		A pointer to user-defined data that is passed to the thread's entry point.
	//# \param	stack		The thread's stack size in bytes. If this is 0, then the default stack size is used.
	//# \param	signal		The signal object to assign to the thread.
	//
	//# \desc
	//# The $Thread$ class defines a platform-independent thread object that can be used to
	//# achieve multithreaded execution. When a new $Thread$ object is constructed, the entry function
	//# specified by the $proc$ parameter begins executing immediately. The pointer passed into the $cookie$
	//# parameter is passed to the thread's entry function, which must have the following signature.
	//
	//# \code	typedef void ThreadProc(const Thread *, void *);
	//
	//# If the $stack$ parameter is not zero, then it specifies the size of the stack allocated for the thread.
	//# Otherwise, the default stack size, as determined by the operating system, is used.
	//#
	//# If the $signal$ parameter is not $nullptr$, then it specifies a $@Signal@$ object that is assigned
	//# to the thread. When a $Thread$ object is destroyed and it has a $Signal$ object assigned to it, the
	//# thread triggers signal 0 to indicate that the thread should exit.
	//
	//# \also	$@Lock@$
	//# \also	$@Mutex@$
	//# \also	$@Signal@$


	//# \function	Thread::GetThreadSignal		Returns the signal associated with the thread.
	//
	//# \proto	Signal *GetThreadSignal(void) const;
	//
	//# \desc
	//# The $GetThreadSignal$ function returns the $@Signal@$ object assigned to the thread.
	//# Initially, a thread has no signal object assigned to it, so this function returns $nullptr$.
	//
	//# \also	$@Thread::SetThreadSignal@$
	//# \also	$@Signal@$


	//# \function	Thread::SetThreadSignal		Sets the signal associated with the thread.
	//
	//# \proto	void SetThreadSignal(Signal *signal);
	//
	//# \param	signal	The signal to assign to the thread.
	//
	//# \desc
	//# The $SetThreadSignal$ function assigns a $@Signal@$ object to a thread. When a $Thread$ object
	//# is destroyed and it has a $Signal$ object assigned to it, the thread triggers signal 0 to
	//# indicate that the thread should exit.
	//
	//# \also	$@Thread::GetThreadSignal@$
	//# \also	$@Signal@$


	//# \function	Thread::SetThreadPriority	Sets the priority of a thread.
	//
	//# \proto	void SetThreadPriority(int32 priority);
	//
	//# \param	priority	The new thread priority.
	//
	//# \desc
	//# The $SetThreadPriority$ function changes the priority of a thread to the value specified by the
	//# $priority$ parameter. This value can be one of the following constants.
	//
	//# \table	ThreadPriority
	//
	//# \also	$@Thread::Yield@$


	//# \function	Thread::Complete	Indicates whether a thread has completed execution.
	//
	//# \proto	bool Complete(void) const;
	//
	//# \desc
	//# The $Complete$ function returns $false$ while a thread is running, and it returns $true$
	//# after a thread has completed execution.


	//# \function	Thread::Sleep		Suspends the current thread for a period of time.
	//
	//# \proto	static void Sleep(unsigned_int32 time);
	//
	//# \param	time	The length of time to sleep, in milliseconds.
	//
	//# \desc
	//# The $Sleep$ function blocks the calling thread for the length of time specified by the
	//# $time$ parameter.
	//
	//# \also	$@Thread::Yield@$


	//# \function	Thread::Yield		Yields the time slice for the current thread.
	//
	//# \proto	static void Yield(void);
	//
	//# \desc
	//# The $Yield$ function causes the calling thread to give up the remainder of its time slice.
	//# The thread will continue running normally once the operating system schedules it for execution again.
	//
	//# \also	$@Thread::Sleep@$


	class Thread
	{
		friend class JobMgr;

		public:

			typedef void ThreadProc(const Thread *, void *);

		private:

			ThreadProc						*threadProc;
			void							*threadCookie;

			Signal							*threadSignal;
			volatile bool					threadComplete;

			#if C4WINDOWS

				HANDLE						threadHandle;
				DWORD						threadID;

				static DWORD				mainThreadID;

				static DWORD WINAPI ThreadEntry(void *cookie);

			#elif C4POSIX

				pthread_t					threadID;
				int							schedPolicy;
				sched_param					schedParam;

				static pthread_t			mainThreadID;

				static void *ThreadEntry(void *cookie);

			#elif C4PS4 //[ PS4

				// -- PS4 code hidden --

			#elif C4PS3 //[ PS3

				// -- PS3 code hidden --

			#endif //]

		public:

			C4API Thread(ThreadProc *proc, void *cookie, unsigned_int32 stack = 0, Signal *signal = nullptr);
			C4API ~Thread();

			Signal *GetThreadSignal(void) const
			{
				return (threadSignal);
			}

			void SetThreadSignal(Signal *signal)
			{
				threadSignal = signal;
			}

			bool Complete(void) const
			{
				return (threadComplete);
			}

			static bool MainThread(void)
			{
				#if C4WINDOWS

					return (GetCurrentThreadId() == mainThreadID);

				#elif C4POSIX

					return (pthread_equal(pthread_self(), mainThreadID) != 0);

				#elif C4PS4 //[ PS4

					// -- PS4 code hidden --

				#elif C4PS3 //[ PS3

					// -- PS3 code hidden --

				#endif //]
			}

			static void Sleep(unsigned_int32 time)
			{
				#if C4WINDOWS

					::Sleep(time);

				#elif C4POSIX

					usleep(time * 1000);

				#elif C4PS4 //[ PS4

					// -- PS4 code hidden --

				#elif C4PS3 //[ PS3

					// -- PS3 code hidden --

				#endif //]
			}

			static void Yield(void)
			{
				#if C4WINDOWS

					SwitchToThread();

				#elif C4POSIX

					sched_yield();

				#elif C4PS4 //[ PS4

					// -- PS4 code hidden --

				#elif C4PS3 //[ PS3

					// -- PS3 code hidden --

				#endif //]
			}

			static void Fence(void)
			{
				#if C4WINDOWS

					MemoryBarrier();

				#else

					__sync_synchronize();

				#endif
			}

			#if C4DEBUG

				C4API static void SetThreadName(const char *name);

			#else

				static void SetThreadName(const char *name)
				{
				}

			#endif

			C4API void SetThreadPriority(int32 priority) const;
	};


	//# \class	Job		Represents a single job.
	//
	//# The $Job$ class represents a single job that runs in one of the Job Manager's worker threads.
	//
	//# \def	class Job : public ListElement<Job>
	//
	//# \ctor	Job(ExecuteProc *execProc, void *cookie = nullptr);
	//
	//# \param	execProc	The main execution function for the job.
	//# \param	cookie		A pointer to user-defined data that is passed to the job's execution function.
	//
	//# \desc
	//# The $Job$ class is used to encapsulate the execution function and data for a single job. Jobs are queued for
	//# execution by one of the Job Manager's worker threads by calling the $@JobMgr::SubmitJob@$ function.
	//#
	//# The function specified by the $execProc$ parameter should have the following signature.
	//
	//# \code	typedef void ExecuteProc(Job *, void *);
	//
	//# The first parameter passed to the execution function is a pointer to the $Job$ object itself, and the second
	//# parameter is the $cookie$ parameter that was passed to the $Job$ constructor.
	//
	//# \base	Utilities/ListElement<Job>		Used internally by the Job Manager.
	//
	//# \also	$@BatchJob@$
	//# \also	$@JobMgr@$


	//# \function	Job::Complete		Returns a boolean value indicating whether a job has finished executing.
	//
	//# \proto	bool Complete(void) const;
	//
	//# \desc
	//# The $Complete$ function returns a boolean value indicating whether a job has finished executing.
	//
	//# \also	$@Job::Cancelled@$


	//# \function	Job::Cancelled		Returns a boolean value indicating whether a job has been cancelled.
	//
	//# \proto	bool Cancelled(void) const;
	//
	//# \desc
	//# The $Cancelled$ function returns a boolean value indicating whether a job has been cancelled by the
	//# $@JobMgr::CancelJob@$ function.
	//
	//# \also	$@Job::Complete@$


	//# \function	Job::GetThreadIndex		Returns the worker thread index for a job.
	//
	//# \proto	int32 GetThreadIndex(void) const;
	//
	//# \desc
	//# The $GetThreadIndex$ function returns the index of the worker thread on which the job is executing.
	//# The index is between 0 and <i>n</i>&nbsp;&minus;&nbsp;1, inclusive, where <i>n</i> is the number of worker
	//# threads returned by the $@JobMgr::GetWorkerThreadCount@$ function. The value of the thread index is valid
	//# only when the $GetThreadIndex$ function is called from within the job's execution function.
	//
	//# \also	$@JobMgr::GetWorkerThreadCount@$


	class Job : public ListElement<Job>
	{
		friend class JobMgr;

		public:

			typedef void ExecuteProc(Job *, void *);
			typedef void FinalizeProc(Job *, void *);

		private:

			ExecuteProc					*executeProc;
			FinalizeProc				*finalizeProc;
			void						*jobCookie;

			Batch						*jobBatch;

			unsigned_int32				jobFlags;
			volatile unsigned_int32		jobState;
			volatile int32				threadIndex;

			volatile int32				jobProgress;
			volatile int32				jobMagnitude;

		protected:

			Job(ExecuteProc *execProc, void *cookie, unsigned_int32 flags);
			Job(ExecuteProc *execProc, FinalizeProc *finalProc, void *cookie = nullptr, unsigned_int32 flags = 0);

		public:

			C4API Job(ExecuteProc *execProc, void *cookie = nullptr);
			C4API ~Job();

			ExecuteProc *GetExecuteProc(void) const
			{
				return (executeProc);
			}

			void SetExecuteProc(ExecuteProc *execProc)
			{
				executeProc = execProc;
			}

			FinalizeProc *GetFinalizeProc(void) const
			{
				return (finalizeProc);
			}

			void SetFinalizeProc(FinalizeProc *finalProc)
			{
				finalizeProc = finalProc;
			}

			unsigned_int32 GetJobFlags(void) const
			{
				return (jobFlags);
			}

			void SetJobFlags(unsigned_int32 flags)
			{
				jobFlags = flags;
			}

			bool Complete(void) const
			{
				return ((jobState & kJobComplete) != 0);
			}

			bool Cancelled(void) const
			{
				return ((jobState & kJobCancelled) != 0);
			}

			int32 GetThreadIndex(void) const
			{
				return (threadIndex);
			}

			int32 GetJobProgress(void) const
			{
				return (jobProgress);
			}

			void SetJobProgress(int32 progress)
			{
				jobProgress = progress;
			}

			int32 GetJobMagnitude(void) const
			{
				return (jobMagnitude);
			}

			void SetJobMagnitude(int32 magnitude)
			{
				jobMagnitude = magnitude;
			}

			void Execute(void)
			{
				(*executeProc)(this, jobCookie);
			}

			void Finalize(void)
			{
				(*finalizeProc)(this, jobCookie);
			}
	};


	//# \class	BatchJob		Represents a single job that can be part of a batch.
	//
	//# The $BatchJob$ class represents a single job that can be part of a batch.
	//
	//# \def	class BatchJob : public Job, public ListElement<BatchJob>
	//
	//# \ctor	BatchJob(ExecuteProc *execProc, void *cookie = nullptr, unsigned_int32 flags = 0);
	//# \ctor	BatchJob(ExecuteProc *execProc, FinalizeProc *finalProc, void *cookie = nullptr, unsigned_int32 flags = 0);
	//
	//# \param	execProc	The main execution function for the job.
	//# \param	finalProc	The finalization function for the job.
	//# \param	cookie		A pointer to user-defined data that is passed to the job's execution function.
	//# \param	flags		The job flags. See below for possible values.
	//
	//# \desc
	//# The $BatchJob$ class is used to encapsulate the execution function and data for a single job. Batch jobs are queued
	//# for execution by one of the Job Manager's worker threads by calling the $@JobMgr::SubmitJob@$ function and
	//# specifying a $@Batch@$ object.
	//#
	//# The functions specified by the $execProc$ and $finalProc$ parameters should have the following signatures.
	//
	//# \code	typedef void ExecuteProc(Job *, void *);
	//# \code	typedef void FinalizeProc(Job *, void *);
	//
	//# The first parameter passed to the execution function is a pointer to the $Job$ object itself, and the second
	//# parameter is the $cookie$ parameter that was passed to the $Job$ constructor.
	//#
	//# The $flags$ parameter can be a combination (through logical OR) of the following constants.
	//
	//# \table	JobFlags
	//
	//# If the $kJobNonpersistent$ flag is specified, then the $BatchJob$ object is automatically destroyed by the
	//# $@JobMgr::FinishBatch@$ function after a batch has finished executing.
	//
	//# \base	Job									A batch job is a specialized type of job.
	//# \base	Utilities/ListElement<BatchJob>		Used internally by the Job Manager.
	//
	//# \also	$@Batch@$
	//# \also	$@JobMgr@$


	class BatchJob : public Job, public ListElement<BatchJob>
	{
		public:

			C4API BatchJob(ExecuteProc *execProc, void *cookie = nullptr, unsigned_int32 flags = 0);
			C4API BatchJob(ExecuteProc *execProc, FinalizeProc *finalProc, void *cookie = nullptr, unsigned_int32 flags = 0);

			using ListElement<BatchJob>::Previous;
			using ListElement<BatchJob>::Next;
	};


	//# \class	Batch		Represents a batch of jobs.
	//
	//# The $Batch$ class represents a batch of jobs on which the main thread can wait before continuing execution.
	//
	//# \def	class Batch
	//
	//# \ctor	Batch();
	//
	//# \desc
	//# The $Batch$ class is a container for a group of $@BatchJob@$ objects. When a batch job is submitted to the Job Manager using
	//# the $@JobMgr::SubmitJob@$ function, the $Batch$ object can be specified as the second parameter. This causes the batch job
	//# to be added to the batch and queued for execution. The main thread can be blocked until all of the jobs in a batch have
	//# completed by calling the $@JobMgr::FinishBatch@$ function.
	//
	//# \also	$@BatchJob@$
	//# \also	$@JobMgr@$


	class Batch
	{
		friend class JobMgr;

		private:

			List<BatchJob>		jobPendingList;
			List<BatchJob>		jobFinishedList;

			volatile bool		batchActive;
			volatile bool		signalFlag;

			Signal				batchSignal;

		public:

			C4API Batch();
			C4API ~Batch();
	};


	//# \class	JobMgr		The Job Manager class.
	//
	//# \def	class JobMgr : public Manager<JobMgr>
	//
	//# \desc
	//# The $JobMgr$ class manages the multithreaded job processing capabilities of the C4 Engine.
	//# The single instance of the Job Manager is constructed during an application's initialization
	//# and destroyed at termination.
	//#
	//# The Job Manager's member functions are accessed through the global pointer $TheJobMgr$.
	//#
	//# \also	$@Job@$
	//# \also	$@BatchJob@$
	//# \also	$@Batch@$


	//# \function	JobMgr::GetWorkerThreadCount		Returns the total number of worker threads.
	//
	//# \proto	int32 GetWorkerThreadCount(void) const;
	//
	//# \desc
	//# The $GetWorkerThreadCount$ function returns the total number of worker threads in use by the Job Manager.
	//# This number does not change while the engine is running.
	//#
	//# The index of the thread on which a particular job is running can be determined by calling the
	//# $@Job::GetThreadIndex@$ function.
	//
	//# \also	$@Job::GetThreadIndex@$


	//# \function	JobMgr::SubmitJob		Submits a job for execution.
	//
	//# \proto	void SubmitJob(Job *job);
	//# \proto	void SubmitJob(BatchJob *job, Batch *batch);
	//
	//# \param	job		The job to submit.
	//# \param	batch	The batch to which the job is added.
	//
	//# \desc
	//# The $SubmitJob$ function submits the job specified by the $job$ parameter to the Job Manager for execution.
	//# The Job Manager adds the job to a queue, and it is executed by the next available worker thread. Worker threads
	//# take jobs from the queue in first-in-first-out order.
	//#
	//# If the $batch$ parameter is specified, then the $job$ parameter must specify a batch job. The job is added to the
	//# batch before the $SubmitJob$ function returns.
	//#
	//# No assumptions should be made about when a job starts executing. A job may or may not have begun executing
	//# before the $SubmitJob$ function returns.
	//#
	//# If the job specified by the $job$ parameter is already in the executing state when the $SubmitJob$ function is
	//# called, then the $SubmitJob$ function waits for the job to finish execution before queuing for execution again.
	//
	//# \also	$@JobMgr::CancelJob@$
	//# \also	$@JobMgr::CancelJobArray@$
	//# \also	$@JobMgr::FinishBatch@$


	//# \function	JobMgr::CancelJob		Cancels a single job.
	//
	//# \proto	void CancelJob(Job *job);
	//
	//# \param	job		The job to cancel.
	//
	//# \desc
	//# The $CancelJob$ function cancels the job specified by the $job$ parameter. If the job is in the executing state
	//# when this function is called, then a flag is set in the $Job$ object. The job's execution function must call
	//# the $@Job::Cancelled@$ function to be aware of the cancellation, and it should exit early when it has been cancelled.
	//# (Jobs are not forced to exit by the Job Manager.)
	//#
	//# If the job has not yet begun executing when this function is called, then it is simply removed from the execution queue.
	//
	//# \also	$@JobMgr::CancelJobArray@$
	//# \also	$@JobMgr::SubmitJob@$
	//# \also	$@JobMgr::FinishBatch@$


	//# \function	JobMgr::CancelJobArray		Cancels one or more jobs.
	//
	//# \proto	void CancelJobArray(int32 count, Job **jobArray);
	//
	//# \param	count		The number of jobs to cancel.
	//# \param	jobArray	An array of pointers to the $@Job@$ objects to be cancelled.
	//
	//# \desc
	//# The $CancelJobArray$ function cancels the $count$ jobs in the array specified by the $jobArray$ parameter.
	//# The cancellation process is identical to that described for the $@JobMgr::CancelJob@$ function.
	//
	//# \also	$@JobMgr::CancelJob@$
	//# \also	$@JobMgr::SubmitJob@$
	//# \also	$@JobMgr::FinishBatch@$


	//# \function	JobMgr::FinishBatch		Waits for the completion of a batch.
	//
	//# \proto	void FinishBatch(Batch *batch);
	//
	//# \param	batch		The batch to wait on.
	//
	//# \desc
	//# The $FinishBatch$ function blocks the calling thread and waits for all of the jobs belonging to the batch
	//# specified by the $batch$ parameter to complete. If no jobs had been submitted in the batch since it was
	//# constructed or since the last call to $FinishBatch$ for the same batch, then this function returns immediately.
	//#
	//# After all of the jobs in the batch have completed, each job's finalization function is called, if it has one.
	//# Nonpersistent job objects in the batch are destroyed before the $FinishBatch$ function returns.
	//
	//# \also	$@JobMgr::SubmitJob@$
	//# \also	$@JobMgr::CancelJob@$
	//# \also	$@JobMgr::CancelJobArray@$


	class JobMgr : public Manager<JobMgr>
	{
		public:

			enum
			{
				kMaxWorkerThreadCount = 16
			};

			static_assert(kMaxWorkerThreadCount < 32, "kMaxWorkerThreadCount must be less than 32");

		private:

			struct Worker : ListElement<Worker>
			{
				int32		threadIndex;

				Signal		signal;
				Thread		thread;

				Worker(int32 index, Thread::ThreadProc *proc, void *cookie);
				~Worker();
			};

			static int32			reservedProcessorCount;

			volatile bool			exitFlag;
			Mutex					jobMutex;

			List<Worker>			workerWaitList;
			List<Job>				jobReadyList;
			List<Job>				jobExecuteList;

			int32					workerThreadCount;
			char					workerStorage[sizeof(Worker) * kMaxWorkerThreadCount];

			Worker *GetWorker(int32 index)
			{
				return (&reinterpret_cast<Worker *>(workerStorage)[index]);
			}

			static void WorkerThread(const Thread *thread, void *cookie);

			static void ProcessJobBatch(Job *job);

		public:

			JobMgr(int);
			~JobMgr();

			EngineResult Construct(void);
			void Destruct(void);

			static void SetReservedProcessorCount(int32 count)
			{
				reservedProcessorCount = count;
			}

			int32 GetWorkerThreadCount(void) const
			{
				return (workerThreadCount);
			}

			C4API void SubmitJob(Job *job);
			C4API void SubmitJob(BatchJob *job, Batch *batch);

			C4API void CancelJob(Job *job);
			C4API void CancelJobArray(int32 count, Job **jobArray);

			C4API static void FinishJob(Job *job);

			C4API void FinishBatch(Batch *batch);
	};


	C4API extern JobMgr *TheJobMgr;
}


#endif

// ZYUQURM
