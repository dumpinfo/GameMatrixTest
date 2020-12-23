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


#include "MGScripts.h"
#include "MGFighter.h"
#include "MGPistol.h"
#include "MGShotgun.h"
#include "MGCrossbow.h"
#include "MGSpikeShooter.h"
#include "MGGrenadeLauncher.h"
#include "MGQuantumCharger.h"
#include "MGRocketLauncher.h"
#include "MGPlasmaGun.h"
#include "MGProtonCannon.h"
#include "MGCatFlinger.h"
#include "MGChickenBlaster.h"
#include "MGBeerCannon.h"
#include "MGHornetGun.h"
#include "MGPowers.h"
#include "MGTreasure.h"
#include "MGGame.h"


using namespace C4;


DisplayMessageMethod::DisplayMessageMethod() : Method(kMethodDisplayMessage)
{
	messageText[0] = 0;
}

DisplayMessageMethod::DisplayMessageMethod(const char *message) : Method(kMethodDisplayMessage)
{
	messageText = message;
}

DisplayMessageMethod::DisplayMessageMethod(const DisplayMessageMethod& displayMessageMethod) : Method(displayMessageMethod)
{
	messageText = displayMessageMethod.messageText;
}

DisplayMessageMethod::~DisplayMessageMethod()
{
}

Method *DisplayMessageMethod::Replicate(void) const
{
	return (new DisplayMessageMethod(*this));
}

void DisplayMessageMethod::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Method::Pack(data, packFlags);

	data << messageText;
}

void DisplayMessageMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Method::Unpack(data, unpackFlags);

	data >> messageText;
}

int32 DisplayMessageMethod::GetSettingCount(void) const
{
	return (1);
}

Setting *DisplayMessageMethod::GetSetting(int32 index) const
{
	const StringTable *table = TheGame->GetStringTable();
	const char *title = table->GetString(StringID('MTHD', kMethodDisplayMessage, 'TEXT'));
	return (new TextSetting('TEXT', messageText, title, kMaxMessageLength));
}

void DisplayMessageMethod::SetSetting(const Setting *setting)
{
	if (setting->GetSettingIdentifier() == 'TEXT')
	{
		messageText = static_cast<const TextSetting *>(setting)->GetText();
	}
}

void DisplayMessageMethod::Execute(const ScriptState *state)
{
	TheMessageBoard->AddText(messageText);
	CallCompletionProc();
}


DisplayActionMethod::DisplayActionMethod() : Method(kMethodDisplayAction)
{
	actionType = 0;
	displayTime = 5000;
}

DisplayActionMethod::DisplayActionMethod(ActionType type, int32 time) : Method(kMethodDisplayAction)
{
	actionType = type;
	displayTime = time;
}

DisplayActionMethod::DisplayActionMethod(const DisplayActionMethod& displayActionMethod) : Method(displayActionMethod) 
{
	actionType = displayActionMethod.actionType;
	displayTime = displayActionMethod.displayTime; 
}
 
DisplayActionMethod::~DisplayActionMethod()
{
} 

Method *DisplayActionMethod::Replicate(void) const 
{ 
	return (new DisplayActionMethod(*this));
}

void DisplayActionMethod::Pack(Packer& data, unsigned_int32 packFlags) const 
{
	Method::Pack(data, packFlags);

	data << actionType;
	data << displayTime;
}

void DisplayActionMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Method::Unpack(data, unpackFlags);

	data >> actionType;
	data >> displayTime;
}

int32 DisplayActionMethod::GetSettingCount(void) const
{
	return (2);
}

Setting *DisplayActionMethod::GetSetting(int32 index) const
{
	const StringTable *table = TheGame->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('MTHD', kMethodDisplayAction, 'TYPE'));
		return (new TextSetting('TYPE', Text::TypeToString(actionType), title, 4));
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('MTHD', kMethodDisplayAction, 'TIME'));
		return (new TextSetting('TIME', (float) displayTime * 0.001F, title));
	}

	return (nullptr);
}

void DisplayActionMethod::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'TYPE')
	{
		actionType = Text::StringToType(static_cast<const TextSetting *>(setting)->GetText());
	}
	else if (identifier == 'TIME')
	{
		displayTime = (int32) (Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText()) * 1000.0F);
	}
}

void DisplayActionMethod::Execute(const ScriptState *state)
{
	TheActionBoard->DisplayAction(actionType, displayTime);
	CallCompletionProc();
}


ActivateFlashlightMethod::ActivateFlashlightMethod() : Method(kMethodActivateFlashlight)
{
}

