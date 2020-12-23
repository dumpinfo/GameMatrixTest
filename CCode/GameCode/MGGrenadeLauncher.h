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


#ifndef MGGrenadeLauncher_h
#define MGGrenadeLauncher_h


#include "MGWeapons.h"
#include "MGEffects.h"
#include "MGInput.h"
#include "MGMultiplayer.h"


namespace C4
{
	enum : ControllerType
	{
		kControllerGrenadeLauncher			= 'glch',
		kControllerGrenade					= 'gren'
	};


	enum : ModelType
	{
		kModelGrenadeLauncher				= 'glch',
		kModelGrenade						= 'gren',
		kModelGrenadeAmmo					= 'gram'
	};


	enum : ParticleSystemType
	{
		kParticleSystemGrenadeExplosion		= 'grex',
		kParticleSystemGrenadeSparks		= 'grsp'
	};


	class GrenadeController;


	class GrenadeLauncher final : public Weapon
	{
		private:

			ControllerReg<GrenadeController>	grenadeControllerRegistration;

			ModelRegistration					grenadeLauncherModelRegistration;
			ModelRegistration					grenadeAmmoModelRegistration;
			ModelRegistration					grenadeModelRegistration;

			WeaponAction						grenadeLauncherAction;

			GrenadeLauncher();
			~GrenadeLauncher();

		public:

			static void Construct(void);
			static void Destruct(void);

			WeaponController *NewWeaponController(FighterController *fighter) const;
	};


	class GrenadeLauncherController final : public WeaponController
	{
		private:

			Node				*drumNode;
			float				currentDrumAngle;
			float				targetDrumAngle;

			FrameAnimator		frameAnimator;

		public:

			enum
			{
				kGrenadeLauncherMessageAdvance = kWeaponMessageBaseCount
			};

			GrenadeLauncherController(FighterController *fighter);
			~GrenadeLauncherController();

			void Preprocess(void) override;

			ControllerMessage *CreateMessage(ControllerMessageType type) const override;

			void Move(void) override;

			void ShowFireAnimation(void);

			WeaponResult UpdateWeapon(const Point3D& position, const Vector3D& direction, const Point3D& center) override;
	};


	class GrenadeLauncherAdvanceMessage : public ControllerMessage
	{
		public:

			GrenadeLauncherAdvanceMessage(int32 index);
			~GrenadeLauncherAdvanceMessage();

			void HandleControllerMessage(Controller *controller) const; 
	};

 
	class GrenadeExplosionParticleSystem : public TexcoordPointParticleSystem
	{ 
		private:

			enum 
			{
				kMaxParticleCount = 768 
			}; 

			Vector3D						explosionVelocity;

			ParticlePool<SmokeParticle>		particlePool; 
			SmokeParticle					particleArray[kMaxParticleCount];

			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		public:

			GrenadeExplosionParticleSystem(const Vector3D& velocity = Zero3D);
			~GrenadeExplosionParticleSystem();

			void Preprocess(void) override;
			void AnimateParticles(void) override;
	};


	class GrenadeSparksParticleSystem : public LineParticleSystem
	{
		private:

			enum
			{
				kMaxParticleCount = 64
			};

			Vector3D			sparksVelocity;

			ParticlePool<>		particlePool;
			Particle			particleArray[kMaxParticleCount];

			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		public:

			GrenadeSparksParticleSystem(const Vector3D& velocity = Zero3D);
			~GrenadeSparksParticleSystem();

			void Preprocess(void) override;
			void AnimateParticles(void) override;
	};


	class GrenadeController final : public ProjectileController
	{
		friend class ControllerReg<GrenadeController>;

		private:

			int32						lifeTime;
			int32						collisionCount;

			int32						smokeTime;
			SmokeTrailParticleSystem	*smokeTrail;

			GrenadeController();
			GrenadeController(const GrenadeController& grenadeController);

			Controller *Replicate(void) const override;

			static void SmokeTrailLinkProc(Node *node, void *cookie);

		public:

			enum
			{
				kGrenadeMessageTeleport = kRigidBodyMessageBaseCount,
				kGrenadeMessageExplode
			};

			GrenadeController(const Vector3D& velocity, GameCharacterController *attacker);
			~GrenadeController();

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

			void Bounce(void);
	};


	class CreateGrenadeMessage : public CreateModelMessage
	{
		friend class CreateModelMessage;

		private:

			Vector3D		initialVelocity;
			int32			attackerIndex;

			CreateGrenadeMessage();

		public:

			CreateGrenadeMessage(int32 projectileIndex, int32 characterIndex, const Point3D& position, const Vector3D& velocity);
			~CreateGrenadeMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class GrenadeTeleportMessage : public ControllerMessage
	{
		friend class GrenadeController;

		private:

			Point3D			teleportPosition;
			Vector3D		teleportVelocity;
			Point3D			effectCenter;

			GrenadeTeleportMessage(int32 controllerIndex);

		public:

			GrenadeTeleportMessage(int32 controllerIndex, const Point3D& position, const Vector3D& velocity, const Point3D& center);
			~GrenadeTeleportMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			void HandleControllerMessage(Controller *controller) const;
	};


	class GrenadeExplodeMessage : public ControllerMessage
	{
		friend class GrenadeController;

		private:

			Point3D			explodePosition;

			GrenadeExplodeMessage(int32 controllerIndex);

		public:

			GrenadeExplodeMessage(int32 controllerIndex, const Point3D& position);
			~GrenadeExplodeMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			void HandleControllerMessage(Controller *controller) const;
	};
}


#endif

// ZYUQURM
