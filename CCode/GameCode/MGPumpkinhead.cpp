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


#include "MGPumpkinhead.h"
#include "MGFireball.h"
#include "MGGame.h"


using namespace C4;


namespace
{
	const float kPumpkinheadRunForce = 400.0F;
	const float kPumpkinheadResistForce = 50.0F;
	const float kPumpkinheadTurnRate = 0.01F;
	const float kPumpkinheadAttackDistance = 2.25F;

	Storage<Pumpkinhead> pumpkinheadStorage;

	const Type pumpkinheadHeadType[kPumpkinheadHeadCount] =
	{
		kPumpkinheadHeadPumpkin, kPumpkinheadHeadWatermelon, kPumpkinheadHeadCardboardBox, kPumpkinheadHeadPropaneTank
	};
}


int32 PumpkinheadController::globalPumpkinheadCount = 0;

bool (PumpkinheadController::*PumpkinheadController::pumpkinheadStateHandler[kPumpkinheadStateCount])(void) =
{
	&PumpkinheadController::HandleSleepingState,
	&PumpkinheadController::HandleStandingState,
	&PumpkinheadController::HandleWakingState,
	&PumpkinheadController::HandleChasingState,
	&PumpkinheadController::HandleAttackingState,
	&PumpkinheadController::HandleThrowing1State,
	&PumpkinheadController::HandleThrowing2State,
	&PumpkinheadController::HandleRecoveringState,
	&PumpkinheadController::HandleDeadState
};


Pumpkinhead::Pumpkinhead() :
		pumpkinheadControllerRegistration(kControllerPumpkinhead, TheGame->GetStringTable()->GetString(StringID('CTRL', kControllerPumpkinhead))),
		pumpkinheadModelRegistration(kModelPumpkinhead, TheGame->GetStringTable()->GetString(StringID('MODL', 'MNST', kModelPumpkinhead)), "pumpkin/Pumpkinhead", kModelPrecache, kControllerPumpkinhead),
		pumpkin1ModelRegistration(kModelPumpkin1, nullptr, "pumpkin/pumpkin/Pumpkin-A", kModelPrecache | kModelPrivate),
		pumpkin2ModelRegistration(kModelPumpkin2, nullptr, "pumpkin/pumpkin/Pumpkin-B", kModelPrecache | kModelPrivate),
		watermelonModelRegistration(kModelWatermelon, nullptr, "pumpkin/watermelon/Watermelon", kModelPrecache | kModelPrivate),
		cardboardBoxModelRegistration(kModelCardboardBox, nullptr, "pumpkin/box/Box", kModelPrecache | kModelPrivate),
		propaneTankModelRegistration(kModelPropaneTank, nullptr, "pumpkin/propane/Propane", kModelPrecache | kModelPrivate),
		pumpkinheadFireballModelRegistration(kModelPumpkinheadFireball, nullptr, "pumpkin/fireball/Fireball", kModelPrecache | kModelPrivate),

		pumpkinBurnMaterialRegistration(kMaterialPumpkinBurn, "pumpkin/pumpkin/Pumpkin-burn"),
		watermelonBurnMaterialRegistration(kMaterialWatermelonBurn, "pumpkin/watermelon/Watermelon-burn"),

		generatePumpkinheadRegistration(kMethodGeneratePumpkinhead, TheGame->GetStringTable()->GetString(StringID('MTHD', kMethodGeneratePumpkinhead))),
		getPumpkinheadCountRegistration(kMethodGetPumpkinheadCount, TheGame->GetStringTable()->GetString(StringID('MTHD', kMethodGetPumpkinheadCount)), kMethodNoTarget | kMethodOutputValue)
{
}

Pumpkinhead::~Pumpkinhead()
{
}

void Pumpkinhead::Construct(void)
{
	new(pumpkinheadStorage) Pumpkinhead;
}

void Pumpkinhead::Destruct(void)
{
	pumpkinheadStorage->~Pumpkinhead();
}


inline PumpkinheadConfiguration::PumpkinheadConfiguration()
{
	pumpkinheadFlags = 0;
	pumpkinheadHead = kPumpkinheadHeadPumpkin;
}

inline PumpkinheadConfiguration::PumpkinheadConfiguration(const PumpkinheadConfiguration& pumpkinheadConfiguration)
{
	pumpkinheadFlags = pumpkinheadConfiguration.pumpkinheadFlags;
	pumpkinheadHead = pumpkinheadConfiguration.pumpkinheadHead;
}

void PumpkinheadConfiguration::Pack(Packer& data) const
{
	data << ChunkHeader('FLAG', 4);
	data << pumpkinheadFlags;

	data << ChunkHeader('HEAD', 4);
	data << pumpkinheadHead;
}

bool PumpkinheadConfiguration::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> pumpkinheadFlags; 
			return (true);

		case 'HEAD': 

			data >> pumpkinheadHead; 
			return (true);
	}
 
	return (false);
} 
 
