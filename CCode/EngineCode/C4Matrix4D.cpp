 

#include "C4Matrix4D.h"
#include "C4Constants.h"


using namespace C4;


const ConstTransform4D C4::Identity4D = {{{1.0F, 0.0F, 0.0F, 0.0F}, {0.0F, 1.0F, 0.0F, 0.0F}, {0.0F, 0.0F, 1.0F, 0.0F}, {0.0F, 0.0F, 0.0F, 1.0F}}};


Matrix4D::Matrix4D(const Vector4D& c1, const Vector4D& c2, const Vector4D& c3, const Vector4D& c4)
{
	n[0][0] = c1.x;
	n[0][1] = c1.y;
	n[0][2] = c1.z;
	n[0][3] = c1.w;
	n[1][0] = c2.x;
	n[1][1] = c2.y;
	n[1][2] = c2.z;
	n[1][3] = c2.w;
	n[2][0] = c3.x;
	n[2][1] = c3.y;
	n[2][2] = c3.z;
	n[2][3] = c3.w;
	n[3][0] = c4.x;
	n[3][1] = c4.y;
	n[3][2] = c4.z;
	n[3][3] = c4.w;
}

Matrix4D::Matrix4D(float n00, float n01, float n02, float n03, float n10, float n11, float n12, float n13, float n20, float n21, float n22, float n23, float n30, float n31, float n32, float n33)
{
	n[0][0] = n00;
	n[1][0] = n01;
	n[2][0] = n02;
	n[3][0] = n03;
	n[0][1] = n10;
	n[1][1] = n11;
	n[2][1] = n12;
	n[3][1] = n13;
	n[0][2] = n20;
	n[1][2] = n21;
	n[2][2] = n22;
	n[3][2] = n23;
	n[0][3] = n30;
	n[1][3] = n31;
	n[2][3] = n32;
	n[3][3] = n33;
}

Matrix4D& Matrix4D::Set(const Vector4D& c1, const Vector4D& c2, const Vector4D& c3, const Vector4D& c4)
{
	n[0][0] = c1.x;
	n[0][1] = c1.y;
	n[0][2] = c1.z;
	n[0][3] = c1.w;
	n[1][0] = c2.x;
	n[1][1] = c2.y;
	n[1][2] = c2.z;
	n[1][3] = c2.w;
	n[2][0] = c3.x;
	n[2][1] = c3.y;
	n[2][2] = c3.z;
	n[2][3] = c3.w;
	n[3][0] = c4.x;
	n[3][1] = c4.y;
	n[3][2] = c4.z;
	n[3][3] = c4.w;

	return (*this);
}

Matrix4D& Matrix4D::Set(float n00, float n01, float n02, float n03, float n10, float n11, float n12, float n13, float n20, float n21, float n22, float n23, float n30, float n31, float n32, float n33)
{
	n[0][0] = n00;
	n[1][0] = n01;
	n[2][0] = n02;
	n[3][0] = n03;
	n[0][1] = n10;
	n[1][1] = n11;
	n[2][1] = n12;
	n[3][1] = n13;
	n[0][2] = n20;
	n[1][2] = n21;
	n[2][2] = n22;
	n[3][2] = n23;
	n[0][3] = n30;
	n[1][3] = n31;
	n[2][3] = n32;
	n[3][3] = n33;

	return (*this);
}

#if !C4SIMD

	Matrix4D& Matrix4D::operator =(const Matrix4D& m)
	{
		n[0][0] = m.n[0][0];
		n[0][1] = m.n[0][1];
		n[0][2] = m.n[0][2]; 
		n[0][3] = m.n[0][3];
		n[1][0] = m.n[1][0];
		n[1][1] = m.n[1][1]; 
		n[1][2] = m.n[1][2];
		n[1][3] = m.n[1][3]; 
		n[2][0] = m.n[2][0];
		n[2][1] = m.n[2][1];
		n[2][2] = m.n[2][2]; 
		n[2][3] = m.n[2][3];
		n[3][0] = m.n[3][0]; 
		n[3][1] = m.n[3][1]; 
		n[3][2] = m.n[3][2];
		n[3][3] = m.n[3][3];

		return (*this); 
	}

#endif

Matrix4D& Matrix4D::operator *=(const Matrix4D& m)
{
	float x = n[0][0];
	float y = n[1][0];
	float z = n[2][0];
	float w = n[3][0];
	n[0][0] = x * m.n[0][0] + y * m.n[0][1] + z * m.n[0][2] + w * m.n[0][3];
	n[1][0] = x * m.n[1][0] + y * m.n[1][1] + z * m.n[1][2] + w * m.n[1][3];
	n[2][0] = x * m.n[2][0] + y * m.n[2][1] + z * m.n[2][2] + w * m.n[2][3];
	n[3][0] = x * m.n[3][0] + y * m.n[3][1] + z * m.n[3][2] + w * m.n[3][3];

	x = n[0][1];
	y = n[1][1];
	z = n[2][1];
	w = n[3][1];
	n[0][1] = x * m.n[0][0] + y * m.n[0][1] + z * m.n[0][2] + w * m.n[0][3];
	n[1][1] = x * m.n[1][0] + y * m.n[1][1] + z * m.n[1][2] + w * m.n[1][3];
	n[2][1] = x * m.n[2][0] + y * m.n[2][1] + z * m.n[2][2] + w * m.n[2][3];
	n[3][1] = x * m.n[3][0] + y * m.n[3][1] + z * m.n[3][2] + w * m.n[3][3];

	x = n[0][2];
	y = n[1][2];
	z = n[2][2];
	w = n[3][2];
	n[0][2] = x * m.n[0][0] + y * m.n[0][1] + z * m.n[0][2] + w * m.n[0][3];
	n[1][2] = x * m.n[1][0] + y * m.n[1][1] + z * m.n[1][2] + w * m.n[1][3];
	n[2][2] = x * m.n[2][0] + y * m.n[2][1] + z * m.n[2][2] + w * m.n[2][3];
	n[3][2] = x * m.n[3][0] + y * m.n[3][1] + z * m.n[3][2] + w * m.n[3][3];

	x = n[0][3];
	y = n[1][3];
	z = n[2][3];
	w = n[3][3];
	n[0][3] = x * m.n[0][0] + y * m.n[0][1] + z * m.n[0][2] + w * m.n[0][3];
	n[1][3] = x * m.n[1][0] + y * m.n[1][1] + z * m.n[1][2] + w * m.n[1][3];
	n[2][3] = x * m.n[2][0] + y * m.n[2][1] + z * m.n[2][2] + w * m.n[2][3];
	n[3][3] = x * m.n[3][0] + y * m.n[3][1] + z * m.n[3][2] + w * m.n[3][3];

	return (*this);
}

Matrix4D& Matrix4D::operator *=(const Matrix3D& m)
{
	float x = n[0][0];
	float y = n[1][0];
	float z = n[2][0];
	n[0][0] = x * m.n[0][0] + y * m.n[0][1] + z * m.n[0][2];
	n[1][0] = x * m.n[1][0] + y * m.n[1][1] + z * m.n[1][2];
	n[2][0] = x * m.n[2][0] + y * m.n[2][1] + z * m.n[2][2];

	x = n[0][1];
	y = n[1][1];
	z = n[2][1];
	n[0][1] = x * m.n[0][0] + y * m.n[0][1] + z * m.n[0][2];
	n[1][1] = x * m.n[1][0] + y * m.n[1][1] + z * m.n[1][2];
	n[2][1] = x * m.n[2][0] + y * m.n[2][1] + z * m.n[2][2];

	x = n[0][2];
	y = n[1][2];
	z = n[2][2];
	n[0][2] = x * m.n[0][0] + y * m.n[0][1] + z * m.n[0][2];
	n[1][2] = x * m.n[1][0] + y * m.n[1][1] + z * m.n[1][2];
	n[2][2] = x * m.n[2][0] + y * m.n[2][1] + z * m.n[2][2];

	x = n[0][3];
	y = n[1][3];
	z = n[2][3];
	n[0][3] = x * m.n[0][0] + y * m.n[0][1] + z * m.n[0][2];
	n[1][3] = x * m.n[1][0] + y * m.n[1][1] + z * m.n[1][2];
	n[2][3] = x * m.n[2][0] + y * m.n[2][1] + z * m.n[2][2];

	return (*this);
}

Matrix4D& Matrix4D::SetIdentity(void)
{
	n[0][0] = n[1][1] = n[2][2] = n[3][3] = 1.0F;
	n[1][0] = n[2][0] = n[3][0] = n[0][1] = n[2][1] = n[3][1] = n[0][2] = n[1][2] = n[3][2] = n[0][3] = n[1][3] = n[2][3] = 0.0F;
	return (*this);
}


