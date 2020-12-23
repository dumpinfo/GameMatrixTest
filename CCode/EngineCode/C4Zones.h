#ifndef C4Zones_h
#define C4Zones_h


//# \component	World Manager
//# \prefix		WorldMgr/


#include "C4Portals.h"
#include "C4Regions.h"
#include "C4Lights.h"
#include "C4Sources.h"
#include "C4Effects.h"
#include "C4Instances.h"
#include "C4Models.h"
#include "C4Markers.h"
#include "C4Physics.h"


namespace C4
{
	typedef Type	ZoneType;


	enum : ObjectType
	{
		kObjectZone				= 'ZONE'
	};


	//# \enum	ZoneType

	enum : ZoneType
	{
		kZoneInfinite			= 'INFT',		//## Infinite zone that covers all space.
		kZoneBox				= 'BOX ',		//## Zone having the shape of a box.
		kZoneCylinder			= 'CYLD',		//## Zone having the shape of a cylinder.
		kZonePolygon			= 'POLY'		//## Zone having the shape of an extruded convex polygon.
	};


	//# \enum	ZoneFlags

	enum
	{
		kZoneRenderSkybox		= 1 << 0		//## The skybox is visible from this zone.
	};


	enum
	{
		kMaxZoneVertexCount		= 8
	};


	enum
	{
		kZoneTraversalLocal		= 1 << 0,
		kZoneTraversalGlobal	= 1 << 1
	};


	enum
	{
		kCellGraphGeometry,
		kCellGraphEffect,
		kCellGraphTrigger,
		kCellGraphField,
		kCellGraphCount
	};


	C4API extern const char kConnectorKeyFog[];
	C4API extern const char kConnectorKeyAcoustics[];
	C4API extern const char kConnectorKeyRadiosity[];


	class Texture;


	//# \class	ZoneObject	Encapsulates data for a zone.
	//
	//# The $ZoneObject$ class encapsulates data for a zone.
	//
	//# \def	class ZoneObject : public Object
	//
	//# \ctor	ZoneObject(ZoneType type);
	//
	//# The constructor has protected access. The $ZoneObject$ class can only exist as the base class for a more specific type of zone.
	//
	//# \param	type	The zone type.
	//
	//# \desc
	//# The $ZoneObject$ class is the base class for all types of zone objects, and it stores
	//# data that is common to all of them. Each specific type of zone has an associated object
	//# that is a subclass of the $ZoneObject$ class.
	//#
	//# A zone object can have one of the following types.
	//
	//# \table	ZoneType
	//
	//# \base	Object		A $ZoneObject$ is an object that can be shared by multiple zone nodes.
	//
	//# \also	$@Zone@$
	//# \also	$@PortalObject@$
	//
	//# \wiki	Zones


	//# \function	ZoneObject::GetZoneType		Returns the type of a zone.
	//
	//# \proto	ZoneType GetZoneType(void) const;
	//
	//# \desc
	//# The $GetZoneType$ function returns the type of a zone, which can be one of the following constants.
	//
	//# \table	ZoneType


	//# \function	ZoneObject::GetAmbientLight		Returns the color of the ambient light for a zone.
	//
	//# \proto	const ColorRGBA& GetAmbientLight(void) const;
	//
	//# \desc
	//# The $GetAmbientLight$ function returns the color of the ambient light used in a zone.
	//# The alpha component of the color is not used.
	//#
	//# The initial ambient light color is (1.0, 1.0, 1.0, 1.0).
	//
	//# \also	$@ZoneObject::SetAmbientLight@$


	//# \function	ZoneObject::SetAmbientLight		Sets the color of the ambient light for a zone.
	//
	//# \proto	void SetAmbientLight(const ColorRGBA& ambient);
	//
	//# \param	ambient		The new ambient light color.
	//
	//# \desc
	//# The $SetAmbientLight$ function sets the color of the ambient light used in a zone to that
	//# specified by the $ambient$ paramater. The alpha component of the color is not used and should be set to 1.0.
	//#
	//# The initial ambient light color is (1.0, 1.0, 1.0, 1.0).
	//
	//# \also	$@ZoneObject::GetAmbientLight@$


	class ZoneObject : public Object
	{
		friend class WorldMgr;

		private:

