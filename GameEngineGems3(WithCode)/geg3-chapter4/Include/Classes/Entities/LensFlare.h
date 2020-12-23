#pragma once

#include "../../Leadwerks.h"

namespace Leadwerks
{
	class Sprite;

	class LensFlare : public Sprite//lua
	{
	public:
		Vec2 radius;
		float range;
		float rotationspeed;
		OcclusionQuery* query;
		std::map<Camera*, Vec4> camerainfo;
		float faderange;
		float angleseed;
		Surface* occlusionquerysurface;
		float fadespeed;
		std::map<Camera*, bool> visibility;
		std::map<Camera*, float> smoothedvisibility;
		long lastraycasttime;
		long lastvisibletime;
		int pixelsdrawn;

		LensFlare();
		virtual ~LensFlare();
		
		virtual void UpdateAABB(const int mode);
		virtual Surface* GetOcclusionQuerySurface();
		virtual bool OcclusionVolumeIntersectsPoint(const Vec3& p, const float radius);
		virtual void SetRange(const float range);//lua
		virtual float GetRange();//lua
		virtual void SetRadius(const float nearradius, const float farradius);//lua
		virtual Vec2 GetRadius();//lua
		virtual void SetRotationSpeed(const float rotationspeed);//lua
		virtual float GetRotationSpeed();//lua
		virtual void CopyTo(Entity* entity,const int mode);//lua
		virtual Entity* Copy(const bool recursive = true, const bool callstartfunction=true);//lua
		virtual Entity* Instance(const bool recursive = true, const bool callstartfunction=true);//lua
		virtual std::string GetClassName();
		virtual void Draw(Camera* camera, const bool recursive, const bool drawsorted);
		virtual bool OcclusionCulled(Camera* camera, const bool conservative=false);

		static LensFlare* Create(Entity* parent=NULL);//lua
	};
}
