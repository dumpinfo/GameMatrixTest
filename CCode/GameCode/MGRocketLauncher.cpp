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


#include "MGRocketLauncher.h"
#include "MGFighter.h"
#include "MGGame.h"


using namespace C4;


namespace
{
	const float kRocketSplashDamageRadius = 5.0F;

	Storage<RocketLauncher> rocketLauncherStorage;
}


RocketLauncher::RocketLauncher() :
		Weapon(kWeaponRocketLauncher, 0, kModelRocketLauncher, kActionRocketLauncher, 15, 40),
		rocketControllerRegistration(kControllerRocket, nullptr),
		rocketLauncherModelRegistration(kModelRocketLauncher, TheGame->GetStringTable()->GetString(StringID('MODL', 'WEAP', kModelRocketLauncher)), "rocket/gun/RocketLauncher", kModelPrecache, kControllerCollectable),
		rocketAmmoModelRegistration(kModelRocketAmmo, TheGame->GetStringTable()->GetString(StringID('MODL', 'AMMO', kModelRocketAmmo)), "rocket/ammo/Ammo", kModelPrecache, kControllerCollectable),
		rocketModelRegistration(kModelRocket, nullptr, "rocket/rocket/Rocket", kModelPrecache | kModelPrivate),
		rocketLauncherAction(kWeaponRocketLauncher)
{
	TheInputMgr->AddAction(&rocketLauncherAction);
}

RocketLauncher::~RocketLauncher()
{
}

void RocketLauncher::Construct(void)
{
	new(rocketLauncherStorage) RocketLauncher;
}

void RocketLauncher::Destruct(void)
{
	rocketLauncherStorage->~RocketLauncher();
}

WeaponController *RocketLauncher::NewWeaponController(FighterController *fighter) const
{
	return (new RocketLauncherController(fighter));
}


RocketLauncherController::RocketLauncherController(FighterController *fighter) : WeaponController(kControllerRocketLauncher, fighter)
{
}

RocketLauncherController::~RocketLauncherController()
{
}

void RocketLauncherController::Preprocess(void)
{
	WeaponController::Preprocess();

	currentDrumAngle = 0.0F;
	targetDrumAngle = 0.0F;

	drumNode = GetTargetNode()->FindNode(Text::StaticHash<'D', 'r', 'u', 'm'>::value);
}

ControllerMessage *RocketLauncherController::CreateMessage(ControllerMessageType type) const
{
	switch (type)
	{
		case kRocketLauncherMessageAdvance:

			return (new RocketLauncherAdvanceMessage(GetControllerIndex()));
	}

	return (WeaponController::CreateMessage(type));
}

void RocketLauncherController::Move(void)
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
} 

WeaponResult RocketLauncherController::UpdateWeapon(const Point3D& position, const Vector3D& direction, const Point3D& center)
{ 
	if (TheMessageMgr->Server())
	{ 
		int32 dt = TheTimeMgr->GetDeltaTime();
		int32 time = GetFireDelayTime() - dt;
 
		const FighterController *fighter = GetFighterController();
		unsigned_int32 flags = fighter->GetFighterFlags(); 
 
		if (((flags & kFighterFiring) != 0) && (time <= 0))
		{
			SetFireDelayTime(time + 750);
 
			GamePlayer *player = fighter->GetFighterPlayer();
			int32 ammo = player->GetWeaponAmmo(kWeaponRocketLauncher, 0);
			if (ammo <= 0)
			{
				return (kWeaponEmpty);
			}

			player->SetWeaponAmmo(kWeaponRocketLauncher, 0, ammo - 1);

			World *world = GetTargetNode()->GetWorld();
			int32 projectileIndex = world->NewControllerIndex();
			TheMessageMgr->SendMessageAll(CreateRocketMessage(projectileIndex, fighter->GetControllerIndex(), position, direction * 50.0F));

			ProjectileController *projectileController = static_cast<ProjectileController *>(world->GetController(projectileIndex));
			projectileController->SetFirstPosition(center);

			TheMessageMgr->SendMessageAll(RocketLauncherAdvanceMessage(GetControllerIndex()));
			return (kWeaponFired);
		}
		else
		{
			SetFireDelayTime(time);
		}
	}

	return (kWeaponIdle);
}


