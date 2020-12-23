 

#ifndef C4Bivector4D_h
#define C4Bivector4D_h


//# \component	Math Library
//# \prefix		Math/


#include "C4Antivector4D.h"


namespace C4
{
	//# \class	Bivector4D		Encapsulates a 4D bivector.
	//
	//# The $Bivector4D$ class encapsulates a 4D bivector.
	//
	//# \def	class Bivector4D
	//
	//# \data	Bivector4D
	//
	//# \ctor	Bivector4D();
	//# \ctor	Bivector4D(const Point3D& p, const Point3D& q);
	//# \ctor	Bivector4D(const Point3D& p, const Vector3D& v);
	//# \ctor	Bivector4D(const Antivector4D& e, const Antivector4D& f);
	//
	//# \param	p,q		Two 3D points that lie on the line.
	//# \param	v		A 3D vector corresponding to the tangent direction of the line.
	//# \param	e,f		Two planes that intersect at the line.
	//
	//# \desc
	//# The $Bivector4D$ class is used to store a four-dimensional bivector having six floating-point components.
	//#
	//# The default constructor leaves the components of the bivector undefined.
	//#
	//# If points $p$ and $q$ are specified, then the bivector is initialized to the wedge product between homogeneous
	//# extensions of $p$ and $q$ with <i>w</i> coordinates set to 1, giving a representation of the 3D line containing
	//# both points. The tangent component of the bivector is assigned the value $q$&nbsp;&minus;&nbsp;$p$, and the moment
	//# component is assigned the value $p$&nbsp;&and;&nbsp;$q$.
	//#
	//# If the point $p$ and the direction $v$ are specified, then the line contains the point $p$ and runs parallel
	//# to the direction $v$. The bivector is initialized to the wedge product between the homogeneous extension of $p$
	//# with <i>w</i> coordinate set to 1 and the homogeneous extension of $v$ with <i>w</i> coordinate set to 0.
	//# The tangent component of the bivector is set equal to $v$, and the moment component is assigned the value
	//# $p$&nbsp;&and;&nbsp;$v$.
	//#
	//# If planes $e$ and $f$ are specified, then the bivector is initialized to the antiwedge product between the 4D
	//# antivectors $e$ and $f$, giving a representation of the 3D line where the planes intersect.
	//
	//# \operator	Bivector4D& operator *=(float t);
	//#				Multiplies by the scalar $t$.
	//
	//# \operator	Bivector4D& operator /=(float t);
	//#				Multiplies by the inverse of the scalar $t$.
	//
	//# \action		Bivector4D operator -(const Bivector4D& u);
	//#				Returns the negation of the bivector $u$.
	//
	//# \action		Bivector4D operator *(const Bivector4D& u, float t);
	//#				Returns the product of the bivector $u$ and the scalar $t$.
	//
	//# \action		Bivector4D operator *(float t, const Bivector4D& u);
	//#				Returns the product of the bivector $u$ and the scalar $t$.
	//
	//# \action		Bivector4D operator /(const Bivector4D& u, float t);
	//#				Returns the product of the bivector $u$ and the inverse of the scalar $t$.
	//
	//# \action		Bivector4D operator ^(const Point3D& p, const Point3D& q);
	//#				Returns the wedge product of the points $p$ and $q$. The <i>w</i> coordinates of $p$ and $q$ are assumed to be 1.
	//
	//# \action		Bivector4D operator ^(const Antivector4D& e, const Antivector4D& f);
	//#				Returns the antiwedge product of the antivectors $e$ and $f$.
	//
	//# \action		Antivector4D operator ^(const Bivector4D& u, const Point3D& p);
	//#				Returns the wedge product of the bivector $u$ and the point $p$. The <i>w</i> coordinate of $p$ is assumed to be 1.
	//#				(The result represents the plane containing the line $u$ and the point $p$, wound from the tangent component of $u$ toward the direction to $p$ perpendicular to the line $u$.)
	//
	//# \action		Antivector4D operator ^(const Point3D& p, const Bivector4D& u);
	//#				Returns the wedge product of the point $p$ and the bivector $u$. The <i>w</i> coordinate of $p$ is assumed to be 1.
	//#				(The result represents the plane containing the line $u$ and the point $p$, wound from the direction to $p$ perpendicular to the line $u$ toward the tangent component of $u$.)
	//
	//# \action		Antivector4D operator ^(const Bivector4D& u, const Vector3D& v);
	//#				Returns the wedge product of the bivector $u$ and the direction $v$. The <i>w</i> coordinate of $v$ is assumed to be 0.
	//#				(The result represents the plane containing the line $u$ and the direction $v$, wound from the tangent component of $u$ toward the direction $v$.)
	//
	//# \action		Antivector4D operator ^(const Vector3D& v, const Bivector4D& u);
	//#				Returns the wedge product of the direction $v$ and the bivector $u$. The <i>w</i> coordinate of $v$ is assumed to be 0.
	//#				(The result represents the plane containing the line $u$ and the direction $v$, wound from the direction $v$ toward the tangent component of $u$.)
	//
	//# \action		Vector4D operator ^(const Bivector4D& u, const Antivector4D& e);
	//#				Returns the antiwedge product of the bivector $u$ and the plane $e$.
	//#				(The result represents the homogeneous point where the plane and plane intersect. The <i>x</i>, <i>y</i>, and <i>z</i> must be divided by the <i>w</i> coordinate to produce a 3D point.)
	//
	//# \action		Vector4D operator ^(const Antivector4D& e, const Bivector4D& u);
	//#				Returns the antiwedge product of the plane $e$ and the bivector $u$.
	//#				(The result represents the homogeneous point where the plane and plane intersect. The <i>x</i>, <i>y</i>, and <i>z</i> must be divided by the <i>w</i> coordinate to produce a 3D point.)
	//
	//# \action		float operator ^(const Bivector4D& u1, const Bivector4D& u2);
	//#				Returns the negative antiwedge product of the bivectors $u1$ and $u2$. (The negative value is returned for greater efficiency.)
	//#				(This gives the crossing relationship between the two lines, with positive values representing counterclockwise crossings and negative values representing clockwise crossings.)
	// 
	//# \action		bool operator ==(const Bivector4D& u1, const Bivector4D& u2);
	//#				Returns a boolean value indicating the equality of the two bivectors $u1$ and $u2$.
	// 
	//# \action		bool operator !=(const Bivector4D& u1, const Bivector4D& u2);
	//#				Returns a boolean value indicating the inequality of the two bivectors $u1$ and $u2$ 
	//
	//# \also	$@Vector4D@$
	//# \also	$@Antivector4D@$ 

 
	//# \function	Bivector4D::Set		Sets all six components of a bivector. 
	//
	//# \proto	Bivector4D& Set(const Point3D& p, const Point3D& q);
	//# \proto	Bivector4D& Set(const Point3D& p, const Vector3D& v);
	//# \proto	Bivector4D& Set(const Antivector4D& e, const Antivector4D& f); 
	//
	//# \param	p,q		Two 3D points that lie on the line.
	//# \param	v		A 3D vector corresponding to the tangent direction of the line.
	//# \param	e,f		Two planes that intersect at the line.
	//
	//# \desc
	//# The $Set$ function replaces all six components of a bivector with new values.
	//#
	//# If points $p$ and $q$ are specified, then the bivector is initialized to the wedge product between homogeneous
	//# extensions of $p$ and $q$ with <i>w</i> coordinates set to 1, giving a representation of the 3D line containing
	//# both points. The tangent component of the bivector is assigned the value $q$&nbsp;&minus;&nbsp;$p$, and the moment
	//# component is assigned the value $p$&nbsp;&and;&nbsp;$q$.
	//#
	//# If the point $p$ and the direction $v$ are specified, then the line contains the point $p$ and runs parallel
	//# to the direction $v$. The bivector is initialized to the wedge product between the homogeneous extension of $p$
	//# with <i>w</i> coordinate set to 1 and the homogeneous extension of $v$ with <i>w</i> coordinate set to 0.
	//# The tangent component of the bivector is set equal to $v$, and the moment component is assigned the value
	//# $p$&nbsp;&and;&nbsp;$v$.
	//#
	//# If planes $e$ and $f$ are specified, then the bivector is initialized to the antiwedge product between the 4D
	//# antivectors $e$ and $f$, giving a representation of the 3D line where the planes intersect.
	//#
	//# The return value is a reference to the bivector object.


