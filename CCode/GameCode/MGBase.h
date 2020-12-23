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


#ifndef MGBase_h
#define MGBase_h


#include "C4Sound.h"
#include "C4Zones.h"
#include "C4Application.h"


#define MGCHEATS	1


extern "C"
{
	C4MODULEEXPORT C4::Application *CreateApplication(void);
}


namespace C4
{
	enum
	{
		kInputXInverted				= 1 << 0,
		kInputYInverted				= 1 << 1
	};


	enum
	{
		kPlayerColorCount			= 11
	};


	enum : LocatorType
	{
		kLocatorSpawn				= 'spwn',
		kLocatorSpectator			= 'spec',
		kLocatorTeleport			= 'tlpt',
		kLocatorBenchmark			= 'bnch'
	};


	enum : SoundGroupType
	{
		kSoundGroupEffects			= 'efct',
		kSoundGroupMusic			= 'musi',
		kSoundGroupVoice			= 'voic'
	};


	enum
	{
		kSoundPriorityExplosion		= 1,
		kSoundPriorityImpact		= 1,
		kSoundPriorityPlayer		= 2,
		kSoundPriorityWeapon		= 2,
		kSoundPriorityDeath			= 3
	};


	enum
	{
		kCollisionCorpse			= kCollisionBaseKind << 0,
		kCollisionRemains			= kCollisionBaseKind << 1,
		kCollisionPlayer			= kCollisionBaseKind << 2,
		kCollisionShot				= kCollisionBaseKind << 3,
		kCollisionPlasma			= kCollisionBaseKind << 4,
		kCollisionFireball			= kCollisionBaseKind << 5,
		kCollisionBlackCat			= kCollisionBaseKind << 6,
		kCollisionVenom				= kCollisionBaseKind << 7
	};


	class GameCharacterController;
	class GameRigidBodyController;


	struct SplashDamageData
	{
		Array<Link<GameCharacterController>, 32>	characterArray;
		Array<Link<GameRigidBodyController>, 32>	rigidBodyArray;
	};
}


#endif

// ZYUQURM
