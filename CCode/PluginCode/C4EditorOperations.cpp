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


#include "C4EditorOperations.h"
#include "C4WorldEditor.h"
#include "C4EditorSupport.h"
#include "C4GeometryManipulators.h"
#include "C4InstanceManipulators.h"
#include "C4Primitives.h"
#include "C4Terrain.h"
#include "C4Water.h"
#include "C4World.h"


using namespace C4;


Operation::Operation(OperationType type)
{
	operationType = type;
	coupledFlag = false;
}

Operation::~Operation()
{
}


CreateOperation::CreateOperation() : Operation(kOperationCreate)
{
}

CreateOperation::CreateOperation(Node *node) : Operation(kOperationCreate)
{
	AddNode(node);
}

CreateOperation::CreateOperation(const List<NodeReference> *referenceList) : Operation(kOperationCreate)
{
	const NodeReference *reference = referenceList->First();
	while (reference)
	{
		AddNode(reference->GetNode());
		reference = reference->Next();
	}
}

CreateOperation::~CreateOperation()
{
}

void CreateOperation::Restore(Editor *editor)
{
	const NodeReference *created = createdList.First();
	while (created)
	{
		editor->DeleteSubtree(created->GetNode());
		created = created->Next();
	}
}


MoveOperation::MoveOperation(Node *node) : Operation(kOperationMove)
{
	if (node->GetNodeType() == kNodeMarker)
	{
		Marker *marker = static_cast<Marker *>(node);
		if (marker->GetMarkerType() == kMarkerPath)
		{
			movedList.Append(new PathReference(static_cast<PathMarker *>(marker)));
			return;
		}
	}

	movedList.Append(new NodeTransformReference(node));
}

MoveOperation::MoveOperation(const List<NodeReference> *referenceList) : Operation(kOperationMove)
{
	for (const NodeReference *reference = referenceList->First(); reference; reference = reference->Next())
	{
		Node *node = reference->GetNode();
		if (node->GetNodeType() == kNodeMarker)
		{
			Marker *marker = static_cast<Marker *>(node);
			if (marker->GetMarkerType() == kMarkerPath)
			{
				movedList.Append(new PathReference(static_cast<PathMarker *>(marker)));
				continue;
			}
		}

		movedList.Append(new NodeTransformReference(node));
	}
}

MoveOperation::~MoveOperation()
{
}

MoveOperation::PathReference::PathReference(PathMarker *marker) :
		NodeTransformReference(marker),
		path(*marker->GetPath())
{ 
}

void MoveOperation::Restore(Editor *editor) 
{
	const NodeReference *reference = movedList.First(); 
	while (reference)
	{
		Node *node = reference->GetNode(); 
		if (node->GetNodeType() == kNodeMarker)
		{ 
			Marker *marker = static_cast<Marker *>(node); 
			if (marker->GetMarkerType() == kMarkerPath)
			{
				PathMarker *pathMarker = static_cast<PathMarker *>(marker);
				*pathMarker->GetPath() = *static_cast<const PathReference *>(reference)->GetPath(); 
			}
		}

		node->SetNodeTransform(static_cast<const NodeTransformReference *>(reference)->GetTransform());
		Editor::GetManipulator(node)->InvalidateNode();

		reference = reference->Next();
	}

	editor->RegenerateTexcoords(&movedList);
}


SizeOperation::SizeOperation(Node *node) : Operation(kOperationSize)
{
	sizeNode = node;
	sizeCount = node->GetObject()->GetObjectSize(objectSize);
}

SizeOperation::~SizeOperation()
{
}

void SizeOperation::Restore(Editor *editor)
{
	Editor::GetManipulator(sizeNode)->HandleSizeUpdate(sizeCount, objectSize);
}


ResizeOperation::ResizeOperation(Node *node) : Operation(kOperationResize)
{
	AddNode(node);
}

ResizeOperation::ResizeOperation(const List<NodeReference> *referenceList) : Operation(kOperationResize)
{
	const NodeReference *reference = referenceList->First();
	while (reference)
	{
		AddNode(reference->GetNode());
		reference = reference->Next();
	}
}

ResizeOperation::~ResizeOperation()
{
}

ResizeOperation::ResizedReference::ResizedReference(Node *node)
{
	reference = node;
	transform = node->GetNodeTransform();

	const Object *object = node->GetObject();
	if (object)
	{
		object->GetObjectSize(objectSize);
	}
}

ResizeOperation::ResizedGeometryReference::ResizedGeometryReference(Geometry *geometry) : ResizedReference(geometry)
{
	const GeometryObject *object = geometry->GetObject();
	int32 levelCount = object->GetGeometryLevelCount();

	geometryMesh = new Mesh[levelCount];
	for (machine a = 0; a < levelCount; a++)
	{
		geometryMesh[a].CopyMesh(object->GetGeometryLevel(a));
	}
}

ResizeOperation::ResizedGeometryReference::~ResizedGeometryReference()
{
	delete[] geometryMesh;
}

ResizeOperation::ResizedMeshReference::ResizedMeshReference(GenericGeometry *mesh) : ResizedGeometryReference(mesh)
{
	const GenericGeometryObject *object = mesh->GetObject();
	boundingSphere = *object->GetBoundingSphere();
	boundingBox = object->GetBoundingBox();
}

ResizeOperation::ResizedPortalReference::ResizedPortalReference(Portal *portal) : ResizedReference(portal)
{
	const PortalObject *object = portal->GetObject();
	const Point3D *vertex = object->GetVertexArray();

	int32 count = object->GetVertexCount();
	for (machine a = 0; a < count; a++)
	{
		portalVertex[a] = vertex[a];
	}
}

ResizeOperation::ResizedPolygonZoneReference::ResizedPolygonZoneReference(PolygonZone *polygon) : ResizedReference(polygon)
{
	const PolygonZoneObject *object = polygon->GetObject();
	const Point3D *vertex = object->GetVertexArray();

	int32 count = object->GetVertexCount();
	for (machine a = 0; a < count; a++)
	{
		zoneVertex[a] = vertex[a];
	}
}

ResizeOperation::AffectedReference::AffectedReference(Node *node)
{
	reference = node;
	position = node->GetNodePosition();
}

