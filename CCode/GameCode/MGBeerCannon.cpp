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


#include "MGBeerCannon.h"
#include "MGGame.h"


using namespace C4;


namespace
{
	Storage<BeerCannon> beerCannonStorage;
}


BeerCannon::BeerCannon() :
		Weapon(kWeaponBeerCannon, 0, kModelBeerCannon, 0, 0, 1),
		beerCannonModelRegistration(kModelBeerCannon, TheGame->GetStringTable()->GetString(StringID('MODL', 'WEAP', kModelBeerCannon)), "beer/gun/BeerCannon", 0, kControllerCollectable)
{
}

BeerCannon::~BeerCannon()
{
}

void BeerCannon::Construct(void)
{
	new(beerCannonStorage) BeerCannon;
}

void BeerCannon::Destruct(void)
{
	beerCannonStorage->~BeerCannon();
}

WeaponController *BeerCannon::NewWeaponController(FighterController *fighter) const
{
	return (new BeerCannonController(fighter));
}


BeerCannonController::BeerCannonController(FighterController *fighter) : WeaponController(kControllerBeerCannon, fighter)
{
}

BeerCannonController::~BeerCannonController()
{
}

void BeerCannonController::Preprocess(void)
{
	WeaponController::Preprocess();
}

WeaponResult BeerCannonController::UpdateWeapon(const Point3D& position, const Vector3D& direction, const Point3D& center)
{
	return (kWeaponIdle);
}

// ZYUQURM
