 

#ifndef C4Matrix3D_h
#define C4Matrix3D_h


//# \component	Math Library
//# \prefix		Math/


#include "C4Vector3D.h"


namespace C4
{
	class Matrix4D;
	class Transform4D;


	class MatrixRow3D
	{
		friend class Matrix3D;

		private:

			float	matrix[7];

			MatrixRow3D() = delete;

		public:

			operator Antivector3D(void) const
			{
				return (Antivector3D(matrix[0], matrix[3], matrix[6]));
			}

			float& operator [](machine j)
			{
				return (matrix[j * 3]);
			}

			const float& operator [](machine j) const
			{
				return (matrix[j * 3]);
			}

			MatrixRow3D& operator *=(float t)
			{
				matrix[0] *= t;
				matrix[3] *= t;
				matrix[6] *= t;
				return (*this);
			}

			MatrixRow3D& operator /=(float t)
			{
				float f = 1.0F / t;
				matrix[0] *= f;
				matrix[3] *= f;
				matrix[6] *= f;
				return (*this);
			}

			friend float operator ^(const MatrixRow3D& row, const Vector3D& v);
	};


	inline float operator ^(const MatrixRow3D& row, const Vector3D& v)
	{
		return (row.matrix[0] * v.x + row.matrix[3] * v.y + row.matrix[6] * v.z);
	}


	//# \class	Matrix3D	Encapsulates a 3&nbsp;&times;&nbsp;3 matrix.
	//
	//# The $Matrix3D$ class encapsulates a 3&nbsp;&times;&nbsp;3 matrix.
	//
	//# \def	class Matrix3D
	//
	//# \ctor	Matrix3D();
	//# \ctor	Matrix3D(const Vector3D& c1, const Vector3D& c2, const Vector3D& c3);
	//# \ctor	Matrix3D(float n00, float n01, float n02, float n10, float n11, float n12, float n20, float n21, float n22);
	//
	//# \param	c1		The values of the entries residing in first column.
	//# \param	c2		The values of the entries residing in second column.
	//# \param	c3		The values of the entries residing in third column.
	//# \param	nij		The value of the entry residing in row <i>i</i> and column <i>j</i>.
	//
	//# \desc
	//# The $Matrix3D$ class is used to store a 3&nbsp;&times;&nbsp;3 matrix. The entries of the matrix
	//# are accessed using the $()$ operator with two indexes specifying the row and column of an entry.
	//#
	//# The default constructor leaves the entries of the matrix undefined. If the nine entries are
	//# supplied, then the $n$<i>ij</i> parameter specifies the entry in the <i>i</i>-th row and
	//# <i>j</i>-th column. If the matrix is constructed using the three vectors $c1$, $c2$, and
	//# $c3$, then these vectors initialize the three columns of the matrix.
	//
	//# \operator	float& operator ()(int32 i, int32 j);
	//#				Returns a reference to the entry in the <i>i</i>-th row and <i>j</i>-th column.
	//#				Both $i$ and $j$ must be 0, 1, or 2.
	//
	//# \operator	const float& operator ()(int32 i, int32 j) const;
	//#				Returns a constant reference to the entry in the <i>i</i>-th row and <i>j</i>-th column. 
	//#				Both $i$ and $j$ must be 0, 1, or 2.
	//
	//# \operator	Vector3D& operator [](machine j); 
	//#				Returns a reference to the <i>j</i>-th column of a matrix. $j$ must be 0, 1, or 2.
	// 
	//# \operator	const Vector3D& operator [](machine j) const;
	//#				Returns a constant reference to the <i>j</i>-th column of a matrix. $j$ must be 0, 1, or 2.
	// 
	//# \operator	Matrix3D& operator *=(const Matrix3D& m);
	//#				Multiplies by the matrix $m$. 
	// 
	//# \operator	Matrix3D& operator *=(float t);
	//#				Multiplies by the scalar $t$.
	//
	//# \operator	Matrix3D& operator /=(float t); 
	//#				Multiplies by the inverse of the scalar $t$.
	//
	//# \action		Matrix3D operator *(const Matrix3D& m1, const Matrix3D& m2);
	//#				Returns the product of the matrices $m1$ and $m2$.
	//
	//# \action		Matrix3D operator *(const Matrix3D& m, float t);
	//#				Returns the product of the matrix $m$ and the scalar $t$.
	//
	//# \action		Matrix3D operator /(const Matrix3D& m, float t);
	//#				Returns the product of the matrix $m$ and the inverse of the scalar $t$.
	//
	//# \action		Vector3D operator *(const Matrix3D& m, const Vector3D& v);
	//#				Returns the product of the matrix $m$ and the column vector $v$.
	//
	//# \action		Vector3D operator *(const Matrix3D& m, const Point3D& p);
	//#				Returns the product of the matrix $m$ and the column vector $p$.
	//
	//# \action		Vector3D operator *(const Vector3D& v, const Matrix3D& m);
	//#				Returns the product of the row vector $v$ and the matrix $m$.
	//
	//# \action		Vector3D operator *(const Point3D& p, const Matrix3D& m);
	//#				Returns the product of the row vector $p$ and the matrix $m$.
	//
	//# \action		bool operator ==(const Matrix3D& m1, const Matrix3D& m2);
	//#				Returns a boolean value indicating the equality of the two matrices $m1$ and $m2$.
	//
	//# \action		bool operator !=(const Matrix3D& m1, const Matrix3D& m2);
	//#				Returns a boolean value indicating the inequality of the two matrices $m1$ and $m2$.
	//
	//# \action		float Determinant(const Matrix3D& m);
	//#				Returns the determinant of the matrix $m$.
	//
	//# \action		Matrix3D Inverse(const Matrix3D& m);
	//#				Returns the inverse of the matrix $m$. If $m$ is singular, then the result is undefined.
	//
	//# \action		Matrix3D Adjugate(const Matrix3D& m);
	//#				Returns the adjugate of the matrix $m$.
	//
	//# \action		Matrix3D Transpose(const Matrix3D& m);
	//#				Returns the transpose of the matrix $m$.
	//
	//# \also	$@Matrix4D@$
	//# \also	$@Transform4D@$


