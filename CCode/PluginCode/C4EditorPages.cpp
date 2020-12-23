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


#include "C4EditorPages.h"
#include "C4EditorSupport.h"
#include "C4WorldEditor.h"
#include "C4InstanceManipulators.h"
#include "C4MarkerManipulators.h"
#include "C4TerrainTools.h"
#include "C4Triggers.h"
#include "C4Fields.h"
#include "C4Blockers.h"
#include "C4Shafts.h"
#include "C4Water.h"
#include "C4World.h"


using namespace C4;


namespace
{
	const float kMinEditorGridSpacing = 0.001F;


	const TextureAlignMode alignModeTable[kTextureAlignModeCount] =
	{
		kTextureAlignNatural,
		kTextureAlignObjectPlane,
		kTextureAlignWorldPlane,
		kTextureAlignGlobalObjectPlane
	};


	enum
	{
		kTextureOperationNone,
		kTextureOperationOffset,
		kTextureOperationScale,
		kTextureOperationRotation,
		kTextureOperationReflection,
		kTextureOperationMode
	};
}


TypeWidget::TypeWidget(const Vector2D& size, const char *text, Type type) : TextWidget(size, text, "font/Normal")
{
	itemType = type;
}

TypeWidget::~TypeWidget()
{
}


EditorPage::EditorPage(PageType type, const char *panelName, int32 index) : Page(panelName)
{
	pageType = type;
	prevPageType = 0;

	bookIndex = index;
	pageState = kWidgetHidden;

	worldEditor = nullptr;
}

EditorPage::~EditorPage()
{
}

void EditorPage::Pack(Packer& data, unsigned_int32 packFlags) const
{
	data << ChunkHeader('PGST', 4);
	data << pageState;

	data << TerminatorChunk;
}

void EditorPage::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	UnpackChunkList<EditorPage>(data, unpackFlags);
}

bool EditorPage::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'PGST':

			data >> pageState;
			return (true);
	}

	return (false);
}

void EditorPage::SetWidgetState(unsigned_int32 state)
{
	pageState = state & kWidgetCollapsed;
	Page::SetWidgetState(state);
}

void EditorPage::Preprocess(void) 
{
	Page::Preprocess();
 
	if (pageState & kWidgetCollapsed)
	{ 
		Collapse();
	}
} 

 
GeometriesPage::GeometriesPage() : 
		EditorPage(kEditorPageGeometries, "WorldEditor/geometry/Geometries"),
		geometryButtonObserver(this, &GeometriesPage::HandleGeometryButtonEvent)
{
	currentTool = -1; 
}

GeometriesPage::~GeometriesPage()
{
}

void GeometriesPage::Preprocess(void)
{
	static const char *const buttonIdentifier[kEditorGeometryCount] =
	{
		"Plate", "Disk", "Hole", "Annulus", "Box", "Pyramid", "Cylinder", "Cone",
		"Sphere", "Dome", "Torus", "TruncCone", "Tube", "Extrusion", "Revolution", "Rope", "Cloth"
	};

	EditorPage::Preprocess();

	for (machine a = 0; a < kEditorGeometryCount; a++)
	{
		geometryButton[a] = static_cast<IconButtonWidget *>(FindWidget(buttonIdentifier[a]));
		geometryButton[a]->SetObserver(&geometryButtonObserver);
	}
}

void GeometriesPage::HandleGeometryButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		GetEditor()->SetCurrentTool(this, widget);
	}
}

void GeometriesPage::Engage(Editor *editor, void *cookie)
{
	Widget *widget = static_cast<Widget *>(cookie);

	for (machine a = 0; a < kEditorGeometryCount; a++)
	{
		if (widget == geometryButton[a])
		{
			currentTool = a;
			geometryButton[a]->SetValue(1);
			break;
		}
	}

	editor->SetCurrentCursor(TheWorldEditor->GetEditorCursor(kEditorCursorCross));
}

void GeometriesPage::Disengage(Editor *editor, void *cookie)
{
	if (currentTool != -1)
	{
		geometryButton[currentTool]->SetValue(0);
		currentTool = -1;
	}
}

bool GeometriesPage::BeginTool(Editor *editor, EditorTrackData *trackData)
{
	if (trackData->viewportType == kEditorViewportOrtho)
	{
		PrimitiveGeometry *geometry = nullptr;

		switch (currentTool)
		{
			case kEditorGeometryPlate:

				geometry = new PlateGeometry(Vector2D(kSizeEpsilon, kSizeEpsilon));
				break;

			case kEditorGeometryDisk:

				geometry = new DiskGeometry(Vector2D(kSizeEpsilon, kSizeEpsilon));
				break;

			case kEditorGeometryHole:

				geometry = new HoleGeometry(Vector2D(kSizeEpsilon, kSizeEpsilon), Vector2D(kSizeEpsilon * 0.5F, kSizeEpsilon * 0.5F));
				break;

			case kEditorGeometryAnnulus:

				geometry = new AnnulusGeometry(Vector2D(kSizeEpsilon, kSizeEpsilon), Vector2D(kSizeEpsilon * 0.5F, kSizeEpsilon * 0.5F));
				break;

			case kEditorGeometryBox:

				geometry = new BoxGeometry(Vector3D(kSizeEpsilon, kSizeEpsilon, kSizeEpsilon));
				break;

			case kEditorGeometryPyramid:

				geometry = new PyramidGeometry(Vector2D(kSizeEpsilon, kSizeEpsilon), kSizeEpsilon);
				break;

			case kEditorGeometryCylinder:

				geometry = new CylinderGeometry(Vector2D(kSizeEpsilon, kSizeEpsilon), kSizeEpsilon * 2.0F);
				break;

			case kEditorGeometryCone:

				geometry = new ConeGeometry(Vector2D(kSizeEpsilon, kSizeEpsilon), kSizeEpsilon * 2.0F);
				break;

			case kEditorGeometrySphere:

				geometry = new SphereGeometry(Vector3D(kSizeEpsilon, kSizeEpsilon, kSizeEpsilon));
				break;

			case kEditorGeometryDome:

				geometry = new DomeGeometry(Vector3D(kSizeEpsilon, kSizeEpsilon, kSizeEpsilon));
				break;

			case kEditorGeometryTorus:

				geometry = new TorusGeometry(Vector2D(kSizeEpsilon, kSizeEpsilon), kSizeEpsilon * 0.25F);
				break;

			case kEditorGeometryTruncatedCone:

				geometry = new TruncatedConeGeometry(Vector2D(kSizeEpsilon, kSizeEpsilon), kSizeEpsilon * 2.0F, 0.5F);
				break;

			case kEditorGeometryTube:
			case kEditorGeometryExtrusion:
			case kEditorGeometryRevolution:
			{
				const NodeReference *reference = editor->GetFirstSelectedNode();
				if ((reference) && (!reference->Next()))
				{
					Node *node = reference->GetNode();
					if (node->GetNodeType() == kNodeMarker)
					{
						Marker *marker = static_cast<Marker *>(node);
						if (marker->GetMarkerType() == kMarkerPath)
						{
							PathMarker *pathMarker = static_cast<PathMarker *>(marker);
							const Path *path = pathMarker->GetPath();

							if (currentTool == kEditorGeometryTube)
							{
								geometry = new TubeGeometry(path, Vector2D(kSizeEpsilon, kSizeEpsilon));
							}
							else if (currentTool == kEditorGeometryExtrusion)
							{
								geometry = new ExtrusionGeometry(path, Vector2D(kSizeEpsilon, kSizeEpsilon), kSizeEpsilon);
							}
							else
							{
								geometry = new RevolutionGeometry(path, Vector2D(kSizeEpsilon, kSizeEpsilon), kSizeEpsilon * 2.0F);
							}

							static_cast<PathPrimitiveGeometry *>(geometry)->SetConnectedPathMarker(pathMarker);
						}
					}
				}

				break;
			}

			case kEditorGeometryRope:

				geometry = new RopeGeometry(kSizeEpsilon, kSizeEpsilon, 16);
				geometry->GetObject()->SetPrimitiveFlags(0);
				break;

			case kEditorGeometryCloth:

				geometry = new ClothGeometry(Vector2D(kSizeEpsilon, kSizeEpsilon), 16, 16);
				break;
		}

		if (geometry)
		{
			if (!(editor->GetEditorObject()->GetEditorFlags() & kEditorCapGeometry))
			{
				geometry->GetObject()->SetPrimitiveFlags(0);
			}

			editor->InitNewNode(trackData, geometry);
			return (true);
		}
	}

	return (false);
}

bool GeometriesPage::TrackTool(Editor *editor, EditorTrackData *trackData)
{
	editor->AutoScroll(trackData);

	Point2D anchor = trackData->snappedAnchorPosition;
	float dx = trackData->snappedCurrentPosition.x - anchor.x;
	float dy = anchor.y - trackData->snappedCurrentPosition.y;

	float ax = Fabs(dx);
	float ay = Fabs(dy);

	if (InterfaceMgr::GetShiftKey())
	{
		ax = ay = Fmax(ax, ay);
		dx = (dx < 0.0F) ? -ax : ax;
		dy = (dy < 0.0F) ? -ax : ax;
	}

	if ((dx != trackData->currentSize.x) || (dy != trackData->currentSize.y))
	{
		trackData->currentSize.Set(dx, dy);

		unsigned_int32 editorFlags = editor->GetEditorObject()->GetEditorFlags();

		PrimitiveGeometry *geometry = static_cast<PrimitiveGeometry *>(trackData->trackNode);
		PrimitiveType primitiveType = geometry->GetPrimitiveType();

		if ((primitiveType == kPrimitivePlate) || (primitiveType == kPrimitiveBox) || (primitiveType == kPrimitivePyramid) || (primitiveType == kPrimitiveExtrusion) || (primitiveType == kPrimitiveCloth))
		{
			if (editorFlags & kEditorDrawFromCenter)
			{
				anchor.x -= ax;
				anchor.y += ay;
				dx = ax * 2.0F;
				dy = ay * 2.0F;
			}
			else
			{
				if (dx < 0.0F)
				{
					anchor.x += dx;
					dx = -dx;
				}

				if (dy < 0.0F)
				{
					anchor.y -= dy;
					dy = -dy;
				}
			}
		}
		else if (primitiveType == kPrimitiveRope)
		{
			if (editorFlags & kEditorDrawFromCenter)
			{
				anchor.x -= ax;
				dx = ax * 2.0F;
			}
			else
			{
				if (dx < 0.0F)
				{
					anchor.x += dx;
					dx = -dx;
				}
			}

			dy = Fabs(dy);
		}
		else
		{
			if (editorFlags & kEditorDrawFromCenter)
			{
				dx = ax;
				dy = ay;
			}
			else
			{
				anchor.x += dx * 0.5F;
				anchor.y -= dy * 0.5F;
				dx = ax * 0.5F;
				dy = ay * 0.5F;
			}
		}

		float sx = Fmax(dx, kSizeEpsilon);
		float sy = Fmax(dy, kSizeEpsilon);

		if (primitiveType != kPrimitiveTube)
		{
			geometry->SetNodePosition(editor->GetTargetSpacePosition(trackData, anchor));
		}

		switch (primitiveType)
		{
			case kPrimitivePlate:
			{
				const PlateGeometry *plate = static_cast<PlateGeometry *>(geometry);
				plate->GetObject()->SetPlateSize(Vector2D(sx, sy));
				break;
			}

			case kPrimitiveDisk:
			{
				const DiskGeometry *disk = static_cast<DiskGeometry *>(geometry);
				disk->GetObject()->SetDiskSize(Vector2D(sx, sy));
				break;
			}

			case kPrimitiveHole:
			{
				const HoleGeometry *hole = static_cast<HoleGeometry *>(geometry);
				HoleGeometryObject *object = hole->GetObject();
				object->SetOuterSize(Vector2D(sx, sy));
				object->SetInnerSize(Vector2D(sx * 0.5F, sy * 0.5F));
				break;
			}

			case kPrimitiveAnnulus:
			{
				const AnnulusGeometry *annulus = static_cast<AnnulusGeometry *>(geometry);
				AnnulusGeometryObject *object = annulus->GetObject();
				object->SetOuterSize(Vector2D(sx, sy));
				object->SetInnerSize(Vector2D(sx * 0.5F, sy * 0.5F));
				break;
			}

			case kPrimitiveBox:
			{
				const BoxGeometry *box = static_cast<BoxGeometry *>(geometry);
				box->GetObject()->SetBoxSize(Vector3D(sx, sy, Fmax(sx, sy)));
				break;
			}

			case kPrimitivePyramid:
			{
				const PyramidGeometry *cone = static_cast<PyramidGeometry *>(geometry);
				PyramidGeometryObject *object = cone->GetObject();
				object->SetPyramidSize(Vector2D(sx, sy));
				object->SetPyramidHeight(Fmax(sx, sy));
				break;
			}

			case kPrimitiveCylinder:
			{
				const CylinderGeometry *cylinder = static_cast<CylinderGeometry *>(geometry);
				CylinderGeometryObject *object = cylinder->GetObject();
				object->SetCylinderSize(Vector2D(sx, sy));
				object->SetCylinderHeight(Fmax(sx, sy) * 2.0F);
				break;
			}

			case kPrimitiveCone:
			{
				const ConeGeometry *cone = static_cast<ConeGeometry *>(geometry);
				ConeGeometryObject *object = cone->GetObject();
				object->SetConeSize(Vector2D(sx, sy));
				object->SetConeHeight(Fmax(sx, sy) * 2.0F);
				break;
			}

			case kPrimitiveTruncatedCone:
			{
				const TruncatedConeGeometry *cone = static_cast<TruncatedConeGeometry *>(geometry);
				TruncatedConeGeometryObject *object = cone->GetObject();
				object->SetConeSize(Vector2D(sx, sy));
				object->SetConeHeight(Fmax(sx, sy) * 2.0F);
				break;
			}

			case kPrimitiveSphere:
			{
				const SphereGeometry *sphere = static_cast<SphereGeometry *>(geometry);
				sphere->GetObject()->SetSphereSize(Vector3D(sx, sy, Fmax(sx, sy)));
				break;
			}

			case kPrimitiveDome:
			{
				const DomeGeometry *dome = static_cast<DomeGeometry *>(geometry);
				dome->GetObject()->SetDomeSize(Vector3D(sx, sy, Fmax(sx, sy)));
				break;
			}

			case kPrimitiveTorus:
			{
				const TorusGeometry *torus = static_cast<TorusGeometry *>(geometry);
				TorusGeometryObject *object = torus->GetObject();
				float r = Fmin(sx, sy) * 0.25F;
				object->SetPrimarySize(Vector2D(sx - r, sy - r));
				object->SetSecondaryRadius(r);
				break;
			}

			case kPrimitiveTube:
			{
				const TubeGeometry *tube = static_cast<TubeGeometry *>(geometry);
				tube->GetObject()->SetTubeSize(Vector2D(sx, sy));
				break;
			}

			case kPrimitiveExtrusion:
			{
				const ExtrusionGeometry *extrusion = static_cast<ExtrusionGeometry *>(geometry);
				ExtrusionGeometryObject *object = extrusion->GetObject();
				object->SetExtrusionSize(Vector2D(sx, sy));
				object->SetExtrusionHeight(Fmax(sx, sy));
				break;
			}

			case kPrimitiveRevolution:
			{
				const RevolutionGeometry *revolution = static_cast<RevolutionGeometry *>(geometry);
				RevolutionGeometryObject *object = revolution->GetObject();
				object->SetRevolutionSize(Vector2D(sx, sy));
				object->SetRevolutionHeight(Fmax(sx, sy) * 2.0F);
				break;
			}

			case kPrimitiveRope:
			{
				const RopeGeometry *rope = static_cast<RopeGeometry *>(geometry);
				RopeGeometryObject *object = rope->GetObject();
				object->SetRopeLength(sx);
				object->SetRopeRadius(sy);
				break;
			}

			case kPrimitiveCloth:
			{
				const ClothGeometry *cloth = static_cast<ClothGeometry *>(geometry);
				cloth->GetObject()->SetClothSize(Vector2D(sx, sy));
				break;
			}
		}

		editor->InvalidateNode(geometry);
		editor->GetRootNode()->Update();
		editor->RebuildGeometry(geometry);
	}

	return ((dx != 0.0F) && (dy != 0.0F));
}

bool GeometriesPage::EndTool(Editor *editor, EditorTrackData *trackData)
{
	editor->CommitNewNode(trackData, TrackTool(editor, trackData));
	return (true);
}


LightsPage::LightsPage() :
		EditorPage(kEditorPageLights, "WorldEditor/light/Lights"),
		editorObserver(this, &LightsPage::HandleEditorEvent),
		lightButtonObserver(this, &LightsPage::HandleLightButtonEvent),
		generateRadiosityButtonObserver(this, &LightsPage::HandleGenerateRadiosityButtonEvent)
{
	currentTool = -1;
}

LightsPage::~LightsPage()
{
}

void LightsPage::Preprocess(void)
{
	static const char *const lightButtonIdentifier[kEditorLightCount] =
	{
		"Infinite", "Point", "Cube", "Spot"
	};

	static const char *const spaceButtonIdentifier[kEditorSpaceCount] =
	{
		"Shadow", "Radiosity"
	};

	EditorPage::Preprocess();
	GetEditor()->AddObserver(&editorObserver);

	for (machine a = 0; a < kEditorLightCount; a++)
	{
		lightButton[a] = static_cast<IconButtonWidget *>(FindWidget(lightButtonIdentifier[a]));
		lightButton[a]->SetObserver(&lightButtonObserver);
	}

	for (machine a = 0; a < kEditorSpaceCount; a++)
	{
		spaceButton[a] = static_cast<IconButtonWidget *>(FindWidget(spaceButtonIdentifier[a]));
		spaceButton[a]->SetObserver(&lightButtonObserver);
	}

	generateRadiosityButton = static_cast<PushButtonWidget *>(FindWidget("Generate"));
	generateRadiosityButton->SetObserver(&generateRadiosityButtonObserver);
}

void LightsPage::HandleEditorEvent(Editor *editor, const EditorEvent& event)
{
	if (event.GetEventType() == kEditorEventSelectionUpdated)
	{
		const NodeReference *reference = editor->GetFirstSelectedNode();
		while (reference)
		{
			const Node *node = reference->GetNode();
			if (node->GetNodeType() == kNodeSpace)
			{
				const Space *space = static_cast<const Space *>(node);
				if (space->GetSpaceType() == kSpaceRadiosity)
				{
					generateRadiosityButton->Enable();
					return;
				}
			}

			reference = reference->Next();
		}

		generateRadiosityButton->Disable();
	}
}

void LightsPage::HandleLightButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		GetEditor()->SetCurrentTool(this, widget);
	}
}

void LightsPage::HandleGenerateRadiosityButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		Editor *editor = GetEditor();
		editor->SetWorldUnsavedFlag();
		editor->AddSubwindow(new GenerateRadiosityDataWindow(editor));
	}
}

void LightsPage::Engage(Editor *editor, void *cookie)
{
	Widget *widget = static_cast<Widget *>(cookie);

	for (machine a = 0; a < kEditorLightCount; a++)
	{
		if (widget == lightButton[a])
		{
			currentMode = kEditorLightModeLight;
			currentTool = a;

			lightButton[a]->SetValue(1);
			goto end;
		}
	}

	for (machine a = 0; a < kEditorSpaceCount; a++)
	{
		if (widget == spaceButton[a])
		{
			currentMode = kEditorLightModeSpace;
			currentTool = a;

			spaceButton[a]->SetValue(1);
			goto end;
		}
	}

	end:
	editor->SetCurrentCursor(TheWorldEditor->GetEditorCursor(kEditorCursorCross));
}

void LightsPage::Disengage(Editor *editor, void *cookie)
{
	if (currentTool != -1)
	{
		if (currentMode == kEditorLightModeLight)
		{
			lightButton[currentTool]->SetValue(0);
		}
		else if (currentMode == kEditorLightModeSpace)
		{
			spaceButton[currentTool]->SetValue(0);
		}

		currentTool = -1;
	}
}

bool LightsPage::BeginTool(Editor *editor, EditorTrackData *trackData)
{
	if (currentMode == kEditorLightModeLight)
	{
		Light *light = nullptr;

		switch (currentTool)
		{
			case kEditorLightInfinite:

				if ((trackData->viewportType != kEditorViewportGraph) || (trackData->superNode))
				{
					light = new InfiniteLight(K::white->GetColorRGB());
				}

				break;

			case kEditorLightPoint:

				if (trackData->viewportType == kEditorViewportOrtho)
				{
					light = new PointLight(K::white->GetColorRGB(), 0.0F);
				}

				break;

			case kEditorLightCube:

				if (trackData->viewportType == kEditorViewportOrtho)
				{
					light = new CubeLight(K::white->GetColorRGB(), 0.0F, "C4/cube");
				}

				break;

			case kEditorLightSpot:

				if (trackData->viewportType == kEditorViewportOrtho)
				{
					light = new SpotLight(K::white->GetColorRGB(), 0.0F, 1.0F, "C4/spot");
				}

				break;
		}

		if (light)
		{
			editor->InitNewNode(trackData, light);

			if (trackData->viewportType == kEditorViewportOrtho)
			{
				return (true);
			}

			editor->CommitNewNode(trackData, true);
		}
	}
	else if (currentMode == kEditorLightModeSpace)
	{
		if (trackData->viewportType == kEditorViewportOrtho)
		{
			Space *space = nullptr;

			switch (currentTool)
			{
				case kEditorSpaceShadow:

					space = new ShadowSpace(Zero3D);
					break;

				case kEditorSpaceRadiosity:

					space = new RadiositySpace(Zero3D, Integer3D(16, 16, 16));
					break;
			}

			if (space)
			{
				editor->InitNewNode(trackData, space);
				return (true);
			}
		}
	}

	return (false);
}

bool LightsPage::TrackTool(Editor *editor, EditorTrackData *trackData)
{
	editor->AutoScroll(trackData);

	if (currentMode == kEditorLightModeLight)
	{
		float dx = trackData->currentPosition.x - trackData->snappedAnchorPosition.x;
		float dy = trackData->snappedAnchorPosition.y - trackData->currentPosition.y;

		Light *light = static_cast<Light *>(trackData->trackNode);
		LightType lightType = light->GetLightType();

		if ((InterfaceMgr::GetShiftKey()) && (lightType != kLightPoint) && (lightType != kLightCube))
		{
			if (Fabs(dx) > Fabs(dy))
			{
				dy = 0.0F;
			}
			else
			{
				dx = 0.0F;
			}
		}

		float r2 = dx * dx + dy * dy;
		if ((dx != trackData->currentSize.x) || (dy != trackData->currentSize.y))
		{
			trackData->currentSize.Set(dx, dy);

			float range = Sqrt(r2);

			switch (lightType)
			{
				case kLightSpot:

					static_cast<SpotLightObject *>(light->GetObject())->SetLightRange(range);
					// no break

				case kLightInfinite:

					if (r2 != 0.0F)
					{
						Vector3D direction = editor->GetTargetSpaceDirection(trackData, Vector3D(dx, -dy, 0.0F));
						Vector3D down = editor->GetTargetSpaceDirection(trackData, Vector3D(0.0F, 0.0F, 1.0F));

						direction.Normalize();
						if (lightType != kLightSpot)
						{
							direction = -direction;
						}

						down = Normalize(down - ProjectOnto(down, direction));
						light->SetNodeMatrix3D(down % direction, down, direction);
					}

					break;

				case kLightPoint:
				case kLightCube:

					static_cast<PointLightObject *>(light->GetObject())->SetLightRange(range);
					break;
			}

			editor->InvalidateNode(light);
		}

		return (r2 != 0.0F);
	}
	else if (currentMode == kEditorLightModeSpace)
	{
		Point2D anchor = trackData->snappedAnchorPosition;
		float dx = trackData->snappedCurrentPosition.x - anchor.x;
		float dy = anchor.y - trackData->snappedCurrentPosition.y;

		float ax = Fabs(dx);
		float ay = Fabs(dy);

		if (InterfaceMgr::GetShiftKey())
		{
			ax = ay = Fmax(ax, ay);
			dx = (dx < 0.0F) ? -ax : ax;
			dy = (dy < 0.0F) ? -ax : ax;
		}

		if ((dx != trackData->currentSize.x) || (dy != trackData->currentSize.y))
		{
			trackData->currentSize.Set(dx, dy);

			unsigned_int32 editorFlags = editor->GetEditorObject()->GetEditorFlags();
			if (editorFlags & kEditorDrawFromCenter)
			{
				anchor.x -= ax;
				anchor.y += ay;
				dx = ax * 2.0F;
				dy = ay * 2.0F;
			}
			else
			{
				if (dx < 0.0F)
				{
					anchor.x += dx;
					dx = -dx;
				}

				if (dy < 0.0F)
				{
					anchor.y -= dy;
					dy = -dy;
				}
			}

			Space *space = static_cast<Space *>(trackData->trackNode);
			space->SetNodePosition(editor->GetTargetSpacePosition(trackData, anchor));

			BoxVolume *box = static_cast<BoxVolume *>(space->GetObject()->GetVolume());
			box->SetBoxSize(Vector3D(dx, dy, Fmax(dx, dy)));

			editor->InvalidateNode(space);
		}

		return ((dx != 0.0F) && (dy != 0.0F));
	}

	return (false);
}

bool LightsPage::EndTool(Editor *editor, EditorTrackData *trackData)
{
	editor->CommitNewNode(trackData, TrackTool(editor, trackData));
	return (true);
}


SourcesPage::SourcesPage() :
		EditorPage(kEditorPageSources, "WorldEditor/source/Sources"),
		sourceButtonObserver(this, &SourcesPage::HandleSourceButtonEvent)
{
	currentTool = -1;
}

SourcesPage::~SourcesPage()
{
}

void SourcesPage::Preprocess(void)
{
	static const char *const buttonIdentifier[kEditorSourceCount] =
	{
		"Ambient", "Omni", "Directed"
	};

	EditorPage::Preprocess();

	for (machine a = 0; a < kEditorSourceCount; a++)
	{
		sourceButton[a] = static_cast<IconButtonWidget *>(FindWidget(buttonIdentifier[a]));
		sourceButton[a]->SetObserver(&sourceButtonObserver);
	}

	acousticsSpaceButton = static_cast<IconButtonWidget *>(FindWidget("Acoustics"));
	acousticsSpaceButton->SetObserver(&sourceButtonObserver);
}

void SourcesPage::HandleSourceButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		GetEditor()->SetCurrentTool(this, widget);
	}
}

void SourcesPage::Engage(Editor *editor, void *cookie)
{
	Widget *widget = static_cast<Widget *>(cookie);

	if (widget == acousticsSpaceButton)
	{
		currentMode = kEditorSourceModeSpace;
		currentTool = 0;
		acousticsSpaceButton->SetValue(1);
	}
	else
	{
		for (machine a = 0; a < kEditorSourceCount; a++)
		{
			if (widget == sourceButton[a])
			{
				currentMode = kEditorSourceModeSource;
				currentTool = a;
				sourceButton[a]->SetValue(1);
				break;
			}
		}
	}

	editor->SetCurrentCursor(TheWorldEditor->GetEditorCursor(kEditorCursorCross));
}

void SourcesPage::Disengage(Editor *editor, void *cookie)
{
	if (currentTool != -1)
	{
		if (currentMode == kEditorSourceModeSource)
		{
			sourceButton[currentTool]->SetValue(0);
		}
		else if (currentMode == kEditorSourceModeSpace)
		{
			acousticsSpaceButton->SetValue(0);
		}

		currentTool = -1;
	}
}

bool SourcesPage::BeginTool(Editor *editor, EditorTrackData *trackData)
{
	if (trackData->viewportType == kEditorViewportOrtho)
	{
		if (currentMode == kEditorSourceModeSource)
		{
			Source *source = nullptr;

			switch (currentTool)
			{
				case kEditorSourceAmbient:

					source = new AmbientSource(nullptr, true);
					break;

				case kEditorSourceOmni:

					source = new OmniSource("C4/missing", 0.0F, true);
					break;

				case kEditorSourceDirected:

					source = new DirectedSource("C4/missing", 0.0F, 1.0F, true);
					break;
			}

			if (source)
			{
				SourceObject *object = source->GetObject();
				object->SetSourceFlags(object->GetSourceFlags() | kSourceLoop);

				editor->InitNewNode(trackData, source);
				return (true);
			}
		}
		else if (currentMode == kEditorSourceModeSpace)
		{
			Space *space = new AcousticsSpace(Zero3D);
			editor->InitNewNode(trackData, space);
			return (true);
		}
	}

	return (false);
}

bool SourcesPage::TrackTool(Editor *editor, EditorTrackData *trackData)
{
	editor->AutoScroll(trackData);

	if (currentMode == kEditorSourceModeSource)
	{
		Source *source = static_cast<Source *>(trackData->trackNode);
		SourceType sourceType = source->GetSourceType();

		if (sourceType != kSourceAmbient)
		{
			float dx = trackData->currentPosition.x - trackData->snappedAnchorPosition.x;
			float dy = trackData->snappedAnchorPosition.y - trackData->currentPosition.y;

			if ((InterfaceMgr::GetShiftKey()) && (sourceType != kSourceOmni))
			{
				if (Fabs(dx) > Fabs(dy))
				{
					dy = 0.0F;
				}
				else
				{
					dx = 0.0F;
				}
			}

			float r2 = dx * dx + dy * dy;
			if ((dx != trackData->currentSize.x) || (dy != trackData->currentSize.y))
			{
				trackData->currentSize.Set(dx, dy);
				static_cast<OmniSourceObject *>(source->GetObject())->SetSourceRange(Sqrt(r2));

				if ((sourceType == kSourceDirected) && (r2 != 0.0F))
				{
					Vector3D direction = editor->GetTargetSpaceDirection(trackData, Vector3D(dx, -dy, 0.0F));
					Vector3D down = editor->GetTargetSpaceDirection(trackData, Vector3D(0.0F, 0.0F, 1.0F));

					direction.Normalize();
					down = Normalize(down - ProjectOnto(down, direction));

					source->SetNodeMatrix3D(down % direction, down, direction);
				}

				editor->InvalidateNode(source);
			}

			return (r2 != 0.0F);
		}

		return (true);
	}

	Point2D anchor = trackData->snappedAnchorPosition;
	float dx = trackData->snappedCurrentPosition.x - anchor.x;
	float dy = anchor.y - trackData->snappedCurrentPosition.y;

	float ax = Fabs(dx);
	float ay = Fabs(dy);

	if (InterfaceMgr::GetShiftKey())
	{
		ax = ay = Fmax(ax, ay);
		dx = (dx < 0.0F) ? -ax : ax;
		dy = (dy < 0.0F) ? -ax : ax;
	}

	if ((dx != trackData->currentSize.x) || (dy != trackData->currentSize.y))
	{
		trackData->currentSize.Set(dx, dy);

		unsigned_int32 editorFlags = editor->GetEditorObject()->GetEditorFlags();
		if (editorFlags & kEditorDrawFromCenter)
		{
			anchor.x -= ax;
			anchor.y += ay;
			dx = ax * 2.0F;
			dy = ay * 2.0F;
		}
		else
		{
			if (dx < 0.0F)
			{
				anchor.x += dx;
				dx = -dx;
			}

			if (dy < 0.0F)
			{
				anchor.y -= dy;
				dy = -dy;
			}
		}

		Space *space = static_cast<Space *>(trackData->trackNode);
		space->SetNodePosition(editor->GetTargetSpacePosition(trackData, anchor));

		BoxVolume *box = static_cast<BoxVolume *>(space->GetObject()->GetVolume());
		box->SetBoxSize(Vector3D(dx, dy, Fmax(dx, dy)));

		editor->InvalidateNode(space);
	}

	return ((dx != 0.0F) && (dy != 0.0F));
}

bool SourcesPage::EndTool(Editor *editor, EditorTrackData *trackData)
{
	editor->CommitNewNode(trackData, TrackTool(editor, trackData));
	return (true);
}


ZonesPage::ZonesPage() :
		EditorPage(kEditorPageZones, "WorldEditor/zone/Zones"),
		zoneButtonObserver(this, &ZonesPage::HandleZoneButtonEvent)
{
	currentTool = -1;
}

ZonesPage::~ZonesPage()
{
}

void ZonesPage::Preprocess(void)
{
	static const char *const zoneButtonIdentifier[kEditorZoneCount] =
	{
		"Box", "Cylinder", "Polygon"
	};

	static const char *const toolButtonIdentifier[kEditorZoneToolCount] =
	{
		"Insert", "Remove"
	};

	EditorPage::Preprocess();

	for (machine a = 0; a < kEditorZoneCount; a++)
	{
		zoneButton[a] = static_cast<IconButtonWidget *>(FindWidget(zoneButtonIdentifier[a]));
		zoneButton[a]->SetObserver(&zoneButtonObserver);
	}

	for (machine a = 0; a < kEditorZoneToolCount; a++)
	{
		toolButton[a] = static_cast<IconButtonWidget *>(FindWidget(toolButtonIdentifier[a]));
		toolButton[a]->SetObserver(&zoneButtonObserver);
	}
}

void ZonesPage::HandleZoneButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		GetEditor()->SetCurrentTool(this, widget);
	}
}

void ZonesPage::Engage(Editor *editor, void *cookie)
{
	Widget *widget = static_cast<Widget *>(cookie);

	for (machine a = 0; a < kEditorZoneCount; a++)
	{
		if (widget == zoneButton[a])
		{
			currentMode = kEditorZoneModeDraw;
			currentTool = a;
			zoneButton[a]->SetValue(1);

			editor->SetCurrentCursor(TheWorldEditor->GetEditorCursor(kEditorCursorCross));
			return;
		}
	}

	for (machine a = 0; a < kEditorZoneToolCount; a++)
	{
		if (widget == toolButton[a])
		{
			currentMode = kEditorZoneModeTool;
			currentTool = a;
			toolButton[a]->SetValue(1);

			editor->SetRenderFlags(editor->GetRenderFlags() | kEditorRenderHandles);
			editor->SetCurrentCursor(TheWorldEditor->GetEditorCursor((a == 0) ? kEditorCursorInsert : kEditorCursorRemove));
			return;
		}
	}
}

void ZonesPage::Disengage(Editor *editor, void *cookie)
{
	if (currentTool != -1)
	{
		if (currentMode == kEditorZoneModeDraw)
		{
			zoneButton[currentTool]->SetValue(0);
		}
		else
		{
			toolButton[currentTool]->SetValue(0);
			editor->SetRenderFlags(editor->GetRenderFlags() & ~kEditorRenderHandles);
		}

		currentTool = -1;
	}
}

