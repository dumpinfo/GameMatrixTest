 

#include "C4Particles.h"
#include "C4World.h"
#include "C4Cameras.h"
#include "C4Configuration.h"


using namespace C4;


namespace
{
	const TextureHeader particleTextureHeader =
	{
		kTexture2D,
		kTextureForceHighQuality,
		kTextureSemanticEmission,
		kTextureSemanticNone,
		kTextureL8,
		16, 16, 1,
		{kTextureClampBorder, kTextureClampBorder, kTextureClamp},
		1
	};


	const unsigned_int8 particleTextureImage[256] =
	{
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x03, 0x03, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x03, 0x05, 0x07, 0x08, 0x09, 0x08, 0x07, 0x05, 0x03, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x03, 0x06, 0x0A, 0x0D, 0x0F, 0x0F, 0x0F, 0x0D, 0x0A, 0x06, 0x03, 0x00, 0x00,
		0x00, 0x00, 0x03, 0x06, 0x0B, 0x0F, 0x14, 0x18, 0x19, 0x18, 0x14, 0x0F, 0x0B, 0x06, 0x03, 0x00,
		0x00, 0x01, 0x05, 0x0A, 0x0F, 0x16, 0x1E, 0x25, 0x27, 0x25, 0x1E, 0x16, 0x0F, 0x0A, 0x05, 0x01,
		0x00, 0x02, 0x07, 0x0D, 0x14, 0x1E, 0x2B, 0x38, 0x3F, 0x38, 0x2B, 0x1E, 0x14, 0x0D, 0x07, 0x02,
		0x00, 0x03, 0x08, 0x0F, 0x18, 0x25, 0x38, 0x56, 0x6F, 0x56, 0x38, 0x25, 0x18, 0x0F, 0x08, 0x03,
		0x00, 0x03, 0x09, 0x0F, 0x19, 0x27, 0x3F, 0x6F, 0xFF, 0x6F, 0x3F, 0x27, 0x19, 0x0F, 0x09, 0x03,
		0x00, 0x03, 0x08, 0x0F, 0x18, 0x25, 0x38, 0x56, 0x6F, 0x56, 0x38, 0x25, 0x18, 0x0F, 0x08, 0x03,
		0x00, 0x02, 0x07, 0x0D, 0x14, 0x1E, 0x2B, 0x38, 0x3F, 0x38, 0x2B, 0x1E, 0x14, 0x0D, 0x07, 0x02,
		0x00, 0x01, 0x05, 0x0A, 0x0F, 0x16, 0x1E, 0x25, 0x27, 0x25, 0x1E, 0x16, 0x0F, 0x0A, 0x05, 0x01,
		0x00, 0x00, 0x03, 0x06, 0x0B, 0x0F, 0x14, 0x18, 0x19, 0x18, 0x14, 0x0F, 0x0B, 0x06, 0x03, 0x00,
		0x00, 0x00, 0x00, 0x03, 0x06, 0x0A, 0x0D, 0x0F, 0x0F, 0x0F, 0x0D, 0x0A, 0x06, 0x03, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x03, 0x05, 0x07, 0x08, 0x09, 0x08, 0x07, 0x05, 0x03, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x03, 0x03, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00
	};
}


const char C4::kConnectorKeyEmitter[] = "%Emitter";


namespace C4
{
	template class Registrable<ParticleSystem, ParticleSystemRegistration>;
}


ResourceDescriptor StarFieldResource::descriptor("sfd");


List<BlobParticleSystem::BlobBatch> BlobParticleSystem::batchList;


StarFieldResource::StarFieldResource(const char *name, ResourceCatalog *catalog) : Resource<StarFieldResource>(name, catalog)
{
}

StarFieldResource::~StarFieldResource()
{
}


ParticleSystemRegistration::ParticleSystemRegistration(ParticleSystemType type, const char *name) : Registration<ParticleSystem, ParticleSystemRegistration>(type)
{
	particleSystemName = name;
}

ParticleSystemRegistration::~ParticleSystemRegistration()
{
}


ParticlePoolBase::ParticlePoolBase(int32 count, Particle *pool, unsigned_int32 size)
{
	particlePool = pool;
	particleDataSize = size;

	totalParticleCount = count;
	activeParticleCount = 0;

	firstFreeParticle = nullptr;
	lastFreeParticle = nullptr;
}

ParticlePoolBase::~ParticlePoolBase()
{
}

void ParticlePoolBase::Reset(void)
{
	activeParticleCount = 0;
	firstFreeParticle = nullptr;
	lastFreeParticle = nullptr; 
}

Particle *ParticlePoolBase::NewParticle(void) 
{
	Particle *particle = firstFreeParticle; 
	if (particle)
	{
		Particle *next = particle->nextParticle; 
		if (next)
		{ 
			firstFreeParticle = next; 
			next->prevParticle = nullptr;
		}
		else
		{ 
			firstFreeParticle = nullptr;
			lastFreeParticle = nullptr;
		}

		return (particle);
	}

	int32 count = activeParticleCount;
	if (count < totalParticleCount)
	{
		particle = reinterpret_cast<Particle *>(reinterpret_cast<char *>(particlePool) + count * particleDataSize);
		particle->particleIndex = count;
		activeParticleCount = count + 1;
		return (particle);
	}

	return (nullptr);
}


ParticleSystem::ParticleSystem(ParticleSystemType type, ParticlePoolBase *pool, RenderType renderType, unsigned_int32 renderState) :
		Effect(kEffectParticleSystem, renderType, renderState),
		deltaDepthAttribute(1.0F)
{
	particleSystemType = type;
	particleSystemFlags = 0;
	particlePool = pool;

	materialObject = nullptr;
	Initialize();
}

ParticleSystem::ParticleSystem(const ParticleSystem& particleSystem, ParticlePoolBase *pool) :
		Effect(particleSystem),
		deltaDepthAttribute(particleSystem.deltaDepthAttribute)
{
	particleSystemType = particleSystem.particleSystemType;
	particleSystemFlags = particleSystem.particleSystemFlags;
	particlePool = pool;

	materialObject = particleSystem.materialObject;
	if (materialObject)
	{
		materialObject->Retain();
	}

	Initialize();
}

ParticleSystem::~ParticleSystem()
{
	if (materialObject)
	{
		materialObject->Release();
	}
}

void ParticleSystem::Initialize(void)
{
	animateFlag = false;

	connectedEmitter = nullptr;

	firstUsedParticle = nullptr;
	lastUsedParticle = nullptr;

	boundingBoxPointer = nullptr;
}

ParticleSystem::ParticleJob::ParticleJob(ExecuteProc *execProc, FinalizeProc *finalProc, void *cookie) : BatchJob(execProc, finalProc, cookie)
{
}

ParticleSystem *ParticleSystem::New(ParticleSystemType type)
{
	Type	data[2];

	data[0] = type;
	data[1] = 0;

	Unpacker unpacker(data);
	return (Registrable<ParticleSystem, ParticleSystemRegistration>::Create(unpacker));
}

void ParticleSystem::RegisterStandardParticleSystems(void)
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	static ParticleSystemReg<StarField> starFieldRegistration(kParticleSystemStarField, table->GetString(StringID('PTCL', kParticleSystemStarField)));
}

void ParticleSystem::PackType(Packer& data) const
{
	Effect::PackType(data);
	data << particleSystemType;
}

void ParticleSystem::Prepack(List<Object> *linkList) const
{
	Node::Prepack(linkList);

	if (materialObject)
	{
		linkList->Append(materialObject);
	}
}

void ParticleSystem::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Effect::Pack(data, packFlags);

	data << ChunkHeader('FLAG', 4);
	data << particleSystemFlags;

	if ((materialObject) && (!(packFlags & kPackSettings)))
	{
		data << ChunkHeader('MATL', 4);
		data << materialObject->GetObjectIndex();
	}

	if (particleSystemFlags & kParticleSystemSoftDepth)
	{
		data << ChunkHeader('DLTA', 4);
		data << deltaDepthAttribute.GetDeltaScale();
	}

	if (particleSystemFlags & kParticleSystemDepthRamp)
	{
		data << ChunkHeader('RAMP', sizeof(Range<float>));
		data << depthRampAttribute.GetRampRange();
	}

	if ((!GetManipulator()) && (!(particleSystemFlags & kParticleSystemNonpersistent)))
	{
		int32 count = 0;
		const Particle *particle = firstUsedParticle;
		while (particle)
		{
			count++;
			particle = particle->nextParticle;
		}

		if (count != 0)
		{
			PackHandle handle = data.BeginChunk('PART');
			data << count;
			particlePool->PackParticlePool(data, firstUsedParticle);
			data.EndChunk(handle);
		}
	}

	data << TerminatorChunk;
}

void ParticleSystem::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Effect::Unpack(data, unpackFlags);
	UnpackChunkList<ParticleSystem>(data, unpackFlags);
}

bool ParticleSystem::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> particleSystemFlags;
			return (true);

		case 'MATL':
		{
			int32	objectIndex;

			data >> objectIndex;
			data.AddObjectLink(objectIndex, &MaterialObjectLinkProc, this);
			return (true);
		}

		case 'DLTA':
		{
			float	scale;

			data >> scale;
			deltaDepthAttribute.SetDeltaScale(scale);
			return (true);
		}

		case 'RAMP':
		{
			Range<float>	range;

			data >> range;
			depthRampAttribute.SetRampRange(range);
			return (true);
		}

		case 'PART':
		{
			int32	count;

			data >> count;
			particlePool->activeParticleCount = count;
			particlePool->UnpackParticlePool(data, count, &firstUsedParticle, &lastUsedParticle);
			return (true);
		}
	}

	return (false);
}

void ParticleSystem::MaterialObjectLinkProc(Object *object, void *cookie)
{
	ParticleSystem *particleSystem = static_cast<ParticleSystem *>(cookie);
	particleSystem->SetMaterialObject(static_cast<MaterialObject *>(object));
}

bool ParticleSystem::CalculateBoundingBox(Box3D *box) const
{
	const Box3D *boundingBox = boundingBoxPointer;
	if (boundingBox)
	{
		*box = *boundingBox;
		return (true);
	}

	return (false);
}

bool ParticleSystem::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	const Emitter *emitter = connectedEmitter;
	if (emitter)
	{
		emitter->GetObject()->CalculateBoundingSphere(sphere);
		sphere->SetCenter(GetInverseWorldTransform() * (emitter->GetWorldTransform() * sphere->GetCenter()));
		return (true);
	}

	sphere->SetCenter(0.0F, 0.0F, 0.0F);
	sphere->SetRadius(0.0F);
	return (true);
}

int32 ParticleSystem::GetInternalConnectorCount(void) const
{
	return (1);
}

const char *ParticleSystem::GetInternalConnectorKey(int32 index) const
{
	if (index == 0)
	{
		return (kConnectorKeyEmitter);
	}

	return (nullptr);
}

void ParticleSystem::ProcessInternalConnectors(void)
{
	Node *node = GetConnectedNode(kConnectorKeyEmitter);
	if (node)
	{
		Emitter *emitter = static_cast<Emitter *>(node);
		connectedEmitter = emitter;

		emitter->GetObject()->CalculateBoundingBox(&defaultBoundingBox);
		defaultBoundingBox.Transform(GetInverseWorldTransform() * emitter->GetWorldTransform());
	}
	else
	{
		connectedEmitter = nullptr;

		defaultBoundingBox.min.Set(0.0F, 0.0F, 0.0F);
		defaultBoundingBox.max.Set(0.0F, 0.0F, 0.0F);
	}
}

bool ParticleSystem::ValidConnectedNode(const ConnectorKey& key, const Node *node) const
{
	if (key == kConnectorKeyEmitter)
	{
		return (node->GetNodeType() == kNodeEmitter);
	}

	return (Effect::ValidConnectedNode(key, node));
}

void ParticleSystem::SetConnectedEmitter(Emitter *emitter)
{
	connectedEmitter = emitter;
	SetConnectedNode(kConnectorKeyEmitter, emitter);
}

void ParticleSystem::SetMaterialObject(MaterialObject *object)
{
	if (materialObject != object)
	{
		if (materialObject)
		{
			materialObject->Release();
		}

		if (object)
		{
			object->Retain();
		}

		materialObject = object;
	}
}

void ParticleSystem::Invalidate(void)
{
	if (particleSystemFlags & kParticleSystemUnfreezeDynamic)
	{
		animateFlag = true;
	}

	Effect::Invalidate();
}

void ParticleSystem::Preprocess(void)
{
	Effect::Preprocess();
	Wake();

	boundingBoxPointer = &defaultBoundingBox;

	SetVisibilityProc(&BoxVisible);
	SetOcclusionProc(&BoxVisible);

	SetAmbientBlendState(BlendState(kBlendSourceAlpha, kBlendOne));
	SetShaderFlags(kShaderAmbientEffect);

	RenderSegment *segment = GetFirstRenderSegment();
	segment->SetMaterialState(kMaterialTwoSided);
	segment->SetMaterialObjectPointer(&materialObject);
	segment->SetMaterialAttributeList(&attributeList);

	unsigned_int32 flags = particleSystemFlags;

	if (!(flags & kParticleSystemFreezeInvisible))
	{
		animateFlag = true;
	}

	if (flags & kParticleSystemSoftDepth)
	{
		attributeList.Append(&deltaDepthAttribute);
	}

	if (flags & kParticleSystemDepthRamp)
	{
		attributeList.Append(&depthRampAttribute);
	}

	#if C4PS4 //[ PS4

		// -- PS4 code hidden --

	#endif //]
}

