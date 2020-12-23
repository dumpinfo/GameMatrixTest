 

#ifndef C4World_h
#define C4World_h


//# \component	World Manager
//# \prefix		WorldMgr/

//# \import		C4Physics.h


#include "C4Display.h"
#include "C4Graphics.h"
#include "C4Impostors.h"
#include "C4Zones.h"


namespace C4
{
	typedef EngineResult	WorldResult;


	enum : WorldResult
	{
		kWorldOkay			= kEngineOkay,
		kWorldLoadFailed	= (kManagerWorld << 16) | 0x0001
	};


	enum
	{
		kWorldPaused					= 1 << 0,
		kWorldViewport					= 1 << 1,
		kWorldClearColor				= 1 << 2,
		kWorldAmbientOnly				= 1 << 3,
		kWorldMotionBlurInhibit			= 1 << 4,
		kWorldZeroBackgroundVelocity	= 1 << 5,
		kWorldPostColorMatrix			= 1 << 6,
		kWorldListenerInhibit			= 1 << 7,
		kWorldOculusCamera				= 1 << 8,
		kWorldRestore					= 1 << 9,
		kWorldWarmup					= 1 << 10
	};


	enum
	{
		kRenderStageAmbientDefault,
		kRenderStageAmbientAlphaTest,
		kRenderStageLightDefault,
		kRenderStageLightAlphaTest,
		kRenderStageCover,
		kRenderStageDecal,
		kRenderStageEffectLight,
		kRenderStageEffectOpaque,
		kRenderStageEffectTransparent,
		kRenderStageEffectFrontmost,
		kRenderStageEffectOcclusion,
		kRenderStageEffectDistortion,
		kRenderStageEffectVelocity,
		kRenderStageEffectCover,
		kRenderStageCount,

		kRenderStageFirstEffect = kRenderStageEffectLight,
		kRenderStageLastEffect = kRenderStageEffectCover
	};


	enum
	{
		kRenderStageShadowDefault,
		kRenderStageShadowAlphaTest = kRenderStageShadowDefault + kMaxShadowCascadeCount,
		kRenderStageShadowCount = kRenderStageShadowAlphaTest + kMaxShadowCascadeCount
	};


	//# \enum	CollisionState

	enum
	{
		kCollisionStateNone			= 0,		//## No collision occurred.
		kCollisionStateGeometry		= 1,		//## Collision occurred with world geometry.
		kCollisionStateRigidBody	= 2			//## Collision occurred with a rigid body.
	};


	//# \enum	ProximityResult

	enum
	{
		kProximityContinue			= 0,		//## Continue visting nodes normally.
		kProximitySkipSuccessors	= 1,		//## Do not visit any successors of the current node.
		kProximityStop				= 2			//## Stop the proximity query at the current node.
	};


	enum
	{
		kWorldMaxInstanceDepth		= 3
	};

 
	#if C4STATS

		enum 
		{
			kWorldCounterGeometry, 
			kWorldCounterTerrain,
			kWorldCounterWater,
			kWorldCounterImpostor, 

			kWorldCounterInfiniteLight, 
			kWorldCounterPointLight, 
			kWorldCounterSpotLight,
			kWorldCounterInfiniteShadow,
			kWorldCounterInfiniteShadowCascade,
			kWorldCounterInfiniteShadowGeometry, 
			kWorldCounterPointShadow,
			kWorldCounterPointShadowFace,
			kWorldCounterPointShadowGeometry,
			kWorldCounterSpotShadow,
			kWorldCounterSpotShadowGeometry,

			kWorldCounterDirectPortal,
			kWorldCounterRemotePortal,
			kWorldCounterOcclusionRegion,
			kWorldCounterRenderCount,

			kWorldCounterPlayingSource = kWorldCounterRenderCount,
			kWorldCounterEngagedSource,
			kWorldCounterRunningScript,
			kWorldCounterWaterMove,
			kWorldCounterWaterUpdate,
			kWorldCounterCount
		};

	#endif


	#if C4DIAGS

		enum
		{
			kDiagnosticLightRegions		= 1 << 0,
			kDiagnosticShadowRegions	= 1 << 1,
			kDiagnosticSourcePaths		= 1 << 2,
			kDiagnosticRigidBodies		= 1 << 3,
			kDiagnosticContacts			= 1 << 4
		};

	#endif


	class Skybox;
	class Trigger;
	class OrthoCamera;
	class FrustumCamera;
	class TerrainGeometry;
	class TerrainLevelGeometry;
	class PanelEffect;
	class PortalData;
	class CollisionThreadData;
	class QueryThreadData;
	class InteractionThreadData;
	struct WorldContext;
	struct CollisionParams;
	struct ProximityParams;
	struct InteractionData;


	class WorldResource : public Resource<WorldResource>
	{
		friend class Resource<WorldResource>;

		private:

			static C4API ResourceDescriptor		descriptor;

			~WorldResource();

		public:

			C4API WorldResource(const char *name, ResourceCatalog *catalog);

			int32 GetControllerCount(void) const
			{
				return (static_cast<const int32 *>(GetData())[2]);
			}

			ResourceResult LoadObjectOffsetTable(ResourceLoader *loader, WorldHeader *worldHeader, int32 **offsetTable) const;
			ResourceResult LoadAllObjects(ResourceLoader *loader, const WorldHeader *header, const int32 *offsetTable, char **objectData) const;
			ResourceResult LoadObject(ResourceLoader *loader, int32 index, const int32 *offsetTable, char **objectData) const;
	};


	class SaveResource : public Resource<SaveResource>
	{
		friend class Resource<SaveResource>;

		private:

			static C4API ResourceDescriptor		descriptor;

			~SaveResource();

		public:

			C4API SaveResource(const char *name, ResourceCatalog *catalog);

			int32 GetControllerCount(void) const
			{
				return (static_cast<const int32 *>(GetData())[2]);
			}
	};


	class WorldObservable : public Observable<WorldObservable>
	{
		private:

			World		*observableWorld;

		public:

			WorldObservable(World *world)
			{
				observableWorld = world;
			}

			World *GetWorld(void) const
			{
				return (observableWorld);
			}
	};


	class InstancedWorldData : public MapElement<InstancedWorldData>
	{
		private:

			unsigned_int32	worldHash;
			Node			*prototypeCopy;

		public:

			typedef unsigned_int32 KeyType;

			InstancedWorldData(unsigned_int32 hash, Node *node);
			~InstancedWorldData();

			KeyType GetKey(void) const
			{
				return (worldHash);
			}

			Node *GetPrototypeCopy(void) const
			{
				return (prototypeCopy);
			}
	};


	class GenericModelData : public MapElement<GenericModelData>
	{
		private:

			unsigned_int32			modelHash;
			List<GenericModel>		modelList;

		public:

			typedef unsigned_int32 KeyType;

			GenericModelData(unsigned_int32 hash, GenericModel *model);
			~GenericModelData();

			KeyType GetKey(void) const
			{
				return (modelHash);
			}

			GenericModel *GetGenericModel(void) const
			{
				return (modelList.First());
			}

			void AddGenericModel(GenericModel *model)
			{
				modelList.Append(model);
			}
	};


	//# \class	Interactor		Handles interaction events.
	//
	//# The $Interactor$ class handles interaction events.
	//
	//# \def	class Interactor : public ListElement<Interactor>
	//
	//# \ctor	Interactor();
	//
	//# \desc
	//#
	//
	//# \base	Utilities/ListElement<Interactor>	Used internally by the World Manager.
	//
	//# \also	$@World::AddInteractor@$
	//# \also	$@World::RemoveInteractor@$
	//# \also	$@World::Interact@$


	//# \function	Interactor::GetInteractionNode		Returns the node currently engaged in interaction.
	//
	//# \proto	Node *GetInteractionNode(void) const;
	//
	//# \desc
	//# The $GetInteractionNode$ function returns a pointer to the node that is currently engaged in interaction
	//# by a particular instance of the $@Interactor@$ class. If no node is currently engaged in interaction,
	//# then this function returns $nullptr$.
	//
	//# \also	$@Interactor::SetInteractionProbe@$
	//# \also	$@Interactor::HandleInteractionEvent@$


