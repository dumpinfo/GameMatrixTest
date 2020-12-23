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


#include "MGInput.h"
#include "MGFighter.h"
#include "MGGame.h"


using namespace C4;


MovementAction::MovementAction(ActionType type, unsigned_int32 moveFlag, unsigned_int32 specFlag) : Action(type)
{
	movementFlag = moveFlag;
	spectatorFlag = specFlag;
}

MovementAction::~MovementAction()
{
}

void MovementAction::Begin(void)
{
	const Player *player = TheMessageMgr->GetLocalPlayer();
	if (player)
	{
		const FighterController *controller = static_cast<const GamePlayer *>(player)->GetPlayerController();
		if (controller)
		{
			ClientMovementMessage message(kMessageClientMovementBegin, movementFlag, controller->GetLookAzimuth(), controller->GetLookAltitude());
			TheMessageMgr->SendMessage(kPlayerServer, message);
			return;
		}
	}

	GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
	if (world)
	{
		SpectatorCamera *camera = world->GetSpectatorCamera();
		camera->SetSpectatorFlags(camera->GetSpectatorFlags() | spectatorFlag);
	}
}

void MovementAction::End(void)
{
	const Player *player = TheMessageMgr->GetLocalPlayer();
	if (player)
	{
		const FighterController *controller = static_cast<const GamePlayer *>(player)->GetPlayerController();
		if (controller)
		{
			ClientMovementMessage message(kMessageClientMovementEnd, movementFlag, controller->GetLookAzimuth(), controller->GetLookAltitude());
			TheMessageMgr->SendMessage(kPlayerServer, message);
			return;
		}
	}

	GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
	if (world)
	{
		SpectatorCamera *camera = world->GetSpectatorCamera();
		camera->SetSpectatorFlags(camera->GetSpectatorFlags() & ~spectatorFlag);
	}
}

void MovementAction::Move(int32 value)
{
	static const unsigned_int32 movementFlags[8] =
	{
		kMovementForward,
		kMovementForward | kMovementRight,
		kMovementRight,
		kMovementRight | kMovementBackward,
		kMovementBackward,
		kMovementBackward | kMovementLeft,
		kMovementLeft,
		kMovementLeft | kMovementForward
	};

	static const unsigned_int32 spectatorFlags[8] =
	{
		kSpectatorMoveForward,
		kSpectatorMoveForward | kSpectatorMoveRight,
		kSpectatorMoveRight,
		kSpectatorMoveRight | kSpectatorMoveBackward,
		kSpectatorMoveBackward,
		kSpectatorMoveBackward | kSpectatorMoveLeft,
		kSpectatorMoveLeft,
		kSpectatorMoveLeft | kSpectatorMoveForward
	};

	const Player *player = TheMessageMgr->GetLocalPlayer();
	if (player)
	{
		const FighterController *controller = static_cast<const GamePlayer *>(player)->GetPlayerController();
		if (controller)
		{
			unsigned_int32 flags = (value >= 0) ? movementFlags[value] : 0;
			ClientMovementMessage message(kMessageClientMovementChange, flags, controller->GetLookAzimuth(), controller->GetLookAltitude());
			TheMessageMgr->SendMessage(kPlayerServer, message);
			return;
		}
	}
 
	GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
	if (world)
	{ 
		unsigned_int32 flags = (value >= 0) ? spectatorFlags[value] : 0;
		SpectatorCamera *camera = world->GetSpectatorCamera(); 
		camera->SetSpectatorFlags((camera->GetSpectatorFlags() & ~kSpectatorPlanarMask) | flags);
	}
} 

 
LookAction::LookAction(ActionType type) : Action(type) 
{
}

LookAction::~LookAction() 
{
}

void LookAction::Update(float value)
{
	if (GetActionType() == kActionHorizontal)
	{
		TheGame->UpdateLookSpeedX(value);
	}
	else
	{
		TheGame->UpdateLookSpeedY(value);
	}
}


FireAction::FireAction(ActionType type) : Action(type)
{
}

FireAction::~FireAction()
{
}

void FireAction::Begin(void)
{
	const Player *player = TheMessageMgr->GetLocalPlayer();
	if (player)
	{
		FighterController *controller = static_cast<const GamePlayer *>(player)->GetPlayerController();
		if (controller)
		{
			// The player is controlling a fighter, so either fire the weapon or interact with a panel effect.

			const FighterInteractor *interactor = controller->GetFighterInteractor();
			const Node *interactionNode = interactor->GetInteractionNode();
			if ((!interactionNode) || (interactionNode->GetNodeType() != kNodeEffect))
			{
				// No panel effect, so fire the weapon.

				ClientFiringMessage message((GetActionType() == kActionFirePrimary) ? kMessageClientFiringPrimaryBegin : kMessageClientFiringSecondaryBegin, controller->GetLookAzimuth(), controller->GetLookAltitude());
				TheMessageMgr->SendMessage(kPlayerServer, message);
			}
			else
			{
				// The player's interacting with a panel effect.

				ClientInteractionMessage message(kMessageClientInteractionBegin, interactor->GetInteractionPosition());
				TheMessageMgr->SendMessage(kPlayerServer, message);
			}
		}
		else
		{
			// There's no fighter yet, so spawn the player in response to the fire button being pressed.

			if (TheMessageMgr->Synchronized())
			{
				if ((!TheMessageMgr->Server()) || (!(TheGame->GetMultiplayerFlags() & kMultiplayerDedicated)))
				{
					TheMessageMgr->SendMessage(kPlayerServer, ClientMiscMessage(kMessageClientSpawn));
				}
			}
		}
	}
}

