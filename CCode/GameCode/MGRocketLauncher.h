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


#ifndef MGRocketLauncher_h
#define MGRocketLauncher_h


#include "C4Sources.h"
#include "MGWeapons.h"
#include "MGEffects.h"
#include "MGInput.h"
#include "MGMultiplayer.h"


namespace C4
{
	enum : ControllerType
	{
		kControllerRocketLauncher			= 'rlch',
		kControllerRocket					= 'rock'
	};


	enum : ModelType
	{
		kModelRocketLauncher				= 'rlch',
		kModelRocket						= 'rock',
		kModelRocketAmmo					= 'rkam'
	};


	enum : ParticleSystemType
	{
		kParticleSystemRocketGlow			= 'rkgl',
		kParticleSystemRocketExplosion		= 'rkex',
		kParticleSystemRocketSparks			= 'rksp'
	};


	class RocketController;


	class RocketLauncher final : public Weapon
	{
		private:

			ControllerReg<RocketController>		rocketControllerRegistration;

			ModelRegistration					rocketLauncherModelRegistration;
			ModelRegistration					rocketAmmoModelRegistration;
			ModelRegistration					rocketModelRegistration;

			WeaponAction						rocketLauncherAction;

			RocketLauncher();
			~RocketLauncher();

		public:

			static void Construct(void);
			static void Destruct(void);

			WeaponController *NewWeaponController(FighterController *fighter) const;
	};


	class RocketLauncherController final : public WeaponController
	{
		private:

			Node		*drumNode;
			float		currentDrumAngle;
			float		targetDrumAngle;

		public:

			enum
			{
				kRocketLauncherMessageAdvance = kWeaponMessageBaseCount
			};

			RocketLauncherController(FighterController *fighter);
			~RocketLauncherController();

			void ShowFireAnimation(void)
			{
				targetDrumAngle += 1.0F;
			}

			void Preprocess(void) override;

			ControllerMessage *CreateMessage(ControllerMessageType type) const override;

			void Move(void) override;

			WeaponResult UpdateWeapon(const Point3D& position, const Vector3D& direction, const Point3D& center) override;
	};


	class RocketLauncherAdvanceMessage : public ControllerMessage
	{
		public:

			RocketLauncherAdvanceMessage(int32 index); 
			~RocketLauncherAdvanceMessage();

			void HandleControllerMessage(Controller *controller) const; 
	};
 

	class RocketExplosionParticleSystem : public TexcoordPointParticleSystem
	{ 
		private:
 
			enum 
			{
				kMaxParticleCount = 768
			};
 
			ParticlePool<SmokeParticle>		particlePool;
			SmokeParticle					particleArray[kMaxParticleCount];

			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		public:

			RocketExplosionParticleSystem();
			~RocketExplosionParticleSystem();

			void Preprocess(void) override;
			void AnimateParticles(void) override;
	};


	class RocketSparksParticleSystem : public LineParticleSystem
	{
		private:

			enum
			{
				kMaxParticleCount = 64
			};

			ParticlePool<>		particlePool;
			Particle			particleArray[kMaxParticleCount];

			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		public:

			RocketSparksParticleSystem();
			~RocketSparksParticleSystem();

			void Preprocess(void) override;
			void AnimateParticles(void) override;
	};


	class RocketController final : public ProjectileController
	{
		friend class ControllerReg<RocketController>;

		private:

			int32						lifeTime;
			float						smokeDistance;
			bool						teleportFlag;

			SmokeTrailParticleSystem	*smokeTrail;
			OmniSource					*rocketSource;

			RocketController();
			RocketController(const RocketController& rocketController);

			Controller *Replicate(void) const override;

			static void SmokeTrailLinkProc(Node *node, void *cookie);

		public:

			enum
			{
				kRocketMessageTeleport = kRigidBodyMessageBaseCount,
				kRocketMessageExplode
			};

			RocketController(const Vector3D& velocity, GameCharacterController *attacker);
			~RocketController();

			void SetTeleportFlag(void)
			{
				teleportFlag = true;
			}

			void SetRocketSourceVelocity(const Vector3D& velocity)
			{
				rocketSource->SetSourceVelocity(velocity);
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
	};


	class CreateRocketMessage : public CreateModelMessage
	{
		friend class CreateModelMessage;

		private:

			Vector3D		initialVelocity;
			int32			attackerIndex;

			CreateRocketMessage();

		public:

			CreateRocketMessage(int32 projectileIndex, int32 characterIndex, const Point3D& position, const Vector3D& velocity);
			~CreateRocketMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class RocketTeleportMessage : public ControllerMessage
	{
		friend class RocketController;

		private:

			Point3D			teleportPosition;
			Vector3D		teleportVelocity;
			Point3D			effectCenter;

			RocketTeleportMessage(int32 controllerIndex);

		public:

			RocketTeleportMessage(int32 controllerIndex, const Point3D& position, const Vector3D& velocity, const Point3D& center);
			~RocketTeleportMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			void HandleControllerMessage(Controller *controller) const;
	};


	class RocketExplodeMessage : public ControllerMessage
	{
		friend class RocketController;

		private:

			Point3D			explodePosition;
			Vector3D		explodeNormal;

			RocketExplodeMessage(int32 controllerIndex);

		public:

			RocketExplodeMessage(int32 controllerIndex, const Point3D& position, const Vector3D& normal);
			~RocketExplodeMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			void HandleControllerMessage(Controller *controller) const;
	};
}


#endif

// ZYUQURM
