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


#include "C4SpaceManipulators.h"
#include "C4EditorSupport.h"
#include "C4WorldEditor.h"


using namespace C4;


namespace
{
	const ConstColorRGBA kSpaceInteriorColor = {0.75F, 0.75F, 0.75F, 0.75F};
	const ConstColorRGBA kSpaceOutlineColor = {1.0F, 1.0F, 1.0F, 1.0F};
}


SpaceManipulator::SpaceManipulator(Space *space, VolumeManipulator *volume, const char *iconName) : EditorManipulator(space, iconName)
{
	volumeManipulator = volume;
}

SpaceManipulator::~SpaceManipulator()
{
}

Manipulator *SpaceManipulator::Create(Space *space)
{
	switch (space->GetSpaceType())
	{
		case kSpaceFog:

			return (new FogSpaceManipulator(static_cast<FogSpace *>(space)));

		case kSpaceShadow:

			return (new ShadowSpaceManipulator(static_cast<ShadowSpace *>(space)));

		case kSpaceRadiosity:

			return (new RadiositySpaceManipulator(static_cast<RadiositySpace *>(space)));

		case kSpaceAcoustics:

			return (new AcousticsSpaceManipulator(static_cast<AcousticsSpace *>(space)));

		case kSpaceOcclusion:

			return (new OcclusionSpaceManipulator(static_cast<OcclusionSpace *>(space)));

		case kSpacePaint:

			return (new PaintSpaceManipulator(static_cast<PaintSpace *>(space)));

		case kSpacePhysics:

			return (new PhysicsSpaceManipulator(static_cast<PhysicsSpace *>(space)));
	}

	return (nullptr);
}

const char *SpaceManipulator::GetDefaultNodeName(void) const
{
	return (TheWorldEditor->GetStringTable()->GetString(StringID('NAME', kNodeSpace, GetTargetNode()->GetSpaceType())));
}

void SpaceManipulator::Select(void)
{
	EditorManipulator::Select();
	volumeManipulator->Select();
}

void SpaceManipulator::Unselect(void)
{
	EditorManipulator::Unselect();
	volumeManipulator->Unselect();
}

bool SpaceManipulator::Pick(const Ray *ray, PickData *data) const
{
	return (volumeManipulator->Pick(ray, data));
}

bool SpaceManipulator::RegionPick(const VisibilityRegion *region) const
{
	return (volumeManipulator->RegionPick(GetTargetNode()->GetWorldTransform(), region));
}

void SpaceManipulator::Render(const ManipulatorRenderData *renderData)
{
	volumeManipulator->Render(renderData);
	EditorManipulator::Render(renderData);
}

void SpaceManipulator::UpdateZoneConnectors(void) const
{
	Zone *zone = GetTargetNode()->GetOwningZone();
	if (zone)
	{
		Editor::GetManipulator(zone)->UpdateConnectors();
	}
} 


FogSpaceManipulator::FogSpaceManipulator(FogSpace *fog) : 
		SpaceManipulator(fog, this, "WorldEditor/atmosphere/Fog"),
		PlateVolumeManipulator(fog, kSpaceInteriorColor, kSpaceOutlineColor, "WorldEditor/volume/Fog") 
{
}
 
FogSpaceManipulator::~FogSpaceManipulator()
{ 
} 

Box3D FogSpaceManipulator::CalculateNodeBoundingBox(void) const
{
	return (PlateVolumeManipulator::CalculateBoundingBox(GetObject()->GetPlateSize())); 
}

int32 FogSpaceManipulator::GetHandleTable(Point3D *handle) const
{
	return (PlateVolumeManipulator::GetHandleTable(GetObject()->GetPlateSize(), handle));
}

void FogSpaceManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	PlateVolumeManipulator::GetHandleData(index, handleData);
}

bool FogSpaceManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	FogSpaceObject *object = GetObject();

	Vector2D newSize = object->GetPlateSize();
	const Vector2D& oldSize = *reinterpret_cast<const Vector2D *>(GetOriginalSize());
	bool move = PlateVolumeManipulator::Resize(resizeData, oldSize, newSize);

	object->SetPlateSize(newSize);
	return (move);
}

