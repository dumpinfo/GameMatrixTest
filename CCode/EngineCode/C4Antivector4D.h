#ifndef C4Antivector4D_h
#define C4Antivector4D_h


//# \component	Math Library
//# \prefix		Math/


#include "C4Vector4D.h"


namespace C4
{
	//# \class	Antivector4D	Encapsulates a 4D antivector.
	//
	//# The $Antivector4D$ class encapsulates a 4D antivector.
	//
	//# \def	class Antivector4D
	//
	//# \data	Antivector4D
	//
	//# \ctor	Antivector4D();
	//# \ctor	Antivector4D(float r, float s, float t, float u);
	//# \ctor	Antivector4D(const Antivector3D& n, float u);
	//# \ctor	Antivector4D(const Antivector3D& n, const Point3D& p);
	//# \ctor	Antivector4D(const Point3D& p1, const Point3D& p2, const Point3D& p3);
	//
	//# \param	r			The value of the <i>x</i> coordinate.
	//# \param	s			The value of the <i>y</i> coordinate.
	//# \param	t			The value of the <i>z</i> coordinate.
	//# \param	u			The value of the <i>w</i> coordinate.
	//# \param	n			A 3D antivector whose entries are copied to the <i>x</i>, <i>y</i>, and <i>z</i> coordinates.
	//# \param	p			A 3D point lying in the plane.
	//# \param	p1,p2,p3	Three 3D points that lie in the plane.
	//
	//# \desc
	//# The $Antivector4D$ class is used to store a four-dimensional antivector
	//# having floating-point components <i>x</i>, <i>y</i>, <i>z</i>, and <i>w</i>.
	//#
	//# The default constructor leaves the components of the antivector undefined.
	//# If the values $r$, $s$, $t$, and $u$ are supplied, then they are assigned to the
	//# <i>x</i>, <i>y</i>, <i>z</i>, and <i>w</i> coordinates of the antivector, respectively.
	//#
	//# If the $p$ parameter is specified, then the <i>w</i> coordinate is given by &minus;($n$&nbsp;&and;&nbsp;$p$).
	//# If the $p1$, $p2$, and $p3$ parameters are specified, then the antivector is set to the
	//# triple wedge product $p1$&nbsp;&and;&nbsp;$p2$&nbsp;&and;&nbsp;$p3$.
	//
	//# \operator	float& operator [](machine k);
	//#				Returns a reference to the $k$-th component of an antivector.
	//#				The value of $k$ must be 0, 1, 2, or 3.
	//
	//# \operator	const float& operator [](machine k) const;
	//#				Returns a constant reference to the $k$-th component of an antivector.
	//#				The value of $k$ must be 0, 1, 2, or 3.
	//
	//# \operator	Antivector4D& operator *=(float t);
	//#				Multiplies by the scalar $t$.
	//
	//# \operator	Antivector4D& operator /=(float t);
	//#				Multiplies by the inverse of the scalar $t$.
	//
	//# \action		Antivector4D operator -(const Antivector4D& v);
	//#				Returns the negation of the antivector $v$.
	//
	//# \action		Antivector4D operator *(const Antivector4D& v, float t);
	//#				Returns the product of the antivector $v$ and the scalar $t$.
	//
	//# \action		Antivector4D operator *(float t, const Antivector4D& v);
	//#				Returns the product of the antivector $v$ and the scalar $t$.
	//
	//# \action		Antivector4D operator /(const Antivector4D& v, float t);
	//#				Returns the product of the antivector $v$ and the inverse of the scalar $t$.
	//
	//# \action		float operator ^(const Antivector4D& v1, const Vector4D& v2);
	//#				Returns the negative antiwedge product of the antivector $v1$ and the vector $v2$. (The negative value is returned because vectors and antivectors anticommute in four dimensions.)
	//
	//# \action		float operator ^(const Vector4D& v1, const Antivector4D& v2);
	//#				Returns the antiwedge product of the vector $v1$ and the antivector $v2$.
	//
	//# \action		float operator ^(const Antivector4D& v1, const Vector3D& v2);
	//#				Returns the negative antiwedge product of the antivector $v1$ and the vector $v2$. The <i>w</i> coordinate of $v2$ is assumed to be 0. (The negative value is returned because vectors and antivectors anticommute in four dimensions.)
	//
	//# \action		float operator ^(const Antivector4D& v, const Point3D& p);
	//#				Returns the negative antiwedge product of the antivector $v$ and the point $p$. The <i>w</i> coordinate of $p$ is assumed to be 1. (The negative value is returned because vectors and antivectors anticommute in four dimensions.)
	//#				(This gives the distance from a plane, with standardized normal, represented by an $Antivector4D$ object to the point $p$.)
	//
	//# \action		float operator ^(const Antivector4D& v, const Point2D& p);
	//#				Returns the negative antiwedge product of the antivector $v$ and the point $p$. The <i>z</i> coordinate of $p$ is assumed to be 0, and the <i>w</i> coordinate of $p$ is assumed to be 1. (The negative value is returned because vectors and antivectors anticommute in four dimensions.)
	//#				(This gives the distance from a plane, with standardized normal, represented by an $Antivector4D$ object to the point $p$.)
	//
	//# \action		bool operator ==(const Antivector4D& v1, const Antivector4D& v2);
	//#				Returns a boolean value indicating the equality of the two antivectors $v1$ and $v2$.
	//
	//# \action		bool operator !=(const Antivector4D& v1, const Antivector4D& v2);
	//#				Returns a boolean value indicating the inequality of the two antivectors $v1$ and $v2$
	//
	//# \also	$@Vector4D@$
	//# \also	$@Point3D@$
	//# \also	$@Point2D@$