ActivateFlashlightMethod::ActivateFlashlightMethod(const ActivateFlashlightMethod& activateFlashlightMethod) : Method(activateFlashlightMethod)
{
}

ActivateFlashlightMethod::~ActivateFlashlightMethod()
{
}

Method *ActivateFlashlightMethod::Replicate(void) const
{
	return (new ActivateFlashlightMethod(*this));
}

void ActivateFlashlightMethod::Execute(const ScriptState *state)
{
	Node *node = GetTargetNode(state);
	if (node)
	{
		Controller *controller = node->GetController();
		if ((controller) && (controller->GetBaseControllerType() == kControllerRigidBody))
		{
			RigidBodyController *rigidBody = static_cast<RigidBodyController *>(controller);
			if (rigidBody->GetRigidBodyType() == kRigidBodyCharacter)
			{
				GameCharacterController *character = static_cast<GameCharacterController *>(rigidBody);
				if (character->GetCharacterType() == kCharacterPlayer)
				{
					static_cast<FighterController *>(character)->ActivateFlashlight();
				}
			}
		}
	}

	CallCompletionProc();
}


DeactivateFlashlightMethod::DeactivateFlashlightMethod() : Method(kMethodDeactivateFlashlight)
{
}

DeactivateFlashlightMethod::DeactivateFlashlightMethod(const DeactivateFlashlightMethod& deactivateFlashlightMethod) : Method(deactivateFlashlightMethod)
{
}

DeactivateFlashlightMethod::~DeactivateFlashlightMethod()
{
}

Method *DeactivateFlashlightMethod::Replicate(void) const
{
	return (new DeactivateFlashlightMethod(*this));
}

void DeactivateFlashlightMethod::Execute(const ScriptState *state)
{
	Node *node = GetTargetNode(state);
	if (node)
	{
		Controller *controller = node->GetController();
		if ((controller) && (controller->GetBaseControllerType() == kControllerRigidBody))
		{
			RigidBodyController *rigidBody = static_cast<RigidBodyController *>(controller);
			if (rigidBody->GetRigidBodyType() == kRigidBodyCharacter)
			{
				GameCharacterController *character = static_cast<GameCharacterController *>(rigidBody);
				if (character->GetCharacterType() == kCharacterPlayer)
				{
					static_cast<FighterController *>(character)->DeactivateFlashlight();
				}
			}
		}
	}

	CallCompletionProc();
}


GetFlashlightStateMethod::GetFlashlightStateMethod() : Method(kMethodGetFlashlightState)
{
}

GetFlashlightStateMethod::GetFlashlightStateMethod(const GetFlashlightStateMethod& getFlashlightStateMethod) : Method(getFlashlightStateMethod)
{
}

GetFlashlightStateMethod::~GetFlashlightStateMethod()
{
}

Method *GetFlashlightStateMethod::Replicate(void) const
{
	return (new GetFlashlightStateMethod(*this));
}

void GetFlashlightStateMethod::Execute(const ScriptState *state)
{
	bool active = false;

	Node *node = GetTargetNode(state);
	if (node)
	{
		Controller *controller = node->GetController();
		if ((controller) && (controller->GetBaseControllerType() == kControllerRigidBody))
		{
			RigidBodyController *rigidBody = static_cast<RigidBodyController *>(controller);
			if (rigidBody->GetRigidBodyType() == kRigidBodyCharacter)
			{
				GameCharacterController *character = static_cast<GameCharacterController *>(rigidBody);
				if (character->GetCharacterType() == kCharacterPlayer)
				{
					active = (static_cast<FighterController *>(character)->GetFlashlight() != nullptr);
				}
			}
		}
	}

	SetOutputValue(state, active);
	CallCompletionProc();
}


GiveWeaponMethod::GiveWeaponMethod() : Method(kMethodGiveWeapon)
{
	weaponType = kModelPistol;
}

GiveWeaponMethod::GiveWeaponMethod(const GiveWeaponMethod& giveWeaponMethod) : Method(giveWeaponMethod)
{
	weaponType = giveWeaponMethod.weaponType;
}

GiveWeaponMethod::~GiveWeaponMethod()
{
}

Method *GiveWeaponMethod::Replicate(void) const
{
	return (new GiveWeaponMethod(*this));
}

void GiveWeaponMethod::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Method::Pack(data, packFlags);

	data << weaponType;
}

void GiveWeaponMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Method::Unpack(data, unpackFlags);

	data >> weaponType;
}

int32 GiveWeaponMethod::GetSettingCount(void) const
{
	return (1);
}