void ParticleSystem::Move(void)
{
	if (animateFlag)
	{
		unsigned_int32 flags = particleSystemFlags;
		if (flags & kParticleSystemFreezeInvisible)
		{
			animateFlag = false;
		}

		AnimateParticles();

		const Particle *particle = firstUsedParticle;
		if (particle)
		{
			if (!(flags & kParticleSystemStaticBoundingBox))
			{
				#if C4SIMD

					vec_float min = VecLoadUnaligned(&particle->position.x);
					vec_float max = min;

					vec_float r = VecLoadSmearScalar(&particle->radius);
					min = VecSub(min, r);
					max = VecAdd(max, r);

					for (;;)
					{
						particle = particle->nextParticle;
						if (!particle)
						{
							break;
						}

						vec_float p = VecLoadUnaligned(&particle->position.x);
						r = VecLoadSmearScalar(&particle->radius);

						min = VecMin(min, VecSub(p, r));
						max = VecMax(max, VecAdd(p, r));
					}

					if (!(flags & kParticleSystemObjectSpace))
					{
						SetWorldBoundingBox(min, max);
						boundingBoxPointer = nullptr;
					}
					else
					{
						objectBoundingBox.Set(min, max);
						boundingBoxPointer = &objectBoundingBox;
					}

				#else

					Point3D min = particle->position;
					Point3D max = particle->position;

					float r = particle->radius;
					min -= Vector3D(r, r, r);
					max += Vector3D(r, r, r);

					for (;;)
					{
						particle = particle->nextParticle;
						if (!particle)
						{
							break;
						}

						const Point3D& p = particle->position;
						r = particle->radius;

						min.x = Fmin(min.x, p.x - r);
						min.y = Fmin(min.y, p.y - r);
						min.z = Fmin(min.z, p.z - r);

						max.x = Fmax(max.x, p.x + r);
						max.y = Fmax(max.y, p.y + r);
						max.z = Fmax(max.z, p.z + r);
					}

					if (!(flags & kParticleSystemObjectSpace))
					{
						SetWorldBoundingBox(min, max);
						boundingBoxPointer = nullptr;
					}
					else
					{
						objectBoundingBox.Set(min, max);
						boundingBoxPointer = &objectBoundingBox;
					}

				#endif

				InvalidateUpdateFlags(kUpdateVisibility);
			}
		}
		else
		{
			if (flags & kParticleSystemSelfDestruct)
			{
				delete this;
				return;
			}

			boundingBoxPointer = &defaultBoundingBox;
		}
	}
}

void ParticleSystem::AddParticle(Particle *particle)
{
	Particle *last = lastUsedParticle;
	if (last)
	{
		last->nextParticle = particle;
		lastUsedParticle = particle;
		particle->prevParticle = last;
		particle->nextParticle = nullptr;
	}
	else
	{
		firstUsedParticle = particle;
		lastUsedParticle = particle;
		particle->prevParticle = nullptr;
		particle->nextParticle = nullptr;
	}
}

void ParticleSystem::AddFarthestParticle(Particle *particle)
{
	Particle *first = firstUsedParticle;
	if (first)
	{
		first->prevParticle = particle;
		firstUsedParticle = particle;
		particle->prevParticle = nullptr;
		particle->nextParticle = first;
	}
	else
	{
		firstUsedParticle = particle;
		lastUsedParticle = particle;
		particle->prevParticle = nullptr;
		particle->nextParticle = nullptr;
	}
}

void ParticleSystem::FreeParticle(Particle *particle)
{
	Particle *next = particle->nextParticle;
	Particle *prev = particle->prevParticle;

	if (firstUsedParticle == particle)
	{
		firstUsedParticle = next;
	}

	if (lastUsedParticle == particle)
	{
		lastUsedParticle = prev;
	}

	if (next)
	{
		next->prevParticle = prev;
	}

	if (prev)
	{
		prev->nextParticle = next;
	}

	Particle *last = particlePool->lastFreeParticle;
	if (last)
	{
		last->nextParticle = particle;
		particlePool->lastFreeParticle = particle;
		particle->prevParticle = last;
		particle->nextParticle = nullptr;
	}
	else
	{
		particlePool->firstFreeParticle = particle;
		particlePool->lastFreeParticle = particle;
		particle->prevParticle = nullptr;
		particle->nextParticle = nullptr;
	}
}

void ParticleSystem::FreeAllParticles(void)
{
	particlePool->Reset();

	firstUsedParticle = nullptr;
	lastUsedParticle = nullptr;
}

void ParticleSystem::AnimateParticles(void)
{
	int32 dt = TheTimeMgr->GetDeltaTime();
	float fdt = TheTimeMgr->GetFloatDeltaTime();

	Particle *particle = firstUsedParticle;
	while (particle)
	{
		Particle *next = particle->nextParticle;

		if ((particle->emitTime -= dt) <= 0)
		{
			if ((particle->lifeTime -= dt) > 0)
			{
				particle->position += particle->velocity * fdt;
			}
			else
			{
				FreeParticle(particle);
			}
		}

		particle = next;
	}
}


PointParticleSystem::PointParticleSystem(ParticleSystemType type, ParticlePoolBase *pool, const char *textureName) :
		ParticleSystem(type, pool, kRenderQuads, kRenderDepthTest | kRenderDepthInhibit),
		renderJob(&JobRenderParticles, &FinalizeParticles, this),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		textureAttribute(textureName, &particleTextureHeader, particleTextureImage)
{
}

PointParticleSystem::PointParticleSystem(const PointParticleSystem& pointParticleSystem, ParticlePoolBase *pool) :
		ParticleSystem(pointParticleSystem, pool),
		renderJob(&JobRenderParticles, &FinalizeParticles, this),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		textureAttribute(pointParticleSystem.textureAttribute)
{
}

PointParticleSystem::~PointParticleSystem()
{
}

void PointParticleSystem::Preprocess(void)
{
	ParticleSystem::Preprocess();
	AddAttribute(&textureAttribute);

	if (GetParticleSystemFlags() & kParticleSystemPointSprite)
	{
		spriteFlag = true;
		SetRenderType(kRenderPointSprites);

		int32 vertexCount = GetTotalParticleCount();
		vertexBuffer.Establish(vertexCount * sizeof(SpriteVertex));
		SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(SpriteVertex));

		SetVertexAttributeArray(kArrayPosition, 0, 3);
		SetVertexAttributeArray(kArrayColor, sizeof(Point3D), 4);
		SetVertexAttributeArray(kArrayRadius, sizeof(Point3D) + sizeof(ColorRGBA), 1);
	}
	else
	{
		spriteFlag = false;
		SetRenderType(kRenderQuads);
		SetShaderFlags(GetShaderFlags() | kShaderVertexBillboard);

		int32 vertexCount = GetTotalParticleCount() * 4;
		vertexBuffer.Establish(vertexCount * sizeof(PointVertex));
		SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(PointVertex));

		SetVertexAttributeArray(kArrayPosition, 0, 3);
		SetVertexAttributeArray(kArrayColor, sizeof(Point3D), 4);
		SetVertexAttributeArray(kArrayTexcoord, sizeof(Point3D) + sizeof(ColorRGBA), 2);
		SetVertexAttributeArray(kArrayBillboard, sizeof(Point3D) + sizeof(ColorRGBA) + sizeof(Point2D), 4);
	}
}

void PointParticleSystem::JobRenderParticles(Job *job, void *cookie)
{
	const ParticleJob *particleJob = static_cast<ParticleJob *>(job);
	PointParticleSystem *particleSystem = static_cast<PointParticleSystem *>(cookie);
	const FrustumCamera *camera = particleJob->camera;

	int32 vertexCount = 0;

	if (particleSystem->spriteFlag)
	{
		volatile SpriteVertex *restrict vertex = static_cast<volatile SpriteVertex *>(particleJob->attributeBuffer);

		if (!(particleSystem->GetParticleSystemFlags() & kParticleSystemObjectSpace))
		{
			const Particle *particle = particleSystem->GetFirstParticle();
			while (particle)
			{
				if (particle->emitTime <= 0)
				{
					float r = particle->radius;
					if (camera->FrustumCamera::SphereVisible(particle->position, r))
					{
						vertex->position = particle->position;
						vertex->color = particle->color;
						vertex->radius = r;

						vertex++;
						vertexCount++;
					}
				}

				particle = particle->GetNextParticle();
			}
		}
		else
		{
			const Particle *particle = particleSystem->GetFirstParticle();
			while (particle)
			{
				if (particle->emitTime <= 0)
				{
					vertex->position = particle->position;
					vertex->color = particle->color;
					vertex->radius = particle->radius;

					vertex++;
					vertexCount++;
				}

				particle = particle->GetNextParticle();
			}
		}
	}
	else
	{
		volatile PointVertex *restrict vertex = static_cast<volatile PointVertex *>(particleJob->attributeBuffer);

		const ConstVector2D *trig = Math::GetTrigTable();

		if (!(particleSystem->GetParticleSystemFlags() & kParticleSystemObjectSpace))
		{
			const Particle *particle = particleSystem->GetFirstParticle();
			while (particle)
			{
				if (particle->emitTime <= 0)
				{
					float r = particle->radius;
					if (camera->FrustumCamera::SphereVisible(particle->position, r))
					{
						unsigned_int32 angle = particle->orientation >> 24;
						const Vector2D& cs = trig[angle];

						vertex[0].position = particle->position;
						vertex[0].color = particle->color;
						vertex[0].texcoord.Set(0.0F, 0.0F);
						vertex[0].billboard.Set(cs.x, cs.y, -r, r);

						vertex[1].position = particle->position;
						vertex[1].color = particle->color;
						vertex[1].texcoord.Set(1.0F, 0.0F);
						vertex[1].billboard.Set(cs.x, cs.y, r, r);

						vertex[2].position = particle->position;
						vertex[2].color = particle->color;
						vertex[2].texcoord.Set(1.0F, 1.0F);
						vertex[2].billboard.Set(cs.x, cs.y, r, -r);

						vertex[3].position = particle->position;
						vertex[3].color = particle->color;
						vertex[3].texcoord.Set(0.0F, 1.0F);
						vertex[3].billboard.Set(cs.x, cs.y, -r, -r);

						vertex += 4;
						vertexCount += 4;
					}
				}

				particle = particle->GetNextParticle();
			}
		}
		else
		{
			const Particle *particle = particleSystem->GetFirstParticle();
			while (particle)
			{
				if (particle->emitTime <= 0)
				{
					unsigned_int32 angle = particle->orientation >> 24;
					const Vector2D& cs = trig[angle];
					float r = particle->radius;

					vertex[0].position = particle->position;
					vertex[0].color = particle->color;
					vertex[0].texcoord.Set(0.0F, 0.0F);
					vertex[0].billboard.Set(cs.x, cs.y, -r, r);

					vertex[1].position = particle->position;
					vertex[1].color = particle->color;
					vertex[1].texcoord.Set(1.0F, 0.0F);
					vertex[1].billboard.Set(cs.x, cs.y, r, r);

					vertex[2].position = particle->position;
					vertex[2].color = particle->color;
					vertex[2].texcoord.Set(1.0F, 1.0F);
					vertex[2].billboard.Set(cs.x, cs.y, r, -r);

					vertex[3].position = particle->position;
					vertex[3].color = particle->color;
					vertex[3].texcoord.Set(0.0F, 1.0F);
					vertex[3].billboard.Set(cs.x, cs.y, -r, -r);

					vertex += 4;
					vertexCount += 4;
				}

				particle = particle->GetNextParticle();
			}
		}
	}

	particleSystem->SetVertexCount(vertexCount);
}

void PointParticleSystem::FinalizeParticles(Job *job, void *cookie)
{
	PointParticleSystem *particleSystem = static_cast<PointParticleSystem *>(cookie);
	particleSystem->vertexBuffer.EndUpdate();

	if (particleSystem->GetVertexCount() == 0)
	{
		particleSystem->ListElement<Renderable>::GetOwningList()->Remove(particleSystem);
	}
}

void PointParticleSystem::Render(const FrustumCamera *camera, List<Renderable> *effectList)
{
	renderJob.camera = camera;
	renderJob.attributeBuffer = vertexBuffer.BeginUpdate();
	GetWorld()->SubmitWorldJob(&renderJob);

	SetAnimateFlag();
	effectList[GetEffectListIndex()].Append(this);
}


TexcoordPointParticleSystem::TexcoordPointParticleSystem(ParticleSystemType type, ParticlePoolBase *pool, const char *textureName) :
		ParticleSystem(type, pool, kRenderQuads, kRenderDepthTest | kRenderDepthInhibit),
		renderJob(&JobRenderParticles, &FinalizeParticles, this),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		textureAttribute(textureName, &particleTextureHeader, particleTextureImage)
{
}

