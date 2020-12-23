#pragma once

#include "Math3D.h"

namespace Leadwerks
{
	class Vec3;

	class Triangle : public Object
	{
	public:
		Vec3 p[3];
		Plane plane;
		AABB aabb;
		int index;

		Triangle();
		Triangle(const Vec3& p0, const Vec3& p1, const Vec3& p2);
		
		virtual void Update();
		virtual int Slice(Plane& plane, Triangle& t0, Triangle& t1, Triangle& t2, const float epsilon = 0.001);
		virtual bool IntersectsRay(const Vec3& p0, const Vec3& p1, Vec3& position, const float radius = 0.0);
		virtual Vec3& operator[](const unsigned int n);
		virtual Vec3 GetNormal();
		virtual void Invert();
	};
	void SweptSphereRayTest(const Vec3& s0,const Vec3& s1, const float radius, const Vec3& p0, const Vec3& p1, Vec3& position);
}
