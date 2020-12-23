 

#ifndef C4GeometryObjects_h
#define C4GeometryObjects_h


//# \component	World Manager
//# \prefix		WorldMgr/

//# \import		C4Node.h


#include "C4Objects.h"
#include "C4Bounding.h"
#include "C4Mesh.h"
#include "C4Node.h"


namespace C4
{
	typedef Type	GeometryType;
	typedef Type	PrimitiveType;


	enum : ObjectType
	{
		kObjectGeometry			= 'GEOM'
	};


	enum : GeometryType
	{
		kGeometryGeneric		= 'GNRC'
	};


	//# \enum	GeometryFlags

	enum
	{
		kGeometryInvisible				= 1 << 1,		//## The geometry is invisible (but can still participate in collision detection).
		kGeometryAmbientOnly			= 1 << 2,		//## The geometry is rendered only in ambient light.
		kGeometryCastShadows			= 1 << 4,		//## The geometry casts shadows.
		kGeometryCubeLightInhibit		= 1 << 6,		//## Render with point light shaders when illuminated by a cube light.
		kGeometryMarkingFullPolygon		= 1 << 12,		//## Surface markings applied to the geometry are always created with complete, unclipped polygons from the mesh.
		kGeometryMarkingInhibit			= 1 << 13,		//## Surface markings are never applied to the geometry.
		kGeometryFogInhibit				= 1 << 14,		//## Fog is not applied to the geometry.
		kGeometryShaderDetailEnable		= 1 << 18,		//## Multiple shader detail levels are enabled.
		kGeometryDynamic				= 1 << 19,		//## The geometry is dynamic and could move or deform.
		kGeometryMotionBlurInhibit		= 1 << 23,		//## The geometry does not get rendered with motion blur.
		kGeometryRemotePortal			= 1 << 24,		//## The geometry covers a remote portal and should be rendered first when visible.
		kGeometryRenderEffectPass		= 1 << 25,		//## The geometry is rendered after lighting in the effect pass.
		kGeometryRenderDecal			= 1 << 26,		//## The geometry is rendered with depth offset for decaling.
		kGeometryConvexHull				= 1 << 27,		//## The geometry's convex hull is used for collision detection.
		kGeometryInstancePaintSpace		= 1 << 28,		//## If the geometry is not directly connected to a paint space, then it should use the paint space connected by an instance super node.

		kGeometryModelExportFlags		= kGeometryDynamic,
		kGeometryDeformModelFlags		= kGeometryModelExportFlags | kGeometryMarkingInhibit
	};


	//# \enum	GeometryEffectFlags

	enum
	{
		kGeometryEffectShader			= 1 << 0,
		kGeometryEffectAccumulate		= 1 << 1,		//## Use an additive blending mode instead of alpha interpolation.
		kGeometryEffectOpaque			= 1 << 2		//## Render the geometry in the opaque effect stage instead of the transparent effect stage.
	};


	enum
	{
		kGeometryObjectPrototype		= 1 << 0,
		kGeometryObjectPreprocessed		= 1 << 1,
		kGeometryObjectStaticSurfaces	= 1 << 2,
		kGeometryObjectConvexPrimitive	= 1 << 3
	};


	enum BooleanOperation
	{
		kBooleanUnion,
		kBooleanIntersection
	};


	class Zone;
	class Geometry;
	class Manipulator;
	struct BooleanLoop;


	struct SkinData
	{
		int32					boneCount;
		const unsigned_int32	*boneHashArray;
		const Transform4D		*inverseBindTransformArray;
		const SkinWeight		*const *skinWeightTable;
	};


	struct MorphData 
	{
		int32					morphCount;
		const bool				*relativeArray; 
		const int32				*baseIndexArray;
		const unsigned_int32	*morphHashArray; 
		const Point3D			*const *morphPositionTable;
		const Vector3D			*const *morphNormalTable;
	}; 

 
	struct GeometryHitData 
	{
		Point3D				position;
		Vector3D			normal;
		float				param; 
		unsigned_int32		triangleIndex;
	};


	struct CollisionOctree
	{
		Box3D				collisionBounds;
		unsigned_int16		subnodeOffset[8];
		unsigned_int16		elementCount;
		unsigned_int16		offsetAlign;