void ResizeOperation::AddNode(Node *node)
{
	NodeType nodeType = node->GetNodeType();
	if (nodeType != kNodeGeneric)
	{
		if ((nodeType != kNodeTerrainBlock) && (nodeType != kNodeWaterBlock))
		{
			if (nodeType == kNodeGeometry)
			{
				Geometry *geometry = static_cast<Geometry *>(node);
				if (geometry->GetGeometryType() == kGeometryGeneric)
				{
					resizedList.Append(new ResizedMeshReference(static_cast<GenericGeometry *>(geometry)));
				}
				else
				{
					resizedList.Append(new ResizedGeometryReference(geometry));
				}
			}
			else if (nodeType == kNodePortal)
			{
				resizedList.Append(new ResizedPortalReference(static_cast<Portal *>(node)));
			}
			else if ((nodeType == kNodeZone) && (static_cast<Zone *>(node)->GetZoneType() == kZonePolygon))
			{
				resizedList.Append(new ResizedPolygonZoneReference(static_cast<PolygonZone *>(node)));
			}
			else
			{
				resizedList.Append(new ResizedReference(node));
			}

			node = node->GetFirstSubnode();
			while (node)
			{
				affectedList.Append(new AffectedReference(node));
				node = node->Next();
			}
		}
		else if (nodeType == kNodeTerrainBlock)
		{
			resizedList.Append(new ResizedReference(node));

			Node *subnode = node->GetFirstSubnode();
			while (subnode)
			{
				if (subnode->GetNodeType() == kNodeGeometry)
				{
					Geometry *geometry = static_cast<Geometry *>(subnode);
					if (geometry->GetGeometryType() == kGeometryTerrain)
					{
						resizedList.Append(new ResizedGeometryReference(geometry));
					}
				}

				subnode = node->GetNextNode(subnode);
			}
		}
		else
		{
			resizedList.Append(new ResizedReference(node));

			Node *subnode = node->GetFirstSubnode();
			while (subnode)
			{
				if (subnode->GetNodeType() == kNodeGeometry)
				{
					Geometry *geometry = static_cast<Geometry *>(subnode);
					GeometryType type = geometry->GetGeometryType();

					if ((type == kGeometryWater) || (type == kGeometryHorizonWater))
					{
						resizedList.Append(new ResizedGeometryReference(geometry));
					}
				}

				subnode = subnode->Next();
			}
		}
	}
}

void ResizeOperation::Restore(Editor *editor)
{
	const AffectedReference *affected = affectedList.First();
	while (affected)
	{
		Node *node = affected->GetNode();
		node->SetNodePosition(affected->GetPosition());
		node->Invalidate();

		affected = affected->Next();
	}

	const ResizedReference *resized = resizedList.First();
	while (resized)
	{
		Node *node = resized->GetNode();
		node->SetNodeTransform(resized->GetTransform());

		NodeType nodeType = node->GetNodeType();
		if (nodeType == kNodeGeometry)
		{
			const ResizedGeometryReference *resizedGeometry = static_cast<const ResizedGeometryReference *>(resized);
			Geometry *geometry = static_cast<Geometry *>(node);
			GeometryObject *object = geometry->GetObject();

			int32 levelCount = object->GetGeometryLevelCount();
			for (machine a = 0; a < levelCount; a++)
			{
				object->GetGeometryLevel(a)->CopyMesh(resizedGeometry->GetGeometryLevel(a));
			}

			if (geometry->GetGeometryType() == kGeometryGeneric)
			{
				const ResizedMeshReference *resizedMesh = static_cast<const ResizedMeshReference *>(resizedGeometry);
				GenericGeometryObject *meshObject = static_cast<GenericGeometryObject *>(object);

				meshObject->SetBoundingSphere(resizedMesh->GetBoundingSphere());
				meshObject->SetBoundingBox(resizedMesh->GetBoundingBox());
			}
			else
			{
				object->SetObjectSize(resized->GetObjectSize());
			}

			object->BuildCollisionData();
			editor->InvalidateGeometry(geometry);
		}
		else if (nodeType == kNodePortal)
		{
			const Point3D *portalVertex = static_cast<const ResizedPortalReference *>(resized)->GetVertexArray();

			PortalObject *portalObject = static_cast<Portal *>(node)->GetObject();
			Point3D *vertex = portalObject->GetVertexArray();

			int32 count = portalObject->GetVertexCount();
			for (machine a = 0; a < count; a++)
			{
				vertex[a] = portalVertex[a];
			}
		}
		else if (nodeType == kNodeZone)
		{
			Object *object = node->GetObject();
			object->SetObjectSize(resized->GetObjectSize());

			if (static_cast<Zone *>(node)->GetZoneType() == kZonePolygon)
			{
				const Point3D *zoneVertex = static_cast<const ResizedPolygonZoneReference *>(resized)->GetVertexArray();

				PolygonZoneObject *polygonObject = static_cast<PolygonZoneObject *>(object);
				Point3D *vertex = polygonObject->GetVertexArray();

				int32 count = polygonObject->GetVertexCount();
				for (machine a = 0; a < count; a++)
				{
					vertex[a] = zoneVertex[a];
				}
			}

			editor->InvalidateNode(editor->GetRootNode());
		}
		else
		{
			Object *object = node->GetObject();
			if (object)
			{
				object->SetObjectSize(resized->GetObjectSize());
			}

			if (nodeType == kNodeEffect)
			{
				static_cast<Effect *>(node)->UpdateEffectGeometry();
			}
		}

		node->Invalidate();

		resized = resized->Next();
	}
}


PasteOperation::PasteOperation(const List<NodeReference> *referenceList) : Operation(kOperationPaste)
{
	const NodeReference *reference = referenceList->First();
	while (reference)
	{
		pastedList.Append(new NodeReference(reference->GetNode()));
		reference = reference->Next();
	}
}

PasteOperation::~PasteOperation()
{
}

void PasteOperation::Restore(Editor *editor)
{
	const NodeReference *pasted = pastedList.First();
	while (pasted)
	{
		editor->DeleteNode(pasted->GetNode());
		pasted = pasted->Next();
	}
}


DeleteOperation::DeleteOperation(const List<NodeReference> *referenceList) : Operation(kOperationDelete)
{
	const NodeReference *reference = referenceList->First();
	while (reference)
	{
		Node *node = reference->GetNode();
		if (node->GetSuperNode())
		{
			affectedList.Append(new AffectedReference(node, true));

			node = node->GetFirstSubnode();
			while (node)
			{
				if ((!(node->GetNodeFlags() & kNodeNonpersistent)) && (!node->GetManipulator()->Selected()))
				{
					affectedList.Append(new AffectedReference(node, false));
				}

				node = node->Next();
			}
		}

		reference = reference->Next();
	}
}

DeleteOperation::~DeleteOperation()
{
	AffectedReference *affected = affectedList.First();
	while (affected)
	{
		if (affected->GetDeletedFlag())
		{
			delete affected->GetNode();
		}

		affected = affected->Next();
	}
}

