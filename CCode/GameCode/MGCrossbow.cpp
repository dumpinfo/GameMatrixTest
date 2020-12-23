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


#include "MGCrossbow.h"
#include "MGFighter.h"
#include "MGGame.h"


using namespace C4;


namespace
{
	const float kArrowSplashDamageRadius = 5.0F;

	Storage<Crossbow> crossbowStorage;
}


Crossbow::Crossbow() :
		Weapon(kWeaponCrossbow, kWeaponSecondaryAmmo, kModelCrossbow, kActionCrossbow, 24, 24, 0, 12),
		arrowControllerRegistration(kControllerArrow, nullptr),
		explosiveArrowControllerRegistration(kControllerExplosiveArrow, nullptr),
		crossbowModelRegistration(kModelCrossbow, TheGame->GetStringTable()->GetString(StringID('MODL', 'WEAP', kModelCrossbow)), "crossbow/gun/Crossbow", kModelPrecache, kControllerCollectable),
		arrowAmmoModelRegistration(kModelArrowAmmo, TheGame->GetStringTable()->GetString(StringID('MODL', 'AMMO', kModelArrowAmmo)), "crossbow/ammo/Ammo", kModelPrecache, kControllerCollectable),
		explosiveArrowAmmoModelRegistration(kModelExplosiveArrowAmmo, TheGame->GetStringTable()->GetString(StringID('MODL', 'AMMO', kModelExplosiveArrowAmmo)), "crossbow/ammo/Ammo-expl", kModelPrecache, kControllerCollectable),
		arrowModelRegistration(kModelArrow, nullptr, "crossbow/arrow/Arrow", kModelPrecache | kModelPrivate),
		crossbowAction(kWeaponCrossbow)
{
	TheInputMgr->AddAction(&crossbowAction);
}

Crossbow::~Crossbow()
{
}

void Crossbow::Construct(void)
{
	new(crossbowStorage) Crossbow;
}

void Crossbow::Destruct(void)
{
	crossbowStorage->~Crossbow();
}

WeaponController *Crossbow::NewWeaponController(FighterController *fighter) const
{
	return (new CrossbowController(fighter));
}


CrossbowController::CrossbowController(FighterController *fighter) : WeaponController(kControllerCrossbow, fighter)
{
	explosiveTime = 0;
}

CrossbowController::~CrossbowController()
{
}

void CrossbowController::Preprocess(void)
{
	WeaponController::Preprocess();

	Model *model = GetTargetNode();

	centerNode = model->FindNode(Text::StaticHash<'R', 'e', 'c', 'o', 'v', 'e', 'r', 'S', 't', 'r', 'i', 'n', 'g'>::value);
	stringNode[0] = centerNode->GetFirstSubnode();
	stringNode[1] = stringNode[0]->Next();

	stringNode[0]->Enable();
	stringNode[1]->Enable();

	wheelNode[0] = model->FindNode(Text::StaticHash<'W', 'h', 'e', 'e', 'l', 'L', 'e', 'f', 't'>::value)->GetFirstSubnode();
	wheelNode[1] = model->FindNode(Text::StaticHash<'W', 'h', 'e', 'e', 'l', 'R', 'i', 'g', 'h', 't'>::value)->GetFirstSubnode();

	axisNode = model->FindNode(Text::StaticHash<'A', 'x', 'i', 's'>::value);
	drumNode[0] = axisNode->GetFirstSubnode();
	drumNode[1] = drumNode[0]->Next();
	drumNode[2] = drumNode[1]->Next();

	for (machine a = 0; a < 3; a++)
	{
		Node *node = drumNode[a]->GetFirstSubnode();
		for (machine b = 0; b < 8; b++)
		{
			arrowNode[a][b] = node;
			node = node->Next();
		}
	}

	explosiveNode = model->FindNode(Text::StaticHash<'T', 'i', 'p'>::value);
	explosiveHeight = explosiveNode->GetNodePosition().z;

	ResetAmmo();

	frameAnimator.SetTargetModel(model);
	frameAnimator.SetAnimation("crossbow/gun/Fire");
	model->SetRootAnimator(&frameAnimator);
}

ControllerMessage *CrossbowController::CreateMessage(ControllerMessageType type) const
{ 
	switch (type)
	{
		case kCrossbowMessageFire: 

			return (new CrossbowFireMessage(GetControllerIndex())); 

		case kCrossbowMessageExplosive:
 
			return (new CrossbowExplosiveMessage(GetControllerIndex()));
	} 
 
	return (WeaponController::CreateMessage(type));
}

