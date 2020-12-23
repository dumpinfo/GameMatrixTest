 

#include "C4World.h"
#include "C4Cameras.h"
#include "C4Particles.h"
#include "C4Forces.h"
#include "C4Fields.h"
#include "C4Joints.h"
#include "C4Blockers.h"
#include "C4Skybox.h"
#include "C4Triggers.h"
#include "C4Terrain.h"
#include "C4Water.h"
#include "C4Movies.h"


using namespace C4;


namespace
{
	const float kCameraNearDepth			= 0.1F;
	const float kCameraFarDepth				= 1000.0F;
	const float kCameraLightClipEpsilon		= 0.01F;
	const float kShadowRegionEpsilon		= 1.0e-6F;
	const float kDetailEpsilon				= 0.01F;


	enum
	{
		kMaxCollisionEdgeCount		= 32,
		kMaxRemoteRecursionCount	= 3,
		kMaxCameraRecursionCount	= 3
	};


	enum
	{
		kPortalGroupReflection,
		kPortalGroupRefraction,
		kPortalGroupRemote,
		kPortalGroupCamera,
		kPortalGroupCount
	};


	enum
	{
		kWorldUnfogEnable		= 1 << 0
	};
}


WorldMgr *C4::TheWorldMgr = nullptr;


namespace C4
{
	template <> WorldMgr Manager<WorldMgr>::managerObject(0);
	template <> WorldMgr **Manager<WorldMgr>::managerPointer = &TheWorldMgr;

	template <> const char *const Manager<WorldMgr>::resultString[] =
	{
		nullptr,
		"World failed to load"
	};

	template <> const unsigned_int32 Manager<WorldMgr>::resultIdentifier[] =
	{
		0, 'LOAD'
	};

	template class Manager<WorldMgr>;

	template <> Heap C4::Memory<PortalData>::heap("PortalData", 4096, kHeapMutexless);


	struct CollisionParams
	{
		const RigidBodyController	*excludedRigidBody;
		const Point3D				*colliderPosition[2];
		float						colliderRadius;
		Box3D						colliderBox;
		unsigned_int32				collisionKind;
	};


	struct ProximityParams
	{
		World::ProximityProc		*proximityProc;
		void						*proximityCookie;
		const Point3D				*proximityCenter;
		float						proximityRadius;
		Box3D						proximityBox;
	};


	struct InteractionData
	{
		float			param;
		Point3D			position;
		Node			*interaction;
	}; 


	class PortalData : public MapElement<PortalData>, public Memory<PortalData> 
	{
		private: 

			Portal		*targetPortal;
			Zone		*originZone; 

			int32		portalVertexCount; 
			Point3D		portalVertex[kMaxClippedPortalVertexCount]; 

		public:

			typedef Portal *KeyType; 

			PortalData(Portal *portal, Zone *zone, int32 vertexCount, const Point3D *vertex);
			~PortalData();

			KeyType GetKey(void) const
			{
				return (targetPortal);
			}

			Portal *GetPortal(void) const
			{
				return (targetPortal);
			}

			Zone *GetOriginZone(void) const
			{
				return (originZone);
			}

			int32 GetVertexCount(void) const
			{
				return (portalVertexCount);
			}

			const Point3D *GetVertexArray(void) const
			{
				return (portalVertex);
			}
	};


	class CollisionThreadData
	{
		private:

			int32								threadFlag;
			Array<Geometry *, 64>				geometryArray;

		protected:

			int32 GetThreadFlag(void) const
			{
				return (threadFlag);
			}

		public:

			CollisionThreadData(int32 flag);
			~CollisionThreadData();

			bool AddGeometry(Geometry *geometry);
	};


	class QueryThreadData : public CollisionThreadData
	{
		private:

			Array<RigidBodyController *, 32>	rigidBodyArray;

		public:

			QueryThreadData(int32 flag);
			~QueryThreadData();

			bool AddRigidBody(RigidBodyController *rigidBody);
	};


	class InteractionThreadData : public CollisionThreadData
	{
		private:

			Array<PanelEffect *, 8>				panelEffectArray;

		public:

			InteractionThreadData(int32 flag);
			~InteractionThreadData();

			bool AddPanelEffect(PanelEffect *panelEffect);
	};


	struct WorldContext
	{
		const FrustumCamera			*renderCamera;
		InfiniteLight				*unifiedLight;

		Skybox						*skyboxNode;
		bool						skyboxFlag;

		bool						shadowEnableFlag;
		mutable bool				reusableShadowFlag;

		unsigned_int32				perspectiveFlags;
		int32						lightDetailLevel;
		int32						cameraMinDetailLevel;
		float						cameraDetailBias;

		int8						extentBoxComponent[2][3];
		int8						depthBoxComponent[3];
		Range<float>				maxGeometryExtent[2];
		float						maxGeometryDepth;

		List<OcclusionRegion>		occlusionList;
		Map<PortalData>				portalGroup[kPortalGroupCount];

		const FogSpace				**fogSpacePtr;
		List<OcclusionRegion>		unfoggedList;
		OcclusionRegion				unfoggedRegion;

		WorldContext(const FrustumCamera *camera);
	};
}


ResourceDescriptor WorldResource::descriptor("wld");
ResourceDescriptor SaveResource::descriptor("sav");


#if C4DIAGS

	namespace
	{
		class LightRegionOutline : public ListElement<LightRegionOutline>
		{
			public:

				Polyhedron		regionPolyhedron;

				LightRegionOutline(const Polyhedron& polyhedron) : regionPolyhedron(polyhedron) {}
		};

		List<LightRegionOutline>	lightRegionDiagnosticList;
		List<Renderable>			lightRegionRenderList;
		Renderable					lightRegionRenderable(kRenderIndexedLines);
		VertexBuffer				lightRegionVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic);
		VertexBuffer				lightRegionIndexBuffer(kVertexBufferIndex | kVertexBufferDynamic);
		List<Attribute>				lightRegionAttributeList;
		DiffuseAttribute			lightRegionDiffuseColor(ColorRGBA(1.0F, 1.0F, 0.0F, 1.0F));

		List<Renderable>			sourcePathRenderList;
		Renderable					sourcePathRenderable(kRenderLines);
		VertexBuffer				sourcePathVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic);
		List<Attribute>				sourcePathAttributeList;
		DiffuseAttribute			sourcePathDiffuseColor(ColorRGBA(0.0F, 0.5F, 1.0F, 1.0F));
	}

#endif


PortalData::PortalData(Portal *portal, Zone *zone, int32 vertexCount, const Point3D *vertex)
{
	targetPortal = portal;
	originZone = zone;

	portalVertexCount = vertexCount;
	for (machine a = 0; a < vertexCount; a++)
	{
		portalVertex[a] = vertex[a];
	}
}

PortalData::~PortalData()
{
}


CollisionThreadData::CollisionThreadData(int32 flag)
{
	threadFlag = flag;
}

CollisionThreadData::~CollisionThreadData()
{
	int32 mask = ~threadFlag;

	for (Geometry *geometry : geometryArray)
	{
		AtomicAnd(geometry->GetQueryThreadFlags(), mask);
	}
}

bool CollisionThreadData::AddGeometry(Geometry *geometry)
{
	int32 flag = threadFlag;

	volatile int32 *geometryFlags = geometry->GetQueryThreadFlags();
	if (AtomicOr(geometryFlags, flag) & flag)
	{
		return (false);
	}

	geometryArray.AddElement(geometry);
	return (true);
}


QueryThreadData::QueryThreadData(int32 flag) : CollisionThreadData(flag)
{
}

QueryThreadData::~QueryThreadData()
{
	int32 mask = ~GetThreadFlag();

	for (RigidBodyController *rigidBody : rigidBodyArray)
	{
		AtomicAnd(rigidBody->GetQueryThreadFlags(), mask);
	}
}

bool QueryThreadData::AddRigidBody(RigidBodyController *rigidBody)
{
	int32 flag = GetThreadFlag();

	volatile int32 *rigidBodyFlags = rigidBody->GetQueryThreadFlags();
	if (AtomicOr(rigidBodyFlags, flag) & flag)
	{
		return (false);
	}

	rigidBodyArray.AddElement(rigidBody);
	return (true);
}


InteractionThreadData::InteractionThreadData(int32 flag) : CollisionThreadData(flag)
{
}

InteractionThreadData::~InteractionThreadData()
{
	int32 mask = ~GetThreadFlag();

	for (PanelEffect *panelEffect : panelEffectArray)
	{
		AtomicAnd(panelEffect->GetQueryThreadFlags(), mask);
	}
}

bool InteractionThreadData::AddPanelEffect(PanelEffect *panelEffect)
{
	int32 flag = GetThreadFlag();

	volatile int32 *panelEffectFlags = panelEffect->GetQueryThreadFlags();
	if (AtomicOr(panelEffectFlags, flag) & flag)
	{
		return (false);
	}

	panelEffectArray.AddElement(panelEffect);
	return (true);
}


WorldContext::WorldContext(const FrustumCamera *camera)
{
	renderCamera = camera;
	skyboxFlag = false;
	shadowEnableFlag = true;
	reusableShadowFlag = false;
	unfoggedRegion.SetPlaneCount(1);

	const Vector3D& right = camera->GetWorldTransform()[0];
	extentBoxComponent[0][0] = (right.x > 0.0F);
	extentBoxComponent[0][1] = (right.y > 0.0F);
	extentBoxComponent[0][2] = (right.z > 0.0F);

	const Vector3D& down = camera->GetWorldTransform()[1];
	extentBoxComponent[1][0] = (down.x > 0.0F);
	extentBoxComponent[1][1] = (down.y > 0.0F);
	extentBoxComponent[1][2] = (down.z > 0.0F);

	const Vector3D& view = camera->GetWorldTransform()[2];
	depthBoxComponent[0] = (view.x > 0.0F);
	depthBoxComponent[1] = (view.y > 0.0F);
	depthBoxComponent[2] = (view.z > 0.0F);

	maxGeometryExtent[0].Set(0.0F, 0.0F);
	maxGeometryExtent[1].Set(0.0F, 0.0F);
	maxGeometryDepth = camera->GetObject()->GetNearDepth() + 1.0F;
}


WorldResource::WorldResource(const char *name, ResourceCatalog *catalog) : Resource<WorldResource>(name, catalog)
{
}

WorldResource::~WorldResource()
{
}

ResourceResult WorldResource::LoadObjectOffsetTable(ResourceLoader *loader, WorldHeader *worldHeader, int32 **offsetTable) const
{
	ResourceResult result = loader->Read(worldHeader, 0, sizeof(WorldHeader));
	if (result != kResourceOkay)
	{
		return (result);
	}

	int32 offsetCount = worldHeader->offsetCount;
	int32 *table = new int32[offsetCount];

	result = loader->Read(table, sizeof(WorldHeader), offsetCount * 4);
	if (result == kResourceOkay)
	{
		*offsetTable = table;
		return (kResourceOkay);
	}

	delete[] table;
	return (result);
}

ResourceResult WorldResource::LoadAllObjects(ResourceLoader *loader, const WorldHeader *header, const int32 *offsetTable, char **objectData) const
{
	int32 start = offsetTable[0];
	unsigned_int32 size = offsetTable[header->offsetCount - 1] - start;
	char *data = new char[size];

	ResourceResult result = loader->Read(data, start, size);
	if (result == kResourceOkay)
	{
		*objectData = data;
		return (kResourceOkay);
	}

	delete[] data;
	return (result);
}

ResourceResult WorldResource::LoadObject(ResourceLoader *loader, int32 index, const int32 *offsetTable, char **objectData) const
{
	int32 start = offsetTable[index];
	unsigned_int32 size = offsetTable[index + 1] - start;
	char *data = new char[size];

	ResourceResult result = loader->Read(data, start, size);
	if (result == kResourceOkay)
	{
		*objectData = data;
		return (kResourceOkay);
	}

	delete[] data;
	return (result);
}


SaveResource::SaveResource(const char *name, ResourceCatalog *catalog) : Resource<SaveResource>(name, catalog)
{
}

SaveResource::~SaveResource()
{
}


InstancedWorldData::InstancedWorldData(unsigned_int32 hash, Node *node)
{
	worldHash = hash;
	prototypeCopy = node;
}

InstancedWorldData::~InstancedWorldData()
{
	delete prototypeCopy;
}


GenericModelData::GenericModelData(unsigned_int32 hash, GenericModel *model)
{
	modelHash = hash;
	modelList.Append(model);
}

GenericModelData::~GenericModelData()
{
}


Interactor::Interactor()
{
}

Interactor::~Interactor()
{
}

void Interactor::SetInteractionProbe(const Point3D& p1, const Point3D& p2)
{
	interactionPoint[0] = p1;
	interactionPoint[1] = p2;
}

void Interactor::HandleInteractionEvent(InteractionEventType type, Node *node, const Point3D *position)
{
	switch (type)
	{
		case kInteractionEventEngage:

			interactionNode = node;
			interactionPosition = *position;
			break;

		case kInteractionEventDisengage:

			interactionNode = nullptr;
			break;

		case kInteractionEventTrack:

			interactionPosition = *position;
			break;
	}
}

void Interactor::DetectInteraction(const World *world)
{
	InteractionData		data;

	data.param = 1.0F;
	if (world->DetectInteraction(interactionPoint[0], interactionPoint[1], &data))
	{
		Node *node = data.interaction;
		Point3D p = node->GetInverseWorldTransform() * data.position;

		if (node == interactionNode)
		{
			HandleInteractionEvent(kInteractionEventTrack, node, &p);
		}
		else
		{
			if (interactionNode)
			{
				HandleInteractionEvent(kInteractionEventDisengage, interactionNode);
			}

			HandleInteractionEvent(kInteractionEventEngage, node, &p);
		}
	}
	else
	{
		if (interactionNode)
		{
			HandleInteractionEvent(kInteractionEventDisengage, interactionNode);
		}
	}
}


World::World(const char *name, unsigned_int32 flags) :
		updateObservable(this),
		controllerTable(1024, 2)
{
	worldName = name;
	worldFlags = flags;
	rootNode = nullptr;
}

World::World(Node *root, unsigned_int32 flags) :
		updateObservable(this),
		controllerTable(1024, 2)
{
	worldName[0] = 0;
	worldFlags = flags;
	rootNode = root;
}

World::~World()
{
	engagedSourceList.RemoveAll();

	activeTriggerList[0].RemoveAll();
	activeTriggerList[1].RemoveAll();

	controllerList[0].RemoveAll();
	controllerList[1].RemoveAll();

	delete rootNode;
	SetCamera(nullptr);

	delete previousWorld.GetTarget();
}

WorldResult World::Preprocess(void)
{
	if (!rootNode)
	{
		int32	controllerCount;

		if (worldFlags & kWorldRestore)
		{
			SaveResource *resource = SaveResource::Get(worldName, 0, TheResourceMgr->GetSaveCatalog());
			if (resource)
			{
				#if C4LOG_RESOURCES

					TheResourceMgr->IncrementResourceLogLevel();

				#endif

				rootNode = Node::UnpackDeltaTree(resource->GetData(), worldName, previousWorld);
				controllerCount = resource->GetControllerCount();
				resource->Release();

				#if C4LOG_RESOURCES

					TheResourceMgr->DecrementResourceLogLevel();

				#endif
			}
		}
		else
		{
			WorldResource *resource = WorldResource::Get(worldName, 0, nullptr, &resourceLocation);
			if (resource)
			{
				#if C4LOG_RESOURCES

					TheResourceMgr->IncrementResourceLogLevel();

				#endif

				rootNode = Node::UnpackTree(resource->GetData(), 0);
				controllerCount = resource->GetControllerCount();
				resource->Release();

				#if C4LOG_RESOURCES

					TheResourceMgr->DecrementResourceLogLevel();

				#endif
			}
		}

		if (rootNode)
		{
			#if C4LOG_RESOURCES

				TheResourceMgr->IncrementResourceLogLevel();

			#endif

			loadContext.loadMagnitude = ExpandInstancedWorlds(rootNode);

			nextControllerIndex = controllerCount;

			#if C4PS4 //[ PS4

				// -- PS4 code hidden --

			#endif //]

			#if C4LOG_RESOURCES

				TheResourceMgr->DecrementResourceLogLevel();

			#endif
		}
		else
		{
			return (kWorldLoadFailed);
		}
	}
	else
	{
		nextControllerIndex = 0;
	}

	SetCamera(nullptr);

	worldFlags &= ~kWorldRestore;
	worldPerspective = 0;

	shaderTime = 0.0F;
	velocityNormalizationTime = TheWorldMgr->GetDefaultVelocityNormalizationTime();

	finalColorScale[0].Set(1.0F, 1.0F, 1.0F, 1.0F);
	finalColorBias.Set(0.0F, 0.0F, 0.0F, 0.0F);

	renderWidth = TheDisplayMgr->GetDisplayWidth();
	renderHeight = TheDisplayMgr->GetDisplayHeight();

	#if C4OCULUS

		if (TheDisplayMgr->GetDisplayFlags() & kDisplayOculus)
		{
			worldFlags |= kWorldOculusCamera;
		}

	#endif

	ambientRenderStamp = 0xFFFFFFFF;
	lightRenderStamp = 0xFFFFFFFF;
	shadowRenderStamp = 0xFFFFFFFF;
	shadowTerrainStamp = 0xFFFFFFFF;
	regionRenderStamp = 0xFFFFFFFF;
	triggerActivationStamp = 0xFFFFFFFF;

	controllerParity = 0;
	effectParity = 0;
	sourceParity = 0;
	triggerParity = 0;

	#if C4STATS

		for (machine a = 0; a < kWorldCounterCount; a++)
		{
			worldCounter[a] = 0;
		}

	#endif

	#if C4DIAGS

		diagnosticFlags = 0;

	#endif

	ProcessWorldProperties();

	rootNode->SetWorld(this);
	rootNode->Preprocess();
	rootNode->Update();

	Node *node = rootNode->GetFirstSubnode();
	while (node)
	{
		if (node->GetNodeType() == kNodeMarker)
		{
			Marker *marker = static_cast<Marker *>(node);
			if (marker->GetMarkerType() == kMarkerShader)
			{
				shaderMarkerList.Append(marker);
			}
		}

		node = rootNode->GetNextNode(node);
	}

	return (kWorldOkay);
}

void World::ProcessWorldProperties(void)
{
	worldSkybox = nullptr;
	clearColor = nullptr;
	farClipDepth = kCameraFarDepth;

	Node *node = rootNode->GetFirstSubnode();
	while (node)
	{
		if (node->GetNodeType() == kNodeSkybox)
		{
			worldSkybox = static_cast<Skybox *>(node);
			break;
		}

		node = node->Next();
	}

	const Property *property = rootNode->GetFirstProperty();
	while (property)
	{
		PropertyType type = property->GetPropertyType();
		if (type == kPropertyClear)
		{
			worldFlags |= kWorldClearColor;
			clearColor = &static_cast<const ClearProperty *>(property)->GetClearColor();
		}
		else if (type == kPropertyFarClip)
		{
			farClipDepth = static_cast<const FarClipProperty *>(property)->GetFarClipDepth();
		}

		property = property->Next();
	}
}

bool World::Warmup(void)
{
	unsigned_int32 flags = worldFlags;
	if (flags & kWorldWarmup)
	{
		int32 faceIndex = warmupShaderFace + 1;
		if (faceIndex < 6)
		{
			warmupShaderFace = faceIndex;
		}
		else
		{
			Marker *marker = warmupShaderMarker->Next();
			if (!marker)
			{
				worldFlags = flags & ~kWorldWarmup;
				TheWorldMgr->warmupWorld = nullptr;
				return (false);
			}

			warmupShaderMarker = static_cast<ShaderMarker *>(marker);
			warmupShaderFace = 0;
		}
	}
	else
	{
		Marker *marker = shaderMarkerList.First();
		if (!marker)
		{
			return (false);
		}

		warmupShaderMarker = static_cast<ShaderMarker *>(marker);
		warmupShaderFace = 0;

		worldFlags = flags | kWorldWarmup;
		TheWorldMgr->warmupWorld = this;
	}

	return (true);
}

int32 World::ExpandInstancedWorlds(Node *root, int32 depth)
{
	int32 count = 1;

	Node *node = root->GetFirstSubnode();
	while (node)
	{
		count++;

		if (node->GetNodeType() == kNodeInstance)
		{
			Node *next = root->GetNextLevelNode(node);

			Instance *instance = static_cast<Instance *>(node);
			if (instance->Expand(this))
			{
				count += node->GetSubtreeNodeCount();
				if (depth < kWorldMaxInstanceDepth)
				{
					ExpandInstancedWorlds(node, depth + 1);
				}

				instance->Extract(this);
			}

			node = next;
			continue;
		}

		node = root->GetNextNode(node);
	}

	return (count);
}

Node *World::NewInstancedWorld(const char *name, Node::CloneFilterProc *filterProc, void *filterCookie)
{
	unsigned_int32 hash = Text::Hash(name);
	InstancedWorldData *data = instancedWorldDataMap.Find(hash);
	if (data)
	{
		return (data->GetPrototypeCopy()->Clone(filterProc, filterCookie));
	}

	WorldResource *resource = WorldResource::Get(name);
	if (!resource)
	{
		return (nullptr);
	}

	#if C4LOG_RESOURCES

		TheResourceMgr->IncrementResourceLogLevel();

	#endif

	unsigned_int32 unpackFlags = (rootNode->GetManipulator()) ? kUnpackEditor | kUnpackNonpersistent | kUnpackExternal : kUnpackNonpersistent | kUnpackExternal;
	Node *node = Node::UnpackTree(resource->GetData(), unpackFlags);
	resource->Release();

	#if C4LOG_RESOURCES

		TheResourceMgr->DecrementResourceLogLevel();

	#endif

	data = new InstancedWorldData(hash, node);
	instancedWorldDataMap.Insert(data);
	return (node->Clone(filterProc, filterCookie));
}

Node *World::NewGenericModel(const char *name, GenericModel *model)
{
	unsigned_int32 hash = Text::Hash(name);
	GenericModelData *data = genericModelDataMap.Find(hash);
	if (data)
	{
		Node *node = data->GetGenericModel();
		data->AddGenericModel(model);

		if (node)
		{
			return (node->Clone());
		}
	}

	ModelResource *resource = ModelResource::Get(name);
	if (!resource)
	{
		return (nullptr);
	}

	#if C4LOG_RESOURCES

		TheResourceMgr->IncrementResourceLogLevel();

	#endif

	Node *node = Node::UnpackTree(resource->GetData(), kUnpackNonpersistent | kUnpackExternal);
	resource->Release();

	#if C4LOG_RESOURCES

		TheResourceMgr->DecrementResourceLogLevel();

	#endif

	if (!data)
	{
		genericModelDataMap.Insert(new GenericModelData(hash, model));
	}

	return (node);
}

ImpostorSystem *World::GetImpostorSystem(MaterialObject *material, const float *clipData)
{
	ImpostorSystem *system = impostorSystemMap.Find(material);
	if (system)
	{
		return (system);
	}

	system = new ImpostorSystem(material, clipData);
	impostorSystemMap.Insert(system);
	return (system);
}

PhysicsController *World::FindPhysicsController(void) const
{
	PhysicsNode *physicsNode = GetRootNode()->GetPhysicsNode();
	if (physicsNode)
	{
		Controller *controller = physicsNode->GetController();
		if ((controller) && (controller->GetControllerType() == kControllerPhysics))
		{
			return (static_cast<PhysicsController *>(controller));
		}
	}

	return (nullptr);
}

void World::AddController(Controller *controller)
{
	unsigned_int32 flags = controller->GetControllerFlags();
	if (!(flags & kControllerAsleep))
	{
		controller->Wake();
	}

	if (!(flags & kControllerLocal))
	{
		if (controller->GetControllerIndex() == kControllerUnassigned)
		{
			controller->SetControllerIndex(NewControllerIndex());
		}

		controllerTable.Insert(controller);
	}
}

void World::RemoveController(Controller *controller)
{
	HashTable<Controller>::Remove(controller);
	controller->ListElement<Controller>::Detach();
}

void World::WakeController(Controller *controller)
{
	if (!controller->GetOwningList())
	{
		unsigned_int32 flags = controller->GetControllerFlags();

		if (flags & kControllerPhysicsSimulation)
		{
			physicsControllerList.Append(controller);
		}
		else if (!(flags & kControllerMoveInhibit))
		{
			controllerList[controllerParity].Append(controller);
		}
	}
}

void World::SleepController(Controller *controller)
{
	List<Controller> *list = controller->GetOwningList();
	if (list)
	{
		list->Remove(controller);
	}
}

ControllerMessage *World::CreateControllerMessage(ControllerMessageType controllerMessageType, int32 controllerIndex, Decompressor& data, void *world)
{
	Controller *controller = static_cast<World *>(world)->GetController(controllerIndex);
	if (controller)
	{
		return (controller->CreateMessage(controllerMessageType));
	}

	return (nullptr);
}

void World::ReceiveControllerMessage(const ControllerMessage *message, void *world)
{
	Controller *controller = static_cast<World *>(world)->GetController(message->GetControllerIndex());
	if (controller)
	{
		unsigned_int32 flags = message->GetMessageFlags();
		if ((flags & (kMessageDestroyer | kMessageJournaled)) == kMessageJournaled)
		{
			ControllerMessage *journaledMessage = controller->GetFirstJournaledMessage();
			while (journaledMessage)
			{
				ControllerMessage *next = journaledMessage->Next();

				if (message->OverridesMessage(journaledMessage))
				{
					delete journaledMessage;
				}

				journaledMessage = next;
			}

			controller->AddJournaledMessage(const_cast<ControllerMessage *>(message));
		}

		controller->ReceiveMessage(message);
	}
}

void World::SetCameraClearParams(CameraObject *object) const
{
	if (worldFlags & kWorldClearColor)
	{
		if (clearColor)
		{
			object->SetClearColor(*clearColor);
		}

		object->SetClearFlags(kClearColorBuffer | kClearDepthStencilBuffer);
	}
	else
	{
		object->SetClearFlags(kClearDepthStencilBuffer);
	}
}

void World::SetCamera(FrustumCamera *camera)
{
	currentCamera = camera;

	unsigned_int32 flags = worldFlags;
	if (camera)
	{
		camera->SetWorld(this);

		if (!(flags & kWorldListenerInhibit))
		{
			TheSoundMgr->SetListenerTransformable(camera);
		}

		FrustumCameraObject *object = camera->GetObject();
		SetCameraClearParams(object);

		object->SetFrustumFlags(kFrustumInfinite);
		object->SetNearDepth(kCameraNearDepth);
		object->SetFarDepth(farClipDepth);

		#if C4OCULUS

			if (flags & kWorldOculusCamera)
			{
				object->SetProjectionOffset(Oculus::GetLensCenter());
				object->SetFocalLength(Oculus::GetLensFocalLength());
			}

		#endif
	}
	else
	{
		if (!(flags & kWorldListenerInhibit))
		{
			TheSoundMgr->SetListenerTransformable(nullptr);
		}
	}
}

bool World::DetectGeometryCollision(Geometry *geometry, const CollisionParams *collisionParams, CollisionData *collisionData, CollisionThreadData *threadData)
{
	const GeometryObject *object = geometry->GetObject();
	if ((object->GetCollisionExclusionMask() & collisionParams->collisionKind) == 0)
	{
		if (threadData->AddGeometry(geometry))
		{
			GeometryHitData		geometryHitData;

			const Transform4D& inverseTransform = geometry->GetInverseWorldTransform();
			if (object->DetectCollision(inverseTransform * *collisionParams->colliderPosition[0], inverseTransform * *collisionParams->colliderPosition[1], collisionParams->colliderRadius, &geometryHitData))
			{
				float t = geometryHitData.param;
				if (t < collisionData->param)
				{
					collisionData->param = t;
					collisionData->position = geometry->GetWorldTransform() * geometryHitData.position;
					collisionData->normal = geometryHitData.normal * inverseTransform;
					collisionData->geometry = geometry;
					collisionData->triangleIndex = geometryHitData.triangleIndex;
					return (true);
				}
			}
		}
	}

	return (false);
}

bool World::DetectNodeCollision(Node *node, const CollisionParams *collisionParams, CollisionData *collisionData, CollisionThreadData *threadData)
{
	if (node->Enabled())
	{
		bool result = false;

		if (node->GetNodeType() == kNodeGeometry)
		{
			result = DetectGeometryCollision(static_cast<Geometry *>(node), collisionParams, collisionData, threadData);
		}

		const Bond *bond = node->GetFirstOutgoingEdge();
		while (bond)
		{
			Site *site = bond->GetFinishElement();
			if (site->GetWorldBoundingBox().Intersection(collisionParams->colliderBox))
			{
				result |= DetectNodeCollision(static_cast<Node *>(site), collisionParams, collisionData, threadData);
			}

			bond = bond->GetNextOutgoingEdge();
		}

		return (result);
	}

	return (false);
}

bool World::DetectCellCollision(const Site *cell, const CollisionParams *collisionParams, CollisionData *collisionData, CollisionThreadData *threadData)
{
	bool result = false;

	const Bond *bond = cell->GetFirstOutgoingEdge();
	while (bond)
	{
		Site *site = bond->GetFinishElement();
		if (site->GetWorldBoundingBox().Intersection(collisionParams->colliderBox))
		{
			if (site->GetCellIndex() < 0)
			{
				result |= DetectNodeCollision(static_cast<Node *>(site), collisionParams, collisionData, threadData);
			}
			else
			{
				result |= DetectCellCollision(site, collisionParams, collisionData, threadData);
			}
		}

		bond = bond->GetNextOutgoingEdge();
	}

	return (result);
}

bool World::DetectZoneCollision(Zone *zone, const CollisionParams *collisionParams, CollisionData *collisionData, CollisionThreadData *threadData)
{
	const Point3D& p1 = *collisionParams->colliderPosition[0];
	const Transform4D& transform = zone->GetInverseWorldTransform();
	if (!zone->GetObject()->ExteriorSweptSphere(transform * p1, transform * (p1 + (*collisionParams->colliderPosition[1] - p1) * collisionData->param), collisionParams->colliderRadius))
	{
		bool result = false;

		if (DetectCellCollision(zone->GetCellGraphSite(kCellGraphGeometry), collisionParams, collisionData, threadData))
		{
			result = true;
		}

		Zone *subzone = zone->GetFirstSubzone();
		while (subzone)
		{
			result |= DetectZoneCollision(subzone, collisionParams, collisionData, threadData);
			subzone = subzone->Next();
		}

		return (result);
	}

	return (false);
}

