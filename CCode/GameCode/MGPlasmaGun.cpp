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


#include "MGPlasmaGun.h"
#include "MGFighter.h"
#include "MGGame.h"


using namespace C4;


namespace
{
	Storage<PlasmaGun> plasmaGunStorage;
}


PlasmaGun::PlasmaGun() :
		Weapon(kWeaponPlasmaGun, 0, kModelPlasmaGun, kActionPlasmaGun, 50, 100),
		plasmaControllerRegistration(kControllerPlasma, nullptr),
		plasmaGunModelRegistration(kModelPlasmaGun, TheGame->GetStringTable()->GetString(StringID('MODL', 'WEAP', kModelPlasmaGun)), "plasma/gun/PlasmaGun", kModelPrecache, kControllerCollectable),
		plasmaAmmoModelRegistration(kModelPlasmaAmmo, TheGame->GetStringTable()->GetString(StringID('MODL', 'AMMO', kModelPlasmaAmmo)), "plasma/ammo/Ammo", kModelPrecache, kControllerCollectable),
		plasmaModelRegistration(kModelPlasma, nullptr, "plasma/plasma/Plasma", kModelPrecache | kModelPrivate),
		plasmaGunAction(kWeaponPlasmaGun)
{
	TheInputMgr->AddAction(&plasmaGunAction);

	plasmaScorchTexture = Texture::Get("plasma/gun/Scorch");
}

PlasmaGun::~PlasmaGun()
{
	plasmaScorchTexture->Release();
}

void PlasmaGun::Construct(void)
{
	new(plasmaGunStorage) PlasmaGun;
}

void PlasmaGun::Destruct(void)
{
	plasmaGunStorage->~PlasmaGun();
}

WeaponController *PlasmaGun::NewWeaponController(FighterController *fighter) const
{
	return (new PlasmaGunController(fighter));
}


PlasmaGunController::PlasmaGunController(FighterController *fighter) : WeaponController(kControllerPlasmaGun, fighter)
{
}

PlasmaGunController::~PlasmaGunController()
{
}

WeaponResult PlasmaGunController::UpdateWeapon(const Point3D& position, const Vector3D& direction, const Point3D& center)
{
	if (TheMessageMgr->Server())
	{
		int32 dt = TheTimeMgr->GetDeltaTime();
		int32 time = GetFireDelayTime() - dt;

		const FighterController *fighter = GetFighterController();
		unsigned_int32 flags = fighter->GetFighterFlags();

		if (((flags & kFighterFiring) != 0) && (time <= 0))
		{
			SetFireDelayTime(time + 100);

			GamePlayer *player = fighter->GetFighterPlayer();
			int32 ammo = player->GetWeaponAmmo(kWeaponPlasmaGun, 0);
			if (ammo <= 0)
			{
				return (kWeaponEmpty);
			}

			player->SetWeaponAmmo(kWeaponPlasmaGun, 0, ammo - 1);

			World *world = GetTargetNode()->GetWorld();
			int32 projectileIndex = world->NewControllerIndex();
			TheMessageMgr->SendMessageAll(CreatePlasmaMessage(projectileIndex, fighter->GetControllerIndex(), position, direction * 100.0F));

			ProjectileController *projectileController = static_cast<ProjectileController *>(world->GetController(projectileIndex));
			projectileController->SetFirstPosition(center);
			return (kWeaponFired);
		}
		else
		{
			SetFireDelayTime(time);
		}
	}

	return (kWeaponIdle);
}


PlasmaExplosionParticleSystem::PlasmaExplosionParticleSystem() :
		LineParticleSystem(kParticleSystemPlasmaExplosion, &particlePool, "particle/Spark1"),
		particlePool(kMaxParticleCount, particleArray)
{ 
	SetLengthMultiplier(2.0F);
}
 
PlasmaExplosionParticleSystem::~PlasmaExplosionParticleSystem()
{ 
}

bool PlasmaExplosionParticleSystem::CalculateBoundingSphere(BoundingSphere *sphere) const 
{
	sphere->SetCenter(0.0F, 0.0F, 0.0F); 
	sphere->SetRadius(15.12F); 
	return (true);
}

