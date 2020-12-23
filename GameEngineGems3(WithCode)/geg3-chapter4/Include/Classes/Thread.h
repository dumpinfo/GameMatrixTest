#pragma once

#include "../Leadwerks.h"

#ifdef _WIN32
#include <process.h>
namespace win32
{
	//#include <windows.h>
}
#else
#include <pthread.h>
#endif

namespace Leadwerks
{
	const int THREAD_RUNNING = 1;
	const int THREAD_FINISHED = 0;
	const int THREAD_PAUSED = 2;
	const int THREAD_READY = 3;

    class Mutex;
	class Object;
	
	class Thread : public Object
	{
	public:
        Object* argument;
		Object* result;
		int state;
#ifdef _WIN32
		HANDLE id;
		//uintptr_t id;
#else
		pthread_t thread;
#endif
		Mutex* statemutex;
		
		Thread();
		virtual ~Thread();
		
        static Thread* Create(Object* EntryPoint(Object* o), Object* o=NULL);
        Object* (*entrypoint)(Object* o);
		virtual void Wait();
		//virtual void Resume();
		//virtual void Pause();
		virtual int GetState();
		virtual Object* GetResult();
		virtual std::string GetClassName();

		static const int Running;
		static const int Finished;
		static const int Paused;
		static const int Ready;
	};
	
#ifndef _WIN32
    void *thread_function(void *ptr);
#endif
}
