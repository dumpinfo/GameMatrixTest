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


#include "MGControllers.h"
#include "MGGame.h"


using namespace C4;


int32 SecretScriptController::totalSecretCount = 0;
int32 SecretScriptController::foundSecretCount = 0;

const Type SecretScriptController::secretMessageType[kSecretMessageCount] =
{
	0, 'MINI', 'AREA', 'ROOM', 'PASS', 'TRES', 'ITEM'
};


SecretScriptController::SecretScriptController() : ScriptController(kControllerSecretScript)
{
	secretState = 0;
	messageType = 0;
}

SecretScriptController::SecretScriptController(const SecretScriptController& secretScriptController) : ScriptController(secretScriptController)
{
	secretState = secretScriptController.secretState;
	messageType = secretScriptController.messageType;
}

SecretScriptController::~SecretScriptController()
{
	unsigned_int32 state = secretState;
	if (state & kSecretCounted)
	{
		totalSecretCount--;
		if (state & kSecretFound)
		{
			foundSecretCount--;
		}
	}
}

Controller *SecretScriptController::Replicate(void) const
{
	return (new SecretScriptController(*this));
}

void SecretScriptController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	ScriptController::Pack(data, packFlags);

	data << ChunkHeader('STAT', 4);
	data << unsigned_int32(secretState & ~kSecretCounted);

	data << ChunkHeader('MESS', 4);
	data << messageType;

	data << TerminatorChunk;
}

void SecretScriptController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	ScriptController::Unpack(data, unpackFlags);
	UnpackChunkList<SecretScriptController>(data, unpackFlags);
}

bool SecretScriptController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'STAT':

			data >> secretState;
			return (true);

		case 'MESS':

			data >> messageType;
			return (true);
	}

	return (false);
}

int32 SecretScriptController::GetSettingCount(void) const
{
	return (ScriptController::GetSettingCount() + 2);
}

Setting *SecretScriptController::GetSetting(int32 index) const
{
	int32 count = ScriptController::GetSettingCount();
	if (index < count)
	{
		return (ScriptController::GetSetting(index));
	}

	const StringTable *table = TheGame->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerSecretScript, 'SETT')); 
		return (new HeadingSetting(kControllerSecretScript, title));
	}
 
	if (index == count + 1)
	{ 
		machine selection = 0;
		for (machine a = 1; a < kSecretMessageCount; a++)
		{ 
			if (messageType == secretMessageType[a])
			{ 
				selection = a; 
				break;
			}
		}
 
		const char *title = table->GetString(StringID('CTRL', kControllerSecretScript, 'MESS'));
		MenuSetting *menu = new MenuSetting('MESS', selection, title, kSecretMessageCount);

		menu->SetMenuItemString(0, table->GetString(StringID('CTRL', kControllerSecretScript, 'MESS', 'NONE')));
		for (machine a = 1; a < kSecretMessageCount; a++)
		{
			menu->SetMenuItemString(a, table->GetString(StringID('CTRL', kControllerSecretScript, 'MESS', secretMessageType[a])));
		}

		return (menu);
	}

	return (nullptr);
}

void SecretScriptController::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'MESS')
	{
		int32 selection = static_cast<const MenuSetting *>(setting)->GetMenuSelection();
		messageType = secretMessageType[selection];
	}
	else
	{
		ScriptController::SetSetting(setting);
	}
}

void SecretScriptController::Preprocess(void)
{
	unsigned_int32 state = secretState;
	if (!(state & kSecretCounted))
	{
		secretState = state | kSecretCounted;

		totalSecretCount++;
		if (state & kSecretFound)
		{
			foundSecretCount++;
		}
	}

	ScriptController::Preprocess();
}

void SecretScriptController::Activate(Node *initiator, Node *trigger)
{
	unsigned_int32 state = secretState;
	if (!(state & kSecretFound))
	{
		secretState = state | kSecretFound;
		foundSecretCount++;

		Sound *sound = new Sound;
		sound->Load("sound/Secret");
		sound->Play();

		if (messageType != 0)
		{
			TheSecretPopupBoard->SetMessage(TheGame->GetStringTable()->GetString(StringID('CTRL', kControllerSecretScript, 'MESS', messageType)));
		}
	}

	ScriptController::Activate(initiator, trigger);
}


