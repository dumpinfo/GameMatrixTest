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


#include "C4TriggerManipulators.h"
#include "C4WorldEditor.h"


using namespace C4;


namespace
{
	const ConstColorRGBA kTriggerInteriorColor = {0.5F, 0.0F, 1.0F, 0.75F};
	const ConstColorRGBA kTriggerOutlineColor = {1.0F, 1.0F, 1.0F, 1.0F};
}


TriggerManipulator::TriggerManipulator(Trigger *trigger, VolumeManipulator *volume) : EditorManipulator(trigger, "WorldEditor/trigger/Box")
{
	volumeManipulator = volume;
}

TriggerManipulator::~TriggerManipulator()
{
}

Manipulator *TriggerManipulator::Create(Trigger *trigger)
{
	switch (trigger->GetTriggerType())
	{
		case kTriggerBox:

			return (new BoxTriggerManipulator(static_cast<BoxTrigger *>(trigger)));

		case kTriggerCylinder:

			return (new CylinderTriggerManipulator(static_cast<CylinderTrigger *>(trigger)));

		case kTriggerSphere:

			return (new SphereTriggerManipulator(static_cast<SphereTrigger *>(trigger)));
	}

	return (nullptr);
}

const char *TriggerManipulator::GetDefaultNodeName(void) const
{
	return (TheWorldEditor->GetStringTable()->GetString(StringID('NAME', kNodeTrigger, GetTargetNode()->GetTriggerType())));
}

void TriggerManipulator::Select(void)
{
	EditorManipulator::Select();
	volumeManipulator->Select();
}

void TriggerManipulator::Unselect(void)
{
	EditorManipulator::Unselect();
	volumeManipulator->Unselect();
}

bool TriggerManipulator::Pick(const Ray *ray, PickData *data) const
{
	return (volumeManipulator->Pick(ray, data));
}

bool TriggerManipulator::RegionPick(const VisibilityRegion *region) const
{
	return (volumeManipulator->RegionPick(GetTargetNode()->GetWorldTransform(), region));
}

void TriggerManipulator::Render(const ManipulatorRenderData *renderData)
{
	volumeManipulator->Render(renderData);
	EditorManipulator::Render(renderData);
}


BoxTriggerManipulator::BoxTriggerManipulator(BoxTrigger *box) :
		TriggerManipulator(box, this),
		BoxVolumeManipulator(box, kTriggerInteriorColor, kTriggerOutlineColor, "WorldEditor/volume/Trigger")
{
}

BoxTriggerManipulator::~BoxTriggerManipulator()
{
}

bool BoxTriggerManipulator::CalculateNodeSphere(BoundingSphere *sphere) const
{
	BoxVolumeManipulator::CalculateVolumeSphere(GetObject()->GetBoxSize(), sphere);
	return (true);
}

Box3D BoxTriggerManipulator::CalculateNodeBoundingBox(void) const
{
	return (BoxVolumeManipulator::CalculateBoundingBox(GetObject()->GetBoxSize()));
}

int32 BoxTriggerManipulator::GetHandleTable(Point3D *handle) const
{
	return (BoxVolumeManipulator::GetHandleTable(GetObject()->GetBoxSize(), handle)); 
}

void BoxTriggerManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const 
{
	BoxVolumeManipulator::GetHandleData(index, handleData); 
}

bool BoxTriggerManipulator::Resize(const ManipulatorResizeData *resizeData) 
{
	BoxTriggerObject *object = GetObject(); 
 
	Vector3D newSize = object->GetBoxSize();
	const Vector3D& oldSize = *reinterpret_cast<const Vector3D *>(GetOriginalSize());
	bool move = BoxVolumeManipulator::Resize(resizeData, oldSize, newSize);
 
	object->SetBoxSize(newSize);
	return (move);
}

void BoxTriggerManipulator::Update(void)
{
	if (!(GetManipulatorState() & kManipulatorUpdated))
	{
		SetBoxSize(GetObject()->GetBoxSize());
	}

	TriggerManipulator::Update();
}


