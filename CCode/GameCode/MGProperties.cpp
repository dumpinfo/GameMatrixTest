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


#include "MGProperties.h"
#include "MGFighter.h"
#include "MGSpider.h"
#include "MGGame.h"


using namespace C4;


const CollectableProperty::CollectableType CollectableProperty::collectableTypeTable[kCollectableTypeCount] =
{
	kCollectableWeapon, kCollectableAmmo, kCollectableHealth, kCollectablePower, kCollectableTreasure
};


PlayerStateProperty::PlayerStateProperty() : Property(kPropertyPlayerState)
{
	modelNode = nullptr;
}

PlayerStateProperty::PlayerStateProperty(Model *model, const PlayerState *state, bool flashlight) : Property(kPropertyPlayerState)
{
	modelNode = model;
	playerState = *state;
	flashlightFlag = flashlight;
}

PlayerStateProperty::~PlayerStateProperty()
{
}

void PlayerStateProperty::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Property::Pack(data, packFlags);

	if (modelNode)
	{
		data << ChunkHeader('modl', 4);
		data << modelNode->GetNodeIndex();
	}

	data << ChunkHeader('scor', 8);
	data << playerState.playerScore;
	data << playerState.playerScoreFraction;

	data << ChunkHeader('hlth', 4);
	data << playerState.playerHealth;

	data << ChunkHeader('weap', 8);
	data << playerState.weaponFlags;
	data << playerState.currentWeapon;

	data << ChunkHeader('dfer', 12);
	data << playerState.deferredWeapon;
	data << playerState.deferredWeaponTime;
	data << playerState.deferredWeaponFrame;

	data << ChunkHeader('ammo', 4 * kWeaponCount);
	for (machine a = 0; a < kWeaponCount; a++)
	{
		data << playerState.weaponAmmo[a][0];
		data << playerState.weaponAmmo[a][1];
	}

	data << ChunkHeader('powr', 4 * kPowerCount);
	for (machine a = 0; a < kPowerCount; a++)
	{
		data << playerState.powerTime[a];
	}

	data << ChunkHeader('kill', 4);
	data << playerState.killCount;

	data << ChunkHeader('tres', 4 * kTreasureCount);
	for (machine a = 0; a < kTreasureCount; a++)
	{
		data << playerState.treasureCount[a];
	}

	data << ChunkHeader('flsh', 4);
	data << flashlightFlag;

	data << TerminatorChunk;
}

void PlayerStateProperty::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Property::Unpack(data, unpackFlags);
	UnpackChunkList<PlayerStateProperty>(data, unpackFlags);
}

bool PlayerStateProperty::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'modl':
		{
			int32	nodeIndex;

			data >> nodeIndex;
			data.AddNodeLink(nodeIndex, &ModelLinkProc, this); 
			return (true);
		}
 
		case 'scor':
 
			data >> playerState.playerScore;
			data >> playerState.playerScoreFraction;
			return (true); 

		case 'hlth': 
 
			data >> playerState.playerHealth;
			return (true);

		case 'weap': 

			data >> playerState.weaponFlags;
			data >> playerState.currentWeapon;
			return (true);

		case 'dfer':

			data >> playerState.deferredWeapon;
			data >> playerState.deferredWeaponTime;
			data >> playerState.deferredWeaponFrame;
			return (true);

		case 'ammo':

			for (machine a = 0; a < kWeaponCount; a++)
			{
				data >> playerState.weaponAmmo[a][0];
				data >> playerState.weaponAmmo[a][1];
			}

			return (true);

		case 'powr':

			for (machine a = 0; a < kPowerCount; a++)
			{
				data >> playerState.powerTime[a];
			}

			return (true);

		case 'kill':

			data >> playerState.killCount;
			return (true);

		case 'tres':

			for (machine a = 0; a < kTreasureCount; a++)
			{
				data >> playerState.treasureCount[a];
			}

			return (true);

		case 'flsh':

			data >> flashlightFlag;
			return (true);
	}

	return (false);
}

void PlayerStateProperty::ModelLinkProc(Node *node, void *cookie)
{
	PlayerStateProperty *property = static_cast<PlayerStateProperty *>(cookie);
	property->modelNode = static_cast<Model *>(node);
}


CollectableProperty::CollectableProperty() : Property(kPropertyCollectable)
{
	collectableType = kCollectableWeapon;
	respawnColor.Set(1.0F, 1.0F, 0.25F, 1.0F);
}

