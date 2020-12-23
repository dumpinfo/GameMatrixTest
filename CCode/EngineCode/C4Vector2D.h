 

#ifndef C4Vector2D_h
#define C4Vector2D_h


//# \component	Math Library
//# \prefix		Math/


#include "C4Math.h"


namespace C4
{
	//# \class	Vector2D	Encapsulates a 2D vector.
	//
	//# The $Vector2D$ class encapsulates a 2D vector.
	//
	//# \def	class Vector2D
	//
	//# \data	Vector2D
	//
	//# \ctor	Vector2D();
	//# \ctor	Vector2D(float r, float s);
	//
	//# \param	r	The value of the <i>x</i> coordinate.
	//# \param	s	The value of the <i>y</i> coordinate.
	//
	//# \desc
	//# The $Vector2D$ class is used to store a two-dimensional direction vector
	//# having floating-point components <i>x</i> and <i>y</i>. A direction vector
	//# stored in this class is assumed to have a <i>w</i> coordinate of 0 whenever it needs
	//# to be converted to a four-dimensional representation. Two-dimensional points
	//# (for which the <i>w</i> coordinate is 1) should be stored using the $@Point2D@$ class.
	//# The <i>z</i> coordinate of a 2D vector is always assumed to be 0.
	//#
	//# The default constructor leaves the components of the vector undefined.
	//# If the values $r$ and $s$ are supplied, then they are assigned to the
	//# <i>x</i> and <i>y</i> coordinates of the vector, respectively.
	//
	//# \operator	float& operator [](machine k);
	//#				Returns a reference to the $k$-th component of a vector.
	//#				The value of $k$ must be 0 or 1.
	//
	//# \operator	const float& operator [](machine k) const;
	//#				Returns a constant reference to the $k$-th component of a vector.
	//#				The value of $k$ must be 0 or 1.
	//
	//# \operator	Vector2D& operator +=(const Vector2D& v);
	//#				Adds the vector $v$.
	//
	//# \operator	Vector2D& operator -=(const Vector2D& v);
	//#				Subtracts the vector $v$.
	//
	//# \operator	Vector2D& operator *=(float t);
	//#				Multiplies by the scalar $t$.
	//
	//# \operator	Vector2D& operator /=(float t);
	//#				Divides by the scalar $t$.
	//
	//# \operator	Vector2D& operator &=(const Vector2D& v);
	//#				Calculates the componentwise product with the vector $v$.
	//
	//# \action		Vector2D operator -(const Vector2D& v) const;
	//#				Returns the negation of the vector $v$.
	//
	//# \action		Vector2D operator +(const Vector2D& v1, const Vector2D& v2) const;
	//#				Returns the sum of the vectors $v1$ and $v2$.
	//
	//# \action		Vector2D operator -(const Vector2D& v1, const Vector2D& v2) const;
	//#				Returns the difference of the vectors $v1$ and $v2$.
	//
	//# \action		Vector2D operator *(const Vector2D& v, float t) const;
	//#				Returns the product of the vector $v$ and the scalar $t$.
	//
	//# \action		Vector2D operator *(float t, const Vector2D& v);
	//#				Returns the product of the vector $v$ and the scalar $t$.
	//
	//# \action		Vector2D operator /(const Vector2D& v, float t) const;
	//#				Returns the product of the vector $v$ and the inverse of the scalar $t$.
	//
	//# \action		float operator *(const Vector2D& v1, const Vector2D& v2) const;
	//#				Returns the dot product of the vectors $v1$ and $v2$.
	//
	//# \action		Vector2D operator &(const Vector2D& v1, const Vector2D& v2) const;
	//#				Returns the componentwise product of the vectors $v1$ and $v2$.
	//
	//# \action		bool operator ==(const Vector2D& v1, const Vector2D& v2) const;
	//#				Returns a boolean value indicating the equality of the two vectors $v1$ and $v2$.
	//
	//# \action		bool operator !=(const Vector2D& v1, const Vector2D& v2) const;
	//#				Returns a boolean value indicating the inequality of the two vectors $v1$ and $v2$.
	//
	//# \action		float Dot(const Vector2D& v1, const Vector2D& v2);
	//#				Returns the dot product between $v1$ and $v2$.
	//
	//# \action		Vector2D ProjectOnto(const Vector2D& v1, const Vector2D& v2);
	//#				Returns the projection of $v1$ onto $v2$ scaled by the squared magnitude of $v2$.
	//
	//# \action		float Magnitude(const Vector2D& v);
	//#				Returns the magnitude of the vector $v$.
	// 
	//# \action		float InverseMag(const Vector2D& v);
	//#				Returns the inverse magnitude of the vector $v$.
	// 
	//# \action		float SquaredMag(const Vector2D& v);
	//#				Returns the squared magnitude of the vector $v$. 
	//
	//# \also	$@Point2D@$
	//# \also	$@Vector4D@$ 

 
	//# \function	Vector2D::Set		Sets both components of a vector. 
	//
	//# \proto	Vector2D& Set(float r, float s);
	//
	//# \param	r	The new <i>x</i> coordinate. 
	//# \param	s	The new <i>y</i> coordinate.
	//
	//# \desc
	//# The $Set$ function sets the <i>x</i> and <i>y</i> coordinates of a vector to
	//# the values given by the $r$ and $s$ parameters, respectively.
	//#
	//# The return value is a reference to the vector object.


