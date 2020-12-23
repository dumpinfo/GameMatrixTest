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


#include "MGCatFlinger.h"
#include "MGBlackCat.h"
#include "MGFighter.h"
#include "MGGame.h"


using namespace C4;


namespace
{
	Storage<CatFlinger> catFlingerStorage;
}


CatFlinger::CatFlinger() :
		Weapon(kWeaponCatFlinger, 0, kModelCatFlinger, kActionSpecialWeapon, 16, 16),
		catFlingerModelRegistration(kModelCatFlinger, TheGame->GetStringTable()->GetString(StringID('MODL', 'WEAP', kModelCatFlinger)), "cat/gun/CatFlinger", 0, kControllerCollectable)
{
}

CatFlinger::~CatFlinger()
{
}

void CatFlinger::Construct(void)
{
	new(catFlingerStorage) CatFlinger;
}

void CatFlinger::Destruct(void)
{
	catFlingerStorage->~CatFlinger();
}

WeaponController *CatFlinger::NewWeaponController(FighterController *fighter) const
{
	return (new CatFlingerController(fighter));
}


CatFlingerController::CatFlingerController(FighterController *fighter) : WeaponController(kControllerCatFlinger, fighter)
{
}

CatFlingerController::~CatFlingerController()
{
}

void CatFlingerController::Preprocess(void)
{
	WeaponController::Preprocess();
}

WeaponResult CatFlingerController::UpdateWeapon(const Point3D& position, const Vector3D& direction, const Point3D& center)
{
	if (TheMessageMgr->Server())
	{
		int32 dt = TheTimeMgr->GetDeltaTime();
		int32 time = GetFireDelayTime() - dt;

		const FighterController *fighter = GetFighterController();
		unsigned_int32 flags = fighter->GetFighterFlags();

		if (((flags & kFighterFiring) != 0) && (time <= 0))
		{
			SetFireDelayTime(time + 500);

			GamePlayer *player = fighter->GetFighterPlayer();
			int32 ammo = player->GetWeaponAmmo(kWeaponCatFlinger, 0);
			if (ammo <= 0)
			{
				return (kWeaponEmpty);
			}

			player->SetWeaponAmmo(kWeaponCatFlinger, 0, ammo - 1);

			World *world = GetTargetNode()->GetWorld();
			int32 projectileIndex = world->NewControllerIndex();
			TheMessageMgr->SendMessageAll(CreateBlackCatMessage(projectileIndex, fighter->GetControllerIndex(), position, direction * 40.0F, 2));

			ProjectileController *projectileController = static_cast<ProjectileController *>(world->GetController(projectileIndex));
			projectileController->SetFirstPosition(center);
			return (kWeaponFired);
		}
		else
		{
			SetFireDelayTime(time);
		}
	}

	return (kWeaponIdle);
}

// ZYUQURM
