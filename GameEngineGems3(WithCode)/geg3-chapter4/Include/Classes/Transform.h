#include "../Leadwerks.h"

namespace Leadwerks
{
	class Entity;
	class Mat4;
	class Vec3;
	class Plane;
	class Quat;

	class Transform//lua
	{
	public:
		//Entity transformations
		static Vec3 Point(const Vec3& point, Entity* src, Entity* dst);//lua
		static dVec3 Point(const dVec3& point, Entity* src, Entity* dst);//lua
		static Vec3 Vector(const Vec3& vector, Entity* src, Entity* dst);//lua
		static Vec3 Normal(const Vec3& normal, Entity* src, Entity* dst);//lua
		static Vec3 Rotation(const Vec3& rotation, Entity* src, Entity* dst);//lua
		static Quat Rotation(const Quat& rotation, Entity* src, Entity* dst);//lua
		static Leadwerks::Plane Plane(const Leadwerks::Plane& plane, Entity* src, Entity* dst);//lua
		static Leadwerks::Plane Plane(const float x, const float y, const float z, const float w, Entity* src, Entity* dst);//lua
		static Vec3 Point(const float x, const float y, const float z, Entity* src, Entity* dst);//lua
		static Vec3 Vector(const float x, const float y, const float z, Entity* src, Entity* dst);//lua
		static Vec3 Normal(const float x, const float y, const float z, Entity* src, Entity* dst);//lua
		static Vec3 Rotation(const float x, const float y, const float z, Entity* src, Entity* dst);//lua

		//Mat4 transformations
		static Vec3 Point(const Vec3& point, const Mat4& src, const Mat4& dst);//lua
		static Vec3 Vector(const Vec3& vector, const Mat4& src, const Mat4& dst);//lua
		static Vec3 Normal(const Vec3& normal, const Mat4& src, const Mat4& dst);//lua
		static Vec3 Rotation(const Vec3& rotation, const Mat4& src, const Mat4& dst);//lua
		static Quat Rotation(const Quat& rotation, const Mat4& src, const Mat4& dst);//lua
		static Leadwerks::Plane Plane(const Leadwerks::Plane& plane, const Mat4& src, const Mat4& dst);//lua
		static Leadwerks::Plane Plane(const float x, const float y, const float z, const float w, const Mat4& src, const Mat4& dst);//lua
		static Vec3 Point(const float x, const float y, const float z, const Mat4& src, const Mat4& dst);//lua
		static dVec3 Point(const dVec3& point, const dMat4& src, const dMat4& dst);
		static Vec3 Vector(const float x, const float y, const float z, const Mat4& src, const Mat4& dst);//lua
		static Vec3 Normal(const float x, const float y, const float z, const Mat4& src, const Mat4& dst);//lua
		static Vec3 Rotation(const float x, const float y, const float z, const Mat4& src, const Mat4& dst);//lua
		static Leadwerks::AABB AABB(const Leadwerks::AABB& aabb, Entity* src, Entity* dst, const bool exact);//lua
		static Leadwerks::AABB AABB(const Leadwerks::AABB& aabb, Mat4& src, Mat4& dst, const bool exact);//lua
	};
}