	//# \function	Interactor::SetInteractionProbe		Sets the line segment representing the interaction probe.
	//
	//# \proto	void SetInteractionProbe(const Point3D& p1, const Point3D& p2);
	//
	//# \param	p1		The beginning of the line segment.
	//# \param	p2		The end of the line segment.
	//
	//# \desc
	//# The $SetInteractionProbe$ sets the world-space endpoint coordinates of the probe used to test for
	//# interactive objects. This function is typically called from within a function overriding the
	//# $@World::Interact@$ function before the base class $Interact$ function is called.
	//#
	//# When the interaction probe intersects interactive objects in a world, the $@Interactor::HandleInteractionEvent@$
	//# function is called to handle various interaction events.
	//
	//# \also	$@Interactor::GetInteractionNode@$
	//# \also	$@Interactor::HandleInteractionEvent@$


	//# \function	Interactor::HandleInteractionEvent		Called to handle an interaction event.
	//
	//# \proto	virtual void HandleInteractionEvent(InteractionEventType type, Node *node, const Point3D *position = nullptr);
	//
	//# \param	type		The type of event. See the $@Controller/Controller::HandleInteractionEvent@$ for a list of possible types.
	//# \param	node		The interactive node to which the event pertains.
	//# \param	position	The object-space position on the interactive node at which the event took place. If the $type$
	//#						parameter is $kInteractionEventDisengage$, then this parameter is $nullptr$.
	//
	//# \desc
	//# The $HandleInteractionEvent$ function is called by the World Manager when an interaction event takes place
	//# for a particular instance of the $@Interactor@$ class. If this function is overridden, it must call the base
	//# class counterpart first. An overriding function will typical cause the $@Controller/Controller::HandleInteractionEvent@$
	//# function to be called for the controller attached to the node specified by the $node$ parameter.
	//
	//# The $HandleInteractionEvent$ function only receives the $kInteractionEventEngage$, $kInteractionEventDisengage$,
	//# and $kInteractionEventTrack$ events. The remaining two event types, $kInteractionEventActivate$ and
	//# $kInteractionEventDeactivate$, should be sent directly to a controller when the user explicitly provides
	//# input with the intent to interact with an object.
	//
	//# It can be determined whether an $Interactor$ instance is currently engaged with an interactive object by calling
	//# the $@Interactor::GetInteractionNode@$ function.
	//
	//# \also	$@Interactor::SetInteractionProbe@$
	//# \also	$@Interactor::GetInteractionNode@$
	//# \also	$@Controller/Controller::HandleInteractionEvent@$


	class Interactor : public ListElement<Interactor>
	{
		private:

			Link<Node>	interactionNode;
			Point3D		interactionPosition;

			Point3D		interactionPoint[2];

		public:

			C4API Interactor();
			C4API virtual ~Interactor();

			Node *GetInteractionNode(void) const
			{
				return (interactionNode);
			}

			void SetInteractionNode(Node *node)
			{
				interactionNode = node;
			}

			const Point3D& GetInteractionPosition(void) const
			{
				return (interactionPosition);
			}

			C4API void SetInteractionProbe(const Point3D& p1, const Point3D& p2);
			C4API virtual void HandleInteractionEvent(InteractionEventType type, Node *node, const Point3D *position = nullptr);

			void DetectInteraction(const World *world);
	};


	//# \class	World	Encapsulates a complete world.
	//
	//# The $World$ class encapsulates a complete world.
	//
	//# \def	class World : public LinkTarget<World>
	//
	//# \ctor	World(const char *name, unsigned_int32 flags = 0);
	//# \ctor	World(Node *root, unsigned_int32 flags = 0);
	//
	//# \param	name	The name of a world resource.
	//# \param	root	A pointer to the root node of the world's transform tree.
	//# \param	flags	The initial world flags.
	//
	//# \desc
	//#
	//
	//# \base	Utilities/LinkTarget<World>		Used internally by the World Manager.
	//
	//# \also	$@Node@$
	//# \also	$@WorldMgr@$


	//# \function	World::GetWorldFlags		Returns the world flags.
	//
	//# \proto	unsigned_int32 GetWorldFlags(void) const;
	//
	//# \desc
	//
	//# \also	$@World::SetWorldFlags@$


	//# \function	World::SetWorldFlags		Sets the world flags.
	//
	//# \proto	void SetWorldFlags(unsigned_int32 flags);
	//
	//# \param	flags	The new world flags.
	//
	//# \desc
	//
	//# \also	$@World::GetWorldFlags@$


	//# \function	World::SetFinalColorTransform		Sets the color transform used in post-processing.
	//
	//# \proto	void SetFinalColorTransform(const ColorRGBA& scale, const ColorRGBA& bias);
	//# \proto	void SetFinalColorTransform(const ColorRGBA& red, const ColorRGBA& green, const ColorRGBA& blue, const ColorRGBA& bias);
	//
	//# \param	scale	The componentwise scale color.
	//# \param	bias	The componentwise bias color.
	//# \param	red		The color matrix row for the red channel.
	//# \param	green	The color matrix row for the green channel.
	//# \param	blue	The color matrix row for the blue channel.
	//
	//# \desc
	//# The $SetFinalColorTransform$ sets the color transform that is used in the final stage of
	//# post-processing when a world is rendered. If the $scale$ parameter is specified, then the red,
	//# green, and blue components of the final color are multiplied by the corresponding components of the
	//# $scale$ color. If the $red$, $green$, and $blue$ parameters are specified, then they serve as rows of
	//# a color matrix that transform the final color. The alpha channels are not included in the calculation.
	//# In both cases, the $bias$ parameter specifies a color that is added to the result of the scale or matrix transform.
	//#
	//# The channels of each color passed to the $SetFinalColorTransform$ function may be any floating-point values.
	//# By default, the post-processing color transform uses a scale color of (1,1,1,1) and a bias color of (0,0,0,0).


	//# \function	World::GetRootNode		Returns the root node of a world.
	//
	//# \proto	Zone *GetRootNode(void) const;
	//
	//# \desc
	//# The $GetRootNode$ function returns a pointer to the root node of a world. Since the root node
	//# of a world is always a zone, the return value is a pointer to a $@Zone@$ node.
	//
	//# \also	$@Zone@$


	//# \function	World::AddNewNode		Adds a new node to the world and preprocesses it.
	//
	//# \proto	void AddNewNode(Node *node);
	//
	//# \param	node	The node to add to the world.
	//
	//# \desc
	//# The $AddNewNode$ function adds the node specified by the $node$ parameter to the root zone of the
	//# world and then calls its $@Node::Preprocess@$ function.
	//
	//# \also	$@Node@$
	//# \also	$@Node::Preprocess@$


	//# \function	World::GetCamera		Returns the current camera.
	//
	//# \proto	FrustumCamera *GetCamera(void) const;
	//
	//# \desc
	//
	//# \also	$@World::SetCamera@$
	//# \also	$@World::Render@$


	//# \function	World::SetCamera		Sets the current camera.
	//
	//# \proto	void SetCamera(FrustumCamera *camera);
	//
	//# \param	camera		A pointer to the camera.
	//
	//# \desc
	//
	//# \also	$@World::GetCamera@$
	//# \also	$@World::Render@$


	//# \function	World::GetController		Returns the controller having a specific controller index.
	//
	//# \proto	Controller *GetController(int32 index) const;
	//
	//# \param	index	The index of the controller.
	//
	//# \desc
	//# The $GetController$ function returns a pointer to the controller having the index specified by the $index$
	//# parameter. If no such controller exists, then the return value is $nullptr$.
	//
	//# \also	$@Controller/Controller@$