DeleteOperation::AffectedReference::AffectedReference(Node *node, bool deleted)
{
	reference = node;
	superNode = node->GetSuperNode();
	nodeTransform = node->GetNodeTransform();

	deletedFlag = deleted;
	if (deleted)
	{
		for (;;)
		{
			Link<Node> *link = node->GetFirstLink();
			if (!link)
			{
				break;
			}

			linkArray.AddElement(link);
			*link = nullptr;
		}

		const Hub *hub = node->GetHub();
		if (hub)
		{
			Connector *connector = hub->GetFirstOutgoingEdge();
			while (connector)
			{
				outgoingConnectorArray.AddElement(OutgoingConnectorData(connector));
				connector = connector->GetNextOutgoingEdge();
			}

			connector = hub->GetFirstIncomingEdge();
			while (connector)
			{
				incomingConnectorArray.AddElement(IncomingConnectorData(connector));
				connector = connector->GetNextIncomingEdge();
			}
		}
	}
}

DeleteOperation::AffectedReference::~AffectedReference()
{
}

void DeleteOperation::Restore(Editor *editor)
{
	const AffectedReference *affected = affectedList.First();
	while (affected)
	{
		Node *node = affected->GetNode();
		if (!affected->GetDeletedFlag())
		{
			node->Neutralize();
		}

		node->SetNodeTransform(affected->GetNodeTransform());

		Node *super = affected->GetSuperNode();
		super->AppendSubnode(node);

		const ImmutableArray<Link<Node> *>& linkArray = affected->GetLinkArray();
		for (Link<Node> *link : linkArray)
		{
			*link = node;
		}

		const ImmutableArray<OutgoingConnectorData>& outgoingConnectorArray = affected->GetOutgoingConnectorArray();
		for (const OutgoingConnectorData& data : outgoingConnectorArray)
		{
			data.outgoingConnector->SetConnectorTarget(data.targetNode);
		}

		Editor::GetManipulator(node)->HandleConnectorUpdate();

		const ImmutableArray<IncomingConnectorData>& incomingConnectorArray = affected->GetIncomingConnectorArray();
		for (const IncomingConnectorData& data : incomingConnectorArray)
		{
			const ConnectorKey& key = data.connectorKey;
			Node *connectorNode = data.connectorNode;
			connectorNode->SetConnectedNode(key, node);
			Editor::GetManipulator(connectorNode)->HandleConnectorUpdate();
		}

		Editor::GetManipulator(super)->InvalidateGraph();

		affected = affected->Next();
	}

	affected = affectedList.First();
	while (affected)
	{
		if (affected->GetDeletedFlag())
		{
			Node *node = affected->GetNode();
			node->Preprocess();

			Editor::GetManipulator(node)->HandleUndelete();
		}

		affected = affected->Next();
	}

	affectedList.Purge();

	const NodeReference *gizmoTarget = editor->GetGizmoTarget();
	if (gizmoTarget)
	{
		editor->PostEvent(GizmoEditorEvent(kEditorEventGizmoTargetModified, gizmoTarget->GetNode()));
	}
}


GroupOperation::GroupOperation() : Operation(kOperationGroup)
{
}

GroupOperation::~GroupOperation()
{
}

void GroupOperation::Restore(Editor *editor)
{
	const NodeReference *reference = groupList.First();
	while (reference)
	{
		Node *group = reference->GetNode();
		Node *superNode = group->GetSuperNode();

		for (;;)
		{
			Node *node = group->GetFirstSubnode();
			if (!node)
			{
				break;
			}

			superNode->AppendSubnode(node);
			static_cast<EditorManipulator *>(node->GetManipulator())->InvalidateGraph();
		}

		editor->DeleteNode(group);
		reference = reference->Next();
	}

	editor->GetRootNode()->Update();
}


ConnectOperation::ConnectOperation(const List<EditorManipulator> *manipulatorList) : Operation(kOperationConnect)
{
	EditorManipulator *manipulator = manipulatorList->First();
	while (manipulator)
	{
		connectedList.Append(new ConnectedReference(manipulator->GetTargetNode()));
		manipulator = manipulator->Next();
	}
}

ConnectOperation::ConnectOperation(const List<NodeReference> *referenceList) : Operation(kOperationConnect)
{
	const NodeReference *reference = referenceList->First();
	while (reference)
	{
		connectedList.Append(new ConnectedReference(reference->GetNode()));
		reference = reference->Next();
	}
}

ConnectOperation::~ConnectOperation()
{
}

ConnectOperation::ConnectedReference::ConnectedReference(Node *node)
{
	reference = node;

	const EditorManipulator *manipulator = Editor::GetManipulator(node);
	int32 count = manipulator->GetConnectorCount();

	for (machine a = 0; a < count; a++)
	{
		connectorArray.AddElement(ConnectorData(a, manipulator->GetConnectorTarget(a)));
	}
}

ConnectOperation::ConnectedReference::~ConnectedReference()
{
}

void ConnectOperation::Restore(Editor *editor)
{
	const ConnectedReference *connected = connectedList.First();
	while (connected)
	{
		Node *node = connected->GetNode();
		EditorManipulator *manipulator = Editor::GetManipulator(node);

		const ImmutableArray<ConnectorData>& connectorArray = connected->GetConnectorArray();
		for (const ConnectorData& data : connectorArray)
		{
			manipulator->SetConnectorTarget(data.connectorIndex, data.targetNode);
		}

		node->ProcessInternalConnectors();
		node->Invalidate();

		connected = connected->Next();
	}

	const NodeReference *reference = genericControllerList.First();
	while (reference)
	{
		Node *node = reference->GetNode();
		Controller *controller = node->GetController();
		node->SetController(nullptr);
		delete controller;

		reference = reference->Next();
	}
}


ReparentOperation::ReparentOperation() : Operation(kOperationReparent)
{
}

ReparentOperation::ReparentOperation(const List<NodeReference> *referenceList) : Operation(kOperationReparent)
{
	const NodeReference *reference = referenceList->First();
	while (reference)
	{
		AddNode(reference->GetNode());
		reference = reference->Next();
	}
}

ReparentOperation::~ReparentOperation()
{
}

ReparentOperation::MovedReference::MovedReference(Node *node)
{
	reference = node;
	superNode = node->GetSuperNode();
	owningZone = node->GetOwningZone();
	transform = node->GetNodeTransform();
}

void ReparentOperation::AddNode(Node *node)
{
	movedList.Append(new MovedReference(node));
}

void ReparentOperation::Restore(Editor *editor)
{
	const MovedReference *moved = movedList.Last();
	while (moved)
	{
		Node *node = moved->GetNode();
		Editor::GetManipulator(node)->InvalidateGraph();

		Zone *oldZone = node->GetOwningZone();
		Zone *newZone = moved->GetOwningZone();

		if (newZone != oldZone)
		{
			node->Neutralize();
		}

		node->SetNodeTransform(moved->GetTransform());

		Node *super = moved->GetSuperNode();
		super->AppendSubnode(node);

		if (newZone != oldZone)
		{
			node->Preprocess();
		}

		super->Invalidate();
		Editor::GetManipulator(super)->InvalidateGraph();

		moved = moved->Previous();
	}

	editor->GetRootNode()->Update();
}


