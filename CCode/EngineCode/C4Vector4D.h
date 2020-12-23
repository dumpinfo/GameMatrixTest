 

#ifndef C4Vector4D_h
#define C4Vector4D_h


//# \component	Math Library
//# \prefix		Math/


#include "C4Vector3D.h"


namespace C4
{
	//# \class	Vector4D	Encapsulates a 4D vector.
	//
	//# The $Vector4D$ class encapsulates a 4D vector.
	//
	//# \def	class Vector4D
	//
	//# \data	Vector4D
	//
	//# \ctor	Vector4D();
	//# \ctor	Vector4D(float r, float s, float t, float u);
	//# \ctor	Vector4D(const Vector3D& v, float u);
	//# \ctor	Vector4D(const Vector3D& v);
	//# \ctor	Vector4D(const Point3D& p);
	//# \ctor	Vector4D(const Vector2D& v);
	//# \ctor	Vector4D(const Point2D& p);
	//
	//# \param	r	The value of the <i>x</i> coordinate.
	//# \param	s	The value of the <i>y</i> coordinate.
	//# \param	t	The value of the <i>z</i> coordinate.
	//# \param	u	The value of the <i>w</i> coordinate.
	//# \param	v	A 3D vector whose entries are copied to the <i>x</i>, <i>y</i>, and <i>z</i> coordinates.
	//# \param	p	A 3D point whose entries are copied to the <i>x</i>, <i>y</i>, and <i>z</i> coordinates.
	//
	//# \desc
	//# The $Vector4D$ class is used to store a four-dimensional vector
	//# having floating-point components <i>x</i>, <i>y</i>, <i>z</i>, and <i>w</i>.
	//#
	//# The default constructor leaves the components of the vector undefined.
	//# If the values $r$, $s$, $t$, and $u$ are supplied, then they are assigned to the
	//# <i>x</i>, <i>y</i>, <i>z</i>, and <i>w</i> coordinates of the vector, respectively.
	//# If the $Vector4D$ object is constructed using a $@Vector3D@$ or $@Vector2D@$ object,
	//# then the <i>w</i> coordinate is set to 0. If the $Vector4D$ object is constructed
	//# using a $@Point3D@$ or $@Point2D@$ object, then the <i>w</i> coordinate is set to 1.
	//# The <i>z</i> coordinate of a 2D vector or point is assumed to be 0.
	//
	//# \operator	float& operator [](machine k);
	//#				Returns a reference to the $k$-th component of a vector.
	//#				The value of $k$ must be 0, 1, 2, or 3.
	//
	//# \operator	const float& operator [](machine k) const;
	//#				Returns a constant reference to the $k$-th component of a vector.
	//#				The value of $k$ must be 0, 1, 2, or 3.
	//
	//# \operator	Vector4D& operator =(const Vector3D& v);
	//#				Copies the <i>x</i>, <i>y</i>, and <i>z</i> coordinates of $v$, and assigns
	//#				a value of 0 to the <i>w</i> coordinate.
	//
	//# \operator	Vector4D& operator =(const Point3D& p);
	//#				Copies the <i>x</i>, <i>y</i>, and <i>z</i> coordinates of $p$, and assigns
	//#				a value of 1 to the <i>w</i> coordinate.
	//
	//# \operator	Vector4D& operator =(const Vector2D& v);
	//#				Copies the <i>x</i> and <i>y</i> coordinates of $v$, assigns a value of 0 to
	//#				the <i>z</i> coordinate, and assigns a value of 0 to the <i>w</i> coordinate.
	//
	//# \operator	Vector4D& operator =(const Point2D& p);
	//#				Copies the <i>x</i> and <i>y</i> coordinates of $p$, assigns a value of 0 to
	//#				the <i>z</i> coordinate, and assigns a value of 1 to the <i>w</i> coordinate.
	//
	//# \operator	Vector4D& operator +=(const Vector4D& v);
	//#				Adds the vector $v$.
	//
	//# \operator	Vector4D& operator +=(const Vector3D& v);
	//#				Adds the vector $v$. The <i>w</i> coordinate is not modified.
	//
	//# \operator	Vector4D& operator +=(const Vector2D& v);
	//#				Adds the vector $v$. The <i>z</i> and <i>w</i> coordinates are not modified.
	//
	//# \operator	Vector4D& operator -=(const Vector4D& v);
	//#				Subtracts the vector $v$.
	//
	//# \operator	Vector4D& operator -=(const Vector3D& v);
	//#				Subtracts the vector $v$. The <i>w</i> coordinate is not modified.
	//
	//# \operator	Vector4D& operator -=(const Vector2D& v);
	//#				Subtracts the vector $v$. The <i>z</i> and <i>w</i> coordinates are not modified.
	//
	//# \operator	Vector4D& operator *=(float t);
	//#				Multiplies by the scalar $t$.
	//
	//# \operator	Vector4D& operator /=(float t);
	//#				Multiplies by the inverse of the scalar $t$.
	//
	//# \operator	Vector4D& operator &=(const Vector4D& v);
	//#				Calculates the componentwise product with the vector $v$.
	//
	//# \action		Vector4D operator -(const Vector4D& v);
	//#				Returns the negation of the vector $v$. 
	//
	//# \action		Vector4D operator +(const Vector4D& v1, const Vector4D& v2);
	//#				Returns the sum of the vectors $v1$ and $v2$. 
	//
	//# \action		Vector4D operator +(const Vector4D& v1, const Vector3D& v2); 
	//#				Returns the sum of the vectors $v1$ and $v2$. The <i>w</i> coordinate of $v2$ is assumed to be 0.
	//
	//# \action		Vector4D operator +(const Vector3D& v1, const Vector4D& v2); 
	//#				Returns the sum of the vectors $v1$ and $v2$. The <i>w</i> coordinate of $v1$ is assumed to be 0.
	// 
	//# \action		Vector4D operator +(const Vector4D& v1, const Vector2D& v2); 
	//#				Returns the sum of the vectors $v1$ and $v2$. The <i>z</i> and <i>w</i> coordinates of $v2$ are assumed to be 0.
	//
	//# \action		Vector4D operator +(const Vector2D& v1, const Vector4D& v2);
	//#				Returns the sum of the vectors $v1$ and $v2$. The <i>z</i> and <i>w</i> coordinates of $v1$ are assumed to be 0. 
	//
	//# \action		Vector4D operator -(const Vector4D& v1, const Vector4D& v2);
	//#				Returns the difference of the vectors $v1$ and $v2$.
	//
	//# \action		Vector4D operator -(const Vector4D& v1, const Vector3D& v2);
	//#				Returns the difference of the vectors $v1$ and $v2$. The <i>w</i> coordinate of $v2$ is assumed to be 0.
	//
	//# \action		Vector4D operator -(const Vector3D& v1, const Vector4D& v2);
	//#				Returns the difference of the vectors $v1$ and $v2$. The <i>w</i> coordinate of $v1$ is assumed to be 0.
	//
	//# \action		Vector4D operator -(const Vector4D& v1, const Vector2D& v2);
	//#				Returns the difference of the vectors $v1$ and $v2$. The <i>z</i> and <i>w</i> coordinates of $v2$ are assumed to be 0.
	//
	//# \action		Vector4D operator -(const Vector2D& v1, const Vector4D& v2);
	//#				Returns the difference of the vectors $v1$ and $v2$. The <i>z</i> and <i>w</i> coordinates of $v1$ are assumed to be 0.
	//
	//# \action		Vector4D operator *(const Vector4D& v, float t);
	//#				Returns the product of the vector $v$ and the scalar $t$.
	//
	//# \action		Vector4D operator *(float t, const Vector4D& v);
	//#				Returns the product of the vector $v$ and the scalar $t$.
	//
	//# \action		Vector4D operator /(const Vector4D& v, float t);
	//#				Returns the product of the vector $v$ and the inverse of the scalar $t$.
	//
	//# \action		float operator *(const Vector4D& v1, const Vector4D& v2);
	//#				Returns the four-dimensional dot product of the vectors $v1$ and $v2$.
	//
	//# \action		float operator *(const Vector4D& v1, const Vector3D& v2);
	//#				Returns the dot product of the vectors $v1$ and $v2$. The <i>w</i> coordinate of $v2$ is assumed to be 0.
	//
	//# \action		float operator *(const Vector3D& v1, const Vector4D& v2);
	//#				Returns the dot product of the vectors $v1$ and $v2$. The <i>w</i> coordinate of $v1$ is assumed to be 0.
	//
	//# \action		float operator *(const Vector4D& v, const Point3D& p);
	//#				Returns the dot product of the vector $v$ and the point $p$. The <i>w</i> coordinate of $p$ is assumed to be 1.
	//
	//# \action		float operator *(const Point3D& p, const Vector4D& v);
	//#				Returns the dot product of the point $p$ and the vector $v$. The <i>w</i> coordinate of $p$ is assumed to be 1.
	//
	//# \action		float operator *(const Vector4D& v1, const Vector2D& v2);
	//#				Returns the dot product with the vector $v$. The <i>z</i> and <i>w</i> coordinates of $v$ are assumed to be 0.
	//
	//# \action		float operator *(const Vector2D& v1, const Vector4D& v2);
	//#				Returns the dot product of the vectors $v1$ and $v2$. The <i>z</i> and <i>w</i> coordinates of $v1$ is assumed to be 0.
	//
	//# \action		float operator *(const Vector4D& v, const Point2D& p);
	//#				Returns the dot product of the vector $v$ and the point $p$. The <i>z</i> coordinate of $p$ is assumed to be 0, and the <i>w</i> coordinate of $p$ is assumed to be 1.
	//
	//# \action		float operator *(const Point2D& p, const Vector4D& v);
	//#				Returns the dot product of the point $p$ and the vector $v$. The <i>z</i> coordinate of $p$ is assumed to be 0, and the <i>w</i> coordinate of $p$ is assumed to be 1.
	//
	//# \action		Vector4D operator &(const Vector4D& v1, const Vector4D& v2);
	//#				Returns the componentwise product of the vectors $v1$ and $v2$.
	//
	//# \action		bool operator ==(const Vector4D& v1, const Vector4D& v2);
	//#				Returns a boolean value indicating the equality of the two vectors $v1$ and $v2$.
	//
	//# \action		bool operator !=(const Vector4D& v1, const Vector4D& v2);
	//#				Returns a boolean value indicating the inequality of the two vectors $v1$ and $v2$
	//
	//# \action		float Dot(const Vector4D& v1, const Vector4D& v2);
	//#				Returns the dot product between $v1$ and $v2$.
	//
	//# \action		Vector4D ProjectOnto(const Vector4D& v1, const Vector4D& v2);
	//#				Returns the projection of $v1$ onto $v2$ scaled by the squared magnitude of $v2$.
	//
	//# \action		float Magnitude(const Vector4D& v);
	//#				Returns the magnitude of the vector $v$.
	//
	//# \action		float InverseMag(const Vector4D& v);
	//#				Returns the inverse magnitude of the vector $v$.
	//
	//# \action		float SquaredMag(const Vector4D& v);
	//#				Returns the squared magnitude of the vector $v$.
	//
	//# \also	$@Antivector4D@$
	//# \also	$@Vector3D@$
	//# \also	$@Point3D@$
	//# \also	$@Vector2D@$
	//# \also	$@Point2D@$


