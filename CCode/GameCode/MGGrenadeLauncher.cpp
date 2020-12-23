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


#include "MGGrenadeLauncher.h"
#include "MGFighter.h"
#include "MGGame.h"


using namespace C4;


namespace
{
	const float kGrenadeSplashDamageRadius = 5.0F;

	Storage<GrenadeLauncher> grenadeLauncherStorage;
}


GrenadeLauncher::GrenadeLauncher() :
		Weapon(kWeaponGrenadeLauncher, 0, kModelGrenadeLauncher, kActionGrenadeLauncher, 20, 40),
		grenadeControllerRegistration(kControllerGrenade, nullptr),
		grenadeLauncherModelRegistration(kModelGrenadeLauncher, TheGame->GetStringTable()->GetString(StringID('MODL', 'WEAP', kModelGrenadeLauncher)), "grenade/gun/GrenadeLauncher", kModelPrecache, kControllerCollectable),
		grenadeAmmoModelRegistration(kModelGrenadeAmmo, TheGame->GetStringTable()->GetString(StringID('MODL', 'AMMO', kModelGrenadeAmmo)), "grenade/ammo/Ammo", kModelPrecache, kControllerCollectable),
		grenadeModelRegistration(kModelGrenade, nullptr, "grenade/grenade/Grenade", kModelPrecache | kModelPrivate),
		grenadeLauncherAction(kWeaponGrenadeLauncher)
{
	TheInputMgr->AddAction(&grenadeLauncherAction);
}

GrenadeLauncher::~GrenadeLauncher()
{
}

void GrenadeLauncher::Construct(void)
{
	new(grenadeLauncherStorage) GrenadeLauncher;
}

void GrenadeLauncher::Destruct(void)
{
	grenadeLauncherStorage->~GrenadeLauncher();
}

WeaponController *GrenadeLauncher::NewWeaponController(FighterController *fighter) const
{
	return (new GrenadeLauncherController(fighter));
}


GrenadeLauncherController::GrenadeLauncherController(FighterController *fighter) : WeaponController(kControllerGrenadeLauncher, fighter)
{
}

GrenadeLauncherController::~GrenadeLauncherController()
{
}

void GrenadeLauncherController::Preprocess(void)
{
	WeaponController::Preprocess();

	currentDrumAngle = 0.0F;
	targetDrumAngle = 0.0F;

	Model *model = GetTargetNode();
	drumNode = model->FindNode(Text::StaticHash<'D', 'r', 'u', 'm'>::value);

	frameAnimator.SetTargetModel(model);
	frameAnimator.SetAnimation("grenade/gun/Fire");
	model->SetRootAnimator(&frameAnimator);
}

ControllerMessage *GrenadeLauncherController::CreateMessage(ControllerMessageType type) const
{
	switch (type)
	{
		case kGrenadeLauncherMessageAdvance:

			return (new GrenadeLauncherAdvanceMessage(GetControllerIndex()));
	}

	return (WeaponController::CreateMessage(type));
}

void GrenadeLauncherController::Move(void)
{
	float angle = currentDrumAngle;
	float target = targetDrumAngle;
	if (angle < target)
	{
		angle += TheTimeMgr->GetFloatDeltaTime() * 0.005F;
		if (!(angle < target))
		{
			if (target >= 8.0F)
			{
				target = 0.0F;
			}

			targetDrumAngle = target;
			angle = target;
		}

		currentDrumAngle = angle; 

		drumNode->SetNodeMatrix3D(Matrix3D().SetRotationAboutX(angle * K::tau_over_8));
		drumNode->Invalidate(); 
	}
 
	GetTargetNode()->Animate();
}
 
void GrenadeLauncherController::ShowFireAnimation(void)
{ 
	targetDrumAngle += 1.0F; 

	frameAnimator.GetFrameInterpolator()->Set(0.0F, 1.0F, kInterpolatorForward);
}
 
