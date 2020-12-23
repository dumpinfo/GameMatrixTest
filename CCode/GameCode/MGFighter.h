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


#ifndef MGFighter_h
#define MGFighter_h


#include "C4World.h"
#include "MGEffects.h"
#include "MGMultiplayer.h"


namespace C4
{
	enum : AnimatorType
	{
		kAnimatorSpineTwist		= 'spin',
		kAnimatorScale			= 'scal',
		kAnimatorWeaponIK		= 'wpik'
	};


	enum
	{
		kFighterIconChat,
		kFighterIconCount
	};


	enum
	{
		kFighterDead				= 1 << 0,
		kFighterFiringPrimary		= 1 << 1,
		kFighterFiringSecondary		= 1 << 2,
		kFighterEmptyAmmo			= 1 << 3,
		kFighterTargetDistance		= 1 << 4,

		kFighterFiring				= kFighterFiringPrimary | kFighterFiringSecondary
	};


	enum
	{
		kFighterMotionNone,
		kFighterMotionStop,
		kFighterMotionStand,
		kFighterMotionForward,
		kFighterMotionBackward,
		kFighterMotionTurnLeft,
		kFighterMotionTurnRight,
		kFighterMotionJump,
		kFighterMotionDeath
	};


	class FighterController;
	class WeaponIKAnimator;


	class FighterInteractor : public Interactor
	{
		private:

			FighterController	*fighterController;

		public:

			FighterInteractor(FighterController *controller);
			~FighterInteractor();

			void HandleInteractionEvent(InteractionEventType type, Node *node, const Point3D *position = nullptr) override;
	};


	class FighterController : public GameCharacterController
	{
		private:

			unsigned_int32			fighterFlags;
			Link<Player>			fighterPlayer;

			float					primaryAzimuth;
			float					modelAzimuth;

			float					lookAzimuth;
			float					lookAltitude;
			float					deltaLookAzimuth;
			float					deltaLookAltitude;
			float					lookInterpolateParam;

			Point3D					previousCenterOfMass;

			unsigned_int32			movementFlags;
			int32					fighterMotion;
			bool					motionComplete;

			Vector3D				firingDirection;
			float					targetDistance;

			int32					deathTime;
			int32					damageTime;
			unsigned_int32			weaponSwitchTime;

			Model					*weaponModel; 
			const Marker			*weaponFireMarker;
			const Marker			*weaponMountMarker;
 
			int32					iconIndex;
			QuadEffect				*iconEffect; 
			ShellEffect				*shieldEffect;

			Node					*mountNode; 
			Light					*flashlight;
 
			MergeAnimator			*rootAnimator; 
			MergeAnimator			*mergeAnimator;
			BlendAnimator			*blendAnimator;
			FrameAnimator			*frameAnimator[2];
			WeaponIKAnimator		*weaponIKAnimator[2]; 

			FighterInteractor								fighterInteractor;
			FrameAnimatorObserver<FighterController>		frameAnimatorObserver;
			Observer<FighterController, WorldObservable>	worldUpdateObserver;

			void SetOrientation(float azm, float alt)
			{
				lookAzimuth = azm;
				lookAltitude = alt;
				lookInterpolateParam = 0.0F;
			}

			void SetMountNodeTransform(void);
			void UpdateWeapon(WorldObservable *observable);

			void HandleAnimationEvent(FrameAnimator *animator, CueType cueType);

			static void MotionComplete(Interpolator *interpolator, void *cookie);

		protected:

			FighterController(ControllerType type);

			float GetModelAzimuth(void) const
			{
				return (modelAzimuth);
			}

			float GetInterpolatedLookAzimuth(void) const
			{
				return (lookAzimuth + deltaLookAzimuth * lookInterpolateParam);
			}

			float GetInterpolatedLookAltitude(void) const
			{
				return (lookAltitude + deltaLookAltitude * lookInterpolateParam);
			}

			bool DamageTimeExpired(int32 reset)
			{
				if (damageTime > 0)
				{
					return (false);
				}

				damageTime = reset;
				return (true);
			}