void CrossbowController::Move(void) 
{
	GetTargetNode()->Animate();

	if (centerNode->GetCurrentUpdateFlags() & Node::kUpdateTransform)
	{
		Point3D startPosition = stringNode[0]->GetSuperNode()->GetNodeTransform() * stringNode[0]->GetNodePosition();

		for (machine a = 0; a < 2; a++)
		{
			const Node *super = wheelNode[a]->GetSuperNode();
			Point3D targetPosition = super->GetSuperNode()->GetNodeTransform() * (super->GetNodeTransform() * wheelNode[a]->GetNodePosition());

			Vector3D xdir = targetPosition - startPosition;
			Vector3D ydir(-xdir.y, xdir.x, 0.0F);
			ydir.GetVector2D().Normalize();

			stringNode[a]->SetNodeMatrix3D(xdir, ydir, K::z_unit);
			stringNode[a]->Invalidate();
		}
	}

	float angle = currentDrumAngle;
	float target = targetDrumAngle;
	if (angle < target)
	{
		angle = Fmin(angle + TheTimeMgr->GetFloatDeltaTime() * 0.005F, target);
		currentDrumAngle = angle;

		int32 index = drumIndex;
		Node *node = drumNode[index];
		node->SetNodeMatrix3D(Matrix3D().SetRotationAboutX(angle * K::tau_over_8 - (float) index * K::tau_over_3));
		node->Invalidate();
	}

	angle = currentAxisAngle;
	target = targetAxisAngle;
	if (angle < target)
	{
		angle = Fmin(angle + TheTimeMgr->GetFloatDeltaTime() * 0.005F, target);
		currentAxisAngle = angle;

		axisNode->SetNodeMatrix3D(Matrix3D().SetRotationAboutX(angle * K::tau_over_3));
		axisNode->Invalidate();
	}

	if (explosiveTime > 0)
	{
		const Point3D& position = explosiveNode->GetNodePosition();
		float z = Fmin(position.z + TheTimeMgr->GetFloatDeltaTime() * 6.0e-5F, 0.05F);
		explosiveNode->SetNodePosition(Point3D(position.x, position.y, z));
		explosiveNode->Invalidate();
	}
}

void CrossbowController::ShowFireAnimation(bool empty)
{
	frameAnimator.GetFrameInterpolator()->Set(0.0F, 1.0F, kInterpolatorForward);

	int32 index = arrowIndex;
	arrowNode[drumIndex][index]->Disable();

	if (++index < 8)
	{
		arrowIndex = index;
		targetDrumAngle += 1.0F;
	}
	else
	{
		index = drumIndex + 1;
		if (index < 3)
		{
			drumIndex = index;
			arrowIndex = 0;

			currentDrumAngle = 0.0F;
			targetDrumAngle = 0.0F;
			targetAxisAngle += 1.0F;
		}
	}

	const Point3D& position = explosiveNode->GetNodePosition();
	explosiveNode->SetNodePosition(Point3D(position.x, position.y, explosiveHeight));
	explosiveNode->Invalidate();

	if (empty)
	{
		explosiveNode->Disable();
	}
}

void CrossbowController::ShowExplosiveAnimation(void)
{
	explosiveTime = 250;

	OmniSource *source = new OmniSource("crossbow/gun/Tip", 20.0F);
	source->SetSourcePriority(kSoundPriorityWeapon);
	source->SetNodePosition(explosiveNode->GetWorldPosition());
	GetTargetNode()->GetWorld()->AddNewNode(source);
}

