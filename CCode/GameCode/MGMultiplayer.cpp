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


#include "MGMultiplayer.h"
#include "MGGame.h"
#include "MGGusGraves.h"
#include "MGSoldier.h"
#include "MGShotgun.h"
#include "MGCrossbow.h"
#include "MGSpikeShooter.h"
#include "MGGrenadeLauncher.h"
#include "MGQuantumCharger.h"
#include "MGRocketLauncher.h"
#include "MGPlasmaGun.h"
#include "MGProtonCannon.h"
#include "MGCatFlinger.h"
#include "MGSpider.h"
#include "MGSkeleton.h"
#include "MGFireball.h"
#include "MGTarball.h"
#include "MGLavaball.h"
#include "MGVenom.h"
#include "MGBlackCat.h"


using namespace C4;


namespace
{
	enum
	{
		kScoreUpdateInterval			= 150,
		kScoreboardRefreshInterval		= 2000,
		kPlayerRespawnInterval			= 1500,
		kProtectionShieldInterval		= 4000
	};
}


ServerInfoMessage::ServerInfoMessage() : Message(kMessageServerInfo)
{
}

ServerInfoMessage::ServerInfoMessage(int32 numPlayers, int32 maxPlayers, const char *game, const char *world) : Message(kMessageServerInfo)
{
	playerCount = numPlayers;
	maxPlayerCount = maxPlayers;

	gameName = game;
	worldName = world;
}

ServerInfoMessage::~ServerInfoMessage()
{
}

void ServerInfoMessage::Compress(Compressor& data) const
{
	data << (unsigned_int16) playerCount;
	data << (unsigned_int16) maxPlayerCount;
	data << gameName;
	data << worldName;
}

bool ServerInfoMessage::Decompress(Decompressor& data)
{
	unsigned_int16	count, max;

	data >> count;
	data >> max;

	if (count > max)
	{
		return (false);
	}

	playerCount = count;
	maxPlayerCount = max;

	data >> gameName;
	data >> worldName;
	return (true);
}


GameInfoMessage::GameInfoMessage() : Message(kMessageGameInfo)
{
}

GameInfoMessage::GameInfoMessage(unsigned_int32 flags, const char *world) : Message(kMessageGameInfo)
{
	multiplayerFlags = flags;
	worldName = world;
}

GameInfoMessage::~GameInfoMessage()
{
}

void GameInfoMessage::Compress(Compressor& data) const
{
	data << multiplayerFlags;
	data << worldName; 
}

bool GameInfoMessage::Decompress(Decompressor& data) 
{
	data >> multiplayerFlags; 
	data >> worldName;
	return (true);
} 

bool GameInfoMessage::HandleMessage(Player *sender) const 
{ 
	if (sender->GetPlayerKey() == kPlayerServer)
	{
		TheGame->JoinMultiplayerGame(worldName, multiplayerFlags);
	} 

	return (true);
}


PlayerStyleMessage::PlayerStyleMessage() : Message(kMessagePlayerStyle)
{
}

PlayerStyleMessage::PlayerStyleMessage(int32 player, const int32 *style) : Message(kMessagePlayerStyle)
{
	playerKey = player;

	for (machine a = 0; a < kPlayerStyleCount; a++)
	{
		playerStyle[a] = style[a];
	}
}

PlayerStyleMessage::~PlayerStyleMessage()
{
}

void PlayerStyleMessage::Compress(Compressor& data) const
{
	data << (int16) playerKey;

	for (machine a = 0; a < kPlayerStyleCount; a++)
	{
		data << (char) playerStyle[a];
	}
}

bool PlayerStyleMessage::Decompress(Decompressor& data)
{
	int16	player;

	data >> player;
	playerKey = player;

	for (machine a = 0; a < kPlayerStyleCount; a++)
	{
		char	style;

		data >> style;
		playerStyle[a] = style;
	}

	return (true);
}

bool PlayerStyleMessage::HandleMessage(Player *sender) const
{
	GamePlayer *player = static_cast<GamePlayer *>(TheMessageMgr->GetPlayer(playerKey));
	if (player)
	{
		player->SetPlayerStyle(playerStyle);
	}

	return (true);
}


CreateModelMessage::CreateModelMessage(ModelMessageType type) : Message(kMessageCreateModel)
{
	modelMessageType = type;
}

CreateModelMessage::CreateModelMessage(ModelMessageType type, int32 index, const Point3D& position) : Message(kMessageCreateModel)
{
	modelMessageType = type;
	controllerIndex = index;
	initialPosition = position;
}

CreateModelMessage::~CreateModelMessage()
{
}

void CreateModelMessage::Compress(Compressor& data) const
{
	data << (char) modelMessageType;
	data << (unsigned_int16) controllerIndex;
	data << initialPosition;
}

bool CreateModelMessage::Decompress(Decompressor& data)
{
	unsigned_int16	index;

	data >> index;
	controllerIndex = index;

	data >> initialPosition;
	return (true);
}

void CreateModelMessage::InitializeModel(GameWorld *world, Model *model, Controller *controller) const
{
	controller->SetControllerIndex(controllerIndex);
	model->SetController(controller);

	model->SetNodePosition(initialPosition);
	world->AddNewNode(model);

	if (TheMessageMgr->Synchronized())
	{
		RigidBodyController *rigidBody = static_cast<RigidBodyController *>(controller);
		RigidBodyType type = rigidBody->GetRigidBodyType();

		if (type == kRigidBodyProjectile)
		{
			static_cast<ProjectileController *>(rigidBody)->EnterWorld(world, initialPosition);
		}
		else if (type == kRigidBodyCharacter)
		{
			static_cast<GameCharacterController *>(rigidBody)->EnterWorld(world, initialPosition);
		}
	}
}

CreateModelMessage *CreateModelMessage::CreateMessage(ModelMessageType type)
{
	switch (type)
	{
		case kModelMessageArrow:

			return (new CreateArrowMessage);

		case kModelMessageExplosiveArrow:

			return (new CreateExplosiveArrowMessage);

		case kModelMessageSpike:

			return (new CreateSpikeMessage);

		case kModelMessageRailSpike:

			return (new CreateRailSpikeMessage);

		case kModelMessageGrenade:

			return (new CreateGrenadeMessage);

		case kModelMessageCharge:

			return (new CreateChargeMessage);

		case kModelMessageRocket:

			return (new CreateRocketMessage);

		case kModelMessagePlasma:

			return (new CreatePlasmaMessage);

		case kModelMessageFireball:

			return (new CreateFireballMessage);

		case kModelMessageTarball:

			return (new CreateTarballMessage);

		case kModelMessageLavaball:

			return (new CreateLavaballMessage);

		case kModelMessageVenom:

			return (new CreateVenomMessage);

		case kModelMessageBlackCat:

			return (new CreateBlackCatMessage);

		case kModelMessageGusGraves:

			return (new CreateGusGravesMessage);

		case kModelMessageSoldier:

			return (new CreateSoldierMessage);
	}

	return (nullptr);
}


