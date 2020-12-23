 

#ifndef C4Vector3D_h
#define C4Vector3D_h


//# \component	Math Library
//# \prefix		Math/


#include "C4Vector2D.h"


namespace C4
{
	class DVector3D;


	//# \class	Vector3D	Encapsulates a 3D vector.
	//
	//# The $Vector3D$ class encapsulates a 3D vector.
	//
	//# \def	class Vector3D
	//
	//# \data	Vector3D
	//
	//# \ctor	Vector3D();
	//# \ctor	Vector3D(float r, float s, float t);
	//
	//# \param	r	The value of the <i>x</i> coordinate.
	//# \param	s	The value of the <i>y</i> coordinate.
	//# \param	t	The value of the <i>z</i> coordinate.
	//
	//# \desc
	//# The $Vector3D$ class is used to store a three-dimensional direction vector
	//# having floating-point components <i>x</i>, <i>y</i>, and <i>z</i>. A direction vector
	//# stored in this class is assumed to have a <i>w</i> coordinate of 0 whenever it needs
	//# to be converted to a four-dimensional representation. Three-dimensional points
	//# (for which the <i>w</i> coordinate is 1) should be stored using the $@Point3D@$ class.
	//#
	//# The default constructor leaves the components of the vector undefined.
	//# If the values $r$, $s$, and $t$ are supplied, then they are assigned to the
	//# <i>x</i>, <i>y</i>, and <i>z</i> coordinates of the vector, respectively.
	//
	//# \operator	float& operator [](machine k);
	//#				Returns a reference to the $k$-th component of a vector.
	//#				The value of $k$ must be 0, 1, or 2.
	//
	//# \operator	const float& operator [](machine k) const;
	//#				Returns a constant reference to the $k$-th component of a vector.
	//#				The value of $k$ must be 0, 1, or 2.
	//
	//# \operator	Vector3D& operator +=(const Vector3D& v);
	//#				Adds the vector $v$.
	//
	//# \operator	Vector3D& operator -=(const Vector3D& v);
	//#				Subtracts the vector $v$.
	//
	//# \operator	Vector3D& operator *=(float t);
	//#				Multiplies by the scalar $t$.
	//
	//# \operator	Vector3D& operator /=(float t);
	//#				Divides by the scalar $t$.
	//
	//# \operator	Vector3D& operator %=(const Vector3D& v);
	//#				Calculates the cross product with the vector $v$.
	//
	//# \operator	Vector3D& operator &=(const Vector3D& v);
	//#				Calculates the componentwise product with the vector $v$.
	//
	//# \action		Vector3D operator -(const Vector3D& v);
	//#				Returns the negation of the vector $v$.
	//
	//# \action		Vector3D operator +(const Vector3D& v1, const Vector3D& v2);
	//#				Returns the sum of the vectors $v1$ and $v2$.
	//
	//# \action		Vector3D operator -(const Vector3D& v1, const Vector3D& v);
	//#				Returns the difference of the vectors $v1$ and $v2$.
	//
	//# \action		Vector3D operator *(const Vector3D& v, float t);
	//#				Returns the product of the vector $v$ and the scalar $t$.
	//
	//# \action		Vector3D operator *(float t, const Vector3D& v);
	//#				Returns the product of the vector $v$ and the scalar $t$.
	//
	//# \action		Vector3D operator /(const Vector3D& v, float t);
	//#				Returns the product of the vector $v$ and the inverse of the scalar $t$.
	//
	//# \action		float operator *(const Vector3D& v1, const Vector3D& v2);
	//#				Returns the dot product of the vectors $v1$ and $v2$.
	//
	//# \action		Vector3D operator %(const Vector3D& v1, const Vector3D& v2);
	//#				Returns the cross product of the vectors $v1$ and $v2$
	//
	//# \action		Vector3D operator &(const Vector3D& v1, const Vector3D& v2);
	//#				Returns the componentwise product of the vectors $v1$ and $v2$
	//
	//# \action		bool operator ==(const Vector3D& v1, const Vector3D& v2);
	//#				Returns a boolean value indicating the equality of the two vectors $v1$ and $v2$.
	//
	//# \action		bool operator !=(const Vector3D& v1, const Vector3D& v2);
	//#				Returns a boolean value indicating the inequality of the two vectors $v1$ and $v2$.
	// 
	//# \action		float Magnitude(const Vector3D& v);
	//#				Returns the magnitude of the vector $v$.
	// 
	//# \action		float InverseMag(const Vector3D& v);
	//#				Returns the inverse magnitude of the vector $v$. 
	//
	//# \action		float SquaredMag(const Vector3D& v);
	//#				Returns the squared magnitude of the vector $v$. 
	//
	//# \action		float Dot(const Vector3D& v1, const Vector3D& v2); 
	//#				Returns the dot product between $v1$ and $v2$. 
	//
	//# \action		Vector3D Cross(const Vector3D& v1, const Vector3D& v2);
	//#				Returns the cross product between $v1$ and $v2$.
	// 
	//# \action		Vector3D ProjectOnto(const Vector3D& v1, const Vector3D& v2);
	//#				Returns the projection of $v1$ onto $v2$ scaled by the squared magnitude of $v2$.
	//
	//# \also	$@Point3D@$
	//# \also	$@Vector4D@$