bool World::DetectCollision(const Point3D& p1, const Point3D& p2, float radius, unsigned_int32 kind, CollisionData *collisionData, int32 threadIndex) const
{
	if (p1 != p2)
	{
		CollisionParams		collisionParams;

		#if C4SIMD

			vec_float r = VecLoadSmearScalar(&radius);
			vec_float q1 = VecLoadUnaligned(&p1.x);
			vec_float q2 = VecLoadUnaligned(&p2.x);
			collisionParams.colliderBox.Set(VecSub(VecMin(q1, q2), r), VecAdd(VecMax(q1, q2), r));

		#else

			collisionParams.colliderBox.min.Set(Fmin(p1.x, p2.x) - radius, Fmin(p1.y, p2.y) - radius, Fmin(p1.z, p2.z) - radius);
			collisionParams.colliderBox.max.Set(Fmax(p1.x, p2.x) + radius, Fmax(p1.y, p2.y) + radius, Fmax(p1.z, p2.z) + radius);

		#endif

		collisionParams.colliderPosition[0] = &p1;
		collisionParams.colliderPosition[1] = &p2;
		collisionParams.colliderRadius = radius;
		collisionParams.collisionKind = kind;

		CollisionThreadData threadData(1 << threadIndex);
		bool result = false;

		collisionData->param = 1.0F;
		if (DetectZoneCollision(GetRootNode(), &collisionParams, collisionData, &threadData))
		{
			collisionData->normal.Normalize();
			collisionData->position -= collisionData->normal * radius;
			result = true;
		}

		return (result);
	}

	return (false);
}

const AcousticsProperty *World::DetectObstruction(const Point3D& position) const
{
	CollisionData	collisionData;

	const Point3D& listenerPosition = currentCamera->GetWorldPosition();
	if (DetectCollision(position, listenerPosition, 0.0F, kCollisionSoundPath, &collisionData))
	{
		return (static_cast<const AcousticsProperty *>(collisionData.geometry->GetProperty(kPropertyAcoustics)));
	}

	return (nullptr);
}

CollisionState World::QueryNodeCollision(Node *node, const CollisionParams *collisionParams, CollisionData *collisionData, QueryThreadData *threadData)
{
	if (node->Enabled())
	{
		CollisionState result = kCollisionStateNone;

		Controller *controller = node->GetController();
		if ((controller) && (controller->GetBaseControllerType() == kControllerRigidBody))
		{
			RigidBodyController *rigidBody = static_cast<RigidBodyController *>(controller);
			if (threadData->AddRigidBody(rigidBody))
			{
				if (((rigidBody->GetCollisionExclusionMask() & collisionParams->collisionKind) == 0) && (rigidBody != collisionParams->excludedRigidBody))
				{
					BodyHitData		bodyHitData;

					if (rigidBody->DetectSegmentIntersection(*collisionParams->colliderPosition[0], *collisionParams->colliderPosition[1], collisionParams->colliderRadius, &bodyHitData))
					{
						float t = bodyHitData.param;
						if (t < collisionData->param)
						{
							result = kCollisionStateRigidBody;

							collisionData->param = t;
							collisionData->position = bodyHitData.position;
							collisionData->normal = bodyHitData.normal;
							collisionData->rigidBody = rigidBody;
							collisionData->shape = bodyHitData.shape;
						}
					}
				}
			}

			return (result);
		}
		else
		{
			if ((node->GetNodeType() == kNodeGeometry) && (DetectGeometryCollision(static_cast<Geometry *>(node), collisionParams, collisionData, threadData)))
			{
				result = kCollisionStateGeometry;
			}

			const Bond *bond = node->GetFirstOutgoingEdge();
			while (bond)
			{
				Site *site = bond->GetFinishElement();
				if (site->GetWorldBoundingBox().Intersection(collisionParams->colliderBox))
				{
					CollisionState state = QueryNodeCollision(static_cast<Node *>(site), collisionParams, collisionData, threadData);
					if (state != kCollisionStateNone)
					{
						result = state;
					}
				}

				bond = bond->GetNextOutgoingEdge();
			}
		}

		return (result);
	}

	return (kCollisionStateNone);
}

CollisionState World::QueryCellCollision(const Site *cell, const CollisionParams *collisionParams, CollisionData *collisionData, QueryThreadData *threadData)
{
	CollisionState result = kCollisionStateNone;

	const Bond *bond = cell->GetFirstOutgoingEdge();
	while (bond)
	{
		Site *site = bond->GetFinishElement();
		if (site->GetWorldBoundingBox().Intersection(collisionParams->colliderBox))
		{
			if (site->GetCellIndex() < 0)
			{
				CollisionState state = QueryNodeCollision(static_cast<Node *>(site), collisionParams, collisionData, threadData);
				if (state != kCollisionStateNone)
				{
					result = state;
				}
			}
			else
			{
				CollisionState state = QueryCellCollision(site, collisionParams, collisionData, threadData);
				if (state != kCollisionStateNone)
				{
					result = state;
				}
			}
		}

		bond = bond->GetNextOutgoingEdge();
	}

	return (result);
}

CollisionState World::QueryZoneCollision(Zone *zone, const CollisionParams *collisionParams, CollisionData *collisionData, QueryThreadData *threadData)
{
	const Point3D& p1 = *collisionParams->colliderPosition[0];
	const Transform4D& transform = zone->GetInverseWorldTransform();
	if (!zone->GetObject()->ExteriorSweptSphere(transform * p1, transform * (p1 + (*collisionParams->colliderPosition[1] - p1) * collisionData->param), collisionParams->colliderRadius))
	{
		CollisionState result = kCollisionStateNone;

		CollisionState state = QueryCellCollision(zone->GetCellGraphSite(kCellGraphGeometry), collisionParams, collisionData, threadData);
		if (state != kCollisionStateNone)
		{
			result = state;
		}

		Zone *subzone = zone->GetFirstSubzone();
		while (subzone)
		{
			state = QueryZoneCollision(subzone, collisionParams, collisionData, threadData);
			if (state != kCollisionStateNone)
			{
				result = state;
			}

			subzone = subzone->Next();
		}

		return (result);
	}

	return (kCollisionStateNone);
}

CollisionState World::QueryCollision(const Point3D& p1, const Point3D& p2, float radius, unsigned_int32 kind, CollisionData *collisionData, const RigidBodyController *excludedBody, int32 threadIndex) const
{
	if (p1 != p2)
	{
		CollisionParams		collisionParams;

		#if C4SIMD

			vec_float r = VecLoadSmearScalar(&radius);
			vec_float q1 = VecLoadUnaligned(&p1.x);
			vec_float q2 = VecLoadUnaligned(&p2.x);
			collisionParams.colliderBox.Set(VecSub(VecMin(q1, q2), r), VecAdd(VecMax(q1, q2), r));

		#else

			collisionParams.colliderBox.min.Set(Fmin(p1.x, p2.x) - radius, Fmin(p1.y, p2.y) - radius, Fmin(p1.z, p2.z) - radius);
			collisionParams.colliderBox.max.Set(Fmax(p1.x, p2.x) + radius, Fmax(p1.y, p2.y) + radius, Fmax(p1.z, p2.z) + radius);

		#endif

		collisionParams.excludedRigidBody = excludedBody;
		collisionParams.colliderPosition[0] = &p1;
		collisionParams.colliderPosition[1] = &p2;
		collisionParams.colliderRadius = radius;
		collisionParams.collisionKind = kind;

		QueryThreadData threadData(1 << threadIndex);
		CollisionState result = kCollisionStateNone;

		collisionData->param = 1.0F;
		CollisionState state = QueryZoneCollision(GetRootNode(), &collisionParams, collisionData, &threadData);
		if (state != kCollisionStateNone)
		{
			collisionData->normal.Normalize();
			collisionData->position -= collisionData->normal * radius;
			result = state;
		}

		return (result);
	}

	return (kCollisionStateNone);
}

ProximityResult World::QueryNodeProximity(Node *node, const ProximityParams *proximityParams, QueryThreadData *threadData)
{
	if (node->Enabled())
	{
		Controller *controller = node->GetController();
		if ((controller) && (controller->GetBaseControllerType() == kControllerRigidBody))
		{
			RigidBodyController *rigidBody = static_cast<RigidBodyController *>(controller);
			if (threadData->AddRigidBody(rigidBody))
			{
				ProximityResult result = (*proximityParams->proximityProc)(node, *proximityParams->proximityCenter, proximityParams->proximityRadius, proximityParams->proximityCookie);
				if (result == kProximityStop)
				{
					return (kProximityStop);
				}
			}
		}
		else
		{
			if ((node->GetNodeType() == kNodeGeometry) && (threadData->AddGeometry(static_cast<Geometry *>(node))))
			{
				ProximityResult result = (*proximityParams->proximityProc)(node, *proximityParams->proximityCenter, proximityParams->proximityRadius, proximityParams->proximityCookie);
				if (result == kProximityStop)
				{
					return (kProximityStop);
				}

				if (result == kProximitySkipSuccessors)
				{
					return (kProximityContinue);
				}
			}

			const Bond *bond = node->GetFirstOutgoingEdge();
			while (bond)
			{
				const Bond *next = bond->GetNextOutgoingEdge();

				Site *site = bond->GetFinishElement();
				if (site->GetWorldBoundingBox().Intersection(proximityParams->proximityBox))
				{
					if (QueryNodeProximity(static_cast<Node *>(site), proximityParams, threadData) == kProximityStop)
					{
						return (kProximityStop);
					}
				}

				bond = next;
			}
		}
	}

	return (kProximityContinue);
}

ProximityResult World::QueryCellProximity(const Site *cell, const ProximityParams *proximityParams, QueryThreadData *threadData)
{
	const Bond *bond = cell->GetFirstOutgoingEdge();
	while (bond)
	{
		const Bond *next = bond->GetNextOutgoingEdge();

		Site *site = bond->GetFinishElement();
		if (site->GetWorldBoundingBox().Intersection(proximityParams->proximityBox))
		{
			if (site->GetCellIndex() < 0)
			{
				if (QueryNodeProximity(static_cast<Node *>(site), proximityParams, threadData) == kProximityStop)
				{
					return (kProximityStop);
				}
			}
			else
			{
				if (QueryCellProximity(site, proximityParams, threadData) == kProximityStop)
				{
					return (kProximityStop);
				}
			}
		}

		bond = next;
	}

	return (kProximityContinue);
}

ProximityResult World::QueryZoneProximity(Zone *zone, const ProximityParams *proximityParams, QueryThreadData *threadData)
{
	const Transform4D& transform = zone->GetInverseWorldTransform();
	if (!zone->GetObject()->ExteriorSphere(transform * *proximityParams->proximityCenter, proximityParams->proximityRadius))
	{
		if (QueryCellProximity(zone->GetCellGraphSite(kCellGraphGeometry), proximityParams, threadData) == kProximityStop)
		{
			return (kProximityStop);
		}

		Zone *subzone = zone->GetFirstSubzone();
		while (subzone)
		{
			if (QueryZoneProximity(subzone, proximityParams, threadData) == kProximityStop)
			{
				return (kProximityStop);
			}

			subzone = subzone->Next();
		}
	}

	return (kProximityContinue);
}

void World::QueryProximity(const Point3D& center, float radius, ProximityProc *proc, void *cookie, int32 threadIndex) const
{
	ProximityParams		proximityParams;

	#if C4SIMD

		vec_float r = VecLoadSmearScalar(&radius);
		vec_float p = VecLoadUnaligned(&center.x);
		proximityParams.proximityBox.Set(VecSub(p, r), VecAdd(p, r));

	#else

		proximityParams.proximityBox.min.Set(center.x - radius, center.y - radius, center.z - radius);
		proximityParams.proximityBox.max.Set(center.x + radius, center.y + radius, center.z + radius);

	#endif

	proximityParams.proximityProc = proc;
	proximityParams.proximityCookie = cookie;
	proximityParams.proximityCenter = &center;
	proximityParams.proximityRadius = radius;

	QueryThreadData threadData(1 << threadIndex);

	QueryZoneProximity(GetRootNode(), &proximityParams, &threadData);
}

bool World::DetectGeometryNodeInteraction(Node *node, const Box3D& box, const Point3D& p1, const Point3D& p2, InteractionData *interactionData, InteractionThreadData *threadData)
{
	bool result = false;

	if (node->Enabled())
	{
		if (node->GetNodeType() == kNodeGeometry)
		{
			Geometry *geometry = static_cast<Geometry *>(node);

			const GeometryObject *object = geometry->GetObject();
			if (!(object->GetCollisionExclusionMask() & kCollisionInteraction))
			{
				const Property *property = geometry->GetProperty(kPropertyInteraction);
				if ((property) && (!(property->GetPropertyFlags() & kPropertyDisabled)))
				{
					if (threadData->AddGeometry(geometry))
					{
						GeometryHitData		geometryHitData;

						const Transform4D& inverseTransform = geometry->GetInverseWorldTransform();
						if (object->DetectCollision(inverseTransform * p1, inverseTransform * p2, 0.0F, &geometryHitData))
						{
							float t = geometryHitData.param;
							if (t < interactionData->param)
							{
								interactionData->param = t;
								interactionData->position = geometry->GetWorldTransform() * geometryHitData.position;
								interactionData->interaction = geometry;
								result = true;
							}
						}
					}
				}
			}
		}

		const Bond *bond = node->GetFirstOutgoingEdge();
		while (bond)
		{
			Site *site = bond->GetFinishElement();
			if (site->GetWorldBoundingBox().Intersection(box))
			{
				result |= DetectGeometryNodeInteraction(static_cast<Node *>(site), box, p1, p2, interactionData, threadData);
			}

			bond = bond->GetNextOutgoingEdge();
		}
	}

	return (result);
}

bool World::DetectGeometryCellInteraction(const Site *cell, const Box3D& box, const Point3D& p1, const Point3D& p2, InteractionData *interactionData, InteractionThreadData *threadData)
{
	bool result = false;

	const Bond *bond = cell->GetFirstOutgoingEdge();
	while (bond)
	{
		Site *site = bond->GetFinishElement();
		if (site->GetWorldBoundingBox().Intersection(box))
		{
			if (site->GetCellIndex() < 0)
			{
				result |= DetectGeometryNodeInteraction(static_cast<Node *>(site), box, p1, p2, interactionData, threadData);
			}
			else
			{
				result |= DetectGeometryCellInteraction(site, box, p1, p2, interactionData, threadData);
			}
		}

		bond = bond->GetNextOutgoingEdge();
	}

	return (result);
}

bool World::DetectEffectNodeInteraction(Effect *effect, const Box3D& box, const Point3D& p1, const Point3D& p2, InteractionData *interactionData, InteractionThreadData *threadData)
{
	bool result = false;

	if ((effect->Enabled()) && (effect->GetEffectType() == kEffectPanel))
	{
		const Property *property = effect->GetProperty(kPropertyInteraction);
		if ((property) && (!(property->GetPropertyFlags() & kPropertyDisabled)))
		{
			PanelEffect *panelEffect = static_cast<PanelEffect *>(effect);

			if (threadData->AddPanelEffect(panelEffect))
			{
				CollisionPoint		collisionPoint;

				const Transform4D& inverseTransform = effect->GetInverseWorldTransform();
				if (static_cast<PanelEffect *>(effect)->DetectCollision(inverseTransform * p1, inverseTransform * p2, &collisionPoint))
				{
					float t = collisionPoint.param;
					if (t < interactionData->param)
					{
						interactionData->param = t;
						interactionData->position = p1 + (p2 - p1) * t;
						interactionData->interaction = effect;
						result = true;
					}
				}
			}
		}
	}

	return (result);
}

bool World::DetectEffectCellInteraction(const Site *cell, const Box3D& box, const Point3D& p1, const Point3D& p2, InteractionData *interactionData, InteractionThreadData *threadData)
{
	bool result = false;

	const Bond *bond = cell->GetFirstOutgoingEdge();
	while (bond)
	{
		Site *site = bond->GetFinishElement();
		if (site->GetWorldBoundingBox().Intersection(box))
		{
			if (site->GetCellIndex() < 0)
			{
				result |= DetectEffectNodeInteraction(static_cast<Effect *>(site), box, p1, p2, interactionData, threadData);
			}
			else
			{
				result |= DetectEffectCellInteraction(site, box, p1, p2, interactionData, threadData);
			}
		}

		bond = bond->GetNextOutgoingEdge();
	}

	return (result);
}

bool World::DetectZoneInteraction(const Zone *zone, const Box3D& box, const Point3D& p1, const Point3D& p2, InteractionData *interactionData, InteractionThreadData *threadData)
{
	const Transform4D& transform = zone->GetInverseWorldTransform();
	if (!zone->GetObject()->ExteriorSweptSphere(transform * p1, transform * (p1 + (p2 - p1) * interactionData->param), 0.0F))
	{
		bool result = DetectGeometryCellInteraction(zone->GetCellGraphSite(kCellGraphGeometry), box, p1, p2, interactionData, threadData);
		result |= DetectEffectCellInteraction(zone->GetCellGraphSite(kCellGraphEffect), box, p1, p2, interactionData, threadData);

		const Zone *subzone = zone->GetFirstSubzone();
		while (subzone)
		{
			result |= DetectZoneInteraction(subzone, box, p1, p2, interactionData, threadData);
			subzone = subzone->Next();
		}

		return (result);
	}

	return (false);
}

bool World::DetectInteraction(const Point3D& p1, const Point3D& p2, InteractionData *interactionData) const
{
	Box3D	box;

	#if C4SIMD

		vec_float q1 = VecLoadUnaligned(&p1.x);
		vec_float q2 = VecLoadUnaligned(&p2.x);
		box.Set(VecMin(q1, q2), VecMax(q1, q2));

	#else

		box.min.Set(Fmin(p1.x, p2.x), Fmin(p1.y, p2.y), Fmin(p1.z, p2.z));
		box.max.Set(Fmax(p1.x, p2.x), Fmax(p1.y, p2.y), Fmax(p1.z, p2.z));

	#endif

	InteractionThreadData threadData(1 << JobMgr::kMaxWorkerThreadCount);
	return (DetectZoneInteraction(GetRootNode(), box, p1, p2, interactionData, &threadData));
}

void World::ActivateCellTriggers(Site *cell, const Box3D& box, const Point3D& p1, const Point3D& p2, float radius, List<Trigger> *triggerList)
{
	const Bond *bond = cell->GetFirstOutgoingEdge();
	while (bond)
	{
		const Bond *next = bond->GetNextOutgoingEdge();

		Site *site = bond->GetFinishElement();
		if (site->GetWorldBoundingBox().Intersection(box))
		{
			if (site->GetCellIndex() < 0)
			{
				Trigger *trigger = static_cast<Trigger *>(site);

				unsigned_int32 stamp = triggerActivationStamp;
				if (trigger->GetSiteStamp() != stamp)
				{
					trigger->SetSiteStamp(stamp);

					if ((trigger->Enabled()) && (!trigger->ListElement<Trigger>::GetOwningList()))
					{
						const Transform4D& transform = trigger->GetInverseWorldTransform();
						if (trigger->GetObject()->IntersectSegment(transform * p1, transform * p2, radius))
						{
							triggerList->Append(trigger);
						}
					}
				}
			}
			else
			{
				ActivateCellTriggers(site, box, p1, p2, radius, triggerList);
			}
		}

		bond = next;
	}
}

void World::ActivateZoneTriggers(Zone *zone, const Point3D& p1, const Point3D& p2, float radius, List<Trigger> *triggerList)
{
	const Transform4D& zoneTransform = zone->GetInverseWorldTransform();
	if (!zone->GetObject()->ExteriorSweptSphere(zoneTransform * p1, zoneTransform * p2, radius))
	{
		Box3D	box;

		#if C4SIMD

			vec_float r = VecLoadSmearScalar(&radius);
			vec_float q1 = VecLoadUnaligned(&p1.x);
			vec_float q2 = VecLoadUnaligned(&p2.x);
			box.Set(VecSub(VecMin(q1, q2), r), VecAdd(VecMax(q1, q2), r));

		#else

			box.min.Set(Fmin(p1.x, p2.x) - radius, Fmin(p1.y, p2.y) - radius, Fmin(p1.z, p2.z) - radius);
			box.max.Set(Fmax(p1.x, p2.x) + radius, Fmax(p1.y, p2.y) + radius, Fmax(p1.z, p2.z) + radius);

		#endif

		ActivateCellTriggers(zone->GetCellGraphSite(kCellGraphTrigger), box, p1, p2, radius, triggerList);

		Zone *subzone = zone->GetFirstSubzone();
		while (subzone)
		{
			ActivateZoneTriggers(subzone, p1, p2, radius, triggerList);
			subzone = subzone->Next();
		}
	}
}

void World::ActivateTriggers(const Point3D& p1, const Point3D& p2, float radius, Node *initiator)
{
	List<Trigger>	activateList;

	triggerActivationStamp++;
	ActivateZoneTriggers(GetRootNode(), p1, p2, radius, &activateList);

	for (;;)
	{
		Trigger *trigger = activateList.First();
		if (!trigger)
		{
			break;
		}

		unsigned_int32 triggerFlags = trigger->GetObject()->GetTriggerFlags();
		if (triggerFlags & kTriggerActivateDisable)
		{
			trigger->SetNodeFlags(trigger->GetNodeFlags() | kNodeDisabled);
			activateList.Remove(trigger);
		}
		else if (!(triggerFlags & kTriggerContinuouslyActivated))
		{
			activeTriggerList[triggerParity ^ 1].Append(trigger);
		}
		else
		{
			activateList.Remove(trigger);
		}

		trigger->Activate(initiator);
	}

	List<Trigger> *triggerList = &activeTriggerList[triggerParity];
	Trigger *trigger = triggerList->First();
	while (trigger)
	{
		Trigger *next = trigger->Next();

		if (trigger->Enabled())
		{
			const Transform4D& transform = trigger->GetInverseWorldTransform();
			if (trigger->GetObject()->IntersectSegment(transform * p1, transform * p2, radius))
			{
				activeTriggerList[triggerParity ^ 1].Append(trigger);
			}
		}
		else
		{
			triggerList->Remove(trigger);
		}

		trigger = next;
	}
}

RigidBodyStatus World::HandleNewRigidBodyContact(RigidBodyController *rigidBody, const RigidBodyContact *contact, RigidBodyController *contactBody)
{
	return (kRigidBodyUnchanged);
}

RigidBodyStatus World::HandleNewGeometryContact(RigidBodyController *rigidBody, const GeometryContact *contact)
{
	return (kRigidBodyUnchanged);
}

void World::HandlePhysicsSpaceExit(RigidBodyController *rigidBody)
{
	delete rigidBody->GetTargetNode();
}

void World::HandleWaterSubmergence(RigidBodyController *rigidBody)
{
}

void World::MoveControllers(unsigned_int32 parity)
{
	List<Controller> *currentList = &controllerList[parity];
	List<Controller> *nextList = &controllerList[parity ^ 1];

	for (;;)
	{
		Controller *controller = currentList->First();
		if (!controller)
		{
			break;
		}

		nextList->Append(controller);
		controller->Move();
	}

	Controller *controller = physicsControllerList.First();
	if (controller)
	{
		controller->Move();
	}
}

void World::MoveEffects(unsigned_int32 parity)
{
	List<Effect> *currentList = &movingEffectList[parity];
	List<Effect> *nextList = &movingEffectList[parity ^ 1];

	for (;;)
	{
		Effect *effect = currentList->First();
		if (!effect)
		{
			break;
		}

		nextList->Append(effect);
		effect->Move();
	}
}

void World::MoveSources(unsigned_int32 parity)
{
	List<OmniSource> *currentList = &playingSourceList[parity];
	List<OmniSource> *nextList = &playingSourceList[parity ^ 1];
	for (;;)
	{
		OmniSource *source = currentList->First();
		if (!source)
		{
			break;
		}

		nextList->Append(source);
		source->Move();

		#if C4STATS

			worldCounter[kWorldCounterPlayingSource]++;

		#endif
	}

	OmniSource *source = engagedSourceList.First();
	while (source)
	{
		OmniSource *next = source->Next();
		source->Move();
		source = next;

		#if C4STATS

			worldCounter[kWorldCounterEngagedSource]++;

		#endif
	}

	#if C4STATS

		worldCounter[kWorldCounterPlayingSource] += worldCounter[kWorldCounterEngagedSource];

	#endif
}

void World::Move(void)
{
	#if C4STATS

		for (machine a = 0; a < kWorldCounterCount; a++)
		{
			worldCounter[a] = 0;
		}

	#endif

	for (;;)
	{
		DeferredTask *task = deferredTaskList.First();
		if (!task)
		{
			break;
		}

		deferredTaskList.Remove(task);
		task->CallCompletionProc();
	}

	unsigned_int8 parity = controllerParity;
	MoveControllers(parity);
	controllerParity = parity ^ 1;

	parity = effectParity;
	MoveEffects(parity);
	effectParity = parity ^ 1;

	parity = sourceParity;
	MoveSources(parity);
	sourceParity = parity ^ 1;

	parity = triggerParity;
	List<Trigger> *triggerList = &activeTriggerList[parity];
	for (;;)
	{
		Trigger *trigger = triggerList->First();
		if (!trigger)
		{
			break;
		}

		triggerList->Remove(trigger);
		trigger->Deactivate();
	}

	triggerParity = parity ^ 1;
}

void World::Update(void)
{
	FrustumCamera *camera = currentCamera;
	if (camera)
	{
		if (!(worldFlags & kWorldViewport))
		{
			FrustumCameraObject *object = camera->GetObject();
			object->SetViewRect(Rect(0, 0, renderWidth, renderHeight));
			object->SetAspectRatio((float) renderHeight / (float) renderWidth);
		}

		camera->CancelUpdate();
		rootNode->Update();

		camera->Move();
		camera->Invalidate();
		camera->Update();

		camera->UpdateRootRegions(static_cast<Zone *>(rootNode));
	}
	else
	{
		rootNode->Update();
	}

	updateObservable.PostEvent();
}

void World::Interact(void)
{
	Interactor *interactor = interactorList.First();
	while (interactor)
	{
		interactor->DetectInteraction(this);
		interactor = interactor->Next();
	}
}

void World::Listen(void)
{
	const FrustumCamera *camera = currentCamera;
	if (camera)
	{
		if (!(worldFlags & kWorldListenerInhibit))
		{
			OmniSource *source = engagedSourceList.First();
			while (source)
			{
				source->BeginUpdate();
				source = source->Next();
			}

			const Point3D& listenerPosition = camera->GetWorldPosition();
			SoundRoom *listenerRoom = nullptr;

			const RootCameraRegion *cameraRegion = camera->GetFirstRootRegion();
			while (cameraRegion)
			{
				Zone *zone = cameraRegion->GetZone();
				if (zone->GetTraversalExclusionMask() == 0)
				{
					zone->SetTraversalExclusionMask(kZoneTraversalLocal);

					const AcousticsSpace *acousticsSpace = zone->GetConnectedAcousticsSpace();
					if (acousticsSpace)
					{
						listenerRoom = acousticsSpace->GetSoundRoom();
					}

					SourceRegion *sourceRegion = zone->GetFirstSourceRegion();
					while (sourceRegion)
					{
						OmniSource *omniSource = sourceRegion->GetSource();

						const Point3D& sourcePosition = sourceRegion->GetPermeatedPosition();
						if (Magnitude(sourcePosition - listenerPosition) + sourceRegion->GetPermeatedPathLength() < omniSource->GetSourceRange())
						{
							unsigned_int32 state = omniSource->sourceState;
							if (!(state & kSourceEngaged))
							{
								engagedSourceList.Append(omniSource);
								omniSource->BeginUpdate();
							}

							omniSource->sourceState = state | kSourceAudible;
							omniSource->AddPlayRegion(sourceRegion, listenerPosition);
						}

						sourceRegion = sourceRegion->Next();
					}
				}

				cameraRegion = cameraRegion->Next();
			}

			TheSoundMgr->SetListenerRoom(listenerRoom);

			cameraRegion = camera->GetFirstRootRegion();
			while (cameraRegion)
			{
				cameraRegion->GetZone()->SetTraversalExclusionMask(0);
				cameraRegion = cameraRegion->Next();
			}

			source = engagedSourceList.First();
			while (source)
			{
				OmniSource *next = source->Next();

				unsigned_int32 state = source->sourceState;
				if (state & kSourceAudible)
				{
					if (!(state & kSourceEngaged))
					{
						if (!source->Engage())
						{
							playingSourceList[sourceParity].Append(source);
						}
					}
					else
					{
						source->EndUpdate();
					}
				}
				else
				{
					source->Disengage();
					playingSourceList[sourceParity].Append(source);
				}

				source = next;
			}
		}
	}
}

bool World::WorldBoundingBoxVisible(const Box3D& box, const VisibilityRegion *region, const List<OcclusionRegion> *occlusionList)
{
	if (region->BoxVisible(box))
	{
		const OcclusionRegion *occluder = occlusionList->First();
		while (occluder)
		{
			if (occluder->BoxOccluded(box))
			{
				return (false);
			}

			occluder = occluder->Next();
		}

		return (true);
	}

	return (false);
}

bool World::LightNodeVisible(const Node *node, const ImmutableArray<LightRegion *>& regionArray)
{
	for (const LightRegion *lightRegion : regionArray)
	{
		if (node->Visible(lightRegion))
		{
			return (true);
		}
	}

	return (false);
}

void World::UpdateMaxGeometryDepth(WorldContext *worldContext, Geometry *geometry)
{
	const Box3D& box = geometry->GetWorldBoundingBox();
	const Transform4D& transform = worldContext->renderCamera->GetInverseWorldTransform();

	const int8 *index = worldContext->extentBoxComponent[0];
	int8 ix = index[0];
	int8 iy = index[1];
	int8 iz = index[2];
	Point3D xmin(box[ix ^ 1].x, box[iy ^ 1].y, box[iz ^ 1].z);
	Point3D xmax(box[ix].x, box[iy].y, box[iz].z);
	float minExtent = transform.GetRow(0) ^ xmin;
	float maxExtent = transform.GetRow(0) ^ xmax;
	worldContext->maxGeometryExtent[0].min = Fmin(worldContext->maxGeometryExtent[0].min, minExtent);
	worldContext->maxGeometryExtent[0].max = Fmax(worldContext->maxGeometryExtent[0].max, maxExtent);

	index = worldContext->extentBoxComponent[1];
	ix = index[0];
	iy = index[1];
	iz = index[2];
	Point3D ymin(box[ix ^ 1].x, box[iy ^ 1].y, box[iz ^ 1].z);
	Point3D ymax(box[ix].x, box[iy].y, box[iz].z);
	minExtent = transform.GetRow(1) ^ ymin;
	maxExtent = transform.GetRow(1) ^ ymax;
	worldContext->maxGeometryExtent[1].min = Fmin(worldContext->maxGeometryExtent[1].min, minExtent);
	worldContext->maxGeometryExtent[1].max = Fmax(worldContext->maxGeometryExtent[1].max, maxExtent);

	index = worldContext->depthBoxComponent;
	Point3D zmax(box[index[0]].x, box[index[1]].y, box[index[2]].z);
	float maxDepth = transform.GetRow(2) ^ zmax;
	worldContext->maxGeometryDepth = Fmax(worldContext->maxGeometryDepth, maxDepth);
}

void World::SetNodeFogState(const WorldContext *worldContext, RenderableNode *node)
{
	unsigned_int32 renderableFlags = node->GetRenderableFlags() & ~kRenderableUnfog;

	const OcclusionRegion *region = worldContext->unfoggedList.First();
	if ((region) && (region->BoxOccluded(node->GetWorldBoundingBox())))
	{
		renderableFlags |= kRenderableUnfog;
	}

	node->SetRenderableFlags(renderableFlags);
}

