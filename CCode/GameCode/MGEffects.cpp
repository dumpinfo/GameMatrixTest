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


#include "MGEffects.h"
#include "MGGame.h"


using namespace C4;


ExplosionForce::ExplosionForce() : Force(kForceExplosion)
{
}

ExplosionForce::ExplosionForce(float maxForce, unsigned_int32 mask) : Force(kForceExplosion)
{
	maxExplosionForce = maxForce;
	forceExclusionMask = mask;
}

ExplosionForce::ExplosionForce(const ExplosionForce& explosionForce) : Force(explosionForce)
{
	maxExplosionForce = explosionForce.maxExplosionForce;
	forceExclusionMask = explosionForce.forceExclusionMask;
}

ExplosionForce::~ExplosionForce()
{
}

Force *ExplosionForce::Replicate(void) const
{
	return (new ExplosionForce(*this));
}

bool ExplosionForce::ValidField(const Field *field)
{
	return (field->GetFieldType() == kFieldSphere);
}

void ExplosionForce::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Force::Pack(data, packFlags);

	data << ChunkHeader('EXPL', 4);
	data << maxExplosionForce;

	data << ChunkHeader('EMSK', 4);
	data << forceExclusionMask;

	data << TerminatorChunk;
}

void ExplosionForce::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Force::Unpack(data, unpackFlags);
	UnpackChunkList<ExplosionForce>(data, unpackFlags);
}

bool ExplosionForce::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'EXPL':

			data >> maxExplosionForce;
			return (true);

		case 'EMSK':

			data >> forceExclusionMask;
			return (true);
	}

	return (false);
}

void ExplosionForce::Preprocess(void)
{
	const SphereField *field = static_cast<SphereField *>(GetTargetField());
	forceFalloff = -maxExplosionForce / field->GetObject()->GetSphereSize().x;
}

bool ExplosionForce::ApplyForce(RigidBodyController *rigidBody, const Transform4D& worldTransform, Vector3D *restrict force, Antivector3D *restrict torque)
{
	if ((rigidBody->GetCollisionKind() & forceExclusionMask) == 0)
	{
		static const ConstVector3D vertexOffset[8] =
		{
			{0.0F, 0.0F, 0.0F}, {1.0F, 0.0F, 0.0F}, {0.0F, 1.0F, 0.0F}, {1.0F, 1.0F, 0.0F},
			{0.0F, 0.0F, 1.0F}, {1.0F, 0.0F, 1.0F}, {0.0F, 1.0F, 1.0F}, {1.0F, 1.0F, 1.0F}
		};

		float k = rigidBody->GetWindDragMultiplier() * 0.125F;
		const Point3D& center = rigidBody->GetWorldCenterOfMass();
		const Box3D& box = rigidBody->GetBoundingBox();
		Vector3D size = box.max - box.min;

		force->Set(0.0F, 0.0F, 0.0F);
		torque->Set(0.0F, 0.0F, 0.0F);

		for (machine a = 0; a < 8; a++)
		{
			Point3D p = worldTransform * (box.min + (size & vertexOffset[a])); 
			Vector3D v = p - GetTargetField()->GetWorldPosition();

			float m = SquaredMag(v); 
			if (m > K::min_float)
			{ 
				Vector3D f = v * (FmaxZero(forceFalloff + maxExplosionForce * InverseSqrt(m)) * k);

				*force += f; 
				*torque += (p - center) ^ f;
			} 
		} 

		return (true);
	}
 
	return (false);
}

void ExplosionForce::ApplyForce(DeformableBodyController *deformableBody, const Transformable *transformable, int32 count, const Point3D *position, const SimdVector3D *velocity, SimdVector3D *restrict force)
{
	Point3D center = transformable->GetInverseWorldTransform() * GetTargetField()->GetWorldPosition();
	float multiplier = deformableBody->GetWindDragMultiplier();

	for (machine a = 0; a < count; a++)
	{
		Vector3D v = position[a] - center;
		float m = SquaredMag(v);

		if (m > K::min_float)
		{
			force[a] += v * (FmaxZero(forceFalloff + maxExplosionForce * InverseSqrt(m)) * multiplier);
		}
	}
}


ExplosionController::ExplosionController() : Controller(kControllerExplosion)
{
}

ExplosionController::ExplosionController(int32 time) : Controller(kControllerExplosion)
{
	explosionTime = time;
}

ExplosionController::ExplosionController(const ExplosionController& explosionController) : Controller(explosionController)
{
	explosionTime = explosionController.explosionTime;
}

ExplosionController::~ExplosionController()
{
}

Controller *ExplosionController::Replicate(void) const
{
	return (new ExplosionController(*this));
}

bool ExplosionController::ValidNode(const Node *node)
{
	return ((node->GetNodeType() == kNodeField) && (static_cast<const Field *>(node)->GetFieldType() == kFieldSphere));
}

void ExplosionController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Controller::Pack(data, packFlags);

	data << ChunkHeader('TIME', 4);
	data << explosionTime;

	data << TerminatorChunk;
}

void ExplosionController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Controller::Unpack(data, unpackFlags);
	UnpackChunkList<ExplosionController>(data, unpackFlags);
}

bool ExplosionController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'TIME':

			data >> explosionTime;
			return (true);
	}

	return (false);
}

void ExplosionController::Preprocess(void)
{
	Controller::Preprocess();

	initFlag = true;
}

void ExplosionController::Move(void)
{
	if (initFlag)
	{
		initFlag = false;

		SphereField *field = static_cast<SphereField *>(GetTargetNode());
		field->Update();

		PhysicsController *physicsController = field->GetWorld()->FindPhysicsController();
		if (physicsController)
		{
			physicsController->WakeFieldRigidBodies(field);
		}
	}

	if ((explosionTime -= TheTimeMgr->GetDeltaTime()) <= 0)
	{
		delete GetTargetNode();
	}
}


SparksParticleSystem::SparksParticleSystem() :
		LineParticleSystem(kParticleSystemSparks, &particlePool, "particle/Spark1"),
		particlePool(kMaxParticleCount, particleArray)
{
}

SparksParticleSystem::SparksParticleSystem(int32 count, float speed) :
		LineParticleSystem(kParticleSystemSparks, &particlePool, "particle/Spark1"),
		particlePool(kMaxParticleCount, particleArray)
{
	particleCount = Min(count, kMaxParticleCount);
	particleSpeed = speed;

	SetLengthMultiplier(4.0F);
}

SparksParticleSystem::~SparksParticleSystem()
{
}

bool SparksParticleSystem::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	sphere->SetCenter(0.0F, 0.0F, 0.0F);
	sphere->SetRadius(10.0F);
	return (true);
}

void SparksParticleSystem::Preprocess(void)
{
	LineParticleSystem::Preprocess();
	SetParticleSystemFlags(kParticleSystemSelfDestruct);

	if (!GetFirstParticle())
	{
		Point3D center = GetSuperNode()->GetWorldTransform() * GetNodePosition();
		const ConstVector2D *trig = Math::GetTrigTable();

		int32 count = particleCount;
		for (machine a = 0; a < count; a++)
		{
			Particle *particle = particlePool.NewParticle();

			particle->emitTime = 0;
			particle->lifeTime = 200 + Math::Random(400);
			particle->radius = 0.02F;
			float brightness = Math::RandomFloat(0.875F) + 0.125F;
			particle->color.Set(brightness, brightness, brightness * Math::RandomFloat(0.5F) + 0.1F, 1.0F);
			particle->orientation = 0;
			particle->position = center;

			int32 phi = Math::Random(128);
			int32 theta = Math::Random(256);
			float speed = (Math::RandomFloat(0.01F) + 0.01F) * particleSpeed;
			Vector2D csp = trig[phi] * speed;
			const Vector2D& cst = trig[theta];
			particle->velocity.Set(cst.x * csp.y, cst.y * csp.y, csp.x);

			AddParticle(particle);
		}
	}
}

void SparksParticleSystem::AnimateParticles(void)
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
			if (life < 200)
			{
				particle->color.alpha = (float) life * 0.005F;
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


DustParticleSystem::DustParticleSystem() :
		PointParticleSystem(kParticleSystemDust, &particlePool, "texture/Trail"),
		particlePool(kMaxParticleCount, particleArray)
{
}

DustParticleSystem::DustParticleSystem(int32 count, const Point3D *center, const ColorRGBA& color) :
		PointParticleSystem(kParticleSystemDust, &particlePool, "texture/Trail"),
		particlePool(kMaxParticleCount, particleArray)
{
	centerCount = Min(count, kMaxCenterCount);
	for (machine a = 0; a < count; a++)
	{
		centerArray[a] = center[a];
	}

	dustColor = color;
}

DustParticleSystem::~DustParticleSystem()
{
}

void DustParticleSystem::Pack(Packer& data, unsigned_int32 packFlags) const
{
	PointParticleSystem::Pack(data, packFlags);

	data << ChunkHeader('COLR', sizeof(ColorRGBA));
	data << dustColor;

	data << TerminatorChunk;
}

void DustParticleSystem::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	PointParticleSystem::Unpack(data, unpackFlags);
	UnpackChunkList<DustParticleSystem>(data, unpackFlags);
}

