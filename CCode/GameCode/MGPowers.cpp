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


#include "MGPowers.h"
#include "MGGame.h"


using namespace C4;


namespace
{
	Storage<CandyPower>			candyStorage;
	Storage<ClockPower>			clockStorage;
	Storage<FrogPower>			frogStorage;
	Storage<DoubleAxePower>		doubleAxeStorage;
	Storage<SkullSwordPower>	skullSwordStorage;
	Storage<PotionPower>		potionStorage;
}


const Power *Power::powerTable[kPowerCount] = {nullptr};


Power::Power(int32 index, ModelType type)
{
	powerIndex = index;
	modelType = type;

	powerTable[index] = this;
}

ModelType Power::PowerIndexToType(int32 powerIndex)
{
	const Power *power = powerTable[powerIndex];
	return ((power) ? power->GetModelType() : 0);
}

int32 Power::PowerTypeToIndex(ModelType powerType)
{
	for (machine a = kPowerNone + 1; a < kPowerCount; a++)
	{
		const Power *power = powerTable[a];
		if ((power) && (power->GetModelType() == powerType))
		{
			return (a);
		}
	}

	return (kPowerNone);
}


CandyPower::CandyPower() :
		Power(kPowerCandy, kModelCandyPower),
		candyPowerModelReg(kModelCandyPower, TheGame->GetStringTable()->GetString(StringID('MODL', 'POWR', kModelCandyPower)), "power/candy/Candy", 0, kControllerCollectable)
{
}

CandyPower::~CandyPower()
{
}

void CandyPower::Construct(void)
{
	new(candyStorage) CandyPower;
}

void CandyPower::Destruct(void)
{
	candyStorage->~CandyPower();
}


ClockPower::ClockPower() :
		Power(kPowerClock, kModelClockPower),
		clockPowerModelReg(kModelClockPower, TheGame->GetStringTable()->GetString(StringID('MODL', 'POWR', kModelClockPower)), "power/clock/Clock", 0, kControllerCollectable)
{
}

ClockPower::~ClockPower()
{
}

void ClockPower::Construct(void)
{
	new(clockStorage) ClockPower;
}

void ClockPower::Destruct(void)
{
	clockStorage->~ClockPower();
}


FrogPower::FrogPower() :
		Power(kPowerFrog, kModelFrogPower),
		frogPowerModelReg(kModelFrogPower, TheGame->GetStringTable()->GetString(StringID('MODL', 'POWR', kModelFrogPower)), "power/frog/Frog", 0, kControllerCollectable)
{
}

FrogPower::~FrogPower()
{
} 

void FrogPower::Construct(void)
{ 
	new(frogStorage) FrogPower;
} 

void FrogPower::Destruct(void)
{ 
	frogStorage->~FrogPower();
} 
 

DoubleAxePower::DoubleAxePower() :
		Power(kPowerDoubleAxe, kModelDoubleAxePower),
		doubleAxePowerModelReg(kModelDoubleAxePower, TheGame->GetStringTable()->GetString(StringID('MODL', 'POWR', kModelDoubleAxePower)), "power/axes/Axes", 0, kControllerCollectable) 
{
}

DoubleAxePower::~DoubleAxePower()
{
}

void DoubleAxePower::Construct(void)
{
	new(doubleAxeStorage) DoubleAxePower;
}

void DoubleAxePower::Destruct(void)
{
	doubleAxeStorage->~DoubleAxePower();
}


SkullSwordPower::SkullSwordPower() :
		Power(kPowerSkullSword, kModelSkullSwordPower),
		skullSwordPowerModelReg(kModelSkullSwordPower, TheGame->GetStringTable()->GetString(StringID('MODL', 'POWR', kModelSkullSwordPower)), "power/sword/Sword", 0, kControllerCollectable)
{
}

SkullSwordPower::~SkullSwordPower()
{
}

void SkullSwordPower::Construct(void)
{
	new(skullSwordStorage) SkullSwordPower;
}

void SkullSwordPower::Destruct(void)
{
	skullSwordStorage->~SkullSwordPower();
}


PotionPower::PotionPower() :
		Power(kPowerPotion, kModelPotionPower),
		potionPowerModelReg(kModelPotionPower, TheGame->GetStringTable()->GetString(StringID('MODL', 'POWR', kModelPotionPower)), "power/potion/Potion", 0, kControllerCollectable)
{
}

PotionPower::~PotionPower()
{
}

void PotionPower::Construct(void)
{
	new(potionStorage) PotionPower;
}

void PotionPower::Destruct(void)
{
	potionStorage->~PotionPower();
}

// ZYUQURM