Matrix4D C4::operator *(const Matrix4D& m1, const Matrix4D& m2)
{
	return (Matrix4D(m1.n[0][0] * m2.n[0][0] + m1.n[1][0] * m2.n[0][1] + m1.n[2][0] * m2.n[0][2] + m1.n[3][0] * m2.n[0][3],
					 m1.n[0][0] * m2.n[1][0] + m1.n[1][0] * m2.n[1][1] + m1.n[2][0] * m2.n[1][2] + m1.n[3][0] * m2.n[1][3],
					 m1.n[0][0] * m2.n[2][0] + m1.n[1][0] * m2.n[2][1] + m1.n[2][0] * m2.n[2][2] + m1.n[3][0] * m2.n[2][3],
					 m1.n[0][0] * m2.n[3][0] + m1.n[1][0] * m2.n[3][1] + m1.n[2][0] * m2.n[3][2] + m1.n[3][0] * m2.n[3][3],
					 m1.n[0][1] * m2.n[0][0] + m1.n[1][1] * m2.n[0][1] + m1.n[2][1] * m2.n[0][2] + m1.n[3][1] * m2.n[0][3],
					 m1.n[0][1] * m2.n[1][0] + m1.n[1][1] * m2.n[1][1] + m1.n[2][1] * m2.n[1][2] + m1.n[3][1] * m2.n[1][3],
					 m1.n[0][1] * m2.n[2][0] + m1.n[1][1] * m2.n[2][1] + m1.n[2][1] * m2.n[2][2] + m1.n[3][1] * m2.n[2][3],
					 m1.n[0][1] * m2.n[3][0] + m1.n[1][1] * m2.n[3][1] + m1.n[2][1] * m2.n[3][2] + m1.n[3][1] * m2.n[3][3],
					 m1.n[0][2] * m2.n[0][0] + m1.n[1][2] * m2.n[0][1] + m1.n[2][2] * m2.n[0][2] + m1.n[3][2] * m2.n[0][3],
					 m1.n[0][2] * m2.n[1][0] + m1.n[1][2] * m2.n[1][1] + m1.n[2][2] * m2.n[1][2] + m1.n[3][2] * m2.n[1][3],
					 m1.n[0][2] * m2.n[2][0] + m1.n[1][2] * m2.n[2][1] + m1.n[2][2] * m2.n[2][2] + m1.n[3][2] * m2.n[2][3],
					 m1.n[0][2] * m2.n[3][0] + m1.n[1][2] * m2.n[3][1] + m1.n[2][2] * m2.n[3][2] + m1.n[3][2] * m2.n[3][3],
					 m1.n[0][3] * m2.n[0][0] + m1.n[1][3] * m2.n[0][1] + m1.n[2][3] * m2.n[0][2] + m1.n[3][3] * m2.n[0][3],
					 m1.n[0][3] * m2.n[1][0] + m1.n[1][3] * m2.n[1][1] + m1.n[2][3] * m2.n[1][2] + m1.n[3][3] * m2.n[1][3],
					 m1.n[0][3] * m2.n[2][0] + m1.n[1][3] * m2.n[2][1] + m1.n[2][3] * m2.n[2][2] + m1.n[3][3] * m2.n[2][3],
					 m1.n[0][3] * m2.n[3][0] + m1.n[1][3] * m2.n[3][1] + m1.n[2][3] * m2.n[3][2] + m1.n[3][3] * m2.n[3][3]));
}

Matrix4D C4::operator *(const Matrix4D& m1, const Matrix3D& m2)
{
	return (Matrix4D(m1.n[0][0] * m2.n[0][0] + m1.n[1][0] * m2.n[0][1] + m1.n[2][0] * m2.n[0][2],
					 m1.n[0][0] * m2.n[1][0] + m1.n[1][0] * m2.n[1][1] + m1.n[2][0] * m2.n[1][2],
					 m1.n[0][0] * m2.n[2][0] + m1.n[1][0] * m2.n[2][1] + m1.n[2][0] * m2.n[2][2],
					 m1.n[3][0],
					 m1.n[0][1] * m2.n[0][0] + m1.n[1][1] * m2.n[0][1] + m1.n[2][1] * m2.n[0][2],
					 m1.n[0][1] * m2.n[1][0] + m1.n[1][1] * m2.n[1][1] + m1.n[2][1] * m2.n[1][2],
					 m1.n[0][1] * m2.n[2][0] + m1.n[1][1] * m2.n[2][1] + m1.n[2][1] * m2.n[2][2],
					 m1.n[3][1],
					 m1.n[0][2] * m2.n[0][0] + m1.n[1][2] * m2.n[0][1] + m1.n[2][2] * m2.n[0][2],
					 m1.n[0][2] * m2.n[1][0] + m1.n[1][2] * m2.n[1][1] + m1.n[2][2] * m2.n[1][2],
					 m1.n[0][2] * m2.n[2][0] + m1.n[1][2] * m2.n[2][1] + m1.n[2][2] * m2.n[2][2],
					 m1.n[3][2],
					 m1.n[0][3] * m2.n[0][0] + m1.n[1][3] * m2.n[0][1] + m1.n[2][3] * m2.n[0][2],
					 m1.n[0][3] * m2.n[1][0] + m1.n[1][3] * m2.n[1][1] + m1.n[2][3] * m2.n[1][2],
					 m1.n[0][3] * m2.n[2][0] + m1.n[1][3] * m2.n[2][1] + m1.n[2][3] * m2.n[2][2],
					 m1.n[3][3]));
}

Vector4D C4::operator *(const Matrix4D& m, const Vector4D& v)
{
	return (Vector4D(m.n[0][0] * v.x + m.n[1][0] * v.y + m.n[2][0] * v.z + m.n[3][0] * v.w,
					 m.n[0][1] * v.x + m.n[1][1] * v.y + m.n[2][1] * v.z + m.n[3][1] * v.w,
					 m.n[0][2] * v.x + m.n[1][2] * v.y + m.n[2][2] * v.z + m.n[3][2] * v.w,
					 m.n[0][3] * v.x + m.n[1][3] * v.y + m.n[2][3] * v.z + m.n[3][3] * v.w));
}

Antivector4D C4::operator *(const Antivector4D& v, const Matrix4D& m)
{
	return (Antivector4D(m.n[0][0] * v.x + m.n[0][1] * v.y + m.n[0][2] * v.z + m.n[0][3] * v.w,
						 m.n[1][0] * v.x + m.n[1][1] * v.y + m.n[1][2] * v.z + m.n[1][3] * v.w,
						 m.n[2][0] * v.x + m.n[2][1] * v.y + m.n[2][2] * v.z + m.n[2][3] * v.w,
						 m.n[3][0] * v.x + m.n[3][1] * v.y + m.n[3][2] * v.z + m.n[3][3] * v.w));
}

Vector4D C4::operator *(const Matrix4D& m, const Vector3D& v)
{
	return (Vector4D(m.n[0][0] * v.x + m.n[1][0] * v.y + m.n[2][0] * v.z,
					 m.n[0][1] * v.x + m.n[1][1] * v.y + m.n[2][1] * v.z,
					 m.n[0][2] * v.x + m.n[1][2] * v.y + m.n[2][2] * v.z,
					 m.n[0][3] * v.x + m.n[1][3] * v.y + m.n[2][3] * v.z));
}

Vector4D C4::operator *(const Vector3D& v, const Matrix4D& m)
{
	return (Vector4D(m.n[0][0] * v.x + m.n[0][1] * v.y + m.n[0][2] * v.z,
					 m.n[1][0] * v.x + m.n[1][1] * v.y + m.n[1][2] * v.z,
					 m.n[2][0] * v.x + m.n[2][1] * v.y + m.n[2][2] * v.z,
					 m.n[3][0] * v.x + m.n[3][1] * v.y + m.n[3][2] * v.z));
}

Vector4D C4::operator *(const Matrix4D& m, const Point3D& p)
{
	return (Vector4D(m.n[0][0] * p.x + m.n[1][0] * p.y + m.n[2][0] * p.z + m.n[3][0],
					 m.n[0][1] * p.x + m.n[1][1] * p.y + m.n[2][1] * p.z + m.n[3][1],
					 m.n[0][2] * p.x + m.n[1][2] * p.y + m.n[2][2] * p.z + m.n[3][2],
					 m.n[0][3] * p.x + m.n[1][3] * p.y + m.n[2][3] * p.z + m.n[3][3]));
}

Vector4D C4::operator *(const Point3D& p, const Matrix4D& m)
{
	return (Vector4D(m.n[0][0] * p.x + m.n[0][1] * p.y + m.n[0][2] * p.z + m.n[0][3],
					 m.n[1][0] * p.x + m.n[1][1] * p.y + m.n[1][2] * p.z + m.n[1][3],
					 m.n[2][0] * p.x + m.n[2][1] * p.y + m.n[2][2] * p.z + m.n[2][3],
					 m.n[3][0] * p.x + m.n[3][1] * p.y + m.n[3][2] * p.z + m.n[3][3]));
}

Vector4D C4::operator *(const Matrix4D& m, const Vector2D& v)
{
	return (Vector4D(m.n[0][0] * v.x + m.n[1][0] * v.y,
					 m.n[0][1] * v.x + m.n[1][1] * v.y,
					 m.n[0][2] * v.x + m.n[1][2] * v.y,
					 m.n[0][3] * v.x + m.n[1][3] * v.y));
}

Vector4D C4::operator *(const Vector2D& v, const Matrix4D& m)
{
	return (Vector4D(m.n[0][0] * v.x + m.n[0][1] * v.y,
					 m.n[1][0] * v.x + m.n[1][1] * v.y,
					 m.n[2][0] * v.x + m.n[2][1] * v.y,
					 m.n[3][0] * v.x + m.n[3][1] * v.y));
}

Vector4D C4::operator *(const Matrix4D& m, const Point2D& p)
{
	return (Vector4D(m.n[0][0] * p.x + m.n[1][0] * p.y + m.n[3][0],
					 m.n[0][1] * p.x + m.n[1][1] * p.y + m.n[3][1],
					 m.n[0][2] * p.x + m.n[1][2] * p.y + m.n[3][2],
					 m.n[0][3] * p.x + m.n[1][3] * p.y + m.n[3][3]));
}

Vector4D C4::operator *(const Point2D& p, const Matrix4D& m)
{
	return (Vector4D(m.n[0][0] * p.x + m.n[0][1] * p.y + m.n[0][3],
					 m.n[1][0] * p.x + m.n[1][1] * p.y + m.n[1][3],
					 m.n[2][0] * p.x + m.n[2][1] * p.y + m.n[2][3],
					 m.n[3][0] * p.x + m.n[3][1] * p.y + m.n[3][3]));
}