ZoneVertexOperation::ZoneVertexOperation(PolygonZone *polygon) : Operation(kOperationZoneVertex)
{
	zoneNode = polygon;

	const PolygonZoneObject *object = polygon->GetObject();
	const Point3D *vertex = object->GetVertexArray();

	int32 count = object->GetVertexCount();
	zoneVertexCount = count;

	for (machine a = 0; a < count; a++)
	{
		zoneVertex[a] = vertex[a];
	}
}

ZoneVertexOperation::~ZoneVertexOperation()
{
}

void ZoneVertexOperation::Restore(Editor *editor)
{
	PolygonZoneObject *object = zoneNode->GetObject();
	Point3D *vertex = object->GetVertexArray();

	int32 count = zoneVertexCount;
	object->SetVertexCount(count);

	for (machine a = 0; a < count; a++)
	{
		vertex[a] = zoneVertex[a];
	}

	zoneNode->Invalidate();
}


PortalVertexOperation::PortalVertexOperation(Portal *portal) : Operation(kOperationPortalVertex)
{
	portalNode = portal;

	const PortalObject *object = portal->GetObject();
	const Point3D *vertex = object->GetVertexArray();

	int32 count = object->GetVertexCount();
	portalVertexCount = count;

	for (machine a = 0; a < count; a++)
	{
		portalVertex[a] = vertex[a];
	}
}

PortalVertexOperation::~PortalVertexOperation()
{
}

void PortalVertexOperation::Restore(Editor *editor)
{
	PortalObject *object = portalNode->GetObject();
	Point3D *vertex = object->GetVertexArray();

	int32 count = portalVertexCount;
	object->SetVertexCount(count);

	for (machine a = 0; a < count; a++)
	{
		vertex[a] = portalVertex[a];
	}

	portalNode->Invalidate();
}


MaterialOperation::MaterialOperation(const List<NodeReference> *referenceList) : Operation(kOperationMaterial)
{
	const NodeReference *reference = referenceList->First();
	while (reference)
	{
		Node *node = reference->GetNode();
		NodeType type = node->GetNodeType();

		if (type == kNodeGeometry)
		{
			geometryList.Append(new GeometryReference(static_cast<Geometry *>(node)));
		}
		else if (type == kNodeSkybox)
		{
			skyboxList.Append(new SkyboxReference(static_cast<Skybox *>(node)));
		}
		else if (type == kNodeImpostor)
		{
			impostorList.Append(new ImpostorReference(static_cast<Impostor *>(node)));
		}
		else if (type == kNodeEffect)
		{
			Effect *effect = static_cast<Effect *>(node);
			if (effect->GetEffectType() == kEffectParticleSystem)
			{
				particleSystemList.Append(new ParticleSystemReference(static_cast<ParticleSystem *>(effect)));
			}
		}
		else if (type == kNodeInstance)
		{
			Instance *instance = static_cast<Instance *>(node);

			Modifier *modifier = instance->GetFirstModifier();
			while (modifier)
			{
				if (modifier->GetModifierType() == kModifierReplaceMaterial)
				{
					replaceMaterialModifierList.Append(new ReplaceMaterialModifierReference(instance, static_cast<ReplaceMaterialModifier *>(modifier)));
				}

				modifier = modifier->Next();
			}
		}

		reference = reference->Next();
	}
}

MaterialOperation::~MaterialOperation()
{
}

MaterialOperation::GeometryReference::GeometryReference(Geometry *geometry)
{
	reference = geometry;

	materialCount = geometry->GetMaterialCount();

	const GeometryObject *object = geometry->GetObject();
	int32 surfaceCount = object->GetSurfaceCount();

	materialStorage = new char[materialCount * sizeof(MaterialObject *) + object->GetSurfaceCount() * 4];
	materialObject = reinterpret_cast<MaterialObject **>(materialStorage);
	materialIndex = reinterpret_cast<unsigned_int32 *>(materialObject + materialCount);

	for (machine a = 0; a < materialCount; a++)
	{
		MaterialObject *material = geometry->GetMaterialObject(a);
		materialObject[a] = material;
		if (material)
		{
			material->Retain();
		}
	}

	for (machine a = 0; a < surfaceCount; a++)
	{
		materialIndex[a] = object->GetSurfaceData(a)->materialIndex;
	}
}

MaterialOperation::GeometryReference::~GeometryReference()
{
	for (machine a = 0; a < materialCount; a++)
	{
		MaterialObject *material = materialObject[a];
		if (material)
		{
			material->Release();
		}
	}

	delete[] materialStorage;
}

MaterialOperation::SkyboxReference::SkyboxReference(Skybox *skybox)
{
	reference = skybox;

	materialObject = skybox->GetMaterialObject();
	if (materialObject)
	{
		materialObject->Retain();
	}
}

MaterialOperation::SkyboxReference::~SkyboxReference()
{
	if (materialObject)
	{
		materialObject->Release();
	}
}

MaterialOperation::ImpostorReference::ImpostorReference(Impostor *impostor)
{
	reference = impostor;

	materialObject = impostor->GetMaterialObject();
	if (materialObject)
	{
		materialObject->Retain();
	}
}

MaterialOperation::ImpostorReference::~ImpostorReference()
{
	if (materialObject)
	{
		materialObject->Release();
	}
}

MaterialOperation::ParticleSystemReference::ParticleSystemReference(ParticleSystem *particleSystem)
{
	reference = particleSystem;

	materialObject = particleSystem->GetMaterialObject();
	if (materialObject)
	{
		materialObject->Retain();
	}
}

MaterialOperation::ParticleSystemReference::~ParticleSystemReference()
{
	if (materialObject)
	{
		materialObject->Release();
	}
}

MaterialOperation::ReplaceMaterialModifierReference::ReplaceMaterialModifierReference(Instance *node, ReplaceMaterialModifier *replaceMaterialModifier)
{
	instance = node;
	reference = replaceMaterialModifier;

	materialObject = replaceMaterialModifier->GetMaterialObject();
	if (materialObject)
	{
		materialObject->Retain();
	}
}

MaterialOperation::ReplaceMaterialModifierReference::~ReplaceMaterialModifierReference()
{
	if (materialObject)
	{
		materialObject->Release();
	}
}