WeaponResult GrenadeLauncherController::UpdateWeapon(const Point3D& position, const Vector3D& direction, const Point3D& center)
{
	if (TheMessageMgr->Server())
	{
		int32 dt = TheTimeMgr->GetDeltaTime();
		int32 time = GetFireDelayTime() - dt;

		const FighterController *fighter = GetFighterController();
		unsigned_int32 flags = fighter->GetFighterFlags();

		if (((flags & kFighterFiring) != 0) && (time <= 0))
		{
			SetFireDelayTime(time + 500);

			GamePlayer *player = fighter->GetFighterPlayer();
			int32 ammo = player->GetWeaponAmmo(kWeaponGrenadeLauncher, 0);
			if (ammo <= 0)
			{
				return (kWeaponEmpty);
			}

			player->SetWeaponAmmo(kWeaponGrenadeLauncher, 0, ammo - 1);

			World *world = GetTargetNode()->GetWorld();
			int32 projectileIndex = world->NewControllerIndex();
			TheMessageMgr->SendMessageAll(CreateGrenadeMessage(projectileIndex, fighter->GetControllerIndex(), position, direction * 20.0F));

			ProjectileController *projectileController = static_cast<ProjectileController *>(world->GetController(projectileIndex));
			projectileController->SetFirstPosition(center);

			TheMessageMgr->SendMessageAll(GrenadeLauncherAdvanceMessage(GetControllerIndex()));
			return (kWeaponFired);
		}
		else
		{
			SetFireDelayTime(time);
		}
	}

	return (kWeaponIdle);
}


GrenadeLauncherAdvanceMessage::GrenadeLauncherAdvanceMessage(int32 index) : ControllerMessage(GrenadeLauncherController::kGrenadeLauncherMessageAdvance, index)
{
}

GrenadeLauncherAdvanceMessage::~GrenadeLauncherAdvanceMessage()
{
}

void GrenadeLauncherAdvanceMessage::HandleControllerMessage(Controller *controller) const
{
	static_cast<GrenadeLauncherController *>(controller)->ShowFireAnimation();
}


GrenadeExplosionParticleSystem::GrenadeExplosionParticleSystem(const Vector3D& velocity) :
		TexcoordPointParticleSystem(kParticleSystemGrenadeExplosion, &particlePool, "particle/Puff1"),
		particlePool(kMaxParticleCount, particleArray)
{
	explosionVelocity = velocity * 0.001F;
}

GrenadeExplosionParticleSystem::~GrenadeExplosionParticleSystem()
{
}

bool GrenadeExplosionParticleSystem::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	sphere->SetCenter(0.0F, 0.0F, 0.0F);
	sphere->SetRadius(16.0F);
	return (true);
}

void GrenadeExplosionParticleSystem::Preprocess(void)
{
	SetParticleSystemFlags(kParticleSystemSelfDestruct | kParticleSystemSoftDepth);
	TexcoordPointParticleSystem::Preprocess();

	SetAmbientBlendState(kBlendInterpolate | kBlendAlphaPreserve);
	SetTransparentPosition(&GetWorldPosition());
	SetSoftDepthScale(2.0F);

	if (!GetFirstParticle())
	{
		Point3D center = GetSuperNode()->GetWorldTransform() * GetNodePosition();
		const ConstVector2D *trig = Math::GetTrigTable();

		for (machine a = 0; a < 20; a++)
		{
			SmokeParticle *particle = particlePool.NewParticle();

			particle->emitTime = 0;
			particle->lifeTime = Math::Random(800) + 200;
			particle->radius = 0.1F + Math::RandomFloat(0.5F);
			particle->color.Set(1.0F, Math::RandomFloat(0.5F) + 0.25F, 0.0F, 1.0F);
			particle->orientation = Math::Random(256) << 24;
			particle->position = center;

			float speed = Math::RandomFloat(0.01F) + 0.005F;
			Vector2D csp = trig[Math::Random(128)] * speed;
			const Vector2D& cst = trig[Math::Random(256)];
			particle->velocity.Set(cst.x * csp.y, cst.y * csp.y, csp.x);
			particle->velocity += explosionVelocity;

			int32 i = Math::Random(4);
			particle->texcoordScale.Set(0.5F, 0.5F);
			particle->texcoordOffset.Set((i & 1) ? 0.5F : 0.0F, (i & 2) ? 0.5F : 0.0F);

			particle->smokeCount = 0;
			AddParticle(particle);
		}
	}
}

