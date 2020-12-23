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


#ifndef MGGusGraves_h
#define MGGusGraves_h


#include "MGFighter.h"


namespace C4
{
	enum : ControllerType
	{
		kControllerGusGraves	= 'ggrv'
	};


	enum : ModelType
	{
		kModelGusGraves			= 'ggrv'
	};


	class GusGravesController;


	class GusGraves
	{
		private:

			ControllerReg<GusGravesController>	gusGravesControllerRegistration;
			ModelRegistration					gusGravesModelRegistration;

			Texture								*flashlightTexture;

			GusGraves();
			~GusGraves();

		public:

			static void Construct(void);
			static void Destruct(void);
	};


	class GusGravesController final : public FighterController
	{
		private:

			SpineTwistAnimator		*spineTwistAnimator;

		public:

			GusGravesController();
			~GusGravesController();

			void Preprocess(void) override;
			void SendInitialStateMessages(Player *player) const override;

			void SetFighterMotion(int32 motion);

			void Animate(void);
	};


	class CreateGusGravesMessage : public CreateFighterMessage
	{
		friend class CreateModelMessage;

		private:

			CreateGusGravesMessage();

		public:

			CreateGusGravesMessage(int32 fighterIndex, const Point3D& position, float azm, float alt, unsigned_int32 movement, int32 weapon, int32 weaponIndex, int32 key);
			~CreateGusGravesMessage();

			bool HandleMessage(Player *sender) const override;
	};
}


#endif

// ZYUQURM
