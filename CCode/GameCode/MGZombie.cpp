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


#include "MGZombie.h"
#include "MGGame.h"


using namespace C4;


namespace
{
	const float kZombieWalkForce = 42.0F;
	const float kZombieRunForce = 135.0F;
	const float kZombieResistForce = 50.0F;
	const float kZombieTurnRate = 0.002F;
	const float kZombieFastTurnRate = 0.006F;
	const float kZombieAttackDistance = 1.5F;

	Storage<Zombie> zombieStorage;

	const Type zombieVariantType[kZombieVariantCount] =
	{
		kZombieVariantNormal, kZombieVariantTechnician, kZombieVariantScientist
	};

	const Type zombiePoseType[kZombiePoseCount] =
	{
		kZombiePoseWakeup, kZombiePoseRest1, kZombiePoseRest2, kZombiePoseRest3, kZombiePoseRest4
	};

	const char *const zombiePoseAnimation[kZombiePoseCount] =
	{
		"zombie/Wakeup", "zombie/Rest1", "zombie/Rest2", "zombie/Rest3", "zombie/Rest4"
	};
}


int32 ZombieController::globalZombieCount = 0;

bool (ZombieController::*ZombieController::zombieStateHandler[kZombieStateCount])(void) =
{
	&ZombieController::HandleSleepingState,
	&ZombieController::HandleStandingState,
	&ZombieController::HandleWakingState,
	&ZombieController::HandleChasingState,
	&ZombieController::HandleAttackingState,
	&ZombieController::HandleRecoveringState,
	&ZombieController::HandleDeadState
};


SharedVertexBuffer OpenGraveEffect::staticVertexBuffer(kVertexBufferAttribute | kVertexBufferStatic);


Zombie::Zombie() :
		zombieControllerRegistration(kControllerZombie, TheGame->GetStringTable()->GetString(StringID('CTRL', kControllerZombie))),
		zombieModelRegistration(kModelZombie, TheGame->GetStringTable()->GetString(StringID('MODL', 'MNST', kModelZombie)), "zombie/Zombie", kModelPrecache, kControllerZombie),
		zombieHardHatModelRegistration(kModelZombieHardHat, nullptr, "zombie/hardhat/Hardhat", kModelPrecache | kModelPrivate),

		technicianMaterialRegistration(kMaterialZombieTechnician, "zombie/Technician"),
		scientistMaterialRegistration(kMaterialZombieScientist, "zombie/Scientist"),

		generateZombieRegistration(kMethodGenerateZombie, TheGame->GetStringTable()->GetString(StringID('MTHD', kMethodGenerateZombie))),
		getZombieCountRegistration(kMethodGetZombieCount, TheGame->GetStringTable()->GetString(StringID('MTHD', kMethodGetZombieCount)), kMethodNoTarget | kMethodOutputValue)
{
}

Zombie::~Zombie()
{
}

void Zombie::Construct(void)
{
	new(zombieStorage) Zombie;
}

void Zombie::Destruct(void)
{
	zombieStorage->~Zombie();
}


inline ZombieConfiguration::ZombieConfiguration()
{
	zombieFlags = 0;
	zombieVariant = kZombieVariantNormal;
	zombiePose = kZombiePoseNone;
}

inline ZombieConfiguration::ZombieConfiguration(const ZombieConfiguration& zombieConfiguration)
{
	zombieFlags = zombieConfiguration.zombieFlags;
	zombieVariant = zombieConfiguration.zombieVariant;
	zombiePose = zombieConfiguration.zombiePose;
}

void ZombieConfiguration::Pack(Packer& data) const
{
	data << ChunkHeader('FLAG', 4);
	data << zombieFlags;

	data << ChunkHeader('VRNT', 4);
	data << zombieVariant; 

	data << ChunkHeader('POSE', 4);
	data << zombiePose; 
}
 
bool ZombieConfiguration::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data)
{
	switch (chunkHeader->chunkType) 
	{
		case 'FLAG': 
 
			data >> zombieFlags;
			return (true);

		case 'VRNT': 

			data >> zombieVariant;
			return (true);

		case 'POSE':

			data >> zombiePose;
			return (true);
	}

	return (false);
}