	//# \function	Vector2D::Normalize		Normalizes a 2D vector.
	//
	//# \proto	Vector2D& Normalize(void);
	//
	//# \desc
	//# The $Normalize$ function multiplies a 2D vector by the inverse of its magnitude,
	//# normalizing it to unit length. Normalizing the zero vector produces undefined results.
	//#
	//# The return value is a reference to the vector object.


	//# \function	Vector2D::Rotate		Rotates a vector in the <i>x</i>-<i>y</i> plane.
	//
	//# \proto	Vector2D& Rotate(float angle);
	//
	//# \param	angle	The angle through which the vector is rotated, in radians.
	//
	//# \desc
	//# The $Rotate$ function rotates a vector in the <i>x</i>-<i>y</i> plane through the
	//# angle given by the $angle$ parameter.
	//#
	//# The return value is a reference to the vector object.


	//# \member		Vector2D

	class Vector2D
	{
		public:

			float	x;		//## The <i>x</i> coordinate.
			float	y;		//## The <i>y</i> coordinate.

			Vector2D() = default;

			Vector2D(float r, float s)
			{
				x = r;
				y = s;
			}

			Vector2D& Set(float r, float s)
			{
				x = r;
				y = s;
				return (*this);
			}

			void Set(float r, float s) volatile
			{
				x = r;
				y = s;
			}

			float& operator [](machine k)
			{
				return ((&x)[k]);
			}

			const float& operator [](machine k) const
			{
				return ((&x)[k]);
			}

			Vector2D& operator =(const Vector2D& v)
			{
				x = v.x;
				y = v.y;
				return (*this);
			}

			void operator =(const Vector2D& v) volatile
			{
				x = v.x;
				y = v.y;
			}

			Vector2D& operator +=(const Vector2D& v)
			{
				x += v.x;
				y += v.y;
				return (*this);
			}

			Vector2D& operator -=(const Vector2D& v)
			{
				x -= v.x;
				y -= v.y;
				return (*this);
			}

			Vector2D& operator *=(float t)
			{
				x *= t;
				y *= t;
				return (*this);
			}

			Vector2D& operator /=(float t)
			{
				float f = 1.0F / t;
				x *= f;
				y *= f;
				return (*this);
			}

			Vector2D& operator &=(const Vector2D& v)
			{
				x *= v.x;
				y *= v.y;
				return (*this);
			}

			Vector2D& Normalize(void)
			{
				return (*this *= InverseSqrt(x * x + y * y));
			}

			C4API Vector2D& Rotate(float angle);
	};


	inline Vector2D operator -(const Vector2D& v)
	{
		return (Vector2D(-v.x, -v.y));
	}

	inline Vector2D operator +(const Vector2D& v1, const Vector2D& v2)
	{
		return (Vector2D(v1.x + v2.x, v1.y + v2.y));
	}

	inline Vector2D operator -(const Vector2D& v1, const Vector2D& v2)
	{
		return (Vector2D(v1.x - v2.x, v1.y - v2.y));
	}

