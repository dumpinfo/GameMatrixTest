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


#include "MGWitch.h"
#include "MGBlackCat.h"
#include "MGGame.h"


using namespace C4;


namespace
{
	const float kWitchRunForce = 180.0F;
	const float kWitchResistForce = 20.0F;
	const float kWitchTurnRate = 0.005F;
	const float kWitchFlyTurnRate = 0.001F;
	const float kWitchAttackDistance = 2.0F;

	Storage<Witch> witchStorage;

	const Type witchVariantType[kWitchVariantCount] =
	{
		kWitchVariantNormal, kWitchVariantMaster
	};
}


int32 WitchController::globalWitchCount = 0;

bool (WitchController::*WitchController::witchStateHandler[kWitchStateCount])(void) =
{
	&WitchController::HandleSleepingState,
	&WitchController::HandleStandingState,
	&WitchController::HandleFlyingState,
	&WitchController::HandleChasingState,
	&WitchController::HandleAttackingState,
	&WitchController::HandleThrowingState,
	&WitchController::HandleFlyingState,
	&WitchController::HandleRecoveringState,
	&WitchController::HandleDeadState
};


Witch::Witch() :
		witchControllerRegistration(kControllerWitch, TheGame->GetStringTable()->GetString(StringID('CTRL', kControllerWitch))),
		witchModelRegistration(kModelWitch, TheGame->GetStringTable()->GetString(StringID('MODL', 'MNST', kModelWitch)), "witch/Witch", kModelPrecache, kControllerWitch),
		witchHatModelRegistration(kModelWitchHat, nullptr, "witch/hat/Hat", kModelPrecache | kModelPrivate),
		witchCatModelRegistration(kModelWitchCat, nullptr, "witch/cat/Cat", kModelPrecache | kModelPrivate),

		masterMaterialRegistration(kMaterialWitchMaster, "witch/Cloth-B"),

		generateWitchRegistration(kMethodGenerateWitch, TheGame->GetStringTable()->GetString(StringID('MTHD', kMethodGenerateWitch))),
		getWitchCountRegistration(kMethodGetWitchCount, TheGame->GetStringTable()->GetString(StringID('MTHD', kMethodGetWitchCount)), kMethodNoTarget | kMethodOutputValue)
{
}

Witch::~Witch()
{
}

void Witch::Construct(void)
{
	new(witchStorage) Witch;
}

void Witch::Destruct(void)
{
	witchStorage->~Witch();
}


inline WitchConfiguration::WitchConfiguration()
{
	witchFlags = 0;
	witchVariant = kWitchVariantNormal;
	attackMask = kWitchAttackCat;
}

inline WitchConfiguration::WitchConfiguration(const WitchConfiguration& witchConfiguration)
{
	witchFlags = witchConfiguration.witchFlags;
	witchVariant = witchConfiguration.witchVariant;
	attackMask = witchConfiguration.attackMask;
}

void WitchConfiguration::Pack(Packer& data) const
{
	data << ChunkHeader('FLAG', 4);
	data << witchFlags;

	data << ChunkHeader('VRNT', 4);
	data << witchVariant;

	data << ChunkHeader('ATCK', 4);
	data << attackMask;
}

bool WitchConfiguration::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':
 
			data >> witchFlags;
			return (true);
 
		case 'VRNT':
 
			data >> witchVariant;
			return (true);
 
		case 'ATCK':
 
			data >> attackMask; 
			return (true);
	}

	return (false); 
}

int32 WitchConfiguration::GetSettingCount(void) const
{
	return (8);
}

