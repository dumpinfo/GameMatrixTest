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


#ifndef MGGoblin_h
#define MGGoblin_h


#include "MGMonster.h"


namespace C4
{
	enum : ControllerType
	{
		kControllerGoblin			= 'gbln'
	};


	enum : ModelType
	{
		kModelGoblin				= 'gbln',
		kModelGoblinRagdoll			= 'gbrd',
		kModelGoatSkull				= 'gtsk',
		kModelDeerSkull				= 'drsk'
	};


	enum : MethodType
	{
		kMethodGenerateGoblin		= 'gbln',
		kMethodGetGoblinCount		= 'gblc'
	};


	enum : Type
	{
		kGoblinSkullNone			= 0,
		kGoblinSkullRandom			= 1,
		kGoblinSkullGoat			= 'GOAT',
		kGoblinSkullDeer			= 'DEER',
		kGoblinSkullCount			= 2
	};


	enum
	{
		kGoblinLeapAttack			= 1 << 0
	};


	class GoblinController;
	class GenerateGoblinMethod;
	class GetGoblinCountMethod;


	class Goblin
	{
		private:

			ControllerReg<GoblinController>		goblinControllerRegistration;
			ModelRegistration					goblinModelRegistration;
			ModelRegistration					goblinRagdollModelRegistration;
			ModelRegistration					goatSkullModelRegistration;
			ModelRegistration					deerSkullModelRegistration;

			MethodReg<GenerateGoblinMethod>		generateGoblinRegistration;
			MethodReg<GetGoblinCountMethod>		getGoblinCountRegistration;

			Goblin();
			~Goblin();

		public:

			static void Construct(void);
			static void Destruct(void);
	};


	class GoblinConfiguration
	{
		protected:

			unsigned_int32		goblinFlags;
			Type				goblinSkull;

			GoblinConfiguration();
			GoblinConfiguration(const GoblinConfiguration& goblinConfiguration);

			void Pack(Packer& data) const;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data);

		public:

			int32 GetSettingCount(void) const;
			Setting *GetSetting(int32 index) const;
			void SetSetting(const Setting *setting);
	};


	class GoblinController final : public MonsterController, public GoblinConfiguration
	{
		private:

			enum 
			{
				kGoblinStateSleeping,
				kGoblinStateStanding, 
				kGoblinStateCreeping,
				kGoblinStateChasing, 
				kGoblinStatePreleaping,
				kGoblinStateLeaping,
				kGoblinStateLanding, 
				kGoblinStateAttacking,
				kGoblinStateRecovering, 
				kGoblinStateDead, 
				kGoblinStateCount
			};

			enum 
			{
				kGoblinMotionNone,
				kGoblinMotionStand,
				kGoblinMotionCreep,
				kGoblinMotionChase,
				kGoblinMotionLeap,
				kGoblinMotionLand,
				kGoblinMotionAttack,
				kGoblinMotionDamage,
				kGoblinMotionDeath
			};

			FrameAnimatorObserver<GoblinController>		frameAnimatorObserver;

			int32					soundTime;
			int32					waitTime;
			float					leapDistance;

			static int32			globalGoblinCount;
			static bool				(GoblinController::*goblinStateHandler[kGoblinStateCount])(void);

			GoblinController(const GoblinController& goblinController);

			Controller *Replicate(void) const override;

			void NewSoundTime(void);
			void NewLeapDistance(void);

			bool HandleSleepingState(void);
			bool HandleStandingState(void);
			bool HandleCreepingState(void);
			bool HandleChasingState(void);
			bool HandlePreleapingState(void);
			bool HandleLeapingState(void);
			bool HandleLandingState(void);
			bool HandleAttackingState(void);
			bool HandleRecoveringState(void);
			bool HandleDeadState(void);

			void HandleAnimationEvent(FrameAnimator *animator, CueType cueType);

			void SetGoblinMotion(int32 motion);

			static void LandCallback(Interpolator *interpolator, void *cookie);
			static void AttackCallback(Interpolator *interpolator, void *cookie);
			static void DamageCallback(Interpolator *interpolator, void *cookie);

		public:

			GoblinController();
			GoblinController(const GoblinConfiguration& configuration);
			~GoblinController();

			static int32 GetGlobalGoblinCount(void)
			{
				return (globalGoblinCount);
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

			RigidBodyStatus HandleNewRigidBodyContact(const RigidBodyContact *contact, RigidBodyController *contactBody) override;

			CharacterStatus Damage(Fixed damage, unsigned_int32 flags, GameCharacterController *attacker, const Point3D *position, const Vector3D *force) override;
			void Kill(GameCharacterController *attacker, const Point3D *position, const Vector3D *force) override;
	};


	class GenerateGoblinMethod final : public GenerateMonsterMethod, public GoblinConfiguration
	{
		private:

			GenerateGoblinMethod(const GenerateGoblinMethod& generateGoblinMethod);

			Method *Replicate(void) const override;

		public:

			GenerateGoblinMethod();
			~GenerateGoblinMethod();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(const ScriptState *state) override;
	};


	class GetGoblinCountMethod final : public Method
	{
		private:

			GetGoblinCountMethod(const GetGoblinCountMethod& getGoblinCountMethod);

			Method *Replicate(void) const override;

		public:

			GetGoblinCountMethod();
			~GetGoblinCountMethod();

			void Execute(const ScriptState *state) override;
	};
}


#endif

// ZYUQURM
