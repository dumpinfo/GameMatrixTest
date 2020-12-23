 

#ifndef C4Matrix4D_h
#define C4Matrix4D_h


//# \component	Math Library
//# \prefix		Math/


#include "C4Antivector4D.h"
#include "C4Matrix3D.h"


namespace C4
{
	class Transform4D;


	class MatrixRow4D
	{
		friend class Matrix4D;

		private:

			float	matrix[13];

			MatrixRow4D() = delete;

		public:

			operator Antivector4D(void) const
			{
				return (Antivector4D(matrix[0], matrix[4], matrix[8], matrix[12]));
			}

			float& operator [](machine j)
			{
				return (matrix[j * 4]);
			}

			const float& operator [](machine j) const
			{
				return (matrix[j * 4]);
			}

			MatrixRow4D& operator *=(float t)
			{
				matrix[0] *= t;
				matrix[4] *= t;
				matrix[8] *= t;
				matrix[12] *= t;
				return (*this);
			}

			MatrixRow4D& operator /=(float t)
			{
				float f = 1.0F / t;
				matrix[0] *= f;
				matrix[4] *= f;
				matrix[8] *= f;
				matrix[12] *= f;
				return (*this);
			}

			friend float operator ^(const MatrixRow4D& row, const Vector4D& v);
			friend float operator ^(const MatrixRow4D& row, const Vector3D& v);
			friend float operator ^(const MatrixRow4D& row, const Point3D& v);
	};


	inline float operator ^(const MatrixRow4D& row, const Vector4D& v)
	{
		return (row.matrix[0] * v.x + row.matrix[4] * v.y + row.matrix[8] * v.z + row.matrix[12] * v.w);
	}

	inline float operator ^(const MatrixRow4D& row, const Vector3D& v)
	{
		return (row.matrix[0] * v.x + row.matrix[4] * v.y + row.matrix[8] * v.z);
	}

	inline float operator ^(const MatrixRow4D& row, const Point3D& v)
	{
		return (row.matrix[0] * v.x + row.matrix[4] * v.y + row.matrix[8] * v.z + row.matrix[12]);
	}