ScoreMessage::ScoreMessage() : Message(kMessageScore)
{
}

ScoreMessage::ScoreMessage(int32 score) : Message(kMessageScore)
{
	playerScore = score;
}

ScoreMessage::~ScoreMessage()
{
}

void ScoreMessage::Compress(Compressor& data) const
{
	data << playerScore;
}

bool ScoreMessage::Decompress(Decompressor& data)
{
	data >> playerScore;
	return (true);
}

bool ScoreMessage::HandleMessage(Player *sender) const
{
	Player *player = TheMessageMgr->GetLocalPlayer();
	if (player)
	{
		if (!TheMessageMgr->Server())
		{
			static_cast<GamePlayer *>(player)->GetPlayerState()->playerScore = playerScore;
		}

		if (TheMessageMgr->Multiplayer())
		{
			Sound *sound = new Sound;
			sound->Load("sound/Hit");
			sound->Delay(200);
		}

		if (TheDisplayBoard)
		{
			TheDisplayBoard->UpdatePlayerScore();
		}

		if (TheScoreBoard)
		{
			TheScoreBoard->Refresh();
		}
	}

	return (true);
}


HealthMessage::HealthMessage() : Message(kMessageHealth)
{
}

HealthMessage::HealthMessage(int32 health) : Message(kMessageHealth)
{
	playerHealth = health;
}

HealthMessage::~HealthMessage()
{
}

void HealthMessage::Compress(Compressor& data) const
{
	data << (unsigned_int16) playerHealth;
}

bool HealthMessage::Decompress(Decompressor& data)
{
	unsigned_int16	health;

	data >> health;
	playerHealth = health;
	return (true);
}

bool HealthMessage::HandleMessage(Player *sender) const
{
	Player *player = TheMessageMgr->GetLocalPlayer();
	if (player)
	{
		if (!TheMessageMgr->Server())
		{
			static_cast<GamePlayer *>(player)->GetPlayerState()->playerHealth = playerHealth << 16;
		}

		if (TheDisplayBoard)
		{
			TheDisplayBoard->UpdatePlayerHealth();
		}
	}

	return (true);
}


WeaponMessage::WeaponMessage() : Message(kMessageWeapon)
{
}

WeaponMessage::WeaponMessage(int32 weapon, int32 ammo1, int32 ammo2) : Message(kMessageWeapon)
{
	weaponIndex = weapon;
	weaponAmmo[0] = ammo1;
	weaponAmmo[1] = ammo2;
}

WeaponMessage::~WeaponMessage()
{
}

void WeaponMessage::Compress(Compressor& data) const
{
	data << (unsigned_int8) weaponIndex;
	data << (int16) weaponAmmo[0];
	data << (int16) weaponAmmo[1];
}

bool WeaponMessage::Decompress(Decompressor& data)
{
	unsigned_int8	index;
	int16			ammo1, ammo2;

	data >> index;
	if (index >= kWeaponCount)
	{
		return (false);
	}

	weaponIndex = index;

	data >> ammo1;
	data >> ammo2;
	weaponAmmo[0] = ammo1;
	weaponAmmo[1] = ammo2;

	return (true);
}

bool WeaponMessage::HandleMessage(Player *sender) const
{
	Player *player = TheMessageMgr->GetLocalPlayer();
	if (player)
	{
		PlayerState *state = static_cast<GamePlayer *>(player)->GetPlayerState();

		if (!TheMessageMgr->Server())
		{
			state->weaponFlags |= 1 << weaponIndex;
			state->weaponAmmo[weaponIndex][0] = weaponAmmo[0];
			state->weaponAmmo[weaponIndex][1] = weaponAmmo[1];
		}

		if ((weaponIndex < kWeaponStandardCount) && (TheEngine->GetVariable("weaponSwitch")->GetIntegerValue() != 0))
		{
			int32 currentWeapon = state->currentWeapon;
			if ((weaponIndex != currentWeapon) && (state->weaponAmmo[currentWeapon] == 0))
			{
				TheMessageMgr->SendMessage(kPlayerServer, ClientWeaponMessage(kMessageClientSwitchWeapon, weaponIndex));
			}
			else if ((weaponIndex > currentWeapon) || (TheEngine->GetVariable("weaponBetterSwitch")->GetIntegerValue() == 0))
			{
				TheMessageMgr->SendMessage(kPlayerServer, ClientWeaponMessage(kMessageClientDeferWeapon, weaponIndex));
			}
		}

		if (TheDisplayBoard)
		{
			TheDisplayBoard->UpdatePlayerWeapons();
		}
	}

	return (true);
}


AmmoMessage::AmmoMessage(MessageType type) : Message(type)
{
}

AmmoMessage::AmmoMessage(MessageType type, int32 weapon, int32 ammo) : Message(type)
{
	weaponIndex = weapon;
	weaponAmmo = ammo;
}

AmmoMessage::~AmmoMessage()
{
}

void AmmoMessage::Compress(Compressor& data) const
{
	data << (unsigned_int8) weaponIndex;
	data << (int16) weaponAmmo;
}

bool AmmoMessage::Decompress(Decompressor& data)
{
	unsigned_int8	index;
	int16			ammo;

	data >> index;
	if (index >= kWeaponCount)
	{
		return (false);
	}

	weaponIndex = index;

	data >> ammo;
	weaponAmmo = ammo;

	return (true);
}

bool AmmoMessage::HandleMessage(Player *sender) const
{
	Player *player = TheMessageMgr->GetLocalPlayer();
	if (player)
	{
		if (!TheMessageMgr->Server())
		{
			PlayerState *state = static_cast<GamePlayer *>(player)->GetPlayerState();

			if (GetMessageType() == kMessagePrimaryAmmo)
			{
				state->weaponAmmo[weaponIndex][0] = weaponAmmo;
			}
			else
			{
				state->weaponAmmo[weaponIndex][1] = weaponAmmo;
			}
		}

		if (TheDisplayBoard)
		{
			TheDisplayBoard->UpdateWeaponAmmo(weaponIndex);
		}
	}

	return (true);
}


PowerMessage::PowerMessage() : Message(kMessagePower)
{
}

PowerMessage::PowerMessage(int32 power) : Message(kMessagePower)
{
}

PowerMessage::~PowerMessage()
{
}

void PowerMessage::Compress(Compressor& data) const
{
	data << (unsigned_int8) powerIndex;
}

bool PowerMessage::Decompress(Decompressor& data)
{
	unsigned_int8	index;

	data >> index;
	if (index >= kPowerCount)
	{
		return (false);
	}

	powerIndex = index;
	return (true);
}

