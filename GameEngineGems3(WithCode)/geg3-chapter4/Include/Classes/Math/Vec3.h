#pragma once

#include "Math3D.h"

namespace Leadwerks
{
	class dVec3;
	class Quat;
	
	class Vec3 : public Object//lua
	{
		public:
		
		//Attributes
		union{float x,r;};
		union{float y,g;};
		union{float z,b;};
		//float x,y,z,r,g,b;//lua
		
		//Constructors
		Vec3();//lua
		Vec3(const float x);//lua
		Vec3(const float x, const float y);//lua
		Vec3(const float x, const float y, const float z);//lua
		Vec3(const Quat& q);//lua
		Vec3(const dVec3& v);//lua
        Vec3(const std::string& s);//lua
		
		//Operators
		operator std::string();
		virtual bool operator<(const Vec3 v);
		bool operator>(const Vec3 v);
		bool operator!=(const Vec3& v);//lua
		bool operator==(const Vec3& v);//lua
		float& operator[](unsigned int n);//lua
		Vec3 operator^(const Vec3& v);
		Vec3 operator-(const Vec3& v);//lua
		//Vec3 operator-();
		void operator-=(const Vec3& v);
		Vec3 operator+(const Vec3& v);//lua
		dVec3 operator+(const dVec3& v);//lua
		void operator+=(const Vec3& v);
		Vec3 operator*(const Vec3& v);//lua
		Vec3 operator*(const float f);//lua
		void operator*=(const Vec3& v);
		void operator*=(const float f);
		Vec3 operator/(const Vec3& v);//lua
		Vec3 operator/(const float f);//lua
		Vec3 operator+(const float f);//lua
		Vec3 operator-(const float f);//lua
		void operator/=(const Vec3& v);
		void operator/=(const float f);
		float operator%(const Vec3& v);
		//Vec3 operator=(const dVec3 dv);
		Vec3 operator-();

		//Methods
		virtual Vec3 Reflect(Vec3& normal);//lua
		virtual std::string GetClassName();
		virtual void Serialize(Stream* stream);
		virtual void Deserialize(Stream* stream);
		virtual Vec2 xy();//lua
		virtual Vec2 zy();//lua
		virtual Vec2 xz();//lua
		virtual std::string ToString();// const;
		virtual float Length();//lua
		virtual Vec3 Normalize();//lua
		virtual Vec3 Inverse();//lua
		virtual float DistanceToPoint(const Vec3& p);//lua
		virtual Vec3 Cross(const Vec3& v);//lua
		virtual Vec3 Add(const Vec3& v);
		virtual Vec3 Subtract(const Vec3& v);
		virtual Vec3 Multiply(const Vec3& v);
		virtual Vec3 Multiply(const float f);
		virtual Vec3 Divide(const Vec3& v);
		virtual Vec3 Divide(const float f);
		virtual float Dot(const Vec3& v);//lua		
		virtual void Push();
		virtual std::string Debug();
	};
	
    //Vec3 operator*(const Vec3 v, const float f);
	bool operator<(const Vec3& v0,const Vec3& v1);
	bool operator>(const Vec3& v0,const Vec3& v1);
}
