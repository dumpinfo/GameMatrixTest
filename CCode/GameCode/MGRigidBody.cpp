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


#include "MGRigidBody.h"
#include "MGGrenadeLauncher.h"
#include "MGGame.h"


using namespace C4;


namespace
{
	const float kExplosiveSplashDamageRadius = 5.0F;
}


const GameRigidBodyController::GameRigidBodyType GameRigidBodyController::gameRigidBodyTypeTable[kGameRigidBodyTypeCount] =
{
	kGameRigidBodyPumpkin, kGameRigidBodyWatermelon, kGameRigidBodyCardboardBox, kGameRigidBodyPropaneTank, kGameRigidBodyHornetNest
};


GameRigidBodyController::GameRigidBodyController() : RigidBodyController(kControllerGameRigidBody)
{
	SetRigidBodyType(kRigidBodyGame);
	gameRigidBodyType = kGameRigidBodyNone;
	gameRigidBodyState = 0;
}

GameRigidBodyController::GameRigidBodyController(ControllerType type) : RigidBodyController(type)
{
	SetRigidBodyType(kRigidBodyGame);
	gameRigidBodyType = kGameRigidBodyNone;
	gameRigidBodyState = 0;
}

GameRigidBodyController::GameRigidBodyController(const GameRigidBodyController& gameRigidBodyController) : RigidBodyController(gameRigidBodyController)
{
	gameRigidBodyType = gameRigidBodyController.gameRigidBodyType;
	gameRigidBodyState = 0;
}

GameRigidBodyController::~GameRigidBodyController()
{
}

Controller *GameRigidBodyController::Replicate(void) const
{
	return (new GameRigidBodyController(*this));
}

void GameRigidBodyController::RegisterFunctions(ControllerRegistration *registration)
{
	const StringTable *table = TheGame->GetStringTable();

	static FunctionReg<BreakApartFunction> breakApartRegistration(registration, kFunctionBreakApart, table->GetString(StringID('CTRL', kControllerGameRigidBody, kFunctionBreakApart)));
}

void GameRigidBodyController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	RigidBodyController::Pack(data, packFlags);

	data << ChunkHeader('TYPE', 4);
	data << gameRigidBodyType;

	data << TerminatorChunk;
}

void GameRigidBodyController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	RigidBodyController::Unpack(data, unpackFlags);
	UnpackChunkList<GameRigidBodyController>(data, unpackFlags);
}

bool GameRigidBodyController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'TYPE':

			data >> gameRigidBodyType;
			return (true);
	}

	return (false);
}

int32 GameRigidBodyController::GetSettingCount(void) const
{
	return (RigidBodyController::GetSettingCount() + 3);
}

Setting *GameRigidBodyController::GetSetting(int32 index) const
{
	int32 count = RigidBodyController::GetSettingCount();
	if (index < count)
	{
		return (RigidBodyController::GetSetting(index));
	}

	const StringTable *table = TheGame->GetStringTable();

	if (index == count) 
	{
		const char *title = table->GetString(StringID('CTRL', kControllerGameRigidBody, 'GAME'));
		return (new HeadingSetting('GAME', title)); 
	}
 
	if (index == count + 1)
	{
		int32 selection = 0; 
		for (machine a = 0; a < kGameRigidBodyTypeCount; a++)
		{ 
			if (gameRigidBodyType == gameRigidBodyTypeTable[a]) 
			{
				selection = a + 1;
				break;
			} 
		}

		const char *title = table->GetString(StringID('CTRL', kControllerGameRigidBody, 'GTYP'));
		MenuSetting *menu = new MenuSetting('GTYP', selection, title, kGameRigidBodyTypeCount + 1);
		menu->SetMenuItemString(0, table->GetString(StringID('CTRL', kControllerGameRigidBody, 'GTYP', 'NONE')));
		for (machine a = 1; a <= kGameRigidBodyTypeCount; a++)
		{
			menu->SetMenuItemString(a, table->GetString(StringID('CTRL', kControllerGameRigidBody, 'GTYP', gameRigidBodyTypeTable[a - 1])));
		}

		return (menu);
	}

	if (index == count + 2)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerGameRigidBody, 'IMMV'));
		return (new BooleanSetting('IMMV', ((GetRigidBodyFlags() & kRigidBodyImmovable) != 0), title));
	}

	return (nullptr);
}

