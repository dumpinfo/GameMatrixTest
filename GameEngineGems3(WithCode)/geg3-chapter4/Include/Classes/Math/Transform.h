#pragma once

#include "Math3D.h"

namespace Leadwerks
{
	
	class Vec3;
	class Quat;
	class AABB;
	class Mat4;
	class Plane;
	class Entity;
	
	//TFormpoint overloaded functions
	dVec3 TFormPoint(const dVec3& point, const dMat4& src, const dMat4& dst);
	Vec3 TFormPoint(const Vec3& point, const Mat4& src, const Mat4& dst);
	//Vec3 TFormPoint(const Vec3& point, Entity& src, Entity& dst);
	dVec3 TFormPoint(const double x, const double y, const double z, const dMat4& src, const dMat4& dst);
	Vec3 TFormPoint(const float x, const float y, const float z, const Mat4& src, const Mat4& dst);
	Vec3 TFormPoint(const float x, const float y, const float z, Entity& src, Entity& dst);
	//dVec3 TFormPoint(const dVec3& point, dMat4* src=NULL, dMat4* dst=NULL); 
	Vec3 TFormPoint(const Vec3& point, Mat4* src=NULL, Mat4* dst=NULL); 
	//Vec3 TFormPoint(const Vec3& point, Entity* src=NULL, Entity* dst=NULL);
	Vec3 TFormPoint(const float x, const float y, const float z, Mat4* src=NULL, Mat4* dst=NULL); 
	//Vec3 TFormPoint(const float x, const float y, const float z, Entity* src=NULL, Entity* dst=NULL);
	
	Quat TFormQuaternion(const Quat& quat, const Mat4& src, const Mat4& dst); 

	Vec3 TFormVector(const Vec3& vector, const Mat4& src, const Mat4& dst); 
	//Vec3 TFormVector(const Vec3& vector, Entity& src, Entity& dst);
	Vec3 TFormVector(const float x, const float y, const float z, const Mat4 src, const Mat4 dst); 
	//Vec3 TFormVector(const float x, const float y, const float z, Entity& src, Entity& dst);
	
	Vec3 TFormNormal(const Vec3& normal, Mat4& src, Mat4& dst); 
	//Vec3 TFormNormal(const Vec3& normal, Entity& src, Entity& dst);
	Vec3 TFormNormal(const float x, const float y, const float z, Mat4& src, Mat4& dst); 
	//Vec3 TFormNormal(const float x, const float y, const float z, Entity& src, Entity& dst);

	Vec3 TFormRotation(const Vec3& rotation, const Mat4& src, Mat4& dst); 
	//Vec3 TFormRotation(const Vec3& rotation, Entity& src, Entity& dst);
	Vec3 TFormRotation(const float pitch, const float yaw, const float roll, const Mat4& src, Mat4& dst); 
	Vec3 TFormRotation(const float pitch, const float yaw, const float roll, Entity& src, Entity& dst); 
	Quat TFormRotation(const Quat& quat, const Mat4& src, Mat4& dst); 
	//Quat TFormRotation(const Quat& quat, Entity& src, Entity& dst);

	Plane TFormPlane(const Plane& plane, Mat4& src, Mat4& dst); 
	//Plane TFormPlane(const Plane& plane, Entity& src, Entity& dst);

	AABB TFormAABB(const AABB& aabb, Mat4& src, Mat4& dst); 
	AABB TFormAABB(const AABB& aabb, Entity* src, Entity* dst); 
	
}
