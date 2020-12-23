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


#include "MGQuantumCharger.h"
#include "MGFighter.h"
#include "MGGame.h"


using namespace C4;


namespace
{
	const float kLoopOscillationRadius = 0.046875F;

	enum
	{
		kMaxChargeLevel			= 5,
		kChargeFireDelayTime	= 350
	};

	Storage<QuantumCharger> quantumChargerStorage;
}


const ConstColorRGBA QuantumLoopParticleSystem::loopColor[kLoopCount] =
{
	{1.0F, 1.0F, 1.0F, 1.0F},
	{0.125F, 0.5F, 1.0F, 1.0F},
	{0.25F, 0.75F, 1.0F, 1.0F},
	{0.375F, 0.875F, 1.0F, 1.0F}
};


QuantumCharger::QuantumCharger() :
		Weapon(kWeaponQuantumCharger, 0, kModelQuantumCharger, kActionQuantumCharger, 40, 100),
		chargeControllerRegistration(kControllerCharge, nullptr),
		quantumChargerModelRegistration(kModelQuantumCharger, TheGame->GetStringTable()->GetString(StringID('MODL', 'WEAP', kModelQuantumCharger)), "quantum/gun/QuantumCharger", kModelPrecache, kControllerCollectable),
		chargeAmmoModelRegistration(kModelChargeAmmo, TheGame->GetStringTable()->GetString(StringID('MODL', 'AMMO', kModelChargeAmmo)), "quantum/ammo/Ammo", kModelPrecache, kControllerCollectable),
		chargeModelRegistration(kModelCharge, nullptr, "quantum/charge/Charge", kModelPrecache | kModelPrivate),
		quantumChargerAction(kWeaponQuantumCharger)
{
	TheInputMgr->AddAction(&quantumChargerAction);

	quantumScorchTexture = Texture::Get("quantum/gun/Scorch");
}

QuantumCharger::~QuantumCharger()
{
	quantumScorchTexture->Release();
}

void QuantumCharger::Construct(void)
{
	new(quantumChargerStorage) QuantumCharger;
}

void QuantumCharger::Destruct(void)
{
	quantumChargerStorage->~QuantumCharger();
}

WeaponController *QuantumCharger::NewWeaponController(FighterController *fighter) const
{
	return (new QuantumChargerController(fighter));
}


QuantumLoopParticleSystem::QuantumLoopParticleSystem() :
		PolyboardParticleSystem(kParticleSystemQuantumLoop, &particlePool),
		particlePool(kMaxParticleCount, particleArray)
{
}

QuantumLoopParticleSystem::QuantumLoopParticleSystem(float radius) :
		PolyboardParticleSystem(kParticleSystemQuantumLoop, &particlePool),
		particlePool(kMaxParticleCount, particleArray)
{
	quantumLoopFlags = 0;
	loopRadius = radius;
	lifeTime = -1;
}

QuantumLoopParticleSystem::~QuantumLoopParticleSystem()
{
}

void QuantumLoopParticleSystem::Pack(Packer& data, unsigned_int32 packFlags) const
{
	PolyboardParticleSystem::Pack(data, packFlags);

	data << ChunkHeader('FLAG', 4);
	data << quantumLoopFlags;

	data << ChunkHeader('RADI', 4);
	data << loopRadius;

	data << ChunkHeader('LIFE', 4);
	data << lifeTime;

	data << ChunkHeader('ROTA', (sizeof(Vector3D) + 8) * kLoopCount);

	for (machine a = 0; a < kLoopCount; a++)
	{ 
		data << rotationAxis[a];
	}
 
	for (machine a = 0; a < kLoopCount; a++)
	{ 
		data << rotationVelocity[a];
	}
 
	for (machine a = 0; a < kLoopCount; a++)
	{ 
		data << rotationAngle[a]; 
	}

	data << ChunkHeader('CTRL', 12 * kControlPointCount * kLoopCount);
	for (machine a = 0; a < kLoopCount; a++) 
	{
		for (machine b = 0; b < kControlPointCount; b++)
		{
			data << controlRadius[a][b];
		}

		for (machine b = 0; b < kControlPointCount; b++)
		{
			data << controlVelocity[a][b];
		}

		for (machine b = 0; b < kControlPointCount; b++)
		{
			data << controlAngle[a][b];
		}
	}

	data << TerminatorChunk;
}

void QuantumLoopParticleSystem::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	PolyboardParticleSystem::Unpack(data, unpackFlags);
	UnpackChunkList<QuantumLoopParticleSystem>(data, unpackFlags);
}

bool QuantumLoopParticleSystem::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> quantumLoopFlags;
			return (true);

		case 'RADI':

			data >> loopRadius;
			return (true);

		case 'LIFE':

			data >> lifeTime;
			return (true);

		case 'ROTA':

			for (machine a = 0; a < kLoopCount; a++)
			{
				data >> rotationAxis[a];
			}

			for (machine a = 0; a < kLoopCount; a++)
			{
				data >> rotationVelocity[a];
			}

			for (machine a = 0; a < kLoopCount; a++)
			{
				data >> rotationAngle[a];
			}

			return (true);

		case 'CTRL':

			for (machine a = 0; a < kLoopCount; a++)
			{
				for (machine b = 0; b < kControlPointCount; b++)
				{
					data >> controlRadius[a][b];
				}

				for (machine b = 0; b < kControlPointCount; b++)
				{
					data >> controlVelocity[a][b];
				}

				for (machine b = 0; b < kControlPointCount; b++)
				{
					data >> controlAngle[a][b];
				}
			}

			return (true);
	}

	return (false);
}

bool QuantumLoopParticleSystem::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	sphere->SetCenter(0.0F, 0.0F, 0.0F);
	sphere->SetRadius(loopRadius * 1.5F);
	return (true);
}

void QuantumLoopParticleSystem::Preprocess(void)
{
	PolyboardParticleSystem::Preprocess();

	SetTransformable(this);
	SetParticleSystemFlags(kParticleSystemObjectSpace);

	if (!GetManipulator())
	{
		if (lifeTime < 0)
		{
			lifeTime = 1000;

			for (machine loop = 0; loop < kLoopCount; loop++)
			{
				rotationAxis[loop] = Math::RandomUnitVector3D();
				rotationVelocity[loop] = Math::Random(64) - 32;
				rotationAngle[loop] = Math::Random(256) << 24;

				for (machine a = 0; a < kControlPointCount; a++)
				{
					controlRadius[loop][a] = Math::RandomFloat(kLoopOscillationRadius);
					int32 velocity = Math::Random(256) - 128;
					controlVelocity[loop][a] = velocity & ~((velocity >> 31) & 0x80);
					controlAngle[loop][a] = Math::Random(256) << 24;
				}

				float radius = (Math::RandomFloat(0.01F) + 0.005F);
				const ColorRGBA& color = loopColor[loop];

				for (machine a = 0; a < kLoopParticleCount; a++)
				{
					PolyParticle *particle = particlePool.NewParticle();

					particle->emitTime = 0;
					particle->lifeTime = 1;
					particle->radius = radius;
					particle->color = color;
					particle->position.Set(0.0F, 0.0F, 0.0F);
					particle->tangent.Set(0.0F, 0.0F, 1.0F);
					particle->texcoord = 0.53125F;
					particle->terminator = false;

					AddParticle(particle);
				}

				PolyParticle *particle = particlePool.NewParticle();

				particle->emitTime = 0;
				particle->lifeTime = 1;
				particle->radius = radius;
				particle->color = color;
				particle->position.Set(0.0F, 0.0F, 0.0F);
				particle->tangent.Set(0.0F, 0.0F, 1.0F);
				particle->texcoord = 0.53125F;
				particle->terminator = true;

				AddParticle(particle);
			}
		}
	}
}

