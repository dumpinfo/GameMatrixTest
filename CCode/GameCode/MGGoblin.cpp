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


#include "MGGoblin.h"
#include "MGGame.h"


using namespace C4;


namespace
{
	const float kGoblinRunForce = 200.0F;
	const float kGoblinResistForce = 20.0F;
	const float kGoblinTurnRate = 0.01F;
	const float kGoblinAttackDistance = 2.0F;

	Storage<Goblin> goblinStorage;

	const Type goblinSkullType[kGoblinSkullCount] =
	{
		kGoblinSkullGoat, kGoblinSkullDeer
	};
}


int32 GoblinController::globalGoblinCount = 0;

bool (GoblinController::*GoblinController::goblinStateHandler[kGoblinStateCount])(void) =
{
	&GoblinController::HandleSleepingState,
	&GoblinController::HandleStandingState,
	&GoblinController::HandleCreepingState,
	&GoblinController::HandleChasingState,
	&GoblinController::HandlePreleapingState,
	&GoblinController::HandleLeapingState,
	&GoblinController::HandleLandingState,
	&GoblinController::HandleAttackingState,
	&GoblinController::HandleRecoveringState,
	&GoblinController::HandleDeadState
};


Goblin::Goblin() :
		goblinControllerRegistration(kControllerGoblin, TheGame->GetStringTable()->GetString(StringID('CTRL', kControllerGoblin))),
		goblinModelRegistration(kModelGoblin, TheGame->GetStringTable()->GetString(StringID('MODL', 'MNST', kModelGoblin)), "goblin/Goblin", kModelPrecache, kControllerGoblin),
		goblinRagdollModelRegistration(kModelGoblinRagdoll, nullptr, "goblin/Ragdoll", kModelPrecache | kModelPrivate),
		goatSkullModelRegistration(kModelGoatSkull, nullptr, "goblin/Skull-goat", kModelPrivate),
		deerSkullModelRegistration(kModelDeerSkull, nullptr, "goblin/Skull-deer", kModelPrivate),

		generateGoblinRegistration(kMethodGenerateGoblin, TheGame->GetStringTable()->GetString(StringID('MTHD', kMethodGenerateGoblin))),
		getGoblinCountRegistration(kMethodGetGoblinCount, TheGame->GetStringTable()->GetString(StringID('MTHD', kMethodGetGoblinCount)), kMethodNoTarget | kMethodOutputValue)
{
}

Goblin::~Goblin()
{
}

void Goblin::Construct(void)
{
	new(goblinStorage) Goblin;
}

void Goblin::Destruct(void)
{
	goblinStorage->~Goblin();
}


inline GoblinConfiguration::GoblinConfiguration()
{
	goblinFlags = 0;
	goblinSkull = kGoblinSkullNone;
}

inline GoblinConfiguration::GoblinConfiguration(const GoblinConfiguration& goblinConfiguration)
{
	goblinFlags = goblinConfiguration.goblinFlags;
	goblinSkull = goblinConfiguration.goblinSkull;
}

void GoblinConfiguration::Pack(Packer& data) const
{
	data << ChunkHeader('FLAG', 4);
	data << goblinFlags;

	data << ChunkHeader('SKUL', 4);
	data << goblinSkull;
}

bool GoblinConfiguration::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> goblinFlags;
			return (true);

		case 'SKUL':

			data >> goblinSkull;
			return (true); 
	}

	return (false); 
}
 
int32 GoblinConfiguration::GetSettingCount(void) const
{
	return (1); 
}
 
Setting *GoblinConfiguration::GetSetting(int32 index) const 
{
	const StringTable *table = TheGame->GetStringTable();

	if (index == 0) 
	{
		int32 selection = 0;
		if (goblinSkull == kGoblinSkullRandom)
		{
			selection = 1;
		}
		else
		{
			for (machine a = 0; a < kGoblinSkullCount; a++)
			{
				if (goblinSkull == goblinSkullType[a])
				{
					selection = a + 2;
					break;
				}
			}
		}

		const char *title = table->GetString(StringID('CTRL', kControllerGoblin, 'SKUL'));
		MenuSetting *menu = new MenuSetting('SKUL', selection, title, kGoblinSkullCount + 2);
		menu->SetMenuItemString(0, table->GetString(StringID('CTRL', kControllerGoblin, 'SKUL', 'NONE')));
		menu->SetMenuItemString(1, table->GetString(StringID('CTRL', kControllerGoblin, 'SKUL', 'RAND')));
		for (machine a = 0; a < kGoblinSkullCount; a++)
		{
			menu->SetMenuItemString(a + 2, table->GetString(StringID('CTRL', kControllerGoblin, 'SKUL', goblinSkullType[a])));
		}

		return (menu);
	}

	return (nullptr);
}

