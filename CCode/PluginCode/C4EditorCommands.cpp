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


#include "C4EditorCommands.h"
#include "C4EditorSupport.h"
#include "C4GeometryManipulators.h"
#include "C4InstanceManipulators.h"
#include "C4WorldEditor.h"
#include "C4ScriptEditor.h"
#include "C4PanelEditor.h"
#include "C4Application.h"
#include "C4ConfigData.h"
#include "C4ToolWindows.h"
#include "C4World.h"


using namespace C4;


void Editor::HandleCloseMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	Close();
}

void Editor::HandleSaveWorldMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	if (resourceName[0] == 0)
	{
		HandleSaveWorldAsMenuItem(nullptr, nullptr);
	}
	else
	{
		SaveWorld();
	}
}

void Editor::HandleSaveWorldAsMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	FilePicker *picker = new WorldSavePicker;
	picker->SetCompletionProc(&SavePickerProc, this);
	AddSubwindow(picker);
}

void Editor::HandleSaveAndPlayWorldMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	if (resourceName[0] == 0)
	{
		HandleSaveWorldAsMenuItem(nullptr, nullptr);
	}
	else
	{
		if ((!(editorState & kEditorWorldUnsaved)) || (SaveWorld()))
		{
			ResourceName name(resourceName);
			Close();

			TheInterfaceMgr->GetStrip()->Hide();
			TheConsoleWindow->Close();

			TheWorldEditor->SetPlayedWorldName(name);
			TheApplication->LoadWorld(name);
		}
	}
}

void Editor::HandleImportSceneMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	SceneImportPicker *picker = new SceneImportPicker(nullptr, kSceneImportGeometry);
	picker->SetCompletionProc(&SceneImportPickerProc, this);
	AddSubwindow(picker);
}

void Editor::HandleExportSceneMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	SceneExportPicker *picker = new SceneExportPicker(nullptr);
	picker->SetCompletionProc(&SceneExportPickerProc, this);
	AddSubwindow(picker);
}

void Editor::HandleLoadModelResourceMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	const char *title = TheWorldEditor->GetStringTable()->GetString(StringID('IMDL'));
	FilePicker *picker = new FilePicker('WMDL', title, TheResourceMgr->GetGenericCatalog(), ModelResource::GetDescriptor());
	picker->SetCompletionProc(&ModelLoadPickerProc, this);
	AddSubwindow(picker);
}

void Editor::HandleSaveModelResourceMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	const char *title = TheWorldEditor->GetStringTable()->GetString(StringID('EMDL'));
	FilePicker *picker = new FilePicker('WMDL', title, TheResourceMgr->GetGenericCatalog(), ModelResource::GetDescriptor(), nullptr, kFilePickerSave);
	picker->SetCompletionProc(&ModelSavePickerProc, this);
	AddSubwindow(picker);

	const char *name = resourceName;
	int32 directoryLength = Text::GetDirectoryPathLength(name);
	picker->SetFileName(&name[directoryLength]);
}

void Editor::HandleUndoMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	for (;;)
	{
		Operation *operation = operationList.Last(); 
		if (!operation)
		{
			break; 
		}
 
		bool coupled = operation->Coupled();
		operation->Restore(this);
		delete operation; 

		if (!coupled) 
		{ 
			break;
		}
	}
 
	if (operationList.Empty())
	{
		editorMenuItem[kEditorMenuUndo]->Disable();
	}

	editorState |= kEditorWorldUnsaved | kEditorRedrawViewports | kEditorUpdateConditionalItems;
}

void Editor::HandleCutMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	const Node *targetZone = GetTargetZone();

	NodeReference *reference = GetFirstSelectedNode();
	while (reference)
	{
		Node *node = reference->GetNode();
		if (node == targetZone)
		{
			SetTargetZone(GetRootNode());
			break;
		}

		reference = reference->Next();
	}

	HandleCopyMenuItem(nullptr, nullptr);
	HandleClearMenuItem(nullptr, nullptr);
}

void Editor::HandleCopyMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	delete editorClipboard;

	Node *root = GetRootNode();
	InfiniteZone *zone = static_cast<InfiniteZone *>(root);
	Object *auxiliaryObject = zone->GetAuxiliaryObject();
	auxiliaryObject->Retain();
	zone->SetAuxiliaryObject(nullptr);

	Manipulator *manipulator = root->GetManipulator();
	unsigned_int32 state = manipulator->GetManipulatorState();
	manipulator->SetManipulatorState(state | kManipulatorSelected);

	editorClipboard = new Package(nullptr);
	root->PackTree(editorClipboard, kPackSelected | kPackEditor);

	zone->SetAuxiliaryObject(auxiliaryObject);
	auxiliaryObject->Release();

	manipulator->SetManipulatorState(state);

	editorMenuItem[kEditorMenuPaste]->Enable();
}

void Editor::HandlePasteMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	Paste(GetTargetZone());
}

void Editor::HandlePasteSubnodesMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	Paste(GetFirstSelectedNode()->GetNode());
}

void Editor::HandleClearMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	NodeReference *reference = GetFirstSelectedNode();
	if (reference)
	{
		if ((reference->GetNode() != GetRootNode()) || (reference->Next()))
		{
			do
			{
				Node *root = reference->GetNode()->GetSuperNode();
				if ((root) && (root->GetNodeType() == kNodeGeneric))
				{
					const Manipulator *manipulator = root->GetManipulator();
					if ((!manipulator->Selected()) && (EntireGroupSelected(root)))
					{
						SelectNode(root);
					}
				}

				reference = reference->Next();

			} while (reference);

			AddOperation(new DeleteOperation(GetSelectionList()));

			reference = GetFirstSelectedNode();
			do
			{
				NodeReference *next = reference->Next();
				DeleteNode(reference->GetNode(), true);
				reference = next;
			} while (reference);
		}
	}
}

void Editor::HandleSelectAllMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	SelectAll(GetRootNode());
}

void Editor::HandleSelectAllZoneMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	NodeReference *reference = GetFirstSelectedNode();
	while (reference)
	{
		const Node *node = reference->GetNode();
		if (node->GetNodeType() == kNodeZone)
		{
			const Zone *zone = static_cast<const Zone *>(node);

			Node *subnode = zone->GetFirstSubnode();
			while (subnode)
			{
				SelectNode(subnode);
				subnode = subnode->Next();
			}

			for (machine a = 0; a < kCellGraphCount; a++)
			{
				SelectAllCell(zone->GetCellGraphSite(a));
			}
		}

		reference = reference->Next();
	}
}

void Editor::HandleSelectAllMaskMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	UnselectAll();
	SelectAllMask(GetRootNode());
}

void Editor::HandleSelectMaterialMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	UnselectAll();
	SelectWithMaterial();
}

void Editor::HandleSelectSuperNodeMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	NodeReference *reference = GetLastSelectedNode();
	while (reference)
	{
		Node *super = reference->GetNode()->GetSuperNode();
		if (super)
		{
			SelectNode(super);
		}

		reference = reference->Previous();
	}
}

void Editor::HandleSelectSubtreeMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	NodeReference *reference = GetFirstSelectedNode();
	while (reference)
	{
		Node *node = reference->GetNode()->GetFirstSubnode();
		while (node)
		{
			SelectNode(node);
			node = node->Next();
		}

		reference = reference->Next();
	}
}

void Editor::HandleLockSelectionMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	editorState |= kEditorSelectionLocked | kEditorUpdateConditionalItems;
}

void Editor::HandleUnlockSelectionMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	editorState = (editorState & ~kEditorSelectionLocked) | kEditorUpdateConditionalItems;
}

void Editor::HandleDuplicateMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	HandleCopyMenuItem(nullptr, nullptr);
	HandlePasteMenuItem(nullptr, nullptr);
}

void Editor::HandleCloneMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	List<NodeReference>		cloneList;

	Zone *targetZone = GetTargetZone();

	NodeReference *reference = GetFirstSelectedNode();
	while (reference)
	{
		const Node *node = reference->GetNode();
		if (!GetManipulator(node)->PredecessorSelected())
		{
			Node *clone = node->Clone();
			cloneList.Append(new NodeReference(clone));
		}

		reference = reference->Next();
	}

	UnselectAll();

	reference = cloneList.First();
	while (reference)
	{
		Node *clone = reference->GetNode();
		targetZone->AppendSubnode(clone);

		EditorManipulator::Install(this, clone);
		GetManipulator(clone)->InvalidateGraph();

		clone->Preprocess();
		SelectNode(clone);

		Node *node = clone->GetFirstSubnode();
		while (node)
		{
			SelectNode(node);
			node = clone->GetNextNode(node);
		}

		reference = reference->Next();
	}

	AddOperation(new PasteOperation(&cloneList));
}

void Editor::HandleNodeInfoMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	if (!selectionList.Empty())
	{
		AddSubwindow(new NodeInfoWindow(this));
	}
}

void Editor::HandleEditControllerMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	NodeReference *reference = GetFirstSelectedNode();
	if (reference)
	{
		const Node *targetNode = reference->GetNode();
		Controller *controller = targetNode->GetController();
		if (controller)
		{
			ControllerType type = controller->GetBaseControllerType();
			if (type == kControllerScript)
			{
				ScriptController *scriptController = static_cast<ScriptController *>(controller);
				ScriptObject *scriptObject = scriptController->GetScriptObject();
				if (!scriptObject)
				{
					scriptObject = new ScriptObject;

					ScriptGraph *graph = new ScriptGraph;
					graph->GetScriptEventArray()->AddElement(kEventControllerActivate);
					scriptObject->AddScriptGraph(graph);

					scriptController->SetScriptObject(scriptObject);
					scriptObject->Release();
				}

				AddSubwindow(new ScriptEditor(targetNode, scriptController, scriptObject));
				SetWorldUnsavedFlag();
			}
			else if (type == kControllerPanel)
			{
				AddSubwindow(new PanelEditor(static_cast<const PanelEffect *>(targetNode)));
				SetWorldUnsavedFlag();
			}
		}
	}
}

void Editor::HandleGroupMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	List<NodeReference>		referenceList;

	NodeReference *reference = GetFirstSelectedNode();
	while (reference)
	{
		Node *node = reference->GetNode();
		if (node->GetSuperNode()->GetNodeType() != kNodeGeneric)
		{
			referenceList.Append(new NodeReference(node));
		}

		reference = reference->Next();
	}

	GroupOperation *operation = new GroupOperation;
	AddOperation(operation);

	for (;;)
	{
		reference = referenceList.First();
		if (!reference)
		{
			break;
		}

		Node *group = new Node;
		EditorManipulator::Install(this, group);

		Node *super = reference->GetNode()->GetSuperNode();
		do
		{
			NodeReference *next = reference->Next();

			Node *node = reference->GetNode();
			if (node->GetSuperNode() == super)
			{
				GetManipulator(node)->InvalidateGraph();
				node->Neutralize();
				group->AppendSubnode(node);
				delete reference;
			}

			reference = next;
		} while (reference);

		super->AppendNewSubnode(group);
		operation->AddGroup(group);

		GetManipulator(group)->InvalidateGraph();
	}
}