void QuantumLoopParticleSystem::Move(void)
{
	if (quantumLoopFlags & kQuantumLoopExplode)
	{
		if ((lifeTime -= TheTimeMgr->GetDeltaTime()) <= 0)
		{
			delete this;
			return;
		}

		float fdt = TheTimeMgr->GetFloatDeltaTime();
		loopRadius += fdt * 0.0002F;
	}

	PolyboardParticleSystem::Move();
}

void QuantumLoopParticleSystem::AnimateParticles(void)
{
	int32 dt = TheTimeMgr->GetDeltaTime();
	const ConstVector2D *trig = Math::GetTrigTable();

	PolyParticle *particle = static_cast<PolyParticle *>(GetFirstParticle());
	for (machine loop = 0; loop < kLoopCount; loop++)
	{
		Point3D		controlPoint[kControlPointCount];

		UnsignedFixed angle = rotationAngle[loop] + ((rotationVelocity[loop] * dt) << 18);
		rotationAngle[loop] = angle;
		const Vector2D& cs = trig[angle >> 24];
		Quaternion rotation(rotationAxis[loop] * cs.x, cs.y);

		float s = loopRadius * 0.25F;
		for (machine a = 0; a < kLoopSegmentCount; a++)
		{
			int32 index1 = a * 2;
			int32 index2 = index1 + 1;

			UnsignedFixed angle1 = controlAngle[loop][index1] + ((controlVelocity[loop][index1] * dt) << 18);
			UnsignedFixed angle2 = controlAngle[loop][index2] + ((controlVelocity[loop][index2] * dt) << 18);
			controlAngle[loop][index1] = angle1;
			controlAngle[loop][index2] = angle2;

			int32 t1 = a * (256 / kLoopSegmentCount);
			int32 t2 = t1 + 256 / (kLoopSegmentCount * 3);

			float sxy1 = loopRadius + trig[angle1 >> 24].x * s;
			float sxy2 = loopRadius + trig[angle2 >> 24].x * s;

			controlPoint[index1] = Transform(Point3D(trig[t1] * sxy1, cs.y * s), rotation);
			controlPoint[index2] = Transform(Point3D(trig[t2] * sxy2, cs.y * s), rotation);
		}

		PolyParticle *firstParticle = particle;
		for (machine a = 0; a < kLoopSegmentCount; a++)
		{
			int32 index = a * 2;
			const Point3D& p1 = controlPoint[index];
			const Point3D& p2 = controlPoint[index + 1];

			int32 nextIndex = index + 2;
			if (nextIndex == kControlPointCount)
			{
				nextIndex = 0;
			}

			const Point3D& p4 = controlPoint[nextIndex];
			Vector3D p3 = p4 * 2.0F - controlPoint[nextIndex + 1];

			for (machine b = 0; b < kSegmentParticleCount; b++)
			{
				float t = (float) b * (1.0F / (float) kSegmentParticleCount);
				float u = 1.0F - t;
				float t2 = t * t;
				float u2 = u * u;

				particle->position = p1 * (u2 * u) + p2 * (3.0F * u2 * t) + p3 * (3.0F * u * t2) + p4 * (t2 * t);
				particle->tangent = Normalize(p1 * (-1.0F + 2.0F * t - t2) + p2 * (1.0F - 4.0F * t + 3.0F * t2) + p3 * (2.0F * t - 3.0F * t2) + p4 * t2);

				particle = particle->GetNextParticle();
			}
		}

		particle->position = firstParticle->position;
		particle->tangent = firstParticle->tangent;

		particle = particle->GetNextParticle();
	}
}

void QuantumLoopParticleSystem::Explode(void)
{
	quantumLoopFlags = kQuantumLoopExplode;

	for (machine a = 0; a < kLoopCount; a++)
	{
		rotationAxis[a].Set(0.0F, 0.0F, 1.0F);
	}
}


QuantumBoltParticleSystem::QuantumBoltParticleSystem() :
		PolyboardParticleSystem(kParticleSystemQuantumBolt, &particlePool),
		particlePool(kMaxParticleCount, particleArray)
{
}

QuantumBoltParticleSystem::QuantumBoltParticleSystem(const Point3D& p1, const Point3D& p2, const Vector3D& velocity) :
		PolyboardParticleSystem(kParticleSystemQuantumBolt, &particlePool),
		particlePool(kMaxParticleCount, particleArray)
{
	boltEndpoint[0] = p1;
	boltEndpoint[1] = p2;
	boltVelocity = velocity * 0.001F;

	lifeTime = 200;
}

QuantumBoltParticleSystem::~QuantumBoltParticleSystem()
{
}

void QuantumBoltParticleSystem::Pack(Packer& data, unsigned_int32 packFlags) const
{
	PolyboardParticleSystem::Pack(data, packFlags);

	data << ChunkHeader('PONT', sizeof(Point3D) * 2);
	data << boltEndpoint[0];
	data << boltEndpoint[1];

	data << ChunkHeader('VELO', sizeof(Vector3D));
	data << boltVelocity;

	data << ChunkHeader('LIFE', 4);
	data << lifeTime;

	data << TerminatorChunk;
}

void QuantumBoltParticleSystem::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	PolyboardParticleSystem::Unpack(data, unpackFlags);
	UnpackChunkList<QuantumBoltParticleSystem>(data, unpackFlags);
}

bool QuantumBoltParticleSystem::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'PONT':

			data >> boltEndpoint[0];
			data >> boltEndpoint[1];
			return (true);

		case 'VELO':

			data >> boltVelocity;
			return (true);

		case 'LIFE':

			data >> lifeTime;
			return (true);
	}

	return (false);
}

bool QuantumBoltParticleSystem::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	const Point3D& p1 = boltEndpoint[0];
	const Point3D& p2 = boltEndpoint[1];
	Point3D p3 = p1 + boltVelocity * (float) lifeTime;
	Point3D center = (p1 + p2 + p3) * K::one_over_3;

	sphere->SetCenter(GetInverseWorldTransform() * center);
	sphere->SetRadius(Magnitude(p1 - center));
	return (true);
}

