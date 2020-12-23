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


#ifndef MGGame_h
#define MGGame_h


#include "C4Logo.h"
#include "C4World.h"
#include "C4Markings.h"
#include "C4Shakers.h"
#include "MGInput.h"
#include "MGScripts.h"
#include "MGCameras.h"
#include "MGMaterials.h"
#include "MGProperties.h"
#include "MGControllers.h"
#include "MGRigidBody.h"
#include "MGInterface.h"
#include "MGCrosshairs.h"
#include "MGWeather.h"


namespace C4
{
	#if MGCHEATS

		enum
		{
			kGameGodMode		= 1 << 0
		};

	#endif


	class GameWorld : public World
	{
		private:

			int32					spawnLocatorCount;

			List<Marker>			spawnLocatorList;
			List<Marker>			spectatorLocatorList;
			List<Marker>			teleportLocatorList;
			List<Marker>			benchmarkLocatorList;

			SpectatorCamera			spectatorCamera;
			FirstPersonCamera		firstPersonCamera;
			ChaseCamera				chaseCamera;
			ModelCamera				*playerCamera;

			RumbleShaker			rumbleShaker;

			float					bloodIntensity;

			void CollectZoneMarkers(Zone *zone);

			static void ShakeComplete(RumbleShaker *shaker, void *cookie);

			static ProximityResult RecordSplashDamage(Node *node, const Point3D& center, float radius, void *cookie);

		public:

			GameWorld(const char *name);
			~GameWorld();

			int32 GetSpawnLocatorCount(void) const
			{
				return (spawnLocatorCount);
			}

			Marker *GetSpawnLocator(int32 index) const
			{
				return (spawnLocatorList[index]);
			}

			Marker *GetFirstSpectatorLocator(void) const
			{
				return (spectatorLocatorList.First());
			}

			Marker *GetFirstTeleportLocator(void) const
			{
				return (teleportLocatorList.First());
			}

			Marker *GetFirstBenchmarkLocator(void) const
			{
				return (benchmarkLocatorList.First());
			}

			SpectatorCamera *GetSpectatorCamera(void)
			{
				return (&spectatorCamera);
			}

			ModelCamera *GetPlayerCamera(void)
			{
				return (playerCamera);
			}

			bool UsingFirstPersonCamera(void) const
			{
				return (playerCamera == &firstPersonCamera); 
			}

			void SetBloodIntensity(float blood) 
			{
				bloodIntensity = Fmin(blood, 1.0F); 
			}

			WorldResult Preprocess(void) override; 

			RigidBodyStatus HandleNewRigidBodyContact(RigidBodyController *rigidBody, const RigidBodyContact *contact, RigidBodyController *contactBody) override; 
			RigidBodyStatus HandleNewGeometryContact(RigidBodyController *rigidBody, const GeometryContact *contact) override; 

			void HandleWaterSubmergence(RigidBodyController *rigidBody) override;

			void Interact(void) override; 

			void BeginRendering(void) override;
			void EndRendering(void) override;

			void SetCameraTargetModel(Model *model);
			void SetSpectatorCamera(const Point3D& position, float azm, float alt);

			void SetLocalPlayerVisibility(void);
			void ChangePlayerCamera(void);
			void SetFocalLength(float focal);

			void ShakeCamera(float intensity, int32 duration);
			void ShakeCamera(const Point3D& position, float intensity, int32 duration);

			void ProduceSplashDamage(const Point3D& center, float radius, int32 damage, GameCharacterController *attacker, const GameRigidBodyController *excludedBody = nullptr) const;
	};


	class Game : public Application, public Singleton<Game>
	{
		private:

			const StringTable								stringTable;

			Creator<Controller>								controllerCreator;
			Creator<Force>									forceCreator;
			Creator<Effect>									effectCreator;
			Creator<ParticleSystem>							particleSystemCreator;

			DisplayEventHandler								displayEventHandler;

