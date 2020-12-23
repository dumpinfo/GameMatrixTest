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


#ifndef MGQuantumCharger_h
#define MGQuantumCharger_h


#include "C4Lights.h"
#include "C4Sources.h"
#include "C4Particles.h"
#include "MGWeapons.h"
#include "MGEffects.h"
#include "MGInput.h"
#include "MGMultiplayer.h"


namespace C4
{
	enum : ControllerType
	{
		kControllerQuantumCharger			= 'qchg',
		kControllerCharge					= 'chrg'
	};


	enum : ModelType
	{
		kModelQuantumCharger				= 'qchg',
		kModelCharge						= 'chrg',
		kModelChargeAmmo					= 'cham'
	};


	enum : ParticleSystemType
	{
		kParticleSystemQuantumLoop			= 'qlup',
		kParticleSystemQuantumBolt			= 'qblt',
		kParticleSystemQuantumSpider		= 'qspd',
		kParticleSystemQuantumWaves			= 'qwav',
		kParticleSystemCharge				= 'chrg',
		kParticleSystemChargeExplosion		= 'chex'
	};


	class ChargeController;


	class QuantumCharger final : public Weapon
	{
		private:

			ControllerReg<ChargeController>		chargeControllerRegistration;

			ModelRegistration					quantumChargerModelRegistration;
			ModelRegistration					chargeAmmoModelRegistration;
			ModelRegistration					chargeModelRegistration;

			WeaponAction						quantumChargerAction;

			Texture								*quantumScorchTexture;

			QuantumCharger();
			~QuantumCharger();

		public:

			static void Construct(void);
			static void Destruct(void);

			WeaponController *NewWeaponController(FighterController *fighter) const;
	};


	class QuantumLoopParticleSystem : public PolyboardParticleSystem
	{
		friend class Game;

		private:

			enum
			{
				kLoopCount				= 4,
				kLoopSegmentCount		= 8,
				kLoopParticleCount		= 64,
				kControlPointCount		= kLoopSegmentCount * 2,
				kMaxParticleCount		= kLoopCount * (kLoopParticleCount + 1),
				kSegmentParticleCount	= kLoopParticleCount / kLoopSegmentCount
			};

			enum
			{
				kQuantumLoopExplode		= 1 << 0
			};

			unsigned_int32					quantumLoopFlags;

			float							loopRadius;
			int32							lifeTime;

			Vector3D						rotationAxis[kLoopCount];
			Fixed							rotationVelocity[kLoopCount];
			UnsignedFixed					rotationAngle[kLoopCount];

			float							controlRadius[kLoopCount][kControlPointCount]; 
			Fixed							controlVelocity[kLoopCount][kControlPointCount];
			UnsignedFixed					controlAngle[kLoopCount][kControlPointCount];
 
			ParticlePool<PolyParticle>		particlePool;
			PolyParticle					particleArray[kMaxParticleCount]; 

			static const ConstColorRGBA		loopColor[kLoopCount];
 
			QuantumLoopParticleSystem();
 
			bool CalculateBoundingSphere(BoundingSphere *sphere) const override; 

		public:

			QuantumLoopParticleSystem(float radius); 
			~QuantumLoopParticleSystem();

			float GetLoopRadius(void) const
			{
				return (loopRadius);
			}

			void SetLoopRadius(float radius)
			{
				loopRadius = radius;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Preprocess(void) override;

			void Move(void) override;
			void AnimateParticles(void) override;

			void Explode(void);
	};


	class QuantumBoltParticleSystem : public PolyboardParticleSystem
	{
		friend class Game;

		private:

			enum
			{
				kMaxParticleCount = 32
			};

			Point3D							boltEndpoint[2];
			Vector3D						boltVelocity;
			int32							lifeTime;

			ParticlePool<PolyParticle>		particlePool;
			PolyParticle					particleArray[kMaxParticleCount];

			QuantumBoltParticleSystem();

			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		public:

			QuantumBoltParticleSystem(const Point3D& p1, const Point3D& p2, const Vector3D& velocity);
			~QuantumBoltParticleSystem();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Preprocess(void) override;

			void Move(void) override;
			void AnimateParticles(void) override;
	};


	class QuantumSpiderParticleSystem : public PolyboardParticleSystem
	{
		friend class Game;

		private:

			enum
			{
				kMaxParticleCount = 32
			};

			Point3D							boltEndpoint[2];
			Vector3D						boltNormal;
			int32							lifeTime;

			ParticlePool<PolyParticle>		particlePool;
			PolyParticle					particleArray[kMaxParticleCount];

			QuantumSpiderParticleSystem();

			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		public:

			QuantumSpiderParticleSystem(const Point3D& p1, const Point3D& p2, const Vector3D& normal, int32 life);
			~QuantumSpiderParticleSystem();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Preprocess(void) override;

			void Move(void) override;
			void AnimateParticles(void) override;
	};


	class QuantumWavesParticleSystem : public PointParticleSystem
	{
		private:

			enum
			{
				kMaxParticleCount = 128
			};

			ParticlePool<RotateParticle>	particlePool;
			RotateParticle					particleArray[kMaxParticleCount];

		public:

			QuantumWavesParticleSystem();
			~QuantumWavesParticleSystem();

			void Preprocess(void) override;
			void AnimateParticles(void) override;
	};


	class QuantumChargerController final : public WeaponController
	{
		private:

			enum
			{
				kQuantumChargerHold			= 1 << 0,
				kQuantumChargerRelease		= 1 << 1
			};

			unsigned_int32				chargeState;
			float						chargeLevel;
			float						chargeLimit;
			int32						chargeAmmo;