		CollisionOctree *GetSubnode(int32 index)
		{
			return (reinterpret_cast<CollisionOctree *>(reinterpret_cast<char *>(this) + subnodeOffset[index] * offsetAlign));
		}

		const CollisionOctree *GetSubnode(int32 index) const
		{
			return (reinterpret_cast<const CollisionOctree *>(reinterpret_cast<const char *>(this) + subnodeOffset[index] * offsetAlign));
		}

		unsigned_int16 *GetIndexArray(void)
		{
			return (reinterpret_cast<unsigned_int16 *>(this + 1));
		}

		const unsigned_int16 *GetIndexArray(void) const
		{
			return (reinterpret_cast<const unsigned_int16 *>(this + 1));
		}
	};


	class GeometryOctree : public Memory<GeometryOctree>
	{
		private:

			enum
			{
				kOctantX		= 1 << 0,
				kOctantY		= 1 << 1,
				kOctantZ		= 1 << 2
			};

			GeometryOctree				*subnode[8];

			Point3D						octreeCenter;
			Vector3D					octreeSize;

			Array<unsigned_int32, 8>	indexArray;

			int32 ClassifyPoint(const Vector3D& p) const;

		public:

			GeometryOctree();
			GeometryOctree(const Box3D& bounds);
			GeometryOctree(const GeometryOctree *octree, int32 subnodeIndex);
			~GeometryOctree();

			GeometryOctree *GetSubnode(int32 octant) const
			{
				return (subnode[octant]);
			}

			void SetSubnode(int32 octant, GeometryOctree *node)
			{
				subnode[octant] = node;
			}

			const Point3D& GetCenter(void) const
			{
				return (octreeCenter);
			}

			void SetCenter(const Point3D& center)
			{
				octreeCenter = center;
			}

			const Vector3D& GetSize(void) const
			{
				return (octreeSize);
			}

			void SetSize(const Vector3D& size)
			{
				octreeSize = size;
			}

			int32 GetIndexCount(void) const
			{
				return (indexArray.GetElementCount());
			}

			const unsigned_int32 *GetIndexArray(void) const
			{
				return (indexArray);
			}

			void AddIndex(unsigned_int32 index)
			{
				indexArray.AddElement(index);
			}

			GeometryOctree *FindNodeContainingTriangle(const Vector3D& p1, const Vector3D& p2, const Vector3D& p3, int32 maxDepth = 12);
	};


	//# \class	GeometryObject	Encapsulates data for a geometry.
	//
	//# The $GeometryObject$ class encapsulates data for a geometry.
	//
	//# \def	class GeometryObject : public Object
	//
	//# \ctor	GeometryObject(GeometryType type);
	//
	//# The constructor has protected access. The $GeometryObject$ class can only exist as the base class for a more specific type of geometry.
	//
	//# \param	type	The geometry type.
	//
	//# \desc
	//# The $GeometryObject$ class all of the geometric information pertaining to a geometry. Each geometry object has
	//# one or more levels of detail, and the geometric data (such as vertex and triangle arrays) for each level is stored
	//# in a $@Mesh@$ object.
	//
	//# \base	Object		A $GeometryObject$ is an object that can be shared by multiple geometry nodes.
	//
	//# \also	$@Mesh@$
	//# \also	$@Geometry@$
	//
	//# \wiki	Geometries


	//# \function	GeometryObject::GetGeometryType		Returns the geometry type.
	//
	//# \proto	GeometryType GetGeometryType(void) const;
	//
	//# \desc
	//# The $GetGeometryType$ function returns the geometry type.


	//# \function	GeometryObject::GetGeometryFlags		Returns the geometry flags.
	//
	//# \proto	unsigned_int32 GetGeometryFlags(void) const;
	//
	//# \desc
	//# The $GetGeometryFlags$ function returns the geometry flags, which can be a combination (through logical OR) of the
	//# following bit flags.
	//
	//# \table	GeometryFlags
	//
	//# By default, none of the geometry flags are set.
	//
	//# \also	$@GeometryObject::SetGeometryFlags@$


