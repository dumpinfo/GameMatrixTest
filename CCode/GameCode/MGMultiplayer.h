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


#ifndef MGMultiplayer_h
#define MGMultiplayer_h


#include "MGCharacter.h"
#include "MGProperties.h"


namespace C4
{
	typedef unsigned_int32	ModelMessageType;


	enum
	{
		kGameProtocol	= 0x00000019,
		kGamePort		= 28327
	};


	enum
	{
		kMessageServerInfo = kMessageBaseCount,
		kMessageGameInfo,
		kMessagePlayerStyle,
		kMessageCreateModel,
		kMessageScore,
		kMessageHealth,
		kMessagePrimaryAmmo,
		kMessageSecondaryAmmo,
		kMessageWeapon,
		kMessagePower,
		kMessageTreasure,
		kMessageDeath,
		kMessageRefreshScoreboard,
		kMessageClientStyle,
		kMessageClientOrientation,
		kMessageClientMovementBegin,
		kMessageClientMovementEnd,
		kMessageClientMovementChange,
		kMessageClientFiringPrimaryBegin,
		kMessageClientFiringSecondaryBegin,
		kMessageClientFiringEnd,
		kMessageClientInteractionBegin,
		kMessageClientInteractionEnd,
		kMessageClientSwitchWeapon,
		kMessageClientDeferWeapon,
		kMessageClientSpecialWeapon,
		kMessageClientCycleWeapon,
		kMessageClientSpawn,
		kMessageClientScoreOpen,
		kMessageClientScoreClose,
		kMessageClientChatOpen,
		kMessageClientChatClose,
		kMessageClientVoiceReceiveStart,
		kMessageClientVoiceReceiveStop
	};


	enum
	{
		kModelMessageArrow,
		kModelMessageExplosiveArrow,
		kModelMessageSpike,
		kModelMessageRailSpike,
		kModelMessageGrenade,
		kModelMessageCharge,
		kModelMessageRocket,
		kModelMessagePlasma,
		kModelMessageFireball,
		kModelMessageTarball,
		kModelMessageLavaball,
		kModelMessageVenom,
		kModelMessageBlackCat,
		kModelMessageGusGraves,
		kModelMessageSoldier
	};


	enum
	{
		kPlayerStyleEmissionColor,
		kPlayerStyleArmorColor,
		kPlayerStyleArmorTexture,
		kPlayerStyleHelmetType,
		kPlayerStyleCount
	};


	enum
	{
		kPlayerInactive				= 1 << 0,
		kPlayerReceiveVoiceChat		= 1 << 1,
		kPlayerScoreUpdate			= 1 << 2,
		kPlayerScoreboardOpen		= 1 << 3
	};


	enum
	{
		kMultiplayerDedicated		= 1 << 0 
	};

 
	class GameWorld;
	class FighterController; 


	class ServerInfoMessage : public Message 
	{
		friend class Game; 
 
		private:

			int32						playerCount;
			int32						maxPlayerCount; 
			String<kMaxGameNameLength>	gameName;
			ResourceName				worldName;

			ServerInfoMessage();

		public:

			ServerInfoMessage(int32 numPlayers, int32 maxPlayers, const char *game, const char *world);
			~ServerInfoMessage();

			int32 GetPlayerCount(void) const
			{
				return (playerCount);
			}

			int32 GetMaxPlayerCount(void) const
			{
				return (maxPlayerCount);
			}

			const char *GetGameName(void) const
			{
				return (gameName);
			}

			const char *GetWorldName(void) const
			{
				return (worldName);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;
	};


	class GameInfoMessage : public Message
	{
		friend class Game;

		private:

			unsigned_int32		multiplayerFlags;
			ResourceName		worldName;

			GameInfoMessage();

		public:

			GameInfoMessage(unsigned_int32 flags, const char *world);
			~GameInfoMessage();

			unsigned_int32 GetMultiplayerFlags(void) const
			{
				return (multiplayerFlags);
			}

