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


#ifndef MGInterface_h
#define MGInterface_h


#include "C4World.h"
#include "C4FilePicker.h"
#include "MGMultiplayer.h"


namespace C4
{
	class PlayerSettingsWindow;
	class GraphicsSettingsWindow;
	class AudioSettingsWindow;
	class ControlSettingsWindow;


	class GameBoard : public Board, public ListElement<GameBoard>
	{
		protected:

			GameBoard();

		public:

			~GameBoard();
	};


	class GameWindow : public Window, public ListElement<GameWindow>
	{
		protected:

			GameWindow(const char *panelName);

		public:

			virtual ~GameWindow();
	};


	class ChatWindow : public GameWindow, public Singleton<ChatWindow>
	{
		private:

			TextWidget			*outputText;
			EditTextWidget		*inputText;

			ChatWindow();

		public:

			~ChatWindow();

			static void New(void);
			static void Open(void);

			void Preprocess(void) override;

			void UpdateDisplayPosition(void);
			void AddText(const char *text);

			void EnterForeground(void) override;
			void Close(void) override;

			bool HandleKeyboardEvent(const KeyboardEventData *eventData) override;
	};


	class PopupBoard : public GameBoard
	{
		private:

			float			popupOffset;

		protected:

			enum PopupState
			{
				kPopupHidden,
				kPopupVisible,
				kPopupFading
			};

			PopupState		popupState;
			TextWidget		popupText;

			PopupBoard(const char *text = nullptr, float offset = 0.0F);

		public:

			~PopupBoard();

			void InstantHide(void)
			{
				popupState = kPopupHidden;
				popupText.SetWidgetAlpha(0.0F);
				Hide();
			}

			void Preprocess(void) override; 
			void Move(void) override;

			void UpdateDisplayPosition(void); 
	};
 

	class MessageBoard : public GameBoard, public Singleton<MessageBoard>
	{ 
		private:
 
			int32			scrollTime; 
			TextWidget		textWidget;

			MessageBoard();
 
		public:

			~MessageBoard();

			void InstantHide(void)
			{
				textWidget.SetText(nullptr);
			}

			static void New(void);

			void Move(void) override;

			void UpdateDisplayPosition(void);
			void AddText(const char *text);
	};


	class UsePopupBoard : public PopupBoard, public Singleton<UsePopupBoard>
	{
		private:

			UsePopupBoard();

		public:

			~UsePopupBoard();

			static void New(void);

			void Engage(void)
			{
				popupState = kPopupVisible;
				Show();
			}

			void Disengage(void)
			{
				popupState = kPopupFading;
			}
	};


	class NamePopupBoard : public PopupBoard, public Singleton<NamePopupBoard>
	{
		private:

			Link<Player>	namePlayer;

			NamePopupBoard();

		public:

			~NamePopupBoard();

			static void New(void);

			void SetPlayer(Player *player);
			void RenamePlayer(const Player *player);
	};


	class SecretPopupBoard : public PopupBoard, public Singleton<SecretPopupBoard>
	{
		private:

			int32		displayTime;

			SecretPopupBoard();

		public:

			~SecretPopupBoard();

			static void New(void);

			void Move(void) override;

			void SetMessage(const char *message);
	};


	class ActionBoard : public GameBoard, public Singleton<ActionBoard>
	{
		private:

			int32			displayTime;

			TextWidget		*actionText;
			TextWidget		*controlText;

			ActionBoard();

		public:

			~ActionBoard();

			static void New(void);

			void Preprocess(void) override;
			void Move(void) override;

			void UpdateDisplayPosition(void);
			void DisplayAction(ActionType type, int32 time);
	};


	class DeadBoard : public GameBoard, public Singleton<DeadBoard>
	{
		private:

			DeadBoard();

		public:

			~DeadBoard();

			static void Open(void);

			void UpdateDisplayPosition(void);
	};


	class ScoreBoard : public GameBoard, public Singleton<ScoreBoard>
	{
		private:

			struct PlayerData : public ListElement<PlayerData>
			{
				Link<Player>		playerLink;

				TextWidget			nameText;
				TextWidget			scoreText;
				TextWidget			pingText;

				PlayerData(ScoreBoard *board, GamePlayer *player);
				~PlayerData();

				void Update(int32 line);
			};

			TextWidget			*respawnText;
			List<PlayerData>	playerDataList;

			ScoreBoard();

		public:

			~ScoreBoard();

			static void Open(void);

			void Preprocess(void) override;