	//# \function	Vector4D::Set		Sets all four components of a vector.
	//
	//# \proto	Vector4D& Set(float r, float s, float t, float u);
	//# \proto	Vector4D& Set(const Vector3D& v, float u);
	//
	//# \param	r	The new <i>x</i> coordinate.
	//# \param	s	The new <i>y</i> coordinate.
	//# \param	t	The new <i>z</i> coordinate.
	//# \param	u	The new <i>w</i> coordinate.
	//# \param	v	A 3D vector whose entries are copied to the <i>x</i>, <i>y</i>, and <i>z</i> coordinates.
	//
	//# \desc
	//# The $Set$ function sets the <i>x</i>, <i>y</i>, <i>z</i>, and <i>w</i> coordinates of a vector to
	//# the values given by the $r$, $s$, $t$, and $u$ parameters, respectively.
	//#
	//# The return value is a reference to the vector object.


	//# \function	Vector4D::Normalize		Normalizes a 4D vector.
	//
	//# \proto	Vector4D& Normalize(void);
	//
	//# \desc
	//# The $Normalize$ function multiplies a 4D vector by the inverse of its magnitude,
	//# normalizing it to unit length. Normalizing the zero vector produces undefined results.
	//#
	//# The return value is a reference to the vector object.


	//# \function	Vector4D::Standardize	Standardizes a 4D vector.
	//
	//# \proto	Vector4D& Standardize(void);
	//
	//# \desc
	//# The $Standardize$ function multiplies a 4D vector by the inverse of its <i>w</i> coordinate,
	//# transforming it into a homogeneous point having a unit weight. If the <i>w</i> coordinate is
	//# zero, then the resulting <i>x</i>, <i>y</i>, and <i>z</i> coordinates are undefined.
	//# In all cases, the <i>w</i> coordinate is 1.0 when this function returns.
	//#
	//# The return value is a reference to the vector object.


