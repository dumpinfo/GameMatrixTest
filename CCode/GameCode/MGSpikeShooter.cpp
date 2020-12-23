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


#include "MGSpikeShooter.h"
#include "MGFighter.h"
#include "MGGame.h"


using namespace C4;


namespace
{
	Storage<SpikeShooter> spikeShooterStorage;
}


SpikeShooter::SpikeShooter() :
		Weapon(kWeaponSpikeShooter, kWeaponSecondaryAmmo | kWeaponIndependentFire, kModelSpikeShooter, kActionSpikeShooter, 100, 200, 20, 40),
		spikeControllerRegistration(kControllerSpike, nullptr),
		railSpikeControllerRegistration(kControllerRailSpike, nullptr),
		spikeShooterModelRegistration(kModelSpikeShooter, TheGame->GetStringTable()->GetString(StringID('MODL', 'WEAP', kModelSpikeShooter)), "spike/gun/SpikeShooter", kModelPrecache, kControllerCollectable),
		spikeAmmoModelRegistration(kModelSpikeAmmo, TheGame->GetStringTable()->GetString(StringID('MODL', 'AMMO', kModelSpikeAmmo)), "spike/ammo/Ammo", kModelPrecache, kControllerCollectable),
		railSpikeAmmoModelRegistration(kModelRailSpikeAmmo, TheGame->GetStringTable()->GetString(StringID('MODL', 'AMMO', kModelRailSpikeAmmo)), "spike/ammo/Ammo-rail", kModelPrecache, kControllerCollectable),
		comboSpikeAmmoModelRegistration(kModelComboSpikeAmmo, TheGame->GetStringTable()->GetString(StringID('MODL', 'AMMO', kModelComboSpikeAmmo)), "spike/ammo/Ammo-cmbo", kModelPrecache, kControllerCollectable),
		spikeModelRegistration(kModelSpike, nullptr, "spike/spike/Spike", kModelPrecache | kModelPrivate),
		railSpikeModelRegistration(kModelRailSpike, nullptr, "spike/spike/Spike-rail", kModelPrecache | kModelPrivate),
		spikeShooterAction(kWeaponSpikeShooter)
{
	TheInputMgr->AddAction(&spikeShooterAction);
}

SpikeShooter::~SpikeShooter()
{
}

void SpikeShooter::Construct(void)
{
	new(spikeShooterStorage) SpikeShooter;
}

void SpikeShooter::Destruct(void)
{
	spikeShooterStorage->~SpikeShooter();
}

WeaponController *SpikeShooter::NewWeaponController(FighterController *fighter) const
{
	return (new SpikeShooterController(fighter));
}


SpikeShooterController::SpikeShooterController(FighterController *fighter) : WeaponController(kControllerSpikeShooter, fighter)
{
}

SpikeShooterController::~SpikeShooterController()
{
}

void SpikeShooterController::Preprocess(void)
{
	WeaponController::Preprocess();

	fireCount = 0;

	Model *model = GetTargetNode();
	Node *node = model->GetFirstSubnode();
	while (node)
	{
		NodeType type = node->GetNodeType();
		if (type == kNodeSource)
		{
			motorSource = static_cast<OmniSource *>(node);
			motorSource->SetSourcePriority(kSoundPriorityWeapon);
			break;
		}

		node = model->GetNextNode(node);
	}

	frameAnimator.SetTargetModel(model);
	frameAnimator.SetAnimation("spike/gun/Vibrate");

	Interpolator *interpolator = frameAnimator.GetFrameInterpolator();
	interpolator->SetMode(kInterpolatorForward | kInterpolatorLoop);
	interpolator->SetCompletionProc(&FireCallback, this);
	model->SetRootAnimator(&frameAnimator);
}

ControllerMessage *SpikeShooterController::CreateMessage(ControllerMessageType type) const
{
	switch (type)
	{
		case kSpikeShooterMessageRecoil:

			return (new SpikeShooterRecoilMessage(GetControllerIndex()));
	}

	return (WeaponController::CreateMessage(type));
}