bool ZonesPage::BeginTool(Editor *editor, EditorTrackData *trackData)
{
	if (currentMode == kEditorZoneModeDraw)
	{
		if (trackData->viewportType == kEditorViewportOrtho)
		{
			Zone *zone = nullptr;

			switch (currentTool)
			{
				case kEditorZoneBox:

					zone = new BoxZone(Zero3D);
					break;

				case kEditorZoneCylinder:

					zone = new CylinderZone(Zero2D, 0.0F);
					break;

				case kEditorZonePolygon:

					zone = new PolygonZone(Zero2D, 0.0F);
					break;
			}

			if (zone)
			{
				editor->InitNewNode(trackData, zone);
				return (true);
			}
		}
	}
	else
	{
		int32	handleIndex;

		Node *handleNode = editor->PickHandle(trackData, &handleIndex);

		if (currentTool == kEditorZoneToolInsert)
		{
			if (!handleNode)
			{
				PickData	pickData;

				auto filter = [](const Node *node, const PickData *, const void *) -> bool
				{
					return (node->GetNodeType() == kNodeZone);
				};

				Node *selectNode = editor->PickNode(trackData, &pickData, filter);
				if (selectNode)
				{
					editor->UnselectAll();
					editor->SelectNode(selectNode);

					Zone *zone = static_cast<Zone *>(selectNode);
					if (zone->GetZoneType() == kZonePolygon)
					{
						PolygonZone *polygon = static_cast<PolygonZone *>(zone);
						PolygonZoneObject *object = polygon->GetObject();

						int32 vertexCount = object->GetVertexCount();
						if (vertexCount < kMaxZoneVertexCount)
						{
							int32 index = pickData.pickIndex[0];
							if (index >= 0)
							{
								editor->AddOperation(new ZoneVertexOperation(polygon));

								Point3D *vertex = object->GetVertexArray();
								for (machine a = vertexCount - 1; a >= index; a--)
								{
									vertex[a + 1] = vertex[a];
								}

								object->SetVertexCount(vertexCount + 1);
								vertex[index] = pickData.pickPoint;
								polygon->Invalidate();

								trackData->resizeData.resizeFlags = 0;
								trackData->resizeData.handleFlags = 0;
								trackData->resizeData.handleIndex = index;

								Editor::GetManipulator(polygon)->BeginResize(&trackData->resizeData);
								trackData->trackNode = polygon;
								return (true);
							}
						}
					}
				}
			}
		}
		else if (currentTool == kEditorZoneToolRemove)
		{
			if ((handleNode) && (handleNode->GetNodeType() == kNodeZone))
			{
				Zone *zone = static_cast<Zone *>(handleNode);
				if (zone->GetZoneType() == kZonePolygon)
				{
					PolygonZone *polygon = static_cast<PolygonZone *>(zone);
					PolygonZoneObject *object = polygon->GetObject();

					int32 vertexCount = object->GetVertexCount();
					if (vertexCount > 3)
					{
						editor->AddOperation(new ZoneVertexOperation(polygon));

						Point3D *vertex = object->GetVertexArray();
						for (machine a = handleIndex + 1; a < vertexCount; a++)
						{
							vertex[a - 1] = vertex[a];
						}

						object->SetVertexCount(vertexCount - 1);
						polygon->Invalidate();
					}
				}
			}
		}
	}

	return (false);
}

bool ZonesPage::TrackTool(Editor *editor, EditorTrackData *trackData)
{
	if (currentMode == kEditorZoneModeDraw)
	{
		editor->AutoScroll(trackData);

		Point2D anchor = trackData->snappedAnchorPosition;
		float dx = trackData->snappedCurrentPosition.x - anchor.x;
		float dy = anchor.y - trackData->snappedCurrentPosition.y;

		float ax = Fabs(dx);
		float ay = Fabs(dy);

		if (InterfaceMgr::GetShiftKey())
		{
			ax = ay = Fmax(ax, ay);
			dx = (dx < 0.0F) ? -ax : ax;
			dy = (dy < 0.0F) ? -ax : ax;
		}

		if ((dx != trackData->currentSize.x) || (dy != trackData->currentSize.y))
		{
			trackData->currentSize.Set(dx, dy);

			unsigned_int32 editorFlags = editor->GetEditorObject()->GetEditorFlags();

			Zone *zone = static_cast<Zone *>(trackData->trackNode);
			ZoneType zoneType = zone->GetZoneType();

			if ((zoneType == kZoneBox) || (zoneType == kZonePolygon))
			{
				if (editorFlags & kEditorDrawFromCenter)
				{
					anchor.x -= ax;
					anchor.y += ay;
					dx = ax * 2.0F;
					dy = ay * 2.0F;
				}
				else
				{
					if (dx < 0.0F)
					{
						anchor.x += dx;
						dx = -dx;
					}

					if (dy < 0.0F)
					{
						anchor.y -= dy;
						dy = -dy;
					}
				}

				zone->SetNodePosition(editor->GetTargetSpacePosition(trackData, anchor));

				if (zoneType == kZoneBox)
				{
					const BoxZone *box = static_cast<BoxZone *>(zone);
					box->GetObject()->SetBoxSize(Vector3D(dx, dy, Fmax(dx, dy)));
				}
				else
				{
					const PolygonZone *polygon = static_cast<PolygonZone *>(zone);
					polygon->GetObject()->SetPolygonSize(Vector2D(dx, dy), Fmax(dx, dy));
				}
			}
			else
			{
				if (editorFlags & kEditorDrawFromCenter)
				{
					dx = ax;
					dy = ay;
				}
				else
				{
					anchor.x += dx * 0.5F;
					anchor.y -= dy * 0.5F;
					dx = ax * 0.5F;
					dy = ay * 0.5F;

					zone->SetNodePosition(editor->GetTargetSpacePosition(trackData, anchor));
				}

				const CylinderZone *cylinder = static_cast<CylinderZone *>(zone);
				CylinderZoneObject *object = cylinder->GetObject();
				object->SetCylinderSize(Vector2D(dx, dy));
				object->SetCylinderHeight(Fmax(dx, dy) * 2.0F);
			}

			editor->InvalidateNode(zone);
		}

		return ((dx != 0.0F) && (dy != 0.0F));
	}

	if (currentTool == kEditorZoneToolInsert)
	{
		editor->AutoScroll(trackData);

		if (trackData->currentPosition != trackData->previousPosition)
		{
			PolygonZone *polygon = static_cast<PolygonZone *>(trackData->trackNode);

			Vector2D delta = trackData->currentPosition - trackData->anchorPosition;
			trackData->resizeData.resizeDelta = polygon->GetInverseWorldTransform() * editor->GetWorldSpaceDirection(trackData, Vector3D(delta.x, delta.y, 0.0F));
			Editor::GetManipulator(polygon)->Resize(&trackData->resizeData);
			editor->InvalidateNode(polygon);
		}
	}

	return (true);
}

bool ZonesPage::EndTool(Editor *editor, EditorTrackData *trackData)
{
	bool result = TrackTool(editor, trackData);

	if (currentMode == kEditorZoneModeDraw)
	{
		editor->CommitNewNode(trackData, result);
	}

	editor->InvalidateNode(editor->GetRootNode());
	return (true);
}


PortalsPage::PortalsPage() :
		EditorPage(kEditorPagePortals, "WorldEditor/portal/Portals"),
		editorObserver(this, &PortalsPage::HandleEditorEvent),
		portalButtonObserver(this, &PortalsPage::HandlePortalButtonEvent),
		createOpposingButtonObserver(this, &PortalsPage::HandleCreateOpposingButtonEvent)
{
	currentTool = -1;
}

PortalsPage::~PortalsPage()
{
}

void PortalsPage::Preprocess(void)
{
	static const char *const portalButtonIdentifier[kEditorPortalCount] =
	{
		"Direct", "Remote", "Occlusion"
	};

	static const char *const toolButtonIdentifier[kEditorPortalToolCount] =
	{
		"Insert", "Remove"
	};

	EditorPage::Preprocess();
	GetEditor()->AddObserver(&editorObserver);

	for (machine a = 0; a < kEditorPortalCount; a++)
	{
		portalButton[a] = static_cast<IconButtonWidget *>(FindWidget(portalButtonIdentifier[a]));
		portalButton[a]->SetObserver(&portalButtonObserver);
	}

	for (machine a = 0; a < kEditorPortalToolCount; a++)
	{
		toolButton[a] = static_cast<IconButtonWidget *>(FindWidget(toolButtonIdentifier[a]));
		toolButton[a]->SetObserver(&portalButtonObserver);
	}

	occlusionSpaceButton = static_cast<IconButtonWidget *>(FindWidget("Space"));
	occlusionSpaceButton->SetObserver(&portalButtonObserver);

	createOpposingButton = static_cast<PushButtonWidget *>(FindWidget("Opposing"));
	createOpposingButton->SetObserver(&createOpposingButtonObserver);
}

void PortalsPage::HandleEditorEvent(Editor *editor, const EditorEvent& event)
{
	if (event.GetEventType() == kEditorEventSelectionUpdated)
	{
		const NodeReference *reference = editor->GetFirstSelectedNode();
		while (reference)
		{
			const Node *node = reference->GetNode();
			if (node->GetNodeType() == kNodePortal)
			{
				const Portal *portal = static_cast<const Portal *>(node);
				if ((portal->GetPortalType() == kPortalDirect) && (portal->GetConnectedZone()))
				{
					createOpposingButton->Enable();
					return;
				}
			}

			reference = reference->Next();
		}

		createOpposingButton->Disable();
	}
}

void PortalsPage::HandlePortalButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		GetEditor()->SetCurrentTool(this, widget);
	}
}

void PortalsPage::HandleCreateOpposingButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		List<NodeReference>		newPortalList;

		Editor *editor = GetEditor();

		const NodeReference *reference = editor->GetFirstSelectedNode();
		while (reference)
		{
			const Node *node = reference->GetNode();
			if (node->GetNodeType() == kNodePortal)
			{
				const Portal *portal = static_cast<const Portal *>(node);
				if (portal->GetPortalType() == kPortalDirect)
				{
					Zone *zone = portal->GetConnectedZone();
					if (zone)
					{
						const PortalObject *object = portal->GetObject();
						int32 vertexCount = object->GetVertexCount();

						Portal *opposingPortal = new DirectPortal(vertexCount);
						opposingPortal->SetConnectedZone(portal->GetOwningZone());

						Transform4D transform = zone->GetInverseWorldTransform() * portal->GetWorldTransform();
						opposingPortal->SetNodeTransform(Transform4D(-transform[0], transform[1], -transform[2], transform.GetTranslation()));

						PortalObject *opposingObject = opposingPortal->GetObject();
						opposingObject->SetPortalFlags(object->GetPortalFlags());

						const Point3D *vertex = object->GetVertexArray();
						Point3D *opposingVertex = opposingObject->GetVertexArray() + (vertexCount - 1);

						for (machine a = 0; a < vertexCount; a++)
						{
							opposingVertex->Set(-vertex->x, vertex->y, -vertex->z);

							vertex++;
							opposingVertex--;
						}

						EditorManipulator::Install(editor, opposingPortal);
						zone->AppendNewSubnode(opposingPortal);

						editor->PostEvent(NodeEditorEvent(kEditorEventNodeCreated, opposingPortal));
						Editor::GetManipulator(opposingPortal)->InvalidateGraph();

						newPortalList.Append(new NodeReference(opposingPortal));
					}
				}
			}

			reference = reference->Next();
		}

		if (!newPortalList.Empty())
		{
			editor->UnselectAll();
			editor->AddOperation(new CreateOperation(&newPortalList));

			reference = newPortalList.First();
			while (reference)
			{
				editor->SelectNode(reference->GetNode());
				reference = reference->Next();
			}
		}
	}
}

void PortalsPage::Engage(Editor *editor, void *cookie)
{
	Widget *widget = static_cast<Widget *>(cookie);

	for (machine a = 0; a < kEditorPortalCount; a++)
	{
		if (widget == portalButton[a])
		{
			currentMode = kEditorPortalModePortal;
			currentTool = a;
			portalButton[a]->SetValue(1);

			editor->SetCurrentCursor(TheWorldEditor->GetEditorCursor(kEditorCursorCross));
			return;
		}
	}

	if (widget == occlusionSpaceButton)
	{
		currentMode = kEditorPortalModeSpace;
		currentTool = 0;
		occlusionSpaceButton->SetValue(1);

		editor->SetCurrentCursor(TheWorldEditor->GetEditorCursor(kEditorCursorCross));
		return;
	}

	for (machine a = 0; a < kEditorPortalToolCount; a++)
	{
		if (widget == toolButton[a])
		{
			currentMode = kEditorPortalModeTool;
			currentTool = a;
			toolButton[a]->SetValue(1);

			editor->SetRenderFlags(editor->GetRenderFlags() | kEditorRenderHandles);
			editor->SetCurrentCursor(TheWorldEditor->GetEditorCursor((a == 0) ? kEditorCursorInsert : kEditorCursorRemove));
			return;
		}
	}
}

void PortalsPage::Disengage(Editor *editor, void *cookie)
{
	if (currentTool != -1)
	{
		if (currentMode == kEditorPortalModePortal)
		{
			portalButton[currentTool]->SetValue(0);
		}
		else if (currentMode == kEditorPortalModePortal)
		{
			occlusionSpaceButton->SetValue(0);
		}
		else if (currentMode == kEditorPortalModeTool)
		{
			toolButton[currentTool]->SetValue(0);
			editor->SetRenderFlags(editor->GetRenderFlags() & ~kEditorRenderHandles);
		}

		currentTool = -1;
	}
}

bool PortalsPage::BeginTool(Editor *editor, EditorTrackData *trackData)
{
	if (currentMode == kEditorPortalModePortal)
	{
		if (trackData->viewportType == kEditorViewportOrtho)
		{
			Portal *portal = nullptr;

			switch (currentTool)
			{
				case kEditorPortalDirect:

					portal = new DirectPortal(Zero2D);
					portal->AddConnector(kConnectorKeyZone);
					break;

				case kEditorPortalRemote:

					portal = new RemotePortal(Zero2D);
					break;

				case kEditorPortalOcclusion:

					portal = new OcclusionPortal(Zero2D);
					break;
			}

			if (portal)
			{
				editor->InitNewNode(trackData, portal);
				return (true);
			}
		}
	}
	else if (currentMode == kEditorPortalModeSpace)
	{
		if (trackData->viewportType == kEditorViewportOrtho)
		{
			Space *space = new OcclusionSpace(Zero3D);
			editor->InitNewNode(trackData, space);
			return (true);
		}
	}
	else if (currentMode == kEditorPortalModeTool)
	{
		int32	handleIndex;

		Node *handleNode = editor->PickHandle(trackData, &handleIndex);

		if (currentTool == kEditorPortalToolInsert)
		{
			if (!handleNode)
			{
				PickData	pickData;

				auto filter = [](const Node *node, const PickData *, const void *) -> bool
				{
					return (node->GetNodeType() == kNodePortal);
				};

				Node *selectNode = editor->PickNode(trackData, &pickData, filter);
				if (selectNode)
				{
					editor->UnselectAll();
					editor->SelectNode(selectNode);

					Portal *portal = static_cast<Portal *>(selectNode);
					PortalObject *object = portal->GetObject();

					int32 vertexCount = object->GetVertexCount();
					if (vertexCount < kMaxPortalVertexCount)
					{
						int32 index = pickData.pickIndex[0];
						if (index >= 0)
						{
							editor->AddOperation(new PortalVertexOperation(portal));

							Point3D *vertex = object->GetVertexArray();
							for (machine a = vertexCount - 1; a >= index; a--)
							{
								vertex[a + 1] = vertex[a];
							}

							object->SetVertexCount(vertexCount + 1);
							vertex[index] = pickData.pickPoint;
							portal->Invalidate();

							trackData->resizeData.resizeFlags = 0;
							trackData->resizeData.handleFlags = 0;
							trackData->resizeData.handleIndex = index;

							Editor::GetManipulator(portal)->BeginResize(&trackData->resizeData);
							trackData->trackNode = portal;
							return (true);
						}
					}
				}
			}
		}
		else if (currentTool == kEditorPortalToolRemove)
		{
			if ((handleNode) && (handleNode->GetNodeType() == kNodePortal))
			{
				Portal *portal = static_cast<Portal *>(handleNode);
				PortalObject *object = portal->GetObject();

				int32 vertexCount = object->GetVertexCount();
				if (vertexCount > 3)
				{
					editor->AddOperation(new PortalVertexOperation(portal));

					Point3D *vertex = object->GetVertexArray();
					for (machine a = handleIndex + 1; a < vertexCount; a++)
					{
						vertex[a - 1] = vertex[a];
					}

					object->SetVertexCount(vertexCount - 1);
					portal->Invalidate();
				}
			}
		}
	}

	return (false);
}

bool PortalsPage::TrackTool(Editor *editor, EditorTrackData *trackData)
{
	editor->AutoScroll(trackData);

	if (currentMode == kEditorPortalModePortal)
	{
		Point2D anchor = trackData->snappedAnchorPosition;
		float dx = trackData->snappedCurrentPosition.x - anchor.x;
		float dy = anchor.y - trackData->snappedCurrentPosition.y;

		float ax = Fabs(dx);
		float ay = Fabs(dy);

		if (InterfaceMgr::GetShiftKey())
		{
			ax = ay = Fmax(ax, ay);
			dx = (dx < 0.0F) ? -ax : ax;
			dy = (dy < 0.0F) ? -ax : ax;
		}

		if ((dx != trackData->currentSize.x) || (dy != trackData->currentSize.y))
		{
			trackData->currentSize.Set(dx, dy);

			unsigned_int32 editorFlags = editor->GetEditorObject()->GetEditorFlags();
			if (editorFlags & kEditorDrawFromCenter)
			{
				anchor.x -= ax;
				anchor.y += ay;
				dx = ax * 2.0F;
				dy = ay * 2.0F;
			}
			else
			{
				if (dx < 0.0F)
				{
					anchor.x += dx;
					dx = -dx;
				}

				if (dy < 0.0F)
				{
					anchor.y -= dy;
					dy = -dy;
				}
			}

			Portal *portal = static_cast<Portal *>(trackData->trackNode);
			portal->SetNodePosition(editor->GetTargetSpacePosition(trackData, anchor));
			portal->GetObject()->SetPortalSize(Vector2D(dx, dy));
			editor->InvalidateNode(portal);
		}

		return ((dx != 0.0F) && (dy != 0.0F));
	}
	else if (currentMode == kEditorPortalModeSpace)
	{
		Point2D anchor = trackData->snappedAnchorPosition;
		float dx = trackData->snappedCurrentPosition.x - anchor.x;
		float dy = anchor.y - trackData->snappedCurrentPosition.y;

		float ax = Fabs(dx);
		float ay = Fabs(dy);

		if (InterfaceMgr::GetShiftKey())
		{
			ax = ay = Fmax(ax, ay);
			dx = (dx < 0.0F) ? -ax : ax;
			dy = (dy < 0.0F) ? -ax : ax;
		}

		if ((dx != trackData->currentSize.x) || (dy != trackData->currentSize.y))
		{
			trackData->currentSize.Set(dx, dy);

			unsigned_int32 editorFlags = editor->GetEditorObject()->GetEditorFlags();
			if (editorFlags & kEditorDrawFromCenter)
			{
				anchor.x -= ax;
				anchor.y += ay;
				dx = ax * 2.0F;
				dy = ay * 2.0F;
			}
			else
			{
				if (dx < 0.0F)
				{
					anchor.x += dx;
					dx = -dx;
				}

				if (dy < 0.0F)
				{
					anchor.y -= dy;
					dy = -dy;
				}
			}

			Space *space = static_cast<Space *>(trackData->trackNode);
			space->SetNodePosition(editor->GetTargetSpacePosition(trackData, anchor));

			BoxVolume *box = static_cast<BoxVolume *>(space->GetObject()->GetVolume());
			box->SetBoxSize(Vector3D(dx, dy, Fmax(dx, dy)));

			editor->InvalidateNode(space);
		}

		return ((dx != 0.0F) && (dy != 0.0F));
	}
	else if (currentMode == kEditorPortalModeTool)
	{
		if (currentTool == kEditorPortalToolInsert)
		{
			if (trackData->currentPosition != trackData->previousPosition)
			{
				Portal *portal = static_cast<Portal *>(trackData->trackNode);

				Vector2D delta = trackData->currentPosition - trackData->anchorPosition;
				trackData->resizeData.resizeDelta = portal->GetInverseWorldTransform() * editor->GetWorldSpaceDirection(trackData, Vector3D(delta.x, delta.y, 0.0F));
				Editor::GetManipulator(portal)->Resize(&trackData->resizeData);
				editor->InvalidateNode(portal);
			}
		}
	}

	return (true);
}

bool PortalsPage::EndTool(Editor *editor, EditorTrackData *trackData)
{
	bool result = TrackTool(editor, trackData);

	if ((currentMode == kEditorPortalModePortal) || (currentMode == kEditorPortalModeSpace))
	{
		editor->CommitNewNode(trackData, result);
	}

	return (true);
}


TriggersPage::TriggersPage() :
		EditorPage(kEditorPageTriggers, "WorldEditor/trigger/Triggers"),
		triggerButtonObserver(this, &TriggersPage::HandleTriggerButtonEvent)
{
	currentTool = -1;
}

TriggersPage::~TriggersPage()
{
}

void TriggersPage::Preprocess(void)
{
	static const char *const buttonIdentifier[kEditorTriggerCount] =
	{
		"Box", "Cylinder", "Sphere"
	};

	EditorPage::Preprocess();

	for (machine a = 0; a < kEditorTriggerCount; a++)
	{
		triggerButton[a] = static_cast<IconButtonWidget *>(FindWidget(buttonIdentifier[a]));
		triggerButton[a]->SetObserver(&triggerButtonObserver);
	}
}

void TriggersPage::HandleTriggerButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		GetEditor()->SetCurrentTool(this, widget);
	}
}

void TriggersPage::Engage(Editor *editor, void *cookie)
{
	Widget *widget = static_cast<Widget *>(cookie);

	for (machine a = 0; a < kEditorTriggerCount; a++)
	{
		if (widget == triggerButton[a])
		{
			currentTool = a;
			triggerButton[a]->SetValue(1);
			break;
		}
	}

	editor->SetCurrentCursor(TheWorldEditor->GetEditorCursor(kEditorCursorCross));
}

void TriggersPage::Disengage(Editor *editor, void *cookie)
{
	if (currentTool != -1)
	{
		triggerButton[currentTool]->SetValue(0);
		currentTool = -1;
	}
}

bool TriggersPage::BeginTool(Editor *editor, EditorTrackData *trackData)
{
	if (trackData->viewportType == kEditorViewportOrtho)
	{
		Trigger *trigger = nullptr;

		switch (currentTool)
		{
			case kEditorTriggerBox:

				trigger = new BoxTrigger(Zero3D);
				break;

			case kEditorTriggerCylinder:

				trigger = new CylinderTrigger(Zero2D, 0.0F);
				break;

			case kEditorTriggerSphere:

				trigger = new SphereTrigger(Zero3D);
				break;
		}

		if (trigger)
		{
			editor->InitNewNode(trackData, trigger);
			return (true);
		}
	}

	return (false);
}

bool TriggersPage::TrackTool(Editor *editor, EditorTrackData *trackData)
{
	editor->AutoScroll(trackData);

	Point2D anchor = trackData->snappedAnchorPosition;
	float dx = trackData->snappedCurrentPosition.x - anchor.x;
	float dy = anchor.y - trackData->snappedCurrentPosition.y;

	float ax = Fabs(dx);
	float ay = Fabs(dy);

	if (InterfaceMgr::GetShiftKey())
	{
		ax = ay = Fmax(ax, ay);
		dx = (dx < 0.0F) ? -ax : ax;
		dy = (dy < 0.0F) ? -ax : ax;
	}

	if ((dx != trackData->currentSize.x) || (dy != trackData->currentSize.y))
	{
		trackData->currentSize.Set(dx, dy);

		unsigned_int32 editorFlags = editor->GetEditorObject()->GetEditorFlags();

		Trigger *trigger = static_cast<Trigger *>(trackData->trackNode);
		TriggerType triggerType = trigger->GetTriggerType();

		if (triggerType == kTriggerBox)
		{
			if (editorFlags & kEditorDrawFromCenter)
			{
				anchor.x -= ax;
				anchor.y += ay;
				dx = ax * 2.0F;
				dy = ay * 2.0F;
			}
			else
			{
				if (dx < 0.0F)
				{
					anchor.x += dx;
					dx = -dx;
				}

				if (dy < 0.0F)
				{
					anchor.y -= dy;
					dy = -dy;
				}
			}

			trigger->SetNodePosition(editor->GetTargetSpacePosition(trackData, anchor));

			const BoxTrigger *box = static_cast<BoxTrigger *>(trigger);
			box->GetObject()->SetBoxSize(Vector3D(dx, dy, Fmax(dx, dy)));
		}
		else
		{
			if (editorFlags & kEditorDrawFromCenter)
			{
				dx = ax;
				dy = ay;
			}
			else
			{
				anchor.x += dx * 0.5F;
				anchor.y -= dy * 0.5F;
				dx = ax * 0.5F;
				dy = ay * 0.5F;

				trigger->SetNodePosition(editor->GetTargetSpacePosition(trackData, anchor));
			}

			switch (triggerType)
			{
				case kTriggerCylinder:
				{
					const CylinderTrigger *cylinder = static_cast<CylinderTrigger *>(trigger);
					CylinderTriggerObject *object = cylinder->GetObject();
					object->SetCylinderSize(Vector2D(dx, dy));
					object->SetCylinderHeight(Fmax(dx, dy) * 2.0F);
					break;
				}

				case kTriggerSphere:
				{
					const SphereTrigger *sphere = static_cast<SphereTrigger *>(trigger);
					SphereTriggerObject *object = sphere->GetObject();
					object->SetSphereSize(Vector3D(dx, dy, Fmax(dx, dy)));
					break;
				}
			}
		}

		editor->InvalidateNode(trigger);
	}

	return ((dx != 0.0F) && (dy != 0.0F));
}

bool TriggersPage::EndTool(Editor *editor, EditorTrackData *trackData)
{
	editor->CommitNewNode(trackData, TrackTool(editor, trackData));
	return (true);
}


MarkersPage::MarkersPage() :
		EditorPage(kEditorPageMarkers, "WorldEditor/marker/Markers"),
		markerButtonObserver(this, &MarkersPage::HandleMarkerButtonEvent)
{
	currentTool = -1;
}

MarkersPage::~MarkersPage()
{
}

void MarkersPage::Preprocess(void)
{
	static const char *const buttonIdentifier[kEditorMarkerCount] =
	{
		"Locator", "Connection", "Cube", "Shader"
	};

	EditorPage::Preprocess();

	for (machine a = 0; a < kEditorMarkerCount; a++)
	{
		markerButton[a] = static_cast<IconButtonWidget *>(FindWidget(buttonIdentifier[a]));
		markerButton[a]->SetObserver(&markerButtonObserver);
	}

	locatorList = static_cast<ListWidget *>(FindWidget("List"));
	locatorList->SetObserver(&markerButtonObserver);
	locatorList->SetWidgetUsage(locatorList->GetWidgetUsage() & ~kWidgetKeyboardFocus);

	Vector2D size = locatorList->GetNaturalListItemSize();

	const LocatorRegistration *registration = LocatorMarker::GetFirstRegistration();
	while (registration)
	{
		locatorList->InsertSortedListItem(new TypeWidget(size, registration->GetLocatorName(), registration->GetLocatorType()));
		registration = registration->Next();
	}
}

void MarkersPage::HandleMarkerButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		GetEditor()->SetCurrentTool(this, widget);
	}
}

void MarkersPage::Engage(Editor *editor, void *cookie)
{
	Widget *widget = static_cast<Widget *>(cookie);

	currentTool = -1;
	for (machine a = 0; a < kEditorMarkerCount; a++)
	{
		if (widget == markerButton[a])
		{
			currentTool = a;
			markerButton[a]->SetValue(1);
			break;
		}
	}

	editor->SetCurrentCursor(TheWorldEditor->GetEditorCursor(kEditorCursorCross));
}

void MarkersPage::Disengage(Editor *editor, void *cookie)
{
	if (currentTool != -1)
	{
		markerButton[currentTool]->SetValue(0);
		currentTool = -1;
	}
	else
	{
		if (cookie != locatorList)
		{
			locatorList->UnselectAllListItems();
		}
	}
}

bool MarkersPage::BeginTool(Editor *editor, EditorTrackData *trackData)
{
	if ((trackData->viewportType != kEditorViewportGraph) || (trackData->superNode))
	{
		Marker *marker = nullptr;

		if (currentTool != -1)
		{
			switch (currentTool)
			{
				case kEditorMarkerLocator:

					marker = new LocatorMarker(0);
					break;

				case kEditorMarkerConnection:

					marker = new ConnectionMarker;
					break;

				case kEditorMarkerCube:

					marker = new CubeMarker("C4/environment", kTextureBC13, 128);
					break;

				case kEditorMarkerShader:

					marker = new ShaderMarker;
					break;
			}
		}
		else
		{
			const Widget *listItem = locatorList->GetFirstSelectedListItem();
			if (listItem)
			{
				marker = new LocatorMarker(static_cast<const TypeWidget *>(listItem)->GetItemType());
			}
		}

		if (marker)
		{
			marker->SetNodeFlags(kNodeAnimateInhibit);
			editor->InitNewNode(trackData, marker);

			if (trackData->viewportType == kEditorViewportOrtho)
			{
				return (true);
			}

			editor->CommitNewNode(trackData, true);
		}
	}

	return (false);
}

bool MarkersPage::TrackTool(Editor *editor, EditorTrackData *trackData)
{
	editor->AutoScroll(trackData);

	float dx = trackData->currentPosition.x - trackData->snappedAnchorPosition.x;
	float dy = trackData->snappedAnchorPosition.y - trackData->currentPosition.y;

	if (InterfaceMgr::GetShiftKey())
	{
		if (Fabs(dx) > Fabs(dy))
		{
			dy = 0.0F;
		}
		else
		{
			dx = 0.0F;
		}
	}

	if ((dx != trackData->currentSize.x) || (dy != trackData->currentSize.y))
	{
		trackData->currentSize.Set(dx, dy);

		Marker *marker = static_cast<Marker *>(trackData->trackNode);
		if (marker->GetMarkerType() == kMarkerLocator)
		{
			float scale = trackData->viewportScale * 8.0F;
			if (dx * dx + dy * dy > scale * scale)
			{
				Vector3D direction = editor->GetTargetSpaceDirection(trackData, Vector3D(dx, -dy, 0.0F));
				Vector3D up = editor->GetTargetSpaceDirection(trackData, Vector3D(0.0F, 0.0F, -1.0F));

				direction.Normalize();
				up = Normalize(up - ProjectOnto(up, direction));

				marker->SetNodeMatrix3D(direction, up % direction, up);
				editor->InvalidateNode(marker);
			}
		}
	}

	return (true);
}

bool MarkersPage::EndTool(Editor *editor, EditorTrackData *trackData)
{
	editor->CommitNewNode(trackData, TrackTool(editor, trackData));
	return (true);
}


PathsPage::PathsPage() :
		EditorPage(kEditorPagePaths, "WorldEditor/path/Paths"),
		pathButtonObserver(this, &PathsPage::HandlePathButtonEvent)
{
	currentTool = -1;
}

PathsPage::~PathsPage()
{
}

void PathsPage::Preprocess(void)
{
	static const char *const buttonIdentifier[kEditorPathCount] =
	{
		"Linear", "Elliptical", "Bezier"
	};

	EditorPage::Preprocess();

	for (machine a = 0; a < kEditorPathCount; a++)
	{
		pathButton[a] = static_cast<IconButtonWidget *>(FindWidget(buttonIdentifier[a]));
		pathButton[a]->SetObserver(&pathButtonObserver);
	}
}

void PathsPage::HandlePathButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		GetEditor()->SetCurrentTool(this, widget);
	}
}

void PathsPage::Engage(Editor *editor, void *cookie)
{
	Widget *widget = static_cast<Widget *>(cookie);

	for (machine a = 0; a < kEditorPathCount; a++)
	{
		if (widget == pathButton[a])
		{
			currentTool = a;
			pathButton[a]->SetValue(1);
			break;
		}
	}

	editor->SetCurrentCursor(TheWorldEditor->GetEditorCursor(kEditorCursorCross));
}

void PathsPage::Disengage(Editor *editor, void *cookie)
{
	if (currentTool != -1)
	{
		pathButton[currentTool]->SetValue(0);
		currentTool = -1;
	}
}

bool PathsPage::SnapToBeginning(const PathMarker *marker, const EditorTrackData *trackData)
{
	if (!(trackData->currentModifierKeys & kModifierKeyCommand))
	{
		Ray			ray;
		PickData	pickData;

		const Transform4D& transform = marker->GetInverseWorldTransform();

		ray.origin = transform * trackData->worldRay.origin;
		ray.direction = transform * trackData->worldRay.direction;
		ray.radius = trackData->worldRay.radius;
		ray.tmin = trackData->worldRay.tmin;
		ray.tmax = trackData->worldRay.tmax;

		if (static_cast<PathManipulator *>(Editor::GetManipulator(marker))->PickControlPoint(&ray, &pickData))
		{
			return (pickData.pickIndex[0] == 0);
		}
	}

	return (false);
}

bool PathsPage::BeginTool(Editor *editor, EditorTrackData *trackData)
{
	if (trackData->viewportType == kEditorViewportOrtho)
	{
		PathMarker *pathMarker = nullptr;

		const NodeReference *reference = editor->GetFirstSelectedNode();
		if ((reference) && (!reference->Next()))
		{
			Node *node = reference->GetNode();
			if (node->GetNodeType() == kNodeMarker)
			{
				Marker *marker = static_cast<Marker *>(node);
				if (marker->GetMarkerType() == kMarkerPath)
				{
					pathMarker = static_cast<PathMarker *>(marker);
				}
			}
		}

		if (pathMarker)
		{
			targetPath = pathMarker;
			Path *path = pathMarker->GetPath();

			PathComponent *prevComponent = path->GetLastPathComponent();
			const Point3D& point = prevComponent->GetEndPosition();
			Vector3D tangent = prevComponent->GetEndTangent();

			Point3D position = pathMarker->GetInverseWorldTransform() * (editor->GetTargetZone()->GetWorldTransform() * editor->GetTargetSpacePosition(trackData, trackData->snappedAnchorPosition));
			Point3D snappedPosition = (SnapToBeginning(pathMarker, trackData)) ? path->GetFirstPathComponent()->GetBeginPosition() : position;

			PathComponent *component = nullptr;

			switch (currentTool)
			{
				case kEditorPathLinear:

					component = new LinearPathComponent(point, snappedPosition);
					break;

				case kEditorPathElliptical:

					component = new EllipticalPathComponent(point, snappedPosition, point + tangent * InverseMag(tangent));
					break;

				case kEditorPathBezier:
				{
					if (prevComponent->GetPathType() == kPathBezier)
					{
						BezierPathComponent *bezierComponent = static_cast<BezierPathComponent *>(prevComponent);
						const Point3D& p = bezierComponent->GetControlPoint(0);
						if (bezierComponent->GetControlPoint(3) == p)
						{
							bezierComponent->SetControlPoint(2, position);
							bezierComponent->SetControlPoint(3, position);

							trackData->trackNode = pathMarker;
							editor->InvalidateNode(pathMarker);
							return (true);
						}
					}

					component = new BezierPathComponent(point, point + tangent, snappedPosition, snappedPosition);
					break;
				}
			}

			if (component)
			{
				editor->AddOperation(new PathOperation(pathMarker));

				path->AppendPathComponent(component);
				editor->InvalidateNode(pathMarker);

				trackData->trackNode = pathMarker;
				return (true);
			}
		}
		else
		{
			targetPath = nullptr;

			PathMarker *marker = new PathMarker(K::z_unit);
			PathComponent *component = nullptr;

			switch (currentTool)
			{
				case kEditorPathLinear:

					component = new LinearPathComponent(Zero3D, Zero3D);
					break;

				case kEditorPathElliptical:

					component = new EllipticalPathComponent(Zero3D, Zero3D, Point3D(1.0F, 0.0F, 0.0F));
					break;

				case kEditorPathBezier:

					component = new BezierPathComponent(Zero3D, Zero3D, Zero3D, Zero3D);
					break;
			}

			if (component)
			{
				marker->GetPath()->AppendPathComponent(component);
				editor->InitNewNode(trackData, marker);
				return (true);
			}

			delete marker;
		}
	}

	return (false);
}

