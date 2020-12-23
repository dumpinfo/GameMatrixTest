#pragma once

#include "../NewtonDynamicsPhysicsDriver.h"

namespace Leadwerks
{
	class NewtonDynamicsBody : public Body
	{
	public:
		NewtonBody* body;
		NewtonCollision* collision;
		std::list<NewtonDynamicsBody*>::iterator updatelink;
		std::list<NewtonDynamicsBody*>::iterator activelink[NewtonDynamicsMaxThreadCount];
		std::vector<NewtonCollision*> collisionmodifier;
		bool updateneeded;
		bool updateshapeneeded;
		bool updatematrixneeded;
        bool updatescaleneeded;
		bool updatemassneeded;
		bool updatevelocityneeded;
		bool updateshapematrixneeded;
		bool active;
		int lastupdatetick;
		bool sleepstate;
		bool protectcollision;
		bool updatecollisiontypeneeded;
		int requestedcollisiontype;
		bool updatejointtypeneeded;
        bool requestedgravitymode;
		Vec3 velocity;
		Vec3 omega;
		float staticfriction;
		float kineticfriction;
		float softness;
		float elasticity;
		float requestedsoftness;
		float requestedstaticfriction;
		float requestedkineticfriction;
		float requestedelasticity;
		bool forceawake;
		bool updateomeganeeded;
		Vec3 requestedvelocity;
		Vec3 requestedomega;
		bool addedtoactivelist;
		std::list<NewtonDynamicsBody*>::iterator activebodieslink;
		bool forcesetshape;
		bool massmatrixdefined;
		float massmatrix[6];

		NewtonDynamicsBody();
		virtual ~NewtonDynamicsBody();
		
		virtual void Wake();
		virtual void AddBuoyancyForce(const float nx, const float ny, const float nz, const float d, const float fluidDensity, const float fluidViscosity);
		virtual bool BodyRequired();
		virtual void SetForce(const Vec3& force);
		virtual void SetTorque(const Vec3& torque);
		virtual void UpdateJointState();
		virtual void SetVelocity(const float x, const float y, const float z);
		virtual void SetOmega(const float x, const float y, const float z);
		virtual void SetFriction(const float staticfriction, const float kineticfriction);
		virtual Vec2 GetFriction();
		virtual Vec3 GetOmega();
		virtual void SetGravityMode(const bool mode);
        /*virtual void SetJointType(const int jointtype);
        virtual void SetJointPosition(const float x, const float y, const float z);
        virtual void SetJointPin(const float x, const float y, const float z);
        virtual void SetJointLimits(const float limit0, const float limit1);*/
        virtual void SetSweptCollisionMode(const bool mode);
		virtual bool GetSweptCollisionMode();
		virtual void SetCollisionType(const int collisiontype);
		virtual void AddForce(const Vec3& force);
		virtual void AddTorque(const Vec3& torque);
		virtual bool ProcessCollision(NewtonDynamicsBody* body, const float* position, const float* normal, const float speed, const int threadindex);
		virtual bool Awake();
		virtual bool Active();
		virtual void Free();
		//virtual void SetVelocity(const Vec3& velocity);
		virtual void SetMass(const float mass);
		virtual void SetMass(const float mass, const float cx, const float cy, const float cz, const float ixx, const float iyy, const float izz);
		virtual void SetMatrix(const Mat4& mat, const Quat& quat);
		virtual void AddToUpdateList();
		virtual void Update();
		virtual void SetScale(const Vec3& scale);
		virtual void SetShape(Shape* shape,const Mat4& mat);
		//virtual void AddShape(Shape* shape,const Mat4& mat);
        virtual Vec3 GetVelocity();
		virtual Vec3 GetPointVelocity(const Vec3& point);

        static void NewtonCompoundCollisionSetScale(NewtonCollision* const compoundCollision, const float x, const float y, const float z);
        static void NewtonStaticBodySetFreezeState(const NewtonBody* staticBody, const int state);
	};
	
	void NewtonApplyForceAndTorque(const NewtonBody* body, dFloat timestep, int threadIndex);
	void NewtonSetTransform(const NewtonBody* body, const dFloat* matrix, int threadIndex);
	int NewtonOnAABBOverlap(const NewtonMaterial* material, const NewtonBody* body0, const NewtonBody* body1, int threadIndex);
	void NewtonContactsProcess(const NewtonJoint* const contact, dFloat timestep, int threadIndex);
}
