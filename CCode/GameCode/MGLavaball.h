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


#ifndef MGLavaball_h
#define MGLavaball_h


#include "C4Sources.h"
#include "C4Particles.h"
#include "MGWeapons.h"
#include "MGMultiplayer.h"


namespace C4
{
	enum : ControllerType
	{
		kControllerLavaball					= 'lvbl'
	};


	enum : ModelType
	{
		kModelLavaball						= 'lvbl'
	};


	enum : ParticleSystemType
	{
		kParticleSystemLavaball				= 'lvbl',
		kParticleSystemLavaballTrail		= 'lbtr',
		kParticleSystemLavaballExplosion	= 'lbex'
	};


	class GooSplashParticleSystem;


	struct LavaballParticle : BlobParticle
	{
		Vector3D		rotationAxis;

		LavaballParticle *GetPreviousParticle(void) const
		{
			return (static_cast<LavaballParticle *>(prevParticle));
		}

		LavaballParticle *GetNextParticle(void) const
		{
			return (static_cast<LavaballParticle *>(nextParticle));
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


	class LavaballTrailParticleSystem : public PointParticleSystem
	{
		private:

			enum
			{
				kMaxParticleCount	= 160
			};

			ParticlePool<>		particlePool;
			Particle			particleArray[kMaxParticleCount];

		public:

			LavaballTrailParticleSystem();
			~LavaballTrailParticleSystem();

			void Finalize(void)
			{
				SetParticleSystemFlags(GetParticleSystemFlags() | kParticleSystemSelfDestruct);
			}

			void Preprocess(void) override;
			void AnimateParticles(void) override;

			void NewParticle(const Point3D& position);
	};


	class LavaballExplosionParticleSystem : public BlobParticleSystem
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
 
			LavaballExplosionParticleSystem();

			static void GooSplashLinkProc(Node *node, void *cookie);
 
		public:

			LavaballExplosionParticleSystem(const Vector3D& velocity, const Vector3D& normal);
			~LavaballExplosionParticleSystem();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Preprocess(void) override;
			void AnimateParticles(void) override;
	};


	class Lavaball : public BlobParticleSystem
	{
		private:

			enum
			{
				kMaxParticleCount	= 3
			};

			ParticlePool<LavaballParticle>		particlePool;
			LavaballParticle					particleArray[kMaxParticleCount];

			Lavaball(const Lavaball& lavaball);

			Node *Replicate(void) const override;

		public:

			Lavaball();
			~Lavaball();

			void Preprocess(void) override;
			void AnimateParticles(void) override;
	};


	class LavaballController final : public ProjectileController
	{
		friend class Game;

		private:

			int32							lifeTime;
			float							trailDistance;

			bool							teleportFlag;

			LavaballTrailParticleSystem		*lavaballTrail;
			OmniSource						*lavaballSource;

			LavaballController();
			LavaballController(const LavaballController& lavaballController);

			Controller *Replicate(void) const override;

			static void LavaballTrailLinkProc(Node *node, void *cookie);

		public:

			enum
			{
				kLavaballMessageTeleport = kRigidBodyMessageBaseCount,
				kLavaballMessageExplode
			};

			LavaballController(const Vector3D& velocity, GameCharacterController *attacker);
			~LavaballController();

			void SetTeleportFlag(void)
			{
				teleportFlag = true;
			}

			void SetLavaballSourceVelocity(const Vector3D& velocity)
			{
				lavaballSource->SetSourceVelocity(velocity);
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


	class CreateLavaballMessage : public CreateModelMessage
	{
		friend class CreateModelMessage;

		private:

			Vector3D		initialVelocity;
			int32			attackerIndex;

			CreateLavaballMessage();

		public:

			CreateLavaballMessage(int32 projectileIndex, int32 characterIndex, const Point3D& position, const Vector3D& velocity);
			~CreateLavaballMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class LavaballTeleportMessage : public ControllerMessage
	{
		friend class LavaballController;

		private:

			Point3D			teleportPosition;
			Vector3D		teleportVelocity;
			Point3D			effectCenter;

			LavaballTeleportMessage(int32 controllerIndex);

		public:

			LavaballTeleportMessage(int32 controllerIndex, const Point3D& position, const Vector3D& velocity, const Point3D& center);
			~LavaballTeleportMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			void HandleControllerMessage(Controller *controller) const;
	};


	class LavaballExplodeMessage : public ControllerMessage
	{
		friend class LavaballController;

		private:

			Point3D			explodePosition;
			Vector3D		explodeNormal;

			LavaballExplodeMessage(int32 controllerIndex);

		public:

			LavaballExplodeMessage(int32 controllerIndex, const Point3D& position, const Vector3D& normal);
			~LavaballExplodeMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			void HandleControllerMessage(Controller *controller) const;
	};
}


#endif

// ZYUQURM