bool PathsPage::TrackTool(Editor *editor, EditorTrackData *trackData)
{
	editor->AutoScroll(trackData);

	float dx = trackData->snappedCurrentPosition.x - trackData->snappedAnchorPosition.x;
	float dy = trackData->snappedCurrentPosition.y - trackData->snappedAnchorPosition.y;
	float ax = Fabs(dx);
	float ay = Fabs(dy);

	if (InterfaceMgr::GetShiftKey())
	{
		if (ax < ay * 0.5F)
		{
			ax = 0.0F;
		}
		else if (ay < ax * 0.5F)
		{
			ay = 0.0F;
		}
		else
		{
			ax = ay = Fmax(ax, ay);
		}

		dx = (dx < 0.0F) ? -ax : ax;
		dy = (dy < 0.0F) ? -ay : ay;
	}

	if ((dx != trackData->currentSize.x) || (dy != trackData->currentSize.y) || (trackData->currentModifierKeys != trackData->previousModifierKeys))
	{
		trackData->currentSize.Set(dx, dy);

		PathMarker *pathMarker = static_cast<PathMarker *>(trackData->trackNode);
		const Path *path = pathMarker->GetPath();

		Point3D position = pathMarker->GetInverseWorldTransform() * (editor->GetTargetZone()->GetWorldTransform() * editor->GetTargetSpacePosition(trackData, trackData->snappedAnchorPosition + Vector2D(dx, dy)));
		Point3D snappedPosition = (SnapToBeginning(pathMarker, trackData)) ? path->GetFirstPathComponent()->GetBeginPosition() : position;

		PathComponent *component = path->GetLastPathComponent();
		switch (component->GetPathType())
		{
			case kPathLinear:
			{
				LinearPathComponent *linearComponent = static_cast<LinearPathComponent *>(component);
				linearComponent->SetControlPoint(1, snappedPosition);
				break;
			}

			case kPathElliptical:
			{
				EllipticalPathComponent *ellipticalComponent = static_cast<EllipticalPathComponent *>(component);
				ellipticalComponent->SetControlPoint(1, snappedPosition);
				break;
			}

			case kPathBezier:
			{
				BezierPathComponent *bezierComponent = static_cast<BezierPathComponent *>(component);
				if (targetPath)
				{
					bezierComponent->SetControlPoint(2, position);
				}
				else
				{
					bezierComponent->SetControlPoint(1, position);
				}

				break;
			}
		}

		editor->InvalidateNode(pathMarker);
	}

	return ((dx != 0.0F) || (dy != 0.0F));
}

bool PathsPage::EndTool(Editor *editor, EditorTrackData *trackData)
{
	PathMarker *pathMarker = static_cast<PathMarker *>(trackData->trackNode);
	const Path *path = pathMarker->GetPath();

	PathComponent *component = path->GetLastPathComponent();
	if (component->GetPathType() == kPathBezier)
	{
		BezierPathComponent *bezierComponent = static_cast<BezierPathComponent *>(component);
		const Point3D& p2 = bezierComponent->GetControlPoint(2);
		const Point3D& p3 = bezierComponent->GetControlPoint(3);

		if (SquaredMag(p3 - p2) < K::min_float)
		{
			Point3D q = bezierComponent->GetControlPoint(0) * 0.25F + p3 * 0.75F;
			bezierComponent->SetControlPoint(2, q);
			editor->InvalidateNode(pathMarker);
		}
	}

	if (!targetPath)
	{
		editor->CommitNewNode(trackData, TrackTool(editor, trackData));
	}

	return (true);
}


EffectsPage::EffectsPage() :
		EditorPage(kEditorPageEffects, "WorldEditor/effect/Effects"),
		effectButtonObserver(this, &EffectsPage::HandleEffectButtonEvent)
{
	currentTool = -1;
}

EffectsPage::~EffectsPage()
{
}

void EffectsPage::Preprocess(void)
{
	static const char *const buttonIdentifier[kEditorEffectCount] =
	{
		"Quad", "Flare", "Beam", "Tube", "Fire", "BoxShaft", "CylinderShaft", "TruncPyraShaft", "TruncConeShaft", "Panel"
	};

	EditorPage::Preprocess();

	for (machine a = 0; a < kEditorEffectCount; a++)
	{
		effectButton[a] = static_cast<IconButtonWidget *>(FindWidget(buttonIdentifier[a]));
		effectButton[a]->SetObserver(&effectButtonObserver);
	}

	frustumCameraButton = static_cast<IconButtonWidget *>(FindWidget("Frustum"));
	frustumCameraButton->SetObserver(&effectButtonObserver);
}

void EffectsPage::HandleEffectButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		GetEditor()->SetCurrentTool(this, widget);
	}
}

void EffectsPage::Engage(Editor *editor, void *cookie)
{
	Widget *widget = static_cast<Widget *>(cookie);

	if (widget == frustumCameraButton)
	{
		currentMode = kEditorEffectModeCamera;
		currentTool = 0;

		frustumCameraButton->SetValue(1);
	}
	else
	{
		currentMode = kEditorEffectModeEffect;

		for (machine a = 0; a < kEditorEffectCount; a++)
		{
			if (widget == effectButton[a])
			{
				currentTool = a;
				effectButton[a]->SetValue(1);
				break;
			}
		}
	}

	editor->SetCurrentCursor(TheWorldEditor->GetEditorCursor(kEditorCursorCross));
}

void EffectsPage::Disengage(Editor *editor, void *cookie)
{
	if (currentTool != -1)
	{
		if (currentMode == kEditorEffectModeEffect)
		{
			effectButton[currentTool]->SetValue(0);
		}
		else if (currentMode == kEditorEffectModeCamera)
		{
			frustumCameraButton->SetValue(0);
		}

		currentTool = -1;
	}
}

bool EffectsPage::BeginTool(Editor *editor, EditorTrackData *trackData)
{
	if (trackData->viewportType == kEditorViewportOrtho)
	{
		if (currentMode == kEditorEffectModeEffect)
		{
			Effect *effect = nullptr;

			switch (currentTool)
			{
				case kEditorEffectQuad:

					effect = new QuadEffect(0.0F, K::white, "texture/LightFlare");
					break;

				case kEditorEffectFlare:

					effect = new FlareEffect(0.0F, 0.15F, 0.1F, "texture/LightFlare");
					break;

				case kEditorEffectBeam:

					effect = new BeamEffect(0.0F, 0.0F, ColorRGBA(1.0F, 1.0F, 1.0F, 1.0F));
					break;

				case kEditorEffectTube:
				{
					const NodeReference *reference = editor->GetFirstSelectedNode();
					if ((reference) && (!reference->Next()))
					{
						Node *node = reference->GetNode();
						if (node->GetNodeType() == kNodeMarker)
						{
							Marker *marker = static_cast<Marker *>(node);
							if (marker->GetMarkerType() == kMarkerPath)
							{
								PathMarker *pathMarker = static_cast<PathMarker *>(marker);
								const Path *path = pathMarker->GetPath();

								TubeEffect *tubeEffect = new TubeEffect(path, 0.0F, ColorRGBA(1.0F, 1.0F, 1.0F, 1.0F));
								tubeEffect->SetConnectedPathMarker(pathMarker);
								effect = tubeEffect;
							}
						}
					}

					break;
				}

				case kEditorEffectFire:

					effect = new FireEffect(0.0F, 0.0F, 0.25F, 16, "fire/Flame");
					break;

				case kEditorEffectBoxShaft:

					effect = new BoxShaftEffect(Zero3D, ColorRGBA(1.0F, 1.0F, 1.0F, 1.0F));
					break;

				case kEditorEffectCylinderShaft:

					effect = new CylinderShaftEffect(Zero2D, 0.0F, ColorRGBA(1.0F, 1.0F, 1.0F, 1.0F));
					break;

				case kEditorEffectTruncatedPyramidShaft:

					effect = new TruncatedPyramidShaftEffect(Zero2D, 0.0F, 0.5F, ColorRGBA(1.0F, 1.0F, 1.0F, 1.0F));
					break;

				case kEditorEffectTruncatedConeShaft:

					effect = new TruncatedConeShaftEffect(Zero2D, 0.0F, 0.5F, ColorRGBA(1.0F, 1.0F, 1.0F, 1.0F));
					break;

				case kEditorEffectPanel:

					effect = new PanelEffect(Zero2D);
					break;
			}

			if (effect)
			{
				editor->InitNewNode(trackData, effect);
				return (true);
			}
		}
		else if (currentMode == kEditorEffectModeCamera)
		{
			FrustumCamera *camera = new FrustumCamera(1.0F, 1.0F);
			FrustumCameraObject *object = camera->GetObject();
			object->SetFrustumFlags(kFrustumInfinite);
			object->SetNearDepth(0.1F);
			object->SetFarDepth(1.0F);

			editor->InitNewNode(trackData, camera);
			return (true);
		}
	}

	return (false);
}

bool EffectsPage::TrackTool(Editor *editor, EditorTrackData *trackData)
{
	editor->AutoScroll(trackData);

	Point2D anchor = trackData->snappedAnchorPosition;
	float dx = trackData->snappedCurrentPosition.x - anchor.x;
	float dy = anchor.y - trackData->snappedCurrentPosition.y;

	if (currentMode == kEditorEffectModeEffect)
	{
		float ax = Fabs(dx);
		float ay = Fabs(dy);

		Effect *effect = static_cast<Effect *>(trackData->trackNode);
		EffectType effectType = effect->GetEffectType();

		ShaftType shaftType = 0;
		if (effectType == kEffectShaft)
		{
			shaftType = static_cast<ShaftEffect *>(effect)->GetShaftType();
		}

		if ((shaftType == kShaftBox) || (shaftType == kShaftTruncatedPyramid) || (effectType == kEffectPanel))
		{
			if (InterfaceMgr::GetShiftKey())
			{
				ax = ay = Fmax(ax, ay);
				dx = (dx < 0.0F) ? -ax : ax;
				dy = (dy < 0.0F) ? -ax : ax;
			}
		}
		else
		{
			if (ax > ay)
			{
				dy = (dy > 0.0F) ? ax : -ax;
				ay = ax;
			}
			else
			{
				dx = (dx > 0.0F) ? ay : -ay;
				ax = ay;
			}
		}

		if ((dx != trackData->currentSize.x) || (dy != trackData->currentSize.y))
		{
			trackData->currentSize.Set(dx, dy);

			unsigned_int32 editorFlags = editor->GetEditorObject()->GetEditorFlags();
			if ((effectType == kEffectPanel) || (shaftType == kShaftBox) || (shaftType == kShaftTruncatedPyramid))
			{
				if (editorFlags & kEditorDrawFromCenter)
				{
					anchor.x -= ax;
					anchor.y += ay;
					dx = ax * 2.0F;
					dy = ay * 2.0F;
				}
				else
				{
					if (dx < 0.0F)
					{
						anchor.x += dx;
						dx = -dx;
					}

					if (dy < 0.0F)
					{
						anchor.y -= dy;
						dy = -dy;
					}
				}

				effect->SetNodePosition(editor->GetTargetSpacePosition(trackData, anchor));

				if (effectType == kEffectPanel)
				{
					const PanelEffect *panelEffect = static_cast<PanelEffect *>(effect);
					panelEffect->GetObject()->SetExternalPanelSize(Vector2D(dx, dy));
				}
				else
				{
					ShaftEffect *shaftEffect = static_cast<ShaftEffect *>(effect);
					if (shaftType == kShaftBox)
					{
						BoxShaftEffectObject *object = static_cast<BoxShaftEffect *>(shaftEffect)->GetObject();
						object->SetBoxSize(Vector3D(dx, dy, Fmax(dx, dy) * 2.0F));
					}
					else
					{
						TruncatedPyramidShaftEffectObject *object = static_cast<TruncatedPyramidShaftEffect *>(shaftEffect)->GetObject();
						object->SetPyramidSize(Vector2D(dx, dy));
						object->SetPyramidHeight(Fmax(dx, dy) * 2.0F);
					}
				}
			}
			else
			{
				if (editorFlags & kEditorDrawFromCenter)
				{
					dx = ax;
					dy = ay;
				}
				else
				{
					anchor.x += dx * 0.5F;
					anchor.y -= dy * 0.5F;
					dx = ax * 0.5F;
					dy = ay * 0.5F;

					effect->SetNodePosition(editor->GetTargetSpacePosition(trackData, anchor));
				}

				switch (effectType)
				{
					case kEffectQuad:
					{
						const QuadEffect *quadEffect = static_cast<QuadEffect *>(effect);
						QuadEffectObject *object = quadEffect->GetObject();
						object->SetQuadRadius(dx);
						break;
					}

					case kEffectFlare:
					{
						const FlareEffect *flareEffect = static_cast<FlareEffect *>(effect);
						FlareEffectObject *object = flareEffect->GetObject();
						object->SetFlareRadius(dx);
						break;
					}

					case kEffectBeam:
					{
						BeamEffect *beamEffect = static_cast<BeamEffect *>(effect);
						BeamEffectObject *object = beamEffect->GetObject();
						object->SetBeamRadius(dx);
						object->SetBeamHeight(dx * 2.0F);
						break;
					}

					case kEffectTube:
					{
						const TubeEffect *tubeEffect = static_cast<TubeEffect *>(effect);
						TubeEffectObject *object = tubeEffect->GetObject();
						object->SetTubeRadius(dx);
						break;
					}

					case kEffectFire:
					{
						const FireEffect *fireEffect = static_cast<FireEffect *>(effect);
						FireEffectObject *object = fireEffect->GetObject();
						object->SetFireRadius(dx);
						object->SetFireHeight(dx * 2.0F);
						break;
					}

					case kEffectShaft:
					{
						ShaftEffect *shaftEffect = static_cast<ShaftEffect *>(effect);
						if (shaftType == kShaftCylinder)
						{
							CylinderShaftEffectObject *object = static_cast<CylinderShaftEffect *>(shaftEffect)->GetObject();
							object->SetCylinderSize(Vector2D(dx, dy));
							object->SetCylinderHeight(Fmax(dx, dy) * 2.0F);
						}
						else
						{
							TruncatedConeShaftEffectObject *object = static_cast<TruncatedConeShaftEffect *>(shaftEffect)->GetObject();
							object->SetConeSize(Vector2D(dx, dy));
							object->SetConeHeight(Fmax(dx, dy) * 2.0F);
						}

						break;
					}
				}
			}

			effect->UpdateEffectGeometry();

			editor->InvalidateNode(effect);
			if (effectType == kEffectTube)
			{
				static_cast<TubeEffect *>(effect)->GetObject()->Build();
			}
		}
	}
	else if (currentMode == kEditorEffectModeCamera)
	{
		Camera *camera = static_cast<Camera *>(trackData->trackNode);

		if (InterfaceMgr::GetShiftKey())
		{
			if (Fabs(dx) > Fabs(dy))
			{
				dy = 0.0F;
			}
			else
			{
				dx = 0.0F;
			}
		}

		if ((dx != trackData->currentSize.x) || (dy != trackData->currentSize.y))
		{
			trackData->currentSize.Set(dx, dy);

			float r2 = dx * dx + dy * dy;
			float scale = trackData->viewportScale * 8.0F;
			if (r2 > scale * scale)
			{
				Vector3D direction = editor->GetTargetSpaceDirection(trackData, Vector3D(dx, -dy, 0.0F));
				Vector3D down = editor->GetTargetSpaceDirection(trackData, Vector3D(0.0F, 0.0F, 1.0F));

				direction.Normalize();
				down = Normalize(down - ProjectOnto(down, direction));

				camera->SetNodeMatrix3D(down % direction, down, direction);
			}

			static_cast<FrustumCameraObject *>(camera->GetObject())->SetFarDepth(Fmax(Sqrt(r2), 1.0F));
			editor->InvalidateNode(camera);
		}

		return (true);
	}

	return ((dx != 0.0F) && (dy != 0.0F));
}

bool EffectsPage::EndTool(Editor *editor, EditorTrackData *trackData)
{
	editor->CommitNewNode(trackData, TrackTool(editor, trackData));
	return (true);
}


ParticlesPage::ParticlesPage() :
		EditorPage(kEditorPageParticles, "WorldEditor/particle/Particles"),
		menuButtonObserver(this, &ParticlesPage::HandleMenuButtonEvent),
		emitterButtonObserver(this, &ParticlesPage::HandleEmitterButtonEvent),
		particleSystemListObserver(this, &ParticlesPage::HandleParticleSystemListEvent)
{
	currentTool = -1;
}

ParticlesPage::~ParticlesPage()
{
}

ParticlesPage::GenerateHeightFieldWindow::GenerateHeightFieldWindow(Editor *editor) : Window("WorldEditor/particle/Generate")
{
	worldEditor = editor;
}

ParticlesPage::GenerateHeightFieldWindow::~GenerateHeightFieldWindow()
{
	int32 count = jobCount;
	for (machine a = count - 1; a >= 0; a--)
	{
		delete jobTable[a];
	}

	delete[] jobTable;
}

void ParticlesPage::GenerateHeightFieldWindow::JobGenerateHeightField(Job *job, void *cookie)
{
	Box3D	box;

	const Emitter *emitter = static_cast<Emitter *>(cookie);
	EmitterObject *object = emitter->GetObject();

	int32 subdivX = object->GetHeightFieldSubdivX();
	int32 subdivY = object->GetHeightFieldSubdivY();
	float *heightField = object->GetHeightFieldStorage();

	object->CalculateBoundingBox(&box);
	float xscale = (box.max.x - box.min.x) / (float) subdivX;
	float yscale = (box.max.y - box.min.y) / (float) subdivY;
	float boxHeight = box.max.z - box.min.z;

	const World *world = emitter->GetWorld();
	const Transform4D& transform = emitter->GetWorldTransform();

	for (machine j = 0; j <= subdivY; j++)
	{
		float v = (float) j * yscale;
		for (machine i = 0; i <= subdivX; i++)
		{
			CollisionData	data;

			float u = (float) i * xscale;
			Point3D p1 = transform * Point3D(box.min.x + u, box.min.y + v, box.min.z);
			Point3D p2 = p1 + transform[2] * boxHeight;

			if (world->DetectCollision(p2, p1, 0.0F, kCollisionCamera, &data))
			{
				heightField[0] = boxHeight * (1.0F - data.param);
			}

			heightField++;
		}
	}
}

void ParticlesPage::GenerateHeightFieldWindow::Preprocess(void)
{
	Window::Preprocess();

	stopButton = static_cast<PushButtonWidget *>(FindWidget("Stop"));
	progressBar = static_cast<ProgressWidget *>(FindWidget("Progress"));

	auto filter = [](const Node *node) -> bool
	{
		return ((node->GetNodeType() == kNodeEmitter) && (static_cast<const Emitter *>(node)->GetObject()->GetEmitterFlags() & kEmitterHeightField));
	};

	worldEditor->AddOperation(new ObjectDataOperation(worldEditor->GetSelectionList(), filter));

	int32 count = 0;
	const NodeReference *reference = worldEditor->GetFirstSelectedNode();
	while (reference)
	{
		const Node *node = reference->GetNode();
		if (node->GetNodeType() == kNodeEmitter)
		{
			const Emitter *emitter = static_cast<const Emitter *>(node);
			EmitterObject *object = emitter->GetObject();

			if (object->GetEmitterFlags() & kEmitterHeightField)
			{
				count++;
			}
		}

		reference = reference->Next();
	}

	progressBar->SetMaxValue(count);

	jobCount = count;
	jobTable = new Job *[count];

	count = 0;
	reference = worldEditor->GetFirstSelectedNode();
	while (reference)
	{
		Node *node = reference->GetNode();
		if (node->GetNodeType() == kNodeEmitter)
		{
			Emitter *emitter = static_cast<Emitter *>(node);
			EmitterObject *object = emitter->GetObject();

			if (object->GetEmitterFlags() & kEmitterHeightField)
			{
				Job *job = new Job(&JobGenerateHeightField, emitter);
				TheJobMgr->SubmitJob(job);
				jobTable[count] = job;
				count++;
			}
		}

		reference = reference->Next();
	}
}

void ParticlesPage::GenerateHeightFieldWindow::Move(void)
{
	Window::Move();

	int32 count = jobCount;
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

void ParticlesPage::GenerateHeightFieldWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if ((eventData->eventType == kEventWidgetActivate) && (widget == stopButton))
	{
		TheJobMgr->CancelJobArray(jobCount, jobTable);
		Close();
	}
}

void ParticlesPage::Preprocess(void)
{
	static const char *const buttonIdentifier[kEditorEmitterCount] =
	{
		"Box", "Cylinder", "Sphere"
	};

	EditorPage::Preprocess();

	menuButton = static_cast<IconButtonWidget *>(FindWidget("Menu"));
	menuButton->SetObserver(&menuButtonObserver);

	const StringTable *table = TheWorldEditor->GetStringTable();

	MenuItemWidget *widget = new MenuItemWidget(table->GetString(StringID('PAGE', kEditorPageParticles, 'SALL')), WidgetObserver<ParticlesPage>(this, &ParticlesPage::HandleSelectAllMenuItemEvent));
	particleMenuItem[kParticleMenuSelectAll] = widget;
	particleMenuItemList.Append(widget);

	particleMenuItemList.Append(new MenuItemWidget(kLineSolid));

	widget = new MenuItemWidget(table->GetString(StringID('PAGE', kEditorPageParticles, 'GHIT')), WidgetObserver<ParticlesPage>(this, &ParticlesPage::HandleGenerateEmitterHeightMapMenuItemEvent));
	particleMenuItem[kParticleMenuGenerateEmitterHeightMap] = widget;
	particleMenuItemList.Append(widget);

	for (machine a = 0; a < kEditorEmitterCount; a++)
	{
		emitterButton[a] = static_cast<IconButtonWidget *>(FindWidget(buttonIdentifier[a]));
		emitterButton[a]->SetObserver(&emitterButtonObserver);
	}

	particleSystemList = static_cast<ListWidget *>(FindWidget("List"));
	particleSystemList->SetObserver(&particleSystemListObserver);
	particleSystemList->SetWidgetUsage(particleSystemList->GetWidgetUsage() & ~kWidgetKeyboardFocus);

	Vector2D size = particleSystemList->GetNaturalListItemSize();

	const ParticleSystemRegistration *registration = Registrable<ParticleSystem, ParticleSystemRegistration>::GetFirstRegistration();
	while (registration)
	{
		particleSystemList->InsertSortedListItem(new TypeWidget(size, registration->GetParticleSystemName(), registration->GetParticleSystemType()));
		registration = registration->Next();
	}
}

void ParticlesPage::HandleSelectAllMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData)
{
	Editor *editor = GetEditor();
	editor->UnselectAll();

	auto filter = [](const Node *node, const void *cookie) -> bool
	{
		if (node->GetNodeType() == kNodeEffect)
		{
			const Effect *effect = static_cast<const Effect *>(node);
			ParticleSystemType type = *static_cast<const ParticleSystemType *>(cookie);
			return ((effect->GetEffectType() == kEffectParticleSystem) && (static_cast<const ParticleSystem *>(effect)->GetParticleSystemType() == type));
		}

		return (false);
	};

	const Widget *listItem = particleSystemList->GetFirstSelectedListItem();
	ParticleSystemType type = static_cast<const TypeWidget *>(listItem)->GetItemType();

	editor->SelectAllFilter(filter, &type);
}

void ParticlesPage::HandleGenerateEmitterHeightMapMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData)
{
	Editor *editor = GetEditor();
	editor->AddSubwindow(new GenerateHeightFieldWindow(editor));
}

void ParticlesPage::HandleMenuButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		for (machine a = 0; a < kParticleMenuItemCount; a++)
		{
			particleMenuItem[a]->Disable();
		}

		if (particleSystemList->GetFirstSelectedListItem())
		{
			particleMenuItem[kParticleMenuSelectAll]->Enable();
		}

		const NodeReference *reference = GetEditor()->GetFirstSelectedNode();
		while (reference)
		{
			const Node *node = reference->GetNode();
			if (node->GetNodeType() == kNodeEmitter)
			{
				const Emitter *emitter = static_cast<const Emitter *>(node);
				if (emitter->GetObject()->GetEmitterFlags() & kEmitterHeightField)
				{
					particleMenuItem[kParticleMenuGenerateEmitterHeightMap]->Enable();
				}

				break;
			}

			reference = reference->Next();
		}

		Menu *menu = new Menu(kMenuContextual, &particleMenuItemList);
		menu->SetWidgetPosition(menuButton->GetWorldPosition() + Vector3D(25.0F, 0.0F, 0.0F));
		TheInterfaceMgr->SetActiveMenu(menu);
	}
}

void ParticlesPage::HandleParticleSystemListEvent(Widget *widget, const WidgetEventData *eventData)
{
	EventType eventType = eventData->eventType;

	if (eventType == kEventWidgetActivate)
	{
		HandleSelectAllMenuItemEvent(nullptr, nullptr);
	}
	else if (eventType == kEventWidgetChange)
	{
		GetEditor()->SetCurrentTool(this, widget);
	}
}

void ParticlesPage::HandleEmitterButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		GetEditor()->SetCurrentTool(this, widget);
	}
}

void ParticlesPage::Engage(Editor *editor, void *cookie)
{
	Widget *widget = static_cast<Widget *>(cookie);

	currentTool = -1;
	for (machine a = 0; a < kEditorEmitterCount; a++)
	{
		if (widget == emitterButton[a])
		{
			currentTool = a;
			emitterButton[a]->SetValue(1);
			break;
		}
	}

	editor->SetCurrentCursor(TheWorldEditor->GetEditorCursor(kEditorCursorCross));
}

void ParticlesPage::Disengage(Editor *editor, void *cookie)
{
	if (currentTool != -1)
	{
		emitterButton[currentTool]->SetValue(0);
		currentTool = -1;
	}
	else
	{
		if (cookie != particleSystemList)
		{
			particleSystemList->UnselectAllListItems();
		}
	}
}

bool ParticlesPage::BeginTool(Editor *editor, EditorTrackData *trackData)
{
	if (currentTool != -1)
	{
		if (trackData->viewportType == kEditorViewportOrtho)
		{
			Emitter *emitter = nullptr;

			switch (currentTool)
			{
				case kEditorEmitterBox:

					emitter = new BoxEmitter(Zero3D);
					break;

				case kEditorEmitterCylinder:

					emitter = new CylinderEmitter(Zero2D, 0.0F);
					break;

				case kEditorEmitterSphere:

					emitter = new SphereEmitter(Zero3D);
					break;
			}

			if (emitter)
			{
				editor->InitNewNode(trackData, emitter);
				return (true);
			}
		}
	}
	else
	{
		if ((trackData->viewportType != kEditorViewportGraph) || (trackData->superNode))
		{
			const Widget *listItem = particleSystemList->GetFirstSelectedListItem();
			if (listItem)
			{
				ParticleSystem *particleSystem = ParticleSystem::New(static_cast<const TypeWidget *>(listItem)->GetItemType());
				editor->InitNewNode(trackData, particleSystem);

				if (trackData->viewportType == kEditorViewportOrtho)
				{
					return (true);
				}

				editor->CommitNewNode(trackData, true);
			}
		}
	}

	return (false);
}

bool ParticlesPage::TrackTool(Editor *editor, EditorTrackData *trackData)
{
	editor->AutoScroll(trackData);

	Point2D anchor = trackData->snappedAnchorPosition;

	if (trackData->trackNode->GetNodeType() == kNodeEmitter)
	{
		float dx = trackData->snappedCurrentPosition.x - anchor.x;
		float dy = anchor.y - trackData->snappedCurrentPosition.y;

		float ax = Fabs(dx);
		float ay = Fabs(dy);

		if (InterfaceMgr::GetShiftKey())
		{
			ax = ay = Fmax(ax, ay);
			dx = (dx < 0.0F) ? -ax : ax;
			dy = (dy < 0.0F) ? -ax : ax;
		}

		if ((dx != trackData->currentSize.x) || (dy != trackData->currentSize.y))
		{
			trackData->currentSize.Set(dx, dy);

			unsigned_int32 editorFlags = editor->GetEditorObject()->GetEditorFlags();

			Emitter *emitter = static_cast<Emitter *>(trackData->trackNode);
			EmitterType emitterType = emitter->GetEmitterType();

			if (emitterType == kEmitterBox)
			{
				if (editorFlags & kEditorDrawFromCenter)
				{
					anchor.x -= ax;
					anchor.y += ay;
					dx = ax * 2.0F;
					dy = ay * 2.0F;
				}
				else
				{
					if (dx < 0.0F)
					{
						anchor.x += dx;
						dx = -dx;
					}

					if (dy < 0.0F)
					{
						anchor.y -= dy;
						dy = -dy;
					}
				}

				emitter->SetNodePosition(editor->GetTargetSpacePosition(trackData, anchor));

				const BoxEmitter *box = static_cast<BoxEmitter *>(emitter);
				box->GetObject()->SetBoxSize(Vector3D(dx, dy, Fmax(dx, dy)));
			}
			else
			{
				if (editorFlags & kEditorDrawFromCenter)
				{
					dx = ax;
					dy = ay;
				}
				else
				{
					anchor.x += dx * 0.5F;
					anchor.y -= dy * 0.5F;
					dx = ax * 0.5F;
					dy = ay * 0.5F;

					emitter->SetNodePosition(editor->GetTargetSpacePosition(trackData, anchor));
				}

				switch (emitterType)
				{
					case kEmitterCylinder:
					{
						const CylinderEmitter *cylinder = static_cast<CylinderEmitter *>(emitter);
						CylinderEmitterObject *object = cylinder->GetObject();
						object->SetCylinderSize(Vector2D(dx, dy));
						object->SetCylinderHeight(Fmax(dx, dy) * 2.0F);
						break;
					}

					case kEmitterSphere:
					{
						const SphereEmitter *sphere = static_cast<SphereEmitter *>(emitter);
						SphereEmitterObject *object = sphere->GetObject();
						object->SetSphereSize(Vector3D(dx, dy, Fmax(dx, dy)));
						break;
					}
				}
			}

			editor->InvalidateNode(emitter);
		}

		return ((dx != 0.0F) && (dy != 0.0F));
	}

	float dx = trackData->currentPosition.x - anchor.x;
	float dy = anchor.y - trackData->currentPosition.y;

	if (InterfaceMgr::GetShiftKey())
	{
		if (Fabs(dx) > Fabs(dy))
		{
			dy = 0.0F;
		}
		else
		{
			dx = 0.0F;
		}
	}

	if ((dx != trackData->currentSize.x) || (dy != trackData->currentSize.y))
	{
		trackData->currentSize.Set(dx, dy);

		if (dx * dx + dy * dy != 0.0F)
		{
			Vector3D direction = editor->GetTargetSpaceDirection(trackData, Vector3D(dx, -dy, 0.0F));
			Vector3D up = editor->GetTargetSpaceDirection(trackData, Vector3D(0.0F, 0.0F, -1.0F));

			direction.Normalize();
			up = Normalize(up - ProjectOnto(up, direction));

			Node *node = trackData->trackNode;
			node->SetNodeMatrix3D(direction, up % direction, up);
			editor->InvalidateNode(node);
		}
	}

	return (true);
}

bool ParticlesPage::EndTool(Editor *editor, EditorTrackData *trackData)
{
	editor->CommitNewNode(trackData, TrackTool(editor, trackData));
	return (true);
}


AtmospherePage::AtmospherePage() :
		EditorPage(kEditorPageAtmosphere, "WorldEditor/atmosphere/Atmosphere"),
		atmosphereButtonObserver(this, &AtmospherePage::HandleAtmosphereButtonEvent)
{
	currentTool = -1;
}

AtmospherePage::~AtmospherePage()
{
}

void AtmospherePage::Preprocess(void)
{
	EditorPage::Preprocess();

	skyboxButton = static_cast<IconButtonWidget *>(FindWidget("Skybox"));
	skyboxButton->SetObserver(&atmosphereButtonObserver);

	fogSpaceButton = static_cast<IconButtonWidget *>(FindWidget("Fog"));
	fogSpaceButton->SetObserver(&atmosphereButtonObserver);
}

void AtmospherePage::HandleAtmosphereButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		GetEditor()->SetCurrentTool(this, widget);
	}
}

void AtmospherePage::Engage(Editor *editor, void *cookie)
{
	Widget *widget = static_cast<Widget *>(cookie);

	if (widget == skyboxButton)
	{
		currentMode = kEditorAtmosphereModeSkybox;
		currentTool = 0;
		skyboxButton->SetValue(1);
	}
	else if (widget == fogSpaceButton)
	{
		currentMode = kEditorAtmosphereModeFogSpace;
		currentTool = 0;
		fogSpaceButton->SetValue(1);
	}

	editor->SetCurrentCursor(TheWorldEditor->GetEditorCursor(kEditorCursorCross));
}

void AtmospherePage::Disengage(Editor *editor, void *cookie)
{
	if (currentTool != -1)
	{
		if (currentMode == kEditorAtmosphereModeSkybox)
		{
			skyboxButton->SetValue(0);
		}
		else if (currentMode == kEditorAtmosphereModeFogSpace)
		{
			fogSpaceButton->SetValue(0);
		}

		currentTool = -1;
	}
}

bool AtmospherePage::BeginTool(Editor *editor, EditorTrackData *trackData)
{
	if (currentMode == kEditorAtmosphereModeSkybox)
	{
		if ((trackData->viewportType != kEditorViewportGraph) || (trackData->superNode))
		{
			if (currentTool == 0)
			{
				editor->InitNewNode(trackData, new Skybox);
				editor->CommitNewNode(trackData, true);
			}
		}
	}
	else if (currentMode == kEditorAtmosphereModeFogSpace)
	{
		if (trackData->viewportType == kEditorViewportOrtho)
		{
			if (currentTool == 0)
			{
				Space *space = new FogSpace(Zero2D);
				editor->InitNewNode(trackData, space);
				return (true);
			}
		}
	}

	return (false);
}

