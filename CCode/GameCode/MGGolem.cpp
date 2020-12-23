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


#include "MGGolem.h"
#include "MGGame.h"


using namespace C4;


namespace
{
	const float kGolemTurnRate = 0.001F;

	Storage<Golem> golemStorage;

	const Type golemVariantType[kGolemVariantCount] =
	{
		kGolemVariantTar, kGolemVariantMud, kGolemVariantSwamp, kGolemVariantToxic, kGolemVariantLava
	};

	const MaterialType golemMaterialType[kGolemVariantCount] =
	{
		kMaterialGolemTar, kMaterialGolemMud, kMaterialGolemSwamp, kMaterialGolemToxic, kMaterialGolemLava
	};
}


int32 GolemController::globalGolemCount = 0;

bool (GolemController::*GolemController::golemStateHandler[kGolemStateCount])(void) =
{
	&GolemController::HandleSleepingState,
	&GolemController::HandleStandingState,
	&GolemController::HandleRisingState,
	&GolemController::HandleThrowingState,
	&GolemController::HandleRecoveringState,
	&GolemController::HandleDeadState
};


Golem::Golem() :
		golemControllerRegistration(kControllerGolem, TheGame->GetStringTable()->GetString(StringID('CTRL', kControllerGolem))),
		golemModelRegistration(kModelGolem, TheGame->GetStringTable()->GetString(StringID('MODL', 'MNST', kModelGolem)), "golem/Golem", kModelPrecache, kControllerGolem),
		tarballParticleSystemRegistration(kParticleSystemTarball, TheGame->GetStringTable()->GetString(StringID('PART', kParticleSystemTarball))),
		tarballModelReg(kModelTarball, nullptr, "golem/Tarball", kModelPrecache | kModelPrivate),
		lavaballParticleSystemRegistration(kParticleSystemLavaball, TheGame->GetStringTable()->GetString(StringID('PART', kParticleSystemLavaball))),
		lavaballModelReg(kModelLavaball, nullptr, "golem/Lavaball", kModelPrecache | kModelPrivate),

		tarMaterialRegistration(kMaterialGolemTar, "golem/Tar"),
		mudMaterialRegistration(kMaterialGolemMud, "golem/Mud"),
		swampMaterialRegistration(kMaterialGolemSwamp, "golem/Swamp"),
		toxicMaterialRegistration(kMaterialGolemToxic, "golem/Toxic"),
		lavaMaterialRegistration(kMaterialGolemLava, "golem/Lava"),

		generateGolemRegistration(kMethodGenerateGolem, TheGame->GetStringTable()->GetString(StringID('MTHD', kMethodGenerateGolem))),
		getGolemCountRegistration(kMethodGetGolemCount, TheGame->GetStringTable()->GetString(StringID('MTHD', kMethodGetGolemCount)), kMethodNoTarget | kMethodOutputValue)
{
}

Golem::~Golem()
{
}

void Golem::Construct(void)
{
	new(golemStorage) Golem;
}

void Golem::Destruct(void)
{
	golemStorage->~Golem();
}


inline GolemConfiguration::GolemConfiguration()
{
	golemFlags = 0;
	golemVariant = kGolemVariantTar;
}

inline GolemConfiguration::GolemConfiguration(const GolemConfiguration& golemConfiguration)
{
	golemFlags = golemConfiguration.golemFlags;
	golemVariant = golemConfiguration.golemVariant;
}

void GolemConfiguration::Pack(Packer& data) const
{
	data << ChunkHeader('FLAG', 4);
	data << golemFlags;

	data << ChunkHeader('VRNT', 4);
	data << golemVariant;
}

bool GolemConfiguration::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> golemFlags;
			return (true); 

		case 'VRNT':
 
			data >> golemVariant;
			return (true); 
	}

	return (false); 
}
 
int32 GolemConfiguration::GetSettingCount(void) const 
{
	return (1);
}
 
