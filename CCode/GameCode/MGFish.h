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


#ifndef MGFish_h
#define MGFish_h


#include "MGCharacter.h"


namespace C4
{
	enum : ControllerType
	{
		kControllerFish		= 'fish'
	};


	enum : ModelType
	{
		kModelFish			= 'fish'
	};


	enum
	{
		kFishInitialized	= 1 << 0,
		kFishContactTurn	= 1 << 1
	};


	class FishController;


	class Fish
	{
		private:

			ControllerReg<FishController>		fishControllerRegistration;
			ModelRegistration					fishModelRegistration;

			Fish();
			~Fish();

		public:

			static void Construct(void);
			static void Destruct(void);
	};


	class FishController final : public GameCharacterController
	{
		private:

			unsigned_int32		fishFlags;
			int32				decisionTime;

			Vector3D			fishScale;
			float				fishAzimuth;

			float				swimLevel;
			float				swimSpeed;
			float				swimTargetSpeed;
			float				swimAcceleration;

			int32				turnTime;
			float				turnRate;
			float				turnTargetRate;
			float				turnAcceleration;

			Node				*rootBone;
			Node				*swimBone[2];

			float				animationRate;
			FrameAnimator		frameAnimator;

			FishController(const FishController& fishController);

			Controller *Replicate(void) const override;

			void NewDecisionTime(void);
			void NewSwimSpeed(void);

		public:

			FishController();
			~FishController();

			static bool ValidNode(const Node *node);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Preprocess(void) override;
			void Wake(void) override;
			void Move(void) override;

			RigidBodyStatus HandleNewRigidBodyContact(const RigidBodyContact *contact, RigidBodyController *contactBody);
			RigidBodyStatus HandleNewGeometryContact(const GeometryContact *contact);
	};
}
 

#endif

// ZYUQURM
