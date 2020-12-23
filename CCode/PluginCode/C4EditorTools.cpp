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


#include "C4EditorTools.h"
#include "C4EditorSupport.h"
#include "C4EditorGizmo.h"
#include "C4WorldEditor.h"
#include "C4GeometryManipulators.h"
#include "C4MarkerManipulators.h"
#include "C4Water.h"


using namespace C4;


SharedVertexBuffer NodeReparentTool::reparentIndexBuffer(kVertexBufferIndex | kVertexBufferStatic);


EditorTool::EditorTool()
{
}

EditorTool::~EditorTool()
{
}

void EditorTool::Engage(Editor *editor, void *cookie)
{
}

void EditorTool::Disengage(Editor *editor, void *cookie)
{
}

bool EditorTool::BeginTool(Editor *editor, EditorTrackData *trackData)
{
	return (false);
}

bool EditorTool::TrackTool(Editor *editor, EditorTrackData *trackData)
{
	return (false);
}

bool EditorTool::EndTool(Editor *editor, EditorTrackData *trackData)
{
	return (false);
}

bool EditorTool::SelectNode(Editor *editor, EditorTrackData *trackData)
{
	PickData	pickData;

	Node *node = nullptr;
	bool getInfo = false;
	bool result = false;

	if (trackData->viewportType != kEditorViewportGraph)
	{
		node = editor->PickNode(trackData, &pickData, nullptr);
	}
	else
	{
		node = Editor::GetManipulator(editor->GetRootNode())->PickGraphNode(trackData, &trackData->worldRay);
		getInfo = ((trackData->mouseEventFlags & kMouseDoubleClick) != 0);
		pickData.pickIndex[0] = -1;
	}

	if (!(editor->GetEditorState() & kEditorSelectionLocked))
	{
		bool shift = ((trackData->currentModifierKeys & kModifierKeyShift) != 0);
		if (node)
		{
			trackData->trackNode = node;
			trackData->originalTransform = node->GetWorldTransform();

			if (node->GetNodeType() == kNodeMarker)
			{
				Marker *marker = static_cast<Marker *>(node);
				if (marker->GetMarkerType() == kMarkerPath)
				{
					int32 pointIndex1 = pickData.pickIndex[0];
					int32 pointIndex2 = pickData.pickIndex[1];
					if (pointIndex1 >= 0)
					{
						PathManipulator *manipulator = static_cast<PathManipulator *>(marker->GetManipulator());

						bool tangentFlag = ((trackData->currentModifierKeys & kModifierKeyCommand) != 0);
						if (shift)
						{
							if (manipulator->ControlPointSelected(pointIndex1))
							{
								manipulator->UnselectControlPoint(pointIndex1, tangentFlag);
								manipulator->UnselectControlPoint(pointIndex2, tangentFlag);
							}
							else
							{
								manipulator->SelectControlPoint(pointIndex1, tangentFlag);
								manipulator->SelectControlPoint(pointIndex2, tangentFlag);
							}
						}
						else
						{
							if (!manipulator->ControlPointSelected(pointIndex1)) 
							{
								editor->UnselectAll();
								editor->SelectNode(marker); 
								manipulator->SelectControlPoint(pointIndex1, tangentFlag);
								manipulator->SelectControlPoint(pointIndex2, tangentFlag); 
							}
						}
 
						trackData->trackType = kEditorTrackVertex;
						return (false); 
					} 
				}
			}

			if (node->GetManipulator()->Selected()) 
			{
				if (shift)
				{
					trackData->trackNode = nullptr;
					editor->UnselectNode(node);
					getInfo = false;
				}
				else
				{
					result = true;
				}
			}
			else
			{
				if (!shift)
				{
					editor->UnselectAll();
				}

				editor->SelectNode(node);
				result = true;
			}

			if (getInfo)
			{
				editor->OpenNodeInfo();
				result = false;
			}
		}
		else if (!shift)
		{
			editor->UnselectAll();
		}
	}
	else if ((node) && (node->GetManipulator()->Selected()))
	{
		trackData->trackNode = node;
		trackData->originalTransform = node->GetWorldTransform();
	}

	return (result);
}


StandardEditorTool::StandardEditorTool(IconButtonWidget *widget) : toolObserver(this, &StandardEditorTool::HandleToolButtonEvent)
{
	toolButton = widget;
	widget->SetObserver(&toolObserver);
}

StandardEditorTool::~StandardEditorTool()
{
}

void StandardEditorTool::HandleToolButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		Editor *editor = static_cast<Editor *>(widget->GetOwningWindow());
		editor->SetCurrentTool(this);
		editor->SetFocusWidget(nullptr);
	}
}

void StandardEditorTool::Engage(Editor *editor, void *cookie)
{
	toolButton->SetValue(1);
}

void StandardEditorTool::Disengage(Editor *editor, void *cookie)
{
	toolButton->SetValue(0);
}


NodeSelectTool::NodeSelectTool(IconButtonWidget *widget) : StandardEditorTool(widget)
{
}

NodeSelectTool::~NodeSelectTool()
{
}

bool NodeSelectTool::BeginTool(Editor *editor, EditorTrackData *trackData)
{
	SelectNode(editor, trackData);
	return (false);
}


NodeMoveTool::NodeMoveTool(IconButtonWidget *widget) : StandardEditorTool(widget)
{
}

NodeMoveTool::~NodeMoveTool()
{
}

void NodeMoveTool::Engage(Editor *editor, void *cookie)
{
	StandardEditorTool::Engage(editor, cookie);

	editor->SetGizmoState(editor->GetGizmoState() | kGizmoMoveEnable);
}

void NodeMoveTool::Disengage(Editor *editor, void *cookie)
{
	StandardEditorTool::Disengage(editor, cookie);

	editor->SetGizmoState(editor->GetGizmoState() & ~kGizmoMoveEnable);
}