	//# \function	Vector3D::Set		Sets all three components of a vector.
	//
	//# \proto	Vector3D& Set(float r, float s, float t);
	//
	//# \param	r	The new <i>x</i> coordinate.
	//# \param	s	The new <i>y</i> coordinate.
	//# \param	t	The new <i>z</i> coordinate.
	//
	//# \desc
	//# The $Set$ function sets the <i>x</i>, <i>y</i>, and <i>z</i> coordinates of a vector to
	//# the values given by the $r$, $s$, and $t$ parameters, respectively.
	//#
	//# The return value is a reference to the vector object.


	//# \function	Vector3D::Normalize		Normalizes a 3D vector.
	//
	//# \proto	Vector3D& Normalize(void);
	//
	//# \desc
	//# The $Normalize$ function multiplies a 3D vector by the inverse of its magnitude,
	//# normalizing it to unit length. Normalizing the zero vector produces undefined results.
	//#
	//# The return value is a reference to the vector object.


	//# \function	Vector3D::GetVector2D		Returns a reference to a $@Vector2D@$ object.
	//
	//# \proto	Vector2D& GetVector2D(void);
	//# \proto	const Vector2D& GetVector2D(void) const;
	//
	//# \desc
	//# The $GetVector2D$ function returns a reference to a $@Vector2D@$ object that refers to
	//# the same data contained in the <i>x</i> and <i>y</i> coordinates of a $Vector3D$ object.


	//# \function	Vector3D::GetPoint2D		Returns a reference to a $@Point2D@$ object.
	//
	//# \proto	Point2D& GetPoint2D(void);
	//# \proto	const Point2D& GetPoint2D(void) const;
	//
	//# \desc
	//# The $GetPoint2D$ function returns a reference to a $@Point2D@$ object that refers to
	//# the same data contained in the <i>x</i> and <i>y</i> coordinates of a $Vector3D$ object.


	//# \function	Vector3D::RotateAboutX		Rotates a vector about the <i>x</i> axis.
	//
	//# \proto	Vector3D& RotateAboutX(float angle);
	//
	//# \param	angle	The angle through which the vector is rotated, in radians.
	//
	//# \desc
	//# The $RotateAboutX$ function rotates a vector about the <i>x</i> axis through the
	//# angle given by the $angle$ parameter.
	//#
	//# The return value is a reference to the vector object.
	//
	//# \also	$@Vector3D::RotateAboutY@$
	//# \also	$@Vector3D::RotateAboutZ@$
	//# \also	$@Vector3D::RotateAboutAxis@$


	//# \function	Vector3D::RotateAboutY		Rotates a vector about the <i>y</i> axis.
	//
	//# \proto	Vector3D& RotateAboutY(float angle);
	//
	//# \param	angle	The angle through which the vector is rotated, in radians.
	//
	//# \desc
	//# The $RotateAboutY$ function rotates a vector about the <i>y</i> axis through the
	//# angle given by the $angle$ parameter.
	//#
	//# The return value is a reference to the vector object.
	//
	//# \also	$@Vector3D::RotateAboutX@$
	//# \also	$@Vector3D::RotateAboutZ@$
	//# \also	$@Vector3D::RotateAboutAxis@$


	//# \function	Vector3D::RotateAboutZ		Rotates a vector about the <i>z</i> axis.
	//
	//# \proto	Vector3D& RotateAboutZ(float angle);
	//
	//# \param	angle	The angle through which the vector is rotated, in radians.
	//
	//# \desc
	//# The $RotateAboutZ$ function rotates a vector about the <i>z</i> axis through the
	//# angle given by the $angle$ parameter.
	//#
	//# The return value is a reference to the vector object.
	//
	//# \also	$@Vector3D::RotateAboutX@$
	//# \also	$@Vector3D::RotateAboutY@$
	//# \also	$@Vector3D::RotateAboutAxis@$


