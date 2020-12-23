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


#ifndef MGRigidBody_h
#define MGRigidBody_h


#include "C4Physics.h"
#include "MGBase.h"


namespace C4
{
	enum : ControllerType
	{
		kControllerGameRigidBody	= 'body',
		kControllerRemains			= 'rmns',
		kControllerExplosive		= 'expl',
		kControllerCrumble			= 'crmb'
	};


	enum : RigidBodyType
	{
		kRigidBodyGame				= 'game'
	};


	enum : FunctionType
	{
		kFunctionBreakApart			= 'brak',
		kFunctionExplode			= 'expl',
		kFunctionCrumble			= 'crmb'
	};


	enum
	{
		kGameRigidBodyBreaking		= 1 << 0
	};


	enum
	{
		kBreakApartSoundInhibit		= 1 << 0
	};


	class GameRigidBodyController : public RigidBodyController, public LinkTarget<GameRigidBodyController>
	{
		public:

			typedef Type	GameRigidBodyType;

			enum
			{
				kGameRigidBodyNone			= 0,
				kGameRigidBodyPumpkin		= 'pump',
				kGameRigidBodyWatermelon	= 'wmln',
				kGameRigidBodyCardboardBox	= 'cbox',
				kGameRigidBodyPropaneTank	= 'prop',
				kGameRigidBodyHornetNest	= 'hrnt',
				kGameRigidBodyTypeCount		= 5
			};

			static const GameRigidBodyType gameRigidBodyTypeTable[kGameRigidBodyTypeCount];

		private:

			GameRigidBodyType		gameRigidBodyType;
			unsigned_int32			gameRigidBodyState;

			Controller *Replicate(void) const override;

		protected:

			GameRigidBodyController(ControllerType type);
			GameRigidBodyController(const GameRigidBodyController& gameRigidBodyController);

		public:

			enum
			{
				kGameRigidBodyMessageBreakApart = kRigidBodyMessageBaseCount,
				kGameRigidBodyMessageLeakGoo,
				kGameRigidBodyMessageBaseCount
			};

			GameRigidBodyController();
			~GameRigidBodyController();

			static void RegisterFunctions(ControllerRegistration *registration);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			ControllerMessage *CreateMessage(ControllerMessageType type) const override;

			void BreakApart(unsigned_int32 flags, Node *initiator = nullptr, Node *trigger = nullptr); 

			virtual void HandleSplashDamage(const Point3D& center, float radius, float damage, GameCharacterController *attacker);
	}; 

 
	class BreakApartFunction final : public Function
	{
		private: 

			BreakApartFunction(const BreakApartFunction& breakApartFunction); 
 
			Function *Replicate(void) const override;

		public:
 
			BreakApartFunction();
			~BreakApartFunction();

			void Execute(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
	};


	class BreakApartMessage : public ControllerMessage
	{
		friend class GameRigidBodyController;

		public:

			enum
			{
				kMaxPieceCount = 20
			};

			struct PieceData
			{
				int32		pieceIndex;
				int32		controllerIndex;
				Vector3D	initialVelocity;
			};

		private:

			unsigned_int32		breakApartFlags;

			int32				pieceCount;
			PieceData			pieceData[kMaxPieceCount];

			BreakApartMessage(int32 index);

		public:

			BreakApartMessage(int32 index, unsigned_int32 flags);
			~BreakApartMessage();

			void SetPieceCount(int32 count)
			{
				pieceCount = count;
			}

			PieceData *GetPieceDataArray(void)
			{
				return (pieceData);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			void HandleControllerMessage(Controller *controller) const;
	};


	class LeakGooMessage : public ControllerMessage
	{
		friend class GameRigidBodyController;

		private:

			Point3D		leakPosition;
			Vector3D	leakNormal;

			LeakGooMessage(int32 index);

		public:

			LeakGooMessage(int32 index, const Point3D& position, const Vector3D& normal);
			~LeakGooMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			void HandleControllerMessage(Controller *controller) const;
	};


	class RemainsController : public RigidBodyController
	{
		friend class Game;

		private:

			int32			lifeTime;
			Vector4D		parameterValue;

			RemainsController();

			static const float *GetShaderParameterData(int32 slot, void *cookie);

		protected:

			RemainsController(ControllerType type, int32 life);

		public:

			RemainsController(int32 life);
			~RemainsController();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Preprocess(void) override;
			void Move(void) override;
	};


	class ExplosiveController : public GameRigidBodyController
	{
		friend class ExplodeFunction;

		private:

			Fixed							explosiveHealth;
			int32							explodeDamage;
			int32							explodeTime;

			float							shakeIntensity;
			int32							shakeDuration;

			Link<GameCharacterController>	attackerController;

			ExplosiveController(const ExplosiveController& explosiveController);

			Controller *Replicate(void) const override;

		public:

			enum
			{
				kExplosiveMessageExplode = kGameRigidBodyMessageBaseCount
			};

			ExplosiveController();
			~ExplosiveController();

			static void RegisterFunctions(ControllerRegistration *registration);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			ControllerMessage *CreateMessage(ControllerMessageType type) const override;
			void ReceiveMessage(const ControllerMessage *message) override;

			void Move(void) override;

			void HandleSplashDamage(const Point3D& center, float radius, float damage, GameCharacterController *attacker) override;
			void Damage(Fixed damage, float squaredDistance, GameCharacterController *attacker);
			void Explode(void);
	};


	class ExplodeFunction final : public Function
	{
		private:

			ExplodeFunction(const ExplodeFunction& explodeFunction);

			Function *Replicate(void) const override;

		public:

			ExplodeFunction();
			~ExplodeFunction();

			void Execute(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
	};


	class CrumbleController : public GameRigidBodyController
	{
		private:

			Fixed		crumbleHealth;

			CrumbleController(const CrumbleController& crumbleController);

			Controller *Replicate(void) const override;

		public:

			CrumbleController();
			~CrumbleController();

			static void RegisterFunctions(ControllerRegistration *registration);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void HandleSplashDamage(const Point3D& center, float radius, float damage, GameCharacterController *attacker) override;
	};


	class CrumbleFunction final : public Function
	{
		private:

			CrumbleFunction(const CrumbleFunction& crumbleFunction);

			Function *Replicate(void) const override;

		public:

			CrumbleFunction();
			~CrumbleFunction();

			void Execute(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
	};
}


#endif

// ZYUQURM