void SpikeShooterController::Move(void)
{ 
	if (!motorSource->Playing())
	{
		motorSource->Play(); 
	}
 
	GetTargetNode()->Animate();
}
 
void SpikeShooterController::ShowFireAnimation(void)
{ 
	frameAnimator.SetAnimation("spike/gun/Fire"); 
	frameAnimator.GetFrameInterpolator()->Set(0.0F, 1.0F, kInterpolatorForward);
}

void SpikeShooterController::FireCallback(Interpolator *interpolator, void *cookie) 
{
	SpikeShooterController *controller = static_cast<SpikeShooterController *>(cookie);

	controller->frameAnimator.SetAnimation("spike/gun/Vibrate");
	controller->frameAnimator.GetFrameInterpolator()->Set(0.0F, 1.0F, kInterpolatorForward | kInterpolatorLoop);
}

WeaponResult SpikeShooterController::UpdateWeapon(const Point3D& position, const Vector3D& direction, const Point3D& center)
{
	static const ConstVector3D fireOffset[4] =
	{
		{-0.05F, 0.02F, 0.0805F}, {-0.05F, 0.0F, 0.1005F}, {-0.05F, -0.02F, 0.0805F}, {-0.05F, 0.0F, 0.0605F}
	};

	if (TheMessageMgr->Server())
	{
		int32 dt = TheTimeMgr->GetDeltaTime();
		int32 time = GetFireDelayTime() - dt;

		const FighterController *fighter = GetFighterController();
		unsigned_int32 flags = fighter->GetFighterFlags();

		if (((flags & kFighterFiring) != 0) && (time <= 0))
		{
			GamePlayer *player = fighter->GetFighterPlayer();

			int32 ammo1 = player->GetWeaponAmmo(kWeaponSpikeShooter, 0);
			int32 ammo2 = player->GetWeaponAmmo(kWeaponSpikeShooter, 1);

			if (ammo1 <= 0)
			{
				if (ammo2 <= 0)
				{
					return (kWeaponEmpty);
				}

				flags &= ~kFighterFiringPrimary;
			}
			else if (ammo2 <= 0)
			{
				flags |= kFighterFiringPrimary;
			}

			const Model *model = GetTargetNode();
			World *world = model->GetWorld();

			if (flags & kFighterFiringPrimary)
			{
				SetFireDelayTime(time + 100);
				player->SetWeaponAmmo(kWeaponSpikeShooter, 0, ammo1 - 1);

				int32 projectileIndex = world->NewControllerIndex();
				TheMessageMgr->SendMessageAll(CreateSpikeMessage(projectileIndex, fighter->GetControllerIndex(), position + model->GetWorldTransform() * fireOffset[++fireCount & 3], direction * 60.0F));

				ProjectileController *projectileController = static_cast<ProjectileController *>(world->GetController(projectileIndex));
				projectileController->SetFirstPosition(center);
			}
			else
			{
				SetFireDelayTime(time + 500);
				player->SetWeaponAmmo(kWeaponSpikeShooter, 1, ammo2 - 1);

				int32 projectileIndex = world->NewControllerIndex();
				TheMessageMgr->SendMessageAll(CreateRailSpikeMessage(projectileIndex, fighter->GetControllerIndex(), position, direction * 50.0F));

				ProjectileController *projectileController = static_cast<ProjectileController *>(world->GetController(projectileIndex));
				projectileController->SetFirstPosition(center);

				TheMessageMgr->SendMessageAll(SpikeShooterRecoilMessage(GetControllerIndex()));
			}

			return (kWeaponFired);
		}
		else
		{
			SetFireDelayTime(time);
		}
	}

	return (kWeaponIdle);
}


SpikeShooterRecoilMessage::SpikeShooterRecoilMessage(int32 index) : ControllerMessage(SpikeShooterController::kSpikeShooterMessageRecoil, index)
{
}

SpikeShooterRecoilMessage::~SpikeShooterRecoilMessage()
{
}

