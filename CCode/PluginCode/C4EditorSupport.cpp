//=============================================================
//
// C4 Engine version 4.5
// Copyright 1999-2015, by Terathon Software LLC
//
// This file is part of the C4 Engine and is provided under the
// terms of the license agreement entered by the registed user.
//
// Unauthorized redistribution of source code is strictly
// prohibited. Violators will be prosecuted.
//
//=============================================================


#include "C4EditorSupport.h"
#include "C4TerrainTools.h"
#include "C4WaterTools.h"
#include "C4Landscaping.h"
#include "C4World.h"


using namespace C4;


const NodeType EditorObject::nodeTypeTable[kNodeTypeCount] =
{
	kNodeGeometry, kNodeLight, kNodeSource, kNodeZone, kNodePortal, kNodeTrigger, kNodeMarker, kNodeEffect, kNodeEmitter, kNodeSpace, kNodeShape, kNodeJoint,
	kNodeField, kNodeBlocker, kNodeCamera, kNodeSkybox, kNodeInstance, kNodeModel, kNodeBone, kNodeImpostor, kNodeTerrainBlock, kNodeWaterBlock, kNodePhysics, kNodeGeneric
};

const GeometryType EditorObject::geometryTypeTable[kGeometryTypeCount] =
{
	kGeometryPrimitive, kGeometryGeneric, kGeometryTerrain, kGeometryWater, kGeometryHorizonWater
};

const PrimitiveType EditorObject::primitiveTypeTable[kPrimitiveTypeCount] =
{
	kPrimitivePlate, kPrimitiveDisk, kPrimitiveHole, kPrimitiveAnnulus, kPrimitiveBox, kPrimitivePyramid, kPrimitiveCylinder, kPrimitiveCone, kPrimitiveSphere,
	kPrimitiveDome, kPrimitiveTorus, kPrimitiveTruncatedCone, kPrimitiveTube, kPrimitiveExtrusion, kPrimitiveRevolution, kPrimitiveRope, kPrimitiveCloth
};

const LightType EditorObject::lightTypeTable[kLightTypeCount] =
{
	kLightInfinite, kLightPoint, kLightCube, kLightSpot
};

const SourceType EditorObject::sourceTypeTable[kSourceTypeCount] =
{
	kSourceAmbient, kSourceOmni, kSourceDirected
};

const ZoneType EditorObject::zoneTypeTable[kZoneTypeCount] =
{
	kZoneBox, kZoneCylinder, kZonePolygon
};

const PortalType EditorObject::portalTypeTable[kPortalTypeCount] =
{
	kPortalDirect, kPortalRemote, kPortalOcclusion
};

const TriggerType EditorObject::triggerTypeTable[kTriggerTypeCount] =
{
	kTriggerBox, kTriggerCylinder, kTriggerSphere
};

const MarkerType EditorObject::markerTypeTable[kMarkerTypeCount] =
{
	kMarkerLocator, kMarkerConnection, kMarkerCube, kMarkerShader, kMarkerPath
};

const EffectType EditorObject::effectTypeTable[kEffectTypeCount] =
{
	kEffectQuad, kEffectFlare, kEffectBeam, kEffectTube, kEffectFire, kEffectShaft, kEffectPanel, kEffectParticleSystem
};

const ShaftType EditorObject::shaftTypeTable[kShaftTypeCount] =
{
	kShaftBox, kShaftCylinder, kShaftTruncatedPyramid, kShaftTruncatedCone
};

const EmitterType EditorObject::emitterTypeTable[kEmitterTypeCount] =
{
	kEmitterBox, kEmitterCylinder, kEmitterSphere
};

const SpaceType EditorObject::spaceTypeTable[kSpaceTypeCount] =
{
	kSpaceFog, kSpaceShadow, kSpaceRadiosity, kSpaceAcoustics, kSpaceOcclusion, kSpacePaint, kSpacePhysics
};