int32 ZombieConfiguration::GetSettingCount(void) const
{
	return (6);
}

Setting *ZombieConfiguration::GetSetting(int32 index) const
{
	const StringTable *table = TheGame->GetStringTable();

	if (index == 0)
	{
		int32 selection = 0;
		for (machine a = 1; a < kZombieVariantCount; a++)
		{
			if (zombieVariant == zombieVariantType[a])
			{
				selection = a;
				break;
			}
		}

		const char *title = table->GetString(StringID('CTRL', kControllerZombie, 'VRNT'));
		MenuSetting *menu = new MenuSetting('VRNT', selection, title, kZombieVariantCount);
		for (machine a = 0; a < kZombieVariantCount; a++)
		{
			menu->SetMenuItemString(a, table->GetString(StringID('CTRL', kControllerZombie, 'VRNT', zombieVariantType[a])));
		}

		return (menu);
	}

	if (index == 1)
	{
		int32 selection = 0;
		for (machine a = 0; a < kZombiePoseCount; a++)
		{
			if (zombiePose == zombiePoseType[a])
			{
				selection = a + 1;
				break;
			}
		}

		const char *title = table->GetString(StringID('CTRL', kControllerZombie, 'POSE'));
		MenuSetting *menu = new MenuSetting('POSE', selection, title, kZombiePoseCount + 1);
		menu->SetMenuItemString(0, table->GetString(StringID('CTRL', kControllerZombie, 'POSE', 'NONE')));
		for (machine a = 0; a < kZombiePoseCount; a++)
		{
			menu->SetMenuItemString(a + 1, table->GetString(StringID('CTRL', kControllerZombie, 'POSE', zombiePoseType[a])));
		}

		return (menu);
	}

	if (index == 2)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerZombie, 'RISE'));
		return (new BooleanSetting('RISE', ((zombieFlags & kZombieRiseFromGrave) != 0), title));
	}

	if (index == 3)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerZombie, 'DIRT'));
		return (new BooleanSetting('DIRT', ((zombieFlags & kZombieDirtInhibit) != 0), title));
	}

	if (index == 4)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerZombie, 'FAST'));
		return (new BooleanSetting('FAST', ((zombieFlags & kZombieFastChase) != 0), title));
	}

	if (index == 5)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerZombie, 'HAT '));
		return (new BooleanSetting('HAT ', ((zombieFlags & kZombieHardHat) != 0), title));
	}

	return (nullptr);
}

void ZombieConfiguration::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'VRNT')
	{
		int32 selection = static_cast<const MenuSetting *>(setting)->GetMenuSelection();
		zombieVariant = zombieVariantType[selection];
	}
	else if (identifier == 'POSE')
	{
		int32 selection = static_cast<const MenuSetting *>(setting)->GetMenuSelection();
		if (selection == 0)
		{
			zombiePose = kZombiePoseNone;
		}
		else
		{
			zombiePose = zombiePoseType[selection - 1];
		}
	}
	else if (identifier == 'RISE')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			zombieFlags |= kZombieRiseFromGrave;
		}
		else
		{
			zombieFlags &= ~kZombieRiseFromGrave;
		}
	}
	else if (identifier == 'DIRT')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			zombieFlags |= kZombieDirtInhibit;
		}
		else
		{
			zombieFlags &= ~kZombieDirtInhibit;
		}
	}
	else if (identifier == 'FAST')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			zombieFlags |= kZombieFastChase;
		}
		else
		{
			zombieFlags &= ~kZombieFastChase;
		}
	}
	else if (identifier == 'HAT ')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			zombieFlags |= kZombieHardHat;
		}
		else
		{
			zombieFlags &= ~kZombieHardHat;
		}
	}
}


inline void ZombieController::NewSoundTime(void)
{
	soundTime = Math::Random(5000) + 3000;
}