CollectableProperty::CollectableProperty(const CollectableProperty& collectableProperty) : Property(collectableProperty)
{
	collectableType = collectableProperty.collectableType;
	respawnColor = collectableProperty.respawnColor;
}

CollectableProperty::~CollectableProperty()
{
}

Property *CollectableProperty::Replicate(void) const
{
	return (new CollectableProperty(*this));
}

void CollectableProperty::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Property::Pack(data, packFlags);

	data << ChunkHeader('TYPE', 4);
	data << collectableType;

	data << ChunkHeader('COLR', sizeof(ColorRGBA));
	data << respawnColor;

	data << TerminatorChunk;
}

void CollectableProperty::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Property::Unpack(data, unpackFlags);
	UnpackChunkList<CollectableProperty>(data, unpackFlags);
}

bool CollectableProperty::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'TYPE':

			data >> collectableType;
			return (true);

		case 'COLR':

			data >> respawnColor;
			return (true);
	}

	return (false);
}

int32 CollectableProperty::GetSettingCount(void) const
{
	return (2);
}

Setting *CollectableProperty::GetSetting(int32 index) const
{
	const StringTable *table = TheGame->GetStringTable();

	if (index == 0)
	{
		int32 selection = 0;
		for (machine a = 1; a < kCollectableTypeCount; a++)
		{
			if (collectableType == collectableTypeTable[a])
			{
				selection = a;
				break;
			}
		}

		const char *title = table->GetString(StringID('PROP', kPropertyCollectable, 'TYPE'));
		MenuSetting *menu = new MenuSetting('TYPE', selection, title, kCollectableTypeCount);
		for (machine a = 0; a < kCollectableTypeCount; a++)
		{
			menu->SetMenuItemString(a, table->GetString(StringID('PROP', kPropertyCollectable, 'TYPE', collectableTypeTable[a])));
		}

		return (menu);
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('PROP', kPropertyCollectable, 'COLR'));
		const char *picker = table->GetString(StringID('PROP', kPropertyCollectable, 'PICK'));
		return (new ColorSetting('COLR', respawnColor, title, picker, kColorPickerAlpha));
	}

	return (nullptr);
}

void CollectableProperty::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'TYPE')
	{
		collectableType = collectableTypeTable[static_cast<const MenuSetting *>(setting)->GetMenuSelection()];
	}
	else if (identifier == 'COLR')
	{
		respawnColor = static_cast<const ColorSetting *>(setting)->GetColor();
	}
}


ContainerProperty::ContainerProperty() : Property(kPropertyContainer)
{
	treasureType = 0;
	healthType = 0;
	powerType = 0;
	weaponType = 0;
	ammoType = 0;
	monsterType = 0;
	hazardType = 0;
}

ContainerProperty::ContainerProperty(const ContainerProperty& containerProperty) : Property(containerProperty)
{
	treasureType = containerProperty.treasureType;
	healthType = containerProperty.healthType;
	powerType = containerProperty.powerType;
	weaponType = containerProperty.weaponType;
	ammoType = containerProperty.ammoType;
	monsterType = containerProperty.monsterType;
	hazardType = containerProperty.hazardType;
}

ContainerProperty::~ContainerProperty()
{
}

Property *ContainerProperty::Replicate(void) const
{
	return (new ContainerProperty(*this));
}

bool ContainerProperty::ValidNode(const Node *node)
{
	const Controller *controller = node->GetController();
	return ((controller) && (controller->GetControllerType() == kControllerGameRigidBody));
}

void ContainerProperty::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Property::Pack(data, packFlags);

	data << ChunkHeader('TRES', 4);
	data << treasureType;

	data << ChunkHeader('HLTH', 4);
	data << healthType;

	data << ChunkHeader('POWR', 4);
	data << powerType;

	data << ChunkHeader('WEAP', 4);
	data << weaponType;

	data << ChunkHeader('AMMO', 4);
	data << ammoType;

	data << ChunkHeader('MNST', 4);
	data << monsterType;

	data << ChunkHeader('HZRD', 4);
	data << hazardType;

	data << TerminatorChunk;
}

void ContainerProperty::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Property::Unpack(data, unpackFlags);
	UnpackChunkList<ContainerProperty>(data, unpackFlags);
}

