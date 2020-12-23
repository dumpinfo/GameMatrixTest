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


#include "MGBat.h"
#include "MGGame.h"


using namespace C4;


namespace
{
	const float kBatFlyForce = 25.0F;
	const float kBatFlyResistForce = 2.0F;
	const float kBatDeathResistForce = 0.06F;
	const float kBatTurnRate = 0.002F;
	const float kBatAttackDistance = 1.0F;

	Storage<Bat> batStorage;
}


int32 BatController::globalBatCount = 0;

bool (BatController::*BatController::batStateHandler[kBatStateCount])(void) =
{
	&BatController::HandleSleepingState,
	&BatController::HandleFlyingState,
	&BatController::HandleEscapingState,
	&BatController::HandleAttackingState,
	&BatController::HandleDeadState
};


Bat::Bat() :
		batControllerRegistration(kControllerBat, TheGame->GetStringTable()->GetString(StringID('CTRL', kControllerBat))),
		batModelRegistration(kModelBat, TheGame->GetStringTable()->GetString(StringID('MODL', 'MNST', kModelBat)), "bat/Bat", kModelPrecache, kControllerBat),

		generateBatRegistration(kMethodGenerateBat, TheGame->GetStringTable()->GetString(StringID('MTHD', kMethodGenerateBat))),
		getBatCountRegistration(kMethodGetBatCount, TheGame->GetStringTable()->GetString(StringID('MTHD', kMethodGetBatCount)), kMethodNoTarget | kMethodOutputValue)
{
}

Bat::~Bat()
{
}

void Bat::Construct(void)
{
	new(batStorage) Bat;
}

void Bat::Destruct(void)
{
	batStorage->~Bat();
}


inline BatConfiguration::BatConfiguration()
{
	batFlags = 0;
}

inline BatConfiguration::BatConfiguration(const BatConfiguration& batConfiguration)
{
	batFlags = batConfiguration.batFlags;
}

void BatConfiguration::Pack(Packer& data) const
{
	data << ChunkHeader('FLAG', 4);
	data << batFlags;
}

bool BatConfiguration::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> batFlags;
			return (true);
	}

	return (false);
}

int32 BatConfiguration::GetSettingCount(void) const
{
	return (0);
}

Setting *BatConfiguration::GetSetting(int32 index) const
{
	return (nullptr);
}

void BatConfiguration::SetSetting(const Setting *setting)
{
}


BatController::BatController() :
		MonsterController(kControllerBat), 
		frameAnimatorObserver(this, &BatController::HandleAnimationEvent)
{
	SetControllerFlags(kControllerAsleep); 

	SetMonsterHealth(10 << 16); 
	SetMonsterState(kBatStateSleeping);
	SetMonsterMotion(kBatMotionNone);
 
	turnTime = 0;
	turnRate = kBatTurnRate; 
	targetRoll = 0.0F; 
	currentRoll = 0.0F;
}

BatController::BatController(const BatConfiguration& configuration) : 
		MonsterController(kControllerBat),
		BatConfiguration(configuration),
		frameAnimatorObserver(this, &BatController::HandleAnimationEvent)
{
	SetControllerFlags(kControllerAsleep);

	SetMonsterHealth(10 << 16);
	SetMonsterState(kBatStateSleeping);
	SetMonsterMotion(kBatMotionNone);

	turnTime = 0;
	turnRate = kBatTurnRate;
	targetRoll = 0.0F;
	currentRoll = 0.0F;
}

BatController::BatController(const BatController& batController) :
		MonsterController(batController),
		BatConfiguration(batController),
		frameAnimatorObserver(this, &BatController::HandleAnimationEvent)
{
	SetControllerFlags(kControllerAsleep);

	turnTime = 0;
	turnRate = kBatTurnRate;
	targetRoll = 0.0F;
	currentRoll = 0.0F;
}

BatController::~BatController()
{
	UncountMonster(globalBatCount);
}

Controller *BatController::Replicate(void) const
{
	return (new BatController(*this));
}

bool BatController::ValidNode(const Node *node)
{
	return ((node->GetNodeType() == kNodeModel) && (static_cast<const Model *>(node)->GetModelType() == kModelBat));
}