void Editor::HandleResetTransformMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	AddOperation(new MoveOperation(GetSelectionList()));

	NodeReference *reference = GetFirstSelectedNode();
	while (reference)
	{
		Node *node = reference->GetNode();
		if (node->GetSuperNode())
		{
			node->SetNodeTransform(Identity4D);
			node->Invalidate();
		}

		reference = reference->Next();
	}

	rootNode->Update();

	reference = GetFirstSelectedNode();
	while (reference)
	{
		Node *node = reference->GetNode();
		if (node->GetNodeType() == kNodeGeometry)
		{
			RebuildGeometry(static_cast<Geometry *>(node));
		}

		reference = reference->Next();
	}
}

void Editor::HandleAlignToGridMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	AddOperation(new MoveOperation(GetSelectionList()));

	float spacing = GetEditorObject()->GetGridLineSpacing();
	float inverse = 1.0F / spacing;

	NodeReference *reference = GetFirstSelectedNode();
	while (reference)
	{
		Node *node = reference->GetNode();
		const Node *super = node->GetSuperNode();
		if (super)
		{
			Point3D p = node->GetWorldPosition();
			p.x = Floor(p.x * inverse + 0.5F) * spacing;
			p.y = Floor(p.y * inverse + 0.5F) * spacing;
			p.z = Floor(p.z * inverse + 0.5F) * spacing;

			node->SetNodePosition(super->GetInverseWorldTransform() * p);
			node->Invalidate();
		}

		reference = reference->Next();
	}

	rootNode->Update();

	reference = GetFirstSelectedNode();
	while (reference)
	{
		Node *node = reference->GetNode();
		if (node->GetNodeType() == kNodeGeometry)
		{
			RebuildGeometry(static_cast<Geometry *>(node));
		}

		reference = reference->Next();
	}
}

void Editor::HandleSetTargetZoneMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	NodeReference *reference = GetFirstSelectedNode();
	while (reference)
	{
		Node *node = reference->GetNode();
		if (node->GetNodeType() == kNodeZone)
		{
			SetTargetZone(static_cast<Zone *>(node));
			break;
		}

		reference = reference->Next();
	}
}

void Editor::HandleSetInfiniteTargetZoneMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	SetTargetZone(GetRootNode());
}

void Editor::HandleMoveToTargetZoneMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	ReparentSelectedNodes(GetTargetZone());
}

void Editor::HandleConnectNodeMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	NodeReference *reference = GetFirstSelectedNode();
	if (reference)
	{
		ConnectOperation *connectOperation = new ConnectOperation(&selectedConnectorList);
		AddOperation(connectOperation);

		bool connection = false;

		Node *target = reference->GetNode();
		EditorManipulator *manipulator = selectedConnectorList.First();
		while (manipulator)
		{
			Node *node = manipulator->GetTargetNode();
			if (node != target)
			{
				const ConnectorKey		*connectorKey;

				if (manipulator->SetConnectorSelectionTarget(target, &connectorKey))
				{
					connection = true;
					node->Invalidate();

					if (!target->GetController())
					{
						const Controller *controller = node->GetController();
						if (controller)
						{
							// If the connecting node has a script controller that references the target node such
							// that a method would need to send messages to the target node's controller, then assign
							// a generic controller to the target node if it doesn't already have a controller.

							ControllerType type = controller->GetBaseControllerType();
							if (type == kControllerScript)
							{
								AssignGenericControllers(static_cast<const ScriptController *>(controller)->GetScriptObject(), *connectorKey, target, connectOperation);
							}
							else if (type == kControllerPanel)
							{
								const Panel *panel = static_cast<const PanelController *>(controller)->GetRootWidget();
								const Widget *widget = panel->GetFirstSubnode();
								while (widget)
								{
									AssignGenericControllers(widget->GetScriptObject(), *connectorKey, target, connectOperation);
									widget = panel->GetNextNode(widget);
								}
							}
						}
					}
				}
			}

			manipulator = manipulator->Next();
		}

		if (!connection)
		{
			DeleteLastOperation();
		}
	}

	editorState |= kEditorUpdateConditionalItems;
}

void Editor::HandleUnconnectNodeMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	AddOperation(new ConnectOperation(&selectedConnectorList));

	EditorManipulator *manipulator = selectedConnectorList.First();
	while (manipulator)
	{
		manipulator->SetConnectorSelectionTarget(nullptr);
		manipulator->GetTargetNode()->Invalidate();
		manipulator = manipulator->Next();
	}

	editorState |= kEditorUpdateConditionalItems;
}

void Editor::HandleConnectInfiniteZoneMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	AddOperation(new ConnectOperation(&selectedConnectorList));
	bool undoFlag = false;

	EditorManipulator *manipulator = selectedConnectorList.First();
	while (manipulator)
	{
		if (manipulator->GetTargetNode() != rootNode)
		{
			undoFlag |= manipulator->SetConnectorSelectionTarget(rootNode);
		}

		manipulator = manipulator->Next();
	}

	if (!undoFlag)
	{
		DeleteLastOperation();
	}

	editorState |= kEditorUpdateConditionalItems;
}

void Editor::HandleSelectConnectedNodeMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	UnselectAll();

	const EditorManipulator *manipulator = selectedConnectorList.First();
	while (manipulator)
	{
		Node *node = manipulator->GetConnectorSelectionTarget();
		if (node)
		{
			SelectNode(node);
		}

		manipulator = manipulator->Next();
	}
}

void Editor::HandleSelectIncomingConnectingNodesMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	List<NodeReference>		referenceList;

	const NodeReference *reference = GetFirstSelectedNode();
	while (reference)
	{
		referenceList.Append(new NodeReference(reference->GetNode()));
		reference = reference->Next();
	}

	UnselectAll();

	reference = referenceList.First();
	while (reference)
	{
		const Hub *incomingHub = reference->GetNode()->GetHub();
		if (incomingHub)
		{
			const Connector *connector = incomingHub->GetFirstIncomingEdge();
			while (connector)
			{
				const Hub *outgoingHub = connector->GetStartElement();
				if (outgoingHub != incomingHub)
				{
					SelectNode(outgoingHub->GetNode());
				}

				connector = connector->GetNextIncomingEdge();
			}
		}

		reference = reference->Next();
	}
}

void Editor::HandleMoveViewportCameraToNodeMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	const NodeReference *reference = GetFirstSelectedNode();
	if (reference)
	{
		const Transform4D& transform = reference->GetNode()->GetWorldTransform();
		const Vector3D& view = transform[2];
		const Point3D& position = transform.GetTranslation();

		float azm = ((view.x != 0.0F) || (view.y != 0.0F)) ? Atan(view.y, view.x) : 0.0F;
		float alt = Clamp(Atan(view.z, Sqrt(view.x * view.x + view.y * view.y)), -1.45F, 1.45F);

		for (machine a = 0; a < kEditorViewportCount; a++)
		{
			ViewportWidget *viewport = GetViewport(a)->GetViewportWidget();
			if (viewport->GetWidgetType() == kWidgetFrustumViewport)
			{
				static_cast<FrustumViewportWidget *>(viewport)->SetCameraTransform(azm, alt, position);
			}
		}
	}
}

void Editor::HandleOpenInstancedWorldMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	const NodeReference *reference = GetFirstSelectedNode();
	while (reference)
	{
		const Node *node = reference->GetNode();

		if (node->GetNodeType() == kNodeInstance)
		{
			const ResourceName& name = static_cast<const Instance *>(node)->GetWorldName();
			if (name[0] != 0)
			{
				Open(name);
			}
		}

		reference = reference->Next();
	}
}

void Editor::HandleRebuildGeometryMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	AddOperation(new GeometryOperation(GetSelectionList()));

	const NodeReference *reference = GetFirstSelectedNode();
	while (reference)
	{
		Node *node = reference->GetNode();
		if (node->GetNodeType() == kNodeGeometry)
		{
			RebuildGeometry(static_cast<Geometry *>(node));
		}

		reference = reference->Next();
	}
}

void Editor::HandleRebuildWithNewPathMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	Operation *geometryOperation = nullptr;
	Operation *effectOperation = nullptr;

	const NodeReference *reference = GetFirstSelectedNode();
	while (reference)
	{
		Node *node = reference->GetNode();

		NodeType type = node->GetNodeType();
		if (type == kNodeGeometry)
		{
			Geometry *geometry = static_cast<Geometry *>(node);
			if (geometry->GetGeometryType() == kGeometryPrimitive)
			{
				PrimitiveGeometry *primitive = static_cast<PrimitiveGeometry *>(geometry);
				if (primitive->PathPrimitive())
				{
					PathPrimitiveGeometry *pathPrimitive = static_cast<PathPrimitiveGeometry *>(primitive);
					const PathMarker *marker = pathPrimitive->GetConnectedPathMarker();
					if (marker)
					{
						if (!geometryOperation)
						{
							auto filter = [](const Geometry *geom) -> bool
							{
								return (geom->GetGeometryType() == kGeometryPrimitive);
							};

							geometryOperation = new GeometryOperation(GetSelectionList(), filter);
							if (effectOperation)
							{
								geometryOperation->SetCoupledFlag(true);
							}

							AddOperation(geometryOperation);
						}

						pathPrimitive->GetObject()->SetPrimitivePath(marker->GetPath());
						RebuildGeometry(pathPrimitive);
					}
				}
			}
		}
		else if (type == kNodeEffect)
		{
			Effect *effect = static_cast<Effect *>(node);
			if (effect->GetEffectType() == kEffectTube)
			{
				TubeEffect *tube = static_cast<TubeEffect *>(effect);
				const PathMarker *marker = tube->GetConnectedPathMarker();
				if (marker)
				{
					if (!effectOperation)
					{
						effectOperation = new TubeEffectOperation(GetSelectionList());
						if (geometryOperation)
						{
							effectOperation->SetCoupledFlag(true);
						}

						AddOperation(effectOperation);
					}

					TubeEffectObject *object = tube->GetObject();
					object->SetTubePath(marker->GetPath());
					object->Build();

					tube->Invalidate();
					tube->Neutralize();
					tube->Preprocess();
				}
			}
		}

		reference = reference->Next();
	}
}

void Editor::HandleRecalculateNormalsMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	AddOperation(new GeometryOperation(GetSelectionList()));

	const NodeReference *reference = GetFirstSelectedNode();
	while (reference)
	{
		Node *node = reference->GetNode();
		if (node->GetNodeType() == kNodeGeometry)
		{
			bool tangentsBuilt = false;

			Geometry *geometry = static_cast<Geometry *>(node);
			const GeometryObject *object = geometry->GetObject();

			int32 levelCount = object->GetGeometryLevelCount();
			for (machine a = 0; a < levelCount; a++)
			{
				Mesh *geometryMesh = object->GetGeometryLevel(a);
				if (geometryMesh->GetArray(kArrayNormal))
				{
					geometryMesh->CalculateNormalArray();
					if (geometryMesh->GetArray(kArrayTangent))
					{
						if (geometryMesh->GetArrayDescriptor(kArrayTangent)->componentCount == 4)
						{
							geometryMesh->CalculateTangentArray();
						}
						else
						{
							Mesh	tempMesh;

							tempMesh.CopyMesh(geometryMesh);
							geometryMesh->BuildTangentArray(&tempMesh);
							tangentsBuilt = true;
						}
					}
				}
				else
				{
					Mesh	tempMesh;

					tempMesh.BuildNormalArray(geometryMesh);
					geometryMesh->BuildTangentArray(&tempMesh);
					tangentsBuilt = true;
				}
			}

			if (tangentsBuilt)
			{
				InvalidateGeometry(geometry);
			}
		}

		reference = reference->Next();
	}
}

