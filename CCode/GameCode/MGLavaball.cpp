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


#include "MGLavaball.h"
#include "MGGame.h"


using namespace C4;


namespace
{
	const float kLavaballSplashDamageRadius = 7.0F;
}


LavaballTrailParticleSystem::LavaballTrailParticleSystem() :
		PointParticleSystem(kParticleSystemLavaballTrail, &particlePool, "texture/Trail"),
		particlePool(kMaxParticleCount, particleArray)
{
}

LavaballTrailParticleSystem::~LavaballTrailParticleSystem()
{
}

void LavaballTrailParticleSystem::Preprocess(void)
{
	PointParticleSystem::Preprocess();
	SetAmbientBlendState(kBlendInterpolate);
}

void LavaballTrailParticleSystem::AnimateParticles(void)
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

			particle->radius += fdt * 0.002F;
		}
		else
		{
			FreeParticle(particle);
		}

		particle = next;
	}
}

void LavaballTrailParticleSystem::NewParticle(const Point3D& position)
{
	Particle *particle = particlePool.NewParticle();
	if (particle)
	{
		particle->emitTime = 0;
		particle->lifeTime = Math::Random(200) + 400;
		particle->radius = 0.25F;
		particle->color.Set(1.0F, 1.0F, 1.0F, 1.0F);
		particle->orientation = Math::Random(256) << 24;
		particle->position = position + Math::RandomUnitVector3D() * Math::RandomFloat(0.5F);

		AddParticle(particle);
	}
}


LavaballExplosionParticleSystem::LavaballExplosionParticleSystem() :
		BlobParticleSystem(kParticleSystemLavaballExplosion, &particlePool),
		particlePool(kMaxParticleCount, particleArray)
{
	gooSplash = nullptr;
}

LavaballExplosionParticleSystem::LavaballExplosionParticleSystem(const Vector3D& velocity, const Vector3D& normal) :
		BlobParticleSystem(kParticleSystemLavaballExplosion, &particlePool, 0.125F, 0.5F),
		particlePool(kMaxParticleCount, particleArray)
{
	explosionVelocity = velocity * 0.001F;
	explosionNormal = normal;

	gooSplash = nullptr;
}

LavaballExplosionParticleSystem::~LavaballExplosionParticleSystem()
{
	if (gooSplash)
	{
		gooSplash->Finalize();
	}
}

void LavaballExplosionParticleSystem::Pack(Packer& data, unsigned_int32 packFlags) const 
{
	BlobParticleSystem::Pack(data, packFlags);
 
	if ((gooSplash) && (gooSplash->LinkedNodePackable(packFlags)))
	{ 
		data << ChunkHeader('GSPL', 4);
		data << gooSplash->GetNodeIndex();
	} 

	data << TerminatorChunk; 
} 

void LavaballExplosionParticleSystem::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	BlobParticleSystem::Unpack(data, unpackFlags); 
	UnpackChunkList<LavaballExplosionParticleSystem>(data, unpackFlags);
}

bool LavaballExplosionParticleSystem::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'GSPL':
		{
			int32	nodeIndex;

			data >> nodeIndex;
			data.AddNodeLink(nodeIndex, &GooSplashLinkProc, this);
			return (true);
		}
	}

	return (false);
}

void LavaballExplosionParticleSystem::GooSplashLinkProc(Node *node, void *cookie)
{
	LavaballExplosionParticleSystem *lavaballExplosion = static_cast<LavaballExplosionParticleSystem *>(cookie);
	lavaballExplosion->gooSplash = static_cast<GooSplashParticleSystem *>(node);
}

