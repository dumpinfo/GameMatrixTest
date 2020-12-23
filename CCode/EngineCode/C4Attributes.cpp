 

#include "C4Attributes.h"
#include "C4Shaders.h"
#include "C4Random.h"


using namespace C4;


const TextureHeader MicrofacetAttribute::isotropicMicrofacetTextureHeader =
{
	kTexture2D,
	0,
	kTextureSemanticNone,
	kTextureSemanticNone,
	kTextureRGBA8,
	kMicrofacetTextureSize, kMicrofacetTextureSize, 1,
	{kTextureClamp, kTextureClamp, kTextureClamp},
	1
};

const TextureHeader MicrofacetAttribute::anisotropicMicrofacetTextureHeader =
{
	kTexture3D,
	0,
	kTextureSemanticNone,
	kTextureSemanticNone,
	kTextureRGBA8,
	kMicrofacetTextureSize, kMicrofacetTextureSize, kMicrofacetTextureSize,
	{kTextureClamp, kTextureClamp, kTextureClamp},
	1
};

Storage<HashTable<MicrofacetAttribute::IsotropicMicrofacetTexture>> MicrofacetAttribute::isotropicHashTable;
Storage<HashTable<MicrofacetAttribute::AnisotropicMicrofacetTexture>> MicrofacetAttribute::anisotropicHashTable;


Attribute::Attribute(AttributeType type, unsigned_int32 flags)
{
	attributeType = type;
	attributeFlags = flags;
}

Attribute::Attribute(const Attribute& attribute)
{
	attributeType = attribute.attributeType;
	attributeFlags = attribute.attributeFlags;
}

Attribute::~Attribute()
{
}

Attribute *Attribute::Create(Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (data.GetType())
	{
		case kAttributeDiffuse:

			return (new DiffuseAttribute);

		case kAttributeSpecular:

			return (new SpecularAttribute);

		case kAttributeEmission:

			return (new EmissionAttribute);

		case kAttributeEnvironment:

			return (new EnvironmentAttribute);

		case kAttributeReflection:

			return (new ReflectionAttribute);

		case kAttributeRefraction:

			return (new RefractionAttribute);

		case kAttributeMicrofacet:

			return (new MicrofacetAttribute);

		case kAttributeDiffuseTexture:

		#if C4LEGACY

			case 'TEXT':

		#endif

			return (new DiffuseTextureAttribute);

		case kAttributeNormalTexture:

		#if C4LEGACY

			case 'BUMP':

		#endif 

			return (new NormalTextureAttribute);
 
		case kAttributeHorizonTexture:
 
			return (new HorizonTextureAttribute);

		case kAttributeSpecularTexture: 

		#if C4LEGACY 
 
			case 'GLOS':

		#endif
 
			return (new SpecularTextureAttribute);

		case kAttributeEmissionTexture:

		#if C4LEGACY

			case 'EMAP':

		#endif

			return (new EmissionTextureAttribute);

		case kAttributeOpacityTexture:

			return (new OpacityTextureAttribute);

		case kAttributeEnvironmentTexture:

		#if C4LEGACY

			case 'ENVM':

		#endif

			return (new EnvironmentTextureAttribute);

		case kAttributeFireTexture:

			return (new FireTextureAttribute);

		case kAttributeDeltaDepth:

			return (new DeltaDepthAttribute);

		case kAttributeDepthRamp:

			return (new DepthRampAttribute);

		case kAttributeShader:

			return (new ShaderAttribute);
	}

	return (nullptr);
}

void Attribute::PackType(Packer& data) const
{
	data << attributeType;
}

void Attribute::Pack(Packer& data, unsigned_int32 packFlags) const
{
	data << ChunkHeader('FLAG', 4);
	data << attributeFlags;

	data << TerminatorChunk;
}

void Attribute::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	UnpackChunkList<Attribute>(data, unpackFlags);
}

bool Attribute::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> attributeFlags;
			return (true);
	}

	return (false);
}

bool Attribute::operator ==(const Attribute& attribute) const
{
	return (false);
}

void Attribute::SetAttributeColor(const ColorRGBA& color)
{
}


ReferenceAttribute::ReferenceAttribute() : Attribute(kAttributeReference)
{
	attributeReference = nullptr;
}

ReferenceAttribute::ReferenceAttribute(const Attribute *attribute) : Attribute(kAttributeReference)
{
	attributeReference = attribute;
}

ReferenceAttribute::ReferenceAttribute(const ReferenceAttribute& referenceAttribute) : Attribute(referenceAttribute)
{
	attributeReference = referenceAttribute.attributeReference;
}

ReferenceAttribute::~ReferenceAttribute()
{
}

Attribute *ReferenceAttribute::Replicate(void) const
{
	return (new ReferenceAttribute(*this));
}

bool ReferenceAttribute::operator ==(const Attribute& attribute) const
{
	if (attribute.GetAttributeType() == kAttributeReference)
	{
		const ReferenceAttribute *referenceAttribute = static_cast<const ReferenceAttribute *>(&attribute);

		if (referenceAttribute->attributeReference == attributeReference)
		{
			return (true);
		}
	}

	return (false);
}


DiffuseAttribute::DiffuseAttribute(unsigned_int32 flags) : Attribute(kAttributeDiffuse, flags)
{
}

DiffuseAttribute::DiffuseAttribute(const ColorRGBA& color, unsigned_int32 flags) : Attribute(kAttributeDiffuse, flags)
{
	diffuseColor = color;
}

DiffuseAttribute::DiffuseAttribute(const DiffuseAttribute& diffuseAttribute) : Attribute(diffuseAttribute)
{
	diffuseColor = diffuseAttribute.diffuseColor;
}

DiffuseAttribute::~DiffuseAttribute()
{
}

Attribute *DiffuseAttribute::Replicate(void) const
{
	return (new DiffuseAttribute(*this));
}

void DiffuseAttribute::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Attribute::Pack(data, packFlags);

	data << ChunkHeader('COLR', sizeof(ColorRGBA));
	data << diffuseColor;

	data << TerminatorChunk;
}