	//# \function	Vector4D::GetVector3D		Returns a reference to a $@Vector3D@$ object.
	//
	//# \proto	Vector3D& GetVector3D(void);
	//# \proto	const Vector3D& GetVector3D(void) const;
	//
	//# \desc
	//# The $GetVector3D$ function returns a reference to a $@Vector3D@$ object that refers to
	//# the same data contained in the <i>x</i>, <i>y</i>, and <i>z</i> coordinates of a $Vector4D$ object.


	//# \function	Vector4D::GetPoint3D		Returns a reference to a $@Point3D@$ object.
	//
	//# \proto	Point3D& GetPoint3D(void);
	//# \proto	const Point3D& GetPoint3D(void) const;
	//
	//# \desc
	//# The $GetPoint3D$ function returns a reference to a $@Point3D@$ object that refers to
	//# the same data contained in the <i>x</i>, <i>y</i>, and <i>z</i> coordinates of a $Vector4D$ object.


	//# \function	Vector4D::GetVector2D		Returns a reference to a $@Vector2D@$ object.
	//
	//# \proto	Vector2D& GetVector2D(void);
	//# \proto	const Vector2D& GetVector2D(void) const;
	//
	//# \desc
	//# The $GetVector2D$ function returns a reference to a $@Vector2D@$ object that refers to
	//# the same data contained in the <i>x</i> and <i>y</i> coordinates of a $Vector4D$ object.