int32 PumpkinheadConfiguration::GetSettingCount(void) const
{
	return (3);
} 

Setting *PumpkinheadConfiguration::GetSetting(int32 index) const
{
	const StringTable *table = TheGame->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerPumpkinhead, 'THRW'));
		return (new BooleanSetting('THRW', ((pumpkinheadFlags & kPumpkinheadDoubleThrow) != 0), title));
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerPumpkinhead, 'RISE'));
		return (new BooleanSetting('RISE', ((pumpkinheadFlags & kPumpkinheadRiseFromGround) != 0), title));
	}

	if (index == 2)
	{
		int32 selection = 0;
		for (machine a = 1; a < kPumpkinheadHeadCount; a++)
		{
			if (pumpkinheadHead == pumpkinheadHeadType[a])
			{
				selection = a;
				break;
			}
		}

		const char *title = table->GetString(StringID('CTRL', kControllerPumpkinhead, 'HEAD'));
		MenuSetting *menu = new MenuSetting('HEAD', selection, title, kPumpkinheadHeadCount);
		for (machine a = 0; a < kPumpkinheadHeadCount; a++)
		{
			menu->SetMenuItemString(a, table->GetString(StringID('CTRL', kControllerPumpkinhead, 'HEAD', pumpkinheadHeadType[a])));
		}

		return (menu);
	}

	return (nullptr);
}

void PumpkinheadConfiguration::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'THRW')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			pumpkinheadFlags |= kPumpkinheadDoubleThrow;
		}
		else
		{
			pumpkinheadFlags &= ~kPumpkinheadDoubleThrow;
		}
	}
	else if (identifier == 'RISE')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			pumpkinheadFlags |= kPumpkinheadRiseFromGround;
		}
		else
		{
			pumpkinheadFlags &= ~kPumpkinheadRiseFromGround;
		}
	}
	else if (identifier == 'HEAD')
	{
		int32 selection = static_cast<const MenuSetting *>(setting)->GetMenuSelection();
		pumpkinheadHead = pumpkinheadHeadType[selection];
	}
}


inline void PumpkinheadController::NewThrowTime(void)
{
	throwTime = Math::Random(2500) + 250;
}

PumpkinheadController::PumpkinheadController() :
		MonsterController(kControllerPumpkinhead),
		frameAnimatorObserver(this, &PumpkinheadController::HandleAnimationEvent)
{
	SetControllerFlags(kControllerAsleep);

	SetMonsterHealth(120 << 16);
	SetMonsterState(kPumpkinheadStateSleeping);
	SetMonsterMotion(kPumpkinheadMotionNone);

	SetSourcePosition(Point3D(0.0F, 0.0F, 1.8F));
	NewThrowTime();
}

PumpkinheadController::PumpkinheadController(const PumpkinheadConfiguration& configuration) :
		MonsterController(kControllerPumpkinhead),
		PumpkinheadConfiguration(configuration),
		frameAnimatorObserver(this, &PumpkinheadController::HandleAnimationEvent)
{
	SetControllerFlags(kControllerAsleep);

	SetMonsterHealth(120 << 16);
	SetMonsterState(kPumpkinheadStateSleeping);
	SetMonsterMotion(kPumpkinheadMotionNone);

	SetSourcePosition(Point3D(0.0F, 0.0F, 1.8F));
	NewThrowTime();
}

PumpkinheadController::PumpkinheadController(const PumpkinheadController& pumpkinheadController) :
		MonsterController(pumpkinheadController),
		PumpkinheadConfiguration(pumpkinheadController),
		frameAnimatorObserver(this, &PumpkinheadController::HandleAnimationEvent)
{
	SetControllerFlags(kControllerAsleep);

	NewThrowTime();
}

PumpkinheadController::~PumpkinheadController()
{
	UncountMonster(globalPumpkinheadCount);
}

Controller *PumpkinheadController::Replicate(void) const
{
	return (new PumpkinheadController(*this));
}

bool PumpkinheadController::ValidNode(const Node *node)
{
	return ((node->GetNodeType() == kNodeModel) && (static_cast<const Model *>(node)->GetModelType() == kModelPumpkinhead));
}

void PumpkinheadController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	MonsterController::Pack(data, packFlags);
	PumpkinheadConfiguration::Pack(data);

	if (!(packFlags & kPackEditor))
	{
		data << ChunkHeader('THTM', 4);
		data << throwTime;

		int32 state = GetMonsterState();
		if (state == kPumpkinheadStateThrowing2)
		{
			data << ChunkHeader('THRW', 4);
			data << throwCount;
		}
		else if (state == kPumpkinheadStateStanding)
		{
			data << ChunkHeader('WTTM', 4);
			data << waitTime;
		}
		else if (state == kPumpkinheadStateWaking)
		{
			data << ChunkHeader('RISE', 8);
			data << riseOffset;
			data << riseVelocity;
		}
	}

	data << TerminatorChunk;
}

