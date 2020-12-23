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


#include "C4LightManipulators.h"
#include "C4WorldEditor.h"


using namespace C4;


namespace
{
	const ConstColorRGBA kLightInteriorColor = {1.0F, 1.0F, 0.25F, 0.75F};
	const ConstColorRGBA kLightOutlineColor = {1.0F, 1.0F, 0.0F, 1.0F};
}


SharedVertexBuffer InfiniteLightManipulator::directionVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic);


LightManipulator::LightManipulator(Light *light, const char *iconName) : EditorManipulator(light, iconName)
{
}

LightManipulator::~LightManipulator()
{
}

Manipulator *LightManipulator::Create(Light *light)
{
	switch (light->GetLightType())
	{
		case kLightInfinite:

			return (new InfiniteLightManipulator(static_cast<InfiniteLight *>(light)));

		case kLightPoint:
		case kLightCube:

			return (new PointLightManipulator(static_cast<PointLight *>(light)));

		case kLightSpot:

			return (new SpotLightManipulator(static_cast<SpotLight *>(light)));
	}

	return (nullptr);
}

const char *LightManipulator::GetDefaultNodeName(void) const
{
	return (TheWorldEditor->GetStringTable()->GetString(StringID('NAME', kNodeLight, GetTargetNode()->GetLightType())));
}

void LightManipulator::Preprocess(void)
{
	SetManipulatorState(GetManipulatorState() | kManipulatorShowIcon);
	EditorManipulator::Preprocess();
}


InfiniteLightManipulator::InfiniteLightManipulator(InfiniteLight *infiniteLight) :
		LightManipulator(infiniteLight, "WorldEditor/light/Infinite"),
		directionColorAttribute(kLightInteriorColor),
		directionTextureAttribute("WorldEditor/direction"),
		directionRenderable(kRenderQuads)
{
	directionRenderable.SetAmbientBlendState(kBlendInterpolate);
	directionRenderable.SetShaderFlags(kShaderAmbientEffect | kShaderVertexPostboard | kShaderScaleVertex);
	directionRenderable.SetRenderParameterPointer(&directionSizeVector);
	directionRenderable.SetTransformable(infiniteLight);

	directionRenderable.SetVertexCount(4);
	directionRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &directionVertexBuffer, sizeof(LightVertex));
	directionRenderable.SetVertexAttributeArray(kArrayPosition, 0, 3);
	directionRenderable.SetVertexAttributeArray(kArrayRadius, sizeof(Point3D), 1);
	directionRenderable.SetVertexAttributeArray(kArrayTexcoord, sizeof(Point3D) + 4, 2);

	directionAttributeList.Append(&directionColorAttribute);
	directionAttributeList.Append(&directionTextureAttribute);
	directionRenderable.SetMaterialAttributeList(&directionAttributeList);

	if (directionVertexBuffer.Retain() == 1)
	{
		static const LightVertex lightVertex[4] =
		{
			{Point3D(0.0F, 0.0F, -2.0F), -1.0F, Point2D(0.0F, 1.0F)},
			{Point3D(0.0F, 0.0F, -2.0F), 1.0F, Point2D(1.0F, 1.0F)},
			{Point3D(0.0F, 0.0F, 0.0F), 1.0F, Point2D(1.0F, 0.0F)},
			{Point3D(0.0F, 0.0F, 0.0F), -1.0F, Point2D(0.0F, 0.0F)}
		};

		directionVertexBuffer.Establish(sizeof(LightVertex) * 4, lightVertex);
	}
}

InfiniteLightManipulator::~InfiniteLightManipulator()
{
	directionVertexBuffer.Release();
}

void InfiniteLightManipulator::Render(const ManipulatorRenderData *renderData)
{
	LightManipulator::Render(renderData); 

	List<Renderable> *renderList = renderData->manipulatorList;
	if (renderList) 
	{
		float scale = renderData->viewportScale * 24.0F; 
		directionSizeVector.Set(scale, scale, scale, scale);
		renderList->Append(&directionRenderable);
	} 
}
 
 
PointLightManipulator::PointLightManipulator(PointLight *pointLight) :
		LightManipulator(pointLight, (pointLight->GetLightType() == kLightPoint) ? "WorldEditor/light/Point" : "WorldEditor/light/Cube"),
		SphereVolumeManipulator(pointLight, kLightInteriorColor, kLightOutlineColor, "WorldEditor/volume/Light")
{ 
}

PointLightManipulator::~PointLightManipulator()
{
}

bool PointLightManipulator::CalculateNodeSphere(BoundingSphere *sphere) const
{
	SphereVolumeManipulator::CalculateVolumeSphere(GetObject()->GetLightRange(), sphere);
	return (true);
}

Box3D PointLightManipulator::CalculateNodeBoundingBox(void) const
{
	return (SphereVolumeManipulator::CalculateBoundingBox(GetObject()->GetLightRange()));
}

