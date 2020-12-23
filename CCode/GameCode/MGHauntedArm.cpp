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


#include "MGHauntedArm.h"
#include "MGGame.h"


using namespace C4;


namespace
{
	const float kHauntedArmDragForce = 5.0F;
	const float kHauntedArmResistForce = 1.0F;
	const float kHauntedArmTurnRate = 0.02F;

	Storage<HauntedArm> hauntedArmStorage;
}


int32 HauntedArmController::globalHauntedArmCount = 0;

bool (HauntedArmController::*HauntedArmController::hauntedArmStateHandler[kHauntedArmStateCount])(void) =
{
	&HauntedArmController::HandleSleepingState,
	&HauntedArmController::HandleChasingState,
	&HauntedArmController::HandleAttackingState,
	&HauntedArmController::HandleFallingState
};


HauntedArm::HauntedArm() :
		hauntedArmControllerRegistration(kControllerHauntedArm, TheGame->GetStringTable()->GetString(StringID('CTRL', kControllerHauntedArm))),
		hauntedArmModelRegistration(kModelHauntedArm, TheGame->GetStringTable()->GetString(StringID('MODL', 'MNST', kModelHauntedArm)), "skeleton/arm/Arm", kModelPrecache, kControllerHauntedArm),

		generateHauntedArmRegistration(kMethodGenerateHauntedArm, TheGame->GetStringTable()->GetString(StringID('MTHD', kMethodGenerateHauntedArm))),
		getHauntedArmCountRegistration(kMethodGetHauntedArmCount, TheGame->GetStringTable()->GetString(StringID('MTHD', kMethodGetHauntedArmCount)), kMethodNoTarget | kMethodOutputValue)
{
}

HauntedArm::~HauntedArm()
{
}

void HauntedArm::Construct(void)
{
	new(hauntedArmStorage) HauntedArm;
}

void HauntedArm::Destruct(void)
{
	hauntedArmStorage->~HauntedArm();
}


inline HauntedArmConfiguration::HauntedArmConfiguration()
{
	hauntedArmFlags = 0;
}

inline HauntedArmConfiguration::HauntedArmConfiguration(const HauntedArmConfiguration& hauntedArmConfiguration)
{
	hauntedArmFlags = hauntedArmConfiguration.hauntedArmFlags;
}

void HauntedArmConfiguration::Pack(Packer& data) const
{
	data << ChunkHeader('FLAG', 4);
	data << hauntedArmFlags;
}

bool HauntedArmConfiguration::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> hauntedArmFlags;
			return (true);
	}

	return (false);
}

int32 HauntedArmConfiguration::GetSettingCount(void) const
{
	return (0);
}

Setting *HauntedArmConfiguration::GetSetting(int32 index) const
{
	return (nullptr);
}

void HauntedArmConfiguration::SetSetting(const Setting *setting)
{
}


inline void HauntedArmController::NewAttackDistance(void)
{
	attackDistance = Math::RandomFloat(3.0F) + 3.0F;
}
 
HauntedArmController::HauntedArmController() :
		MonsterController(kControllerHauntedArm),
		frameAnimatorObserver(this, &HauntedArmController::HandleAnimationEvent) 
{
	SetControllerFlags(kControllerAsleep); 

	SetMonsterHealth(10 << 16);
	SetMonsterRepelForce(5.0F); 

	SetMonsterState(kHauntedArmStateSleeping); 
	SetMonsterMotion(kHauntedArmMotionNone); 

	hauntedArmState = 0;
	NewAttackDistance();
} 

HauntedArmController::HauntedArmController(const HauntedArmConfiguration& configuration) :
		MonsterController(kControllerHauntedArm),
		HauntedArmConfiguration(configuration),
		frameAnimatorObserver(this, &HauntedArmController::HandleAnimationEvent)
{
	SetControllerFlags(kControllerAsleep);

	SetMonsterHealth(10 << 16);
	SetMonsterRepelForce(5.0F);

	SetMonsterState(kHauntedArmStateSleeping);
	SetMonsterMotion(kHauntedArmMotionNone);

	hauntedArmState = 0;
	NewAttackDistance();
}

