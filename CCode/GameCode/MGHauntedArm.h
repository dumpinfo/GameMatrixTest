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


#ifndef MGHauntedArm_h
#define MGHauntedArm_h


#include "MGMonster.h"


namespace C4
{
	enum : ControllerType
	{
		kControllerHauntedArm		= 'harm'
	};


	enum : ModelType
	{
		kModelHauntedArm			= 'harm'
	};


	enum : MethodType
	{
		kMethodGenerateHauntedArm	= 'harm',
		kMethodGetHauntedArmCount	= 'armc'
	};


	enum
	{
		kHauntedArmAttacked			= 1 << 0
	};


	class HauntedArmController;
	class GenerateHauntedArmMethod;
	class GetHauntedArmCountMethod;


	class HauntedArm
	{
		private:

			ControllerReg<HauntedArmController>		hauntedArmControllerRegistration;
			ModelRegistration						hauntedArmModelRegistration;

			MethodReg<GenerateHauntedArmMethod>		generateHauntedArmRegistration;
			MethodReg<GetHauntedArmCountMethod>		getHauntedArmCountRegistration;

			HauntedArm();
			~HauntedArm();

		public:

			static void Construct(void);
			static void Destruct(void);
	};


	class HauntedArmConfiguration
	{
		protected:

			unsigned_int32		hauntedArmFlags;

			HauntedArmConfiguration();
			HauntedArmConfiguration(const HauntedArmConfiguration& hauntedArmConfiguration);

			void Pack(Packer& data) const;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data);

		public:

			int32 GetSettingCount(void) const;
			Setting *GetSetting(int32 index) const;
			void SetSetting(const Setting *setting);
	};


	class HauntedArmController final : public MonsterController, public HauntedArmConfiguration
	{
		private:

			enum
			{
				kHauntedArmStateSleeping,
				kHauntedArmStateChasing,
				kHauntedArmStateAttacking,
				kHauntedArmStateFalling,
				kHauntedArmStateCount
			};

			enum
			{
				kHauntedArmMotionNone,
				kHauntedArmMotionStand,
				kHauntedArmMotionChase,
				kHauntedArmMotionAttack
			};

			FrameAnimatorObserver<HauntedArmController>		frameAnimatorObserver; 

			unsigned_int32			hauntedArmState;
 
			float					attackDistance;
 
			static int32			globalHauntedArmCount;
			static bool				(HauntedArmController::*hauntedArmStateHandler[kHauntedArmStateCount])(void);
 
			HauntedArmController(const HauntedArmController& hauntedArmController);
 
			Controller *Replicate(void) const override; 

			void NewAttackDistance(void);

			bool HandleSleepingState(void); 
			bool HandleChasingState(void);
			bool HandleAttackingState(void);
			bool HandleFallingState(void);

			void HandleAnimationEvent(FrameAnimator *animator, CueType cueType);

			void SetHauntedArmMotion(int32 motion);

		public:

			HauntedArmController();
			HauntedArmController(const HauntedArmConfiguration& configuration);
			~HauntedArmController();

			static int32 GetGlobalHauntedArmCount(void)
			{
				return (globalHauntedArmCount);
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


	class GenerateHauntedArmMethod final : public GenerateMonsterMethod, public HauntedArmConfiguration
	{
		private:

			GenerateHauntedArmMethod(const GenerateHauntedArmMethod& generateHauntedArmMethod);

			Method *Replicate(void) const override;

		public:

			GenerateHauntedArmMethod();
			~GenerateHauntedArmMethod();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(const ScriptState *state) override;
	};


	class GetHauntedArmCountMethod final : public Method
	{
		private:

			GetHauntedArmCountMethod(const GetHauntedArmCountMethod& getHauntedArmCountMethod);

			Method *Replicate(void) const override;

		public:

			GetHauntedArmCountMethod();
			~GetHauntedArmCountMethod();

			void Execute(const ScriptState *state) override;
	};
}


#endif

// ZYUQURM