			void Refresh(void);
			void RenamePlayer(const Player *player);
			void ShowRespawnMessage(void);
	};


	class DisplayBoard : public GameBoard, public Singleton<DisplayBoard>
	{
		private:

			int32				weaponTime;
			int32				previousHealth;

			TextWidget			*scoreText;
			TextWidget			*healthText;
			ProgressWidget		*healthProgress;
			Widget				*healthGlow;

			TextWidget			*equippedWeaponName;
			ImageWidget			*equippedWeaponImage;
			ProgressWidget		*equippedAmmoProgress[2];
			TextWidget			*equippedAmmoText[2];
			Widget				*ammoBackground[2];

			Widget				*arrowWidget;
			Widget				*spikeWidget;

			Widget				*weaponGlow;
			Widget				*inventoryGlow;

			Widget				*inventoryWeaponGroup[kWeaponCount];
			TextWidget			*inventoryWeaponControl[kWeaponCount];
			ProgressWidget		*inventoryAmmoProgress[kWeaponCount];

			Widget				healthGroup;
			Widget				scoreGroup;
			Widget				weaponsGroup;

			static ColorRGBA CalculateBarColor(float value);


		public:

			DisplayBoard();
			~DisplayBoard();

			static void Open(void);

			void ShowShield(void)
			{
			}

			void HideShield(void)
			{
			}

			void Preprocess(void) override;
			void Move(void) override;

			void UpdateDisplayPosition(void);
			void UpdateWeaponControls(void);

			void UpdatePlayerScore(void);
			void UpdatePlayerHealth(void);
			void UpdatePlayerWeapons(void);
			void UpdateEquippedWeapon(void);
			void UpdateWeaponAmmo(int32 weaponIndex);
	};


	class StartWindow : public GameWindow, public Completable<StartWindow>
	{
		protected:

			const char				*stringTableName;

			PushButtonWidget		*startButton;
			PushButtonWidget		*cancelButton;

			ListWidget				*worldList;
			ImageWidget				*previewImage;

			TextWidget				*titleText;
			TextWidget				*authorText;

			StringTable				*worldTable;
			const Widget			*worldSelection;

			StartWindow(const char *panelName, const char *stringName);

			void UpdatePreview(void);

		public:

			~StartWindow();

			const char *GetSelectedWorldName(unsigned_int32 *stringID = nullptr) const;

			void Preprocess(void) override;
			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};


	class SinglePlayerWindow : public StartWindow, public Singleton<SinglePlayerWindow>
	{
		private:

			SinglePlayerWindow();

		public:

			~SinglePlayerWindow();

			static void Open(void);

			void Move(void) override;
			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};


	class HostGameWindow : public StartWindow, public Singleton<HostGameWindow>
	{
		private:

			EditTextWidget		*gameNameText;
			EditTextWidget		*maxPlayersText;
			CheckWidget			*playHereBox;

			HostGameWindow();

		public:

			~HostGameWindow();

			static void Open(void);

			void Preprocess(void) override;
			void Move(void) override;

			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};


	class JoinGameWindow : public GameWindow, public Completable<JoinGameWindow>, public Singleton<JoinGameWindow>
	{
		private:

			class GameWidget final : public Widget
			{
				public:

					int32				aliveCount;
					NetworkAddress		gameAddress;
					ResourceName		worldName;

					TextWidget			nameText;
					TextWidget			playerText;
					TextWidget			worldText;

					GameWidget(const NetworkAddress& address, const ServerInfoMessage *message);
					~GameWidget();
			};

			PushButtonWidget		*joinButton;
			PushButtonWidget		*cancelButton;

			EditTextWidget			*addressBox;
			ListWidget				*gameList;
			ImageWidget				*previewImage;

			int32					queryTime;

			JoinGameWindow();

		public:

			~JoinGameWindow();

			static void Open(void);

			void Preprocess(void) override;
			void Move(void) override;

			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;

			void ReceiveServerInfo(const NetworkAddress& address, const ServerInfoMessage *message);
	};


	class ConnectWindow : public GameWindow, public Singleton<ConnectWindow>
	{
		private:

			NetworkAddress		connectAddress;

			TextWidget			*messageText;
			ProgressWidget		*progressBar;
			PushButtonWidget	*cancelButton;

			ConnectWindow(const NetworkAddress& address);

		public:

			~ConnectWindow();

			static void Open(const NetworkAddress& address);

			void Preprocess(void) override;
			void Move(void) override;

			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;