TexcoordPointParticleSystem::TexcoordPointParticleSystem(const TexcoordPointParticleSystem& texcoordPointParticleSystem, ParticlePoolBase *pool) :
		ParticleSystem(texcoordPointParticleSystem, pool),
		renderJob(&JobRenderParticles, &FinalizeParticles, this),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		textureAttribute(texcoordPointParticleSystem.textureAttribute)
{
}

TexcoordPointParticleSystem::~TexcoordPointParticleSystem()
{
}

void TexcoordPointParticleSystem::Preprocess(void)
{
	ParticleSystem::Preprocess();
	AddAttribute(&textureAttribute);

	SetRenderType(kRenderQuads);
	SetShaderFlags(GetShaderFlags() | kShaderVertexBillboard);

	int32 vertexCount = GetTotalParticleCount() * 4;
	vertexBuffer.Establish(vertexCount * sizeof(PointVertex));
	SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(PointVertex));

	SetVertexAttributeArray(kArrayPosition, 0, 3);
	SetVertexAttributeArray(kArrayColor, sizeof(Point3D), 4);
	SetVertexAttributeArray(kArrayTexcoord, sizeof(Point3D) + sizeof(ColorRGBA), 2);
	SetVertexAttributeArray(kArrayBillboard, sizeof(Point3D) + sizeof(ColorRGBA) + sizeof(Point2D), 4);
}

void TexcoordPointParticleSystem::JobRenderParticles(Job *job, void *cookie)
{
	const ParticleJob *particleJob = static_cast<ParticleJob *>(job);
	TexcoordPointParticleSystem *particleSystem = static_cast<TexcoordPointParticleSystem *>(cookie);
	const FrustumCamera *camera = particleJob->camera;

	int32 vertexCount = 0;
	volatile PointVertex *restrict vertex = static_cast<volatile PointVertex *>(particleJob->attributeBuffer);

	const ConstVector2D *trig = Math::GetTrigTable();

	if (!(particleSystem->GetParticleSystemFlags() & kParticleSystemObjectSpace))
	{
		const TexcoordParticle *particle = static_cast<TexcoordParticle *>(particleSystem->GetFirstParticle());
		while (particle)
		{
			if (particle->emitTime <= 0)
			{
				float r = particle->radius;
				if (camera->FrustumCamera::SphereVisible(particle->position, r))
				{
					float s1 = particle->texcoordOffset.x;
					float s2 = particle->texcoordScale.x + s1;
					float t1 = particle->texcoordOffset.y;
					float t2 = particle->texcoordScale.y + t1;

					unsigned_int32 angle = particle->orientation >> 24;
					const Vector2D& cs = trig[angle];

					vertex[0].position = particle->position;
					vertex[0].color = particle->color;
					vertex[0].texcoord.Set(s1, t1);
					vertex[0].billboard.Set(cs.x, cs.y, -r, r);

					vertex[1].position = particle->position;
					vertex[1].color = particle->color;
					vertex[1].texcoord.Set(s2, t1);
					vertex[1].billboard.Set(cs.x, cs.y, r, r);

					vertex[2].position = particle->position;
					vertex[2].color = particle->color;
					vertex[2].texcoord.Set(s2, t2);
					vertex[2].billboard.Set(cs.x, cs.y, r, -r);

					vertex[3].position = particle->position;
					vertex[3].color = particle->color;
					vertex[3].texcoord.Set(s1, t2);
					vertex[3].billboard.Set(cs.x, cs.y, -r, -r);

					vertex += 4;
					vertexCount += 4;
				}
			}

			particle = particle->GetNextParticle();
		}
	}
	else
	{
		const TexcoordParticle *particle = static_cast<TexcoordParticle *>(particleSystem->GetFirstParticle());
		while (particle)
		{
			if (particle->emitTime <= 0)
			{
				float s1 = particle->texcoordOffset.x;
				float s2 = particle->texcoordScale.x + s1;
				float t1 = particle->texcoordOffset.y;
				float t2 = particle->texcoordScale.y + t1;

				unsigned_int32 angle = particle->orientation >> 24;
				const Vector2D& cs = trig[angle];
				float r = particle->radius;

				vertex[0].position = particle->position;
				vertex[0].color = particle->color;
				vertex[0].texcoord.Set(s1, t1);
				vertex[0].billboard.Set(cs.x, cs.y, -r, r);

				vertex[1].position = particle->position;
				vertex[1].color = particle->color;
				vertex[1].texcoord.Set(s2, t1);
				vertex[1].billboard.Set(cs.x, cs.y, r, r);

				vertex[2].position = particle->position;
				vertex[2].color = particle->color;
				vertex[2].texcoord.Set(s2, t2);
				vertex[2].billboard.Set(cs.x, cs.y, r, -r);

				vertex[3].position = particle->position;
				vertex[3].color = particle->color;
				vertex[3].texcoord.Set(s1, t2);
				vertex[3].billboard.Set(cs.x, cs.y, -r, -r);

				vertex += 4;
				vertexCount += 4;
			}

			particle = particle->GetNextParticle();
		}
	}

	particleSystem->SetVertexCount(vertexCount);
}

void TexcoordPointParticleSystem::FinalizeParticles(Job *job, void *cookie)
{
	TexcoordPointParticleSystem *particleSystem = static_cast<TexcoordPointParticleSystem *>(cookie);
	particleSystem->vertexBuffer.EndUpdate();

	if (particleSystem->GetVertexCount() == 0)
	{
		particleSystem->ListElement<Renderable>::GetOwningList()->Remove(particleSystem);
	}
}

void TexcoordPointParticleSystem::Render(const FrustumCamera *camera, List<Renderable> *effectList)
{
	renderJob.camera = camera;
	renderJob.attributeBuffer = vertexBuffer.BeginUpdate();
	GetWorld()->SubmitWorldJob(&renderJob);

	SetAnimateFlag();
	effectList[GetEffectListIndex()].Append(this);
}


InfinitePointParticleSystem::InfinitePointParticleSystem(ParticleSystemType type, ParticlePoolBase *pool, const char *textureName) :
		ParticleSystem(type, pool, kRenderQuads, kRenderDepthTest | kRenderDepthInhibit),
		renderJob(&JobRenderParticles, &FinalizeParticles, this),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		textureAttribute(textureName, &particleTextureHeader, particleTextureImage)
{
}

InfinitePointParticleSystem::InfinitePointParticleSystem(const InfinitePointParticleSystem& infinitePointParticleSystem, ParticlePoolBase *pool) :
		ParticleSystem(infinitePointParticleSystem, pool),
		renderJob(&JobRenderParticles, &FinalizeParticles, this),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		textureAttribute(infinitePointParticleSystem.textureAttribute)
{
}

InfinitePointParticleSystem::~InfinitePointParticleSystem()
{
}

void InfinitePointParticleSystem::Preprocess(void)
{
	ParticleSystem::Preprocess();
	SetNodeFlags(GetNodeFlags() | kNodeInfiniteVisibility);

	AddAttribute(&textureAttribute);

	if (GetParticleSystemFlags() & kParticleSystemPointSprite)
	{
		spriteFlag = true;
		SetRenderType(kRenderPointSprites);
		SetShaderFlags(GetShaderFlags() | kShaderVertexInfinite);

		int32 vertexCount = GetTotalParticleCount();
		vertexBuffer.Establish(vertexCount * sizeof(SpriteVertex));
		SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(SpriteVertex));

		SetVertexAttributeArray(kArrayPosition, 0, 3);
		SetVertexAttributeArray(kArrayColor, sizeof(Point3D), 4);
		SetVertexAttributeArray(kArrayRadius, sizeof(Point3D) + sizeof(ColorRGBA), 1);
	}
	else
	{
		spriteFlag = false;
		SetRenderType(kRenderQuads);
		SetShaderFlags(GetShaderFlags() | (kShaderVertexInfinite | kShaderVertexBillboard));

		int32 vertexCount = GetTotalParticleCount() * 4;
		vertexBuffer.Establish(vertexCount * sizeof(PointVertex));
		SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(PointVertex));

		SetVertexAttributeArray(kArrayPosition, 0, 3);
		SetVertexAttributeArray(kArrayColor, sizeof(Point3D), 4);
		SetVertexAttributeArray(kArrayTexcoord, sizeof(Point3D) + sizeof(ColorRGBA), 2);
		SetVertexAttributeArray(kArrayBillboard, sizeof(Point3D) + sizeof(ColorRGBA) + sizeof(Point2D), 2);
	}
}

void InfinitePointParticleSystem::JobRenderParticles(Job *job, void *cookie)
{
	const ParticleJob *particleJob = static_cast<ParticleJob *>(job);
	InfinitePointParticleSystem *particleSystem = static_cast<InfinitePointParticleSystem *>(cookie);
	const FrustumCamera *camera = particleJob->camera;

	int32 vertexCount = 0;

	if (particleSystem->spriteFlag)
	{
		volatile SpriteVertex *restrict vertex = static_cast<volatile SpriteVertex *>(particleJob->attributeBuffer);

		if (!(particleSystem->GetParticleSystemFlags() & kParticleSystemObjectSpace))
		{
			const Particle *particle = particleSystem->GetFirstParticle();
			while (particle)
			{
				if (particle->emitTime <= 0)
				{
					float r = particle->radius;
					if (camera->DirectionVisible(particle->position, r))
					{
						vertex->position = particle->position;
						vertex->color = particle->color;
						vertex->radius = r;

						vertex++;
						vertexCount++;
					}
				}

				particle = particle->GetNextParticle();
			}
		}
		else
		{
			const Particle *particle = particleSystem->GetFirstParticle();
			while (particle)
			{
				if (particle->emitTime <= 0)
				{
					vertex->position = particle->position;
					vertex->color = particle->color;
					vertex->radius = particle->radius;

					vertex++;
					vertexCount++;
				}

				particle = particle->GetNextParticle();
			}
		}
	}
	else
	{
		volatile PointVertex *restrict vertex = static_cast<volatile PointVertex *>(particleJob->attributeBuffer);

		const ConstVector2D *trig = Math::GetTrigTable();

		if (!(particleSystem->GetParticleSystemFlags() & kParticleSystemObjectSpace))
		{
			const Particle *particle = particleSystem->GetFirstParticle();
			while (particle)
			{
				if (particle->emitTime <= 0)
				{
					float r = particle->radius;
					if (camera->DirectionVisible(particle->position, r))
					{
						unsigned_int32 angle = particle->orientation >> 24;
						Vector2D cs = trig[angle] * r;
						float u = cs.y + cs.x;
						float v = cs.y - cs.x;

						vertex[0].position = particle->position;
						vertex[0].color = particle->color;
						vertex[0].texcoord.Set(0.0F, 0.0F);
						vertex[0].billboard.Set(-u, -v);

						vertex[1].position = particle->position;
						vertex[1].color = particle->color;
						vertex[1].texcoord.Set(1.0F, 0.0F);
						vertex[1].billboard.Set(-v, u);

						vertex[2].position = particle->position;
						vertex[2].color = particle->color;
						vertex[2].texcoord.Set(1.0F, 1.0F);
						vertex[2].billboard.Set(u, v);

						vertex[3].position = particle->position;
						vertex[3].color = particle->color;
						vertex[3].texcoord.Set(0.0F, 1.0F);
						vertex[3].billboard.Set(v, -u);

						vertex += 4;
						vertexCount += 4;
					}
				}

				particle = particle->GetNextParticle();
			}
		}
		else
		{
			const Particle *particle = particleSystem->GetFirstParticle();
			while (particle)
			{
				if (particle->emitTime <= 0)
				{
					unsigned_int32 angle = particle->orientation >> 24;
					Vector2D cs = trig[angle] * particle->radius;
					float u = cs.y + cs.x;
					float v = cs.y - cs.x;

					vertex[0].position = particle->position;
					vertex[0].color = particle->color;
					vertex[0].texcoord.Set(0.0F, 0.0F);
					vertex[0].billboard.Set(-u, -v);

					vertex[1].position = particle->position;
					vertex[1].color = particle->color;
					vertex[1].texcoord.Set(1.0F, 0.0F);
					vertex[1].billboard.Set(-v, u);

					vertex[2].position = particle->position;
					vertex[2].color = particle->color;
					vertex[2].texcoord.Set(1.0F, 1.0F);
					vertex[2].billboard.Set(u, v);

					vertex[3].position = particle->position;
					vertex[3].color = particle->color;
					vertex[3].texcoord.Set(0.0F, 1.0F);
					vertex[3].billboard.Set(v, -u);

					vertex += 4;
					vertexCount += 4;
				}

				particle = particle->GetNextParticle();
			}
		}
	}

	particleSystem->SetVertexCount(vertexCount);
}

void InfinitePointParticleSystem::FinalizeParticles(Job *job, void *cookie)
{
	InfinitePointParticleSystem *particleSystem = static_cast<InfinitePointParticleSystem *>(cookie);
	particleSystem->vertexBuffer.EndUpdate();

	if (particleSystem->GetVertexCount() == 0)
	{
		particleSystem->ListElement<Renderable>::GetOwningList()->Remove(particleSystem);
	}
}