WeaponResult CrossbowController::UpdateWeapon(const Point3D& position, const Vector3D& direction, const Point3D& center)
{
	int32 dt = TheTimeMgr->GetDeltaTime();

	int32 time = explosiveTime;
	if (time > 0)
	{
		time = MaxZero(time - dt);
		explosiveTime = time;

		if (TheMessageMgr->Server())
		{
			SetFireDelayTime(GetFireDelayTime() - dt);

			if (time == 0)
			{
				const FighterController *fighter = GetFighterController();
				GamePlayer *player = fighter->GetFighterPlayer();

				player->SetWeaponAmmo(kWeaponCrossbow, 0, player->GetWeaponAmmo(kWeaponCrossbow, 0) - 1);
				int32 explosiveAmmo = player->GetWeaponAmmo(kWeaponCrossbow, 1) - 1;
				player->SetWeaponAmmo(kWeaponCrossbow, 1, explosiveAmmo);

				const Model *model = GetTargetNode();
				World *world = model->GetWorld();
				int32 projectileIndex = world->NewControllerIndex();

				TheMessageMgr->SendMessageAll(CreateExplosiveArrowMessage(projectileIndex, fighter->GetControllerIndex(), position, direction * 60.0F));

				ProjectileController *projectileController = static_cast<ProjectileController *>(world->GetController(projectileIndex));
				projectileController->SetFirstPosition(center);

				TheMessageMgr->SendMessageAll(CrossbowFireMessage(GetControllerIndex(), (explosiveAmmo == 0)));
			}
		}

		return (kWeaponIdle);
	}

	if (TheMessageMgr->Server())
	{
		time = GetFireDelayTime() - dt;

		const FighterController *fighter = GetFighterController();
		unsigned_int32 flags = fighter->GetFighterFlags();

		if (((flags & kFighterFiring) != 0) && (time <= 0))
		{
			SetFireDelayTime(time + 750);

			GamePlayer *player = fighter->GetFighterPlayer();
			int32 ammo = player->GetWeaponAmmo(kWeaponCrossbow, 0);
			if (ammo <= 0)
			{
				return (kWeaponEmpty);
			}

			if (flags & kFighterFiringSecondary)
			{
				if (player->GetWeaponAmmo(kWeaponCrossbow, 1) > 0)
				{
					TheMessageMgr->SendMessageAll(CrossbowExplosiveMessage(GetControllerIndex()));
					return (kWeaponFired);
				}
			}

			player->SetWeaponAmmo(kWeaponCrossbow, 0, ammo - 1);

			const Model *model = GetTargetNode();
			World *world = model->GetWorld();
			int32 projectileIndex = world->NewControllerIndex();

			TheMessageMgr->SendMessageAll(CreateArrowMessage(projectileIndex, fighter->GetControllerIndex(), position, direction * 60.0F));

			ProjectileController *projectileController = static_cast<ProjectileController *>(world->GetController(projectileIndex));
			projectileController->SetFirstPosition(center);

			TheMessageMgr->SendMessageAll(CrossbowFireMessage(GetControllerIndex(), false));
			return (kWeaponFired);
		}
		else
		{
			SetFireDelayTime(time);
		}
	}

	return (kWeaponIdle);
}

void CrossbowController::ResetAmmo(void)
{
	drumIndex = 0;
	arrowIndex = 0;
	currentDrumAngle = 0.0F;
	targetDrumAngle = 0.0F;
	currentAxisAngle = 0.0F;
	targetAxisAngle = 0.0F;

	const FighterController *fighter = GetFighterController();
	const GamePlayer *player = fighter->GetFighterPlayer();
	int32 ammo = player->GetWeaponAmmo(kWeaponCrossbow, 0);

	for (machine a = 0; a < 24; a++)
	{
		Node *node = arrowNode[a >> 3][a & 7];
		if (a < ammo)
		{
			node->Enable();
		}
		else
		{
			node->Disable();
		}
	}

	if (player->GetWeaponAmmo(kWeaponCrossbow, 1) > 0)
	{
		explosiveNode->Enable();
	}
	else
	{
		explosiveNode->Disable();
	}
}

void CrossbowController::ResetWeapon(void)
{
	axisNode->SetNodeMatrix3D(Identity3D);
	axisNode->Invalidate();

	float angle = 0.0F;
	for (machine a = 0; a < 3; a++)
	{
		Node *node = drumNode[a];
		node->SetNodeMatrix3D(Matrix3D().SetRotationAboutX(angle));
		node->Invalidate();

		angle -= K::tau_over_3;
	}

	ResetAmmo();
}


CrossbowFireMessage::CrossbowFireMessage(int32 index) : ControllerMessage(CrossbowController::kCrossbowMessageFire, index)
{
}

CrossbowFireMessage::CrossbowFireMessage(int32 index, bool empty) : ControllerMessage(CrossbowController::kCrossbowMessageFire, index)
{
	explosiveEmpty = empty;
}

CrossbowFireMessage::~CrossbowFireMessage()
{
}

void CrossbowFireMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << explosiveEmpty;
}

bool CrossbowFireMessage::Decompress(Decompressor& data)
{
	if (ControllerMessage::Decompress(data))
	{
		data >> explosiveEmpty;
		return (true);
	}

	return (false);
}

