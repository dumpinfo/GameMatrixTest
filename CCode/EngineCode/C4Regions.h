 

#ifndef C4Regions_h
#define C4Regions_h


//# \component	World Manager
//# \prefix		WorldMgr/


#include "C4Polyhedron.h"
#include "C4Bounding.h"
#include "C4Portals.h"

#if C4DIAGS

	#include "C4Renderable.h"

#endif


namespace C4
{
	enum
	{
		kOcclusionGeometry		= 1 << 0,
		kOcclusionPortal		= 1 << 1,
		kOcclusionFog			= 1 << 2,
		kOcclusionCascade		= 1 << 3
	};


	class Light;
	class LightRegion;
	class Camera;
	class FrustumCamera;
	class Portal;
	class Zone;


	//# \class	VisibilityRegion		Represents a convex region of space used for visibility testing.
	//
	//# The $VisibilityRegion$ class represents a convex region of space used for visibility testing.
	//
	//# \def	class VisibilityRegion
	//
	//# \ctor	VisibilityRegion(Convexity *conv);
	//
	//# \desc
	//# The $VisibilityRegion$ class represents a convex region of space that is bounded by a set of planes.
	//# These regions are often used for camera visibility testing, light source permeation, and shadow
	//# casting set determination. The $VisibilityRegion$ class contains methods for testing whether various
	//# types of objects may intersect the region, providing basic culling functionality. Note that the
	//# visibility tests are not perfect and may return $true$ in cases where the tested object lies
	//# completely outside the region but not completely on the negative side of any one plane.
	//
	//# \also	$@OcclusionRegion@$


	//# \function	VisibilityRegion::BoxVisible		Determines whether a box is visible in a region.
	//
	//# \proto	bool BoxVisible(const Point3D& center, const Vector3D *axis) const;
	//
	//# \param	center		The world-space center of the box.
	//# \param	axis		A pointer to an array containing the three semi-axis vectors of the box.
	//
	//# \desc
	//# The $BoxVisible$ function determines whether the box specified by the $center$ and $axis$ parameters is
	//# visible inside a region. If the box falls completely on the negative side of any single plane bounding
	//# the region, then the return value is $false$. Otherwise, the return value is $true$.
	//
	//# \also	$@VisibilityRegion::QuadVisible@$
	//# \also	$@VisibilityRegion::PolygonVisible@$
	//# \also	$@VisibilityRegion::SphereVisible@$
	//# \also	$@VisibilityRegion::EllipsoidVisible@$
	//# \also	$@VisibilityRegion::CylinderVisible@$


	//# \function	VisibilityRegion::QuadVisible		Determines whether quad is visible in a region.
	//
	//# \proto	bool QuadVisible(const Point3D *vertex) const;
	//
	//# \param	vertex		A pointer to an array containing the four world-space vertex positions belonging to the quad.
	//
	//# \desc
	//# The $QuadVisible$ function determines whether the quad specified by the $vertex$ parameter is visible
	//# inside a region. If all four vertex positions fall completely on the negative side of any single plane
	//# bounding the region, then the return value is $false$. Otherwise, the return value is $true$.
	//
	//# \also	$@VisibilityRegion::BoxVisible@$
	//# \also	$@VisibilityRegion::PolygonVisible@$
	//# \also	$@VisibilityRegion::SphereVisible@$
	//# \also	$@VisibilityRegion::EllipsoidVisible@$
	//# \also	$@VisibilityRegion::CylinderVisible@$


	//# \function	VisibilityRegion::PolygonVisible		Determines whether polygon is visible in a region.
	//
	//# \proto	bool PolygonVisible(int32 vertexCount, const Point3D *vertex) const;
	//
	//# \param	vertexCount		The number of vertices in the polygon.
	//# \param	vertex			A pointer to an array containing the polygon's world-space vertex positions.
	// 
	//# \desc
	//# The $PolygonVisible$ function determines whether the polygon specified by the $vertexCount$ and $vertex$
	//# parameters is visible inside a region. If all of the vertex positions fall completely on the negative 
	//# side of any single plane bounding the region, then the return value is $false$. Otherwise, the return
	//# value is $true$. 
	//
	//# \also	$@VisibilityRegion::BoxVisible@$
	//# \also	$@VisibilityRegion::QuadVisible@$ 
	//# \also	$@VisibilityRegion::SphereVisible@$
	//# \also	$@VisibilityRegion::EllipsoidVisible@$ 
	//# \also	$@VisibilityRegion::CylinderVisible@$ 