			MergeAnimator *GetMergeAnimator(void) const
			{
				return (mergeAnimator);
			}

			BlendAnimator *GetBlendAnimator(void) const
			{
				return (blendAnimator);
			}

			FrameAnimator *GetFrameAnimator(int32 index) const
			{
				return (frameAnimator[index]);
			}

			void SetFrameAnimatorObserver(FrameAnimator::ObserverType *observer)
			{
				frameAnimator[0]->SetObserver(observer);
				frameAnimator[1]->SetObserver(observer);
			}

		public:

			enum
			{
				kFighterMessageEngageInteraction = kRigidBodyMessageBaseCount,
				kFighterMessageDisengageInteraction,
				kFighterMessageBeginMovement,
				kFighterMessageEndMovement,
				kFighterMessageChangeMovement,
				kFighterMessageBeginShield,
				kFighterMessageEndShield,
				kFighterMessageBeginIcon,
				kFighterMessageEndIcon,
				kFighterMessageTeleport,
				kFighterMessageLaunch,
				kFighterMessageLand,
				kFighterMessageUpdate,
				kFighterMessageWeapon,
				kFighterMessageEmptyAmmo,
				kFighterMessageDamage,
				kFighterMessageDeath
			};

			~FighterController();

			unsigned_int32 GetFighterFlags(void) const
			{
				return (fighterFlags);
			}

			void SetFighterFlags(unsigned_int32 flags)
			{
				fighterFlags = flags;
			}

			GamePlayer *GetFighterPlayer(void) const
			{
				return (static_cast<GamePlayer *>(fighterPlayer.GetTarget()));
			}

			void SetFighterPlayer(Player *player)
			{
				fighterPlayer = player;
			}

			float GetPrimaryAzimuth(void) const
			{
				return (primaryAzimuth);
			}

			void SetPrimaryAzimuth(float azimuth)
			{
				primaryAzimuth = azimuth;
			}

			float GetLookAzimuth(void) const
			{
				return (lookAzimuth);
			}

			void SetLookAzimuth(float azimuth)
			{
				lookAzimuth = azimuth;
			}

			float GetLookAltitude(void) const
			{
				return (lookAltitude);
			}

			void SetLookAltitude(float altitude)
			{
				lookAltitude = altitude;
			}

			unsigned_int32 GetMovementFlags(void) const
			{
				return (movementFlags);
			}

			void SetMovementFlags(unsigned_int32 flags)
			{
				movementFlags = flags;
			}

			int32 GetFighterMotion(void) const
			{
				return (fighterMotion);
			}

			const Vector3D& GetFiringDirection(void) const
			{
				return (firingDirection);
			}

			float GetTargetDistance(void) const
			{
				return (targetDistance);
			}

			void SetTargetDistance(float distance)
			{
				targetDistance = distance;
			}

			Model *GetWeaponModel(void) const
			{
				return (weaponModel);
			}

			const Marker *GetWeaponFireMarker(void) const
			{
				return (weaponFireMarker);
			}

			WeaponController *GetWeaponController(void) const
			{
				return (static_cast<WeaponController *>(weaponModel->GetController()));
			}

			Light *GetFlashlight(void) const
			{
				return (flashlight);
			}

			FighterInteractor *GetFighterInteractor(void)
			{
				return (&fighterInteractor);
			}