void GameRigidBodyController::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'GTYP')
	{
		int32 selection = static_cast<const MenuSetting *>(setting)->GetMenuSelection();
		if (selection == 0)
		{
			gameRigidBodyType = kGameRigidBodyNone;
		}
		else
		{
			gameRigidBodyType = gameRigidBodyTypeTable[selection - 1];
		}
	}
	else if (identifier == 'IMMV')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			SetRigidBodyFlags(GetRigidBodyFlags() | kRigidBodyImmovable);
		}
		else
		{
			SetRigidBodyFlags(GetRigidBodyFlags() & ~kRigidBodyImmovable);
		}
	}
	else
	{
		return (RigidBodyController::SetSetting(setting));
	}
}

ControllerMessage *GameRigidBodyController::CreateMessage(ControllerMessageType type) const
{
	switch (type)
	{
		case kGameRigidBodyMessageBreakApart:

			return (new BreakApartMessage(GetControllerIndex()));

		case kGameRigidBodyMessageLeakGoo:

			return (new LeakGooMessage(GetControllerIndex()));
	}

	return (RigidBodyController::CreateMessage(type));
}

void GameRigidBodyController::BreakApart(unsigned_int32 flags, Node *initiator, Node *trigger)
{
	unsigned_int32 state = gameRigidBodyState;
	if (!(state & kGameRigidBodyBreaking))
	{
		gameRigidBodyState = state | kGameRigidBodyBreaking;

		const Node *target = GetTargetNode();
		const Property *property = target->GetSharedProperty(kPropertyBreakable);
		if (property)
		{
			const BreakableProperty *breakableProperty = static_cast<const BreakableProperty *>(property);

			const Node *node = target->GetConnectedNode(breakableProperty->GetActivationConnectorKey());
			if (node)
			{
				Controller *controller = node->GetController();
				if (controller)
				{
					controller->Activate(initiator, trigger);
				}
			}

			const Node *group = target->GetFirstSubnode();
			while (group)
			{
				if (group->GetNodeType() == kNodeGeneric)
				{
					BreakApartMessage message(GetControllerIndex(), flags);

					World *world = target->GetWorld();
					const Vector3D& velocity = GetLinearVelocity();
					unsigned_int32 breakableFlags = breakableProperty->GetBreakableFlags();

					unsigned_int32 detailLevel = TheGame->GetGameDetailLevel();

					int32 index = 0;
					int32 count = 0;
					BreakApartMessage::PieceData *piece = message.GetPieceDataArray();

					node = group->GetFirstSubnode();
					while (node)
					{
						if ((breakableFlags & BreakableProperty::kBreakableKeepAll) || (Math::Random(3) > detailLevel))
						{
							piece->pieceIndex = index;
							piece->controllerIndex = world->NewControllerIndex();
							piece->initialVelocity = velocity + Math::RandomUnitVector3D() * 5.0F;
							piece->initialVelocity.z = Fabs(piece->initialVelocity.z);

							if (++count == BreakApartMessage::kMaxPieceCount)
							{
								break;
							}

							piece++;
						}

						index++;
						node = node->Next();
					}

					if (count != 0)
					{
						message.SetPieceCount(count);
						TheMessageMgr->SendMessageAll(message);
					}

					break;
				}

				group = group->Next();
			}
		}
	}
}

void GameRigidBodyController::HandleSplashDamage(const Point3D& center, float radius, float damage, GameCharacterController *attacker)
{
}


BreakApartFunction::BreakApartFunction() : Function(kFunctionBreakApart, kControllerGameRigidBody)
{
}

BreakApartFunction::BreakApartFunction(const BreakApartFunction& breakApartFunction) : Function(breakApartFunction)
{
}