void PumpkinheadController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	MonsterController::Unpack(data, unpackFlags);
	UnpackChunkList<PumpkinheadController>(data, unpackFlags);
}

bool PumpkinheadController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'THTM':

			data >> throwTime;
			return (true);

		case 'THRW':

			data >> throwCount;
			return (true);

		case 'WTTM':

			data >> waitTime;
			return (true);

		case 'RISE':

			data >> riseOffset;
			data >> riseVelocity;
			return (true);
	}

	return (PumpkinheadConfiguration::UnpackChunk(chunkHeader, data));
}

int32 PumpkinheadController::GetSettingCount(void) const
{
	return (PumpkinheadConfiguration::GetSettingCount());
}

Setting *PumpkinheadController::GetSetting(int32 index) const
{
	return (PumpkinheadConfiguration::GetSetting(index));
}

void PumpkinheadController::SetSetting(const Setting *setting)
{
	PumpkinheadConfiguration::SetSetting(setting);
}

void PumpkinheadController::Preprocess(void)
{
	Model *model = GetTargetNode();
	if (!model->GetManipulator())
	{
		static const ModelType headModelType[kPumpkinheadHeadCount] =
		{
			kModelPumpkin1, kModelWatermelon, kModelCardboardBox, kModelPropaneTank
		};

		ModelType modelType = headModelType[0] + Math::Random(2);
		for (machine a = 1; a < kPumpkinheadHeadCount; a++)
		{
			if (pumpkinheadHead == pumpkinheadHeadType[a])
			{
				modelType = headModelType[a];
				break;
			}
		}

		Model *head = Model::Get(modelType);
		head->SetNodeFlags(kNodeNonpersistent | kNodeCloneInhibit | kNodeAnimateInhibit);
		model->FindNode(Text::StaticHash<'N', 'e', 'c', 'k', '_', '1'>::value)->AppendNewSubnode(head);

		headFull = static_cast<Geometry *>(head->FindNode(Text::StaticHash<'J', 'a', 'c', 'k'>::value));
		headLeft = static_cast<Geometry *>(head->FindNode(Text::StaticHash<'L', 'e', 'f', 't'>::value));
		headRight = static_cast<Geometry *>(head->FindNode(Text::StaticHash<'R', 'i', 'g', 'h', 't'>::value));
		headBottom = static_cast<Geometry *>(head->FindNode(Text::StaticHash<'B', 'o', 't', 't', 'o', 'm'>::value));

		Model *fireball = Model::Get(kModelPumpkinheadFireball);
		fireballModel[0] = fireball;
		fireball->SetNodeFlags(kNodeNonpersistent | kNodeCloneInhibit | kNodeAnimateInhibit);
		fireball->SetNodeMatrix3D(Matrix3D().SetRotationAboutZ(K::tau_over_8) * Matrix3D().SetRotationAboutX(-K::tau_over_4));
		fireball->SetNodePosition(Point3D(0.1F, 0.0F, -0.1F));
		fireball->Disable();

		if (pumpkinheadFlags & kPumpkinheadDoubleThrow)
		{
			fireball = Model::Get(kModelPumpkinheadFireball);
			fireballModel[1] = fireball;
			fireball->SetNodeFlags(kNodeNonpersistent | kNodeCloneInhibit | kNodeAnimateInhibit);
			fireball->SetNodeMatrix3D(Matrix3D().SetRotationAboutZ(K::tau_over_8) * Matrix3D().SetRotationAboutX(-K::tau_over_4));
			fireball->SetNodePosition(Point3D(-0.1F, 0.0F, 0.1F));
			fireball->Disable();
		}

		model->FindNode(Text::StaticHash<'R', '_', 'H', 'a', 'n', 'd'>::value)->AppendNewSubnode(fireballModel[0]);
		if (pumpkinheadFlags & kPumpkinheadDoubleThrow)
		{
			model->FindNode(Text::StaticHash<'L', '_', 'H', 'a', 'n', 'd'>::value)->AppendNewSubnode(fireballModel[1]);
		}

		Node *node = model->GetFirstSubnode();
		while (node)
		{
			if (node->GetNodeType() == kNodeBone)
			{
				rootBone = node;
				break;
			}

			node = node->Next();
		}

		capeController = nullptr;
		node = model->FindNode(Text::StaticHash<'S', 'p', 'i', 'n', 'e', '_', '2'>::value)->GetFirstSubnode();
		while (node)
		{
			NodeType nodeType = node->GetNodeType();
			if (nodeType == kNodeGeometry)
			{
				Controller *controller = node->GetController();
				if ((controller) && (controller->GetControllerType() == kControllerCloth))
				{
					capeController = static_cast<ClothController *>(controller);
					capeController->Sleep();
					break;
				}
			}

			node = node->Next();
		}
	}

	MonsterController::Preprocess();
	SetCollisionExclusionMask(GetCollisionExclusionMask() | kCollisionFireball);

	if (!model->GetManipulator())
	{
		SetAttackable(true);

		AnimationBlender *animationBlender = GetAnimationBlender();
		animationBlender->SetFrameAnimatorObserver(&frameAnimatorObserver);

		int32 motion = GetMonsterMotion();
		Interpolator *interpolator = animationBlender->GetRecentAnimator()->GetFrameInterpolator();

		if (motion == kPumpkinheadMotionRise)
		{
			interpolator->SetCompletionProc(&WakeCallback, this);
		}
		else if (motion == kPumpkinheadMotionAttack)
		{
			interpolator->SetCompletionProc(&AttackCallback, this);
		}
		else if (motion == kPumpkinheadMotionIgnite)
		{
			interpolator->SetCompletionProc(&IgniteCallback, this);
		}
		else if (motion == kPumpkinheadMotionThrow1)
		{
			interpolator->SetCompletionProc(&Throw1Callback, this);
		}
		else if (motion == kPumpkinheadMotionThrow2)
		{
			interpolator->SetLoopProc(&Throw2Callback, this);
		}
		else if (motion == kPumpkinheadMotionDamage)
		{
			interpolator->SetCompletionProc(&DamageCallback, this);
		}
	}
}

