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


#ifndef MGChicken_h
#define MGChicken_h


#include "C4Sources.h"
#include "MGCharacter.h"


namespace C4
{
	enum : ControllerType
	{
		kControllerChicken		= 'chik'
	};


	enum : ModelType
	{
		kModelChicken			= 'chik'
	};


	enum : MaterialType
	{
		kMaterialChickenWhite	= 'cwht',
		kMaterialChickenBrown	= 'cbrn',
		kMaterialChickenBlack	= 'cblk'
	};


	enum : Type
	{
		kChickenVariantWhite	= 'WHIT',
		kChickenVariantBrown	= 'BRWN',
		kChickenVariantBlack	= 'BLCK',
		kChickenVariantCount	= 3
	};


	enum
	{
		kChickenInitialized		= 1 << 0
	};


	class ChickenController;


	class Chicken
	{
		private:

			ControllerReg<ChickenController>	chickenControllerRegistration;

			ModelRegistration					chickenModelRegistration;

			MaterialRegistration				whiteMaterialRegistration;
			MaterialRegistration				brownMaterialRegistration;
			MaterialRegistration				blackMaterialRegistration;

			Chicken();
			~Chicken();

		public:

			static void Construct(void);
			static void Destruct(void);
	};


	class ChickenController final : public GameCharacterController
	{
		private:

			enum
			{
				kChickenStateSleeping	= 0,
				kChickenStateStanding	= 1,
				kChickenStateWalking	= 2,
				kChickenStateRunning	= 3,
				kChickenStateCount		= 4
			};

			enum
			{
				kChickenMotionNone		= 0,
				kChickenMotionStand		= 1,
				kChickenMotionWalk		= 2,
				kChickenMotionRun		= 3
			};

			unsigned_int32			chickenFlags;
			Type					chickenVariant;

			int32					chickenState;
			int32					chickenMotion;

			int32					soundTime;
			int32					decisionTime;

			int32					turnTime; 
			float					turnRate;

			float					chickenAzimuth; 

			AnimationBlender		animationBlender; 

			static bool				(ChickenController::*chickenStateHandler[kChickenStateCount])(void);
 
			ChickenController(const ChickenController& chickenController);
 
			Controller *Replicate(void) const override; 

			void NewSoundTime(void);
			void NewStandingTime(void);
			void NewWalkingTime(void); 
			void NewRunningTime(void);

			bool HandleSleepingState(void);
			bool HandleStandingState(void);
			bool HandleWalkingState(void);
			bool HandleRunningState(void);

			void SetChickenMotion(int32 motion);

			static void StandCallback(Interpolator *interpolator, void *cookie);
			static void SoundComplete(Source *source, void *cookie);

		public:

			ChickenController();
			~ChickenController();

			static bool ValidNode(const Node *node);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Preprocess(void) override;
			void Wake(void) override;
			void Move(void) override;

			RigidBodyStatus HandleNewRigidBodyContact(const RigidBodyContact *contact, RigidBodyController *contactBody);
			RigidBodyStatus HandleNewGeometryContact(const GeometryContact *contact);
	};
}


#endif

// ZYUQURM