bool PowerMessage::HandleMessage(Player *sender) const
{
	return (true);
}


TreasureMessage::TreasureMessage() : Message(kMessageTreasure)
{
}

TreasureMessage::TreasureMessage(int32 treasure, int32 count) : Message(kMessageTreasure)
{
	treasureIndex = treasure;
	treasureCount = count;
}

TreasureMessage::~TreasureMessage()
{
}

void TreasureMessage::Compress(Compressor& data) const
{
	data << (unsigned_int8) treasureIndex;
	data << (int16) treasureCount;
}

bool TreasureMessage::Decompress(Decompressor& data)
{
	unsigned_int8	index;
	int16			count;

	data >> index;
	if (index >= kTreasureCount)
	{
		return (false);
	}

	treasureIndex = index;

	data >> count;
	treasureCount = count;

	return (true);
}

bool TreasureMessage::HandleMessage(Player *sender) const
{
	Player *player = TheMessageMgr->GetLocalPlayer();
	if (player)
	{
		if (!TheMessageMgr->Server())
		{
			PlayerState *state = static_cast<GamePlayer *>(player)->GetPlayerState();
			state->treasureCount[treasureIndex] = treasureCount;
		}
	}

	return (true);
}


DeathMessage::DeathMessage() : Message(kMessageDeath)
{
}

DeathMessage::DeathMessage(int32 player, PlayerKey attacker) : Message(kMessageDeath)
{
	playerKey = player;
	attackerKey = attacker;
}

DeathMessage::~DeathMessage()
{
}

void DeathMessage::Compress(Compressor& data) const
{
	data << playerKey;
	data << attackerKey;
}

bool DeathMessage::Decompress(Decompressor& data)
{
	data >> playerKey;
	data >> attackerKey;
	return (true);
}

bool DeathMessage::HandleMessage(Player *sender) const
{
	GamePlayer *player = static_cast<GamePlayer *>(TheMessageMgr->GetPlayer(playerKey));
	if (player)
	{
		const StringTable *table = TheGame->GetStringTable();

		Player *attackerPlayer = TheMessageMgr->GetPlayer(attackerKey);
		if (attackerPlayer)
		{
			if (player == attackerPlayer)
			{
				String<kMaxChatMessageLength> string(player->GetPlayerName());
				string += table->GetString(StringID('MULT', 'SUIC'));
				TheMessageBoard->AddText(string);
			}
			else
			{
				String<kMaxChatMessageLength> string(attackerPlayer->GetPlayerName());
				string += table->GetString(StringID('MULT', 'KILL'));
				string += player->GetPlayerName();
				string += table->GetString(StringID('MULT', 'STOP'));
				TheMessageBoard->AddText(string);
			}
		}
		else
		{
			String<kMaxChatMessageLength> string(player->GetPlayerName());
			string += table->GetString(StringID('MULT', 'DIED'));
			TheMessageBoard->AddText(string);
		}

		if (TheMessageMgr->GetLocalPlayer() == player)
		{
			const FighterController *controller = player->GetPlayerController();
			float azm = controller->GetLookAzimuth();
			float alt = controller->GetLookAltitude();

			GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
			const Point3D& position = world->GetPlayerCamera()->GetNodePosition();
			world->SetSpectatorCamera(position, azm, alt);

			TheUsePopupBoard->InstantHide();
			TheNamePopupBoard->SetPlayer(nullptr);
			TheNamePopupBoard->InstantHide();

			if (TheMessageMgr->Multiplayer())
			{
				if (!TheScoreBoard)
				{
					ScoreBoard::Open();
				}

				TheScoreBoard->ShowRespawnMessage();
			}
			else
			{
				DeadBoard::Open();
			}
		}

		player->SetPlayerController(nullptr);
	}

	return (true);
}


RefreshScoreboardMessage::RefreshScoreboardMessage() : Message(kMessageRefreshScoreboard)
{
}

RefreshScoreboardMessage::RefreshScoreboardMessage(int32 count) : Message(kMessageRefreshScoreboard, kMessageUnreliable)
{
	playerCount = count;
}

RefreshScoreboardMessage::~RefreshScoreboardMessage()
{
}

void RefreshScoreboardMessage::Compress(Compressor& data) const
{
	data << (unsigned_int16) playerCount;

	const PlayerData *pd = playerData;
	for (machine a = 0; a < playerCount; a++)
	{
		data << (int16) pd->playerKey;
		data << (int16) pd->playerPing;
		data << pd->playerScore;

		pd++;
	}
}

bool RefreshScoreboardMessage::Decompress(Decompressor& data)
{
	unsigned_int16	count;

	data >> count;
	playerCount = count;

	if (count <= kMaxScoreboardRefreshCount)
	{
		PlayerData *pd = playerData;
		for (machine a = 0; a < playerCount; a++)
		{
			int16	player, ping;

			data >> player;
			data >> ping;
			pd->playerKey = player;
			pd->playerPing = ping;

			data >> pd->playerScore;

			pd++;
		}

		return (true);
	}

	return (false);
}

bool RefreshScoreboardMessage::HandleMessage(Player *sender) const
{
	TheGame->RefreshScoreboard(this);
	return (true);
}


ClientStyleMessage::ClientStyleMessage() : Message(kMessageClientStyle)
{
}

ClientStyleMessage::ClientStyleMessage(const int32 *style) : Message(kMessageClientStyle)
{
	for (machine a = 0; a < kPlayerStyleCount; a++)
	{
		playerStyle[a] = style[a];
	}
}

ClientStyleMessage::~ClientStyleMessage()
{
}

void ClientStyleMessage::Compress(Compressor& data) const
{
	for (machine a = 0; a < kPlayerStyleCount; a++)
	{
		data << (char) playerStyle[a];
	}
}

bool ClientStyleMessage::Decompress(Decompressor& data)
{
	for (machine a = 0; a < kPlayerStyleCount; a++)
	{
		char	style;

		data >> style;
		playerStyle[a] = style;
	}

	return (true);
}

bool ClientStyleMessage::HandleMessage(Player *sender) const
{
	PlayerStyleMessage styleMessage(sender->GetPlayerKey(), playerStyle);
	TheMessageMgr->SendMessage(kPlayerServer, styleMessage);
	TheMessageMgr->SendMessageClients(styleMessage, sender);
	return (true);
}


ClientOrientationMessage::ClientOrientationMessage() : Message(kMessageClientOrientation)
{
}

ClientOrientationMessage::ClientOrientationMessage(float azimuth, float altitude) : Message(kMessageClientOrientation, kMessageUnreliable)
{
	orientationAzimuth = azimuth;
	orientationAltitude = altitude;
}