bool C4::operator ==(const Matrix4D& m1, const Matrix4D& m2)
{
	return ((m1.n[0][0] == m2.n[0][0]) && (m1.n[0][1] == m2.n[0][1]) && (m1.n[0][2] == m2.n[0][2]) && (m1.n[0][3] == m2.n[0][3]) && (m1.n[1][0] == m2.n[1][0]) && (m1.n[1][1] == m2.n[1][1]) && (m1.n[1][2] == m2.n[1][2]) && (m1.n[1][3] == m2.n[1][3]) && (m1.n[2][0] == m2.n[2][0]) && (m1.n[2][1] == m2.n[2][1]) && (m1.n[2][2] == m2.n[2][2]) && (m1.n[2][3] == m2.n[2][3]) && (m1.n[3][0] == m2.n[3][0]) && (m1.n[3][1] == m2.n[3][1]) && (m1.n[3][2] == m2.n[3][2]) && (m1.n[3][3] == m2.n[3][3]));
}

bool C4::operator !=(const Matrix4D& m1, const Matrix4D& m2)
{
	return ((m1.n[0][0] != m2.n[0][0]) || (m1.n[0][1] != m2.n[0][1]) || (m1.n[0][2] != m2.n[0][2]) || (m1.n[0][3] != m2.n[0][3]) || (m1.n[1][0] != m2.n[1][0]) || (m1.n[1][1] != m2.n[1][1]) || (m1.n[1][2] != m2.n[1][2]) || (m1.n[1][3] != m2.n[1][3]) || (m1.n[2][0] != m2.n[2][0]) || (m1.n[2][1] != m2.n[2][1]) || (m1.n[2][2] != m2.n[2][2]) || (m1.n[2][3] != m2.n[2][3]) || (m1.n[3][0] != m2.n[3][0]) || (m1.n[3][1] != m2.n[3][1]) || (m1.n[3][2] != m2.n[3][2]) || (m1.n[3][3] != m2.n[3][3]));
}

float C4::Determinant(const Matrix4D& m)
{
	float n00 = m(0,0);
	float n01 = m(0,1);
	float n02 = m(0,2);
	float n03 = m(0,3);

	float n10 = m(1,0);
	float n11 = m(1,1);
	float n12 = m(1,2);
	float n13 = m(1,3);

	float n20 = m(2,0);
	float n21 = m(2,1);
	float n22 = m(2,2);
	float n23 = m(2,3);

	float n30 = m(3,0);
	float n31 = m(3,1);
	float n32 = m(3,2);
	float n33 = m(3,3);

	return (n00 * (n11 * (n22 * n33 - n23 * n32) + n12 * (n23 * n31 - n21 * n33) + n13 * (n21 * n32 - n22 * n31)) +
			n01 * (n10 * (n23 * n32 - n22 * n33) + n12 * (n20 * n33 - n23 * n30) + n13 * (n22 * n30 - n20 * n32)) +
			n02 * (n10 * (n21 * n33 - n23 * n31) + n11 * (n23 * n30 - n20 * n33) + n13 * (n20 * n31 - n21 * n30)) +
			n03 * (n10 * (n22 * n31 - n21 * n32) + n11 * (n20 * n32 - n22 * n30) + n12 * (n21 * n30 - n20 * n31)));
}

Matrix4D C4::Inverse(const Matrix4D& m)
{
	float n00 = m(0,0);
	float n01 = m(0,1);
	float n02 = m(0,2);
	float n03 = m(0,3);

	float n10 = m(1,0);
	float n11 = m(1,1);
	float n12 = m(1,2);
	float n13 = m(1,3);

	float n20 = m(2,0);
	float n21 = m(2,1);
	float n22 = m(2,2);
	float n23 = m(2,3);

	float n30 = m(3,0);
	float n31 = m(3,1);
	float n32 = m(3,2);
	float n33 = m(3,3);

	float p00 = n11 * (n22 * n33 - n23 * n32) + n12 * (n23 * n31 - n21 * n33) + n13 * (n21 * n32 - n22 * n31);
	float p10 = n10 * (n23 * n32 - n22 * n33) + n12 * (n20 * n33 - n23 * n30) + n13 * (n22 * n30 - n20 * n32);
	float p20 = n10 * (n21 * n33 - n23 * n31) + n11 * (n23 * n30 - n20 * n33) + n13 * (n20 * n31 - n21 * n30);
	float p30 = n10 * (n22 * n31 - n21 * n32) + n11 * (n20 * n32 - n22 * n30) + n12 * (n21 * n30 - n20 * n31);

	float t = 1.0F / (n00 * p00 + n01 * p10 + n02 * p20 + n03 * p30);

	return (Matrix4D(p00 * t,
					 (n01 * (n23 * n32 - n22 * n33) + n02 * (n21 * n33 - n23 * n31) + n03 * (n22 * n31 - n21 * n32)) * t,
					 (n01 * (n12 * n33 - n13 * n32) + n02 * (n13 * n31 - n11 * n33) + n03 * (n11 * n32 - n12 * n31)) * t,
					 (n01 * (n13 * n22 - n12 * n23) + n02 * (n11 * n23 - n13 * n21) + n03 * (n12 * n21 - n11 * n22)) * t,
					 p10 * t,
					 (n00 * (n22 * n33 - n23 * n32) + n02 * (n23 * n30 - n20 * n33) + n03 * (n20 * n32 - n22 * n30)) * t,
					 (n00 * (n13 * n32 - n12 * n33) + n02 * (n10 * n33 - n13 * n30) + n03 * (n12 * n30 - n10 * n32)) * t,
					 (n00 * (n12 * n23 - n13 * n22) + n02 * (n13 * n20 - n10 * n23) + n03 * (n10 * n22 - n12 * n20)) * t,
					 p20 * t,
					 (n00 * (n23 * n31 - n21 * n33) + n01 * (n20 * n33 - n23 * n30) + n03 * (n21 * n30 - n20 * n31)) * t,
					 (n00 * (n11 * n33 - n13 * n31) + n01 * (n13 * n30 - n10 * n33) + n03 * (n10 * n31 - n11 * n30)) * t,
					 (n00 * (n13 * n21 - n11 * n23) + n01 * (n10 * n23 - n13 * n20) + n03 * (n11 * n20 - n10 * n21)) * t,
					 p30 * t,
					 (n00 * (n21 * n32 - n22 * n31) + n01 * (n22 * n30 - n20 * n32) + n02 * (n20 * n31 - n21 * n30)) * t,
					 (n00 * (n12 * n31 - n11 * n32) + n01 * (n10 * n32 - n12 * n30) + n02 * (n11 * n30 - n10 * n31)) * t,
					 (n00 * (n11 * n22 - n12 * n21) + n01 * (n12 * n20 - n10 * n22) + n02 * (n10 * n21 - n11 * n20)) * t));
}

Matrix4D C4::Adjugate(const Matrix4D& m)
{
	float n00 = m(0,0);
	float n01 = m(0,1);
	float n02 = m(0,2);
	float n03 = m(0,3);

	float n10 = m(1,0);
	float n11 = m(1,1);
	float n12 = m(1,2);
	float n13 = m(1,3);

	float n20 = m(2,0);
	float n21 = m(2,1);
	float n22 = m(2,2);
	float n23 = m(2,3);

	float n30 = m(3,0);
	float n31 = m(3,1);
	float n32 = m(3,2);
	float n33 = m(3,3);

	return (Matrix4D(n11 * (n22 * n33 - n23 * n32) + n12 * (n23 * n31 - n21 * n33) + n13 * (n21 * n32 - n22 * n31),
					 n01 * (n23 * n32 - n22 * n33) + n02 * (n21 * n33 - n23 * n31) + n03 * (n22 * n31 - n21 * n32),
					 n01 * (n12 * n33 - n13 * n32) + n02 * (n13 * n31 - n11 * n33) + n03 * (n11 * n32 - n12 * n31),
					 n01 * (n13 * n22 - n12 * n23) + n02 * (n11 * n23 - n13 * n21) + n03 * (n12 * n21 - n11 * n22),
					 n10 * (n23 * n32 - n22 * n33) + n12 * (n20 * n33 - n23 * n30) + n13 * (n22 * n30 - n20 * n32),
					 n00 * (n22 * n33 - n23 * n32) + n02 * (n23 * n30 - n20 * n33) + n03 * (n20 * n32 - n22 * n30),
					 n00 * (n13 * n32 - n12 * n33) + n02 * (n10 * n33 - n13 * n30) + n03 * (n12 * n30 - n10 * n32),
					 n00 * (n12 * n23 - n13 * n22) + n02 * (n13 * n20 - n10 * n23) + n03 * (n10 * n22 - n12 * n20),
					 n10 * (n21 * n33 - n23 * n31) + n11 * (n23 * n30 - n20 * n33) + n13 * (n20 * n31 - n21 * n30),
					 n00 * (n23 * n31 - n21 * n33) + n01 * (n20 * n33 - n23 * n30) + n03 * (n21 * n30 - n20 * n31),
					 n00 * (n11 * n33 - n13 * n31) + n01 * (n13 * n30 - n10 * n33) + n03 * (n10 * n31 - n11 * n30),
					 n00 * (n13 * n21 - n11 * n23) + n01 * (n10 * n23 - n13 * n20) + n03 * (n11 * n20 - n10 * n21),
					 n10 * (n22 * n31 - n21 * n32) + n11 * (n20 * n32 - n22 * n30) + n12 * (n21 * n30 - n20 * n31),
					 n00 * (n21 * n32 - n22 * n31) + n01 * (n22 * n30 - n20 * n32) + n02 * (n20 * n31 - n21 * n30),
					 n00 * (n12 * n31 - n11 * n32) + n01 * (n10 * n32 - n12 * n30) + n02 * (n11 * n30 - n10 * n31),
					 n00 * (n11 * n22 - n12 * n21) + n01 * (n12 * n20 - n10 * n22) + n02 * (n10 * n21 - n11 * n20)));
}