bool DustParticleSystem::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'COLR':

			data >> dustColor;
			return (true);
	}

	return (false);
}

bool DustParticleSystem::CalculateBoundingBox(Box3D *box) const
{
	box->min.Set(-1.0F, -1.0F, -1.0F);
	box->max.Set(1.0F, 1.0F, 1.0F);
	return (true);
}

bool DustParticleSystem::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	sphere->SetCenter(0.0F, 0.0F, 0.0F);
	sphere->SetRadius(1.0F);
	return (true);
}

void DustParticleSystem::Preprocess(void)
{
	SetParticleSystemFlags(kParticleSystemSelfDestruct | kParticleSystemStaticBoundingBox | kParticleSystemSoftDepth);
	PointParticleSystem::Preprocess();

	SetEffectListIndex(kEffectListLight);
	SetLightBlendState(BlendState(kBlendSourceAlpha, kBlendOne));
	SetSoftDepthScale(2.0F);

	if (!GetFirstParticle())
	{
		int32 count = centerCount;
		for (machine a = 0; a < count; a++)
		{
			const Point3D& center = centerArray[a];

			for (machine b = 0; b < kMaxDustCount; b++)
			{
				Particle *particle = particlePool.NewParticle();

				particle->emitTime = 0;
				particle->lifeTime = 400 + Math::Random(400);
				particle->color = dustColor;
				particle->orientation = Math::Random(256) << 24;
				particle->position = center;
				particle->velocity = Math::RandomUnitVector3D();
				particle->velocity.GetVector2D() *= 2.0e-4F;
				particle->velocity.z = Fabs(particle->velocity.z) * 6.0e-4F;
				particle->radius = 0.2F;

				AddParticle(particle);
			}
		}
	}
}

void DustParticleSystem::AnimateParticles(void)
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
			particle->radius += fdt * 0.0005F;
			particle->position += particle->velocity * fdt;
			particle->color.alpha = (float) life * dustColor.alpha * 0.00125F;
		}
		else
		{
			FreeParticle(particle);
		}

		particle = next;
	}
}


BloodParticleSystem::BloodParticleSystem() :
		TexcoordPointParticleSystem(kParticleSystemBlood, &particlePool, "particle/Splat3"),
		particlePool(kMaxParticleCount, particleArray)
{
}

BloodParticleSystem::BloodParticleSystem(const ColorRGB& color, int32 count) :
		TexcoordPointParticleSystem(kParticleSystemBlood, &particlePool, "particle/Splat3"),
		particlePool(kMaxParticleCount, particleArray)
{
	bloodColor = color;
	bloodCount = count;
}

BloodParticleSystem::~BloodParticleSystem()
{
}

void BloodParticleSystem::Pack(Packer& data, unsigned_int32 packFlags) const
{
	TexcoordPointParticleSystem::Pack(data, packFlags);

	data << ChunkHeader('COLR', sizeof(ColorRGB));
	data << bloodColor;

	data << TerminatorChunk;
}

void BloodParticleSystem::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	TexcoordPointParticleSystem::Unpack(data, unpackFlags);
	UnpackChunkList<BloodParticleSystem>(data, unpackFlags);
}

bool BloodParticleSystem::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'COLR':

			data >> bloodColor;
			return (true);
	}

	return (false);
}

bool BloodParticleSystem::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	sphere->SetCenter(0.0F, 0.0F, 0.0F);
	sphere->SetRadius(60.5F);
	return (true);
}

void BloodParticleSystem::Preprocess(void)
{
	TexcoordPointParticleSystem::Preprocess();

	SetEffectListIndex(kEffectListLight);
	SetParticleSystemFlags(kParticleSystemSelfDestruct);

	SetRenderState(kRenderDepthTest);
	SetAmbientBlendState(kBlendReplace);

	RenderSegment *segment = GetFirstRenderSegment();
	segment->SetMaterialState(segment->GetMaterialState() | kMaterialAlphaTest);

	if (!GetFirstParticle())
	{
		Point3D center = GetSuperNode()->GetWorldTransform() * GetNodePosition();
		const ConstVector2D *trig = Math::GetTrigTable();

		int32 particleCount = Max(bloodCount >> TheGame->GetGameDetailLevel(), 1);
		int32 splatCount = particleCount >> 3;

		for (machine a = 0; a < particleCount; a++)
		{
			TexcoordParticle *particle = particlePool.NewParticle();
			if (!particle)
			{
				break;
			}

			particle->emitTime = 0;
			particle->lifeTime = 1000 + Math::Random(1000);
			particle->radius = 0.1F + Math::RandomFloat(0.05F);
			particle->color = bloodColor;
			particle->position = center;

			UnsignedFixed angle = Math::Random(256) << 24;
			if (a < splatCount)
			{
				angle |= 1;
			}

			particle->orientation = angle;

			int32 phi = Math::Random(128);
			int32 theta = Math::Random(256);
			float speed = Math::RandomFloat(0.005F) + 0.0025F;
			Vector2D csp = trig[phi] * speed;
			const Vector2D& cst = trig[theta];
			particle->velocity.Set(cst.x * csp.y, cst.y * csp.y, csp.x);

			particle->texcoordScale.Set(0.5F, 0.5F);
			particle->texcoordOffset.Set((Math::Random(2) == 0) ? 0.0F : 0.5F, (Math::Random(2) == 0) ? 0.0F : 0.5F);

			AddParticle(particle);
		}
	}
}

void BloodParticleSystem::AnimateParticles(void)
{
	int32 dt = TheTimeMgr->GetDeltaTime();
	float fdt = TheTimeMgr->GetFloatDeltaTime();

	TexcoordParticle *particle = static_cast<TexcoordParticle *>(GetFirstParticle());
	while (particle)
	{
		TexcoordParticle *next = particle->GetNextParticle();

		int32 life = (particle->lifeTime -= dt);
		if (life > 0)
		{
			particle->velocity.z += fdt * -1.5e-5F;
			particle->radius += fdt * 7.0e-4F;

			if ((particle->orientation & 1) == 0)
			{
				particle->position += particle->velocity * fdt;
			}
			else
			{
				CollisionData	collisionData;

				World *world = GetWorld();
				Point3D newPosition = particle->position + particle->velocity * fdt;
				if (GetWorld()->DetectCollision(particle->position, newPosition, 0.0F, kCollisionRemains, &collisionData))
				{
					int32 i = Math::Random(4);
					MarkingData markingData(Vector2D(0.5F, 0.5F), Vector2D((i & 1) ? 0.5F : 0.0F, (i & 2) ? 0.5F : 0.0F), kMarkingLight | kMarkingDepthWrite | kMarkingForceClip);
					AutoReleaseMaterial material(kMaterialSplatter);

					markingData.markingOffset = Math::RandomFloat(0.015625F) + 0.0078125F;
					markingData.center = collisionData.position;
					markingData.normal = collisionData.normal;
					markingData.tangent = Math::CreatePerpendicular(markingData.normal);
					markingData.tangent.RotateAboutAxis(Math::RandomFloat(K::tau), markingData.normal);
					markingData.radius = Math::RandomFloat(0.5F) + 0.25F;
					markingData.materialObject = material;
					markingData.color = bloodColor * 1.25F;
					markingData.lifeTime = 30000;
					MarkingEffect::New(world, &markingData);

					FreeParticle(particle);
					goto nextParticle;
				}

				particle->position = newPosition;
			}

			if (life < 250)
			{
				particle->color.alpha = (float) life * 0.004F;
			}
		}
		else
		{
			FreeParticle(particle);
		}

		nextParticle:
		particle = next;
	}
}


SmokeTrailParticleSystem::SmokeTrailParticleSystem() :
		PointParticleSystem(kParticleSystemSmokeTrail, &particlePool, "texture/Trail"),
		particlePool(kMaxParticleCount, particleArray)
{
}

SmokeTrailParticleSystem::SmokeTrailParticleSystem(const ColorRGBA& color) :
		PointParticleSystem(kParticleSystemSmokeTrail, &particlePool, "texture/Trail"),
		particlePool(kMaxParticleCount, particleArray)
{
	smokeColor = color;
}

SmokeTrailParticleSystem::~SmokeTrailParticleSystem()
{
}

void SmokeTrailParticleSystem::Pack(Packer& data, unsigned_int32 packFlags) const
{
	PointParticleSystem::Pack(data, packFlags);

	data << ChunkHeader('COLR', sizeof(ColorRGBA));
	data << smokeColor;

	data << TerminatorChunk;
}

void SmokeTrailParticleSystem::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	PointParticleSystem::Unpack(data, unpackFlags);
	UnpackChunkList<SmokeTrailParticleSystem>(data, unpackFlags);
}

