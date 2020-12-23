#pragma once

#include "../Leadwerks.h"

namespace Leadwerks
{
	class Simulation : public Object
	{
	public:
		PhysicsDriver* driver;
		World* world;
        int updatecount;

		Simulation();
		virtual ~Simulation();
		
		virtual void SerializeToFile(const std::string filename)=0;
		virtual void SetPhysicsDetail(const int mode)=0;
		virtual void Update()=0;
		virtual int GetMaxThreads()=0;
		virtual int GetThreadCount()=0;
		virtual void SetThreadCount(const int threads)=0;
	};
}
