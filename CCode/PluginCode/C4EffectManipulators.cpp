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


#include "C4EffectManipulators.h"
#include "C4WorldEditor.h"


using namespace C4;


namespace
{
	const ConstColorRGBA kEffectInteriorColor = {1.0F, 0.75F, 0.0F, 0.75F};
	const ConstColorRGBA kEffectOutlineColor = {1.0F, 1.0F, 0.0F, 1.0F};
}


EffectManipulator::EffectManipulator(Effect *effect, VolumeManipulator *volume, const char *iconName) : EditorManipulator(effect, iconName)
{
	volumeManipulator = volume;
}

EffectManipulator::~EffectManipulator()
{
}

Manipulator *EffectManipulator::Create(Effect *effect)
{
	switch (effect->GetEffectType())
	{
		case kEffectParticleSystem:

			return (new ParticleSystemManipulator(static_cast<ParticleSystem *>(effect)));

		case kEffectShaft:
		{
			ShaftEffect *shaftEffect = static_cast<ShaftEffect *>(effect);
			switch (shaftEffect->GetShaftType())
			{
				case kShaftBox:

					return (new BoxShaftEffectManipulator(static_cast<BoxShaftEffect *>(shaftEffect)));

				case kShaftCylinder:

					return (new CylinderShaftEffectManipulator(static_cast<CylinderShaftEffect *>(shaftEffect)));

				case kShaftTruncatedPyramid:

					return (new TruncatedPyramidShaftEffectManipulator(static_cast<TruncatedPyramidShaftEffect *>(shaftEffect)));

				case kShaftTruncatedCone:

					return (new TruncatedConeShaftEffectManipulator(static_cast<TruncatedConeShaftEffect *>(shaftEffect)));
			}
		}

		case kEffectQuad:

			return (new QuadEffectManipulator(static_cast<QuadEffect *>(effect)));

		case kEffectFlare:

			return (new FlareEffectManipulator(static_cast<FlareEffect *>(effect)));

		case kEffectBeam:

			return (new BeamEffectManipulator(static_cast<BeamEffect *>(effect)));

		case kEffectTube:

			return (new TubeEffectManipulator(static_cast<TubeEffect *>(effect)));

		case kEffectFire:

			return (new FireEffectManipulator(static_cast<FireEffect *>(effect)));

		case kEffectPanel:

			return (new PanelEffectManipulator(static_cast<PanelEffect *>(effect)));
	}

	return (nullptr);
}

const char *EffectManipulator::GetDefaultNodeName(void) const
{
	return (TheWorldEditor->GetStringTable()->GetString(StringID('NAME', kNodeEffect, GetTargetNode()->GetEffectType())));
}

void EffectManipulator::Invalidate(void)
{
	EditorManipulator::Invalidate();

	GetTargetNode()->PurgeIncomingEdges();
}

void EffectManipulator::Select(void)
{
	EditorManipulator::Select();
	volumeManipulator->Select();
}

void EffectManipulator::Unselect(void) 
{
	EditorManipulator::Unselect();
	volumeManipulator->Unselect(); 
}
 
void EffectManipulator::HandleSizeUpdate(int32 count, const float *size)
{
	EditorManipulator::HandleSizeUpdate(count, size); 
	GetTargetNode()->UpdateEffectGeometry();
} 
 
void EffectManipulator::InvalidateShaderData(void)
{
	GetTargetNode()->InvalidateShaderData();
} 

bool EffectManipulator::Pick(const Ray *ray, PickData *data) const
{
	return (volumeManipulator->Pick(ray, data));
}

bool EffectManipulator::RegionPick(const VisibilityRegion *region) const
{
	return (volumeManipulator->RegionPick(GetTargetNode()->GetWorldTransform(), region));
}

void EffectManipulator::Render(const ManipulatorRenderData *renderData)
{
	volumeManipulator->Render(renderData);
	EditorManipulator::Render(renderData);
}


ParticleSystemManipulator::ParticleSystemManipulator(ParticleSystem *particleSystem) : EditorManipulator(particleSystem, "WorldEditor/particle/Particle")
{
}

ParticleSystemManipulator::~ParticleSystemManipulator()
{
}