void LavaballExplosionParticleSystem::Preprocess(void)
{
	SetMaterialObject(AutoReleaseMaterial(kMaterialLavaball));
	splatterColor = static_cast<const MaterialData *>(MaterialObject::FindMaterialRegistration(kMaterialLavaball)->GetMaterialCookie())->materialColor;

	BlobParticleSystem::Preprocess();
	SetParticleSystemFlags(kParticleSystemSelfDestruct);

	if (!GetFirstParticle())
	{
		const Point3D& position = GetWorldPosition();

		const Vector3D& normal = explosionNormal;
		if (SquaredMag(normal) > 0.25F)
		{
			Vector3D tangent = Math::CreateUnitPerpendicular(explosionNormal);
			Vector3D bitangent = normal % tangent;

			for (machine a = 0; a < kMaxParticleCount; a++)
			{
				BlobParticle *particle = particlePool.NewParticle();

				Vector3D direction = Math::RandomUnitVector3D() * (Math::RandomFloat(0.006F) + 0.006F);
				particle->velocity = tangent * direction.x + bitangent * direction.y + normal * Fabs(direction.z);
				particle->position = position + particle->velocity;
				particle->lifeTime = 5000;
				particle->radius = Math::RandomFloat(0.3F) + 0.2F;
				particle->scaleAxis = Math::RandomUnitVector3D();
				particle->inverseScale = Math::RandomFloat(0.5F) + 1.0F;
				AddParticle(particle);
			}
		}
		else
		{
			for (machine a = 0; a < kMaxParticleCount; a++)
			{
				BlobParticle *particle = particlePool.NewParticle();

				particle->velocity = explosionVelocity + Math::RandomUnitVector3D() * (Math::RandomFloat(0.004F) + 0.002F);
				particle->position = position + particle->velocity;
				particle->lifeTime = 5000;
				particle->radius = Math::RandomFloat(0.3F) + 0.2F;
				particle->scaleAxis = Math::RandomUnitVector3D();
				particle->inverseScale = Math::RandomFloat(0.5F) + 1.0F;
				AddParticle(particle);
			}
		}

		gooSplash = new GooSplashParticleSystem(splatterColor);
		gooSplash->SetNodePosition(position);
		GetWorld()->AddNewNode(gooSplash);

		gooSplash->AddSplash(position, Max(50 - TheGame->GetGameDetailLevel() * 10, 10), 3.0F);
	}
}

void LavaballExplosionParticleSystem::AnimateParticles(void)
{
	int32 dt = TheTimeMgr->GetDeltaTime();
	float fdt = TheTimeMgr->GetFloatDeltaTime();

	World *world = GetWorld();
	int32 detailLevel = TheGame->GetGameDetailLevel() + 2;

	BlobParticle *particle = static_cast<BlobParticle *>(GetFirstParticle());
	while (particle)
	{
		BlobParticle *next = particle->GetNextParticle();

		int32 life = (particle->lifeTime -= dt);
		if (life > 0)
		{
			CollisionData	collisionData;

			particle->velocity.z += K::gravity * fdt;
			Point3D newPosition = particle->position + particle->velocity * fdt;
			if (world->DetectCollision(particle->position, newPosition, 0.0F, kCollisionRemains, &collisionData))
			{
				if (Math::Random(detailLevel) == 0)
				{
					static const char soundName[2][16] =
					{
						"sound/GooSplat1", "sound/GooSplat2"
					};

					OmniSource *source = new OmniSource(soundName[Math::Random(2)], 64.0F);
					source->GetObject()->SetInitialSourceFrequency(Math::RandomFloat(0.5F) + 0.5F);
					source->SetNodePosition(collisionData.position);
					GetWorld()->AddNewNode(source);

					int32 i = Math::Random(4);
					MarkingData markingData(Vector2D(0.5F, 0.5F), Vector2D((i & 1) ? 0.5F : 0.0F, (i & 2) ? 0.5F : 0.0F), kMarkingLight | kMarkingDepthWrite | kMarkingForceClip);
					AutoReleaseMaterial material(kMaterialSplatter);

					markingData.center = collisionData.position;
					markingData.normal = collisionData.normal;
					markingData.tangent = Math::CreatePerpendicular(markingData.normal);
					markingData.tangent.RotateAboutAxis(Math::RandomFloat(K::tau), markingData.normal);
					markingData.radius = Math::RandomFloat(0.5F) + 0.5F;
					markingData.materialObject = material;
					markingData.color = splatterColor;
					markingData.lifeTime = 30000;
					MarkingEffect::New(world, &markingData);
				}

				gooSplash->AddSplash(collisionData.position, 20, 2.0F);

				FreeParticle(particle);
				goto nextParticle;
			}

			particle->position = newPosition;
		}
		else
		{
			FreeParticle(particle);
		}

		nextParticle:
		particle = next;
	}
}


Lavaball::Lavaball() :
		BlobParticleSystem(kParticleSystemLavaball, &particlePool, 0.05F, 0.3F),
		particlePool(kMaxParticleCount, particleArray)
{
}

Lavaball::Lavaball(const Lavaball& lavaball) :
		BlobParticleSystem(lavaball, &particlePool),
		particlePool(kMaxParticleCount, particleArray)
{
}