Setting *WitchConfiguration::GetSetting(int32 index) const
{
	const StringTable *table = TheGame->GetStringTable();

	if (index == 0)
	{
		int32 selection = 0;
		for (machine a = 1; a < kWitchVariantCount; a++)
		{
			if (witchVariant == witchVariantType[a])
			{
				selection = a;
				break;
			}
		}

		const char *title = table->GetString(StringID('CTRL', kControllerWitch, 'VRNT'));
		MenuSetting *menu = new MenuSetting('VRNT', selection, title, kWitchVariantCount);
		for (machine a = 0; a < kWitchVariantCount; a++)
		{
			menu->SetMenuItemString(a, table->GetString(StringID('CTRL', kControllerWitch, 'VRNT', witchVariantType[a])));
		}

		return (menu);
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerWitch, 'FLYB'));
		return (new BooleanSetting('FLYB', ((witchFlags & kWitchFlying) != 0), title));
	}

	if (index == 2)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerWitch, 'HAT '));
		return (new BooleanSetting('HAT ', ((witchFlags & kWitchHat) != 0), title));
	}

	if (index == 3)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerWitch, 'ATCK'));
		return (new HeadingSetting('ATCK', title));
	}

	if (index == 4)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerWitch, 'ATCK', 'CATS'));
		return (new BooleanSetting('CATS', ((attackMask & kWitchAttackCat) != 0), title));
	}

	if (index == 5)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerWitch, 'ATCK', 'AXES'));
		return (new BooleanSetting('AXES', ((attackMask & kWitchAttackAxe) != 0), title));
	}

	if (index == 6)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerWitch, 'ATCK', 'KNIF'));
		return (new BooleanSetting('KNIF', ((attackMask & kWitchAttackKnife) != 0), title));
	}

	if (index == 7)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerWitch, 'ATCK', 'POTN'));
		return (new BooleanSetting('POTN', ((attackMask & kWitchAttackPotion) != 0), title));
	}

	return (nullptr);
}

void WitchConfiguration::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'VRNT')
	{
		int32 selection = static_cast<const MenuSetting *>(setting)->GetMenuSelection();
		witchVariant = witchVariantType[selection];
	}
	else if (identifier == 'FLYB')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			witchFlags |= kWitchFlying;
		}
		else
		{
			witchFlags &= ~kWitchFlying;
		}
	}
	else if (identifier == 'HAT ')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			witchFlags |= kWitchHat;
		}
		else
		{
			witchFlags &= ~kWitchHat;
		}
	}
	else if (identifier == 'CATS')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			attackMask |= kWitchAttackCat;
		}
		else
		{
			attackMask &= ~kWitchAttackCat;
		}
	}
	else if (identifier == 'AXES')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			attackMask |= kWitchAttackAxe;
		}
		else
		{
			attackMask &= ~kWitchAttackAxe;
		}
	}
	else if (identifier == 'KNIF')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			attackMask |= kWitchAttackKnife;
		}
		else
		{
			attackMask &= ~kWitchAttackKnife;
		}
	}
	else if (identifier == 'POTN')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			attackMask |= kWitchAttackPotion;
		}
		else
		{
			attackMask &= ~kWitchAttackPotion;
		}
	}
}


inline void WitchController::NewThrowTime(void)
{
	throwTime = Math::Random(1000) + 500;
}

WitchController::WitchController() :
		MonsterController(kControllerWitch),
		frameAnimatorObserver(this, &WitchController::HandleAnimationEvent)
{
	SetControllerFlags(kControllerAsleep);

	SetMonsterState(kWitchStateSleeping);
	SetMonsterMotion(kWitchMotionNone);

	SetSourcePosition(Point3D(0.0F, 0.0F, 1.5F));

	targetRoll = 0.0F;
	currentRoll = 0.0F;
	NewThrowTime();

	broomSmokeParticleSystem = nullptr;
	magicBroomParticleSystem = nullptr;
}

WitchController::WitchController(const WitchConfiguration& configuration) :
		MonsterController(kControllerWitch),
		WitchConfiguration(configuration),
		frameAnimatorObserver(this, &WitchController::HandleAnimationEvent)
{
	SetControllerFlags(kControllerAsleep);

	SetMonsterState(kWitchStateSleeping);
	SetMonsterMotion(kWitchMotionNone);

	SetSourcePosition(Point3D(0.0F, 0.0F, 1.5F));

	targetRoll = 0.0F;
	currentRoll = 0.0F;
	NewThrowTime();

	broomSmokeParticleSystem = nullptr;
	magicBroomParticleSystem = nullptr;
}

WitchController::WitchController(const WitchController& witchController) :
		MonsterController(witchController),
		WitchConfiguration(witchController),
		frameAnimatorObserver(this, &WitchController::HandleAnimationEvent)
{
	SetControllerFlags(kControllerAsleep);

	targetRoll = 0.0F;
	currentRoll = 0.0F;
	NewThrowTime();

	broomSmokeParticleSystem = nullptr;
	magicBroomParticleSystem = nullptr;
}

WitchController::~WitchController()
{
	if (broomSmokeParticleSystem)
	{
		broomSmokeParticleSystem->Finalize();
	}

	if (magicBroomParticleSystem)
	{
		magicBroomParticleSystem->Finalize();
	}

	UncountMonster(globalWitchCount);
}