void GoblinConfiguration::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'SKUL')
	{
		int32 selection = static_cast<const MenuSetting *>(setting)->GetMenuSelection();
		if (selection == 0)
		{
			goblinSkull = kGoblinSkullNone;
		}
		else if (selection == 1)
		{
			goblinSkull = kGoblinSkullRandom;
		}
		else
		{
			goblinSkull = goblinSkullType[selection - 2];
		}
	}
}


inline void GoblinController::NewSoundTime(void)
{
	soundTime = Math::Random(5000) + 3000;
}

inline void GoblinController::NewLeapDistance(void)
{
	leapDistance = Math::RandomFloat(8.0F) + 5.0F;
}

GoblinController::GoblinController() :
		MonsterController(kControllerGoblin),
		frameAnimatorObserver(this, &GoblinController::HandleAnimationEvent)
{
	SetControllerFlags(kControllerAsleep);

	SetMonsterState(kGoblinStateSleeping);
	SetMonsterMotion(kGoblinMotionNone);

	SetSourcePosition(Point3D(0.0F, 0.0F, 1.1F));

	NewSoundTime();
	NewLeapDistance();
}

GoblinController::GoblinController(const GoblinConfiguration& configuration) :
		MonsterController(kControllerGoblin),
		GoblinConfiguration(configuration),
		frameAnimatorObserver(this, &GoblinController::HandleAnimationEvent)
{
	SetControllerFlags(kControllerAsleep);

	SetMonsterState(kGoblinStateSleeping);
	SetMonsterMotion(kGoblinMotionNone);

	SetSourcePosition(Point3D(0.0F, 0.0F, 1.1F));

	NewSoundTime();
	NewLeapDistance();
}

GoblinController::GoblinController(const GoblinController& goblinController) :
		MonsterController(goblinController),
		GoblinConfiguration(goblinController),
		frameAnimatorObserver(this, &GoblinController::HandleAnimationEvent)
{
	SetControllerFlags(kControllerAsleep);

	NewSoundTime();
	NewLeapDistance();
}

GoblinController::~GoblinController()
{
	UncountMonster(globalGoblinCount);
}

Controller *GoblinController::Replicate(void) const
{
	return (new GoblinController(*this));
}

bool GoblinController::ValidNode(const Node *node)
{
	return ((node->GetNodeType() == kNodeModel) && (static_cast<const Model *>(node)->GetModelType() == kModelGoblin));
}

void GoblinController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	MonsterController::Pack(data, packFlags);
	GoblinConfiguration::Pack(data);

	if (!(packFlags & kPackEditor))
	{
		data << ChunkHeader('SDTM', 4);
		data << soundTime;

		data << ChunkHeader('LEAP', 4);
		data << leapDistance;

		if (GetMonsterState() == kGoblinStateStanding)
		{
			data << ChunkHeader('WTTM', 4);
			data << waitTime;
		}
	}

	data << TerminatorChunk;
}

void GoblinController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	MonsterController::Unpack(data, unpackFlags);
	UnpackChunkList<GoblinController>(data, unpackFlags);
}

bool GoblinController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'SDTM':

			data >> soundTime;
			return (true);

		case 'LEAP':

			data >> leapDistance;
			return (true);

		case 'WTTM':

			data >> waitTime;
			return (true);
	}

	return (GoblinConfiguration::UnpackChunk(chunkHeader, data));
}

int32 GoblinController::GetSettingCount(void) const
{
	return (GoblinConfiguration::GetSettingCount());
}

Setting *GoblinController::GetSetting(int32 index) const
{
	return (GoblinConfiguration::GetSetting(index));
}

