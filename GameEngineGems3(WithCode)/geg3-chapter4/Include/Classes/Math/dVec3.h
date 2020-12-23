#pragma once

#include "Math3D.h"

namespace Leadwerks
{
	class ddVec3;
	class Quat;
	class Vec3;
	
	class dVec3 : public Object//lua
	{
    public:
		
		//Attributes
		union{double x,r;};
		union{double y,g;};
		union{double z,b;};
		//double x,y,z,r,g,b;//lua
		
		//Constructors
		dVec3();//lua
		dVec3(const double& x);//lua
		dVec3(const double& x, const double& y);//lua
		dVec3(const double& x, const double& y, const double& z);//lua
		dVec3(const Quat& q);
		dVec3(const Vec3& v);//lua
        dVec3(const std::string& s);//lua
		
		//Operators
		operator std::string();
		virtual bool operator<(const dVec3 v);
		bool operator>(const dVec3 v);//lua
		bool operator!=(const dVec3& v);//lua
		bool operator==(const dVec3& v);//lua
		double& operator[](unsigned int n);//lua
		dVec3 operator^(const dVec3& v);
		dVec3 operator-(const dVec3& v);//lua
		dVec3 operator-(const Vec3& v);//lua
		//dVec3 operator-();
		void operator-=(const dVec3& v);
		dVec3 operator+(const dVec3& v);//lua
		dVec3 operator+(const Vec3& v);//lua
		void operator+=(const dVec3& v);
		dVec3 operator*(const dVec3& v);//lua
		dVec3 operator*(const double f);//lua
		void operator*=(const dVec3& v);
		void operator*=(const double f);
		dVec3 operator/(const dVec3& v);//lua
		dVec3 operator/(const double f);//lua
		void operator/=(const dVec3& v);
		void operator/=(const double f);
		float operator%(const dVec3& v);
		dVec3 operator-();
        
		//Methods
		virtual std::string GetClassName();
		virtual void Serialize(Stream* stream);
		virtual void Deserialize(Stream* stream);
		virtual std::string ToString();// const;
		virtual float Length();//lua
		virtual dVec3 Normalize();//lua
		virtual dVec3 Inverse();//lua
		virtual double DistanceToPoint(const dVec3& p);//lua
		virtual double DistanceToPoint(const Vec3& p);//lua
		virtual dVec3 Cross(const dVec3& v);//lua
		virtual dVec3 Add(const dVec3& v);
		virtual dVec3 Subtract(const dVec3& v);
		virtual dVec3 Multiply(const dVec3& v);
		virtual dVec3 Multiply(const float f);
		virtual dVec3 Divide(const dVec3& v);
		virtual dVec3 Divide(const float f);
		virtual float Dot(const dVec3& v);//lua
		virtual void Push();
		virtual std::string Debug();
		virtual Vec2 xy();
		virtual Vec2 zy();
		virtual Vec2 xz();
	};
	
    //dVec3 operator*(const dVec3 v, const float f);
	bool operator<(const dVec3& v0,const dVec3& v1);
	bool operator>(const dVec3& v0,const dVec3& v1);
}