Controller *WitchController::Replicate(void) const
{
	return (new WitchController(*this));
}

bool WitchController::ValidNode(const Node *node)
{
	return ((node->GetNodeType() == kNodeModel) && (static_cast<const Model *>(node)->GetModelType() == kModelWitch));
}

void WitchController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	MonsterController::Pack(data, packFlags);
	WitchConfiguration::Pack(data);

	if (!(packFlags & kPackEditor))
	{
		data << ChunkHeader('THTM', 4);
		data << throwTime;

		if (GetMonsterState() == kWitchStateStanding)
		{
			data << ChunkHeader('WTTM', 4);
			data << waitTime;
		}

		if ((broomSmokeParticleSystem) && (broomSmokeParticleSystem->LinkedNodePackable(packFlags)))
		{
			data << ChunkHeader('BSMK', 4);
			data << broomSmokeParticleSystem->GetNodeIndex();
		}

		if ((magicBroomParticleSystem) && (magicBroomParticleSystem->LinkedNodePackable(packFlags)))
		{
			data << ChunkHeader('BROM', 4);
			data << magicBroomParticleSystem->GetNodeIndex();
		}
	}

	data << TerminatorChunk;
}

void WitchController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	MonsterController::Unpack(data, unpackFlags);
	UnpackChunkList<WitchController>(data, unpackFlags);
}

bool WitchController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'THTM':

			data >> throwTime;
			return (true);

		case 'WTTM':

			data >> waitTime;
			return (true);

		case 'BSMK':
		{
			int32	nodeIndex;

			data >> nodeIndex;
			data.AddNodeLink(nodeIndex, &BroomSmokeLinkProc, this);
			return (true);
		}

		case 'BROM':
		{
			int32	nodeIndex;

			data >> nodeIndex;
			data.AddNodeLink(nodeIndex, &MagicBroomLinkProc, this);
			return (true);
		}
	}

	return (WitchConfiguration::UnpackChunk(chunkHeader, data));
}

void WitchController::BroomSmokeLinkProc(Node *node, void *cookie)
{
	WitchController *controller = static_cast<WitchController *>(cookie);
	controller->broomSmokeParticleSystem = static_cast<BroomSmokeParticleSystem *>(node);
}

void WitchController::MagicBroomLinkProc(Node *node, void *cookie)
{
	WitchController *controller = static_cast<WitchController *>(cookie);
	controller->magicBroomParticleSystem = static_cast<MagicBroomParticleSystem *>(node);
}

int32 WitchController::GetSettingCount(void) const
{
	return (WitchConfiguration::GetSettingCount());
}

Setting *WitchController::GetSetting(int32 index) const
{
	return (WitchConfiguration::GetSetting(index));
}

void WitchController::SetSetting(const Setting *setting)
{
	WitchConfiguration::SetSetting(setting);
}

