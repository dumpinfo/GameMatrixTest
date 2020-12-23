 

#ifndef C4Portals_h
#define C4Portals_h


//# \component	World Manager
//# \prefix		WorldMgr/


#include "C4Node.h"
#include "C4Polyhedron.h"


namespace C4
{
	typedef Type	PortalType;
	typedef Type	PortalBuffer;


	const float kMinPortalClipDistance = 0.2F;


	enum : ObjectType
	{
		kObjectPortal				= 'PORT'
	};


	//# \enum	PortalType

	enum : PortalType
	{
		kPortalDirect				= 'DRCT',		//## Ordinary portal that leads from one zone to another through a polygonal boundary.
		kPortalRemote				= 'REMO',		//## Portal having a remote transform. This is used for mirrors and portals through which remote areas of the world can be seen. It is also used for rendering into the reflection and refraction buffers.
		kPortalCamera				= 'CAMR',		//## Special portal used internally for camera widgets in panel effects.
		kPortalOcclusion			= 'OCCL'		//## Occluder, or antiportal, that blocks objects behind it from the perspective of the camera.
	};


	//# \enum	PortalFlags

	enum
	{
		kPortalLightInhibit			= 1 << 0,		//## No lights shine through the portal.
		kPortalStaticLightInhibit	= 1 << 1,		//## Only dynamic lights shine through the portal.
		kPortalShadowMapInhibit		= 1 << 2,		//## For a direct portal, the portal is not followed when rendering a shadow map. For a remote portal, shadows are not rendered in the image generated by the portal.
		kPortalFogInhibit			= 1 << 3,		//## Do not apply fog through the portal (direct portal only).
		kPortalSkyboxInhibit		= 1 << 4,		//## Do not render the skybox through the portal (remote portal only).
		kPortalOverrideClearColor	= 1 << 5,		//## Override the camera clear color when rendering through the portal (remote portal only).
		kPortalSeparateShadowMap	= 1 << 6,		//## An infinite shadow map rendered through the portal is not reused (remote portal only). This only affects portals whose remote transform is the identity.
		kPortalAllowRemoteLight		= 1 << 7,		//## Allow lights to shine through the portal (remote portal only).
		kPortalObliqueFrustum		= 1 << 8,		//## Use an oblique view frustum when rendering through the portal (remote portal only).
		kPortalRecursive			= 1 << 9,		//## Allow the portal to be rendered recursively (remote portal only).
		kPortalDistant				= 1 << 10,		//## Render only infinitely distant objects, like the skybox, through the portal (remote portal only).
		kPortalSourceInhibit		= 1 << 11,		//## No source audio flows through the portal (direct portal only).
		kPortalForceCameraZone		= 1 << 12,		//## The remote camera regions are rooted only in the target zone (remote portal only).
		kPortalShadowMapMaxDepth	= 1 << 13,		//## Disables an optimization that uses the maximum depth of all rendered geometries to cull infinite shadow cascades (remote portal only). This applies to remote portals that do not have the $kPortalSeparateShadowMap$ flag specified.
		kPortalFullShadowRegion		= 1 << 14		//## Causes the full-size shadow region to be used on the side of the portal containing a light (direct portal only).
	};


	//# \enum	PortalBuffer

	enum : PortalBuffer
	{
		kPortalBufferPrimary		= 0,
		kPortalBufferReflection		= 'RFLC',
		kPortalBufferRefraction		= 'RFRC'
	};


	enum
	{
		kMaxPortalVertexCount			= 8,
		kMaxClippedPortalVertexCount	= kMaxPolyhedronFaceCount - 2
	};


	C4API extern const char kConnectorKeyZone[];


	class FrustumCamera;
	class OrthoCamera;


