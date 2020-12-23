#pragma once

#include "../Leadwerks.h"

namespace Leadwerks
{
	const int OCCLUSIONQUERY_READY = 0;
	const int OCCLUSIONQUERY_ACTIVE = 1;
	const int OCCLUSIONQUERY_RUNNING = 2;
	const int OCCLUSIONQUERY_FINISHED = 3;
	
	class OcclusionQuery : public Object
	{
	public:
		AABB aabb;
		Entity* entity;
		OctreeNode* octreenode;
		Camera* camera;
		Context* context;
		int renderticks;
		GraphicsDriver* graphicsdriver;		
		std::list<OcclusionQuery*>::iterator cameralink;
		std::list<OcclusionQuery*>::iterator contextlink;
		bool deletingowner;

		OcclusionQuery();
		virtual ~OcclusionQuery();

		virtual void Reset()=0;
		virtual void Begin()=0;
		virtual void End()=0;
		virtual int GetResult()=0;
		virtual int GetState()=0;
	};
}
