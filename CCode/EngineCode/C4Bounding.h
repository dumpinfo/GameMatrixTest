 

#ifndef C4Bounding_h
#define C4Bounding_h


//# \component	Graphics Manager
//# \prefix		GraphicsMgr/


#include "C4Types.h"


namespace C4
{
	//# \class	Box2D		Encapsulates a 2D aligned bounding box.
	//
	//# The $Box2D$ class encapsulates a 2D aligned bounding box.
	//
	//# \def	class Box2D : public Range<Point2D>
	//
	//# \ctor	Box2D(const Point2D& pmin, const Point2D& pmax);
	//
	//# \param	pmin	The point at the minimum corner of the bounding box.
	//# \param	pmax	The point at the maximum corner of the bounding box.
	//
	//# \desc
	//# The $Box2D$ class encapsulates the 2D aligned bounding box whose minimum and maximum extents are specified
	//# by the $pmin$ and $pmax$ parameters.
	//
	//# \base	Utilities/Range<Point2D>	An aligned box is implemented as a range of points.
	//
	//# \also	$@Box3D@$
	//# \also	$@BoundingBox@$


	//# \function	Box2D::Calculate		Calculates a bounding box for a set of 2D vertices.
	//
	//# \proto	void Calculate(int32 vertexCount, const Point2D *vertex);
	//
	//# \param	vertexCount		The number of vertices. This must be at least 1.
	//# \param	vertex			A pointer to the array of vertex positions.
	//
	//# \desc
	//# The $Calculate$ function calculates the aligned bounding box for an array of vertices.


	//# \function	Box2D::Contains		Returns a boolean value indicating whether an aligned box contains a point.
	//
	//# \proto	bool Contains(const Point2D& p) const;
	//
	//# \param	p		The point that is tested for containment.
	//
	//# \desc
	//# The $Contains$ function determines whether the point specified by the $p$ parameter is contained in an
	//# aligned bounding box and returns $true$ if so and $false$ otherwise.
	//
	//# \also	$@Box2D::Intersection@$


	//# \function	Box2D::Intersection		Returns a boolean value indicating whether two aligned boxes intersect.
	//
	//# \proto	bool Intersection(const Box2D& box) const;
	//
	//# \param	box		The aligned bounding box with which intersection is tested.
	//
	//# \desc
	//# The $Intersection$ function determines whether an aligned bounding box intersects the aligned bounding box
	//# specified by the $box$ parameter. If the two boxes intersect, then the return value is $true$. Otherwise,
	//# the return value is $false$.
	//
	//# \also	$@Box2D::Contains@$


	class Box2D : public Range<Point2D>
	{
		public:

			Box2D() = default;

			Box2D(const Point2D& pmin, const Point2D& pmax) : Range<Point2D>(pmin, pmax)
			{
			}

			Vector2D GetSize(void) const
			{
				return (max - min);
			}

			Point2D GetCenter(void) const
			{
				return ((min + max) * 0.5F);
			}

			float GetComponent(int32 index) const
			{
				return (reinterpret_cast<const float *>(this)[index]);
			}

			void Offset(const Vector2D& offset)
			{
				min += offset;
				max += offset; 
			}

			void Scale(float scale) 
			{
				min *= scale; 
				max *= scale;
			}
 
			void Scale(const Vector2D& scale)
			{ 
				min &= scale; 
				max &= scale;
			}

			Vector2D IncludePoint(const Point2D& p) 
			{
				min.x = Fmin(min.x, p.x);
				min.y = Fmin(min.y, p.y);
				max.x = Fmax(max.x, p.x);
				max.y = Fmax(max.y, p.y);

				return (max - min);
			}

			C4API void Union(const Box2D& box);
			C4API void Union(const Point2D& p);
			C4API void Intersect(const Box2D& box);
			C4API void Transform(const Transform4D& transform);
			C4API void Calculate(int32 vertexCount, const Point2D *vertex);
			C4API void Calculate(const Point2D& p, int32 vertexCount, const Point2D *vertex);

