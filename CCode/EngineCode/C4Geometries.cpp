 

#include "C4Geometries.h"
#include "C4Terrain.h"
#include "C4Water.h"
#include "C4World.h"
#include "C4Configuration.h"


using namespace C4;


const char C4::kConnectorKeyPaint[] = "%Paint";


Geometry::Geometry(GeometryType type) : RenderableNode(kNodeGeometry, kRenderIndexedTriangles, kRenderDepthTest)
{
	geometryType = type;

	minGeometryDetailLevel = 0;

	processStamp = 0xFFFFFFFF;
	lightStamp = 0xFFFFFFFF;
	shadowStamp = 0xFFFFFFFF;
	collisionStamp = 0xFFFFFFFF;
	queryThreadFlags = 0;

	geometryVelocity.Set(0.0F, 0.0F, 0.0F);

	materialCount = 1;
	materialObject = nullptr;

	RenderSegment *segment = GetFirstRenderSegment();
	segment->SetMaterialObjectPointer(&materialObject);
	segmentStorage = nullptr;

	connectedPaintSpace = nullptr;
}

Geometry::Geometry(const Geometry& geometry) : RenderableNode(geometry)
{
	geometryType = geometry.geometryType;

	minGeometryDetailLevel = geometry.minGeometryDetailLevel;

	processStamp = 0xFFFFFFFF;
	lightStamp = 0xFFFFFFFF;
	shadowStamp = 0xFFFFFFFF;
	collisionStamp = 0xFFFFFFFF;
	queryThreadFlags = 0;

	geometryVelocity.Set(0.0F, 0.0F, 0.0F);

	materialCount = 1;
	materialObject = geometry.materialObject;
	if (materialObject)
	{
		materialObject->Retain();
	}

	RenderSegment *segment = GetFirstRenderSegment();
	segment->SetMaterialObjectPointer(&materialObject);
	segmentStorage = nullptr;

	connectedPaintSpace = nullptr;

	int32 count = geometry.materialCount;
	if (count > 1)
	{
		SetMaterialCount(count);

		count--;
		for (machine a = 0; a < count; a++)
		{
			MaterialObject *object = geometry.GetMaterialObjectTable()[a];
			if (object)
			{
				object->Retain();
				GetMaterialObjectTable()[a] = object;
			}
		}
	}
}

Geometry::~Geometry()
{
	ReleaseSegmentStorage();

	if (materialObject)
	{
		materialObject->Release();
	}
}

Geometry *Geometry::Create(Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (data.GetType())
	{
		case kGeometryGeneric:

			return (new GenericGeometry);

		case kGeometryPrimitive: 

			return (PrimitiveGeometry::Create(++data, unpackFlags));
 
		case kGeometryTerrain:
 
			if ((++data).GetType() == 0)
			{
				return (new TerrainGeometry); 
			}
 
			return (new TerrainLevelGeometry); 

		case kGeometryWater:

			return (new WaterGeometry); 

		case kGeometryHorizonWater:

			return (new HorizonWaterGeometry);
	}

	return (nullptr);
}

void Geometry::PackType(Packer& data) const
{
	RenderableNode::PackType(data);
	data << geometryType;
}

void Geometry::Prepack(List<Object> *linkList) const
{
	RenderableNode::Prepack(linkList);

	if (materialObject)
	{
		linkList->Append(materialObject);
	}

	int32 count = materialCount;
	if (count > 1)
	{
		count--;
		for (machine a = 0; a < count; a++)
		{
			MaterialObject *object = GetMaterialObjectTable()[a];
			if (object)
			{
				linkList->Append(object);
			}
		}
	}
}

void Geometry::Pack(Packer& data, unsigned_int32 packFlags) const
{
	RenderableNode::Pack(data, packFlags);

	if (!(packFlags & kPackSettings))
	{
		data << ChunkHeader('MAT0', 4);

		const MaterialObject *object = materialObject;
		int32 objectIndex = (object) ? object->GetObjectIndex() : -1;
		data << objectIndex;

		int32 count = materialCount;
		if (count > 1)
		{
			data << ChunkHeader('MCNT', 4);
			data << count;

			count--;
			for (machine a = 0; a < count; a++)
			{
				object = GetMaterialObjectTable()[a];
				if (object)
				{
					data << ChunkHeader('MATL', 8);
					data << int32(a);
					data << object->GetObjectIndex();
				}
			}
		}
	}

	data << TerminatorChunk;
}

