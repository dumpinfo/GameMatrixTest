#pragma once

#include "../NewtonDynamicsPhysicsDriver.h"

namespace Leadwerks
{
	class NewtonDynamicsCharacterController : public CharacterController
	{
	public:
        Vec3 direction[2];
        Vec3 velocity;
        std::vector<Entity*> adjacentbodies;
        NewtonBody* newtonbody;
        Vec3 groundvelocity;
		Vec3 groundnormal;
		int currentiteration;
		bool keepawake;
		//CustomPlayerControllerManager* newtoncontroller;

        NewtonDynamicsCharacterController();
        virtual ~NewtonDynamicsCharacterController();
        
		virtual Vec3 GetVelocity(const bool global=true);
        virtual void Update();
        virtual void SetMass(const float mass);
        virtual float GetMass();
        virtual bool PickGroundConvex(const Vec3& p0, const Vec3& p1, PickInfo& result);
        virtual bool AlignToGround();
        virtual void UpdateCollision();
        virtual bool ProcessCollisions(const Vec3& p0, const Vec3& p1, PickInfo& result);
        virtual void GetAdjacentBodies();        
        virtual void AddForce(const Vec3& force);
        virtual void AddTorque(const Vec3& torque, const bool global=true);
        virtual Vec3 GetVelocity();
        
        static int contacts;
        static NewtonWorldConvexCastReturnInfo contactdata[100];
        static dFloat hitParam[100];
        static Vec3 PickPosition0;
        static Vec3 PickPosition1;
        static bool canceldeltay;
        static void ForEachEntityInAABBDoCallback(Entity* entity, Object* extra);

        static dFloat NewtonWorldRayFilterCallback(const NewtonBody* const body, const NewtonCollision* const shapeHit, const dFloat* const hitContact, const dFloat* const hitNormal, dLong collisionID, void* const userData, dFloat intersectParam);
		//typedef dFloat (*NewtonWorldRayFilterCallback)(const NewtonBody* const body, const NewtonCollision* const shapeHit, const dFloat* const hitContact, const dFloat* const hitNormal, int* const collisionID, void* const userData, dFloat intersectParam);
		//typedef dFloat (*NewtonWorldRayFilterCallback)(const NewtonBody* const body, const NewtonCollision* const shapeHit, const dFloat* const hitContact, const dFloat* const hitNormal, int* const collisionID, void* const userData, dFloat intersectParam);
        static unsigned int NewtonWorldRayPrefilterCallback(const NewtonBody* newtonbody, const NewtonCollision* newtoncollision, void* userData);
		//typedef unsigned (*NewtonWorldRayPrefilterCallback)(const NewtonBody* const body, const NewtonCollision* const collision, void* const userData);
		static int NewtonBodyIterator(const NewtonBody* const newtonbody, void* const userData);
	};
}