ClientOrientationMessage::~ClientOrientationMessage()
{
}

void ClientOrientationMessage::Compress(Compressor& data) const
{
	data << orientationAzimuth;
	data << orientationAltitude;
}

bool ClientOrientationMessage::Decompress(Decompressor& data)
{
	data >> orientationAzimuth;
	data >> orientationAltitude;
	return (true);
}

bool ClientOrientationMessage::HandleMessage(Player *sender) const
{
	FighterController *controller = static_cast<GamePlayer *>(sender)->GetPlayerController();
	if (controller)
	{
		controller->UpdateOrientation(orientationAzimuth, orientationAltitude);
	}

	return (true);
}


ClientMovementMessage::ClientMovementMessage(MessageType type) : Message(type)
{
}

ClientMovementMessage::ClientMovementMessage(MessageType type, unsigned_int32 flag, float azimuth, float altitude) : Message(type)
{
	movementFlag = flag;
	movementAzimuth = azimuth;
	movementAltitude = altitude;
}

ClientMovementMessage::~ClientMovementMessage()
{
}

void ClientMovementMessage::Compress(Compressor& data) const
{
	data << (unsigned_int8) movementFlag;
	data << movementAzimuth;
	data << movementAltitude;
}

bool ClientMovementMessage::Decompress(Decompressor& data)
{
	unsigned_int8	flag;

	data >> flag;
	movementFlag = flag;

	data >> movementAzimuth;
	data >> movementAltitude;

	return (true);
}

bool ClientMovementMessage::HandleMessage(Player *sender) const
{
	FighterController *controller = static_cast<GamePlayer *>(sender)->GetPlayerController();
	if (controller)
	{
		switch (GetMessageType())
		{
			case kMessageClientMovementBegin:

				controller->BeginMovement(movementFlag, movementAzimuth, movementAltitude);
				break;

			case kMessageClientMovementEnd:

				controller->EndMovement(movementFlag, movementAzimuth, movementAltitude);
				break;

			case kMessageClientMovementChange:

				controller->ChangeMovement(movementFlag, movementAzimuth, movementAltitude);
				break;
		}
	}

	return (true);
}


ClientFiringMessage::ClientFiringMessage(MessageType type) : Message(type)
{
}

ClientFiringMessage::ClientFiringMessage(MessageType type, float azimuth, float altitude) : Message(type)
{
	firingAzimuth = azimuth;
	firingAltitude = altitude;
}

ClientFiringMessage::~ClientFiringMessage()
{
}

void ClientFiringMessage::Compress(Compressor& data) const
{
	data << firingAzimuth;
	data << firingAltitude;
}

bool ClientFiringMessage::Decompress(Decompressor& data)
{
	data >> firingAzimuth;
	data >> firingAltitude;
	return (true);
}

bool ClientFiringMessage::HandleMessage(Player *sender) const
{
	GamePlayer *player = static_cast<GamePlayer *>(sender);
	FighterController *controller = player->GetPlayerController();
	if (controller)
	{
		MessageType type = GetMessageType();
		if (type == kMessageClientFiringEnd)
		{
			controller->EndFiring(firingAzimuth, firingAltitude);

			// If the player picked up a better weapon while he was firing, then the deferred
			// weapon type might be set. If so, switch to that weapon at this time.

			if ((player->GetDeferredWeapon() != kWeaponNone) && (player->GetDeferredWeaponTime() == 0))
			{
				player->SetDeferredWeaponState(250, 1);
			}
		}
		else
		{
			if (player->GetShieldTime() >= 0)
			{
				player->SetShieldTime(-1);
				TheMessageMgr->SendMessageAll(ControllerMessage(FighterController::kFighterMessageEndShield, controller->GetControllerIndex()));
			}

			controller->BeginFiring((type == kMessageClientFiringPrimaryBegin), firingAzimuth, firingAltitude);
		}
	}

	return (true);
}


ClientInteractionMessage::ClientInteractionMessage(MessageType type) : Message(type)
{
}

ClientInteractionMessage::ClientInteractionMessage(MessageType type, const Point3D& position) : Message(type)
{
	interactionPosition = position;
}

ClientInteractionMessage::~ClientInteractionMessage()
{
}

void ClientInteractionMessage::Compress(Compressor& data) const
{
	data << interactionPosition;
}

bool ClientInteractionMessage::Decompress(Decompressor& data)
{
	data >> interactionPosition;
	return (true);
}

bool ClientInteractionMessage::HandleMessage(Player *sender) const
{
	FighterController *controller = static_cast<GamePlayer *>(sender)->GetPlayerController();
	if (controller)
	{
		const Node *node = controller->GetFighterInteractor()->GetInteractionNode();
		if (node)
		{
			Controller *interactionController = node->GetController();
			if (interactionController)
			{
				InteractionEventType eventType = (GetMessageType() == kMessageClientInteractionBegin) ? kInteractionEventActivate : kInteractionEventDeactivate;
				interactionController->HandleInteractionEvent(eventType, &interactionPosition, controller->GetTargetNode());
			}
		}
	}

	return (true);
}


ClientWeaponMessage::ClientWeaponMessage(MessageType type) : Message(type)
{
}

ClientWeaponMessage::ClientWeaponMessage(MessageType type, int32 weapon) : Message(type)
{
	weaponIndex = weapon;
}

ClientWeaponMessage::~ClientWeaponMessage()
{
}

void ClientWeaponMessage::Compress(Compressor& data) const
{
	data << (unsigned_int8) weaponIndex;
}

bool ClientWeaponMessage::Decompress(Decompressor& data)
{
	unsigned_int8	index;

	data >> index;
	weaponIndex = index;
	return (index < kWeaponCount);
}

bool ClientWeaponMessage::HandleMessage(Player *sender) const
{
	GamePlayer *player = static_cast<GamePlayer *>(sender);
	const FighterController *controller = player->GetPlayerController();
	if (controller)
	{
		int32 weapon = weaponIndex;

		MessageType type = GetMessageType();
		if (type == kMessageClientSpecialWeapon)
		{
			for (machine a = kWeaponStandardCount; a < kWeaponCount; a++)
			{
				if (player->GetWeaponFlag(a))
				{
					weapon = a;
					break;
				}
			}
		}

		if (weapon != kWeaponNone)
		{
			if ((weapon == kWeaponPistol) && (player->GetCurrentWeapon() == kWeaponPistol))
			{
				weapon = kWeaponAxe;
			}

			if (player->GetWeaponFlag(weapon))
			{
				// The message type is kMessageClientDeferWeapon when the player picks up a better
				// weapon while firing a different weapon. The deferred weapon time is positive after the
				// player runs out of ammo. In either of these cases, we don't switch weapons immediately,
				// but only after the player has stopped firing or the empty ammo time has elapsed.

				if (((type == kMessageClientDeferWeapon) && ((controller->GetFighterFlags() & kFighterFiring) != 0)) || (player->GetDeferredWeaponTime() > 0))
				{
					player->SetDeferredWeapon(weapon);
				}
				else
				{
					player->SetDeferredWeapon(kWeaponNone);
					World *world = controller->GetTargetNode()->GetWorld();
					TheMessageMgr->SendMessageAll(FighterWeaponMessage(controller->GetControllerIndex(), weapon, world->NewControllerIndex()));
				}
			}
		}
	}

	return (true);
}