const ShapeType EditorObject::shapeTypeTable[kShapeTypeCount] =
{
	kShapeBox, kShapePyramid, kShapeCylinder, kShapeCone, kShapeSphere, kShapeDome, kShapeCapsule, kShapeTruncatedPyramid, kShapeTruncatedCone, kShapeTruncatedDome
};

const JointType EditorObject::jointTypeTable[kJointTypeCount] =
{
	kJointSpherical, kJointUniversal, kJointDiscal, kJointRevolute, kJointCylindrical, kJointPrismatic
};

const FieldType EditorObject::fieldTypeTable[kFieldTypeCount] =
{
	kFieldBox, kFieldCylinder, kFieldSphere
};

const BlockerType EditorObject::blockerTypeTable[kBlockerTypeCount] =
{
	kBlockerPlate, kBlockerBox, kBlockerCylinder, kBlockerSphere, kBlockerCapsule
};


SelectionMask::SelectionMask(Type type)
{
	nodeType = type; 
	maskValue = 1;
}
 
SelectionMask::~SelectionMask()
{ 
}

void SelectionMask::SetMaskValue(int32 value) 
{
	if (maskValue != value) 
	{ 
		maskValue = value;

		SelectionMask *submask = GetFirstSubnode();
		while (submask) 
		{
			submask->SetMaskValue(value);
			submask = GetNextNode(submask);
		}

		SelectionMask *supermask = GetSuperNode();
		while (supermask)
		{
			SelectionMask *mask = supermask->GetFirstSubnode();
			do
			{
				if (mask != this)
				{
					if (mask->maskValue != value)
					{
						supermask->maskValue = -1;
						goto next;
					}
				}

				mask = mask->Next();
			} while (mask);

			supermask->maskValue = value;

			next:
			supermask = supermask->GetSuperNode();
		}
	}
}


EditorObject::EditorObject() : Object(kObjectEditor)
{
	editorFlags = kEditorShowGridlines | kEditorShowViewportInfo | kEditorSnapToGrid | kEditorCapGeometry;

	gridLineSpacing = 0.1F;
	majorLineInterval = 10;

	Variable *var = TheEngine->InitVariable("editorGridColor", "808080");
	gridColor.SetHexString(var->GetValue());

	snapAngle = K::tau_over_8;
	cameraSpeed = 0.0625F;
	meshOriginSettings = kMeshOriginCenter | (kMeshOriginCenter << 8) | (kMeshOriginCenter << 16);

	targetZone = nullptr;
	selectedMaterial = nullptr;

	currentViewportLayout = kEditorLayout4;
	previousViewportLayout = kEditorLayout4;
	fullViewportIndex = kEditorViewportTopLeft;

	SetViewportMode(kEditorViewportTopLeft, kViewportModeTop);
	SetViewportMode(kEditorViewportTopRight, kViewportModeFront);
	SetViewportMode(kEditorViewportBottomLeft, kViewportModeGraph);
	SetViewportMode(kEditorViewportBottomRight, kViewportModeFrustum);
	SetViewportMode(kEditorViewportLeft, kViewportModeTop);
	SetViewportMode(kEditorViewportRight, kViewportModeFront);
	SetViewportMode(kEditorViewportTop, kViewportModeTop);
	SetViewportMode(kEditorViewportBottom, kViewportModeRight);

	BuildSelectionMask();
	BuildPages();

	TheWorldEditor->InitializePlugins(this);
}

EditorObject::~EditorObject()
{
}