			ControllerReg<GameRigidBodyController>			gameRigidBodyControllerReg;
			ControllerReg<ExplosiveController>				explosiveControllerReg;
			ControllerReg<CrumbleController>				crumbleControllerReg;
			ControllerReg<SecretScriptController>			secretScriptControllerReg;
			ControllerReg<CollectableController>			collectableControllerReg;
			ControllerReg<WindController>					windControllerReg;
			ControllerReg<LightningController>				lightningControllerReg;

			PropertyReg<PlayerStateProperty>				playerStatePropertyReg;
			PropertyReg<CollectableProperty>				collectablePropertyReg;
			PropertyReg<ContainerProperty>					containerPropertyReg;
			PropertyReg<ShootableProperty>					shootablePropertyReg;
			PropertyReg<BreakableProperty>					breakablePropertyReg;
			PropertyReg<LeakingGooProperty>					leakingGooPropertyReg;
			PropertyReg<ImpactSoundProperty>				impactSoundPropertyReg;
			PropertyReg<PaintCanProperty>					paintCanPropertyReg;
			PropertyReg<TeleporterProperty>					teleporterPropertyReg;
			PropertyReg<JumpProperty>						jumpPropertyReg;

			MethodReg<DisplayMessageMethod>					displayMessageReg;
			MethodReg<DisplayActionMethod>					displayActionReg;
			MethodReg<ActivateFlashlightMethod>				activateFlashlightReg;
			MethodReg<DeactivateFlashlightMethod>			deactivateFlashlightReg;
			MethodReg<GetFlashlightStateMethod>				getFlashlightStateReg;
			MethodReg<GiveWeaponMethod>						giveWeaponReg;
			MethodReg<GiveAmmoMethod>						giveAmmoReg;
			MethodReg<GivePowerMethod>						givePowerReg;
			MethodReg<GiveTreasureMethod>					giveTreasureReg;
			MethodReg<GiveHealthMethod>						giveHealthReg;
			MethodReg<GiveScoreMethod>						giveScoreReg;
			MethodReg<DamageCharacterMethod>				damageCharacterReg;
			MethodReg<KillCharacterMethod>					killCharacterReg;
			MethodReg<KillAllMonstersMethod>				killAllMonstersReg;
			MethodReg<TeleportPlayerMethod>					teleportPlayerReg;
			MethodReg<ShakeCameraMethod>					shakeCameraReg;
			MethodReg<EndWorldMethod>						endWorldReg;

			ParticleSystemReg<RainParticleSystem>			rainReg;
			ParticleSystemReg<BurningRainParticleSystem>	burningRainReg;
			ParticleSystemReg<GroundFogParticleSystem>		groundFogReg;
			ParticleSystemReg<TorchSmokeParticleSystem>		torchSmokeReg;
			ParticleSystemReg<ChimneySmokeParticleSystem>	chimneySmokeReg;
			ParticleSystemReg<HeatWavesParticleSystem>		heatWavesReg;
			ParticleSystemReg<BubblingSteamParticleSystem>	bubblingSteamReg;
			ParticleSystemReg<FlowingGooParticleSystem>		flowingGooReg;

			ModelRegistration								appleModelReg;
			ModelRegistration								bananaModelReg;
			ModelRegistration								grapesModelReg;
			ModelRegistration								orangeModelReg;
			ModelRegistration								burgerModelReg;
			ModelRegistration								pizzaModelReg;
			ModelRegistration								subModelReg;
			ModelRegistration								tacoModelReg;
			ModelRegistration								hamModelReg;
			ModelRegistration								roastModelReg;
			ModelRegistration								spaghettiModelReg;
			ModelRegistration								turkeyModelReg;
			ModelRegistration								gobletModelReg;

			ModelRegistration								fireballModelReg;
			ModelRegistration								blackCatModelReg;

			LocatorRegistration								spawnLocatorReg;
			LocatorRegistration								spectatorLocatorReg;
			LocatorRegistration								teleportLocatorReg;
			LocatorRegistration								benchmarkLocatorReg;
			LocatorRegistration								centerLocatorReg;
			LocatorRegistration								fireLocatorReg;