			ZoneType				zoneType;
			unsigned_int32			zoneFlags;

			ColorRGBA				ambientLight;

			Texture					*environmentMap;
			ResourceName			environmentName;

			static unsigned_int32	(*const simplexMinFunc[4])(const Point3D *, Point3D *);

			static ZoneObject *Create(Unpacker& data, unsigned_int32 unpackFlags);

			static unsigned_int32 CalculateSimplexMinimum(int32 count, const Point3D *simplex, Point3D *p)
			{
				return ((*simplexMinFunc[count])(simplex, p));
			}

			static unsigned_int32 CalculateZeroSimplexMinimum(const Point3D *simplex, Point3D *p);
			static unsigned_int32 CalculateOneSimplexMinimum(const Point3D *simplex, Point3D *p);
			static unsigned_int32 CalculateTwoSimplexMinimum(const Point3D *simplex, Point3D *p);
			static unsigned_int32 CalculateThreeSimplexMinimum(const Point3D *simplex, Point3D *p);

		protected:

			ZoneObject(ZoneType type);
			~ZoneObject();

		public:

			ZoneType GetZoneType(void) const
			{
				return (zoneType);
			}

			unsigned_int32 GetZoneFlags(void) const
			{
				return (zoneFlags);
			}

			void SetZoneFlags(unsigned_int32 flags)
			{
				zoneFlags = flags;
			}

			const ColorRGBA& GetAmbientLight(void) const
			{
				return (ambientLight);
			}

			void SetAmbientLight(const ColorRGBA& ambient)
			{
				ambientLight = ambient;
			}

			Texture *const& GetEnvironmentMap(void) const
			{
				return (environmentMap);
			}

			const ResourceName& GetEnvironmentName(void) const
			{
				return (environmentName);
			}

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

			C4API void SetEnvironmentMap(const char *name);

			virtual bool ExteriorSphere(const Point3D& center, float radius) const = 0;
			virtual bool InteriorSphere(const Point3D& center, float radius) const = 0;
			virtual bool ExteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const = 0;
			virtual bool InteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const = 0;

			virtual bool InteriorPoint(const Point3D& position) const = 0;
			virtual Point3D GetInitialSupportPoint(void) const;
			virtual bool CalculateSupportPoint(const Vector3D& direction, Point3D *support) const = 0;
			virtual int32 ClipInteriorEdges(int32 edgeCount, const Point3D (*edge)[2], Point3D (*restrict clippedEdge)[2]) const = 0;

			bool IntersectsBoundingBox(const BoundingBox *box) const;
	};


	//# \class	InfiniteZoneObject	Encapsulates data for an infinite zone.
	//
	//# The $InfiniteZoneObject$ class encapsulates data for data for an infinite zone.
	//
	//# \def	class InfiniteZoneObject final : public ZoneObject
	//
	//# \ctor	InfiniteZoneObject();
	//
	//# \desc
	//
	//# \base	ZoneObject	An $InfiniteZoneObject$ is a specific type of zone object.
	//
	//# \also	$@InfiniteZone@$


	class InfiniteZoneObject final : public ZoneObject
	{
		private:

			Box3D		zoneBox;

			~InfiniteZoneObject();

		public:

			C4API InfiniteZoneObject();
			C4API InfiniteZoneObject(const Box3D& box);

			const Box3D& GetZoneBox(void) const
			{
				return (zoneBox);
			}

			void SetZoneBox(const Box3D& box)
			{
				zoneBox = box;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetObjectSize(float *size) const override;
			void SetObjectSize(const float *size) override;

			bool ExteriorSphere(const Point3D& center, float radius) const override;
			bool InteriorSphere(const Point3D& center, float radius) const override;
			bool ExteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const override;
			bool InteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const override;

			bool InteriorPoint(const Point3D& position) const override;
			bool CalculateSupportPoint(const Vector3D& direction, Point3D *support) const override;
			int32 ClipInteriorEdges(int32 edgeCount, const Point3D (*edge)[2], Point3D (*restrict clippedEdge)[2]) const override;
	};