void PointLightManipulator::Select(void)
{
	EditorManipulator::Select();
	SphereVolumeManipulator::Select();
}

void PointLightManipulator::Unselect(void)
{
	EditorManipulator::Unselect();
	SphereVolumeManipulator::Unselect();
}

int32 PointLightManipulator::GetHandleTable(Point3D *handle) const
{
	return (SphereVolumeManipulator::GetHandleTable(GetObject()->GetLightRange(), handle));
}

void PointLightManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	SphereVolumeManipulator::GetCircularHandleData(index, handleData);
}

bool PointLightManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	PointLightObject *object = GetObject();

	float newRadius = object->GetLightRange();
	float oldRadius = GetOriginalSize()[0];
	SphereVolumeManipulator::Resize(resizeData, oldRadius, newRadius);

	object->SetLightRange(newRadius);
	return (false);
}

void PointLightManipulator::Update(void)
{
	if (!(GetManipulatorState() & kManipulatorUpdated))
	{
		SetSphereSize(GetObject()->GetLightRange());
	}

	LightManipulator::Update();
}

void PointLightManipulator::Render(const ManipulatorRenderData *renderData)
{
	if (Selected())
	{
		SphereVolumeManipulator::Render(renderData);
	}

	EditorManipulator::Render(renderData);
}


SpotLightManipulator::SpotLightManipulator(SpotLight *spotLight) :
		LightManipulator(spotLight, "WorldEditor/light/Spot"),
		ProjectionVolumeManipulator(spotLight, kLightInteriorColor, kLightOutlineColor, "WorldEditor/volume/Light")
{
}

SpotLightManipulator::~SpotLightManipulator()
{
}

bool SpotLightManipulator::CalculateNodeSphere(BoundingSphere *sphere) const
{
	const SpotLightObject *object = GetObject();
	float h = object->GetLightRange();
	float rx = h / object->GetApexTangent();
	float ry = rx * object->GetAspectRatio();

	ProjectionVolumeManipulator::CalculateVolumeSphere(Vector2D(rx, ry), h, sphere);
	return (true);
}

Box3D SpotLightManipulator::CalculateNodeBoundingBox(void) const
{
	const SpotLightObject *object = GetObject();
	float h = object->GetLightRange();
	float rx = h / object->GetApexTangent();
	float ry = rx * object->GetAspectRatio();

	return (ProjectionVolumeManipulator::CalculateBoundingBox(Vector2D(rx, ry), h));
}

int32 SpotLightManipulator::GetHandleTable(Point3D *handle) const
{
	const SpotLightObject *object = GetObject();
	float h = object->GetLightRange();
	float rx = h / object->GetApexTangent();
	float ry = rx * object->GetAspectRatio();

	return (ProjectionVolumeManipulator::GetHandleTable(Vector2D(rx, ry), h, handle));
}

void SpotLightManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	ProjectionVolumeManipulator::GetHandleData(index, handleData);
}

bool SpotLightManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	SpotLightObject *object = GetObject();
	float aspect = object->GetAspectRatio();

	float newHeight = object->GetLightRange();
	float newSizeX = newHeight / object->GetApexTangent();
	float newSizeY = newSizeX * aspect;
	Vector2D newSize(newSizeX, newSizeY);

	float oldHeight = GetOriginalSize()[0];
	float oldSizeX = oldHeight / GetOriginalSize()[1];
	float oldSizeY = oldSizeX * aspect;

	ProjectionVolumeManipulator::Resize(resizeData, Vector2D(oldSizeX, oldSizeY), oldHeight, newSize, newHeight);

	float size = (resizeData->handleFlags & kManipulatorHandleNonzeroX) ? newSize.x : newSize.y / aspect;
	object->SetApexTangent(newHeight / size);
	object->SetLightRange(newHeight);
	return (false);
}

void SpotLightManipulator::Select(void)
{
	EditorManipulator::Select();
	ProjectionVolumeManipulator::Select();
}

void SpotLightManipulator::Unselect(void)
{
	EditorManipulator::Unselect();
	ProjectionVolumeManipulator::Unselect();
}

void SpotLightManipulator::Update(void)
{
	if (!(GetManipulatorState() & kManipulatorUpdated))
	{
		const SpotLightObject *object = GetObject();
		float h = object->GetLightRange();
		float rx = h / object->GetApexTangent();
		float ry = rx * object->GetAspectRatio();

		SetProjectionSize(Vector2D(rx, ry), h);
	}

	LightManipulator::Update();
}

void SpotLightManipulator::Render(const ManipulatorRenderData *renderData)
{
	if (Selected())
	{
		ProjectionVolumeManipulator::Render(renderData);
	}

	EditorManipulator::Render(renderData);
}

// ZYUQURM
