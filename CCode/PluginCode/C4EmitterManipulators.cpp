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


#include "C4EmitterManipulators.h"
#include "C4WorldEditor.h"


using namespace C4;


namespace
{
	const ConstColorRGBA kEmitterInteriorColor = {0.75F, 1.0F, 0.0F, 0.75F};
	const ConstColorRGBA kEmitterOutlineColor = {1.0F, 1.0F, 0.0F, 1.0F};
}


EmitterManipulator::EmitterManipulator(Emitter *emitter, VolumeManipulator *volume) : EditorManipulator(emitter, "WorldEditor/particle/Box")
{
	volumeManipulator = volume;
}

EmitterManipulator::~EmitterManipulator()
{
}

Manipulator *EmitterManipulator::Create(Emitter *emitter)
{
	switch (emitter->GetEmitterType())
	{
		case kEmitterBox:

			return (new BoxEmitterManipulator(static_cast<BoxEmitter *>(emitter)));

		case kEmitterCylinder:

			return (new CylinderEmitterManipulator(static_cast<CylinderEmitter *>(emitter)));

		case kEmitterSphere:

			return (new SphereEmitterManipulator(static_cast<SphereEmitter *>(emitter)));
	}

	return (nullptr);
}

const char *EmitterManipulator::GetDefaultNodeName(void) const
{
	return (TheWorldEditor->GetStringTable()->GetString(StringID('NAME', kNodeEmitter, GetTargetNode()->GetEmitterType())));
}

void EmitterManipulator::Select(void)
{
	EditorManipulator::Select();
	volumeManipulator->Select();
}

void EmitterManipulator::Unselect(void)
{
	EditorManipulator::Unselect();
	volumeManipulator->Unselect();
}

bool EmitterManipulator::Pick(const Ray *ray, PickData *data) const
{
	return (volumeManipulator->Pick(ray, data));
}

bool EmitterManipulator::RegionPick(const VisibilityRegion *region) const
{
	return (volumeManipulator->RegionPick(GetTargetNode()->GetWorldTransform(), region));
}

void EmitterManipulator::Render(const ManipulatorRenderData *renderData)
{
	volumeManipulator->Render(renderData);
	EditorManipulator::Render(renderData);
}


BoxEmitterManipulator::BoxEmitterManipulator(BoxEmitter *box) :
		EmitterManipulator(box, this),
		BoxVolumeManipulator(box, kEmitterInteriorColor, kEmitterOutlineColor, "WorldEditor/volume/Emitter")
{
}

BoxEmitterManipulator::~BoxEmitterManipulator()
{
}

bool BoxEmitterManipulator::CalculateNodeSphere(BoundingSphere *sphere) const
{
	BoxVolumeManipulator::CalculateVolumeSphere(GetObject()->GetBoxSize(), sphere);
	return (true);
}

Box3D BoxEmitterManipulator::CalculateNodeBoundingBox(void) const
{
	return (BoxVolumeManipulator::CalculateBoundingBox(GetObject()->GetBoxSize()));
}

int32 BoxEmitterManipulator::GetHandleTable(Point3D *handle) const
{
	return (BoxVolumeManipulator::GetHandleTable(GetObject()->GetBoxSize(), handle)); 
}

void BoxEmitterManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const 
{
	BoxVolumeManipulator::GetHandleData(index, handleData); 
}

bool BoxEmitterManipulator::Resize(const ManipulatorResizeData *resizeData) 
{
	BoxEmitterObject *object = GetObject(); 
 
	Vector3D newSize = object->GetBoxSize();
	const Vector3D& oldSize = *reinterpret_cast<const Vector3D *>(GetOriginalSize());
	bool move = BoxVolumeManipulator::Resize(resizeData, oldSize, newSize);
 
	object->SetBoxSize(newSize);
	return (move);
}

void BoxEmitterManipulator::Update(void)
{
	if (!(GetManipulatorState() & kManipulatorUpdated))
	{
		SetBoxSize(GetObject()->GetBoxSize());
	}

	EmitterManipulator::Update();
}


