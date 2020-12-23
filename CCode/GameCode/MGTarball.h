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


#ifndef MGTarball_h
#define MGTarball_h


#include "C4Sources.h"
#include "C4Particles.h"
#include "MGWeapons.h"
#include "MGMultiplayer.h"


namespace C4
{
	enum : ControllerType
	{
		kControllerTarball					= 'trbl'
	};


	enum : ModelType
	{
		kModelTarball						= 'trbl'
	};


	enum : ParticleSystemType
	{
		kParticleSystemTarball				= 'trbl',
		kParticleSystemTarballTrail			= 'tbtr',
		kParticleSystemTarballExplosion		= 'tbex'
	};


	class GooSplashParticleSystem;


	struct TarballParticle : BlobParticle
	{
		Vector3D		rotationAxis;

		TarballParticle *GetPreviousParticle(void) const
		{
			return (static_cast<TarballParticle *>(prevParticle));
		}

		TarballParticle *GetNextParticle(void) const
		{
			return (static_cast<TarballParticle *>(nextParticle));
		}

		void Pack(Packer& data) const
		{
			BlobParticle::Pack(data);
			data << rotationAxis;
		}

		void Unpack(Unpacker& data)
		{
			BlobParticle::Unpack(data);
			data >> rotationAxis;
		}
	};


	class TarballTrailParticleSystem : public PointParticleSystem
	{
		private:

			enum
			{
				kMaxParticleCount	= 160
			};

			ParticlePool<>		particlePool;
			Particle			particleArray[kMaxParticleCount];

		public:

			TarballTrailParticleSystem();
			~TarballTrailParticleSystem();

			void Finalize(void)
			{
				SetParticleSystemFlags(GetParticleSystemFlags() | kParticleSystemSelfDestruct);
			}

			void Preprocess(void) override;
			void AnimateParticles(void) override;

			void NewParticle(const Point3D& position);
	};


	class TarballExplosionParticleSystem : public BlobParticleSystem
	{
		friend class Game;

		private:

			enum
			{
				kMaxParticleCount	= 8 
			};

			Vector3D						explosionVelocity; 
			Vector3D						explosionNormal;
			ColorRGB						splatterColor; 

			GooSplashParticleSystem			*gooSplash;
 
			ParticlePool<BlobParticle>		particlePool;
			BlobParticle					particleArray[kMaxParticleCount]; 
 
			TarballExplosionParticleSystem();

			static void GooSplashLinkProc(Node *node, void *cookie);
 
		public:

			TarballExplosionParticleSystem(const Vector3D& velocity, const Vector3D& normal);
			~TarballExplosionParticleSystem();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Preprocess(void) override;
			void AnimateParticles(void) override;
	};


	class Tarball : public BlobParticleSystem
	{
		private:

			enum
			{
				kMaxParticleCount	= 3
			};

			ParticlePool<TarballParticle>	particlePool;
			TarballParticle					particleArray[kMaxParticleCount];

			Tarball(const Tarball& tarball);

			Node *Replicate(void) const override;

		public:

			Tarball();
			~Tarball();

			void Preprocess(void) override;
			void AnimateParticles(void) override;
	};


	class TarballController final : public ProjectileController
	{
		friend class Game;

		private:

			int32							lifeTime;
			float							trailDistance;

			bool							teleportFlag;

			TarballTrailParticleSystem		*tarballTrail;
			OmniSource						*tarballSource;

			TarballController();
			TarballController(const TarballController& tarballController);

			Controller *Replicate(void) const override;

			static void TarballTrailLinkProc(Node *node, void *cookie);

		public:

			enum
			{
				kTarballMessageTeleport = kRigidBodyMessageBaseCount,
				kTarballMessageExplode
			};

			TarballController(const Vector3D& velocity, GameCharacterController *attacker);
			~TarballController();

			void SetTeleportFlag(void)
			{
				teleportFlag = true;
			}

			void SetTarballSourceVelocity(const Vector3D& velocity)
			{
				tarballSource->SetSourceVelocity(velocity);
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


	class CreateTarballMessage : public CreateModelMessage
	{
		friend class CreateModelMessage;

		private:

			Vector3D		initialVelocity;
			int32			attackerIndex;

			CreateTarballMessage();

		public:

			CreateTarballMessage(int32 projectileIndex, int32 characterIndex, const Point3D& position, const Vector3D& velocity);
			~CreateTarballMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class TarballTeleportMessage : public ControllerMessage
	{
		friend class TarballController;

		private:

			Point3D			teleportPosition;
			Vector3D		teleportVelocity;
			Point3D			effectCenter;

			TarballTeleportMessage(int32 controllerIndex);

		public:

			TarballTeleportMessage(int32 controllerIndex, const Point3D& position, const Vector3D& velocity, const Point3D& center);
			~TarballTeleportMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			void HandleControllerMessage(Controller *controller) const;
	};


	class TarballExplodeMessage : public ControllerMessage
	{
		friend class TarballController;

		private:

			Point3D			explodePosition;
			Vector3D		explodeNormal;

			TarballExplodeMessage(int32 controllerIndex);

		public:

			TarballExplodeMessage(int32 controllerIndex, const Point3D& position, const Vector3D& normal);
			~TarballExplodeMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			void HandleControllerMessage(Controller *controller) const;
	};
}


#endif

// ZYUQURM
