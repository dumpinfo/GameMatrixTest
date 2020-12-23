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


#ifndef MGChickenBlaster_h
#define MGChickenBlaster_h


#include "MGWeapons.h"


namespace C4
{
	enum : ControllerType
	{
		kControllerChickenBlaster	= 'chbl'
	};


	enum : ModelType
	{
		kModelChickenBlaster		= 'chbl'
	};


	class ChickenBlaster final : public Weapon
	{
		private:

			ModelRegistration		chickenBlasterModelRegistration;

			ChickenBlaster();
			~ChickenBlaster();

		public:

			static void Construct(void);
			static void Destruct(void);

			WeaponController *NewWeaponController(FighterController *fighter) const;
	};


	class ChickenBlasterController final : public WeaponController
	{
		public:

			ChickenBlasterController(FighterController *fighter);
			~ChickenBlasterController();

			void Preprocess(void) override;

			WeaponResult UpdateWeapon(const Point3D& position, const Vector3D& direction, const Point3D& center) override;
	};
}


#endif

// ZYUQURM
