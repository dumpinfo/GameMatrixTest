#pragma once

#include "../Leadwerks.h"

namespace Leadwerks
{
	class Mutex : public Object
	{
	public:
#ifdef WINDOWS
		CRITICAL_SECTION cs;
#else
        pthread_mutex_t* mutex;
#endif
		
		Mutex();
		virtual ~Mutex();
		
		virtual void Lock();
		virtual void Unlock();
		virtual std::string GetClassName();
		
		static Mutex* Create();
	};
}