void WitchController::Preprocess(void)
{
	Model *model = GetTargetNode();
	if (!model->GetManipulator())
	{
		unsigned_int32 monsterFlags = GetMonsterFlags();
		if (!(monsterFlags & kMonsterInitialized))
		{
			SetMonsterFlags(monsterFlags | kMonsterInitialized);

			int32 health = 80;

			if (witchVariant != kWitchVariantNormal)
			{
				health += 20;
			}

			if (witchFlags & kWitchHat)
			{
				health += 20;
			}

			SetMonsterHealth(health << 16);
		}

		witchSource = nullptr;

		if (witchFlags & kWitchFlying)
		{
			Node *node = model->GetFirstSubnode();
			while (node)
			{
				Node *next = node->Next();

				NodeType type = node->GetNodeType();
				if (type == kNodeShape)
				{
					if (static_cast<Shape *>(node)->GetShapeType() != kShapeBox)
					{
						delete node;
					}
				}
				else if (type == kNodeSource)
				{
					witchSource = static_cast<OmniSource *>(node);
				}

				node = next;
			}
		}
		else
		{
			Node *node = model->GetFirstSubnode();
			while (node)
			{
				if ((node->GetNodeType() == kNodeShape) && (static_cast<Shape *>(node)->GetShapeType() == kShapeBox))
				{
					delete node;
					break;
				}

				node = node->Next();
			}

			Model *blackCat = Model::Get(kModelWitchCat);
			blackCatModel = blackCat;

			blackCat->SetNodeFlags(kNodeNonpersistent | kNodeCloneInhibit | kNodeAnimateInhibit);
			blackCat->SetNodeMatrix3D(Matrix3D().SetRotationAboutZ(K::tau_over_8) * Matrix3D().SetRotationAboutX(-K::tau_over_4));
			blackCat->SetNodePosition(Point3D(0.3F, 0.0F, 0.0F));
			blackCat->Disable();

			node = blackCat->GetFirstSubnode();
			while (node)
			{
				if (node->GetNodeType() == kNodeGeometry)
				{
					Geometry *geometry = static_cast<Geometry *>(node);
					if (geometry->GetGeometryType() == kGeometryGeneric)
					{
						geometry->SetRenderableFlags(geometry->GetRenderableFlags() | kRenderableMotionBlurGradient);
					}
				}

				node = node->Next();
			}
		}
	}

	MonsterController::Preprocess();
	SetCollisionExclusionMask(GetCollisionExclusionMask() | kCollisionBlackCat);

	if (witchVariant == kWitchVariantMaster)
	{
		Node *node = model->GetFirstSubnode();
		while (node)
		{
			if (node->GetNodeType() == kNodeGeometry)
			{
				Geometry *geometry = static_cast<Geometry *>(node);
				geometry->SetMaterialObject(1, AutoReleaseMaterial(kMaterialWitchMaster));
				break;
			}

			node = node->Next();
		}
	}

	if (!model->GetManipulator())
	{
		Node *hand = model->FindNode(Text::StaticHash<'W', 'i', 't', 'c', 'h', '_', 'R', '_', 'H', 'a', 'n', 'd'>::value);

		if (witchFlags & kWitchFlying)
		{
			SetGravityMultiplier(0.0F);
			SetFrictionCoefficient(0.0F);
			SetCharacterState(kCharacterFlying);

			witchSource->SetSourceVelocity(GetLinearVelocity());

			if (!broomSmokeParticleSystem)
			{
				broomSmokeParticleSystem = new BroomSmokeParticleSystem((witchVariant == kWitchVariantMaster) ? ColorRGBA(1.0F, 0.25F, 0.25F, 0.675F) : ColorRGBA(0.25F, 1.0F, 0.25F, 0.675F));
				model->GetOwningZone()->AppendNewSubnode(broomSmokeParticleSystem);
			}

			if (!magicBroomParticleSystem)
			{
				magicBroomParticleSystem = new MagicBroomParticleSystem;
				model->GetOwningZone()->AppendNewSubnode(magicBroomParticleSystem);
			}

			Node *node = hand->GetFirstSubnode();
			while (node)
			{
				if (node->GetNodeType() == kNodeGeometry)
				{
					broomNode = node;
					break;
				}

				node = node->Next();
			}

			hand = model->FindNode(Text::StaticHash<'W', 'i', 't', 'c', 'h', '_', 'L', '_', 'H', 'a', 'n', 'd'>::value);
		}
		else
		{
			hand->AppendSubnode(blackCatModel);

			SetAttackable(true);
		}

		Node *subnode = hand->GetFirstSubnode();
		while (subnode)
		{
			if (subnode->GetNodeType() == kNodeGeometry)
			{
				subnode->Disable();
				break;
			}

			subnode = subnode->Next();
		}

		if (witchFlags & kWitchHat)
		{
			Node *headNode = model->FindNode(Text::StaticHash<'W', 'i', 't', 'c', 'h', '_', 'H', 'e', 'a', 'd'>::value);

			Model *hat = Model::Get(kModelWitchHat);
			hat->SetNodeFlags(kNodeNonpersistent | kNodeCloneInhibit | kNodeAnimateInhibit);
			headNode->AppendNewSubnode(hat);
		}

		AnimationBlender *animationBlender = GetAnimationBlender();
		animationBlender->SetFrameAnimatorObserver(&frameAnimatorObserver);

		int32 motion = GetMonsterMotion();
		Interpolator *interpolator = animationBlender->GetRecentAnimator()->GetFrameInterpolator();

		if (motion == kWitchMotionAttack)
		{
			interpolator->SetCompletionProc(&AttackCallback, this);
		}
		else if (motion == kWitchMotionThrow)
		{
			interpolator->SetCompletionProc(&ThrowCallback, this);
		}
		else if (motion == kWitchMotionFlyThrow)
		{
			interpolator->SetCompletionProc(&FlyThrowCallback, this);
		}
		else if (motion == kWitchMotionDamage)
		{
			interpolator->SetCompletionProc(&DamageCallback, this);
		}
	}
}

