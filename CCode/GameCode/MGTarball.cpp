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


#include "MGTarball.h"
#include "MGGame.h"


using namespace C4;


namespace
{
	const float kTarballSplashDamageRadius = 5.0F;
}


TarballTrailParticleSystem::TarballTrailParticleSystem() :
		PointParticleSystem(kParticleSystemTarballTrail, &particlePool, "texture/Trail"),
		particlePool(kMaxParticleCount, particleArray)
{
}

TarballTrailParticleSystem::~TarballTrailParticleSystem()
{
}

void TarballTrailParticleSystem::Preprocess(void)
{
	PointParticleSystem::Preprocess();
	SetAmbientBlendState(kBlendInterpolate);
}

void TarballTrailParticleSystem::AnimateParticles(void)
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

void TarballTrailParticleSystem::NewParticle(const Point3D& position)
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


TarballExplosionParticleSystem::TarballExplosionParticleSystem() :
		BlobParticleSystem(kParticleSystemTarballExplosion, &particlePool),
		particlePool(kMaxParticleCount, particleArray)
{
	gooSplash = nullptr;
}

TarballExplosionParticleSystem::TarballExplosionParticleSystem(const Vector3D& velocity, const Vector3D& normal) :
		BlobParticleSystem(kParticleSystemTarballExplosion, &particlePool, 0.125F, 0.5F),
		particlePool(kMaxParticleCount, particleArray)
{
	explosionVelocity = velocity * 0.001F;
	explosionNormal = normal;

	gooSplash = nullptr;
}

TarballExplosionParticleSystem::~TarballExplosionParticleSystem()
{
	if (gooSplash)
	{
		gooSplash->Finalize();
	}
}

void TarballExplosionParticleSystem::Pack(Packer& data, unsigned_int32 packFlags) const 
{
	BlobParticleSystem::Pack(data, packFlags);
 
	if ((gooSplash) && (gooSplash->LinkedNodePackable(packFlags)))
	{ 
		data << ChunkHeader('GSPL', 4);
		data << gooSplash->GetNodeIndex();
	} 

	data << TerminatorChunk; 
} 

void TarballExplosionParticleSystem::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	BlobParticleSystem::Unpack(data, unpackFlags); 
	UnpackChunkList<TarballExplosionParticleSystem>(data, unpackFlags);
}

bool TarballExplosionParticleSystem::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
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

void TarballExplosionParticleSystem::GooSplashLinkProc(Node *node, void *cookie)
{
	TarballExplosionParticleSystem *tarballExplosion = static_cast<TarballExplosionParticleSystem *>(cookie);
	tarballExplosion->gooSplash = static_cast<GooSplashParticleSystem *>(node);
}

void TarballExplosionParticleSystem::Preprocess(void)
{
	SetMaterialObject(AutoReleaseMaterial(kMaterialTarball));
	splatterColor = static_cast<const MaterialData *>(MaterialObject::FindMaterialRegistration(kMaterialTarball)->GetMaterialCookie())->materialColor;

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

void TarballExplosionParticleSystem::AnimateParticles(void)
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


Tarball::Tarball() :
		BlobParticleSystem(kParticleSystemTarball, &particlePool, 0.05F, 0.3F),
		particlePool(kMaxParticleCount, particleArray)
{
}

Tarball::Tarball(const Tarball& tarball) :
		BlobParticleSystem(tarball, &particlePool),
		particlePool(kMaxParticleCount, particleArray)
{
}

Tarball::~Tarball()
{
}

Node *Tarball::Replicate(void) const
{
	return (new Tarball(*this));
}

void Tarball::Preprocess(void)
{
	BlobParticleSystem::Preprocess();

	if ((!GetManipulator()) && (!GetFirstParticle()))
	{
		for (machine a = 0; a < kMaxParticleCount; a++)
		{
			TarballParticle *particle = particlePool.NewParticle();
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

void Tarball::AnimateParticles(void)
{
	Vector2D cs = CosSin(TheTimeMgr->GetFloatDeltaTime() * 0.002F);
	const Point3D& position = GetWorldPosition();

	TarballParticle *particle = static_cast<TarballParticle *>(GetFirstParticle());
	while (particle)
	{
		particle->position = position + particle->velocity;
		particle->scaleAxis = Transform(particle->scaleAxis, Quaternion(particle->rotationAxis * cs.x, cs.y));

		particle = particle->GetNextParticle();
	}
}


TarballController::TarballController() : ProjectileController(kControllerTarball)
{
	tarballTrail = nullptr;
}

TarballController::TarballController(const Vector3D& velocity, GameCharacterController *attacker) : ProjectileController(kControllerTarball, velocity, attacker)
{
	lifeTime = 10000;

	trailDistance = 0.0F;
	tarballTrail = nullptr;
}

TarballController::TarballController(const TarballController& tarballController) : ProjectileController(tarballController)
{
	lifeTime = tarballController.lifeTime;

	trailDistance = 0.0F;
	tarballTrail = nullptr;
}

TarballController::~TarballController()
{
	if (tarballTrail)
	{
		tarballTrail->Finalize();
	}
}

Controller *TarballController::Replicate(void) const
{
	return (new TarballController(*this));
}

void TarballController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	ProjectileController::Pack(data, packFlags);

	data << ChunkHeader('LIFE', 4);
	data << lifeTime;

	data << ChunkHeader('TDST', 4);
	data << trailDistance;

	if ((tarballTrail) && (tarballTrail->LinkedNodePackable(packFlags)))
	{
		data << ChunkHeader('TRAL', 4);
		data << tarballTrail->GetNodeIndex();
	}

	data << TerminatorChunk;
}

void TarballController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	ProjectileController::Unpack(data, unpackFlags);
	UnpackChunkList<TarballController>(data, unpackFlags);
}

bool TarballController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
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
			data.AddNodeLink(nodeIndex, &TarballTrailLinkProc, this);
			return (true);
		}
	}

	return (false);
}

