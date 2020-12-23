 

#ifndef C4Computation_h
#define C4Computation_h


//# \component	Math Library
//# \prefix		Math/


#include "C4Types.h"


namespace C4
{
	//# \enum	PolygonLocation

	enum
	{
		kPolygonInterior		= 1,		//## The point lies in the interior of the polygon.
		kPolygonBoundary		= 0,		//## The point lies on or very near the boundary of the polygon.
		kPolygonExterior		= -1		//## The point lies outside the polygon.
	};


	struct Ray
	{
		Point3D			origin;
		Vector3D		direction;
		float			radius;
		float			tmin;
		float			tmax;
	};


	//# \namespace	Math	Contains miscellaneous mathematical functions.
	//
	//# The $Math$ namespace contains miscellaneous mathematical functions.
	//
	//# \def	namespace C4 { namespace Math {...} }
	//
	//# \also	$@C4@$


	//# \function	Math::GetTrigTable		Returns a pointer to a table of precomputed cosine and sine values.
	//
	//# \proto	const ConstVector2D *GetTrigTable(void);
	//
	//# \desc
	//# The $GetTrigTable$ function returns a pointer to a table of 256 precomputed cosine and sine values.
	//# Entry <i>k</i> in the table corresponds to the angle (<i>k</i>/255)&middot;360 degrees. Each entry in
	//# the table is a 2D vector whose <i>x</i> coordinate holds the cosine of the angle and whose <i>y</i>
	//# coordinate holds the sine of the angle.
	//
	//# \also	$@C4::CosSin@$


	//# \div
	//# \function	Math::DistancePointToLine		Calculates the distance from a point to a line.
	//
	//# \proto	float DistancePointToLine(const Point3D& q, const Point3D& s, const Vector3D& v);
	//
	//# \param	q	The point <b>Q</b>.
	//# \param	s	The origin <b>S</b> of the line.
	//# \param	v	The direction <b>V</b> of the line.
	//
	//# \desc
	//# The $DistancePointToLine$ function returns the perpendicular distance from the point <b>Q</b> given by
	//# the $q$ parameter to the line <b>S</b>&nbsp;+&nbsp;<i>t</i>&nbsp;&middot;&nbsp;<b>V</b>
	//# given by the $s$ and $v$ parameters.
	//
	//# \also	$@Math::DistanceLineToLine@$
	//# \also	$@Math::SquaredDistancePointToLine@$
	//# \also	$@Math::SquaredDistanceLineToLine@$


	//# \function	Math::DistanceLineToLine		Calculates the distance between two lines.
	//
	//# \proto	float DistanceLineToLine(const Point3D& s1, const Vector3D& v1, const Point3D& s2, const Vector3D& v2);
	//
	//# \param	s1	The origin <b>S</b><sub>1</sub> of the first line.
	//# \param	v1	The direction <b>V</b><sub>1</sub> of the first line.
	//# \param	s2	The origin <b>S</b><sub>2</sub> of the second line.
	//# \param	v2	The direction <b>V</b><sub>2</sub> of the second line.
	//
	//# \desc
	//# The $DistanceLineToLine$ function returns the distance of closest approach between the two lines defined by
	//# <b>S</b><sub>1</sub>&nbsp;+&nbsp;<i>t</i>&nbsp;&middot;&nbsp;<b>V</b><sub>1</sub> and
	//# <b>S</b><sub>2</sub>&nbsp;+&nbsp;<i>t</i>&nbsp;&middot;&nbsp;<b>V</b><sub>2</sub>. If the lines are
	//# parallel, then the perpendicular distance between the lines is returned.
	//
	//# \also	$@Math::DistancePointToLine@$
	//# \also	$@Math::SquaredDistanceLineToLine@$
	//# \also	$@Math::SquaredDistancePointToLine@$