void WitchController::Wake(void)
{
	MonsterController::Wake();

	if (GetMonsterState() != kWitchStateDead)
	{
		if (witchFlags & kWitchFlying)
		{
			if (GetMonsterMotion() == kWitchMotionNone)
			{
				SetMonsterState(kWitchStateFlying);
				SetWitchMotion(kWitchMotionFly);
			}
		}
		else
		{
			blackCatModel->Disable();

			if (GetMonsterMotion() == kWitchMotionNone)
			{
				GetAnimationBlender()->StartAnimation("witch/Stand", kInterpolatorStop);
				SetMonsterState(kWitchStateChasing);
				SetWitchMotion(kWitchMotionChase);
			}
		}
	}

	CountMonster(globalWitchCount);
}

void WitchController::Move(void)
{
	if ((this->*witchStateHandler[GetMonsterState()])())
	{
		float roll = currentRoll;
		if (roll != targetRoll)
		{
			float dr = TheTimeMgr->GetFloatDeltaTime() * 2.0e-6F;
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

bool WitchController::HandleSleepingState(void)
{
	return (true);
}

bool WitchController::HandleStandingState(void)
{
	if (GetStateTime() >= waitTime)
	{
		Vector3D	direction;

		if (GetEnemyDirection(&direction))
		{
			if (Magnitude(direction) > kWitchAttackDistance)
			{
				SetMonsterState(kWitchStateChasing);
				SetWitchMotion(kWitchMotionChase);
			}
			else
			{
				SetMonsterState(kWitchStateAttacking);
				SetWitchMotion(kWitchMotionAttack);
			}
		}
	}

	SetExternalForce(GetRepulsionForce());
	SetExternalLinearResistance(Vector2D(kWitchResistForce, kWitchResistForce));
	return (true);
}

bool WitchController::HandleFlyingState(void)
{
	Vector3D	direction;

	Vector3D force = GetRepulsionForce();

	if (GetEnemyDirection(&direction))
	{
		float targetDistance = Magnitude(direction.GetVector2D());
		float azm = SetMonsterAzimuth(Atan(direction.y, direction.x), kWitchFlyTurnRate, &targetRoll);

		float m = Clamp(targetDistance * 0.1F + 0.5F, 1.5F, 2.5F) * kWitchRunForce;
		force.GetVector2D() += CosSin(azm) * m;

		float h = Fmin(targetDistance * 0.4F, 25.0F) + 5.0F;
		force.z = (h + direction.z) * (kWitchRunForce * 0.125F);

		if (GetMonsterState() == kWitchStateFlying)
		{
			if (((throwTime = MaxZero(throwTime - TheTimeMgr->GetDeltaTime())) <= 0) && (targetDistance < 50.0F))
			{
				NewThrowTime();

				SetMonsterState(kWitchStateFlyThrowing);
				SetWitchMotion(kWitchMotionFlyThrow);
			}
		}
	}

	if (!witchSource->Playing())
	{
		witchSource->Play();
	}

	witchSource->SetSourceVelocity(GetLinearVelocity());

	SetExternalForce(force);
	SetExternalLinearResistance(Vector3D(kWitchResistForce, kWitchResistForce, kWitchResistForce));

	Point3D position = broomNode->GetWorldPosition() - broomNode->GetWorldTransform()[2] * 0.75F;
	const Vector3D& velocity = GetLinearVelocity();
	broomSmokeParticleSystem->SetBroomState(position, velocity);
	magicBroomParticleSystem->SetBroomState(position, velocity);
	return (true);
}

bool WitchController::HandleChasingState(void)
{
	Vector3D	direction;

	Vector2D force = GetRepulsionForce();

	if (GetEnemyDirection(&direction))
	{
		float targetDistance = Magnitude(direction);
		float azm = SetMonsterAzimuth(Atan(direction.y, direction.x), kWitchTurnRate);

		if ((targetDistance > kWitchAttackDistance) || (GetStateTime() < 250))
		{
			force += CosSin(azm) * kWitchRunForce;

			if (((throwTime = MaxZero(throwTime - TheTimeMgr->GetDeltaTime())) <= 0) && (targetDistance < 15.0F))
			{
				CollisionData	data;

				NewThrowTime();

				Model *model = GetTargetNode();
				Point3D position = model->GetWorldPosition();
				position.z += 1.0F;

				if (!model->GetWorld()->DetectCollision(position, position + direction, 0.0F, kCollisionSightPath, &data))
				{
					SetMonsterState(kWitchStateThrowing);
					SetWitchMotion(kWitchMotionThrow);
				}
			}
		}
		else
		{
			SetMonsterState(kWitchStateAttacking);
			SetWitchMotion(kWitchMotionAttack);
		}
	}

	if (GetCharacterState() & kCharacterGround)
	{
		SetExternalLinearResistance(Vector2D(kWitchResistForce, kWitchResistForce));
	}
	else
	{
		force *= 0.02F;
		SetExternalLinearResistance(Zero2D);
	}

	SetExternalForce(force);
	return (true);
}

bool WitchController::HandleAttackingState(void)
{
	Vector3D	direction;

	if (GetEnemyDirection(&direction))
	{
		SetMonsterAzimuth(Atan(direction.y, direction.x), kWitchTurnRate);
	}

	SetExternalForce(GetRepulsionForce());
	SetExternalLinearResistance(Vector2D(kWitchResistForce, kWitchResistForce));
	return (true);
}

bool WitchController::HandleThrowingState(void)
{
	Vector3D	direction;

	if (GetEnemyDirection(&direction))
	{
		SetMonsterAzimuth(Atan(direction.y, direction.x), kWitchTurnRate);
	}
	else
	{
		blackCatModel->Disable();

		SetMonsterState(kWitchStateStanding);
		SetWitchMotion(kWitchMotionStand);
		waitTime = 250;
	}

	SetExternalForce(GetRepulsionForce());
	SetExternalLinearResistance(Vector2D(kWitchResistForce, kWitchResistForce));
	return (true);
}

bool WitchController::HandleRecoveringState(void)
{
	SetExternalForce(GetRepulsionForce());
	SetExternalLinearResistance(Vector2D(kWitchResistForce, kWitchResistForce));
	return (true);
}

bool WitchController::HandleDeadState(void)
{
	if (GetStateTime() >= 10000)
	{
		delete GetTargetNode();
		return (false);
	}

	SetExternalForce(Zero3D);

	if (GetCharacterState() & kCharacterGround)
	{
		SetExternalLinearResistance(Vector2D(kWitchResistForce, kWitchResistForce));
	}
	else
	{
		SetExternalLinearResistance(Zero3D);
	}

	return (true);
}

void WitchController::HandleAnimationEvent(FrameAnimator *animator, CueType cueType)
{
	switch (cueType)
	{
		case 'DAMG':

			DamageEnemy(15 << 16, kWitchAttackDistance);
			break;

		case 'ATT2':

			PlaySource("witch/Attack2", 32.0F);
			break;

		case 'TSND':

			PlaySource("witch/Throw", 48.0F);
			break;

		case 'SHOW':

			if ((GetMonsterMotion() != kWitchMotionDeath) && (!blackCatModel->Enabled()))
			{
				blackCatModel->Enable();
			}

			break;

		case 'THRO':

			blackCatModel->Disable();
			if (TheMessageMgr->Server())
			{
				Vector3D	direction;

				if (GetEnemyDirection(&direction))
				{
					float d = Magnitude(direction) - 0.5F;
					Vector3D blackCatDirection(20.0F, 0.0F, d * -0.02F * GetPhysicsController()->GetGravityAcceleration().z);
					blackCatDirection.RotateAboutZ(GetMonsterAzimuth() + Atan(0.33F / d) + Math::RandomFloat(0.1F) - 0.05F);

					const Model *model = GetTargetNode();
					const Transform4D& transform = model->GetWorldTransform();
					int32 projectileIndex = model->GetWorld()->NewControllerIndex();
					TheMessageMgr->SendMessageAll(CreateBlackCatMessage(projectileIndex, GetControllerIndex(), transform.GetTranslation() + transform[2] - transform[1] * 0.5F, blackCatDirection));
				}
			}

			break;

		case 'FTHR':

			if (TheMessageMgr->Server())
			{
				Vector3D	direction;

				if (GetEnemyDirection(&direction))
				{
					float dx = Magnitude(direction.GetVector2D());
					float dz = direction.z + 1.0F;
					float g = -GetPhysicsController()->GetGravityAcceleration().z;

					float vx = 2.0F;
					float vz = vx * dz / dx + g * 0.5F * dx / vx;

					if (vz > 1.0F)
					{
						vz = 1.0F;
						vx = dx * g / (vz + Sqrt(vz * vz - dz * g * 2.0F));
					}

					Vector3D blackCatDirection(vx, 0.0F, vz);
					blackCatDirection.RotateAboutZ(Atan(direction.y, direction.x));

					const Model *model = GetTargetNode();
					const Transform4D& transform = model->GetWorldTransform();
					int32 projectileIndex = model->GetWorld()->NewControllerIndex();
					TheMessageMgr->SendMessageAll(CreateBlackCatMessage(projectileIndex, GetControllerIndex(), transform.GetTranslation() + transform[0] + transform[1] * 0.5F, blackCatDirection));
				}
			}

			break;
	}
}

void WitchController::SetWitchMotion(int32 motion)
{
	SetMonsterMotion(motion);
	switch (motion)
	{
		case kWitchMotionStand:

			GetAnimationBlender()->BlendAnimation("witch/Stand", kInterpolatorForward | kInterpolatorLoop, 0.004F);
			break;

		case kWitchMotionFly:

			GetAnimationBlender()->StartAnimation("witch/Fly", kInterpolatorForward | kInterpolatorLoop);
			break;

		case kWitchMotionChase:

			GetAnimationBlender()->BlendAnimation("witch/Chase", kInterpolatorForward | kInterpolatorLoop, 0.004F);
			break;

		case kWitchMotionAttack:
		{
			FrameAnimator *animator = GetAnimationBlender()->BlendAnimation("witch/Attack", kInterpolatorForward, 0.004F, &AttackCallback, this);
			animator->GetFrameInterpolator()->SetRate(1.5F);

			PlaySource("witch/Attack1", 32.0F);
			break;
		}

		case kWitchMotionThrow:
		{
			FrameAnimator *animator = GetAnimationBlender()->BlendAnimation("witch/Throw", kInterpolatorForward, 0.004F, &ThrowCallback, this);
			animator->GetFrameInterpolator()->SetRate(2.0F);
			break;
		}

		case kWitchMotionFlyThrow:
		{
			FrameAnimator *animator = GetAnimationBlender()->BlendAnimation("witch/FlyThrow", kInterpolatorForward, 0.004F, &FlyThrowCallback, this);
			animator->GetFrameInterpolator()->SetRate(2.0F);
			break;
		}

		case kWitchMotionDamage:
		{
			static const char damageName[2][16] =
			{
				"witch/Damage1", "witch/Damage2"
			};

			GetAnimationBlender()->BlendAnimation(damageName[Math::Random(2)], kInterpolatorForward, 0.004F, &DamageCallback, this);
			break;
		}

		case kWitchMotionDeath:
		{
			FrameAnimator *animator = GetAnimationBlender()->BlendAnimation("witch/Death", kInterpolatorForward, 0.004F);
			Interpolator *interpolator = animator->GetFrameInterpolator();
			interpolator->SetRange(330.0F, animator->GetAnimationDuration());
			interpolator->SetValue(330.0F);

			if (witchSource)
			{
				witchSource->Stop();
			}

			PlaySource("witch/Death", 64.0F);

			SetGravityMultiplier(1.0F);
			SetCharacterState(GetCharacterState() & ~kCharacterFlying);
			targetRoll = 0.0F;

			if (witchFlags & kWitchFlying)
			{
				if (broomSmokeParticleSystem)
				{
					broomSmokeParticleSystem->Finalize();
					broomSmokeParticleSystem = nullptr;
				}

				if (magicBroomParticleSystem)
				{
					magicBroomParticleSystem->Finalize();
					magicBroomParticleSystem = nullptr;
				}
			}
			else
			{
				blackCatModel->Disable();
			}

			break;
		}
	}
}

void WitchController::AttackCallback(Interpolator *interpolator, void *cookie)
{
	WitchController *witchController = static_cast<WitchController *>(cookie);
	witchController->SetMonsterState(kWitchStateStanding);
	witchController->SetWitchMotion(kWitchMotionStand);
	witchController->waitTime = Math::Random(300) + 200;
}

void WitchController::ThrowCallback(Interpolator *interpolator, void *cookie)
{
	WitchController *witchController = static_cast<WitchController *>(cookie);
	witchController->SetMonsterState(kWitchStateStanding);
	witchController->SetWitchMotion(kWitchMotionStand);
	witchController->waitTime = 250;
}

void WitchController::FlyThrowCallback(Interpolator *interpolator, void *cookie)
{
	WitchController *witchController = static_cast<WitchController *>(cookie);
	witchController->SetMonsterState(kWitchStateFlying);
	witchController->SetWitchMotion(kWitchMotionFly);
}

void WitchController::DamageCallback(Interpolator *interpolator, void *cookie)
{
	WitchController *witchController = static_cast<WitchController *>(cookie);
	witchController->SetMonsterState(kWitchStateStanding);
	witchController->SetWitchMotion(kWitchMotionStand);
	witchController->waitTime = Math::Random(200);
}

CharacterStatus WitchController::Damage(Fixed damage, unsigned_int32 flags, GameCharacterController *attacker, const Point3D *position, const Vector3D *force)
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
			BloodParticleSystem *blood = new BloodParticleSystem(ColorRGB(0.5F, 0.0F, 0.0F), particleCount);
			blood->SetNodePosition(*position);
			model->GetWorld()->AddNewNode(blood);
		}
	}

	if (DamageTimeExpired(200))
	{
		//OmniSource *source = PlaySource("witch/Damage", 32.0F);
		//source->SetSourceFrequency(Math::RandomFloat(0.2F) + 0.8F);
	}

	CharacterStatus status = MonsterController::Damage(damage, flags, attacker, position, force);
	if ((status == kCharacterDamaged) && (GetMonsterState() == kWitchStateChasing))
	{
		if (Math::Random(2) == 0)
		{
			SetMonsterState(kWitchStateRecovering);
			SetWitchMotion(kWitchMotionDamage);
		}
	}

	return (status);
}