	//# \class	Matrix4D	Encapsulates a 4&nbsp;&times;&nbsp;4 matrix.
	//
	//# The $Matrix4D$ class encapsulates a 4&nbsp;&times;&nbsp;4 matrix.
	//
	//# \def	class Matrix4D
	//
	//# \ctor	Matrix4D();
	//# \ctor	Matrix4D(const Vector4D& c1, const Vector4D& c2, const Vector4D& c3, const Vector4D& c4);
	//# \ctor	Matrix4D(float n00, float n01, float n02, float n03, float n10, float n11, float n12, float n13,
	//# \ctor2	float n20, float n21, float n22, float n23, float n30, float n31, float n32, float n33);
	//
	//# \param	c1		The values of the entries residing in first column.
	//# \param	c2		The values of the entries residing in second column.
	//# \param	c3		The values of the entries residing in third column.
	//# \param	c4		The values of the entries residing in fourth column.
	//# \param	nij		The value of the entry residing in row <i>i</i> and column <i>j</i>. 
	//
	//# \desc
	//# The $Matrix4D$ class is used to store a 4&nbsp;&times;&nbsp;4 matrix. The entries of the matrix 
	//# are accessed using the $()$ operator with two indexes specifying the row and column of an entry.
	//# 
	//# The default constructor leaves the entries of the matrix undefined. If the 16 entries are
	//# supplied, then the $n$<i>ij</i> parameter specifies the entry in the <i>i</i>-th row and
	//# <i>j</i>-th column. If the matrix is constructed using the four vectors $c1$, $c2$, $c3$, and 
	//# $c4$, then these vectors initialize the four columns of the matrix.
	// 
	//# \operator	float& operator ()(int32 i, int32 j); 
	//#				Returns a reference to the entry in the <i>i</i>-th row and <i>j</i>-th column.
	//#				Both $i$ and $j$ must be 0, 1, 2, or 3.
	//
	//# \operator	const float& operator ()(int32 i, int32 j) const; 
	//#				Returns a constant reference to the entry in the <i>i</i>-th row and <i>j</i>-th column.
	//#				Both $i$ and $j$ must be 0, 1, 2, or 3.
	//
	//# \operator	Vector4D& operator [](machine j);
	//#				Returns a reference to the <i>j</i>-th column of a matrix. $j$ must be 0, 1, 2, or 3.
	//
	//# \operator	const Vector4D& operator [](machine j) const;
	//#				Returns a constant reference to the <i>j</i>-th column of a matrix. $j$ must be 0, 1, 2, or 3.
	//
	//# \operator	Matrix4D& operator =(const Matrix3D& m);
	//#				Assigns the entries of $m$ to the upper-left 3&nbsp;&times;&nbsp;3 portion of a matrix and
	//#				assigns the entries of the identity matrix to the fourth row and fourth column.
	//
	//# \operator	Matrix4D& operator *=(const Matrix4D& m);
	//#				Multiplies by the matrix $m$.
	//
	//# \operator	Matrix4D& operator *=(const Matrix3D& m);
	//#				Multiplies by the matrix $m$. The entries of the fourth row and fourth column of $m$ are assumed
	//#				to be those of the identity matrix.
	//
	//# \action		Matrix4D operator *(const Matrix4D& m1, const Matrix4D& m2);
	//#				Returns the product of the matrices $m1$ and $m2$.
	//
	//# \action		Matrix4D operator *(const Matrix4D& m1, const Matrix3D& m2);
	//#				Returns the product of the matrices $m1$ and $m2$. The entries of the fourth row and fourth column of $m2$
	//#				are assumed to be those of the identity matrix.
	//
	//# \action		Vector4D operator *(const Matrix4D& m, const Vector4D& v);
	//#				Returns the product of the matrix $m$ and the column vector $v$.
	//
	//# \action		Antivector4D operator *(const Antivector4D& v, const Matrix4D& m);
	//#				Returns the prodict of the antivector $v$ and the matrix $m$.
	//
	//# \action		Vector4D operator *(const Matrix4D& m, const Vector3D& v);
	//#				Returns the product of the matrix $m$ and the column vector $v$. The <i>w</i> coordinate of $v$ is assumed to be 0.
	//
	//# \action		Vector4D operator *(const Vector3D& v, const Matrix4D& m);
	//#				Returns the prodict of the row vector $v$ and the matrix $m$. The <i>w</i> coordinate of $v$ is assumed to be 0.
	//
	//# \action		Vector4D operator *(const Matrix4D& m, const Point3D& p);
	//#				Returns the product of the matrix $m$ and the column vector $p$. The <i>w</i> coordinate of $p$ is assumed to be 1.
	//
	//# \action		Vector4D operator *(const Point3D& p, const Matrix4D& m);
	//#				Returns the prodict of the row vector $p$ and the matrix $m$. The <i>w</i> coordinate of $p$ is assumed to be 1.
	//
	//# \action		Vector4D operator *(const Matrix4D& m, const Vector2D& v);
	//#				Returns the product of the matrix $m$ and the column vector $v$. The <i>z</i> and <i>w</i> coordinates of $v$ are assumed to be 0.
	//
	//# \action		Vector4D operator *(const Vector2D& v, const Matrix4D& m);
	//#				Returns the prodict of the row vector $v$ and the matrix $m$. The <i>z</i> and <i>w</i> coordinates of $v$ are assumed to be 0.
	//
	//# \action		Vector4D operator *(const Matrix4D& m, const Point2D& p);
	//#				Returns the product of the matrix $m$ and the column vector $p$. The <i>z</i> coordinate of $p$ is assumed to be 0,
	//#				and the <i>w</i> coordinate of $p$ is assumed to be 1.
	//
	//# \action		Vector4D operator *(const Point2D& p, const Matrix4D& m);
	//#				Returns the prodict of the row vector $p$ and the matrix $m$. The <i>z</i> coordinate of $p$ is assumed to be 0,
	//#				and the <i>w</i> coordinate of $p$ is assumed to be 1.
	//
	//# \action		bool operator ==(const Matrix4D& m1, const Matrix4D& m2);
	//#				Returns a boolean value indicating the equality of the two matrices $m1$ and $m2$.
	//
	//# \action		bool operator !=(const Matrix4D& m1, const Matrix4D& m2);
	//#				Returns a boolean value indicating the inequality of the two matrices $m1$ and $m2$.
	//
	//# \action		float Determinant(const Matrix4D& m);
	//#				Returns the determinant of the matrix $m$.
	//
	//# \action		Matrix4D Inverse(const Matrix4D& m);
	//#				Returns the inverse of the matrix $m$. If $m$ is singular, then the result is undefined.
	//
	//# \action		Matrix4D Adjugate(const Matrix4D& m);
	//#				Returns the adjugate of the matrix $m$.
	//
	//# \action		Matrix4D Transpose(const Matrix4D& m);
	//#				Returns the transpose of the matrix $m$.
	//
	//# \also	$@Transform4D@$
	//# \also	$@Matrix3D@$
	//# \also	$@Vector3D@$
	//# \also	$@Point3D@$


