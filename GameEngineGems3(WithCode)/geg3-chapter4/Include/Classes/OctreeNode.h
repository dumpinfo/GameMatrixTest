#pragma once
#include "../Leadwerks.h"

namespace Leadwerks
{
	class Entity;
	class Camera;
	class GraphicsDriver;
	class OcclusionQuery;
	class Light;
	class Context;
    
	class OctreeNode : public Object
	{
	public:
        AABB aabb;
		GraphicsDriver* graphicsdriver;
		OctreeNode* parent;
		//std::vector<OctreeNode*> child;
		int sublevels;		
		float distance;
		//LinkedList entities[4];
		std::list<Entity*> entities[5];
        std::list<Light*> lights;
		OctreeNode* child[2][2][2];
		int sumentities[8];
		iVec3 position;
		std::map<Camera*, std::map<Context*, OcclusionQuery*> > occlusionquery;
		//std::map<Camera*,OcclusionQuery*> occlusionquery;
        int countparentlights;
        
		OctreeNode();
		virtual ~OctreeNode();
		
        virtual void IncrementKidsParentLightCount(const int count);
        virtual void ForEachLightInAABBDo(const AABB& aabb, void callback(Light* light, Object* extra), Object* extra = NULL);
        //virtual void ForEachLightInAABBDo(const AABB& aabb, void callback(Light* light, Object* extra), Object* extra, const bool up, const bool down);
		virtual void InsertEntity(Entity* entity);
        virtual void RemoveEntity(Entity* entity);
        virtual OcclusionQuery* GetOcclusionQuery(Camera* camera);
		virtual void IncrementEntityCount(const int i0,const int i1,const int i2,const int i3,const int i4,const int i5);
		virtual void ForEachEntityInAABBDo(const AABB& aabb, void callback(Entity* entity, Object* extra), Object* extra);
		//virtual void ForEachEntityInAABBDo(const AABB& aabb, const std::string& funcname, Object* extra);
		virtual OctreeNode* GetChild(const int x, const int y, const int z);
		virtual void Clear();
		//virtual void Populate(const int sublevels)=0;
		//virtual bool Pick(const Vec3& p0, const Vec3& p1);
		//virtual void ForEachNodeInSolidDo(const Solid& solid, const char* callback);
		//virtual void Draw(const Camera* camera);
		virtual void ForEachNodeInAABBDo(const AABB& aabb, void callback(OctreeNode* node, const AABB& aabb, const char* extra), const char* extra = NULL);
		virtual void ForEachVisibleEntityDo(Camera* camera, void callback(Camera* camera, Entity* entity, Object* extra), Object* extra = NULL);
		virtual OctreeNode* FindNode(const Vec3& point);
		virtual OctreeNode* FindNode(const AABB& aabb);
		virtual bool Pick(const Vec3& p0, const Vec3& p1, PickInfo& pick, const float radius = 0.0, bool closest = false, const int collisiontype=0);
		
		static AABB CalculateChildAABB(const AABB& aabb, const float x, const float y, const float z);
		static OctreeNode* Create(const AABB& aabb, const int sublevels);
	};
	
	bool CompareOctreeNode(OctreeNode* a,OctreeNode* b);
}

