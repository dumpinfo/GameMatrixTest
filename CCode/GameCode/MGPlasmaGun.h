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


#ifndef MGPlasmaGun_h
#define MGPlasmaGun_h


#include "C4Particles.h"
#include "MGWeapons.h"
#include "MGInput.h"
#include "MGMultiplayer.h"


namespace C4
{
	enum : ControllerType
	{
		kControllerPlasmaGun				= 'pgun',
		kControllerPlasma					= 'plas'
	};


	enum : ModelType
	{
		kModelPlasmaGun						= 'pgun',
		kModelPlasma						= 'plas',
		kModelPlasmaAmmo					= 'plam'
	};


	enum : ParticleSystemType
	{
		kParticleSystemPlasmaExplosion		= 'plex'
	};


	class PlasmaController;


	class PlasmaGun final : public Weapon
	{
		private:

			ControllerReg<PlasmaController>		plasmaControllerRegistration;

			ModelRegistration					plasmaGunModelRegistration;
			ModelRegistration					plasmaAmmoModelRegistration;
			ModelRegistration					plasmaModelRegistration;

			WeaponAction						plasmaGunAction;

			Texture								*plasmaScorchTexture;

			PlasmaGun();
			~PlasmaGun();

		public:

			static void Construct(void);
			static void Destruct(void);

			WeaponController *NewWeaponController(FighterController *fighter) const;
	};


	class PlasmaGunController final : public WeaponController
	{
		public:

			PlasmaGunController(FighterController *fighter);
			~PlasmaGunController();

			WeaponResult UpdateWeapon(const Point3D& position, const Vector3D& direction, const Point3D& center) override;
	};


	class PlasmaExplosionParticleSystem : public LineParticleSystem
	{
		private:

			enum
			{
				kMaxParticleCount = 128
			};

			ParticlePool<>		particlePool;
			Particle			particleArray[kMaxParticleCount];

			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		public:

			PlasmaExplosionParticleSystem();
			~PlasmaExplosionParticleSystem();

			void Preprocess(void) override;
			void AnimateParticles(void) override;
	};


	class PlasmaController final : public ProjectileController
	{
		friend class ControllerReg<PlasmaController>;
 
		private:

			int32		lifeTime; 

			PlasmaController(); 
			PlasmaController(const PlasmaController& plasmaController);

			Controller *Replicate(void) const override; 

		public: 
 
			enum
			{
				kPlasmaMessageTeleport = kRigidBodyMessageBaseCount,
				kPlasmaMessageExplode 
			};

			PlasmaController(const Vector3D& velocity, GameCharacterController *attacker);
			~PlasmaController();

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


	class CreatePlasmaMessage : public CreateModelMessage
	{
		friend class CreateModelMessage;

		private:

			Vector3D		initialVelocity;
			int32			attackerIndex;

			CreatePlasmaMessage();

		public:

			CreatePlasmaMessage(int32 projectileIndex, int32 characterIndex, const Point3D& position, const Vector3D& velocity);
			~CreatePlasmaMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class PlasmaTeleportMessage : public ControllerMessage
	{
		friend class PlasmaController;

		private:

			Point3D			teleportPosition;
			Vector3D		teleportVelocity;
			Point3D			effectCenter;

			PlasmaTeleportMessage(int32 controllerIndex);

		public:

			PlasmaTeleportMessage(int32 controllerIndex, const Point3D& position, const Vector3D& velocity, const Point3D& center);
			~PlasmaTeleportMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			void HandleControllerMessage(Controller *controller) const;
	};


	class PlasmaExplodeMessage : public ControllerMessage
	{
		friend class PlasmaController;

		private:

			Point3D			explodePosition;
			Vector3D		explodeNormal;

			PlasmaExplodeMessage(int32 controllerIndex);

		public:

			PlasmaExplodeMessage(int32 controllerIndex, const Point3D& position, const Vector3D& normal);
			~PlasmaExplodeMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			void HandleControllerMessage(Controller *controller) const;
	};
}


#endif

// ZYUQURM
