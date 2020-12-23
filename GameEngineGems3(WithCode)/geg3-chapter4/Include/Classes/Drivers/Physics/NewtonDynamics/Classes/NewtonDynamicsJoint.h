#pragma once

#include "../NewtonDynamicsPhysicsDriver.h"

namespace Leadwerks
{
	class NewtonDynamicsJoint : public Joint
	{
	public:
        //NewtonUserJoint* newtonuserjoint;
        CustomJoint* newtonuserjoint;
		NewtonJoint* newtonjoint;
		float targetangle;
		float motorspeed;
        std::list<NewtonDynamicsJoint*>::iterator updatelink;
        bool updateneeded;
#ifdef _NEWTON_USE_DOUBLE
		dVec3 fixedjointchildrelativeposition;
        dVec3 fixedjointparentrelativeaxes[3];
        dVec3 fixedjointchildrelativeaxes[3];
#else
		Vec3 fixedjointchildrelativeposition;
		Vec3 fixedjointparentrelativeaxes[3];
		Vec3 fixedjointchildrelativeaxes[3];
#endif
		bool limitsenabled;

		NewtonDynamicsJoint();
		virtual ~NewtonDynamicsJoint();

		virtual void Wake();
		virtual void SetMotorSpeed(const float speed);
		virtual float GetMotorSpeed();
		virtual void SetAngle(const float angle);
		virtual float GetAngle();
        virtual void SetLimits(const float limits0, const float limits1);
        virtual void AddToUpdateList();
        virtual void Update();
		virtual void EnableLimits();
        virtual void EnableMotor();
		virtual void DisableLimits();
        virtual void DisableMotor();
		virtual bool LimitsEnabled();
		virtual bool MotorEnabled();

        static void NewtonUserBilateralCallBack(const NewtonJoint* newtonjoint, dFloat timestep, int threadIndex);
	};
}