void PlasmaExplosionParticleSystem::Preprocess(void) 
{
	SetParticleSystemFlags(kParticleSystemSelfDestruct | kParticleSystemPointSprite);
	LineParticleSystem::Preprocess();

	if (!GetFirstParticle())
	{
		Point3D center = GetSuperNode()->GetWorldTransform() * GetNodePosition();
		const ConstVector2D *trig = Math::GetTrigTable();

		for (machine a = 0; a < kMaxParticleCount; a++)
		{
			Particle *particle = particlePool.NewParticle();
			if (!particle)
			{
				break;
			}

			particle->emitTime = 0;
			particle->lifeTime = Math::Random(800) + 200;
			particle->radius = 0.005F + Math::RandomFloat(0.05F);

			float f = Math::RandomFloat(1.0F);
			particle->color.Set(f, 1.0F, f, 1.0F);

			particle->orientation = 0;
			particle->position = center;

			float speed = Math::RandomFloat(0.0125F) + 0.0025F;
			Vector2D csp = trig[Math::Random(128)] * speed;
			const Vector2D& cst = trig[Math::Random(256)];
			particle->velocity.Set(cst.x * csp.y, cst.y * csp.y, csp.x);

			AddParticle(particle);
		}
	}
}

void PlasmaExplosionParticleSystem::AnimateParticles(void)
{
	int32 dt = TheTimeMgr->GetDeltaTime();
	float fdt = TheTimeMgr->GetFloatDeltaTime();

	Particle *particle = GetFirstParticle();
	while (particle)
	{
		Particle *next = particle->nextParticle;

		int32 life = (particle->lifeTime -= dt);
		if (life > 0)
		{
			particle->velocity.z += K::gravity * fdt;
			particle->position += particle->velocity * fdt;

			if (life < 200)
			{
				particle->color.alpha = (float) life * 0.005F;
			}
		}
		else
		{
			FreeParticle(particle);
		}

		particle = next;
	}
}


PlasmaController::PlasmaController() : ProjectileController(kControllerPlasma)
{
}

PlasmaController::PlasmaController(const Vector3D& velocity, GameCharacterController *attacker) : ProjectileController(kControllerPlasma, velocity, attacker)
{
	lifeTime = 6000;

	SetCollisionKind(GetCollisionKind() | kCollisionPlasma);
	SetCollisionExclusionMask(GetCollisionExclusionMask() | kCollisionPlasma);
}

PlasmaController::PlasmaController(const PlasmaController& plasmaController) : ProjectileController(plasmaController)
{
	lifeTime = plasmaController.lifeTime;
}

PlasmaController::~PlasmaController()
{
}

Controller *PlasmaController::Replicate(void) const
{
	return (new PlasmaController(*this));
}

void PlasmaController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	ProjectileController::Pack(data, packFlags);

	data << ChunkHeader('life', 4);
	data << lifeTime;

	data << TerminatorChunk;
}

void PlasmaController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	ProjectileController::Unpack(data, unpackFlags);
	UnpackChunkList<PlasmaController>(data, unpackFlags);
}

bool PlasmaController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'life':

			data >> lifeTime;
			return (true);
	}

	return (false);
}

void PlasmaController::Preprocess(void)
{
	ProjectileController::Preprocess();

	SetRigidBodyFlags(kRigidBodyKeepAwake | kRigidBodyFixedOrientation | kRigidBodyDisabledContact);
	SetCollisionKind(GetCollisionKind() | kCollisionShot);
	SetGravityMultiplier(0.0F);

	Node *node = GetTargetNode()->GetFirstSubnode();
	while (node)
	{
		if (node->GetNodeType() == kNodeLight)
		{
			const GameCharacterController *controller = GetAttackerController();
			if (controller)
			{
				static_cast<Light *>(node)->SetExcludedNode(controller->GetTargetNode());
			}

			break;
		}

		node = node->Next();
	}
}

ControllerMessage *PlasmaController::CreateMessage(ControllerMessageType type) const
{
	switch (type)
	{
		case kPlasmaMessageTeleport:

			return (new PlasmaTeleportMessage(GetControllerIndex()));

		case kPlasmaMessageExplode:

			return (new PlasmaExplodeMessage(GetControllerIndex()));
	}

	return (ProjectileController::CreateMessage(type));
}

void PlasmaController::SendInitialStateMessages(Player *player) const
{
	player->SendMessage(CreatePlasmaMessage(GetControllerIndex(), GetAttackerIndex(), GetTargetNode()->GetWorldPosition(), GetLinearVelocity()));
}