void FogSpaceManipulator::Update(void)
{
	if (!(GetManipulatorState() & kManipulatorUpdated))
	{
		SetPlateSize(GetObject()->GetPlateSize());
	}

	SpaceManipulator::Update();
}


ShadowSpaceManipulator::ShadowSpaceManipulator(ShadowSpace *shadow) :
		SpaceManipulator(shadow, this, "WorldEditor/light/Shadow"),
		BoxVolumeManipulator(shadow, kSpaceInteriorColor, kSpaceOutlineColor, "WorldEditor/volume/Shadow")
{
}

ShadowSpaceManipulator::~ShadowSpaceManipulator()
{
}

Box3D ShadowSpaceManipulator::CalculateNodeBoundingBox(void) const
{
	return (BoxVolumeManipulator::CalculateBoundingBox(GetObject()->GetBoxSize()));
}

int32 ShadowSpaceManipulator::GetHandleTable(Point3D *handle) const
{
	return (BoxVolumeManipulator::GetHandleTable(GetObject()->GetBoxSize(), handle));
}

void ShadowSpaceManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	BoxVolumeManipulator::GetHandleData(index, handleData);
}

bool ShadowSpaceManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	ShadowSpaceObject *object = GetObject();

	Vector3D newSize = object->GetBoxSize();
	const Vector3D& oldSize = *reinterpret_cast<const Vector3D *>(GetOriginalSize());
	bool move = BoxVolumeManipulator::Resize(resizeData, oldSize, newSize);

	object->SetBoxSize(newSize);
	return (move);
}

void ShadowSpaceManipulator::Update(void)
{
	if (!(GetManipulatorState() & kManipulatorUpdated))
	{
		SetBoxSize(GetObject()->GetBoxSize());
	}

	SpaceManipulator::Update();
}


RadiositySpaceManipulator::RadiositySpaceManipulator(RadiositySpace *radiosity) :
		SpaceManipulator(radiosity, this, "WorldEditor/light/Radiosity"),
		BoxVolumeManipulator(radiosity, kSpaceInteriorColor, kSpaceOutlineColor, "WorldEditor/volume/Radiosity"),
		gridVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		gridIndexBuffer(kVertexBufferIndex | kVertexBufferDynamic),
		gridDiffuseColor(ColorRGBA(0.25F, 1.0F, 0.0F, 1.0F)),
		gridRenderable(kRenderIndexedLines, kRenderDepthTest | kRenderDepthInhibit)
{
	gridRenderable.SetTransformable(radiosity);

	gridRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &gridVertexBuffer, sizeof(Point3D));
	gridRenderable.SetVertexBuffer(kVertexBufferIndexArray, &gridIndexBuffer);
	gridRenderable.SetVertexAttributeArray(kArrayPosition, 0, 3);

	gridAttributeList.Append(&gridDiffuseColor);
	gridRenderable.SetMaterialAttributeList(&gridAttributeList);
}

RadiositySpaceManipulator::~RadiositySpaceManipulator()
{
}

void RadiositySpaceManipulator::Preprocess(void)
{
	SpaceManipulator::Preprocess();
	UpdateZoneConnectors();
}

Box3D RadiositySpaceManipulator::CalculateNodeBoundingBox(void) const
{
	return (BoxVolumeManipulator::CalculateBoundingBox(GetObject()->GetBoxSize()));
}

int32 RadiositySpaceManipulator::GetHandleTable(Point3D *handle) const
{
	return (BoxVolumeManipulator::GetHandleTable(GetObject()->GetBoxSize(), handle));
}

void RadiositySpaceManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	BoxVolumeManipulator::GetHandleData(index, handleData);
}

bool RadiositySpaceManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	RadiositySpaceObject *object = GetObject();

	Vector3D newSize = object->GetBoxSize();
	const Vector3D& oldSize = *reinterpret_cast<const Vector3D *>(GetOriginalSize());
	bool move = BoxVolumeManipulator::Resize(resizeData, oldSize, newSize);

	object->SetBoxSize(newSize);
	return (move);
}