void GoblinController::SetSetting(const Setting *setting)
{
	GoblinConfiguration::SetSetting(setting);
}

void GoblinController::Preprocess(void)
{
	const Model *model = GetTargetNode();
	if (!model->GetManipulator())
	{
		unsigned_int32 monsterFlags = GetMonsterFlags();
		if (!(monsterFlags & kMonsterInitialized))
		{
			SetMonsterFlags(monsterFlags | kMonsterInitialized);

			if (goblinSkull == kGoblinSkullRandom)
			{
				goblinSkull = goblinSkullType[Math::Random(kGoblinSkullCount)];
			}

			int32 health = 40;
			if (goblinSkull != kGoblinSkullNone)
			{
				health += 30;
			}

			SetMonsterHealth(health << 16);
		}

		if (goblinSkull != kGoblinSkullNone)
		{
			Node *headNode = model->FindNode(Text::StaticHash<'G', 'o', 'b', 'l', 'i', 'n', '_', 'H', 'e', 'a', 'd'>::value);

			ModelType type = (goblinSkull == kGoblinSkullGoat) ? kModelGoatSkull : kModelDeerSkull;
			Model *skull = Model::Get(type);

			skull->SetNodeFlags(kNodeNonpersistent | kNodeCloneInhibit | kNodeAnimateInhibit);
			headNode->AppendNewSubnode(skull);
		}
	}

	MonsterController::Preprocess();

	if (!model->GetManipulator())
	{
		SetAttackable(true);

		AnimationBlender *animationBlender = GetAnimationBlender();
		animationBlender->SetFrameAnimatorObserver(&frameAnimatorObserver);

		int32 motion = GetMonsterMotion();
		Interpolator *interpolator = animationBlender->GetRecentAnimator()->GetFrameInterpolator();

		if (motion == kGoblinMotionLand)
		{
			interpolator->SetCompletionProc(&LandCallback, this);
		}
		else if (motion == kGoblinMotionAttack)
		{
			interpolator->SetCompletionProc(&AttackCallback, this);
		}
		else if (motion == kGoblinMotionDamage)
		{
			interpolator->SetCompletionProc(&DamageCallback, this);
		}
	}
}

void GoblinController::Wake(void)
{
	MonsterController::Wake();

	if (GetMonsterState() != kGoblinStateDead)
	{
		if (GetMonsterMotion() == kGoblinMotionNone)
		{
			/*if (Math::Random(2) == 0)
			{
				goblinFlags |= kGoblinLeapAttack;
			}*/

			GetAnimationBlender()->StartAnimation("goblin/Stand2", kInterpolatorStop);
			SetMonsterState(kGoblinStateChasing);
			SetGoblinMotion(kGoblinMotionChase);
		}
	}

	CountMonster(globalGoblinCount);
}

void GoblinController::Move(void)
{
	if ((this->*goblinStateHandler[GetMonsterState()])())
	{
		MonsterController::Move();
	}
}

bool GoblinController::HandleSleepingState(void)
{
	return (true);
}

bool GoblinController::HandleStandingState(void)
{
	if (GetStateTime() >= waitTime)
	{
		Vector3D	direction;

		if (GetEnemyDirection(&direction))
		{
			if (Magnitude(direction) > kGoblinAttackDistance)
			{
				SetMonsterState(kGoblinStateChasing);
				SetGoblinMotion(kGoblinMotionChase);
			}
			else
			{
				SetMonsterState(kGoblinStateAttacking);
				SetGoblinMotion(kGoblinMotionAttack);
			}
		}
	}

	SetExternalForce(GetRepulsionForce());
	SetExternalLinearResistance(Vector2D(kGoblinResistForce, kGoblinResistForce));
	return (true);
}

bool GoblinController::HandleCreepingState(void)
{
	return (true);
}