	inline Vector2D operator *(const Vector2D& v, float t)
	{
		return (Vector2D(v.x * t, v.y * t));
	}

	inline Vector2D operator *(float t, const Vector2D& v)
	{
		return (Vector2D(t * v.x, t * v.y));
	}

	inline Vector2D operator /(const Vector2D& v, float t)
	{
		float f = 1.0F / t;
		return (Vector2D(v.x * f, v.y * f));
	}

	inline float operator *(const Vector2D& v1, const Vector2D& v2)
	{
		return (v1.x * v2.x + v1.y * v2.y);
	}

	inline Vector2D operator &(const Vector2D& v1, const Vector2D& v2)
	{
		return (Vector2D(v1.x * v2.x, v1.y * v2.y));
	}

	inline bool operator ==(const Vector2D& v1, const Vector2D& v2)
	{
		return ((v1.x == v2.x) && (v1.y == v2.y));
	}

	inline bool operator !=(const Vector2D& v1, const Vector2D& v2)
	{
		return ((v1.x != v2.x) || (v1.y != v2.y));
	}


	//# \class	Point2D		Encapsulates a 2D point.
	//
	//# The $Point2D$ class encapsulates a 2D point.
	//
	//# \def	class Point2D : public Vector2D
	//
	//# \ctor	Point2D();
	//# \ctor	Point2D(float r, float s);
	//
	//# \param	r	The value of the <i>x</i> coordinate.
	//# \param	s	The value of the <i>y</i> coordinate.
	//
	//# \desc
	//# The $Point2D$ class is used to store a two-dimensional point having floating-point
	//# coordinates <i>x</i> and <i>y</i>. The difference between a point and a
	//# vector is that a point is assumed to have a <i>w</i> coordinate of 1 whenever it
	//# needs to be converted to a four-dimensional representation, whereas a vector is
	//# assumed to have a <i>w</i> coordinate of 0. Such a conversion occurs when a vector or
	//# point is assigned to a $@Vector4D@$ object or is multiplied by a $@Transform4D@$ object.
	//#
	//# The default constructor leaves the components of the vector undefined.
	//# If the values $r$ and $s$ are supplied, then they are assigned to the
	//# <i>x</i> and <i>y</i> coordinates of the base vector object, respectively.
	//#
	//# The difference between two points produces a direction vector. A two-dimensional
	//# direction vector is converted to a point by adding it to the identifier $Zero2D$.
	//
	//# \operator	Point2D& operator *=(float t);
	//#				Multiplies by the scalar $t$.
	//
	//# \operator	Point2D& operator /=(float t);
	//#				Divides by the scalar $t$.
	//
	//# \action		Point2D operator -(const Point2D& p);
	//#				Returns the negation of the point $p$.
	//
	//# \action		Point2D operator +(const Point2D& p1, const Point2D& p2);
	//#				Returns the sum of the points $p1$ and $p2$.
	//
	//# \action		Point2D operator +(const Point2D& p, const Vector2D& v);
	//#				Returns the sum of the point $p$ and the vector $v$.
	//
	//# \action		Point2D operator -(const Point2D& p, const Vector2D& v);
	//#				Returns the difference of the point $p$ and the vector $v$.
	//
	//# \action		Vector2D operator -(const Point2D& p1, const Point2D& p2);
	//#				Returns the difference of the points $p1$ and $p2$.
	//
	//# \action		Point2D operator *(const Point2D& p, float t);
	//#				Returns the product of the point $p$ and the scalar $t$.
	//
	//# \action		Point2D operator *(float t, const Point2D& p);
	//#				Returns the product of the point $p$ and the scalar $t$.
	//
	//# \action		Point2D operator /(const Point2D& p, float t) const;
	//#				Returns the product of the point $p$ and the inverse of the scalar $t$.
	//
	//# \base	Vector2D	A $Point2D$ object behaves much like a $@Vector2D@$ object, but some
	//#						properties are altered.
	//
	//# \also	$@Vector2D@$
	//# \also	$@Vector4D@$


	class Point2D : public Vector2D
	{
		public:

			Point2D() = default;

			Point2D(float r, float s) : Vector2D(r, s) {}

			Vector2D& GetVector2D(void)
			{
				return (*this);
			}

			const Vector2D& GetVector2D(void) const
			{
				return (*this);
			}

