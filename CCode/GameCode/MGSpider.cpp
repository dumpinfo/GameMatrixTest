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


#include "MGSpider.h"
#include "MGGame.h"


using namespace C4;


namespace
{
	const float kSpiderRunForce = 70.0F;
	const float kSpiderResistForce = 5.0F;
	const float kSpiderTurnRate = 0.02F;

	Storage<Spider> spiderStorage;

	const Type spiderVariantType[kSpiderVariantCount] =
	{
		kSpiderVariantNormal, kSpiderVariantVenomous
	};

	const MaterialData spiderGutsMaterialData =
	{
		{0.85F, 0.85F, 0.25F}
	};
}


int32 SpiderController::globalSpiderCount = 0;
unsigned_int32 SpiderController::globalSpiderFlags = 0;

bool (SpiderController::*SpiderController::spiderStateHandler[kSpiderStateCount])(void) =
{
	&SpiderController::HandleSleepingState,
	&SpiderController::HandleChasingState,
	&SpiderController::HandleAttackingState,
	&SpiderController::HandleJumpingState,
	&SpiderController::HandleRecoveringState,
	&SpiderController::HandleHurlingState
};


Spider::Spider() :
		spiderControllerRegistration(kControllerSpider, TheGame->GetStringTable()->GetString(StringID('CTRL', kControllerSpider))),
		spiderModelRegistration(kModelSpider, TheGame->GetStringTable()->GetString(StringID('MODL', 'MNST', kModelSpider)), "spider/Spider", kModelPrecache, kControllerSpider),
		spiderLegModelRegistration(kModelSpiderLeg, nullptr, "spider/leg/Leg", kModelPrecache | kModelPrivate),

		venomParticleSystemRegistration(kParticleSystemVenom, TheGame->GetStringTable()->GetString(StringID('PART', kParticleSystemVenom))),
		venomModelReg(kModelVenom, nullptr, "spider/Venom", kModelPrecache | kModelPrivate),

		venomousMaterialRegistration(kMaterialSpiderVenomous, "spider/Venomous"),
		gutsMaterialRegistration(kMaterialSpiderGuts, "spider/Guts", &spiderGutsMaterialData),

		generateSpiderRegistration(kMethodGenerateSpider, TheGame->GetStringTable()->GetString(StringID('MTHD', kMethodGenerateSpider))),
		getSpiderCountRegistration(kMethodGetSpiderCount, TheGame->GetStringTable()->GetString(StringID('MTHD', kMethodGetSpiderCount)), kMethodNoTarget | kMethodOutputValue)
{
}

Spider::~Spider()
{
}

void Spider::Construct(void)
{
	new(spiderStorage) Spider;
}

void Spider::Destruct(void)
{
	spiderStorage->~Spider();
}


inline SpiderConfiguration::SpiderConfiguration()
{
	spiderFlags = 0;
	spiderVariant = kSpiderVariantNormal;
}

inline SpiderConfiguration::SpiderConfiguration(const SpiderConfiguration& spiderConfiguration)
{
	spiderFlags = spiderConfiguration.spiderFlags;
	spiderVariant = spiderConfiguration.spiderVariant;
}

void SpiderConfiguration::Pack(Packer& data) const
{
	data << ChunkHeader('FLAG', 4);
	data << spiderFlags;

	data << ChunkHeader('VRNT', 4);
	data << spiderVariant;
}

bool SpiderConfiguration::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> spiderFlags;
			return (true); 

		case 'VRNT':
 
			data >> spiderVariant;
			return (true); 
	}

	return (false); 
}
 
int32 SpiderConfiguration::GetSettingCount(void) const 
{
	return (1);
}
 
Setting *SpiderConfiguration::GetSetting(int32 index) const
{
	const StringTable *table = TheGame->GetStringTable();

	if (index == 0)
	{
		int32 selection = 0;
		for (machine a = 1; a < kSpiderVariantCount; a++)
		{
			if (spiderVariant == spiderVariantType[a])
			{
				selection = a;
				break;
			}
		}

		const char *title = table->GetString(StringID('CTRL', kControllerSpider, 'VRNT'));
		MenuSetting *menu = new MenuSetting('VRNT', selection, title, kSpiderVariantCount);
		for (machine a = 0; a < kSpiderVariantCount; a++)
		{
			menu->SetMenuItemString(a, table->GetString(StringID('CTRL', kControllerSpider, 'VRNT', spiderVariantType[a])));
		}

		return (menu);
	}

	return (nullptr);
}