Lavaball::~Lavaball()
{
}

Node *Lavaball::Replicate(void) const
{
	return (new Lavaball(*this));
}

void Lavaball::Preprocess(void)
{
	BlobParticleSystem::Preprocess();

	if ((!GetManipulator()) && (!GetFirstParticle()))
	{
		for (machine a = 0; a < kMaxParticleCount; a++)
		{
			LavaballParticle *particle = particlePool.NewParticle();
			AddParticle(particle);

			particle->emitTime = 0;
			particle->lifeTime = 1;
			particle->radius = 0.3F;
			particle->position.Set(0.0F, 0.0F, 0.0F);
			particle->velocity = Math::RandomUnitVector3D() * Math::RandomFloat(0.25F);
			particle->scaleAxis = Math::RandomUnitVector3D();
			particle->inverseScale = Math::RandomFloat(0.5F) + 1.0F;
			particle->rotationAxis = Math::RandomUnitVector3D();
		}
	}
}

void Lavaball::AnimateParticles(void)
{
	Vector2D cs = CosSin(TheTimeMgr->GetFloatDeltaTime() * 0.002F);
	const Point3D& position = GetWorldPosition();

	LavaballParticle *particle = static_cast<LavaballParticle *>(GetFirstParticle());
	while (particle)
	{
		particle->position = position + particle->velocity;
		particle->scaleAxis = Transform(particle->scaleAxis, Quaternion(particle->rotationAxis * cs.x, cs.y));

		particle = particle->GetNextParticle();
	}
}


LavaballController::LavaballController() : ProjectileController(kControllerLavaball)
{
	lavaballTrail = nullptr;
}

LavaballController::LavaballController(const Vector3D& velocity, GameCharacterController *attacker) : ProjectileController(kControllerLavaball, velocity, attacker)
{
	lifeTime = 10000;

	trailDistance = 0.0F;
	lavaballTrail = nullptr;
}

LavaballController::LavaballController(const LavaballController& lavaballController) : ProjectileController(lavaballController)
{
	lifeTime = lavaballController.lifeTime;

	trailDistance = 0.0F;
	lavaballTrail = nullptr;
}

LavaballController::~LavaballController()
{
	if (lavaballTrail)
	{
		lavaballTrail->Finalize();
	}
}

Controller *LavaballController::Replicate(void) const
{
	return (new LavaballController(*this));
}

void LavaballController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	ProjectileController::Pack(data, packFlags);

	data << ChunkHeader('LIFE', 4);
	data << lifeTime;

	data << ChunkHeader('TDST', 4);
	data << trailDistance;

	if ((lavaballTrail) && (lavaballTrail->LinkedNodePackable(packFlags)))
	{
		data << ChunkHeader('TRAL', 4);
		data << lavaballTrail->GetNodeIndex();
	}

	data << TerminatorChunk;
}

void LavaballController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	ProjectileController::Unpack(data, unpackFlags);
	UnpackChunkList<LavaballController>(data, unpackFlags);
}

bool LavaballController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'LIFE':

			data >> lifeTime;
			return (true);

		case 'TDST':

			data >> trailDistance;
			return (true);

		case 'TRAL':
		{
			int32	nodeIndex;

			data >> nodeIndex;
			data.AddNodeLink(nodeIndex, &LavaballTrailLinkProc, this);
			return (true);
		}
	}

	return (false);
}

void LavaballController::LavaballTrailLinkProc(Node *node, void *cookie)
{
	LavaballController *controller = static_cast<LavaballController *>(cookie);
	controller->lavaballTrail = static_cast<LavaballTrailParticleSystem *>(node);
}

void LavaballController::Preprocess(void)
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
			if (node->GetNodeType() == kNodeSource)
			{
				lavaballSource = static_cast<OmniSource *>(node);
				lavaballSource->SetSourceVelocity(GetLinearVelocity());
				break;
			}

			node = node->Next();
		}

		if (!lavaballTrail)
		{
			lavaballTrail = new LavaballTrailParticleSystem;
			model->GetOwningZone()->AppendNewSubnode(lavaballTrail);
		}
	}
}

ControllerMessage *LavaballController::CreateMessage(ControllerMessageType type) const
{
	switch (type)
	{
		case kLavaballMessageTeleport:

			return (new LavaballTeleportMessage(GetControllerIndex()));

		case kLavaballMessageExplode:

			return (new LavaballExplodeMessage(GetControllerIndex()));
	}

	return (ProjectileController::CreateMessage(type));
}