	//# \function	GeometryObject::SetGeometryFlags		Sets the geometry flags.
	//
	//# \proto	void SetGeometryFlags(unsigned_int32 flags);
	//
	//# \param	flags	The new geometry flags.
	//
	//# \desc
	//# The $SetGeometryFlags$ function sets the geometry flags. The $flags$ parameter may be any
	//# combination of the following bit flags.
	//
	//# \table	GeometryFlags
	//
	//# By default, none of the geometry flags are set.
	//
	//# \also	$@GeometryObject::GetGeometryFlags@$


	//# \function	GeometryObject::GetGeometryLevelCount		Returns the number of detail levels.
	//
	//# \proto	int32 GetGeometryLevelCount(void) const;
	//
	//# \desc
	//# The $GetGeometryLevelCount$ function returns the number of detail levels belonging to a geometry object.
	//# The geometric information for a particular detail level can be retrieved using the $@GeometryObject::GetGeometryLevel@$ function.
	//
	//# \also	$@GeometryObject::GetGeometryLevel@$


	//# \function	GeometryObject::GetGeometryLevel		Returns a specific geometric level of detail.
	//
	//# \proto	Mesh *GetGeometryLevel(int32 level) const;
	//
	//# \param	level	The detail level to retrieve.
	//
	//# \desc
	//# The $GetGeometryLevel$ function returns the a pointer to the $@Mesh@$ object for the detail level
	//# specified by the $level$ parameter. The number of detail levels can be determined by calling the
	//# $@GeometryObject::GetGeometryLevelCount@$ function. The $level$ parameter must be in the range
	//# [0,&nbsp;<i>n</i>&nbsp;&minus;&nbsp;1], where <i>n</i> is the number of detail levels.
	//
	//# \also	$@GeometryObject::GetGeometryLevelCount@$
	//# \also	$@Mesh@$


	//# \function	GeometryObject::GetCollisionExclusionMask		Returns the collision exclusion mask.
	//
	//# \proto	unsigned_int32 GetCollisionExclusionMask(void) const;
	//
	//# \desc
	//# The $GetCollisionExclusionMask$ function returns the collision exclusion mask, which may be a combination
	//# (through logical OR) of the following bit flags.
	//
	//# \table	CollisionKind
	//
	//# The collision exclusion mask is used to invalidate collisions with rigid bodies having specific collision kinds.
	//# The mask is also used to invalidate collisions that are detected by the $@World::DetectCollision@$
	//# and $@World::QueryCollision@$ functions.
	//
	//# \also	$@GeometryObject::SetCollisionExclusionMask@$
	//# \also	$@PhysicsMgr/RigidBodyController::GetCollisionKind@$
	//# \also	$@PhysicsMgr/RigidBodyController::SetCollisionKind@$
	//# \also	$@PhysicsMgr/RigidBodyController::ValidGeometryCollision@$
	//# \also	$@World::DetectCollision@$
	//# \also	$@World::QueryCollision@$


	//# \function	GeometryObject::SetCollisionExclusionMask		Sets the collision exclusion mask.
	//
	//# \proto	void SetCollisionExclusionMask(unsigned_int32 mask);
	//
	//# \param	mask	The new collision exclusion mask.
	//
	//# \desc
	//# The $SetCollisionExclusionMask$ function sets the collision mask. The $mask$ parameter may be a
	//# combination (through logical OR) of the following bit flags.
	//
	//# \table	CollisionKind
	//
	//# The collision exclusion mask is used to invalidate collisions with rigid bodies having specific collision kinds.
	//# The mask is also used to invalidate collisions that are detected by the $@World::DetectCollision@$
	//# and $@World::QueryCollision@$ functions.
	//
	//# \also	$@GeometryObject::GetCollisionExclusionMask@$
	//# \also	$@PhysicsMgr/RigidBodyController::GetCollisionKind@$
	//# \also	$@PhysicsMgr/RigidBodyController::SetCollisionKind@$
	//# \also	$@PhysicsMgr/RigidBodyController::ValidGeometryCollision@$
	//# \also	$@World::DetectCollision@$
	//# \also	$@World::QueryCollision@$