Matrix4D C4::Transpose(const Matrix4D& m)
{
	return (Matrix4D(m(0,0), m(1,0), m(2,0), m(3,0), m(0,1), m(1,1), m(2,1), m(3,1), m(0,2), m(1,2), m(2,2), m(3,2), m(0,3), m(1,3), m(2,3), m(3,3)));
}


Transform4D::Transform4D(const Matrix3D& m)
{
	n[0][0] = m.n[0][0];
	n[0][1] = m.n[0][1];
	n[0][2] = m.n[0][2];
	n[1][0] = m.n[1][0];
	n[1][1] = m.n[1][1];
	n[1][2] = m.n[1][2];
	n[2][0] = m.n[2][0];
	n[2][1] = m.n[2][1];
	n[2][2] = m.n[2][2];

	n[0][3] = n[1][3] = n[2][3] = n[3][0] = n[3][1] = n[3][2] = 0.0F;
	n[3][3] = 1.0F;
}

Transform4D::Transform4D(const Matrix3D& m, const Vector3D& v)
{
	n[0][0] = m.n[0][0];
	n[0][1] = m.n[0][1];
	n[0][2] = m.n[0][2];
	n[1][0] = m.n[1][0];
	n[1][1] = m.n[1][1];
	n[1][2] = m.n[1][2];
	n[2][0] = m.n[2][0];
	n[2][1] = m.n[2][1];
	n[2][2] = m.n[2][2];
	n[3][0] = v.x;
	n[3][1] = v.y;
	n[3][2] = v.z;

	n[0][3] = n[1][3] = n[2][3] = 0.0F;
	n[3][3] = 1.0F;
}

Transform4D::Transform4D(const Vector3D& c1, const Vector3D& c2, const Vector3D& c3, const Point3D& c4)
{
	n[0][0] = c1.x;
	n[0][1] = c1.y;
	n[0][2] = c1.z;
	n[1][0] = c2.x;
	n[1][1] = c2.y;
	n[1][2] = c2.z;
	n[2][0] = c3.x;
	n[2][1] = c3.y;
	n[2][2] = c3.z;
	n[3][0] = c4.x;
	n[3][1] = c4.y;
	n[3][2] = c4.z;

	n[0][3] = n[1][3] = n[2][3] = 0.0F;
	n[3][3] = 1.0F;
}

Transform4D::Transform4D(float n00, float n01, float n02, float n03, float n10, float n11, float n12, float n13, float n20, float n21, float n22, float n23)
{
	n[0][0] = n00;
	n[1][0] = n01;
	n[2][0] = n02;
	n[3][0] = n03;
	n[0][1] = n10;
	n[1][1] = n11;
	n[2][1] = n12;
	n[3][1] = n13;
	n[0][2] = n20;
	n[1][2] = n21;
	n[2][2] = n22;
	n[3][2] = n23;

	n[0][3] = n[1][3] = n[2][3] = 0.0F;
	n[3][3] = 1.0F;
}

Transform4D& Transform4D::Set(const Matrix3D& m, const Vector3D& v)
{
	n[0][0] = m.n[0][0];
	n[0][1] = m.n[0][1];
	n[0][2] = m.n[0][2];
	n[1][0] = m.n[1][0];
	n[1][1] = m.n[1][1];
	n[1][2] = m.n[1][2];
	n[2][0] = m.n[2][0];
	n[2][1] = m.n[2][1];
	n[2][2] = m.n[2][2];
	n[3][0] = v.x;
	n[3][1] = v.y;
	n[3][2] = v.z;

	n[0][3] = n[1][3] = n[2][3] = 0.0F;
	n[3][3] = 1.0F;

	return (*this);
}

Transform4D& Transform4D::Set(const Vector3D& c1, const Vector3D& c2, const Vector3D& c3, const Point3D& c4)
{
	n[0][0] = c1.x;
	n[0][1] = c1.y;
	n[0][2] = c1.z;
	n[1][0] = c2.x;
	n[1][1] = c2.y;
	n[1][2] = c2.z;
	n[2][0] = c3.x;
	n[2][1] = c3.y;
	n[2][2] = c3.z;
	n[3][0] = c4.x;
	n[3][1] = c4.y;
	n[3][2] = c4.z;

	n[0][3] = n[1][3] = n[2][3] = 0.0F;
	n[3][3] = 1.0F;

	return (*this);
}

Transform4D& Transform4D::Set(float n00, float n01, float n02, float n03, float n10, float n11, float n12, float n13, float n20, float n21, float n22, float n23)
{
	n[0][0] = n00;
	n[1][0] = n01;
	n[2][0] = n02;
	n[3][0] = n03;
	n[0][1] = n10;
	n[1][1] = n11;
	n[2][1] = n12;
	n[3][1] = n13;
	n[0][2] = n20;
	n[1][2] = n21;
	n[2][2] = n22;
	n[3][2] = n23;

	n[0][3] = n[1][3] = n[2][3] = 0.0F;
	n[3][3] = 1.0F;

	return (*this);
}

Transform4D& Transform4D::operator *=(const Transform4D& m)
{
	float x = n[0][0];
	float y = n[1][0];
	float z = n[2][0];
	n[0][0] = x * m.n[0][0] + y * m.n[0][1] + z * m.n[0][2];
	n[1][0] = x * m.n[1][0] + y * m.n[1][1] + z * m.n[1][2];
	n[2][0] = x * m.n[2][0] + y * m.n[2][1] + z * m.n[2][2];
	n[3][0] = x * m.n[3][0] + y * m.n[3][1] + z * m.n[3][2] + n[3][0];

	x = n[0][1];
	y = n[1][1];
	z = n[2][1];
	n[0][1] = x * m.n[0][0] + y * m.n[0][1] + z * m.n[0][2];
	n[1][1] = x * m.n[1][0] + y * m.n[1][1] + z * m.n[1][2];
	n[2][1] = x * m.n[2][0] + y * m.n[2][1] + z * m.n[2][2];
	n[3][1] = x * m.n[3][0] + y * m.n[3][1] + z * m.n[3][2] + n[3][1];

	x = n[0][2];
	y = n[1][2];
	z = n[2][2];
	n[0][2] = x * m.n[0][0] + y * m.n[0][1] + z * m.n[0][2];
	n[1][2] = x * m.n[1][0] + y * m.n[1][1] + z * m.n[1][2];
	n[2][2] = x * m.n[2][0] + y * m.n[2][1] + z * m.n[2][2];
	n[3][2] = x * m.n[3][0] + y * m.n[3][1] + z * m.n[3][2] + n[3][2];

	return (*this);
}

Transform4D& Transform4D::operator *=(const Matrix3D& m)
{
	float x = n[0][0];
	float y = n[1][0];
	float z = n[2][0];
	n[0][0] = x * m.n[0][0] + y * m.n[0][1] + z * m.n[0][2];
	n[1][0] = x * m.n[1][0] + y * m.n[1][1] + z * m.n[1][2];
	n[2][0] = x * m.n[2][0] + y * m.n[2][1] + z * m.n[2][2];

	x = n[0][1];
	y = n[1][1];
	z = n[2][1];
	n[0][1] = x * m.n[0][0] + y * m.n[0][1] + z * m.n[0][2];
	n[1][1] = x * m.n[1][0] + y * m.n[1][1] + z * m.n[1][2];
	n[2][1] = x * m.n[2][0] + y * m.n[2][1] + z * m.n[2][2];

	x = n[0][2];
	y = n[1][2];
	z = n[2][2];
	n[0][2] = x * m.n[0][0] + y * m.n[0][1] + z * m.n[0][2];
	n[1][2] = x * m.n[1][0] + y * m.n[1][1] + z * m.n[1][2];
	n[2][2] = x * m.n[2][0] + y * m.n[2][1] + z * m.n[2][2];

	return (*this);
}

Matrix3D Transform4D::GetMatrix3D(void) const
{
	return (Matrix3D(n[0][0], n[1][0], n[2][0], n[0][1], n[1][1], n[2][1], n[0][2], n[1][2], n[2][2]));
}

Transform4D& Transform4D::SetMatrix3D(const Matrix3D& m)
{
	n[0][0] = m.n[0][0];
	n[0][1] = m.n[0][1];
	n[0][2] = m.n[0][2];
	n[1][0] = m.n[1][0];
	n[1][1] = m.n[1][1];
	n[1][2] = m.n[1][2];
	n[2][0] = m.n[2][0];
	n[2][1] = m.n[2][1];
	n[2][2] = m.n[2][2];

	return (*this);
}

Transform4D& Transform4D::SetMatrix3D(const Transform4D& m)
{
	n[0][0] = m.n[0][0];
	n[0][1] = m.n[0][1];
	n[0][2] = m.n[0][2];
	n[1][0] = m.n[1][0];
	n[1][1] = m.n[1][1];
	n[1][2] = m.n[1][2];
	n[2][0] = m.n[2][0];
	n[2][1] = m.n[2][1];
	n[2][2] = m.n[2][2];

	return (*this);
}

Transform4D& Transform4D::SetMatrix3D(const Vector3D& c1, const Vector3D& c2, const Vector3D& c3)
{
	n[0][0] = c1.x;
	n[0][1] = c1.y;
	n[0][2] = c1.z;
	n[1][0] = c2.x;
	n[1][1] = c2.y;
	n[1][2] = c2.z;
	n[2][0] = c3.x;
	n[2][1] = c3.y;
	n[2][2] = c3.z;

	return (*this);
}

Transform4D& Transform4D::SetMatrix3D(float n00, float n01, float n02, float n10, float n11, float n12, float n20, float n21, float n22)
{
	n[0][0] = n00;
	n[1][0] = n01;
	n[2][0] = n02;
	n[0][1] = n10;
	n[1][1] = n11;
	n[2][1] = n12;
	n[0][2] = n20;
	n[1][2] = n21;
	n[2][2] = n22;

	return (*this);
}