void LavaballController::SendInitialStateMessages(Player *player) const
{
	player->SendMessage(CreateLavaballMessage(GetControllerIndex(), GetAttackerIndex(), GetTargetNode()->GetWorldPosition(), GetLinearVelocity()));
}

void LavaballController::Move(void)
{
	if (TheMessageMgr->Server())
	{
		if ((lifeTime -= TheTimeMgr->GetDeltaTime()) <= 0)
		{
			LavaballController::Destroy(GetTargetNode()->GetWorldPosition(), Zero3D);
			return;
		}
	}

	Vector3D direction = GetLinearVelocity();
	SetLavaballSourceVelocity(direction);

	float fdt = TheTimeMgr->GetFloatDeltaTime();

	if (direction.z < 0.0F)
	{
		lavaballSource->SetSourceFrequency(lavaballSource->GetSourceFrequency() * Exp(fdt * -2.0e-4F));
	}

	if (!teleportFlag)
	{
		float m = SquaredMag(direction);
		float r = InverseSqrt(m);
		direction *= r;

		float d = trailDistance + m * r * (fdt * 0.001F);
		if (d >= 0.125F)
		{
			const Point3D& position = GetFinalTransform().GetTranslation();

			float count = PositiveFloor(d * 8.0F);
			trailDistance = d - count * 0.125F;
			count = Fmin(count, 16.0F);
			do
			{
				float f = d * Math::RandomFloat(1.0F) + 0.25F;
				lavaballTrail->NewParticle(position - direction * f);
			} while ((count -= 1.0F) > 0.0F);
		}
		else
		{
			trailDistance = d;
		}
	}
	else
	{
		teleportFlag = false;
	}
}

RigidBodyStatus LavaballController::HandleNewRigidBodyContact(const RigidBodyContact *contact, RigidBodyController *contactBody)
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

		LavaballController::Destroy(worldPosition, -worldNormal);
		return (kRigidBodyDestroyed);
	}

	return (kRigidBodyUnchanged);
}

RigidBodyStatus LavaballController::HandleNewGeometryContact(const GeometryContact *contact)
{
	if (TheMessageMgr->Server())
	{
		const Transform4D& worldTransform = GetFinalTransform();
		Vector3D worldNormal = -(worldTransform * contact->GetRigidBodyContactNormal());

		const Geometry *geometry = contact->GetContactGeometry();

		const RemotePortal *portal = Game::DetectTeleporter(geometry);
		if (portal)
		{
			// Lavaball goes through a teleporting portal.

			Transform4D		portalTransform;

			portal->CalculateRemoteTransform(&portalTransform);

			Point3D position = portalTransform * worldTransform.GetTranslation();
			Vector3D velocity = portalTransform * GetOriginalLinearVelocity();
			Point3D center = worldTransform * contact->GetRigidBodyContactPosition() + worldNormal;

			TheMessageMgr->SendMessageAll(LavaballTeleportMessage(GetControllerIndex(), position, velocity, center));
			return (kRigidBodyContactsBroken);
		}

		// The splash damage will not cause the contact to be deleted in this case.

		const Point3D& position = contact->GetGeometryContactPosition();
		Game::ProcessGeometryProperties(geometry, position, GetAttackerNode());

		LavaballController::Destroy(geometry->GetWorldTransform() * position, worldNormal);
		return (kRigidBodyDestroyed);
	}

	return (kRigidBodyUnchanged);
}

void LavaballController::Destroy(const Point3D& position, const Vector3D& normal)
{
	GameWorld *world = static_cast<GameWorld *>(GetTargetNode()->GetWorld());
	world->ProduceSplashDamage(position, kLavaballSplashDamageRadius, 35, GetAttackerController());

	TheMessageMgr->SendMessageAll(LavaballExplodeMessage(GetControllerIndex(), position, normal));
}


CreateLavaballMessage::CreateLavaballMessage() : CreateModelMessage(kModelMessageLavaball)
{
}

CreateLavaballMessage::CreateLavaballMessage(int32 projectileIndex, int32 characterIndex, const Point3D& position, const Vector3D& velocity) : CreateModelMessage(kModelMessageLavaball, projectileIndex, position)
{
	initialVelocity = velocity;
	attackerIndex = characterIndex;
}

