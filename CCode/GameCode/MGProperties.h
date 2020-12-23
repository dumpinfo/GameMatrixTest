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


#ifndef MGProperties_h
#define MGProperties_h


#include "C4Properties.h"
#include "MGWeapons.h"
#include "MGPowers.h"
#include "MGTreasure.h"


namespace C4
{
	enum : PropertyType
	{
		kPropertyPlayerState	= 'play',
		kPropertyCollectable	= 'clct',
		kPropertyContainer		= 'ctnr',
		kPropertyShootable		= 'shot',
		kPropertyBreakable		= 'brak',
		kPropertyLeakingGoo		= 'leak',
		kPropertyImpactSound	= 'isnd',
		kPropertyPaintCan		= 'pant',
		kPropertyTeleporter		= 'tprt',
		kPropertyJump			= 'jump'
	};


	class FighterController;


	struct PlayerState
	{
		int32				playerScore;
		Fixed				playerScoreFraction;

		Fixed				playerHealth;

		unsigned_int32		weaponFlags;
		int32				currentWeapon;
		int32				deferredWeapon;
		int32				deferredWeaponTime;
		int32				deferredWeaponFrame;
		int32				weaponAmmo[kWeaponCount][2];

		int32				powerTime[kPowerCount];

		int32				killCount;
		int32				treasureCount[kTreasureCount];
	};


	class PlayerStateProperty : public Property
	{
		friend class PropertyReg<PlayerStateProperty>;

		private:

			Model				*modelNode;
			PlayerState			playerState;
			bool				flashlightFlag;

			PlayerStateProperty();

			static void ModelLinkProc(Node *node, void *cookie);

		public:

			PlayerStateProperty(Model *model, const PlayerState *state, bool flashlight);
			~PlayerStateProperty();

			Model *GetModelNode(void) const
			{
				return (modelNode);
			}

			const PlayerState *GetPlayerState(void) const
			{
				return (&playerState);
			}

			bool GetFlashlightFlag(void) const
			{
				return (flashlightFlag);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
	};


	class CollectableProperty : public Property
	{
		public:

			typedef Type	CollectableType;

			enum
			{
				kCollectableWeapon		= 'weap',
				kCollectableAmmo		= 'ammo', 
				kCollectableHealth		= 'hlth',
				kCollectablePower		= 'powr',
				kCollectableTreasure	= 'tres', 
				kCollectableTypeCount	= 5
			}; 

			static const CollectableType collectableTypeTable[kCollectableTypeCount];
 
		private:
 
			CollectableType		collectableType; 
			ColorRGBA			respawnColor;

			CollectableProperty(const CollectableProperty& collectableProperty);
 
			Property *Replicate(void) const override;

		public:

			CollectableProperty();
			~CollectableProperty();

			CollectableType GetCollectableType(void) const
			{
				return (collectableType);
			}

			const ColorRGBA& GetRespawnColor(void) const
			{
				return (respawnColor);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;
	};


	class ContainerProperty : public Property
	{
		private:

			Type			treasureType;
			Type			healthType;
			Type			powerType;
			Type			weaponType;
			Type			ammoType;
			Type			monsterType;
			Type			hazardType;

			ContainerProperty(const ContainerProperty& containerProperty);

			Property *Replicate(void) const override;

		public:

			ContainerProperty();
			~ContainerProperty();

			static bool ValidNode(const Node *node);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;
	};


	class ShootableProperty : public Property
	{
		private:

			ShootableProperty(const ShootableProperty& shootableProperty);

			Property *Replicate(void) const override;

		public:

			ShootableProperty();
			~ShootableProperty();

			static bool ValidNode(const Node *node);

			bool SharedProperty(void) const override;
	};


	class BreakableProperty : public Property
	{
		public:

			enum
			{
				kMaxBreakableSoundCount = 3
			};

			enum
			{
				kBreakableKeepAll				= 1 << 0,
				kBreakableForceFieldInhibit		= 1 << 1
			};

		private:

			unsigned_int32		breakableFlags;
			unsigned_int32		collisionExclusionMask;

			ConnectorKey		activationConnectorKey;
			ResourceName		breakableSoundName[kMaxBreakableSoundCount];

			BreakableProperty(const BreakableProperty& breakableProperty);

			Property *Replicate(void) const override;

		public:

			BreakableProperty();
			~BreakableProperty();

			unsigned_int32 GetBreakableFlags(void) const
			{
				return (breakableFlags);
			}

			unsigned_int32 GetCollisionExclusionMask(void) const
			{
				return (collisionExclusionMask);
			}

			const ConnectorKey& GetActivationConnectorKey(void) const
			{
				return (activationConnectorKey);
			}

			static bool ValidNode(const Node *node);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			bool SharedProperty(void) const override;

			const ResourceName& GetBreakableSoundName(void) const;
	};


	class LeakingGooProperty : public Property
	{
		private:

			int32			maxLeakCount;
			MaterialType	materialType;

			LeakingGooProperty(const LeakingGooProperty& leakingGooProperty);

			Property *Replicate(void) const override;

		public:

			LeakingGooProperty();
			~LeakingGooProperty();

			int32 GetMaxLeakCount(void) const
			{
				return (maxLeakCount);
			}

			void SetMaxLeakCount(int32 count)
			{
				maxLeakCount = count;
			}

			MaterialType GetMaterialType(void) const
			{
				return (materialType);
			}

			void SetMaterialType(MaterialType type)
			{
				materialType = type;
			}

			static bool ValidNode(const Node *node);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;
	};


	class ImpactSoundProperty : public Property
	{
		private:

			enum
			{
				kMaxImpactSoundCount	= 2
			};

			ResourceName		hardImpactSoundName[kMaxImpactSoundCount];
			ResourceName		softImpactSoundName[kMaxImpactSoundCount];

			ImpactSoundProperty(const ImpactSoundProperty& impactSoundProperty);

			Property *Replicate(void) const override;

		public:

			ImpactSoundProperty();
			~ImpactSoundProperty();

			static bool ValidNode(const Node *node);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			bool SharedProperty(void) const override;

			const ResourceName& GetHardImpactSoundName(void) const;
			const ResourceName& GetSoftImpactSoundName(void) const;
	};


	class PaintCanProperty : public Property
	{
		private:

			ColorRGBA		paintColor;

			PaintCanProperty(const PaintCanProperty& paintCanProperty);

			Property *Replicate(void) const override;

		public:

			PaintCanProperty();
			~PaintCanProperty();

			const ColorRGBA& GetPaintColor(void) const
			{
				return (paintColor);
			}

			static bool ValidNode(const Node *node);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			bool SharedProperty(void) const override;
	};


	class TeleporterProperty : public Property
	{
		private:

			ConnectorKey	portalConnectorKey;

			TeleporterProperty(const TeleporterProperty& teleporterProperty);

			Property *Replicate(void) const override;

		public:

			TeleporterProperty();
			~TeleporterProperty();

			const ConnectorKey& GetPortalConnectorKey(void) const
			{
				return (portalConnectorKey);
			}

			static bool ValidNode(const Node *node);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;
	};


	class JumpProperty : public Property
	{
		private:

			float			jumpHeight;
			ConnectorKey	destinationConnectorKey;

			JumpProperty(const JumpProperty& jumpProperty);

			Property *Replicate(void) const override;

		public:

			JumpProperty();
			~JumpProperty();

			float GetJumpHeight(void) const
			{
				return (jumpHeight);
			}

			const ConnectorKey& GetDestinationConnectorKey(void) const
			{
				return (destinationConnectorKey);
			}

			static bool ValidNode(const Node *node);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;
	};
}


#endif

// ZYUQURM