	//# \function	Antivector4D::Set		Sets all four components of an antivector.
	//
	//# \proto	Antivector4D& Set(float r, float s, float t, float u);
	//# \proto	Antivector4D& Set(const Antivector3D& n, float u);
	//# \proto	Antivector4D& Set(const Antivector3D& n, const Point3D& p);
	//# \proto	Antivector4D& Set(const Point3D& p1, const Point3D& p2, const Point3D& p3);
	//
	//# \param	r			The new <i>x</i> coordinate.
	//# \param	s			The new <i>y</i> coordinate.
	//# \param	t			The new <i>z</i> coordinate.
	//# \param	u			The new <i>w</i> coordinate.
	//# \param	n			A 3D antivector whose entries are copied to the <i>x</i>, <i>y</i>, and <i>z</i> coordinates.
	//# \param	p			A 3D point lying in the plane.
	//# \param	p1,p2,p3	Three 3D points that lie in the plane.
	//
	//# \desc
	//# The $Set$ function sets the <i>x</i>, <i>y</i>, <i>z</i>, and <i>w</i> coordinates of a vector to
	//# the values given by the $r$, $s$, $t$, and $u$ parameters, respectively.
	//#
	//# If the $p$ parameter is specified, then the <i>w</i> coordinate is given by &minus;($n$&nbsp;&and;&nbsp;$p$).
	//# If the $p1$, $p2$, and $p3$ parameters are specified, then the antivector is set to the
	//# triple wedge product $p1$&nbsp;&and;&nbsp;$p2$&nbsp;&and;&nbsp;$p3$.
	//#
	//# The return value is a reference to the antivector object.


	//# \function	Antivector4D::SetStandard	Sets an antivector to the plane containing three points and standardizes it.
	//
	//# \proto	Antivector4D& SetStandard(const Point3D& p1, const Point3D& p2, const Point3D& p3);
	//
	//# \param	p1,p2,p3	Three 3D points that lie in the plane.
	//
	//# \desc
	//# The $SetStandard$ function sets an antivector to the triple wedge product $p1$&nbsp;&and;&nbsp;$p2$&nbsp;&and;&nbsp;$p3$
	//# and then standardizes it by dividing by the magnitude of the 3D antivector given by the <i>x</i>, <i>y</i>, and <i>z</i>
	//# so that the plane's normal has unit length.
	//#
	//# The return value is a reference to the antivector object.


	//# \function	Antivector4D::Normalize		Normalizes a 4D antivector.
	//
	//# \proto	Antivector4D& Normalize(void);
	//
	//# \desc
	//# The $Normalize$ function multiplies a 4D antivector by the inverse of its magnitude,
	//# normalizing it to unit length. Normalizing the zero vector produces undefined results.
	//#
	//# The return value is a reference to the antivector object.


	//# \function	Antivector4D::Standardize		Standardizes a 4D antivector.
	//
	//# \proto	Antivector4D& Standardize(void);
	//
	//# \desc
	//# The $Standardize$ function multiplies a 4D antivector by the inverse magnitude of the 3D antivector
	//# given by its <i>x</i>, <i>y</i>, and <i>z</i> coordinates, transforming it into a plane having a unit normal.
	//# If the <i>x</i>, <i>y</i>, and <i>z</i> coordinates are all zero, then the result is undefined.
	//#
	//# The return value is a reference to the antivector object.


	//# \function	Antivector4D::GetAntivector3D		Returns a reference to an $Antivector3D$ object.
	//
	//# \proto	Antivector3D& GetAntivector3D(void);
	//# \proto	const Antivector3D& GetAntivector3D(void) const;
	//
	//# \desc
	//# The $GetAntivector3D$ function returns a reference to a $Antivector3D$ object that refers to
	//# the same data contained in the <i>x</i>, <i>y</i>, and <i>z</i> coordinates of a $Antivector4D$ object.
	//# This antivector represents the normal direction to the plane.


	class Antivector4D
	{
		public:

			float	x;		//## The <i>x</i> coordinate.
			float	y;		//## The <i>y</i> coordinate.
			float	z;		//## The <i>z</i> coordinate.
			float	w;		//## The <i>w</i> coordinate.

			Antivector4D() = default;