void DiffuseAttribute::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Attribute::Unpack(data, unpackFlags);
	UnpackChunkList<DiffuseAttribute>(data, unpackFlags);
}

bool DiffuseAttribute::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'COLR':

			data >> diffuseColor;
			return (true);
	}

	return (false);
}

bool DiffuseAttribute::operator ==(const Attribute& attribute) const
{
	if (attribute.GetAttributeType() == kAttributeDiffuse)
	{
		const DiffuseAttribute *diffuseAttribute = static_cast<const DiffuseAttribute *>(&attribute);

		if (diffuseAttribute->diffuseColor == diffuseColor)
		{
			return (true);
		}
	}

	return (false);
}

void DiffuseAttribute::SetAttributeColor(const ColorRGBA& color)
{
	diffuseColor = color;
}


SpecularAttribute::SpecularAttribute(unsigned_int32 flags) : Attribute(kAttributeSpecular, flags)
{
}

SpecularAttribute::SpecularAttribute(const ColorRGBA& color, float exponent, unsigned_int32 flags) : Attribute(kAttributeSpecular, flags)
{
	specularColor = color;
	specularExponent = exponent;
}

SpecularAttribute::SpecularAttribute(const SpecularAttribute& specularAttribute) : Attribute(specularAttribute)
{
	specularColor = specularAttribute.specularColor;
	specularExponent = specularAttribute.specularExponent;
}

SpecularAttribute::~SpecularAttribute()
{
}

Attribute *SpecularAttribute::Replicate(void) const
{
	return (new SpecularAttribute(*this));
}

void SpecularAttribute::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Attribute::Pack(data, packFlags);

	data << ChunkHeader('DATA', sizeof(ColorRGBA) + 4);
	data << specularColor;
	data << specularExponent;

	data << TerminatorChunk;
}

void SpecularAttribute::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Attribute::Unpack(data, unpackFlags);
	UnpackChunkList<SpecularAttribute>(data, unpackFlags);
}

bool SpecularAttribute::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'DATA':

			data >> specularColor;
			data >> specularExponent;
			return (true);
	}

	return (false);
}

bool SpecularAttribute::operator ==(const Attribute& attribute) const
{
	if (attribute.GetAttributeType() == kAttributeSpecular)
	{
		const SpecularAttribute *specularAttribute = static_cast<const SpecularAttribute *>(&attribute);

		if (specularAttribute->specularColor != specularColor)
		{
			return (false);
		}

		if (specularAttribute->specularExponent == specularExponent)
		{
			return (true);
		}
	}

	return (false);
}

void SpecularAttribute::SetAttributeColor(const ColorRGBA& color)
{
	specularColor = color;
}


EmissionAttribute::EmissionAttribute(unsigned_int32 flags) : Attribute(kAttributeEmission, flags)
{
}

EmissionAttribute::EmissionAttribute(const ColorRGBA& color, unsigned_int32 flags) : Attribute(kAttributeEmission, flags)
{
	emissionColor = color;
}

EmissionAttribute::EmissionAttribute(const EmissionAttribute& emissionAttribute) : Attribute(emissionAttribute)
{
	emissionColor = emissionAttribute.emissionColor;
}

EmissionAttribute::~EmissionAttribute()
{
}

Attribute *EmissionAttribute::Replicate(void) const
{
	return (new EmissionAttribute(*this));
}

void EmissionAttribute::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Attribute::Pack(data, packFlags);

	data << ChunkHeader('COLR', sizeof(ColorRGBA));
	data << emissionColor;

	data << TerminatorChunk;
}

void EmissionAttribute::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Attribute::Unpack(data, unpackFlags);
	UnpackChunkList<EmissionAttribute>(data, unpackFlags);
}

bool EmissionAttribute::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'COLR':

			data >> emissionColor;
			return (true);
	}

	return (false);
}

bool EmissionAttribute::operator ==(const Attribute& attribute) const
{
	if (attribute.GetAttributeType() == kAttributeEmission)
	{
		const EmissionAttribute *emissionAttribute = static_cast<const EmissionAttribute *>(&attribute);

		if (emissionAttribute->emissionColor == emissionColor)
		{
			return (true);
		}
	}

	return (false);
}

void EmissionAttribute::SetAttributeColor(const ColorRGBA& color)
{
	emissionColor = color;
}


EnvironmentAttribute::EnvironmentAttribute(unsigned_int32 flags) : Attribute(kAttributeEnvironment, flags)
{
}

EnvironmentAttribute::EnvironmentAttribute(const ColorRGBA& color, unsigned_int32 flags) : Attribute(kAttributeEnvironment, flags)
{
	environmentColor = color;
}

EnvironmentAttribute::EnvironmentAttribute(const EnvironmentAttribute& environmentAttribute) : Attribute(environmentAttribute)
{
	environmentColor = environmentAttribute.environmentColor;
}

EnvironmentAttribute::~EnvironmentAttribute()
{
}

Attribute *EnvironmentAttribute::Replicate(void) const
{
	return (new EnvironmentAttribute(*this));
}

void EnvironmentAttribute::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Attribute::Pack(data, packFlags);

	data << ChunkHeader('COLR', sizeof(ColorRGBA));
	data << environmentColor;

	data << TerminatorChunk;
}

void EnvironmentAttribute::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Attribute::Unpack(data, unpackFlags);
	UnpackChunkList<EnvironmentAttribute>(data, unpackFlags);
}

bool EnvironmentAttribute::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'COLR':

			data >> environmentColor;
			return (true);
	}

	return (false);
}

bool EnvironmentAttribute::operator ==(const Attribute& attribute) const
{
	if (attribute.GetAttributeType() == kAttributeEnvironment)
	{
		const EnvironmentAttribute *environmentAttribute = static_cast<const EnvironmentAttribute *>(&attribute);

		if (environmentAttribute->environmentColor == environmentColor)
		{
			return (true);
		}
	}

	return (false);
}

void EnvironmentAttribute::SetAttributeColor(const ColorRGBA& color)
{
	environmentColor = color;
}


ReflectionAttribute::ReflectionAttribute(unsigned_int32 flags) : Attribute(kAttributeReflection, flags)
{
}

