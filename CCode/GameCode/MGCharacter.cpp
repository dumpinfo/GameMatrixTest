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


#include "MGCharacter.h"
#include "MGMaterials.h"


using namespace C4;


namespace
{
	const float kStandingVelocityTime = 400.0F;
}


GameCharacterController::GameCharacterController(CharacterType charType, ControllerType contType) : CharacterController(contType)
{
	characterType = charType;
	characterState = 0;

	standingTime = 0.0F;
	offGroundTime = 0.0F;

	SetRigidBodyType(kRigidBodyCharacter);
	SetCollisionKind(kCollisionCharacter);
	SetCollisionExclusionMask(kCollisionSoundPath | kCollisionCorpse);
}

GameCharacterController::GameCharacterController(const GameCharacterController& gameCharacterController) : CharacterController(gameCharacterController)
{
	characterType = gameCharacterController.characterType;
	characterState = 0;

	standingTime = 0.0F;
	offGroundTime = 0.0F;
}

GameCharacterController::~GameCharacterController()
{
}

void GameCharacterController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	CharacterController::Pack(data, packFlags);

	if (!(packFlags & kPackEditor))
	{
		data << ChunkHeader('STAT', 4);
		data << characterState;

		data << ChunkHeader('STND', 4);
		data << standingTime;

		data << ChunkHeader('OFFG', 4);
		data << offGroundTime;
	}

	data << TerminatorChunk;
}

void GameCharacterController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	CharacterController::Unpack(data, unpackFlags);
	UnpackChunkList<GameCharacterController>(data, unpackFlags);
}

bool GameCharacterController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'STAT':

			data >> characterState;
			return (true);

		case 'STND':

			data >> standingTime;
			return (true);

		case 'OFFG':

			data >> offGroundTime;
			return (true);
	}

	return (false);
}

void GameCharacterController::Move(void)
{
	unsigned_int32 state = characterState;
	if (!(state & (kCharacterFlying | kCharacterSwimming)))
	{
		const CollisionContact *contact = GetGroundContact();
		if (contact)
		{
			if (state & kCharacterOffGround)
			{
				state &= ~(kCharacterOffGround | kCharacterJumping);
			}

			characterState = state | kCharacterGround; 
			offGroundTime = 0.0F;

			if ((!GetSubmergedWaterBlock()) && (SquaredMag(GetExternalForce()) < K::min_float)) 
			{
				float time = standingTime; 
				if (time < kStandingVelocityTime)
				{
					standingTime = (time += TheTimeMgr->GetFloatDeltaTime()); 
					SetVelocityMultiplier(FmaxZero(1.0F - time * (1.0F / kStandingVelocityTime)));
				} 
				else 
				{
					SetVelocityMultiplier(0.0F);
				}
			} 
			else
			{
				ResetStandingTime();
			}
		}
		else
		{
			characterState = (state | kCharacterOffGround) & ~kCharacterGround;

			standingTime = 0.0F;
			offGroundTime += TheTimeMgr->GetFloatDeltaTime();

			SetVelocityMultiplier(1.0F);

			if (!(state & kCharacterJumping))
			{
				Vector3D velocity = GetLinearVelocity();
				velocity.z = FminZero(velocity.z);
				SetLinearVelocity(velocity);
			}
		}
	}
}

void GameCharacterController::HandlePhysicsSpaceExit(void)
{
	if ((!(characterState & kCharacterDead)) && (TheMessageMgr->Server()))
	{
		Kill(nullptr);
	}
}

const SubstanceData *GameCharacterController::GetGroundSubstanceData(void) const
{
	const CollisionContact *contact = GetGroundContact();
	if (contact)
	{
		SubstanceType substanceType = kSubstanceNone;
		const Subcontact *subcontact = contact->GetSubcontact(0);

		ContactType type = contact->GetContactType();
		if (type == kContactGeometry)
		{
			const GeometryContact *geometryContact = static_cast<const GeometryContact *>(contact);
			const MaterialObject *materialObject = geometryContact->GetContactGeometry()->GetTriangleMaterial(subcontact->triangleIndex);
			if (materialObject)
			{
				substanceType = materialObject->GetMaterialSubstance();
			}
		}
		else if (type == kContactRigidBody)
		{
			const RigidBodyContact *rigidBodyContact = static_cast<const RigidBodyContact *>(contact);
			const Shape *shape = (rigidBodyContact->GetStartElement() == this) ? rigidBodyContact->GetFinishShape() : rigidBodyContact->GetStartShape();
			substanceType = shape->GetObject()->GetShapeSubstance();
		}

		if (substanceType != kSubstanceNone)
		{
			const GameSubstance *substance = static_cast<const GameSubstance *>(MaterialObject::FindRegisteredSubstance(substanceType));
			if (substance)
			{
				return (substance->GetSubstanceData());
			}
		}
	}

	return (nullptr);
}

void GameCharacterController::EnterWorld(World *world, const Point3D& worldPosition)
{
}