bool NodeMoveTool::BeginTool(Editor *editor, EditorTrackData *trackData)
{
	if (trackData->currentModifierKeys == 0)
	{
		Node *node = editor->PickGizmoArrow(trackData);
		if (node)
		{
			if (Editor::GetManipulator(node)->GetManipulatorFlags() & kManipulatorLockedTransform)
			{
				return (false);
			}

			trackData->trackNode = node;
			trackData->originalTransform = node->GetWorldTransform();

			NodeReference *reference = editor->GetFirstSelectedNode();
			while (reference)
			{
				Editor::GetManipulator(reference->GetNode())->BeginTransform();
				reference = reference->Next();
			}

			operationFlag = false;
			planarMove = false;

			editor->SetGizmoState(editor->GetGizmoState() | kGizmoAxisMoveActive);
			return (true);
		}
		else
		{
			node = editor->PickGizmoSquare(trackData);
			if (node)
			{
				if (Editor::GetManipulator(node)->GetManipulatorFlags() & kManipulatorLockedTransform)
				{
					return (false);
				}

				trackData->trackNode = node;
				trackData->originalTransform = node->GetWorldTransform();

				NodeReference *reference = editor->GetFirstSelectedNode();
				while (reference)
				{
					Editor::GetManipulator(reference->GetNode())->BeginTransform();
					reference = reference->Next();
				}

				operationFlag = false;
				planarMove = true;

				editor->SetGizmoState(editor->GetGizmoState() | kGizmoPlaneMoveActive);
				return (true);
			}
		}
	}

	SelectNode(editor, trackData);

	if (trackData->viewportType == kEditorViewportOrtho)
	{
		Node *node = trackData->trackNode;
		if ((node) && (!(Editor::GetManipulator(node)->GetManipulatorFlags() & kManipulatorLockedTransform)))
		{
			NodeReference *reference = editor->GetFirstSelectedNode();
			while (reference)
			{
				Editor::GetManipulator(reference->GetNode())->BeginTransform();
				reference = reference->Next();
			}

			operationFlag = false;
			return (true);
		}
	}

	return (false);
}

bool NodeMoveTool::TrackTool(Editor *editor, EditorTrackData *trackData)
{
	editor->AutoScroll(trackData);

	if (trackData->currentModifierKeys & kModifierKeyShift)
	{
		float dx = trackData->currentPosition.x - trackData->anchorPosition.x;
		float dy = trackData->currentPosition.y - trackData->anchorPosition.y;

		if (Fabs(dy) > Fabs(dx))
		{
			trackData->currentPosition.x = trackData->anchorPosition.x;
		}
		else
		{
			trackData->currentPosition.y = trackData->anchorPosition.y;
		}
	}

	if (trackData->currentPosition != trackData->previousPosition)
	{
		Vector3D	delta;

		if (!operationFlag)
		{
			operationFlag = true;
			editor->AddOperation(new MoveOperation(editor->GetSelectionList()));
		}

		const EditorGizmo *gizmo = trackData->gizmo;
		if (gizmo)
		{
			if (!planarMove)
			{
				if (!gizmo->TrackArrow(&trackData->worldRay, &delta))
				{
					return (true);
				}
			}
			else
			{
				if (!gizmo->TrackSquare(&trackData->worldRay, &delta))
				{
					return (true);
				}
			}
		}
		else
		{
			delta = Editor::GetWorldSpaceDirection(trackData, trackData->currentPosition - trackData->anchorPosition);
		}

		const Point3D& originalPosition = trackData->originalTransform.GetTranslation();
		delta = editor->SnapToGrid(originalPosition + delta) - trackData->trackNode->GetWorldPosition();

		const NodeReference *reference = editor->GetFirstSelectedNode();
		while (reference)
		{
			Node *node = reference->GetNode();

			if (trackData->trackType == kEditorTrackVertex)
			{
				if (node->GetNodeType() == kNodeMarker)
				{
					const Marker *marker = static_cast<Marker *>(node);
					if (marker->GetMarkerType() == kMarkerPath)
					{
						EditorManipulator *manipulator = Editor::GetManipulator(node);
						if (manipulator->GetSelectionType() == kEditorSelectionVertex)
						{
							Vector3D dp = Editor::GetWorldSpaceDirection(trackData, trackData->snappedCurrentPosition - trackData->snappedPreviousPosition);
							static_cast<PathManipulator *>(manipulator)->MoveSelectedControlPoints(node->GetInverseWorldTransform() * dp, ((trackData->currentModifierKeys & kModifierKeyCommand) == 0));
						}
					}
				}
			}
			else
			{
				EditorManipulator *manipulator = Editor::GetManipulator(node);
				if (!(manipulator->GetManipulatorFlags() & kManipulatorLockedTransform))
				{
					Node *super = node->GetSuperNode();
					if ((super) && ((super == editor->GetRootNode()) || (!manipulator->PredecessorSelected())))
					{
						node->SetNodePosition(node->GetNodeTransform() * (Zero3D + node->GetInverseWorldTransform() * delta));
						manipulator->InvalidateNode();
					}
				}
			}

			reference = reference->Next();
		}

		editor->RegenerateTexcoords(editor->GetSelectionList());
	}

	return (true);
}

bool NodeMoveTool::EndTool(Editor *editor, EditorTrackData *trackData)
{
	EditorGizmo *gizmo = trackData->gizmo;
	if (gizmo)
	{
		editor->SetGizmoState(editor->GetGizmoState() & ~(kGizmoAxisMoveActive | kGizmoPlaneMoveActive));
	}

	bool result = NodeMoveTool::TrackTool(editor, trackData);

	NodeReference *reference = editor->GetFirstSelectedNode();
	while (reference)
	{
		Editor::GetManipulator(reference->GetNode())->EndTransform();
		reference = reference->Next();
	}

	editor->InvalidateAllViewports();
	return (result);
}


NodeRotateTool::NodeRotateTool(IconButtonWidget *widget) : StandardEditorTool(widget)
{
}

NodeRotateTool::~NodeRotateTool()
{
}

void NodeRotateTool::Engage(Editor *editor, void *cookie)
{
	StandardEditorTool::Engage(editor, cookie);

	editor->SetGizmoState(editor->GetGizmoState() | kGizmoRotateEnable);
	editor->SetRenderFlags(editor->GetRenderFlags() | kEditorRenderHandles);
}

void NodeRotateTool::Disengage(Editor *editor, void *cookie)
{
	StandardEditorTool::Disengage(editor, cookie);

	editor->SetGizmoState(editor->GetGizmoState() & ~kGizmoRotateEnable);
	editor->SetRenderFlags(editor->GetRenderFlags() & ~kEditorRenderHandles);
}