			const char *GetWorldName(void) const
			{
				return (worldName);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class PlayerStyleMessage : public Message
	{
		friend class Game;

		private:

			int32		playerKey;
			int32		playerStyle[kPlayerStyleCount];

			PlayerStyleMessage();

		public:

			PlayerStyleMessage(int32 player, const int32 *style);
			~PlayerStyleMessage();

			int32 GetPlayerKey(void) const
			{
				return (playerKey);
			}

			const int32 *GetPlayerStyle(void) const
			{
				return (playerStyle);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class CreateModelMessage : public Message
	{
		private:

			ModelMessageType		modelMessageType;

			int32					controllerIndex;
			Point3D					initialPosition;

		protected:

			CreateModelMessage(ModelMessageType type);

			void InitializeModel(GameWorld *world, Model *model, Controller *controller) const;

		public:

			CreateModelMessage(ModelMessageType type, int32 index, const Point3D& position);
			~CreateModelMessage();

			ModelMessageType GetModelMessageType(void) const
			{
				return (modelMessageType);
			}

			int32 GetControllerIndex(void) const
			{
				return (controllerIndex);
			}

			const Point3D& GetInitialPosition(void) const
			{
				return (initialPosition);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			static CreateModelMessage *CreateMessage(ModelMessageType type);
	};


	class ScoreMessage : public Message
	{
		friend class Game;

		private:

			int32		playerScore;

			ScoreMessage();

		public:

			ScoreMessage(int32 score);
			~ScoreMessage();

			int32 GetPlayerScore(void) const
			{
				return (playerScore);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class HealthMessage : public Message
	{
		friend class Game;

		private:

			int32		playerHealth;

			HealthMessage();

		public:

			HealthMessage(int32 health);
			~HealthMessage();

			int32 GetPlayerHealth(void) const
			{
				return (playerHealth);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class WeaponMessage : public Message
	{
		friend class Game;

		private:

			int32		weaponIndex;
			int32		weaponAmmo[2];

			WeaponMessage();

		public:

			WeaponMessage(int32 weapon, int32 ammo1, int32 ammo2);
			~WeaponMessage();

			int32 GetWeaponIndex(void) const
			{
				return (weaponIndex);
			}

			int32 GetWeaponAmmo(int32 index) const
			{
				return (weaponAmmo[index]);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class AmmoMessage : public Message
	{
		friend class Game;

		private:

			int32		weaponIndex;
			int32		weaponAmmo;

			AmmoMessage(MessageType type);

		public:

			AmmoMessage(MessageType type, int32 weapon, int32 ammo);
			~AmmoMessage();

			int32 GetWeaponIndex(void) const
			{
				return (weaponIndex);
			}

			int32 GetWeaponAmmo(void) const
			{
				return (weaponAmmo);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class PowerMessage : public Message
	{
		friend class Game;

		private:

			int32		powerIndex;

			PowerMessage();

		public:

			PowerMessage(int32 power);
			~PowerMessage();

			int32 GetPowerIndex(void) const
			{
				return (powerIndex);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class TreasureMessage : public Message
	{
		friend class Game;

		private:

			int32		treasureIndex;
			int32		treasureCount;

			TreasureMessage();

		public:

			TreasureMessage(int32 treasure, int32 count);
			~TreasureMessage();

			int32 GetTreasureIndex(void) const
			{
				return (treasureIndex);
			}

			int32 GetTreasureCount(void) const
			{
				return (treasureCount);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class DeathMessage : public Message
	{
		friend class Game;

		private:

			PlayerKey		playerKey;
			PlayerKey		attackerKey;

			DeathMessage();

		public:

			DeathMessage(PlayerKey player, PlayerKey shooter);
			~DeathMessage();

			PlayerKey GetPlayerKey(void) const
			{
				return (playerKey);
			}

			PlayerKey GetAttackerKey(void) const
			{
				return (attackerKey);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class RefreshScoreboardMessage : public Message
	{
		friend class Game;

		public:

			enum
			{
				kMaxScoreboardRefreshCount = (kMaxMessageDataSize - 2) / 8
			};

			struct PlayerData
			{
				PlayerKey	playerKey;
				int32		playerScore;
				int32		playerPing;
			};

		private:

			int32			playerCount;
			PlayerData		playerData[kMaxScoreboardRefreshCount];

			RefreshScoreboardMessage();

		public:

			RefreshScoreboardMessage(int32 count);
			~RefreshScoreboardMessage();

			int32 GetPlayerCount(void) const
			{
				return (playerCount);
			}

			PlayerData *GetPlayerData(int32 index)
			{
				return (&playerData[index]);
			}

			const PlayerData *GetPlayerData(int32 index) const
			{
				return (&playerData[index]);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class ClientStyleMessage : public Message
	{
		friend class Game;

		private:

			int32		playerStyle[kPlayerStyleCount];

			ClientStyleMessage();

		public:

			ClientStyleMessage(const int32 *style);
			~ClientStyleMessage();

			const int32 *GetPlayerStyle(void) const
			{
				return (playerStyle);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class ClientOrientationMessage : public Message
	{
		friend class Game;

		private:

			float		orientationAzimuth;
			float		orientationAltitude;

			ClientOrientationMessage();

		public:

			ClientOrientationMessage(float azimuth, float altitude);
			~ClientOrientationMessage();

			float GetOrientationAzimuth(void) const
			{
				return (orientationAzimuth);
			}

			float GetOrientationAltitude(void) const
			{
				return (orientationAltitude);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class ClientMovementMessage : public Message
	{
		friend class Game;

		private:

			unsigned_int32	movementFlag;
			float			movementAzimuth;
			float			movementAltitude;

			ClientMovementMessage(MessageType type);

		public:

			ClientMovementMessage(MessageType type, unsigned_int32 flag, float azimuth, float altitude);
			~ClientMovementMessage();

			int32 GetMovementFlag(void) const
			{
				return (movementFlag);
			}

			float GetMovementAzimuth(void) const
			{
				return (movementAzimuth);
			}

			float GetMovementAltitude(void) const
			{
				return (movementAltitude);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class ClientFiringMessage : public Message
	{
		friend class Game;

		private:

			float		firingAzimuth;
			float		firingAltitude;

			ClientFiringMessage(MessageType type);

		public:

			ClientFiringMessage(MessageType type, float azimuth, float altitude);
			~ClientFiringMessage();

			float GetFiringAzimuth(void) const
			{
				return (firingAzimuth);
			}

			float GetFiringAltitude(void) const
			{
				return (firingAltitude);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class ClientInteractionMessage : public Message
	{
		friend class Game;

		private:

			Point3D		interactionPosition;

			ClientInteractionMessage(MessageType type);

		public:

			ClientInteractionMessage(MessageType type, const Point3D& position);
			~ClientInteractionMessage();

			const Point3D& GetInteractionPosition(void) const
			{
				return (interactionPosition);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class ClientWeaponMessage : public Message
	{
		friend class Game;

		private:

			int32		weaponIndex;

			ClientWeaponMessage(MessageType type);

		public:

			ClientWeaponMessage(MessageType type, int32 weapon);
			~ClientWeaponMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class ClientWeaponCycleMessage : public Message
	{
		friend class Game;

		private:

			int32		cycleDirection;

			ClientWeaponCycleMessage();

		public:

			ClientWeaponCycleMessage(int32 direction);
			~ClientWeaponCycleMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class ClientMiscMessage : public Message
	{
		public:

			ClientMiscMessage(MessageType type);
			~ClientMiscMessage();

			bool HandleMessage(Player *sender) const override;
	};


	class GamePlayer : public Player
	{
		friend class Game;

		private:

			FighterController		*playerController;

			PlayerState				playerState;
			unsigned_int32			playerFlags;
			int32					playerPing;

			int32					playerStyle[kPlayerStyleCount];

			int32					scoreUpdateTime;
			int32					scoreboardTime;

			int32					deathTime;
			int32					shieldTime;

		public:

			GamePlayer(PlayerKey key);
			~GamePlayer();

			GamePlayer *Previous(void) const
			{
				return (static_cast<GamePlayer *>(Player::Previous()));
			}

			GamePlayer *Next(void) const
			{
				return (static_cast<GamePlayer *>(Player::Next()));
			}

			FighterController *GetPlayerController(void) const
			{
				return (playerController);
			}

			PlayerState *GetPlayerState(void)
			{
				return (&playerState);
			}

			const PlayerState *GetPlayerState(void) const
			{
				return (&playerState);
			}

			void RestorePlayerState(const PlayerState *state)
			{
				playerState = *state;
			}

			int32 GetPlayerScore(void) const
			{
				return (playerState.playerScore);
			}

			Fixed GetPlayerHealth(void) const
			{
				return (playerState.playerHealth);
			}

			bool GetWeaponFlag(int32 weaponIndex) const
			{
				return ((playerState.weaponFlags & (1 << weaponIndex)) != 0);
			}

			int32 GetCurrentWeapon(void) const
			{
				return (playerState.currentWeapon);
			}

			void SetCurrentWeapon(int32 weaponIndex)
			{
				playerState.currentWeapon = weaponIndex;
			}

			int32 GetDeferredWeapon(void) const
			{
				return (playerState.deferredWeapon);
			}

			void SetDeferredWeapon(int32 weaponIndex)
			{
				playerState.deferredWeapon = weaponIndex;
			}

			int32 GetDeferredWeaponTime(void) const
			{
				return (playerState.deferredWeaponTime);
			}

			int32 GetDeferredWeaponFrame(void) const
			{
				return (playerState.deferredWeaponFrame);
			}

			void SetDeferredWeaponState(int32 time, int32 frame)
			{
				playerState.deferredWeaponTime = time;
				playerState.deferredWeaponFrame = frame;
			}

			int32 GetWeaponAmmo(int32 weaponIndex, int32 ammoIndex) const
			{
				return (playerState.weaponAmmo[weaponIndex][ammoIndex]);
			}

			int32 GetPlayerKillCount(void) const
			{
				return (playerState.killCount);
			}

			unsigned_int32 GetPlayerFlags(void) const
			{
				return (playerFlags);
			}

			void SetPlayerFlags(unsigned_int32 flags)
			{
				playerFlags = flags;
			}

			int32 GetPlayerPing(void) const
			{
				return (playerPing);
			}

			void SetPlayerPing(int32 ping)
			{
				playerPing = ping;
			}

			const int32 *GetPlayerStyle(void) const
			{
				return (playerStyle);
			}

			int32 GetScoreUpdateTime(void) const
			{
				return (scoreUpdateTime);
			}

			void SetScoreUpdateTime(int32 time)
			{
				scoreUpdateTime = time;
			}

			int32 GetScoreboardTime(void) const
			{
				return (scoreboardTime);
			}

			void SetScoreboardTime(int32 time)
			{
				scoreboardTime = time;
			}

			int32 GetDeathTime(void) const
			{
				return (deathTime);
			}

			void SetDeathTime(int32 time)
			{
				deathTime = time;
			}

			int32 GetShieldTime(void) const
			{
				return (shieldTime);
			}

			void SetShieldTime(int32 time)
			{
				shieldTime = time;
			}

			void SetPlayerController(FighterController *controller, const PlayerState *state = nullptr);
			void SetPlayerStyle(const int32 *style);

			void AddPlayerScore(Fixed delta);
			void SetPlayerHealth(int32 health);

			void AcquireWeapon(int32 weaponIndex);
			void SetWeaponAmmo(int32 weaponIndex, int32 ammoIndex, int32 ammo);

			void AddPlayerKill(void);
			void AddPlayerTreasure(int32 treasureIndex, int32 count);

			CharacterStatus Damage(Fixed damage, unsigned_int32 flags, GameCharacterController *attacker = nullptr);
			void Kill(GameCharacterController *attacker = nullptr);

			static GamePlayer *GetAttackerPlayer(const GameCharacterController *attacker);
			static PlayerKey GetAttackerKey(const GameCharacterController *attacker);
	};
}


#endif

// ZYUQURM
