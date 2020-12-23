#pragma once
#include "../Leadwerks.h"

namespace Leadwerks
{	
	class SurfaceTreeNode : public OctreeNode
	{
	public:
			std::vector<unsigned int> index;
			std::vector<Triangle> triangle;
			
			SurfaceTreeNode();
			virtual ~SurfaceTreeNode();
			
			virtual void Clear();
			OctreeNode* FindNode(const AABB& aabb);
			/*
			virtual void Populate(const int sublevels);
			virtual bool Pick(const Vec3& p0, const Vec3& p1);
			virtual void ForEachInAABBDo(const AABB& aabb, const char* callback);
			virtual void ForEachInSolidDo(const Solid& solid, const char* callback);
			virtual void Draw(const Camera* camera);
			virtual SurfaceTreeNode* FindNode(const Vec3& point);
			virtual SurfaceTreeNode* FindNode(const AABB& aabb);*/
			virtual bool Pick(const Vec3& p0, const Vec3& p1, Pick& pick, const float radius = 0.0, bool closest = false);
	};
}