	//# \function	Vector3D::RotateAboutAxis		Rotates a vector about a given axis.
	//
	//# \proto	Vector3D& RotateAboutAxis(float angle, const Vector3D& axis);
	//
	//# \param	angle	The angle through which the vector is rotated, in radians.
	//# \param	axis	The axis about which the vector is rotated.
	//
	//# \desc
	//# The $RotateAboutAxis$ function rotates a vector through the angle given by the
	//# $angle$ parameter about the axis given by the $axis$ parameter.
	//#
	//# The return value is a reference to the vector object.
	//
	//# \also	$@Vector3D::RotateAboutX@$
	//# \also	$@Vector3D::RotateAboutY@$
	//# \also	$@Vector3D::RotateAboutZ@$


	//# \member		Vector3D

	class Vector3D
	{
		public:

			float	x;		//## The <i>x</i> coordinate.
			float	y;		//## The <i>y</i> coordinate.
			float	z;		//## The <i>z</i> coordinate.

			Vector3D() = default;

			Vector3D(float r, float s, float t)
			{
				x = r;
				y = s;
				z = t;
			}

			Vector3D(const Vector2D& v)
			{
				x = v.x;
				y = v.y;
				z = 0.0F;
			}

			Vector3D(const Vector2D& v, float u)
			{
				x = v.x;
				y = v.y;
				z = u;
			}

			Vector3D& Set(float r, float s, float t)
			{
				x = r;
				y = s;
				z = t;
				return (*this);
			}

			void Set(float r, float s, float t) volatile
			{
				x = r;
				y = s;
				z = t;
			}

			Vector3D& Set(const Vector2D& v, float u)
			{
				x = v.x;
				y = v.y;
				z = u;
				return (*this);
			}

			void Set(const Vector2D& v, float u) volatile
			{
				x = v.x;
				y = v.y;
				z = u;
			}

			float& operator [](machine k)
			{
				return ((&x)[k]);
			}

			const float& operator [](machine k) const
			{
				return ((&x)[k]);
			}

			Vector2D& GetVector2D(void)
			{
				return (*reinterpret_cast<Vector2D *>(this));
			}

			const Vector2D& GetVector2D(void) const
			{
				return (*reinterpret_cast<const Vector2D *>(this));
			}

			Vector2D& GetVectorYZ(void)
			{
				return (*reinterpret_cast<Vector2D *>(&y));
			}

			const Vector2D& GetVectorYZ(void) const
			{
				return (*reinterpret_cast<const Vector2D *>(&y));
			}

			Point2D& GetPoint2D(void)
			{
				return (*reinterpret_cast<Point2D *>(this));
			}

			const Point2D& GetPoint2D(void) const
			{
				return (*reinterpret_cast<const Point2D *>(this));
			}

			Vector3D& operator =(const Vector3D& v)
			{
				x = v.x;
				y = v.y;
				z = v.z;
				return (*this);
			}

			void operator =(const Vector3D& v) volatile
			{
				x = v.x;
				y = v.y;
				z = v.z;
			}

			Vector3D& operator =(const Vector2D& v)
			{
				x = v.x;
				y = v.y;
				z = 0.0F;
				return (*this);
			}

			void operator =(const Vector2D& v) volatile
			{
				x = v.x;
				y = v.y;
				z = 0.0F;
			}

			Vector3D& operator =(const DVector3D& v);

			Vector3D& operator +=(const Vector3D& v)
			{
				x += v.x;
				y += v.y;
				z += v.z;
				return (*this);
			}

			Vector3D& operator +=(const Vector2D& v)
			{
				x += v.x;
				y += v.y;
				return (*this);
			}

			Vector3D& operator -=(const Vector3D& v)
			{
				x -= v.x;
				y -= v.y;
				z -= v.z;
				return (*this);
			}

			Vector3D& operator -=(const Vector2D& v)
			{
				x -= v.x;
				y -= v.y;
				return (*this);
			}

			Vector3D& operator *=(float t)
			{
				x *= t;
				y *= t;
				z *= t;
				return (*this);
			}

			Vector3D& operator /=(float t)
			{
				float f = 1.0F / t;
				x *= f;
				y *= f;
				z *= f;
				return (*this);
			}

			Vector3D& operator %=(const Vector3D& v)
			{
				float		r, s;

				r = y * v.z - z * v.y;
				s = z * v.x - x * v.z;
				z = x * v.y - y * v.x;
				x = r;
				y = s;

				return (*this);
			}