BreakApartFunction::~BreakApartFunction()
{
}

Function *BreakApartFunction::Replicate(void) const
{
	return (new BreakApartFunction(*this));
}

void BreakApartFunction::Execute(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	static_cast<GameRigidBodyController *>(controller)->BreakApart(0, state->GetInitiatorNode(), state->GetTriggerNode());
}


BreakApartMessage::BreakApartMessage(int32 index) : ControllerMessage(GameRigidBodyController::kGameRigidBodyMessageBreakApart, index)
{
}

BreakApartMessage::BreakApartMessage(int32 index, unsigned_int32 flags) : ControllerMessage(GameRigidBodyController::kGameRigidBodyMessageBreakApart, index)
{
	breakApartFlags = flags;
}

BreakApartMessage::~BreakApartMessage()
{
}

void BreakApartMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << (unsigned_int8) breakApartFlags;

	int32 count = pieceCount;
	data << (unsigned_int8) count;

	const PieceData *piece = pieceData;
	for (machine a = 0; a < count; a++)
	{
		data << (unsigned_int8) piece->pieceIndex;
		data << (unsigned_int16) piece->controllerIndex;
		data << piece->initialVelocity.x;
		data << piece->initialVelocity.y;
		data << piece->initialVelocity.z;

		piece++;
	}
}

bool BreakApartMessage::Decompress(Decompressor& data)
{
	if (ControllerMessage::Decompress(data))
	{
		unsigned_int8	flags, count;

		data >> flags;
		breakApartFlags = flags;

		data >> count;
		if (count < kMaxPieceCount)
		{
			pieceCount = count;

			PieceData *piece = pieceData;
			for (machine a = 0; a < count; a++)
			{
				unsigned_int8	pieceIndex;
				unsigned_int16	controllerIndex;

				data >> pieceIndex;
				piece->pieceIndex = pieceIndex;

				data >> controllerIndex;
				piece->controllerIndex = controllerIndex;

				data >> piece->initialVelocity.x;
				data >> piece->initialVelocity.y;
				data >> piece->initialVelocity.z;

				piece++;
			}

			return (true);
		}
	}

	return (false);
}

