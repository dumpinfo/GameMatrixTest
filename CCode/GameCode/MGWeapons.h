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


#ifndef MGWeapons_h
#define MGWeapons_h


#include "C4Input.h"
#include "C4Models.h"
#include "C4Sources.h"
#include "MGCharacter.h"


namespace C4
{
	enum : RigidBodyType
	{
		kRigidBodyProjectile	= 'proj'
	};


	enum
	{
		kWeaponNone,
		kWeaponAxe,
		kWeaponPistol,
		kWeaponShotgun,
		kWeaponCrossbow,
		kWeaponSpikeShooter,
		kWeaponGrenadeLauncher,
		kWeaponQuantumCharger,
		kWeaponRocketLauncher,
		kWeaponPlasmaGun,
		kWeaponProtonCannon,
		kWeaponStandardCount,
		kWeaponCatFlinger = kWeaponStandardCount,
		kWeaponChickenBlaster,
		kWeaponHornetGun,
		kWeaponBeerCannon,
		kWeaponCount
	};


	enum
	{
		kWeaponSecondaryAmmo		= 1 << 0,
		kWeaponIndependentFire		= 1 << 1
	};


	enum WeaponResult
	{
		kWeaponIdle,
		kWeaponFired,
		kWeaponEmpty
	};


	class WeaponController;
	class FighterController;


	class Weapon
	{
		private:

			int32					weaponIndex;
			unsigned_int32			weaponFlags;

			ModelType				modelType;
			ActionType				actionType;

			int32					initWeaponAmmo[2];
			int32					maxWeaponAmmo[2];

			static const Weapon		*weaponTable[kWeaponCount];

			static Texture			*smokeTexture;
			static Texture			*sparkTexture;
			static Texture			*scorchTexture;
			static Texture			*trailTexture;
			static Texture			*flareTexture;
			static Texture			*shockTexture;
			static Texture			*warpTexture;
			static Texture			*distortTexture;
			static Texture			*flameTexture;

		protected:

			Weapon(int32 index, unsigned_int32 flags, ModelType type, ActionType action, int32 ammo1, int32 maxAmmo1, int32 ammo2 = 0, int32 maxAmmo2 = 0);

		public:

			int32 GetWeaponIndex(void) const
			{
				return (weaponIndex);
			}

			unsigned_int32 GetWeaponFlags(void) const
			{
				return (weaponFlags);
			}

			ModelType GetModelType(void) const 
			{
				return (modelType);
			} 

			ActionType GetActionType(void) const 
			{
				return (actionType);
			} 

			int32 GetInitWeaponAmmo(int32 index) const 
			{ 
				return (initWeaponAmmo[index]);
			}

			int32 GetMaxWeaponAmmo(int32 index) const 
			{
				return (maxWeaponAmmo[index]);
			}

			static const Weapon *Get(int32 weaponIndex)
			{
				return (weaponTable[weaponIndex]);
			}

			static ModelType WeaponIndexToType(int32 weaponIndex);
			static int32 WeaponTypeToIndex(ModelType weaponType);

			static void PreloadTextures(void);
			static void ReleaseTextures(void);

			virtual WeaponController *NewWeaponController(FighterController *fighter) const;
	};


	class WeaponController : public Controller
	{
		private:

			FighterController		*fighterController;

			int32					fireDelayTime;

		protected:

			WeaponController(ControllerType type, FighterController *fighter);

		public:

			enum
			{
				kWeaponMessageReset,
				kWeaponMessageBaseCount
			};

			~WeaponController();

			Model *GetTargetNode(void) const
			{
				return (static_cast<Model *>(Controller::GetTargetNode()));
			}

			FighterController *GetFighterController(void) const
			{
				return (fighterController);
			}

			int32 GetFireDelayTime(void) const
			{
				return (fireDelayTime);
			}

			void SetFireDelayTime(int32 time)
			{
				fireDelayTime = MaxZero(time);
			}

			ControllerMessage *CreateMessage(ControllerMessageType type) const override;

			virtual void BeginFiring(bool primary);
			virtual void EndFiring(void);

			virtual WeaponResult UpdateWeapon(const Point3D& position, const Vector3D& direction, const Point3D& center);
			virtual void ResetWeapon(void);
	};


	class WeaponResetMessage : public ControllerMessage
	{
		public:

			WeaponResetMessage(int32 index);
			~WeaponResetMessage();

			void HandleControllerMessage(Controller *controller) const;
	};


	class ProjectileController : public RigidBodyController
	{
		private:

			Vector3D						initialVelocity;
			Point3D							firstPosition;

			bool							velocityFlag;
			bool							positionFlag;

			Link<GameCharacterController>	attackerController;

			static void AttackerLinkProc(Node *node, void *cookie);

		protected:

			ProjectileController(ControllerType type);
			ProjectileController(ControllerType type, const Vector3D& velocity, GameCharacterController *attacker = nullptr);
			ProjectileController(const ProjectileController& projectileController);

			void SetSourceVelocity(OmniSource *source);

			int32 GetAttackerIndex(void) const;

		public:

			~ProjectileController();

			Model *GetTargetNode(void) const
			{
				return (static_cast<Model *>(Controller::GetTargetNode()));
			}

			GameCharacterController *GetAttackerController(void) const
			{
				return (attackerController);
			}

			Node *GetAttackerNode(void) const
			{
				return (attackerController.GetTarget() ? attackerController->GetTargetNode() : nullptr);
			}

			void SetFirstPosition(const Point3D& position)
			{
				firstPosition = position;
				positionFlag = true;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Preprocess(void) override;
			bool ValidRigidBodyCollision(const RigidBodyController *body) const override;
			void HandlePhysicsSpaceExit(void) override;

			virtual void EnterWorld(World *world, const Point3D& worldPosition);

			static Transform4D CalculateProjectileTransform(const Point3D& position, const Vector3D& velocity);
			void Teleport(const Point3D& startPosition, const Transform4D& teleportTransform, const Vector3D& teleportVelocity);

			static void ApplyRigidBodyImpulse(RigidBodyController *rigidBody, const Vector3D& impulse, const Point3D& position);

			virtual void Destroy(const Point3D& position, const Vector3D& normal) = 0;
	};
}


#endif

// ZYUQURM