			Variable										*fovVariable;
			Variable										*crossSizeVariable;
			Variable										*effectsVolumeVariable;
			Variable										*musicVolumeVariable;
			Variable										*voiceVolumeVariable;
			Variable										*voiceReceiveVariable;
			Variable										*voiceSendVariable;

			VariableObserver<Game>							looksensObserver;
			VariableObserver<Game>							invertXObserver;
			VariableObserver<Game>							invertYObserver;
			VariableObserver<Game>							fovObserver;
			VariableObserver<Game>							effectsVolumeObserver;
			VariableObserver<Game>							musicVolumeObserver;
			VariableObserver<Game>							voiceVolumeObserver;
			VariableObserver<Game>							voiceReceiveObserver;
			VariableObserver<Game>							voiceSendObserver;

			Observer<Crosshairs, Variable>					crossTypeObserver;
			Observer<Crosshairs, Variable>					crossColorObserver;
			Observer<Crosshairs, Variable>					crossSizeObserver;

			CommandObserver<Game>							hostCommandObserver;
			CommandObserver<Game>							joinCommandObserver;
			CommandObserver<Game>							saveCommandObserver;
			CommandObserver<Game>							restoreCommandObserver;

			Command											hostCommand;
			Command											joinCommand;
			Command											saveCommand;
			Command											restoreCommand;

			#if MGCHEATS

				CommandObserver<Game>						teleportCommandObserver;
				CommandObserver<Game>						benchCommandObserver;
				CommandObserver<Game>						giveCommandObserver;
				CommandObserver<Game>						godCommandObserver;

				Command										teleportCommand;
				Command										benchCommand;
				Command										giveCommand;
				Command										godCommand;

			#endif

			SoundGroup										effectsSoundGroup;
			SoundGroup										musicSoundGroup;
			SoundGroup										voiceSoundGroup;

			MovementAction									forwardAction;
			MovementAction									backwardAction;
			MovementAction									leftAction;
			MovementAction									rightAction;
			MovementAction									upAction;
			MovementAction									downAction;
			MovementAction									movementAction;
			LookAction										horizontalAction;
			LookAction										verticalAction;
			UseAction										useObjectAction;
			FireAction										primaryFireAction;
			FireAction										secondaryFireAction;
			SwitchAction									specialWeaponAction;
			SwitchAction									nextWeaponAction;
			SwitchAction									prevWeaponAction;
			SwitchAction									flashlightAction;
			SwitchAction									cameraViewAction;
			SwitchAction									scoreboardAction;
			SwitchAction									loadAction;
			SwitchAction									saveAction;
			ChatAction										chatAction;

			Crosshairs										crosshairs;

			List<GameBoard>									boardList;
			List<GameWindow>								windowList;

			InputMgr::KeyProc								*prevEscapeProc;
			void											*prevEscapeCookie;

			unsigned_int32									gameFlags;
			unsigned_int32									inputFlags;
			unsigned_int32									multiplayerFlags;

			int32											gameDetailLevel;

			int32											lookSensitivity;
			float											lookMultiplierX;
			float											lookMultiplierY;

			float											lookSpeedX;
			float											lookSpeedY;

			Sound											*titleMusic;

			ResourceName									currentWorldName;
			PlayerState										previousPlayerState;

			static World *CreateWorld(const char *name, void *cookie);
			static Player *CreatePlayer(PlayerKey key, void *cookie);

			static Controller *CreateController(Unpacker& data, unsigned_int32 unpackFlags);
			static Force *CreateForce(Unpacker& data, unsigned_int32 unpackFlags);
			static Effect *CreateEffect(Unpacker& data, unsigned_int32 unpackFlags);
			static ParticleSystem *CreateParticleSystem(Unpacker& data, unsigned_int32 unpackFlags);

			static void HandleDisplayEvent(const DisplayEventData *eventData, void *cookie);
			static void LogoCompleteProc(LogoWindow *window, void *cookie);
			static void EscapeProc(void *cookie);

