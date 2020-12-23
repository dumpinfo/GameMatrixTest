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


#ifndef MGCrossbow_h
#define MGCrossbow_h


#include "MGWeapons.h"
#include "MGEffects.h"
#include "MGInput.h"
#include "MGMultiplayer.h"


namespace C4
{
	enum : ControllerType
	{
		kControllerCrossbow				= 'cbow',
		kControllerArrow				= 'arrw',
		kControllerExplosiveArrow		= 'earw'
	};


	enum : ModelType
	{
		kModelCrossbow					= 'cbow',
		kModelArrow						= 'arrw',
		kModelArrowAmmo					= 'aram',
		kModelExplosiveArrowAmmo		= 'exam'
	};


	enum : ParticleSystemType
	{
		kParticleSystemArrowExplosion	= 'arex',
		kParticleSystemArrowSparks		= 'arsp'
	};


	enum
	{
		kArrowImpactStick				= 1 << 0
	};


	class ArrowController;
	class ExplosiveArrowController;


	class Crossbow final : public Weapon
	{
		private:

			ControllerReg<ArrowController>				arrowControllerRegistration;
			ControllerReg<ExplosiveArrowController>		explosiveArrowControllerRegistration;

			ModelRegistration							crossbowModelRegistration;
			ModelRegistration							arrowAmmoModelRegistration;
			ModelRegistration							explosiveArrowAmmoModelRegistration;
			ModelRegistration							arrowModelRegistration;

			WeaponAction								crossbowAction;

			Crossbow();
			~Crossbow();

		public:

			static void Construct(void);
			static void Destruct(void);

			WeaponController *NewWeaponController(FighterController *fighter) const;
	};


	class CrossbowController final : public WeaponController
	{
		private:

			Node				*centerNode;
			Node				*stringNode[2];
			Node				*wheelNode[2];

			Node				*axisNode;
			Node				*drumNode[3];
			Node				*arrowNode[3][8];

			int32				drumIndex;
			int32				arrowIndex;
			float				currentDrumAngle;
			float				targetDrumAngle;
			float				currentAxisAngle;
			float				targetAxisAngle;

			Node				*explosiveNode;
			float				explosiveHeight;
			int32				explosiveTime;

			FrameAnimator		frameAnimator;

			void ResetAmmo(void);

		public:
 
			enum
			{
				kCrossbowMessageFire = kWeaponMessageBaseCount, 
				kCrossbowMessageExplosive
			}; 

			CrossbowController(FighterController *fighter);
			~CrossbowController(); 

			void Preprocess(void) override; 
 
			ControllerMessage *CreateMessage(ControllerMessageType type) const override;

			void Move(void) override;
 
			void ShowFireAnimation(bool empty);
			void ShowExplosiveAnimation(void);

			WeaponResult UpdateWeapon(const Point3D& position, const Vector3D& direction, const Point3D& center) override;
			void ResetWeapon(void) override;
	};


	class CrossbowFireMessage : public ControllerMessage
	{
		friend class CrossbowController;

		private:

			bool	explosiveEmpty;

			CrossbowFireMessage(int32 index);

		public:

			CrossbowFireMessage(int32 index, bool empty);
			~CrossbowFireMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			void HandleControllerMessage(Controller *controller) const;
	};


	class CrossbowExplosiveMessage : public ControllerMessage
	{
		public:

			CrossbowExplosiveMessage(int32 index);
			~CrossbowExplosiveMessage();

			void HandleControllerMessage(Controller *controller) const;
	};


	class ArrowExplosionParticleSystem : public TexcoordPointParticleSystem
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

			ArrowExplosionParticleSystem();
			~ArrowExplosionParticleSystem();

