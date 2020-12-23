 

#include "C4Threads.h"
#include "C4Engine.h"


using namespace C4;


namespace
{
	enum
	{
		kThreadDefaultStackSize		= 131072
	};
}


JobMgr *C4::TheJobMgr = nullptr;


namespace C4
{
	template <> JobMgr Manager<JobMgr>::managerObject(0);
	template <> JobMgr **Manager<JobMgr>::managerPointer = &TheJobMgr;

	template <> const char *const Manager<JobMgr>::resultString[] =
	{
		nullptr
	};

	template <> const unsigned_int32 Manager<JobMgr>::resultIdentifier[] =
	{
		0
	};

	template class Manager<JobMgr>;
}


#if C4WINDOWS

	DWORD Thread::mainThreadID;

#elif C4POSIX

	pthread_t Thread::mainThreadID;

#elif C4PS4 //[ PS4

	// -- PS4 code hidden --

#elif C4PS3 //[ PS3

	// -- PS3 code hidden --

#endif //]


int32 JobMgr::reservedProcessorCount = 0;


Lock::Lock()
{
	lockCount = 0;
}

void Lock::AcquireExclusive(void)
{
	for (;;)
	{
		lockMutex.Acquire();

		if (lockCount == 0)
		{
			break;
		}

		lockMutex.Release();
		Thread::Yield();
	}
}

void Lock::AcquireShared(void)
{
	lockMutex.Acquire();
	lockCount++;
	lockMutex.Release();
}

void Lock::ReleaseShared(void)
{
	lockMutex.Acquire();
	lockCount--;
	lockMutex.Release();
}


Signal::Signal(int32 count)
{
	#if C4WINDOWS

		signalCount = count; 
		for (machine a = 0; a < count; a++)
		{
			signalEvent[a] = CreateEventA(nullptr, false, false, nullptr); 
		}
 
	#elif C4POSIX

		pthread_cond_init(&condID, nullptr); 
		pthread_mutex_init(&mutexID, nullptr);
 
		signalValue = 0; 

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden -- 

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]
}

Signal::~Signal()
{
	#if C4WINDOWS

		for (machine a = signalCount - 1; a >= 0; a--)
		{
			CloseHandle(signalEvent[a]);
		}

	#elif C4POSIX

		pthread_mutex_destroy(&mutexID);
		pthread_cond_destroy(&condID);

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]
}

#if C4WINDOWS

	int32 Signal::Wait(int32 time)
	{
		if (time < 0)
		{
			if (signalCount == 1)
			{
				WaitForSingleObjectEx(signalEvent[0], INFINITE, false);
				return (0);
			}

			return (WaitForMultipleObjectsEx(signalCount, signalEvent, false, INFINITE, false) - WAIT_OBJECT_0);
		}

		if (signalCount == 1)
		{
			if (WaitForSingleObjectEx(signalEvent[0], time, false) == WAIT_TIMEOUT)
			{
				return (kSignalTimeout);
			}

			return (0);
		}

		DWORD result = WaitForMultipleObjectsEx(signalCount, signalEvent, false, time, false);
		if (result == WAIT_TIMEOUT)
		{
			return (kSignalTimeout);
		}

		return (result - WAIT_OBJECT_0);
	}

#elif C4POSIX

	void Signal::Trigger(int32 index)
	{
		pthread_mutex_lock(&mutexID);

		signalValue |= 1 << index;
		pthread_cond_signal(&condID);

		pthread_mutex_unlock(&mutexID);
	}

	int32 Signal::Wait(int32 time)
	{
		int32 result = kSignalTimeout;
		pthread_mutex_lock(&mutexID);

		if (time < 0)
		{
			for (;;)
			{
				unsigned_int32 value = signalValue;
				if (value != 0)
				{
					result = 0;
					unsigned_int32 i = 1;
					while ((value & i) == 0)
					{
						result++;
						i <<= 1;
					}

					signalValue = value & ~i;
					break;
				}

				pthread_cond_wait(&condID, &mutexID);
			}
		}
		else
		{
			timespec	spec;

			#if C4LINUX

				clock_gettime(CLOCK_REALTIME, &spec);

				int32 sec = spec.tv_sec + time / 1000;
				int32 nsec = spec.tv_nsec + (time - sec * 1000) * 1000000;
				if (nsec > 1000000000)
				{
					sec++;
					nsec -= 1000000000;
				}

				spec.tv_sec = sec;
				spec.tv_nsec = nsec;

			#else

				timeval		val;

				gettimeofday(&val, nullptr);

				int32 sec = val.tv_sec + time / 1000;
				int32 usec = val.tv_usec + (time - sec * 1000) * 1000;
				if (usec > 1000000)
				{
					sec++;
					usec -= 1000000;
				}

				spec.tv_sec = sec;
				spec.tv_nsec = usec * 1000;

			#endif

			for (;;)
			{
				unsigned_int32 value = signalValue;
				if (value != 0)
				{
					result = 0;
					unsigned_int32 i = 1;
					while ((value & i) == 0)
					{
						result++;
						i <<= 1;
					}

					signalValue = value & ~i;
					break;
				}

				if (pthread_cond_timedwait(&condID, &mutexID, &spec) == ETIMEDOUT)
				{
					break;
				}
			}
		}

		pthread_mutex_unlock(&mutexID);
		return (result);
	}