Transform4D& Transform4D::SetRotationAboutX(float angle)
{
	CosSin(angle, &n[1][1], &n[1][2]);

	n[2][2] = n[1][1];
	n[2][1] = -n[1][2];
	n[0][0] = n[3][3] = 1.0F;
	n[0][1] = n[0][2] = n[0][3] = n[1][0] = n[1][3] = n[2][0] = n[2][3] = n[3][0] = n[3][1] = n[3][2] = 0.0F;

	return (*this);
}

Transform4D& Transform4D::SetRotationAboutY(float angle)
{
	CosSin(angle, &n[0][0], &n[2][0]);

	n[2][2] = n[0][0];
	n[0][2] = -n[2][0];
	n[1][1] = n[3][3] = 1.0F;
	n[0][1] = n[0][3] = n[1][0] = n[1][2] = n[1][3] = n[2][1] = n[2][3] = n[3][0] = n[3][1] = n[3][2] = 0.0F;

	return (*this);
}

Transform4D& Transform4D::SetRotationAboutZ(float angle)
{
	CosSin(angle, &n[0][0], &n[0][1]);

	n[1][1] = n[0][0];
	n[1][0] = -n[0][1];
	n[2][2] = n[3][3] = 1.0F;
	n[0][2] = n[0][3] = n[1][2] = n[1][3] = n[2][0] = n[2][1] = n[2][3] = n[3][0] = n[3][1] = n[3][2] = 0.0F;

	return (*this);
}

Transform4D& Transform4D::SetRotationAboutAxis(float angle, const Antivector3D& axis)
{
	Vector2D t = CosSin(angle);
	float u = 1.0F - t.x;

	n[0][0] = t.x + u * axis.x * axis.x;
	n[0][1] = u * axis.x * axis.y + t.y * axis.z;
	n[0][2] = u * axis.x * axis.z - t.y * axis.y;
	n[1][0] = u * axis.x * axis.y - t.y * axis.z;
	n[1][1] = t.x + u * axis.y * axis.y;
	n[1][2] = u * axis.y * axis.z + t.y * axis.x;
	n[2][0] = u * axis.x * axis.z + t.y * axis.y;
	n[2][1] = u * axis.y * axis.z - t.y * axis.x;
	n[2][2] = t.x + u * axis.z * axis.z;

	n[3][3] = 1.0F;
	n[0][3] = n[1][3] = n[2][3] = n[3][0] = n[3][1] = n[3][2] = 0.0F;

	return (*this);
}

Transform4D& Transform4D::SetPlaneReflection(const Antivector4D& plane)
{
	float px = plane.x * -2.0F;
	float py = plane.y * -2.0F;
	float pz = plane.z * -2.0F;
	float pw = plane.w * -2.0F;

	n[0][0] = 1.0F + px * plane.x;
	n[0][1] = px * plane.y;
	n[0][2] = px * plane.z;
	n[1][0] = n[0][1];
	n[1][1] = 1.0F + py * plane.y;
	n[1][2] = py * plane.z;
	n[2][0] = n[0][2];
	n[2][1] = n[1][2];
	n[2][2] = 1.0F + pz * plane.z;
	n[3][0] = pw * plane.x;
	n[3][1] = pw * plane.y;
	n[3][2] = pw * plane.z;

	n[0][3] = n[1][3] = n[2][3] = 0.0F;
	n[3][3] = 1.0F;

	return (*this);
}

Transform4D& Transform4D::SetPlaneReflection(const MatrixRow4D& plane)
{
	float px = plane[0] * -2.0F;
	float py = plane[1] * -2.0F;
	float pz = plane[2] * -2.0F;
	float pw = plane[3] * -2.0F;

	n[0][0] = 1.0F + px * plane[0];
	n[0][1] = px * plane[1];
	n[0][2] = px * plane[2];
	n[1][0] = n[0][1];
	n[1][1] = 1.0F + py * plane[1];
	n[1][2] = py * plane[2];
	n[2][0] = n[0][2];
	n[2][1] = n[1][2];
	n[2][2] = 1.0F + pz * plane[2];
	n[3][0] = pw * plane[0];
	n[3][1] = pw * plane[1];
	n[3][2] = pw * plane[2];

	n[0][3] = n[1][3] = n[2][3] = 0.0F;
	n[3][3] = 1.0F;

	return (*this);
}

void Transform4D::GetEulerAngles(float *x, float *y, float *z) const
{
	float sy = n[2][0];
	if (sy < 1.0F)
	{
		if (sy > -1.0F)
		{
			*x = -Atan(n[2][1], n[2][2]);
			*y = Asin(sy);
			*z = -Atan(n[1][0], n[0][0]);
		}
		else
		{
			*x = 0.0F;
			*y = -K::tau_over_4;
			*z = Atan(n[0][1], n[1][1]);
		}
	}
	else
	{
		*x = 0.0F;
		*y = K::tau_over_4;
		*z = Atan(n[0][1], n[1][1]);
	}
}

Transform4D& Transform4D::SetEulerAngles(float x, float y, float z)
{
	Vector2D xrot = CosSin(x);
	Vector2D yrot = CosSin(y);
	Vector2D zrot = CosSin(z);

	n[0][0] = yrot.x * zrot.x;
	n[0][1] = xrot.x * zrot.y + xrot.y * yrot.y * zrot.x;
	n[0][2] = xrot.y * zrot.y - xrot.x * yrot.y * zrot.x;
	n[1][0] = -yrot.x * zrot.y;
	n[1][1] = xrot.x * zrot.x - xrot.y * yrot.y * zrot.y;
	n[1][2] = xrot.y * zrot.x + xrot.x * yrot.y * zrot.y;
	n[2][0] = yrot.y;
	n[2][1] = -xrot.y * yrot.x;
	n[2][2] = xrot.x * yrot.x;

	n[3][3] = 1.0F;
	n[0][3] = n[1][3] = n[2][3] = n[3][0] = n[3][1] = n[3][2] = 0.0F;

	return (*this);
}

Transform4D& Transform4D::SetScale(float t)
{
	n[0][0] = n[1][1] = n[2][2] = t;
	n[3][3] = 1.0F;
	n[1][0] = n[2][0] = n[3][0] = n[0][1] = n[2][1] = n[3][1] = n[0][2] = n[1][2] = n[3][2] = n[0][3] = n[1][3] = n[2][3] = 0.0F;
	return (*this);
}

Transform4D& Transform4D::SetScale(float r, float s, float t)
{
	n[0][0] = r;
	n[1][1] = s;
	n[2][2] = t;
	n[3][3] = 1.0F;
	n[1][0] = n[2][0] = n[3][0] = n[0][1] = n[2][1] = n[3][1] = n[0][2] = n[1][2] = n[3][2] = n[0][3] = n[1][3] = n[2][3] = 0.0F;
	return (*this);
}

Transform4D& Transform4D::SetDisplacement(const Vector3D& dv)
{
	n[3][0] = dv.x;
	n[3][1] = dv.y;
	n[3][2] = dv.z;
	n[0][0] = n[1][1] = n[2][2] = n[3][3] = 1.0F;
	n[1][0] = n[2][0] = n[0][1] = n[2][1] = n[0][2] = n[1][2] = n[0][3] = n[1][3] = n[2][3] = 0.0F;
	return (*this);
}

Transform4D& Transform4D::Normalize(void)
{
	(*this)[0].Normalize();
	(*this)[1].Normalize();
	(*this)[2].Normalize();
	return (*this);
}


Transform4D C4::operator *(const Transform4D& m, float t)
{
	return (Transform4D(m.n[0][0] * t, m.n[1][0] * t, m.n[2][0] * t, m.n[3][0] * t,
						m.n[0][1] * t, m.n[1][1] * t, m.n[2][1] * t, m.n[3][1] * t,
						m.n[0][2] * t, m.n[1][2] * t, m.n[2][2] * t, m.n[3][2] * t));
}

Transform4D C4::operator &(const Transform4D& m, const Vector3D& v)
{
	return (Transform4D(m.n[0][0] * v.x, m.n[1][0] * v.x, m.n[2][0] * v.x, m.n[3][0] * v.x,
						m.n[0][1] * v.y, m.n[1][1] * v.y, m.n[2][1] * v.y, m.n[3][1] * v.y,
						m.n[0][2] * v.z, m.n[1][2] * v.z, m.n[2][2] * v.z, m.n[3][2] * v.z));
}