Setting *GiveWeaponMethod::GetSetting(int32 index) const
{
	const StringTable *table = TheGame->GetStringTable();
	const char *title = table->GetString(StringID('MTHD', kMethodGiveWeapon, 'TYPE'));
	MenuSetting *menu = new MenuSetting('TYPE', MaxZero(Weapon::WeaponTypeToIndex(weaponType) - kWeaponPistol), title, kWeaponCount - kWeaponPistol);
	for (machine a = kWeaponPistol; a < kWeaponCount; a++)
	{
		menu->SetMenuItemString(a - kWeaponPistol, table->GetString(StringID('MODL', 'WEAP', Weapon::WeaponIndexToType(a))));
	}

	return (menu);
}

void GiveWeaponMethod::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'TYPE')
	{
		weaponType = Weapon::WeaponIndexToType(static_cast<const MenuSetting *>(setting)->GetMenuSelection() + kWeaponPistol);
	}
}

void GiveWeaponMethod::Execute(const ScriptState *state)
{
	Node *node = GetTargetNode(state);
	if (node)
	{
		Controller *controller = node->GetController();
		if ((controller) && (controller->GetBaseControllerType() == kControllerRigidBody))
		{
			RigidBodyController *rigidBody = static_cast<RigidBodyController *>(controller);
			if (rigidBody->GetRigidBodyType() == kRigidBodyCharacter)
			{
				GameCharacterController *character = static_cast<GameCharacterController *>(rigidBody);
				if (character->GetCharacterType() == kCharacterPlayer)
				{
					GamePlayer *player = static_cast<FighterController *>(character)->GetFighterPlayer();
					player->AcquireWeapon(Weapon::WeaponTypeToIndex(weaponType));
				}
			}
		}
	}

	CallCompletionProc();
}


GiveAmmoMethod::GiveAmmoMethod() : Method(kMethodGiveAmmo)
{
	weaponType = kModelPistol;
	ammoCount = 25;
	ammoIndex = 0;
}

GiveAmmoMethod::GiveAmmoMethod(const GiveAmmoMethod& giveAmmoMethod) : Method(giveAmmoMethod)
{
	weaponType = giveAmmoMethod.weaponType;
	ammoCount = giveAmmoMethod.ammoCount;
	ammoIndex = giveAmmoMethod.ammoIndex;
}

GiveAmmoMethod::~GiveAmmoMethod()
{
}

Method *GiveAmmoMethod::Replicate(void) const
{
	return (new GiveAmmoMethod(*this));
}

void GiveAmmoMethod::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Method::Pack(data, packFlags);

	data << weaponType;
	data << ammoCount;
	data << ammoIndex;
}

void GiveAmmoMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Method::Unpack(data, unpackFlags);

	data >> weaponType;
	data >> ammoCount;
	data >> ammoIndex;
}

int32 GiveAmmoMethod::GetSettingCount(void) const
{
	return (3);
}

Setting *GiveAmmoMethod::GetSetting(int32 index) const
{
	const StringTable *table = TheGame->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('MTHD', kMethodGiveAmmo, 'TYPE'));
		MenuSetting *menu = new MenuSetting('TYPE', MaxZero(Weapon::WeaponTypeToIndex(weaponType) - kWeaponPistol), title, kWeaponCount - kWeaponPistol);
		for (machine a = kWeaponPistol; a < kWeaponCount; a++)
		{
			menu->SetMenuItemString(a - kWeaponPistol, table->GetString(StringID('MODL', 'WEAP', Weapon::WeaponIndexToType(a))));
		}

		return (menu);
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('MTHD', kMethodGiveAmmo, 'AMMO'));
		return (new TextSetting('AMMO', String<7>(ammoCount), title, 7, &EditTextWidget::NumberFilter));
	}

	if (index == 2)
	{
		const char *title = table->GetString(StringID('MTHD', kMethodGiveAmmo, 'SCND'));
		return (new BooleanSetting('SCND', (ammoIndex != 0), title));
	}

	return (nullptr);
}

void GiveAmmoMethod::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'TYPE')
	{
		weaponType = Weapon::WeaponIndexToType(static_cast<const MenuSetting *>(setting)->GetMenuSelection() + kWeaponPistol);
	}
	else if (identifier == 'AMMO')
	{
		ammoCount = Text::StringToInteger(static_cast<const TextSetting *>(setting)->GetText());
	}
	else if (identifier == 'SCND')
	{
		ammoIndex = static_cast<const BooleanSetting *>(setting)->GetBooleanValue();
	}
}

