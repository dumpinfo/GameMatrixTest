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


#include "C4ConfigData.h"
#include "MGInterface.h"
#include "MGConfiguration.h"
#include "MGFighter.h"
#include "MGGame.h"


using namespace C4;


namespace
{
	enum
	{
		kServerAliveCount		= 10,
		kServerQueryInterval	= 1000,

		kChatHistoryCount		= 24,
		kMessageLineCount		= 6,
		kMessageScrollTime		= 3000
	};


	const float kInterfaceScaleHeight = 1088.0F;
}


ChatWindow *C4::TheChatWindow = nullptr;
MessageBoard *C4::TheMessageBoard = nullptr;
UsePopupBoard *C4::TheUsePopupBoard = nullptr;
NamePopupBoard *C4::TheNamePopupBoard = nullptr;
SecretPopupBoard *C4::TheSecretPopupBoard = nullptr;
ActionBoard *C4::TheActionBoard = nullptr;
DeadBoard *C4::TheDeadBoard = nullptr;
ScoreBoard *C4::TheScoreBoard = nullptr;
DisplayBoard *C4::TheDisplayBoard = nullptr;
SinglePlayerWindow *C4::TheSinglePlayerWindow = nullptr;
HostGameWindow *C4::TheHostGameWindow = nullptr;
JoinGameWindow *C4::TheJoinGameWindow = nullptr;
ConnectWindow *C4::TheConnectWindow = nullptr;
CreditsWindow *C4::TheCreditsWindow = nullptr;
MainWindow *C4::TheMainWindow = nullptr;
TeleportWindow *C4::TheTeleportWindow = nullptr;


GameBoard::GameBoard()
{
}

GameBoard::~GameBoard()
{
}


GameWindow::GameWindow(const char *panelName) : Window(panelName)
{
}

GameWindow::~GameWindow()
{
}


ChatWindow::ChatWindow() :
		GameWindow("game/Chat"),
		Singleton<ChatWindow>(TheChatWindow)
{
}

ChatWindow::~ChatWindow()
{
}

void ChatWindow::New(void)
{
	if (!TheChatWindow)
	{
		ChatWindow *window = new ChatWindow;
		TheGame->AddWindow(window);
		window->Hide();
	}
}

void ChatWindow::Open(void)
{
	TheChatWindow->UpdateDisplayPosition();
	TheInterfaceMgr->SetActiveWindow(TheChatWindow);

	TheMessageMgr->SendMessage(kPlayerServer, ClientMiscMessage(kMessageClientChatOpen));
}

void ChatWindow::Preprocess(void)
{
	GameWindow::Preprocess();

	outputText = static_cast<TextWidget *>(FindWidget("Output"));
	inputText = static_cast<EditTextWidget *>(FindWidget("Input"));
}

void ChatWindow::UpdateDisplayPosition(void)
{ 
	const Vector2D& desktopSize = TheInterfaceMgr->GetDesktopSize();
	SetWidgetPosition(Point3D(4.0F, desktopSize.y - GetWidgetSize().y - 10.0F, 0.0F));
	Invalidate(); 
}
 
void ChatWindow::AddText(const char *text)
{
	const char *history = outputText->GetText(); 
	if (history[0] != 0)
	{ 
		outputText->SetText((String<>(history) += "\n[INIT]") += text); 
	}
	else
	{
		outputText->SetText(text); 
	}

	outputText->SplitLines();
	int32 count = outputText->GetLineCount();
	if (count > kChatHistoryCount)
	{
		String<> string(outputText->GetText() + outputText->GetLineEnd(count - kChatHistoryCount - 1));
		outputText->SetText(string);
	}
}

void ChatWindow::EnterForeground(void)
{
	GameWindow::EnterForeground();

	Show();
	SetFocusWidget(inputText);
}

void ChatWindow::Close(void)
{
	Hide();
	TheMessageMgr->SendMessage(kPlayerServer, ClientMiscMessage(kMessageClientChatClose));
}

bool ChatWindow::HandleKeyboardEvent(const KeyboardEventData *eventData)
{
	EventType eventType = eventData->eventType;

	if (eventType == kEventKeyDown)
	{
		unsigned_int32 keyCode = eventData->keyCode;

		if (keyCode == kKeyCodeEnter)
		{
			const char *text = inputText->GetText();
			if (text[0] != 0)
			{
				if (!TheMessageMgr->SendChatMessage(text))
				{
					return (true);
				}

				inputText->SetText(nullptr);
			}

			if (TheEngine->GetVariable("chatOpen")->GetIntegerValue() == 0)
			{
				Close();
			}

			return (true);
		}
		else if (keyCode == kKeyCodeEscape)
		{
			Close();
			return (true);
		}
	}
	else if (eventType == kEventKeyCommand)
	{
		if (eventData->keyCode == 'W')
		{
			Close();
			return (true);
		}
	}

	return (GameWindow::HandleKeyboardEvent(eventData));
}


MessageBoard::MessageBoard() :
		Singleton<MessageBoard>(TheMessageBoard),
		textWidget(Vector2D(436.0F, 0.0F), nullptr, "font/Bold")
{
	textWidget.SetWidgetColor(K::white);
	textWidget.SetTextFlags(kTextWrapped);
	AppendSubnode(&textWidget);

	UpdateDisplayPosition();
}

MessageBoard::~MessageBoard()
{
}

void MessageBoard::New(void)
{
	if (!TheMessageBoard)
	{
		TheGame->AddBoard(new MessageBoard);
	}
}

void MessageBoard::Move(void)
{
	const char *history = textWidget.GetText();
	if (history[0] != 0)
	{
		if ((scrollTime -= TheTimeMgr->GetSystemDeltaTime()) <= 0)
		{
			scrollTime += kMessageScrollTime;

			String<> string(history + textWidget.GetLineEnd(0));
			if (string.Length() > 0)
			{
				textWidget.SetText(string);
			}
			else
			{
				textWidget.SetText(nullptr);
			}
		}
	}
}

void MessageBoard::UpdateDisplayPosition(void)
{
	float height = (float) kMessageLineCount * 15.0F + 20.0F;
	const Vector2D& desktopSize = TheInterfaceMgr->GetDesktopSize();
	SetWidgetPosition(Point3D(8.0F, desktopSize.y - TheChatWindow->GetWidgetSize().y - height, 0.0F));
	Invalidate();
}

void MessageBoard::AddText(const char *text)
{
	const char *history = textWidget.GetText();
	if (history[0] != 0)
	{
		textWidget.SetText((String<>(history) += "\n[INIT]") += text);
	}
	else
	{
		textWidget.SetText(text);
	}

	textWidget.SplitLines();
	int32 count = textWidget.GetLineCount();
	if (count > kMessageLineCount)
	{
		String<> string(textWidget.GetText() + textWidget.GetLineEnd(count - kMessageLineCount - 1));
		textWidget.SetText(string);
	}

	scrollTime = kMessageScrollTime;
	TheChatWindow->AddText(text);
}


PopupBoard::PopupBoard(const char *text, float offset) : popupText(Vector2D(256.0F, 32.0F), text, "font/Sword")
{
	popupText.SetTextAlignment(kTextAlignCenter);
	AppendSubnode(&popupText);

	popupOffset = offset + 48.0F;
	popupState = kPopupHidden;
}

PopupBoard::~PopupBoard()
{
}

void PopupBoard::Preprocess(void)
{
	GameBoard::Preprocess();
	Hide();
}

void PopupBoard::Move(void)
{
	PopupState state = popupState;
	if (state != kPopupHidden)
	{
		float alpha = popupText.GetWidgetColor().alpha;
		float da = TheTimeMgr->GetFloatDeltaTime() * 0.01F;

		if (state == kPopupVisible)
		{
			alpha = Fmin(alpha + da, 1.0F);
		}
		else
		{
			alpha -= da;
			if (alpha <= 0.0F)
			{
				alpha = 0.0F;
				popupState = kPopupHidden;
				Hide();
			}
		}

		popupText.SetWidgetAlpha(alpha);
	}
}

