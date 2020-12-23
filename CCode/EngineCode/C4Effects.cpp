 

#include "C4Effects.h"
#include "C4Particles.h"
#include "C4Markings.h"
#include "C4Shafts.h"
#include "C4Panels.h"
#include "C4World.h"


using namespace C4;


namespace
{
	const TextureHeader beamTextureHeader =
	{
		kTexture2D,
		kTextureForceHighQuality,
		kTextureSemanticEmission,
		kTextureSemanticNone,
		kTextureL8,
		16, 1, 1,
		{kTextureClamp, kTextureClamp, kTextureClamp},
		1
	};


	const unsigned_int8 beamTextureImage[16] =
	{
		0x00, 0x03, 0x09, 0x0F, 0x19, 0x27, 0x3F, 0x6F, 0xFF, 0x6F, 0x3F, 0x27, 0x19, 0x0F, 0x09, 0x03
	};
}


namespace C4
{
	template class Registrable<Effect, EffectRegistration>;
}


EffectRegistration::EffectRegistration(EffectType type, const char *name) : Registration<Effect, EffectRegistration>(type)
{
	effectName = name;
}

EffectRegistration::~EffectRegistration()
{
}


EffectObject::EffectObject(EffectType type) : Object(kObjectEffect)
{
	effectType = type;
}

EffectObject::~EffectObject()
{
}

EffectObject *EffectObject::Create(Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (data.GetType())
	{
		case kEffectShaft:

			return (ShaftEffectObject::Create(++data, unpackFlags));

		case kEffectQuad:

			return (new QuadEffectObject);

		case kEffectFlare:

			return (new FlareEffectObject);

		case kEffectBeam:

			return (new BeamEffectObject);

		case kEffectTube:

			return (new TubeEffectObject);

		case kEffectBolt:

			return (new BoltEffectObject);

		case kEffectFire:

			return (new FireEffectObject);

		case kEffectPanel:

			return (new PanelEffectObject);
	}

	return (nullptr);
}

void EffectObject::PackType(Packer& data) const
{
	Object::PackType(data); 
	data << effectType;
}
 

Effect::Effect(EffectType type, RenderType renderType, unsigned_int32 renderState) : RenderableNode(kNodeEffect, renderType, renderState) 
{
	effectType = type;
 
	effectFlags = 0;
	effectListIndex = kEffectListTransparent; 
 
	renderStamp = 0xFFFFFFFF;
	lightStamp = 0xFFFFFFFF;

	SetPerspectiveExclusionMask(kPerspectiveRadiositySpace); 
}

Effect::Effect(const Effect& effect) : RenderableNode(effect)
{
	effectType = effect.effectType;

	effectFlags = effect.effectFlags;
	effectListIndex = effect.effectListIndex;

	renderStamp = 0xFFFFFFFF;
	lightStamp = 0xFFFFFFFF;
}

Effect::~Effect()
{
}

Effect *Effect::Create(Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (data.GetType())
	{
		case kEffectParticleSystem:

			return (Registrable<ParticleSystem, ParticleSystemRegistration>::Create(++data, unpackFlags));

		case kEffectMarking:

			return (new MarkingEffect);

		case kEffectShaft:

			return (ShaftEffect::Create(++data, unpackFlags));

		case kEffectQuad:

			return (new QuadEffect);

		case kEffectFlare:

			return (new FlareEffect);

		case kEffectBeam:

			return (new BeamEffect);

		case kEffectTube:

			return (new TubeEffect);

		case kEffectBolt:

			return (new BoltEffect);

		case kEffectFire:

			return (new FireEffect);

		case kEffectShockwave:

			return (new ShockwaveEffect);

		case kEffectPanel:

			return (new PanelEffect);
	}

	return (Registrable<Effect, EffectRegistration>::Create(data, unpackFlags));
}

Effect *Effect::New(EffectType type)
{
	Type	data[2];

	data[0] = type;
	data[1] = 0;

	Unpacker unpacker(data);
	return (Create(unpacker));
}

void Effect::PackType(Packer& data) const
{
	RenderableNode::PackType(data);
	data << effectType;
}

void Effect::Pack(Packer& data, unsigned_int32 packFlags) const
{
	RenderableNode::Pack(data, packFlags);

	data << ChunkHeader('FLAG', 4);
	data << effectFlags;

	data << TerminatorChunk;
}

void Effect::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	RenderableNode::Unpack(data, unpackFlags);
	UnpackChunkList<Effect>(data, unpackFlags);
}

bool Effect::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> effectFlags;
			return (true);
	}

	return (false);
}

int32 Effect::GetCategorySettingCount(Type category) const
{
	int32 count = RenderableNode::GetCategorySettingCount(category);
	if (category == 'NODE')
	{
		count += kPerspectiveMaskSettingCount;
	}

	return (count);
}

Setting *Effect::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == 'NODE')
	{
		int32 i = index - RenderableNode::GetCategorySettingCount('NODE');
		if (i >= 0)
		{
			return (GetPerspectiveMaskSetting(i));
		}
	}

	return (RenderableNode::GetCategorySetting(category, index, flags));
}

void Effect::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == 'NODE')
	{
		if (!SetPerspectiveMaskSetting(setting))
		{
			RenderableNode::SetCategorySetting('NODE', setting);
		}
	}
	else
	{
		RenderableNode::SetCategorySetting(category, setting);
	}
}

void Effect::Preprocess(void)
{
	SetActiveUpdateFlags(GetActiveUpdateFlags() | kUpdateVisibility);

	if (effectFlags & kEffectCubeLightInhibit)
	{
		SetShaderFlags(kShaderCubeLightInhibit);
	}

	RenderableNode::Preprocess();
}

void Effect::Neutralize(void)
{
	ListElement<Effect>::Detach();
	RenderableNode::Neutralize();
}

void Effect::HandleVisibilityUpdate(void)
{
	PurgeVisibility();

	Zone *zone = GetOwningZone();
	if (!(GetNodeFlags() & kNodeInfiniteVisibility))
	{
		zone->InsertZoneTreeSite(kCellGraphEffect, this, GetMaxSubzoneDepth(), GetForcedSubzoneDepth());
	}
	else
	{
		zone->InsertInfiniteSite(kCellGraphEffect, this, Min(GetMaxSubzoneDepth(), GetForcedSubzoneDepth()));
	}

	if (effectListIndex == kEffectListLight)
	{
		SelectAmbientEnvironment(this);
	}
}

void Effect::UpdateEffectGeometry(void)
{
}

void Effect::Wake(void)
{
	World *world = GetWorld();
	if (world)
	{
		world->AddMovingEffect(this);
	}
}

void Effect::Sleep(void)
{
	ListElement<Effect>::Detach();
}

void Effect::Move(void)
{
}

void Effect::Render(const FrustumCamera *camera, List<Renderable> *effectList)
{
	effectList[effectListIndex].Append(this);
}


QuadEffectObject::QuadEffectObject() : EffectObject(kEffectQuad)
{
	quadFlags = 0;
	quadBlendState = BlendState(kBlendSourceAlpha, kBlendOne);
	quadDeltaScale = 1.0F;
}

QuadEffectObject::QuadEffectObject(float radius, const ColorRGBA& color, const char *textureName) : EffectObject(kEffectQuad)
{
	quadFlags = 0;
	quadRadius = radius;
	quadColor = color;
	quadTextureName = textureName;

	quadBlendState = BlendState(kBlendSourceAlpha, kBlendOne);
	quadDeltaScale = 1.0F;
}

QuadEffectObject::~QuadEffectObject()
{
}

void QuadEffectObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	EffectObject::Pack(data, packFlags);

	data << ChunkHeader('FLAG', 4);
	data << quadFlags;

	PackHandle handle = data.BeginChunk('DATA');
	data << quadRadius;
	data << quadColor;
	data << quadTextureName;
	data.EndChunk(handle);

	data << ChunkHeader('BLND', 4);
	data << quadBlendState;

	data << ChunkHeader('DSCL', 4);
	data << quadDeltaScale;

	data << TerminatorChunk;
}

void QuadEffectObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	EffectObject::Unpack(data, unpackFlags);
	UnpackChunkList<QuadEffectObject>(data, unpackFlags);
}

bool QuadEffectObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> quadFlags;
			return (true);

		case 'DATA':

			data >> quadRadius;
			data >> quadColor;
			data >> quadTextureName;
			return (true);

		case 'BLND':

			data >> quadBlendState;
			return (true);

		case 'DSCL':

			data >> quadDeltaScale;
			return (true);
	}

	return (false);
}

int32 QuadEffectObject::GetCategoryCount(void) const
{
	return (1);
}

Type QuadEffectObject::GetCategoryType(int32 index, const char **title) const
{
	if (index == 0)
	{
		*title = TheInterfaceMgr->GetStringTable()->GetString(StringID(kEffectQuad));
		return (kEffectQuad);
	}

	return (0);
}

int32 QuadEffectObject::GetCategorySettingCount(Type category) const
{
	if (category == kEffectQuad)
	{
		return (8);
	}

	return (0);
}

Setting *QuadEffectObject::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kEffectQuad)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();

		if (index == 0)
		{
			const char *title = table->GetString(StringID(kEffectQuad, 'QUAD'));
			return (new HeadingSetting(kEffectQuad, title));
		}

		if (index == 1)
		{
			const char *title = table->GetString(StringID(kEffectQuad, 'QUAD', 'COLR'));
			const char *picker = table->GetString(StringID(kEffectQuad, 'QUAD', 'CPCK'));
			return (new ColorSetting('COLR', quadColor, title, picker, kColorPickerAlpha));
		}

		if (index == 2)
		{
			const char *title = table->GetString(StringID(kEffectQuad, 'QUAD', 'TNAM'));
			const char *picker = table->GetString(StringID(kEffectQuad, 'QUAD', 'TPCK'));
			return (new ResourceSetting('TNAM', quadTextureName, title, picker, TextureResource::GetDescriptor()));
		}

		if (index == 3)
		{
			if (flags & kConfigurationScript)
			{
				return (nullptr);
			}

			int32 selection = 0;
			if (quadBlendState == BlendState(kBlendSourceAlpha, kBlendOne))
			{
				selection = 1;
			}
			else if (quadBlendState == kBlendInterpolate)
			{
				selection = 2;
			}
			else if (quadBlendState == kBlendPremultInterp)
			{
				selection = 3;
			}
			else if (quadBlendState == kBlendReplace)
			{
				selection = 4;
			}

			const char *title = table->GetString(StringID(kEffectQuad, 'QUAD', 'BLND'));
			MenuSetting *menu = new MenuSetting('BLND', selection, title, 5);

			menu->SetMenuItemString(0, table->GetString(StringID(kEffectQuad, 'QUAD', 'BLND', 'ADD ')));
			menu->SetMenuItemString(1, table->GetString(StringID(kEffectQuad, 'QUAD', 'BLND', 'ADDA')));
			menu->SetMenuItemString(2, table->GetString(StringID(kEffectQuad, 'QUAD', 'BLND', 'TERP')));
			menu->SetMenuItemString(3, table->GetString(StringID(kEffectQuad, 'QUAD', 'BLND', 'PREM')));
			menu->SetMenuItemString(4, table->GetString(StringID(kEffectQuad, 'QUAD', 'BLND', 'REPL')));

			return (menu);
		}

		if (index == 4)
		{
			if (flags & kConfigurationScript)
			{
				return (nullptr);
			}

			const char *title = table->GetString(StringID(kEffectQuad, 'QUAD', 'RFOG'));
			return (new BooleanSetting('RFOG', ((quadFlags & kQuadFogInhibit) == 0), title));
		}

		if (index == 5)
		{
			if (flags & kConfigurationScript)
			{
				return (nullptr);
			}

			const char *title = table->GetString(StringID(kEffectQuad, 'QUAD', 'INFT'));
			return (new BooleanSetting('INFT', ((quadFlags & kQuadInfinite) != 0), title));
		}

		if (index == 6)
		{
			if (flags & kConfigurationScript)
			{
				return (nullptr);
			}

			const char *title = table->GetString(StringID(kEffectQuad, 'QUAD', 'SOFT'));
			return (new BooleanSetting('SOFT', ((quadFlags & kQuadSoftDepth) != 0), title));
		}

		if (index == 7)
		{
			if (flags & kConfigurationScript)
			{
				return (nullptr);
			}

			const char *title = table->GetString(StringID(kEffectQuad, 'QUAD', 'DSCL'));
			return (new TextSetting('DSCL', quadDeltaScale, title));
		}
	}

	return (nullptr);
}