bool NodeRotateTool::BeginTool(Editor *editor, EditorTrackData *trackData)
{
	if (trackData->currentModifierKeys == 0)
	{
		Node *node = editor->PickGizmoCircle(trackData);
		if (node)
		{
			if (Editor::GetManipulator(node)->GetManipulatorFlags() & kManipulatorLockedTransform)
			{
				return (false);
			}

			trackData->trackNode = node;
			rotationCenter = node->GetWorldPosition();

			NodeReference *reference = editor->GetFirstSelectedNode();
			while (reference)
			{
				Editor::GetManipulator(reference->GetNode())->BeginTransform();
				reference = reference->Next();
			}

			operationFlag = false;
			accumAngle = 0.0F;

			editor->SetGizmoState(editor->GetGizmoState() | kGizmoRotateActive);
			return (true);
		}
	}

	if (trackData->viewportType == kEditorViewportOrtho)
	{
		int32	handleIndex;

		Node *node = editor->PickHandle(trackData, &handleIndex);
		if (node)
		{
			ManipulatorHandleData	handleData;

			const EditorManipulator *manipulator = Editor::GetManipulator(node);
			if (manipulator->GetManipulatorFlags() & kManipulatorLockedTransform)
			{
				return (false);
			}

			if (editor->GetEditorObject()->GetEditorFlags() & kEditorDrawFromCenter)
			{
				rotationCenter = manipulator->GetNodeSphere()->GetCenter();
			}
			else
			{
				manipulator->GetHandleData(handleIndex, &handleData);

				int32 index = handleData.oppositeIndex;
				if (index == kHandleOrigin)
				{
					rotationCenter = node->GetWorldPosition();
				}
				else
				{
					rotationCenter = node->GetWorldTransform() * manipulator->GetHandlePosition(index);
				}
			}

			operationFlag = false;
			accumAngle = 0.0F;

			NodeReference *reference = editor->GetFirstSelectedNode();
			while (reference)
			{
				Editor::GetManipulator(reference->GetNode())->BeginTransform();
				reference = reference->Next();
			}

			editor->InvalidateAllViewports();
			return (true);
		}
	}

	SelectNode(editor, trackData);
	return (false);
}

bool NodeRotateTool::TrackTool(Editor *editor, EditorTrackData *trackData)
{
	float			angle;
	Antivector3D	axis;

	editor->AutoScroll(trackData);

	const EditorGizmo *gizmo = trackData->gizmo;
	if (gizmo)
	{
		if (!gizmo->TrackCircle(&trackData->worldRay, &angle))
		{
			return (true);
		}

		axis = gizmo->GetTransformable()->GetWorldTransform()[trackData->gizmoIndex];

		if (trackData->currentModifierKeys & kModifierKeyShift)
		{
			float snap = editor->GetEditorObject()->GetSnapAngle();
			angle = Floor(angle / snap + 0.5F) * snap;
		}
	}
	else
	{
		const Camera *camera = trackData->viewportCamera;
		Vector3D center = camera->GetInverseWorldTransform() * (rotationCenter + camera->GetWorldPosition());

		Vector2D v1(trackData->anchorPosition.x - center.x, trackData->anchorPosition.y - center.y);
		Vector2D v2(trackData->currentPosition.x - center.x, trackData->currentPosition.y - center.y);

		angle = Acos(v1 * v2 * InverseSqrt(SquaredMag(v1) * SquaredMag(v2)));
		if (v1.x * v2.y - v1.y * v2.x < 0.0F)
		{
			angle = -angle;
		}

		axis = camera->GetWorldTransform()[2];
	}

	if (trackData->currentModifierKeys & kModifierKeyShift)
	{
		float snap = editor->GetEditorObject()->GetSnapAngle();
		angle = Floor(angle / snap + 0.5F) * snap;
	}

	float accum = accumAngle;
	if (angle != accum)
	{
		Matrix3D	rotation;

		accumAngle = angle;
		rotation.SetRotationAboutAxis(angle - accum, axis);

		Transform4D centralRotation(rotation, rotationCenter - rotation * rotationCenter);

		if (!operationFlag)
		{
			operationFlag = true;
			editor->AddOperation(new MoveOperation(editor->GetSelectionList()));
		}

		const NodeReference *reference = editor->GetFirstSelectedNode();
		while (reference)
		{
			Node *node = reference->GetNode();
			EditorManipulator *manipulator = Editor::GetManipulator(node);
			if (!(manipulator->GetManipulatorFlags() & kManipulatorLockedTransform))
			{
				Node *super = node->GetSuperNode();
				if ((super) && ((super == editor->GetRootNode()) || (!manipulator->PredecessorSelected())))
				{
					Transform4D transform = node->GetNodeTransform() * node->GetInverseWorldTransform() * centralRotation * node->GetWorldTransform();
					node->SetNodeTransform(transform.Normalize());
					manipulator->InvalidateNode();
				}
			}

			reference = reference->Next();
		}

		editor->RegenerateTexcoords(editor->GetSelectionList());
	}

	return (true);
}

bool NodeRotateTool::EndTool(Editor *editor, EditorTrackData *trackData)
{
	EditorGizmo *gizmo = trackData->gizmo;
	if (gizmo)
	{
		editor->SetGizmoState(editor->GetGizmoState() & ~kGizmoRotateActive);
	}

	bool result = NodeRotateTool::TrackTool(editor, trackData);

	NodeReference *reference = editor->GetFirstSelectedNode();
	while (reference)
	{
		Editor::GetManipulator(reference->GetNode())->EndTransform();
		reference = reference->Next();
	}

	editor->InvalidateAllViewports();
	return (result);
}


NodeScaleTool::NodeScaleTool(IconButtonWidget *widget) : StandardEditorTool(widget)
{
}

NodeScaleTool::~NodeScaleTool()
{
}

void NodeScaleTool::Engage(Editor *editor, void *cookie)
{
	StandardEditorTool::Engage(editor, cookie);

	editor->SetRenderFlags(editor->GetRenderFlags() | kEditorRenderHandles);
}

void NodeScaleTool::Disengage(Editor *editor, void *cookie)
{
	StandardEditorTool::Disengage(editor, cookie);

	editor->SetRenderFlags(editor->GetRenderFlags() & ~kEditorRenderHandles);
}