bool ContainerProperty::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'TRES':

			data >> treasureType;
			return (true);

		case 'HLTH':

			data >> healthType;
			return (true);

		case 'POWR':

			data >> powerType;
			return (true);

		case 'WEAP':

			data >> weaponType;
			return (true);

		case 'AMMO':

			data >> ammoType;
			return (true);

		case 'MNST':

			data >> monsterType;
			return (true);

		case 'HZRD':

			data >> hazardType;
			return (true);
	}

	return (false);
}

int32 ContainerProperty::GetSettingCount(void) const
{
	return (7);
}

Setting *ContainerProperty::GetSetting(int32 index) const
{
	const StringTable *table = TheGame->GetStringTable();
	const char *none = table->GetString(StringID('PROP', kPropertyContainer, 'NONE'));

	if (index == 0)
	{
		const char *title = table->GetString(StringID('PROP', kPropertyContainer, 'TRES'));
		MenuSetting *menu = new MenuSetting('TRES', 0, title, 1);
		menu->SetMenuItemString(0, none);
		return (menu);
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('PROP', kPropertyContainer, 'HLTH'));
		MenuSetting *menu = new MenuSetting('HLTH', 0, title, 1);
		menu->SetMenuItemString(0, none);
		return (menu);
	}

	if (index == 2)
	{
		const char *title = table->GetString(StringID('PROP', kPropertyContainer, 'POWR'));
		MenuSetting *menu = new MenuSetting('POWR', 0, title, 1);
		menu->SetMenuItemString(0, none);
		return (menu);
	}

	if (index == 3)
	{
		const char *title = table->GetString(StringID('PROP', kPropertyContainer, 'WEAP'));
		MenuSetting *menu = new MenuSetting('WEAP', 0, title, 1);
		menu->SetMenuItemString(0, none);
		return (menu);
	}

	if (index == 4)
	{
		const char *title = table->GetString(StringID('PROP', kPropertyContainer, 'AMMO'));
		MenuSetting *menu = new MenuSetting('AMMO', 0, title, 1);
		menu->SetMenuItemString(0, none);
		return (menu);
	}

	if (index == 5)
	{
		const char *title = table->GetString(StringID('PROP', kPropertyContainer, 'MNST'));
		MenuSetting *menu = new MenuSetting('MNST', 0, title, 1);
		menu->SetMenuItemString(0, none);
		return (menu);
	}

	if (index == 6)
	{
		const char *title = table->GetString(StringID('PROP', kPropertyContainer, 'HZRD'));
		MenuSetting *menu = new MenuSetting('HZRD', 0, title, 1);
		menu->SetMenuItemString(0, none);
		return (menu);
	}

	return (nullptr);
}

void ContainerProperty::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'TRES')
	{

	}
	else if (identifier == 'HLTH')
	{

	}
	else if (identifier == 'POWR')
	{

	}
	else if (identifier == 'WEAP')
	{

	}
	else if (identifier == 'AMMO')
	{

	}
	else if (identifier == 'MNST')
	{

	}
	else if (identifier == 'HZRD')
	{

	}
}


ShootableProperty::ShootableProperty() : Property(kPropertyShootable)
{
}

ShootableProperty::ShootableProperty(const ShootableProperty& shootableProperty) : Property(shootableProperty)
{
}

ShootableProperty::~ShootableProperty()
{
}

Property *ShootableProperty::Replicate(void) const
{
	return (new ShootableProperty(*this));
}

bool ShootableProperty::ValidNode(const Node *node)
{
	return (node->GetNodeType() == kNodeGeometry);
}

bool ShootableProperty::SharedProperty(void) const
{
	return (true);
}


BreakableProperty::BreakableProperty() : Property(kPropertyBreakable)
{
	breakableFlags = 0;
	collisionExclusionMask = 0;
	activationConnectorKey[0] = 0;

	for (machine a = 0; a < kMaxBreakableSoundCount; a++)
	{
		breakableSoundName[a][0] = 0;
	}
}

BreakableProperty::BreakableProperty(const BreakableProperty& breakableProperty) : Property(breakableProperty)
{
	breakableFlags = breakableProperty.breakableFlags;
	collisionExclusionMask = breakableProperty.collisionExclusionMask;
	activationConnectorKey = breakableProperty.activationConnectorKey;

	for (machine a = 0; a < kMaxBreakableSoundCount; a++)
	{
		breakableSoundName[a] = breakableProperty.breakableSoundName[a];
	}
}