	//# \function	VisibilityRegion::SphereVisible		Determines whether a sphere is visible in a region.
	// 
	//# \proto	bool SphereVisible(const Point3D& center, float radius) const;
	//
	//# \param	center		The world-space center of the sphere.
	//# \param	radius		The radius of the sphere.
	//
	//# \desc
	//# The $SphereVisible$ function determines whether the sphere specified by the $center$ and $radius$ parameters
	//# is visible inside a region. If the sphere falls completely on the negative side of any single plane bounding
	//# the region, then the return value is $false$. Otherwise, the return value is $true$.
	//
	//# \also	$@VisibilityRegion::BoxVisible@$
	//# \also	$@VisibilityRegion::QuadVisible@$
	//# \also	$@VisibilityRegion::PolygonVisible@$
	//# \also	$@VisibilityRegion::EllipsoidVisible@$
	//# \also	$@VisibilityRegion::CylinderVisible@$


	//# \function	VisibilityRegion::EllipsoidVisible		Determines whether an ellipsoid is visible in a region.
	//
	//# \proto	bool EllipsoidVisible(const Point3D& center, const Vector3D *axis) const;
	//
	//# \param	center		The world-space center of the ellipsoid.
	//# \param	axis		A pointer to an array containing the three semi-axis vectors of the ellipsoid.
	//
	//# \desc
	//# The $EllipsoidVisible$ function determines whether the sphere specified by the $center$ and $axis$ parameters
	//# is visible inside a region. If the ellipsoid falls completely on the negative side of any single plane bounding
	//# the region, then the return value is $false$. Otherwise, the return value is $true$.
	//
	//# \also	$@VisibilityRegion::BoxVisible@$
	//# \also	$@VisibilityRegion::QuadVisible@$
	//# \also	$@VisibilityRegion::PolygonVisible@$
	//# \also	$@VisibilityRegion::SphereVisible@$
	//# \also	$@VisibilityRegion::CylinderVisible@$


	//# \function	VisibilityRegion::CylinderVisible		Determines whether a cylinder is visible in a region.
	//
	//# \proto	bool CylinderVisible(const Point3D& p1, const Point3D& p2, float radius) const;
	//
	//# \param	p1			The world-space center of one end of the cylinder.
	//# \param	p2			The world-space center of the other end of the cylinder.
	//# \param	radius		The radius of the cylinder.
	//
	//# \desc
	//# The $CylinderVisible$ function determines whether the cylinder specified by the $p1$, $p2$, and $radius$ parameters
	//# is visible inside a region. The cylinder's axis is clipped as it is tested against each plane bounding the region.
	//# If at any point the clipped cylinder falls completely on the negative side of a single plane then the return
	//# value is $false$. Otherwise, the return value is $true$.
	//
	//# \also	$@VisibilityRegion::BoxVisible@$
	//# \also	$@VisibilityRegion::QuadVisible@$
	//# \also	$@VisibilityRegion::PolygonVisible@$
	//# \also	$@VisibilityRegion::SphereVisible@$
	//# \also	$@VisibilityRegion::EllipsoidVisible@$


	class VisibilityRegion
	{
		private:

			Convexity		*convexity;
			int32			auxiliaryPlaneCount;

		protected:

			VisibilityRegion(Convexity *conv)
			{
				convexity = conv;
				auxiliaryPlaneCount = 0;
			}

		public:

			int32 GetPlaneCount(void) const
			{
				return (convexity->planeCount);
			}

			void SetPlaneCount(int32 count)
			{
				Assert(count <= kMaxPolyhedronFaceCount, "VisibilityRegion::SetPlaneCount(), count too large\n");
				convexity->planeCount = (unsigned_int8) count;
			}