	//# \class	PortalObject	Encapsulates data for a portal.
	//
	//# The $PortalObject$ class encapsulates data for a portal.
	//
	//# \def	class PortalObject : public Object
	//
	//# \ctor	PortalObject(PortalType type);
	//# \ctor	PortalObject(PortalType type, const Vector2D& size);
	//
	//# The constructors have protected access. The $PortalObject$ class can only exist as the base class for a more specific type of portal.
	//
	//# \param	type	The portal type. See below for a list of possible types.
	//# \param	size	The width and height of a rectangular portal.
	//
	//# \desc
	//# The $PortalObject$ class is the base class for all types of portal objects, and it stores
	//# data that is common to all of them. Each specific type of portal has an associated object 
	//# that is a subclass of the $PortalObject$ class.
	//#
	//# A portal object can have one of the following types. 
	//
	//# \table	PortalType 
	//
	//# \base	Object		A $PortalObject$ is an object that can be shared by multiple portal nodes.
	// 
	//# \also	$@Portal@$
	//# \also	$@ZoneObject@$ 
	// 
	//# \wiki	Portals


	//# \function	PortalObject::GetPortalType		Returns the portal type. 
	//
	//# \proto	PortalType GetPortalType(void) const;
	//
	//# \desc
	//# The $GetPortalType$ function returns the portal type, which can be one of the following constants.
	//
	//# \table	PortalType


	//# \function	PortalObject::GetPortalFlags		Returns the portal flags.
	//
	//# \proto	unsigned_int32 GetPortalFlags(void) const;
	//
	//# \desc
	//# The $GetPortalFlags$ function returns the portal flags, which can a combination (through logical OR) of the following constants.
	//
	//# \table	PortalFlags
	//
	//# \also	$@PortalObject::SetPortalFlags@$


	//# \function	PortalObject::SetPortalFlags		Sets the portal flags.
	//
	//# \proto	void SetPortalFlags(unsigned_int32 flags);
	//
	//# \param	flags	The new portal flags.
	//
	//# \desc
	//# The $SetPortalFlags$ function sets the portal flags to the value specified by the $flags$ parameter,
	//# which can a combination (through logical OR) of the following constants.
	//
	//# \table	PortalFlags
	//
	//# \also	$@PortalObject::GetPortalFlags@$


	//# \function	PortalObject::GetVertexCount		Returns the number of vertices defining the boundary of the portal.
	//
	//# \proto	int32 GetVertexCount(void) const;
	//
	//# \desc
	//# The $GetVertexCount$ function returns the number of vertices defining the boundary of the portal.
	//# The $@PortalObject::GetVertexArray@$ function can be used to retrieve the array of vertex positions.
	//
	//# \also	$@PortalObject::SetVertexCount@$
	//# \also	$@PortalObject::GetVertexArray@$


	//# \function	PortalObject::SetVertexCount		Sets the number of vertices defining the boundary of the portal.
	//
	//# \proto	void SetVertexCount(int32 count);
	//
	//# \param	count	The new vertex count. This should be at least 3 and no greater than $kMaxPortalVertexCount$.
	//
	//# \desc
	//# The $SetVertexCount$ function sets the number of vertices defining the boundary of the portal.
	//# The maximum number of vertices that a portal may have is given by the $kMaxPortalVertexCount$ constant.
	//# The vertex positions can be set by calling the $@PortalObject::GetVertexArray@$ function.
	//
	//# \also	$@PortalObject::GetVertexCount@$
	//# \also	$@PortalObject::GetVertexArray@$


	//# \function	PortalObject::GetVertexArray		Returns the object-space vertex array defining the boundary of the portal.
	//
	//# \proto	Point3D *GetVertexArray(void);
	//# \proto	const Point3D *GetVertexArray(void) const;
	//
	//# \desc
	//# The $GetVertexArray$ function returns the object-space vertex array defining the boundary of the portal.
	//# The number of vertices in the array is returned by the $@PortalObject::GetVertexCount@$ function.
	//#
	//# To specify the vertex array for a portal, the $@PortalObject::SetVertexCount@$ function should be called
	//# to set the number of vertices, and then the positions should be stored in the array returned by the
	//# $GetVertexArray$ function. All of the portal's vertices should lie in the <i>x</i>-<i>y</i> plane; that is,
	//# each <i>z</i> coordinate should be 0.0.
	//
	//# \also	$@PortalObject::GetVertexCount@$
	//# \also	$@PortalObject::SetVertexCount@$


