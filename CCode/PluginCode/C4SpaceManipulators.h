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


#ifndef C4SpaceManipulators_h
#define C4SpaceManipulators_h


#include "C4EditorManipulators.h"
#include "C4VolumeManipulators.h"
#include "C4EditorOperations.h"
#include "C4Spaces.h"


namespace C4
{
	class SpaceManipulator : public EditorManipulator
	{
		friend class EditorManipulator;

		private:

			VolumeManipulator	*volumeManipulator;

			static Manipulator *Create(Space *space);

		protected:

			SpaceManipulator(Space *space, VolumeManipulator *volume, const char *iconName);

			void UpdateZoneConnectors(void) const;

		public:

			~SpaceManipulator();

			Space *GetTargetNode(void) const
			{
				return (static_cast<Space *>(EditorManipulator::GetTargetNode()));
			}

			SpaceObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			const char *GetDefaultNodeName(void) const override;

			void Select(void) override;
			void Unselect(void) override;

			bool Pick(const Ray *ray, PickData *data) const override;
			bool RegionPick(const VisibilityRegion *region) const override;

			void Render(const ManipulatorRenderData *renderData) override;
	};


	class FogSpaceManipulator : public SpaceManipulator, public PlateVolumeManipulator
	{
		private:

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			FogSpaceManipulator(FogSpace *fog);
			~FogSpaceManipulator();

			FogSpace *GetTargetNode(void) const
			{
				return (static_cast<FogSpace *>(EditorManipulator::GetTargetNode()));
			}

			FogSpaceObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			Box3D CalculateNodeBoundingBox(void) const override;

			bool Resize(const ManipulatorResizeData *resizeData) override;
			void Update(void) override;
	};


	class ShadowSpaceManipulator : public SpaceManipulator, public BoxVolumeManipulator
	{
		private:

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			ShadowSpaceManipulator(ShadowSpace *shadow);
			~ShadowSpaceManipulator();

			ShadowSpace *GetTargetNode(void) const
			{
				return (static_cast<ShadowSpace *>(EditorManipulator::GetTargetNode()));
			}

			ShadowSpaceObject *GetObject(void) const 
			{
				return (GetTargetNode()->GetObject());
			} 

			Box3D CalculateNodeBoundingBox(void) const override; 

			bool Resize(const ManipulatorResizeData *resizeData) override;
			void Update(void) override; 
	};
 
 
	class RadiositySpaceManipulator : public SpaceManipulator, public BoxVolumeManipulator
	{
		private:
 
			VertexBuffer		gridVertexBuffer;
			VertexBuffer		gridIndexBuffer;

			List<Attribute>		gridAttributeList;
			DiffuseAttribute	gridDiffuseColor;
			Renderable			gridRenderable;

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			RadiositySpaceManipulator(RadiositySpace *radiosity);
			~RadiositySpaceManipulator();

			RadiositySpace *GetTargetNode(void) const
			{
				return (static_cast<RadiositySpace *>(EditorManipulator::GetTargetNode()));
			}

			RadiositySpaceObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			void Preprocess(void) override;

			Box3D CalculateNodeBoundingBox(void) const override;

			bool Resize(const ManipulatorResizeData *resizeData) override;
			void Update(void) override;

			void HandleSettingsUpdate(void) override;

			void Render(const ManipulatorRenderData *renderData) override;
	};


	class AcousticsSpaceManipulator : public SpaceManipulator, public BoxVolumeManipulator
	{
		private:

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			AcousticsSpaceManipulator(AcousticsSpace *acoustics);
			~AcousticsSpaceManipulator();

			AcousticsSpace *GetTargetNode(void) const
			{
				return (static_cast<AcousticsSpace *>(EditorManipulator::GetTargetNode()));
			}

			AcousticsSpaceObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			void Preprocess(void) override;

			Box3D CalculateNodeBoundingBox(void) const override;

			bool Resize(const ManipulatorResizeData *resizeData) override;
			void Update(void) override;
	};


	class OcclusionSpaceManipulator : public SpaceManipulator, public BoxVolumeManipulator
	{
		private:

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			OcclusionSpaceManipulator(OcclusionSpace *occlusion);
			~OcclusionSpaceManipulator();

			OcclusionSpace *GetTargetNode(void) const
			{
				return (static_cast<OcclusionSpace *>(EditorManipulator::GetTargetNode()));
			}

			OcclusionSpaceObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			Box3D CalculateNodeBoundingBox(void) const override;

			bool Resize(const ManipulatorResizeData *resizeData) override;
			void Update(void) override;
	};


	class PaintSpaceManipulator : public SpaceManipulator, public BoxVolumeManipulator
	{
		private:

			List<NodeReference>		undoGeometryList;

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			PaintSpaceManipulator(PaintSpace *shadow);
			~PaintSpaceManipulator();

			PaintSpace *GetTargetNode(void) const
			{
				return (static_cast<PaintSpace *>(EditorManipulator::GetTargetNode()));
			}

			PaintSpaceObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			Box3D CalculateNodeBoundingBox(void) const override;

			bool Resize(const ManipulatorResizeData *resizeData) override;
			void Update(void) override;

			void HandleDelete(bool undoable) override;
			void HandleUndelete(void) override;
			void HandleSettingsUpdate(void) override;
	};


	class PhysicsSpaceManipulator : public SpaceManipulator, public BoxVolumeManipulator
	{
		private:

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			PhysicsSpaceManipulator(PhysicsSpace *physics);
			~PhysicsSpaceManipulator();

			PhysicsSpace *GetTargetNode(void) const
			{
				return (static_cast<PhysicsSpace *>(EditorManipulator::GetTargetNode()));
			}

			PhysicsSpaceObject *GetObject(void) const
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