void PopupBoard::UpdateDisplayPosition(void)
{
	const Vector2D& desktopSize = TheInterfaceMgr->GetDesktopSize();
	SetWidgetPosition(Point3D((desktopSize.x - popupText.GetWidgetSize().x) * 0.5F, desktopSize.y * 0.5F + popupOffset, 0.0F));
	Invalidate();
}


UsePopupBoard::UsePopupBoard() :
		PopupBoard(TheGame->GetStringTable()->GetString(StringID('MISC', 'USE '))),
		Singleton<UsePopupBoard>(TheUsePopupBoard)
{
	popupText.SetWidgetColor(ColorRGBA(1.0F, 1.0F, 0.0F, 0.0F));
}

UsePopupBoard::~UsePopupBoard()
{
}

void UsePopupBoard::New(void)
{
	if (!TheUsePopupBoard)
	{
		UsePopupBoard *board = new UsePopupBoard;
		board->UpdateDisplayPosition();
		TheGame->AddBoard(board);
	}
}


NamePopupBoard::NamePopupBoard() : Singleton<NamePopupBoard>(TheNamePopupBoard)
{
	popupText.SetWidgetColor(ColorRGBA(0.75F, 0.75F, 0.75F, 0.0F));
}

NamePopupBoard::~NamePopupBoard()
{
}

void NamePopupBoard::New(void)
{
	if (!TheNamePopupBoard)
	{
		NamePopupBoard *board = new NamePopupBoard;
		board->UpdateDisplayPosition();
		TheGame->AddBoard(board);
	}
}

void NamePopupBoard::SetPlayer(Player *player)
{
	if (player != namePlayer)
	{
		namePlayer = player;
		if (player)
		{
			popupText.SetText(player->GetPlayerName());
			popupState = kPopupVisible;
			Show();
		}
		else
		{
			popupState = kPopupFading;
		}
	}
}

void NamePopupBoard::RenamePlayer(const Player *player)
{
	if (player == namePlayer)
	{
		popupText.SetText(player->GetPlayerName());
	}
}


SecretPopupBoard::SecretPopupBoard() :
		PopupBoard(nullptr, 48.0F),
		Singleton<SecretPopupBoard>(TheSecretPopupBoard)
{
	popupText.SetWidgetColor(ColorRGBA(1.0F, 1.0F, 1.0F, 0.0F));
	displayTime = 0;
}

SecretPopupBoard::~SecretPopupBoard()
{
}

void SecretPopupBoard::New(void)
{
	if (!TheSecretPopupBoard)
	{
		SecretPopupBoard *board = new SecretPopupBoard;
		board->UpdateDisplayPosition();
		TheGame->AddBoard(board);
	}
}

void SecretPopupBoard::Move(void)
{
	int32 time = displayTime;
	if (time > 0)
	{
		time -= TheTimeMgr->GetDeltaTime();
		displayTime = time;

		if (time <= 0)
		{
			popupState = kPopupFading;
		}
	}

	PopupBoard::Move();
}

void SecretPopupBoard::SetMessage(const char *message)
{
	popupText.SetText(message);
	popupState = kPopupVisible;
	displayTime = 3500;
	Show();
}


ActionBoard::ActionBoard() : Singleton<ActionBoard>(TheActionBoard)
{
	Load("hud/Action");
	UpdateDisplayPosition();
}

ActionBoard::~ActionBoard()
{
}

void ActionBoard::New(void)
{
	if (!TheActionBoard)
	{
		TheGame->AddBoard(new ActionBoard);
	}
}

void ActionBoard::Preprocess(void)
{
	GameBoard::Preprocess();

	actionText = static_cast<TextWidget *>(FindWidget("Action"));
	controlText = static_cast<TextWidget *>(FindWidget("Control"));

	displayTime = 0;
	Hide();
}

void ActionBoard::Move(void)
{
	int32 time = displayTime;
	if ((time > 0) && ((displayTime = time - TheTimeMgr->GetDeltaTime()) <= 0))
	{
		Hide();
	}
}

void ActionBoard::UpdateDisplayPosition(void)
{
	const Vector2D& desktopSize = TheInterfaceMgr->GetDesktopSize();
	float scale = desktopSize.y / kInterfaceScaleHeight;

	SetWidgetTransform(Transform4D().SetScale(scale));
	SetWidgetPosition(Point3D(0.0F, Floor(desktopSize.y - 8.0F - GetWidgetSize().y * scale), 0.0F));
	Invalidate();
}

void ActionBoard::DisplayAction(ActionType type, int32 time)
{
	StringTable *actionTable = new StringTable("game/Input");
	actionText->SetText(actionTable->GetString(StringID(type)));
	delete actionTable;

	const char *text = nullptr;
	const Action *action = TheInputMgr->FindAction(type);
	if (action)
	{
		const InputControl *control = TheInputMgr->GetActionControl(action);
		if (control)
		{
			text = control->GetControlName();
		}
	}

	controlText->SetText(text);
	displayTime = time;
	Show();

	Sound *sound = new Sound;
	sound->Load("sound/Notify");
	sound->SetSoundPriority(kSoundPriorityPlayer);
	sound->SetSoundProperty(kSoundVolume, 0.25F);
	sound->Play();
}


DeadBoard::DeadBoard() : Singleton<DeadBoard>(TheDeadBoard)
{
	Load("hud/Dead");
	UpdateDisplayPosition();
}

DeadBoard::~DeadBoard()
{
}

void DeadBoard::Open(void)
{
	if (!TheDeadBoard)
	{
		TheGame->AddBoard(new DeadBoard);
	}
}

void DeadBoard::UpdateDisplayPosition(void)
{
	const Vector2D& desktopSize = TheInterfaceMgr->GetDesktopSize();
	float scale = desktopSize.y / kInterfaceScaleHeight;

	SetWidgetTransform(Transform4D().SetScale(scale));
	SetWidgetPosition(Point3D(Floor((desktopSize.x - GetWidgetSize().x * scale) * 0.5F), Floor((desktopSize.y - GetWidgetSize().y * scale) * 0.5F), 0.0F));
	Invalidate();
}


ScoreBoard::ScoreBoard() : Singleton<ScoreBoard>(TheScoreBoard)
{
	Load("game/Scoreboard");
}

ScoreBoard::~ScoreBoard()
{
	TheMessageMgr->SendMessage(kPlayerServer, ClientMiscMessage(kMessageClientScoreClose));
}

ScoreBoard::PlayerData::PlayerData(ScoreBoard *board, GamePlayer *player) :
		nameText(Vector2D(288.0F, 0.0F), player->GetPlayerName(), "font/Sword"),
		scoreText(Vector2D(80.0F, 0.0F), String<7>(player->GetPlayerScore()), "font/Sword"),
		pingText(Vector2D(80.0F, 0.0F), nullptr, "font/Sword")
{
	static const ConstColorRGBA dataColor = {1.0F, 0.7F, 0.1F, 1.0F};

	playerLink = player;

	nameText.SetWidgetColor(dataColor);
	board->AppendNewSubnode(&nameText);

	scoreText.SetWidgetColor(dataColor);
	scoreText.SetTextAlignment(kTextAlignRight);
	board->AppendNewSubnode(&scoreText);

	int32 ping = player->GetPlayerPing();
	if (ping >= 0)
	{
		pingText.SetText(String<7>(ping));
	}
	else
	{
		pingText.SetText("\xE2\x80\x92");	// U+2012
	}

	pingText.SetWidgetColor(dataColor);
	pingText.SetTextAlignment(kTextAlignRight);
	board->AppendNewSubnode(&pingText);
}

ScoreBoard::PlayerData::~PlayerData()
{
}