void CrossbowFireMessage::HandleControllerMessage(Controller *controller) const
{
	static_cast<CrossbowController *>(controller)->ShowFireAnimation(explosiveEmpty);
}


CrossbowExplosiveMessage::CrossbowExplosiveMessage(int32 index) : ControllerMessage(CrossbowController::kCrossbowMessageExplosive, index)
{
}

CrossbowExplosiveMessage::~CrossbowExplosiveMessage()
{
}

void CrossbowExplosiveMessage::HandleControllerMessage(Controller *controller) const
{
	static_cast<CrossbowController *>(controller)->ShowExplosiveAnimation();
}


ArrowExplosionParticleSystem::ArrowExplosionParticleSystem() :
		TexcoordPointParticleSystem(kParticleSystemArrowExplosion, &particlePool, "particle/Puff1"),
		particlePool(kMaxParticleCount, particleArray)
{
}

ArrowExplosionParticleSystem::~ArrowExplosionParticleSystem()
{
}

bool ArrowExplosionParticleSystem::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	sphere->SetCenter(0.0F, 0.0F, 0.0F);
	sphere->SetRadius(16.0F);
	return (true);
}

void ArrowExplosionParticleSystem::Preprocess(void)
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

			float speed = Math::RandomFloat(0.015F) + 0.001F;
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

void ArrowExplosionParticleSystem::AnimateParticles(void)
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
					p->lifeTime = Math::Random(750) + 500;
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


ArrowSparksParticleSystem::ArrowSparksParticleSystem() :
		LineParticleSystem(kParticleSystemArrowSparks, &particlePool, "particle/Spark1"),
		particlePool(kMaxParticleCount, particleArray)
{
	SetLengthMultiplier(8.0F);
}

ArrowSparksParticleSystem::~ArrowSparksParticleSystem()
{
}

bool ArrowSparksParticleSystem::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	sphere->SetCenter(0.0F, 0.0F, 0.0F);
	sphere->SetRadius(10.0F);
	return (true);
}

void ArrowSparksParticleSystem::Preprocess(void)
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

void ArrowSparksParticleSystem::AnimateParticles(void)
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


ArrowController::ArrowController() : ProjectileController(kControllerArrow)
{
}

ArrowController::ArrowController(ControllerType type) : ProjectileController(type)
{
}

ArrowController::ArrowController(const Vector3D& velocity, GameCharacterController *attacker) : ProjectileController(kControllerArrow, velocity, attacker)
{
	lifeTime = 6000;
}

ArrowController::ArrowController(ControllerType type, const Vector3D& velocity, GameCharacterController *attacker) : ProjectileController(type, velocity, attacker)
{
	lifeTime = 6000;
}

ArrowController::ArrowController(const ArrowController& arrowController) : ProjectileController(arrowController)
{
	lifeTime = arrowController.lifeTime;
}

ArrowController::~ArrowController()
{
}

Controller *ArrowController::Replicate(void) const
{
	return (new ArrowController(*this));
}

void ArrowController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	ProjectileController::Pack(data, packFlags);

	data << ChunkHeader('life', 4);
	data << lifeTime;

	data << TerminatorChunk;
}

void ArrowController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	ProjectileController::Unpack(data, unpackFlags);
	UnpackChunkList<ArrowController>(data, unpackFlags);
}

bool ArrowController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'life':

			data >> lifeTime;
			return (true);
	}

	return (false);
}

void ArrowController::Preprocess(void)
{
	ProjectileController::Preprocess();

	SetRigidBodyFlags(kRigidBodyKeepAwake | kRigidBodyFixedOrientation);
	SetCollisionKind(GetCollisionKind() | kCollisionShot);
	SetGravityMultiplier(0.25F);

	Model *model = GetTargetNode();
	Zone *zone = model->GetOwningZone();

	const Vector3D& velocity = GetLinearVelocity();
	SetRigidBodyTransform(CalculateProjectileTransform(model->GetNodePosition(), zone->GetInverseWorldTransform() * velocity));
	model->Invalidate();
}

ControllerMessage *ArrowController::CreateMessage(ControllerMessageType type) const
{
	switch (type)
	{
		case kArrowMessageTeleport:

			return (new ArrowTeleportMessage(GetControllerIndex()));

		case kArrowMessageDestroy:

			return (new ArrowDestroyMessage(GetControllerIndex()));

		case kArrowMessageImpact:

			return (new ArrowImpactMessage(GetControllerIndex()));

		case kArrowMessageExplode:

			return (new ArrowExplodeMessage(GetControllerIndex()));
	}

	return (ProjectileController::CreateMessage(type));
}