	//# \function	Matrix4D::Set		Sets all 16 entries of a matrix.
	//
	//# \proto	Matrix4D& Set(const Vector4D& c1, const Vector4D& c2, const Vector4D& c3, const Vector4D& c4);
	//# \proto	Set(float n00, float n01, float n02, float n03, float n10, float n11, float n12, float n13,
	//# \proto2	float n20, float n21, float n22, float n23, float n30, float n31, float n32, float n33);
	//
	//# \param	c1		The new values of the entries residing in first column.
	//# \param	c2		The new values of the entries residing in second column.
	//# \param	c3		The new values of the entries residing in third column.
	//# \param	c4		The new values of the entries residing in fourth column.
	//# \param	nij		The new value of the entry residing in row <i>i</i> and column <i>j</i>.
	//
	//# \desc
	//# The $Set$ function sets all 16 entries of a matrix, either by specifying each entry individually
	//# or by specifying each of the four columns as 4D vectors.
	//#
	//# The return value is a reference to the matrix object.
	//
	//# \also	$@Matrix4D::SetIdentity@$


	//# \function	Matrix4D::SetIdentity		Sets a matrix to the 4&nbsp;&times;&nbsp;4 identity matrix.
	//
	//# \proto	Matrix4D& SetIdentity(void);
	//
	//# \desc
	//# The $SetIdentity$ function replaces all entries of a matrix with the entries of the identity matrix.
	//#
	//# The return value is a reference to the matrix object.
	//
	//# \also	$@Matrix4D::Set@$


	//# \function	Matrix4D::GetRow		Returns a $@Vector4D@$ object containing the entries of a particular row.
	//
	//# \proto	Antivector4D GetRow(int32 i) const;
	//
	//# \param	i	The index of the row. This must be 0, 1, 2, or 3.
	//
	//# \desc
	//# The $GetRow$ function returns an antivector containing the four entries in the row specified by
	//# the <i>i</i> parameter. Since matrices are stored in column-major order, this function must
	//# copy the entries into a new $@Antivector4D@$ object. A reference to a column of a matrix, requiring
	//# no copy, can be retrieved using the $[]$ operator.
	//
	//# \also	$@Matrix4D::SetRow@$


	//# \function	Matrix4D::SetRow		Sets the entries of a particular row to the values of a $@Vector4D@$ object.
	//
	//# \proto	Matrix4D& SetRow(int32 i, const Antivector4D& row);
	//
	//# \param	i		The index of the row. This must be 0, 1, 2, or 3.
	//# \param	row		A 4D antivector containing the new values to be stored in the row.
	//
	//# \desc
	//# The $SetRow$ function sets the four entries in the row specified by the <i>i</i> parameter to the
	//# values contained in the 4D antivector specified by the $row$ parameter.
	//
	//# \also	$@Matrix4D::GetRow@$


	class Matrix4D
	{
		friend class Transform4D;

		protected:

			alignas(16) float	n[4][4];

		public:

			Matrix4D() = default;

			C4API Matrix4D(const Vector4D& c1, const Vector4D& c2, const Vector4D& c3, const Vector4D& c4);
			C4API Matrix4D(float n00, float n01, float n02, float n03, float n10, float n11, float n12, float n13, float n20, float n21, float n22, float n23, float n30, float n31, float n32, float n33);

			C4API Matrix4D& Set(const Vector4D& c1, const Vector4D& c2, const Vector4D& c3, const Vector4D& c4);
			C4API Matrix4D& Set(float n00, float n01, float n02, float n03, float n10, float n11, float n12, float n13, float n20, float n21, float n22, float n23, float n30, float n31, float n32, float n33);

			float& operator ()(int32 i, int32 j)
			{
				return (n[j][i]);
			}

			const float& operator ()(int32 i, int32 j) const
			{
				return (n[j][i]);
			}

			Vector4D& operator [](machine j)
			{
				return (*reinterpret_cast<Vector4D *>(n[j]));
			}

			const Vector4D& operator [](machine j) const
			{
				return (*reinterpret_cast<const Vector4D *>(n[j]));
			}

			MatrixRow4D& GetRow(int32 i)
			{
				return (*reinterpret_cast<MatrixRow4D *>(&n[0][i]));
			}

			const MatrixRow4D& GetRow(int32 i) const
			{
				return (*reinterpret_cast<const MatrixRow4D *>(&n[0][i]));
			}

			Matrix4D& SetRow(int32 i, const Antivector3D& row)
			{
				n[0][i] = row.x;
				n[1][i] = row.y;
				n[2][i] = row.z;
				n[3][i] = 0.0F;
				return (*this);
			}

			Matrix4D& SetRow(int32 i, const Antivector4D& row)
			{
				n[0][i] = row.x;
				n[1][i] = row.y;
				n[2][i] = row.z;
				n[3][i] = row.w;
				return (*this);
			}

			#if C4SIMD