void EditorObject::BuildSelectionMask(void)
{
	for (NodeType nodeType : nodeTypeTable)
	{
		SelectionMask *nodeMask = new SelectionMask(nodeType);

		if (nodeType == kNodeGeometry)
		{
			for (GeometryType geometryType : geometryTypeTable)
			{
				SelectionMask *geometryMask = new SelectionMask(geometryType);

				if (geometryType == kGeometryPrimitive)
				{
					for (PrimitiveType primitiveType : primitiveTypeTable)
					{
						geometryMask->AppendSubnode(new SelectionMask(primitiveType));
					}
				}

				nodeMask->AppendSubnode(geometryMask);
			}
		}
		else if (nodeType == kNodeLight)
		{
			for (LightType lightType : lightTypeTable)
			{
				nodeMask->AppendSubnode(new SelectionMask(lightType));
			}
		}
		else if (nodeType == kNodeSource)
		{
			for (SourceType sourceType : sourceTypeTable)
			{
				nodeMask->AppendSubnode(new SelectionMask(sourceType));
			}
		}
		else if (nodeType == kNodeZone)
		{
			for (ZoneType zoneType : zoneTypeTable)
			{
				nodeMask->AppendSubnode(new SelectionMask(zoneType));
			}
		}
		else if (nodeType == kNodePortal)
		{
			for (PortalType portalType : portalTypeTable)
			{
				nodeMask->AppendSubnode(new SelectionMask(portalType));
			}
		}
		else if (nodeType == kNodeTrigger)
		{
			for (TriggerType triggerType : triggerTypeTable)
			{
				nodeMask->AppendSubnode(new SelectionMask(triggerType));
			}
		}
		else if (nodeType == kNodeMarker)
		{
			for (MarkerType markerType : markerTypeTable)
			{
				nodeMask->AppendSubnode(new SelectionMask(markerType));
			}
		}
		else if (nodeType == kNodeEffect)
		{
			for (EffectType effectType : effectTypeTable)
			{
				SelectionMask *effectMask = new SelectionMask(effectType);

				if (effectType == kEffectShaft)
				{
					for (ShaftType shaftType : shaftTypeTable)
					{
						effectMask->AppendSubnode(new SelectionMask(shaftType));
					}
				}

				nodeMask->AppendSubnode(effectMask);
			}
		}
		else if (nodeType == kNodeEmitter)
		{
			for (EmitterType emitterType : emitterTypeTable)
			{
				nodeMask->AppendSubnode(new SelectionMask(emitterType));
			}
		}
		else if (nodeType == kNodeSpace)
		{
			for (SpaceType spaceType : spaceTypeTable)
			{
				nodeMask->AppendSubnode(new SelectionMask(spaceType));
			}
		}
		else if (nodeType == kNodeShape)
		{
			for (ShapeType shapeType : shapeTypeTable)
			{
				nodeMask->AppendSubnode(new SelectionMask(shapeType));
			}
		}
		else if (nodeType == kNodeJoint)
		{
			for (JointType jointType : jointTypeTable)
			{
				nodeMask->AppendSubnode(new SelectionMask(jointType));
			}
		}
		else if (nodeType == kNodeField)
		{
			for (FieldType fieldType : fieldTypeTable)
			{
				nodeMask->AppendSubnode(new SelectionMask(fieldType));
			}
		}
		else if (nodeType == kNodeBlocker)
		{
			for (BlockerType blockerType : blockerTypeTable)
			{
				nodeMask->AppendSubnode(new SelectionMask(blockerType));
			}
		}

		selectionMask.AppendSubnode(nodeMask);
	}
}

void EditorObject::BuildPages(void)
{
	// Object pane.

	pageList.Append(new GeometriesPage);
	pageList.Append(new LightsPage);
	pageList.Append(new SourcesPage);
	pageList.Append(new ZonesPage);
	pageList.Append(new PortalsPage);
	pageList.Append(new TriggersPage);
	pageList.Append(new MarkersPage);
	pageList.Append(new PathsPage);
	pageList.Append(new EffectsPage);
	pageList.Append(new ParticlesPage);
	pageList.Append(new AtmospherePage);
	pageList.Append(new PhysicsPage);

	// Material pane.

	pageList.Append(new MaterialPage);
	pageList.Append(paintPage = new PaintPage);
	pageList.Append(new TextureMappingPage);

	// Earth pane.

	pageList.Append(new TerrainPage);
	pageList.Append(new WaterPage);
	//pageList.Append(new LandscapingPage);		// Not fully implemented.

	// Instance pane.

	pageList.Append(new WorldsPage);
	pageList.Append(new ModelsPage);
	pageList.Append(new ImpostorsPage);
	pageList.Append(new PlacementPage);

	// Editor pane.

	pageList.Append(new NodeManagementPage);
	pageList.Append(new GridPage);
	pageList.Append(viewportsPage = new ViewportsPage);
	pageList.Append(new TransformPage);
	pageList.Append(new InfoPage);
	pageList.Append(new FindPage);
}