			void HandleLooksensEvent(Variable *variable);
			void HandleInvertXEvent(Variable *variable);
			void HandleInvertYEvent(Variable *variable);
			void HandleFovEvent(Variable *variable);
			void HandleEffectsVolumeEvent(Variable *variable);
			void HandleMusicVolumeEvent(Variable *variable);
			void HandleVoiceVolumeEvent(Variable *variable);
			void HandleVoiceReceiveEvent(Variable *variable);
			void HandleVoiceSendEvent(Variable *variable);

			static void SinglePlayerWorldLoaded(LoadWindow *window, void *cookie);

		public:

			Game();
			~Game();

			const StringTable *GetStringTable(void) const
			{
				return (&stringTable);
			}

			SoundGroup *GetVoiceSoundGroup(void)
			{
				return (&voiceSoundGroup);
			}

			Crosshairs *GetCrosshairs(void)
			{
				return (&crosshairs);
			}

			void AddBoard(GameBoard *board)
			{
				boardList.Append(board);
				TheInterfaceMgr->AddWidget(board);
			}

			void AddWindow(GameWindow *window)
			{
				windowList.Append(window);
				TheInterfaceMgr->AddWidget(window);
			}

			unsigned_int32 GetGameFlags(void) const
			{
				return (gameFlags);
			}

			unsigned_int32 GetInputFlags(void) const
			{
				return (inputFlags);
			}

			unsigned_int32 GetMultiplayerFlags(void) const
			{
				return (multiplayerFlags);
			}

			int32 GetGameDetailLevel(void) const
			{
				return (gameDetailLevel);
			}

			int32 GetLookSensitivity(void) const
			{
				return (lookSensitivity);
			}

			float GetLookSpeedX(void)
			{
				return (lookSpeedX * lookMultiplierX);
			}

			float GetLookSpeedY(void)
			{
				return (lookSpeedY * lookMultiplierY);
			}

			void UpdateLookSpeedX(float value)
			{
				lookSpeedX = value;
			}

			void UpdateLookSpeedY(float value)
			{
				lookSpeedY = value;
			}

			static void InitPlayerStyle(int32 *style);

			float GetCameraFocalLength(void) const;

			void HandleHostCommand(Command *command, const char *text);
			void HandleJoinCommand(Command *command, const char *text);
			void HandleSaveCommand(Command *command, const char *text);
			void HandleRestoreCommand(Command *command, const char *text);

			#if MGCHEATS

				void HandleTeleportCommand(Command *command, const char *text);
				void HandleBenchCommand(Command *command, const char *text);
				void HandleGiveCommand(Command *command, const char *text);
				void HandleGodCommand(Command *command, const char *text);

			#endif

			void ClearInterface(void);
			void StartSinglePlayerGame(const char *name);
			void SaveSinglePlayerGame(const char *name);
			WorldResult RestoreSinglePlayerGame(const char *name);
			EngineResult HostMultiplayerGame(const char *name, unsigned_int32 flags);
			EngineResult JoinMultiplayerGame(const char *name, unsigned_int32 flags);
			void ExitCurrentGame(void);

			WorldResult RestartWorld(void);

			void StartTitleMusic(void);
			void StopTitleMusic(void);

			void HandleConnectionEvent(ConnectionEvent event, const NetworkAddress& address, const void *param) override;
			void HandlePlayerEvent(PlayerEvent event, Player *player, const void *param) override;
			void HandleGameEvent(GameEvent event, const void *param) override;

			Message *CreateMessage(MessageType type, Decompressor& data) const override;
			void ReceiveMessage(Player *sender, const NetworkAddress& address, const Message *message) override;

			void SpawnPlayer(Player *player);
			void RefreshScoreboard(const RefreshScoreboardMessage *message);

			EngineResult LoadWorld(const char *name) override;
			void UnloadWorld(void) override;

			void ApplicationTask(void) override;

			static const RemotePortal *DetectTeleporter(const Geometry *geometry);
			static void ProcessGeometryProperties(const Geometry *geometry, const Point3D& position, Node *initiator);
	};


	extern Game *TheGame;
}


#endif

// ZYUQURM