void TarballController::TarballTrailLinkProc(Node *node, void *cookie)
{
	TarballController *controller = static_cast<TarballController *>(cookie);
	controller->tarballTrail = static_cast<TarballTrailParticleSystem *>(node);
}

void TarballController::Preprocess(void)
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
				tarballSource = static_cast<OmniSource *>(node);
				tarballSource->SetSourceVelocity(GetLinearVelocity());
				break;
			}

			node = node->Next();
		}

		if (!tarballTrail)
		{
			tarballTrail = new TarballTrailParticleSystem;
			model->GetOwningZone()->AppendNewSubnode(tarballTrail);
		}
	}
}

ControllerMessage *TarballController::CreateMessage(ControllerMessageType type) const
{
	switch (type)
	{
		case kTarballMessageTeleport:

			return (new TarballTeleportMessage(GetControllerIndex()));

		case kTarballMessageExplode:

			return (new TarballExplodeMessage(GetControllerIndex()));
	}

	return (ProjectileController::CreateMessage(type));
}

void TarballController::SendInitialStateMessages(Player *player) const
{
	player->SendMessage(CreateTarballMessage(GetControllerIndex(), GetAttackerIndex(), GetTargetNode()->GetWorldPosition(), GetLinearVelocity()));
}

void TarballController::Move(void)
{
	if (TheMessageMgr->Server())
	{
		if ((lifeTime -= TheTimeMgr->GetDeltaTime()) <= 0)
		{
			TarballController::Destroy(GetTargetNode()->GetWorldPosition(), Zero3D);
			return;
		}
	}

	Vector3D direction = GetLinearVelocity();
	SetTarballSourceVelocity(direction);

	float fdt = TheTimeMgr->GetFloatDeltaTime();

	if (direction.z < 0.0F)
	{
		tarballSource->SetSourceFrequency(tarballSource->GetSourceFrequency() * Exp(fdt * -2.0e-4F));
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
				tarballTrail->NewParticle(position - direction * f);
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

RigidBodyStatus TarballController::HandleNewRigidBodyContact(const RigidBodyContact *contact, RigidBodyController *contactBody)
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

		TarballController::Destroy(worldPosition, -worldNormal);
		return (kRigidBodyDestroyed);
	}

	return (kRigidBodyUnchanged);
}

RigidBodyStatus TarballController::HandleNewGeometryContact(const GeometryContact *contact)
{
	if (TheMessageMgr->Server())
	{
		const Transform4D& worldTransform = GetFinalTransform();
		Vector3D worldNormal = -(worldTransform * contact->GetRigidBodyContactNormal());

		const Geometry *geometry = contact->GetContactGeometry();

		const RemotePortal *portal = Game::DetectTeleporter(geometry);
		if (portal)
		{
			// Tarball goes through a teleporting portal.

			Transform4D		portalTransform;

			portal->CalculateRemoteTransform(&portalTransform);

			Point3D position = portalTransform * worldTransform.GetTranslation();
			Vector3D velocity = portalTransform * GetOriginalLinearVelocity();
			Point3D center = worldTransform * contact->GetRigidBodyContactPosition() + worldNormal;

			TheMessageMgr->SendMessageAll(TarballTeleportMessage(GetControllerIndex(), position, velocity, center));
			return (kRigidBodyContactsBroken);
		}

		// The splash damage will not cause the contact to be deleted in this case.

		const Point3D& position = contact->GetGeometryContactPosition();
		Game::ProcessGeometryProperties(geometry, position, GetAttackerNode());

		TarballController::Destroy(geometry->GetWorldTransform() * position, worldNormal);
		return (kRigidBodyDestroyed);
	}

	return (kRigidBodyUnchanged);
}

