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


#include "MGVenom.h"
#include "MGGame.h"


using namespace C4;


VenomTrailParticleSystem::VenomTrailParticleSystem() :
		PointParticleSystem(kParticleSystemVenomTrail, &particlePool, "texture/Trail"),
		particlePool(kMaxParticleCount, particleArray)
{
}

VenomTrailParticleSystem::~VenomTrailParticleSystem()
{
}

void VenomTrailParticleSystem::Preprocess(void)
{
	PointParticleSystem::Preprocess();
	SetAmbientBlendState(kBlendInterpolate);
}

void VenomTrailParticleSystem::AnimateParticles(void)
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

			particle->velocity *= Exp(fdt * -0.005F);
			particle->position += particle->velocity * fdt;
		}
		else
		{
			FreeParticle(particle);
		}

		particle = next;
	}
}

void VenomTrailParticleSystem::NewParticle(const Point3D& position, const Vector3D& velocity)
{
	Particle *particle = particlePool.NewParticle();
	if (particle)
	{
		particle->emitTime = 0;
		particle->lifeTime = Math::Random(400) + 800;
		particle->radius = 0.125F;
		particle->color.Set(0.078125F, 0.5F, 0.078125F, 1.0F);
		particle->orientation = Math::Random(256) << 24;
		particle->position = position + Math::RandomUnitVector3D() * Math::RandomFloat(0.125F);
		particle->velocity = velocity * 0.0005F;
		particle->velocity.z -= 0.001F;

		AddParticle(particle);
	}
}


Venom::Venom() :
		BlobParticleSystem(kParticleSystemVenom, &particlePool, 0.02F, 0.1F),
		particlePool(kMaxParticleCount, particleArray)
{
}

Venom::Venom(const Venom& venom) :
		BlobParticleSystem(venom, &particlePool),
		particlePool(kMaxParticleCount, particleArray)
{
}

Venom::~Venom()
{
}

Node *Venom::Replicate(void) const
{
	return (new Venom(*this));
}

void Venom::Preprocess(void)
{
	BlobParticleSystem::Preprocess();

	SetTransformable(this);
	SetParticleSystemFlags(kParticleSystemObjectSpace);

	if ((!GetManipulator()) && (!GetFirstParticle())) 
	{
		BlobParticle *particle = particlePool.NewParticle();
		AddParticle(particle); 

		particle->emitTime = 0; 
		particle->lifeTime = 1;
		particle->radius = 0.08F;
		particle->position.Set(0.0F, 0.0F, 0.0F); 
		particle->velocity.Set(0.0F, 0.0F, 0.0F);
		particle->scaleAxis = Math::RandomUnitVector3D(); 
		particle->inverseScale = Math::RandomFloat(0.5F) + 1.0F; 

		for (machine a = 1; a < kMaxParticleCount; a++)
		{
			particle = particlePool.NewParticle(); 
			AddParticle(particle);

			particle->emitTime = 0;
			particle->lifeTime = 1;
			particle->radius = Math::RandomFloat(0.02F) + 0.02F;
			particle->position = Math::RandomUnitVector3D() * 0.1F;
			particle->velocity = Math::RandomUnitVector3D() * 0.0002F;
			particle->scaleAxis = Math::RandomUnitVector3D();
			particle->inverseScale = Math::RandomFloat(0.5F) + 1.0F;
		}
	}
}

void Venom::AnimateParticles(void)
{
	float fdt = TheTimeMgr->GetFloatDeltaTime();

	BlobParticle *particle = static_cast<BlobParticle *>(GetFirstParticle());
	while (particle)
	{
		particle->position += particle->velocity * fdt;
		particle = particle->GetNextParticle();
	}
}


VenomController::VenomController() : ProjectileController(kControllerVenom)
{
}

VenomController::VenomController(const Vector3D& velocity, GameCharacterController *attacker) : ProjectileController(kControllerVenom, velocity, attacker)
{
	lifeTime = 10000;

	trailDistance = 0.0F;
	venomTrail = nullptr;
}

VenomController::VenomController(const VenomController& venomController) : ProjectileController(venomController)
{
	lifeTime = venomController.lifeTime;

	trailDistance = 0.0F;
	venomTrail = nullptr;
}

VenomController::~VenomController()
{
	if (venomTrail)
	{
		venomTrail->Finalize();
	}
}

Controller *VenomController::Replicate(void) const
{
	return (new VenomController(*this));
}

void VenomController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	ProjectileController::Pack(data, packFlags);

	data << ChunkHeader('LIFE', 4);
	data << lifeTime;

	data << ChunkHeader('TDST', 4);
	data << trailDistance;

	if ((venomTrail) && (venomTrail->LinkedNodePackable(packFlags)))
	{
		data << ChunkHeader('TRAL', 4);
		data << venomTrail->GetNodeIndex();
	}

	data << TerminatorChunk;
}

