 

#ifndef C4Geometries_h
#define C4Geometries_h


//# \component	World Manager
//# \prefix		WorldMgr/

//# \import		C4World.h


#include "C4GeometryObjects.h"
#include "C4Spaces.h"


namespace C4
{
	C4API extern const char kConnectorKeyPaint[];


	//# \class	Geometry	Represents a geometry node in a world.
	//
	//# The $Geometry$ class represents a geometry node in a world.
	//
	//# \def	class Geometry : public RenderableNode
	//
	//# \ctor	Geometry(GeometryType type);
	//
	//# The constructor has protected access. A $Geometry$ class can only exist as the base class for another class.
	//
	//# \desc
	//# The $Geometry$ class serves as the base class for all geometrical nodes in the world. The geometrical information
	//# itself is stored in the associated $@Mesh@$ class and $@GeometryObject@$ class.
	//#
	//# A geometry node can be a generic geometry, a type of primitive geometry, a chunk of voxel terrain, or a piece of a water surface.
	//# See the <a href="Node_tree.html">node class hierarchy</a> for a diagram showing the relationships among these types.
	//
	//# \base	RenderableNode		A $Geometry$ node is a renderable scene graph node.
	//
	//# \also	$@Mesh@$
	//# \also	$@GeometryObject@$
	//# \also	$@GraphicsMgr/MaterialObject@$
	//
	//# \wiki	Geometries


	//# \function	Geometry::GetGeometryType		Returns the geometry type.
	//
	//# \proto	GeometryType GetGeometryType(void) const;
	//
	//# \desc
	//# The $GetGeometryType$ function returns the geometry type. It can be one of the following constants.
	//
	//# \value	kGeometryGeometry		A generic geometry.
	//# \value	kGeometryPrimitive		A primitive geometry.
	//# \value	kGeometryTerrain		A chunk of voxel terrain.


	//# \function	Geometry::GetMaterialCount		Returns the number of material slots.
	//
	//# \proto	int32 GetMaterialCount(void) const;
	//
	//# \desc
	//# The $GetMaterialCount$ function returns the number of material slots allocated for a geometry.
	//# The return value is always at least 1.
	//
	//# \also	$@Geometry::SetMaterialCount@$
	//# \also	$@Geometry::GetMaterialObject@$
	//# \also	$@Geometry::SetMaterialObject@$


	//# \function	Geometry::SetMaterialCount		Sets the number of material slots.
	//
	//# \proto	void SetMaterialCount(int32 count);
	//
	//# \param	count	The new number of material slots. This cannot be less than 1.
	//
	//# \desc
	//# The $SetMaterialCount$ function sets the number of material slots allocated for a geometry to the
	//# number specified by the $count$ parameter. If the new number of material slots is less than its
	//# previous value, then any material objects assigned to slots with indexes greater than or equal to
	//# $count$ are released.
	//
	//# \also	$@Geometry::GetMaterialCount@$
	//# \also	$@Geometry::GetMaterialObject@$
	//# \also	$@Geometry::SetMaterialObject@$


	//# \function	Geometry::GetMaterialObject		Returns a material object.
	//
	//# \proto	MaterialObject *GetMaterialObject(unsigned_int32 index) const;
	//
	//# \param	index		The index of the material slot from which to retrieve a material object.
	//
	//# \desc
	//# The $GetMaterialObject$ function returns one of the material objects assigned to a geometry node.
	//# For a geometry having <i>n</i> materials, the $index$ parameter should be an integer between 0 and
	//# <i>n</i>&nbsp;&minus;&nbsp;1. If no material object has been assigned for the specified index, then
	//# this function returns $nullptr$.
	//#
	//# The number of material slots can be determined using the $@Geometry::GetMaterialCount@$ function.
	// 
	//# \also	$@GraphicsMgr/MaterialObject@$
	//# \also	$@Geometry::SetMaterialObject@$
	//# \also	$@Geometry::GetMaterialCount@$ 
	//# \also	$@Geometry::SetMaterialCount@$
 