			C4API bool Contains(const Point2D& p) const;
			C4API bool Intersection(const Box2D& box) const;
	};


	C4API Box2D Union(const Box2D& box1, const Box2D& box2);
	C4API Box2D Transform(const Box2D& box, const Transform4D& transform);


	//# \class	Box3D		Encapsulates a 3D aligned bounding box.
	//
	//# The $Box3D$ class encapsulates a 3D aligned bounding box.
	//
	//# \def	class Box3D : public Range<Point3D>
	//
	//# \ctor	Box3D(const Point3D& pmin, const Point3D& pmax);
	//
	//# \param	pmin	The point at the minimum corner of the bounding box.
	//# \param	pmax	The point at the maximum corner of the bounding box.
	//
	//# \desc
	//# The $Box3D$ class encapsulates the 3D aligned bounding box whose minimum and maximum extents are specified
	//# by the $pmin$ and $pmax$ parameters.
	//
	//# \base	Utilities/Range<Point3D>	An aligned box is implemented as a range of points.
	//
	//# \also	$@Box2D@$
	//# \also	$@BoundingBox@$


	//# \function	Box3D::Calculate		Calculates a bounding box for a set of vertices.
	//
	//# \proto	void Calculate(int32 vertexCount, const Point3D *vertex);
	//
	//# \param	vertexCount		The number of vertices. This must be at least 1.
	//# \param	vertex			A pointer to the array of vertex positions.
	//
	//# \desc
	//# The $Calculate$ function calculates the aligned bounding box for an array of vertices.


	//# \function	Box3D::Contains		Returns a boolean value indicating whether an aligned box contains a point.
	//
	//# \proto	bool Contains(const Point3D& p) const;
	//
	//# \param	p		The point that is tested for containment.
	//
	//# \desc
	//# The $Contains$ function determines whether the point specified by the $p$ parameter is contained in an
	//# aligned bounding box and returns $true$ if so and $false$ otherwise.
	//
	//# \also	$@Box3D::Intersection@$


	//# \function	Box3D::Intersection		Returns a boolean value indicating whether two aligned boxes intersect.
	//
	//# \proto	bool Intersection(const Box3D& box) const;
	//
	//# \param	box		The aligned bounding box with which intersection is tested.
	//
	//# \desc
	//# The $Intersection$ function determines whether an aligned bounding box intersects the aligned bounding box
	//# specified by the $box$ parameter. If the two boxes intersect, then the return value is $true$. Otherwise,
	//# the return value is $false$.
	//
	//# \also	$@Box3D::Contains@$


	class Box3D : public Range<Point3D>
	{
		public:

			Box3D() = default;

			Box3D(const Point3D& pmin, const Point3D& pmax) : Range<Point3D>(pmin, pmax)
			{
			}

			#if C4SIMD

				Box3D(vec_float pmin, vec_float pmax)
				{
					VecStore3D(pmin, &min.x);
					VecStore3D(pmax, &max.x);
				}

				Box3D& Set(vec_float pmin, vec_float pmax)
				{
					VecStore3D(pmin, &min.x);
					VecStore3D(pmax, &max.x);
					return (*this);
				}

				using Range<Point3D>::Set;

			#endif

			Vector3D GetSize(void) const
			{
				return (max - min);
			}

			Point3D GetCenter(void) const
			{
				return ((min + max) * 0.5F);
			}

			float GetComponent(int32 index) const
			{
				return (reinterpret_cast<const float *>(this)[index]);
			}

			void Offset(const Vector3D& offset)
			{
				min += offset;
				max += offset;
			}

			void Scale(float scale)
			{
				min *= scale;
				max *= scale;
			}

			void Scale(const Vector3D& scale)
			{
				min &= scale;
				max &= scale;
			}

			void Expand(float expand)
			{
				min.x -= expand;
				min.y -= expand;
				min.z -= expand;
				max.x += expand;
				max.y += expand;
				max.z += expand;
			}