	//# \function	GeometryObject::GetCollisionLevel		Returns the index of the detail level used for collision detection.
	//
	//# \proto	int32 GetCollisionLevel(void) const;
	//
	//# \desc
	//# The $GetCollisionLevel$ function returns the index of the detail level used for collision detection.
	//
	//# \also	$@GeometryObject::SetCollisionLevel@$
	//# \also	$@GeometryObject::GetCollisionExclusionMask@$
	//# \also	$@GeometryObject::SetCollisionExclusionMask@$
	//# \also	$@World::DetectCollision@$
	//# \also	$@World::QueryCollision@$


	//# \function	GeometryObject::SetCollisionLevel		Sets the index of the detail level used for collision detection.
	//
	//# \proto	void SetCollisionLevel(int32 level) const;
	//
	//# \param	level	The detail level index.
	//
	//# \desc
	//# The $SetCollisionLevel$ function sets the index of the detail level used for collision detection. The $level$
	//# parameter should be in the range [0,&nbsp;<i>n</i>&nbsp;&minus;&nbsp;1], where <i>n</i> is the number of detail levels.
	//# If the $level$ parameter is greater than or equal to <i>n</i>, then the collision level is set to <i>n</i>&nbsp;&minus;&nbsp;1.
	//
	//# \also	$@GeometryObject::GetCollisionLevel@$
	//# \also	$@GeometryObject::GetCollisionExclusionMask@$
	//# \also	$@GeometryObject::SetCollisionExclusionMask@$
	//# \also	$@World::DetectCollision@$
	//# \also	$@World::QueryCollision@$


	class GeometryObject : public Object
	{
		friend class WorldMgr;

		private:

			GeometryType			geometryType;

			unsigned_int32			geometryFlags;
			unsigned_int32			geometryEffectFlags;

			float					geometryDetailBias;
			float					shaderDetailBias;

			unsigned_int16			geometryObjectFlags;
			unsigned_int16			dynamicArrayFlags;

			unsigned_int32			collisionExclusionMask;
			int32					collisionLevel;

			int32					geometryLevelCount;
			Mesh					*geometryLevel;

			int32					surfaceCount;
			SurfaceData				*surfaceData;

			CollisionOctree			*collisionOctree;
			unsigned_int32			collisionOctreeSize;

			int32					convexHullVertexCount;
			unsigned_int16			*convexHullIndexArray;

			unsigned_int32			staticVertexStride;
			VertexBuffer			staticVertexBuffer;
			VertexBuffer			staticIndexBuffer;

			void Initialize(void);

			static GeometryObject *Create(Unpacker& data, unsigned_int32 unpackFlags);

			void BuildStaticVertexBuffer(VertexBuffer *vertexBuffer);
			void BuildStaticIndexBuffer(VertexBuffer *indexBuffer);

			static unsigned_int32 GetCompressedOctreeSize(const GeometryOctree *geometryOctree);
			static char *CompressOctree(const GeometryOctree *geometryOctree, CollisionOctree *collisionOctree);

			GeometryOctree *BuildCollisionOctree(const Mesh *level, const Box3D& boundingBox);

			static bool ClipSegmentToCollisionBounds(const Box3D& bounds, float radius, Point3D& p1, Point3D& p2);
			static bool DetectSegmentIntersection(const CollisionOctree *octree, const Mesh *level, const Point3D& p1, const Point3D& p2, GeometryHitData *geometryHitData);
			static bool DetectSegmentEdgeIntersection(const Bivector4D& segmentLine, const Bivector4D& edgeLine, const Point3D& p1, const Vector3D& v1, float r2, float& smax, GeometryHitData *geometryHitData);
			static bool DetectSegmentVertexIntersection(const Bivector4D& segmentLine, const Point3D& p1, const Vector3D& v1, float r2, float a, float ainv, float& smax, GeometryHitData *geometryHitData);
			static bool DetectSegmentIntersection(const CollisionOctree *octree, const Mesh *level, const Point3D& p1, const Point3D& p2, float radius, GeometryHitData *geometryHitData);

			static void ScaleCollisionOctree(CollisionOctree *octree, float factor);
			static void OffsetCollisionOctree(CollisionOctree *octree, const Vector3D& dv);

		protected:

			GeometryObject(GeometryType type);
			~GeometryObject();

			void SetConvexPrimitiveFlag(void)
			{
				geometryObjectFlags |= kGeometryObjectConvexPrimitive;
			}