void QuadEffectObject::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kEffectQuad)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'COLR')
		{
			quadColor = static_cast<const ColorSetting *>(setting)->GetColor();
		}
		else if (identifier == 'TNAM')
		{
			quadTextureName = static_cast<const ResourceSetting *>(setting)->GetResourceName();
		}
		else if (identifier == 'BLND')
		{
			static const unsigned_int32 stateTable[5] =
			{
				kBlendAccumulate, BlendState(kBlendSourceAlpha, kBlendOne), kBlendInterpolate, kBlendPremultInterp, kBlendReplace
			};

			quadBlendState = stateTable[static_cast<const MenuSetting *>(setting)->GetMenuSelection()];
		}
		else if (identifier == 'RFOG')
		{
			if (!static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				quadFlags |= kQuadFogInhibit;
			}
			else
			{
				quadFlags &= ~kQuadFogInhibit;
			}
		}
		else if (identifier == 'INFT')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				quadFlags |= kQuadInfinite;
			}
			else
			{
				quadFlags &= ~kQuadInfinite;
			}
		}
		else if (identifier == 'SOFT')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				quadFlags |= kQuadSoftDepth;
			}
			else
			{
				quadFlags &= ~kQuadSoftDepth;
			}
		}
		else if (identifier == 'DSCL')
		{
			quadDeltaScale = FmaxZero(Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText()));
		}
	}
}

int32 QuadEffectObject::GetObjectSize(float *size) const
{
	size[0] = quadRadius;
	return (1);
}

void QuadEffectObject::SetObjectSize(const float *size)
{
	quadRadius = size[0];
}


QuadEffect::QuadEffect() :
		Effect(kEffectQuad, kRenderTriangleStrip, kRenderDepthTest | kRenderDepthInhibit),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		colorAttribute(kAttributeMutable)
{
	quadOrientation = 0;
}

QuadEffect::QuadEffect(float radius, const ColorRGBA& color, const char *textureName) :
		Effect(kEffectQuad, kRenderTriangleStrip, kRenderDepthTest | kRenderDepthInhibit),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		colorAttribute(kAttributeMutable)
{
	SetNewObject(new QuadEffectObject(radius, color, textureName));
	quadOrientation = 0;
}

QuadEffect::QuadEffect(const QuadEffect& quadEffect) :
		Effect(quadEffect),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		colorAttribute(kAttributeMutable)
{
	quadOrientation = quadEffect.quadOrientation;
}

QuadEffect::~QuadEffect()
{
}

Node *QuadEffect::Replicate(void) const
{
	return (new QuadEffect(*this));
}

void QuadEffect::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Effect::Pack(data, packFlags);

	data << ChunkHeader('ORNT', 4);
	data << quadOrientation;

	data << TerminatorChunk;
}

void QuadEffect::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Effect::Unpack(data, unpackFlags);
	UnpackChunkList<QuadEffect>(data, unpackFlags);
}

bool QuadEffect::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'ORNT':

			data >> quadOrientation;
			return (true);
	}

	return (false);
}

void QuadEffect::HandlePostprocessUpdate(void)
{
	const Vector3D& direction = GetWorldTransform()[2];
	for (machine a = 0; a < 4; a++)
	{
		quadVertex[a].position = direction;
	}

	if (vertexBuffer.Active())
	{
		vertexBuffer.UpdateBufferSync(0, sizeof(QuadVertex) * 4, quadVertex);
	}
}

bool QuadEffect::CalculateBoundingBox(Box3D *box) const
{
	float r = GetObject()->GetQuadRadius();
	box->min.Set(-r, -r, -r);
	box->max.Set(r, r, r);
	return (true);
}

bool QuadEffect::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	sphere->SetCenter(0.0F, 0.0F, 0.0F);
	sphere->SetRadius(GetObject()->GetQuadRadius());
	return (true);
}

bool QuadEffect::DirectionVisible(const Node *node, const VisibilityRegion *region)
{
	const QuadEffectObject *object = static_cast<const QuadEffect *>(node)->GetObject();
	return (region->DirectionVisible(node->GetWorldTransform()[2], object->GetQuadRadius() * 0.015625F));
}

bool QuadEffect::DirectionVisible(const Node *node, const VisibilityRegion *region, const List<OcclusionRegion> *occlusionList)
{
	const QuadEffectObject *object = static_cast<const QuadEffect *>(node)->GetObject();
	float radius = object->GetQuadRadius() * 0.015625F;
	const Vector3D& direction = node->GetWorldTransform()[2];

	if (region->DirectionVisible(direction, radius))
	{
		const OcclusionRegion *occluder = occlusionList->First();
		while (occluder)
		{
			if (occluder->DirectionOccluded(direction, radius))
			{
				return (false);
			}

			occluder = occluder->Next();
		}

		return (true);
	}

	return (false);
}

void QuadEffect::Preload(void)
{
	textureAttribute.SetTexture(GetObject()->GetQuadTextureName());

	Effect::Preload();
}

void QuadEffect::Preprocess(void)
{
	Effect::Preprocess();

	SetVertexCount(4);
	SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(QuadVertex));
	SetVertexAttributeArray(kArrayPosition, 0, 3);
	SetVertexAttributeArray(kArrayTexcoord, sizeof(Point3D), 2);
	vertexBuffer.Establish(sizeof(QuadVertex) * 4);

	unsigned_int32 flags = GetObject()->GetQuadFlags();
	if (flags & kQuadFogInhibit)
	{
		SetRenderableFlags(kRenderableFogInhibit);
	}

	if (!(flags & kQuadInfinite))
	{
		SetNodeFlags(GetNodeFlags() & ~kNodeInfiniteVisibility);
		SetActiveUpdateFlags(GetActiveUpdateFlags() & ~kUpdatePostprocess);

		SetTransformable(this);
		SetTransparentPosition(&GetWorldPosition());
		SetShaderFlags(kShaderAmbientEffect | kShaderVertexBillboard);
		SetVertexAttributeArray(kArrayBillboard, 0, 0);

		SetVisibilityProc(&SphereVisible);
		SetOcclusionProc(&SphereVisible);

		for (machine a = 0; a < 4; a++)
		{
			quadVertex[a].billboard.Set(0.0F, 0.0F);
		}
	}
	else
	{
		SetNodeFlags(GetNodeFlags() | kNodeInfiniteVisibility);
		SetActiveUpdateFlags(GetActiveUpdateFlags() | kUpdatePostprocess);

		SetTransformable(nullptr);
		SetShaderFlags(kShaderAmbientEffect | kShaderVertexInfinite | kShaderVertexBillboard);
		SetVertexAttributeArray(kArrayBillboard, sizeof(Point3D) + sizeof(Point2D), 2);

		SetVisibilityProc(&DirectionVisible);
		SetOcclusionProc(&DirectionVisible);
	}

	quadVertex[0].texcoord.Set(0.0F, 0.0F);
	quadVertex[1].texcoord.Set(1.0F, 0.0F);
	quadVertex[2].texcoord.Set(0.0F, 1.0F);
	quadVertex[3].texcoord.Set(1.0F, 1.0F);

	attributeList.Append(&colorAttribute);
	attributeList.Append(&textureAttribute);

	RenderSegment *segment = GetFirstRenderSegment();
	segment->SetMaterialAttributeList(&attributeList);
	segment->SetMaterialState(kMaterialTwoSided);

	QuadEffect::ProcessObjectSettings();
	QuadEffect::UpdateEffectGeometry();
}

void QuadEffect::ProcessObjectSettings(void)
{
	const QuadEffectObject *object = GetObject();
	SetAmbientBlendState(object->GetQuadBlendState());

	textureAttribute.SetTexture(object->GetQuadTextureName());

	unsigned_int32 flags = object->GetQuadFlags();
	SetEffectListIndex((flags & kQuadInfinite) ? kEffectListOpaque : kEffectListTransparent);

	if (flags & kQuadSoftDepth)
	{
		deltaDepthAttribute.SetDeltaScale(object->GetQuadSoftDepthScale());
		attributeList.Append(&deltaDepthAttribute);
	}
	else
	{
		deltaDepthAttribute.Detach();
	}

	InvalidateShaderData();
}

void QuadEffect::UpdateEffectGeometry(void)
{
	const QuadEffectObject *object = GetObject();

	float r = object->GetQuadRadius();
	int32 angle = quadOrientation;

	Vector2D cs = Math::GetTrigTable()[angle] * r;
	float u = cs.y + cs.x;
	float v = cs.y - cs.x;

	if (!(object->GetQuadFlags() & kQuadInfinite))
	{
		quadVertex[0].position.Set(v, u, 0.0F);
		quadVertex[1].position.Set(u, -v, 0.0F);
		quadVertex[2].position.Set(-u, v, 0.0F);
		quadVertex[3].position.Set(-v, -u, 0.0F);
	}
	else
	{
		u *= 0.015625F;
		v *= 0.015625F;

		quadVertex[0].billboard.Set(v, u);
		quadVertex[1].billboard.Set(u, -v);
		quadVertex[2].billboard.Set(-u, v);
		quadVertex[3].billboard.Set(-v, -u);
	}

	vertexBuffer.UpdateBufferSync(0, sizeof(QuadVertex) * 4, quadVertex);
}

void QuadEffect::Render(const FrustumCamera *camera, List<Renderable> *effectList)
{
	colorAttribute.SetDiffuseColor(GetObject()->GetQuadColor());
	effectList[GetEffectListIndex()].Append(this);
}


FlareEffectObject::FlareEffectObject() : EffectObject(kEffectFlare)
{
	flareFlags = 0;
}

FlareEffectObject::FlareEffectObject(float flare, float occlusion, float rotation, const char *textureName) : EffectObject(kEffectFlare)
{
	flareFlags = 0;
	flareRadius = flare;
	occlusionRadius = occlusion;
	rotationRadius = rotation;

	flareColor.Set(1.0F, 1.0F, 1.0F);
	flareTextureName = textureName;
}

