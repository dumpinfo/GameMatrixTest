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


#include "MGHornetGun.h"
#include "MGGame.h"


using namespace C4;


namespace
{
	Storage<HornetGun> hornetGunStorage;
}


HornetGun::HornetGun() :
		Weapon(kWeaponHornetGun, 0, kModelHornetGun, 0, 0, 1),
		hornetGunModelRegistration(kModelHornetGun, TheGame->GetStringTable()->GetString(StringID('MODL', 'WEAP', kModelHornetGun)), "hornet/gun/HornetGun", 0, kControllerCollectable)
{
}

HornetGun::~HornetGun()
{
}

void HornetGun::Construct(void)
{
	new(hornetGunStorage) HornetGun;
}

void HornetGun::Destruct(void)
{
	hornetGunStorage->~HornetGun();
}

WeaponController *HornetGun::NewWeaponController(FighterController *fighter) const
{
	return (new HornetGunController(fighter));
}


HornetGunController::HornetGunController(FighterController *fighter) : WeaponController(kControllerHornetGun, fighter)
{
}

HornetGunController::~HornetGunController()
{
}

void HornetGunController::Preprocess(void)
{
	WeaponController::Preprocess();
}

WeaponResult HornetGunController::UpdateWeapon(const Point3D& position, const Vector3D& direction, const Point3D& center)
{
	return (kWeaponIdle);
}

// ZYUQURM