void VenomController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	ProjectileController::Unpack(data, unpackFlags);
	UnpackChunkList<VenomController>(data, unpackFlags);
}

bool VenomController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
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
			data.AddNodeLink(nodeIndex, &VenomTrailLinkProc, this);
			return (true);
		}
	}

	return (false);
}

void VenomController::VenomTrailLinkProc(Node *node, void *cookie)
{
	VenomController *controller = static_cast<VenomController *>(cookie);
	controller->venomTrail = static_cast<VenomTrailParticleSystem *>(node);
}

void VenomController::Preprocess(void)
{
	ProjectileController::Preprocess();

	SetCollisionKind(GetCollisionKind() | kCollisionVenom);

	SetRigidBodyFlags(kRigidBodyKeepAwake | kRigidBodyFixedOrientation | kRigidBodyDisabledContact);
	teleportFlag = false;

	Model *model = GetTargetNode();
	if (!model->GetManipulator())
	{
		if (!venomTrail)
		{
			venomTrail = new VenomTrailParticleSystem;
			model->GetOwningZone()->AppendNewSubnode(venomTrail);
		}
	}
}

ControllerMessage *VenomController::CreateMessage(ControllerMessageType type) const
{
	switch (type)
	{
		case kVenomMessageTeleport:

			return (new VenomTeleportMessage(GetControllerIndex()));

		case kVenomMessageExplode:

			return (new VenomExplodeMessage(GetControllerIndex()));
	}

	return (ProjectileController::CreateMessage(type));
}

void VenomController::SendInitialStateMessages(Player *player) const
{
	player->SendMessage(CreateVenomMessage(GetControllerIndex(), GetAttackerIndex(), GetTargetNode()->GetWorldPosition(), GetLinearVelocity()));
}

void VenomController::Move(void)
{
	if (TheMessageMgr->Server())
	{
		if ((lifeTime -= TheTimeMgr->GetDeltaTime()) <= 0)
		{
			VenomController::Destroy(GetTargetNode()->GetWorldPosition(), Zero3D);
			return;
		}
	}

	if (!teleportFlag)
	{
		Vector3D direction = GetLinearVelocity();
		float m = SquaredMag(direction);
		float r = InverseSqrt(m);
		direction *= r;

		float fdt = TheTimeMgr->GetFloatDeltaTime();
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
				venomTrail->NewParticle(position - direction * f, GetLinearVelocity());
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

RigidBodyStatus VenomController::HandleNewRigidBodyContact(const RigidBodyContact *contact, RigidBodyController *contactBody)
{
	if (TheMessageMgr->Server())
	{
		Point3D		worldPosition;
		Vector3D	worldNormal;

		contact->GetWorldContactPosition(this, &worldPosition, &worldNormal);

		RigidBodyType type = contactBody->GetRigidBodyType();
		if (type == kRigidBodyCharacter)
		{
			GameCharacterController *character = static_cast<GameCharacterController *>(contactBody);
			character->Damage(5 << 16, 0, GetAttackerController(), &worldPosition);
		}
		else if (type == kRigidBodyProjectile)
		{
			static_cast<ProjectileController *>(contactBody)->Destroy(worldPosition, worldNormal);
		}

		VenomController::Destroy(worldPosition, -worldNormal);
		return (kRigidBodyDestroyed);
	}

	return (kRigidBodyUnchanged);
}

RigidBodyStatus VenomController::HandleNewGeometryContact(const GeometryContact *contact)
{
	if (TheMessageMgr->Server())
	{
		const Transform4D& worldTransform = GetFinalTransform();
		Vector3D worldNormal = -(worldTransform * contact->GetRigidBodyContactNormal());

		const Geometry *geometry = contact->GetContactGeometry();

		const RemotePortal *portal = Game::DetectTeleporter(geometry);
		if (portal)
		{
			// Venom goes through a teleporting portal.

			Transform4D		portalTransform;

			portal->CalculateRemoteTransform(&portalTransform);

			Point3D position = portalTransform * worldTransform.GetTranslation();
			Vector3D velocity = portalTransform * GetOriginalLinearVelocity();
			Point3D center = worldTransform * contact->GetRigidBodyContactPosition() + worldNormal;

			TheMessageMgr->SendMessageAll(VenomTeleportMessage(GetControllerIndex(), position, velocity, center));
			return (kRigidBodyContactsBroken);
		}

		// The splash damage will not cause the contact to be deleted in this case.

		const Point3D& position = contact->GetGeometryContactPosition();
		Game::ProcessGeometryProperties(geometry, position, GetAttackerNode());

		VenomController::Destroy(geometry->GetWorldTransform() * position, worldNormal);
		return (kRigidBodyDestroyed);
	}

	return (kRigidBodyUnchanged);
}

void VenomController::Destroy(const Point3D& position, const Vector3D& normal)
{
	TheMessageMgr->SendMessageAll(VenomExplodeMessage(GetControllerIndex(), position, normal));
}


CreateVenomMessage::CreateVenomMessage() : CreateModelMessage(kModelMessageVenom)
{
}

CreateVenomMessage::CreateVenomMessage(int32 projectileIndex, int32 characterIndex, const Point3D& position, const Vector3D& velocity) : CreateModelMessage(kModelMessageVenom, projectileIndex, position)
{
	initialVelocity = velocity;
	attackerIndex = characterIndex;
}

CreateVenomMessage::~CreateVenomMessage()
{
}

void CreateVenomMessage::Compress(Compressor& data) const
{
	CreateModelMessage::Compress(data);

	data << initialVelocity;
	data << attackerIndex;
}

bool CreateVenomMessage::Decompress(Decompressor& data)
{
	if (CreateModelMessage::Decompress(data))
	{
		data >> initialVelocity;
		data >> attackerIndex;
		return (true);
	}

	return (false);
}

bool CreateVenomMessage::HandleMessage(Player *sender) const
{
	GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
	if (world)
	{
		Controller *controller = new VenomController(initialVelocity, static_cast<GameCharacterController *>(world->GetController(attackerIndex)));
		InitializeModel(world, Model::Get(kModelVenom), controller);

		OmniSource *source = new OmniSource("spider/Shoot", 64.0F);
		source->SetNodePosition(GetInitialPosition());
		world->AddNewNode(source);
	}

	return (true);
}


VenomTeleportMessage::VenomTeleportMessage(int32 controllerIndex) : ControllerMessage(VenomController::kVenomMessageTeleport, controllerIndex)
{
}

VenomTeleportMessage::VenomTeleportMessage(int32 controllerIndex, const Point3D& position, const Vector3D& velocity, const Point3D& center) : ControllerMessage(VenomController::kVenomMessageTeleport, controllerIndex)
{
	teleportPosition = position;
	teleportVelocity = velocity;
	effectCenter = center;
}

VenomTeleportMessage::~VenomTeleportMessage()
{
}

void VenomTeleportMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << teleportPosition;
	data << teleportVelocity;
	data << effectCenter;
}

bool VenomTeleportMessage::Decompress(Decompressor& data)
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

void VenomTeleportMessage::HandleControllerMessage(Controller *controller) const
{
	VenomController *venom = static_cast<VenomController *>(controller);

	venom->Teleport(effectCenter, Transform4D(K::x_unit, K::y_unit, K::z_unit, teleportPosition), teleportVelocity);
	venom->SetTeleportFlag();
}


VenomExplodeMessage::VenomExplodeMessage(int32 controllerIndex) : ControllerMessage(VenomController::kVenomMessageExplode, controllerIndex)
{
}

VenomExplodeMessage::VenomExplodeMessage(int32 controllerIndex, const Point3D& position, const Vector3D& normal) : ControllerMessage(VenomController::kVenomMessageExplode, controllerIndex)
{
	explodePosition = position;
	explodeNormal = normal;
}

VenomExplodeMessage::~VenomExplodeMessage()
{
}

void VenomExplodeMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << explodePosition;
	data << explodeNormal;
}