void ScoreBoard::PlayerData::Update(int32 line)
{
	float y = (float) (46 + line * 28);
	nameText.SetWidgetPosition(Point3D(16.0F, y, 0.0F));
	scoreText.SetWidgetPosition(Point3D(320.0F, y, 0.0F));
	pingText.SetWidgetPosition(Point3D(420.0F, y, 0.0F));

	nameText.Invalidate();
	scoreText.Invalidate();
	pingText.Invalidate();

	const GamePlayer *player = static_cast<GamePlayer *>(playerLink.GetTarget());
	scoreText.SetText(String<7>(player->GetPlayerScore()));

	int32 ping = player->GetPlayerPing();
	if (ping >= 0)
	{
		pingText.SetText(String<7>(ping));
	}
	else
	{
		pingText.SetText("\xE2\x80\x92");	// U+2012
	}
}

void ScoreBoard::Open(void)
{
	if (!TheScoreBoard)
	{
		ScoreBoard *board = new ScoreBoard;

		const Vector2D& desktopSize = TheInterfaceMgr->GetDesktopSize();
		board->SetWidgetPosition(Point3D(desktopSize.x * 0.5F - 258.0F, desktopSize.y * 0.5F - 208.0F, 0.0F));

		TheGame->AddBoard(board);
	}
}

void ScoreBoard::Preprocess(void)
{
	GameBoard::Preprocess();

	bool server = (TheMessageMgr->Multiplayer()) && (TheMessageMgr->Server());

	if (server)
	{
		TextWidget *addressText = static_cast<TextWidget *>(FindWidget("Address"));
		addressText->AppendText(MessageMgr::AddressToString(TheNetworkMgr->GetLocalAddress()));
	}

	respawnText = static_cast<TextWidget *>(FindWidget("Respawn"));

	if (!(TheGame->GetMultiplayerFlags() & kMultiplayerDedicated))
	{
		const GamePlayer *player = static_cast<GamePlayer *>(TheMessageMgr->GetLocalPlayer());
		if ((!player) || (player->GetPlayerController()))
		{
			respawnText->Hide();
		}
	}
	else
	{
		respawnText->Hide();
		respawnText = nullptr;

		FindWidget("Server")->Show();
	}

	TheMessageMgr->SendMessage(kPlayerServer, ClientMiscMessage(kMessageClientScoreOpen));
	Refresh();
}

void ScoreBoard::Refresh(void)
{
	PlayerData *playerData = playerDataList.First();
	while (playerData)
	{
		PlayerData *next = playerData->Next();

		const GamePlayer *player = static_cast<GamePlayer *>(playerData->playerLink.GetTarget());
		if (player)
		{
			PlayerData *data = playerDataList.First();
			while (data)
			{
				if (data == playerData)
				{
					break;
				}

				if (player->GetPlayerScore() > static_cast<GamePlayer *>(data->playerLink.GetTarget())->GetPlayerScore())
				{
					playerDataList.InsertBefore(playerData, data);
					break;
				}

				data = data->Next();
			}
		}
		else
		{
			delete playerData;
		}

		playerData = next;
	}

	GamePlayer *player = static_cast<GamePlayer *>(TheMessageMgr->GetFirstPlayer());
	while (player)
	{
		if (!(player->GetPlayerFlags() & kPlayerInactive))
		{
			playerData = playerDataList.First();
			while (playerData)
			{
				if (playerData->playerLink == player)
				{
					break;
				}

				playerData = playerData->Next();
			}

			if (!playerData)
			{
				playerData = new PlayerData(this, player);

				PlayerData *data = playerDataList.First();
				while (data)
				{
					if (player->GetPlayerScore() > static_cast<GamePlayer *>(data->playerLink.GetTarget())->GetPlayerScore())
					{
						playerDataList.InsertBefore(playerData, data);
						break;
					}

					data = data->Next();
				}

				if (!playerData->GetOwningList())
				{
					playerDataList.Append(playerData);
				}
			}
		}

		player = player->Next();
	}

	int32 line = 0;
	playerData = playerDataList.First();
	while (playerData)
	{
		playerData->Update(line);
		line++;

		playerData = playerData->Next();
	}
}

void ScoreBoard::RenamePlayer(const Player *player)
{
	PlayerData *playerData = playerDataList.First();
	while (playerData)
	{
		Player *p = playerData->playerLink;
		if (p == player)
		{
			playerData->nameText.SetText(player->GetPlayerName());
			break;
		}

		playerData = playerData->Next();
	}
}

void ScoreBoard::ShowRespawnMessage(void)
{
	if (respawnText)
	{
		respawnText->Show();
	}
}


DisplayBoard::DisplayBoard() : Singleton<DisplayBoard>(TheDisplayBoard)
{
	weaponTime = 0;
	previousHealth = 0;

	AppendSubnode(&healthGroup);
	healthGroup.Load("hud/Health");

	AppendSubnode(&scoreGroup);
	scoreGroup.Load("hud/Score");

	AppendSubnode(&weaponsGroup);
	weaponsGroup.Load("hud/Weapons");

	UpdateDisplayPosition();
}

DisplayBoard::~DisplayBoard()
{
}

void DisplayBoard::Open(void)
{
	if (!TheDisplayBoard)
	{
		TheGame->AddBoard(new DisplayBoard);
	}
}

void DisplayBoard::Preprocess(void)
{
	static const char *const weaponGroupKey[kWeaponCount] =
	{
		nullptr, "Axe", "Pistol", "Shotgun", "Crossbow", "Spike", "Grenade", "Quantum", "Rocket", "Plasma", "Proton", "Cat", "Chicken", "Hornet", "Beer"
	};

	static const char *const weaponControlKey[kWeaponCount] =
	{
		nullptr, "AxeControl", "PistolControl", "ShotgunControl", "CrossbowControl", "SpikeControl", "GrenadeControl", "QuantumControl", "RocketControl", "PlasmaControl", "ProtonControl", "CatControl", "ChickenControl", "HornetControl", "BeerControl"
	};

	static const char *const ammoProgressKey[kWeaponCount] =
	{
		nullptr, nullptr, "PistolAmmo", "ShotgunAmmo", "CrossbowAmmo", "SpikeAmmo", "GrenadeAmmo", "QuantumAmmo", "RocketAmmo", "PlasmaAmmo", "ProtonAmmo", "CatAmmo", "ChickenAmmo", "HornetAmmo", "BeerAmmo"
	};

	GameBoard::Preprocess();

	scoreText = static_cast<TextWidget *>(FindWidget("Score"));
	healthText = static_cast<TextWidget *>(FindWidget("Health"));
	healthProgress = static_cast<ProgressWidget *>(FindWidget("HealthBar"));
	healthGlow = FindWidget("HealthGlow");

	equippedWeaponName = static_cast<TextWidget *>(FindWidget("Name"));
	equippedWeaponImage = static_cast<ImageWidget *>(FindWidget("Weapon"));
	equippedAmmoProgress[0] = static_cast<ProgressWidget *>(FindWidget("Ammo1"));
	equippedAmmoProgress[1] = static_cast<ProgressWidget *>(FindWidget("Ammo2"));
	equippedAmmoText[0] = static_cast<TextWidget *>(FindWidget("Count1"));
	equippedAmmoText[1] = static_cast<TextWidget *>(FindWidget("Count2"));
	ammoBackground[0] = FindWidget("AmmoBack1");
	ammoBackground[1] = FindWidget("AmmoBack2");

	arrowWidget = FindWidget("Arrow");
	spikeWidget = FindWidget("Rail");

	weaponGlow = FindWidget("WeaponGlow");
	inventoryGlow = FindWidget("InvGlow");

	for (machine a = kWeaponAxe; a < kWeaponCount; a++)
	{
		inventoryWeaponGroup[a] = FindWidget(weaponGroupKey[a]);
	}

	for (machine a = kWeaponAxe; a < kWeaponCount; a++)
	{
		inventoryWeaponControl[a] = static_cast<TextWidget *>(FindWidget(weaponControlKey[a]));
	}

	inventoryAmmoProgress[kWeaponAxe] = nullptr;
	for (machine a = kWeaponPistol; a < kWeaponCount; a++)
	{
		inventoryAmmoProgress[a] = static_cast<ProgressWidget *>(FindWidget(ammoProgressKey[a]));
	}

	UpdateWeaponControls();
}