	//# \class	BoxZoneObject	Encapsulates data for a box zone.
	//
	//# The $BoxZoneObject$ class encapsulates data for data for a box zone.
	//
	//# \def	class BoxZoneObject final : public ZoneObject
	//
	//# \ctor	BoxZoneObject(const Vector3D& size);
	//
	//# \param	size	The size of the box.
	//
	//# \desc
	//
	//# \base	ZoneObject	A $BoxZoneObject$ is a specific type of zone object.
	//
	//# \also	$@BoxZone@$


	class BoxZoneObject final : public ZoneObject
	{
		friend class ZoneObject;

		private:

			Vector3D	boxSize;

			BoxZoneObject();
			~BoxZoneObject();

		public:

			BoxZoneObject(const Vector3D& size);

			const Vector3D& GetBoxSize(void) const
			{
				return (boxSize);
			}

			void SetBoxSize(const Vector3D& size)
			{
				boxSize = size;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetObjectSize(float *size) const override;
			void SetObjectSize(const float *size) override;

			bool ExteriorSphere(const Point3D& center, float radius) const override;
			bool InteriorSphere(const Point3D& center, float radius) const override;
			bool ExteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const override;
			bool InteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const override;

			bool InteriorPoint(const Point3D& position) const override;
			bool CalculateSupportPoint(const Vector3D& direction, Point3D *support) const override;
			int32 ClipInteriorEdges(int32 edgeCount, const Point3D (*edge)[2], Point3D (*restrict clippedEdge)[2]) const override;
	};


	//# \class	CylinderZoneObject	Encapsulates data for a cylinder zone.
	//
	//# The $CylinderZoneObject$ class encapsulates data for data for a cylinder zone.
	//
	//# \def	class CylinderZoneObject final : public ZoneObject
	//
	//# \ctor	CylinderZoneObject(const Vector2D& size, float height);
	//
	//# \param	size	The size of the cylinder's base.
	//# \param	height	The height of the cylinder.
	//
	//# \desc
	//
	//# \base	ZoneObject	A $CylinderZoneObject$ is a specific type of zone object.
	//
	//# \also	$@CylinderZone@$


	class CylinderZoneObject final : public ZoneObject
	{
		friend class ZoneObject;

		private:

			Vector2D	cylinderSize;
			float		cylinderHeight;
			float		ratioXY;

			CylinderZoneObject();
			~CylinderZoneObject();

		public:

			CylinderZoneObject(const Vector2D& size, float height);

			const Vector2D& GetCylinderSize(void) const
			{
				return (cylinderSize);
			}

			void SetCylinderSize(const Vector2D& size)
			{
				cylinderSize = size;
				ratioXY = size.x / size.y;
			}

			float GetCylinderHeight(void) const
			{
				return (cylinderHeight);
			}

			void SetCylinderHeight(float height)
			{
				cylinderHeight = height;
			}

			float GetRatioXY(void) const
			{
				return (ratioXY);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetObjectSize(float *size) const override;
			void SetObjectSize(const float *size) override;

			bool ExteriorSphere(const Point3D& center, float radius) const override;
			bool InteriorSphere(const Point3D& center, float radius) const override;
			bool ExteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const override;
			bool InteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const override;

			bool InteriorPoint(const Point3D& position) const override;
			bool CalculateSupportPoint(const Vector3D& direction, Point3D *support) const override;
			int32 ClipInteriorEdges(int32 edgeCount, const Point3D (*edge)[2], Point3D (*restrict clippedEdge)[2]) const override;
	};


	//# \class	PolygonZoneObject	Encapsulates data for a polygon zone.
	//
	//# The $PolygonZoneObject$ class encapsulates data for data for a polygon zone.
	//
	//# \def	class PolygonZoneObject final : public ZoneObject
	//
	//# \ctor	PolygonZoneObject(const Vector2D& size, float height);
	//
	//# \param	size	The dimensions of a rectangular base for the zone.
	//# \param	height	The height of the zone.
	//
	//# \desc
	//
	//# \base	ZoneObject	A $PolygonZoneObject$ is a specific type of zone object.
	//
	//# \also	$@PolygonZone@$


	class PolygonZoneObject final : public ZoneObject
	{
		friend class ZoneObject;

		private:

			float		polygonHeight;

			int32		vertexCount;
			Point3D		polygonVertex[kMaxZoneVertexCount];

			PolygonZoneObject();
			~PolygonZoneObject();

		public:

			PolygonZoneObject(const Vector2D& size, float height);