void QuantumBoltParticleSystem::Preprocess(void)
{
	PolyboardParticleSystem::Preprocess();
	SetParticleSystemFlags(kParticleSystemSelfDestruct);

	if ((!GetManipulator()) && (!GetFirstParticle()))
	{
		const Point3D& p1 = boltEndpoint[0];
		const Point3D& p4 = boltEndpoint[1];

		Vector3D dp = p4 - p1;
		Vector3D tangent = dp * InverseMag(dp);
		Vector3D normal = Math::CreateUnitPerpendicular(tangent);
		Vector3D binormal = tangent % normal;

		Vector3D dq = dp * K::one_over_3;
		Point3D p2 = p1 + dq;
		Point3D p3 = p2 + dq;

		float maxRadius = Magnitude(dp) * 0.25F;
		float r2 = (Math::RandomFloat(1.0F) + 0.125F) * maxRadius;
		float r3 = (Math::RandomFloat(1.0F) + 0.125F) * maxRadius;

		const ConstVector2D *trig = Math::GetTrigTable();
		const Vector2D& cs2 = trig[Math::Random(256)];
		const Vector2D& cs3 = trig[Math::Random(256)];
		p2 += (normal * cs2.x + binormal * cs2.y) * r2;
		p3 += (normal * cs3.x + binormal * cs3.y) * r3;

		float t = 0.0F;
		maxRadius *= 0.125F;

		for (machine a = 0; a < kMaxParticleCount; a++)
		{
			PolyParticle *particle = particlePool.NewParticle();

			particle->emitTime = 0;
			particle->lifeTime = 1;
			particle->radius = 0.1F;
			particle->color.Set(1.0F, 1.0F, 1.0F, 1.0F);
			particle->texcoord = 0.53125F;
			particle->terminator = (a == kMaxParticleCount - 1);

			float u = 1.0F - t;
			float t2 = t * t;
			float u2 = u * u;

			int32 angle = Math::Random(256);
			const Vector2D& cs = trig[angle];
			float r = (Math::RandomFloat(1.0F) + 0.125F) * maxRadius * t * u;

			particle->position = p1 * (u2 * u) + p2 * (3.0F * u2 * t) + p3 * (3.0F * u * t2) + p4 * (t2 * t) + (normal * cs.x + binormal * cs.y) * r;
			particle->tangent = Normalize(p1 * (-1.0F + 2.0F * t - t2) + p2 * (1.0F - 4.0F * t + 3.0F * t2) + p3 * (2.0F * t - 3.0F * t2) + p4 * t2);
			particle->velocity = boltVelocity * u;

			AddParticle(particle);

			t += 1.0F / (float) (kMaxParticleCount - 1);
		}
	}
}

void QuantumBoltParticleSystem::Move(void)
{
	if ((lifeTime -= TheTimeMgr->GetDeltaTime()) <= 0)
	{
		delete this;
		return;
	}

	PolyboardParticleSystem::Move();
}

void QuantumBoltParticleSystem::AnimateParticles(void)
{
	float fdt = TheTimeMgr->GetFloatDeltaTime();
	PolyParticle *particle = static_cast<PolyParticle *>(GetFirstParticle());

	for (machine a = 0; a < kMaxParticleCount; a++)
	{
		particle->position += particle->velocity * fdt;
		particle->color.alpha = (float) lifeTime * 0.005F;

		particle = particle->GetNextParticle();
	}
}


QuantumSpiderParticleSystem::QuantumSpiderParticleSystem() :
		PolyboardParticleSystem(kParticleSystemQuantumSpider, &particlePool),
		particlePool(kMaxParticleCount, particleArray)
{
}

QuantumSpiderParticleSystem::QuantumSpiderParticleSystem(const Point3D& p1, const Point3D& p2, const Vector3D& normal, int32 life) :
		PolyboardParticleSystem(kParticleSystemQuantumSpider, &particlePool),
		particlePool(kMaxParticleCount, particleArray)
{
	boltEndpoint[0] = p1;
	boltEndpoint[1] = p2;
	boltNormal = normal;

	lifeTime = life;
}

QuantumSpiderParticleSystem::~QuantumSpiderParticleSystem()
{
}

void QuantumSpiderParticleSystem::Pack(Packer& data, unsigned_int32 packFlags) const
{
	PolyboardParticleSystem::Pack(data, packFlags);

	data << ChunkHeader('PONT', sizeof(Point3D) * 2);
	data << boltEndpoint[0];
	data << boltEndpoint[1];

	data << ChunkHeader('NRML', sizeof(Vector3D));
	data << boltNormal;

	data << ChunkHeader('LIFE', 4);
	data << lifeTime;

	data << TerminatorChunk;
}

void QuantumSpiderParticleSystem::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	PolyboardParticleSystem::Unpack(data, unpackFlags);
	UnpackChunkList<QuantumSpiderParticleSystem>(data, unpackFlags);
}

bool QuantumSpiderParticleSystem::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'PONT':

			data >> boltEndpoint[0];
			data >> boltEndpoint[1];
			return (true);

		case 'NRML':

			data >> boltNormal;
			return (true);

		case 'LIFE':

			data >> lifeTime;
			return (true);
	}

	return (false);
}

bool QuantumSpiderParticleSystem::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	const Point3D& p1 = boltEndpoint[0];
	const Point3D& p2 = boltEndpoint[1];
	Point3D center = (p1 + p2) * 0.5F;

	sphere->SetCenter(GetInverseWorldTransform() * center);
	sphere->SetRadius(Magnitude(p1 - center));
	return (true);
}

void QuantumSpiderParticleSystem::Preprocess(void)
{
	PolyboardParticleSystem::Preprocess();
	SetParticleSystemFlags(kParticleSystemSelfDestruct);

	if ((!GetManipulator()) && (!GetFirstParticle()))
	{
		const Point3D& p1 = boltEndpoint[0];
		const Point3D& p4 = boltEndpoint[1];

		Vector3D dp = p4 - p1;
		Vector3D tangent = dp * InverseMag(dp);
		Vector3D binormal = tangent % boltNormal;

		Vector3D dq = dp * K::one_over_3;
		Point3D p2 = p1 + dq;
		Point3D p3 = p2 + dq;

		float maxRadius = Magnitude(dp) * 0.25F;
		float r2 = (Math::RandomFloat(1.0F) + 0.125F) * maxRadius;
		float r3 = (Math::RandomFloat(1.0F) + 0.125F) * maxRadius;

		const ConstVector2D *trig = Math::GetTrigTable();
		const Vector2D& cs2 = trig[Math::Random(128)];
		const Vector2D& cs3 = trig[Math::Random(128)];
		p2 += (boltNormal * cs2.y + binormal * cs2.x) * r2;
		p3 += (boltNormal * cs3.y + binormal * cs3.x) * r3;

		float t = 0.0F;
		maxRadius *= 0.125F;

		for (machine a = 0; a < kMaxParticleCount; a++)
		{
			PolyParticle *particle = particlePool.NewParticle();

			particle->emitTime = 0;
			particle->lifeTime = 1;
			particle->radius = 0.1F;
			particle->texcoord = 0.53125F;
			particle->terminator = (a == kMaxParticleCount - 1);

			float u = 1.0F - t;
			float f = Fmin(u * 2.0F, 1.0F);
			particle->color.Set(f, f, f, 1.0F);

			float t2 = t * t;
			float u2 = u * u;

			int32 angle = Math::Random(256);
			const Vector2D& cs = trig[angle];
			float r = (Math::RandomFloat(1.0F) + 0.125F) * maxRadius * t * u;

			particle->position = p1 * (u2 * u) + p2 * (3.0F * u2 * t) + p3 * (3.0F * u * t2) + p4 * (t2 * t) + (boltNormal * cs.x + binormal * cs.y) * r;
			particle->tangent = Normalize(p1 * (-1.0F + 2.0F * t - t2) + p2 * (1.0F - 4.0F * t + 3.0F * t2) + p3 * (2.0F * t - 3.0F * t2) + p4 * t2);

			AddParticle(particle);

			t += 1.0F / (float) (kMaxParticleCount - 1);
		}
	}
}

