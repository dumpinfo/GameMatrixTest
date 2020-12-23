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


#ifndef MGSkeleton_h
#define MGSkeleton_h


#include "C4Lights.h"
#include "C4Effects.h"
#include "MGMonster.h"


namespace C4
{
	enum : ControllerType
	{
		kControllerSkeleton			= 'skel'
	};


	enum : ModelType
	{
		kModelSkeleton				= 'skel',
		kModelSkull					= 'skul',
		kModelRibs					= 'ribs',
		kModelFemur					= 'femr',
		kModelHumerus				= 'humr',
		kModelShield				= 'shld',
		kModelSword					= 'swrd',
		kModelPipe					= 'pipe',
		kModelBone					= 'bone',
		kModelBranch				= 'brch',
		kModelRomanHelmet			= 'romn',
		kModelVikingHelmet1			= 'vik1',
		kModelVikingHelmet2			= 'vik2',
		kModelVikingHelmet3			= 'vik3'
	};


	enum : MethodType
	{
		kMethodGenerateSkeleton		= 'skel',
		kMethodGetSkeletonCount		= 'sklc'
	};


	enum : Type
	{
		kSkeletonPoseNone			= 0,
		kSkeletonPoseWakeup			= 'WAKE',
		kSkeletonPoseRest1			= 'RST1',
		kSkeletonPoseRest2			= 'RST2',
		kSkeletonPoseRest3			= 'RST3',
		kSkeletonPoseRest4			= 'RST4',
		kSkeletonPoseRest5			= 'RST5',
		kSkeletonPoseCount			= 6
	};


	enum : Type
	{
		kSkeletonHelmetNone			= 0,
		kSkeletonHelmetRandom		= 1,
		kSkeletonHelmetRoman		= 'ROMN',
		kSkeletonHelmetViking		= 'VIKG',
		kSkeletonHelmetCount		= 2
	};


	enum : Type
	{
		kSkeletonWeaponNone			= 0,
		kSkeletonWeaponRandom		= 1,
		kSkeletonWeaponSword		= 'SWRD',
		kSkeletonWeaponPipe			= 'PIPE',
		kSkeletonWeaponBone			= 'BONE',
		kSkeletonWeaponBranch		= 'BRCH',
		kSkeletonWeaponArm			= 'SARM',
		kSkeletonWeaponCount		= 5
	};


	enum
	{
		kSkeletonRiseFromGround		= 1 << 0,
		kSkeletonCarryShield		= 1 << 1,
		kSkeletonShowPatch			= 1 << 2,
		kSkeletonShowHook			= 1 << 3,
		kSkeletonMorphSkull			= 1 << 4
	};


	enum
	{
		kSkeletonBrokenArm			= 1 << 0,
		kSkeletonMorphingSkull		= 1 << 1
	};


	enum
	{
		kSkeletonDeathSound			= 1 << 0
	};
 

	class SkeletonController;
	class GenerateSkeletonMethod; 
	class GetSkeletonCountMethod;
 

	class Skeleton
	{ 
		private:
 
			ControllerReg<SkeletonController>	skeletonControllerRegistration; 

			ModelRegistration					skeletonModelRegistration;
			ModelRegistration					skullModelRegistration;
			ModelRegistration					ribsModelRegistration; 
			ModelRegistration					femurModelRegistration;
			ModelRegistration					humerusModelRegistration;
			ModelRegistration					shieldModelRegistration;
			ModelRegistration					swordModelRegistration;
			ModelRegistration					pipeModelRegistration;
			ModelRegistration					boneModelRegistration;
			ModelRegistration					branchModelRegistration;
			ModelRegistration					romanHelmetModelRegistration;
			ModelRegistration					vikingHelmet1ModelRegistration;
			ModelRegistration					vikingHelmet2ModelRegistration;
			ModelRegistration					vikingHelmet3ModelRegistration;

			MethodReg<GenerateSkeletonMethod>	generateSkeletonRegistration;
			MethodReg<GetSkeletonCountMethod>	getSkeletonCountRegistration;

			Skeleton();
			~Skeleton();

		public:

			static void Construct(void);
			static void Destruct(void);
	};


	class SkeletonConfiguration
	{
		protected:

			unsigned_int32		skeletonFlags;
			Type				skeletonPose;
			Type				skeletonHelmet;
			Type				skeletonWeapon;

			SkeletonConfiguration();
			SkeletonConfiguration(const SkeletonConfiguration& skeletonConfiguration);

			void Pack(Packer& data) const;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data);

		public:

			int32 GetSettingCount(void) const;
			Setting *GetSetting(int32 index) const;
			void SetSetting(const Setting *setting);
	};


	class SkeletonController final : public MonsterController, public SkeletonConfiguration
	{
		private:

			enum
			{
				kSkeletonStateSleeping,
				kSkeletonStateStanding,
				kSkeletonStateWaking,
				kSkeletonStateChasing,
				kSkeletonStateBreakingArm,
				kSkeletonStateAttacking,
				kSkeletonStateRecovering,
				kSkeletonStateCount
			};

			enum
			{
				kSkeletonMotionNone,
				kSkeletonMotionStand,
				kSkeletonMotionRise,
				kSkeletonMotionWake,
				kSkeletonMotionChase,
				kSkeletonMotionBreakArm,
				kSkeletonMotionAttack1,
				kSkeletonMotionAttack2,
				kSkeletonMotionDamage
			};

			FrameAnimatorObserver<SkeletonController>	frameAnimatorObserver;

			unsigned_int32			skeletonState;
			Fixed					skeletonDamage;

			int32					waitTime;
			float					breakArmDistance;

			float					skullMorphWeight;
			MorphController			*skullMorphController;

			Node					*leftArmNode;
			const Node				*patchNode;
			const Node				*hookNode;

			Model					*weaponModel;

			Light					*skeletonLight;
			QuadEffect				*ribcageQuadEffect;
			QuadEffect				*eyeQuadEffect[2];

			List<Attribute>			poseAttributeList;
			EmissionAttribute		poseEmissionAttribute;

			List<Attribute>			pirateAttributeList;
			DiffuseAttribute		pirateDiffuseAttribute;

			static int32			globalSkeletonCount;
			static unsigned_int32	globalSkeletonFlags;
			static bool				(SkeletonController::*skeletonStateHandler[kSkeletonStateCount])(void);

			SkeletonController(const SkeletonController& skeletonController);

			Controller *Replicate(void) const override;

			void SetInitialPose(void);

			bool HandleSleepingState(void);
			bool HandleStandingState(void);
			bool HandleWakingState(void);
			bool HandleChasingState(void);
			bool HandleBreakingArmState(void);
			bool HandleAttackingState(void);
			bool HandleRecoveringState(void);
			bool HandleDeadState(void);

			void HandleAnimationEvent(FrameAnimator *animator, CueType cueType);

			void SetSkeletonMotion(int32 motion);

			static void WakeCallback(Interpolator *interpolator, void *cookie);
			static void BreakArmCallback(Interpolator *interpolator, void *cookie);
			static void AttackCallback(Interpolator *interpolator, void *cookie);
			static void DamageCallback(Interpolator *interpolator, void *cookie);

		public:

			SkeletonController();
			SkeletonController(const SkeletonConfiguration& configuration);
			~SkeletonController();

			static int32 GetGlobalSkeletonCount(void)
			{
				return (globalSkeletonCount);
			}

			static void ResetGlobalSkeletonFlags(void)
			{
				globalSkeletonFlags = 0;
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


	class GenerateSkeletonMethod final : public GenerateMonsterMethod, public SkeletonConfiguration
	{
		private:

			GenerateSkeletonMethod(const GenerateSkeletonMethod& generateSkeletonMethod);

			Method *Replicate(void) const override;

		public:

			GenerateSkeletonMethod();
			~GenerateSkeletonMethod();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(const ScriptState *state) override;
	};


	class GetSkeletonCountMethod final : public Method
	{
		private:

			GetSkeletonCountMethod(const GetSkeletonCountMethod& getSkeletonCountMethod);

			Method *Replicate(void) const override;

		public:

			GetSkeletonCountMethod();
			~GetSkeletonCountMethod();

			void Execute(const ScriptState *state) override;
	};
}


#endif

// ZYUQURM