void RadiositySpaceManipulator::Update(void)
{
	if (!(GetManipulatorState() & kManipulatorUpdated))
	{
		SetBoxSize(GetObject()->GetBoxSize());

		const RadiositySpaceObject *object = GetObject();
		const Vector3D& boxSize = object->GetBoxSize();

		const Integer3D& radiosityResolution = object->GetRadiosityResolution();
		int32 width = radiosityResolution.x - 1;
		int32 height = radiosityResolution.y - 1;
		int32 depth = radiosityResolution.z - 1;

		int32 count = ((width - 1) + (height - 1) + (depth - 1)) * 4;
		gridRenderable.SetVertexCount(count);
		gridRenderable.SetPrimitiveCount(count);
		gridRenderable.InvalidateVertexData();

		gridVertexBuffer.Establish(sizeof(Point3D) * count);
		gridIndexBuffer.Establish(sizeof(Line) * count);

		volatile Point3D *restrict vertex = gridVertexBuffer.BeginUpdate<Point3D>();
		volatile Line *restrict line = gridIndexBuffer.BeginUpdate<Line>();

		unsigned_int32 base = 0;

		float dx = boxSize.x / (float) width;
		for (machine i = 1; i < width; i++)
		{
			float x = (float) i * dx;

			vertex[0].Set(x, 0.0F, 0.0F);
			vertex[1].Set(x, boxSize.y, 0.0F);
			vertex[2].Set(x, boxSize.y, boxSize.z);
			vertex[3].Set(x, 0.0F, boxSize.z);

			line[0].Set(base, base + 1);
			line[1].Set(base + 1, base + 2);
			line[2].Set(base + 2, base + 3);
			line[3].Set(base + 3, base);

			vertex += 4;
			line += 4;
			base += 4;
		}

		float dy = boxSize.y / (float) height;
		for (machine j = 1; j < height; j++)
		{
			float y = (float) j * dy;

			vertex[0].Set(0.0F, y, 0.0F);
			vertex[1].Set(boxSize.x, y, 0.0F);
			vertex[2].Set(boxSize.x, y, boxSize.z);
			vertex[3].Set(0.0F, y, boxSize.z);

			line[0].Set(base, base + 1);
			line[1].Set(base + 1, base + 2);
			line[2].Set(base + 2, base + 3);
			line[3].Set(base + 3, base);

			vertex += 4;
			line += 4;
			base += 4;
		}

		float dz = boxSize.z / (float) depth;
		for (machine k = 1; k < depth; k++)
		{
			float z = (float) k * dz;

			vertex[0].Set(0.0F, 0.0F, z);
			vertex[1].Set(boxSize.x, 0.0F, z);
			vertex[2].Set(boxSize.x, boxSize.y, z);
			vertex[3].Set(0.0F, boxSize.y, z);

			line[0].Set(base, base + 1);
			line[1].Set(base + 1, base + 2);
			line[2].Set(base + 2, base + 3);
			line[3].Set(base + 3, base);

			vertex += 4;
			line += 4;
			base += 4;
		}

		gridIndexBuffer.EndUpdate();
		gridVertexBuffer.EndUpdate();
	}

	SpaceManipulator::Update();
}

void RadiositySpaceManipulator::HandleSettingsUpdate(void)
{
	EditorManipulator::HandleSettingsUpdate();
	GetEditor()->InvalidateAllShaderData();
}

void RadiositySpaceManipulator::Render(const ManipulatorRenderData *renderData)
{
	SpaceManipulator::Render(renderData);

	List<Renderable> *renderList = renderData->manipulatorList;
	if (renderList)
	{
		renderList->Append(&gridRenderable);
	}
}


AcousticsSpaceManipulator::AcousticsSpaceManipulator(AcousticsSpace *acoustics) :
		SpaceManipulator(acoustics, this, "WorldEditor/source/Acoustics"),
		BoxVolumeManipulator(acoustics, kSpaceInteriorColor, kSpaceOutlineColor, "WorldEditor/volume/Acoustics")
{
}

AcousticsSpaceManipulator::~AcousticsSpaceManipulator()
{
}

void AcousticsSpaceManipulator::Preprocess(void)
{
	SpaceManipulator::Preprocess();
	UpdateZoneConnectors();
}

Box3D AcousticsSpaceManipulator::CalculateNodeBoundingBox(void) const
{
	return (BoxVolumeManipulator::CalculateBoundingBox(GetObject()->GetBoxSize()));
}

