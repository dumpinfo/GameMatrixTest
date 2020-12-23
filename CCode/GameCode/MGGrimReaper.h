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


#ifndef MGGrimReaper_h
#define MGGrimReaper_h


#include "MGMonster.h"


namespace C4
{
	enum : ControllerType
	{
		kControllerGrimReaper			= 'reap'
	};


	enum : ModelType
	{
		kModelGrimReaper				= 'reap'
	};


	enum : MethodType
	{
		kMethodGenerateGrimReaper		= 'reap',
		kMethodGetGrimReaperCount		= 'repc'
	};


	class GrimReaperController;
	class GenerateGrimReaperMethod;
	class GetGrimReaperCountMethod;


	class GrimReaper
	{
		private:

			ControllerReg<GrimReaperController>		grimReaperControllerRegistration;
			ModelRegistration						grimReaperModelRegistration;

			MethodReg<GenerateGrimReaperMethod>		generateGrimReaperRegistration;
			MethodReg<GetGrimReaperCountMethod>		getGrimReaperCountRegistration;

			GrimReaper();
			~GrimReaper();

		public:

			static void Construct(void);
			static void Destruct(void);
	};


	class GrimReaperConfiguration
	{
		protected:

			unsigned_int32		grimReaperFlags;

			GrimReaperConfiguration();
			GrimReaperConfiguration(const GrimReaperConfiguration& grimReaperConfiguration);

			void Pack(Packer& data) const;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data);

		public:

			int32 GetSettingCount(void) const;
			Setting *GetSetting(int32 index) const;
			void SetSetting(const Setting *setting);
	};


	class GrimReaperController final : public MonsterController, public GrimReaperConfiguration
	{
		private:

			enum
			{
				kGrimReaperStateSleeping,
				kGrimReaperStateStanding,
				kGrimReaperStateDead,
				kGrimReaperStateCount
			};

			enum
			{
				kGrimReaperMotionNone,
				kGrimReaperMotionStand,
				kGrimReaperMotionDeath
			};

			FrameAnimatorObserver<GrimReaperController>		frameAnimatorObserver;

			static int32		globalGrimReaperCount;
			static bool			(GrimReaperController::*grimReaperStateHandler[kGrimReaperStateCount])(void);

			GrimReaperController(const GrimReaperController& grimReaperController);

			Controller *Replicate(void) const override;
 
			bool HandleSleepingState(void);
			bool HandleStandingState(void);
			bool HandleDeadState(void); 

			void HandleAnimationEvent(FrameAnimator *animator, CueType cueType); 

			void SetGrimReaperMotion(int32 motion);
 
		public:
 
			GrimReaperController(); 
			GrimReaperController(const GrimReaperConfiguration& configuration);
			~GrimReaperController();

			static int32 GetGlobalGrimReaperCount(void) 
			{
				return (globalGrimReaperCount);
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


	class GenerateGrimReaperMethod final : public GenerateMonsterMethod, public GrimReaperConfiguration
	{
		private:

			GenerateGrimReaperMethod(const GenerateGrimReaperMethod& generateGrimReaperMethod);

			Method *Replicate(void) const override;

		public:

			GenerateGrimReaperMethod();
			~GenerateGrimReaperMethod();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(const ScriptState *state) override;
	};


	class GetGrimReaperCountMethod final : public Method
	{
		private:

			GetGrimReaperCountMethod(const GetGrimReaperCountMethod& getGrimReaperCountMethod);

			Method *Replicate(void) const override;

		public:

			GetGrimReaperCountMethod();
			~GetGrimReaperCountMethod();

			void Execute(const ScriptState *state) override;
	};
}


#endif

// ZYUQURM