HauntedArmController::HauntedArmController(const HauntedArmController& hauntedArmController) :
		MonsterController(hauntedArmController),
		HauntedArmConfiguration(hauntedArmController),
		frameAnimatorObserver(this, &HauntedArmController::HandleAnimationEvent)
{
	SetControllerFlags(kControllerAsleep);

	hauntedArmState = 0;
	NewAttackDistance();
}

HauntedArmController::~HauntedArmController()
{
	UncountMonster(globalHauntedArmCount);
}

Controller *HauntedArmController::Replicate(void) const
{
	return (new HauntedArmController(*this));
}

bool HauntedArmController::ValidNode(const Node *node)
{
	return ((node->GetNodeType() == kNodeModel) && (static_cast<const Model *>(node)->GetModelType() == kModelHauntedArm));
}

void HauntedArmController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	MonsterController::Pack(data, packFlags);
	HauntedArmConfiguration::Pack(data);

	data << ChunkHeader('STAT', 4);
	data << hauntedArmState;

	data << TerminatorChunk;
}

void HauntedArmController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	MonsterController::Unpack(data, unpackFlags);
	UnpackChunkList<HauntedArmController>(data, unpackFlags);
}

bool HauntedArmController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'STAT':

			data >> hauntedArmState;
			return (true);
	}

	return (HauntedArmConfiguration::UnpackChunk(chunkHeader, data));
}

int32 HauntedArmController::GetSettingCount(void) const
{
	return (HauntedArmConfiguration::GetSettingCount());
}

Setting *HauntedArmController::GetSetting(int32 index) const
{
	return (HauntedArmConfiguration::GetSetting(index));
}

void HauntedArmController::SetSetting(const Setting *setting)
{
	HauntedArmConfiguration::SetSetting(setting);
}

void HauntedArmController::Preprocess(void)
{
	MonsterController::Preprocess();

	if (!GetTargetNode()->GetManipulator())
	{
		SetAttackable(true);

		GetAnimationBlender()->SetFrameAnimatorObserver(&frameAnimatorObserver);
	}
}

void HauntedArmController::Wake(void)
{
	MonsterController::Wake();

	if (GetMonsterMotion() == kHauntedArmMotionNone)
	{
		SetMonsterState(kHauntedArmStateChasing);
		SetHauntedArmMotion(kHauntedArmMotionStand);
	}

	CountMonster(globalHauntedArmCount);
}

void HauntedArmController::Move(void)
{
	if ((this->*hauntedArmStateHandler[GetMonsterState()])())
	{
		MonsterController::Move();
	}
}

bool HauntedArmController::HandleSleepingState(void)
{
	return (true);
}

bool HauntedArmController::HandleChasingState(void)
{
	Vector3D	direction;

	Vector2D force = GetRepulsionForce();

	if (GetEnemyDirection(&direction))
	{
		float targetDistance = Magnitude(direction);
		float azm0 = Atan(direction.y, direction.x);
		float azm = SetMonsterAzimuth(azm0, kHauntedArmTurnRate);

		if (GetMonsterMotion() == kHauntedArmMotionStand)
		{
			if (targetDistance > attackDistance)
			{
				SetHauntedArmMotion(kHauntedArmMotionChase);
			}
		}

		if ((targetDistance > attackDistance) || (Fabs(azm - azm0) > 0.5F))
		{
			force += CosSin(azm) * kHauntedArmDragForce;
		}
		else
		{
			SetMonsterState(kHauntedArmStateAttacking);
			SetHauntedArmMotion(kHauntedArmMotionAttack);
			NewAttackDistance();

			SetCharacterState(GetCharacterState() | kCharacterJumping);
		}
	}

	if (GetCharacterState() & kCharacterGround)
	{
		SetExternalLinearResistance(Vector2D(kHauntedArmResistForce, kHauntedArmResistForce));
	}
	else
	{
		force *= 0.02F;
		SetExternalLinearResistance(Zero2D);
	}

	SetExternalForce(force);
	return (true);
}