void BreakApartMessage::HandleControllerMessage(Controller *controller) const
{
	Node *target = controller->GetTargetNode();
	const Property *property = target->GetSharedProperty(kPropertyBreakable);
	if (property)
	{
		const BreakableProperty *breakableProperty = static_cast<const BreakableProperty *>(property);
		Node *super = target->GetSuperNode();

		const Node *group = target->GetFirstSubnode();
		while (group)
		{
			if (group->GetNodeType() == kNodeGeneric)
			{
				unsigned_int32 flags = breakableProperty->GetBreakableFlags();
				unsigned_int32 rigidBodyFlags = (flags & BreakableProperty::kBreakableForceFieldInhibit) ? kRigidBodyForceFieldInhibit : 0;
				unsigned_int32 collisionExclusionMask = breakableProperty->GetCollisionExclusionMask();

				const RigidBodyController *brokenBody = static_cast<RigidBodyController *>(controller);
				float frictionCoefficient = brokenBody->GetFrictionCoefficient();
				float spinFrictionMultiplier = brokenBody->GetSpinFrictionMultiplier();
				float rollingResistance = brokenBody->GetRollingResistance();
				float sleepBoxMultiplier = brokenBody->GetSleepBoxMultiplier() * 2.0F;

				int32 index = 0;
				int32 count = pieceCount;
				const PieceData *piece = pieceData;

				Node *node = group->GetFirstSubnode();
				while (node)
				{
					Node *next = node->Next();

					if (piece->pieceIndex == index)
					{
						node->Enable();
						node->SetNodeTransform(target->GetNodeTransform() * group->GetNodeTransform() * node->GetNodeTransform());
						super->AppendSubnode(node);
						node->Invalidate();

						const Node *subnode = node->GetFirstSubnode();
						while (subnode)
						{
							if (subnode->GetNodeType() == kNodeShape)
							{
								RigidBodyController *rigidBody = new RigidBodyController;
								rigidBody->SetControllerIndex(piece->controllerIndex);
								rigidBody->SetRigidBodyFlags(rigidBodyFlags);
								rigidBody->SetCollisionExclusionMask(collisionExclusionMask);
								rigidBody->SetFrictionCoefficient(frictionCoefficient);
								rigidBody->SetSpinFrictionMultiplier(spinFrictionMultiplier);
								rigidBody->SetRollingResistance(rollingResistance);
								rigidBody->SetSleepBoxMultiplier(sleepBoxMultiplier);
								node->SetController(rigidBody);

								rigidBody->Preprocess();
								rigidBody->SetLinearVelocity(piece->initialVelocity);
								rigidBody->SetAngularVelocity(Math::RandomUnitVector3D() * Math::RandomFloat(5.0F));

								node->EstablishVisibility();
								break;
							}

							subnode = subnode->Next();
						}

						if (--count == 0)
						{
							break;
						}

						piece++;
					}

					index++;
					node = next;
				}

				break;
			}

			group = group->Next();
		}

		const RigidBodyController *rigidBody = static_cast<RigidBodyController *>(controller);

		if (!(breakApartFlags & kBreakApartSoundInhibit))
		{
			const ResourceName& soundName = breakableProperty->GetBreakableSoundName();
			if (soundName[0] != 0)
			{
				OmniSource *source = new OmniSource(soundName, 32.0F);
				source->SetNodePosition(rigidBody->GetWorldCenterOfMass());
				super->AppendNewSubnode(source);
			}
		}

		property = target->GetProperty(kPropertyLeakingGoo);
		if (property)
		{
			const LeakingGooProperty *leakingGooProperty = static_cast<const LeakingGooProperty *>(property);

			BurstingGooParticleSystem *burstingGoo = new BurstingGooParticleSystem(rigidBody->GetLinearVelocity());
			burstingGoo->SetMaterialObject(AutoReleaseMaterial(leakingGooProperty->GetMaterialType()));

			burstingGoo->SetNodePosition(rigidBody->GetWorldCenterOfMass());
			super->AppendNewSubnode(burstingGoo);
		}

		delete target;
	}
}


LeakGooMessage::LeakGooMessage(int32 index) : ControllerMessage(GameRigidBodyController::kGameRigidBodyMessageLeakGoo, index)
{
}

LeakGooMessage::LeakGooMessage(int32 index, const Point3D& position, const Vector3D& normal) : ControllerMessage(GameRigidBodyController::kGameRigidBodyMessageLeakGoo, index)
{
	leakPosition = position;
	leakNormal = normal;
}

LeakGooMessage::~LeakGooMessage()
{
}

void LeakGooMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << leakPosition;
	data << leakNormal;
}

bool LeakGooMessage::Decompress(Decompressor& data)
{
	if (ControllerMessage::Decompress(data))
	{
		data >> leakPosition;
		data >> leakNormal;
		return (true);
	}

	return (false);
}

void LeakGooMessage::HandleControllerMessage(Controller *controller) const
{
	Node *target = controller->GetTargetNode();

	const LeakingGooProperty *property = static_cast<LeakingGooProperty *>(target->GetProperty(kPropertyLeakingGoo));

	LeakingGooParticleSystem *leakingGoo = new LeakingGooParticleSystem(static_cast<RigidBodyController *>(controller), 0.03F, 0.0015F, 5000.0F);
	leakingGoo->SetMaterialObject(AutoReleaseMaterial(property->GetMaterialType()));

	Vector3D tangent = Math::CreateUnitPerpendicular(leakNormal);
	leakingGoo->SetNodeTransform(tangent, leakNormal % tangent, leakNormal, leakPosition);
	target->AppendNewSubnode(leakingGoo);

	OmniSource *source = new OmniSource("sound/GooLeak", 20.0F);
	source->SetNodePosition(leakPosition);
	target->AppendNewSubnode(source);
}