void Editor::HandleBakeTransformIntoVerticesMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	auto filter = [](const Geometry *geometry) -> bool
	{
		return (geometry->GetGeometryType() == kGeometryGeneric);
	};

	AddOperation(new GeometryOperation(GetSelectionList(), filter));

	const NodeReference *reference = GetFirstSelectedNode();
	while (reference)
	{
		Node *node = reference->GetNode();

		if (node->GetNodeType() == kNodeGeometry)
		{
			Geometry *geometry = static_cast<Geometry *>(node);
			if (geometry->GetGeometryType() == kGeometryGeneric)
			{
				const Transform4D& transform = geometry->GetNodeTransform();
				GeometryObject *object = geometry->GetObject();

				int32 levelCount = object->GetGeometryLevelCount();
				for (machine a = 0; a < levelCount; a++)
				{
					Mesh *mesh = object->GetGeometryLevel(a);
					mesh->TransformMesh(transform);
				}

				static_cast<GenericGeometryObject *>(object)->UpdateBounds();
				object->BuildCollisionData();

				Node *subnode = geometry->GetFirstSubnode();
				while (subnode)
				{
					subnode->SetNodeTransform(transform * subnode->GetNodeTransform());
					subnode = subnode->Next();
				}

				geometry->SetNodeTransform(Identity4D);
				InvalidateGeometry(geometry);
			}
		}

		reference = reference->Next();
	}
}

void Editor::HandleRepositionMeshOriginMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	AddSubwindow(new MeshOriginWindow(this));
}

void Editor::HandleSetMaterialMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	AddOperation(new MaterialOperation(GetSelectionList()));

	MaterialObject *materialObject = GetSelectedMaterial()->GetMaterialObject();

	const NodeReference *reference = GetFirstSelectedNode();
	while (reference)
	{
		Node *node = reference->GetNode();
		if (node->GetNodeType() == kNodeGeometry)
		{
			Geometry *geometry = static_cast<Geometry *>(node);
			GeometryManipulator *manipulator = static_cast<GeometryManipulator *>(geometry->GetManipulator());

			GeometryObject *object = geometry->GetObject();
			if (object->GetReferenceCount() == 1)
			{
				int32 materialCount = geometry->GetMaterialCount();

				if (manipulator->GetSelectionType() == kEditorSelectionSurface)
				{
					int32	materialIndex;

					for (machine a = 0; a < materialCount; a++)
					{
						if (geometry->GetMaterialObject(a) == materialObject)
						{
							materialIndex = a;
							goto found;
						}
					}

					materialIndex = materialCount;
					geometry->SetMaterialCount(++materialCount);
					geometry->SetMaterialObject(materialIndex, materialObject);

					found:
					int32 surfaceCount = object->GetSurfaceCount();
					if (surfaceCount != 0)
					{
						for (machine a = 0; a < surfaceCount; a++)
						{
							if (manipulator->SurfaceSelected(a))
							{
								object->GetSurfaceData(a)->materialIndex = (unsigned_int16) materialIndex;
							}
						}
					}
					else
					{
						geometry->SetMaterialObject(0, materialObject);
					}
				}
				else
				{
					manipulator->SetMaterial(materialObject);
				}

				geometry->OptimizeMaterials();
				object->BuildCollisionData();
			}
			else
			{
				if (manipulator->GetSelectionType() == kEditorSelectionSurface)
				{
					int32 surfaceCount = object->GetSurfaceCount();
					if (surfaceCount != 0)
					{
						for (machine a = 0; a < surfaceCount; a++)
						{
							if (manipulator->SurfaceSelected(a))
							{
								geometry->SetMaterialObject(object->GetSurfaceData(a)->materialIndex, materialObject);
							}
						}
					}
					else
					{
						geometry->SetMaterialObject(0, materialObject);
					}
				}
				else
				{
					manipulator->SetMaterial(materialObject);
				}
			}

			InvalidateGeometry(geometry);
		}
		else
		{
			Editor::GetManipulator(node)->SetMaterial(materialObject);
		}

		reference = reference->Next();
	}
}

void Editor::HandleRemoveMaterialMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	AddOperation(new MaterialOperation(GetSelectionList()));

	const NodeReference *reference = GetFirstSelectedNode();
	while (reference)
	{
		Editor::GetManipulator(reference->GetNode())->RemoveMaterial();
		reference = reference->Next();
	}
}

void Editor::HandleCombineDetailLevelsMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	Array<Geometry *, kMaxCombineGeometryCount>		combineArray;

	int32 combineCount = 0;
	const NodeReference *reference = GetFirstSelectedNode();
	while (reference)
	{
		Node *node = reference->GetNode();
		if (node->GetNodeType() == kNodeGeometry)
		{
			Geometry *geometry = static_cast<Geometry *>(node);
			int32 vertexCount = geometry->GetObject()->GetGeometryLevel(0)->GetVertexCount();

			int32 index = combineCount;
			for (machine a = 0; a < combineCount; a++)
			{
				int32 count = combineArray[a]->GetObject()->GetGeometryLevel(0)->GetVertexCount();
				if (count == vertexCount)
				{
					goto next;
				}

				if (count < vertexCount)
				{
					index = a;
					break;
				}
			}

			combineArray.InsertElement(index, geometry);

			if (++combineCount == kMaxCombineGeometryCount)
			{
				break;
			}
		}

		next:
		reference = reference->Next();
	}

	if (combineCount > 1)
	{
		Geometry *primaryGeometry = combineArray[0];
		GeometryObject *primaryObject = primaryGeometry->GetObject();
		Mesh *primaryMesh = primaryObject->GetGeometryLevel(0);

		if (primaryMesh->GetSkinWeightData())
		{
			bool match = true;
			int32 boneCount = 0;

			const ArrayBundle *bundle = primaryMesh->GetArrayBundle(kArrayInverseBindTransform);
			if (bundle)
			{
				boneCount = bundle->descriptor.elementCount;
			}

			for (machine a = 1; a < combineCount; a++)
			{
				const Mesh *mesh = combineArray[a]->GetObject()->GetGeometryLevel(0);
				if (mesh->GetSkinWeightData())
				{
					bundle = mesh->GetArrayBundle(kArrayInverseBindTransform);
					if ((!bundle) || (bundle->descriptor.elementCount != boneCount))
					{
						match = false;
						break;
					}
				}
				else
				{
					match = false;
					break;
				}
			}

			if (!match)
			{
				const StringTable *table = TheWorldEditor->GetStringTable();
				DisplayError(table->GetString(StringID('ERRR', 'COMB')));
				return;
			}
		}

		List<NodeReference>		deletedList;
		Mesh					tempMesh[kMaxCombineGeometryCount];

		AddOperation(new GeometryOperation(combineArray[0]));

		for (machine a = 1; a < combineCount; a++)
		{
			deletedList.Append(new NodeReference(combineArray[a]));
		}

		DeleteOperation *operation = new DeleteOperation(&deletedList);
		operation->SetCoupledFlag(true);
		AddOperation(operation);

		const Transform4D& primaryTransform = primaryGeometry->GetInverseWorldTransform();

		tempMesh[0].CopyMesh(primaryMesh);
		for (machine a = 1; a < combineCount; a++)
		{
			Geometry *geometry = combineArray[a];
			tempMesh[a].CopyMesh(geometry->GetObject()->GetGeometryLevel(0));
			tempMesh[a].TransformMesh(primaryTransform * geometry->GetWorldTransform());
		}

		int32 primarySurfaceCount = primaryObject->GetSurfaceCount();

		for (machine a = 1; a < combineCount; a++)
		{
			Geometry *geometry = combineArray[a];

			unsigned_int16 *surfaceIndex = tempMesh[a].GetArray<unsigned_int16>(kArraySurfaceIndex);
			if (surfaceIndex)
			{
				const GeometryObject *object = geometry->GetObject();

				int32 surfaceCount = object->GetSurfaceCount();
				if (surfaceCount != 0)
				{
					unsigned_int32 *surfaceRemapTable = new unsigned_int32[surfaceCount];

					for (machine b = 0; b < surfaceCount; b++)
					{
						unsigned_int32 remap = MaxZero(Min(b, primarySurfaceCount - 1));

						const MaterialObject *materialObject = geometry->GetMaterialObject(object->GetSurfaceData(b)->materialIndex);
						if (primaryGeometry->GetMaterialObject(primaryObject->GetSurfaceData(remap)->materialIndex) != materialObject)
						{
							for (machine c = 0; c < primarySurfaceCount; c++)
							{
								if (primaryGeometry->GetMaterialObject(primaryObject->GetSurfaceData(c)->materialIndex) == materialObject)
								{
									remap = c;
									break;
								}
							}
						}

						surfaceRemapTable[b] = remap;
					}

					int32 vertexCount = tempMesh[a].GetVertexCount();
					for (machine b = 0; b < vertexCount; b++)
					{
						surfaceIndex[b] = (unsigned_int16) surfaceRemapTable[surfaceIndex[b]];
					}

					delete[] surfaceRemapTable;
				}
				else
				{
					int32 vertexCount = tempMesh[a].GetVertexCount();
					for (machine b = 0; b < vertexCount; b++)
					{
						surfaceIndex[b] = (unsigned_int16) MaxZero(Min(surfaceIndex[b], primarySurfaceCount - 1));
					}
				}
			}

			DeleteNode(geometry, true);
		}

		primaryObject->SetGeometryLevelCount(combineCount);
		for (machine a = 0; a < combineCount; a++)
		{
			primaryObject->GetGeometryLevel(a)->BuildSegmentArray(&tempMesh[a], primarySurfaceCount, primaryObject->GetSurfaceData());
		}

		InvalidateGeometry(primaryGeometry);
	}
}

void Editor::HandleSeparateDetailLevelsMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	List<NodeReference>		geometryList;

	const NodeReference *reference = GetFirstSelectedNode();
	while (reference)
	{
		Node *node = reference->GetNode();
		if (node->GetNodeType() == kNodeGeometry)
		{
			Geometry *geometry = static_cast<Geometry *>(node);
			if (geometry->GetObject()->GetGeometryLevelCount() > 1)
			{
				geometryList.Append(new NodeReference(geometry));
			}
		}

		reference = reference->Next();
	}

	if (!geometryList.Empty())
	{
		List<NodeReference>		createdList;

		AddOperation(new GeometryOperation(&geometryList));

		Buffer buffer(kPackageDefaultSize);

		reference = geometryList.First();
		while (reference)
		{
			Mesh	tempMesh;

			Geometry *primaryGeometry = static_cast<Geometry *>(reference->GetNode());
			GeometryObject *primaryObject = primaryGeometry->GetObject();
			int32 levelCount = primaryObject->GetGeometryLevelCount();
			Node *super = primaryGeometry->GetSuperNode();

			for (machine a = 1; a < levelCount; a++)
			{
				Geometry *geometry = static_cast<Geometry *>(primaryGeometry->Replicate());
				GeometryObject *object = geometry->GetObject();

				Package package(buffer, kPackageDefaultSize);
				Packer packer(&package);

				object->PackType(packer);
				object->Pack(packer, kPackEditor);

				Unpacker unpacker(package.GetStorage(), kEngineInternalVersion);
				object = static_cast<GeometryObject *>(Object::Create(unpacker, kUnpackEditor));
				object->Unpack(++unpacker, kUnpackEditor);

				geometry->SetObject(object);
				object->Release();

				tempMesh.CopyMesh(object->GetGeometryLevel(a));
				object->SetGeometryLevelCount(1);
				object->GetGeometryLevel(0)->CopyMesh(&tempMesh);
				object->BuildCollisionData();

				super->AppendSubnode(geometry);
				EditorManipulator::Install(this, geometry);
				GetManipulator(geometry)->InvalidateGraph();
				geometry->Preprocess();

				createdList.Append(new NodeReference(geometry));
			}

			tempMesh.CopyMesh(primaryObject->GetGeometryLevel(0));
			primaryObject->SetGeometryLevelCount(1);
			primaryObject->GetGeometryLevel(0)->CopyMesh(&tempMesh);
			primaryObject->BuildCollisionData();

			InvalidateGeometry(primaryGeometry);

			reference = reference->Next();
		}

		CreateOperation *operation = new CreateOperation(&createdList);
		operation->SetCoupledFlag(true);
		AddOperation(operation);
	}
}

void Editor::HandleConvertToGenericMeshMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	CreateOperation *createOperation = new CreateOperation;
	ReparentOperation *reparentOperation = new ReparentOperation;

	const NodeReference *reference = GetFirstSelectedNode();
	while (reference)
	{
		const Node *node = reference->GetNode();
		if (node->GetNodeType() == kNodeGeometry)
		{
			const Geometry *geometry = static_cast<const Geometry *>(node);
			GenericGeometry *mesh = new GenericGeometry(geometry);
			mesh->SetNodeTransform(geometry->GetNodeTransform());

			const Property *property = geometry->GetFirstProperty();
			while (property)
			{
				Property *clone = property->Clone();
				if (clone)
				{
					mesh->AddProperty(clone);
				}

				property = property->Next();
			}

			EditorManipulator::Install(this, mesh);
			geometry->GetSuperNode()->AppendNewSubnode(mesh);
			createOperation->AddNode(mesh);

			for (;;)
			{
				Node *subnode = geometry->GetFirstSubnode();
				if (!subnode)
				{
					break;
				}

				reparentOperation->AddNode(subnode);
				mesh->AppendNewSubnode(subnode);
			}

			GetManipulator(mesh)->InvalidateGraph();
		}

		reference = reference->Next();
	}

	const List<NodeReference> *createdList = createOperation->GetCreatedList();
	if (!createdList->Empty())
	{
		AddOperation(createOperation);

		reparentOperation->SetCoupledFlag(true);
		AddOperation(reparentOperation);

		UnselectNonGeometryNodes();
		HandleClearMenuItem(nullptr, nullptr);
		operationList.Last()->SetCoupledFlag(true);
	}
	else
	{
		delete reparentOperation;
		delete createOperation;
	}
}

void Editor::HandleMergeGeometryMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	Array<const Geometry *, 8>		geometryArray;

	int32 vertexCount = 0;
	const NodeReference *reference = GetFirstSelectedNode();
	while (reference)
	{
		const Node *node = reference->GetNode();
		if (node->GetNodeType() == kNodeGeometry)
		{
			const Geometry *geometry = static_cast<const Geometry *>(node);
			vertexCount += geometry->GetObject()->GetGeometryLevel(0)->GetVertexCount();
			geometryArray.AddElement(geometry);
		}

		reference = reference->Next();
	}

	int32 count = geometryArray.GetElementCount();
	if (count != 0)
	{
		if (vertexCount < 65535)
		{
			UnselectNonGeometryNodes();

			GenericGeometry *geometry = new GenericGeometry(count, geometryArray, geometryArray[0]);
			geometry->SetNodeTransform(geometryArray[0]->GetNodeTransform());

			Node *super = geometryArray[0]->GetSuperNode();
			while (super->GetManipulator()->Selected())
			{
				super = super->GetSuperNode();
			}

			HandleClearMenuItem(nullptr, nullptr);

			EditorManipulator::Install(this, geometry);
			super->AppendNewSubnode(geometry);

			CreateOperation *operation = new CreateOperation(geometry);
			operation->SetCoupledFlag(true);
			AddOperation(operation);

			GetManipulator(geometry)->InvalidateGraph();
		}
		else
		{
			const StringTable *table = TheWorldEditor->GetStringTable();
			DisplayError(table->GetString(StringID('ERRR', 'MERG')));
		}
	}
}

void Editor::HandleInvertGeometryMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	AddOperation(new GeometryOperation(GetSelectionList()));

	const NodeReference *reference = GetFirstSelectedNode();
	while (reference)
	{
		Node *node = reference->GetNode();
		if (node->GetNodeType() == kNodeGeometry)
		{
			Geometry *geometry = static_cast<Geometry *>(node);
			GeometryObject *object = geometry->GetObject();

			int32 levelCount = object->GetGeometryLevelCount();
			for (machine a = 0; a < levelCount; a++)
			{
				object->GetGeometryLevel(a)->InvertMesh();
			}

			if (object->GetGeometryType() == kGeometryPrimitive)
			{
				PrimitiveGeometryObject *primitiveObject = static_cast<PrimitiveGeometryObject *>(object);
				primitiveObject->SetPrimitiveFlags(primitiveObject->GetPrimitiveFlags() ^ kPrimitiveInvert);
			}

			InvalidateGeometry(geometry);
			static_cast<GeometryManipulator *>(GetManipulator(geometry))->UpdateSurfaceSelection();
		}

		reference = reference->Next();
	}
}

void Editor::HandleBooleanGeometryMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	const Geometry *geometry1 = nullptr;
	const Geometry *geometry2 = nullptr;

	const NodeReference *reference = GetFirstSelectedNode();
	while (reference)
	{
		const Node *node = reference->GetNode();
		if (node->GetNodeType() == kNodeGeometry)
		{
			if (!geometry1)
			{
				geometry1 = static_cast<const Geometry *>(node);
			}
			else
			{
				geometry2 = static_cast<const Geometry *>(node);
				break;
			}
		}

		reference = reference->Next();
	}

	if (geometry2)
	{
		BooleanOperation op = (menuItem == editorMenuItem[kEditorMenuIntersectGeometry]) ? kBooleanIntersection : kBooleanUnion;
		GenericGeometry *booleanGeometry = new GenericGeometry(op, geometry1, geometry2);

		if (booleanGeometry->GetObject()->GetGeometryLevel(0)->GetVertexCount() != 0)
		{
			booleanGeometry->SetNodeTransform(geometry1->GetNodeTransform());

			EditorManipulator::Install(this, booleanGeometry);
			geometry1->GetSuperNode()->AppendNewSubnode(booleanGeometry);

			GetManipulator(booleanGeometry)->InvalidateGraph();
		}
		else
		{
			delete booleanGeometry;
			booleanGeometry = nullptr;
		}

		HandleClearMenuItem(nullptr, nullptr);

		if (booleanGeometry)
		{
			CreateOperation *operation = new CreateOperation(booleanGeometry);
			operation->SetCoupledFlag(true);
			AddOperation(operation);
		}
	}
}

void Editor::HandleGenerateAmbientOcclusionMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	AddSubwindow(new GenerateAmbientOcclusionWindow(this));
}

void Editor::HandleRemoveAmbientOcclusionMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	AddOperation(new GeometryOperation(GetSelectionList()));

	const NodeReference *reference = GetFirstSelectedNode();
	while (reference)
	{
		Node *node = reference->GetNode();
		if ((node->GetNodeType() == kNodeGeometry) && (node->GetObject()->GetReferenceCount() == 1))
		{
			Geometry *geometry = static_cast<Geometry *>(node);
			const GeometryObject *object = geometry->GetObject();

			int32 levelCount = object->GetGeometryLevelCount();
			for (machine level = 0; level < levelCount; level++)
			{
				Mesh *geometryMesh = object->GetGeometryLevel(level);
				Color4C *restrict color = geometryMesh->GetArray<Color4C>(kArrayColor);
				if (color)
				{
					if (object->GetGeometryType() == kGeometryTerrain)
					{
						int32 vertexCount = geometryMesh->GetVertexCount();
						for (machine a = 0; a < vertexCount; a++)
						{
							color[a].SetAlpha(255);
						}
					}
					else
					{
						ArrayDescriptor		desc;
						Mesh				tempMesh;

						tempMesh.CopyMesh(geometryMesh);

						desc.identifier = kArrayColor;
						desc.elementCount = 0;
						desc.elementSize = 4;
						desc.componentCount = 1;

						geometryMesh->AllocateStorage(&tempMesh, 1, &desc);
						geometry->SetVertexAttributeArray(kArrayColor, 0, 0);
					}

					InvalidateGeometry(geometry);
					static_cast<GeometryManipulator *>(GetManipulator(geometry))->UpdateSurfaceSelection();
				}
			}
		}

		reference = reference->Next();
	}
}

void Editor::HandleHideSelectionMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	const NodeReference *reference = GetFirstSelectedNode();
	while (reference)
	{
		const NodeReference *next = reference->Next();
		HideNode(reference->GetNode());
		reference = next;
	}
}

void Editor::HandleUnhideAllMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	Node *root = GetRootNode();

	Node *node = root->GetNextNode(root);
	while (node)
	{
		if (!(node->GetNodeFlags() & kNodeNonpersistent))
		{
			ShowNode(node);
			node = root->GetNextNode(node);
		}
		else
		{
			node = root->GetNextLevelNode(node);
		}
	}
}

void Editor::HandleUnhideAllInTargetZoneMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	Zone *zone = GetTargetZone();

	Node *node = zone->GetFirstSubnode();
	while (node)
	{
		if (!(node->GetNodeFlags() & kNodeNonpersistent))
		{
			if (node->GetOwningZone() == zone)
			{
				ShowNode(node);
			}

			node = zone->GetNextNode(node);
		}
		else
		{
			node = zone->GetNextLevelNode(node);
		}
	}
}

void Editor::HandleHideNonTargetZonesMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	Zone *zone = GetTargetZone();
	Node *root = GetRootNode();

	Node *node = root->GetNextNode(root);
	while (node)
	{
		if ((node->GetOwningZone() != zone) && (node != zone))
		{
			HideNode(node);
		}

		node = root->GetNextNode(node);
	}
}

void Editor::HandleShowBackfacesMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	editorState |= kEditorRedrawViewports;

	unsigned_int32 editorFlags = editorObject->GetEditorFlags() ^ kEditorShowBackfaces;
	editorObject->SetEditorFlags(editorFlags);

	if (editorFlags & kEditorShowBackfaces)
	{
		flagButton[kEditorFlagBackfaces]->SetValue(1);
		static_cast<MenuItemWidget *>(menuItem)->ShowBullet();
	}
	else
	{
		flagButton[kEditorFlagBackfaces]->SetValue(0);
		static_cast<MenuItemWidget *>(menuItem)->HideBullet();
	}
}

