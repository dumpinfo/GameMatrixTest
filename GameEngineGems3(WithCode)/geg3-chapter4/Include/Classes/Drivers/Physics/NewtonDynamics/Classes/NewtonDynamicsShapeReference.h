#pragma once

#include "../NewtonDynamicsPhysicsDriver.h"

namespace Leadwerks
{
	class NewtonDynamicsShapeReference : public ShapeReference
	{
	public:
        static float contacts[256*3];
        static float timeOfImpact[256];
        static float normals[256*3];
        static float penetration[256];	        
        
        std::vector<NewtonCollision*> collisions;
		Bank* polymeshdata;	
        int polymeshtris;
		
		NewtonDynamicsShapeReference();
		virtual ~NewtonDynamicsShapeReference();
		
		virtual void Finalize();
		//virtual NewtonCollision* RequestCollision(const Mat4& mat);
		virtual void Reset();
		virtual AssetReference* Copy();
		virtual bool Pick(const Vec3& p0, const Vec3& p1, Pick& pick, const Vec3& radius, const bool closestpoint=false);
		virtual void AddConvexHullSurface(Surface* surface,Mat4& src,Mat4& dst);
		virtual void AddPolyMeshSurface(Surface* surface,Mat4& src,Mat4& dst);
		virtual void AddConvexHullEntity(Entity* entity, Entity* root=NULL);
		virtual void AddPolyMeshEntity(Entity* entity, Entity* root=NULL);
		virtual Surface* ExtractSurface(const AABB& aabb);
		virtual bool ConvexCast(Shape* shape, const Vec3& p0, const Vec3& p1, Pick& pick, const bool closestpoint=false);
        virtual void UpdateAABB();
        virtual void Box(const float x, const float y, const float z, const float pitch, const float yaw, const float roll, const float width, const float height, const float depth);
        virtual void Cylinder(const float x, const float y, const float z, const float pitch, const float yaw, const float roll, const float width, const float height, const float depth);
        virtual void Cone(const float x, const float y, const float z, const float pitch, const float yaw, const float roll, const float width, const float height, const float depth);
        virtual void Sphere(const float x, const float y, const float z, const float pitch, const float yaw, const float roll, const float width, const float height, const float depth);
        virtual void ConvexHull(Surface* surf);
        //virtual void Compound(std::vector<Shape*>& shapes);
        virtual void PolyMesh(Surface* surface);
        virtual void ConvexDecomposition(Surface* surface);
	};
	
	void NewtonCollisionIterator(void* const userData, int vertexCount, const dFloat* const faceArray, int faceId);
	extern NewtonCollision* _nullCollision;
	extern NewtonWorld* _collisionWorld;
	extern std::map<Vec3,NewtonCollision*> _PickSphere;
}