bool SmokeTrailParticleSystem::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'COLR':

			data >> smokeColor;
			return (true);
	}

	return (false);
}

void SmokeTrailParticleSystem::Preprocess(void)
{
	SetParticleSystemFlags(kParticleSystemSoftDepth | kParticleSystemDepthRamp);
	PointParticleSystem::Preprocess();

	SetAmbientBlendState(kBlendInterpolate);

	SetSoftDepthScale(2.0F);
	SetDepthRampRange(Range<float>(0.125F, 1.0F));
}

void SmokeTrailParticleSystem::AnimateParticles(void)
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
			particle->radius += fdt * 0.0003F;
			if (life < 1000)
			{
				particle->color.alpha = (float) life * smokeColor.alpha * 0.001F;
			}
		}
		else
		{
			FreeParticle(particle);
		}

		particle = next;
	}
}

void SmokeTrailParticleSystem::CreateSmoke(const Point3D& position, int32 life, float radius)
{
	Particle *particle = particlePool.NewParticle();
	if (particle)
	{
		particle->emitTime = 0;
		particle->lifeTime = life;
		particle->radius = radius;
		particle->color = smokeColor;
		particle->orientation = Math::Random(256) << 24;
		particle->position = position;

		AddFarthestParticle(particle);
	}
}


MaterializeParticleSystem::MaterializeParticleSystem(const ColorRGB& color, float radius) :
		PointParticleSystem(kParticleSystemMaterialize, &particlePool),
		particlePool(kMaxParticleCount, particleArray)
{
	effectColor = color;
	effectRadius = radius;

	SetNodeFlags(kNodeNonpersistent);
}

MaterializeParticleSystem::~MaterializeParticleSystem()
{
}

bool MaterializeParticleSystem::CalculateBoundingBox(Box3D *box) const
{
	float t = (float) kParticleLifeTime;
	float r = Fmax(t * 0.01F, effectRadius) + 0.2F;
	float halfHeight = t * 0.00025F + 0.5F;

	box->min.Set(-r, -r, -halfHeight);
	box->max.Set(r, r, halfHeight);
	return (true);
}

bool MaterializeParticleSystem::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	sphere->SetCenter(Zero3D);

	float t = (float) kParticleLifeTime;
	float r = Fmax(t * 0.01F, effectRadius) + 0.2F;
	float halfHeight = t * 0.00025F + 0.5F;
	sphere->SetRadius(Sqrt(r * r + halfHeight * halfHeight));

	return (true);
}

void MaterializeParticleSystem::Preprocess(void)
{
	SetParticleSystemFlags(kParticleSystemSelfDestruct | kParticleSystemPointSprite | kParticleSystemStaticBoundingBox);
	PointParticleSystem::Preprocess();

	const ConstVector2D *trig = Math::GetTrigTable();

	Point3D center = GetSuperNode()->GetWorldTransform() * GetNodePosition();
	float radius = effectRadius;

	for (machine a = 0; a < 50; a++)
	{
		OrbitParticle *particle = particlePool.NewParticle();

		particle->emitTime = (a != 0) ? Math::Random(100) : 0;
		particle->lifeTime = kParticleLifeTime;
		particle->radius = 0.2F;
		particle->orientation = 0;
		particle->color.Set(1.0F, 1.0F, 1.0F, 1.0F);

		int32 angle = Math::Random(256);
		const Vector2D& cs = trig[angle];
		particle->position.Set(center.x + radius * cs.x, center.y + radius * cs.y, center.z);
		particle->angularPosition = (float) angle * (K::tau * K::one_over_256) - K::tau;
		particle->angularVelocity = 0.02F;
		particle->velocity.z = (a < 25) ? 0.001F : -0.001F;

		AddParticle(particle);
	}

	for (machine a = 0; a < 50; a++)
	{
		OrbitParticle *particle = particlePool.NewParticle();

		particle->emitTime = Math::Random(500);
		particle->lifeTime = kParticleLifeTime;
		particle->radius = 0.1F;
		particle->orientation = 0;
		particle->color = effectColor;
		particle->color.GetColorRGB() *= Math::RandomFloat(1.0F);

		int32 angle = Math::Random(256);
		const Vector2D& cs = trig[angle];
		particle->position.Set(center.x + radius * cs.x, center.y + radius * cs.y, center.z);
		particle->velocity.z = Math::RandomFloat(0.002F) - 0.001F;
		particle->angularPosition = (float) angle * (K::tau * K::one_over_256) - K::tau;

		particle->angularVelocity = Math::RandomFloat(0.01F) + 0.01F;
		if (a < 25)
		{
			particle->angularVelocity = -particle->angularVelocity;
		}

		AddParticle(particle);
	}

	for (machine a = 0; a < 16; a++)
	{
		OrbitParticle *particle = particlePool.NewParticle();

		particle->emitTime = 0;
		particle->lifeTime = kParticleLifeTime;
		particle->radius = 0.2F;
		particle->orientation = 0;
		particle->position = center;
		particle->color.Set(1.0F, 1.0F, 1.0F, 1.0F);

		const Vector2D& cs = trig[a * 16];
		particle->velocity.Set(cs.x * 0.004F, cs.y * 0.004F, 0.0F);
		particle->angularPosition = 0.0F;
		particle->angularVelocity = 0.0F;

		AddParticle(particle);
	}

	for (machine a = 0; a < 24; a++)
	{
		OrbitParticle *particle = particlePool.NewParticle();

		particle->emitTime = Math::Random(100);
		particle->lifeTime = kParticleLifeTime;
		particle->radius = 0.05F;
		particle->orientation = 0;
		particle->position = center;
		particle->color = effectColor;
		particle->color.GetColorRGB() *= Math::RandomFloat(1.0F);

		const Vector2D& cs = trig[Math::Random(256)];
		float speed = Math::RandomFloat(0.002F) + 0.002F;
		particle->velocity.Set(cs.x * speed, cs.y * speed, 0.0F);
		particle->angularPosition = 0.0F;
		particle->angularVelocity = 0.0F;

		AddParticle(particle);
	}
}

void MaterializeParticleSystem::AnimateParticles(void)
{
	int32 dt = TheTimeMgr->GetDeltaTime();
	float fdt = TheTimeMgr->GetFloatDeltaTime();

	const Point3D& center = GetWorldPosition();
	float radius = effectRadius;

	OrbitParticle *particle = static_cast<OrbitParticle *>(GetFirstParticle());
	while (particle)
	{
		OrbitParticle *next = particle->GetNextParticle();

		if ((particle->emitTime -= dt) <= 0)
		{
			int32 life = (particle->lifeTime -= dt);
			if (life > 0)
			{
				float w = particle->angularVelocity;
				if (w != 0.0F)
				{
					float angle = (particle->angularPosition += fdt * w);
					particle->position.GetVector2D() = CosSin(angle) * radius + center.GetVector2D();
					particle->position.z += particle->velocity.z * fdt;
				}
				else
				{
					particle->position += particle->velocity * fdt;
				}

				if (life < 250)
				{
					particle->color.alpha = (float) life * 0.004F;
				}
			}
			else
			{
				FreeParticle(particle);
			}
		}

		particle = next;
	}
}


SpiralHelixParticleSystem::SpiralHelixParticleSystem(const ColorRGB& color, float radius, float velocity) :
		PolyboardParticleSystem(kParticleSystemSpiralHelix, &particlePool),
		particlePool(kMaxParticleCount, particleArray)
{
	effectColor = color;
	effectRadius = radius;
	upwardVelocity = velocity;

	emitTime = 0;

	SetNodeFlags(kNodeNonpersistent);
}

SpiralHelixParticleSystem::~SpiralHelixParticleSystem()
{
}

bool SpiralHelixParticleSystem::CalculateBoundingBox(Box3D *box) const
{
	float halfHeight = upwardVelocity * (float) (kParticleLifeTime / 2) * 0.5F + 0.5F;
	Point3D center(Zero3D + GetSuperNode()->GetInverseWorldTransform()[2] * (halfHeight - 1.0F));
	float r = effectRadius + 0.05F;

	box->min.Set(center.x - r, center.y - r, center.z - halfHeight);
	box->max.Set(center.x + r, center.y + r, center.z + halfHeight);
	return (true);
}

bool SpiralHelixParticleSystem::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	float halfHeight = upwardVelocity * (float) (kParticleLifeTime / 2) * 0.5F + 0.5F;
	sphere->SetCenter(Zero3D + GetSuperNode()->GetInverseWorldTransform()[2] * (halfHeight - 1.0F));

	float r = effectRadius + 0.05F;
	sphere->SetRadius(Sqrt(r * r + halfHeight * halfHeight));

	return (true);
}

void SpiralHelixParticleSystem::Preprocess(void)
{
	SetParticleSystemFlags(kParticleSystemStaticBoundingBox);
	PolyboardParticleSystem::Preprocess();
}