void SpiderConfiguration::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'VRNT')
	{
		int32 selection = static_cast<const MenuSetting *>(setting)->GetMenuSelection();
		spiderVariant = spiderVariantType[selection];
	}
}


inline void SpiderController::NewSoundTime(void)
{
	soundTime = Math::Random(5000) + 2000;
}

inline void SpiderController::NewHurlTime(void)
{
	hurlTime = Math::Random(1000) + 200;
}

inline void SpiderController::NewJumpDistance(void)
{
	jumpDistance = Math::RandomFloat(6.0F) + 6.0F;
}

SpiderController::SpiderController() :
		MonsterController(kControllerSpider),
		frameAnimatorObserver(this, &SpiderController::HandleAnimationEvent)
{
	SetControllerFlags(kControllerAsleep);

	SetMonsterHealth(7 << 16);
	SetMonsterRepelForce(16.0F);

	SetMonsterState(kSpiderStateSleeping);
	SetMonsterMotion(kSpiderMotionNone);

	spiderState = 0;
	NewSoundTime();
	NewHurlTime();
	NewJumpDistance();
}

SpiderController::SpiderController(const SpiderConfiguration& configuration) :
		MonsterController(kControllerSpider),
		SpiderConfiguration(configuration),
		frameAnimatorObserver(this, &SpiderController::HandleAnimationEvent)
{
	SetControllerFlags(kControllerAsleep);

	SetMonsterHealth(7 << 16);
	SetMonsterRepelForce(16.0F);

	SetMonsterState(kSpiderStateSleeping);
	SetMonsterMotion(kSpiderMotionNone);

	spiderState = 0;
	NewSoundTime();
	NewHurlTime();
	NewJumpDistance();
}

SpiderController::SpiderController(const SpiderController& spiderController) :
		MonsterController(spiderController),
		SpiderConfiguration(spiderController),
		frameAnimatorObserver(this, &SpiderController::HandleAnimationEvent)
{
	SetControllerFlags(kControllerAsleep);

	spiderState = 0;
	NewSoundTime();
	NewHurlTime();
	NewJumpDistance();
}

SpiderController::~SpiderController()
{
	UncountMonster(globalSpiderCount);
}

Controller *SpiderController::Replicate(void) const
{
	return (new SpiderController(*this));
}

bool SpiderController::ValidNode(const Node *node)
{
	return ((node->GetNodeType() == kNodeModel) && (static_cast<const Model *>(node)->GetModelType() == kModelSpider));
}

void SpiderController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	MonsterController::Pack(data, packFlags);
	SpiderConfiguration::Pack(data);

	data << ChunkHeader('STAT', 4);
	data << spiderState;

	if (!(packFlags & kPackEditor))
	{
		data << ChunkHeader('SDTM', 4);
		data << soundTime;
	}

	data << TerminatorChunk;
}

void SpiderController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	MonsterController::Unpack(data, unpackFlags);
	UnpackChunkList<SpiderController>(data, unpackFlags);
}

bool SpiderController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'STAT':

			data >> spiderState;
			return (true);

		case 'SDTM':

			data >> soundTime;
			return (true);
	}

	return (SpiderConfiguration::UnpackChunk(chunkHeader, data));
}

int32 SpiderController::GetSettingCount(void) const
{
	return (SpiderConfiguration::GetSettingCount());
}

Setting *SpiderController::GetSetting(int32 index) const
{
	return (SpiderConfiguration::GetSetting(index));
}

void SpiderController::SetSetting(const Setting *setting)
{
	SpiderConfiguration::SetSetting(setting);
}

