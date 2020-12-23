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


#ifndef MGPowers_h
#define MGPowers_h


#include "C4Models.h"
#include "MGBase.h"


namespace C4
{
	enum
	{
		kPowerNone,
		kPowerCandy,
		kPowerClock,
		kPowerFrog,
		kPowerDoubleAxe,
		kPowerSkullSword,
		kPowerPotion,
		kPowerCount
	};


	enum : ModelType
	{
		kModelCandyPower			= 'cndp',
		kModelClockPower			= 'clkp',
		kModelFrogPower				= 'frgp',
		kModelDoubleAxePower		= 'daxp',
		kModelSkullSwordPower		= 'sksp',
		kModelPotionPower			= 'potp'
	};


	enum : ModelType
	{
		kModelAppleHealth			= 'appl',
		kModelBananaHealth			= 'bana',
		kModelGrapesHealth			= 'grap',
		kModelOrangeHealth			= 'orng',
		kModelBurgerHealth			= 'burg',
		kModelPizzaHealth			= 'pizz',
		kModelSubHealth				= 'sub ',
		kModelTacoHealth			= 'taco',
		kModelHamHealth				= 'ham ',
		kModelRoastHealth			= 'rost',
		kModelSpaghettiHealth		= 'spag',
		kModelTurkeyHealth			= 'turk',
		kModelGobletHealth			= 'gblt'
	};


	class Power
	{
		private:

			int32			powerIndex;
			ModelType		modelType;

			static const Power *powerTable[kPowerCount];

		protected:

			Power(int32 index, ModelType type);

		public:

			int32 GetPowerIndex(void) const
			{
				return (powerIndex);
			}

			ModelType GetModelType(void) const
			{
				return (modelType);
			}

			static const Power *Get(int32 type)
			{
				return (powerTable[type]);
			}

			static ModelType PowerIndexToType(int32 powerIndex);
			static int32 PowerTypeToIndex(ModelType powerType);
	};


	class CandyPower final : public Power
	{
		private:

			ModelRegistration		candyPowerModelReg;

			CandyPower();
			~CandyPower();

		public:

			static void Construct(void);
			static void Destruct(void);
	}; 


	class ClockPower final : public Power 
	{
		private: 

			ModelRegistration		clockPowerModelReg;
 
			ClockPower();
			~ClockPower(); 
 
		public:

			static void Construct(void);
			static void Destruct(void); 
	};


	class FrogPower final : public Power
	{
		private:

			ModelRegistration		frogPowerModelReg;

			FrogPower();
			~FrogPower();

		public:

			static void Construct(void);
			static void Destruct(void);
	};


	class DoubleAxePower final : public Power
	{
		private:

			ModelRegistration		doubleAxePowerModelReg;

			DoubleAxePower();
			~DoubleAxePower();

		public:

			static void Construct(void);
			static void Destruct(void);
	};


	class SkullSwordPower final : public Power
	{
		private:

			ModelRegistration		skullSwordPowerModelReg;

			SkullSwordPower();
			~SkullSwordPower();

		public:

			static void Construct(void);
			static void Destruct(void);
	};


	class PotionPower final : public Power
	{
		private:

			ModelRegistration		potionPowerModelReg;

			PotionPower();
			~PotionPower();

		public:

			static void Construct(void);
			static void Destruct(void);
	};
}


#endif

// ZYUQURM