void PumpkinheadController::Wake(void)
{
	MonsterController::Wake();

	if (GetMonsterState() != kPumpkinheadStateDead)
	{
		unsigned_int32 flags = pumpkinheadFlags;

		fireballModel[0]->Disable();
		if (flags & kPumpkinheadDoubleThrow)
		{
			fireballModel[1]->Disable();
		}

		if (GetMonsterMotion() == kPumpkinheadMotionNone)
		{
			if (flags & kPumpkinheadRiseFromGround)
			{
				SetMonsterState(kPumpkinheadStateWaking);
				SetPumpkinheadMotion(kPumpkinheadMotionRise);
			}
			else
			{
				GetAnimationBlender()->StartAnimation("pumpkin/Stand", kInterpolatorStop);
				SetMonsterState(kPumpkinheadStateChasing);
				SetPumpkinheadMotion(kPumpkinheadMotionChase);
			}
		}

		if (capeController)
		{
			capeController->Wake();
		}
	}

	CountMonster(globalPumpkinheadCount);
}

void PumpkinheadController::Move(void)
{
	if ((this->*pumpkinheadStateHandler[GetMonsterState()])())
	{
		if (capeController)
		{
			Vector3D wind = capeController->GetTargetNode()->GetInverseWorldTransform() * GetLinearVelocity();
			capeController->SetWindVelocity(-wind);
		}

		MonsterController::Move();

		if (GetMonsterState() == kPumpkinheadStateWaking)
		{
			Point3D position = rootBone->GetNodePosition();
			rootBone->SetNodePosition(Point3D(position.x, position.y, position.z + riseOffset));
		}
	}
}

bool PumpkinheadController::HandleSleepingState(void)
{
	return (true);
}

bool PumpkinheadController::HandleStandingState(void)
{
	if (GetStateTime() >= waitTime)
	{
		Vector3D	direction;

		if (GetEnemyDirection(&direction))
		{
			if (Magnitude(direction) > kPumpkinheadAttackDistance)
			{
				SetMonsterState(kPumpkinheadStateChasing);
				SetPumpkinheadMotion(kPumpkinheadMotionChase);
			}
			else
			{
				SetMonsterState(kPumpkinheadStateAttacking);
				SetPumpkinheadMotion(kPumpkinheadMotionAttack);
			}
		}
	}

	SetExternalForce(GetRepulsionForce());
	SetExternalLinearResistance(Vector2D(kPumpkinheadResistForce, kPumpkinheadResistForce));
	return (true);
}

bool PumpkinheadController::HandleWakingState(void)
{
	float offset = riseOffset;
	if (offset < 0.0F)
	{
		float dt = TheTimeMgr->GetFloatDeltaTime();
		float velocity = riseVelocity + K::gravity * dt;
		riseVelocity = velocity;

		offset += velocity * dt;
		if (!(offset < 0.0F))
		{
			offset = 0.0F;
			GetAnimationBlender()->GetFrameAnimator(0)->GetFrameInterpolator()->SetMode(kInterpolatorForward);
		}

		riseOffset = offset;
	}

	SetExternalLinearResistance(Vector2D(kPumpkinheadResistForce, kPumpkinheadResistForce));
	return (true);
}