	//# \function	Bivector4D::GetTangent		Returns a reference to the tangent component of a 4D bivector.
	//
	//# \proto	const Vector3D& GetTangent(void) const;
	//
	//# \desc
	//# The $GetTangent$ function returns a reference to the 3D tangent component of a 4D bivector.
	//
	//# \also	$@Bivector4D::GetMoment@$


	//# \function	Bivector4D::GetMoment		Returns a reference to the tangent component of a 4D bivector.
	//
	//# \proto	const Antivector3D& GetMoment(void) const;
	//
	//# \desc
	//# The $GetMoment$ function returns a reference to the 3D moment component of a 4D bivector.
	//
	//# \also	$@Bivector4D::GetTangent@$


	//# \function	Bivector4D::Standardize		Standardizes a 4D bivector.
	//
	//# \proto	Bivector4D& Standardize(void);
	//
	//# \desc
	//# The $Standardize$ function multiplies a 4D bivector by the inverse magnitude of the 3D tangent component.
	//# After calling this function, the tangent component has unit length, and the magnitude of the moment component
	//# is the perpendicular distance between the origin and the line.
	//#
	//# The return value is a reference to the bivector object.


	class Bivector4D
	{
		private:

			Vector3D		tangent;
			Antivector3D	moment;

			Bivector4D(float tx, float ty, float tz, float mx, float my, float mz)
			{
				tangent.Set(tx, ty, tz);
				moment.Set(mx, my, mz);
			}