	//# \function	Math::SquaredDistancePointToLine		Calculates the squared distance from a point to a line.
	//
	//# \proto	float SquaredDistancePointToLine(const Point3D& q, const Point3D& s, const Vector3D& v);
	//
	//# \param	q	The point <b>Q</b>.
	//# \param	s	The origin <b>S</b> of the line.
	//# \param	v	The direction <b>V</b> of the line. 
	//
	//# \desc
	//# The $SquaredDistancePointToLine$ function returns the squared perpendicular distance from the point <b>Q</b> given by 
	//# the $q$ parameter to the line <b>S</b>&nbsp;+&nbsp;<i>t</i>&nbsp;&middot;&nbsp;<b>V</b>
	//# given by the $s$ and $v$ parameters. 
	//
	//# \also	$@Math::SquaredDistanceLineToLine@$
	//# \also	$@Math::DistancePointToLine@$ 
	//# \also	$@Math::DistanceLineToLine@$
 
 
	//# \function	Math::SquaredDistanceLineToLine		Calculates the squared distance between two lines.
	//
	//# \proto	float SquaredDistanceLineToLine(const Point3D& s1, const Vector3D& v1, const Point3D& s2, const Vector3D& v2);
	// 
	//# \param	s1	The origin <b>S</b><sub>1</sub> of the first line.
	//# \param	v1	The direction <b>V</b><sub>1</sub> of the first line.
	//# \param	s2	The origin <b>S</b><sub>2</sub> of the second line.
	//# \param	v2	The direction <b>V</b><sub>2</sub> of the second line.
	//
	//# \desc
	//# The $SquaredDistanceLineToLine$ function returns the squared distance of closest approach between the two lines defined by
	//# <b>S</b><sub>1</sub>&nbsp;+&nbsp;<i>t</i>&nbsp;&middot;&nbsp;<b>V</b><sub>1</sub> and
	//# <b>S</b><sub>2</sub>&nbsp;+&nbsp;<i>t</i>&nbsp;&middot;&nbsp;<b>V</b><sub>2</sub>. If the lines are
	//# parallel, then the squared perpendicular distance between the lines is returned.
	//
	//# \also	$@Math::DistanceLineToLine@$
	//# \also	$@Math::DistancePointToLine@$
	//# \also	$@Math::SquaredDistancePointToLine@$


	//# \div
	//# \function	Math::SolveLinearSystem		Solves the <i>n</i>&nbsp;&times;&nbsp;<i>n</i> linear system
	//#											<b>Mx</b>&nbsp;=&nbsp;<b>r</b>.
	//
	//# \proto	bool SolveLinearSystem(int32 n, float *restrict m, float *restrict r);
	//
	//# \param	n	The size of the <i>n</i>&nbsp;&times;&nbsp;<i>n</i> coefficient matrix <b>M</b>.
	//# \param	m	A pointer to the <i>n</i><SUP>2</SUP> entries of the coefficient matrix <b>M</b>,
	//#				stored in column-major order. These entries are modified by this function.
	//# \param	r	A pointer to the constant vector <b>r</b>. The solution vector <b>x</b>
	//#				is returned in this array.
	//
	//# \desc
	//# The $SolveLinearSystem$ function solves the linear system <b>Mx</b>&nbsp;=&nbsp;<b>r</b>
	//# using Guassian elimination with implicit pivoting. If the system can be solved, the
	//# return value is $true$, but if the matrix <b>M</b> is singular, the return value is $false$.
	//#
	//# The entries of the matrix <b>M</b> and the vector <b>r</b> are transformed in place by
	//# the $SolveLinearSystem$ function. If these values are still needed after solving the
	//# linear system, then they should be copied before calling $SolveLinearSystem$.
	//#
	//# The solution vector <b>x</b> is returned in the array pointed to by the $r$ parameter.
	//
	//# \also	$@Math::LUDecompose@$
	//# \also	$@Math::LUBacksubstitute@$
	//# \also	$@Math::SolveTridiagonalSystem@$


	//# \function	Math::LUDecompose		Performs the LU decomposition of an <i>n</i>&nbsp;&times;&nbsp;<i>n</i> matrix <b>M</b>.
	//
	//# \proto	bool LUDecompose(int32 n, float *restrict m, unsigned_int16 *restrict index, float *restrict detSign = nullptr);
	//
	//# \param	n			The size of the <i>n</i>&nbsp;&times;&nbsp;<i>n</i> matrix <b>M</b>.
	//# \param	m			A pointer to the <i>n</i><SUP>2</SUP> entries of the matrix <b>M</b>,
	//#						stored in column-major order. The LU-decomposed matrix entries are returned in this array.
	//# \param	index		A pointer to an array of size <i>n</i> where the row permutation information can be stored.
	//# \param	detSign		A pointer to a location where the parity of the row exchanges can be stored.
	//#						This parameter may be $nullptr$ if this information is not needed.
	//
	//# \desc
	//# The $LUDecompose$ function performs the LU decomposition of an <i>n</i>&nbsp;&times;&nbsp;<i>n</i> matrix <b>M</b>.
	//# The decomposition is performed in place&mdash;the decomposed matrix entries are returned in the space
	//# occupied by <b>M</b>. This function also returns an array of indexes that indicate how the rows were
	//# permuted during the decomposition process. The decomposed matrix and the permutation array are passed to
	//# the $@Math::LUBacksubstitute@$ function to solve linear systems.
	//#
	//# The $LUDecompose$ function returns $false$ if the matrix <b>M</b> is singular and $true$ otherwise.
	//
	//# \also	$@Math::LUBacksubstitute@$
	//# \also	$@Math::LURefineSolution@$