Setting *GolemConfiguration::GetSetting(int32 index) const
{
	const StringTable *table = TheGame->GetStringTable();

	if (index == 0)
	{
		int32 selection = 0;
		for (machine a = 1; a < kGolemVariantCount; a++)
		{
			if (golemVariant == golemVariantType[a])
			{
				selection = a;
				break;
			}
		}

		const char *title = table->GetString(StringID('CTRL', kControllerGolem, 'VRNT'));
		MenuSetting *menu = new MenuSetting('VRNT', selection, title, kGolemVariantCount);
		for (machine a = 0; a < kGolemVariantCount; a++)
		{
			menu->SetMenuItemString(a, table->GetString(StringID('CTRL', kControllerGolem, 'VRNT', golemVariantType[a])));
		}

		return (menu);
	}

	return (nullptr);
}

void GolemConfiguration::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'VRNT')
	{
		int32 selection = static_cast<const MenuSetting *>(setting)->GetMenuSelection();
		golemVariant = golemVariantType[selection];
	}
}


inline void GolemController::NewSoundTime(void)
{
	soundTime = Math::Random(5000) + 5000;
}

inline void GolemController::NewThrowTime(void)
{
	throwTime = Math::Random(2000) + 1000;
}

GolemController::GolemController() :
		MonsterController(kControllerGolem),
		frameAnimatorObserver(this, &GolemController::HandleAnimationEvent)
{
	SetControllerFlags(kControllerAsleep);

	SetMonsterHealth(400 << 16);
	SetMonsterState(kGolemStateSleeping);
	SetMonsterMotion(kGolemMotionNone);

	SetSourcePosition(Point3D(0.0F, 0.0F, 1.5F));
	NewSoundTime();
	NewThrowTime();
}

GolemController::GolemController(const GolemConfiguration& configuration) :
		MonsterController(kControllerGolem),
		GolemConfiguration(configuration),
		frameAnimatorObserver(this, &GolemController::HandleAnimationEvent)
{
	SetControllerFlags(kControllerAsleep);

	SetMonsterHealth(400 << 16);
	SetMonsterState(kGolemStateSleeping);
	SetMonsterMotion(kGolemMotionNone);

	SetSourcePosition(Point3D(0.0F, 0.0F, 1.5F));
	NewSoundTime();
	NewThrowTime();
}

GolemController::GolemController(const GolemController& golemController) :
		MonsterController(golemController),
		GolemConfiguration(golemController),
		frameAnimatorObserver(this, &GolemController::HandleAnimationEvent)
{
	SetControllerFlags(kControllerAsleep);

	NewSoundTime();
	NewThrowTime();
}

GolemController::~GolemController()
{
	UncountMonster(globalGolemCount);
}

Controller *GolemController::Replicate(void) const
{
	return (new GolemController(*this));
}

bool GolemController::ValidNode(const Node *node)
{
	return ((node->GetNodeType() == kNodeModel) && (static_cast<const Model *>(node)->GetModelType() == kModelGolem));
}

void GolemController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	MonsterController::Pack(data, packFlags);
	GolemConfiguration::Pack(data);

	if (!(packFlags & kPackEditor))
	{
		data << ChunkHeader('SDTM', 4);
		data << soundTime;

		data << ChunkHeader('THTM', 4);
		data << throwTime;
	}

	data << TerminatorChunk;
}

void GolemController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	MonsterController::Unpack(data, unpackFlags);
	UnpackChunkList<GolemController>(data, unpackFlags);
}

bool GolemController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'SDTM':

			data >> soundTime;
			return (true);

		case 'THTM':

			data >> throwTime;
			return (true);
	}

	return (GolemConfiguration::UnpackChunk(chunkHeader, data));
}

int32 GolemController::GetSettingCount(void) const
{
	return (GolemConfiguration::GetSettingCount());
}

Setting *GolemController::GetSetting(int32 index) const
{
	return (GolemConfiguration::GetSetting(index));
}

void GolemController::SetSetting(const Setting *setting)
{
	GolemConfiguration::SetSetting(setting);
}

