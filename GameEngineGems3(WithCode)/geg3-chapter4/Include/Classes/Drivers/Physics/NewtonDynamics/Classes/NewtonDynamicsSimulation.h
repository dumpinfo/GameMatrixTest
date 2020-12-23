#pragma once

#include "../NewtonDynamicsPhysicsDriver.h"

namespace Leadwerks
{
	const int NewtonDynamicsMaxThreadCount = 8;
	
	class NewtonDynamicsBody;
	class Thread;
	class NewtonDynamicsCharacterController;
    class NewtonDynamicsJoint;
    class Collision;

	class NewtonDynamicsSimulation : public Simulation
	{
	public:
		Thread* thread;
		NewtonWorld* newtonworld;
		std::list<NewtonDynamicsBody*> updatelist;
		std::list<NewtonDynamicsBody*> activebodies[NewtonDynamicsMaxThreadCount];
		std::list<NewtonDynamicsBody*> lastupdateactivebodies;
		std::list<NewtonDynamicsBody*> freedbodies;
        std::list<NewtonDynamicsBody*> freedjoints;
        std::list<NewtonDynamicsJoint*> updatejointslist;
		int threadcount;
		double lastupdatestarttime;
		double accumulatedtime;
		CustomVehicleControllerManager* manager;
		
		NewtonDynamicsSimulation();
		virtual ~NewtonDynamicsSimulation();
		
		virtual void SerializeToFile(const std::string filename);
		virtual void SetPhysicsDetail(const int mode);
		virtual void BeginStep();
		virtual void EndStep();
		virtual void InterpolateBodies(const bool clearactive);
		virtual void Push();
		virtual void Pull();
		virtual void Update();

		virtual int GetMaxThreads();
		virtual int GetThreadCount();
		virtual void SetThreadCount(const int threads);
	};

	//DWORD WINAPI UpdateNewtonDynamicsSimulation( LPVOID lpParam );
	Object* UpdateNewtonDynamicsSimulation(Object* o);
}
