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


#include "MGFish.h"
#include "MGGame.h"


using namespace C4;


namespace
{
	const float kFishSwimForce = 0.01F;
	const float kFishLevelForce = 0.25F;
	const float kFishResistForce = 0.01F;
	const float kFishSwimAcceleration = 2.0e-4F;
	const float kFishTurnAcceleration = 1.0e-6F;

	Storage<Fish> fishStorage;
}


Fish::Fish() :
		fishControllerRegistration(kControllerFish, TheGame->GetStringTable()->GetString(StringID('CTRL', kControllerFish))),
		fishModelRegistration(kModelFish, TheGame->GetStringTable()->GetString(StringID('MODL', 'ANML', kModelFish)), "fish/Fish", 0, kControllerFish)
{
}

Fish::~Fish()
{
}

void Fish::Construct(void)
{
	new(fishStorage) Fish;
}

void Fish::Destruct(void)
{
	fishStorage->~Fish();
}


FishController::FishController() : GameCharacterController(kCharacterAnimal, kControllerFish)
{
	fishFlags = 0;
}

FishController::FishController(const FishController& fishController) : GameCharacterController(fishController)
{
	fishFlags = fishController.fishFlags & ~(kFishInitialized | kFishContactTurn);
}

FishController::~FishController()
{
}

Controller *FishController::Replicate(void) const
{
	return (new FishController(*this));
}

bool FishController::ValidNode(const Node *node)
{
	return ((node->GetNodeType() == kNodeModel) && (static_cast<const Model *>(node)->GetModelType() == kModelFish));
}

void FishController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	GameCharacterController::Pack(data, packFlags);

	data << ChunkHeader('FLAG', 4);
	data << fishFlags;

	if (!(packFlags & kPackEditor))
	{
		data << ChunkHeader('DTIM', 4);
		data << decisionTime;

		data << ChunkHeader('SCAL', sizeof(Vector3D));
		data << fishScale;

		data << ChunkHeader('AZTH', 4);
		data << fishAzimuth;

		data << ChunkHeader('LEVL', 4);
		data << swimLevel;

		data << ChunkHeader('SPED', 12);
		data << swimSpeed;
		data << swimTargetSpeed;
		data << swimAcceleration;

		if (turnTime > 0)
		{
			data << ChunkHeader('TURN', 16);
			data << turnTime;
			data << turnRate;
			data << turnTargetRate;
			data << turnAcceleration;
		}
	}

	data << TerminatorChunk; 
}

void FishController::Unpack(Unpacker& data, unsigned_int32 unpackFlags) 
{
	GameCharacterController::Unpack(data, unpackFlags); 
	UnpackChunkList<FishController>(data, unpackFlags);
}
 
bool FishController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{ 
	switch (chunkHeader->chunkType) 
	{
		case 'FLAG':

			data >> fishFlags; 
			return (true);

		case 'DTIM':

			data >> decisionTime;
			return (true);

		case 'SCAL':

			data >> fishScale;
			return (true);

		case 'AZTH':

			data >> fishAzimuth;
			return (true);

		case 'LEVL':

			data >> swimLevel;
			return (true);

		case 'SPED':

			data >> swimSpeed;
			data >> swimTargetSpeed;
			data >> swimAcceleration;
			return (true);

		case 'TURN':

			data >> turnTime;
			data >> turnRate;
			data >> turnTargetRate;
			data >> turnAcceleration;
			return (true);
	}

	return (false);
}

inline void FishController::NewDecisionTime(void)
{
	decisionTime = Math::Random(5000) + 2000;
}

inline void FishController::NewSwimSpeed(void)
{
	swimTargetSpeed = Math::RandomFloat(2.0F);
}

void FishController::Preprocess(void)
{
	Model *model = GetTargetNode();
	if (!model->GetManipulator())
	{
		unsigned_int32 flags = fishFlags;
		if (!(flags & kFishInitialized))
		{
			fishFlags = flags | kFishInitialized;
			NewDecisionTime();

			float s = Math::RandomFloat(1.0F) + 1.0F;
			float t = Math::RandomFloat(1.0F) + 1.0F;
			fishScale.Set(s * t, s, s);

			const Transform4D& transform = model->GetWorldTransform();
			const Vector3D direction = transform[0];
			fishAzimuth = Atan(direction.y, direction.x);
			swimLevel = transform.GetTranslation().z;

			swimSpeed = 0.0F;
			swimAcceleration = kFishSwimAcceleration;
			NewSwimSpeed();

			turnTime = 0;
			turnRate = 0.0F;
			turnTargetRate = 0.0F;
			turnAcceleration = kFishTurnAcceleration;
		}

		swimBone[0] = model->FindNode(Text::StaticHash<'B', 'o', 'n', 'e', '0', '0', '2'>::value);
		swimBone[1] = model->FindNode(Text::StaticHash<'B', 'o', 'n', 'e', '0', '0', '4'>::value);

		Node *node = model->GetFirstSubnode();
		while (node)
		{
			NodeType type = node->GetNodeType();
			if (type == kNodeBone)
			{
				rootBone = node;
			}
			else if (type == kNodeShape)
			{
				const Point3D& position = node->GetNodePosition();
				const CapsuleShapeObject *shapeObject = static_cast<CapsuleShape *>(node)->GetObject();
				float dx = (fishScale.x - 1.0F) * (shapeObject->GetCapsuleSize().z + shapeObject->GetCapsuleHeight());
				node->SetNodePosition(Point3D(position.x * fishScale.x + dx, position.y, position.z));
			}

			node = node->Next();
		}
	}

	GameCharacterController::Preprocess();

	if (!model->GetManipulator())
	{
		SetRigidBodyFlags(GetRigidBodyFlags() | kRigidBodyForceFieldInhibit);
		SetGravityMultiplier(0.0F);
		SetFrictionCoefficient(0.0F);
		SetExternalLinearResistance(Vector3D(kFishResistForce, kFishResistForce, kFishResistForce));

		frameAnimator.SetTargetModel(model);
		model->SetRootAnimator(&frameAnimator);
	}
}