void ArrowController::SendInitialStateMessages(Player *player) const
{
	player->SendMessage(CreateArrowMessage(GetControllerIndex(), GetAttackerIndex(), GetTargetNode()->GetWorldPosition(), GetLinearVelocity()));
}

void ArrowController::Move(void)
{
	if (TheMessageMgr->Server())
	{
		int32 dt = TheTimeMgr->GetDeltaTime();

		int32 time = lifeTime;
		if (time > 0)
		{
			lifeTime = (time -= dt);
			if (time <= 0)
			{
				Destroy(GetTargetNode()->GetWorldPosition(), Normalize(-GetLinearVelocity()));
			}
		}
		else
		{
			lifeTime = (time += dt);
			if (time >= 0)
			{
				ArrowController::Destroy(GetTargetNode()->GetWorldPosition(), Normalize(-GetLinearVelocity()));
			}
		}
	}
}

RigidBodyStatus ArrowController::HandleNewRigidBodyContact(const RigidBodyContact *contact, RigidBodyController *contactBody)
{
	if ((TheMessageMgr->Server()) && (lifeTime > 0))
	{
		Point3D		worldPosition;
		Vector3D	worldNormal;

		contact->GetWorldContactPosition(this, &worldPosition, &worldNormal);

		RigidBodyType type = contactBody->GetRigidBodyType();
		if (type == kRigidBodyCharacter)
		{
			Vector3D force = GetLinearVelocity() * 2.0F;
			GameCharacterController *character = static_cast<GameCharacterController *>(contactBody);
			character->Damage(30 << 16, 0, GetAttackerController(), &worldPosition, &force);

			TheMessageMgr->SendMessageAll(ArrowDestroyMessage(GetControllerIndex(), worldPosition, kSubstanceFlesh));
			return (kRigidBodyDestroyed);
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
				static_cast<ExplosiveController *>(contactBody)->Damage(30 << 16, 0.0F, GetAttackerController());
			}
		}

		const Shape *shape = (contact->GetStartElement() == this) ? contact->GetFinishShape() : contact->GetStartShape();
		SubstanceType substance = shape->GetObject()->GetShapeSubstance();

		TheMessageMgr->SendMessageAll(ArrowImpactMessage(GetControllerIndex(), worldPosition, substance, 0));
	}

	return (kRigidBodyUnchanged);
}

RigidBodyStatus ArrowController::HandleNewGeometryContact(const GeometryContact *contact)
{
	if ((TheMessageMgr->Server()) && (lifeTime > 0))
	{
		const Transform4D& worldTransform = GetFinalTransform();
		Vector3D worldNormal = -(worldTransform * contact->GetRigidBodyContactNormal());

		const Geometry *geometry = contact->GetContactGeometry();

		const RemotePortal *portal = Game::DetectTeleporter(geometry);
		if (portal)
		{
			// Arrow goes through a teleporting portal.

			Transform4D		portalTransform;

			portal->CalculateRemoteTransform(&portalTransform);

			Point3D position = portalTransform * worldTransform.GetTranslation();
			Vector3D velocity = portalTransform * GetOriginalLinearVelocity();
			Point3D center = worldTransform * contact->GetRigidBodyContactPosition() + worldNormal;

			TheMessageMgr->SendMessageAll(ArrowTeleportMessage(GetControllerIndex(), position, velocity, center));
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

		TheMessageMgr->SendMessageAll(ArrowImpactMessage(GetControllerIndex(), geometry->GetWorldTransform() * position, substance, kArrowImpactStick));
	}

	return (kRigidBodyUnchanged);
}

void ArrowController::EnterWorld(World *world, const Point3D& worldPosition)
{
	OmniSource *source = new OmniSource("crossbow/gun/Fire", 32.0F);
	source->SetSourcePriority(kSoundPriorityWeapon);
	source->SetNodePosition(worldPosition);
	SetSourceVelocity(source);
	world->AddNewNode(source);
}

void ArrowController::Destroy(const Point3D& position, const Vector3D& normal)
{
	TheMessageMgr->SendMessageAll(ArrowDestroyMessage(GetControllerIndex(), position));
}


ExplosiveArrowController::ExplosiveArrowController() : ArrowController(kControllerExplosiveArrow)
{
}

ExplosiveArrowController::ExplosiveArrowController(const Vector3D& velocity, GameCharacterController *attacker) : ArrowController(kControllerExplosiveArrow, velocity, attacker)
{
}

ExplosiveArrowController::ExplosiveArrowController(const ExplosiveArrowController& explosiveArrowController) : ArrowController(explosiveArrowController)
{
}

ExplosiveArrowController::~ExplosiveArrowController()
{
}

Controller *ExplosiveArrowController::Replicate(void) const
{
	return (new ExplosiveArrowController(*this));
}

void ExplosiveArrowController::SendInitialStateMessages(Player *player) const
{
	player->SendMessage(CreateExplosiveArrowMessage(GetControllerIndex(), GetAttackerIndex(), GetTargetNode()->GetWorldPosition(), GetLinearVelocity()));
}

void ExplosiveArrowController::Preprocess(void)
{
	ArrowController::Preprocess();

	Node *node = GetTargetNode()->GetFirstSubnode();
	while (node)
	{
		if (node->GetNodeType() == kNodeGeometry)
		{
			node->GetFirstSubnode()->Enable();
			break;
		}

		node = node->Next();
	}
}

RigidBodyStatus ExplosiveArrowController::HandleNewRigidBodyContact(const RigidBodyContact *contact, RigidBodyController *contactBody)
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

		ExplosiveArrowController::Destroy(worldPosition, -worldNormal);
		return (kRigidBodyDestroyed);
	}

	return (kRigidBodyUnchanged);
}