CollectableController::CollectableController() : Controller(kControllerCollectable)
{
	collectableFlags = kCollectableDelete;

	triggerNode = nullptr;
	quadEffectNode = nullptr;
	spiralHelixNode = nullptr;

	respawnInterval = 12000;
	respawnTime = 0;

	phaseAngle = Math::RandomFloat(K::two_tau);
}

CollectableController::CollectableController(const CollectableController& collectableController) : Controller(collectableController)
{
	collectableFlags = collectableController.collectableFlags;

	triggerNode = nullptr;
	quadEffectNode = nullptr;
	spiralHelixNode = nullptr;

	respawnInterval = collectableController.respawnInterval;
	respawnTime = 0;

	phaseAngle = Math::RandomFloat(K::two_tau);
}

CollectableController::~CollectableController()
{
	delete triggerNode;
	delete quadEffectNode;
	delete spiralHelixNode;
}

Controller *CollectableController::Replicate(void) const
{
	return (new CollectableController(*this));
}

bool CollectableController::ValidNode(const Node *node)
{
	return (node->GetNodeType() == kNodeModel);
}

void CollectableController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Controller::Pack(data, packFlags);

	data << ChunkHeader('parm', 4);
	data << collectableFlags;

	data << ChunkHeader('rtim', 4);
	data << respawnInterval;

	data << ChunkHeader('stat', 8);
	data << respawnTime;
	data << phaseAngle;

	if (collectableFlags & kCollectableInitialized)
	{
		data << ChunkHeader('cent', sizeof(Point3D));
		data << centerPosition;
	}

	data << TerminatorChunk;
}

void CollectableController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Controller::Unpack(data, unpackFlags);
	UnpackChunkList<CollectableController>(data, unpackFlags);
}

bool CollectableController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'parm':

			data >> collectableFlags;
			return (true);

		case 'rtim':

			data >> respawnInterval;
			return (true);

		case 'stat':

			data >> respawnTime;
			data >> phaseAngle;
			return (true);

		case 'cent':

			data >> centerPosition;
			return (true);
	}

	return (false);
}

int32 CollectableController::GetSettingCount(void) const
{
	return (2);
}

Setting *CollectableController::GetSetting(int32 index) const
{
	const StringTable *table = TheGame->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerCollectable, 'RSPN'));
		return (new BooleanSetting('RSPN', ((collectableFlags & kCollectableDelete) == 0), title));
	}
	else if (index == 1)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerCollectable, 'RTIM'));
		return (new TextSetting('RTIM', (float) respawnInterval * 0.001F, title));
	}

	return (nullptr);
}

void CollectableController::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'RSPN')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			collectableFlags &= ~kCollectableDelete;
		}
		else
		{
			collectableFlags |= kCollectableDelete;
		}
	}
	else if (identifier == 'RTIM')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		respawnInterval = (int32) (Text::StringToFloat(text) * 1000.0F + 0.5F);
	}
}