void EditorObject::Prepack(List<Object> *linkList) const
{
	MaterialContainer *material = materialList.First();
	while (material)
	{
		MaterialObject *object = material->GetMaterialObject();
		if (object)
		{
			linkList->Append(object);
		}

		material = material->Next();
	}

	const EditorPage *page = pageList.First();
	while (page)
	{
		page->Prepack(linkList);
		page = page->ListElement<EditorPage>::Next();
	}
}

void EditorObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	data << ChunkHeader('FLAG', 4);
	data << editorFlags;

	const SelectionMask *mask = selectionMask.GetFirstSubnode();
	while (mask)
	{
		if (mask->GetMaskValue() != 0)
		{
			mask = selectionMask.GetNextNode(mask);
		}
		else
		{
			Type	type[kMaxNodeTypeSize];

			const SelectionMask *m = mask;
			int32 count = mask->GetNodeDepth();
			for (machine a = count - 1; a >= 0; a--)
			{
				type[a] = m->GetNodeType();
				m = m->GetSuperNode();
			}

			data << ChunkHeader('ZMSK', count * sizeof(Type));
			data.WriteArray(count, type);

			mask = selectionMask.GetNextLevelNode(mask);
		}
	}

	data << ChunkHeader('GRID', 4 + sizeof(ColorRGB));
	data << gridLineSpacing;
	data << gridColor;

	data << ChunkHeader('GMAJ', 4);
	data << majorLineInterval;

	data << ChunkHeader('SNAP', 4);
	data << snapAngle;

	data << ChunkHeader('CAMR', 4);
	data << cameraSpeed;

	if (targetZone)
	{
		data << ChunkHeader('TARG', 4);
		data << targetZone->GetNodeIndex();
	}

	data << ChunkHeader('LAYO', 12);
	data << currentViewportLayout;
	data << previousViewportLayout;
	data << fullViewportIndex;

	for (machine a = 0; a < kEditorViewportCount; a++)
	{
		data << ChunkHeader('VPRT', 8 + sizeof(Transform4D) + sizeof(Vector2D));
		data << int32(a);
		data << viewportMode[a];
		data << viewportTransform[a];
		data << viewportData[a];
	}

	data << ChunkHeader('ORIG', 4);
	data << meshOriginSettings;

	const MaterialContainer *material = materialList.First();
	while (material)
	{
		PackHandle handle = data.BeginChunk('MATR');
		material->Pack(data, packFlags);
		data.EndChunk(handle);

		material = material->Next();
	}

	int32 index = 0;
	material = selectedMaterial->Previous();
	while (material)
	{
		index++;
		material = material->Previous();
	}

	data << ChunkHeader('CMAT', 4);
	data << index;

	int32 pageCount = 0;
	const EditorPage *page = pageList.First();
	while (page)
	{
		PackHandle handle = data.BeginChunk('PAGE');
		data << page->GetPageType();
		page->Pack(data, packFlags);
		data.EndChunk(handle);

		pageCount++;
		page = page->ListElement<EditorPage>::Next();
	}

	data << ChunkHeader('PGOR', 4 + pageCount * 8);
	data << pageCount;

	page = pageList.First();
	while (page)
	{
		data << static_cast<const EditorPage *>(page)->GetPageType();

		Page *prev = page->ListElement<Page>::Previous();
		if (prev)
		{
			data << static_cast<const EditorPage *>(prev)->GetPageType();
		}
		else
		{
			data << int32(0);
		}

		page = page->ListElement<EditorPage>::Next();
	}

	data << TerminatorChunk;
}

void EditorObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	UnpackChunkList<EditorObject>(data, unpackFlags);
}

