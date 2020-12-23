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


#include "C4SourceManipulators.h"
#include "C4WorldEditor.h"


using namespace C4;


namespace
{
	const ConstColorRGBA kSourceInteriorColor = {0.25F, 1.0F, 1.0F, 0.75F};
	const ConstColorRGBA kSourceOutlineColor = {0.0F, 1.0F, 1.0F, 1.0F};
}


SourceManipulator::SourceManipulator(Source *source, VolumeManipulator *volume, const char *iconName) : EditorManipulator(source, iconName)
{
	volumeManipulator = volume;
}

SourceManipulator::~SourceManipulator()
{
}

Manipulator *SourceManipulator::Create(Source *source)
{
	switch (source->GetSourceType())
	{
		case kSourceAmbient:

			return (new SourceManipulator(source, nullptr, "WorldEditor/source/Ambient"));

		case kSourceOmni:

			return (new OmniSourceManipulator(static_cast<OmniSource *>(source)));

		case kSourceDirected:

			return (new DirectedSourceManipulator(static_cast<DirectedSource *>(source)));
	}

	return (nullptr);
}

const char *SourceManipulator::GetDefaultNodeName(void) const
{
	return (TheWorldEditor->GetStringTable()->GetString(StringID('NAME', kNodeSource, GetTargetNode()->GetSourceType())));
}

void SourceManipulator::Preprocess(void)
{
	SetManipulatorState(GetManipulatorState() | kManipulatorShowIcon);
	EditorManipulator::Preprocess();
}

void SourceManipulator::Select(void)
{
	EditorManipulator::Select();

	if (volumeManipulator)
	{
		volumeManipulator->Select();
	}
}

void SourceManipulator::Unselect(void)
{
	EditorManipulator::Unselect();

	if (volumeManipulator)
	{
		volumeManipulator->Unselect();
	}
}


OmniSourceManipulator::OmniSourceManipulator(OmniSource *omniSource) :
		SourceManipulator(omniSource, this, "WorldEditor/source/Omni"),
		SphereVolumeManipulator(omniSource, kSourceInteriorColor, kSourceOutlineColor, "WorldEditor/volume/Source")
{
}

OmniSourceManipulator::~OmniSourceManipulator()
{
}

bool OmniSourceManipulator::CalculateNodeSphere(BoundingSphere *sphere) const
{
	SphereVolumeManipulator::CalculateVolumeSphere(GetObject()->GetSourceRange(), sphere);
	return (true);
}

Box3D OmniSourceManipulator::CalculateNodeBoundingBox(void) const
{
	return (SphereVolumeManipulator::CalculateBoundingBox(GetObject()->GetSourceRange()));
}

int32 OmniSourceManipulator::GetHandleTable(Point3D *handle) const
{
	return (SphereVolumeManipulator::GetHandleTable(GetObject()->GetSourceRange(), handle));
}
 
void OmniSourceManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	SphereVolumeManipulator::GetCircularHandleData(index, handleData); 
}
 
bool OmniSourceManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	OmniSourceObject *object = GetObject(); 

	float newRadius = object->GetSourceRange(); 
	float oldRadius = GetOriginalSize()[0]; 
	SphereVolumeManipulator::Resize(resizeData, oldRadius, newRadius);

	object->SetSourceRange(newRadius);
	return (false); 
}

void OmniSourceManipulator::Update(void)
{
	if (!(GetManipulatorState() & kManipulatorUpdated))
	{
		SetSphereSize(GetObject()->GetSourceRange());
	}

	SourceManipulator::Update();
}

void OmniSourceManipulator::Render(const ManipulatorRenderData *renderData)
{
	if (Selected())
	{
		SphereVolumeManipulator::Render(renderData);
	}

	EditorManipulator::Render(renderData);
}


DirectedSourceManipulator::DirectedSourceManipulator(DirectedSource *directedSource) :
		SourceManipulator(directedSource, this, "WorldEditor/source/Directed"),
		ProjectionVolumeManipulator(directedSource, kSourceInteriorColor, kSourceOutlineColor, "WorldEditor/volume/Source")
{
}

DirectedSourceManipulator::~DirectedSourceManipulator()
{
}

bool DirectedSourceManipulator::CalculateNodeSphere(BoundingSphere *sphere) const
{
	const DirectedSourceObject *object = GetObject();
	float h = object->GetSourceRange();
	float r = h / object->GetApexTangent();

	ProjectionVolumeManipulator::CalculateVolumeSphere(Vector2D(r, r), h, sphere);
	return (true);
}

Box3D DirectedSourceManipulator::CalculateNodeBoundingBox(void) const
{
	const DirectedSourceObject *object = GetObject();
	float h = object->GetSourceRange();
	float r = h / object->GetApexTangent();

	return (ProjectionVolumeManipulator::CalculateBoundingBox(Vector2D(r, r), h));
}

int32 DirectedSourceManipulator::GetHandleTable(Point3D *handle) const
{
	const DirectedSourceObject *object = GetObject();
	float h = object->GetSourceRange();
	float r = h / object->GetApexTangent();

	return (ProjectionVolumeManipulator::GetHandleTable(Vector2D(r, r), h, handle));
}

void DirectedSourceManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	ProjectionVolumeManipulator::GetHandleData(index, handleData);
}

bool DirectedSourceManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	DirectedSourceObject *object = GetObject();

	float newHeight = object->GetSourceRange();
	float newRadius = newHeight / object->GetApexTangent();
	Vector2D newSize(newRadius, newRadius);

	float oldHeight = GetOriginalSize()[0];
	float oldRadius = oldHeight / GetOriginalSize()[1];

	ProjectionVolumeManipulator::Resize(resizeData, Vector2D(oldRadius, oldRadius), oldHeight, newSize, newHeight);

	float radius = (resizeData->handleFlags & kManipulatorHandleNonzeroX) ? newSize.x : newSize.y;
	object->SetApexTangent(newHeight / radius);
	object->SetSourceRange(newHeight);
	return (false);
}

void DirectedSourceManipulator::Update(void)
{
	if (!(GetManipulatorState() & kManipulatorUpdated))
	{
		const DirectedSourceObject *object = GetObject();
		float h = object->GetSourceRange();
		float r = h / object->GetApexTangent();

		SetProjectionSize(Vector2D(r, r), h);
	}

	SourceManipulator::Update();
}

void DirectedSourceManipulator::Render(const ManipulatorRenderData *renderData)
{
	if (Selected())
	{
		ProjectionVolumeManipulator::Render(renderData);
	}

	EditorManipulator::Render(renderData);
}

// ZYUQURM