const char *ParticleSystemManipulator::GetDefaultNodeName(void) const
{
	const ParticleSystemRegistration *registration = ParticleSystem::FindRegistration(GetTargetNode()->GetParticleSystemType());
	if (registration)
	{
		const char *name = registration->GetParticleSystemName();
		if (name)
		{
			return (name);
		}
	}

	return (TheWorldEditor->GetStringTable()->GetString(StringID('NAME', kNodeEffect, kEffectParticleSystem)));
}

void ParticleSystemManipulator::Preprocess(void)
{
	SetManipulatorState(GetManipulatorState() | kManipulatorShowIcon);
	EditorManipulator::Preprocess();
}

bool ParticleSystemManipulator::MaterialSettable(void) const
{
	return (true);
}

bool ParticleSystemManipulator::MaterialRemovable(void) const
{
	return (true);
}

int32 ParticleSystemManipulator::GetMaterialCount(void) const
{
	return (1);
}

MaterialObject *ParticleSystemManipulator::GetMaterial(int32 index) const
{
	return (GetTargetNode()->GetMaterialObject());
}

void ParticleSystemManipulator::SetMaterial(MaterialObject *material)
{
	ParticleSystem *particleSystem = GetTargetNode();
	particleSystem->SetMaterialObject(material);
}

void ParticleSystemManipulator::ReplaceMaterial(MaterialObject *oldMaterial, MaterialObject *newMaterial)
{
	ParticleSystem *particleSystem = GetTargetNode();
	if (particleSystem->GetMaterialObject() == oldMaterial)
	{
		particleSystem->SetMaterialObject(newMaterial);
	}
}

void ParticleSystemManipulator::RemoveMaterial(void)
{
	ParticleSystem *particleSystem = GetTargetNode();
	particleSystem->SetMaterialObject(nullptr);
}

void ParticleSystemManipulator::InvalidateShaderData(void)
{
	GetTargetNode()->InvalidateShaderData();
}


BoxShaftEffectManipulator::BoxShaftEffectManipulator(BoxShaftEffect *boxShaftEffect) :
		EffectManipulator(boxShaftEffect, this, "WorldEditor/effect/BoxShaft"),
		BoxVolumeManipulator(boxShaftEffect, kEffectInteriorColor, kEffectOutlineColor, "WorldEditor/volume/Shaft")
{
}

BoxShaftEffectManipulator::~BoxShaftEffectManipulator()
{
}

const char *BoxShaftEffectManipulator::GetDefaultNodeName(void) const
{
	return (TheWorldEditor->GetStringTable()->GetString(StringID('NAME', kNodeEffect, kEffectShaft, kShaftBox)));
}

Box3D BoxShaftEffectManipulator::CalculateNodeBoundingBox(void) const
{
	return (BoxVolumeManipulator::CalculateBoundingBox(GetObject()->GetBoxSize()));
}

int32 BoxShaftEffectManipulator::GetHandleTable(Point3D *handle) const
{
	return (BoxVolumeManipulator::GetHandleTable(GetObject()->GetBoxSize(), handle));
}

void BoxShaftEffectManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	BoxVolumeManipulator::GetHandleData(index, handleData);
}

bool BoxShaftEffectManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	BoxShaftEffectObject *object = GetObject();

	Vector3D newSize = object->GetBoxSize();
	const Vector3D& oldSize = *reinterpret_cast<const Vector3D *>(GetOriginalSize());
	bool move = BoxVolumeManipulator::Resize(resizeData, oldSize, newSize);

	object->SetBoxSize(newSize);
	GetTargetNode()->UpdateEffectGeometry();

	return (move);
}

void BoxShaftEffectManipulator::Update(void)
{
	if (!(GetManipulatorState() & kManipulatorUpdated))
	{
		SetBoxSize(GetObject()->GetBoxSize());
	}

	EffectManipulator::Update();
}


CylinderShaftEffectManipulator::CylinderShaftEffectManipulator(CylinderShaftEffect *cylinderShaftEffect) :
		EffectManipulator(cylinderShaftEffect, this, "WorldEditor/effect/CylinderShaft"),
		CylinderVolumeManipulator(cylinderShaftEffect, kEffectInteriorColor, kEffectOutlineColor, "WorldEditor/volume/Shaft")
{
}

CylinderShaftEffectManipulator::~CylinderShaftEffectManipulator()
{
}

const char *CylinderShaftEffectManipulator::GetDefaultNodeName(void) const
{
	return (TheWorldEditor->GetStringTable()->GetString(StringID('NAME', kNodeEffect, kEffectShaft, kShaftCylinder)));
}