BreakableProperty::~BreakableProperty()
{
}

Property *BreakableProperty::Replicate(void) const
{
	return (new BreakableProperty(*this));
}

bool BreakableProperty::ValidNode(const Node *node)
{
	const Controller *controller = node->GetController();
	if ((controller) && (controller->GetBaseControllerType() == kControllerRigidBody))
	{
		return (static_cast<const RigidBodyController *>(controller)->GetRigidBodyType() == kRigidBodyGame);
	}

	return (false);
}

void BreakableProperty::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Property::Pack(data, packFlags);

	data << ChunkHeader('FLAG', 4);
	data << breakableFlags;

	data << ChunkHeader('COLL', 4);
	data << collisionExclusionMask;

	if (activationConnectorKey[0] != 0)
	{
		PackHandle handle = data.BeginChunk('CKEY');
		data << activationConnectorKey;
		data.EndChunk(handle);
	}

	for (machine a = 0; a < kMaxBreakableSoundCount; a++)
	{
		const ResourceName& name = breakableSoundName[a];
		if (name[0] != 0)
		{
			PackHandle handle = data.BeginChunk('SND0' + a);
			data << name;
			data.EndChunk(handle);
		}
	}

	data << TerminatorChunk;
}

void BreakableProperty::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Property::Unpack(data, unpackFlags);
	UnpackChunkList<BreakableProperty>(data, unpackFlags);
}

bool BreakableProperty::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> breakableFlags;
			return (true);

		case 'COLL':

			data >> collisionExclusionMask;
			return (true);

		case 'CKEY':

			data >> activationConnectorKey;
			return (true);
	}

	unsigned_int32 index = chunkHeader->chunkType - 'SND0';
	if (index < kMaxBreakableSoundCount)
	{
		data >> breakableSoundName[index];
		return (true);
	}

	return (false);
}

void *BreakableProperty::BeginSettingsUnpack(void)
{
	activationConnectorKey[0] = 0;

	for (machine a = 0; a < kMaxBreakableSoundCount; a++)
	{
		breakableSoundName[a][0] = 0;
	}

	return (Property::BeginSettingsUnpack());
}

int32 BreakableProperty::GetSettingCount(void) const
{
	return (kMaxBreakableSoundCount + 10);
}

Setting *BreakableProperty::GetSetting(int32 index) const
{
	const StringTable *table = TheGame->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('PROP', kPropertyBreakable, 'BRAK'));
		return (new HeadingSetting('BRAK', title));
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('PROP', kPropertyBreakable, 'BRAK', 'KALL'));
		return (new BooleanSetting('KALL', ((breakableFlags & kBreakableKeepAll) != 0), title));
	}

	if (index == 2)
	{
		const char *title = table->GetString(StringID('PROP', kPropertyBreakable, 'BRAK', 'IFRC'));
		return (new BooleanSetting('IFRC', ((breakableFlags & kBreakableForceFieldInhibit) != 0), title));
	}

	if (index == 3)
	{
		const char *title = table->GetString(StringID('PROP', kPropertyBreakable, 'BRAK', 'CKEY'));
		return (new TextSetting('CKEY', activationConnectorKey, title, kMaxConnectorKeyLength, &Connector::ConnectorKeyFilter));
	}

	if (index == 4)
	{
		const char *title = table->GetString(StringID('PROP', kPropertyBreakable, 'SOND'));
		return (new HeadingSetting('SOND', title));
	}

	if ((index -= 5) < kMaxBreakableSoundCount)
	{
		Type identifier = 'SND0' + index;
		const char *title = table->GetString(StringID('PROP', kPropertyBreakable, 'SOND', identifier));
		const char *picker = table->GetString(StringID('PROP', kPropertyBreakable, 'SOND', 'PICK'));
		return (new ResourceSetting(identifier, breakableSoundName[index], title, picker, SoundResource::GetDescriptor()));
	}

	if (index == kMaxBreakableSoundCount)
	{
		const char *title = table->GetString(StringID('PROP', kPropertyBreakable, 'CFLG'));
		return (new HeadingSetting('CFLG', title));
	}

	if (index == kMaxBreakableSoundCount + 1)
	{
		const char *title = table->GetString(StringID('PROP', kPropertyBreakable, 'CFLG', 'BODY'));
		return (new BooleanSetting('BODY', ((collisionExclusionMask & kCollisionRigidBody) == 0), title));
	}

	if (index == kMaxBreakableSoundCount + 2)
	{
		const char *title = table->GetString(StringID('PROP', kPropertyBreakable, 'CFLG', 'CHAR'));
		return (new BooleanSetting('CHAR', ((collisionExclusionMask & kCollisionCharacter) == 0), title));
	}

	if (index == kMaxBreakableSoundCount + 3)
	{
		const char *title = table->GetString(StringID('PROP', kPropertyBreakable, 'CFLG', 'PROJ'));
		return (new BooleanSetting('PROJ', ((collisionExclusionMask & kCollisionProjectile) == 0), title));
	}

	if (index == kMaxBreakableSoundCount + 4)
	{
		const char *title = table->GetString(StringID('PROP', kPropertyBreakable, 'CFLG', 'VHCL'));
		return (new BooleanSetting('VHCL', ((collisionExclusionMask & kCollisionVehicle) == 0), title));
	}

	return (nullptr);
}

