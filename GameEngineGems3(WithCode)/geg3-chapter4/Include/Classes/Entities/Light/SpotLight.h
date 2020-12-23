#pragma once
#include "../../../Leadwerks.h"

namespace Leadwerks
{
	class SpotLight : public Light//lua
	{
		public:
		std::list<SpotLight*>::iterator link;
		
		SpotLight();
		virtual ~SpotLight();
		
        virtual void UpdateAABB(const int mode);
		virtual Entity* Copy(const bool recursive = true, const bool callstartfunction=true);
		virtual Entity* Instance(const bool recursive = true, const bool callstartfunction=true);
		virtual int GetClass();
		virtual void SetConeAngles(const float outer, const float inner);//lua
        virtual Vec2 GetConeAngles();//lua
		virtual std::string GetClassName();
		virtual void Draw(Camera* camera, const bool recursive, const bool drawsorted);
		virtual void DetermineVisibleEntities();
		virtual void UpdateOcclusionQuerySurface();
		virtual bool OcclusionVolumeIntersectsPoint(const Vec3& p, const float radius);

		static SpotLight* Create(Entity* parent = NULL);//lua
	};
}