	//# \function	Math::LUBacksubstitute		Uses an LU-decomposed matrix to solve a linear system.
	//
	//# \proto	void LUBacksubstitute(int32 n, const float *d, const unsigned_int16 *index, const float *r, float *restrict x);
	//
	//# \param	n		The size of the LU-decomposed matrix.
	//# \param	d		A pointer to the entries of the LU-decomposed matrix. This should be the same pointer that
	//#					was previously passed to the $m$ parameter of the $@Math::LUDecompose@$ function.
	//# \param	index	A pointer to the array of row permutation indexes returned by the $@Math::LUDecompose@$ function.
	//# \param	r		A pointer to an array of <i>n</i> constant values representing the vector <b>r</b> for which
	//#					the linear system <b>Mx</b>&nbsp;=&nbsp;<b>r</b> is to be solved.
	//# \param	x		A pointer to the array in which the <i>n</i> solutions representing the vector <b>x</b>
	//#					are to be returned. This may <i>not</i> be the same pointer passed as the $r$ parameter.
	//
	//# \desc
	//# The $LUBacksubstitute$ function is used in conjunction with the $@Math::LUDecompose@$ function to solve
	//# a linear system of <i>n</i> equations <b>Mx</b>&nbsp;=&nbsp;<b>r</b>. The $@Math::LUDecompose@$ function
	//# is called once to calculate the LU decomposition of the coefficient matrix <b>M</b>, and the results are
	//# passed to the $LUBacksubstitute$ function any number of times with different constant vectors <b>r</b>
	//# to solve multiple linear systems.
	//
	//# \also	$@Math::LUDecompose@$
	//# \also	$@Math::LURefineSolution@$


	//# \function	Math::LURefineSolution		Refines the numerical solution to a linear system previously calculated by LU decomposition.
	//
	//# \proto	void LURefineSolution(int32 n, const float *m, const float *d, const unsigned_int16 *index, const float *r, float *restrict x);
	//
	//# \param	n		The size of the coefficient matrix <b>M</b>.
	//# \param	m		A pointer to the <i>n</i><SUP>2</SUP> entries of the coefficient matrix <b>M</b>,
	//#					stored in column-major order.
	//# \param	d		A pointer to the entries of the LU decomposition of <b>M</b>. This should be the same pointer that
	//#					was previously passed to the $m$ parameter of the $@Math::LUDecompose@$ function.
	//# \param	index	A pointer to the array of row permutation indexes returned by the $@Math::LUDecompose@$ function.
	//# \param	r		A pointer to an array of <i>n</i> constant values representing the vector <b>r</b> for which
	//#					the linear system <b>Mx</b>&nbsp;=&nbsp;<b>r</b> was originally solved.
	//# \param	x		A pointer to the array containing the <i>n</i> solutions representing the vector <b>x</b>.
	//#					These solutions are refined in place by this function.
	//
	//# \desc
	//# The $LURefineSolution$ function improves the numerical solution <b>x</b> to the linear system
	//# <b>Mx</b>&nbsp;=&nbsp;<b>r</b>. This solution must have been previously calculated using the
	//# $@Math::LUDecompose@$ and $@Math::LUBacksubstitute@$ functions.
	//
	//# \also	$@Math::LUDecompose@$
	//# \also	$@Math::LUBacksubstitute@$