void DisplayBoard::Move(void)
{
	int32 time = weaponTime;
	if (time > 0)
	{
		time -= TheTimeMgr->GetDeltaTime();
		weaponTime = MaxZero(time);

		if (time <= 0)
		{
			inventoryGlow->Hide();

			for (machine a = kWeaponAxe; a < kWeaponCount; a++)
			{
				Widget *widget = inventoryWeaponGroup[a];
				if (widget)
				{
					widget->Hide();
				}
			}
		}
	}

	Board::Move();
}

ColorRGBA DisplayBoard::CalculateBarColor(float value)
{
	return (ColorRGBA(FmaxZero(Fmin(3.0F - value * 6.0F, 1.0F)), FmaxZero(Fmin(value * 6.0F - 1.0F, 1.0F)), 0.0F, 1.0F));
}

void DisplayBoard::UpdateDisplayPosition(void)
{
	Transform4D		transform;

	const Vector2D& desktopSize = TheInterfaceMgr->GetDesktopSize();
	float scale = desktopSize.y / kInterfaceScaleHeight;
	transform.SetScale(scale);

	healthGroup.SetWidgetTransform(transform);

	scoreGroup.SetWidgetTransform(transform);
	scoreGroup.SetWidgetPosition(Point3D(desktopSize.x * 0.5F - scale * 144.0F, 0.0F, 0.0F));

	weaponsGroup.SetWidgetTransform(transform);
	weaponsGroup.SetWidgetPosition(Point3D(desktopSize.x - scale * 492.0F, 0.0F, 0.0F));

	Invalidate();
}

void DisplayBoard::UpdateWeaponControls(void)
{
	for (machine a = kWeaponAxe; a < kWeaponCount; a++)
	{
		TextWidget *textWidget = inventoryWeaponControl[a];
		if (textWidget)
		{
			const Weapon *weapon = Weapon::Get(a);
			if (weapon)
			{
				const char *text = nullptr;

				ActionType type = weapon->GetActionType();
				if (type == 0)
				{
					type = kActionPistol;
				}

				const Action *action = TheInputMgr->FindAction(type);
				if (action)
				{
					const InputControl *control = TheInputMgr->GetActionControl(action);
					if (control)
					{
						text = control->GetControlName();
					}
				}

				textWidget->SetText(text);
				if (text)
				{
					float textWidth = textWidget->GetFont()->GetTextWidthUTF8(text);
					const Vector2D& size = textWidget->GetWidgetSize();
					if (textWidth > size.x)
					{
						float scale = size.x / textWidth;
						textWidget->SetTextScale(scale * 0.75F);
						textWidget->SetTextRenderOffset(Vector3D(0.0F, Floor((size.y - 2.0F) * (1.0F - scale) * 0.5F), 0.0F));
					}
					else
					{
						textWidget->SetTextScale(0.75F);
						textWidget->SetTextRenderOffset(Zero3D);
					}
				}
			}
		}
	}
}

void DisplayBoard::UpdatePlayerScore(void)
{
	const Player *player = TheMessageMgr->GetLocalPlayer();
	if (player)
	{
		const PlayerState *state = static_cast<const GamePlayer *>(player)->GetPlayerState();
		scoreText->SetText(String<7>(state->playerScore));
	}
}

void DisplayBoard::UpdatePlayerHealth(void)
{
	const Player *player = TheMessageMgr->GetLocalPlayer();
	if (player)
	{
		const PlayerState *state = static_cast<const GamePlayer *>(player)->GetPlayerState();

		int32 health = (state->playerHealth + 0xFFFF) >> 16;
		ColorRGBA color = CalculateBarColor((float) MaxZero(Min(health, 100)) * 0.01F);

		healthProgress->SetValue(health);
		healthProgress->SetWidgetColor(color, kWidgetColorHilite);
		healthText->SetText(String<7>(health));

		int32 previous = previousHealth;
		previousHealth = health;

		if (health <= 20)
		{
			if (!healthGlow->Visible())
			{
				healthGlow->Show();
				healthGlow->GetFirstMutator()->SetMutatorState(0);
			}

			if ((previous > 20) && (health > 0))
			{
				Sound *sound = new Sound;
				sound->Load("sound/Bell");
				sound->SetSoundPriority(kSoundPriorityDeath);
				sound->Play();
			}
		}
		else if (healthGlow->Visible())
		{
			healthGlow->Hide();
		}
	}
}

void DisplayBoard::UpdatePlayerWeapons(void)
{
	UpdateEquippedWeapon();

	const Player *player = TheMessageMgr->GetLocalPlayer();
	if (player)
	{
		const PlayerState *state = static_cast<const GamePlayer *>(player)->GetPlayerState();
		unsigned_int32 flags = state->weaponFlags;
		float y = 89.0F;

		for (machine a = kWeaponAxe; a < kWeaponCount; a++)
		{
			flags >>= 1;

			Widget *widget = inventoryWeaponGroup[a];
			if (widget)
			{
				if (flags & 1)
				{
					widget->SetWidgetPosition(Point3D(widget->GetWidgetPosition().x, y, 0.0F));
					widget->Invalidate();
					widget->Show();

					if (state->currentWeapon == a)
					{
						inventoryGlow->SetWidgetPosition(Point3D(widget->GetWidgetPosition().x, y, 0.0F));
						inventoryGlow->Invalidate();
						inventoryGlow->Show();
					}

					if (a != kWeaponAxe)
					{
						y += 100.0F;
					}
				}
				else
				{
					widget->Hide();
				}
			}
		}
	}

	for (machine a = kWeaponPistol; a < kWeaponCount; a++)
	{
		UpdateWeaponAmmo(a);
	}

	weaponTime = 3000;
}

void DisplayBoard::UpdateEquippedWeapon(void)
{
	const Player *player = TheMessageMgr->GetLocalPlayer();
	if (player)
	{
		int32 weaponIndex = static_cast<const GamePlayer *>(player)->GetPlayerState()->currentWeapon;
		equippedWeaponImage->SetImageOffset(Vector2D(0.0F, (float) (kWeaponCount - 1 - weaponIndex) * (1.0F / (float) (kWeaponCount - 1))));

		if (weaponIndex != kWeaponAxe)
		{
			equippedAmmoProgress[0]->Show();
			equippedAmmoText[0]->Show();
			ammoBackground[0]->Show();
		}
		else
		{
			equippedAmmoProgress[0]->Hide();
			equippedAmmoText[0]->Hide();
			ammoBackground[0]->Hide();
			weaponGlow->Hide();
		}

		const Weapon *weapon = Weapon::Get(weaponIndex);
		if (weapon->GetWeaponFlags() & kWeaponSecondaryAmmo)
		{
			equippedAmmoProgress[1]->Show();
			equippedAmmoText[1]->Show();
			ammoBackground[1]->Show();

			bool showArrow = (weaponIndex == kWeaponCrossbow);
			bool showSpike = (weaponIndex == kWeaponSpikeShooter);

			if (showArrow)
			{
				arrowWidget->Show();
			}
			else
			{
				arrowWidget->Hide();
			}

			if (showSpike)
			{
				spikeWidget->Show();
			}
			else
			{
				spikeWidget->Hide();
			}
		}
		else
		{
			equippedAmmoProgress[1]->Hide();
			equippedAmmoText[1]->Hide();
			ammoBackground[1]->Hide();
			arrowWidget->Hide();
			spikeWidget->Hide();
		}

		UpdateWeaponAmmo(weaponIndex);
	}
}

