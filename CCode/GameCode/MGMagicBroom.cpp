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


#include "MGMagicBroom.h"
#include "MGGame.h"


using namespace C4;


BroomSmokeParticleSystem::BroomSmokeParticleSystem() :
		TexcoordPointParticleSystem(kParticleSystemBroomSmoke, &particlePool, "particle/Puff1"),
		particlePool(kMaxParticleCount, particleArray)
{
}

BroomSmokeParticleSystem::BroomSmokeParticleSystem(const ColorRGBA& color) :
		TexcoordPointParticleSystem(kParticleSystemBroomSmoke, &particlePool, "particle/Puff1"),
		particlePool(kMaxParticleCount, particleArray)
{
	smokeColor = color;
	smokeDistance = 0.0F;
}

BroomSmokeParticleSystem::~BroomSmokeParticleSystem()
{
}

void BroomSmokeParticleSystem::Pack(Packer& data, unsigned_int32 packFlags) const
{
	TexcoordPointParticleSystem::Pack(data, packFlags);

	data << ChunkHeader('COLR', sizeof(ColorRGBA));
	data << smokeColor;

	data << ChunkHeader('DIST', 4);
	data << smokeDistance;

	data << TerminatorChunk;
}

void BroomSmokeParticleSystem::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	TexcoordPointParticleSystem::Unpack(data, unpackFlags);
	UnpackChunkList<BroomSmokeParticleSystem>(data, unpackFlags);
}

bool BroomSmokeParticleSystem::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'COLR':

			data >> smokeColor;
			return (true);

		case 'DIST':

			data >> smokeDistance;
			return (true);
	}

	return (false);
}

void BroomSmokeParticleSystem::Preprocess(void)
{
	SetParticleSystemFlags(kParticleSystemSoftDepth);
	TexcoordPointParticleSystem::Preprocess();

	SetAmbientBlendState(kBlendInterpolate);
	SetSoftDepthScale(2.0F);
}

void BroomSmokeParticleSystem::AnimateParticles(void)
{
	if (!(smokeDistance < 0.0F))
	{
		float m = SquaredMag(broomVelocity);
		if (m > K::min_float)
		{
			float r = InverseSqrt(m);
			float distance = smokeDistance + m * r * TheTimeMgr->GetDeltaSeconds();

			float multiplier = 1.5F - (float) TheGame->GetGameDetailLevel() * 0.5F;
			float count = PositiveFloor(distance * multiplier);
			smokeDistance = FmaxZero(distance - count / multiplier);

			if (count > 0.0F)
			{
				count = Fmin(count, 4.0F);
				Vector3D velocity = broomVelocity * r;
				do
				{
					TexcoordParticle *particle = particlePool.NewParticle();
					if (!particle)
					{
						break;
					}

					particle->emitTime = 0;
					particle->lifeTime = Math::Random(2000) + 1000;
					particle->radius = 1.0F;
					particle->color = smokeColor; 
					particle->orientation = Math::Random(256) << 24;
					particle->position = broomPosition - velocity * (distance * Math::RandomFloat(1.0F));
 
					int32 i = Math::Random(4);
					particle->texcoordScale.Set(0.5F, 0.5F); 
					particle->texcoordOffset.Set((i & 1) ? 0.5F : 0.0F, (i & 2) ? 0.5F : 0.0F);

					AddFarthestParticle(particle); 
				} while ((count -= 1.0F) > 0.0F);
			} 
		} 
	}

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

			if (life < 1000)
			{
				particle->color.alpha = (float) life * 0.001F * smokeColor.alpha;
			}
			else
			{
				int32 emit = (particle->emitTime -= dt);
				particle->color.alpha = Fmin((float) emit * -0.004F, 1.0F) * smokeColor.alpha;
			}
		}
		else
		{
			FreeParticle(particle);
		}

		particle = next;
	}
}


MagicBroomParticleSystem::MagicBroomParticleSystem() :
		PointParticleSystem(kParticleSystemMagicBroom, &particlePool, "particle/Star2"),
		particlePool(kMaxParticleCount, particleArray)
{
	emissionTime = 0;
}

MagicBroomParticleSystem::~MagicBroomParticleSystem()
{
}

void MagicBroomParticleSystem::Pack(Packer& data, unsigned_int32 packFlags) const
{
	PointParticleSystem::Pack(data, packFlags);

	data << ChunkHeader('TIME', 4);
	data << emissionTime;

	data << TerminatorChunk;
}

void MagicBroomParticleSystem::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	PointParticleSystem::Unpack(data, unpackFlags);
	UnpackChunkList<MagicBroomParticleSystem>(data, unpackFlags);
}

bool MagicBroomParticleSystem::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'TIME':

			data >> emissionTime;
			return (true);
	}

	return (false);
}

void MagicBroomParticleSystem::Preprocess(void)
{
	PointParticleSystem::Preprocess();
	SetAmbientBlendState(kBlendInterpolate);
}

void MagicBroomParticleSystem::AnimateParticles(void)
{
	int32 dt = TheTimeMgr->GetDeltaTime();
	float fdt = TheTimeMgr->GetFloatDeltaTime();

	int32 time = emissionTime;
	if (time >= 0)
	{
		time += dt;
		int32 count = time >> 4;
		emissionTime = time - (count << 4);

		if (count != 0)
		{
			Vector3D velocity = broomVelocity * 0.0005F;
			Vector3D tangent = Math::CreateUnitPerpendicular(broomVelocity);
			Vector3D bitangent = (broomVelocity * InverseMag(broomVelocity)) % tangent;

			const ConstVector2D *trig = Math::GetTrigTable();

			for (machine a = 0; a < count; a++)
			{
				Particle *particle = particlePool.NewParticle();
				if (!particle)
				{
					break;
				}

				particle->emitTime = 0;
				particle->lifeTime = Math::Random(1000) + 500;
				particle->radius = Math::RandomFloat(0.15F) + 0.05F;
				particle->color.Set(1.0F, 1.0F, Math::RandomFloat(0.375F) + 0.625F, 1.0F);
				particle->orientation = Math::Random(256) << 24;

				float v = Math::RandomFloat(0.003F);
				const Vector2D& cs = trig[Math::Random(256)];
				particle->velocity = velocity + tangent * (v * cs.x) + bitangent * (v * cs.y);

				float f = Math::RandomFloat(0.25F);
				particle->position = broomPosition + tangent * (f * cs.x) + bitangent * (f * cs.y);
				AddParticle(particle);
			}
		}
	}

	Particle *particle = GetFirstParticle();
	while (particle)
	{
		Particle *next = particle->nextParticle;

		int32 life = (particle->lifeTime -= dt);
		if (life > 0)
		{
			if (life < 250)
			{
				particle->color.alpha = (float) life * 0.004F;
			}

			particle->velocity *= Exp(fdt * -0.002F);
			particle->position += particle->velocity * fdt;
		}
		else
		{
			FreeParticle(particle);
		}

		particle = next;
	}
}

// ZYUQURM
