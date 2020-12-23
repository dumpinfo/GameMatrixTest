 

#ifndef C4Quaternion_h
#define C4Quaternion_h


//# \component	Math Library
//# \prefix		Math/


#include "C4Matrix4D.h"


namespace C4
{
	//# \class	Quaternion		Encapsulates a quaternion.
	//
	//# The $Quaternion$ class encapsulates a quaternion.
	//
	//# \def	class Quaternion
	//
	//# \data	Quaternion
	//
	//# \ctor	Quaternion();
	//# \ctor	Quaternion(float a, float b, float c, float s);
	//# \ctor	explicit Quaternion(const Vector3D& v);
	//# \ctor	Quaternion(const Vector3D& v, float s);
	//# \ctor	explicit Quaternion(float s);
	//
	//# \desc
	//# The $Quaternion$ class encapsulates a Hamiltonian quaternion having the form
	//# <i>xi</i>&nbsp;+&nbsp;<i>yj</i>&nbsp;+&nbsp;<i>zk</i>&nbsp;+&nbsp;<i>w</i>.
	//#
	//# The default constructor leaves the components of the quaternion undefined.
	//# If the values $a$, $b$, $c$, and $s$ are supplied, then they are assigned to the
	//# <i>x</i>, <i>y</i>, <i>z</i>, and <i>w</i> coordinates of the quaternion, respectively.
	//# The scalar $s$ is always assigned to the real component <i>w</i>, and the components of $v$
	//# are assigned to the imaginary components <i>x</i>, <i>y</i>, and <i>z</i> of the quaternion.
	//# If a quaternion is constructed with only the $@Vector3D@$ object $v$, then the real
	//# coordinate <i>w</i> is set to 0. If a quaternion is constructed with only the scalar $s$,
	//# then the imaginary components <i>x</i>, <i>y</i>, and <i>z</i> are set to 0.
	//#
	//# When performing arithmetic with quaternions, 3D vectors are always treated as quaternions
	//# with <i>w</i>&nbsp;=&nbsp;0, and scalars are always treated as quaternions with
	//# <i>x</i>,<i>y</i>,<i>z</i>&nbsp;=&nbsp;0.
	//
	//# \operator	float& operator [](machine k);
	//#				Returns a reference to the $k$-th component of a quaternion.
	//#				The value of $k$ must be 0, 1, 2, or 3.
	//
	//# \operator	const float& operator [](machine k) const;
	//#				Returns a constant reference to the $k$-th component of a quaternion.
	//#				The value of $k$ must be 0, 1, 2, or 3.
	//
	//# \operator	Quaternion& operator =(const Vector3D& v);
	//#				Assigns the of $v$ to the imaginary components <i>x</i>, <i>y</i>,
	//#				and <i>z</i> of the quaternion. The real component <i>w</i> is set to 0.
	//
	//# \operator	Quaternion& operator =(float s);
	//#				Sets the real component <i>w</i> of the quaternion to $s$, and sets the
	//#				imaginary components <i>x</i>, <i>y</i>, and <i>z</i> to 0.
	//
	//# \operator	Quaternion& operator +=(const Quaternion& q);
	//#				Adds the quaternion $q$.
	//
	//# \operator	Quaternion& operator +=(const Vector3D& v);
	//#				Adds the vector $v$.
	//
	//# \operator	Quaternion& operator +=(float s);
	//#				Adds the scalar $s$.
	//
	//# \operator	Quaternion& operator -=(const Quaternion& q);
	//#				Subtracts the quaternion $q$.
	//
	//# \operator	Quaternion& operator -=(const Vector3D& v);
	//#				Subtracts the vector $v$.
	//
	//# \operator	Quaternion& operator -=(float s);
	//#				Subtracts the scalar $s$.
	//
	//# \operator	Quaternion& operator *=(const Quaternion& q);
	//#				Multiplies by the quaternion $q$.
	//
	//# \operator	Quaternion& operator *=(const Vector3D& v);
	//#				Multiplies by the vector $v$.
	//
	//# \operator	Quaternion& operator *=(float s);
	//#				Multiplies by the scalar $s$.
	//
	//# \operator	Quaternion& operator /=(const Quaternion& q);
	//#				Multiplies by the inverse of the quaternion $q$.
	//
	//# \operator	Quaternion& operator /=(const Vector3D& v);
	//#				Multiplies by the inverse of the vector $v$.
	//
	//# \operator	Quaternion& operator /=(float s);
	//#				Multiplies by the inverse of the scalar $s$.
	//
	//# \action		Quaternion operator -(const Quaternion& q);
	//#				Returns the negation of the quaternion $q$.
	//
	//# \action		Quaternion operator +(const Quaternion& q1, const Quaternion& q2);
	//#				Returns the sum of the quaternions $q1$ and $q2$. 
	//
	//# \action		Quaternion operator +(const Quaternion& q, const Vector3D& v);
	//#				Returns the sum of the quaternion $q$ and the vector $v$. 
	//
	//# \action		Quaternion operator +(const Vector3D& v, const Quaternion& q); 
	//#				Returns the sum of the vector $v$ and the quaternion $q$.
	//
	//# \action		Quaternion operator +(const Quaternion& q, float s); 
	//#				Returns the sum of the quaternion $q$ and the scalar $s$.
	// 
	//# \action		Quaternion operator +(float s, const Quaternion& q); 
	//#				Returns the sum of the scalar $s$ and the quaternion $q$.
	//
	//# \action		Quaternion operator -(const Quaternion& q1, const Quaternion& q2);
	//#				Returns the difference of the quaternions $q1$ and $q2$. 
	//
	//# \action		Quaternion operator -(const Quaternion& q, const Vector3D& v);
	//#				Returns the difference of the quaternion $q$ and the vector $v$.
	//
	//# \action		Quaternion operator -(const Vector3D& v, const Quaternion& q);
	//#				Returns the difference of the vector $v$ and the quaternion $q$.
	//
	//# \action		Quaternion operator -(const Quaternion& q, float s);
	//#				Returns the difference of the quaternion $q$ and the scalar $s$.
	//
	//# \action		Quaternion operator -(float s, const Quaternion& q);
	//#				Returns the difference of the scalar $s$ and the quaternion $q$.
	//
	//# \action		Quaternion operator *(const Quaternion& q1, const Quaternion& q2);
	//#				Returns the product of the quaternions $q1$ and $q2$.
	//
	//# \action		Quaternion operator *(const Quaternion& q, const Vector3D& v);
	//#				Returns the product of the quaternion $q$ and the vector $v$.
	//
	//# \action		Quaternion operator *(const Vector3D& v, const Quaternion& q);
	//#				Returns the product of the vector $v$ and the quaternion $q$.
	//
	//# \action		Quaternion operator *(const Quaternion& q, float s);
	//#				Returns the product of the quaternion $q$ and the scalar $s$.
	//
	//# \action		Quaternion operator *(float s, const Quaternion& q);
	//#				Returns the product of the scalar $s$ and the quaternion $q$.
	//
	//# \action		Quaternion operator /(const Quaternion& q1, const Quaternion& q2);
	//#				Returns the quotient of the quaternions $q1$ and $q2$.
	//
	//# \action		Quaternion operator /(const Quaternion& q, const Vector3D& v);
	//#				Returns the quotient of the quaternion $q$ and of the vector $v$.
	//
	//# \action		Quaternion operator /(const Vector3D& v, const Quaternion& q);
	//#				Returns the quotient of the vector $v$ and the quaternion $q$.
	//
	//# \action		Quaternion operator /(const Quaternion& q, float s);
	//#				Returns the quotient of the quaternion $q$ and the scalar $s$.
	//
	//# \action		Quaternion operator /(float s, const Quaternion& q);
	//#				Returns the quotient of the scalar $s$ and the quaternion $q$.
	//
	//# \action		bool operator ==(const Quaternion& q1, const Quaternion& q2);
	//#				Returns a boolean value indicating the equality of the two quaternions $q1$ and $q2$.
	//
	//# \action		bool operator ==(const Quaternion& q, const Vector3D& v);
	//#				Returns a boolean value indicating the equality of the quaternion $q$ and the vector $v$.
	//
	//# \action		bool operator ==(const Vector3D& v, const Quaternion& q);
	//#				Returns a boolean value indicating the equality of the vector $v$ and the quaternion $q$.
	//
	//# \action		bool operator ==(const Quaternion& q, float s);
	//#				Returns a boolean value indicating the equality of the quaternion $q$ and the scalar $s$.
	//
	//# \action		bool operator ==(float s, const Quaternion& q);
	//#				Returns a boolean value indicating the equality of the scalar $a$ and the quaternion $q$.
	//
	//# \action		bool operator !=(const Quaternion& q1, const Quaternion& q2);
	//#				Returns a boolean value indicating the inequality of the two quaternions $q1$ and $q2$.
	//
	//# \action		bool operator !=(const Quaternion& q, const Vector3D& v);
	//#				Returns a boolean value indicating the inequality of the quaternion $q$ and the vector $v$.
	//
	//# \action		bool operator !=(const Vector3D& v, const Quaternion& q);
	//#				Returns a boolean value indicating the inequality of the vector $v$ and the quaternion $q$.
	//
	//# \action		bool operator !=(const Quaternion& q, float s);
	//#				Returns a boolean value indicating the inequality of the quaternion $q$ and the scalar $s$.
	//
	//# \action		bool operator !=(float s, const Quaternion& q);
	//#				Returns a boolean value indicating the inequality of the scalar $s$ and the quaternion $q$.
	//
	//# \action		float Magnitude(const Quaternion& q);
	//#				Returns the magnitude of a quaternion.
	//
	//# \action		float SquaredMag(const Quaternion& q);
	//#				Returns the squared magnitude of a quaternion.
	//
	//# \action		Quaternion Conjugate(const Quaternion& q);
	//#				Returns the conjugate of a quaternion.
	//
	//# \action		Quaternion Inverse(const Quaternion& q);
	//#				Returns the inverse of a quaternion.
	//
	//# \also	$@Vector3D@$
	//# \also	$@Matrix3D@$


