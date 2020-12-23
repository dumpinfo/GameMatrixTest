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


#ifndef MGCameras_h
#define MGCameras_h


#include "C4Cameras.h"
#include "C4Models.h"
#include "MGBase.h"


namespace C4
{
	extern const float kCameraPositionHeight;


	class ModelCamera : public FrustumCamera
	{
		private:

			Model		*targetModel;

		protected:

			ModelCamera();

		public:

			~ModelCamera();

			Model *GetTargetModel(void) const
			{
				return (targetModel);
			}

			void SetTargetModel(Model *model)
			{
				targetModel = model;
			}
	};


	class FirstPersonCamera final : public ModelCamera
	{
		public:

			FirstPersonCamera();
			~FirstPersonCamera();

			void Move(void) override;
	};


	class ChaseCamera final : public ModelCamera
	{
		public:

			ChaseCamera();
			~ChaseCamera();

			void Move(void) override;
	};
}


#endif

// ZYUQURM