void BatController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	MonsterController::Pack(data, packFlags);
	BatConfiguration::Pack(data);

	if (!(packFlags & kPackEditor))
	{
		if (GetMonsterState() == kBatStateEscaping)
		{
			data << ChunkHeader('WTTM', 4);
			data << waitTime;

			data << ChunkHeader('ESCP', 4);
			data << escapeAzimuth;
		}

		data << ChunkHeader('TURN', 8);
		data << turnTime;
		data << turnRate;

		data << ChunkHeader('ROLL', 8);
		data << targetRoll;
		data << currentRoll;
	}

	data << TerminatorChunk;
}

void BatController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	MonsterController::Unpack(data, unpackFlags);
	UnpackChunkList<BatController>(data, unpackFlags);
}

bool BatController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'WTTM':

			data >> waitTime;
			return (true);

		case 'ESCP':

			data >> escapeAzimuth;
			return (true);

		case 'TURN':

			data >> turnTime;
			data >> turnRate;
			return (true);

		case 'ROLL':

			data >> targetRoll;
			data >> currentRoll;
			return (true);
	}

	return (BatConfiguration::UnpackChunk(chunkHeader, data));
}

int32 BatController::GetSettingCount(void) const
{
	return (BatConfiguration::GetSettingCount());
}

Setting *BatController::GetSetting(int32 index) const
{
	return (BatConfiguration::GetSetting(index));
}

void BatController::SetSetting(const Setting *setting)
{
	BatConfiguration::SetSetting(setting);
}

void BatController::Preprocess(void)
{
	Model *model = GetTargetNode();
	if (!model->GetManipulator())
	{
		SetCharacterState(kCharacterFlying);
		SetGravityMultiplier(0.0F);
	}

	MonsterController::Preprocess();

	if (!model->GetManipulator())
	{
		Node *node = model->GetFirstSubnode();
		while (node)
		{
			if (node->GetNodeType() == kNodeSource)
			{
				batSource = static_cast<OmniSource *>(node);
				batSource->SetSourceVelocity(GetLinearVelocity());
				break;
			}

			node = node->Next();
		}

		GetAnimationBlender()->SetFrameAnimatorObserver(&frameAnimatorObserver);
	}
}

void BatController::Wake(void)
{
	MonsterController::Wake();

	if (GetMonsterState() != kBatStateDead)
	{
		if (GetMonsterMotion() == kBatMotionNone)
		{
			SetMonsterState(kBatStateFlying);
			SetBatMotion(kBatMotionFly);
		}
	}

	CountMonster(globalBatCount);
}

void BatController::Move(void)
{
	if ((this->*batStateHandler[GetMonsterState()])())
	{
		float roll = currentRoll;
		if (roll != targetRoll)
		{
			float dr = TheTimeMgr->GetFloatDeltaTime() * 2.5e-5F;
			if (targetRoll > roll)
			{
				roll = Fmin(roll + dr, targetRoll);
				currentRoll = roll;
			}
			else
			{
				roll = Fmax(roll - dr, targetRoll);
				currentRoll = roll;
			}
		}

		SetMonsterRoll(roll * -400.0F);
		MonsterController::Move();
	}
}

bool BatController::HandleSleepingState(void)
{
	return (true);
}

bool BatController::HandleFlyingState(void)
{
	Vector3D	direction;

	UpdateTurnRate(TheTimeMgr->GetDeltaTime());

	Vector3D force = GetRepulsionForce();

	if (GetEnemyDirection(&direction))
	{
		float targetDistance = Magnitude(direction.GetVector2D());
		if (targetDistance > kBatAttackDistance)
		{
			float azm = SetMonsterAzimuth(Atan(direction.y, direction.x), turnRate, &targetRoll);
			float m = (Fmin(targetDistance * 0.2F, 1.25F) + 0.5F) * kBatFlyForce;
			force.GetVector2D() += CosSin(azm) * m;

			float h = Clamp(targetDistance * 0.4F, 1.5F, 25.0F);
			force.z = (h + direction.z) * kBatFlyForce * 0.5F;
		}
		else
		{
			SetMonsterState(kBatStateAttacking);
			SetBatMotion(kBatMotionAttack);
		}
	}

	if (!batSource->Playing())
	{
		batSource->Play();
	}

	batSource->SetSourceVelocity(GetLinearVelocity());

	SetExternalForce(force);
	SetExternalLinearResistance(Vector3D(kBatFlyResistForce, kBatFlyResistForce, kBatFlyResistForce));
	return (true);
}