RocketLauncherAdvanceMessage::RocketLauncherAdvanceMessage(int32 index) : ControllerMessage(RocketLauncherController::kRocketLauncherMessageAdvance, index)
{
}

RocketLauncherAdvanceMessage::~RocketLauncherAdvanceMessage()
{
}

void RocketLauncherAdvanceMessage::HandleControllerMessage(Controller *controller) const
{
	static_cast<RocketLauncherController *>(controller)->ShowFireAnimation();
}


RocketExplosionParticleSystem::RocketExplosionParticleSystem() :
		TexcoordPointParticleSystem(kParticleSystemRocketExplosion, &particlePool, "particle/Puff1"),
		particlePool(kMaxParticleCount, particleArray)
{
}

RocketExplosionParticleSystem::~RocketExplosionParticleSystem()
{
}

bool RocketExplosionParticleSystem::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	sphere->SetCenter(0.0F, 0.0F, 0.0F);
	sphere->SetRadius(16.0F);
	return (true);
}

void RocketExplosionParticleSystem::Preprocess(void)
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

			int32 i = Math::Random(4);
			particle->texcoordScale.Set(0.5F, 0.5F);
			particle->texcoordOffset.Set((i & 1) ? 0.5F : 0.0F, (i & 2) ? 0.5F : 0.0F);

			particle->smokeCount = 0;
			AddParticle(particle);
		}
	}
}

void RocketExplosionParticleSystem::AnimateParticles(void)
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


RocketSparksParticleSystem::RocketSparksParticleSystem() :
		LineParticleSystem(kParticleSystemRocketSparks, &particlePool, "particle/Spark1"),
		particlePool(kMaxParticleCount, particleArray)
{
	SetLengthMultiplier(8.0F);
}

RocketSparksParticleSystem::~RocketSparksParticleSystem()
{
}

bool RocketSparksParticleSystem::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	sphere->SetCenter(0.0F, 0.0F, 0.0F);
	sphere->SetRadius(10.0F);
	return (true);
}

void RocketSparksParticleSystem::Preprocess(void)
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

			AddParticle(particle);
		}
	}
}

void RocketSparksParticleSystem::AnimateParticles(void)
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


RocketController::RocketController() : ProjectileController(kControllerRocket)
{
	smokeTrail = nullptr;
}

RocketController::RocketController(const Vector3D& velocity, GameCharacterController *attacker) : ProjectileController(kControllerRocket, velocity, attacker)
{
	lifeTime = 10000;

	smokeDistance = 0.0F;
	smokeTrail = nullptr;
}

RocketController::RocketController(const RocketController& rocketController) : ProjectileController(rocketController)
{
	lifeTime = rocketController.lifeTime;

	smokeDistance = 0.0F;
	smokeTrail = nullptr;
}

RocketController::~RocketController()
{
	if (smokeTrail)
	{
		smokeTrail->Finalize();
	}
}

Controller *RocketController::Replicate(void) const
{
	return (new RocketController(*this));
}

void RocketController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	ProjectileController::Pack(data, packFlags);

	data << ChunkHeader('LIFE', 4);
	data << lifeTime;

	data << ChunkHeader('SDST', 4);
	data << smokeDistance;

	if ((smokeTrail) && (smokeTrail->LinkedNodePackable(packFlags)))
	{
		data << ChunkHeader('TRAL', 4);
		data << smokeTrail->GetNodeIndex();
	}

	data << TerminatorChunk;
}

void RocketController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	ProjectileController::Unpack(data, unpackFlags);
	UnpackChunkList<RocketController>(data, unpackFlags);
}

