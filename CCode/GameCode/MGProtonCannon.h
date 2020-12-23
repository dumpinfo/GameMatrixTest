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


#ifndef MGProtonCannon_h
#define MGProtonCannon_h


#include "C4Lights.h"
#include "C4Sources.h"
#include "C4Particles.h"
#include "MGWeapons.h"
#include "MGInput.h"


namespace C4
{
	enum : ControllerType
	{
		kControllerProtonCannon			= 'pcan'
	};


	enum : ModelType
	{
		kModelProtonCannon				= 'pcan',
		kModelProtonAmmo				= 'pram'
	};


	enum : ParticleSystemType
	{
		kParticleSystemProtonFlames		= 'pflm',
		kParticleSystemProtonSparks		= 'pspk',
		kParticleSystemProtonBeam		= 'pbem',
		kParticleSystemProtonAmmoBeam	= 'pabm'
	};


	class ProtonAmmoBeam;


	class ProtonCannon final : public Weapon
	{
		private:

			ParticleSystemReg<ProtonAmmoBeam>	protonAmmoBeamRegistration;

			ModelRegistration					protonCannonModelRegistration;
			ModelRegistration					protonAmmoModelRegistration;

			WeaponAction						protonCannonAction;

			Texture								*protonScorchTexture;

			ProtonCannon();
			~ProtonCannon();

		public:

			static void Construct(void);
			static void Destruct(void);

			WeaponController *NewWeaponController(FighterController *fighter) const;
	};


	class ProtonFlames : public FireParticleSystem
	{
		private:

			enum
			{
				kMaxParticleCount = 128
			};

			ParticlePool<FireParticle>		particlePool;
			FireParticle					particleArray[kMaxParticleCount];

		public:

			ProtonFlames();
			~ProtonFlames();

			void NewFlame(const Point3D& position);

			void AnimateParticles(void) override;
	};


	class ProtonSparks : public LineParticleSystem
	{
		private:

			enum
			{
				kMaxParticleCount = 512
			};

			ParticlePool<>		particlePool;
			Particle			particleArray[kMaxParticleCount];

		public:

			ProtonSparks(); 
			~ProtonSparks();

			void NewSparks(const Point3D& position, const Vector3D& normal, int32 count); 

			void AnimateParticles(void) override; 
	};

 
	class ProtonBeam : public PolyboardParticleSystem
	{ 
		private: 

			enum
			{
				kMaxParticleCount = 256 
			};

			int32							markingTime;
			int32							flameTime;
			int32							crackleTime;

			GameCharacterController			*attackerController;

			ProtonFlames					*flamesSystem;
			ProtonSparks					*sparksSystem;

			Interpolator					flickerInterpolator;
			float							flickerIntensity;

			float							controlRadius[4][4];
			float							controlDistance[4][4];
			float							controlVelocity[4][4];
			float							controlAngle[4][4];

			PointLight						protonLight;
			FlareEffect						protonFlare;
			QuadEffect						protonWarp;
			OmniSource						protonSource;
			OmniSource						shockSource;

			ParticlePool<PolyParticle>		particlePool;
			PolyParticle					particleArray[kMaxParticleCount];

			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		public:

			ProtonBeam(GameCharacterController *attacker);
			~ProtonBeam();

			void SetSourceVelocity(const Vector3D& velocity)
			{
				protonSource.SetSourceVelocity(velocity);
			}

			void Preprocess(void) override;
			void AnimateParticles(void) override;

			void SetBarrelAngle(float angle);
			void SetBeamPosition(const Vector3D& firingDirection);
	};


	class ProtonAmmoBeam : public PolyboardParticleSystem
	{
		private:

			enum
			{
				kMaxParticleCount = 64
			};

			float							beamLength;

			float							controlRadius[4][2];
			float							controlDistance[4][2];
			float							controlVelocity[4][2];
			float							controlAngle[4][2];

			ParticlePool<PolyParticle>		particlePool;
			PolyParticle					particleArray[kMaxParticleCount];

			ProtonAmmoBeam(const ProtonAmmoBeam& protonAmmoBeam);

			Node *Replicate(void) const override;

			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		public:

			ProtonAmmoBeam();
			~ProtonAmmoBeam();

			void Preprocess(void) override;
			void AnimateParticles(void) override;
	};


	class ProtonCannonController final : public WeaponController
	{
		private:

			enum
			{
				kProtonCannonSpin		= 1 << 0,
				kProtonCannonBeam		= 1 << 1
			};

			int32				firingTime;
			unsigned_int32		firingState;

			Node				*barrelNode;
			OmniSource			*spinSource;
			ProtonBeam			*protonBeam;

			Transform4D			barrelTransform;
			float				barrelAngle;
			float				barrelSpeed;

			List<Attribute>		attributeList;
			EmissionAttribute	emissionAttribute;

		public:

			enum
			{
				kProtonCannonMessageState = kWeaponMessageBaseCount
			};

			ProtonCannonController(FighterController *fighter);
			~ProtonCannonController();

			void Preprocess(void) override;

			ControllerMessage *CreateMessage(ControllerMessageType type) const override;
			void SendInitialStateMessages(Player *player) const override;

			void Move(void) override;

			void BeginFiring(bool primary);
			void EndFiring(void);

			void SetFiringState(unsigned_int32 state, float speed);

			WeaponResult UpdateWeapon(const Point3D& position, const Vector3D& direction, const Point3D& center) override;
	};


	class ProtonCannonStateMessage : public ControllerMessage
	{
		friend class ProtonCannonController;

		private:

			unsigned_int32	firingState;
			float			barrelSpeed;

			ProtonCannonStateMessage(int32 index);

		public:

			ProtonCannonStateMessage(int32 index, unsigned_int32 state, float speed);
			~ProtonCannonStateMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			void HandleControllerMessage(Controller *controller) const;
	};
}


#endif

// ZYUQURM