void SpiralHelixParticleSystem::AnimateParticles(void)
{
	int32 dt = TheTimeMgr->GetDeltaTime();
	float fdt = TheTimeMgr->GetFloatDeltaTime();

	const Point3D& center = GetWorldPosition();

	// Move the existing particles first.

	HelixParticle *particle = static_cast<HelixParticle *>(GetFirstParticle());
	while (particle)
	{
		HelixParticle *next = particle->GetNextParticle();

		int32 life = (particle->lifeTime -= dt);
		if (life > 0)
		{
			Vector2D t = CosSin(particle->angularPosition += fdt * particle->angularVelocity);
			float r = (particle->revolveRadius += fdt * 0.001F);

			particle->position.GetVector2D() = t * r + center.GetVector2D();
			particle->position.z += particle->velocity.z * fdt;

			particle->tangent.x = t.y;
			particle->tangent.y = -t.x;

			if (life < 250)
			{
				particle->color.alpha = (float) life * 0.004F;
			}
		}
		else
		{
			FreeParticle(particle);
		}

		particle = next;
	}

	if ((emitTime -= dt) < 0)
	{
		// It's time to emit a new particle.
		// Reset the next emit time up to a quarter second from now.

		emitTime = Math::Random(250);

		particle = particlePool.NewParticle();
		if (particle)
		{
			const ConstVector2D *trig = Math::GetTrigTable();
			float radius = effectRadius;

			int32 angle = Math::Random(256);
			float omega = Math::RandomFloat(0.005F) + 0.005F;

			// Each ribbon is made of 8 particles. Since we know one is available and the total
			// particle pool size is divisible by 8, we can assume 7 more are also available.

			for (machine a = 0; a < 8; a++)
			{
				particle->emitTime = 0;
				particle->lifeTime = kParticleLifeTime;
				particle->radius = 0.05F;
				particle->color = effectColor;

				int32 index = (angle - a * 4) & 0xFF;
				const Vector2D& cs = trig[index];

				particle->position.Set(center.x + radius * cs.x, center.y + radius * cs.y, center.z - (float) a * 0.01F);
				particle->angularPosition = (float) index * (K::tau * K::one_over_256) - K::tau;
				particle->angularVelocity = omega;
				particle->revolveRadius = radius;
				particle->velocity.z = upwardVelocity;

				particle->tangent.x = cs.y;
				particle->tangent.y = -cs.x;
				particle->tangent.z = 0.0F;

				AddParticle(particle);

				// The last particle in the ribbon has its terminator flag set.
				// Since we're using the default 16x16 particle texture, we set the texcoord for the
				// first particle to 0.09375 (1.5/16), the last one to 0.96875 (15.5/16), and all the
				// particles in the middle to 0.53125 (8.5/16).

				if (a < 7)
				{
					particle->texcoord = (a != 0) ? 0.53125F : 0.09375F;
					particle->terminator = false;

					particle = particlePool.NewParticle();
				}
				else
				{
					particle->texcoord = 0.96875F;
					particle->terminator = true;
				}
			}
		}
	}
}


TorchSmokeParticleSystem::TorchSmokeParticleSystem() :
		PointParticleSystem(kParticleSystemTorchSmoke, &particlePool, "texture/Smoke"),
		particlePool(kMaxParticleCount, particleArray)
{
	smokeTime = 0;
}

TorchSmokeParticleSystem::TorchSmokeParticleSystem(const TorchSmokeParticleSystem& torchSmokeParticleSystem) :
		PointParticleSystem(torchSmokeParticleSystem, &particlePool),
		particlePool(kMaxParticleCount, particleArray)
{
	smokeTime = 0;
}

TorchSmokeParticleSystem::~TorchSmokeParticleSystem()
{
}

Node *TorchSmokeParticleSystem::Replicate(void) const
{
	return (new TorchSmokeParticleSystem(*this));
}

void TorchSmokeParticleSystem::Pack(Packer& data, unsigned_int32 packFlags) const
{
	PointParticleSystem::Pack(data, packFlags);

	data << ChunkHeader('TIME', 4);
	data << smokeTime;

	data << TerminatorChunk;
}

void TorchSmokeParticleSystem::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	PointParticleSystem::Unpack(data, unpackFlags);
	UnpackChunkList<TorchSmokeParticleSystem>(data, unpackFlags);
}

bool TorchSmokeParticleSystem::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'TIME':

			data >> smokeTime;
			return (true);
	}

	return (false);
}

void TorchSmokeParticleSystem::Preprocess(void)
{
	SetParticleSystemFlags(kParticleSystemFreezeInvisible | kParticleSystemUnfreezeDynamic | kParticleSystemSoftDepth);
	PointParticleSystem::Preprocess();

	SetAmbientBlendState(kBlendInterpolate);
	SetSoftDepthScale(5.0F);

	Node *node = GetSuperNode();
	if (node->GetNodeType() == kNodeEffect)
	{
		SetTransparentAttachment(static_cast<Effect *>(node));
	}
}

void TorchSmokeParticleSystem::AnimateParticles(void)
{
	int32 dt = TheTimeMgr->GetDeltaTime();
	float fdt = TheTimeMgr->GetFloatDeltaTime();

	RotateParticle *particle = static_cast<RotateParticle *>(GetFirstParticle());
	while (particle)
	{
		RotateParticle *next = particle->GetNextParticle();

		int32 life = (particle->lifeTime -= dt);
		if (life > 0)
		{
			particle->radius += fdt * 0.0002F;
			particle->position.z += particle->velocity.z * fdt;
			particle->orientation += (particle->angularVelocity * dt) << 5;

			int32 emit = (particle->emitTime -= dt);
			if (emit > -500)
			{
				particle->color.alpha = (float) emit * -0.001F;
			}
			else if (life < 2000)
			{
				particle->color.alpha = (float) life * 0.00025F;
			}
			else
			{
				particle->color.alpha = 0.5F;
			}
		}
		else
		{
			FreeParticle(particle);
		}

		particle = next;
	}

	if ((smokeTime -= dt) <= 0)
	{
		smokeTime = MaxZero(smokeTime + 300);

		particle = particlePool.NewParticle();
		if (particle)
		{
			particle->emitTime = 0;
			particle->lifeTime = 3500;
			particle->radius = 0.4F;
			particle->color.Set(1.0F, 1.0F, 1.0F, 0.0F);
			particle->orientation = Math::Random(256) << 24;

			particle->position = GetWorldPosition();
			particle->velocity.Set(0.0F, 0.0F, Math::RandomFloat(0.00025F) + 0.00075F);
			particle->angularVelocity = Math::Random(60000) - 30000;

			AddParticle(particle);
		}
	}
}


ChimneySmokeParticleSystem::ChimneySmokeParticleSystem() :
		PointParticleSystem(kParticleSystemChimneySmoke, &particlePool, "texture/Smoke"),
		particlePool(kMaxParticleCount, particleArray)
{
	smokeTime = 0;
}

ChimneySmokeParticleSystem::ChimneySmokeParticleSystem(const ChimneySmokeParticleSystem& chimneySmokeParticleSystem) :
		PointParticleSystem(chimneySmokeParticleSystem, &particlePool),
		particlePool(kMaxParticleCount, particleArray)
{
	smokeTime = 0;
}

ChimneySmokeParticleSystem::~ChimneySmokeParticleSystem()
{
}

Node *ChimneySmokeParticleSystem::Replicate(void) const
{
	return (new ChimneySmokeParticleSystem(*this));
}

void ChimneySmokeParticleSystem::Pack(Packer& data, unsigned_int32 packFlags) const
{
	PointParticleSystem::Pack(data, packFlags);

	data << ChunkHeader('TIME', 4);
	data << smokeTime;

	data << TerminatorChunk;
}

void ChimneySmokeParticleSystem::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	PointParticleSystem::Unpack(data, unpackFlags);
	UnpackChunkList<ChimneySmokeParticleSystem>(data, unpackFlags);
}

bool ChimneySmokeParticleSystem::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'TIME':

			data >> smokeTime;
			return (true);
	}

	return (false);
}

void ChimneySmokeParticleSystem::Preprocess(void)
{
	SetParticleSystemFlags(kParticleSystemFreezeInvisible | kParticleSystemUnfreezeDynamic | kParticleSystemSoftDepth);
	PointParticleSystem::Preprocess();

	SetAmbientBlendState(kBlendInterpolate);
	SetSoftDepthScale(5.0F);

	Node *node = GetSuperNode();
	if (node->GetNodeType() == kNodeEffect)
	{
		SetTransparentAttachment(static_cast<Effect *>(node));
	}
}

