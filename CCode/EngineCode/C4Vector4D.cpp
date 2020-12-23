 

#include "C4Vector4D.h"


using namespace C4;


ConstVector4D Zero4DType::zero = {0.0F, 0.0F};

const Zero4DType C4::Zero4D = {};


Vector4D& Vector4D::RotateAboutX(float angle)
{
	Vector2D t = CosSin(angle);
	float ny = t.x * y - t.y * z;
	float nz = t.x * z + t.y * y;

	y = ny;
	z = nz;
	return (*this);
}

Vector4D& Vector4D::RotateAboutY(float angle)
{
	Vector2D t = CosSin(angle);
	float nx = t.x * x + t.y * z;
	float nz = t.x * z - t.y * x;

	x = nx;
	z = nz;
	return (*this);
}

Vector4D& Vector4D::RotateAboutZ(float angle)
{
	Vector2D t = CosSin(angle);
	float nx = t.x * x - t.y * y;
	float ny = t.x * y + t.y * x;

	x = nx;
	y = ny;
	return (*this);
}

Vector4D& Vector4D::RotateAboutAxis(float angle, const Vector3D& axis)
{
	Vector2D t = CosSin(angle);
	float u = 1.0F - t.x;

	float nx = x * (t.x + u * axis.x * axis.x) + y * (u * axis.x * axis.y - t.y * axis.z) + z * (u * axis.x * axis.z + t.y * axis.y);
	float ny = x * (u * axis.x * axis.y + t.y * axis.z) + y * (t.x + u * axis.y * axis.y) + z * (u * axis.y * axis.z - t.y * axis.x);
	float nz = x * (u * axis.x * axis.z - t.y * axis.y) + y * (u * axis.y * axis.z + t.y * axis.x) + z * (t.x + u * axis.z * axis.z);

	x = nx;
	y = ny;
	z = nz;
	return (*this);
}

// ZYUQURM