void PlasmaController::Move(void)
{
	if (TheMessageMgr->Server())
	{
		if ((lifeTime -= TheTimeMgr->GetDeltaTime()) <= 0)
		{
			PlasmaController::Destroy(GetTargetNode()->GetWorldPosition(), Normalize(-GetLinearVelocity()));
			return;
		}
	}
}

RigidBodyStatus PlasmaController::HandleNewRigidBodyContact(const RigidBodyContact *contact, RigidBodyController *contactBody)
{
	if (TheMessageMgr->Server())
	{
		Point3D		worldPosition;
		Vector3D	worldNormal;

		contact->GetWorldContactPosition(this, &worldPosition, &worldNormal);

		RigidBodyType type = contactBody->GetRigidBodyType();
		if (type == kRigidBodyCharacter)
		{
			Vector3D force = GetLinearVelocity() * 1.5F;
			GameCharacterController *character = static_cast<GameCharacterController *>(contactBody);
			character->Damage(20 << 16, 0, GetAttackerController(), &worldPosition, &force);
		}
		else if (type == kRigidBodyProjectile)
		{
			static_cast<ProjectileController *>(contactBody)->Destroy(worldPosition, worldNormal);
		}
		else
		{
			const Transform4D& inverseTransform = contactBody->GetTargetNode()->GetInverseWorldTransform();
			Vector3D impulse = inverseTransform * GetLinearVelocity();
			ApplyRigidBodyImpulse(contactBody, impulse * 0.03125F, inverseTransform * worldPosition);

			if (contactBody->GetControllerType() == kControllerExplosive)
			{
				static_cast<ExplosiveController *>(contactBody)->Damage(20 << 16, 0.0F, GetAttackerController());
			}
		}

		PlasmaController::Destroy(worldPosition, -worldNormal);
		return (kRigidBodyDestroyed);
	}

	return (kRigidBodyUnchanged);
}

RigidBodyStatus PlasmaController::HandleNewGeometryContact(const GeometryContact *contact)
{
	if (TheMessageMgr->Server())
	{
		const Transform4D& worldTransform = GetFinalTransform();
		Vector3D worldNormal = -(worldTransform * contact->GetRigidBodyContactNormal());

		const Geometry *geometry = contact->GetContactGeometry();

		const RemotePortal *portal = Game::DetectTeleporter(geometry);
		if (portal)
		{
			// Plasma goes through a teleporting portal.

			Transform4D		portalTransform;

			portal->CalculateRemoteTransform(&portalTransform);

			Point3D position = portalTransform * worldTransform.GetTranslation();
			Vector3D velocity = portalTransform * GetOriginalLinearVelocity();
			Point3D center = worldTransform * contact->GetRigidBodyContactPosition() + worldNormal;

			TheMessageMgr->SendMessageAll(PlasmaTeleportMessage(GetControllerIndex(), position, velocity, center));
			return (kRigidBodyContactsBroken);
		}

		const Point3D& position = contact->GetGeometryContactPosition();
		Game::ProcessGeometryProperties(geometry, position, GetAttackerNode());

		PlasmaController::Destroy(geometry->GetWorldTransform() * position, worldNormal);
		return (kRigidBodyDestroyed);
	}

	return (kRigidBodyUnchanged);
}

void PlasmaController::EnterWorld(World *world, const Point3D& worldPosition)
{
	OmniSource *source = new OmniSource("plasma/gun/Fire", 32.0F);
	source->SetSourcePriority(kSoundPriorityWeapon);
	source->SetNodePosition(worldPosition);
	SetSourceVelocity(source);
	world->AddNewNode(source);
}

void PlasmaController::Destroy(const Point3D& position, const Vector3D& normal)
{
	TheMessageMgr->SendMessageAll(PlasmaExplodeMessage(GetControllerIndex(), position, normal));
}


CreatePlasmaMessage::CreatePlasmaMessage() : CreateModelMessage(kModelMessagePlasma)
{
}

CreatePlasmaMessage::CreatePlasmaMessage(int32 projectileIndex, int32 characterIndex, const Point3D& position, const Vector3D& velocity) : CreateModelMessage(kModelMessagePlasma, projectileIndex, position)
{
	initialVelocity = velocity;
	attackerIndex = characterIndex;
}