CylinderTriggerManipulator::CylinderTriggerManipulator(CylinderTrigger *cylinder) :
		TriggerManipulator(cylinder, this),
		CylinderVolumeManipulator(cylinder, kTriggerInteriorColor, kTriggerOutlineColor, "WorldEditor/volume/Trigger")
{
}

CylinderTriggerManipulator::~CylinderTriggerManipulator()
{
}

bool CylinderTriggerManipulator::CalculateNodeSphere(BoundingSphere *sphere) const
{
	const CylinderTriggerObject *object = GetObject();
	CylinderVolumeManipulator::CalculateVolumeSphere(object->GetCylinderSize(), object->GetCylinderHeight(), sphere);
	return (true);
}

Box3D CylinderTriggerManipulator::CalculateNodeBoundingBox(void) const
{
	const CylinderTriggerObject *object = GetObject();
	return (CylinderVolumeManipulator::CalculateBoundingBox(object->GetCylinderSize(), object->GetCylinderHeight()));
}

int32 CylinderTriggerManipulator::GetHandleTable(Point3D *handle) const
{
	const CylinderTriggerObject *object = GetObject();
	return (CylinderVolumeManipulator::GetHandleTable(object->GetCylinderSize(), object->GetCylinderHeight(), handle));
}

void CylinderTriggerManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	CylinderVolumeManipulator::GetHandleData(index, handleData);
}

bool CylinderTriggerManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	CylinderTriggerObject *object = GetObject();

	Vector2D newSize = object->GetCylinderSize();
	float newHeight = object->GetCylinderHeight();
	const Vector2D& oldSize = *reinterpret_cast<const Vector2D *>(GetOriginalSize());
	float oldHeight = GetOriginalSize()[2];
	bool move = CylinderVolumeManipulator::Resize(resizeData, oldSize, oldHeight, newSize, newHeight);

	object->SetCylinderSize(newSize);
	object->SetCylinderHeight(newHeight);
	return (move);
}

void CylinderTriggerManipulator::Update(void)
{
	if (!(GetManipulatorState() & kManipulatorUpdated))
	{
		const CylinderTriggerObject *object = GetObject();
		SetCylinderSize(object->GetCylinderSize(), object->GetCylinderHeight());
	}

	TriggerManipulator::Update();
}


SphereTriggerManipulator::SphereTriggerManipulator(SphereTrigger *sphere) :
		TriggerManipulator(sphere, this),
		SphereVolumeManipulator(sphere, kTriggerInteriorColor, kTriggerOutlineColor, "WorldEditor/volume/Trigger")
{
}

SphereTriggerManipulator::~SphereTriggerManipulator()
{
}

bool SphereTriggerManipulator::CalculateNodeSphere(BoundingSphere *sphere) const
{
	SphereVolumeManipulator::CalculateVolumeSphere(GetObject()->GetSphereSize(), sphere);
	return (true);
}

Box3D SphereTriggerManipulator::CalculateNodeBoundingBox(void) const
{
	return (SphereVolumeManipulator::CalculateBoundingBox(GetObject()->GetSphereSize()));
}

int32 SphereTriggerManipulator::GetHandleTable(Point3D *handle) const
{
	return (SphereVolumeManipulator::GetHandleTable(GetObject()->GetSphereSize(), handle));
}

void SphereTriggerManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	SphereVolumeManipulator::GetHandleData(index, handleData);
}

bool SphereTriggerManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	SphereTriggerObject *object = GetObject();

	Vector3D newSize = object->GetSphereSize();
	const Vector3D& oldSize = *reinterpret_cast<const Vector3D *>(GetOriginalSize());
	bool move = SphereVolumeManipulator::Resize(resizeData, oldSize, newSize);

	object->SetSphereSize(newSize);
	return (move);
}

void SphereTriggerManipulator::Update(void)
{
	if (!(GetManipulatorState() & kManipulatorUpdated))
	{
		SetSphereSize(GetObject()->GetSphereSize());
	}

	TriggerManipulator::Update();
}

// ZYUQURM