ZombieController::ZombieController() :
		MonsterController(kControllerZombie),
		frameAnimatorObserver(this, &ZombieController::HandleAnimationEvent)
{
	SetControllerFlags(kControllerAsleep);

	SetMonsterHealth(60 << 16);
	SetMonsterState(kZombieStateSleeping);
	SetMonsterMotion(kZombieMotionNone);

	SetSourcePosition(Point3D(0.0F, 0.0F, 1.5F));
	NewSoundTime();
}

ZombieController::ZombieController(const ZombieConfiguration& configuration) :
		MonsterController(kControllerZombie),
		ZombieConfiguration(configuration),
		frameAnimatorObserver(this, &ZombieController::HandleAnimationEvent)
{
	SetControllerFlags(kControllerAsleep);

	SetMonsterHealth(60 << 16);
	SetMonsterState(kZombieStateSleeping);
	SetMonsterMotion(kZombieMotionNone);

	SetSourcePosition(Point3D(0.0F, 0.0F, 1.5F));
	NewSoundTime();
}

ZombieController::ZombieController(const ZombieController& zombieController) :
		MonsterController(zombieController),
		ZombieConfiguration(zombieController),
		frameAnimatorObserver(this, &ZombieController::HandleAnimationEvent)
{
	SetControllerFlags(kControllerAsleep);

	NewSoundTime();
}

ZombieController::~ZombieController()
{
	UncountMonster(globalZombieCount);
}

Controller *ZombieController::Replicate(void) const
{
	return (new ZombieController(*this));
}

bool ZombieController::ValidNode(const Node *node)
{
	return ((node->GetNodeType() == kNodeModel) && (static_cast<const Model *>(node)->GetModelType() == kModelZombie));
}

void ZombieController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	MonsterController::Pack(data, packFlags);
	ZombieConfiguration::Pack(data);

	if (!(packFlags & kPackEditor))
	{
		data << ChunkHeader('SDTM', 4);
		data << soundTime;

		int32 state = GetMonsterState();
		if (state == kZombieStateStanding)
		{
			data << ChunkHeader('WTTM', 4);
			data << waitTime;
		}
	}

	data << TerminatorChunk;
}

void ZombieController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	MonsterController::Unpack(data, unpackFlags);
	UnpackChunkList<ZombieController>(data, unpackFlags);
}

bool ZombieController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'SDTM':

			data >> soundTime;
			return (true);

		case 'WTTM':

			data >> waitTime;
			return (true);
	}

	return (ZombieConfiguration::UnpackChunk(chunkHeader, data));
}

int32 ZombieController::GetSettingCount(void) const
{
	return (ZombieConfiguration::GetSettingCount());
}

Setting *ZombieController::GetSetting(int32 index) const
{
	return (ZombieConfiguration::GetSetting(index));
}

void ZombieController::SetSetting(const Setting *setting)
{
	ZombieConfiguration::SetSetting(setting);
}

void ZombieController::Preprocess(void)
{
	MonsterController::Preprocess();

	Model *model = GetTargetNode();

	if (zombieVariant != kZombieVariantNormal)
	{
		Node *node = model->GetFirstSubnode();
		while (node)
		{
			if (node->GetNodeType() == kNodeGeometry)
			{
				Geometry *geometry = static_cast<Geometry *>(node);
				geometry->SetMaterialObject(1, AutoReleaseMaterial((zombieVariant == kZombieVariantTechnician) ? kMaterialZombieTechnician : kMaterialZombieScientist));
				break;
			}

			node = node->Next();
		}
	}

	if (!model->GetManipulator())
	{
		unsigned_int32 monsterFlags = GetMonsterFlags();
		if (!(monsterFlags & kMonsterInitialized))
		{
			SetMonsterFlags(monsterFlags | kMonsterInitialized);

			if (Math::Random(2) == 0)
			{
				zombieFlags |= kZombieReachOut;
			}
		}

		if (Asleep())
		{
			SetInitialPose();
		}

		SetAttackable(true);

		if (zombieFlags & kZombieFastChase)
		{
			zombiePropelForce = kZombieRunForce;
			zombieTurnRate = kZombieFastTurnRate;
		}
		else
		{
			zombiePropelForce = kZombieWalkForce;
			zombieTurnRate = kZombieTurnRate;
		}

		if (zombieFlags & kZombieHardHat)
		{
			Node *headNode = model->FindNode(Text::StaticHash<'B', 'i', 'p', '0', '1', ' ', 'H', 'e', 'a', 'd'>::value);

			Model *hat = Model::Get(kModelZombieHardHat);
			hat->SetNodeFlags(kNodeNonpersistent | kNodeCloneInhibit | kNodeAnimateInhibit);
			headNode->AppendNewSubnode(hat);
		}

		AnimationBlender *animationBlender = GetAnimationBlender();
		animationBlender->SetFrameAnimatorObserver(&frameAnimatorObserver);

		int32 motion = GetMonsterMotion();
		Interpolator *interpolator = animationBlender->GetRecentAnimator()->GetFrameInterpolator();

		if ((motion == kZombieMotionRise) || (motion == kZombieMotionWake))
		{
			interpolator->SetCompletionProc(&WakeCallback, this);
		}
		else if (motion == kZombieMotionAttack)
		{
			interpolator->SetCompletionProc(&AttackCallback, this);
		}
		else if ((motion == kZombieMotionDamage) || (motion == kZombieMotionHeadDamage))
		{
			interpolator->SetCompletionProc(&DamageCallback, this);
		}
	}
	else if (model->GetFirstSubnode())
	{
		SetInitialPose();
	}
}