void GiveAmmoMethod::Execute(const ScriptState *state)
{
	Node *node = GetTargetNode(state);
	if (node)
	{
		Controller *controller = node->GetController();
		if ((controller) && (controller->GetBaseControllerType() == kControllerRigidBody))
		{
			RigidBodyController *rigidBody = static_cast<RigidBodyController *>(controller);
			if (rigidBody->GetRigidBodyType() == kRigidBodyCharacter)
			{
				GameCharacterController *character = static_cast<GameCharacterController *>(rigidBody);
				if (character->GetCharacterType() == kCharacterPlayer)
				{
					int32 weaponIndex = Weapon::WeaponTypeToIndex(weaponType);
					const Weapon *weapon = Weapon::Get(weaponIndex);
					if (weapon)
					{
						GamePlayer *player = static_cast<FighterController *>(character)->GetFighterPlayer();
						int32 ammo = MaxZero(Min(player->GetWeaponAmmo(weaponIndex, ammoIndex) + ammoCount, weapon->GetMaxWeaponAmmo(ammoIndex)));
						player->SetWeaponAmmo(weaponIndex, ammoIndex, ammo);
					}
				}
			}
		}
	}

	CallCompletionProc();
}


GivePowerMethod::GivePowerMethod() : Method(kMethodGivePower)
{
	powerType = kModelCandyPower;
}

GivePowerMethod::GivePowerMethod(const GivePowerMethod& givePowerMethod) : Method(givePowerMethod)
{
	powerType = givePowerMethod.powerType;
}

GivePowerMethod::~GivePowerMethod()
{
}

Method *GivePowerMethod::Replicate(void) const
{
	return (new GivePowerMethod(*this));
}

void GivePowerMethod::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Method::Pack(data, packFlags);

	data << powerType;
}

void GivePowerMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Method::Unpack(data, unpackFlags);

	data >> powerType;
}

int32 GivePowerMethod::GetSettingCount(void) const
{
	return (1);
}

Setting *GivePowerMethod::GetSetting(int32 index) const
{
	const StringTable *table = TheGame->GetStringTable();
	const char *title = table->GetString(StringID('MTHD', kMethodGivePower, 'TYPE'));
	MenuSetting *menu = new MenuSetting('TYPE', MaxZero(Power::PowerTypeToIndex(powerType) - 1), title, kPowerCount - 1);
	for (machine a = 1; a < kPowerCount; a++)
	{
		menu->SetMenuItemString(a - 1, table->GetString(StringID('MTHD', kMethodGivePower, 'TYPE', Power::PowerIndexToType(a))));
	}

	return (menu);
}

void GivePowerMethod::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'TYPE')
	{
		powerType = Power::PowerIndexToType(static_cast<const MenuSetting *>(setting)->GetMenuSelection() + 1);
	}
}

void GivePowerMethod::Execute(const ScriptState *state)
{
	Node *node = GetTargetNode(state);
	if (node)
	{
		Controller *controller = node->GetController();
		if ((controller) && (controller->GetBaseControllerType() == kControllerRigidBody))
		{
			RigidBodyController *rigidBody = static_cast<RigidBodyController *>(controller);
			if (rigidBody->GetRigidBodyType() == kRigidBodyCharacter)
			{
				GameCharacterController *character = static_cast<GameCharacterController *>(rigidBody);
				if (character->GetCharacterType() == kCharacterPlayer)
				{
					//GamePlayer *player = static_cast<FighterController *>(character)->GetFighterPlayer();
					// not implemented
				}
			}
		}
	}

	CallCompletionProc();
}


GiveTreasureMethod::GiveTreasureMethod() : Method(kMethodGiveTreasure)
{
	treasureType = kModelGoldCoinTreasure;
	treasureCount = 1;
}

GiveTreasureMethod::GiveTreasureMethod(const GiveTreasureMethod& giveTreasureMethod) : Method(giveTreasureMethod)
{
	treasureType = giveTreasureMethod.treasureType;
	treasureCount = giveTreasureMethod.treasureCount;
}

GiveTreasureMethod::~GiveTreasureMethod()
{
}

Method *GiveTreasureMethod::Replicate(void) const
{
	return (new GiveTreasureMethod(*this));
}

void GiveTreasureMethod::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Method::Pack(data, packFlags);

	data << treasureType;
	data << treasureCount;
}

void GiveTreasureMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Method::Unpack(data, unpackFlags);

	data >> treasureType;
	data >> treasureCount;
}

int32 GiveTreasureMethod::GetSettingCount(void) const
{
	return (2);
}