void DisplayBoard::UpdateWeaponAmmo(int32 weaponIndex)
{
	const Player *player = TheMessageMgr->GetLocalPlayer();
	if (player)
	{
		ProgressWidget *progress = inventoryAmmoProgress[weaponIndex];
		if (progress)
		{
			const PlayerState *state = static_cast<const GamePlayer *>(player)->GetPlayerState();
			const Weapon *weapon = Weapon::Get(weaponIndex);

			int32 ammo = state->weaponAmmo[weaponIndex][0];
			int32 maxAmmo = weapon->GetMaxWeaponAmmo(0);
			ColorRGBA color = CalculateBarColor((float) ammo / (float) maxAmmo);

			progress->SetProgress(ammo, maxAmmo);
			progress->SetWidgetColor(color, kWidgetColorHilite);

			if ((state->currentWeapon == weaponIndex) && (weaponIndex != kWeaponAxe))
			{
				equippedAmmoProgress[0]->SetProgress(ammo, maxAmmo);
				equippedAmmoProgress[0]->SetWidgetColor(color, kWidgetColorHilite);
				equippedAmmoText[0]->SetText(String<7>(ammo));

				if (ammo <= maxAmmo / 5)
				{
					if (!weaponGlow->Visible())
					{
						weaponGlow->Show();
						weaponGlow->GetFirstMutator()->SetMutatorState(0);
					}
				}
				else if (weaponGlow->Visible())
				{
					weaponGlow->Hide();
				}

				if (weapon->GetWeaponFlags() & kWeaponSecondaryAmmo)
				{
					ammo = state->weaponAmmo[weaponIndex][1];
					maxAmmo = weapon->GetMaxWeaponAmmo(1);
					color = CalculateBarColor((float) ammo / (float) maxAmmo);

					equippedAmmoProgress[1]->SetProgress(ammo, maxAmmo);
					equippedAmmoProgress[1]->SetWidgetColor(color, kWidgetColorHilite);
					equippedAmmoText[1]->SetText(String<7>(ammo));
				}
			}
		}
	}
}


StartWindow::StartWindow(const char *panelName, const char *stringName) : GameWindow(panelName)
{
	stringTableName = stringName;
	worldTable = nullptr;
}

StartWindow::~StartWindow()
{
	delete worldTable;
}

void StartWindow::Preprocess(void)
{
	GameWindow::Preprocess();

	startButton = static_cast<PushButtonWidget *>(FindWidget("Start"));
	cancelButton = static_cast<PushButtonWidget *>(FindWidget("Cancel"));

	worldList = static_cast<ListWidget *>(FindWidget("List"));
	previewImage = static_cast<ImageWidget *>(FindWidget("Preview"));

	titleText = static_cast<TextWidget *>(FindWidget("Title"));
	authorText = static_cast<TextWidget *>(FindWidget("Author"));

	worldTable = new StringTable(stringTableName);
	const StringHeader *stringHeader = worldTable->GetRootStringHeader();
	while (stringHeader)
	{
		if (stringHeader->GetFirstSubstring())
		{
			unsigned_int32 id = stringHeader->GetStringID();
			worldList->AppendListItem(worldTable->GetString(StringID(id, 'TITL')));
		}
		else
		{
			const char *name = stringHeader->GetString();
			worldList->AppendListItem(&name[Text::GetDirectoryPathLength(name)]);
		}

		stringHeader = stringHeader->GetNextString();
	}

	worldSelection = nullptr;
	worldList->SelectListItem(0);

	SetFocusWidget(worldList);
	UpdatePreview();
}

const char *StartWindow::GetSelectedWorldName(unsigned_int32 *stringID) const
{
	const Widget *listItem = worldList->GetFirstSelectedListItem();
	if (listItem)
	{
		int32 index = listItem->GetNodeIndex();

		const StringHeader *stringHeader = worldTable->GetRootStringHeader();
		while (stringHeader)
		{
			if (index == 0)
			{
				if ((stringHeader->GetFirstSubstring()) && (stringID))
				{
					*stringID = stringHeader->GetStringID();
				}

				return (stringHeader->GetString());
			}

			index--;
			stringHeader = stringHeader->GetNextString();
		}
	}

	return (nullptr);
}

void StartWindow::UpdatePreview(void)
{
	const Widget *listItem = worldList->GetFirstSelectedListItem();
	if (listItem != worldSelection)
	{
		worldSelection = listItem;

		unsigned_int32 id = 0;
		previewImage->SetTexture(0, GetSelectedWorldName(&id));

		if (id != 0)
		{
			titleText->SetText(worldTable->GetString(StringID(id, 'TITL')));
			authorText->SetText(worldTable->GetString(StringID(id, 'AUTH')));
		}
		else
		{
			titleText->SetText(nullptr);
			authorText->SetText(nullptr);
		}
	}
}

void StartWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	EventType eventType = eventData->eventType;

	if (eventType == kEventWidgetActivate)
	{
		if (widget == cancelButton)
		{
			CallCompletionProc();
			delete this;
		}
		else if (widget == worldList)
		{
			startButton->Activate();
		}
	}
	else if (eventType == kEventWidgetChange)
	{
		if (widget == worldList)
		{
			UpdatePreview();
		}
	}
}


SinglePlayerWindow::SinglePlayerWindow() :
		StartWindow("game/Start", "game/Worlds"),
		Singleton<SinglePlayerWindow>(TheSinglePlayerWindow)
{
}

SinglePlayerWindow::~SinglePlayerWindow()
{
}

void SinglePlayerWindow::Open(void)
{
	if (TheSinglePlayerWindow)
	{
		TheInterfaceMgr->SetActiveWindow(TheSinglePlayerWindow);
	}
	else
	{
		TheGame->AddWindow(new SinglePlayerWindow);
	}
}

void SinglePlayerWindow::Move(void)
{
	if (worldList->GetFirstSelectedListItem())
	{
		startButton->Enable();
	}
	else
	{
		startButton->Disable();
	}

	StartWindow::Move();
}

void SinglePlayerWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if ((eventData->eventType == kEventWidgetActivate) && (widget == startButton))
	{
		ResourceName name(GetSelectedWorldName());
		delete this;

		TheGame->StartSinglePlayerGame(name);
		MainWindow::ReturnToGame();
	}
	else
	{
		StartWindow::HandleWidgetEvent(widget, eventData);
	}
}


HostGameWindow::HostGameWindow() :
		StartWindow("game/Host", "game/MP"),
		Singleton<HostGameWindow>(TheHostGameWindow)
{
}

HostGameWindow::~HostGameWindow()
{
}

void HostGameWindow::Open(void)
{
	if (TheHostGameWindow)
	{
		TheInterfaceMgr->SetActiveWindow(TheHostGameWindow);
	}
	else
	{
		TheGame->AddWindow(new HostGameWindow);
	}
}

void HostGameWindow::Preprocess(void)
{
	StartWindow::Preprocess();

	gameNameText = static_cast<EditTextWidget *>(FindWidget("Name"));
	maxPlayersText = static_cast<EditTextWidget *>(FindWidget("Max"));
	playHereBox = static_cast<CheckWidget *>(FindWidget("Play"));

	gameNameText->SetText(TheEngine->GetVariable("gameName")->GetValue());
	maxPlayersText->SetText(Text::IntegerToString(TheMessageMgr->GetMaxPlayerCount()));
}

void HostGameWindow::Move(void)
{
	if ((worldList->GetFirstSelectedListItem()) && (gameNameText->GetText()[0] != 0) && (Text::StringToInteger(maxPlayersText->GetText()) > 1))
	{
		startButton->Enable();
	}
	else
	{
		startButton->Disable();
	}

	StartWindow::Move();
}

void HostGameWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if ((eventData->eventType == kEventWidgetActivate) && (widget == startButton))
	{
		TheGame->ExitCurrentGame();

		const char *gameName = gameNameText->GetText();
		TheEngine->GetVariable("gameName")->SetValue(gameName);
		ConfigDataDescription::WriteEngineConfig();

		TheMessageMgr->SetMaxPlayerCount(Text::StringToInteger(maxPlayersText->GetText()));
		unsigned_int32 flags = (playHereBox->GetValue() == 0) ? kMultiplayerDedicated : 0;

		ResourceName name(GetSelectedWorldName());
		delete this;

		TheGame->HostMultiplayerGame(name, flags);
		MainWindow::ReturnToGame();
	}
	else
	{
		StartWindow::HandleWidgetEvent(widget, eventData);
	}
}