#elif C4PS4 //[ PS4

	// -- PS4 code hidden --

#elif C4PS3 //[ PS3

	// -- PS3 code hidden --

#endif //]


Thread::Thread(ThreadProc *proc, void *cookie, unsigned_int32 stack, Signal *signal)
{
	threadProc = proc;
	threadCookie = cookie;

	threadSignal = signal;
	threadComplete = false;

	if (stack == 0)
	{
		stack = kThreadDefaultStackSize;
	}

	#if C4WINDOWS

		threadHandle = CreateThread(nullptr, stack, &ThreadEntry, this, 0, &threadID);

	#elif C4POSIX

		pthread_attr_t		attr;

		pthread_attr_init(&attr);
		pthread_attr_getschedpolicy(&attr, &schedPolicy);
		pthread_attr_getschedparam(&attr, &schedParam);

		pthread_attr_setstacksize(&attr, stack);
		pthread_create(&threadID, &attr, &ThreadEntry, this);

		pthread_attr_destroy(&attr);

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]
}

Thread::~Thread()
{
	if ((threadSignal) && (!threadComplete))
	{
		threadSignal->Trigger();
	}

	#if C4WINDOWS

		WaitForSingleObjectEx(threadHandle, INFINITE, false);
		CloseHandle(threadHandle);

	#elif C4POSIX

		pthread_join(threadID, nullptr);

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]
}

#if C4WINDOWS

	DWORD WINAPI Thread::ThreadEntry(void *cookie)
	{
		#if C4SIMD && C4SSE

			_mm_setcsr(_mm_getcsr() | 0x8040);

		#endif

		Thread *thread = static_cast<Thread *>(cookie);
		(*thread->threadProc)(thread, thread->threadCookie);

		Fence();
		thread->threadComplete = true;
		return (0);
	}

#elif C4POSIX

	void *Thread::ThreadEntry(void *cookie)
	{
		#if C4SIMD && C4SSE

			_mm_setcsr(_mm_getcsr() | 0x8040);

		#endif

		Thread *thread = static_cast<Thread *>(cookie);
		(*thread->threadProc)(thread, thread->threadCookie);

		Fence();
		thread->threadComplete = true;
		return (nullptr);
	}

#elif C4PS4 //[ PS4

	// -- PS4 code hidden --

#elif C4PS3 //[ PS3

	// -- PS3 code hidden --

#endif //]

#if C4DEBUG

	void Thread::SetThreadName(const char *name)
	{
		#if C4WINDOWS

			ULONG_PTR	args[4];

			args[0] = 0x1000;
			args[1] = reinterpret_cast<ULONG_PTR>(name);
			args[2] = ~ULONG_PTR(0);
			args[3] = 0;

			__try
			{
				RaiseException(0x406D1388, 0, 4, args);
			}
			__except(1)
			{
			}

		#elif C4MACOS || C4IOS

			pthread_setname_np(name);

		#elif C4LINUX

			pthread_setname_np(pthread_self(), name);

		#elif C4PS4 //[ PS4

			// -- PS4 code hidden --

		#elif C4PS3 //[ PS3

			// -- PS3 code hidden --

		#endif //]
	}

#endif