			Vector3D IncludePoint(const Point3D& p)
			{
				min.x = Fmin(min.x, p.x);
				min.y = Fmin(min.y, p.y);
				min.z = Fmin(min.z, p.z);
				max.x = Fmax(max.x, p.x);
				max.y = Fmax(max.y, p.y);
				max.z = Fmax(max.z, p.z);

				return (max - min);
			}

			#if C4SIMD

				vec_float IncludePoint(vec_float p)
				{
					vec_float pmin = VecMin(VecLoadUnaligned(&min.x), p);
					vec_float pmax = VecMax(VecLoadUnaligned(&max.x), p);
					VecStore3D(pmin, &min.x);
					VecStore3D(pmax, &max.x);
					return (VecSub(pmax, pmin));
				}

				void Union(const Box3D& box)
				{
					vec_float pmin = VecMin(VecLoadUnaligned(&min.x), VecLoadUnaligned(&box.min.x));
					vec_float pmax = VecMax(VecLoadUnaligned(&max.x), VecLoadUnaligned(&box.max.x));
					VecStore3D(pmin, &min.x);
					VecStore3D(pmax, &max.x);
				}

				void Union(const Point3D& p)
				{
					vec_float point = VecLoadUnaligned(&p.x);
					vec_float pmin = VecMin(VecLoadUnaligned(&min.x), point);
					vec_float pmax = VecMax(VecLoadUnaligned(&max.x), point);
					VecStore3D(pmin, &min.x);
					VecStore3D(pmax, &max.x);
				}

				void Intersect(const Box3D& box)
				{
					vec_float pmin = VecMax(VecLoadUnaligned(&min.x), VecLoadUnaligned(&box.min.x));
					vec_float pmax = VecMin(VecLoadUnaligned(&max.x), VecLoadUnaligned(&box.max.x));
					VecStore3D(pmin, &min.x);
					VecStore3D(pmax, &max.x);
				}

			#else

				C4API void Union(const Box3D& box);
				C4API void Union(const Point3D& p);
				C4API void Intersect(const Box3D& box);

			#endif

			C4API void Transform(const Transform4D& transform);
			C4API void Calculate(int32 vertexCount, const Point3D *vertex);
			C4API void Calculate(const Point3D& p, int32 vertexCount, const Point3D *vertex);

			C4API bool ExteriorSphere(const Point3D& center, float radius) const;
			C4API bool ExteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const;

			C4API bool Contains(const Point3D& p) const;
			C4API bool Intersection(const Box3D& box) const;
	};


	C4API Box3D Union(const Box3D& box1, const Box3D& box2);
	C4API Box3D Transform(const Box3D& box, const Transform4D& transform);


	//# \class	BoundingBox		Encapsulates an oriented bounding box.
	//
	//# The $BoundingBox$ class encapsulates an oriented bounding box.
	//
	//# \def	class BoundingBox
	//
	//# \ctor	BoundingBox(const Point3D& center, const Vector3D *axis);
	//
	//# \param	center		The center of the bounding box.
	//# \param	axis		A pointer to an array of three vectors represent the semi-axes of the bounding box.
	//
	//# \desc
	//#
	//
	//# \also	$@BoundingSphere@$
	//# \also	$@BoundingEllipsoid@$
	//# \also	$@BoundingCylinder@$


	//# \function	BoundingBox::Calculate		Calculates a bounding box for a set of vertices.
	//
	//# \proto	void Calculate(int32 vertexCount, const Point3D *vertex);
	//
	//# \param	vertexCount		The number of vertices.
	//# \param	vertex			A pointer to the array of vertex positions.
	//
	//# \desc
	//# The $Calculate$ function calculates a good oriented bounding box for an array of vertices.
	//# Principal component analysis is used to determine the orientation of the box. For best results,
	//# the number of vertices should be at least 4, and the vertices should not all lie in the same plane.


	class BoundingBox
	{
		private:

			Point3D		boxCenter;
			Vector3D	boxAxis[3];

		public:

			BoundingBox() = default;

