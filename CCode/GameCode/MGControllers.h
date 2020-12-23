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


#ifndef MGControllers_h
#define MGControllers_h


#include "C4Scripts.h"
#include "C4Sources.h"
#include "C4Portals.h"
#include "C4Triggers.h"
#include "C4Effects.h"
#include "C4Fields.h"
#include "C4Forces.h"
#include "MGProperties.h"


namespace C4
{
	enum : ControllerType
	{
		kControllerSecretScript		= 'scrt',
		kControllerCollectable		= 'clct',
		kControllerWind				= 'wndf',
		kControllerLightning		= 'ltng'
	};


	enum : LocatorType
	{
		kLocatorCenter				= 'cent',
		kLocatorFire				= 'fire'
	};


	class SpiralHelixParticleSystem;


	class SecretScriptController final : public ScriptController
	{
		private:

			enum
			{
				kSecretFound			= 1 << 0,
				kSecretCounted			= 1 << 1
			};

			enum
			{
				kSecretMessageCount		= 7
			};

			unsigned_int32		secretState;
			Type				messageType;

			static int32		totalSecretCount;
			static int32		foundSecretCount;

			static const Type	secretMessageType[kSecretMessageCount];

			SecretScriptController(const SecretScriptController& secretScriptController);

			Controller *Replicate(void) const override;

		public:

			SecretScriptController();
			~SecretScriptController();

			static int32 GetTotalSecretCount(void)
			{
				return (totalSecretCount);
			}

			static int32 GetFoundSecretCount(void)
			{
				return (foundSecretCount);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Preprocess(void) override;
			void Activate(Node *initiator, Node *trigger) override;
	};


	// This controller makes a model such as a gun or power-up
	// rotate and float up and down.

	class CollectableController final : public Controller
	{
		private:

			typedef CollectableProperty::CollectableType CollectableType;

			enum
			{ 
				kCollectableInitialized		= 1 << 0,
				kCollectableDelete			= 1 << 1
			}; 

			unsigned_int32				collectableFlags; 

			ScriptController			*scriptController;
			CollectableType				collectableType; 
			ColorRGBA					respawnColor;
 
			Trigger						*triggerNode; 
			QuadEffect					*quadEffectNode;
			SpiralHelixParticleSystem	*spiralHelixNode;

			Interpolator				effectInterpolator; 
			int32						respawnInterval;
			int32						respawnTime;
			float						phaseAngle;

			Point3D						centerPosition;

			CollectableController(const CollectableController& collectableController);

			Controller *Replicate(void) const override;

		public:

			enum
			{
				kCollectableMessageCollect,
				kCollectableMessageRespawn,
				kCollectableMessageState
			};

			CollectableController();
			~CollectableController();

			Model *GetTargetNode(void) const
			{
				return (static_cast<Model *>(Controller::GetTargetNode()));
			}

			static bool ValidNode(const Node *node);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Preprocess(void) override;

			ControllerMessage *CreateMessage(ControllerMessageType type) const override;
			void ReceiveMessage(const ControllerMessage *message) override;
			void SendInitialStateMessages(Player *player) const override;

			void Move(void) override;
			void Activate(Node *initiator, Node *trigger) override;
	};


	class CollectableStateMessage : public ControllerMessage
	{
		friend class CollectableController;

		private:

			bool	activeFlag;

			CollectableStateMessage(int32 controllerIndex);

		public:

			CollectableStateMessage(int32 controllerIndex, bool active);
			~CollectableStateMessage();

			bool GetActiveFlag(void) const
			{
				return (activeFlag);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;
	};


	class WindController final : public Controller
	{
		private:

			WindForce			*windForce;

			Range<float>		windRange[3];
			Range<int32>		blowTime;
			Range<int32>		restTime;

			bool				windFlag;
			int32				windTime;
			float				windParam;
			Vector3D			windVelocity[2];

			WindController(const WindController& windController);

			Controller *Replicate(void) const override;

		public:

			enum
			{
				kWindMessageState
			};

			WindController();
			~WindController();

			Field *GetTargetNode(void) const
			{
				return (static_cast<Field *>(Controller::GetTargetNode()));
			}

			static bool ValidNode(const Node *node);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Preprocess(void) override;

			ControllerMessage *CreateMessage(ControllerMessageType type) const override;
			void ReceiveMessage(const ControllerMessage *message) override;
			void SendInitialStateMessages(Player *player) const override;

			void Move(void) override;
	};


	class WindStateMessage : public ControllerMessage
	{
		friend class WindController;

		private:

			Vector3D	windVelocity;

			WindStateMessage(int32 controllerIndex);

		public:

			WindStateMessage(int32 controllerIndex, const Vector3D& velocity);
			~WindStateMessage();

			const Vector3D& GetWindVelocity(void) const
			{
				return (windVelocity);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;
	};


	// This modifies the ambient light in a zone to give the appearance
	// of occasional lightning effects.

	class LightningController final : public Controller
	{
		private:

			ColorRGB			lightColor;

			int32				lightningCount;
			int32				lightningTime;
			float				lightningIntensity;
			Interpolator		lightningInterpolator;

			const Marker		*boltMarker[2];
			BoltEffect			*boltEffect;

			LightningController(const LightningController& lightningController);

			Controller *Replicate(void) const override;

			static void BoltLinkProc(Node *node, void *cookie);

		public:

			enum
			{
				kLightningMessageActivate
			};

			LightningController();
			~LightningController();

			static bool ValidNode(const Node *node);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Preprocess(void) override;

			ControllerMessage *CreateMessage(ControllerMessageType type) const override;
			void ReceiveMessage(const ControllerMessage *message) override;

			void Wake(void) override;
			void Move(void) override;
	};
}


#endif

// ZYUQURM