void Editor::HandleExpandWorldsMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	unsigned_int32 editorFlags = editorObject->GetEditorFlags() ^ kEditorExpandWorlds;
	editorObject->SetEditorFlags(editorFlags);

	if (editorFlags & kEditorExpandWorlds)
	{
		flagButton[kEditorFlagExpandWorlds]->SetValue(1);
		static_cast<MenuItemWidget *>(menuItem)->ShowBullet();
		ExpandAllWorlds();
	}
	else
	{
		flagButton[kEditorFlagExpandWorlds]->SetValue(0);
		static_cast<MenuItemWidget *>(menuItem)->HideBullet();
		CollapseAllWorlds();
	}
}

void Editor::HandleExpandModelsMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	unsigned_int32 editorFlags = editorObject->GetEditorFlags() ^ kEditorExpandModels;
	editorObject->SetEditorFlags(editorFlags);

	if (editorFlags & kEditorExpandModels)
	{
		flagButton[kEditorFlagExpandModels]->SetValue(1);
		static_cast<MenuItemWidget *>(menuItem)->ShowBullet();
		ExpandAllModels();
	}
	else
	{
		flagButton[kEditorFlagExpandModels]->SetValue(0);
		static_cast<MenuItemWidget *>(menuItem)->HideBullet();
		CollapseAllModels();
	}
}

void Editor::HandleRenderLightingMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	editorState |= kEditorRedrawViewports;

	unsigned_int32 editorFlags = editorObject->GetEditorFlags() ^ kEditorRenderLighting;
	editorObject->SetEditorFlags(editorFlags);

	if (editorFlags & kEditorRenderLighting)
	{
		flagButton[kEditorFlagLighting]->SetValue(1);
		static_cast<MenuItemWidget *>(menuItem)->ShowBullet();
	}
	else
	{
		flagButton[kEditorFlagLighting]->SetValue(0);
		static_cast<MenuItemWidget *>(menuItem)->HideBullet();
	}

	InvalidateAllShaderData();
}

void Editor::HandleDrawFromCenterMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	unsigned_int32 editorFlags = editorObject->GetEditorFlags() ^ kEditorDrawFromCenter;
	editorObject->SetEditorFlags(editorFlags);

	if (editorFlags & kEditorDrawFromCenter)
	{
		flagButton[kEditorFlagCenter]->SetValue(1);
		static_cast<MenuItemWidget *>(menuItem)->ShowBullet();
	}
	else
	{
		flagButton[kEditorFlagCenter]->SetValue(0);
		static_cast<MenuItemWidget *>(menuItem)->HideBullet();
	}
}

void Editor::HandleCapGeometryMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	unsigned_int32 editorFlags = editorObject->GetEditorFlags() ^ kEditorCapGeometry;
	editorObject->SetEditorFlags(editorFlags);

	if (editorFlags & kEditorCapGeometry)
	{
		flagButton[kEditorFlagCap]->SetValue(1);
		static_cast<MenuItemWidget *>(menuItem)->ShowBullet();
	}
	else
	{
		flagButton[kEditorFlagCap]->SetValue(0);
		static_cast<MenuItemWidget *>(menuItem)->HideBullet();
	}
}

void Editor::HandleShowViewportMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	for (machine a = 0; a < kEditorViewportCount; a++)
	{
		if (menuItem == showViewportItem[a])
		{
			ShowViewport(a);
			break;
		}
	}
}

void Editor::HandleChangeViewportMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	int32 index = viewportMenuIndex;

	int32 selection = static_cast<MenuItemWidget *>(menuItem)->ListElement<MenuItemWidget>::GetListIndex();
	if (selection != editorObject->GetViewportMode(index))
	{
		editorObject->SetViewportMode(index, selection);
		BuildViewport(index);
		UpdateViewportStructures();

		if (GetFirstSelectedNode())
		{
			HandleFrameSelectionMenuItem(nullptr, nullptr);
		}
	}
}

void Editor::HandleFrameAllMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	EditorViewport *viewport = editorViewport[viewportMenuIndex];
	ViewportWidget *viewportWidget = viewport->GetViewportWidget();
	viewport->Invalidate();

	EditorViewportType viewportType = viewport->GetEditorViewportType();
	if (viewportType == kEditorViewportGraph)
	{
		const EditorManipulator *manipulator = GetManipulator(GetRootNode());
		float width = manipulator->GetGraphWidth();
		float height = manipulator->GetGraphHeight();

		OrthoViewportWidget *orthoViewportWidget = static_cast<OrthoViewportWidget *>(viewportWidget);
		orthoViewportWidget->GetViewportCamera()->SetNodePosition(Point3D(PositiveFloor(width * 0.5F), PositiveFloor(height * 0.5F), 0.0F));

		float scale = Fmax((width + 32.0F) / orthoViewportWidget->GetWidgetSize().x, (height + 32.0F) / orthoViewportWidget->GetWidgetSize().y, 1.0F);
		orthoViewportWidget->SetOrthoScale(Vector2D(scale, scale));
	}
	else
	{
		const BoundingSphere *sphere = GetManipulator(GetRootNode())->GetTreeSphere();
		if (sphere)
		{
			const Point3D& center = sphere->GetCenter();
			float radius = sphere->GetRadius();

			if (viewportType == kEditorViewportOrtho)
			{
				OrthoViewportWidget *orthoViewportWidget = static_cast<OrthoViewportWidget *>(viewportWidget);
				OrthoCamera *camera = orthoViewportWidget->GetViewportCamera();

				camera->SetNodePosition(center - ProjectOnto(center, camera->GetNodeTransform()[2]));
				float scale = Fmax(radius / orthoViewportWidget->GetWidgetSize().x, radius / orthoViewportWidget->GetWidgetSize().y) * 2.0F;
				orthoViewportWidget->SetOrthoScale(Vector2D(scale, scale));
			}
			else if (viewportType == kEditorViewportFrustum)
			{
				FrustumViewportWidget *frustumViewportWidget = static_cast<FrustumViewportWidget *>(viewportWidget);
				FrustumCamera *camera = frustumViewportWidget->GetViewportCamera();

				float focal = camera->GetObject()->GetFocalLength();
				camera->SetNodePosition(center - camera->GetNodeTransform()[2] * (focal * radius));
			}
		}
	}
}

void Editor::HandleFrameSelectionMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	EditorViewport *viewport = editorViewport[viewportMenuIndex];
	ViewportWidget *viewportWidget = viewport->GetViewportWidget();
	viewport->Invalidate();

	EditorViewportType viewportType = viewport->GetEditorViewportType();
	if (viewportType == kEditorViewportGraph)
	{
		const NodeReference *reference = GetFirstSelectedNode();
		if (reference)
		{
			do
			{
				const Node *super = reference->GetNode()->GetSuperNode();
				while (super)
				{
					GetManipulator(super)->ExpandSubgraph();
					super = super->GetSuperNode();
				}

				reference = reference->Next();
			} while (reference);

			GetManipulator(GetRootNode())->UpdateGraph();

			reference = GetFirstSelectedNode();
			const EditorManipulator *manipulator = GetManipulator(reference->GetNode());
			const Point3D *position = &manipulator->GetGraphPosition();
			float xmin = position->x;
			float ymin = position->y;
			float xmax = xmin + kGraphBoxWidth;
			float ymax = ymin + kGraphBoxHeight;

			for (;;)
			{
				reference = reference->Next();
				if (!reference)
				{
					break;
				}

				manipulator = GetManipulator(reference->GetNode());
				position = &manipulator->GetGraphPosition();
				xmin = Fmin(xmin, position->x);
				ymin = Fmin(ymin, position->y);
				xmax = Fmax(xmax, position->x + kGraphBoxWidth);
				ymax = Fmax(ymax, position->y + kGraphBoxHeight);
			}

			OrthoViewportWidget *orthoViewportWidget = static_cast<OrthoViewportWidget *>(viewportWidget);
			orthoViewportWidget->GetViewportCamera()->SetNodePosition(Point3D(Floor((xmin + xmax) * 0.5F), Floor((ymin + ymax) * 0.5F), 0.0F));

			float scale = Fmax((xmax - xmin + 32.0F) / orthoViewportWidget->GetWidgetSize().x, (ymax - ymin + 32.0F) / orthoViewportWidget->GetWidgetSize().y, 1.0F);
			orthoViewportWidget->SetOrthoScale(Vector2D(scale, scale));
		}
	}
	else
	{
		const NodeReference *reference = GetFirstSelectedNode();
		if (reference)
		{
			Box3D box = GetManipulator(reference->GetNode())->CalculateWorldBoundingBox();
			for (;;)
			{
				reference = reference->Next();
				if (!reference)
				{
					break;
				}

				box.Union(GetManipulator(reference->GetNode())->CalculateWorldBoundingBox());
			}

			EditorManipulator::AdjustBoundingBox(&box);

			Point3D center = box.GetCenter();
			Vector3D size = box.GetSize();

			if (viewportType == kEditorViewportOrtho)
			{
				OrthoViewportWidget *orthoViewportWidget = static_cast<OrthoViewportWidget *>(viewportWidget);
				OrthoCamera *camera = orthoViewportWidget->GetViewportCamera();

				camera->SetNodePosition(center - ProjectOnto(center, camera->GetNodeTransform()[2]));

				float x = Magnitude(ProjectOnto(size, camera->GetNodeTransform()[0]));
				float y = Magnitude(ProjectOnto(size, camera->GetNodeTransform()[1]));

				float scale = Fmax(x / orthoViewportWidget->GetWidgetSize().x, y / orthoViewportWidget->GetWidgetSize().y);
				orthoViewportWidget->SetOrthoScale(Vector2D(scale, scale));
			}
			else if (viewportType == kEditorViewportFrustum)
			{
				FrustumViewportWidget *frustumViewportWidget = static_cast<FrustumViewportWidget *>(viewportWidget);
				FrustumCamera *camera = frustumViewportWidget->GetViewportCamera();

				float x = Magnitude(ProjectOnto(size, camera->GetNodeTransform()[0]));
				float y = Magnitude(ProjectOnto(size, camera->GetNodeTransform()[1]));

				float focal = camera->GetObject()->GetFocalLength();
				camera->SetNodePosition(center - camera->GetNodeTransform()[2] * (focal * Fmax(x, y) * 0.5F));
			}
		}
	}
}

void Editor::SavePickerProc(FilePicker *picker, void *cookie)
{
	ResourceName name(picker->GetFileName());
	name[Text::GetResourceNameLength(name)] = 0;
	int32 len = Text::GetPrefixDirectoryLength(name);

	Editor *editor = static_cast<Editor *>(cookie);
	editor->resourceName = &name[len];
	editor->resourceLocation.GetPath().Set(name, len - 1);
	editor->SaveWorld(static_cast<WorldSavePicker *>(picker)->GetStripFlag());

	ResourcePath title(editor->resourceName);
	editor->SetWindowTitle(title += WorldResource::GetDescriptor()->GetExtension());
	editor->SetStripTitle(&title[Text::GetDirectoryPathLength(title)]);
}