void BreakableProperty::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'KALL')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			breakableFlags |= kBreakableKeepAll;
		}
		else
		{
			breakableFlags &= ~kBreakableKeepAll;
		}
	}
	else if (identifier == 'IFRC')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			breakableFlags |= kBreakableForceFieldInhibit;
		}
		else
		{
			breakableFlags &= ~kBreakableForceFieldInhibit;
		}
	}
	else if (identifier == 'CKEY')
	{
		activationConnectorKey = static_cast<const TextSetting *>(setting)->GetText();
	}
	else if (identifier == 'BODY')
	{
		if (!static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			collisionExclusionMask |= kCollisionRigidBody;
		}
		else
		{
			collisionExclusionMask &= ~kCollisionRigidBody;
		}
	}
	else if (identifier == 'CHAR')
	{
		if (!static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			collisionExclusionMask |= kCollisionCharacter;
		}
		else
		{
			collisionExclusionMask &= ~kCollisionCharacter;
		}
	}
	else if (identifier == 'PROJ')
	{
		if (!static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			collisionExclusionMask |= kCollisionProjectile;
		}
		else
		{
			collisionExclusionMask &= ~kCollisionProjectile;
		}
	}
	else if (identifier == 'VHCL')
	{
		if (!static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			collisionExclusionMask |= kCollisionVehicle;
		}
		else
		{
			collisionExclusionMask &= ~kCollisionVehicle;
		}
	}
	else
	{
		unsigned_int32 index = identifier - 'SND0';
		if (index < kMaxBreakableSoundCount)
		{
			breakableSoundName[index] = static_cast<const ResourceSetting *>(setting)->GetResourceName();
		}
	}
}

bool BreakableProperty::SharedProperty(void) const
{
	return (true);
}

const ResourceName& BreakableProperty::GetBreakableSoundName(void) const
{
	for (machine a = kMaxBreakableSoundCount - 1; a >= 1; a--)
	{
		const ResourceName& name = breakableSoundName[a];
		if (name[0] != 0)
		{
			return (breakableSoundName[Math::Random(a + 1)]);
		}
	}

	return (breakableSoundName[0]);
}


LeakingGooProperty::LeakingGooProperty() : Property(kPropertyLeakingGoo)
{
	maxLeakCount = 1;
	materialType = kMaterialWine;
}

LeakingGooProperty::LeakingGooProperty(const LeakingGooProperty& leakingGooProperty) : Property(leakingGooProperty)
{
	maxLeakCount = leakingGooProperty.maxLeakCount;
	materialType = leakingGooProperty.materialType;
}

LeakingGooProperty::~LeakingGooProperty()
{
}

Property *LeakingGooProperty::Replicate(void) const
{
	return (new LeakingGooProperty(*this));
}

bool LeakingGooProperty::ValidNode(const Node *node)
{
	if (node->GetNodeType() == kNodeInstance)
	{
		return (true);
	}

	const Controller *controller = node->GetController();
	return ((controller) && (controller->GetControllerType() == kControllerGameRigidBody));
}

void LeakingGooProperty::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Property::Pack(data, packFlags);

	data << ChunkHeader('LCNT', 4);
	data << maxLeakCount;

	data << ChunkHeader('MATL', 4);
	data << materialType;

	data << TerminatorChunk;
}

void LeakingGooProperty::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Property::Unpack(data, unpackFlags);
	UnpackChunkList<LeakingGooProperty>(data, unpackFlags);
}