bool GoblinController::HandleChasingState(void)
{
	Vector3D	direction;

	if ((soundTime -= TheTimeMgr->GetDeltaTime()) <= 0)
	{
		NewSoundTime();
		PlaySource("goblin/Goblin", 32.0F);
	}

	Vector2D force = GetRepulsionForce();

	if (GetEnemyDirection(&direction))
	{
		float targetDistance = Magnitude(direction);
		float azm0 = Atan(direction.y, direction.x);
		float azm = SetMonsterAzimuth(azm0, kGoblinTurnRate);

		if ((targetDistance > kGoblinAttackDistance) || (GetStateTime() < 100))
		{
			force += CosSin(azm) * kGoblinRunForce;

			if (goblinFlags & kGoblinLeapAttack)
			{
				if ((targetDistance < leapDistance) && (targetDistance > leapDistance * 0.8F) && (Fabs(azm - azm0) < 0.5F))
				{
					SetMonsterState(kGoblinStatePreleaping);
					SetGoblinMotion(kGoblinMotionLeap);

					SetCharacterState(kCharacterJumping);
				}
			}
		}
		else
		{
			SetMonsterState(kGoblinStateAttacking);
			SetGoblinMotion(kGoblinMotionAttack);
		}
	}

	if (GetCharacterState() & kCharacterGround)
	{
		SetExternalLinearResistance(Vector2D(kGoblinResistForce, kGoblinResistForce));
	}
	else
	{
		force *= 0.02F;
		SetExternalLinearResistance(Zero2D);
	}

	SetExternalForce(force);
	return (true);
}

bool GoblinController::HandlePreleapingState(void)
{
	if (GetStateTime() >= 231)
	{
		Vector3D	direction;

		if (GetEnemyDirection(&direction))
		{
			float f = 5.0F / Sqrt(direction.x * direction.x + direction.y * direction.y);
			ApplyImpulse(GetTargetNode()->GetInverseWorldTransform() * Vector3D(direction.x * f, direction.y * f, 1.75F));

			SetMonsterState(kGoblinStateLeaping);

			SetExternalForce(Zero3D);
			SetExternalLinearResistance(Zero2D);
			return (true);
		}
	}

	SetExternalForce(GetRepulsionForce());
	SetExternalLinearResistance(Vector2D(kGoblinResistForce, kGoblinResistForce));
	return (true);
}

bool GoblinController::HandleLeapingState(void)
{
	if ((GetStateTime() >= 250) && (GetCharacterState() & kCharacterGround))
	{
		SetMonsterState(kGoblinStateLanding);
		SetGoblinMotion(kGoblinMotionLand);
	}

	return (true);
}

bool GoblinController::HandleLandingState(void)
{
	SetExternalForce(GetRepulsionForce());
	SetExternalLinearResistance(Vector2D(kGoblinResistForce, kGoblinResistForce));
	return (true);
}

bool GoblinController::HandleAttackingState(void)
{
	Vector3D	direction;

	if (GetEnemyDirection(&direction))
	{
		SetMonsterAzimuth(Atan(direction.y, direction.x), kGoblinTurnRate);
	}

	SetExternalForce(GetRepulsionForce());
	SetExternalLinearResistance(Vector2D(kGoblinResistForce, kGoblinResistForce));
	return (true);
}

bool GoblinController::HandleRecoveringState(void)
{
	SetExternalForce(GetRepulsionForce());
	SetExternalLinearResistance(Vector2D(kGoblinResistForce, kGoblinResistForce));
	return (true);
}

bool GoblinController::HandleDeadState(void)
{
	if (GetStateTime() >= 10000)
	{
		delete GetTargetNode();
		return (false);
	}

	SetExternalForce(Zero3D);
	SetExternalLinearResistance(Vector2D(kGoblinResistForce, kGoblinResistForce));
	return (true);
}

void GoblinController::HandleAnimationEvent(FrameAnimator *animator, CueType cueType)
{
	switch (cueType)
	{
		case 'DAMG':

			DamageEnemy(15 << 16, kGoblinAttackDistance);
			break;
	}
}

