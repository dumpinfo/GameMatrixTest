#pragma once

#include "../../Leadwerks.h"

namespace Leadwerks
{
	class Entity;
	
	class Joint : public Entity//lua
	{
    public:
        std::list<Joint*>::iterator link;
        Entity* entity[2];
        Mat4 entitymatrix[2];
        Vec3 pin[2];
        int type;
        bool uselimits;
        float limits[2];
        Vec3 origin;
        std::list<Joint*>::iterator bodylink[2];
        
        Joint();
		virtual ~Joint();
        
        virtual void SetLimits(const float limits0, const float limits1)=0;//lua
        virtual Vec2 GetLimits();//lua
		virtual Entity* Copy(const bool recursive=true, const bool callstartfunction=true);
		virtual Entity* Instance(const bool recursive=true, const bool callstartfunction=true);
        virtual std::string GetClassName();
		virtual void SetAngle(const float angle)=0;//lua
		virtual float GetAngle()=0;//lua
		virtual void SetMotorSpeed(const float speed)=0;//lua
		virtual float GetMotorSpeed()=0;//lua
		//virtual void UseLimits(const bool enabled)=0;
		virtual void EnableLimits()=0;//lua
        virtual void EnableMotor()=0;//lua
		virtual void DisableLimits()=0;//lua
        virtual void DisableMotor()=0;//lua
		virtual bool LimitsEnabled()=0;//lua
		virtual bool MotorEnabled()=0;//lua
		virtual void SetTargetMatrix(Mat4& mat, const float blend=0.5);//lua

        static const int HINGE;
        static const int BALL;
        static const int SLIDER;
        static const int FIXED;
        static const int CORKSCREW;
        static const int UNIVERSAL;
        static const int MOTORIZEDHINGE;
        static const int MOTORIZEDSLIDER;
		static const int KINEMATIC;
		
		static Joint* Kinematic(const float posx, const float posy, const float posz, Entity* child);//lua
		static Joint* Hinge(const float posx, const float posy, const float posz, const float pinx, const float piny, const float pinz, Entity* child, Entity* parent=NULL);//lua
        static Joint* Ball(const float posx, const float posy, const float posz, Entity* child, Entity* parent=NULL);//lua
		static Joint* Slider(const float posx, const float posy, const float posz, const float pinx, const float piny, const float pinz, Entity* child, Entity* parent=NULL);//lua
        static Joint* Fixed(const float posx, const float posy, const float posz, Entity* child, Entity* parent=NULL);//lua
	};
}