FlareEffectObject::~FlareEffectObject()
{
}

void FlareEffectObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	EffectObject::Pack(data, packFlags);

	data << ChunkHeader('FLAG', 4);
	data << flareFlags;

	PackHandle handle = data.BeginChunk('DATA');
	data << flareRadius;
	data << occlusionRadius;
	data << rotationRadius;
	data << flareColor;
	data << flareTextureName;
	data.EndChunk(handle);

	data << TerminatorChunk;
}

void FlareEffectObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	EffectObject::Unpack(data, unpackFlags);
	UnpackChunkList<FlareEffectObject>(data, unpackFlags);
}

bool FlareEffectObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> flareFlags;
			return (true);

		case 'DATA':

			data >> flareRadius;
			data >> occlusionRadius;
			data >> rotationRadius;
			data >> flareColor;
			data >> flareTextureName;
			return (true);
	}

	return (false);
}

int32 FlareEffectObject::GetCategoryCount(void) const
{
	return (1);
}

Type FlareEffectObject::GetCategoryType(int32 index, const char **title) const
{
	if (index == 0)
	{
		*title = TheInterfaceMgr->GetStringTable()->GetString(StringID(kEffectFlare));
		return (kEffectFlare);
	}

	return (0);
}

int32 FlareEffectObject::GetCategorySettingCount(Type category) const
{
	if (category == kEffectFlare)
	{
		return (7);
	}

	return (0);
}

Setting *FlareEffectObject::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kEffectFlare)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();

		if (index == 0)
		{
			const char *title = table->GetString(StringID(kEffectFlare, 'FLAR'));
			return (new HeadingSetting(kEffectFlare, title));
		}

		if (index == 1)
		{
			const char *title = table->GetString(StringID(kEffectFlare, 'FLAR', 'COLR'));
			const char *picker = table->GetString(StringID(kEffectFlare, 'FLAR', 'CPCK'));
			return (new ColorSetting('COLR', flareColor, title, picker));
		}

		if (index == 2)
		{
			const char *title = table->GetString(StringID(kEffectFlare, 'FLAR', 'TNAM'));
			const char *picker = table->GetString(StringID(kEffectFlare, 'FLAR', 'TPCK'));
			return (new ResourceSetting('TNAM', flareTextureName, title, picker, TextureResource::GetDescriptor()));
		}

		if (index == 3)
		{
			if (flags & kConfigurationScript)
			{
				return (nullptr);
			}

			const char *title = table->GetString(StringID(kEffectFlare, 'FLAR', 'OCCL'));
			return (new TextSetting('OCCL', occlusionRadius, title));
		}

		if (index == 4)
		{
			if (flags & kConfigurationScript)
			{
				return (nullptr);
			}

			const char *title = table->GetString(StringID(kEffectFlare, 'FLAR', 'ROTA'));
			return (new TextSetting('ROTA', rotationRadius, title));
		}

		if (index == 5)
		{
			if (flags & kConfigurationScript)
			{
				return (nullptr);
			}

			const char *title = table->GetString(StringID(kEffectFlare, 'FLAR', 'RFOG'));
			return (new BooleanSetting('RFOG', ((flareFlags & kFlareFogInhibit) == 0), title));
		}

		if (index == 6)
		{
			if (flags & kConfigurationScript)
			{
				return (nullptr);
			}

			const char *title = table->GetString(StringID(kEffectFlare, 'FLAR', 'INFT'));
			return (new BooleanSetting('INFT', ((flareFlags & kFlareInfinite) != 0), title));
		}
	}

	return (nullptr);
}

void FlareEffectObject::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kEffectFlare)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'COLR')
		{
			flareColor = static_cast<const ColorSetting *>(setting)->GetColor();
		}
		else if (identifier == 'TNAM')
		{
			flareTextureName = static_cast<const ResourceSetting *>(setting)->GetResourceName();
		}
		else if (identifier == 'OCCL')
		{
			occlusionRadius = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
		}
		else if (identifier == 'ROTA')
		{
			rotationRadius = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
		}
		else if (identifier == 'RFOG')
		{
			if (!static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				flareFlags |= kFlareFogInhibit;
			}
			else
			{
				flareFlags &= ~kFlareFogInhibit;
			}
		}
		else if (identifier == 'INFT')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				flareFlags |= kFlareInfinite;
			}
			else
			{
				flareFlags &= ~kFlareInfinite;
			}
		}
	}
}

int32 FlareEffectObject::GetObjectSize(float *size) const
{
	size[0] = flareRadius;
	return (1);
}

void FlareEffectObject::SetObjectSize(const float *size)
{
	flareRadius = size[0];
}


FlareEffect::FlareEffect() :
		Effect(kEffectFlare, kRenderTriangleStrip),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		colorAttribute(kAttributeMutable),
		occlusionRenderable(kRenderTriangleStrip, kRenderDepthTest | kRenderColorInhibit | kRenderDepthInhibit),
		occlusionVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		occlusionQuery(&RenderFlare, this)
{
}

FlareEffect::FlareEffect(float flare, float occlusion, float rotation, const char *textureName) :
		Effect(kEffectFlare, kRenderTriangleStrip),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		colorAttribute(kAttributeMutable),
		occlusionRenderable(kRenderTriangleStrip, kRenderDepthTest | kRenderColorInhibit | kRenderDepthInhibit),
		occlusionVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		occlusionQuery(&RenderFlare, this)
{
	SetNewObject(new FlareEffectObject(flare, occlusion, rotation, textureName));
}

FlareEffect::FlareEffect(const FlareEffect& flareEffect) :
		Effect(flareEffect),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		colorAttribute(kAttributeMutable),
		occlusionRenderable(kRenderTriangleStrip, kRenderDepthTest | kRenderColorInhibit | kRenderDepthInhibit),
		occlusionVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		occlusionQuery(&RenderFlare, this)
{
}

FlareEffect::~FlareEffect()
{
}

Node *FlareEffect::Replicate(void) const
{
	return (new FlareEffect(*this));
}

void FlareEffect::HandlePostprocessUpdate(void)
{
	const Vector3D& direction = GetWorldTransform()[2];

	for (machine a = 0; a < 4; a++)
	{
		flareVertex[a].position = direction;
		occlusionVertex[a].position = direction;
	}

	if (vertexBuffer.Active())
	{
		vertexBuffer.UpdateBufferSync(0, sizeof(FlareVertex) * 4, flareVertex);
	}

	if (occlusionVertexBuffer.Active())
	{
		occlusionVertexBuffer.UpdateBufferSync(0, sizeof(OcclusionVertex) * 4, occlusionVertex);
	}
}

bool FlareEffect::CalculateBoundingBox(Box3D *box) const
{
	float r = GetObject()->GetOcclusionRadius();
	box->min.Set(-r, -r, -r);
	box->max.Set(r, r, r);
	return (true);
}

bool FlareEffect::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	sphere->SetCenter(0.0F, 0.0F, 0.0F);
	sphere->SetRadius(GetObject()->GetOcclusionRadius());
	return (true);
}

bool FlareEffect::DirectionVisible(const Node *node, const VisibilityRegion *region)
{
	const FlareEffectObject *object = static_cast<const FlareEffect *>(node)->GetObject();
	return (region->DirectionVisible(node->GetWorldTransform()[2], object->GetFlareRadius() * 0.015625F));
}

bool FlareEffect::DirectionVisible(const Node *node, const VisibilityRegion *region, const List<OcclusionRegion> *occlusionList)
{
	const FlareEffectObject *object = static_cast<const FlareEffect *>(node)->GetObject();
	float radius = object->GetFlareRadius() * 0.015625F;
	const Vector3D& direction = node->GetWorldTransform()[2];

	if (region->DirectionVisible(direction, radius))
	{
		const OcclusionRegion *occluder = occlusionList->First();
		while (occluder)
		{
			if (occluder->DirectionOccluded(direction, radius))
			{
				return (false);
			}

			occluder = occluder->Next();
		}

		return (true);
	}

	return (false);
}

void FlareEffect::Preload(void)
{
	textureAttribute.SetTexture(GetObject()->GetFlareTextureName());

	Effect::Preload();
}

void FlareEffect::Preprocess(void)
{
	Effect::Preprocess();

	SetEffectListIndex(kEffectListFrontmost);
	SetAmbientBlendState(BlendState(kBlendSourceAlpha, kBlendOne));

	SetVertexCount(4);
	SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(FlareVertex));
	SetVertexAttributeArray(kArrayPosition, 0, 3);
	SetVertexAttributeArray(kArrayTexcoord, sizeof(Point3D), 2);
	vertexBuffer.Establish(sizeof(FlareVertex) * 4);

	occlusionRenderable.SetVertexCount(4);
	occlusionRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &occlusionVertexBuffer, sizeof(OcclusionVertex));
	occlusionRenderable.SetVertexAttributeArray(kArrayPosition, 0, 3);
	occlusionVertexBuffer.Establish(sizeof(OcclusionVertex) * 4);

	unsigned_int32 flags = GetObject()->GetFlareFlags();
	if (flags & kFlareFogInhibit)
	{
		SetRenderableFlags(kRenderableFogInhibit);
	}

	if (!(flags & kFlareInfinite))
	{
		SetNodeFlags(GetNodeFlags() & ~kNodeInfiniteVisibility);
		SetActiveUpdateFlags(GetActiveUpdateFlags() & ~kUpdatePostprocess);

		SetTransformable(this);
		SetShaderFlags(kShaderAmbientEffect | kShaderVertexBillboard);
		SetVertexAttributeArray(kArrayBillboard, 0, 0);

		occlusionRenderable.SetShaderFlags(kShaderAmbientEffect);
		occlusionRenderable.SetVertexAttributeArray(kArrayBillboard, 0, 0);

		SetVisibilityProc(&SphereVisible);
		SetOcclusionProc(&SphereVisible);

		for (machine a = 0; a < 4; a++)
		{
			flareVertex[a].billboard.Set(0.0F, 0.0F);
			occlusionVertex[a].billboard.Set(0.0F, 0.0F);
		}
	}
	else
	{
		SetNodeFlags(GetNodeFlags() | kNodeInfiniteVisibility);
		SetActiveUpdateFlags(GetActiveUpdateFlags() | kUpdatePostprocess);

		SetTransformable(nullptr);
		SetShaderFlags(kShaderAmbientEffect | kShaderVertexInfinite | kShaderVertexBillboard);
		SetVertexAttributeArray(kArrayBillboard, sizeof(Point3D) + sizeof(Point2D), 2);

		occlusionRenderable.SetShaderFlags(kShaderAmbientEffect | kShaderVertexInfinite | kShaderVertexBillboard);
		occlusionRenderable.SetVertexAttributeArray(kArrayBillboard, sizeof(Point3D), 2);

		SetVisibilityProc(&DirectionVisible);
		SetOcclusionProc(&DirectionVisible);
	}

	flareVertex[0].texcoord.Set(0.0F, 0.0F);
	flareVertex[1].texcoord.Set(1.0F, 0.0F);
	flareVertex[2].texcoord.Set(0.0F, 1.0F);
	flareVertex[3].texcoord.Set(1.0F, 1.0F);

	attributeList.Append(&colorAttribute);
	attributeList.Append(&textureAttribute);

	RenderSegment *segment = GetFirstRenderSegment();
	segment->SetMaterialAttributeList(&attributeList);
	segment->SetMaterialState(kMaterialTwoSided);

	occlusionRenderable.SetRenderableFlags(kRenderableFogInhibit);
	occlusionRenderable.SetOcclusionQuery(&occlusionQuery);
	occlusionRenderable.GetFirstRenderSegment()->SetMaterialState(kMaterialTwoSided);

	FlareEffect::ProcessObjectSettings();
	FlareEffect::UpdateEffectGeometry();
}