bool NodeScaleTool::BeginTool(Editor *editor, EditorTrackData *trackData)
{
	if (trackData->viewportType == kEditorViewportOrtho)
	{
		int32	handleIndex;

		Node *node = editor->PickHandle(trackData, &handleIndex);
		if (node)
		{
			ManipulatorHandleData	handleData;

			const EditorManipulator *manipulator = Editor::GetManipulator(node);
			if ((manipulator->GetManipulatorFlags() & kManipulatorLockedTransform) && (node->GetSuperNode()))
			{
				return (false);
			}

			manipulator->GetHandleData(handleIndex, &handleData);
			trackData->resizeData.resizeFlags = (editor->GetEditorObject()->GetEditorFlags() & kEditorDrawFromCenter) ? kManipulatorResizeCenter : 0;
			trackData->resizeData.handleFlags = handleData.handleFlags;
			trackData->resizeData.handleIndex = handleIndex;

			NodeReference *reference = editor->GetFirstSelectedNode();
			while (reference)
			{
				Editor::GetManipulator(reference->GetNode())->BeginResize(&trackData->resizeData);
				reference = reference->Next();
			}

			operationFlag = false;
			return (true);
		}
	}

	SelectNode(editor, trackData);
	return (false);
}

bool NodeScaleTool::TrackTool(Editor *editor, EditorTrackData *trackData)
{
	editor->AutoScroll(trackData);

	bool currentShift = ((trackData->currentModifierKeys & kModifierKeyShift) != 0);
	bool previousShift = ((trackData->previousModifierKeys & kModifierKeyShift) != 0);

	if ((trackData->currentPosition != trackData->previousPosition) || (currentShift != previousShift))
	{
		if (!operationFlag)
		{
			operationFlag = true;
			editor->AddOperation(new ResizeOperation(editor->GetSelectionList()));
		}

		if (currentShift)
		{
			trackData->resizeData.resizeFlags |= kManipulatorResizeConstrain;
		}
		else
		{
			trackData->resizeData.resizeFlags &= ~kManipulatorResizeConstrain;
		}

		Vector3D delta = Editor::GetWorldSpaceDirection(trackData, trackData->snappedCurrentPosition - trackData->snappedAnchorPosition);

		const NodeReference *reference = editor->GetFirstSelectedNode();
		while (reference)
		{
			Node *node = reference->GetNode();
			if (node->GetNodeType() != kNodeGeneric)
			{
				EditorManipulator *manipulator = Editor::GetManipulator(node);
				if ((!(manipulator->GetManipulatorFlags() & kManipulatorLockedTransform)) || (!node->GetSuperNode()))
				{
					trackData->resizeData.resizeDelta = node->GetInverseWorldTransform() * delta;
					trackData->resizeData.positionOffset.Set(0.0F, 0.0F, 0.0F);

					bool move = manipulator->Resize(&trackData->resizeData);
					node->Invalidate();

					if (move)
					{
						Point3D compensator = node->GetNodePosition();
						node->SetNodePosition(manipulator->GetOriginalPosition() + node->GetNodeTransform() * trackData->resizeData.positionOffset);

						Node *subnode = node->GetFirstSubnode();
						if (subnode)
						{
							compensator = Inverse(node->GetNodeTransform()) * compensator;
							do
							{
								subnode->SetNodePosition(compensator + subnode->GetNodePosition());
								subnode->Invalidate();

								subnode = subnode->Next();
							} while (subnode);
						}
					}
				}
			}

			reference = reference->Next();
		}

		editor->RebuildGeometry(editor->GetSelectionList());
	}

	return (true);
}

bool NodeScaleTool::EndTool(Editor *editor, EditorTrackData *trackData)
{
	bool result = NodeScaleTool::TrackTool(editor, trackData);

	NodeReference *reference = editor->GetFirstSelectedNode();
	while (reference)
	{
		Editor::GetManipulator(reference->GetNode())->EndResize(&trackData->resizeData);
		reference = reference->Next();
	}

	return (result);
}


ConnectTool::ConnectTool(IconButtonWidget *widget) : StandardEditorTool(widget)
{
}

ConnectTool::~ConnectTool()
{
}

void ConnectTool::Engage(Editor *editor, void *cookie)
{
	StandardEditorTool::Engage(editor, cookie);

	editor->SetRenderFlags(editor->GetRenderFlags() | kEditorRenderConnectors);
}

void ConnectTool::Disengage(Editor *editor, void *cookie)
{
	StandardEditorTool::Disengage(editor, cookie);

	editor->UnselectAllConnectors();
	editor->SetRenderFlags(editor->GetRenderFlags() & ~kEditorRenderConnectors);
}

bool ConnectTool::BeginTool(Editor *editor, EditorTrackData *trackData)
{
	if (trackData->viewportType == kEditorViewportOrtho)
	{
		int32	index;

		Node *node = editor->PickConnector(trackData, &index);
		if (node)
		{
			if ((trackData->currentModifierKeys & kModifierKeyShift) == 0)
			{
				editor->UnselectAllConnectors(node);
				editor->SelectConnector(node, index, false);

				if (trackData->mouseEventFlags & kMouseDoubleClick)
				{
					Node *target = Editor::GetManipulator(node)->GetConnectorSelectionTarget();
					if (target)
					{
						editor->UnselectAll();
						editor->SelectNode(target);
					}
				}
			}
			else
			{
				editor->SelectConnector(node, index, true);
			}

			return (false);
		}
	}

	SelectNode(editor, trackData);
	return (false);
}


SurfaceSelectTool::SurfaceSelectTool(IconButtonWidget *widget) : StandardEditorTool(widget)
{
}

SurfaceSelectTool::~SurfaceSelectTool()
{
}

void SurfaceSelectTool::Engage(Editor *editor, void *cookie)
{
	StandardEditorTool::Engage(editor, cookie);
}

void SurfaceSelectTool::Disengage(Editor *editor, void *cookie)
{
	StandardEditorTool::Disengage(editor, cookie);
}