bool VenomExplodeMessage::Decompress(Decompressor& data)
{
	if (ControllerMessage::Decompress(data))
	{
		data >> explodePosition;
		data >> explodeNormal;
		return (true);
	}

	return (false);
}

void VenomExplodeMessage::HandleControllerMessage(Controller *controller) const
{
	Node *node = controller->GetTargetNode();
	World *world = node->GetWorld();

	static const char soundName[2][16] =
	{
		"sound/GooSplat1", "sound/GooSplat2"
	};

	OmniSource *source = new OmniSource(soundName[Math::Random(2)], 20.0F);
	source->GetObject()->SetInitialSourceFrequency(Math::RandomFloat(0.5F) + 0.75F);
	source->SetNodePosition(explodePosition);
	world->AddNewNode(source);

	int32 i = Math::Random(4);
	MarkingData markingData(Vector2D(0.5F, 0.5F), Vector2D((i & 1) ? 0.5F : 0.0F, (i & 2) ? 0.5F : 0.0F), kMarkingLight | kMarkingDepthWrite | kMarkingForceClip);
	AutoReleaseMaterial material(kMaterialVenomSplat);

	markingData.center = explodePosition;
	markingData.normal = explodeNormal;
	markingData.tangent = Math::CreatePerpendicular(explodeNormal);
	markingData.tangent.RotateAboutAxis(Math::RandomFloat(K::tau), explodeNormal);
	markingData.radius = 0.25F;
	markingData.materialObject = material;
	markingData.color = static_cast<const MaterialData *>(MaterialObject::FindMaterialRegistration(kMaterialVenomSplat)->GetMaterialCookie())->materialColor;
	markingData.lifeTime = 30000;
	MarkingEffect::New(world, &markingData);

	delete node;
}

// ZYUQURM