	//# \function	Matrix3D::Set		Sets all nine entries of a matrix.
	//
	//# \proto	Matrix3D& Set(const Vector3D& c1, const Vector3D& c2, const Vector3D& c3);
	//# \proto	Matrix3D& Set(float n00, float n01, float n02, float n10, float n11, float n12, float n20, float n21, float n22);
	//
	//# \param	c1		The new values of the entries residing in first column.
	//# \param	c2		The new values of the entries residing in second column.
	//# \param	c3		The new values of the entries residing in third column.
	//# \param	nij		The new value of the entry residing in row <i>i</i> and column <i>j</i>.
	//
	//# \desc
	//# The $Set$ function sets all nine entries of a matrix, either by specifying each entry individually
	//# or by specifying each of the three columns as 3D vectors.
	//#
	//# The return value is a reference to the matrix object.
	//
	//# \also	$@Matrix3D::SetIdentity@$
	//# \also	$@Matrix3D::SetRotationAboutX@$
	//# \also	$@Matrix3D::SetRotationAboutY@$
	//# \also	$@Matrix3D::SetRotationAboutZ@$
	//# \also	$@Matrix3D::SetRotationAboutAxis@$


	//# \function	Matrix3D::SetIdentity		Sets a matrix to the 3&nbsp;&times;&nbsp;3 identity matrix.
	//
	//# \proto	Matrix3D& SetIdentity(void);
	//
	//# \desc
	//# The $SetIdentity$ function replaces all entries of a matrix with the entries of the identity matrix.
	//#
	//# The return value is a reference to the matrix object.
	//
	//# \also	$@Matrix3D::Set@$
	//# \also	$@Matrix3D::SetRotationAboutX@$
	//# \also	$@Matrix3D::SetRotationAboutY@$
	//# \also	$@Matrix3D::SetRotationAboutZ@$
	//# \also	$@Matrix3D::SetRotationAboutAxis@$