Box3D CylinderShaftEffectManipulator::CalculateNodeBoundingBox(void) const
{
	const CylinderShaftEffectObject *object = GetObject();
	return (CylinderVolumeManipulator::CalculateBoundingBox(object->GetCylinderSize(), object->GetCylinderHeight()));
}

int32 CylinderShaftEffectManipulator::GetHandleTable(Point3D *handle) const
{
	const CylinderShaftEffectObject *object = GetObject();
	return (CylinderVolumeManipulator::GetHandleTable(object->GetCylinderSize(), object->GetCylinderHeight(), handle));
}

void CylinderShaftEffectManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	CylinderVolumeManipulator::GetHandleData(index, handleData);
}

bool CylinderShaftEffectManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	CylinderShaftEffectObject *object = GetObject();

	Vector2D newSize = object->GetCylinderSize();
	float newHeight = object->GetCylinderHeight();
	const Vector2D& oldSize = *reinterpret_cast<const Vector2D *>(GetOriginalSize());
	float oldHeight = GetOriginalSize()[2];
	bool move = CylinderVolumeManipulator::Resize(resizeData, oldSize, oldHeight, newSize, newHeight);

	object->SetCylinderSize(newSize);
	object->SetCylinderHeight(newHeight);
	GetTargetNode()->UpdateEffectGeometry();

	return (move);
}

void CylinderShaftEffectManipulator::Update(void)
{
	if (!(GetManipulatorState() & kManipulatorUpdated))
	{
		const CylinderShaftEffectObject *object = GetObject();
		SetCylinderSize(object->GetCylinderSize(), object->GetCylinderHeight());
	}

	EffectManipulator::Update();
}


TruncatedPyramidShaftEffectManipulator::TruncatedPyramidShaftEffectManipulator(TruncatedPyramidShaftEffect *truncatedPyramidShaftEffect) :
		EffectManipulator(truncatedPyramidShaftEffect, this, "WorldEditor/effect/TruncPyramidShaft"),
		TruncatedPyramidVolumeManipulator(truncatedPyramidShaftEffect, kEffectInteriorColor, kEffectOutlineColor, "WorldEditor/volume/Shaft")
{
}

TruncatedPyramidShaftEffectManipulator::~TruncatedPyramidShaftEffectManipulator()
{
}

const char *TruncatedPyramidShaftEffectManipulator::GetDefaultNodeName(void) const
{
	return (TheWorldEditor->GetStringTable()->GetString(StringID('NAME', kNodeEffect, kEffectShaft, kShaftTruncatedPyramid)));
}

Box3D TruncatedPyramidShaftEffectManipulator::CalculateNodeBoundingBox(void) const
{
	const TruncatedPyramidShaftEffectObject *object = GetObject();
	return (TruncatedPyramidVolumeManipulator::CalculateBoundingBox(object->GetPyramidSize(), object->GetPyramidHeight()));
}

int32 TruncatedPyramidShaftEffectManipulator::GetHandleTable(Point3D *handle) const
{
	const TruncatedPyramidShaftEffectObject *object = GetObject();
	return (TruncatedPyramidVolumeManipulator::GetHandleTable(object->GetPyramidSize(), object->GetPyramidHeight(), object->GetPyramidRatio(), handle));
}

void TruncatedPyramidShaftEffectManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	TruncatedPyramidVolumeManipulator::GetHandleData(index, handleData);
}

bool TruncatedPyramidShaftEffectManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	TruncatedPyramidShaftEffectObject *object = GetObject();

	Vector2D newSize = object->GetPyramidSize();
	float newHeight = object->GetPyramidHeight();
	float newRatio = object->GetPyramidRatio();
	const Vector2D& oldSize = *reinterpret_cast<const Vector2D *>(GetOriginalSize());
	float oldHeight = GetOriginalSize()[2];
	float oldRatio = GetOriginalSize()[3];
	bool move = TruncatedPyramidVolumeManipulator::Resize(resizeData, oldSize, oldHeight, oldRatio, newSize, newHeight, newRatio);

	object->SetPyramidSize(newSize);
	object->SetPyramidHeight(newHeight);
	object->SetPyramidRatio(newRatio);
	GetTargetNode()->UpdateEffectGeometry();

	return (move);
}