CreateLavaballMessage::~CreateLavaballMessage()
{
}

void CreateLavaballMessage::Compress(Compressor& data) const
{
	CreateModelMessage::Compress(data);

	data << initialVelocity;
	data << attackerIndex;
}

bool CreateLavaballMessage::Decompress(Decompressor& data)
{
	if (CreateModelMessage::Decompress(data))
	{
		data >> initialVelocity;
		data >> attackerIndex;
		return (true);
	}

	return (false);
}

bool CreateLavaballMessage::HandleMessage(Player *sender) const
{
	GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
	if (world)
	{
		Controller *controller = new LavaballController(initialVelocity, static_cast<GameCharacterController *>(world->GetController(attackerIndex)));
		InitializeModel(world, Model::Get(kModelLavaball), controller);
	}

	return (true);
}


LavaballTeleportMessage::LavaballTeleportMessage(int32 controllerIndex) : ControllerMessage(LavaballController::kLavaballMessageTeleport, controllerIndex)
{
}

LavaballTeleportMessage::LavaballTeleportMessage(int32 controllerIndex, const Point3D& position, const Vector3D& velocity, const Point3D& center) : ControllerMessage(LavaballController::kLavaballMessageTeleport, controllerIndex)
{
	teleportPosition = position;
	teleportVelocity = velocity;
	effectCenter = center;
}

LavaballTeleportMessage::~LavaballTeleportMessage()
{
}

void LavaballTeleportMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << teleportPosition;
	data << teleportVelocity;
	data << effectCenter;
}

bool LavaballTeleportMessage::Decompress(Decompressor& data)
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

void LavaballTeleportMessage::HandleControllerMessage(Controller *controller) const
{
	LavaballController *lavaball = static_cast<LavaballController *>(controller);

	lavaball->Teleport(effectCenter, Transform4D(K::x_unit, K::y_unit, K::z_unit, teleportPosition), teleportVelocity);
	lavaball->SetLavaballSourceVelocity(teleportVelocity);
	lavaball->SetTeleportFlag();
}


LavaballExplodeMessage::LavaballExplodeMessage(int32 controllerIndex) : ControllerMessage(LavaballController::kLavaballMessageExplode, controllerIndex)
{
}

LavaballExplodeMessage::LavaballExplodeMessage(int32 controllerIndex, const Point3D& position, const Vector3D& normal) : ControllerMessage(LavaballController::kLavaballMessageExplode, controllerIndex)
{
	explodePosition = position;
	explodeNormal = normal;
}

LavaballExplodeMessage::~LavaballExplodeMessage()
{
}

void LavaballExplodeMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << explodePosition;
	data << explodeNormal;
}

bool LavaballExplodeMessage::Decompress(Decompressor& data)
{
	if (ControllerMessage::Decompress(data))
	{
		data >> explodePosition;
		data >> explodeNormal;
		return (true);
	}

	return (false);
}

void LavaballExplodeMessage::HandleControllerMessage(Controller *controller) const
{
	static const char explodeName[2][32] =
	{
		"fireball/Explode1", "fireball/Explode2"
	};

	Node *node = controller->GetTargetNode();
	World *world = node->GetWorld();

	OmniSource *source = new OmniSource(explodeName[Math::Random(2)], 256.0F);
	OmniSourceObject *sourceObject = source->GetObject();
	sourceObject->SetInitialSourceVolume(2.0F);
	sourceObject->SetReflectionVolume(0.5F);
	sourceObject->SetReflectionHFVolume(0.5F);
	source->SetSourcePriority(kSoundPriorityExplosion);
	source->SetNodePosition(explodePosition);
	world->AddNewNode(source);

	const RigidBodyController *rigidBody = static_cast<RigidBodyController *>(controller);
	LavaballExplosionParticleSystem *lavaballExplosion = new LavaballExplosionParticleSystem(rigidBody->GetLinearVelocity(), explodeNormal);
	lavaballExplosion->SetNodePosition(explodePosition);
	node->GetSuperNode()->AppendNewSubnode(lavaballExplosion);

	ShockwaveEffect *shockwave = new ShockwaveEffect("effects/Shock", 32.0F, 4.0F, 0.03125F);
	shockwave->SetNodePosition(explodePosition);
	world->AddNewNode(shockwave);

	static_cast<GameWorld *>(world)->ShakeCamera(explodePosition, 0.08F, 1500);

	delete node;
}

// ZYUQURM