void FlareEffect::ProcessObjectSettings(void)
{
	textureAttribute.SetTexture(GetObject()->GetFlareTextureName());
	InvalidateShaderData();
}

void FlareEffect::UpdateEffectGeometry(void)
{
	const FlareEffectObject *object = GetObject();
	float r = object->GetFlareRadius();

	if (!(object->GetFlareFlags() & kFlareInfinite))
	{
		flareVertex[0].position.Set(-r, r, 0.0F);
		flareVertex[1].position.Set(r, r, 0.0F);
		flareVertex[2].position.Set(-r, -r, 0.0F);
		flareVertex[3].position.Set(r, -r, 0.0F);

		vertexBuffer.UpdateBufferSync(0, sizeof(FlareVertex) * 4, flareVertex);
	}
	else
	{
		r *= 0.015625F;
		flareVertex[0].billboard.Set(-r, r);
		flareVertex[1].billboard.Set(r, r);
		flareVertex[2].billboard.Set(-r, -r);
		flareVertex[3].billboard.Set(r, -r);

		vertexBuffer.UpdateBufferSync(0, sizeof(FlareVertex) * 4, flareVertex);

		r = object->GetOcclusionRadius() * 0.015625F;
		inverseOcclusionSize = 1.0F / r;

		occlusionVertex[0].billboard.Set(-r, r);
		occlusionVertex[1].billboard.Set(r, r);
		occlusionVertex[2].billboard.Set(-r, -r);
		occlusionVertex[3].billboard.Set(r, -r);

		occlusionVertexBuffer.UpdateBufferSync(0, sizeof(OcclusionVertex) * 4, occlusionVertex);
	}
}

void FlareEffect::Render(const FrustumCamera *camera, List<Renderable> *effectList)
{
	const FlareEffectObject *object = GetObject();

	if (!(object->GetFlareFlags() & kFlareInfinite))
	{
		const Point3D& p = GetWorldPosition();
		Vector3D offset = camera->GetWorldPosition() - p;
		Point3D q = p + offset * (object->GetRotationRadius() * InverseMag(offset));

		float r = object->GetOcclusionRadius();
		float d = (q - camera->GetWorldPosition()) * camera->GetWorldTransform()[2];
		float e = static_cast<FrustumCameraObject *>(camera->Node::GetObject())->GetFocalLength();
		inverseOcclusionSize = FmaxZero(d) / (r * e);

		const Vector3D& right = camera->GetWorldTransform()[0];
		const Vector3D& down = camera->GetWorldTransform()[1];

		Vector3D v1 = (right + down) * r;
		Vector3D v2 = (right - down) * r;

		occlusionVertex[0].position = q - v1;
		occlusionVertex[1].position = q - v2;
		occlusionVertex[2].position = q + v2;
		occlusionVertex[3].position = q + v1;

		occlusionVertexBuffer.UpdateBufferSync(0, sizeof(OcclusionVertex) * 4, occlusionVertex);
	}

	effectList[kEffectListOcclusion].Append(&occlusionRenderable);
}

void FlareEffect::RenderFlare(OcclusionQuery *query, List<Renderable> *renderList, void *cookie)
{
	FlareEffect *flareEffect = static_cast<FlareEffect *>(cookie);

	float size = flareEffect->inverseOcclusionSize;
	float intensity = Fmin(query->GetUnoccludedArea() * (size * size), 1.0F);

	const ColorRGB& flareColor = flareEffect->GetObject()->GetFlareColor();
	flareEffect->colorAttribute.SetDiffuseColor(ColorRGBA(flareColor * intensity, 1.0F));

	renderList->Append(flareEffect);
}


BeamEffectObject::BeamEffectObject() : EffectObject(kEffectBeam)
{
	beamFlags = 0;
	beamDeltaScale = 1.0F;
	beamTexcoordScale = 1.0F;
	beamTextureName[0] = 0;
}

BeamEffectObject::BeamEffectObject(float radius, float height, const ColorRGBA& color, const char *textureName) : EffectObject(kEffectBeam)
{
	beamFlags = 0;
	beamRadius = radius;
	beamHeight = height;

	beamColor = color;
	beamDeltaScale = 1.0F;
	beamTexcoordScale = 1.0F;

	if (textureName)
	{
		beamTextureName = textureName;
	}
	else
	{
		beamTextureName[0] = 0;
	}
}

BeamEffectObject::~BeamEffectObject()
{
}

void BeamEffectObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	EffectObject::Pack(data, packFlags);

	data << ChunkHeader('FLAG', 4);
	data << beamFlags;

	data << ChunkHeader('SIZE', 8);
	data << beamRadius;
	data << beamHeight;

	data << ChunkHeader('COLR', sizeof(ColorRGBA));
	data << beamColor;

	data << ChunkHeader('DSCL', 4);
	data << beamDeltaScale;

	data << ChunkHeader('TSCL', 4);
	data << beamTexcoordScale;

	if (beamTextureName[0] != 0)
	{
		PackHandle handle = data.BeginChunk('TXTR');
		data << beamTextureName;
		data.EndChunk(handle);
	}

	data << TerminatorChunk;
}

void BeamEffectObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	EffectObject::Unpack(data, unpackFlags);
	UnpackChunkList<BeamEffectObject>(data, unpackFlags);
}

bool BeamEffectObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> beamFlags;
			return (true);

		case 'SIZE':

			data >> beamRadius;
			data >> beamHeight;
			return (true);

		case 'COLR':

			data >> beamColor;
			return (true);

		case 'DSCL':

			data >> beamDeltaScale;
			return (true);

		case 'TSCL':

			data >> beamTexcoordScale;
			return (true);

		case 'TXTR':

			data >> beamTextureName;
			return (true);
	}

	return (false);
}

void *BeamEffectObject::BeginSettingsUnpack(void)
{
	beamTextureName[0] = 0;
	return (EffectObject::BeginSettingsUnpack());
}

int32 BeamEffectObject::GetCategoryCount(void) const
{
	return (1);
}

Type BeamEffectObject::GetCategoryType(int32 index, const char **title) const
{
	if (index == 0)
	{
		*title = TheInterfaceMgr->GetStringTable()->GetString(StringID(kEffectBeam));
		return (kEffectBeam);
	}

	return (0);
}

int32 BeamEffectObject::GetCategorySettingCount(Type category) const
{
	if (category == kEffectBeam)
	{
		return (6);
	}

	return (0);
}

Setting *BeamEffectObject::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kEffectBeam)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();

		if (index == 0)
		{
			const char *title = table->GetString(StringID(kEffectBeam, 'BEAM'));
			return (new HeadingSetting(kEffectBeam, title));
		}

		if (index == 1)
		{
			const char *title = table->GetString(StringID(kEffectBeam, 'BEAM', 'COLR'));
			const char *picker = table->GetString(StringID(kEffectBeam, 'BEAM', 'CPCK'));
			return (new ColorSetting('COLR', beamColor, title, picker, kColorPickerAlpha));
		}

		if (index == 2)
		{
			const char *title = table->GetString(StringID(kEffectBeam, 'BEAM', 'TNAM'));
			const char *picker = table->GetString(StringID(kEffectBeam, 'BEAM', 'TPCK'));
			return (new ResourceSetting('TNAM', beamTextureName, title, picker, TextureResource::GetDescriptor()));
		}

		if (index == 3)
		{
			if (flags & kConfigurationScript)
			{
				return (nullptr);
			}

			const char *title = table->GetString(StringID(kEffectBeam, 'BEAM', 'TSCL'));
			return (new TextSetting('TSCL', beamTexcoordScale, title));
		}

		if (index == 4)
		{
			if (flags & kConfigurationScript)
			{
				return (nullptr);
			}

			const char *title = table->GetString(StringID(kEffectBeam, 'BEAM', 'SOFT'));
			return (new BooleanSetting('SOFT', ((beamFlags & kBeamSoftDepth) != 0), title));
		}

		if (index == 5)
		{
			if (flags & kConfigurationScript)
			{
				return (nullptr);
			}

			const char *title = table->GetString(StringID(kEffectBeam, 'BEAM', 'DSCL'));
			return (new TextSetting('DSCL', beamDeltaScale, title));
		}
	}

	return (nullptr);
}

void BeamEffectObject::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kEffectBeam)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'COLR')
		{
			beamColor = static_cast<const ColorSetting *>(setting)->GetColor();
		}
		else if (identifier == 'TNAM')
		{
			beamTextureName = static_cast<const ResourceSetting *>(setting)->GetResourceName();
		}
		else if (identifier == 'TSCL')
		{
			beamTexcoordScale = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
		}
		else if (identifier == 'SOFT')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				beamFlags |= kBeamSoftDepth;
			}
			else
			{
				beamFlags &= ~kBeamSoftDepth;
			}
		}
		else if (identifier == 'DSCL')
		{
			beamDeltaScale = FmaxZero(Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText()));
		}
	}
}

int32 BeamEffectObject::GetObjectSize(float *size) const
{
	size[0] = beamRadius;
	size[1] = beamHeight;
	return (2);
}

void BeamEffectObject::SetObjectSize(const float *size)
{
	beamRadius = size[0];
	beamHeight = size[1];
}


BeamEffect::BeamEffect() :
		Effect(kEffectBeam, kRenderTriangleStrip, kRenderDepthTest | kRenderDepthInhibit),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
{
}

BeamEffect::BeamEffect(float radius, float height, const ColorRGBA& color, const char *textureName) :
		Effect(kEffectBeam, kRenderTriangleStrip, kRenderDepthTest | kRenderDepthInhibit),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
{
	SetNewObject(new BeamEffectObject(radius, height, color, textureName));
}

BeamEffect::BeamEffect(const BeamEffect& beamEffect) :
		Effect(beamEffect),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
{
}

BeamEffect::~BeamEffect()
{
}

Node *BeamEffect::Replicate(void) const
{
	return (new BeamEffect(*this));
}

bool BeamEffect::CalculateBoundingBox(Box3D *box) const
{
	const BeamEffectObject *object = GetObject();
	float r = object->GetBeamRadius();
	float h = object->GetBeamHeight();

	box->min.Set(-r, -r, 0.0F);
	box->max.Set(r, r, h);
	return (true);
}

bool BeamEffect::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	const BeamEffectObject *object = GetObject();
	float r = object->GetBeamRadius();
	float h = object->GetBeamHeight() * 0.5F;

	sphere->SetCenter(0.0F, 0.0F, h);
	sphere->SetRadius(Sqrt(h * h + r * r));
	return (true);
}