bool PumpkinheadController::HandleChasingState(void)
{
	Vector3D	direction;

	Model *model = GetTargetNode();
	Vector2D force = GetRepulsionForce();

	if (GetEnemyDirection(&direction))
	{
		float targetDistance = Magnitude(direction);
		float azm = SetMonsterAzimuth(Atan(direction.y, direction.x), 0.01F);

		if ((targetDistance > kPumpkinheadAttackDistance) || (GetStateTime() < 100))
		{
			force += CosSin(azm) * kPumpkinheadRunForce;

			if (((throwTime = MaxZero(throwTime - TheTimeMgr->GetDeltaTime())) <= 0) && (targetDistance < 25.0F))
			{
				CollisionData	data;

				NewThrowTime();

				Point3D position = model->GetWorldPosition();
				position.z += 2.0F;

				if (!model->GetWorld()->DetectCollision(position, position + direction, 0.0F, kCollisionSightPath, &data))
				{
					if (pumpkinheadFlags & kPumpkinheadDoubleThrow)
					{
						SetMonsterState(kPumpkinheadStateThrowing2);
						SetPumpkinheadMotion(kPumpkinheadMotionThrow2);
					}
					else
					{
						SetMonsterState(kPumpkinheadStateThrowing1);
						SetPumpkinheadMotion(kPumpkinheadMotionIgnite);
					}
				}
			}
		}
		else
		{
			SetMonsterState(kPumpkinheadStateAttacking);
			SetPumpkinheadMotion(kPumpkinheadMotionAttack);
		}
	}

	if (GetCharacterState() & kCharacterGround)
	{
		SetExternalLinearResistance(Vector2D(kPumpkinheadResistForce, kPumpkinheadResistForce));
	}
	else
	{
		force *= 0.02F;
		SetExternalLinearResistance(Zero2D);
	}

	SetExternalForce(force);
	return (true);
}

bool PumpkinheadController::HandleAttackingState(void)
{
	Vector3D	direction;

	if (GetEnemyDirection(&direction))
	{
		SetMonsterAzimuth(Atan(direction.y, direction.x), kPumpkinheadTurnRate);
	}

	SetExternalForce(GetRepulsionForce());
	SetExternalLinearResistance(Vector2D(kPumpkinheadResistForce, kPumpkinheadResistForce));
	return (true);
}

bool PumpkinheadController::HandleThrowing1State(void)
{
	if (GetMonsterMotion() == kPumpkinheadMotionIgnite)
	{
		if (GetStateTime() >= 600)
		{
			SetPumpkinheadMotion(kPumpkinheadMotionThrow1);
		}
	}

	SetExternalForce(GetRepulsionForce());
	SetExternalLinearResistance(Vector2D(kPumpkinheadResistForce, kPumpkinheadResistForce));
	return (true);
}

bool PumpkinheadController::HandleThrowing2State(void)
{
	Vector3D	direction;

	if (GetEnemyDirection(&direction))
	{
		SetMonsterAzimuth(Atan(direction.y, direction.x), kPumpkinheadTurnRate);
	}

	SetExternalForce(GetRepulsionForce());
	SetExternalLinearResistance(Vector2D(kPumpkinheadResistForce, kPumpkinheadResistForce));
	return (true);
}

bool PumpkinheadController::HandleRecoveringState(void)
{
	SetExternalForce(GetRepulsionForce());
	SetExternalLinearResistance(Vector2D(kPumpkinheadResistForce, kPumpkinheadResistForce));
	return (true);
}

bool PumpkinheadController::HandleDeadState(void)
{
	if (GetStateTime() >= 10000)
	{
		delete GetTargetNode();
		return (false);
	}

	SetExternalForce(Zero3D);
	SetExternalLinearResistance(Vector2D(kPumpkinheadResistForce, kPumpkinheadResistForce));
	return (true);
}

void PumpkinheadController::HandleAnimationEvent(FrameAnimator *animator, CueType cueType)
{
	switch (cueType)
	{
		case 'DMG1':

			DamageEnemy(5 << 16, kPumpkinheadAttackDistance);
			break;

		case 'DMG2':

			DamageEnemy(20 << 16, kPumpkinheadAttackDistance);
			break;

		case 'THRO':

			fireballModel[0]->Disable();
			if (TheMessageMgr->Server())
			{
				Vector3D	direction;

				if (GetEnemyDirection(&direction))
				{
					float d = Magnitude(direction) - 1.0F;
					Vector3D fireballDirection(10.0F, 0.0F, d * -0.05F * GetPhysicsController()->GetGravityAcceleration().z);
					fireballDirection.RotateAboutZ(GetMonsterAzimuth() + Atan(0.33F / d) + Math::RandomFloat(0.1F) - 0.05F);

					int32 projectileIndex = GetTargetNode()->GetWorld()->NewControllerIndex();
					TheMessageMgr->SendMessageAll(CreateFireballMessage(projectileIndex, GetControllerIndex(), fireballModel[0]->GetWorldPosition(), fireballDirection, 0.5F));
				}
				else
				{
					SetMonsterState(kPumpkinheadStateStanding);
					SetPumpkinheadMotion(kPumpkinheadMotionStand);
					waitTime = 250;
				}
			}

			break;

		case 'THR1':

			if (GetMonsterState() == kPumpkinheadStateThrowing2)
			{
				ThrowFireball(0);
			}

			break;

		case 'SHW2':

			if (GetMonsterState() == kPumpkinheadStateThrowing2)
			{
				fireballModel[1]->Enable();
				PlaySource("pumpkin/Throw2", 64.0F);
			}

			break;

		case 'THR2':

			if (GetMonsterState() == kPumpkinheadStateThrowing2)
			{
				ThrowFireball(1);
			}

			break;
	}
}