void SpiderController::Preprocess(void)
{
	MonsterController::Preprocess();

	Model *model = GetTargetNode();
	bodyNode = nullptr;

	if (spiderVariant == kSpiderVariantVenomous)
	{
		SetCollisionExclusionMask(GetCollisionExclusionMask() | kCollisionVenom);

		AutoReleaseMaterial material(kMaterialSpiderVenomous);

		Node *node = model->GetFirstSubnode();
		while (node)
		{
			if (node->GetNodeType() == kNodeGeometry)
			{
				if (!bodyNode)
				{
					bodyNode = node;
				}

				Geometry *geometry = static_cast<Geometry *>(node);
				geometry->SetMaterialObject(0, material);
			}

			node = model->GetNextNode(node);
		}
	}

	if (!model->GetManipulator())
	{
		SetAttackable(true);

		AnimationBlender *animationBlender = GetAnimationBlender();
		animationBlender->SetFrameAnimatorObserver(&frameAnimatorObserver);

		int32 motion = GetMonsterMotion();
		Interpolator *interpolator = animationBlender->GetRecentAnimator()->GetFrameInterpolator();
		if (motion == kSpiderMotionLand)
		{
			interpolator->SetCompletionProc(&RecoverCallback, this);
		}
		else if (motion == kSpiderMotionHurl)
		{
			interpolator->SetCompletionProc(&HurlCallback, this);
		}
	}
}

void SpiderController::Wake(void)
{
	MonsterController::Wake();

	if (GetMonsterMotion() == kSpiderMotionNone)
	{
		SetMonsterState(kSpiderStateChasing);
		SetSpiderMotion(kSpiderMotionStand);
	}

	CountMonster(globalSpiderCount);
}

void SpiderController::Move(void)
{
	if ((this->*spiderStateHandler[GetMonsterState()])())
	{
		MonsterController::Move();
	}
}

bool SpiderController::HandleSleepingState(void)
{
	return (true);
}

bool SpiderController::HandleChasingState(void)
{
	Vector3D	direction;

	int32 dt = TheTimeMgr->GetDeltaTime();
	if ((soundTime -= dt) <= 0)
	{
		NewSoundTime();
		PlaySource("spider/Spider", 48.0F);
	}

	Vector2D force = GetRepulsionForce();

	if (GetEnemyDirection(&direction))
	{
		float targetDistance = Magnitude(direction);
		float azm0 = Atan(direction.y, direction.x);
		float azm = SetMonsterAzimuth(azm0, kSpiderTurnRate);

		if (GetMonsterMotion() == kSpiderMotionStand)
		{
			if (targetDistance > jumpDistance)
			{
				SetSpiderMotion(kSpiderMotionChase);
			}
		}

		if (targetDistance < jumpDistance)
		{
			if (Fabs(azm - azm0) < 0.5F)
			{
				NewJumpDistance();
				SetMonsterState(kSpiderStateAttacking);
				SetSpiderMotion(kSpiderMotionJump);
				SetCharacterState(GetCharacterState() | kCharacterJumping);
			}
			else
			{
				force += CosSin(azm) * kSpiderRunForce;
			}
		}
		else
		{
			if (spiderVariant == kSpiderVariantVenomous)
			{
				if (((hurlTime -= dt) <= 0) && (Fabs(azm - azm0) < 0.1F))
				{
					NewHurlTime();
					SetMonsterState(kSpiderStateHurling);
					SetSpiderMotion(kSpiderMotionHurl);
				}
				else
				{
					force += CosSin(azm) * kSpiderRunForce;
				}
			}
			else
			{
				force += CosSin(azm) * kSpiderRunForce;
			}
		}
	}

	if (GetCharacterState() & kCharacterGround)
	{
		SetExternalLinearResistance(Vector2D(kSpiderResistForce, kSpiderResistForce));
	}
	else
	{
		force *= 0.02F;
		SetExternalLinearResistance(Zero2D);
	}

	SetExternalForce(force);
	return (true);
}