int32 AcousticsSpaceManipulator::GetHandleTable(Point3D *handle) const
{
	return (BoxVolumeManipulator::GetHandleTable(GetObject()->GetBoxSize(), handle));
}

void AcousticsSpaceManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	BoxVolumeManipulator::GetHandleData(index, handleData);
}

bool AcousticsSpaceManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	AcousticsSpaceObject *object = GetObject();

	Vector3D newSize = object->GetBoxSize();
	const Vector3D& oldSize = *reinterpret_cast<const Vector3D *>(GetOriginalSize());
	bool move = BoxVolumeManipulator::Resize(resizeData, oldSize, newSize);

	object->SetBoxSize(newSize);
	return (move);
}

void AcousticsSpaceManipulator::Update(void)
{
	if (!(GetManipulatorState() & kManipulatorUpdated))
	{
		SetBoxSize(GetObject()->GetBoxSize());
	}

	SpaceManipulator::Update();
}


OcclusionSpaceManipulator::OcclusionSpaceManipulator(OcclusionSpace *occlusion) :
		SpaceManipulator(occlusion, this, "WorldEditor/portal/Space"),
		BoxVolumeManipulator(occlusion, ColorRGBA(0.75F, 0.0F, 0.0F, 0.75F), kSpaceOutlineColor, "WorldEditor/volume/Occlusion")
{
}

OcclusionSpaceManipulator::~OcclusionSpaceManipulator()
{
}

Box3D OcclusionSpaceManipulator::CalculateNodeBoundingBox(void) const
{
	return (BoxVolumeManipulator::CalculateBoundingBox(GetObject()->GetBoxSize()));
}

int32 OcclusionSpaceManipulator::GetHandleTable(Point3D *handle) const
{
	return (BoxVolumeManipulator::GetHandleTable(GetObject()->GetBoxSize(), handle));
}

void OcclusionSpaceManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	BoxVolumeManipulator::GetHandleData(index, handleData);
}

bool OcclusionSpaceManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	OcclusionSpaceObject *object = GetObject();

	Vector3D newSize = object->GetBoxSize();
	const Vector3D& oldSize = *reinterpret_cast<const Vector3D *>(GetOriginalSize());
	bool move = BoxVolumeManipulator::Resize(resizeData, oldSize, newSize);

	object->SetBoxSize(newSize);
	return (move);
}

void OcclusionSpaceManipulator::Update(void)
{
	if (!(GetManipulatorState() & kManipulatorUpdated))
	{
		SetBoxSize(GetObject()->GetBoxSize());
	}

	SpaceManipulator::Update();
}


PaintSpaceManipulator::PaintSpaceManipulator(PaintSpace *paint) :
		SpaceManipulator(paint, this, "WorldEditor/paint/Space"),
		BoxVolumeManipulator(paint, kSpaceInteriorColor, kSpaceOutlineColor, "WorldEditor/volume/Paint")
{
}

PaintSpaceManipulator::~PaintSpaceManipulator()
{
}

Box3D PaintSpaceManipulator::CalculateNodeBoundingBox(void) const
{
	return (BoxVolumeManipulator::CalculateBoundingBox(GetObject()->GetBoxSize()));
}

int32 PaintSpaceManipulator::GetHandleTable(Point3D *handle) const
{
	return (BoxVolumeManipulator::GetHandleTable(GetObject()->GetBoxSize(), handle));
}

void PaintSpaceManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	BoxVolumeManipulator::GetHandleData(index, handleData);
}

bool PaintSpaceManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	PaintSpaceObject *object = GetObject();

	Vector3D newSize = object->GetBoxSize();
	const Vector3D& oldSize = *reinterpret_cast<const Vector3D *>(GetOriginalSize());
	bool move = BoxVolumeManipulator::Resize(resizeData, oldSize, newSize);

	object->SetBoxSize(newSize);
	return (move);
}

void PaintSpaceManipulator::Update(void)
{
	if (!(GetManipulatorState() & kManipulatorUpdated))
	{
		SetBoxSize(GetObject()->GetBoxSize());
	}

	SpaceManipulator::Update();
}