ClientWeaponCycleMessage::ClientWeaponCycleMessage() : Message(kMessageClientCycleWeapon)
{
}

ClientWeaponCycleMessage::ClientWeaponCycleMessage(int32 direction) : Message(kMessageClientCycleWeapon)
{
	cycleDirection = direction;
}

ClientWeaponCycleMessage::~ClientWeaponCycleMessage()
{
}

void ClientWeaponCycleMessage::Compress(Compressor& data) const
{
	data << (unsigned_int8) cycleDirection;
}

bool ClientWeaponCycleMessage::Decompress(Decompressor& data)
{
	unsigned_int8	direction;

	data >> direction;
	cycleDirection = direction;
	return (direction < 2);
}

bool ClientWeaponCycleMessage::HandleMessage(Player *sender) const
{
	GamePlayer *player = static_cast<GamePlayer *>(sender);
	const FighterController *controller = player->GetPlayerController();
	if (controller)
	{
		player->SetDeferredWeapon(kWeaponNone);

		int32 weaponIndex = player->GetCurrentWeapon();
		if (weaponIndex != kWeaponNone)
		{
			int32 direction = cycleDirection * 2 - 1;
			for (machine a = 1; a < kWeaponCount; a++)
			{
				weaponIndex += direction;
				if (weaponIndex == kWeaponCount)
				{
					weaponIndex = kWeaponNone + 1;
				}
				else if (weaponIndex == kWeaponNone)
				{
					weaponIndex = kWeaponCount - 1;
				}

				if (player->GetWeaponFlag(weaponIndex))
				{
					World *world = controller->GetTargetNode()->GetWorld();
					TheMessageMgr->SendMessageAll(FighterWeaponMessage(controller->GetControllerIndex(), weaponIndex, world->NewControllerIndex()));
					break;
				}
			}
		}
	}

	return (true);
}


ClientMiscMessage::ClientMiscMessage(MessageType type) : Message(type)
{
}

ClientMiscMessage::~ClientMiscMessage()
{
}

bool ClientMiscMessage::HandleMessage(Player *sender) const
{
	GamePlayer *player = static_cast<GamePlayer *>(sender);

	switch (GetMessageType())
	{
		case kMessageClientSpawn:

			if ((!(player->GetPlayerFlags() & kPlayerInactive)) && (player->GetDeathTime() < 0))
			{
				FighterController *controller = player->GetPlayerController();
				if (!controller)
				{
					delete TheDeadBoard;

					if (TheMessageMgr->Multiplayer())
					{
						TheGame->SpawnPlayer(player);
					}
					else
					{
						TheGame->RestartWorld();
					}
				}
			}

			break;

		case kMessageClientScoreOpen:

			player->SetPlayerFlags(player->GetPlayerFlags() | kPlayerScoreboardOpen);
			player->SetScoreboardTime(0);
			break;

		case kMessageClientScoreClose:

			player->SetPlayerFlags(player->GetPlayerFlags() & ~kPlayerScoreboardOpen);
			break;

		case kMessageClientChatOpen:
		{
			const FighterController *controller = player->GetPlayerController();
			if (controller)
			{
				TheMessageMgr->SendMessageAll(FighterIconMessage(FighterController::kFighterMessageBeginIcon, controller->GetControllerIndex(), kFighterIconChat));
			}

			break;
		}

		case kMessageClientChatClose:
		{
			const FighterController *controller = player->GetPlayerController();
			if (controller)
			{
				TheMessageMgr->SendMessageAll(FighterIconMessage(FighterController::kFighterMessageEndIcon, controller->GetControllerIndex(), kFighterIconChat));
			}

			break;
		}

		case kMessageClientVoiceReceiveStart:
		{
			// Client has asked to start receiving voice chat, so create a channel
			// leading from each of the other players to that player.

			player->SetPlayerFlags(player->GetPlayerFlags() | kPlayerReceiveVoiceChat);

			GamePlayer *p = static_cast<GamePlayer *>(TheMessageMgr->GetFirstPlayer());
			while (p)
			{
				if ((p != player) && (!p->FindOutgoingEdge(player)))
				{
					new Channel(p, player);
				}

				p = p->Next();
			}

			break;
		}

		case kMessageClientVoiceReceiveStop:

			// Client has asked to stop receiving voice chat, so purge all of the
			// channels (edges in the graph) leading to that player.

			player->PurgeIncomingEdges();
			break;
	}

	return (true);
}


GamePlayer::GamePlayer(PlayerKey key) : Player(key)
{
	playerController = nullptr;

	playerState.playerScore = 0;
	playerState.playerScoreFraction = 0;

	playerFlags = 0;
	playerPing = -1;

	Game::InitPlayerStyle(playerStyle);

	scoreUpdateTime = 0;
	deathTime = -1;
	shieldTime = -1;

	SetChatSoundGroup(TheGame->GetVoiceSoundGroup());
}

GamePlayer::~GamePlayer()
{
}

void GamePlayer::SetPlayerController(FighterController *controller, const PlayerState *state)
{
	playerController = controller;
	if (controller)
	{
		controller->SetFighterPlayer(this);

		if (state)
		{
			playerState = *state;
		}
		else
		{
			playerState.playerHealth = 100 << 16;
			playerState.weaponFlags = (1 << kWeaponAxe) | (1 << kWeaponPistol);
			playerState.currentWeapon = kWeaponPistol;
			playerState.deferredWeapon = kWeaponNone;
			playerState.deferredWeaponTime = 0;
			playerState.deferredWeaponFrame = 0;

			for (machine a = 0; a < kWeaponCount; a++)
			{
				playerState.weaponAmmo[a][0] = 0;
				playerState.weaponAmmo[a][1] = 0;
			}

			playerState.weaponAmmo[kWeaponPistol][0] = Weapon::Get(kWeaponPistol)->GetInitWeaponAmmo(0);

			for (machine a = 0; a < kPowerCount; a++)
			{
				playerState.powerTime[a] = 0;
			}

			playerState.killCount = 0;

			for (machine a = 0; a < kTreasureCount; a++)
			{
				playerState.treasureCount[a] = 0;
			}
		}
	}
}

