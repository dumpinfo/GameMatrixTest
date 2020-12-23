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


#include "MGMaterials.h"
#include "MGAxe.h"
#include "MGPistol.h"
#include "MGCrossbow.h"
#include "MGSpikeShooter.h"
#include "MGGame.h"


using namespace C4;


namespace
{
	const SubstanceType gameSubstanceType[kGameSubstanceCount] =
	{
		kGameSubstanceDirt, kGameSubstanceStone, kGameSubstanceClay, kGameSubstanceWood, kGameSubstanceMetal,
		kGameSubstanceGlass, kGameSubstanceRubber, kGameSubstanceCardboard, kGameSubstanceStraw, kGameSubstanceFence, kGameSubstanceBurlap
	};

	const SubstanceData gameSubstanceData[kGameSubstanceCount] =
	{
		// Dirt
		{kSubstanceSoft,
		{"sound/step/Dirt1", "sound/step/Dirt2", "sound/step/Dirt3", "sound/step/Dirt4"},
		kAxeEffectNone, "",
		kMaterialGeneric, 0, {1.0F, 1.0F, 1.0F}, "",
		0, "spike/spike/Dirt2", "",
		kSpikeEffectDust, {"spike/spike/Dirt1", "spike/spike/Dirt2", "spike/spike/Dirt3"}, ""},

		// Stone
		{0,
		{"sound/step/Stone1", "sound/step/Stone2", "sound/step/Stone3", "sound/step/Stone4"},
		kAxeEffectSparks, "axe/Stone",
		kMaterialBulletHoleStone, kMarkingLight | kMarkingDepthWrite | kMarkingClipRange, {1.0F, 1.0F, 1.0F}, "",
		0, "crossbow/arrow/Stone", "",
		kSpikeEffectSparks, {"spike/spike/Metal1", "spike/spike/Metal2", "spike/spike/Metal3"}, "spike/spike/Rail"},

		// Clay
		{0,
		{"sound/step/Stone1", "sound/step/Stone2", "sound/step/Stone3", "sound/step/Stone4"},
		kAxeEffectSparks, "axe/Stone",
		kMaterialBulletHoleStone, kMarkingLight | kMarkingDepthWrite | kMarkingClipRange, {0.82F, 0.54F, 0.33F}, "",
		0, "crossbow/arrow/Stone", "",
		kSpikeEffectSparks, {"spike/spike/Metal1", "spike/spike/Metal2", "spike/spike/Metal3"}, "spike/spike/Rail"},

		// Wood
		{0,
		{"sound/step/Wood1", "sound/step/Wood2", "sound/step/Wood3", "sound/step/Wood4"},
		kAxeEffectNone, "axe/Wood",
		kMaterialBulletHoleWood, kMarkingLight | kMarkingDepthWrite | kMarkingClipRange, {1.0F, 1.0F, 1.0F}, "",
		kArrowImpactStick, "spike/spike/Wood1", "crossbow/arrow/Stick",
		kSpikeEffectSparks, {"spike/spike/Wood1", "spike/spike/Wood2", "spike/spike/Wood3"}, ""},

		// Metal
		{0,
		{"sound/step/Metal1", "sound/step/Metal2", "sound/step/Metal3", "sound/step/Metal4"},
		kAxeEffectSparks, "axe/Metal",
		kMaterialBulletHoleMetal, kMarkingLight | kMarkingDepthWrite | kMarkingClipRange, {1.0F, 1.0F, 1.0F}, "pistol/hole/Metal",
		0, "crossbow/arrow/Metal", "",
		kSpikeEffectSparks, {"spike/spike/Metal1", "spike/spike/Metal2", "spike/spike/Metal3"}, "spike/spike/Rail"},

		// Glass
		{0,
		{"sound/step/Stone1", "sound/step/Stone2", "sound/step/Stone3", "sound/step/Stone4"},
		kAxeEffectSparks, "axe/Metal",
		kMaterialBulletHoleGlass, kMarkingLight | kMarkingBlendLight, {1.0F, 1.0F, 1.0F}, "pistol/hole/Glass",
		0, "crossbow/arrow/Metal", "",
		kSpikeEffectSparks, {"spike/spike/Metal1", "spike/spike/Metal2", "spike/spike/Metal3"}, "spike/spike/Rail"},

		// Rubber
		{0,
		{"", "", "", ""},
		kAxeEffectNone, "axe/Rubber",
		kMaterialGeneric, 0, {1.0F, 1.0F, 1.0F}, "",
		0, "axe/Rubber", "",
		kSpikeEffectSparks, {"spike/spike/Dirt1", "spike/spike/Dirt2", "spike/spike/Dirt3"}, "spike/spike/Rail"},

		// Cardboard
		{0,
		{"", "", "", ""},
		kAxeEffectNone, "axe/Cardboard",
		kMaterialGeneric, 0, {1.0F, 1.0F, 1.0F}, "",
		0, "axe/Rubber", "",
		kSpikeEffectDust, {"spike/spike/Dirt1", "spike/spike/Dirt2", "spike/spike/Dirt3"}, ""},

		// Straw
		{0,
		{"", "", "", ""},
		kAxeEffectNone, "axe/Straw",
		kMaterialGeneric, 0, {1.0F, 1.0F, 1.0F}, "",
		0, "axe/Straw", "",
		kSpikeEffectDust, {"spike/spike/Dirt1", "spike/spike/Dirt2", "spike/spike/Dirt3"}, ""},

		// Fence
		{0,
		{"", "", "", ""},
		kAxeEffectSparks, "axe/Fence",
		kMaterialGeneric, 0, {1.0F, 1.0F, 1.0F}, "",
		0, "crossbow/arrow/Metal", "",
		kSpikeEffectSparks, {"spike/spike/Metal1", "spike/spike/Metal2", "spike/spike/Metal3"}, "spike/spike/Rail"},
 
		// Burlap
		{kSubstanceSoft,
		{"sound/step/Dirt1", "sound/step/Dirt2", "sound/step/Dirt3", "sound/step/Dirt4"}, 
		kAxeEffectNone, "axe/Burlap",
		kMaterialGeneric, 0, {1.0F, 1.0F, 1.0F}, "", 
		0, "spike/spike/Dirt2", "",
		kSpikeEffectDust, {"spike/spike/Dirt1", "spike/spike/Dirt2", "spike/spike/Dirt3"}, ""}
	}; 

