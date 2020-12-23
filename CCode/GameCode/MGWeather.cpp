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


#include "MGWeather.h"
#include "MGFireball.h"
#include "MGGame.h"


using namespace C4;


RainParticleSystem::RainParticleSystem(ParticleSystemType type) :
		LineParticleSystem(type, &particlePool, "particle/Spark1"),
		particlePool(kMaxParticleCount, particleArray)
{
	rainIntensity = 0.001F;
	rainRadius.Set(0.025F, 0.05F);
	rainColor.Set(ColorRGBA(0.25F, 0.75F, 1.0F, 0.5F), ColorRGBA(1.0F, 1.0F, 1.0F, 0.5F));
}

RainParticleSystem::RainParticleSystem(const RainParticleSystem& rainParticleSystem) :
		LineParticleSystem(rainParticleSystem, &particlePool),
		particlePool(kMaxParticleCount, particleArray)
{
	rainIntensity = rainParticleSystem.rainIntensity;
	rainRadius = rainParticleSystem.rainRadius;
	rainColor = rainParticleSystem.rainColor;
}

RainParticleSystem::~RainParticleSystem()
{
}

Node *RainParticleSystem::Replicate(void) const
{
	return (new RainParticleSystem(*this));
}

void RainParticleSystem::Pack(Packer& data, unsigned_int32 packFlags) const
{
	LineParticleSystem::Pack(data, packFlags);

	data << ChunkHeader('rain', 4);
	data << rainIntensity;

	data << ChunkHeader('radi', sizeof(Range<float>));
	data << rainRadius;

	data << ChunkHeader('colr', sizeof(Range<ColorRGBA>));
	data << rainColor;

	data << TerminatorChunk;
}

void RainParticleSystem::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	LineParticleSystem::Unpack(data, unpackFlags);
	UnpackChunkList<RainParticleSystem>(data, unpackFlags);
}

bool RainParticleSystem::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'rain':

			data >> rainIntensity;
			return (true);

		case 'radi':

			data >> rainRadius;
			return (true);

		case 'colr':

			data >> rainColor;
			return (true);
	}

	return (false);
}

int32 RainParticleSystem::GetCategoryCount(void) const
{
	return (LineParticleSystem::GetCategoryCount() + 1);
}

Type RainParticleSystem::GetCategoryType(int32 index, const char **title) const
{
	if (index == 0)
	{
		*title = TheGame->GetStringTable()->GetString(StringID('PART', kParticleSystemRain));
		return (kParticleSystemRain);
	}

	return (LineParticleSystem::GetCategoryType(index - 1, title));
}

int32 RainParticleSystem::GetCategorySettingCount(Type category) const
{
	if (category == kParticleSystemRain)
	{
		return (6); 
	}

	return (LineParticleSystem::GetCategorySettingCount(category)); 
}
 
Setting *RainParticleSystem::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kParticleSystemRain) 
	{
		const StringTable *table = TheGame->GetStringTable(); 
 
		if (index == 0)
		{
			const char *title = table->GetString(StringID('PART', kParticleSystemRain, 'SETT'));
			return (new HeadingSetting(kParticleSystemRain, title)); 
		}

		if (index == 1)
		{
			const char *title = table->GetString(StringID('PART', kParticleSystemRain, 'INTS'));
			return (new TextSetting('INTS', rainIntensity * 1000.0F, title));
		}

		if (index == 2)
		{
			const char *title = table->GetString(StringID('PART', kParticleSystemRain, 'RMIN'));
			return (new TextSetting('RMIN', rainRadius.min, title));
		}

		if (index == 3)
		{
			const char *title = table->GetString(StringID('PART', kParticleSystemRain, 'RMAX'));
			return (new TextSetting('RMAX', rainRadius.max, title));
		}

		if (index == 4)
		{
			const char *title = table->GetString(StringID('PART', kParticleSystemRain, 'CMIN'));
			const char *picker = table->GetString(StringID('PART', kParticleSystemRain, 'PICK'));
			return (new ColorSetting('CMIN', rainColor.min, title, picker, kColorPickerAlpha));
		}

		if (index == 5)
		{
			const char *title = table->GetString(StringID('PART', kParticleSystemRain, 'CMAX'));
			const char *picker = table->GetString(StringID('PART', kParticleSystemRain, 'PICK'));
			return (new ColorSetting('CMAX', rainColor.max, title, picker, kColorPickerAlpha));
		}

		return (nullptr);
	}

	return (LineParticleSystem::GetCategorySetting(category, index, flags));
}

