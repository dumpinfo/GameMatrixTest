 

#ifndef C4Node_h
#define C4Node_h


//# \component	World Manager
//# \prefix		WorldMgr/


#include "C4Cell.h"
#include "C4Resources.h"
#include "C4Properties.h"
#include "C4Objects.h"
#include "C4Connector.h"
#include "C4Renderable.h"


namespace C4
{
	//# \tree	Node
	//
	//# \node	RenderableNode
	//# \sub
	//#		\node	Geometry
	//#		\sub
	//#			\node	GenericGeometry
	//#			\node	PrimitiveGeometry
	//#			\sub
	//#				\node	PlateGeometry
	//#				\node	DiskGeometry
	//#				\node	HoleGeometry
	//#				\node	AnnulusGeometry
	//#				\node	BoxGeometry
	//#				\node	PyramidGeometry
	//#				\node	CylinderGeometry
	//#				\node	ConeGeometry
	//#				\node	SphereGeometry
	//#				\node	DomeGeometry
	//#				\node	TorusGeometry
	//#				\node	PathPrimitiveGeometry
	//#				\sub
	//#					\node	TubeGeometry
	//#					\node	ExtrusionGeometry
	//#					\node	RevolutionGeometry
	//#				\end
	//#				\node	PhysicsMgr/RopeGeometry
	//#				\node	PhysicsMgr/ClothGeometry
	//#				\node	PhysicsMgr/WaterGeometry
	//#			\end
	//#		\end
	//
	//#		\node	EffectMgr/Effect
	//#		\sub
	//#			\node	EffectMgr/ParticleSystem
	//#			\sub
	//#				\node	EffectMgr/PointParticleSystem
	//#				\node	EffectMgr/InfinitePointParticleSystem
	//#				\sub
	//#					\node	EffectMgr/StarField
	//#				\end
	//#				\node	EffectMgr/LineParticleSystem
	//#				\node	EffectMgr/QuadParticleSystem
	//#				\node	EffectMgr/FireParticleSystem
	//#				\node	EffectMgr/PolyboardParticleSystem
	//#				\node	EffectMgr/BlobParticleSystem
	//#			\end
	//#			\node	EffectMgr/MarkingEffect
	//#			\node	EffectMgr/ShaftEffect
	//#			\sub
	//#				\node	EffectMgr/BoxShaftEffect
	//#				\node	EffectMgr/CylinderShaftEffect
	//#				\node	EffectMgr/TruncatedPyramidShaftEffect
	//#				\node	EffectMgr/TruncatedConeShaftEffect
	//#			\end
	//#			\node	EffectMgr/QuadEffect
	//#			\node	EffectMgr/FlareEffect
	//#			\node	EffectMgr/BeamEffect
	//#			\node	EffectMgr/TubeEffect
	//#			\node	EffectMgr/FireEffect
	//#			\node	EffectMgr/ShockwaveEffect
	//#			\node	EffectMgr/PanelEffect
	//#		\end
	//# \end
	//
	//# \node	Light
	//# \sub
	//#		\node	InfiniteLight
	//#		\node	PointLight
	//#		\sub
	//#			\node	CubeLight
	//#			\node	SpotLight
	//#		\end
	//# \end
	//
	//# \node	Source
	//# \sub
	//#		\node	AmbientSource
	//#		\node	OmniSource
	//#		\sub
	//#			\node	DirectedSource
	//#		\end
	//# \end 
	//
	//# \node	Camera
	//# \sub 
	//#		\node	OrthoCamera
	//#		\node	FrustumCamera 
	//#		\sub
	//#			\node	RemoteCamera
	//#			\node	OrientedCamera 
	//#			\sub
	//#				\node	SpectatorCamera 
	//#				\node	BenchmarkCamera 
	//#			\end
	//#		\end
	//# \end
	// 
	//# \node	Zone
	//# \sub
	//#		\node	InfiniteZone
	//#		\node	BoxZone
	//#		\node	CylinderZone
	//#		\node	PolygonZone
	//# \end
	//
	//# \node	Portal
	//# \sub
	//#		\node	DirectPortal
	//#		\node	RemotePortal
	//#		\node	OcclusionPortal
	//# \end
	//
	//# \node	Trigger
	//# \sub
	//#		\node	BoxTrigger
	//#		\node	CylinderTrigger
	//#		\node	SphereTrigger
	//# \end
	//
	//# \node	Marker
	//# \sub
	//#		\node	LocatorMarker
	//#		\node	ConnectionMarker
	//#		\node	CubeMarker
	//#		\node	ShaderMarker
	//# \end
	//
	//# \node	Instance
	//# \node	Impostor
	//
	//# \node	Model
	//# \sub
	//#		\node	GenericModel
	//# \end
	//
	//# \node	Bone
	//
	//# \node	EffectMgr/Emitter
	//# \sub
	//#		\node	EffectMgr/BoxEmitter
	//#		\node	EffectMgr/CylinderEmitter
	//#		\node	EffectMgr/SphereEmitter
	//# \end
	//
	//# \node	Space
	//# \sub
	//#		\node	FogSpace
	//#		\node	ShadowSpace
	//#		\node	RadiositySpace
	//#		\node	AcousticsSpace
	//#		\node	OcclusionSpace
	//#		\node	PaintSpace
	//#		\node	PhysicsSpace
	//# \end
	//
	//# \node	Skybox
	//
	//# \node	PhysicsMgr/PhysicsNode
	//# \node	PhysicsMgr/Shape
	//# \sub
	//#		\node	PhysicsMgr/BoxShape
	//#		\node	PhysicsMgr/PyramidShape
	//#		\node	PhysicsMgr/CylinderShape
	//#		\node	PhysicsMgr/ConeShape
	//#		\node	PhysicsMgr/SphereShape
	//#		\node	PhysicsMgr/DomeShape
	//#		\node	PhysicsMgr/CapsuleShape
	//#		\node	PhysicsMgr/TruncatedPyramidShape
	//#		\node	PhysicsMgr/TruncatedConeShape
	//#		\node	PhysicsMgr/TruncatedDomeShape
	//# \end
	//# \node	PhysicsMgr/Joint
	//# \sub
	//#		\node	PhysicsMgr/SphericalJoint
	//#		\sub
	//#			\node	PhysicsMgr/UniversalJoint
	//#		\end
	//#		\node	PhysicsMgr/DiscalJoint
	//#		\sub
	//#			\node	PhysicsMgr/RevoluteJoint
	//#			\node	PhysicsMgr/CylindricalJoint
	//#			\sub
	//#				\node	PhysicsMgr/PrismaticJoint
	//#			\end
	//#		\end
	//# \end
	//# \node	PhysicsMgr/Field
	//# \sub
	//#		\node	PhysicsMgr/BoxField
	//#		\node	PhysicsMgr/CylinderField
	//#		\node	PhysicsMgr/SphereField
	//# \end
	//# \node	PhysicsMgr/Blocker
	//# \sub
	//#		\node	PhysicsMgr/PlateBlocker
	//#		\node	PhysicsMgr/BoxBlocker
	//#		\node	PhysicsMgr/CylinderBlocker
	//#		\node	PhysicsMgr/SphereBlocker
	//#		\node	PhysicsMgr/CapsuleBlocker
	//# \end


	typedef int32 CollisionState;
	typedef int32 ProximityResult;


	enum
	{
		kMaxNodeTypeSize		= 5
	};


	//# \enum	NodeType

	enum : NodeType
	{
		kNodeGeneric		= 0,			//## Generic node.
		kNodeCamera			= 'CAMR',		//## Camera node.
		kNodeLight			= 'LITE',		//## Light node.
		kNodeSource			= 'SORC',		//## Sound source node.
		kNodeGeometry		= 'GEOM',		//## Geometry node.
		kNodeInstance		= 'INST',		//## Instanced world node.
		kNodeModel			= 'MODL',		//## Model node (character, projectile, etc.).
		kNodeBone			= 'BONE',		//## Bone node used in skeletal animation.
		kNodeMarker			= 'MARK',		//## Marker node.
		kNodeTrigger		= 'TRIG',		//## Trigger node.
		kNodeEffect			= 'EFCT',		//## Effect node.
		kNodeEmitter		= 'EMIT',		//## Emitter node.
		kNodeSpace			= 'SPAC',		//## Space node.
		kNodePortal			= 'PORT',		//## Portal node.
		kNodeZone			= 'ZONE',		//## Zone node.
		kNodeSkybox			= 'SKYB',		//## Skybox node.
		kNodeImpostor		= 'IPST',		//## Impostor node.
		kNodePhysics		= 'PHYS',		//## Physics node.
		kNodeShape			= 'SHAP',		//## Physics shape node.
		kNodeJoint			= 'JONT',		//## Physics joint node.
		kNodeField			= 'FELD',		//## Physics field node.
		kNodeBlocker		= 'BLKR',		//## Physics blocker node.
		kNodeWildcard		= 0xFFFFFFFF
	};


	//# \enum	NodeFlags