ReflectionAttribute::ReflectionAttribute(const ColorRGBA& color, float factor, float scale, unsigned_int32 flags) : Attribute(kAttributeReflection, flags)
{
	reflectionColor = color;
	reflectionParams.normalIncidenceReflectivity = factor;
	reflectionParams.reflectionOffsetScale = scale;
}

ReflectionAttribute::ReflectionAttribute(const ReflectionAttribute& reflectionAttribute) : Attribute(reflectionAttribute)
{
	reflectionColor = reflectionAttribute.reflectionColor;
	reflectionParams.normalIncidenceReflectivity = reflectionAttribute.reflectionParams.normalIncidenceReflectivity;
	reflectionParams.reflectionOffsetScale = reflectionAttribute.reflectionParams.reflectionOffsetScale;
}

ReflectionAttribute::~ReflectionAttribute()
{
}

Attribute *ReflectionAttribute::Replicate(void) const
{
	return (new ReflectionAttribute(*this));
}

void ReflectionAttribute::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Attribute::Pack(data, packFlags);

	data << ChunkHeader('DATA', sizeof(ColorRGBA) + 8);
	data << reflectionColor;
	data << reflectionParams.normalIncidenceReflectivity;
	data << reflectionParams.reflectionOffsetScale;

	data << TerminatorChunk;
}

void ReflectionAttribute::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Attribute::Unpack(data, unpackFlags);
	UnpackChunkList<ReflectionAttribute>(data, unpackFlags);
}

bool ReflectionAttribute::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'DATA':

			data >> reflectionColor;
			data >> reflectionParams.normalIncidenceReflectivity;
			data >> reflectionParams.reflectionOffsetScale;
			return (true);
	}

	return (false);
}

bool ReflectionAttribute::operator ==(const Attribute& attribute) const
{
	if (attribute.GetAttributeType() == kAttributeReflection)
	{
		const ReflectionAttribute *reflectionAttribute = static_cast<const ReflectionAttribute *>(&attribute);

		if (reflectionAttribute->reflectionColor != reflectionColor)
		{
			return (false);
		}

		if (reflectionAttribute->reflectionParams.normalIncidenceReflectivity != reflectionParams.normalIncidenceReflectivity)
		{
			return (false);
		}

		if (reflectionAttribute->reflectionParams.reflectionOffsetScale == reflectionParams.reflectionOffsetScale)
		{
			return (true);
		}
	}

	return (false);
}

void ReflectionAttribute::SetAttributeColor(const ColorRGBA& color)
{
	reflectionColor = color;
}


RefractionAttribute::RefractionAttribute(unsigned_int32 flags) : Attribute(kAttributeRefraction, flags)
{
}

RefractionAttribute::RefractionAttribute(const ColorRGBA& color, float scale, unsigned_int32 flags) : Attribute(kAttributeRefraction, flags)
{
	refractionColor = color;
	refractionParams.refractionOffsetScale = scale;
}

RefractionAttribute::RefractionAttribute(const RefractionAttribute& refractionAttribute) : Attribute(refractionAttribute)
{
	refractionColor = refractionAttribute.refractionColor;
	refractionParams.refractionOffsetScale = refractionAttribute.refractionParams.refractionOffsetScale;
}

RefractionAttribute::~RefractionAttribute()
{
}

Attribute *RefractionAttribute::Replicate(void) const
{
	return (new RefractionAttribute(*this));
}

void RefractionAttribute::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Attribute::Pack(data, packFlags);

	data << ChunkHeader('DATA', sizeof(ColorRGBA) + 4);
	data << refractionColor;
	data << refractionParams.refractionOffsetScale;

	data << TerminatorChunk;
}

void RefractionAttribute::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Attribute::Unpack(data, unpackFlags);
	UnpackChunkList<RefractionAttribute>(data, unpackFlags);
}

bool RefractionAttribute::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'DATA':

			data >> refractionColor;
			data >> refractionParams.refractionOffsetScale;
			return (true);
	}

	return (false);
}

bool RefractionAttribute::operator ==(const Attribute& attribute) const
{
	if (attribute.GetAttributeType() == kAttributeRefraction)
	{
		const RefractionAttribute *refractionAttribute = static_cast<const RefractionAttribute *>(&attribute);

		if (refractionAttribute->refractionColor != refractionColor)
		{
			return (false);
		}

		if (refractionAttribute->refractionParams.refractionOffsetScale == refractionParams.refractionOffsetScale)
		{
			return (true);
		}
	}

	return (false);
}

void RefractionAttribute::SetAttributeColor(const ColorRGBA& color)
{
	refractionColor = color;
}


MicrofacetAttribute::MicrofacetAttribute(unsigned_int32 flags) : Attribute(kAttributeMicrofacet, flags)
{
}

MicrofacetAttribute::MicrofacetAttribute(const ColorRGBA& color, const Vector2D& slope, float reflectivity, unsigned_int32 flags) : Attribute(kAttributeMicrofacet, flags)
{
	microfacetParams.microfacetColor = color;
	microfacetParams.microfacetSlope = slope;
	microfacetReflectivity = reflectivity;
}

MicrofacetAttribute::MicrofacetAttribute(const MicrofacetAttribute& microfacetAttribute) : Attribute(microfacetAttribute)
{
	microfacetReflectivity = microfacetAttribute.microfacetReflectivity;
	microfacetParams.microfacetColor = microfacetAttribute.microfacetParams.microfacetColor;
	microfacetParams.microfacetSlope = microfacetAttribute.microfacetParams.microfacetSlope;
}

MicrofacetAttribute::~MicrofacetAttribute()
{
	MicrofacetTexture *texture = microfacetParams.microfacetTexture;
	if (texture)
	{
		texture->Release();
	}
}

Attribute *MicrofacetAttribute::Replicate(void) const
{
	return (new MicrofacetAttribute(*this));
}

void MicrofacetAttribute::Initialize(void)
{
	new(isotropicHashTable) HashTable<IsotropicMicrofacetTexture>(8, 16);
	new(anisotropicHashTable) HashTable<AnisotropicMicrofacetTexture>(8, 16);
}

void MicrofacetAttribute::Terminate(void)
{
	anisotropicHashTable->~HashTable();
	isotropicHashTable->~HashTable();
}