void Editor::SceneImportPickerProc(FilePicker *picker, void *cookie)
{
	GeometryImportData		importData;

	const SceneImportPicker *importPicker = static_cast<SceneImportPicker *>(picker);
	SceneImportPlugin *importPlugin = importPicker->GetSceneImportPlugin();

	TheEngine->GetVariable("sceneImportType")->SetValue(&importPlugin->GetImportResourceDescriptor(kSceneImportGeometry)->GetExtension()[1]);
	ConfigDataDescription::WriteEngineConfig();

	ResourceName name(importPicker->GetFileName());
	name[Text::GetResourceNameLength(name)] = 0;

	unsigned_int32 flags = 0;

	if (importPicker->GetImportTexturesFlag())
	{
		flags |= kGeometryImportTextures;
	}

	if (importPicker->GetMergeMaterialsFlag())
	{
		flags |= kGeometryImportMergeMaterials;
	}

	if (importPicker->GetReuseNamedMaterialsFlag())
	{
		flags |= kGeometryImportReuseNamedMaterials;
	}

	importData.importFlags = flags;

	float scale = importPicker->GetSceneScale();
	if (scale < K::min_float)
	{
		scale = 1.0F;
	}

	importData.importScale = scale;

	importPlugin->ImportGeometry(static_cast<Editor *>(cookie), name, &importData);
}

void Editor::SceneExportPickerProc(FilePicker *picker, void *cookie)
{
	const SceneExportPicker *exportPicker = static_cast<SceneExportPicker *>(picker);
	SceneExportPlugin *exportPlugin = exportPicker->GetSceneExportPlugin();

	TheEngine->GetVariable("sceneExportType")->SetValue(&exportPlugin->GetExportResourceDescriptor()->GetExtension()[1]);
	ConfigDataDescription::WriteEngineConfig();

	ResourceName name(exportPicker->GetFileName());
	name[Text::GetResourceNameLength(name)] = 0;

	exportPlugin->ExportScene(name, static_cast<Editor *>(cookie)->GetEditorWorld());
}

void Editor::ModelLoadPickerProc(FilePicker *picker, void *cookie)
{
	Editor *editor = static_cast<Editor *>(cookie);
	editor->UnselectAll();

	Model *model = Model::New(picker->GetResourceName(), kModelUnknown, kUnpackEditor);
	if (model)
	{
		Node *node = model->GetFirstSubnode();
		while (node)
		{
			node->SetNodeFlags(node->GetNodeFlags() & ~kNodeNonpersistent);
			node = model->GetNextNode(node);
		}

		Zone *zone = editor->GetTargetZone();

		node = model->GetFirstSubnode();
		Node *subnode = node;
		while (subnode)
		{
			Node *next = subnode->Next();

			EditorManipulator::Install(editor, subnode);
			zone->AppendSubnode(subnode);
			GetManipulator(subnode)->InvalidateGraph();

			subnode = next;
		}

		delete model;
		editor->editorState |= kEditorWorldUnsaved | kEditorRedrawViewports;

		while (node)
		{
			node->Preprocess();
			node = node->Next();
		}
	}
}

void Editor::ModelSavePickerProc(FilePicker *picker, void *cookie)
{
	File			file;
	ResourcePath	path;

	Editor *editor = static_cast<Editor *>(cookie);

	Node *root = editor->GetRootNode();
	root->InvalidateNodeIndex();

	bool newModelFlag = false;
	Model *model = nullptr;

	Node *node = root->GetFirstSubnode();
	while (node)
	{
		if (node->GetNodeType() == kNodeModel)
		{
			model = static_cast<Model *>(node);
			break;
		}

		node = node->Next();
	}

	if (!model)
	{
		newModelFlag = true;
		model = new Model;

		for (;;)
		{
			node = root->GetFirstSubnode();
			if (!node)
			{
				break;
			}

			model->AppendSubnode(node);
		}
	}

	node = model;
	for (;;)
	{
		node = model->GetNextNode(node);
		if (!node)
		{
			break;
		}

		if (node->GetNodeType() == kNodeGeometry)
		{
			GeometryObject *object = static_cast<Geometry *>(node)->GetObject();
			object->SetGeometryFlags(object->GetGeometryFlags() | kGeometryModelExportFlags);
		}
	}

	ResourceName name(picker->GetFileName());
	name[Text::GetResourceNameLength(name)] = 0;

	TheResourceMgr->GetGenericCatalog()->GetResourcePath(ModelResource::GetDescriptor(), name, &path);
	TheResourceMgr->CreateDirectoryPath(path);

	if (file.Open(path, kFileCreate) == kFileOkay)
	{
		model->PackTree(&file, kPackInitialize);
		file.Close();
	}

	int32 start = Text::GetPrefixDirectoryLength(name);
	ModelRegistration *registration = Model::GetFirstRegistration();
	while (registration)
	{
		if (Text::CompareText(&name[start], registration->GetResourceName()))
		{
			registration->Reload();
			break;
		}

		registration = registration->Next();
	}

	if (newModelFlag)
	{
		for (;;)
		{
			node = model->GetFirstSubnode();
			if (!node)
			{
				break;
			}

			root->AppendSubnode(node);
		}

		delete model;
	}
}


MeshOriginWindow::MeshOriginWindow(Editor *editor) : Window("WorldEditor/MeshOrigin")
{
	worldEditor = editor;
}

MeshOriginWindow::~MeshOriginWindow()
{
}

void MeshOriginWindow::Preprocess(void)
{
	Window::Preprocess();

	okayButton = static_cast<PushButtonWidget *>(FindWidget("OK"));
	cancelButton = static_cast<PushButtonWidget *>(FindWidget("Cancel"));

	unsigned_int32 settings = worldEditor->GetEditorObject()->GetMeshOriginSettings();

	radioButton[0][0] = static_cast<RadioWidget *>(FindWidget("Xmin"));
	radioButton[0][1] = static_cast<RadioWidget *>(FindWidget("Xcen"));
	radioButton[0][2] = static_cast<RadioWidget *>(FindWidget("Xmax"));
	radioButton[0][settings & 0xFF]->SetValue(1);

	radioButton[1][0] = static_cast<RadioWidget *>(FindWidget("Ymin"));
	radioButton[1][1] = static_cast<RadioWidget *>(FindWidget("Ycen"));
	radioButton[1][2] = static_cast<RadioWidget *>(FindWidget("Ymax"));
	radioButton[1][(settings >> 8) & 0xFF]->SetValue(1);

	radioButton[2][0] = static_cast<RadioWidget *>(FindWidget("Zmin"));
	radioButton[2][1] = static_cast<RadioWidget *>(FindWidget("Zcen"));
	radioButton[2][2] = static_cast<RadioWidget *>(FindWidget("Zmax"));
	radioButton[2][(settings >> 16) & 0xFF]->SetValue(1);
}

void MeshOriginWindow::CommitSettings(void) const
{
	unsigned_int32 settings = 0;
	for (machine a = 0; a < 3; a++)
	{
		for (machine b = 0; b < 3; b++)
		{
			if (radioButton[a][b]->GetValue() != 0)
			{
				settings |= b << (a * 8);
				break;
			}
		}
	}

	Editor *editor = worldEditor;
	editor->GetEditorObject()->SetMeshOriginSettings(settings);

	auto filter = [](const Geometry *geometry) -> bool
	{
		return (geometry->GetGeometryType() == kGeometryGeneric);
	};

	editor->AddOperation(new GeometryOperation(editor->GetSelectionList(), filter));

	const NodeReference *reference = editor->GetFirstSelectedNode();
	while (reference)
	{
		Node *node = reference->GetNode();

		if (node->GetNodeType() == kNodeGeometry)
		{
			Geometry *geometry = static_cast<Geometry *>(node);
			if (geometry->GetGeometryType() == kGeometryGeneric)
			{
				GenericGeometry *genericGeometry = static_cast<GenericGeometry *>(geometry);
				GenericGeometryObject *object = genericGeometry->GetObject();

				const Box3D& bounds = object->GetBoundingBox();

				unsigned_int32 i = settings & 0xFF;
				unsigned_int32 j = (settings >> 8) & 0xFF;
				unsigned_int32 k = (settings >> 16) & 0xFF;
				float x = (i == kMeshOriginMin) ? bounds.min.x : ((i == kMeshOriginMax) ? bounds.max.x : (bounds.min.x + bounds.max.x) * 0.5F);
				float y = (j == kMeshOriginMin) ? bounds.min.y : ((j == kMeshOriginMax) ? bounds.max.y : (bounds.min.y + bounds.max.y) * 0.5F);
				float z = (k == kMeshOriginMin) ? bounds.min.z : ((k == kMeshOriginMax) ? bounds.max.z : (bounds.min.z + bounds.max.z) * 0.5F);

				Vector3D dp(-x, -y, -z);
				genericGeometry->SetNodePosition(genericGeometry->GetNodePosition() - dp);

				int32 levelCount = object->GetGeometryLevelCount();
				for (machine a = 0; a < levelCount; a++)
				{
					Mesh *mesh = object->GetGeometryLevel(a);
					mesh->TranslateMesh(dp);
				}

				object->UpdateBounds();
				object->BuildCollisionData();

				editor->InvalidateGeometry(genericGeometry);
			}
		}

		reference = reference->Next();
	}
}

void MeshOriginWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		if (widget == okayButton)
		{
			CommitSettings();
			Close();
		}
		else if (widget == cancelButton)
		{
			Close();
		}
	}
}


GenerateAmbientOcclusionWindow::GenerateAmbientOcclusionWindow(Editor *editor) : Window("WorldEditor/AmbientOcclusion")
{
	worldEditor = editor;

	jobCount = 0;
	jobTable = nullptr;
}

GenerateAmbientOcclusionWindow::~GenerateAmbientOcclusionWindow()
{
	int32 count = jobCount;
	if (count > 0)
	{
		TheJobMgr->CancelJobArray(count, jobTable);
		for (machine a = count - 1; a >= 0; a--)
		{
			delete jobTable[a];
		}

		delete[] jobTable;
	}

	const NodeReference *reference = worldEditor->GetFirstSelectedNode();
	while (reference)
	{
		Node *node = reference->GetNode();
		if ((node->GetNodeType() == kNodeGeometry) && (node->GetObject()->GetReferenceCount() == 1))
		{
			Geometry *geometry = static_cast<Geometry *>(node);
			worldEditor->InvalidateGeometry(geometry);
		}

		reference = reference->Next();
	}
}

GenerateAmbientOcclusionWindow::AmbientOcclusionJob::AmbientOcclusionJob(GenerateAmbientOcclusionWindow *window, ExecuteProc *execProc, void *cookie) : Job(execProc, cookie)
{
	jobWindow = window;
}

void GenerateAmbientOcclusionWindow::Preprocess(void)
{
	Window::Preprocess();

	startButton = static_cast<PushButtonWidget *>(FindWidget("Start"));
	cancelButton = static_cast<PushButtonWidget *>(FindWidget("Cancel"));
	intensityBox = static_cast<EditTextWidget *>(FindWidget("Intensity"));
	inputText = static_cast<TextWidget *>(FindWidget("Input"));

	stopButton = static_cast<PushButtonWidget *>(FindWidget("Stop"));
	progressBar = static_cast<ProgressWidget *>(FindWidget("Progress"));
	borderWidget = static_cast<BorderWidget *>(FindWidget("Border"));
	messageText = static_cast<TextWidget *>(FindWidget("Message"));

	SetFocusWidget(intensityBox);
}

void GenerateAmbientOcclusionWindow::Move(void)
{
	Window::Move();

	int32 count = jobCount;
	if (count > 0)
	{
		int32 progress = 0;
		for (machine a = 0; a < count; a++)
		{
			progress += jobTable[a]->Complete();
		}

		progressBar->SetValue(progress);

		if (progress == count)
		{
			Close();
		}
	}
}