void RainParticleSystem::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kParticleSystemRain)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'INTS')
		{
			rainIntensity = FmaxZero(Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText()) * 0.001F);
		}
		else if (identifier == 'RMIN')
		{
			rainRadius.min = FmaxZero(Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText()));
		}
		else if (identifier == 'RMAX')
		{
			rainRadius.max = Fmax(Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText()), rainRadius.min);
		}
		else if (identifier == 'CMIN')
		{
			rainColor.min = static_cast<const ColorSetting *>(setting)->GetColor();
		}
		else if (identifier == 'CMAX')
		{
			rainColor.max = static_cast<const ColorSetting *>(setting)->GetColor();
		}
	}
	else
	{
		LineParticleSystem::SetCategorySetting(category, setting);
	}
}

void RainParticleSystem::AnimateParticles(void)
{
	const Emitter *emitter = GetConnectedEmitter();
	if (emitter)
	{
		float fdt = TheTimeMgr->GetFloatDeltaTime();
		float bottom = emitter->GetWorldPosition().z;

		RainParticle *particle = static_cast<RainParticle *>(GetFirstParticle());
		while (particle)
		{
			RainParticle *next = particle->GetNextParticle();

			particle->position += particle->velocity * fdt;
			if (particle->position.z < bottom)
			{
				FreeParticle(particle);
			}

			particle = next;
		}

		const EmitterObject *object = emitter->GetObject();
		const Transform4D& transform = emitter->GetWorldTransform();

		int32 count = (int32) (rainIntensity * fdt * object->GetEmitterSurfaceArea());
		for (machine a = 0; a < count; a++)
		{
			particle = particlePool.NewParticle();
			if (!particle)
			{
				break;
			}

			particle->emitTime = 0;
			particle->lifeTime = 0;
			particle->radius = Math::RandomFloat(rainRadius.max - rainRadius.min) + rainRadius.min;
			particle->deathHeight = 0.0F;

			float f = Math::RandomFloat(1.0F);
			particle->color = rainColor.min * (1.0F - f) + rainColor.max * f;

			particle->position = transform * object->GetTopSurfaceEmissionPoint();
			particle->velocity.Set(Math::RandomFloat(0.002F) - 0.001F, Math::RandomFloat(0.002F) - 0.001F, -Math::RandomFloat(0.02F) - 0.04F);

			AddParticle(particle);
		}
	}
}


BurningFlamesParticleSystem::BurningFlamesParticleSystem() :
		FireParticleSystem(kParticleSystemBurningFlames, &particlePool, "fire/Flame"),
		particlePool(kMaxParticleCount, particleArray)
{
}

BurningFlamesParticleSystem::~BurningFlamesParticleSystem()
{
}

bool BurningFlamesParticleSystem::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	// Bounding sphere is the same as the BurningRain particle system that should be this system's parent.
	return (GetSuperNode()->CalculateBoundingSphere(sphere));
}

void BurningFlamesParticleSystem::NewFlame(const Point3D& position)
{
	FireParticle *particle = particlePool.NewParticle();
	if (particle)
	{
		particle->emitTime = 0;
		particle->lifeTime = 6000;

		particle->color.Set(1.0F, 1.0F, 1.0F, 1.0F);
		particle->position = position;

		particle->intensity = 0.25F;
		particle->noiseOffset.Set(Math::RandomFloat(1.0F), Math::RandomFloat(1.0F));
		FireTextureAttribute::CalculateNoiseVelocities(16, particle->noiseVelocity);

		AddParticle(particle);
	}
}

void BurningFlamesParticleSystem::AnimateParticles(void)
{
	int32 dt = TheTimeMgr->GetDeltaTime();

	Particle *particle = GetFirstParticle();
	while (particle)
	{
		Particle *next = particle->nextParticle;

		int32 time = (particle->lifeTime -= dt);
		if (time > 0)
		{
			float h = 0.5F;
			if (time > 5750)
			{
				h = (6000.0F - (float) time) * 0.002F;
			}
			else if (time < 4000)
			{
				h = (float) time * 0.000125F;
			}

			particle->radius = h * 0.25F;
			particle->height = h;
		}
		else
		{
			FreeParticle(particle);
		}

		particle = next;
	}
}


BurningRainParticleSystem::BurningRainParticleSystem() : RainParticleSystem(kParticleSystemBurningRain)
{
	burningRainFlags = kBurningRainFireballs;
	burningRainPercentage = 12;

	minFireballTime = 1000;
	maxFireballTime = 2000;

	burningFlames = nullptr;
}

BurningRainParticleSystem::BurningRainParticleSystem(const BurningRainParticleSystem& burningRainParticleSystem) : RainParticleSystem(kParticleSystemBurningRain)
{
	burningRainFlags = burningRainParticleSystem.burningRainFlags;
	burningRainPercentage = burningRainParticleSystem.burningRainPercentage;

	minFireballTime = burningRainParticleSystem.minFireballTime;
	maxFireballTime = burningRainParticleSystem.minFireballTime;

	burningFlames = nullptr;
}

BurningRainParticleSystem::~BurningRainParticleSystem()
{
	if (burningFlames)
	{
		burningFlames->Finalize();
	}
}

