#pragma once

#include "Math3D.h"

using namespace std;

namespace Leadwerks
{
	
	class Vec3;
	class dVec4;

	// Classes
	class Vec4 : public Object//lua
	{
	public:

		//Attributes
		union{float x,r;};
		union{float y,g;};
		union{float z,b;};
		union{float w,a;};
		//float x,y,z,w,r,g,b,a;//lua
		
		//Constructors
		Vec4();//lua
		Vec4(const float x);//lua
		Vec4(const float x, const float y);//lua
		Vec4(const float x, const float y, const float z);//lua
		Vec4(const float x, const float y, const float z, const float w);//lua
		Vec4(const std::string& s);//lua
		Vec4(const Vec3& v, const float w);//lua
		Vec4(const dVec4& v);//lua

		//Methods
		virtual Vec2 xy();//lua
		virtual Vec3 xyz();//lua
		virtual std::string ToString() const;//lua
		virtual float Length();
		virtual float Dot(const Vec4& v);
		virtual Vec4 Normalize();//lua
		virtual std::string Debug();
		virtual void Serialize(Stream* stream);
		virtual void Deserialize(Stream* stream);
		virtual float DistanceToPoint(const Vec4& v);
		virtual std::string GetClassName();
		
		//Operators
		Vec4 operator+(const Vec4& v);//lua
		bool operator!=(const Vec4& v);//lua
		bool operator==(const Vec4& v);//lua
		Vec4 operator*(const float f);//lua
		void operator*=(const float f);
		float operator%(const Vec4& v);
		float& operator[](const unsigned int n);//lua
		bool operator<(const Vec4 v);
		void operator/=(const float f);
		void operator+=(const Vec4& f);
	};
	
	bool operator<(const Vec4& v0,const Vec4& v1);
	bool operator>(const Vec4& v0,const Vec4& v1);
}