	enum
	{
		kNodeNonpersistent						= 1 << 0,			//## The node is skipped during world serialization.
		kNodeDisabled							= 1 << 1,			//## The node is disabled (applies to most node types).
		kNodeCloneInhibit						= 1 << 2,			//## The node is skipped during a cloning operation.
		kNodeAnimateInhibit						= 1 << 3,			//## The node is not animated in any way as part of a model hierarchy.
		kNodeAnimateSubtree						= 1 << 4,			//## The node's subnodes can still be animated when $kNodeAnimateInhibit$ is set.
		kNodeUnsharedObject						= 1 << 5,			//## The node's object should be replicated instead of shared when the node is replicated.
		kNodeDirectEnableOnly					= 1 << 6,			//## The node can only be enabled or disabled by applying such an operation directly to the node, and not to a predecessor in the transform tree.
		kNodeSubzoneAmbient						= 1 << 7,			//## When a node is a member of a zone and at least one of its subzones, the ambient environments for the subzones have priority.
		kNodeTransformAnimationInhibit			= 1 << 8,			//## Animation is not applied to the node's transform. (Ignored if $kNodeAnimateInhibit$ is specified.)
		kNodeMorphWeightAnimationInhibit		= 1 << 9,			//## Animation is not applied to the morph weights if the node has a morph controller. (Ignored if $kNodeAnimateInhibit$ is specified.)
		kNodePreinitialized						= 1 << 24,
		kNodeInstanceHidden						= 1 << 25,
		kNodeVisibilitySite						= 1 << 27,
		kNodeIndependentVisibility				= 1 << 28,
		kNodeIsolatedVisibility					= 1 << 29,
		kNodeInfiniteVisibility					= 1 << 30,
		kNodeFlagsMask							= 0x00FFFFFF
	};


	//# \enum	PerspectiveMask

	enum
	{
		kPerspectivePrimary						= 0x00000001,		//## The perspective of the primary camera used to render the world.
		kPerspectiveReflection					= 0x00000002,		//## The perspective of a camera used to render into the reflection buffer.
		kPerspectiveRefraction					= 0x00000004,		//## The perspective of a camera used to render into the refraction buffer.
		kPerspectiveRadiositySpace				= 0x80000000,		//## The perspective of a camera used to generate a radiosity space texture map.

		kPerspectiveDirectMask					= 0x0000000F,		//## A mask that includes all direct perspectives (primary camera, reflection buffer, and refraction buffer). A node having these bits set will never be rendered from the direct perspective.
		kPerspectiveRemotePortalMask			= 0x000000F0,		//## A mask that includes all remote portal perspectives. A node having these bits set will never be rendered from the remote portal perspective.
		kPerspectiveCameraWidgetMask			= 0x00000F00,		//## A mask that includes all camera widget perspectives. A node having these bits set will never be rendered from the camera widget perspective.
		kPerspectiveDirectShadowMask			= 0x000F0000,		//## A mask that includes all direct shadow perspectives. A node having these bits set will never cast a shadow from the direct perspective.
		kPerspectiveRemotePortalShadowMask		= 0x00F00000,		//## A mask that includes all remote portal shadow perspectives. A node having these bits set will never cast a shadow from the remote portal perspective.
		kPerspectiveCameraWidgetShadowMask		= 0x0F000000,		//## A mask that includes all camera widget shadow perspectives. A node having these bits set will never cast a shadow from the camera widget perspective.

		kPerspectivePrimaryMask					= 0x00000111,		//## A mask that includes all primary camera perspectives (direct, remote portal, and camera widget). A node having these bits set will never be rendered through the primary camera.
		kPerspectiveReflectionMask				= 0x00000222,		//## A mask that includes all reflection buffer perspectives. A node having these bits set will never be rendered in the reflection buffer.
		kPerspectiveRefractionMask				= 0x00000444,		//## A mask that includes all refraction buffer perspectives. A node having these bits set will never be rendered in the refraction buffer.

		kPerspectivePrimaryShadowMask			= 0x01110000,		//## A mask that includes all primary camera shadow perspectives (direct, remote portal, and camera widget). A node having these bits set will never cast a shadow through the primary camera.
		kPerspectiveReflectionShadowMask		= 0x02220000,		//## A mask that includes all reflection buffer shadow perspectives. A node having these bits set will never cast a shadow in the reflection buffer.
		kPerspectiveRefractionShadowMask		= 0x04440000,		//## A mask that includes all refraction buffer shadow perspectives. A node having these bits set will never cast a shadow in the refraction buffer.

		kPerspectiveEverythingMask				= 0x8FFF0FFF,		//## A mask that includes every possible perspective. A node having these bits set will never be rendered, will never cast a shadow, and will not participate in the generation of radiosity space texture maps.

		kPerspectiveRemotePortalShift			= 4,
		kPerspectiveCameraWidgetShift			= 8,
		kPerspectiveDirectShadowShift			= 16,
		kPerspectiveRemotePortalShadowShift		= 20,
		kPerspectiveCameraWidgetShadowShift		= 24,

		kPerspectiveMaskSettingCount			= 11
	};


	enum
	{
		kSubzoneDepthUnlimited					= 0x7FFF
	};


	//# \enum	CollisionKind

	enum
	{
		kCollisionRigidBody			= 1 << 0,		//## Any type of rigid body.
		kCollisionCharacter			= 1 << 1,		//## A rigid body that represents a character.
		kCollisionProjectile		= 1 << 2,		//## A rigid body that represents a projectile.
		kCollisionVehicle			= 1 << 3,		//## A rigid body that represents a vehicle.

		kCollisionCamera			= 1 << 8,		//## A type of camera.
		kCollisionInteraction		= 1 << 9,

		kCollisionSightPath			= 1 << 13,		//## When used in an exclusion mask, does not obstruct sight.
		kCollisionSoundPath			= 1 << 15,		//## When used in an exclusion mask, does not obstruct sound.

		kCollisionBaseKind			= 1 << 16,		//## First application-defined collision kind.

		kCollisionExcludeAll		= 0xFFFFFFFF	//## When used as a collision exclusion mask, this value prevents collisions with everything.
	};


	C4API extern const char kConnectorKeyMember[];


	class Node;
	class World;
	class Controller;
	class Manipulator;
	class Geometry;
	class Marker;
	class Zone;
	class Shape;
	class VisibilityRegion;
	class OcclusionRegion;
	class RigidBodyController;


	//# \struct	CollisionPoint		Contains basic information about a collision point.
	//
	//# The $CollisionPoint$ structure contains basic information about a collision point.
	//
	//# \def	struct CollisionPoint
	//
	//# \data	CollisionPoint
	//
	//# \also	$@CollisionData@$
	//# \also	$@World::DetectCollision@$
	//# \also	$@World::QueryCollision@$


	//# \member		CollisionPoint

	struct CollisionPoint
	{
		float			param;			//## The fractional distance between the beginning and ending positions where the collision occurred.
		Point3D			position;		//## The world-space point at which the collision occurred.
		Vector3D		normal;			//## The world-space normal at the point of collision.
	};


	//# \struct	CollisionData		Contains extended information about a collision.
	//
	//# The $CollisionData$ structure contains extended information about a collision.
	//
	//# \def	struct CollisionData : CollisionPoint
	//
	//# \data	CollisionData
	//
	//# \desc
	//# The $CollisionData$ structure is used to return information about a collision when using the
	//# $@World::DetectCollision@$ and $@World::QueryCollision@$ functions.
	//
	//# \base	CollisionPoint		The $CollisionData$ structure extends the $CollisionPoint$ structure.
	//
	//# \also	$@World::DetectCollision@$
	//# \also	$@World::QueryCollision@$


	//# \member		CollisionData

	struct CollisionData : CollisionPoint
	{
		union
		{
			Geometry				*geometry;		//## The geometry node with which the collision occurred. This is valid when the $@World::DetectCollision@$ function returns $true$ or the $@World::QueryCollision@$ function returns $kCollisionStateGeometry$.
			RigidBodyController		*rigidBody;		//## The rigid body with which the collision occurred. This is valid only if the $@World::QueryCollision@$ function was called and it returned $kCollisionStateRigidBody$.
		};

		union
		{
			unsigned_int32			triangleIndex;	//## The index of the mesh triangle where the collision occurred. This is valid when the $@World::DetectCollision@$ function returns $true$ or the $@World::QueryCollision@$ function returns $kCollisionStateGeometry$.
			const Shape				*shape;			//## The shape with which the collision occurred. This is valid only if the $@World::QueryCollision@$ function was called and it returned $kCollisionStateRigidBody$.
		};
	};


	struct WorldHeader
	{
		int32		endian;
		int32		version;
		int32		controllerCount;
		int32		objectCount;
		int32		nodeCount;
		int32		offsetCount;
	};


	struct LoadContext
	{
		volatile int32		loadProgress;
		volatile int32		loadMagnitude;

		LoadContext()
		{
			loadProgress = 0;
			loadMagnitude = 1;
		}
	};


	class NodeTree : public Tree<Node>
	{
		private:

			NodeTree			*prevBranch;
			NodeTree			*nextBranch;
			NodeTree			*firstSubbranch;
			NodeTree			*lastSubbranch;

			bool				branchFlag;
			unsigned_int8		activeUpdateFlags;
			unsigned_int8		currentUpdateFlags;
			unsigned_int8		subtreeUpdateFlags;

			void RemoveBranch(NodeTree *branch);

			void PropagateUpdateFlags(unsigned_int32 flags);

		protected:

			NodeTree();