bool SpiderController::HandleAttackingState(void)
{
	if (GetStateTime() >= 412)
	{
		Vector3D	direction;

		if (GetEnemyDirection(&direction))
		{
			SetMonsterState(kSpiderStateJumping);

			float f = 2.5F / Sqrt(direction.x * direction.x + direction.y * direction.y);
			ApplyImpulse(GetTargetNode()->GetInverseWorldTransform() * Vector3D(direction.x * f, direction.y * f, 1.0F));

			SetExternalForce(Zero3D);
			SetExternalLinearResistance(Zero2D);
			PurgeContacts();

			OmniSource *source = PlaySource("spider/Attack", 32.0F);
			source->SetSourceVolume(0.75F);
			return (true);
		}

		SetMonsterState(kSpiderStateSleeping);
	}

	SetExternalForce(GetRepulsionForce());
	SetExternalLinearResistance(Vector2D(kSpiderResistForce, kSpiderResistForce));
	return (true);
}

bool SpiderController::HandleJumpingState(void)
{
	if ((GetStateTime() >= 250) && (GetGroundContact()))
	{
		SetMonsterState(kSpiderStateRecovering);
		SetSpiderMotion(kSpiderMotionLand);
		spiderState &= ~kSpiderAttacked;
	}

	return (true);
}

bool SpiderController::HandleRecoveringState(void)
{
	SetExternalForce(GetRepulsionForce());
	SetExternalLinearResistance(Vector2D(kSpiderResistForce, kSpiderResistForce));
	return (true);
}

bool SpiderController::HandleHurlingState(void)
{
	SetExternalForce(GetRepulsionForce());
	SetExternalLinearResistance(Vector2D(kSpiderResistForce, kSpiderResistForce));
	return (true);
}

void SpiderController::HandleAnimationEvent(FrameAnimator *animator, CueType cueType)
{
	if (TheMessageMgr->Server())
	{
		if (cueType == 'DAMG')
		{
			if (GetMonsterState() == kSpiderStateHurling)
			{
				Vector3D	direction;

				if (GetEnemyDirection(&direction))
				{
					direction.z += 1.25F;
					float m = Magnitude(direction);
					Vector3D venomVelocity = direction * (16.0F / m);
					venomVelocity.z -= 0.03125F * m * GetPhysicsController()->GetGravityAcceleration().z;

					int32 projectileIndex = GetTargetNode()->GetWorld()->NewControllerIndex();
					Point3D venomPosition = bodyNode->GetWorldPosition() - bodyNode->GetWorldTransform()[1] * 0.6F;
					TheMessageMgr->SendMessageAll(CreateVenomMessage(projectileIndex, GetControllerIndex(), venomPosition, venomVelocity));
				}
			}
		}
	}
}

void SpiderController::SetSpiderMotion(int32 motion)
{
	SetMonsterMotion(motion);
	switch (motion)
	{
		case kSpiderMotionStand:

			GetAnimationBlender()->StartAnimation("spider/Run", kInterpolatorStop);
			break;

		case kSpiderMotionChase:
		{
			FrameAnimator *animator = GetAnimationBlender()->BlendAnimation("spider/Run", kInterpolatorForward | kInterpolatorLoop, 0.004F);
			animator->GetFrameInterpolator()->SetRate(2.0F);
			break;
		}

		case kSpiderMotionJump:
		{
			FrameAnimator *animator = GetAnimationBlender()->BlendAnimation("spider/Jump", kInterpolatorForward, 0.004F);
			Interpolator *interpolator = animator->GetFrameInterpolator();
			interpolator->SetMaxValue(1320.0F);
			interpolator->SetRate(2.0F);
			break;
		}

		case kSpiderMotionLand:
		{
			FrameAnimator *animator = GetAnimationBlender()->GetRecentAnimator();
			Interpolator *interpolator = animator->GetFrameInterpolator();
			interpolator->SetMaxValue(animator->GetAnimationDuration());
			interpolator->SetMode(kInterpolatorForward);
			interpolator->SetRate(1.0F);
			interpolator->SetCompletionProc(&RecoverCallback, this);
			break;
		}

		case kSpiderMotionHurl:
		{
			FrameAnimator *animator = GetAnimationBlender()->BlendAnimation("spider/Attack1", kInterpolatorForward, 0.004F);
			animator->GetFrameInterpolator()->SetCompletionProc(&HurlCallback, this);
			break;
		}
	}
}