			const FighterInteractor *GetFighterInteractor(void) const
			{
				return (&fighterInteractor);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Preprocess(void) override;

			ControllerMessage *CreateMessage(ControllerMessageType type) const override;
			void ReceiveMessage(const ControllerMessage *message) override;
			void SendInitialStateMessages(Player *player) const override;
			void SendSnapshot(void);

			void Move(void) override;
			RigidBodyStatus HandleNewGeometryContact(const GeometryContact *contact);

			void EnterWorld(World *world, const Point3D& worldPosition) override;

			CharacterStatus Damage(Fixed damage, unsigned_int32 flags, GameCharacterController *attacker, const Point3D *position = nullptr, const Vector3D *impulse = nullptr) override;
			void Kill(GameCharacterController *attacker, const Point3D *position = nullptr, const Vector3D *impulse = nullptr) override;

			void UpdateOrientation(float azm, float alt);
			void BeginMovement(unsigned_int32 flag, float azm, float alt);
			void EndMovement(unsigned_int32 flag, float azm, float alt);
			void ChangeMovement(unsigned_int32 flags, float azm, float alt);

			void BeginFiring(bool primary, float azm, float alt);
			void EndFiring(float azm, float alt);

			void SetWeapon(int32 weaponIndex, int32 weaponControllerIndex);

			void ActivateFlashlight(void);
			void DeactivateFlashlight(void);
			void ToggleFlashlight(void);

			void SetPerspectiveExclusionMask(unsigned_int32 mask) const;

			virtual void SetFighterStyle(const int32 *style, bool prep = true);
			virtual void SetFighterMotion(int32 motion);

			virtual void Animate(void);
	};


	class CreateFighterMessage : public CreateModelMessage
	{
		private:

			float				initialAzimuth;
			float				initialAltitude;

			unsigned_int32		movementFlags;

			int32				weaponIndex;
			int32				weaponControllerIndex;

			int32				playerKey;

		protected:

			CreateFighterMessage(ModelMessageType type);
			~CreateFighterMessage();

		public:

			CreateFighterMessage(ModelMessageType type, int32 fighterIndex, const Point3D& position, float azm, float alt, unsigned_int32 movement, int32 weapon, int32 weaponController, int32 key);

			float GetInitialAzimuth(void) const
			{
				return (initialAzimuth);
			}

			float GetInitialAltitude(void) const
			{
				return (initialAltitude);
			}

			unsigned_int32 GetMovementFlags(void) const
			{
				return (movementFlags);
			}

			int32 GetWeaponIndex(void) const
			{
				return (weaponIndex);
			}

			int32 GetWeaponControllerIndex(void) const
			{
				return (weaponControllerIndex);
			}

			int32 GetPlayerKey(void) const
			{
				return (playerKey);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;
	};


	class FighterInteractionMessage : public ControllerMessage
	{
		friend class FighterController;

		private:

			int32		interactionControllerIndex;

		protected:

			FighterInteractionMessage(ControllerMessageType type, int32 controllerIndex);

		public:

			FighterInteractionMessage(ControllerMessageType type, int32 controllerIndex, int32 interactionIndex);
			~FighterInteractionMessage();

			int32 GetInteractionControllerIndex(void) const
			{
				return (interactionControllerIndex);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;
	};


	class FighterBeginInteractionMessage : public FighterInteractionMessage
	{
		friend class FighterController;

		private:

			Point3D		interactionPosition;

			FighterBeginInteractionMessage(int32 controllerIndex);

		public:

			FighterBeginInteractionMessage(int32 controllerIndex, int32 interactionIndex, const Point3D& position);
			~FighterBeginInteractionMessage();

			const Point3D& GetInteractionPosition(void) const
			{
				return (interactionPosition);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;
	};


	class FighterMovementMessage : public CharacterStateMessage
	{
		friend class FighterController;

		private:

			float			movementAzimuth;
			float			movementAltitude;
			unsigned_int32	movementFlag;

			FighterMovementMessage(ControllerMessageType type, int32 controllerIndex);

		public:

			FighterMovementMessage(ControllerMessageType type, int32 controllerIndex, const Point3D& position, const Vector3D& velocity, float azimuth, float altitude, unsigned_int32 flag);
			~FighterMovementMessage();

			float GetMovementAzimuth(void) const
			{
				return (movementAzimuth);
			}

			float GetMovementAltitude(void) const
			{
				return (movementAltitude);
			}

			int32 GetMovementFlag(void) const
			{
				return (movementFlag);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;
	};


	class FighterIconMessage : public ControllerMessage
	{
		friend class FighterController;

		private:

			int32		iconIndex;

			FighterIconMessage(ControllerMessageType type, int32 controllerIndex);

		public:

			FighterIconMessage(ControllerMessageType type, int32 controllerIndex, int32 icon);
			~FighterIconMessage();

			int32 GetIconIndex(void) const
			{
				return (iconIndex);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;
	};


