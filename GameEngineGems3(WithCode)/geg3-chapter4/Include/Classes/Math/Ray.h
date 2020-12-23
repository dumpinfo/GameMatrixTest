#pragma once

#include "Math3D.h"

namespace Leadwerks
{
	class Vec3;
	
	class Ray
	{
	public:
		Vec3 p[2];
		
		Ray();
		Ray(const Vec3& p0, const Vec3& p1);

		float DistanceToPoint(const Vec3& p);
		float DistanceToRay(const Ray& r, Vec3& position);
	};
}