			void SetAcceptMessage(const char *text);
			void SetFailMessage(const char *text);
	};


	class CreditsWindow : public GameWindow, public Singleton<CreditsWindow>
	{
		private:

			PushButtonWidget	*okayButton;

			CreditsWindow();

		public:

			~CreditsWindow();

			static void Open(void);

			void Preprocess(void) override;

			bool HandleKeyboardEvent(const KeyboardEventData *eventData) override;
			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};


	class MainWindow : public GameWindow, public Singleton<MainWindow>
	{
		private:

			TextButtonWidget		*newButton;
			TextButtonWidget		*loadButton;
			TextButtonWidget		*saveButton;
			TextButtonWidget		*exitButton;

			TextButtonWidget		*hostButton;
			TextButtonWidget		*joinButton;

			TextButtonWidget		*playerButton;
			TextButtonWidget		*controlsButton;
			TextButtonWidget		*graphicsButton;
			TextButtonWidget		*audioButton;

			TextButtonWidget		*creditsButton;
			TextButtonWidget		*quitButton;

			MainWindow();

			static void SinglePlayerComplete(StartWindow *window, void *cookie);
			static void LoadSavedGameComplete(FilePicker *picker, void *cookie);
			static void SaveCurrentGameComplete(FilePicker *picker, void *cookie);
			static void HostGameComplete(StartWindow *window, void *cookie);
			static void JoinGameComplete(JoinGameWindow *window, void *cookie);
			static void PlayerSettingsComplete(PlayerSettingsWindow *window, void *cookie);
			static void ControlSettingsComplete(ControlSettingsWindow *window, void *cookie);
			static void GraphicsSettingsComplete(GraphicsSettingsWindow *window, void *cookie);
			static void AudioSettingsComplete(AudioSettingsWindow *window, void *cookie);

		public:

			~MainWindow();

			static void Open(void);

			static void ReturnToGame(void);

			void Preprocess(void) override;

			bool HandleKeyboardEvent(const KeyboardEventData *eventData) override;
			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};


	class LoadWindow : public GameWindow, public Completable<LoadWindow>
	{
		private:

			ProgressWidget			*progressWidget;
			TextButtonWidget		*startButton;

			World					*loadWorld;
			volatile WorldResult	loadResult;
			volatile bool			loadComplete;
			bool					warmupInitialized;

			static void LoadTask(void *cookie);

		public:

			LoadWindow(World *world);
			~LoadWindow();

			void Preprocess(void) override;
			void Move(void) override;
	};


	class SummaryWindow : public GameWindow
	{
		private:

			bool				unloadedFlag;
			Sound				*musicSound;
			ResourceName		nextWorldName;

			TextButtonWidget	*continueButton;
			TextWidget			*killsText;
			TextWidget			*secretsText;
			TextWidget			*scoreText;

		public:

			SummaryWindow(const char *nextWorld);
			~SummaryWindow();

			void Preprocess(void) override;
			void Move(void) override;

			bool HandleKeyboardEvent(const KeyboardEventData *eventData) override;
			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};


	class TeleportWindow : public GameWindow, public Singleton<TeleportWindow>
	{
		private:

			PushButtonWidget	*goButton;
			PushButtonWidget	*cancelButton;
			ListWidget			*listWidget;

			TeleportWindow();

		public:

			~TeleportWindow();

			static void Open(void);

			void Preprocess(void) override;

			bool HandleKeyboardEvent(const KeyboardEventData *eventData) override;
			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;

			static void Teleport(const char *teleportName);
	};


	extern ChatWindow *TheChatWindow;
	extern MessageBoard *TheMessageBoard;
	extern UsePopupBoard *TheUsePopupBoard;
	extern NamePopupBoard *TheNamePopupBoard;
	extern SecretPopupBoard *TheSecretPopupBoard;
	extern ActionBoard *TheActionBoard;
	extern DeadBoard *TheDeadBoard;
	extern ScoreBoard *TheScoreBoard;
	extern DisplayBoard *TheDisplayBoard;
	extern SinglePlayerWindow *TheSinglePlayerWindow;
	extern HostGameWindow *TheHostGameWindow;
	extern JoinGameWindow *TheJoinGameWindow;
	extern ConnectWindow *TheConnectWindow;
	extern CreditsWindow *TheCreditsWindow;
	extern MainWindow *TheMainWindow;
	extern TeleportWindow *TheTeleportWindow;
}


#endif

// ZYUQURM
