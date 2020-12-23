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


#ifndef MGHornetGun_h
#define MGHornetGun_h


#include "MGWeapons.h"


namespace C4
{
	enum : ControllerType
	{
		kControllerHornetGun	= 'hngn'
	};


	enum : ModelType
	{
		kModelHornetGun			= 'hngn'
	};


	class HornetGun final : public Weapon
	{
		private:

			ModelRegistration		hornetGunModelRegistration;

			HornetGun();
			~HornetGun();

		public:

			static void Construct(void);
			static void Destruct(void);

			WeaponController *NewWeaponController(FighterController *fighter) const;
	};


	class HornetGunController final : public WeaponController
	{
		public:

			HornetGunController(FighterController *fighter);
			~HornetGunController();

			void Preprocess(void) override;

			WeaponResult UpdateWeapon(const Point3D& position, const Vector3D& direction, const Point3D& center) override;
	};
}


#endif

// ZYUQURM