bool SurfaceSelectTool::BeginTool(Editor *editor, EditorTrackData *trackData)
{
	if (trackData->viewportType != kEditorViewportGraph)
	{
		PickData	pickData;

		auto filter = [](const Node *node, const PickData *, const void *) -> bool
		{
			return (node->GetNodeType() == kNodeGeometry);
		};

		bool shift = ((trackData->currentModifierKeys & kModifierKeyShift) != 0);
		Node *node = editor->PickNode(trackData, &pickData, filter);
		if ((node) && (pickData.triangleIndex != kInvalidTriangleIndex))
		{
			const GeometryObject *object = static_cast<Geometry *>(node)->GetObject();
			const Mesh *level = object->GetGeometryLevel(0);
			const Triangle *triangle = level->GetArray<Triangle>(kArrayPrimitive);
			const unsigned_int16 *surfaceIndex = level->GetArray<unsigned_int16>(kArraySurfaceIndex);
			int32 selectIndex = (surfaceIndex) ? surfaceIndex[triangle[pickData.triangleIndex].index[0]] : 0;

			GeometryManipulator *manipulator = static_cast<GeometryManipulator *>(node->GetManipulator());

			if (shift)
			{
				if (!manipulator->Selected())
				{
					editor->SelectNode(node);
				}

				if (manipulator->SurfaceSelected(selectIndex))
				{
					manipulator->UnselectSurface(selectIndex);
					if (manipulator->GetSelectionType() == kEditorSelectionObject)
					{
						editor->UnselectNode(node);
					}
				}
				else
				{
					manipulator->SelectSurface(selectIndex);
				}

				editor->InvalidateSelection();
			}
			else
			{
				editor->UnselectAll();
				editor->SelectNode(node);
				manipulator->SelectSurface(selectIndex);
			}
		}
		else
		{
			if (!shift)
			{
				editor->UnselectAll();
			}
		}
	}

	return (false);
}


ViewportScrollTool::ViewportScrollTool(IconButtonWidget *widget) : StandardEditorTool(widget)
{
}

ViewportScrollTool::~ViewportScrollTool()
{
}

void ViewportScrollTool::Engage(Editor *editor, void *cookie)
{
	StandardEditorTool::Engage(editor, cookie);

	editor->SetCurrentCursor(TheWorldEditor->GetEditorCursor(kEditorCursorHand));
}

bool ViewportScrollTool::BeginTool(Editor *editor, EditorTrackData *trackData)
{
	initalCameraPosition = trackData->viewportCamera->GetNodePosition();

	previousCursor = editor->GetCurrentCursor();
	editor->SetCurrentCursor(TheWorldEditor->GetEditorCursor(kEditorCursorDrag));
	return (true);
}

bool ViewportScrollTool::TrackTool(Editor *editor, EditorTrackData *trackData)
{
	float dx = trackData->anchorViewportPosition.x - trackData->currentViewportPosition.x;
	float dy = trackData->anchorViewportPosition.y - trackData->currentViewportPosition.y;

	if ((dx != 0.0F) || (dy != 0.0F))
	{
		Camera *camera = trackData->viewportCamera;
		const Vector3D& right = camera->GetNodeTransform()[0];
		const Vector3D& down = camera->GetNodeTransform()[1];

		if (camera->GetCameraType() == kCameraOrtho)
		{
			const OrthoCameraObject *object = static_cast<OrthoCameraObject *>(camera->GetObject());
			dx *= object->GetOrthoRectRight() - object->GetOrthoRectLeft();
			dy *= object->GetOrthoRectBottom() - object->GetOrthoRectTop();

			if (trackData->viewportType == kEditorViewportGraph)
			{
				float x = Floor(initalCameraPosition.x + right.x * dx + down.x * dy);
				float y = Floor(initalCameraPosition.y + right.y * dx + down.y * dy);
				camera->SetNodePosition(Point3D(x, y, initalCameraPosition.z));
			}
			else
			{
				Vector3D offset = right * dx + down * dy;
				camera->SetNodePosition(initalCameraPosition + offset);
			}
		}
		else
		{
			Vector3D offset = right * (dx * 8.0F) + down * (dy * 8.0F);
			camera->SetNodePosition(initalCameraPosition + offset);
		}

		editor->InvalidateViewport(trackData->viewportIndex);
	}

	return (true);
}

bool ViewportScrollTool::EndTool(Editor *editor, EditorTrackData *trackData)
{
	editor->SetCurrentCursor(previousCursor);
	return (true);
}


ViewportZoomTool::ViewportZoomTool(IconButtonWidget *widget) : StandardEditorTool(widget)
{
}

ViewportZoomTool::~ViewportZoomTool()
{
}

void ViewportZoomTool::Engage(Editor *editor, void *cookie)
{
	StandardEditorTool::Engage(editor, cookie);

	editor->SetCurrentCursor(TheWorldEditor->GetEditorCursor(kEditorCursorGlass));
}

bool ViewportZoomTool::BeginTool(Editor *editor, EditorTrackData *trackData)
{
	return (true);
}

bool ViewportZoomTool::TrackTool(Editor *editor, EditorTrackData *trackData)
{
	float dy = trackData->previousViewportPosition.y - trackData->currentViewportPosition.y;
	if (dy != 0.0F)
	{
		EditorViewport *viewport = editor->GetViewport(trackData->viewportIndex);
		ViewportWidget *viewportWidget = viewport->GetViewportWidget();
		Camera *camera = trackData->viewportCamera;

		if (camera->GetCameraType() == kCameraOrtho)
		{
			float scale = trackData->viewportScale * Exp(dy * -4.0F);
			if (trackData->viewportType == kEditorViewportGraph)
			{
				scale = Fmax(scale, 1.0F);
			}

			trackData->viewportScale = scale;
			static_cast<OrthoViewportWidget *>(viewportWidget)->SetOrthoScale(Vector2D(scale, scale));
		}
		else
		{
			const Vector3D& view = camera->GetNodeTransform()[2];
			camera->SetNodePosition(camera->GetNodePosition() + view * (dy * 16.0F));
		}

		viewport->Invalidate();
	}

	return (true);
}


DragRectTool::DragRectTool(IconButtonWidget *widget, const ColorRGBA& color) :
		StandardEditorTool(widget),
		dragRect(color)
{
}

DragRectTool::~DragRectTool()
{
}

bool DragRectTool::BeginTool(Editor *editor, EditorTrackData *trackData)
{
	if (trackData->viewportType != kEditorViewportFrustum)
	{
		const Point2D& anchor = trackData->anchorPosition;
		const Transform4D& transform = trackData->viewportCamera->GetNodeTransform();
		dragRect.Build(anchor, anchor, transform[0], transform[1], trackData->viewportScale);

		EditorViewport *viewport = editor->GetViewport(trackData->viewportIndex);
		viewport->SetToolRenderable(&dragRect);
		viewport->GetViewportWidget()->InvalidateTexture();
		return (true);
	}

	return (false);
}

