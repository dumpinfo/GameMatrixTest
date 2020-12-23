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


#ifndef MGTreasure_h
#define MGTreasure_h


#include "C4Models.h"
#include "MGBase.h"


namespace C4
{
	enum
	{
		kTreasureNone,
		kTreasureGoldCoin,
		kTreasureSilverCoin,
		kTreasureBronzeCoin,
		kTreasureCandyCorn,
		kTreasureGoldSkull,
		kTreasureRedJewel,
		kTreasurePurpleJewel,
		kTreasureGreenJewel,
		kTreasureYellowJewel,
		kTreasureCrystal,
		kTreasureCount
	};


	enum : ModelType
	{
		kModelGoldCoinTreasure		= 'gdct',
		kModelSilverCoinTreasure	= 'svct',
		kModelBronzeCoinTreasure	= 'bzct',
		kModelCandyCornTreasure		= 'cnct',
		kModelGoldSkullTreasure		= 'gskt',
		kModelRedJewelTreasure		= 'rdjt',
		kModelPurpleJewelTreasure	= 'prjt',
		kModelGreenJewelTreasure	= 'grjt',
		kModelYellowJewelTreasure	= 'yljt',
		kModelCrystalTreasure		= 'cryt'
	};


	class Treasure
	{
		private:

			int32			treasureIndex;
			ModelType		modelType;

			static const Treasure *treasureTable[kTreasureCount];

		protected:

			Treasure(int32 index, ModelType type);

		public:

			int32 GetTreasureIndex(void) const
			{
				return (treasureIndex);
			}

			ModelType GetModelType(void) const
			{
				return (modelType);
			}

			static const Treasure *Get(int32 type)
			{
				return (treasureTable[type]);
			}

			static ModelType TreasureIndexToType(int32 treasureIndex);
			static int32 TreasureTypeToIndex(ModelType treasureType);
	};


	class GoldCoinTreasure final : public Treasure
	{
		private:

			ModelRegistration		goldCoinTreasureModelReg;

			GoldCoinTreasure();
			~GoldCoinTreasure();

		public:

			static void Construct(void);
			static void Destruct(void);
	};


	class SilverCoinTreasure final : public Treasure
	{
		private:

			ModelRegistration		silverCoinTreasureModelReg;

			SilverCoinTreasure();
			~SilverCoinTreasure(); 

		public:
 
			static void Construct(void);
			static void Destruct(void); 
	};

 
	class BronzeCoinTreasure final : public Treasure
	{ 
		private: 

			ModelRegistration		bronzeCoinTreasureModelReg;

			BronzeCoinTreasure(); 
			~BronzeCoinTreasure();

		public:

			static void Construct(void);
			static void Destruct(void);
	};


	class CandyCornTreasure final : public Treasure
	{
		private:

			ModelRegistration		candyCornTreasureModelReg;

			CandyCornTreasure();
			~CandyCornTreasure();

		public:

			static void Construct(void);
			static void Destruct(void);
	};


	class GoldSkullTreasure final : public Treasure
	{
		private:

			ModelRegistration		goldSkullTreasureModelReg;

			GoldSkullTreasure();
			~GoldSkullTreasure();

		public:

			static void Construct(void);
			static void Destruct(void);
	};


	class RedJewelTreasure final : public Treasure
	{
		private:

			ModelRegistration		redJewelTreasureModelReg;

			RedJewelTreasure();
			~RedJewelTreasure();

		public:

			static void Construct(void);
			static void Destruct(void);
	};


	class PurpleJewelTreasure final : public Treasure
	{
		private:

			ModelRegistration		purpleJewelTreasureModelReg;

			PurpleJewelTreasure();
			~PurpleJewelTreasure();

		public:

			static void Construct(void);
			static void Destruct(void);
	};


	class GreenJewelTreasure final : public Treasure
	{
		private:

			ModelRegistration		greenJewelTreasureModelReg;

			GreenJewelTreasure();
			~GreenJewelTreasure();

		public:

			static void Construct(void);
			static void Destruct(void);
	};


	class YellowJewelTreasure final : public Treasure
	{
		private:

			ModelRegistration		yellowJewelTreasureModelReg;

			YellowJewelTreasure();
			~YellowJewelTreasure();

		public:

			static void Construct(void);
			static void Destruct(void);
	};


	class CrystalTreasure final : public Treasure
	{
		private:

			ModelRegistration		crystalTreasureModelReg;

			CrystalTreasure();
			~CrystalTreasure();

		public:

			static void Construct(void);
			static void Destruct(void);
	};
}


#endif

// ZYUQURM