void CollectableController::Preprocess(void)
{
	Controller::Preprocess();

	scriptController = nullptr;

	Model *model = GetTargetNode();
	if (!model->GetManipulator())
	{
		const CollectableProperty *collectableProperty = nullptr;

		const Node *node = model->GetFirstSubnode();
		while (node)
		{
			const Property *property = node->GetProperty(kPropertyCollectable);
			if (property)
			{
				collectableProperty = static_cast<const CollectableProperty *>(property);

				Controller *controller = node->GetController();
				if ((controller) && (controller->GetControllerType() == kControllerScript))
				{
					scriptController = static_cast<ScriptController *>(controller);
				}

				break;
			}

			node = node->Next();
		}

		if (collectableProperty)
		{
			collectableType = collectableProperty->GetCollectableType();
			respawnColor = collectableProperty->GetRespawnColor();
		}
		else
		{
			collectableType = CollectableProperty::kCollectableWeapon;
			respawnColor.Set(1.0F, 1.0F, 0.25F, 1.0F);
		}

		effectInterpolator.SetMaxValue(4.0F);

		unsigned_int32 flags = collectableFlags;
		if (!(flags & kCollectableInitialized))
		{
			collectableFlags = flags | kCollectableInitialized;
			centerPosition = model->GetNodePosition();
		}

		if (collectableType == CollectableProperty::kCollectableWeapon)
		{
			spiralHelixNode = new SpiralHelixParticleSystem(ColorRGB(1.0F, 0.875F, 0.25F), 0.2F, 0.001F);
			spiralHelixNode->SetNodePosition(Point3D(centerPosition.x, centerPosition.y, centerPosition.z - 0.5F));
			spiralHelixNode->SetNodeFlags(kNodeNonpersistent);
			model->GetSuperNode()->AppendSubnode(spiralHelixNode);
		}
		else
		{
			spiralHelixNode = nullptr;
		}

		if (TheMessageMgr->Server())
		{
			Trigger *trigger = new CylinderTrigger(Vector2D(0.3F, 0.3F), 1.0F);
			trigger->SetNodeFlags(kNodeNonpersistent | (model->GetNodeFlags() & kNodeDisabled));
			trigger->SetNodePosition(Point3D(centerPosition.x, centerPosition.y, centerPosition.z - 0.5F));
			trigger->AddConnector("Model", model);
			triggerNode = trigger;

			TriggerObject *object = trigger->GetObject();
			object->SetActivationConnectorKey("Model");
			object->SetTriggerFlags(0);

			model->GetSuperNode()->AppendSubnode(trigger);
		}
	}
}

ControllerMessage *CollectableController::CreateMessage(ControllerMessageType type) const
{
	switch (type)
	{
		case kCollectableMessageCollect:
		case kCollectableMessageRespawn:

			return (new ControllerMessage(type, GetControllerIndex()));

		case kCollectableMessageState:

			return (new CollectableStateMessage(GetControllerIndex()));
	}

	return (Controller::CreateMessage(type));
}

void CollectableController::ReceiveMessage(const ControllerMessage *message)
{
	Model *model = GetTargetNode();

	switch (message->GetControllerMessageType())
	{
		case kCollectableMessageCollect:
		{
			static const char soundName[CollectableProperty::kCollectableTypeCount][32] =
			{
				"sound/PickupWeapon", "sound/PickupAmmo", "sound/PickupHealth", "", "sound/PickupTreasure"
			};

			for (machine a = 0; a < CollectableProperty::kCollectableTypeCount; a++)
			{
				if (collectableType == CollectableProperty::collectableTypeTable[a])
				{
					const char *name = soundName[a];
					if (name[0] != 0)
					{
						OmniSource *source = new OmniSource(name, 32.0F);
						source->SetNodePosition(centerPosition);
						model->GetSuperNode()->AppendNewSubnode(source);
					}

					break;
				}
			}

			if (!(collectableFlags & kCollectableDelete))
			{
				model->Disable();

				Trigger *trigger = triggerNode;
				if (trigger)
				{
					trigger->Disable();
				}

				respawnTime = respawnInterval;
			}
			else
			{
				delete model;
			}

			break;
		}

		case kCollectableMessageRespawn:
		{
			QuadEffect *effect = new QuadEffect(1.5F, K::transparent, "effects/Warp");
			effect->SetNodePosition(centerPosition);
			model->GetSuperNode()->AppendNewSubnode(effect);

			effect->SetDistortionState();
			effect->SetRenderState(kRenderDepthTest | kRenderDepthInhibit | kRenderDepthOffset);
			effect->SetDepthOffset(1.0F, &effect->GetWorldPosition());
			effect->GetFirstRenderSegment()->SetMaterialState(kMaterialTwoSided);
			quadEffectNode = effect;

			effectInterpolator.Set(0.0F, 0.008F, kInterpolatorForward | kInterpolatorOscillate);
			respawnTime = 0;
			break;
		}

		case kCollectableMessageState:
		{
			Trigger *trigger = triggerNode;
			if (static_cast<const CollectableStateMessage *>(message)->GetActiveFlag())
			{
				model->Enable();
				if (trigger)
				{
					trigger->Enable();
				}
			}
			else
			{
				model->Disable();
				if (trigger)
				{
					trigger->Disable();
				}
			}

			break;
		}

		default:

			Controller::ReceiveMessage(message);
			break;
	}
}