	//# \function	Matrix3D::SetRotationAboutX		Sets a matrix to represent a rotation about the <i>x</i> axis.
	//
	//# \proto	Matrix3D& SetRotationAboutX(float angle);
	//
	//# \param	angle	The angle through which to rotate, in radians.
	//
	//# \desc
	//# The $SetRotationAboutX$ function replaces all entries of a matrix with those representing a
	//# rotation about the <i>x</i> axis through the angle given by the $angle$ parameter.
	//#
	//# The return value is a reference to the matrix object.
	//
	//# \also	$@Matrix3D::SetRotationAboutY@$
	//# \also	$@Matrix3D::SetRotationAboutZ@$
	//# \also	$@Matrix3D::SetRotationAboutAxis@$
	//# \also	$@Matrix3D::SetIdentity@$
	//# \also	$@Matrix3D::Set@$


	//# \function	Matrix3D::SetRotationAboutY		Sets a matrix to represent a rotation about the <i>y</i>axis.
	//
	//# \proto	Matrix3D& SetRotationAboutY(float angle);
	//
	//# \param	angle	The angle through which to rotate, in radians.
	//
	//# \desc
	//# The $SetRotationAboutY$ function replaces all entries of a matrix with those representing a
	//# rotation about the <i>y</i> axis through the angle given by the $angle$ parameter.
	//#
	//# The return value is a reference to the matrix object.
	//
	//# \also	$@Matrix3D::SetRotationAboutX@$
	//# \also	$@Matrix3D::SetRotationAboutZ@$
	//# \also	$@Matrix3D::SetRotationAboutAxis@$
	//# \also	$@Matrix3D::SetIdentity@$
	//# \also	$@Matrix3D::Set@$


	//# \function	Matrix3D::SetRotationAboutZ		Sets a matrix to represent a rotation about the <i>z</i> axis.
	//
	//# \proto	Matrix3D& SetRotationAboutZ(float angle);
	//
	//# \param	angle	The angle through which to rotate, in radians.
	//
	//# \desc
	//# The $SetRotationAboutZ$ function replaces all entries of a matrix with those representing a
	//# rotation about the <i>z</i> axis through the angle given by the $angle$ parameter.
	//#
	//# The return value is a reference to the matrix object.
	//
	//# \also	$@Matrix3D::SetRotationAboutX@$
	//# \also	$@Matrix3D::SetRotationAboutY@$
	//# \also	$@Matrix3D::SetRotationAboutAxis@$
	//# \also	$@Matrix3D::SetIdentity@$
	//# \also	$@Matrix3D::Set@$


	//# \function	Matrix3D::SetRotationAboutAxis		Sets a matrix to represent a rotation about a given axis.
	//
	//# \proto	Matrix3D& SetRotationAboutAxis(float angle, const Antivector3D& axis);
	//
	//# \param	angle	The angle through which to rotate, in radians.
	//# \param	axis	The axis about which to rotate. This vector should have unit length.
	//
	//# \desc
	//# The $SetRotationAboutAxis$ function replaces all entries of a matrix with those representing a
	//# rotation about the axis given by the $axis$ parameter through the angle given by the $angle$ parameter.
	//#
	//# The return value is a reference to the matrix object.
	//
	//# \also	$@Matrix3D::SetRotationAboutX@$
	//# \also	$@Matrix3D::SetRotationAboutY@$
	//# \also	$@Matrix3D::SetRotationAboutZ@$
	//# \also	$@Matrix3D::SetIdentity@$
	//# \also	$@Matrix3D::Set@$