void MaterialOperation::Restore(Editor *editor)
{
	const GeometryReference *geometryReference = geometryList.First();
	while (geometryReference)
	{
		Geometry *geometry = geometryReference->GetGeometry();
		GeometryObject *object = geometry->GetObject();
		int32 materialCount = geometryReference->GetMaterialCount();

		if (object->GetReferenceCount() == 1)
		{
			geometry->SetMaterialCount(materialCount);
			for (machine a = 0; a < materialCount; a++)
			{
				geometry->SetMaterialObject(a, geometryReference->GetMaterialObject(a));
			}

			int32 surfaceCount = object->GetSurfaceCount();
			for (machine a = 0; a < surfaceCount; a++)
			{
				object->GetSurfaceData(a)->materialIndex = (unsigned_int16) geometryReference->GetMaterialIndex(a);
			}

			geometry->OptimizeMaterials();
		}
		else
		{
			for (machine a = 0; a < materialCount; a++)
			{
				geometry->SetMaterialObject(a, geometryReference->GetMaterialObject(a));
			}

			geometry->InvalidateShaderData();
		}

		geometryReference = geometryReference->Next();
	}

	const SkyboxReference *skyboxReference = skyboxList.First();
	while (skyboxReference)
	{
		Skybox *skybox = skyboxReference->GetSkybox();
		skybox->SetMaterialObject(skyboxReference->GetMaterialObject());
		skybox->InvalidateShaderData();

		skyboxReference = skyboxReference->Next();
	}

	const ImpostorReference *impostorReference = impostorList.First();
	while (impostorReference)
	{
		Impostor *impostor = impostorReference->GetImpostor();
		impostor->SetMaterialObject(impostorReference->GetMaterialObject());

		impostorReference = impostorReference->Next();
	}

	const ParticleSystemReference *particleSystemReference = particleSystemList.First();
	while (particleSystemReference)
	{
		ParticleSystem *particleSystem = particleSystemReference->GetParticleSystem();
		particleSystem->SetMaterialObject(particleSystemReference->GetMaterialObject());

		particleSystemReference = particleSystemReference->Next();
	}

	const ReplaceMaterialModifierReference *replaceMaterialModifierReference = replaceMaterialModifierList.First();
	while (replaceMaterialModifierReference)
	{
		ReplaceMaterialModifier *replaceMaterialModifier = replaceMaterialModifierReference->GetReplaceMaterialModifier();
		replaceMaterialModifier->SetMaterialObject(replaceMaterialModifierReference->GetMaterialObject());

		if (editor->GetEditorObject()->GetEditorFlags() & kEditorExpandWorlds)
		{
			InstanceManipulator *manipulator = static_cast<InstanceManipulator *>(Editor::GetManipulator(replaceMaterialModifierReference->GetInstance()));
			manipulator->CollapseWorld();
			manipulator->ExpandWorld();
			editor->InvalidateAllViewports();
		}

		replaceMaterialModifierReference = replaceMaterialModifierReference->Next();
	}
}


GeometryOperation::GeometryOperation(Geometry *geometry) : Operation(kOperationGeometry)
{
	AddGeometry(geometry);
}

GeometryOperation::GeometryOperation(const List<NodeReference> *referenceList, bool (*filter)(const Geometry *)) : Operation(kOperationGeometry)
{
	const NodeReference *reference = referenceList->First();
	while (reference)
	{
		Node *node = reference->GetNode();

		if (node->GetNodeType() == kNodeGeometry)
		{
			Geometry *geometry = static_cast<Geometry *>(node);
			if ((!filter) || ((*filter)(geometry)))
			{
				AddGeometry(geometry);
			}
		}

		reference = reference->Next();
	}
}

GeometryOperation::~GeometryOperation()
{
}

GeometryOperation::GeometryReference::GeometryReference(Geometry *geometry)
{
	reference = geometry;
	transform = geometry->GetNodeTransform();

	const GeometryObject *object = geometry->GetObject();
	if (object->GetGeometryType() == kGeometryPrimitive)
	{
		primitiveFlags = static_cast<const PrimitiveGeometryObject *>(object)->GetPrimitiveFlags();
	}

	int32 levelCount = object->GetGeometryLevelCount();
	geometryLevelCount = levelCount;
	collisionLevel = object->GetCollisionLevel();

	geometryMesh = new Mesh[levelCount];
	for (machine a = 0; a < levelCount; a++)
	{
		geometryMesh[a].CopyMesh(object->GetGeometryLevel(a));
	}
}

GeometryOperation::GeometryReference::~GeometryReference()
{
	delete[] geometryMesh;
}

GeometryOperation::TerrainReference::TerrainReference(TerrainGeometry *terrain) : GeometryReference(terrain)
{
	const TerrainGeometryObject *object = terrain->GetObject();
	if (object->GetDetailLevel() != 0)
	{
		static_cast<const TerrainLevelGeometryObject *>(object)->SaveBorderRenderData(&borderRenderData);
	}
}

GeometryOperation::MovedReference::MovedReference(Node *node)
{
	reference = node;
	transform = node->GetNodeTransform();
}

void GeometryOperation::AddGeometry(Geometry *geometry)
{
	if (geometry->GetGeometryType() != kGeometryTerrain)
	{
		geometryList.Append(new GeometryReference(geometry));
	}
	else
	{
		geometryList.Append(new TerrainReference(static_cast<TerrainGeometry *>(geometry)));
	}

	Node *subnode = geometry->GetFirstSubnode();
	while (subnode)
	{
		movedList.Append(new MovedReference(subnode));
		subnode = subnode->Next();
	}
}

void GeometryOperation::Restore(Editor *editor)
{
	const MovedReference *moved = movedList.First();
	while (moved)
	{
		moved->GetNode()->SetNodeTransform(moved->GetTransform());
		moved = moved->Next();
	}

	const GeometryReference *reference = geometryList.First();
	while (reference)
	{
		Geometry *geometry = reference->GetGeometry();
		geometry->SetNodeTransform(reference->GetTransform());

		GeometryObject *object = geometry->GetObject();
		GeometryType type = object->GetGeometryType();
		if (type == kGeometryPrimitive)
		{
			static_cast<PrimitiveGeometryObject *>(object)->SetPrimitiveFlags(reference->GetPrimitiveFlags());
		}
		else if (type == kGeometryTerrain)
		{
			TerrainGeometryObject *terrainObject = static_cast<TerrainGeometryObject *>(object);
			if (terrainObject->GetDetailLevel() != 0)
			{
				const TerrainReference *terrainReference = static_cast<const TerrainReference *>(reference);
				static_cast<TerrainLevelGeometryObject *>(terrainObject)->RestoreBorderRenderData(terrainReference->GetBorderRenderData());
			}
		}

		int32 levelCount = reference->GetGeometryLevelCount();
		object->SetGeometryLevelCount(levelCount);
		for (machine a = 0; a < levelCount; a++)
		{
			object->GetGeometryLevel(a)->CopyMesh(reference->GetGeometryLevel(a));
		}

		if (type == kGeometryGeneric)
		{
			static_cast<GenericGeometryObject *>(object)->UpdateBounds();
		}

		object->SetCollisionLevel(reference->GetCollisionLevel());
		object->BuildCollisionData();

		editor->InvalidateGeometry(geometry);
		static_cast<GeometryManipulator *>(Editor::GetManipulator(geometry))->UpdateSurfaceSelection();

		reference = reference->Next();
	}
}