bool BatController::HandleEscapingState(void)
{
	Vector3D	direction;

	int32 dt = TheTimeMgr->GetDeltaTime();
	waitTime = MaxZero(waitTime - dt);
	if (waitTime <= 0)
	{
		SetMonsterState(kBatStateFlying);
	}

	UpdateTurnRate(dt);

	Vector3D force = GetRepulsionForce();

	float azm = SetMonsterAzimuth(escapeAzimuth, turnRate, &targetRoll);
	force.GetVector2D() += CosSin(azm) * kBatFlyForce;

	if (GetEnemyDirection(&direction))
	{
		float h = Clamp(Magnitude(direction.GetVector2D()) * 0.4F, 1.5F, 25.0F);
		force.z = (h + direction.z) * kBatFlyForce * 0.5F;
	}

	batSource->SetSourceVelocity(GetLinearVelocity());

	SetExternalForce(force);
	SetExternalLinearResistance(Vector3D(kBatFlyResistForce, kBatFlyResistForce, kBatFlyResistForce));
	return (true);
}

bool BatController::HandleAttackingState(void)
{
	Vector3D	direction;

	if (GetEnemyDirection(&direction))
	{
		SetMonsterAzimuth(Atan(direction.y, direction.x), turnRate, &targetRoll);
	}

	batSource->SetSourceVelocity(GetLinearVelocity());

	SetExternalForce(GetRepulsionForce());
	SetExternalLinearResistance(Vector3D(kBatFlyResistForce, kBatFlyResistForce, kBatFlyResistForce));
	return (true);
}

bool BatController::HandleDeadState(void)
{
	if (GetStateTime() >= 10000)
	{
		delete GetTargetNode();
		return (false);
	}

	SetExternalForce(Zero3D);
	if (GetCharacterState() & kCharacterGround)
	{
		SetExternalLinearResistance(Vector2D(kBatFlyResistForce, kBatFlyResistForce));

		FrameAnimator *animator = GetAnimationBlender()->GetRecentAnimator();
		Interpolator *interpolator = animator->GetFrameInterpolator();
		float duration = animator->GetAnimationDuration();
		if (interpolator->GetRange().max < duration)
		{
			interpolator->SetMaxValue(duration);
			interpolator->SetMode(kInterpolatorForward);
		}
	}
	else
	{
		SetExternalLinearResistance(Vector2D(kBatDeathResistForce, kBatDeathResistForce));
	}

	return (true);
}

void BatController::HandleAnimationEvent(FrameAnimator *animator, CueType cueType)
{
	if (cueType == 'DAMG')
	{
		DamageEnemy(7 << 16, kBatAttackDistance);
	}
}

void BatController::UpdateTurnRate(int32 dt)
{
	int32 time = turnTime - dt;
	turnTime = time;
	if (time <= 0)
	{
		if (turnRate > kBatTurnRate)
		{
			turnTime = Math::Random(200) + 50;
			turnRate = kBatTurnRate;
		}
		else
		{
			turnTime = Math::Random(50) + 75;
			turnRate = (Math::RandomFloat(1.5F) + 1.5F) * kBatTurnRate;
		}
	}
}

void BatController::SetBatMotion(int32 motion)
{
	SetMonsterMotion(motion);
	switch (motion)
	{
		case kBatMotionFly:
		{
			FrameAnimator *animator = GetAnimationBlender()->StartAnimation("bat/Fly", kInterpolatorForward | kInterpolatorLoop);
			animator->GetFrameInterpolator()->SetRate(2.0F);
			break;
		}

		case kBatMotionAttack:
		{
			static const char attackName[2][16] =
			{
				"bat/Attack1", "bat/Attack2"
			};

			FrameAnimator *animator = GetAnimationBlender()->BlendAnimation("bat/Attack", kInterpolatorForward, 0.004F, &AttackCallback, this);
			animator->GetFrameInterpolator()->SetRate(1.5F);

			PlaySource(attackName[Math::Random(2)], 64.0F);
			break;
		}

		case kBatMotionDeath:
		{
			static const char deathName[2][16] =
			{
				"bat/Death1", "bat/Death2"
			};

			FrameAnimator *animator = GetAnimationBlender()->BlendAnimation("bat/Death", kInterpolatorForward, 0.004F);
			Interpolator *interpolator = animator->GetFrameInterpolator();
			interpolator->SetMaxValue(825.0F);
			interpolator->SetRate(2.0F);

			batSource->Stop();
			PlaySource(deathName[Math::Random(2)], 64.0F);

			SetGravityMultiplier(1.0F);
			SetCharacterState(GetCharacterState() & ~kCharacterFlying);
			targetRoll = 0.0F;
			break;
		}
	}
}