	//# \function	Geometry::SetMaterialObject		Sets a material object.
	// 
	//# \proto	void SetMaterialObject(unsigned_int32 index, MaterialObject *object);
	// 
	//# \param	index		The index of the material slot to which a material object is to be assigned. 
	//# \param	object		The new material object. This can be $nullptr$.
	//
	//# \desc
	//# The $SetMaterialObject$ function assigns the material object specified by the $object$ parameter 
	//# to a geometry node in the material slot specified by the $index$ parameter. If $object$ is $nullptr$,
	//# then the geometry node does not have a material in the specified material slot after this function is
	//# called. Otherwise, the reference count of the material object is incremented, and the new material
	//# object is assigned to the geometry node. The reference count of any material object previously
	//# assigned to the geometry node in the same slot is decremented, and the old material object is
	//# deleted if its reference count reaches zero.
	//#
	//# The number of material slots can be determined using the $@Geometry::GetMaterialCount@$ function.
	//
	//# \also	$@GraphicsMgr/MaterialObject@$
	//# \also	$@Geometry::GetMaterialObject@$
	//# \also	$@Geometry::GetMaterialCount@$
	//# \also	$@Geometry::SetMaterialCount@$


	class Geometry : public RenderableNode
	{
		friend class Node;

		private:

			GeometryType			geometryType;

			int32					geometryDetailLevel;
			int32					minGeometryDetailLevel;

			int8					ambientRenderStage;
			int8					lightRenderStage;
			int8					shadowRenderStage;

			unsigned_int32			processStamp;
			unsigned_int32			lightStamp;
			unsigned_int32			shadowStamp;
			unsigned_int32			collisionStamp;
			volatile int32			queryThreadFlags;

			Vector3D				geometryVelocity;

			int32					materialCount;
			MaterialObject			*materialObject;
			char					*segmentStorage;

			PaintSpace				*connectedPaintSpace;

			const ArrayBundle		*positionArrayBundle;
			const ArrayBundle		*primitiveArrayBundle;

			MaterialObject **GetMaterialObjectTable(void) const
			{
				return (reinterpret_cast<MaterialObject **>(segmentStorage));
			}

			RenderSegment *GetRenderSegmentTable(void) const
			{
				return (reinterpret_cast<RenderSegment *>(GetMaterialObjectTable() + (materialCount - 1)));
			}

			static Geometry *Create(Unpacker& data, unsigned_int32 unpackFlags);
			static void MaterialObjectLinkProc(Object *object, void *cookie);

			void ReleaseSegmentStorage(void);

			void HandleVisibilityUpdate(void) override;

			bool AlphaTestMaterial(void) const;

		protected:

			Geometry(GeometryType type);
			Geometry(const Geometry& geometry);

		public:

			virtual ~Geometry();

			GeometryType GetGeometryType(void) const
			{
				return (geometryType);
			}

			GeometryObject *GetObject(void) const
			{
				return (static_cast<GeometryObject *>(Node::GetObject()));
			}

			int32 GetDetailLevel(void) const
			{
				return (geometryDetailLevel);
			}

			int32 GetMinDetailLevel(void) const
			{
				return (minGeometryDetailLevel);
			}

			void SetMinDetailLevel(int32 level)
			{
				minGeometryDetailLevel = level;
			}

			int32 GetAmbientRenderStage(void) const
			{
				return (ambientRenderStage);
			}

			int32 GetLightRenderStage(void) const
			{
				return (lightRenderStage);
			}

			int32 GetShadowRenderStage(void) const
			{
				return (shadowRenderStage);
			}

			unsigned_int32 GetProcessStamp(void) const
			{
				return (processStamp);
			}

			void SetProcessStamp(unsigned_int32 stamp)
			{
				processStamp = stamp;
			}

			unsigned_int32 GetLightStamp(void) const
			{
				return (lightStamp);
			}

			void SetLightStamp(unsigned_int32 stamp)
			{
				lightStamp = stamp;
			}

			unsigned_int32 GetShadowStamp(void) const
			{
				return (shadowStamp);
			}

			void SetShadowStamp(unsigned_int32 stamp)
			{
				shadowStamp = stamp;
			}

			unsigned_int32 GetCollisionStamp(void) const
			{
				return (collisionStamp);
			}

			void SetCollisionStamp(unsigned_int32 stamp)
			{
				collisionStamp = stamp;
			}

			volatile int32 *GetQueryThreadFlags(void)
			{
				return (&queryThreadFlags);
			}