			void SetCurrentUpdateFlags(unsigned_int32 flags)
			{
				currentUpdateFlags = (unsigned_int8) flags;
			}

			void SetSubtreeUpdateFlags(unsigned_int32 flags)
			{
				subtreeUpdateFlags = (unsigned_int8) flags;
			}

			Node *GetFirstSubbranch(void) const;
			Node *GetNextBranch(void) const;

			void RemoveAllBranches(void);

		public:

			~NodeTree();

			unsigned_int32 GetActiveUpdateFlags(void) const
			{
				return (activeUpdateFlags);
			}

			unsigned_int32 GetCurrentUpdateFlags(void) const
			{
				return (currentUpdateFlags);
			}

			unsigned_int32 GetSubtreeUpdateFlags(void) const
			{
				return (subtreeUpdateFlags);
			}

			C4API void Detach(void) override;

			C4API void AppendSubnode(Node *node) override;
			C4API void PrependSubnode(Node *node) override;
			C4API void InsertSubnodeBefore(Node *node, Node *before) override;
			C4API void InsertSubnodeAfter(Node *node, Node *after) override;
			C4API void RemoveSubnode(Node *node) override;

			C4API void InvalidateUpdateFlags(unsigned_int32 flags);
			C4API void SetActiveUpdateFlags(unsigned_int32 flags);

			void AddBranch(NodeTree *branch);
			void CancelUpdate(void);

			virtual void Invalidate(void);
			virtual void Update(void);
	};


	//# \class	Node	The base class for all elements of a scene graph.
	//
	//# Every node that belongs to a scene graph is a subclass of the $Node$ class.
	//
	//# \def	class Node : public NodeTree, public Transformable, public LinkTarget<Node>, public Packable, public Configurable, public Creatable<Node>
	//
	//# \ctor	Node(NodeType type = kNodeGeneric);
	//
	//# \desc
	//# The $Node$ class provides the base functionality for all members of the scene graph representing a world.
	//# Most nodes are represented by subclasses of the $Node$ class such as $@Geometry@$ or $@Light@$.
	//# When the $Node$ class itself appears in a world, it simply acts as a generic grouping mechanism and has
	//# the $kNodeGeneric$ type.
	//
	//# \base	Utilities/Tree<Node>			Nodes are organized in a tree hierarchy. (The $NodeTree$ class is an intermediate class used internally.)
	//# \base	Utilities/Transformable			Holds the object-to-world transform for a node.
	//# \base	Utilities/LinkTarget<Node>		Nodes support smart linking with the $@Utilities/Link@$ class.
	//# \base	ResourceMgr/Packable			Nodes can be packed for storage in resources.
	//# \base	InterfaceMgr/Configurable		Nodes can define configurable parameters that are exposed as user interface widgets in the World Editor.
	//# \base	System/Creatable<Object>		New node subclasses may be defined by an application, and a creator function can be installed using the $Creatable$ class.
	//
	//# \also	$@Object@$
	//# \also	$@Controller/Controller@$


	//# \function	Node::GetNodeType		Returns the type of a node.
	//
	//# \proto	NodeType GetNodeType(void) const;
	//
	//# \desc
	//# The $GetNodeType$ function returns the type of a node. The following table lists the built-in types
	//# that can be returned. Additional types may be defined by the application.
	//
	//# \table	NodeType


	//# \function	Node::GetNodeFlags		Returns the node flags.
	//
	//# \proto	unsigned_int32 GetNodeFlags(void) const;
	//
	//# \desc
	//# The $GetNodeFlags$ function returns the node flags, which can be a combination (through logical OR) of the following bit flags.
	//
	//# \table	NodeFlags
	//
	//# \also	$@Node::SetNodeFlags@$


	//# \function	Node::SetNodeFlags		Sets the node flags.
	//
	//# \proto	void SetNodeFlags(unsigned_int32 flags);
	//
	//# \param	flags	The new node flags.
	//
	//# \desc
	//# The $SetNodeFlags$ function sets the node flags to the value given by the $flags$ parameter, which can be
	//# a combination (through logical OR) of the following bit flags.
	//
	//# \table	NodeFlags
	//
	//# \also	$@Node::GetNodeFlags@$


	//# \function	Node::GetPerspectiveExclusionMask		Returns the perspective exclusion mask.
	//
	//# \proto	unsigned_int32 GetPerspectiveExclusionMask(void) const;
	//
	//# \desc
	//# The $GetPerspectiveExclusionMask$ function returns the perspective exclusion mask that determines from
	//# what camera perspectives the node is enabled. The mask can be a combination (through logical OR)
	//# of the following constants.
	//
	//# \table	PerspectiveMask
	//
	//# See the $@Node::SetPerspectiveExclusionMask@$ function for more information.
	//
	//# \also	$@Node::SetPerspectiveExclusionMask@$


	//# \function	Node::SetPerspectiveExclusionMask		Sets the perspective exclusion mask.
	//
	//# \proto	void SetPerspectiveExclusionMask(unsigned_int32 mask);
	//
	//# \param	mask	The new perspective exclusion mask.
	//
	//# \desc
	//# The $SetPerspectiveExclusionMask$ function sets the perspective exclusion mask that determines from
	//# what camera perspectives the node is enabled to the value specified by the $mask$ parameter.
	//# The mask can be a combination (through logical OR) of the following constants.
	//
	//# \table	PerspectiveMask
	//
	//# For any bits that are set in the perspective exclusion mask, the node is not enabled for cameras
	//# having the matching perspective. The initial value of the mask is 0, meaning that the node is
	//# enabled from all camera perspectives.
	//#
	//# There are three different perspective classifications defined by the engine for three
	//# different camera classifications, and each combination of these classifications can apply to
	//# geometry rendering or shadow rendering. The three perspective classifications correspond to the
	//# primary rendering buffer, the reflection buffer, and the refraction buffer. The three camera
	//# classifications correspond to direct rendering, rendering through a remote portal, and rendering
	//# through a camera widget in a panel effect. The bits corresponding to the three perspective
	//# classifications for direct rendering are the values $kPerspectivePrimary$, $kPerspectiveReflection$,
	//# and $kPerspectiveRefraction$. For the other camera classifications and for shadows, these three
	//# bit values can be shifted by one of the following constants.
	//
	//# \value	kPerspectiveRemotePortalShift			The shift value for the remote portal classification.
	//# \value	kPerspectiveCameraWidgetShift			The shift value for the camera widget classification.
	//# \value	kPerspectiveDirectShadowShift			The shift value for the direct shadow classification.
	//# \value	kPerspectiveRemotePortalShadowShift		The shift value for the remote portal shadow classification.
	//# \value	kPerspectiveCameraWidgetShadowShift		The shift value for the camera widget shadow classification.
	//
	//# For example, to prevent a node from being rendered or casting a shadow in reflections seen through
	//# a camera widget, the perspective exclusion mask should be set to
	//# $(kPerspectiveReflection << kPerspectiveCameraWidgetShift) | (kPerspectiveReflection << kPerspectiveCameraWidgetShadowShift)$
	//
	//# \also	$@Node::GetPerspectiveExclusionMask@$


	//# \function	Node::GetWorld		Returns the world to which a node belongs.
	//
	//# \proto	World *GetWorld(void) const;
	//
	//# \desc
	//# The $GetWorld$ function returns a pointer to the $@World@$ object to which the node belongs.
	//# A node's world pointer is automatically set when a node is preprocessed.
	//
	//# \also	$@Node::AppendNewSubnode@$
	//# \also	$@World@$


	//# \div
	//# \function	Node::GetOwningZone		Returns the most immediate zone containing a node.
	//
	//# \proto	Zone *GetOwningZone(void) const;
	//
	//# \desc
	//# The $GetOwningZone$ function returns the most immediate zone containing a node. If the node is not inside
	//# a zone, then the return value is $nullptr$. If the node is a zone itself, then the containing zone is returned,
	//# not the zone for which this function is called.
	//
	//# \also	$@Zone@$


	//# \function	Node::AppendNewSubnode		Adds a newly created subnode to a node.
	//
	//# \proto	void AppendNewSubnode(Node *node);
	//
	//# \param	node	A pointer to the subnode to add.
	//
	//# \desc
	//# The $AppendNewSubnode$ function add the subnode specified by the $node$ parameter to a node
	//# and subsequently preprocesses the subnode. Calling $AppendNewSubnode$ is equivalent to calling
	//# $@Utilities/Tree::AppendSubnode@$ for the same node object and then calling $@Node::Preprocess@$
	//# for the node specified by $node$.
	//
	//# \also	$@Utilities/Tree::AppendSubnode@$
	//# \also	$@Node::Preprocess@$
	//# \also	$@World::AddNewNode@$


	//# \div
	//# \function	Node::Enable		Enables a node tree.
	//
	//# \proto	virtual void Enable(void);
	//
	//# \desc
	//# The $Enable$ function enables a node and all of its subnodes. Calling the $Enable$ function is
	//# equivalent to clearing the $kNodeDisabled$ flag for each node in the tree rooted at the
	//# node for which it is called.
	//#
	//# If any subnode in the tree has the $kNodeDirectEnableOnly$ flag set, then that subnode is not
	//# affected by the $Enable$ function. The root node for which the $Enable$ function is called is
	//# always enabled regardless of the $kNodeDirectEnableOnly$ flag.
	//
	//# \also	$@Node::Disable@$
	//# \also	$@Node::GetNodeFlags@$
	//# \also	$@Node::SetNodeFlags@$