				Matrix4D& operator =(const Matrix4D& m)
				{
					VecStore(VecLoad(&m.n[0][0], 0), &n[0][0], 0);
					VecStore(VecLoad(&m.n[0][0], 4), &n[0][0], 4);
					VecStore(VecLoad(&m.n[0][0], 8), &n[0][0], 8);
					VecStore(VecLoad(&m.n[0][0], 12), &n[0][0], 12);
					return (*this);
				}

			#else

				C4API Matrix4D& operator =(const Matrix4D& m);

			#endif

			C4API Matrix4D& operator *=(const Matrix4D& m);
			C4API Matrix4D& operator *=(const Matrix3D& m);

			C4API Matrix4D& SetIdentity(void);

			friend C4API Matrix4D operator *(const Matrix4D& m1, const Matrix4D& m2);
			friend C4API Matrix4D operator *(const Matrix4D& m1, const Matrix3D& m2);
			friend C4API Vector4D operator *(const Matrix4D& m, const Vector4D& v);
			friend C4API Antivector4D operator *(const Antivector4D& v, const Matrix4D& m);
			friend C4API Vector4D operator *(const Matrix4D& m, const Vector3D& v);
			friend C4API Vector4D operator *(const Vector3D& v, const Matrix4D& m);
			friend C4API Vector4D operator *(const Matrix4D& m, const Point3D& p);
			friend C4API Vector4D operator *(const Point3D& p, const Matrix4D& m);
			friend C4API Vector4D operator *(const Matrix4D& m, const Vector2D& v);
			friend C4API Vector4D operator *(const Vector2D& v, const Matrix4D& m);
			friend C4API Vector4D operator *(const Matrix4D& m, const Point2D& p);
			friend C4API Vector4D operator *(const Point2D& p, const Matrix4D& m);
			friend C4API bool operator ==(const Matrix4D& m1, const Matrix4D& m2);
			friend C4API bool operator !=(const Matrix4D& m1, const Matrix4D& m2);

			friend C4API Matrix4D operator *(const Matrix4D& m1, const Transform4D& m2);
			friend C4API Matrix4D operator *(const Transform4D& m1, const Matrix4D& m2);
	};


	C4API float Determinant(const Matrix4D& m);
	C4API Matrix4D Inverse(const Matrix4D& m);
	C4API Matrix4D Adjugate(const Matrix4D& m);
	C4API Matrix4D Transpose(const Matrix4D& m);