bool DragRectTool::TrackTool(Editor *editor, EditorTrackData *trackData)
{
	editor->AutoScroll(trackData);

	const Point2D& anchor = trackData->anchorPosition;
	const Point2D& position = trackData->currentPosition;
	const Transform4D& transform = trackData->viewportCamera->GetNodeTransform();
	dragRect.Build(anchor, position, transform[0], transform[1], trackData->viewportScale);

	EditorViewport *viewport = editor->GetViewport(trackData->viewportIndex);
	viewport->SetToolRenderable(&dragRect);
	viewport->GetViewportWidget()->InvalidateTexture();
	return (true);
}

bool DragRectTool::EndTool(Editor *editor, EditorTrackData *trackData)
{
	editor->InvalidateViewport(trackData->viewportIndex);
	return (true);
}


BoxSelectTool::BoxSelectTool(IconButtonWidget *widget) : DragRectTool(widget, ColorRGBA(0.5F, 0.5F, 0.5F, 1.0F))
{
}

BoxSelectTool::~BoxSelectTool()
{
}

bool BoxSelectTool::BeginTool(Editor *editor, EditorTrackData *trackData)
{
	if (DragRectTool::BeginTool(editor, trackData))
	{
		if ((trackData->currentModifierKeys & kModifierKeyShift) == 0)
		{
			editor->UnselectAll();
		}

		return (true);
	}

	SelectNode(editor, trackData);
	return (false);
}

bool BoxSelectTool::TrackTool(Editor *editor, EditorTrackData *trackData)
{
	DragRectTool::TrackTool(editor, trackData);

	editor->UnselectAllTemp();

	const Point2D& position = trackData->currentPosition;
	const Point2D& anchor = trackData->anchorPosition;

	if ((position.x != anchor.x) || (position.y != anchor.y))
	{
		Point3D p1 = Editor::GetWorldSpacePosition(trackData, anchor);
		Point3D p3 = Editor::GetWorldSpacePosition(trackData, position);

		Node *rootNode = editor->GetRootNode();

		if (trackData->viewportType == kEditorViewportOrtho)
		{
			GenericRegion	region;

			float sx = (position.x > anchor.x) ? 1.0F : -1.0F;
			float sy = (position.y > anchor.y) ? 1.0F : -1.0F;

			const Transform4D& transform = trackData->viewportCamera->GetNodeTransform();
			Antivector4D *plane = region.GetPlaneArray();
			plane[0].Set(transform[0] * sx, p1);
			plane[1].Set(transform[1] * sy, p1);
			plane[2].Set(-transform[0] * sx, p3);
			plane[3].Set(-transform[1] * sy, p3);
			region.SetPlaneCount(4);

			Node *node = rootNode->GetFirstSubnode();
			while (node)
			{
				const EditorManipulator *editorManipulator = Editor::GetManipulator(node);
				const BoundingSphere *sphere = editorManipulator->GetTreeSphere();

				if ((sphere) && (region.SphereVisible(sphere->GetCenter(), sphere->GetRadius())))
				{
					if ((!(editorManipulator->GetManipulatorState() & (kManipulatorSelected | kManipulatorHidden))) && (editor->NodeSelectable(node)))
					{
						if (editorManipulator->RegionPick(&region))
						{
							Manipulator *manipulator = node->GetManipulator();
							if (!manipulator->Selected())
							{
								editor->SelectNode(node);
								manipulator->SetManipulatorState(manipulator->GetManipulatorState() | kManipulatorTempSelected);
							}
						}
					}

					node = rootNode->GetNextNode(node);
				}
				else
				{
					node = rootNode->GetNextLevelNode(node);
				}
			}
		}
		else
		{
			float left = p1.x;
			float right = p3.x;
			if (left > right)
			{
				float t = left;
				left = right;
				right = t;
			}

			float top = p1.y;
			float bottom = p3.y;
			if (top > bottom)
			{
				float t = top;
				top = bottom;
				bottom = t;
			}

			Editor::GetManipulator(rootNode)->SelectGraphNodes(left, right, top, bottom, true);
		}
	}

	return (true);
}

bool BoxSelectTool::EndTool(Editor *editor, EditorTrackData *trackData)
{
	TrackTool(editor, trackData);

	NodeReference *reference = editor->GetFirstSelectedNode();
	while (reference)
	{
		Manipulator *manipulator = reference->GetNode()->GetManipulator();
		manipulator->SetManipulatorState(manipulator->GetManipulatorState() & ~kManipulatorTempSelected);

		reference = reference->Next();
	}

	editor->GetViewport(trackData->viewportIndex)->SetToolRenderable(nullptr);
	return (DragRectTool::EndTool(editor, trackData));
}


ViewportBoxZoomTool::ViewportBoxZoomTool(IconButtonWidget *widget) : DragRectTool(widget, ColorRGBA(0.5F, 0.5F, 0.75F, 1.0F))
{
}

ViewportBoxZoomTool::~ViewportBoxZoomTool()
{
}

void ViewportBoxZoomTool::Engage(Editor *editor, void *cookie)
{
	DragRectTool::Engage(editor, cookie);

	editor->SetCurrentCursor(TheWorldEditor->GetEditorCursor(kEditorCursorGlass));
}

bool ViewportBoxZoomTool::EndTool(Editor *editor, EditorTrackData *trackData)
{
	editor->AutoScroll(trackData);

	float x = Fabs(trackData->currentViewportPosition.x - trackData->anchorViewportPosition.x);
	float y = Fabs(trackData->currentViewportPosition.y - trackData->anchorViewportPosition.y);
	float scale = Fmax(x, y) * trackData->viewportScale;

	if (scale != 0.0F)
	{
		Point3D p = Editor::GetWorldSpacePosition(trackData, (trackData->anchorPosition + trackData->currentPosition) * 0.5F);

		if (trackData->viewportType == kEditorViewportGraph)
		{
			scale = Fmax(scale, 1.0F);
			p.x = Floor(p.x);
			p.y = Floor(p.y);
		}

		trackData->viewportCamera->SetNodePosition(p);

		ViewportWidget *viewport = editor->GetViewport(trackData->viewportIndex)->GetViewportWidget();
		static_cast<OrthoViewportWidget *>(viewport)->SetOrthoScale(Vector2D(scale, scale));
	}

	return (DragRectTool::EndTool(editor, trackData));
}