void SpiderController::RecoverCallback(Interpolator *interpolator, void *cookie)
{
	SpiderController *controller = static_cast<SpiderController *>(cookie);
	controller->SetMonsterState(kSpiderStateChasing);
	controller->SetSpiderMotion(kSpiderMotionChase);

	interpolator->SetCompletionProc(nullptr);
}

void SpiderController::HurlCallback(Interpolator *interpolator, void *cookie)
{
	SpiderController *controller = static_cast<SpiderController *>(cookie);
	controller->SetMonsterState(kSpiderStateChasing);
	controller->SetSpiderMotion(kSpiderMotionChase);

	interpolator->SetCompletionProc(nullptr);
}

RigidBodyStatus SpiderController::HandleNewRigidBodyContact(const RigidBodyContact *contact, RigidBodyController *contactBody)
{
	if (contactBody->GetRigidBodyType() == kRigidBodyCharacter)
	{
		if (GetMonsterMotion() == kSpiderMotionJump)
		{
			unsigned_int32 flags = spiderState;
			if (!(flags & kSpiderAttacked))
			{
				spiderState = flags | kSpiderAttacked;

				if (contactBody->GetControllerType() != kControllerSpider)
				{
					static_cast<GameCharacterController *>(contactBody)->Damage(5 << 16, 0, this);
					return (kRigidBodyContactsBroken);
				}
			}
		}

		return (kRigidBodyUnchanged);
	}

	return (MonsterController::HandleNewRigidBodyContact(contact, contactBody));
}

CharacterStatus SpiderController::Damage(Fixed damage, unsigned_int32 flags, GameCharacterController *attacker, const Point3D *position, const Vector3D *force)
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
			World *world = model->GetWorld();

			BloodParticleSystem *blood = new BloodParticleSystem(spiderGutsMaterialData.materialColor, particleCount);
			blood->SetNodePosition(*position);
			world->AddNewNode(blood);

			SpiderGutsParticleSystem *spiderGuts = new SpiderGutsParticleSystem(kMaterialSpiderGuts, spiderGutsMaterialData.materialColor, particleCount >> 1);
			spiderGuts->SetNodePosition(*position);
			world->AddNewNode(spiderGuts);
		}
	}

	return (MonsterController::Damage(damage, flags, attacker, position, force));
}

void SpiderController::Kill(GameCharacterController *attacker, const Point3D *position, const Vector3D *force)
{
	MonsterController::Kill(attacker, position, force);

	Model *model = GetTargetNode();
	World *world = model->GetWorld();
	const Point3D& modelPosition = model->GetWorldPosition();

	unsigned_int32 flags = globalSpiderFlags;
	if (!(flags & kSpiderDeathSound))
	{
		// Set a flag that prevents the death sound from being played more than once per frame.

		globalSpiderFlags = flags | kSpiderDeathSound;

		OmniSource *source = new OmniSource("spider/Splat", 64.0F);
		source->SetNodePosition(modelPosition);
		world->AddNewNode(source);
		source->SetSourceVolume(2.0F);

		if (Math::Random(5) == 0)
		{
			static const char *soundName[2] =
			{
				"spider/Death1", "spider/Death2"
			};

			source = new OmniSource(soundName[Math::Random(2)], 64.0F);
			source->SetNodePosition(modelPosition);
			world->AddNewNode(source);
		}
	}

	int32 count = 3 - TheGame->GetGameDetailLevel();
	for (machine a = 0; a < count; a++)
	{
		Model *leg = Model::Get(kModelSpiderLeg);

		RemainsController *controller = new RemainsController(Math::Random(5000, 8000));
		leg->SetController(controller);

		leg->SetNodePosition(Point3D(modelPosition.x, modelPosition.y, modelPosition.z + 0.25F));
		world->AddNewNode(leg);

		Vector3D linearVelocity = Math::RandomUnitVector3D() * 10.0F;
		linearVelocity.z = Fabs(linearVelocity.z);
		controller->SetLinearVelocity(linearVelocity);

		controller->SetAngularVelocity(Math::RandomUnitVector3D() * (Math::RandomFloat(4.0F) + 4.0F));
	}

	delete model;
}