void MicrofacetAttribute::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Attribute::Pack(data, packFlags);

	data << ChunkHeader('DATA', sizeof(ColorRGBA) + sizeof(Vector2D) + 4);
	data << microfacetParams.microfacetColor;
	data << microfacetParams.microfacetSlope;
	data << microfacetReflectivity;

	data << TerminatorChunk;
}

void MicrofacetAttribute::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Attribute::Unpack(data, unpackFlags);
	UnpackChunkList<MicrofacetAttribute>(data, unpackFlags);
}

bool MicrofacetAttribute::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'DATA':

			data >> microfacetParams.microfacetColor;
			data >> microfacetParams.microfacetSlope;
			data >> microfacetReflectivity;
			return (true);
	}

	return (false);
}

float MicrofacetAttribute::CalculateThreshold(const MicrofacetParams *params)
{
	const float kMicrofacetEpsilon = 0.01F;

	float m = Fmax(params->microfacetSlope.x, params->microfacetSlope.y);
	float m2 = m * m;

	float x = 1.0F - (m2 * (1.0F - K::two_tau * kMicrofacetEpsilon * m2)) / (2.0F - 4.0F * m2);
	for (machine a = 0; a < 6; a++)
	{
		float x2 = x * x;
		float m2x2 = m2 * x2;
		x -= (m2x2 * x * (1.0F - K::two_tau * kMicrofacetEpsilon * m2x2 * x2 * Exp((1.0F - x2) / m2x2))) / (2.0F - 4.0F * m2x2);
	}

	return (FmaxZero(x));
}

ColorRGB MicrofacetAttribute::CalculateRefractionIndex(const ColorRGB& color)
{
	float red = Sqrt(Fmin(color.red, 0.996F));
	float green = Sqrt(Fmin(color.green, 0.996F));
	float blue = Sqrt(Fmin(color.blue, 0.996F));

	red = (1.0F + red) / (1.0F - red);
	green = (1.0F + green) / (1.0F - green);
	blue = (1.0F + blue) / (1.0F - blue);

	return (ColorRGB(red, green, blue));
}

float MicrofacetAttribute::CalculateFresnelTerm(float L_dot_H, float refractionIndex)
{
	float g = Sqrt(refractionIndex * refractionIndex + L_dot_H * L_dot_H - 1.0F);
	float k1 = g + L_dot_H;
	float k2 = g - L_dot_H;
	float l1 = L_dot_H * k2 + 1.0F;
	float l2 = L_dot_H * k1 - 1.0F;
	return (0.5F * k2 * k2 / (k1 * k1) * (l2 * l2 / (l1 * l1) + 1.0F));
}

bool MicrofacetAttribute::operator ==(const Attribute& attribute) const
{
	if (attribute.GetAttributeType() == kAttributeMicrofacet)
	{
		const MicrofacetAttribute *microfacetAttribute = static_cast<const MicrofacetAttribute *>(&attribute);

		if (microfacetAttribute->microfacetParams.microfacetColor != microfacetParams.microfacetColor)
		{
			return (false);
		}

		if (microfacetAttribute->microfacetParams.microfacetSlope != microfacetParams.microfacetSlope)
		{
			return (false);
		}

		if (microfacetAttribute->microfacetReflectivity == microfacetReflectivity)
		{
			return (true);
		}
	}

	return (false);
}

void MicrofacetAttribute::SetAttributeColor(const ColorRGBA& color)
{
	microfacetParams.microfacetColor = color;
	microfacetParams.Invalidate();
}

const Texture *MicrofacetAttribute::GetTextureObject(const MicrofacetParams *params)
{
	MicrofacetTexture *texture = params->microfacetTexture;
	if (texture)
	{
		return (texture->GetTextureObject());
	}

	params->microfacetThreshold = CalculateThreshold(params);

	if (params->microfacetSlope.x == params->microfacetSlope.y)
	{
		IsotropicMicrofacetTexture::KeyType		key;

		key.color = params->microfacetColor;
		key.slope = params->microfacetSlope.x;
		IsotropicMicrofacetTexture *isotropicTexture = isotropicHashTable->Find(key);
		if (isotropicTexture)
		{
			isotropicTexture->Retain();
		}
		else
		{
			isotropicTexture = new IsotropicMicrofacetTexture(params->microfacetColor, params->microfacetSlope.x, params->microfacetThreshold);
			isotropicHashTable->Insert(isotropicTexture);
		}

		params->microfacetTexture = isotropicTexture;
		return (isotropicTexture->GetTextureObject());
	}

	AnisotropicMicrofacetTexture::KeyType	key;

	key.color = params->microfacetColor;
	key.slope = params->microfacetSlope;
	AnisotropicMicrofacetTexture *anisotropicTexture = anisotropicHashTable->Find(key);
	if (anisotropicTexture)
	{
		anisotropicTexture->Retain();
	}
	else
	{
		anisotropicTexture = new AnisotropicMicrofacetTexture(params->microfacetColor, params->microfacetSlope, params->microfacetThreshold);
		anisotropicHashTable->Insert(anisotropicTexture);
	}

	params->microfacetTexture = anisotropicTexture;
	return (anisotropicTexture->GetTextureObject());
}

MicrofacetAttribute::MicrofacetTexture::MicrofacetTexture(const ColorRGB& color)
{
	microfacetColor = color;
}

MicrofacetAttribute::MicrofacetTexture::~MicrofacetTexture()
{
	texture->Release();
}

