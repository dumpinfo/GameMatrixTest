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


#ifndef MGFireball_h
#define MGFireball_h


#include "C4Sources.h"
#include "MGWeapons.h"
#include "MGEffects.h"
#include "MGMultiplayer.h"


namespace C4
{
	enum : ControllerType
	{
		kControllerFireball					= 'frbl'
	};


	enum : ModelType
	{
		kModelFireball						= 'frbl'
	};


	enum : ParticleSystemType
	{
		kParticleSystemFireballExplosion	= 'frex',
		kParticleSystemFireballSparks		= 'frsp'
	};


	class FireballExplosionParticleSystem : public TexcoordPointParticleSystem
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

			FireballExplosionParticleSystem();
			~FireballExplosionParticleSystem();

			void Preprocess(void) override;
			void AnimateParticles(void) override;
	};


	class FireballSparksParticleSystem : public LineParticleSystem
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

			FireballSparksParticleSystem();
			~FireballSparksParticleSystem();

			void Preprocess(void) override;
			void AnimateParticles(void) override;
	};


	class FireballController final : public ProjectileController
	{
		friend class Game;

		private:

			int32						lifeTime;
			float						smokeMultiplier;
			float						smokeDistance;

			bool						teleportFlag;

			SmokeTrailParticleSystem	*smokeTrail;
			OmniSource					*fireballSource;

			FireballController();
			FireballController(const FireballController& fireballController);

			Controller *Replicate(void) const override;

			static void SmokeTrailLinkProc(Node *node, void *cookie);
 
		public:

			enum 
			{
				kFireballMessageTeleport = kRigidBodyMessageBaseCount, 
				kFireballMessageExplode
			};
 
			FireballController(const Vector3D& velocity, float smoke, GameCharacterController *attacker);
			~FireballController(); 
 
			void SetTeleportFlag(void)
			{
				teleportFlag = true;
			} 

			void SetFireballSourceVelocity(const Vector3D& velocity)
			{
				fireballSource->SetSourceVelocity(velocity);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Preprocess(void) override;

			ControllerMessage *CreateMessage(ControllerMessageType type) const override;
			void SendInitialStateMessages(Player *player) const override;

			void Move(void) override;

			static Transform4D CalculateFireballTransform(const Point3D& position, const Vector3D& velocity);

			RigidBodyStatus HandleNewRigidBodyContact(const RigidBodyContact *contact, RigidBodyController *contactBody) override;
			RigidBodyStatus HandleNewGeometryContact(const GeometryContact *contact) override;

			void Destroy(const Point3D& position, const Vector3D& normal) override;
	};


	class CreateFireballMessage : public CreateModelMessage
	{
		friend class CreateModelMessage;

		private:

			Vector3D		initialVelocity;
			float			smokeMultiplier;
			int32			attackerIndex;

			CreateFireballMessage();

		public:

			CreateFireballMessage(int32 projectileIndex, int32 characterIndex, const Point3D& position, const Vector3D& velocity, float smoke);
			~CreateFireballMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class FireballTeleportMessage : public ControllerMessage
	{
		friend class FireballController;

		private:

			Point3D			teleportPosition;
			Vector3D		teleportVelocity;
			Point3D			effectCenter;

			FireballTeleportMessage(int32 controllerIndex);

		public:

			FireballTeleportMessage(int32 controllerIndex, const Point3D& position, const Vector3D& velocity, const Point3D& center);
			~FireballTeleportMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			void HandleControllerMessage(Controller *controller) const;
	};


	class FireballExplodeMessage : public ControllerMessage
	{
		friend class FireballController;

		private:

			Point3D			explodePosition;
			Vector3D		explodeNormal;

			FireballExplodeMessage(int32 controllerIndex);

		public:

			FireballExplodeMessage(int32 controllerIndex, const Point3D& position, const Vector3D& normal);
			~FireballExplodeMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			void HandleControllerMessage(Controller *controller) const;
	};
}


#endif

// ZYUQURM