	//# \function	Math::SolveTridiagonalSystem		Solves a linear system having a tridiagonal coefficient matrix.
	//
	//# \proto	void SolveTridiagonalSystem(int32 n, const float *a, const float *b, const float *c, const float *r, float *restrict x);
	//
	//# \param	n	The size of the tridiagonal matrix <b>M</b>.
	//# \param	a	A pointer to the array containing the <i>n</i> subdiagonal entries of <b>M</b>.
	//#				The coefficient $a[0]$ does not exist and is never accessed.
	//# \param	b	A pointer to the array containing the <i>n</i> diagonal entries of <b>M</b>.
	//# \param	c	A pointer to the array containing the <i>n</i> superdiagonal entries of <b>M</b>.
	//#				The coefficient $c[n - 1]$ does not exist and is never accessed.
	//# \param	r	A pointer to an array of <i>n</i> constant values representing the vector <b>r</b> for which
	//#				the linear system <b>Mx</b>&nbsp;=&nbsp;<b>r</b> is to be solved.
	//# \param	x	A pointer to the array in which the <i>n</i> solutions representing the vector <b>x</b> are to be returned.
	//#				This may be the same pointer passed as the $r$ parameter.
	//
	//# \desc
	//# The $SolveTridiagonalSystem$ function solves a linear system <b>Mx</b>&nbsp;=&nbsp;<b>r</b> for which the
	//# coefficient matrix <b>M</b> is tridiagonal. The matrix <b>M</b> must be diagonally dominant to guarantee
	//# a valid solution.
	//#
	//# The entries of the matrix <b>M</b> are specified as three arrays of size <i>n</i> containing the subdiagonal,
	//# diagonal, and superdiagonal entries arranged as follows.
	//#
	//# <table cellspacing="0" cellpadding="0"><tr>
	//#		<td style="padding: 0px 6px 2px 6px; vertical-align: middle; text-align: center;"><b>M</b>&nbsp;=&nbsp;</td>
	//#		<td style="border-left: solid 1px black; border-top: solid 1px black; border-bottom: solid 1px black;">&nbsp;</td>
	//#		<td style="padding: 0px 6px 2px 6px; vertical-align: middle; text-align: center;">$b[0]$<br/>$a[1]$<br/>$0$<br/>$0$<br/><code>&#x22EE;</code></td>
	//#		<td style="padding: 0px 6px 2px 6px; vertical-align: middle; text-align: center;">$c[0]$<br/>$b[1]$<br/>$a[2]$<br/>$0$<br/><code>&#x22EE;</code></td>
	//#		<td style="padding: 0px 6px 2px 6px; vertical-align: middle; text-align: center;">$0$<br/>$c[1]$<br/>$b[2]$<br/>$a[3]$<br/><code>&#x22EE;</code></td>
	//#		<td style="padding: 0px 6px 2px 6px; vertical-align: middle; text-align: center;">$0$<br/>$0$<br/>$c[2]$<br/>$b[3]$<br/><code>&#x22EE;</code></td>
	//#		<td style="padding: 0px 6px 2px 6px; vertical-align: middle; text-align: center;"><code>&#x22EF;</code><br/><code>&#x22EF;</code><br/><code>&#x22EF;</code><br/><code>&#x22EF;</code><br/><code>&#x22F1;</code></td>
	//#		<td style="border-right: solid 1px black; border-top: solid 1px black; border-bottom: solid 1px black;">&nbsp;</td>
	//# </tr></table>
	//
	//# \also	$@Math::SolveLinearSystem@$


	//# \function	Math::CalculateEigensystem		Calculates eigenvalues and eigenvectors for a 3&nbsp;&times;&nbsp;3 symmetric matrix.
	//
	//# \proto	void CalculateEigensystem(const Matrix3D& m, Vector3D *eigenvalue, Matrix3D *eigenvector);
	//
	//# \param	m				The 3&nbsp;&times;&nbsp;3 matrix for which the eigensystem is calculated. This must a symmetric matrix.
	//# \param	eigenvalue		A pointer to a $@Vector3D@$ object in which the three eigenvalues are returned.
	//# \param	eigenvector		A pointer to a $@Matrix3D@$ object whose columns contain the three eigenvectors upon return.
	//
	//# \desc
	//# The $CalculateEigensystem$ function calculates eigenvalues and eigenvectors of the 3&nbsp;&times;&nbsp;3
	//# symmetric matrix given by the $m$ parameter. The three eigenvalues are returned in the three components of
	//# the $@Vector3D@$ object pointed to by the $eigenvalue$ parameter, and the three eigenvectors are returned
	//# in the columns of the $@Matrix3D@$ object pointed to by the $eigenvector$ parameter. The first, second, and
	//# third columns of the eigenvector matrix correspond to the <i>x</i>, <i>y</i>, and <i>z</i> coordinates of the
	//# eigenvalue vector, respectively.


	//# \function	Math::CalculatePrincipalAxes		Calculates the principal axes for an arbitrary set of points.
	//
	//# \proto	void CalculatePrincipalAxes(int32 vertexCount, const Point3D *vertex, Point3D *center, Vector3D *axis);
	//
	//# \param	vertexCount		The number of vertices.
	//# \param	vertex			An array of vertices whose size is given by the $vertexCount$ parameter.
	//# \param	center			A pointer to a location in which the average position of the vertices is returned.
	//# \param	axis			An array of three vectors in which the principal axes are returned.
	//
	//# \desc
	//# The $CalculatePrincipalAxes$ function uses principal component analysis to calculate the machine axes
	//# for an arbitrary set of points. The $vertexCount$ parameter specifies how many points are in the array
	//# specified by the $vertex$ parameter. The $@Point3D@$ object pointed to by the $center$ parameter receives
	//# the average position of all the vertices. The three principal axes (which are orthogonal) are returned in
	//# the array of $@Vector3D@$ objects specified by the $axis$ parameter. The primary axis is stored in
	//# $axis[0]$, the second most dominant axis is stored in $axis[1]$, and the least dominant axis is stored in $axis[2]$.


