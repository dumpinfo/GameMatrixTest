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


#ifndef MGCatFlinger_h
#define MGCatFlinger_h


#include "MGWeapons.h"


namespace C4
{
	enum : ControllerType
	{
		kControllerCatFlinger		= 'catf'
	};


	enum : ModelType
	{
		kModelCatFlinger			= 'catf'
	};


	class CatFlinger final : public Weapon
	{
		private:

			ModelRegistration		catFlingerModelRegistration;

			CatFlinger();
			~CatFlinger();

		public:

			static void Construct(void);
			static void Destruct(void);

			WeaponController *NewWeaponController(FighterController *fighter) const;
	};


	class CatFlingerController final : public WeaponController
	{
		public:

			CatFlingerController(FighterController *fighter);
			~CatFlingerController();

			void Preprocess(void) override;

			WeaponResult UpdateWeapon(const Point3D& position, const Vector3D& direction, const Point3D& center) override;
	};
}


#endif

// ZYUQURM
