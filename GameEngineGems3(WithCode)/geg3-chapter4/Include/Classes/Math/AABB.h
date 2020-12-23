#pragma once

#include "Math3D.h"

namespace Leadwerks
{
	
	class Vec3;
	class Plane;
	
	class AABB : public Object//lua
	{
		public:
		Vec3 min;//lua
		Vec3 max;//lua
		Vec3 center;//lua
		Vec3 size;//lua
		float radius;//lua

		AABB();//lua
		AABB( const Vec3& min, const Vec3& max );//lua
		AABB( const float x0, const float y0, const float z0, const float x1, const float y1, const float z1 );//lua
		
		//AABB(const int count, const float cloud[]);
		virtual void Update();//lua
		virtual void Serialize(Stream* stream);
		virtual void Deserialize(Stream* stream);
		virtual std::string Debug();
		virtual bool IntersectsPoint(const Vec3& p, const float radius=0);//lua
		virtual bool IntersectsAABB(const AABB& aabb, const float overlap=0);//lua
		virtual void Add(const AABB& aabb);
		virtual bool IsOnEdge(const AABB& aabb);
		virtual bool ContainsAABB(const AABB& aabb);
		virtual string ToString();
		virtual int IntersectsPlane(Plane plane);//lua
		virtual bool IntersectsLine(const Vec3& p0, const Vec3& p1, const float radius=0.0);//lua
		virtual float DistanceToPoint(const Vec3& point, const float radius=0);//lua
		virtual std::string GetClassName();
		//virtual AABB& TForm(const Mat4 src,const Mat4 dst);
	};
}