CreatePlasmaMessage::~CreatePlasmaMessage()
{
}

void CreatePlasmaMessage::Compress(Compressor& data) const
{
	CreateModelMessage::Compress(data);

	data << initialVelocity;
	data << attackerIndex;
}

bool CreatePlasmaMessage::Decompress(Decompressor& data)
{
	if (CreateModelMessage::Decompress(data))
	{
		data >> initialVelocity;
		data >> attackerIndex;
		return (true);
	}

	return (false);
}

bool CreatePlasmaMessage::HandleMessage(Player *sender) const
{
	GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
	if (world)
	{
		Controller *controller = new PlasmaController(initialVelocity, static_cast<GameCharacterController *>(world->GetController(attackerIndex)));
		InitializeModel(world, Model::Get(kModelPlasma), controller);
	}

	return (true);
}


PlasmaTeleportMessage::PlasmaTeleportMessage(int32 controllerIndex) : ControllerMessage(PlasmaController::kPlasmaMessageTeleport, controllerIndex)
{
}

PlasmaTeleportMessage::PlasmaTeleportMessage(int32 controllerIndex, const Point3D& position, const Vector3D& velocity, const Point3D& center) : ControllerMessage(PlasmaController::kPlasmaMessageTeleport, controllerIndex)
{
	teleportPosition = position;
	teleportVelocity = velocity;
	effectCenter = center;
}

PlasmaTeleportMessage::~PlasmaTeleportMessage()
{
}

void PlasmaTeleportMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << teleportPosition;
	data << teleportVelocity;
	data << effectCenter;
}

bool PlasmaTeleportMessage::Decompress(Decompressor& data)
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

void PlasmaTeleportMessage::HandleControllerMessage(Controller *controller) const
{
	PlasmaController *plasma = static_cast<PlasmaController *>(controller);
	plasma->Teleport(effectCenter, Transform4D(Identity3D, teleportPosition), teleportVelocity);
}


PlasmaExplodeMessage::PlasmaExplodeMessage(int32 controllerIndex) : ControllerMessage(PlasmaController::kPlasmaMessageExplode, controllerIndex)
{
}

PlasmaExplodeMessage::PlasmaExplodeMessage(int32 controllerIndex, const Point3D& position, const Vector3D& normal) : ControllerMessage(PlasmaController::kPlasmaMessageExplode, controllerIndex)
{
	explodePosition = position;
	explodeNormal = normal;
}

PlasmaExplodeMessage::~PlasmaExplodeMessage()
{
}

void PlasmaExplodeMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << explodePosition;
	data << explodeNormal;
}

bool PlasmaExplodeMessage::Decompress(Decompressor& data)
{
	if (ControllerMessage::Decompress(data))
	{
		data >> explodePosition;
		data >> explodeNormal;
		return (true);
	}

	return (false);
}

void PlasmaExplodeMessage::HandleControllerMessage(Controller *controller) const
{
	MarkingData		markingData;

	Node *node = controller->GetTargetNode();
	World *world = node->GetWorld();

	OmniSource *source = new OmniSource("plasma/plasma/Explode", 160.0F);
	source->GetObject()->SetReflectionVolume(0.5F);
	source->SetSourcePriority(kSoundPriorityImpact);
	source->SetNodePosition(explodePosition);
	world->AddNewNode(source);

	PlasmaExplosionParticleSystem *plasmaExplosion = new PlasmaExplosionParticleSystem;
	plasmaExplosion->SetNodePosition(explodePosition);
	world->AddNewNode(plasmaExplosion);

	markingData.center = explodePosition - explodeNormal * static_cast<RigidBodyController *>(controller)->GetBoundingRadius();
	markingData.normal = explodeNormal;
	markingData.tangent = Math::CreatePerpendicular(explodeNormal);
	markingData.tangent.RotateAboutAxis(Math::RandomFloat(K::tau), explodeNormal);
	markingData.radius = 0.75F;
	markingData.textureName = "plasma/gun/Scorch";
	markingData.color.Set(0.875F, 1.0F, 0.875F, 1.0F);
	markingData.lifeTime = 5000;
	MarkingEffect::New(world, &markingData);

	delete node;
}

// ZYUQURM