MicrofacetAttribute::IsotropicMicrofacetTexture::IsotropicMicrofacetTexture(const ColorRGB& color, float slope, float threshold) : MicrofacetTexture(color)
{
	textureKey.color = color;
	textureKey.slope = slope;

	ColorRGB eta = CalculateRefractionIndex(color);
	float s = 1.0F / (float) (kMicrofacetTextureSize - 1);
	float f = (1.0F - threshold) * s;
	float m2 = slope * slope;

	Color4C *image = textureImage;
	for (machine j = 0; j < kMicrofacetTextureSize; j++)
	{
		image->Clear();
		image++;

		float L_dot_H = (float) j * s;

		float red = CalculateFresnelTerm(L_dot_H, eta.red) * 255.0F;
		float green = CalculateFresnelTerm(L_dot_H, eta.green) * 255.0F;
		float blue = CalculateFresnelTerm(L_dot_H, eta.blue) * 255.0F;

		for (machine i = 1; i < kMicrofacetTextureSize; i++)
		{
			float N_dot_H = (float) i * f + threshold;

			float nh2 = N_dot_H * N_dot_H;
			float D = Exp((nh2 - 1.0F) / (m2 * nh2)) / (K::two_tau * m2 * nh2 * nh2);

			int32 r = Min((int32) (red * D), 255);
			int32 g = Min((int32) (green * D), 255);
			int32 b = Min((int32) (blue * D), 255);

			image->Set(r, g, b, 0);
			image++;
		}
	}

	SetTexture(&isotropicMicrofacetTextureHeader, textureImage);
}

MicrofacetAttribute::IsotropicMicrofacetTexture::~IsotropicMicrofacetTexture()
{
}

MicrofacetAttribute::AnisotropicMicrofacetTexture::AnisotropicMicrofacetTexture(const ColorRGB& color, const Vector2D& slope, float threshold) : MicrofacetTexture(color)
{
	textureKey.color = color;
	textureKey.slope = slope;

	ColorRGB eta = CalculateRefractionIndex(color);
	float s = 1.0F / (float) (kMicrofacetTextureSize - 1);
	float f = (1.0F - threshold) * s;

	float mx = 1.0F / slope.x;
	float my = 1.0F / slope.y;
	float mx2 = mx * mx;
	float my2 = my * my;
	float h = mx * my / K::two_tau;

	Color4C *image = textureImage;
	for (machine k = 0; k < kMicrofacetTextureSize; k++)
	{
		float tp2 = (float) k * s;
		float anisoFactor = tp2 * mx2 + (1.0F - tp2) * my2;

		for (machine j = 0; j < kMicrofacetTextureSize; j++)
		{
			image->Clear();
			image++;

			float L_dot_H = (float) j * s;

			float red = CalculateFresnelTerm(L_dot_H, eta.red) * 255.0F;
			float green = CalculateFresnelTerm(L_dot_H, eta.green) * 255.0F;
			float blue = CalculateFresnelTerm(L_dot_H, eta.blue) * 255.0F;

			for (machine i = 1; i < kMicrofacetTextureSize; i++)
			{
				float N_dot_H = (float) i * f + threshold;

				float nh2 = N_dot_H * N_dot_H;
				float D = Exp(anisoFactor * (nh2 - 1.0F) / nh2) * h / (nh2 * nh2);

				int32 r = Min((int32) (red * D), 255);
				int32 g = Min((int32) (green * D), 255);
				int32 b = Min((int32) (blue * D), 255);

				image->Set(r, g, b, 0);
				image++;
			}
		}
	}

	SetTexture(&anisotropicMicrofacetTextureHeader, textureImage);
}

MicrofacetAttribute::AnisotropicMicrofacetTexture::~AnisotropicMicrofacetTexture()
{
}


TextureAttribute::TextureAttribute(AttributeType type) : Attribute(type)
{
	textureName[0] = 0;
	textureObject = nullptr;
	texcoordIndex = 0;
}

TextureAttribute::TextureAttribute(AttributeType type, const char *name) : Attribute(type)
{
	textureObject = nullptr;
	texcoordIndex = 0;

	SetTexture(name);
}

TextureAttribute::TextureAttribute(AttributeType type, Texture *texture) : Attribute(type)
{
	textureObject = nullptr;
	texcoordIndex = 0;

	SetTexture(texture);
}

TextureAttribute::TextureAttribute(AttributeType type, const TextureHeader *header, const void *image) : Attribute(type)
{
	textureObject = nullptr;
	texcoordIndex = 0;

	SetTexture(header, image);
}

TextureAttribute::TextureAttribute(AttributeType type, const char *name, const TextureHeader *header, const void *image) : Attribute(type)
{
	textureObject = nullptr;
	texcoordIndex = 0;

	if ((name) && (name[0] != 0))
	{
		SetTexture(name);
	}
	else
	{
		SetTexture(header, image);
	}
}

TextureAttribute::TextureAttribute(const TextureAttribute& textureAttribute) : Attribute(textureAttribute)
{
	textureObject = nullptr;
	texcoordIndex = textureAttribute.texcoordIndex;

	SetTexture(textureAttribute.textureObject);
	textureName = textureAttribute.textureName;
}

TextureAttribute::~TextureAttribute()
{
	if (textureObject)
	{
		textureObject->Release();
	}
}

void TextureAttribute::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Attribute::Pack(data, packFlags);

	PackHandle handle = data.BeginChunk('NAME');
	data << textureName;
	data.EndChunk(handle);

	if (texcoordIndex != 0)
	{
		data << ChunkHeader('TEXC', 4);
		data << texcoordIndex;
	}

	data << TerminatorChunk;
}

void TextureAttribute::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Attribute::Unpack(data, unpackFlags);
	UnpackChunkList<TextureAttribute>(data, unpackFlags);
}

bool TextureAttribute::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'NAME':

			data >> textureName;
			SetTexture(textureName);
			return (true);

		case 'TEXC':

			data >> texcoordIndex;
			return (true);
	}

	return (false);
}

void *TextureAttribute::BeginSettingsUnpack(void)
{
	texcoordIndex = 0;

	if (textureObject)
	{
		textureObject->Release();
		textureObject = nullptr;
	}

	return (Attribute::BeginSettingsUnpack());
}

bool TextureAttribute::operator ==(const Attribute& attribute) const
{
	if (attribute.GetAttributeType() == GetAttributeType())
	{
		const TextureAttribute *textureAttribute = static_cast<const TextureAttribute *>(&attribute);

		if ((textureAttribute->textureName == textureName) && (textureAttribute->texcoordIndex == texcoordIndex))
		{
			return (true);
		}
	}

	return (false);
}