GenerateSpiderMethod::GenerateSpiderMethod() : GenerateMonsterMethod(kMethodGenerateSpider)
{
}

GenerateSpiderMethod::GenerateSpiderMethod(const GenerateSpiderMethod& generateSpiderMethod) :
		GenerateMonsterMethod(generateSpiderMethod),
		SpiderConfiguration(generateSpiderMethod)
{
}

GenerateSpiderMethod::~GenerateSpiderMethod()
{
}

Method *GenerateSpiderMethod::Replicate(void) const
{
	return (new GenerateSpiderMethod(*this));
}

void GenerateSpiderMethod::Pack(Packer& data, unsigned_int32 packFlags) const
{
	GenerateMonsterMethod::Pack(data, packFlags);
	SpiderConfiguration::Pack(data);
	data << TerminatorChunk;
}

void GenerateSpiderMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	GenerateMonsterMethod::Unpack(data, unpackFlags);
	UnpackChunkList<GenerateSpiderMethod>(data, unpackFlags);
}

bool GenerateSpiderMethod::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	return (SpiderConfiguration::UnpackChunk(chunkHeader, data));
}

int32 GenerateSpiderMethod::GetSettingCount(void) const
{
	return (SpiderConfiguration::GetSettingCount());
}

Setting *GenerateSpiderMethod::GetSetting(int32 index) const
{
	return (SpiderConfiguration::GetSetting(index));
}

void GenerateSpiderMethod::SetSetting(const Setting *setting)
{
	SpiderConfiguration::SetSetting(setting);
}

void GenerateSpiderMethod::Execute(const ScriptState *state)
{
	Node *node = GetTargetNode(state);
	if (node)
	{
		GamePlayer *player = static_cast<GamePlayer *>(TheMessageMgr->GetLocalPlayer());
		if (player->GetPlayerController())
		{
			Model *model = Model::Get(kModelSpider);
			SpiderController *controller = new SpiderController(*this);
			GenerateMonster(node, model, controller);
		}
	}

	CallCompletionProc();
}


GetSpiderCountMethod::GetSpiderCountMethod() : Method(kMethodGetSpiderCount)
{
}

GetSpiderCountMethod::GetSpiderCountMethod(const GetSpiderCountMethod& getSpiderCountMethod) : Method(getSpiderCountMethod)
{
}

GetSpiderCountMethod::~GetSpiderCountMethod()
{
}

Method *GetSpiderCountMethod::Replicate(void) const
{
	return (new GetSpiderCountMethod(*this));
}

void GetSpiderCountMethod::Execute(const ScriptState *state)
{
	SetOutputValue(state, SpiderController::GetGlobalSpiderCount());
	CallCompletionProc();
}


SpiderGutsParticleSystem::SpiderGutsParticleSystem() :
		BlobParticleSystem(kParticleSystemSpiderGuts, &particlePool, 0.0375F, 0.15F),
		particlePool(kMaxParticleCount, particleArray)
{
}

SpiderGutsParticleSystem::SpiderGutsParticleSystem(MaterialType material, const ColorRGB& color, int32 count) :
		BlobParticleSystem(kParticleSystemSpiderGuts, &particlePool, 0.0375F, 0.15F),
		particlePool(kMaxParticleCount, particleArray)
{
	gutsMaterial = material;
	gutsColor = color;
	gutsCount = count;
}

SpiderGutsParticleSystem::~SpiderGutsParticleSystem()
{
}

void SpiderGutsParticleSystem::Pack(Packer& data, unsigned_int32 packFlags) const
{
	BlobParticleSystem::Pack(data, packFlags);

	data << ChunkHeader('MATL', 4);
	data << gutsMaterial;

	data << ChunkHeader('COLR', sizeof(ColorRGB));
	data << gutsColor;

	data << TerminatorChunk;
}

void SpiderGutsParticleSystem::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	BlobParticleSystem::Unpack(data, unpackFlags);
	UnpackChunkList<SpiderGutsParticleSystem>(data, unpackFlags);
}