void SpikeShooterRecoilMessage::HandleControllerMessage(Controller *controller) const
{
	static_cast<SpikeShooterController *>(controller)->ShowFireAnimation();
}


SpikeController::SpikeController() : ProjectileController(kControllerSpike)
{
	spikeDamage = 7 << 16;
	spikeImpulse = 4.0e-4F;
}

SpikeController::SpikeController(ControllerType type) : ProjectileController(type)
{
}

SpikeController::SpikeController(const Vector3D& velocity, GameCharacterController *attacker) : ProjectileController(kControllerSpike, velocity, attacker)
{
	lifeTime = 6000;

	spikeDamage = 7 << 16;
	spikeImpulse = 4.0e-4F;
}

SpikeController::SpikeController(ControllerType type, const Vector3D& velocity, GameCharacterController *attacker) : ProjectileController(type, velocity, attacker)
{
	lifeTime = 6000;
}

SpikeController::SpikeController(const SpikeController& spikeController) : ProjectileController(spikeController)
{
	lifeTime = spikeController.lifeTime;

	spikeDamage = spikeController.spikeDamage;
	spikeImpulse = spikeController.spikeImpulse;
}

SpikeController::~SpikeController()
{
}

Controller *SpikeController::Replicate(void) const
{
	return (new SpikeController(*this));
}

void SpikeController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	ProjectileController::Pack(data, packFlags);

	data << ChunkHeader('life', 4);
	data << lifeTime;

	data << TerminatorChunk;
}

void SpikeController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	ProjectileController::Unpack(data, unpackFlags);
	UnpackChunkList<SpikeController>(data, unpackFlags);
}

bool SpikeController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'life':

			data >> lifeTime;
			return (true);
	}

	return (false);
}

void SpikeController::Preprocess(void)
{
	ProjectileController::Preprocess();

	SetRigidBodyFlags(kRigidBodyKeepAwake | kRigidBodyFixedOrientation);
	SetCollisionKind(GetCollisionKind() | kCollisionShot);
	SetGravityMultiplier(0.5F);

	Model *model = GetTargetNode();
	Zone *zone = model->GetOwningZone();

	const Vector3D& velocity = GetLinearVelocity();
	SetRigidBodyTransform(CalculateProjectileTransform(model->GetNodePosition(), zone->GetInverseWorldTransform() * velocity));
	model->Invalidate();
}

ControllerMessage *SpikeController::CreateMessage(ControllerMessageType type) const
{
	switch (type)
	{
		case kSpikeMessageTeleport:

			return (new SpikeTeleportMessage(GetControllerIndex()));

		case kSpikeMessageDestroy:

			return (new SpikeDestroyMessage(GetControllerIndex()));
	}

	return (ProjectileController::CreateMessage(type));
}

void SpikeController::SendInitialStateMessages(Player *player) const
{
	player->SendMessage(CreateSpikeMessage(GetControllerIndex(), GetAttackerIndex(), GetTargetNode()->GetWorldPosition(), GetLinearVelocity()));
}

void SpikeController::Move(void)
{
	if (TheMessageMgr->Server())
	{
		if ((lifeTime -= TheTimeMgr->GetDeltaTime()) <= 0)
		{
			SpikeController::Destroy(GetTargetNode()->GetWorldPosition(), Normalize(-GetLinearVelocity()));
		}
	}
}