bool RocketController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'LIFE':

			data >> lifeTime;
			return (true);

		case 'SDST':

			data >> smokeDistance;
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

void RocketController::SmokeTrailLinkProc(Node *node, void *cookie)
{
	RocketController *controller = static_cast<RocketController *>(cookie);
	controller->smokeTrail = static_cast<SmokeTrailParticleSystem *>(node);
}

void RocketController::Preprocess(void)
{
	ProjectileController::Preprocess();

	SetRigidBodyFlags(kRigidBodyKeepAwake | kRigidBodyFixedOrientation | kRigidBodyDisabledContact);
	SetCollisionKind(GetCollisionKind() | kCollisionShot);
	SetGravityMultiplier(0.0F);
	teleportFlag = false;

	Model *model = GetTargetNode();
	const Vector3D& velocity = GetLinearVelocity();
	SetRigidBodyTransform(CalculateProjectileTransform(model->GetWorldPosition(), velocity));
	model->Invalidate();

	Node *node = model->GetFirstSubnode();
	while (node)
	{
		NodeType type = node->GetNodeType();
		if (type == kNodeLight)
		{
			const GameCharacterController *controller = GetAttackerController();
			if (controller)
			{
				static_cast<Light *>(node)->SetExcludedNode(controller->GetTargetNode());
			}
		}
		else if (type == kNodeSource)
		{
			rocketSource = static_cast<OmniSource *>(node);
			rocketSource->SetSourceVelocity(velocity);
		}

		node = node->Next();
	}

	if (!smokeTrail)
	{
		smokeTrail = new SmokeTrailParticleSystem(ColorRGBA(1.0F, 1.0F, 1.0F, 0.3F));
		model->GetWorld()->AddNewNode(smokeTrail);
	}
}

ControllerMessage *RocketController::CreateMessage(ControllerMessageType type) const
{
	switch (type)
	{
		case kRocketMessageTeleport:

			return (new RocketTeleportMessage(GetControllerIndex()));

		case kRocketMessageExplode:

			return (new RocketExplodeMessage(GetControllerIndex()));
	}

	return (ProjectileController::CreateMessage(type));
}

void RocketController::SendInitialStateMessages(Player *player) const
{
	player->SendMessage(CreateRocketMessage(GetControllerIndex(), GetAttackerIndex(), GetTargetNode()->GetWorldPosition(), GetLinearVelocity()));
}

void RocketController::Move(void)
{
	if (TheMessageMgr->Server())
	{
		if ((lifeTime -= TheTimeMgr->GetDeltaTime()) <= 0)
		{
			RocketController::Destroy(GetTargetNode()->GetWorldPosition(), Normalize(-GetLinearVelocity()));
			return;
		}
	}

	if (!teleportFlag)
	{
		Vector3D direction = GetLinearVelocity();
		float m = SquaredMag(direction);
		float r = InverseSqrt(m);
		direction *= r;

		float d = smokeDistance + m * r * TheTimeMgr->GetDeltaSeconds();
		if (d >= 0.5F)
		{
			const Point3D& position = GetFinalTransform().GetTranslation();

			float count = PositiveFloor(d * 2.0F);
			smokeDistance = d - count * 0.5F;
			count = Fmin(count, 4.0F);
			do
			{
				float f = d * Math::RandomFloat(1.0F);
				smokeTrail->CreateSmoke(position - direction * f, Math::Random(1500) + 1000, 0.25F);
			} while ((count -= 1.0F) > 0.0F);
		}
		else
		{
			smokeDistance = d;
		}
	}
	else
	{
		teleportFlag = false;
	}
}

RigidBodyStatus RocketController::HandleNewRigidBodyContact(const RigidBodyContact *contact, RigidBodyController *contactBody)
{
	if (TheMessageMgr->Server())
	{
		Point3D		worldPosition;
		Vector3D	worldNormal;

		contact->GetWorldContactPosition(this, &worldPosition, &worldNormal);

		if (contactBody->GetRigidBodyType() == kRigidBodyProjectile)
		{
			static_cast<ProjectileController *>(contactBody)->Destroy(worldPosition, worldNormal);
		}

		RocketController::Destroy(worldPosition, -worldNormal);
		return (kRigidBodyDestroyed);
	}

	return (kRigidBodyUnchanged);
}