void InfinitePointParticleSystem::Render(const FrustumCamera *camera, List<Renderable> *effectList)
{
	renderJob.camera = camera;
	renderJob.attributeBuffer = vertexBuffer.BeginUpdate();
	GetWorld()->SubmitWorldJob(&renderJob);

	SetAnimateFlag();
	effectList[GetEffectListIndex()].Append(this);
}


LineParticleSystem::LineParticleSystem(ParticleSystemType type, ParticlePoolBase *pool, const char *textureName) :
		ParticleSystem(type, pool, kRenderQuads, kRenderDepthTest | kRenderDepthInhibit),
		renderJob(&JobRenderParticles, &FinalizeParticles, this),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		textureAttribute(textureName, &particleTextureHeader, particleTextureImage)
{
	lengthMultiplier = 1.0F;
}

LineParticleSystem::LineParticleSystem(const LineParticleSystem& lineParticleSystem, ParticlePoolBase *pool) :
		ParticleSystem(lineParticleSystem, pool),
		renderJob(&JobRenderParticles, &FinalizeParticles, this),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		textureAttribute(lineParticleSystem.textureAttribute)
{
	lengthMultiplier = lineParticleSystem.lengthMultiplier;
}

LineParticleSystem::~LineParticleSystem()
{
}

void LineParticleSystem::Pack(Packer& data, unsigned_int32 packFlags) const
{
	ParticleSystem::Pack(data, packFlags);

	data << ChunkHeader('LMUL', 4);
	data << lengthMultiplier;

	data << TerminatorChunk;
}

void LineParticleSystem::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	ParticleSystem::Unpack(data, unpackFlags);
	UnpackChunkList<LineParticleSystem>(data, unpackFlags);
}

bool LineParticleSystem::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'LMUL':

			data >> lengthMultiplier;
			return (true);
	}

	return (false);
}

void LineParticleSystem::Preprocess(void)
{
	ParticleSystem::Preprocess();
	AddAttribute(&textureAttribute);

	int32 vertexCount = GetTotalParticleCount() * 4;
	vertexBuffer.Establish(vertexCount * sizeof(LineVertex));
	SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(LineVertex));

	SetVertexAttributeArray(kArrayPosition, 0, 3);
	SetVertexAttributeArray(kArrayColor, sizeof(Point3D), 4);
	SetVertexAttributeArray(kArrayTangent, sizeof(Point3D) + sizeof(ColorRGBA), 4);
	SetVertexAttributeArray(kArrayTexcoord, sizeof(Point3D) + sizeof(ColorRGBA) + sizeof(Vector4D), 2);

	SetShaderFlags(GetShaderFlags() | (kShaderVertexPolyboard | kShaderLinearPolyboard));
}

void LineParticleSystem::JobRenderParticles(Job *job, void *cookie)
{
	const ParticleJob *particleJob = static_cast<ParticleJob *>(job);
	LineParticleSystem *particleSystem = static_cast<LineParticleSystem *>(cookie);
	const FrustumCamera *camera = particleJob->camera;

	int32 vertexCount = 0;
	volatile LineVertex *restrict vertex = static_cast<volatile LineVertex *>(particleJob->attributeBuffer);

	float halfTime = particleSystem->GetWorld()->GetVelocityNormalizationTime() * (particleSystem->lengthMultiplier * 0.5F);

	if (!(particleSystem->GetParticleSystemFlags() & kParticleSystemObjectSpace))
	{
		const Particle *particle = particleSystem->GetFirstParticle();
		while (particle)
		{
			if (particle->emitTime <= 0)
			{
				Vector3D halfTravel = particle->velocity * halfTime;
				float squaredMag = SquaredMag(halfTravel);
				float inverseMag = InverseSqrt(squaredMag);
				float halfDistance = squaredMag * inverseMag;

				float radius = particle->radius;
				float halfLength = halfDistance + radius;
				Point3D center = particle->position - halfTravel;

				if (camera->FrustumCamera::SphereVisible(center, halfLength))
				{
					Vector3D direction = halfTravel * inverseMag;
					Vector3D halfLengthTravel = direction * halfLength;

					Point3D p1 = center - halfLengthTravel;
					Point3D p2 = center + halfLengthTravel;

					vertex[0].position = p1;
					vertex[0].color = particle->color;
					vertex[0].tangent.Set(direction, -radius);
					vertex[0].texcoord.Set(0.0F, 0.0F);

					vertex[1].position = p1;
					vertex[1].color = particle->color;
					vertex[1].tangent.Set(direction, radius);
					vertex[1].texcoord.Set(0.0F, 1.0F);

					vertex[2].position = p2;
					vertex[2].color = particle->color;
					vertex[2].tangent.Set(direction, radius);
					vertex[2].texcoord.Set(1.0F, 1.0F);

					vertex[3].position = p2;
					vertex[3].color = particle->color;
					vertex[3].tangent.Set(direction, -radius);
					vertex[3].texcoord.Set(1.0F, 0.0F);

					vertex += 4;
					vertexCount += 4;
				}
			}

			particle = particle->GetNextParticle();
		}
	}
	else
	{
		const Particle *particle = particleSystem->GetFirstParticle();
		while (particle)
		{
			if (particle->emitTime <= 0)
			{
				Vector3D halfTravel = particle->velocity * halfTime;
				float squaredMag = SquaredMag(halfTravel);
				float inverseMag = InverseSqrt(squaredMag);
				float halfDistance = squaredMag * inverseMag;

				float radius = particle->radius;
				float halfLength = halfDistance + radius;
				Point3D center = particle->position - halfTravel;

				Vector3D direction = halfTravel * inverseMag;
				Vector3D halfLengthTravel = direction * halfLength;

				Point3D p1 = center - halfLengthTravel;
				Point3D p2 = center + halfLengthTravel;

				vertex[0].position = p1;
				vertex[0].color = particle->color;
				vertex[0].tangent.Set(direction, -radius);
				vertex[0].texcoord.Set(0.0F, 0.0F);

				vertex[1].position = p1;
				vertex[1].color = particle->color;
				vertex[1].tangent.Set(direction, radius);
				vertex[1].texcoord.Set(0.0F, 1.0F);

				vertex[2].position = p2;
				vertex[2].color = particle->color;
				vertex[2].tangent.Set(direction, radius);
				vertex[2].texcoord.Set(1.0F, 1.0F);

				vertex[3].position = p2;
				vertex[3].color = particle->color;
				vertex[3].tangent.Set(direction, -radius);
				vertex[3].texcoord.Set(1.0F, 0.0F);

				vertex += 4;
				vertexCount += 4;
			}

			particle = particle->GetNextParticle();
		}
	}

	particleSystem->SetVertexCount(vertexCount);
}

void LineParticleSystem::FinalizeParticles(Job *job, void *cookie)
{
	LineParticleSystem *particleSystem = static_cast<LineParticleSystem *>(cookie);
	particleSystem->vertexBuffer.EndUpdate();

	if (particleSystem->GetVertexCount() == 0)
	{
		particleSystem->ListElement<Renderable>::GetOwningList()->Remove(particleSystem);
	}
}

void LineParticleSystem::Render(const FrustumCamera *camera, List<Renderable> *effectList)
{
	renderJob.camera = camera;
	renderJob.attributeBuffer = vertexBuffer.BeginUpdate();
	GetWorld()->SubmitWorldJob(&renderJob);

	SetAnimateFlag();
	effectList[GetEffectListIndex()].Append(this);
}


QuadParticleSystem::QuadParticleSystem(ParticleSystemType type, ParticlePoolBase *pool, const char *textureName) :
		ParticleSystem(type, pool, kRenderQuads, kRenderDepthTest | kRenderDepthInhibit),
		renderJob(&JobRenderParticles, &FinalizeParticles, this),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		textureAttribute(textureName, &particleTextureHeader, particleTextureImage)
{
}

QuadParticleSystem::QuadParticleSystem(const QuadParticleSystem& quadParticleSystem, ParticlePoolBase *pool) :
		ParticleSystem(quadParticleSystem, pool),
		renderJob(&JobRenderParticles, &FinalizeParticles, this),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		textureAttribute(quadParticleSystem.textureAttribute)
{
}

QuadParticleSystem::~QuadParticleSystem()
{
}

void QuadParticleSystem::Preprocess(void)
{
	ParticleSystem::Preprocess();
	AddAttribute(&textureAttribute);

	int32 vertexCount = GetTotalParticleCount() * 4;
	vertexBuffer.Establish(vertexCount * sizeof(QuadVertex));
	SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(QuadVertex));

	SetVertexAttributeArray(kArrayPosition, 0, 3);
	SetVertexAttributeArray(kArrayColor, sizeof(Point3D), 4);
	SetVertexAttributeArray(kArrayTexcoord, sizeof(Point3D) + sizeof(ColorRGBA), 2);
}

void QuadParticleSystem::JobRenderParticles(Job *job, void *cookie)
{
	const ParticleJob *particleJob = static_cast<ParticleJob *>(job);
	QuadParticleSystem *particleSystem = static_cast<QuadParticleSystem *>(cookie);
	const FrustumCamera *camera = particleJob->camera;

	int32 vertexCount = 0;
	volatile QuadVertex *restrict vertex = static_cast<volatile QuadVertex *>(particleJob->attributeBuffer);

	if (!(particleSystem->GetParticleSystemFlags() & kParticleSystemObjectSpace))
	{
		const QuadParticle *particle = static_cast<const QuadParticle *>(particleSystem->GetFirstParticle());
		while (particle)
		{
			if (particle->emitTime <= 0)
			{
				float r = particle->radius;
				if (camera->FrustumCamera::SphereVisible(particle->position, r))
				{
					Vector3D xdir = particle->transform.GetDirectionX() * r;
					Vector3D ydir = particle->transform.GetDirectionY() * r;
					Vector3D v1 = xdir + ydir;
					Vector3D v2 = xdir - ydir;

					vertex[0].position = particle->position - v1;
					vertex[0].color = particle->color;
					vertex[0].texcoord.Set(0.0F, 0.0F);

					vertex[1].position = particle->position + v2;
					vertex[1].color = particle->color;
					vertex[1].texcoord.Set(1.0F, 0.0F);

					vertex[2].position = particle->position + v1;
					vertex[2].color = particle->color;
					vertex[2].texcoord.Set(1.0F, 1.0F);

					vertex[3].position = particle->position - v2;
					vertex[3].color = particle->color;
					vertex[3].texcoord.Set(0.0F, 1.0F);

					vertex += 4;
					vertexCount += 4;
				}
			}

			particle = particle->GetNextParticle();
		}
	}
	else
	{
		const QuadParticle *particle = static_cast<const QuadParticle *>(particleSystem->GetFirstParticle());
		while (particle)
		{
			if (particle->emitTime <= 0)
			{
				float r = particle->radius;
				Vector3D xdir = particle->transform.GetDirectionX() * r;
				Vector3D ydir = particle->transform.GetDirectionY() * r;
				Vector3D v1 = xdir + ydir;
				Vector3D v2 = xdir - ydir;

				vertex[0].position = particle->position - v1;
				vertex[0].color = particle->color;
				vertex[0].texcoord.Set(0.0F, 0.0F);

				vertex[1].position = particle->position + v2;
				vertex[1].color = particle->color;
				vertex[1].texcoord.Set(1.0F, 0.0F);

				vertex[2].position = particle->position + v1;
				vertex[2].color = particle->color;
				vertex[2].texcoord.Set(1.0F, 1.0F);

				vertex[3].position = particle->position - v2;
				vertex[3].color = particle->color;
				vertex[3].texcoord.Set(0.0F, 1.0F);

				vertex += 4;
				vertexCount += 4;
			}

			particle = particle->GetNextParticle();
		}
	}

	particleSystem->SetVertexCount(vertexCount);
}

void QuadParticleSystem::FinalizeParticles(Job *job, void *cookie)
{
	QuadParticleSystem *particleSystem = static_cast<QuadParticleSystem *>(cookie);
	particleSystem->vertexBuffer.EndUpdate();

	if (particleSystem->GetVertexCount() == 0)
	{
		particleSystem->ListElement<Renderable>::GetOwningList()->Remove(particleSystem);
	}
}

void QuadParticleSystem::Render(const FrustumCamera *camera, List<Renderable> *effectList)
{
	renderJob.camera = camera;
	renderJob.attributeBuffer = vertexBuffer.BeginUpdate();
	GetWorld()->SubmitWorldJob(&renderJob);

	SetAnimateFlag();
	effectList[GetEffectListIndex()].Append(this);
}


FireParticleSystem::FireParticleSystem(ParticleSystemType type, ParticlePoolBase *pool, const char *textureName) :
		ParticleSystem(type, pool, kRenderQuads, kRenderDepthTest | kRenderDepthInhibit),
		renderJob(&JobRenderParticles, &FinalizeParticles, this),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		textureAttribute(textureName, &particleTextureHeader, particleTextureImage),
		noiseAttribute("C4/noise")
{
}