void GolemController::Preprocess(void)
{
	MonsterController::Preprocess();

	SetRigidBodyFlags(GetRigidBodyFlags() | kRigidBodyImmovable);

	Model *model = GetTargetNode();
	if (!model->GetManipulator())
	{
		ballNode[0] = model->FindNode(Text::StaticHash<'R', 'i', 'g', 'h', 't', 'T', 'a', 'r'>::value);
		ballNode[1] = model->FindNode(Text::StaticHash<'L', 'e', 'f', 't', 'T', 'a', 'r'>::value);
	}

	for (machine a = 0; a < kGolemVariantCount; a++)
	{
		if (golemVariant == golemVariantType[a])
		{
			AutoReleaseMaterial material(golemMaterialType[a]);

			Node *node = model->GetFirstSubnode();
			while (node)
			{
				if (node->GetNodeType() == kNodeGeometry)
				{
					Geometry *geometry = static_cast<Geometry *>(node);
					geometry->SetMaterialObject(0, material);
					break;
				}

				node = node->Next();
			}

			if (!model->GetManipulator())
			{
				static_cast<Geometry *>(ballNode[0])->SetMaterialObject(0, material);
				static_cast<Geometry *>(ballNode[1])->SetMaterialObject(0, material);
			}

			break;
		}
	}

	if (!model->GetManipulator())
	{
		AnimationBlender *animationBlender = GetAnimationBlender();
		animationBlender->SetFrameAnimatorObserver(&frameAnimatorObserver);

		int32 motion = GetMonsterMotion();
		Interpolator *interpolator = animationBlender->GetRecentAnimator()->GetFrameInterpolator();

		if (motion == kGolemMotionRise)
		{
			interpolator->SetCompletionProc(&RiseCallback, this);
		}
		else if ((motion == kGolemMotionThrow1) || (motion == kGolemMotionThrow2))
		{
			interpolator->SetCompletionProc(&ThrowCallback, this);
		}
		else if (motion == kGolemMotionDamage)
		{
			interpolator->SetCompletionProc(&DamageCallback, this);
		}
	}
}

void GolemController::Wake(void)
{
	MonsterController::Wake();

	if (GetMonsterMotion() == kGolemMotionNone)
	{
		SetMonsterState(kGolemStateRising);
		SetGolemMotion(kGolemMotionRise);
	}

	CountMonster(globalGolemCount);
}

void GolemController::Move(void)
{
	if ((this->*golemStateHandler[GetMonsterState()])())
	{
		MonsterController::Move();
	}
}

bool GolemController::HandleSleepingState(void)
{
	return (true);
}

bool GolemController::HandleStandingState(void)
{
	Vector3D	direction;

	if (GetEnemyDirection(&direction))
	{
		SetMonsterAzimuth(Atan(direction.y, direction.x), kGolemTurnRate);

		int32 dt = TheTimeMgr->GetDeltaTime();
		if ((soundTime -= dt) <= 0)
		{
			static const char soundName[2][16] =
			{
				"golem/Roar1", "golem/Roar2"
			};

			NewSoundTime();

			OmniSource *source = PlaySource(soundName[Math::Random(2)], 128.0F);
			source->SetSourceFrequency(Math::RandomFloat(0.2F) + 0.8F);
		}

		if (((throwTime = MaxZero(throwTime - dt)) <= 0) && (SquaredMag(direction) < 1.0e4F))
		{
			CollisionData	data;

			NewThrowTime();

			Model *model = GetTargetNode();
			Point3D position = model->GetWorldPosition();
			position.z += 1.5F;

			if (!model->GetWorld()->DetectCollision(position, position + direction, 0.0F, kCollisionSightPath, &data))
			{
				SetMonsterState(kGolemStateThrowing);
				SetGolemMotion(kGolemMotionThrow1 + Math::Random(2));
			}
		}
	}

	return (true);
}

bool GolemController::HandleRisingState(void)
{
	Vector3D	direction;

	if (GetEnemyDirection(&direction))
	{
		SetMonsterAzimuth(Atan(direction.y, direction.x), kGolemTurnRate);
	}

	return (true);
}

bool GolemController::HandleThrowingState(void)
{
	Vector3D	direction;

	if (GetEnemyDirection(&direction))
	{
		SetMonsterAzimuth(Atan(direction.y, direction.x), kGolemTurnRate);
	}

	return (true);
}

bool GolemController::HandleRecoveringState(void)
{
	Vector3D	direction;

	if (GetEnemyDirection(&direction))
	{
		SetMonsterAzimuth(Atan(direction.y, direction.x), kGolemTurnRate);
	}

	return (true);
}

