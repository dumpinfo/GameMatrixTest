#pragma once

#include "../Leadwerks.h"

namespace Leadwerks
{	
	const int SHAPE_NULL = 0;
	const int SHAPE_BOX = 1;
	const int SHAPE_CYLINDER = 2;
	const int SHAPE_CONE = 3;
	const int SHAPE_SPHERE = 4;
	const int SHAPE_CONVEXHULL = 5;
	const int SHAPE_POLYMESH = 6;
    const int SHAPE_CONVEXDECOMPOSITION = 7;
    const int SHAPE_COMPOUND = 8;
	const int SHAPE_CAPSULE = 9;
	
	class ShapeReference : public AssetReference
	{
	public:
		int shapeid;
		Vec3 size;
		Surface* surface[2];
		Vec3 position;
        Vec3 rotation;
        Vec3 scale;
        AABB aabb;
        
		ShapeReference();
		virtual ~ShapeReference();
		
		//virtual Pick(p0,p1,pick,radius,closest)=0;
		virtual void Finalize()=0;
		virtual void Reset();
		virtual bool Reload(const int flags);
		virtual bool Pick(const Vec3& p0, const Vec3& p1, Pick& pick, const Vec3& radius, const bool closestpoint=false)=0;
		virtual bool ConvexCast(Shape* shape, const Vec3& p0, const Vec3& p1, Pick& pick, const bool closestpoint=false)=0;
		//virtual bool ConvexCast(Shape* shape, const Vec3& p0, const Vec3& rotation0, const Vec3& p1, const Vec3& rotation1, Pick& pick, const bool closestpoint=false)=0;
		virtual Asset* Instance();
		virtual Surface* ExtractSurface(const AABB& aabb)=0;
        virtual void Box(const float x, const float y, const float z, const float pitch, const float yaw, const float roll, const float width, const float height, const float depth)=0;
        virtual void Cylinder(const float x, const float y, const float z, const float pitch, const float yaw, const float roll, const float width, const float height, const float depth)=0;
        virtual void Cone(const float x, const float y, const float z, const float pitch, const float yaw, const float roll, const float width, const float height, const float depth)=0;
        virtual void Sphere(const float x, const float y, const float z, const float pitch, const float yaw, const float roll, const float width, const float height, const float depth)=0;
        virtual void ConvexHull(Surface* surf)=0;
        //virtual void Compound(std::vector<Shape*>& shapes)=0;
        virtual void PolyMesh(Surface* surface)=0;
        virtual void ConvexDecomposition(Surface* surface)=0;
    };
}