JoinGameWindow::JoinGameWindow() :
		GameWindow("game/Join"),
		Singleton<JoinGameWindow>(TheJoinGameWindow)
{
}

JoinGameWindow::~JoinGameWindow()
{
	if (!TheMessageMgr->Multiplayer())
	{
		TheNetworkMgr->Terminate();
	}
}

void JoinGameWindow::Open(void)
{
	if (TheJoinGameWindow)
	{
		TheInterfaceMgr->SetActiveWindow(TheJoinGameWindow);
	}
	else
	{
		TheGame->AddWindow(new JoinGameWindow);
	}
}

JoinGameWindow::GameWidget::GameWidget(const NetworkAddress& address, const ServerInfoMessage *message) :
		worldName(message->GetWorldName()),
		nameText(Vector2D(206.0F, 13.0F), message->GetGameName(), "font/Gui"),
		playerText(Vector2D(56.0F, 13.0F), (String<31>(message->GetPlayerCount()) += '/') += String<7>(message->GetMaxPlayerCount()), "font/Gui"),
		worldText(Vector2D(136.0F, 13.0F), &worldName[Text::GetDirectoryPathLength(worldName)], "font/Gui")
{
	aliveCount = kServerAliveCount;
	gameAddress = address;

	nameText.SetWidgetPosition(Point3D(2.0F, 0.0F, 0.0F));
	AppendSubnode(&nameText);

	playerText.SetWidgetPosition(Point3D(232.0F, 0.0F, 0.0F));
	playerText.SetTextAlignment(kTextAlignCenter);
	AppendSubnode(&playerText);

	worldText.SetWidgetPosition(Point3D(320.0F, 0.0F, 0.0F));
	AppendSubnode(&worldText);
}

JoinGameWindow::GameWidget::~GameWidget()
{
}

void JoinGameWindow::Preprocess(void)
{
	GameWindow::Preprocess();

	joinButton = static_cast<PushButtonWidget *>(FindWidget("Start"));
	cancelButton = static_cast<PushButtonWidget *>(FindWidget("Cancel"));

	addressBox = static_cast<EditTextWidget *>(FindWidget("Address"));
	gameList = static_cast<ListWidget *>(FindWidget("List"));
	previewImage = static_cast<ImageWidget *>(FindWidget("Preview"));

	auto filter = [](unsigned_int32 code) -> bool
	{
		return ((code - 0x0030 < 10U) || (code == '.') || (code == ':'));
	};

	addressBox->SetFilterProc(filter);

	queryTime = 0;
	SetFocusWidget(gameList);

	if (!TheMessageMgr->Multiplayer())
	{
		TheNetworkMgr->SetProtocol(kGameProtocol);
		TheNetworkMgr->SetPortNumber(0);
		TheNetworkMgr->SetBroadcastPortNumber(kGamePort);
		TheNetworkMgr->Initialize();
	}
}

void JoinGameWindow::Move(void)
{
	if ((queryTime -= TheTimeMgr->GetSystemDeltaTime()) <= 0)
	{
		queryTime = kServerQueryInterval;
		TheMessageMgr->BroadcastServerQuery();

		GameWidget *gameWidget = static_cast<GameWidget *>(gameList->GetFirstListItem());
		while (gameWidget)
		{
			GameWidget *next = static_cast<GameWidget *>(gameWidget->Next());

			if (--gameWidget->aliveCount == 0)
			{
				if (gameWidget == gameList->GetFirstSelectedListItem())
				{
					previewImage->SetTexture(0, "C4/black");
				}

				gameList->RemoveListItem(gameWidget);
				delete gameWidget;
			}

			gameWidget = next;
		}
	}

	GameWidget *listItem = static_cast<GameWidget *>(gameList->GetFirstSelectedListItem());
	if (listItem)
	{
		addressBox->SetText(MessageMgr::AddressToString(listItem->gameAddress, true));
	}

	if ((listItem) || (addressBox->GetText()[0] != 0))
	{
		joinButton->Enable();
	}
	else
	{
		joinButton->Disable();
	}

	GameWindow::Move();
}

void JoinGameWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	EventType eventType = eventData->eventType;

	if (eventType == kEventWidgetActivate)
	{
		if ((widget == joinButton) || (widget == gameList))
		{
			NetworkAddress	address;

			TheGame->ExitCurrentGame();
			MainWindow::ReturnToGame();
			TheMessageMgr->BeginMultiplayerGame(false);

			const Widget *listItem = gameList->GetFirstSelectedListItem();
			if (listItem)
			{
				address = static_cast<const GameWidget *>(listItem)->gameAddress;
			}
			else
			{
				address = MessageMgr::StringToAddress(addressBox->GetText());
				if (address.GetPort() == 0)
				{
					address.SetPort(kGamePort);
				}
			}

			delete this;

			TheMessageMgr->Connect(address);
			ConnectWindow::Open(address);
		}
		else if (widget == cancelButton)
		{
			CallCompletionProc();
			delete this;
		}
	}
	else if (eventType == kEventWidgetChange)
	{
		if (widget == addressBox)
		{
			gameList->UnselectAllListItems();
		}
		else if (widget == gameList)
		{
			const Widget *listItem = gameList->GetFirstSelectedListItem();
			if (listItem)
			{
				previewImage->SetTexture(0, static_cast<const GameWidget *>(listItem)->worldName);
			}
			else
			{
				previewImage->SetTexture(0, "C4/black");
			}
		}
	}
}

void JoinGameWindow::ReceiveServerInfo(const NetworkAddress& address, const ServerInfoMessage *message)
{
	GameWidget *gameWidget = static_cast<GameWidget *>(gameList->GetFirstListItem());
	while (gameWidget)
	{
		if (gameWidget->gameAddress == address)
		{
			gameWidget->aliveCount = kServerAliveCount;
			return;
		}

		gameWidget = static_cast<GameWidget *>(gameWidget->Next());
	}

	gameList->AppendListItem(new GameWidget(address, message));
}


ConnectWindow::ConnectWindow(const NetworkAddress& address) :
		GameWindow("game/Connect"),
		Singleton<ConnectWindow>(TheConnectWindow)
{
	connectAddress = address;
}

ConnectWindow::~ConnectWindow()
{
}

void ConnectWindow::Open(const NetworkAddress& address)
{
	if (!TheConnectWindow)
	{
		TheGame->AddWindow(new ConnectWindow(address));
	}
}

void ConnectWindow::Preprocess(void)
{
	GameWindow::Preprocess();

	messageText = static_cast<TextWidget *>(FindWidget("Message"));
	progressBar = static_cast<ProgressWidget *>(FindWidget("Progress"));
	cancelButton = static_cast<PushButtonWidget *>(FindWidget("Cancel"));

	messageText->SetText(String<>(messageText->GetText()) += MessageMgr::AddressToString(connectAddress, false));
	progressBar->SetMaxValue(TheNetworkMgr->GetReliableResendCount() * TheNetworkMgr->GetReliableResendTime());
}

void ConnectWindow::Move(void)
{
	GameWindow::Move();

	progressBar->SetValue(progressBar->GetValue() + TheTimeMgr->GetSystemDeltaTime());
}

void ConnectWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if ((eventData->eventType == kEventWidgetActivate) && (widget == cancelButton))
	{
		TheGame->ExitCurrentGame();

		delete this;
		MainWindow::Open();
	}
}

void ConnectWindow::SetAcceptMessage(const char *text)
{
	messageText->SetText(text);

	progressBar->SetWidgetColor(ColorRGBA(0.0F, 0.875F, 0.0F, 1.0F));
	progressBar->SetValue(progressBar->GetMaxValue());
}