Node *BurningRainParticleSystem::Replicate(void) const
{
	return (new BurningRainParticleSystem(*this));
}

void BurningRainParticleSystem::Pack(Packer& data, unsigned_int32 packFlags) const
{
	RainParticleSystem::Pack(data, packFlags);

	data << ChunkHeader('FLAG', 4);
	data << burningRainFlags;

	data << ChunkHeader('BURN', 4);
	data << burningRainPercentage;

	data << ChunkHeader('TIME', 8);
	data << minFireballTime;
	data << maxFireballTime;

	if ((burningFlames) && (burningFlames->LinkedNodePackable(packFlags)))
	{
		data << ChunkHeader('FLAM', 4);
		data << burningFlames->GetNodeIndex();
	}

	data << TerminatorChunk;
}

void BurningRainParticleSystem::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	RainParticleSystem::Unpack(data, unpackFlags);
	UnpackChunkList<BurningRainParticleSystem>(data, unpackFlags);
}

bool BurningRainParticleSystem::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> burningRainFlags;
			return (true);

		case 'BURN':

			data >> burningRainPercentage;
			return (true);

		case 'TIME':

			data >> minFireballTime;
			data >> maxFireballTime;
			return (true);

		case 'FLAM':
		{
			int32	nodeIndex;

			data >> nodeIndex;
			data.AddNodeLink(nodeIndex, &BurningFlamesLinkProc, this);
			return (true);
		}
	}

	return (false);
}

void BurningRainParticleSystem::BurningFlamesLinkProc(Node *node, void *cookie)
{
	BurningRainParticleSystem *burningRain = static_cast<BurningRainParticleSystem *>(cookie);
	burningRain->burningFlames = static_cast<BurningFlamesParticleSystem *>(node);
}

int32 BurningRainParticleSystem::GetCategorySettingCount(Type category) const
{
	int32 count = RainParticleSystem::GetCategorySettingCount(category);
	if (category == kParticleSystemRain)
	{
		count += 5;
	}

	return (count);
}

Setting *BurningRainParticleSystem::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kParticleSystemRain)
	{
		int32 i = index - RainParticleSystem::GetCategorySettingCount(category);
		if (i >= 0)
		{
			const StringTable *table = TheGame->GetStringTable();

			if (i == 0)
			{
				const char *title = table->GetString(StringID('PART', kParticleSystemBurningRain, 'SETT'));
				return (new HeadingSetting(kParticleSystemBurningRain, title));
			}

			if (i == 1)
			{
				const char *title = table->GetString(StringID('PART', kParticleSystemBurningRain, 'BURN'));
				return (new IntegerSetting('BURN', burningRainPercentage, title, 1, 100, 1));
			}

			if (i == 2)
			{
				const char *title = table->GetString(StringID('PART', kParticleSystemBurningRain, 'FIRE'));
				return (new BooleanSetting('FIRE', ((burningRainFlags & kBurningRainFireballs) != 0), title));
			}

			if (i == 3)
			{
				const char *title = table->GetString(StringID('PART', kParticleSystemBurningRain, 'TMIN'));
				return (new TextSetting('TMIN', (float) minFireballTime * 0.001F, title));
			}

			if (i == 4)
			{
				const char *title = table->GetString(StringID('PART', kParticleSystemBurningRain, 'TMAX'));
				return (new TextSetting('TMAX', (float) maxFireballTime * 0.001F, title));
			}

			return (nullptr);
		}
	}

	return (RainParticleSystem::GetCategorySetting(category, index, flags));
}

void BurningRainParticleSystem::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kParticleSystemRain)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'BURN')
		{
			burningRainPercentage = static_cast<const IntegerSetting *>(setting)->GetIntegerValue();
		}
		else if (identifier == 'FIRE')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				burningRainFlags |= kBurningRainFireballs;
			}
			else
			{
				burningRainFlags &= ~kBurningRainFireballs;
			}
		}
		else if (identifier == 'TMIN')
		{
			const char *text = static_cast<const TextSetting *>(setting)->GetText();
			minFireballTime = (int32) (Text::StringToFloat(text) * 1000.0F + 0.5F);
		}
		else if (identifier == 'TMAX')
		{
			const char *text = static_cast<const TextSetting *>(setting)->GetText();
			maxFireballTime = (int32) (Text::StringToFloat(text) * 1000.0F + 0.5F);
		}
		else
		{
			RainParticleSystem::SetCategorySetting(kParticleSystemRain, setting);
		}
	}
	else
	{
		RainParticleSystem::SetCategorySetting(category, setting);
	}
}

void BurningRainParticleSystem::Preprocess(void)
{
	RainParticleSystem::Preprocess();

	if (!GetManipulator())
	{
		fireballTime = Math::Random(maxFireballTime - minFireballTime) + minFireballTime;

		if (!burningFlames)
		{
			burningFlames = new BurningFlamesParticleSystem;
			burningFlames->SetEffectFlags(GetEffectFlags());
			AppendNewSubnode(burningFlames);
		}
	}
}