	class FighterTeleportMessage : public CharacterStateMessage
	{
		friend class FighterController;

		private:

			float			teleportAzimuth;
			float			teleportAltitude;

			Point3D			effectCenter;

			FighterTeleportMessage(int32 controllerIndex);

		public:

			FighterTeleportMessage(int32 controllerIndex, const Point3D& position, const Vector3D& velocity, float azimuth, float altitude, const Point3D& center);
			~FighterTeleportMessage();

			float GetTeleportAzimuth(void) const
			{
				return (teleportAzimuth);
			}

			float GetTeleportAltitude(void) const
			{
				return (teleportAltitude);
			}

			const Point3D& GetEffectCenter(void) const
			{
				return (effectCenter);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;
	};


	class FighterUpdateMessage : public ControllerMessage
	{
		friend class FighterController;

		private:

			float		updateAzimuth;
			float		updateAltitude;

			FighterUpdateMessage(int32 controllerIndex);

		public:

			FighterUpdateMessage(int32 controllerIndex, float azimuth, float altitude);
			~FighterUpdateMessage();

			float GetUpdateAzimuth(void) const
			{
				return (updateAzimuth);
			}

			float GetUpdateAltitude(void) const
			{
				return (updateAltitude);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;
	};


	class FighterWeaponMessage : public ControllerMessage
	{
		friend class FighterController;

		private:

			int32		weaponIndex;
			int32		weaponControllerIndex;

			FighterWeaponMessage(int32 controllerIndex);

		public:

			FighterWeaponMessage(int32 controllerIndex, int32 weapon, int32 weaponController);
			~FighterWeaponMessage();

			int32 GetWeaponIndex(void) const
			{
				return (weaponIndex);
			}

			int32 GetWeaponControllerIndex(void) const
			{
				return (weaponControllerIndex);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;
	};


	class FighterDamageMessage : public ControllerMessage
	{
		friend class FighterController;

		private:

			int32			damageIntensity;
			unsigned_int32	damageFlags;
			Point3D			damageCenter;

			FighterDamageMessage(int32 controllerIndex);

		public:

			FighterDamageMessage(int32 controllerIndex, int32 intensity, unsigned_int32 flags, const Point3D& center);
			~FighterDamageMessage();

			int32 GetDamageIntensity(void) const
			{
				return (damageIntensity);
			}

			unsigned_int32 GetDamageFlags(void) const
			{
				return (damageFlags);
			}

			const Point3D& GetDamageCenter(void) const
			{
				return (damageCenter);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;
	};


	class SpineTwistAnimator : public Animator
	{
		private:

			int32			superNodeTransformIndex[2];
			Quaternion		spineRotation;

			SpineTwistAnimator();

		public:

			SpineTwistAnimator(Model *model, Node *node);
			~SpineTwistAnimator();

			void SetSpineRotation(const Quaternion& q)
			{
				spineRotation = q;
			}

			void Preprocess(void) override;
			void Configure(void) override;
			void Move(void) override;
	};


	class ScaleAnimator : public Animator
	{
		private:

			float		scale;

			ScaleAnimator();

		public:

			ScaleAnimator(Model *model, Node *node = nullptr);
			~ScaleAnimator();

			void SetScale(float s)
			{
				scale = s;
			}

			void Configure(void) override;
			void Move(void) override;
	};


	class WeaponIKAnimator : public Animator
	{
		private:

			enum
			{
				kMaxSuperNodeCount = 8
			};

			LocatorType		locatorType;
			float			effectorDistance;

			const Node		*effectorNode;
			const Node		*ikTargetNode;

			int32			rootAnimationIndex;
			int32			middleAnimationIndex;
			int32			superAnimationIndex[kMaxSuperNodeCount];

			WeaponIKAnimator();

		public:

			WeaponIKAnimator(Model *model, Node *node, LocatorType type);
			~WeaponIKAnimator();

			void SetIKTarget(Node *node)
			{
				ikTargetNode = node;
			}

			void Preprocess(void) override;
			void Configure(void) override;
			void Move(void) override;
	};
}


#endif

// ZYUQURM