void PumpkinheadController::StartDoubleThrow(void)
{
	fireballModel[0]->Enable();
	PlaySource("pumpkin/Throw1", 64.0F);
}

void PumpkinheadController::ThrowFireball(int32 hand)
{
	fireballModel[hand]->Disable();
	if (TheMessageMgr->Server())
	{
		Vector3D	direction;

		if (GetEnemyDirection(&direction))
		{
			float d = Magnitude(direction) - 1.0F;
			Vector3D fireballDirection(30.0F, 0.0F, d * -0.015F * GetPhysicsController()->GetGravityAcceleration().z);
			fireballDirection.RotateAboutZ(GetMonsterAzimuth() + Math::RandomFloat(0.1F) - 0.05F);

			int32 projectileIndex = GetTargetNode()->GetWorld()->NewControllerIndex();
			TheMessageMgr->SendMessageAll(CreateFireballMessage(projectileIndex, GetControllerIndex(), fireballModel[hand]->GetWorldPosition(), fireballDirection, 0.125F));
		}
		else
		{
			SetMonsterState(kPumpkinheadStateStanding);
			SetPumpkinheadMotion(kPumpkinheadMotionStand);
			waitTime = 250;
		}
	}
}

void PumpkinheadController::SetPumpkinheadMotion(int32 motion)
{
	SetMonsterMotion(motion);
	switch (motion)
	{
		case kPumpkinheadMotionStand:

			GetAnimationBlender()->BlendAnimation("pumpkin/Stand", kInterpolatorForward | kInterpolatorLoop, 0.004F);
			break;

		case kPumpkinheadMotionRise:
		{
			FrameAnimator *animator = GetAnimationBlender()->StartAnimation("pumpkin/Jump", kInterpolatorStop, &WakeCallback, this);
			animator->GetFrameInterpolator()->SetValue(396.0F);
			riseOffset = -2.0F;
			riseVelocity = 0.012F;
			break;
		}

		case kPumpkinheadMotionChase:

			GetAnimationBlender()->BlendAnimation("pumpkin/Run", kInterpolatorForward | kInterpolatorLoop, 0.004F);
			break;

		case kPumpkinheadMotionAttack:
		{
			static const char soundName[3][16] =
			{
				"pumpkin/Attack1", "pumpkin/Attack2", "pumpkin/Attack3"
			};

			GetAnimationBlender()->BlendAnimation("pumpkin/Attack", kInterpolatorForward, 0.004F, &AttackCallback, this);

			OmniSource *source = PlaySource(soundName[Math::Random(3)], 32.0F);
			source->SetSourceFrequency(Math::RandomFloat(0.2F) + 0.8F);
			break;
		}

		case kPumpkinheadMotionIgnite:
		{
			FrameAnimator *animator = GetAnimationBlender()->BlendAnimation("pumpkin/Throw1", kInterpolatorForward, 0.004F, &IgniteCallback, this);
			Interpolator *interpolator = animator->GetFrameInterpolator();
			interpolator->SetMaxValue(132.0F);
			interpolator->SetRate(0.5F);

			PlaySource("pumpkin/fireball/Appear", 64.0F);
			break;
		}

		case kPumpkinheadMotionThrow1:
		{
			static const char soundName[3][16] =
			{
				"pumpkin/Throw1", "pumpkin/Throw2", "pumpkin/Throw3"
			};

			FrameAnimator *animator = GetAnimationBlender()->GetRecentAnimator();
			Interpolator *interpolator = animator->GetFrameInterpolator();
			interpolator->SetMaxValue(animator->GetAnimationDuration());
			interpolator->SetMode(kInterpolatorForward);
			interpolator->SetCompletionProc(&Throw1Callback, this);

			PlaySource(soundName[Math::Random(3)], 64.0F);
			break;
		}

		case kPumpkinheadMotionThrow2:
		{
			FrameAnimator *animator = GetAnimationBlender()->BlendAnimation("pumpkin/Throw2", kInterpolatorForward | kInterpolatorLoop, 0.004F);
			Interpolator *interpolator = animator->GetFrameInterpolator();
			interpolator->SetLoopProc(&Throw2Callback, this);

			throwCount = Math::Random(4) + 2;
			StartDoubleThrow();
			break;
		}

		case kPumpkinheadMotionDamage:

			GetAnimationBlender()->BlendAnimation("pumpkin/Damage", kInterpolatorForward, 0.004F, &DamageCallback, this);
			break;

		case kPumpkinheadMotionDeath:
		{
			AnimationBlender *animationBlender = GetAnimationBlender();
			animationBlender->GetRecentAnimator()->GetFrameInterpolator()->SetLoopProc(nullptr);

			FrameAnimator *animator = animationBlender->BlendAnimation("pumpkin/Death", kInterpolatorForward, 0.004F);
			animator->GetFrameInterpolator()->SetRate(0.625F);

			PlaySource("pumpkin/Death", 64.0F);
			break;
		}
	}
}