	//# \function	Quaternion::Set		Sets all four components of a quaternion.
	//
	//# \proto	Quaternion& Set(float a, float b, float c, float s);
	//# \proto	Quaternion& Set(const Vector3D& v, float s);
	//
	//# \param	a	The new <i>x</i> coordinate.
	//# \param	b	The new <i>y</i> coordinate.
	//# \param	c	The new <i>z</i> coordinate.
	//# \param	s	The new <i>w</i> coordinate.
	//# \param	v	The new <i>x</i>, <i>y</i>, and <i>z</i> coordinates.
	//
	//# \desc
	//# The $Set$ function sets the <i>x</i>, <i>y</i>, <i>z</i>, and <i>w</i> coordinates of
	//# a quaternion to the values given by the $a$, $b$, $c$, and $s$ parameters, respectively.
	//# If the parameters $v$ and $s$ are supplied, then the <i>x</i>, <i>y</i>, and <i>z</i> coordinates
	//# are set to those of the vector $v$, and the <i>w</i> coordinate is set to the value
	//# given by $s$.
	//#
	//# The return value is a reference to the quaternion object.


	//# \function	Quaternion::Normalize		Normalizes a quaternion.
	//
	//# \proto	Quatnerion& Normalize(void);
	//
	//# \desc
	//# The $Normalize$ function multiplies a quaternion by the inverse of its magnitude,
	//# normalizing it to unit length. Normalizing the zero quaternion produces undefined results.
	//#
	//# The return value is a reference to the quaternion object.


