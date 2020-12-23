#pragma once

#include "../NewtonDynamicsPhysicsDriver.h"

namespace Leadwerks
{
	class NewtonDynamicsShape : public Shape
	{
	public:
#ifdef _NEWTON_USE_DOUBLE
        static double contacts[256*3];
		static double timeOfImpact[256];
		static double normals[256 * 3];
		static double penetration[256];
#else
		static float contacts[256 * 3];
		static float timeOfImpact[256];
		static float normals[256 * 3];
		static float penetration[256];
#endif
		static long long attributeA[256];
		static long long attributeB[256];

        std::vector<NewtonCollision*> collisions;
		//Bank* polymeshdata;	
        int polymeshtris;
		int heightfieldresolution;
		float heightfieldscale;
		std::vector< std::vector<float> > convexhulldata;
		std::vector<float> polymeshdata;
		std::map<Vec3, std::map<int,NewtonCollision*> > scaledcollision;
		int currentbuildsurface;
		unsigned char* offsetdata;

		NewtonDynamicsShape();
		virtual ~NewtonDynamicsShape();
		
		virtual bool Save(Stream* stream); 
		virtual Surface* GetSurface(const int index);
		virtual void Finalize();
		//virtual NewtonCollision* RequestCollision(const Mat4& mat);
		virtual void Reset();
		virtual Asset* Copy();
		virtual bool Pick(const Vec3& p0, const Vec3& p1, PickInfo& pick, const Vec3& radius, const bool closestpoint=false);
		virtual void AddConvexHullSurface(Surface* surface,Mat4& src,Mat4& dst);
		virtual void AddPolyMeshSurface(Surface* surface,Mat4& src,Mat4& dst);
		virtual void AddConvexHullEntity(Entity* entity, Entity* root=NULL);
		virtual void AddPolyMeshEntity(Entity* entity, Entity* root=NULL);
		virtual Surface* ExtractSurface(const AABB& aabb);
		virtual bool ConvexCast(Shape* shape, const Vec3& p0, const Vec3& p1, PickInfo& pick, const bool closestpoint=false);
        virtual void UpdateAABB();
		virtual NewtonCollision* RequestCollision(const Vec3& scale, const int index);
		virtual void* GetHeightField(const int mode);
		virtual void SetDisplacementMap(const unsigned short* displacementmap, const float displacementscale);
		/*virtual void Box(const float x, const float y, const float z, const float pitch, const float yaw, const float roll, const float width, const float height, const float depth);
        virtual void Cylinder(const float x, const float y, const float z, const float pitch, const float yaw, const float roll, const float width, const float height, const float depth);
        virtual void Cone(const float x, const float y, const float z, const float pitch, const float yaw, const float roll, const float width, const float height, const float depth);
        virtual void Sphere(const float x, const float y, const float z, const float pitch, const float yaw, const float roll, const float width, const float height, const float depth);
        virtual void ConvexHull(Surface* surf);
        //virtual void Compound(std::vector<Shape*>& shapes);
        virtual void PolyMesh(Surface* surface);
        virtual void ConvexDecomposition(Surface* surface);*/
	};
	
	void NewtonCollisionIterator(void* const userData, int vertexCount, const dFloat* const faceArray, int faceId);
	extern NewtonCollision* _nullCollision;
	extern NewtonWorld* _collisionWorld;
	extern std::map<Vec3,NewtonCollision*> _raycastSphere;
}