	//# \function	PortalObject::SetPortalSize		Configures a portal to be a rectangle of given size.
	//
	//# \proto	void SetPortalSize(const Vector2D& size);
	//
	//# \param	size	The portal size.
	//
	//# \desc
	//# The $SetPortalSize$ function sets the portal's vertex count to 4 and sets the vertex positions to
	//# be a rectangle having one corner at the origin, width given by the <i>x</i> coordinate of the $size$
	//# parameter, and height given by the <i>y</i> coordinate of the $size$ parameter.
	//
	//# \also	$@PortalObject::GetVertexCount@$
	//# \also	$@PortalObject::GetVertexArray@$


	class PortalObject : public Object
	{
		friend class WorldMgr;

		private:

			PortalType			portalType;
			unsigned_int32		portalFlags;

			int32				vertexCount;
			Point3D				portalVertex[kMaxPortalVertexCount];

			static PortalObject *Create(Unpacker& data, unsigned_int32 unpackFlags);

		protected:

			PortalObject(PortalType type);
			PortalObject(PortalType type, int32 count);
			PortalObject(PortalType type, const Vector2D& size);
			~PortalObject();

		public:

			PortalType GetPortalType(void) const
			{
				return (portalType);
			}

			unsigned_int32 GetPortalFlags(void) const
			{
				return (portalFlags);
			}

			void SetPortalFlags(unsigned_int32 flags)
			{
				portalFlags = flags;
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
				return (portalVertex);
			}

			const Point3D *GetVertexArray(void) const
			{
				return (portalVertex);
			}

			void PackType(Packer& data) const override;
			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			C4API void SetPortalSize(const Vector2D& size);
	};


	//# \class	DirectPortalObject		Encapsulates data for a direct portal.
	//
	//# The $DirectPortalObject$ class encapsulates data for a direct portal.
	//
	//# \def	class DirectPortalObject final : public PortalObject
	//
	//# \ctor	DirectPortalObject();
	//# \ctor	DirectPortalObject(const Vector2D& size);
	//
	//# \param	size	The width and height of a rectangular portal.
	//
	//# \desc
	//
	//# \base	PortalObject	A $DirectPortalObject$ is a specific type of portal object.
	//
	//# \also	$@DirectPortal@$


	class DirectPortalObject final : public PortalObject
	{
		friend class PortalObject;

		private:

			DirectPortalObject();
			~DirectPortalObject();

		public:

			DirectPortalObject(int32 count);
			DirectPortalObject(const Vector2D& size);

			int32 GetCategoryCount(void) const override;
			Type GetCategoryType(int32 index, const char **title) const override;
			int32 GetCategorySettingCount(Type category) const override;
			Setting *GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const override;
			void SetCategorySetting(Type category, const Setting *setting) override;
	};


	//# \class	RemotePortalObject		Encapsulates data for a remote portal.
	//
	//# The $RemotePortalObject$ class encapsulates data for a remote portal.
	//
	//# \def	class RemotePortalObject final : public PortalObject
	//
	//# \ctor	RemotePortalObject();
	//# \ctor	RemotePortalObject(const Vector2D& size);
	//
	//# \param	size	The width and height of a rectangular portal.
	//
	//# \desc
	//
	//# \base	PortalObject	A $RemotePortalObject$ is a specific type of portal object.
	//
	//# \also	$@RemotePortal@$


	class RemotePortalObject final : public PortalObject
	{
		friend class PortalObject;

		private:

			PortalBuffer		portalBuffer;
			ColorRGBA			portalClearColor;
			float				portalPlaneOffset;

			int32				minDetailLevel;
			float				detailLevelBias;
			float				farClipDepth;

			float				focalLengthMultiplier;

			ConnectorKey		localConnectorKey;
			ConnectorKey		remoteConnectorKey;

			RemotePortalObject();
			~RemotePortalObject();

		public:

			RemotePortalObject(const Vector2D& size);

			PortalBuffer GetPortalBuffer(void) const
			{
				return (portalBuffer);
			}

			void SetPortalBuffer(PortalBuffer buffer)
			{
				portalBuffer = buffer;
			}

			const ColorRGBA& GetPortalClearColor(void) const
			{
				return (portalClearColor);
			}

			void SetPortalClearColor(const ColorRGBA& color)
			{
				portalClearColor = color;
			}

			float GetPortalPlaneOffset(void) const
			{
				return (portalPlaneOffset);
			}