Transform4D C4::operator *(const Transform4D& m1, const Transform4D& m2)
{
	#if C4SIMD

		Transform4D		result;

		vec_float c1 = VecLoad(&m1[0][0]);
		vec_float c2 = VecLoad(&m1[0][0], 4);
		vec_float c3 = VecLoad(&m1[0][0], 8);
		VecStore(VecTransformVector3D(c1, c2, c3, VecLoad(&m2[0][0])), &result[0][0]);
		VecStore(VecTransformVector3D(c1, c2, c3, VecLoad(&m2[0][0], 4)), &result[1][0]);
		VecStore(VecTransformVector3D(c1, c2, c3, VecLoad(&m2[0][0], 8)), &result[2][0]);
		VecStore(VecTransformPoint3D(c1, c2, c3, VecLoad(&m1[0][0], 12), VecLoad(&m2[0][0], 12)), &result[3][0]);

		return (result);

	#else

		return (Transform4D(m1.n[0][0] * m2.n[0][0] + m1.n[1][0] * m2.n[0][1] + m1.n[2][0] * m2.n[0][2],
							m1.n[0][0] * m2.n[1][0] + m1.n[1][0] * m2.n[1][1] + m1.n[2][0] * m2.n[1][2],
							m1.n[0][0] * m2.n[2][0] + m1.n[1][0] * m2.n[2][1] + m1.n[2][0] * m2.n[2][2],
							m1.n[0][0] * m2.n[3][0] + m1.n[1][0] * m2.n[3][1] + m1.n[2][0] * m2.n[3][2] + m1.n[3][0],
							m1.n[0][1] * m2.n[0][0] + m1.n[1][1] * m2.n[0][1] + m1.n[2][1] * m2.n[0][2],
							m1.n[0][1] * m2.n[1][0] + m1.n[1][1] * m2.n[1][1] + m1.n[2][1] * m2.n[1][2],
							m1.n[0][1] * m2.n[2][0] + m1.n[1][1] * m2.n[2][1] + m1.n[2][1] * m2.n[2][2],
							m1.n[0][1] * m2.n[3][0] + m1.n[1][1] * m2.n[3][1] + m1.n[2][1] * m2.n[3][2] + m1.n[3][1],
							m1.n[0][2] * m2.n[0][0] + m1.n[1][2] * m2.n[0][1] + m1.n[2][2] * m2.n[0][2],
							m1.n[0][2] * m2.n[1][0] + m1.n[1][2] * m2.n[1][1] + m1.n[2][2] * m2.n[1][2],
							m1.n[0][2] * m2.n[2][0] + m1.n[1][2] * m2.n[2][1] + m1.n[2][2] * m2.n[2][2],
							m1.n[0][2] * m2.n[3][0] + m1.n[1][2] * m2.n[3][1] + m1.n[2][2] * m2.n[3][2] + m1.n[3][2]));

	#endif
}

Matrix4D C4::operator *(const Matrix4D& m1, const Transform4D& m2)
{
	#if C4SIMD

		Transform4D		result;

		vec_float c1 = VecLoad(&m1[0][0]);
		vec_float c2 = VecLoad(&m1[0][0], 4);
		vec_float c3 = VecLoad(&m1[0][0], 8);
		VecStore(VecTransformVector3D(c1, c2, c3, VecLoad(&m2[0][0])), &result[0][0]);
		VecStore(VecTransformVector3D(c1, c2, c3, VecLoad(&m2[0][0], 4)), &result[1][0]);
		VecStore(VecTransformVector3D(c1, c2, c3, VecLoad(&m2[0][0], 8)), &result[2][0]);
		VecStore(VecTransformPoint3D(c1, c2, c3, VecLoad(&m1[0][0], 12), VecLoad(&m2[0][0], 12)), &result[3][0]);

		return (result);

	#else

		return (Matrix4D(m1.n[0][0] * m2.n[0][0] + m1.n[1][0] * m2.n[0][1] + m1.n[2][0] * m2.n[0][2],
						 m1.n[0][0] * m2.n[1][0] + m1.n[1][0] * m2.n[1][1] + m1.n[2][0] * m2.n[1][2],
						 m1.n[0][0] * m2.n[2][0] + m1.n[1][0] * m2.n[2][1] + m1.n[2][0] * m2.n[2][2],
						 m1.n[0][0] * m2.n[3][0] + m1.n[1][0] * m2.n[3][1] + m1.n[2][0] * m2.n[3][2] + m1.n[3][0],
						 m1.n[0][1] * m2.n[0][0] + m1.n[1][1] * m2.n[0][1] + m1.n[2][1] * m2.n[0][2],
						 m1.n[0][1] * m2.n[1][0] + m1.n[1][1] * m2.n[1][1] + m1.n[2][1] * m2.n[1][2],
						 m1.n[0][1] * m2.n[2][0] + m1.n[1][1] * m2.n[2][1] + m1.n[2][1] * m2.n[2][2],
						 m1.n[0][1] * m2.n[3][0] + m1.n[1][1] * m2.n[3][1] + m1.n[2][1] * m2.n[3][2] + m1.n[3][1],
						 m1.n[0][2] * m2.n[0][0] + m1.n[1][2] * m2.n[0][1] + m1.n[2][2] * m2.n[0][2],
						 m1.n[0][2] * m2.n[1][0] + m1.n[1][2] * m2.n[1][1] + m1.n[2][2] * m2.n[1][2],
						 m1.n[0][2] * m2.n[2][0] + m1.n[1][2] * m2.n[2][1] + m1.n[2][2] * m2.n[2][2],
						 m1.n[0][2] * m2.n[3][0] + m1.n[1][2] * m2.n[3][1] + m1.n[2][2] * m2.n[3][2] + m1.n[3][2],
						 m1.n[0][3] * m2.n[0][0] + m1.n[1][3] * m2.n[0][1] + m1.n[2][3] * m2.n[0][2],
						 m1.n[0][3] * m2.n[1][0] + m1.n[1][3] * m2.n[1][1] + m1.n[2][3] * m2.n[1][2],
						 m1.n[0][3] * m2.n[2][0] + m1.n[1][3] * m2.n[2][1] + m1.n[2][3] * m2.n[2][2],
						 m1.n[0][3] * m2.n[3][0] + m1.n[1][3] * m2.n[3][1] + m1.n[2][3] * m2.n[3][2] + m1.n[3][3]));

	#endif
}

Matrix4D C4::operator *(const Transform4D& m1, const Matrix4D& m2)
{
	return (Matrix4D(m1.n[0][0] * m2.n[0][0] + m1.n[1][0] * m2.n[0][1] + m1.n[2][0] * m2.n[0][2] + m1.n[3][0] * m2.n[0][3],
					 m1.n[0][0] * m2.n[1][0] + m1.n[1][0] * m2.n[1][1] + m1.n[2][0] * m2.n[1][2] + m1.n[3][0] * m2.n[1][3],
					 m1.n[0][0] * m2.n[2][0] + m1.n[1][0] * m2.n[2][1] + m1.n[2][0] * m2.n[2][2] + m1.n[3][0] * m2.n[2][3],
					 m1.n[0][0] * m2.n[3][0] + m1.n[1][0] * m2.n[3][1] + m1.n[2][0] * m2.n[3][2] + m1.n[3][0] * m2.n[3][3],
					 m1.n[0][1] * m2.n[0][0] + m1.n[1][1] * m2.n[0][1] + m1.n[2][1] * m2.n[0][2] + m1.n[3][1] * m2.n[0][3],
					 m1.n[0][1] * m2.n[1][0] + m1.n[1][1] * m2.n[1][1] + m1.n[2][1] * m2.n[1][2] + m1.n[3][1] * m2.n[1][3],
					 m1.n[0][1] * m2.n[2][0] + m1.n[1][1] * m2.n[2][1] + m1.n[2][1] * m2.n[2][2] + m1.n[3][1] * m2.n[2][3],
					 m1.n[0][1] * m2.n[3][0] + m1.n[1][1] * m2.n[3][1] + m1.n[2][1] * m2.n[3][2] + m1.n[3][1] * m2.n[3][3],
					 m1.n[0][2] * m2.n[0][0] + m1.n[1][2] * m2.n[0][1] + m1.n[2][2] * m2.n[0][2] + m1.n[3][2] * m2.n[0][3],
					 m1.n[0][2] * m2.n[1][0] + m1.n[1][2] * m2.n[1][1] + m1.n[2][2] * m2.n[1][2] + m1.n[3][2] * m2.n[1][3],
					 m1.n[0][2] * m2.n[2][0] + m1.n[1][2] * m2.n[2][1] + m1.n[2][2] * m2.n[2][2] + m1.n[3][2] * m2.n[2][3],
					 m1.n[0][2] * m2.n[3][0] + m1.n[1][2] * m2.n[3][1] + m1.n[2][2] * m2.n[3][2] + m1.n[3][2] * m2.n[3][3],
					 m2.n[0][3], m2.n[1][3], m2.n[2][3], m2.n[3][3]));
}

Transform4D C4::operator *(const Transform4D& m1, const Matrix3D& m2)
{
	return (Transform4D(m1.n[0][0] * m2.n[0][0] + m1.n[1][0] * m2.n[0][1] + m1.n[2][0] * m2.n[0][2],
						m1.n[0][0] * m2.n[1][0] + m1.n[1][0] * m2.n[1][1] + m1.n[2][0] * m2.n[1][2],
						m1.n[0][0] * m2.n[2][0] + m1.n[1][0] * m2.n[2][1] + m1.n[2][0] * m2.n[2][2],
						m1.n[3][0],
						m1.n[0][1] * m2.n[0][0] + m1.n[1][1] * m2.n[0][1] + m1.n[2][1] * m2.n[0][2],
						m1.n[0][1] * m2.n[1][0] + m1.n[1][1] * m2.n[1][1] + m1.n[2][1] * m2.n[1][2],
						m1.n[0][1] * m2.n[2][0] + m1.n[1][1] * m2.n[2][1] + m1.n[2][1] * m2.n[2][2],
						m1.n[3][1],
						m1.n[0][2] * m2.n[0][0] + m1.n[1][2] * m2.n[0][1] + m1.n[2][2] * m2.n[0][2],
						m1.n[0][2] * m2.n[1][0] + m1.n[1][2] * m2.n[1][1] + m1.n[2][2] * m2.n[1][2],
						m1.n[0][2] * m2.n[2][0] + m1.n[1][2] * m2.n[2][1] + m1.n[2][2] * m2.n[2][2],
						m1.n[3][2]));
}