	//# \function	Vector4D::RotateAboutX		Rotates a vector about the <i>x</i> axis.
	//
	//# \proto	Vector4D& RotateAboutX(float angle);
	//
	//# \param	angle	The angle through which the vector is rotated, in radians.
	//
	//# \desc
	//# The $RotateAboutX$ function rotates a vector about the <i>x</i> axis through the
	//# angle given by the $angle$ parameter. The <i>w</i> coordinate is not modified.
	//#
	//# The return value is a reference to the vector object.
	//
	//# \also	$@Vector4D::RotateAboutY@$
	//# \also	$@Vector4D::RotateAboutZ@$
	//# \also	$@Vector4D::RotateAboutAxis@$


	//# \function	Vector4D::RotateAboutY		Rotates a vector about the <i>y</i> axis.
	//
	//# \proto	Vector4D& RotateAboutY(float angle);
	//
	//# \param	angle	The angle through which the vector is rotated, in radians.
	//
	//# \desc
	//# The $RotateAboutY$ function rotates a vector about the <i>y</i> axis through the
	//# angle given by the $angle$ parameter. The <i>w</i> coordinate is not modified.
	//#
	//# The return value is a reference to the vector object.
	//
	//# \also	$@Vector4D::RotateAboutX@$
	//# \also	$@Vector4D::RotateAboutZ@$
	//# \also	$@Vector4D::RotateAboutAxis@$


	//# \function	Vector4D::RotateAboutZ		Rotates a vector about the <i>z</i> axis.
	//
	//# \proto	Vector4D& RotateAboutZ(float angle);
	//
	//# \param	angle	The angle through which the vector is rotated, in radians.
	//
	//# \desc
	//# The $RotateAboutZ$ function rotates a vector about the <i>z</i> axis through the
	//# angle given by the $angle$ parameter. The <i>w</i> coordinate is not modified.
	//#
	//# The return value is a reference to the vector object.
	//
	//# \also	$@Vector4D::RotateAboutX@$
	//# \also	$@Vector4D::RotateAboutY@$
	//# \also	$@Vector4D::RotateAboutAxis@$