void GrenadeExplosionParticleSystem::AnimateParticles(void)
{
	int32 dt = TheTimeMgr->GetDeltaTime();
	float fdt = TheTimeMgr->GetFloatDeltaTime();

	SmokeParticle *particle = static_cast<SmokeParticle *>(GetFirstParticle());
	while (particle)
	{
		SmokeParticle *next = particle->GetNextParticle();

		int32 life = (particle->lifeTime -= dt);
		if (life > 0)
		{
			if (life < 500)
			{
				particle->color.alpha = Fmin((float) life * 0.002F, particle->color.alpha);
			}

			Fixed count = particle->smokeCount;
			if (count < 0)
			{
				particle->radius += fdt * 0.001F;

				float f = Exp(fdt * -0.003F);
				particle->color.red *= f;
				particle->color.green *= f;
			}
			else
			{
				particle->velocity.z += K::gravity * 0.5F * fdt;
				Vector3D dp = particle->velocity * fdt;
				particle->position += dp;

				int32 k = count + dt;
				count = k >> 5;
				particle->smokeCount = k - (count << 5);

				for (machine a = 0; a < count; a++)
				{
					SmokeParticle *p = particlePool.NewParticle();
					if (!p)
					{
						break;
					}

					p->emitTime = 0;
					p->lifeTime = Math::Random(1000) + 500;
					p->radius = 0.2F;
					p->color = particle->color;
					p->orientation = Math::Random(256) << 24;
					p->position = particle->position - dp * Math::RandomFloat(1.0F);
					p->velocity.Set(0.0F, 0.0F, 0.0F);

					int32 i = Math::Random(4);
					p->texcoordScale.Set(0.5F, 0.5F);
					p->texcoordOffset.Set((i & 1) ? 0.5F : 0.0F, (i & 2) ? 0.5F : 0.0F);

					p->smokeCount = -1;
					AddParticle(p);
				}
			}
		}
		else
		{
			FreeParticle(particle);
		}

		particle = next;
	}
}


GrenadeSparksParticleSystem::GrenadeSparksParticleSystem(const Vector3D& velocity) :
		LineParticleSystem(kParticleSystemGrenadeSparks, &particlePool, "particle/Spark1"),
		particlePool(kMaxParticleCount, particleArray)
{
	sparksVelocity = velocity * 0.001F;
	SetLengthMultiplier(8.0F);
}

GrenadeSparksParticleSystem::~GrenadeSparksParticleSystem()
{
}

bool GrenadeSparksParticleSystem::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	sphere->SetCenter(0.0F, 0.0F, 0.0F);
	sphere->SetRadius(10.0F);
	return (true);
}

void GrenadeSparksParticleSystem::Preprocess(void)
{
	LineParticleSystem::Preprocess();
	SetParticleSystemFlags(kParticleSystemSelfDestruct);

	if (!GetFirstParticle())
	{
		Point3D center = GetSuperNode()->GetWorldTransform() * GetNodePosition();
		const ConstVector2D *trig = Math::GetTrigTable();

		for (machine a = 0; a < kMaxParticleCount; a++)
		{
			Particle *particle = particlePool.NewParticle();

			particle->emitTime = 0;
			particle->lifeTime = 1000 + Math::Random(500);
			particle->radius = Math::RandomFloat(0.025F) + 0.05F;
			float blue = Math::RandomFloat(1.0F);
			particle->color.Set(1.0F, blue + Math::RandomFloat(1.0F - blue), blue, 1.0F);
			particle->orientation = 0;
			particle->position = center;

			int32 phi = Math::Random(128);
			int32 theta = Math::Random(256);
			float speed = Math::RandomFloat(0.0125F) + 0.0025F;
			Vector2D csp = trig[phi] * speed;
			const Vector2D& cst = trig[theta];
			particle->velocity.Set(cst.x * csp.y, cst.y * csp.y, csp.x);
			particle->velocity += sparksVelocity;

			AddParticle(particle);
		}
	}
}

void GrenadeSparksParticleSystem::AnimateParticles(void)
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
			if (life < 400)
			{
				particle->color.alpha = (float) life * 0.0025F;
			}

			particle->velocity.z += K::gravity * fdt;
			particle->position += particle->velocity * fdt;
		}
		else
		{
			FreeParticle(particle);
		}

		particle = next;
	}
}


GrenadeController::GrenadeController() : ProjectileController(kControllerGrenade)
{
	smokeTrail = nullptr;
}

GrenadeController::GrenadeController(const Vector3D& velocity, GameCharacterController *attacker) : ProjectileController(kControllerGrenade, velocity, attacker)
{
	lifeTime = 10000;
	collisionCount = 0;

	smokeTime = 250;
	smokeTrail = nullptr;
}

GrenadeController::GrenadeController(const GrenadeController& grenadeController) : ProjectileController(grenadeController)
{
	lifeTime = grenadeController.lifeTime;
	collisionCount = 0;

	smokeTime = 250;
	smokeTrail = nullptr;
}

