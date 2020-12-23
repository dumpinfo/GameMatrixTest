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


#ifndef MGSpider_h
#define MGSpider_h


#include "MGMonster.h"
#include "MGVenom.h"


namespace C4
{
	enum : ControllerType
	{
		kControllerSpider			= 'spid'
	};


	enum : ModelType
	{
		kModelSpider				= 'spid',
		kModelSpiderLeg				= 'sleg'
	};


	enum : MaterialType
	{
		kMaterialSpiderVenomous		= 'sven',
		kMaterialSpiderGuts			= 'sgut'
	};


	enum : MethodType
	{
		kMethodGenerateSpider		= 'spid',
		kMethodGetSpiderCount		= 'spdc'
	};


	enum : ParticleSystemType
	{
		kParticleSystemSpiderGuts	= 'sgut'
	};


	enum : Type
	{
		kSpiderVariantNormal		= 'NRML',
		kSpiderVariantVenomous		= 'VENM',
		kSpiderVariantCount			= 2
	};


	enum
	{
		kSpiderDeathSound			= 1 << 0
	};


	enum
	{
		kSpiderAttacked				= 1 << 0
	};


	class SpiderController;
	class GenerateSpiderMethod;
	class GetSpiderCountMethod;


	class Spider
	{
		private:

			ControllerReg<SpiderController>		spiderControllerRegistration;

			ModelRegistration					spiderModelRegistration;
			ModelRegistration					spiderLegModelRegistration;

			ParticleSystemReg<Venom>			venomParticleSystemRegistration;
			ModelRegistration					venomModelReg;

			MaterialRegistration				venomousMaterialRegistration;
			MaterialRegistration				gutsMaterialRegistration;

			MethodReg<GenerateSpiderMethod>		generateSpiderRegistration;
			MethodReg<GetSpiderCountMethod>		getSpiderCountRegistration;

			Spider();
			~Spider();

		public:

			static void Construct(void);
			static void Destruct(void);
	};


	class SpiderConfiguration
	{
		protected:

			unsigned_int32		spiderFlags; 
			Type				spiderVariant;

			SpiderConfiguration(); 
			SpiderConfiguration(const SpiderConfiguration& spiderConfiguration);
 
			void Pack(Packer& data) const;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data);
 
		public:
 
			int32 GetSettingCount(void) const; 
			Setting *GetSetting(int32 index) const;
			void SetSetting(const Setting *setting);
	};
 

	class SpiderController final : public MonsterController, public SpiderConfiguration
	{
		private:

			enum
			{
				kSpiderStateSleeping,
				kSpiderStateChasing,
				kSpiderStateAttacking,
				kSpiderStateJumping,
				kSpiderStateRecovering,
				kSpiderStateHurling,
				kSpiderStateCount
			};

			enum
			{
				kSpiderMotionNone,
				kSpiderMotionStand,
				kSpiderMotionChase,
				kSpiderMotionJump,
				kSpiderMotionLand,
				kSpiderMotionHurl
			};

			FrameAnimatorObserver<SpiderController>		frameAnimatorObserver;

			unsigned_int32			spiderState;

			int32					soundTime;
			int32					hurlTime;
			float					jumpDistance;

			const Node				*bodyNode;

			static int32			globalSpiderCount;
			static unsigned_int32	globalSpiderFlags;
			static bool				(SpiderController::*spiderStateHandler[kSpiderStateCount])(void);

			SpiderController(const SpiderController& spiderController);

			Controller *Replicate(void) const override;

			void NewSoundTime(void);
			void NewHurlTime(void);
			void NewJumpDistance(void);

			bool HandleSleepingState(void);
			bool HandleChasingState(void);
			bool HandleAttackingState(void);
			bool HandleJumpingState(void);
			bool HandleRecoveringState(void);
			bool HandleHurlingState(void);

			void HandleAnimationEvent(FrameAnimator *animator, CueType cueType);

			void SetSpiderMotion(int32 motion);

			static void RecoverCallback(Interpolator *interpolator, void *cookie);
			static void HurlCallback(Interpolator *interpolator, void *cookie);

		public:

			SpiderController();
			SpiderController(const SpiderConfiguration& configuration);
			~SpiderController();

			static int32 GetGlobalSpiderCount(void)
			{
				return (globalSpiderCount);
			}

			static void ResetGlobalSpiderFlags(void)
			{
				globalSpiderFlags = 0;
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


	class GenerateSpiderMethod final : public GenerateMonsterMethod, public SpiderConfiguration
	{
		private:

			GenerateSpiderMethod(const GenerateSpiderMethod& generateSpiderMethod);

			Method *Replicate(void) const override;

		public:

			GenerateSpiderMethod();
			~GenerateSpiderMethod();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(const ScriptState *state) override;
	};


	class GetSpiderCountMethod final : public Method
	{
		private:

			GetSpiderCountMethod(const GetSpiderCountMethod& getSpiderCountMethod);

			Method *Replicate(void) const override;

		public:

			GetSpiderCountMethod();
			~GetSpiderCountMethod();

			void Execute(const ScriptState *state) override;
	};


	class SpiderGutsParticleSystem : public BlobParticleSystem
	{
		friend class Game;

		private:

			enum
			{
				kMaxParticleCount = 32
			};

			MaterialType					gutsMaterial;
			ColorRGB						gutsColor;
			int32							gutsCount;

			ParticlePool<BlobParticle>		particlePool;
			BlobParticle					particleArray[kMaxParticleCount];

			SpiderGutsParticleSystem();

		public:

			SpiderGutsParticleSystem(MaterialType material, const ColorRGB& color, int32 count);
			~SpiderGutsParticleSystem();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Preprocess(void) override;
			void AnimateParticles(void) override;
	};
}


#endif

// ZYUQURM