void ZombieController::SetInitialPose(void)
{
	Type pose = zombiePose;
	if (pose != kZombiePoseNone)
	{
		for (machine a = 0; a < kZombiePoseCount; a++)
		{
			if (pose == zombiePoseType[a])
			{
				GetAnimationBlender()->StartAnimation(zombiePoseAnimation[a], kInterpolatorStop);

				Model *model = GetTargetNode();
				model->Animate();
				model->StopMotion();
				break;
			}
		}
	}
}

void ZombieController::Wake(void)
{
	MonsterController::Wake();

	if (GetMonsterState() != kZombieStateDead)
	{
		if (GetMonsterMotion() == kZombieMotionNone)
		{
			unsigned_int32 flags = zombieFlags;
			if (flags & kZombieRiseFromGrave)
			{
				SetMonsterState(kZombieStateWaking);
				SetZombieMotion(kZombieMotionRise);

				Model *model = GetTargetNode();
				model->Animate();

				World *world = model->GetWorld();
				Transform4D transform = model->GetWorldTransform();

				if (!(flags & kZombieDirtInhibit))
				{
					OpenGraveEffect *effect = new OpenGraveEffect;
					effect->SetNodeTransform(transform);
					world->AddNewNode(effect);
				}

				OmniSource *source = new OmniSource("zombie/Rise", 48.0F);
				source->SetNodePosition(transform.GetTranslation());
				world->AddNewNode(source);
			}
			else if (zombiePose != kZombiePoseNone)
			{
				SetMonsterState(kZombieStateWaking);
				SetZombieMotion(kZombieMotionWake);
			}
			else
			{
				GetAnimationBlender()->StartAnimation("zombie/Stand", kInterpolatorStop);
				SetMonsterState(kZombieStateChasing);
				SetZombieMotion(kZombieMotionChase);
			}
		}
	}

	CountMonster(globalZombieCount);
}

void ZombieController::Move(void)
{
	if ((this->*zombieStateHandler[GetMonsterState()])())
	{
		MonsterController::Move();
	}
}

bool ZombieController::HandleSleepingState(void)
{
	return (true);
}

bool ZombieController::HandleStandingState(void)
{
	if (GetStateTime() >= waitTime)
	{
		Vector3D	direction;

		if (GetEnemyDirection(&direction))
		{
			if (Magnitude(direction) > kZombieAttackDistance)
			{
				SetMonsterState(kZombieStateChasing);
				SetZombieMotion(kZombieMotionChase);
			}
			else
			{
				SetMonsterState(kZombieStateAttacking);
				SetZombieMotion(kZombieMotionAttack);
			}
		}
	}

	SetExternalForce(GetRepulsionForce());
	SetExternalLinearResistance(Vector2D(kZombieResistForce, kZombieResistForce));
	return (true);
}