GrenadeController::~GrenadeController()
{
	if (smokeTrail)
	{
		smokeTrail->Finalize();
	}
}

Controller *GrenadeController::Replicate(void) const
{
	return (new GrenadeController(*this));
}

void GrenadeController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	ProjectileController::Pack(data, packFlags);

	data << ChunkHeader('LIFE', 4);
	data << lifeTime;

	data << ChunkHeader('STIM', 4);
	data << smokeTime;

	data << ChunkHeader('COLL', 4);
	data << collisionCount;

	if ((smokeTrail) && (smokeTrail->LinkedNodePackable(packFlags)))
	{
		data << ChunkHeader('TRAL', 4);
		data << smokeTrail->GetNodeIndex();
	}

	data << TerminatorChunk;
}

void GrenadeController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	ProjectileController::Unpack(data, unpackFlags);
	UnpackChunkList<GrenadeController>(data, unpackFlags);
}

bool GrenadeController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'LIFE':

			data >> lifeTime;
			return (true);

		case 'STIM':

			data >> smokeTime;
			return (true);

		case 'COLL':

			data >> collisionCount;
			return (true);

		case 'TRAL':
		{
			int32	nodeIndex;

			data >> nodeIndex;
			data.AddNodeLink(nodeIndex, &SmokeTrailLinkProc, this);
			return (true);
		}
	}

	return (false);
}

void GrenadeController::SmokeTrailLinkProc(Node *node, void *cookie)
{
	GrenadeController *controller = static_cast<GrenadeController *>(cookie);
	controller->smokeTrail = static_cast<SmokeTrailParticleSystem *>(node);
}

void GrenadeController::Preprocess(void)
{
	ProjectileController::Preprocess();

	SetRigidBodyFlags(kRigidBodyPartialSleep);
	SetCollisionKind(GetCollisionKind() | kCollisionShot);
	SetRestitutionCoefficient(0.75F);

	Model *model = GetTargetNode();
	Transform4D transform = CalculateProjectileTransform(model->GetWorldPosition(), GetLinearVelocity());
	SetAngularVelocity(transform[0] * -2.0F);
	SetRigidBodyTransform(transform);
	model->Invalidate();

	if (!smokeTrail)
	{
		smokeTrail = new SmokeTrailParticleSystem(ColorRGBA(1.0F, 1.0F, 1.0F, 0.3F));
		model->GetWorld()->AddNewNode(smokeTrail);
	}
}

ControllerMessage *GrenadeController::CreateMessage(ControllerMessageType type) const
{
	switch (type)
	{
		case kGrenadeMessageTeleport:

			return (new GrenadeTeleportMessage(GetControllerIndex()));

		case kGrenadeMessageExplode:

			return (new GrenadeExplodeMessage(GetControllerIndex()));
	}

	return (ProjectileController::CreateMessage(type));
}

void GrenadeController::SendInitialStateMessages(Player *player) const
{
	player->SendMessage(CreateGrenadeMessage(GetControllerIndex(), GetAttackerIndex(), GetTargetNode()->GetWorldPosition(), GetLinearVelocity()));
}

void GrenadeController::Move(void)
{
	int32 dt = TheTimeMgr->GetDeltaTime();

	if (TheMessageMgr->Server())
	{
		if ((lifeTime -= dt) <= 0)
		{
			const Point3D& position = GetTargetNode()->GetWorldPosition();
			const Vector3D& velocity = GetLinearVelocity();

			float m = SquaredMag(velocity);
			if (m > K::min_float)
			{
				GrenadeController::Destroy(position, velocity * -InverseSqrt(m));
			}
			else
			{
				GrenadeController::Destroy(position, K::z_unit);
			}

			return;
		}
	}

	if ((smokeTime -= dt) <= 0)
	{
		smokeTime = MaxZero(smokeTime + 30);

		const Node *node = GetTargetNode();
		smokeTrail->CreateSmoke(node->GetWorldPosition() - node->GetWorldTransform()[2] * 0.125F, 750, 0.1F);
	}
}

RigidBodyStatus GrenadeController::HandleNewRigidBodyContact(const RigidBodyContact *contact, RigidBodyController *contactBody)
{
	if (TheMessageMgr->Server())
	{
		if (contactBody->GetRigidBodyType() == kRigidBodyCharacter)
		{
			Point3D		worldPosition;
			Vector3D	worldNormal;

			contact->GetWorldContactPosition(this, &worldPosition, &worldNormal);

			GrenadeController::Destroy(worldPosition, worldNormal);
			return (kRigidBodyDestroyed);
		}
	}

	Bounce();
	return (kRigidBodyUnchanged);
}

