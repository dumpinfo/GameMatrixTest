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


#ifndef MGPumpkinhead_h
#define MGPumpkinhead_h


#include "C4Deformable.h"
#include "MGMonster.h"


namespace C4
{
	enum : ControllerType
	{
		kControllerPumpkinhead			= 'pmkn'
	};


	enum : ModelType
	{
		kModelPumpkinhead				= 'pmkn',
		kModelPumpkin1					= 'pmk1',
		kModelPumpkin2					= 'pmk2',
		kModelWatermelon				= 'wmln',
		kModelCardboardBox				= 'cbox',
		kModelPropaneTank				= 'prop',
		kModelPumpkinheadFireball		= 'phfb'
	};


	enum : MaterialType
	{
		kMaterialPumpkinBurn			= 'pbrn',
		kMaterialWatermelonBurn			= 'wbrn'
	};


	enum : MethodType
	{
		kMethodGeneratePumpkinhead		= 'pmkn',
		kMethodGetPumpkinheadCount		= 'pmkc'
	};


	enum : Type
	{
		kPumpkinheadHeadPumpkin			= 'PMKN',
		kPumpkinheadHeadWatermelon		= 'WMLN',
		kPumpkinheadHeadCardboardBox	= 'CBOX',
		kPumpkinheadHeadPropaneTank		= 'PROP',
		kPumpkinheadHeadCount			= 4
	};


	enum
	{
		kPumpkinheadDoubleThrow			= 1 << 0,
		kPumpkinheadRiseFromGround		= 1 << 1
	};


	class PumpkinheadController;
	class GeneratePumpkinheadMethod;
	class GetPumpkinheadCountMethod;


	class Pumpkinhead
	{
		private:

			ControllerReg<PumpkinheadController>	pumpkinheadControllerRegistration;

			ModelRegistration						pumpkinheadModelRegistration;
			ModelRegistration						pumpkin1ModelRegistration;
			ModelRegistration						pumpkin2ModelRegistration;
			ModelRegistration						watermelonModelRegistration;
			ModelRegistration						cardboardBoxModelRegistration;
			ModelRegistration						propaneTankModelRegistration;
			ModelRegistration						pumpkinheadFireballModelRegistration;

			MaterialRegistration					pumpkinBurnMaterialRegistration;
			MaterialRegistration					watermelonBurnMaterialRegistration;

			MethodReg<GeneratePumpkinheadMethod>	generatePumpkinheadRegistration;
			MethodReg<GetPumpkinheadCountMethod>	getPumpkinheadCountRegistration;

			Pumpkinhead();
			~Pumpkinhead();

		public:

			static void Construct(void);
			static void Destruct(void);
	};


	class PumpkinheadConfiguration
	{
		protected:

			unsigned_int32		pumpkinheadFlags;
			Type				pumpkinheadHead;
 
			PumpkinheadConfiguration();
			PumpkinheadConfiguration(const PumpkinheadConfiguration& pumpkinheadConfiguration);
 
			void Pack(Packer& data) const;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data); 

		public:
 
			int32 GetSettingCount(void) const;
			Setting *GetSetting(int32 index) const; 
			void SetSetting(const Setting *setting); 
	};


	class PumpkinheadController final : public MonsterController, public PumpkinheadConfiguration 
	{
		private:

			enum
			{
				kPumpkinheadStateSleeping,
				kPumpkinheadStateStanding,
				kPumpkinheadStateWaking,
				kPumpkinheadStateChasing,
				kPumpkinheadStateAttacking,
				kPumpkinheadStateThrowing1,
				kPumpkinheadStateThrowing2,
				kPumpkinheadStateRecovering,
				kPumpkinheadStateDead,
				kPumpkinheadStateCount
			};

			enum
			{
				kPumpkinheadMotionNone,
				kPumpkinheadMotionStand,
				kPumpkinheadMotionRise,
				kPumpkinheadMotionChase,
				kPumpkinheadMotionAttack,
				kPumpkinheadMotionIgnite,
				kPumpkinheadMotionThrow1,
				kPumpkinheadMotionThrow2,
				kPumpkinheadMotionDamage,
				kPumpkinheadMotionDeath
			};

			FrameAnimatorObserver<PumpkinheadController>	frameAnimatorObserver;

			float				riseOffset;
			float				riseVelocity;

			int32				throwTime;
			int32				throwCount;
			int32				waitTime;

			Node				*rootBone;
			Geometry			*headFull;
			Geometry			*headLeft;
			Geometry			*headRight;
			Geometry			*headBottom;

			Model				*fireballModel[2];
			ClothController		*capeController;

			static int32		globalPumpkinheadCount;
			static bool			(PumpkinheadController::*pumpkinheadStateHandler[kPumpkinheadStateCount])(void);

			PumpkinheadController(const PumpkinheadController& pumpkinheadController);

			Controller *Replicate(void) const override;

			void NewThrowTime(void);

			bool HandleSleepingState(void);
			bool HandleStandingState(void);
			bool HandleWakingState(void);
			bool HandleChasingState(void);
			bool HandleAttackingState(void);
			bool HandleThrowing1State(void);
			bool HandleThrowing2State(void);
			bool HandleRecoveringState(void);
			bool HandleDeadState(void);

			void HandleAnimationEvent(FrameAnimator *animator, CueType cueType);

			void StartDoubleThrow(void);
			void ThrowFireball(int32 hand);

			void SetPumpkinheadMotion(int32 motion);

			void BreakOffNode(Geometry *geometry, float direction);

			static void WakeCallback(Interpolator *interpolator, void *cookie);
			static void AttackCallback(Interpolator *interpolator, void *cookie);
			static void IgniteCallback(Interpolator *interpolator, void *cookie);
			static void Throw1Callback(Interpolator *interpolator, void *cookie);
			static void Throw2Callback(Interpolator *interpolator, void *cookie);
			static void DamageCallback(Interpolator *interpolator, void *cookie);

		public:

			PumpkinheadController();
			PumpkinheadController(const PumpkinheadConfiguration& configuration);
			~PumpkinheadController();

			static int32 GetGlobalPumpkinheadCount(void)
			{
				return (globalPumpkinheadCount);
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


	class GeneratePumpkinheadMethod final : public GenerateMonsterMethod, public PumpkinheadConfiguration
	{
		private:

			GeneratePumpkinheadMethod(const GeneratePumpkinheadMethod& generatePumpkinheadMethod);

			Method *Replicate(void) const override;

		public:

			GeneratePumpkinheadMethod();
			~GeneratePumpkinheadMethod();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(const ScriptState *state) override;
	};


	class GetPumpkinheadCountMethod final : public Method
	{
		private:

			GetPumpkinheadCountMethod(const GetPumpkinheadCountMethod& getPumpkinheadCountMethod);

			Method *Replicate(void) const override;

		public:

			GetPumpkinheadCountMethod();
			~GetPumpkinheadCountMethod();

			void Execute(const ScriptState *state) override;
	};
}


#endif

// ZYUQURM