void World::ProcessGeometry(const WorldContext *worldContext, Geometry *geometry)
{
	const GeometryObject *object = geometry->GetObject();
	int32 geometryLevelCount = object->GetGeometryLevelCount();
	if ((geometryLevelCount > 1) || (object->GetGeometryFlags() & kGeometryShaderDetailEnable))
	{
		int32 minLevel = Max(geometry->GetMinDetailLevel(), worldContext->cameraMinDetailLevel);

		const BoundingSphere *sphere = geometry->GetBoundingSphere();
		const Point3D& center = sphere->GetCenter();

		const FrustumCamera *camera = worldContext->renderCamera;
		Vector3D direction = center - camera->GetWorldPosition();
		float d = Magnitude(direction);
		if ((d > kDetailEpsilon) && (camera->GetWorldTransform()[2] * direction > 0.0F))
		{
			float focalLength = static_cast<FrustumCameraObject *>(camera->Node::GetObject())->GetFocalLength();
			float r = sphere->GetRadius() * focalLength / d;
			float t = worldContext->cameraDetailBias - Log(r);

			int32 level = Min(Max((int32) (t - 1.5F + object->GetGeometryDetailBias()), minLevel), geometryLevelCount - 1);
			if (geometry->GetDetailLevel() != level)
			{
				geometry->SetDetailLevel(level);
			}

			if (object->GetGeometryFlags() & kGeometryShaderDetailEnable)
			{
				float u = t + 1.0F + object->GetShaderDetailBias();
				geometry->SetShaderDetailLevel(Min(Max((int32) u, minLevel), 1));
				geometry->SetShaderDetailParameter(FmaxZero(Fmin(1.0F - u, 1.0F)));
			}
		}
		else
		{
			if (geometry->GetDetailLevel() != 0)
			{
				geometry->SetDetailLevel(0);
			}

			geometry->SetShaderDetailLevel(0);
			geometry->SetShaderDetailParameter(1.0F);
		}
	}

	Controller *controller = geometry->GetController();
	if ((controller) && (controller->GetControllerFlags() & kControllerUpdate))
	{
		controller->Update();
	}
}

void World::UpdateGeometry(Geometry *geometry)
{
	unsigned_int32 ambientStamp = ambientRenderStamp;
	if (geometry->GetProcessStamp() != ambientStamp)
	{
		geometry->SetProcessStamp(ambientStamp);
		ProcessGeometry(currentWorldContext, geometry);
	}
}

void World::RenderAmbientGeometry(WorldContext *worldContext, const CameraRegion *cameraRegion, Geometry *geometry)
{
	unsigned_int32 flags = geometry->GetObject()->GetGeometryFlags();
	if (!(flags & kGeometryInvisible))
	{
		geometry->SetProcessStamp(ambientRenderStamp);
		ProcessGeometry(worldContext, geometry);

		UpdateMaxGeometryDepth(worldContext, geometry);
		SetNodeFogState(worldContext, geometry);

		renderStageList[geometry->GetAmbientRenderStage()].Append(geometry);

		#if C4STATS

			worldCounter[kWorldCounterGeometry]++;

		#endif

		if (geometry->GetGeometryType() == kGeometryWater)
		{
			float d = SquaredMag(geometry->GetBoundingSphere()->GetCenter() - worldContext->renderCamera->GetWorldPosition());
			static_cast<WaterGeometry *>(geometry)->UpdateWater(d);

			#if C4STATS

				worldCounter[kWorldCounterWater]++;

			#endif
		}
	}
}

void World::RenderAmbientTerrain(WorldContext *worldContext, const CameraRegion *cameraRegion, TerrainGeometry *terrain, TerrainLevelGeometry *superTerrain)
{
	const TerrainGeometryObject *object = terrain->GetObject();
	int32 level = object->GetDetailLevel();

	if (level <= worldContext->cameraMinDetailLevel)
	{
		unsigned_int32 ambientStamp = ambientRenderStamp;
		if (terrain->GetSiteStamp() != ambientStamp)
		{
			terrain->SetSiteStamp(ambientStamp);

			if (!(object->GetGeometryFlags() & kGeometryInvisible))
			{
				UpdateMaxGeometryDepth(worldContext, terrain);
				SetNodeFogState(worldContext, terrain);

				renderStageList[kRenderStageAmbientDefault].Append(terrain);

				if (level != 0)
				{
					TerrainLevelGeometry *terrainLevel = static_cast<TerrainLevelGeometry *>(terrain);
					terrainUpdateArray.AddElement(terrainLevel);
					terrainLevel->SetTerrainStamp(kTerrainStampAmbient, ambientStamp);
				}

				if (superTerrain)
				{
					superTerrain->SetSubterrainStamp(kTerrainStampAmbient, ambientStamp);
				}

				#if C4STATS

					worldCounter[kWorldCounterTerrain]++;

				#endif
			}
		}
	}
	else
	{
		const FrustumCamera *camera = worldContext->renderCamera;
		Vector3D direction = terrain->GetWorldCenter() - camera->GetWorldPosition();
		float d = Magnitude(direction) / camera->GetObject()->GetFocalLength();

		if ((d > terrain->GetRenderDistance()) && (camera->GetWorldTransform()[2] * direction > 0.0F))
		{
			unsigned_int32 ambientStamp = ambientRenderStamp;
			if (terrain->GetSiteStamp() != ambientStamp)
			{
				terrain->SetSiteStamp(ambientStamp);

				if (!(object->GetGeometryFlags() & kGeometryInvisible))
				{
					UpdateMaxGeometryDepth(worldContext, terrain);
					SetNodeFogState(worldContext, terrain);

					renderStageList[kRenderStageAmbientDefault].Append(terrain);

					TerrainLevelGeometry *terrainLevel = static_cast<TerrainLevelGeometry *>(terrain);
					terrainUpdateArray.AddElement(terrainLevel);

					terrainLevel->SetTerrainStamp(kTerrainStampAmbient, ambientStamp);
					if (superTerrain)
					{
						superTerrain->SetSubterrainStamp(kTerrainStampAmbient, ambientStamp);
					}


					#if C4STATS

						worldCounter[kWorldCounterTerrain]++;

					#endif
				}
			}
		}
		else
		{
			terrain->SetSiteStamp(ambientRenderStamp);

			const Bond *bond = terrain->GetFirstOutgoingEdge();
			while (bond)
			{
				TerrainGeometry *subterrain = static_cast<TerrainGeometry *>(bond->GetFinishElement());
				if ((subterrain->Enabled()) && ((subterrain->GetPerspectiveExclusionMask() & worldContext->perspectiveFlags) == 0) && (subterrain->Visible(cameraRegion, &worldContext->occlusionList)))
				{
					RenderAmbientTerrain(worldContext, cameraRegion, subterrain, static_cast<TerrainLevelGeometry *>(terrain));
				}

				bond = bond->GetNextOutgoingEdge();
			}
		}
	}
}

void World::RenderAmbientNode(WorldContext *worldContext, const CameraRegion *cameraRegion, Node *node)
{
	if ((node->Enabled()) && ((node->GetPerspectiveExclusionMask() & worldContext->perspectiveFlags) == 0))
	{
		NodeType type = node->GetNodeType();

		if (node->Visible(cameraRegion, &worldContext->occlusionList))
		{
			unsigned_int32 ambientStamp = ambientRenderStamp;

			if (type == kNodeGeometry)
			{
				Geometry *geometry = static_cast<Geometry *>(node);
				if (geometry->GetGeometryType() != kGeometryTerrain)
				{
					if (geometry->GetSiteStamp() != ambientStamp)
					{
						geometry->SetSiteStamp(ambientStamp);
						RenderAmbientGeometry(worldContext, cameraRegion, geometry);
					}
				}
				else
				{
					RenderAmbientTerrain(worldContext, cameraRegion, static_cast<TerrainGeometry *>(geometry), nullptr);
					return;
				}
			}
			else if (type == kNodeImpostor)
			{
				Impostor *impostor = static_cast<Impostor *>(node);

				float distance = SquaredMag(impostor->GetWorldPosition().GetVector2D() - worldContext->renderCamera->GetWorldPosition().GetVector2D());
				if (distance > impostor->GetSquaredRenderDistance())
				{
					if (impostor->GetSiteStamp() != ambientStamp)
					{
						impostor->SetSiteStamp(ambientStamp);
						impostor->Render(kImpostorRenderAmbient);

						#if C4STATS

							worldCounter[kWorldCounterImpostor]++;

						#endif
					}

					if (distance > impostor->GetSquaredGeometryDistance())
					{
						return;
					}
				}
				else
				{
					node->SetSiteStamp(ambientStamp);
				}
			}
			else
			{
				node->SetSiteStamp(ambientStamp);
			}

			const Bond *bond = node->GetFirstOutgoingEdge();
			while (bond)
			{
				RenderAmbientNode(worldContext, cameraRegion, static_cast<Node *>(bond->GetFinishElement()));
				bond = bond->GetNextOutgoingEdge();
			}
		}
		else if (type == kNodeGeometry)
		{
			Geometry *geometry = static_cast<Geometry *>(node);
			if (geometry->GetGeometryType() == kGeometryWater)
			{
				float d = SquaredMag(geometry->GetBoundingSphere()->GetCenter() - worldContext->renderCamera->GetWorldPosition());
				static_cast<WaterGeometry *>(geometry)->UpdateInvisibleWater(d);
			}
		}
	}
}

void World::RenderAmbientCell(WorldContext *worldContext, const CameraRegion *cameraRegion, Site *cell)
{
	if (WorldBoundingBoxVisible(cell->GetWorldBoundingBox(), cameraRegion, &worldContext->occlusionList))
	{
		cell->SetSiteStamp(ambientRenderStamp);

		const Bond *bond = cell->GetFirstOutgoingEdge();
		while (bond)
		{
			Site *site = bond->GetFinishElement();
			if (site->GetCellIndex() < 0)
			{
				RenderAmbientNode(worldContext, cameraRegion, static_cast<Node *>(site));
			}
			else
			{
				RenderAmbientCell(worldContext, cameraRegion, site);
			}

			bond = bond->GetNextOutgoingEdge();
		}
	}
}

void World::RenderAmbientEffectNode(const WorldContext *worldContext, const CameraRegion *cameraRegion, Effect *effect)
{
	unsigned_int32 ambientStamp = ambientRenderStamp;
	if (effect->GetSiteStamp() != ambientStamp)
	{
		if ((effect->Enabled()) && ((effect->GetPerspectiveExclusionMask() & worldContext->perspectiveFlags) == 0) && (effect->Visible(cameraRegion, &worldContext->occlusionList)))
		{
			effect->SetSiteStamp(ambientStamp);

			SetNodeFogState(worldContext, effect);
			effect->Render(worldContext->renderCamera, &renderStageList[kRenderStageFirstEffect]);

			if ((effect->GetEffectListIndex() == kEffectListLight) && (effect->Rendering()))
			{
				effect->SetRenderStamp(ambientStamp);
			}
		}
	}
}

void World::RenderAmbientEffectCell(WorldContext *worldContext, const CameraRegion *cameraRegion, Site *cell)
{
	if (WorldBoundingBoxVisible(cell->GetWorldBoundingBox(), cameraRegion, &worldContext->occlusionList))
	{
		cell->SetSiteStamp(ambientRenderStamp);

		const Bond *bond = cell->GetFirstOutgoingEdge();
		while (bond)
		{
			Site *site = bond->GetFinishElement();
			if (site->GetCellIndex() < 0)
			{
				RenderAmbientEffectNode(worldContext, cameraRegion, static_cast<Effect *>(site));
			}
			else
			{
				RenderAmbientEffectCell(worldContext, cameraRegion, site);
			}

			bond = bond->GetNextOutgoingEdge();
		}
	}
}

void World::RenderAmbientRegion(WorldContext *worldContext, const CameraRegion *cameraRegion)
{
	const Zone *zone = cameraRegion->GetZone();

	if (zone->GetObject()->GetZoneFlags() & kZoneRenderSkybox)
	{
		worldContext->skyboxFlag = true;
	}

	OcclusionRegion *unfoggedRegion = worldContext->unfoggedList.First();
	if ((unfoggedRegion) && (!zone->GetFirstFogSpace()) && (!zone->GetConnectedFogSpace()))
	{
		worldContext->unfoggedList.Remove(unfoggedRegion);
	}

	const Bond *bond = zone->GetCellGraphSite(kCellGraphGeometry)->GetFirstOutgoingEdge();
	while (bond)
	{
		Site *site = bond->GetFinishElement();
		if (site->GetCellIndex() < 0)
		{
			RenderAmbientNode(worldContext, cameraRegion, static_cast<Node *>(site));
		}
		else
		{
			RenderAmbientCell(worldContext, cameraRegion, site);
		}

		bond = bond->GetNextOutgoingEdge();
	}

	bond = zone->GetCellGraphSite(kCellGraphEffect)->GetFirstOutgoingEdge();
	while (bond)
	{
		Site *site = bond->GetFinishElement();
		if (site->GetCellIndex() < 0)
		{
			RenderAmbientEffectNode(worldContext, cameraRegion, static_cast<Effect *>(site));
		}
		else
		{
			RenderAmbientEffectCell(worldContext, cameraRegion, site);
		}

		bond = bond->GetNextOutgoingEdge();
	}

	if (unfoggedRegion)
	{
		worldContext->unfoggedList.Append(unfoggedRegion);
	}

	CameraRegion *subregion = cameraRegion->GetFirstSubnode();
	while (subregion)
	{
		RenderAmbientRegion(worldContext, subregion);
		subregion = subregion->Tree<CameraRegion>::Next();
	}
}

void World::RenderUnifiedGeometry(WorldContext *worldContext, const CameraRegion *cameraRegion, const ImmutableArray<LightRegion *>& regionArray, Geometry *geometry)
{
	unsigned_int32 ambientStamp = ambientRenderStamp;
	unsigned_int32 lightStamp = lightRenderStamp;

	if (geometry->GetSiteStamp() != ambientStamp)
	{
		geometry->SetSiteStamp(ambientStamp);

		unsigned_int32 flags = geometry->GetObject()->GetGeometryFlags();
		if (!(flags & kGeometryInvisible))
		{
			geometry->SetProcessStamp(ambientRenderStamp);
			ProcessGeometry(worldContext, geometry);

			UpdateMaxGeometryDepth(worldContext, geometry);
			SetNodeFogState(worldContext, geometry);

			if ((LightNodeVisible(geometry, regionArray)) && ((flags & (kGeometryAmbientOnly | kGeometryRenderEffectPass)) == 0))
			{
				geometry->SetLightStamp(lightStamp);
				renderStageList[geometry->GetLightRenderStage()].Append(geometry);
			}
			else
			{
				renderStageList[geometry->GetAmbientRenderStage()].Append(geometry);
			}

			#if C4STATS

				worldCounter[kWorldCounterGeometry]++;

			#endif

			if (geometry->GetGeometryType() == kGeometryWater)
			{
				float d = SquaredMag(geometry->GetBoundingSphere()->GetCenter() - worldContext->renderCamera->GetWorldPosition());
				static_cast<WaterGeometry *>(geometry)->UpdateWater(d);

				#if C4STATS

					worldCounter[kWorldCounterWater]++;

				#endif
			}
		}
	}
	else if (geometry->GetLightStamp() != lightStamp)
	{
		unsigned_int32 flags = geometry->GetObject()->GetGeometryFlags();
		if ((flags & (kGeometryInvisible | kGeometryAmbientOnly | kGeometryRenderEffectPass)) == 0)
		{
			if (LightNodeVisible(geometry, regionArray))
			{
				geometry->SetLightStamp(lightStamp);
				renderStageList[geometry->GetLightRenderStage()].Append(geometry);
			}
		}
	}
}

void World::RenderUnifiedTerrain(WorldContext *worldContext, const CameraRegion *cameraRegion, const ImmutableArray<LightRegion *>& regionArray, TerrainGeometry *terrain, TerrainLevelGeometry *superTerrain)
{
	const TerrainGeometryObject *object = terrain->GetObject();
	int32 level = object->GetDetailLevel();

	if (level <= worldContext->cameraMinDetailLevel)
	{
		unsigned_int32 ambientStamp = ambientRenderStamp;
		unsigned_int32 lightStamp = lightRenderStamp;

		if (terrain->GetSiteStamp() != ambientStamp)
		{
			terrain->SetSiteStamp(ambientStamp);

			unsigned_int32 flags = object->GetGeometryFlags();
			if (!(flags & kGeometryInvisible))
			{
				UpdateMaxGeometryDepth(worldContext, terrain);
				SetNodeFogState(worldContext, terrain);

				if ((LightNodeVisible(terrain, regionArray)) && ((flags & (kGeometryAmbientOnly | kGeometryRenderEffectPass)) == 0))
				{
					terrain->SetLightStamp(lightStamp);
					renderStageList[kRenderStageLightDefault].Append(terrain);
				}
				else
				{
					renderStageList[kRenderStageAmbientDefault].Append(terrain);
				}

				if (level != 0)
				{
					TerrainLevelGeometry *terrainLevel = static_cast<TerrainLevelGeometry *>(terrain);
					terrainUpdateArray.AddElement(terrainLevel);
					terrainLevel->SetTerrainStamp(kTerrainStampAmbient, ambientStamp);
				}

				if (superTerrain)
				{
					superTerrain->SetSubterrainStamp(kTerrainStampAmbient, ambientStamp);
				}

				#if C4STATS

					worldCounter[kWorldCounterTerrain]++;

				#endif
			}
		}
		else if ((terrain->GetLightStamp() != lightStamp) && (LightNodeVisible(terrain, regionArray)))
		{
			unsigned_int32 flags = object->GetGeometryFlags();
			if ((flags & (kGeometryInvisible | kGeometryAmbientOnly | kGeometryRenderEffectPass)) == 0)
			{
				terrain->SetLightStamp(lightStamp);
				renderStageList[kRenderStageLightDefault].Append(terrain);
			}
		}
	}
	else
	{
		const FrustumCamera *camera = worldContext->renderCamera;
		Vector3D direction = terrain->GetWorldCenter() - camera->GetWorldPosition();
		float d = Magnitude(direction) / camera->GetObject()->GetFocalLength();

		if ((d > terrain->GetRenderDistance()) && (camera->GetWorldTransform()[2] * direction > 0.0F))
		{
			unsigned_int32 ambientStamp = ambientRenderStamp;
			if (terrain->GetSiteStamp() != ambientStamp)
			{
				terrain->SetSiteStamp(ambientStamp);

				unsigned_int32 flags = object->GetGeometryFlags();
				if (!(flags & kGeometryInvisible))
				{
					UpdateMaxGeometryDepth(worldContext, terrain);
					SetNodeFogState(worldContext, terrain);

					if ((LightNodeVisible(terrain, regionArray)) && ((flags & (kGeometryAmbientOnly | kGeometryRenderEffectPass)) == 0))
					{
						renderStageList[kRenderStageLightDefault].Append(terrain);
					}
					else
					{
						renderStageList[kRenderStageAmbientDefault].Append(terrain);
					}

					TerrainLevelGeometry *terrainLevel = static_cast<TerrainLevelGeometry *>(terrain);
					terrainUpdateArray.AddElement(terrainLevel);

					terrainLevel->SetTerrainStamp(kTerrainStampAmbient, ambientStamp);
					if (superTerrain)
					{
						superTerrain->SetSubterrainStamp(kTerrainStampAmbient, ambientStamp);
					}

					#if C4STATS

						worldCounter[kWorldCounterTerrain]++;

					#endif
				}
			}
		}
		else
		{
			terrain->SetSiteStamp(ambientRenderStamp);

			const Bond *bond = terrain->GetFirstOutgoingEdge();
			while (bond)
			{
				TerrainGeometry *subterrain = static_cast<TerrainGeometry *>(bond->GetFinishElement());
				if ((subterrain->Enabled()) && ((subterrain->GetPerspectiveExclusionMask() & worldContext->perspectiveFlags) == 0) && (subterrain->Visible(cameraRegion, &worldContext->occlusionList)))
				{
					RenderUnifiedTerrain(worldContext, cameraRegion, regionArray, subterrain, static_cast<TerrainLevelGeometry *>(terrain));
				}

				bond = bond->GetNextOutgoingEdge();
			}
		}
	}
}

void World::RenderUnifiedNode(WorldContext *worldContext, const CameraRegion *cameraRegion, const ImmutableArray<LightRegion *>& regionArray, Node *node)
{
	if ((node->Enabled()) && ((node->GetPerspectiveExclusionMask() & worldContext->perspectiveFlags) == 0))
	{
		NodeType type = node->GetNodeType();

		if (node->Visible(cameraRegion, &worldContext->occlusionList))
		{
			if (type == kNodeGeometry)
			{
				Geometry *geometry = static_cast<Geometry *>(node);
				if (geometry->GetGeometryType() != kGeometryTerrain)
				{
					RenderUnifiedGeometry(worldContext, cameraRegion, regionArray, geometry);
				}
				else
				{
					RenderUnifiedTerrain(worldContext, cameraRegion, regionArray, static_cast<TerrainGeometry *>(geometry), nullptr);
					return;
				}
			}
			else if (type == kNodeImpostor)
			{
				Impostor *impostor = static_cast<Impostor *>(node);
				unsigned_int32 ambientStamp = ambientRenderStamp;

				float distance = SquaredMag(impostor->GetWorldPosition().GetVector2D() - worldContext->renderCamera->GetWorldPosition().GetVector2D());
				if (distance > impostor->GetSquaredRenderDistance())
				{
					if (impostor->GetSiteStamp() != ambientStamp)
					{
						// Always assume that an impostor is lit because there is no good way to
						// remove it from the ambient pass if it was unlit in a different region.

						impostor->SetSiteStamp(ambientStamp);
						impostor->Render(kImpostorRenderLight);

						#if C4STATS

							worldCounter[kWorldCounterImpostor]++;

						#endif
					}

					if (distance > impostor->GetSquaredGeometryDistance())
					{
						return;
					}
				}
				else
				{
					node->SetSiteStamp(ambientStamp);
				}
			}
			else
			{
				node->SetSiteStamp(ambientRenderStamp);
			}

			const Bond *bond = node->GetFirstOutgoingEdge();
			while (bond)
			{
				RenderUnifiedNode(worldContext, cameraRegion, regionArray, static_cast<Node *>(bond->GetFinishElement()));
				bond = bond->GetNextOutgoingEdge();
			}
		}
		else if (type == kNodeGeometry)
		{
			Geometry *geometry = static_cast<Geometry *>(node);
			if (geometry->GetGeometryType() == kGeometryWater)
			{
				float d = SquaredMag(geometry->GetBoundingSphere()->GetCenter() - worldContext->renderCamera->GetWorldPosition());
				static_cast<WaterGeometry *>(geometry)->UpdateInvisibleWater(d);
			}
		}
	}
}

void World::RenderUnifiedCell(WorldContext *worldContext, const CameraRegion *cameraRegion, const ImmutableArray<LightRegion *>& regionArray, Site *cell)
{
	if (WorldBoundingBoxVisible(cell->GetWorldBoundingBox(), cameraRegion, &worldContext->occlusionList))
	{
		cell->SetSiteStamp(ambientRenderStamp);

		const Bond *bond = cell->GetFirstOutgoingEdge();
		while (bond)
		{
			Site *site = bond->GetFinishElement();
			if (site->GetCellIndex() < 0)
			{
				RenderUnifiedNode(worldContext, cameraRegion, regionArray, static_cast<Node *>(site));
			}
			else
			{
				RenderUnifiedCell(worldContext, cameraRegion, regionArray, site);
			}

			bond = bond->GetNextOutgoingEdge();
		}
	}
}

void World::RenderUnifiedEffectNode(const WorldContext *worldContext, const CameraRegion *cameraRegion, const ImmutableArray<LightRegion *>& regionArray, Effect *effect)
{
	unsigned_int32 ambientStamp = ambientRenderStamp;
	if (effect->GetSiteStamp() != ambientStamp)
	{
		if ((effect->Enabled()) && ((effect->GetPerspectiveExclusionMask() & worldContext->perspectiveFlags) == 0) && (effect->Visible(cameraRegion, &worldContext->occlusionList)))
		{
			effect->SetSiteStamp(ambientStamp);

			SetNodeFogState(worldContext, effect);
			effect->Render(worldContext->renderCamera, &renderStageList[kRenderStageFirstEffect]);

			if ((effect->GetEffectListIndex() == kEffectListLight) && (effect->Rendering()))
			{
				effect->SetRenderStamp(ambientStamp);
			}
		}
	}
}

void World::RenderUnifiedEffectCell(WorldContext *worldContext, const CameraRegion *cameraRegion, const ImmutableArray<LightRegion *>& regionArray, Site *cell)
{
	if (WorldBoundingBoxVisible(cell->GetWorldBoundingBox(), cameraRegion, &worldContext->occlusionList))
	{
		cell->SetSiteStamp(ambientRenderStamp);

		const Bond *bond = cell->GetFirstOutgoingEdge();
		while (bond)
		{
			Site *site = bond->GetFinishElement();
			if (site->GetCellIndex() < 0)
			{
				RenderUnifiedEffectNode(worldContext, cameraRegion, regionArray, static_cast<Effect *>(site));
			}
			else
			{
				RenderUnifiedEffectCell(worldContext, cameraRegion, regionArray, site);
			}

			bond = bond->GetNextOutgoingEdge();
		}
	}
}

void World::RenderUnifiedGeometry(WorldContext *worldContext, const CameraRegion *cameraRegion, Geometry *geometry)
{
	unsigned_int32 ambientStamp = ambientRenderStamp;
	unsigned_int32 lightStamp = lightRenderStamp;

	if (geometry->GetSiteStamp() != ambientStamp)
	{
		geometry->SetSiteStamp(ambientStamp);

		unsigned_int32 flags = geometry->GetObject()->GetGeometryFlags();
		if (!(flags & kGeometryInvisible))
		{
			geometry->SetProcessStamp(ambientRenderStamp);
			ProcessGeometry(worldContext, geometry);

			UpdateMaxGeometryDepth(worldContext, geometry);
			SetNodeFogState(worldContext, geometry);

			if ((flags & (kGeometryAmbientOnly | kGeometryRenderEffectPass)) == 0)
			{
				geometry->SetLightStamp(lightStamp);
				renderStageList[geometry->GetLightRenderStage()].Append(geometry);
			}
			else
			{
				renderStageList[geometry->GetAmbientRenderStage()].Append(geometry);
			}

			#if C4STATS

				worldCounter[kWorldCounterGeometry]++;

			#endif

			if (geometry->GetGeometryType() == kGeometryWater)
			{
				float d = SquaredMag(geometry->GetBoundingSphere()->GetCenter() - worldContext->renderCamera->GetWorldPosition());
				static_cast<WaterGeometry *>(geometry)->UpdateWater(d);

				#if C4STATS

					worldCounter[kWorldCounterWater]++;

				#endif
			}
		}
	}
	else if (geometry->GetLightStamp() != lightStamp)
	{
		unsigned_int32 flags = geometry->GetObject()->GetGeometryFlags();
		if ((flags & (kGeometryInvisible | kGeometryAmbientOnly | kGeometryRenderEffectPass)) == 0)
		{
			geometry->SetLightStamp(lightStamp);
			renderStageList[geometry->GetLightRenderStage()].Append(geometry);
		}
	}
}

void World::RenderUnifiedTerrain(WorldContext *worldContext, const CameraRegion *cameraRegion, TerrainGeometry *terrain, TerrainLevelGeometry *superTerrain)
{
	const TerrainGeometryObject *object = terrain->GetObject();
	int32 level = object->GetDetailLevel();

	if (level <= worldContext->cameraMinDetailLevel)
	{
		unsigned_int32 ambientStamp = ambientRenderStamp;
		unsigned_int32 lightStamp = lightRenderStamp;

		if (terrain->GetSiteStamp() != ambientStamp)
		{
			terrain->SetSiteStamp(ambientStamp);

			unsigned_int32 flags = object->GetGeometryFlags();
			if (!(flags & kGeometryInvisible))
			{
				UpdateMaxGeometryDepth(worldContext, terrain);
				SetNodeFogState(worldContext, terrain);

				if ((flags & (kGeometryAmbientOnly | kGeometryRenderEffectPass)) == 0)
				{
					terrain->SetLightStamp(lightStamp);
					renderStageList[kRenderStageLightDefault].Append(terrain);
				}
				else
				{
					renderStageList[kRenderStageAmbientDefault].Append(terrain);
				}

				if (level != 0)
				{
					TerrainLevelGeometry *terrainLevel = static_cast<TerrainLevelGeometry *>(terrain);
					terrainUpdateArray.AddElement(terrainLevel);
					terrainLevel->SetTerrainStamp(kTerrainStampAmbient, ambientStamp);
				}

				if (superTerrain)
				{
					superTerrain->SetSubterrainStamp(kTerrainStampAmbient, ambientStamp);
				}

				#if C4STATS

					worldCounter[kWorldCounterTerrain]++;

				#endif
			}
		}
		else if (terrain->GetLightStamp() != lightStamp)
		{
			unsigned_int32 flags = object->GetGeometryFlags();
			if ((flags & (kGeometryInvisible | kGeometryAmbientOnly | kGeometryRenderEffectPass)) == 0)
			{
				terrain->SetLightStamp(lightStamp);
				renderStageList[kRenderStageLightDefault].Append(terrain);
			}
		}
	}
	else
	{
		const FrustumCamera *camera = worldContext->renderCamera;
		Vector3D direction = terrain->GetWorldCenter() - camera->GetWorldPosition();
		float d = Magnitude(direction) / camera->GetObject()->GetFocalLength();

		if ((d > terrain->GetRenderDistance()) && (camera->GetWorldTransform()[2] * direction > 0.0F))
		{
			unsigned_int32 ambientStamp = ambientRenderStamp;
			if (terrain->GetSiteStamp() != ambientStamp)
			{
				terrain->SetSiteStamp(ambientStamp);

				unsigned_int32 flags = object->GetGeometryFlags();
				if (!(flags & kGeometryInvisible))
				{
					UpdateMaxGeometryDepth(worldContext, terrain);
					SetNodeFogState(worldContext, terrain);

					if ((flags & (kGeometryAmbientOnly | kGeometryRenderEffectPass)) == 0)
					{
						renderStageList[kRenderStageLightDefault].Append(terrain);
					}
					else
					{
						renderStageList[kRenderStageAmbientDefault].Append(terrain);
					}

					TerrainLevelGeometry *terrainLevel = static_cast<TerrainLevelGeometry *>(terrain);
					terrainUpdateArray.AddElement(terrainLevel);

					terrainLevel->SetTerrainStamp(kTerrainStampAmbient, ambientStamp);
					if (superTerrain)
					{
						superTerrain->SetSubterrainStamp(kTerrainStampAmbient, ambientStamp);
					}

					#if C4STATS

						worldCounter[kWorldCounterTerrain]++;

					#endif
				}
			}
		}
		else
		{
			terrain->SetSiteStamp(ambientRenderStamp);

			const Bond *bond = terrain->GetFirstOutgoingEdge();
			while (bond)
			{
				TerrainGeometry *subterrain = static_cast<TerrainGeometry *>(bond->GetFinishElement());
				if ((subterrain->Enabled()) && ((subterrain->GetPerspectiveExclusionMask() & worldContext->perspectiveFlags) == 0) && (subterrain->Visible(cameraRegion, &worldContext->occlusionList)))
				{
					RenderUnifiedTerrain(worldContext, cameraRegion, subterrain, static_cast<TerrainLevelGeometry *>(terrain));
				}

				bond = bond->GetNextOutgoingEdge();
			}
		}
	}
}