RigidBodyStatus GrenadeController::HandleNewGeometryContact(const GeometryContact *contact)
{
	const Geometry *geometry = contact->GetContactGeometry();
	const RemotePortal *portal = Game::DetectTeleporter(geometry);

	if (TheMessageMgr->Server())
	{
		if (portal)
		{
			// Grenade goes through a teleporting portal.

			Transform4D		portalTransform;

			portal->CalculateRemoteTransform(&portalTransform);
			const Transform4D& worldTransform = GetFinalTransform();

			Point3D position = portalTransform * worldTransform.GetTranslation();
			Vector3D velocity = portalTransform * GetOriginalLinearVelocity();
			Point3D center = worldTransform * (contact->GetRigidBodyContactPosition() - contact->GetRigidBodyContactNormal() * 0.5F);

			TheMessageMgr->SendMessageAll(GrenadeTeleportMessage(GetControllerIndex(), position, velocity, center));
			return (kRigidBodyContactsBroken);
		}
	}

	if (!portal)
	{
		Bounce();
	}

	return (kRigidBodyUnchanged);
}

void GrenadeController::EnterWorld(World *world, const Point3D& worldPosition)
{
	OmniSource *source = new OmniSource("grenade/gun/Fire", 32.0F);
	source->SetSourcePriority(kSoundPriorityWeapon);
	source->SetNodePosition(worldPosition);
	SetSourceVelocity(source);
	world->AddNewNode(source);
}

void GrenadeController::Destroy(const Point3D& position, const Vector3D& normal)
{
	GameWorld *world = static_cast<GameWorld *>(GetTargetNode()->GetWorld());
	world->ProduceSplashDamage(position, kGrenadeSplashDamageRadius, 70, GetAttackerController());

	TheMessageMgr->SendMessageAll(GrenadeExplodeMessage(GetControllerIndex(), position));
}

void GrenadeController::Bounce(void)
{
	static const char soundName[2][32] =
	{
		"grenade/grenade/Bounce1", "grenade/grenade/Bounce2"
	};

	OmniSource *source = new OmniSource(soundName[Math::Random(2)], 64.0F);
	source->SetSourcePriority(kSoundPriorityImpact);
	source->SetNodePosition(GetFinalTransform().GetTranslation());
	GetTargetNode()->GetWorld()->AddNewNode(source);

	lifeTime = Min(lifeTime, 2000);
}


CreateGrenadeMessage::CreateGrenadeMessage() : CreateModelMessage(kModelMessageGrenade)
{
}

CreateGrenadeMessage::CreateGrenadeMessage(int32 projectileIndex, int32 characterIndex, const Point3D& position, const Vector3D& velocity) : CreateModelMessage(kModelMessageGrenade, projectileIndex, position)
{
	initialVelocity = velocity;
	attackerIndex = characterIndex;
}

CreateGrenadeMessage::~CreateGrenadeMessage()
{
}

void CreateGrenadeMessage::Compress(Compressor& data) const
{
	CreateModelMessage::Compress(data);

	data << initialVelocity;
	data << attackerIndex;
}

bool CreateGrenadeMessage::Decompress(Decompressor& data)
{
	if (CreateModelMessage::Decompress(data))
	{
		data >> initialVelocity;
		data >> attackerIndex;
		return (true);
	}

	return (false);
}

bool CreateGrenadeMessage::HandleMessage(Player *sender) const
{
	GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
	if (world)
	{
		Controller *controller = new GrenadeController(initialVelocity, static_cast<GameCharacterController *>(world->GetController(attackerIndex)));
		InitializeModel(world, Model::Get(kModelGrenade), controller);
	}

	return (true);
}


GrenadeTeleportMessage::GrenadeTeleportMessage(int32 controllerIndex) : ControllerMessage(GrenadeController::kGrenadeMessageTeleport, controllerIndex)
{
}

GrenadeTeleportMessage::GrenadeTeleportMessage(int32 controllerIndex, const Point3D& position, const Vector3D& velocity, const Point3D& center) : ControllerMessage(GrenadeController::kGrenadeMessageTeleport, controllerIndex)
{
	teleportPosition = position;
	teleportVelocity = velocity;
	effectCenter = center;
}