			void ClearConvexPrimitiveFlag(void)
			{
				geometryObjectFlags &= ~kGeometryObjectConvexPrimitive;
			}

			C4API void SetStaticSurfaceData(int32 count, SurfaceData *data, bool init = false);

			void ResetVertexBuffers(void);

		public:

			C4API GeometryObject(GeometryType type, int32 levelCount);

			GeometryType GetGeometryType(void) const
			{
				return (geometryType);
			}

			unsigned_int32 GetGeometryFlags(void) const
			{
				return (geometryFlags);
			}

			void SetGeometryFlags(unsigned_int32 flags)
			{
				geometryFlags = flags;
			}

			unsigned_int32 GetGeometryEffectFlags(void) const
			{
				return (geometryEffectFlags);
			}

			void SetGeometryEffectFlags(unsigned_int32 flags)
			{
				geometryEffectFlags = flags;
			}

			float GetGeometryDetailBias(void) const
			{
				return (geometryDetailBias);
			}

			void SetGeometryDetailBias(float bias)
			{
				geometryDetailBias = bias;
			}

			float GetShaderDetailBias(void) const
			{
				return (shaderDetailBias);
			}

			void SetShaderDetailBias(float bias)
			{
				shaderDetailBias = bias;
			}

			void SetPrototypeFlag(void)
			{
				geometryObjectFlags |= kGeometryObjectPrototype;
			}

			bool GetConvexPrimitiveFlag(void) const
			{
				return ((geometryObjectFlags & kGeometryObjectConvexPrimitive) != 0);
			}

			unsigned_int32 GetStaticVertexStride(void) const
			{
				return (staticVertexStride);
			}

			const VertexBuffer *GetStaticVertexBuffer(void) const
			{
				return (&staticVertexBuffer);
			}

			const VertexBuffer *GetStaticIndexBuffer(void) const
			{
				return (&staticIndexBuffer);
			}

			unsigned_int32 GetCollisionExclusionMask(void) const
			{
				return (collisionExclusionMask);
			}

			void SetCollisionExclusionMask(unsigned_int32 mask)
			{
				collisionExclusionMask = mask;
			}

			int32 GetCollisionLevel(void) const
			{
				return (collisionLevel);
			}

			void SetCollisionLevel(int32 level)
			{
				collisionLevel = Min(level, GetMaxCollisionLevel());
			}

			int32 GetGeometryLevelCount(void) const
			{
				return (geometryLevelCount);
			}

			Mesh *GetGeometryLevel(int32 level) const
			{
				return (&geometryLevel[level]);
			}

			int32 GetSurfaceCount(void) const
			{
				return (surfaceCount);
			}

			SurfaceData *GetSurfaceData(int32 index = 0) const
			{
				return (&surfaceData[index]);
			}

			const CollisionOctree *GetCollisionOctree(void) const
			{
				return (collisionOctree);
			}

			const Point3D *GetConvexHullVertexArray(void) const
			{
				return (geometryLevel[collisionLevel].GetArray<Point3D>(kArrayPosition));
			}

			Point3D GetInitialConvexHullSupportPoint(const Point3D *vertex) const
			{
				return (vertex[convexHullIndexArray[0]]);
			}

			C4API int32 Release(void) override;

			void PackType(Packer& data) const override;
			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;

			int32 GetCategoryCount(void) const override;
			Type GetCategoryType(int32 index, const char **title) const override;
			int32 GetCategorySettingCount(Type category) const override;
			Setting *GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const override;
			void SetCategorySetting(Type category, const Setting *setting) override;

			virtual void Preprocess(unsigned_int32 dynamicFlags);
			virtual void Neutralize(void);

			C4API void SetGeometryLevelCount(int32 levelCount);
			C4API void SetSurfaceCount(int32 count);

			C4API void BuildCollisionData(void);
			C4API void ScaleCollisionData(float factor);
			C4API void OffsetCollisionData(const Vector3D& dv);

			const Point3D& CalculateConvexHullSupportPoint(const Point3D *vertex, const Vector3D& direction) const;
			void CalculateConvexHullSupportPointArray(const Point3D *vertex, int32 count, const Vector3D *direction, Point3D *support) const;

			virtual int32 GetMaxCollisionLevel(void) const;
			virtual bool DetectCollision(const Point3D& p1, const Point3D& p2, float radius, GeometryHitData *geometryHitData) const;