FireParticleSystem::FireParticleSystem(const FireParticleSystem& fireParticleSystem, ParticlePoolBase *pool) :
		ParticleSystem(fireParticleSystem, pool),
		renderJob(&JobRenderParticles, &FinalizeParticles, this),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		textureAttribute(fireParticleSystem.textureAttribute),
		noiseAttribute("C4/noise")
{
}

FireParticleSystem::~FireParticleSystem()
{
}

void FireParticleSystem::Preprocess(void)
{
	ParticleSystem::Preprocess();
	AddAttribute(&textureAttribute);
	AddAttribute(&noiseAttribute);

	int32 vertexCount = GetTotalParticleCount() * 4;
	vertexBuffer.Establish(vertexCount * sizeof(FireVertex));
	SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(FireVertex));

	SetVertexAttributeArray(kArrayPosition, 0, 3);
	SetVertexAttributeArray(kArrayColor, sizeof(Point3D), 4);
	SetVertexAttributeArray(kArrayRadius, sizeof(Point3D) + sizeof(ColorRGBA), 4);
	SetVertexAttributeArray(kArrayTexcoord0, sizeof(Point3D) + sizeof(ColorRGBA) + sizeof(Vector4D), 4);
	SetVertexAttributeArray(kArrayTexcoord1, sizeof(Point3D) + sizeof(ColorRGBA) + sizeof(Vector4D) * 2, 4);

	SetShaderFlags(GetShaderFlags() | (kShaderFireArrays | kShaderVertexPostboard));
}

void FireParticleSystem::JobRenderParticles(Job *job, void *cookie)
{
	const ParticleJob *particleJob = static_cast<ParticleJob *>(job);
	FireParticleSystem *particleSystem = static_cast<FireParticleSystem *>(cookie);
	const FrustumCamera *camera = particleJob->camera;

	int32 vertexCount = 0;
	volatile FireVertex *restrict vertex = static_cast<volatile FireVertex *>(particleJob->attributeBuffer);

	if (!(particleSystem->GetParticleSystemFlags() & kParticleSystemObjectSpace))
	{
		const FireParticle *particle = static_cast<FireParticle *>(particleSystem->GetFirstParticle());
		while (particle)
		{
			if (particle->emitTime <= 0)
			{
				float r = particle->radius;
				float h = particle->height * 0.5F;

				const Vector2D& offset = particle->noiseOffset;
				const Vector2D *velocity = particle->noiseVelocity;

				const Point3D& p = particle->position;
				if (camera->FrustumCamera::SphereVisible(Point3D(p.x, p.y, p.z + h), Sqrt(r * r + h * h)))
				{
					float z = p.z + h * 2.0F;

					vertex[0].position = p;
					vertex[0].color = particle->color;
					vertex[0].fire[0].Set(-r, particle->intensity, velocity[2].x, velocity[2].y);
					vertex[0].fire[1].Set(0.0F, 0.0F, offset.x, offset.y);
					vertex[0].fire[2].Set(velocity[0].x, velocity[0].y, velocity[1].x, velocity[1].y);

					vertex[1].position = p;
					vertex[1].color = particle->color;
					vertex[1].fire[0].Set(r, particle->intensity, velocity[2].x, velocity[2].y);
					vertex[1].fire[1].Set(1.0F, 0.0F, offset.x, offset.y);
					vertex[1].fire[2].Set(velocity[0].x, velocity[0].y, velocity[1].x, velocity[1].y);

					vertex[2].position.Set(p.x, p.y, z);
					vertex[2].color = particle->color;
					vertex[2].fire[0].Set(r, particle->intensity, velocity[2].x, velocity[2].y);
					vertex[2].fire[1].Set(1.0F, 1.0F, offset.x, offset.y);
					vertex[2].fire[2].Set(velocity[0].x, velocity[0].y, velocity[1].x, velocity[1].y);

					vertex[3].position.Set(p.x, p.y, z);
					vertex[3].color = particle->color;
					vertex[3].fire[0].Set(-r, particle->intensity, velocity[2].x, velocity[2].y);
					vertex[3].fire[1].Set(0.0F, 1.0F, offset.x, offset.y);
					vertex[3].fire[2].Set(velocity[0].x, velocity[0].y, velocity[1].x, velocity[1].y);

					vertex += 4;
					vertexCount += 4;
				}
			}

			particle = particle->GetNextParticle();
		}
	}
	else
	{
		const FireParticle *particle = static_cast<FireParticle *>(particleSystem->GetFirstParticle());
		while (particle)
		{
			if (particle->emitTime <= 0)
			{
				const Point3D& p = particle->position;
				float z = p.z + particle->height;

				float r = particle->radius;
				const Vector2D *velocity = particle->noiseVelocity;
				const Vector2D& offset = particle->noiseOffset;

				vertex[0].position = p;
				vertex[0].color = particle->color;
				vertex[0].fire[0].Set(-r, particle->intensity, velocity[2].x, velocity[2].y);
				vertex[0].fire[1].Set(0.0F, 0.0F, offset.x, offset.y);
				vertex[0].fire[2].Set(velocity[0].x, velocity[0].y, velocity[1].x, velocity[1].y);

				vertex[1].position = p;
				vertex[1].color = particle->color;
				vertex[1].fire[0].Set(r, particle->intensity, velocity[2].x, velocity[2].y);
				vertex[1].fire[1].Set(1.0F, 0.0F, offset.x, offset.y);
				vertex[1].fire[2].Set(velocity[0].x, velocity[0].y, velocity[1].x, velocity[1].y);

				vertex[2].position.Set(p.x, p.y, z);
				vertex[2].color = particle->color;
				vertex[2].fire[0].Set(r, particle->intensity, velocity[2].x, velocity[2].y);
				vertex[2].fire[1].Set(1.0F, 1.0F, offset.x, offset.y);
				vertex[2].fire[2].Set(velocity[0].x, velocity[0].y, velocity[1].x, velocity[1].y);

				vertex[3].position.Set(p.x, p.y, z);
				vertex[3].color = particle->color;
				vertex[3].fire[0].Set(-r, particle->intensity, velocity[2].x, velocity[2].y);
				vertex[3].fire[1].Set(0.0F, 1.0F, offset.x, offset.y);
				vertex[3].fire[2].Set(velocity[0].x, velocity[0].y, velocity[1].x, velocity[1].y);

				vertex += 4;
				vertexCount += 4;
			}

			particle = particle->GetNextParticle();
		}
	}

	particleSystem->SetVertexCount(vertexCount);
}

void FireParticleSystem::FinalizeParticles(Job *job, void *cookie)
{
	FireParticleSystem *particleSystem = static_cast<FireParticleSystem *>(cookie);
	particleSystem->vertexBuffer.EndUpdate();

	if (particleSystem->GetVertexCount() == 0)
	{
		particleSystem->ListElement<Renderable>::GetOwningList()->Remove(particleSystem);
	}
}

void FireParticleSystem::Render(const FrustumCamera *camera, List<Renderable> *effectList)
{
	renderJob.camera = camera;
	renderJob.attributeBuffer = vertexBuffer.BeginUpdate();
	GetWorld()->SubmitWorldJob(&renderJob);

	SetAnimateFlag();
	effectList[GetEffectListIndex()].Append(this);
}


PolyboardParticleSystem::PolyboardParticleSystem(ParticleSystemType type, ParticlePoolBase *pool, const char *textureName) :
		ParticleSystem(type, pool, kRenderIndexedTriangles, kRenderDepthTest | kRenderDepthInhibit),
		renderJob(&JobRenderParticles, &FinalizeParticles, this),
		attributeVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		indexVertexBuffer(kVertexBufferIndex | kVertexBufferDynamic),
		textureAttribute(textureName, &particleTextureHeader, particleTextureImage)
{
}

PolyboardParticleSystem::PolyboardParticleSystem(const PolyboardParticleSystem& polyboardParticleSystem, ParticlePoolBase *pool) :
		ParticleSystem(polyboardParticleSystem, pool),
		renderJob(&JobRenderParticles, &FinalizeParticles, this),
		attributeVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		indexVertexBuffer(kVertexBufferIndex | kVertexBufferDynamic),
		textureAttribute(polyboardParticleSystem.textureAttribute)
{
}

PolyboardParticleSystem::~PolyboardParticleSystem()
{
}

void PolyboardParticleSystem::Preprocess(void)
{
	ParticleSystem::Preprocess();
	AddAttribute(&textureAttribute);

	int32 count = GetTotalParticleCount();
	int32 vertexCount = count * 3;
	int32 triangleCount = count * 4;

	attributeVertexBuffer.Establish(vertexCount * sizeof(PolyVertex));
	indexVertexBuffer.Establish(triangleCount * sizeof(Triangle));
	SetVertexBuffer(kVertexBufferAttributeArray, &attributeVertexBuffer, sizeof(PolyVertex));
	SetVertexBuffer(kVertexBufferIndexArray, &indexVertexBuffer);

	SetVertexAttributeArray(kArrayPosition, 0, 3);
	SetVertexAttributeArray(kArrayColor, sizeof(Point3D), 4);
	SetVertexAttributeArray(kArrayTangent, sizeof(Point3D) + sizeof(ColorRGBA), 4);
	SetVertexAttributeArray(kArrayTexcoord, sizeof(Point3D) + sizeof(ColorRGBA) + sizeof(Vector4D), 2);

	SetShaderFlags(GetShaderFlags() | kShaderVertexPolyboard);
}

void PolyboardParticleSystem::JobRenderParticles(Job *job, void *cookie)
{
	const ParticleJob *particleJob = static_cast<ParticleJob *>(job);
	PolyboardParticleSystem *particleSystem = static_cast<PolyboardParticleSystem *>(cookie);

	int32 vertexCount = 0;
	int32 triangleCount = 0;
	volatile PolyVertex *restrict vertex = static_cast<volatile PolyVertex *>(particleJob->attributeBuffer);
	volatile Triangle *restrict triangle = static_cast<volatile Triangle *>(particleJob->indexBuffer);

	const PolyParticle *particle = static_cast<const PolyParticle *>(particleSystem->GetFirstParticle());
	while (particle)
	{
		const PolyParticle *next = particle->GetNextParticle();

		if (particle->emitTime <= 0)
		{
			vertex[0].position = particle->position;
			vertex[0].color = particle->color;
			vertex[0].tangent.Set(particle->tangent.x, particle->tangent.y, particle->tangent.z, -particle->radius);
			vertex[0].texcoord.Set(0.0F, particle->texcoord);

			vertex[1].position = particle->position;
			vertex[1].color = particle->color;
			vertex[1].tangent.Set(particle->tangent.x, particle->tangent.y, particle->tangent.z, particle->radius);
			vertex[1].texcoord.Set(1.0F, particle->texcoord);

			vertex += 2;
			vertexCount += 2;

			if (!particle->terminator)
			{
				vertex[0].position = (particle->position + next->position) * 0.5F;
				vertex[0].color = (particle->color + next->color) * 0.5F;
				vertex[0].tangent.Set(particle->tangent + next->tangent, 0.0F);
				vertex[0].texcoord.Set(0.5F, (particle->texcoord + next->texcoord) * 0.5F);

				triangle[0].Set(vertexCount - 2, vertexCount - 1, vertexCount);
				triangle[1].Set(vertexCount - 2, vertexCount, vertexCount + 1);
				triangle[2].Set(vertexCount - 1, vertexCount + 2, vertexCount);
				triangle[3].Set(vertexCount, vertexCount + 2, vertexCount + 1);

				vertex++;
				vertexCount++;

				triangle += 4;
				triangleCount += 4;
			}
		}

		particle = next;
	}

	particleSystem->SetVertexCount(vertexCount);
	particleSystem->SetPrimitiveCount(triangleCount);
}

void PolyboardParticleSystem::FinalizeParticles(Job *job, void *cookie)
{
	PolyboardParticleSystem *particleSystem = static_cast<PolyboardParticleSystem *>(cookie);
	particleSystem->indexVertexBuffer.EndUpdate();
	particleSystem->attributeVertexBuffer.EndUpdate();

	if (particleSystem->GetVertexCount() == 0)
	{
		particleSystem->ListElement<Renderable>::GetOwningList()->Remove(particleSystem);
	}
}

void PolyboardParticleSystem::Render(const FrustumCamera *camera, List<Renderable> *effectList)
{
	renderJob.camera = camera;
	renderJob.attributeBuffer = attributeVertexBuffer.BeginUpdate();
	renderJob.indexBuffer = indexVertexBuffer.BeginUpdate();
	GetWorld()->SubmitWorldJob(&renderJob);

	SetAnimateFlag();
	effectList[GetEffectListIndex()].Append(this);
}


BlobParticleSystem::BlobParticleSystem(ParticleSystemType type, ParticlePoolBase *pool) :
		ParticleSystem(type, pool, kRenderMultiIndexedTriangles, kRenderDepthTest),
		attributeVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		indexVertexBuffer(kVertexBufferIndex | kVertexBufferDynamic),
		blobBatch(this)
{
	particleSystemStorage = nullptr;
}