			Antivector4D *GetPlaneArray(void)
			{
				return (convexity->plane);
			}

			const Antivector4D *GetPlaneArray(void) const
			{
				return (convexity->plane);
			}

			int32 GetAuxiliaryPlaneCount(void) const
			{
				return (auxiliaryPlaneCount);
			}

			void SetAuxiliaryPlaneCount(int32 count)
			{
				auxiliaryPlaneCount = count;
			}

			C4API bool BoxVisible(const Box3D& box) const;
			C4API bool BoxVisible(const Point3D& center, const Vector3D *axis) const;
			C4API bool BoxVisible(const Point3D& center, const Vector3D& size) const;
			C4API bool DirectionVisible(const Vector3D& center, float radius) const;
			C4API bool QuadVisible(const Point3D *vertex) const;
			C4API bool PolygonVisible(int32 vertexCount, const Point3D *vertex) const;
			C4API bool PyramidVisible(const Point3D& apex, int32 vertexCount, const Point3D *vertex) const;
			C4API bool SphereVisible(const Point3D& center, float radius) const;
			C4API bool EllipsoidVisible(const Point3D& center, const Vector3D *axis) const;
			C4API bool CylinderVisible(const Point3D& p1, const Point3D& p2, float radius) const;
	};


	//# \class	OcclusionRegion		Represents a convex region of space used for occlusion testing.
	//
	//# The $OcclusionRegion$ class represents a convex region of space used for occlusion testing.
	//
	//# \def	class OcclusionRegion : public ListElement<OcclusionRegion>, public Memory<OcclusionRegion>
	//
	//# \ctor	OcclusionRegion();
	//
	//# \desc
	//# The $OcclusionRegion$ class represents a convex region of space bounded by a set of planes.
	//# These regions are created automatically by the World Manager when occlusion portals or occlusion
	//# spaces are present in the world. The $OcclusionRegion$ class contains methods for testing whether
	//# various types of objects are completely contained within the region, providing basic occlusion functionality.
	//
	//# \base	Utilities/ListElement<OcclusionRegion>		Occlusion regions can be stored in a list.
	//# \base	MemoryMgrMemory<OcclusionRegion>			Occlusion regions are stored in a dedicated heap.
	//
	//# \also	$@VisibilityRegion@$


	//# \function	OcclusionRegion::BoxOccluded		Determines whether a box is occluded in a region.
	//
	//# \proto	bool BoxOccluded(const Point3D& center, const Vector3D *axis) const;
	//
	//# \param	center		The world-space center of the box.
	//# \param	axis		A pointer to an array containing the three semi-axis vectors of the box.
	//
	//# \desc
	//# The $BoxOccluded$ function determines whether the box specified by the $center$ and $axis$ parameters is
	//# occluded by a region. If the box falls completely on the positive side of all the planes bounding the region,
	//# then the return value is $true$. Otherwise, the return value is $false$.
	//
	//# \also	$@OcclusionRegion::QuadOccluded@$
	//# \also	$@OcclusionRegion::PolygonOccluded@$
	//# \also	$@OcclusionRegion::SphereOccluded@$
	//# \also	$@OcclusionRegion::EllipsoidOccluded@$
	//# \also	$@OcclusionRegion::CylinderOccluded@$


	//# \function	OcclusionRegion::QuadOccluded		Determines whether a quad is occluded in a region.
	//
	//# \proto	bool QuadOccluded(const Point3D *vertex) const;
	//
	//# \param	vertex		A pointer to an array containing the four world-space vertex positions belonging to the quad.
	//
	//# \desc
	//# The $QuadOccluded$ function determines whether the quad specified by the $vertex$ parameter is occluded by
	//# a region. If all four vertex positions fall on the positive side of all the planes bounding the region,
	//# then the return value is $true$. Otherwise, the return value is $false$.
	//
	//# \also	$@OcclusionRegion::BoxOccluded@$
	//# \also	$@OcclusionRegion::PolygonOccluded@$
	//# \also	$@OcclusionRegion::SphereOccluded@$
	//# \also	$@OcclusionRegion::EllipsoidOccluded@$
	//# \also	$@OcclusionRegion::CylinderOccluded@$


