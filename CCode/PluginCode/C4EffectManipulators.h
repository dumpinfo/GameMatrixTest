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


#ifndef C4EffectManipulators_h
#define C4EffectManipulators_h


#include "C4EditorManipulators.h"
#include "C4VolumeManipulators.h"
#include "C4Particles.h"
#include "C4Shafts.h"
#include "C4Panels.h"


namespace C4
{
	class EffectManipulator : public EditorManipulator
	{
		friend class EditorManipulator;

		private:

			VolumeManipulator	*volumeManipulator;

			static Manipulator *Create(Effect *effect);

		protected:

			EffectManipulator(Effect *effect, VolumeManipulator *volume, const char *iconName);

		public:

			~EffectManipulator();

			Effect *GetTargetNode(void) const
			{
				return (static_cast<Effect *>(EditorManipulator::GetTargetNode()));
			}

			const char *GetDefaultNodeName(void) const override;

			void Invalidate(void) override;

			void Select(void) override;
			void Unselect(void) override;

			void HandleSizeUpdate(int32 count, const float *size) override;

			void InvalidateShaderData(void) override;

			bool Pick(const Ray *ray, PickData *data) const override;
			bool RegionPick(const VisibilityRegion *region) const override;

			void Render(const ManipulatorRenderData *renderData) override;
	};


	class ParticleSystemManipulator : public EditorManipulator
	{
		public:

			ParticleSystemManipulator(ParticleSystem *particleSystem);
			~ParticleSystemManipulator();

			ParticleSystem *GetTargetNode(void) const
			{
				return (static_cast<ParticleSystem *>(EditorManipulator::GetTargetNode()));
			}

			const char *GetDefaultNodeName(void) const override;

			void Preprocess(void) override;

			bool MaterialSettable(void) const override;
			bool MaterialRemovable(void) const override;
			int32 GetMaterialCount(void) const override;
			MaterialObject *GetMaterial(int32 index) const override;
			void SetMaterial(MaterialObject *material) override;
			void ReplaceMaterial(MaterialObject *oldMaterial, MaterialObject *newMaterial) override;
			void RemoveMaterial(void) override;
			void InvalidateShaderData(void) override;
	};


	class BoxShaftEffectManipulator : public EffectManipulator, public BoxVolumeManipulator
	{
		private:

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			BoxShaftEffectManipulator(BoxShaftEffect *boxShaftEffect);
			~BoxShaftEffectManipulator();

			BoxShaftEffect *GetTargetNode(void) const
			{
				return (static_cast<BoxShaftEffect *>(EditorManipulator::GetTargetNode()));
			}

			BoxShaftEffectObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject()); 
			}

			const char *GetDefaultNodeName(void) const override; 

			Box3D CalculateNodeBoundingBox(void) const override; 

			bool Resize(const ManipulatorResizeData *resizeData) override;
			void Update(void) override; 
	};
 
 
	class CylinderShaftEffectManipulator : public EffectManipulator, public CylinderVolumeManipulator
	{
		private:
 
			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			CylinderShaftEffectManipulator(CylinderShaftEffect *cylinderShaftEffect);
			~CylinderShaftEffectManipulator();

			CylinderShaftEffect *GetTargetNode(void) const
			{
				return (static_cast<CylinderShaftEffect *>(EditorManipulator::GetTargetNode()));
			}

			CylinderShaftEffectObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			const char *GetDefaultNodeName(void) const override;

			Box3D CalculateNodeBoundingBox(void) const override;

			bool Resize(const ManipulatorResizeData *resizeData) override;
			void Update(void) override;
	};


	class TruncatedPyramidShaftEffectManipulator : public EffectManipulator, public TruncatedPyramidVolumeManipulator
	{
		private:

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			TruncatedPyramidShaftEffectManipulator(TruncatedPyramidShaftEffect *truncatedPyramidShaftEffect);
			~TruncatedPyramidShaftEffectManipulator();

			TruncatedPyramidShaftEffect *GetTargetNode(void) const
			{
				return (static_cast<TruncatedPyramidShaftEffect *>(EditorManipulator::GetTargetNode()));
			}

			TruncatedPyramidShaftEffectObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			const char *GetDefaultNodeName(void) const override;

			Box3D CalculateNodeBoundingBox(void) const override;

			bool Resize(const ManipulatorResizeData *resizeData) override;
			void Update(void) override;
	};


	class TruncatedConeShaftEffectManipulator : public EffectManipulator, public TruncatedConeVolumeManipulator
	{
		private:

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			TruncatedConeShaftEffectManipulator(TruncatedConeShaftEffect *truncatedConeShaftEffect);
			~TruncatedConeShaftEffectManipulator();

			TruncatedConeShaftEffect *GetTargetNode(void) const
			{
				return (static_cast<TruncatedConeShaftEffect *>(EditorManipulator::GetTargetNode()));
			}

			TruncatedConeShaftEffectObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			const char *GetDefaultNodeName(void) const override;

			Box3D CalculateNodeBoundingBox(void) const override;

			bool Resize(const ManipulatorResizeData *resizeData) override;
			void Update(void) override;
	};


	class QuadEffectManipulator : public EffectManipulator, public SphereVolumeManipulator
	{
		private:

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			QuadEffectManipulator(QuadEffect *quadEffect);
			~QuadEffectManipulator();

			QuadEffect *GetTargetNode(void) const
			{
				return (static_cast<QuadEffect *>(EditorManipulator::GetTargetNode()));
			}

			QuadEffectObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			Box3D CalculateNodeBoundingBox(void) const override;

			bool Resize(const ManipulatorResizeData *resizeData) override;
			void Update(void) override;
	};


	class FlareEffectManipulator : public EffectManipulator, public SphereVolumeManipulator
	{
		private:

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			FlareEffectManipulator(FlareEffect *flareEffect);
			~FlareEffectManipulator();

			FlareEffect *GetTargetNode(void) const
			{
				return (static_cast<FlareEffect *>(EditorManipulator::GetTargetNode()));
			}

			FlareEffectObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			Box3D CalculateNodeBoundingBox(void) const override;

			bool Resize(const ManipulatorResizeData *resizeData) override;
			void Update(void) override;
	};


	class BeamEffectManipulator : public EffectManipulator, public CylinderVolumeManipulator
	{
		private:

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			BeamEffectManipulator(BeamEffect *beamEffect);
			~BeamEffectManipulator();

			BeamEffect *GetTargetNode(void) const
			{
				return (static_cast<BeamEffect *>(EditorManipulator::GetTargetNode()));
			}

			BeamEffectObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			Box3D CalculateNodeBoundingBox(void) const override;

			bool Resize(const ManipulatorResizeData *resizeData) override;
			void Update(void) override;
	};


	class TubeEffectManipulator : public EffectManipulator, public DiskVolumeManipulator
	{
		private:

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			TubeEffectManipulator(TubeEffect *tubeEffect);
			~TubeEffectManipulator();

			TubeEffect *GetTargetNode(void) const
			{
				return (static_cast<TubeEffect *>(EditorManipulator::GetTargetNode()));
			}

			TubeEffectObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			Box3D CalculateNodeBoundingBox(void) const override;

			bool Resize(const ManipulatorResizeData *resizeData) override;
			void Update(void) override;
	};


	class FireEffectManipulator : public EffectManipulator, public CylinderVolumeManipulator
	{
		private:

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			FireEffectManipulator(FireEffect *fireEffect);
			~FireEffectManipulator();

			FireEffect *GetTargetNode(void) const
			{
				return (static_cast<FireEffect *>(EditorManipulator::GetTargetNode()));
			}

			FireEffectObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			Box3D CalculateNodeBoundingBox(void) const override;

			bool Resize(const ManipulatorResizeData *resizeData) override;
			void Update(void) override;
	};


	class PanelEffectManipulator : public EffectManipulator, public PlateVolumeManipulator
	{
		private:

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			PanelEffectManipulator(PanelEffect *panelEffect);
			~PanelEffectManipulator();

			PanelEffect *GetTargetNode(void) const
			{
				return (static_cast<PanelEffect *>(EditorManipulator::GetTargetNode()));
			}

			PanelEffectObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			Box3D CalculateNodeBoundingBox(void) const override;

			bool Resize(const ManipulatorResizeData *resizeData) override;
			void Update(void) override;
	};
}


#endif

// ZYUQURM