	//# \div
	//# \function	Math::ClipSegment		Clips a line segment against a plane.
	//
	//# \proto	bool ClipSegment(Point3D *p1, Point3D *p2, const Antivector4D& clippingPlane);
	//
	//# \param	p1				A pointer to the first endpoint of the line segment.
	//# \param	p2				A pointer to the second endpoint of the line segment.
	//# \param	clippingPlane	The plane against which the line segment is clipped.
	//
	//# \desc
	//# The $ClipSegment$ function clips the line segment specified by the $p1$ and $p2$ parameters against
	//# the plane specified by the $clippingPlane$ parameter. Any portion of the line segment that falls on the
	//# negative side of the clipping plane is removed, and the endpoints of the clipped segment are returned by replacing
	//# the values pointed to by the $p1$ and $p2$ parameters. If both endpoints lie on the negative side of the
	//# clipping plane, then this function returns $false$, and the endpoints are not modified. If either one of the
	//# endpoints lies on the positive side of the plane (or on the plane itself), then this function returns $true$.


	//# \function	Math::GetPolygonArea		Returns the signed area of a polygon.
	//
	//# \proto	float GetPolygonArea(int32 vertexCount, const Point3D *vertex, const Vector3D& normal);
	//
	//# \param	vertexCount		The number of vertices defining the boundary of the polygon. This must be at least 3.
	//# \param	vertex			An array containing the vertices of the polygon.
	//# \param	normal			The normal direction of the polygon.
	//
	//# \desc
	//# The $GetPolygonArea$ function returns the signed area of a polygon whose vertices are given by the
	//# $vertexCount$ and $vertex$ parameters. If the vertices are wound counterclockwise about the normal
	//# direction, then the returned area is positive. If the vertices are wound clockwise, then the returned
	//# area is negative. The polygon does not need to be convex.
	//
	//# \also	$@Math::ConvexPolygon@$
	//# \also	$@Math::PointInConvexPolygon@$


	//# \function	Math::ConvexPolygon		Returns a boolean value indicating whether a given polygon is convex.
	//
	//# \proto	bool ConvexPolygon(int32 vertexCount, const Point3D *vertex, const Vector3D& normal);
	//
	//# \param	vertexCount		The number of vertices defining the boundary of the polygon. This must be at least 3.
	//# \param	vertex			An array containing the vertices of the polygon, wound in counterclockwise order.
	//# \param	normal			The normal direction of the polygon.
	//
	//# \desc
	//# The $ConvexPolygon$ function returns $true$ if the polygon defined by the $vertexCount$ and $vertex$
	//# parameters is convex. If the polygon is concave or has three consecutive collinear vertices, then the
	//# return value is $false$.
	//
	//# \also	$@Math::PointInConvexPolygon@$
	//# \also	$@Math::GetPolygonArea@$


	//# \function	Math::PointInConvexPolygon		Returns a value indicating whether a point lies within the interior of a convex polygon, lies on its boundary, or lies outside the polygon.
	//
	//# \proto	int32 PointInConvexPolygon(const Point3D& p, int32 vertexCount, const Point3D *vertex, const Vector3D& normal);
	//
	//# \param	p				The point to test.
	//# \param	vertexCount		The number of vertices defining the boundary of the polygon. This must be at least 3.
	//# \param	vertex			An array containing the vertices of the polygon, wound in counterclockwise order.
	//# \param	normal			The normal direction of the polygon.
	//
	//# \desc
	//# The $PointInConvexPolygon$ function determines the location of the point given by the $p$ parameter with
	//# respect to the polygon specified by the $vertexCount$ and $vertex$ parameters. The return value is one
	//# of the following constants.
	//
	//# \table	PolygonLocation
	//
	//# The $vertex$ parameter should point to an array of $@Point3D@$ objects of size given by the $vertexCount$
	//# parameter. These vertices should be wound in counterclockwise order about the normal direction specified
	//# by the $normal$ parameter. The polygon must be convex and must lie in a plane that is perpendicular to
	//# the normal direction. If these requirements are not satisfied, then the result is undefined.
	//
	//# \also	$@Math::ConvexPolygon@$
	//# \also	$@Math::GetPolygonArea@$


	//# \function	Math::ClipPolygon		Clips a convex polygon against a plane.
	//
	//# \proto	int32 ClipPolygon(int32 vertexCount, const Point3D *vertex, const Antivector4D& clippingPlane, int8 *restrict location, Point3D *restrict result);
	//
	//# \param	vertexCount		The number of vertices defining the boundary of the polygon. This must be at least 3.
	//# \param	vertex			An array containing the vertices of the polygon, wound in counterclockwise order.
	//# \param	clippingPlane	The plane against which the polygon is clipped.
	//# \param	location		An array large enough to hold $vertexCount$ elements for internal use.
	//# \param	result			An array of points large enough to hold $vertexCount$&nbsp;+&nbsp;1 vertices where the
	//#							clipped polygon is to be returned.
	//
	//# \desc
	//# The $ClipPolygon$ function clips the polygon specified by the $vertexCount$ and $vertex$ parameters and clips
	//# it against the plane specified by the $clippingPlane$ parameter. Any portion of the polygon that lies on the
	//# negative side of the clipping plane is removed, and the resulting polygon is stored in the array pointed to by
	//# the $result$ parameter. The return value of this function is the number of vertices in the clipped polygon.
	//# If the polygon is completely clipped away, then the return value is 0.