CharacterStatus GameCharacterController::Damage(Fixed damage, unsigned_int32 flags, GameCharacterController *attacker, const Point3D *position, const Vector3D *force)
{
	return (kCharacterUnaffected);
}

void GameCharacterController::Kill(GameCharacterController *attacker, const Point3D *position, const Vector3D *force)
{
	characterState = (characterState | kCharacterDead) & ~kCharacterAttackable;
}

void GameCharacterController::SetAttackable(bool attackable)
{
	unsigned_int32 state = characterState;
	if (attackable)
	{
		if (!(state & kCharacterDead))
		{
			characterState = state | kCharacterAttackable;
		}
	}
	else
	{
		characterState = state & ~kCharacterAttackable;
	}
}


CharacterStateMessage::CharacterStateMessage(ControllerMessageType type, int32 controllerIndex) : ControllerMessage(type, controllerIndex)
{
}

CharacterStateMessage::CharacterStateMessage(ControllerMessageType type, int32 controllerIndex, const Point3D& position, const Vector3D& velocity) : ControllerMessage(type, controllerIndex)
{
	initialPosition = position;
	initialVelocity = velocity;
}

CharacterStateMessage::~CharacterStateMessage()
{
}

void CharacterStateMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << initialPosition;
	data << initialVelocity;
}

bool CharacterStateMessage::Decompress(Decompressor& data)
{
	if (ControllerMessage::Decompress(data))
	{
		data >> initialPosition;
		data >> initialVelocity;
		return (true);
	}

	return (false);
}


AnimationBlender::AnimationBlender()
{
	blendParity = 0;
}

AnimationBlender::~AnimationBlender()
{
}

void AnimationBlender::Pack(Packer& data, unsigned_int32 packFlags) const
{
	data << ChunkHeader('PRTY', 4);
	data << blendParity;

	PackHandle handle = data.BeginChunk('BLND');
	blendAnimator.Pack(data, packFlags);
	data.EndChunk(handle);

	handle = data.BeginChunk('FRM0');
	frameAnimator[0].Pack(data, packFlags);
	data.EndChunk(handle);

	handle = data.BeginChunk('FRM1');
	frameAnimator[1].Pack(data, packFlags);
	data.EndChunk(handle);

	data << TerminatorChunk;
}

void AnimationBlender::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	UnpackChunkList<AnimationBlender>(data, unpackFlags);
}

bool AnimationBlender::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'PRTY':

			data >> blendParity;
			return (true);

		case 'BLND':

			blendAnimator.Unpack(data, unpackFlags);
			return (true);

		case 'FRM0':

			frameAnimator[0].Unpack(data, unpackFlags);
			return (true);

		case 'FRM1':

			frameAnimator[1].Unpack(data, unpackFlags);
			return (true);
	}

	return (false);
}

void AnimationBlender::Preprocess(Model *model)
{
	blendAnimator.SetTargetModel(model);
	frameAnimator[0].SetTargetModel(model);
	frameAnimator[1].SetTargetModel(model);

	blendAnimator.AppendSubnode(&frameAnimator[0]);
	blendAnimator.AppendSubnode(&frameAnimator[1]);
	model->SetRootAnimator(&blendAnimator);
}

FrameAnimator *AnimationBlender::StartAnimation(const char *name, unsigned_int32 mode, Interpolator::CompletionProc *proc, void *cookie)
{
	FrameAnimator *animator = &frameAnimator[0];

	animator->SetAnimation(name);
	animator->GetWeightInterpolator()->Set(1.0F, 0.0F, kInterpolatorStop);

	Interpolator *interpolator = animator->GetFrameInterpolator();
	interpolator->SetMode(mode);
	interpolator->SetCompletionProc(proc, cookie);

	frameAnimator[1].GetWeightInterpolator()->Set(0.0F, 0.0F, kInterpolatorStop);
	frameAnimator[1].GetFrameInterpolator()->SetCompletionProc(nullptr);

	blendParity = 0;
	return (animator);
}

FrameAnimator *AnimationBlender::BlendAnimation(const char *name, unsigned_int32 mode, float blendRate, Interpolator::CompletionProc *proc, void *cookie)
{
	int32 parity = blendParity;
	FrameAnimator *oldAnimator = &frameAnimator[parity];
	FrameAnimator *newAnimator = &frameAnimator[parity ^= 1];
	blendParity = parity;

	newAnimator->SetAnimation(name);
	newAnimator->GetWeightInterpolator()->Set(0.0F, blendRate, kInterpolatorForward);

	Interpolator *interpolator = newAnimator->GetFrameInterpolator();
	interpolator->SetMode(mode);
	interpolator->SetCompletionProc(proc, cookie);

	oldAnimator->GetWeightInterpolator()->Set(1.0F, blendRate, kInterpolatorBackward);
	oldAnimator->GetFrameInterpolator()->SetCompletionProc(nullptr);

	return (newAnimator);
}

// ZYUQURM