	//# \class	Transform4D		Encapsulates a 4&nbsp;&times;&nbsp;4 matrix whose fourth row is always (0,&nbsp;0,&nbsp;0,&nbsp;1).
	//
	//# The $Transform4D$ class encapsulates a 4&nbsp;&times;&nbsp;4 matrix whose fourth row is always (0,&nbsp;0,&nbsp;0,&nbsp;1).
	//
	//# \def	class Transform4D : public Matrix4D
	//
	//# \ctor	Transform4D();
	//# \ctor	Transform4D(const Matrix3D& m);
	//# \ctor	Transform4D(const Matrix3D& m, Vector3D& v);
	//# \ctor	Transform4D(const Vector3D& c1, const Vector3D& c2, const Vector3D& c3, const Point3D& c4);
	//# \ctor	Transform4D(float n00, float n01, float n02, float n03, float n10, float n11, float n12, float n13,
	//# \ctor2	float n20, float n21, float n22, float n23);
	//
	//# \param	m		A 3&nbsp;&times;&nbsp;3 matrix that is copied to the upper-left 3&nbsp;&times;&nbsp;3 portion of the matrix.
	//# \param	v		A 3D vector that is copied to the fourth column of the matrix.
	//# \param	c1		The values of the entries residing in first column.
	//# \param	c2		The values of the entries residing in second column.
	//# \param	c3		The values of the entries residing in third column.
	//# \param	c4		The values of the entries residing in fourth column.
	//# \param	nij		The value of the entry residing in row <i>i</i> and column <i>j</i>.
	//
	//# \desc
	//# The $Transform4D$ class is used to store a 4&nbsp;&times;&nbsp;4 matrix whose fourth row is always (0,&nbsp;0,&nbsp;0,&nbsp;1).
	//#
	//# The default constructor leaves the entries of the matrix undefined, including the fourth row. All other constructors set
	//# the fourth row to (0,&nbsp;0,&nbsp;0,&nbsp;1).
	//
	//# \operator	Vector3D& operator [](machine j);
	//#				Returns a reference to the <i>j</i>-th column of a matrix. $j$ must be 0, 1, or 2.
	//#				Use the $@Transform4D::GetTranslation@$ and $@Transform4D::SetTranslation@$ functions to access the fourth column.
	//
	//# \operator	const Vector3D& operator [](machine j) const;
	//#				Returns a constant reference to the <i>j</i>-th column of a matrix. $j$ must be 0, 1, or 2.
	//#				Use the $@Transform4D::GetTranslation@$ and $@Transform4D::SetTranslation@$ functions to access the fourth column.
	//
	//# \operator	Transform4D& operator *=(const Transform4D& m);
	//#				Multiplies by the matrix $m$.
	//
	//# \operator	Transform4D& operator *=(const Matrix3D& m);
	//#				Multiplies by the matrix $m$. The entries of the fourth row and fourth column of $m$ are assumed
	//#				to be those of the identity matrix.
	//
	//# \action		Transform4D operator *(const Transform4D& m1, const Transform4D& m2);
	//#				Returns the product of the matrices $m1$ and $m2$.
	//
	//# \action		Transform4D operator *(const Transform4D& m1, const Matrix4D& m2);
	//#				Returns the product of the matrices $m1$ and $m2$.
	//
	//# \action		Transform4D operator *(const Transform4D& m1, const Matrix3D& m2);
	//#				Returns the product of the matrices $m1$ and $m2$. The entries of the fourth row and fourth column of $m2$
	//#				are assumed to be those of the identity matrix.
	//
	//# \action		Vector4D operator *(const Transform4D& m, const Vector4D& v);
	//#				Returns the product of the matrix $m$ and the column vector $v$.
	//
	//# \action		Vector4D operator *(const Antivector4D& v, const Transform4D& m);
	//#				Returns the product of the antivector $v$ and the matrix $m$.
	//
	//# \action		Vector3D operator *(const Transform4D& m, const Vector3D& v);
	//#				Returns the product of the matrix $m$ and the column vector $v$. The <i>w</i> coordinate of $v$ is assumed to be 0.
	//
	//# \action		Antivector3D operator *(const Antivector3D& v, const Transform4D& m);
	//#				Returns the product of the antivector $v$ and the matrix $m$. The <i>w</i> coordinate of $v$ is assumed to be 0.
	//
	//# \action		Point3D operator *(const Transform4D& m, const Point3D& p);
	//#				Returns the product of the matrix $m$ and the column vector $p$. The <i>w</i> coordinate of $p$ is assumed to be 1.
	//
	//# \action		Vector2D operator *(const Transform4D& m, const Vector2D& v);
	//#				Returns the product of the matrix $m$ and the column vector $v$. The <i>z</i> and <i>w</i> coordinates of $v$ are assumed to be 0.
	//
	//# \action		Point2D operator *(const Transform4D& m, const Point2D& p);
	//#				Returns the product of the matrix $m$ and the column vector $p$. The <i>z</i> coordinate of $p$ is assumed to be 0,
	//#				and the <i>w</i> coordinate of $p$ is assumed to be 1.
	//
	//# \action		float Determinant(const Transform4D& m);
	//#				Returns the determinant of the matrix $m$.
	//
	//# \action		Transform4D Inverse(const Transform4D& m);
	//#				Returns the inverse of the matrix $m$. If $m$ is singular, then the result is undefined.
	//
	//# \action		Transform4D Adjugate(const Transform4D& m);
	//#				Returns the adjugate of the matrix $m$.
	//
	//# \action		Vector3D InverseTransform(const Transform4D& m, const Vector3D& v);
	//#				Returns the product of the inverse of the matrix $m$ and the vector $v$. If $m$ is singular, then the result is undefined.
	//
	//# \action		Point3D InverseTransform(const Transform4D& m, const Point3D& p);
	//#				Returns the product of the inverse of the matrix $m$ and the point $p$. If $m$ is singular, then the result is undefined.
	//
	//# \action		Vector3D AdjugateTransform(const Transform4D& m, const Vector3D& v);
	//#				Returns the product of the adjugate of the matrix $m$ and the vector $v$.
	//
	//# \action		Point3D AdjugateTransform(const Transform4D& m, const Point3D& p);
	//#				Returns the product of the adjugate of the matrix $m$ and the point $p$.
	//
	//# \base	Matrix4D	A $Transform4D$ object behaves much like a $@Matrix4D@$ object,
	//#						except that the fourth row is always (0,&nbsp;0,&nbsp;0,&nbsp;1).
	//
	//# \also	$@Matrix4D@$
	//# \also	$@Matrix3D@$
	//# \also	$@Vector3D@$
	//# \also	$@Point3D@$


	//# \function	Transform4D::GetMatrix3D		Returns the upper-left 3&nbsp;&times;&nbsp;3 portion of a matrix.
	//
	//# \proto	Matrix3D GetMatrix3D(void) const;
	//
	//# \desc
	//# The $GetMatrix3D$ function returns the upper-left 3&nbsp;&times;&nbsp;3 portion of a matrix as a $@Matrix3D@$ object.
	//
	//# \also	$@Transform4D::SetMatrix3D@$
	//# \also	$@Transform4D::GetTranslation@$
	//# \also	$@Transform4D::SetTranslation@$