void ConnectWindow::SetFailMessage(const char *text)
{
	messageText->SetText(text);

	progressBar->SetWidgetColor(ColorRGBA(1.0F, 0.0F, 0.0F, 1.0F));
	progressBar->SetValue(progressBar->GetMaxValue());
}


CreditsWindow::CreditsWindow() :
		GameWindow("game/Credits"),
		Singleton<CreditsWindow>(TheCreditsWindow)
{
}

CreditsWindow::~CreditsWindow()
{
}

void CreditsWindow::Open(void)
{
	if (TheCreditsWindow)
	{
		TheInterfaceMgr->SetActiveWindow(TheCreditsWindow);
	}
	else
	{
		TheGame->AddWindow(new CreditsWindow);
	}
}

void CreditsWindow::Preprocess(void)
{
	GameWindow::Preprocess();

	okayButton = static_cast<PushButtonWidget *>(FindWidget("OK"));
}

bool CreditsWindow::HandleKeyboardEvent(const KeyboardEventData *eventData)
{
	if (eventData->eventType == kEventKeyDown)
	{
		if (eventData->keyCode == kKeyCodeEnter)
		{
			okayButton->Activate();
			return (true);
		}
	}

	return (GameWindow::HandleKeyboardEvent(eventData));
}

void CreditsWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if ((eventData->eventType == kEventWidgetActivate) && (widget == okayButton))
	{
		Close();
		MainWindow::Open();
	}
}


MainWindow::MainWindow() :
		GameWindow((TheWorldMgr->GetWorld()) ? "game/Main2" : "game/Main1"),
		Singleton<MainWindow>(TheMainWindow)
{
}

MainWindow::~MainWindow()
{
}

void MainWindow::Open(void)
{
	if (TheMainWindow)
	{
		TheInterfaceMgr->SetActiveWindow(TheMainWindow);
	}
	else
	{
		TheGame->AddWindow(new MainWindow);
	}
}

void MainWindow::ReturnToGame(void)
{
	TheGame->StopTitleMusic();

	World *world = TheWorldMgr->GetWorld();
	if (world)
	{
		world->SetWorldFlags(world->GetWorldFlags() & ~kWorldPaused);
	}

	TheTimeMgr->SetWorldTimeMultiplier(1.0F);
}

void MainWindow::SinglePlayerComplete(StartWindow *window, void *cookie)
{
	Open();
}

void MainWindow::LoadSavedGameComplete(FilePicker *picker, void *cookie)
{
	TheGame->RestoreSinglePlayerGame(picker->GetResourceName());
	ReturnToGame();
}

void MainWindow::SaveCurrentGameComplete(FilePicker *picker, void *cookie)
{
	TheGame->SaveSinglePlayerGame(picker->GetResourceName());
	delete static_cast<MainWindow *>(cookie);
	ReturnToGame();
}

void MainWindow::HostGameComplete(StartWindow *window, void *cookie)
{
	Open();
}

void MainWindow::JoinGameComplete(JoinGameWindow *window, void *cookie)
{
	Open();
}

void MainWindow::PlayerSettingsComplete(PlayerSettingsWindow *window, void *cookie)
{
	Open();
}

void MainWindow::ControlSettingsComplete(ControlSettingsWindow *window, void *cookie)
{
	Open();
}

void MainWindow::GraphicsSettingsComplete(GraphicsSettingsWindow *window, void *cookie)
{
	Open();
}

void MainWindow::AudioSettingsComplete(AudioSettingsWindow *window, void *cookie)
{
	Open();
}

void MainWindow::Preprocess(void)
{
	GameWindow::Preprocess();

	newButton = static_cast<TextButtonWidget *>(FindWidget("New"));
	loadButton = static_cast<TextButtonWidget *>(FindWidget("Load"));
	saveButton = static_cast<TextButtonWidget *>(FindWidget("Save"));
	exitButton = static_cast<TextButtonWidget *>(FindWidget("Exit"));

	hostButton = static_cast<TextButtonWidget *>(FindWidget("Host"));
	joinButton = static_cast<TextButtonWidget *>(FindWidget("Join"));

	playerButton = static_cast<TextButtonWidget *>(FindWidget("Player"));
	controlsButton = static_cast<TextButtonWidget *>(FindWidget("Controls"));
	graphicsButton = static_cast<TextButtonWidget *>(FindWidget("Graphics"));
	audioButton = static_cast<TextButtonWidget *>(FindWidget("Audio"));

	creditsButton = static_cast<TextButtonWidget *>(FindWidget("Credits"));
	quitButton = static_cast<TextButtonWidget *>(FindWidget("Quit"));

	World *world = TheWorldMgr->GetWorld();
	if (world)
	{
		if (!TheMessageMgr->Multiplayer())
		{
			saveButton->Enable();

			world->SetWorldFlags(world->GetWorldFlags() | kWorldPaused);
			TheTimeMgr->SetWorldTimeMultiplier(0.0F);
		}
	}
	else
	{
		TheGame->StartTitleMusic();
		TheInterfaceMgr->GetStrip()->Show();
	}
}

bool MainWindow::HandleKeyboardEvent(const KeyboardEventData *eventData)
{
	if (eventData->eventType == kEventKeyDown)
	{
		if (eventData->keyCode == kKeyCodeEscape)
		{
			delete this;
			ReturnToGame();

			return (true);
		}
	}

	return (GameWindow::HandleKeyboardEvent(eventData));
}

void MainWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		if (widget == newButton)
		{
			SinglePlayerWindow::Open();
			TheSinglePlayerWindow->SetCompletionProc(&SinglePlayerComplete);
			delete this;
		}
		else if (widget == loadButton)
		{
			const StringTable *table = TheGame->GetStringTable();
			const char *title = table->GetString(StringID('LOAD'));

			FilePicker *picker = new FilePicker('GAME', title, TheResourceMgr->GetSaveCatalog(), SaveResource::GetDescriptor(), nullptr, kFilePickerEnableRootDirectory | kFilePickerStayInsideRootDirectory);
			picker->SetCompletionProc(&LoadSavedGameComplete);
			AddSubwindow(picker);
		}
		else if (widget == saveButton)
		{
			const StringTable *table = TheGame->GetStringTable();
			const char *title = table->GetString(StringID('SAVE'));

			FilePicker *picker = new FilePicker('GAME', title, TheResourceMgr->GetSaveCatalog(), SaveResource::GetDescriptor(), nullptr, kFilePickerSave | kFilePickerEnableRootDirectory | kFilePickerStayInsideRootDirectory);
			picker->SetCompletionProc(&SaveCurrentGameComplete, this);
			AddSubwindow(picker);
		}
		else if (widget == hostButton)
		{
			HostGameWindow::Open();
			TheHostGameWindow->SetCompletionProc(&HostGameComplete);
			delete this;
		}
		else if (widget == joinButton)
		{
			JoinGameWindow::Open();
			TheJoinGameWindow->SetCompletionProc(&JoinGameComplete);
			delete this;
		}
		else if (widget == playerButton)
		{
			PlayerSettingsWindow::Open();
			ThePlayerSettingsWindow->SetCompletionProc(&PlayerSettingsComplete);
			delete this;
		}
		else if (widget == controlsButton)
		{
			ControlSettingsWindow::Open();
			TheControlSettingsWindow->SetCompletionProc(&ControlSettingsComplete);
			delete this;
		}
		else if (widget == graphicsButton)
		{
			GraphicsSettingsWindow::Open();
			TheGraphicsSettingsWindow->SetCompletionProc(&GraphicsSettingsComplete);
			delete this;
		}
		else if (widget == audioButton)
		{
			AudioSettingsWindow::Open();
			TheAudioSettingsWindow->SetCompletionProc(&AudioSettingsComplete);
			delete this;
		}
		else if (widget == exitButton)
		{
			TheGame->ExitCurrentGame();
			delete this;
			Open();
		}
		else if (widget == creditsButton)
		{
			delete this;
			CreditsWindow::Open();
		}
		else if (widget == quitButton)
		{
			TheEngine->Quit();
		}
	}
}