	//# \div
	//# \function	World::AddInteractor	Adds an interactor to a world.
	//
	//# \proto	void AddInteractor(Interactor *interactor);
	//
	//# \param	interactor		The interactor to add to the world.
	//
	//# \desc
	//# The $AddInteractor$ function adds an interactor to a world. Each interactor has an interaction probe
	//# that is tested against interactive nodes in a world when the $@World::Interact@$ function is called
	//# by the World Manager.
	//
	//# \also	$@World::RemoveInteractor@$
	//# \also	$@World::Interact@$
	//# \also	$@Interactor@$


	//# \function	World::RemoveInteractor		Removes an interactor from a world.
	//
	//# \proto	void RemoveInteractor(Interactor *interactor);
	//
	//# \param	interactor		The interactor to remove from the world.
	//
	//# \desc
	//# The $RemoveInteractor$ function removes an interactor from a world so that it is no longer tested against
	//# interactive nodes.
	//
	//# \also	$@World::AddInteractor@$
	//# \also	$@World::Interact@$
	//# \also	$@Interactor@$


	//# \function	World::Interact		Tests for interactions in a world.
	//
	//# \proto	virtual void Interact(void);
	//
	//# \desc
	//# The $Interact$ function is called each frame after all movement has completed and before any rendering
	//# takes place. It tests all of the active interaction probes and dispatches passive interaction events.
	//# This function can be overridden in order to set interaction probes before the base class counterpart is called.
	//
	//# \also	$@World::AddInteractor@$
	//# \also	$@World::RemoveInteractor@$
	//# \also	$@Interactor@$


	//# \div
	//# \function	World::SetRenderSize		Sets the size of the viewport into which the world is rendered.
	//
	//# \proto	void SetRenderSize(int32 width, int32 height);
	//
	//# \param	width		The width of the viewport.
	//# \param	height		The height of the viewport.
	//
	//# \desc
	//
	//# \also	$@World::Render@$


	//# \function	World::Render		Renders a world.
	//
	//# \proto	virtual void Render(void);
	//
	//# \desc
	//
	//# \also	$@World::SetCamera@$
	//# \also	$@World::SetRenderSize@$


	//# \div
	//# \function	World::DetectCollision		Detects a collision between world geometry and a swept sphere.
	//
	//# \proto	bool DetectCollision(const Point3D& p1, const Point3D& p2, float radius, unsigned_int32 kind,
	//# \proto2	CollisionData *collisionData, int32 threadIndex = JobMgr::kMaxWorkerThreadCount) const;
	//
	//# \param	p1				The beginning of the line segment in world space.
	//# \param	p2				The end of the line segment in world space.
	//# \param	radius			The radius of the sphere. This cannot be negative, but it can be zero.
	//# \param	kind			The collision kind.
	//# \param	collisionData	The returned collision data.
	//# \param	threadIndex		The index of the Job Manager worker thread that is calling this function.
	//
	//# \desc
	//# The points specified by the parameters $p1$ and $p2$, combined with the radius specified by the $radius$
	//# parameter, define a directed swept sphere. The $DetectCollision$ function detects the first collision between
	//# this swept sphere and all enabled $@Geometry@$ nodes possessing collision information. If a collision is detected,
	//# then the function returns $true$; otherwise, it returns $false$.
	//#
	//# The $kind$ parameter can be used to invalidate certain types of collisions. When a candidate geometry is
	//# encountered in the collision detection process, its collision exclusion mask is logically ANDed with the
	//# value of the $kind$ parameter. A collision can only occur if the result of this operation is zero. The collision
	//# mask associated with a geometry can be set using the $@GeometryObject::SetCollisionExclusionMask@$ function.
	//# The collision kind can be a combination (through logical OR) of the following predefined values and application-defined values.
	//
	//# \table	CollisionKind
	//
	//# If a collision occurs, then the $@CollisionData@$ structure pointed to by the $collisionData$ parameter is
	//# filled out with information about the collision. The $param$ field of this data structure represents the
	//# fraction of the distance that the sphere traveled from $p1$ to $p2$ before the collision occurred.
	//#
	//# The $threadIndex$ parameter specifies the index of the Job Manager worker thread that has called the $DetectCollision$
	//# function. If the $DetectCollision$ function is called from the main thread, then this parameter should not be
	//# specified so that the default value is used. If the $DetectCollision$ function is called from a job, then the
	//# $threadIndex$ parameter should be set to the value returned by the $@System/Job::GetThreadIndex@$ function.
	//# The $threadIndex$ parameter must be set correctly in order for multithreaded collision detection to work properly.
	//
	//# \important
	//# If the $DetectCollision$ function is called from inside a Job Manager worker thread, then the application code must
	//# ensure that the scene is not modified while the job is running.
	//
	//# \desc
	//# The $DetectCollision$ function works by intersecting a line segment with the Minkowski sum of a sphere and
	//# arbitrary polygon meshes. The algorithm is very precise and can determine when collisions occur with the
	//# expanded faces, edges, or vertices of the mesh referenced by a geometry node. If the value of the $radius$
	//# parameter is 0.0, then the collision detection reduces to a ray intersection with faces only.
	//
	//# \also	$@CollisionData@$
	//# \also	$@World::QueryCollision@$
	//# \also	$@World::QueryProximity@$
	//# \also	$@GeometryObject::GetCollisionExclusionMask@$
	//# \also	$@GeometryObject::SetCollisionExclusionMask@$


	//# \function	World::QueryCollision		Detects whether a swept sphere collides with world geometry or rigid bodies.
	//
	//# \proto	CollisionState QueryCollision(const Point3D& p1, const Point3D& p2, float radius, unsigned_int32 kind,
	//# \proto2	CollisionData *collisionData, const RigidBodyController *excludedBody = nullptr, int32 threadIndex = JobMgr::kMaxWorkerThreadCount) const;
	//
	//# \param	p1				The beginning of the line segment in world space.
	//# \param	p2				The end of the line segment in world space.
	//# \param	radius			The radius of the sphere. This cannot be negative, but it can be zero.
	//# \param	kind			The collision kind.
	//# \param	collisionData	The returned collision data.
	//# \param	excludedBody	A rigid body that will be excluded from the query.
	//# \param	threadIndex		The index of the Job Manager worker thread that is calling this function.
	//
	//# \desc
	//# The points specified by the parameters $p1$ and $p2$, combined with the radius specified by the $radius$
	//# parameter, define a directed swept sphere. The $QueryCollision$ function detects the first collision between
	//# this swept sphere and all $@Geometry@$ nodes possessing collision information. Unlike the $@World::DetectCollision@$
	//# function, the $QueryCollision$ function also detects collisions with any $@PhysicsMgr/RigidBodyController@$ objects. If a
	//# collision is detected, then the function returns either $kCollisionStateGeometry$ or $kCollisionStateRigidBody$,
	//# depending on the type of collision; otherwise, it returns $kCollisionStateNone$.
	//#
	//# The $kind$ parameter can be used to invalidate certain types of collisions. When a candidate geometry or rigid body
	//# is encountered in the collision detection process, its collision exclusion mask is logically ANDed with the
	//# value of the $kind$ parameter. A collision can only occur if the result of this operation is zero. The collision
	//# mask associated with a geometry can be set using the $@GeometryObject::SetCollisionExclusionMask@$ function,
	//# and the collision mask for a rigid body can be set using the $@PhysicsMgr/RigidBodyController::SetCollisionExclusionMask@$ function.
	//# The collision kind can be a combination (through logical OR) of the following predefined values and application-defined values.
	//
	//# \table	CollisionKind
	//
	//# If a collision occurs, then the $@CollisionData@$ structure pointed to by the $collisionData$ parameter is
	//# filled out with information about the collision. The $param$ field of this data structure represents the
	//# fraction of the distance that the sphere traveled from $p1$ to $p2$ before the collision occurred.
	//#
	//# If the $excludedBody$ parameter is not $nullptr$, then any potential collisions with the rigid body is specifies are ignored.
	//#
	//# The $threadIndex$ parameter specifies the index of the Job Manager worker thread that has called the $QueryCollision$
	//# function. If the $QueryCollision$ function is called from the main thread, then this parameter should not be
	//# specified so that the default value is used. If the $QueryCollision$ function is called from a job, then the
	//# $threadIndex$ parameter should be set to the value returned by the $@System/Job::GetThreadIndex@$ function.
	//# The $threadIndex$ parameter must be set correctly in order for multithreaded collision queries to work properly.
	//
	//# \important
	//# If the $QueryCollision$ function is called from inside a Job Manager worker thread, then the application code must
	//# ensure that the scene is not modified while the job is running.
	//
	//# \also	$@CollisionData@$
	//# \also	$@World::DetectCollision@$
	//# \also	$@World::QueryProximity@$
	//# \also	$@GeometryObject::GetCollisionExclusionMask@$
	//# \also	$@GeometryObject::SetCollisionExclusionMask@$
	//# \also	$@PhysicsMgr/RigidBodyController::GetCollisionExclusionMask@$
	//# \also	$@PhysicsMgr/RigidBodyController::SetCollisionExclusionMask@$