			Bivector4D(const Vector3D& t, const Vector3D& m)
			{
				tangent = t;
				moment = m;
			}

		public:

			Bivector4D() = default;

			Bivector4D(const Point3D& p, const Point3D& q)
			{
				tangent.Set(q.x - p.x, q.y - p.y, q.z - p.z);
				moment.Set(p.y * q.z - q.y * p.z, p.z * q.x - q.z * p.x, p.x * q.y - q.x * p.y);
			}

			Bivector4D(const Point3D& p, const Vector3D& v)
			{
				tangent = v;
				moment.Set(p.y * v.z - v.y * p.z, p.z * v.x - v.z * p.x, p.x * v.y - v.x * p.y);
			}

			Bivector4D(const Antivector4D& e, const Antivector4D& f)
			{
				tangent.Set(e.z * f.y - f.z * e.y, e.x * f.z - f.x * e.z, e.y * f.x - f.y * e.x);
				moment.Set(e.x * f.w - f.x * e.w, e.y * f.w - f.y * e.w, e.z * f.w - f.z * e.w);
			}

			Bivector4D& Set(const Point3D& p, const Point3D& q)
			{
				tangent.Set(q.x - p.x, q.y - p.y, q.z - p.z);
				moment.Set(p.y * q.z - q.y * p.z, p.z * q.x - q.z * p.x, p.x * q.y - q.x * p.y);
				return (*this);
			}

			void Set(const Point3D& p, const Point3D& q) volatile
			{
				tangent.Set(q.x - p.x, q.y - p.y, q.z - p.z);
				moment.Set(p.y * q.z - q.y * p.z, p.z * q.x - q.z * p.x, p.x * q.y - q.x * p.y);
			}

			Bivector4D& Set(const Point3D& p, const Vector3D& v)
			{
				tangent = v;
				moment.Set(p.y * v.z - v.y * p.z, p.z * v.x - v.z * p.x, p.x * v.y - v.x * p.y);
				return (*this);
			}

			void Set(const Point3D& p, const Vector3D& v) volatile
			{
				tangent = v;
				moment.Set(p.y * v.z - v.y * p.z, p.z * v.x - v.z * p.x, p.x * v.y - v.x * p.y);
			}

