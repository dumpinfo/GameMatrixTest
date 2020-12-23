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


#ifndef MGGolem_h
#define MGGolem_h


#include "MGMonster.h"
#include "MGTarball.h"
#include "MGLavaball.h"


namespace C4
{
	enum : ControllerType
	{
		kControllerGolem		= 'golm'
	};


	enum : ModelType
	{
		kModelGolem				= 'golm'
	};


	enum : MaterialType
	{
		kMaterialGolemTar		= 'gtar',
		kMaterialGolemMud		= 'gmud',
		kMaterialGolemSwamp		= 'gswp',
		kMaterialGolemToxic		= 'gtox',
		kMaterialGolemLava		= 'glav'
	};


	enum : MethodType
	{
		kMethodGenerateGolem	= 'golm',
		kMethodGetGolemCount	= 'golc'
	};


	enum : Type
	{
		kGolemVariantTar		= 'TAR ',
		kGolemVariantMud		= 'MUD ',
		kGolemVariantSwamp		= 'SWMP',
		kGolemVariantToxic		= 'TOXC',
		kGolemVariantLava		= 'LAVA',
		kGolemVariantCount		= 5
	};


	class GolemController;
	class GenerateGolemMethod;
	class GetGolemCountMethod;


	class Golem
	{
		private:

			ControllerReg<GolemController>		golemControllerRegistration;
			ModelRegistration					golemModelRegistration;

			ParticleSystemReg<Tarball>			tarballParticleSystemRegistration;
			ModelRegistration					tarballModelReg;

			ParticleSystemReg<Lavaball>			lavaballParticleSystemRegistration;
			ModelRegistration					lavaballModelReg;

			MaterialRegistration				tarMaterialRegistration;
			MaterialRegistration				mudMaterialRegistration;
			MaterialRegistration				swampMaterialRegistration;
			MaterialRegistration				toxicMaterialRegistration;
			MaterialRegistration				lavaMaterialRegistration;

			MethodReg<GenerateGolemMethod>		generateGolemRegistration;
			MethodReg<GetGolemCountMethod>		getGolemCountRegistration;

			Golem();
			~Golem();

		public:

			static void Construct(void);
			static void Destruct(void);
	};


	class GolemConfiguration
	{
		protected:

			unsigned_int32		golemFlags;
			Type				golemVariant;

			GolemConfiguration();
			GolemConfiguration(const GolemConfiguration& golemConfiguration);

			void Pack(Packer& data) const;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data);
 
		public:

			int32 GetSettingCount(void) const; 
			Setting *GetSetting(int32 index) const;
			void SetSetting(const Setting *setting); 
	};

 
	class GolemController final : public MonsterController, public GolemConfiguration
	{ 
		private: 

			enum
			{
				kGolemStateSleeping, 
				kGolemStateStanding,
				kGolemStateRising,
				kGolemStateThrowing,
				kGolemStateRecovering,
				kGolemStateDead,
				kGolemStateCount
			};

			enum
			{
				kGolemMotionNone,
				kGolemMotionStand,
				kGolemMotionRise,
				kGolemMotionThrow1,
				kGolemMotionThrow2,
				kGolemMotionDamage,
				kGolemMotionDeath
			};

			int32				soundTime;
			int32				throwTime;

			Node				*ballNode[2];

			FrameAnimatorObserver<GolemController>		frameAnimatorObserver;

			static int32		globalGolemCount;
			static bool			(GolemController::*golemStateHandler[kGolemStateCount])(void);

			GolemController(const GolemController& golemController);

			Controller *Replicate(void) const override;

			void NewSoundTime(void);
			void NewThrowTime(void);

			bool HandleSleepingState(void);
			bool HandleStandingState(void);
			bool HandleRisingState(void);
			bool HandleThrowingState(void);
			bool HandleRecoveringState(void);
			bool HandleDeadState(void);

			void HandleAnimationEvent(FrameAnimator *animator, CueType cueType);

			void SetGolemMotion(int32 motion);

			static void RiseCallback(Interpolator *interpolator, void *cookie);
			static void ThrowCallback(Interpolator *interpolator, void *cookie);
			static void DamageCallback(Interpolator *interpolator, void *cookie);

		public:

			GolemController();
			GolemController(const GolemConfiguration& configuration);
			~GolemController();

			static int32 GetGlobalGolemCount(void)
			{
				return (globalGolemCount);
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


	class GenerateGolemMethod final : public GenerateMonsterMethod, public GolemConfiguration
	{
		private:

			GenerateGolemMethod(const GenerateGolemMethod& generateGolemMethod);

			Method *Replicate(void) const override;

		public:

			GenerateGolemMethod();
			~GenerateGolemMethod();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(const ScriptState *state) override;
	};


	class GetGolemCountMethod final : public Method
	{
		private:

			GetGolemCountMethod(const GetGolemCountMethod& getGolemCountMethod);

			Method *Replicate(void) const override;

		public:

			GetGolemCountMethod();
			~GetGolemCountMethod();

			void Execute(const ScriptState *state) override;
	};
}


#endif

// ZYUQURM