	//# \function	Transform4D::SetMatrix3D		Sets the entries of the upper-left 3&nbsp;&times;&nbsp;3 portion of a matrix.
	//
	//# \proto	Transform4D& SetMatrix3D(const Matrix3D& m);
	//# \proto	Transform4D& SetMatrix3D(const Vector3D& c1, const Vector3D& c2, const Vector3D& c3);
	//# \proto	Transform4D& SetMatrix3D(float n00, float n01, float n02, float n10, float n11, float n12, float n20, float n21, float n22);
	//
	//# \param	m		A 3&nbsp;&times;&nbsp;3 matrix that is copied to the upper-left 3&nbsp;&times;&nbsp;3 portion of the matrix.
	//# \param	c1		The new values of the entries residing in first column.
	//# \param	c2		The new values of the entries residing in second column.
	//# \param	c3		The new values of the entries residing in third column.
	//# \param	nij		The new value of the entry residing in row <i>i</i> and column <i>j</i>.
	//
	//# \desc
	//# The $SetMatrix3D$ function sets the entries of the upper-left 3&nbsp;&times;&nbsp;3 portion of a matrix.
	//# The fourth row and fourth column of the matrix are not modified.
	//
	//# \also	$@Transform4D::GetMatrix3D@$
	//# \also	$@Transform4D::GetTranslation@$
	//# \also	$@Transform4D::SetTranslation@$


	//# \function	Transform4D::GetTranslation		Returns the fourth column of a matrix.
	//
	//# \proto	const Point3D& GetTranslation(void) const;
	//
	//# \desc
	//# The $GetTranslation$ function returns a reference to the fourth column of a matrix as a $@Point3D@$ object.
	//
	//# \also	$@Transform4D::SetTranslation@$
	//# \also	$@Transform4D::GetMatrix3D@$
	//# \also	$@Transform4D::SetMatrix3D@$


	//# \function	Transform4D::SetTranslation		Sets the fourth column of a matrix.
	//
	//# \proto	Transform4D& SetTranslation(const Point3D& p);
	//# \proto	Transform4D& SetTranslation(float x, float y, float z);
	//
	//# \param	p	The 3D point whose entries are copied into the fourth column.
	//# \param	x	The <i>x</i> coordinate of the translation.
	//# \param	y	The <i>y</i> coordinate of the translation.
	//# \param	z	The <i>z</i> coordinate of the translation.
	//
	//# \desc
	//# The $SetTranslation$ function sets the fourth column of a matrix to the position given by the $p$ parameter.
	//# The first three columns and the fourth row of the matrix are not modified.
	//#
	//# The return value is a reference to the matrix object.


	//# \function	Transform4D::SetRotationAboutX		Sets a matrix to represent a rotation about the <i>x</i> axis.
	//
	//# \proto	Transform4D& SetRotationAboutX(float angle);
	//
	//# \param	angle	The angle through which to rotate, in radians.
	//
	//# \desc
	//# The $SetRotationAboutX$ function replaces all entries of a matrix with those representing a
	//# rotation about the <i>x</i> axis through the angle given by the $angle$ parameter.
	//# The entries of the fourth row and fourth column of the matrix are set to those of the identity matrix.
	//#
	//# The return value is a reference to the matrix object.
	//
	//# \also	$@Transform4D::SetRotationAboutY@$
	//# \also	$@Transform4D::SetRotationAboutZ@$
	//# \also	$@Transform4D::SetRotationAboutAxis@$


	//# \function	Transform4D::SetRotationAboutY		Sets a matrix to represent a rotation about the <i>y</i> axis.
	//
	//# \proto	Transform4D& SetRotationAboutY(float angle);
	//
	//# \param	angle	The angle through which to rotate, in radians.
	//
	//# \desc
	//# The $SetRotationAboutY$ function replaces all entries of a matrix with those representing a
	//# rotation about the <i>y</i> axis through the angle given by the $angle$ parameter.
	//# The entries of the fourth row and fourth column of the matrix are set to those of the identity matrix.
	//#
	//# The return value is a reference to the matrix object.
	//
	//# \also	$@Transform4D::SetRotationAboutX@$
	//# \also	$@Transform4D::SetRotationAboutZ@$
	//# \also	$@Transform4D::SetRotationAboutAxis@$