	//# \function	OcclusionRegion::PolygonOccluded		Determines whether a polygon is occluded in a region.
	//
	//# \proto	bool PolygonOccluded(int32 vertexCount, const Point3D *vertex) const;
	//
	//# \param	vertexCount		The number of vertices in the polygon.
	//# \param	vertex			A pointer to an array containing the polygon's world-space vertex positions.
	//
	//# \desc
	//# The $PolygonOccluded$ function determines whether the polygon specified by the $vertexCount$ and $vertex$
	//# parameters is occluded by a region. If all the vertex positions fall on the positive side of all the planes
	//# bounding the region, then the return value is $true$. Otherwise, the return value is $false$.
	//
	//# \also	$@OcclusionRegion::BoxOccluded@$
	//# \also	$@OcclusionRegion::QuadOccluded@$
	//# \also	$@OcclusionRegion::SphereOccluded@$
	//# \also	$@OcclusionRegion::EllipsoidOccluded@$
	//# \also	$@OcclusionRegion::CylinderOccluded@$


	//# \function	OcclusionRegion::SphereOccluded		Determines whether a sphere is occluded in a region.
	//
	//# \proto	bool SphereOccluded(const Point3D& center, float radius) const;
	//
	//# \param	center		The world-space center of the sphere.
	//# \param	radius		The radius of the sphere.
	//
	//# \desc
	//# The $SphereOccluded$ function determines whether the sphere specified by the $center$ and $radius$ parameters is
	//# occluded by a region. If the sphere falls completely on the positive side of all the planes bounding the region,
	//# then the return value is $true$. Otherwise, the return value is $false$.
	//
	//# \also	$@OcclusionRegion::BoxOccluded@$
	//# \also	$@OcclusionRegion::QuadOccluded@$
	//# \also	$@OcclusionRegion::PolygonOccluded@$
	//# \also	$@OcclusionRegion::EllipsoidOccluded@$
	//# \also	$@OcclusionRegion::CylinderOccluded@$


	//# \function	OcclusionRegion::EllipsoidOccluded		Determines whether an ellipsoid is occluded in a region.
	//
	//# \proto	bool EllipsoidOccluded(const Point3D& center, const Vector3D *axis) const;
	//
	//# \param	center		The world-space center of the ellipsoid.
	//# \param	axis		A pointer to an array containing the three semi-axis vectors of the ellipsoid.
	//
	//# \desc
	//# The $EllipsoidOccluded$ function determines whether the ellipsoid specified by the $center$ and $axis$ parameters is
	//# occluded by a region. If the ellipsoid falls completely on the positive side of all the planes bounding the region,
	//# then the return value is $true$. Otherwise, the return value is $false$.
	//
	//# \also	$@OcclusionRegion::BoxOccluded@$
	//# \also	$@OcclusionRegion::QuadOccluded@$
	//# \also	$@OcclusionRegion::PolygonOccluded@$
	//# \also	$@OcclusionRegion::SphereOccluded@$
	//# \also	$@OcclusionRegion::CylinderOccluded@$


	//# \function	OcclusionRegion::CylinderOccluded		Determines whether a cylinder is occluded in a region.
	//
	//# \proto	bool CylinderOccluded(const Point3D& p1, const Point3D& p2, float radius) const;
	//
	//# \param	p1			The world-space center of one end of the cylinder.
	//# \param	p2			The world-space center of the other end of the cylinder.
	//# \param	radius		The radius of the cylinder.
	//
	//# \desc
	//# The $CylinderOccluded$ function determines whether the cylinder specified by the $p1$, $p2$, and $radius$ parameters is
	//# occluded by a region. If the cylinder falls completely on the positive side of all the planes bounding the region,
	//# then the return value is $true$. Otherwise, the return value is $false$.
	//
	//# \also	$@OcclusionRegion::BoxOccluded@$
	//# \also	$@OcclusionRegion::QuadOccluded@$
	//# \also	$@OcclusionRegion::PolygonOccluded@$
	//# \also	$@OcclusionRegion::SphereOccluded@$
	//# \also	$@OcclusionRegion::EllipsoidOccluded@$