			float GetPolygonHeight(void) const
			{
				return (polygonHeight);
			}

			void SetPolygonHeight(float height)
			{
				polygonHeight = height;
			}

			int32 GetVertexCount(void) const
			{
				return (vertexCount);
			}

			void SetVertexCount(int32 count)
			{
				vertexCount = count;
			}

			Point3D *GetVertexArray(void)
			{
				return (polygonVertex);
			}

			const Point3D *GetVertexArray(void) const
			{
				return (polygonVertex);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			C4API void SetPolygonSize(const Vector2D& size, float height);

			int32 GetObjectSize(float *size) const override;
			void SetObjectSize(const float *size) override;

			bool ExteriorSphere(const Point3D& center, float radius) const override;
			bool InteriorSphere(const Point3D& center, float radius) const override;
			bool ExteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const override;
			bool InteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const override;

			bool InteriorPoint(const Point3D& position) const override;
			Point3D GetInitialSupportPoint(void) const override;
			bool CalculateSupportPoint(const Vector3D& direction, Point3D *support) const override;
			int32 ClipInteriorEdges(int32 edgeCount, const Point3D (*edge)[2], Point3D (*restrict clippedEdge)[2]) const override;
	};


	//# \class	Zone	Represents a zone node in a world.
	//
	//# The $Zone$ class represents a zone node in a world.
	//
	//# \def	class Zone : public Node, public ListElement<Zone>
	//
	//# \ctor	Zone(ZoneType type);
	//
	//# The constructor has protected access. The $Zone$ class can only exist as the base class for a more specific type of zone.
	//
	//# \param	type	The zone type. See below for a list of possible types.
	//
	//# \desc
	//# The $Zone$ class is the base class for all zone nodes. The $type$ parameter can be one of the following
	//# constants, and it is specified by the subclass's constructor.
	//
	//# \table	ZoneType
	//
	//# \base	Node							A $Zone$ node is a scene graph node.
	//# \base	Utilities/ListElement<Zone>		Each zone has a list of its subzones.
	//
	//# \also	$@ZoneObject@$
	//# \also	$@Portal@$
	//
	//# \wiki	Zones


	//# \function	Zone::GetZoneType		Returns the type of a zone.
	//
	//# \proto	ZoneType GetZoneType(void) const;
	//
	//# \desc
	//# The $GetZoneType$ function returns the type of a zone, which can be one of the following constants.
	//
	//# \table	ZoneType


	class Zone : public Node, public ListElement<Zone>
	{
		friend class Node;

		private:

			ZoneType					zoneType;

			unsigned_int32				traversalExclusionMask;

			FogSpace					*connectedFogSpace;
			ShadowSpace					*connectedShadowSpace;
			AcousticsSpace				*connectedAcousticsSpace;
			RadiositySpace				*connectedRadiositySpace;

			AmbientEnvironment			ambientEnvironment;

			CellGraphSite				cellGraph[kCellGraphCount];

			List<Zone>					subzoneList;
			List<Portal>				portalList;
			List<Portal>				occlusionPortalList;
			List<OcclusionSpace>		occlusionSpaceList;
			List<FogSpace>				fogSpaceList;
			List<Marker>				markerList;

			Link<Node>					physicsNodeLink;

			List<CameraRegion>			cameraRegionList;
			List<LightRegion>			lightRegionList;
			List<SourceRegion>			sourceRegionList;

			static Zone *Create(Unpacker& data, unsigned_int32 unpackFlags);

			void AddSubzone(Zone *zone)
			{
				subzoneList.Append(zone);
			}

			static void PhysicsNodeLinkProc(Node *node, void *cookie);

			Node *GetHierarchicalConnectedNode(const char *key) const;

		protected:

			Zone(ZoneType type);
			Zone(const Zone& zone);

		public:

			virtual ~Zone();

			using ListElement<Zone>::Previous;
			using ListElement<Zone>::Next;

			ZoneType GetZoneType(void) const
			{
				return (zoneType);
			}

			ZoneObject *GetObject(void) const
			{
				return (static_cast<ZoneObject *>(Node::GetObject()));
			}

			unsigned_int32 GetTraversalExclusionMask(void) const
			{
				return (traversalExclusionMask);
			}

			void SetTraversalExclusionMask(unsigned_int32 mask)
			{
				traversalExclusionMask = mask;
			}