bool HauntedArmController::HandleAttackingState(void)
{
	if (GetStateTime() >= 412)
	{
		Vector3D	direction;

		if (GetEnemyDirection(&direction))
		{
			Model *model = GetTargetNode();

			float f = 0.25F / Sqrt(direction.x * direction.x + direction.y * direction.y);
			ApplyImpulse(model->GetInverseWorldTransform() * Vector3D(direction.x * f, direction.y * f, 0.35F));

			//PlaySource("hauntedArm/Attack", 32.0F);

			SetMonsterState(kHauntedArmStateFalling);

			SetExternalForce(Zero3D);
			SetExternalLinearResistance(Zero2D);
			return (true);
		}

		SetMonsterState(kHauntedArmStateSleeping);
	}

	SetExternalForce(GetRepulsionForce());
	SetExternalLinearResistance(Vector2D(kHauntedArmResistForce, kHauntedArmResistForce));
	return (true);
}

bool HauntedArmController::HandleFallingState(void)
{
	if ((GetStateTime() >= 250) && (GetGroundContact()))
	{
		SetMonsterState(kHauntedArmStateChasing);
		SetHauntedArmMotion(kHauntedArmMotionChase);
		hauntedArmState &= ~kHauntedArmAttacked;
	}

	return (true);
}

void HauntedArmController::HandleAnimationEvent(FrameAnimator *animator, CueType cueType)
{
}

void HauntedArmController::SetHauntedArmMotion(int32 motion)
{
	SetMonsterMotion(motion);
	switch (motion)
	{
		case kHauntedArmMotionStand:

			GetAnimationBlender()->StartAnimation("skeleton/arm/Drag", kInterpolatorStop);
			break;

		case kHauntedArmMotionChase:
		{
			FrameAnimator *animator = GetAnimationBlender()->BlendAnimation("skeleton/arm/Drag", kInterpolatorForward | kInterpolatorLoop, 0.004F);
			animator->GetFrameInterpolator()->SetRate(2.0F);
			break;
		}

		case kHauntedArmMotionAttack:
		{
			FrameAnimator *animator = GetAnimationBlender()->BlendAnimation("skeleton/arm/Attack", kInterpolatorForward, 0.004F);
			Interpolator *interpolator = animator->GetFrameInterpolator();
			interpolator->SetRate(2.0F);
			break;
		}
	}
}

RigidBodyStatus HauntedArmController::HandleNewRigidBodyContact(const RigidBodyContact *contact, RigidBodyController *contactBody)
{
	if (contactBody->GetRigidBodyType() == kRigidBodyCharacter)
	{
		if (GetMonsterMotion() == kHauntedArmMotionAttack)
		{
			unsigned_int32 flags = hauntedArmState;
			if (!(flags & kHauntedArmAttacked))
			{
				hauntedArmState = flags | kHauntedArmAttacked;

				if (contactBody->GetControllerType() != kControllerHauntedArm)
				{
					static_cast<GameCharacterController *>(contactBody)->Damage(10 << 16, 0, this);
					return (kRigidBodyContactsBroken);
				}
			}
		}

		return (kRigidBodyUnchanged);
	}

	return (MonsterController::HandleNewRigidBodyContact(contact, contactBody));
}