RigidBodyStatus ExplosiveArrowController::HandleNewGeometryContact(const GeometryContact *contact)
{
	if (TheMessageMgr->Server())
	{
		const Transform4D& worldTransform = GetFinalTransform();
		Vector3D worldNormal = -(worldTransform * contact->GetRigidBodyContactNormal());

		const Geometry *geometry = contact->GetContactGeometry();

		const RemotePortal *portal = Game::DetectTeleporter(geometry);
		if (portal)
		{
			// Arrow goes through a teleporting portal.

			Transform4D		portalTransform;

			portal->CalculateRemoteTransform(&portalTransform);

			Point3D position = portalTransform * worldTransform.GetTranslation();
			Vector3D velocity = portalTransform * GetOriginalLinearVelocity();
			Point3D center = worldTransform * contact->GetRigidBodyContactPosition() + worldNormal;

			TheMessageMgr->SendMessageAll(ArrowTeleportMessage(GetControllerIndex(), position, velocity, center));
			return (kRigidBodyContactsBroken);
		}

		// The splash damage will not cause the contact to be deleted in this case.

		const Point3D& position = contact->GetGeometryContactPosition();
		Game::ProcessGeometryProperties(geometry, position, GetAttackerNode());

		ExplosiveArrowController::Destroy(geometry->GetWorldTransform() * position, worldNormal);
		return (kRigidBodyDestroyed);
	}

	return (kRigidBodyUnchanged);
}

void ExplosiveArrowController::Destroy(const Point3D& position, const Vector3D& normal)
{
	GameWorld *world = static_cast<GameWorld *>(GetTargetNode()->GetWorld());
	world->ProduceSplashDamage(position, kArrowSplashDamageRadius, 60, GetAttackerController());

	TheMessageMgr->SendMessageAll(ArrowExplodeMessage(GetControllerIndex(), position, normal));
}


CreateArrowMessage::CreateArrowMessage() : CreateModelMessage(kModelMessageArrow)
{
}

CreateArrowMessage::CreateArrowMessage(int32 projectileIndex, int32 characterIndex, const Point3D& position, const Vector3D& velocity) : CreateModelMessage(kModelMessageArrow, projectileIndex, position)
{
	initialVelocity = velocity;
	attackerIndex = characterIndex;
}

CreateArrowMessage::~CreateArrowMessage()
{
}

void CreateArrowMessage::Compress(Compressor& data) const
{
	CreateModelMessage::Compress(data);

	data << initialVelocity;
	data << attackerIndex;
}

bool CreateArrowMessage::Decompress(Decompressor& data)
{
	if (CreateModelMessage::Decompress(data))
	{
		data >> initialVelocity;
		data >> attackerIndex;
		return (true);
	}

	return (false);
}