bool ZombieController::HandleWakingState(void)
{
	SetExternalLinearResistance(Vector2D(kZombieResistForce, kZombieResistForce));
	return (true);
}

bool ZombieController::HandleChasingState(void)
{
	Vector3D	direction;

	if ((soundTime -= TheTimeMgr->GetDeltaTime()) <= 0)
	{
		static const char soundName[2][16] =
		{
			"zombie/Zombie1", "zombie/Zombie2"
		};

		NewSoundTime();

		OmniSource *source = PlaySource(soundName[Math::Random(2)], 32.0F);
		source->SetSourceFrequency(Math::RandomFloat(0.2F) + 0.8F);
	}

	Vector2D force = GetRepulsionForce();

	if (GetEnemyDirection(&direction))
	{
		float targetDistance = Magnitude(direction);
		float azm = SetMonsterAzimuth(Atan(direction.y, direction.x), zombieTurnRate);

		if ((targetDistance > kZombieAttackDistance) || (GetStateTime() < 250))
		{
			force += CosSin(azm) * zombiePropelForce;
		}
		else
		{
			SetMonsterState(kZombieStateAttacking);
			SetZombieMotion(kZombieMotionAttack);
		}
	}

	if (GetCharacterState() & kCharacterGround)
	{
		SetExternalLinearResistance(Vector2D(kZombieResistForce, kZombieResistForce));
	}
	else
	{
		force *= 0.02F;
		SetExternalLinearResistance(Zero2D);
	}

	SetExternalForce(force);
	return (true);
}

bool ZombieController::HandleAttackingState(void)
{
	Vector3D	direction;

	if (GetEnemyDirection(&direction))
	{
		SetMonsterAzimuth(Atan(direction.y, direction.x), zombieTurnRate);
	}

	SetExternalForce(GetRepulsionForce());
	SetExternalLinearResistance(Vector2D(kZombieResistForce, kZombieResistForce));
	return (true);
}

bool ZombieController::HandleRecoveringState(void)
{
	SetExternalForce(GetRepulsionForce());
	SetExternalLinearResistance(Vector2D(kZombieResistForce, kZombieResistForce));
	return (true);
}

bool ZombieController::HandleDeadState(void)
{
	if (GetStateTime() >= 10000)
	{
		delete GetTargetNode();
		return (false);
	}

	SetExternalForce(Zero3D);
	SetExternalLinearResistance(Vector2D(kZombieResistForce, kZombieResistForce));
	return (true);
}

void ZombieController::HandleAnimationEvent(FrameAnimator *animator, CueType cueType)
{
	switch (cueType)
	{
		case 'DAMG':

			DamageEnemy(25 << 16, kZombieAttackDistance);
			break;
	}
}