			FogSpace *GetConnectedFogSpace(void) const
			{
				return (connectedFogSpace);
			}

			ShadowSpace *GetConnectedShadowSpace(void) const
			{
				return (connectedShadowSpace);
			}

			AcousticsSpace *GetConnectedAcousticsSpace(void) const
			{
				return (connectedAcousticsSpace);
			}

			RadiositySpace *GetConnectedRadiositySpace(void) const
			{
				return (connectedRadiositySpace);
			}

			const AmbientEnvironment *GetAmbientEnvironment(void) const
			{
				return (&ambientEnvironment);
			}

			Site *GetCellGraphSite(int32 index)
			{
				return (&cellGraph[index]);
			}

			const Site *GetCellGraphSite(int32 index) const
			{
				return (&cellGraph[index]);
			}

			Zone *GetFirstSubzone(void) const
			{
				return (subzoneList.First());
			}

			Portal *GetFirstPortal(void) const
			{
				return (portalList.First());
			}

			void AddPortal(Portal *portal)
			{
				portalList.Append(portal);
			}

			Portal *GetFirstOcclusionPortal(void) const
			{
				return (occlusionPortalList.First());
			}

			void AddOcclusionPortal(Portal *portal)
			{
				occlusionPortalList.Append(portal);
			}

			OcclusionSpace *GetFirstOcclusionSpace(void) const
			{
				return (occlusionSpaceList.First());
			}

			void AddOcclusionSpace(OcclusionSpace *space)
			{
				occlusionSpaceList.Append(space);
			}

			FogSpace *GetFirstFogSpace(void) const
			{
				return (fogSpaceList.First());
			}

			void AddFogSpace(FogSpace *fogSpace)
			{
				fogSpaceList.Append(fogSpace);
			}

			Marker *GetFirstMarker(void) const
			{
				return (markerList.First());
			}

			void AddMarker(Marker *marker)
			{
				markerList.Append(marker);
			}

			PhysicsNode *GetPhysicsNode(void) const
			{
				return (static_cast<PhysicsNode *>(physicsNodeLink.GetTarget()));
			}

			void SetPhysicsNode(PhysicsNode *physicsNode)
			{
				physicsNodeLink = physicsNode;
			}

			CameraRegion *GetFirstCameraRegion(void) const
			{
				return (cameraRegionList.First());
			}

			void AddCameraRegion(CameraRegion *cameraRegion)
			{
				cameraRegionList.Append(cameraRegion);
			}

			LightRegion *GetFirstLightRegion(void) const
			{
				return (lightRegionList.First());
			}

			void AddLightRegion(LightRegion *lightRegion)
			{
				lightRegionList.Append(lightRegion);
			}

			SourceRegion *GetFirstSourceRegion(void) const
			{
				return (sourceRegionList.First());
			}

			void AddSourceRegion(SourceRegion *sourceRegion)
			{
				sourceRegionList.Append(sourceRegion);
			}

			void Detach(void) override;

			void PackType(Packer& data) const override;
			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetInternalConnectorCount(void) const override;
			const char *GetInternalConnectorKey(int32 index) const override;
			void ProcessInternalConnectors(void) override;
			bool ValidConnectedNode(const ConnectorKey& key, const Node *node) const override;
			C4API void SetConnectedFogSpace(FogSpace *fogSpace);
			C4API void SetConnectedShadowSpace(ShadowSpace *shadowSpace);
			C4API void SetConnectedAcousticsSpace(AcousticsSpace *acousticsSpace);
			C4API void SetConnectedRadiositySpace(RadiositySpace *radiositySpace);

			void Preprocess(void) override;

			C4API void InvalidateLightRegions(void) const;
			C4API void InvalidateSourceRegions(void) const;

			bool InsertZoneTreeSite(int32 index, Node *node, int32 maxDepth, int32 forcedDepth);
			void InsertInfiniteSite(int32 index, Node *node, int32 depth);
	};


	//# \class	InfiniteZone	Represents an infinite zone node in a world.
	//
	//# The $InfiniteZone$ class represents an infinite zone node in a world.
	//
	//# \def	class InfiniteZone final : public Zone
	//
	//# \ctor	InfiniteZone();
	//
	//# \desc
	//#
	//
	//# \base	Zone		An $InfiniteZone$ node is a specific type of zone.
	//
	//# \also	$@InfiniteZoneObject@$