			Bivector4D& Set(const Antivector4D& e, const Antivector4D& f)
			{
				tangent.Set(e.z * f.y - f.z * e.y, e.x * f.z - f.x * e.z, e.y * f.x - f.y * e.x);
				moment.Set(e.x * f.w - f.x * e.w, e.y * f.w - f.y * e.w, e.z * f.w - f.z * e.w);
				return (*this);
			}

			void Set(const Antivector4D& e, const Antivector4D& f) volatile
			{
				tangent.Set(e.z * f.y - f.z * e.y, e.x * f.z - f.x * e.z, e.y * f.x - f.y * e.x);
				moment.Set(e.x * f.w - f.x * e.w, e.y * f.w - f.y * e.w, e.z * f.w - f.z * e.w);
			}

			const Vector3D& GetTangent(void) const
			{
				return (tangent);
			}

			const Antivector3D& GetMoment(void) const
			{
				return (moment);
			}

			const Vector3D GetSupport(void) const
			{
				return (tangent % moment);
			}

			Bivector4D& operator =(const Bivector4D& b)
			{
				tangent = b.tangent;
				moment = b.moment;
				return (*this);
			}

			void operator =(const Bivector4D& b) volatile
			{
				tangent = b.tangent;
				moment = b.moment;
			}

			Bivector4D& operator *=(float t)
			{
				tangent *= t;
				moment *= t;
				return (*this);
			}

			Bivector4D& operator /=(float t)
			{
				float f = 1.0F / t;
				tangent *= f;
				moment *= f;
				return (*this);
			}

			Bivector4D& Standardize(void)
			{
				return (*this *= InverseMag(tangent));
			}

			friend Bivector4D operator -(const Bivector4D& u);
			friend Bivector4D operator *(const Bivector4D& u, float t);
			friend Bivector4D operator *(float t, const Bivector4D& u);
			friend Bivector4D operator /(const Bivector4D& u, float t);
			friend Bivector4D operator ^(const Point3D& p, const Point3D& q);
			friend Bivector4D operator ^(const Antivector4D& e, const Antivector4D& f);
			friend Antivector4D operator ^(const Bivector4D& u, const Point3D& p);
			friend Antivector4D operator ^(const Point3D& p, const Bivector4D& u);
			friend Antivector4D operator ^(const Bivector4D& u, const Vector3D& v);
			friend Antivector4D operator ^(const Vector3D& v, const Bivector4D& u);
			friend Vector4D operator ^(const Bivector4D& u, const Antivector4D& e);
			friend Vector4D operator ^(const Antivector4D& e, const Bivector4D& u);
			friend float operator ^(const Bivector4D& u1, const Bivector4D& u2);
			friend bool operator ==(const Bivector4D& u1, const Bivector4D& u2);
			friend bool operator !=(const Bivector4D& u1, const Bivector4D& u2);

			friend Bivector4D Translate(const Bivector4D& u, const Vector3D& v);
	};


	inline Bivector4D operator -(const Bivector4D& u)
	{
		return (Bivector4D(-u.tangent.x, -u.tangent.y, -u.tangent.z, -u.moment.x, -u.moment.y, -u.moment.z));
	}

	inline Bivector4D operator *(const Bivector4D& u, float t)
	{
		return (Bivector4D(u.tangent.x * t, u.tangent.y * t, u.tangent.z * t, u.moment.x * t, u.moment.y * t, u.moment.z * t));
	}

	inline Bivector4D operator *(float t, const Bivector4D& u)
	{
		return (Bivector4D(t * u.tangent.x, t * u.tangent.y, t * u.tangent.z, t * u.moment.x, t * u.moment.y, t * u.moment.z));
	}

	inline Bivector4D operator /(const Bivector4D& u, float t)
	{
		float f = 1.0F / t;
		return (Bivector4D(u.tangent.x * f, u.tangent.y * f, u.tangent.z * f, u.moment.x * f, u.moment.y * f, u.moment.z * f));
	}