void TextureAttribute::SetTexture(const char *name)
{
	Texture *object = textureObject;

	if (name)
	{
		if (name != &textureName[0])
		{
			textureName = name;
		}

		textureObject = Texture::Get(name);
	}
	else
	{
		textureName[0] = 0;
		textureObject = nullptr;
	}

	if (object)
	{
		object->Release();
	}
}

void TextureAttribute::SetTexture(Texture *texture)
{
	Texture *object = textureObject;
	if (texture != object)
	{
		if (object)
		{
			object->Release();
		}

		if (texture)
		{
			texture->Retain();
		}

		textureObject = texture;
		textureName[0] = 0;
	}
}

void TextureAttribute::SetTexture(const TextureHeader *header, const void *image)
{
	Texture *object = textureObject;

	textureName[0] = 0;

	if (header)
	{
		textureObject = Texture::Get(header, image);
	}
	else
	{
		textureObject = nullptr;
	}

	if (object)
	{
		object->Release();
	}
}


DiffuseTextureAttribute::DiffuseTextureAttribute() : TextureAttribute(kAttributeDiffuseTexture)
{
}

DiffuseTextureAttribute::DiffuseTextureAttribute(const char *name) : TextureAttribute(kAttributeDiffuseTexture, name)
{
}

DiffuseTextureAttribute::DiffuseTextureAttribute(Texture *texture) : TextureAttribute(kAttributeDiffuseTexture, texture)
{
}

DiffuseTextureAttribute::DiffuseTextureAttribute(const TextureHeader *header, const void *image) : TextureAttribute(kAttributeDiffuseTexture, header, image)
{
}

DiffuseTextureAttribute::DiffuseTextureAttribute(const char *name, const TextureHeader *header, const void *image) : TextureAttribute(kAttributeDiffuseTexture, name, header, image)
{
}

DiffuseTextureAttribute::DiffuseTextureAttribute(const DiffuseTextureAttribute& diffuseTextureAttribute) : TextureAttribute(diffuseTextureAttribute)
{
}

DiffuseTextureAttribute::~DiffuseTextureAttribute()
{
}

Attribute *DiffuseTextureAttribute::Replicate(void) const
{
	return (new DiffuseTextureAttribute(*this));
}


SpecularTextureAttribute::SpecularTextureAttribute() : TextureAttribute(kAttributeSpecularTexture)
{
}

SpecularTextureAttribute::SpecularTextureAttribute(const char *name) : TextureAttribute(kAttributeSpecularTexture, name)
{
}

SpecularTextureAttribute::SpecularTextureAttribute(Texture *texture) : TextureAttribute(kAttributeSpecularTexture, texture)
{
}

SpecularTextureAttribute::SpecularTextureAttribute(const TextureHeader *header, const void *image) : TextureAttribute(kAttributeSpecularTexture, header, image)
{
}

SpecularTextureAttribute::SpecularTextureAttribute(const SpecularTextureAttribute& specularTextureAttribute) : TextureAttribute(specularTextureAttribute)
{
}

SpecularTextureAttribute::~SpecularTextureAttribute()
{
}

Attribute *SpecularTextureAttribute::Replicate(void) const
{
	return (new SpecularTextureAttribute(*this));
}


EmissionTextureAttribute::EmissionTextureAttribute() : TextureAttribute(kAttributeEmissionTexture)
{
}

EmissionTextureAttribute::EmissionTextureAttribute(const char *name) : TextureAttribute(kAttributeEmissionTexture, name)
{
}

EmissionTextureAttribute::EmissionTextureAttribute(Texture *texture) : TextureAttribute(kAttributeEmissionTexture, texture)
{
}

EmissionTextureAttribute::EmissionTextureAttribute(const TextureHeader *header, const void *image) : TextureAttribute(kAttributeEmissionTexture, header, image)
{
}

EmissionTextureAttribute::EmissionTextureAttribute(const EmissionTextureAttribute& emissionTextureAttribute) : TextureAttribute(emissionTextureAttribute)
{
}

EmissionTextureAttribute::~EmissionTextureAttribute()
{
}

Attribute *EmissionTextureAttribute::Replicate(void) const
{
	return (new EmissionTextureAttribute(*this));
}


NormalTextureAttribute::NormalTextureAttribute() : TextureAttribute(kAttributeNormalTexture)
{
	parallaxScale = 1.0F;
}

NormalTextureAttribute::NormalTextureAttribute(const char *name) : TextureAttribute(kAttributeNormalTexture, name)
{
	parallaxScale = 1.0F;
}

NormalTextureAttribute::NormalTextureAttribute(Texture *texture) : TextureAttribute(kAttributeNormalTexture, texture)
{
	parallaxScale = 1.0F;
}

NormalTextureAttribute::NormalTextureAttribute(const TextureHeader *header, const void *image) : TextureAttribute(kAttributeNormalTexture, header, image)
{
	parallaxScale = 1.0F;
}

NormalTextureAttribute::NormalTextureAttribute(const NormalTextureAttribute& normalTextureAttribute) : TextureAttribute(normalTextureAttribute)
{
	parallaxScale = normalTextureAttribute.parallaxScale;
}

NormalTextureAttribute::~NormalTextureAttribute()
{
}

Attribute *NormalTextureAttribute::Replicate(void) const
{
	return (new NormalTextureAttribute(*this));
}

void NormalTextureAttribute::Pack(Packer& data, unsigned_int32 packFlags) const
{
	TextureAttribute::Pack(data, packFlags);

	if (parallaxScale != 1.0F)
	{
		data << ChunkHeader('SCAL', 4);
		data << parallaxScale;
	}

	data << TerminatorChunk;
}

void NormalTextureAttribute::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	TextureAttribute::Unpack(data, unpackFlags);
	UnpackChunkList<NormalTextureAttribute>(data, unpackFlags);
}

bool NormalTextureAttribute::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'SCAL':

			data >> parallaxScale;
			return (true);
	}

	return (false);
}

void *NormalTextureAttribute::BeginSettingsUnpack(void)
{
	parallaxScale = 1.0F;
	return (TextureAttribute::BeginSettingsUnpack());
}