	class InfiniteZone final : public Zone
	{
		private:

			Object			*auxiliaryObject;

			InfiniteZone(const InfiniteZone& infiniteZone);

			Node *Replicate(void) const override;

			static void AuxiliaryObjectLinkProc(Object *object, void *cookie);

			bool CalculateBoundingBox(Box3D *box) const override;
			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		public:

			C4API InfiniteZone();
			C4API ~InfiniteZone();

			InfiniteZoneObject *GetObject(void) const
			{
				return (static_cast<InfiniteZoneObject *>(Node::GetObject()));
			}

			Object *GetAuxiliaryObject(void) const
			{
				return (auxiliaryObject);
			}

			void Prepack(List<Object> *linkList) const override;
			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			C4API void SetAuxiliaryObject(Object *object);

			void Preprocess(void) override;
	};


	//# \class	BoxZone		Represents a box zone node in a world.
	//
	//# The $BoxZone$ class represents a box zone node in a world.
	//
	//# \def	class BoxZone final : public Zone
	//
	//# \ctor	BoxZone(const Vector3D& size);
	//
	//# \param	size	The size of the box.
	//
	//# \desc
	//#
	//
	//# \base	Zone		A $BoxZone$ node is a specific type of zone.
	//
	//# \also	$@BoxZoneObject@$


	class BoxZone final : public Zone
	{
		friend class Zone;

		private:

			BoxZone();
			BoxZone(const BoxZone& boxZone);

			Node *Replicate(void) const override;

			bool CalculateBoundingBox(Box3D *box) const override;
			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		public:

			C4API BoxZone(const Vector3D& size);
			C4API ~BoxZone();

			BoxZoneObject *GetObject(void) const
			{
				return (static_cast<BoxZoneObject *>(Node::GetObject()));
			}

			void Preprocess(void) override;
	};


	//# \class	CylinderZone		Represents a cylinder zone node in a world.
	//
	//# The $CylinderZone$ class represents a cylinder zone node in a world.
	//
	//# \def	class CylinderZone final : public Zone
	//
	//# \ctor	CylinderZone(const Vector2D& size, float height);
	//
	//# \param	size	The size of the cylinder's base.
	//# \param	height	The height of the cylinder.
	//
	//# \desc
	//#
	//
	//# \base	Zone		A $CylinderZone$ node is a specific type of zone.
	//
	//# \also	$@CylinderZoneObject@$


	class CylinderZone final : public Zone
	{
		friend class Zone;

		private:

			CylinderZone();
			CylinderZone(const CylinderZone& cylinderZone);

			Node *Replicate(void) const override;

			bool CalculateBoundingBox(Box3D *box) const override;
			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		public:

			C4API CylinderZone(const Vector2D& size, float height);
			C4API ~CylinderZone();

			CylinderZoneObject *GetObject(void) const
			{
				return (static_cast<CylinderZoneObject *>(Node::GetObject()));
			}

			void Preprocess(void) override;
	};


	//# \class	PolygonZone		Represents a polygon zone node in a world.
	//
	//# The $PolygonZone$ class represents a polygon zone node in a world.
	//
	//# \def	class PolygonZone final : public Zone
	//
	//# \ctor	PolygonZone(const Vector2D& size, float height);
	//
	//# \param	size	The dimensions of a rectangular base for the zone.
	//# \param	height	The height of the zone.
	//
	//# \desc
	//#
	//
	//# \base	Zone		A $PolygonZone$ node is a specific type of zone.
	//
	//# \also	$@PolygonZoneObject@$


	class PolygonZone final : public Zone
	{
		friend class Zone;

		private:

			PolygonZone();
			PolygonZone(const PolygonZone& polygonZone);

			Node *Replicate(void) const override;

			bool CalculateBoundingBox(Box3D *box) const override;
			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		public:

			C4API PolygonZone(const Vector2D& size, float height);
			C4API ~PolygonZone();

			PolygonZoneObject *GetObject(void) const
			{
				return (static_cast<PolygonZoneObject *>(Node::GetObject()));
			}

			void Preprocess(void) override;
	};
}


#endif

// ZYUQURM