bool EditorObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> editorFlags;
			return (true);

		case 'ZMSK':
		{
			Type	type[kMaxNodeTypeSize];

			int32 count = chunkHeader->chunkSize / sizeof(Type);
			data.ReadArray(count, type);

			SelectionMask *mask = selectionMask.GetFirstSubnode();
			for (machine a = 0;;)
			{
				Type t = type[a];
				while (mask)
				{
					if (mask->GetNodeType() == t)
					{
						if (++a < count)
						{
							mask = mask->GetFirstSubnode();
							goto next;
						}

						mask->SetMaskValue(0);
						break;
					}

					mask = mask->Next();
				}

				break;
				next:;
			}

			return (true);
		}

		case 'GRID':

			data >> gridLineSpacing;
			data >> gridColor;
			return (true);

		case 'GMAJ':

			data >> majorLineInterval;
			return (true);

		case 'SNAP':

			data >> snapAngle;
			return (true);

		case 'CAMR':

			data >> cameraSpeed;
			return (true);

		case 'TARG':
		{
			int32	zoneIndex;

			data >> zoneIndex;
			data.AddNodeLink(zoneIndex, &TargetZoneLinkProc, this);
			return (true);
		}

		case 'LAYO':

			data >> currentViewportLayout;
			data >> previousViewportLayout;
			data >> fullViewportIndex;
			return (true);

		case 'VPRT':
		{
			int32	index;

			data >> index;
			data >> viewportMode[index];
			data >> viewportTransform[index];
			data >> viewportData[index];
			return (true);
		}

		case 'ORIG':

			data >> meshOriginSettings;
			return (true);

		case 'MATR':

			if (unpackFlags & kUnpackEditor)
			{
				MaterialContainer *material = new MaterialContainer;
				material->Unpack(data, unpackFlags);
				materialList.Append(material);
				return (true);
			}

			break;

		case 'CMAT':

			if (unpackFlags & kUnpackEditor)
			{
				int32	index;

				data >> index;
				selectedMaterial = materialList[index];
				return (true);
			}

			break;

		case 'PAGE':

			if (unpackFlags & kUnpackEditor)
			{
				PageType	type;

				data >> type;

				EditorPage *page = pageList.First();
				while (page)
				{
					if (page->GetPageType() == type)
					{
						page->Unpack(data, unpackFlags);
						return (true);
					}

					page = page->ListElement<EditorPage>::Next();
				}
			}

			break;

		case 'PGOR':

			if (unpackFlags & kUnpackEditor)
			{
				int32				count;
				List<EditorPage>	tempList;

				data >> count;
				for (machine a = 0; a < count; a++)
				{
					PageType	type, prevType;

					data >> type;
					data >> prevType;

					EditorPage *page = pageList.First();
					while (page)
					{
						if (page->GetPageType() == type)
						{
							page->prevPageType = prevType;
							tempList.Append(page);
							break;
						}

						page = page->ListElement<EditorPage>::Next();
					}
				}

				for (;;)
				{
					EditorPage *page = tempList.First();
					if (!page)
					{
						break;
					}

					bool progress = false;
					do
					{
						EditorPage *next = page->ListElement<EditorPage>::Next();

						PageType prevType = page->prevPageType;
						if (prevType != 0)
						{
							EditorPage *prevPage = pageList.First();
							while (prevPage)
							{
								if (prevPage->GetPageType() == prevType)
								{
									pageList.InsertAfter(page, prevPage);
									progress = true;
									break;
								}

								prevPage = prevPage->ListElement<EditorPage>::Next();
							}
						}
						else
						{
							pageList.Prepend(page);
							progress = true;
						}

						page = next;
					} while (page);

					if (!progress)
					{
						pageList.Append(tempList.First());
					}
				}

				return (true);
			}

			break;
	}

	return (false);
}

void EditorObject::TargetZoneLinkProc(Node *node, void *cookie)
{
	EditorObject *editorObject = static_cast<EditorObject *>(cookie);
	editorObject->targetZone = static_cast<Zone *>(node);
}