	//# \function	Transform4D::SetRotationAboutZ		Sets a matrix to represent a rotation about the <i>z</i> axis.
	//
	//# \proto	Transform4D& SetRotationAboutZ(float angle);
	//
	//# \param	angle	The angle through which to rotate, in radians.
	//
	//# \desc
	//# The $SetRotationAboutZ$ function replaces all entries of a matrix with those representing a
	//# rotation about the <i>z</i> axis through the angle given by the $angle$ parameter.
	//# The entries of the fourth row and fourth column of the matrix are set to those of the identity matrix.
	//#
	//# The return value is a reference to the matrix object.
	//
	//# \also	$@Transform4D::SetRotationAboutX@$
	//# \also	$@Transform4D::SetRotationAboutY@$
	//# \also	$@Transform4D::SetRotationAboutAxis@$


	//# \function	Transform4D::SetRotationAboutAxis		Sets a matrix to represent a rotation about a given axis.
	//
	//# \proto	Transform4D& SetRotationAboutAxis(float angle, const Antivector3D& axis);
	//
	//# \param	angle	The angle through which to rotate, in radians.
	//# \param	axis	The axis about which to rotate. This vector should have unit length.
	//
	//# \desc
	//# The $SetRotationAboutAxis$ function replaces all entries of a matrix with those representing a
	//# rotation about the axis given by the $axis$ parameter through the angle given by the $angle$ parameter.
	//# The entries of the fourth row and fourth column of the matrix are set to those of the identity matrix.
	//#
	//# The return value is a reference to the matrix object.
	//
	//# \also	$@Transform4D::SetRotationAboutX@$
	//# \also	$@Transform4D::SetRotationAboutY@$
	//# \also	$@Transform4D::SetRotationAboutZ@$


	//# \function	Transform4D::SetScale		Sets a matrix to represent a scale.
	//
	//# \proto	Transform4D& SetScale(float t);
	//# \proto	Transform4D& SetScale(float r, float s, float t);
	//
	//# \param	s	The scale along the <i>x</i> axis.
	//# \param	r	The scale along the <i>y</i> axis.
	//# \param	t	The scale along the <i>z</i> axis, or if specified by itself, the scale along all three axes.
	//
	//# \desc
	//# The $SetScale$ function replaces all entries of a matrix with those representing a scale.
	//# If only the $t$ parameter is specified, then the scale is uniform along all three axes.
	//# If the $r$, $s$, and $t$ parameters are specified, then they correspond to the scale along
	//# the <i>x</i>-, <i>y</i>, and <i>z</i> axis, respectively.
	//# The entries of the fourth row and fourth column of the matrix are set to those of the identity matrix.
	//#
	//# The return value is a reference to the matrix object.
	//
	//# \also	$@Matrix4D::SetIdentity@$


	class Transform4D : public Matrix4D
	{
		public:

			Transform4D() = default;

			C4API Transform4D(const Matrix3D& m);
			C4API Transform4D(const Matrix3D& m, const Vector3D& v);
			C4API Transform4D(const Vector3D& c1, const Vector3D& c2, const Vector3D& c3, const Point3D& c4);
			C4API Transform4D(float n00, float n01, float n02, float n03, float n10, float n11, float n12, float n13, float n20, float n21, float n22, float n23);

			C4API Transform4D& Set(const Matrix3D& m, const Vector3D& v);
			C4API Transform4D& Set(const Vector3D& c1, const Vector3D& c2, const Vector3D& c3, const Point3D& c4);
			C4API Transform4D& Set(float n00, float n01, float n02, float n03, float n10, float n11, float n12, float n13, float n20, float n21, float n22, float n23);

			Vector3D& operator [](machine j)
			{
				return (*reinterpret_cast<Vector3D *>(n[j]));
			}

			const Vector3D& operator [](machine j) const
			{
				return (*reinterpret_cast<const Vector3D *>(n[j]));
			}

			const Point3D& GetTranslation(void) const
			{
				return (*reinterpret_cast<const Point3D *>(n[3]));
			}

			Transform4D& SetTranslation(const Point3D& p)
			{
				n[3][0] = p.x;
				n[3][1] = p.y;
				n[3][2] = p.z;
				return (*this);
			}

			Transform4D& SetTranslation(float x, float y, float z)
			{
				n[3][0] = x;
				n[3][1] = y;
				n[3][2] = z;
				return (*this);
			}

			C4API Transform4D& operator *=(const Transform4D& m);
			C4API Transform4D& operator *=(const Matrix3D& m);

			C4API Matrix3D GetMatrix3D(void) const;
			C4API Transform4D& SetMatrix3D(const Matrix3D& m);
			C4API Transform4D& SetMatrix3D(const Transform4D& m);
			C4API Transform4D& SetMatrix3D(const Vector3D& c1, const Vector3D& c2, const Vector3D& c3);
			C4API Transform4D& SetMatrix3D(float n00, float n01, float n02, float n10, float n11, float n12, float n20, float n21, float n22);