bool LeakingGooProperty::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'LCNT':

			data >> maxLeakCount;
			return (true);

		case 'MATL':

			data >> materialType;
			return (true);
	}

	return (false);
}

int32 LeakingGooProperty::GetSettingCount(void) const
{
	return (2);
}

Setting *LeakingGooProperty::GetSetting(int32 index) const
{
	const StringTable *table = TheGame->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('PROP', kPropertyLeakingGoo, 'LEAK'));
		return (new TextSetting('LEAK', Text::IntegerToString(maxLeakCount), title, 2, &EditTextWidget::NumberFilter));
	}

	if (index == 1)
	{
		int32 selection = 0;
		if (materialType == kMaterialToxicGoo)
		{
			selection = 1;
		}

		const char *title = table->GetString(StringID('PROP', kPropertyLeakingGoo, 'MATL'));
		MenuSetting *menu = new MenuSetting('MATL', selection, title, 3);

		menu->SetMenuItemString(0, table->GetString(StringID('PROP', kPropertyLeakingGoo, 'MATL', kMaterialWine)));
		menu->SetMenuItemString(1, table->GetString(StringID('PROP', kPropertyLeakingGoo, 'MATL', kMaterialToxicGoo)));
		menu->SetMenuItemString(2, table->GetString(StringID('PROP', kPropertyLeakingGoo, 'MATL', kMaterialSpiderGuts)));

		return (menu);
	}

	return (nullptr);
}

void LeakingGooProperty::SetSetting(const Setting *setting)
{
	unsigned_int32 identifier = setting->GetSettingIdentifier();

	if (identifier == 'LEAK')
	{
		maxLeakCount = Max(Text::StringToInteger(static_cast<const TextSetting *>(setting)->GetText()), 1);
	}
	else if (identifier == 'MATL')
	{
		static const MaterialType type[3] =
		{
			kMaterialWine, kMaterialToxicGoo, kMaterialSpiderGuts
		};

		int32 selection = static_cast<const MenuSetting *>(setting)->GetMenuSelection();
		materialType = type[selection];
	}
}


ImpactSoundProperty::ImpactSoundProperty() : Property(kPropertyImpactSound)
{
	for (machine a = 0; a < kMaxImpactSoundCount; a++)
	{
		hardImpactSoundName[a][0] = 0;
		softImpactSoundName[a][0] = 0;
	}
}

ImpactSoundProperty::ImpactSoundProperty(const ImpactSoundProperty& impactSoundProperty) : Property(impactSoundProperty)
{
	for (machine a = 0; a < kMaxImpactSoundCount; a++)
	{
		hardImpactSoundName[a] = impactSoundProperty.hardImpactSoundName[a];
		softImpactSoundName[a] = impactSoundProperty.softImpactSoundName[a];
	}
}

ImpactSoundProperty::~ImpactSoundProperty()
{
}

Property *ImpactSoundProperty::Replicate(void) const
{
	return (new ImpactSoundProperty(*this));
}

bool ImpactSoundProperty::ValidNode(const Node *node)
{
	const Controller *controller = node->GetController();
	return ((node->GetNodeType() == kNodeGeometry) || ((controller) && (controller->GetBaseControllerType() == kControllerRigidBody)));
}

void ImpactSoundProperty::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Property::Pack(data, packFlags);

	for (machine a = 0; a < kMaxImpactSoundCount; a++)
	{
		const ResourceName& name = hardImpactSoundName[a];
		if (name[0] != 0)
		{
			PackHandle handle = data.BeginChunk('HRD0' + a);
			data << name;
			data.EndChunk(handle);
		}
	}

	for (machine a = 0; a < kMaxImpactSoundCount; a++)
	{
		const ResourceName& name = softImpactSoundName[a];
		if (name[0] != 0)
		{
			PackHandle handle = data.BeginChunk('SFT0' + a);
			data << name;
			data.EndChunk(handle);
		}
	}

	data << TerminatorChunk;
}

void ImpactSoundProperty::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Property::Unpack(data, unpackFlags);
	UnpackChunkList<ImpactSoundProperty>(data, unpackFlags);
}