void TarballController::Destroy(const Point3D& position, const Vector3D& normal)
{
	GameWorld *world = static_cast<GameWorld *>(GetTargetNode()->GetWorld());
	world->ProduceSplashDamage(position, kTarballSplashDamageRadius, 25, GetAttackerController());

	TheMessageMgr->SendMessageAll(TarballExplodeMessage(GetControllerIndex(), position, normal));
}


CreateTarballMessage::CreateTarballMessage() : CreateModelMessage(kModelMessageTarball)
{
}

CreateTarballMessage::CreateTarballMessage(int32 projectileIndex, int32 characterIndex, const Point3D& position, const Vector3D& velocity) : CreateModelMessage(kModelMessageTarball, projectileIndex, position)
{
	initialVelocity = velocity;
	attackerIndex = characterIndex;
}

CreateTarballMessage::~CreateTarballMessage()
{
}

void CreateTarballMessage::Compress(Compressor& data) const
{
	CreateModelMessage::Compress(data);

	data << initialVelocity;
	data << attackerIndex;
}

bool CreateTarballMessage::Decompress(Decompressor& data)
{
	if (CreateModelMessage::Decompress(data))
	{
		data >> initialVelocity;
		data >> attackerIndex;
		return (true);
	}

	return (false);
}

bool CreateTarballMessage::HandleMessage(Player *sender) const
{
	GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
	if (world)
	{
		Controller *controller = new TarballController(initialVelocity, static_cast<GameCharacterController *>(world->GetController(attackerIndex)));
		InitializeModel(world, Model::Get(kModelTarball), controller);
	}

	return (true);
}


TarballTeleportMessage::TarballTeleportMessage(int32 controllerIndex) : ControllerMessage(TarballController::kTarballMessageTeleport, controllerIndex)
{
}

TarballTeleportMessage::TarballTeleportMessage(int32 controllerIndex, const Point3D& position, const Vector3D& velocity, const Point3D& center) : ControllerMessage(TarballController::kTarballMessageTeleport, controllerIndex)
{
	teleportPosition = position;
	teleportVelocity = velocity;
	effectCenter = center;
}

TarballTeleportMessage::~TarballTeleportMessage()
{
}

void TarballTeleportMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << teleportPosition;
	data << teleportVelocity;
	data << effectCenter;
}

bool TarballTeleportMessage::Decompress(Decompressor& data)
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

void TarballTeleportMessage::HandleControllerMessage(Controller *controller) const
{
	TarballController *tarball = static_cast<TarballController *>(controller);

	tarball->Teleport(effectCenter, Transform4D(K::x_unit, K::y_unit, K::z_unit, teleportPosition), teleportVelocity);
	tarball->SetTarballSourceVelocity(teleportVelocity);
	tarball->SetTeleportFlag();
}


TarballExplodeMessage::TarballExplodeMessage(int32 controllerIndex) : ControllerMessage(TarballController::kTarballMessageExplode, controllerIndex)
{
}

TarballExplodeMessage::TarballExplodeMessage(int32 controllerIndex, const Point3D& position, const Vector3D& normal) : ControllerMessage(TarballController::kTarballMessageExplode, controllerIndex)
{
	explodePosition = position;
	explodeNormal = normal;
}

TarballExplodeMessage::~TarballExplodeMessage()
{
}

void TarballExplodeMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << explodePosition;
	data << explodeNormal;
}

bool TarballExplodeMessage::Decompress(Decompressor& data)
{
	if (ControllerMessage::Decompress(data))
	{
		data >> explodePosition;
		data >> explodeNormal;
		return (true);
	}

	return (false);
}

void TarballExplodeMessage::HandleControllerMessage(Controller *controller) const
{
	Node *node = controller->GetTargetNode();

	const RigidBodyController *rigidBody = static_cast<RigidBodyController *>(controller);
	TarballExplosionParticleSystem *tarballExplosion = new TarballExplosionParticleSystem(rigidBody->GetLinearVelocity(), explodeNormal);
	tarballExplosion->SetNodePosition(explodePosition);
	node->GetSuperNode()->AppendNewSubnode(tarballExplosion);

	delete node;
}

// ZYUQURM