void FishController::Wake(void)
{
	GameCharacterController::Wake();

	frameAnimator.SetAnimation("fish/Swim");
	frameAnimator.GetFrameInterpolator()->SetMode(kInterpolatorForward | kInterpolatorLoop);
	animationRate = frameAnimator.GetFrameInterpolator()->GetRate() * 2.0F;
}

void FishController::Move(void)
{
	Matrix3D	rotator;

	int32 dt = TheTimeMgr->GetDeltaTime();

	if ((turnTime > 0) && ((turnTime -= dt) <= 0))
	{
		fishFlags &= ~kFishContactTurn;
		turnTargetRate = 0.0F;
	}

	if ((decisionTime -= dt) <= 0)
	{
		NewDecisionTime();
		NewSwimSpeed();
		swimAcceleration = kFishSwimAcceleration;

		turnTime = Math::Random(800) + 400;
		turnTargetRate = (Math::RandomFloat(0.003F) - 0.0015F) * swimTargetSpeed;
		turnAcceleration = kFishTurnAcceleration;
	}

	float fdt = TheTimeMgr->GetFloatDeltaTime();

	if (turnRate < turnTargetRate)
	{
		turnRate = Fmin(turnRate + fdt * turnAcceleration, turnTargetRate);
	}
	else
	{
		turnRate = Fmax(turnRate - fdt * turnAcceleration, turnTargetRate);
	}

	float azm = fishAzimuth + turnRate * fdt;
	if (azm > K::tau_over_2)
	{
		azm -= K::tau;
	}
	else if (azm < -K::tau_over_2)
	{
		azm += K::tau;
	}

	fishAzimuth = azm;
	SetRigidBodyMatrix3D(Transform(GetTargetNode()->GetSuperNode()->GetInverseWorldTransform(), Matrix3D().SetRotationAboutZ(azm)));

	if (swimSpeed < swimTargetSpeed)
	{
		swimSpeed = Fmin(swimSpeed + fdt * swimAcceleration, swimTargetSpeed);
	}
	else
	{
		swimSpeed = Fmax(swimSpeed - fdt * swimAcceleration, swimTargetSpeed);
	}

	Model *model = GetTargetNode();
	float dz = swimLevel - model->GetWorldPosition().z;
	if (dz > 0.0F)
	{
		dz *= 0.25F;
	}

	SetExternalForce(Vector3D(CosSin(azm) * (swimSpeed * kFishSwimForce), dz * kFishLevelForce));

	frameAnimator.GetFrameInterpolator()->SetRate(animationRate * Fmax(swimSpeed, 0.25F));
	model->Animate();

	rootBone->SetNodeTransform(rootBone->GetNodeTransform() & fishScale);

	rotator.SetRotationAboutZ(Clamp(turnRate * -250.0F, -0.7F, 0.7F));
	swimBone[0]->SetNodeTransform(rotator * swimBone[0]->GetNodeTransform());

	rotator(1,0) = -rotator(1,0);
	rotator(0,1) = -rotator(0,1);
	swimBone[1]->SetNodeTransform(rotator * swimBone[1]->GetNodeTransform());
}

RigidBodyStatus FishController::HandleNewRigidBodyContact(const RigidBodyContact *contact, RigidBodyController *contactBody)
{
	if (contactBody->GetRigidBodyType() == kRigidBodyCharacter)
	{
		const GameCharacterController *character = static_cast<GameCharacterController *>(contactBody);
		if (character->GetCharacterType() == kCharacterAnimal)
		{
			return (kRigidBodyUnchanged);
		}
	}

	decisionTime = Math::Random(2000) + 1000;

	swimSpeed = Fmax(swimSpeed, 1.0F);
	swimTargetSpeed = 3.0F;
	swimAcceleration = kFishSwimAcceleration * 10.0F;

	unsigned_int32 flags = fishFlags;
	if (!(flags & kFishContactTurn))
	{
		fishFlags = flags | kFishContactTurn;

		turnTime = 500;
		turnTargetRate = NonzeroFsgn(contact->GetContactNormal().y) * -0.01F;
		turnAcceleration = kFishTurnAcceleration * 10.0F;
	}

	return (kRigidBodyUnchanged);
}

RigidBodyStatus FishController::HandleNewGeometryContact(const GeometryContact *contact)
{
	decisionTime = Max(decisionTime, 3000);
	swimTargetSpeed = Fmax(swimTargetSpeed, 0.5F);
	swimAcceleration = kFishSwimAcceleration * 4.0F;

	unsigned_int32 flags = fishFlags;
	if (!(flags & kFishContactTurn))
	{
		const Vector3D& normal = contact->GetRigidBodyContactNormal();
		if (normal.x > 0.0F)
		{
			float d = Fmax(normal.x, 0.25F) * -0.01F;
			turnTargetRate = NonzeroFsgn(normal.y) * d;
			turnAcceleration = kFishTurnAcceleration * 4.0F;
			turnTime = Math::Random(500) + 500;

			fishFlags = flags | kFishContactTurn;
		}
	}

	return (kRigidBodyUnchanged);
}

// ZYUQURM