			virtual bool ExteriorSphere(const Point3D& center, float radius) const;
			virtual bool ExteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const;
	};


	//# \class	GenericGeometryObject	Encapsulates data for a generic geometry.
	//
	//# The $GenericGeometryObject$ class encapsulates data for a generic geometry.
	//
	//# \def	class GenericGeometryObject final : public GeometryObject
	//
	//# \ctor	GenericGeometryObject(const Geometry *geometry);
	//
	//# \param	geometry	A pointer to another geometry node that is copied into the generic geometry.
	//
	//# \desc
	//
	//# \base	GeometryObject		A $GenericGeometryObject$ is an object that can be shared by multiple generic geometry nodes.
	//
	//# \also	$@GenericGeometry@$


	class GenericGeometryObject final : public GeometryObject
	{
		friend class GeometryObject;

		private:

			BoundingSphere		boundingSphere;
			Box3D				boundingBox;

			~GenericGeometryObject();

			void BuildMesh(int32 level, unsigned_int32 flags, const List<GeometrySurface> *surfaceList, int32 materialCount, const SkinData *skinData = nullptr, const MorphData *morphData = nullptr);

			static unsigned_int32 IntersectMeshes(const Mesh *targetMesh, const Mesh *auxMesh, List<GeometrySurface> *resultList, const Geometry *targetGeometry, Array<SurfaceData> *surfaceDataArray, const ImmutableArray<MaterialObject *>& materialArray);
			static void IntersectPolygonAndMesh(const Point3D *polygonVertex, const Vector3D *polygonNormal, const ColorRGBA *polygonColor, const Point2D *polygonTexcoord, const Mesh *mesh, float geometryVolume, List<GeometryPolygon> *resultList);
			static void BuildBooleanLoops(const Antivector4D& plane, const Mesh *mesh, List<BooleanLoop> *positiveList, List<BooleanLoop> *negativeList);
			static void ConvexDecomposeLoop(const Vector3D& normal, BooleanLoop *inputLoop, List<BooleanLoop> *outputList);
			static void CalculatePolygonAttributes(const Point3D *polygonVertex, const Vector3D *polygonNormal, const ColorRGBA *polygonColor, const Point2D *polygonTexcoord, int32 vertexCount, const Point3D *vertex, Vector3D *normal, ColorRGBA *color, Point2D *texcoord);

		public:

			C4API GenericGeometryObject();
			C4API GenericGeometryObject(int32 levelCount);
			C4API GenericGeometryObject(const Geometry *geometry);
			C4API GenericGeometryObject(int32 levelCount, const List<GeometrySurface> *const *surfaceListTable, int32 surfaceCount, const ImmutableArray<int32>& materialIndexArray, const SkinData *const *skinDataTable = nullptr, const MorphData *const *morphDataTable = nullptr);
			C4API GenericGeometryObject(int32 geometryCount, const Geometry *const *geometryArray, const ImmutableArray<MaterialObject *>& materialArray, const Transformable *transformable);
			C4API GenericGeometryObject(BooleanOperation operation, const Geometry *geometry1, const Geometry *geometry2, const ImmutableArray<MaterialObject *>& materialArray);

			BoundingSphere *GetBoundingSphere(void)
			{
				return (&boundingSphere);
			}

			const BoundingSphere *GetBoundingSphere(void) const
			{
				return (&boundingSphere);
			}

			void SetBoundingSphere(const BoundingSphere *sphere)
			{
				boundingSphere = *sphere;
			}

			void SetBoundingSphere(const Point3D& center, float radius)
			{
				boundingSphere.SetCenter(center);
				boundingSphere.SetRadius(radius);
			}

			const Box3D& GetBoundingBox(void) const
			{
				return (boundingBox);
			}

			void SetBoundingBox(const Box3D& box)
			{
				boundingBox = box;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			C4API int32 GetObjectSize(float *size) const override;
			C4API void SetObjectSize(const float *size) override;

			bool ExteriorSphere(const Point3D& center, float radius) const override;
			bool ExteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const override;

			C4API void UpdateBounds(void);
			C4API void Rebuild(const Geometry *geometry);
	};
}


#endif

// ZYUQURM
