 

#include "C4Quaternion.h"


using namespace C4;


Quaternion& Quaternion::operator *=(const Quaternion& q)
{
	float a = w * q.x + x * q.w + y * q.z - z * q.y;
	float b = w * q.y - x * q.z + y * q.w + z * q.x;
	float c = w * q.z + x * q.y - y * q.x + z * q.w;

	w = w * q.w - x * q.x - y * q.y - z * q.z;
	x = a;
	y = b;
	z = c;

	return (*this);
}

Quaternion& Quaternion::operator *=(const Vector3D& v)
{
	float a = w * v.x + y * v.z - z * v.y;
	float b = w * v.y - x * v.z + z * v.x;
	float c = w * v.z + x * v.y - y * v.x;

	w = -x * v.x - y * v.y - z * v.z;
	x = a;
	y = b;
	z = c;

	return (*this);
}

Quaternion& Quaternion::SetRotationAboutAxis(float angle, const Vector3D& axis)
{
	Vector2D t = CosSin(angle * 0.5F);
	float s = t.y * InverseMag(axis);

	w = t.x;
	x = s * axis.x;
	y = s * axis.y;
	z = s * axis.z;

	return (*this);
}

Matrix3D Quaternion::GetRotationMatrix(void) const
{
	return (Matrix3D(1.0F - 2.0F * (y * y + z * z), 2.0F * (x * y - w * z), 2.0F * (x * z + w * y),
					 2.0F * (x * y + w * z), 1.0F - 2.0F * (x * x + z * z), 2.0F * (y * z - w * x),
					 2.0F * (x * z - w * y), 2.0F * (y * z + w * x), 1.0F - 2.0F * (x * x + y * y)));
}

Matrix3D Quaternion::GetRotationScaleMatrix(void) const
{
	float s2 = SquaredMag(*this);
	float r = InverseSqrt(s2);
	float s = s2 * r;
	float t = s * 2.0F;

	float a = w * r;
	float b = x * r;
	float c = y * r;
	float d = z * r;

	return (Matrix3D(s - t * (c * c + d * d), t * (b * c - a * d), t * (b * d + a * c),
					 t * (b * c + a * d), s - t * (b * b + d * d), t * (c * d - a * b),
					 t * (b * d - a * c), t * (c * d + a * b), s - t * (b * b + c * c)));
}

Quaternion& Quaternion::SetRotationMatrix(const Matrix3D& m)
{
	float m00 = m(0,0);
	float m11 = m(1,1);
	float m22 = m(2,2);
	float trace = m00 + m11 + m22;

	if (trace > 0.0F)
	{
		w = 0.5F * Sqrt(trace + 1.0F);
		float f = 0.25F / w;

		x = (m(2,1) - m(1,2)) * f;
		y = (m(0,2) - m(2,0)) * f;
		z = (m(1,0) - m(0,1)) * f;
	}
	else if ((m00 > m11) && (m00 > m22))
	{
		x = 0.5F * Sqrt(m00 - m11 - m22 + 1.0F);
		float f = 0.25F / x;

		y = (m(0,1) + m(1,0)) * f;
		z = (m(0,2) + m(2,0)) * f;
		w = (m(2,1) - m(1,2)) * f;
	}
	else if (m11 > m22)
	{
		y = 0.5F * Sqrt(m11 - m00 - m22 + 1.0F);
		float f = 0.25F / y;
 
		x = (m(0,1) + m(1,0)) * f;
		z = (m(1,2) + m(2,1)) * f;
		w = (m(0,2) - m(2,0)) * f; 
	}
	else 
	{
		z = 0.5F * Sqrt(m22 - m00 - m11 + 1.0F);
		float f = 0.25F / z; 

		x = (m(0,2) + m(2,0)) * f; 
		y = (m(1,2) + m(2,1)) * f; 
		w = (m(1,0) - m(0,1)) * f;
	}

	return (*this); 
}

Quaternion& Quaternion::SetRotationMatrix(const Transform4D& m)
{
	float m00 = m(0,0);
	float m11 = m(1,1);
	float m22 = m(2,2);
	float trace = m00 + m11 + m22;

	if (trace > 0.0F)
	{
		w = 0.5F * Sqrt(trace + 1.0F);
		float f = 0.25F / w;

		x = (m(2,1) - m(1,2)) * f;
		y = (m(0,2) - m(2,0)) * f;
		z = (m(1,0) - m(0,1)) * f;
	}
	else if ((m00 > m11) && (m00 > m22))
	{
		x = 0.5F * Sqrt(m00 - m11 - m22 + 1.0F);
		float f = 0.25F / x;

		y = (m(0,1) + m(1,0)) * f;
		z = (m(0,2) + m(2,0)) * f;
		w = (m(2,1) - m(1,2)) * f;
	}
	else if (m11 > m22)
	{
		y = 0.5F * Sqrt(m11 - m00 - m22 + 1.0F);
		float f = 0.25F / y;

		x = (m(0,1) + m(1,0)) * f;
		z = (m(1,2) + m(2,1)) * f;
		w = (m(0,2) - m(2,0)) * f;
	}
	else
	{
		z = 0.5F * Sqrt(m22 - m00 - m11 + 1.0F);
		float f = 0.25F / z;

		x = (m(0,2) + m(2,0)) * f;
		y = (m(1,2) + m(2,1)) * f;
		w = (m(1,0) - m(0,1)) * f;
	}

	return (*this);
}


Quaternion C4::operator *(const Quaternion& q1, const Quaternion& q2)
{
	return (Quaternion(q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
					   q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x,
					   q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w,
					   q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z));
}

Quaternion C4::operator *(const Quaternion& q, const Vector3D& v)
{
	return (Quaternion(q.w * v.x + q.y * v.z - q.z * v.y,
					   q.w * v.y - q.x * v.z + q.z * v.x,
					   q.w * v.z + q.x * v.y - q.y * v.x,
					  -q.x * v.x - q.y * v.y - q.z * v.z));
}

Vector3D C4::Transform(const Vector3D& v, const Quaternion& q)
{
	float x2 = q.x * q.x;
	float y2 = q.y * q.y;
	float z2 = q.z * q.z;

	float xy = q.x * q.y;
	float xz = q.x * q.z;
	float yz = q.y * q.z;

	float wx = q.w * q.x;
	float wy = q.w * q.y;
	float wz = q.w * q.z;

	return (Vector3D(v.x * (1.0F - 2.0F * (y2 + z2)) + 2.0F * (v.y * (xy - wz) + v.z * (xz + wy)),
					 v.y * (1.0F - 2.0F * (x2 + z2)) + 2.0F * (v.z * (yz - wx) + v.x * (xy + wz)),
					 v.z * (1.0F - 2.0F * (x2 + y2)) + 2.0F * (v.x * (xz - wy) + v.y * (yz + wx))));
}

// ZYUQURM