Setting *GiveTreasureMethod::GetSetting(int32 index) const
{
	const StringTable *table = TheGame->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('MTHD', kMethodGiveTreasure, 'TYPE'));
		MenuSetting *menu = new MenuSetting('TYPE', MaxZero(Treasure::TreasureTypeToIndex(treasureType) - 1), title, kTreasureCount - 1);
		for (machine a = 1; a < kTreasureCount; a++)
		{
			menu->SetMenuItemString(a - 1, table->GetString(StringID('MTHD', kMethodGiveTreasure, 'TYPE', Treasure::TreasureIndexToType(a))));
		}

		return (menu);
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('MTHD', kMethodGiveTreasure, 'QUAN'));
		return (new TextSetting('QUAN', Text::IntegerToString(treasureCount), title, 4, &EditTextWidget::NumberFilter));
	}

	return (nullptr);
}

void GiveTreasureMethod::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'TYPE')
	{
		treasureType = Treasure::TreasureIndexToType(static_cast<const MenuSetting *>(setting)->GetMenuSelection() + 1);
	}
	else if (identifier == 'QUAN')
	{
		treasureCount = Max(Text::StringToInteger(static_cast<const TextSetting *>(setting)->GetText()), 1);
	}
}

void GiveTreasureMethod::Execute(const ScriptState *state)
{
	Node *node = GetTargetNode(state);
	if (node)
	{
		Controller *controller = node->GetController();
		if ((controller) && (controller->GetBaseControllerType() == kControllerRigidBody))
		{
			RigidBodyController *rigidBody = static_cast<RigidBodyController *>(controller);
			if (rigidBody->GetRigidBodyType() == kRigidBodyCharacter)
			{
				GameCharacterController *character = static_cast<GameCharacterController *>(rigidBody);
				if (character->GetCharacterType() == kCharacterPlayer)
				{
					GamePlayer *player = static_cast<FighterController *>(character)->GetFighterPlayer();
					player->AddPlayerTreasure(Treasure::TreasureTypeToIndex(treasureType), treasureCount);
				}
			}
		}
	}

	CallCompletionProc();
}


GiveHealthMethod::GiveHealthMethod() : Method(kMethodGiveHealth)
{
	healthCount = 25 << 16;
	maxHealth = 100 << 16;
}

GiveHealthMethod::GiveHealthMethod(const GiveHealthMethod& giveHealthMethod) : Method(giveHealthMethod)
{
	healthCount = giveHealthMethod.healthCount;
	maxHealth = giveHealthMethod.maxHealth;
}

GiveHealthMethod::~GiveHealthMethod()
{
}

Method *GiveHealthMethod::Replicate(void) const
{
	return (new GiveHealthMethod(*this));
}

void GiveHealthMethod::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Method::Pack(data, packFlags);

	data << healthCount;
	data << maxHealth;
}

void GiveHealthMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Method::Unpack(data, unpackFlags);

	data >> healthCount;
	data >> maxHealth;
}

int32 GiveHealthMethod::GetSettingCount(void) const
{
	return (2);
}

Setting *GiveHealthMethod::GetSetting(int32 index) const
{
	const StringTable *table = TheGame->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('MTHD', kMethodGiveHealth, 'HLTH'));
		return (new TextSetting('HLTH', String<7>(healthCount >> 16), title, 7, &EditTextWidget::NumberFilter));
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('MTHD', kMethodGiveHealth, 'OVER'));
		return (new BooleanSetting('OVER', (maxHealth != (100 << 16)), title));
	}

	return (nullptr);
}

void GiveHealthMethod::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'HLTH')
	{
		healthCount = Text::StringToInteger(static_cast<const TextSetting *>(setting)->GetText()) << 16;
	}
	else if (identifier == 'OVER')
	{
		maxHealth = (static_cast<const BooleanSetting *>(setting)->GetBooleanValue()) ? 200 << 16 : 100 << 16;
	}
}

void GiveHealthMethod::Execute(const ScriptState *state)
{
	Node *node = GetTargetNode(state);
	if (node)
	{
		Controller *controller = node->GetController();
		if ((controller) && (controller->GetBaseControllerType() == kControllerRigidBody))
		{
			RigidBodyController *rigidBody = static_cast<RigidBodyController *>(controller);
			if (rigidBody->GetRigidBodyType() == kRigidBodyCharacter)
			{
				GameCharacterController *character = static_cast<GameCharacterController *>(rigidBody);
				if (character->GetCharacterType() == kCharacterPlayer)
				{
					GamePlayer *player = static_cast<FighterController *>(character)->GetFighterPlayer();

					Fixed health = player->GetPlayerHealth();
					if (health < maxHealth)
					{
						player->SetPlayerHealth(Min(health + healthCount, maxHealth));
						SetOutputValue(state, true);
					}
					else
					{
						SetOutputValue(state, false);
					}
				}
			}
		}
	}

	CallCompletionProc();
}