bool AtmospherePage::TrackTool(Editor *editor, EditorTrackData *trackData)
{
	editor->AutoScroll(trackData);

	if ((currentMode == kEditorAtmosphereModeFogSpace) && (currentTool == 0))
	{
		Point2D anchor = trackData->snappedAnchorPosition;
		float dx = trackData->snappedCurrentPosition.x - anchor.x;
		float dy = anchor.y - trackData->snappedCurrentPosition.y;

		float ax = Fabs(dx);
		float ay = Fabs(dy);

		if (InterfaceMgr::GetShiftKey())
		{
			ax = ay = Fmax(ax, ay);
			dx = (dx < 0.0F) ? -ax : ax;
			dy = (dy < 0.0F) ? -ax : ax;
		}

		if ((dx != trackData->currentSize.x) || (dy != trackData->currentSize.y))
		{
			trackData->currentSize.Set(dx, dy);

			unsigned_int32 editorFlags = editor->GetEditorObject()->GetEditorFlags();
			if (editorFlags & kEditorDrawFromCenter)
			{
				anchor.x -= ax;
				anchor.y += ay;
				dx = ax * 2.0F;
				dy = ay * 2.0F;
			}
			else
			{
				if (dx < 0.0F)
				{
					anchor.x += dx;
					dx = -dx;
				}

				if (dy < 0.0F)
				{
					anchor.y -= dy;
					dy = -dy;
				}
			}

			Space *space = static_cast<Space *>(trackData->trackNode);
			space->SetNodePosition(editor->GetTargetSpacePosition(trackData, anchor));

			PlateVolume *plate = static_cast<PlateVolume *>(space->GetObject()->GetVolume());
			plate->SetPlateSize(Vector2D(dx, dy));

			editor->InvalidateNode(space);
		}

		return ((dx != 0.0F) && (dy != 0.0F));
	}

	return (true);
}

bool AtmospherePage::EndTool(Editor *editor, EditorTrackData *trackData)
{
	editor->CommitNewNode(trackData, TrackTool(editor, trackData));
	return (true);
}


PhysicsPage::PhysicsPage() :
		EditorPage(kEditorPagePhysics, "WorldEditor/physics/Physics"),
		physicsButtonObserver(this, &PhysicsPage::HandlePhysicsButtonEvent)
{
	currentTool = -1;
}

PhysicsPage::~PhysicsPage()
{
}

void PhysicsPage::Preprocess(void)
{
	static const char *const shapeButtonIdentifier[kEditorShapeCount] =
	{
		"Box", "Pyramid", "Cylinder", "Cone", "Sphere", "Dome", "Capsule", "TruncPyramid", "TruncCone", "TruncDome"
	};

	static const char *const jointButtonIdentifier[kEditorJointCount] =
	{
		"Spherical", "Universal", "Discal", "Revolute", "Cylindrical", "Prismatic"
	};

	static const char *const fieldButtonIdentifier[kEditorFieldCount] =
	{
		"BoxField", "CylinderField", "SphereField"
	};

	static const char *const blockerButtonIdentifier[kEditorBlockerCount] =
	{
		"PlateBlocker", "BoxBlocker", "CylinderBlocker", "SphereBlocker", "CapsuleBlocker"
	};

	EditorPage::Preprocess();

	physicsNodeButton = static_cast<IconButtonWidget *>(FindWidget("Node"));
	physicsNodeButton->SetObserver(&physicsButtonObserver);

	physicsSpaceButton = static_cast<IconButtonWidget *>(FindWidget("Space"));
	physicsSpaceButton->SetObserver(&physicsButtonObserver);

	for (machine a = 0; a < kEditorShapeCount; a++)
	{
		shapeButton[a] = static_cast<IconButtonWidget *>(FindWidget(shapeButtonIdentifier[a]));
		shapeButton[a]->SetObserver(&physicsButtonObserver);
	}

	for (machine a = 0; a < kEditorJointCount; a++)
	{
		jointButton[a] = static_cast<IconButtonWidget *>(FindWidget(jointButtonIdentifier[a]));
		jointButton[a]->SetObserver(&physicsButtonObserver);
	}

	for (machine a = 0; a < kEditorFieldCount; a++)
	{
		fieldButton[a] = static_cast<IconButtonWidget *>(FindWidget(fieldButtonIdentifier[a]));
		fieldButton[a]->SetObserver(&physicsButtonObserver);
	}

	for (machine a = 0; a < kEditorBlockerCount; a++)
	{
		blockerButton[a] = static_cast<IconButtonWidget *>(FindWidget(blockerButtonIdentifier[a]));
		blockerButton[a]->SetObserver(&physicsButtonObserver);
	}
}

void PhysicsPage::HandlePhysicsButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		GetEditor()->SetCurrentTool(this, widget);
	}
}

void PhysicsPage::Engage(Editor *editor, void *cookie)
{
	Widget *widget = static_cast<Widget *>(cookie);

	if (widget == physicsNodeButton)
	{
		currentMode = kEditorPhysicsModeNode;
		currentTool = 0;
		physicsNodeButton->SetValue(1);
	}
	else if (widget == physicsSpaceButton)
	{
		currentMode = kEditorPhysicsModeSpace;
		currentTool = 0;
		physicsSpaceButton->SetValue(1);
	}
	else
	{
		for (machine a = 0; a < kEditorShapeCount; a++)
		{
			if (widget == shapeButton[a])
			{
				currentMode = kEditorPhysicsModeShape;
				currentTool = a;
				shapeButton[a]->SetValue(1);
				goto end;
			}
		}

		for (machine a = 0; a < kEditorJointCount; a++)
		{
			if (widget == jointButton[a])
			{
				currentMode = kEditorPhysicsModeJoint;
				currentTool = a;
				jointButton[a]->SetValue(1);
				goto end;
			}
		}

		for (machine a = 0; a < kEditorFieldCount; a++)
		{
			if (widget == fieldButton[a])
			{
				currentMode = kEditorPhysicsModeField;
				currentTool = a;
				fieldButton[a]->SetValue(1);
				goto end;
			}
		}

		for (machine a = 0; a < kEditorBlockerCount; a++)
		{
			if (widget == blockerButton[a])
			{
				currentMode = kEditorPhysicsModeBlocker;
				currentTool = a;
				blockerButton[a]->SetValue(1);
				goto end;
			}
		}
	}

	end:
	editor->SetCurrentCursor(TheWorldEditor->GetEditorCursor(kEditorCursorCross));
}

void PhysicsPage::Disengage(Editor *editor, void *cookie)
{
	if (currentTool != -1)
	{
		if (currentMode == kEditorPhysicsModeNode)
		{
			physicsNodeButton->SetValue(0);
		}
		else if (currentMode == kEditorPhysicsModeSpace)
		{
			physicsSpaceButton->SetValue(0);
		}
		else if (currentMode == kEditorPhysicsModeShape)
		{
			shapeButton[currentTool]->SetValue(0);
		}
		else if (currentMode == kEditorPhysicsModeJoint)
		{
			jointButton[currentTool]->SetValue(0);
		}
		else if (currentMode == kEditorPhysicsModeField)
		{
			fieldButton[currentTool]->SetValue(0);
		}
		else if (currentMode == kEditorPhysicsModeBlocker)
		{
			blockerButton[currentTool]->SetValue(0);
		}

		currentTool = -1;
	}
}

bool PhysicsPage::BeginTool(Editor *editor, EditorTrackData *trackData)
{
	if (currentMode == kEditorPhysicsModeNode)
	{
		if ((trackData->viewportType != kEditorViewportGraph) || (trackData->superNode))
		{
			PhysicsNode *node = new PhysicsNode;
			node->SetController(new PhysicsController);

			editor->InitNewNode(trackData, node);
			editor->CommitNewNode(trackData, true);
		}
	}
	else if (currentMode == kEditorPhysicsModeSpace)
	{
		if (trackData->viewportType == kEditorViewportOrtho)
		{
			Space *space = new PhysicsSpace(Zero3D);
			editor->InitNewNode(trackData, space);
			return (true);
		}
	}
	else if (currentMode == kEditorPhysicsModeShape)
	{
		if (trackData->viewportType == kEditorViewportOrtho)
		{
			Shape *shape = nullptr;

			switch (currentTool)
			{
				case kEditorShapeBox:

					shape = new BoxShape(Zero3D);
					break;

				case kEditorShapePyramid:

					shape = new PyramidShape(Zero2D, 0.0F);
					break;

				case kEditorShapeCylinder:

					shape = new CylinderShape(Zero2D, 0.0F);
					break;

				case kEditorShapeCone:

					shape = new ConeShape(Zero2D, 0.0F);
					break;

				case kEditorShapeSphere:

					shape = new SphereShape(Zero3D);
					break;

				case kEditorShapeDome:

					shape = new DomeShape(Zero3D);
					break;

				case kEditorShapeCapsule:

					shape = new CapsuleShape(Zero3D, 0.0F);
					break;

				case kEditorShapeTruncatedPyramid:

					shape = new TruncatedPyramidShape(Zero2D, 0.0F, 0.5F);
					break;

				case kEditorShapeTruncatedCone:

					shape = new TruncatedConeShape(Zero2D, 0.0F, 0.5F);
					break;

				case kEditorShapeTruncatedDome:

					shape = new TruncatedDomeShape(Zero2D, 0.0F, 0.5F);
					break;
			}

			if (shape)
			{
				shape->SetNodeFlags(kNodeAnimateInhibit);
				editor->InitNewNode(trackData, shape);
				return (true);
			}
		}
	}
	else if (currentMode == kEditorPhysicsModeJoint)
	{
		if ((trackData->viewportType != kEditorViewportGraph) || (trackData->superNode))
		{
			Joint *joint = nullptr;

			switch (currentTool)
			{
				case kEditorJointSpherical:

					joint = new SphericalJoint;
					break;

				case kEditorJointUniversal:

					joint = new UniversalJoint;
					break;

				case kEditorJointDiscal:

					joint = new DiscalJoint;
					break;

				case kEditorJointRevolute:

					joint = new RevoluteJoint;
					break;

				case kEditorJointCylindrical:

					joint = new CylindricalJoint;
					break;

				case kEditorJointPrismatic:

					joint = new PrismaticJoint;
					break;
			}

			if (joint)
			{
				joint->AddConnector(kConnectorKeyBody1);
				editor->InitNewNode(trackData, joint);

				if (trackData->viewportType == kEditorViewportOrtho)
				{
					return (true);
				}

				editor->CommitNewNode(trackData, true);
			}
		}
	}
	else if (currentMode == kEditorPhysicsModeField)
	{
		if (trackData->viewportType == kEditorViewportOrtho)
		{
			Field *field = nullptr;

			switch (currentTool)
			{
				case kEditorFieldBox:

					field = new BoxField(Zero3D);
					break;

				case kEditorFieldCylinder:

					field = new CylinderField(Zero2D, 0.0F);
					break;

				case kEditorFieldSphere:

					field = new SphereField(Zero3D);
					break;
			}

			if (field)
			{
				editor->InitNewNode(trackData, field);
				return (true);
			}
		}
	}
	else if (currentMode == kEditorPhysicsModeBlocker)
	{
		if (trackData->viewportType == kEditorViewportOrtho)
		{
			Blocker *blocker = nullptr;

			switch (currentTool)
			{
				case kEditorBlockerPlate:

					blocker = new PlateBlocker(Zero2D);
					break;

				case kEditorBlockerBox:

					blocker = new BoxBlocker(Zero3D);
					break;

				case kEditorBlockerCylinder:

					blocker = new CylinderBlocker(Zero2D, 0.0F);
					break;

				case kEditorBlockerSphere:

					blocker = new SphereBlocker(Zero3D);
					break;

				case kEditorBlockerCapsule:

					blocker = new CapsuleBlocker(Zero3D, 0.0F);
					break;
			}

			if (blocker)
			{
				editor->InitNewNode(trackData, blocker);
				return (true);
			}
		}
	}

	return (false);
}

bool PhysicsPage::TrackTool(Editor *editor, EditorTrackData *trackData)
{
	editor->AutoScroll(trackData);

	Point2D anchor = trackData->snappedAnchorPosition;
	float dx = trackData->snappedCurrentPosition.x - anchor.x;
	float dy = anchor.y - trackData->snappedCurrentPosition.y;

	if (currentMode == kEditorPhysicsModeSpace)
	{
		float ax = Fabs(dx);
		float ay = Fabs(dy);

		if (InterfaceMgr::GetShiftKey())
		{
			ax = ay = Fmax(ax, ay);
			dx = (dx < 0.0F) ? -ax : ax;
			dy = (dy < 0.0F) ? -ax : ax;
		}

		if ((dx != trackData->currentSize.x) || (dy != trackData->currentSize.y))
		{
			trackData->currentSize.Set(dx, dy);

			unsigned_int32 editorFlags = editor->GetEditorObject()->GetEditorFlags();
			if (editorFlags & kEditorDrawFromCenter)
			{
				anchor.x -= ax;
				anchor.y += ay;
				dx = ax * 2.0F;
				dy = ay * 2.0F;
			}
			else
			{
				if (dx < 0.0F)
				{
					anchor.x += dx;
					dx = -dx;
				}

				if (dy < 0.0F)
				{
					anchor.y -= dy;
					dy = -dy;
				}
			}

			Space *space = static_cast<Space *>(trackData->trackNode);
			space->SetNodePosition(editor->GetTargetSpacePosition(trackData, anchor));

			BoxVolume *box = static_cast<BoxVolume *>(space->GetObject()->GetVolume());
			box->SetBoxSize(Vector3D(dx, dy, Fmax(dx, dy)));

			editor->InvalidateNode(space);
		}

		return ((dx != 0.0F) && (dy != 0.0F));
	}
	else if (currentMode == kEditorPhysicsModeShape)
	{
		float ax = Fabs(dx);
		float ay = Fabs(dy);

		if (InterfaceMgr::GetShiftKey())
		{
			ax = ay = Fmax(ax, ay);
			dx = (dx < 0.0F) ? -ax : ax;
			dy = (dy < 0.0F) ? -ax : ax;
		}

		if ((dx != trackData->currentSize.x) || (dy != trackData->currentSize.y))
		{
			trackData->currentSize.Set(dx, dy);

			unsigned_int32 editorFlags = editor->GetEditorObject()->GetEditorFlags();

			Shape *shape = static_cast<Shape *>(trackData->trackNode);
			ShapeType shapeType = shape->GetShapeType();

			if ((shapeType == kShapeBox) || (shapeType == kShapePyramid) || (shapeType == kShapeTruncatedPyramid))
			{
				if (editorFlags & kEditorDrawFromCenter)
				{
					anchor.x -= ax;
					anchor.y += ay;
					dx = ax * 2.0F;
					dy = ay * 2.0F;
				}
				else
				{
					if (dx < 0.0F)
					{
						anchor.x += dx;
						dx = -dx;
					}

					if (dy < 0.0F)
					{
						anchor.y -= dy;
						dy = -dy;
					}
				}

				shape->SetNodePosition(editor->GetTargetSpacePosition(trackData, anchor));

				switch (shapeType)
				{
					case kShapeBox:
					{
						const BoxShape *box = static_cast<BoxShape *>(shape);
						box->GetObject()->SetBoxSize(Vector3D(dx, dy, Fmax(dx, dy)));
						break;
					}

					case kShapePyramid:
					{
						const PyramidShape *pyramid = static_cast<PyramidShape *>(shape);
						PyramidShapeObject *object = pyramid->GetObject();
						object->SetPyramidSize(Vector2D(dx, dy));
						object->SetPyramidHeight(Fmax(dx, dy));
						break;
					}

					case kShapeTruncatedPyramid:
					{
						const TruncatedPyramidShape *truncatedPyramid = static_cast<TruncatedPyramidShape *>(shape);
						TruncatedPyramidShapeObject *object = truncatedPyramid->GetObject();
						object->SetPyramidSize(Vector2D(dx, dy));
						object->SetPyramidHeight(Fmax(dx, dy));
						break;
					}
				}
			}
			else
			{
				if (editorFlags & kEditorDrawFromCenter)
				{
					dx = ax;
					dy = ay;
				}
				else
				{
					anchor.x += dx * 0.5F;
					anchor.y -= dy * 0.5F;
					dx = ax * 0.5F;
					dy = ay * 0.5F;

					shape->SetNodePosition(editor->GetTargetSpacePosition(trackData, anchor));
				}

				switch (shapeType)
				{
					case kShapeCylinder:
					{
						const CylinderShape *cylinder = static_cast<CylinderShape *>(shape);
						CylinderShapeObject *object = cylinder->GetObject();
						object->SetCylinderSize(Vector2D(dx, dy));
						object->SetCylinderHeight(Fmax(dx, dy) * 2.0F);
						break;
					}

					case kShapeCone:
					{
						const ConeShape *cone = static_cast<ConeShape *>(shape);
						ConeShapeObject *object = cone->GetObject();
						object->SetConeSize(Vector2D(dx, dy));
						object->SetConeHeight(Fmax(dx, dy) * 2.0F);
						break;
					}

					case kShapeSphere:
					{
						const SphereShape *sphere = static_cast<SphereShape *>(shape);
						SphereShapeObject *object = sphere->GetObject();
						object->SetSphereSize(Vector3D(dx, dy, Fmax(dx, dy)));
						break;
					}

					case kShapeDome:
					{
						const DomeShape *dome = static_cast<DomeShape *>(shape);
						DomeShapeObject *object = dome->GetObject();
						object->SetDomeSize(Vector3D(dx, dy, Fmax(dx, dy)));
						break;
					}

					case kShapeCapsule:
					{
						const CapsuleShape *capsule = static_cast<CapsuleShape *>(shape);
						CapsuleShapeObject *object = capsule->GetObject();
						float m = Fmax(dx, dy);
						object->SetCapsuleSize(Vector3D(dx, dy, m));
						object->SetCapsuleHeight(m * 2.0F);
						break;
					}

					case kShapeTruncatedCone:
					{
						const TruncatedConeShape *cone = static_cast<TruncatedConeShape *>(shape);
						TruncatedConeShapeObject *object = cone->GetObject();
						object->SetConeSize(Vector2D(dx, dy));
						object->SetConeHeight(Fmax(dx, dy) * 2.0F);
						break;
					}

					case kShapeTruncatedDome:
					{
						const TruncatedDomeShape *dome = static_cast<TruncatedDomeShape *>(shape);
						TruncatedDomeShapeObject *object = dome->GetObject();
						object->SetDomeSize(Vector2D(dx, dy));
						object->SetDomeHeight(Fmax(dx, dy));
						break;
					}
				}
			}

			editor->InvalidateNode(shape);
		}

		return ((dx != 0.0F) && (dy != 0.0F));
	}
	else if (currentMode == kEditorPhysicsModeJoint)
	{
		if (InterfaceMgr::GetShiftKey())
		{
			if (Fabs(dx) > Fabs(dy))
			{
				dy = 0.0F;
			}
			else
			{
				dx = 0.0F;
			}
		}

		if ((dx != trackData->currentSize.x) || (dy != trackData->currentSize.y))
		{
			trackData->currentSize.Set(dx, dy);

			float scale = trackData->viewportScale * 8.0F;
			if (dx * dx + dy * dy > scale * scale)
			{
				Vector3D direction = editor->GetTargetSpaceDirection(trackData, Vector3D(dx, -dy, 0.0F));
				Vector3D up = editor->GetTargetSpaceDirection(trackData, Vector3D(0.0F, 0.0F, -1.0F));

				direction.Normalize();
				up = Normalize(up - ProjectOnto(up, direction));

				Node *node = trackData->trackNode;
				node->SetNodeMatrix3D(direction, up % direction, up);
				editor->InvalidateNode(node);
			}
		}

		return (true);
	}
	else if (currentMode == kEditorPhysicsModeField)
	{
		float ax = Fabs(dx);
		float ay = Fabs(dy);

		if (InterfaceMgr::GetShiftKey())
		{
			ax = ay = Fmax(ax, ay);
			dx = (dx < 0.0F) ? -ax : ax;
			dy = (dy < 0.0F) ? -ax : ax;
		}

		if ((dx != trackData->currentSize.x) || (dy != trackData->currentSize.y))
		{
			trackData->currentSize.Set(dx, dy);

			unsigned_int32 editorFlags = editor->GetEditorObject()->GetEditorFlags();

			Field *field = static_cast<Field *>(trackData->trackNode);
			FieldType fieldType = field->GetFieldType();

			if (fieldType == kFieldBox)
			{
				if (editorFlags & kEditorDrawFromCenter)
				{
					anchor.x -= ax;
					anchor.y += ay;
					dx = ax * 2.0F;
					dy = ay * 2.0F;
				}
				else
				{
					if (dx < 0.0F)
					{
						anchor.x += dx;
						dx = -dx;
					}

					if (dy < 0.0F)
					{
						anchor.y -= dy;
						dy = -dy;
					}
				}

				field->SetNodePosition(editor->GetTargetSpacePosition(trackData, anchor));

				const BoxField *box = static_cast<BoxField *>(field);
				box->GetObject()->SetBoxSize(Vector3D(dx, dy, Fmax(dx, dy)));
			}
			else
			{
				if (editorFlags & kEditorDrawFromCenter)
				{
					dx = ax;
					dy = ay;
				}
				else
				{
					anchor.x += dx * 0.5F;
					anchor.y -= dy * 0.5F;
					dx = ax * 0.5F;
					dy = ay * 0.5F;

					field->SetNodePosition(editor->GetTargetSpacePosition(trackData, anchor));
				}

				switch (fieldType)
				{
					case kFieldCylinder:
					{
						const CylinderField *cylinder = static_cast<CylinderField *>(field);
						CylinderFieldObject *object = cylinder->GetObject();
						object->SetCylinderSize(Vector2D(dx, dy));
						object->SetCylinderHeight(Fmax(dx, dy) * 2.0F);
						break;
					}

					case kFieldSphere:
					{
						const SphereField *sphere = static_cast<SphereField *>(field);
						SphereFieldObject *object = sphere->GetObject();
						object->SetSphereSize(Vector3D(dx, dy, Fmax(dx, dy)));
						break;
					}
				}
			}

			editor->InvalidateNode(field);
		}

		return ((dx != 0.0F) && (dy != 0.0F));
	}
	else if (currentMode == kEditorPhysicsModeBlocker)
	{
		float ax = Fabs(dx);
		float ay = Fabs(dy);

		if (InterfaceMgr::GetShiftKey())
		{
			ax = ay = Fmax(ax, ay);
			dx = (dx < 0.0F) ? -ax : ax;
			dy = (dy < 0.0F) ? -ax : ax;
		}

		if ((dx != trackData->currentSize.x) || (dy != trackData->currentSize.y))
		{
			trackData->currentSize.Set(dx, dy);

			unsigned_int32 editorFlags = editor->GetEditorObject()->GetEditorFlags();

			Blocker *blocker = static_cast<Blocker *>(trackData->trackNode);
			BlockerType blockerType = blocker->GetBlockerType();

			if ((blockerType == kBlockerPlate) || (blockerType == kBlockerBox))
			{
				if (editorFlags & kEditorDrawFromCenter)
				{
					anchor.x -= ax;
					anchor.y += ay;
					dx = ax * 2.0F;
					dy = ay * 2.0F;
				}
				else
				{
					if (dx < 0.0F)
					{
						anchor.x += dx;
						dx = -dx;
					}

					if (dy < 0.0F)
					{
						anchor.y -= dy;
						dy = -dy;
					}
				}

				blocker->SetNodePosition(editor->GetTargetSpacePosition(trackData, anchor));

				switch (blockerType)
				{
					case kBlockerPlate:
					{
						const PlateBlocker *plate = static_cast<PlateBlocker *>(blocker);
						plate->GetObject()->SetPlateSize(Vector2D(dx, dy));
						break;
					}

					case kBlockerBox:
					{
						const BoxBlocker *box = static_cast<BoxBlocker *>(blocker);
						box->GetObject()->SetBoxSize(Vector3D(dx, dy, Fmax(dx, dy)));
						break;
					}
				}
			}
			else
			{
				if (editorFlags & kEditorDrawFromCenter)
				{
					dx = ax;
					dy = ay;
				}
				else
				{
					anchor.x += dx * 0.5F;
					anchor.y -= dy * 0.5F;
					dx = ax * 0.5F;
					dy = ay * 0.5F;

					blocker->SetNodePosition(editor->GetTargetSpacePosition(trackData, anchor));
				}

				switch (blockerType)
				{
					case kBlockerCylinder:
					{
						const CylinderBlocker *cylinder = static_cast<CylinderBlocker *>(blocker);
						CylinderBlockerObject *object = cylinder->GetObject();
						object->SetCylinderSize(Vector2D(dx, dy));
						object->SetCylinderHeight(Fmax(dx, dy) * 2.0F);
						break;
					}

					case kBlockerSphere:
					{
						const SphereBlocker *sphere = static_cast<SphereBlocker *>(blocker);
						SphereBlockerObject *object = sphere->GetObject();
						object->SetSphereSize(Vector3D(dx, dy, Fmax(dx, dy)));
						break;
					}

					case kBlockerCapsule:
					{
						const CapsuleBlocker *capsule = static_cast<CapsuleBlocker *>(blocker);
						CapsuleBlockerObject *object = capsule->GetObject();
						float m = Fmax(dx, dy);
						object->SetCapsuleSize(Vector3D(dx, dy, m));
						object->SetCapsuleHeight(m * 2.0F);
						break;
					}
				}
			}

			editor->InvalidateNode(blocker);
		}

		return ((dx != 0.0F) && (dy != 0.0F));
	}

	return (false);
}

bool PhysicsPage::EndTool(Editor *editor, EditorTrackData *trackData)
{
	editor->CommitNewNode(trackData, TrackTool(editor, trackData));
	return (true);
}


MaterialPage::MaterialPage() :
		EditorPage(kEditorPageMaterial, "WorldEditor/page/Material", kEditorBookMaterial),
		editorObserver(this, &MaterialPage::HandleEditorEvent),
		toolButtonObserver(this, &MaterialPage::HandleToolButtonEvent),
		materialWidgetObserver(this, &MaterialPage::HandleMaterialWidgetEvent)
{
	currentTool = -1;
}

MaterialPage::~MaterialPage()
{
}

void MaterialPage::Preprocess(void)
{
	static const char *const buttonIdentifier[kEditorMaterialToolCount] =
	{
		"Pickup"
	};

	Editor *editor = GetEditor();
	editor->AddObserver(&editorObserver);

	materialWidget = new MaterialWidget(Vector2D(64.0F, 64.0F), editor->GetSelectedMaterial());
	materialWidget->SetWidgetPosition(Point3D(32.0F, 7.0F, 0.0F));
	materialWidget->SetObserver(&materialWidgetObserver);
	AppendSubnode(materialWidget);

	EditorPage::Preprocess();

	for (machine a = 0; a < kEditorMaterialToolCount; a++)
	{
		toolButton[a] = static_cast<IconButtonWidget *>(FindWidget(buttonIdentifier[a]));
		toolButton[a]->SetObserver(&toolButtonObserver);
	}
}

void MaterialPage::HandleEditorEvent(Editor *editor, const EditorEvent& event)
{
	EditorEventType type = event.GetEventType();
	if (type == kEditorEventMaterialSelected)
	{
		materialWidget->SetMaterial(editor->GetSelectedMaterial());
	}
	else if (type == kEditorEventMaterialModified)
	{
		const MaterialObject *materialObject = static_cast<const MaterialEditorEvent *>(&event)->GetEventMaterialObject();
		if (materialWidget->GetMaterialContainer()->GetMaterialObject() == materialObject)
		{
			materialWidget->UpdatePreview();
		}
	}
}

void MaterialPage::HandleToolButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		GetEditor()->SetCurrentTool(this, widget);
	}
}

void MaterialPage::Engage(Editor *editor, void *cookie)
{
	Widget *widget = static_cast<Widget *>(cookie);

	if (widget == toolButton[kEditorMaterialToolPickup])
	{
		currentTool = 0;
		toolButton[kEditorMaterialToolPickup]->SetValue(1);

		editor->SetCurrentCursor(TheWorldEditor->GetEditorCursor(kEditorCursorDropper));
	}
}

void MaterialPage::Disengage(Editor *editor, void *cookie)
{
	if (currentTool != -1)
	{
		toolButton[currentTool]->SetValue(0);
		currentTool = -1;
	}
}

bool MaterialPage::BeginTool(Editor *editor, EditorTrackData *trackData)
{
	const MaterialObject *materialObject = nullptr;

	if (trackData->viewportType != kEditorViewportGraph)
	{
		PickData	pickData;

		auto filter = [](const Node *node, const PickData *, const void *) -> bool
		{
			return (node->GetNodeType() == kNodeGeometry);
		};

		const Node *node = editor->PickNode(trackData, &pickData, filter);
		if ((node) && (pickData.triangleIndex != kInvalidTriangleIndex))
		{
			const Geometry *geometry = static_cast<const Geometry *>(node);
			materialObject = geometry->GetTriangleMaterial(pickData.triangleIndex);
		}

		pickupNode = nullptr;
	}
	else
	{
		Node *node = Editor::GetManipulator(editor->GetRootNode())->PickGraphNode(trackData, &trackData->worldRay);
		if (node)
		{
			int32	index;

			const Node *pickup = pickupNode;
			if (pickup == node)
			{
				index = pickupIndex + 1;
			}
			else
			{
				pickupNode = node;
				index = 0;
			}

			pickupIndex = index;
			materialObject = Editor::GetManipulator(node)->GetMaterial(index);
		}
	}

	if (materialObject)
	{
		MaterialContainer *container = editor->GetEditorObject()->FindMaterialContainer(materialObject);
		if (container)
		{
			editor->SelectMaterial(container);
		}
	}

	return (false);
}

void MaterialPage::HandleMaterialWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		GetEditor()->OpenMaterialManager();
	}
}


PaintPage::PaintPage() :
		EditorPage(kEditorPagePaint, "WorldEditor/paint/Paint", kEditorBookMaterial),
		editorObserver(this, &PaintPage::HandleEditorEvent),
		paintButtonObserver(this, &PaintPage::HandlePaintButtonEvent),
		channelButtonObserver(this, &PaintPage::HandleChannelButtonEvent),
		colorObserver(this, &PaintPage::HandleColorEvent),
		checkObserver(this, &PaintPage::HandleCheckEvent),
		sliderObserver(this, &PaintPage::HandleSliderEvent),
		menuButtonObserver(this, &PaintPage::HandleMenuButtonEvent)
{
	currentTool = -1;
	targetPaintSpace = nullptr;

	for (machine a = 0; a < 4; a++)
	{
		channelMask[a] = true;
	}

	brushColor.Set(1.0F, 1.0F, 1.0F, 1.0F);

	invertValue = 0;
	stylusValue = 0;
	strengthValue = 100;
	radiusValue = 7;
	fuzzyValue = 50;
}

PaintPage::~PaintPage()
{
}

void PaintPage::Pack(Packer& data, unsigned_int32 packFlags) const
{
	EditorPage::Pack(data, packFlags);

	data << ChunkHeader('MASK', 16);
	for (machine a = 0; a < 4; a++)
	{
		data << channelMask[a];
	}

	data << ChunkHeader('BCOL', sizeof(ColorRGBA));
	data << brushColor;

	data << ChunkHeader('INVT', 4);
	data << invertValue;

	data << ChunkHeader('STYL', 4);
	data << stylusValue;

	data << ChunkHeader('STRE', 4);
	data << strengthValue;

	data << ChunkHeader('RADI', 4);
	data << radiusValue;

	data << ChunkHeader('FUZZ', 4);
	data << fuzzyValue;

	data << TerminatorChunk;
}

void PaintPage::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	EditorPage::Unpack(data, unpackFlags);
	UnpackChunkList<PaintPage>(data, unpackFlags);
}

bool PaintPage::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'MASK':

			for (machine a = 0; a < 4; a++)
			{
				data >> channelMask[a];
			}

			return (true);

		case 'BCOL':

			data >> brushColor;
			return (true);

		case 'INVT':

			data >> invertValue;
			return (true);

		case 'STYL':

			data >> stylusValue;
			return (true);

		case 'STRE':

			data >> strengthValue;
			return (true);

		case 'RADI':

			data >> radiusValue;
			return (true);

		case 'FUZZ':

			data >> fuzzyValue;
			return (true);
	}

	return (false);
}

void PaintPage::Preprocess(void)
{
	static const char *const buttonIdentifier[kEditorPaintToolCount] =
	{
		"Paint"
	};

	static const char *const channelIdentifier[4] =
	{
		"Red", "Green", "Blue", "Alpha"
	};

	EditorPage::Preprocess();
	GetEditor()->AddObserver(&editorObserver);

	paintSpaceButton = static_cast<IconButtonWidget *>(FindWidget("Space"));
	paintSpaceButton->SetObserver(&paintButtonObserver);

	for (machine a = 0; a < kEditorPaintToolCount; a++)
	{
		toolButton[a] = static_cast<IconButtonWidget *>(FindWidget(buttonIdentifier[a]));
		toolButton[a]->SetObserver(&paintButtonObserver);
	}

	for (machine a = 0; a < 4; a++)
	{
		channelButton[a] = static_cast<IconButtonWidget *>(FindWidget(channelIdentifier[a]));
		channelButton[a]->SetObserver(&channelButtonObserver);
	}

	colorWidget = static_cast<ColorWidget *>(FindWidget("Color"));
	invertWidget = static_cast<CheckWidget *>(FindWidget("Invert"));
	stylusWidget = static_cast<CheckWidget *>(FindWidget("Stylus"));
	imageWidget = static_cast<ImageWidget *>(FindWidget("Image"));

	colorWidget->SetObserver(&colorObserver);
	invertWidget->SetObserver(&checkObserver);
	stylusWidget->SetObserver(&checkObserver);

	strengthSlider = static_cast<SliderWidget *>(FindWidget("StrengthValue"));
	strengthText = static_cast<TextWidget *>(FindWidget("StrengthText"));

	radiusSlider = static_cast<SliderWidget *>(FindWidget("RadiusValue"));
	radiusText = static_cast<TextWidget *>(FindWidget("RadiusText"));

	fuzzySlider = static_cast<SliderWidget *>(FindWidget("FuzzyValue"));
	fuzzyText = static_cast<TextWidget *>(FindWidget("FuzzyText"));

	strengthSlider->SetObserver(&sliderObserver);
	radiusSlider->SetObserver(&sliderObserver);
	fuzzySlider->SetObserver(&sliderObserver);

	for (machine a = 0; a < 4; a++)
	{
		channelButton[a]->SetValue(channelMask[a]);
	}

	colorWidget->SetValue(brushColor);

	invertWidget->SetValue(invertValue);
	stylusWidget->SetValue(stylusValue);

	strengthSlider->SetValue(strengthValue);
	radiusSlider->SetValue(radiusValue);
	fuzzySlider->SetValue(fuzzyValue);

	UpdateSlider(strengthSlider);
	UpdateSlider(radiusSlider);
	UpdateSlider(fuzzySlider);

	menuButton = static_cast<IconButtonWidget *>(FindWidget("Menu"));
	menuButton->SetObserver(&menuButtonObserver);

	const StringTable *table = TheWorldEditor->GetStringTable();

	MenuItemWidget *widget = new MenuItemWidget(table->GetString(StringID('PAGE', kEditorPagePaint, 'ASSC')), WidgetObserver<PaintPage>(this, &PaintPage::HandleAssociatePaintSpaceMenuItemEvent));
	paintMenuItem[kPaintMenuAssociatePaintSpace] = widget;
	paintMenuItemList.Append(widget);

	widget = new MenuItemWidget(table->GetString(StringID('PAGE', kEditorPagePaint, 'DSSC')), WidgetObserver<PaintPage>(this, &PaintPage::HandleDissociatePaintSpaceMenuItemEvent));
	paintMenuItem[kPaintMenuDissociatePaintSpace] = widget;
	paintMenuItemList.Append(widget);

	paintMenuItemList.Append(new MenuItemWidget(kLineSolid));

	widget = new MenuItemWidget(table->GetString(StringID('PAGE', kEditorPagePaint, 'SPNT')), WidgetObserver<PaintPage>(this, &PaintPage::HandleSelectAssociatedPaintSpacesMenuItemEvent));
	paintMenuItem[kPaintMenuSelectAssociatedPaintSpaces] = widget;
	paintMenuItemList.Append(widget);

	widget = new MenuItemWidget(table->GetString(StringID('PAGE', kEditorPagePaint, 'SNOD')), WidgetObserver<PaintPage>(this, &PaintPage::HandleSelectAssociatedNodesMenuItemEvent));
	paintMenuItem[kPaintMenuSelectAssociatedNodes] = widget;
	paintMenuItemList.Append(widget);
}