void QuantumSpiderParticleSystem::Move(void)
{
	if ((lifeTime -= TheTimeMgr->GetDeltaTime()) <= 0)
	{
		delete this;
		return;
	}

	PolyboardParticleSystem::Move();
}

void QuantumSpiderParticleSystem::AnimateParticles(void)
{
	PolyParticle *particle = static_cast<PolyParticle *>(GetFirstParticle());
	for (machine a = 0; a < kMaxParticleCount; a++)
	{
		particle->color.alpha = (float) lifeTime * 0.005F;
		particle = particle->GetNextParticle();
	}
}


QuantumWavesParticleSystem::QuantumWavesParticleSystem() :
		PointParticleSystem(kParticleSystemQuantumWaves, &particlePool, "effects/Heat"),
		particlePool(kMaxParticleCount, particleArray)
{
}

QuantumWavesParticleSystem::~QuantumWavesParticleSystem()
{
}

void QuantumWavesParticleSystem::Preprocess(void)
{
	PointParticleSystem::Preprocess();

	SetDistortionState();
	SetTransformable(this);
	SetParticleSystemFlags(kParticleSystemObjectSpace);
}

void QuantumWavesParticleSystem::AnimateParticles(void)
{
	int32 dt = TheTimeMgr->GetDeltaTime();
	float fdt = TheTimeMgr->GetFloatDeltaTime();

	RotateParticle *particle = static_cast<RotateParticle *>(GetFirstParticle());
	while (particle)
	{
		RotateParticle *next = particle->GetNextParticle();

		if ((particle->emitTime -= dt) <= 0)
		{
			int32 life = (particle->lifeTime -= dt);
			if (life > 0)
			{
				particle->radius += fdt * 0.0002F;
				particle->position += particle->velocity * fdt;
				particle->orientation += (particle->angularVelocity * dt) << 5;

				float t = (float) life * 0.004F;
				particle->color.alpha = (t - t * t) * 0.6F;
			}
			else
			{
				FreeParticle(particle);
			}
		}

		particle = next;
	}

	for (;;)
	{
		particle = particlePool.NewParticle();
		if (!particle)
		{
			break;
		}

		particle->emitTime = Math::Random(300);
		particle->lifeTime = 250;
		particle->radius = (Math::RandomFloat(0.02F) + 0.02F);
		particle->color.Set(1.0F, 1.0F, 1.0F, 1.0F);
		particle->orientation = Math::Random(256) << 24;

		particle->position.Set(0.0F, 0.0F, 0.0F);
		particle->velocity = Math::RandomUnitVector3D() * 1.0e-3F;
		particle->angularVelocity = Math::Random(120000) - 60000;

		AddParticle(particle);
	}
}


QuantumChargerController::QuantumChargerController(FighterController *fighter) : WeaponController(kControllerQuantumCharger, fighter)
{
}

QuantumChargerController::~QuantumChargerController()
{
}

void QuantumChargerController::Preprocess(void)
{
	WeaponController::Preprocess();

	chargeState = 0;
	coreAngle = 0.0F;
	coreSpeed = 0.004F;
	beamTime = 0;

	quantumLoop = nullptr;
	quantumWaves = nullptr;

	Model *model = GetTargetNode();
	Node *node = model->GetFirstSubnode();
	while (node)
	{
		if (node->GetNodeFlags() & kNodeAnimateInhibit)
		{
			NodeType type = node->GetNodeType();
			if (type == kNodeGeometry)
			{
				const char *name = node->GetNodeName();
				if (name)
				{
					if (Text::CompareText(name, "Core"))
					{
						coreNode = node;
						coreTransform = node->GetNodeTransform().GetMatrix3D();
					}
					else if (Text::CompareText(name, "Charge"))
					{
						chargeNode = static_cast<Geometry *>(node);
						chargeLight = static_cast<Light *>(node->GetFirstSubnode());
					}
				}
			}
			else if (type == kNodeEffect)
			{
				Effect *effect = static_cast<Effect *>(node);
				if (effect->GetEffectType() == kEffectBeam)
				{
					chargeBeam = effect;
					effect->SetEffectListIndex(kEffectListFrontmost);
				}
			}
			else if (type == kNodeSource)
			{
				const char *name = node->GetNodeName();
				if (name)
				{
					if (Text::CompareText(name, "Build"))
					{
						buildSource = static_cast<OmniSource *>(node);
						buildSource->SetSourcePriority(kSoundPriorityWeapon);
					}
					else if (Text::CompareText(name, "Hold"))
					{
						holdSource = static_cast<OmniSource *>(node);
						holdSource->SetSourcePriority(kSoundPriorityWeapon);
					}
				}
			}
		}

		node = node->Next();
	}

	frameAnimator.SetTargetModel(model);
	frameAnimator.SetAnimation("quantum/gun/Fire");
	model->SetRootAnimator(&frameAnimator);
}

ControllerMessage *QuantumChargerController::CreateMessage(ControllerMessageType type) const
{
	switch (type)
	{
		case kQuantumChargerMessageBeam:

			return (new QuantumChargerBeamMessage(GetControllerIndex()));

		case kQuantumChargerMessageCharge:

			return (new QuantumChargerChargeMessage(GetControllerIndex()));

		case kQuantumChargerMessageRelease:

			return (new QuantumChargerReleaseMessage(GetControllerIndex()));
	}

	return (WeaponController::CreateMessage(type));
}

void QuantumChargerController::SendInitialStateMessages(Player *player) const
{
	if (chargeState & kQuantumChargerHold)
	{
		player->SendMessage(QuantumChargerChargeMessage(GetControllerIndex(), chargeLevel, chargeLimit));
	}
}

void QuantumChargerController::Move(void)
{
	Matrix3D	m;

	float speed = coreSpeed;
	float dt = TheTimeMgr->GetFloatDeltaTime();

	if (chargeState & kQuantumChargerHold)
	{
		speed = Fmin(speed + 5.0e-5F * dt, 0.012F);

		float level = Fmin(chargeLevel + chargeSpeed * dt, chargeLimit);
		chargeLevel = level;

		chargeNode->SetNodeMatrix3D(Matrix3D().SetScale(level * 0.2F));
		chargeNode->Invalidate();

		float f = level * (1.0F / (float) kMaxChargeLevel);
		chargeLight->GetObject()->SetLightColor(ColorRGB(f * 0.25F, f, f * 2.0F));

		const Vector3D& velocity = GetFighterController()->GetLinearVelocity();
		buildSource->SetSourceVelocity(velocity);
		holdSource->SetSourceVelocity(velocity);

		if ((level > chargeLimit * 0.875F) && (!holdSource->Playing()))
		{
			holdSource->Play();
		}

		quantumLoop->SetLoopRadius(level * 0.025F);
	}
	else
	{
		speed = Fmax(speed - 5.0e-5F * dt, 0.004F);

		if (chargeBeam->Enabled())
		{
			int32 time = beamTime;
			if (time < 0)
			{
				chargeBeam->SetNodeFlags(chargeBeam->GetNodeFlags() | kNodeDisabled);
			}
			else
			{
				beamTime = time - TheTimeMgr->GetDeltaTime();
			}
		}
	}

	coreSpeed = speed;
	float angle = coreAngle + speed * dt;
	if (angle > K::tau)
	{
		angle -= K::tau;
	}

	coreAngle = angle;

	m.SetRotationAboutX(angle);
	coreNode->SetNodeMatrix3D(m * coreTransform);
	coreNode->Invalidate();

	GetTargetNode()->Animate();
}