bool ImpactSoundProperty::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	unsigned_int32 index = chunkHeader->chunkType - 'HRD0';
	if (index < kMaxImpactSoundCount)
	{
		data >> hardImpactSoundName[index];
		return (true);
	}

	index = chunkHeader->chunkType - 'SFT0';
	if (index < kMaxImpactSoundCount)
	{
		data >> softImpactSoundName[index];
		return (true);
	}

	return (false);
}

void *ImpactSoundProperty::BeginSettingsUnpack(void)
{
	for (machine a = 0; a < kMaxImpactSoundCount; a++)
	{
		hardImpactSoundName[a][0] = 0;
		softImpactSoundName[a][0] = 0;
	}

	return (Property::BeginSettingsUnpack());
}

int32 ImpactSoundProperty::GetSettingCount(void) const
{
	return (kMaxImpactSoundCount * 2);
}

Setting *ImpactSoundProperty::GetSetting(int32 index) const
{
	const StringTable *table = TheGame->GetStringTable();
	const char *picker = table->GetString(StringID('PROP', kPropertyImpactSound, 'PICK'));

	if (index < kMaxImpactSoundCount)
	{
		Type identifier = 'HRD0' + index;
		const char *title = table->GetString(StringID('PROP', kPropertyImpactSound, identifier));
		return (new ResourceSetting(identifier, hardImpactSoundName[index], title, picker, SoundResource::GetDescriptor()));
	}

	if ((index -= kMaxImpactSoundCount) < kMaxImpactSoundCount)
	{
		Type identifier = 'SFT0' + index;
		const char *title = table->GetString(StringID('PROP', kPropertyImpactSound, identifier));
		return (new ResourceSetting(identifier, softImpactSoundName[index], title, picker, SoundResource::GetDescriptor()));
	}

	return (nullptr);
}

void ImpactSoundProperty::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	unsigned_int32 index = identifier - 'HRD0';
	if (index < kMaxImpactSoundCount)
	{
		hardImpactSoundName[index] = static_cast<const ResourceSetting *>(setting)->GetResourceName();
	}
	else
	{
		index = identifier - 'SFT0';
		if (index < kMaxImpactSoundCount)
		{
			softImpactSoundName[index] = static_cast<const ResourceSetting *>(setting)->GetResourceName();
		}
	}
}

bool ImpactSoundProperty::SharedProperty(void) const
{
	return (true);
}

const ResourceName& ImpactSoundProperty::GetHardImpactSoundName(void) const
{
	for (machine a = kMaxImpactSoundCount - 1; a >= 1; a--)
	{
		const ResourceName& name = hardImpactSoundName[a];
		if (name[0] != 0)
		{
			return (hardImpactSoundName[Math::Random(a)]);
		}
	}

	return (hardImpactSoundName[0]);
}

const ResourceName& ImpactSoundProperty::GetSoftImpactSoundName(void) const
{
	for (machine a = kMaxImpactSoundCount - 1; a >= 1; a--)
	{
		const ResourceName& name = softImpactSoundName[a];
		if (name[0] != 0)
		{
			return (softImpactSoundName[Math::Random(a)]);
		}
	}

	return (softImpactSoundName[0]);
}


PaintCanProperty::PaintCanProperty() : Property(kPropertyPaintCan)
{
	paintColor.Set(1.0F, 1.0F, 1.0F);
}

PaintCanProperty::PaintCanProperty(const PaintCanProperty& paintCanProperty) : Property(paintCanProperty)
{
	paintColor = paintCanProperty.paintColor;
}

PaintCanProperty::~PaintCanProperty()
{
}

Property *PaintCanProperty::Replicate(void) const
{
	return (new PaintCanProperty(*this));
}

bool PaintCanProperty::ValidNode(const Node *node)
{
	const Controller *controller = node->GetController();
	return ((controller) && (controller->GetControllerType() == kControllerRigidBody));
}

void PaintCanProperty::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Property::Pack(data, packFlags);

	data << paintColor;
}

void PaintCanProperty::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Property::Unpack(data, unpackFlags);

	data >> paintColor;
}

int32 PaintCanProperty::GetSettingCount(void) const
{
	return (1);
}

Setting *PaintCanProperty::GetSetting(int32 index) const
{
	if (index == 0)
	{
		const StringTable *table = TheGame->GetStringTable();
		const char *title = table->GetString(StringID('PROP', kPropertyPaintCan, 'COLR'));
		const char *picker = table->GetString(StringID('PROP', kPropertyPaintCan, 'PICK'));
		return (new ColorSetting('COLR', paintColor, title, picker));
	}

	return (nullptr);
}