RigidBodyStatus SpikeController::HandleNewRigidBodyContact(const RigidBodyContact *contact, RigidBodyController *contactBody)
{
	if (TheMessageMgr->Server())
	{
		Point3D		worldPosition;
		Vector3D	worldNormal;

		contact->GetWorldContactPosition(this, &worldPosition, &worldNormal);
		SubstanceType substance = kSubstanceNone;

		RigidBodyType type = contactBody->GetRigidBodyType();
		if (type == kRigidBodyCharacter)
		{
			Vector3D force = GetLinearVelocity() * ((GetControllerType() == kControllerRailSpike) ? 4.0F : 1.5F);
			GameCharacterController *character = static_cast<GameCharacterController *>(contactBody);
			character->Damage(spikeDamage, 0, GetAttackerController(), &worldPosition, &force);
			substance = kSubstanceFlesh;
		}
		else if (type == kRigidBodyProjectile)
		{
			static_cast<ProjectileController *>(contactBody)->Destroy(worldPosition, worldNormal);
		}
		else
		{
			const Shape *shape = (contact->GetStartElement() == this) ? contact->GetFinishShape() : contact->GetStartShape();
			substance = shape->GetObject()->GetShapeSubstance();

			const Transform4D& inverseTransform = contactBody->GetTargetNode()->GetInverseWorldTransform();
			Vector3D impulse = inverseTransform * GetLinearVelocity();
			ApplyRigidBodyImpulse(contactBody, impulse * spikeImpulse, inverseTransform * worldPosition);

			if (contactBody->GetControllerType() == kControllerExplosive)
			{
				static_cast<ExplosiveController *>(contactBody)->Damage(spikeDamage, 0.0F, GetAttackerController());
			}
		}

		TheMessageMgr->SendMessageAll(SpikeDestroyMessage(GetControllerIndex(), worldPosition, -worldNormal, substance));
		return (kRigidBodyDestroyed);
	}

	return (kRigidBodyUnchanged);
}

RigidBodyStatus SpikeController::HandleNewGeometryContact(const GeometryContact *contact)
{
	if (TheMessageMgr->Server())
	{
		const Transform4D& worldTransform = GetFinalTransform();
		Vector3D worldNormal = -(worldTransform * contact->GetRigidBodyContactNormal());

		const Geometry *geometry = contact->GetContactGeometry();

		const RemotePortal *portal = Game::DetectTeleporter(geometry);
		if (portal)
		{
			// Spike goes through a teleporting portal.

			Transform4D		portalTransform;

			portal->CalculateRemoteTransform(&portalTransform);

			Point3D position = portalTransform * worldTransform.GetTranslation();
			Vector3D velocity = portalTransform * GetOriginalLinearVelocity();
			Point3D center = worldTransform * contact->GetRigidBodyContactPosition() + worldNormal;

			TheMessageMgr->SendMessageAll(SpikeTeleportMessage(GetControllerIndex(), position, velocity, center));
			return (kRigidBodyContactsBroken);
		}

		const Point3D& position = contact->GetGeometryContactPosition();
		Game::ProcessGeometryProperties(geometry, position, GetAttackerNode());

		SubstanceType substance = kSubstanceNone;
		const MaterialObject *material = geometry->GetTriangleMaterial(contact->GetSubcontact(0)->triangleIndex);
		if (material)
		{
			substance = material->GetMaterialSubstance();
		}

		TheMessageMgr->SendMessageAll(SpikeDestroyMessage(GetControllerIndex(), geometry->GetWorldTransform() * position, worldNormal, substance));
		return (kRigidBodyDestroyed);
	}

	return (kRigidBodyUnchanged);
}

void SpikeController::EnterWorld(World *world, const Point3D& worldPosition)
{
	OmniSource *source = new OmniSource("spike/gun/Fire", 32.0F);
	source->SetSourcePriority(kSoundPriorityWeapon);
	source->SetNodePosition(worldPosition);
	SetSourceVelocity(source);
	world->AddNewNode(source);
}

void SpikeController::Destroy(const Point3D& position, const Vector3D& normal)
{
	TheMessageMgr->SendMessageAll(SpikeDestroyMessage(GetControllerIndex(), position, normal, kSubstanceNone));
}


RailSpikeController::RailSpikeController() : SpikeController(kControllerRailSpike)
{
	spikeDamage = 40 << 16;
	spikeImpulse = 0.03125F;
}

RailSpikeController::RailSpikeController(const Vector3D& velocity, GameCharacterController *attacker) : SpikeController(kControllerRailSpike, velocity, attacker)
{
	spikeDamage = 40 << 16;
	spikeImpulse = 0.03125F;
}