void World::RenderUnifiedNode(WorldContext *worldContext, const CameraRegion *cameraRegion, Node *node)
{
	if ((node->Enabled()) && ((node->GetPerspectiveExclusionMask() & worldContext->perspectiveFlags) == 0))
	{
		NodeType type = node->GetNodeType();

		if (node->Visible(cameraRegion, &worldContext->occlusionList))
		{
			if (type == kNodeGeometry)
			{
				Geometry *geometry = static_cast<Geometry *>(node);
				if (geometry->GetGeometryType() != kGeometryTerrain)
				{
					RenderUnifiedGeometry(worldContext, cameraRegion, geometry);
				}
				else
				{
					RenderUnifiedTerrain(worldContext, cameraRegion, static_cast<TerrainGeometry *>(geometry), nullptr);
					return;
				}
			}
			else if (type == kNodeImpostor)
			{
				Impostor *impostor = static_cast<Impostor *>(node);
				unsigned_int32 ambientStamp = ambientRenderStamp;

				float distance = SquaredMag(impostor->GetWorldPosition().GetVector2D() - worldContext->renderCamera->GetWorldPosition().GetVector2D());
				if (distance > impostor->GetSquaredRenderDistance())
				{
					if (impostor->GetSiteStamp() != ambientStamp)
					{
						impostor->SetSiteStamp(ambientStamp);
						impostor->Render(kImpostorRenderLight);

						#if C4STATS

							worldCounter[kWorldCounterImpostor]++;

						#endif
					}

					if (distance > impostor->GetSquaredGeometryDistance())
					{
						return;
					}
				}
				else
				{
					node->SetSiteStamp(ambientStamp);
				}
			}
			else
			{
				node->SetSiteStamp(ambientRenderStamp);
			}

			const Bond *bond = node->GetFirstOutgoingEdge();
			while (bond)
			{
				RenderUnifiedNode(worldContext, cameraRegion, static_cast<Node *>(bond->GetFinishElement()));
				bond = bond->GetNextOutgoingEdge();
			}
		}
		else if (type == kNodeGeometry)
		{
			Geometry *geometry = static_cast<Geometry *>(node);
			if (geometry->GetGeometryType() == kGeometryWater)
			{
				float d = SquaredMag(geometry->GetBoundingSphere()->GetCenter() - worldContext->renderCamera->GetWorldPosition());
				static_cast<WaterGeometry *>(geometry)->UpdateInvisibleWater(d);
			}
		}
	}
}

void World::RenderUnifiedCell(WorldContext *worldContext, const CameraRegion *cameraRegion, Site *cell)
{
	if (WorldBoundingBoxVisible(cell->GetWorldBoundingBox(), cameraRegion, &worldContext->occlusionList))
	{
		cell->SetSiteStamp(ambientRenderStamp);

		const Bond *bond = cell->GetFirstOutgoingEdge();
		while (bond)
		{
			Site *site = bond->GetFinishElement();
			if (site->GetCellIndex() < 0)
			{
				RenderUnifiedNode(worldContext, cameraRegion, static_cast<Node *>(site));
			}
			else
			{
				RenderUnifiedCell(worldContext, cameraRegion, site);
			}

			bond = bond->GetNextOutgoingEdge();
		}
	}
}

void World::RenderUnifiedEffectNode(const WorldContext *worldContext, const CameraRegion *cameraRegion, Effect *effect)
{
	unsigned_int32 ambientStamp = ambientRenderStamp;
	if (effect->GetSiteStamp() != ambientStamp)
	{
		if ((effect->Enabled()) && ((effect->GetPerspectiveExclusionMask() & worldContext->perspectiveFlags) == 0) && (effect->Visible(cameraRegion, &worldContext->occlusionList)))
		{
			effect->SetSiteStamp(ambientStamp);

			SetNodeFogState(worldContext, effect);
			effect->Render(worldContext->renderCamera, &renderStageList[kRenderStageFirstEffect]);

			if ((effect->GetEffectListIndex() == kEffectListLight) && (effect->Rendering()))
			{
				effect->SetRenderStamp(ambientStamp);
			}
		}
	}
}

void World::RenderUnifiedEffectCell(WorldContext *worldContext, const CameraRegion *cameraRegion, Site *cell)
{
	if (WorldBoundingBoxVisible(cell->GetWorldBoundingBox(), cameraRegion, &worldContext->occlusionList))
	{
		cell->SetSiteStamp(ambientRenderStamp);

		const Bond *bond = cell->GetFirstOutgoingEdge();
		while (bond)
		{
			Site *site = bond->GetFinishElement();
			if (site->GetCellIndex() < 0)
			{
				RenderUnifiedEffectNode(worldContext, cameraRegion, static_cast<Effect *>(site));
			}
			else
			{
				RenderUnifiedEffectCell(worldContext, cameraRegion, site);
			}

			bond = bond->GetNextOutgoingEdge();
		}
	}
}

void World::RenderUnifiedRegion(WorldContext *worldContext, InfiniteLight *light, const CameraRegion *cameraRegion)
{
	const Zone *zone = cameraRegion->GetZone();

	if (zone->GetObject()->GetZoneFlags() & kZoneRenderSkybox)
	{
		worldContext->skyboxFlag = true;
	}

	OcclusionRegion *unfoggedRegion = worldContext->unfoggedList.First();
	if ((unfoggedRegion) && (!zone->GetFirstFogSpace()) && (!zone->GetConnectedFogSpace()))
	{
		worldContext->unfoggedList.Remove(unfoggedRegion);
	}

	Array<LightRegion *, 8>		zoneRegionArray;

	int32 maxPlaneCount = -1;

	const ImmutableArray<LightRegion *>& activeLightRegionArray = light->GetActiveLightRegionArray();
	for (LightRegion *lightRegion : activeLightRegionArray)
	{
		if (lightRegion->GetZone() == zone)
		{
			maxPlaneCount = Max(maxPlaneCount, lightRegion->GetPlaneCount());
			zoneRegionArray.AddElement(lightRegion);
		}
	}

	if (maxPlaneCount == 0)
	{
		const Bond *bond = zone->GetCellGraphSite(kCellGraphGeometry)->GetFirstOutgoingEdge();
		while (bond)
		{
			Site *site = bond->GetFinishElement();
			if (site->GetCellIndex() < 0)
			{
				RenderUnifiedNode(worldContext, cameraRegion, static_cast<Node *>(site));
			}
			else
			{
				RenderUnifiedCell(worldContext, cameraRegion, site);
			}

			bond = bond->GetNextOutgoingEdge();
		}

		bond = zone->GetCellGraphSite(kCellGraphEffect)->GetFirstOutgoingEdge();
		while (bond)
		{
			Site *site = bond->GetFinishElement();
			if (site->GetCellIndex() < 0)
			{
				RenderUnifiedEffectNode(worldContext, cameraRegion, static_cast<Effect *>(site));
			}
			else
			{
				RenderUnifiedEffectCell(worldContext, cameraRegion, site);
			}

			bond = bond->GetNextOutgoingEdge();
		}
	}
	else if (maxPlaneCount > 0)
	{
		const Bond *bond = zone->GetCellGraphSite(kCellGraphGeometry)->GetFirstOutgoingEdge();
		while (bond)
		{
			Site *site = bond->GetFinishElement();
			if (site->GetCellIndex() < 0)
			{
				RenderUnifiedNode(worldContext, cameraRegion, zoneRegionArray, static_cast<Node *>(site));
			}
			else
			{
				RenderUnifiedCell(worldContext, cameraRegion, zoneRegionArray, site);
			}

			bond = bond->GetNextOutgoingEdge();
		}

		bond = zone->GetCellGraphSite(kCellGraphEffect)->GetFirstOutgoingEdge();
		while (bond)
		{
			Site *site = bond->GetFinishElement();
			if (site->GetCellIndex() < 0)
			{
				RenderUnifiedEffectNode(worldContext, cameraRegion, zoneRegionArray, static_cast<Effect *>(site));
			}
			else
			{
				RenderUnifiedEffectCell(worldContext, cameraRegion, zoneRegionArray, site);
			}

			bond = bond->GetNextOutgoingEdge();
		}
	}
	else
	{
		const Bond *bond = zone->GetCellGraphSite(kCellGraphGeometry)->GetFirstOutgoingEdge();
		while (bond)
		{
			Site *site = bond->GetFinishElement();
			if (site->GetCellIndex() < 0)
			{
				RenderAmbientNode(worldContext, cameraRegion, static_cast<Node *>(site));
			}
			else
			{
				RenderAmbientCell(worldContext, cameraRegion, site);
			}

			bond = bond->GetNextOutgoingEdge();
		}

		bond = zone->GetCellGraphSite(kCellGraphEffect)->GetFirstOutgoingEdge();
		while (bond)
		{
			Site *site = bond->GetFinishElement();
			if (site->GetCellIndex() < 0)
			{
				RenderAmbientEffectNode(worldContext, cameraRegion, static_cast<Effect *>(site));
			}
			else
			{
				RenderAmbientEffectCell(worldContext, cameraRegion, site);
			}

			bond = bond->GetNextOutgoingEdge();
		}
	}

	if (unfoggedRegion)
	{
		worldContext->unfoggedList.Append(unfoggedRegion);
	}

	CameraRegion *subregion = cameraRegion->GetFirstSubnode();
	while (subregion)
	{
		RenderUnifiedRegion(worldContext, light, subregion);
		subregion = subregion->Tree<CameraRegion>::Next();
	}
}

void World::RenderUnified(WorldContext *worldContext, const Camera *camera, InfiniteLight *light, bool *shadowFlag)
{
	lightRenderStamp++;

	Controller *controller = light->GetController();
	if ((controller) && (controller->GetControllerFlags() & kControllerUpdate))
	{
		controller->Update();
	}

	RootCameraRegion *cameraRegion = camera->GetFirstRootRegion();
	do
	{
		RenderUnifiedRegion(worldContext, light, cameraRegion);
		cameraRegion = cameraRegion->Next();
	} while (cameraRegion);

	const InfiniteLightObject *lightObject = light->GetObject();
	if ((!(lightObject->GetLightFlags() & kLightShadowInhibit)) && (worldContext->shadowEnableFlag))
	{
		*shadowFlag = true;
		const LightShadowData *shadowData = RenderInfiniteShadow(worldContext, light);
		TheGraphicsMgr->SetInfiniteLight(lightObject, light, shadowData, true);
	}
	else
	{
		*shadowFlag = false;
		TheGraphicsMgr->SetInfiniteLight(lightObject, light, nullptr, true);
	}

	light->ClearActiveRegions();

	#if C4STATS

		worldCounter[kWorldCounterInfiniteLight]++;

	#endif
}

void World::RenderInfiniteLightTerrain(const WorldContext *worldContext, const ImmutableArray<LightRegion *>& regionArray, TerrainGeometry *terrain)
{
	unsigned_int32 lightStamp = lightRenderStamp;
	if (terrain->GetLightStamp() != lightStamp)
	{
		const TerrainGeometryObject *object = terrain->GetObject();
		int32 level = object->GetDetailLevel();

		if (level <= worldContext->cameraMinDetailLevel)
		{
			terrain->SetLightStamp(lightStamp);

			if ((object->GetGeometryFlags() & (kGeometryInvisible | kGeometryAmbientOnly | kGeometryRenderEffectPass)) == 0)
			{
				renderStageList[kRenderStageLightDefault].Append(terrain);
			}
		}
		else
		{
			const FrustumCamera *camera = worldContext->renderCamera;
			Vector3D direction = terrain->GetWorldCenter() - camera->GetWorldPosition();
			float d = Magnitude(direction) / camera->GetObject()->GetFocalLength();

			if ((d > terrain->GetRenderDistance()) && (camera->GetWorldTransform()[2] * direction > 0.0F))
			{
				terrain->SetLightStamp(lightStamp);

				if ((object->GetGeometryFlags() & (kGeometryInvisible | kGeometryAmbientOnly | kGeometryRenderEffectPass)) == 0)
				{
					renderStageList[kRenderStageLightDefault].Append(terrain);
				}
			}
			else
			{
				const Bond *bond = terrain->GetFirstOutgoingEdge();
				while (bond)
				{
					Node *node = static_cast<Node *>(bond->GetFinishElement());
					if ((node->GetSiteStamp() == ambientRenderStamp) && (LightNodeVisible(node, regionArray)))
					{
						RenderInfiniteLightTerrain(worldContext, regionArray, static_cast<TerrainGeometry *>(node));
					}

					bond = bond->GetNextOutgoingEdge();
				}
			}
		}
	}
}

void World::RenderInfiniteLightNode(const WorldContext *worldContext, const ImmutableArray<LightRegion *>& regionArray, Node *node)
{
	if (LightNodeVisible(node, regionArray))
	{
		NodeType type = node->GetNodeType();
		if (type == kNodeGeometry)
		{
			Geometry *geometry = static_cast<Geometry *>(node);
			if (geometry->GetGeometryType() != kGeometryTerrain)
			{
				unsigned_int32 lightStamp = lightRenderStamp;
				if (geometry->GetLightStamp() != lightStamp)
				{
					geometry->SetLightStamp(lightStamp);

					unsigned_int32 flags = geometry->GetObject()->GetGeometryFlags();
					if ((flags & (kGeometryInvisible | kGeometryAmbientOnly | kGeometryRenderEffectPass)) == 0)
					{
						renderStageList[geometry->GetLightRenderStage()].Append(geometry);
					}
				}
			}
			else
			{
				RenderInfiniteLightTerrain(worldContext, regionArray, static_cast<TerrainGeometry *>(geometry));
				return;
			}
		}
		else if (type == kNodeImpostor)
		{
			Impostor *impostor = static_cast<Impostor *>(node);

			float distance = SquaredMag(impostor->GetWorldPosition().GetVector2D() - worldContext->renderCamera->GetWorldPosition().GetVector2D());
			if (distance > impostor->GetSquaredRenderDistance())
			{
				unsigned_int32 lightStamp = lightRenderStamp;
				if (impostor->GetLightStamp() != lightStamp)
				{
					impostor->SetLightStamp(lightStamp);
					impostor->Render(kImpostorRenderLight);
				}

				if (distance > impostor->GetSquaredGeometryDistance())
				{
					return;
				}
			}
		}

		const Bond *bond = node->GetFirstOutgoingEdge();
		while (bond)
		{
			Site *site = bond->GetFinishElement();
			if (site->GetSiteStamp() == ambientRenderStamp)
			{
				RenderInfiniteLightNode(worldContext, regionArray, static_cast<Node *>(site));
			}

			bond = bond->GetNextOutgoingEdge();
		}
	}
}

void World::RenderInfiniteLightCell(const WorldContext *worldContext, const ImmutableArray<LightRegion *>& regionArray, const Site *cell)
{
	const Box3D& box = cell->GetWorldBoundingBox();

	for (const LightRegion *lightRegion : regionArray)
	{
		if (lightRegion->BoxVisible(box))
		{
			const Bond *bond = cell->GetFirstOutgoingEdge();
			while (bond)
			{
				Site *site = bond->GetFinishElement();
				if (site->GetSiteStamp() == ambientRenderStamp)
				{
					if (site->GetCellIndex() < 0)
					{
						RenderInfiniteLightNode(worldContext, regionArray, static_cast<Node *>(site));
					}
					else
					{
						RenderInfiniteLightCell(worldContext, regionArray, site);
					}
				}

				bond = bond->GetNextOutgoingEdge();
			}

			break;
		}
	}
}

void World::RenderInfiniteLightEffectNode(const WorldContext *worldContext, const ImmutableArray<LightRegion *>& regionArray, Effect *effect)
{
	unsigned_int32 lightStamp = lightRenderStamp;
	if (effect->GetLightStamp() != lightStamp)
	{
		effect->SetLightStamp(lightStamp);

		if ((effect->GetRenderStamp() == ambientRenderStamp) && (LightNodeVisible(effect, regionArray)))
		{
			renderStageList[kRenderStageLightDefault].Append(effect);
		}
	}
}

void World::RenderInfiniteLightEffectCell(const WorldContext *worldContext, const ImmutableArray<LightRegion *>& regionArray, const Site *cell)
{
	const Box3D& box = cell->GetWorldBoundingBox();

	for (const LightRegion *lightRegion : regionArray)
	{
		if (lightRegion->BoxVisible(box))
		{
			const Bond *bond = cell->GetFirstOutgoingEdge();
			while (bond)
			{
				Site *site = bond->GetFinishElement();
				if (site->GetSiteStamp() == ambientRenderStamp)
				{
					if (site->GetCellIndex() < 0)
					{
						RenderInfiniteLightEffectNode(worldContext, regionArray, static_cast<Effect *>(site));
					}
					else
					{
						RenderInfiniteLightEffectCell(worldContext, regionArray, site);
					}
				}

				bond = bond->GetNextOutgoingEdge();
			}

			break;
		}
	}
}

void World::RenderInfiniteLightZone(const WorldContext *worldContext, const Zone *zone, const ImmutableArray<LightRegion *>& regionArray)
{
	const Bond *bond = zone->GetCellGraphSite(kCellGraphGeometry)->GetFirstOutgoingEdge();
	while (bond)
	{
		Site *site = bond->GetFinishElement();
		if (site->GetSiteStamp() == ambientRenderStamp)
		{
			if (site->GetCellIndex() < 0)
			{
				RenderInfiniteLightNode(worldContext, regionArray, static_cast<Node *>(site));
			}
			else
			{
				RenderInfiniteLightCell(worldContext, regionArray, site);
			}
		}

		bond = bond->GetNextOutgoingEdge();
	}

	bond = zone->GetCellGraphSite(kCellGraphEffect)->GetFirstOutgoingEdge();
	while (bond)
	{
		Site *site = bond->GetFinishElement();
		if (site->GetSiteStamp() == ambientRenderStamp)
		{
			if (site->GetCellIndex() < 0)
			{
				RenderInfiniteLightEffectNode(worldContext, regionArray, static_cast<Effect *>(site));
			}
			else
			{
				RenderInfiniteLightEffectCell(worldContext, regionArray, site);
			}
		}

		bond = bond->GetNextOutgoingEdge();
	}
}

void World::RenderInfiniteLightTerrain(const WorldContext *worldContext, TerrainGeometry *terrain)
{
	unsigned_int32 lightStamp = lightRenderStamp;
	if (terrain->GetLightStamp() != lightStamp)
	{
		const TerrainGeometryObject *object = terrain->GetObject();
		int32 level = object->GetDetailLevel();

		if (level <= worldContext->cameraMinDetailLevel)
		{
			terrain->SetLightStamp(lightStamp);

			if ((object->GetGeometryFlags() & (kGeometryInvisible | kGeometryAmbientOnly | kGeometryRenderEffectPass)) == 0)
			{
				renderStageList[kRenderStageLightDefault].Append(terrain);
			}
		}
		else
		{
			const FrustumCamera *camera = worldContext->renderCamera;
			Vector3D direction = terrain->GetWorldCenter() - camera->GetWorldPosition();
			float d = Magnitude(direction) / camera->GetObject()->GetFocalLength();

			if ((d > terrain->GetRenderDistance()) && (camera->GetWorldTransform()[2] * direction > 0.0F))
			{
				terrain->SetLightStamp(lightStamp);

				if ((object->GetGeometryFlags() & (kGeometryInvisible | kGeometryAmbientOnly | kGeometryRenderEffectPass)) == 0)
				{
					renderStageList[kRenderStageLightDefault].Append(terrain);
				}
			}
			else
			{
				const Bond *bond = terrain->GetFirstOutgoingEdge();
				while (bond)
				{
					Node *node = static_cast<Node *>(bond->GetFinishElement());
					if (node->GetSiteStamp() == ambientRenderStamp)
					{
						RenderInfiniteLightTerrain(worldContext, static_cast<TerrainGeometry *>(node));
					}

					bond = bond->GetNextOutgoingEdge();
				}
			}
		}
	}
}

void World::RenderInfiniteLightNode(const WorldContext *worldContext, Node *node)
{
	NodeType type = node->GetNodeType();
	if (type == kNodeGeometry)
	{
		Geometry *geometry = static_cast<Geometry *>(node);
		if (geometry->GetGeometryType() != kGeometryTerrain)
		{
			unsigned_int32 lightStamp = lightRenderStamp;
			if (geometry->GetLightStamp() != lightStamp)
			{
				geometry->SetLightStamp(lightStamp);

				unsigned_int32 flags = geometry->GetObject()->GetGeometryFlags();
				if ((flags & (kGeometryInvisible | kGeometryAmbientOnly | kGeometryRenderEffectPass)) == 0)
				{
					renderStageList[geometry->GetLightRenderStage()].Append(geometry);
				}
			}
		}
		else
		{
			RenderInfiniteLightTerrain(worldContext, static_cast<TerrainGeometry *>(geometry));
			return;
		}
	}
	else if (type == kNodeImpostor)
	{
		Impostor *impostor = static_cast<Impostor *>(node);

		float distance = SquaredMag(impostor->GetWorldPosition().GetVector2D() - worldContext->renderCamera->GetWorldPosition().GetVector2D());
		if (distance > impostor->GetSquaredRenderDistance())
		{
			unsigned_int32 lightStamp = lightRenderStamp;
			if (impostor->GetLightStamp() != lightStamp)
			{
				impostor->SetLightStamp(lightStamp);
				impostor->Render(kImpostorRenderLight);
			}

			if (distance > impostor->GetSquaredGeometryDistance())
			{
				return;
			}
		}
	}

	const Bond *bond = node->GetFirstOutgoingEdge();
	while (bond)
	{
		Site *site = bond->GetFinishElement();
		if (site->GetSiteStamp() == ambientRenderStamp)
		{
			RenderInfiniteLightNode(worldContext, static_cast<Node *>(site));
		}

		bond = bond->GetNextOutgoingEdge();
	}
}

void World::RenderInfiniteLightCell(const WorldContext *worldContext, const Site *cell)
{
	const Bond *bond = cell->GetFirstOutgoingEdge();
	while (bond)
	{
		Site *site = bond->GetFinishElement();
		if (site->GetSiteStamp() == ambientRenderStamp)
		{
			if (site->GetCellIndex() < 0)
			{
				RenderInfiniteLightNode(worldContext, static_cast<Node *>(site));
			}
			else
			{
				RenderInfiniteLightCell(worldContext, site);
			}
		}

		bond = bond->GetNextOutgoingEdge();
	}
}

void World::RenderInfiniteLightEffectNode(const WorldContext *worldContext, Effect *effect)
{
	unsigned_int32 lightStamp = lightRenderStamp;
	if (effect->GetLightStamp() != lightStamp)
	{
		effect->SetLightStamp(lightStamp);

		if (effect->GetRenderStamp() == ambientRenderStamp)
		{
			renderStageList[kRenderStageLightDefault].Append(effect);
		}
	}
}

void World::RenderInfiniteLightEffectCell(const WorldContext *worldContext, const Site *cell)
{
	const Bond *bond = cell->GetFirstOutgoingEdge();
	while (bond)
	{
		Site *site = bond->GetFinishElement();
		if (site->GetSiteStamp() == ambientRenderStamp)
		{
			if (site->GetCellIndex() < 0)
			{
				RenderInfiniteLightEffectNode(worldContext, static_cast<Effect *>(site));
			}
			else
			{
				RenderInfiniteLightEffectCell(worldContext, site);
			}
		}

		bond = bond->GetNextOutgoingEdge();
	}
}

void World::RenderInfiniteLightZone(const WorldContext *worldContext, const Zone *zone)
{
	const Bond *bond = zone->GetCellGraphSite(kCellGraphGeometry)->GetFirstOutgoingEdge();
	while (bond)
	{
		Site *site = bond->GetFinishElement();
		if (site->GetSiteStamp() == ambientRenderStamp)
		{
			if (site->GetCellIndex() < 0)
			{
				RenderInfiniteLightNode(worldContext, static_cast<Node *>(site));
			}
			else
			{
				RenderInfiniteLightCell(worldContext, site);
			}
		}

		bond = bond->GetNextOutgoingEdge();
	}

	bond = zone->GetCellGraphSite(kCellGraphEffect)->GetFirstOutgoingEdge();
	while (bond)
	{
		Site *site = bond->GetFinishElement();
		if (site->GetSiteStamp() == ambientRenderStamp)
		{
			if (site->GetCellIndex() < 0)
			{
				RenderInfiniteLightEffectNode(worldContext, static_cast<Effect *>(site));
			}
			else
			{
				RenderInfiniteLightEffectCell(worldContext, site);
			}
		}

		bond = bond->GetNextOutgoingEdge();
	}
}

void World::RenderInfiniteLight(const WorldContext *worldContext, InfiniteLight *light, bool *shadowFlag)
{
	const InfiniteLightObject *lightObject = light->GetObject();

	if ((!(lightObject->GetLightFlags() & kLightShadowInhibit)) && (worldContext->shadowEnableFlag))
	{
		*shadowFlag = true;
		const LightShadowData *shadowData = RenderInfiniteShadow(worldContext, light);
		TheGraphicsMgr->SetInfiniteLight(lightObject, light, shadowData);
	}
	else
	{
		*shadowFlag = false;
		TheGraphicsMgr->SetInfiniteLight(lightObject, light, nullptr);
	}

	Array<LightRegion *, 8>		zoneRegionArray;

	const ImmutableArray<LightRegion *>& activeLightRegionArray = light->GetActiveLightRegionArray();

	int32 regionCount = activeLightRegionArray.GetElementCount();
	for (machine a = 0; a < regionCount; a++)
	{
		LightRegion *lightRegion = activeLightRegionArray[a];
		if (lightRegion)
		{
			zoneRegionArray.AddElement(lightRegion);

			int32 maxPlaneCount = lightRegion->GetPlaneCount();
			const Zone *zone = lightRegion->GetZone();

			for (machine b = a + 1; b < regionCount; b++)
			{
				lightRegion = activeLightRegionArray[b];
				if ((lightRegion) && (lightRegion->GetZone() == zone))
				{
					maxPlaneCount = Max(maxPlaneCount, lightRegion->GetPlaneCount());
					zoneRegionArray.AddElement(lightRegion);
					activeLightRegionArray[b] = nullptr;
				}
			}

			if (maxPlaneCount == 0)
			{
				RenderInfiniteLightZone(worldContext, zone);
			}
			else
			{
				RenderInfiniteLightZone(worldContext, zone, zoneRegionArray);
			}

			zoneRegionArray.Clear();
		}
	}

	ImpostorSystem *system = impostorSystemMap.First();
	while (system)
	{
		system->Render(&lightImpostorSystemArray, kImpostorRenderLight);
		system = system->Next();
	}
}

void World::RenderPointLightTerrain(const WorldContext *worldContext, const ImmutableArray<LightRegion *>& regionArray, TerrainGeometry *terrain)
{
	unsigned_int32 lightStamp = lightRenderStamp;
	if (terrain->GetLightStamp() != lightStamp)
	{
		const TerrainGeometryObject *object = terrain->GetObject();
		int32 level = object->GetDetailLevel();

		if (level <= worldContext->cameraMinDetailLevel)
		{
			terrain->SetLightStamp(lightStamp);

			if ((object->GetGeometryFlags() & (kGeometryInvisible | kGeometryAmbientOnly | kGeometryRenderEffectPass)) == 0)
			{
				renderStageList[kRenderStageLightDefault].Append(terrain);
			}
		}
		else
		{
			const FrustumCamera *camera = worldContext->renderCamera;
			Vector3D direction = terrain->GetWorldCenter() - camera->GetWorldPosition();
			float d = Magnitude(direction) / camera->GetObject()->GetFocalLength();

			if ((d > terrain->GetRenderDistance()) && (camera->GetWorldTransform()[2] * direction > 0.0F))
			{
				terrain->SetLightStamp(lightStamp);

				if ((object->GetGeometryFlags() & (kGeometryInvisible | kGeometryAmbientOnly | kGeometryRenderEffectPass)) == 0)
				{
					renderStageList[kRenderStageLightDefault].Append(terrain);
				}
			}
			else
			{
				const Bond *bond = terrain->GetFirstOutgoingEdge();
				while (bond)
				{
					Node *node = static_cast<Node *>(bond->GetFinishElement());
					if ((node->GetSiteStamp() == ambientRenderStamp) && (LightNodeVisible(node, regionArray)))
					{
						RenderPointLightTerrain(worldContext, regionArray, static_cast<TerrainGeometry *>(node));
					}

					bond = bond->GetNextOutgoingEdge();
				}
			}
		}
	}
}

void World::RenderPointLightNode(const WorldContext *worldContext, const PointLight *light, const ImmutableArray<LightRegion *>& regionArray, Node *node)
{
	if ((node != light->GetExcludedNode()) && (node->GetWorldBoundingBox().Intersection(light->GetWorldBoundingBox())) && (LightNodeVisible(node, regionArray)))
	{
		NodeType type = node->GetNodeType();
		if (type == kNodeGeometry)
		{
			Geometry *geometry = static_cast<Geometry *>(node);
			if (geometry->GetGeometryType() != kGeometryTerrain)
			{
				unsigned_int32 lightStamp = lightRenderStamp;
				if (geometry->GetLightStamp() != lightStamp)
				{
					geometry->SetLightStamp(lightStamp);

					if (!geometry->GetObject()->ExteriorSphere(geometry->GetInverseWorldTransform() * light->GetWorldPosition(), light->GetObject()->GetLightRange()))
					{
						unsigned_int32 flags = geometry->GetObject()->GetGeometryFlags();
						if ((flags & (kGeometryInvisible | kGeometryAmbientOnly | kGeometryRenderEffectPass)) == 0)
						{
							renderStageList[geometry->GetLightRenderStage()].Append(geometry);
						}
					}
				}
			}
			else
			{
				RenderPointLightTerrain(worldContext, regionArray, static_cast<TerrainGeometry *>(geometry));
				return;
			}
		}
		else if (type == kNodeImpostor)
		{
			Impostor *impostor = static_cast<Impostor *>(node);

			float distance = SquaredMag(impostor->GetWorldPosition().GetVector2D() - worldContext->renderCamera->GetWorldPosition().GetVector2D());
			if (distance > impostor->GetSquaredRenderDistance())
			{
				unsigned_int32 lightStamp = lightRenderStamp;
				if (impostor->GetLightStamp() != lightStamp)
				{
					impostor->SetLightStamp(lightStamp);
					impostor->Render(kImpostorRenderLight);
				}

				if (distance > impostor->GetSquaredGeometryDistance())
				{
					return;
				}
			}
		}

		const Bond *bond = node->GetFirstOutgoingEdge();
		while (bond)
		{
			Site *site = bond->GetFinishElement();
			if (site->GetSiteStamp() == ambientRenderStamp)
			{
				RenderPointLightNode(worldContext, light, regionArray, static_cast<Node *>(site));
			}

			bond = bond->GetNextOutgoingEdge();
		}
	}
}