bool SpiderGutsParticleSystem::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'MATL':

			data >> gutsMaterial;
			return (true);

		case 'COLR':

			data >> gutsColor;
			return (true);
	}

	return (false);
}

void SpiderGutsParticleSystem::Preprocess(void)
{
	BlobParticleSystem::Preprocess();
	SetParticleSystemFlags(kParticleSystemSelfDestruct);
	SetMaterialObject(AutoReleaseMaterial(gutsMaterial));

	if (!GetFirstParticle())
	{
		Point3D center = GetSuperNode()->GetWorldTransform() * GetNodePosition();

		int32 particleCount = Max(gutsCount >> 2, 2);
		for (machine a = 0; a < particleCount; a++)
		{
			BlobParticle *particle = particlePool.NewParticle();
			if (!particle)
			{
				return;
			}

			particle->velocity = Math::RandomUnitVector3D() * (Math::RandomFloat(0.01F) + 0.0025F);
			particle->position = center + particle->velocity * 10.0F;
			particle->lifeTime = 1500 + Math::Random(1000);
			particle->radius = Math::RandomFloat(0.06F) + 0.06F;
			particle->scaleAxis = Math::RandomUnitVector3D();
			particle->inverseScale = Math::RandomFloat(0.5F) + 1.0F;

			AddParticle(particle);

			int32 count = Math::Random(3) + 1;
			particle->emitTime = -count;

			for (machine b = 0; b < count; b++)
			{
				BlobParticle *p = particlePool.NewParticle();
				if (!p)
				{
					return;
				}

				p->velocity = particle->velocity + Math::RandomUnitVector3D() * 0.0002F;
				p->position = particle->position;
				p->emitTime = 0;
				p->lifeTime = particle->lifeTime;
				p->radius = Math::RandomFloat(0.06F) + 0.06F;
				p->scaleAxis = Math::RandomUnitVector3D();
				p->inverseScale = Math::RandomFloat(0.5F) + 1.0F;

				AddParticle(p);
			}
		}
	}
}

void SpiderGutsParticleSystem::AnimateParticles(void)
{
	int32 dt = TheTimeMgr->GetDeltaTime();
	float fdt = TheTimeMgr->GetFloatDeltaTime();

	World *world = GetWorld();

	BlobParticle *particle = static_cast<BlobParticle *>(GetFirstParticle());
	while (particle)
	{
		BlobParticle *next = particle->GetNextParticle();

		int32 life = (particle->lifeTime -= dt);
		if (life > 0)
		{
			particle->velocity.z += K::gravity * fdt;

			if (particle->emitTime == 0)
			{
				particle->position += particle->velocity * fdt;
			}
			else
			{
				CollisionData	collisionData;

				Point3D newPosition = particle->position + particle->velocity * fdt;
				if (world->DetectCollision(particle->position, newPosition, 0.0F, kCollisionRemains, &collisionData))
				{
					int32 i = Math::Random(4);
					MarkingData markingData(Vector2D(0.5F, 0.5F), Vector2D((i & 1) ? 0.5F : 0.0F, (i & 2) ? 0.5F : 0.0F), kMarkingLight | kMarkingDepthWrite | kMarkingForceClip);
					AutoReleaseMaterial material(kMaterialSplatter);

					markingData.center = collisionData.position;
					markingData.normal = collisionData.normal;
					markingData.tangent = Math::CreatePerpendicular(markingData.normal);
					markingData.tangent.RotateAboutAxis(Math::RandomFloat(K::tau), markingData.normal);
					markingData.radius = Math::RandomFloat(0.5F) + 0.25F;
					markingData.materialObject = material;
					markingData.color = gutsColor * 1.25F;
					markingData.lifeTime = 30000;
					MarkingEffect::New(world, &markingData);

					int32 count = particle->emitTime;
					FreeParticle(particle);

					do
					{
						particle = next;
						if (!particle)
						{
							break;
						}

						next = particle->GetNextParticle();
						FreeParticle(particle);
					} while (++count < 0);

					goto nextParticle;
				}

				particle->position = newPosition;
			}
		}
		else
		{
			FreeParticle(particle);
		}

		nextParticle:
		particle = next;
	}
}

// ZYUQURM