bool CreateArrowMessage::HandleMessage(Player *sender) const
{
	GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
	if (world)
	{
		Controller *controller = new ArrowController(initialVelocity, static_cast<GameCharacterController *>(world->GetController(attackerIndex)));
		InitializeModel(world, Model::Get(kModelArrow), controller);
	}

	return (true);
}


CreateExplosiveArrowMessage::CreateExplosiveArrowMessage() : CreateModelMessage(kModelMessageExplosiveArrow)
{
}

CreateExplosiveArrowMessage::CreateExplosiveArrowMessage(int32 projectileIndex, int32 characterIndex, const Point3D& position, const Vector3D& velocity) : CreateModelMessage(kModelMessageExplosiveArrow, projectileIndex, position)
{
	initialVelocity = velocity;
	attackerIndex = characterIndex;
}

CreateExplosiveArrowMessage::~CreateExplosiveArrowMessage()
{
}

void CreateExplosiveArrowMessage::Compress(Compressor& data) const
{
	CreateModelMessage::Compress(data);

	data << initialVelocity;
	data << attackerIndex;
}

bool CreateExplosiveArrowMessage::Decompress(Decompressor& data)
{
	if (CreateModelMessage::Decompress(data))
	{
		data >> initialVelocity;
		data >> attackerIndex;
		return (true);
	}

	return (false);
}

bool CreateExplosiveArrowMessage::HandleMessage(Player *sender) const
{
	GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
	if (world)
	{
		Controller *controller = new ExplosiveArrowController(initialVelocity, static_cast<GameCharacterController *>(world->GetController(attackerIndex)));
		InitializeModel(world, Model::Get(kModelArrow), controller);
	}

	return (true);
}


ArrowTeleportMessage::ArrowTeleportMessage(int32 controllerIndex) : ControllerMessage(ArrowController::kArrowMessageTeleport, controllerIndex)
{
}

ArrowTeleportMessage::ArrowTeleportMessage(int32 controllerIndex, const Point3D& position, const Vector3D& velocity, const Point3D& center) : ControllerMessage(ArrowController::kArrowMessageTeleport, controllerIndex)
{
	teleportPosition = position;
	teleportVelocity = velocity;
	effectCenter = center;
}

ArrowTeleportMessage::~ArrowTeleportMessage()
{
}

void ArrowTeleportMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << teleportPosition;
	data << teleportVelocity;
	data << effectCenter;
}

bool ArrowTeleportMessage::Decompress(Decompressor& data)
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

void ArrowTeleportMessage::HandleControllerMessage(Controller *controller) const
{
	ProjectileController *arrow = static_cast<ProjectileController *>(controller);

	Transform4D teleportTransform = ProjectileController::CalculateProjectileTransform(teleportPosition, teleportVelocity);
	arrow->Teleport(effectCenter, teleportTransform, teleportVelocity);
}


ArrowDestroyMessage::ArrowDestroyMessage(int32 controllerIndex) : ControllerMessage(ArrowController::kArrowMessageDestroy, controllerIndex)
{
}

ArrowDestroyMessage::ArrowDestroyMessage(int32 controllerIndex, const Point3D& position, SubstanceType substance) : ControllerMessage(ArrowController::kArrowMessageDestroy, controllerIndex)
{
	destroyPosition = position;
	destroySubstance = substance;
}

ArrowDestroyMessage::~ArrowDestroyMessage()
{
}

void ArrowDestroyMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << destroyPosition;
	data << destroySubstance;
}

bool ArrowDestroyMessage::Decompress(Decompressor& data)
{
	if (ControllerMessage::Decompress(data))
	{
		data >> destroyPosition;
		data >> destroySubstance;
		return (true);
	}

	return (false);
}

void ArrowDestroyMessage::HandleControllerMessage(Controller *controller) const
{
	Node *node = controller->GetTargetNode();

	if (destroySubstance == kSubstanceFlesh)
	{
		static const char soundName[2][32] =
		{
			"spike/spike/Flesh1", "spike/spike/Flesh2"
		};

		OmniSource *source = new OmniSource(soundName[Math::Random(2)], 64.0F);
		source->SetSourcePriority(kSoundPriorityImpact);
		source->SetNodePosition(destroyPosition);
		node->GetWorld()->AddNewNode(source);
	}

	delete node;
}


ArrowImpactMessage::ArrowImpactMessage(int32 controllerIndex) : ControllerMessage(ArrowController::kArrowMessageImpact, controllerIndex)
{
}