bool NormalTextureAttribute::operator ==(const Attribute& attribute) const
{
	if (TextureAttribute::operator ==(attribute))
	{
		const NormalTextureAttribute *normalTextureAttribute = static_cast<const NormalTextureAttribute *>(&attribute);

		if (normalTextureAttribute->parallaxScale == parallaxScale)
		{
			return (true);
		}
	}

	return (false);
}


OpacityTextureAttribute::OpacityTextureAttribute() : TextureAttribute(kAttributeOpacityTexture)
{
}

OpacityTextureAttribute::OpacityTextureAttribute(const char *name) : TextureAttribute(kAttributeOpacityTexture, name)
{
}

OpacityTextureAttribute::OpacityTextureAttribute(Texture *texture) : TextureAttribute(kAttributeOpacityTexture, texture)
{
}

OpacityTextureAttribute::OpacityTextureAttribute(const TextureHeader *header, const void *image) : TextureAttribute(kAttributeOpacityTexture, header, image)
{
}

OpacityTextureAttribute::OpacityTextureAttribute(const OpacityTextureAttribute& opacityTextureAttribute) : TextureAttribute(opacityTextureAttribute)
{
}

OpacityTextureAttribute::~OpacityTextureAttribute()
{
}

Attribute *OpacityTextureAttribute::Replicate(void) const
{
	return (new OpacityTextureAttribute(*this));
}


EnvironmentTextureAttribute::EnvironmentTextureAttribute() : TextureAttribute(kAttributeEnvironmentTexture)
{
}

EnvironmentTextureAttribute::EnvironmentTextureAttribute(const char *name) : TextureAttribute(kAttributeEnvironmentTexture, name)
{
}

EnvironmentTextureAttribute::EnvironmentTextureAttribute(Texture *texture) : TextureAttribute(kAttributeEnvironmentTexture, texture)
{
}

EnvironmentTextureAttribute::EnvironmentTextureAttribute(const TextureHeader *header, const void *image) : TextureAttribute(kAttributeEnvironmentTexture, header, image)
{
}

EnvironmentTextureAttribute::EnvironmentTextureAttribute(const EnvironmentTextureAttribute& environmentTextureAttribute) : TextureAttribute(environmentTextureAttribute)
{
}

EnvironmentTextureAttribute::~EnvironmentTextureAttribute()
{
}

Attribute *EnvironmentTextureAttribute::Replicate(void) const
{
	return (new EnvironmentTextureAttribute(*this));
}


FireTextureAttribute::FireTextureAttribute() : TextureAttribute(kAttributeFireTexture, "C4/noise")
{
}

FireTextureAttribute::FireTextureAttribute(float intensity, const Vector2D& velocity1, const Vector2D& velocity2, const Vector2D& velocity3) : TextureAttribute(kAttributeFireTexture, "C4/noise")
{
	fireParams.fireIntensity = intensity;
	fireParams.noiseVelocity[0] = velocity1;
	fireParams.noiseVelocity[1] = velocity2;
	fireParams.noiseVelocity[2] = velocity3;
}

FireTextureAttribute::FireTextureAttribute(const FireTextureAttribute& fireTextureAttribute) : TextureAttribute(fireTextureAttribute)
{
	fireParams.fireIntensity = fireTextureAttribute.fireParams.fireIntensity;
	fireParams.noiseVelocity[0] = fireTextureAttribute.fireParams.noiseVelocity[0];
	fireParams.noiseVelocity[1] = fireTextureAttribute.fireParams.noiseVelocity[1];
	fireParams.noiseVelocity[2] = fireTextureAttribute.fireParams.noiseVelocity[2];
}

FireTextureAttribute::~FireTextureAttribute()
{
}

Attribute *FireTextureAttribute::Replicate(void) const
{
	return (new FireTextureAttribute(*this));
}

void FireTextureAttribute::Pack(Packer& data, unsigned_int32 packFlags) const
{
	TextureAttribute::Pack(data, packFlags);

	data << ChunkHeader('INTS', 4);
	data << fireParams.fireIntensity;

	data << ChunkHeader('VELO', sizeof(Vector2D) * 3);
	data << fireParams.noiseVelocity[0];
	data << fireParams.noiseVelocity[1];
	data << fireParams.noiseVelocity[2];

	data << TerminatorChunk;
}

void FireTextureAttribute::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	TextureAttribute::Unpack(data, unpackFlags);
	UnpackChunkList<FireTextureAttribute>(data, unpackFlags);
}

bool FireTextureAttribute::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'INTS':

			data >> fireParams.fireIntensity;
			return (true);

		case 'VELO':

			data >> fireParams.noiseVelocity[0];
			data >> fireParams.noiseVelocity[1];
			data >> fireParams.noiseVelocity[2];
			return (true);
	}

	return (false);
}

bool FireTextureAttribute::operator ==(const Attribute& attribute) const
{
	if (TextureAttribute::operator ==(attribute))
	{
		const FireTextureAttribute *fireTextureAttribute = static_cast<const FireTextureAttribute *>(&attribute);

		if (fireTextureAttribute->fireParams.fireIntensity != fireParams.fireIntensity)
		{
			return (false);
		}

		if (fireTextureAttribute->fireParams.noiseVelocity[0] != fireParams.noiseVelocity[0])
		{
			return (false);
		}

		if (fireTextureAttribute->fireParams.noiseVelocity[1] != fireParams.noiseVelocity[1])
		{
			return (false);
		}

		if (fireTextureAttribute->fireParams.noiseVelocity[2] != fireParams.noiseVelocity[2])
		{
			return (false);
		}
	}

	return (false);
}