RemainsController::RemainsController() : RigidBodyController(kControllerRemains)
{
}

RemainsController::RemainsController(int32 life) : RigidBodyController(kControllerRemains)
{
	lifeTime = life;
}

RemainsController::RemainsController(ControllerType type, int32 life) : RigidBodyController(type)
{
	lifeTime = life;
}

RemainsController::~RemainsController()
{
}

void RemainsController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	RigidBodyController::Pack(data, packFlags);

	data << ChunkHeader('LIFE', 4);
	data << lifeTime;

	data << TerminatorChunk;
}

void RemainsController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	RigidBodyController::Unpack(data, unpackFlags);
	UnpackChunkList<RemainsController>(data, unpackFlags);
}

bool RemainsController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'LIFE':

			data >> lifeTime;
			return (true);
	}

	return (false);
}

void RemainsController::Preprocess(void)
{
	RigidBodyController::Preprocess();

	SetRigidBodyFlags(kRigidBodyPartialSleep | kRigidBodyLocalSimulation);
	SetCollisionKind(kCollisionRemains);
	SetCollisionExclusionMask(kCollisionCharacter | kCollisionProjectile | kCollisionCorpse | kCollisionRemains);
	SetRollingResistance(2.0F);
	SetSleepBoxMultiplier(2.0F);

	parameterValue.Set(-1.0F, -1.0F, -1.0F, -1.0F);

	Node *target = GetTargetNode();
	Node *node = target;
	do
	{
		if (node->GetNodeType() == kNodeGeometry)
		{
			Geometry *geometry = static_cast<Geometry *>(node);
			geometry->SetShaderParameterProc(&GetShaderParameterData, this);
		}

		node = target->GetNextNode(node);
	} while (node);
}

void RemainsController::Move(void)
{
	int32 time = (lifeTime -= TheTimeMgr->GetDeltaTime());
	if (time <= 0)
	{
		delete GetTargetNode();
	}
	else if (time < 2000)
	{
		float t = 1.0F - (float) time * 0.0005F;
		parameterValue.Set(t, t, t, t);
	}
}

const float *RemainsController::GetShaderParameterData(int32 slot, void *cookie)
{
	if (slot == 0)
	{
		RemainsController *controller = static_cast<RemainsController *>(cookie);
		return (&controller->parameterValue.x);
	}

	return (nullptr);
}


ExplosiveController::ExplosiveController() : GameRigidBodyController(kControllerExplosive)
{
	explosiveHealth = 5 << 16;
	explodeDamage = 50;
	explodeTime = 0;

	shakeIntensity = 0.1F;
	shakeDuration = 1000;
}

ExplosiveController::ExplosiveController(const ExplosiveController& explosiveController) : GameRigidBodyController(explosiveController)
{
	explosiveHealth = explosiveController.explosiveHealth;
	explodeDamage = explosiveController.explodeDamage;
	explodeTime = 0;

	shakeIntensity = explosiveController.shakeIntensity;
	shakeDuration = explosiveController.shakeDuration;
}

ExplosiveController::~ExplosiveController()
{
}

Controller *ExplosiveController::Replicate(void) const
{
	return (new ExplosiveController(*this));
}

void ExplosiveController::RegisterFunctions(ControllerRegistration *registration)
{
	const StringTable *table = TheGame->GetStringTable();

	static FunctionReg<ExplodeFunction> explodeRegistration(registration, kFunctionExplode, table->GetString(StringID('CTRL', kControllerExplosive, kFunctionExplode)));
}

void ExplosiveController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	GameRigidBodyController::Pack(data, packFlags);

	data << ChunkHeader('hlth', 4);
	data << explosiveHealth;

	data << ChunkHeader('damg', 4);
	data << explodeDamage;

	if (explodeTime > 0)
	{
		data << ChunkHeader('time', 4);
		data << explodeTime;
	}

	data << ChunkHeader('shak', 8);
	data << shakeIntensity;
	data << shakeDuration;

	data << TerminatorChunk;
}