			Vector3D& operator &=(const Vector3D& v)
			{
				x *= v.x;
				y *= v.y;
				z *= v.z;
				return (*this);
			}

			Vector3D& Normalize(void)
			{
				return (*this *= InverseSqrt(x * x + y * y + z * z));
			}

			C4API Vector3D& RotateAboutX(float angle);
			C4API Vector3D& RotateAboutY(float angle);
			C4API Vector3D& RotateAboutZ(float angle);
			C4API Vector3D& RotateAboutAxis(float angle, const Vector3D& axis);
	};


	inline Vector3D operator -(const Vector3D& v)
	{
		return (Vector3D(-v.x, -v.y, -v.z));
	}

	inline Vector3D operator +(const Vector3D& v1, const Vector3D& v2)
	{
		return (Vector3D(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z));
	}

	inline Vector3D operator +(const Vector3D& v1, const Vector2D& v2)
	{
		return (Vector3D(v1.x + v2.x, v1.y + v2.y, v1.z));
	}

	inline Vector3D operator -(const Vector3D& v1, const Vector3D& v2)
	{
		return (Vector3D(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z));
	}

	inline Vector3D operator -(const Vector3D& v1, const Vector2D& v2)
	{
		return (Vector3D(v1.x - v2.x, v1.y - v2.y, v1.z));
	}

	inline Vector3D operator *(const Vector3D& v, float t)
	{
		return (Vector3D(v.x * t, v.y * t, v.z * t));
	}

	inline Vector3D operator *(float t, const Vector3D& v)
	{
		return (Vector3D(t * v.x, t * v.y, t * v.z));
	}

	inline Vector3D operator /(const Vector3D& v, float t)
	{
		float f = 1.0F / t;
		return (Vector3D(v.x * f, v.y * f, v.z * f));
	}

	inline float operator *(const Vector3D& v1, const Vector3D& v2)
	{
		return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
	}

	inline float operator *(const Vector3D& v1, const Vector2D& v2)
	{
		return (v1.x * v2.x + v1.y * v2.y);
	}

	inline Vector3D operator %(const Vector3D& v1, const Vector3D& v2)
	{
		return (Vector3D(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x));
	}

	inline Vector3D operator &(const Vector3D& v1, const Vector3D& v2)
	{
		return (Vector3D(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z));
	}

	inline bool operator ==(const Vector3D& v1, const Vector3D& v2)
	{
		return ((v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z));
	}

	inline bool operator !=(const Vector3D& v1, const Vector3D& v2)
	{
		return ((v1.x != v2.x) || (v1.y != v2.y) || (v1.z != v2.z));
	}