void GenerateAmbientOcclusionWindow::StartJob(void)
{
	blockageMultiplier = Text::StringToFloat(intensityBox->GetText()) * 12.055888F;		// 12.055888F = 255.0F / normalization

	startButton->Hide();
	cancelButton->Hide();
	intensityBox->Hide();
	inputText->Hide();
	stopButton->Show();
	progressBar->Show();
	borderWidget->Show();
	messageText->Show();

	worldEditor->AddOperation(new GeometryOperation(worldEditor->GetSelectionList()));

	int32 count = 0;
	const NodeReference *reference = worldEditor->GetFirstSelectedNode();
	while (reference)
	{
		const Node *node = reference->GetNode();
		if ((node->GetNodeType() == kNodeGeometry) && (node->GetObject()->GetReferenceCount() == 1))
		{
			count++;
		}

		reference = reference->Next();
	}

	jobCount = count;
	if (count > 0)
	{
		jobTable = new Job *[count];

		count = 0;
		reference = worldEditor->GetFirstSelectedNode();
		while (reference)
		{
			Node *node = reference->GetNode();
			if ((node->GetNodeType() == kNodeGeometry) && (node->GetObject()->GetReferenceCount() == 1))
			{
				Geometry *geometry = static_cast<Geometry *>(node);
				Job *job = new AmbientOcclusionJob(this, &JobGenerateAmbientOcclusion, geometry);
				jobTable[count++] = job;
				TheJobMgr->SubmitJob(job);
			}

			reference = reference->Next();
		}
	}

	progressBar->SetMaxValue(count);
}

void GenerateAmbientOcclusionWindow::JobGenerateAmbientOcclusion(Job *job, void *cookie)
{
	static const float directionTable[35][3] =
	{
		{0.9238795F, 0.0F, 0.3826833F},
		{0.8535532F, 0.3535534F, 0.3826833F},
		{0.6532814F, 0.6532814F, 0.3826833F},
		{0.3535532F, 0.8535532F, 0.3826833F},
		{0.0F, 0.9238795F, 0.3826833F},
		{-0.3535534F, 0.8535532F, 0.3826833F},
		{-0.6532814F, 0.6532814F, 0.3826833F},
		{-0.8535534F, 0.3535531F, 0.3826833F},
		{-0.9238795F, 0.0F, 0.3826833F},
		{-0.8535532F, -0.3535532F, 0.3826833F},
		{-0.6532813F, -0.6532815F, 0.3826833F},
		{-0.353553F, -0.8535534F, 0.3826833F},
		{0.0F, -0.9238795F, 0.3826833F},
		{0.3535535F, -0.8535532F, 0.3826833F},
		{0.6532816F, -0.6532812F, 0.3826833F},
		{0.8535534F, -0.3535532F, 0.3826833F},
		{0.7071067F, 0.0F, 0.7071067F},
		{0.6123723F, 0.3535532F, 0.7071067F},
		{0.3535532F, 0.6123723F, 0.7071067F},
		{0.0F, 0.7071067F, 0.7071067F},
		{-0.3535534F, 0.6123723F, 0.7071067F},
		{-0.6123723F, 0.3535534F, 0.7071067F},
		{-0.7071067F, 0.0F, 0.7071067F},
		{-0.6123722F, -0.3535534F, 0.7071067F},
		{-0.3535532F, -0.6123723F, 0.7071067F},
		{0.0F, -0.7071067F, 0.7071067F},
		{0.3535532F, -0.6123723F, 0.7071067F},
		{0.6123725F, -0.3535531F, 0.7071067F},
		{0.3826833F, 0.0F, 0.9238795F},
		{0.1913416F, 0.3314135F, 0.9238795F},
		{-0.1913416F, 0.3314135F, 0.9238795F},
		{-0.3826833F, 0.0F, 0.9238795F},
		{-0.1913416F, -0.3314135F, 0.9238795F},
		{0.1913416F, -0.3314135F, 0.9238795F},
		{0.0F, 0.0F, 1.0F}
	};

	GenerateAmbientOcclusionWindow *window = static_cast<AmbientOcclusionJob *>(job)->GetJobWindow();
	const Node *rootNode = window->worldEditor->GetRootNode();
	const World *world = rootNode->GetWorld();
	int32 threadIndex = job->GetThreadIndex();
	float multiplier = window->blockageMultiplier;

	const Geometry *geometry = static_cast<Geometry *>(cookie);
	const GeometryObject *object = geometry->GetObject();

	int32 levelCount = object->GetGeometryLevelCount();
	for (machine level = 0; level < levelCount; level++)
	{
		Mesh *geometryMesh = object->GetGeometryLevel(level);
		int32 vertexCount = geometryMesh->GetVertexCount();

		Color4C *restrict color = geometryMesh->GetArray<Color4C>(kArrayColor);
		if (!color)
		{
			ArrayDescriptor		desc;
			Mesh				tempMesh;

			window->jobLock.AcquireExclusive();

			tempMesh.CopyMesh(geometryMesh);

			desc.identifier = kArrayColor;
			desc.elementCount = vertexCount;
			desc.elementSize = 4;
			desc.componentCount = 1;

			geometryMesh->AllocateStorage(&tempMesh, 1, &desc);
			color = geometryMesh->GetArray<Color4C>(kArrayColor);
			MemoryMgr::ClearMemory(color, vertexCount * sizeof(Color4C));

			window->jobLock.ReleaseExclusive();
		}

		window->jobLock.AcquireShared();

		const Point3D *position = geometryMesh->GetArray<Point3D>(kArrayPosition);
		const Vector3D *normal = geometryMesh->GetArray<Vector3D>(kArrayNormal);

		float radius = 12.0F;

		for (machine a = 0; a < vertexCount; a++)
		{
			Vector3D nrml = normal[a] * geometry->GetInverseWorldTransform();
			Vector3D tang = Math::CreateUnitPerpendicular(nrml);
			Vector3D btng = nrml % tang;

			Point3D p1 = geometry->GetWorldTransform() * position[a] + nrml * 0.03125F;
			float blockage = 0.0F;

			for (machine b = 0; b < 35; b++)
			{
				CollisionData	collisionData;

				const float *d = directionTable[b];
				Point3D p2 = p1 + (tang * d[0] + btng * d[1] + nrml * d[2]) * radius;

				if (world->DetectCollision(p1, p2, 0.0F, kCollisionProjectile, &collisionData, threadIndex))
				{
					float t = collisionData.param * collisionData.param;
					float m = (1.0F - t * t) * d[2];
					blockage += m;
				}
			}

			color[a].SetAlpha(Min(MaxZero((int32) (255.0F - blockage * multiplier)), 255));
		}

		window->jobLock.ReleaseShared();
	}
}

bool GenerateAmbientOcclusionWindow::HandleKeyboardEvent(const KeyboardEventData *eventData)
{
	if (eventData->eventType == kEventKeyDown)
	{
		if (eventData->keyCode == kKeyCodeEscape)
		{
			if (cancelButton->Visible())
			{
				cancelButton->Activate();
			}
			else
			{
				stopButton->Activate();
			}

			return (true);
		}
	}

	return (Window::HandleKeyboardEvent(eventData));
}

void GenerateAmbientOcclusionWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		if (widget == startButton)
		{
			StartJob();
		}
		else if (widget == cancelButton)
		{
			Close();
		}
		else if (widget == stopButton)
		{
			Close();
		}
	}
}


GenerateRadiosityDataWindow::GenerateRadiosityDataWindow(Editor *editor) : Window("WorldEditor/RadiositySpace")
{
	worldEditor = editor;

	const NodeReference *reference = editor->GetFirstSelectedNode();
	while (reference)
	{
		const Node *node = reference->GetNode();
		if (node->GetNodeType() == kNodeSpace)
		{
			const Space *space = static_cast<const Space *>(node);
			if (space->GetSpaceType() == kSpaceRadiosity)
			{
				spaceArray.AddElement(static_cast<const RadiositySpace *>(space));
			}
		}

		reference = reference->Next();
	}

	imageBuffer[0] = nullptr;
	valueData = nullptr;
	colorData = nullptr;
}

GenerateRadiosityDataWindow::~GenerateRadiosityDataWindow()
{
	delete[] colorData;
	delete[] valueData;
	delete[] imageBuffer[0];

	worldEditor->InvalidateAllShaderData();
}

void GenerateRadiosityDataWindow::Preprocess(void)
{
	Window::Preprocess();

	stopButton = static_cast<PushButtonWidget *>(FindWidget("Stop"));
	progressBar = static_cast<ProgressWidget *>(FindWidget("Progress"));
	countText = static_cast<TextWidget *>(FindWidget("Count"));

	worldEditor->InvalidateAllShaderData();

	nearDistance = 0.015625F;
	cameraOffset = nearDistance * 2.0F;

	spaceIndex = 0;
	BeginRadiositySpace();
}

void GenerateRadiosityDataWindow::BeginRadiositySpace(void)
{
	countText->SetText((String<15>(spaceIndex + 1) += " / ") += spaceArray.GetElementCount());

	const RadiositySpace *space = spaceArray[spaceIndex];
	const RadiositySpaceObject *object = space->GetObject();
	object->ClearTextureData();

	const Integer3D& radiosityResolution = object->GetRadiosityResolution();
	imageVoxelCount = radiosityResolution.x * radiosityResolution.y * radiosityResolution.z;

	sampleCoord.Set(0, 0, 0);

	const Vector3D& spaceSize = object->GetBoxSize();
	sampleDelta.x = spaceSize.x / (float) (radiosityResolution.x - 1);
	sampleDelta.y = spaceSize.y / (float) (radiosityResolution.y - 1);
	sampleDelta.z = spaceSize.z / (float) (radiosityResolution.z - 1);

	imageBuffer[0] = new Color4C[kRenderArea * 6];
	for (machine a = 1; a < 6; a++)
	{
		imageBuffer[a] = imageBuffer[a - 1] + kRenderArea;
	}

	valueData = new unsigned_int32[imageVoxelCount * 6];
	colorData = new Color2C[imageVoxelCount];

	cubeCamera.Preprocess();
}

void GenerateRadiosityDataWindow::EndRadiositySpace(void)
{
	const RadiositySpace *space = spaceArray[spaceIndex];
	const RadiositySpaceObject *object = space->GetObject();

	Color4C *image1 = object->GetRadiosityImage(0);
	Color4C *image2 = object->GetRadiosityImage(1);

	float multiplier = object->GetIntensityScale() / (float) kRenderArea;

	const unsigned_int32 *value = valueData;
	const Color2C *color = colorData;

	int32 voxelCount = imageVoxelCount;
	for (machine a = 0; a < voxelCount; a++)
	{
		float xpos = (float) value[0] * multiplier;
		float xneg = (float) value[1] * multiplier;
		float ypos = (float) value[2] * multiplier;
		float yneg = (float) value[3] * multiplier;
		float zpos = (float) value[4] * multiplier;
		float zneg = (float) value[5] * multiplier;

		unsigned_int32 red1 = Min((int32) xpos, 255);
		unsigned_int32 green1 = Min((int32) ypos, 255);
		unsigned_int32 blue1 = Min((int32) zpos, 255);

		unsigned_int32 red2 = Min((int32) xneg, 255);
		unsigned_int32 green2 = Min((int32) yneg, 255);
		unsigned_int32 blue2 = Min((int32) zneg, 255);

		image1[a].Set(red1, green1, blue1, color->GetRed());
		image2[a].Set(red2, green2, blue2, color->GetGreen());

		value += 6;
		color++;
	}

	delete[] colorData;
	colorData = nullptr;

	delete[] valueData;
	valueData = nullptr;

	delete[] imageBuffer[0];
	imageBuffer[0] = nullptr;

	object->UpdateTextureData();
}