	class OcclusionRegion : public ListElement<OcclusionRegion>, public EngineMemory<OcclusionRegion>
	{
		private:

			unsigned_int32		occlusionMask;

			int32				planeCount;
			Antivector4D		planeArray[kMaxPolyhedronFaceCount];

		public:

			OcclusionRegion(unsigned_int32 mask = ~0U)
			{
				occlusionMask = mask;
			}

			OcclusionRegion(const FrustumCamera *camera, int32 vertexCount, const Point3D *vertex, int32 frontPlaneCount, const Antivector4D *frontPlane, unsigned_int32 mask = ~0U);

			unsigned_int32 GetOcclusionMask(void) const
			{
				return (occlusionMask);
			}

			int32 GetPlaneCount(void) const
			{
				return (planeCount);
			}

			void SetPlaneCount(int32 count)
			{
				Assert(count <= kMaxPolyhedronFaceCount, "OcclusionRegion::SetPlaneCount(), count too large\n");
				planeCount = count;
			}

			Antivector4D *GetPlaneArray(void)
			{
				return (planeArray);
			}

			const Antivector4D *GetPlaneArray(void) const
			{
				return (planeArray);
			}

			C4API bool BoxOccluded(const Box3D& box) const;
			C4API bool BoxOccluded(const Point3D& center, const Vector3D *axis) const;
			C4API bool BoxOccluded(const Point3D& center, const Vector3D& size) const;
			C4API bool DirectionOccluded(const Vector3D& center, float radius) const;
			C4API bool QuadOccluded(const Point3D *vertex) const;
			C4API bool PolygonOccluded(int32 vertexCount, const Point3D *vertex) const;
			C4API bool SphereOccluded(const Point3D& center, float radius) const;
			C4API bool EllipsoidOccluded(const Point3D& center, const Vector3D *axis) const;
			C4API bool CylinderOccluded(const Point3D& p1, const Point3D& p2, float radius) const;
	};


	class GenericRegion : public VisibilityRegion
	{
		private:

			Convexity		regionConvexity;

		public:

			GenericRegion() : VisibilityRegion(&regionConvexity)
			{
			}
	};


	class ShadowRegion : public VisibilityRegion, public ListElement<ShadowRegion>, public EngineMemory<ShadowRegion>
	{
		private:

			Zone			*regionZone;

			Convexity		regionConvexity;
			Antivector4D	portalExtrusionPlane[kMaxPortalVertexCount];	// Continuation of plane array in convexity.

		public:

			ShadowRegion(Zone *zone) : VisibilityRegion(&regionConvexity)
			{
				regionZone = zone;
			}

			ShadowRegion(Zone *zone, const ShadowRegion *shadowRegion);

			ShadowRegion *GetPreviousShadowRegion(void) const
			{
				return (ListElement<ShadowRegion>::Previous());
			}

			ShadowRegion *GetNextShadowRegion(void) const
			{
				return (ListElement<ShadowRegion>::Next());
			}

			Zone *GetZone(void) const
			{
				return (regionZone);
			}

			Convexity *GetRegionConvexity(void)
			{
				return (&regionConvexity);
			}
	};


	class CameraRegion : public VisibilityRegion, public ListElement<CameraRegion>, public Tree<CameraRegion>, public EngineMemory<CameraRegion>
	{
		private:

			const Camera					*regionCamera;
			Zone							*regionZone;

			Array<const LightRegion *, 8>	lightRegionArray;

			Polyhedron						regionPolyhedron;

		public:

			CameraRegion(const Camera *camera, Zone *zone);
			CameraRegion(const CameraRegion *region, const Polyhedron *polyhedron);

			CameraRegion *GetPreviousCameraRegion(void) const
			{
				return (ListElement<CameraRegion>::Previous());
			}

			CameraRegion *GetNextCameraRegion(void) const
			{
				return (ListElement<CameraRegion>::Next());
			}