	//# \function	Node::Disable		Disables a node tree.
	//
	//# \proto	virtual void Disable(void);
	//
	//# \desc
	//# The $Disable$ function disables a node and all of its subnodes. Calling the $Disable$ function is
	//# equivalent to setting the $kNodeDisabled$ flag for each node in the tree rooted at the
	//# node for which it is called.
	//#
	//# If any subnode in the tree has the $kNodeDirectEnableOnly$ flag set, then that subnode is not
	//# affected by the $Disable$ function. The root node for which the $Disable$ function is called is
	//# always disabled regardless of the $kNodeDirectEnableOnly$ flag.
	//
	//# \also	$@Node::Enable@$
	//# \also	$@Node::GetNodeFlags@$
	//# \also	$@Node::SetNodeFlags@$


	//# \div
	//# \function	Node::GetObject		Returns a node's object.
	//
	//# \proto	Object *GetObject(void) const;
	//
	//# \desc
	//# The $GetObject$ function returns the primary object referenced by a node. If the node does not reference an
	//# object, then the return value is $nullptr$.
	//#
	//# Objects hold data that can be shared among multiple nodes for the purposes of instancing. An object's reference
	//# count represents the number of instances of that object in a scene.
	//
	//# \also	$@Node::SetObject@$
	//# \also	$@Object@$


	//# \function	Node::SetObject		Sets a node's object.
	//
	//# \proto	void SetObject(Object *object);
	//
	//# \param	object		A pointer to the object to which the node should refer. This can be $nullptr$.
	//
	//# \desc
	//# The $SetObject$ function sets the primary object referenced by a node. If the node previously referenced a
	//# different object, then that reference is released, causing the destruction of the old object if its reference
	//# count reaches zero. The reference count of the new object (if $object$ is not $nullptr$) is incremented by one.
	//
	//# \also	$@Node::GetObject@$
	//# \also	$@Object@$


	//# \function	Node::GetController		Returns a node's controller.
	//
	//# \proto	Controller *GetController(void) const;
	//
	//# \desc
	//# The $GetController$ function returns the $@Controller/Controller@$ assigned to a node. If the node does not
	//# have a controller, then the return value is $nullptr$.
	//
	//# \also	$@Controller/Controller@$
	//# \also	$@Node::SetController@$


	//# \function	Node::SetController		Sets a node's controller.
	//
	//# \proto	void SetController(Controller *controller);
	//
	//# \param	controller		A pointer to the controller. This can be $nullptr$.
	//
	//# \desc
	//# The $SetController$ function assigns a controller to a node. If the node already has a different controller
	//# assigned to it, then that controller is removed and becomes unassigned, but it is not destroyed.
	//
	//# \also	$@Controller/Controller@$
	//# \also	$@Node::GetController@$


	//# \div
	//# \function	Node::GetNodeTransform		Returns a node's local transform.
	//
	//# \proto	const Transform4D& GetNodeTransform(void) const;
	//
	//# \desc
	//# The $GetNodeTransform$ function returns a node's local transform. This transform represents the change
	//# in coordinates between a node's local coordinate system (object space) and the coordinate system of its
	//# immediate parent node. To retrieve the transform from object space to world space, call the
	//# $@Utilities/Transformable::GetWorldTransform@$ function.
	//
	//# \also	$@Node::SetNodeTransform@$
	//# \also	$@Node::GetNodePosition@$
	//# \also	$@Utilities/Transformable::GetWorldTransform@$
	//# \also	$@Math/Transform4D@$


	//# \function	Node::SetNodeTransform		Sets a node's local transform.
	//
	//# \proto	void SetNodeTransform(const Transform4D& transform);
	//# \proto	void SetNodeTransform(const Matrix3D& matrix, const Point3D& position);
	//# \proto	void SetNodeTransform(const Vector3D& c1, const Vector3D& c2, const Vector3D& c3, const Point3D& c4);
	//
	//# \param	transform	The new local transform.
	//# \param	matrix		The new upper-left 3&times;3 portion of the local transform.
	//# \param	position	The new local position.
	//# \param	c1			The first column of the 4D transform.
	//# \param	c2			The second column of the 4D transform.
	//# \param	c3			The third column of the 4D transform.
	//# \param	c4			The fourth column of the 4D transform.
	//
	//# \desc
	//# The $SetNodeTransform$ function sets a node's local transform. This transform represents the change
	//# in coordinates between a node's local coordinate system (object space) and the coordinate system of its
	//# immediate parent node.
	//#
	//# After the node transform has been set, the world transform returned by the
	//# $@Utilities/Transformable::GetWorldTransform@$ function is not valid until the node is updated. To cause
	//# a node to be updated, the $@Node::Invalidate@$ function should be called after altering its transform.
	//# The node will then be updated the next time the World Manager processes the scene containing it or when
	//# the $@Node::Update@$ function is explicitly called for the node.
	//
	//# \special
	//# The $SetNodeTransform$ function should not be called for any node under the control of the physics simulation
	//# through a $@PhysicsMgr/RigidBodyController@$. Instead, the $@PhysicsMgr/RigidBodyController::SetRigidBodyTransform@$
	//# function should be called.
	//
	//# \also	$@Node::GetNodeTransform@$
	//# \also	$@Node::SetNodeMatrix3D@$
	//# \also	$@Node::SetNodePosition@$
	//# \also	$@Utilities/Transformable::GetWorldTransform@$
	//# \also	$@Math/Transform4D@$


	//# \function	Node::SetNodeMatrix3D		Sets the upper-left 3&times;3 portion of a node's local transform.
	//
	//# \proto	void SetNodeMatrix3D(const Matrix3D& matrix);
	//# \proto	void SetNodeMatrix3D(const Vector3D& c1, const Vector3D& c2, const Vector3D& c3);
	//
	//# \param	matrix		The new upper-left 3&times;3 portion of the local transform.
	//# \param	c1			The first column of the 3&times;3 matrix.
	//# \param	c2			The second column of the 3&times;3 matrix.
	//# \param	c3			The third column of the 3&times;3 matrix.
	//
	//# \desc
	//# The $SetNodeMatrix3D$ function sets the upper-left 3&times;3 portion of a node's local transform without
	//# effecting the translation portion in the fourth column of the matrix. As with the $@Node::SetNodeTransform@$
	//# function, the world transform is not valid until the node is updated.
	//
	//# \special
	//# The $SetNodeMatrix3D$ function should not be called for any node under the control of the physics simulation
	//# through a $@PhysicsMgr/RigidBodyController@$. Instead, the $@PhysicsMgr/RigidBodyController::SetRigidBodyMatrix3D@$
	//# function should be called.
	//
	//# \also	$@Node::SetNodeTransform@$
	//# \also	$@Node::SetNodePosition@$


	//# \function	Node::GetNodePosition		Returns a node's local position.
	//
	//# \proto	const Point3D& GetNodePosition(void) const;
	//
	//# \desc
	//# The $GetNodePosition$ function returns a node's local position. This position represents the origin of the
	//# node in the coordinate system of the node's immediate parent node. To retrieve the world-space position,
	//# call the $@Utilities/Transformable::GetWorldPosition@$ function.
	//
	//# \also	$@Node::SetNodePosition@$
	//# \also	$@Node::GetNodeTransform@$
	//# \also	$@Utilities/Transformable::GetWorldPosition@$
	//# \also	$@Math/Point3D@$


	//# \function	Node::SetNodePosition		Sets a node's local position.
	//
	//# \proto	void SetNodePosition(const Point3D& position);
	//
	//# \param	position	The new local position.
	//
	//# \desc
	//# The $SetNodePosition$ function sets a node's local position without affecting the rest of the node's transform.
	//# As with the $@Node::SetNodeTransform@$ function, the world transform is not valid until the node is updated.
	//
	//# \special
	//# The $SetNodePosition$ function should not be called for any node under the control of the physics simulation
	//# through a $@PhysicsMgr/RigidBodyController@$. Instead, the $@PhysicsMgr/RigidBodyController::SetRigidBodyPosition@$
	//# function should be called.
	//
	//# \also	$@Node::GetNodePosition@$
	//# \also	$@Node::SetNodeTransform@$
	//# \also	$@Node::SetNodeMatrix3D@$


	//# \function	Node::StopMotion		Resets the motion information stored for a node so that motion blur is correctly stopped.
	//
	//# \proto	void StopMotion(void);
	//
	//# \desc
	//# The $StopMotion$ function should be called when a node that has been in motion stops. This function ensures that
	//# the previous transforms used for rendering motion blur are reset so that motion blur does not continue to be
	//# applied to a stopped object. When the $StopMotion$ function is called, it affects the node for which it is called
	//# and its entire tree of subnodes.
	//#
	//# The $@Node::Invalidate@$ function should always be called for the same node during the same frame when the $StopMotion$
	//# function is called. Failing to do this will result in motion blur artifacts.
	//
	//# \also	$@Node::Invalidate@$


