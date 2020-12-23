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


#include "MGFireball.h"
#include "MGGame.h"


using namespace C4;


namespace
{
	const float kFireballSplashDamageRadius = 5.0F;
}


FireballExplosionParticleSystem::FireballExplosionParticleSystem() :
		TexcoordPointParticleSystem(kParticleSystemFireballExplosion, &particlePool, "particle/Puff1"),
		particlePool(kMaxParticleCount, particleArray)
{
}

FireballExplosionParticleSystem::~FireballExplosionParticleSystem()
{
}

bool FireballExplosionParticleSystem::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	sphere->SetCenter(0.0F, 0.0F, 0.0F);
	sphere->SetRadius(16.0F);
	return (true);
}

void FireballExplosionParticleSystem::Preprocess(void)
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

void FireballExplosionParticleSystem::AnimateParticles(void)
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


FireballSparksParticleSystem::FireballSparksParticleSystem() :
		LineParticleSystem(kParticleSystemFireballSparks, &particlePool, "particle/Spark1"),
		particlePool(kMaxParticleCount, particleArray)
{
	SetLengthMultiplier(8.0F);
}

FireballSparksParticleSystem::~FireballSparksParticleSystem()
{
}

bool FireballSparksParticleSystem::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	sphere->SetCenter(0.0F, 0.0F, 0.0F);
	sphere->SetRadius(10.0F);
	return (true);
}

void FireballSparksParticleSystem::Preprocess(void)
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

void FireballSparksParticleSystem::AnimateParticles(void)
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


FireballController::FireballController() : ProjectileController(kControllerFireball)
{
	smokeTrail = nullptr;
}

FireballController::FireballController(const Vector3D& velocity, float smoke, GameCharacterController *attacker) : ProjectileController(kControllerFireball, velocity, attacker)
{
	lifeTime = 10000;
	smokeMultiplier = smoke;

	smokeDistance = 0.0F;
	smokeTrail = nullptr;

	SetCollisionKind(GetCollisionKind() | kCollisionFireball);
}

FireballController::FireballController(const FireballController& fireballController) : ProjectileController(fireballController)
{
	lifeTime = fireballController.lifeTime;
	smokeMultiplier = fireballController.smokeMultiplier;

	smokeDistance = 0.0F;
	smokeTrail = nullptr;
}

FireballController::~FireballController()
{
	if (smokeTrail)
	{
		smokeTrail->Finalize();
	}
}

Controller *FireballController::Replicate(void) const
{
	return (new FireballController(*this));
}

void FireballController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	ProjectileController::Pack(data, packFlags);

	data << ChunkHeader('LIFE', 4);
	data << lifeTime;

	data << ChunkHeader('SMOK', 8);
	data << smokeMultiplier;
	data << smokeDistance;

	if ((smokeTrail) && (smokeTrail->LinkedNodePackable(packFlags)))
	{
		data << ChunkHeader('TRAL', 4);
		data << smokeTrail->GetNodeIndex();
	}

	data << TerminatorChunk;
}

void FireballController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	ProjectileController::Unpack(data, unpackFlags);
	UnpackChunkList<FireballController>(data, unpackFlags);
}

bool FireballController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'LIFE':

			data >> lifeTime;
			return (true);

		case 'SMOK':

			data >> smokeMultiplier;
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

void FireballController::SmokeTrailLinkProc(Node *node, void *cookie)
{
	FireballController *controller = static_cast<FireballController *>(cookie);
	controller->smokeTrail = static_cast<SmokeTrailParticleSystem *>(node);
}

void FireballController::Preprocess(void)
{
	ProjectileController::Preprocess();

	SetRigidBodyFlags(kRigidBodyKeepAwake | kRigidBodyFixedOrientation | kRigidBodyDisabledContact);
	teleportFlag = false;

	Model *model = GetTargetNode();
	if (!model->GetManipulator())
	{
		Node *node = model->GetFirstSubnode();
		while (node)
		{
			NodeType type = node->GetNodeType();
			if (type == kNodeLight)
			{
				static_cast<Light *>(node)->SetExcludedNode(model);
			}
			else if (type == kNodeSource)
			{
				fireballSource = static_cast<OmniSource *>(node);
				fireballSource->SetSourceVelocity(GetLinearVelocity());
			}

			node = node->Next();
		}

		if (!smokeTrail)
		{
			smokeTrail = new SmokeTrailParticleSystem(ColorRGBA(0.0F, 0.0F, 0.0F, 1.0F));
			model->GetOwningZone()->AppendNewSubnode(smokeTrail);
		}
	}
}