	//# \function	Quaternion::SetRotationAboutX		Sets a quaternion to represent a rotation about the <i>x</i> axis.
	//
	//# \proto	Quaternion& SetRotationAboutX(float angle);
	//
	//# \param	angle	The angle of rotation, in radians.
	//
	//# \desc
	//# The $SetRotationAboutX$ function replaces the components of a quaternion with those
	//# representing the rotation about the <i>x</i> axis through the angle given by the $angle$
	//# parameter. The resulting quaternion has unit length.
	//
	//# \also	$@Quaternion::SetRotationAboutY@$
	//# \also	$@Quaternion::SetRotationAboutZ@$
	//# \also	$@Quaternion::SetRotationAboutAxis@$
	//# \also	$@Quaternion::GetRotationMatrix@$


	//# \function	Quaternion::SetRotationAboutY		Sets a quaternion to represent a rotation about the <i>y</i> axis.
	//
	//# \proto	Quaternion& SetRotationAboutY(float angle);
	//
	//# \param	angle	The angle of rotation, in radians.
	//
	//# \desc
	//# The $SetRotationAboutY$ function replaces the components of a quaternion with those
	//# representing the rotation about the <i>y</i> axis through the angle given by the $angle$
	//# parameter. The resulting quaternion has unit length.
	//
	//# \also	$@Quaternion::SetRotationAboutX@$
	//# \also	$@Quaternion::SetRotationAboutZ@$
	//# \also	$@Quaternion::SetRotationAboutAxis@$
	//# \also	$@Quaternion::GetRotationMatrix@$


