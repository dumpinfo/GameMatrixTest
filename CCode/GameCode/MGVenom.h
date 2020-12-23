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


#ifndef MGVenom_h
#define MGVenom_h


#include "C4Sources.h"
#include "C4Particles.h"
#include "MGWeapons.h"
#include "MGMultiplayer.h"


namespace C4
{
	enum : ControllerType
	{
		kControllerVenom				= 'venm'
	};


	enum : ModelType
	{
		kModelVenom						= 'venm'
	};


	enum : ParticleSystemType
	{
		kParticleSystemVenom			= 'venm',
		kParticleSystemVenomTrail		= 'vntr'
	};


	class VenomTrailParticleSystem : public PointParticleSystem
	{
		private:

			enum
			{
				kMaxParticleCount	= 160
			};

			ParticlePool<>		particlePool;
			Particle			particleArray[kMaxParticleCount];

		public:

			VenomTrailParticleSystem();
			~VenomTrailParticleSystem();

			void Finalize(void)
			{
				SetParticleSystemFlags(GetParticleSystemFlags() | kParticleSystemSelfDestruct);
			}

			void Preprocess(void) override;
			void AnimateParticles(void) override;

			void NewParticle(const Point3D& position, const Vector3D& velocity);
	};


	class Venom : public BlobParticleSystem
	{
		private:

			enum
			{
				kMaxParticleCount	= 5
			};

			ParticlePool<BlobParticle>		particlePool;
			BlobParticle					particleArray[kMaxParticleCount];

			Venom(const Venom& venom);

			Node *Replicate(void) const override;

		public:

			Venom();
			~Venom();

			void Preprocess(void) override;
			void AnimateParticles(void) override;
	};


	class VenomController final : public ProjectileController
	{
		friend class Game;

		private:

			int32							lifeTime;
			float							trailDistance;

			bool							teleportFlag;

			VenomTrailParticleSystem		*venomTrail;

			VenomController();
			VenomController(const VenomController& venomController); 

			Controller *Replicate(void) const override;
 
			static void VenomTrailLinkProc(Node *node, void *cookie);
 
		public:

			enum 
			{
				kVenomMessageTeleport = kRigidBodyMessageBaseCount, 
				kVenomMessageExplode 
			};

			VenomController(const Vector3D& velocity, GameCharacterController *attacker);
			~VenomController(); 

			void SetTeleportFlag(void)
			{
				teleportFlag = true;
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

			void Destroy(const Point3D& position, const Vector3D& normal) override;
	};


	class CreateVenomMessage : public CreateModelMessage
	{
		friend class CreateModelMessage;

		private:

			Vector3D		initialVelocity;
			int32			attackerIndex;

			CreateVenomMessage();

		public:

			CreateVenomMessage(int32 projectileIndex, int32 characterIndex, const Point3D& position, const Vector3D& velocity);
			~CreateVenomMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class VenomTeleportMessage : public ControllerMessage
	{
		friend class VenomController;

		private:

			Point3D			teleportPosition;
			Vector3D		teleportVelocity;
			Point3D			effectCenter;

			VenomTeleportMessage(int32 controllerIndex);

		public:

			VenomTeleportMessage(int32 controllerIndex, const Point3D& position, const Vector3D& velocity, const Point3D& center);
			~VenomTeleportMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			void HandleControllerMessage(Controller *controller) const;
	};


	class VenomExplodeMessage : public ControllerMessage
	{
		friend class VenomController;

		private:

			Point3D			explodePosition;
			Vector3D		explodeNormal;

			VenomExplodeMessage(int32 controllerIndex);

		public:

			VenomExplodeMessage(int32 controllerIndex, const Point3D& position, const Vector3D& normal);
			~VenomExplodeMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			void HandleControllerMessage(Controller *controller) const;
	};
}


#endif

// ZYUQURM