void GamePlayer::SetPlayerStyle(const int32 *style)
{
	for (machine a = 0; a < kPlayerStyleCount; a++)
	{
		playerStyle[a] = style[a];
	}

	if (playerController)
	{
		playerController->SetFighterStyle(style);
	}
}

void GamePlayer::AddPlayerScore(Fixed delta)
{
	Fixed fraction = playerState.playerScoreFraction + (delta & 0xFFFF);
	playerState.playerScore = playerState.playerScore + (delta >> 16) + (fraction >> 16);
	playerState.playerScoreFraction = fraction & 0xFFFF;
	playerFlags |= kPlayerScoreUpdate;
	scoreUpdateTime = 0;
}

void GamePlayer::SetPlayerHealth(Fixed health)
{
	if (playerState.playerHealth != health)
	{
		playerState.playerHealth = health;
		SendMessage(HealthMessage(health >> 16));
	}
}

void GamePlayer::AcquireWeapon(int32 weaponIndex)
{
	playerState.weaponFlags |= 1 << weaponIndex;

	const Weapon *weapon = Weapon::Get(weaponIndex);
	if (weapon)
	{
		int32 prevAmmo1 = playerState.weaponAmmo[weaponIndex][0];
		int32 prevAmmo2 = playerState.weaponAmmo[weaponIndex][1];
		int32 newAmmo1 = Min(prevAmmo1 + weapon->GetInitWeaponAmmo(0), weapon->GetMaxWeaponAmmo(0));
		int32 newAmmo2 = Min(prevAmmo2 + weapon->GetInitWeaponAmmo(1), weapon->GetMaxWeaponAmmo(1));
		playerState.weaponAmmo[weaponIndex][0] = newAmmo1;
		playerState.weaponAmmo[weaponIndex][1] = newAmmo2;

		// Always send the acquire weapon message because it might cause an auto-switch.

		SendMessage(WeaponMessage(weaponIndex, newAmmo1, newAmmo2));

		const FighterController *fighter = playerController;
		if (fighter)
		{
			const Model *model = fighter->GetWeaponModel();
			if (model)
			{
				Controller *controller = model->GetController();
				TheMessageMgr->SendMessageAll(WeaponResetMessage(controller->GetControllerIndex()));
			}
		}
	}
}

void GamePlayer::SetWeaponAmmo(int32 weaponIndex, int32 ammoIndex, int32 ammo)
{
	int32& currentAmmo = playerState.weaponAmmo[weaponIndex][ammoIndex];
	if (currentAmmo != ammo)
	{
		bool increase = (ammo > currentAmmo);
		currentAmmo = ammo;

		SendMessage(AmmoMessage(kMessagePrimaryAmmo + ammoIndex, weaponIndex, ammo));

		if (increase)
		{
			const FighterController *fighter = playerController;
			if (fighter)
			{
				const Model *model = fighter->GetWeaponModel();
				if (model)
				{
					Controller *controller = model->GetController();
					TheMessageMgr->SendMessageAll(WeaponResetMessage(controller->GetControllerIndex()));
				}
			}
		}
	}
}

void GamePlayer::AddPlayerKill(void)
{
	playerState.killCount++;
}

void GamePlayer::AddPlayerTreasure(int32 treasureIndex, int32 count)
{
	count += playerState.treasureCount[treasureIndex];
	playerState.treasureCount[treasureIndex] = count;
	SendMessage(TreasureMessage(treasureIndex, count));
}

CharacterStatus GamePlayer::Damage(Fixed damage, unsigned_int32 flags, GameCharacterController *attacker)
{
	if (TheMessageMgr->Server())
	{
		if (shieldTime < 0)
		{
			GamePlayer *attackerPlayer = GetAttackerPlayer(attacker);
			if (attackerPlayer == this)
			{
				// Only do half damage to yourself.

				damage >>= 1;
			}

			int32 health = GetPlayerHealth();
			damage = Min(damage, health);

			if (damage > 0)
			{
				#if MGCHEATS

					if (!(TheGame->GetGameFlags() & kGameGodMode))
					{
						health -= damage;
					}

				#else

					health -= damage;

				#endif

				if ((attackerPlayer) && (attackerPlayer != this))
				{
					attackerPlayer->AddPlayerScore(damage);
				}

				if (health > 0)
				{
					SetPlayerHealth(health);
					return (kCharacterDamaged);
				}

				Kill(attacker);
				return (kCharacterKilled);
			}
		}
	}

	return (kCharacterUnaffected);
}

void GamePlayer::Kill(GameCharacterController *attacker)
{
	if (TheMessageMgr->Server())
	{
		SetPlayerHealth(0);
		playerState.deferredWeapon = kWeaponNone;

		if (playerController)
		{
			TheMessageMgr->SendMessageAll(ControllerMessage(FighterController::kFighterMessageDeath, playerController->GetControllerIndex()));
			TheMessageMgr->SendMessageAll(DeathMessage(GetPlayerKey(), GetAttackerKey(attacker)));

			scoreUpdateTime = 0;
			scoreboardTime = 0;
			deathTime = kPlayerRespawnInterval;
		}
	}
}

GamePlayer *GamePlayer::GetAttackerPlayer(const GameCharacterController *attacker)
{
	if ((attacker) && (attacker->GetCharacterType() == kCharacterPlayer))
	{
		return (static_cast<const FighterController *>(attacker)->GetFighterPlayer());
	}

	return (nullptr);
}

PlayerKey GamePlayer::GetAttackerKey(const GameCharacterController *attacker)
{
	if ((attacker) && (attacker->GetCharacterType() == kCharacterPlayer))
	{
		return (static_cast<const FighterController *>(attacker)->GetFighterPlayer()->GetPlayerKey());
	}

	return (kPlayerNone);
}