	//# \function	Quaternion::SetRotationAboutZ		Sets a quaternion to represent a rotation about the <i>z</i> axis.
	//
	//# \proto	Quaternion& SetRotationAboutZ(float angle);
	//
	//# \param	angle	The angle of rotation, in radians.
	//
	//# \desc
	//# The $SetRotationAboutZ$ function replaces the components of a quaternion with those
	//# representing the rotation about the <i>z</i> axis through the angle given by the $angle$
	//# parameter. The resulting quaternion has unit length.
	//
	//# \also	$@Quaternion::SetRotationAboutX@$
	//# \also	$@Quaternion::SetRotationAboutY@$
	//# \also	$@Quaternion::SetRotationAboutAxis@$
	//# \also	$@Quaternion::GetRotationMatrix@$


	//# \function	Quaternion::SetRotationAboutAxis		Sets a quaternion to represent a rotation about a given axis.
	//
	//# \proto	Quaternion& SetRotationAboutAxis(float angle, const Vector3D& axis);
	//
	//# \param	angle	The angle of rotation, in radians.
	//# \param	axis	The axis about which to rotate. This vector should have unit length.
	//
	//# \desc
	//# The $SetRotationAboutAxis$ function replaces the components of a quaternion with those
	//# representing the rotation about the axis given by the $axis$ parameter through the angle
	//# given by the $angle$ parameter. The resulting quaternion has unit length.
	//
	//# \also	$@Quaternion::SetRotationAboutX@$
	//# \also	$@Quaternion::SetRotationAboutY@$
	//# \also	$@Quaternion::SetRotationAboutZ@$
	//# \also	$@Quaternion::GetRotationMatrix@$


	//# \function	Quaternion::GetRotationMatrix		Converts a quaternion to a 3&nbsp;&times;&nbsp;3 matrix.
	//
	//# \proto	Matrix3D GetRotationMatrix(void) const;
	//
	//# \desc
	//# The $GetRotationMatrix$ function converts a unit quaternion to a $@Matrix3D@$ object that
	//# represents the same rotation when it premultiplies a $@Vector3D@$ object.
	//
	//# \also	$@Quaternion::GetRotationScaleMatrix@$
	//# \also	$@Quaternion::SetRotationMatrix@$
	//# \also	$@Quaternion::SetRotationAboutX@$
	//# \also	$@Quaternion::SetRotationAboutY@$
	//# \also	$@Quaternion::SetRotationAboutZ@$
	//# \also	$@Quaternion::SetRotationAboutAxis@$


	//# \function	Quaternion::GetRotationScaleMatrix		Converts a quaternion to a 3&nbsp;&times;&nbsp;3 matrix with scale information.
	//
	//# \proto	Matrix3D GetRotationScaleMatrix(void) const;
	//
	//# \desc
	//# The $GetRotationScaleMatrix$ function converts a (not necessarily unit) quaternion to a $@Matrix3D@$ object that
	//# represents the same rotation when it premultiplies a $@Vector3D@$ object. This function differs
	//# from the $@Quaternion::GetRotationMatrix@$ function in that the magnitude of the quaternion
	//# is included in the output matrix.
	//
	//# \also	$@Quaternion::GetRotationMatrix@$
	//# \also	$@Quaternion::SetRotationMatrix@$
	//# \also	$@Quaternion::SetRotationAboutX@$
	//# \also	$@Quaternion::SetRotationAboutY@$
	//# \also	$@Quaternion::SetRotationAboutZ@$
	//# \also	$@Quaternion::SetRotationAboutAxis@$