void PaintPage::UpdateSlider(SliderWidget *widget)
{
	if (widget == strengthSlider)
	{
		int32 strength = (int32) (GetBrushStrength() * 100.0F + 0.5F);
		strengthText->SetText(String<15>(strength) += '%');
	}
	else if (widget == radiusSlider)
	{
		radiusText->SetText(Text::FloatToString(GetBrushRadius()));
	}
	else if (widget == fuzzySlider)
	{
		int32 fuzzy = (int32) (GetBrushFuzziness() * 100.0F + 0.5F);
		fuzzyText->SetText(String<15>(fuzzy) += '%');
	}
}

void PaintPage::UpdateImage(void)
{
	if (painter->UpdateImage())
	{
		targetPaintSpace->GetObject()->UpdatePaintTexture(painter->GetPaintBounds());
		GetEditor()->InvalidateViewports(kEditorViewportFrustum);
	}
}

void PaintPage::HandleEditorEvent(Editor *editor, const EditorEvent& event)
{
	if (event.GetEventType() == kEditorEventGizmoMoved)
	{
		Node *node = static_cast<const NodeEditorEvent *>(&event)->GetEventNode();
		if ((node) && (node->GetNodeType() == kNodeSpace))
		{
			Space *space = static_cast<Space *>(node);
			if (space->GetSpaceType() == kSpacePaint)
			{
				SetTargetPaintSpace(static_cast<PaintSpace *>(space));
			}
		}
	}
}

void PaintPage::HandlePaintButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		GetEditor()->SetCurrentTool(this, widget);
	}
}

void PaintPage::HandleChannelButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		IconButtonWidget *iconButtonWidget = static_cast<IconButtonWidget *>(widget);
		for (machine a = 0; a < 4; a++)
		{
			if (channelButton[a] == iconButtonWidget)
			{
				bool mask = (channelMask[a] = !channelMask[a]);
				iconButtonWidget->SetValue(mask);
				break;
			}
		}
	}
}

void PaintPage::HandleColorEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		brushColor = static_cast<ColorWidget *>(widget)->GetValue();
	}
}

void PaintPage::HandleCheckEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		CheckWidget *checkWidget = static_cast<CheckWidget *>(widget);
		int32 value = checkWidget->GetValue();

		if (checkWidget == invertWidget)
		{
			invertValue = value;
		}
		else if (checkWidget == stylusWidget)
		{
			stylusValue = value;
		}
	}
}

void PaintPage::HandleSliderEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		SliderWidget *sliderWidget = static_cast<SliderWidget *>(widget);
		int32 value = sliderWidget->GetValue();

		if (sliderWidget == strengthSlider)
		{
			strengthValue = value;
		}
		else if (sliderWidget == radiusSlider)
		{
			radiusValue = value;
		}
		else if (sliderWidget == fuzzySlider)
		{
			fuzzyValue = value;
		}

		UpdateSlider(static_cast<SliderWidget *>(widget));
	}
}

void PaintPage::HandleMenuButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		int32 paintSpaceCount = 0;
		int32 geometryCount = 0;
		int32 instanceCount = 0;

		const NodeReference *reference = GetEditor()->GetFirstSelectedNode();
		while (reference)
		{
			const Node *node = reference->GetNode();
			NodeType type = node->GetNodeType();
			if (type == kNodeSpace)
			{
				if (static_cast<const Space *>(node)->GetSpaceType() == kSpacePaint)
				{
					paintSpaceCount++;
				}
			}
			else if (type == kNodeGeometry)
			{
				geometryCount++;
			}
			else if (type == kNodeInstance)
			{
				instanceCount++;
			}

			reference = reference->Next();
		}

		if (paintSpaceCount != 0)
		{
			paintMenuItem[kPaintMenuSelectAssociatedNodes]->Enable();
		}
		else
		{
			paintMenuItem[kPaintMenuSelectAssociatedNodes]->Disable();
		}

		if ((geometryCount != 0) || (instanceCount != 0))
		{
			if (paintSpaceCount == 1)
			{
				paintMenuItem[kPaintMenuAssociatePaintSpace]->Enable();
			}
			else
			{
				paintMenuItem[kPaintMenuAssociatePaintSpace]->Disable();
			}

			paintMenuItem[kPaintMenuDissociatePaintSpace]->Enable();
			paintMenuItem[kPaintMenuSelectAssociatedPaintSpaces]->Enable();
		}
		else
		{
			paintMenuItem[kPaintMenuAssociatePaintSpace]->Disable();
			paintMenuItem[kPaintMenuDissociatePaintSpace]->Disable();
			paintMenuItem[kPaintMenuSelectAssociatedPaintSpaces]->Disable();
		}

		Menu *menu = new Menu(kMenuContextual, &paintMenuItemList);
		menu->SetWidgetPosition(menuButton->GetWorldPosition() + Vector3D(25.0F, 0.0F, 0.0F));
		TheInterfaceMgr->SetActiveMenu(menu);
	}
}

void PaintPage::HandleAssociatePaintSpaceMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData)
{
	Editor *editor = GetEditor();

	PaintSpace *paintSpace = nullptr;
	const NodeReference *reference = editor->GetFirstSelectedNode();
	while (reference)
	{
		Node *node = reference->GetNode();
		if (node->GetNodeType() == kNodeSpace)
		{
			Space *space = static_cast<Space *>(node);
			if (space->GetSpaceType() == kSpacePaint)
			{
				paintSpace = static_cast<PaintSpace *>(space);
				break;
			}
		}

		reference = reference->Next();
	}

	if (paintSpace)
	{
		editor->AddOperation(new AssociatePaintSpaceOperation(editor->GetSelectionList()));

		reference = editor->GetFirstSelectedNode();
		while (reference)
		{
			Node *node = reference->GetNode();

			NodeType type = node->GetNodeType();
			if (type == kNodeGeometry)
			{
				Geometry *geometry = static_cast<Geometry *>(node);
				geometry->SetConnectedPaintSpace(paintSpace);
				geometry->InvalidateShaderData();

				Editor::GetManipulator(geometry)->UpdateConnectors();
			}
			else if (type == kNodeInstance)
			{
				Instance *instance = static_cast<Instance *>(node);
				instance->SetConnectedNode(kConnectorKeyPaint, paintSpace);

				InstanceManipulator *manipulator = static_cast<InstanceManipulator *>(Editor::GetManipulator(instance));
				manipulator->InvalidatePaintSpace();
				manipulator->UpdateConnectors();
			}

			reference = reference->Next();
		}
	}
}

void PaintPage::HandleDissociatePaintSpaceMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData)
{
	Editor *editor = GetEditor();
	editor->AddOperation(new AssociatePaintSpaceOperation(editor->GetSelectionList()));

	const NodeReference *reference = editor->GetFirstSelectedNode();
	while (reference)
	{
		Node *node = reference->GetNode();

		NodeType type = node->GetNodeType();
		if (type == kNodeGeometry)
		{
			Geometry *geometry = static_cast<Geometry *>(node);
			geometry->SetConnectedPaintSpace(nullptr);
			geometry->InvalidateShaderData();

			Editor::GetManipulator(geometry)->UpdateConnectors();
		}
		else if (type == kNodeInstance)
		{
			Instance *instance = static_cast<Instance *>(node);
			Connector *connector = instance->GetConnector(kConnectorKeyPaint);
			if (connector)
			{
				connector->SetConnectorTarget(nullptr);

				InstanceManipulator *manipulator = static_cast<InstanceManipulator *>(Editor::GetManipulator(instance));
				manipulator->InvalidatePaintSpace();
				manipulator->UpdateConnectors();
			}
		}

		reference = reference->Next();
	}
}

void PaintPage::HandleSelectAssociatedPaintSpacesMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData)
{
	List<NodeReference>		paintSpaceList;

	Editor *editor = GetEditor();

	const NodeReference *reference = editor->GetFirstSelectedNode();
	while (reference)
	{
		const Node *node = reference->GetNode();
		PaintSpace *paintSpace = nullptr;

		NodeType type = node->GetNodeType();
		if (type == kNodeGeometry)
		{
			paintSpace = static_cast<const Geometry *>(node)->GetConnectedPaintSpace();
		}
		else if (type == kNodeInstance)
		{
			paintSpace = static_cast<PaintSpace *>(node->GetConnectedNode(kConnectorKeyPaint));
		}

		if (paintSpace)
		{
			const NodeReference *paintSpaceReference = paintSpaceList.First();
			while (paintSpaceReference)
			{
				if (paintSpaceReference->GetNode() == paintSpace)
				{
					goto next;
				}

				paintSpaceReference = paintSpaceReference->Next();
			}

			paintSpaceList.Append(new NodeReference(paintSpace));
		}

		next:
		reference = reference->Next();
	}

	editor->UnselectAll();

	reference = paintSpaceList.First();
	while (reference)
	{
		editor->SelectNode(reference->GetNode());
		reference = reference->Next();
	}
}

void PaintPage::HandleSelectAssociatedNodesMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData)
{
	List<NodeReference>		nodeList;

	Editor *editor = GetEditor();

	const NodeReference *reference = editor->GetFirstSelectedNode();
	while (reference)
	{
		const Node *node = reference->GetNode();
		if (node->GetNodeType() == kNodeSpace)
		{
			const Space *space = static_cast<const Space *>(node);
			if (space->GetSpaceType() == kSpacePaint)
			{
				const PaintSpace *paintSpace = static_cast<const PaintSpace *>(space);

				const Hub *hub = paintSpace->GetHub();
				if (hub)
				{
					const Connector *connector = hub->GetFirstIncomingEdge();
					while (connector)
					{
						if (connector->GetConnectorKey() == kConnectorKeyPaint)
						{
							Node *start = connector->GetStartElement()->GetNode();

							const NodeReference *nodeReference = nodeList.First();
							while (nodeReference)
							{
								if (nodeReference->GetNode() == start)
								{
									goto next;
								}

								nodeReference = nodeReference->Next();
							}

							nodeList.Append(new NodeReference(start));
						}

						next:
						connector = connector->GetNextIncomingEdge();
					}
				}
			}
		}

		reference = reference->Next();
	}

	editor->UnselectAll();

	reference = nodeList.First();
	while (reference)
	{
		editor->SelectNode(reference->GetNode());
		reference = reference->Next();
	}
}

void PaintPage::Engage(Editor *editor, void *cookie)
{
	Widget *widget = static_cast<Widget *>(cookie);

	if (widget == paintSpaceButton)
	{
		currentMode = kEditorPaintModeSpace;
		currentTool = 0;
		paintSpaceButton->SetValue(1);
	}
	else
	{
		for (machine a = 0; a < kEditorPaintToolCount; a++)
		{
			if (widget == toolButton[a])
			{
				currentMode = kEditorPaintModeTool;
				currentTool = a;
				toolButton[a]->SetValue(1);
				break;
			}
		}
	}

	editor->SetCurrentCursor(TheWorldEditor->GetEditorCursor(kEditorCursorCross));
}

void PaintPage::Disengage(Editor *editor, void *cookie)
{
	if (currentTool != -1)
	{
		if (currentMode == kEditorPaintModeSpace)
		{
			paintSpaceButton->SetValue(0);
		}
		else if (currentMode == kEditorPaintModeTool)
		{
			toolButton[currentTool]->SetValue(0);
		}

		currentTool = -1;
	}
}

bool PaintPage::PaintPickFilter(const Node *node, const PickData *pickData, const void *cookie)
{
	if ((!Editor::GetManipulator(node)->Hidden()) && (node->GetNodeType() == kNodeGeometry) && (pickData->triangleIndex != kInvalidTriangleIndex))
	{
		const Geometry *geometry = static_cast<const Geometry *>(node);
		const PaintSpace *paintSpace = static_cast<const PaintPage *>(cookie)->GetTargetPaintSpace();
		if ((!paintSpace) || (geometry->GetConnectedPaintSpace() == paintSpace))
		{
			const MaterialObject *materialObject = geometry->GetTriangleMaterial(pickData->triangleIndex);
			if (materialObject)
			{
				const Attribute *attribute = materialObject->GetFirstAttribute();
				if ((attribute) && (attribute->GetAttributeType() == kAttributeShader))
				{
					const Process *process = static_cast<const ShaderAttribute *>(attribute)->GetShaderGraph()->GetFirstElement();
					while (process)
					{
						if (process->GetProcessType() == kProcessPaintTexture)
						{
							return (true);
						}

						process = process->GetNextElement();
					}
				}
			}
		}
	}

	return (false);
}

bool PaintPage::BeginTool(Editor *editor, EditorTrackData *trackData)
{
	if (currentMode == kEditorPaintModeSpace)
	{
		if (trackData->viewportType == kEditorViewportOrtho)
		{
			Space *space = new PaintSpace(Zero3D, Integer2D(128, 128), 1);
			editor->InitNewNode(trackData, space);
			return (true);
		}
	}
	else if (currentMode == kEditorPaintModeTool)
	{
		SetTargetPaintSpace(nullptr);

		if (editor->SetTrackPickFilter(trackData, &PaintPickFilter, this))
		{
			const Geometry *geometry = static_cast<const Geometry *>(trackData->currentPickNode);
			PaintSpace *paintSpace = geometry->GetConnectedPaintSpace();
			if (paintSpace)
			{
				SetTargetPaintSpace(paintSpace);

				paintState.SetChannelMask(channelMask[0], channelMask[1], channelMask[2], channelMask[3]);

				if (invertWidget->GetValue() == 0)
				{
					paintState.SetBrushColor(brushColor);
				}
				else
				{
					float red = 1.0F - brushColor.red;
					float green = 1.0F - brushColor.green;
					float blue = 1.0F - brushColor.blue;
					float alpha = 1.0F - brushColor.alpha;
					paintState.SetBrushColor(ColorRGBA(red, green, blue, alpha));
				}

				paintState.SetBrushRadius(GetBrushRadius());
				paintState.SetBrushFuzziness(GetBrushFuzziness());

				float strength = GetBrushStrength();
				if (stylusWidget->GetValue() != 0)
				{
					strength *= TheEngine->GetStylusPressure();
				}

				paintState.SetBrushOpacity(strength);

				const PaintSpaceObject *object = paintSpace->GetObject();
				const Integer2D& resolution = object->GetPaintResolution();
				painter = new Painter(resolution, object->GetChannelCount(), object->GetPaintImage(), &paintState);

				previousPosition = (paintSpace->GetPaintEnvironment()->paintTransform * trackData->currentPickPoint).GetPoint2D();
				previousPosition.x *= (float) resolution.x;
				previousPosition.y *= (float) resolution.y;

				painter->BeginPainting();
				painter->DrawDot(previousPosition);

				UpdateImage();
				return (true);
			}
		}
	}

	return (false);
}

bool PaintPage::TrackTool(Editor *editor, EditorTrackData *trackData)
{
	editor->AutoScroll(trackData);

	if (currentMode == kEditorPaintModeSpace)
	{
		Point2D anchor = trackData->snappedAnchorPosition;
		float dx = trackData->snappedCurrentPosition.x - anchor.x;
		float dy = anchor.y - trackData->snappedCurrentPosition.y;

		float ax = Fabs(dx);
		float ay = Fabs(dy);

		if (InterfaceMgr::GetShiftKey())
		{
			ax = ay = Fmax(ax, ay);
			dx = (dx < 0.0F) ? -ax : ax;
			dy = (dy < 0.0F) ? -ax : ax;
		}

		if ((dx != trackData->currentSize.x) || (dy != trackData->currentSize.y))
		{
			trackData->currentSize.Set(dx, dy);

			unsigned_int32 editorFlags = editor->GetEditorObject()->GetEditorFlags();
			if (editorFlags & kEditorDrawFromCenter)
			{
				anchor.x -= ax;
				anchor.y += ay;
				dx = ax * 2.0F;
				dy = ay * 2.0F;
			}
			else
			{
				if (dx < 0.0F)
				{
					anchor.x += dx;
					dx = -dx;
				}

				if (dy < 0.0F)
				{
					anchor.y -= dy;
					dy = -dy;
				}
			}

			Space *space = static_cast<Space *>(trackData->trackNode);
			space->SetNodePosition(editor->GetTargetSpacePosition(trackData, anchor));

			BoxVolume *box = static_cast<BoxVolume *>(space->GetObject()->GetVolume());
			box->SetBoxSize(Vector3D(dx, dy, Fmax(dx, dy)));

			editor->InvalidateNode(space);
		}

		return ((dx != 0.0F) && (dy != 0.0F));
	}
	else if (currentMode == kEditorPaintModeTool)
	{
		if (trackData->currentPickNode)
		{
			Point2D position = (targetPaintSpace->GetPaintEnvironment()->paintTransform * trackData->currentPickPoint).GetPoint2D();
			const Integer2D& resolution = targetPaintSpace->GetObject()->GetPaintResolution();
			position.x *= (float) resolution.x;
			position.y *= (float) resolution.y;

			if (SquaredMag(position - previousPosition) > K::min_float)
			{
				if (stylusWidget->GetValue() != 0)
				{
					float strength = GetBrushStrength() * TheEngine->GetStylusPressure();
					paintState.SetBrushOpacity(strength);
				}

				painter->DrawLine(previousPosition, position);
				previousPosition = position;
				UpdateImage();
			}
		}

		return (true);
	}

	return (false);
}

bool PaintPage::EndTool(Editor *editor, EditorTrackData *trackData)
{
	if (currentMode == kEditorPaintModeSpace)
	{
		editor->CommitNewNode(trackData, TrackTool(editor, trackData));
	}
	else if (currentMode == kEditorPaintModeTool)
	{
		bool result = TrackTool(editor, trackData);

		painter->EndPainting();
		editor->AddOperation(new PaintOperation(targetPaintSpace->GetObject(), painter));

		delete painter;
		return (result);
	}

	return (true);
}

void PaintPage::SetTargetPaintSpace(PaintSpace *paintSpace)
{
	targetPaintSpace = paintSpace;

	if (paintSpace)
	{
		imageWidget->SetTexture(0, paintSpace->GetObject()->GetPaintTexture());
		imageWidget->Show();
	}
	else
	{
		Texture *texture = nullptr;
		imageWidget->SetTexture(0, texture);
		imageWidget->Hide();
	}
}


TextureMappingPage::TextureMappingPage() :
		EditorPage(kEditorPageTextureMapping, "WorldEditor/page/TextureMapping", kEditorBookMaterial),
		editorObserver(this, &TextureMappingPage::HandleEditorEvent),
		toolButtonObserver(this, &TextureMappingPage::HandleToolButtonEvent),
		offsetTextObserver(this, &TextureMappingPage::HandleOffsetTextEvent),
		scaleTextObserver(this, &TextureMappingPage::HandleScaleTextEvent),
		rotationTextObserver(this, &TextureMappingPage::HandleRotationTextEvent),
		reflectionBoxObserver(this, &TextureMappingPage::HandleReflectionBoxEvent),
		modeMenuObserver(this, &TextureMappingPage::HandleModeMenuEvent)
{
	currentTool = -1;
	textureOperationType = kTextureOperationNone;
	targetGeometry = nullptr;
}

TextureMappingPage::~TextureMappingPage()
{
}

void TextureMappingPage::Preprocess(void)
{
	static const char *const toolButtonIdentifier[kEditorTextureToolCount] =
	{
		"Offset", "Rotate", "Scale"
	};

	static const char *const offsetIdentifier[2] =
	{
		"Soffset", "Toffset"
	};

	static const char *const scaleIdentifier[2] =
	{
		"Sscale", "Tscale"
	};

	static const char *const modeIdentifier[2] =
	{
		"Salign", "Talign"
	};

	EditorPage::Preprocess();
	GetEditor()->AddObserver(&editorObserver);

	for (machine a = 0; a < kEditorTextureToolCount; a++)
	{
		toolButton[a] = static_cast<IconButtonWidget *>(FindWidget(toolButtonIdentifier[a]));
		toolButton[a]->SetObserver(&toolButtonObserver);
	}

	for (machine a = 0; a < 2; a++)
	{
		offsetTextWidget[a] = static_cast<EditTextWidget *>(FindWidget(offsetIdentifier[a]));
		offsetTextWidget[a]->SetObserver(&offsetTextObserver);

		scaleTextWidget[a] = static_cast<EditTextWidget *>(FindWidget(scaleIdentifier[a]));
		scaleTextWidget[a]->SetObserver(&scaleTextObserver);
	}

	rotationTextWidget = static_cast<EditTextWidget *>(FindWidget("Rot"));
	rotationTextWidget->SetObserver(&rotationTextObserver);

	reflectionCheckWidget = static_cast<CheckWidget *>(FindWidget("Reflect"));
	reflectionCheckWidget->SetObserver(&rotationTextObserver);

	for (machine a = 0; a < 2; a++)
	{
		modePopupMenu[a] = static_cast<PopupMenuWidget *>(FindWidget(modeIdentifier[a]));
		modePopupMenu[a]->SetObserver(&modeMenuObserver);
	}

	planePopupMenu = static_cast<PopupMenuWidget *>(FindWidget("Plane"));
	planePopupMenu->SetObserver(&modeMenuObserver);
}

void TextureMappingPage::HandleEditorEvent(Editor *editor, const EditorEvent& event)
{
	EditorEventType type = event.GetEventType();
	if (type == kEditorEventSelectionUpdated)
	{
		textureOperationType = kTextureOperationNone;
		targetGeometry = nullptr;

		const NodeReference *reference = editor->GetFirstSelectedNode();
		while (reference)
		{
			Node *node = reference->GetNode();
			if (node->GetNodeType() == kNodeGeometry)
			{
				const GeometryManipulator *manipulator = static_cast<GeometryManipulator *>(Editor::GetManipulator(node));
				if (manipulator->GetSelectionType() == kEditorSelectionSurface)
				{
					if (targetGeometry)
					{
						targetGeometry = nullptr;
						break;
					}

					Geometry *geometry = static_cast<Geometry *>(node);
					int32 surfaceCount = geometry->GetObject()->GetSurfaceCount();

					int32 selectionCount = 0;
					for (machine a = 0; a < surfaceCount; a++)
					{
						if (manipulator->SurfaceSelected(a))
						{
							selectionCount++;
							targetSurfaceIndex = a;
						}
					}

					if (selectionCount == 1)
					{
						targetGeometry = geometry;
					}
				}
			}

			reference = reference->Next();
		}

		UpdateTextureAlignData();
	}
	else if (type == kEditorEventTexcoordModified)
	{
		if (targetGeometry == static_cast<const NodeEditorEvent *>(&event)->GetEventNode())
		{
			textureOperationType = kTextureOperationNone;
			UpdateTextureAlignData();
		}
	}
}

void TextureMappingPage::HandleToolButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		GetEditor()->SetCurrentTool(this, widget);
	}
}

void TextureMappingPage::HandleOffsetTextEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		Geometry *geometry = targetGeometry;
		Editor *editor = GetEditor();

		if (textureOperationType != kTextureOperationOffset)
		{
			textureOperationType = kTextureOperationOffset;
			editor->AddOperation(new TextureOperation(geometry));
		}

		const EditTextWidget *editText = static_cast<EditTextWidget *>(widget);
		float f = Text::StringToFloat(editText->GetText());

		const GeometryObject *object = geometry->GetObject();
		TextureAlignData *alignData = object->GetSurfaceData(targetSurfaceIndex)->textureAlignData;

		if (editText == offsetTextWidget[0])
		{
			float offset = f - alignData[0].alignPlane.w;
			alignData[0].alignPlane.w = f;

			if (alignData[0].alignMode == kTextureAlignNatural)
			{
				OffsetTexcoords(object, targetSurfaceIndex, Vector2D(offset, 0.0F));
			}
		}
		else
		{
			float offset = f - alignData[1].alignPlane.w;
			alignData[1].alignPlane.w = f;

			if (alignData[1].alignMode == kTextureAlignNatural)
			{
				OffsetTexcoords(object, targetSurfaceIndex, Vector2D(0.0F, offset));
			}
		}

		editor->RegenerateTexcoords(geometry);
	}
}

void TextureMappingPage::HandleScaleTextEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		Geometry *geometry = targetGeometry;
		Editor *editor = GetEditor();

		if (textureOperationType != kTextureOperationScale)
		{
			textureOperationType = kTextureOperationScale;
			editor->AddOperation(new TextureOperation(geometry));
		}

		const EditTextWidget *editText = static_cast<EditTextWidget *>(widget);
		float f = Fabs(Text::StringToFloat(editText->GetText()));
		if (f > K::min_float)
		{
			const GeometryObject *object = geometry->GetObject();
			TextureAlignData *alignData = object->GetSurfaceData(targetSurfaceIndex)->textureAlignData;

			if (editText == scaleTextWidget[0])
			{
				float scale = f * InverseMag(alignData[0].alignPlane.GetAntivector3D());
				alignData[0].alignPlane.GetAntivector3D() *= scale;

				if (alignData[0].alignMode == kTextureAlignNatural)
				{
					ScaleTexcoords(object, targetSurfaceIndex, Vector2D(scale, 1.0F));
				}
			}
			else
			{
				float scale = f * InverseMag(alignData[1].alignPlane.GetAntivector3D());
				alignData[1].alignPlane.GetAntivector3D() *= scale;

				if (alignData[1].alignMode == kTextureAlignNatural)
				{
					ScaleTexcoords(object, targetSurfaceIndex, Vector2D(1.0F, scale));
				}
			}

			editor->RegenerateTexcoords(geometry);
		}
	}
}

void TextureMappingPage::HandleRotationTextEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		Transform4D		matrix;
		float			angle;

		Geometry *geometry = targetGeometry;
		Editor *editor = GetEditor();

		if (textureOperationType != kTextureOperationRotation)
		{
			textureOperationType = kTextureOperationRotation;
			editor->AddOperation(new TextureOperation(geometry));
		}

		const EditTextWidget *editText = static_cast<EditTextWidget *>(widget);
		float f = Text::StringToFloat(editText->GetText()) * K::radians;

		const GeometryObject *object = geometry->GetObject();
		TextureAlignData *alignData = object->GetSurfaceData(targetSurfaceIndex)->textureAlignData;

		bool snat = (alignData[0].alignMode == kTextureAlignNatural);
		bool tnat = (alignData[1].alignMode == kTextureAlignNatural);
		Vector3D& snormal = alignData[0].alignPlane.GetAntivector3D();
		Vector3D& tnormal = alignData[1].alignPlane.GetAntivector3D();

		if ((snormal.x == 0.0F) && (tnormal.x == 0.0F))
		{
			angle = f - Atan(snormal.z, snormal.y);
			matrix.SetRotationAboutX(angle);
		}
		else if ((snormal.y == 0.0F) && (tnormal.y == 0.0F))
		{
			angle = f - Atan(-snormal.z, snormal.x);
			matrix.SetRotationAboutY(angle);
		}
		else
		{
			angle = f - Atan(snormal.y, snormal.x);
			matrix.SetRotationAboutZ(angle);
		}

		snormal = matrix * snormal;
		tnormal = matrix * tnormal;

		if (snat | tnat)
		{
			matrix.SetRotationAboutZ(angle);

			if (!snat)
			{
				matrix.SetRow(0, Identity3D[0]);
			}

			if (!tnat)
			{
				matrix.SetRow(1, Identity3D[1]);
			}

			RotateTexcoords(object, targetSurfaceIndex, matrix);
		}

		editor->RegenerateTexcoords(geometry);
	}
}

void TextureMappingPage::HandleReflectionBoxEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		Geometry *geometry = targetGeometry;
		Editor *editor = GetEditor();

		if (textureOperationType != kTextureOperationReflection)
		{
			textureOperationType = kTextureOperationReflection;
			editor->AddOperation(new TextureOperation(geometry));
		}

		const GeometryObject *object = geometry->GetObject();
		TextureAlignData *alignData = object->GetSurfaceData(targetSurfaceIndex)->textureAlignData;

		Antivector4D& splane = alignData[0].alignPlane;
		splane = -splane;

		if (alignData[0].alignMode == kTextureAlignNatural)
		{
			ScaleTexcoords(object, targetSurfaceIndex, Vector2D(-1.0F, 1.0F));
		}

		editor->RegenerateTexcoords(geometry);
		UpdateTextureAlignData();
	}
}

void TextureMappingPage::HandleModeMenuEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		Geometry *geometry = targetGeometry;
		Editor *editor = GetEditor();

		if (textureOperationType != kTextureOperationMode)
		{
			textureOperationType = kTextureOperationMode;
			editor->AddOperation(new TextureOperation(geometry));
		}

		const PopupMenuWidget *popupMenu = static_cast<PopupMenuWidget *>(widget);

		const GeometryObject *object = geometry->GetObject();
		TextureAlignData *alignData = object->GetSurfaceData(targetSurfaceIndex)->textureAlignData;

		if (popupMenu == modePopupMenu[0])
		{
			alignData[0].alignMode = alignModeTable[popupMenu->GetSelection()];
		}
		else if (popupMenu == modePopupMenu[1])
		{
			alignData[1].alignMode = alignModeTable[popupMenu->GetSelection()];
		}

		const Antivector4D& splane = alignData[0].alignPlane;
		const Antivector4D& tplane = alignData[1].alignPlane;

		float sx = splane.x;
		float sy = splane.y;
		float tx = tplane.x;
		float ty = tplane.y;

		if ((splane.x == 0.0F) && (tplane.x == 0.0F))
		{
			sx = splane.y;
			sy = splane.z;
			tx = tplane.y;
			ty = tplane.z;
		}
		else if ((splane.y == 0.0F) && (tplane.y == 0.0F))
		{
			sx = splane.x;
			sy = -splane.z;
			tx = tplane.x;
			ty = -tplane.z;
		}

		int32 selection = planePopupMenu->GetSelection();

		if (selection == 0)
		{
			alignData[0].alignPlane.Set(sx, sy, 0.0F, splane.w);
		}
		else if (selection == 1)
		{
			alignData[0].alignPlane.Set(sx, 0.0F, -sy, splane.w);
		}
		else
		{
			alignData[0].alignPlane.Set(0.0F, sx, sy, splane.w);
		}

		if (selection == 0)
		{
			alignData[1].alignPlane.Set(tx, ty, 0.0F, tplane.w);
		}
		else if (selection == 1)
		{
			alignData[1].alignPlane.Set(tx, 0.0F, -ty, tplane.w);
		}
		else
		{
			alignData[1].alignPlane.Set(0.0F, tx, ty, tplane.w);
		}

		editor->RebuildGeometry(geometry);
	}
}

void TextureMappingPage::Engage(Editor *editor, void *cookie)
{
	Widget *widget = static_cast<Widget *>(cookie);

	for (machine a = 0; a < kEditorTextureToolCount; a++)
	{
		if (widget == toolButton[a])
		{
			currentTool = a;
			toolButton[a]->SetValue(1);
			break;
		}
	}
}

void TextureMappingPage::Disengage(Editor *editor, void *cookie)
{
	if (currentTool != -1)
	{
		toolButton[currentTool]->SetValue(0);
		currentTool = -1;
	}
}

bool TextureMappingPage::BeginTool(Editor *editor, EditorTrackData *trackData)
{
	if (trackData->viewportType != kEditorViewportGraph)
	{
		PickData	pickData;

		auto filter = [](const Node *node, const PickData *, const void *) -> bool
		{
			return (node->GetNodeType() == kNodeGeometry);
		};

		Node *node = editor->PickNode(trackData, &pickData, filter);
		if ((node) && (node->GetManipulator()->Selected()))
		{
			operationFlag = false;
			return (true);
		}
	}

	return (false);
}

bool TextureMappingPage::TrackTool(Editor *editor, EditorTrackData *trackData)
{
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
		if (!operationFlag)
		{
			operationFlag = true;
			editor->AddOperation(new TextureOperation(editor->GetSelectionList()));
		}

		Vector2D delta = (trackData->currentPosition - trackData->previousPosition) * 0.125F;

		const NodeReference *reference = editor->GetFirstSelectedNode();
		while (reference)
		{
			Node *node = reference->GetNode();
			if (node->GetNodeType() == kNodeGeometry)
			{
				Geometry *geometry = static_cast<Geometry *>(node);
				const GeometryObject *object = static_cast<Geometry *>(node)->GetObject();
				int32 surfaceCount = object->GetSurfaceCount();

				const GeometryManipulator *manipulator = static_cast<GeometryManipulator *>(Editor::GetManipulator(geometry));
				int32 selectedCount = manipulator->GetSelectedSurfaceCount();

				for (machine a = 0; a < surfaceCount; a++)
				{
					if ((selectedCount == 0) || (manipulator->SurfaceSelected(a)))
					{
						SurfaceData *data = object->GetSurfaceData(a);
						bool snat = (data->textureAlignData[0].alignMode == kTextureAlignNatural);
						bool tnat = (data->textureAlignData[1].alignMode == kTextureAlignNatural);

						if (currentTool == kEditorTextureToolOffset)
						{
							data->textureAlignData[0].alignPlane.w -= delta.x;
							data->textureAlignData[1].alignPlane.w += delta.y;

							if (snat | tnat)
							{
								OffsetTexcoords(object, a, Vector2D((snat) ? -delta.x : 0.0F, (tnat) ? delta.y : 0.0F));
							}
						}
						else if (currentTool == kEditorTextureToolRotate)
						{
							Transform4D		matrix;

							float angle = delta.y;

							Vector3D& snormal = data->textureAlignData[0].alignPlane.GetAntivector3D();
							Vector3D& tnormal = data->textureAlignData[1].alignPlane.GetAntivector3D();

							if ((snormal.x == 0.0F) && (tnormal.x == 0.0F))
							{
								matrix.SetRotationAboutX(angle);
							}
							else if ((snormal.y == 0.0F) && (tnormal.y == 0.0F))
							{
								matrix.SetRotationAboutY(angle);
							}
							else
							{
								matrix.SetRotationAboutZ(angle);
							}

							snormal = matrix * snormal;
							tnormal = matrix * tnormal;

							if (snat | tnat)
							{
								matrix.SetRotationAboutZ(angle);

								if (!snat)
								{
									matrix.SetRow(0, Identity3D[0]);
								}

								if (!tnat)
								{
									matrix.SetRow(1, Identity3D[1]);
								}

								RotateTexcoords(object, a, matrix);
							}
						}
						else if (currentTool == kEditorTextureToolScale)
						{
							float sx = Exp(delta.x * K::ln_2);
							float sy = Exp(-delta.y * K::ln_2);

							data->textureAlignData[0].alignPlane.GetAntivector3D() *= sx;
							data->textureAlignData[1].alignPlane.GetAntivector3D() *= sy;

							if (snat | tnat)
							{
								ScaleTexcoords(object, a, Vector2D((snat) ? sx : 1.0F, (tnat) ? sy : 1.0F));
							}
						}
					}
				}

				editor->RegenerateTexcoords(geometry);
			}

			reference = reference->Next();
		}

		if (targetGeometry)
		{
			UpdateTextureAlignData();
		}
	}

	return (true);
}