			Antivector4D(float r, float s, float t, float u)
			{
				x = r;
				y = s;
				z = t;
				w = u;
			}

			Antivector4D(const Antivector3D& n, float u)
			{
				x = n.x;
				y = n.y;
				z = n.z;
				w = u;
			}

			Antivector4D(const Antivector3D& n, const Point3D& p)
			{
				x = n.x;
				y = n.y;
				z = n.z;
				w = -(n * p);
			}

			Antivector4D(const Point3D& p1, const Point3D& p2, const Point3D& p3)
			{
				GetAntivector3D() = (p2 - p1) % (p3 - p1);
				w = -(GetAntivector3D() * p1);
			}

			Antivector4D& Set(float r, float s, float t, float u)
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

			Antivector4D& Set(const Antivector3D& n, float u)
			{
				x = n.x;
				y = n.y;
				z = n.z;
				w = u;
				return (*this);
			}

			void Set(const Antivector3D& n, float u) volatile
			{
				x = n.x;
				y = n.y;
				z = n.z;
				w = u;
			}

			Antivector4D& Set(const Antivector3D& n, const Point3D& p)
			{
				x = n.x;
				y = n.y;
				z = n.z;
				w = -(n * p);
				return (*this);
			}

			void Set(const Antivector3D& n, const Point3D& p) volatile
			{
				x = n.x;
				y = n.y;
				z = n.z;
				w = -(n * p);
			}

			Antivector4D& Set(const Point3D& p1, const Point3D& p2, const Point3D& p3)
			{
				GetAntivector3D() = (p2 - p1) % (p3 - p1);
				w = -(GetAntivector3D() * p1);
				return (*this);
			}

			Antivector4D& SetStandard(const Point3D& p1, const Point3D& p2, const Point3D& p3)
			{
				GetAntivector3D() = C4::Normalize((p2 - p1) % (p3 - p1));
				w = -(GetAntivector3D() * p1);
				return (*this);
			}

			float& operator [](machine k)
			{
				return ((&x)[k]);
			}

			const float& operator [](machine k) const
			{
				return ((&x)[k]);
			}

			Antivector3D& GetAntivector3D(void)
			{
				return (*reinterpret_cast<Antivector3D *>(this));
			}

			const Antivector3D& GetAntivector3D(void) const
			{
				return (*reinterpret_cast<const Antivector3D *>(this));
			}

			Antivector4D& operator =(const Antivector4D& a)
			{
				x = a.x;
				y = a.y;
				z = a.z;
				w = a.w;
				return (*this);
			}

			void operator =(const Antivector4D& a) volatile
			{
				x = a.x;
				y = a.y;
				z = a.z;
				w = a.w;
			}

			Antivector4D& operator *=(float t)
			{
				x *= t;
				y *= t;
				z *= t;
				w *= t;
				return (*this);
			}

			Antivector4D& operator /=(float t)
			{
				float f = 1.0F / t;
				x *= f;
				y *= f;
				z *= f;
				w *= f;
				return (*this);
			}

			Antivector4D& Normalize(void)
			{
				return (*this *= InverseSqrt(x * x + y * y + z * z + w * w));
			}

			Antivector4D& Standardize(void)
			{
				return (*this *= InverseSqrt(x * x + y * y + z * z));
			}
	};


	inline Antivector4D operator -(const Antivector4D& v)
	{
		return (Antivector4D(-v.x, -v.y, -v.z, -v.w));
	}

	inline Antivector4D operator *(const Antivector4D& v, float t)
	{
		return (Antivector4D(v.x * t, v.y * t, v.z * t, v.w * t));
	}

	inline Antivector4D operator *(float t, const Antivector4D& v)
	{
		return (Antivector4D(t * v.x, t * v.y, t * v.z, t * v.w));
	}

	inline Antivector4D operator /(const Antivector4D& v, float t)
	{
		float f = 1.0F / t;
		return (Antivector4D(v.x * f, v.y * f, v.z * f, v.w * f));
	}

	inline float operator ^(const Vector4D& v1, const Antivector4D& v2)
	{
		return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w);
	}

	inline float operator ^(const Antivector4D& v1, const Vector4D& v2)
	{
		return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w);
	}

	inline float operator ^(const Antivector4D& v1, const Vector3D& v2)
	{
		return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
	}

	inline float operator ^(const Antivector4D& v, const Point3D& p)
	{
		return (v.x * p.x + v.y * p.y + v.z * p.z + v.w);
	}

	inline float operator ^(const Antivector4D& v, const Point2D& p)
	{
		return (v.x * p.x + v.y * p.y + v.w);
	}

	inline bool operator ==(const Antivector4D& v1, const Antivector4D& v2)
	{
		return ((v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z) && (v1.w == v2.w));
	}

	inline bool operator !=(const Antivector4D& v1, const Antivector4D& v2)
	{
		return ((v1.x != v2.x) || (v1.y != v2.y) || (v1.z != v2.z) || (v1.w != v2.w));
	}
}


#endif

// ZYUQURM
