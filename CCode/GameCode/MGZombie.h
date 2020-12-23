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


#ifndef MGZombie_h
#define MGZombie_h


#include "C4Effects.h"
#include "MGMonster.h"


namespace C4
{
	enum : ControllerType
	{
		kControllerZombie			= 'zomb'
	};


	enum : ModelType
	{
		kModelZombie				= 'zomb',
		kModelZombieHardHat			= 'zhat'
	};


	enum : MaterialType
	{
		kMaterialZombieTechnician	= 'ztch',
		kMaterialZombieScientist	= 'zsci'
	};


	enum : MethodType
	{
		kMethodGenerateZombie		= 'zomb',
		kMethodGetZombieCount		= 'zmbc'
	};


	enum : EffectType
	{
		kEffectOpenGrave			= 'ogrv'
	};


	enum : Type
	{
		kZombieVariantNormal		= 'NRML',
		kZombieVariantTechnician	= 'TECH',
		kZombieVariantScientist		= 'SCNT',
		kZombieVariantCount			= 3
	};


	enum : Type
	{
		kZombiePoseNone				= 0,
		kZombiePoseWakeup			= 'WAKE',
		kZombiePoseRest1			= 'RST1',
		kZombiePoseRest2			= 'RST2',
		kZombiePoseRest3			= 'RST3',
		kZombiePoseRest4			= 'RST4',
		kZombiePoseCount			= 5
	};


	enum
	{
		kZombieRiseFromGrave		= 1 << 0,
		kZombieFastChase			= 1 << 1,
		kZombieReachOut				= 1 << 2,
		kZombieDirtInhibit			= 1 << 3,
		kZombieHardHat				= 1 << 4
	};


	class ZombieController;
	class GenerateZombieMethod;
	class GetZombieCountMethod;


	class Zombie
	{
		private:

			ControllerReg<ZombieController>		zombieControllerRegistration;
			ModelRegistration					zombieModelRegistration;
			ModelRegistration					zombieHardHatModelRegistration;

			MaterialRegistration				technicianMaterialRegistration;
			MaterialRegistration				scientistMaterialRegistration;

			MethodReg<GenerateZombieMethod>		generateZombieRegistration;
			MethodReg<GetZombieCountMethod>		getZombieCountRegistration;

			Zombie();
			~Zombie();

		public:

			static void Construct(void);
			static void Destruct(void);
	}; 


	class ZombieConfiguration 
	{
		protected: 

			unsigned_int32		zombieFlags;
			Type				zombieVariant; 
			Type				zombiePose;
 
			ZombieConfiguration(); 
			ZombieConfiguration(const ZombieConfiguration& zombieConfiguration);

			void Pack(Packer& data) const;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data); 

		public:

			int32 GetSettingCount(void) const;
			Setting *GetSetting(int32 index) const;
			void SetSetting(const Setting *setting);
	};


	class ZombieController final : public MonsterController, public ZombieConfiguration
	{
		private:

			enum
			{
				kZombieStateSleeping,
				kZombieStateStanding,
				kZombieStateWaking,
				kZombieStateChasing,
				kZombieStateAttacking,
				kZombieStateRecovering,
				kZombieStateDead,
				kZombieStateCount
			};

			enum
			{
				kZombieMotionNone,
				kZombieMotionStand,
				kZombieMotionRise,
				kZombieMotionWake,
				kZombieMotionChase,
				kZombieMotionAttack,
				kZombieMotionDamage,
				kZombieMotionHeadDamage,
				kZombieMotionDeath
			};

			FrameAnimatorObserver<ZombieController>		frameAnimatorObserver;

			float				zombiePropelForce;
			float				zombieTurnRate;

			int32				soundTime;
			int32				waitTime;

			static int32		globalZombieCount;
			static bool			(ZombieController::*zombieStateHandler[kZombieStateCount])(void);

			ZombieController(const ZombieController& zombieController);

			Controller *Replicate(void) const override;

			void NewSoundTime(void);
			void SetInitialPose(void);

			bool HandleSleepingState(void);
			bool HandleStandingState(void);
			bool HandleWakingState(void);
			bool HandleChasingState(void);
			bool HandleAttackingState(void);
			bool HandleRecoveringState(void);
			bool HandleDeadState(void);

			void HandleAnimationEvent(FrameAnimator *animator, CueType cueType);

			void SetZombieMotion(int32 motion);

			static void WakeCallback(Interpolator *interpolator, void *cookie);
			static void AttackCallback(Interpolator *interpolator, void *cookie);
			static void DamageCallback(Interpolator *interpolator, void *cookie);

		public:

			ZombieController();
			ZombieController(const ZombieConfiguration& configuration);
			~ZombieController();

			static int32 GetGlobalZombieCount(void)
			{
				return (globalZombieCount);
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


	class GenerateZombieMethod final : public GenerateMonsterMethod, public ZombieConfiguration
	{
		private:

			GenerateZombieMethod(const GenerateZombieMethod& generateZombieMethod);

			Method *Replicate(void) const override;

		public:

			GenerateZombieMethod();
			~GenerateZombieMethod();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(const ScriptState *state) override;
	};


	class GetZombieCountMethod final : public Method
	{
		private:

			GetZombieCountMethod(const GetZombieCountMethod& getZombieCountMethod);

			Method *Replicate(void) const override;

		public:

			GetZombieCountMethod();
			~GetZombieCountMethod();

			void Execute(const ScriptState *state) override;
	};


	class OpenGraveEffect final : public Effect
	{
		private:

			struct GraveVertex
			{
				Point2D		position;
				Vector3D	normal;
				Vector3D	tangent;
			};

			int32							lifeTime;

			float							effectFrame;
			int32							effectAngle;

			static SharedVertexBuffer		staticVertexBuffer;
			VertexBuffer					dynamicVertexBuffer;
			List<Attribute>					attributeList;
			DiffuseTextureAttribute			textureAttribute;

			OpenGraveEffect(const OpenGraveEffect& openGraveEffect);

			void Initialize(void);

			Node *Replicate(void) const override;

			bool CalculateBoundingBox(Box3D *box) const override;
			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

			void UpdateTexcoords(float frame);

		public:

			OpenGraveEffect();
			~OpenGraveEffect();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Preprocess(void) override;
			void Move(void) override;
	};
}


#endif

// ZYUQURM
