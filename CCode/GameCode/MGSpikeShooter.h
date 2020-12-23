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


#ifndef MGSpikeShooter_h
#define MGSpikeShooter_h


#include "MGWeapons.h"
#include "MGInput.h"
#include "MGMultiplayer.h"


namespace C4
{
	enum : ControllerType
	{
		kControllerSpikeShooter		= 'spsh',
		kControllerSpike			= 'spik',
		kControllerRailSpike		= 'rspk'
	};


	enum : ModelType
	{
		kModelSpikeShooter			= 'spsh',
		kModelSpike					= 'spik',
		kModelRailSpike				= 'rspk',
		kModelSpikeAmmo				= 'spam',
		kModelRailSpikeAmmo			= 'rsam',
		kModelComboSpikeAmmo		= 'csam'
	};


	enum
	{
		kSpikeEffectNone,
		kSpikeEffectDust,
		kSpikeEffectSparks
	};


	class SpikeController;
	class RailSpikeController;


	class SpikeShooter final : public Weapon
	{
		private:

			ControllerReg<SpikeController>		spikeControllerRegistration;
			ControllerReg<RailSpikeController>	railSpikeControllerRegistration;

			ModelRegistration					spikeShooterModelRegistration;
			ModelRegistration					spikeAmmoModelRegistration;
			ModelRegistration					railSpikeAmmoModelRegistration;
			ModelRegistration					comboSpikeAmmoModelRegistration;
			ModelRegistration					spikeModelRegistration;
			ModelRegistration					railSpikeModelRegistration;

			WeaponAction						spikeShooterAction;

			SpikeShooter();
			~SpikeShooter();

		public:

			static void Construct(void);
			static void Destruct(void);

			WeaponController *NewWeaponController(FighterController *fighter) const;
	};


	class SpikeShooterController final : public WeaponController
	{
		private:

			int32				fireCount;

			OmniSource			*motorSource;
			FrameAnimator		frameAnimator;

			static void FireCallback(Interpolator *interpolator, void *cookie);

		public:

			enum
			{
				kSpikeShooterMessageRecoil = kWeaponMessageBaseCount
			};

			SpikeShooterController(FighterController *fighter);
			~SpikeShooterController();

			void Preprocess(void) override;

			ControllerMessage *CreateMessage(ControllerMessageType type) const override;

			void Move(void) override;

			void ShowFireAnimation(void);

			WeaponResult UpdateWeapon(const Point3D& position, const Vector3D& direction, const Point3D& center) override;
	}; 


	class SpikeShooterRecoilMessage : public ControllerMessage 
	{
		public: 

			SpikeShooterRecoilMessage(int32 index);
			~SpikeShooterRecoilMessage(); 

			void HandleControllerMessage(Controller *controller) const; 
	}; 


	class SpikeController : public ProjectileController
	{ 
		friend class ControllerReg<SpikeController>;

		private:

			int32		lifeTime;

			SpikeController();

			Controller *Replicate(void) const override;

		protected:

			Fixed		spikeDamage;
			float		spikeImpulse;

			SpikeController(ControllerType type);
			SpikeController(ControllerType type, const Vector3D& velocity, GameCharacterController *attacker);
			SpikeController(const SpikeController& spikeController);

		public:

			enum
			{
				kSpikeMessageTeleport = kRigidBodyMessageBaseCount,
				kSpikeMessageDestroy
			};

			SpikeController(const Vector3D& velocity, GameCharacterController *attacker);
			~SpikeController();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Preprocess(void) override;

			ControllerMessage *CreateMessage(ControllerMessageType type) const override;
			void SendInitialStateMessages(Player *player) const override;
			void Move(void) override;

			RigidBodyStatus HandleNewRigidBodyContact(const RigidBodyContact *contact, RigidBodyController *contactBody) override;
			RigidBodyStatus HandleNewGeometryContact(const GeometryContact *contact) override;

			void EnterWorld(World *world, const Point3D& worldPosition) override;
			void Destroy(const Point3D& position, const Vector3D& normal) override;
	};


	class RailSpikeController final : public SpikeController
	{
		friend class ControllerReg<RailSpikeController>;

		private:

			RailSpikeController();
			RailSpikeController(const RailSpikeController& railSpikeController);

			Controller *Replicate(void) const override;

		public:

			RailSpikeController(const Vector3D& velocity, GameCharacterController *attacker);
			~RailSpikeController();

			void SendInitialStateMessages(Player *player) const override;

			void EnterWorld(World *world, const Point3D& worldPosition) override;
	};


	class CreateSpikeMessage : public CreateModelMessage
	{
		friend class CreateModelMessage;

		private:

			Vector3D		initialVelocity;
			int32			attackerIndex;

			CreateSpikeMessage();

		public:

			CreateSpikeMessage(int32 projectileIndex, int32 characterIndex, const Point3D& position, const Vector3D& velocity);
			~CreateSpikeMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class CreateRailSpikeMessage : public CreateModelMessage
	{
		friend class CreateModelMessage;

		private:

			Vector3D		initialVelocity;
			int32			attackerIndex;

			CreateRailSpikeMessage();

		public:

			CreateRailSpikeMessage(int32 projectileIndex, int32 characterIndex, const Point3D& position, const Vector3D& velocity);
			~CreateRailSpikeMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class SpikeTeleportMessage : public ControllerMessage
	{
		friend class SpikeController;

		private:

			Point3D			teleportPosition;
			Vector3D		teleportVelocity;
			Point3D			effectCenter;

			SpikeTeleportMessage(int32 controllerIndex);

		public:

			SpikeTeleportMessage(int32 controllerIndex, const Point3D& position, const Vector3D& velocity, const Point3D& center);
			~SpikeTeleportMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			void HandleControllerMessage(Controller *controller) const;
	};


	class SpikeDestroyMessage : public ControllerMessage
	{
		friend class SpikeController;

		private:

			Point3D			destroyPosition;
			Vector3D		destroyNormal;
			SubstanceType	destroySubstance;

			SpikeDestroyMessage(int32 controllerIndex);

		public:

			SpikeDestroyMessage(int32 controllerIndex, const Point3D& position, const Vector3D& normal, SubstanceType substance);
			~SpikeDestroyMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			void HandleControllerMessage(Controller *controller) const;
	};
}


#endif

// ZYUQURM