	//# \class	Point3D		Encapsulates a 3D point.
	//
	//# The $Point3D$ class encapsulates a 3D point.
	//
	//# \def	class Point3D : public Vector3D
	//
	//# \ctor	Point3D();
	//# \ctor	Point3D(float r, float s, float t);
	//
	//# \param	r	The value of the <i>x</i> coordinate.
	//# \param	s	The value of the <i>y</i> coordinate.
	//# \param	t	The value of the <i>z</i> coordinate.
	//
	//# \desc
	//# The $Point3D$ class is used to store a three-dimensional point having floating-point
	//# coordinates <i>x</i>, <i>y</i>, and <i>z</i>. The difference between a point and a
	//# vector is that a point is assumed to have a <i>w</i> coordinate of 1 whenever it
	//# needs to be converted to a four-dimensional representation, whereas a vector is
	//# assumed to have a <i>w</i> coordinate of 0. Such a conversion occurs when a vector or
	//# point is assigned to a $@Vector4D@$ object or is multiplied by a $@Transform4D@$ object.
	//#
	//# The default constructor leaves the components of the vector undefined.
	//# If the values $r$, $s$, and $t$ are supplied, then they are assigned to the
	//# <i>x</i>, <i>y</i>, and <i>z</i> coordinates of the base vector object, respectively.
	//#
	//# The difference between two points produces a direction vector. A three-dimensional
	//# direction vector is converted to a point by adding it to the identifier $Zero3D$.
	//
	//# \operator	Point3D& operator *=(float t);
	//#				Multiplies by the scalar $t$.
	//
	//# \operator	Point3D& operator /=(float t);
	//#				Divides by the scalar $t$.
	//
	//# \action		Point3D operator -(const Point3D& p);
	//#				Returns the negation of the point $p$.
	//
	//# \action		Point3D operator +(const Point3D& p1, const Point3D& p2);
	//#				Returns the sum of the points $p1$ and $p2$.
	//
	//# \action		Point3D operator +(const Point3D& p, const Vector3D& v);
	//#				Returns the sum of the point $p$ and the vector $v$.
	//
	//# \action		Vector3D operator -(const Point3D& p1, const Point3D& p2);
	//#				Returns the difference of the points $p1$ and $p2$.
	//
	//# \action		Point3D operator -(const Point3D& p, const Vector3D& v);
	//#				Returns the difference of the point $p$ and the vector $v$.
	//
	//# \action		Point3D operator -(const Vector3D& v, const Point3D& p);
	//#				Returns the difference of the vector $v$ and the point $p$.
	//
	//# \action		Point3D operator *(const Point3D& p, float t);
	//#				Returns the product of the point $p$ and the scalar $t$.
	//
	//# \action		Point3D operator *(float t, const Point3D& p);
	//#				Returns the product of the point $p$ and the scalar $t$.
	//
	//# \action		Point3D operator /(const Point3D& p, float t);
	//#				Returns the product of the point $p$ and the inverse of the scalar $t$.
	//
	//# \action		float operator *(const Point3D& p1, const Point3D& p2);
	//#				Returns the dot product of the points $p1$ and $p2$.
	//
	//# \action		float operator *(const Point3D& p, const Vector3D& v);
	//#				Returns the dot product of the point $p$ and the vector $v$.
	//
	//# \action		float operator *(const Vector3D& v, const Point3D& p);
	//#				Returns the dot product of the vector $v$ and the point $p$.
	//
	//# \action		float operator *(const Point3D& p, const Vector2D& v);
	//#				Returns the dot product of the point $p$ and the vector $v$.
	//
	//# \action		float operator *(const Vector2D& v, const Point3D& p);
	//#				Returns the dot product of the vector $v$ and the point $p$.
	//
	//# \action		Vector3D operator %(const Point3D& p1, const Point3D& p2);
	//#				Returns the cross product of the points $p1$ and $p2$.
	//
	//# \action		Vector3D operator %(const Point3D& p, const Vector3D& v);
	//#				Returns the cross product of the point $p$ and the vector $v$.
	//
	//# \action		Vector3D operator %(const Vector3D& v, const Point3D& p);
	//#				Returns the cross product of the vector $v$ and the point $p$.
	//
	//# \action		Point3D operator &(const Point3D& p1, const Point3D& p2);
	//#				Returns the componentwise product of the points $p1$ and $p2$.
	//
	//# \action		Point3D operator &(const Point3D& p, const Vector3D& v);
	//#				Returns the componentwise product of the point $p$ and the vector $v$.
	//
	//# \action		Point3D operator &(const Vector3D& v, const Point3D& p);
	//#				Returns the componentwise product of the vector $v$ and the point $p$.
	//
	//# \base	Vector3D	A $Point3D$ object behaves much like a $@Vector3D@$ object, but some
	//#						properties are altered.
	//
	//# \also	$@Vector3D@$
	//# \also	$@Vector4D@$


	class Point3D : public Vector3D
	{
		public:

			Point3D() = default;

			Point3D(float r, float s, float t) : Vector3D(r, s, t) {}
			Point3D(const Vector2D& v) : Vector3D(v) {}
			Point3D(const Vector2D& v, float u) : Vector3D(v, u) {}

			Vector3D& GetVector3D(void)
			{
				return (*this);
			}

			const Vector3D& GetVector3D(void) const
			{
				return (*this);
			}

			Point2D& GetPoint2D(void)
			{
				return (*reinterpret_cast<Point2D *>(this));
			}

			const Point2D& GetPoint2D(void) const
			{
				return (*reinterpret_cast<const Point2D *>(this));
			}

			Point3D& operator =(const Vector3D& v)
			{
				x = v.x;
				y = v.y;
				z = v.z;
				return (*this);
			}

			void operator =(const Vector3D& v) volatile
			{
				x = v.x;
				y = v.y;
				z = v.z;
			}

			Point3D& operator =(const Vector2D& v)
			{
				x = v.x;
				y = v.y;
				z = 0.0F;
				return (*this);
			}

			void operator =(const Vector2D& v) volatile
			{
				x = v.x;
				y = v.y;
				z = 0.0F;
			}

			Point3D& operator =(const DVector3D& v);

			Point3D& operator *=(float t)
			{
				x *= t;
				y *= t;
				z *= t;
				return (*this);
			}