void EditorObject::Preprocess(Editor *editor)
{
	if (!selectedMaterial)
	{
		MaterialContainer *material = materialList.First();
		if (!material)
		{
			MaterialObject *object = new MaterialObject;
			material = new MaterialContainer(object, "default");
			materialList.Append(material);
			object->Release();
		}

		selectedMaterial = material;
	}

	EditorPage *page = pageList.First();
	while (page)
	{
		page->SetEditor(editor);
		page = page->ListElement<EditorPage>::Next();
	}
}

void EditorObject::SetCurrentViewportLayout(int32 layout)
{
	currentViewportLayout = layout;
	viewportsPage->SetViewportLayout(layout);
}

void EditorObject::SetViewportMode(int32 index, int32 mode)
{
	viewportMode[index] = mode;

	switch (mode)
	{
		case kViewportModeTop:

			viewportTransform[index].Set(1.0F, 0.0F, 0.0F, 0.0F, 0.0F, -1.0F, 0.0F, 0.0F, 0.0F, 0.0F, -1.0F, 0.0F);
			viewportData[index].Set(0.015625F, 0.015625F);
			break;

		case kViewportModeBottom:

			viewportTransform[index].Set(-1.0F, 0.0F, 0.0F, 0.0F, 0.0F, -1.0F, 0.0F, 0.0F, 0.0F, 0.0F, 1.0F, 0.0F);
			viewportData[index].Set(0.015625F, 0.015625F);
			break;

		case kViewportModeFront:

			viewportTransform[index].Set(0.0F, 0.0F, -1.0F, 0.0F, 1.0F, 0.0F, 0.0F, 0.0F, 0.0F, -1.0F, 0.0F, 0.0F);
			viewportData[index].Set(0.015625F, 0.015625F);
			break;

		case kViewportModeBack:

			viewportTransform[index].Set(0.0F, 0.0F, 1.0F, 0.0F, -1.0F, 0.0F, 0.0F, 0.0F, 0.0F, -1.0F, 0.0F, 0.0F);
			viewportData[index].Set(0.015625F, 0.015625F);
			break;

		case kViewportModeRight:

			viewportTransform[index].Set(1.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F, -1.0F, 0.0F, 0.0F);
			viewportData[index].Set(0.015625F, 0.015625F);
			break;

		case kViewportModeLeft:

			viewportTransform[index].Set(-1.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, -1.0F, 0.0F, 0.0F, -1.0F, 0.0F, 0.0F);
			viewportData[index].Set(0.015625F, 0.015625F);
			break;

		case kViewportModeFrustum:

			viewportTransform[index].Set(0.0F, 0.0F, -1.0F, 5.0F, 1.0F, 0.0F, 0.0F, 0.0F, 0.0F, -1.0F, 0.0F, 1.0F);
			viewportData[index].Set(K::tau_over_2, 0.0F);
			break;

		case kViewportModeGraph:

			viewportTransform[index].Set(1.0F, 0.0F, 0.0F, kGraphBoxWidth, 0.0F, 1.0F, 0.0F, 8.0F, 0.0F, 0.0F, 1.0F, 0.0F);
			viewportData[index].Set(1.0F, 1.0F);
			break;
	}
}

void EditorObject::ConfirmMaterialContainer(MaterialObject *materialObject)
{
	MaterialContainer *container = materialList.First();
	while (container)
	{
		if (container->GetMaterialObject() == materialObject)
		{
			container->SetUsageCount(container->GetUsageCount() + 1);
			return;
		}

		container = container->Next();
	}

	container = new MaterialContainer(materialObject);
	container->SetUsageCount(1);
	materialList.Append(container);
}