			Point2D& operator =(const Vector2D& v)
			{
				x = v.x;
				y = v.y;
				return (*this);
			}

			void operator =(const Vector2D& v) volatile
			{
				x = v.x;
				y = v.y;
			}

			Point2D& operator *=(float t)
			{
				x *= t;
				y *= t;
				return (*this);
			}

			Point2D& operator /=(float t)
			{
				float f = 1.0F / t;
				x *= f;
				y *= f;
				return (*this);
			}
	};


	inline Point2D operator -(const Point2D& p)
	{
		return (Point2D(-p.x, -p.y));
	}

	inline Point2D operator +(const Point2D& p1, const Point2D& p2)
	{
		return (Point2D(p1.x + p2.x, p1.y + p2.y));
	}

	inline Point2D operator +(const Point2D& p, const Vector2D& v)
	{
		return (Point2D(p.x + v.x, p.y + v.y));
	}

	inline Point2D operator -(const Point2D& p, const Vector2D& v)
	{
		return (Point2D(p.x - v.x, p.y - v.y));
	}

	inline Vector2D operator -(const Point2D& p1, const Point2D& p2)
	{
		return (Vector2D(p1.x - p2.x, p1.y - p2.y));
	}

	inline Point2D operator *(const Point2D& p, float t)
	{
		return (Point2D(p.x * t, p.y * t));
	}

	inline Point2D operator *(float t, const Point2D& p)
	{
		return (Point2D(t * p.x, t * p.y));
	}

	inline Point2D operator /(const Point2D& p, float t)
	{
		float f = 1.0F / t;
		return (Point2D(p.x * f, p.y * f));
	}


	inline float Dot(const Vector2D& v1, const Vector2D& v2)
	{
		return (v1 * v2);
	}

	inline Vector2D ProjectOnto(const Vector2D& v1, const Vector2D& v2)
	{
		return (v2 * (v1 * v2));
	}

	inline float Magnitude(const Vector2D& v)
	{
		return (Sqrt(v.x * v.x + v.y * v.y));
	}

	inline float InverseMag(const Vector2D& v)
	{
		return (InverseSqrt(v.x * v.x + v.y * v.y));
	}

	inline float SquaredMag(const Vector2D& v)
	{
		return (v.x * v.x + v.y * v.y);
	}

	inline Vector2D Normalize(const Vector2D& v)
	{
		return (v * InverseSqrt(v.x * v.x + v.y * v.y));
	}

	inline unsigned_int32 Hash(const Vector2D& v)
	{
		return (Math::Hash(2, &v.x));
	}


	typedef Vector2D Antivector2D;


	inline float operator ^(const Vector2D& v1, const Vector2D& v2)
	{
		return (v1.x * v2.y - v1.y * v2.x);
	}


	struct ConstVector2D
	{
		float	x;
		float	y;

		operator const Vector2D&(void) const
		{
			return (reinterpret_cast<const Vector2D&>(*this));
		}

		const Vector2D *operator &(void) const
		{
			return (reinterpret_cast<const Vector2D *>(this));
		}

		const Vector2D *operator ->(void) const
		{
			return (reinterpret_cast<const Vector2D *>(this));
		}
	};


	struct ConstPoint2D
	{
		float	x;
		float	y;

		operator const Point2D&(void) const
		{
			return (reinterpret_cast<const Point2D&>(*this));
		}

		const Point2D *operator &(void) const
		{
			return (reinterpret_cast<const Point2D *>(this));
		}

		const Point2D *operator ->(void) const
		{
			return (reinterpret_cast<const Point2D *>(this));
		}
	};


	class Zero2DType
	{
		private:

			C4API static ConstPoint2D zero;

		public:

			operator const Vector2D&(void) const
			{
				return (zero);
			}

			operator const Point2D&(void) const
			{
				return (zero);
			}
	};


	inline const Point2D& operator +(const Zero2DType&, const Vector2D& v)
	{
		return (static_cast<const Point2D&>(v));
	}

	inline Point2D operator -(const Zero2DType&, const Vector2D& v)
	{
		return (Point2D(-v.x, -v.y));
	}


	C4API extern const Zero2DType Zero2D;
}


#endif

// ZYUQURM