void ChimneySmokeParticleSystem::AnimateParticles(void)
{
	int32 dt = TheTimeMgr->GetDeltaTime();
	float fdt = TheTimeMgr->GetFloatDeltaTime();

	Vector3D acceleration = GetWorldTransform()[0] * (fdt * 5.0e-8F);

	RotateParticle *particle = static_cast<RotateParticle *>(GetFirstParticle());
	while (particle)
	{
		RotateParticle *next = particle->GetNextParticle();

		int32 life = (particle->lifeTime -= dt);
		if (life > 0)
		{
			particle->radius += fdt * 0.0001F;
			particle->velocity += acceleration;
			particle->position += particle->velocity * fdt;
			particle->orientation += (particle->angularVelocity * dt) << 5;

			int32 emit = (particle->emitTime -= dt);
			if (emit > -500)
			{
				particle->color.alpha = (float) emit * -0.001F;
			}
			else if (life < 16000)
			{
				particle->color.alpha = (float) life * 3.125e-5F;
			}
			else
			{
				particle->color.alpha = 0.5F;
			}
		}
		else
		{
			FreeParticle(particle);
		}

		particle = next;
	}

	if ((smokeTime -= dt) <= 0)
	{
		smokeTime = MaxZero(smokeTime + 500);

		particle = particlePool.NewParticle();
		if (particle)
		{
			particle->emitTime = 0;
			particle->lifeTime = 20000 + Math::Random(2000);
			particle->radius = 0.6F;
			particle->color.Set(1.0F, 1.0F, 1.0F, 0.0F);
			particle->orientation = Math::Random(256) << 24;

			particle->position = GetWorldPosition();
			particle->velocity.Set(0.0F, 0.0F, Math::RandomFloat(0.0001F) + 0.001F);
			particle->angularVelocity = Math::Random(60000) - 30000;

			AddParticle(particle);
		}
	}
}


HeatWavesParticleSystem::HeatWavesParticleSystem() :
		PointParticleSystem(kParticleSystemHeatWaves, &particlePool, "effects/Heat"),
		particlePool(kMaxParticleCount, particleArray)
{
	waveTime = 0;
}

HeatWavesParticleSystem::HeatWavesParticleSystem(const HeatWavesParticleSystem& heatWavesParticleSystem) :
		PointParticleSystem(heatWavesParticleSystem, &particlePool),
		particlePool(kMaxParticleCount, particleArray)
{
	waveTime = 0;
}

HeatWavesParticleSystem::~HeatWavesParticleSystem()
{
}

Node *HeatWavesParticleSystem::Replicate(void) const
{
	return (new HeatWavesParticleSystem(*this));
}

void HeatWavesParticleSystem::Pack(Packer& data, unsigned_int32 packFlags) const
{
	PointParticleSystem::Pack(data, packFlags);

	data << ChunkHeader('TIME', 4);
	data << waveTime;

	data << TerminatorChunk;
}

void HeatWavesParticleSystem::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	PointParticleSystem::Unpack(data, unpackFlags);
	UnpackChunkList<HeatWavesParticleSystem>(data, unpackFlags);
}

bool HeatWavesParticleSystem::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'TIME':

			data >> waveTime;
			return (true);
	}

	return (false);
}

void HeatWavesParticleSystem::Preprocess(void)
{
	SetParticleSystemFlags(kParticleSystemFreezeInvisible | kParticleSystemUnfreezeDynamic);
	PointParticleSystem::Preprocess();

	SetDistortionState();
	SetRenderState(kRenderDepthTest | kRenderDepthInhibit | kRenderDepthOffset);
	SetDepthOffset(0.25F, &GetWorldPosition());
}

void HeatWavesParticleSystem::AnimateParticles(void)
{
	const Emitter *emitter = GetConnectedEmitter();
	if (emitter)
	{
		int32 dt = TheTimeMgr->GetDeltaTime();
		float fdt = TheTimeMgr->GetFloatDeltaTime();

		RotateParticle *particle = static_cast<RotateParticle *>(GetFirstParticle());
		while (particle)
		{
			RotateParticle *next = particle->GetNextParticle();

			int32 life = (particle->lifeTime -= dt);
			if (life > 0)
			{
				particle->radius += fdt * 0.0002F;
				particle->position.z += particle->velocity.z * fdt;
				particle->orientation += (particle->angularVelocity * dt) << 5;

				float t = (float) life * 0.001F;
				particle->color.alpha = (t - t * t) * 0.6F;
			}
			else
			{
				FreeParticle(particle);
			}

			particle = next;
		}

		const EmitterObject *object = emitter->GetObject();
		const Transform4D& transform = emitter->GetWorldTransform();

		if ((waveTime -= dt) <= 0)
		{
			waveTime = MaxZero(waveTime + Math::Random(150) + 50);

			particle = particlePool.NewParticle();
			if (particle)
			{
				particle->emitTime = 0;
				particle->lifeTime = 1000;
				particle->radius = (Math::RandomFloat(0.125F) + 0.0625F);
				particle->color.Set(0.0F, 0.0F, 0.0F, 0.0F);
				particle->orientation = Math::Random(256) << 24;

				particle->position = transform * object->GetBottomSurfaceEmissionPoint();
				particle->velocity.z = Math::RandomFloat(0.0005F) + 0.001F;
				particle->angularVelocity = Math::Random(120000) - 60000;

				AddParticle(particle);
			}
		}
	}
}


BubblingSteamParticleSystem::BubblingSteamParticleSystem() :
		TexcoordPointParticleSystem(kParticleSystemBubblingSteam, &particlePool, "particle/Puff1"),
		particlePool(kMaxParticleCount, particleArray)
{
	steamColor.Set(1.0F, 1.0F, 1.0F, 0.5F);
}

BubblingSteamParticleSystem::BubblingSteamParticleSystem(const BubblingSteamParticleSystem& bubblingSteamParticleSystem) :
		TexcoordPointParticleSystem(bubblingSteamParticleSystem, &particlePool),
		particlePool(kMaxParticleCount, particleArray)
{
	steamColor = bubblingSteamParticleSystem.steamColor;
}

BubblingSteamParticleSystem::~BubblingSteamParticleSystem()
{
}

Node *BubblingSteamParticleSystem::Replicate(void) const
{
	return (new BubblingSteamParticleSystem(*this));
}

void BubblingSteamParticleSystem::Pack(Packer& data, unsigned_int32 packFlags) const
{
	TexcoordPointParticleSystem::Pack(data, packFlags);

	data << ChunkHeader('colr', sizeof(ColorRGBA));
	data << steamColor;

	data << TerminatorChunk;
}

void BubblingSteamParticleSystem::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	TexcoordPointParticleSystem::Unpack(data, unpackFlags);
	UnpackChunkList<BubblingSteamParticleSystem>(data, unpackFlags);
}

bool BubblingSteamParticleSystem::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'colr':

			data >> steamColor;
			return (true);
	}

	return (false);
}

int32 BubblingSteamParticleSystem::GetCategoryCount(void) const
{
	return (TexcoordPointParticleSystem::GetCategoryCount() + 1);
}

Type BubblingSteamParticleSystem::GetCategoryType(int32 index, const char **title) const
{
	if (index == 0)
	{
		*title = TheGame->GetStringTable()->GetString(StringID('PART', kParticleSystemBubblingSteam));
		return (kParticleSystemBubblingSteam);
	}

	return (TexcoordPointParticleSystem::GetCategoryType(index - 1, title));
}

int32 BubblingSteamParticleSystem::GetCategorySettingCount(Type category) const
{
	if (category == kParticleSystemBubblingSteam)
	{
		return (2);
	}

	return (TexcoordPointParticleSystem::GetCategorySettingCount(category));
}

Setting *BubblingSteamParticleSystem::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kParticleSystemBubblingSteam)
	{
		const StringTable *table = TheGame->GetStringTable();

		if (index == 0)
		{
			const char *title = table->GetString(StringID('PART', kParticleSystemBubblingSteam, 'SETT'));
			return (new HeadingSetting(kParticleSystemBubblingSteam, title));
		}

		if (index == 1)
		{
			const char *title = table->GetString(StringID('PART', kParticleSystemBubblingSteam, 'COLR'));
			const char *picker = table->GetString(StringID('PART', kParticleSystemBubblingSteam, 'PICK'));
			return (new ColorSetting('COLR', steamColor, title, picker, kColorPickerAlpha));
		}

		return (nullptr);
	}

	return (TexcoordPointParticleSystem::GetCategorySetting(category, index, flags));
}

void BubblingSteamParticleSystem::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kParticleSystemBubblingSteam)
	{
		if (setting->GetSettingIdentifier() == 'COLR')
		{
			steamColor = static_cast<const ColorSetting *>(setting)->GetColor();
		}
	}
	else
	{
		TexcoordPointParticleSystem::SetCategorySetting(category, setting);
	}
}

void BubblingSteamParticleSystem::Preprocess(void)
{
	SetParticleSystemFlags(kParticleSystemFreezeInvisible | kParticleSystemUnfreezeDynamic | kParticleSystemSoftDepth);
	TexcoordPointParticleSystem::Preprocess();

	SetAmbientBlendState(kBlendInterpolate | kBlendAlphaPreserve);
	SetTransparentPosition(&GetWorldPosition());
	SetSoftDepthScale(5.0F);

	steamTime = 0;
}