			void SetPortalPlaneOffset(float offset)
			{
				portalPlaneOffset = offset;
			}

			int32 GetMinDetailLevel(void) const
			{
				return (minDetailLevel);
			}

			void SetMinDetailLevel(int32 level)
			{
				minDetailLevel = level;
			}

			float GetDetailLevelBias(void) const
			{
				return (detailLevelBias);
			}

			void SetDetailLevelBias(float bias)
			{
				detailLevelBias = bias;
			}

			float GetFarClipDepth(void) const
			{
				return (farClipDepth);
			}

			void SetFarClipDepth(float depth)
			{
				farClipDepth = depth;
			}

			float GetFocalLengthMultiplier(void) const
			{
				return (focalLengthMultiplier);
			}

			void SetFocalLengthMultiplier(float multiplier)
			{
				focalLengthMultiplier = multiplier;
			}

			const ConnectorKey& GetLocalConnectorKey(void) const
			{
				return (localConnectorKey);
			}

			void SetLocalConnectorKey(const ConnectorKey& key)
			{
				localConnectorKey = key;
			}

			const ConnectorKey& GetRemoteConnectorKey(void) const
			{
				return (remoteConnectorKey);
			}

			void SetRemoteConnectorKey(const ConnectorKey& key)
			{
				remoteConnectorKey = key;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetCategoryCount(void) const override;
			Type GetCategoryType(int32 index, const char **title) const override;
			int32 GetCategorySettingCount(Type category) const override;
			Setting *GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const override;
			void SetCategorySetting(Type category, const Setting *setting) override;
	};


	//# \class	OcclusionPortalObject	Encapsulates data for an occlusion portal.
	//
	//# The $OcclusionPortalObject$ class encapsulates data for an occlusion portal.
	//
	//# \def	class OcclusionPortalObject final : public PortalObject
	//
	//# \ctor	OcclusionPortalObject();
	//# \ctor	OcclusionPortalObject(const Vector2D& size);
	//
	//# \param	size	The width and height of a rectangular portal.
	//
	//# \desc
	//
	//# \base	PortalObject	An $OcclusionPortalObject$ is a specific type of portal object.
	//
	//# \also	$@OcclusionPortal@$


	class OcclusionPortalObject final : public PortalObject
	{
		friend class PortalObject;

		private:

			OcclusionPortalObject();
			~OcclusionPortalObject();

		public:

			OcclusionPortalObject(const Vector2D& size);
	};


	//# \class	Portal		Represents a portal node in a world.
	//
	//# The $Portal$ class represents a portal node in a world.
	//
	//# \def	class Portal : public Node, public ListElement<Portal>
	//
	//# \ctor	Portal(PortalType type);
	//
	//# The constructor has protected access. The $Portal$ class can only exist as the base class for a more specific type of portal.
	//
	//# \param	type	The portal type. See below for a list of possible types.
	//
	//# \desc
	//# The $Portal$ class is the base class for all portal nodes. The $type$ parameter can be one of the following
	//# constants, and it is specified by the subclass's constructor.
	//
	//# \table	PortalType
	//
	//# \base	Node							A $Portal$ node is a scene graph node.
	//# \base	Utilities/ListElement<Portal>	Each zone has a list of portals that lead out of the zone.
	//
	//# \also	$@PortalObject@$
	//# \also	$@Zone@$
	//
	//# \wiki	Portals


	//# \function	Portal::GetPortalType		Returns the portal type.
	//
	//# \proto	PortalType GetPortalType(void) const;
	//
	//# \desc
	//# The $GetPortalType$ function returns the specific portal type, which can be one of the following constants.
	//
	//# \table	PortalType


	//# \function	Portal::GetWorldPlane		Returns the world-space plane containing the portal.
	//
	//# \proto	const Antivector4D& GetWorldPlane(void) const;
	//
	//# \desc
	//# The $GetWorldPlane$ function returns the world-space plane containing the portal. A portal is visible
	//# only from the positive side of this plane.
	//
	//# \also	$@Math/Antivector4D@$
	//# \also	$@Portal::GetWorldVertexArray@$