	//# \function	Quaternion::SetRotationMatrix		Converts a 3&nbsp;&times;&nbsp;3 matrix to a quaternion.
	//
	//# \proto	Quaternion& SetRotationMatrix(const Matrix3D& m);
	//# \proto	Quaternion& SetRotationMatrix(const Transform4D& m);
	//
	//# \param	m	The matrix to convert to a quaternion.
	//
	//# \desc
	//# The $SetRotationMatrix$ function sets the components of a quaternion to values that
	//# represent the same rotation as the one represented by the matrix specified by the $m$ parameter.
	//# If $m$ is a $@Transform4D@$ object, then the upper-left 3&nbsp;&times;&nbsp;3 submatrix is used,
	//# and the fourth column is ignored.
	//#
	//# For best results, the matrix $m$ should be orthogonal. If the determinant of $m$ is not positive,
	//# then the results are undefined.
	//
	//# \also	$@Quaternion::GetRotationMatrix@$
	//# \also	$@Quaternion::SetRotationAboutX@$
	//# \also	$@Quaternion::SetRotationAboutY@$
	//# \also	$@Quaternion::SetRotationAboutZ@$
	//# \also	$@Quaternion::SetRotationAboutAxis@$


	//# \member		Quaternion

	class Quaternion
	{
		public:

			float	x;		//## The <i>x</i> coordinate.
			float	y;		//## The <i>y</i> coordinate.
			float	z;		//## The <i>z</i> coordinate.
			float	w;		//## The <i>w</i> coordinate.

			Quaternion() = default;

			explicit Quaternion(const Vector3D& v)
			{
				x = v.x;
				y = v.y;
				z = v.z;
				w = 0.0F;
			}

			Quaternion(const Vector3D& v, float s)
			{
				x = v.x;
				y = v.y;
				z = v.z;
				w = s;
			}

			explicit Quaternion(float s)
			{
				w = s;
				x = y = z = 0.0F;
			}

			Quaternion(float a, float b, float c, float s)
			{
				x = a;
				y = b;
				z = c;
				w = s;
			}

			Quaternion& Set(const Vector3D& v, float s)
			{
				x = v.x;
				y = v.y;
				z = v.z;
				w = s;
				return (*this);
			}

			void Set(const Vector3D& v, float s) volatile
			{
				x = v.x;
				y = v.y;
				z = v.z;
				w = s;
			}

			Quaternion& Set(float a, float b, float c, float s)
			{
				x = a;
				y = b;
				z = c;
				w = s;
				return (*this);
			}

			void Set(float a, float b, float c, float s) volatile
			{
				x = a;
				y = b;
				z = c;
				w = s;
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

			Quaternion& operator =(const Quaternion& q)
			{
				x = q.x;
				y = q.y;
				z = q.z;
				w = q.w;
				return (*this);
			}

			void operator =(const Quaternion& q) volatile
			{
				x = q.x;
				y = q.y;
				z = q.z;
				w = q.w;
			}

			Quaternion& operator =(const Vector3D& v)
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

			Quaternion& operator =(float s)
			{
				w = s;
				x = y = z = 0.0F;
				return (*this);
			}

			void operator =(float s) volatile
			{
				w = s;
				x = y = z = 0.0F;
			}

			Quaternion& operator +=(const Quaternion& q)
			{
				x += q.x;
				y += q.y;
				z += q.z;
				w += q.w;
				return (*this);
			}

			Quaternion& operator +=(const Vector3D& v)
			{
				x += v.x;
				y += v.y;
				z += v.z;
				return (*this);
			}

			Quaternion& operator +=(float s)
			{
				w += s;
				return (*this);
			}

			Quaternion& operator -=(const Quaternion& q)
			{
				x -= q.x;
				y -= q.y;
				z -= q.z;
				w -= q.w;
				return (*this);
			}

			Quaternion& operator -=(const Vector3D& v)
			{
				x -= v.x;
				y -= v.y;
				z -= v.z;
				return (*this);
			}

			Quaternion& operator -=(float s)
			{
				w -= s;
				return (*this);
			}

			C4API Quaternion& operator *=(const Quaternion& q);
			C4API Quaternion& operator *=(const Vector3D& v);

			Quaternion& operator *=(float s)
			{
				x *= s;
				y *= s;
				z *= s;
				w *= s;
				return (*this);
			}

			C4API Quaternion& operator /=(const Quaternion& q);
			C4API Quaternion& operator /=(const Vector3D& v);

			Quaternion& operator /=(float s)
			{
				float f = 1.0F / s;
				x *= f;
				y *= f;
				z *= f;
				w *= f;
				return (*this);
			}

			Quaternion& Normalize(void)
			{
				return (*this *= InverseSqrt(x * x + y * y + z * z + w * w));
			}

			Vector3D GetDirectionX(void) const
			{
				return (Vector3D(1.0F - 2.0F * (y * y + z * z), 2.0F * (x * y + w * z), 2.0F * (x * z - w * y)));
			}

			Vector3D GetDirectionY(void) const
			{
				return (Vector3D(2.0F * (x * y - w * z), 1.0F - 2.0F * (x * x + z * z), 2.0F * (y * z + w * x)));
			}

			Vector3D GetDirectionZ(void) const
			{
				return (Vector3D(2.0F * (x * z + w * y), 2.0F * (y * z - w * x), 1.0F - 2.0F * (x * x + y * y)));
			}

			Quaternion& SetRotationAboutX(float angle)
			{
				CosSin(angle * 0.5F, &w, &x);
				y = z = 0.0F;
				return (*this);
			}

			Quaternion& SetRotationAboutY(float angle)
			{
				CosSin(angle * 0.5F, &w, &y);
				x = z = 0.0F;
				return (*this);
			}

			Quaternion& SetRotationAboutZ(float angle)
			{
				CosSin(angle * 0.5F, &w, &z);
				x = y = 0.0F;
				return (*this);
			}

			C4API Quaternion& SetRotationAboutAxis(float angle, const Vector3D& axis);
			C4API Matrix3D GetRotationMatrix(void) const;
			C4API Matrix3D GetRotationScaleMatrix(void) const;
			C4API Quaternion& SetRotationMatrix(const Matrix3D& m);
			C4API Quaternion& SetRotationMatrix(const Transform4D& m);
	};