void BurningRainParticleSystem::Move(void)
{
	RainParticleSystem::Move();

	if ((burningRainFlags & kBurningRainFireballs) && (Enabled()) && ((fireballTime -= TheTimeMgr->GetDeltaTime()) < 0))
	{
		fireballTime = Math::Random(maxFireballTime - minFireballTime) + minFireballTime;

		const Emitter *emitter = GetConnectedEmitter();
		if (emitter)
		{
			Point3D position = emitter->GetWorldTransform() * emitter->GetObject()->GetTopSurfaceEmissionPoint();
			Vector3D direction(Math::RandomFloat(1.0F) - 0.5F, Math::RandomFloat(1.0F) - 0.5F, -1.0F);

			int32 projectileIndex = GetWorld()->NewControllerIndex();
			TheMessageMgr->SendMessageAll(CreateFireballMessage(projectileIndex, kControllerUnassigned, position, direction.Normalize() * 0.04F, 1.0F));
		}
	}
}

void BurningRainParticleSystem::AnimateParticles(void)
{
	const Emitter *emitter = GetConnectedEmitter();
	if (emitter)
	{
		float fdt = TheTimeMgr->GetFloatDeltaTime();

		RainParticle *particle = static_cast<RainParticle *>(GetFirstParticle());
		while (particle)
		{
			RainParticle *next = particle->GetNextParticle();

			particle->position += particle->velocity * fdt;
			if (particle->position.z < particle->deathHeight)
			{
				if (particle->lifeTime != 0)
				{
					particle->position.z = particle->deathHeight;
					burningFlames->NewFlame(particle->position);
				}

				FreeParticle(particle);
			}

			particle = next;
		}

		const EmitterObject *object = emitter->GetObject();
		const Transform4D& transform = emitter->GetWorldTransform();

		float bottom = emitter->GetWorldPosition().z;
		int32 count = (int32) (GetRainIntensity() * fdt * object->GetEmitterSurfaceArea());
		for (machine a = 0; a < count; a++)
		{
			particle = particlePool.NewParticle();
			if (!particle)
			{
				break;
			}

			particle->emitTime = 0;
			particle->deathHeight = bottom;

			float minRadius = GetMinRainRadius();
			particle->radius = Math::RandomFloat(GetMaxRainRadius() - minRadius) + minRadius;

			particle->position = transform * object->GetTopSurfaceEmissionPoint();
			particle->velocity.Set(Math::RandomFloat(0.002F) - 0.001F, Math::RandomFloat(0.002F) - 0.001F, -Math::RandomFloat(0.02F) - 0.04F);

			if (Math::Random(100) >= burningRainPercentage)
			{
				particle->lifeTime = 0;
				float f = Math::RandomFloat(1.0F);
				particle->color = GetMinRainColor() * (1.0F - f) + GetMaxRainColor() * f;
			}
			else
			{
				CollisionData	data;

				if (GetWorld()->DetectCollision(particle->position, particle->position + particle->velocity * 5000.0F, 0.0F, kCollisionProjectile, &data))
				{
					particle->deathHeight = data.position.z;
					particle->lifeTime = 1;
				}

				particle->color.Set(1.0F, 1.0F, 0.5F, 1.0F);
			}

			AddParticle(particle);
		}
	}
}


GroundFogParticleSystem::GroundFogParticleSystem() :
		TexcoordPointParticleSystem(kParticleSystemGroundFog, &particlePool, "particle/Puff1"),
		particlePool(kMaxParticleCount, particleArray)
{
	groundFogFlags = kGroundFogSoftDepth | kGroundFogDepthRamp | kGroundFogInitWarm;
	groundFogColor.Set(1.0F, 1.0F, 1.0F, 0.25F);

	deltaDepthScale = 0.25F;
	depthRampRange.Set(0.125F, 1.0F);

	emissionRate = 1.0F;
	lifeTimeRange.Set(12000, 16000);
	fadeInTime = 1000;
	fadeOutTime = 2000;

	radiusRange.Set(0.75F, 1.0F);
	expansionRateRange.Set(0.00025F, 0.00025F);
	rotationRateRange.Set(-0.000125F, 0.000125F);

	verticalVelocityRange.Set(0.000125F, 0.00025F);
	horizontalVelocityRange.Set(0.0F, 0.0F);
}

