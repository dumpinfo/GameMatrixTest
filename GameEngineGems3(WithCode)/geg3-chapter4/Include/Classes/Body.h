#pragma once

#include "../Leadwerks.h"

namespace Leadwerks
{
	class Shape;
	class World;
	class Simulation;
	class Joint;
    
	//A body has a 4x4 matrix, but is not an entity.  Bodies are attached to entities.
	//This eliminates a lot of redundant entity hierarchy.
	class Body
	{
	public:
		PhysicsDriver* driver;
		World* world;
		float mass;
		std::list<Body*>::iterator link;
		AABB aabb;
		Vec3 scale;
		Mat4 nextmat;
		Mat4 prevmat;
		Quat prevquat;
		Quat nextquat;
		Simulation* simulation;
		Entity* entity;
		Shape* shape;
		Mat4 shapematrix;
        //std::vector<Mat4> shapematrix;
		Vec3 force;
		Vec3 torque;
		bool gravitymode;
        int collisiontype;
        bool sweptcollisionmode;
		Vec3 centerofmass;
        /*int jointtype;
        Vec3 jointposition;
        Vec3 jointpin;
        Vec2 jointlimits;*/
        
		Body();
		virtual ~Body();
		
        //Joint functions
        /*virtual void SetJointType(const int jointtype)=0;
        virtual int GetJointType();
        virtual void SetJointPosition(const float x, const float y, const float z)=0;
        virtual Vec3 GetJointPosition();
        virtual void SetJointPin(const float x, const float y, const float z)=0;
        virtual Vec3 GetJointPin();
        virtual void SetJointLimits(const float limit0, const float limit1)=0;
        virtual Vec2 GetJointLimits();*/
        
		virtual void AddBuoyancyForce(const float nx, const float ny, const float nz, const float d, const float fluidDensity, const float fluidViscosity)=0;
		virtual void SetForce(const Vec3& force)=0;
		virtual void SetTorque(const Vec3& torque)=0;
		virtual void UpdateJointState()=0;
		virtual void SetFriction(const float staticfriction, const float kineticfriction)=0;
		virtual Vec2 GetFriction()=0;
		virtual Vec3 GetOmega()=0;
		virtual void SetGravityMode(const bool mode)=0;
        virtual bool GetGravityMode();
		virtual void SetSweptCollisionMode(const bool mode)=0;
		virtual bool GetSweptCollisionMode()=0;
        virtual void SetCollisionType(const int collisiontype)=0;
		virtual void AddForce(const Vec3& force)=0;
		virtual void AddTorque(const Vec3& torque)=0;
		virtual bool Awake()=0;
		virtual bool Active()=0;
		virtual void Free();
		virtual void SetShape(Shape* shape,const Mat4& mat)=0;
		//virtual void AddShape(Shape* shape,const Mat4& mat)=0;
		virtual Vec3 GetVelocity()=0;
		virtual void SetMass(const float mass)=0;
		virtual void SetMass(const float mass, const float cx, const float cy, const float cz, const float ixx, const float iyy, const float izz) = 0;
		//virtual float GetMass()=0;
		virtual void SetScale(const Vec3& scale)=0;
		virtual void SetMatrix(const Mat4& mat, const Quat& quat)=0;
		//virtual Mat4 GetMatrix()=0;
		virtual void SetVelocity(const float x, const float y, const float z)=0;
		virtual void SetOmega(const float x, const float y, const float z)=0;
		virtual Vec3 GetPointVelocity(const Vec3& point)=0;
	};
}