	//# \function	Vector4D::RotateAboutAxis		Rotates a vector about a given axis.
	//
	//# \proto	Vector4D& RotateAboutAxis(float angle, const Vector3D& axis);
	//
	//# \param	angle	The angle through which the vector is rotated, in radians.
	//# \param	axis	The axis about which the vector is rotated.
	//
	//# \desc
	//# The $RotateAboutAxis$ function rotates a vector through the angle given by the
	//# $angle$ parameter about the axis given by the $axis$ parameter. The <i>w</i> coordinate is not modified.
	//#
	//# The return value is a reference to the vector object.
	//
	//# \also	$@Vector4D::RotateAboutX@$
	//# \also	$@Vector4D::RotateAboutY@$
	//# \also	$@Vector4D::RotateAboutZ@$


	//# \member		Vector4D

	class Vector4D
	{
		public:

			float	x;		//## The <i>x</i> coordinate.
			float	y;		//## The <i>y</i> coordinate.
			float	z;		//## The <i>z</i> coordinate.
			float	w;		//## The <i>w</i> coordinate.

			Vector4D() = default;

			Vector4D(float r, float s, float t, float u)
			{
				x = r;
				y = s;
				z = t;
				w = u;
			}

			Vector4D(const Vector3D& v, float u)
			{
				x = v.x;
				y = v.y;
				z = v.z;
				w = u;
			}

			Vector4D(const Vector3D& v)
			{
				x = v.x;
				y = v.y;
				z = v.z;
				w = 0.0F;
			}

			Vector4D(const Point3D& p)
			{
				x = p.x;
				y = p.y;
				z = p.z;
				w = 1.0F;
			}

			Vector4D(const Vector2D& v)
			{
				x = v.x;
				y = v.y;
				z = 0.0F;
				w = 0.0F;
			}

			Vector4D(const Point2D& p)
			{
				x = p.x;
				y = p.y;
				z = 0.0F;
				w = 1.0F;
			}

			Vector4D& Set(float r, float s, float t, float u)
			{
				x = r;
				y = s;
				z = t;
				w = u;
				return (*this);
			}

			void Set(float r, float s, float t, float u) volatile
			{
				x = r;
				y = s;
				z = t;
				w = u;
			}

			Vector4D& Set(const Vector3D& v, float u)
			{
				x = v.x;
				y = v.y;
				z = v.z;
				w = u;
				return (*this);
			}

			void Set(const Vector3D& v, float u) volatile
			{
				x = v.x;
				y = v.y;
				z = v.z;
				w = u;
			}

			float& operator [](machine k)
			{
				return ((&x)[k]);
			}

			const float& operator [](machine k) const
			{
				return ((&x)[k]);
			}

			Vector3D& GetVector3D(void)
			{
				return (*reinterpret_cast<Vector3D *>(this));
			}

			const Vector3D& GetVector3D(void) const
			{
				return (*reinterpret_cast<const Vector3D *>(this));
			}

			Point3D& GetPoint3D(void)
			{
				return (*reinterpret_cast<Point3D *>(this));
			}

			const Point3D& GetPoint3D(void) const
			{
				return (*reinterpret_cast<const Point3D *>(this));
			}

			Vector2D& GetVector2D(void)
			{
				return (*reinterpret_cast<Vector2D *>(this));
			}

			const Vector2D& GetVector2D(void) const
			{
				return (*reinterpret_cast<const Vector2D *>(this));
			}

			Vector4D& operator =(const Vector4D& v)
			{
				x = v.x;
				y = v.y;
				z = v.z;
				w = v.w;
				return (*this);
			}

			void operator =(const Vector4D& v) volatile
			{
				x = v.x;
				y = v.y;
				z = v.z;
				w = v.w;
			}

			Vector4D& operator =(const Vector3D& v)
			{
				x = v.x;
				y = v.y;
				z = v.z;
				w = 0.0F;
				return (*this);
			}

			void operator =(const Vector3D& v) volatile
			{
				x = v.x;
				y = v.y;
				z = v.z;
				w = 0.0F;
			}

			Vector4D& operator =(const Point3D& p)
			{
				x = p.x;
				y = p.y;
				z = p.z;
				w = 1.0F;
				return (*this);
			}