void BubblingSteamParticleSystem::AnimateParticles(void)
{
	const Emitter *emitter = GetConnectedEmitter();
	if (emitter)
	{
		int32 dt = TheTimeMgr->GetDeltaTime();
		float fdt = TheTimeMgr->GetFloatDeltaTime();

		TexcoordParticle *particle = static_cast<TexcoordParticle *>(GetFirstParticle());
		while (particle)
		{
			TexcoordParticle *next = particle->GetNextParticle();

			int32 life = (particle->lifeTime -= dt);
			if (life > 0)
			{
				particle->radius += fdt * 0.0002F;
				particle->position.z += particle->velocity.z * fdt;
				if (life < 500)
				{
					particle->color.alpha = (float) life * steamColor.alpha * 0.002F;
				}
			}
			else
			{
				FreeParticle(particle);
			}

			particle = next;
		}

		int32 time = steamTime + dt;
		int32 count = time / kSteamEmitTime;
		steamTime = time - count * kSteamEmitTime;
		count = Min(count, 5);

		const EmitterObject *object = emitter->GetObject();
		const Transform4D& transform = emitter->GetWorldTransform();

		for (machine a = 0; a < count; a++)
		{
			particle = particlePool.NewParticle();
			if (!particle)
			{
				break;
			}

			particle->emitTime = 0;
			particle->lifeTime = 1500;
			particle->radius = 0.125F;
			particle->color = steamColor;
			particle->orientation = Math::Random(256) << 24;

			particle->position = transform * object->GetBottomSurfaceEmissionPoint();
			particle->velocity.z = Math::RandomFloat(0.0001F) + 0.0003F;

			int32 i = Math::Random(4);
			particle->texcoordScale.Set(0.5F, 0.5F);
			particle->texcoordOffset.Set((i & 1) ? 0.5F : 0.0F, (i & 2) ? 0.5F : 0.0F);

			AddParticle(particle);
		}
	}
}


FlowingGooParticleSystem::FlowingGooParticleSystem() :
		BlobParticleSystem(kParticleSystemFlowingGoo, &particlePool, 0.1F, 0.3F),
		particlePool(kMaxParticleCount, particleArray)
{
	gravityMultiplier = 1.0F;
	particleLifeTime = 1000;
}

FlowingGooParticleSystem::FlowingGooParticleSystem(const FlowingGooParticleSystem& flowingGooParticleSystem) :
		BlobParticleSystem(flowingGooParticleSystem, &particlePool),
		particlePool(kMaxParticleCount, particleArray)
{
	gravityMultiplier = flowingGooParticleSystem.gravityMultiplier;
	particleLifeTime = flowingGooParticleSystem.particleLifeTime;
}

FlowingGooParticleSystem::~FlowingGooParticleSystem()
{
}

Node *FlowingGooParticleSystem::Replicate(void) const
{
	return (new FlowingGooParticleSystem(*this));
}

void FlowingGooParticleSystem::Pack(Packer& data, unsigned_int32 packFlags) const
{
	BlobParticleSystem::Pack(data, packFlags);

	data << ChunkHeader('grav', 4);
	data << gravityMultiplier;

	data << ChunkHeader('life', 4);
	data << particleLifeTime;

	data << TerminatorChunk;
}

void FlowingGooParticleSystem::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	BlobParticleSystem::Unpack(data, unpackFlags);
	UnpackChunkList<FlowingGooParticleSystem>(data, unpackFlags);
}

bool FlowingGooParticleSystem::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'grav':

			data >> gravityMultiplier;
			return (true);

		case 'life':

			data >> particleLifeTime;
			return (true);
	}

	return (false);
}

int32 FlowingGooParticleSystem::GetCategoryCount(void) const
{
	return (BlobParticleSystem::GetCategoryCount() + 1);
}

Type FlowingGooParticleSystem::GetCategoryType(int32 index, const char **title) const
{
	if (index == 0)
	{
		*title = TheGame->GetStringTable()->GetString(StringID('PART', kParticleSystemFlowingGoo));
		return (kParticleSystemFlowingGoo);
	}

	return (BlobParticleSystem::GetCategoryType(index - 1, title));
}

int32 FlowingGooParticleSystem::GetCategorySettingCount(Type category) const
{
	if (category == kParticleSystemFlowingGoo)
	{
		return (3);
	}

	return (BlobParticleSystem::GetCategorySettingCount(category));
}

Setting *FlowingGooParticleSystem::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kParticleSystemFlowingGoo)
	{
		const StringTable *table = TheGame->GetStringTable();

		if (index == 0)
		{
			const char *title = table->GetString(StringID('PART', kParticleSystemFlowingGoo, 'SETT'));
			return (new HeadingSetting(kParticleSystemFlowingGoo, title));
		}

		if (index == 1)
		{
			const char *title = table->GetString(StringID('PART', kParticleSystemFlowingGoo, 'GRAV'));
			return (new TextSetting('GRAV', gravityMultiplier, title));
		}

		if (index == 2)
		{
			const char *title = table->GetString(StringID('PART', kParticleSystemFlowingGoo, 'LIFE'));
			return (new TextSetting('LIFE', (float) particleLifeTime * 0.001F, title));
		}

		return (nullptr);
	}

	return (BlobParticleSystem::GetCategorySetting(category, index, flags));
}

void FlowingGooParticleSystem::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kParticleSystemFlowingGoo)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'GRAV')
		{
			const char *text = static_cast<const TextSetting *>(setting)->GetText();
			gravityMultiplier = FmaxZero(Text::StringToFloat(text));
		}
		else if (identifier == 'LIFE')
		{
			const char *text = static_cast<const TextSetting *>(setting)->GetText();
			particleLifeTime = (int32) (FmaxZero(Text::StringToFloat(text)) * 1000.0F + 0.5F);
		}
	}
	else
	{
		BlobParticleSystem::SetCategorySetting(category, setting);
	}
}

void FlowingGooParticleSystem::AnimateParticles(void)
{
	const Emitter *emitter = GetConnectedEmitter();
	if (emitter)
	{
		float	size[kMaxObjectSizeCount];

		int32 dt = TheTimeMgr->GetDeltaTime();
		float fdt = TheTimeMgr->GetFloatDeltaTime();
		float gravity = gravityMultiplier * K::gravity;

		const Transform4D& transform = emitter->GetWorldTransform();
		const EmitterObject *object = emitter->GetObject();
		object->GetObjectSize(size);

		BlobParticle *particle = static_cast<BlobParticle *>(GetFirstParticle());
		while (particle)
		{
			BlobParticle *next = particle->GetNextParticle();

			float d = (particle->position - transform.GetTranslation()) * transform[2];
			if (d > size[2])
			{
				if ((particle->lifeTime -= dt) > 0)
				{
					particle->velocity.z += fdt * gravity;
					particle->position += particle->velocity * fdt;
				}
				else
				{
					FreeParticle(particle);
				}
			}
			else
			{
				particle->position += particle->velocity * fdt;
			}

			particle = next;
		}

		particle = particlePool.NewParticle();
		if (particle)
		{
			Point3D p = object->GetBottomSurfaceEmissionPoint();
			float r = Math::RandomFloat(0.15F) + 0.15F;
			float s = size[0] - r - GetVoxelScale();
			float p2 = p * p;
			if (p2 > s * s)
			{
				p *= s * InverseSqrt(p2);
			}

			particle->emitTime = 0;
			particle->lifeTime = particleLifeTime;
			particle->radius = r;
			particle->position = transform * p;
			particle->velocity = emitter->GetWorldTransform()[2] * (Math::RandomFloat(0.002F) + 0.003F);
			particle->scaleAxis = Math::RandomUnitVector3D();
			particle->inverseScale = Math::RandomFloat(0.5F) + 1.0F;

			AddParticle(particle);
		}
	}
}


LeakingGooParticleSystem::LeakingGooParticleSystem() :
		BlobParticleSystem(kParticleSystemLeakingGoo, &particlePool),
		particlePool(kMaxParticleCount, particleArray)
{
}

LeakingGooParticleSystem::LeakingGooParticleSystem(const RigidBodyController *rigidBody, float radius, float speed, float time) :
		BlobParticleSystem(kParticleSystemLeakingGoo, &particlePool, radius * 0.5F, radius),
		particlePool(kMaxParticleCount, particleArray)
{
	rigidBodyController = rigidBody;

	leakRadius = radius * 0.5F;
	leakSpeed = speed;

	leakTime = time;
	inverseLeakTime = 1.0F / time;

	emissionCount = 1.0F;
	speedMultiplier = 1.0F;
}

LeakingGooParticleSystem::~LeakingGooParticleSystem()
{
}

void LeakingGooParticleSystem::Pack(Packer& data, unsigned_int32 packFlags) const
{
	BlobParticleSystem::Pack(data, packFlags);

	data << ChunkHeader('BODY', 4);
	data << rigidBodyController->GetTargetNode()->GetNodeIndex();

	data << ChunkHeader('RADI', 4);
	data << leakRadius;

	data << ChunkHeader('SPED', 4);
	data << leakSpeed;

	data << ChunkHeader('TIME', 8);
	data << leakTime;
	data << inverseLeakTime;

	data << ChunkHeader('EMIS', 4);
	data << emissionCount;

	data << ChunkHeader('MULT', 4);
	data << speedMultiplier;

	data << TerminatorChunk;
}