void WitchController::Kill(GameCharacterController *attacker, const Point3D *position, const Vector3D *force)
{
	MonsterController::Kill(attacker, position, force);

	UncountMonster(globalWitchCount);
	SetMonsterState(kWitchStateDead);

	SetWitchMotion(kWitchMotionDeath);
	SetExternalForce(Zero3D);
}


GenerateWitchMethod::GenerateWitchMethod() : GenerateMonsterMethod(kMethodGenerateWitch)
{
}

GenerateWitchMethod::GenerateWitchMethod(const GenerateWitchMethod& generateWitchMethod) :
		GenerateMonsterMethod(generateWitchMethod),
		WitchConfiguration(generateWitchMethod)
{
}

GenerateWitchMethod::~GenerateWitchMethod()
{
}

Method *GenerateWitchMethod::Replicate(void) const
{
	return (new GenerateWitchMethod(*this));
}

void GenerateWitchMethod::Pack(Packer& data, unsigned_int32 packFlags) const
{
	GenerateMonsterMethod::Pack(data, packFlags);
	WitchConfiguration::Pack(data);
	data << TerminatorChunk;
}

void GenerateWitchMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	GenerateMonsterMethod::Unpack(data, unpackFlags);
	UnpackChunkList<GenerateWitchMethod>(data, unpackFlags);
}