void ExplosiveController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	GameRigidBodyController::Unpack(data, unpackFlags);
	UnpackChunkList<ExplosiveController>(data, unpackFlags);
}

bool ExplosiveController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'hlth':

			data >> explosiveHealth;
			return (true);

		case 'damg':

			data >> explodeDamage;
			return (true);

		case 'time':

			data >> explodeTime;
			return (true);

		case 'shak':

			data >> shakeIntensity;
			data >> shakeDuration;
			return (true);
	}

	return (false);
}

int32 ExplosiveController::GetSettingCount(void) const
{
	return (GameRigidBodyController::GetSettingCount() + 5);
}

Setting *ExplosiveController::GetSetting(int32 index) const
{
	int32 count = GameRigidBodyController::GetSettingCount();
	if (index < count)
	{
		return (GameRigidBodyController::GetSetting(index));
	}

	const StringTable *table = TheGame->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerExplosive, 'SETT'));
		return (new HeadingSetting(kControllerExplosive, title));
	}

	if (index == count + 1)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerExplosive, 'HLTH'));
		return (new TextSetting('HLTH', Text::IntegerToString(explosiveHealth >> 16), title, 4, &EditTextWidget::NumberFilter));
	}

	if (index == count + 2)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerExplosive, 'DAMG'));
		return (new TextSetting('DAMG', Text::IntegerToString(explodeDamage), title, 4, &EditTextWidget::NumberFilter));
	}

	if (index == count + 3)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerExplosive, 'SHKI'));
		return (new TextSetting('SHKI', shakeIntensity, title));
	}

	if (index == count + 4)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerExplosive, 'SHKD'));
		return (new TextSetting('SHKD', (float) shakeDuration * 0.001F, title));
	}

	return (nullptr);
}

void ExplosiveController::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'HLTH')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		explosiveHealth = Text::StringToInteger(text) << 16;
	}
	else if (identifier == 'DAMG')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		explodeDamage = Text::StringToInteger(text);
	}
	else if (identifier == 'SHKI')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		shakeIntensity = FmaxZero(Text::StringToFloat(text));
	}
	else if (identifier == 'SHKD')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		shakeDuration = MaxZero((int32) (Text::StringToFloat(text) * 1000.0F + 0.5F));
	}
	else
	{
		GameRigidBodyController::SetSetting(setting);
	}
}

ControllerMessage *ExplosiveController::CreateMessage(ControllerMessageType type) const
{
	switch (type)
	{
		case kExplosiveMessageExplode:

			return (new ControllerMessage(kExplosiveMessageExplode, GetControllerIndex(), kMessageDestroyer));
	}

	return (GameRigidBodyController::CreateMessage(type));
}

