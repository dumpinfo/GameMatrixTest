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


#include "MGTreasure.h"
#include "MGGame.h"


using namespace C4;


namespace
{
	Storage<GoldCoinTreasure>		goldCoinStorage;
	Storage<SilverCoinTreasure>		silverCoinStorage;
	Storage<BronzeCoinTreasure>		bronzeCoinStorage;
	Storage<CandyCornTreasure>		candyCornStorage;
	Storage<GoldSkullTreasure>		goldSkullStorage;
	Storage<RedJewelTreasure>		redJewelStorage;
	Storage<PurpleJewelTreasure>	purpleJewelStorage;
	Storage<GreenJewelTreasure>		greenJewelStorage;
	Storage<YellowJewelTreasure>	yellowJewelStorage;
	Storage<CrystalTreasure>		crystalStorage;
}


const Treasure *Treasure::treasureTable[kTreasureCount] = {nullptr};


Treasure::Treasure(int32 index, ModelType type)
{
	treasureIndex = index;
	modelType = type;

	treasureTable[index] = this;
}

ModelType Treasure::TreasureIndexToType(int32 treasureIndex)
{
	const Treasure *treasure = treasureTable[treasureIndex];
	return ((treasure) ? treasure->GetModelType() : 0);
}

int32 Treasure::TreasureTypeToIndex(ModelType treasureType)
{
	for (machine a = kTreasureNone + 1; a < kTreasureCount; a++)
	{
		const Treasure *treasure = treasureTable[a];
		if ((treasure) && (treasure->GetModelType() == treasureType))
		{
			return (a);
		}
	}

	return (kTreasureNone);
}


GoldCoinTreasure::GoldCoinTreasure() :
		Treasure(kTreasureGoldCoin, kModelGoldCoinTreasure),
		goldCoinTreasureModelReg(kModelGoldCoinTreasure, TheGame->GetStringTable()->GetString(StringID('MODL', 'TRES', kModelGoldCoinTreasure)), "treasure/Coin-gold", 0, kControllerCollectable)
{
}

GoldCoinTreasure::~GoldCoinTreasure()
{
}

void GoldCoinTreasure::Construct(void)
{
	new(goldCoinStorage) GoldCoinTreasure;
}

void GoldCoinTreasure::Destruct(void)
{
	goldCoinStorage->~GoldCoinTreasure();
}


SilverCoinTreasure::SilverCoinTreasure() :
		Treasure(kTreasureSilverCoin, kModelSilverCoinTreasure),
		silverCoinTreasureModelReg(kModelSilverCoinTreasure, TheGame->GetStringTable()->GetString(StringID('MODL', 'TRES', kModelSilverCoinTreasure)), "treasure/Coin-silver", 0, kControllerCollectable)
{
}

SilverCoinTreasure::~SilverCoinTreasure()
{
}

void SilverCoinTreasure::Construct(void)
{
	new(silverCoinStorage) SilverCoinTreasure;
}

void SilverCoinTreasure::Destruct(void)
{
	silverCoinStorage->~SilverCoinTreasure();
}


BronzeCoinTreasure::BronzeCoinTreasure() :
		Treasure(kTreasureBronzeCoin, kModelBronzeCoinTreasure),
		bronzeCoinTreasureModelReg(kModelBronzeCoinTreasure, TheGame->GetStringTable()->GetString(StringID('MODL', 'TRES', kModelBronzeCoinTreasure)), "treasure/Coin-bronze", 0, kControllerCollectable)
{
} 

BronzeCoinTreasure::~BronzeCoinTreasure()
{ 
}
 
void BronzeCoinTreasure::Construct(void)
{
	new(bronzeCoinStorage) BronzeCoinTreasure; 
}
 
void BronzeCoinTreasure::Destruct(void) 
{
	bronzeCoinStorage->~BronzeCoinTreasure();
}
 

CandyCornTreasure::CandyCornTreasure() :
		Treasure(kTreasureCandyCorn, kModelCandyCornTreasure),
		candyCornTreasureModelReg(kModelCandyCornTreasure, TheGame->GetStringTable()->GetString(StringID('MODL', 'TRES', kModelCandyCornTreasure)), "treasure/CandyCorn", 0, kControllerCollectable)
{
}

CandyCornTreasure::~CandyCornTreasure()
{
}

void CandyCornTreasure::Construct(void)
{
	new(candyCornStorage) CandyCornTreasure;
}