void Thread::SetThreadPriority(int32 priority) const
{
	#if C4WINDOWS

		switch (priority)
		{
			case kThreadPriorityLow:

				::SetThreadPriority(threadHandle, THREAD_PRIORITY_LOWEST);
				break;

			case kThreadPriorityNormal:

				::SetThreadPriority(threadHandle, THREAD_PRIORITY_NORMAL);
				break;

			case kThreadPriorityHigh:

				::SetThreadPriority(threadHandle, THREAD_PRIORITY_HIGHEST);
				break;

			case kThreadPriorityCritical:

				::SetThreadPriority(threadHandle, THREAD_PRIORITY_TIME_CRITICAL);
				break;
		}

	#elif C4POSIX

		sched_param param = schedParam;

		switch (priority)
		{
			case kThreadPriorityLow:

				param.sched_priority = (param.sched_priority + sched_get_priority_min(schedPolicy)) >> 1;
				pthread_setschedparam(threadID, schedPolicy, &param);
				break;

			case kThreadPriorityNormal:

				pthread_setschedparam(threadID, schedPolicy, &param);
				break;

			case kThreadPriorityHigh:

				param.sched_priority = (param.sched_priority + sched_get_priority_max(schedPolicy)) >> 1;
				pthread_setschedparam(threadID, schedPolicy, &param);
				break;

			case kThreadPriorityCritical:

				param.sched_priority = sched_get_priority_max(schedPolicy);
				pthread_setschedparam(threadID, schedPolicy, &param);
				break;
		}

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]
}


Job::Job(ExecuteProc *execProc, void *cookie)
{
	executeProc = execProc;
	finalizeProc = nullptr;
	jobCookie = cookie;

	jobBatch = nullptr;

	jobFlags = 0;
	jobState = 0;

	jobProgress = 0;
	jobMagnitude = 1;
}

Job::Job(ExecuteProc *execProc, void *cookie, unsigned_int32 flags)
{
	executeProc = execProc;
	finalizeProc = nullptr;
	jobCookie = cookie;

	jobBatch = nullptr;

	jobFlags = flags;
	jobState = 0;

	jobProgress = 0;
	jobMagnitude = 1;
}

Job::Job(ExecuteProc *execProc, FinalizeProc *finalProc, void *cookie, unsigned_int32 flags)
{
	executeProc = execProc;
	finalizeProc = finalProc;
	jobCookie = cookie;

	jobBatch = nullptr;

	jobFlags = flags;
	jobState = 0;

	jobProgress = 0;
	jobMagnitude = 1;
}

Job::~Job()
{
	if (jobState & kJobExecuting)
	{
		TheJobMgr->CancelJob(this);
		do
		{
			Thread::Yield();
		} while (jobState & kJobExecuting);
	}
}


BatchJob::BatchJob(ExecuteProc *execProc, void *cookie, unsigned_int32 flags) : Job(execProc, cookie, flags)
{
}

BatchJob::BatchJob(ExecuteProc *execProc, FinalizeProc *finalProc, void *cookie, unsigned_int32 flags) : Job(execProc, finalProc, cookie, flags)
{
}


Batch::Batch()
{
	batchActive = false;
	signalFlag = false;
}

Batch::~Batch()
{
}


JobMgr::JobMgr(int)
{
	#if C4WINDOWS

		Thread::mainThreadID = GetCurrentThreadId();

	#elif C4POSIX

		Thread::mainThreadID = pthread_self();

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]
}

JobMgr::~JobMgr()
{
}

JobMgr::Worker::Worker(int32 index, Thread::ThreadProc *proc, void *cookie) : thread(proc, cookie, 0, &signal)
{
	threadIndex = index;
}

JobMgr::Worker::~Worker()
{
}

EngineResult JobMgr::Construct(void)
{
	exitFlag = false;

	int32 count = Min(Max(TheEngine->GetProcessorCount() - reservedProcessorCount, 1), kMaxWorkerThreadCount);
	workerThreadCount = count;

	for (machine a = 0; a < count; a++)
	{
		Worker *worker = GetWorker(a);
		new(worker) Worker(a, &WorkerThread, worker);
		workerWaitList.Append(worker);
	}

	return (kEngineOkay);
}

void JobMgr::Destruct(void)
{
	exitFlag = true;

	for (machine a = workerThreadCount - 1; a >= 0; a--)
	{
		GetWorker(a)->~Worker();
	}
}