RailSpikeController::RailSpikeController(const RailSpikeController& railSpikeController) : SpikeController(railSpikeController)
{
}

RailSpikeController::~RailSpikeController()
{
}

Controller *RailSpikeController::Replicate(void) const
{
	return (new RailSpikeController(*this));
}

void RailSpikeController::SendInitialStateMessages(Player *player) const
{
	player->SendMessage(CreateRailSpikeMessage(GetControllerIndex(), GetAttackerIndex(), GetTargetNode()->GetWorldPosition(), GetLinearVelocity()));
}

void RailSpikeController::EnterWorld(World *world, const Point3D& worldPosition)
{
	OmniSource *source = new OmniSource("spike/gun/Fire-rail", 32.0F);
	source->SetSourcePriority(kSoundPriorityWeapon);
	source->SetNodePosition(worldPosition);
	SetSourceVelocity(source);
	world->AddNewNode(source);
}


CreateSpikeMessage::CreateSpikeMessage() : CreateModelMessage(kModelMessageSpike)
{
}

CreateSpikeMessage::CreateSpikeMessage(int32 projectileIndex, int32 characterIndex, const Point3D& position, const Vector3D& velocity) : CreateModelMessage(kModelMessageSpike, projectileIndex, position)
{
	initialVelocity = velocity;
	attackerIndex = characterIndex;
}

CreateSpikeMessage::~CreateSpikeMessage()
{
}

void CreateSpikeMessage::Compress(Compressor& data) const
{
	CreateModelMessage::Compress(data);

	data << initialVelocity;
	data << attackerIndex;
}

bool CreateSpikeMessage::Decompress(Decompressor& data)
{
	if (CreateModelMessage::Decompress(data))
	{
		data >> initialVelocity;
		data >> attackerIndex;
		return (true);
	}

	return (false);
}

bool CreateSpikeMessage::HandleMessage(Player *sender) const
{
	GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
	if (world)
	{
		Controller *controller = new SpikeController(initialVelocity, static_cast<GameCharacterController *>(world->GetController(attackerIndex)));
		InitializeModel(world, Model::Get(kModelSpike), controller);
	}

	return (true);
}


CreateRailSpikeMessage::CreateRailSpikeMessage() : CreateModelMessage(kModelMessageRailSpike)
{
}

CreateRailSpikeMessage::CreateRailSpikeMessage(int32 projectileIndex, int32 characterIndex, const Point3D& position, const Vector3D& velocity) : CreateModelMessage(kModelMessageRailSpike, projectileIndex, position)
{
	initialVelocity = velocity;
	attackerIndex = characterIndex;
}

CreateRailSpikeMessage::~CreateRailSpikeMessage()
{
}

void CreateRailSpikeMessage::Compress(Compressor& data) const
{
	CreateModelMessage::Compress(data);

	data << initialVelocity;
	data << attackerIndex;
}

bool CreateRailSpikeMessage::Decompress(Decompressor& data)
{
	if (CreateModelMessage::Decompress(data))
	{
		data >> initialVelocity;
		data >> attackerIndex;
		return (true);
	}

	return (false);
}

bool CreateRailSpikeMessage::HandleMessage(Player *sender) const
{
	GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
	if (world)
	{
		Controller *controller = new RailSpikeController(initialVelocity, static_cast<GameCharacterController *>(world->GetController(attackerIndex)));
		InitializeModel(world, Model::Get(kModelRailSpike), controller);
	}

	return (true);
}


SpikeTeleportMessage::SpikeTeleportMessage(int32 controllerIndex) : ControllerMessage(SpikeController::kSpikeMessageTeleport, controllerIndex)
{
}

SpikeTeleportMessage::SpikeTeleportMessage(int32 controllerIndex, const Point3D& position, const Vector3D& velocity, const Point3D& center) : ControllerMessage(SpikeController::kSpikeMessageTeleport, controllerIndex)
{
	teleportPosition = position;
	teleportVelocity = velocity;
	effectCenter = center;
}