Transform4D C4::operator *(const Matrix3D& m1, const Transform4D& m2)
{
	return (Transform4D(m1.n[0][0] * m2.n[0][0] + m1.n[1][0] * m2.n[0][1] + m1.n[2][0] * m2.n[0][2],
						m1.n[0][0] * m2.n[1][0] + m1.n[1][0] * m2.n[1][1] + m1.n[2][0] * m2.n[1][2],
						m1.n[0][0] * m2.n[2][0] + m1.n[1][0] * m2.n[2][1] + m1.n[2][0] * m2.n[2][2],
						m1.n[0][0] * m2.n[3][0] + m1.n[1][0] * m2.n[3][1] + m1.n[2][0] * m2.n[3][2],
						m1.n[0][1] * m2.n[0][0] + m1.n[1][1] * m2.n[0][1] + m1.n[2][1] * m2.n[0][2],
						m1.n[0][1] * m2.n[1][0] + m1.n[1][1] * m2.n[1][1] + m1.n[2][1] * m2.n[1][2],
						m1.n[0][1] * m2.n[2][0] + m1.n[1][1] * m2.n[2][1] + m1.n[2][1] * m2.n[2][2],
						m1.n[0][1] * m2.n[3][0] + m1.n[1][1] * m2.n[3][1] + m1.n[2][1] * m2.n[3][2],
						m1.n[0][2] * m2.n[0][0] + m1.n[1][2] * m2.n[0][1] + m1.n[2][2] * m2.n[0][2],
						m1.n[0][2] * m2.n[1][0] + m1.n[1][2] * m2.n[1][1] + m1.n[2][2] * m2.n[1][2],
						m1.n[0][2] * m2.n[2][0] + m1.n[1][2] * m2.n[2][1] + m1.n[2][2] * m2.n[2][2],
						m1.n[0][2] * m2.n[3][0] + m1.n[1][2] * m2.n[3][1] + m1.n[2][2] * m2.n[3][2]));
}

Vector4D C4::operator *(const Transform4D& m, const Vector4D& v)
{
	return (Vector4D(m.n[0][0] * v.x + m.n[1][0] * v.y + m.n[2][0] * v.z + m.n[3][0] * v.w,
					 m.n[0][1] * v.x + m.n[1][1] * v.y + m.n[2][1] * v.z + m.n[3][1] * v.w,
					 m.n[0][2] * v.x + m.n[1][2] * v.y + m.n[2][2] * v.z + m.n[3][2] * v.w,
					 v.w));
}

Antivector4D C4::operator *(const Antivector4D& v, const Transform4D& m)
{
	return (Antivector4D(m.n[0][0] * v.x + m.n[0][1] * v.y + m.n[0][2] * v.z,
						 m.n[1][0] * v.x + m.n[1][1] * v.y + m.n[1][2] * v.z,
						 m.n[2][0] * v.x + m.n[2][1] * v.y + m.n[2][2] * v.z,
						 m.n[3][0] * v.x + m.n[3][1] * v.y + m.n[3][2] * v.z + v.w));
}

Vector3D C4::operator *(const Transform4D& m, const Vector3D& v)
{
	#if C4SIMD

		Vector3D	result;

		VecStore3D(VecTransformVector3D(VecLoad(&m.n[0][0]), VecLoad(&m.n[0][0], 4), VecLoad(&m.n[0][0], 8), VecLoadUnaligned(&v.x)), &result.x);
		return (result);

	#else

		return (Vector3D(m.n[0][0] * v.x + m.n[1][0] * v.y + m.n[2][0] * v.z,
						 m.n[0][1] * v.x + m.n[1][1] * v.y + m.n[2][1] * v.z,
						 m.n[0][2] * v.x + m.n[1][2] * v.y + m.n[2][2] * v.z));

	#endif
}

Antivector3D C4::operator *(const Antivector3D& v, const Transform4D& m)
{
	return (Antivector3D(m.n[0][0] * v.x + m.n[0][1] * v.y + m.n[0][2] * v.z,
						 m.n[1][0] * v.x + m.n[1][1] * v.y + m.n[1][2] * v.z,
						 m.n[2][0] * v.x + m.n[2][1] * v.y + m.n[2][2] * v.z));
}

Point3D C4::operator *(const Transform4D& m, const Point3D& p)
{
	#if C4SIMD

		Point3D		result;

		VecStore3D(VecTransformPoint3D(VecLoad(&m.n[0][0]), VecLoad(&m.n[0][0], 4), VecLoad(&m.n[0][0], 8), VecLoad(&m.n[0][0], 12), VecLoadUnaligned(&p.x)), &result.x);
		return (result);

	#else

		return (Point3D(m.n[0][0] * p.x + m.n[1][0] * p.y + m.n[2][0] * p.z + m.n[3][0],
						m.n[0][1] * p.x + m.n[1][1] * p.y + m.n[2][1] * p.z + m.n[3][1],
						m.n[0][2] * p.x + m.n[1][2] * p.y + m.n[2][2] * p.z + m.n[3][2]));

	#endif
}

Vector2D C4::operator *(const Transform4D& m, const Vector2D& v)
{
	return (Vector2D(m.n[0][0] * v.x + m.n[1][0] * v.y,
					 m.n[0][1] * v.x + m.n[1][1] * v.y));
}

Point2D C4::operator *(const Transform4D& m, const Point2D& p)
{
	return (Point2D(m.n[0][0] * p.x + m.n[1][0] * p.y + m.n[3][0],
					m.n[0][1] * p.x + m.n[1][1] * p.y + m.n[3][1]));
}

Matrix3D C4::Transform(const Transform4D& m1, const Matrix3D& m2)
{
	return (Matrix3D(m1.n[0][0] * m2.n[0][0] + m1.n[1][0] * m2.n[0][1] + m1.n[2][0] * m2.n[0][2],
					 m1.n[0][0] * m2.n[1][0] + m1.n[1][0] * m2.n[1][1] + m1.n[2][0] * m2.n[1][2],
					 m1.n[0][0] * m2.n[2][0] + m1.n[1][0] * m2.n[2][1] + m1.n[2][0] * m2.n[2][2],
					 m1.n[0][1] * m2.n[0][0] + m1.n[1][1] * m2.n[0][1] + m1.n[2][1] * m2.n[0][2],
					 m1.n[0][1] * m2.n[1][0] + m1.n[1][1] * m2.n[1][1] + m1.n[2][1] * m2.n[1][2],
					 m1.n[0][1] * m2.n[2][0] + m1.n[1][1] * m2.n[2][1] + m1.n[2][1] * m2.n[2][2],
					 m1.n[0][2] * m2.n[0][0] + m1.n[1][2] * m2.n[0][1] + m1.n[2][2] * m2.n[0][2],
					 m1.n[0][2] * m2.n[1][0] + m1.n[1][2] * m2.n[1][1] + m1.n[2][2] * m2.n[1][2],
					 m1.n[0][2] * m2.n[2][0] + m1.n[1][2] * m2.n[2][1] + m1.n[2][2] * m2.n[2][2]));
}

Matrix3D C4::TransformTranspose(const Transform4D& m1, const Matrix3D& m2)
{
	return (Matrix3D(m1.n[0][0] * m2.n[0][0] + m1.n[1][0] * m2.n[1][0] + m1.n[2][0] * m2.n[2][0],
					 m1.n[0][0] * m2.n[0][1] + m1.n[1][0] * m2.n[1][1] + m1.n[2][0] * m2.n[2][1],
					 m1.n[0][0] * m2.n[0][2] + m1.n[1][0] * m2.n[1][2] + m1.n[2][0] * m2.n[2][2],
					 m1.n[0][1] * m2.n[0][0] + m1.n[1][1] * m2.n[1][0] + m1.n[2][1] * m2.n[2][0],
					 m1.n[0][1] * m2.n[0][1] + m1.n[1][1] * m2.n[1][1] + m1.n[2][1] * m2.n[2][1],
					 m1.n[0][1] * m2.n[0][2] + m1.n[1][1] * m2.n[1][2] + m1.n[2][1] * m2.n[2][2],
					 m1.n[0][2] * m2.n[0][0] + m1.n[1][2] * m2.n[1][0] + m1.n[2][2] * m2.n[2][0],
					 m1.n[0][2] * m2.n[0][1] + m1.n[1][2] * m2.n[1][1] + m1.n[2][2] * m2.n[2][1],
					 m1.n[0][2] * m2.n[0][2] + m1.n[1][2] * m2.n[1][2] + m1.n[2][2] * m2.n[2][2]));
}

Transform4D C4::TransposeTransform(const Matrix3D& m1, const Transform4D& m2)
{
	return (Transform4D(m1.n[0][0] * m2.n[0][0] + m1.n[0][1] * m2.n[0][1] + m1.n[0][2] * m2.n[0][2],
						m1.n[0][0] * m2.n[1][0] + m1.n[0][1] * m2.n[1][1] + m1.n[0][2] * m2.n[1][2],
						m1.n[0][0] * m2.n[2][0] + m1.n[0][1] * m2.n[2][1] + m1.n[0][2] * m2.n[2][2],
						m1.n[0][0] * m2.n[3][0] + m1.n[0][1] * m2.n[3][1] + m1.n[0][2] * m2.n[3][2],
						m1.n[1][0] * m2.n[0][0] + m1.n[1][1] * m2.n[0][1] + m1.n[1][2] * m2.n[0][2],
						m1.n[1][0] * m2.n[1][0] + m1.n[1][1] * m2.n[1][1] + m1.n[1][2] * m2.n[1][2],
						m1.n[1][0] * m2.n[2][0] + m1.n[1][1] * m2.n[2][1] + m1.n[1][2] * m2.n[2][2],
						m1.n[1][0] * m2.n[3][0] + m1.n[1][1] * m2.n[3][1] + m1.n[1][2] * m2.n[3][2],
						m1.n[2][0] * m2.n[0][0] + m1.n[2][1] * m2.n[0][1] + m1.n[2][2] * m2.n[0][2],
						m1.n[2][0] * m2.n[1][0] + m1.n[2][1] * m2.n[1][1] + m1.n[2][2] * m2.n[1][2],
						m1.n[2][0] * m2.n[2][0] + m1.n[2][1] * m2.n[2][1] + m1.n[2][2] * m2.n[2][2],
						m1.n[2][0] * m2.n[3][0] + m1.n[2][1] * m2.n[3][1] + m1.n[2][2] * m2.n[3][2]));
}