void JobMgr::WorkerThread(const Thread *thread, void *cookie)
{
	Thread::SetThreadName("C4-JB Worker");

	Worker *worker = static_cast<Worker *>(cookie);
	JobMgr *jobMgr = TheJobMgr;

	for (;;)
	{
		thread->GetThreadSignal()->Wait();
		if (jobMgr->exitFlag)
		{
			break;
		}

		jobMgr->jobMutex.Acquire();

		for (;;)
		{
			Job *job = jobMgr->jobReadyList.First();
			if (!job)
			{
				break;
			}

			job->jobState = kJobExecuting;
			job->threadIndex = worker->threadIndex;
			jobMgr->jobExecuteList.Append(job);

			jobMgr->jobMutex.Release();
			job->Execute();
			jobMgr->jobMutex.Acquire();

			jobMgr->jobExecuteList.Remove(job);
			ProcessJobBatch(job);

			job->jobState = (job->jobState & ~kJobExecuting) | kJobComplete;
		}

		jobMgr->workerWaitList.Append(worker);
		jobMgr->jobMutex.Release();
	}
}

void JobMgr::ProcessJobBatch(Job *job)
{
	Batch *batch = job->jobBatch;
	if (batch)
	{
		job->jobBatch = nullptr;

		BatchJob *batchJob = static_cast<BatchJob *>(job);
		if (batchJob->ListElement<BatchJob>::GetOwningList() == &batch->jobPendingList)
		{
			batch->jobFinishedList.Append(batchJob);

			if ((batch->signalFlag) && (batch->jobPendingList.Empty()))
			{
				batch->batchSignal.Trigger();
			}
		}
	}
}

void JobMgr::SubmitJob(Job *job)
{
	jobMutex.Acquire();

	if (job->jobState & kJobExecuting)
	{
		jobMutex.Release();
		FinishJob(job);
		jobMutex.Acquire();
	}

	job->jobState = 0;
	jobReadyList.Append(job);

	Worker *worker = workerWaitList.First();
	if (worker)
	{
		workerWaitList.Remove(worker);
		worker->signal.Trigger();
	}

	jobMutex.Release();
}

void JobMgr::SubmitJob(BatchJob *job, Batch *batch)
{
	jobMutex.Acquire();

	if (job->jobState & kJobExecuting)
	{
		jobMutex.Release();
		FinishJob(job);
		jobMutex.Acquire();
	}

	job->jobBatch = batch;
	job->jobState = 0;

	jobReadyList.Append(job);
	batch->jobPendingList.Append(job);

	Thread::Fence();
	batch->batchActive = true;

	Worker *worker = workerWaitList.First();
	if (worker)
	{
		workerWaitList.Remove(worker);
		worker->signal.Trigger();
	}

	jobMutex.Release();
}

void JobMgr::CancelJob(Job *job)
{
	jobMutex.Acquire();

	job->jobState |= kJobCancelled;
	if (job->GetOwningList() == &jobReadyList)
	{
		jobReadyList.Remove(job);
		ProcessJobBatch(job);
	}

	jobMutex.Release();
}

void JobMgr::CancelJobArray(int32 count, Job **jobArray)
{
	jobMutex.Acquire();

	for (machine a = 0; a < count; a++)
	{
		Job *job = jobArray[a];
		job->jobState |= kJobCancelled;
		if (job->GetOwningList() == &jobReadyList)
		{
			jobReadyList.Remove(job);
			ProcessJobBatch(job);
		}
	}

	jobMutex.Release();
}

void JobMgr::FinishJob(Job *job)
{
	while (!job->Complete())
	{
		Thread::Yield();
	}
}

void JobMgr::FinishBatch(Batch *batch)
{
	while (batch->batchActive)
	{
		batch->batchActive = false;

		jobMutex.Acquire();
		batch->signalFlag = true;
		bool empty = batch->jobPendingList.Empty();
		jobMutex.Release();

		if (!empty)
		{
			batch->batchSignal.Wait();
		}

		batch->signalFlag = false;

		for (;;)
		{
			jobMutex.Acquire();
			BatchJob *job = batch->jobFinishedList.First();
			if (!job)
			{
				jobMutex.Release();
				break;
			}

			batch->jobFinishedList.Remove(job);
			jobMutex.Release();

			if (job->GetFinalizeProc())
			{
				job->Finalize();
			}

			if (job->GetJobFlags() & kJobNonpersistent)
			{
				delete job;
			}
		}
	}
}

// ZYUQURM