	//# \function	World::QueryProximity		Enumerates the world geometry nodes and rigid bodies that intersect a sphere.
	//
	//# \proto	void QueryProximity(const Point3D& center, float radius, ProximityProc *proc, void *cookie, int32 threadIndex = JobMgr::kMaxWorkerThreadCount) const;
	//
	//# \param	center			The center of the sphere in world space.
	//# \param	radius			The radius of the sphere. This must be positive.
	//# \param	proc			A pointer to a function that is called for each node intersecting the sphere.
	//# \param	cookie			A user-defined pointer that is passed to the callback function specified by the $proc$ parameter.
	//# \param	threadIndex		The index of the Job Manager worker thread that is calling this function.
	//
	//# \desc
	//# The $QueryProximity$ function searches the world for all geometry nodes and rigid bodies having bounding volumes
	//# that intersect the sphere given by the $center$ and $radius$ parameters. For each geometry node or rigid body found,
	//# the callback function specified by the $proc$ parameter is called. The $ProximityProc$ type is defined as follows.
	//
	//# \code	typedef ProximityResult ProximityProc(Node *node, const Point3D& center, float radius, void *cookie);
	//
	//# The $node$ parameter passed to the callback function is either a geometry node or a node of any type to which a
	//# rigid body controller is attached. The $center$, $radius$, and $cookie$ parameters are the same as those passed to the
	//# $QueryProximity$ function. The callback function should return one of the following constants to determine how the enumerate proceeds.
	//
	//# \table	ProximityResult
	//
	//# The callback function is allowed to delete the node passed to it or any of its subnodes, but it may not delete
	//# other nodes elsewhere in the world. If the callback function deletes the node passed into the $node$ parameter,
	//# then it must return either $kProximitySkipSuccessors$ or $kProximityStop$.
	//#
	//# The $threadIndex$ parameter specifies the index of the Job Manager worker thread that has called the $QueryProximity$
	//# function. If the $QueryProximity$ function is called from the main thread, then this parameter should not be
	//# specified so that the default value is used. If the $QueryProximity$ function is called from a job, then the
	//# $threadIndex$ parameter should be set to the value returned by the $@System/Job::GetThreadIndex@$ function.
	//# The $threadIndex$ parameter must be set correctly in order for multithreaded proximity queries to work properly.
	//
	//# \important
	//# If the $QueryProximity$ function is called from inside a Job Manager worker thread, then the application code must
	//# ensure that the scene is not modified while the job is running.
	//
	//# \also	$@World::QueryCollision@$
	//# \also	$@World::DetectCollision@$


	//# \function	World::ActivateTriggers		Activates all triggers through which a given segment passes.
	//
	//# \proto	void ActivateTriggers(const Point3D& p1, const Point3D& p2, float radius, Node *initiator = nullptr);
	//
	//# \param	p1			The beginning of the line segment in world space.
	//# \param	p2			The end of the line segment in world space.
	//# \param	radius		The radius of the line segment.
	//# \param	initiator	The node that is assigned to be a trigger's initiator.
	//
	//# \desc
	//# The $ActivateTriggers$ function finds all trigger nodes in the world that intersect the line segment specified
	//# by the $p1$ and $p2$ parameters and activates them. If the $radius$ parameter is greater than zero, then the
	//# line segment is actually a swept sphere, and intersections are tested with its volume instead of an infinitely
	//# thin line segment.
	//#
	//# When a trigger node is activated, it searches for a controller to activate in the following order and performs
	//# exactly one of the following actions.
	//#
	//# 1. If the trigger node itself has a controller, then that controller is activated.<br/>
	//# 2. If the trigger object specifies a target connector key, there is a node connected to the trigger through a connector having that key, and that node has a controller, then that controller is activated.<br/>
	//# 3. If such a target node exists, but it does not have a controller, then its immediate subnodes are examined. Every controller belonging to that set of subnodes is activated.
	//
	//# \also	$@Trigger@$


	//# \div
	//# \function	World::HandleNewRigidBodyContact		Called by default when a new contact is made between two rigid bodies.
	//
	//# \proto	virtual RigidBodyStatus HandleNewRigidBodyContact(RigidBodyController *rigidBody, const RigidBodyContact *contact, RigidBodyController *contactBody);
	//
	//# \param	rigidBody		One rigid body making contact.
	//# \param	contact			The new contact.
	//# \param	contactBody		The other rigid body making contact.
	//
	//# \desc
	//# The $HandleNewRigidBodyContact$ function is called by the $@PhysicsMgr/RigidBodyController@$ class by default when a rigid body
	//# makes a new contact with another rigid body. This function can be overridden in a subclass of $World$ in order to carry
	//# out a specialized response to a collision.
	//#
	//# The $HandleNewRigidBodyContact$ function should return one of the following constants.
	//
	//# \table RigidBodyStatus
	//
	//# The default implementation of the $HandleNewRigidBodyContact$ function returns $kRigidBodyUnchanged$.
	//
	//# \also	$@World::HandleNewGeometryContact@$
	//# \also	$@PhysicsMgr/RigidBodyController::HandleNewRigidBodyContact@$
	//# \also	$@PhysicsMgr/RigidBodyContact@$


	//# \function	World::HandleNewGeometryContact		Called by default when a new contact is made between a rigid body and a geometry node.
	//
	//# \proto	virtual RigidBodyStatus HandleNewGeometryContact(RigidBodyController *rigidBody, const GeometryContact *contact);
	//
	//# \param	rigidBody		The rigid body making contact.
	//# \param	contact			The new contact.
	//
	//# \desc
	//# The $HandleNewGeometryContact$ function is called by the $@PhysicsMgr/RigidBodyController@$ class by default when a rigid body
	//# makes a new contact with a geometry node. This function can be overridden in a subclass of $World$ in order to carry
	//# out a specialized response to a collision.
	//#
	//# The $HandleNewGeometryContact$ function should return one of the following constants.
	//
	//# \table RigidBodyStatus
	//
	//# The default implementation of the $HandleNewGeometryContact$ function returns $kRigidBodyUnchanged$.
	//
	//# \also	$@World::HandleNewRigidBodyContact@$
	//# \also	$@PhysicsMgr/RigidBodyController::HandleNewGeometryContact@$
	//# \also	$@PhysicsMgr/GeometryContact@$


	//# \div
	//# \function	World::HandlePhysicsSpaceExit		Called by default when a rigid body exits the volume enclosed by the physics space.
	//
	//# \proto	virtual void HandlePhysicsSpaceExit(RigidBodyController *rigidBody);
	//
	//# \param	rigidBody		The rigid body that exited the physics space.
	//
	//# \desc
	//# The $HandlePhysicsSpaceExit$ function is called by the $@PhysicsMgr/RigidBodyController@$ class by default when a rigid body
	//# exits the volume enclosed by the physics space. This function can be overridden in a subclass of $World$ in order to carry
	//# out a specialized response to this occurrence.
	//
	//# The default implementation of the $HandlePhysicsSpaceExit$ function immediately deletes the node to which the rigid body
	//# controller is assigned.
	//
	//# \also	$@PhysicsMgr/RigidBodyController::HandlePhysicsSpaceExit@$