void CollectableController::SendInitialStateMessages(Player *player) const
{
	player->SendMessage(CollectableStateMessage(GetControllerIndex(), GetTargetNode()->Enabled()));
}

void CollectableController::Move(void)
{
	Model *model = GetTargetNode();
	QuadEffect *effect = quadEffectNode;

	if (model->Enabled())
	{
		Matrix3D	rotator;

		float dt = TheTimeMgr->GetFloatDeltaTime();
		phaseAngle = PositiveFrac((phaseAngle + dt * 0.0025F) * K::one_over_two_tau) * K::two_tau;

		rotator.SetRotationAboutZ(phaseAngle);
		model->SetNodeMatrix3D(rotator);
		model->SetNodePosition(Point3D(centerPosition.x, centerPosition.y, centerPosition.z + 0.2F * Sin(phaseAngle * 0.5F)));
		model->Invalidate();
	}
	else if (!effect)
	{
		if ((TheMessageMgr->Server()) && ((respawnTime -= TheTimeMgr->GetDeltaTime()) <= 0))
		{
			TheMessageMgr->SendMessageAll(ControllerMessage(kCollectableMessageRespawn, GetControllerIndex()));
		}
	}

	if (effect)
	{
		float alpha = effectInterpolator.UpdateValue();
		unsigned_int32 mode = effectInterpolator.GetMode();
		if (mode != kInterpolatorStop)
		{
			effect->GetObject()->SetQuadAlpha(alpha);
			if ((mode & kInterpolatorBackward) && (!model->Enabled()) && (respawnTime <= 0))
			{
				effectInterpolator.SetRate(0.016F);

				model->Enable();
				model->Invalidate();

				Trigger *trigger = triggerNode;
				if (trigger)
				{
					trigger->Enable();
				}

				OmniSource *source = new OmniSource("sound/Respawn", 50.0F);
				source->SetNodePosition(centerPosition);
				model->GetSuperNode()->AppendNewSubnode(source);

				MaterializeParticleSystem *system = new MaterializeParticleSystem(respawnColor, (collectableType == CollectableProperty::kCollectableWeapon) ? 0.75F : 0.5F);
				system->SetNodePosition(centerPosition);
				model->GetSuperNode()->AppendNewSubnode(system);

				phaseAngle = 0.0F;
			}
		}
		else
		{
			delete effect;
			quadEffectNode = nullptr;
		}
	}
}

void CollectableController::Activate(Node *initiator, Node *trigger)
{
	if (scriptController)
	{
		ScriptObject *scriptObject = scriptController->GetScriptObject();
		if (scriptObject)
		{
			scriptController->ExecuteScript(scriptObject, initiator, trigger, kEventControllerActivate);

			const Value *value = scriptObject->GetValue("ok");
			if ((value) && (!value->GetBooleanValue()))
			{
				return;
			}
		}
	}

	TheMessageMgr->SendMessageAll(ControllerMessage(kCollectableMessageCollect, GetControllerIndex()));
}


CollectableStateMessage::CollectableStateMessage(int32 controllerIndex) : ControllerMessage(CollectableController::kCollectableMessageState, controllerIndex)
{
}

CollectableStateMessage::CollectableStateMessage(int32 controllerIndex, bool active) : ControllerMessage(CollectableController::kCollectableMessageState, controllerIndex)
{
	activeFlag = active;
}

CollectableStateMessage::~CollectableStateMessage()
{
}

void CollectableStateMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << activeFlag;
}

bool CollectableStateMessage::Decompress(Decompressor& data)
{
	if (ControllerMessage::Decompress(data))
	{
		data >> activeFlag;
		return (true);
	}

	return (false);
}


WindController::WindController() : Controller(kControllerWind)
{
	windRange[0].Set(-1.0F, 1.0F);
	windRange[1].Set(-1.0F, 1.0F);
	windRange[2].Set(-1.0F, 1.0F);

	blowTime.Set(1000, 2000);
	restTime.Set(0, 0);
}