void BeamEffect::Preload(void)
{
	const char *textureName = GetObject()->GetBeamTextureName();
	if (textureName[0] != 0)
	{
		textureAttribute.SetTexture(textureName);
	}
	else
	{
		textureAttribute.SetTexture(&beamTextureHeader, beamTextureImage);
	}

	Effect::Preload();
}

void BeamEffect::Preprocess(void)
{
	Effect::Preprocess();

	SetTransformable(this);
	SetTransparentPosition(&GetWorldPosition());
	SetShaderFlags(kShaderAmbientEffect | kShaderVertexPolyboard | kShaderLinearPolyboard);
	SetAmbientBlendState(BlendState(kBlendSourceAlpha, kBlendOne));

	SetVertexCount(4);
	SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(BeamVertex));
	SetVertexAttributeArray(kArrayPosition, 0, 3);
	SetVertexAttributeArray(kArrayTangent, sizeof(Point3D), 4);
	SetVertexAttributeArray(kArrayTexcoord, sizeof(Point3D) + sizeof(Vector4D), 2);
	vertexBuffer.Establish(sizeof(BeamVertex) * 4);

	attributeList.Append(&colorAttribute);
	attributeList.Append(&textureAttribute);
	SetMaterialAttributeList(&attributeList);

	BeamEffect::ProcessObjectSettings();
	BeamEffect::UpdateEffectGeometry();
}

void BeamEffect::ProcessObjectSettings(void)
{
	const BeamEffectObject *object = GetObject();

	colorAttribute.SetDiffuseColor(object->GetBeamColor());

	const char *textureName = object->GetBeamTextureName();
	if (textureName[0] != 0)
	{
		textureAttribute.SetTexture(textureName);
	}
	else
	{
		textureAttribute.SetTexture(&beamTextureHeader, beamTextureImage);
	}

	if (object->GetBeamFlags() & kBeamSoftDepth)
	{
		deltaDepthAttribute.SetDeltaScale(object->GetBeamSoftDepthScale());
		attributeList.Append(&deltaDepthAttribute);
	}
	else
	{
		deltaDepthAttribute.Detach();
	}

	InvalidateShaderData();
}

void BeamEffect::UpdateEffectGeometry(void)
{
	volatile BeamVertex *restrict beamVertex = vertexBuffer.BeginUpdateSync<BeamVertex>();

	const BeamEffectObject *object = GetObject();
	float height = object->GetBeamHeight();
	float radius = object->GetBeamRadius();
	float scale = object->GetBeamTexcoordScale();

	beamVertex[0].position.Set(0.0F, 0.0F, 0.0F);
	beamVertex[0].tangent.Set(0.0F, 0.0F, 1.0F, -radius);
	beamVertex[0].texcoord.Set(0.0F, 0.0F);

	beamVertex[1].position.Set(0.0F, 0.0F, 0.0F);
	beamVertex[1].tangent.Set(0.0F, 0.0F, 1.0F, radius);
	beamVertex[1].texcoord.Set(1.0F, 0.0F);

	beamVertex[2].position.Set(0.0F, 0.0F, height);
	beamVertex[2].tangent.Set(0.0F, 0.0F, 1.0F, -radius);
	beamVertex[2].texcoord.Set(0.0F, scale);

	beamVertex[3].position.Set(0.0F, 0.0F, height);
	beamVertex[3].tangent.Set(0.0F, 0.0F, 1.0F, radius);
	beamVertex[3].texcoord.Set(1.0F, scale);

	vertexBuffer.EndUpdateSync();
}

void BeamEffect::Render(const FrustumCamera *camera, List<Renderable> *effectList)
{
	colorAttribute.SetDiffuseColor(GetObject()->GetBeamColor());
	effectList[GetEffectListIndex()].Append(this);
}


TubeEffectObject::TubeEffectObject(EffectType type) :
		EffectObject(type),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
{
	texcoordScale = 1.0F;
	maxSubdiv = 16;

	tubeTextureName[0] = 0;
	tubeVertex = nullptr;
}

TubeEffectObject::TubeEffectObject(const Path *path, float radius, const ColorRGBA& color, const char *textureName) :
		EffectObject(kEffectTube),
		tubePath(*path),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
{
	tubeRadius = radius;
	tubePath.GetBoundingBox(&pathBoundingBox);

	tubeColor = color;

	if (textureName)
	{
		tubeTextureName = textureName;
	}
	else
	{
		tubeTextureName[0] = 0;
	}

	texcoordScale = 1.0F;
	maxSubdiv = 16;

	tubeVertex = nullptr;
}

TubeEffectObject::TubeEffectObject(EffectType type, const Path *path, float radius, const ColorRGBA& color, const char *textureName) :
		EffectObject(type),
		tubePath(*path),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
{
	tubeRadius = radius;
	tubePath.GetBoundingBox(&pathBoundingBox);

	tubeColor = color;

	if (textureName)
	{
		tubeTextureName = textureName;
	}
	else
	{
		tubeTextureName[0] = 0;
	}

	texcoordScale = 1.0F;
	maxSubdiv = 16;

	tubeVertex = nullptr;
}

TubeEffectObject::~TubeEffectObject()
{
	delete[] tubeVertex;
}

void TubeEffectObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	EffectObject::Pack(data, packFlags);

	data << ChunkHeader('RADI', 4);
	data << tubeRadius;

	PackHandle handle = data.BeginChunk('PATH');
	tubePath.Pack(data, packFlags);
	data.EndChunk(handle);

	data << ChunkHeader('COLR', sizeof(ColorRGBA));
	data << tubeColor;

	data << ChunkHeader('SBDV', 4);
	data << maxSubdiv;

	if (tubeTextureName[0] != 0)
	{
		data << ChunkHeader('TSCL', 4);
		data << texcoordScale;

		handle = data.BeginChunk('TXTR');
		data << tubeTextureName;
		data.EndChunk(handle);
	}

	if (tubeVertex)
	{
		int32 count = tubeVertexCount;

		data << ChunkHeader('VRTX', 4 + count * sizeof(TubeVertex));
		data << count;
		data.WriteArray(count, tubeVertex);
	}

	data << TerminatorChunk;
}

void TubeEffectObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	EffectObject::Unpack(data, unpackFlags);
	UnpackChunkList<TubeEffectObject>(data, unpackFlags);
}

bool TubeEffectObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'RADI':

			data >> tubeRadius;
			return (true);

		case 'PATH':

			tubePath.Unpack(data, unpackFlags);
			tubePath.GetBoundingBox(&pathBoundingBox);
			return (true);

		case 'COLR':

			data >> tubeColor;
			return (true);

		case 'SBDV':

			data >> maxSubdiv;
			return (true);

		case 'TSCL':

			data >> texcoordScale;
			return (true);

		case 'TXTR':

			data >> tubeTextureName;
			return (true);

		case 'VRTX':
		{
			int32	count;

			data >> count;
			AllocateStorage(count);
			data.ReadArray(count, tubeVertex);

			vertexBuffer.UpdateBufferSync(0, sizeof(TubeVertex) * count, tubeVertex);
			return (true);
		}
	}

	return (false);
}

void *TubeEffectObject::BeginSettingsUnpack(void)
{
	tubePath.BeginSettingsUnpack();
	tubeTextureName[0] = 0;

	delete[] tubeVertex;
	tubeVertex = nullptr;

	vertexBuffer.Establish(0);

	return (EffectObject::BeginSettingsUnpack());
}

int32 TubeEffectObject::GetCategoryCount(void) const
{
	return (1);
}

Type TubeEffectObject::GetCategoryType(int32 index, const char **title) const
{
	if (index == 0)
	{
		*title = TheInterfaceMgr->GetStringTable()->GetString(StringID(kEffectTube));
		return (kEffectTube);
	}

	return (0);
}

int32 TubeEffectObject::GetCategorySettingCount(Type category) const
{
	if (category == kEffectTube)
	{
		return (5);
	}

	return (0);
}

Setting *TubeEffectObject::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kEffectTube)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();

		if (index == 0)
		{
			const char *title = table->GetString(StringID(kEffectTube, 'TUBE'));
			return (new HeadingSetting(kEffectTube, title));
		}

		if (index == 1)
		{
			const char *title = table->GetString(StringID(kEffectTube, 'TUBE', 'COLR'));
			const char *picker = table->GetString(StringID(kEffectTube, 'TUBE', 'CPCK'));
			return (new ColorSetting('COLR', tubeColor, title, picker, kColorPickerAlpha));
		}

		if (index == 2)
		{
			const char *title = table->GetString(StringID(kEffectTube, 'TUBE', 'TNAM'));
			const char *picker = table->GetString(StringID(kEffectTube, 'TUBE', 'TPCK'));
			return (new ResourceSetting('TNAM', tubeTextureName, title, picker, TextureResource::GetDescriptor()));
		}

		if (index == 3)
		{
			if (flags & kConfigurationScript)
			{
				return (nullptr);
			}

			const char *title = table->GetString(StringID(kEffectTube, 'TUBE', 'TSCL'));
			return (new TextSetting('TSCL', texcoordScale, title));
		}

		if (index == 4)
		{
			if (flags & kConfigurationScript)
			{
				return (nullptr);
			}

			const char *title = table->GetString(StringID(kEffectTube, 'TUBE', 'SBDV'));
			return (new TextSetting('SBDV', Text::IntegerToString(maxSubdiv), title, 2, &EditTextWidget::NumberFilter));
		}
	}

	return (nullptr);
}

void TubeEffectObject::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kEffectTube)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'COLR')
		{
			tubeColor = static_cast<const ColorSetting *>(setting)->GetColor();
		}
		else if (identifier == 'TNAM')
		{
			tubeTextureName = static_cast<const ResourceSetting *>(setting)->GetResourceName();
		}
		else if (identifier == 'TSCL')
		{
			texcoordScale = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
		}
		else if (identifier == 'SBDV')
		{
			maxSubdiv = Max(Text::StringToInteger(static_cast<const TextSetting *>(setting)->GetText()), 1);
		}
	}
}

int32 TubeEffectObject::GetObjectSize(float *size) const
{
	size[0] = tubeRadius;
	return (1);
}

void TubeEffectObject::SetObjectSize(const float *size)
{
	tubeRadius = size[0];
}

void TubeEffectObject::SetTubePath(const Path *path)
{
	tubePath = *path;
	tubePath.GetBoundingBox(&pathBoundingBox);
}

void TubeEffectObject::AllocateStorage(int32 vertexCount)
{
	if ((!tubeVertex) || (tubeVertexCount != vertexCount))
	{
		delete[] tubeVertex;

		tubeVertexCount = vertexCount;
		tubeVertex = new TubeVertex[vertexCount];

		vertexBuffer.Establish(sizeof(TubeVertex) * vertexCount);
	}
}