bool GenerateRadiosityDataWindow::DetectInteriorCollision(const World *world, const Point3D& p1, const Point3D& p2, CollisionData *data)
{
	if ((world->DetectCollision(p1, p2, 0.0F, kCollisionCamera, data)) && (!(data->geometry->GetPerspectiveExclusionMask() & kPerspectiveRadiositySpace)))
	{
		return (true);
	}

	return (false);
}

void GenerateRadiosityDataWindow::Move(void)
{
	CollisionData	collisionData;

	Window::Move();

	World *world = worldEditor->GetEditorWorld();
	unsigned_int32 worldFlags = world->GetWorldFlags();
	int32 worldRenderWidth = world->GetRenderWidth();
	int32 worldRenderHeight = world->GetRenderHeight();
	FrustumCamera *worldCamera = world->GetCamera();

	world->SetWorldFlags((worldFlags & ~kWorldViewport) | (kWorldMotionBlurInhibit | kWorldListenerInhibit));
	world->SetWorldPerspective(kPerspectiveRadiositySpace);
	world->SetRenderSize(kRenderSize, kRenderSize);
	world->SetCamera(&cubeCamera);

	FrustumCameraObject *cameraObject = cubeCamera.GetObject();
	cameraObject->SetNearDepth(nearDistance);
	cameraObject->SetClearFlags(kClearColorBuffer | kClearDepthStencilBuffer);
	cameraObject->SetClearColor(K::black);

	unsigned_int32 renderOptions = TheGraphicsMgr->GetRenderOptionFlags();
	TheGraphicsMgr->SetRenderOptionFlags(renderOptions & ~(kRenderOptionMotionBlur | kRenderOptionDistortion));
	TheGraphicsMgr->SetAmbientMode(kAmbientDark);

	const RadiositySpace *space = spaceArray[spaceIndex];
	const RadiositySpaceObject *object = space->GetObject();

	const Integer3D& radiosityResolution = object->GetRadiosityResolution();
	float satMultiplier = space->GetObject()->GetSaturationScale() * 127.0F;

	unsigned_int32 offset = (sampleCoord.z * radiosityResolution.y + sampleCoord.y) * radiosityResolution.x + sampleCoord.x;
	unsigned_int32 *value = valueData + offset * 6;
	Color2C *color = colorData + offset;

	bool completeFlag = false;

	float px = (float) sampleCoord.x * sampleDelta.x;
	float py = (float) sampleCoord.y * sampleDelta.y;
	float pz = (float) sampleCoord.z * sampleDelta.z;

	for (machine n = 0; n < 8; n++)
	{
		Point3D		samplePosition[6];

		Point3D center(px, py, pz);
		Point3D worldCenter = space->GetWorldTransform() * center;

		for (machine a = 0; a < 6; a++)
		{
			samplePosition[a] = worldCenter;
		}

		if (sampleCoord.x < radiosityResolution.x - 1)
		{
			Point3D p = space->GetWorldTransform() * Point3D(center.x + sampleDelta.x, center.y, center.z);
			if (DetectInteriorCollision(world, p, worldCenter, &collisionData))
			{
				samplePosition[0] = collisionData.position + collisionData.normal * cameraOffset;
			}
		}

		if (sampleCoord.x > 0)
		{
			Point3D p = space->GetWorldTransform() * Point3D(center.x - sampleDelta.x, center.y, center.z);
			if (DetectInteriorCollision(world, p, worldCenter, &collisionData))
			{
				samplePosition[1] = collisionData.position + collisionData.normal * cameraOffset;
			}
		}

		if (sampleCoord.y < radiosityResolution.y - 1)
		{
			Point3D p = space->GetWorldTransform() * Point3D(center.x, center.y + sampleDelta.y, center.z);
			if (DetectInteriorCollision(world, p, worldCenter, &collisionData))
			{
				samplePosition[2] = collisionData.position + collisionData.normal * cameraOffset;
			}
		}

		if (sampleCoord.y > 0)
		{
			Point3D p = space->GetWorldTransform() * Point3D(center.x, center.y - sampleDelta.y, center.z);
			if (DetectInteriorCollision(world, p, worldCenter, &collisionData))
			{
				samplePosition[3] = collisionData.position + collisionData.normal * cameraOffset;
			}
		}

		if (sampleCoord.z < radiosityResolution.z - 1)
		{
			Point3D p = space->GetWorldTransform() * Point3D(center.x, center.y, center.z + sampleDelta.z);
			if (DetectInteriorCollision(world, p, worldCenter, &collisionData))
			{
				samplePosition[4] = collisionData.position + collisionData.normal * cameraOffset;
			}
		}

		if (sampleCoord.z > 0)
		{
			Point3D p = space->GetWorldTransform() * Point3D(center.x, center.y, center.z - sampleDelta.z);
			if (DetectInteriorCollision(world, p, worldCenter, &collisionData))
			{
				samplePosition[5] = collisionData.position + collisionData.normal * cameraOffset;
			}
		}

		for (machine a = 0; a < 6; a++)
		{
			cubeCamera.SetNodePosition(samplePosition[a]);
			cubeCamera.SetFaceIndex(a);

			world->Update();
			world->BeginRendering();
			world->Render();
			world->EndRendering();

			TheGraphicsMgr->ReadImageBuffer(Rect(0, 0, kRenderSize, kRenderSize), imageBuffer[a], kRenderSize);
		}

		Integer3D chromaticity(0, 0, 0);

		for (machine a = 0; a < 6; a += 2)
		{
			const Matrix3D& transform = CubeCamera::cameraRotation[a];

			const Vector3D& direction = transform[2];
			ColorRGB accum[2]{{0.0F, 0.0F, 0.0F}, {0.0F, 0.0F, 0.0F}};

			for (machine k = 0; k < 6; k++)
			{
				const Matrix3D& m = CubeCamera::cameraRotation[k];
				const Color4C *image = imageBuffer[k];

				for (machine j = 0; j < kRenderSize; j++)
				{
					float v = ((float) j * 2.0F + 1.0F) * (1.0F / (float) kRenderSize) - 1.0F;
					for (machine i = 0; i < kRenderSize; i++)
					{
						float u = ((float) i * 2.0F + 1.0F) * (1.0F / (float) kRenderSize) - 1.0F;

						float d = direction * (m * Vector3D(u, -v, 1.0F)) * InverseSqrt(u * u + v * v + 1.0F);
						float f1 = FmaxZero(d);
						float f2 = FmaxZero(-d);

						const Color4C& c = image[j * kRenderSize + i];
						float r = (float) c.GetRed();
						float g = (float) c.GetGreen();
						float b = (float) c.GetBlue();

						accum[0].red += r * f1;
						accum[0].green += g * f1;
						accum[0].blue += b * f1;

						accum[1].red += r * f2;
						accum[1].green += g * f2;
						accum[1].blue += b * f2;
					}
				}
			}

			int32 red1 = (int32) (accum[0].red * K::four_over_tau);
			int32 green1 = (int32) (accum[0].green * K::four_over_tau);
			int32 blue1 = (int32) (accum[0].blue * K::four_over_tau);

			unsigned_int32 intensity1 = Max(Max(red1, green1), blue1);
			value[a] = intensity1;

			int32 red2 = (int32) (accum[1].red * K::four_over_tau);
			int32 green2 = (int32) (accum[1].green * K::four_over_tau);
			int32 blue2 = (int32) (accum[1].blue * K::four_over_tau);

			unsigned_int32 intensity2 = Max(Max(red2, green2), blue2);
			value[a + 1] = intensity2;

			unsigned_int32 i1 = intensity1 >> 16;
			unsigned_int32 i2 = intensity2 >> 16;
			chromaticity.x += red1 * i1 + red2 * i2;
			chromaticity.y += green1 * i1 + green2 * i2;
			chromaticity.z += blue1 * i1 + blue2 * i2;
		}

		float hue = 0.0F;
		float sat = 0.0F;

		float red = (float) chromaticity.x;
		float green = (float) chromaticity.y;
		float blue = (float) chromaticity.z;

		if ((red >= green) && (red >= blue))
		{
			if (red > K::min_float)
			{
				float f = 1.0F / red;
				green *= f;
				blue *= f;

				float s = 1.0F - Fmin(green, blue);
				hue = (s > K::min_float) ? (green - blue) / s : 0.0F;
				sat = s;
			}
		}
		else if (green >= blue)
		{
			float f = 1.0F / green;
			red *= f;
			blue *= f;

			float s = 1.0F - Fmin(red, blue);
			hue = (s > K::min_float) ? (blue - red) / s + 2.0F : 0.0F;
			sat = s;
		}
		else
		{
			float f = 1.0F / blue;
			red *= f;
			green *= f;

			float s = 1.0F - Fmin(red, green);
			hue = (s > K::min_float) ? (red - green) / s + 4.0F : 0.0F;
			sat = s;
		}

		Vector2D cs = CosSin(hue * K::tau_over_6) * (sat * satMultiplier);
		color->Set(MaxZero(Min((int32) (cs.x + 128.0F), 255)), MaxZero(Min((int32) (cs.y + 128.0F), 255)));

		value += 6;
		color++;

		if (++sampleCoord.x == radiosityResolution.x)
		{
			sampleCoord.x = 0;
			px = 0.0F;

			if (++sampleCoord.y == radiosityResolution.y)
			{
				sampleCoord.y = 0;
				py = 0.0F;

				if (++sampleCoord.z == radiosityResolution.z)
				{
					completeFlag = true;
					break;
				}

				pz = (float) sampleCoord.z * sampleDelta.z;
			}
			else
			{
				py = (float) sampleCoord.y * sampleDelta.y;
			}
		}
		else
		{
			px = (float) sampleCoord.x * sampleDelta.x;
		}
	}

	TheGraphicsMgr->SetAmbientMode(kAmbientNormal);
	TheGraphicsMgr->SetRenderOptionFlags(renderOptions);

	world->SetWorldFlags(worldFlags);
	world->SetWorldPerspective(0);
	world->SetRenderSize(worldRenderWidth, worldRenderHeight);
	world->SetCamera(worldCamera);

	if (completeFlag)
	{
		EndRadiositySpace();

		if (++spaceIndex < spaceArray.GetElementCount())
		{
			BeginRadiositySpace();
		}
		else
		{
			Close();
			return;
		}
	}

	int32 progress = ((sampleCoord.z * radiosityResolution.y + sampleCoord.y) * radiosityResolution.x + sampleCoord.x) * progressBar->GetMaxValue() / imageVoxelCount;
	progressBar->SetValue(progress);
}

void GenerateRadiosityDataWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if ((eventData->eventType == kEventWidgetActivate) && (widget == stopButton))
	{
		Close();
	}
}

// ZYUQURM