GroundFogParticleSystem::GroundFogParticleSystem(const GroundFogParticleSystem& groundFogParticleSystem) :
		TexcoordPointParticleSystem(groundFogParticleSystem, &particlePool),
		particlePool(kMaxParticleCount, particleArray)
{
	groundFogFlags = groundFogParticleSystem.groundFogFlags;
	groundFogColor = groundFogParticleSystem.groundFogColor;

	deltaDepthScale = groundFogParticleSystem.deltaDepthScale;
	depthRampRange = groundFogParticleSystem.depthRampRange;

	emissionRate = groundFogParticleSystem.emissionRate;
	lifeTimeRange = groundFogParticleSystem.lifeTimeRange;
	fadeInTime = groundFogParticleSystem.fadeInTime;
	fadeOutTime = groundFogParticleSystem.fadeOutTime;

	radiusRange = groundFogParticleSystem.radiusRange;
	expansionRateRange = groundFogParticleSystem.expansionRateRange;
	rotationRateRange = groundFogParticleSystem.rotationRateRange;

	verticalVelocityRange = groundFogParticleSystem.verticalVelocityRange;
	horizontalVelocityRange = groundFogParticleSystem.horizontalVelocityRange;
}

GroundFogParticleSystem::~GroundFogParticleSystem()
{
}

Node *GroundFogParticleSystem::Replicate(void) const
{
	return (new GroundFogParticleSystem(*this));
}

void GroundFogParticleSystem::Pack(Packer& data, unsigned_int32 packFlags) const
{
	TexcoordPointParticleSystem::Pack(data, packFlags);

	data << ChunkHeader('FLAG', 4);
	data << groundFogFlags;

	data << ChunkHeader('COLR', sizeof(ColorRGBA));
	data << groundFogColor;

	data << ChunkHeader('DLTA', 4);
	data << deltaDepthScale;

	data << ChunkHeader('RAMP', sizeof(Range<float>));
	data << depthRampRange;

	data << ChunkHeader('EMIT', 4);
	data << emissionRate;

	data << ChunkHeader('LIFE', sizeof(Range<int32>));
	data << lifeTimeRange;

	data << ChunkHeader('FADE', 8);
	data << fadeInTime;
	data << fadeOutTime;

	data << ChunkHeader('RADI', sizeof(Range<float>));
	data << radiusRange;

	data << ChunkHeader('EXPN', sizeof(Range<float>));
	data << expansionRateRange;

	data << ChunkHeader('ROTA', sizeof(Range<float>));
	data << rotationRateRange;

	data << ChunkHeader('ZVEL', sizeof(Range<float>));
	data << verticalVelocityRange;

	data << ChunkHeader('XVEL', sizeof(Range<float>));
	data << horizontalVelocityRange;

	data << TerminatorChunk;
}

void GroundFogParticleSystem::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	TexcoordPointParticleSystem::Unpack(data, unpackFlags);
	UnpackChunkList<GroundFogParticleSystem>(data, unpackFlags);
}

bool GroundFogParticleSystem::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> groundFogFlags;
			return (true);

		case 'COLR':

			data >> groundFogColor;
			return (true);

		case 'DLTA':

			data >> deltaDepthScale;
			return (true);

		case 'RAMP':

			data >> depthRampRange;
			return (true);

		case 'EMIT':

			data >> emissionRate;
			return (true);

		case 'LIFE':

			data >> lifeTimeRange;
			return (true);

		case 'FADE':

			data >> fadeInTime;
			data >> fadeOutTime;
			return (true);

		case 'RADI':

			data >> radiusRange;
			return (true);

		case 'EXPN':

			data >> expansionRateRange;
			return (true);

		case 'ROTA':

			data >> rotationRateRange;
			return (true);

		case 'ZVEL':

			data >> verticalVelocityRange;
			return (true);

		case 'XVEL':

			data >> horizontalVelocityRange;
			return (true);
	}

	return (false);
}

int32 GroundFogParticleSystem::GetCategoryCount(void) const
{
	return (TexcoordPointParticleSystem::GetCategoryCount() + 1);
}

Type GroundFogParticleSystem::GetCategoryType(int32 index, const char **title) const
{
	if (index == 0)
	{
		*title = TheGame->GetStringTable()->GetString(StringID('PART', kParticleSystemGroundFog));
		return (kParticleSystemGroundFog);
	}

	return (TexcoordPointParticleSystem::GetCategoryType(index - 1, title));
}

int32 GroundFogParticleSystem::GetCategorySettingCount(Type category) const
{
	if (category == kParticleSystemGroundFog)
	{
		return (26);
	}

	return (TexcoordPointParticleSystem::GetCategorySettingCount(category));
}