	//# \function	Node::Invalidate		Invalidates the world transform and dependent information.
	//
	//# \proto	void Invalidate(void);
	//
	//# \desc
	//# The $Invalidate$ function should be called whenever a node's local transform is altered through the $@Node::SetNodeTransform@$
	//# function or one of the associated functions that alters only part of the local transform. When a node is invalidated, it is
	//# queued to be updated the next time the World Manager processes the scene before rendering. When a node is updated, its world
	//# transform is recalculated, and if needed, its world-space bounding box and world-space bounding sphere are recalculated.
	//#
	//# Invalidating a node causes its entire subtree to be invalidated, so it is not necessary to call the $Invalidate$ function
	//# for each node in the subtree if many transforms are changed. Calling the $Invalidate$ function for the highest node in a
	//# hierarchy is sufficient.
	//
	//# \also	$@Node::Update@$
	//# \also	$@Node::SetNodeTransform@$


	//# \function	Node::Update		Updates the world transform and dependent information.
	//
	//# \proto	void Update(void);
	//
	//# \desc
	//# The $Update$ function causes the world transform to be recalculated for a node. When needed, the world-space bounding box
	//# and world-space bounding sphere are also recalculated during the update operation.
	//#
	//# Updating a node causes its entire subtree to be updated, so the $Update$ function should be called only for the highest node
	//# in a hierarchy.
	//#
	//# It is normally not necessary to call the $Update$ function directly because the World Manager automatically updates any
	//# invalid nodes when it processes the world before rendering.
	//
	//# \also	$@Node::Invalidate@$
	//# \also	$@Node::SetNodeTransform@$


	//# \div
	//# \function	Node::GetBoundingSphere		Returns a node's world-space bounding sphere.
	//
	//# \proto	const BoundingSphere *GetBoundingSphere(void) const;
	//
	//# \desc
	//# The $GetBoundingSphere$ function returns a node's world-space bounding sphere.
	//
	//# \also	$@Node::Visible@$


	//# \function	Node::Visible		Determines whether a node is visible within a given region.
	//
	//# \proto	bool Visible(const VisibilityRegion *region) const;
	//# \proto	bool Visible(const VisibilityRegion *region, const List<OcclusionRegion> *occlusionList) const;
	//
	//# \param	region			The region for which the node should be tested for visibility.
	//# \param	occlusionList	A list of occlusion regions for which the node should be tested for occlusion. This cannot be $nullptr$.
	//
	//# \desc
	//# The $Visible$ function calls a node's currently installed visibility procedure to determine whether
	//# the node is visible within the region specified by the $region$ parameter. This function is normally
	//# only called from within the World Manager. The return value is $true$ if the node is visible, and
	//# $false$ otherwise.
	//#
	//# If the $occlusionList$ parameter is specified, then the $Visible$ function also determines whether
	//# the node is occluded by any of the regions in the occlusion list. If the node is fully occluded by
	//# any single region in the occlusion list, then the return value is $false$.
	//#
	//# By default, a node's visibility procedure tests the node's bounding sphere against the planes of the
	//# given regions. A different visibility procedure can be installed by calling the
	//# $@Node::SetVisibilityProc@$ function.
	//
	//# \also	$@Node::SetVisibilityProc@$
	//# \also	$@VisibilityRegion@$
	//# \also	$@OcclusionRegion@$


	//# \function	Node::SetVisibilityProc		Sets the function that handles visibility testing for a node.
	//
	//# \proto	void SetVisibilityProc(VisibilityProc *proc);
	//
	//# \param	proc		A pointer to the function that performs the visibility test.
	//
	//# \desc
	//# The $SetVisibilityProc$ function installs the procedure that is called when visibility testing
	//# is needed for a node. The $VisibilityProc$ type is defined as follows.
	//
	//# \code	typedef bool VisibilityProc(const Node *, const VisibilityRegion *);
	//
	//# When the visibility procedure is called, it can use whatever means is appropriate to determine whether
	//# the node is visible within the given region. This is normally accomplished by calling one or more of
	//# the following member functions of the $@VisibilityRegion@$ class.
	//#
	//# $@VisibilityRegion::PolygonVisible@$
	//# $@VisibilityRegion::SphereVisible@$
	//# $@VisibilityRegion::EllipsoidVisible@$
	//# $@VisibilityRegion::BoxVisible@$
	//# $@VisibilityRegion::CylinderVisible@$
	//#
	//# The visibility procedure should return $true$ if the node is visible and $false$ otherwise.
	//#
	//# By default, the $Node::SphereVisible$ function is installed as a node's visibility procedure. This function
	//# passes the node's bounding sphere to the $@VisibilityRegion::SphereVisible@$ function to determine whether the node
	//# is visible.
	//#
	//# The $Node::AlwaysVisible$ function may be installed as the visibility procedure to force a node to be
	//# visible all the time.
	//#
	//# If a custom visibility procedure is installed using the $SetVisibilityProc$, then a custom occlusion
	//# procedure should also be installed using the $@Node::SetOcclusionProc@$ function if occlusion portals are in use.
	//
	//# \also	$@Node::Visible@$
	//# \also	$@Node::SetOcclusionProc@$
	//# \also	$@VisibilityRegion@$


	//# \function	Node::SetOcclusionProc		Sets the function that handles occlusion testing for a node.
	//
	//# \proto	void SetOcclusionProc(OcclusionProc *proc);
	//
	//# \param	proc		A pointer to the function that performs the occlusion test.
	//
	//# \desc
	//# The $SetOcclusionProc$ function installs the procedure that is called when occlusion testing
	//# is needed for a node. The $OcclusionProc$ type is defined as follows.
	//
	//# \code	typedef bool OcclusionProc(const Node *, const VisibilityRegion *, const List<OcclusionRegion> *);
	//
	//# When the occlusion procedure is called, it should perform the same visibility test as the function installed
	//# by the $@Node::SetVisibilityProc@$ function. The occlusion procedure should then use whatever means is appropriate
	//# to determine whether the node is occluded within any of the regions in the list specified by the last parameter.
	//# This is normally accomplished by calling one or more of the following member functions of the $@OcclusionRegion@$ class.
	//#
	//# $@OcclusionRegion::PolygonOccluded@$
	//# $@OcclusionRegion::SphereOccluded@$
	//# $@OcclusionRegion::EllipsoidOccluded@$
	//# $@OcclusionRegion::BoxOccluded@$
	//# $@OcclusionRegion::CylinderOccluded@$
	//#
	//# The occlusion procedure should return $true$ if the node is visible and $false$ otherwise.
	//#
	//# By default, the $Node::SphereVisible$ function is installed as a node's occlusion procedure. This function
	//# passes the node's bounding sphere to the $@OcclusionRegion::SphereOccluded@$ function for each region in the list to
	//# determine whether the node is visible.
	//#
	//# The $Node::AlwaysVisible$ function may be installed as the occlusion procedure to force a node to be
	//# unoccluded all the time.
	//#
	//# If a custom occlusion procedure is installed using the $SetOcclusionProc$, then a custom visibility
	//# procedure should also be installed using the $@Node::SetVisibilityProc@$ function.
	//
	//# \also	$@Node::Visible@$
	//# \also	$@Node::SetVisibilityProc@$
	//# \also	$@VisibilityRegion@$
	//# \also	$@OcclusionRegion@$


	//# \function	Node::CalculateBoundingBox		Calculates the bounding box for a node.
	//
	//# \proto	virtual bool CalculateBoundingBox(Box3D *box) const;
	//
	//# \param	box		A pointer to the location where the bounding box is stored.
	//
	//# \desc
	//# The $CalculateBoundingBox$ function should be overridden by any node type that needs to calculate its
	//# bounding box in some special way. The extents of the axis-aligned bounding box in the local coordinate
	//# system of the node should be stored in the $@GraphicsMgr/Box3D@$ object specified by the $box$ parameter.
	//# The $CalculateBoundingBox$ function should return $true$ if a bounding box could be calculated, and it
	//# should return $false$ if the node does not have a bounding box (in which case the data stored in the
	//# $box$ parameter is ignored).
	//
	//# \also	$@GraphicsMgr/Box3D@$


	//# \div
	//# \function	Node::GetHub		Returns the hub attached to a node.
	//
	//# \proto	Hub *GetHub(void) const;
	//
	//# \desc
	//# The $GetHub$ function returns a pointer to the hub attached to a node. A hub exists for a node whenever the node has
	//# any outgoing or incoming connector. If a node has no hub, then the $GetHub$ function returns $nullptr$.
	//#
	//# To iterate over the connectors for a node, the member functions of the $@Utilities/GraphElement@$ base
	//# class can be used. For example, to iterate over all outgoing connectors, call the $@Utilities/GraphElement::GetFirstOutgoingEdge@$
	//# function to retrieve the first connector, and then call $@Utilities/GraphEdge::GetNextOutgoingEdge@$ function for the connector
	//# until $nullptr$ is returned.
	//
	//# \also	$@Hub@$
	//# \also	$@Connector@$
	//# \also	$@Node::AddConnector@$
	//# \also	$@Node::RemoveConnector@$
	//# \also	$@Node::GetConnectedNode@$
	//# \also	$@Node::SetConnectedNode@$