ControllerMessage *FireballController::CreateMessage(ControllerMessageType type) const
{
	switch (type)
	{
		case kFireballMessageTeleport:

			return (new FireballTeleportMessage(GetControllerIndex()));

		case kFireballMessageExplode:

			return (new FireballExplodeMessage(GetControllerIndex()));
	}

	return (ProjectileController::CreateMessage(type));
}

void FireballController::SendInitialStateMessages(Player *player) const
{
	player->SendMessage(CreateFireballMessage(GetControllerIndex(), GetAttackerIndex(), GetTargetNode()->GetWorldPosition(), GetLinearVelocity(), smokeMultiplier));
}

void FireballController::Move(void)
{
	if (TheMessageMgr->Server())
	{
		if ((lifeTime -= TheTimeMgr->GetDeltaTime()) <= 0)
		{
			FireballController::Destroy(GetTargetNode()->GetWorldPosition(), Normalize(-GetLinearVelocity()));
			return;
		}
	}

	Vector3D direction = GetLinearVelocity();
	SetFireballSourceVelocity(direction);

	float m = SquaredMag(direction);
	float r = InverseSqrt(m);
	direction *= r;

	Vector3D z = -direction;
	Vector3D x = Math::CreateUnitPerpendicular(z);
	SetRigidBodyMatrix3D(Transform(GetTargetNode()->GetSuperNode()->GetInverseWorldTransform(), Matrix3D(x, z % x, z)));

	if (!teleportFlag)
	{
		float d = smokeDistance + m * r * (TheTimeMgr->GetDeltaSeconds() * smokeMultiplier);
		if (d >= 0.25F)
		{
			const Point3D& position = GetFinalTransform().GetTranslation();

			float count = PositiveFloor(d * 4.0F);
			smokeDistance = d - count * 0.25F;
			count = Fmin(count, 8.0F);
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

Transform4D FireballController::CalculateFireballTransform(const Point3D& position, const Vector3D& velocity)
{
	Vector3D zdir = -velocity * InverseMag(velocity);
	Vector3D xdir = Math::CreateUnitPerpendicular(zdir);
	return (Transform4D(xdir, zdir % xdir, zdir, position));
}

RigidBodyStatus FireballController::HandleNewRigidBodyContact(const RigidBodyContact *contact, RigidBodyController *contactBody)
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

		FireballController::Destroy(worldPosition, -worldNormal);
		return (kRigidBodyDestroyed);
	}

	return (kRigidBodyUnchanged);
}

RigidBodyStatus FireballController::HandleNewGeometryContact(const GeometryContact *contact)
{
	if (TheMessageMgr->Server())
	{
		const Transform4D& worldTransform = GetFinalTransform();
		Vector3D worldNormal = -(worldTransform * contact->GetRigidBodyContactNormal());

		const Geometry *geometry = contact->GetContactGeometry();

		const RemotePortal *portal = Game::DetectTeleporter(geometry);
		if (portal)
		{
			// Fireball goes through a teleporting portal.

			Transform4D		portalTransform;

			portal->CalculateRemoteTransform(&portalTransform);

			Point3D position = portalTransform * worldTransform.GetTranslation();
			Vector3D velocity = portalTransform * GetOriginalLinearVelocity();
			Point3D center = worldTransform * contact->GetRigidBodyContactPosition() + worldNormal;

			TheMessageMgr->SendMessageAll(FireballTeleportMessage(GetControllerIndex(), position, velocity, center));
			return (kRigidBodyContactsBroken);
		}

		// The splash damage will not cause the contact to be deleted in this case.

		const Point3D& position = contact->GetGeometryContactPosition();
		Game::ProcessGeometryProperties(geometry, position, GetAttackerNode());

		FireballController::Destroy(geometry->GetWorldTransform() * position, worldNormal);
		return (kRigidBodyDestroyed);
	}

	return (kRigidBodyUnchanged);
}

void FireballController::Destroy(const Point3D& position, const Vector3D& normal)
{
	GameWorld *world = static_cast<GameWorld *>(GetTargetNode()->GetWorld());
	world->ProduceSplashDamage(position, kFireballSplashDamageRadius, 25, GetAttackerController());

	TheMessageMgr->SendMessageAll(FireballExplodeMessage(GetControllerIndex(), position, normal));
}


CreateFireballMessage::CreateFireballMessage() : CreateModelMessage(kModelMessageFireball)
{
}

CreateFireballMessage::CreateFireballMessage(int32 projectileIndex, int32 characterIndex, const Point3D& position, const Vector3D& velocity, float smoke) : CreateModelMessage(kModelMessageFireball, projectileIndex, position)
{
	initialVelocity = velocity;
	smokeMultiplier = smoke;
	attackerIndex = characterIndex;
}

CreateFireballMessage::~CreateFireballMessage()
{
}

void CreateFireballMessage::Compress(Compressor& data) const
{
	CreateModelMessage::Compress(data);

	data << initialVelocity;
	data << smokeMultiplier;
	data << attackerIndex;
}

bool CreateFireballMessage::Decompress(Decompressor& data)
{
	if (CreateModelMessage::Decompress(data))
	{
		data >> initialVelocity;
		data >> smokeMultiplier;
		data >> attackerIndex;
		return (true);
	}

	return (false);
}

bool CreateFireballMessage::HandleMessage(Player *sender) const
{
	GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
	if (world)
	{
		Controller *controller = new FireballController(initialVelocity, smokeMultiplier, static_cast<GameCharacterController *>(world->GetController(attackerIndex)));
		InitializeModel(world, Model::Get(kModelFireball), controller);
	}

	return (true);
}


FireballTeleportMessage::FireballTeleportMessage(int32 controllerIndex) : ControllerMessage(FireballController::kFireballMessageTeleport, controllerIndex)
{
}

FireballTeleportMessage::FireballTeleportMessage(int32 controllerIndex, const Point3D& position, const Vector3D& velocity, const Point3D& center) : ControllerMessage(FireballController::kFireballMessageTeleport, controllerIndex)
{
	teleportPosition = position;
	teleportVelocity = velocity;
	effectCenter = center;
}

FireballTeleportMessage::~FireballTeleportMessage()
{
}

void FireballTeleportMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << teleportPosition;
	data << teleportVelocity;
	data << effectCenter;
}