void Game::HandleConnectionEvent(ConnectionEvent event, const NetworkAddress& address, const void *param)
{
	switch (event)
	{
		case kConnectionQueryReceived:
		{
			World *world = TheWorldMgr->GetWorld();
			if (world)
			{
				const char *gameName = TheEngine->GetVariable("gameName")->GetValue();
				const char *worldName = world->GetWorldName();

				ServerInfoMessage message(TheMessageMgr->GetPlayerCount(), TheMessageMgr->GetMaxPlayerCount(), gameName, worldName);
				TheMessageMgr->SendConnectionlessMessage(address, message);
			}

			break;
		}

		case kConnectionAttemptFailed:

			// The server rejected our connection.

			if (TheConnectWindow)
			{
				unsigned_int32 id = 'TIME';
				unsigned_int32 reason = *static_cast<const unsigned_int32 *>(param);
				if (reason == kNetworkFailWrongProtocol)
				{
					id = 'PROT';
				}
				else if (reason == kNetworkFailNotServer)
				{
					id = 'NSRV';
				}
				else if (reason == kNetworkFailServerFull)
				{
					id = 'FULL';
				}

				TheConnectWindow->SetFailMessage(stringTable.GetString(StringID('MULT', 'FAIL', id)));
			}

			break;

		case kConnectionServerAccepted:

			// The server accepted our connection.

			if (TheConnectWindow)
			{
				TheConnectWindow->SetAcceptMessage(stringTable.GetString(StringID('MULT', 'ACPT')));
			}

			// Tell the server what player styles the user has set.
			// The server will forward this information to the other players.

			TheMessageMgr->SendMessage(kPlayerServer, ClientStyleMessage(static_cast<GamePlayer *>(TheMessageMgr->GetLocalPlayer())->GetPlayerStyle()));
			break;

		case kConnectionServerClosed:

			// The server was shut down.

			ExitCurrentGame();
			TheMessageBoard->AddText(stringTable.GetString(StringID('MULT', 'LOST')));
			break;

		case kConnectionServerTimedOut:

			// The server has stopped responding.

			ExitCurrentGame();
			TheMessageBoard->AddText(stringTable.GetString(StringID('MULT', 'LOST')));
			break;
	}

	Application::HandleConnectionEvent(event, address, param);
}

void Game::HandlePlayerEvent(PlayerEvent event, Player *player, const void *param)
{
	switch (event)
	{
		case kPlayerConnected:
		{
			if (TheScoreBoard)
			{
				TheScoreBoard->Refresh();
			}

			if (TheMessageMgr->Synchronized())
			{
				String<kMaxPlayerNameLength + 48> message(player->GetPlayerName());
				message += TheGame->GetStringTable()->GetString(StringID('MULT', 'CONN'));
				TheMessageBoard->AddText(message);
			}

			if (TheMessageMgr->Server())
			{
				GamePlayer *p = static_cast<GamePlayer *>(TheMessageMgr->GetFirstPlayer());
				while (p)
				{
					if ((p != player) && (p->GetPlayerFlags() & kPlayerReceiveVoiceChat))
					{
						new Channel(player, p);
					}

					p = p->Next();
				}
			}

			break;
		}

		case kPlayerDisconnected:
		{
			Controller *controller = static_cast<GamePlayer *>(player)->GetPlayerController();
			if (controller)
			{
				delete controller->GetTargetNode();
			}

			String<kMaxPlayerNameLength + 48> message(player->GetPlayerName());
			message += TheGame->GetStringTable()->GetString(StringID('MULT', 'DSCN'));
			TheMessageBoard->AddText(message);
			break;
		}

		case kPlayerTimedOut:
		{
			Controller *controller = static_cast<GamePlayer *>(player)->GetPlayerController();
			if (controller)
			{
				delete controller->GetTargetNode();
			}

			String<kMaxPlayerNameLength + 48> message(player->GetPlayerName());
			message += TheGame->GetStringTable()->GetString(StringID('MULT', 'TIME'));
			TheMessageBoard->AddText(message);
			break;
		}

		case kPlayerInitialized:
		{
			// A new player joining the game has been initialized. For each player already
			// in the game, send a message containing the existing player's style to the new player.

			const GamePlayer *gamePlayer = static_cast<GamePlayer *>(TheMessageMgr->GetFirstPlayer());
			do
			{
				if (gamePlayer != player)
				{
					player->SendMessage(PlayerStyleMessage(gamePlayer->GetPlayerKey(), gamePlayer->GetPlayerStyle()));
				}

				gamePlayer = static_cast<GamePlayer *>(gamePlayer->Next());
			} while (gamePlayer);

			// Now tell the new player what world is being played.

			World *world = TheWorldMgr->GetWorld();
			if (world)
			{
				player->SendMessage(GameInfoMessage(multiplayerFlags, world->GetWorldName()));
			}

			break;
		}

		case kPlayerChatReceived:
		{
			String<kMaxChatMessageLength + kMaxPlayerNameLength + 32> string(player->GetPlayerName());
			string += TheGame->GetStringTable()->GetString(StringID('MULT', 'CHAT'));
			string += static_cast<const char *>(param);

			TheMessageBoard->AddText(string);
			Sound *sound = new Sound;
			sound->Load("sound/Message");
			sound->Play();
			break;
		}

		case kPlayerRenamed:
		{
			String<kMaxPlayerNameLength * 2 + 32> string(static_cast<const char *>(param));
			const StringTable *table = TheGame->GetStringTable();
			string += table->GetString(StringID('MULT', 'NAME'));
			string += player->GetPlayerName();
			string += table->GetString(StringID('MULT', 'STOP'));

			TheMessageBoard->AddText(string);

			if (TheNamePopupBoard)
			{
				TheNamePopupBoard->RenamePlayer(player);
			}

			if (TheScoreBoard)
			{
				TheScoreBoard->RenamePlayer(static_cast<GamePlayer *>(player));
			}

			break;
		}
	}

	Application::HandlePlayerEvent(event, player, param);
}

void Game::HandleGameEvent(GameEvent event, const void *param)
{
	switch (event)
	{
		case kGameSynchronized:

			delete TheConnectWindow;
			ScoreBoard::Open();

			if (TheEngine->GetVariable("voiceReceive")->GetIntegerValue() != 0)
			{
				// If voice chat receive is enabled, then tell the server to send voice chat messages.

				TheMessageMgr->SendMessage(kPlayerServer, ClientMiscMessage(kMessageClientVoiceReceiveStart));
			}

			break;
	}
}

Message *Game::CreateMessage(MessageType type, Decompressor& data) const
{
	switch (type)
	{
		case kMessageServerInfo:

			return (new ServerInfoMessage);

		case kMessageGameInfo:

			return (new GameInfoMessage);

		case kMessagePlayerStyle:

			return (new PlayerStyleMessage);

		case kMessageCreateModel:
		{
			unsigned_int8	modelType;

			data >> modelType;
			return (CreateModelMessage::CreateMessage(modelType));
		}

		case kMessageScore:

			return (new ScoreMessage);

		case kMessageHealth:

			return (new HealthMessage);

		case kMessageWeapon:

			return (new WeaponMessage);

		case kMessagePrimaryAmmo:
		case kMessageSecondaryAmmo:

			return (new AmmoMessage(type));

		case kMessagePower:

			return (new PowerMessage);

		case kMessageTreasure:

			return (new TreasureMessage);

		case kMessageDeath:

			return (new DeathMessage);

		case kMessageRefreshScoreboard:

			return (new RefreshScoreboardMessage);

		case kMessageClientStyle:

			return (new ClientStyleMessage);

		case kMessageClientOrientation:

			return (new ClientOrientationMessage);

		case kMessageClientMovementBegin:
		case kMessageClientMovementEnd:
		case kMessageClientMovementChange:

			return (new ClientMovementMessage(type));

		case kMessageClientFiringPrimaryBegin:
		case kMessageClientFiringSecondaryBegin:
		case kMessageClientFiringEnd:

			return (new ClientFiringMessage(type));

		case kMessageClientInteractionBegin:
		case kMessageClientInteractionEnd:

			return (new ClientInteractionMessage(type));

		case kMessageClientSwitchWeapon:
		case kMessageClientDeferWeapon:
		case kMessageClientSpecialWeapon:

			return (new ClientWeaponMessage(type));

		case kMessageClientCycleWeapon:

			return (new ClientWeaponCycleMessage);

		case kMessageClientSpawn:
		case kMessageClientScoreOpen:
		case kMessageClientScoreClose:
		case kMessageClientChatOpen:
		case kMessageClientChatClose:
		case kMessageClientVoiceReceiveStart:
		case kMessageClientVoiceReceiveStop:

			return (new ClientMiscMessage(type));
	}

	return (nullptr);
}