			BoundingBox(const Point3D& center, const Vector3D *axis)
			{
				boxCenter = center;
				boxAxis[0] = axis[0];
				boxAxis[1] = axis[1];
				boxAxis[2] = axis[2];
			}

			BoundingBox(const Point3D& center, const Vector3D& xaxis, const Vector3D& yaxis, const Vector3D& zaxis)
			{
				boxCenter = center;
				boxAxis[0] = xaxis;
				boxAxis[1] = yaxis;
				boxAxis[2] = zaxis;
			}

			const Point3D& GetCenter(void) const
			{
				return (boxCenter);
			}

			void SetCenter(const Point3D& center)
			{
				boxCenter = center;
			}

			void SetCenter(float x, float y, float z)
			{
				boxCenter.Set(x, y, z);
			}

			const Vector3D *GetAxes(void) const
			{
				return (boxAxis);
			}

			void SetAxes(const Vector3D *axis)
			{
				boxAxis[0] = axis[0];
				boxAxis[1] = axis[1];
				boxAxis[2] = axis[2];
			}

			void SetAxes(const Vector3D& xaxis, const Vector3D& yaxis, const Vector3D& zaxis)
			{
				boxAxis[0] = xaxis;
				boxAxis[1] = yaxis;
				boxAxis[2] = zaxis;
			}

			C4API void Calculate(int32 vertexCount, const Point3D *vertex);
	};


	//# \class	BoundingSphere		Encapsulates a bounding sphere.
	//
	//# The $BoundingSphere$ class encapsulates a bounding sphere.
	//
	//# \def	class BoundingSphere
	//
	//# \ctor	BoundingSphere(const Point3D& center, float radius);
	//
	//# \param	center		The center of the bounding sphere.
	//# \param	radius		The radius of the bounding sphere.
	//
	//# \desc
	//#
	//
	//# \also	$@BoundingBox@$
	//# \also	$@BoundingEllipsoid@$
	//# \also	$@BoundingCylinder@$


	//# \function	BoundingSphere::Calculate		Calculates a bounding sphere for a set of vertices.
	//
	//# \proto	void Calculate(int32 vertexCount, const Point3D *vertex);
	//
	//# \param	vertexCount		The number of vertices.
	//# \param	vertex			A pointer to the array of vertex positions.
	//
	//# \desc
	//# The $Calculate$ function calculates a good bounding sphere for an array of vertices.


	class BoundingSphere
	{
		private:

			Point3D		sphereCenter;
			float		sphereRadius;

		public:

			BoundingSphere() = default;

			BoundingSphere(const Point3D& center, float radius)
			{
				sphereCenter = center;
				sphereRadius = radius;
			}

			const Point3D& GetCenter(void) const
			{
				return (sphereCenter);
			}

			void SetCenter(const Point3D& center)
			{
				sphereCenter = center;
			}

			void SetCenter(float x, float y, float z)
			{
				sphereCenter.Set(x, y, z);
			}

			const float& GetRadius(void) const
			{
				return (sphereRadius);
			}

			void SetRadius(float radius)
			{
				sphereRadius = radius;
			}

			C4API void Union(const BoundingSphere *sphere);
			C4API void Calculate(int32 vertexCount, const Point3D *vertex);

			C4API bool IntersectsSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const;
	};


	//# \class	BoundingEllipsoid		Encapsulates an oriented bounding ellipsoid.
	//
	//# The $BoundingEllipsoid$ class encapsulates an oriented bounding ellipsoid.
	//
	//# \def	class BoundingEllipsoid
	//
	//# \ctor	BoundingEllipsoid(const Point3D& center, const Vector3D *axis);
	//
	//# \param	center		The center of the bounding box.
	//# \param	axis		A pointer to an array of three vectors represent the semi-axes of the bounding ellipsoid.
	//
	//# \desc
	//#
	//
	//# \also	$@BoundingBox@$
	//# \also	$@BoundingSphere@$
	//# \also	$@BoundingCylinder@$