	class World : public LinkTarget<World>
	{
		friend class WorldMgr;

		public:

			typedef ProximityResult ProximityProc(Node *, const Point3D&, float, void *);

		private:

			ResourceName					worldName;
			ResourceLocation				resourceLocation;

			Link<World>						previousWorld;
			LoadContext						loadContext;

			Map<InstancedWorldData>			instancedWorldDataMap;
			Map<GenericModelData>			genericModelDataMap;

			unsigned_int32					worldFlags;
			unsigned_int32					worldPerspective;

			float							shaderTime;
			float							velocityNormalizationTime;

			int32							renderWidth;
			int32							renderHeight;

			ColorRGBA						finalColorScale[3];
			ColorRGBA						finalColorBias;

			Node							*rootNode;
			Skybox							*worldSkybox;
			const ColorRGBA					*clearColor;
			float							farClipDepth;

			FrustumCamera					*currentCamera;
			const WorldContext				*currentWorldContext;

			int32							remoteRecursionCount;
			int32							cameraRecursionCount;

			unsigned_int32					ambientRenderStamp;
			unsigned_int32					lightRenderStamp;
			unsigned_int32					shadowRenderStamp;
			unsigned_int32					shadowTerrainStamp;
			unsigned_int32					regionRenderStamp;

			List<Renderable>				renderStageList[kRenderStageCount];
			Array<Renderable *, 6>			skyboxRenderArray;
			Array<Renderable *>				ambientImpostorSystemArray;
			Array<Renderable *>				lightImpostorSystemArray;
			Array<Renderable *>				shadowImpostorSystemArray;
			Array<Renderable *>				renderStageArray[kRenderStageShadowCount];

			Array<TerrainLevelGeometry *>	terrainUpdateArray;
			Array<TerrainLevelGeometry *>	terrainShadowUpdateArray;
			Array<const Impostor *>			impostorRenderArray[kMaxShadowCascadeCount];
			Map<ImpostorSystem>				impostorSystemMap;

			Array<Light *, 8>				visibleLightArray;
			List<CameraRegion>				clippedCameraRegionList;

			unsigned_int8					controllerParity;
			unsigned_int8					effectParity;
			unsigned_int8					sourceParity;
			unsigned_int8					triggerParity;

			List<Effect>					movingEffectList[2];

			List<OmniSource>				engagedSourceList;
			List<OmniSource>				playingSourceList[2];

			unsigned_int32					triggerActivationStamp;
			List<Trigger>					activeTriggerList[2];

			Batch							worldBatch;

			WorldObservable					updateObservable;

			HashTable<Controller>			controllerTable;
			int32							nextControllerIndex;

			List<Controller>				controllerList[2];
			List<Controller>				physicsControllerList;

			List<Interactor>				interactorList;
			List<DeferredTask>				deferredTaskList;

			List<Marker>					shaderMarkerList;
			ShaderMarker					*warmupShaderMarker;
			int32							warmupShaderFace;

			#if C4STATS

				int32						worldCounter[kWorldCounterCount];

			#endif

			#if C4DIAGS

				unsigned_int32				diagnosticFlags;

				List<Renderable>			shadowRegionDiagnosticList;
				List<Renderable>			rigidBodyDiagnosticList;
				List<Renderable>			contactDiagnosticList;

			#endif

			void SetCameraClearParams(CameraObject *object) const;

			static ControllerMessage *CreateControllerMessage(ControllerMessageType controllerMessageType, int32 controllerIndex, Decompressor& data, void *world);
			static void ReceiveControllerMessage(const ControllerMessage *message, void *world);

			static bool DetectGeometryCollision(Geometry *geometry, const CollisionParams *collisionParams, CollisionData *collisionData, CollisionThreadData *threadData);
			static bool DetectNodeCollision(Node *node, const CollisionParams *collisionParams, CollisionData *collisionData, CollisionThreadData *threadData);
			static bool DetectCellCollision(const Site *cell, const CollisionParams *collisionParams, CollisionData *collisionData, CollisionThreadData *threadData);
			static bool DetectZoneCollision(Zone *zone, const CollisionParams *collisionParams, CollisionData *collisionData, CollisionThreadData *threadData);

			static CollisionState QueryNodeCollision(Node *node, const CollisionParams *collisionParams, CollisionData *collisionData, QueryThreadData *threadData);
			static CollisionState QueryCellCollision(const Site *cell, const CollisionParams *collisionParams, CollisionData *collisionData, QueryThreadData *threadData);
			static CollisionState QueryZoneCollision(Zone *zone, const CollisionParams *collisionParams, CollisionData *collisionData, QueryThreadData *threadData);

			static ProximityResult QueryNodeProximity(Node *node, const ProximityParams *proximityParams, QueryThreadData *threadData);
			static ProximityResult QueryCellProximity(const Site *cell, const ProximityParams *proximityParams, QueryThreadData *threadData);
			static ProximityResult QueryZoneProximity(Zone *zone, const ProximityParams *proximityParams, QueryThreadData *threadData);

			static bool DetectGeometryNodeInteraction(Node *node, const Box3D& box, const Point3D& p1, const Point3D& p2, InteractionData *interactionData, InteractionThreadData *threadData);
			static bool DetectGeometryCellInteraction(const Site *cell, const Box3D& box, const Point3D& p1, const Point3D& p2, InteractionData *interactionData, InteractionThreadData *threadData);
			static bool DetectEffectNodeInteraction(Effect *effect, const Box3D& box, const Point3D& p1, const Point3D& p2, InteractionData *interactionData, InteractionThreadData *threadData);
			static bool DetectEffectCellInteraction(const Site *cell, const Box3D& box, const Point3D& p1, const Point3D& p2, InteractionData *interactionData, InteractionThreadData *threadData);
			static bool DetectZoneInteraction(const Zone *zone, const Box3D& box, const Point3D& p1, const Point3D& p2, InteractionData *interactionData, InteractionThreadData *threadData);

			void ActivateCellTriggers(Site *cell, const Box3D& box, const Point3D& p1, const Point3D& p2, float radius, List<Trigger> *triggerList);
			void ActivateZoneTriggers(Zone *zone, const Point3D& p1, const Point3D& p2, float radius, List<Trigger> *triggerList);

			void MoveControllers(unsigned_int32 parity);
			void MoveEffects(unsigned_int32 parity);
			void MoveSources(unsigned_int32 parity);

			void Listen(void);

			static bool WorldBoundingBoxVisible(const Box3D& box, const VisibilityRegion *region, const List<OcclusionRegion> *occlusionList);
			static bool LightNodeVisible(const Node *node, const ImmutableArray<LightRegion *>& regionArray);

			static void UpdateMaxGeometryDepth(WorldContext *worldContext, Geometry *geometry);
			static void SetNodeFogState(const WorldContext *worldContext, RenderableNode *node);
			void ProcessGeometry(const WorldContext *worldContext, Geometry *geometry);

			void RenderAmbientGeometry(WorldContext *worldContext, const CameraRegion *cameraRegion, Geometry *geometry);
			void RenderAmbientTerrain(WorldContext *worldContext, const CameraRegion *cameraRegion, TerrainGeometry *terrain, TerrainLevelGeometry *superTerrain);
			void RenderAmbientNode(WorldContext *worldContext, const CameraRegion *cameraRegion, Node *node);
			void RenderAmbientCell(WorldContext *worldContext, const CameraRegion *cameraRegion, Site *cell);
			void RenderAmbientEffectNode(const WorldContext *worldContext, const CameraRegion *cameraRegion, Effect *effect);
			void RenderAmbientEffectCell(WorldContext *worldContext, const CameraRegion *cameraRegion, Site *cell);
			void RenderAmbientRegion(WorldContext *worldContext, const CameraRegion *cameraRegion);