void ZombieController::SetZombieMotion(int32 motion)
{
	SetMonsterMotion(motion);
	switch (motion)
	{
		case kZombieMotionStand:

			GetAnimationBlender()->BlendAnimation("zombie/Stand", kInterpolatorForward | kInterpolatorLoop, 0.004F);
			break;

		case kZombieMotionRise:

			GetAnimationBlender()->StartAnimation("zombie/Rise", kInterpolatorForward, &WakeCallback, this);
			PlaySource("zombie/Zombie2", 48.0F);
			break;

		case kZombieMotionWake:
		{
			Type pose = zombiePose;
			if (pose == kZombiePoseWakeup)
			{
				Interpolator *interpolator = GetAnimationBlender()->GetRecentAnimator()->GetFrameInterpolator();
				interpolator->SetMode(kInterpolatorForward);
				interpolator->SetCompletionProc(&WakeCallback, this);
			}
			else
			{
				GetAnimationBlender()->BlendAnimation("zombie/Wakeup", kInterpolatorForward, 0.002F, &WakeCallback, this);
			}

			PlaySource("zombie/Zombie2", 48.0F);
			break;
		}

		case kZombieMotionChase:
		{
			unsigned_int32 flags = zombieFlags;
			if (flags & kZombieFastChase)
			{
				FrameAnimator *animator = GetAnimationBlender()->BlendAnimation("zombie/Run", kInterpolatorForward | kInterpolatorLoop, 0.004F);
				animator->GetFrameInterpolator()->SetRate(1.5F);
			}
			else
			{
				GetAnimationBlender()->BlendAnimation((flags & kZombieReachOut) ? "zombie/Chase" : "zombie/Walk", kInterpolatorForward | kInterpolatorLoop, 0.004F);
			}

			break;
		}

		case kZombieMotionAttack:
		{
			static const char attackName[2][16] =
			{
				"zombie/Attack1", "zombie/Attack2"
			};

			GetAnimationBlender()->BlendAnimation(attackName[Math::Random(2)], kInterpolatorForward, 0.004F, &AttackCallback, this);

			OmniSource *source = PlaySource(attackName[Math::Random(2)], 32.0F);
			source->SetSourceFrequency(Math::RandomFloat(0.2F) + 0.8F);
			break;
		}

		case kZombieMotionDamage:
		{
			static const char damageName[2][16] =
			{
				"zombie/Damage1", "zombie/Damage2"
			};

			FrameAnimator *animator = GetAnimationBlender()->BlendAnimation(damageName[Math::Random(2)], kInterpolatorForward, 0.004F, &DamageCallback, this);
			animator->GetFrameInterpolator()->SetRate(2.0F);
			break;
		}

		case kZombieMotionHeadDamage:

			GetAnimationBlender()->BlendAnimation("zombie/Damage3", kInterpolatorForward, 0.004F, &DamageCallback, this);
			break;

		case kZombieMotionDeath:

			GetAnimationBlender()->BlendAnimation("zombie/Death", kInterpolatorForward, 0.004F);
			PlaySource("zombie/Death", 64.0F);
			break;
	}
}

void ZombieController::WakeCallback(Interpolator *interpolator, void *cookie)
{
	ZombieController *zombieController = static_cast<ZombieController *>(cookie);
	zombieController->SetMonsterState(kZombieStateChasing);
	zombieController->SetZombieMotion(kZombieMotionChase);
}

void ZombieController::AttackCallback(Interpolator *interpolator, void *cookie)
{
	ZombieController *zombieController = static_cast<ZombieController *>(cookie);
	zombieController->SetMonsterState(kZombieStateStanding);
	zombieController->SetZombieMotion(kZombieMotionStand);
	zombieController->waitTime = Math::Random(300) + 200;
}

void ZombieController::DamageCallback(Interpolator *interpolator, void *cookie)
{
	ZombieController *zombieController = static_cast<ZombieController *>(cookie);
	zombieController->SetMonsterState(kZombieStateStanding);
	zombieController->SetZombieMotion(kZombieMotionStand);
	zombieController->waitTime = Math::Random(200);
}

CharacterStatus ZombieController::Damage(Fixed damage, unsigned_int32 flags, GameCharacterController *attacker, const Point3D *position, const Vector3D *force)
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
			BloodParticleSystem *blood = new BloodParticleSystem(ColorRGB(0.125F, 0.125F, 0.125F), particleCount);
			blood->SetNodePosition(*position);
			model->GetWorld()->AddNewNode(blood);
		}
	}

	if (DamageTimeExpired(200))
	{
		OmniSource *source = PlaySource("zombie/Damage", 32.0F);
		source->SetSourceFrequency(Math::RandomFloat(0.2F) + 0.8F);
	}

	unsigned_int32 state = GetMonsterState();
	if (state == kZombieStateWaking)
	{
		damage >>= 1;
	}

	CharacterStatus status = MonsterController::Damage(damage, flags, attacker, position, force);
	if ((status == kCharacterDamaged) && (state == kZombieStateChasing))
	{
		if (Math::Random(2) == 0)
		{
			SetMonsterState(kZombieStateRecovering);
			SetZombieMotion(((model->GetInverseWorldTransform().GetRow(2) ^ *position) < 1.4F) ? kZombieMotionDamage : kZombieMotionHeadDamage);
		}
	}

	return (status);
}