RigidBodyStatus RocketController::HandleNewGeometryContact(const GeometryContact *contact)
{
	if (TheMessageMgr->Server())
	{
		const Transform4D& worldTransform = GetFinalTransform();
		Vector3D worldNormal = -(worldTransform * contact->GetRigidBodyContactNormal());

		const Geometry *geometry = contact->GetContactGeometry();

		const RemotePortal *portal = Game::DetectTeleporter(geometry);
		if (portal)
		{
			// Rocket goes through a teleporting portal.

			Transform4D		portalTransform;

			portal->CalculateRemoteTransform(&portalTransform);

			Point3D position = portalTransform * worldTransform.GetTranslation();
			Vector3D velocity = portalTransform * GetOriginalLinearVelocity();
			Point3D center = worldTransform * contact->GetRigidBodyContactPosition() + worldNormal;

			TheMessageMgr->SendMessageAll(RocketTeleportMessage(GetControllerIndex(), position, velocity, center));
			return (kRigidBodyContactsBroken);
		}

		// The splash damage will not cause the contact to be deleted in this case.

		const Point3D& position = contact->GetGeometryContactPosition();
		Game::ProcessGeometryProperties(geometry, position, GetAttackerNode());

		RocketController::Destroy(geometry->GetWorldTransform() * position, worldNormal);
		return (kRigidBodyDestroyed);
	}

	return (kRigidBodyUnchanged);
}

void RocketController::EnterWorld(World *world, const Point3D& worldPosition)
{
	OmniSource *source = new OmniSource("rocket/gun/Fire", 32.0F);
	source->SetSourcePriority(kSoundPriorityWeapon);
	source->SetNodePosition(worldPosition);
	SetSourceVelocity(source);
	world->AddNewNode(source);
}

void RocketController::Destroy(const Point3D& position, const Vector3D& normal)
{
	GameWorld *world = static_cast<GameWorld *>(GetTargetNode()->GetWorld());
	world->ProduceSplashDamage(position, kRocketSplashDamageRadius, 75, GetAttackerController());

	TheMessageMgr->SendMessageAll(RocketExplodeMessage(GetControllerIndex(), position, normal));
}


CreateRocketMessage::CreateRocketMessage() : CreateModelMessage(kModelMessageRocket)
{
}

CreateRocketMessage::CreateRocketMessage(int32 projectileIndex, int32 characterIndex, const Point3D& position, const Vector3D& velocity) : CreateModelMessage(kModelMessageRocket, projectileIndex, position)
{
	initialVelocity = velocity;
	attackerIndex = characterIndex;
}

CreateRocketMessage::~CreateRocketMessage()
{
}

void CreateRocketMessage::Compress(Compressor& data) const
{
	CreateModelMessage::Compress(data);

	data << initialVelocity;
	data << attackerIndex;
}

bool CreateRocketMessage::Decompress(Decompressor& data)
{
	if (CreateModelMessage::Decompress(data))
	{
		data >> initialVelocity;
		data >> attackerIndex;
		return (true);
	}

	return (false);
}

bool CreateRocketMessage::HandleMessage(Player *sender) const
{
	GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
	if (world)
	{
		Controller *controller = new RocketController(initialVelocity, static_cast<GameCharacterController *>(world->GetController(attackerIndex)));
		InitializeModel(world, Model::Get(kModelRocket), controller);
	}

	return (true);
}


RocketTeleportMessage::RocketTeleportMessage(int32 controllerIndex) : ControllerMessage(RocketController::kRocketMessageTeleport, controllerIndex)
{
}