void TubeEffectObject::Build(void)
{
	int32 zdiv = Max(maxSubdiv, 1);
	float dz = 1.0F / (float) zdiv;

	int32 vertexCount = 2;
	float totalLength = 0.0F;

	const PathComponent *component = tubePath.GetFirstPathComponent();
	const Point3D& pathBeginPosition = component->GetBeginPosition();

	do
	{
		if (component->GetPathType() == kPathLinear)
		{
			vertexCount += 2;

			const LinearPathComponent *linearComponent = static_cast<const LinearPathComponent *>(component);
			totalLength += Magnitude(linearComponent->GetControlPoint(1) - linearComponent->GetControlPoint(0));
		}
		else
		{
			vertexCount += zdiv * 2;

			Point3D p1 = component->GetBeginPosition();
			for (machine j = 1; j <= zdiv; j++)
			{
				Point3D p2 = component->GetPosition((float) j * dz);
				totalLength += Magnitude(p2 - p1);
				p1 = p2;
			}
		}

		component = component->Next();
	} while (component);

	if (vertexCount != 0)
	{
		AllocateStorage(vertexCount);
		TubeVertex *vertex = tubeVertex;

		float radius = tubeRadius;
		float invLength = 1.0F / totalLength;
		float tex = 0.0F;

		component = tubePath.GetFirstPathComponent();
		for (;;)
		{
			if (component->GetPathType() == kPathLinear)
			{
				const Point3D& beginPosition = component->GetBeginPosition();
				Vector3D beginTangent = Normalize(component->GetBeginTangent());

				vertex[0].position = beginPosition;
				vertex[0].tangent.Set(beginTangent, -radius);
				vertex[0].texcoord.Set(0.0F, tex * texcoordScale);

				vertex[1].position = beginPosition;
				vertex[1].tangent.Set(beginTangent, radius);
				vertex[1].texcoord.Set(1.0F, tex * texcoordScale);

				vertex += 2;

				const LinearPathComponent *linearComponent = static_cast<const LinearPathComponent *>(component);
				tex += Magnitude(linearComponent->GetControlPoint(1) - linearComponent->GetControlPoint(0)) * invLength;
			}
			else
			{
				Point3D p1 = component->GetBeginPosition();
				Vector3D tang = Normalize(component->GetBeginTangent());

				vertex[0].position = p1;
				vertex[0].tangent.Set(tang, -radius);
				vertex[0].texcoord.Set(0.0F, tex * texcoordScale);

				vertex[1].position = p1;
				vertex[1].tangent.Set(tang, radius);
				vertex[1].texcoord.Set(1.0F, tex * texcoordScale);

				vertex += 2;

				for (machine j = 1; j < zdiv; j++)
				{
					float t = (float) j * dz;
					Point3D p2 = component->GetPosition(t);
					tang = Normalize(component->GetTangent(t));

					tex += Magnitude(p2 - p1) * invLength;
					p1 = p2;

					vertex[0].position = p2;
					vertex[0].tangent.Set(tang, -radius);
					vertex[0].texcoord.Set(0.0F, tex * texcoordScale);

					vertex[1].position = p2;
					vertex[1].tangent.Set(tang, radius);
					vertex[1].texcoord.Set(1.0F, tex * texcoordScale);

					vertex += 2;
				}

				tex += Magnitude(component->GetEndPosition() - p1) * invLength;
			}

			const PathComponent *nextComponent = component->Next();
			if (!nextComponent)
			{
				const Point3D	*endPosition;
				Vector3D		endTangent;

				if (tubePath.GetLastPathComponent()->GetEndPosition() == pathBeginPosition)
				{
					endPosition = &pathBeginPosition;
					endTangent = tubePath.GetFirstPathComponent()->GetBeginTangent();
				}
				else
				{
					endPosition = &component->GetEndPosition();
					endTangent = component->GetEndTangent();
				}

				endTangent.Normalize();

				vertex[0].position = *endPosition;
				vertex[0].tangent.Set(endTangent, -radius);
				vertex[0].texcoord.Set(0.0F, texcoordScale);

				vertex[1].position = *endPosition;
				vertex[1].tangent.Set(endTangent, radius);
				vertex[1].texcoord.Set(1.0F, texcoordScale);

				break;
			}

			component = nextComponent;
		}

		vertexBuffer.UpdateBuffer(0, sizeof(TubeVertex) * vertexCount, tubeVertex);
	}
	else
	{
		delete[] tubeVertex;
		tubeVertexCount = 0;

		vertexBuffer.Establish(0);
	}
}


TubeEffect::TubeEffect(EffectType type) : Effect(type, kRenderTriangleStrip, kRenderDepthTest | kRenderDepthInhibit)
{
}

TubeEffect::TubeEffect(const Path *path, float radius, const ColorRGBA& color, const char *textureName) : Effect(kEffectTube, kRenderTriangleStrip, kRenderDepthTest | kRenderDepthInhibit)
{
	SetNewObject(new TubeEffectObject(path, radius, color, textureName));
}

TubeEffect::TubeEffect(EffectType type, const Path *path, float radius, const ColorRGBA& color, const char *textureName) : Effect(type, kRenderTriangleStrip, kRenderDepthTest | kRenderDepthInhibit)
{
}

TubeEffect::TubeEffect(const TubeEffect& tubeEffect) : Effect(tubeEffect)
{
}

TubeEffect::~TubeEffect()
{
}

Node *TubeEffect::Replicate(void) const
{
	return (new TubeEffect(*this));
}

void TubeEffect::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Effect::Pack(data, packFlags);

	data << TerminatorChunk;
}

void TubeEffect::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Effect::Unpack(data, unpackFlags);
	UnpackChunkList<TubeEffect>(data, unpackFlags);
}

bool TubeEffect::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	return (false);
}

bool TubeEffect::CalculateBoundingBox(Box3D *box) const
{
	*box = GetObject()->GetPathBoundingBox();
	return (true);
}

bool TubeEffect::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	const TubeEffectObject *object = GetObject();
	const Box3D& bounds = object->GetPathBoundingBox();

	sphere->SetCenter((bounds.min + bounds.max) * 0.5F);
	sphere->SetRadius(Magnitude(bounds.max - bounds.min) * 0.5F + object->GetTubeRadius());
	return (true);
}

int32 TubeEffect::GetInternalConnectorCount(void) const
{
	return (1);
}

const char *TubeEffect::GetInternalConnectorKey(int32 index) const
{
	if (index == 0)
	{
		return (kConnectorKeyPath);
	}

	return (nullptr);
}

bool TubeEffect::ValidConnectedNode(const ConnectorKey& key, const Node *node) const
{
	if (key == kConnectorKeyPath)
	{
		if (node->GetNodeType() == kNodeMarker)
		{
			return (static_cast<const Marker *>(node)->GetMarkerType() == kMarkerPath);
		}

		return (false);
	}

	return (Effect::ValidConnectedNode(key, node));
}

PathMarker *TubeEffect::GetConnectedPathMarker(void) const
{
	Node *node = GetConnectedNode(kConnectorKeyPath);
	if (node)
	{
		return (static_cast<PathMarker *>(node));
	}

	return (nullptr);
}

void TubeEffect::SetConnectedPathMarker(PathMarker *marker)
{
	SetConnectedNode(kConnectorKeyPath, marker);
}

void TubeEffect::Preload(void)
{
	const char *textureName = GetObject()->GetTubeTextureName();
	if (textureName[0] != 0)
	{
		textureAttribute.SetTexture(textureName);
	}
	else
	{
		textureAttribute.SetTexture(&beamTextureHeader, beamTextureImage);
	}

	Effect::Preload();
}

void TubeEffect::Preprocess(void)
{
	Effect::Preprocess();

	SetTransformable(this);
	SetTransparentPosition(&GetWorldPosition());
	SetShaderFlags(kShaderAmbientEffect | kShaderVertexPolyboard);
	SetAmbientBlendState(kBlendAccumulate);

	TubeEffectObject *object = GetObject();

	SetVertexCount(object->GetVertexCount());
	SetVertexBuffer(kVertexBufferAttributeArray, object->GetVertexBuffer(), sizeof(TubeEffectObject::TubeVertex));
	SetVertexAttributeArray(kArrayPosition, 0, 3);
	SetVertexAttributeArray(kArrayTangent, sizeof(Point3D), 4);
	SetVertexAttributeArray(kArrayTexcoord, sizeof(Point3D) + sizeof(Vector4D), 2);

	attributeList.Append(&colorAttribute);
	attributeList.Append(&textureAttribute);
	SetMaterialAttributeList(&attributeList);

	TubeEffect::ProcessObjectSettings();
}

void TubeEffect::ProcessObjectSettings(void)
{
	const TubeEffectObject *object = GetObject();

	colorAttribute.SetDiffuseColor(object->GetTubeColor());

	const char *textureName = object->GetTubeTextureName();
	if (textureName[0] != 0)
	{
		textureAttribute.SetTexture(textureName);
	}
	else
	{
		textureAttribute.SetTexture(&beamTextureHeader, beamTextureImage);
	}

	InvalidateShaderData();
}


BoltEffectObject::BoltEffectObject() : TubeEffectObject(kEffectBolt)
{
}

BoltEffectObject::BoltEffectObject(const Path *path, float radius, float deviation, const ColorRGBA& color, const char *textureName) : TubeEffectObject(kEffectBolt, path, radius, color, textureName)
{
	maxPathDeviation = deviation;

	branchingDepth = 0;
	branchCount = 2;
	branchRadiusScale = 0.5F;
	branchLengthRange.min = 1.0F;
	branchLengthRange.max = 2.0F;
}

BoltEffectObject::~BoltEffectObject()
{
}

void BoltEffectObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	TubeEffectObject::Pack(data, packFlags);

	data << ChunkHeader('DEVI', 4);
	data << maxPathDeviation;

	data << ChunkHeader('BRCH', 12 + sizeof(Range<float>));
	data << branchingDepth;
	data << branchCount;
	data << branchRadiusScale;
	data << branchLengthRange;

	data << TerminatorChunk;
}

void BoltEffectObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	TubeEffectObject::Unpack(data, unpackFlags);
	UnpackChunkList<BoltEffectObject>(data, unpackFlags);
}

bool BoltEffectObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'DEVI':

			data >> maxPathDeviation;
			return (true);

		case 'BRCH':

			data >> branchingDepth;
			data >> branchCount;
			data >> branchRadiusScale;
			data >> branchLengthRange;
			return (true);
	}

	return (false);
}

int32 BoltEffectObject::GetCategorySettingCount(Type category) const
{
	int32 count = TubeEffectObject::GetCategorySettingCount(category);
	if (category == kEffectTube)
	{
		count += 7;
	}

	return (count);
}

Setting *BoltEffectObject::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kEffectTube)
	{
		int32 i = index - TubeEffectObject::GetCategorySettingCount(kEffectTube);
		if (i >= 0)
		{
			const StringTable *table = TheInterfaceMgr->GetStringTable();

			if (i == 0)
			{
				const char *title = table->GetString(StringID(kEffectBolt, 'BOLT'));
				return (new HeadingSetting(kEffectBolt, title));
			}

			if (i == 1)
			{
				const char *title = table->GetString(StringID(kEffectBolt, 'BOLT', 'DEVI'));
				return (new TextSetting('DEVI', maxPathDeviation, title));
			}

			if (i == 2)
			{
				const char *title = table->GetString(StringID(kEffectBolt, 'BOLT', 'DPTH'));
				return (new IntegerSetting('DPTH', branchingDepth, title, 0, 4, 1));
			}

			if (i == 3)
			{
				const char *title = table->GetString(StringID(kEffectBolt, 'BOLT', 'BRCH'));
				return (new IntegerSetting('BRCH', branchCount, title, 1, 4, 1));
			}

			if (i == 4)
			{
				const char *title = table->GetString(StringID(kEffectBolt, 'BOLT', 'SCAL'));
				return (new TextSetting('SCAL', branchRadiusScale, title));
			}

			if (i == 5)
			{
				const char *title = table->GetString(StringID(kEffectBolt, 'BOLT', 'MINL'));
				return (new TextSetting('MINL', branchLengthRange.min, title));
			}

			if (i == 6)
			{
				const char *title = table->GetString(StringID(kEffectBolt, 'BOLT', 'MAXL'));
				return (new TextSetting('MAXL', branchLengthRange.max, title));
			}
		}
	}

	return (TubeEffectObject::GetCategorySetting(category, index, flags));
}

