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


#include "MGChickenBlaster.h"
#include "MGGame.h"


using namespace C4;


namespace
{
	Storage<ChickenBlaster> chickenBlasterStorage;
}


ChickenBlaster::ChickenBlaster() :
		Weapon(kWeaponChickenBlaster, 0, kModelChickenBlaster, 0, 0, 1),
		chickenBlasterModelRegistration(kModelChickenBlaster, TheGame->GetStringTable()->GetString(StringID('MODL', 'WEAP', kModelChickenBlaster)), "chicken/gun/ChickenBlaster", 0, kControllerCollectable)
{
}

ChickenBlaster::~ChickenBlaster()
{
}

void ChickenBlaster::Construct(void)
{
	new(chickenBlasterStorage) ChickenBlaster;
}

void ChickenBlaster::Destruct(void)
{
	chickenBlasterStorage->~ChickenBlaster();
}

WeaponController *ChickenBlaster::NewWeaponController(FighterController *fighter) const
{
	return (new ChickenBlasterController(fighter));
}


ChickenBlasterController::ChickenBlasterController(FighterController *fighter) : WeaponController(kControllerChickenBlaster, fighter)
{
}

ChickenBlasterController::~ChickenBlasterController()
{
}

void ChickenBlasterController::Preprocess(void)
{
	WeaponController::Preprocess();
}

WeaponResult ChickenBlasterController::UpdateWeapon(const Point3D& position, const Vector3D& direction, const Point3D& center)
{
	return (kWeaponIdle);
}

// ZYUQURM