void PumpkinheadController::WakeCallback(Interpolator *interpolator, void *cookie)
{
	PumpkinheadController *pumpkinheadController = static_cast<PumpkinheadController *>(cookie);
	pumpkinheadController->SetMonsterState(kPumpkinheadStateChasing);
	pumpkinheadController->SetPumpkinheadMotion(kPumpkinheadMotionChase);
}

void PumpkinheadController::AttackCallback(Interpolator *interpolator, void *cookie)
{
	PumpkinheadController *pumpkinheadController = static_cast<PumpkinheadController *>(cookie);
	pumpkinheadController->SetMonsterState(kPumpkinheadStateStanding);
	pumpkinheadController->SetPumpkinheadMotion(kPumpkinheadMotionStand);
	pumpkinheadController->waitTime = Math::Random(500) + 200;
}

void PumpkinheadController::IgniteCallback(Interpolator *interpolator, void *cookie)
{
	PumpkinheadController *pumpkinheadController = static_cast<PumpkinheadController *>(cookie);
	pumpkinheadController->fireballModel[0]->Enable();
}

void PumpkinheadController::Throw1Callback(Interpolator *interpolator, void *cookie)
{
	PumpkinheadController *pumpkinheadController = static_cast<PumpkinheadController *>(cookie);
	pumpkinheadController->SetMonsterState(kPumpkinheadStateStanding);
	pumpkinheadController->SetPumpkinheadMotion(kPumpkinheadMotionStand);
	pumpkinheadController->waitTime = 250;
}

void PumpkinheadController::Throw2Callback(Interpolator *interpolator, void *cookie)
{
	PumpkinheadController *pumpkinheadController = static_cast<PumpkinheadController *>(cookie);
	if (--pumpkinheadController->throwCount > 0)
	{
		pumpkinheadController->StartDoubleThrow();
	}
	else
	{
		pumpkinheadController->SetMonsterState(kPumpkinheadStateStanding);
		pumpkinheadController->SetPumpkinheadMotion(kPumpkinheadMotionStand);
		pumpkinheadController->waitTime = 250;

		interpolator->SetLoopProc(nullptr);
	}
}

void PumpkinheadController::DamageCallback(Interpolator *interpolator, void *cookie)
{
	PumpkinheadController *pumpkinheadController = static_cast<PumpkinheadController *>(cookie);
	pumpkinheadController->SetMonsterState(kPumpkinheadStateStanding);
	pumpkinheadController->SetPumpkinheadMotion(kPumpkinheadMotionStand);
	pumpkinheadController->waitTime = Math::Random(200);
}

CharacterStatus PumpkinheadController::Damage(Fixed damage, unsigned_int32 flags, GameCharacterController *attacker, const Point3D *position, const Vector3D *force)
{
	Model *model = GetTargetNode();

	if (position)
	{
		float z = model->GetInverseWorldTransform().GetRow(2) ^ *position;
		if (z > 1.6F)
		{
			if ((headFull->Enabled()) && (headLeft) && (headRight))
			{
				headFull->Disable();
				headBottom->Enable();

				float x = headFull->GetInverseWorldTransform().GetRow(0) ^ *position;

				int32 detailLevel = TheGame->GetGameDetailLevel();
				if (x < 0.0F)
				{
					headRight->Enable();
					if (detailLevel < 2)
					{
						BreakOffNode(headLeft, 1.0F);
					}
				}
				else
				{
					headLeft->Enable();
					if (detailLevel < 2)
					{
						BreakOffNode(headRight, -1.0F);
					}
				}
			}
		}
	}
	else
	{
		position = &GetWorldCenterOfMass();
	}

	if (!(flags & kDamageBloodInhibit))
	{
		int32 particleCount = damage >> 15;
		if (particleCount > 0)
		{
			BloodParticleSystem *blood = new BloodParticleSystem(ColorRGB(0.75F, 0.3F, 0.0F), particleCount);
			blood->SetNodePosition(*position);
			model->GetWorld()->AddNewNode(blood);
		}
	}

	if (DamageTimeExpired(200))
	{
		//OmniSource *source = PlaySource("pumpkin/Damage", 32.0F);
		//source->SetSourceFrequency(Math::RandomFloat(0.2F) + 0.8F);
	}

	unsigned_int32 state = GetMonsterState();
	if (state == kPumpkinheadStateWaking)
	{
		damage >>= 1;
	}

	CharacterStatus status = MonsterController::Damage(damage, flags, attacker, position, force);
	if ((status == kCharacterDamaged) && (GetMonsterState() == kPumpkinheadStateChasing))
	{
		if (Math::Random(3) == 0)
		{
			SetMonsterState(kPumpkinheadStateRecovering);
			SetPumpkinheadMotion(kPumpkinheadMotionDamage);
		}
	}

	return (status);
}