			void RenderUnifiedGeometry(WorldContext *worldContext, const CameraRegion *cameraRegion, const ImmutableArray<LightRegion *>& regionArray, Geometry *geometry);
			void RenderUnifiedTerrain(WorldContext *worldContext, const CameraRegion *cameraRegion, const ImmutableArray<LightRegion *>& regionArray, TerrainGeometry *terrain, TerrainLevelGeometry *superTerrain);
			void RenderUnifiedNode(WorldContext *worldContext, const CameraRegion *cameraRegion, const ImmutableArray<LightRegion *>& regionArray, Node *node);
			void RenderUnifiedCell(WorldContext *worldContext, const CameraRegion *cameraRegion, const ImmutableArray<LightRegion *>& regionArray, Site *cell);
			void RenderUnifiedEffectNode(const WorldContext *worldContext, const CameraRegion *cameraRegion, const ImmutableArray<LightRegion *>& regionArray, Effect *effect);
			void RenderUnifiedEffectCell(WorldContext *worldContext, const CameraRegion *cameraRegion, const ImmutableArray<LightRegion *>& regionArray, Site *cell);
			void RenderUnifiedGeometry(WorldContext *worldContext, const CameraRegion *cameraRegion, Geometry *geometry);
			void RenderUnifiedTerrain(WorldContext *worldContext, const CameraRegion *cameraRegion, TerrainGeometry *terrain, TerrainLevelGeometry *superTerrain);
			void RenderUnifiedNode(WorldContext *worldContext, const CameraRegion *cameraRegion, Node *node);
			void RenderUnifiedCell(WorldContext *worldContext, const CameraRegion *cameraRegion, Site *cell);
			void RenderUnifiedEffectNode(const WorldContext *worldContext, const CameraRegion *cameraRegion, Effect *effect);
			void RenderUnifiedEffectCell(WorldContext *worldContext, const CameraRegion *cameraRegion, Site *cell);
			void RenderUnifiedRegion(WorldContext *worldContext, InfiniteLight *light, const CameraRegion *cameraRegion);
			void RenderUnified(WorldContext *worldContext, const Camera *camera, InfiniteLight *light, bool *shadowFlag);

			void RenderInfiniteLightTerrain(const WorldContext *worldContext, const ImmutableArray<LightRegion *>& regionArray, TerrainGeometry *terrain);
			void RenderInfiniteLightNode(const WorldContext *worldContext, const ImmutableArray<LightRegion *>& regionArray, Node *node);
			void RenderInfiniteLightCell(const WorldContext *worldContext, const ImmutableArray<LightRegion *>& regionArray, const Site *cell);
			void RenderInfiniteLightEffectNode(const WorldContext *worldContext, const ImmutableArray<LightRegion *>& regionArray, Effect *effect);
			void RenderInfiniteLightEffectCell(const WorldContext *worldContext, const ImmutableArray<LightRegion *>& regionArray, const Site *cell);
			void RenderInfiniteLightZone(const WorldContext *worldContext, const Zone *zone, const ImmutableArray<LightRegion *>& regionArray);
			void RenderInfiniteLightTerrain(const WorldContext *worldContext, TerrainGeometry *terrain);
			void RenderInfiniteLightNode(const WorldContext *worldContext, Node *node);
			void RenderInfiniteLightCell(const WorldContext *worldContext, const Site *cell);
			void RenderInfiniteLightEffectNode(const WorldContext *worldContext, Effect *effect);
			void RenderInfiniteLightEffectCell(const WorldContext *worldContext, const Site *cell);
			void RenderInfiniteLightZone(const WorldContext *worldContext, const Zone *zone);
			void RenderInfiniteLight(const WorldContext *worldContext, InfiniteLight *light, bool *shadowFlag);

			void RenderPointLightTerrain(const WorldContext *worldContext, const ImmutableArray<LightRegion *>& regionArray, TerrainGeometry *terrain);
			void RenderPointLightNode(const WorldContext *worldContext, const PointLight *light, const ImmutableArray<LightRegion *>& regionArray, Node *node);
			void RenderPointLightCell(const WorldContext *worldContext, const PointLight *light, const ImmutableArray<LightRegion *>& regionArray, const Site *cell);
			void RenderPointLightEffectNode(const WorldContext *worldContext, const PointLight *light, const ImmutableArray<LightRegion *>& regionArray, Effect *effect);
			void RenderPointLightEffectCell(const WorldContext *worldContext, const PointLight *light, const ImmutableArray<LightRegion *>& regionArray, const Site *cell);
			void RenderPointLightZone(const WorldContext *worldContext, const PointLight *light, const Zone *zone, const ImmutableArray<LightRegion *>& regionArray);
			void RenderPointLightTerrain(const WorldContext *worldContext, TerrainGeometry *terrain);
			void RenderPointLightNode(const WorldContext *worldContext, const PointLight *light, Node *node);
			void RenderPointLightCell(const WorldContext *worldContext, const PointLight *light, const Site *cell);
			void RenderPointLightEffectNode(const WorldContext *worldContext, const PointLight *light, Effect *effect);
			void RenderPointLightEffectCell(const WorldContext *worldContext, const PointLight *light, const Site *cell);
			void RenderPointLightZone(const WorldContext *worldContext, const PointLight *light, const Zone *zone);
			bool RenderPointLight(const WorldContext *worldContext, PointLight *light, bool *shadowFlag);
			bool RenderSpotLight(const WorldContext *worldContext, SpotLight *light, bool *shadowFlag);

			static void CalculateInfiniteShadowRegion(const CameraRegion *cameraRegion, const Polyhedron *cameraPolyhedron, const Vector3D& lightDirection, ShadowRegion *shadowRegion);
			static void CalculatePointShadowRegion(const CameraRegion *cameraRegion, const Point3D& lightPosition, ShadowRegion *shadowRegion);

			void RenderInfiniteShadowTerrain(const WorldContext *worldContext, int32 cascade, TerrainGeometry *terrain, TerrainLevelGeometry *superTerrain, const ShadowRegion *shadowRegion, const List<OcclusionRegion> *occlusionList);
			void RenderInfiniteShadowNode(const WorldContext *worldContext, int32 cascade, Node *node, const ShadowRegion *shadowRegion, const List<OcclusionRegion> *occlusionList);
			void RenderInfiniteShadowCell(const WorldContext *worldContext, int32 cascade, const Site *cell, const ShadowRegion *shadowRegion, const List<OcclusionRegion> *occlusionList);
			void RenderInfiniteShadowRegion(const WorldContext *worldContext, int32 cascade, const ShadowRegion *shadowRegion, const List<OcclusionRegion> *occlusionList);
			void RenderInfiniteShadowCascade(const WorldContext *worldContext, InfiniteLight *light, int32 cascade, const LightShadowData *shadowData);
			const LightShadowData *RenderInfiniteShadow(const WorldContext *worldContext, InfiniteLight *light);

			void RenderPointShadowTerrain(const WorldContext *worldContext, TerrainGeometry *terrain, TerrainLevelGeometry *superTerrain, const Box3D& lightBox, const LightRegion *lightRegion, const ShadowRegion *shadowRegion, const List<OcclusionRegion> *occlusionList);
			void RenderPointShadowNode(const WorldContext *worldContext, Node *node, const Node *excludedNode, const Box3D& lightBox, const LightRegion *lightRegion, const ShadowRegion *shadowRegion, const List<OcclusionRegion> *occlusionList);
			void RenderPointShadowCell(const WorldContext *worldContext, const Site *cell, const Node *excludedNode, const Box3D& lightBox, const LightRegion *lightRegion, const ShadowRegion *shadowRegion, const List<OcclusionRegion> *occlusionList);
			void RenderPointShadowRegion(const WorldContext *worldContext, const Node *excludedNode, const Box3D& lightBox, const LightRegion *lightRegion, const ShadowRegion *shadowRegion, const List<OcclusionRegion> *occlusionList);
			void RenderPointShadow(const WorldContext *worldContext, const PointLight *light);