void TruncatedPyramidShaftEffectManipulator::Update(void)
{
	if (!(GetManipulatorState() & kManipulatorUpdated))
	{
		const TruncatedPyramidShaftEffectObject *object = GetObject();
		SetTruncatedPyramidSize(object->GetPyramidSize(), object->GetPyramidHeight(), object->GetPyramidRatio());
	}

	EffectManipulator::Update();
}


TruncatedConeShaftEffectManipulator::TruncatedConeShaftEffectManipulator(TruncatedConeShaftEffect *truncatedConeShaftEffect) :
		EffectManipulator(truncatedConeShaftEffect, this, "WorldEditor/effect/TruncConeShaft"),
		TruncatedConeVolumeManipulator(truncatedConeShaftEffect, kEffectInteriorColor, kEffectOutlineColor, "WorldEditor/volume/Shaft")
{
}

TruncatedConeShaftEffectManipulator::~TruncatedConeShaftEffectManipulator()
{
}

const char *TruncatedConeShaftEffectManipulator::GetDefaultNodeName(void) const
{
	return (TheWorldEditor->GetStringTable()->GetString(StringID('NAME', kNodeEffect, kEffectShaft, kShaftTruncatedCone)));
}

Box3D TruncatedConeShaftEffectManipulator::CalculateNodeBoundingBox(void) const
{
	const TruncatedConeShaftEffectObject *object = GetObject();
	return (TruncatedConeVolumeManipulator::CalculateBoundingBox(object->GetConeSize(), object->GetConeHeight()));
}

int32 TruncatedConeShaftEffectManipulator::GetHandleTable(Point3D *handle) const
{
	const TruncatedConeShaftEffectObject *object = GetObject();
	return (TruncatedConeVolumeManipulator::GetHandleTable(object->GetConeSize(), object->GetConeHeight(), object->GetConeRatio(), handle));
}

void TruncatedConeShaftEffectManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	TruncatedConeVolumeManipulator::GetHandleData(index, handleData);
}

bool TruncatedConeShaftEffectManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	TruncatedConeShaftEffectObject *object = GetObject();

	Vector2D newSize = object->GetConeSize();
	float newHeight = object->GetConeHeight();
	float newRatio = object->GetConeRatio();
	const Vector2D& oldSize = *reinterpret_cast<const Vector2D *>(GetOriginalSize());
	float oldHeight = GetOriginalSize()[2];
	float oldRatio = GetOriginalSize()[3];
	bool move = TruncatedConeVolumeManipulator::Resize(resizeData, oldSize, oldHeight, oldRatio, newSize, newHeight, newRatio);

	object->SetConeSize(newSize);
	object->SetConeHeight(newHeight);
	object->SetConeRatio(newRatio);
	GetTargetNode()->UpdateEffectGeometry();

	return (move);
}

void TruncatedConeShaftEffectManipulator::Update(void)
{
	if (!(GetManipulatorState() & kManipulatorUpdated))
	{
		const TruncatedConeShaftEffectObject *object = GetObject();
		SetTruncatedConeSize(object->GetConeSize(), object->GetConeHeight(), object->GetConeRatio());
	}

	EffectManipulator::Update();
}


QuadEffectManipulator::QuadEffectManipulator(QuadEffect *quadEffect) :
		EffectManipulator(quadEffect, this, "WorldEditor/effect/Quad"),
		SphereVolumeManipulator(quadEffect, kEffectInteriorColor, kEffectOutlineColor, "WorldEditor/volume/Quad")
{
}

QuadEffectManipulator::~QuadEffectManipulator()
{
}

Box3D QuadEffectManipulator::CalculateNodeBoundingBox(void) const
{
	return (SphereVolumeManipulator::CalculateBoundingBox(GetObject()->GetQuadRadius()));
}

int32 QuadEffectManipulator::GetHandleTable(Point3D *handle) const
{
	return (SphereVolumeManipulator::GetHandleTable(GetObject()->GetQuadRadius(), handle));
}

void QuadEffectManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	SphereVolumeManipulator::GetCircularHandleData(index, handleData);
}

bool QuadEffectManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	QuadEffectObject *object = GetObject();

	float newRadius = object->GetQuadRadius();
	float oldRadius = GetOriginalSize()[0];
	SphereVolumeManipulator::Resize(resizeData, oldRadius, newRadius);

	object->SetQuadRadius(newRadius);
	GetTargetNode()->UpdateEffectGeometry();

	return (false);
}