TextureOperation::TextureOperation(Geometry *geometry) : Operation(kOperationGeometry)
{
	geometryList.Append(new GeometryReference(geometry));
}

TextureOperation::TextureOperation(const List<NodeReference> *referenceList) : Operation(kOperationGeometry)
{
	const NodeReference *reference = referenceList->First();
	while (reference)
	{
		Node *node = reference->GetNode();

		if (node->GetNodeType() == kNodeGeometry)
		{
			geometryList.Append(new GeometryReference(static_cast<Geometry *>(node)));
		}

		reference = reference->Next();
	}
}

TextureOperation::~TextureOperation()
{
}

TextureOperation::GeometryReference::GeometryReference(Geometry *geometry)
{
	reference = geometry;

	const GeometryObject *object = geometry->GetObject();
	int32 levelCount = object->GetGeometryLevelCount();

	int32 texcoordCount = 0;
	for (machine a = 0; a < levelCount; a++)
	{
		const Mesh *level = object->GetGeometryLevel(a);
		if (level->GetArray<Point2D>(kArrayTexcoord))
		{
			texcoordCount += level->GetVertexCount();
		}
	}

	int32 surfaceCount = object->GetSurfaceCount();

	textureStorage = new char[texcoordCount * sizeof(Point2D) + surfaceCount * sizeof(TextureAlignData) * 2];
	texcoordArray = reinterpret_cast<Point2D *>(textureStorage);
	textureAlignData = reinterpret_cast<TextureAlignData *>(texcoordArray + texcoordCount);

	texcoordCount = 0;
	for (machine a = 0; a < levelCount; a++)
	{
		const Mesh *mesh = object->GetGeometryLevel(a);
		const Point2D *texcoord = mesh->GetArray<Point2D>(kArrayTexcoord);
		if (texcoord)
		{
			int32 vertexCount = mesh->GetVertexCount();
			MemoryMgr::CopyMemory(texcoord, &texcoordArray[texcoordCount], vertexCount * sizeof(Point2D));
			texcoordCount += vertexCount;
		}
	}

	for (machine a = 0; a < surfaceCount; a++)
	{
		const SurfaceData *data = object->GetSurfaceData(a);
		textureAlignData[a * 2] = data->textureAlignData[0];
		textureAlignData[a * 2 + 1] = data->textureAlignData[1];
	}
}

TextureOperation::GeometryReference::~GeometryReference()
{
	delete[] textureStorage;
}

void TextureOperation::Restore(Editor *editor)
{
	const NodeReference *gizmoTarget = editor->GetGizmoTarget();
	const Node *gizmoNode = (gizmoTarget) ? gizmoTarget->GetNode() : nullptr;

	const GeometryReference *reference = geometryList.First();
	while (reference)
	{
		Geometry *geometry = reference->GetGeometry();
		GeometryObject *object = geometry->GetObject();
		int32 levelCount = object->GetGeometryLevelCount();

		int32 texcoordCount = 0;
		const Point2D *texcoordArray = reference->GetTexcoordArray();

		for (machine a = 0; a < levelCount; a++)
		{
			Mesh *mesh = object->GetGeometryLevel(a);
			Point2D *texcoord = mesh->GetArray<Point2D>(kArrayTexcoord);
			if (texcoord)
			{
				int32 vertexCount = mesh->GetVertexCount();
				MemoryMgr::CopyMemory(&texcoordArray[texcoordCount], texcoord, vertexCount * sizeof(Point2D));
				texcoordCount += vertexCount;

				mesh->CalculateTangentArray();
			}
		}

		geometry->Neutralize();
		geometry->Preprocess();

		int32 surfaceCount = object->GetSurfaceCount();
		const TextureAlignData *textureAlignData = reference->GetTextureAlignData();

		for (machine a = 0; a < surfaceCount; a++)
		{
			SurfaceData *data = object->GetSurfaceData(a);
			data->textureAlignData[0] = textureAlignData[a * 2];
			data->textureAlignData[1] = textureAlignData[a * 2 + 1];
		}

		editor->PostEvent(NodeEditorEvent(kEditorEventTexcoordModified, geometry));
		if (geometry == gizmoNode)
		{
			editor->PostEvent(GizmoEditorEvent(kEditorEventGizmoTargetModified, geometry));
		}

		reference = reference->Next();
	}
}


PaintOperation::PaintOperation(const PaintSpaceObject *object, const Painter *painter) : Operation(kOperationPaint)
{
	paintSpaceObject = object;
	paintBounds = painter->GetPaintBounds();

	undoImage = painter->CreateUndoImage(paintBounds);
}

PaintOperation::~PaintOperation()
{
	Painter::ReleaseUndoImage(undoImage);
}

void PaintOperation::Restore(Editor *editor)
{
	Painter::ApplyUndoImage(paintSpaceObject, paintBounds, undoImage);
	paintSpaceObject->GetPaintTexture()->UpdateRect(paintBounds);
	editor->InvalidateViewports(kEditorViewportFrustum);
}


PathOperation::PathOperation(PathMarker *marker) : Operation(kOperationPath)
{
	pathList.Append(new PathReference(marker));
}

PathOperation::PathOperation(const List<NodeReference> *referenceList) : Operation(kOperationPath)
{
	const NodeReference *reference = referenceList->First();
	while (reference)
	{
		Node *node = reference->GetNode();
		if (node->GetNodeType() == kNodeMarker)
		{
			Marker *marker = static_cast<Marker *>(node);
			if (marker->GetMarkerType() == kMarkerPath)
			{
				pathList.Append(new PathReference(static_cast<PathMarker *>(marker)));
			}
		}

		reference = reference->Next();
	}
}

PathOperation::~PathOperation()
{
}

PathOperation::PathReference::PathReference(PathMarker *marker) : path(*marker->GetPath())
{
	reference = marker;
}

PathOperation::PathReference::~PathReference()
{
}

void PathOperation::Restore(Editor *editor)
{
	const PathReference *reference = pathList.First();
	while (reference)
	{
		PathMarker *marker = reference->GetPathMarker();
		*marker->GetPath() = *reference->GetPath();
		marker->Invalidate();

		reference = reference->Next();
	}
}