CharacterStatus HauntedArmController::Damage(Fixed damage, unsigned_int32 flags, GameCharacterController *attacker, const Point3D *position, const Vector3D *force)
{
	Model *model = GetTargetNode();

	if (!position)
	{
		position = &GetWorldCenterOfMass();
	}

	if (!(flags & kDamageBloodInhibit))
	{
		int32 particleCount = damage >> 15;
		if (particleCount > 0)
		{
			BloodParticleSystem *blood = new BloodParticleSystem(ColorRGB(0.5F, 0.375F, 0.125F), particleCount);
			blood->SetNodePosition(*position);
			model->GetWorld()->AddNewNode(blood);
		}
	}

	return (MonsterController::Damage(damage, flags, attacker, position, force));
}

void HauntedArmController::Kill(GameCharacterController *attacker, const Point3D *position, const Vector3D *force)
{
	MonsterController::Kill(attacker, position, force);

	Model *model = GetTargetNode();

	//OmniSource *source = new OmniSource("hauntedArm/Death", 64.0F);
	//source->SetNodePosition(model->GetNodePosition());
	//GetTargetNode()->GetWorld()->AddNewNode(source);

	delete model;
}


GenerateHauntedArmMethod::GenerateHauntedArmMethod() : GenerateMonsterMethod(kMethodGenerateHauntedArm)
{
}

GenerateHauntedArmMethod::GenerateHauntedArmMethod(const GenerateHauntedArmMethod& generateHauntedArmMethod) :
		GenerateMonsterMethod(generateHauntedArmMethod),
		HauntedArmConfiguration(generateHauntedArmMethod)
{
}

GenerateHauntedArmMethod::~GenerateHauntedArmMethod()
{
}

Method *GenerateHauntedArmMethod::Replicate(void) const
{
	return (new GenerateHauntedArmMethod(*this));
}

void GenerateHauntedArmMethod::Pack(Packer& data, unsigned_int32 packFlags) const
{
	GenerateMonsterMethod::Pack(data, packFlags);
	HauntedArmConfiguration::Pack(data);
	data << TerminatorChunk;
}

void GenerateHauntedArmMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	GenerateMonsterMethod::Unpack(data, unpackFlags);
	UnpackChunkList<GenerateHauntedArmMethod>(data, unpackFlags);
}

bool GenerateHauntedArmMethod::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	return (HauntedArmConfiguration::UnpackChunk(chunkHeader, data));
}

int32 GenerateHauntedArmMethod::GetSettingCount(void) const
{
	return (HauntedArmConfiguration::GetSettingCount());
}

Setting *GenerateHauntedArmMethod::GetSetting(int32 index) const
{
	return (HauntedArmConfiguration::GetSetting(index));
}

void GenerateHauntedArmMethod::SetSetting(const Setting *setting)
{
	HauntedArmConfiguration::SetSetting(setting);
}

void GenerateHauntedArmMethod::Execute(const ScriptState *state)
{
	Node *node = GetTargetNode(state);
	if (node)
	{
		GamePlayer *player = static_cast<GamePlayer *>(TheMessageMgr->GetLocalPlayer());
		if (player->GetPlayerController())
		{
			Model *model = Model::Get(kModelHauntedArm);
			HauntedArmController *controller = new HauntedArmController(*this);
			GenerateMonster(node, model, controller);
		}
	}

	CallCompletionProc();
}


GetHauntedArmCountMethod::GetHauntedArmCountMethod() : Method(kMethodGetHauntedArmCount)
{
}

GetHauntedArmCountMethod::GetHauntedArmCountMethod(const GetHauntedArmCountMethod& getHauntedArmCountMethod) : Method(getHauntedArmCountMethod)
{
}

GetHauntedArmCountMethod::~GetHauntedArmCountMethod()
{
}

Method *GetHauntedArmCountMethod::Replicate(void) const
{
	return (new GetHauntedArmCountMethod(*this));
}

void GetHauntedArmCountMethod::Execute(const ScriptState *state)
{
	SetOutputValue(state, HauntedArmController::GetGlobalHauntedArmCount());
	CallCompletionProc();
}

// ZYUQURM