CylinderEmitterManipulator::CylinderEmitterManipulator(CylinderEmitter *cylinder) :
		EmitterManipulator(cylinder, this),
		CylinderVolumeManipulator(cylinder, kEmitterInteriorColor, kEmitterOutlineColor, "WorldEditor/volume/Emitter")
{
}

CylinderEmitterManipulator::~CylinderEmitterManipulator()
{
}

bool CylinderEmitterManipulator::CalculateNodeSphere(BoundingSphere *sphere) const
{
	const CylinderEmitterObject *object = GetObject();
	CylinderVolumeManipulator::CalculateVolumeSphere(object->GetCylinderSize(), object->GetCylinderHeight(), sphere);
	return (true);
}

Box3D CylinderEmitterManipulator::CalculateNodeBoundingBox(void) const
{
	const CylinderEmitterObject *object = GetObject();
	return (CylinderVolumeManipulator::CalculateBoundingBox(object->GetCylinderSize(), object->GetCylinderHeight()));
}

int32 CylinderEmitterManipulator::GetHandleTable(Point3D *handle) const
{
	const CylinderEmitterObject *object = GetObject();
	return (CylinderVolumeManipulator::GetHandleTable(object->GetCylinderSize(), object->GetCylinderHeight(), handle));
}

void CylinderEmitterManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	CylinderVolumeManipulator::GetHandleData(index, handleData);
}

bool CylinderEmitterManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	CylinderEmitterObject *object = GetObject();

	Vector2D newSize = object->GetCylinderSize();
	float newHeight = object->GetCylinderHeight();
	const Vector2D& oldSize = *reinterpret_cast<const Vector2D *>(GetOriginalSize());
	float oldHeight = GetOriginalSize()[2];
	bool move = CylinderVolumeManipulator::Resize(resizeData, oldSize, oldHeight, newSize, newHeight);

	object->SetCylinderSize(newSize);
	object->SetCylinderHeight(newHeight);
	return (move);
}

void CylinderEmitterManipulator::Update(void)
{
	if (!(GetManipulatorState() & kManipulatorUpdated))
	{
		const CylinderEmitterObject *object = GetObject();
		SetCylinderSize(object->GetCylinderSize(), object->GetCylinderHeight());
	}

	EmitterManipulator::Update();
}


SphereEmitterManipulator::SphereEmitterManipulator(SphereEmitter *sphere) :
		EmitterManipulator(sphere, this),
		SphereVolumeManipulator(sphere, kEmitterInteriorColor, kEmitterOutlineColor, "WorldEditor/volume/Emitter")
{
}

SphereEmitterManipulator::~SphereEmitterManipulator()
{
}

bool SphereEmitterManipulator::CalculateNodeSphere(BoundingSphere *sphere) const
{
	SphereVolumeManipulator::CalculateVolumeSphere(GetObject()->GetSphereSize(), sphere);
	return (true);
}

Box3D SphereEmitterManipulator::CalculateNodeBoundingBox(void) const
{
	return (SphereVolumeManipulator::CalculateBoundingBox(GetObject()->GetSphereSize()));
}

int32 SphereEmitterManipulator::GetHandleTable(Point3D *handle) const
{
	return (SphereVolumeManipulator::GetHandleTable(GetObject()->GetSphereSize(), handle));
}

void SphereEmitterManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	SphereVolumeManipulator::GetHandleData(index, handleData);
}

bool SphereEmitterManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	SphereEmitterObject *object = GetObject();

	Vector3D newSize = object->GetSphereSize();
	const Vector3D& oldSize = *reinterpret_cast<const Vector3D *>(GetOriginalSize());
	bool move = SphereVolumeManipulator::Resize(resizeData, oldSize, newSize);

	object->SetSphereSize(newSize);
	return (move);
}

void SphereEmitterManipulator::Update(void)
{
	if (!(GetManipulatorState() & kManipulatorUpdated))
	{
		SetSphereSize(GetObject()->GetSphereSize());
	}

	EmitterManipulator::Update();
}

// ZYUQURM
