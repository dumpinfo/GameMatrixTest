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


#ifndef MGPistol_h
#define MGPistol_h


#include "C4Lights.h"
#include "C4Effects.h"
#include "MGInput.h"
#include "MGWeapons.h"
#include "MGMultiplayer.h"


namespace C4
{
	enum : ControllerType
	{
		kControllerPistol		= 'pist'
	};


	enum : ModelType
	{
		kModelPistol			= 'pist',
		kModelBulletAmmo		= 'blam'
	};


	enum : MaterialType
	{
		kMaterialBulletHoleStone	= 'bstn',
		kMaterialBulletHoleWood		= 'bwod',
		kMaterialBulletHoleMetal	= 'bmtl',
		kMaterialBulletHoleGlass	= 'bgls'
	};


	class Pistol final : public Weapon
	{
		private:

			ModelRegistration		pistolModelRegistration;
			ModelRegistration		bulletAmmoModelRegistration;

			MaterialRegistration	stoneBulletHoleMaterialRegistration;
			MaterialRegistration	woodBulletHoleMaterialRegistration;
			MaterialRegistration	metalBulletHoleMaterialRegistration;
			MaterialRegistration	glassBulletHoleMaterialRegistration;

			WeaponAction			pistolAction;

			Pistol();
			~Pistol();

		public:

			static void Construct(void);
			static void Destruct(void);

			WeaponController *NewWeaponController(FighterController *fighter) const;
	};


	class PistolController final : public WeaponController
	{
		private:

			Light				*muzzleLight;
			BeamEffect			*muzzleFlashBeam;
			QuadEffect			*muzzleFlashQuad;

			int32				flashTime;

			FrameAnimator		frameAnimator;

		public:

			enum
			{
				kPistolMessageFire = kWeaponMessageBaseCount,
				kPistolMessageImpact
			};

			PistolController(FighterController *fighter);
			~PistolController();

			void Preprocess(void) override;

			ControllerMessage *CreateMessage(ControllerMessageType type) const override;

			void Move(void) override;

			void ShowFireAnimation(void);

			WeaponResult UpdateWeapon(const Point3D& position, const Vector3D& direction, const Point3D& center) override;
	};


	class PistolFireMessage : public ControllerMessage
	{
		friend class PistolController;

		private: 

			Point3D		firePosition;
 
			PistolFireMessage(int32 index);
 
		public:

			PistolFireMessage(int32 index, const Point3D& position); 
			~PistolFireMessage();
 
			void Compress(Compressor& data) const override; 
			bool Decompress(Decompressor& data) override;

			void HandleControllerMessage(Controller *controller) const;
	}; 


	class PistolImpactMessage : public ControllerMessage
	{
		friend class PistolController;

		private:

			Point3D			impactPosition;
			Vector3D		impactNormal;
			SubstanceType	impactSubstance;

			PistolImpactMessage(int32 index);

		public:

			PistolImpactMessage(int32 index, const Point3D& position, const Vector3D& normal, SubstanceType substance);
			~PistolImpactMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			void HandleControllerMessage(Controller *controller) const;
	};
}


#endif

// ZYUQURM