	inline Bivector4D operator ^(const Point3D& p, const Point3D& q)
	{
		return (Bivector4D(q.x - p.x, q.y - p.y, q.z - p.z, p.y * q.z - q.y * p.z, p.z * q.x - q.z * p.x, p.x * q.y - q.x * p.y));
	}

	inline Bivector4D operator ^(const Antivector4D& e, const Antivector4D& f)
	{
		return (Bivector4D(e.z * f.y - f.z * e.y, e.x * f.z - f.x * e.z, e.y * f.x - f.y * e.x, e.x * f.w - f.x * e.w, e.y * f.w - f.y * e.w, e.z * f.w - f.z * e.w));
	}

	inline Antivector4D operator ^(const Bivector4D& u, const Point3D& p)
	{
		return (Antivector4D(u.tangent.y * p.z - u.tangent.z * p.y + u.moment.x,
							 u.tangent.z * p.x - u.tangent.x * p.z + u.moment.y,
							 u.tangent.x * p.y - u.tangent.y * p.x + u.moment.z,
							 -p.x * u.moment.x - p.y * u.moment.y - p.z * u.moment.z));
	}

	inline Antivector4D operator ^(const Point3D& p, const Bivector4D& u)
	{
		return (Antivector4D(u.tangent.z * p.y - u.tangent.y * p.z - u.moment.x,
							 u.tangent.x * p.z - u.tangent.z * p.x - u.moment.y,
							 u.tangent.y * p.x - u.tangent.x * p.y - u.moment.z,
							 p.x * u.moment.x + p.y * u.moment.y + p.z * u.moment.z));
	}

	inline Antivector4D operator ^(const Bivector4D& u, const Vector3D& v)
	{
		return (Antivector4D(u.tangent.y * v.z - u.tangent.z * v.y,
							 u.tangent.z * v.x - u.tangent.x * v.z,
							 u.tangent.x * v.y - u.tangent.y * v.x,
							 -v.x * u.moment.x - v.y * u.moment.y - v.z * u.moment.z));
	}

	inline Antivector4D operator ^(const Vector3D& v, const Bivector4D& u)
	{
		return (Antivector4D(u.tangent.z * v.y - u.tangent.y * v.z,
							 u.tangent.x * v.z - u.tangent.z * v.x,
							 u.tangent.y * v.x - u.tangent.x * v.y,
							 v.x * u.moment.x + v.y * u.moment.y + v.z * u.moment.z));
	}

	inline Vector4D operator ^(const Bivector4D& u, const Antivector4D& e)
	{
		return (Vector4D(u.moment.z * e.y - u.moment.y * e.z - u.tangent.x * e.w,
						 u.moment.x * e.z - u.moment.z * e.x - u.tangent.y * e.w,
						 u.moment.y * e.x - u.moment.x * e.y - u.tangent.z * e.w,
						 e.x * u.tangent.x + e.y * u.tangent.y + e.z * u.tangent.z));
	}

	inline Vector4D operator ^(const Antivector4D& e, const Bivector4D& u)
	{
		return (Vector4D(u.moment.y * e.z - u.moment.z * e.y + u.tangent.x * e.w,
						 u.moment.z * e.x - u.moment.x * e.z + u.tangent.y * e.w,
						 u.moment.x * e.y - u.moment.y * e.x + u.tangent.z * e.w,
						 -e.x * u.tangent.x - e.y * u.tangent.y - e.z * u.tangent.z));
	}

	inline float operator ^(const Bivector4D& u1, const Bivector4D& u2)
	{
		return (u1.tangent * u2.moment + u1.moment * u2.tangent);
	}

	inline bool operator ==(const Bivector4D& u1, const Bivector4D& u2)
	{
		return ((u1.tangent == u2.tangent) && (u1.moment == u2.moment));
	}

	inline bool operator !=(const Bivector4D& u1, const Bivector4D& u2)
	{
		return ((u1.tangent != u2.tangent) || (u1.moment != u2.moment));
	}

	inline Bivector4D Translate(const Bivector4D& u, const Vector3D& v)
	{
		return (Bivector4D(u.tangent, u.moment + v % u.tangent));
	}
}


#endif

// ZYUQURM