bool TextureMappingPage::EndTool(Editor *editor, EditorTrackData *trackData)
{
	return (TrackTool(editor, trackData));
}

void TextureMappingPage::OffsetTexcoords(const GeometryObject *object, unsigned_int32 index, const Vector2D& offset)
{
	int32 levelCount = object->GetGeometryLevelCount();
	for (machine a = 0; a < levelCount; a++)
	{
		Mesh *mesh = object->GetGeometryLevel(a);

		int32 vertexCount = mesh->GetVertexCount();
		Point2D *texcoord = mesh->GetArray<Point2D>(kArrayTexcoord);

		const unsigned_int16 *surfaceIndex = mesh->GetArray<unsigned_int16>(kArraySurfaceIndex);
		if (surfaceIndex)
		{
			for (machine b = 0; b < vertexCount; b++)
			{
				if (surfaceIndex[b] == index)
				{
					texcoord[b] += offset;
				}
			}
		}
		else
		{
			for (machine b = 0; b < vertexCount; b++)
			{
				texcoord[b] += offset;
			}
		}
	}
}

void TextureMappingPage::RotateTexcoords(const GeometryObject *object, unsigned_int32 index, const Transform4D& rotation)
{
	int32 levelCount = object->GetGeometryLevelCount();
	for (machine a = 0; a < levelCount; a++)
	{
		Mesh *mesh = object->GetGeometryLevel(a);

		int32 vertexCount = mesh->GetVertexCount();
		Point2D *texcoord = mesh->GetArray<Point2D>(kArrayTexcoord);

		const unsigned_int16 *surfaceIndex = mesh->GetArray<unsigned_int16>(kArraySurfaceIndex);
		if (surfaceIndex)
		{
			for (machine b = 0; b < vertexCount; b++)
			{
				if (surfaceIndex[b] == index)
				{
					texcoord[b] = rotation * texcoord[b];
				}
			}
		}
		else
		{
			for (machine b = 0; b < vertexCount; b++)
			{
				texcoord[b] = rotation * texcoord[b];
			}
		}
	}
}

void TextureMappingPage::ScaleTexcoords(const GeometryObject *object, unsigned_int32 index, const Vector2D& scale)
{
	int32 levelCount = object->GetGeometryLevelCount();
	for (machine a = 0; a < levelCount; a++)
	{
		Mesh *mesh = object->GetGeometryLevel(a);

		int32 vertexCount = mesh->GetVertexCount();
		Point2D *texcoord = mesh->GetArray<Point2D>(kArrayTexcoord);

		const unsigned_int16 *surfaceIndex = mesh->GetArray<unsigned_int16>(kArraySurfaceIndex);
		if (surfaceIndex)
		{
			for (machine b = 0; b < vertexCount; b++)
			{
				if (surfaceIndex[b] == index)
				{
					texcoord[b] &= scale;
				}
			}
		}
		else
		{
			for (machine b = 0; b < vertexCount; b++)
			{
				texcoord[b] &= scale;
			}
		}
	}
}

void TextureMappingPage::UpdateTextureAlignData(void)
{
	if (targetGeometry)
	{
		const SurfaceData *surfaceData = targetGeometry->GetObject()->GetSurfaceData(targetSurfaceIndex);
		for (machine a = 0; a < 2; a++)
		{
			const TextureAlignData *alignData = &surfaceData->textureAlignData[a];

			offsetTextWidget[a]->SetText(Text::FloatToString(alignData->alignPlane.w));
			offsetTextWidget[a]->Enable();

			scaleTextWidget[a]->SetText(Text::FloatToString(Magnitude(alignData->alignPlane.GetAntivector3D())));
			scaleTextWidget[a]->Enable();

			int32 selection = 0;
			TextureAlignMode mode = alignData->alignMode;
			if (mode == kTextureAlignObjectPlane)
			{
				selection = 1;
			}
			else if (mode == kTextureAlignWorldPlane)
			{
				selection = 2;
			}
			else if (mode == kTextureAlignGlobalObjectPlane)
			{
				selection = 3;
			}

			modePopupMenu[a]->SetSelection(selection);
			modePopupMenu[a]->Enable();
		}

		const Antivector4D& splane = surfaceData->textureAlignData[0].alignPlane;
		const Antivector4D& tplane = surfaceData->textureAlignData[1].alignPlane;

		float sx = splane.x;
		float sy = splane.y;
		float tx = tplane.x;
		float ty = tplane.y;
		int32 selection = 0;

		if ((splane.y == 0.0F) && (tplane.y == 0.0F))
		{
			sx = splane.x;
			sy = -splane.z;
			tx = tplane.x;
			ty = -tplane.z;
			selection = 1;
		}
		else if ((splane.x == 0.0F) && (tplane.x == 0.0F))
		{
			sx = splane.y;
			sy = splane.z;
			tx = tplane.y;
			ty = tplane.z;
			selection = 2;
		}

		rotationTextWidget->SetText(Text::FloatToString(Atan(sy, sx) * K::degrees));
		rotationTextWidget->Enable();

		float d = sx * ty - sy * tx;
		reflectionCheckWidget->SetValue(d < 0.0F);
		reflectionCheckWidget->Enable();

		planePopupMenu->SetSelection(selection);
		planePopupMenu->Enable();
	}
	else
	{
		for (machine a = 0; a < 2; a++)
		{
			offsetTextWidget[a]->SetText(nullptr);
			offsetTextWidget[a]->Disable();

			scaleTextWidget[a]->SetText(nullptr);
			scaleTextWidget[a]->Disable();

			modePopupMenu[a]->SetSelection(kWidgetValueNone);
			modePopupMenu[a]->Disable();
		}

		rotationTextWidget->SetText(nullptr);
		rotationTextWidget->Disable();

		reflectionCheckWidget->SetValue(0);
		reflectionCheckWidget->Disable();

		planePopupMenu->SetSelection(kWidgetValueNone);
		planePopupMenu->Disable();
	}
}


WorldsPage::WorldsPage() :
		EditorPage(kEditorPageWorlds, "WorldEditor/world/Worlds", kEditorBookInstance),
		editorObserver(this, &WorldsPage::HandleEditorEvent),
		worldButtonObserver(this, &WorldsPage::HandleWorldButtonEvent),
		worldListObserver(this, &WorldsPage::HandleWorldListEvent),
		modifierListObserver(this, &WorldsPage::HandleModifierListEvent),
		placeWidgetObserver(this, &WorldsPage::HandlePlaceWidgetEvent),
		menuButtonObserver(this, &WorldsPage::HandleMenuButtonEvent)
{
	currentTool = -1;
	modifierIndex = 0;
	selectPercentage = 50;
	placeFilter = kPlaceDefault;
}

WorldsPage::~WorldsPage()
{
}

WorldsPage::WorldWidget::WorldWidget(const char *text) : TextWidget(text, "font/Normal")
{
}

WorldsPage::WorldWidget::~WorldWidget()
{
}

WorldsPage::ModifierWidget::ModifierWidget(const char *text, const Instance *instance) : TextWidget(text, "font/Normal")
{
	if (instance)
	{
		const Modifier *modifier = instance->GetFirstModifier();
		while (modifier)
		{
			modifierList.Append(modifier->Clone());
			modifier = modifier->Next();
		}
	}
}

WorldsPage::ModifierWidget::~ModifierWidget()
{
}

void WorldsPage::ModifierWidget::Prepack(List<Object> *linkList) const
{
	const Modifier *modifier = modifierList.First();
	while (modifier)
	{
		modifier->Prepack(linkList);
		modifier = modifier->Next();
	}
}

void WorldsPage::ModifierWidget::Pack(Packer& data, unsigned_int32 packFlags) const
{
	PackHandle handle = data.BeginChunk('NAME');
	data << GetText();
	data.EndChunk(handle);

	const Modifier *modifier = modifierList.First();
	while (modifier)
	{
		handle = data.BeginChunk('MDFR');
		modifier->PackType(data);
		modifier->Pack(data, packFlags);
		data.EndChunk(handle);

		modifier = modifier->Next();
	}

	data << TerminatorChunk;
}

void WorldsPage::ModifierWidget::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	UnpackChunkList<ModifierWidget>(data, unpackFlags);
}

bool WorldsPage::ModifierWidget::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'NAME':

			SetText(data.ReadString());
			return (true);

		case 'MDFR':
		{
			Modifier *modifier = Modifier::Create(data, unpackFlags);
			if (modifier)
			{
				modifier->Unpack(++data, unpackFlags);
				modifierList.Append(modifier);
				return (true);
			}

			break;
		}
	}

	return (false);
}

WorldsPage::SelectSomeWindow::SelectSomeWindow(unsigned_int32 percent) : Window("WorldEditor/world/SelectSome")
{
	selectPercentage = percent;
}

WorldsPage::SelectSomeWindow::~SelectSomeWindow()
{
}

void WorldsPage::SelectSomeWindow::Preprocess(void)
{
	Window::Preprocess();

	okayButton = static_cast<PushButtonWidget *>(FindWidget("OK"));
	cancelButton = static_cast<PushButtonWidget *>(FindWidget("Cancel"));

	percentBox = static_cast<EditTextWidget *>(FindWidget("Percent"));
	percentBox->SetText(Text::IntegerToString(selectPercentage));
	SetFocusWidget(percentBox);
}

void WorldsPage::SelectSomeWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		if (widget == okayButton)
		{
			unsigned_int32 percent = Text::StringToInteger(percentBox->GetText());
			if (percent > 0)
			{
				selectPercentage = Min(percent, 100);
				CallCompletionProc();
			}

			Close();
		}
		else if (widget == cancelButton)
		{
			Close();
		}
	}
}

WorldsPage::ModifierPresetWindow::ModifierPresetWindow(WorldsPage *page, const char *name) :
		Window("WorldEditor/world/ModifierPreset"),
		presetName(name)
{
	worldsPage = page;
}

WorldsPage::ModifierPresetWindow::~ModifierPresetWindow()
{
}

void WorldsPage::ModifierPresetWindow::Preprocess(void)
{
	Window::Preprocess();

	okayButton = static_cast<PushButtonWidget *>(FindWidget("OK"));
	cancelButton = static_cast<PushButtonWidget *>(FindWidget("Cancel"));

	nameBox = static_cast<EditTextWidget *>(FindWidget("Name"));
	nameBox->SetText(presetName);
	SetFocusWidget(nameBox);
}

void WorldsPage::ModifierPresetWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	EventType eventType = eventData->eventType;
	if (eventType == kEventWidgetActivate)
	{
		if (widget == okayButton)
		{
			CallCompletionProc();
			Close();
		}
		else if (widget == cancelButton)
		{
			Close();
		}
	}
	else if (eventType == kEventWidgetChange)
	{
		if (widget == nameBox)
		{
			const char *name = nameBox->GetText();
			if (name[0] != 0)
			{
				if ((presetName == name) || (worldsPage->ModifierPresetNameAllowed(name)))
				{
					okayButton->Enable();
				}
				else
				{
					okayButton->Disable();
				}
			}
			else
			{
				okayButton->Disable();
			}
		}
	}
}

void WorldsPage::Prepack(List<Object> *linkList) const
{
	EditorPage::Prepack(linkList);

	const ModifierWidget *modifierWidget = modifierWidgetMap.First();
	while (modifierWidget)
	{
		modifierWidget->Prepack(linkList);
		modifierWidget = modifierWidget->MapElement<ModifierWidget>::Next();
	}
}

void WorldsPage::Pack(Packer& data, unsigned_int32 packFlags) const
{
	EditorPage::Pack(data, packFlags);

	const WorldWidget *worldWidget = worldWidgetMap.First();
	while (worldWidget)
	{
		PackHandle handle = data.BeginChunk('WRLD');
		data << worldWidget->GetText();
		data.EndChunk(handle);

		worldWidget = worldWidget->MapElement<WorldWidget>::Next();
	}

	const ModifierWidget *modifierWidget = modifierWidgetMap.First();
	while (modifierWidget)
	{
		PackHandle handle = data.BeginChunk('MDFR');
		modifierWidget->Pack(data, packFlags);
		data.EndChunk(handle);

		modifierWidget = modifierWidget->MapElement<ModifierWidget>::Next();
	}

	if (modifierIndex != 0)
	{
		data << ChunkHeader('MIDX', 4);
		data << modifierIndex;
	}

	data << ChunkHeader('PCNT', 4);
	data << selectPercentage;

	data << ChunkHeader('PFLT', 4);
	data << placeFilter;

	data << TerminatorChunk;
}

void WorldsPage::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	EditorPage::Unpack(data, unpackFlags);
	UnpackChunkList<WorldsPage>(data, unpackFlags);
}

bool WorldsPage::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'WRLD':
		{
			ResourcePath	text;

			data >> text;
			AddWorldWidget(text);
			return (true);
		}

		case 'MDFR':
		{
			ModifierWidget *modifierWidget = new ModifierWidget;
			modifierWidget->Unpack(data, unpackFlags);
			modifierWidgetMap.Insert(modifierWidget);
			return (true);
		}

		case 'MIDX':

			data >> modifierIndex;
			return (true);

		case 'PCNT':

			data >> selectPercentage;
			return (true);

		case 'PFLT':

			data >> placeFilter;
			return (true);
	}

	return (false);
}

void WorldsPage::Preprocess(void)
{
	EditorPage::Preprocess();
	GetEditor()->AddObserver(&editorObserver);

	worldButton = static_cast<IconButtonWidget *>(FindWidget("World"));
	worldButton->SetObserver(&worldButtonObserver);

	worldList = static_cast<ListWidget *>(FindWidget("List"));
	worldList->SetObserver(&worldListObserver);
	worldList->SetWidgetUsage(worldList->GetWidgetUsage() & ~kWidgetKeyboardFocus);

	modifierList = static_cast<ListWidget *>(FindWidget("Modifiers"));
	modifierList->SetObserver(&modifierListObserver);
	modifierList->SetWidgetUsage(modifierList->GetWidgetUsage() & ~kWidgetKeyboardFocus);

	placeWidget = static_cast<CheckWidget *>(FindWidget("Place"));
	if (placeFilter == kPlaceDefault)
	{
		placeWidget->SetValue(1);
	}

	placeWidget->SetObserver(&placeWidgetObserver);

	menuButton = static_cast<IconButtonWidget *>(FindWidget("Menu"));
	menuButton->SetObserver(&menuButtonObserver);

	const StringTable *table = TheWorldEditor->GetStringTable();

	worldMenuItemList.Append(new MenuItemWidget(table->GetString(StringID('PAGE', kEditorPageWorlds, 'CLEN')), WidgetObserver<WorldsPage>(this, &WorldsPage::HandleCleanupMenuItemEvent)));
	worldMenuItemList.Append(new MenuItemWidget(kLineSolid));

	MenuItemWidget *widget = new MenuItemWidget(table->GetString(StringID('PAGE', kEditorPageWorlds, 'SALL')), WidgetObserver<WorldsPage>(this, &WorldsPage::HandleSelectAllMenuItemEvent));
	worldMenuItem[kWorldMenuSelectAll] = widget;
	worldMenuItemList.Append(widget);

	widget = new MenuItemWidget(table->GetString(StringID('PAGE', kEditorPageWorlds, 'SSOM')), WidgetObserver<WorldsPage>(this, &WorldsPage::HandleSelectSomeMenuItemEvent));
	worldMenuItem[kWorldMenuSelectSome] = widget;
	worldMenuItemList.Append(widget);

	worldMenuItemList.Append(new MenuItemWidget(kLineSolid));

	widget = new MenuItemWidget(table->GetString(StringID('PAGE', kEditorPageWorlds, 'EXAL')), WidgetObserver<WorldsPage>(this, &WorldsPage::HandleExpandAllInSceneGraphMenuItemEvent));
	worldMenuItem[kWorldMenuExpandAllInSceneGraph] = widget;
	worldMenuItemList.Append(widget);

	widget = new MenuItemWidget(table->GetString(StringID('PAGE', kEditorPageWorlds, 'COAL')), WidgetObserver<WorldsPage>(this, &WorldsPage::HandleCollapseAllInSceneGraphMenuItemEvent));
	worldMenuItem[kWorldMenuCollapseAllInSceneGraph] = widget;
	worldMenuItemList.Append(widget);

	worldMenuItemList.Append(new MenuItemWidget(kLineSolid));

	widget = new MenuItemWidget(table->GetString(StringID('PAGE', kEditorPageWorlds, 'RPLI')), WidgetObserver<WorldsPage>(this, &WorldsPage::HandleReplaceInstancesMenuItemEvent));
	worldMenuItem[kWorldMenuReplaceInstances] = widget;
	worldMenuItemList.Append(widget);

	widget = new MenuItemWidget(table->GetString(StringID('PAGE', kEditorPageWorlds, 'RPLM')), WidgetObserver<WorldsPage>(this, &WorldsPage::HandleReplaceModifiersMenuItemEvent));
	worldMenuItem[kWorldMenuReplaceModifiers] = widget;
	worldMenuItemList.Append(widget);

	worldMenuItemList.Append(new MenuItemWidget(kLineSolid));

	widget = new MenuItemWidget(table->GetString(StringID('PAGE', kEditorPageWorlds, 'NWMD')), WidgetObserver<WorldsPage>(this, &WorldsPage::HandleNewModifierPresetMenuItemEvent));
	worldMenuItem[kWorldMenuNewModifierPreset] = widget;
	worldMenuItemList.Append(widget);

	widget = new MenuItemWidget(table->GetString(StringID('PAGE', kEditorPageWorlds, 'DLMD')), WidgetObserver<WorldsPage>(this, &WorldsPage::HandleDeleteModifierPresetMenuItemEvent));
	worldMenuItem[kWorldMenuDeleteModifierPreset] = widget;
	worldMenuItemList.Append(widget);

	widget = new MenuItemWidget(table->GetString(StringID('PAGE', kEditorPageWorlds, 'RNMD')), WidgetObserver<WorldsPage>(this, &WorldsPage::HandleRenameModifierPresetMenuItemEvent));
	worldMenuItem[kWorldMenuRenameModifierPreset] = widget;
	worldMenuItemList.Append(widget);

	BuildWorldList();

	BuildModifierList();
	modifierList->SelectListItem(modifierIndex);
}

void WorldsPage::AddWorldWidget(const char *text)
{
	if (text[0] != 0)
	{
		MapReservation		reservation;

		if (worldWidgetMap.Reserve(text, &reservation))
		{
			WorldWidget *widget = new WorldWidget(text);
			worldWidgetMap.Insert(widget, &reservation);
		}
	}
}

void WorldsPage::AddInstances(void)
{
	const Zone *root = GetEditor()->GetRootNode();

	const Node *node = root->GetFirstSubnode();
	while (node)
	{
		if (node->GetNodeType() == kNodeInstance)
		{
			const Instance *instance = static_cast<const Instance *>(node);
			AddWorldWidget(instance->GetWorldName());

			node = root->GetNextLevelNode(node);
		}
		else
		{
			node = root->GetNextNode(node);
		}
	}
}

void WorldsPage::BuildWorldList(void)
{
	WorldWidget *widget = worldWidgetMap.First();
	while (widget)
	{
		if (widget->GetSuperNode())
		{
			worldList->RemoveListItem(widget);
		}

		widget = widget->MapElement<WorldWidget>::Next();
	}

	Vector2D size = worldList->GetNaturalListItemSize();

	widget = worldWidgetMap.First();
	while (widget)
	{
		widget->SetWidgetSize(size);
		worldList->AppendListItem(widget);
		widget = widget->MapElement<WorldWidget>::Next();
	}
}

void WorldsPage::BuildModifierList(void)
{
	delete modifierList->GetFirstListItem();

	ModifierWidget *widget = modifierWidgetMap.First();
	while (widget)
	{
		if (widget->GetSuperNode())
		{
			modifierList->RemoveListItem(widget);
		}

		widget = widget->MapElement<ModifierWidget>::Next();
	}

	Vector2D size = worldList->GetNaturalListItemSize();

	widget = modifierWidgetMap.First();
	while (widget)
	{
		widget->SetWidgetSize(size);
		modifierList->AppendListItem(widget);
		widget = widget->MapElement<ModifierWidget>::Next();
	}

	widget = new ModifierWidget(TheWorldEditor->GetStringTable()->GetString(StringID('PAGE', kEditorPageWorlds, 'NONE')));
	widget->SetWidgetSize(size);
	modifierList->PrependListItem(widget);
}

bool WorldsPage::PlacePickFilter(const Node *node, const PickData *pickData, const void *cookie)
{
	return ((!Editor::GetManipulator(node)->Hidden()) && (!(node->GetNodeFlags() & kNodeNonpersistent)));
}

void WorldsPage::SelectAllInstances(Editor *editor, const char *worldName)
{
	auto filter = [](const Node *node, const void *cookie) -> bool
	{
		if (node->GetNodeType() == kNodeInstance)
		{
			return (Text::CompareText(static_cast<const Instance *>(node)->GetWorldName(), static_cast<const char *>(cookie)));
		}

		return (false);
	};

	editor->SelectAllFilter(filter, worldName);
}

int32 WorldsPage::GatherInstances(Editor *editor, const char *worldName, List<NodeReference> *worldList)
{
	const Zone *root = editor->GetRootNode();
	int32 count = 0;

	Node *node = root->GetFirstSubnode();
	while (node)
	{
		if (!(node->GetNodeFlags() & kNodeNonpersistent))
		{
			if (node->GetNodeType() == kNodeInstance)
			{
				Instance *instance = static_cast<Instance *>(node);
				if (Text::CompareText(instance->GetWorldName(), worldName))
				{
					worldList->Append(new NodeReference(instance));
					count++;
				}

				node = root->GetNextLevelNode(node);
			}
			else
			{
				node = root->GetNextNode(node);
			}
		}
		else
		{
			node = root->GetNextLevelNode(node);
		}
	}

	return (count);
}

bool WorldsPage::ModifiersEqual(const Instance *instance, const List<Modifier> *modifierList)
{
	const Modifier *instanceModifier = instance->GetFirstModifier();
	while (instanceModifier)
	{
		const Modifier *modifier = modifierList->First();
		while (modifier)
		{
			if (*instanceModifier == *modifier)
			{
				break;
			}

			modifier = modifier->Next();
		}

		if (!modifier)
		{
			return (false);
		}

		instanceModifier = instanceModifier->Next();
	}

	const Modifier *modifier = modifierList->First();
	while (modifier)
	{
		instanceModifier = instance->GetFirstModifier();
		while (instanceModifier)
		{
			if (*modifier == *instanceModifier)
			{
				break;
			}

			instanceModifier = instanceModifier->Next();
		}

		if (!instanceModifier)
		{
			return (false);
		}

		modifier = modifier->Next();
	}

	return (true);
}

void WorldsPage::HandleCleanupMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData)
{
	if (worldList->GetFirstSelectedListItem())
	{
		GetEditor()->SetCurrentTool(this, worldButton);
	}

	worldList->PurgeListItems();
	AddInstances();
	BuildWorldList();
}

void WorldsPage::HandleSelectAllMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData)
{
	Editor *editor = GetEditor();
	editor->UnselectAll();

	const Widget *listItem = worldList->GetFirstSelectedListItem();
	while (listItem)
	{
		SelectAllInstances(editor, static_cast<const WorldWidget *>(listItem)->GetText());
		listItem = worldList->GetNextSelectedListItem(listItem);
	}
}

void WorldsPage::HandleSelectSomeMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData)
{
	SelectSomeWindow *window = new SelectSomeWindow(selectPercentage);
	window->SetCompletionProc(&SelectSomeComplete, this);
	GetEditor()->AddSubwindow(window);
}

void WorldsPage::HandleExpandAllInSceneGraphMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData)
{
	Editor *editor = GetEditor();
	Zone *root = editor->GetRootNode();

	Node *node = root->GetFirstSubnode();
	while (node)
	{
		if (node->GetNodeType() == kNodeInstance)
		{
			Instance *instance = static_cast<Instance *>(node);

			const Widget *listItem = worldList->GetFirstSelectedListItem();
			while (listItem)
			{
				const char *worldName = static_cast<const WorldWidget *>(listItem)->GetText();
				if (Text::CompareText(instance->GetWorldName(), worldName))
				{
					Editor::GetManipulator(instance)->ExpandSubgraph();
					break;
				}

				listItem = worldList->GetNextSelectedListItem(listItem);
			}

			node = root->GetNextLevelNode(node);
		}
		else
		{
			node = root->GetNextNode(node);
		}
	}

	editor->InvalidateViewports(kEditorViewportGraph);
}

void WorldsPage::HandleCollapseAllInSceneGraphMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData)
{
	Editor *editor = GetEditor();
	Zone *root = editor->GetRootNode();

	Node *node = root->GetFirstSubnode();
	while (node)
	{
		if (node->GetNodeType() == kNodeInstance)
		{
			Instance *instance = static_cast<Instance *>(node);

			const Widget *listItem = worldList->GetFirstSelectedListItem();
			while (listItem)
			{
				const char *worldName = static_cast<const WorldWidget *>(listItem)->GetText();
				if (Text::CompareText(instance->GetWorldName(), worldName))
				{
					Editor::GetManipulator(instance)->CollapseSubgraph();
					break;
				}

				listItem = worldList->GetNextSelectedListItem(listItem);
			}

			node = root->GetNextLevelNode(node);
		}
		else
		{
			node = root->GetNextNode(node);
		}
	}

	editor->InvalidateViewports(kEditorViewportGraph);
}

void WorldsPage::HandleReplaceInstancesMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData)
{
	Editor *editor = GetEditor();
	editor->AddOperation(new ReplaceInstanceOperation(editor->GetSelectionList()));

	const WorldWidget *worldWidget = static_cast<WorldWidget *>(worldList->GetFirstSelectedListItem());
	const char *worldName = worldWidget->GetText();

	const NodeReference *reference = editor->GetFirstSelectedNode();
	while (reference)
	{
		Node *node = reference->GetNode();
		if (node->GetNodeType() == kNodeInstance)
		{
			Instance *instance = static_cast<Instance *>(node);
			instance->Collapse();
			instance->SetWorldName(worldName);
			editor->ExpandWorld(instance);
		}

		reference = reference->Next();
	}
}

void WorldsPage::HandleReplaceModifiersMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData)
{
	Editor *editor = GetEditor();
	editor->AddOperation(new ReplaceModifiersOperation(editor->GetSelectionList()));

	const ModifierWidget *modifierWidget = static_cast<ModifierWidget *>(modifierList->GetFirstSelectedListItem());

	const NodeReference *reference = GetEditor()->GetFirstSelectedNode();
	while (reference)
	{
		Node *node = reference->GetNode();
		if (node->GetNodeType() == kNodeInstance)
		{
			Instance *instance = static_cast<Instance *>(node);
			instance->PurgeModifiers();

			const Modifier *modifier = modifierWidget->modifierList.First();
			while (modifier)
			{
				instance->AddModifier(modifier->Clone());
				modifier = modifier->Next();
			}

			Editor::GetManipulator(instance)->HandleSettingsUpdate();
		}

		reference = reference->Next();
	}
}

void WorldsPage::HandleNewModifierPresetMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData)
{
	ModifierPresetWindow *window = new ModifierPresetWindow(this);
	window->SetCompletionProc(&NewModifierPresetComplete, this);
	GetEditor()->AddSubwindow(window);
}

void WorldsPage::HandleDeleteModifierPresetMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData)
{
	delete modifierList->GetFirstSelectedListItem();

	BuildModifierList();
	modifierIndex = 0;
	modifierList->SelectListItem(0);
}

void WorldsPage::HandleRenameModifierPresetMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData)
{
	ModifierPresetWindow *window = new ModifierPresetWindow(this, static_cast<ModifierWidget *>(modifierList->GetFirstSelectedListItem())->GetText());
	window->SetCompletionProc(&RenameModifierPresetComplete, this);
	GetEditor()->AddSubwindow(window);
}

void WorldsPage::SelectSomeComplete(SelectSomeWindow *window, void *cookie)
{
	WorldsPage *page = static_cast<WorldsPage *>(cookie);

	Editor *editor = page->GetEditor();
	editor->UnselectAll();

	const Widget *listItem = page->worldList->GetFirstSelectedListItem();
	while (listItem)
	{
		List<NodeReference>		worldList;

		const char *worldName = static_cast<const WorldWidget *>(listItem)->GetText();
		int32 totalCount = GatherInstances(editor, worldName, &worldList);

		unsigned_int32 percent = window->GetSelectPercentage();
		page->selectPercentage = percent;
		int32 selectCount = (totalCount * percent + 50) / 100;

		for (machine a = 0; a < selectCount; a++)
		{
			NodeReference *reference = worldList[Math::Random(totalCount)];
			editor->SelectNode(reference->GetNode());

			delete reference;
			totalCount--;
		}

		listItem = page->worldList->GetNextSelectedListItem(listItem);
	}
}

void WorldsPage::NewModifierPresetComplete(ModifierPresetWindow *window, void *cookie)
{
	WorldsPage *worldsPage = static_cast<WorldsPage *>(cookie);

	const NodeReference *reference = worldsPage->GetEditor()->GetFirstSelectedNode();
	while (reference)
	{
		const Node *node = reference->GetNode();
		if (node->GetNodeType() == kNodeInstance)
		{
			const Instance *instance = static_cast<const Instance *>(node);

			ModifierWidget *widget = new ModifierWidget(window->GetModifierPresetName(), instance);
			worldsPage->modifierWidgetMap.Insert(widget);

			worldsPage->BuildModifierList();
			int32 index = widget->GetNodeIndex();
			worldsPage->modifierIndex = index;
			worldsPage->modifierList->SelectListItem(index);

			break;
		}

		reference = reference->Next();
	}
}

void WorldsPage::RenameModifierPresetComplete(ModifierPresetWindow *window, void *cookie)
{
	WorldsPage *worldsPage = static_cast<WorldsPage *>(cookie);

	ModifierWidget *modifierWidget = static_cast<ModifierWidget *>(worldsPage->modifierList->GetFirstSelectedListItem());
	modifierWidget->SetText(window->GetModifierPresetName());
	worldsPage->BuildModifierList();
}

bool WorldsPage::ModifierPresetNameAllowed(const char *name) const
{
	return ((!modifierWidgetMap.Find(name)) && (!Text::CompareText(name, static_cast<TextWidget *>(modifierList->GetFirstListItem())->GetText())));
}

void WorldsPage::HandleEditorEvent(Editor *editor, const EditorEvent& event)
{
	EditorEventType type = event.GetEventType();
	if ((type == kEditorEventNodesPasted) || (type == kEditorEventNodeInfoModified))
	{
		const NodeReference *reference = editor->GetFirstSelectedNode();
		while (reference)
		{
			const Node *node = reference->GetNode();
			if (node->GetNodeType() == kNodeInstance)
			{
				AddWorldWidget(static_cast<const Instance *>(node)->GetWorldName());
			}

			reference = reference->Next();
		}

		if (worldList->GetFirstSelectedListItem())
		{
			GetEditor()->SetCurrentTool(this, worldButton);
		}

		BuildWorldList();
	}
}

void WorldsPage::HandleWorldButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		GetEditor()->SetCurrentTool(this, widget);
	}
}

void WorldsPage::HandleWorldListEvent(Widget *widget, const WidgetEventData *eventData)
{
	EventType eventType = eventData->eventType;

	if (eventType == kEventWidgetActivate)
	{
		HandleSelectAllMenuItemEvent(nullptr, nullptr);
	}
	else if (eventType == kEventWidgetChange)
	{
		if (worldList->GetFirstSelectedListItem())
		{
			GetEditor()->SetCurrentTool(this, widget);
		}
		else
		{
			GetEditor()->SetCurrentTool(this, worldButton);
		}
	}
}

void WorldsPage::HandleModifierListEvent(Widget *widget, const WidgetEventData *eventData)
{
	EventType eventType = eventData->eventType;

	if (eventType == kEventWidgetActivate)
	{
		Editor *editor = GetEditor();
		editor->UnselectAll();

		auto filter = [](const Node *node, const void *cookie) -> bool
		{
			if (node->GetNodeType() == kNodeInstance)
			{
				return (ModifiersEqual(static_cast<const Instance *>(node), static_cast<const List<Modifier> *>(cookie)));
			}

			return (false);
		};

		const ModifierWidget *modifierWidget = static_cast<ModifierWidget *>(modifierList->GetFirstSelectedListItem());
		editor->SelectAllFilter(filter, &modifierWidget->modifierList);
	}
	else if (eventType == kEventWidgetChange)
	{
		const Widget *listItem = modifierList->GetFirstSelectedListItem();
		modifierIndex = (listItem) ? listItem->GetNodeIndex() : 0;
	}
}

void WorldsPage::HandlePlaceWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (static_cast<CheckWidget *>(widget)->GetValue() != 0)
	{
		placeFilter = kPlaceDefault;
	}
	else
	{
		placeFilter = kPlaceIgnoreInstances;
	}
}

void WorldsPage::HandleMenuButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		for (machine a = 0; a < kWorldMenuItemCount; a++)
		{
			worldMenuItem[a]->Disable();
		}

		int32 worldCount = worldList->GetSelectedListItemCount();
		if (worldCount > 0)
		{
			worldMenuItem[kWorldMenuSelectAll]->Enable();
			worldMenuItem[kWorldMenuSelectSome]->Enable();
			worldMenuItem[kWorldMenuExpandAllInSceneGraph]->Enable();
			worldMenuItem[kWorldMenuCollapseAllInSceneGraph]->Enable();
		}

		int32 instanceCount = 0;
		const NodeReference *reference = GetEditor()->GetFirstSelectedNode();
		while (reference)
		{
			if (reference->GetNode()->GetNodeType() == kNodeInstance)
			{
				instanceCount++;
			}

			reference = reference->Next();
		}

		if (instanceCount > 0)
		{
			worldMenuItem[kWorldMenuReplaceModifiers]->Enable();

			if (worldCount == 1)
			{
				worldMenuItem[kWorldMenuReplaceInstances]->Enable();
			}

			if (instanceCount == 1)
			{
				worldMenuItem[kWorldMenuNewModifierPreset]->Enable();
			}
		}

		const Widget *listItem = modifierList->GetFirstSelectedListItem();
		if ((listItem) && (listItem->Previous()))
		{
			worldMenuItem[kWorldMenuDeleteModifierPreset]->Enable();
			worldMenuItem[kWorldMenuRenameModifierPreset]->Enable();
		}

		Menu *menu = new Menu(kMenuContextual, &worldMenuItemList);
		menu->SetWidgetPosition(menuButton->GetWorldPosition() + Vector3D(25.0F, 0.0F, 0.0F));
		TheInterfaceMgr->SetActiveMenu(menu);
	}
}

void WorldsPage::Engage(Editor *editor, void *cookie)
{
	Widget *widget = static_cast<Widget *>(cookie);

	if (widget == worldButton)
	{
		currentTool = 0;
		worldButton->SetValue(1);
	}
	else
	{
		currentTool = -1;
	}

	editor->SetCurrentCursor(TheWorldEditor->GetEditorCursor(kEditorCursorCross));
}