void PaintSpaceManipulator::HandleDelete(bool undoable)
{
	EditorManipulator::HandleDelete(undoable);

	PaintSpace *paintSpace = GetTargetNode();
	const Hub *hub = paintSpace->GetHub();
	if (hub)
	{
		const Connector *connector = hub->GetFirstIncomingEdge();
		while (connector)
		{
			const Connector *next = connector->GetNextIncomingEdge();

			Node *start = connector->GetStartElement()->GetNode();
			if (start->GetNodeType() == kNodeGeometry)
			{
				Geometry *geometry = static_cast<Geometry *>(start);
				if (geometry->GetConnectedPaintSpace() == paintSpace)
				{
					geometry->SetConnectedPaintSpace(nullptr);
					geometry->InvalidateShaderData();

					Editor::GetManipulator(geometry)->UpdateConnectors();

					if (undoable)
					{
						undoGeometryList.Append(new NodeReference(geometry));
					}
				}
			}

			connector = next;
		}
	}

	PaintPage *paintPage = GetEditor()->GetEditorObject()->GetPaintPage();
	if (paintPage->GetTargetPaintSpace() == paintSpace)
	{
		paintPage->SetTargetPaintSpace(nullptr);
	}
}

void PaintSpaceManipulator::HandleUndelete(void)
{
	EditorManipulator::HandleUndelete();

	PaintSpace *paintSpace = GetTargetNode();
	for (;;)
	{
		const NodeReference *reference = undoGeometryList.First();
		if (!reference)
		{
			break;
		}

		Geometry *geometry = static_cast<Geometry *>(reference->GetNode());
		geometry->SetConnectedPaintSpace(paintSpace);
		geometry->InvalidateShaderData();

		delete reference;
	}
}

void PaintSpaceManipulator::HandleSettingsUpdate(void)
{
	EditorManipulator::HandleSettingsUpdate();

	PaintSpace *paintSpace = GetTargetNode();
	const Hub *hub = paintSpace->GetHub();
	if (hub)
	{
		const Connector *connector = hub->GetFirstIncomingEdge();
		while (connector)
		{
			Node *start = connector->GetStartElement()->GetNode();
			if (start->GetNodeType() == kNodeGeometry)
			{
				Geometry *geometry = static_cast<Geometry *>(start);
				if (geometry->GetConnectedPaintSpace() == paintSpace)
				{
					geometry->InvalidateShaderData();
				}
			}

			connector = connector->GetNextIncomingEdge();
		}
	}

	PaintPage *paintPage = GetEditor()->GetEditorObject()->GetPaintPage();
	if (paintPage->GetTargetPaintSpace() == paintSpace)
	{
		paintPage->SetTargetPaintSpace(nullptr);
	}
}


PhysicsSpaceManipulator::PhysicsSpaceManipulator(PhysicsSpace *physics) :
		SpaceManipulator(physics, this, "WorldEditor/physics/Space"),
		BoxVolumeManipulator(physics, kSpaceInteriorColor, kSpaceOutlineColor, "WorldEditor/volume/Physics")
{
}

PhysicsSpaceManipulator::~PhysicsSpaceManipulator()
{
}

Box3D PhysicsSpaceManipulator::CalculateNodeBoundingBox(void) const
{
	return (BoxVolumeManipulator::CalculateBoundingBox(GetObject()->GetBoxSize()));
}

int32 PhysicsSpaceManipulator::GetHandleTable(Point3D *handle) const
{
	return (BoxVolumeManipulator::GetHandleTable(GetObject()->GetBoxSize(), handle));
}

void PhysicsSpaceManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	BoxVolumeManipulator::GetHandleData(index, handleData);
}

bool PhysicsSpaceManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	PhysicsSpaceObject *object = GetObject();

	Vector3D newSize = object->GetBoxSize();
	const Vector3D& oldSize = *reinterpret_cast<const Vector3D *>(GetOriginalSize());
	bool move = BoxVolumeManipulator::Resize(resizeData, oldSize, newSize);

	object->SetBoxSize(newSize);
	return (move);
}

void PhysicsSpaceManipulator::Update(void)
{
	if (!(GetManipulatorState() & kManipulatorUpdated))
	{
		SetBoxSize(GetObject()->GetBoxSize());
	}

	SpaceManipulator::Update();
}

// ZYUQURM
