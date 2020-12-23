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


#ifndef MGSoldier_h
#define MGSoldier_h


#include "MGFighter.h"


namespace C4
{
	enum : ControllerType
	{
		kControllerSoldier		= 'sold'
	};


	enum : ModelType
	{
		kModelSoldier			= 'sold'
	};


	enum
	{
		kSoldierArmorTextureCount	= 25,
		kSoldierHelmetTypeCount		= 10
	};


	class SoldierController;


	class Soldier
	{
		private:

			ControllerReg<SoldierController>	soldierControllerRegistration;
			ModelRegistration					soldierModelRegistration;

			Soldier();
			~Soldier();

		public:

			static void Construct(void);
			static void Destruct(void);
	};


	class SoldierController final : public FighterController
	{
		private:

			SpineTwistAnimator			*spineTwistAnimator;

			Node						*headBone;
			Node						*hairNode;

			RenderSegment				*armorSegment;
			Model						*helmetModel;

			List<Attribute>				armorAttributeList;
			EmissionAttribute			armorEmissionAttribute;
			DiffuseAttribute			armorDiffuseAttribute;
			DiffuseTextureAttribute		armorTextureAttribute;

			List<Attribute>				helmetAttributeList;
			EmissionAttribute			helmetEmissionAttribute;
			DiffuseAttribute			helmetDiffuseAttribute;

		public:

			SoldierController();
			~SoldierController();

			void Preprocess(void) override;
			void SendInitialStateMessages(Player *player) const override;

			void SetFighterStyle(const int32 *style, bool prep = true);
			void SetFighterMotion(int32 motion);

			void Animate(void);
	};


	class CreateSoldierMessage : public CreateFighterMessage
	{
		friend class CreateModelMessage;

		private:

			CreateSoldierMessage();

		public:

			CreateSoldierMessage(int32 fighterIndex, const Point3D& position, float azm, float alt, unsigned_int32 movement, int32 weapon, int32 weaponIndex, int32 key);
			~CreateSoldierMessage();

			bool HandleMessage(Player *sender) const override;
	};
}
 

#endif

// ZYUQURM