	//# \function	Matrix3D::SetScale		Sets a matrix to represent a scale.
	//
	//# \proto	Matrix3D& SetScale(float t);
	//# \proto	Matrix3D& SetScale(float r, float s, float t);
	//
	//# \param	s	The scale along the <i>x</i> axis.
	//# \param	r	The scale along the <i>y</i> axis.
	//# \param	t	The scale along the <i>z</i> axis, or if specified by itself, the scale along all three axes.
	//
	//# \desc
	//# The $SetScale$ function replaces all entries of a matrix with those representing a scale.
	//# If only the $t$ parameter is specified, then the scale is uniform along all three axes.
	//# If the $r$, $s$, and $t$ parameters are specified, then they correspond to the scale along
	//# the <i>x</i>, <i>y</i>, and <i>z</i> axis, respectively.
	//#
	//# The return value is a reference to the matrix object.
	//
	//# \also	$@Matrix3D::SetIdentity@$
	//# \also	$@Matrix3D::Set@$


	//# \function	Matrix3D::Orthonormalize		Orthonormalizes the columns of a matrix.
	//
	//# \proto	Matrix3D& Orthonormalize(column);
	//
	//# \param	column		The index of the column whose direction does not change. This must be 0, 1, or 2.
	//
	//# \desc
	//# The $Orthonormalize$ function uses Gram-Schmidt orthogonalization to orthogonalize the columns
	//# of a matrix. The column whose index is specified by the $column$ parameter is normalized to unit length.
	//# The remaining two columns are orthogonalized and made unit length. Only the two columns not specified
	//# by the $column$ parameter can change direction.


	//# \function	Matrix3D::GetRow		Returns an $Antivector3D$ object containing the entries of a particular row.
	//
	//# \proto	Antivector3D GetRow(int32 i) const;
	//
	//# \param	i	The index of the row. This must be 0, 1, or 2.
	//
	//# \desc
	//# The $GetRow$ function returns an antivector containing the three entries in the row specified by
	//# the <i>i</i> parameter. Since matrices are stored in column-major order, this function must
	//# copy the entries into a new $Antivector3D$ object. A reference to a column of a matrix, requiring
	//# no copy, can be retrieved using the $[]$ operator.
	//
	//# \also	$@Matrix3D::SetRow@$


	//# \function	Matrix3D::SetRow		Sets the entries of a particular row to the values of an $Antivector3D$ object.
	//
	//# \proto	Matrix3D& SetRow(int32 i, const Antivector3D& row);
	//
	//# \param	i		The index of the row. This must be 0, 1, or 2.
	//# \param	row		A 3D vector containing the new values to be stored in the row.
	//
	//# \desc
	//# The $SetRow$ function sets the three entries in the row specified by the <i>i</i> parameter to the
	//# values contained in the 3D antivector specified by the $row$ parameter.
	//
	//# \also	$@Matrix3D::GetRow@$


	class Matrix3D
	{
		friend class Matrix4D;
		friend class Transform4D;

		private:

			float	n[3][3];

		public:

			Matrix3D() = default;

			C4API Matrix3D(const Vector3D& c1, const Vector3D& c2, const Vector3D& c3);
			C4API Matrix3D(float n00, float n01, float n02, float n10, float n11, float n12, float n20, float n21, float n22);

			C4API Matrix3D& Set(const Vector3D& c1, const Vector3D& c2, const Vector3D& c3);
			C4API Matrix3D& Set(float n00, float n01, float n02, float n10, float n11, float n12, float n20, float n21, float n22);

			float& operator ()(int32 i, int32 j)
			{
				return (n[j][i]);
			}

			const float& operator ()(int32 i, int32 j) const
			{
				return (n[j][i]);
			}

			Vector3D& operator [](machine j)
			{
				return (*reinterpret_cast<Vector3D *>(n[j]));
			}

			const Vector3D& operator [](machine j) const
			{
				return (*reinterpret_cast<const Vector3D *>(n[j]));
			}

			MatrixRow3D& GetRow(int32 i)
			{
				return (*reinterpret_cast<MatrixRow3D *>(&n[0][i]));
			}