void QuantumChargerController::BeginFiring(bool primary)
{
	if ((!primary) && (chargeState == 0))
	{
		GamePlayer *player = GetFighterController()->GetFighterPlayer();
		int32 ammo = player->GetWeaponAmmo(kWeaponQuantumCharger, 0);
		if (ammo > 0)
		{
			TheMessageMgr->SendMessageAll(QuantumChargerChargeMessage(GetControllerIndex(), 1.0F, (float) Min(ammo, kMaxChargeLevel)));
		}
	}
}

void QuantumChargerController::EndFiring(void)
{
	if (chargeState & kQuantumChargerHold)
	{
		chargeState = kQuantumChargerRelease;
	}
}

void QuantumChargerController::ShowBeam(void)
{
	chargeBeam->SetNodeFlags(chargeBeam->GetNodeFlags() & ~kNodeDisabled);
	beamTime = 72;
}

void QuantumChargerController::BeginCharging(float level, float limit)
{
	chargeState = kQuantumChargerHold;
	chargeLevel = level;
	chargeLimit = limit;
	chargeAmmo = 0;

	float time = frameAnimator.GetAnimationHeader()->frameDuration * 31.0F;
	float start = (level < limit) ? time * (level - 1.0F) / (limit - 1.0F) : 1.0F;
	float finish = time * limit * 0.2F;

	Interpolator *interpolator = frameAnimator.GetFrameInterpolator();
	interpolator->Set(start, 1.0F, kInterpolatorForward);
	interpolator->SetMaxValue(finish);

	chargeSpeed = (float) (kMaxChargeLevel - 1) / time;
	chargeNode->SetNodeFlags(chargeNode->GetNodeFlags() & ~kNodeDisabled);
	chargeLight->SetNodeFlags(chargeLight->GetNodeFlags() & ~kNodeDisabled);
	chargeBeam->SetNodeFlags(chargeBeam->GetNodeFlags() & ~kNodeDisabled);

	if (!buildSource->Playing())
	{
		buildSource->Play();
	}

	Model *model = GetTargetNode();
	const Point3D& center = chargeNode->GetNodePosition();

	quantumLoop = new QuantumLoopParticleSystem(0.025F);
	quantumLoop->SetNodeFlags(kNodeCloneInhibit | kNodeAnimateInhibit);
	quantumLoop->SetNodeTransform(K::z_unit, K::y_unit, K::minus_x_unit, center);
	model->AppendNewSubnode(quantumLoop);

	quantumWaves = new QuantumWavesParticleSystem;
	quantumWaves->SetNodeFlags(kNodeCloneInhibit | kNodeAnimateInhibit);
	quantumWaves->SetNodeTransform(K::x_unit, K::y_unit, K::z_unit, center);
	model->AppendNewSubnode(quantumWaves);
}

void QuantumChargerController::ReleaseCharge(void)
{
	chargeState = 0;

	Interpolator *interpolator = frameAnimator.GetFrameInterpolator();
	interpolator->SetMode(kInterpolatorBackward);
	interpolator->SetRate(4.0F);

	chargeNode->SetNodeFlags(chargeNode->GetNodeFlags() | kNodeDisabled);
	chargeLight->SetNodeFlags(chargeLight->GetNodeFlags() | kNodeDisabled);
	chargeBeam->SetNodeFlags(chargeBeam->GetNodeFlags() | kNodeDisabled);

	if (buildSource->Playing())
	{
		buildSource->Stop();
	}

	if (holdSource->Playing())
	{
		holdSource->Stop();
	}

	delete quantumWaves;
	quantumWaves = nullptr;

	delete quantumLoop;
	quantumLoop = nullptr;
}

WeaponResult QuantumChargerController::UpdateWeapon(const Point3D& position, const Vector3D& direction, const Point3D& center)
{
	if (TheMessageMgr->Server())
	{
		const FighterController *fighter = GetFighterController();
		GamePlayer *player = fighter->GetFighterPlayer();

		int32 dt = TheTimeMgr->GetDeltaTime();
		int32 time = GetFireDelayTime() - dt;
		SetFireDelayTime(time);

		unsigned_int32 state = chargeState;
		if (state != 0)
		{
			int32 ammo = (int32) chargeLevel;
			int32 diff = ammo - chargeAmmo;
			if (diff > 0)
			{
				chargeAmmo = ammo;
				player->SetWeaponAmmo(kWeaponQuantumCharger, 0, player->GetWeaponAmmo(kWeaponQuantumCharger, 0) - diff);
			}

			if ((state & kQuantumChargerRelease) && (time <= 0))
			{
				SetFireDelayTime(time + kChargeFireDelayTime);

				TheMessageMgr->SendMessageAll(QuantumChargerReleaseMessage(GetControllerIndex()));

				World *world = GetTargetNode()->GetWorld();
				int32 projectileIndex = world->NewControllerIndex();
				TheMessageMgr->SendMessageAll(CreateChargeMessage(projectileIndex, fighter->GetControllerIndex(), position, direction * 100.0F, ammo));

				ProjectileController *projectileController = static_cast<ProjectileController *>(world->GetController(projectileIndex));
				projectileController->SetFirstPosition(center);
				return (kWeaponFired);
			}
		}
		else
		{
			unsigned_int32 flags = fighter->GetFighterFlags();
			if (((flags & kFighterFiringPrimary) != 0) && (time <= 0))
			{
				SetFireDelayTime(time + kChargeFireDelayTime);

				int32 ammo = player->GetWeaponAmmo(kWeaponQuantumCharger, 0);
				if (ammo <= 0)
				{
					return (kWeaponEmpty);
				}

				player->SetWeaponAmmo(kWeaponQuantumCharger, 0, ammo - 1);
				TheMessageMgr->SendMessageAll(QuantumChargerBeamMessage(GetControllerIndex()));

				World *world = GetTargetNode()->GetWorld();
				int32 projectileIndex = world->NewControllerIndex();
				TheMessageMgr->SendMessageAll(CreateChargeMessage(projectileIndex, fighter->GetControllerIndex(), position, direction * 100.0F, 1));

				ProjectileController *projectileController = static_cast<ProjectileController *>(world->GetController(projectileIndex));
				projectileController->SetFirstPosition(center);
				return (kWeaponFired);
			}
			else if (flags & kFighterFiringSecondary)
			{
				if (player->GetWeaponAmmo(kWeaponQuantumCharger, 0) <= 0)
				{
					return (kWeaponEmpty);
				}
			}
		}
	}

	return (kWeaponIdle);
}


QuantumChargerBeamMessage::QuantumChargerBeamMessage(int32 index) : ControllerMessage(QuantumChargerController::kQuantumChargerMessageBeam, index)
{
}