			void RenderSpotShadowTerrain(const WorldContext *worldContext, TerrainGeometry *terrain, TerrainLevelGeometry *superTerrain, const LightRegion *lightRegion, const ShadowRegion *shadowRegion, const List<OcclusionRegion> *occlusionList);
			void RenderSpotShadowNode(const WorldContext *worldContext, Node *node, const Node *excludedNode, const LightRegion *lightRegion, const ShadowRegion *shadowRegion, const List<OcclusionRegion> *occlusionList);
			void RenderSpotShadowCell(const WorldContext *worldContext, const Site *cell, const Node *excludedNode, const LightRegion *lightRegion, const ShadowRegion *shadowRegion, const List<OcclusionRegion> *occlusionList);
			void RenderSpotShadowRegion(const WorldContext *worldContext, const Node *excludedNode, const LightRegion *lightRegion, const ShadowRegion *shadowRegion, const List<OcclusionRegion> *occlusionList);
			void RenderSpotShadow(const WorldContext *worldContext, const SpotLight *light);

			static bool PointLightVisible(PointLight *light, const Point3D& cameraPosition, const CameraRegion *cameraRegion, const List<OcclusionRegion> *occlusionList);
			static bool LightRegionVisible(const WorldContext *worldContext, const LightRegion *lightRegion, const CameraRegion *cameraRegion);

			void CollectLightRegions(WorldContext *worldContext, const Point3D& cameraPosition, CameraRegion *cameraRegion);
			void RenderLight(const WorldContext *worldContext, Light *light, bool unified = false);

			bool ProcessFogSpace(WorldContext *worldContext, const FogSpace *fogSpace, const CameraRegion *rootRegion);
			void ProcessPortal(WorldContext *worldContext, Portal *portal, CameraRegion *rootRegion);
			void ProcessCameraRegion(WorldContext *worldContext, CameraRegion *rootRegion);

			void RenderIndirectPortals(const WorldContext *worldContext);
			void RenderRemoteCamera(const WorldContext *worldContext, RemotePortal *remotePortal, RenderTargetType target, unsigned_int32 perspectiveFlags, const PortalData *portalData);
			void RenderCamera(WorldContext *worldContext, RenderTargetType target);
			void RenderDistantCamera(WorldContext *worldContext, RenderTargetType target);

			#if C4DIAGS

				void RenderLightRegionOutline(const Polyhedron *polyhedron);
				void RenderSourcePaths(Zone *zone, const Transform4D& listenerTransform);

			#endif

		public:

			C4API World(const char *name, unsigned_int32 flags = 0);
			C4API World(Node *root, unsigned_int32 flags = 0);
			C4API virtual ~World();

			const ResourceName& GetWorldName(void) const
			{
				return (worldName);
			}

			const ResourceLocation *GetResourceLocation(void) const
			{
				return (&resourceLocation);
			}

			LoadContext *GetLoadContext(void)
			{
				return (&loadContext);
			}

			void PurgeInstancedWorldData(void)
			{
				instancedWorldDataMap.Purge();
			}

			unsigned_int32 GetWorldFlags(void) const
			{
				return (worldFlags);
			}

			void SetWorldFlags(unsigned_int32 flags)
			{
				worldFlags = flags;
			}

			unsigned_int32 GetWorldPerspective(void) const
			{
				return (worldPerspective);
			}

			void SetWorldPerspective(unsigned_int32 perspective)
			{
				worldPerspective = perspective;
			}

			float GetVelocityNormalizationTime(void) const
			{
				return (velocityNormalizationTime);
			}

			void SetVelocityNormalizationTime(float time)
			{
				velocityNormalizationTime = time;
			}

			const ColorRGBA& GetFinalColorScale(int32 index = 0) const
			{
				return (finalColorScale[index]);
			}

			const ColorRGBA& GetFinalColorBias(void) const
			{
				return (finalColorBias);
			}

			FrustumCamera *GetCamera(void) const
			{
				return (currentCamera);
			}

			int32 GetRenderWidth(void) const
			{
				return (renderWidth);
			}

			int32 GetRenderHeight(void) const
			{
				return (renderHeight);
			}

			void SetRenderSize(int32 width, int32 height)
			{
				renderWidth = width;
				renderHeight = height;
			}

			Zone *GetRootNode(void) const
			{
				return (static_cast<Zone *>(rootNode));
			}

			void AddNewNode(Node *node)
			{
				rootNode->AppendNewSubnode(node);
			}

			void AddPlayingSource(OmniSource *source)
			{
				playingSourceList[sourceParity].Append(source);
			}

			void AddMovingEffect(Effect *effect)
			{
				movingEffectList[effectParity].Append(effect);
			}

			void AddInteractor(Interactor *interactor)
			{
				interactorList.Append(interactor);
			}

			void RemoveInteractor(Interactor *interactor)
			{
				if (interactorList.Member(interactor))
				{
					interactorList.Remove(interactor);
				}
			}

			void AddDeferredTask(DeferredTask *task)
			{
				deferredTaskList.Append(task);
			}

			void SubmitWorldJob(BatchJob *job)
			{
				TheJobMgr->SubmitJob(job, &worldBatch);
			}

			void FinishWorldBatch(void)
			{
				TheJobMgr->FinishBatch(&worldBatch);
			}

			void AddUpdateObserver(WorldObservable::ObserverType *observer)
			{
				updateObservable.AddObserver(observer);
			}

			const Map<ImpostorSystem> *GetImpostorSystemMap(void) const
			{
				return (&impostorSystemMap);
			}

			unsigned_int32 GetControllerParity(void) const
			{
				return (controllerParity);
			}

			unsigned_int32 GetTriggerParity(void) const
			{
				return (triggerParity);
			}

			unsigned_int32 GetEffectParity(void) const
			{
				return (effectParity);
			}

			unsigned_int32 GetSourceParity(void) const
			{
				return (sourceParity);
			}

			Controller *GetController(int32 index) const
			{
				return (controllerTable.Find(index));
			}

			int32 NewControllerIndex(void)
			{
				return (nextControllerIndex++);
			}

			#if C4STATS

				int32 GetWorldCounter(int32 index) const
				{
					return (worldCounter[index]);
				}

				void IncrementWorldCounter(int32 index)
				{
					worldCounter[index]++;
				}

			#endif

			#if C4DIAGS

				unsigned_int32 GetDiagnosticFlags(void) const
				{
					return (diagnosticFlags);
				}

				void SetDiagnosticFlags(unsigned_int32 flags)
				{
					diagnosticFlags = flags;
				}

				void PurgeShadowDiagnosticData(void)
				{
					shadowRegionDiagnosticList.Purge();
				}

				void AddRigidBodyRenderable(RigidBodyRenderable *renderable)
				{
					rigidBodyDiagnosticList.Append(renderable);
				}

				void PurgeRigidBodyDiagnosticData(void)
				{
					rigidBodyDiagnosticList.Purge();
				}

				void AddContactRenderable(ContactRenderable *renderable)
				{
					contactDiagnosticList.Append(renderable);
				}

				void PurgeContactDiagnosticData(void)
				{
					contactDiagnosticList.Purge();
				}

			#endif

			C4API virtual WorldResult Preprocess(void);
			C4API void ProcessWorldProperties(void);

			C4API bool Warmup(void);

			C4API int32 ExpandInstancedWorlds(Node *root, int32 depth = 0);

			Node *NewInstancedWorld(const char *name, Node::CloneFilterProc *filterProc = &Node::DefaultCloneFilter, void *filterCookie = nullptr);
			Node *NewGenericModel(const char *name, GenericModel *model);