void GoblinController::SetGoblinMotion(int32 motion)
{
	SetMonsterMotion(motion);
	switch (motion)
	{
		case kGoblinMotionStand:

			GetAnimationBlender()->BlendAnimation("goblin/Attack", kInterpolatorStop, 0.004F);
			break;

		case kGoblinMotionChase:

			GetAnimationBlender()->BlendAnimation("goblin/Run2", kInterpolatorForward | kInterpolatorLoop, 0.004F);
			break;

		case kGoblinMotionLeap:
		{
			static const char leapName[2][16] =
			{
				"goblin/Leap1", "goblin/Leap2"
			};

			static const float leapRange[2] =
			{
				396.0F, 462.0F
			};

			int32 index = Math::Random(2);
			FrameAnimator *animator = GetAnimationBlender()->StartAnimation(leapName[index], kInterpolatorForward);
			animator->GetFrameInterpolator()->SetMaxValue(leapRange[index]);
			break;
		}

		case kGoblinMotionLand:
		{
			FrameAnimator *animator = GetAnimationBlender()->GetFrameAnimator(0);
			Interpolator *interpolator = animator->GetFrameInterpolator();
			interpolator->SetMaxValue(animator->GetAnimationDuration());
			interpolator->SetMode(kInterpolatorForward);
			interpolator->SetCompletionProc(&LandCallback, this);
			break;
		}

		case kGoblinMotionAttack:
		{
			static const char attackName[2][16] =
			{
				"goblin/Attack1", "goblin/Attack2"
			};

			GetAnimationBlender()->BlendAnimation("goblin/Attack", kInterpolatorForward, 0.004F, &AttackCallback, this);

			OmniSource *source = PlaySource(attackName[Math::Random(2)], 32.0F);
			source->SetSourceFrequency(Math::RandomFloat(0.2F) + 1.0F);
			break;
		}

		case kGoblinMotionDamage:
		{
			static const char damageName[2][16] =
			{
				"goblin/Damage1", "goblin/Damage2"
			};

			FrameAnimator *animator = GetAnimationBlender()->BlendAnimation(damageName[Math::Random(2)], kInterpolatorForward, 0.004F, &DamageCallback, this);
			animator->GetFrameInterpolator()->SetRate(1.5F);
			break;
		}

		case kGoblinMotionDeath:
		{
			FrameAnimator *animator = GetAnimationBlender()->BlendAnimation("goblin/Death", kInterpolatorForward, 0.004F);
			Interpolator *interpolator = animator->GetFrameInterpolator();
			interpolator->SetRange(231.0F, animator->GetAnimationDuration());
			interpolator->SetValue(231.0F);

			PlaySource("goblin/Death", 64.0F);
			break;
		}
	}
}

void GoblinController::LandCallback(Interpolator *interpolator, void *cookie)
{
	GoblinController *goblinController = static_cast<GoblinController *>(cookie);
	goblinController->SetMonsterState(kGoblinStateStanding);
	goblinController->SetGoblinMotion(kGoblinMotionStand);
	goblinController->waitTime = 250;
}

void GoblinController::AttackCallback(Interpolator *interpolator, void *cookie)
{
	GoblinController *goblinController = static_cast<GoblinController *>(cookie);
	goblinController->SetMonsterState(kGoblinStateStanding);
	goblinController->SetGoblinMotion(kGoblinMotionStand);
	goblinController->waitTime = Math::Random(300) + 200;
}

void GoblinController::DamageCallback(Interpolator *interpolator, void *cookie)
{
	GoblinController *goblinController = static_cast<GoblinController *>(cookie);
	goblinController->SetMonsterState(kGoblinStateStanding);
	goblinController->SetGoblinMotion(kGoblinMotionStand);
	goblinController->waitTime = Math::Random(200);
}

RigidBodyStatus GoblinController::HandleNewRigidBodyContact(const RigidBodyContact *contact, RigidBodyController *contactBody)
{
	if (contactBody->GetRigidBodyType() == kRigidBodyCharacter)
	{
		GameCharacterController *character = static_cast<GameCharacterController *>(contactBody);
		if (character->GetCharacterType() == kCharacterPlayer)
		{
			if (GetMonsterState() == kGoblinStateLeaping)
			{
				SetMonsterState(kGoblinStateAttacking);
				SetGoblinMotion(kGoblinMotionAttack);
			}

			return (kRigidBodyUnchanged);
		}
	}

	return (MonsterController::HandleNewRigidBodyContact(contact, contactBody));
}

