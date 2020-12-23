 

#include "C4Matrix3D.h"
#include "C4Constants.h"


using namespace C4;


const ConstMatrix3D C4::Identity3D = {{{1.0F, 0.0F, 0.0F}, {0.0F, 1.0F, 0.0F}, {0.0F, 0.0F, 1.0F}}};


Matrix3D::Matrix3D(const Vector3D& c1, const Vector3D& c2, const Vector3D& c3)
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
}

Matrix3D::Matrix3D(float n00, float n01, float n02, float n10, float n11, float n12, float n20, float n21, float n22)
{
	n[0][0] = n00;
	n[0][1] = n10;
	n[0][2] = n20;
	n[1][0] = n01;
	n[1][1] = n11;
	n[1][2] = n21;
	n[2][0] = n02;
	n[2][1] = n12;
	n[2][2] = n22;
}

Matrix3D& Matrix3D::Set(const Vector3D& c1, const Vector3D& c2, const Vector3D& c3)
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

Matrix3D& Matrix3D::Set(float n00, float n01, float n02, float n10, float n11, float n12, float n20, float n21, float n22)
{
	n[0][0] = n00;
	n[0][1] = n10;
	n[0][2] = n20;
	n[1][0] = n01;
	n[1][1] = n11;
	n[1][2] = n21;
	n[2][0] = n02;
	n[2][1] = n12;
	n[2][2] = n22;

	return (*this);
}

Matrix3D& Matrix3D::operator *=(const Matrix3D& m)
{
	float t = n[0][0] * m.n[0][0] + n[1][0] * m.n[0][1] + n[2][0] * m.n[0][2];
	float u = n[0][0] * m.n[1][0] + n[1][0] * m.n[1][1] + n[2][0] * m.n[1][2];
	n[2][0] = n[0][0] * m.n[2][0] + n[1][0] * m.n[2][1] + n[2][0] * m.n[2][2];
	n[0][0] = t;
	n[1][0] = u;

	t = n[0][1] * m.n[0][0] + n[1][1] * m.n[0][1] + n[2][1] * m.n[0][2];
	u = n[0][1] * m.n[1][0] + n[1][1] * m.n[1][1] + n[2][1] * m.n[1][2];
	n[2][1] = n[0][1] * m.n[2][0] + n[1][1] * m.n[2][1] + n[2][1] * m.n[2][2];
	n[0][1] = t;
	n[1][1] = u;

	t = n[0][2] * m.n[0][0] + n[1][2] * m.n[0][1] + n[2][2] * m.n[0][2];
	u = n[0][2] * m.n[1][0] + n[1][2] * m.n[1][1] + n[2][2] * m.n[1][2];
	n[2][2] = n[0][2] * m.n[2][0] + n[1][2] * m.n[2][1] + n[2][2] * m.n[2][2];
	n[0][2] = t;
	n[1][2] = u;

	return (*this);
}

Matrix3D& Matrix3D::operator *=(float t)
{
	n[0][0] *= t;
	n[0][1] *= t;
	n[0][2] *= t;
	n[1][0] *= t;
	n[1][1] *= t;
	n[1][2] *= t;
	n[2][0] *= t;
	n[2][1] *= t;
	n[2][2] *= t;
 
	return (*this);
}
 
Matrix3D& Matrix3D::operator /=(float t)
{ 
	float f = 1.0F / t;
	n[0][0] *= f;
	n[0][1] *= f; 
	n[0][2] *= f;
	n[1][0] *= f; 
	n[1][1] *= f; 
	n[1][2] *= f;
	n[2][0] *= f;
	n[2][1] *= f;
	n[2][2] *= f; 

	return (*this);
}

Matrix3D& Matrix3D::SetIdentity(void)
{
	n[0][0] = n[1][1] = n[2][2] = 1.0F;
	n[0][1] = n[0][2] = n[1][0] = n[1][2] = n[2][0] = n[2][1] = 0.0F;

	return (*this);
}

Matrix3D& Matrix3D::SetRotationAboutX(float angle)
{
	CosSin(angle, &n[1][1], &n[1][2]);

	n[2][2] = n[1][1];
	n[2][1] = -n[1][2];
	n[0][0] = 1.0F;
	n[0][1] = n[0][2] = n[1][0] = n[2][0] = 0.0F;

	return (*this);
}

Matrix3D& Matrix3D::SetRotationAboutY(float angle)
{
	CosSin(angle, &n[0][0], &n[2][0]);

	n[2][2] = n[0][0];
	n[0][2] = -n[2][0];
	n[1][1] = 1.0F;
	n[1][0] = n[1][2] = n[0][1] = n[2][1] = 0.0F;

	return (*this);
}

Matrix3D& Matrix3D::SetRotationAboutZ(float angle)
{
	CosSin(angle, &n[0][0], &n[0][1]);

	n[1][1] = n[0][0];
	n[1][0] = -n[0][1];
	n[2][2] = 1.0F;
	n[2][0] = n[2][1] = n[0][2] = n[1][2] = 0.0F;

	return (*this);
}

Matrix3D& Matrix3D::SetRotationAboutAxis(float angle, const Antivector3D& axis)
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

	return (*this);
}

void Matrix3D::GetEulerAngles(float *x, float *y, float *z) const
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

Matrix3D& Matrix3D::SetEulerAngles(float x, float y, float z)
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

	return (*this);
}

Matrix3D& Matrix3D::SetScale(float t)
{
	n[0][0] = n[1][1] = n[2][2] = t;
	n[0][1] = n[0][2] = n[1][0] = n[1][2] = n[2][0] = n[2][1] = 0.0F;

	return (*this);
}