void QuadEffectManipulator::Update(void)
{
	if (!(GetManipulatorState() & kManipulatorUpdated))
	{
		SetSphereSize(GetObject()->GetQuadRadius());
	}

	EffectManipulator::Update();
}


FlareEffectManipulator::FlareEffectManipulator(FlareEffect *flareEffect) :
		EffectManipulator(flareEffect, this, "WorldEditor/effect/Flare"),
		SphereVolumeManipulator(flareEffect, kEffectInteriorColor, kEffectOutlineColor, "WorldEditor/volume/Flare")
{
}

FlareEffectManipulator::~FlareEffectManipulator()
{
}

Box3D FlareEffectManipulator::CalculateNodeBoundingBox(void) const
{
	return (SphereVolumeManipulator::CalculateBoundingBox(GetObject()->GetFlareRadius()));
}

int32 FlareEffectManipulator::GetHandleTable(Point3D *handle) const
{
	return (SphereVolumeManipulator::GetHandleTable(GetObject()->GetFlareRadius(), handle));
}

void FlareEffectManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	SphereVolumeManipulator::GetCircularHandleData(index, handleData);
}

bool FlareEffectManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	FlareEffectObject *object = GetObject();

	float newRadius = object->GetFlareRadius();
	float oldRadius = GetOriginalSize()[0];
	SphereVolumeManipulator::Resize(resizeData, oldRadius, newRadius);

	object->SetFlareRadius(newRadius);
	GetTargetNode()->UpdateEffectGeometry();

	return (false);
}

void FlareEffectManipulator::Update(void)
{
	if (!(GetManipulatorState() & kManipulatorUpdated))
	{
		SetSphereSize(GetObject()->GetFlareRadius());
	}

	EffectManipulator::Update();
}


BeamEffectManipulator::BeamEffectManipulator(BeamEffect *beamEffect) :
		EffectManipulator(beamEffect, this, "WorldEditor/effect/Beam"),
		CylinderVolumeManipulator(beamEffect, kEffectInteriorColor, kEffectOutlineColor, "WorldEditor/volume/Beam")
{
}

BeamEffectManipulator::~BeamEffectManipulator()
{
}

Box3D BeamEffectManipulator::CalculateNodeBoundingBox(void) const
{
	const BeamEffectObject *object = GetObject();
	return (CylinderVolumeManipulator::CalculateBoundingBox(object->GetBeamRadius(), object->GetBeamHeight()));
}

int32 BeamEffectManipulator::GetHandleTable(Point3D *handle) const
{
	const BeamEffectObject *object = GetObject();
	return (CylinderVolumeManipulator::GetHandleTable(object->GetBeamRadius(), object->GetBeamHeight(), handle));
}

void BeamEffectManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	CylinderVolumeManipulator::GetHandleData(index, handleData);
}

bool BeamEffectManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	BeamEffectObject *object = GetObject();

	float newRadius = object->GetBeamRadius();
	float newHeight = object->GetBeamHeight();
	float oldRadius = GetOriginalSize()[0];
	float oldHeight = GetOriginalSize()[1];
	bool move = CylinderVolumeManipulator::Resize(resizeData, oldRadius, oldHeight, newRadius, newHeight);

	object->SetBeamRadius(newRadius);
	object->SetBeamHeight(newHeight);
	GetTargetNode()->UpdateEffectGeometry();

	return (move);
}

void BeamEffectManipulator::Update(void)
{
	if (!(GetManipulatorState() & kManipulatorUpdated))
	{
		const BeamEffectObject *object = GetObject();
		SetCylinderSize(object->GetBeamRadius(), object->GetBeamHeight());
	}

	EffectManipulator::Update();
}


TubeEffectManipulator::TubeEffectManipulator(TubeEffect *tubeEffect) :
		EffectManipulator(tubeEffect, this, "WorldEditor/effect/Tube"),
		DiskVolumeManipulator(tubeEffect, kEffectInteriorColor, kEffectOutlineColor, "WorldEditor/volume/Tube")
{
}

TubeEffectManipulator::~TubeEffectManipulator()
{
}

Box3D TubeEffectManipulator::CalculateNodeBoundingBox(void) const
{
	const TubeEffectObject *object = GetObject();
	float radius = object->GetTubeRadius();
	const Box3D& box = object->GetPathBoundingBox();
	return (Box3D(Point3D(box.min.x - radius, box.min.y - radius, box.min.z - radius), Point3D(box.max.x + radius, box.max.y + radius, box.max.z + radius)));
}