void FireAction::End(void)
{
	const Player *player = TheMessageMgr->GetLocalPlayer();
	if (player)
	{
		FighterController *controller = static_cast<const GamePlayer *>(player)->GetPlayerController();
		if (controller)
		{
			// The player is controlling a fighter, so either stop firing the weapon or interact with a panel effect.

			const FighterInteractor *interactor = controller->GetFighterInteractor();
			const Node *interactionNode = interactor->GetInteractionNode();
			if ((!interactionNode) || (interactionNode->GetNodeType() != kNodeEffect))
			{
				// No panel effect, so stop firing the weapon.

				ClientFiringMessage message(kMessageClientFiringEnd, controller->GetLookAzimuth(), controller->GetLookAltitude());
				TheMessageMgr->SendMessage(kPlayerServer, message);
			}
			else
			{
				// The player's interacting with a panel.

				ClientInteractionMessage message(kMessageClientInteractionEnd, interactor->GetInteractionPosition());
				TheMessageMgr->SendMessage(kPlayerServer, message);
			}
		}
	}
}


UseAction::UseAction() : Action(kActionUse)
{
}

UseAction::~UseAction()
{
}

void UseAction::Begin(void)
{
	const Player *player = TheMessageMgr->GetLocalPlayer();
	if (player)
	{
		FighterController *controller = static_cast<const GamePlayer *>(player)->GetPlayerController();
		if (controller)
		{
			const FighterInteractor *interactor = controller->GetFighterInteractor();
			if (interactor->GetInteractionNode())
			{
				ClientInteractionMessage message(kMessageClientInteractionBegin, interactor->GetInteractionPosition());
				TheMessageMgr->SendMessage(kPlayerServer, message);
			}
			else
			{
				Sound *sound = new Sound;
				sound->Load("sound/Use");
				sound->Play();
			}
		}
	}
}

void UseAction::End(void)
{
	const Player *player = TheMessageMgr->GetLocalPlayer();
	if (player)
	{
		FighterController *controller = static_cast<const GamePlayer *>(player)->GetPlayerController();
		if (controller)
		{
			const FighterInteractor *interactor = controller->GetFighterInteractor();
			if (interactor->GetInteractionNode())
			{
				ClientInteractionMessage message(kMessageClientInteractionEnd, interactor->GetInteractionPosition());
				TheMessageMgr->SendMessage(kPlayerServer, message);
			}
		}
	}
}


WeaponAction::WeaponAction(int32 weapon) : Action(Weapon::Get(weapon)->GetActionType())
{
	weaponIndex = weapon;
}

WeaponAction::~WeaponAction()
{
}

void WeaponAction::Begin(void)
{
	const Player *player = TheMessageMgr->GetLocalPlayer();
	if ((player) && (static_cast<const GamePlayer *>(player)->GetPlayerController()))
	{
		const Weapon *weapon = Weapon::Get(weaponIndex);
		if (weapon)
		{
			TheMessageMgr->SendMessage(kPlayerServer, ClientWeaponMessage(kMessageClientSwitchWeapon, weaponIndex));
		}
	}
}


SwitchAction::SwitchAction(ActionType type) : Action(type)
{
}

SwitchAction::~SwitchAction()
{
}

void SwitchAction::Begin(void)
{
	switch (GetActionType())
	{
		case kActionSpecialWeapon:

			TheMessageMgr->SendMessage(kPlayerServer, ClientWeaponMessage(kMessageClientSpecialWeapon, kWeaponNone));
			break;

		case kActionNextWeapon:

			TheMessageMgr->SendMessage(kPlayerServer, ClientWeaponCycleMessage(1));
			break;

		case kActionPrevWeapon:

			TheMessageMgr->SendMessage(kPlayerServer, ClientWeaponCycleMessage(0));
			break;

		case kActionFlashlight:
		{
			const Player *player = TheMessageMgr->GetLocalPlayer();
			if (player)
			{
				Controller *controller = static_cast<const GamePlayer *>(player)->GetPlayerController();
				if (controller)
				{
					static_cast<FighterController *>(controller)->ToggleFlashlight();
				}
			}

			break;
		}

		case kActionCameraView:

			if (!TheMessageMgr->Multiplayer())
			{
				GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
				if (world)
				{
					world->ChangePlayerCamera();
				}
			}

			break;

		case kActionScoreboard:

			if ((TheWorldMgr->GetWorld()) && (TheMessageMgr->Multiplayer()))
			{
				if (!TheScoreBoard)
				{
					ScoreBoard::Open();
				}
				else
				{
					delete TheScoreBoard;
				}
			}

			break;

		case kActionLoad:

			if (!TheMessageMgr->Multiplayer())
			{
				if (TheGame->RestoreSinglePlayerGame("QuickSave") != kWorldOkay)
				{
					MainWindow::Open();
				}
			}

			break;

		case kActionSave:

			if ((TheWorldMgr->GetWorld()) && (!TheMessageMgr->Multiplayer()))
			{
				TheGame->SaveSinglePlayerGame("QuickSave");
				TheMessageBoard->AddText(TheGame->GetStringTable()->GetString(StringID('QSAV')));
			}

			break;
	}
}

void SwitchAction::Update(float value)
{
	ActionType type = GetActionType();

	if (type == kActionPrevWeapon)
	{
		value = -value;
	}
	else if (type != kActionNextWeapon)
	{
		return;
	}

	TheMessageMgr->SendMessage(kPlayerServer, ClientWeaponCycleMessage(value > 0.0F));
}


ChatAction::ChatAction() : Action(kActionChat)
{
}

ChatAction::~ChatAction()
{
}

void ChatAction::Begin(void)
{
	if (TheWorldMgr->GetWorld())
	{
		ChatWindow::Open();
	}
}

// ZYUQURM
