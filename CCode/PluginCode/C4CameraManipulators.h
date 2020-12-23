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


#ifndef C4CameraManipulators_h
#define C4CameraManipulators_h


#include "C4EditorManipulators.h"
#include "C4VolumeManipulators.h"
#include "C4Cameras.h"


namespace C4
{
	class CameraManipulator : public EditorManipulator
	{
		friend class EditorManipulator;

		private:

			VolumeManipulator	*volumeManipulator;

			static Manipulator *Create(Camera *camera);

		public:

			CameraManipulator(Camera *camera, VolumeManipulator *volume);
			~CameraManipulator();

			Camera *GetTargetNode(void) const
			{
				return (static_cast<Camera *>(EditorManipulator::GetTargetNode()));
			}

			const char *GetDefaultNodeName(void) const override;

			void Preprocess(void) override;

			void Select(void) override;
			void Unselect(void) override;

			void Render(const ManipulatorRenderData *renderData) override;
	};


	class FrustumCameraManipulator : public CameraManipulator, public ProjectionVolumeManipulator
	{
		private:

			bool CalculateNodeSphere(BoundingSphere *sphere) const;

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			FrustumCameraManipulator(FrustumCamera *frustumCamera);
			~FrustumCameraManipulator();

			FrustumCamera *GetTargetNode(void) const
			{
				return (static_cast<FrustumCamera *>(EditorManipulator::GetTargetNode()));
			}

			FrustumCameraObject *GetObject(void) const
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