			Point3D& operator /=(float t)
			{
				float f = 1.0F / t;
				x *= f;
				y *= f;
				z *= f;
				return (*this);
			}

			Point3D& operator &=(const Vector3D& v)
			{
				x *= v.x;
				y *= v.y;
				z *= v.z;
				return (*this);
			}
	};


	inline Point3D operator -(const Point3D& p)
	{
		return (Point3D(-p.x, -p.y, -p.z));
	}

	inline Point3D operator +(const Point3D& p1, const Point3D& p2)
	{
		return (Point3D(p1.x + p2.x, p1.y + p2.y, p1.z + p2.z));
	}

	inline Point3D operator +(const Point3D& p, const Vector3D& v)
	{
		return (Point3D(p.x + v.x, p.y + v.y, p.z + v.z));
	}

	inline Point3D operator +(const Vector3D& v, const Point3D& p)
	{
		return (Point3D(v.x + p.x, v.y + p.y, v.z + p.z));
	}

	inline Vector3D operator -(const Point3D& p1, const Point3D& p2)
	{
		return (Vector3D(p1.x - p2.x, p1.y - p2.y, p1.z - p2.z));
	}

	inline Point3D operator -(const Point3D& p, const Vector3D& v)
	{
		return (Point3D(p.x - v.x, p.y - v.y, p.z - v.z));
	}

	inline Point3D operator -(const Vector3D& v, const Point3D& p)
	{
		return (Point3D(v.x - p.x, v.y - p.y, v.z - p.z));
	}

	inline Point3D operator *(const Point3D& p, float t)
	{
		return (Point3D(p.x * t, p.y * t, p.z * t));
	}

	inline Point3D operator *(float t, const Point3D& p)
	{
		return (Point3D(t * p.x, t * p.y, t * p.z));
	}

	inline Point3D operator /(const Point3D& p, float t)
	{
		float f = 1.0F / t;
		return (Point3D(p.x * f, p.y * f, p.z * f));
	}

	inline float operator *(const Point3D& p1, const Point3D& p2)
	{
		return (p1.x * p2.x + p1.y * p2.y + p1.z * p2.z);
	}

	inline float operator *(const Point3D& p, const Vector3D& v)
	{
		return (p.x * v.x + p.y * v.y + p.z * v.z);
	}

	inline float operator *(const Vector3D& v, const Point3D& p)
	{
		return (v.x * p.x + v.y * p.y + v.z * p.z);
	}

	inline float operator *(const Point3D& p, const Vector2D& v)
	{
		return (p.x * v.x + p.y * v.y);
	}

	inline float operator *(const Vector2D& v, const Point3D& p)
	{
		return (v.x * p.x + v.y * p.y);
	}

	inline Vector3D operator %(const Point3D& p1, const Point3D& p2)
	{
		return (Vector3D(p1.y * p2.z - p1.z * p2.y, p1.z * p2.x - p1.x * p2.z, p1.x * p2.y - p1.y * p2.x));
	}

	inline Vector3D operator %(const Point3D& p, const Vector3D& v)
	{
		return (Vector3D(p.y * v.z - p.z * v.y, p.z * v.x - p.x * v.z, p.x * v.y - p.y * v.x));
	}

	inline Vector3D operator %(const Vector3D& v, const Point3D& p)
	{
		return (Vector3D(v.y * p.z - v.z * p.y, v.z * p.x - v.x * p.z, v.x * p.y - v.y * p.x));
	}

	inline Point3D operator &(const Point3D& p1, const Point3D& p2)
	{
		return (Point3D(p1.x * p2.x, p1.y * p2.y, p1.z * p2.z));
	}

	inline Point3D operator &(const Point3D& p, const Vector3D& v)
	{
		return (Point3D(p.x * v.x, p.y * v.y, p.z * v.z));
	}

	inline Point3D operator &(const Vector3D& v, const Point3D& p)
	{
		return (Point3D(v.x * p.x, v.y * p.y, v.z * p.z));
	}


	inline float Dot(const Vector3D& v1, const Vector3D& v2)
	{
		return (v1 * v2);
	}

	inline float Dot(const Point3D& p, const Vector3D& v)
	{
		return (p * v);
	}

	inline Vector3D Cross(const Vector3D& v1, const Vector3D& v2)
	{
		return (v1 % v2);
	}

	inline Vector3D Cross(const Point3D& p, const Vector3D& v)
	{
		return (p % v);
	}

	inline Vector3D ProjectOnto(const Vector3D& v1, const Vector3D& v2)
	{
		return (v2 * (v1 * v2));
	}