			ImpostorSystem *GetImpostorSystem(MaterialObject *material, const float *clipData);

			C4API PhysicsController *FindPhysicsController(void) const;

			void AddController(Controller *controller);
			void RemoveController(Controller *controller);
			void WakeController(Controller *controller);
			static void SleepController(Controller *controller);

			C4API void SetCamera(FrustumCamera *camera);
			C4API void UpdateGeometry(Geometry *geometry);

			C4API bool DetectCollision(const Point3D& p1, const Point3D& p2, float radius, unsigned_int32 kind, CollisionData *collisionData, int32 threadIndex = JobMgr::kMaxWorkerThreadCount) const;
			C4API CollisionState QueryCollision(const Point3D& p1, const Point3D& p2, float radius, unsigned_int32 kind, CollisionData *collisionData, const RigidBodyController *excludedBody = nullptr, int32 threadIndex = JobMgr::kMaxWorkerThreadCount) const;
			C4API void QueryProximity(const Point3D& center, float radius, ProximityProc *proc, void *cookie, int32 threadIndex = JobMgr::kMaxWorkerThreadCount) const;

			const AcousticsProperty *DetectObstruction(const Point3D& position) const;
			bool DetectInteraction(const Point3D& p1, const Point3D& p2, InteractionData *interactionData) const;

			C4API void ActivateTriggers(const Point3D& p1, const Point3D& p2, float radius, Node *initiator = nullptr);

			C4API virtual RigidBodyStatus HandleNewRigidBodyContact(RigidBodyController *rigidBody, const RigidBodyContact *contact, RigidBodyController *contactBody);
			C4API virtual RigidBodyStatus HandleNewGeometryContact(RigidBodyController *rigidBody, const GeometryContact *contact);

			C4API virtual void HandlePhysicsSpaceExit(RigidBodyController *rigidBody);
			C4API virtual void HandleWaterSubmergence(RigidBodyController *rigidBody);

			C4API virtual void Move(void);
			C4API virtual void Update(void);
			C4API virtual void Interact(void);

			C4API virtual void BeginRendering(void);
			C4API virtual void EndRendering(void);
			C4API virtual void Render(void);

			C4API void SetFinalColorTransform(const ColorRGBA& scale, const ColorRGBA& bias);
			C4API void SetFinalColorTransform(const ColorRGBA& red, const ColorRGBA& green, const ColorRGBA& blue, const ColorRGBA& bias);
	};


	//# \class	WorldMgr	The World Manager class.
	//
	//# \def	class WorldMgr : public Manager<WorldMgr>
	//
	//# \desc
	//# The $WorldMgr$ class encapsulates the high-level world management features of the C4 Engine.
	//# The single instance of the World Manager is constructed during an application's initialization
	//# and destroyed at termination.
	//#
	//# The World Manager's member functions are accessed through the global pointer $TheWorldMgr$.
	//
	//# \also	$@World@$


	//# \function	WorldMgr::GetWorld		Returns the currently active world.
	//
	//# \proto	World *GetWorld(void) const;
	//
	//# \desc
	//
	//# \also	$@World@$
	//# \also	$@WorldMgr::LoadWorld@$
	//# \also	$@WorldMgr::UnloadWorld@$


	//# \function	WorldMgr::SetWorldCreator		Installs a world class creator function.
	//
	//# \proto	void SetWorldCreator(WorldCreateProc *proc, void *cookie = nullptr);
	//
	//# \param	proc	A pointer to the world creator function.
	//# \param	cookie	A pointer to user-defined data that is passed to the world creator function.
	//
	//# \desc
	//
	//# \code	typedef World *WorldCreateProc(const char *, void *);
	//
	//# \also	$@World@$


	//# \function	WorldMgr::LoadWorld		Loads a world resource and makes it the current world.
	//
	//# \proto	WorldResult LoadWorld(const char *name);
	//
	//# \param	name	The name of the world resource to load.
	//
	//# \desc
	//
	//# \also	$@World@$
	//# \also	$@WorldMgr::UnloadWorld@$
	//# \also	$@WorldMgr::SaveDeltaWorld@$
	//# \also	$@WorldMgr::RestoreDeltaWorld@$


	//# \function	WorldMgr::UnloadWorld		Unloads the current world data.
	//
	//# \proto	void UnloadWorld(void);
	//
	//# \desc
	//
	//# \also	$@World@$
	//# \also	$@WorldMgr::LoadWorld@$
	//# \also	$@WorldMgr::SaveDeltaWorld@$
	//# \also	$@WorldMgr::RestoreDeltaWorld@$


	//# \function	WorldMgr::SaveDeltaWorld		Saves a delta file for the current world.
	//
	//# \proto	void SaveDeltaWorld(const char *name);
	//
	//# \param	name	The name of the file to save.
	//
	//# \desc
	//
	//# \also	$@World@$
	//# \also	$@WorldMgr::RestoreDeltaWorld@$
	//# \also	$@WorldMgr::LoadWorld@$
	//# \also	$@WorldMgr::UnloadWorld@$


	//# \function	WorldMgr::RestoreDeltaWorld		Restores a previously saved delta file.
	//
	//# \proto	void RestoreDeltaWorld(const char *name);
	//
	//# \param	name	The name of the file to restore.
	//
	//# \desc
	//
	//# \also	$@World@$
	//# \also	$@WorldMgr::SaveDeltaWorld@$
	//# \also	$@WorldMgr::LoadWorld@$
	//# \also	$@WorldMgr::UnloadWorld@$


	class WorldMgr : public Manager<WorldMgr>
	{
		friend class World;

		public:

			typedef World *WorldCreateProc(const char *, void *);

		private:

			World							*currentWorld;
			World							*warmupWorld;

			WorldCreateProc					*worldCreatorProc;
			void							*worldCreatorCookie;

			Creator<Object>					objectCreator;
			StateSender						controllerStateSender;
			DisplayEventHandler				displayEventHandler;
			VariableObserver<WorldMgr>		lightDetailLevelObserver;

			int32							lightDetailLevel;
			float							defaultVelocityNormalizationTime;

			Quaternion						trackingOrientation;

			Storage<Signal>					loaderSignal;
			Storage<Thread>					loaderThread;
			void							(*loaderProc)(void *);
			void							*loaderCookie;

			static Object *CreateObject(Unpacker& data, unsigned_int32 unpackFlags);
			static void SendControllerState(Player *to, void *cookie);
			static void HandleDisplayEvent(const DisplayEventData *eventData, void *cookie);

			void HandleLightDetailLevelEvent(Variable *variable);

			static void LoaderThread(const Thread *thread, void *cookie);

		public:

			WorldMgr(int);
			~WorldMgr();

			EngineResult Construct(void);
			void Destruct(void);

			World *GetWorld(void) const
			{
				return (currentWorld);
			}

			void SetWorldCreator(WorldCreateProc *proc, void *cookie = nullptr)
			{
				worldCreatorProc = proc;
				worldCreatorCookie = cookie;
			}

			int32 GetLightDetailLevel(void) const
			{
				return (lightDetailLevel);
			}

			float GetDefaultVelocityNormalizationTime(void) const
			{
				return (defaultVelocityNormalizationTime);
			}

			void SetDefaultVelocityNormalizationTime(float time)
			{
				defaultVelocityNormalizationTime = time;
			}

			const Quaternion& GetTrackingOrientation(void) const
			{
				return (trackingOrientation);
			}

			C4API WorldResult LoadWorld(const char *name);
			C4API void UnloadWorld(void);
			C4API void RunWorld(World *world);

			C4API void SaveDeltaWorld(const char *name);
			C4API WorldResult RestoreDeltaWorld(const char *name);

			C4API void RunLoaderTask(void (*proc)(void *), void *cookie);

			void Move(void);
			void Render(void);
			void Warmup(void);
	};


	C4API extern WorldMgr *TheWorldMgr;
}


#endif

// ZYUQURM