Matrix3D& Matrix3D::SetScale(float r, float s, float t)
{
	n[0][0] = r;
	n[1][1] = s;
	n[2][2] = t;
	n[0][1] = n[0][2] = n[1][0] = n[1][2] = n[2][0] = n[2][1] = 0.0F;

	return (*this);
}

Matrix3D& Matrix3D::Orthonormalize(int32 column)
{
	Vector3D& x = (*this)[column];
	Vector3D& y = (*this)[IncMod<3>(column)];
	Vector3D& z = (*this)[DecMod<3>(column)];

	x.Normalize();
	y = Normalize(y - x * (x * y));
	z = Normalize(z - x * (x * z) - y * (y * z));

	return (*this);
}


Matrix3D C4::operator *(const Matrix3D& m1, const Matrix3D& m2)
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

Matrix3D C4::operator *(const Matrix3D& m, float t)
{
	return (Matrix3D(m.n[0][0] * t, m.n[1][0] * t, m.n[2][0] * t, m.n[0][1] * t, m.n[1][1] * t, m.n[2][1] * t, m.n[0][2] * t, m.n[1][2] * t, m.n[2][2] * t));
}

Matrix3D C4::operator /(const Matrix3D& m, float t)
{
	float f = 1.0F / t;
	return (Matrix3D(m.n[0][0] * f, m.n[1][0] * f, m.n[2][0] * f, m.n[0][1] * f, m.n[1][1] * f, m.n[2][1] * f, m.n[0][2] * f, m.n[1][2] * f, m.n[2][2] * f));
}

Vector3D C4::operator *(const Matrix3D& m, const Vector3D& v)
{
	return (Vector3D(m.n[0][0] * v.x + m.n[1][0] * v.y + m.n[2][0] * v.z, m.n[0][1] * v.x + m.n[1][1] * v.y + m.n[2][1] * v.z, m.n[0][2] * v.x + m.n[1][2] * v.y + m.n[2][2] * v.z));
}

Vector3D C4::operator *(const Matrix3D& m, const Point3D& p)
{
	return (Point3D(m.n[0][0] * p.x + m.n[1][0] * p.y + m.n[2][0] * p.z, m.n[0][1] * p.x + m.n[1][1] * p.y + m.n[2][1] * p.z, m.n[0][2] * p.x + m.n[1][2] * p.y + m.n[2][2] * p.z));
}

Vector3D C4::operator *(const Vector3D& v, const Matrix3D& m)
{
	return (Vector3D(m.n[0][0] * v.x + m.n[0][1] * v.y + m.n[0][2] * v.z, m.n[1][0] * v.x + m.n[1][1] * v.y + m.n[1][2] * v.z, m.n[2][0] * v.x + m.n[2][1] * v.y + m.n[2][2] * v.z));
}

Vector3D C4::operator *(const Point3D& p, const Matrix3D& m)
{
	return (Point3D(m.n[0][0] * p.x + m.n[0][1] * p.y + m.n[0][2] * p.z, m.n[1][0] * p.x + m.n[1][1] * p.y + m.n[1][2] * p.z, m.n[2][0] * p.x + m.n[2][1] * p.y + m.n[2][2] * p.z));
}

bool C4::operator ==(const Matrix3D& m1, const Matrix3D& m2)
{
	return ((m1.n[0][0] == m2.n[0][0]) && (m1.n[0][1] == m2.n[0][1]) && (m1.n[0][2] == m2.n[0][2]) && (m1.n[1][0] == m2.n[1][0]) && (m1.n[1][1] == m2.n[1][1]) && (m1.n[1][2] == m2.n[1][2]) && (m1.n[2][0] == m2.n[2][0]) && (m1.n[2][1] == m2.n[2][1]) && (m1.n[2][2] == m2.n[2][2]));
}

bool C4::operator !=(const Matrix3D& m1, const Matrix3D& m2)
{
	return ((m1.n[0][0] != m2.n[0][0]) || (m1.n[0][1] != m2.n[0][1]) || (m1.n[0][2] != m2.n[0][2]) || (m1.n[1][0] != m2.n[1][0]) || (m1.n[1][1] != m2.n[1][1]) || (m1.n[1][2] != m2.n[1][2]) || (m1.n[2][0] != m2.n[2][0]) || (m1.n[2][1] != m2.n[2][1]) || (m1.n[2][2] != m2.n[2][2]));
}

float C4::Determinant(const Matrix3D& m)
{
	return (m(0,0) * (m(1,1) * m(2,2) - m(1,2) * m(2,1)) - m(0,1) * (m(1,0) * m(2,2) - m(1,2) * m(2,0)) + m(0,2) * (m(1,0) * m(2,1) - m(1,1) * m(2,0)));
}

Matrix3D C4::Inverse(const Matrix3D& m)
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

	return (Matrix3D(p00 * t, (n02 * n21 - n01 * n22) * t, (n01 * n12 - n02 * n11) * t,
					 p10 * t, (n00 * n22 - n02 * n20) * t, (n02 * n10 - n00 * n12) * t,
					 p20 * t, (n01 * n20 - n00 * n21) * t, (n00 * n11 - n01 * n10) * t));
}

Matrix3D C4::Adjugate(const Matrix3D& m)
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

Matrix3D C4::Transpose(const Matrix3D& m)
{
	return (Matrix3D(m(0,0), m(1,0), m(2,0), m(0,1), m(1,1), m(2,1), m(0,2), m(1,2), m(2,2)));
}

// ZYUQURM