void Geometry::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	RenderableNode::Unpack(data, unpackFlags);
	UnpackChunkList<Geometry>(data, unpackFlags);
}

bool Geometry::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'MAT0':
		{
			int32	objectIndex;

			data >> objectIndex;
			if (objectIndex >= 0)
			{
				data.AddObjectLink(objectIndex, &MaterialObjectLinkProc, &materialObject);
			}

			return (true);
		}

		case 'MCNT':
		{
			int32	count;

			data >> count;
			SetMaterialCount(count);
			return (true);
		}

		case 'MATL':
		{
			int32	index;
			int32	objectIndex;

			data >> index;
			data >> objectIndex;
			data.AddObjectLink(objectIndex, &MaterialObjectLinkProc, &GetMaterialObjectTable()[index]);
			return (true);
		}
	}

	return (false);
}

void Geometry::MaterialObjectLinkProc(Object *object, void *cookie)
{
	*static_cast<MaterialObject **>(cookie) = static_cast<MaterialObject *>(object);
	object->Retain();
}

int32 Geometry::GetCategorySettingCount(Type category) const
{
	int32 count = RenderableNode::GetCategorySettingCount(category);
	if (category == 'NODE')
	{
		count += kPerspectiveMaskSettingCount + 10;
	}

	return (count);
}

Setting *Geometry::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == 'NODE')
	{
		int32 i = index - RenderableNode::GetCategorySettingCount('NODE');
		if (i >= 0)
		{
			if (i < kPerspectiveMaskSettingCount)
			{
				return (GetPerspectiveMaskSetting(i));
			}

			const StringTable *table = TheInterfaceMgr->GetStringTable();

			if (i == kPerspectiveMaskSettingCount)
			{
				const char *title = table->GetString(StringID(kNodeGeometry, 'SPRS'));
				return (new HeadingSetting('SPRS', title));
			}

			if (i == kPerspectiveMaskSettingCount + 1)
			{
				const char *title = table->GetString(StringID('NODE', 'PRSP', 'DPRM'));
				return (new BooleanSetting('DSPR', (((GetPerspectiveExclusionMask() >> kPerspectiveDirectShadowShift) & kPerspectivePrimary) == 0), title));
			}

			if (i == kPerspectiveMaskSettingCount + 2)
			{
				const char *title = table->GetString(StringID('NODE', 'PRSP', 'DRFL'));
				return (new BooleanSetting('DSFL', (((GetPerspectiveExclusionMask() >> kPerspectiveDirectShadowShift) & kPerspectiveReflection) == 0), title));
			}

			if (i == kPerspectiveMaskSettingCount + 3)
			{
				const char *title = table->GetString(StringID('NODE', 'PRSP', 'DRFR'));
				return (new BooleanSetting('DSFR', (((GetPerspectiveExclusionMask() >> kPerspectiveDirectShadowShift) & kPerspectiveRefraction) == 0), title));
			}

			if (i == kPerspectiveMaskSettingCount + 4)
			{
				const char *title = table->GetString(StringID('NODE', 'PRSP', 'RPRM'));
				return (new BooleanSetting('RSPR', (((GetPerspectiveExclusionMask() >> kPerspectiveRemotePortalShadowShift) & kPerspectivePrimary) == 0), title));
			}

			if (i == kPerspectiveMaskSettingCount + 5)
			{
				const char *title = table->GetString(StringID('NODE', 'PRSP', 'RRFL'));
				return (new BooleanSetting('RSFL', (((GetPerspectiveExclusionMask() >> kPerspectiveRemotePortalShadowShift) & kPerspectiveReflection) == 0), title));
			}

			if (i == kPerspectiveMaskSettingCount + 6)
			{
				const char *title = table->GetString(StringID('NODE', 'PRSP', 'RRFR'));
				return (new BooleanSetting('RSFR', (((GetPerspectiveExclusionMask() >> kPerspectiveRemotePortalShadowShift) & kPerspectiveRefraction) == 0), title));
			}

			if (i == kPerspectiveMaskSettingCount + 7)
			{
				const char *title = table->GetString(StringID('NODE', 'PRSP', 'CPRM'));
				return (new BooleanSetting('CSPR', (((GetPerspectiveExclusionMask() >> kPerspectiveCameraWidgetShadowShift) & kPerspectivePrimary) == 0), title));
			}

			if (i == kPerspectiveMaskSettingCount + 8)
			{
				const char *title = table->GetString(StringID('NODE', 'PRSP', 'CRFL'));
				return (new BooleanSetting('CSFL', (((GetPerspectiveExclusionMask() >> kPerspectiveCameraWidgetShadowShift) & kPerspectiveReflection) == 0), title));
			}

			if (i == kPerspectiveMaskSettingCount + 9)
			{
				const char *title = table->GetString(StringID('NODE', 'PRSP', 'CRFR'));
				return (new BooleanSetting('CSFR', (((GetPerspectiveExclusionMask() >> kPerspectiveCameraWidgetShadowShift) & kPerspectiveRefraction) == 0), title));
			}

			return (nullptr);
		}
	}

	return (RenderableNode::GetCategorySetting(category, index, flags));
}