bool GolemController::HandleDeadState(void)
{
	if (GetStateTime() >= 6000)
	{
		delete GetTargetNode();
		return (false);
	}

	return (true);
}

void GolemController::HandleAnimationEvent(FrameAnimator *animator, CueType cueType)
{
	switch (cueType)
	{
		case 'SHW1':
		case 'SHW2':
		{
			static const char soundName[2][16] =
			{
				"golem/Throw1", "golem/Throw2"
			};

			ballNode[cueType - 'SHW1']->Enable();
			OmniSource *source = PlaySource(soundName[Math::Random(2)], 128.0F);
			source->SetSourceFrequency(Math::RandomFloat(0.25F) + 0.75F);
			break;
		}

		case 'THR1':
		case 'THR2':
		{
			int32 hand = cueType - 'THR1';
			ballNode[hand]->Disable();

			if (TheMessageMgr->Server())
			{
				Vector3D	direction;

				if (GetEnemyDirection(&direction))
				{
					float d = Magnitude(direction) - 5.0F;
					Vector3D ballDirection(16.0F, 0.0F, d * -0.03125F * GetPhysicsController()->GetGravityAcceleration().z);
					ballDirection.RotateAboutZ(GetMonsterAzimuth() + Atan(0.33F / d) + Math::RandomFloat(0.1F) - 0.05F);

					int32 projectileIndex = GetTargetNode()->GetWorld()->NewControllerIndex();

					if (golemVariant == kGolemVariantLava)
					{
						TheMessageMgr->SendMessageAll(CreateLavaballMessage(projectileIndex, GetControllerIndex(), ballNode[hand]->GetWorldPosition(), ballDirection));
					}
					else
					{
						TheMessageMgr->SendMessageAll(CreateTarballMessage(projectileIndex, GetControllerIndex(), ballNode[hand]->GetWorldPosition(), ballDirection));
					}
				}
				else
				{
					SetMonsterState(kGolemStateStanding);
					SetGolemMotion(kGolemMotionStand);
				}
			}

			break;
		}
	}
}

void GolemController::SetGolemMotion(int32 motion)
{
	SetMonsterMotion(motion);
	switch (motion)
	{
		case kGolemMotionStand:

			GetAnimationBlender()->BlendAnimation("golem/Stand", kInterpolatorForward | kInterpolatorLoop, 0.004F);
			break;

		case kGolemMotionRise:

			GetAnimationBlender()->StartAnimation("golem/Rise", kInterpolatorForward, &RiseCallback, this);
			PlaySource("golem/Rise", 128.0F);
			break;

		case kGolemMotionThrow1:

			GetAnimationBlender()->StartAnimation("golem/Throw1", kInterpolatorForward, &ThrowCallback, this);
			break;

		case kGolemMotionThrow2:

			GetAnimationBlender()->StartAnimation("golem/Throw2", kInterpolatorForward, &ThrowCallback, this);
			break;

		case kGolemMotionDamage:

			GetAnimationBlender()->BlendAnimation("golem/Damage", kInterpolatorForward, 0.004F, &DamageCallback, this);
			break;

		case kGolemMotionDeath:

			GetAnimationBlender()->BlendAnimation("golem/Rise", kInterpolatorBackward, 0.004F);
			PlaySource("golem/Death", 128.0F);
			break;
	}
}

void GolemController::RiseCallback(Interpolator *interpolator, void *cookie)
{
	GolemController *golemController = static_cast<GolemController *>(cookie);
	golemController->SetMonsterState(kGolemStateStanding);
	golemController->SetGolemMotion(kGolemMotionStand);
}

void GolemController::ThrowCallback(Interpolator *interpolator, void *cookie)
{
	GolemController *golemController = static_cast<GolemController *>(cookie);
	golemController->SetMonsterState(kGolemStateStanding);
	golemController->SetGolemMotion(kGolemMotionStand);
}

void GolemController::DamageCallback(Interpolator *interpolator, void *cookie)
{
	GolemController *golemController = static_cast<GolemController *>(cookie);
	golemController->SetMonsterState(kGolemStateStanding);
	golemController->SetGolemMotion(kGolemMotionStand);
}