SpikeTeleportMessage::~SpikeTeleportMessage()
{
}

void SpikeTeleportMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << teleportPosition;
	data << teleportVelocity;
	data << effectCenter;
}

bool SpikeTeleportMessage::Decompress(Decompressor& data)
{
	if (ControllerMessage::Decompress(data))
	{
		data >> teleportPosition;
		data >> teleportVelocity;
		data >> effectCenter;
		return (true);
	}

	return (false);
}

void SpikeTeleportMessage::HandleControllerMessage(Controller *controller) const
{
	SpikeController *spike = static_cast<SpikeController *>(controller);

	Transform4D teleportTransform = ProjectileController::CalculateProjectileTransform(teleportPosition, teleportVelocity);
	spike->Teleport(effectCenter, teleportTransform, teleportVelocity);
}


SpikeDestroyMessage::SpikeDestroyMessage(int32 controllerIndex) : ControllerMessage(SpikeController::kSpikeMessageDestroy, controllerIndex)
{
}

SpikeDestroyMessage::SpikeDestroyMessage(int32 controllerIndex, const Point3D& position, const Vector3D& normal, SubstanceType substance) : ControllerMessage(SpikeController::kSpikeMessageDestroy, controllerIndex)
{
	destroyPosition = position;
	destroyNormal = normal;
	destroySubstance = substance;
}

SpikeDestroyMessage::~SpikeDestroyMessage()
{
}

void SpikeDestroyMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << destroyPosition;
	data << destroyNormal;
	data << destroySubstance;
}

bool SpikeDestroyMessage::Decompress(Decompressor& data)
{
	if (ControllerMessage::Decompress(data))
	{
		data >> destroyPosition;
		data >> destroyNormal;
		data >> destroySubstance;
		return (true);
	}

	return (false);
}

void SpikeDestroyMessage::HandleControllerMessage(Controller *controller) const
{
	Node *node = controller->GetTargetNode();
	World *world = node->GetWorld();

	if (destroySubstance == kSubstanceFlesh)
	{
		if (controller->GetControllerType() == kControllerRailSpike)
		{
			static const char soundName[2][32] =
			{
				"spike/spike/Flesh1", "spike/spike/Flesh2"
			};

			OmniSource *source = new OmniSource(soundName[Math::Random(2)], 64.0F);
			source->SetSourcePriority(kSoundPriorityImpact);
			source->SetNodePosition(destroyPosition);
			world->AddNewNode(source);
			source->SetSourceVolume(2.0F);
		}
	}
	else if (destroySubstance != kSubstanceNone)
	{
		const GameSubstance *substance = static_cast<const GameSubstance *>(MaterialObject::FindRegisteredSubstance(destroySubstance));
		if (substance)
		{
			const SubstanceData *substanceData = substance->GetSubstanceData();
			const char *soundName = "";

			if (controller->GetControllerType() == kControllerRailSpike)
			{
				soundName = substanceData->railImpactSoundName;
			}

			if (soundName[0] == 0)
			{
				soundName = substanceData->spikeImpactSoundName[Math::Random(3)];
			}

			OmniSource *source = new OmniSource(soundName, 64.0F);
			source->SetSourcePriority(kSoundPriorityImpact);
			source->SetNodePosition(destroyPosition);
			world->AddNewNode(source);

			int32 effectType = substanceData->spikeImpactEffectType;
			if (effectType == kSpikeEffectDust)
			{
				DustParticleSystem *dust = new DustParticleSystem(1, &destroyPosition, ColorRGBA(0.5F, 0.25F, 0.125F, 1.0F));
				dust->SetNodePosition(destroyPosition);
				world->AddNewNode(dust);
			}
			else if (effectType == kSpikeEffectSparks)
			{
				SparksParticleSystem *sparks = new SparksParticleSystem(3);
				sparks->SetNodePosition(destroyPosition);
				world->AddNewNode(sparks);
			}
		}
	}

	delete node;
}

// ZYUQURM