Setting *GroundFogParticleSystem::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kParticleSystemGroundFog)
	{
		const StringTable *table = TheGame->GetStringTable();

		if (index == 0)
		{
			const char *title = table->GetString(StringID('PART', kParticleSystemGroundFog, 'FSET'));
			return (new HeadingSetting('FSET', title));
		}

		if (index == 1)
		{
			const char *title = table->GetString(StringID('PART', kParticleSystemGroundFog, 'COLR'));
			const char *picker = table->GetString(StringID('PART', kParticleSystemGroundFog, 'PICK'));
			return (new ColorSetting('COLR', groundFogColor, title, picker, kColorPickerAlpha));
		}

		if (index == 2)
		{
			const char *title = table->GetString(StringID('PART', kParticleSystemGroundFog, 'SOFT'));
			return (new BooleanSetting('SOFT', ((groundFogFlags & kGroundFogSoftDepth) != 0), title));
		}

		if (index == 3)
		{
			const char *title = table->GetString(StringID('PART', kParticleSystemGroundFog, 'DSCL'));
			return (new TextSetting('DSCL', deltaDepthScale, title));
		}

		if (index == 4)
		{
			const char *title = table->GetString(StringID('PART', kParticleSystemGroundFog, 'RAMP'));
			return (new BooleanSetting('RAMP', ((groundFogFlags & kGroundFogDepthRamp) != 0), title));
		}

		if (index == 5)
		{
			const char *title = table->GetString(StringID('PART', kParticleSystemGroundFog, 'DMIN'));
			return (new TextSetting('DMIN', depthRampRange.min, title));
		}

		if (index == 6)
		{
			const char *title = table->GetString(StringID('PART', kParticleSystemGroundFog, 'DMAX'));
			return (new TextSetting('DMAX', depthRampRange.max, title));
		}

		if (index == 7)
		{
			const char *title = table->GetString(StringID('PART', kParticleSystemGroundFog, 'ESET'));
			return (new HeadingSetting('ESET', title));
		}

		if (index == 8)
		{
			const char *title = table->GetString(StringID('PART', kParticleSystemGroundFog, 'EMIT'));
			return (new TextSetting('EMIT', emissionRate, title));
		}

		if (index == 9)
		{
			const char *title = table->GetString(StringID('PART', kParticleSystemGroundFog, 'LMIN'));
			return (new TextSetting('LMIN', (float) lifeTimeRange.min * 0.001F, title));
		}

		if (index == 10)
		{
			const char *title = table->GetString(StringID('PART', kParticleSystemGroundFog, 'LMAX'));
			return (new TextSetting('LMAX', (float) lifeTimeRange.max * 0.001F, title));
		}

		if (index == 11)
		{
			const char *title = table->GetString(StringID('PART', kParticleSystemGroundFog, 'FADI'));
			return (new TextSetting('FADI', (float) fadeInTime * 0.001F, title));
		}

		if (index == 12)
		{
			const char *title = table->GetString(StringID('PART', kParticleSystemGroundFog, 'FADO'));
			return (new TextSetting('FADO', (float) fadeOutTime * 0.001F, title));
		}

		if (index == 13)
		{
			const char *title = table->GetString(StringID('PART', kParticleSystemGroundFog, 'WARM'));
			return (new BooleanSetting('WARM', ((groundFogFlags & kGroundFogInitWarm) != 0), title));
		}

		if (index == 14)
		{
			const char *title = table->GetString(StringID('PART', kParticleSystemGroundFog, 'PSET'));
			return (new HeadingSetting('PSET', title));
		}

		if (index == 15)
		{
			const char *title = table->GetString(StringID('PART', kParticleSystemGroundFog, 'RMIN'));
			return (new TextSetting('RMIN', radiusRange.min, title));
		}

		if (index == 16)
		{
			const char *title = table->GetString(StringID('PART', kParticleSystemGroundFog, 'RMAX'));
			return (new TextSetting('RMAX', radiusRange.max, title));
		}

		if (index == 17)
		{
			const char *title = table->GetString(StringID('PART', kParticleSystemGroundFog, 'EMIN'));
			return (new TextSetting('EMIN', expansionRateRange.min * 1000.0F, title));
		}

		if (index == 18)
		{
			const char *title = table->GetString(StringID('PART', kParticleSystemGroundFog, 'EMAX'));
			return (new TextSetting('EMAX', expansionRateRange.max * 1000.0F, title));
		}

		if (index == 19)
		{
			const char *title = table->GetString(StringID('PART', kParticleSystemGroundFog, 'WMIN'));
			return (new TextSetting('WMIN', rotationRateRange.min * 1000.0F, title));
		}

		if (index == 20)
		{
			const char *title = table->GetString(StringID('PART', kParticleSystemGroundFog, 'WMAX'));
			return (new TextSetting('WMAX', rotationRateRange.max * 1000.0F, title));
		}

		if (index == 21)
		{
			const char *title = table->GetString(StringID('PART', kParticleSystemGroundFog, 'VSET'));
			return (new HeadingSetting('VSET', title));
		}

		if (index == 22)
		{
			const char *title = table->GetString(StringID('PART', kParticleSystemGroundFog, 'ZMIN'));
			return (new TextSetting('ZMIN', verticalVelocityRange.min * 1000.0F, title));
		}

		if (index == 23)
		{
			const char *title = table->GetString(StringID('PART', kParticleSystemGroundFog, 'ZMAX'));
			return (new TextSetting('ZMAX', verticalVelocityRange.max * 1000.0F, title));
		}

		if (index == 24)
		{
			const char *title = table->GetString(StringID('PART', kParticleSystemGroundFog, 'XMIN'));
			return (new TextSetting('XMIN', horizontalVelocityRange.min * 1000.0F, title));
		}

		if (index == 25)
		{
			const char *title = table->GetString(StringID('PART', kParticleSystemGroundFog, 'XMAX'));
			return (new TextSetting('XMAX', horizontalVelocityRange.max * 1000.0F, title));
		}

		return (nullptr);
	}

	return (TexcoordPointParticleSystem::GetCategorySetting(category, index, flags));
}