	inline Quaternion operator -(const Quaternion& q)
	{
		return (Quaternion(-q.x, -q.y, -q.z, -q.w));
	}

	inline Quaternion operator +(const Quaternion& q1, const Quaternion& q2)
	{
		return (Quaternion(q1.x + q2.x, q1.y + q2.y, q1.z + q2.z, q1.w + q2.w));
	}

	inline Quaternion operator +(const Quaternion& q, const Vector3D& v)
	{
		return (Quaternion(q.x + v.x, q.y + v.y, q.z + v.z, q.w));
	}

	inline Quaternion operator +(const Vector3D& v, const Quaternion& q)
	{
		return (Quaternion(v.x + q.x, v.y + q.y, v.z + q.z, q.w));
	}

	inline Quaternion operator +(const Quaternion& q, float s)
	{
		return (Quaternion(q.x, q.y, q.z, q.w + s));
	}

	inline Quaternion operator +(float s, const Quaternion& q)
	{
		return (Quaternion(q.x, q.y, q.z, s + q.w));
	}

	inline Quaternion operator -(const Quaternion& q1, const Quaternion& q2)
	{
		return (Quaternion(q1.x - q2.x, q1.y - q2.y, q1.z - q2.z, q1.w - q2.w));
	}

	inline Quaternion operator -(const Quaternion& q, const Vector3D& v)
	{
		return (Quaternion(q.x - v.x, q.y - v.y, q.z - v.z, q.w));
	}

	inline Quaternion operator -(const Vector3D& v, const Quaternion& q)
	{
		return (Quaternion(v.x - q.x, v.y - q.y, v.z - q.z, -q.w));
	}

	inline Quaternion operator -(const Quaternion& q, float s)
	{
		return (Quaternion(q.x, q.y, q.z, q.w - s));
	}

	inline Quaternion operator -(float s, const Quaternion& q)
	{
		return (Quaternion(-q.x, -q.y, -q.z, s - q.w));
	}

	inline Quaternion operator *(const Quaternion& q, float s)
	{
		return (Quaternion(q.x * s, q.y * s, q.z * s, q.w * s));
	}

	inline Quaternion operator *(float s, const Quaternion& q)
	{
		return (Quaternion(s * q.x, s * q.y, s * q.z, s * q.w));
	}

	inline Quaternion operator /(const Quaternion& q, float s)
	{
		float f = 1.0F / s;
		return (Quaternion(q.x * f, q.y * f, q.z * f, q.w * f));
	}