	//# \function	Node::AddConnector		Adds a node connection.
	//
	//# \proto	Connector *AddConnector(const char *key, Node *node = nullptr);
	//
	//# \param	key		The key value for the connector. This is a string up to 15 bytes in length, not counting the null terminator.
	//# \param	node	The initial target node for the connector.
	//
	//# \desc
	//# The $AddConnector$ function attaches a new $@Connector@$ object to a node. The $key$ parameter specifies a
	//# unique identifier for the connector that is normally used to assign some kind of meaning to the node that
	//# it connects to. The key value is used by the $@Node::GetConnectedNode@$ function to retrieve the node that
	//# is connected through a particular connector. The key value should be unique among all connectors attached to
	//# the same node. If a node has two or more connectors with the same key, then it is undefined which connector
	//# will be returned by searches for a connector with that key.
	//#
	//# If necessary, a hub is created for the node for which the $AddConnector$ function is called. The new connector becomes
	//# the last outgoing edge for the node's hub. If the $node$ parameter is not $nullptr$, then a hub is also created for
	//# the target node, if necessary, and the new connector becomes the last incoming edge for the target node's hub.
	//#
	//# If the $node$ parameter is $nullptr$, then the new connector's start and finish elements are both set to the hub attached
	//# to the node for which the $AddConnector$ function is called.
	//
	//# \also	$@Hub@$
	//# \also	$@Connector@$
	//# \also	$@Node::GetHub@$
	//# \also	$@Node::RemoveConnector@$
	//# \also	$@Node::GetConnectedNode@$
	//# \also	$@Node::SetConnectedNode@$


	//# \function	Node::RemoveConnector	Removes a node connector.
	//
	//# \proto	bool RemoveConnector(const char *key);
	//
	//# \param	key		The key value for the connector. This is a string up to 15 bytes in length, not counting the null terminator.
	//
	//# \desc
	//# The $RemoveConnector$ function removes an existing $@Connector@$ object from a node. The $key$ parameter specifies the
	//# unique identifier for the connector that is to be removed. If a connector with this key exists, then it is deleted,
	//# and the $RemoveConnector$ function returns $true$. The no such connector exists, then the $RemoveConnector$ function
	//# performs no action and returns $false$.
	//#
	//# If the node's hub has no connectors remaining after the connector specified by the $key$ parameter is deleted, then
	//# the hub is also deleted.
	//
	//# \also	$@Hub@$
	//# \also	$@Connector@$
	//# \also	$@Node::GetHub@$
	//# \also	$@Node::RemoveConnector@$
	//# \also	$@Node::GetConnectedNode@$
	//# \also	$@Node::SetConnectedNode@$


	//# \function	Node::GetConnectedNode		Returns the connected node with a particular key.
	//
	//# \proto	Node *GetConnectedNode(const char *key) const;
	//
	//# \param	key		The key value of the connector.
	//
	//# \desc
	//# The $GetConnectedNode$ function searches for a connector having a key matching the $key$ parameter and,
	//# if such a connector is found, returns the node to which it connects. If there is no connector with the
	//# matching key, or the connector exists but is not connected to another node, then the return value is $nullptr$.
	//# If the $key$ parameter points to an empty string, then the $GetConnectedNode$ function always returns $nullptr$.
	//
	//# \also	$@Node::SetConnectedNode@$
	//# \also	$@Node::AddConnector@$
	//# \also	$@Node::RemoveConnector@$
	//# \also	$@Node::GetHub@$


	//# \function	Node::SetConnectedNode		Sets the connected node with a particular key.
	//
	//# \proto	void SetConnectedNode(const char *key, Node *node);
	//
	//# \param	key		The key value of the connector.
	//# \param	node	The node to which the connector should be linked. This may be $nullptr$.
	//
	//# \desc
	//# The $SetConnectedNode$ function searches for a connector having a key matching the $key$ parameter and,
	//# if such a connector is found, connects it to the node specified by the $node$ parameter. If there is no
	//# connector with the matching key and the $node$ is not $nullptr$, then it is created and connected to the
	//# specified node.
	//#
	//# If the $node$ parameter is $nullptr$ and a connector with the matching key exists, then the target of the
	//# connector is set to the node for which the $SetConnectedNode$ function is called. That is, unconnected
	//# connectors loop back to their starting points.
	//
	//# \also	$@Node::GetConnectedNode@$
	//# \also	$@Node::AddConnector@$
	//# \also	$@Node::RemoveConnector@$


	//# \div
	//# \function	Node::GetProperty		Returns the property of a given type that is attached to a node.
	//
	//# \proto	Property *GetProperty(PropertyType type) const;
	//
	//# \param	type	The property type.
	//
	//# \desc
	//# The $GetProperty$ function returns the property attached to a node having the type specified
	//# by the $type$ parameter. If no such property exists, then the return value is $nullptr$.
	//
	//# \also	$@Node::GetFirstProperty@$
	//# \also	$@Node::AddProperty@$
	//# \also	$@Property@$


	//# \function	Node::GetFirstProperty		Returns the first property directly attached to a node.
	//
	//# \proto	Property *GetFirstProperty(void) const;
	//
	//# \desc
	//# The $GetFirstProperty$ function returns the first property directly attached to a node. All of the
	//# properties directly attached to a node can be iterated by repeatedly calling the $@Utilities/ListElement::Next@$
	//# function on the returned pointer. If no properties are directly attached to a node, then the return
	//# value is $nullptr$.
	//
	//# \also	$@Node::GetProperty@$
	//# \also	$@Node::AddProperty@$
	//# \also	$@Property@$


	//# \function	Node::AddProperty		Attaches a property to a node.
	//
	//# \proto	bool AddProperty(Property *property);
	//
	//# \param	property	The property to attach.
	//
	//# \desc
	//# The $AddProperty$ function attaches the property specified by the $property$ parameter directly to a node.
	//# A property can be attached to only one node at a time, so the property is first removed from any other node
	//# to which it may have previously been attached.
	//#
	//# Only one property of any particular type can be attached to a node at one time. If the node has no property having
	//# the same type as the property specified by the $property$ parameter, then the return value is $true$. If a property
	//# having the same type is already attached to the node, then no action is taken and the $AddProperty$ function returns
	//# $false$. In the case, the property is not removed from any node to which it may have previously been attached.
	//
	//# \also	$@Node::GetFirstProperty@$
	//# \also	$@Node::GetProperty@$
	//# \also	$@Property@$


	//# \function	Node::GetPropertyObject		Returns the property object attached to a node.
	//
	//# \proto	PropertyObject *GetPropertyObject(void) const;
	//
	//# \desc
	//# The $GetPropertyObject$ function returns a pointer to the property object attached to a node.
	//# If there is no property object attached to a node, then the return value is $nullptr$.
	//# A node does not have a property object by default.
	//
	//# \also	$@Node::SetPropertyObject@$
	//# \also	$@Node::GetSharedProperty@$
	//# \also	$@PropertyObject@$


	//# \function	Node::SetPropertyObject		Attaches a property object to a node.
	//
	//# \proto	void SetPropertyObject(PropertyObject *object);
	//
	//# \param	object		The property object to attach.
	//
	//# \desc
	//# The $SetPropertyObject$ function attaches the property object specified by the $object$ parameter to a node.
	//# If $object$ is $nullptr$, then the node does not have a property object after this function is called.
	//# Otherwise, the reference count of the property object is incremented, and the new property is attached to the node.
	//# The reference count of any property object previously attached to the node is decremented, and the old property
	//# object is deleted if its reference count reaches zero.
	//
	//# \also	$@Node::SetPropertyObject@$
	//# \also	$@Node::GetSharedProperty@$
	//# \also	$@PropertyObject@$


	//# \function	Node::GetSharedProperty		Returns the shared property of a given type that is stored in a node's property object.
	//
	//# \proto	Property *GetSharedProperty(PropertyType type) const;
	//
	//# \param	type	The property type.
	//
	//# \desc
	//# The $GetSharedProperty$ function returns the property stored in a node's property object having the type specified
	//# by the $type$ parameter. If no such property exists or there is no property object attached to the node, then the
	//# return value is $nullptr$.
	//
	//# \also	$@Node::GetPropertyObject@$
	//# \also	$@Node::SetPropertyObject@$
	//# \also	$@PropertyObject@$


	//# \function	Node::GetNodeName		Returns the name of a node.
	//
	//# \proto	const char *GetNodeName(void) const;
	//
	//# \desc
	//# The $GetNodeName$ function returns a pointer to the name of a node. If the node does not have a name,
	//# then the return value is $nullptr$. (The name itself is stored in a property attached to the node.)
	//
	//# \also	$@Node::SetNodeName@$


	//# \function	Node::SetNodeName		Sets the name of a node.
	//
	//# \proto	void SetNodeName(const char *name);
	//
	//# \param	name	The new node name. This cannot be $nullptr$.
	//
	//# \desc
	//# The $SetNodeName$ function sets the name of a node to the string specified by the $name$ parameter.
	//# If the node did not previously have a name, then a new property is created in which to store the name,
	//# and that property is attached to the node. There is no practical limit to the length of a node name.
	//#
	//# To remove the name from a node, use the $@Node::GetProperty@$ function to get the property having type
	//# $kPropertyName$, and delete it. Do not call $SetNodeName$ with an empty string or $nullptr$.
	//
	//# \also	$@Node::GetNodeName@$
	//# \also	$@Node::GetProperty@$