BlobParticleSystem::BlobParticleSystem(ParticleSystemType type, ParticlePoolBase *pool, float scale, float maxRadius) :
		ParticleSystem(type, pool, kRenderMultiIndexedTriangles, kRenderDepthTest),
		attributeVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		indexVertexBuffer(kVertexBufferIndex | kVertexBufferDynamic),
		blobBatch(this)
{
	voxelScale = scale;
	inverseVoxelScale = 1.0F / scale;

	maxParticleRadius = maxRadius;

	particleSystemStorage = nullptr;
}

BlobParticleSystem::BlobParticleSystem(const BlobParticleSystem& blobParticleSystem, ParticlePoolBase *pool) :
		ParticleSystem(blobParticleSystem, pool),
		attributeVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		indexVertexBuffer(kVertexBufferIndex | kVertexBufferDynamic),
		blobBatch(this)
{
	voxelScale = blobParticleSystem.voxelScale;
	inverseVoxelScale = 1.0F / voxelScale;

	maxParticleRadius = blobParticleSystem.maxParticleRadius;

	particleSystemStorage = nullptr;
}

BlobParticleSystem::~BlobParticleSystem()
{
	if (particleSystemStorage)
	{
		int32 count = GetTotalParticleCount();
		for (machine a = count - 1; a >= 0; a--)
		{
			jobArray[a].~BlobJob();
		}

		delete[] particleSystemStorage;
	}
}

BlobParticleSystem::BlobJob::BlobJob(ExecuteProc *execProc, void *cookie) : BatchJob(execProc, cookie)
{
}

BlobParticleSystem::BlobBatch::BlobBatch(BlobParticleSystem *system)
{
	particleSystem = system;
}

void BlobParticleSystem::Pack(Packer& data, unsigned_int32 packFlags) const
{
	ParticleSystem::Pack(data, packFlags);

	data << ChunkHeader('SCAL', 4);
	data << voxelScale;

	data << ChunkHeader('MRAD', 4);
	data << maxParticleRadius;

	data << TerminatorChunk;
}

void BlobParticleSystem::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	ParticleSystem::Unpack(data, unpackFlags);
	UnpackChunkList<BlobParticleSystem>(data, unpackFlags);
}

bool BlobParticleSystem::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'SCAL':

			data >> voxelScale;
			inverseVoxelScale = 1.0F / voxelScale;
			return (true);

		case 'MRAD':

			data >> maxParticleRadius;
			return (true);
	}

	return (false);
}

void BlobParticleSystem::Preprocess(void)
{
	ParticleSystem::Preprocess();

	SetEffectListIndex(kEffectListLight);

	if (!GetManipulator())
	{
		int32 radius = (int32) PositiveCeil(maxParticleRadius * inverseVoxelScale);
		int32 r2 = radius * radius;

		maxParticleVertexCount = r2 * 18;
		maxParticleTriangleCount = r2 * 36;

		int32 maxVoxelMapRowSize = (radius + 1) * 4;
		int32 maxVoxelMapDeckSize = maxVoxelMapRowSize * maxVoxelMapRowSize;
		int32 maxVoxelMapSize = maxVoxelMapRowSize * maxVoxelMapDeckSize;

		int32 particleCount = GetTotalParticleCount();
		Assert(maxParticleVertexCount * particleCount < 65535, "BlobParticleSystem::Preprocess(), the ratio of maxRadius to scale is too large\n");

		threadStorageSize = maxVoxelMapSize + maxVoxelMapDeckSize * (sizeof(VoxelStorage) * 2);
		unsigned_int32 systemStorageSize = (particleCount * (sizeof(BlobJob) + 4 + sizeof(machine_address)) + 15) & ~15;
		particleSystemStorage = new char[systemStorageSize + threadStorageSize * TheJobMgr->GetWorkerThreadCount()];

		jobArray = reinterpret_cast<BlobJob *>(particleSystemStorage);
		for (machine a = 0; a < particleCount; a++)
		{
			new(&jobArray[a]) BlobJob(&JobRenderBlob, this);
		}

		indexCountArray = reinterpret_cast<unsigned_int32 *>(jobArray + particleCount);
		indexOffsetArray = reinterpret_cast<machine_address *>(indexCountArray + particleCount);
		voxelMapStorage = particleSystemStorage + systemStorageSize;

		RenderSegment *segment = GetFirstRenderSegment();
		segment->SetMultiRenderArrays(indexCountArray, indexOffsetArray);

		int32 vertexCount = particleCount * maxParticleVertexCount;
		int32 triangleCount = particleCount * maxParticleTriangleCount;

		attributeVertexBuffer.Establish(vertexCount * sizeof(BlobVertex));
		indexVertexBuffer.Establish(triangleCount * sizeof(Triangle));
		SetVertexBuffer(kVertexBufferAttributeArray, &attributeVertexBuffer, sizeof(BlobVertex));
		SetVertexBuffer(kVertexBufferIndexArray, &indexVertexBuffer);

		SetVertexAttributeArray(kArrayPosition, 0, 3);
		SetVertexAttributeArray(kArrayNormal, sizeof(Point3D), 3);

		SetVertexCount(0);
		SetAmbientBlendState(kBlendReplace);
		SetShaderFlags((GetShaderFlags() | kShaderNormalizeBasisVectors) & ~kShaderAmbientEffect);
		segment->SetMaterialState(kMaterialOcclusionInhibit);
	}
}

void BlobParticleSystem::Neutralize(void)
{
	if (particleSystemStorage)
	{
		int32 count = GetTotalParticleCount();
		for (machine a = count - 1; a >= 0; a--)
		{
			jobArray[a].~BlobJob();
		}

		delete[] particleSystemStorage;
		particleSystemStorage = nullptr;
	}

	ParticleSystem::Neutralize();
}

float BlobParticleSystem::SortParticleList(List<BlobParticle> *inputList, int32 depth, float minValue, float maxValue, int32 index, List<BlobParticle> *outputList)
{
	BlobParticle *particle = inputList->First();
	if (particle == inputList->Last())
	{
		outputList->Append(particle);
		return (0.0F);
	}

	float distance = maxValue - minValue;
	if ((distance < voxelScale) || (--depth == 0))
	{
		do
		{
			BlobParticle *next = particle->Next();
			outputList->Append(particle);
			particle = next;
		} while (particle);

		return (distance);
	}

	List<BlobParticle>		lowerList;

	float maxLower = minValue;
	float minHigher = maxValue;
	float center = (minValue + maxValue) * 0.5F;

	do
	{
		BlobParticle *next = particle->Next();
		float v = particle->bounds.min[index];
		if (v < center)
		{
			maxLower = Fmax(maxLower, v);
			lowerList.Append(particle);
		}
		else
		{
			minHigher = Fmin(minHigher, v);
		}

		particle = next;
	} while (particle);

	distance = SortParticleList(&lowerList, depth, minValue, maxLower, index, outputList);
	distance = Fmax(SortParticleList(inputList, depth, minHigher, maxValue, index, outputList), distance);
	return (distance);
}

void BlobParticleSystem::ClusterParticlesX(List<BlobParticle> *inputList, int32 depth, float xmin, float xmax, List<BlobParticle> *singleList, List<BlobParticle> *multipleList)
{
	List<BlobParticle>		sortedList;

	float overlap = SortParticleList(inputList, depth, xmin, xmax, 0, &sortedList);

	BlobParticle *lowerParticle = sortedList.First();
	if (lowerParticle)
	{
		for (;;)
		{
			BlobParticle *higherParticle = lowerParticle->Next();
			if (!higherParticle)
			{
				singleList->Append(lowerParticle);
				break;
			}

			xmax = lowerParticle->bounds.max.x;
			if (higherParticle->bounds.min.x - overlap < xmax)
			{
				List<BlobParticle>		clusterList;

				float y = lowerParticle->bounds.min.y;
				float ymin = y;
				float ymax = y;

				clusterList.Append(lowerParticle);
				do
				{
					lowerParticle = higherParticle;
					higherParticle = higherParticle->Next();
					clusterList.Append(lowerParticle);

					y = lowerParticle->bounds.min.y;
					ymin = Fmin(ymin, y);
					ymax = Fmax(ymax, y);

					xmax = Fmax(xmax, lowerParticle->bounds.max.x);
				} while ((higherParticle) && (higherParticle->bounds.min.x - overlap < xmax));

				ClusterParticlesY(&clusterList, depth, ymin, ymax, singleList, multipleList);

				if (!higherParticle)
				{
					break;
				}
			}
			else
			{
				singleList->Append(lowerParticle);
			}

			lowerParticle = higherParticle;
		}
	}
}

void BlobParticleSystem::ClusterParticlesY(List<BlobParticle> *inputList, int32 depth, float ymin, float ymax, List<BlobParticle> *singleList, List<BlobParticle> *multipleList)
{
	List<BlobParticle>		sortedList;

	float overlap = SortParticleList(inputList, depth, ymin, ymax, 1, &sortedList);

	BlobParticle *lowerParticle = sortedList.First();
	for (;;)
	{
		BlobParticle *higherParticle = lowerParticle->Next();
		if (!higherParticle)
		{
			singleList->Append(lowerParticle);
			break;
		}

		ymax = lowerParticle->bounds.max.y;
		if (higherParticle->bounds.min.y - overlap < ymax)
		{
			List<BlobParticle>		clusterList;

			float z = lowerParticle->bounds.min.z;
			float zmin = z;
			float zmax = z;

			clusterList.Append(lowerParticle);
			do
			{
				lowerParticle = higherParticle;
				higherParticle = higherParticle->Next();
				clusterList.Append(lowerParticle);

				z = lowerParticle->bounds.min.z;
				zmin = Fmin(zmin, z);
				zmax = Fmax(zmax, z);

				ymax = Fmax(ymax, lowerParticle->bounds.max.y);
			} while ((higherParticle) && (higherParticle->bounds.min.y - overlap < ymax));

			ClusterParticlesZ(&clusterList, depth, zmin, zmax, singleList, multipleList);

			if (!higherParticle)
			{
				break;
			}
		}
		else
		{
			singleList->Append(lowerParticle);
		}

		lowerParticle = higherParticle;
	}
}

void BlobParticleSystem::ClusterParticlesZ(List<BlobParticle> *inputList, int32 depth, float zmin, float zmax, List<BlobParticle> *singleList, List<BlobParticle> *multipleList)
{
	List<BlobParticle>		sortedList;

	float overlap = SortParticleList(inputList, depth, zmin, zmax, 2, &sortedList);

	BlobParticle *lowerParticle = sortedList.First();
	for (;;)
	{
		BlobParticle *higherParticle = lowerParticle->Next();
		if (!higherParticle)
		{
			singleList->Append(lowerParticle);
			break;
		}

		zmax = lowerParticle->bounds.max.z;
		if (higherParticle->bounds.min.z - overlap < zmax)
		{
			lowerParticle->terminator = false;
			multipleList->Append(lowerParticle);

			do
			{
				lowerParticle = higherParticle;
				higherParticle = higherParticle->Next();

				lowerParticle->terminator = false;
				multipleList->Append(lowerParticle);

				zmax = Fmax(zmax, lowerParticle->bounds.max.z);
			} while ((higherParticle) && (higherParticle->bounds.min.z - overlap < zmax));

			lowerParticle->terminator = true;

			if (!higherParticle)
			{
				break;
			}
		}
		else
		{
			singleList->Append(lowerParticle);
		}

		lowerParticle = higherParticle;
	}
}

Vector3D BlobParticleSystem::CalculateNormal(const Voxel *voxelMap, const Integer3D& voxelMapSize, const Integer3D& coord)
{
	int32 x0 = MaxZero(coord.x - 1) - coord.x;
	int32 x1 = Min(coord.x + 1, voxelMapSize.x) - coord.x;
	int32 y0 = MaxZero(coord.y - 1) - coord.y;
	int32 y1 = Min(coord.y + 1, voxelMapSize.y) - coord.y;
	int32 z0 = MaxZero(coord.z - 1) - coord.z;
	int32 z1 = Min(coord.z + 1, voxelMapSize.z) - coord.z;

	int32 rowSize = voxelMapSize.x;
	int32 deckSize = rowSize * voxelMapSize.y;
	voxelMap += coord.z * deckSize + coord.y * rowSize + coord.x;

	int32 dx = voxelMap[x1] - voxelMap[x0];
	int32 dy = voxelMap[rowSize * y1] - voxelMap[rowSize * y0];
	int32 dz = voxelMap[deckSize * z1] - voxelMap[deckSize * z0];

	return (Vector3D((float) dx, (float) dy, (float) dz));
}

Vector3D BlobParticleSystem::CalculateNormal(const Voxel *voxelMap, const Integer3D& voxelMapSize, const Integer3D& coord0, const Integer3D& coord1, Fixed t, Fixed u)
{
	Vector3D n0 = CalculateNormal(voxelMap, voxelMapSize, coord0);
	Vector3D n1 = CalculateNormal(voxelMap, voxelMapSize, coord1);
	return (n0 * (float) t + n1 * (float) u);
}

