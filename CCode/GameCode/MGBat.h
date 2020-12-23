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


#ifndef MGBat_h
#define MGBat_h


#include "MGMonster.h"


namespace C4
{
	enum : ControllerType
	{
		kControllerBat			= 'bat '
	};


	enum : ModelType
	{
		kModelBat				= 'bat '
	};


	enum : MethodType
	{
		kMethodGenerateBat		= 'bat ',
		kMethodGetBatCount		= 'batc'
	};


	class BatController;
	class GenerateBatMethod;
	class GetBatCountMethod;


	class Bat
	{
		private:

			ControllerReg<BatController>	batControllerRegistration;
			ModelRegistration				batModelRegistration;

			MethodReg<GenerateBatMethod>	generateBatRegistration;
			MethodReg<GetBatCountMethod>	getBatCountRegistration;

			Bat();
			~Bat();

		public:

			static void Construct(void);
			static void Destruct(void);
	};


	class BatConfiguration
	{
		protected:

			unsigned_int32		batFlags;

			BatConfiguration();
			BatConfiguration(const BatConfiguration& batConfiguration);

			void Pack(Packer& data) const;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data);

		public:

			int32 GetSettingCount(void) const;
			Setting *GetSetting(int32 index) const;
			void SetSetting(const Setting *setting);
	};


	class BatController final : public MonsterController, public BatConfiguration
	{
		private:

			enum
			{
				kBatStateSleeping,
				kBatStateFlying,
				kBatStateEscaping,
				kBatStateAttacking,
				kBatStateDead,
				kBatStateCount
			};

			enum
			{
				kBatMotionNone,
				kBatMotionRoost,
				kBatMotionFly,
				kBatMotionAttack,
				kBatMotionDeath
			};

			FrameAnimatorObserver<BatController>	frameAnimatorObserver;

			int32				waitTime;
			float				escapeAzimuth;
 
			int32				turnTime;
			float				turnRate;
 
			float				targetRoll;
			float				currentRoll; 

			OmniSource			*batSource;
 
			static int32		globalBatCount;
			static bool			(BatController::*batStateHandler[kBatStateCount])(void); 
 
			BatController(const BatController& batController);

			Controller *Replicate(void) const override;
 
			bool HandleSleepingState(void);
			bool HandleFlyingState(void);
			bool HandleEscapingState(void);
			bool HandleAttackingState(void);
			bool HandleDeadState(void);

			void HandleAnimationEvent(FrameAnimator *animator, CueType cueType);

			void UpdateTurnRate(int32 dt);
			void SetBatMotion(int32 motion);

			static void AttackCallback(Interpolator *interpolator, void *cookie);

		public:

			BatController();
			BatController(const BatConfiguration& configuration);
			~BatController();

			static int32 GetGlobalBatCount(void)
			{
				return (globalBatCount);
			}

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

			CharacterStatus Damage(Fixed damage, unsigned_int32 flags, GameCharacterController *attacker, const Point3D *position, const Vector3D *force) override;
			void Kill(GameCharacterController *attacker, const Point3D *position, const Vector3D *force) override;
	};


	class GenerateBatMethod final : public GenerateMonsterMethod, public BatConfiguration
	{
		private:

			GenerateBatMethod(const GenerateBatMethod& generateBatMethod);

			Method *Replicate(void) const override;

		public:

			GenerateBatMethod();
			~GenerateBatMethod();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(const ScriptState *state) override;
	};


	class GetBatCountMethod final : public Method
	{
		private:

			GetBatCountMethod(const GetBatCountMethod& getBatCountMethod);

			Method *Replicate(void) const override;

		public:

			GetBatCountMethod();
			~GetBatCountMethod();

			void Execute(const ScriptState *state) override;
	};
}


#endif

// ZYUQURM
