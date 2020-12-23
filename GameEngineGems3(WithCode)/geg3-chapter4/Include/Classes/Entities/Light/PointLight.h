#pragma once
#include "../../../Leadwerks.h"

namespace Leadwerks
{
	class PointLight : public Light//lua
	{
		public:
		std::list<PointLight*>::iterator link;
		
		PointLight();
		virtual ~PointLight();
		
		virtual Entity* Copy(const bool recursive = true, const bool callstartfunction=true);
		virtual Entity* Instance(const bool recursive = true, const bool callstartfunction=true);
        virtual void UpdateAABB(const int mode);
        virtual int GetClass();
		virtual std::string GetClassName();
		virtual void Draw(Camera* camera, const bool recursive, const bool drawsorted);
		virtual void DetermineVisibleEntities();
		virtual void UpdateOcclusionQuerySurface();
		virtual bool OcclusionVolumeIntersectsPoint(const Vec3& p, const float radius);

        static PointLight* Create(Entity* parent = NULL);//lua
	};
}