WindController::WindController(const WindController& windController) : Controller(windController)
{
	windRange[0] = windController.windRange[0];
	windRange[1] = windController.windRange[1];
	windRange[2] = windController.windRange[2];

	blowTime = windController.blowTime;
	restTime = windController.restTime;
}

WindController::~WindController()
{
}

Controller *WindController::Replicate(void) const
{
	return (new WindController(*this));
}

bool WindController::ValidNode(const Node *node)
{
	return (node->GetNodeType() == kNodeField);
}

void WindController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Controller::Pack(data, packFlags);

	data << ChunkHeader('RANG', sizeof(Range<float>) * 3);
	data << windRange[0];
	data << windRange[1];
	data << windRange[2];

	data << ChunkHeader('BLOW', sizeof(Range<int32>));
	data << blowTime;

	data << ChunkHeader('REST', sizeof(Range<int32>));
	data << restTime;

	data << TerminatorChunk;
}

void WindController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Controller::Unpack(data, unpackFlags);
	UnpackChunkList<WindController>(data, unpackFlags);
}

bool WindController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'RANG':

			data >> windRange[0];
			data >> windRange[1];
			data >> windRange[2];
			return (true);

		case 'BLOW':

			data >> blowTime;
			return (true);

		case 'REST':

			data >> restTime;
			return (true);
	}

	return (false);
}

int32 WindController::GetSettingCount(void) const
{
	return (10);
}

Setting *WindController::GetSetting(int32 index) const
{
	const StringTable *table = TheGame->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerWind, 'XMIN'));
		return (new TextSetting('XMIN', windRange[0].min, title));
	}
	else if (index == 1)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerWind, 'XMAX'));
		return (new TextSetting('XMAX', windRange[0].max, title));
	}
	else if (index == 2)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerWind, 'YMIN'));
		return (new TextSetting('YMIN', windRange[1].min, title));
	}
	else if (index == 3)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerWind, 'YMAX'));
		return (new TextSetting('YMAX', windRange[1].max, title));
	}
	else if (index == 4)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerWind, 'ZMIN'));
		return (new TextSetting('ZMIN', windRange[2].min, title));
	}
	else if (index == 5)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerWind, 'ZMAX'));
		return (new TextSetting('ZMAX', windRange[2].max, title));
	}
	else if (index == 6)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerWind, 'BMIN'));
		return (new TextSetting('BMIN', (float) (blowTime.min * 0.001F), title));
	}
	else if (index == 7)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerWind, 'BMAX'));
		return (new TextSetting('BMAX', (float) (blowTime.max * 0.001F), title));
	}
	else if (index == 8)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerWind, 'RMIN'));
		return (new TextSetting('RMIN', (float) (restTime.min * 0.001F), title));
	}
	else if (index == 9)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerWind, 'RMAX'));
		return (new TextSetting('RMAX', (float) (restTime.max * 0.001F), title));
	}

	return (nullptr);
}

void WindController::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'XMIN')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		windRange[0].min = Text::StringToFloat(text);
	}
	else if (identifier == 'XMAX')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		windRange[0].max = Text::StringToFloat(text);
	}
	else if (identifier == 'YMIN')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		windRange[1].min = Text::StringToFloat(text);
	}
	else if (identifier == 'YMAX')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		windRange[1].max = Text::StringToFloat(text);
	}
	else if (identifier == 'ZMIN')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		windRange[2].min = Text::StringToFloat(text);
	}
	else if (identifier == 'ZMAX')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		windRange[2].max = Text::StringToFloat(text);
	}
	else if (identifier == 'BMIN')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		blowTime.min = MaxZero((int32) (Text::StringToFloat(text) * 1000.0F + 0.5F));
	}
	else if (identifier == 'BMAX')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		blowTime.max = Max((int32) (Text::StringToFloat(text) * 1000.0F + 0.5F), blowTime.min);
	}
	else if (identifier == 'RMIN')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		restTime.min = MaxZero((int32) (Text::StringToFloat(text) * 1000.0F + 0.5F));
	}
	else if (identifier == 'RMAX')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		restTime.max = Max((int32) (Text::StringToFloat(text) * 1000.0F + 0.5F), restTime.min);
	}
}