void Game::ReceiveMessage(Player *sender, const NetworkAddress& address, const Message *message)
{
	if (message->GetMessageType() == kMessageServerInfo)
	{
		if (TheJoinGameWindow)
		{
			TheJoinGameWindow->ReceiveServerInfo(address, static_cast<const ServerInfoMessage *>(message));
		}
	}
}

void Game::SpawnPlayer(Player *player)
{
	GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
	if (world)
	{
		int32 count = world->GetSpawnLocatorCount();
		if (count != 0)
		{
			const Marker *marker = world->GetSpawnLocator(Math::Random(count));

			const Vector3D direction = marker->GetWorldTransform()[0];
			float azimuth = Atan(direction.y, direction.x);

			int32 fighterIndex = world->NewControllerIndex();
			int32 weaponIndex = world->NewControllerIndex();
			TheMessageMgr->SendMessageAll(CreateGusGravesMessage(fighterIndex, marker->GetWorldPosition(), azimuth, 0.0F, 0, kWeaponPistol, weaponIndex, player->GetPlayerKey()));

			static_cast<GamePlayer *>(player)->SetShieldTime(kProtectionShieldInterval);
			TheMessageMgr->SendMessageAll(ControllerMessage(FighterController::kFighterMessageBeginShield, fighterIndex));
		}
	}
}

void Game::RefreshScoreboard(const RefreshScoreboardMessage *message)
{
	int32 count = message->GetPlayerCount();
	for (machine a = 0; a < count; a++)
	{
		const RefreshScoreboardMessage::PlayerData *data = message->GetPlayerData(a);

		GamePlayer *player = static_cast<GamePlayer *>(TheMessageMgr->GetPlayer(data->playerKey));
		if (player)
		{
			player->playerState.playerScore = data->playerScore;
			player->playerPing = data->playerPing;
		}
	}

	if (TheScoreBoard)
	{
		TheScoreBoard->Refresh();
	}
}

void Game::ApplicationTask(void)
{
	SpiderController::ResetGlobalSpiderFlags();
	SkeletonController::ResetGlobalSkeletonFlags();

	if (TheMessageMgr->Server())
	{
		int32 dt = TheTimeMgr->GetSystemDeltaTime();

		GamePlayer *player = static_cast<GamePlayer *>(TheMessageMgr->GetFirstPlayer());
		while (player)
		{
			unsigned_int32 flags = player->GetPlayerFlags();

			int32 scoreUpdateTime = MaxZero(player->GetScoreUpdateTime() - dt);
			if ((scoreUpdateTime == 0) && (flags & kPlayerScoreUpdate))
			{
				scoreUpdateTime = kScoreUpdateInterval;

				player->SetPlayerFlags(flags & ~kPlayerScoreUpdate);
				player->SendMessage(ScoreMessage(player->GetPlayerScore()));
			}

			player->SetScoreUpdateTime(scoreUpdateTime);

			if (flags & kPlayerScoreboardOpen)
			{
				int32 time = player->GetScoreboardTime() - dt;
				if (time <= 0)
				{
					time = kScoreboardRefreshInterval;

					const GamePlayer *p = static_cast<GamePlayer *>(TheMessageMgr->GetFirstPlayer());
					int32 playerCount = TheMessageMgr->GetPlayerCount();
					while (playerCount > 0)
					{
						int32 count = Min(playerCount, RefreshScoreboardMessage::kMaxScoreboardRefreshCount);
						RefreshScoreboardMessage message(count);

						for (machine a = 0; a < count; a++)
						{
							RefreshScoreboardMessage::PlayerData *data = message.GetPlayerData(a);

							data->playerKey = p->GetPlayerKey();
							data->playerScore = p->GetPlayerScore();
							data->playerPing = p->GetNetworkPing();

							p = p->Next();
						}

						player->SendMessage(message);

						playerCount -= count;
					}
				}

				player->SetScoreboardTime(time);
			}

			int32 deathTime = player->GetDeathTime();
			if (deathTime >= 0)
			{
				player->SetDeathTime(deathTime - dt);
			}

			int32 shieldTime = player->GetShieldTime();
			if (shieldTime >= 0)
			{
				int32 time = shieldTime - dt;
				player->SetShieldTime(time);

				if (time < 0)
				{
					FighterController *controller = player->GetPlayerController();
					if (controller)
					{
						TheMessageMgr->SendMessageAll(ControllerMessage(FighterController::kFighterMessageEndShield, controller->GetControllerIndex()));
					}
				}
			}

			// If the deferred weapon time is positive, then we might be waiting to change
			// weapons after the player has run out of ammo or has stopped firing after
			// picking up a better weapon.

			int32 deferredWeaponTime = player->GetDeferredWeaponTime();
			if (deferredWeaponTime > 0)
			{
				int32 deferredWeaponFrame = player->GetDeferredWeaponFrame();
				if (deferredWeaponFrame == 0)
				{
					deferredWeaponTime -= dt;
					if (deferredWeaponTime <= 0)
					{
						int32 weaponIndex = player->GetDeferredWeapon();
						player->SetDeferredWeaponState(0, 0);

						if (weaponIndex != kWeaponNone)
						{
							player->SetDeferredWeapon(kWeaponNone);

							FighterController *controller = player->GetPlayerController();
							if (controller)
							{
								World *world = controller->GetTargetNode()->GetWorld();
								TheMessageMgr->SendMessageAll(FighterWeaponMessage(controller->GetControllerIndex(), weaponIndex, world->NewControllerIndex()));
							}
						}
					}
					else
					{
						player->SetDeferredWeaponState(deferredWeaponTime, 0);
					}
				}
				else
				{
					player->SetDeferredWeaponState(deferredWeaponTime, deferredWeaponFrame - 1);
				}
			}

			player = player->Next();
		}
	}
}

// ZYUQURM