	//# \function	Portal::GetWorldVertexArray		Returns the world-space vertex array defining the boundary of the portal.
	//
	//# \proto	const Point3D *GetWorldVertexArray(void) const;
	//
	//# \desc
	//# The $GetWorldVertexArray$ function returns the world-space vertex array defining the boundary of the portal.
	//# The number of vertices can be retrieved by using the $@Node::GetObject@$ function to obtain the associated
	//# $@PortalObject@$ object and then calling the $@PortalObject::GetVertexCount@$ function. The object-space
	//# vertex positions can be retrieved by calling the $@PortalObject::GetVertexArray@$ function.
	//
	//# \also	$@Math/Point3D@$
	//# \also	$@Portal::GetWorldPlane@$
	//# \also	$@PortalObject::GetVertexCount@$
	//# \also	$@PortalObject::GetVertexArray@$


	//# \function	Portal::GetConnectedZone	Returns the zone to which the portal is connected.
	//
	//# \proto	Zone *GetConnectedZone(void) const;
	//
	//# \desc
	//# The $GetConnectedZone$ function returns the zone to which the portal is connected. This is the zone to which
	//# the portal leads, allowing cameras and lights to see from one zone to another.
	//
	//# \also	$@Zone@$
	//# \also	$@Portal::SetConnectedZone@$


	//# \function	Portal::SetConnectedZone	Sets the zone to which the portal is connected.
	//
	//# \proto	void SetConnectedZone(Zone *zone);
	//
	//# \param	zone	The zone to which the portal is to be connected.
	//
	//# \desc
	//# The $SetConnectedZone$ function sets the zone to which the portal is connected. This is the zone to which
	//# the portal leads, allowing cameras and lights to see from one zone to another.
	//
	//# \also	$@Zone@$
	//# \also	$@Portal::GetConnectedZone@$


	class Portal : public Node, public ListElement<Portal>
	{
		friend class Node;

		private:

			PortalType			portalType;

			Zone				*connectedZone;

			Antivector4D		worldPlane;
			Point3D				worldVertex[kMaxPortalVertexCount];

			Bivector4D			worldEdgeLine[kMaxPortalVertexCount];
			Vector3D			worldInwardDirection[kMaxPortalVertexCount];

			static Portal *Create(Unpacker& data, unsigned_int32 unpackFlags);

			void HandleTransformUpdate(void) override;

			bool CalculateBoundingBox(Box3D *box) const override;
			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		protected:

			Portal(PortalType type);
			Portal(const Portal& portal);

		public:

			virtual ~Portal();

			using ListElement<Portal>::Previous;
			using ListElement<Portal>::Next;

			PortalType GetPortalType(void) const
			{
				return (portalType);
			}

			PortalObject *GetObject(void) const
			{
				return (static_cast<PortalObject *>(Node::GetObject()));
			}

			Zone *GetConnectedZone(void) const
			{
				return (connectedZone);
			}

			const Antivector4D& GetWorldPlane(void) const
			{
				return (worldPlane);
			}

			const Point3D *GetWorldVertexArray(void) const
			{
				return (worldVertex);
			}

			const Bivector4D *GetWorldEdgeLineArray(void) const
			{
				return (worldEdgeLine);
			}

			void PackType(Packer& data) const override;
			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetCategorySettingCount(Type category) const override;
			Setting *GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const override;
			void SetCategorySetting(Type category, const Setting *setting) override;

			int32 GetInternalConnectorCount(void) const override;
			const char *GetInternalConnectorKey(int32 index) const override;
			void ProcessInternalConnectors(void) override;
			bool ValidConnectedNode(const ConnectorKey& key, const Node *node) const override;
			C4API void SetConnectedZone(Zone *zone);

			void Preprocess(void) override;
			void Neutralize(void) override;

			Point3D CalculateClosestBoundaryPoint(const Point3D& p) const;
			bool CalculateClosestBoundaryPoint(const Bivector4D& line, Point3D *result) const;
	};


	//# \class	DirectPortal		Represents a direct portal node in a world.
	//
	//# The $DirectPortal$ class represents a direct portal node in a world.
	//
	//# \def	class DirectPortal final : public Portal
	//
	//# \ctor	DirectPortal();
	//# \ctor	DirectPortal(const Vector2D& size);
	//
	//# \param	size	The size of a rectangular portal.
	//
	//# \desc
	//#
	//
	//# \base	Portal		A $DirectPortal$ node is a specific type of portal.
	//
	//# \also	$@DirectPortalObject@$
	//# \also	$@Zone@$