TubeEffectOperation::TubeEffectOperation(const List<NodeReference> *referenceList) : Operation(kOperationTubeEffect)
{
	const NodeReference *reference = referenceList->First();
	while (reference)
	{
		Node *node = reference->GetNode();
		if (node->GetNodeType() == kNodeEffect)
		{
			Effect *effect = static_cast<Effect *>(node);
			if (effect->GetEffectType() == kEffectTube)
			{
				tubeList.Append(new TubeReference(static_cast<TubeEffect *>(effect)));
			}
		}

		reference = reference->Next();
	}
}

TubeEffectOperation::~TubeEffectOperation()
{
}

TubeEffectOperation::TubeReference::TubeReference(TubeEffect *tube) : path(*tube->GetObject()->GetTubePath())
{
	reference = tube;
}

TubeEffectOperation::TubeReference::~TubeReference()
{
}

void TubeEffectOperation::Restore(Editor *editor)
{
	const TubeReference *reference = tubeList.First();
	while (reference)
	{
		TubeEffect *tube = reference->GetTubeEffect();
		TubeEffectObject *object = tube->GetObject();
		object->SetTubePath(reference->GetPath());
		object->Build();

		tube->Invalidate();
		tube->Neutralize();
		tube->Preprocess();

		reference = reference->Next();
	}
}


ReplaceInstanceOperation::ReplaceInstanceOperation(const List<NodeReference> *referenceList) : Operation(kOperationReplaceInstance)
{
	const NodeReference *reference = referenceList->First();
	while (reference)
	{
		Node *node = reference->GetNode();
		if (node->GetNodeType() == kNodeInstance)
		{
			instanceList.Append(new InstanceReference(static_cast<Instance *>(node)));
		}

		reference = reference->Next();
	}
}

ReplaceInstanceOperation::~ReplaceInstanceOperation()
{
}

ReplaceInstanceOperation::InstanceReference::InstanceReference(Instance *instance)
{
	reference = instance;
	worldName = instance->GetWorldName();
}

ReplaceInstanceOperation::InstanceReference::~InstanceReference()
{
}

void ReplaceInstanceOperation::Restore(Editor *editor)
{
	const InstanceReference *reference = instanceList.First();
	while (reference)
	{
		Instance *instance = reference->GetInstance();
		instance->Collapse();
		instance->SetWorldName(reference->GetWorldName());
		editor->ExpandWorld(instance);

		reference = reference->Next();
	}
}


ReplaceModifiersOperation::ReplaceModifiersOperation(const List<NodeReference> *referenceList) : Operation(kOperationReplaceModifiers)
{
	const NodeReference *reference = referenceList->First();
	while (reference)
	{
		Node *node = reference->GetNode();
		if (node->GetNodeType() == kNodeInstance)
		{
			instanceList.Append(new InstanceReference(static_cast<Instance *>(node)));
		}

		reference = reference->Next();
	}
}

ReplaceModifiersOperation::~ReplaceModifiersOperation()
{
}

ReplaceModifiersOperation::InstanceReference::InstanceReference(Instance *instance)
{
	reference = instance;

	const Modifier *modifier = instance->GetFirstModifier();
	while (modifier)
	{
		modifierList.Append(modifier->Clone());
		modifier = modifier->Next();
	}
}

ReplaceModifiersOperation::InstanceReference::~InstanceReference()
{
}

void ReplaceModifiersOperation::Restore(Editor *editor)
{
	const InstanceReference *reference = instanceList.First();
	while (reference)
	{
		Instance *instance = reference->GetInstance();
		instance->PurgeModifiers();

		for (;;)
		{
			Modifier *modifier = reference->GetFirstModifier();
			if (!modifier)
			{
				break;
			}

			instance->AddModifier(modifier);
		}

		Editor::GetManipulator(instance)->HandleSettingsUpdate();

		reference = reference->Next();
	}
}


AssociatePaintSpaceOperation::AssociatePaintSpaceOperation(const List<NodeReference> *referenceList) : Operation(kOperationAssociatePaintSpace)
{
	const NodeReference *reference = referenceList->First();
	while (reference)
	{
		Node *node = reference->GetNode();

		NodeType type = node->GetNodeType();
		if ((type == kNodeGeometry) || (type == kNodeInstance))
		{
			paintList.Append(new PaintReference(node));
		}

		reference = reference->Next();
	}
}

AssociatePaintSpaceOperation::~AssociatePaintSpaceOperation()
{
}

AssociatePaintSpaceOperation::PaintReference::PaintReference(Node *node)
{
	reference = node;

	const Connector *connector = node->GetConnector(kConnectorKeyPaint);
	if (connector)
	{
		paintSpace = static_cast<PaintSpace *>(connector->GetConnectorTarget());
		connectorFlag = true;
	}
	else
	{
		paintSpace = nullptr;
		connectorFlag = false;
	}
}

AssociatePaintSpaceOperation::PaintReference::~PaintReference()
{
}

void AssociatePaintSpaceOperation::Restore(Editor *editor)
{
	const PaintReference *paintReference = paintList.First();
	while (paintReference)
	{
		Node *node = paintReference->GetNode();

		NodeType type = node->GetNodeType();
		if (type == kNodeGeometry)
		{
			Geometry *geometry = static_cast<Geometry *>(node);

			geometry->SetConnectedPaintSpace(paintReference->GetPaintSpace());
			geometry->InvalidateShaderData();

			if (!paintReference->GetConnectorFlag())
			{
				node->RemoveConnector(kConnectorKeyPaint);
				Editor::GetManipulator(geometry)->UpdateConnectors();
			}
		}
		else if (type == kNodeInstance)
		{
			Instance *instance = static_cast<Instance *>(node);

			Connector *connector = instance->GetConnector(kConnectorKeyPaint);
			if (connector)
			{
				if (paintReference->GetConnectorFlag())
				{
					connector->SetConnectorTarget(paintReference->GetPaintSpace());
				}
				else
				{
					instance->RemoveConnector(kConnectorKeyPaint);
				}

				InstanceManipulator *manipulator = static_cast<InstanceManipulator *>(Editor::GetManipulator(instance));
				manipulator->InvalidatePaintSpace();
				manipulator->UpdateConnectors();
			}
		}

		paintReference = paintReference->Next();
	}
}


ObjectDataOperation::ObjectDataOperation(const List<NodeReference> *referenceList, bool (*filter)(const Node *)) : Operation(kOperationObjectData)
{
	const NodeReference *reference = referenceList->First();
	while (reference)
	{
		Node *node = reference->GetNode();
		if ((!filter) || ((*filter)(node)))
		{
			objectDataList.Append(new ObjectDataReference(node));
		}

		reference = reference->Next();
	}
}

ObjectDataOperation::~ObjectDataOperation()
{
}