void ZombieController::Kill(GameCharacterController *attacker, const Point3D *position, const Vector3D *force)
{
	MonsterController::Kill(attacker, position, force);

	UncountMonster(globalZombieCount);
	SetMonsterState(kZombieStateDead);

	SetZombieMotion(kZombieMotionDeath);
	SetExternalForce(Zero3D);
}


GenerateZombieMethod::GenerateZombieMethod() : GenerateMonsterMethod(kMethodGenerateZombie)
{
}

GenerateZombieMethod::GenerateZombieMethod(const GenerateZombieMethod& generateZombieMethod) :
		GenerateMonsterMethod(generateZombieMethod),
		ZombieConfiguration(generateZombieMethod)
{
}

GenerateZombieMethod::~GenerateZombieMethod()
{
}

Method *GenerateZombieMethod::Replicate(void) const
{
	return (new GenerateZombieMethod(*this));
}

void GenerateZombieMethod::Pack(Packer& data, unsigned_int32 packFlags) const
{
	GenerateMonsterMethod::Pack(data, packFlags);
	ZombieConfiguration::Pack(data);
	data << TerminatorChunk;
}

void GenerateZombieMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	GenerateMonsterMethod::Unpack(data, unpackFlags);
	UnpackChunkList<GenerateZombieMethod>(data, unpackFlags);
}

bool GenerateZombieMethod::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	return (ZombieConfiguration::UnpackChunk(chunkHeader, data));
}

int32 GenerateZombieMethod::GetSettingCount(void) const
{
	return (ZombieConfiguration::GetSettingCount());
}

Setting *GenerateZombieMethod::GetSetting(int32 index) const
{
	return (ZombieConfiguration::GetSetting(index));
}

void GenerateZombieMethod::SetSetting(const Setting *setting)
{
	ZombieConfiguration::SetSetting(setting);
}

void GenerateZombieMethod::Execute(const ScriptState *state)
{
	Node *node = GetTargetNode(state);
	if (node)
	{
		GamePlayer *player = static_cast<GamePlayer *>(TheMessageMgr->GetLocalPlayer());
		if (player->GetPlayerController())
		{
			Model *model = Model::Get(kModelZombie);
			ZombieController *controller = new ZombieController(*this);
			GenerateMonster(node, model, controller);
		}
	}

	CallCompletionProc();
}


GetZombieCountMethod::GetZombieCountMethod() : Method(kMethodGetZombieCount)
{
}

GetZombieCountMethod::GetZombieCountMethod(const GetZombieCountMethod& getZombieCountMethod) : Method(getZombieCountMethod)
{
}

GetZombieCountMethod::~GetZombieCountMethod()
{
}

Method *GetZombieCountMethod::Replicate(void) const
{
	return (new GetZombieCountMethod(*this));
}

void GetZombieCountMethod::Execute(const ScriptState *state)
{
	SetOutputValue(state, ZombieController::GetGlobalZombieCount());
	CallCompletionProc();
}


OpenGraveEffect::OpenGraveEffect() :
		Effect(kEffectOpenGrave, kRenderTriangleStrip, kRenderDepthTest | kRenderDepthOffset),
		dynamicVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		textureAttribute("zombie/Dirt")
{
	Initialize();
}

OpenGraveEffect::OpenGraveEffect(const OpenGraveEffect& openGraveEffect) :
		Effect(openGraveEffect),
		dynamicVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		textureAttribute("zombie/Dirt")
{
	Initialize();
}

OpenGraveEffect::~OpenGraveEffect()
{
	staticVertexBuffer.Release();
}

void OpenGraveEffect::Initialize(void)
{
	lifeTime = 60000;

	effectFrame = 0.0F;
	effectAngle = -1;

	if (staticVertexBuffer.Retain() == 1)
	{
		static const GraveVertex graveVertex[4] =
		{
			{Point2D(-1.5F, 1.5F), Vector3D(0.0F, 0.0F, 1.0F), Vector3D(0.0F, 1.0F, 0.0F)},
			{Point2D(-1.5F, -1.5F), Vector3D(0.0F, 0.0F, 1.0F), Vector3D(0.0F, 1.0F, 0.0F)},
			{Point2D(1.5F, 1.5F), Vector3D(0.0F, 0.0F, 1.0F), Vector3D(0.0F, 1.0F, 0.0F)},
			{Point2D(1.5F, -1.5F), Vector3D(0.0F, 0.0F, 1.0F), Vector3D(0.0F, 1.0F, 0.0F)}
		};

		staticVertexBuffer.Establish(sizeof(GraveVertex) * 4, graveVertex);
	}
}