	class DirectPortal final : public Portal
	{
		private:

			DirectPortal(const DirectPortal& directPortal);

			Node *Replicate(void) const override;

		public:

			C4API DirectPortal();
			C4API DirectPortal(int32 count);
			C4API DirectPortal(const Vector2D& size);
			C4API ~DirectPortal();

			DirectPortalObject *GetObject(void) const
			{
				return (static_cast<DirectPortalObject *>(Node::GetObject()));
			}

			void Enable(void) override;
			void Disable(void) override;

			void Preprocess(void) override;
	};


	//# \class	RemotePortal		Represents a remote portal node in a world.
	//
	//# The $RemotePortal$ class represents a remote portal node in a world.
	//
	//# \def	class RemotePortal final : public Portal
	//
	//# \ctor	RemotePortal();
	//# \ctor	RemotePortal(const Vector2D& size);
	//
	//# \param	size	The size of a rectangular portal.
	//
	//# \desc
	//#
	//
	//# \base	Portal		A $RemotePortal$ node is a specific type of portal.
	//
	//# \also	$@RemotePortalObject@$


	//# \function	RemotePortal::GetRemoteTransform		Returns the remote transform.
	//
	//# \proto	const Transform4D& GetRemoteTransform(void) const;
	//
	//# \desc
	//# The $GetRemoteTransform$ function returns the coordinate transform that a remote portal applies
	//# to the camera. This determines what the camera position and orientation is on the "other side"
	//# of the portal.
	//#
	//# For remote portals that target the reflection buffer, the default remote transform is a
	//# reflection in the world-space plane of the portal. For remote portals that target any other
	//# rendering buffer, the default remote transform is the identity, meaning that the camera position
	//# and orientation do not change.


	class RemotePortal final : public Portal
	{
		friend class Portal;

		private:

			mutable Transform4D		previousCameraWorldTransform;

			RemotePortal(const RemotePortal& remotePortal);

			Node *Replicate(void) const override;

		public:

			C4API RemotePortal();
			C4API RemotePortal(const Vector2D& size);
			C4API ~RemotePortal();

			RemotePortalObject *GetObject(void) const
			{
				return (static_cast<RemotePortalObject *>(Node::GetObject()));
			}

			const Transform4D& GetPreviousCameraWorldTransform(void) const
			{
				return (previousCameraWorldTransform);
			}

			void SetPreviousCameraWorldTransform(const Transform4D& transform) const
			{
				previousCameraWorldTransform = transform;
			}

			C4API bool CalculateRemoteTransform(Transform4D *transform) const;
	};


	//# \class	OcclusionPortal		Represents an occlusion portal node in a world.
	//
	//# The $OcclusionPortal$ class represents an occlusion portal node in a world.
	//
	//# \def	class OcclusionPortal final : public Portal
	//
	//# \ctor	OcclusionPortal();
	//# \ctor	OcclusionPortal(const Vector2D& size);
	//
	//# \param	size	The size of a rectangular portal.
	//
	//# \desc
	//#
	//
	//# \base	Portal		An $OcclusionPortal$ node is a specific type of portal.
	//
	//# \also	$@OcclusionPortalObject@$
	//# \also	$@Zone@$


	class OcclusionPortal final : public Portal
	{
		friend class Portal;

		private:

			OcclusionPortal(const OcclusionPortal& occlusionPortal);

			Node *Replicate(void) const override;

		public:

			C4API OcclusionPortal();
			C4API OcclusionPortal(const Vector2D& size);
			C4API ~OcclusionPortal();

			OcclusionPortalObject *GetObject(void) const
			{
				return (static_cast<OcclusionPortalObject *>(Node::GetObject()));
			}

			OcclusionRegion *NewFrustumOcclusionRegion(const FrustumCamera *camera) const;
			OcclusionRegion *NewOrthoOcclusionRegion(const OrthoCamera *camera) const;
	};
}


#endif

// ZYUQURM