List<MaterialContainer> *EditorObject::UpdateMaterialList(const Node *root)
{
	MaterialContainer *container = materialList.First();
	while (container)
	{
		container->SetUsageCount(0);
		container = container->Next();
	}

	const Node *node = root;
	do
	{
		const EditorManipulator *manipulator = Editor::GetManipulator(node);
		int32 count = manipulator->GetMaterialCount();
		for (machine a = 0; a < count; a++)
		{
			MaterialObject *materialObject = manipulator->GetMaterial(a);
			if (materialObject)
			{
				ConfirmMaterialContainer(materialObject);
			}
		}

		NodeType type = node->GetNodeType();
		if ((type != kNodeInstance) && (type != kNodeModel))
		{
			node = root->GetNextNode(node);
		}
		else
		{
			node = root->GetNextLevelNode(node);
		}
	} while (node);

	return (&materialList);
}

MaterialContainer *EditorObject::FindMaterialContainer(const MaterialObject *materialObject) const
{
	MaterialContainer *container = materialList.First();
	while (container)
	{
		const MaterialObject *object = container->GetMaterialObject();
		if (object == materialObject)
		{
			return (container);
		}

		container = container->Next();
	}

	return (nullptr);
}

MaterialObject *EditorObject::FindMatchingMaterial(const MaterialObject *materialObject) const
{
	MaterialContainer *container = materialList.First();
	while (container)
	{
		MaterialObject *object = container->GetMaterialObject();
		if (*object == *materialObject)
		{
			return (object);
		}

		container = container->Next();
	}

	return (nullptr);
}

MaterialObject *EditorObject::FindNamedMaterial(const char *name) const
{
	MaterialContainer *container = materialList.First();
	while (container)
	{
		if (container->GetMaterialName() == name)
		{
			return (container->GetMaterialObject());
		}

		container = container->Next();
	}

	return (nullptr);
}

void EditorObject::AddMaterial(MaterialObject *materialObject, const char *name)
{
	MaterialContainer *container = new MaterialContainer(materialObject, name);
	materialList.Append(container);
}

void EditorObject::ReplaceMaterial(MaterialObject *oldMaterial, MaterialObject *newMaterial, Node *root)
{
	Node *node = root->GetFirstSubnode();
	while (node)
	{
		Editor::GetManipulator(node)->ReplaceMaterial(oldMaterial, newMaterial);
		node = root->GetNextNode(node);
	}
}

void EditorObject::CleanupMaterials(Node *root)
{
	if (selectedMaterial)
	{
		MaterialObject *selectedObject = selectedMaterial->GetMaterialObject();

		MaterialContainer *container = materialList.First();
		while (container)
		{
			MaterialContainer *next = container->Next();

			MaterialObject *object = container->GetMaterialObject();
			if ((object != selectedObject) && (*object == *selectedObject))
			{
				ReplaceMaterial(object, selectedObject, root);
				delete container;
			}

			container = next;
		}
	}

	MaterialContainer *originalContainer = materialList.First();
	while (originalContainer)
	{
		MaterialContainer *nextOriginal = originalContainer->Next();
		MaterialObject *originalObject = originalContainer->GetMaterialObject();

		MaterialContainer *duplicateContainer = nextOriginal;
		while (duplicateContainer)
		{
			MaterialContainer *nextDuplicate = duplicateContainer->Next();

			MaterialObject *duplicateObject = duplicateContainer->GetMaterialObject();
			if (*duplicateObject == *originalObject)
			{
				ReplaceMaterial(duplicateObject, originalObject, root);

				if (duplicateContainer == nextOriginal)
				{
					nextOriginal = nextDuplicate;
				}

				delete duplicateContainer;
			}

			duplicateContainer = nextDuplicate;
		}

		originalContainer = nextOriginal;
	}

	UpdateMaterialList(root);
}

void EditorObject::AddEditorPage(EditorPage *page)
{
	pageList.Append(page);
}


WorldSavePicker::WorldSavePicker() : FilePicker('WRLD', nullptr, TheResourceMgr->GetGenericCatalog(), WorldResource::GetDescriptor(), nullptr, kFilePickerSave, "WorldEditor/WorldSave")
{
}

WorldSavePicker::~WorldSavePicker()
{
}

void WorldSavePicker::Preprocess(void)
{
	FilePicker::Preprocess();

	stripBox = static_cast<CheckWidget *>(FindWidget("Strip"));
}

// ZYUQURM