QuantumChargerBeamMessage::~QuantumChargerBeamMessage()
{
}

void QuantumChargerBeamMessage::HandleControllerMessage(Controller *controller) const
{
	static_cast<QuantumChargerController *>(controller)->ShowBeam();
}


QuantumChargerChargeMessage::QuantumChargerChargeMessage(int32 index) : ControllerMessage(QuantumChargerController::kQuantumChargerMessageCharge, index)
{
}

QuantumChargerChargeMessage::QuantumChargerChargeMessage(int32 index, float level, float limit) : ControllerMessage(QuantumChargerController::kQuantumChargerMessageCharge, index)
{
	chargeLevel = level;
	chargeLimit = limit;
}

QuantumChargerChargeMessage::~QuantumChargerChargeMessage()
{
}

void QuantumChargerChargeMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << chargeLevel;
	data << chargeLimit;
}

bool QuantumChargerChargeMessage::Decompress(Decompressor& data)
{
	if (ControllerMessage::Decompress(data))
	{
		data >> chargeLevel;
		data >> chargeLimit;
		return (true);
	}

	return (false);
}

void QuantumChargerChargeMessage::HandleControllerMessage(Controller *controller) const
{
	static_cast<QuantumChargerController *>(controller)->BeginCharging(chargeLevel, chargeLimit);
}


QuantumChargerReleaseMessage::QuantumChargerReleaseMessage(int32 index) : ControllerMessage(QuantumChargerController::kQuantumChargerMessageRelease, index)
{
}

QuantumChargerReleaseMessage::~QuantumChargerReleaseMessage()
{
}

void QuantumChargerReleaseMessage::HandleControllerMessage(Controller *controller) const
{
	static_cast<QuantumChargerController *>(controller)->ReleaseCharge();
}


ChargeExplosionParticleSystem::ChargeExplosionParticleSystem() :
		LineParticleSystem(kParticleSystemChargeExplosion, &particlePool, "particle/Spark1"),
		particlePool(kMaxParticleCount, particleArray)
{
}

ChargeExplosionParticleSystem::ChargeExplosionParticleSystem(int32 level) :
		LineParticleSystem(kParticleSystemChargeExplosion, &particlePool, "particle/Spark1"),
		particlePool(kMaxParticleCount, particleArray)
{
	chargeLevel = level;
	SetLengthMultiplier(2.0F);
}

ChargeExplosionParticleSystem::~ChargeExplosionParticleSystem()
{
}

void ChargeExplosionParticleSystem::Pack(Packer& data, unsigned_int32 packFlags) const
{
	LineParticleSystem::Pack(data, packFlags);

	data << ChunkHeader('LEVL', 4);
	data << chargeLevel;

	data << TerminatorChunk;
}

void ChargeExplosionParticleSystem::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	LineParticleSystem::Unpack(data, unpackFlags);
	UnpackChunkList<ChargeExplosionParticleSystem>(data, unpackFlags);
}

bool ChargeExplosionParticleSystem::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'LEVL':

			data >> chargeLevel;
			return (true);
	}

	return (false);
}

bool ChargeExplosionParticleSystem::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	sphere->SetCenter(0.0F, 0.0F, 0.0F);
	sphere->SetRadius(15.12F);
	return (true);
}

void ChargeExplosionParticleSystem::Preprocess(void)
{
	LineParticleSystem::Preprocess();
	SetParticleSystemFlags(kParticleSystemSelfDestruct);

	if (!GetFirstParticle())
	{
		Point3D center = GetSuperNode()->GetWorldTransform() * GetNodePosition();
		const ConstVector2D *trig = Math::GetTrigTable();

		int32 count = (chargeLevel - 1) * (kMaxParticleCount * 3) / (4 * (kMaxChargeLevel - 1)) + kMaxParticleCount / 4;
		count >>= TheGame->GetGameDetailLevel();

		for (machine a = 0; a < count; a++)
		{
			Particle *particle = particlePool.NewParticle();
			if (!particle)
			{
				break;
			}

			particle->emitTime = 0;
			particle->lifeTime = Math::Random(800) + 200;
			particle->radius = 0.005F + Math::RandomFloat(0.05F);

			float f = Math::RandomFloat(0.5F) + 0.5F;
			particle->color.Set(f * 2.0F - 1.0F, f, 1.0F, 1.0F);

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

void ChargeExplosionParticleSystem::AnimateParticles(void)
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


ChargeController::ChargeController() : ProjectileController(kControllerCharge)
{
}

inline void ChargeController::NewBoltTime(void)
{
	boltTime = Math::Random((TheGame->GetGameDetailLevel() + 1) * 50);
}

ChargeController::ChargeController(const Vector3D& velocity, int32 level, GameCharacterController *attacker) : ProjectileController(kControllerCharge, velocity, attacker)
{
	lifeTime = 6000;
	chargeLevel = level;

	NewBoltTime();
}

ChargeController::ChargeController(const ChargeController& chargeController) : ProjectileController(chargeController)
{
	lifeTime = chargeController.lifeTime;
	chargeLevel = chargeController.chargeLevel;

	NewBoltTime();
}

ChargeController::~ChargeController()
{
}

Controller *ChargeController::Replicate(void) const
{
	return (new ChargeController(*this));
}

void ChargeController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	ProjectileController::Pack(data, packFlags);

	data << ChunkHeader('LIFE', 4);
	data << lifeTime;

	data << ChunkHeader('BOLT', 4);
	data << boltTime;

	data << ChunkHeader('CHRG', 4);
	data << chargeLevel;

	data << TerminatorChunk;
}

void ChargeController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	ProjectileController::Unpack(data, unpackFlags);
	UnpackChunkList<ChargeController>(data, unpackFlags);
}

bool ChargeController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'LIFE':

			data >> lifeTime;
			return (true);

		case 'BOLT':

			data >> boltTime;
			return (true);

		case 'CHRG':

			data >> chargeLevel;
			return (true);
	}

	return (false);
}

void ChargeController::Preprocess(void)
{
	ProjectileController::Preprocess();

	SetRigidBodyFlags(kRigidBodyKeepAwake | kRigidBodyFixedOrientation | kRigidBodyDisabledContact);
	SetCollisionKind(GetCollisionKind() | kCollisionShot);
	SetGravityMultiplier(0.0F);
	chargeFlags = 0;

	float level = (float) chargeLevel;

	Model *model = GetTargetNode();
	Node *node = model->GetFirstSubnode();
	while (node)
	{
		NodeType type = node->GetNodeType();
		if (type == kNodeGeometry)
		{
			node->SetNodeMatrix3D(Matrix3D().SetScale(level * 0.4F));
		}
		else if (type == kNodeLight)
		{
			const GameCharacterController *controller = GetAttackerController();
			if (controller)
			{
				static_cast<Light *>(node)->SetExcludedNode(controller->GetTargetNode());
			}
		}
		else if (type == kNodeSource)
		{
			chargeSource = static_cast<OmniSource *>(node);
			chargeSource->SetSourceVelocity(GetLinearVelocity());
		}

		node = node->Next();
	}

	quantumLoop = new QuantumLoopParticleSystem(level * 0.075F);
	quantumLoop->SetNodeFlags(kNodeNonpersistent | kNodeCloneInhibit);
	quantumLoop->SetNodeTransform(K::z_unit, K::y_unit, K::minus_x_unit, Zero3D);
	model->AppendSubnode(quantumLoop);
}

