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


#ifndef MGWitch_h
#define MGWitch_h


#include "MGMonster.h"
#include "MGMagicBroom.h"


namespace C4
{
	enum : ControllerType
	{
		kControllerWitch			= 'wtch'
	};


	enum : ModelType
	{
		kModelWitch					= 'wtch',
		kModelWitchHat				= 'what',
		kModelWitchCat				= 'wcat'
	};


	enum : MaterialType
	{
		kMaterialWitchMaster		= 'wmst'
	};


	enum : MethodType
	{
		kMethodGenerateWitch		= 'wtch',
		kMethodGetWitchCount		= 'wchc'
	};


	enum : Type
	{
		kWitchVariantNormal			= 'NRML',
		kWitchVariantMaster			= 'MAST',
		kWitchVariantCount			= 2
	};


	enum
	{
		kWitchFlying				= 1 << 0,
		kWitchHat					= 1 << 1
	};


	enum
	{
		kWitchAttackCat				= 1 << 0,
		kWitchAttackAxe				= 1 << 1,
		kWitchAttackKnife			= 1 << 2,
		kWitchAttackPotion			= 1 << 3
	};


	class WitchController;
	class GenerateWitchMethod;
	class GetWitchCountMethod;


	class Witch
	{
		private:

			ControllerReg<WitchController>		witchControllerRegistration;

			ModelRegistration					witchModelRegistration;
			ModelRegistration					witchHatModelRegistration;
			ModelRegistration					witchCatModelRegistration;

			MaterialRegistration				masterMaterialRegistration;

			MethodReg<GenerateWitchMethod>		generateWitchRegistration;
			MethodReg<GetWitchCountMethod>		getWitchCountRegistration;

			Witch();
			~Witch();

		public:

			static void Construct(void);
			static void Destruct(void);
	};


	class WitchConfiguration
	{
		protected:

			unsigned_int32		witchFlags;
			Type				witchVariant;

			unsigned_int32		attackMask;

			WitchConfiguration(); 
			WitchConfiguration(const WitchConfiguration& witchConfiguration);

			void Pack(Packer& data) const; 
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data);
 
		public:

			int32 GetSettingCount(void) const; 
			Setting *GetSetting(int32 index) const;
			void SetSetting(const Setting *setting); 
	}; 


	class WitchController final : public MonsterController, public WitchConfiguration
	{ 
		private:

			enum
			{
				kWitchStateSleeping,
				kWitchStateStanding,
				kWitchStateFlying,
				kWitchStateChasing,
				kWitchStateAttacking,
				kWitchStateThrowing,
				kWitchStateFlyThrowing,
				kWitchStateRecovering,
				kWitchStateDead,
				kWitchStateCount
			};

			enum
			{
				kWitchMotionNone,
				kWitchMotionStand,
				kWitchMotionFly,
				kWitchMotionChase,
				kWitchMotionAttack,
				kWitchMotionThrow,
				kWitchMotionFlyThrow,
				kWitchMotionDamage,
				kWitchMotionDeath
			};

			FrameAnimatorObserver<WitchController>		frameAnimatorObserver;

			int32						throwTime;
			int32						waitTime;

			float						targetRoll;
			float						currentRoll;

			BroomSmokeParticleSystem	*broomSmokeParticleSystem;
			MagicBroomParticleSystem	*magicBroomParticleSystem;

			Node						*broomNode;
			OmniSource					*witchSource;
			Model						*blackCatModel;

			static int32				globalWitchCount;
			static bool					(WitchController::*witchStateHandler[kWitchStateCount])(void);

			WitchController(const WitchController& witchController);

			Controller *Replicate(void) const override;

			static void BroomSmokeLinkProc(Node *node, void *cookie);
			static void MagicBroomLinkProc(Node *node, void *cookie);

			void NewThrowTime(void);

			bool HandleSleepingState(void);
			bool HandleStandingState(void);
			bool HandleFlyingState(void);
			bool HandleChasingState(void);
			bool HandleAttackingState(void);
			bool HandleThrowingState(void);
			bool HandleRecoveringState(void);
			bool HandleDeadState(void);

			void HandleAnimationEvent(FrameAnimator *animator, CueType cueType);

			void SetWitchMotion(int32 motion);

			static void AttackCallback(Interpolator *interpolator, void *cookie);
			static void ThrowCallback(Interpolator *interpolator, void *cookie);
			static void FlyThrowCallback(Interpolator *interpolator, void *cookie);
			static void DamageCallback(Interpolator *interpolator, void *cookie);

		public:

			WitchController();
			WitchController(const WitchConfiguration& configuration);
			~WitchController();

			static int32 GetGlobalWitchCount(void)
			{
				return (globalWitchCount);
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


	class GenerateWitchMethod final : public GenerateMonsterMethod, public WitchConfiguration
	{
		private:

			GenerateWitchMethod(const GenerateWitchMethod& generateWitchMethod);

			Method *Replicate(void) const override;

		public:

			GenerateWitchMethod();
			~GenerateWitchMethod();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(const ScriptState *state) override;
	};


	class GetWitchCountMethod final : public Method
	{
		private:

			GetWitchCountMethod(const GetWitchCountMethod& getWitchCountMethod);

			Method *Replicate(void) const override;

		public:

			GetWitchCountMethod();
			~GetWitchCountMethod();

			void Execute(const ScriptState *state) override;
	};
}


#endif

// ZYUQURM