	//# \function	Math::TriangulatePolygon		Triangulates a convex or concave polygon having genus zero (i.e., no holes).
	//
	//# \proto	int32 TriangulatePolygon(int32 vertexCount, const Point3D *vertex, const Vector3D& normal, Triangle *restrict triangle, int32 base = 0);
	//
	//# \param	vertexCount		The number of vertices used to specify the boundary of the polygon.
	//# \param	vertex			An array of vertices, wound in counterclockwise order, of the size specified by the
	//#							$vertexCount$ parameter.
	//# \param	normal			The normal direction of the polygon.
	//# \param	triangle		An array to which the triangles are written. This must be large enough to hold
	//#							<i>n</i>&nbsp;&minus;&nbsp;2 triangles, where <i>n</i> is the number of vertices specified
	//#							by the $vertexCount$ parameter.
	//# \param	base			A base vertex index that is added to all of the indexes written to the triangle array.
	//
	//# \desc
	//# The $TriangulatePolygon$ function takes an arbitrary planar polygon specified by <i>n</i> vertices,
	//# triangulates it, and returns the number of triangles that were generated. Except in cases in which there
	//# are degenerate vertices, the number of triangles generated will be <i>n</i>&nbsp;&minus;&nbsp;2.
	//#
	//# The input polygon is specified by the $vertexCount$ and $vertex$ parameters and may be any planar shape that
	//# is not self-intersecting. Convex polygons, concave polygons, and polygons containing sets of collinear
	//# vertices are all valid. The set of vertices must be specified in order and must be wound counterclockwise
	//# about the normal direction specified by the $normal$ parameter. The minimum number of vertices is 3, in
	//# which case a single triangle is generated.
	//#
	//# The results are undefined if the input polygon is self-intersecting or if the vertices do not lie in a plane
	//# that is perpendicular to the normal direction specified by the $normal$ parameter. If the array of vertices
	//# contains degenerate entries (a pair of vertices that are very close to each other), then the result may be
	//# a subset of the fully triangulated polygon, indicated by the return value being less than <i>n</i>&nbsp;&minus;&nbsp;2.
	//
	//# \also	$@Math::TriangulatePolygonalComplex@$