void World::RenderPointLightCell(const WorldContext *worldContext, const PointLight *light, const ImmutableArray<LightRegion *>& regionArray, const Site *cell)
{
	const Box3D& box = cell->GetWorldBoundingBox();
	if (box.Intersection(light->GetWorldBoundingBox()))
	{
		for (const LightRegion *lightRegion : regionArray)
		{
			if (lightRegion->BoxVisible(box))
			{
				const Bond *bond = cell->GetFirstOutgoingEdge();
				while (bond)
				{
					Site *site = bond->GetFinishElement();
					if (site->GetSiteStamp() == ambientRenderStamp)
					{
						if (site->GetCellIndex() < 0)
						{
							RenderPointLightNode(worldContext, light, regionArray, static_cast<Node *>(site));
						}
						else
						{
							RenderPointLightCell(worldContext, light, regionArray, site);
						}
					}

					bond = bond->GetNextOutgoingEdge();
				}

				break;
			}
		}
	}
}

void World::RenderPointLightEffectNode(const WorldContext *worldContext, const PointLight *light, const ImmutableArray<LightRegion *>& regionArray, Effect *effect)
{
	unsigned_int32 lightStamp = lightRenderStamp;
	if (effect->GetLightStamp() != lightStamp)
	{
		effect->SetLightStamp(lightStamp);

		if ((effect->GetRenderStamp() == ambientRenderStamp) && (effect != light->GetExcludedNode()) && (effect->GetWorldBoundingBox().Intersection(light->GetWorldBoundingBox())) && (LightNodeVisible(effect, regionArray)))
		{
			renderStageList[kRenderStageLightDefault].Append(effect);
		}
	}
}

void World::RenderPointLightEffectCell(const WorldContext *worldContext, const PointLight *light, const ImmutableArray<LightRegion *>& regionArray, const Site *cell)
{
	const Box3D& box = cell->GetWorldBoundingBox();
	if (box.Intersection(light->GetWorldBoundingBox()))
	{
		for (const LightRegion *lightRegion : regionArray)
		{
			if (lightRegion->BoxVisible(box))
			{
				const Bond *bond = cell->GetFirstOutgoingEdge();
				while (bond)
				{
					Site *site = bond->GetFinishElement();
					if (site->GetSiteStamp() == ambientRenderStamp)
					{
						if (site->GetCellIndex() < 0)
						{
							RenderPointLightEffectNode(worldContext, light, regionArray, static_cast<Effect *>(site));
						}
						else
						{
							RenderPointLightEffectCell(worldContext, light, regionArray, site);
						}
					}

					bond = bond->GetNextOutgoingEdge();
				}

				break;
			}
		}
	}
}

void World::RenderPointLightZone(const WorldContext *worldContext, const PointLight *light, const Zone *zone, const ImmutableArray<LightRegion *>& regionArray)
{
	const Bond *bond = zone->GetCellGraphSite(kCellGraphGeometry)->GetFirstOutgoingEdge();
	while (bond)
	{
		Site *site = bond->GetFinishElement();
		if (site->GetSiteStamp() == ambientRenderStamp)
		{
			if (site->GetCellIndex() < 0)
			{
				RenderPointLightNode(worldContext, light, regionArray, static_cast<Node *>(site));
			}
			else
			{
				RenderPointLightCell(worldContext, light, regionArray, site);
			}
		}

		bond = bond->GetNextOutgoingEdge();
	}

	bond = zone->GetCellGraphSite(kCellGraphEffect)->GetFirstOutgoingEdge();
	while (bond)
	{
		Site *site = bond->GetFinishElement();
		if (site->GetSiteStamp() == ambientRenderStamp)
		{
			if (site->GetCellIndex() < 0)
			{
				RenderPointLightEffectNode(worldContext, light, regionArray, static_cast<Effect *>(site));
			}
			else
			{
				RenderPointLightEffectCell(worldContext, light, regionArray, site);
			}
		}

		bond = bond->GetNextOutgoingEdge();
	}
}

void World::RenderPointLightTerrain(const WorldContext *worldContext, TerrainGeometry *terrain)
{
	unsigned_int32 lightStamp = lightRenderStamp;
	if (terrain->GetLightStamp() != lightStamp)
	{
		const TerrainGeometryObject *object = terrain->GetObject();
		int32 level = object->GetDetailLevel();

		if (level <= worldContext->cameraMinDetailLevel)
		{
			terrain->SetLightStamp(lightStamp);

			if ((object->GetGeometryFlags() & (kGeometryInvisible | kGeometryAmbientOnly | kGeometryRenderEffectPass)) == 0)
			{
				renderStageList[kRenderStageLightDefault].Append(terrain);
			}
		}
		else
		{
			const FrustumCamera *camera = worldContext->renderCamera;
			Vector3D direction = terrain->GetWorldCenter() - camera->GetWorldPosition();
			float d = Magnitude(direction) / camera->GetObject()->GetFocalLength();

			if ((d > terrain->GetRenderDistance()) && (camera->GetWorldTransform()[2] * direction > 0.0F))
			{
				terrain->SetLightStamp(lightStamp);

				if ((object->GetGeometryFlags() & (kGeometryInvisible | kGeometryAmbientOnly | kGeometryRenderEffectPass)) == 0)
				{
					renderStageList[kRenderStageLightDefault].Append(terrain);
				}
			}
			else
			{
				const Bond *bond = terrain->GetFirstOutgoingEdge();
				while (bond)
				{
					Node *node = static_cast<Node *>(bond->GetFinishElement());
					if (node->GetSiteStamp() == ambientRenderStamp)
					{
						RenderPointLightTerrain(worldContext, static_cast<TerrainGeometry *>(node));
					}

					bond = bond->GetNextOutgoingEdge();
				}
			}
		}
	}
}

void World::RenderPointLightNode(const WorldContext *worldContext, const PointLight *light, Node *node)
{
	if ((node != light->GetExcludedNode()) && (node->GetWorldBoundingBox().Intersection(light->GetWorldBoundingBox())))
	{
		NodeType type = node->GetNodeType();
		if (type == kNodeGeometry)
		{
			Geometry *geometry = static_cast<Geometry *>(node);
			if (geometry->GetGeometryType() != kGeometryTerrain)
			{
				unsigned_int32 lightStamp = lightRenderStamp;
				if (geometry->GetLightStamp() != lightStamp)
				{
					geometry->SetLightStamp(lightStamp);

					if (!geometry->GetObject()->ExteriorSphere(geometry->GetInverseWorldTransform() * light->GetWorldPosition(), light->GetObject()->GetLightRange()))
					{
						unsigned_int32 flags = geometry->GetObject()->GetGeometryFlags();
						if ((flags & (kGeometryInvisible | kGeometryAmbientOnly | kGeometryRenderEffectPass)) == 0)
						{
							renderStageList[geometry->GetLightRenderStage()].Append(geometry);
						}
					}
				}
			}
			else
			{
				RenderPointLightTerrain(worldContext, static_cast<TerrainGeometry *>(geometry));
				return;
			}
		}
		else if (type == kNodeImpostor)
		{
			Impostor *impostor = static_cast<Impostor *>(node);

			float distance = SquaredMag(impostor->GetWorldPosition().GetVector2D() - worldContext->renderCamera->GetWorldPosition().GetVector2D());
			if (distance > impostor->GetSquaredRenderDistance())
			{
				unsigned_int32 lightStamp = lightRenderStamp;
				if (impostor->GetLightStamp() != lightStamp)
				{
					impostor->SetLightStamp(lightStamp);
					impostor->Render(kImpostorRenderLight);
				}

				if (distance > impostor->GetSquaredGeometryDistance())
				{
					return;
				}
			}
		}

		const Bond *bond = node->GetFirstOutgoingEdge();
		while (bond)
		{
			Site *site = bond->GetFinishElement();
			if (site->GetSiteStamp() == ambientRenderStamp)
			{
				RenderPointLightNode(worldContext, light, static_cast<Node *>(site));
			}

			bond = bond->GetNextOutgoingEdge();
		}
	}
}

void World::RenderPointLightCell(const WorldContext *worldContext, const PointLight *light, const Site *cell)
{
	const Box3D& box = cell->GetWorldBoundingBox();
	if (box.Intersection(light->GetWorldBoundingBox()))
	{
		const Bond *bond = cell->GetFirstOutgoingEdge();
		while (bond)
		{
			Site *site = bond->GetFinishElement();
			if (site->GetSiteStamp() == ambientRenderStamp)
			{
				if (site->GetCellIndex() < 0)
				{
					RenderPointLightNode(worldContext, light, static_cast<Node *>(site));
				}
				else
				{
					RenderPointLightCell(worldContext, light, site);
				}
			}

			bond = bond->GetNextOutgoingEdge();
		}
	}
}

void World::RenderPointLightEffectNode(const WorldContext *worldContext, const PointLight *light, Effect *effect)
{
	unsigned_int32 lightStamp = lightRenderStamp;
	if (effect->GetLightStamp() != lightStamp)
	{
		effect->SetLightStamp(lightStamp);

		if ((effect->GetRenderStamp() == ambientRenderStamp) && (effect != light->GetExcludedNode()) && (effect->GetWorldBoundingBox().Intersection(light->GetWorldBoundingBox())))
		{
			renderStageList[kRenderStageLightDefault].Append(effect);
		}
	}
}

void World::RenderPointLightEffectCell(const WorldContext *worldContext, const PointLight *light, const Site *cell)
{
	const Box3D& box = cell->GetWorldBoundingBox();
	if (box.Intersection(light->GetWorldBoundingBox()))
	{
		const Bond *bond = cell->GetFirstOutgoingEdge();
		while (bond)
		{
			Site *site = bond->GetFinishElement();
			if (site->GetSiteStamp() == ambientRenderStamp)
			{
				if (site->GetCellIndex() < 0)
				{
					RenderPointLightEffectNode(worldContext, light, static_cast<Effect *>(site));
				}
				else
				{
					RenderPointLightEffectCell(worldContext, light, site);
				}
			}

			bond = bond->GetNextOutgoingEdge();
		}
	}
}

void World::RenderPointLightZone(const WorldContext *worldContext, const PointLight *light, const Zone *zone)
{
	const Bond *bond = zone->GetCellGraphSite(kCellGraphGeometry)->GetFirstOutgoingEdge();
	while (bond)
	{
		Site *site = bond->GetFinishElement();
		if (site->GetSiteStamp() == ambientRenderStamp)
		{
			if (site->GetCellIndex() < 0)
			{
				RenderPointLightNode(worldContext, light, static_cast<Node *>(site));
			}
			else
			{
				RenderPointLightCell(worldContext, light, site);
			}
		}

		bond = bond->GetNextOutgoingEdge();
	}

	bond = zone->GetCellGraphSite(kCellGraphEffect)->GetFirstOutgoingEdge();
	while (bond)
	{
		Site *site = bond->GetFinishElement();
		if (site->GetSiteStamp() == ambientRenderStamp)
		{
			if (site->GetCellIndex() < 0)
			{
				RenderPointLightEffectNode(worldContext, light, static_cast<Effect *>(site));
			}
			else
			{
				RenderPointLightEffectCell(worldContext, light, site);
			}
		}

		bond = bond->GetNextOutgoingEdge();
	}
}

bool World::RenderPointLight(const WorldContext *worldContext, PointLight *light, bool *shadowFlag)
{
	Rect			lightBounds;
	Range<float>	depthBounds;

	// The GraphicsMgr::CalculatePointLightBounds() function returns kProjectionEmpty if the projection of
	// the point light does not intersect the current viewport rectangle, in which case we skip the light altogether.

	const PointLightObject *lightObject = light->GetObject();

	ProjectionResult projection = TheGraphicsMgr->CalculatePointLightBounds(lightObject, light, &lightBounds, &depthBounds);
	if (projection != kProjectionEmpty)
	{
		if ((!(lightObject->GetLightFlags() & kLightShadowInhibit)) && (worldContext->shadowEnableFlag))
		{
			*shadowFlag = true;
			RenderPointShadow(worldContext, static_cast<PointLight *>(light));
			TheGraphicsMgr->SetPointLight(lightObject, light, light->GetColorMultiplier(), projection, &lightBounds, &depthBounds, true);
		}
		else
		{
			*shadowFlag = false;
			TheGraphicsMgr->SetPointLight(lightObject, light, light->GetColorMultiplier(), projection, &lightBounds, &depthBounds, false);
		}

		const ImmutableArray<LightRegion *>& activeLightRegionArray = light->GetActiveLightRegionArray();

		int32 regionCount = activeLightRegionArray.GetElementCount();
		for (machine a = 0; a < regionCount; a++)
		{
			Array<LightRegion *, 8>		zoneRegionArray;

			LightRegion *lightRegion = activeLightRegionArray[a];
			if (lightRegion)
			{
				zoneRegionArray.AddElement(lightRegion);

				int32 maxPlaneCount = lightRegion->GetPlaneCount();
				const Zone *zone = lightRegion->GetZone();

				for (machine b = a + 1; b < regionCount; b++)
				{
					lightRegion = activeLightRegionArray[b];
					if ((lightRegion) && (lightRegion->GetZone() == zone))
					{
						maxPlaneCount = Max(maxPlaneCount, lightRegion->GetPlaneCount());
						zoneRegionArray.AddElement(lightRegion);
						activeLightRegionArray[b] = nullptr;
					}
				}

				if (maxPlaneCount == 0)
				{
					RenderPointLightZone(worldContext, light, zone);
				}
				else
				{
					RenderPointLightZone(worldContext, light, zone, zoneRegionArray);
				}
			}
		}

		ImpostorSystem *system = impostorSystemMap.First();
		while (system)
		{
			system->Render(&lightImpostorSystemArray, kImpostorRenderLight);
			system = system->Next();
		}

		return (true);
	}

	return (false);
}

bool World::RenderSpotLight(const WorldContext *worldContext, SpotLight *light, bool *shadowFlag)
{
	Rect			lightBounds;
	Range<float>	depthBounds;

	// The GraphicsMgr::CalculatePointLightBounds() function returns kProjectionEmpty if the projection of
	// the point light does not intersect the current viewport rectangle, in which case we skip the light altogether.

	const SpotLightObject *lightObject = light->GetObject();

	ProjectionResult projection = TheGraphicsMgr->CalculatePointLightBounds(lightObject, light, &lightBounds, &depthBounds);
	if (projection != kProjectionEmpty)
	{
		if ((!(lightObject->GetLightFlags() & kLightShadowInhibit)) && (worldContext->shadowEnableFlag))
		{
			*shadowFlag = true;
			RenderSpotShadow(worldContext, static_cast<SpotLight *>(light));
			TheGraphicsMgr->SetSpotLight(lightObject, light, light->GetColorMultiplier(), projection, &lightBounds, &depthBounds, true);
		}
		else
		{
			*shadowFlag = false;
			TheGraphicsMgr->SetSpotLight(lightObject, light, light->GetColorMultiplier(), projection, &lightBounds, &depthBounds, false);
		}

		const ImmutableArray<LightRegion *>& activeLightRegionArray = light->GetActiveLightRegionArray();

		int32 regionCount = activeLightRegionArray.GetElementCount();
		for (machine a = 0; a < regionCount; a++)
		{
			Array<LightRegion *, 8>		zoneRegionArray;

			LightRegion *lightRegion = activeLightRegionArray[a];
			if (lightRegion)
			{
				zoneRegionArray.AddElement(lightRegion);

				int32 maxPlaneCount = lightRegion->GetPlaneCount();
				const Zone *zone = lightRegion->GetZone();

				for (machine b = a + 1; b < regionCount; b++)
				{
					lightRegion = activeLightRegionArray[b];
					if ((lightRegion) && (lightRegion->GetZone() == zone))
					{
						maxPlaneCount = Max(maxPlaneCount, lightRegion->GetPlaneCount());
						zoneRegionArray.AddElement(lightRegion);
						activeLightRegionArray[b] = nullptr;
					}
				}

				if (maxPlaneCount == 0)
				{
					RenderPointLightZone(worldContext, light, zone);
				}
				else
				{
					RenderPointLightZone(worldContext, light, zone, zoneRegionArray);
				}
			}
		}

		ImpostorSystem *system = impostorSystemMap.First();
		while (system)
		{
			system->Render(&lightImpostorSystemArray, kImpostorRenderLight);
			system = system->Next();
		}

		return (true);
	}

	return (false);
}

void World::CalculateInfiniteShadowRegion(const CameraRegion *cameraRegion, const Polyhedron *cameraPolyhedron, const Vector3D& lightDirection, ShadowRegion *shadowRegion)
{
	bool	frontArray[kMaxPolyhedronFaceCount];

	Convexity *shadowConvexity = shadowRegion->GetRegionConvexity();

	int32 shadowPlaneCount = 0;
	Antivector4D *shadowPlane = shadowConvexity->plane;

	int32 cameraPlaneCount = cameraPolyhedron->planeCount;
	for (machine a = 0; a < cameraPlaneCount; a++)
	{
		const Antivector4D& plane = cameraPolyhedron->plane[a];
		bool front = ((plane ^ lightDirection) > 0.0F);
		frontArray[a] = front;

		if (front)
		{
			shadowPlane[shadowPlaneCount++] = plane;
		}
	}

	const Point3D *cameraVertex = cameraPolyhedron->vertex;
	const Edge *cameraEdge = cameraPolyhedron->edge;
	int32 cameraEdgeCount = cameraPolyhedron->edgeCount;

	for (machine a = 0; a < cameraEdgeCount; a++)
	{
		bool front1 = frontArray[cameraEdge->faceIndex[0]];
		bool front2 = frontArray[cameraEdge->faceIndex[1]];

		if (front1 ^ front2)
		{
			const Point3D& p1 = cameraVertex[cameraEdge->vertexIndex[0]];
			const Point3D& p2 = cameraVertex[cameraEdge->vertexIndex[1]];

			Antivector4D& plane = shadowPlane[shadowPlaneCount];
			if (front1)
			{
				plane = Bivector4D(p2, p1) ^ lightDirection;
			}
			else
			{
				plane = Bivector4D(p1, p2) ^ lightDirection;
			}

			float d = SquaredMag(plane.GetAntivector3D());
			if (d > kShadowRegionEpsilon)
			{
				plane *= InverseSqrt(d);

				if (++shadowPlaneCount == kMaxPolyhedronFaceCount)
				{
					break;
				}
			}
		}

		cameraEdge++;
	}

	shadowConvexity->planeCount = shadowPlaneCount;
}

void World::CalculatePointShadowRegion(const CameraRegion *cameraRegion, const Point3D& lightPosition, ShadowRegion *shadowRegion)
{
	if (cameraRegion->ContainsPoint(lightPosition))
	{
		*shadowRegion->GetRegionConvexity() = *cameraRegion->GetRegionPolyhedron();
	}
	else
	{
		bool	frontArray[kMaxPolyhedronFaceCount];

		int32 shadowPlaneCount = 0;
		Convexity *shadowConvexity = shadowRegion->GetRegionConvexity();
		Antivector4D *shadowPlane = shadowConvexity->plane;

		const Polyhedron *cameraPolyhedron = cameraRegion->GetRegionPolyhedron();
		int32 cameraPlaneCount = cameraPolyhedron->planeCount;

		for (machine a = 0; a < cameraPlaneCount; a++)
		{
			const Antivector4D& plane = cameraPolyhedron->plane[a];
			bool front = ((plane ^ lightPosition) > 0.0F);
			frontArray[a] = front;

			if (front)
			{
				shadowPlane[shadowPlaneCount++] = plane;
			}
		}

		const Point3D *cameraVertex = cameraPolyhedron->vertex;
		const Edge *cameraEdge = cameraPolyhedron->edge;
		int32 cameraEdgeCount = cameraPolyhedron->edgeCount;

		for (machine a = 0; a < cameraEdgeCount; a++)
		{
			bool front1 = frontArray[cameraEdge->faceIndex[0]];
			bool front2 = frontArray[cameraEdge->faceIndex[1]];

			if (front1 ^ front2)
			{
				const Point3D& p1 = cameraVertex[cameraEdge->vertexIndex[0]];
				const Point3D& p2 = cameraVertex[cameraEdge->vertexIndex[1]];

				Antivector4D& plane = shadowPlane[shadowPlaneCount];
				if (front1)
				{
					plane = Bivector4D(p2, p1) ^ lightPosition;
				}
				else
				{
					plane = Bivector4D(p1, p2) ^ lightPosition;
				}

				float d = SquaredMag(plane.GetAntivector3D());
				if (d > kShadowRegionEpsilon)
				{
					plane *= InverseSqrt(d);

					if (++shadowPlaneCount == kMaxPolyhedronFaceCount)
					{
						break;
					}
				}
			}

			cameraEdge++;
		}

		shadowConvexity->planeCount = shadowPlaneCount;
	}
}

void World::RenderInfiniteShadowTerrain(const WorldContext *worldContext, int32 cascade, TerrainGeometry *terrain, TerrainLevelGeometry *superTerrain, const ShadowRegion *shadowRegion, const List<OcclusionRegion> *occlusionList)
{
	const TerrainGeometryObject *object = terrain->GetObject();
	int32 level = object->GetDetailLevel();

	if (level <= worldContext->cameraMinDetailLevel)
	{
		unsigned_int32 shadowStamp = shadowRenderStamp;
		if (terrain->GetShadowStamp() != shadowStamp)
		{
			terrain->SetShadowStamp(shadowStamp);

			if (object->GetGeometryFlags() & kGeometryCastShadows)
			{
				renderStageArray[kRenderStageShadowDefault + cascade].AddElement(terrain);

				unsigned_int32 terrainStamp = shadowTerrainStamp;
				if (level != 0)
				{
					TerrainLevelGeometry *terrainLevel = static_cast<TerrainLevelGeometry *>(terrain);
					if (terrainLevel->GetTerrainStamp(kTerrainStampShadow) != terrainStamp)
					{
						terrainLevel->SetTerrainStamp(kTerrainStampShadow, terrainStamp);
						terrainShadowUpdateArray.AddElement(terrainLevel);
					}
				}

				if (superTerrain)
				{
					superTerrain->SetSubterrainStamp(kTerrainStampShadow, terrainStamp);
				}

				#if C4STATS

					worldCounter[kWorldCounterInfiniteShadowGeometry]++;

				#endif
			}
		}
	}
	else
	{
		const FrustumCamera *camera = worldContext->renderCamera;
		Vector3D direction = terrain->GetWorldCenter() - camera->GetWorldPosition();
		float d = Magnitude(direction) / camera->GetObject()->GetFocalLength();

		if ((d > terrain->GetRenderDistance()) && (camera->GetWorldTransform()[2] * direction > 0.0F))
		{
			unsigned_int32 shadowStamp = shadowRenderStamp;
			if (terrain->GetShadowStamp() != shadowStamp)
			{
				terrain->SetShadowStamp(shadowStamp);

				if (object->GetGeometryFlags() & kGeometryCastShadows)
				{
					renderStageArray[kRenderStageShadowDefault + cascade].AddElement(terrain);

					unsigned_int32 terrainStamp = shadowTerrainStamp;
					TerrainLevelGeometry *terrainLevel = static_cast<TerrainLevelGeometry *>(terrain);
					if (terrainLevel->GetTerrainStamp(kTerrainStampShadow) != terrainStamp)
					{
						terrainLevel->SetTerrainStamp(kTerrainStampShadow, terrainStamp);
						if (superTerrain)
						{
							superTerrain->SetSubterrainStamp(kTerrainStampShadow, terrainStamp);
						}

						terrainShadowUpdateArray.AddElement(terrainLevel);
					}

					#if C4STATS

						worldCounter[kWorldCounterInfiniteShadowGeometry]++;

					#endif
				}
			}
		}
		else
		{
			const Bond *bond = terrain->GetFirstOutgoingEdge();
			while (bond)
			{
				TerrainGeometry *subterrain = static_cast<TerrainGeometry *>(bond->GetFinishElement());
				if ((subterrain->Enabled()) && (((subterrain->GetPerspectiveExclusionMask() >> kPerspectiveDirectShadowShift) & worldContext->perspectiveFlags) == 0) && (subterrain->Visible(shadowRegion, occlusionList)))
				{
					RenderInfiniteShadowTerrain(worldContext, cascade, subterrain, static_cast<TerrainLevelGeometry *>(terrain), shadowRegion, occlusionList);
				}

				bond = bond->GetNextOutgoingEdge();
			}
		}
	}
}

void World::RenderInfiniteShadowNode(const WorldContext *worldContext, int32 cascade, Node *node, const ShadowRegion *shadowRegion, const List<OcclusionRegion> *occlusionList)
{
	if ((node->Enabled()) && (((node->GetPerspectiveExclusionMask() >> kPerspectiveDirectShadowShift) & worldContext->perspectiveFlags) == 0) && (node->Visible(shadowRegion, occlusionList)))
	{
		NodeType type = node->GetNodeType();
		if (type == kNodeGeometry)
		{
			Geometry *geometry = static_cast<Geometry *>(node);

			if (geometry->GetGeometryType() != kGeometryTerrain)
			{
				unsigned_int32 shadowStamp = shadowRenderStamp;
				if (geometry->GetShadowStamp() != shadowStamp)
				{
					geometry->SetShadowStamp(shadowStamp);

					if (geometry->GetObject()->GetGeometryFlags() & kGeometryCastShadows)
					{
						unsigned_int32 ambientStamp = ambientRenderStamp;
						if (geometry->GetProcessStamp() != ambientStamp)
						{
							geometry->SetProcessStamp(ambientStamp);
							ProcessGeometry(worldContext, geometry);
						}

						renderStageArray[geometry->GetShadowRenderStage() + cascade].AddElement(geometry);

						#if C4STATS

							worldCounter[kWorldCounterInfiniteShadowGeometry]++;

						#endif
					}
				}
			}
			else
			{
				RenderInfiniteShadowTerrain(worldContext, cascade, static_cast<TerrainGeometry *>(geometry), nullptr, shadowRegion, occlusionList);
				return;
			}
		}
		else if (type == kNodeImpostor)
		{
			Impostor *impostor = static_cast<Impostor *>(node);

			float distance = SquaredMag(impostor->GetWorldPosition().GetVector2D() - worldContext->renderCamera->GetWorldPosition().GetVector2D());
			if (distance > impostor->GetSquaredRenderDistance())
			{
				unsigned_int32 shadowStamp = shadowRenderStamp;
				if (impostor->GetShadowStamp() != shadowStamp)
				{
					impostor->SetShadowStamp(shadowStamp);
					impostorRenderArray[cascade].AddElement(impostor);
				}

				if (distance > impostor->GetSquaredGeometryDistance())
				{
					return;
				}
			}
		}

		const Bond *bond = node->GetFirstOutgoingEdge();
		while (bond)
		{
			RenderInfiniteShadowNode(worldContext, cascade, static_cast<Node *>(bond->GetFinishElement()), shadowRegion, occlusionList);
			bond = bond->GetNextOutgoingEdge();
		}
	}
}

void World::RenderInfiniteShadowCell(const WorldContext *worldContext, int32 cascade, const Site *cell, const ShadowRegion *shadowRegion, const List<OcclusionRegion> *occlusionList)
{
	const Box3D& box = cell->GetWorldBoundingBox();
	if (shadowRegion->BoxVisible(box))
	{
		const OcclusionRegion *occluder = occlusionList->First();
		while (occluder)
		{
			if (occluder->BoxOccluded(box))
			{
				return;
			}

			occluder = occluder->Next();
		}

		const Bond *bond = cell->GetFirstOutgoingEdge();
		while (bond)
		{
			Site *site = bond->GetFinishElement();
			if (site->GetCellIndex() < 0)
			{
				RenderInfiniteShadowNode(worldContext, cascade, static_cast<Node *>(site), shadowRegion, occlusionList);
			}
			else
			{
				RenderInfiniteShadowCell(worldContext, cascade, site, shadowRegion, occlusionList);
			}

			bond = bond->GetNextOutgoingEdge();
		}
	}
}

void World::RenderInfiniteShadowRegion(const WorldContext *worldContext, int32 cascade, const ShadowRegion *shadowRegion, const List<OcclusionRegion> *occlusionList)
{
	const Bond *bond = shadowRegion->GetZone()->GetCellGraphSite(kCellGraphGeometry)->GetFirstOutgoingEdge();
	while (bond)
	{
		Site *site = bond->GetFinishElement();
		if (site->GetCellIndex() < 0)
		{
			RenderInfiniteShadowNode(worldContext, cascade, static_cast<Node *>(site), shadowRegion, occlusionList);
		}
		else
		{
			RenderInfiniteShadowCell(worldContext, cascade, site, shadowRegion, occlusionList);
		}

		bond = bond->GetNextOutgoingEdge();
	}
}

void World::RenderInfiniteShadowCascade(const WorldContext *worldContext, InfiniteLight *light, int32 cascade, const LightShadowData *shadowData)
{
	OrthoCamera		orthoCamera;

	orthoCamera.SetWorld(this);
	OrthoCameraObject *cameraObject = orthoCamera.GetObject();

	int32 shadowMapSize = TheGraphicsMgr->GetInfiniteShadowMapSize();
	cameraObject->SetViewRect(Rect(0, 0, shadowMapSize, shadowMapSize));
	cameraObject->SetNearDepth(0.0F);
	cameraObject->SetFarDepth(shadowData->shadowSize.z);

	float w = shadowData->shadowSize.x * 0.5F;
	float h = shadowData->shadowSize.y * 0.5F;
	cameraObject->SetOrthoRect(-w, w, -h, h);

	const Transform4D& transform = light->GetWorldTransform();
	Point3D position(shadowData->shadowPosition.x, shadowData->shadowPosition.y, shadowData->shadowPosition.z + shadowData->texelSize * 2.0F);
	orthoCamera.SetNodeTransform(-transform[0], transform[1], -transform[2], transform * position);
	orthoCamera.Update();

	TheGraphicsMgr->SetInfiniteShadowCascade(cascade);
	TheGraphicsMgr->SetCamera(cameraObject, &orthoCamera, 0, false);

	TheGraphicsMgr->DrawShadow(renderStageArray[kRenderStageShadowDefault + cascade]);
	renderStageArray[kRenderStageShadowDefault + cascade].Clear();

	TheGraphicsMgr->DrawShadow(renderStageArray[kRenderStageShadowAlphaTest + cascade]);
	renderStageArray[kRenderStageShadowAlphaTest + cascade].Clear();

	for (const Impostor *impostor : impostorRenderArray[cascade])
	{
		impostor->Render(kImpostorRenderShadow);
	}

	impostorRenderArray[cascade].Clear();

	ImpostorSystem *system = impostorSystemMap.First();
	while (system)
	{
		system->Render(&shadowImpostorSystemArray, kImpostorRenderShadow);
		system = system->Next();
	}

	TheGraphicsMgr->DrawShadow(shadowImpostorSystemArray);
	shadowImpostorSystemArray.Clear();
}