	inline float Magnitude(const Vector3D& v)
	{
		return (Sqrt(v.x * v.x + v.y * v.y + v.z * v.z));
	}

	inline float InverseMag(const Vector3D& v)
	{
		return (InverseSqrt(v.x * v.x + v.y * v.y + v.z * v.z));
	}

	inline float SquaredMag(const Vector3D& v)
	{
		return (v.x * v.x + v.y * v.y + v.z * v.z);
	}

	inline Vector3D Normalize(const Vector3D& v)
	{
		return (v * InverseSqrt(v.x * v.x + v.y * v.y + v.z * v.z));
	}

	inline unsigned_int32 Hash(const Vector3D& v)
	{
		return (Math::Hash(3, &v.x));
	}


	#if C4SIMD

		class alignas(16) SimdVector3D : public Vector3D
		{
			public:

				operator vec_float&(void)
				{
					return (*reinterpret_cast<vec_float *>(this));
				}

				operator const vec_float&(void) const
				{
					return (*reinterpret_cast<const vec_float *>(this));
				}

				SimdVector3D& operator =(vec_float v)
				{
					*reinterpret_cast<vec_float *>(this) = v;
					return (*this);
				}

				using Vector3D::operator =;
		};

		class alignas(16) SimdPoint3D : public Point3D
		{
			public:

				operator vec_float&(void)
				{
					return (*reinterpret_cast<vec_float *>(this));
				}

				operator const vec_float&(void) const
				{
					return (*reinterpret_cast<const vec_float *>(this));
				}

				SimdPoint3D& operator =(vec_float v)
				{
					*reinterpret_cast<vec_float *>(this) = v;
					return (*this);
				}

				using Point3D::operator =;
		};

	#else

		typedef Vector3D SimdVector3D;
		typedef Point3D SimdPoint3D;

	#endif


	typedef Vector3D Antivector3D;


	inline Antivector3D operator ^(const Vector3D& v1, const Vector3D& v2)
	{
		return (Antivector3D(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x));
	}


	struct ConstVector3D
	{
		float	x;
		float	y;
		float	z;

		operator const Vector3D&(void) const
		{
			return (reinterpret_cast<const Vector3D&>(*this));
		}

		const Vector3D *operator &(void) const
		{
			return (reinterpret_cast<const Vector3D *>(this));
		}
	};


	struct ConstPoint3D
	{
		float	x;
		float	y;
		float	z;

		operator const Point3D&(void) const
		{
			return (reinterpret_cast<const Point3D&>(*this));
		}

		const Point3D *operator &(void) const
		{
			return (reinterpret_cast<const Point3D *>(this));
		}

		const Point3D *operator ->(void) const
		{
			return (reinterpret_cast<const Point3D *>(this));
		}
	};


	class Zero3DType
	{
		private:

			C4API static ConstPoint3D zero;

		public:

			operator const Vector3D&(void) const
			{
				return (zero);
			}

			operator const Point3D&(void) const
			{
				return (zero);
			}
	};


	inline const Point3D& operator +(const Zero3DType&, const Vector3D& v)
	{
		return (static_cast<const Point3D&>(v));
	}

	inline Point3D operator -(const Zero3DType&, const Vector3D& v)
	{
		return (Point3D(-v.x, -v.y, -v.z));
	}


	C4API extern const Zero3DType Zero3D;


	class DVector3D
	{
		public:

			double		x;
			double		y;
			double		z;

			DVector3D() = default;

			DVector3D(double r, double s, double t)
			{
				x = r;
				y = s;
				z = t;
			}

			DVector3D(const Vector3D& v)
			{
				x = v.x;
				y = v.y;
				z = v.z;
			}

			DVector3D& Set(double r, double s, double t)
			{
				x = r;
				y = s;
				z = t;
				return (*this);
			}

			double& operator [](machine k)
			{
				return ((&x)[k]);
			}

			const double& operator [](machine k) const
			{
				return ((&x)[k]);
			}

			DVector3D& operator =(const DVector3D& v)
			{
				x = v.x;
				y = v.y;
				z = v.z;
				return (*this);
			}

			DVector3D& operator =(const Vector3D& v)
			{
				x = v.x;
				y = v.y;
				z = v.z;
				return (*this);
			}

			DVector3D& operator +=(const DVector3D& v)
			{
				x += v.x;
				y += v.y;
				z += v.z;
				return (*this);
			}

			DVector3D& operator -=(const DVector3D& v)
			{
				x -= v.x;
				y -= v.y;
				z -= v.z;
				return (*this);
			}

