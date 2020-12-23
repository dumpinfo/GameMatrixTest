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


#ifndef MGBlackCat_h
#define MGBlackCat_h


#include "C4Sources.h"
#include "MGWeapons.h"
#include "MGEffects.h"
#include "MGMultiplayer.h"


namespace C4
{
	enum : ControllerType
	{
		kControllerBlackCat		= 'bcat'
	};


	enum : ModelType
	{
		kModelBlackCat			= 'bcat'
	};


	enum : ParticleSystemType
	{
		kParticleSystemBlackCatExplosion	= 'bcex'
	};


	class BlackCatExplosionParticleSystem : public PointParticleSystem
	{
		private:

			enum
			{
				kMaxParticleCount = 50
			};

			ParticlePool<Particle>		particlePool;
			Particle					particleArray[kMaxParticleCount];

			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		public:

			BlackCatExplosionParticleSystem();
			~BlackCatExplosionParticleSystem();

			void Preprocess(void) override;
			void AnimateParticles(void) override;
	};


	class BlackCatController final : public ProjectileController
	{
		friend class Game;

		private:

			int32			lifeTime;
			int32			damageMultiplier;

			OmniSource		*blackCatSource;
			FrameAnimator	frameAnimator;

			BlackCatController();
			BlackCatController(const BlackCatController& blackCatController);

			Controller *Replicate(void) const override;

		public:

			enum
			{
				kBlackCatMessageExplode = kRigidBodyMessageBaseCount
			};

			BlackCatController(const Vector3D& velocity, int32 damage, GameCharacterController *attacker);
			~BlackCatController();

			void SetBlackCatSourceVelocity(const Vector3D& velocity)
			{
				blackCatSource->SetSourceVelocity(velocity);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Preprocess(void) override;

			ControllerMessage *CreateMessage(ControllerMessageType type) const override;
			void SendInitialStateMessages(Player *player) const override;

			void Move(void) override;

			static Transform4D CalculateBlackCatTransform(const Point3D& position, const Vector3D& velocity);

			RigidBodyStatus HandleNewRigidBodyContact(const RigidBodyContact *contact, RigidBodyController *contactBody) override;
			RigidBodyStatus HandleNewGeometryContact(const GeometryContact *contact) override; 

			void Destroy(const Point3D& position, const Vector3D& normal) override;
	}; 

 
	class CreateBlackCatMessage : public CreateModelMessage
	{
		friend class CreateModelMessage; 

		private: 
 
			Vector3D		initialVelocity;
			Fixed			damageMultiplier;
			int32			attackerIndex;
 
			CreateBlackCatMessage();

		public:

			CreateBlackCatMessage(int32 projectileIndex, int32 characterIndex, const Point3D& position, const Vector3D& velocity, int32 damage = 1);
			~CreateBlackCatMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class BlackCatExplodeMessage : public ControllerMessage
	{
		friend class BlackCatController;

		private:

			Point3D			explodePosition;
			Vector3D		explodeNormal;

			BlackCatExplodeMessage(int32 controllerIndex);

		public:

			BlackCatExplodeMessage(int32 controllerIndex, const Point3D& position, const Vector3D& normal);
			~BlackCatExplodeMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			void HandleControllerMessage(Controller *controller) const;
	};
}


#endif

// ZYUQURM
