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


#ifndef MGMaterials_h
#define MGMaterials_h


#include "C4MaterialObjects.h"
#include "MGBase.h"


namespace C4
{
	enum : SubstanceType
	{
		kGameSubstanceDirt			= 'dirt',
		kGameSubstanceStone			= 'ston',
		kGameSubstanceClay			= 'clay',
		kGameSubstanceWood			= 'wood',
		kGameSubstanceMetal			= 'metl',
		kGameSubstanceGlass			= 'glas',
		kGameSubstanceRubber		= 'rbbr',
		kGameSubstanceCardboard		= 'cbrd',
		kGameSubstanceStraw			= 'strw',
		kGameSubstanceFence			= 'fenc',
		kGameSubstanceBurlap		= 'brlp',
		kGameSubstanceCount			= 11
	};


	enum : MaterialType
	{
		kMaterialSplatter			= 'splt',
		kMaterialWine				= 'wine',
		kMaterialToxicGoo			= 'toxc',
		kMaterialTarball			= 'trbl',
		kMaterialLavaball			= 'lvbl',
		kMaterialVenomSplat			= 'vnsp',
		kGameMaterialCount			= 6
	};


	enum
	{
		kSubstanceSoft				= 1 << 0
	};


	struct SubstanceData
	{
		enum
		{
			kMaxStringSize = 32
		};

		unsigned_int32		substanceFlags;

		char				footstepSoundName[4][kMaxStringSize];

		unsigned_int32		axeImpactEffectType;
		char				axeImpactSoundName[kMaxStringSize];

		MaterialType		bulletHoleMaterialType;
		unsigned_int32		bulletHoleMarkingFlags;
		ConstColorRGB		bulletHoleColor;
		char				bulletImpactSoundName[kMaxStringSize];

		unsigned_int32		arrowImpactFlags;
		char				arrowImpactSoundName[kMaxStringSize];
		char				arrowStickSoundName[kMaxStringSize];

		int32				spikeImpactEffectType;
		char				spikeImpactSoundName[3][kMaxStringSize];
		char				railImpactSoundName[kMaxStringSize];
	};


	struct MaterialData
	{
		ConstColorRGB		materialColor;
	};


	class GameSubstance : public Substance
	{
		private:

			SubstanceData		substanceData;

		public:

			GameSubstance(SubstanceType type, const char *name, const SubstanceData& data);
			~GameSubstance();

			const SubstanceData *GetSubstanceData(void) const
			{
				return (&substanceData);
			}

			static void RegisterSubstances(void);
			static void UnregisterSubstances(void);

			static void RegisterMaterials(void);
			static void UnregisterMaterials(void); 
	};
}
 

#endif 

// ZYUQURM
