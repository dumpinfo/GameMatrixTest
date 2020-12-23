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


#ifndef MGScripts_h
#define MGScripts_h


#include "C4Scripts.h"
#include "C4Input.h"
#include "C4Models.h"
#include "MGBase.h"


namespace C4
{
	enum : MethodType
	{
		kMethodDisplayMessage			= 'mess',
		kMethodDisplayAction			= 'actn',
		kMethodActivateFlashlight		= 'lit+',
		kMethodDeactivateFlashlight		= 'lit-',
		kMethodGetFlashlightState		= 'glit',
		kMethodGiveWeapon				= 'weap',
		kMethodGiveAmmo					= 'ammo',
		kMethodGivePower				= 'powr',
		kMethodGiveTreasure				= 'tres',
		kMethodGiveHealth				= 'hlth',
		kMethodGiveScore				= 'scor',
		kMethodDamageCharacter			= 'damg',
		kMethodKillCharacter			= 'kill',
		kMethodKillAllMonsters			= 'kmon',
		kMethodTeleportPlayer			= 'tlpt',
		kMethodShakeCamera				= 'shak',
		kMethodEndWorld					= 'endw'
	};


	class DisplayMessageMethod final : public Method
	{
		friend class MethodReg<DisplayMessageMethod>;

		private:

			enum
			{
				kMaxMessageLength	= 127
			};

			String<kMaxMessageLength>	messageText;

			DisplayMessageMethod();
			DisplayMessageMethod(const DisplayMessageMethod& displayMessageMethod);

			Method *Replicate(void) const override;

		public:

			DisplayMessageMethod(const char *message);
			~DisplayMessageMethod();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(const ScriptState *state) override;
	};


	class DisplayActionMethod final : public Method
	{
		friend class MethodReg<DisplayActionMethod>;

		private:

			ActionType		actionType;
			int32			displayTime;

			DisplayActionMethod();
			DisplayActionMethod(const DisplayActionMethod& displayActionMethod);

			Method *Replicate(void) const override;

		public:

			DisplayActionMethod(ActionType type, int32 time);
			~DisplayActionMethod();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(const ScriptState *state) override;
	};


	class ActivateFlashlightMethod final : public Method
	{
		private: 

			ActivateFlashlightMethod(const ActivateFlashlightMethod& activateFlashlightMethod);
 
			Method *Replicate(void) const override;
 
		public:

			ActivateFlashlightMethod(); 
			~ActivateFlashlightMethod();
 
			void Execute(const ScriptState *state) override; 
	};


	class DeactivateFlashlightMethod final : public Method 
	{
		private:

			DeactivateFlashlightMethod(const DeactivateFlashlightMethod& deactivateFlashlightMethod);

			Method *Replicate(void) const override;

		public:

			DeactivateFlashlightMethod();
			~DeactivateFlashlightMethod();

			void Execute(const ScriptState *state) override;
	};


	class GetFlashlightStateMethod final : public Method
	{
		private:

			GetFlashlightStateMethod(const GetFlashlightStateMethod& getFlashlightStateMethod);

			Method *Replicate(void) const override;

		public:

			GetFlashlightStateMethod();
			~GetFlashlightStateMethod();

			void Execute(const ScriptState *state) override;
	};


	class GiveWeaponMethod final : public Method
	{
		private:

			ModelType		weaponType;

			GiveWeaponMethod(const GiveWeaponMethod& giveWeaponMethod);

			Method *Replicate(void) const override;

		public:

			GiveWeaponMethod();
			~GiveWeaponMethod();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(const ScriptState *state) override;
	};


	class GiveAmmoMethod final : public Method
	{
		private:

			ModelType		weaponType;
			int32			ammoCount;
			int32			ammoIndex;

			GiveAmmoMethod(const GiveAmmoMethod& giveAmmoMethod);

			Method *Replicate(void) const override;

		public:

			GiveAmmoMethod();
			~GiveAmmoMethod();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(const ScriptState *state) override;
	};


	class GivePowerMethod final : public Method
	{
		private:

			ModelType		powerType;

			GivePowerMethod(const GivePowerMethod& givePowerMethod);

			Method *Replicate(void) const override;

		public:

			GivePowerMethod();
			~GivePowerMethod();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(const ScriptState *state) override;
	};


	class GiveTreasureMethod final : public Method
	{
		private:

			ModelType		treasureType;
			int32			treasureCount;

			GiveTreasureMethod(const GiveTreasureMethod& giveTreasureMethod);

			Method *Replicate(void) const override;

		public:

			GiveTreasureMethod();
			~GiveTreasureMethod();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(const ScriptState *state) override;
	};


	class GiveHealthMethod final : public Method
	{
		private:

			Fixed		healthCount;
			Fixed		maxHealth;

			GiveHealthMethod(const GiveHealthMethod& giveHealthMethod);

			Method *Replicate(void) const override;

		public:

			GiveHealthMethod();
			~GiveHealthMethod();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(const ScriptState *state) override;
	};


	class GiveScoreMethod final : public Method
	{
		private:

			Fixed		scoreCount;

			GiveScoreMethod(const GiveScoreMethod& giveScoreMethod);

			Method *Replicate(void) const override;

		public:

			GiveScoreMethod();
			~GiveScoreMethod();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(const ScriptState *state) override;
	};


	class DamageCharacterMethod final : public Method
	{
		private:

			Fixed				damageCount;
			unsigned_int32		damageFlags;

			DamageCharacterMethod(const DamageCharacterMethod& damageCharacterMethod);

			Method *Replicate(void) const override;

		public:

			DamageCharacterMethod();
			~DamageCharacterMethod();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(const ScriptState *state) override;
	};


	class KillCharacterMethod final : public Method
	{
		private:

			KillCharacterMethod(const KillCharacterMethod& killCharacterMethod);

			Method *Replicate(void) const override;

		public:

			KillCharacterMethod();
			~KillCharacterMethod();

			void Execute(const ScriptState *state) override;
	};


	class KillAllMonstersMethod final : public Method
	{
		private:

			KillAllMonstersMethod(const KillAllMonstersMethod& killAllMonstersMethod);

			Method *Replicate(void) const override;

		public:

			KillAllMonstersMethod();
			~KillAllMonstersMethod();

			void Execute(const ScriptState *state) override;
	};


	class TeleportPlayerMethod final : public Method
	{
		private:

			ConnectorKey		destinationConnectorKey;

			TeleportPlayerMethod(const TeleportPlayerMethod& teleportPlayerMethod);

			Method *Replicate(void) const override;

		public:

			TeleportPlayerMethod();
			~TeleportPlayerMethod();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(const ScriptState *state) override;
	};


	class ShakeCameraMethod final : public Method
	{
		private:

			float		shakeIntensity;
			int32		shakeDuration;

			ShakeCameraMethod(const ShakeCameraMethod& shakeCameraMethod);

			Method *Replicate(void) const override;

		public:

			ShakeCameraMethod();
			~ShakeCameraMethod();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(const ScriptState *state) override;
	};


	class EndWorldMethod final : public Method
	{
		private:

			ResourceName		nextWorldName;

			EndWorldMethod(const EndWorldMethod& endWorldMethod);

			Method *Replicate(void) const override;

		public:

			EndWorldMethod();
			~EndWorldMethod();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(const ScriptState *state) override;
	};
}


#endif

// ZYUQURM
