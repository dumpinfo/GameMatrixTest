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


#ifndef MGCharacter_h
#define MGCharacter_h


#include "C4Models.h"
#include "C4Character.h"
#include "MGBase.h"


namespace C4
{
	typedef Type	CharacterType;


	enum : RigidBodyType
	{
		kRigidBodyCharacter		= 'char'
	};


	enum : CharacterType
	{
		kCharacterPlayer		= 'play',
		kCharacterMonster		= 'mnst',
		kCharacterAnimal		= 'anml'
	};


	enum
	{
		kCharacterDead			= 1 << 0,
		kCharacterGround		= 1 << 1,
		kCharacterOffGround		= 1 << 2,
		kCharacterJumping		= 1 << 3,
		kCharacterFlying		= 1 << 4,
		kCharacterSwimming		= 1 << 5,
		kCharacterAttackable	= 1 << 6
	};


	enum
	{
		kDamageBloodInhibit		= 1 << 0
	};


	enum CharacterStatus
	{
		kCharacterUnaffected,
		kCharacterDamaged,
		kCharacterKilled
	};


	struct SubstanceData;


	struct DamageLocation
	{
		Point3D		position;
		Vector3D	momentum;
	};


	class GameCharacterController : public CharacterController, public LinkTarget<GameCharacterController>
	{
		private:

			CharacterType		characterType;
			unsigned_int32		characterState;

			float				standingTime;
			float				offGroundTime;

		protected:

			GameCharacterController(CharacterType charType, ControllerType contType);
			GameCharacterController(const GameCharacterController& gameCharacterController);

			void SetAttackable(bool attackable);

		public:

			virtual ~GameCharacterController();

			CharacterType GetCharacterType(void) const
			{
				return (characterType);
			}

			unsigned_int32 GetCharacterState(void) const
			{
				return (characterState);
			}

			void SetCharacterState(unsigned_int32 state)
			{
				characterState = state;
			}

			void ResetStandingTime(void) 
			{
				standingTime = 0.0F;
				SetVelocityMultiplier(1.0F); 
			}
 
			float GetOffGroundTime(void) const
			{
				return (offGroundTime); 
			}
 
			Model *GetTargetNode(void) const 
			{
				return (static_cast<Model *>(Controller::GetTargetNode()));
			}
 
			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Move(void) override;

			void HandlePhysicsSpaceExit(void) override;

			const SubstanceData *GetGroundSubstanceData(void) const;

			virtual void EnterWorld(World *world, const Point3D& worldPosition);

			virtual CharacterStatus Damage(Fixed damage, unsigned_int32 flags, GameCharacterController *attacker, const Point3D *position = nullptr, const Vector3D *impulse = nullptr);
			virtual void Kill(GameCharacterController *attacker, const Point3D *position = nullptr, const Vector3D *impulse = nullptr);
	};


	class CharacterStateMessage : public ControllerMessage
	{
		friend class CharacterController;

		private:

			Point3D		initialPosition;
			Vector3D	initialVelocity;

		public:

			CharacterStateMessage(ControllerMessageType type, int32 controllerIndex);
			CharacterStateMessage(ControllerMessageType type, int32 controllerIndex, const Point3D& position, const Vector3D& velocity);
			~CharacterStateMessage();

			const Point3D& GetInitialPosition(void) const
			{
				return (initialPosition);
			}

			const Vector3D& GetInitialVelocity(void) const
			{
				return (initialVelocity);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;
	};


	class AnimationBlender : public Packable
	{
		private:

			int32				blendParity;
			BlendAnimator		blendAnimator;
			FrameAnimator		frameAnimator[2];

		public:

			AnimationBlender();
			~AnimationBlender();

			BlendAnimator *GetBlendAnimator(void)
			{
				return (&blendAnimator);
			}

			FrameAnimator *GetFrameAnimator(int32 index)
			{
				return (&frameAnimator[index]);
			}

			FrameAnimator *GetRecentAnimator(void)
			{
				return (&frameAnimator[blendParity]);
			}

			void SetFrameAnimatorObserver(FrameAnimator::ObserverType *observer)
			{
				frameAnimator[0].SetObserver(observer);
				frameAnimator[1].SetObserver(observer);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Preprocess(Model *model);

			FrameAnimator *StartAnimation(const char *name, unsigned_int32 mode, Interpolator::CompletionProc *proc = nullptr, void *cookie = nullptr);
			FrameAnimator *BlendAnimation(const char *name, unsigned_int32 mode, float blendRate, Interpolator::CompletionProc *proc = nullptr, void *cookie = nullptr);
	};
}


#endif

// ZYUQURM