OrbitCameraTool::OrbitCameraTool(IconButtonWidget *widget) : StandardEditorTool(widget)
{
}

OrbitCameraTool::~OrbitCameraTool()
{
}

void OrbitCameraTool::Engage(Editor *editor, void *cookie)
{
	StandardEditorTool::Engage(editor, cookie);
}

bool OrbitCameraTool::BeginTool(Editor *editor, EditorTrackData *trackData)
{
	if (trackData->viewportCamera->GetCameraType() == kCameraFrustum)
	{
		const NodeReference *reference = editor->GetGizmoTarget();
		if (reference)
		{
			Box3D box = Editor::GetManipulator(reference->GetNode())->CalculateWorldBoundingBox();
			orbitCenter = box.GetCenter();
		}
		else
		{
			orbitCenter.Set(0.0F, 0.0F, 0.0F);
		}

		return (true);
	}

	return (false);
}

bool OrbitCameraTool::TrackTool(Editor *editor, EditorTrackData *trackData)
{
	FrustumViewportWidget *viewport = static_cast<FrustumViewportWidget *>(editor->GetViewport(trackData->viewportIndex)->GetViewportWidget());

	float dx = (trackData->previousViewportPosition.x - trackData->currentViewportPosition.x) * 8.0F;
	float dy = InterfaceMgr::GetShiftKey() ? 0.0F : (trackData->previousViewportPosition.y - trackData->currentViewportPosition.y) * 8.0F;

	float azm = viewport->GetCameraAzimuth() + dx;
	if (azm < -K::tau_over_2)
	{
		azm += K::tau;
	}
	else if (azm > K::tau_over_2)
	{
		azm -= K::tau;
	}

	float alt0 = viewport->GetCameraAltitude();
	float alt = Clamp(alt0 + dy, -1.45F, 1.45F);

	const Camera *camera = trackData->viewportCamera;
	const Vector3D& right = camera->GetNodeTransform()[0];

	Vector3D p = camera->GetNodePosition() - orbitCenter;
	Matrix3D m = Quaternion().SetRotationAboutAxis(alt - alt0, right).GetRotationMatrix();
	p = Matrix3D().SetRotationAboutZ(dx) * (m * p);

	viewport->SetCameraTransform(azm, alt, orbitCenter + p);
	return (true);
}


FreeCameraTool::FreeCameraTool(IconButtonWidget *widget) : StandardEditorTool(widget)
{
}

FreeCameraTool::~FreeCameraTool()
{
}

void FreeCameraTool::Engage(Editor *editor, void *cookie)
{
	StandardEditorTool::Engage(editor, cookie);

	editor->SetCurrentCursor(TheWorldEditor->GetEditorCursor(kEditorCursorFree));
}

bool FreeCameraTool::BeginTool(Editor *editor, EditorTrackData *trackData)
{
	if (trackData->viewportCamera->GetCameraType() == kCameraFrustum)
	{
		cameraSpeed = 0.0F;
		cameraFlags = 0;

		previousCursor = editor->GetCurrentCursor();
		editor->SetCurrentCursor(TheWorldEditor->GetEditorCursor(kEditorCursorFree));
		return (true);
	}

	return (false);
}

bool FreeCameraTool::TrackTool(Editor *editor, EditorTrackData *trackData)
{
	FrustumViewportWidget *viewport = static_cast<FrustumViewportWidget *>(editor->GetViewport(trackData->viewportIndex)->GetViewportWidget());

	float dx = trackData->previousViewportPosition.x - trackData->currentViewportPosition.x;
	float dy = trackData->previousViewportPosition.y - trackData->currentViewportPosition.y;

	float azm = viewport->GetCameraAzimuth() + dx * 8.0F;
	if (azm < -K::tau_over_2)
	{
		azm += K::tau;
	}
	else if (azm > K::tau_over_2)
	{
		azm -= K::tau;
	}

	float alt = Clamp(viewport->GetCameraAltitude() + dy * 8.0F, -1.45F, 1.45F);

	const Camera *camera = trackData->viewportCamera;
	Point3D position = camera->GetNodePosition();

	unsigned_int32 flags = cameraFlags;
	if (flags != 0)
	{
		float t = TheTimeMgr->GetSystemFloatDeltaTime();
		cameraSpeed = Fmin(cameraSpeed + t * 5.0e-4F, editor->GetEditorObject()->GetCameraSpeed());
		t *= cameraSpeed;

		if (InterfaceMgr::GetShiftKey())
		{
			t *= 5.0F;
		}

		const Vector3D& view = camera->GetNodeTransform()[2];
		const Vector3D& right = camera->GetNodeTransform()[0];

		if (flags & kFreeCameraForward)
		{
			position += view * t;
		}

		if (flags & kFreeCameraBackward)
		{
			position -= view * t;
		}

		if (flags & kFreeCameraRight)
		{
			position += right * t;
		}

		if (flags & kFreeCameraLeft)
		{
			position -= right * t;
		}

		if (flags & kFreeCameraUp)
		{
			position.z += t;
		}

		if (flags & kFreeCameraDown)
		{
			position.z -= t;
		}
	}
	else
	{
		cameraSpeed = 0.0F;
	}

	viewport->SetCameraTransform(azm, alt, position);
	return (true);
}

bool FreeCameraTool::EndTool(Editor *editor, EditorTrackData *trackData)
{
	editor->SetCurrentCursor(previousCursor);
	return (true);
}


NodeReparentTool::NodeReparentTool() :
		reparentVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		reparentRenderable(kRenderIndexedTriangles)
{
	reparentRenderable.SetAmbientBlendState(kBlendInterpolate);

	reparentRenderable.SetVertexCount(20);
	reparentRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &reparentVertexBuffer, sizeof(ReparentVertex));
	reparentRenderable.SetVertexBuffer(kVertexBufferIndexArray, &reparentIndexBuffer);
	reparentRenderable.SetVertexAttributeArray(kArrayPosition, 0, 2);
	reparentRenderable.SetVertexAttributeArray(kArrayColor, sizeof(Point2D), 1);
	reparentVertexBuffer.Establish(sizeof(ReparentVertex) * 20);

	if (reparentIndexBuffer.Retain() == 1)
	{
		static const Triangle reparentTriangle[18] =
		{
			{{12, 15, 16}}, {{16, 15, 19}}, {{13, 12, 17}}, {{17, 12, 16}},
			{{14, 13, 18}}, {{18, 13, 17}}, {{15, 14, 19}}, {{19, 14, 18}},
			{{ 0,  1,  2}}, {{ 0,  2,  3}}, {{ 4,  7,  8}}, {{ 8,  7, 11}}, {{ 5,  4,  9}},
			{{ 9,  4,  8}}, {{ 6,  5, 10}}, {{10,  5,  9}}, {{ 7,  6, 11}}, {{11,  6, 10}}
		};

		reparentIndexBuffer.Establish(sizeof(Triangle) * 18, reparentTriangle);
	}
}