void FireTextureAttribute::CalculateNoiseVelocities(int32 speed, Vector2D *velocity)
{
	enum
	{
		kFireSpeedCount		= 25
	};

	struct FireData
	{
		float	greenSpeed1;
		float	greenSpeed2;
		float	greenSpeed3;

		int16	redSpeedMin;
		int16	redSpeedMax;
	};

	static const FireData fireData[kFireSpeedCount] =
	{
		{  3.0F,   7.0F,  13.0F,  0,  3},
		{  5.0F,  11.0F,  23.0F,  1,  4},
		{  7.0F,  13.0F,  29.0F,  1,  5},
		{ 11.0F,  23.0F,  47.0F,  2,  6},
		{ 13.0F,  29.0F,  53.0F,  3,  7},
		{ 17.0F,  37.0F,  67.0F,  4,  9},
		{ 19.0F,  41.0F,  79.0F,  4, 10},
		{ 23.0F,  47.0F,  97.0F,  4, 11},
		{ 29.0F,  59.0F, 113.0F,  5, 12},
		{ 31.0F,  61.0F, 127.0F,  5, 14},
		{ 37.0F,  73.0F, 149.0F,  5, 15},
		{ 41.0F,  83.0F, 163.0F,  6, 16},
		{ 43.0F,  89.0F, 173.0F,  6, 17},
		{ 47.0F,  97.0F, 191.0F,  6, 18},
		{ 53.0F, 107.0F, 211.0F,  7, 20},
		{ 59.0F, 113.0F, 239.0F,  8, 22},
		{ 61.0F, 127.0F, 241.0F,  9, 22},
		{ 67.0F, 137.0F, 269.0F,  9, 24},
		{ 71.0F, 139.0F, 283.0F,  9, 24},
		{ 73.0F, 149.0F, 293.0F,  9, 25},
		{ 79.0F, 157.0F, 317.0F, 10, 28},
		{ 83.0F, 167.0F, 331.0F, 10, 30},
		{ 89.0F, 179.0F, 359.0F, 10, 31},
		{ 97.0F, 193.0F, 389.0F, 11, 32},
		{101.0F, 199.0F, 401.0F, 11, 33}
	};

	static const float redSpeed[34] =
	{
		1.0F, 2.0F, 3.0F, 5.0F, 7.0F, 11.0F, 13.0F, 17.0F, 19.0F, 23.0F, 29.0F,
		31.0F, 37.0F, 41.0F, 43.0F, 47.0F, 53.0F, 59.0F, 61.0F, 67.0F, 71.0F, 73.0F,
		79.0F, 83.0F, 89.0F, 97.0F, 101.0F, 101.0F, 103.0F, 107.0F, 109.0F, 113.0F, 127.0F, 131.0F
	};

	static const float inverse_period = 1.0F / 120000.0F;

	const FireData *data = &fireData[MaxZero(Min(speed, kFireSpeedCount - 1))];

	int32 m = data->redSpeedMin;
	int32 r = data->redSpeedMax - m + 1;

	float x1 = redSpeed[Math::Random(r) + m] * inverse_period;
	float x2 = redSpeed[Math::Random(r) + m] * inverse_period;
	float x3 = redSpeed[Math::Random(r) + m] * inverse_period;

	if (Math::Random(2) == 0)
	{
		x1 = -x1;
	}

	if (Math::Random(2) == 0)
	{
		x2 = -x2;
	}

	if (Math::Random(2) == 0)
	{
		x3 = -x3;
	}

	velocity[0].Set(x1, -data->greenSpeed1 * inverse_period);
	velocity[1].Set(x2, -data->greenSpeed2 * inverse_period);
	velocity[2].Set(x3, -data->greenSpeed3 * inverse_period);
}


DeltaDepthAttribute::DeltaDepthAttribute() : Attribute(kAttributeDeltaDepth)
{
}

DeltaDepthAttribute::DeltaDepthAttribute(float scale) : Attribute(kAttributeDeltaDepth)
{
	deltaScale = scale;
}

DeltaDepthAttribute::DeltaDepthAttribute(const DeltaDepthAttribute& deltaDepthAttribute) : Attribute(deltaDepthAttribute)
{
	deltaScale = deltaDepthAttribute.deltaScale;
}

DeltaDepthAttribute::~DeltaDepthAttribute()
{
}

Attribute *DeltaDepthAttribute::Replicate(void) const
{
	return (new DeltaDepthAttribute(*this));
}

void DeltaDepthAttribute::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Attribute::Pack(data, packFlags);

	data << ChunkHeader('SCAL', 4);
	data << deltaScale;

	data << TerminatorChunk;
}

void DeltaDepthAttribute::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Attribute::Unpack(data, unpackFlags);
	UnpackChunkList<DeltaDepthAttribute>(data, unpackFlags);
}

bool DeltaDepthAttribute::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'SCAL':

			data >> deltaScale;
			return (true);
	}

	return (false);
}

bool DeltaDepthAttribute::operator ==(const Attribute& attribute) const
{
	if (attribute.GetAttributeType() == kAttributeDeltaDepth)
	{
		const DeltaDepthAttribute *deltaDepthAttribute = static_cast<const DeltaDepthAttribute *>(&attribute);
		return (deltaDepthAttribute->deltaScale == deltaScale);
	}

	return (false);
}


DepthRampAttribute::DepthRampAttribute() : Attribute(kAttributeDepthRamp)
{
}

DepthRampAttribute::DepthRampAttribute(const Range<float>& range) : Attribute(kAttributeDepthRamp)
{
	rampRange = range;
}

DepthRampAttribute::DepthRampAttribute(const DepthRampAttribute& depthRampAttribute) : Attribute(depthRampAttribute)
{
	rampRange = depthRampAttribute.rampRange;
}

DepthRampAttribute::~DepthRampAttribute()
{
}

Attribute *DepthRampAttribute::Replicate(void) const
{
	return (new DepthRampAttribute(*this));
}

void DepthRampAttribute::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Attribute::Pack(data, packFlags);

	data << ChunkHeader('RANG', sizeof(Range<float>));
	data << rampRange;

	data << TerminatorChunk;
}

void DepthRampAttribute::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Attribute::Unpack(data, unpackFlags);
	UnpackChunkList<DepthRampAttribute>(data, unpackFlags);
}

bool DepthRampAttribute::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'RANG':

			data >> rampRange;
			return (true);
	}

	return (false);
}

bool DepthRampAttribute::operator ==(const Attribute& attribute) const
{
	if (attribute.GetAttributeType() == kAttributeDepthRamp)
	{
		const DepthRampAttribute *depthRampAttribute = static_cast<const DepthRampAttribute *>(&attribute);
		return (depthRampAttribute->rampRange == rampRange);
	}

	return (false);
}

// ZYUQURM