void BlobParticleSystem::TriangulateBlob(const Voxel *voxelMap, const Integer3D& voxelMapSize, VoxelStorage *const (& deckStorage)[2], BlobJob *blobJob)
{
	using namespace Transvoxel;

	int32 globalVertexCount = 0;
	int32 globalTriangleCount = 0;

	unsigned_int8 deckParity = 0;
	int32 rowSize = voxelMapSize.x;
	int32 deckSize = rowSize * voxelMapSize.y;
	Integer3D cellCount(voxelMapSize.x - 1, voxelMapSize.y - 1, voxelMapSize.z - 1);

	const Point3D& base = blobJob->basePosition;
	float scale = blobJob->voxelScale;

	for (machine k = 0; k < cellCount.z; k++)
	{
		VoxelStorage *currentDeck = deckStorage[deckParity];
		VoxelStorage *previousDeck = deckStorage[deckParity ^ 1];

		const Voxel *deckMap = voxelMap + k * deckSize;
		for (machine j = 0; j < cellCount.y; j++)
		{
			Voxel	density[8];

			const Voxel *voxel = deckMap;
			density[0] = voxel[0];
			density[2] = voxel[rowSize];
			density[4] = voxel[deckSize];
			density[6] = voxel[deckSize + rowSize];

			unsigned_int32 code = ((density[0] >> 7) & 0x01) | ((density[2] >> 5) & 0x04) | ((density[4] >> 3) & 0x10) | ((density[6] >> 1) & 0x40);

			for (machine i = 0; i < cellCount.x; i++)
			{
				Voxel mainDensity = voxel[deckSize + rowSize + 1];
				if (mainDensity == 0)
				{
					if (globalVertexCount == blobJob->maxVertexCount)
					{
						goto end;
					}

					unsigned_int16 index = (unsigned_int16) globalVertexCount++;
					currentDeck[j * rowSize + i][0] = index;

					volatile BlobVertex *vertex = &blobJob->vertexArray[index];
					Fixed3D p((i + 1) << kVoxelFractionSize, (j + 1) << kVoxelFractionSize, (k + 1) << kVoxelFractionSize);
					vertex->position.Set(base.x + (float) p.x * scale, base.y + (float) p.y * scale, base.z + (float) p.z * scale);
					vertex->normal = CalculateNormal(voxelMap, voxelMapSize, Integer3D(i + 1, j + 1, k + 1));
				}

				density[1] = voxel[1];
				density[3] = voxel[rowSize + 1];
				density[5] = voxel[deckSize + 1];
				density[7] = mainDensity;

				code |= ((density[1] >> 6) & 0x02) | ((density[3] >> 4) & 0x08) | ((density[5] >> 2) & 0x20) | (mainDensity & 0x80);

				if ((code ^ ((mainDensity >> 7) & 0xFF)) != 0)
				{
					unsigned_int16	globalVertexIndex[12];

					const RegularCellData *cellData = &regularCellData[0][regularCellClass[code]];

					int32 triangleCount = cellData->GetTriangleCount();
					if (globalTriangleCount + triangleCount > blobJob->maxTriangleCount)
					{
						goto end;
					}

					int32 vertexCount = cellData->GetVertexCount();
					const unsigned_int16 *vertexArray = regularVertexData[code];

					for (machine a = 0; a < vertexCount; a++)
					{
						unsigned_int16	index;

						unsigned_int16 vertexData = vertexArray[a];
						unsigned_int16 v0 = (vertexData >> 4) & 0x0F;
						unsigned_int16 v1 = vertexData & 0x0F;

						int32 d0 = density[v0];
						int32 d1 = density[v1];
						Fixed t = (d1 << kVoxelFractionSize) / (d1 - d0);

						Integer3D coord0(i + (v0 & 1), j + ((v0 >> 1) & 1), k + ((v0 >> 2) & 1));
						Integer3D coord1(i + (v1 & 1), j + ((v1 >> 1) & 1), k + ((v1 >> 2) & 1));

						if ((t & (kVoxelFixedUnit - 1)) != 0)
						{
							unsigned_int16 edgeIndex = (vertexData >> 8) & 0x0F;
							unsigned_int16 deltaCode = (vertexData >> 12) & 0x0F;

							if (deltaCode < 8)
							{
								if (deltaCode & 4)
								{
									index = previousDeck[(j - ((deltaCode >> 1) & 1)) * rowSize + (i - (deltaCode & 1))][edgeIndex];
								}
								else
								{
									index = currentDeck[(j - (deltaCode >> 1)) * rowSize + (i - (deltaCode & 1))][edgeIndex];
								}
							}
							else
							{
								if (globalVertexCount == blobJob->maxVertexCount)
								{
									goto end;
								}

								index = (unsigned_int16) globalVertexCount++;
								volatile BlobVertex *vertex = &blobJob->vertexArray[index];

								Fixed u = kVoxelFixedUnit - t;
								Fixed3D p = coord0 * t + coord1 * u;
								vertex->position.Set(base.x + (float) p.x * scale, base.y + (float) p.y * scale, base.z + (float) p.z * scale);
								vertex->normal = CalculateNormal(voxelMap, voxelMapSize, coord0, coord1, t, u);

								if (v1 == 7)
								{
									currentDeck[j * rowSize + i][edgeIndex] = index;
								}
							}
						}
						else if (t == 0)
						{
							if (v1 == 7)
							{
								index = currentDeck[j * rowSize + i][0];
							}
							else
							{
								unsigned_int16 deltaCode = v1 ^ 7;
								if (deltaCode & 4)
								{
									index = previousDeck[(j - ((deltaCode >> 1) & 1)) * rowSize + (i - (deltaCode & 1))][0];
								}
								else
								{
									index = currentDeck[(j - (deltaCode >> 1)) * rowSize + (i - (deltaCode & 1))][0];
								}
							}
						}
						else
						{
							unsigned_int16 deltaCode = v0 ^ 7;
							if (deltaCode & 4)
							{
								index = previousDeck[(j - ((deltaCode >> 1) & 1)) * rowSize + (i - (deltaCode & 1))][0];
							}
							else
							{
								index = currentDeck[(j - (deltaCode >> 1)) * rowSize + (i - (deltaCode & 1))][0];
							}
						}

						globalVertexIndex[a] = (unsigned_int16) (blobJob->baseVertexIndex + index);
					}

					const unsigned_int8 *localVertexIndex = cellData->vertexIndex;
					volatile Triangle *restrict triangle = &blobJob->triangleArray[globalTriangleCount];

					for (machine a = 0; a < triangleCount; a++)
					{
						triangle->index[0] = globalVertexIndex[localVertexIndex[0]];
						triangle->index[1] = globalVertexIndex[localVertexIndex[1]];
						triangle->index[2] = globalVertexIndex[localVertexIndex[2]];

						localVertexIndex += 3;
						triangle++;
					}

					globalTriangleCount += triangleCount;
				}

				voxel++;
				code = (code >> 1) & 0x55;

				density[0] = density[1];
				density[2] = density[3];
				density[4] = density[5];
				density[6] = density[7];
			}

			deckMap += rowSize;
		}

		deckParity ^= 1;
	}

	end:
	blobJob->vertexCount = globalVertexCount;
	blobJob->triangleCount = globalTriangleCount;
}

void BlobParticleSystem::SingleRenderVoxels(Voxel *voxelMap, int32 voxelMapSize, float center, float radius, const Vector3D& scaleAxis, float inverseScale)
{
	Vector3D	p;

	float r = 127.0F / (radius * radius);

	int32 deckSize = voxelMapSize * voxelMapSize;
	for (machine a = 0; a < deckSize; a++)
	{
		*voxelMap++ = 0x7F;
	}

	float z = 1.0F;
	for (machine k = 2; k < voxelMapSize; k++)
	{
		for (machine a = 0; a < voxelMapSize; a++)
		{
			*voxelMap++ = 0x7F;
		}

		float y = 1.0F;
		p.z = z - center;

		for (machine j = 2; j < voxelMapSize; j++)
		{
			*voxelMap++ = 0x7F;

			float x = 1.0F;
			p.y = y - center;

			for (machine i = 2; i < voxelMapSize; i++)
			{
				p.x = x - center;

				Vector3D q = ProjectOnto(p, scaleAxis);
				q = q * (inverseScale - 1.0F) + p;

				*voxelMap++ = (Voxel) Min(Max((int32) (SquaredMag(q) * r) - 0x7F, -0x7F), 0x7F);
				x += 1.0F;
			}

			*voxelMap++ = 0x7F;
			y += 1.0F;
		}

		for (machine a = 0; a < voxelMapSize; a++)
		{
			*voxelMap++ = 0x7F;
		}

		z += 1.0F;
	}

	for (machine a = 0; a < deckSize; a++)
	{
		*voxelMap++ = 0x7F;
	}
}

void BlobParticleSystem::MultipleRenderVoxels(Voxel *voxelMap, const Integer3D& voxelMapSize, const Point3D& center, float radius, const Vector3D& scaleAxis, float inverseScale)
{
	Vector3D	p;

	float r = 127.0F / (radius * radius);

	int32 rowSize = voxelMapSize.x;
	int32 deckSize = rowSize * voxelMapSize.y;

	int32 dr = (int32) PositiveCeil(radius) + 2;
	int32 cx = (int32) center.x;
	int32 cy = (int32) center.y;
	int32 cz = (int32) center.z;
	Integer3D min(Max(cx - dr, 1), Max(cy - dr, 1), Max(cz - dr, 1));
	Integer3D max(Min(cx + dr + 1, rowSize - 2), Min(cy + dr + 1, voxelMapSize.y - 2), Min(cz + dr + 1, voxelMapSize.z - 2));

	float z = (float) min.z;
	for (machine k = min.z; k <= max.z; k++)
	{
		float y = (float) min.y;
		p.z = z - center.z;

		for (machine j = min.y; j <= max.y; j++)
		{
			float x = (float) min.x;
			p.y = y - center.y;

			Voxel *voxel = voxelMap + k * deckSize + j * rowSize;
			for (machine i = min.x; i <= max.x; i++)
			{
				p.x = x - center.x;

				Vector3D q = ProjectOnto(p, scaleAxis);
				q = q * (inverseScale - 1.0F) + p;

				int32 f = MaxZero(0xFE - (int32) (SquaredMag(q) * r));
				voxel[i] = (Voxel) Max(voxel[i] - f, -0x7F);
				x += 1.0F;
			}

			y += 1.0F;
		}

		z += 1.0F;
	}
}

void BlobParticleSystem::JobRenderBlob(Job *job, void *cookie)
{
	Voxel			*voxel;
	VoxelStorage	*deckStorage[2];

	BlobJob *blobJob = static_cast<BlobJob *>(job);
	BlobParticleSystem *particleSystem = static_cast<BlobParticleSystem *>(cookie);

	const BlobParticle *particle = blobJob->particle;
	const BlobParticle *next = particle->Next();
	if (!next)
	{
		float r = Fmin(particle->radius, particleSystem->maxParticleRadius);
		float center = PositiveCeil(r * particleSystem->inverseVoxelScale) + 1.0F;

		int32 mapRadius = (int32) center;
		int32 rowSize = mapRadius * 2 + 1;
		int32 deckSize = rowSize * rowSize;
		int32 mapSize = deckSize * rowSize;
		int32 paddedMapSize = (mapSize + 15) & ~15;

		voxel = reinterpret_cast<Voxel *>(particleSystem->voxelMapStorage + particleSystem->threadStorageSize * blobJob->GetThreadIndex());

		deckStorage[0] = reinterpret_cast<VoxelStorage *>(voxel + paddedMapSize);
		deckStorage[1] = deckStorage[0] + deckSize;

		SingleRenderVoxels(voxel, rowSize, center, r * particleSystem->inverseVoxelScale, particle->scaleAxis, particle->inverseScale);

		float scale = particleSystem->voxelScale;
		float f = (float) mapRadius * scale;
		blobJob->basePosition.Set(particle->position.x - f, particle->position.y - f, particle->position.z - f);
		blobJob->voxelScale = scale * (1.0F / (float) kVoxelFixedUnit);

		particleSystem->TriangulateBlob(voxel, Integer3D(rowSize, rowSize, rowSize), deckStorage, blobJob);
	}
	else
	{
		Box3D blobBounds = Union(particle->bounds, next->bounds);
		while (!next->terminator)
		{
			next = next->Next();
			blobBounds.Union(next->bounds);
		}

		Vector3D voxelBounds = (blobBounds.max - blobBounds.min) * particleSystem->inverseVoxelScale;
		Integer3D voxelMapSize((int32) PositiveCeil(voxelBounds.x) + 2, (int32) PositiveCeil(voxelBounds.y) + 2, (int32) PositiveCeil(voxelBounds.z) + 2);

		int32 rowSize = voxelMapSize.x;
		int32 deckSize = rowSize * voxelMapSize.y;
		int32 mapSize = deckSize * voxelMapSize.z;
		int32 paddedMapSize = (mapSize + 15) & ~15;

		char *storage = nullptr;
		unsigned_int32 storageSize = paddedMapSize + deckSize * (sizeof(VoxelStorage) * 2);
		unsigned_int32 threadStorageSize = particleSystem->threadStorageSize;

		if (storageSize <= threadStorageSize)
		{
			voxel = reinterpret_cast<Voxel *>(particleSystem->voxelMapStorage + threadStorageSize * blobJob->GetThreadIndex());
		}
		else
		{
			storage = new char[storageSize];
			voxel = reinterpret_cast<Voxel *>(storage);
		}

		deckStorage[0] = reinterpret_cast<VoxelStorage *>(voxel + paddedMapSize);
		deckStorage[1] = deckStorage[0] + deckSize;

		unsigned_int32 *voxelGroup = reinterpret_cast<unsigned_int32 *>(voxel);
		int32 groupCount = paddedMapSize >> 4;
		for (machine a = 0; a < groupCount; a++)
		{
			voxelGroup[0] = 0x7F7F7F7F;
			voxelGroup[1] = 0x7F7F7F7F;
			voxelGroup[2] = 0x7F7F7F7F;
			voxelGroup[3] = 0x7F7F7F7F;
			voxelGroup += 4;
		}

		float scale = particleSystem->voxelScale;
		blobJob->basePosition.Set(blobBounds.min.x - scale, blobBounds.min.y - scale, blobBounds.min.z - scale);
		blobJob->voxelScale = scale * (1.0F / (float) kVoxelFixedUnit);

		for (;;)
		{
			Point3D center = Zero3D + (particle->position - blobBounds.min) * particleSystem->inverseVoxelScale + Vector3D(1.0F, 1.0F, 1.0F);
			MultipleRenderVoxels(voxel, voxelMapSize, center, particle->radius * particleSystem->inverseVoxelScale, particle->scaleAxis, particle->inverseScale);

			if (particle->terminator)
			{
				break;
			}

			particle = particle->Next();
		}

		particleSystem->TriangulateBlob(voxel, voxelMapSize, deckStorage, blobJob);
		delete[] storage;
	}
}