GiveScoreMethod::GiveScoreMethod() : Method(kMethodGiveScore)
{
	scoreCount = 0;
}

GiveScoreMethod::GiveScoreMethod(const GiveScoreMethod& giveScoreMethod) : Method(giveScoreMethod)
{
	scoreCount = giveScoreMethod.scoreCount;
}

GiveScoreMethod::~GiveScoreMethod()
{
}

Method *GiveScoreMethod::Replicate(void) const
{
	return (new GiveScoreMethod(*this));
}

void GiveScoreMethod::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Method::Pack(data, packFlags);

	data << scoreCount;
}

void GiveScoreMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Method::Unpack(data, unpackFlags);

	data >> scoreCount;
}

int32 GiveScoreMethod::GetSettingCount(void) const
{
	return (1);
}

Setting *GiveScoreMethod::GetSetting(int32 index) const
{
	const StringTable *table = TheGame->GetStringTable();
	const char *title = table->GetString(StringID('MTHD', kMethodGiveScore, 'SCOR'));
	return (new TextSetting('SCOR', String<7>(scoreCount >> 16), title, 7, &EditTextWidget::NumberFilter));
}

void GiveScoreMethod::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'SCOR')
	{
		scoreCount = Min(Text::StringToInteger(static_cast<const TextSetting *>(setting)->GetText()), 32767) << 16;
	}
}

void GiveScoreMethod::Execute(const ScriptState *state)
{
	Node *node = GetTargetNode(state);
	if (node)
	{
		Controller *controller = node->GetController();
		if ((controller) && (controller->GetBaseControllerType() == kControllerRigidBody))
		{
			RigidBodyController *rigidBody = static_cast<RigidBodyController *>(controller);
			if (rigidBody->GetRigidBodyType() == kRigidBodyCharacter)
			{
				GameCharacterController *character = static_cast<GameCharacterController *>(rigidBody);
				if (character->GetCharacterType() == kCharacterPlayer)
				{
					GamePlayer *player = static_cast<FighterController *>(character)->GetFighterPlayer();
					player->AddPlayerScore(scoreCount);
				}
			}
		}
	}

	CallCompletionProc();
}


DamageCharacterMethod::DamageCharacterMethod() : Method(kMethodDamageCharacter)
{
	damageCount = 25 << 16;
	damageFlags = 0;
}

DamageCharacterMethod::DamageCharacterMethod(const DamageCharacterMethod& damageCharacterMethod) : Method(damageCharacterMethod)
{
	damageCount = damageCharacterMethod.damageCount;
	damageFlags = damageCharacterMethod.damageFlags;
}

DamageCharacterMethod::~DamageCharacterMethod()
{
}

Method *DamageCharacterMethod::Replicate(void) const
{
	return (new DamageCharacterMethod(*this));
}

void DamageCharacterMethod::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Method::Pack(data, packFlags);

	data << damageCount;
	data << damageFlags;
}

void DamageCharacterMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Method::Unpack(data, unpackFlags);

	data >> damageCount;
	data >> damageFlags;
}

int32 DamageCharacterMethod::GetSettingCount(void) const
{
	return (2);
}

Setting *DamageCharacterMethod::GetSetting(int32 index) const
{
	const StringTable *table = TheGame->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('MTHD', kMethodDamageCharacter, 'DAMG'));
		return (new TextSetting('DAMG', String<7>(damageCount >> 16), title, 7, &EditTextWidget::NumberFilter));
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('MTHD', kMethodDamageCharacter, 'BLOD'));
		return (new BooleanSetting('BLOD', ((damageFlags & kDamageBloodInhibit) != 0), title));
	}

	return (nullptr);
}

void DamageCharacterMethod::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'DAMG')
	{
		damageCount = Text::StringToInteger(static_cast<const TextSetting *>(setting)->GetText()) << 16;
	}
	else if (identifier == 'BLOD')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			damageFlags |= kDamageBloodInhibit;
		}
		else
		{
			damageFlags &= ~kDamageBloodInhibit;
		}
	}
}