			const Vector3D& GetGeometryVelocity(void) const
			{
				return (geometryVelocity);
			}

			void SetGeometryVelocity(const Vector3D& velocity)
			{
				geometryVelocity = velocity;
			}

			int32 GetMaterialCount(void) const
			{
				return (materialCount);
			}

			MaterialObject *GetMaterialObject(unsigned_int32 index) const
			{
				return ((index == 0) ? materialObject : GetMaterialObjectTable()[index - 1]);
			}

			PaintSpace *GetConnectedPaintSpace(void) const
			{
				return (connectedPaintSpace);
			}

			const ArrayBundle *GetPositionArrayBundle(void) const
			{
				return (positionArrayBundle);
			}

			const ArrayBundle *GetPrimitiveArrayBundle(void) const
			{
				return (primitiveArrayBundle);
			}

			C4API void PackType(Packer& data) const override;
			C4API void Prepack(List<Object> *linkList) const override;
			C4API void Pack(Packer& data, unsigned_int32 packFlags) const override;
			C4API void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			C4API int32 GetCategorySettingCount(Type category) const override;
			C4API Setting *GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const override;
			C4API void SetCategorySetting(Type category, const Setting *setting) override;

			int32 GetInternalConnectorCount(void) const override;
			const char *GetInternalConnectorKey(int32 index) const override;
			void ProcessInternalConnectors(void) override;
			bool ValidConnectedNode(const ConnectorKey& key, const Node *node) const override;
			C4API void SetConnectedPaintSpace(PaintSpace *paintSpace);

			C4API void Preload(void) override;
			C4API void Preprocess(void) override;
			C4API void Neutralize(void) override;

			C4API void SetMaterialCount(int32 count);
			C4API void SetMaterialObject(unsigned_int32 index, MaterialObject *object);
			C4API void OptimizeMaterials(void);

			C4API const MaterialObject *GetTriangleMaterial(int32 triangleIndex) const;

			void SetDetailLevel(int32 level);
	};


	//# \class	GenericGeometry		Represents a generic geometry node in a world.
	//
	//# The $GenericGeometry$ class represents a generic geometry node in a world.
	//
	//# \def	class GenericGeometry final : public Geometry
	//
	//# \ctor	GenericGeometry(const Geometry *geometry);
	//
	//# \param	geometry	A pointer to another geometry node that is copied into the generic geometry.
	//
	//# \desc
	//# The $GenericGeometry$ class represents a generic geometry node in the world.
	//
	//# \base	Geometry		A generic geometry node is a specific type of geometry.
	//
	//# \also	$@GenericGeometryObject@$


	class GenericGeometry final : public Geometry
	{
		friend class Geometry;

		private:

			typedef void PostprocessProc(GenericGeometry *);

			PostprocessProc		*postprocessProc;

			Point3D				worldCenter;
			Vector3D			worldAxis[3];

			GenericGeometry(const GenericGeometry& genericGeometry);

			Node *Replicate(void) const override;

			void HandlePostprocessUpdate(void) override;

			bool CalculateBoundingBox(Box3D *box) const override;
			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

			static void CalculateOrientedBoundingBox(GenericGeometry *genericGeometry);

			static bool BoxVisible(const Node *node, const VisibilityRegion *region);
			static bool BoxVisible(const Node *node, const VisibilityRegion *region, const List<OcclusionRegion> *occlusionList);

		public:

			C4API GenericGeometry();
			C4API GenericGeometry(const Geometry *geometry);
			C4API GenericGeometry(int32 levelCount, const List<GeometrySurface> *const *surfaceList, MaterialObject *const *materialArray, const SkinData *const *skinDataTable = nullptr, const MorphData *const *morphDataTable = nullptr);
			C4API GenericGeometry(int32 geometryCount, const Geometry *const *geometryArray, const Transformable *transformable);
			C4API GenericGeometry(BooleanOperation operation, const Geometry *geometry1, const Geometry *geometry2);
			C4API ~GenericGeometry();

			GenericGeometryObject *GetObject(void) const
			{
				return (static_cast<GenericGeometryObject *>(Node::GetObject()));
			}

			void SetPostprocessProc(PostprocessProc *proc)
			{
				postprocessProc = proc;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Preprocess(void) override;
	};
}


#endif

// ZYUQURM