Node *OpenGraveEffect::Replicate(void) const
{
	return (new OpenGraveEffect(*this));
}

void OpenGraveEffect::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Effect::Pack(data, packFlags);

	data << ChunkHeader('LIFE', 4);
	data << lifeTime;

	data << ChunkHeader('FRAM', 4);
	data << effectFrame;

	data << ChunkHeader('ORNT', 4);
	data << effectAngle;

	data << TerminatorChunk;
}

void OpenGraveEffect::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Effect::Unpack(data, unpackFlags);
	UnpackChunkList<OpenGraveEffect>(data, unpackFlags);
}

bool OpenGraveEffect::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'LIFE':

			data >> lifeTime;
			return (true);

		case 'FRAM':

			data >> effectFrame;
			return (true);

		case 'ORNT':

			data >> effectAngle;
			return (true);
	}

	return (false);
}

bool OpenGraveEffect::CalculateBoundingBox(Box3D *box) const
{
	box->min.Set(-1.5F, -1.5F, -1.5F);
	box->max.Set(1.5F, 1.5F, 1.5F);
	return (true);
}

bool OpenGraveEffect::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	sphere->SetCenter(0.0F, 0.0F, 0.0F);
	sphere->SetRadius(1.5F);
	return (true);
}

void OpenGraveEffect::Preprocess(void)
{
	Effect::Preprocess();

	Wake();
	SetTransformable(this);

	SetVertexCount(4);
	SetVertexBuffer(kVertexBufferAttributeArray0, &staticVertexBuffer, sizeof(GraveVertex));
	SetVertexBuffer(kVertexBufferAttributeArray1, &dynamicVertexBuffer, sizeof(Point2D));
	SetVertexAttributeArray(kArrayPosition, 0, 2);
	SetVertexAttributeArray(kArrayNormal, sizeof(Point2D), 3);
	SetVertexAttributeArray(kArrayTangent, sizeof(Point2D) + sizeof(Point3D), 3);

	SetVertexAttributeArray(kArrayTexcoord, 0, 2);
	SetVertexBufferArrayFlags(1 << kArrayTexcoord);
	dynamicVertexBuffer.Establish(sizeof(Point2D) * 4);

	attributeList.Append(&textureAttribute);

	RenderSegment *segment = GetFirstRenderSegment();
	segment->SetMaterialAttributeList(&attributeList);
	segment->SetMaterialState(kMaterialAlphaTest);

	SetDepthOffset(0.03125F, &GetBoundingSphere()->GetCenter());
	SetEffectListIndex(kEffectListLight);

	UpdateTexcoords(effectFrame);
}

void OpenGraveEffect::UpdateTexcoords(float frame)
{
	effectFrame = frame;

	volatile Point2D *restrict texcoord = dynamicVertexBuffer.BeginUpdateSync<Point2D>();

	frame = PositiveFloor(frame);
	float v = PositiveFloor(frame * 0.125F);
	float u = frame - v * 8.0F;

	u *= 0.125F;
	v = 1.0F - v * 0.125F;

	texcoord[0].Set(u + 0.125F, v);
	texcoord[1].Set(u, v);
	texcoord[2].Set(u + 0.125F, v - 0.125F);
	texcoord[3].Set(u, v - 0.125F);

	dynamicVertexBuffer.EndUpdateSync();
}

void OpenGraveEffect::Move(void)
{
	if ((lifeTime -= TheTimeMgr->GetDeltaTime()) > 0)
	{
		float frame = Fmin(effectFrame + TheTimeMgr->GetFloatDeltaTime() * 0.02F, 63.0F);
		if (frame != effectFrame)
		{
			UpdateTexcoords(frame);
		}
	}
	else
	{
		delete this;
	}
}

// ZYUQURM