	//# \function	Math::TriangulatePolygonalComplex		Triangulates a convex or concave polygon having nonzero genus.
	//
	//# \proto	int32 TriangulatePolygonalComplex(int32 windingCount, const int32 *vertexCount, const unsigned_int16 *const *vertexIndex,
	//# \proto2	const Point3D *vertex, const Vector3D& normal, Point3D *restrict result, Triangle *restrict triangle);
	//
	//# \param	windingCount	The number of closed loops defining the shape of the polygonal complex. The first
	//#							winding specifies the outside boundary of the positive polygon and should be wound
	//#							counterclockwise. The remaining windings specify the boundaries of negative polygons
	//#							representing holes that are cut out of the positive polygon and should be wound clockwise.
	//# \param	vertexCount		An array of vertex counts specifying the number of vertices in each winding.
	//# \param	vertexIndex		An array of pointers to arrays containing the vertex indexes of the vertices used
	//#							by each winding. For each winding, there is one pointer in this array that points
	//#							to an array of indexes whose size is equal to the number of vertices belonging to
	//#							the winding, as specified by the corresponding entry in the $vertexCount$ array.
	//#							The vertex indexes refer to entries in the vertex array specified by the $vertex$ parameter.
	//# \param	vertex			An array containing all of the vertices belonging to all of the windings. The
	//#							entries of the $vertexCount$ and $vertexIndex$ arrays specify the number and first
	//#							index of the vertices belong to each winding.
	//# \param	normal			The normal direction of the polygonal complex.
	//# \param	result			An array that receives the vertex positions in the final triangulation. This array
	//#							should be large enough to hold the number of vertices equal to the sum of the values
	//#							in the $vertexCount$ array.
	//# \param	triangle		An array to which the triangles are written. This must be large enough to hold
	//#							<i>n</i>&nbsp;&minus;&nbsp;2&nbsp;+&nbsp;2<i>g</i> triangles, where <i>n</i> is the sum of
	//#							the values in the $vertexCount$ array, and <i>g</i> is the genus of the complex
	//#							(which is one less than the value given by the $windingCount$ parameter).
	//
	//# \desc
	//# The $TriangulatePolygonalComplex$ function takes an arbitrary planar polygon having any number of polygonal
	//# holes cut out of it, triangulates it, and returns the number of triangles that were generated.
	//# The polygonal complex is specified by one or more windings representing the boundary of the interior area.
	//# Except in cases in which there are degenerate vertices, the number of triangles generated will be
	//# <i>n</i>&nbsp;&minus;&nbsp;2&nbsp;+&nbsp;2<i>g</i>, where <i>n</i> is the sum of the values in the $vertexCount$
	//# array, and <i>g</i> is the genus of the complex, equal to one less than the value of the $windingCount$ parameter.
	//#
	//# Each of the windings may be convex or concave and may contain sets of collinear vertices. The first winding,
	//# whose vertices are specified by the first entries of the $vertexCount$ and $vertexIndex$ arrays, represents
	//# the (positive) outer boundary of the complex. It should be wound counterclockwise about the normal direction
	//# specified by the $normal$ parameter. The remaining windings represent the boundaries of the (negative) holes
	//# that are cut out of the complex. These windings should be wound clockwise (so that the interior of the
	//# complex is always on the left-hand side when travelling along any winding). The negative windings must not
	//# intersect or enclose any other winding, and each must be completely contained within the positive winding.
	//# If these requirements are not met, then the results are undefined.
	//#
	//# The results are undefined if any input winding is self-intersecting or if the vertices do not lie in a plane
	//# that is perpendicular to the normal direction specified by the $normal$ parameter. If any array of vertices
	//# contains degenerate entries (a pair of vertices that are very close to each other), then the result may be
	//# a subset of the fully triangulated polygonal complex, indicated by the return value being less than
	//# <i>n</i>&nbsp;&minus;&nbsp;2&nbsp;+&nbsp;2<i>g</i>.
	//#
	//# The resulting triangles are returned in the array specified by the $triangle$ parameter. The input vertices
	//# are reordered and stored in the array specified by the $result$ parameter. The indexes contained in the
	//# $triangle$ array refer to vertices in the $result$ array.
	//#
	//# If the $windingCount$ parameter is 1 (the minimum value), then this function behaves exactly like the
	//# $@Math::TriangulatePolygon@$ function.
	//
	//# \also	$@Math::TriangulatePolygon@$


	//# \div
	//# \function	Math::Random		Returns a random integer number.
	//
	//# \proto	unsigned_int32 Random(unsigned_int32 n);
	//
	//# \param	n	The number of possible values returned. This should be less than 65536.
	//
	//# \desc
	//# The $Random$ function returns a random integer between 0 and $n$&nbsp;&minus;&nbsp;1, inclusive.
	//# The number $n$ should be in the range [0, 65535].
	//
	//# \also	$@Math::RandomFloat@$
	//# \also	$@Math::RandomUnitVector2D@$
	//# \also	$@Math::RandomUnitVector3D@$
	//# \also	$@Math::RandomUnitQuaternion@$


	//# \function	Math::RandomFloat		Returns a random floating-point number.
	//
	//# \proto	float RandomFloat(float f);
	//
	//# \param	f	The supremum of the range of possible values returned.
	//
	//# \desc
	//# The $RandomFloat$ function returns a random floating-point number in the range [0.0, $f$).
	//
	//# \also	$@Math::Random@$
	//# \also	$@Math::RandomUnitVector2D@$
	//# \also	$@Math::RandomUnitVector3D@$
	//# \also	$@Math::RandomUnitQuaternion@$


	//# \function	Math::RandomUnitVector2D		Returns a random unit-length 2D vector.
	//
	//# \proto	Vector2D RandomUnitVector2D(void);
	//
	//# \desc
	//# The $RandomUnitVector2D$ function returns a random unit-length 2D vector. The values returned
	//# are evenly distributed over the 2D unit circle.
	//
	//# \also	$@Math::RandomUnitVector3D@$
	//# \also	$@Math::RandomUnitQuaternion@$
	//# \also	$@Math::Random@$
	//# \also	$@Math::RandomFloat@$


	//# \function	Math::RandomUnitVector3D		Returns a random unit-length 3D vector.
	//
	//# \proto	Vector3D RandomUnitVector3D(void);
	//
	//# \desc
	//# The $RandomUnitVector3D$ function returns a random unit-length 3D vector. The values returned
	//# are evenly distributed over the 3D unit sphere.
	//
	//# \also	$@Math::RandomUnitVector2D@$
	//# \also	$@Math::RandomUnitQuaternion@$
	//# \also	$@Math::Random@$
	//# \also	$@Math::RandomFloat@$


