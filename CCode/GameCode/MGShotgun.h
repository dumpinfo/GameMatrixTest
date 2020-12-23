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


#ifndef MGShotgun_h
#define MGShotgun_h


#include "C4Lights.h"
#include "C4Particles.h"
#include "MGWeapons.h"
#include "MGRigidBody.h"
#include "MGInput.h"
#include "MGMultiplayer.h"


namespace C4
{
	enum : ControllerType
	{
		kControllerShotgun		= 'shgn',
		kControllerShell		= 'shel'
	};


	enum : ModelType
	{
		kModelShotgun			= 'shgn',
		kModelShell				= 'shel',
		kModelShellAmmo			= 'sham'
	};


	class Shotgun final : public Weapon
	{
		private:

			ModelRegistration		shotgunModelReg;
			ModelRegistration		shellAmmoModelReg;
			ModelRegistration		shellModelRegistration;

			WeaponAction			shotgunAction;

			Shotgun();
			~Shotgun();

		public:

			static void Construct(void);
			static void Destruct(void);

			WeaponController *NewWeaponController(FighterController *fighter) const;
	};


	class ShotgunController final : public WeaponController
	{
		private:

			const LocatorMarker		*shellEjectMarker[2];

			Light					*muzzleLight;
			BeamEffect				*muzzleFlashBeam[2];
			QuadEffect				*muzzleFlashQuad;

			Node					*drumNode;
			int32					drumShellCount;
			float					currentDrumAngle;
			float					targetDrumAngle;

			int32					flashTime;
			int32					pumpTime;

			FrameAnimator			frameAnimator;

		public:

			enum
			{
				kShotgunMessageFire = kWeaponMessageBaseCount,
				kShotgunMessageImpact,
				kShotgunMessageAdvance
			};

			ShotgunController(FighterController *fighter);
			~ShotgunController();

			void AdvanceDrum(void)
			{
				targetDrumAngle += 1.0F;
			}

			void Preprocess(void) override;

			ControllerMessage *CreateMessage(ControllerMessageType type) const override;

			void Move(void) override;

			void ShowFireAnimation(bool secondary);

			WeaponResult UpdateWeapon(const Point3D& position, const Vector3D& direction, const Point3D& center) override;
	};


	class ShotgunFireMessage : public ControllerMessage 
	{
		friend class ShotgunController;
 
		private:
 
			Point3D		firePosition;
			bool		fireSecondary;
 
			ShotgunFireMessage(int32 index);
 
		public: 

			ShotgunFireMessage(int32 index, const Point3D& position, bool secondary);
			~ShotgunFireMessage();
 
			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			void HandleControllerMessage(Controller *controller) const;
	};


	class ShotgunImpactMessage : public ControllerMessage
	{
		friend class ShotgunController;

		private:

			Point3D			impactPosition;
			Vector3D		impactNormal;
			SubstanceType	impactSubstance;

			ShotgunImpactMessage(int32 index);

		public:

			ShotgunImpactMessage(int32 index, const Point3D& position, const Vector3D& normal, SubstanceType substance);
			~ShotgunImpactMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			void HandleControllerMessage(Controller *controller) const;
	};


	class ShotgunAdvanceMessage : public ControllerMessage
	{
		public:

			ShotgunAdvanceMessage(int32 index);
			~ShotgunAdvanceMessage();

			void HandleControllerMessage(Controller *controller) const;
	};


	class ShellController final : public RemainsController
	{
		friend class Game;

		private:

			int32	bounceCount;

			ShellController();

		public:

			ShellController(int32 life);
			~ShellController();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			RigidBodyStatus HandleNewGeometryContact(const GeometryContact *contact);
	};
}


#endif

// ZYUQURM