ControllerMessage *ChargeController::CreateMessage(ControllerMessageType type) const
{
	switch (type)
	{
		case kChargeMessageTeleport:

			return (new ChargeTeleportMessage(GetControllerIndex()));

		case kChargeMessageExplode:

			return (new ChargeExplodeMessage(GetControllerIndex()));

		case kChargeMessageBolt:

			return (new ChargeBoltMessage(GetControllerIndex()));
	}

	return (ProjectileController::CreateMessage(type));
}

void ChargeController::SendInitialStateMessages(Player *player) const
{
	player->SendMessage(CreateChargeMessage(GetControllerIndex(), GetAttackerIndex(), GetTargetNode()->GetWorldPosition(), GetLinearVelocity(), chargeLevel));
}

void ChargeController::Move(void)
{
	int32 dt = TheTimeMgr->GetDeltaTime();

	if (TheMessageMgr->Server())
	{
		if ((lifeTime -= dt) <= 0)
		{
			ChargeController::Destroy(GetTargetNode()->GetWorldPosition(), Normalize(-GetLinearVelocity()));
			return;
		}

		if ((boltTime -= dt) <= 0)
		{
			CollisionData	data;

			NewBoltTime();

			const Model *model = GetTargetNode();
			const Point3D& position = model->GetWorldPosition();

			Vector3D direction = Math::RandomUnitVector3D();
			const Vector3D& velocity = GetLinearVelocity();
			direction -= ProjectOnto(direction, velocity * InverseMag(velocity));
			direction *= InverseMag(direction) * 4.0F;

			if (model->GetWorld()->DetectCollision(position, position + direction + velocity * 0.1F, 0.0F, kCollisionProjectile, &data))
			{
				TheMessageMgr->SendMessageAll(ChargeBoltMessage(GetControllerIndex(), position, data.position));
			}
		}
	}
}

RigidBodyStatus ChargeController::HandleNewRigidBodyContact(const RigidBodyContact *contact, RigidBodyController *contactBody)
{
	if (TheMessageMgr->Server())
	{
		Point3D		worldPosition;
		Vector3D	worldNormal;

		contact->GetWorldContactPosition(this, &worldPosition, &worldNormal);

		RigidBodyType type = contactBody->GetRigidBodyType();
		if (type == kRigidBodyCharacter)
		{
			Vector3D force = GetLinearVelocity() * ((float) chargeLevel * 0.5F);
			GameCharacterController *character = static_cast<GameCharacterController *>(contactBody);
			character->Damage((15 * chargeLevel) << 16, 0, GetAttackerController(), &worldPosition, &force);
		}
		else if (type == kRigidBodyProjectile)
		{
			static_cast<ProjectileController *>(contactBody)->Destroy(worldPosition, worldNormal);
		}
		else
		{
			const Transform4D& inverseTransform = contactBody->GetTargetNode()->GetInverseWorldTransform();
			Vector3D impulse = inverseTransform * GetLinearVelocity();
			ApplyRigidBodyImpulse(contactBody, impulse * ((float) (chargeLevel - 1) * 0.0078125F + 0.03125F), inverseTransform * worldPosition);

			if (contactBody->GetControllerType() == kControllerExplosive)
			{
				static_cast<ExplosiveController *>(contactBody)->Damage((15 * chargeLevel) << 16, 0.0F, GetAttackerController());
			}
		}

		ChargeController::Destroy(worldPosition, -worldNormal);
		return (kRigidBodyDestroyed);
	}

	return (kRigidBodyUnchanged);
}

RigidBodyStatus ChargeController::HandleNewGeometryContact(const GeometryContact *contact)
{
	if (TheMessageMgr->Server())
	{
		const Transform4D& worldTransform = GetFinalTransform();
		Vector3D worldNormal = -(worldTransform * contact->GetRigidBodyContactNormal());

		const Geometry *geometry = contact->GetContactGeometry();

		const RemotePortal *portal = Game::DetectTeleporter(geometry);
		if (portal)
		{
			// Charge goes through a teleporting portal.

			Transform4D		portalTransform;

			portal->CalculateRemoteTransform(&portalTransform);

			Point3D position = portalTransform * worldTransform.GetTranslation();
			Vector3D velocity = portalTransform * GetOriginalLinearVelocity();
			Point3D center = worldTransform * contact->GetRigidBodyContactPosition() + worldNormal;

			TheMessageMgr->SendMessageAll(ChargeTeleportMessage(GetControllerIndex(), position, velocity, center));
			return (kRigidBodyContactsBroken);
		}

		const Point3D& position = contact->GetGeometryContactPosition();
		Game::ProcessGeometryProperties(geometry, position, GetAttackerNode());

		chargeFlags |= kChargeTransferLoop;
		ChargeController::Destroy(geometry->GetWorldTransform() * position, worldNormal);
		return (kRigidBodyDestroyed);
	}

	return (kRigidBodyUnchanged);
}

void ChargeController::EnterWorld(World *world, const Point3D& worldPosition)
{
	OmniSource *source = new OmniSource("quantum/gun/Fire", 32.0F);
	source->GetObject()->SetInitialSourceFrequency(1.1F - (float) chargeLevel * 0.1F);
	source->SetSourcePriority(kSoundPriorityWeapon);
	source->SetNodePosition(worldPosition);
	SetSourceVelocity(source);
	world->AddNewNode(source);
}

void ChargeController::Destroy(const Point3D& position, const Vector3D& normal)
{
	TheMessageMgr->SendMessageAll(ChargeExplodeMessage(GetControllerIndex(), position, normal, chargeLevel, ((chargeFlags & kChargeTransferLoop) != 0)));
}

void ChargeController::TransferLoopEffect(World *world, const Point3D& position, const Vector3D& normal)
{
	world->GetRootNode()->AppendSubnode(quantumLoop);

	Vector3D tangent = Math::CreateUnitPerpendicular(normal);

	quantumLoop->SetNodeTransform(tangent, normal % tangent, normal, position + normal * kLoopOscillationRadius);
	quantumLoop->Invalidate();
	quantumLoop->Explode();
}


CreateChargeMessage::CreateChargeMessage() : CreateModelMessage(kModelMessageCharge)
{
}

CreateChargeMessage::CreateChargeMessage(int32 projectileIndex, int32 characterIndex, const Point3D& position, const Vector3D& velocity, int32 level) : CreateModelMessage(kModelMessageCharge, projectileIndex, position)
{
	initialVelocity = velocity;
	attackerIndex = characterIndex;
	chargeLevel = level;
}

CreateChargeMessage::~CreateChargeMessage()
{
}

void CreateChargeMessage::Compress(Compressor& data) const
{
	CreateModelMessage::Compress(data);

	data << initialVelocity;
	data << attackerIndex;
	data << (char) chargeLevel;
}

bool CreateChargeMessage::Decompress(Decompressor& data)
{
	if (CreateModelMessage::Decompress(data))
	{
		char	level;

		data >> initialVelocity;
		data >> attackerIndex;

		data >> level;
		chargeLevel = level;
		return (true);
	}

	return (false);
}