void LeakingGooParticleSystem::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	BlobParticleSystem::Unpack(data, unpackFlags);
	UnpackChunkList<LeakingGooParticleSystem>(data, unpackFlags);
}

bool LeakingGooParticleSystem::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'BODY':
		{
			int32	nodeIndex;

			data >> nodeIndex;
			data.AddNodeLink(nodeIndex, &RigidBodyLinkProc, this);
			return (true);
		}

		case 'RADI':

			data >> leakRadius;
			return (true);

		case 'SPED':

			data >> leakSpeed;
			return (true);

		case 'TIME':

			data >> leakTime;
			data >> inverseLeakTime;
			return (true);

		case 'EMIS':

			data >> emissionCount;
			return (true);

		case 'MULT':

			data >> speedMultiplier;
			return (true);
	}

	return (false);
}

void LeakingGooParticleSystem::RigidBodyLinkProc(Node *node, void *cookie)
{
	LeakingGooParticleSystem *leakingGooParticleSystem = static_cast<LeakingGooParticleSystem *>(cookie);
	leakingGooParticleSystem->rigidBodyController = static_cast<RigidBodyController *>(node->GetController());
}

void LeakingGooParticleSystem::Preprocess(void)
{
	BlobParticleSystem::Preprocess();

	splatterColor.Set(1.0F, 1.0F, 1.0F);

	const MaterialObject *materialObject = GetMaterialObject();
	if (materialObject)
	{
		const MaterialRegistration *registration = MaterialObject::FindMaterialRegistration(materialObject->GetMaterialType());
		if (registration)
		{
			splatterColor = static_cast<const MaterialData *>(registration->GetMaterialCookie())->materialColor;
		}
	}
}

void LeakingGooParticleSystem::AnimateParticles(void)
{
	int32 dt = TheTimeMgr->GetDeltaTime();
	float fdt = TheTimeMgr->GetFloatDeltaTime();

	World *world = GetWorld();
	int32 detailLevel = TheGame->GetGameDetailLevel() * 2 + 8;

	BlobParticle *particle = static_cast<BlobParticle *>(GetFirstParticle());
	while (particle)
	{
		BlobParticle *next = particle->GetNextParticle();

		if ((particle->lifeTime -= dt) > 0)
		{
			CollisionData	collisionData;

			particle->velocity.z += fdt * K::gravity * 0.5F;
			Point3D newPosition = particle->position + particle->velocity * fdt;

			CollisionState state = world->QueryCollision(particle->position, newPosition, 0.0F, kCollisionRemains, &collisionData);
			if (state == kCollisionStateNone)
			{
				particle->position = newPosition;
			}
			else
			{
				if (state != kCollisionStateRigidBody)
				{
					if (Math::Random(detailLevel) == 0)
					{
						static const char soundName[2][16] =
						{
							"sound/GooSplat1", "sound/GooSplat2"
						};

						OmniSource *source = new OmniSource(soundName[Math::Random(2)], 20.0F);
						source->GetObject()->SetInitialSourceFrequency(Math::RandomFloat(0.5F) + 0.75F);
						source->SetNodePosition(collisionData.position);
						world->AddNewNode(source);

						int32 i = Math::Random(4);
						MarkingData markingData(Vector2D(0.5F, 0.5F), Vector2D((i & 1) ? 0.5F : 0.0F, (i & 2) ? 0.5F : 0.0F), kMarkingLight | kMarkingDepthWrite | kMarkingForceClip);
						AutoReleaseMaterial material(kMaterialSplatter);

						markingData.markingOffset = Math::RandomFloat(0.015625F) + 0.0078125F;
						markingData.center = collisionData.position;
						markingData.normal = collisionData.normal;
						markingData.tangent = Math::CreatePerpendicular(markingData.normal);
						markingData.tangent.RotateAboutAxis(Math::RandomFloat(K::tau), markingData.normal);
						markingData.radius = (Math::RandomFloat(leakRadius) + leakRadius) * 4.0F;
						markingData.materialObject = material;
						markingData.color = splatterColor;
						markingData.lifeTime = 30000;
						MarkingEffect::New(world, &markingData);
					}

					FreeParticle(particle);
					goto nextParticle;
				}

				particle->position = collisionData.position;
				particle->velocity -= ProjectOnto(particle->velocity, collisionData.normal) * 1.1F;
			}
		}
		else
		{
			FreeParticle(particle);
		}

		nextParticle:
		particle = next;
	}

	float t = (leakTime -= fdt) * inverseLeakTime;
	if (t > 0.0F)
	{
		float total = emissionCount + fdt * 0.05F;
		float f = PositiveFloor(total);
		emissionCount = total - f;

		if (f > 0.0F)
		{
			int32 count = (int32) f;

			float speed = leakSpeed * t * (2.0F - t);
			float multiplier = speedMultiplier;
			const Transform4D& transform = GetWorldTransform();

			do
			{
				particle = particlePool.NewParticle();
				if (!particle)
				{
					break;
				}

				particle->emitTime = 0;
				particle->lifeTime = 1000;
				particle->radius = Math::RandomFloat(leakRadius) + leakRadius;
				particle->velocity = transform[2] * (speed * multiplier) + (rigidBodyController->CalculateWorldPositionVelocity(transform.GetTranslation()) * 0.001F);
				particle->position = transform.GetTranslation() + particle->velocity * Math::RandomFloat(fdt);
				particle->scaleAxis = Math::RandomUnitVector3D();
				particle->inverseScale = Math::RandomFloat(0.5F) + 1.0F;

				AddParticle(particle);

				multiplier = Clamp(multiplier + (Math::RandomFloat(0.2F) - 0.1F), 1.0F, 1.5F);

			} while (--count > 0);

			speedMultiplier = multiplier;
		}
	}
	else
	{
		SetParticleSystemFlags(GetParticleSystemFlags() | kParticleSystemSelfDestruct);
	}
}


GooSplashParticleSystem::GooSplashParticleSystem() :
		TexcoordPointParticleSystem(kParticleSystemGooSplash, &particlePool, "particle/Splat3"),
		particlePool(kMaxParticleCount, particleArray)
{
}

GooSplashParticleSystem::GooSplashParticleSystem(const ColorRGB& color) :
		TexcoordPointParticleSystem(kParticleSystemGooSplash, &particlePool, "particle/Splat3"),
		particlePool(kMaxParticleCount, particleArray)
{
	splashColor = color;
}

GooSplashParticleSystem::~GooSplashParticleSystem()
{
}

void GooSplashParticleSystem::Pack(Packer& data, unsigned_int32 packFlags) const
{
	TexcoordPointParticleSystem::Pack(data, packFlags);

	data << ChunkHeader('COLR', sizeof(ColorRGB));
	data << splashColor;

	data << TerminatorChunk;
}

void GooSplashParticleSystem::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	TexcoordPointParticleSystem::Unpack(data, unpackFlags);
	UnpackChunkList<GooSplashParticleSystem>(data, unpackFlags);
}

bool GooSplashParticleSystem::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'COLR':

			data >> splashColor;
			return (true);
	}

	return (false);
}

void GooSplashParticleSystem::Preprocess(void)
{
	TexcoordPointParticleSystem::Preprocess();

	if (Fmax(splashColor.red, splashColor.green, splashColor.blue) > 0.0F)
	{
		SetEffectListIndex(kEffectListLight);
	}

	SetRenderState(kRenderDepthTest);
	SetAmbientBlendState(kBlendReplace);

	RenderSegment *segment = GetFirstRenderSegment();
	segment->SetMaterialState(segment->GetMaterialState() | kMaterialAlphaTest);
}

void GooSplashParticleSystem::AnimateParticles(void)
{
	int32 dt = TheTimeMgr->GetDeltaTime();
	float fdt = TheTimeMgr->GetFloatDeltaTime();

	TexcoordParticle *particle = static_cast<TexcoordParticle *>(GetFirstParticle());
	while (particle)
	{
		TexcoordParticle *next = particle->GetNextParticle();

		int32 life = (particle->lifeTime -= dt);
		if (life > 0)
		{
			particle->velocity.z += fdt * -1.0e-5F;
			particle->radius += fdt * 7.0e-4F;
			particle->position += particle->velocity * fdt;

			if (life < 1000)
			{
				particle->color.alpha = (float) life * 0.001F;
			}
		}
		else
		{
			FreeParticle(particle);
		}

		particle = next;
	}
}