			const MatrixRow3D& GetRow(int32 i) const
			{
				return (*reinterpret_cast<const MatrixRow3D *>(&n[0][i]));
			}

			Matrix3D& SetRow(int32 i, const Antivector3D& row)
			{
				n[0][i] = row.x;
				n[1][i] = row.y;
				n[2][i] = row.z;
				return (*this);
			}

			C4API Matrix3D& operator *=(const Matrix3D& m);
			C4API Matrix3D& operator *=(float t);
			C4API Matrix3D& operator /=(float t);

			C4API Matrix3D& SetIdentity(void);

			C4API Matrix3D& SetRotationAboutX(float angle);
			C4API Matrix3D& SetRotationAboutY(float angle);
			C4API Matrix3D& SetRotationAboutZ(float angle);
			C4API Matrix3D& SetRotationAboutAxis(float angle, const Antivector3D& axis);

			C4API void GetEulerAngles(float *x, float *y, float *z) const;
			C4API Matrix3D& SetEulerAngles(float x, float y, float z);

			C4API Matrix3D& SetScale(float t);
			C4API Matrix3D& SetScale(float r, float s, float t);

			C4API Matrix3D& Orthonormalize(int32 column);

			friend C4API Matrix3D operator *(const Matrix3D& m1, const Matrix3D& m2);
			friend C4API Matrix3D operator *(const Matrix3D& m, float t);
			friend C4API Matrix3D operator /(const Matrix3D& m, float t);
			friend C4API Vector3D operator *(const Matrix3D& m, const Vector3D& v);
			friend C4API Vector3D operator *(const Matrix3D& m, const Point3D& p);
			friend C4API Vector3D operator *(const Vector3D& v, const Matrix3D& m);
			friend C4API Vector3D operator *(const Point3D& p, const Matrix3D& m);
			friend C4API bool operator ==(const Matrix3D& m1, const Matrix3D& m2);
			friend C4API bool operator !=(const Matrix3D& m1, const Matrix3D& m2);

			friend C4API Matrix4D operator *(const Matrix4D& m1, const Matrix3D& m2);
			friend C4API Transform4D operator *(const Transform4D& m1, const Matrix3D& m2);
			friend C4API Transform4D operator *(const Matrix3D& m1, const Transform4D& m2);

			friend C4API Matrix3D Transform(const Transform4D& m1, const Matrix3D& m2);
			friend C4API Matrix3D TransformTranspose(const Transform4D& m1, const Matrix3D& m2);
			friend C4API Transform4D TransposeTransform(const Matrix3D& m1, const Transform4D& m2);
	};


	inline Matrix3D operator *(float t, const Matrix3D& m)
	{
		return (m * t);
	}


	struct ConstMatrix3D
	{
		float	n[3][3];

		operator const Matrix3D&(void) const
		{
			return (reinterpret_cast<const Matrix3D&>(*this));
		}

		const Matrix3D *operator &(void) const
		{
			return (reinterpret_cast<const Matrix3D *>(this));
		}

		const Matrix3D *operator ->(void) const
		{
			return (reinterpret_cast<const Matrix3D *>(this));
		}

		float operator ()(int32 i, int32 j) const
		{
			return (reinterpret_cast<const Matrix3D&>(*this)(i, j));
		}

		const Vector3D& operator [](machine j) const
		{
			return (reinterpret_cast<const Matrix3D&>(*this)[j]);
		}
	};


	C4API extern const ConstMatrix3D Identity3D;


	C4API Matrix3D Transform(const Transform4D& m1, const Matrix3D& m2);
	C4API Matrix3D TransformTranspose(const Transform4D& m1, const Matrix3D& m2);
	C4API Transform4D TransposeTransform(const Matrix3D& m1, const Transform4D& m2);

	C4API float Determinant(const Matrix3D& m);

	C4API Matrix3D Inverse(const Matrix3D& m);
	C4API Matrix3D Adjugate(const Matrix3D& m);
	C4API Matrix3D Transpose(const Matrix3D& m);
}


#endif

// ZYUQURM