ObjectDataOperation::ObjectDataReference::ObjectDataReference(Node *node) : objectPackage(nullptr)
{
	reference = node;

	const Object *object = node->GetObject();
	if (object)
	{
		Packer objectPacker(&objectPackage);
		object->Pack(objectPacker, kPackEditor | kPackSettings);
	}
}

ObjectDataOperation::ObjectDataReference::~ObjectDataReference()
{
}

void ObjectDataOperation::Restore(Editor *editor)
{
	const NodeReference *gizmoReference = editor->GetGizmoTarget();
	Node *gizmoTarget = (gizmoReference) ? gizmoReference->GetNode() : nullptr;
	bool gizmoEvent = false;

	const ObjectDataReference *reference = objectDataList.First();
	while (reference)
	{
		Node *node = reference->GetNode();
		if (node == gizmoTarget)
		{
			gizmoEvent = true;
		}

		node->Neutralize();

		Object *object = node->GetObject();
		if (object)
		{
			void *cookie = object->BeginSettingsUnpack();
			Unpacker objectUnpacker(reference->GetObjectPackage()->GetStorage(), kEngineInternalVersion);
			object->Unpack(objectUnpacker, kUnpackEditor);
			object->EndSettingsUnpack(cookie);
		}

		node->Preprocess();
		Editor::GetManipulator(node)->HandleSettingsUpdate();

		reference = reference->Next();
	}

	if (gizmoEvent)
	{
		editor->PostEvent(GizmoEditorEvent(kEditorEventGizmoTargetModified, gizmoTarget));
	}

	editor->PostEvent(EditorEvent(kEditorEventNodeInfoModified));
}


NodeInfoOperation::NodeInfoOperation(const List<NodeReference> *referenceList) : Operation(kOperationNodeInfo)
{
	const NodeReference *reference = referenceList->First();
	while (reference)
	{
		nodeList.Append(new NodeInfoReference(reference->GetNode()));
		reference = reference->Next();
	}
}

NodeInfoOperation::~NodeInfoOperation()
{
}

NodeInfoOperation::NodeInfoReference::NodeInfoReference(Node *node) :
		nodePackage(nullptr),
		objectPackage(nullptr),
		propertyPackage(nullptr)
{
	reference = node;

	Packer nodePacker(&nodePackage);
	node->Pack(nodePacker, kPackEditor | kPackSettings);

	const Object *object = node->GetObject();
	if (object)
	{
		Packer objectPacker(&objectPackage);
		object->Pack(objectPacker, kPackEditor | kPackSettings);
	}

	const PropertyObject *propertyObject = node->GetPropertyObject();
	if (propertyObject)
	{
		Packer propertyPacker(&propertyPackage);
		propertyObject->Pack(propertyPacker, kPackEditor | kPackSettings);

		const Node *root = node->GetRootNode();
		Node *subnode = root->GetFirstSubnode();
		while (subnode)
		{
			if ((subnode->GetPropertyObject() == propertyObject) && (subnode != node))
			{
				propertyObjectList.Append(new NodeReference(subnode));
			}

			subnode = root->GetNextNode(subnode);
		}
	}

	const Hub *hub = node->GetHub();
	if (hub)
	{
		Connector *connector = hub->GetFirstOutgoingEdge();
		while (connector)
		{
			connectorArray.AddElement(ConnectorData(connector));
			connector = connector->GetNextOutgoingEdge();
		}
	}
}

NodeInfoOperation::NodeInfoReference::~NodeInfoReference()
{
}

void NodeInfoOperation::Restore(Editor *editor)
{
	const NodeReference *gizmoReference = editor->GetGizmoTarget();
	Node *gizmoTarget = (gizmoReference) ? gizmoReference->GetNode() : nullptr;
	bool gizmoEvent = false;

	const NodeInfoReference *reference = nodeList.First();
	while (reference)
	{
		Node *node = reference->GetNode();
		if (node == gizmoTarget)
		{
			gizmoEvent = true;
		}

		EditorManipulator *manipulator = Editor::GetManipulator(node);
		manipulator->UnselectConnector();

		node->Neutralize();

		void *cookie = node->BeginSettingsUnpack();
		Unpacker nodeUnpacker(reference->GetNodePackage()->GetStorage(), kEngineInternalVersion);
		node->Unpack(nodeUnpacker, kUnpackEditor);
		node->EndSettingsUnpack(cookie);

		Object *object = node->GetObject();
		if (object)
		{
			cookie = object->BeginSettingsUnpack();
			Unpacker objectUnpacker(reference->GetObjectPackage()->GetStorage(), kEngineInternalVersion);
			object->Unpack(objectUnpacker, kUnpackEditor);
			object->EndSettingsUnpack(cookie);
		}

		PropertyObject *propertyObject = node->GetPropertyObject();
		const Package *propertyPackage = reference->GetPropertyPackage();
		if (propertyPackage->GetSize() != 0)
		{
			if (propertyObject)
			{
				cookie = propertyObject->BeginSettingsUnpack();
				Unpacker propertyUnpacker(propertyPackage->GetStorage(), kEngineInternalVersion);
				propertyObject->Unpack(propertyUnpacker, kUnpackEditor);
				propertyObject->EndSettingsUnpack(cookie);
			}
			else
			{
				propertyObject = new PropertyObject;
				node->SetPropertyObject(propertyObject);
				propertyObject->Release();

				const NodeReference *propertyReference = reference->GetFirstPropertyObjectNode();
				while (propertyReference)
				{
					propertyReference->GetNode()->SetPropertyObject(propertyObject);
					propertyReference = propertyReference->Next();
				}

				Unpacker propertyUnpacker(propertyPackage->GetStorage(), kEngineInternalVersion);
				propertyObject->Unpack(propertyUnpacker, kUnpackEditor);
			}
		}
		else
		{
			if (propertyObject)
			{
				if (propertyObject->GetReferenceCount() == 1)
				{
					node->SetPropertyObject(nullptr);
				}
				else
				{
					Node *root = editor->GetRootNode();
					Node *subnode = root->GetFirstSubnode();
					while (subnode)
					{
						if (subnode->GetPropertyObject() == propertyObject)
						{
							subnode->SetPropertyObject(nullptr);
						}

						subnode = root->GetNextNode(subnode);
					}
				}
			}
		}

		const Array<ConnectorData>& connectorArray = reference->GetConnectorArray();
		for (const ConnectorData& data : connectorArray)
		{
			node->SetConnectedNode(data.connectorKey, data.targetNode);
		}

		node->Preprocess();
		manipulator->HandleSettingsUpdate();

		reference = reference->Next();
	}

	if (gizmoEvent)
	{
		editor->PostEvent(GizmoEditorEvent(kEditorEventGizmoTargetModified, gizmoTarget));
	}

	editor->PostEvent(EditorEvent(kEditorEventNodeInfoModified));
}

// ZYUQURM