void ExplosiveController::ReceiveMessage(const ControllerMessage *message)
{
	if (message->GetControllerMessageType() == kExplosiveMessageExplode)
	{
		Node *node = GetTargetNode();

		if (TheMessageMgr->Synchronized())
		{
			MarkingData		markingData;

			World *world = node->GetWorld();
			const Point3D& position = GetWorldCenterOfMass();

			OmniSource *source = new OmniSource("grenade/grenade/Explode", 256.0F);
			OmniSourceObject *sourceObject = source->GetObject();
			sourceObject->SetReflectionVolume(0.5F);
			sourceObject->SetReflectionHFVolume(0.5F);
			source->SetSourcePriority(kSoundPriorityExplosion);
			source->SetNodePosition(position);
			world->AddNewNode(source);

			if (TheWorldMgr->GetLightDetailLevel() > 0)
			{
				ColorRGB color(4.0F, 2.0F, 0.4F);
				PointLight *light = new PointLight(color, 10.0F);
				light->GetObject()->SetLightFlags(kLightShadowInhibit);
				light->SetController(new FlashController(color, 0.5F, 1000));
				light->SetNodePosition(position);
				world->AddNewNode(light);
			}

			GrenadeExplosionParticleSystem *grenadeExplosion = new GrenadeExplosionParticleSystem(GetLinearVelocity());
			grenadeExplosion->SetNodePosition(position);
			world->AddNewNode(grenadeExplosion);

			GrenadeSparksParticleSystem *grenadeSparks = new GrenadeSparksParticleSystem(GetLinearVelocity());
			grenadeSparks->SetNodePosition(position);
			world->AddNewNode(grenadeSparks);

			ShockwaveEffect *shockwave = new ShockwaveEffect("effects/Shock", 16.0F, 8.0F, 0.03125F);
			shockwave->SetNodePosition(position);
			world->AddNewNode(shockwave);

			SphereField *field = new SphereField(Vector3D(6.0F, 6.0F, 6.0F));
			field->SetNodePosition(position);
			field->SetForce(new ExplosionForce(80.0F, kCollisionRemains | kCollisionShot));
			field->SetController(new ExplosionController(250));
			world->AddNewNode(field);

			markingData.center = position;
			markingData.normal.Set(0.0F, 0.0F, 1.0F);
			markingData.tangent.Set(CosSin(Math::RandomFloat(K::tau)), 0.0F);
			markingData.radius = 1.5F;
			markingData.textureName = "texture/Scorch";
			markingData.color.Set(0.0F, 0.0F, 0.0F, 1.0F);
			markingData.lifeTime = 30000;
			MarkingEffect::New(world, &markingData);

			int32 duration = shakeDuration;
			if (duration > 0)
			{
				static_cast<GameWorld *>(world)->ShakeCamera(position, shakeIntensity, duration);
			}
		}

		if (!node->GetSharedProperty(kPropertyBreakable))
		{
			delete node;
		}
		else if (TheMessageMgr->Server())
		{
			BreakApart(kBreakApartSoundInhibit);
		}
	}
	else
	{
		GameRigidBodyController::ReceiveMessage(message);
	}
}

void ExplosiveController::Move(void)
{
	int32 time = explodeTime;
	if (time > 0)
	{
		time -= TheTimeMgr->GetDeltaTime();
		if (time <= 0)
		{
			Explode();
		}
		else
		{
			explodeTime = time;
		}
	}
}

void ExplosiveController::HandleSplashDamage(const Point3D& center, float radius, float damage, GameCharacterController *attacker)
{
	float d2 = SquaredMag(GetWorldCenterOfMass() - center);
	float r2 = radius * radius;
	if (d2 < r2)
	{
		Damage((Fixed) (damage - damage * (d2 / r2)), d2, attacker);
	}
}

void ExplosiveController::Damage(Fixed damage, float squaredDistance, GameCharacterController *attacker)
{
	if ((explosiveHealth -= damage) <= 0)
	{
		attackerController = attacker;

		int32 t = Min((int32) (squaredDistance * 20.0F), 500) + 1;

		int32 time = explodeTime;
		if (time > 0)
		{
			explodeTime = Min(time, t);
		}
		else
		{
			explodeTime = t;
		}

		if (Asleep())
		{
			ExplosiveController::Wake();
		}

		SetRigidBodyFlags(kRigidBodyPartialSleep);
	}
}

void ExplosiveController::Explode(void)
{
	GameWorld *world = static_cast<GameWorld *>(GetTargetNode()->GetWorld());
	world->ProduceSplashDamage(GetWorldCenterOfMass(), kExplosiveSplashDamageRadius, explodeDamage, attackerController, this);

	TheMessageMgr->SendMessageJournal(new ControllerMessage(kExplosiveMessageExplode, GetControllerIndex(), kMessageDestroyer));
}


ExplodeFunction::ExplodeFunction() : Function(kFunctionExplode, kControllerExplosive)
{
}

ExplodeFunction::ExplodeFunction(const ExplodeFunction& explodeFunction) : Function(explodeFunction)
{
}

ExplodeFunction::~ExplodeFunction()
{
}

Function *ExplodeFunction::Replicate(void) const
{
	return (new ExplodeFunction(*this));
}

