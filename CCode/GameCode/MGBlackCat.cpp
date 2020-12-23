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


#include "MGBlackCat.h"
#include "MGGame.h"


using namespace C4;


namespace
{
	const float kBlackCatSplashDamageRadius = 4.0F;
}


BlackCatExplosionParticleSystem::BlackCatExplosionParticleSystem() :
		PointParticleSystem(kParticleSystemBlackCatExplosion, &particlePool, "texture/Trail"),
		particlePool(kMaxParticleCount, particleArray)
{
}

BlackCatExplosionParticleSystem::~BlackCatExplosionParticleSystem()
{
}

bool BlackCatExplosionParticleSystem::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	sphere->SetCenter(0.0F, 0.0F, 0.0F);
	sphere->SetRadius(5.0F);
	return (true);
}

void BlackCatExplosionParticleSystem::Preprocess(void)
{
	SetParticleSystemFlags(kParticleSystemSelfDestruct | kParticleSystemSoftDepth);
	PointParticleSystem::Preprocess();

	SetAmbientBlendState(kBlendInterpolate | kBlendAlphaPreserve);
	SetTransparentPosition(&GetWorldPosition());
	SetSoftDepthScale(2.0F);

	if (!GetFirstParticle())
	{
		Point3D center = GetSuperNode()->GetWorldTransform() * GetNodePosition();
		const ConstVector2D *trig = Math::GetTrigTable();

		for (machine a = 0; a < kMaxParticleCount; a++)
		{
			Particle *particle = particlePool.NewParticle();

			particle->emitTime = 0;
			particle->lifeTime = Math::Random(800) + 200;
			particle->radius = 0.5F + Math::RandomFloat(0.5F);
			particle->color.Set(0.0F, 0.0F, 0.0F, 1.0F);
			particle->orientation = Math::Random(256) << 24;
			particle->position = center + Math::RandomUnitVector3D() * 0.25F;

			float speed = Math::RandomFloat(0.005F);
			Vector2D csp = trig[Math::Random(128)] * speed;
			const Vector2D& cst = trig[Math::Random(256)];
			particle->velocity.Set(cst.x * csp.y, cst.y * csp.y, csp.x);

			AddParticle(particle);
		}
	}
}

void BlackCatExplosionParticleSystem::AnimateParticles(void)
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
			if (life < 500)
			{
				particle->color.alpha = (float) life * 0.002F;
			}

			particle->position += particle->velocity * fdt;
		}
		else
		{
			FreeParticle(particle);
		}

		particle = next;
	}
}


BlackCatController::BlackCatController() : ProjectileController(kControllerBlackCat)
{
}

BlackCatController::BlackCatController(const Vector3D& velocity, int32 damage, GameCharacterController *attacker) : ProjectileController(kControllerBlackCat, velocity, attacker)
{ 
	lifeTime = 10000;
	damageMultiplier = damage;
 
	if ((attacker) && (attacker->GetCharacterType() != kCharacterPlayer))
	{ 
		SetCollisionKind(GetCollisionKind() | kCollisionBlackCat);
	}
} 

BlackCatController::BlackCatController(const BlackCatController& blackCatController) : ProjectileController(blackCatController) 
{ 
	lifeTime = blackCatController.lifeTime;
	damageMultiplier = blackCatController.damageMultiplier;
}
 
BlackCatController::~BlackCatController()
{
}

Controller *BlackCatController::Replicate(void) const
{
	return (new BlackCatController(*this));
}

void BlackCatController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	ProjectileController::Pack(data, packFlags);

	data << ChunkHeader('LIFE', 4);
	data << lifeTime;

	data << ChunkHeader('DMUL', 4);
	data << damageMultiplier;

	data << TerminatorChunk;
}

void BlackCatController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	ProjectileController::Unpack(data, unpackFlags);
	UnpackChunkList<BlackCatController>(data, unpackFlags);
}

bool BlackCatController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'LIFE':

			data >> lifeTime;
			return (true);

		case 'DMUL':

			data >> damageMultiplier;
			return (true);
	}

	return (false);
}

void BlackCatController::Preprocess(void)
{
	ProjectileController::Preprocess();

	SetRigidBodyFlags(kRigidBodyKeepAwake | kRigidBodyFixedOrientation | kRigidBodyDisabledContact);

	Model *model = GetTargetNode();
	if (!model->GetManipulator())
	{
		Node *node = model->GetFirstSubnode();
		while (node)
		{
			if (node->GetNodeType() == kNodeSource)
			{
				blackCatSource = static_cast<OmniSource *>(node);
				blackCatSource->SetSourceVelocity(GetLinearVelocity());
				break;
			}

			node = node->Next();
		}

		frameAnimator.SetTargetModel(model);
		frameAnimator.SetAnimation("cat/cat/Attack");
		frameAnimator.GetFrameInterpolator()->SetMode(kInterpolatorForward | kInterpolatorLoop);
		model->SetRootAnimator(&frameAnimator);
	}
}

ControllerMessage *BlackCatController::CreateMessage(ControllerMessageType type) const
{
	switch (type)
	{
		case kBlackCatMessageExplode:

			return (new BlackCatExplodeMessage(GetControllerIndex()));
	}

	return (ProjectileController::CreateMessage(type));
}

void BlackCatController::SendInitialStateMessages(Player *player) const
{
	player->SendMessage(CreateBlackCatMessage(GetControllerIndex(), GetAttackerIndex(), GetTargetNode()->GetWorldPosition(), GetLinearVelocity()));
}