void WindController::Preprocess(void)
{
	Controller::Preprocess();

	windForce = nullptr;

	const Field *field = GetTargetNode();
	if (!field->GetManipulator())
	{
		if (field->GetObject()->GetFieldFlags() & kFieldExclusive)
		{
			SetControllerFlags(GetControllerFlags() | kControllerLocal);
		}

		Force *force = field->GetForce();
		if ((force) && (force->GetForceType() == kForceWind))
		{
			windForce = static_cast<WindForce *>(force);
		}
	}

	windFlag = false;
	windTime = 0;
	windParam = 1.0F;
	windVelocity[0].Set(0.0F, 0.0F, 0.0F);
}

ControllerMessage *WindController::CreateMessage(ControllerMessageType type) const
{
	switch (type)
	{
		case kWindMessageState:

			return (new WindStateMessage(GetControllerIndex()));
	}

	return (Controller::CreateMessage(type));
}

void WindController::ReceiveMessage(const ControllerMessage *message)
{
	if (message->GetControllerMessageType() == kWindMessageState)
	{
		if (windForce)
		{
			const WindStateMessage *m = static_cast<const WindStateMessage *>(message);
			windVelocity[1] = m->GetWindVelocity();
			windVelocity[0] = windForce->GetWindVelocity();
			windParam = 0.0F;
		}
	}
	else
	{
		Controller::ReceiveMessage(message);
	}
}

void WindController::SendInitialStateMessages(Player *player) const
{
	if (windForce)
	{
		player->SendMessage(WindStateMessage(GetControllerIndex(), windForce->GetWindVelocity()));
	}
}

void WindController::Move(void)
{
	unsigned_int32 flags = GetControllerFlags();
	if ((flags & kControllerLocal) || (TheMessageMgr->Server()))
	{
		if ((windTime -= TheTimeMgr->GetDeltaTime()) <= 0)
		{
			windFlag = !windFlag;
			if (windFlag)
			{
				windTime = Math::Random(blowTime);

				if (windForce)
				{
					float x = Math::RandomFloat(windRange[0]);
					float y = Math::RandomFloat(windRange[1]);
					float z = Math::RandomFloat(windRange[2]);

					if (flags & kControllerLocal)
					{
						windVelocity[1].Set(x, y, z);
						windVelocity[0] = windForce->GetWindVelocity();
						windParam = 0.0F;
					}
					else
					{
						TheMessageMgr->SendMessageAll(WindStateMessage(GetControllerIndex(), Vector3D(x, y, z)));
					}
				}
			}
			else
			{
				windTime = Math::Random(restTime);

				if (windForce)
				{
					if (flags & kControllerLocal)
					{
						windVelocity[1].Set(0.0F, 0.0F, 0.0F);
						windVelocity[0] = windForce->GetWindVelocity();
						windParam = 0.0F;
					}
					else
					{
						TheMessageMgr->SendMessageAll(WindStateMessage(GetControllerIndex(), Zero3D));
					}
				}
			}
		}
	}

	float t = windParam;
	if (t < 1.0F)
	{
		t = Fmin(t + TheTimeMgr->GetFloatDeltaTime() * 0.001F, 1.0F);
		windParam = t;

		windForce->SetWindVelocity(windVelocity[0] * (1.0F - t) + windVelocity[1] * t);
	}
}


WindStateMessage::WindStateMessage(int32 controllerIndex) : ControllerMessage(WindController::kWindMessageState, controllerIndex)
{
}

WindStateMessage::WindStateMessage(int32 controllerIndex, const Vector3D& velocity) : ControllerMessage(WindController::kWindMessageState, controllerIndex)
{
	windVelocity = velocity;
}

WindStateMessage::~WindStateMessage()
{
}

void WindStateMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << windVelocity;
}

bool WindStateMessage::Decompress(Decompressor& data)
{
	if (ControllerMessage::Decompress(data))
	{
		data >> windVelocity;
		return (true);
	}

	return (false);
}


LightningController::LightningController() : Controller(kControllerLightning)
{
	SetControllerFlags(kControllerAsleep);

	lightningCount = -1;
	boltEffect = nullptr;
}

LightningController::LightningController(const LightningController& lightningController) : Controller(lightningController)
{
	SetControllerFlags(kControllerAsleep);

	lightningCount = -1;
	boltEffect = nullptr;
}

