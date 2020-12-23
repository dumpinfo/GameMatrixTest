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
	const int SHAPE_HEIGHTFIELD = 9;
	const int SHAPE_CAPSULE = 10;
	const int SHAPE_CUSTOM = 11;

	class PickInfo;

	class Shape : public Asset//lua
	{
	public:
		int shapeid;
		Vec3 size;
		Surface* surface_[2];
		Vec3 position;
        Vec3 rotation;
        Vec3 scale;
        AABB aabb;
		
		Shape();
		virtual ~Shape();
		
        virtual Surface* GetSurface(const int index)=0;
		//virtual Pick(p0,p1,pick,radius,closest)=0;
		virtual void Finalize()=0;
		virtual void Reset();
		virtual bool Reload(const int flags);
		virtual bool Pick(const Vec3& p0, const Vec3& p1, PickInfo& pick, const Vec3& radius, const bool closestpoint=false)=0;
		virtual bool ConvexCast(Shape* shape, const Vec3& p0, const Vec3& p1, PickInfo& pick, const bool closestpoint=false)=0;
		//virtual bool ConvexCast(Shape* shape, const Vec3& p0, const Vec3& rotation0, const Vec3& p1, const Vec3& rotation1, Pick& pick, const bool closestpoint=false)=0;
		virtual Surface* ExtractSurface(const AABB& aabb)=0;
		virtual void* GetHeightField(const int mode) = 0;
		virtual bool Save(Stream* stream) = 0;
		virtual bool Save(const std::string& path);
		virtual void SetDisplacementMap(const unsigned short* displacementmap, const float displacementscale)=0;

		static Shape* Box(const float x = 0, const float y = 0, const float z = 0, const float pitch = 0, const float yaw = 0, const float roll = 0, const float width = 1, const float height = 1, const float depth = 1);//lua
        static Shape* Cylinder(const float x=0, const float y=0, const float z=0, const float pitch=0, const float yaw=0, const float roll=0, const float width=1, const float height=1, const float depth=1);//lua
        static Shape* Cone(const float x=0, const float y=0, const float z=0, const float pitch=0, const float yaw=0, const float roll=0, const float width=1, const float height=1, const float depth=1);//lua
        static Shape* Sphere(const float x=0, const float y=0, const float z=0, const float pitch=0, const float yaw=0, const float roll=0, const float width=1, const float height=1, const float depth=1);//lua
        static Shape* ConvexHull(Surface* surf);//lua
		static Shape* Capsule(const float x = 0, const float y = 0, const float z = 0, const float pitch = 0, const float yaw = 0, const float roll = 0, const float width = 1, const float height = 1, const float depth = 1);//lua

        //virtual void Compound(std::vector<Shape*>& shapes)=0;
        static Shape* PolyMesh(Surface* surface);//lua
        static Shape* ConvexDecomposition(Surface* surface);//lua
		static Shape* ConvexHull(std::vector<float> points);
		virtual std::string GetClassName();
        
		static Shape* Load(const std::string& path, const int flags = 0, const uint64_t fileid=0);//lua
    };
}