const LightShadowData *World::RenderInfiniteShadow(const WorldContext *worldContext, InfiniteLight *light)
{
	const FrustumCamera *camera = worldContext->renderCamera;
	const LightShadowData *shadowData = light->CalculateShadowData(camera);

	if (!worldContext->reusableShadowFlag)
	{
		Antivector4D	extentPlane[2];
		bool			cascadeRenderFlag[kMaxShadowCascadeCount];

		TheGraphicsMgr->BeginInfiniteShadow();

		const Transform4D& cameraTransform = camera->GetWorldTransform();
		const Vector3D& lightDirection = light->GetWorldTransform()[2];
		Vector2D cameraLightDirection = camera->GetInverseWorldTransform() * lightDirection.GetVector2D();

		if (cameraLightDirection.x > 0.0F)
		{
			extentPlane[0].Set(cameraTransform[0], -(camera->GetWorldPosition() * cameraTransform[0]) - worldContext->maxGeometryExtent[0].min);
		}
		else
		{
			extentPlane[0].Set(-cameraTransform[0], camera->GetWorldPosition() * cameraTransform[0] + worldContext->maxGeometryExtent[0].max);
		}

		if (cameraLightDirection.y > 0.0F)
		{
			extentPlane[1].Set(cameraTransform[1], -(camera->GetWorldPosition() * cameraTransform[1]) - worldContext->maxGeometryExtent[1].min);
		}
		else
		{
			extentPlane[1].Set(-cameraTransform[1], camera->GetWorldPosition() * cameraTransform[1] + worldContext->maxGeometryExtent[1].max);
		}

		shadowTerrainStamp++;
		for (machine a = 0; a < kMaxShadowCascadeCount; a++)
		{
			cascadeRenderFlag[a] = false;
		}

		for (machine a = 0; a < kMaxShadowCascadeCount; a++)
		{
			if (a > 0)
			{
				const Point3D *polygon = shadowData[a].cascadePolygon;

				const OcclusionRegion *occluder = worldContext->occlusionList.First();
				while (occluder)
				{
					if ((occluder->GetOcclusionMask() & kOcclusionCascade) && (occluder->QuadOccluded(polygon)))
					{
						goto end;
					}

					occluder = occluder->Next();
				}
			}

			if (shadowData[a].minDepth < worldContext->maxGeometryDepth)
			{
				List<ShadowRegion>		shadowRegionList;

				const Antivector4D& nearPlane = shadowData[a].nearPlane;
				const Antivector4D& farPlane = shadowData[a].farPlane;

				const ImmutableArray<CameraRegion *>& cameraRegionArray = light->GetActiveCameraRegionArray();
				for (const CameraRegion *cameraRegion : cameraRegionArray)
				{
					Polyhedron		polyhedron[2];

					const Polyhedron *cameraPolyhedron = cameraRegion->GetRegionPolyhedron();
					if (a > 0)
					{
						if (!Math::ClipPolyhedron(cameraPolyhedron, nearPlane, &polyhedron[0]))
						{
							continue;
						}

						if (!Math::ClipPolyhedron(&polyhedron[0], farPlane, &polyhedron[1]))
						{
							continue;
						}
					}
					else
					{
						if (!Math::ClipPolyhedron(cameraPolyhedron, farPlane, &polyhedron[1]))
						{
							continue;
						}
					}

					if (!Math::ClipPolyhedron(&polyhedron[1], extentPlane[0], &polyhedron[0]))
					{
						continue;
					}

					if (!Math::ClipPolyhedron(&polyhedron[0], extentPlane[1], &polyhedron[1]))
					{
						continue;
					}

					#if C4DIAGS

						if (diagnosticFlags & kDiagnosticShadowRegions)
						{
							RegionRenderable *renderable = new RegionRenderable(&polyhedron[1]);
							if (renderable->GetPrimitiveCount() != 0)
							{
								shadowRegionDiagnosticList.Append(renderable);
							}
							else
							{
								delete renderable;
							}
						}

					#endif

					ShadowRegion *shadowRegion = new ShadowRegion(cameraRegion->GetZone());
					shadowRegionList.Append(shadowRegion);

					CalculateInfiniteShadowRegion(cameraRegion, &polyhedron[1], lightDirection, shadowRegion);

					const ImmutableArray<const LightRegion *>& lightRegionArray = cameraRegion->GetShadowCastingLightRegionArray();
					for (const LightRegion *lightRegion : lightRegionArray)
					{
						if (lightRegion->GetLight() == light)
						{
							const LightRegion *subregion = lightRegion;
							const LightRegion *superRegion = subregion->GetSuperNode();
							while (superRegion)
							{
								const Portal *portal = subregion->GetIlluminatedPortal();
								const PortalObject *object = portal->GetObject();

								unsigned_int32 portalFlags = object->GetPortalFlags();
								if (portalFlags & kPortalShadowMapInhibit)
								{
									break;
								}

								Zone *zone = superRegion->GetZone();
								if (zone->GetTraversalExclusionMask() != 0)
								{
									break;
								}

								zone->SetTraversalExclusionMask(kZoneTraversalLocal);

								ShadowRegion *superShadowRegion = new ShadowRegion(zone, shadowRegion);
								shadowRegionList.Append(superShadowRegion);

								if (!(portalFlags & kPortalFullShadowRegion))
								{
									// Add the extruded portal planes to the set of planes in the original shadow region.

									Convexity *convexity = superShadowRegion->GetRegionConvexity();
									Antivector4D *plane = &convexity->plane[convexity->planeCount];

									int32 count = object->GetVertexCount();
									convexity->planeCount += count;

									const Point3D *vertex = portal->GetWorldVertexArray();
									const Point3D *v1 = &vertex[count - 1];
									for (machine b = 0; b < count; b++)
									{
										const Point3D *v2 = &vertex[b];
										plane[b].Set(Normalize(lightDirection % (*v2 - *v1)), *v2);
										v1 = v2;
									}
								}

								subregion = superRegion;
								superRegion = superRegion->GetSuperNode();
							}
						}
					}
				}

				if (!shadowRegionList.Empty())
				{
					shadowRenderStamp++;
					cascadeRenderFlag[a] = true;

					const ShadowRegion *shadowRegion = shadowRegionList.First();
					do
					{
						List<OcclusionRegion>	occlusionList;

						shadowRegion->GetZone()->SetTraversalExclusionMask(0);
						RenderInfiniteShadowRegion(worldContext, a, shadowRegion, &occlusionList);

						shadowRegion = shadowRegion->GetNextShadowRegion();
					} while (shadowRegion);

					#if C4STATS

						worldCounter[kWorldCounterInfiniteShadowCascade]++;

					#endif
				}
			}
		}

		end:
		unsigned_int32 shadowStamp = shadowTerrainStamp;
		for (TerrainLevelGeometry *terrain : terrainShadowUpdateArray)
		{
			terrain->UpdateBorderState(kTerrainStampShadow, shadowStamp);
		}

		terrainShadowUpdateArray.Clear();

		FinishWorldBatch();

		for (machine a = 0; a < kMaxShadowCascadeCount; a++)
		{
			if (cascadeRenderFlag[a])
			{
				RenderInfiniteShadowCascade(worldContext, light, a, &shadowData[a]);
			}
		}

		TheGraphicsMgr->EndInfiniteShadow();

		#if C4STATS

			worldCounter[kWorldCounterInfiniteShadow]++;

		#endif
	}

	return (shadowData);
}

void World::RenderPointShadowTerrain(const WorldContext *worldContext, TerrainGeometry *terrain, TerrainLevelGeometry *superTerrain, const Box3D& lightBox, const LightRegion *lightRegion, const ShadowRegion *shadowRegion, const List<OcclusionRegion> *occlusionList)
{
	const TerrainGeometryObject *object = terrain->GetObject();
	int32 level = object->GetDetailLevel();

	if (level <= worldContext->cameraMinDetailLevel)
	{
		unsigned_int32 shadowStamp = shadowRenderStamp;
		if (terrain->GetShadowStamp() != shadowStamp)
		{
			terrain->SetShadowStamp(shadowStamp);

			if (object->GetGeometryFlags() & kGeometryCastShadows)
			{
				renderStageArray[kRenderStageShadowDefault].AddElement(terrain);

				unsigned_int32 terrainStamp = shadowTerrainStamp;
				if (level != 0)
				{
					TerrainLevelGeometry *terrainLevel = static_cast<TerrainLevelGeometry *>(terrain);
					terrainLevel->SetTerrainStamp(kTerrainStampShadow, terrainStamp);
					terrainShadowUpdateArray.AddElement(terrainLevel);
				}

				if (superTerrain)
				{
					superTerrain->SetSubterrainStamp(kTerrainStampShadow, terrainStamp);
				}

				#if C4STATS

					worldCounter[kWorldCounterPointShadowGeometry]++;

				#endif
			}
		}
	}
	else
	{
		const FrustumCamera *camera = worldContext->renderCamera;
		Vector3D direction = terrain->GetWorldCenter() - camera->GetWorldPosition();
		float d = Magnitude(direction) / camera->GetObject()->GetFocalLength();

		if ((d > terrain->GetRenderDistance()) && (camera->GetWorldTransform()[2] * direction > 0.0F))
		{
			unsigned_int32 shadowStamp = shadowRenderStamp;
			if (terrain->GetShadowStamp() != shadowStamp)
			{
				terrain->SetShadowStamp(shadowStamp);

				if (object->GetGeometryFlags() & kGeometryCastShadows)
				{
					renderStageArray[kRenderStageShadowDefault].AddElement(terrain);

					unsigned_int32 terrainStamp = shadowTerrainStamp;
					TerrainLevelGeometry *terrainLevel = static_cast<TerrainLevelGeometry *>(terrain);
					terrainLevel->SetTerrainStamp(kTerrainStampShadow, terrainStamp);
					if (superTerrain)
					{
						superTerrain->SetSubterrainStamp(kTerrainStampShadow, terrainStamp);
					}

					terrainShadowUpdateArray.AddElement(terrainLevel);

					#if C4STATS

						worldCounter[kWorldCounterPointShadowGeometry]++;

					#endif
				}
			}
		}
		else
		{
			const Bond *bond = terrain->GetFirstOutgoingEdge();
			while (bond)
			{
				TerrainGeometry *subterrain = static_cast<TerrainGeometry *>(bond->GetFinishElement());
				if ((subterrain->GetWorldBoundingBox().Intersection(lightBox)) && (subterrain->Enabled()) && (((subterrain->GetPerspectiveExclusionMask() >> kPerspectiveDirectShadowShift) & worldContext->perspectiveFlags) == 0) && (subterrain->Visible(lightRegion)) && (subterrain->Visible(shadowRegion, occlusionList)))
				{
					RenderPointShadowTerrain(worldContext, subterrain, static_cast<TerrainLevelGeometry *>(terrain), lightBox, lightRegion, shadowRegion, occlusionList);
				}

				bond = bond->GetNextOutgoingEdge();
			}
		}
	}
}

void World::RenderPointShadowNode(const WorldContext *worldContext, Node *node, const Node *excludedNode, const Box3D& lightBox, const LightRegion *lightRegion, const ShadowRegion *shadowRegion, const List<OcclusionRegion> *occlusionList)
{
	if ((node->GetWorldBoundingBox().Intersection(lightBox)) && (node != excludedNode))
	{
		if ((node->Enabled()) && (((node->GetPerspectiveExclusionMask() >> kPerspectiveDirectShadowShift) & worldContext->perspectiveFlags) == 0) && (node->Visible(lightRegion)) && (node->Visible(shadowRegion, occlusionList)))
		{
			if (node->GetNodeType() == kNodeGeometry)
			{
				Geometry *geometry = static_cast<Geometry *>(node);

				if (geometry->GetGeometryType() != kGeometryTerrain)
				{
					unsigned_int32 shadowStamp = shadowRenderStamp;
					if (geometry->GetShadowStamp() != shadowStamp)
					{
						geometry->SetShadowStamp(shadowStamp);

						if (geometry->GetObject()->GetGeometryFlags() & kGeometryCastShadows)
						{
							unsigned_int32 ambientStamp = ambientRenderStamp;
							if (geometry->GetProcessStamp() != ambientStamp)
							{
								geometry->SetProcessStamp(ambientStamp);
								ProcessGeometry(worldContext, geometry);
							}

							renderStageArray[geometry->GetShadowRenderStage()].AddElement(geometry);

							#if C4STATS

								worldCounter[kWorldCounterPointShadowGeometry]++;

							#endif
						}
					}
				}
				else
				{
					RenderPointShadowTerrain(worldContext, static_cast<TerrainGeometry *>(geometry), nullptr, lightBox, lightRegion, shadowRegion, occlusionList);
					return;
				}
			}

			const Bond *bond = node->GetFirstOutgoingEdge();
			while (bond)
			{
				RenderPointShadowNode(worldContext, static_cast<Node *>(bond->GetFinishElement()), excludedNode, lightBox, lightRegion, shadowRegion, occlusionList);
				bond = bond->GetNextOutgoingEdge();
			}
		}
	}
}

void World::RenderPointShadowCell(const WorldContext *worldContext, const Site *cell, const Node *excludedNode, const Box3D& lightBox, const LightRegion *lightRegion, const ShadowRegion *shadowRegion, const List<OcclusionRegion> *occlusionList)
{
	const Box3D& box = cell->GetWorldBoundingBox();
	if ((box.Intersection(lightBox)) && (lightRegion->BoxVisible(box)) && (shadowRegion->BoxVisible(box)))
	{
		const Bond *bond = cell->GetFirstOutgoingEdge();
		while (bond)
		{
			Site *site = bond->GetFinishElement();
			if (site->GetCellIndex() < 0)
			{
				RenderPointShadowNode(worldContext, static_cast<Node *>(site), excludedNode, lightBox, lightRegion, shadowRegion, occlusionList);
			}
			else
			{
				RenderPointShadowCell(worldContext, site, excludedNode, lightBox, lightRegion, shadowRegion, occlusionList);
			}

			bond = bond->GetNextOutgoingEdge();
		}
	}
}

void World::RenderPointShadowRegion(const WorldContext *worldContext, const Node *excludedNode, const Box3D& lightBox, const LightRegion *lightRegion, const ShadowRegion *shadowRegion, const List<OcclusionRegion> *occlusionList)
{
	const Bond *bond = shadowRegion->GetZone()->GetCellGraphSite(kCellGraphGeometry)->GetFirstOutgoingEdge();
	while (bond)
	{
		Site *site = bond->GetFinishElement();
		if (site->GetCellIndex() < 0)
		{
			RenderPointShadowNode(worldContext, static_cast<Node *>(site), excludedNode, lightBox, lightRegion, shadowRegion, occlusionList);
		}
		else
		{
			RenderPointShadowCell(worldContext, site, excludedNode, lightBox, lightRegion, shadowRegion, occlusionList);
		}

		bond = bond->GetNextOutgoingEdge();
	}
}

void World::RenderPointShadow(const WorldContext *worldContext, const PointLight *light)
{
	List<ShadowRegion>		shadowRegionList;

	TheGraphicsMgr->BeginPointShadow();

	const Point3D& lightPosition = light->GetWorldPosition();

	const ImmutableArray<CameraRegion *>& cameraRegionArray = light->GetActiveCameraRegionArray();
	for (const CameraRegion *cameraRegion : cameraRegionArray)
	{
		ShadowRegion *shadowRegion = new ShadowRegion(cameraRegion->GetZone());
		shadowRegionList.Append(shadowRegion);

		CalculatePointShadowRegion(cameraRegion, lightPosition, shadowRegion);

		const ImmutableArray<const LightRegion *>& lightRegionArray = cameraRegion->GetShadowCastingLightRegionArray();
		for (const LightRegion *lightRegion : lightRegionArray)
		{
			if (lightRegion->GetLight() == light)
			{
				const LightRegion *subregion = lightRegion;
				const LightRegion *superRegion = subregion->GetSuperNode();
				while (superRegion)
				{
					const Portal *portal = subregion->GetIlluminatedPortal();
					const PortalObject *object = portal->GetObject();

					if (object->GetPortalFlags() & kPortalShadowMapInhibit)
					{
						break;
					}

					ShadowRegion *superShadowRegion = new ShadowRegion(superRegion->GetZone(), shadowRegion);
					shadowRegionList.Append(superShadowRegion);

					// Add the extruded portal planes to the set of planes in the original shadow region.

					Convexity *convexity = superShadowRegion->GetRegionConvexity();
					Antivector4D *plane = &convexity->plane[convexity->planeCount];

					int32 count = object->GetVertexCount();
					convexity->planeCount += count;

					const Point3D *vertex = portal->GetWorldVertexArray();
					const Point3D *v1 = &vertex[count - 1];
					for (machine a = 0; a < count; a++)
					{
						const Point3D *v2 = &vertex[a];
						plane[a].SetStandard(lightPosition, *v2, *v1);
						v1 = v2;
					}

					subregion = superRegion;
					superRegion = superRegion->GetSuperNode();
				}
			}
		}
	}

	const Point3D& center = light->GetWorldPosition();
	const PointLightObject *lightObject = light->GetObject();
	float range = lightObject->GetLightRange();
	Box3D lightBox(Point3D(center.x - range, center.y - range, center.z - range), Point3D(center.x + range, center.y + range, center.z + range));

	CubeCamera		cubeCamera;

	cubeCamera.SetWorld(this);
	cubeCamera.SetNodeTransform(light->GetWorldTransform());

	FrustumCameraObject *cameraObject = cubeCamera.GetObject();
	int32 shadowMapSize = TheGraphicsMgr->GetPointShadowMapSize();
	cameraObject->SetViewRect(Rect(0, 0, shadowMapSize, shadowMapSize));
	cameraObject->SetNearDepth(lightObject->GetMinShadowDistance());
	cameraObject->SetFarDepth(range);

	const RootCameraRegion *rootCameraRegion = worldContext->renderCamera->GetFirstRootRegion();
	bool renderAllFlag = rootCameraRegion->ContainsPoint(center);

	const Node *excludedNode = light->GetExcludedNode();
	const Point3D *cubeVertexArray = &static_cast<const FrustumCamera&>(cubeCamera).GetFrustumVertexArray()[4];

	for (machine face = 0; face < 6; face++)
	{
		cubeCamera.SetFaceIndex(face);
		cubeCamera.Invalidate();
		cubeCamera.Update();

		if ((renderAllFlag) || (rootCameraRegion->PyramidVisible(center, 4, cubeVertexArray)))
		{
			List<OcclusionRegion>	occlusionList;

			shadowRenderStamp++;
			shadowTerrainStamp++;

			TheGraphicsMgr->SetPointShadowFace(face);
			TheGraphicsMgr->SetCamera(cameraObject, &cubeCamera, 0, false);

			const ShadowRegion *shadowRegion = shadowRegionList.First();
			while (shadowRegion)
			{
				RenderPointShadowRegion(worldContext, excludedNode, lightBox, light->GetFirstRootRegion(), shadowRegion, &occlusionList);
				shadowRegion = shadowRegion->GetNextShadowRegion();
			}

			unsigned_int32 shadowStamp = shadowTerrainStamp;
			for (TerrainLevelGeometry *terrain : terrainShadowUpdateArray)
			{
				terrain->UpdateBorderState(kTerrainStampShadow, shadowStamp);
			}

			terrainShadowUpdateArray.Clear();

			FinishWorldBatch();

			TheGraphicsMgr->DrawShadow(renderStageArray[kRenderStageShadowDefault]);
			renderStageArray[kRenderStageShadowDefault].Clear();

			TheGraphicsMgr->DrawShadow(renderStageArray[kRenderStageShadowAlphaTest]);
			renderStageArray[kRenderStageShadowAlphaTest].Clear();

			#if C4STATS

				worldCounter[kWorldCounterPointShadowFace]++;

			#endif
		}
	}

	TheGraphicsMgr->EndPointShadow();

	#if C4STATS

		worldCounter[kWorldCounterPointShadow]++;

	#endif
}

void World::RenderSpotShadowTerrain(const WorldContext *worldContext, TerrainGeometry *terrain, TerrainLevelGeometry *superTerrain, const LightRegion *lightRegion, const ShadowRegion *shadowRegion, const List<OcclusionRegion> *occlusionList)
{
	const TerrainGeometryObject *object = terrain->GetObject();
	int32 level = object->GetDetailLevel();

	if (level <= worldContext->cameraMinDetailLevel)
	{
		unsigned_int32 shadowStamp = shadowRenderStamp;
		if (terrain->GetShadowStamp() != shadowStamp)
		{
			terrain->SetShadowStamp(shadowStamp);

			if (object->GetGeometryFlags() & kGeometryCastShadows)
			{
				renderStageArray[kRenderStageShadowDefault].AddElement(terrain);

				unsigned_int32 terrainStamp = shadowTerrainStamp;
				if (level != 0)
				{
					TerrainLevelGeometry *terrainLevel = static_cast<TerrainLevelGeometry *>(terrain);
					terrainLevel->SetTerrainStamp(kTerrainStampShadow, terrainStamp);
					terrainShadowUpdateArray.AddElement(terrainLevel);
				}

				if (superTerrain)
				{
					superTerrain->SetSubterrainStamp(kTerrainStampShadow, terrainStamp);
				}

				#if C4STATS

					worldCounter[kWorldCounterSpotShadowGeometry]++;

				#endif
			}
		}
	}
	else
	{
		const FrustumCamera *camera = worldContext->renderCamera;
		Vector3D direction = terrain->GetWorldCenter() - camera->GetWorldPosition();
		float d = Magnitude(direction) / camera->GetObject()->GetFocalLength();

		if ((d > terrain->GetRenderDistance()) && (camera->GetWorldTransform()[2] * direction > 0.0F))
		{
			unsigned_int32 shadowStamp = shadowRenderStamp;
			if (terrain->GetShadowStamp() != shadowStamp)
			{
				terrain->SetShadowStamp(shadowStamp);

				if (object->GetGeometryFlags() & kGeometryCastShadows)
				{
					renderStageArray[kRenderStageShadowDefault].AddElement(terrain);

					unsigned_int32 terrainStamp = shadowTerrainStamp;
					TerrainLevelGeometry *terrainLevel = static_cast<TerrainLevelGeometry *>(terrain);
					terrainLevel->SetTerrainStamp(kTerrainStampShadow, terrainStamp);
					if (superTerrain)
					{
						superTerrain->SetSubterrainStamp(kTerrainStampShadow, terrainStamp);
					}

					terrainShadowUpdateArray.AddElement(terrainLevel);

					#if C4STATS

						worldCounter[kWorldCounterSpotShadowGeometry]++;

					#endif
				}
			}
		}
		else
		{
			const Bond *bond = terrain->GetFirstOutgoingEdge();
			while (bond)
			{
				TerrainGeometry *subterrain = static_cast<TerrainGeometry *>(bond->GetFinishElement());
				if ((subterrain->Enabled()) && (((subterrain->GetPerspectiveExclusionMask() >> kPerspectiveDirectShadowShift) & worldContext->perspectiveFlags) == 0) && (subterrain->Visible(lightRegion)) && (subterrain->Visible(shadowRegion, occlusionList)))
				{
					RenderSpotShadowTerrain(worldContext, subterrain, static_cast<TerrainLevelGeometry *>(terrain), lightRegion, shadowRegion, occlusionList);
				}

				bond = bond->GetNextOutgoingEdge();
			}
		}
	}
}

void World::RenderSpotShadowNode(const WorldContext *worldContext, Node *node, const Node *excludedNode, const LightRegion *lightRegion, const ShadowRegion *shadowRegion, const List<OcclusionRegion> *occlusionList)
{
	if (node != excludedNode)
	{
		if ((node->Enabled()) && (((node->GetPerspectiveExclusionMask() >> kPerspectiveDirectShadowShift) & worldContext->perspectiveFlags) == 0) && (node->Visible(lightRegion)) && (node->Visible(shadowRegion, occlusionList)))
		{
			if (node->GetNodeType() == kNodeGeometry)
			{
				Geometry *geometry = static_cast<Geometry *>(node);

				if (geometry->GetGeometryType() != kGeometryTerrain)
				{
					unsigned_int32 shadowStamp = shadowRenderStamp;
					if (geometry->GetShadowStamp() != shadowStamp)
					{
						geometry->SetShadowStamp(shadowStamp);

						if (geometry->GetObject()->GetGeometryFlags() & kGeometryCastShadows)
						{
							unsigned_int32 ambientStamp = ambientRenderStamp;
							if (geometry->GetProcessStamp() != ambientStamp)
							{
								geometry->SetProcessStamp(ambientStamp);
								ProcessGeometry(worldContext, geometry);
							}

							renderStageArray[geometry->GetShadowRenderStage()].AddElement(geometry);

							#if C4STATS

								worldCounter[kWorldCounterSpotShadowGeometry]++;

							#endif
						}
					}
				}
				else
				{
					RenderSpotShadowTerrain(worldContext, static_cast<TerrainGeometry *>(geometry), nullptr, lightRegion, shadowRegion, occlusionList);
					return;
				}
			}

			const Bond *bond = node->GetFirstOutgoingEdge();
			while (bond)
			{
				RenderSpotShadowNode(worldContext, static_cast<Node *>(bond->GetFinishElement()), excludedNode, lightRegion, shadowRegion, occlusionList);
				bond = bond->GetNextOutgoingEdge();
			}
		}
	}
}

void World::RenderSpotShadowCell(const WorldContext *worldContext, const Site *cell, const Node *excludedNode, const LightRegion *lightRegion, const ShadowRegion *shadowRegion, const List<OcclusionRegion> *occlusionList)
{
	const Box3D& box = cell->GetWorldBoundingBox();
	if ((lightRegion->BoxVisible(box)) && (shadowRegion->BoxVisible(box)))
	{
		const Bond *bond = cell->GetFirstOutgoingEdge();
		while (bond)
		{
			Site *site = bond->GetFinishElement();
			if (site->GetCellIndex() < 0)
			{
				RenderSpotShadowNode(worldContext, static_cast<Node *>(site), excludedNode, lightRegion, shadowRegion, occlusionList);
			}
			else
			{
				RenderSpotShadowCell(worldContext, site, excludedNode, lightRegion, shadowRegion, occlusionList);
			}

			bond = bond->GetNextOutgoingEdge();
		}
	}
}

void World::RenderSpotShadowRegion(const WorldContext *worldContext, const Node *excludedNode, const LightRegion *lightRegion, const ShadowRegion *shadowRegion, const List<OcclusionRegion> *occlusionList)
{
	const Bond *bond = shadowRegion->GetZone()->GetCellGraphSite(kCellGraphGeometry)->GetFirstOutgoingEdge();
	while (bond)
	{
		Site *site = bond->GetFinishElement();
		if (site->GetCellIndex() < 0)
		{
			RenderSpotShadowNode(worldContext, static_cast<Node *>(site), excludedNode, lightRegion, shadowRegion, occlusionList);
		}
		else
		{
			RenderSpotShadowCell(worldContext, site, excludedNode, lightRegion, shadowRegion, occlusionList);
		}

		bond = bond->GetNextOutgoingEdge();
	}
}

void World::RenderSpotShadow(const WorldContext *worldContext, const SpotLight *light)
{
	List<ShadowRegion>		shadowRegionList;
	List<OcclusionRegion>	occlusionList;

	TheGraphicsMgr->BeginSpotShadow();

	const Point3D& lightPosition = light->GetWorldPosition();

	const ImmutableArray<CameraRegion *>& cameraRegionArray = light->GetActiveCameraRegionArray();
	for (const CameraRegion *cameraRegion : cameraRegionArray)
	{
		ShadowRegion *shadowRegion = new ShadowRegion(cameraRegion->GetZone());
		shadowRegionList.Append(shadowRegion);

		CalculatePointShadowRegion(cameraRegion, lightPosition, shadowRegion);

		const ImmutableArray<const LightRegion *>& lightRegionArray = cameraRegion->GetShadowCastingLightRegionArray();
		for (const LightRegion *lightRegion : lightRegionArray)
		{
			if (lightRegion->GetLight() == light)
			{
				const LightRegion *subregion = lightRegion;
				const LightRegion *superRegion = subregion->GetSuperNode();
				while (superRegion)
				{
					const Portal *portal = subregion->GetIlluminatedPortal();
					const PortalObject *object = portal->GetObject();

					if (object->GetPortalFlags() & kPortalShadowMapInhibit)
					{
						break;
					}

					ShadowRegion *superShadowRegion = new ShadowRegion(superRegion->GetZone(), shadowRegion);
					shadowRegionList.Append(superShadowRegion);

					// Add the extruded portal planes to the set of planes in the original shadow region.

					Convexity *convexity = superShadowRegion->GetRegionConvexity();
					Antivector4D *plane = &convexity->plane[convexity->planeCount];

					int32 count = object->GetVertexCount();
					convexity->planeCount += count;

					const Point3D *vertex = portal->GetWorldVertexArray();
					const Point3D *v1 = &vertex[count - 1];
					for (machine a = 0; a < count; a++)
					{
						const Point3D *v2 = &vertex[a];
						plane[a].SetStandard(lightPosition, *v2, *v1);
						v1 = v2;
					}

					subregion = superRegion;
					superRegion = superRegion->GetSuperNode();
				}
			}
		}
	}

	shadowRenderStamp++;
	shadowTerrainStamp++;

	const SpotLightObject *lightObject = light->GetObject();
	float range = lightObject->GetLightRange();

	FrustumCamera frustumCamera(lightObject->GetApexTangent(), lightObject->GetAspectRatio());
	frustumCamera.SetWorld(this);

	FrustumCameraObject *cameraObject = frustumCamera.GetObject();
	int32 shadowMapSize = TheGraphicsMgr->GetSpotShadowMapSize();
	cameraObject->SetViewRect(Rect(0, 0, shadowMapSize, shadowMapSize));
	cameraObject->SetNearDepth(lightObject->GetMinShadowDistance());
	cameraObject->SetFarDepth(range);

	frustumCamera.SetNodeTransform(light->GetWorldTransform());
	frustumCamera.Update();

	TheGraphicsMgr->SetCamera(cameraObject, &frustumCamera, 0, false);

	const Node *excludedNode = light->GetExcludedNode();

	const ShadowRegion *shadowRegion = shadowRegionList.First();
	while (shadowRegion)
	{
		RenderSpotShadowRegion(worldContext, excludedNode, light->GetFirstRootRegion(), shadowRegion, &occlusionList);
		shadowRegion = shadowRegion->GetNextShadowRegion();
	}

	unsigned_int32 shadowStamp = shadowTerrainStamp;
	for (TerrainLevelGeometry *terrain : terrainShadowUpdateArray)
	{
		terrain->UpdateBorderState(kTerrainStampShadow, shadowStamp);
	}

	terrainShadowUpdateArray.Clear();

	FinishWorldBatch();

	TheGraphicsMgr->DrawShadow(renderStageArray[kRenderStageShadowDefault]);
	renderStageArray[kRenderStageShadowDefault].Clear();

	TheGraphicsMgr->DrawShadow(renderStageArray[kRenderStageShadowAlphaTest]);
	renderStageArray[kRenderStageShadowAlphaTest].Clear();

	TheGraphicsMgr->EndSpotShadow();

	#if C4STATS

		worldCounter[kWorldCounterSpotShadow]++;

	#endif
}