	//# \div
	//# \function	Node::Clone		Clones a node hierarchy.
	//
	//# \proto	Node *Clone(void) const;
	//
	//# \desc
	//# The $Clone$ function duplicates a node hierarchy rooted at the node for which this function is called and
	//# returns the root of the duplicate node tree. The objects referenced by the nodes in the tree are not duplicated,
	//# but are referenced by the duplicate nodes.


	//# \div
	//# \function	Node::Preprocess		Performs any preprocessing that a node needs to do before being used in a world.
	//
	//# \proto	virtual void Preprocess(void);
	//
	//# \desc
	//# The $Preprocess$ function performs any preprocessing that a node needs to do before being used in a world.
	//# Whenever a node is added to a scene, it should subsequently be preprocessed. Calling the $@Node::AppendNewSubnode@$
	//# function to add a node to a scene is equivalent to calling $@Utilities/Tree::AppendSubnode@$ and following it with
	//# a call to $Preprocess$.
	//#
	//# When the $Preprocess$ function is called for a node, it internally causes all of the subnodes of that node to be
	//# preprocessed as well. If a node has a controller, then the $@Controller/Controller::Preprocess@$ function is called
	//# before the subnodes are preprocessed.
	//#
	//# The effects of calling the $Preprocess$ function can be reversed by calling the $@Node::Neutralize@$ function.
	//# It is not necessary to call the $Neutralize$ function before deleting a node, but it should be called for any
	//# node that will be removed from the scene without being deleted.
	//#
	//# Whenever a subclass implements an override for the $Preprocess$ function, it should always call the
	//# $Preprocess$ function of its direct base class first.
	//
	//# \also	$@Node::Neutralize@$
	//# \also	$@Node::AppendNewSubnode@$
	//# \also	$@World::AddNewNode@$


	//# \function	Node::Neutralize		Returns a node to the state it was in before being preprocessed.
	//
	//# \proto	virtual void Neutralize(void);
	//
	//# \desc
	//# The $Neutralize$ function returns a node to the state it was in before the $@Node::Preprocess@$ function was
	//# called for it. Whenever a node is removed from a scene, it should first have its $Neutralize$ function called.
	//#
	//# When the $Neutralize$ function is called for a node, it internally causes all of the subnodes of that node to be
	//# neutralized as well, in reverse order compared to the $Preprocess$ function.
	//#
	//# Whenever a subclass implements an override for the $Neutralize$ function, it should always call the
	//# $Neutralize$ function of its direct base class last.
	//
	//# \also	$@Node::Preprocess@$


	class Node : public NodeTree, public Transformable, public Site, public LinkTarget<Node>, public Packable, public Configurable, public Creatable<Node>
	{
		friend class Hub;

		public:

			typedef Array<Zone *, 4>	ZoneMembershipArray;
			typedef bool				CloneFilterProc(const Node *, void *);

			enum
			{
				kUpdateTransform		= 1 << 0,
				kUpdatePostprocess		= 1 << 1,
				kUpdateVisibility		= 1 << 2
			};

		private:

			typedef bool VisibilityProc(const Node *, const VisibilityRegion *);
			typedef bool OcclusionProc(const Node *, const VisibilityRegion *, const List<OcclusionRegion> *);

			struct ConnectorCloneData
			{
				Connector		*connector;
				int32			linkIndex;
			};

			NodeType				nodeType;
			unsigned_int32			nodeFlags;
			unsigned_int32			nodeHash;

			unsigned_int32			perspectiveExclusionMask;

			World					*nodeWorld;
			Manipulator				*nodeManipulator;
			Controller				*nodeController;
			Object					*nodeObject;
			Hub						*nodeHub;

			Map<Property>			propertyMap;
			PropertyObject			*propertyObject;

			Transform4D				nodeTransform;
			Transform4D				previousWorldTransform;

			BoundingSphere			worldBoundingSphere;

			VisibilityProc			*visibilityProc;
			OcclusionProc			*occlusionProc;

			int32					maxSubzoneDepth;
			int32					forcedSubzoneDepth;

			ZoneMembershipArray		zoneMembershipArray;

			union
			{
				mutable int32		nodeIndex;
				mutable int32		superIndex;
			};

			int32					nodeObjectIndex;

			Node *CloneNode(CloneFilterProc *filterProc = &DefaultCloneFilter, void *filterCookie = nullptr) const;
			Node *CloneNode(const Node *root, Node **nodeTable, Array<ConnectorCloneData, 16> *connectorArray, CloneFilterProc *filterProc = &DefaultCloneFilter, void *filterCookie = nullptr) const;

			static void ConnectorLinkProc(Node *node, void *cookie);
			static void PropertyObjectLinkProc(Object *object, void *cookie);

			static Object **LoadOriginalObjects(const ResourceName& name, World *previousWorld, int32 newObjectCount, int32 *originalObjectCount, int32 *totalObjectCount);
			static Node *LoadNodeTable(Unpacker& unpacker, unsigned_int32 unpackFlags, int32 nodeCount, int32 objectCount, Object **objectTable);

		protected:

			C4API Node(const Node& node);

			void SetNewObject(Object *object)
			{
				nodeObject = object;
			}

			void SetBoundingSphere(const Point3D& center, float radius)
			{
				worldBoundingSphere.SetCenter(center);
				worldBoundingSphere.SetRadius(radius);
			}

			Setting *GetPerspectiveMaskSetting(int32 index) const;
			bool SetPerspectiveMaskSetting(const Setting *setting);

			void PurgeVisibility(void)
			{
				PurgeIncomingEdges();
				zoneMembershipArray.Purge();
			}

			void InitializeVisibility(void);

			C4API virtual void HandleTransformUpdate(void);
			C4API virtual void HandlePostprocessUpdate(void);
			C4API virtual void HandleVisibilityUpdate(void);

		public:

			C4API Node(NodeType type = kNodeGeneric);
			C4API virtual ~Node();

			using NodeTree::Previous;
			using NodeTree::Next;
			using NodeTree::GetPreviousNode;
			using NodeTree::GetNextNode;
			using NodeTree::Detach;

			static Node *Create(Unpacker& data, unsigned_int32 unpackFlags);

			NodeType GetNodeType(void) const
			{
				return (nodeType);
			}

			unsigned_int32 GetNodeFlags(void) const
			{
				return (nodeFlags);
			}

			void SetNodeFlags(unsigned_int32 flags)
			{
				nodeFlags = flags;
			}

			bool Enabled(void) const
			{
				return ((nodeFlags & kNodeDisabled) == 0);
			}

			unsigned_int32 GetNodeHash(void) const
			{
				return (nodeHash);
			}

			unsigned_int32 GetPerspectiveExclusionMask(void) const
			{
				return (perspectiveExclusionMask);
			}

			void SetPerspectiveExclusionMask(unsigned_int32 mask)
			{
				perspectiveExclusionMask = mask;
			}

			World *GetWorld(void) const
			{
				return (nodeWorld);
			}

			void SetWorld(World *world)
			{
				nodeWorld = world;
			}

			void AppendNewSubnode(Node *node)
			{
				AppendSubnode(node);
				node->Preprocess();
			}

			void PrependNewSubnode(Node *node)
			{
				PrependSubnode(node);
				node->Preprocess();
			}

			Manipulator *GetManipulator(void) const
			{
				return (nodeManipulator);
			}

			void SetManipulator(Manipulator *manipulator)
			{
				nodeManipulator = manipulator;
			}

			Controller *GetController(void) const
			{
				return (nodeController);
			}

			Object *GetObject(void) const
			{
				return (nodeObject);
			}

			Hub *GetHub(void) const
			{
				return (nodeHub);
			}

			int32 GetPropertyCount(void) const
			{
				return (propertyMap.GetElementCount());
			}

			Property *GetProperty(PropertyType type) const
			{
				return (propertyMap.Find(type));
			}

			Property *GetFirstProperty(void) const
			{
				return (propertyMap.First());
			}

			PropertyObject *GetPropertyObject(void) const
			{
				return (propertyObject);
			}

			Property *GetSharedProperty(PropertyType type) const
			{
				return ((propertyObject) ? propertyObject->GetProperty(type) : nullptr);
			}

			const Transform4D& GetNodeTransform(void) const
			{
				return (nodeTransform);
			}

			void SetNodeTransform(const Transform4D& transform)
			{
				nodeTransform = transform;
			}

			void SetNodeTransform(const Matrix3D& matrix, const Point3D& position)
			{
				nodeTransform.Set(matrix, position);
			}

			void SetNodeTransform(const Vector3D& c1, const Vector3D& c2, const Vector3D& c3, const Point3D& c4)
			{
				nodeTransform.Set(c1, c2, c3, c4);
			}

			void SetNodeMatrix3D(const Matrix3D& matrix)
			{
				nodeTransform.SetMatrix3D(matrix);
			}

			void SetNodeMatrix3D(const Transform4D& transform)
			{
				nodeTransform.SetMatrix3D(transform);
			}

			void SetNodeMatrix3D(const Vector3D& c1, const Vector3D& c2, const Vector3D& c3)
			{
				nodeTransform.SetMatrix3D(c1, c2, c3);
			}

			const Point3D& GetNodePosition(void) const
			{
				return (nodeTransform.GetTranslation());
			}