void BlackCatController::Move(void)
{
	if (TheMessageMgr->Server())
	{
		if ((lifeTime -= TheTimeMgr->GetDeltaTime()) <= 0)
		{
			BlackCatController::Destroy(GetTargetNode()->GetWorldPosition(), Normalize(-GetLinearVelocity()));
			return;
		}
	}

	Vector3D direction = GetLinearVelocity();
	SetBlackCatSourceVelocity(direction);

	Model *model = GetTargetNode();

	float m = SquaredMag(direction.GetVector2D());
	if (m > K::min_float)
	{
		m = -InverseSqrt(m);
		Vector3D y(direction.x * m, direction.y * m, 0.0F);
		Vector3D x(y.y, -y.x, 0.0F);
		SetRigidBodyMatrix3D(Transform(model->GetSuperNode()->GetInverseWorldTransform(), Matrix3D(x, y, K::z_unit)));
	}

	model->Animate();
}

RigidBodyStatus BlackCatController::HandleNewRigidBodyContact(const RigidBodyContact *contact, RigidBodyController *contactBody)
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

		BlackCatController::Destroy(worldPosition, -worldNormal);
		return (kRigidBodyDestroyed);
	}

	return (kRigidBodyUnchanged);
}

RigidBodyStatus BlackCatController::HandleNewGeometryContact(const GeometryContact *contact)
{
	if (TheMessageMgr->Server())
	{
		const Point3D& position = contact->GetGeometryContactPosition();
		Game::ProcessGeometryProperties(contact->GetContactGeometry(), position, GetAttackerNode());

		const Transform4D& worldTransform = GetFinalTransform();
		Vector3D worldNormal = -(worldTransform * contact->GetRigidBodyContactNormal());

		BlackCatController::Destroy(contact->GetContactGeometry()->GetWorldTransform() * position, worldNormal);
		return (kRigidBodyDestroyed);
	}

	return (kRigidBodyUnchanged);
}

void BlackCatController::Destroy(const Point3D& position, const Vector3D& normal)
{
	GameWorld *world = static_cast<GameWorld *>(GetTargetNode()->GetWorld());
	world->ProduceSplashDamage(position, kBlackCatSplashDamageRadius, 20 * damageMultiplier, GetAttackerController());

	TheMessageMgr->SendMessageAll(BlackCatExplodeMessage(GetControllerIndex(), position, normal));
}


CreateBlackCatMessage::CreateBlackCatMessage() : CreateModelMessage(kModelMessageBlackCat)
{
}

CreateBlackCatMessage::CreateBlackCatMessage(int32 projectileIndex, int32 characterIndex, const Point3D& position, const Vector3D& velocity, int32 damage) : CreateModelMessage(kModelMessageBlackCat, projectileIndex, position)
{
	initialVelocity = velocity;
	damageMultiplier = damage;
	attackerIndex = characterIndex;
}

CreateBlackCatMessage::~CreateBlackCatMessage()
{
}

void CreateBlackCatMessage::Compress(Compressor& data) const
{
	CreateModelMessage::Compress(data);

	data << initialVelocity;
	data << damageMultiplier;
	data << attackerIndex;
}

bool CreateBlackCatMessage::Decompress(Decompressor& data)
{
	if (CreateModelMessage::Decompress(data))
	{
		data >> initialVelocity;
		data >> damageMultiplier;
		data >> attackerIndex;
		return (true);
	}

	return (false);
}

bool CreateBlackCatMessage::HandleMessage(Player *sender) const
{
	GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
	if (world)
	{
		Controller *controller = new BlackCatController(initialVelocity, damageMultiplier, static_cast<GameCharacterController *>(world->GetController(attackerIndex)));
		InitializeModel(world, Model::Get(kModelBlackCat), controller);
	}

	return (true);
}


BlackCatExplodeMessage::BlackCatExplodeMessage(int32 controllerIndex) : ControllerMessage(BlackCatController::kBlackCatMessageExplode, controllerIndex)
{
}

BlackCatExplodeMessage::BlackCatExplodeMessage(int32 controllerIndex, const Point3D& position, const Vector3D& normal) : ControllerMessage(BlackCatController::kBlackCatMessageExplode, controllerIndex)
{
	explodePosition = position;
	explodeNormal = normal;
}

BlackCatExplodeMessage::~BlackCatExplodeMessage()
{
}

void BlackCatExplodeMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << explodePosition;
	data << explodeNormal;
}

bool BlackCatExplodeMessage::Decompress(Decompressor& data)
{
	if (ControllerMessage::Decompress(data))
	{
		data >> explodePosition;
		data >> explodeNormal;
		return (true);
	}

	return (false);
}

void BlackCatExplodeMessage::HandleControllerMessage(Controller *controller) const
{
	Node *node = controller->GetTargetNode();
	World *world = node->GetWorld();

	OmniSource *source = new OmniSource("cat/cat/Explode", 64.0F);
	OmniSourceObject *sourceObject = source->GetObject();
	sourceObject->SetReflectionVolume(0.5F);
	sourceObject->SetReflectionHFVolume(0.5F);
	source->SetSourcePriority(kSoundPriorityImpact);
	source->SetNodePosition(explodePosition);
	world->AddNewNode(source);

	BlackCatExplosionParticleSystem *explosion = new BlackCatExplosionParticleSystem;
	explosion->SetNodePosition(explodePosition);
	world->AddNewNode(explosion);

	delete node;
}

// ZYUQURM