	//# \function	Math::RandomUnitQuaternion		Returns a random unit-length quaternion.
	//
	//# \proto	Quaternion RandomUnitQuaternion(void);
	//
	//# \desc
	//# The $RandomUnitQuaternion$ function returns a random unit-length quaternion. The values returned
	//# are evenly distributed over the 4D unit sphere.
	//
	//# \also	$@Math::RandomUnitVector2D@$
	//# \also	$@Math::RandomUnitVector3D@$
	//# \also	$@Math::Random@$
	//# \also	$@Math::RandomFloat@$


	namespace Math
	{
		C4API void CalculateBarycentricCoordinates(const Point3D& p1, const Point3D& p2, const Point3D& p3, const Point3D& c, float *w1, float *w2, float *w3);
		C4API bool CalculateTangent(const Point3D& p1, const Point3D& p2, const Point3D& p3, const Point2D& u1, const Point2D& u2, const Point2D& u3, Vector4D *tangent);

		C4API bool IntersectSegmentAndTriangle(const Point3D& p1, const Point3D& p2, const Point3D& v1, const Point3D& v2, const Point3D& v3, Point3D *position, Vector3D *normal, float *param);
		C4API bool IntersectRayAndSphere(const Ray *ray, const Point3D& center, float radius, float *t1, float *t2);
		C4API bool SegmentIntersectsSphere(const Point3D& p1, const Point3D& p2, const Point3D& center, float radius);

		C4API float DistancePointToLine(const Point3D& q, const Point3D& s, const Vector3D& v);
		C4API float SquaredDistancePointToLine(const Point3D& q, const Point3D& s, const Vector3D& v);
		C4API float DistanceLineToLine(const Point3D& s1, const Vector3D& v1, const Point3D& s2, const Vector3D& v2);
		C4API float SquaredDistanceLineToLine(const Point3D& s1, const Vector3D& v1, const Point3D& s2, const Vector3D& v2);
		C4API bool CalculateNearestParameters(const Point3D& s1, const Vector3D& v1, const Point3D& s2, const Vector3D& v2, float *s, float *t);

		C4API bool SolveLinearSystem(int32 n, float *restrict m, float *restrict r);
		C4API bool LUDecompose(int32 n, float *restrict m, unsigned_int16 *restrict index, float *restrict detSign = nullptr);
		C4API void LUBacksubstitute(int32 n, const float *d, const unsigned_int16 *index, const float *r, float *restrict x);
		C4API void LURefineSolution(int32 n, const float *m, const float *d, const unsigned_int16 *index, const float *r, float *restrict x);
		C4API void SolveTridiagonalSystem(int32 n, const float *a, const float *b, const float *c, const float *r, float *restrict x);

		C4API void CalculateEigensystem(const Matrix3D& m, Vector3D *eigenvalue, Matrix3D *eigenvector);
		C4API void CalculatePrincipalAxes(int32 vertexCount, const Point3D *vertex, Point3D *center, Vector3D *axis);

		C4API bool ClipSegment(Point3D *p1, Point3D *p2, const Antivector4D& clippingPlane);

		C4API float GetPolygonArea(int32 vertexCount, const Point3D *vertex, const Vector3D& normal);
		C4API bool ConvexPolygon(int32 vertexCount, const Point3D *vertex, const Vector3D& normal);
		C4API int32 PointInConvexPolygon(const Point3D& p, int32 vertexCount, const Point3D *vertex, const Vector3D& normal);
		C4API int32 ClipPolygon(int32 vertexCount, const Point3D *vertex, const Antivector4D& clippingPlane, int8 *restrict location, Point3D *restrict result);

		C4API int32 TriangulatePolygon(int32 vertexCount, const Point3D *vertex, const Vector3D& normal, Triangle *restrict triangle, int32 base = 0);
		C4API int32 TriangulatePolygon(int32 windingCount, const int32 *vertexCount, const unsigned_int16 *const *vertexIndex, const Point3D *vertex, const Vector3D& normal, Point3D *restrict result, Triangle *restrict triangle, int32 triangleCount);

		inline int32 TriangulatePolygonalComplex(int32 windingCount, const int32 *vertexCount, const unsigned_int16 *const *vertexIndex, const Point3D *vertex, const Vector3D& normal, Point3D *restrict result, Triangle *restrict triangle)
		{
			return (TriangulatePolygon(windingCount, vertexCount, vertexIndex, vertex, normal, result, triangle, 0));
		}

		void IntersectConvexPolygons(const int32 *vertexCount, const Point3D *const *vertex, const Vector3D& normal, int32 *restrict resultCount, Point3D *restrict result);
		bool SubtractConvexPolygons(const int32 *vertexCount, const Point3D *const *vertex, const Vector3D& normal, int32 *restrict resultCount, int32 *restrict triangleCount, Point3D *restrict result, Triangle *restrict triangle);
	}
}


#endif

// ZYUQURM