void GroundFogParticleSystem::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kParticleSystemGroundFog)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'COLR')
		{
			groundFogColor = static_cast<const ColorSetting *>(setting)->GetColor();
		}
		else if (identifier == 'SOFT')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				groundFogFlags |= kGroundFogSoftDepth;
			}
			else
			{
				groundFogFlags &= ~kGroundFogSoftDepth;
			}
		}
		else if (identifier == 'DSCL')
		{
			deltaDepthScale = FmaxZero(Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText()));
		}
		else if (identifier == 'RAMP')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				groundFogFlags |= kGroundFogDepthRamp;
			}
			else
			{
				groundFogFlags &= ~kGroundFogDepthRamp;
			}
		}
		else if (identifier == 'DMIN')
		{
			depthRampRange.min = FmaxZero(Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText()));
		}
		else if (identifier == 'DMAX')
		{
			depthRampRange.max = Fmax(Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText()), depthRampRange.min);
		}
		else if (identifier == 'EMIT')
		{
			emissionRate = FmaxZero(Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText()));
		}
		else if (identifier == 'LMIN')
		{
			lifeTimeRange.min = MaxZero((int32) (Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText()) * 1000.0F + 0.5F));
		}
		else if (identifier == 'LMAX')
		{
			lifeTimeRange.max = Max((int32) (Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText()) * 1000.0F + 0.5F), lifeTimeRange.min);
		}
		else if (identifier == 'FADI')
		{
			fadeInTime = MaxZero((int32) (Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText()) * 1000.0F + 0.5F));
		}
		else if (identifier == 'FADO')
		{
			fadeOutTime = MaxZero((int32) (Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText()) * 1000.0F + 0.5F));
		}
		else if (identifier == 'WARM')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				groundFogFlags |= kGroundFogInitWarm;
			}
			else
			{
				groundFogFlags &= ~kGroundFogInitWarm;
			}
		}
		else if (identifier == 'RMIN')
		{
			radiusRange.min = FmaxZero(Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText()));
		}
		else if (identifier == 'RMAX')
		{
			radiusRange.max = Fmax(Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText()), radiusRange.min);
		}
		else if (identifier == 'EMIN')
		{
			expansionRateRange.min = FmaxZero(Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText()) * 0.001F);
		}
		else if (identifier == 'EMAX')
		{
			expansionRateRange.max = Fmax(Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText()) * 0.001F, expansionRateRange.min);
		}
		else if (identifier == 'WMIN')
		{
			rotationRateRange.min = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText()) * 0.001F;
		}
		else if (identifier == 'WMAX')
		{
			rotationRateRange.max = Fmax(Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText()) * 0.001F, rotationRateRange.min);
		}
		else if (identifier == 'ZMIN')
		{
			verticalVelocityRange.min = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText()) * 0.001F;
		}
		else if (identifier == 'ZMAX')
		{
			verticalVelocityRange.max = Fmax(Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText()) * 0.001F, verticalVelocityRange.min);
		}
		else if (identifier == 'XMIN')
		{
			horizontalVelocityRange.min = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText()) * 0.001F;
		}
		else if (identifier == 'XMAX')
		{
			horizontalVelocityRange.max = Fmax(Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText()) * 0.001F, horizontalVelocityRange.min);
		}
	}
	else
	{
		TexcoordPointParticleSystem::SetCategorySetting(category, setting);
	}
}