NodeReparentTool::~NodeReparentTool()
{
	reparentIndexBuffer.Release();
}

void NodeReparentTool::CalculateReparentVertices(const Point2D& position, const Box2D *box)
{
	static const ConstColor4C reparentColor[80] =
	{
		{128, 128, 128, 192}, {128, 128, 128, 192}, {128, 128, 128, 192}, {128, 128, 128, 192},
		{0, 0, 0, 192}, {0, 0, 0, 192}, {0, 0, 0, 192}, {0, 0, 0, 192},
		{0, 0, 0, 192}, {0, 0, 0, 192}, {0, 0, 0, 192}, {0, 0, 0, 192},
		{255, 255, 0, 255}, {255, 255, 0, 255}, {255, 255, 0, 255}, {255, 255, 0, 255},
		{255, 255, 0, 255}, {255, 255, 0, 255}, {255, 255, 0, 255}, {255, 255, 0, 255}
	};

	volatile ReparentVertex *restrict vertex = reparentVertexBuffer.BeginUpdate<ReparentVertex>();

	float x = position.x - 24.0F;
	float y = position.y - 6.0F;

	vertex[0].position.Set(x, y);
	vertex[0].color = reparentColor[0];
	vertex[1].position.Set(x, y + 12.0F);
	vertex[1].color = reparentColor[1];
	vertex[2].position.Set(x + 48.0F, y + 12.0F);
	vertex[2].color = reparentColor[2];
	vertex[3].position.Set(x + 48.0F, y);
	vertex[3].color = reparentColor[3];
	vertex[4].position.Set(x, y);
	vertex[4].color = reparentColor[4];
	vertex[5].position.Set(x, y + 12.0F);
	vertex[5].color = reparentColor[5];
	vertex[6].position.Set(x + 48.0F, y + 12.0F);
	vertex[6].color = reparentColor[6];
	vertex[7].position.Set(x + 48.0F, y);
	vertex[7].color = reparentColor[7];
	vertex[8].position.Set(x - 1.0F, y - 1.0F);
	vertex[8].color = reparentColor[8];
	vertex[9].position.Set(x - 1.0F, y + 13.0F);
	vertex[9].color = reparentColor[9];
	vertex[10].position.Set(x + 49.0F, y + 13.0F);
	vertex[10].color = reparentColor[10];
	vertex[11].position.Set(x + 49.0F, y - 1.0F);
	vertex[11].color = reparentColor[11];

	if (box)
	{
		vertex[12].position.Set(box->min.x - 3.0F, box->min.y - 3.0F);
		vertex[12].color = reparentColor[12];
		vertex[13].position.Set(box->min.x - 3.0F, box->max.y + 3.0F);
		vertex[13].color = reparentColor[13];
		vertex[14].position.Set(box->max.x + 3.0F, box->max.y + 3.0F);
		vertex[14].color = reparentColor[14];
		vertex[15].position.Set(box->max.x + 3.0F, box->min.y - 3.0F);
		vertex[15].color = reparentColor[15];
		vertex[16].position.Set(box->min.x - 5.0F, box->min.y - 5.0F);
		vertex[16].color = reparentColor[16];
		vertex[17].position.Set(box->min.x - 5.0F, box->max.y + 5.0F);
		vertex[17].color = reparentColor[17];
		vertex[18].position.Set(box->max.x + 5.0F, box->max.y + 5.0F);
		vertex[18].color = reparentColor[18];
		vertex[19].position.Set(box->max.x + 5.0F, box->min.y - 5.0F);
		vertex[19].color = reparentColor[19];
	}

	reparentVertexBuffer.EndUpdate();
}

EditorManipulator *NodeReparentTool::GetReparentNode(Editor *editor, EditorTrackData *trackData)
{
	const Node *node = Editor::GetManipulator(editor->GetRootNode())->PickGraphNode(trackData, &trackData->worldRay);
	if (node)
	{
		EditorManipulator *manipulator = Editor::GetManipulator(node);
		if ((manipulator->ReparentedSubnodesAllowed()) && (!manipulator->Selected()))
		{
			reparentVisible = true;
			return (manipulator);
		}
	}
	else
	{
		reparentVisible = true;
	}

	return (nullptr);
}

bool NodeReparentTool::BeginTool(Editor *editor, EditorTrackData *trackData)
{
	if (SelectNode(editor, trackData))
	{
		reparentVisible = false;
		return (true);
	}

	return (false);
}

bool NodeReparentTool::TrackTool(Editor *editor, EditorTrackData *trackData)
{
	editor->AutoScroll(trackData);

	EditorManipulator *manipulator = GetReparentNode(editor, trackData);
	if (manipulator)
	{
		Box2D box = manipulator->GetGraphBox();

		reparentRenderable.GetFirstRenderSegment()->SetPrimitiveRange(0, 18);
		CalculateReparentVertices(trackData->currentPosition, &box);
	}
	else
	{
		reparentRenderable.GetFirstRenderSegment()->SetPrimitiveRange(8, 10);
		CalculateReparentVertices(trackData->currentPosition);
	}

	if (reparentVisible)
	{
		EditorViewport *viewport = editor->GetViewport(trackData->viewportIndex);
		viewport->SetToolRenderable(&reparentRenderable);
		viewport->GetViewportWidget()->InvalidateTexture();
	}

	return (true);
}

bool NodeReparentTool::EndTool(Editor *editor, EditorTrackData *trackData)
{
	EditorManipulator *manipulator = GetReparentNode(editor, trackData);
	if (manipulator)
	{
		editor->ReparentSelectedNodes(manipulator->GetTargetNode());
	}
	else
	{
		editor->InvalidateViewport(trackData->viewportIndex);
	}

	return (true);
}

// ZYUQURM