LightningController::~LightningController()
{
}

Controller *LightningController::Replicate(void) const
{
	return (new LightningController(*this));
}

bool LightningController::ValidNode(const Node *node)
{
	return (node->GetNodeType() == kNodeLight);
}

void LightningController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Controller::Pack(data, packFlags);

	int32 count = lightningCount;
	if (count >= 0)
	{
		data << ChunkHeader('COLR', sizeof(ColorRGB));
		data << lightColor;

		data << ChunkHeader('LTNG', 8);
		data << count;
		data << lightningTime;

		if (count > 0)
		{
			data << ChunkHeader('INTS', 4);
			data << lightningIntensity;

			PackHandle handle = data.BeginChunk('INTP');
			lightningInterpolator.Pack(data, packFlags);
			data.EndChunk(handle);
		}
	}

	if ((boltEffect) && (boltEffect->LinkedNodePackable(packFlags)))
	{
		data << ChunkHeader('BOLT', 4);
		data << boltEffect->GetNodeIndex();
	}

	data << TerminatorChunk;
}

void LightningController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Controller::Unpack(data, unpackFlags);
	UnpackChunkList<LightningController>(data, unpackFlags);
}

bool LightningController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'COLR':

			data >> lightColor;
			return (true);

		case 'LTNG':

			data >> lightningCount;
			data >> lightningTime;
			return (true);

		case 'INTS':

			data >> lightningIntensity;
			return (true);

		case 'INTP':

			lightningInterpolator.Unpack(data, unpackFlags);
			return (true);

		case 'BOLT':
		{
			int32	nodeIndex;

			data >> nodeIndex;
			data.AddNodeLink(nodeIndex, &BoltLinkProc, this);
			return (true);
		}
	}

	return (false);
}

void LightningController::BoltLinkProc(Node *node, void *cookie)
{
	LightningController *controller = static_cast<LightningController *>(cookie);
	controller->boltEffect = static_cast<BoltEffect *>(node);
}

void LightningController::Preprocess(void)
{
	Controller::Preprocess();

	boltMarker[0] = nullptr;
	boltMarker[1] = nullptr;

	const Hub *hub = GetTargetNode()->GetHub();
	if (hub)
	{
		const Connector *connector = hub->GetFirstOutgoingEdge();
		while (connector)
		{
			const Node *node = connector->GetConnectorTarget();
			if ((node) && (node->GetNodeType() == kNodeMarker))
			{
				if (!boltMarker[0])
				{
					boltMarker[0] = static_cast<const Marker *>(node);
				}
				else
				{
					boltMarker[1] = static_cast<const Marker *>(node);
					break;
				}
			}

			connector = connector->GetNextOutgoingEdge();
		}
	}
}

ControllerMessage *LightningController::CreateMessage(ControllerMessageType type) const
{
	switch (type)
	{
		case kLightningMessageActivate:

			return (new ControllerMessage(kLightningMessageActivate, GetControllerIndex()));
	}

	return (Controller::CreateMessage(type));
}