void BatController::AttackCallback(Interpolator *interpolator, void *cookie)
{
	BatController *batController = static_cast<BatController *>(cookie);
	batController->SetMonsterState(kBatStateEscaping);
	batController->SetBatMotion(kBatMotionFly);

	batController->waitTime = Math::Random(600) + 400;
	batController->escapeAzimuth = Math::RandomFloat(K::tau) - K::tau_over_2;
}

CharacterStatus BatController::Damage(Fixed damage, unsigned_int32 flags, GameCharacterController *attacker, const Point3D *position, const Vector3D *force)
{
	Model *model = GetTargetNode();

	if (!position)
	{
		position = &GetWorldCenterOfMass();
	}

	if (!(flags & kDamageBloodInhibit))
	{
		int32 particleCount = damage >> 14;
		if (particleCount > 0)
		{
			BloodParticleSystem *blood = new BloodParticleSystem(ColorRGB(0.5F, 0.0F, 0.0F), particleCount);
			blood->SetNodePosition(*position);
			model->GetWorld()->AddNewNode(blood);
		}
	}

	return (MonsterController::Damage(damage, flags, attacker, position, force));
}

void BatController::Kill(GameCharacterController *attacker, const Point3D *position, const Vector3D *force)
{
	MonsterController::Kill(attacker, position, force);

	UncountMonster(globalBatCount);
	SetMonsterState(kBatStateDead);

	SetBatMotion(kBatMotionDeath);
	SetExternalForce(Zero3D);
}


GenerateBatMethod::GenerateBatMethod() : GenerateMonsterMethod(kMethodGenerateBat)
{
}

GenerateBatMethod::GenerateBatMethod(const GenerateBatMethod& generateBatMethod) :
		GenerateMonsterMethod(generateBatMethod),
		BatConfiguration(generateBatMethod)
{
}

GenerateBatMethod::~GenerateBatMethod()
{
}

Method *GenerateBatMethod::Replicate(void) const
{
	return (new GenerateBatMethod(*this));
}

void GenerateBatMethod::Pack(Packer& data, unsigned_int32 packFlags) const
{
	GenerateMonsterMethod::Pack(data, packFlags);
	BatConfiguration::Pack(data);
	data << TerminatorChunk;
}

void GenerateBatMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	GenerateMonsterMethod::Unpack(data, unpackFlags);
	UnpackChunkList<GenerateBatMethod>(data, unpackFlags);
}

bool GenerateBatMethod::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	return (BatConfiguration::UnpackChunk(chunkHeader, data));
}

int32 GenerateBatMethod::GetSettingCount(void) const
{
	return (BatConfiguration::GetSettingCount());
}

Setting *GenerateBatMethod::GetSetting(int32 index) const
{
	return (BatConfiguration::GetSetting(index));
}

void GenerateBatMethod::SetSetting(const Setting *setting)
{
	BatConfiguration::SetSetting(setting);
}

void GenerateBatMethod::Execute(const ScriptState *state)
{
	Node *node = GetTargetNode(state);
	if (node)
	{
		GamePlayer *player = static_cast<GamePlayer *>(TheMessageMgr->GetLocalPlayer());
		if (player->GetPlayerController())
		{
			Model *model = Model::Get(kModelBat);
			BatController *controller = new BatController(*this);
			GenerateMonster(node, model, controller);
		}
	}

	CallCompletionProc();
}


GetBatCountMethod::GetBatCountMethod() : Method(kMethodGetBatCount)
{
}

GetBatCountMethod::GetBatCountMethod(const GetBatCountMethod& getBatCountMethod) : Method(getBatCountMethod)
{
}

GetBatCountMethod::~GetBatCountMethod()
{
}

Method *GetBatCountMethod::Replicate(void) const
{
	return (new GetBatCountMethod(*this));
}

void GetBatCountMethod::Execute(const ScriptState *state)
{
	SetOutputValue(state, BatController::GetGlobalBatCount());
	CallCompletionProc();
}

// ZYUQURM