bool World::PointLightVisible(PointLight *light, const Point3D& cameraPosition, const CameraRegion *cameraRegion, const List<OcclusionRegion> *occlusionList)
{
	const PointLightObject *object = light->GetObject();
	const Point3D& lightPosition = light->GetWorldPosition();

	float multiplier = light->CalculateColorMultiplier(Magnitude(lightPosition - cameraPosition));
	if (multiplier < K::min_float)
	{
		return (false);
	}

	float lightRange = object->GetLightRange();
	if (!cameraRegion->SphereVisible(lightPosition, lightRange))
	{
		return (false);
	}

	const OcclusionRegion *occluder = occlusionList->First();
	while (occluder)
	{
		if (occluder->SphereOccluded(lightPosition, lightRange))
		{
			return (false);
		}

		occluder = occluder->Next();
	}

	return (true);
}

bool World::LightRegionVisible(const WorldContext *worldContext, const LightRegion *lightRegion, const CameraRegion *cameraRegion)
{
	Polyhedron		resultPolyhedron[2];

	const Polyhedron *polyhedron = lightRegion->GetRegionPolyhedron();
	if (polyhedron->faceCount < 4)
	{
		return (true);
	}

	if ((!cameraRegion->GetSuperNode()) && (lightRegion->SphereVisible(worldContext->renderCamera->GetWorldPosition(), kCameraLightClipEpsilon)))
	{
		return (true);
	}

	int32 planeCount = cameraRegion->GetPlaneCount();
	const Antivector4D *planeArray = cameraRegion->GetPlaneArray();

	if (!Math::ClipPolyhedron(polyhedron, planeArray[0], &resultPolyhedron[1]))
	{
		return (false);
	}

	for (machine a = 1; a < planeCount; a++)
	{
		machine parity = a & 1;
		if (!Math::ClipPolyhedron(&resultPolyhedron[parity], planeArray[a], &resultPolyhedron[parity ^ 1]))
		{
			return (false);
		}
	}

	return (true);
}

void World::CollectLightRegions(WorldContext *worldContext, const Point3D& cameraPosition, CameraRegion *cameraRegion)
{
	bool				lightActive;
	const LightObject	*lightObject;
	CameraRegion		*clippedCameraRegion;

	unsigned_int32 regionStamp = regionRenderStamp;
	const Zone *zone = cameraRegion->GetZone();
	const Light *previousLight = nullptr;

	LightRegion *lightRegion = zone->GetFirstLightRegion();
	while (lightRegion)
	{
		Light *light = lightRegion->GetLight();
		if (light != previousLight)
		{
			previousLight = light;
			lightActive = false;

			if ((light->Enabled()) && ((light->GetPerspectiveExclusionMask() & worldContext->perspectiveFlags) == 0))
			{
				lightObject = light->GetObject();
				if (lightObject->GetMinDetailLevel() <= worldContext->lightDetailLevel)
				{
					if ((light->GetBaseLightType() == kLightInfinite) || (PointLightVisible(static_cast<PointLight *>(light), cameraPosition, cameraRegion, &worldContext->occlusionList)))
					{
						clippedCameraRegion = nullptr;
						lightActive = true;
					}
				}
			}
		}

		if ((lightActive) && (LightRegionVisible(worldContext, lightRegion, cameraRegion)))
		{
			if (light->GetActiveLightRegionArray().Empty())
			{
				visibleLightArray.AddElement(light);
			}

			if (lightRegion->GetRegionStamp() != regionStamp)
			{
				lightRegion->SetRegionStamp(regionStamp);
				light->AddActiveLightRegion(lightRegion);
			}

			unsigned_int32 flags = lightObject->GetLightFlags();
			if (flags & kLightUnified)
			{
				worldContext->unifiedLight = static_cast<InfiniteLight *>(light);
			}

			if ((!(flags & kLightShadowInhibit)) && (worldContext->shadowEnableFlag))
			{
				if (!clippedCameraRegion)
				{
					clippedCameraRegion = cameraRegion;

					const ShadowSpace *shadowSpace = zone->GetConnectedShadowSpace();
					if ((shadowSpace) || ((shadowSpace = light->GetConnectedShadowSpace()) != nullptr))
					{
						Polyhedron		polyhedron;

						if (Math::ClipPolyhedron(cameraRegion->GetRegionPolyhedron(), shadowSpace->GetInverseWorldTransform().GetRow(2), &polyhedron))
						{
							clippedCameraRegion = new CameraRegion(cameraRegion, &polyhedron);
							clippedCameraRegionList.Append(clippedCameraRegion);
						}
					}
				}

				light->AddActiveCameraRegion(clippedCameraRegion);
				clippedCameraRegion->AddShadowCastingLightRegion(lightRegion);
			}

			#if C4DIAGS

				if (diagnosticFlags & kDiagnosticLightRegions)
				{
					lightRegionDiagnosticList.Append(new LightRegionOutline(*lightRegion->GetRegionPolyhedron()));
				}

			#endif
		}

		lightRegion = lightRegion->GetNextLightRegion();
	}

	CameraRegion *subregion = cameraRegion->GetFirstSubnode();
	while (subregion)
	{
		CollectLightRegions(worldContext, cameraPosition, subregion);
		subregion = subregion->Tree<CameraRegion>::Next();
	}
}

void World::RenderLight(const WorldContext *worldContext, Light *light, bool unified)
{
	bool	shadowFlag;

	lightRenderStamp++;

	Controller *controller = light->GetController();
	if ((controller) && (controller->GetControllerFlags() & kControllerUpdate))
	{
		controller->Update();
	}

	LightType type = light->GetLightType();
	if (type == kLightInfinite)
	{
		RenderInfiniteLight(worldContext, static_cast<InfiniteLight *>(light), &shadowFlag);

		TheGraphicsMgr->Draw(&renderStageList[kRenderStageLightDefault], true);
		renderStageList[kRenderStageLightDefault].RemoveAll();

		TheGraphicsMgr->Draw(&renderStageList[kRenderStageLightAlphaTest], true);
		renderStageList[kRenderStageLightAlphaTest].RemoveAll();

		TheGraphicsMgr->Draw(&lightImpostorSystemArray);
		lightImpostorSystemArray.Clear();

		if (shadowFlag)
		{
			TheGraphicsMgr->DiscardInfiniteShadow();
		}

		#if C4STATS

			worldCounter[kWorldCounterInfiniteLight]++;

		#endif
	}
	else if (type != kLightSpot)
	{
		if (RenderPointLight(worldContext, static_cast<PointLight *>(light), &shadowFlag))
		{
			TheGraphicsMgr->Draw(&renderStageList[kRenderStageLightDefault], true);
			renderStageList[kRenderStageLightDefault].RemoveAll();

			TheGraphicsMgr->Draw(&renderStageList[kRenderStageLightAlphaTest], true);
			renderStageList[kRenderStageLightAlphaTest].RemoveAll();

			TheGraphicsMgr->Draw(&lightImpostorSystemArray);
			lightImpostorSystemArray.Clear();

			if (shadowFlag)
			{
				TheGraphicsMgr->DiscardPointShadow();
			}

			#if C4STATS

				worldCounter[kWorldCounterPointLight]++;

			#endif
		}
	}
	else
	{
		if (RenderSpotLight(worldContext, static_cast<SpotLight *>(light), &shadowFlag))
		{
			TheGraphicsMgr->Draw(&renderStageList[kRenderStageLightDefault], true);
			renderStageList[kRenderStageLightDefault].RemoveAll();

			TheGraphicsMgr->Draw(&renderStageList[kRenderStageLightAlphaTest], true);
			renderStageList[kRenderStageLightAlphaTest].RemoveAll();

			TheGraphicsMgr->Draw(&lightImpostorSystemArray);
			lightImpostorSystemArray.Clear();

			if (shadowFlag)
			{
				TheGraphicsMgr->DiscardSpotShadow();
			}

			#if C4STATS

				worldCounter[kWorldCounterSpotLight]++;

			#endif
		}
	}

	light->ClearActiveRegions();
}

bool World::ProcessFogSpace(WorldContext *worldContext, const FogSpace *fogSpace, const CameraRegion *rootRegion)
{
	if ((fogSpace->GetPerspectiveExclusionMask() & worldContext->perspectiveFlags) == 0)
	{
		const FrustumCamera *camera = worldContext->renderCamera;
		const Point3D& cameraPosition = camera->GetWorldPosition();
		const Vector3D& cameraDirection = camera->GetWorldTransform()[2];

		Antivector4D fogPlane = fogSpace->GetInverseWorldTransform().GetRow(2);

		float F_wedge_C = fogPlane ^ cameraPosition;
		if (F_wedge_C < 0.0F)
		{
			*worldContext->fogSpacePtr = fogSpace;

			const FogSpaceObject *object = fogSpace->GetObject();
			unsigned_int32 flags = object->GetFogSpaceFlags();

			if (flags & kFogSpaceDistanceOcclusion)
			{
				const Antivector3D& planeNormal = fogPlane.GetAntivector3D();
				Vector3D parallelDirection = cameraDirection - ProjectOnto(cameraDirection, planeNormal);
				float m = SquaredMag(parallelDirection);
				if (m > K::min_float)
				{
					parallelDirection *= InverseSqrt(m);
					float d = object->CalculateOcclusionDistance(F_wedge_C);

					OcclusionRegion *occluder = new OcclusionRegion();
					Antivector4D *planeArray = occluder->GetPlaneArray();
					planeArray[0].Set(parallelDirection, -(cameraPosition * parallelDirection) - d);
					planeArray[1] = -fogPlane;
					occluder->SetPlaneCount(2);

					worldContext->occlusionList.Append(occluder);
				}
			}

			if ((flags & kFogSpaceDepthOcclusion) && ((fogPlane ^ cameraDirection) < camera->GetSineHalfField()))
			{
				float d = object->CalculateOcclusionDepth(F_wedge_C);

				OcclusionRegion *occluder = new OcclusionRegion();
				const Antivector3D& planeNormal = fogPlane.GetAntivector3D();
				occluder->GetPlaneArray()[0].Set(-planeNormal, cameraPosition * planeNormal - d);
				occluder->SetPlaneCount(1);

				worldContext->occlusionList.Append(occluder);
			}
		}
		else if (((fogPlane ^ cameraDirection) < camera->GetSineHalfField()) && (fogSpace->Visible(rootRegion, &worldContext->occlusionList)))
		{
			*worldContext->fogSpacePtr = fogSpace;
			worldContext->unfoggedRegion.GetPlaneArray()[0] = fogPlane;
			worldContext->unfoggedList.Append(&worldContext->unfoggedRegion);

			const FogSpaceObject *object = fogSpace->GetObject();
			if (object->GetFogSpaceFlags() & kFogSpaceDepthOcclusion)
			{
				float d = object->CalculateOcclusionDepth(0.0F);

				OcclusionRegion *occluder = new OcclusionRegion();
				occluder->GetPlaneArray()[0].Set(-fogPlane.GetAntivector3D(), -fogPlane.w - d);
				occluder->SetPlaneCount(1);

				worldContext->occlusionList.Append(occluder);
			}
		}

		return (true);
	}

	return (false);
}

void World::ProcessPortal(WorldContext *worldContext, Portal *portal, CameraRegion *rootRegion)
{
	if ((portal->Enabled()) && ((portal->GetPerspectiveExclusionMask() & worldContext->perspectiveFlags) == 0))
	{
		const BoundingSphere *sphere = portal->GetBoundingSphere();
		if (rootRegion->SphereVisible(sphere->GetCenter(), sphere->GetRadius()))
		{
			const FrustumCamera *camera = worldContext->renderCamera;
			const Point3D& cameraPosition = camera->GetWorldPosition();

			const Antivector4D& portalPlane = portal->GetWorldPlane();
			float distance = portalPlane ^ cameraPosition;
			if ((!(distance < 0.0F)) && ((portalPlane ^ camera->GetWorldTransform()[2]) < camera->GetSineHalfField()))
			{
				Point3D		tempVertex[2][kMaxPortalVertexCount + kMaxPolyhedronFaceCount];

				const PortalObject *portalObject = portal->GetObject();
				int32 vertexCount = portalObject->GetVertexCount();
				const Point3D *vertex = portal->GetWorldVertexArray();

				if ((distance > kMinPortalClipDistance) || (Math::PointInConvexPolygon(cameraPosition, vertexCount, vertex, portalPlane.GetAntivector3D()) != kPolygonInterior))
				{
					int8	location[kMaxPortalVertexCount + kMaxPolyhedronFaceCount];

					int32 planeCount = rootRegion->GetPlaneCount() - rootRegion->GetAuxiliaryPlaneCount();
					const Antivector4D *planeArray = rootRegion->GetPlaneArray();
					for (machine a = 0; a < planeCount; a++)
					{
						Point3D *result = tempVertex[a & 1];
						vertexCount = Math::ClipPolygon(vertexCount, vertex, planeArray[a], location, result);
						if (vertexCount == 0)
						{
							return;
						}

						vertex = result;
					}

					const OcclusionRegion *occluder = worldContext->occlusionList.First();
					while (occluder)
					{
						if ((occluder->GetOcclusionMask() & kOcclusionPortal) && (occluder->PolygonOccluded(vertexCount, vertex)))
						{
							return;
						}

						occluder = occluder->Next();
					}

					vertexCount = Min(vertexCount, kMaxClippedPortalVertexCount);
				}
				else
				{
					vertexCount = 4;
					vertex = camera->GetFrustumVertexArray();
				}

				PortalType portalType = portal->GetPortalType();
				if (portalType == kPortalDirect)
				{
					Zone *connectedZone = portal->GetConnectedZone();
					if ((connectedZone) && (connectedZone->GetTraversalExclusionMask() == 0))
					{
						CameraRegion *newRegion = new CameraRegion(camera, connectedZone);
						newRegion->SetPolygonExtrusion(vertexCount, vertex, cameraPosition, camera->GetWorldTransform()[2], camera->GetObject()->GetFarDepth(), -portalPlane);
						rootRegion->AppendSubnode(newRegion);

						const FogSpace **fsp = worldContext->fogSpacePtr;
						if (portalObject->GetPortalFlags() & kPortalFogInhibit)
						{
							worldContext->fogSpacePtr = nullptr;
						}

						ProcessCameraRegion(worldContext, newRegion);
						worldContext->fogSpacePtr = fsp;

						#if C4STATS

							worldCounter[kWorldCounterDirectPortal]++;

						#endif
					}
				}
				else if (portalType == kPortalRemote)
				{
					if (remoteRecursionCount < kMaxRemoteRecursionCount)
					{
						MapReservation		reservation;

						PortalBuffer buffer = static_cast<const RemotePortalObject *>(portalObject)->GetPortalBuffer();
						if (buffer == kPortalBufferReflection)
						{
							Map<PortalData> *portalMap = &worldContext->portalGroup[kPortalGroupReflection];
							if (portalMap->Reserve(portal, &reservation))
							{
								portalMap->Insert(new PortalData(portal, rootRegion->GetZone(), vertexCount, vertex), &reservation);
							}
						}
						else if (buffer == kPortalBufferRefraction)
						{
							Map<PortalData> *portalMap = &worldContext->portalGroup[kPortalGroupRefraction];
							if (portalMap->Reserve(portal, &reservation))
							{
								portalMap->Insert(new PortalData(portal, rootRegion->GetZone(), vertexCount, vertex), &reservation);
							}
						}
						else
						{
							Map<PortalData> *portalMap = &worldContext->portalGroup[kPortalGroupRemote];
							if (portalMap->Reserve(portal, &reservation))
							{
								portalMap->Insert(new PortalData(portal, rootRegion->GetZone(), vertexCount, vertex), &reservation);
							}
						}
					}
				}
				else if (portalType == kPortalCamera)
				{
					if (cameraRecursionCount < kMaxCameraRecursionCount)
					{
						MapReservation		reservation;

						Map<PortalData> *portalMap = &worldContext->portalGroup[kPortalGroupCamera];
						if (portalMap->Reserve(portal, &reservation))
						{
							portalMap->Insert(new PortalData(portal, rootRegion->GetZone(), vertexCount, vertex), &reservation);
						}
					}
				}
			}
		}
	}
}

void World::ProcessCameraRegion(WorldContext *worldContext, CameraRegion *rootRegion)
{
	Zone *zone = rootRegion->GetZone();
	zone->AddCameraRegion(rootRegion);
	zone->SetTraversalExclusionMask(zone->GetTraversalExclusionMask() | kZoneTraversalLocal);

	if ((worldContext->fogSpacePtr) && (!*worldContext->fogSpacePtr))
	{
		const FogSpace *fogSpace = zone->GetFirstFogSpace();
		while (fogSpace)
		{
			if ((fogSpace->Enabled()) && (ProcessFogSpace(worldContext, fogSpace, rootRegion)))
			{
				break;
			}

			fogSpace = fogSpace->Next();
		}

		if (!fogSpace)
		{
			fogSpace = zone->GetConnectedFogSpace();
			if ((fogSpace) && (fogSpace->Enabled()))
			{
				ProcessFogSpace(worldContext, fogSpace, rootRegion);
			}
		}
	}

	const OcclusionPortal *occlusionPortal = static_cast<OcclusionPortal *>(zone->GetFirstOcclusionPortal());
	while (occlusionPortal)
	{
		if ((occlusionPortal->Enabled()) && ((occlusionPortal->GetPerspectiveExclusionMask() & worldContext->perspectiveFlags) == 0))
		{
			const BoundingSphere *sphere = occlusionPortal->GetBoundingSphere();
			if (rootRegion->SphereVisible(sphere->GetCenter(), sphere->GetRadius()))
			{
				OcclusionRegion *occluder = occlusionPortal->NewFrustumOcclusionRegion(worldContext->renderCamera);
				if (occluder)
				{
					worldContext->occlusionList.Append(occluder);

					#if C4STATS

						worldCounter[kWorldCounterOcclusionRegion]++;

					#endif
				}
			}
		}

		occlusionPortal = static_cast<OcclusionPortal *>(occlusionPortal->Next());
	}

	const OcclusionSpace *occlusionSpace = zone->GetFirstOcclusionSpace();
	while (occlusionSpace)
	{
		if ((occlusionSpace->Enabled()) && (occlusionSpace->Visible(rootRegion, &worldContext->occlusionList)))
		{
			OcclusionRegion *occluder = occlusionSpace->NewFrustumOcclusionRegion(worldContext->renderCamera);
			if (occluder)
			{
				worldContext->occlusionList.Append(occluder);

				#if C4STATS

					worldCounter[kWorldCounterOcclusionRegion]++;

				#endif
			}
		}

		occlusionSpace = occlusionSpace->Next();
	}

	Portal *portal = zone->GetFirstPortal();
	while (portal)
	{
		ProcessPortal(worldContext, portal, rootRegion);
		portal = portal->Next();
	}

	zone->SetTraversalExclusionMask(zone->GetTraversalExclusionMask() & ~kZoneTraversalLocal);
}

void World::RenderIndirectPortals(const WorldContext *worldContext)
{
	const PortalData *portalData = worldContext->portalGroup[kPortalGroupCamera].First();
	while (portalData)
	{
		const CameraPortal *cameraPortal = static_cast<const CameraPortal *>(portalData->GetPortal());
		FrustumCamera *targetCamera = cameraPortal->GetTargetCamera();
		if (targetCamera)
		{
			const CameraPortalObject *portalObject = cameraPortal->GetObject();
			int32 width = portalObject->GetViewportWidth();
			int32 height = portalObject->GetViewportHeight();

			if (width > renderWidth)
			{
				height = Max(renderWidth * height / width, 32);
				width = renderWidth;
			}

			if (height > renderHeight)
			{
				width = Max(renderHeight * width / height, 32);
				height = renderHeight;
			}

			int32 prevRenderWidth = renderWidth;
			int32 prevRenderHeight = renderHeight;
			renderWidth = width;
			renderHeight = height;

			int32 displayHeight = TheDisplayMgr->GetDisplayHeight();
			Rect viewportRect(0, displayHeight - height, width, displayHeight);
			cameraPortal->CallRenderSizeProc(width, height);

			FrustumCameraObject *cameraObject = targetCamera->GetObject();
			cameraObject->SetAspectRatio((float) height / (float) width);
			cameraObject->SetViewRect(viewportRect);
			SetCameraClearParams(cameraObject);

			WorldContext portalContext(targetCamera);
			portalContext.skyboxNode = worldContext->skyboxNode;
			portalContext.perspectiveFlags = kPerspectivePrimary << kPerspectiveCameraWidgetShift;
			portalContext.lightDetailLevel = worldContext->lightDetailLevel;
			portalContext.cameraMinDetailLevel = Max(worldContext->cameraMinDetailLevel, portalObject->GetMinDetailLevel());
			portalContext.cameraDetailBias = worldContext->cameraDetailBias + portalObject->GetDetailLevelBias();

			targetCamera->UpdateRootRegions(targetCamera->GetOwningZone());

			RootCameraRegion *cameraRegion = targetCamera->GetFirstRootRegion();
			while (cameraRegion)
			{
				targetCamera->CalculateFrustumCameraRegion(cameraRegion);
				cameraRegion = cameraRegion->Next();
			}

			cameraRecursionCount++;

			RenderCamera(&portalContext, kRenderTargetPrimary);
			TheGraphicsMgr->CopyRenderTarget(cameraPortal->GetCameraTexture(), viewportRect);

			cameraRecursionCount--;

			renderWidth = prevRenderWidth;
			renderHeight = prevRenderHeight;
		}

		portalData = portalData->Next();
	}

	unsigned_int32 shift = 0;
	unsigned_int32 perspectiveFlags = worldContext->perspectiveFlags;
	if (perspectiveFlags & kPerspectiveRemotePortalMask)
	{
		shift = kPerspectiveRemotePortalShift;
	}
	else if (perspectiveFlags & kPerspectiveCameraWidgetMask)
	{
		shift = kPerspectiveCameraWidgetShift;
	}

	portalData = worldContext->portalGroup[kPortalGroupReflection].First();
	while (portalData)
	{
		RemotePortal *remotePortal = static_cast<RemotePortal *>(portalData->GetPortal());
		RenderRemoteCamera(worldContext, remotePortal, kRenderTargetReflection, kPerspectiveReflection << shift, portalData);
		portalData = portalData->Next();
	}

	portalData = worldContext->portalGroup[kPortalGroupRefraction].First();
	while (portalData)
	{
		RemotePortal *remotePortal = static_cast<RemotePortal *>(portalData->GetPortal());
		RenderRemoteCamera(worldContext, remotePortal, kRenderTargetRefraction, kPerspectiveRefraction << shift, portalData);
		portalData = portalData->Next();
	}

	portalData = worldContext->portalGroup[kPortalGroupRemote].First();
	while (portalData)
	{
		RemotePortal *remotePortal = static_cast<RemotePortal *>(portalData->GetPortal());
		RenderRemoteCamera(worldContext, remotePortal, kRenderTargetPrimary, kPerspectivePrimary << kPerspectiveRemotePortalShift, portalData);
		portalData = portalData->Next();
	}
}

void World::RenderRemoteCamera(const WorldContext *worldContext, RemotePortal *remotePortal, RenderTargetType target, unsigned_int32 perspectiveFlags, const PortalData *portalData)
{
	Transform4D		remoteTransform;

	Zone *remoteZone = remotePortal->GetConnectedZone();
	if (!remoteZone)
	{
		remoteZone = portalData->GetOriginZone();
	}

	int32 vertexCount = portalData->GetVertexCount();
	const Point3D *vertex = portalData->GetVertexArray();

	bool identityFlag = remotePortal->CalculateRemoteTransform(&remoteTransform);

	const FrustumCamera *camera = worldContext->renderCamera;
	const FrustumCameraObject *cameraObject = camera->GetObject();
	const RemotePortalObject *portalObject = remotePortal->GetObject();

	const Antivector4D& portalPlane = remotePortal->GetWorldPlane();
	Antivector4D clipPlane(portalPlane.GetAntivector3D(), portalPlane.w - portalObject->GetPortalPlaneOffset());

	RemoteCamera remoteCamera(cameraObject->GetFocalLength() * portalObject->GetFocalLengthMultiplier(), cameraObject->GetAspectRatio(), remoteTransform, clipPlane);
	RemoteCameraObject *remoteCameraObject = static_cast<RemoteCameraObject *>(remoteCamera.GetObject());

	unsigned_int32 portalFlags = portalObject->GetPortalFlags();
	if (portalFlags & kPortalObliqueFrustum)
	{
		remoteCameraObject->SetFrustumFlags(kFrustumInfinite | kFrustumOblique);
	}
	else
	{
		remoteCameraObject->SetFrustumFlags(kFrustumInfinite);
	}

	SetCameraClearParams(remoteCameraObject);

	if (portalFlags & kPortalOverrideClearColor)
	{
		remoteCameraObject->SetClearColor(portalObject->GetPortalClearColor());
	}

	remoteCameraObject->SetProjectionOffset(cameraObject->GetProjectionOffset());
	remoteCameraObject->SetNearDepth(cameraObject->GetNearDepth());
	remoteCameraObject->SetFarDepth(Fmin(cameraObject->GetFarDepth(), portalObject->GetFarClipDepth()));
	remoteCameraObject->SetViewRect(cameraObject->GetViewRect());

	remoteCamera.SetNodeTransform(camera->GetWorldTransform());
	remoteCamera.Invalidate();
	remoteCamera.Update();

	const Transform4D& previousCameraWorldTransform = remotePortal->GetPreviousCameraWorldTransform();
	if (previousCameraWorldTransform(3,3) != 0.0F)
	{
		remoteCamera.SetPreviousWorldTransform(previousCameraWorldTransform);
	}

	remotePortal->SetPreviousCameraWorldTransform(remoteCamera.GetWorldTransform());

	WorldContext portalContext(&remoteCamera);
	portalContext.skyboxNode = (!(portalFlags & kPortalSkyboxInhibit)) ? worldContext->skyboxNode : nullptr;
	portalContext.perspectiveFlags = perspectiveFlags;
	portalContext.shadowEnableFlag = worldContext->shadowEnableFlag & ((portalFlags & kPortalShadowMapInhibit) == 0);
	portalContext.lightDetailLevel = worldContext->lightDetailLevel;
	portalContext.cameraMinDetailLevel = Max(worldContext->cameraMinDetailLevel, portalObject->GetMinDetailLevel());
	portalContext.cameraDetailBias = worldContext->cameraDetailBias + portalObject->GetDetailLevelBias();

	remoteRecursionCount++;
	unsigned_int32 nodeFlags = remotePortal->GetNodeFlags();
	if (!(portalFlags & kPortalRecursive))
	{
		remotePortal->SetNodeFlags(nodeFlags | kNodeDisabled);
	}

	if (portalFlags & kPortalForceCameraZone)
	{
		remoteCamera.EstablishFixedRootCameraRegion(remoteZone);
	}
	else
	{
		remoteCamera.UpdateRootRegions(GetRootNode());
	}

	remoteCamera.SetRemotePolygon(vertexCount, vertex);
	RootCameraRegion *cameraRegion = remoteCamera.GetFirstRootRegion();

	if ((identityFlag) && (!(portalFlags & kPortalSeparateShadowMap)))
	{
		worldContext->reusableShadowFlag = true;

		while (cameraRegion)
		{
			remoteCamera.CalculateFrustumCameraRegion(cameraRegion);
			cameraRegion = cameraRegion->Next();
		}

		OcclusionRegion *occluder = new OcclusionRegion(~kOcclusionPortal & ~kOcclusionCascade);
		occluder->SetPlaneCount(1);
		occluder->GetPlaneArray()[0] = clipPlane;
		portalContext.occlusionList.Append(occluder);

		const Vector2D& extent = camera->GetMaxFrustumExtent();
		portalContext.maxGeometryExtent[0].Set(-extent.x, extent.x);
		portalContext.maxGeometryExtent[1].Set(-extent.y, extent.y);

		if (!(portalFlags & kPortalShadowMapMaxDepth))
		{
			// Calculate the deepest point where the portal plane intersects the view frustum
			// and set it as the initial max geometry depth. We need to do this because we want
			// shadows to be rendered between the camera and the portal plane, but the max
			// geometry depth would be really small in the case that nothing were rendered
			// through the portal, thus culling shadow cascades that are further away.

			float	z[4];

			const Point3D& position = camera->GetWorldPosition();
			Antivector4D plane(portalPlane.GetAntivector3D(), portalPlane ^ position);

			const Vector3D& view = camera->GetWorldTransform()[2];
			const Point3D *frustumVertex = camera->GetFrustumVertexArray();
			for (machine a = 0; a < 4; a++)
			{
				Vector3D dp = frustumVertex[a] - position;
				float t = -plane.w / (plane ^ dp);
				z[a] = (dp * view) * t;
			}

			portalContext.maxGeometryDepth = Fmax(Fmax(z[0], z[1], z[2], z[3]), portalContext.maxGeometryDepth);
		}
		else
		{
			portalContext.maxGeometryDepth = cameraObject->GetFarDepth();
		}
	}
	else
	{
		while (cameraRegion)
		{
			remoteCamera.CalculateRemoteCameraRegion(cameraRegion);
			cameraRegion = cameraRegion->Next();
		}
	}

	if (!(portalFlags & kPortalDistant))
	{
		RenderCamera(&portalContext, target);
	}
	else
	{
		RenderDistantCamera(&portalContext, target);
	}

	remoteRecursionCount--;
	remotePortal->SetNodeFlags(nodeFlags);

	#if C4STATS

		worldCounter[kWorldCounterRemotePortal]++;

	#endif
}