bool CreateChargeMessage::HandleMessage(Player *sender) const
{
	GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
	if (world)
	{
		Controller *controller = new ChargeController(initialVelocity, chargeLevel, static_cast<GameCharacterController *>(world->GetController(attackerIndex)));
		InitializeModel(world, Model::Get(kModelCharge), controller);
	}

	return (true);
}


ChargeTeleportMessage::ChargeTeleportMessage(int32 controllerIndex) : ControllerMessage(ChargeController::kChargeMessageTeleport, controllerIndex)
{
}

ChargeTeleportMessage::ChargeTeleportMessage(int32 controllerIndex, const Point3D& position, const Vector3D& velocity, const Point3D& center) : ControllerMessage(ChargeController::kChargeMessageTeleport, controllerIndex)
{
	teleportPosition = position;
	teleportVelocity = velocity;
	effectCenter = center;
}

ChargeTeleportMessage::~ChargeTeleportMessage()
{
}

void ChargeTeleportMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << teleportPosition;
	data << teleportVelocity;
	data << effectCenter;
}

bool ChargeTeleportMessage::Decompress(Decompressor& data)
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

void ChargeTeleportMessage::HandleControllerMessage(Controller *controller) const
{
	ChargeController *charge = static_cast<ChargeController *>(controller);

	charge->Teleport(effectCenter, Transform4D(Identity3D, teleportPosition), teleportVelocity);
	charge->SetChargeSourceVelocity(teleportVelocity);
}


ChargeExplodeMessage::ChargeExplodeMessage(int32 controllerIndex) : ControllerMessage(ChargeController::kChargeMessageExplode, controllerIndex)
{
}

ChargeExplodeMessage::ChargeExplodeMessage(int32 controllerIndex, const Point3D& position, const Vector3D& normal, int32 level, bool transfer) : ControllerMessage(ChargeController::kChargeMessageExplode, controllerIndex)
{
	explodePosition = position;
	explodeNormal = normal;
	chargeLevel = level;
	transferFlag = transfer;
}

ChargeExplodeMessage::~ChargeExplodeMessage()
{
}

void ChargeExplodeMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << explodePosition;
	data << explodeNormal;

	data << (char) chargeLevel;
	data << transferFlag;
}

bool ChargeExplodeMessage::Decompress(Decompressor& data)
{
	if (ControllerMessage::Decompress(data))
	{
		char	level;

		data >> explodePosition;
		data >> explodeNormal;

		data >> level;
		chargeLevel = level;

		data >> transferFlag;
		return (true);
	}

	return (false);
}

void ChargeExplodeMessage::HandleControllerMessage(Controller *controller) const
{
	MarkingData		markingData;

	Node *node = controller->GetTargetNode();
	World *world = node->GetWorld();

	OmniSource *source = new OmniSource("quantum/charge/Explode", 160.0F);
	source->GetObject()->SetReflectionVolume(0.5F);
	source->SetSourcePriority(kSoundPriorityImpact);
	source->SetNodePosition(explodePosition);
	world->AddNewNode(source);

	float level = (float) chargeLevel;

	if (TheWorldMgr->GetLightDetailLevel() > 0)
	{
		ColorRGB color(0.5F, 2.0F, 4.0F);
		PointLight *light = new PointLight(color, level * 2.0F + 8.0F);

		if (TheWorldMgr->GetLightDetailLevel() < 3)
		{
			light->GetObject()->SetLightFlags(kLightShadowInhibit);
		}

		light->SetController(new FlashController(color, 0.5F, 500));
		light->SetNodePosition(explodePosition + explodeNormal);
		world->AddNewNode(light);
	}

	ChargeExplosionParticleSystem *chargeExplosion = new ChargeExplosionParticleSystem(chargeLevel);
	chargeExplosion->SetNodePosition(explodePosition);
	world->AddNewNode(chargeExplosion);

	if (transferFlag)
	{
		static_cast<ChargeController *>(controller)->TransferLoopEffect(world, explodePosition, explodeNormal);
	}

	Vector3D explodeTangent = Math::CreateUnitPerpendicular(explodeNormal);
	Vector3D explodeOffset = explodeNormal * static_cast<RigidBodyController *>(controller)->GetBoundingRadius();

	markingData.center = explodePosition - explodeOffset;
	markingData.normal = explodeNormal;
	markingData.tangent = explodeTangent;
	markingData.tangent.RotateAboutAxis(Math::RandomFloat(K::tau), explodeNormal);
	markingData.radius = level * 0.25F;
	markingData.textureName = "quantum/gun/Scorch";
	markingData.color.Set(0.0F, 0.5F, 1.0F, 1.0F);
	markingData.lifeTime = 5000;
	MarkingEffect::New(world, &markingData);

	Point3D center = explodePosition + explodeOffset * 0.5F;

	int32 boltCount = 5 + chargeLevel * 3;
	for (machine a = 0; a < boltCount; a++)
	{
		Vector3D direction = explodeTangent * 4.0F;
		direction.RotateAboutAxis(Math::RandomFloat(K::tau), explodeNormal);

		QuantumSpiderParticleSystem *quantumSpider = new QuantumSpiderParticleSystem(center, center + direction, explodeNormal, Math::Random(200) + 50);
		world->AddNewNode(quantumSpider);
	}

	delete node;
}


ChargeBoltMessage::ChargeBoltMessage(int32 controllerIndex) : ControllerMessage(ChargeController::kChargeMessageBolt, controllerIndex)
{
}

ChargeBoltMessage::ChargeBoltMessage(int32 controllerIndex, const Point3D& p1, const Point3D& p2) : ControllerMessage(ChargeController::kChargeMessageBolt, controllerIndex)
{
	boltPosition[0] = p1;
	boltPosition[1] = p2;

	SetMessageFlags(kMessageUnreliable);
}

ChargeBoltMessage::~ChargeBoltMessage()
{
}

void ChargeBoltMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << boltPosition[0];
	data << boltPosition[1];
}

bool ChargeBoltMessage::Decompress(Decompressor& data)
{
	if (ControllerMessage::Decompress(data))
	{
		data >> boltPosition[0];
		data >> boltPosition[1];
		return (true);
	}

	return (false);
}

void ChargeBoltMessage::HandleControllerMessage(Controller *controller) const
{
	static const char soundName[2][32] =
	{
		"quantum/gun/Bolt1", "quantum/gun/Bolt2"
	};

	RigidBodyController *rigidBody = static_cast<RigidBodyController *>(controller);
	World *world = rigidBody->GetTargetNode()->GetWorld();

	QuantumBoltParticleSystem *quantumBolt = new QuantumBoltParticleSystem(boltPosition[0], boltPosition[1], rigidBody->GetLinearVelocity());
	world->AddNewNode(quantumBolt);

	SparksParticleSystem *sparks = new SparksParticleSystem(32, 0.25F);
	sparks->SetNodePosition(boltPosition[1]);
	world->AddNewNode(sparks);

	OmniSource *source = new OmniSource(soundName[Math::Random(2)], 128.0F);
	source->SetNodePosition(boltPosition[1]);
	world->AddNewNode(source);
}

// ZYUQURM