void CandyCornTreasure::Destruct(void)
{
	candyCornStorage->~CandyCornTreasure();
}


GoldSkullTreasure::GoldSkullTreasure() :
		Treasure(kTreasureGoldSkull, kModelGoldSkullTreasure),
		goldSkullTreasureModelReg(kModelGoldSkullTreasure, TheGame->GetStringTable()->GetString(StringID('MODL', 'TRES', kModelGoldSkullTreasure)), "treasure/Skull-gold", 0, kControllerCollectable)
{
}

GoldSkullTreasure::~GoldSkullTreasure()
{
}

void GoldSkullTreasure::Construct(void)
{
	new(goldSkullStorage) GoldSkullTreasure;
}

void GoldSkullTreasure::Destruct(void)
{
	goldSkullStorage->~GoldSkullTreasure();
}


RedJewelTreasure::RedJewelTreasure() :
		Treasure(kTreasureRedJewel, kModelRedJewelTreasure),
		redJewelTreasureModelReg(kModelRedJewelTreasure, TheGame->GetStringTable()->GetString(StringID('MODL', 'TRES', kModelRedJewelTreasure)), "treasure/Jewel-red", 0, kControllerCollectable)
{
}

RedJewelTreasure::~RedJewelTreasure()
{
}

void RedJewelTreasure::Construct(void)
{
	new(redJewelStorage) RedJewelTreasure;
}

void RedJewelTreasure::Destruct(void)
{
	redJewelStorage->~RedJewelTreasure();
}


PurpleJewelTreasure::PurpleJewelTreasure() :
		Treasure(kTreasurePurpleJewel, kModelPurpleJewelTreasure),
		purpleJewelTreasureModelReg(kModelPurpleJewelTreasure, TheGame->GetStringTable()->GetString(StringID('MODL', 'TRES', kModelPurpleJewelTreasure)), "treasure/Jewel-purple", 0, kControllerCollectable)
{
}

PurpleJewelTreasure::~PurpleJewelTreasure()
{
}

void PurpleJewelTreasure::Construct(void)
{
	new(purpleJewelStorage) PurpleJewelTreasure;
}

void PurpleJewelTreasure::Destruct(void)
{
	purpleJewelStorage->~PurpleJewelTreasure();
}


GreenJewelTreasure::GreenJewelTreasure() :
		Treasure(kTreasureGreenJewel, kModelGreenJewelTreasure),
		greenJewelTreasureModelReg(kModelGreenJewelTreasure, TheGame->GetStringTable()->GetString(StringID('MODL', 'TRES', kModelGreenJewelTreasure)), "treasure/Jewel-green", 0, kControllerCollectable)
{
}

GreenJewelTreasure::~GreenJewelTreasure()
{
}

void GreenJewelTreasure::Construct(void)
{
	new(greenJewelStorage) GreenJewelTreasure;
}

void GreenJewelTreasure::Destruct(void)
{
	greenJewelStorage->~GreenJewelTreasure();
}


YellowJewelTreasure::YellowJewelTreasure() :
		Treasure(kTreasureYellowJewel, kModelYellowJewelTreasure),
		yellowJewelTreasureModelReg(kModelYellowJewelTreasure, TheGame->GetStringTable()->GetString(StringID('MODL', 'TRES', kModelYellowJewelTreasure)), "treasure/Jewel-yellow", 0, kControllerCollectable)
{
}

YellowJewelTreasure::~YellowJewelTreasure()
{
}

void YellowJewelTreasure::Construct(void)
{
	new(yellowJewelStorage) YellowJewelTreasure;
}

void YellowJewelTreasure::Destruct(void)
{
	yellowJewelStorage->~YellowJewelTreasure();
}


CrystalTreasure::CrystalTreasure() :
		Treasure(kTreasureCrystal, kModelCrystalTreasure),
		crystalTreasureModelReg(kModelCrystalTreasure, TheGame->GetStringTable()->GetString(StringID('MODL', 'TRES', kModelCrystalTreasure)), "treasure/Crystal", 0, kControllerCollectable)
{
}

CrystalTreasure::~CrystalTreasure()
{
}

void CrystalTreasure::Construct(void)
{
	new(crystalStorage) CrystalTreasure;
}

void CrystalTreasure::Destruct(void)
{
	crystalStorage->~CrystalTreasure();
}

// ZYUQURM