void World::RenderCamera(WorldContext *worldContext, RenderTargetType target)
{
	currentWorldContext = worldContext;

	const FogSpace *cameraFogSpace = nullptr;
	worldContext->fogSpacePtr = &cameraFogSpace;

	const FrustumCamera *camera = worldContext->renderCamera;
	RootCameraRegion *cameraRegion = camera->GetFirstRootRegion();
	do
	{
		cameraRegion->GetZone()->SetTraversalExclusionMask(kZoneTraversalGlobal);
		cameraRegion = cameraRegion->Next();
	} while (cameraRegion);

	cameraRegion = camera->GetFirstRootRegion();
	do
	{
		ProcessCameraRegion(worldContext, cameraRegion);
		cameraRegion = cameraRegion->Next();
	} while (cameraRegion);

	cameraRegion = camera->GetFirstRootRegion();
	do
	{
		cameraRegion->GetZone()->SetTraversalExclusionMask(0);
		cameraRegion = cameraRegion->Next();
	} while (cameraRegion);

	RenderIndirectPortals(worldContext);

	TheGraphicsMgr->SetRenderTarget(target);

	const FrustumCameraObject *cameraObject = camera->GetObject();
	TheGraphicsMgr->SetCamera(cameraObject, camera);

	if (cameraFogSpace)
	{
		TheGraphicsMgr->SetFogSpace(cameraFogSpace->GetObject(), cameraFogSpace);
	}

	worldContext->unifiedLight = nullptr;

	if (!(worldFlags & kWorldAmbientOnly))
	{
		regionRenderStamp++;

		cameraRegion = camera->GetFirstRootRegion();
		do
		{
			CollectLightRegions(worldContext, camera->GetWorldPosition(), cameraRegion);
			cameraRegion = cameraRegion->Next();
		} while (cameraRegion);
	}

	ambientRenderStamp++;

	if (worldContext->unifiedLight)
	{
		bool	shadowFlag;

		RenderUnified(worldContext, camera, worldContext->unifiedLight, &shadowFlag);

		unsigned_int32 ambientStamp = ambientRenderStamp;
		for (TerrainLevelGeometry *terrain : terrainUpdateArray)
		{
			terrain->UpdateBorderState(kTerrainStampAmbient, ambientStamp);
		}

		terrainUpdateArray.Clear();

		ImpostorSystem *system = impostorSystemMap.First();
		while (system)
		{
			system->Render(&ambientImpostorSystemArray, kImpostorRenderAmbient);
			system->Render(&lightImpostorSystemArray, kImpostorRenderLight);
			system = system->Next();
		}

		FinishWorldBatch();
		BlobParticleSystem::FinishBatches(&renderStageList[kRenderStageFirstEffect]);

		unsigned_int32 structureFlags = kStructureRenderVelocity | kStructureRenderDepth;
		if (worldFlags & kWorldClearColor)
		{
			structureFlags |= kStructureClearBuffer;
		}

		if (worldContext->skyboxFlag)
		{
			Skybox *skybox = worldContext->skyboxNode;
			if (skybox)
			{
				skybox->Render(camera, &skyboxRenderArray);
				if (!skyboxRenderArray.Empty())
				{
					structureFlags |= kStructureClearBuffer;
				}
			}
		}

		if ((worldContext->perspectiveFlags & (kPerspectivePrimary | (kPerspectivePrimary << kPerspectiveCameraWidgetShift))) != 0)
		{
			if (worldFlags & kWorldMotionBlurInhibit)
			{
				structureFlags &= ~kStructureRenderVelocity;
			}
			else if (worldFlags & kWorldZeroBackgroundVelocity)
			{
				structureFlags |= kStructureZeroBackgroundVelocity;
			}

			float velocityScale = velocityNormalizationTime / Fmax(TheTimeMgr->GetSystemFloatDeltaTime(), 1.0F);

			if (TheGraphicsMgr->BeginStructureRendering(camera->GetPreviousWorldTransform(), structureFlags, velocityScale))
			{
				TheGraphicsMgr->DrawStructure(&renderStageList[kRenderStageCover]);
				TheGraphicsMgr->DrawStructure(&renderStageList[kRenderStageAmbientDefault], true);
				TheGraphicsMgr->DrawStructure(&renderStageList[kRenderStageLightDefault], true);
				TheGraphicsMgr->DrawStructure(&renderStageList[kRenderStageAmbientAlphaTest], true);
				TheGraphicsMgr->DrawStructure(&renderStageList[kRenderStageLightAlphaTest], true);
				TheGraphicsMgr->DrawStructure(&ambientImpostorSystemArray);
				TheGraphicsMgr->DrawStructure(&lightImpostorSystemArray);
				TheGraphicsMgr->DrawStructure(&renderStageList[kRenderStageEffectVelocity]);

				TheGraphicsMgr->EndStructureRendering();
			}
		}

		TheGraphicsMgr->Draw(&renderStageList[kRenderStageLightDefault], true);
		renderStageList[kRenderStageLightDefault].RemoveAll();

		TheGraphicsMgr->Draw(&renderStageList[kRenderStageLightAlphaTest], true);
		renderStageList[kRenderStageLightAlphaTest].RemoveAll();

		TheGraphicsMgr->Draw(&lightImpostorSystemArray);
		lightImpostorSystemArray.Clear();

		TheGraphicsMgr->SetDrawShaderType(kShaderAmbient);

		TheGraphicsMgr->Draw(&renderStageList[kRenderStageCover]);
		renderStageList[kRenderStageCover].RemoveAll();

		TheGraphicsMgr->Draw(&renderStageList[kRenderStageAmbientDefault], true);
		renderStageList[kRenderStageAmbientDefault].RemoveAll();

		TheGraphicsMgr->Draw(&renderStageList[kRenderStageAmbientAlphaTest], true);
		renderStageList[kRenderStageAmbientAlphaTest].RemoveAll();

		TheGraphicsMgr->Draw(&ambientImpostorSystemArray);
		ambientImpostorSystemArray.Clear();

		TheGraphicsMgr->Draw(&skyboxRenderArray);
		skyboxRenderArray.Clear();

		TheGraphicsMgr->Draw(&renderStageList[kRenderStageDecal], true);
		renderStageList[kRenderStageDecal].RemoveAll();

		TheGraphicsMgr->SetDrawShaderType(kShaderUnified);

		TheGraphicsMgr->Draw(&renderStageList[kRenderStageEffectLight], true);
		renderStageList[kRenderStageEffectLight].RemoveAll();

		if (shadowFlag)
		{
			TheGraphicsMgr->DiscardInfiniteShadow();
		}
	}
	else
	{
		cameraRegion = camera->GetFirstRootRegion();
		do
		{
			RenderAmbientRegion(worldContext, cameraRegion);
			cameraRegion = cameraRegion->Next();
		} while (cameraRegion);

		unsigned_int32 ambientStamp = ambientRenderStamp;
		for (TerrainLevelGeometry *terrain : terrainUpdateArray)
		{
			terrain->UpdateBorderState(kTerrainStampAmbient, ambientStamp);
		}

		terrainUpdateArray.Clear();

		ImpostorSystem *system = impostorSystemMap.First();
		while (system)
		{
			system->Render(&ambientImpostorSystemArray, kImpostorRenderAmbient);
			system = system->Next();
		}

		FinishWorldBatch();

		unsigned_int32 structureFlags = kStructureRenderVelocity | kStructureRenderDepth;
		if (worldFlags & kWorldClearColor)
		{
			structureFlags |= kStructureClearBuffer;
		}

		if (worldContext->skyboxFlag)
		{
			Skybox *skybox = worldContext->skyboxNode;
			if (skybox)
			{
				skybox->Render(camera, &skyboxRenderArray);
				if (!skyboxRenderArray.Empty())
				{
					structureFlags |= kStructureClearBuffer;
				}
			}
		}

		if ((worldContext->perspectiveFlags & (kPerspectivePrimary | (kPerspectivePrimary << kPerspectiveCameraWidgetShift))) != 0)
		{
			if (worldFlags & kWorldMotionBlurInhibit)
			{
				structureFlags &= ~kStructureRenderVelocity;
			}
			else if (worldFlags & kWorldZeroBackgroundVelocity)
			{
				structureFlags |= kStructureZeroBackgroundVelocity;
			}

			float velocityScale = velocityNormalizationTime / Fmax(TheTimeMgr->GetSystemFloatDeltaTime(), 1.0F);

			if (TheGraphicsMgr->BeginStructureRendering(camera->GetPreviousWorldTransform(), structureFlags, velocityScale))
			{
				TheGraphicsMgr->DrawStructure(&renderStageList[kRenderStageCover]);
				TheGraphicsMgr->DrawStructure(&renderStageList[kRenderStageAmbientDefault], true);
				TheGraphicsMgr->DrawStructure(&renderStageList[kRenderStageAmbientAlphaTest], true);
				TheGraphicsMgr->DrawStructure(&ambientImpostorSystemArray);
				TheGraphicsMgr->DrawStructure(&renderStageList[kRenderStageEffectVelocity]);

				TheGraphicsMgr->EndStructureRendering();
			}
		}

		TheGraphicsMgr->Draw(&renderStageList[kRenderStageCover]);
		renderStageList[kRenderStageCover].RemoveAll();

		TheGraphicsMgr->Draw(&renderStageList[kRenderStageAmbientDefault], true);
		renderStageList[kRenderStageAmbientDefault].RemoveAll();

		TheGraphicsMgr->Draw(&renderStageList[kRenderStageAmbientAlphaTest], true);
		renderStageList[kRenderStageAmbientAlphaTest].RemoveAll();

		TheGraphicsMgr->Draw(&ambientImpostorSystemArray);
		ambientImpostorSystemArray.Clear();

		TheGraphicsMgr->Draw(&skyboxRenderArray);
		skyboxRenderArray.Clear();

		TheGraphicsMgr->Draw(&renderStageList[kRenderStageDecal], true);
		renderStageList[kRenderStageDecal].RemoveAll();

		BlobParticleSystem::FinishBatches(&renderStageList[kRenderStageFirstEffect]);

		TheGraphicsMgr->Draw(&renderStageList[kRenderStageEffectLight], true);
		renderStageList[kRenderStageEffectLight].RemoveAll();
	}

	if (!(worldFlags & kWorldAmbientOnly))
	{
		TheGraphicsMgr->SetDrawShaderType(kShaderAmbient);
		TheGraphicsMgr->SetAmbientDepthLessEqual(true);

		TheGraphicsMgr->Draw(&renderStageList[kRenderStageEffectOcclusion]);

		for (Light *light : visibleLightArray)
		{
			if (light != worldContext->unifiedLight)
			{
				RenderLight(worldContext, light);
			}
		}

		visibleLightArray.Clear();

		TheGraphicsMgr->SetAmbientLight();
		TheGraphicsMgr->SetAmbientDepthLessEqual(false);

		TheGraphicsMgr->Draw(&renderStageList[kRenderStageEffectOpaque]);
		TheGraphicsMgr->Draw(&renderStageList[kRenderStageEffectVelocity]);
		TheGraphicsMgr->Draw(&renderStageList[kRenderStageEffectCover]);

		TheGraphicsMgr->Sort(&renderStageList[kRenderStageEffectTransparent]);
		TheGraphicsMgr->Draw(&renderStageList[kRenderStageEffectTransparent]);
		TheGraphicsMgr->Draw(&renderStageList[kRenderStageEffectFrontmost]);

		TheGraphicsMgr->ProcessOcclusionQueries();
	}

	TheGraphicsMgr->SetFogSpace(nullptr, nullptr);

	if ((!renderStageList[kRenderStageEffectDistortion].Empty()) && (worldContext->perspectiveFlags == kPerspectivePrimary))
	{
		if (TheGraphicsMgr->BeginDistortionRendering())
		{
			TheGraphicsMgr->DrawDistortion(&renderStageList[kRenderStageEffectDistortion]);
			TheGraphicsMgr->EndDistortionRendering();
		}
	}

	for (machine a = kRenderStageFirstEffect; a <= kRenderStageLastEffect; a++)
	{
		renderStageList[a].RemoveAll();
	}

	clippedCameraRegionList.Purge();

	#if C4DIAGS

		#if C4CONSOLE //[ CONSOLE

			// -- Console code hidden --

		#endif //]

		for (;;)
		{
			const LightRegionOutline *outline = lightRegionDiagnosticList.First();
			if (!outline)
			{
				break;
			}

			RenderLightRegionOutline(&outline->regionPolyhedron);
			delete outline;
		}

		if (!shadowRegionDiagnosticList.Empty())
		{
			TheGraphicsMgr->Draw(&shadowRegionDiagnosticList);
			TheGraphicsMgr->DrawWireframe(kWireframeTwoSided | kWireframeColor, &shadowRegionDiagnosticList);
		}

		if (diagnosticFlags & kDiagnosticSourcePaths)
		{
			cameraRegion = camera->GetFirstRootRegion();
			do
			{
				RenderSourcePaths(cameraRegion->GetZone(), TheSoundMgr->GetListenerTransformable()->GetWorldTransform());
				cameraRegion = cameraRegion->Next();
			} while (cameraRegion);
		}

		TheGraphicsMgr->Draw(&rigidBodyDiagnosticList);
		TheGraphicsMgr->Draw(&contactDiagnosticList);

		#if C4CONSOLE //[ CONSOLE

			// -- Console code hidden --

		#endif //]

	#endif
}

void World::RenderDistantCamera(WorldContext *worldContext, RenderTargetType target)
{
	currentWorldContext = worldContext;

	TheGraphicsMgr->SetRenderTarget(target);

	const FrustumCamera *camera = worldContext->renderCamera;
	TheGraphicsMgr->SetCamera(camera->GetObject(), camera);

	const RootCameraRegion *cameraRegion = camera->GetFirstRootRegion();
	while (cameraRegion)
	{
		const Zone *zone = cameraRegion->GetZone();
		if (zone->GetObject()->GetZoneFlags() & kZoneRenderSkybox)
		{
			Skybox *skybox = worldContext->skyboxNode;
			if (skybox)
			{
				const FogSpace *cameraFogSpace = nullptr;
				worldContext->fogSpacePtr = &cameraFogSpace;

				const FogSpace *fogSpace = zone->GetFirstFogSpace();
				while (fogSpace)
				{
					if ((fogSpace->Enabled()) && (ProcessFogSpace(worldContext, fogSpace, cameraRegion)))
					{
						break;
					}

					fogSpace = fogSpace->Next();
				}

				if (!fogSpace)
				{
					fogSpace = zone->GetConnectedFogSpace();
					if ((fogSpace) && (fogSpace->Enabled()))
					{
						ProcessFogSpace(worldContext, fogSpace, cameraRegion);
					}
				}

				if (cameraFogSpace)
				{
					TheGraphicsMgr->SetFogSpace(cameraFogSpace->GetObject(), cameraFogSpace);
				}

				skybox->Render(camera, &skyboxRenderArray);
				TheGraphicsMgr->Draw(&skyboxRenderArray);
				skyboxRenderArray.Clear();

				TheGraphicsMgr->SetFogSpace(nullptr, nullptr);
				break;
			}
		}

		cameraRegion = cameraRegion->Next();
	}
}

void World::BeginRendering(void)
{
	if (worldFlags & kWorldPostColorMatrix)
	{
		TheGraphicsMgr->SetFinalColorTransform(finalColorScale[0], finalColorScale[1], finalColorScale[2], finalColorBias);
	}
	else
	{
		TheGraphicsMgr->SetFinalColorTransform(finalColorScale[0], finalColorBias);
	}

	TheGraphicsMgr->SetShaderTime(shaderTime, TheTimeMgr->GetFloatDeltaTime());

	#if C4CONSOLE //[ CONSOLE

		// -- Console code hidden --

	#endif //]
}

void World::EndRendering(void)
{
	#if C4CONSOLE //[ CONSOLE

		// -- Console code hidden --

	#endif //]

	TheGraphicsMgr->SetDisplayRenderTarget();

	shaderTime = PositiveFrac((shaderTime + TheTimeMgr->GetFloatDeltaTime()) * kInverseShaderTimePeriod) * kShaderTimePeriod;

	#if C4DIAGS

		diagnosticFlags &= ~kDiagnosticShadowRegions;

	#endif
}

void World::SetFinalColorTransform(const ColorRGBA& scale, const ColorRGBA& bias)
{
	finalColorScale[0] = scale;
	finalColorBias = bias;
	worldFlags &= ~kWorldPostColorMatrix;
}

void World::SetFinalColorTransform(const ColorRGBA& red, const ColorRGBA& green, const ColorRGBA& blue, const ColorRGBA& bias)
{
	finalColorScale[0] = red;
	finalColorScale[1] = green;
	finalColorScale[2] = blue;
	finalColorBias = bias;
	worldFlags |= kWorldPostColorMatrix;
}

void World::Render(void)
{
	FrustumCamera *camera = currentCamera;
	if (camera)
	{
		ImpostorSystem *system = impostorSystemMap.First();
		while (system)
		{
			system->Build();
			system = system->Next();
		}

		remoteRecursionCount = 0;
		cameraRecursionCount = 0;

		WorldContext worldContext(camera);

		RootCameraRegion *cameraRegion = camera->GetFirstRootRegion();
		while (cameraRegion)
		{
			camera->CalculateFrustumCameraRegion(cameraRegion);
			cameraRegion = cameraRegion->Next();
		}

		Skybox *skybox = worldSkybox;
		worldContext.skyboxNode = ((skybox) && (skybox->Enabled())) ? skybox : nullptr;

		worldContext.perspectiveFlags = kPerspectivePrimary | worldPerspective;
		worldContext.lightDetailLevel = TheWorldMgr->GetLightDetailLevel();
		worldContext.cameraMinDetailLevel = 0;
		worldContext.cameraDetailBias = 0.0F;

		RenderCamera(&worldContext, kRenderTargetPrimary);
	}
}

#if C4DIAGS

	void World::RenderLightRegionOutline(const Polyhedron *polyhedron)
	{
		if (polyhedron->faceCount != 0)
		{
			if (!lightRegionVertexBuffer.Active())
			{
				lightRegionVertexBuffer.Establish(sizeof(Point3D) * kMaxPolyhedronVertexCount);
				lightRegionIndexBuffer.Establish(sizeof(Line) * kMaxPolyhedronEdgeCount);
			}

			volatile Point3D *restrict vertex = lightRegionVertexBuffer.BeginUpdate<Point3D>();
			volatile Line *restrict line = lightRegionIndexBuffer.BeginUpdate<Line>();

			int32 vertexCount = polyhedron->vertexCount;
			int32 edgeCount = polyhedron->edgeCount;

			for (machine a = 0; a < vertexCount; a++)
			{
				vertex[a] = polyhedron->vertex[a];
			}

			for (machine a = 0; a < edgeCount; a++)
			{
				line[a].index[0] = polyhedron->edge[a].vertexIndex[0];
				line[a].index[1] = polyhedron->edge[a].vertexIndex[1];
			}

			lightRegionIndexBuffer.EndUpdate();
			lightRegionVertexBuffer.EndUpdate();

			lightRegionRenderable.SetVertexCount(vertexCount);
			lightRegionRenderable.SetPrimitiveCount(edgeCount);
			TheGraphicsMgr->Draw(&lightRegionRenderList);
		}
	}

	void World::RenderSourcePaths(Zone *zone, const Transform4D& listenerTransform)
	{
		if (!sourcePathVertexBuffer.Active())
		{
			sourcePathVertexBuffer.Establish(sizeof(Point3D) * 2);
		}

		const SourceRegion *sourceRegion = zone->GetFirstSourceRegion();
		while (sourceRegion)
		{
			const OmniSource *source = sourceRegion->GetSource();
			if (source->sourceState & kSourceEngaged)
			{
				const SourceRegion *region = sourceRegion->GetPrimaryRegion();

				volatile Point3D *restrict vertex = sourcePathVertexBuffer.BeginUpdate<Point3D>();
				vertex[0] = listenerTransform.GetTranslation() + listenerTransform[2];
				vertex[1] = region->GetAudiblePosition();
				sourcePathVertexBuffer.EndUpdate();

				TheGraphicsMgr->Draw(&sourcePathRenderList);

				const SourceRegion *superRegion = region->GetSuperNode();
				while (superRegion)
				{
					const SourceRegion *nextRegion = superRegion->GetSuperNode();
					if (superRegion->GetAudibleSubregion() == region)
					{
						vertex = sourcePathVertexBuffer.BeginUpdate<Point3D>();
						vertex[0] = region->GetAudiblePosition();
						vertex[1] = superRegion->GetAudiblePosition();
						sourcePathVertexBuffer.EndUpdate();

						TheGraphicsMgr->Draw(&sourcePathRenderList);

						region = superRegion;
					}

					superRegion = nextRegion;
				}
			}

			sourceRegion = sourceRegion->Next();
		}
	}

#endif


WorldMgr::WorldMgr(int) :
		objectCreator(&CreateObject),
		controllerStateSender(&SendControllerState, this),
		displayEventHandler(&HandleDisplayEvent, this),
		lightDetailLevelObserver(this, &WorldMgr::HandleLightDetailLevelEvent)
{
}

WorldMgr::~WorldMgr()
{
}

EngineResult WorldMgr::Construct(void)
{
	currentWorld = nullptr;
	warmupWorld = nullptr;

	worldCreatorProc = nullptr;
	Object::InstallCreator(&objectCreator);
	TheMessageMgr->InstallStateSender(&controllerStateSender);
	TheDisplayMgr->InstallDisplayEventHandler(&displayEventHandler);

	unsigned_int32 speedRank = TheGraphicsMgr->GetCapabilities()->hardwareSpeedRank;
	TheEngine->InitVariable("lightDetailLevel", (speedRank >= 2) ? "3" : ((speedRank >= 1) ? "2" : "1"), kVariablePermanent, &lightDetailLevelObserver);

	defaultVelocityNormalizationTime = 8.33333F;
	trackingOrientation.Set(0.0F, 0.0F, 0.0F, 1.0F);

	Controller::RegisterStandardControllers();
	Property::RegisterStandardProperties();
	Modifier::RegisterStandardModifiers();
	Mutator::RegisterStandardMutators();
	Process::RegisterStandardProcesses();
	Widget::RegisterStandardWidgets();
	Method::RegisterStandardMethods();
	Force::RegisterStandardForces();
	ParticleSystem::RegisterStandardParticleSystems();

	#if C4DIAGS

		lightRegionRenderList.Append(&lightRegionRenderable);
		lightRegionRenderable.SetRenderableFlags(kRenderableFogInhibit);
		lightRegionRenderable.SetShaderFlags(kShaderAmbientEffect);
		lightRegionRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &lightRegionVertexBuffer, sizeof(Point3D));
		lightRegionRenderable.SetVertexBuffer(kVertexBufferIndexArray, &lightRegionIndexBuffer);
		lightRegionRenderable.SetVertexAttributeArray(kArrayPosition, 0, 3);
		lightRegionAttributeList.Append(&lightRegionDiffuseColor);
		lightRegionRenderable.SetMaterialAttributeList(&lightRegionAttributeList);

		sourcePathRenderList.Append(&sourcePathRenderable);
		sourcePathRenderable.SetRenderableFlags(kRenderableFogInhibit);
		sourcePathRenderable.SetShaderFlags(kShaderAmbientEffect);
		sourcePathRenderable.SetVertexCount(2);
		sourcePathRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &sourcePathVertexBuffer, sizeof(Point3D));
		sourcePathRenderable.SetVertexAttributeArray(kArrayPosition, 0, 3);
		sourcePathAttributeList.Append(&sourcePathDiffuseColor);
		sourcePathRenderable.SetMaterialAttributeList(&sourcePathAttributeList);

	#endif

	new(loaderSignal) Signal(2);
	new(loaderThread) Thread(&LoaderThread, this, 0, loaderSignal);

	return (kEngineOkay);
}

void WorldMgr::Destruct(void)
{
	loaderThread->~Thread();
	loaderSignal->~Signal();

	#if C4DIAGS

		sourcePathVertexBuffer.Establish(0);
		lightRegionIndexBuffer.Establish(0);
		lightRegionVertexBuffer.Establish(0);

	#endif

	displayEventHandler.Detach();
	controllerStateSender.Detach();
	Object::RemoveCreator(&objectCreator);

	TheResourceMgr->ReleaseCache(AnimationResource::GetDescriptor());
}

Object *WorldMgr::CreateObject(Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (data.GetType())
	{
		case kObjectProperty:

			return (new PropertyObject);

		case kObjectMaterial:

			return (new MaterialObject);

		case kObjectScript:

			return (new ScriptObject);

		case kObjectTerrainBlock:

			return (new TerrainBlockObject);

		case kObjectWaterBlock:

			return (new WaterBlockObject);

		case kObjectGeometry:

			return (GeometryObject::Create(++data, unpackFlags));

		case kObjectSource:

			return (SourceObject::Create(++data, unpackFlags));

		case kObjectPortal:

			return (PortalObject::Create(++data, unpackFlags));

		case kObjectZone:

			return (ZoneObject::Create(++data, unpackFlags));

		case kObjectTrigger:

			return (TriggerObject::Create(++data, unpackFlags));

		case kObjectEffect:

			return (EffectObject::Create(++data, unpackFlags));

		case kObjectEmitter:

			return (EmitterObject::Create(++data, unpackFlags));

		case kObjectShape:

			return (ShapeObject::Create(++data, unpackFlags));

		case kObjectJoint:

			return (JointObject::Create(++data, unpackFlags));

		case kObjectField:

			return (FieldObject::Create(++data, unpackFlags));

		case kObjectBlocker:

			return (BlockerObject::Create(++data, unpackFlags));
	}

	return (nullptr);
}

void WorldMgr::SendControllerState(Player *to, void *cookie)
{
	World *world = static_cast<WorldMgr *>(cookie)->GetWorld();
	if (world)
	{
		int32 bucketCount = world->controllerTable.GetBucketCount();
		for (machine a = 0; a < bucketCount; a++)
		{
			Controller *controller = world->controllerTable.GetFirstBucketElement(a);
			while (controller)
			{
				controller->SendInitialStateMessages(to);
				controller = controller->HashTableElement<Controller>::Next();
			}
		}
	}
}

void WorldMgr::HandleDisplayEvent(const DisplayEventData *eventData, void *cookie)
{
	if (eventData->eventType == kEventDisplayChange)
	{
		WorldMgr *worldMgr = static_cast<WorldMgr *>(cookie);

		World *world = worldMgr->currentWorld;
		if (world)
		{
			int32 width = TheDisplayMgr->GetDisplayWidth();
			int32 height = TheDisplayMgr->GetDisplayHeight();
			world->SetRenderSize(width, height);
		}
	}
}

void WorldMgr::HandleLightDetailLevelEvent(Variable *variable)
{
	lightDetailLevel = MaxZero(Min(variable->GetIntegerValue(), 3));
}

WorldResult WorldMgr::LoadWorld(const char *name)
{
	UnloadWorld();

	World *world = (worldCreatorProc) ? (*worldCreatorProc)(name, worldCreatorCookie) : new World(name);
	if (world->Preprocess() != kWorldOkay)
	{
		delete world;
		return (kWorldLoadFailed);
	}

	RunWorld(world);
	return (kWorldOkay);
}

void WorldMgr::UnloadWorld(void)
{
	if (currentWorld)
	{
		TheMessageMgr->SetControllerMessageProcs(nullptr, nullptr);

		delete currentWorld;
		currentWorld = nullptr;

		MaterialObject::ReleaseCache();
		ShaderProgram::ReleaseCache();
	}
}

void WorldMgr::RunWorld(World *world)
{
	currentWorld = world;

	TheMessageMgr->SetControllerMessageProcs(&World::CreateControllerMessage, &World::ReceiveControllerMessage, world);
	TheTimeMgr->ResetTime();
}

void WorldMgr::SaveDeltaWorld(const char *name)
{
	const World *world = currentWorld;
	if (world)
	{
		File			file;
		ResourcePath	path;

		TheResourceMgr->GetSaveCatalog()->GetResourcePath(SaveResource::GetDescriptor(), name, &path);
		if ((FileMgr::CreateDirectoryPath(path) == kFileOkay) && (file.Open(path, kFileCreate) == kFileOkay))
		{
			world->GetRootNode()->PackDeltaTree(&file, world->GetWorldName());
		}
	}
}

WorldResult WorldMgr::RestoreDeltaWorld(const char *name)
{
	World *world = (worldCreatorProc) ? (*worldCreatorProc)(name, worldCreatorCookie) : new World(name);
	world->SetWorldFlags(world->GetWorldFlags() | kWorldRestore);
	world->previousWorld = currentWorld;
	currentWorld = nullptr;

	if (world->Preprocess() != kWorldOkay)
	{
		delete world;
		return (kWorldLoadFailed);
	}

	RunWorld(world);
	return (kWorldOkay);
}

void WorldMgr::LoaderThread(const Thread *thread, void *cookie)
{
	Thread::SetThreadName("C4-WD Loader");

	WorldMgr *worldMgr = static_cast<WorldMgr *>(cookie);

	for (;;)
	{
		int32 index = worldMgr->loaderSignal->Wait();
		if (index == 0)
		{
			break;
		}

		TheGraphicsMgr->SetSyncLoadFlag(true);
		(*worldMgr->loaderProc)(worldMgr->loaderCookie);
		TheGraphicsMgr->SetSyncLoadFlag(false);
	}
}

void WorldMgr::RunLoaderTask(void (*proc)(void *), void *cookie)
{
	loaderProc = proc;
	loaderCookie = cookie;
	loaderSignal->Trigger(1);
}

void WorldMgr::Move(void)
{
	World *world = currentWorld;
	if (world)
	{
		if (!(world->GetWorldFlags() & kWorldPaused))
		{
			#if C4OCULUS

				Oculus::ReadOrientation(&trackingOrientation);

			#endif

			world->Move();
			world->Update();
			world->Interact();
		}
		else
		{
			#if C4STATS

				for (machine a = 0; a < kWorldCounterRenderCount; a++)
				{
					world->worldCounter[a] = 0;
				}

			#endif

			world->Update();
		}

		world->GetRootNode()->Update();
		world->Listen();
	}
}

void WorldMgr::Render(void)
{
	TheGraphicsMgr->BeginRendering();

	#if C4OCULUS

		if (TheDisplayMgr->GetDisplayFlags() & kDisplayOculus)
		{
			float ipd = Oculus::GetInterpupillaryDistance();

			World *world = currentWorld;
			if (world)
			{
				world->BeginRendering();

				Camera *camera = world->GetCamera();
				camera->SetWorldPosition(camera->GetWorldPosition() - camera->GetWorldTransform()[0] * (ipd * 0.5F));

				world->Render();
				TheGraphicsMgr->SetDisplayRenderTarget();
			}

			TheInterfaceMgr->Render();
			TheGraphicsMgr->SetFullFrameRenderTarget();

			TheGraphicsMgr->SetCameraLensMultiplier(-1.0F);

			if (world)
			{
				Camera *camera = world->GetCamera();
				camera->SetWorldPosition(camera->GetWorldPosition() + camera->GetWorldTransform()[0] * ipd);

				world->Render();
				world->EndRendering();
			}
			else if (warmupWorld)
			{
				Warmup();
			}

			TheInterfaceMgr->Render();
			TheGraphicsMgr->SetFullFrameRenderTarget();
		}
		else
		{
			World *world = currentWorld;
			if (world)
			{
				world->BeginRendering();
				world->Render();
				world->EndRendering();
			}
			else if (warmupWorld)
			{
				Warmup();
			}

			TheInterfaceMgr->Render();
			TheGraphicsMgr->SetFullFrameRenderTarget();
		}

	#else

		World *world = currentWorld;
		if (world)
		{
			world->BeginRendering();
			world->Render();
			world->EndRendering();
		}
		else if (warmupWorld)
		{
			Warmup();
		}

		TheInterfaceMgr->Render();
		TheGraphicsMgr->SetFullFrameRenderTarget();

	#endif

	TheMovieMgr->RecordTask();

	TheGraphicsMgr->EndRendering();
}

void WorldMgr::Warmup(void)
{
	CubeCamera		cubeCamera;

	World *world = warmupWorld;

	cubeCamera.SetNodeTransform(world->warmupShaderMarker->GetWorldTransform());

	FrustumCameraObject *cameraObject = cubeCamera.GetObject();
	cameraObject->SetViewRect(Rect(0, 0, 64, 64));

	FrustumCamera *savedCamera = world->currentCamera;
	world->SetCamera(&cubeCamera);

	cubeCamera.SetFaceIndex(world->warmupShaderFace);
	cubeCamera.Invalidate();
	cubeCamera.Update();

	cubeCamera.UpdateRootRegions(static_cast<Zone *>(world->rootNode));

	world->BeginRendering();
	world->Render();
	world->EndRendering();

	world->SetCamera(savedCamera);
}

// ZYUQURM