void ExplodeFunction::Execute(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	GameCharacterController *attacker = nullptr;

	const Node *node = state->GetInitiatorNode();
	if (node)
	{
		Controller *initiator = node->GetController();
		if ((initiator) && (initiator->GetBaseControllerType() == kControllerRigidBody))
		{
			RigidBodyController *rigidBody = static_cast<RigidBodyController *>(initiator);
			if (rigidBody->GetRigidBodyType() == kRigidBodyCharacter)
			{
				attacker = static_cast<GameCharacterController *>(rigidBody);
			}
		}
	}

	ExplosiveController *explosiveController = static_cast<ExplosiveController *>(controller);
	explosiveController->attackerController = attacker;
	explosiveController->Explode();
}


CrumbleController::CrumbleController() : GameRigidBodyController(kControllerCrumble)
{
	crumbleHealth = 50 << 16;
}

CrumbleController::CrumbleController(const CrumbleController& crumbleController) : GameRigidBodyController(crumbleController)
{
	crumbleHealth = crumbleController.crumbleHealth;
}

CrumbleController::~CrumbleController()
{
}

Controller *CrumbleController::Replicate(void) const
{
	return (new CrumbleController(*this));
}

void CrumbleController::RegisterFunctions(ControllerRegistration *registration)
{
	const StringTable *table = TheGame->GetStringTable();

	static FunctionReg<CrumbleFunction> crumbleRegistration(registration, kFunctionCrumble, table->GetString(StringID('CTRL', kControllerCrumble, kFunctionCrumble)));
}

void CrumbleController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	GameRigidBodyController::Pack(data, packFlags);

	data << ChunkHeader('hlth', 4);
	data << crumbleHealth;

	data << TerminatorChunk;
}

void CrumbleController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	GameRigidBodyController::Unpack(data, unpackFlags);
	UnpackChunkList<CrumbleController>(data, unpackFlags);
}

bool CrumbleController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'hlth':

			data >> crumbleHealth;
			return (true);
	}

	return (false);
}

int32 CrumbleController::GetSettingCount(void) const
{
	return (GameRigidBodyController::GetSettingCount() + 2);
}

Setting *CrumbleController::GetSetting(int32 index) const
{
	int32 count = GameRigidBodyController::GetSettingCount();
	if (index < count)
	{
		return (GameRigidBodyController::GetSetting(index));
	}

	const StringTable *table = TheGame->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerCrumble, 'SETT'));
		return (new HeadingSetting(kControllerCrumble, title));
	}

	if (index == count + 1)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerCrumble, 'HLTH'));
		return (new TextSetting('HLTH', Text::IntegerToString(crumbleHealth >> 16), title, 4, &EditTextWidget::NumberFilter));
	}

	return (nullptr);
}

void CrumbleController::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'HLTH')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		crumbleHealth = Text::StringToInteger(text) << 16;
	}
	else
	{
		GameRigidBodyController::SetSetting(setting);
	}
}

void CrumbleController::HandleSplashDamage(const Point3D& center, float radius, float damage, GameCharacterController *attacker)
{
	float d2 = SquaredMag(GetWorldCenterOfMass() - center);
	float r2 = radius * radius;
	if (d2 < r2)
	{
		Fixed x = (Fixed) (damage - damage * (d2 / r2));
		if ((crumbleHealth -= x) <= 0)
		{
			BreakApart(0, attacker ? attacker->GetTargetNode() : nullptr, nullptr);
		}
	}
}


CrumbleFunction::CrumbleFunction() : Function(kFunctionCrumble, kControllerCrumble)
{
}

CrumbleFunction::CrumbleFunction(const CrumbleFunction& crumbleFunction) : Function(crumbleFunction)
{
}

CrumbleFunction::~CrumbleFunction()
{
}

Function *CrumbleFunction::Replicate(void) const
{
	return (new CrumbleFunction(*this));
}

void CrumbleFunction::Execute(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	static_cast<CrumbleController *>(controller)->BreakApart(0, state->GetInitiatorNode(), state->GetTriggerNode());
}

// ZYUQURM
