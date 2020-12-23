#pragma once

#include "Math3D.h"

namespace Leadwerks
{
	
	class Vec3;
	
	class Quat : public Object//lua
	{
		public:

		//Attributes
		float x,y,z,w;

		//Constructors
		Quat();//lua
		Quat(float x, float y, float z, float w);//lua
		Quat(const Vec3& euler);//lua
		Quat(const float angle, const Vec3& axis);//lua
		Quat(const string& s);//lua
		
		//Methods
		string ToString();//lua
		Quat Inverse();//lua
		Vec3 GetAverageOmega(const Quat& qb, const double dt=1.0);
		Quat Cross(const Quat& q);
		Quat operator^(const Quat& q);
		Quat Multiply(const Quat& q);//lua
		Quat operator*(const Quat& q);
		float Dot(const Quat& q);
		//flt operator%(const Quat& q);
		void ToAngleAxis(float angle, Vec3& axis);
		Quat Normalize();//lua
		Quat Slerp(const Quat& q, float a);//lua
		virtual std::string GetClassName();
		
	};
}