void BoltEffectObject::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kEffectTube)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'DEVI')
		{
			maxPathDeviation = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
		}
		else if (identifier == 'DPTH')
		{
			branchingDepth = static_cast<const IntegerSetting *>(setting)->GetIntegerValue();
		}
		else if (identifier == 'BRCH')
		{
			branchCount = static_cast<const IntegerSetting *>(setting)->GetIntegerValue();
		}
		else if (identifier == 'SCAL')
		{
			branchRadiusScale = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
		}
		else if (identifier == 'MINL')
		{
			branchLengthRange.min = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
		}
		else if (identifier == 'MAXL')
		{
			branchLengthRange.max = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
		}
		else
		{
			TubeEffectObject::SetCategorySetting(kEffectTube, setting);
		}
	}
}

void BoltEffectObject::BuildBranch(const Path *path, float radius, float deviation, int32 zdiv, TubeVertex *vertex)
{
	float dz = 1.0F / (float) zdiv;
	const ConstVector2D *trig = Math::GetTrigTable();

	const PathComponent *component = GetTubePath()->GetFirstPathComponent();
	for (;;)
	{
		Point3D p1 = component->GetBeginPosition();
		Vector3D tang = Normalize(component->GetBeginTangent());

		vertex[0].position = p1;
		vertex[1].position = p1;
		vertex[0].tangent.Set(tang, -radius);
		vertex[1].tangent.Set(tang, radius);
		vertex[0].texcoord.Set(0.0F, 0.0F);
		vertex[1].texcoord.Set(1.0F, 0.0F);

		vertex += 2;

		for (machine j = 1; j < zdiv; j++)
		{
			float t = (float) j * dz;
			Point3D p2 = component->GetPosition(t);
			tang = Normalize(component->GetTangent(t));
			Vector3D v1 = Math::CreateUnitPerpendicular(tang);
			Vector3D v2 = tang % v1;

			float m = deviation * t * (1.0F - t) * Math::RandomFloat(1.0F);
			const Vector2D& cs = trig[Math::Random(256)];
			p2 += v1 * (cs.x * m) + v2 * (cs.y * m);
			p1 = p2;

			vertex[0].position = p2;
			vertex[1].position = p2;
			vertex[0].tangent.Set(tang, -radius);
			vertex[1].tangent.Set(tang, radius);
			vertex[0].texcoord.Set(0.0F, 0.0F);
			vertex[1].texcoord.Set(1.0F, 0.0F);

			vertex += 2;
		}

		const PathComponent *nextComponent = component->Next();
		if (!nextComponent)
		{
			const Point3D& endPosition = component->GetEndPosition();
			Vector3D endTangent = Normalize(component->GetEndTangent());

			vertex[0].position = endPosition;
			vertex[1].position = endPosition;
			vertex[0].tangent.Set(endTangent, -radius);
			vertex[1].tangent.Set(endTangent, radius);
			vertex[0].texcoord.Set(0.0F, 0.0F);
			vertex[1].texcoord.Set(1.0F, 0.0F);

			break;
		}

		component = nextComponent;
	}
}

void BoltEffectObject::Build(void)
{
	int32 zdiv = Max(GetMaxSubdiv(), 1);
	int32 vertexCount = 2 + GetTubePath()->GetPathComponentCount() * zdiv * 2;

	AllocateStorage(vertexCount);
	TubeVertex *vertex = GetVertexArray();

	BuildBranch(GetTubePath(), GetTubeRadius(), maxPathDeviation * 4.0F, zdiv, vertex);
	GetVertexBuffer()->UpdateBuffer(0, sizeof(TubeVertex) * vertexCount, vertex);
}


BoltEffect::BoltEffect() : TubeEffect(kEffectBolt)
{
}

BoltEffect::BoltEffect(const Path *path, float radius, float deviation, const ColorRGBA& color, const char *textureName) : TubeEffect(kEffectBolt, path, radius, color, textureName)
{
	SetNewObject(new BoltEffectObject(path, radius, deviation, color, textureName));
}

BoltEffect::BoltEffect(const BoltEffect& boltEffect) : TubeEffect(boltEffect)
{
}

BoltEffect::~BoltEffect()
{
}

Node *BoltEffect::Replicate(void) const
{
	return (new BoltEffect(*this));
}


FireEffectObject::FireEffectObject() : EffectObject(kEffectFire)
{
}

FireEffectObject::FireEffectObject(float radius, float height, float intensity, int32 speed, const char *textureName) : EffectObject(kEffectFire)
{
	fireRadius = radius;
	fireHeight = height;

	fireIntensity = intensity;
	fireSpeed = speed;

	fireTextureName = textureName;
}

FireEffectObject::~FireEffectObject()
{
}

void FireEffectObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	EffectObject::Pack(data, packFlags);

	data << ChunkHeader('DATA', 8);
	data << fireRadius;
	data << fireHeight;

	data << ChunkHeader('PARM', 8);
	data << fireIntensity;
	data << fireSpeed;

	PackHandle handle = data.BeginChunk('TXTR');
	data << fireTextureName;
	data.EndChunk(handle);

	data << TerminatorChunk;
}

void FireEffectObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	EffectObject::Unpack(data, unpackFlags);
	UnpackChunkList<FireEffectObject>(data, unpackFlags);
}

bool FireEffectObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'DATA':

			data >> fireRadius;
			data >> fireHeight;
			return (true);

		case 'PARM':

			data >> fireIntensity;
			data >> fireSpeed;
			return (true);

		case 'TXTR':

			data >> fireTextureName;
			return (true);
	}

	return (false);
}

int32 FireEffectObject::GetCategoryCount(void) const
{
	return (1);
}

Type FireEffectObject::GetCategoryType(int32 index, const char **title) const
{
	if (index == 0)
	{
		*title = TheInterfaceMgr->GetStringTable()->GetString(StringID(kEffectFire));
		return (kEffectFire);
	}

	return (0);
}

int32 FireEffectObject::GetCategorySettingCount(Type category) const
{
	if (category == kEffectFire)
	{
		return (4);
	}

	return (0);
}

Setting *FireEffectObject::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kEffectFire)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();

		if (index == 0)
		{
			const char *title = table->GetString(StringID(kEffectFire, 'FIRE'));
			return (new HeadingSetting(kEffectFire, title));
		}

		if (index == 1)
		{
			if (flags & kConfigurationScript)
			{
				return (nullptr);
			}

			const char *title = table->GetString(StringID(kEffectFire, 'FIRE', 'TNAM'));
			const char *picker = table->GetString(StringID(kEffectFire, 'FIRE', 'PICK'));
			return (new ResourceSetting('TNAM', fireTextureName, title, picker, TextureResource::GetDescriptor()));
		}

		if (index == 2)
		{
			const char *title = table->GetString(StringID(kEffectFire, 'FIRE', 'INTS'));
			return (new IntegerSetting('INTS', Max((int32) (fireIntensity * 100.0F + 0.5F), 1), title, 1, 60, 1));
		}

		if (index == 3)
		{
			const char *title = table->GetString(StringID(kEffectFire, 'FIRE', 'SPED'));
			return (new IntegerSetting('SPED', fireSpeed + 1, title, 1, 25, 1));
		}
	}

	return (nullptr);
}

void FireEffectObject::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kEffectFire)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'TNAM')
		{
			fireTextureName = static_cast<const ResourceSetting *>(setting)->GetResourceName();
		}
		else if (identifier == 'INTS')
		{
			fireIntensity = (float) static_cast<const IntegerSetting *>(setting)->GetIntegerValue() * 0.01F;
		}
		else if (identifier == 'SPED')
		{
			fireSpeed = static_cast<const IntegerSetting *>(setting)->GetIntegerValue() - 1;
		}
	}
}

int32 FireEffectObject::GetObjectSize(float *size) const
{
	size[0] = fireRadius;
	size[1] = fireHeight;
	return (2);
}

void FireEffectObject::SetObjectSize(const float *size)
{
	fireRadius = size[0];
	fireHeight = size[1];
}


FireEffect::FireEffect() :
		Effect(kEffectFire, kRenderTriangleStrip, kRenderDepthTest | kRenderDepthInhibit),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
{
}

FireEffect::FireEffect(float radius, float height, float intensity, int32 speed, const char *textureName) :
		Effect(kEffectFire, kRenderTriangleStrip, kRenderDepthTest | kRenderDepthInhibit),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
{
	SetNewObject(new FireEffectObject(radius, height, intensity, speed, textureName));
}

FireEffect::FireEffect(const FireEffect& fireEffect) :
		Effect(fireEffect),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
{
}

FireEffect::~FireEffect()
{
}

Node *FireEffect::Replicate(void) const
{
	return (new FireEffect(*this));
}

bool FireEffect::CalculateBoundingBox(Box3D *box) const
{
	const FireEffectObject *object = GetObject();
	float r = object->GetFireRadius();
	float h = object->GetFireHeight();

	box->min.Set(-r, -r, 0.0F);
	box->max.Set(r, r, h);
	return (true);
}

bool FireEffect::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	const FireEffectObject *object = GetObject();
	float r = object->GetFireRadius();
	float h = object->GetFireHeight() * 0.5F;

	sphere->SetCenter(0.0F, 0.0F, h);
	sphere->SetRadius(Sqrt(h * h + r * r));
	return (true);
}

void FireEffect::Preload(void)
{
	textureAttribute.SetTexture(GetObject()->GetFireTextureName());
	Effect::Preload();
}

void FireEffect::Preprocess(void)
{
	Effect::Preprocess();

	SetTransformable(this);
	SetTransparentPosition(&GetWorldPosition());
	SetShaderFlags(kShaderAmbientEffect);
	SetAmbientBlendState(kBlendAccumulate);

	float dx = Math::RandomFloat(1.0F);
	float dy = Math::RandomFloat(1.0F);

	fireVertex[0].texcoord.Set(0.0F, 1.0F, dx, dy);
	fireVertex[1].texcoord.Set(0.0F, 0.0F, dx, dy);
	fireVertex[2].texcoord.Set(1.0F, 1.0F, dx, dy);
	fireVertex[3].texcoord.Set(1.0F, 0.0F, dx, dy);

	SetVertexCount(4);
	SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(FireVertex));
	SetVertexAttributeArray(kArrayPosition, 0, 3);
	SetVertexAttributeArray(kArrayTexcoord, sizeof(Point3D), 4);
	vertexBuffer.Establish(sizeof(FireVertex) * 4);

	attributeList.Append(&textureAttribute);
	attributeList.Append(&fireAttribute);
	SetMaterialAttributeList(&attributeList);

	FireEffect::ProcessObjectSettings();
}