void GooSplashParticleSystem::AddSplash(const Point3D& center, int32 count, float intensity)
{
	const ConstVector2D *trig = Math::GetTrigTable();

	for (machine a = 0; a < count; a++)
	{
		TexcoordParticle *particle = particlePool.NewParticle();
		if (!particle)
		{
			break;
		}

		particle->emitTime = 0;
		particle->lifeTime = 1000 + Math::Random(1000);
		particle->radius = intensity * (0.1F + Math::RandomFloat(0.05F));
		particle->color = splashColor;
		particle->position = center;
		particle->orientation = Math::Random(256) << 24;

		int32 phi = Math::Random(64);
		int32 theta = Math::Random(256);
		float speed = intensity * (Math::RandomFloat(0.0025F) + 0.00125F);
		Vector2D csp = trig[phi] * speed;
		const Vector2D& cst = trig[theta];
		particle->velocity.Set(cst.x * csp.y, cst.y * csp.y, csp.x + 0.001F);

		int32 i = Math::Random(4);
		particle->texcoordScale.Set(0.5F, 0.5F);
		particle->texcoordOffset.Set((i & 1) ? 0.5F : 0.0F, (i & 2) ? 0.5F : 0.0F);

		AddParticle(particle);
	}
}


BurstingGooParticleSystem::BurstingGooParticleSystem() :
		BlobParticleSystem(kParticleSystemBurstingGoo, &particlePool),
		particlePool(kMaxParticleCount, particleArray)
{
	gooSplash = nullptr;
}

BurstingGooParticleSystem::BurstingGooParticleSystem(const Vector3D& velocity) :
		BlobParticleSystem(kParticleSystemBurstingGoo, &particlePool, 0.125F, 0.5F),
		particlePool(kMaxParticleCount, particleArray)
{
	burstVelocity = velocity * 0.001F;
	gooSplash = nullptr;
}

BurstingGooParticleSystem::~BurstingGooParticleSystem()
{
	if (gooSplash)
	{
		gooSplash->Finalize();
	}
}

void BurstingGooParticleSystem::Pack(Packer& data, unsigned_int32 packFlags) const
{
	BlobParticleSystem::Pack(data, packFlags);

	if ((gooSplash) && (gooSplash->LinkedNodePackable(packFlags)))
	{
		data << ChunkHeader('GSPL', 4);
		data << gooSplash->GetNodeIndex();
	}

	data << TerminatorChunk;
}

void BurstingGooParticleSystem::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	BlobParticleSystem::Unpack(data, unpackFlags);
	UnpackChunkList<BurstingGooParticleSystem>(data, unpackFlags);
}

bool BurstingGooParticleSystem::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
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

void BurstingGooParticleSystem::GooSplashLinkProc(Node *node, void *cookie)
{
	BurstingGooParticleSystem *burstingGoo = static_cast<BurstingGooParticleSystem *>(cookie);
	burstingGoo->gooSplash = static_cast<GooSplashParticleSystem *>(node);
}

void BurstingGooParticleSystem::Preprocess(void)
{
	BlobParticleSystem::Preprocess();
	SetParticleSystemFlags(kParticleSystemSelfDestruct);

	if (!GetFirstParticle())
	{
		Point3D center = GetSuperNode()->GetWorldTransform() * GetNodePosition();

		for (machine a = 0; a < kMaxParticleCount; a++)
		{
			BlobParticle *particle = particlePool.NewParticle();

			particle->velocity = burstVelocity + Math::RandomUnitVector3D() * (Math::RandomFloat(0.004F) + 0.002F);
			particle->position = center + particle->velocity;
			particle->lifeTime = 4000;
			particle->radius = Math::RandomFloat(0.3F) + 0.2F;
			particle->scaleAxis = Math::RandomUnitVector3D();
			particle->inverseScale = Math::RandomFloat(0.5F) + 1.0F;

			AddParticle(particle);
		}
	}

	splatterColor.Set(1.0F, 1.0F, 1.0F);

	const MaterialObject *materialObject = GetMaterialObject();
	if (materialObject)
	{
		const MaterialRegistration *registration = MaterialObject::FindMaterialRegistration(materialObject->GetMaterialType());
		if (registration)
		{
			splatterColor = static_cast<const MaterialData *>(registration->GetMaterialCookie())->materialColor;
		}
	}
}

void BurstingGooParticleSystem::AnimateParticles(void)
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

					OmniSource *source = new OmniSource(soundName[Math::Random(2)], 40.0F);
					source->GetObject()->SetInitialSourceFrequency(Math::RandomFloat(0.5F) + 0.5F);
					source->SetNodePosition(collisionData.position);
					world->AddNewNode(source);

					int32 i = Math::Random(4);
					MarkingData markingData(Vector2D(0.5F, 0.5F), Vector2D((i & 1) ? 0.5F : 0.0F, (i & 2) ? 0.5F : 0.0F), kMarkingLight | kMarkingDepthWrite | kMarkingForceClip);
					AutoReleaseMaterial material(kMaterialSplatter);

					markingData.markingOffset = Math::RandomFloat(0.015625F) + 0.0078125F;
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

				if (!gooSplash)
				{
					gooSplash = new GooSplashParticleSystem(splatterColor);
					gooSplash->SetNodePosition(collisionData.position);
					world->AddNewNode(gooSplash);
				}

				gooSplash->AddSplash(collisionData.position, 3);

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


ShellEffect::ShellEffect() :
		Effect(kEffectShell, kRenderIndexedTriangles, kRenderDepthTest | kRenderDepthInhibit),
		diffuseAttribute(kAttributeMutable)
{
	shellGeometry = nullptr;

	SetActiveUpdateFlags(GetActiveUpdateFlags() | kUpdatePostprocess);
}

ShellEffect::ShellEffect(Geometry *geometry, float size, const ColorRGBA& color) :
		Effect(kEffectShell, kRenderIndexedTriangles, kRenderDepthTest | kRenderDepthInhibit),
		diffuseAttribute(color, kAttributeMutable)
{
	shellGeometry = geometry;
	scaleVector.Set(size, 0.0F, 0.0F, 0.0F);

	SetActiveUpdateFlags(GetActiveUpdateFlags() | kUpdatePostprocess);
}

ShellEffect::ShellEffect(const ShellEffect& shellEffect) :
		Effect(shellEffect),
		diffuseAttribute(shellEffect.diffuseAttribute.GetDiffuseColor())
{
	shellGeometry = shellEffect.shellGeometry;
	scaleVector = shellEffect.scaleVector;

	SetActiveUpdateFlags(GetActiveUpdateFlags() | kUpdatePostprocess);
}

ShellEffect::~ShellEffect()
{
}

Node *ShellEffect::Replicate(void) const
{
	return (new ShellEffect(*this));
}

void ShellEffect::Preprocess(void)
{
	Effect::Preprocess();

	if (shellGeometry)
	{
		SetAmbientBlendState(kBlendAccumulate);
		SetShaderFlags(kShaderAmbientEffect | kShaderNormalExpandVertex | (shellGeometry->GetShaderFlags() & kShaderNormalizeBasisVectors));
		SetRenderParameterPointer(&scaleVector);

		SetTransformable(shellGeometry->GetTransformable());
	}

	attributeList.Append(&diffuseAttribute);
	SetMaterialAttributeList(&attributeList);
}

void ShellEffect::HandlePostprocessUpdate(void)
{
	if (shellGeometry)
	{
		const BoundingSphere *sphere = shellGeometry->GetBoundingSphere();
		SetBoundingSphere(sphere->GetCenter(), sphere->GetRadius() + scaleVector.x);
	}
}

void ShellEffect::Render(const FrustumCamera *camera, List<Renderable> *effectList)
{
	if (shellGeometry)
	{
		GetWorld()->UpdateGeometry(shellGeometry);

		SetVertexBufferArrayFlags(shellGeometry->GetVertexBufferArrayFlags());
		SetVertexBuffer(kVertexBufferAttributeArray0, shellGeometry->GetVertexBuffer(kVertexBufferAttributeArray0), shellGeometry->GetVertexBufferStride(kVertexBufferAttributeArray0));
		SetVertexBuffer(kVertexBufferAttributeArray1, shellGeometry->GetVertexBuffer(kVertexBufferAttributeArray1), shellGeometry->GetVertexBufferStride(kVertexBufferAttributeArray1));
		SetVertexBuffer(kVertexBufferIndexArray, shellGeometry->GetVertexBuffer(kVertexBufferIndexArray));

		SetVertexCount(shellGeometry->GetVertexCount());
		SetVertexAttributeArray(kArrayPosition, shellGeometry->GetVertexAttributeOffset(kArrayPosition), shellGeometry->GetVertexAttributeComponentCount(kArrayPosition));
		SetVertexAttributeArray(kArrayNormal, shellGeometry->GetVertexAttributeOffset(kArrayNormal), shellGeometry->GetVertexAttributeComponentCount(kArrayNormal));

		SetPrimitiveCount(shellGeometry->GetPrimitiveCount());
		SetPrimitiveIndexOffset(shellGeometry->GetPrimitiveIndexOffset());

		InvalidateVertexData();

		effectList[kEffectListOpaque].Append(this);
	}
}

// ZYUQURM