CharacterStatus GoblinController::Damage(Fixed damage, unsigned_int32 flags, GameCharacterController *attacker, const Point3D *position, const Vector3D *force)
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
			BloodParticleSystem *blood = new BloodParticleSystem(ColorRGB(0.0F, 0.625F, 0.25F), particleCount);
			blood->SetNodePosition(*position);
			model->GetWorld()->AddNewNode(blood);
		}
	}

	if (DamageTimeExpired(200))
	{
		static const char soundName[2][16] =
		{
			"goblin/Damage1", "goblin/Damage2"
		};

		OmniSource *source = PlaySource(soundName[Math::Random(2)], 32.0F);
		source->SetSourceFrequency(Math::RandomFloat(0.2F) + 0.8F);
	}

	CharacterStatus status = MonsterController::Damage(damage, flags, attacker, position, force);
	if ((status == kCharacterDamaged) && (GetMonsterState() == kGoblinStateChasing))
	{
		if (Math::Random(3) == 0)
		{
			SetMonsterState(kGoblinStateRecovering);
			SetGoblinMotion(kGoblinMotionDamage);
		}
	}

	return (status);
}

void GoblinController::Kill(GameCharacterController *attacker, const Point3D *position, const Vector3D *force)
{
	MonsterController::Kill(attacker, position, force);

	UncountMonster(globalGoblinCount);

	SetMonsterState(kGoblinStateDead);
	SetGoblinMotion(kGoblinMotionDeath);

	//AttachRagdoll(kModelGoblinRagdoll, position, force);
}


GenerateGoblinMethod::GenerateGoblinMethod() : GenerateMonsterMethod(kMethodGenerateGoblin)
{
}

GenerateGoblinMethod::GenerateGoblinMethod(const GenerateGoblinMethod& generateGoblinMethod) :
		GenerateMonsterMethod(generateGoblinMethod),
		GoblinConfiguration(generateGoblinMethod)
{
}

GenerateGoblinMethod::~GenerateGoblinMethod()
{
}

Method *GenerateGoblinMethod::Replicate(void) const
{
	return (new GenerateGoblinMethod(*this));
}

void GenerateGoblinMethod::Pack(Packer& data, unsigned_int32 packFlags) const
{
	GenerateMonsterMethod::Pack(data, packFlags);
	GoblinConfiguration::Pack(data);
	data << TerminatorChunk;
}

void GenerateGoblinMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	GenerateMonsterMethod::Unpack(data, unpackFlags);
	UnpackChunkList<GenerateGoblinMethod>(data, unpackFlags);
}

bool GenerateGoblinMethod::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	return (GoblinConfiguration::UnpackChunk(chunkHeader, data));
}

int32 GenerateGoblinMethod::GetSettingCount(void) const
{
	return (GoblinConfiguration::GetSettingCount());
}

Setting *GenerateGoblinMethod::GetSetting(int32 index) const
{
	return (GoblinConfiguration::GetSetting(index));
}

void GenerateGoblinMethod::SetSetting(const Setting *setting)
{
	GoblinConfiguration::SetSetting(setting);
}

void GenerateGoblinMethod::Execute(const ScriptState *state)
{
	Node *node = GetTargetNode(state);
	if (node)
	{
		GamePlayer *player = static_cast<GamePlayer *>(TheMessageMgr->GetLocalPlayer());
		if (player->GetPlayerController())
		{
			Model *model = Model::Get(kModelGoblin);
			GoblinController *controller = new GoblinController(*this);
			GenerateMonster(node, model, controller);
		}
	}

	CallCompletionProc();
}


GetGoblinCountMethod::GetGoblinCountMethod() : Method(kMethodGetGoblinCount)
{
}

GetGoblinCountMethod::GetGoblinCountMethod(const GetGoblinCountMethod& getGoblinCountMethod) : Method(getGoblinCountMethod)
{
}

GetGoblinCountMethod::~GetGoblinCountMethod()
{
}

Method *GetGoblinCountMethod::Replicate(void) const
{
	return (new GetGoblinCountMethod(*this));
}

void GetGoblinCountMethod::Execute(const ScriptState *state)
{
	SetOutputValue(state, GoblinController::GetGlobalGoblinCount());
	CallCompletionProc();
}

// ZYUQURM