	inline bool operator ==(const Quaternion& q1, const Quaternion& q2)
	{
		return ((q1.x == q2.x) && (q1.y == q2.y) && (q1.z == q2.z) && (q1.w == q2.w));
	}

	inline bool operator ==(const Quaternion& q, const Vector3D& v)
	{
		return ((q.x == v.x) && (q.y == v.y) && (q.z == v.z) && (q.w == 0.0F));
	}

	inline bool operator ==(const Vector3D& v, const Quaternion& q)
	{
		return ((q.x == v.x) && (q.y == v.y) && (q.z == v.z) && (q.w == 0.0F));
	}

	inline bool operator ==(const Quaternion& q, float s)
	{
		return ((q.w == s) && (q.x == 0.0F) && (q.y == 0.0F) && (q.z == 0.0F));
	}

	inline bool operator ==(float s, const Quaternion& q)
	{
		return ((q.w == s) && (q.x == 0.0F) && (q.y == 0.0F) && (q.z == 0.0F));
	}

	inline bool operator !=(const Quaternion& q1, const Quaternion& q2)
	{
		return ((q1.x != q2.x) || (q1.y != q2.y) || (q1.z != q2.z) || (q1.w != q2.w));
	}

	inline bool operator !=(const Quaternion& q, const Vector3D& v)
	{
		return ((q.x != v.x) || (q.y != v.y) || (q.z != v.z) || (q.w != 0.0F));
	}

	inline bool operator !=(const Vector3D& v, const Quaternion& q)
	{
		return ((q.x != v.x) || (q.y != v.y) || (q.z != v.z) || (q.w != 0.0F));
	}

	inline bool operator !=(const Quaternion& q, float s)
	{
		return ((q.w != s) || (q.x != 0.0F) || (q.y != 0.0F) || (q.z != 0.0F));
	}

	inline bool operator !=(float s, const Quaternion& q)
	{
		return ((q.w != s) || (q.x != 0.0F) || (q.y != 0.0F) || (q.z != 0.0F));
	}


	inline float Dot(const Quaternion& q1, const Quaternion& q2)
	{
		return (q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w);
	}

	inline float Magnitude(const Quaternion& q)
	{
		return (Sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w));
	}

	inline float SquaredMag(const Quaternion& q)
	{
		return (q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
	}

	inline Quaternion Conjugate(const Quaternion& q)
	{
		return (Quaternion(-q.x, -q.y, -q.z, q.w));
	}

	inline Quaternion Inverse(const Quaternion& q)
	{
		return (Conjugate(q) / SquaredMag(q));
	}

	inline Quaternion& Quaternion::operator /=(const Quaternion& q)
	{
		return (*this *= Inverse(q));
	}

	inline Quaternion& Quaternion::operator /=(const Vector3D& v)
	{
		return (*this *= -v / SquaredMag(v));
	}

	inline Quaternion operator /(float s, const Quaternion& q)
	{
		return (s * Inverse(q));
	}


	C4API Quaternion operator *(const Quaternion& q1, const Quaternion& q2);
	C4API Quaternion operator *(const Quaternion& q, const Vector3D& v);


	inline Quaternion operator *(const Vector3D& v, const Quaternion& q)
	{
		return (Quaternion(v.x, v.y, v.z, 0.0F) * q);
	}

	inline Quaternion operator /(const Quaternion& q1, const Quaternion& q2)
	{
		return (q1 * Inverse(q2));
	}

	inline Quaternion operator /(const Quaternion& q, const Vector3D& v)
	{
		return (q * (-v / SquaredMag(v)));
	}

	inline Quaternion operator /(const Vector3D& v, const Quaternion& q)
	{
		return (Quaternion(v.x, v.y, v.z, 0.0F) * Inverse(q));
	}


	C4API Vector3D Transform(const Vector3D& v, const Quaternion& q);


	struct ConstQuaternion
	{
		float	x;
		float	y;
		float	z;
		float	w;

		operator const Quaternion&(void) const
		{
			return (reinterpret_cast<const Quaternion&>(*this));
		}

		const Quaternion *operator &(void) const
		{
			return (reinterpret_cast<const Quaternion *>(this));
		}
	};
}


#endif

// ZYUQURM