			void Preprocess(void) override;
			void AnimateParticles(void) override;
	};


	class ArrowSparksParticleSystem : public LineParticleSystem
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

			ArrowSparksParticleSystem();
			~ArrowSparksParticleSystem();

			void Preprocess(void) override;
			void AnimateParticles(void) override;
	};


	class ArrowController : public ProjectileController
	{
		friend class ControllerReg<ArrowController>;

		private:

			int32		lifeTime;

			ArrowController();

			Controller *Replicate(void) const override;

		protected:

			ArrowController(ControllerType type);
			ArrowController(ControllerType type, const Vector3D& velocity, GameCharacterController *attacker);
			ArrowController(const ArrowController& arrowController);

		public:

			enum
			{
				kArrowMessageTeleport = kRigidBodyMessageBaseCount,
				kArrowMessageDestroy,
				kArrowMessageImpact,
				kArrowMessageExplode
			};

			ArrowController(const Vector3D& velocity, GameCharacterController *attacker);
			~ArrowController();

			void SetLifeTime(int32 time)
			{
				lifeTime = time;
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


	class ExplosiveArrowController final : public ArrowController
	{
		friend class ControllerReg<ExplosiveArrowController>;

		private:

			ExplosiveArrowController();
			ExplosiveArrowController(const ExplosiveArrowController& explosiveArrowController);

			Controller *Replicate(void) const override;

		public:

			ExplosiveArrowController(const Vector3D& velocity, GameCharacterController *attacker);
			~ExplosiveArrowController();

			void SendInitialStateMessages(Player *player) const override;
			void Preprocess(void) override;

			RigidBodyStatus HandleNewRigidBodyContact(const RigidBodyContact *contact, RigidBodyController *contactBody) override;
			RigidBodyStatus HandleNewGeometryContact(const GeometryContact *contact) override;

			void Destroy(const Point3D& position, const Vector3D& normal) override;
	};


	class CreateArrowMessage : public CreateModelMessage
	{
		friend class CreateModelMessage;

		private:

			Vector3D		initialVelocity;
			int32			attackerIndex;

			CreateArrowMessage();

		public:

			CreateArrowMessage(int32 projectileIndex, int32 characterIndex, const Point3D& position, const Vector3D& velocity);
			~CreateArrowMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class CreateExplosiveArrowMessage : public CreateModelMessage
	{
		friend class CreateModelMessage;

		private:

			Vector3D		initialVelocity;
			int32			attackerIndex;

			CreateExplosiveArrowMessage();

		public:

			CreateExplosiveArrowMessage(int32 projectileIndex, int32 characterIndex, const Point3D& position, const Vector3D& velocity);
			~CreateExplosiveArrowMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class ArrowTeleportMessage : public ControllerMessage
	{
		friend class ArrowController;

		private:

			Point3D			teleportPosition;
			Vector3D		teleportVelocity;
			Point3D			effectCenter;

			ArrowTeleportMessage(int32 controllerIndex);

		public:

			ArrowTeleportMessage(int32 controllerIndex, const Point3D& position, const Vector3D& velocity, const Point3D& center);
			~ArrowTeleportMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			void HandleControllerMessage(Controller *controller) const;
	};


	class ArrowDestroyMessage : public ControllerMessage
	{
		friend class ArrowController;

		private:

			Point3D			destroyPosition;
			SubstanceType	destroySubstance;

			ArrowDestroyMessage(int32 controllerIndex);

		public:

			ArrowDestroyMessage(int32 controllerIndex, const Point3D& position, SubstanceType substance = kSubstanceNone);
			~ArrowDestroyMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			void HandleControllerMessage(Controller *controller) const;
	};


	class ArrowImpactMessage : public ControllerMessage
	{
		friend class ArrowController;

		private:

			Point3D			impactPosition;
			SubstanceType	impactSubstance;
			unsigned_int32	impactMask;

			ArrowImpactMessage(int32 controllerIndex);

		public:

			ArrowImpactMessage(int32 controllerIndex, const Point3D& position, SubstanceType substance, unsigned_int32 mask);
			~ArrowImpactMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			void HandleControllerMessage(Controller *controller) const;
	};


	class ArrowExplodeMessage : public ControllerMessage
	{
		friend class ArrowController;

		private:

			Point3D			explodePosition;
			Vector3D		explodeNormal;

			ArrowExplodeMessage(int32 controllerIndex);

		public:

			ArrowExplodeMessage(int32 controllerIndex, const Point3D& position, const Vector3D& normal);
			~ArrowExplodeMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			void HandleControllerMessage(Controller *controller) const;
	};
}


#endif

// ZYUQURM