			void operator =(const Point3D& p) volatile
			{
				x = p.x;
				y = p.y;
				z = p.z;
				w = 1.0F;
			}

			Vector4D& operator =(const Vector2D& v)
			{
				x = v.x;
				y = v.y;
				z = 0.0F;
				w = 0.0F;
				return (*this);
			}

			void operator =(const Vector2D& v) volatile
			{
				x = v.x;
				y = v.y;
				z = 0.0F;
				w = 0.0F;
			}

			Vector4D& operator =(const Point2D& p)
			{
				x = p.x;
				y = p.y;
				z = 0.0F;
				w = 1.0F;
				return (*this);
			}

			void operator =(const Point2D& p) volatile
			{
				x = p.x;
				y = p.y;
				z = 0.0F;
				w = 1.0F;
			}

			Vector4D& operator +=(const Vector4D& v)
			{
				x += v.x;
				y += v.y;
				z += v.z;
				w += v.w;
				return (*this);
			}

			Vector4D& operator +=(const Vector3D& v)
			{
				x += v.x;
				y += v.y;
				z += v.z;
				return (*this);
			}

			Vector4D& operator +=(const Vector2D& v)
			{
				x += v.x;
				y += v.y;
				return (*this);
			}

			Vector4D& operator -=(const Vector4D& v)
			{
				x -= v.x;
				y -= v.y;
				z -= v.z;
				w -= v.w;
				return (*this);
			}

			Vector4D& operator -=(const Vector3D& v)
			{
				x -= v.x;
				y -= v.y;
				z -= v.z;
				return (*this);
			}

			Vector4D& operator -=(const Vector2D& v)
			{
				x -= v.x;
				y -= v.y;
				return (*this);
			}

			Vector4D& operator *=(float t)
			{
				x *= t;
				y *= t;
				z *= t;
				w *= t;
				return (*this);
			}

			Vector4D& operator /=(float t)
			{
				float f = 1.0F / t;
				x *= f;
				y *= f;
				z *= f;
				w *= f;
				return (*this);
			}

			Vector4D& operator &=(const Vector4D& v)
			{
				x *= v.x;
				y *= v.y;
				z *= v.z;
				w *= v.w;
				return (*this);
			}

			Vector4D& Normalize(void)
			{
				return (*this *= InverseSqrt(x * x + y * y + z * z + w * w));
			}

			Vector4D& Standardize(void)
			{
				float f = 1.0F / w;
				x *= f;
				y *= f;
				z *= f;
				w = 1.0F;
				return (*this);
			}

			Point3D ProjectPoint3D(void) const
			{
				float f = 1.0F / w;
				return (Point3D(x * f, y * f, z * f));
			}

			C4API Vector4D& RotateAboutX(float angle);
			C4API Vector4D& RotateAboutY(float angle);
			C4API Vector4D& RotateAboutZ(float angle);
			C4API Vector4D& RotateAboutAxis(float angle, const Vector3D& axis);
	};


	inline Vector4D operator -(const Vector4D& v)
	{
		return (Vector4D(-v.x, -v.y, -v.z, -v.w));
	}

	inline Vector4D operator +(const Vector4D& v1, const Vector4D& v2)
	{
		return (Vector4D(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w));
	}

	inline Vector4D operator +(const Vector4D& v1, const Vector3D& v2)
	{
		return (Vector4D(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w));
	}

	inline Vector4D operator +(const Vector3D& v1, const Vector4D& v2)
	{
		return (Vector4D(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v2.w));
	}

	inline Vector4D operator +(const Vector4D& v1, const Vector2D& v2)
	{
		return (Vector4D(v1.x + v2.x, v1.y + v2.y, v1.z, v1.w));
	}

	inline Vector4D operator +(const Vector2D& v1, const Vector4D& v2)
	{
		return (Vector4D(v1.x + v2.x, v1.y + v2.y, v2.z, v2.w));
	}

	inline Vector4D operator -(const Vector4D& v1, const Vector4D& v2)
	{
		return (Vector4D(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w));
	}

	inline Vector4D operator -(const Vector4D& v1, const Vector3D& v2)
	{
		return (Vector4D(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w));
	}