bool GenerateWitchMethod::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	return (WitchConfiguration::UnpackChunk(chunkHeader, data));
}

int32 GenerateWitchMethod::GetSettingCount(void) const
{
	return (WitchConfiguration::GetSettingCount());
}

Setting *GenerateWitchMethod::GetSetting(int32 index) const
{
	return (WitchConfiguration::GetSetting(index));
}

void GenerateWitchMethod::SetSetting(const Setting *setting)
{
	WitchConfiguration::SetSetting(setting);
}

void GenerateWitchMethod::Execute(const ScriptState *state)
{
	Node *node = GetTargetNode(state);
	if (node)
	{
		GamePlayer *player = static_cast<GamePlayer *>(TheMessageMgr->GetLocalPlayer());
		if (player->GetPlayerController())
		{
			Model *model = Model::Get(kModelWitch);
			WitchController *controller = new WitchController(*this);
			GenerateMonster(node, model, controller);
		}
	}

	CallCompletionProc();
}


GetWitchCountMethod::GetWitchCountMethod() : Method(kMethodGetWitchCount)
{
}

GetWitchCountMethod::GetWitchCountMethod(const GetWitchCountMethod& getWitchCountMethod) : Method(getWitchCountMethod)
{
}

GetWitchCountMethod::~GetWitchCountMethod()
{
}

Method *GetWitchCountMethod::Replicate(void) const
{
	return (new GetWitchCountMethod(*this));
}

void GetWitchCountMethod::Execute(const ScriptState *state)
{
	SetOutputValue(state, WitchController::GetGlobalWitchCount());
	CallCompletionProc();
}

// ZYUQURM