	const MaterialType gameMaterialType[kGameMaterialCount] = 
	{ 
		kMaterialSplatter, kMaterialWine, kMaterialToxicGoo, kMaterialTarball, kMaterialLavaball, kMaterialVenomSplat
	};

	const char *const gameMaterialName[kGameMaterialCount] = 
	{
		"material/Splatter",
		"material/Wine",
		"material/ToxicGoo",
		"material/Tarball",
		"material/Lavaball",
		"material/VenomSplat"
	};

	const MaterialData gameMaterialData[kGameMaterialCount] =
	{
		{{1.0F, 1.0F, 1.0F}},
		{{0.3137F, 0.03922F, 0.07843F}},
		{{0.17255F, 0.196F, 0.0353F}},
		{{0.0625F, 0.0625F, 0.0625F}},
		{{1.0F, 0.75F, 0.0F}},
		{{0.078125F, 0.5F, 0.078125F}}
	};

	Storage<GameSubstance>			substanceStorage[kGameSubstanceCount];
	Storage<MaterialRegistration>	materialStorage[kGameMaterialCount];
}


GameSubstance::GameSubstance(SubstanceType type, const char *name, const SubstanceData& data) :
		Substance(type, name),
		substanceData(data)
{
}

GameSubstance::~GameSubstance()
{
}

void GameSubstance::RegisterSubstances(void)
{
	const StringTable *table = TheGame->GetStringTable();

	GameSubstance *substance = substanceStorage[0];
	for (machine a = 0; a < kGameSubstanceCount; a++)
	{
		SubstanceType type = gameSubstanceType[a];
		new(substance) GameSubstance(type, table->GetString(StringID('SBST', type)), gameSubstanceData[a]);
		MaterialObject::RegisterSubstance(substance);
		substance++;
	}
}

void GameSubstance::UnregisterSubstances(void)
{
	GameSubstance *substance = substanceStorage[0];
	for (machine a = kGameSubstanceCount - 1; a >= 0; a--)
	{
		substance[a].~GameSubstance();
	}
}

void GameSubstance::RegisterMaterials(void)
{
	MaterialRegistration *registration = materialStorage[0];
	for (machine a = 0; a < kGameMaterialCount; a++)
	{
		new(registration) MaterialRegistration(gameMaterialType[a], gameMaterialName[a], &gameMaterialData[a]);
		registration++;
	}
}

void GameSubstance::UnregisterMaterials(void)
{
	MaterialRegistration *registration = materialStorage[0];
	for (machine a = kGameMaterialCount - 1; a >= 0; a--)
	{
		registration[a].~MaterialRegistration();
	}
}

// ZYUQURM