void DamageCharacterMethod::Execute(const ScriptState *state)
{
	Node *node = GetTargetNode(state);
	if (node)
	{
		Controller *controller = node->GetController();
		if ((controller) && (controller->GetBaseControllerType() == kControllerRigidBody))
		{
			RigidBodyController *rigidBody = static_cast<RigidBodyController *>(controller);
			if (rigidBody->GetRigidBodyType() == kRigidBodyCharacter)
			{
				static_cast<GameCharacterController *>(rigidBody)->Damage(damageCount, damageFlags, nullptr);
			}
		}
	}

	CallCompletionProc();
}


KillCharacterMethod::KillCharacterMethod() : Method(kMethodKillCharacter)
{
}

KillCharacterMethod::KillCharacterMethod(const KillCharacterMethod& killCharacterMethod) : Method(killCharacterMethod)
{
}

KillCharacterMethod::~KillCharacterMethod()
{
}

Method *KillCharacterMethod::Replicate(void) const
{
	return (new KillCharacterMethod(*this));
}

void KillCharacterMethod::Execute(const ScriptState *state)
{
	Node *node = GetTargetNode(state);
	if (node)
	{
		Controller *controller = node->GetController();
		if ((controller) && (controller->GetBaseControllerType() == kControllerRigidBody))
		{
			RigidBodyController *rigidBody = static_cast<RigidBodyController *>(controller);
			if (rigidBody->GetRigidBodyType() == kRigidBodyCharacter)
			{
				static_cast<GameCharacterController *>(rigidBody)->Kill(nullptr);
			}
		}
	}

	CallCompletionProc();
}


KillAllMonstersMethod::KillAllMonstersMethod() : Method(kMethodKillAllMonsters)
{
}

KillAllMonstersMethod::KillAllMonstersMethod(const KillAllMonstersMethod& killAllMonstersMethod) : Method(killAllMonstersMethod)
{
}

KillAllMonstersMethod::~KillAllMonstersMethod()
{
}

Method *KillAllMonstersMethod::Replicate(void) const
{
	return (new KillAllMonstersMethod(*this));
}

void KillAllMonstersMethod::Execute(const ScriptState *state)
{
	const World *world = state->GetScriptController()->GetTargetNode()->GetWorld();
	const PhysicsController *physicsController = world->FindPhysicsController();

	if (physicsController)
	{
		Body *body = physicsController->GetFirstBody();
		while (body)
		{
			Body *next = body->GetNextElement();

			if (body->GetBodyType() == kBodyRigid)
			{
				RigidBodyController *rigidBody = static_cast<RigidBodyController *>(body);
				if ((!rigidBody->Asleep()) && (rigidBody->GetRigidBodyType() == kRigidBodyCharacter))
				{
					GameCharacterController *character = static_cast<GameCharacterController *>(rigidBody);
					if (character->GetCharacterType() == kCharacterMonster)
					{
						delete character->GetTargetNode();
					}
				}
			}

			body = next;
		}
	}

	CallCompletionProc();
}


TeleportPlayerMethod::TeleportPlayerMethod() : Method(kMethodTeleportPlayer)
{
	destinationConnectorKey[0] = 0;
}

TeleportPlayerMethod::TeleportPlayerMethod(const TeleportPlayerMethod& teleportPlayerMethod) : Method(teleportPlayerMethod)
{
	destinationConnectorKey = teleportPlayerMethod.destinationConnectorKey;
}

TeleportPlayerMethod::~TeleportPlayerMethod()
{
}

Method *TeleportPlayerMethod::Replicate(void) const
{
	return (new TeleportPlayerMethod(*this));
}

void TeleportPlayerMethod::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Method::Pack(data, packFlags);

	data << destinationConnectorKey;
}

void TeleportPlayerMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Method::Unpack(data, unpackFlags);

	data >> destinationConnectorKey;
}

int32 TeleportPlayerMethod::GetSettingCount(void) const
{
	return (1);
}

Setting *TeleportPlayerMethod::GetSetting(int32 index) const
{
	const StringTable *table = TheGame->GetStringTable();
	const char *title = table->GetString(StringID('MTHD', kMethodTeleportPlayer, 'DKEY'));
	return (new TextSetting('DKEY', destinationConnectorKey, title, kMaxConnectorKeyLength, &Connector::ConnectorKeyFilter));
}

void TeleportPlayerMethod::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'DKEY')
	{
		destinationConnectorKey = static_cast<const TextSetting *>(setting)->GetText();
	}
}