CharacterStatus GolemController::Damage(Fixed damage, unsigned_int32 flags, GameCharacterController *attacker, const Point3D *position, const Vector3D *force)
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
			ParticleSystem		*explosion;

			if (golemVariant == kGolemVariantLava)
			{
				explosion = new LavaballExplosionParticleSystem(Zero3D, Zero3D);
			}
			else
			{
				explosion = new TarballExplosionParticleSystem(Zero3D, Zero3D);
			}

			explosion->SetNodePosition(*position);
			model->GetWorld()->AddNewNode(explosion);
		}
	}

	if (DamageTimeExpired(200))
	{
		OmniSource *source = PlaySource("golem/Damage", 128.0F);
		source->SetSourceFrequency(Math::RandomFloat(0.3F) + 0.7F);
	}

	CharacterStatus status = MonsterController::Damage(damage, flags, attacker, position, force);
	if ((status == kCharacterDamaged) && (GetMonsterState() == kGolemStateStanding))
	{
		if (Math::Random(2) == 0)
		{
			SetMonsterState(kGolemStateRecovering);
			SetGolemMotion(kGolemMotionDamage);
		}
	}

	return (status);
}

void GolemController::Kill(GameCharacterController *attacker, const Point3D *position, const Vector3D *force)
{
	MonsterController::Kill(attacker, position, force);

	UncountMonster(globalGolemCount);
	SetMonsterState(kGolemStateDead);

	SetGolemMotion(kGolemMotionDeath);
	SetExternalForce(Zero3D);
}


GenerateGolemMethod::GenerateGolemMethod() : GenerateMonsterMethod(kMethodGenerateGolem)
{
}

GenerateGolemMethod::GenerateGolemMethod(const GenerateGolemMethod& generateGolemMethod) :
		GenerateMonsterMethod(generateGolemMethod),
		GolemConfiguration(generateGolemMethod)
{
}

GenerateGolemMethod::~GenerateGolemMethod()
{
}

Method *GenerateGolemMethod::Replicate(void) const
{
	return (new GenerateGolemMethod(*this));
}

void GenerateGolemMethod::Pack(Packer& data, unsigned_int32 packFlags) const
{
	GenerateMonsterMethod::Pack(data, packFlags);
	GolemConfiguration::Pack(data);
	data << TerminatorChunk;
}

void GenerateGolemMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	GenerateMonsterMethod::Unpack(data, unpackFlags);
	UnpackChunkList<GenerateGolemMethod>(data, unpackFlags);
}

bool GenerateGolemMethod::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	return (GolemConfiguration::UnpackChunk(chunkHeader, data));
}

int32 GenerateGolemMethod::GetSettingCount(void) const
{
	return (GolemConfiguration::GetSettingCount());
}

Setting *GenerateGolemMethod::GetSetting(int32 index) const
{
	return (GolemConfiguration::GetSetting(index));
}

void GenerateGolemMethod::SetSetting(const Setting *setting)
{
	GolemConfiguration::SetSetting(setting);
}

void GenerateGolemMethod::Execute(const ScriptState *state)
{
	Node *node = GetTargetNode(state);
	if (node)
	{
		GamePlayer *player = static_cast<GamePlayer *>(TheMessageMgr->GetLocalPlayer());
		if (player->GetPlayerController())
		{
			Model *model = Model::Get(kModelGolem);
			GolemController *controller = new GolemController(*this);
			GenerateMonster(node, model, controller);
		}
	}

	CallCompletionProc();
}


GetGolemCountMethod::GetGolemCountMethod() : Method(kMethodGetGolemCount)
{
}

GetGolemCountMethod::GetGolemCountMethod(const GetGolemCountMethod& getGolemCountMethod) : Method(getGolemCountMethod)
{
}

GetGolemCountMethod::~GetGolemCountMethod()
{
}

Method *GetGolemCountMethod::Replicate(void) const
{
	return (new GetGolemCountMethod(*this));
}

void GetGolemCountMethod::Execute(const ScriptState *state)
{
	SetOutputValue(state, GolemController::GetGlobalGolemCount());
	CallCompletionProc();
}

// ZYUQURM