#if C4SIMD

	vec_float C4::TransformVector3D(const Transform4D& m, vec_float v)
	{
		return (VecTransformVector3D(VecLoad(&m.n[0][0]), VecLoad(&m.n[0][0], 4), VecLoad(&m.n[0][0], 8), v));
	}

	vec_float C4::TransformPoint3D(const Transform4D& m, vec_float p)
	{
		return (VecTransformPoint3D(VecLoad(&m.n[0][0]), VecLoad(&m.n[0][0], 4), VecLoad(&m.n[0][0], 8), VecLoad(&m.n[0][0], 12), p));
	}

#endif


float C4::Determinant(const Transform4D& m)
{
	float n00 = m(0,0);
	float n01 = m(0,1);
	float n02 = m(0,2);

	float n10 = m(1,0);
	float n11 = m(1,1);
	float n12 = m(1,2);

	float n20 = m(2,0);
	float n21 = m(2,1);
	float n22 = m(2,2);

	return (n00 * (n11 * n22 - n12 * n21) + n01 * (n12 * n20 - n10 * n22) + n02 * (n10 * n21 - n11 * n20));
}

Transform4D C4::Inverse(const Transform4D& m)
{
	float n00 = m(0,0);
	float n01 = m(0,1);
	float n02 = m(0,2);
	float n03 = m(0,3);

	float n10 = m(1,0);
	float n11 = m(1,1);
	float n12 = m(1,2);
	float n13 = m(1,3);

	float n20 = m(2,0);
	float n21 = m(2,1);
	float n22 = m(2,2);
	float n23 = m(2,3);

	float p00 = n11 * n22 - n12 * n21;
	float p10 = n12 * n20 - n10 * n22;
	float p20 = n10 * n21 - n11 * n20;

	float t = 1.0F / (n00 * p00 + n01 * p10 + n02 * p20);

	return (Transform4D(p00 * t,
						(n02 * n21 - n01 * n22) * t,
						(n01 * n12 - n02 * n11) * t,
						(n01 * (n13 * n22 - n12 * n23) + n02 * (n11 * n23 - n13 * n21) + n03 * (n12 * n21 - n11 * n22)) * t,
						p10 * t,
						(n00 * n22 - n02 * n20) * t,
						(n02 * n10 - n00 * n12) * t,
						(n00 * (n12 * n23 - n13 * n22) + n02 * (n13 * n20 - n10 * n23) + n03 * (n10 * n22 - n12 * n20)) * t,
						p20 * t,
						(n01 * n20 - n00 * n21) * t,
						(n00 * n11 - n01 * n10) * t,
						(n00 * (n13 * n21 - n11 * n23) + n01 * (n10 * n23 - n13 * n20) + n03 * (n11 * n20 - n10 * n21)) * t));
}

Transform4D C4::Adjugate(const Transform4D& m)
{
	float n00 = m(0,0);
	float n01 = m(0,1);
	float n02 = m(0,2);
	float n03 = m(0,3);

	float n10 = m(1,0);
	float n11 = m(1,1);
	float n12 = m(1,2);
	float n13 = m(1,3);

	float n20 = m(2,0);
	float n21 = m(2,1);
	float n22 = m(2,2);
	float n23 = m(2,3);

	return (Transform4D(n11 * n22 - n12 * n21,
						n02 * n21 - n01 * n22,
						n01 * n12 - n02 * n11,
						n01 * (n13 * n22 - n12 * n23) + n02 * (n11 * n23 - n13 * n21) + n03 * (n12 * n21 - n11 * n22),
						n12 * n20 - n10 * n22,
						n00 * n22 - n02 * n20,
						n02 * n10 - n00 * n12,
						n00 * (n12 * n23 - n13 * n22) + n02 * (n13 * n20 - n10 * n23) + n03 * (n10 * n22 - n12 * n20),
						n10 * n21 - n11 * n20,
						n01 * n20 - n00 * n21,
						n00 * n11 - n01 * n10,
						n00 * (n13 * n21 - n11 * n23) + n01 * (n10 * n23 - n13 * n20) + n03 * (n11 * n20 - n10 * n21)));
}

Matrix3D C4::Adjugate3D(const Transform4D& m)
{
	float n00 = m(0,0);
	float n01 = m(0,1);
	float n02 = m(0,2);

	float n10 = m(1,0);
	float n11 = m(1,1);
	float n12 = m(1,2);

	float n20 = m(2,0);
	float n21 = m(2,1);
	float n22 = m(2,2);

	return (Matrix3D(n11 * n22 - n12 * n21, n02 * n21 - n01 * n22, n01 * n12 - n02 * n11,
					 n12 * n20 - n10 * n22, n00 * n22 - n02 * n20, n02 * n10 - n00 * n12,
					 n10 * n21 - n11 * n20, n01 * n20 - n00 * n21, n00 * n11 - n01 * n10));
}

Vector3D C4::InverseTransform(const Transform4D& m, const Vector3D& v)
{
	float n00 = m(0,0);
	float n01 = m(0,1);
	float n02 = m(0,2);

	float n10 = m(1,0);
	float n11 = m(1,1);
	float n12 = m(1,2);

	float n20 = m(2,0);
	float n21 = m(2,1);
	float n22 = m(2,2);

	float p00 = n11 * n22 - n12 * n21;
	float p10 = n12 * n20 - n10 * n22;
	float p20 = n10 * n21 - n11 * n20;

	float t = 1.0F / (n00 * p00 + n01 * p10 + n02 * p20);

	return (Vector3D((p00 * v.x + (n02 * n21 - n01 * n22) * v.y + (n01 * n12 - n02 * n11) * v.z) * t,
					 (p10 * v.x + (n00 * n22 - n02 * n20) * v.y + (n02 * n10 - n00 * n12) * v.z) * t,
					 (p20 * v.x + (n01 * n20 - n00 * n21) * v.y + (n00 * n11 - n01 * n10) * v.z) * t));
}

Point3D C4::InverseTransform(const Transform4D& m, const Point3D& p)
{
	float n00 = m(0,0);
	float n01 = m(0,1);
	float n02 = m(0,2);
	float n03 = m(0,3);

	float n10 = m(1,0);
	float n11 = m(1,1);
	float n12 = m(1,2);
	float n13 = m(1,3);

	float n20 = m(2,0);
	float n21 = m(2,1);
	float n22 = m(2,2);
	float n23 = m(2,3);

	float p00 = n11 * n22 - n12 * n21;
	float p10 = n12 * n20 - n10 * n22;
	float p20 = n10 * n21 - n11 * n20;

	float t = 1.0F / (n00 * p00 + n01 * p10 + n02 * p20);

	return (Point3D((p00 * p.x + (n02 * n21 - n01 * n22) * p.y + (n01 * n12 - n02 * n11) * p.z + (n01 * (n13 * n22 - n12 * n23) + n02 * (n11 * n23 - n13 * n21) + n03 * (n12 * n21 - n11 * n22))) * t,
					(p10 * p.x + (n00 * n22 - n02 * n20) * p.y + (n02 * n10 - n00 * n12) * p.z + (n00 * (n12 * n23 - n13 * n22) + n02 * (n13 * n20 - n10 * n23) + n03 * (n10 * n22 - n12 * n20))) * t,
					(p20 * p.x + (n01 * n20 - n00 * n21) * p.y + (n00 * n11 - n01 * n10) * p.z + (n00 * (n13 * n21 - n11 * n23) + n01 * (n10 * n23 - n13 * n20) + n03 * (n11 * n20 - n10 * n21))) * t));
}

Vector3D C4::AdjugateTransform(const Transform4D& m, const Vector3D& v)
{
	float n00 = m(0,0);
	float n01 = m(0,1);
	float n02 = m(0,2);

	float n10 = m(1,0);
	float n11 = m(1,1);
	float n12 = m(1,2);

	float n20 = m(2,0);
	float n21 = m(2,1);
	float n22 = m(2,2);

	return (Vector3D((n11 * n22 - n12 * n21) * v.x + (n02 * n21 - n01 * n22) * v.y + (n01 * n12 - n02 * n11) * v.z,
					 (n12 * n20 - n10 * n22) * v.x + (n00 * n22 - n02 * n20) * v.y + (n02 * n10 - n00 * n12) * v.z,
					 (n10 * n21 - n11 * n20) * v.x + (n01 * n20 - n00 * n21) * v.y + (n00 * n11 - n01 * n10) * v.z));
}

Point3D C4::AdjugateTransform(const Transform4D& m, const Point3D& p)
{
	float n00 = m(0,0);
	float n01 = m(0,1);
	float n02 = m(0,2);
	float n03 = m(0,3);

	float n10 = m(1,0);
	float n11 = m(1,1);
	float n12 = m(1,2);
	float n13 = m(1,3);

	float n20 = m(2,0);
	float n21 = m(2,1);
	float n22 = m(2,2);
	float n23 = m(2,3);

	return (Point3D((n11 * n22 - n12 * n21) * p.x + (n02 * n21 - n01 * n22) * p.y + (n01 * n12 - n02 * n11) * p.z + (n01 * (n13 * n22 - n12 * n23) + n02 * (n11 * n23 - n13 * n21) + n03 * (n12 * n21 - n11 * n22)),
					(n12 * n20 - n10 * n22) * p.x + (n00 * n22 - n02 * n20) * p.y + (n02 * n10 - n00 * n12) * p.z + (n00 * (n12 * n23 - n13 * n22) + n02 * (n13 * n20 - n10 * n23) + n03 * (n10 * n22 - n12 * n20)),
					(n10 * n21 - n11 * n20) * p.x + (n01 * n20 - n00 * n21) * p.y + (n00 * n11 - n01 * n10) * p.z + (n00 * (n13 * n21 - n11 * n23) + n01 * (n10 * n23 - n13 * n20) + n03 * (n11 * n20 - n10 * n21))));
}

// ZYUQURM