	inline Vector4D operator -(const Vector3D& v1, const Vector4D& v2)
	{
		return (Vector4D(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, -v2.w));
	}

	inline Vector4D operator -(const Vector4D& v1, const Vector2D& v2)
	{
		return (Vector4D(v1.x - v2.x, v1.y - v2.y, v1.z, v1.w));
	}

	inline Vector4D operator -(const Vector2D& v1, const Vector4D& v2)
	{
		return (Vector4D(v1.x - v2.x, v1.y - v2.y, -v2.z, -v2.w));
	}

	inline Vector4D operator *(const Vector4D& v, float t)
	{
		return (Vector4D(v.x * t, v.y * t, v.z * t, v.w * t));
	}

	inline Vector4D operator *(float t, const Vector4D& v)
	{
		return (Vector4D(t * v.x, t * v.y, t * v.z, t * v.w));
	}

	inline Vector4D operator /(const Vector4D& v, float t)
	{
		float f = 1.0F / t;
		return (Vector4D(v.x * f, v.y * f, v.z * f, v.w * f));
	}

	inline float operator *(const Vector4D& v1, const Vector4D& v2)
	{
		return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w);
	}

	inline float operator *(const Vector4D& v1, const Vector3D& v2)
	{
		return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
	}

	inline float operator *(const Vector3D& v1, const Vector4D& v2)
	{
		return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
	}

	inline float operator *(const Vector4D& v, const Point3D& p)
	{
		return (v.x * p.x + v.y * p.y + v.z * p.z + v.w);
	}

	inline float operator *(const Point3D& v1, const Vector4D& v2)
	{
		return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v2.w);
	}

	inline float operator *(const Vector4D& v1, const Vector2D& v2)
	{
		return (v1.x * v2.x + v1.y * v2.y);
	}

	inline float operator *(const Vector2D& v1, const Vector4D& v2)
	{
		return (v1.x * v2.x + v1.y * v2.y);
	}

	inline float operator *(const Vector4D& v, const Point2D& p)
	{
		return (v.x * p.x + v.y * p.y + v.w);
	}

	inline float operator *(const Point2D& v1, const Vector4D& v2)
	{
		return (v1.x * v2.x + v1.y * v2.y + v2.w);
	}

	inline Vector4D operator &(const Vector4D& v1, const Vector4D& v2)
	{
		return (Vector4D(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w));
	}

	inline bool operator ==(const Vector4D& v1, const Vector4D& v2)
	{
		return ((v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z) && (v1.w == v2.w));
	}

	inline bool operator !=(const Vector4D& v1, const Vector4D& v2)
	{
		return ((v1.x != v2.x) || (v1.y != v2.y) || (v1.z != v2.z) || (v1.w != v2.w));
	}


	inline float Dot(const Vector4D& v1, const Vector4D& v2)
	{
		return (v1 * v2);
	}

	inline Vector4D ProjectOnto(const Vector4D& v1, const Vector4D& v2)
	{
		return (v2 * (v1 * v2));
	}

	inline float Magnitude(const Vector4D& v)
	{
		return (Sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w));
	}

	inline float InverseMag(const Vector4D& v)
	{
		return (InverseSqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w));
	}

	inline float SquaredMag(const Vector4D& v)
	{
		return (v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
	}

	inline Vector4D Normalize(const Vector4D& v)
	{
		return (v * InverseSqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w));
	}

	inline unsigned_int32 Hash(const Vector4D& v)
	{
		return (Math::Hash(4, &v.x));
	}


	struct ConstVector4D
	{
		float	x;
		float	y;
		float	z;
		float	w;

		operator const Vector4D&(void) const
		{
			return (reinterpret_cast<const Vector4D&>(*this));
		}

		const Vector4D *operator &(void) const
		{
			return (reinterpret_cast<const Vector4D *>(this));
		}

		const Vector4D *operator ->(void) const
		{
			return (reinterpret_cast<const Vector4D *>(this));
		}
	};


	class Zero4DType
	{
		private:

			C4API static ConstVector4D zero;

		public:

			operator const Vector4D&(void) const
			{
				return (zero);
			}
	};


	C4API extern const Zero4DType Zero4D;
}


#endif

// ZYUQURM