LoadWindow::LoadWindow(World *world) : GameWindow("game/Load")
{
	loadWorld = world;
	loadComplete = false;
	warmupInitialized = false;
}

LoadWindow::~LoadWindow()
{
	delete loadWorld;
}

void LoadWindow::Preprocess(void)
{
	GameWindow::Preprocess();

	progressWidget = static_cast<ProgressWidget *>(FindWidget("Progress"));
	startButton = static_cast<TextButtonWidget *>(FindWidget("Start"));

	TheInterfaceMgr->DisableQuit();
	TheInterfaceMgr->DisableConsole();

	TheWorldMgr->RunLoaderTask(&LoadTask, this);
}

void LoadWindow::LoadTask(void *cookie)
{
	LoadWindow *loadWindow = static_cast<LoadWindow *>(cookie);
	loadWindow->loadResult = loadWindow->loadWorld->Preprocess();

	Thread::Fence();
	loadWindow->loadComplete = true;
}

void LoadWindow::Move(void)
{
	GameWindow::Move();

	if (!startButton->Visible())
	{
		if (!loadComplete)
		{
			const LoadContext *loadContext = loadWorld->GetLoadContext();
			progressWidget->SetMaxValue(loadContext->loadMagnitude);
			progressWidget->SetValue(loadContext->loadProgress);
		}
		else if (!warmupInitialized)
		{
			warmupInitialized = true;
			progressWidget->SetValue(progressWidget->GetMaxValue());

			TheInterfaceMgr->EnableQuit();
			TheInterfaceMgr->EnableConsole();

			if (loadResult != kWorldOkay)
			{
				delete this;
			}
		}
		else if (!loadWorld->Warmup())
		{
			TheWorldMgr->RunWorld(loadWorld);
			CallCompletionProc();
			loadWorld = nullptr;
			delete this;
		}
	}
}


SummaryWindow::SummaryWindow(const char *nextWorld) : GameWindow("game/Summary")
{
	unloadedFlag = false;
	musicSound = nullptr;
	nextWorldName = nextWorld;
}

SummaryWindow::~SummaryWindow()
{
	if (musicSound)
	{
		musicSound->Release();
	}
}

void SummaryWindow::Preprocess(void)
{
	GameWindow::Preprocess();

	continueButton = static_cast<TextButtonWidget *>(FindWidget("Continue"));
	killsText = static_cast<TextWidget *>(FindWidget("Kills"));
	secretsText = static_cast<TextWidget *>(FindWidget("Secrets"));
	scoreText = static_cast<TextWidget *>(FindWidget("Score"));

	const GamePlayer *player = static_cast<GamePlayer *>(TheMessageMgr->GetLocalPlayer());
	killsText->SetText(Text::IntegerToString(player->GetPlayerKillCount()));
	secretsText->SetText((String<31>(SecretScriptController::GetFoundSecretCount()) += '/') += SecretScriptController::GetTotalSecretCount());
	scoreText->SetText(Text::IntegerToString(player->GetPlayerScore()));
}

void SummaryWindow::Move(void)
{
	GameWindow::Move();

	if (!unloadedFlag)
	{
		unloadedFlag = true;

		const Node *root = TheWorldMgr->GetWorld()->GetRootNode();
		Node *node = root->GetFirstSubnode();
		while (node)
		{
			if (node->GetNodeType() == kNodeSource)
			{
				Source *source = static_cast<Source *>(node);
				if ((source->GetSourceType() == kSourceAmbient) && (source->Playing()))
				{
					Sound *sound = static_cast<AmbientSource *>(source)->ExtractSound();
					if (sound)
					{
						musicSound = sound;
						sound->VaryVolume(0.0F, 2000, true);
						break;
					}
				}
			}

			node = node->Next();
		}

		TheGame->ExitCurrentGame();
	}
}

bool SummaryWindow::HandleKeyboardEvent(const KeyboardEventData *eventData)
{
	if (eventData->eventType == kEventKeyDown)
	{
		unsigned_int32 keyCode = eventData->keyCode;
		if ((keyCode == kKeyCodeEnter) || (keyCode == kKeyCodeEscape))
		{
			continueButton->Activate();
			return (true);
		}
	}

	return (GameWindow::HandleKeyboardEvent(eventData));
}

void SummaryWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		if (widget == continueButton)
		{
			ResourceName name = nextWorldName;
			delete this;

			if (name[0] != 0)
			{
				TheGame->StartSinglePlayerGame(name);
			}
			else
			{
				MainWindow::Open();
			}
		}
	}
}


TeleportWindow::TeleportWindow() :
		GameWindow("game/Teleport"),
		Singleton<TeleportWindow>(TheTeleportWindow)
{
}

TeleportWindow::~TeleportWindow()
{
}

void TeleportWindow::Open(void)
{
	if (TheTeleportWindow)
	{
		TheInterfaceMgr->SetActiveWindow(TheTeleportWindow);
	}
	else
	{
		TheGame->AddWindow(new TeleportWindow);
	}
}

void TeleportWindow::Preprocess(void)
{
	GameWindow::Preprocess();

	goButton = static_cast<PushButtonWidget *>(FindWidget("Go"));
	cancelButton = static_cast<PushButtonWidget *>(FindWidget("Cancel"));
	listWidget = static_cast<ListWidget *>(FindWidget("List"));

	const Marker *marker = static_cast<GameWorld *>(TheWorldMgr->GetWorld())->GetFirstTeleportLocator();
	while (marker)
	{
		const char *name = marker->GetNodeName();
		if (name)
		{
			listWidget->AppendListItem(name);
		}

		marker = marker->Next();
	}

	if (listWidget->GetFirstListItem())
	{
		listWidget->SelectListItem(0);
		SetFocusWidget(listWidget);
	}
	else
	{
		goButton->Disable();
	}
}

bool TeleportWindow::HandleKeyboardEvent(const KeyboardEventData *eventData)
{
	if (eventData->eventType == kEventKeyDown)
	{
		unsigned_int32 keyCode = eventData->keyCode;

		if (keyCode == kKeyCodeEnter)
		{
			goButton->Activate();
			return (true);
		}
		else if (keyCode == kKeyCodeEscape)
		{
			Close();
			return (true);
		}
	}

	return (GameWindow::HandleKeyboardEvent(eventData));
}

void TeleportWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		if ((widget == goButton) || (widget == listWidget))
		{
			const Widget *listItem = listWidget->GetFirstSelectedListItem();
			if (listItem)
			{
				Teleport(static_cast<const TextWidget *>(listItem)->GetText());
			}
		}

		Close();
	}
}

void TeleportWindow::Teleport(const char *teleportName)
{
	const Marker *marker = static_cast<GameWorld *>(TheWorldMgr->GetWorld())->GetFirstTeleportLocator();
	while (marker)
	{
		const char *name = marker->GetNodeName();
		if ((name) && (Text::CompareTextCaseless(name, teleportName)))
		{
			GamePlayer *player = static_cast<GamePlayer *>(TheMessageMgr->GetLocalPlayer());
			if (player)
			{
				FighterController *playerController = player->GetPlayerController();

				const Vector3D& direction = marker->GetWorldTransform()[0];
				float azm = Atan(direction.y, direction.x);
				float alt = Atan(direction.z);

				const Point3D& position = marker->GetWorldPosition();
				Point3D center = playerController->GetFinalPosition();
				center.z += 1.0F;

				TheMessageMgr->SendMessageAll(FighterTeleportMessage(playerController->GetControllerIndex(), position, Zero3D, azm, alt, center));

				Controller *markerController = marker->GetController();
				if (markerController)
				{
					markerController->Activate(playerController->GetTargetNode(), nullptr);
				}
			}

			TheGame->ClearInterface();
			break;
		}

		marker = marker->Next();
	}
}

// ZYUQURM
