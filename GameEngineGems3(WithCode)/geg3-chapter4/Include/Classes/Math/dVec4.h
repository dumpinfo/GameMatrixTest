#pragma once

#include "Math3D.h"

using namespace std;

namespace Leadwerks
{
	
	class Vec3;

	// Classes
	class dVec4 : public Object//lua
	{
	public:

		//Attributes
		union{double x,r;};
		union{double y, g; };
		union{double z, b; };
		union{double w, a; };
		//double x,y,z,w,r,g,b,a;//lua
		
		//Constructors
		dVec4();//lua
		dVec4(const double x);//lua
		dVec4(const double x, const double y);//lua
		dVec4(const double x, const double y, const double z);//lua
		dVec4(const double x, const double y, const double z, const double w);//lua
		dVec4(const std::string& s);//lua
		dVec4(const Vec3& v, const double w);//lua
		
		//Methods
		virtual Vec2 xy();//lua
		virtual Vec3 xyz();//lua
		virtual std::string ToString() const;
		virtual double Length();
		virtual double Dot(const dVec4& v);
		virtual dVec4 Normalize();
		virtual std::string Debug();
		virtual void Serialize(Stream* stream);
		virtual void Deserialize(Stream* stream);
		virtual double DistanceToPoint(const dVec4& v);
		virtual std::string GetClassName();
		
		//Operators
		dVec4 operator+(const dVec4& v);//lua
		bool operator!=(const dVec4& v);//lua
		bool operator==(const dVec4& v);//lua
		dVec4 operator*(const double f);//lua
		void operator*=(const double f);
		double operator%(const dVec4& v);
		double& operator[](const unsigned int n);//lua
		bool operator<(const dVec4 v);
		dVec4 operator/=(const double f);
		dVec4 operator+=(const dVec4& f);
	};
	
	bool operator<(const dVec4& v0,const dVec4& v1);
	bool operator>(const dVec4& v0,const dVec4& v1);
}
