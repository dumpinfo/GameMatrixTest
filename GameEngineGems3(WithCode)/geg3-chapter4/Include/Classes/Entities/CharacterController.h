#pragma once

#include "../../Leadwerks.h"

namespace Leadwerks
{
	class CharacterController : public Object
	{
    public:
        Entity* entity;
        float angle;
        bool isairborne;
        bool iscrouched;
        Vec3 movement;
        float maxacceleration;
        bool jumpmode;
        float height;
        Mat4 nextmat,prevmat;
        Quat nextquat,prevquat;
		Shape* shape[5];
        float radius;
        float maxslope;
        float crouchheight;
        std::list<CharacterController*>::iterator link;
        float stepheight;
        float stepextra;
        float climbspeed;
        dFloat skinwidth;
        Agent* agent;
        bool destinationset;
        Entity* followtarget;
        Vec3 destination;
        int lastplotpathtime;
        float maxspeed;
		Simulation* simulation;
		bool asleep;
		bool finished;
		int lastupdatestep;
		float maxdeceleration;
		bool detailed;

        CharacterController();
        virtual ~CharacterController();
        
		virtual void Sleep();
		virtual void Wake();
        virtual void AddForce(const Vec3& force)=0;
        virtual Vec3 GetVelocity()=0;
        //virtual Entity* Copy(const bool recursive=true);
        //virtual Entity* Instance(const bool recursive=true);
		virtual void SetRotation(const float pitch, const float yaw, const float roll, const bool global=false);
		virtual std::string GetClassName();
		virtual void InterpolatePhysics(const float f);
		virtual void SetInput(const float angle, const float move, const float strafe = 0.0, const float jump = 0.0, const bool crouch = false, const float maxacceleration = 1.0, const float maxdeceleration = 0.5, const bool detailed=false);
        virtual bool GetCrouched();
        virtual bool GetAirborne();
        //virtual void UpdateAABB(const int mode);
        virtual void Update();
        void SetPosition(const float x, const float y, const float z);
		//virtual void UpdateMatrix();
        virtual void SetMatrix(const Mat4& mat);
        virtual bool GoToPoint(const float x, const float y, const float z, const float maxspeed, const float maxacceleration);
        virtual bool Follow(Entity* entity, const float maxspeed, const float maxacceleration);
        virtual void Stop();
		virtual void SetRadius(const float radius);
		virtual float GetRadius();
		virtual void SetHeight(const float height);
		virtual float GetHeight();
		virtual void SetCrouchHeight(const float crouchheight);
		virtual float GetCrouchHeight();
		virtual void SetStepHeight(const float stepheight);
		virtual float GetStepHeight();
		virtual void SetMaxSlope(const float maxslope);
		virtual float GetMaxSlope();
        
        //static Shape* shape[2];
        static CharacterController* Create(Entity* entity, const float radius=0.4, const float height=1.8, const float stepheight=0.51, const float maxslope=45.0, const float crouchheight=1.2);
    };
}