			DVector3D& operator *=(double t)
			{
				x *= t;
				y *= t;
				z *= t;
				return (*this);
			}

			DVector3D& operator /=(double t)
			{
				double f = 1.0 / t;
				x *= f;
				y *= f;
				z *= f;
				return (*this);
			}

			DVector3D& operator %=(const DVector3D& v)
			{
				double		r, s;

				r = y * v.z - z * v.y;
				s = z * v.x - x * v.z;
				z = x * v.y - y * v.x;
				x = r;
				y = s;

				return (*this);
			}

			DVector3D& operator &=(const DVector3D& v)
			{
				x *= v.x;
				y *= v.y;
				z *= v.z;
				return (*this);
			}
	};


	inline Vector3D& Vector3D::operator =(const DVector3D& v)
	{
		x = float(v.x);
		y = float(v.y);
		z = float(v.z);
		return (*this);
	}

	inline Point3D& Point3D::operator =(const DVector3D& v)
	{
		x = float(v.x);
		y = float(v.y);
		z = float(v.z);
		return (*this);
	}

	inline DVector3D operator -(const DVector3D& v)
	{
		return (DVector3D(-v.x, -v.y, -v.z));
	}

	inline DVector3D operator +(const DVector3D& v1, const DVector3D& v2)
	{
		return (DVector3D(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z));
	}

	inline DVector3D operator +(const Vector3D& v1, const DVector3D& v2)
	{
		return (DVector3D(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z));
	}

	inline DVector3D operator +(const DVector3D& v1, const Vector3D& v2)
	{
		return (DVector3D(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z));
	}

	inline DVector3D operator -(const DVector3D& v1, const DVector3D& v2)
	{
		return (DVector3D(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z));
	}

	inline DVector3D operator -(const Vector3D& v1, const DVector3D& v2)
	{
		return (DVector3D(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z));
	}

	inline DVector3D operator -(const DVector3D& v1, const Vector3D& v2)
	{
		return (DVector3D(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z));
	}

	inline DVector3D operator *(const DVector3D& v, double t)
	{
		return (DVector3D(v.x * t, v.y * t, v.z * t));
	}

	inline DVector3D operator *(double t, const DVector3D& v)
	{
		return (DVector3D(t * v.x, t * v.y, t * v.z));
	}

	inline DVector3D operator /(const DVector3D& v, double t)
	{
		double f = 1.0 / t;
		return (DVector3D(v.x * f, v.y * f, v.z * f));
	}

	inline double operator *(const DVector3D& v1, const DVector3D& v2)
	{
		return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
	}

	inline double operator *(const Vector3D& v1, const DVector3D& v2)
	{
		return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
	}

	inline double operator *(const DVector3D& v1, const Vector3D& v2)
	{
		return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
	}

	inline DVector3D operator %(const DVector3D& v1, const DVector3D& v2)
	{
		return (DVector3D(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x));
	}

	inline DVector3D operator %(const Vector3D& v1, const DVector3D& v2)
	{
		return (DVector3D(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x));
	}

	inline DVector3D operator %(const DVector3D& v1, const Vector3D& v2)
	{
		return (DVector3D(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x));
	}

	inline DVector3D operator &(const DVector3D& v1, const DVector3D& v2)
	{
		return (DVector3D(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z));
	}

	inline DVector3D operator &(const Vector3D& v1, const DVector3D& v2)
	{
		return (DVector3D(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z));
	}

	inline DVector3D operator &(const DVector3D& v1, const Vector3D& v2)
	{
		return (DVector3D(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z));
	}

	inline bool operator ==(const DVector3D& v1, const DVector3D& v2)
	{
		return ((v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z));
	}

	inline bool operator !=(const DVector3D& v1, const DVector3D& v2)
	{
		return ((v1.x != v2.x) || (v1.y != v2.y) || (v1.z != v2.z));
	}

	inline DVector3D ProjectOnto(const DVector3D& v1, const DVector3D& v2)
	{
		return (v2 * (v1 * v2));
	}

	inline DVector3D ProjectOnto(const Vector3D& v1, const DVector3D& v2)
	{
		return (v2 * (v1 * v2));
	}

	inline DVector3D ProjectOnto(const DVector3D& v1, const Vector3D& v2)
	{
		return (DVector3D(v2) * (v1 * v2));
	}

	inline double SquaredMag(const DVector3D& v)
	{
		return (v.x * v.x + v.y * v.y + v.z * v.z);
	}
}


#endif

// ZYUQURM