			const Camera *GetCamera(void) const
			{
				return (regionCamera);
			}

			Zone *GetZone(void) const
			{
				return (regionZone);
			}

			const ImmutableArray<const LightRegion *>& GetShadowCastingLightRegionArray(void) const
			{
				return (lightRegionArray);
			}

			void AddShadowCastingLightRegion(const LightRegion *region)
			{
				lightRegionArray.AddElement(region);
			}

			Polyhedron *GetRegionPolyhedron(void)
			{
				return (&regionPolyhedron);
			}

			const Polyhedron *GetRegionPolyhedron(void) const
			{
				return (&regionPolyhedron);
			}

			bool ContainsPoint(const Point3D& position) const;

			void SetPolygonExtrusion(int32 vertexCount, const Point3D *vertex, const Point3D& cameraPosition, const Vector3D& viewDirection, float farDepth, const Antivector4D& frontPlane);
	};


	class RootCameraRegion : public CameraRegion, public ListElement<RootCameraRegion>
	{
		public:

			RootCameraRegion(const Camera *camera, Zone *zone) : CameraRegion(camera, zone)
			{
			}

			using ListElement<RootCameraRegion>::Previous;
			using ListElement<RootCameraRegion>::Next;
	};


	class LightRegion : public VisibilityRegion, public ListElement<LightRegion>, public Tree<LightRegion>, public EngineMemory<LightRegion>
	{
		private:

			Light					*regionLight;
			Zone					*regionZone;

			const Portal			*illuminatedPortal;
			unsigned_int32			regionStamp;

			Polyhedron				regionPolyhedron;

		public:

			LightRegion(Light *light, Zone *zone, const Portal *portal = nullptr);
			LightRegion(Light *light, Zone *zone, const Portal *portal, const LightRegion *region);
			LightRegion(const LightRegion *region, const Polyhedron *polyhedron);

			LightRegion *GetPreviousLightRegion(void) const
			{
				return (ListElement<LightRegion>::Previous());
			}

			LightRegion *GetNextLightRegion(void) const
			{
				return (ListElement<LightRegion>::Next());
			}

			Light *GetLight(void) const
			{
				return (regionLight);
			}

			Zone *GetZone(void) const
			{
				return (regionZone);
			}

			const Portal *GetIlluminatedPortal(void) const
			{
				return (illuminatedPortal);
			}

			unsigned_int32 GetRegionStamp(void) const
			{
				return (regionStamp);
			}

			void SetRegionStamp(unsigned_int32 stamp)
			{
				regionStamp = stamp;
			}

			Polyhedron *GetRegionPolyhedron(void)
			{
				return (&regionPolyhedron);
			}

			const Polyhedron *GetRegionPolyhedron(void) const
			{
				return (&regionPolyhedron);
			}

			void SetInfinitePolygonExtrusion(int32 vertexCount, const Point3D *vertex, const Vector3D& lightDirection, const Antivector4D& frontPlane);
			void SetPointPolygonExtrusion(int32 vertexCount, const Point3D *vertex, const Point3D& lightPosition, float lightRange, float confinementRadius, const Portal *portal);
	};


	class RootLightRegion : public LightRegion, public ListElement<RootLightRegion>
	{
		public:

			RootLightRegion(Light *light, Zone *zone) : LightRegion(light, zone)
			{
			}

			using ListElement<RootLightRegion>::Previous;
			using ListElement<RootLightRegion>::Next;
	};


	#if C4DIAGS

		class RegionRenderable : public Renderable
		{
			private:

				VertexBuffer		vertexBuffer;
				VertexBuffer		indexBuffer;

				List<Attribute>		attributeList;
				DiffuseAttribute	diffuseColor;

			public:

				RegionRenderable(const Polyhedron *polyhedron);
				~RegionRenderable();

				const ColorRGBA& GetRegionColor(void) const
				{
					return (diffuseColor.GetDiffuseColor());
				}

				void SetRegionColor(const ColorRGBA& color)
				{
					diffuseColor.SetDiffuseColor(color);
				}
		};

	#endif
}


#endif

// ZYUQURM