GrenadeTeleportMessage::~GrenadeTeleportMessage()
{
}

void GrenadeTeleportMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << teleportPosition;
	data << teleportVelocity;
	data << effectCenter;
}

bool GrenadeTeleportMessage::Decompress(Decompressor& data)
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

void GrenadeTeleportMessage::HandleControllerMessage(Controller *controller) const
{
	GrenadeController *grenade = static_cast<GrenadeController *>(controller);

	Transform4D teleportTransform = ProjectileController::CalculateProjectileTransform(teleportPosition, teleportVelocity);
	grenade->Teleport(effectCenter, teleportTransform, teleportVelocity);
	grenade->SetAngularVelocity(teleportTransform[0] * -2.0F);
}


GrenadeExplodeMessage::GrenadeExplodeMessage(int32 controllerIndex) : ControllerMessage(GrenadeController::kGrenadeMessageExplode, controllerIndex)
{
}

GrenadeExplodeMessage::GrenadeExplodeMessage(int32 controllerIndex, const Point3D& position) : ControllerMessage(GrenadeController::kGrenadeMessageExplode, controllerIndex)
{
	explodePosition = position;
}

GrenadeExplodeMessage::~GrenadeExplodeMessage()
{
}

void GrenadeExplodeMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << explodePosition;
}

bool GrenadeExplodeMessage::Decompress(Decompressor& data)
{
	if (ControllerMessage::Decompress(data))
	{
		data >> explodePosition;
		return (true);
	}

	return (false);
}

void GrenadeExplodeMessage::HandleControllerMessage(Controller *controller) const
{
	MarkingData		markingData;

	Node *node = controller->GetTargetNode();
	World *world = node->GetWorld();

	OmniSource *source = new OmniSource("grenade/grenade/Explode", 256.0F);
	OmniSourceObject *sourceObject = source->GetObject();
	sourceObject->SetReflectionVolume(0.5F);
	sourceObject->SetReflectionHFVolume(0.5F);
	source->SetSourcePriority(kSoundPriorityExplosion);
	source->SetNodePosition(explodePosition);
	world->AddNewNode(source);

	if (TheWorldMgr->GetLightDetailLevel() > 0)
	{
		ColorRGB color(4.0F, 2.0F, 0.4F);
		PointLight *light = new PointLight(color, 10.0F);

		if (TheWorldMgr->GetLightDetailLevel() < 3)
		{
			light->GetObject()->SetLightFlags(kLightShadowInhibit);
		}

		light->SetController(new FlashController(color, 0.5F, 1000));
		light->SetNodePosition(explodePosition);
		world->AddNewNode(light);
	}

	const Vector3D& velocity = static_cast<RigidBodyController *>(controller)->GetLinearVelocity();

	GrenadeExplosionParticleSystem *grenadeExplosion = new GrenadeExplosionParticleSystem(velocity);
	grenadeExplosion->SetNodePosition(explodePosition);
	world->AddNewNode(grenadeExplosion);

	GrenadeSparksParticleSystem *grenadeSparks = new GrenadeSparksParticleSystem(velocity);
	grenadeSparks->SetNodePosition(explodePosition);
	world->AddNewNode(grenadeSparks);

	ShockwaveEffect *shockwave = new ShockwaveEffect("effects/Shock", 16.0F, 8.0F, 0.03125F);
	shockwave->SetNodePosition(explodePosition);
	world->AddNewNode(shockwave);

	SphereField *field = new SphereField(Vector3D(6.0F, 6.0F, 6.0F));
	field->SetNodePosition(explodePosition);
	field->SetForce(new ExplosionForce(80.0F, kCollisionRemains | kCollisionShot));
	field->SetController(new ExplosionController(250));
	world->AddNewNode(field);

	int32 t = Math::Random(256);
	markingData.center = explodePosition;
	markingData.normal = K::z_unit;
	markingData.tangent.Set(Math::GetTrigTable()[t], 0.0F);
	markingData.radius = 1.5F;
	markingData.textureName = "texture/Scorch";
	markingData.color.Set(0.0F, 0.0F, 0.0F, 1.0F);
	markingData.lifeTime = 30000;
	MarkingEffect::New(world, &markingData);

	static_cast<GameWorld *>(world)->ShakeCamera(explodePosition, 0.06F, 1000);

	delete node;
}

// ZYUQURM
