#pragma once
#include "../../Leadwerks.h"

namespace Leadwerks
{
	class WaterPlane : public Pivot
	{
	public:
		Camera* camera;
		
		WaterPlane();
		virtual ~WaterPlane();
		
		virtual Mat4 GetOcclusionQueryMatrix();
		virtual void UpdateOcclusionQuerySurface();
		virtual bool OcclusionVolumeIntersectsPoint(const Vec3& p, const float radius);
	};
}