void Geometry::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == 'NODE')
	{
		if (!SetPerspectiveMaskSetting(setting))
		{
			Type identifier = setting->GetSettingIdentifier();

			if (identifier == 'DSPR')
			{
				if (!static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
				{
					SetPerspectiveExclusionMask(GetPerspectiveExclusionMask() | kPerspectivePrimary << kPerspectiveDirectShadowShift);
				}
				else
				{
					SetPerspectiveExclusionMask(GetPerspectiveExclusionMask() & ~(kPerspectivePrimary << kPerspectiveDirectShadowShift));
				}
			}
			else if (identifier == 'DSFL')
			{
				if (!static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
				{
					SetPerspectiveExclusionMask(GetPerspectiveExclusionMask() | kPerspectiveReflection << kPerspectiveDirectShadowShift);
				}
				else
				{
					SetPerspectiveExclusionMask(GetPerspectiveExclusionMask() & ~(kPerspectiveReflection << kPerspectiveDirectShadowShift));
				}
			}
			else if (identifier == 'DSFR')
			{
				if (!static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
				{
					SetPerspectiveExclusionMask(GetPerspectiveExclusionMask() | kPerspectiveRefraction << kPerspectiveDirectShadowShift);
				}
				else
				{
					SetPerspectiveExclusionMask(GetPerspectiveExclusionMask() & ~(kPerspectiveRefraction << kPerspectiveDirectShadowShift));
				}
			}
			else if (identifier == 'RSPR')
			{
				if (!static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
				{
					SetPerspectiveExclusionMask(GetPerspectiveExclusionMask() | kPerspectivePrimary << kPerspectiveRemotePortalShadowShift);
				}
				else
				{
					SetPerspectiveExclusionMask(GetPerspectiveExclusionMask() & ~(kPerspectivePrimary << kPerspectiveRemotePortalShadowShift));
				}
			}
			else if (identifier == 'RSFL')
			{
				if (!static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
				{
					SetPerspectiveExclusionMask(GetPerspectiveExclusionMask() | kPerspectiveReflection << kPerspectiveRemotePortalShadowShift);
				}
				else
				{
					SetPerspectiveExclusionMask(GetPerspectiveExclusionMask() & ~(kPerspectiveReflection << kPerspectiveRemotePortalShadowShift));
				}
			}
			else if (identifier == 'RSFR')
			{
				if (!static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
				{
					SetPerspectiveExclusionMask(GetPerspectiveExclusionMask() | kPerspectiveRefraction << kPerspectiveRemotePortalShadowShift);
				}
				else
				{
					SetPerspectiveExclusionMask(GetPerspectiveExclusionMask() & ~(kPerspectiveRefraction << kPerspectiveRemotePortalShadowShift));
				}
			}
			else if (identifier == 'CSPR')
			{
				if (!static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
				{
					SetPerspectiveExclusionMask(GetPerspectiveExclusionMask() | kPerspectivePrimary << kPerspectiveCameraWidgetShadowShift);
				}
				else
				{
					SetPerspectiveExclusionMask(GetPerspectiveExclusionMask() & ~(kPerspectivePrimary << kPerspectiveCameraWidgetShadowShift));
				}
			}
			else if (identifier == 'CSFL')
			{
				if (!static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
				{
					SetPerspectiveExclusionMask(GetPerspectiveExclusionMask() | kPerspectiveReflection << kPerspectiveCameraWidgetShadowShift);
				}
				else
				{
					SetPerspectiveExclusionMask(GetPerspectiveExclusionMask() & ~(kPerspectiveReflection << kPerspectiveCameraWidgetShadowShift));
				}
			}
			else if (identifier == 'CSFR')
			{
				if (!static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
				{
					SetPerspectiveExclusionMask(GetPerspectiveExclusionMask() | kPerspectiveRefraction << kPerspectiveCameraWidgetShadowShift);
				}
				else
				{
					SetPerspectiveExclusionMask(GetPerspectiveExclusionMask() & ~(kPerspectiveRefraction << kPerspectiveCameraWidgetShadowShift));
				}
			}
			else
			{
				RenderableNode::SetCategorySetting('NODE', setting);
			}
		}
	}
	else
	{
		RenderableNode::SetCategorySetting(category, setting);
	}
}

int32 Geometry::GetInternalConnectorCount(void) const
{
	return (1);
}

const char *Geometry::GetInternalConnectorKey(int32 index) const
{
	if (index == 0)
	{
		return (kConnectorKeyPaint);
	}

	return (nullptr);
}

void Geometry::ProcessInternalConnectors(void)
{
	Node *node = GetConnectedNode(kConnectorKeyPaint);
	if ((!node) && (GetObject()->GetGeometryFlags() & kGeometryInstancePaintSpace))
	{
		const Node *super = GetSuperNode();
		do
		{
			if (super->GetNodeType() == kNodeInstance)
			{
				node = super->GetConnectedNode(kConnectorKeyPaint);
				if (node)
				{
					break;
				}
			}

			super = super->GetSuperNode();
		} while (super);
	}

	if (node)
	{
		PaintSpace *paintSpace = static_cast<PaintSpace *>(node);
		connectedPaintSpace = paintSpace;
		SetPaintEnvironment(paintSpace->GetPaintEnvironment());
	}
	else
	{
		connectedPaintSpace = nullptr;
	}
}

bool Geometry::ValidConnectedNode(const ConnectorKey& key, const Node *node) const
{
	if (key == kConnectorKeyPaint)
	{
		if (node->GetNodeType() == kNodeSpace)
		{
			return (static_cast<const Space *>(node)->GetSpaceType() == kSpacePaint);
		}

		return (false);
	}

	return (Node::ValidConnectedNode(key, node));
}

void Geometry::SetConnectedPaintSpace(PaintSpace *paintSpace)
{
	connectedPaintSpace = paintSpace;

	if (paintSpace)
	{
		SetPaintEnvironment(paintSpace->GetPaintEnvironment());
	}
	else
	{
		SetNullPaintEnvironment();
	}

	SetConnectedNode(kConnectorKeyPaint, paintSpace);
}

bool Geometry::AlphaTestMaterial(void) const
{
	const MaterialObject *material = materialObject;
	if (material)
	{
		unsigned_int32 flags = material->GetMaterialFlags();

		int32 count = materialCount - 1;
		const MaterialObject *const *materialTable = GetMaterialObjectTable();
		for (machine a = 0; a < count; a++)
		{
			material = materialTable[a];
			if (material)
			{
				flags |= material->GetMaterialFlags();
			}
		}

		return ((flags & kMaterialAlphaTest) != 0);
	}

	return (false);
}

void Geometry::Preload(void)
{
	GetObject()->SetPrototypeFlag();
	RenderableNode::Preload();
}

void Geometry::Preprocess(void)
{
	int32	renderStage;

	SetTransformable(this);
	SetPreviousWorldTransformPointer(&GetPreviousWorldTransform());

	SetVisibilityProc(&BoxVisible);
	SetOcclusionProc(&BoxVisible);

	SetActiveUpdateFlags(GetActiveUpdateFlags() | kUpdateVisibility);

	GeometryObject *object = GetObject();
	unsigned_int32 geometryFlags = object->GetGeometryFlags();

	unsigned_int32 shaderFlags = kShaderColorArrayInhibit;
	if (geometryFlags & kGeometryCubeLightInhibit)
	{
		shaderFlags |= kShaderCubeLightInhibit;
	}

	if (!AlphaTestMaterial())
	{
		renderStage = kRenderStageAmbientDefault;
		lightRenderStage = kRenderStageLightDefault;
		shadowRenderStage = kRenderStageShadowDefault;
	}
	else
	{
		renderStage = kRenderStageAmbientAlphaTest;
		lightRenderStage = kRenderStageLightAlphaTest;
		shadowRenderStage = kRenderStageShadowAlphaTest;
	}

	unsigned_int32 renderState = kRenderDepthTest;
	unsigned_int32 blendState = kBlendReplace;

	if (geometryFlags & kGeometryRemotePortal)
	{
		renderStage = kRenderStageCover;
		blendState = kBlendInterpolate;
	}
	else if (geometryFlags & kGeometryRenderEffectPass)
	{
		renderState |= kRenderDepthInhibit;

		unsigned_int32 geometryEffectFlags = object->GetGeometryEffectFlags();
		blendState = (geometryEffectFlags & kGeometryEffectAccumulate) ? kBlendAccumulate | kBlendAlphaPreserve : kBlendInterpolate | kBlendAlphaPreserve;
		renderStage = (geometryEffectFlags & kGeometryEffectOpaque) ? kRenderStageEffectOpaque : kRenderStageEffectTransparent;

		if (geometryEffectFlags & kGeometryEffectShader)
		{
			shaderFlags |= kShaderAmbientEffect;
		}

		SetTransparentPosition(&GetWorldPosition());
	}

	if (geometryFlags & kGeometryRenderDecal)
	{
		renderStage = (geometryFlags & kGeometryRenderEffectPass) ? kRenderStageEffectOpaque : kRenderStageDecal;
		renderState |= kRenderDepthOffset;
		SetDepthOffset(0.0078125F, &GetBoundingSphere()->GetCenter());
	}

	ambientRenderStage = renderStage;

	SetRenderState(renderState);
	SetShaderFlags(shaderFlags);
	SetAmbientBlendState(blendState);

	unsigned_int32 renderableFlags = GetRenderableFlags() & ~(kRenderableStructureVelocityZero | kRenderableFogInhibit);

	if (geometryFlags & kGeometryMotionBlurInhibit)
	{
		renderableFlags |= kRenderableStructureVelocityZero;
	}

	if (geometryFlags & kGeometryFogInhibit)
	{
		renderableFlags |= kRenderableFogInhibit;
	}

	SetRenderableFlags(renderableFlags);

	RenderableNode::Preprocess();

	if (!(GetNodeFlags() & kNodeVisibilitySite))
	{
		InitializeVisibility();
	}

	object->Preprocess(GetVertexBufferArrayFlags());

	SetVertexBuffer(kVertexBufferAttributeArray, object->GetStaticVertexBuffer(), object->GetStaticVertexStride());
	SetVertexBuffer(kVertexBufferIndexArray, object->GetStaticIndexBuffer());

	positionArrayBundle = nullptr;
	primitiveArrayBundle = nullptr;

	SetDetailLevel(0);
}

void Geometry::Neutralize(void)
{
	GeometryObject *object = GetObject();
	if (object)
	{
		object->Neutralize();
	}

	SetVertexBufferArrayFlags(0);
	SetNullPaintEnvironment();

	RenderableNode::Neutralize();
}

void Geometry::HandleVisibilityUpdate(void)
{
	Node::HandleVisibilityUpdate();

	const Node *node = this;
	for (;;)
	{
		const Bond *bond = node->GetFirstIncomingEdge();
		if (!bond) break;

		const Site *site = bond->GetStartElement();
		if (site->GetCellIndex() >= 0)
		{
			break;
		}

		node = static_cast<const Node *>(site);
	}

	SelectAmbientEnvironment(node);
}

void Geometry::ReleaseSegmentStorage(void)
{
	if (segmentStorage)
	{
		int32 count = materialCount - 1;

		MaterialObject **table = GetMaterialObjectTable();
		for (machine a = 0; a < count; a++)
		{
			MaterialObject *object = table[a];
			if (object)
			{
				object->Release();
			}
		}

		RenderSegment *segment = GetRenderSegmentTable();
		for (machine a = count - 1; a >= 0; a--)
		{
			segment[a].~RenderSegment();
		}

		delete[] segmentStorage;
		segmentStorage = nullptr;
	}
}

void Geometry::SetMaterialCount(int32 count)
{
	if (count <= 1)
	{
		ReleaseSegmentStorage();
		materialCount = 1;
	}
	else
	{
		count--;
		char *newStorage = new char[count * (sizeof(MaterialObject *) + sizeof(RenderSegment))];

		MaterialObject **newTable = reinterpret_cast<MaterialObject **>(newStorage);
		MaterialObject *const *oldTable = GetMaterialObjectTable();

		int32 transferCount = Min(count, materialCount - 1);
		for (machine a = 0; a < transferCount; a++)
		{
			MaterialObject *object = oldTable[a];
			if (object)
			{
				object->Retain();
			}

			newTable[a] = object;
		}

		for (machine a = transferCount; a < count; a++)
		{
			newTable[a] = nullptr;
		}

		RenderSegment *segment = reinterpret_cast<RenderSegment *>(newTable + count);
		for (machine a = 0; a < count; a++)
		{
			new(&segment[a]) RenderSegment(this);
			segment[a].SetMaterialObjectPointer(&newTable[a]);
		}

		ReleaseSegmentStorage();
		segmentStorage = newStorage;
		materialCount = count + 1;
	}
}

void Geometry::SetMaterialObject(unsigned_int32 index, MaterialObject *object)
{
	MaterialObject **pointer = (index == 0) ? &materialObject : &GetMaterialObjectTable()[index - 1];

	MaterialObject *prevObject = *pointer;
	if (prevObject != object)
	{
		if (prevObject)
		{
			prevObject->Release();
		}

		if (object)
		{
			object->Retain();
		}

		*pointer = object;
	}
}

void Geometry::OptimizeMaterials(void)
{
	int32 oldCount = materialCount;
	int32 *remap = new int32[oldCount];
	for (machine a = 0; a < oldCount; a++)
	{
		remap[a] = -1;
	}

	const GeometryObject *object = GetObject();
	int32 surfaceCount = object->GetSurfaceCount();
	for (machine a = 0; a < surfaceCount; a++)
	{
		int32 index = object->GetSurfaceData(a)->materialIndex;
		if (index < oldCount)
		{
			remap[index] = 0;
		}
	}

	int32 newCount = 0;
	for (machine a = 0; a < oldCount; a++)
	{
		if (remap[a] == 0)
		{
			MaterialObject *material = GetMaterialObject(a);
			for (machine b = 0; b < newCount; b++)
			{
				if (GetMaterialObject(b) == material)
				{
					remap[a] = b;
					goto next;
				}
			}

			SetMaterialObject(newCount, material);
			remap[a] = newCount++;
		}

		next:;
	}

	SetMaterialCount(newCount);
	GetFirstRenderSegment()->InvalidateShaderData();

	for (machine a = 0; a < surfaceCount; a++)
	{
		SurfaceData *surfaceData = object->GetSurfaceData(a);
		int32 index = surfaceData->materialIndex;
		index = (index < oldCount) ? remap[index] : 0;
		surfaceData->materialIndex = (unsigned_int16) ((index >= 0) ? index : 0);
	}

	delete[] remap;

	int32 levelCount = object->GetGeometryLevelCount();
	for (machine a = 0; a < levelCount; a++)
	{
		Mesh	tempMesh;

		Mesh *geometryMesh = object->GetGeometryLevel(a);
		tempMesh.CopyMesh(geometryMesh);
		geometryMesh->BuildSegmentArray(&tempMesh, surfaceCount, object->GetSurfaceData());
	}

	SetDetailLevel(0);
}

const MaterialObject *Geometry::GetTriangleMaterial(int32 triangleIndex) const
{
	const RenderSegment *segment = GetFirstRenderSegment();
	do
	{
		int32 start = segment->GetPrimitiveStart();
		int32 count = segment->GetPrimitiveCount();
		if ((unsigned_int32) (triangleIndex - start) < (unsigned_int32) count)
		{
			const MaterialObject *const *object = segment->GetMaterialObjectPointer();
			if (object)
			{
				return (*object);
			}

			break;
		}

		segment = segment->GetNextRenderSegment();
	} while (segment);

	return (nullptr);
}

void Geometry::SetDetailLevel(int32 level)
{
	geometryDetailLevel = level;

	const GeometryObject *object = GetObject();
	if (object)
	{
		unsigned_int32 dynamicFlags = GetVertexBufferArrayFlags();
		const Mesh *mesh = object->GetGeometryLevel(level);

		int32 vertexCount = mesh->GetVertexCount();
		SetVertexCount(vertexCount);

		const unsigned_int8 *arrayIndex = mesh->GetAttributeArrayIndex();
		int32 arrayCount = mesh->GetAttributeArrayCount();
		for (machine a = 0; a < arrayCount; a++)
		{
			int32 index = arrayIndex[a];
			if ((dynamicFlags & (1 << index)) == 0)
			{
				const ArrayBundle *bundle = mesh->GetArrayBundle(index);
				SetVertexAttributeArray(index, mesh->GetVertexAttributeOffset(a), bundle->descriptor.componentCount);
			}
		}

		positionArrayBundle = mesh->GetArrayBundle(kArrayPosition);

		const unsigned_int16 *primitive = mesh->GetArray<unsigned_int16>(kArrayPrimitive);
		if (primitive)
		{
			SetPrimitiveIndexOffset(mesh->GetPrimitiveIndexOffset());
			primitiveArrayBundle = mesh->GetArrayBundle(kArrayPrimitive);
		}

		RenderSegment *segment = GetFirstRenderSegment();
		const SegmentData *data = mesh->GetArray<SegmentData>(kArraySegment);
		if (!data)
		{
			segment->SetNextRenderSegment(nullptr);
			segment->SetPrimitiveRange(0, mesh->GetPrimitiveCount());
		}
		else
		{
			segment->SetPrimitiveRange(data->primitiveStart, data->primitiveCount);
			RenderSegment *prevSegment = segment;

			RenderSegment *segmentTable = GetRenderSegmentTable();

			int32 segmentCount = mesh->GetArrayDescriptor(kArraySegment)->elementCount - 1;
			for (machine a = 0; a < segmentCount; a++)
			{
				data++;

				segment = &segmentTable[data->materialIndex - 1];
				segment->SetPrimitiveRange(data->primitiveStart, data->primitiveCount);

				prevSegment->SetNextRenderSegment(segment);
				prevSegment = segment;
			}

			prevSegment->SetNextRenderSegment(nullptr);
		}

		Controller *controller = GetController();
		if (controller)
		{
			controller->SetDetailLevel(level);
		}

		InvalidateVertexData();
	}
}


GenericGeometry::GenericGeometry() : Geometry(kGeometryGeneric)
{
}

GenericGeometry::GenericGeometry(const Geometry *geometry) : Geometry(kGeometryGeneric)
{
	int32 materialCount = geometry->GetMaterialCount();
	SetMaterialCount(materialCount);

	for (machine a = 0; a < materialCount; a++)
	{
		SetMaterialObject(a, geometry->GetMaterialObject(a));
	}

	SetNewObject(new GenericGeometryObject(geometry));
}

GenericGeometry::GenericGeometry(int32 levelCount, const List<GeometrySurface> *const *surfaceList, MaterialObject *const *materialArray, const SkinData *const *skinDataTable, const MorphData *const *morphDataTable) : Geometry(kGeometryGeneric)
{
	Array<int32, 8>				materialIndexArray;
	Array<MaterialObject *, 8>	materialObjectArray;

	int32 surfaceCount = 0;
	int32 materialCount = 0;

	const GeometrySurface *surface = surfaceList[0]->First();
	while (surface)
	{
		MaterialObject *object = materialArray[surfaceCount];

		int32 index = materialObjectArray.FindElement(object);
		if (index == -1)
		{
			materialObjectArray.AddElement(object);
			index = materialCount++;
		}

		materialIndexArray.AddElement(index);

		surfaceCount++;
		surface = surface->Next();
	}

	SetMaterialCount(materialCount);

	for (machine a = 0; a < materialCount; a++)
	{
		SetMaterialObject(a, materialObjectArray[a]);
	}

	SetNewObject(new GenericGeometryObject(levelCount, surfaceList, surfaceCount, materialIndexArray, skinDataTable, morphDataTable));
}

GenericGeometry::GenericGeometry(int32 geometryCount, const Geometry *const *geometryArray, const Transformable *transformable) : Geometry(kGeometryGeneric)
{
	Array<MaterialObject *, 8>		materialArray;

	for (machine a = 0; a < geometryCount; a++)
	{
		const Geometry *geometry = geometryArray[a];
		int32 count = geometry->GetMaterialCount();
		for (machine b = 0; b < count; b++)
		{
			MaterialObject *object = geometry->GetMaterialObject(b);
			if (materialArray.FindElement(object) == -1)
			{
				materialArray.AddElement(object);
			}
		}
	}

	int32 materialCount = materialArray.GetElementCount();
	SetMaterialCount(materialCount);

	for (machine a = 0; a < materialCount; a++)
	{
		SetMaterialObject(a, materialArray[a]);
	}

	SetNewObject(new GenericGeometryObject(geometryCount, geometryArray, materialArray, transformable));
}

GenericGeometry::GenericGeometry(BooleanOperation operation, const Geometry *geometry1, const Geometry *geometry2) : Geometry(kGeometryGeneric)
{
	Array<MaterialObject *, 8>		materialArray;

	int32 count = geometry1->GetMaterialCount();
	for (machine a = 0; a < count; a++)
	{
		MaterialObject *object = geometry1->GetMaterialObject(a);
		if (materialArray.FindElement(object) == -1)
		{
			materialArray.AddElement(object);
		}
	}

	count = geometry2->GetMaterialCount();
	for (machine a = 0; a < count; a++)
	{
		MaterialObject *object = geometry2->GetMaterialObject(a);
		if (materialArray.FindElement(object) == -1)
		{
			materialArray.AddElement(object);
		}
	}

	int32 materialCount = materialArray.GetElementCount();
	SetMaterialCount(materialCount);

	for (machine a = 0; a < materialCount; a++)
	{
		SetMaterialObject(a, materialArray[a]);
	}

	SetNewObject(new GenericGeometryObject(operation, geometry1, geometry2, materialArray));
	OptimizeMaterials();
}

GenericGeometry::GenericGeometry(const GenericGeometry& genericGeometry) : Geometry(genericGeometry)
{
}

GenericGeometry::~GenericGeometry()
{
}

Node *GenericGeometry::Replicate(void) const
{
	return (new GenericGeometry(*this));
}

void GenericGeometry::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Geometry::Pack(data, packFlags);

	data << TerminatorChunk;
}

void GenericGeometry::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Geometry::Unpack(data, unpackFlags);
	UnpackChunkList<GenericGeometry>(data, unpackFlags);
}

bool GenericGeometry::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	return (false);
}

void GenericGeometry::HandlePostprocessUpdate(void)
{
	Geometry::HandlePostprocessUpdate();
	(*postprocessProc)(this);
}

bool GenericGeometry::CalculateBoundingBox(Box3D *box) const
{
	*box = GetObject()->GetBoundingBox();
	return (true);
}

bool GenericGeometry::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	*sphere = *GetObject()->GetBoundingSphere();
	return (true);
}

void GenericGeometry::CalculateOrientedBoundingBox(GenericGeometry *genericGeometry)
{
	const Box3D& box = genericGeometry->GetObject()->GetBoundingBox();
	const Transform4D& transform = genericGeometry->GetWorldTransform();
	genericGeometry->worldCenter = transform * box.GetCenter();

	const Vector3D& size = box.GetSize();
	genericGeometry->worldAxis[0] = transform[0] * (size.x * 0.5F);
	genericGeometry->worldAxis[1] = transform[1] * (size.y * 0.5F);
	genericGeometry->worldAxis[2] = transform[2] * (size.z * 0.5F);
}

bool GenericGeometry::BoxVisible(const Node *node, const VisibilityRegion *region)
{
	const GenericGeometry *geometry = static_cast<const GenericGeometry *>(node);
	return (region->BoxVisible(geometry->worldCenter, geometry->worldAxis));
}

bool GenericGeometry::BoxVisible(const Node *node, const VisibilityRegion *region, const List<OcclusionRegion> *occlusionList)
{
	const GenericGeometry *geometry = static_cast<const GenericGeometry *>(node);
	const Point3D& center = geometry->worldCenter;
	const Vector3D *axis = geometry->worldAxis;

	if (region->BoxVisible(center, axis))
	{
		const OcclusionRegion *occluder = occlusionList->First();
		while (occluder)
		{
			if (occluder->BoxOccluded(center, axis))
			{
				return (false);
			}

			occluder = occluder->Next();
		}

		return (true);
	}

	return (false);
}

void GenericGeometry::Preprocess(void)
{
	SetActiveUpdateFlags(GetActiveUpdateFlags() | kUpdatePostprocess);
	postprocessProc = &CalculateOrientedBoundingBox;

	SetVisibilityProc(&BoxVisible);
	SetOcclusionProc(&BoxVisible);

	SetMotionBlurBox(&GetObject()->GetBoundingBox());

	// Call Geometry::Preprocess() last to allow controllers to override the post-transform proc,
	// the visibility and occlusion procs, or the motion blur box.

	Geometry::Preprocess();
}

// ZYUQURM
