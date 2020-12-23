#pragma once

#include "Math3D.h"

namespace Leadwerks
{
	
	class Vec3;
	class Triangle;

	// Classes
	class Plane : public Object//lua
	{
		public:
		float x,y,z,d;
		
		//Constructors
		Plane();//lua
		Plane(const float x, const float y, const float z, const float d);//lua
		Plane(Vec3& point, const Vec3& normal);//lua
		Plane(const Triangle& triangle);
		
		//Methods
        Plane Inverse();//lua
		float DistanceToPoint(const Vec3& p);//lua
		float DistanceToPoint(const float x, const float y, const float z);//lua
		void Invert();//lua
		void Normalize();//lua
		std::string ToString() const;
		bool IntersectsLine(const Vec3& p0,const Vec3& p1, Vec3& result, const bool twosided=false);//lua
		Vec3 GetNormal();
		bool IntersectsPlanes(Plane& p0, Plane& p1, Vec3& result);//lua
		virtual std::string GetClassName();
	};
} // namespace LE3