			Node						*coreNode;
			Matrix3D					coreTransform;
			float						coreAngle;
			float						coreSpeed;

			Geometry					*chargeNode;
			Light						*chargeLight;
			float						chargeSpeed;

			Effect						*chargeBeam;
			int32						beamTime;

			OmniSource					*buildSource;
			OmniSource					*holdSource;

			QuantumLoopParticleSystem	*quantumLoop;
			QuantumWavesParticleSystem	*quantumWaves;

			FrameAnimator				frameAnimator;

		public:

			enum
			{
				kQuantumChargerMessageBeam = kWeaponMessageBaseCount,
				kQuantumChargerMessageCharge,
				kQuantumChargerMessageRelease
			};

			QuantumChargerController(FighterController *fighter);
			~QuantumChargerController();

			void Preprocess(void) override;

			ControllerMessage *CreateMessage(ControllerMessageType type) const override;
			void SendInitialStateMessages(Player *player) const override;

			void Move(void) override;

			void BeginFiring(bool primary);
			void EndFiring(void);

			void ShowBeam(void);
			void BeginCharging(float level, float limit);
			void ReleaseCharge(void);

			WeaponResult UpdateWeapon(const Point3D& position, const Vector3D& direction, const Point3D& center) override;
	};


	class QuantumChargerBeamMessage : public ControllerMessage
	{
		public:

			QuantumChargerBeamMessage(int32 index);
			~QuantumChargerBeamMessage();

			void HandleControllerMessage(Controller *controller) const;
	};


	class QuantumChargerChargeMessage : public ControllerMessage
	{
		friend class QuantumChargerController;

		private:

			float		chargeLevel;
			float		chargeLimit;

			QuantumChargerChargeMessage(int32 index);

		public:

			QuantumChargerChargeMessage(int32 index, float level, float limit);
			~QuantumChargerChargeMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			void HandleControllerMessage(Controller *controller) const;
	};


	class QuantumChargerReleaseMessage : public ControllerMessage
	{
		public:

			QuantumChargerReleaseMessage(int32 index);
			~QuantumChargerReleaseMessage();

			void HandleControllerMessage(Controller *controller) const;
	};


	class ChargeExplosionParticleSystem : public LineParticleSystem
	{
		friend class Game;

		private:

			enum
			{
				kMaxParticleCount = 1024
			};

			int32				chargeLevel;

			ParticlePool<>		particlePool;
			Particle			particleArray[kMaxParticleCount];

			ChargeExplosionParticleSystem();

			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		public:

			ChargeExplosionParticleSystem(int32 level);
			~ChargeExplosionParticleSystem();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Preprocess(void) override;
			void AnimateParticles(void) override;
	};


	class ChargeController final : public ProjectileController
	{
		friend class ControllerReg<ChargeController>;

		private:

			enum
			{
				kChargeTransferLoop		= 1 << 0
			};

			unsigned_int32				chargeFlags;

			int32						lifeTime;
			int32						boltTime;
			int32						chargeLevel;

			QuantumLoopParticleSystem	*quantumLoop;
			OmniSource					*chargeSource;

			ChargeController();
			ChargeController(const ChargeController& chargeController);

			Controller *Replicate(void) const override;

			void NewBoltTime(void);

		public:

			enum
			{
				kChargeMessageTeleport = kRigidBodyMessageBaseCount,
				kChargeMessageExplode,
				kChargeMessageBolt
			};

			ChargeController(const Vector3D& velocity, int32 level, GameCharacterController *attacker);
			~ChargeController();

			void SetChargeSourceVelocity(const Vector3D& velocity)
			{
				chargeSource->SetSourceVelocity(velocity);
			}

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

			void TransferLoopEffect(World *world, const Point3D& position, const Vector3D& normal);
	};


	class CreateChargeMessage : public CreateModelMessage
	{
		friend class CreateModelMessage;

		private:

			Vector3D		initialVelocity;
			int32			attackerIndex;
			int32			chargeLevel;

			CreateChargeMessage();

		public:

			CreateChargeMessage(int32 projectileIndex, int32 characterIndex, const Point3D& position, const Vector3D& velocity, int32 level);
			~CreateChargeMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class ChargeTeleportMessage : public ControllerMessage
	{
		friend class ChargeController;

		private:

			Point3D			teleportPosition;
			Vector3D		teleportVelocity;
			Point3D			effectCenter;

			ChargeTeleportMessage(int32 controllerIndex);

		public:

			ChargeTeleportMessage(int32 controllerIndex, const Point3D& position, const Vector3D& velocity, const Point3D& center);
			~ChargeTeleportMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			void HandleControllerMessage(Controller *controller) const;
	};


	class ChargeExplodeMessage : public ControllerMessage
	{
		friend class ChargeController;

		private:

			Point3D			explodePosition;
			Vector3D		explodeNormal;
			int32			chargeLevel;
			bool			transferFlag;

			ChargeExplodeMessage(int32 controllerIndex);

		public:

			ChargeExplodeMessage(int32 controllerIndex, const Point3D& position, const Vector3D& normal, int32 level, bool transfer);
			~ChargeExplodeMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			void HandleControllerMessage(Controller *controller) const;
	};


	class ChargeBoltMessage : public ControllerMessage
	{
		friend class ChargeController;

		private:

			Point3D			boltPosition[2];

			ChargeBoltMessage(int32 controllerIndex);

		public:

			ChargeBoltMessage(int32 controllerIndex, const Point3D& p1, const Point3D& p2);
			~ChargeBoltMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			void HandleControllerMessage(Controller *controller) const;
	};
}


#endif

// ZYUQURM