ArrowImpactMessage::ArrowImpactMessage(int32 controllerIndex, const Point3D& position, SubstanceType substance, unsigned_int32 mask) : ControllerMessage(ArrowController::kArrowMessageImpact, controllerIndex)
{
	impactPosition = position;
	impactSubstance = substance;
	impactMask = mask;
}

ArrowImpactMessage::~ArrowImpactMessage()
{
}

void ArrowImpactMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << impactPosition;
	data << impactSubstance;
	data << (unsigned_int8) impactMask;
}

bool ArrowImpactMessage::Decompress(Decompressor& data)
{
	if (ControllerMessage::Decompress(data))
	{
		unsigned_int8	mask;

		data >> impactPosition;
		data >> impactSubstance;

		data >> mask;
		impactMask = mask;
		return (true);
	}

	return (false);
}

void ArrowImpactMessage::HandleControllerMessage(Controller *controller) const
{
	ArrowController *arrowController = static_cast<ArrowController *>(controller);

	unsigned_int32 impactFlags = 0;
	if (impactSubstance != kSubstanceNone)
	{
		const GameSubstance *substance = static_cast<const GameSubstance *>(MaterialObject::FindRegisteredSubstance(impactSubstance));
		if (substance)
		{
			const SubstanceData *substanceData = substance->GetSubstanceData();
			impactFlags = substanceData->arrowImpactFlags & impactMask;

			const char *name = (impactFlags & kArrowImpactStick) ? substanceData->arrowStickSoundName : substanceData->arrowImpactSoundName;
			OmniSource *source = new OmniSource(name, 64.0F);
			source->SetSourcePriority(kSoundPriorityImpact);
			source->SetNodePosition(impactPosition);
			arrowController->GetTargetNode()->GetWorld()->AddNewNode(source);
		}
	}

	if (impactFlags & kArrowImpactStick)
	{
		arrowController->SetRigidBodyFlags(kRigidBodyPartialSleep);
		arrowController->SetCollisionExclusionMask(kCollisionExcludeAll);
		arrowController->SetRigidBodyPosition(impactPosition);
		arrowController->GetTargetNode()->Invalidate();
		arrowController->SetLinearVelocity(Zero3D);
		arrowController->SetAngularVelocity(Zero3D);
		arrowController->Sleep();
	}
	else
	{
		arrowController->SetRigidBodyFlags(kRigidBodyPartialSleep | kRigidBodyLocalSimulation);
		arrowController->SetCollisionKind(kCollisionRemains);
		arrowController->SetCollisionExclusionMask(kCollisionCharacter | kCollisionProjectile | kCollisionCorpse | kCollisionRemains);
		arrowController->SetGravityMultiplier(1.0F);
		arrowController->SetRollingResistance(2.0F);
		arrowController->SetSleepBoxMultiplier(2.0F);
		arrowController->SetLinearVelocity(arrowController->GetLinearVelocity() * 0.125F);
	}

	arrowController->SetLifeTime(-5000);
}


ArrowExplodeMessage::ArrowExplodeMessage(int32 controllerIndex) : ControllerMessage(ArrowController::kArrowMessageExplode, controllerIndex)
{
}

ArrowExplodeMessage::ArrowExplodeMessage(int32 controllerIndex, const Point3D& position, const Vector3D& normal) : ControllerMessage(ArrowController::kArrowMessageExplode, controllerIndex)
{
	explodePosition = position;
	explodeNormal = normal;
}

ArrowExplodeMessage::~ArrowExplodeMessage()
{
}

void ArrowExplodeMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << explodePosition;
	data << explodeNormal;
}

bool ArrowExplodeMessage::Decompress(Decompressor& data)
{
	if (ControllerMessage::Decompress(data))
	{
		data >> explodePosition;
		data >> explodeNormal;
		return (true);
	}

	return (false);
}

void ArrowExplodeMessage::HandleControllerMessage(Controller *controller) const
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

	ArrowExplosionParticleSystem *arrowExplosion = new ArrowExplosionParticleSystem;
	arrowExplosion->SetNodePosition(explodePosition);
	world->AddNewNode(arrowExplosion);

	ArrowSparksParticleSystem *arrowSparks = new ArrowSparksParticleSystem;
	arrowSparks->SetNodePosition(explodePosition);
	world->AddNewNode(arrowSparks);

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

	static_cast<GameWorld *>(world)->ShakeCamera(explodePosition, 0.05F, 1000);

	delete node;
}

// ZYUQURM