			C4API Transform4D& SetRotationAboutX(float angle);
			C4API Transform4D& SetRotationAboutY(float angle);
			C4API Transform4D& SetRotationAboutZ(float angle);
			C4API Transform4D& SetRotationAboutAxis(float angle, const Antivector3D& axis);

			C4API Transform4D& SetPlaneReflection(const Antivector4D& plane);
			C4API Transform4D& SetPlaneReflection(const MatrixRow4D& plane);

			C4API void GetEulerAngles(float *x, float *y, float *z) const;
			C4API Transform4D& SetEulerAngles(float x, float y, float z);

			C4API Transform4D& SetScale(float t);
			C4API Transform4D& SetScale(float r, float s, float t);

			C4API Transform4D& SetDisplacement(const Vector3D& dv);

			C4API Transform4D& Normalize(void);

			friend C4API Transform4D operator *(const Transform4D& m, float t);
			friend C4API Transform4D operator &(const Transform4D& m, const Vector3D& v);

			friend C4API Transform4D operator *(const Transform4D& m1, const Transform4D& m2);
			friend C4API Matrix4D operator *(const Transform4D& m1, const Matrix4D& m2);
			friend C4API Transform4D operator *(const Transform4D& m1, const Matrix3D& m2);
			friend C4API Transform4D operator *(const Matrix3D& m1, const Transform4D& m2);
			friend C4API Vector4D operator *(const Transform4D& m, const Vector4D& v);
			friend C4API Antivector4D operator *(const Antivector4D& v, const Transform4D& m);
			friend C4API Vector3D operator *(const Transform4D& m, const Vector3D& v);
			friend C4API Antivector3D operator *(const Antivector3D& v, const Transform4D& m);
			friend C4API Point3D operator *(const Transform4D& m, const Point3D& p);
			friend C4API Vector2D operator *(const Transform4D& m, const Vector2D& v);
			friend C4API Point2D operator *(const Transform4D& m, const Point2D& p);

			friend C4API Matrix3D Transform(const Transform4D& m1, const Matrix3D& m2);
			friend C4API Matrix3D TransformTranspose(const Transform4D& m1, const Matrix3D& m2);
			friend C4API Transform4D TransposeTransform(const Matrix3D& m1, const Transform4D& m2);

			#if C4SIMD

				friend C4API vec_float TransformVector3D(const Transform4D& m, vec_float v);
				friend C4API vec_float TransformPoint3D(const Transform4D& m, vec_float p);

			#endif
	};


	struct ConstMatrix4D
	{
		alignas(16) float	n[4][4];

		operator const Matrix4D&(void) const
		{
			return (reinterpret_cast<const Matrix4D&>(*this));
		}

		const Matrix4D *operator &(void) const
		{
			return (reinterpret_cast<const Matrix4D *>(this));
		}

		const Matrix4D *operator ->(void) const
		{
			return (reinterpret_cast<const Matrix4D *>(this));
		}

		float operator ()(int32 i, int32 j) const
		{
			return (reinterpret_cast<const Matrix4D&>(*this)(i, j));
		}

		const Vector4D& operator [](machine j) const
		{
			return (reinterpret_cast<const Matrix4D&>(*this)[j]);
		}
	};


	struct ConstTransform4D
	{
		alignas(16) float	n[4][4];

		operator const Transform4D&(void) const
		{
			return (reinterpret_cast<const Transform4D&>(*this));
		}

		const Transform4D *operator &(void) const
		{
			return (reinterpret_cast<const Transform4D *>(this));
		}

		const Transform4D *operator ->(void) const
		{
			return (reinterpret_cast<const Transform4D *>(this));
		}

		float operator ()(int32 i, int32 j) const
		{
			return (reinterpret_cast<const Transform4D&>(*this)(i, j));
		}

		const Vector3D& operator [](machine j) const
		{
			return (reinterpret_cast<const Transform4D&>(*this)[j]);
		}
	};


	C4API extern const ConstTransform4D Identity4D;


	#if C4SIMD

		C4API vec_float TransformVector3D(const Transform4D& m, vec_float v);
		C4API vec_float TransformPoint3D(const Transform4D& m, vec_float p);

	#endif

	C4API float Determinant(const Transform4D& m);
	C4API Transform4D Inverse(const Transform4D& m);
	C4API Transform4D Adjugate(const Transform4D& m);
	C4API Matrix3D Adjugate3D(const Transform4D& m);
	C4API Vector3D InverseTransform(const Transform4D& m, const Vector3D& v);
	C4API Point3D InverseTransform(const Transform4D& m, const Point3D& p);
	C4API Vector3D AdjugateTransform(const Transform4D& m, const Vector3D& v);
	C4API Point3D AdjugateTransform(const Transform4D& m, const Point3D& p);
}


#endif

// ZYUQURM