void PumpkinheadController::Kill(GameCharacterController *attacker, const Point3D *position, const Vector3D *force)
{
	MonsterController::Kill(attacker, position, force);

	UncountMonster(globalPumpkinheadCount);
	SetMonsterState(kPumpkinheadStateDead);

	SetPumpkinheadMotion(kPumpkinheadMotionDeath);
	SetExternalForce(Zero3D);
}

void PumpkinheadController::BreakOffNode(Geometry *geometry, float direction)
{
	const Node *target = GetTargetNode();
	Node *super = target->GetSuperNode();

	geometry->Enable();
	geometry->SetNodeTransform(super->GetInverseWorldTransform() * geometry->GetWorldTransform());
	super->AppendSubnode(geometry);

	MaterialType materialType = (pumpkinheadHead == kPumpkinheadHeadWatermelon) ? kMaterialWatermelonBurn : kMaterialPumpkinBurn;
	geometry->SetMaterialObject(0, AutoReleaseMaterial(materialType));

	RemainsController *controller = new RemainsController(Math::Random(6000, 8000));
	geometry->SetController(controller);
	controller->Preprocess();
	geometry->Invalidate();

	Vector3D velocity = Math::RandomUnitVector3D() * 4.0F;
	velocity.x = Fnabs(velocity.x);
	velocity.y = Fabs(velocity.y) * direction;
	velocity.z = Fabs(velocity.z);
	controller->SetLinearVelocity(target->GetWorldTransform() * velocity);

	controller->SetAngularVelocity(Math::RandomUnitVector3D() * (Math::RandomFloat(40.0F) + 5.0F));

	geometry->EstablishVisibility();
}


GeneratePumpkinheadMethod::GeneratePumpkinheadMethod() : GenerateMonsterMethod(kMethodGeneratePumpkinhead)
{
}

GeneratePumpkinheadMethod::GeneratePumpkinheadMethod(const GeneratePumpkinheadMethod& generatePumpkinheadMethod) :
		GenerateMonsterMethod(generatePumpkinheadMethod),
		PumpkinheadConfiguration(generatePumpkinheadMethod)
{
}

GeneratePumpkinheadMethod::~GeneratePumpkinheadMethod()
{
}

Method *GeneratePumpkinheadMethod::Replicate(void) const
{
	return (new GeneratePumpkinheadMethod(*this));
}

void GeneratePumpkinheadMethod::Pack(Packer& data, unsigned_int32 packFlags) const
{
	GenerateMonsterMethod::Pack(data, packFlags);
	PumpkinheadConfiguration::Pack(data);
	data << TerminatorChunk;
}

void GeneratePumpkinheadMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	GenerateMonsterMethod::Unpack(data, unpackFlags);
	UnpackChunkList<GeneratePumpkinheadMethod>(data, unpackFlags);
}

bool GeneratePumpkinheadMethod::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	return (PumpkinheadConfiguration::UnpackChunk(chunkHeader, data));
}

int32 GeneratePumpkinheadMethod::GetSettingCount(void) const
{
	return (PumpkinheadConfiguration::GetSettingCount());
}

Setting *GeneratePumpkinheadMethod::GetSetting(int32 index) const
{
	return (PumpkinheadConfiguration::GetSetting(index));
}

void GeneratePumpkinheadMethod::SetSetting(const Setting *setting)
{
	PumpkinheadConfiguration::SetSetting(setting);
}

void GeneratePumpkinheadMethod::Execute(const ScriptState *state)
{
	Node *node = GetTargetNode(state);
	if (node)
	{
		GamePlayer *player = static_cast<GamePlayer *>(TheMessageMgr->GetLocalPlayer());
		if (player->GetPlayerController())
		{
			Model *model = Model::Get(kModelPumpkinhead);
			PumpkinheadController *controller = new PumpkinheadController(*this);
			GenerateMonster(node, model, controller);
		}
	}

	CallCompletionProc();
}


GetPumpkinheadCountMethod::GetPumpkinheadCountMethod() : Method(kMethodGetPumpkinheadCount)
{
}

GetPumpkinheadCountMethod::GetPumpkinheadCountMethod(const GetPumpkinheadCountMethod& getPumpkinheadCountMethod) : Method(getPumpkinheadCountMethod)
{
}

GetPumpkinheadCountMethod::~GetPumpkinheadCountMethod()
{
}

Method *GetPumpkinheadCountMethod::Replicate(void) const
{
	return (new GetPumpkinheadCountMethod(*this));
}

void GetPumpkinheadCountMethod::Execute(const ScriptState *state)
{
	SetOutputValue(state, PumpkinheadController::GetGlobalPumpkinheadCount());
	CallCompletionProc();
}

// ZYUQURM