void WorldsPage::Disengage(Editor *editor, void *cookie)
{
	if (currentTool != -1)
	{
		worldButton->SetValue(0);
		currentTool = -1;
	}
	else
	{
		if (cookie != worldList)
		{
			worldList->UnselectAllListItems();
		}
	}
}

bool WorldsPage::BeginTool(Editor *editor, EditorTrackData *trackData)
{
	if ((placeFilter != kPlaceIgnoreInstances) || (editor->SetTrackPickFilter(trackData, &PlacePickFilter, this)))
	{
		if ((trackData->viewportType != kEditorViewportGraph) || (trackData->superNode))
		{
			Instance *instance = nullptr;

			if (currentTool != -1)
			{
				instance = new Instance("");
			}
			else
			{
				int32 count = worldList->GetSelectedListItemCount();
				if (count > 0)
				{
					int32 index = (count == 1) ? 0 : Math::Random(count);
					const Widget *listItem = worldList->GetSelectedListItem(index);
					instance = new Instance(static_cast<const WorldWidget *>(listItem)->GetText());
				}
			}

			if (instance)
			{
				const Widget *listItem = modifierList->GetFirstSelectedListItem();
				if ((listItem) && (listItem->Previous()))
				{
					const ModifierWidget *modifierWidget = static_cast<const ModifierWidget *>(listItem);
					const Modifier *modifier = modifierWidget->modifierList.First();
					while (modifier)
					{
						instance->AddModifier(modifier->Clone());
						modifier = modifier->Next();
					}
				}

				editor->InitNewNode(trackData, instance);
				editor->ExpandWorld(instance);

				if (trackData->viewportType == kEditorViewportOrtho)
				{
					return (true);
				}

				editor->CommitNewNode(trackData, true);
			}
		}
	}

	return (false);
}

bool WorldsPage::TrackTool(Editor *editor, EditorTrackData *trackData)
{
	editor->AutoScroll(trackData);

	float dx = trackData->currentPosition.x - trackData->snappedAnchorPosition.x;
	float dy = trackData->snappedAnchorPosition.y - trackData->currentPosition.y;

	if (InterfaceMgr::GetShiftKey())
	{
		if (Fabs(dx) > Fabs(dy))
		{
			dy = 0.0F;
		}
		else
		{
			dx = 0.0F;
		}
	}

	if ((dx != trackData->currentSize.x) || (dy != trackData->currentSize.y))
	{
		trackData->currentSize.Set(dx, dy);

		float scale = trackData->viewportScale * 8.0F;
		if (dx * dx + dy * dy > scale * scale)
		{
			Vector3D direction = editor->GetTargetSpaceDirection(trackData, Vector3D(dx, -dy, 0.0F));
			Vector3D up = editor->GetTargetSpaceDirection(trackData, Vector3D(0.0F, 0.0F, -1.0F));

			direction.Normalize();
			up = Normalize(up - ProjectOnto(up, direction));

			Node *node = trackData->trackNode;
			node->SetNodeMatrix3D(direction, up % direction, up);
			editor->InvalidateNode(node);
		}
	}

	return (true);
}

bool WorldsPage::EndTool(Editor *editor, EditorTrackData *trackData)
{
	editor->CommitNewNode(trackData, TrackTool(editor, trackData));
	return (true);
}


ModelsPage::ModelsPage() :
		EditorPage(kEditorPageModels, "WorldEditor/model/Models", kEditorBookInstance),
		modelButtonObserver(this, &ModelsPage::HandleModelButtonEvent),
		modelListObserver(this, &ModelsPage::HandleModelListEvent),
		menuButtonObserver(this, &ModelsPage::HandleMenuButtonEvent)
{
	currentTool = -1;
	selectPercentage = 50;
}

ModelsPage::~ModelsPage()
{
}

ModelsPage::SelectSomeWindow::SelectSomeWindow(unsigned_int32 percent) : Window("WorldEditor/model/SelectSome")
{
	selectPercentage = percent;
}

ModelsPage::SelectSomeWindow::~SelectSomeWindow()
{
}

void ModelsPage::SelectSomeWindow::Preprocess(void)
{
	Window::Preprocess();

	okayButton = static_cast<PushButtonWidget *>(FindWidget("OK"));
	cancelButton = static_cast<PushButtonWidget *>(FindWidget("Cancel"));

	percentBox = static_cast<EditTextWidget *>(FindWidget("Percent"));
	percentBox->SetText(Text::IntegerToString(selectPercentage));
	SetFocusWidget(percentBox);
}

void ModelsPage::SelectSomeWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		if (widget == okayButton)
		{
			unsigned_int32 percent = Text::StringToInteger(percentBox->GetText());
			if (percent > 0)
			{
				selectPercentage = Min(percent, 100);
				CallCompletionProc();
			}

			Close();
		}
		else if (widget == cancelButton)
		{
			Close();
		}
	}
}

void ModelsPage::Pack(Packer& data, unsigned_int32 packFlags) const
{
	EditorPage::Pack(data, packFlags);

	data << ChunkHeader('PCNT', 4);
	data << selectPercentage;

	data << TerminatorChunk;
}

void ModelsPage::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	EditorPage::Unpack(data, unpackFlags);
	UnpackChunkList<ModelsPage>(data, unpackFlags);
}

bool ModelsPage::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'PCNT':

			data >> selectPercentage;
			return (true);
	}

	return (false);
}

void ModelsPage::Preprocess(void)
{
	EditorPage::Preprocess();

	modelButton = static_cast<IconButtonWidget *>(FindWidget("Model"));
	modelButton->SetObserver(&modelButtonObserver);

	modelList = static_cast<ListWidget *>(FindWidget("List"));
	modelList->SetObserver(&modelListObserver);
	modelList->SetWidgetUsage(modelList->GetWidgetUsage() & ~kWidgetKeyboardFocus);

	menuButton = static_cast<IconButtonWidget *>(FindWidget("Menu"));
	menuButton->SetObserver(&menuButtonObserver);

	const StringTable *table = TheWorldEditor->GetStringTable();

	MenuItemWidget *widget = new MenuItemWidget(table->GetString(StringID('PAGE', kEditorPageModels, 'SALL')), WidgetObserver<ModelsPage>(this, &ModelsPage::HandleSelectAllMenuItemEvent));
	modelMenuItem[kModelMenuSelectAll] = widget;
	modelMenuItemList.Append(widget);

	widget = new MenuItemWidget(table->GetString(StringID('PAGE', kEditorPageModels, 'SSOM')), WidgetObserver<ModelsPage>(this, &ModelsPage::HandleSelectSomeMenuItemEvent));
	modelMenuItem[kModelMenuSelectSome] = widget;
	modelMenuItemList.Append(widget);

	modelMenuItemList.Append(new MenuItemWidget(kLineSolid));

	widget = new MenuItemWidget(table->GetString(StringID('PAGE', kEditorPageModels, 'EXAL')), WidgetObserver<ModelsPage>(this, &ModelsPage::HandleExpandAllInSceneGraphMenuItemEvent));
	modelMenuItem[kModelMenuExpandAllInSceneGraph] = widget;
	modelMenuItemList.Append(widget);

	widget = new MenuItemWidget(table->GetString(StringID('PAGE', kEditorPageModels, 'COAL')), WidgetObserver<ModelsPage>(this, &ModelsPage::HandleCollapseAllInSceneGraphMenuItemEvent));
	modelMenuItem[kModelMenuCollapseAllInSceneGraph] = widget;
	modelMenuItemList.Append(widget);

	BuildModelList();
}

void ModelsPage::BuildModelList(void)
{
	Vector2D size = modelList->GetNaturalListItemSize();

	const ModelRegistration *registration = Model::GetFirstRegistration();
	while (registration)
	{
		if (!(registration->GetModelFlags() & kModelPrivate))
		{
			modelList->InsertSortedListItem(new TypeWidget(size, registration->GetModelName(), registration->GetModelType()), &Text::CompareNumberedTextLessThanCaseless);
		}

		registration = registration->Next();
	}
}

void ModelsPage::SelectAllModels(Editor *editor, ModelType type)
{
	auto filter = [](const Node *node, const void *cookie) -> bool
	{
		if (node->GetNodeType() == kNodeModel)
		{
			return (static_cast<const Model *>(node)->GetModelType() == *static_cast<const ModelType *>(cookie));
		}

		return (false);
	};

	editor->SelectAllFilter(filter, &type);
}

int32 ModelsPage::GatherModels(Editor *editor, ModelType type, List<NodeReference> *modelList)
{
	const Zone *root = editor->GetRootNode();
	int32 count = 0;

	Node *node = root->GetFirstSubnode();
	while (node)
	{
		if (!(node->GetNodeFlags() & kNodeNonpersistent))
		{
			if (node->GetNodeType() == kNodeModel)
			{
				Model *model = static_cast<Model *>(node);
				if (model->GetModelType() == type)
				{
					modelList->Append(new NodeReference(model));
					count++;
				}

				node = root->GetNextLevelNode(node);
			}
			else
			{
				node = root->GetNextNode(node);
			}
		}
		else
		{
			node = root->GetNextLevelNode(node);
		}
	}

	return (count);
}

void ModelsPage::HandleSelectAllMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData)
{
	Editor *editor = GetEditor();
	editor->UnselectAll();

	const Widget *listItem = modelList->GetFirstSelectedListItem();
	while (listItem)
	{
		SelectAllModels(editor, static_cast<const TypeWidget *>(listItem)->GetItemType());
		listItem = modelList->GetNextSelectedListItem(listItem);
	}
}

void ModelsPage::HandleSelectSomeMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData)
{
	SelectSomeWindow *window = new SelectSomeWindow(selectPercentage);
	window->SetCompletionProc(&SelectSomeComplete, this);
	GetEditor()->AddSubwindow(window);
}

void ModelsPage::HandleExpandAllInSceneGraphMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData)
{
	Editor *editor = GetEditor();
	Zone *root = editor->GetRootNode();

	Node *node = root->GetFirstSubnode();
	while (node)
	{
		if (node->GetNodeType() == kNodeModel)
		{
			Model *model = static_cast<Model *>(node);

			const Widget *listItem = modelList->GetFirstSelectedListItem();
			while (listItem)
			{
				ModelType modelType = static_cast<const TypeWidget *>(listItem)->GetItemType();
				if (model->GetModelType() == modelType)
				{
					Editor::GetManipulator(model)->ExpandSubgraph();
					break;
				}

				listItem = modelList->GetNextSelectedListItem(listItem);
			}

			node = root->GetNextLevelNode(node);
		}
		else
		{
			node = root->GetNextNode(node);
		}
	}

	editor->InvalidateViewports(kEditorViewportGraph);
}

void ModelsPage::HandleCollapseAllInSceneGraphMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData)
{
	Editor *editor = GetEditor();
	Zone *root = editor->GetRootNode();

	Node *node = root->GetFirstSubnode();
	while (node)
	{
		if (node->GetNodeType() == kNodeModel)
		{
			Model *model = static_cast<Model *>(node);

			const Widget *listItem = modelList->GetFirstSelectedListItem();
			while (listItem)
			{
				ModelType modelType = static_cast<const TypeWidget *>(listItem)->GetItemType();
				if (model->GetModelType() == modelType)
				{
					Editor::GetManipulator(model)->CollapseSubgraph();
					break;
				}

				listItem = modelList->GetNextSelectedListItem(listItem);
			}

			node = root->GetNextLevelNode(node);
		}
		else
		{
			node = root->GetNextNode(node);
		}
	}

	editor->InvalidateViewports(kEditorViewportGraph);
}

void ModelsPage::SelectSomeComplete(SelectSomeWindow *window, void *cookie)
{
	ModelsPage *page = static_cast<ModelsPage *>(cookie);

	Editor *editor = page->GetEditor();
	editor->UnselectAll();

	const Widget *listItem = page->modelList->GetFirstSelectedListItem();
	while (listItem)
	{
		List<NodeReference>		modelList;

		ModelType modelType = static_cast<const TypeWidget *>(listItem)->GetItemType();
		int32 totalCount = GatherModels(editor, modelType, &modelList);

		unsigned_int32 percent = window->GetSelectPercentage();
		page->selectPercentage = percent;
		int32 selectCount = (totalCount * percent + 50) / 100;

		for (machine a = 0; a < selectCount; a++)
		{
			NodeReference *reference = modelList[Math::Random(totalCount)];
			editor->SelectNode(reference->GetNode());

			delete reference;
			totalCount--;
		}

		listItem = page->modelList->GetNextSelectedListItem(listItem);
	}
}

void ModelsPage::HandleModelButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		GetEditor()->SetCurrentTool(this, widget);
	}
}

void ModelsPage::HandleModelListEvent(Widget *widget, const WidgetEventData *eventData)
{
	EventType eventType = eventData->eventType;

	if (eventType == kEventWidgetActivate)
	{
		HandleSelectAllMenuItemEvent(nullptr, nullptr);
	}
	else if (eventType == kEventWidgetChange)
	{
		GetEditor()->SetCurrentTool(this, widget);
	}
}

void ModelsPage::HandleMenuButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		for (machine a = 0; a < kModelMenuItemCount; a++)
		{
			modelMenuItem[a]->Disable();
		}

		if (modelList->GetFirstSelectedListItem())
		{
			modelMenuItem[kModelMenuSelectAll]->Enable();
			modelMenuItem[kModelMenuSelectSome]->Enable();
			modelMenuItem[kModelMenuExpandAllInSceneGraph]->Enable();
			modelMenuItem[kModelMenuCollapseAllInSceneGraph]->Enable();
		}

		Menu *menu = new Menu(kMenuContextual, &modelMenuItemList);
		menu->SetWidgetPosition(menuButton->GetWorldPosition() + Vector3D(25.0F, 0.0F, 0.0F));
		TheInterfaceMgr->SetActiveMenu(menu);
	}
}

void ModelsPage::Engage(Editor *editor, void *cookie)
{
	Widget *widget = static_cast<Widget *>(cookie);

	if (widget == modelButton)
	{
		currentTool = 0;
		modelButton->SetValue(1);
	}
	else
	{
		currentTool = -1;
	}

	editor->SetCurrentCursor(TheWorldEditor->GetEditorCursor(kEditorCursorCross));
}

void ModelsPage::Disengage(Editor *editor, void *cookie)
{
	if (currentTool != -1)
	{
		modelButton->SetValue(0);
		currentTool = -1;
	}
	else
	{
		if (cookie != modelList)
		{
			modelList->UnselectAllListItems();
		}
	}
}

bool ModelsPage::BeginTool(Editor *editor, EditorTrackData *trackData)
{
	if ((trackData->viewportType != kEditorViewportGraph) || (trackData->superNode))
	{
		if (currentTool != -1)
		{
			Model *model = new GenericModel("");
			editor->InitNewNode(trackData, model);

			if (trackData->viewportType == kEditorViewportOrtho)
			{
				return (true);
			}

			editor->CommitNewNode(trackData, true);
		}
		else
		{
			const Widget *listItem = modelList->GetFirstSelectedListItem();
			if (listItem)
			{
				Type modelType = static_cast<const TypeWidget *>(listItem)->GetItemType();
				Model *model = new Model(modelType);

				const ModelRegistration *registration = Model::FindRegistration(modelType);
				if (registration)
				{
					ControllerType controllerType = registration->GetControllerType();
					if (controllerType != 0)
					{
						model->SetController(Controller::New(controllerType));
					}

					int32 propertyCount = registration->GetPropertyCount();
					const PropertyType *propertyType = registration->GetPropertyTypeArray();
					for (machine a = 0; a < propertyCount; a++)
					{
						Property *property = Property::New(propertyType[a]);
						if (property)
						{
							model->AddProperty(property);
						}
					}
				}

				editor->InitNewNode(trackData, model);
				editor->ExpandModel(model);

				if (trackData->viewportType == kEditorViewportOrtho)
				{
					return (true);
				}

				editor->CommitNewNode(trackData, true);
			}
		}
	}

	return (false);
}

bool ModelsPage::TrackTool(Editor *editor, EditorTrackData *trackData)
{
	editor->AutoScroll(trackData);

	float dx = trackData->currentPosition.x - trackData->snappedAnchorPosition.x;
	float dy = trackData->snappedAnchorPosition.y - trackData->currentPosition.y;

	if (InterfaceMgr::GetShiftKey())
	{
		if (Fabs(dx) > Fabs(dy))
		{
			dy = 0.0F;
		}
		else
		{
			dx = 0.0F;
		}
	}

	if ((dx != trackData->currentSize.x) || (dy != trackData->currentSize.y))
	{
		trackData->currentSize.Set(dx, dy);

		float scale = trackData->viewportScale * 8.0F;
		if (dx * dx + dy * dy > scale * scale)
		{
			Vector3D direction = editor->GetTargetSpaceDirection(trackData, Vector3D(dx, -dy, 0.0F));
			Vector3D up = editor->GetTargetSpaceDirection(trackData, Vector3D(0.0F, 0.0F, -1.0F));

			direction.Normalize();
			up = Normalize(up - ProjectOnto(up, direction));

			Node *node = trackData->trackNode;
			node->SetNodeMatrix3D(direction, up % direction, up);
			editor->InvalidateNode(node);
		}
	}

	return (true);
}

bool ModelsPage::EndTool(Editor *editor, EditorTrackData *trackData)
{
	editor->CommitNewNode(trackData, TrackTool(editor, trackData));
	return (true);
}


ImpostorsPage::ImpostorsPage() :
		EditorPage(kEditorPageImpostors, "WorldEditor/impostor/Impostors", kEditorBookInstance),
		impostorButtonObserver(this, &ImpostorsPage::HandleImpostorButtonEvent)
{
	currentTool = -1;
}

ImpostorsPage::~ImpostorsPage()
{
}

void ImpostorsPage::Preprocess(void)
{
	EditorPage::Preprocess();

	impostorButton = static_cast<IconButtonWidget *>(FindWidget("Impostor"));
	impostorButton->SetObserver(&impostorButtonObserver);
}

void ImpostorsPage::HandleImpostorButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		GetEditor()->SetCurrentTool(this, widget);
	}
}

void ImpostorsPage::Engage(Editor *editor, void *cookie)
{
	Widget *widget = static_cast<Widget *>(cookie);

	if (widget == impostorButton)
	{
		currentTool = 0;
		impostorButton->SetValue(1);
	}

	editor->SetCurrentCursor(TheWorldEditor->GetEditorCursor(kEditorCursorCross));
}

void ImpostorsPage::Disengage(Editor *editor, void *cookie)
{
	if (currentTool != -1)
	{
		impostorButton->SetValue(0);
		currentTool = -1;
	}
}

bool ImpostorsPage::BeginTool(Editor *editor, EditorTrackData *trackData)
{
	if ((trackData->viewportType != kEditorViewportGraph) || (trackData->superNode))
	{
		if (currentTool == 0)
		{
			editor->InitNewNode(trackData, new Impostor);

			if (trackData->viewportType == kEditorViewportOrtho)
			{
				return (true);
			}

			editor->CommitNewNode(trackData, true);
		}
	}

	return (false);
}

bool ImpostorsPage::TrackTool(Editor *editor, EditorTrackData *trackData)
{
	editor->AutoScroll(trackData);
	return (true);
}

bool ImpostorsPage::EndTool(Editor *editor, EditorTrackData *trackData)
{
	editor->CommitNewNode(trackData, TrackTool(editor, trackData));
	return (true);
}


PlacementPage::PlacementPage() :
		EditorPage(kEditorPagePlacement, "WorldEditor/page/Placement", kEditorBookInstance),
		placementWidgetObserver(this, &PlacementPage::HandlePlacementWidgetEvent),
		applyButtonObserver(this, &PlacementPage::HandleApplyButtonEvent)
{
	enableValue = 0;
}

PlacementPage::~PlacementPage()
{
}

void PlacementPage::Pack(Packer& data, unsigned_int32 packFlags) const
{
	EditorPage::Pack(data, packFlags);

	data << ChunkHeader('ENAB', 4);
	data << enableValue;

	PackHandle handle = data.BeginChunk('AJST');
	placementAdjuster.Pack(data, packFlags);
	data.EndChunk(handle);

	data << TerminatorChunk;
}

void PlacementPage::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	EditorPage::Unpack(data, unpackFlags);
	UnpackChunkList<PlacementPage>(data, unpackFlags);
}

bool PlacementPage::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'ENAB':

			data >> enableValue;
			return (true);

		case 'AJST':

			placementAdjuster.Unpack(data, unpackFlags);
			return (true);
	}

	return (false);
}

void PlacementPage::Preprocess(void)
{
	EditorPage::Preprocess();

	enableWidget = static_cast<CheckWidget *>(FindWidget("Enable"));
	rotateWidget = static_cast<CheckWidget *>(FindWidget("Rotate"));
	tangentWidget = static_cast<CheckWidget *>(FindWidget("Tangent"));
	sinkWidget = static_cast<CheckWidget *>(FindWidget("Sink"));
	radiusWidget = static_cast<EditTextWidget *>(FindWidget("Radius"));
	minOffsetWidget = static_cast<EditTextWidget *>(FindWidget("MinOffset"));
	maxOffsetWidget = static_cast<EditTextWidget *>(FindWidget("MaxOffset"));
	applyButton = static_cast<PushButtonWidget *>(FindWidget("Apply"));

	enableWidget->SetObserver(&placementWidgetObserver);
	rotateWidget->SetObserver(&placementWidgetObserver);
	tangentWidget->SetObserver(&placementWidgetObserver);
	sinkWidget->SetObserver(&placementWidgetObserver);
	radiusWidget->SetObserver(&placementWidgetObserver);
	minOffsetWidget->SetObserver(&placementWidgetObserver);
	maxOffsetWidget->SetObserver(&placementWidgetObserver);
	applyButton->SetObserver(&applyButtonObserver);

	if (enableValue != 0)
	{
		enableWidget->SetValue(1);
	}

	unsigned_int32 flags = placementAdjuster.GetAdjusterFlags();

	if (flags & PlacementAdjuster::kAdjusterRandomRotation)
	{
		rotateWidget->SetValue(1);
	}

	if (flags & PlacementAdjuster::kAdjusterTangentPlane)
	{
		tangentWidget->SetValue(1);
	}

	if (flags & PlacementAdjuster::kAdjusterGroundSink)
	{
		sinkWidget->SetValue(1);
	}

	radiusWidget->SetText(Text::FloatToString(placementAdjuster.GetGroundSinkRadius()));
	minOffsetWidget->SetText(Text::FloatToString(placementAdjuster.GetOffsetRange().min));
	maxOffsetWidget->SetText(Text::FloatToString(placementAdjuster.GetOffsetRange().max));

	GetEditor()->InstallPlacementAdjuster(&AdjustPlacement, this);
}

void PlacementPage::HandlePlacementWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		if (widget == enableWidget)
		{
			enableValue = enableWidget->GetValue();
		}
		else if (widget == rotateWidget)
		{
			unsigned_int32 flags = placementAdjuster.GetAdjusterFlags();
			if (rotateWidget->GetValue() != 0)
			{
				flags |= PlacementAdjuster::kAdjusterRandomRotation;
			}
			else
			{
				flags &= ~PlacementAdjuster::kAdjusterRandomRotation;
			}

			placementAdjuster.SetAdjusterFlags(flags);
		}
		else if (widget == tangentWidget)
		{
			unsigned_int32 flags = placementAdjuster.GetAdjusterFlags();
			if (tangentWidget->GetValue() != 0)
			{
				flags |= PlacementAdjuster::kAdjusterTangentPlane;
			}
			else
			{
				flags &= ~PlacementAdjuster::kAdjusterTangentPlane;
			}

			placementAdjuster.SetAdjusterFlags(flags);
		}
		else if (widget == sinkWidget)
		{
			unsigned_int32 flags = placementAdjuster.GetAdjusterFlags();
			if (sinkWidget->GetValue() != 0)
			{
				flags |= PlacementAdjuster::kAdjusterGroundSink;
			}
			else
			{
				flags &= ~PlacementAdjuster::kAdjusterGroundSink;
			}

			placementAdjuster.SetAdjusterFlags(flags);
		}
		else if (widget == radiusWidget)
		{
			placementAdjuster.SetGroundSinkRadius(FmaxZero(Text::StringToFloat(radiusWidget->GetText())));
		}
		else if (widget == minOffsetWidget)
		{
			const Range<float>& range = placementAdjuster.GetOffsetRange();
			placementAdjuster.SetOffsetRange(Range<float>(Text::StringToFloat(minOffsetWidget->GetText()), range.max));
		}
		else if (widget == maxOffsetWidget)
		{
			const Range<float>& range = placementAdjuster.GetOffsetRange();
			placementAdjuster.SetOffsetRange(Range<float>(range.min, Text::StringToFloat(maxOffsetWidget->GetText())));
		}
	}
}

void PlacementPage::HandleApplyButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		bool operationFlag = false;

		Editor *editor = GetEditor();
		const World *world = editor->GetEditorWorld();

		const NodeReference *reference = editor->GetFirstSelectedNode();
		while (reference)
		{
			Node *node = reference->GetNode();
			if (Editor::GetManipulator(node)->GetManipulatorFlags() & kManipulatorAdjustablePlacement)
			{
				if (!operationFlag)
				{
					operationFlag = true;
					editor->AddOperation(new MoveOperation(editor->GetSelectionList()));
				}

				Transform4D transform = node->GetWorldTransform();
				placementAdjuster.ApplyRandomRotation(&transform);

				unsigned_int32 flags = node->GetNodeFlags();
				node->SetNodeFlags(flags | kNodeDisabled);
				placementAdjuster.ApplyTangentPlane(world, &transform);
				node->SetNodeFlags(flags);

				placementAdjuster.ApplyGroundSink(world, &transform);

				node->SetNodeTransform(node->GetSuperNode()->GetInverseWorldTransform() * transform);
				editor->InvalidateNode(node);
			}

			reference = reference->Next();
		}
	}
}

void PlacementPage::AdjustPlacement(const Editor *editor, Transform4D *transform, const Vector3D& normal, void *cookie)
{
	const PlacementPage *page = static_cast<PlacementPage *>(cookie);
	if (page->enableValue != 0)
	{
		page->placementAdjuster.AdjustPlacement(editor->GetEditorWorld(), transform, normal);
	}
}


NodeManagementPage::NodeManagementPage() :
		EditorPage(kEditorPageNodeManagement, "WorldEditor/page/NodeManagement", kEditorBookEditor),
		editorObserver(this, &NodeManagementPage::HandleEditorEvent),
		treeWidgetObserver(this, &NodeManagementPage::HandleTreeWidgetEvent),
		allWidgetObserver(this, &NodeManagementPage::HandleAllWidgetEvent),
		showButtonObserver(this, &NodeManagementPage::HandleShowButtonEvent),
		hideButtonObserver(this, &NodeManagementPage::HandleHideButtonEvent),
		selectButtonObserver(this, &NodeManagementPage::HandleSelectButtonEvent)
{
}

NodeManagementPage::~NodeManagementPage()
{
}

NodeManagementPage::NodeTreeItemWidget::NodeTreeItemWidget(NodeManagementPage *page, SelectionMask *mask, const char *text) :
		TreeItemWidget(new CheckWidget(Zero2D, text, "font/Gui")),
		checkObserver(this, &NodeTreeItemWidget::HandleCheckEvent)
{
	nodeManagementPage = page;
	selectionMask = mask;

	CheckWidget *checkWidget = static_cast<CheckWidget *>(GetItemWidget());
	checkWidget->SetCheckFlags(checkWidget->GetCheckFlags() | kCheckTextInactive);
	checkWidget->SetObserver(&checkObserver);
}

NodeManagementPage::NodeTreeItemWidget::~NodeTreeItemWidget()
{
}

void NodeManagementPage::NodeTreeItemWidget::HandleCheckEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		if (!InterfaceMgr::GetCommandKey())
		{
			CheckWidget *checkWidget = static_cast<CheckWidget *>(widget);
			selectionMask->SetMaskValue(checkWidget->GetValue());
		}
		else
		{
			SelectionMask *mask = nodeManagementPage->GetEditor()->GetEditorObject()->GetSelectionMask()->GetFirstSubnode();
			while (mask)
			{
				mask->SetMaskValue(0);
				mask = mask->Next();
			}

			selectionMask->SetMaskValue(1);
		}

		nodeManagementPage->UpdateTreeItems();
	}
}

void NodeManagementPage::Preprocess(void)
{
	EditorPage::Preprocess();

	Editor *editor = GetEditor();
	editor->AddObserver(&editorObserver);

	treeWidget = static_cast<TreeWidget *>(FindWidget("Tree"));
	allWidget = static_cast<CheckWidget *>(FindWidget("All"));
	showButton = static_cast<PushButtonWidget *>(FindWidget("Show"));
	hideButton = static_cast<PushButtonWidget *>(FindWidget("Hide"));
	selectButton = static_cast<PushButtonWidget *>(FindWidget("Select"));

	treeWidget->SetObserver(&treeWidgetObserver);
	allWidget->SetObserver(&allWidgetObserver);
	showButton->SetObserver(&showButtonObserver);
	hideButton->SetObserver(&hideButtonObserver);
	selectButton->SetObserver(&selectButtonObserver);

	const StringTable *stringTable = TheWorldEditor->GetStringTable();
	StringID stringID('NAME', 0);

	const SelectionMask *selectionMask = editor->GetEditorObject()->GetSelectionMask();
	SelectionMask *mask = selectionMask->GetFirstSubnode();
	while (mask)
	{
		stringID[1] = mask->GetNodeType();

		NodeTreeItemWidget *item = new NodeTreeItemWidget(this, mask, stringTable->GetString(stringID));
		BuildNodeTree(mask, item, stringID);
		treeWidget->AppendTreeItem(item);

		mask = mask->Next();
	}

	UpdateTreeItems();
}

void NodeManagementPage::BuildNodeTree(const SelectionMask *selectionMask, TreeItemWidget *treeItem, StringID& stringID)
{
	const StringTable *stringTable = TheWorldEditor->GetStringTable();

	int32 size = stringID.GetSize();
	stringID.SetSize(size + 1);

	SelectionMask *mask = selectionMask->GetFirstSubnode();
	while (mask)
	{
		stringID[size] = mask->GetNodeType();

		NodeTreeItemWidget *item = new NodeTreeItemWidget(this, mask, stringTable->GetString(stringID));
		BuildNodeTree(mask, item, stringID);
		treeItem->AppendSubnode(item);

		mask = mask->Next();
	}

	stringID.SetSize(size);
}

void NodeManagementPage::UpdateTreeItems(void)
{
	NodeTreeItemWidget *item = static_cast<NodeTreeItemWidget *>(treeWidget->GetFirstListItem());
	while (item)
	{
		NodeTreeItemWidget *subitem = item;
		do
		{
			int32 value = subitem->GetMaskValue();
			static_cast<CheckWidget *>(subitem->GetItemWidget())->SetValue(value);

			subitem = static_cast<NodeTreeItemWidget *>(item->Tree<TreeItemWidget>::GetNextNode(subitem));
		} while (subitem);

		item = static_cast<NodeTreeItemWidget *>(item->Widget::Next());
	}

	allWidget->SetValue(GetEditor()->GetEditorObject()->GetSelectionMask()->GetMaskValue());
}

void NodeManagementPage::HandleEditorEvent(Editor *editor, const EditorEvent& event)
{
	if (event.GetEventType() == kEditorEventNodeCreated)
	{
		Type	packType[kMaxNodeTypeSize];

		Package package(packType, sizeof(Type) * kMaxNodeTypeSize);
		Packer packer(&package);

		static_cast<const NodeEditorEvent *>(&event)->GetEventNode()->PackType(packer);
		int32 size = package.GetSize() / sizeof(Type) - 1;
		int32 i = 0;

		SelectionMask *mask = editor->GetEditorObject()->GetSelectionMask()->GetFirstSubnode();
		while (mask)
		{
			if (mask->GetNodeType() == packType[i])
			{
				int32 value = mask->GetMaskValue();
				if (value == 1)
				{
					break;
				}
				else if ((value == 0) && (i == size))
				{
					mask->SetMaskValue(1);
					UpdateTreeItems();
					break;
				}

				i++;
				mask = mask->GetFirstSubnode();
				continue;
			}

			mask = mask->Next();
		}
	}
}

void NodeManagementPage::HandleAllWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		const CheckWidget *checkWidget = static_cast<CheckWidget *>(widget);
		GetEditor()->GetEditorObject()->SetAllSelectable(checkWidget->GetValue() != 0);
		UpdateTreeItems();
	}
}

void NodeManagementPage::HandleTreeWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	EventType eventType = eventData->eventType;

	if (eventType == kEventWidgetActivate)
	{
		HandleSelectButtonEvent(widget, eventData);
	}
	else if (eventType == kEventWidgetChange)
	{
		if (treeWidget->GetFirstSelectedListItem())
		{
			showButton->Enable();
			hideButton->Enable();
			selectButton->Enable();
		}
		else
		{
			showButton->Disable();
			hideButton->Disable();
			selectButton->Disable();
		}
	}
}

void NodeManagementPage::HandleShowButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if ((eventData->eventType == kEventWidgetActivate) && (treeWidget->GetFirstSelectedListItem()))
	{
		Type	packType[kMaxNodeTypeSize];

		Package package(packType, sizeof(Type) * kMaxNodeTypeSize);
		Packer packer(&package);

		Editor *editor = GetEditor();

		Node *root = editor->GetRootNode();
		Node *node = root->GetFirstSubnode();
		while (node)
		{
			if (!(node->GetNodeFlags() & kNodeNonpersistent))
			{
				if (Editor::GetManipulator(node)->Hidden())
				{
					node->PackType(packer);
					int32 depth = package.GetSize() / sizeof(Type) - 1;

					const NodeTreeItemWidget *itemWidget = static_cast<NodeTreeItemWidget *>(treeWidget->GetFirstSelectedListItem());
					while (itemWidget)
					{
						int32 d = itemWidget->Tree<TreeItemWidget>::GetNodeDepth();
						if (depth >= d)
						{
							const NodeTreeItemWidget *w = itemWidget;
							for (machine a = d; a >= 0; a--)
							{
								if (packType[a] != w->GetNodeType())
								{
									goto next;
								}

								w = static_cast<NodeTreeItemWidget *>(w->Tree<TreeItemWidget>::GetSuperNode());
							}

							editor->ShowNode(node);
							break;
						}

						next:
						itemWidget = static_cast<NodeTreeItemWidget *>(treeWidget->GetNextSelectedListItem(itemWidget));
					}

					package.Reset();
				}

				node = root->GetNextNode(node);
			}
			else
			{
				node = root->GetNextLevelNode(node);
			}
		}
	}
}