	//# \function	BoundingEllipsoid::Calculate		Calculates a bounding ellipsoid for a set of vertices.
	//
	//# \proto	void Calculate(int32 vertexCount, const Point3D *vertex);
	//
	//# \param	vertexCount		The number of vertices.
	//# \param	vertex			A pointer to the array of vertex positions.
	//
	//# \desc
	//# The $Calculate$ function calculates a good oriented bounding ellipsoid for an array of vertices.
	//# Principal component analysis is used to determine the orientation of the ellipsoid. For best results,
	//# the number of vertices should be at least 4, and the vertices should not all lie in the same plane.


	class BoundingEllipsoid
	{
		private:

			Point3D		ellipsoidCenter;
			Vector3D	ellipsoidAxis[3];

		public:

			BoundingEllipsoid() = default;

			BoundingEllipsoid(const Point3D& center, const Vector3D *axis)
			{
				ellipsoidCenter = center;
				ellipsoidAxis[0] = axis[0];
				ellipsoidAxis[1] = axis[1];
				ellipsoidAxis[2] = axis[2];
			}

			const Point3D& GetCenter(void) const
			{
				return (ellipsoidCenter);
			}

			void SetCenter(const Point3D& center)
			{
				ellipsoidCenter = center;
			}

			const Vector3D *GetAxes(void) const
			{
				return (ellipsoidAxis);
			}

			void SetAxes(const Vector3D *axis)
			{
				ellipsoidAxis[0] = axis[0];
				ellipsoidAxis[1] = axis[1];
				ellipsoidAxis[2] = axis[2];
			}

			C4API void Calculate(int32 vertexCount, const Point3D *vertex);
	};


	//# \class	BoundingCylinder		Encapsulates an oriented bounding cylinder.
	//
	//# The $BoundingCylinder$ class encapsulates an oriented bounding cylinder.
	//
	//# \def	class BoundingCylinder
	//
	//# \ctor	BoundingCylinder(const Point3D& endpoint1, const Point3D& endpoint2, float radius);
	//
	//# \param	endpoint1		The first endpoint of the bounding cylinder.
	//# \param	endpoint2		The second endpoint of the bounding cylinder.
	//# \param	radius			The radius of the bounding cylinder.
	//
	//# \desc
	//#
	//
	//# \also	$@BoundingBox@$
	//# \also	$@BoundingSphere@$
	//# \also	$@BoundingEllipsoid@$


	//# \function	BoundingCylinder::Calculate		Calculates a bounding cylinder for a set of vertices.
	//
	//# \proto	void Calculate(int32 vertexCount, const Point3D *vertex);
	//
	//# \param	vertexCount		The number of vertices.
	//# \param	vertex			A pointer to the array of vertex positions.
	//
	//# \desc
	//# The $Calculate$ function calculates a good oriented bounding cylinder for an array of vertices.
	//# Principal component analysis is used to determine the orientation of the cylinder. For best results,
	//# the number of vertices should be at least 4, and the vertices should not all lie in the same plane.


	class BoundingCylinder
	{
		private:

			Point3D		cylinderEndpoint1;
			Point3D		cylinderEndpoint2;
			float		cylinderRadius;

		public:

			BoundingCylinder() = default;

			BoundingCylinder(const Point3D& endpoint1, const Point3D& endpoint2, float radius)
			{
				cylinderEndpoint1 = endpoint1;
				cylinderEndpoint2 = endpoint2;
				cylinderRadius = radius;
			}

			const Point3D& GetEndpoint1(void) const
			{
				return (cylinderEndpoint1);
			}

			const Point3D& GetEndpoint2(void) const
			{
				return (cylinderEndpoint2);
			}

			void SetEndpoints(const Point3D& endpoint1, const Point3D& endpoint2)
			{
				cylinderEndpoint1 = endpoint1;
				cylinderEndpoint2 = endpoint2;
			}

			const float& GetRadius(void) const
			{
				return (cylinderRadius);
			}

			void SetRadius(float radius)
			{
				cylinderRadius = radius;
			}

			C4API void Calculate(int32 vertexCount, const Point3D *vertex);
	};
}


#endif

// ZYUQURM