			void SetNodePosition(const Point3D& position)
			{
				nodeTransform.SetTranslation(position);
			}

			const Transform4D& GetPreviousWorldTransform(void) const
			{
				return (previousWorldTransform);
			}

			void SetPreviousWorldTransform(const Transform4D& transform)
			{
				previousWorldTransform = transform;
			}

			const BoundingSphere *GetBoundingSphere(void) const
			{
				return (&worldBoundingSphere);
			}

			void SetVisibilityProc(VisibilityProc *proc)
			{
				visibilityProc = proc;
			}

			void SetOcclusionProc(OcclusionProc *proc)
			{
				occlusionProc = proc;
			}

			bool Visible(const VisibilityRegion *region) const
			{
				return ((*visibilityProc)(this, region));
			}

			bool Visible(const VisibilityRegion *region, const List<OcclusionRegion> *occlusionList) const
			{
				return ((*occlusionProc)(this, region, occlusionList));
			}

			int32 GetMaxSubzoneDepth(void) const
			{
				return (maxSubzoneDepth);
			}

			void SetMaxSubzoneDepth(int32 depth)
			{
				maxSubzoneDepth = depth;
			}

			int32 GetForcedSubzoneDepth(void) const
			{
				return (forcedSubzoneDepth);
			}

			void SetForcedSubzoneDepth(int32 depth)
			{
				forcedSubzoneDepth = depth;
			}

			ZoneMembershipArray& GetZoneMembershipArray(void)
			{
				return (zoneMembershipArray);
			}

			const ZoneMembershipArray& GetZoneMembershipArray(void) const
			{
				return (zoneMembershipArray);
			}

			int32 GetNodeIndex(void) const
			{
				return (nodeIndex);
			}

			void InvalidateNodeIndex(void)
			{
				nodeIndex = -1;
			}

			C4API virtual Node *Replicate(void) const;

			C4API static bool DefaultCloneFilter(const Node *node, void *cookie = nullptr);
			C4API Node *Clone(CloneFilterProc *filterProc = &DefaultCloneFilter, void *filterCookie = nullptr) const;
			C4API void CloneSubtree(Node *root) const;

			C4API bool LinkedNodePackable(unsigned_int32 packFlags) const;

			C4API void PackType(Packer& data) const override;
			C4API void Prepack(List<Object> *linkList) const override;
			C4API void Pack(Packer& data, unsigned_int32 packFlags) const override;
			C4API void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			C4API void *BeginSettingsUnpack(void) override;
			C4API void EndSettingsUnpack(void *cookie) override;

			C4API int32 GetCategoryCount(void) const override;
			C4API Type GetCategoryType(int32 index, const char **title) const override;
			C4API int32 GetCategorySettingCount(Type category) const override;
			C4API Setting *GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const override;
			C4API void SetCategorySetting(Type category, const Setting *setting) override;

			C4API void Invalidate(void) override;
			C4API void Update(void) override;

			C4API void UpdateTransform(void);
			C4API void UpdatePostprocess(void);
			C4API void UpdateVisibility(void);

			C4API void StopMotion(void);

			C4API virtual void Enable(void);
			C4API virtual void Disable(void);

			C4API void SetPersistent(void);
			C4API void SetNonpersistent(void);

			C4API void SetObject(Object *object);
			C4API void SetController(Controller *controller);

			C4API bool AddProperty(Property *property);
			C4API void SetPropertyObject(PropertyObject *object);

			C4API const char *GetNodeName(void) const;
			C4API void SetNodeName(const char *name);

			C4API Connector *GetConnector(const char *key) const;
			C4API Node *GetConnectedNode(const char *key) const;
			C4API void SetConnectedNode(const char *key, Node *node);
			C4API void AddConnector(const char *key, Node *node = nullptr);
			C4API bool RemoveConnector(const char *key);

			C4API virtual int32 GetInternalConnectorCount(void) const;
			C4API virtual const char *GetInternalConnectorKey(int32 index) const;
			C4API virtual void ProcessInternalConnectors(void);
			C4API virtual bool ValidConnectedNode(const ConnectorKey& key, const Node *node) const;

			C4API Zone *GetOwningZone(void) const;

			C4API void EstablishVisibility(void);
			C4API void DismantleVisibility(void);

			C4API virtual void Preload(void);
			C4API virtual void Preinitialize(void);
			C4API virtual void Preprocess(void);
			C4API virtual void Neutralize(void);

			C4API virtual void ProcessObjectSettings(void);

			C4API virtual bool CalculateBoundingBox(Box3D *box) const;
			C4API virtual bool CalculateBoundingSphere(BoundingSphere *sphere) const;

			C4API static bool AlwaysVisible(const Node *node, const VisibilityRegion *region);
			C4API static bool AlwaysVisible(const Node *node, const VisibilityRegion *region, const List<OcclusionRegion> *occlusionList);

			C4API static bool BoxVisible(const Node *node, const VisibilityRegion *region);
			C4API static bool BoxVisible(const Node *node, const VisibilityRegion *region, const List<OcclusionRegion> *occlusionList);
			C4API static bool SphereVisible(const Node *node, const VisibilityRegion *region);
			C4API static bool SphereVisible(const Node *node, const VisibilityRegion *region, const List<OcclusionRegion> *occlusionList);

			C4API void PrepackNodeObjects(List<Object> *linkList) const;
			C4API FileResult PackTree(File *file, unsigned_int32 packFlags = 0) const;
			C4API void PackTree(Package *package, unsigned_int32 packFlags = 0) const;
			C4API static Node *UnpackTree(const void *data, unsigned_int32 unpackFlags = 0);

			C4API FileResult PackDeltaTree(File *file, const ResourceName& originalName) const;
			C4API static Node *UnpackDeltaTree(const void *data, ResourceName& originalName, World *previousWorld = nullptr);
	};


	//# \class	RenderableNode		The base class for renderable scene graph nodes.
	//
	//# Every directly-renderable node in a scene graph is a subclass of the $RenderableNode$ class.
	//
	//# \def	class RenderableNode : public Node, public Renderable
	//
	//# \ctor	RenderableNode(NodeType type, RenderType renderType, unsigned_int32 renderState = 0);
	//
	//# \param	type			The node type passed to the $Node$ base class.
	//# \param	renderType		The render type for the $Renderable$ base class.
	//# \param	renderState		The render state for the $Renderable$ base class.
	//
	//# \desc
	//# The $RenderableNode$ class serves as the base class for scene graph nodes that can be directly rendered,
	//# such as geometries and effects.
	//
	//# \base	Node						A $RenderableNode$ is a special type of node.
	//# \base	GraphicsMgr/Renderable		Holds rendering information for the node.
	//
	//# \also	$@Geometry@$
	//# \also	$@EffectMgr/Effect@$


	//# \function	RenderableNode::SetShaderParameterProc		Sets the shader parameter procedure.
	//
	//# \proto	void SetShaderParameterProc(ShaderParameterProc *proc, void *cookie = nullptr);
	//
	//# \param	proc		A pointer to the shader parameter procedure.
	//# \param	cookie		The cookie that is passed to the callback procedure as its last parameter.
	//
	//# \desc
	//# The $SetShaderParameterProc$ function installs a callback procedure on a renderable node that provides the
	//# location of mutable shader parameters. Each time a new shader is compiled for the node, the callback procedure
	//# is invoked for each mutable parameter, and the callback procedure may return a pointer to an array of four
	//# floating-point numbers where the value of the parameter is stored. The $ShaderParameterProc$ type is defined as follows.
	//
	//# \code	typedef const float *ShaderParameterProc(int32 slot, void *cookie);
	//
	//# The $slot$ parameter passed to the callback procedure specifies the slot number used by one of the scalar,
	//# vector, or color processes in the shader, and the $cookie$ parameter is the value previously passed to the
	//# $SetShaderParameterProc$ function. If the callback procedure returns a pointer to a parameter, then the
	//# four floating-point values must remain allocated for the lifetime of the node or until the
	//# $@GraphicsMgr/Renderable::InvalidateShaderData@$ function is called. If the callback procedure returns
	//# $nullptr$, then the parameter value is taken from the material object containing the shader.
	//#
	//# The parameter values at the location returned by the callback procedure may be changed at any time,
	//# and the new values will automatically be used the next time the node is rendered. It is not necessary to
	//# perform any updates or invalidation.


	class RenderableNode : public Node, public Renderable
	{
		public:

			typedef const float *ShaderParameterProc(int32 slot, void *cookie);

		private:

			ShaderParameterProc		*shaderParameterProc;
			void					*shaderParameterCookie;

		protected:

			RenderableNode(NodeType type, RenderType renderType, unsigned_int32 renderState = 0);
			RenderableNode(const RenderableNode& renderableNode);

			C4API void HandlePostprocessUpdate(void) override;

			void SelectAmbientEnvironment(const Node *node);

		public:

			~RenderableNode();

			void SetShaderParameterProc(ShaderParameterProc *proc, void *cookie = nullptr)
			{
				shaderParameterProc = proc;
				shaderParameterCookie = cookie;
			}

			void Neutralize(void) override;

			C4API const float *GetShaderParameterPointer(int32 slot) const override;
	};
}


#endif

// ZYUQURM