void NodeManagementPage::HandleHideButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if ((eventData->eventType == kEventWidgetActivate) && (treeWidget->GetFirstSelectedListItem()))
	{
		Type	packType[kMaxNodeTypeSize];

		Package package(packType, sizeof(Type) * kMaxNodeTypeSize);
		Packer packer(&package);

		Editor *editor = GetEditor();

		Node *root = editor->GetRootNode();
		Node *node = root->GetFirstSubnode();
		while (node)
		{
			if (!(node->GetNodeFlags() & kNodeNonpersistent))
			{
				if (!Editor::GetManipulator(node)->Hidden())
				{
					node->PackType(packer);
					int32 depth = package.GetSize() / sizeof(Type) - 1;

					const NodeTreeItemWidget *itemWidget = static_cast<NodeTreeItemWidget *>(treeWidget->GetFirstSelectedListItem());
					while (itemWidget)
					{
						int32 d = itemWidget->Tree<TreeItemWidget>::GetNodeDepth();
						if (depth >= d)
						{
							const NodeTreeItemWidget *w = itemWidget;
							for (machine a = d; a >= 0; a--)
							{
								if (packType[a] != w->GetNodeType())
								{
									goto next;
								}

								w = static_cast<NodeTreeItemWidget *>(w->Tree<TreeItemWidget>::GetSuperNode());
							}

							editor->HideNode(node);
							break;
						}

						next:
						itemWidget = static_cast<NodeTreeItemWidget *>(treeWidget->GetNextSelectedListItem(itemWidget));
					}

					package.Reset();
				}

				node = root->GetNextNode(node);
			}
			else
			{
				node = root->GetNextLevelNode(node);
			}
		}
	}
}

void NodeManagementPage::HandleSelectButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if ((eventData->eventType == kEventWidgetActivate) && (treeWidget->GetFirstSelectedListItem()))
	{
		Type	packType[kMaxNodeTypeSize];

		Package package(packType, sizeof(Type) * kMaxNodeTypeSize);
		Packer packer(&package);

		Editor *editor = GetEditor();
		editor->UnselectAll();

		Node *root = editor->GetRootNode();
		Node *node = root->GetFirstSubnode();
		while (node)
		{
			if (!(node->GetNodeFlags() & kNodeNonpersistent))
			{
				if (!node->GetManipulator()->Selected())
				{
					node->PackType(packer);
					int32 depth = package.GetSize() / sizeof(Type) - 1;

					const NodeTreeItemWidget *itemWidget = static_cast<NodeTreeItemWidget *>(treeWidget->GetFirstSelectedListItem());
					while (itemWidget)
					{
						int32 d = itemWidget->Tree<TreeItemWidget>::GetNodeDepth();
						if (depth >= d)
						{
							const NodeTreeItemWidget *w = itemWidget;
							for (machine a = d; a >= 0; a--)
							{
								if (packType[a] != w->GetNodeType())
								{
									goto next;
								}

								w = static_cast<NodeTreeItemWidget *>(w->Tree<TreeItemWidget>::GetSuperNode());
							}

							editor->SelectNode(node);
							break;
						}

						next:
						itemWidget = static_cast<NodeTreeItemWidget *>(treeWidget->GetNextSelectedListItem(itemWidget));
					}

					package.Reset();
				}

				node = root->GetNextNode(node);
			}
			else
			{
				node = root->GetNextLevelNode(node);
			}
		}
	}
}


GridPage::GridPage() :
		EditorPage(kEditorPageGrid, "WorldEditor/page/Grid", kEditorBookEditor),
		gridButtonObserver(this, &GridPage::HandleGridButtonEvent),
		angleMenuObserver(this, &GridPage::HangleAngleMenuEvent),
		gridTextObserver(this, &GridPage::HandleGridTextEvent),
		gridColorObserver(this, &GridPage::HandleGridColorEvent)
{
}

GridPage::~GridPage()
{
}

void GridPage::Preprocess(void)
{
	static const char *const buttonIdentifier[kEditorGridButtonCount] =
	{
		"Grid", "Snap", "Axes", "Halve", "Double"
	};

	EditorPage::Preprocess();

	for (machine a = 0; a < kEditorGridButtonCount; a++)
	{
		gridButton[a] = static_cast<IconButtonWidget *>(FindWidget(buttonIdentifier[a]));
		gridButton[a]->SetObserver(&gridButtonObserver);
	}

	snapAngleWidget = static_cast<PopupMenuWidget *>(FindWidget("Angle"));
	gridSpacingWidget = static_cast<EditTextWidget *>(FindWidget("Spacing"));
	majorLineWidget = static_cast<EditTextWidget *>(FindWidget("Major"));
	gridColorWidget = static_cast<ColorWidget *>(FindWidget("Color"));

	snapAngleWidget->SetObserver(&angleMenuObserver);
	gridSpacingWidget->SetObserver(&gridTextObserver);
	majorLineWidget->SetObserver(&gridTextObserver);
	gridColorWidget->SetObserver(&gridColorObserver);

	const EditorObject *object = GetEditor()->GetEditorObject();
	unsigned_int32 flags = object->GetEditorFlags();

	if (flags & kEditorShowGridlines)
	{
		gridButton[kEditorGridButtonShow]->SetValue(1);
	}

	if (flags & kEditorSnapToGrid)
	{
		gridButton[kEditorGridButtonSnap]->SetValue(1);
	}

	if (flags & kEditorShowViewportInfo)
	{
		gridButton[kEditorGridButtonAxes]->SetValue(1);
	}

	int32 angle = Min((int32) (object->GetSnapAngle() * (24.0F * K::one_over_tau) + 0.5F), 5);
	snapAngleWidget->SetSelection(angle - 1);

	gridSpacingWidget->SetText(Text::FloatToString(object->GetGridLineSpacing()));
	majorLineWidget->SetText(Text::IntegerToString(object->GetMajorLineInterval()));
	gridColorWidget->SetValue(object->GetGridColor());
}

void GridPage::HandleGridButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	Editor *editor = GetEditor();
	EditorObject *editorObject = editor->GetEditorObject();

	EventType eventType = eventData->eventType;

	if (eventType == kEventWidgetActivate)
	{
		if (widget == gridButton[kEditorGridButtonHalve])
		{
			float spacing = Fmax(editorObject->GetGridLineSpacing() * 0.5F, kMinEditorGridSpacing);
			editorObject->SetGridLineSpacing(spacing);
			gridSpacingWidget->SetText(Text::FloatToString(spacing));

			editor->InvalidateAllViewports();
			editor->InvalidateAllViewports();
		}
		else if (widget == gridButton[kEditorGridButtonDouble])
		{
			float spacing = editorObject->GetGridLineSpacing() * 2.0F;
			editorObject->SetGridLineSpacing(spacing);
			gridSpacingWidget->SetText(Text::FloatToString(spacing));

			editor->InvalidateAllViewports();
			editor->InvalidateAllViewports();
		}
	}
	else if (eventType == kEventWidgetChange)
	{
		if (widget == gridButton[kEditorGridButtonShow])
		{
			unsigned_int32 flags = editorObject->GetEditorFlags() ^ kEditorShowGridlines;
			editorObject->SetEditorFlags(flags);
			gridButton[kEditorGridButtonShow]->SetValue((flags & kEditorShowGridlines) != 0);
			editor->InvalidateAllViewports();
		}
		else if (widget == gridButton[kEditorGridButtonSnap])
		{
			unsigned_int32 flags = editorObject->GetEditorFlags() ^ kEditorSnapToGrid;
			editorObject->SetEditorFlags(flags);
			gridButton[kEditorGridButtonSnap]->SetValue((flags & kEditorSnapToGrid) != 0);
		}
		else if (widget == gridButton[kEditorGridButtonAxes])
		{
			unsigned_int32 flags = editorObject->GetEditorFlags() ^ kEditorShowViewportInfo;
			editorObject->SetEditorFlags(flags);
			gridButton[kEditorGridButtonAxes]->SetValue((flags & kEditorShowViewportInfo) != 0);

			if (flags & kEditorShowViewportInfo)
			{
				for (machine a = 0; a < kEditorViewportCount; a++)
				{
					editor->GetViewport(a)->ShowViewportInfo();
				}
			}
			else
			{
				for (machine a = 0; a < kEditorViewportCount; a++)
				{
					editor->GetViewport(a)->HideViewportInfo();
				}
			}

			editor->InvalidateAllViewports();
		}
	}
}

void GridPage::HangleAngleMenuEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		EditorObject *editorObject = GetEditor()->GetEditorObject();

		int32 selection = snapAngleWidget->GetSelection() + 1;
		if (selection == 5)
		{
			selection = 6;
		}

		editorObject->SetSnapAngle((float) selection * K::tau_over_24);
	}
}

void GridPage::HandleGridTextEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		Editor *editor = GetEditor();
		EditorObject *editorObject = editor->GetEditorObject();

		if (widget == gridSpacingWidget)
		{
			editorObject->SetGridLineSpacing(Fmax(Text::StringToFloat(gridSpacingWidget->GetText()), kMinEditorGridSpacing));
			editor->InvalidateAllViewports();
			editor->InvalidateAllViewports();
		}
		else if (widget == majorLineWidget)
		{
			editorObject->SetMajorLineInterval(Max(Text::StringToInteger(majorLineWidget->GetText()), 2));
			editor->InvalidateAllViewports();
			editor->InvalidateAllViewports();
		}
	}
}

void GridPage::HandleGridColorEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		Editor *editor = GetEditor();
		EditorObject *editorObject = editor->GetEditorObject();

		editorObject->SetGridColor(gridColorWidget->GetValue().GetColorRGB());
		editor->InvalidateAllViewports();
		editor->InvalidateAllViewports();
	}
}


ViewportsPage::ViewportsPage() :
		EditorPage(kEditorPageViewports, "WorldEditor/page/Viewports", kEditorBookEditor),
		layoutButtonObserver(this, &ViewportsPage::HandleLayoutButtonEvent),
		cameraSpeedSliderObserver(this, &ViewportsPage::HandleCameraSpeedSliderEvent)
{
}

ViewportsPage::~ViewportsPage()
{
}

void ViewportsPage::Preprocess(void)
{
	static const char *const buttonIdentifier[kEditorLayoutCount] =
	{
		"1", "4", "2H", "2V", "3L", "3R", "3T", "3B"
	};

	EditorPage::Preprocess();

	for (machine a = 0; a < kEditorLayoutCount; a++)
	{
		layoutButton[a] = static_cast<IconButtonWidget *>(FindWidget(buttonIdentifier[a]));
		layoutButton[a]->SetObserver(&layoutButtonObserver);
	}

	cameraSpeedSlider = static_cast<SliderWidget *>(FindWidget("Speed"));
	cameraSpeedSlider->SetObserver(&cameraSpeedSliderObserver);

	cameraSpeedText = static_cast<TextWidget *>(FindWidget("SpeedText"));

	const EditorObject *object = GetEditor()->GetEditorObject();
	int32 speed = (int32) (object->GetCameraSpeed() * 64.0F + 0.5F);
	cameraSpeedSlider->SetValue(speed - 1);
	cameraSpeedText->SetText(Text::IntegerToString(cameraSpeedSlider->GetValue() + 1));

	SetViewportLayout(object->GetCurrentViewportLayout());
}

void ViewportsPage::SetViewportLayout(int32 layout)
{
	for (machine a = 0; a < kEditorLayoutCount; a++)
	{
		layoutButton[a]->SetValue(a == layout);
	}
}

void ViewportsPage::HandleLayoutButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		for (machine a = 0; a < kEditorLayoutCount; a++)
		{
			if (widget == layoutButton[a])
			{
				SetViewportLayout(a);

				Editor *editor = GetEditor();
				EditorObject *object = editor->GetEditorObject();

				int32 layout = object->GetCurrentViewportLayout();
				if (a != layout)
				{
					object->SetPreviousViewportLayout(layout);
					object->SetCurrentViewportLayout(a);
					editor->UpdateViewportStructures();
				}

				break;
			}
		}
	}
}

void ViewportsPage::HandleCameraSpeedSliderEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		int32 value = cameraSpeedSlider->GetValue() + 1;
		cameraSpeedText->SetText(Text::IntegerToString(value));
		GetEditor()->GetEditorObject()->SetCameraSpeed((float) value * 0.015625F);
	}
}


TransformPage::TransformPage() :
		EditorPage(kEditorPageTransform, "WorldEditor/page/Transform", kEditorBookEditor),
		editorObserver(this, &TransformPage::HandleEditorEvent),
		positionTextObserver(this, &TransformPage::HandlePositionTextEvent),
		rotationTextObserver(this, &TransformPage::HandleRotationTextEvent),
		sizeTextObserver(this, &TransformPage::HandleSizeTextEvent),
		buttonObserver(this, &TransformPage::HandleButtonEvent)
{
}

TransformPage::~TransformPage()
{
}

void TransformPage::Preprocess(void)
{
	static const char *const positionIdentifier[3] =
	{
		"Xpos", "Ypos", "Zpos"
	};

	static const char *const rotationIdentifier[3] =
	{
		"Xrot", "Yrot", "Zrot"
	};

	static const char *const sizeIdentifier[kMaxObjectSizeCount] =
	{
		"Xsize", "Ysize", "Zsize", "Asize", "Bsize", "Csize"
	};


	EditorPage::Preprocess();
	GetEditor()->AddObserver(&editorObserver);

	for (machine a = 0; a < 3; a++)
	{
		positionTextWidget[a] = static_cast<EditTextWidget *>(FindWidget(positionIdentifier[a]));
		positionTextWidget[a]->SetObserver(&positionTextObserver);

		rotationTextWidget[a] = static_cast<EditTextWidget *>(FindWidget(rotationIdentifier[a]));
		rotationTextWidget[a]->SetObserver(&rotationTextObserver);
	}

	for (machine a = 0; a < kMaxObjectSizeCount; a++)
	{
		sizeTextWidget[a] = static_cast<EditTextWidget *>(FindWidget(sizeIdentifier[a]));
		sizeTextWidget[a]->SetObserver(&sizeTextObserver);
	}

	copyButton = static_cast<PushButtonWidget *>(FindWidget("Copy"));
	pasteButton = static_cast<PushButtonWidget *>(FindWidget("Paste"));
	resetButton = static_cast<PushButtonWidget *>(FindWidget("Reset"));

	copyButton->SetObserver(&buttonObserver);
	pasteButton->SetObserver(&buttonObserver);
	resetButton->SetObserver(&buttonObserver);

	positionButton = static_cast<RadioWidget *>(FindWidget("Position"));
	rotationButton = static_cast<RadioWidget *>(FindWidget("Rotation"));
	bothButton = static_cast<RadioWidget *>(FindWidget("Both"));
}

void TransformPage::HandleEditorEvent(Editor *editor, const EditorEvent& event)
{
	EditorEventType type = event.GetEventType();

	if (type == kEditorEventGizmoMoved)
	{
		operationType = kOperationNone;
		updateFlag = true;

		const Node *node = static_cast<const NodeEditorEvent&>(event).GetEventNode();
		if (node)
		{
			UpdateTransform(node);
			copyButton->Enable();
		}
		else
		{
			for (machine a = 0; a < 3; a++)
			{
				positionTextWidget[a]->SetText(nullptr);
				positionTextWidget[a]->Disable();

				rotationTextWidget[a]->SetText(nullptr);
				rotationTextWidget[a]->Disable();
			}

			for (machine a = 0; a < kMaxObjectSizeCount; a++)
			{
				sizeTextWidget[a]->SetText(nullptr);
				sizeTextWidget[a]->Disable();
			}

			copyButton->Disable();
			pasteButton->Disable();
			resetButton->Disable();
		}
	}
	else if (type == kEditorEventGizmoTargetInvalidated)
	{
		if (updateFlag)
		{
			const Node *node = static_cast<const NodeEditorEvent&>(event).GetEventNode();
			UpdateTransform(node);
			operationType = kOperationNone;
		}
	}
}

void TransformPage::HandlePositionTextEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		Editor *editor = GetEditor();

		const NodeReference *reference = editor->GetGizmoTarget();
		if (reference)
		{
			Node *node = reference->GetNode();

			if (operationType != kOperationMove)
			{
				operationType = kOperationMove;
				editor->AddOperation(new MoveOperation(node));
			}

			const EditTextWidget *editText = static_cast<EditTextWidget *>(widget);
			float f = Text::StringToFloat(editText->GetText());

			Point3D position = node->GetNodePosition();
			for (machine a = 0; a < 3; a++)
			{
				if (editText == positionTextWidget[a])
				{
					position[a] = f;
					break;
				}
			}

			node->SetNodePosition(position);

			updateFlag = false;
			editor->InvalidateNode(node);

			if (node->GetNodeType() == kNodeGeometry)
			{
				node->Update();
				editor->RegenerateTexcoords(static_cast<Geometry *>(node));
			}

			updateFlag = true;
		}
	}
}

void TransformPage::HandleRotationTextEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		Editor *editor = GetEditor();

		const NodeReference *reference = editor->GetGizmoTarget();
		if (reference)
		{
			Node *node = reference->GetNode();

			if (operationType != kOperationMove)
			{
				operationType = kOperationMove;
				editor->AddOperation(new MoveOperation(node));
			}

			float x = Text::StringToFloat(rotationTextWidget[0]->GetText()) * K::radians;
			float y = Text::StringToFloat(rotationTextWidget[1]->GetText()) * K::radians;
			float z = Text::StringToFloat(rotationTextWidget[2]->GetText()) * K::radians;
			node->SetNodeMatrix3D(Matrix3D().SetEulerAngles(x, y, z));

			updateFlag = false;
			editor->InvalidateNode(node);

			if (node->GetNodeType() == kNodeGeometry)
			{
				node->Update();
				editor->RegenerateTexcoords(static_cast<Geometry *>(node));
			}

			updateFlag = true;
		}
	}
}

void TransformPage::HandleSizeTextEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		Editor *editor = GetEditor();

		const NodeReference *reference = editor->GetGizmoTarget();
		if (reference)
		{
			Node *node = reference->GetNode();
			Object *object = node->GetObject();
			if (object)
			{
				float	objectSize[kMaxObjectSizeCount];

				if (operationType != kOperationSize)
				{
					operationType = kOperationSize;
					editor->AddOperation(new SizeOperation(node));
				}

				int32 count = object->GetObjectSize(objectSize);
				for (machine a = 0; a < count; a++)
				{
					const char *string = sizeTextWidget[a]->GetText();
					for (machine b = 0;; b++)
					{
						unsigned_int32 c = string[b];
						if (c == 0)
						{
							break;
						}

						if (c - '0' < 10U)
						{
							objectSize[a] = Text::StringToFloat(string);
							break;
						}
					}
				}

				updateFlag = false;
				Editor::GetManipulator(node)->HandleSizeUpdate(count, objectSize);
				updateFlag = true;
			}
		}
	}
}

void TransformPage::HandleButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		Editor *editor = GetEditor();

		const NodeReference *reference = editor->GetGizmoTarget();
		if (reference)
		{
			if (widget == copyButton)
			{
				const Transform4D& transform = reference->GetNode()->GetNodeTransform();
				Transform4D& clipboard = editor->GetTransformClipboard();

				if (positionButton->GetValue() == 0)
				{
					clipboard[0] = transform[0];
					clipboard[1] = transform[1];
					clipboard[2] = transform[2];
				}

				if (rotationButton->GetValue() == 0)
				{
					clipboard.SetTranslation(transform.GetTranslation());
				}
			}
			else
			{
				editor->SetFocusWidget(nullptr);

				Node *node = reference->GetNode();
				editor->AddOperation(new MoveOperation(node));

				if (widget == pasteButton)
				{
					const Transform4D& clipboard = editor->GetTransformClipboard();

					if (positionButton->GetValue() == 0)
					{
						node->SetNodeMatrix3D(clipboard[0], clipboard[1], clipboard[2]);
					}

					if (rotationButton->GetValue() == 0)
					{
						node->SetNodePosition(clipboard.GetTranslation());
					}
				}
				else
				{
					if (positionButton->GetValue() == 0)
					{
						node->SetNodeMatrix3D(Identity3D[0], Identity3D[1], Identity3D[2]);
					}

					if (rotationButton->GetValue() == 0)
					{
						node->SetNodePosition(Zero3D);
					}
				}

				node->Invalidate();

				if (node->GetNodeType() == kNodeGeometry)
				{
					node->Update();
					editor->RebuildGeometry(static_cast<Geometry *>(node));
				}
			}
		}
	}
}

void TransformPage::UpdateTransform(const Node *node)
{
	float	x, y, z;

	const Point3D& position = node->GetNodePosition();
	positionTextWidget[0]->SetText(Text::FloatToString(position.x));
	positionTextWidget[1]->SetText(Text::FloatToString(position.y));
	positionTextWidget[2]->SetText(Text::FloatToString(position.z));

	node->GetNodeTransform().GetEulerAngles(&x, &y, &z);
	rotationTextWidget[0]->SetText(Text::FloatToString(x * K::degrees));
	rotationTextWidget[1]->SetText(Text::FloatToString(y * K::degrees));
	rotationTextWidget[2]->SetText(Text::FloatToString(z * K::degrees));

	if (!(Editor::GetManipulator(node)->GetManipulatorFlags() & kManipulatorLockedTransform))
	{
		for (machine a = 0; a < 3; a++)
		{
			positionTextWidget[a]->Enable();
			positionTextWidget[a]->SetWidgetAlpha(1.0F);
			rotationTextWidget[a]->Enable();
			rotationTextWidget[a]->SetWidgetAlpha(1.0F);
		}

		pasteButton->Enable();
		resetButton->Enable();
	}
	else
	{
		for (machine a = 0; a < 3; a++)
		{
			positionTextWidget[a]->Disable();
			positionTextWidget[a]->SetWidgetAlpha(0.5F);
			rotationTextWidget[a]->Disable();
			rotationTextWidget[a]->SetWidgetAlpha(0.5F);
		}

		pasteButton->Disable();
		resetButton->Disable();
	}

	int32 count = 0;

	const Object *object = node->GetObject();
	if (object)
	{
		float	objectSize[kMaxObjectSizeCount];

		count = object->GetObjectSize(objectSize);
		for (machine a = 0; a < count; a++)
		{
			sizeTextWidget[a]->Enable();
			sizeTextWidget[a]->SetText(Text::FloatToString(objectSize[a]));
		}
	}

	for (machine a = count; a < kMaxObjectSizeCount; a++)
	{
		sizeTextWidget[a]->SetText(nullptr);
		sizeTextWidget[a]->Disable();
	}
}


InfoPage::InfoPage() :
		EditorPage(kEditorPageInfo, "WorldEditor/page/Info", kEditorBookEditor),
		editorObserver(this, &InfoPage::HandleEditorEvent)
{
}

InfoPage::~InfoPage()
{
}

void InfoPage::Preprocess(void)
{
	static const char *const nodeInfoIdentifier[kNodeInfoCount] =
	{
		"Name", "Controller", "Connectors", "Properties", "Instances"
	};

	static const char *const geometryInfoIdentifier[kGeometryInfoCount] =
	{
		"Vertices", "Primitives", "Surfaces", "Materials", "Levels"
	};

	EditorPage::Preprocess();
	GetEditor()->AddObserver(&editorObserver);

	nodeIconWidget = static_cast<ImageWidget *>(FindWidget("Icon"));

	geometryGroup = FindWidget("GeometryGroup");
	worldGroup = FindWidget("WorldGroup");
	typeGroup = FindWidget("TypeGroup");

	for (machine a = 0; a < kNodeInfoCount; a++)
	{
		nodeWidget[a] = static_cast<TextWidget *>(FindWidget(nodeInfoIdentifier[a]));
	}

	for (machine a = 0; a < kGeometryInfoCount; a++)
	{
		geometryWidget[a] = static_cast<TextWidget *>(FindWidget(geometryInfoIdentifier[a]));
	}

	worldWidget = static_cast<TextWidget *>(FindWidget("World"));
	typeWidget = static_cast<TextWidget *>(FindWidget("Type"));
}

void InfoPage::HandleEditorEvent(Editor *editor, const EditorEvent& event)
{
	EditorEventType type = event.GetEventType();

	if (type == kEditorEventGizmoMoved)
	{
		const Node *node = static_cast<const NodeEditorEvent *>(&event)->GetEventNode();
		if (node)
		{
			UpdateInfo(node);
		}
		else
		{
			ClearInfo();
		}
	}
	else if (type == kEditorEventGizmoTargetModified)
	{
		UpdateInfo(static_cast<const NodeEditorEvent *>(&event)->GetEventNode());
	}
}

void InfoPage::ClearInfo(void)
{
	nodeIconWidget->Hide();

	for (machine a = 0; a < kNodeInfoCount; a++)
	{
		nodeWidget[a]->SetText(nullptr);
	}

	geometryGroup->Hide();
	worldGroup->Hide();
	typeGroup->Hide();
}

void InfoPage::UpdateInfo(const Node *node)
{
	const EditorManipulator *manipulator = Editor::GetManipulator(node);

	nodeIconWidget->SetTexture(0, manipulator->GetIconName());
	nodeIconWidget->Show();

	const char *name = node->GetNodeName();
	if (name)
	{
		nodeWidget[kNodeInfoName]->SetText(name);
	}
	else
	{
		nodeWidget[kNodeInfoName]->SetText(manipulator->GetDefaultNodeName());
	}

	const StringTable *table = TheWorldEditor->GetStringTable();

	const Controller *controller = node->GetController();
	if (controller)
	{
		ControllerType controllerType = controller->GetControllerType();
		if (controllerType == kControllerGeneric)
		{
			nodeWidget[kNodeInfoController]->SetText(table->GetString(StringID('PAGE', kEditorPageInfo, 'GNRC')));
		}
		else
		{
			const ControllerRegistration *registration = Controller::FindRegistration(controllerType);
			if ((registration) && (registration->GetControllerName()))
			{
				nodeWidget[kNodeInfoController]->SetText(registration->GetControllerName());
			}
			else
			{
				nodeWidget[kNodeInfoController]->SetText(Text::TypeToHexCharString(controllerType));
			}
		}
	}
	else
	{
		nodeWidget[kNodeInfoController]->SetText(table->GetString(StringID('PAGE', kEditorPageInfo, 'NONE')));
	}

	const Hub *hub = node->GetHub();
	if (hub)
	{
		nodeWidget[kNodeInfoConnectors]->SetText(String<7>(hub->GetOutgoingEdgeCount()));
	}
	else
	{
		nodeWidget[kNodeInfoConnectors]->SetText("0");
	}

	nodeWidget[kNodeInfoProperties]->SetText(String<7>(node->GetPropertyCount()));

	const Object *object = node->GetObject();
	if (object)
	{
		// We can't just use the object's reference count here because there might be
		// nodes in the undo buffer that still refer to the object.

		int32 count = 0;
		Node *root = GetEditor()->GetRootNode();
		Node *subnode = root;
		do
		{
			count += (subnode->GetObject() == object);
			subnode = root->GetNextNode(subnode);
		} while (subnode);

		nodeWidget[kNodeInfoInstances]->SetText(String<7>(count));
	}
	else
	{
		nodeWidget[kNodeInfoInstances]->SetText("0");
	}

	NodeType nodeType = node->GetNodeType();
	if (nodeType == kNodeGeometry)
	{
		const Geometry *geometry = static_cast<const Geometry *>(node);
		const GeometryObject *geometryObject = geometry->GetObject();
		const Mesh *mesh = geometryObject->GetGeometryLevel(0);

		geometryWidget[kGeometryInfoVertices]->SetText(String<7>(mesh->GetVertexCount()));
		geometryWidget[kGeometryInfoPrimitives]->SetText(String<7>(mesh->GetPrimitiveCount()));
		geometryWidget[kGeometryInfoSurfaces]->SetText(String<7>(geometryObject->GetSurfaceCount()));
		geometryWidget[kGeometryInfoMaterials]->SetText(String<7>(geometry->GetMaterialCount()));
		geometryWidget[kGeometryInfoLevels]->SetText(String<7>(geometryObject->GetGeometryLevelCount()));

		geometryGroup->Show();
		worldGroup->Hide();
		typeGroup->Hide();
	}
	else if (nodeType == kNodeMarker)
	{
		const Marker *marker = static_cast<const Marker *>(node);
		MarkerType markerType = marker->GetMarkerType();
		if (markerType == kMarkerLocator)
		{
			const LocatorMarker *locator = static_cast<const LocatorMarker *>(marker);
			LocatorType locatorType = locator->GetLocatorType();
			if (locatorType != 0)
			{
				typeWidget->SetText(Text::TypeToString(locatorType));
			}
			else
			{
				typeWidget->SetText(table->GetString(StringID('PAGE', kEditorPageInfo, 'NONE')));
			}

			typeGroup->Show();
			worldGroup->Hide();
		}

		geometryGroup->Hide();
	}
	else if (nodeType == kNodeInstance)
	{
		const Instance *instance = static_cast<const Instance *>(node);
		worldWidget->SetText(instance->GetWorldName());

		worldGroup->Show();
		typeGroup->Hide();
		geometryGroup->Hide();
	}
	else if (nodeType == kNodeModel)
	{
		ModelType modelType = static_cast<const Model *>(node)->GetModelType();
		const ModelRegistration *registration = Model::FindRegistration(modelType);
		if ((registration) && (registration->GetModelName()))
		{
			typeWidget->SetText(registration->GetModelName());
		}
		else
		{
			typeWidget->SetText(Text::TypeToHexCharString(modelType));
		}

		typeGroup->Show();
		worldGroup->Hide();
		geometryGroup->Hide();
	}
}


FindPage::FindPage() :
		EditorPage(kEditorPageFind, "WorldEditor/page/Find", kEditorBookEditor),
		findAllButtonObserver(this, &FindPage::HandleFindAllButtonEvent),
		findNextButtonObserver(this, &FindPage::HandleFindNextButtonEvent),
		findPreviousButtonObserver(this, &FindPage::HandleFindPreviousButtonEvent)
{
}

FindPage::~FindPage()
{
}

void FindPage::Preprocess(void)
{
	static const NodeType nodeTypeTable[] =
	{
		kNodeBlocker, kNodeBone, kNodeCamera, kNodeEffect, kNodeEmitter, kNodeField, kNodeGeometry, kNodeGeneric, kNodeImpostor, kNodeInstance, kNodeJoint, kNodeLight,
		kNodeMarker, kNodeModel, kNodePhysics, kNodePortal, kNodeShape, kNodeSkybox, kNodeSource, kNodeSpace, kNodeTerrainBlock, kNodeTrigger, kNodeWaterBlock, kNodeZone
	};

	EditorPage::Preprocess();

	nameWidget = static_cast<EditTextWidget *>(FindWidget("Name"));
	typeBoxWidget = static_cast<CheckWidget *>(FindWidget("TypeBox"));
	typeListWidget = static_cast<ListWidget *>(FindWidget("TypeList"));
	controllerBoxWidget = static_cast<CheckWidget *>(FindWidget("ControllerBox"));
	controllerListWidget = static_cast<ListWidget *>(FindWidget("ControllerList"));

	typeListWidget->SetWidgetUsage(typeListWidget->GetWidgetUsage() & ~kWidgetKeyboardFocus);
	controllerListWidget->SetWidgetUsage(controllerListWidget->GetWidgetUsage() & ~kWidgetKeyboardFocus);

	PushButtonWidget *findAllWidget = static_cast<PushButtonWidget *>(FindWidget("All"));
	PushButtonWidget *findNextWidget = static_cast<PushButtonWidget *>(FindWidget("Next"));
	PushButtonWidget *findPreviousWidget = static_cast<PushButtonWidget *>(FindWidget("Previous"));

	findAllWidget->SetObserver(&findAllButtonObserver);
	findNextWidget->SetObserver(&findNextButtonObserver);
	findPreviousWidget->SetObserver(&findPreviousButtonObserver);

	Vector2D size = typeListWidget->GetNaturalListItemSize();

	const StringTable *table = TheWorldEditor->GetStringTable();
	for (NodeType type : nodeTypeTable)
	{
		typeListWidget->AppendListItem(new TypeWidget(size, table->GetString(StringID('NAME', type)), type));
	}

	size = controllerListWidget->GetNaturalListItemSize();

	const ControllerRegistration *registration = Controller::GetFirstRegistration();
	while (registration)
	{
		const char *name = registration->GetControllerName();
		if (name)
		{
			controllerListWidget->AppendListItem(new TypeWidget(size, name, registration->GetControllerType()));
		}

		registration = registration->Next();
	}
}

bool FindPage::MatchingNode(const Node *node) const
{
	if (node->GetNodeFlags() & kNodeNonpersistent)
	{
		return (false);
	}

	const char *findName = nameWidget->GetText();
	if (findName[0] != 0)
	{
		const char *nodeName = node->GetNodeName();
		if ((!nodeName) || (!Text::CompareTextCaseless(nodeName, findName)))
		{
			return (false);
		}
	}

	if (typeBoxWidget->GetValue() != 0)
	{
		const TypeWidget *widget = static_cast<TypeWidget *>(typeListWidget->GetFirstSelectedListItem());
		if (widget)
		{
			bool match = false;
			NodeType nodeType = node->GetNodeType();

			do
			{
				if (widget->GetItemType() == nodeType)
				{
					match = true;
					break;
				}

				widget = static_cast<TypeWidget *>(typeListWidget->GetNextSelectedListItem(widget));
			} while (widget);

			if (!match)
			{
				return (false);
			}
		}
	}

	if (controllerBoxWidget->GetValue() != 0)
	{
		const TypeWidget *widget = static_cast<TypeWidget *>(controllerListWidget->GetFirstSelectedListItem());
		if (widget)
		{
			const Controller *controller = node->GetController();
			if (!controller)
			{
				return (false);
			}

			bool match = false;
			ControllerType controllerType = controller->GetControllerType();

			do
			{
				if (widget->GetItemType() == controllerType)
				{
					match = true;
					break;
				}

				widget = static_cast<TypeWidget *>(controllerListWidget->GetNextSelectedListItem(widget));
			} while (widget);

			if (!match)
			{
				return (false);
			}
		}
	}

	return (true);
}

void FindPage::HandleFindAllButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		Editor *editor = GetEditor();
		editor->UnselectAll();

		Node *root = editor->GetRootNode();
		Node *node = root;
		while (node)
		{
			if (MatchingNode(node))
			{
				editor->SelectNode(node);
			}

			node = root->GetNextNode(node);
		}

		editor->FrameSelectionAllViewports();
	}
}

void FindPage::HandleFindNextButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		Editor *editor = GetEditor();
		Node *root = editor->GetRootNode();

		const NodeReference *reference = editor->GetFirstSelectedNode();
		Node *node = (reference) ? reference->GetNode() : nullptr;

		editor->UnselectAll();

		if (node)
		{
			node = root->GetNextNode(node);
		}

		if (!node)
		{
			node = root;
		}

		const Node *stop = node;
		for (;;)
		{
			if (MatchingNode(node))
			{
				editor->SelectNode(node);
				break;
			}

			if (node)
			{
				node = root->GetNextNode(node);
			}

			if (!node)
			{
				node = root;
			}

			if (node == stop)
			{
				return;
			}
		}

		editor->FrameSelectionAllViewports();
	}
}

void FindPage::HandleFindPreviousButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		Editor *editor = GetEditor();
		Node *root = editor->GetRootNode();

		const NodeReference *reference = editor->GetFirstSelectedNode();
		Node *node = (reference) ? reference->GetNode() : nullptr;

		editor->UnselectAll();

		if (node)
		{
			node = root->GetPreviousNode(node);
		}

		if (!node)
		{
			node = root->GetRightmostNode();
		}

		const Node *stop = node;
		for (;;)
		{
			if (MatchingNode(node))
			{
				editor->SelectNode(node);
				break;
			}

			if (node)
			{
				node = root->GetPreviousNode(node);
			}

			if (!node)
			{
				node = root->GetRightmostNode();
			}

			if (node == stop)
			{
				return;
			}
		}

		editor->FrameSelectionAllViewports();
	}
}

// ZYUQURM