void BlobParticleSystem::Render(const FrustumCamera *camera, List<Renderable> *effectList)
{
	BlobParticle *particle = static_cast<BlobParticle *>(GetFirstParticle());
	if (particle)
	{
		List<BlobParticle>		particleList;

		int32 particleCount = 0;
		float xmin = K::infinity;
		float xmax = K::minus_infinity;

		if (!(GetParticleSystemFlags() & kParticleSystemObjectSpace))
		{
			do
			{
				float r = particle->radius + voxelScale;
				const Point3D& p = particle->position;
				if (camera->FrustumCamera::SphereVisible(p, r))
				{
					float x = p.x - r;
					xmin = Fmin(xmin, x);
					xmax = Fmax(xmax, x);
					particle->bounds.Set(Point3D(x, p.y - r, p.z - r), Point3D(p.x + r, p.y + r, p.z + r));

					particleList.Append(particle);
					particleCount++;
				}

				particle = particle->GetNextParticle();
			} while (particle);
		}
		else
		{
			const Transform4D& transform = GetWorldTransform();
			do
			{
				float r = particle->radius + voxelScale;
				const Point3D& p = particle->position;
				if (camera->FrustumCamera::SphereVisible(transform * p, r))
				{
					float x = p.x - r;
					xmin = Fmin(xmin, x);
					xmax = Fmax(xmax, x);
					particle->bounds.Set(Point3D(x, p.y - r, p.z - r), Point3D(p.x + r, p.y + r, p.z + r));

					particleList.Append(particle);
					particleCount++;
				}

				particle = particle->GetNextParticle();
			} while (particle);
		}

		if (particleCount != 0)
		{
			List<BlobParticle>		singleList;

			ClusterParticlesX(&particleList, 8, xmin, xmax, &singleList, &blobBatch.multipleList);

			volatile BlobVertex *restrict vertexArray = attributeVertexBuffer.BeginUpdate<BlobVertex>();
			volatile Triangle *restrict triangleArray = indexVertexBuffer.BeginUpdate<Triangle>();

			unsigned_int32 particleIndex = 0;
			BlobJob *blobJob = jobArray;
			for (;;)
			{
				particle = singleList.First();
				if (!particle)
				{
					break;
				}

				singleList.Remove(particle);
				blobJob->particle = particle;

				unsigned_int32 baseVertexIndex = particleIndex * maxParticleVertexCount;
				unsigned_int32 baseTriangleIndex = particleIndex * maxParticleTriangleCount;
				particleIndex++;

				blobJob->baseVertexIndex = baseVertexIndex;
				blobJob->baseTriangleIndex = baseTriangleIndex;
				blobJob->maxVertexCount = maxParticleVertexCount;
				blobJob->maxTriangleCount = maxParticleTriangleCount;
				blobJob->vertexArray = vertexArray + baseVertexIndex;
				blobJob->triangleArray = triangleArray + baseTriangleIndex;

				TheJobMgr->SubmitJob(blobJob, &blobBatch);
				blobJob++;
			}

			particle = blobBatch.multipleList.First();
			while (particle)
			{
				blobJob->particle = particle;

				int32 multipleCount = 2;
				for (;; multipleCount++)
				{
					particle = particle->Next();
					if (particle->terminator)
					{
						break;
					}
				}

				unsigned_int32 baseVertexIndex = particleIndex * maxParticleVertexCount;
				unsigned_int32 baseTriangleIndex = particleIndex * maxParticleTriangleCount;
				particleIndex += multipleCount;

				blobJob->baseVertexIndex = baseVertexIndex;
				blobJob->baseTriangleIndex = baseTriangleIndex;
				blobJob->maxVertexCount = maxParticleVertexCount * multipleCount;
				blobJob->maxTriangleCount = maxParticleTriangleCount * multipleCount;
				blobJob->vertexArray = vertexArray + baseVertexIndex;
				blobJob->triangleArray = triangleArray + baseTriangleIndex;

				TheJobMgr->SubmitJob(blobJob, &blobBatch);
				blobJob++;

				particle = particle->Next();
			}

			blobBatch.jobCount = (int32) (blobJob - jobArray);
			batchList.Append(&blobBatch);

			effectList[GetEffectListIndex()].Append(this);
		}
	}

	SetAnimateFlag();
}

void BlobParticleSystem::Finalize(List<Renderable> *effectList)
{
	TheJobMgr->FinishBatch(&blobBatch);
	blobBatch.multipleList.RemoveAll();

	indexVertexBuffer.EndUpdate();
	attributeVertexBuffer.EndUpdate();

	int32 renderCount = 0;
	int32 jobCount = blobBatch.jobCount;
	BlobJob *blobJob = jobArray;

	for (machine a = 0; a < jobCount; a++)
	{
		int32 triangleCount = blobJob->triangleCount;
		if (triangleCount != 0)
		{
			indexCountArray[renderCount] = triangleCount * 3;
			indexOffsetArray[renderCount] = blobJob->baseTriangleIndex * sizeof(Triangle);
			renderCount++;
		}

		blobJob++;
	}

	if (renderCount != 0)
	{
		GetFirstRenderSegment()->SetMultiRenderCount(renderCount);
	}
	else
	{
		effectList[GetEffectListIndex()].Remove(this);
	}
}

void BlobParticleSystem::FinishBatches(List<Renderable> *effectList)
{
	for (;;)
	{
		BlobBatch *batch = batchList.First();
		if (!batch)
		{
			break;
		}

		batchList.Remove(batch);
		batch->particleSystem->Finalize(effectList);
	}
}


StarField::StarField() :
		InfinitePointParticleSystem(kParticleSystemStarField, &particlePool),
		particlePool(kMaxParticleCount, particleArray)
{
	starFieldName[0] = 0;
	starFieldFlags = 0;
}

StarField::StarField(const char *name) :
		InfinitePointParticleSystem(kParticleSystemStarField, &particlePool),
		particlePool(kMaxParticleCount, particleArray)
{
	starFieldName = (name) ? name : "C4/stars";
	starFieldFlags = 0;
}

StarField::StarField(const StarField& starField) :
		InfinitePointParticleSystem(starField, &particlePool),
		particlePool(kMaxParticleCount, particleArray)
{
	starFieldName = starField.starFieldName;
	starFieldFlags = starField.starFieldFlags;
}

StarField::~StarField()
{
}

Node *StarField::Replicate(void) const
{
	return (new StarField(*this));
}

void StarField::Pack(Packer& data, unsigned_int32 packFlags) const
{
	InfinitePointParticleSystem::Pack(data, packFlags);

	PackHandle handle = data.BeginChunk('NAME');
	data << starFieldName;
	data.EndChunk(handle);

	data << ChunkHeader('FLAG', 4);
	data << starFieldFlags;

	data << TerminatorChunk;
}

void StarField::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	InfinitePointParticleSystem::Unpack(data, unpackFlags);
	UnpackChunkList<StarField>(data, unpackFlags);
}

bool StarField::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'NAME':

			data >> starFieldName;
			return (true);

		case 'FLAG':

			data >> starFieldFlags;
			return (true);
	}

	return (false);
}

int32 StarField::GetCategoryCount(void) const
{
	return (InfinitePointParticleSystem::GetCategoryCount() + 1);
}

Type StarField::GetCategoryType(int32 index, const char **title) const
{
	if (index == 0)
	{
		*title = TheInterfaceMgr->GetStringTable()->GetString(StringID(kParticleSystemStarField));
		return (kParticleSystemStarField);
	}

	return (InfinitePointParticleSystem::GetCategoryType(index - 1, title));
}

int32 StarField::GetCategorySettingCount(Type category) const
{
	if (category == kParticleSystemStarField)
	{
		return (3);
	}

	return (InfinitePointParticleSystem::GetCategorySettingCount(category));
}

Setting *StarField::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kParticleSystemStarField)
	{
		if (flags & kConfigurationScript)
		{
			return (nullptr);
		}

		const StringTable *table = TheInterfaceMgr->GetStringTable();

		if (index == 0)
		{
			const char *title = table->GetString(StringID('PTCL', kParticleSystemStarField, 'STAR'));
			return (new HeadingSetting(kParticleSystemStarField, title));
		}

		if (index == 1)
		{
			const char *title = table->GetString(StringID('PTCL', kParticleSystemStarField, 'STAR', 'FNAM'));
			const char *picker = table->GetString(StringID('PTCL', kParticleSystemStarField, 'STAR', 'PICK'));
			return (new ResourceSetting('FNAM', starFieldName, title, picker, StarFieldResource::GetDescriptor()));
		}

		if (index == 2)
		{
			const char *title = table->GetString(StringID('PTCL', kParticleSystemStarField, 'STAR', 'HORZ'));
			return (new BooleanSetting('HORZ', ((starFieldFlags & kStarFieldAboveHorizon) != 0), title));
		}

		return (nullptr);
	}

	return (InfinitePointParticleSystem::GetCategorySetting(category, index, flags));
}

void StarField::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kParticleSystemStarField)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'FNAM')
		{
			starFieldName = static_cast<const ResourceSetting *>(setting)->GetResourceName();
		}
		else if (identifier == 'HORZ')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				starFieldFlags |= kStarFieldAboveHorizon;
			}
			else
			{
				starFieldFlags &= ~kStarFieldAboveHorizon;
			}
		}
	}
	else
	{
		InfinitePointParticleSystem::SetCategorySetting(category, setting);
	}
}

void StarField::Preprocess(void)
{
	SetParticleSystemFlags(kParticleSystemPointSprite | kParticleSystemNonpersistent | kParticleSystemObjectSpace);
	InfinitePointParticleSystem::Preprocess();
	Sleep();

	SetTransformable(this);

	SetVisibilityProc(&AlwaysVisible);
	SetOcclusionProc(&AlwaysVisible);

	initializedFlag = false;

	if (!GetFirstParticle())
	{
		StarFieldResource *resource = StarFieldResource::Get(starFieldName);
		if (resource)
		{
			const StarData *starData = resource->GetStarData();
			int32 starCount = Min(resource->GetStarCount(), kMaxParticleCount);

			for (machine a = 0; a < starCount; a++)
			{
				Vector2D t = CosSin(starData->rightAscension * K::tau_over_24);
				Vector2D u = CosSin((90.0F - starData->declination) * K::radians);
				Point3D position(t.x * u.y, t.y * u.y, u.x);

				if ((!(starFieldFlags & kStarFieldAboveHorizon)) || ((GetWorldTransform().GetRow(2) ^ position) > 0.0F))
				{
					Particle *particle = particlePool.NewParticle();
					particle->position = position;

					particle->emitTime = 0;
					particle->lifeTime = 1;
					particle->orientation = 0;

					float bright = 1.0F - starData->magnitude * 0.2F;
					float radius = bright * 0.6F;
					float color = 1.0F;

					if (radius < 0.25F)
					{
						color = Fmax(color * radius * 4.0F, 0.15F);
					}

					particle->radius = Fmax(radius, 0.25F) * 0.01F;
					particle->color.Set(color, color, color, 1.0F);

					AddParticle(particle);
				}

				starData++;
			}

			resource->Release();
		}
	}
}

void StarField::Neutralize(void)
{
	InfinitePointParticleSystem::Neutralize();
	FreeAllParticles();
}

void StarField::Move(void)
{
}

void StarField::Render(const FrustumCamera *camera, List<Renderable> *effectList)
{
	if (initializedFlag)
	{
		effectList[GetEffectListIndex()].Append(this);
	}
	else
	{
		initializedFlag = true;
		InfinitePointParticleSystem::Render(camera, effectList);
	}
}

// ZYUQURM