bool FireballTeleportMessage::Decompress(Decompressor& data)
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

void FireballTeleportMessage::HandleControllerMessage(Controller *controller) const
{
	FireballController *fireball = static_cast<FireballController *>(controller);

	Transform4D teleportTransform = FireballController::CalculateFireballTransform(teleportPosition, teleportVelocity);
	fireball->Teleport(effectCenter, teleportTransform, teleportVelocity);
	fireball->SetFireballSourceVelocity(teleportVelocity);
	fireball->SetTeleportFlag();
}


FireballExplodeMessage::FireballExplodeMessage(int32 controllerIndex) : ControllerMessage(FireballController::kFireballMessageExplode, controllerIndex)
{
}

FireballExplodeMessage::FireballExplodeMessage(int32 controllerIndex, const Point3D& position, const Vector3D& normal) : ControllerMessage(FireballController::kFireballMessageExplode, controllerIndex)
{
	explodePosition = position;
	explodeNormal = normal;
}

FireballExplodeMessage::~FireballExplodeMessage()
{
}

void FireballExplodeMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << explodePosition;
	data << explodeNormal;
}

bool FireballExplodeMessage::Decompress(Decompressor& data)
{
	if (ControllerMessage::Decompress(data))
	{
		data >> explodePosition;
		data >> explodeNormal;
		return (true);
	}

	return (false);
}

void FireballExplodeMessage::HandleControllerMessage(Controller *controller) const
{
	static const char explodeName[2][32] =
	{
		"fireball/Explode1", "fireball/Explode2"
	};

	MarkingData		markingData;

	Node *node = controller->GetTargetNode();
	World *world = node->GetWorld();

	OmniSource *source = new OmniSource(explodeName[Math::Random(2)], 256.0F);
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

	FireballExplosionParticleSystem *fireballExplosion = new FireballExplosionParticleSystem;
	fireballExplosion->SetNodePosition(explodePosition);
	world->AddNewNode(fireballExplosion);

	FireballSparksParticleSystem *fireballSparks = new FireballSparksParticleSystem;
	fireballSparks->SetNodePosition(explodePosition);
	world->AddNewNode(fireballSparks);

	ShockwaveEffect *shockwave = new ShockwaveEffect("effects/Shock", 16.0F, 8.0F, 0.03125F);
	shockwave->SetNodePosition(explodePosition);
	world->AddNewNode(shockwave);

	SphereField *field = new SphereField(Vector3D(6.0F, 6.0F, 6.0F));
	field->SetNodePosition(explodePosition);
	field->SetForce(new ExplosionForce(50.0F, kCollisionRemains | kCollisionFireball | kCollisionShot));
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

	static_cast<GameWorld *>(world)->ShakeCamera(explodePosition, 0.04F, 1000);

	delete node;
}

// ZYUQURM