void LightningController::ReceiveMessage(const ControllerMessage *message)
{
	if (message->GetControllerMessageType() == kLightningMessageActivate)
	{
		lightningTime = Math::Random(20000) + 10000;
		lightningCount = Math::Random(3) + 5;

		if (boltMarker[1])
		{
			const Marker *marker1 = boltMarker[0];
			const Marker *marker2 = boltMarker[1];
			const Point3D& position1 = marker1->GetWorldPosition();
			const Point3D& position2 = marker2->GetWorldPosition();

			Point3D boltPosition = position1 + (position2 - position1) * Math::RandomFloat(1.0F);

			Path path(K::y_unit);
			Point3D p1(0.0F, 0.0F, 0.0F);
			Point3D p2(0.0F, 0.0F, -60.0F + Math::RandomFloat(30.0F));

			float x = Math::RandomFloat(180.0F) - 90.0F;
			Point3D p3(x, 0.0F, -100.0F + Math::RandomFloat(30.0F));
			Point3D p4(x + Math::RandomFloat(60.0F) - 30.0F, 0.0F, -150.0F + Math::RandomFloat(30.0F));

			BezierPathComponent component(p1, p2, p3, p4);
			path.AppendPathComponent(&component);

			World *world = marker1->GetWorld();

			BoltEffect *bolt = new BoltEffect(&path, 1.5F, 2.0F, ColorRGBA(1.0F, 1.0F, 1.0F, 1.0F));
			bolt->SetNodePosition(boltPosition);
			bolt->GetObject()->Build();
			world->AddNewNode(bolt);
			boltEffect = bolt;

			const BoltEffectObject *object = bolt->GetObject();
			for (machine a = 0; a < 3; a++)
			{
				const Point3D& q = object->GetVertexArray()[(Math::Random(8) + 5) * 2].position;
				p2 = q + Vector3D(0.0F, 0.0F, -25.0F + Math::RandomFloat(15.0F));

				x = Math::RandomFloat(100.0F) - 50.0F;
				p3 = q + Vector3D(x, 0.0F, -50.0F + Math::RandomFloat(15.0F));
				p4 = q + Vector3D(x + Math::RandomFloat(30.0F) - 15.0F, 0.0F, -75.0F + Math::RandomFloat(15.0F));

				Path subpath(K::y_unit);
				BezierPathComponent subcomponent(q, p2, p3, p4);
				subpath.AppendPathComponent(&subcomponent);

				BoltEffect *subbolt = new BoltEffect(&subpath, 0.5F, 1.5F, ColorRGBA(1.0F, 1.0F, 1.0F, 1.0F));
				subbolt->GetObject()->Build();
				bolt->AppendNewSubnode(subbolt);
			}

			static char thunderName[15] = "sound/Thunder1";
			static char thunderVariation[10] = {'1', '2', '3', '4', '5', '2', '4', '1', '3', '5'};
			static int32 thunderCounter = 0;

			thunderName[13] = thunderVariation[thunderCounter];
			if (++thunderCounter == 10)
			{
				thunderCounter = 0;
			}

			OmniSource *source = new OmniSource(thunderName, 16384.0F);
			OmniSourceObject *sourceObject = source->GetObject();
			sourceObject->SetSourceFlags(sourceObject->GetSourceFlags() | kSourceStream);
			source->SetNodePosition(boltPosition);
			world->AddNewNode(source);
		}
	}
	else
	{
		Controller::ReceiveMessage(message);
	}
}

void LightningController::Wake(void)
{
	Controller::Wake();

	if (lightningCount < 0)
	{
		lightningCount = 0;
		lightningTime = 10000;

		lightColor = GetTargetNode()->GetOwningZone()->GetObject()->GetAmbientLight().GetColorRGB();
	}
}

void LightningController::Move(void)
{
	if (lightningCount == 0)
	{
		if (TheMessageMgr->Server())
		{
			if ((lightningTime -= TheTimeMgr->GetDeltaTime()) < 0)
			{
				TheMessageMgr->SendMessageAll(ControllerMessage(kLightningMessageActivate, GetControllerIndex()));
			}
		}
	}
	else
	{
		if (lightningInterpolator.GetMode() == kInterpolatorStop)
		{
			if (--lightningCount != 0)
			{
				lightningIntensity = Math::RandomFloat(0.75F) + 0.25F;
				float rate = Math::RandomFloat(0.02F) + 0.01F;
				lightningInterpolator.Set(0.0F, rate, kInterpolatorForward | kInterpolatorOscillate);
			}
			else
			{
				delete boltEffect;
				boltEffect = nullptr;
			}
		}
		else
		{
			float intensity = lightningInterpolator.UpdateValue() * lightningIntensity;
			ColorRGB color(Fmin(lightColor.red + intensity, 1.0F), Fmin(lightColor.green + intensity, 1.0F), Fmin(lightColor.blue + intensity, 1.0F));
			GetTargetNode()->GetOwningZone()->GetObject()->SetAmbientLight(color);
			if (boltEffect)
			{
				boltEffect->SetTubeColor(color);

				Node *node = boltEffect->GetFirstSubnode();
				while (node)
				{
					static_cast<BoltEffect *>(node)->SetTubeColor(color);
					node = node->Next();
				}
			}
		}
	}
}

// ZYUQURM