void TeleportPlayerMethod::Execute(const ScriptState *state)
{
	Node *node = GetTargetNode(state);
	if (node)
	{
		Controller *controller = node->GetController();
		if ((controller) && (controller->GetBaseControllerType() == kControllerRigidBody))
		{
			RigidBodyController *rigidBody = static_cast<RigidBodyController *>(controller);
			if (rigidBody->GetRigidBodyType() == kRigidBodyCharacter)
			{
				GameCharacterController *character = static_cast<GameCharacterController *>(rigidBody);
				if (character->GetCharacterType() == kCharacterPlayer)
				{
					const Node *destinationNode = state->GetScriptControllerTarget()->GetConnectedNode(destinationConnectorKey);
					if (destinationNode)
					{
						FighterController *fighter = static_cast<FighterController *>(character);

						const Vector3D& direction = destinationNode->GetWorldTransform()[0];
						float azm = Atan(direction.y, direction.x);
						float alt = Atan(direction.z);

						const Point3D& position = destinationNode->GetWorldPosition();
						Point3D center = fighter->GetFinalPosition();
						center.z += 1.0F;

						TheMessageMgr->SendMessageAll(FighterTeleportMessage(fighter->GetControllerIndex(), position, Zero3D, azm, alt, center));
					}
				}
			}
		}
	}

	CallCompletionProc();
}


ShakeCameraMethod::ShakeCameraMethod() : Method(kMethodShakeCamera)
{
	shakeIntensity = 0.1F;
	shakeDuration = 1000;
}

ShakeCameraMethod::ShakeCameraMethod(const ShakeCameraMethod& shakeCameraMethod) : Method(shakeCameraMethod)
{
	shakeIntensity = shakeCameraMethod.shakeIntensity;
	shakeDuration = shakeCameraMethod.shakeDuration;
}

ShakeCameraMethod::~ShakeCameraMethod()
{
}

Method *ShakeCameraMethod::Replicate(void) const
{
	return (new ShakeCameraMethod(*this));
}

void ShakeCameraMethod::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Method::Pack(data, packFlags);

	data << shakeIntensity;
	data << shakeDuration;
}

void ShakeCameraMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Method::Unpack(data, unpackFlags);

	data >> shakeIntensity;
	data >> shakeDuration;
}

int32 ShakeCameraMethod::GetSettingCount(void) const
{
	return (2);
}

Setting *ShakeCameraMethod::GetSetting(int32 index) const
{
	const StringTable *table = TheGame->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('MTHD', kMethodShakeCamera, 'INTS'));
		return (new TextSetting('INTS', shakeIntensity, title));
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('MTHD', kMethodShakeCamera, 'DURA'));
		return (new TextSetting('DURA', (float) shakeDuration * 0.001F, title));
	}

	return (nullptr);
}

void ShakeCameraMethod::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'INTS')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		shakeIntensity = FmaxZero(Text::StringToFloat(text));
	}
	else if (identifier == 'DURA')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		shakeDuration = MaxZero((int32) (Text::StringToFloat(text) * 1000.0F + 0.5F));
	}
}

void ShakeCameraMethod::Execute(const ScriptState *state)
{
	static_cast<GameWorld *>(state->GetScriptController()->GetTargetNode()->GetWorld())->ShakeCamera(shakeIntensity, shakeDuration);
	CallCompletionProc();
}


EndWorldMethod::EndWorldMethod() : Method(kMethodEndWorld)
{
	nextWorldName[0] = 0;
}

EndWorldMethod::EndWorldMethod(const EndWorldMethod& endWorldMethod) : Method(endWorldMethod)
{
	nextWorldName = endWorldMethod.nextWorldName;
}

EndWorldMethod::~EndWorldMethod()
{
}

Method *EndWorldMethod::Replicate(void) const
{
	return (new EndWorldMethod(*this));
}

void EndWorldMethod::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Method::Pack(data, packFlags);

	data << nextWorldName;
}

void EndWorldMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Method::Unpack(data, unpackFlags);

	data >> nextWorldName;
}

int32 EndWorldMethod::GetSettingCount(void) const
{
	return (1);
}

Setting *EndWorldMethod::GetSetting(int32 index) const
{
	const StringTable *table = TheGame->GetStringTable();
	const char *title = table->GetString(StringID('MTHD', kMethodEndWorld, 'WNAM'));
	const char *picker = table->GetString(StringID('MTHD', kMethodEndWorld, 'PICK'));
	return (new ResourceSetting('WNAM', nextWorldName, title, picker, WorldResource::GetDescriptor()));
}

void EndWorldMethod::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'WNAM')
	{
		nextWorldName = static_cast<const TextSetting *>(setting)->GetText();
	}
}

void EndWorldMethod::Execute(const ScriptState *state)
{
	TheGame->AddWindow(new SummaryWindow(nextWorldName));
	CallCompletionProc();
}

// ZYUQURM