void PaintCanProperty::SetSetting(const Setting *setting)
{
	if (setting->GetSettingIdentifier() == 'COLR')
	{
		paintColor = static_cast<const ColorSetting *>(setting)->GetColor();
	}
}

bool PaintCanProperty::SharedProperty(void) const
{
	return (true);
}


TeleporterProperty::TeleporterProperty() : Property(kPropertyTeleporter)
{
	portalConnectorKey = "Portal";
}

TeleporterProperty::TeleporterProperty(const TeleporterProperty& teleporterProperty) : Property(teleporterProperty)
{
	portalConnectorKey = teleporterProperty.portalConnectorKey;
}

TeleporterProperty::~TeleporterProperty()
{
}

Property *TeleporterProperty::Replicate(void) const
{
	return (new TeleporterProperty(*this));
}

bool TeleporterProperty::ValidNode(const Node *node)
{
	return (node->GetNodeType() == kNodeGeometry);
}

void TeleporterProperty::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Property::Pack(data, packFlags);

	PackHandle handle = data.BeginChunk('PKEY');
	data << portalConnectorKey;
	data.EndChunk(handle);

	data << TerminatorChunk;
}

void TeleporterProperty::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Property::Unpack(data, unpackFlags);
	UnpackChunkList<TeleporterProperty>(data, unpackFlags);
}

bool TeleporterProperty::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'PKEY':

			data >> portalConnectorKey;
			return (true);
	}

	return (false);
}

int32 TeleporterProperty::GetSettingCount(void) const
{
	return (1);
}

Setting *TeleporterProperty::GetSetting(int32 index) const
{
	if (index == 0)
	{
		const char *title = TheGame->GetStringTable()->GetString(StringID('PROP', kPropertyTeleporter, 'PKEY'));
		return (new TextSetting('PKEY', portalConnectorKey, title, kMaxConnectorKeyLength, &Connector::ConnectorKeyFilter));
	}

	return (nullptr);
}

void TeleporterProperty::SetSetting(const Setting *setting)
{
	if (setting->GetSettingIdentifier() == 'DKEY')
	{
		portalConnectorKey = static_cast<const TextSetting *>(setting)->GetText();
	}
}


JumpProperty::JumpProperty() : Property(kPropertyJump)
{
	jumpHeight = 1.0F;
	destinationConnectorKey = "Jump";
}

JumpProperty::JumpProperty(const JumpProperty& jumpProperty) : Property(jumpProperty)
{
	jumpHeight = jumpProperty.jumpHeight;
	destinationConnectorKey = jumpProperty.destinationConnectorKey;
}

JumpProperty::~JumpProperty()
{
}

Property *JumpProperty::Replicate(void) const
{
	return (new JumpProperty(*this));
}

bool JumpProperty::ValidNode(const Node *node)
{
	return (node->GetNodeType() == kNodeGeometry);
}

void JumpProperty::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Property::Pack(data, packFlags);

	data << ChunkHeader('HITE', 4);
	data << jumpHeight;

	PackHandle handle = data.BeginChunk('DKEY');
	data << destinationConnectorKey;
	data.EndChunk(handle);

	data << TerminatorChunk;
}

void JumpProperty::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Property::Unpack(data, unpackFlags);
	UnpackChunkList<JumpProperty>(data, unpackFlags);
}

bool JumpProperty::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'HITE':

			data >> jumpHeight;
			return (true);

		case 'DKEY':

			data >> destinationConnectorKey;
			return (true);
	}

	return (false);
}

int32 JumpProperty::GetSettingCount(void) const
{
	return (2);
}

Setting *JumpProperty::GetSetting(int32 index) const
{
	const StringTable *table = TheGame->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('PROP', kPropertyJump, 'HITE'));
		return (new TextSetting('HITE', jumpHeight, title));
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('PROP', kPropertyJump, 'DKEY'));
		return (new TextSetting('DKEY', destinationConnectorKey, title, kMaxConnectorKeyLength, &Connector::ConnectorKeyFilter));
	}

	return (nullptr);
}

void JumpProperty::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'HITE')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		jumpHeight = Text::StringToFloat(text);
	}
	else if (identifier == 'DKEY')
	{
		destinationConnectorKey = static_cast<const TextSetting *>(setting)->GetText();
	}
}

// ZYUQURM