void GroundFogParticleSystem::Preprocess(void)
{
	unsigned_int32 flags = kParticleSystemFreezeInvisible;

	if (groundFogFlags & kGroundFogSoftDepth)
	{
		flags |= kParticleSystemSoftDepth;
		SetSoftDepthScale(deltaDepthScale);
	}

	if (groundFogFlags & kGroundFogDepthRamp)
	{
		flags |= kParticleSystemDepthRamp;
		SetDepthRampRange(depthRampRange);
	}

	SetParticleSystemFlags(flags);
	TexcoordPointParticleSystem::Preprocess();

	SetEffectListIndex(kEffectListFrontmost);
	SetAmbientBlendState(kBlendInterpolate | kBlendAlphaPreserve);

	inverseFadeInTime = 1.0F / (float) fadeInTime;
	inverseFadeOutTime = 1.0F / (float) fadeOutTime;

	const Emitter *emitter = GetConnectedEmitter();
	if (emitter)
	{
		emissionAccum = 0.0F;
		areaEmissionRate = emitter->GetObject()->GetEmitterSurfaceArea() * (emissionRate * 0.001F);

		if ((!GetManipulator()) && (groundFogFlags & kGroundFogInitWarm))
		{
			groundFogFlags &= ~kGroundFogInitWarm;

			const EmitterObject *object = emitter->GetObject();
			const Transform4D& transform = emitter->GetWorldTransform();

			int32 count = Min((int32) ((float) (lifeTimeRange.min + lifeTimeRange.max) * (areaEmissionRate * 0.5F)), kMaxParticleCount);
			for (machine a = 0; a < count; a++)
			{
				FogParticle *particle = particlePool.NewParticle();
				InitParticle(particle);

				int32 t = Math::Random(particle->lifeTime);
				int32 life = particle->lifeTime - t;
				float ft = (float) t;

				particle->emitTime = -t;
				particle->lifeTime = life;

				float alpha = groundFogColor.alpha;
				if (t < fadeInTime)
				{
					alpha *= ft * inverseFadeInTime;
				}
				else if (life < fadeOutTime)
				{
					alpha *= (float) life * inverseFadeOutTime;
				}

				particle->color.alpha = alpha;

				particle->radius += particle->expansionRate * ft;
				particle->velocity = Math::RandomFloat(horizontalVelocityRange) * transform[0] + Math::RandomFloat(verticalVelocityRange) * transform[2];
				particle->position = transform * object->GetBottomSurfaceEmissionPoint() + particle->velocity * ft;

				AddParticle(particle);
			}
		}
	}
}

void GroundFogParticleSystem::InitParticle(FogParticle *particle) const
{
	particle->lifeTime = Math::Random(lifeTimeRange);
	particle->radius = Math::RandomFloat(radiusRange);
	particle->color.GetColorRGB() = groundFogColor.GetColorRGB();
	particle->orientation = Math::Random(256) << 24;

	int32 i = Math::Random(4);
	particle->texcoordScale.Set(0.5F, 0.5F);
	particle->texcoordOffset.Set((i & 1) ? 0.5F : 0.0F, (i & 2) ? 0.5F : 0.0F);

	particle->expansionRate = Math::RandomFloat(expansionRateRange);
	particle->angularVelocity = (Fixed) (Math::RandomFloat(rotationRateRange) * 16777216.0F);
}

void GroundFogParticleSystem::AnimateParticles(void)
{
	const Emitter *emitter = GetConnectedEmitter();
	if (emitter)
	{
		int32 dt = TheTimeMgr->GetDeltaTime();
		float fdt = TheTimeMgr->GetFloatDeltaTime();

		FogParticle *particle = static_cast<FogParticle *>(GetFirstParticle());
		while (particle)
		{
			FogParticle *next = particle->GetNextParticle();

			int32 life = (particle->lifeTime -= dt);
			if (life > 0)
			{
				particle->radius += particle->expansionRate * fdt;
				particle->orientation += (particle->angularVelocity * dt) << 8;
				particle->position += particle->velocity * fdt;

				float alpha = groundFogColor.alpha;
				int32 emit = -(particle->emitTime -= dt);
				if (emit < fadeInTime)
				{
					alpha *= (float) emit * inverseFadeInTime;
				}
				else if (life < fadeOutTime)
				{
					alpha *= (float) life * inverseFadeOutTime;
				}

				particle->color.alpha = alpha;
			}
			else
			{
				FreeParticle(particle);
			}

			particle = next;
		}

		float accum = emissionAccum + fdt * areaEmissionRate;
		float count = PositiveFloor(accum);
		emissionAccum = accum - count;

		if (count > 0.0F)
		{
			const EmitterObject *object = emitter->GetObject();
			const Transform4D& transform = emitter->GetWorldTransform();

			do
			{
				particle = particlePool.NewParticle();
				if (!particle)
				{
					break;
				}

				particle->emitTime = -dt;
				particle->color.alpha = groundFogColor.alpha * (fdt * inverseFadeInTime);
				particle->position = transform * object->GetBottomSurfaceEmissionPoint();
				particle->velocity = Math::RandomFloat(horizontalVelocityRange) * transform[0] + Math::RandomFloat(verticalVelocityRange) * transform[2];

				InitParticle(particle);
				AddParticle(particle);
			} while ((count -= 1.0F) > 0.0F);
		}
	}
}

void GroundFogParticleSystem::Render(const FrustumCamera *camera, List<Renderable> *effectList)
{
	if ((TheGraphicsMgr->GetGraphicsActiveFlags() & kGraphicsActiveStructureEffects) != 0)
	{
		TexcoordPointParticleSystem::Render(camera, effectList);
	}
}

// ZYUQURM