int32 TubeEffectManipulator::GetHandleTable(Point3D *handle) const
{
	return (DiskVolumeManipulator::GetHandleTable(GetObject()->GetTubeRadius(), handle));
}

void TubeEffectManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	DiskVolumeManipulator::GetHandleData(index, handleData);
}

bool TubeEffectManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	TubeEffectObject *object = GetObject();

	float newRadius = object->GetTubeRadius();
	float oldRadius = GetOriginalSize()[0];
	bool move = DiskVolumeManipulator::Resize(resizeData, oldRadius, newRadius);

	object->SetTubeRadius(newRadius);
	object->Build();

	return (move);
}

void TubeEffectManipulator::Update(void)
{
	if (!(GetManipulatorState() & kManipulatorUpdated))
	{
		SetDiskSize(GetObject()->GetTubeRadius());
	}

	EffectManipulator::Update();
}


FireEffectManipulator::FireEffectManipulator(FireEffect *fireEffect) :
		EffectManipulator(fireEffect, this, "WorldEditor/effect/Fire"),
		CylinderVolumeManipulator(fireEffect, kEffectInteriorColor, kEffectOutlineColor, "WorldEditor/volume/Fire")
{
}

FireEffectManipulator::~FireEffectManipulator()
{
}

Box3D FireEffectManipulator::CalculateNodeBoundingBox(void) const
{
	const FireEffectObject *object = GetObject();
	return (CylinderVolumeManipulator::CalculateBoundingBox(object->GetFireRadius(), object->GetFireHeight()));
}

int32 FireEffectManipulator::GetHandleTable(Point3D *handle) const
{
	const FireEffectObject *object = GetObject();
	return (CylinderVolumeManipulator::GetHandleTable(object->GetFireRadius(), object->GetFireHeight(), handle));
}

void FireEffectManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	CylinderVolumeManipulator::GetHandleData(index, handleData);
}

bool FireEffectManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	FireEffectObject *object = GetObject();

	float newRadius = object->GetFireRadius();
	float newHeight = object->GetFireHeight();
	float oldRadius = GetOriginalSize()[0];
	float oldHeight = GetOriginalSize()[1];
	bool move = CylinderVolumeManipulator::Resize(resizeData, oldRadius, oldHeight, newRadius, newHeight);

	object->SetFireRadius(newRadius);
	object->SetFireHeight(newHeight);
	return (move);
}

void FireEffectManipulator::Update(void)
{
	if (!(GetManipulatorState() & kManipulatorUpdated))
	{
		const FireEffectObject *object = GetObject();
		SetCylinderSize(object->GetFireRadius(), object->GetFireHeight());
	}

	EffectManipulator::Update();
}


PanelEffectManipulator::PanelEffectManipulator(PanelEffect *panelEffect) :
		EffectManipulator(panelEffect, this, "WorldEditor/effect/Panel"),
		PlateVolumeManipulator(panelEffect, kEffectInteriorColor, kEffectOutlineColor, "WorldEditor/volume/Panel")
{
	SetManipulatorFlags(kManipulatorLockedController);
}

PanelEffectManipulator::~PanelEffectManipulator()
{
}

Box3D PanelEffectManipulator::CalculateNodeBoundingBox(void) const
{
	return (PlateVolumeManipulator::CalculateBoundingBox(GetObject()->GetExternalPanelSize()));
}

int32 PanelEffectManipulator::GetHandleTable(Point3D *handle) const
{
	return (PlateVolumeManipulator::GetHandleTable(GetObject()->GetExternalPanelSize(), handle));
}

void PanelEffectManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	PlateVolumeManipulator::GetHandleData(index, handleData);
}

bool PanelEffectManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	PanelEffectObject *object = GetObject();

	Vector2D newSize = object->GetExternalPanelSize();
	const Vector2D& oldSize = *reinterpret_cast<const Vector2D *>(GetOriginalSize());
	bool move = PlateVolumeManipulator::Resize(resizeData, oldSize, newSize);

	object->SetExternalPanelSize(newSize);
	return (move);
}

void PanelEffectManipulator::Update(void)
{
	if (!(GetManipulatorState() & kManipulatorUpdated))
	{
		SetPlateSize(GetObject()->GetExternalPanelSize());
	}

	EffectManipulator::Update();
}

// ZYUQURM
