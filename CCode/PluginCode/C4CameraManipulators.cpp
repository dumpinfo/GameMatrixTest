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


#include "C4CameraManipulators.h"
#include "C4WorldEditor.h"


using namespace C4;


namespace
{
	const ConstColorRGBA kCameraInteriorColor = {0.0F, 0.0F, 1.0F, 0.75F};
	const ConstColorRGBA kCameraOutlineColor = {1.0F, 1.0F, 1.0F, 1.0F};
}


CameraManipulator::CameraManipulator(Camera *camera, VolumeManipulator *volume) : EditorManipulator(camera, "WorldEditor/camera/Camera")
{
	volumeManipulator = volume;
}

CameraManipulator::~CameraManipulator()
{
}

Manipulator *CameraManipulator::Create(Camera *camera)
{
	switch (camera->GetCameraType())
	{
		case kCameraFrustum:

			return (new FrustumCameraManipulator(static_cast<FrustumCamera *>(camera)));
	}

	return (nullptr);
}

const char *CameraManipulator::GetDefaultNodeName(void) const
{
	return (TheWorldEditor->GetStringTable()->GetString(StringID('NAME', kNodeCamera, GetTargetNode()->GetCameraType())));
}

void CameraManipulator::Preprocess(void)
{
	SetManipulatorState(GetManipulatorState() | kManipulatorShowIcon);
	EditorManipulator::Preprocess();
}

void CameraManipulator::Select(void)
{
	EditorManipulator::Select();
	volumeManipulator->Select();
}

void CameraManipulator::Unselect(void)
{
	EditorManipulator::Unselect();
	volumeManipulator->Unselect();
}

void CameraManipulator::Render(const ManipulatorRenderData *renderData)
{
	if (Selected())
	{
		volumeManipulator->Render(renderData);
	}

	EditorManipulator::Render(renderData);
}


FrustumCameraManipulator::FrustumCameraManipulator(FrustumCamera *frustumCamera) :
		CameraManipulator(frustumCamera, this),
		ProjectionVolumeManipulator(frustumCamera, kCameraInteriorColor, kCameraOutlineColor, "WorldEditor/volume/Camera")
{
}

FrustumCameraManipulator::~FrustumCameraManipulator()
{
}

bool FrustumCameraManipulator::CalculateNodeSphere(BoundingSphere *sphere) const
{
	const FrustumCameraObject *object = GetObject();
	float h = object->GetFarDepth();
	float rx = h / object->GetFocalLength();
	float ry = rx * object->GetAspectRatio();

	ProjectionVolumeManipulator::CalculateVolumeSphere(Vector2D(rx, ry), h, sphere);
	return (true);
}

Box3D FrustumCameraManipulator::CalculateNodeBoundingBox(void) const
{
	const FrustumCameraObject *object = GetObject();
	float h = object->GetFarDepth();
	float rx = h / object->GetFocalLength();
	float ry = rx * object->GetAspectRatio();

	return (ProjectionVolumeManipulator::CalculateBoundingBox(Vector2D(rx, ry), h));
}

int32 FrustumCameraManipulator::GetHandleTable(Point3D *handle) const 
{
	const FrustumCameraObject *object = GetObject();
	float h = object->GetFarDepth(); 
	float rx = h / object->GetFocalLength();
	float ry = rx * object->GetAspectRatio(); 

	return (ProjectionVolumeManipulator::GetHandleTable(Vector2D(rx, ry), h, handle));
} 

void FrustumCameraManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const 
{ 
	ProjectionVolumeManipulator::GetHandleData(index, handleData);
}

bool FrustumCameraManipulator::Resize(const ManipulatorResizeData *resizeData) 
{
	FrustumCameraObject *object = GetObject();
	float aspect = object->GetAspectRatio();

	float newHeight = object->GetFarDepth();
	float newSizeX = newHeight / object->GetFocalLength();
	float newSizeY = newSizeX * aspect;
	Vector2D newSize(newSizeX, newSizeY);

	float oldHeight = GetOriginalSize()[0];
	float oldSizeX = oldHeight / GetOriginalSize()[1];
	float oldSizeY = oldSizeX * aspect;

	ProjectionVolumeManipulator::Resize(resizeData, Vector2D(oldSizeX, oldSizeY), oldHeight, newSize, newHeight);

	float size = (resizeData->handleFlags & kManipulatorHandleNonzeroX) ? newSize.x : newSize.y / aspect;
	object->SetFocalLength(newHeight / size);
	object->SetFarDepth(newHeight);
	return (false);
}

void FrustumCameraManipulator::Update(void)
{
	if (!(GetManipulatorState() & kManipulatorUpdated))
	{
		const FrustumCameraObject *object = GetObject();
		float h = object->GetFarDepth();
		float rx = h / object->GetFocalLength();
		float ry = rx * object->GetAspectRatio();

		SetProjectionSize(Vector2D(rx, ry), h);
	}

	CameraManipulator::Update();
}

// ZYUQURM