void FireEffect::ProcessObjectSettings(void)
{
	const FireEffectObject *object = GetObject();

	fireAttribute.SetFireIntensity(object->GetFireIntensity());
	fireAttribute.SetFireSpeed(object->GetFireSpeed());

	textureAttribute.SetTexture(object->GetFireTextureName());
}

void FireEffect::Render(const FrustumCamera *camera, List<Renderable> *effectList)
{
	Point3D cameraPosition = GetInverseWorldTransform() * camera->GetWorldPosition();

	float x = -cameraPosition.y;
	float y = cameraPosition.x;
	float r = x * x + y * y;
	if (r > K::min_float)
	{
		const FireEffectObject *object = GetObject();

		r = InverseSqrt(r) * object->GetFireRadius();
		x *= r;
		y *= r;

		float h = object->GetFireHeight();

		fireVertex[0].position.Set(-x, -y, h);
		fireVertex[1].position.Set(-x, -y, 0.0F);
		fireVertex[2].position.Set(x, y, h);
		fireVertex[3].position.Set(x, y, 0.0F);

		vertexBuffer.UpdateBuffer(0, sizeof(FireVertex) * 4, fireVertex);

		effectList[kEffectListTransparent].Append(this);
	}
}


ShockwaveEffect::ShockwaveEffect() :
		Effect(kEffectShockwave, kRenderTriangleStrip, kRenderDepthTest | kRenderDepthInhibit | kRenderDepthOffset),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
{
}

ShockwaveEffect::ShockwaveEffect(const char *textureName, float radius, float width, float speed) :
		Effect(kEffectShockwave, kRenderTriangleStrip, kRenderDepthTest | kRenderDepthInhibit | kRenderDepthOffset),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		textureAttribute(textureName)
{
	width *= 0.5F;
	shockRadius = width;
	maxShockRadius = radius;
	shockWidth = width;
	shockSpeed = speed;
}

ShockwaveEffect::ShockwaveEffect(const ShockwaveEffect& shockwaveEffect) :
		Effect(shockwaveEffect),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		textureAttribute(shockwaveEffect.textureAttribute)
{
	shockRadius = shockwaveEffect.shockWidth;
	maxShockRadius = shockwaveEffect.maxShockRadius;
	shockWidth = shockwaveEffect.shockWidth;
	shockSpeed = shockwaveEffect.shockSpeed;
}

ShockwaveEffect::~ShockwaveEffect()
{
}

Node *ShockwaveEffect::Replicate(void) const
{
	return (new ShockwaveEffect(*this));
}

void ShockwaveEffect::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Effect::Pack(data, packFlags);

	data << ChunkHeader('RADI', 8);
	data << shockRadius;
	data << maxShockRadius;

	data << ChunkHeader('WIDE', 4);
	data << shockWidth;

	data << ChunkHeader('SPED', 4);
	data << shockSpeed;

	PackHandle handle = data.BeginChunk('TNAM');
	data << textureAttribute.GetTextureName();
	data.EndChunk(handle);

	data << TerminatorChunk;
}

void ShockwaveEffect::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Effect::Unpack(data, unpackFlags);
	UnpackChunkList<ShockwaveEffect>(data, unpackFlags);
}

bool ShockwaveEffect::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'RADI':

			data >> shockRadius;
			data >> maxShockRadius;
			return (true);

		case 'WIDE':

			data >> shockWidth;
			return (true);

		case 'SPED':

			data >> shockSpeed;
			return (true);

		case 'TNAM':
		{
			ResourceName	textureName;

			data >> textureName;
			textureAttribute.SetTexture(textureName);
			return (true);
		}
	}

	return (false);
}

bool ShockwaveEffect::CalculateBoundingBox(Box3D *box) const
{
	float r = maxShockRadius + shockWidth;
	box->min.Set(-r, -r, -r);
	box->max.Set(r, r, r);
	return (true);
}

bool ShockwaveEffect::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	sphere->SetCenter(Point3D(0.0F, 0.0F, 0.0F));
	sphere->SetRadius(maxShockRadius + shockWidth);
	return (true);
}

void ShockwaveEffect::Preprocess(void)
{
	Effect::Preprocess();
	Wake();

	SetTransformable(this);
	SetDistortionState();
	SetDepthOffset(4.0F, &GetWorldPosition());

	attributeList.Append(&textureAttribute);
	SetMaterialAttributeList(&attributeList);
	SetShaderFlags(GetShaderFlags() | kShaderVertexBillboard);

	SetVertexCount(130);
	SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(ShockwaveVertex));
	SetVertexAttributeArray(kArrayPosition, 0, 2);
	SetVertexAttributeArray(kArrayTexcoord, sizeof(Point2D), 2);
	vertexBuffer.Establish(sizeof(ShockwaveVertex) * 130);
}

void ShockwaveEffect::Move(void)
{
	shockRadius += TheTimeMgr->GetFloatDeltaTime() * shockSpeed;
	if (shockRadius > maxShockRadius)
	{
		delete this;
	}
}

void ShockwaveEffect::Render(const FrustumCamera *camera, List<Renderable> *effectList)
{
	static const ConstPoint2D shockwaveTexcoord[130] =
	{
		{1.0F, 0.5F}, {0.5F, 0.5F}, {0.9975923F, 0.5490084F}, {0.5F, 0.5F}, {0.9903925F, 0.5975451F}, {0.5F, 0.5F}, {0.9784702F, 0.6451423F}, {0.5F, 0.5F},
		{0.9619396F, 0.6913417F}, {0.5F, 0.5F}, {0.9409606F, 0.7356983F}, {0.5F, 0.5F}, {0.9157347F, 0.777785F}, {0.5F, 0.5F}, {0.8865052F, 0.8171966F}, {0.5F, 0.5F},
		{0.8535534F, 0.8535534F}, {0.5F, 0.5F}, {0.8171966F, 0.8865052F}, {0.5F, 0.5F}, {0.777785F, 0.9157347F}, {0.5F, 0.5F}, {0.7356983F, 0.9409606F}, {0.5F, 0.5F},
		{0.6913416F, 0.9619396F}, {0.5F, 0.5F}, {0.6451423F, 0.9784702F}, {0.5F, 0.5F}, {0.5975451F, 0.9903926F}, {0.5F, 0.5F}, {0.5490084F, 0.9975923F}, {0.5F, 0.5F},
		{0.5F, 1.0F}, {0.5F, 0.5F}, {0.4509913F, 0.9975923F}, {0.5F, 0.5F}, {0.4024548F, 0.9903926F}, {0.5F, 0.5F}, {0.3548576F, 0.9784702F}, {0.5F, 0.5F},
		{0.3086582F, 0.9619396F}, {0.5F, 0.5F}, {0.2643016F, 0.9409606F}, {0.5F, 0.5F}, {0.2222148F, 0.9157347F}, {0.5F, 0.5F}, {0.1828032F, 0.8865052F}, {0.5F, 0.5F},
		{0.1464465F, 0.8535534F}, {0.5F, 0.5F}, {0.1134947F, 0.8171966F}, {0.5F, 0.5F}, {0.0842651F, 0.777785F}, {0.5F, 0.5F}, {0.0590393F, 0.7356983F}, {0.5F, 0.5F},
		{0.0380601F, 0.6913417F}, {0.5F, 0.5F}, {0.0215297F, 0.6451423F}, {0.5F, 0.5F}, {0.0096073F, 0.5975451F}, {0.5F, 0.5F}, {0.0024075F, 0.5490084F}, {0.5F, 0.5F},
		{0.0F, 0.5F}, {0.5F, 0.5F}, {0.0024075F, 0.4509913F}, {0.5F, 0.5F}, {0.0096073F, 0.4024548F}, {0.5F, 0.5F}, {0.0215297F, 0.3548576F}, {0.5F, 0.5F},
		{0.0380601F, 0.3086582F}, {0.5F, 0.5F}, {0.0590393F, 0.2643015F}, {0.5F, 0.5F}, {0.0842651F, 0.2222148F}, {0.5F, 0.5F}, {0.1134947F, 0.1828032F}, {0.5F, 0.5F},
		{0.1464465F, 0.1464465F}, {0.5F, 0.5F}, {0.1828032F, 0.1134947F}, {0.5F, 0.5F}, {0.2222148F, 0.0842651F}, {0.5F, 0.5F}, {0.2643016F, 0.0590393F}, {0.5F, 0.5F},
		{0.3086582F, 0.0380601F}, {0.5F, 0.5F}, {0.3548576F, 0.0215297F}, {0.5F, 0.5F}, {0.4024548F, 0.0096073F}, {0.5F, 0.5F}, {0.4509913F, 0.0024075F}, {0.5F, 0.5F},
		{0.5F, 0.0F}, {0.5F, 0.5F}, {0.5490084F, 0.0024075F}, {0.5F, 0.5F}, {0.5975451F, 0.0096073F}, {0.5F, 0.5F}, {0.6451423F, 0.0215297F}, {0.5F, 0.5F},
		{0.6913416F, 0.0380601F}, {0.5F, 0.5F}, {0.7356983F, 0.0590393F}, {0.5F, 0.5F}, {0.777785F, 0.0842651F}, {0.5F, 0.5F}, {0.8171966F, 0.1134947F}, {0.5F, 0.5F},
		{0.8535534F, 0.1464465F}, {0.5F, 0.5F}, {0.8865052F, 0.1828032F}, {0.5F, 0.5F}, {0.9157347F, 0.2222148F}, {0.5F, 0.5F}, {0.9409606F, 0.2643015F}, {0.5F, 0.5F},
		{0.9619396F, 0.3086582F}, {0.5F, 0.5F}, {0.9784702F, 0.3548576F}, {0.5F, 0.5F}, {0.9903925F, 0.4024548F}, {0.5F, 0.5F}, {0.9975923F, 0.4509913F}, {0.5F, 0.5F},
		{1.0F, 0.5F}, {0.5F, 0.5F}
	};

	volatile ShockwaveVertex *restrict shockwaveVertex = vertexBuffer.BeginUpdate<ShockwaveVertex>();

	const ConstPoint2D *texcoord = shockwaveTexcoord;
	const ConstVector2D *trig = Math::GetTrigTable();

	float radius = shockRadius;
	float r1 = radius - shockWidth;
	float r2 = radius + shockWidth;

	for (machine a = 0; a < 64; a++)
	{
		const Vector2D& cs = trig[a * 4];

		shockwaveVertex[0].position = cs * r2;
		shockwaveVertex[0].texcoord = texcoord[0];
		shockwaveVertex[1].position = cs * r1;
		shockwaveVertex[1].texcoord = texcoord[1];

		shockwaveVertex += 2;
		texcoord += 2;
	}

	shockwaveVertex[0].position.Set(r2, 0.0F);
	shockwaveVertex[0].texcoord = texcoord[0];
	shockwaveVertex[1].position.Set(r1, 0.0F);
	shockwaveVertex[1].texcoord = texcoord[1];

	vertexBuffer.EndUpdate();

	effectList[GetEffectListIndex()].Append(this);
}

// ZYUQURM
