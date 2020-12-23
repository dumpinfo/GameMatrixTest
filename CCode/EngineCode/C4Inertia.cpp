 

#include "C4Inertia.h"


using namespace C4;


InertiaTensor::InertiaTensor(float n00, float n11, float n22)
{
	n[0] = n00;
	n[1] = 0.0F;
	n[2] = 0.0F;
	n[3] = n11;
	n[4] = 0.0F;
	n[5] = n22;
}

InertiaTensor::InertiaTensor(float n00, float n01, float n02, float n11, float n12, float n22)
{
	n[0] = n00;
	n[1] = n01;
	n[2] = n02;
	n[3] = n11;
	n[4] = n12;
	n[5] = n22;
}

InertiaTensor& InertiaTensor::Set(float n00, float n11, float n22)
{
	n[0] = n00;
	n[1] = 0.0F;
	n[2] = 0.0F;
	n[3] = n11;
	n[4] = 0.0F;
	n[5] = n22;

	return (*this);
}

InertiaTensor& InertiaTensor::Set(float n00, float n01, float n02, float n11, float n12, float n22)
{
	n[0] = n00;
	n[1] = n01;
	n[2] = n02;
	n[3] = n11;
	n[4] = n12;
	n[5] = n22;

	return (*this);
}

Antivector3D C4::operator *(const InertiaTensor& m, const Antivector3D& v)
{
	return (Antivector3D(m[0] * v.x + m[1] * v.y + m[2] * v.z, m[1] * v.x + m[3] * v.y + m[4] * v.z, m[2] * v.x + m[4] * v.y + m[5] * v.z));
}

InertiaTensor C4::Inverse(const InertiaTensor& m)
{
	float n00 = m[0];
	float n01 = m[1];
	float n02 = m[2];
	float n11 = m[3];
	float n12 = m[4];
	float n22 = m[5];

	float p0 = n11 * n22 - n12 * n12;
	float p1 = n12 * n02 - n01 * n22;
	float p2 = n01 * n12 - n11 * n02;

	float t = 1.0F / (n00 * p0 + n01 * p1 + n02 * p2);
	return (InertiaTensor(p0 * t, p1 * t, p2 * t, (n00 * n22 - n02 * n02) * t, (n02 * n01 - n00 * n12) * t, (n00 * n11 - n01 * n01) * t));
}

InertiaTensor C4::Rotate(const InertiaTensor& m, const Transform4D& transform)
{
	float p00 = transform(0,0) * m[0] + transform(0,1) * m[1] + transform(0,2) * m[2];
	float p01 = transform(0,0) * m[1] + transform(0,1) * m[3] + transform(0,2) * m[4];
	float p02 = transform(0,0) * m[2] + transform(0,1) * m[4] + transform(0,2) * m[5];
	float p10 = transform(1,0) * m[0] + transform(1,1) * m[1] + transform(1,2) * m[2];
	float p11 = transform(1,0) * m[1] + transform(1,1) * m[3] + transform(1,2) * m[4];
	float p12 = transform(1,0) * m[2] + transform(1,1) * m[4] + transform(1,2) * m[5];
	float p20 = transform(2,0) * m[0] + transform(2,1) * m[1] + transform(2,2) * m[2];
	float p21 = transform(2,0) * m[1] + transform(2,1) * m[3] + transform(2,2) * m[4];
	float p22 = transform(2,0) * m[2] + transform(2,1) * m[4] + transform(2,2) * m[5];

	return (InertiaTensor(p00 * transform(0,0) + p01 * transform(0,1) + p02 * transform(0,2),
						  p00 * transform(1,0) + p01 * transform(1,1) + p02 * transform(1,2),
						  p00 * transform(2,0) + p01 * transform(2,1) + p02 * transform(2,2),
						  p10 * transform(1,0) + p11 * transform(1,1) + p12 * transform(1,2),
						  p10 * transform(2,0) + p11 * transform(2,1) + p12 * transform(2,2),
						  p20 * transform(2,0) + p21 * transform(2,1) + p22 * transform(2,2)));
}

InertiaTensor C4::Translate(const InertiaTensor& m, const Vector3D& dp, float mass)
{
	float x = dp.x;
	float y = dp.y;
	float z = dp.z;

	return (InertiaTensor(m[0] + mass * (y * y + z * z),
						  m[1] - mass * x * y,
						  m[2] - mass * x * z, 
						  m[3] + mass * (x * x + z * z),
						  m[4] - mass * y * z,
						  m[5] + mass * (x * x + y * y))); 
}

// ZYUQURM