RocketTeleportMessage::RocketTeleportMessage(int32 controllerIndex, const Point3D& position, const Vector3D& velocity, const Point3D& center) : ControllerMessage(RocketController::kRocketMessageTeleport, controllerIndex)
{
	teleportPosition = position;
	teleportVelocity = velocity;
	effectCenter = center;
}

RocketTeleportMessage::~RocketTeleportMessage()
{
}

void RocketTeleportMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << teleportPosition;
	data << teleportVelocity;
	data << effectCenter;
}

bool RocketTeleportMessage::Decompress(Decompressor& data)
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

void RocketTeleportMessage::HandleControllerMessage(Controller *controller) const
{
	RocketController *rocket = static_cast<RocketController *>(controller);

	Transform4D teleportTransform = ProjectileController::CalculateProjectileTransform(teleportPosition, teleportVelocity);
	rocket->Teleport(effectCenter, teleportTransform, teleportVelocity);
	rocket->SetRocketSourceVelocity(teleportVelocity);
	rocket->SetTeleportFlag();
}


RocketExplodeMessage::RocketExplodeMessage(int32 controllerIndex) : ControllerMessage(RocketController::kRocketMessageExplode, controllerIndex)
{
}

RocketExplodeMessage::RocketExplodeMessage(int32 controllerIndex, const Point3D& position, const Vector3D& normal) : ControllerMessage(RocketController::kRocketMessageExplode, controllerIndex)
{
	explodePosition = position;
	explodeNormal = normal;
}

RocketExplodeMessage::~RocketExplodeMessage()
{
}

void RocketExplodeMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << explodePosition;
	data << explodeNormal;
}

bool RocketExplodeMessage::Decompress(Decompressor& data)
{
	if (ControllerMessage::Decompress(data))
	{
		data >> explodePosition;
		data >> explodeNormal;
		return (true);
	}

	return (false);
}

void RocketExplodeMessage::HandleControllerMessage(Controller *controller) const
{
	MarkingData		markingData;

	Node *node = controller->GetTargetNode();
	World *world = node->GetWorld();

	OmniSource *source = new OmniSource("rocket/rocket/Explode", 256.0F);
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
		light->SetNodePosition(explodePosition + explodeNormal);
		world->AddNewNode(light);
	}

	RocketExplosionParticleSystem *rocketExplosion = new RocketExplosionParticleSystem;
	rocketExplosion->SetNodePosition(explodePosition);
	world->AddNewNode(rocketExplosion);

	RocketSparksParticleSystem *rocketSparks = new RocketSparksParticleSystem;
	rocketSparks->SetNodePosition(explodePosition);
	world->AddNewNode(rocketSparks);

	ShockwaveEffect *shockwave = new ShockwaveEffect("effects/Shock", 16.0F, 8.0F, 0.03125F);
	shockwave->SetNodePosition(explodePosition);
	world->AddNewNode(shockwave);

	SphereField *field = new SphereField(Vector3D(6.0F, 6.0F, 6.0F));
	field->SetNodePosition(explodePosition);
	field->SetForce(new ExplosionForce(80.0F, kCollisionRemains | kCollisionShot));
	field->SetController(new ExplosionController(250));
	world->AddNewNode(field);

	markingData.center = explodePosition - explodeNormal * static_cast<RigidBodyController *>(controller)->GetBoundingRadius();
	markingData.normal = explodeNormal;
	markingData.tangent = Math::CreatePerpendicular(explodeNormal);
	markingData.tangent.RotateAboutAxis(Math::RandomFloat(K::tau), explodeNormal);
	markingData.radius = 1.5F;
	markingData.textureName = "texture/Scorch";
	markingData.color.Set(0.0F, 0.0F, 0.0F, 1.0F);
	markingData.lifeTime = 30000;
	MarkingEffect::New(world, &markingData);

	static_cast<GameWorld *>(world)->ShakeCamera(explodePosition, 0.06F, 1000);

	delete node;
}

// ZYUQURM
