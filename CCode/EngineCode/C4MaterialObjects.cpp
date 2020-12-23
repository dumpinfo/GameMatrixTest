 

#include "C4MaterialObjects.h"

#if C4LEGACY

	#include "C4Horizon.h"

#endif


using namespace C4;


ResourceDescriptor MaterialResource::descriptor("mtl");

Map<Substance> MaterialObject::substanceMap;
Map<MaterialRegistration> MaterialObject::registrationMap;
Map<MaterialObject> MaterialObject::materialMap;


Substance::Substance(SubstanceType type, const char *name)
{
	substanceType = type;
	substanceName = name;
}

Substance::~Substance()
{
}


MaterialRegistration::MaterialRegistration(MaterialType type, const char *name, const void *cookie)
{
	materialType = type;
	resourceName = name;
	materialCookie = cookie;

	MaterialObject::registrationMap.Insert(this);
}

MaterialRegistration::~MaterialRegistration()
{
}


MaterialResource::MaterialResource(const char *name, ResourceCatalog *catalog) : Resource<MaterialResource>(name, catalog)
{
}

MaterialResource::~MaterialResource()
{
}


MaterialObject::MaterialObject() : Object(kObjectMaterial)
{
	materialType = kMaterialGeneric;
	materialSubstance = kSubstanceNone;
	materialFlags = 0;

	textureBlendMode = kTextureBlendAdd;
	texcoordGeneration.Set(0.0625F, 0.0625F, 0.0F, 0.0F);

	for (machine a = 0; a < kMaxMaterialTexcoordCount; a++)
	{
		texcoordScale[a].Set(1.0F, 1.0F);
		texcoordOffset[a].Set(0.0F, 0.0F);
		texcoordVelocity[a].Set(0.0F, 0.0F);
	}
}

MaterialObject::MaterialObject(const MaterialObject& materialObject) : Object(kObjectMaterial)
{
	materialType = kMaterialGeneric;
	materialSubstance = materialObject.materialSubstance;
	materialFlags = materialObject.materialFlags;

	textureBlendMode = materialObject.textureBlendMode;
	texcoordGeneration = materialObject.texcoordGeneration;

	for (machine a = 0; a < kMaxMaterialTexcoordCount; a++)
	{
		texcoordScale[a] = materialObject.texcoordScale[a];
		texcoordOffset[a] = materialObject.texcoordOffset[a];
		texcoordVelocity[a] = materialObject.texcoordVelocity[a];
	}
}

MaterialObject::~MaterialObject()
{
}

MaterialObject *MaterialObject::Clone(void) const
{
	MaterialObject *object = new MaterialObject(*this);

	const Attribute *attribute = attributeList.First();
	while (attribute)
	{
		object->AddAttribute(attribute->Clone());
		attribute = attribute->Next();
	} 

	return (object);
} 

void MaterialObject::Pack(Packer& data, unsigned_int32 packFlags) const 
{
	if (materialType != kMaterialGeneric)
	{ 
		data << ChunkHeader('TYPE', 4);
		data << materialType; 
	} 

	data << ChunkHeader('SBST', 4);
	data << materialSubstance;
 
	data << ChunkHeader('FLAG', 4);
	data << materialFlags;

	const Attribute *attribute = attributeList.First();
	while (attribute)
	{
		PackHandle handle = data.BeginChunk('ATTR');
		attribute->PackType(data);
		attribute->Pack(data, packFlags);
		data.EndChunk(handle);

		attribute = attribute->Next();
	}

	data << ChunkHeader('BLND', 4);
	data << textureBlendMode;

	data << ChunkHeader('TGEN', sizeof(Vector4D));
	data << texcoordGeneration;

	for (machine a = 0; a < kMaxMaterialTexcoordCount; a++)
	{
		const Vector2D& scale = texcoordScale[a];
		if ((scale.x != 1.0F) || (scale.y != 1.0F))
		{
			data << ChunkHeader('SCAL', 4 + sizeof(Vector2D));
			data << int32(a);
			data << scale;
		}

		const Vector2D& offset = texcoordOffset[a];
		if ((offset.x != 0.0F) || (offset.y != 0.0F))
		{
			data << ChunkHeader('OFST', 4 + sizeof(Vector2D));
			data << int32(a);
			data << offset;
		}

		if (materialFlags & (kMaterialAnimateTexcoord0 << a))
		{
			data << ChunkHeader('TANM', 4 + sizeof(Vector2D));
			data << int32(a);
			data << texcoordVelocity[a];
		}
	}

	data << TerminatorChunk;
}

void MaterialObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	UnpackChunkList<MaterialObject>(data, unpackFlags);

	#if C4LEGACY

		if (data.GetVersion() <= 65)
		{
			HorizonTextureAttribute *horizonAttribute = nullptr;

			Attribute *attribute = attributeList.First();
			while (attribute)
			{
				Attribute *next = attribute->Next();

				if (attribute->GetAttributeType() == kAttributeHorizonTexture)
				{
					if (!horizonAttribute)
					{
						horizonAttribute = static_cast<HorizonTextureAttribute *>(attribute);

						ResourceName name(horizonAttribute->GetTextureName());
						int32 length = name.Length();

						if ((length > 8) && (Text::CompareText(&name[length - 8], "-nrml", 5)))
						{
							name[length - 7] = 'h';
							name[length - 6] = 'r';
							name[length - 5] = 'z';
							name[length - 4] = 'n';
							name[length - 3] = 0;
						}
						else
						{
							name[length - 1] = 0;
						}

						horizonAttribute->SetTexture(name);
					}
					else
					{
						delete attribute;
					}
				}

				attribute = next;
			}
		}

	#endif
}

bool MaterialObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'TYPE':

			data >> materialType;
			materialMap.Insert(this);
			return (true);

		case 'SBST':

			data >> materialSubstance;
			return (true);

		case 'FLAG':

			data >> materialFlags;
			return (true);

		case 'ATTR':
		{
			Attribute *attribute = Attribute::Create(data, unpackFlags);
			if (attribute)
			{
				attribute->Unpack(++data, unpackFlags);
				attributeList.Append(attribute);
				return (true);
			}

			break;
		}

		case 'BLND':

			data >> textureBlendMode;
			return (true);

		case 'TGEN':

			data >> texcoordGeneration;
			return (true);

		case 'SCAL':
		{
			int32	index;

			data >> index;
			data >> texcoordScale[index];
			return (true);
		}

		case 'OFST':
		{
			int32	index;

			data >> index;
			data >> texcoordOffset[index];
			return (true);
		}

		case 'TANM':
		{
			int32	index;

			data >> index;
			data >> texcoordVelocity[index];
			return (true);
		}
	}

	return (false);
}

void *MaterialObject::BeginSettingsUnpack(void)
{
	materialType = kMaterialGeneric;

	attributeList.Purge();
	MapElement<MaterialObject>::Detach();

	return (nullptr);
}

MaterialObject *MaterialObject::Get(MaterialType type)
{
	MaterialObject *object = materialMap.Find(type);
	if (object)
	{
		object->Retain();
		return (object);
	}

	const MaterialRegistration *registration = registrationMap.Find(type);
	if (registration)
	{
		MaterialResource *resource = MaterialResource::Get(registration->GetResourceName());
		if (resource)
		{
			Unpacker unpacker(resource->GetMaterialData(), resource->GetVersion());

			object = new MaterialObject;
			object->Unpack(unpacker, kUnpackEditor);

			object->materialType = type;
			materialMap.Insert(object);

			resource->Release();
			return (object);
		}
	}

	return (nullptr);
}

MaterialObject *MaterialObject::Get(const char *name)
{
	MaterialResource *resource = MaterialResource::Get(name);
	if (resource)
	{
		Unpacker unpacker(resource->GetMaterialData(), resource->GetVersion());

		MaterialObject *object = new MaterialObject;
		object->Unpack(unpacker, kUnpackEditor);

		resource->Release();
		return (object);
	}

	return (nullptr);
}

void MaterialObject::ReleaseCache(void)
{
	MaterialObject *object = materialMap.First();
	while (object)
	{
		MaterialObject *next = object->MapElement<MaterialObject>::Next();
		object->Release();
		object = next;
	}
}

bool MaterialObject::operator ==(const MaterialObject& object) const
{
	unsigned_int32 flags = materialFlags;
	if (object.materialFlags != flags)
	{
		return (false);
	}

	if (object.textureBlendMode != textureBlendMode)
	{
		return (false);
	}

	if (object.texcoordGeneration != texcoordGeneration)
	{
		return (false);
	}

	for (machine a = 0; a < kMaxMaterialTexcoordCount; a++)
	{
		if (object.texcoordScale[a] != texcoordScale[a])
		{
			return (false);
		}

		if ((flags & (kMaterialAnimateTexcoord0 << a)) && (object.texcoordVelocity[a] != texcoordVelocity[a]))
		{
			return (false);
		}
	}

	const Attribute *attribute = GetFirstAttribute();
	while (attribute)
	{
		const Attribute *objectAttribute = object.GetFirstAttribute();
		while (objectAttribute)
		{
			if (*attribute == *objectAttribute)
			{
				break;
			}

			objectAttribute = objectAttribute->Next();
		}

		if (!objectAttribute)
		{
			return (false);
		}

		attribute = attribute->Next();
	}

	const Attribute *objectAttribute = object.GetFirstAttribute();
	while (objectAttribute)
	{
		attribute = GetFirstAttribute();
		while (attribute)
		{
			if (*objectAttribute == *attribute)
			{
				break;
			}

			attribute = attribute->Next();
		}

		if (!attribute)
		{
			return (false);
		}

		objectAttribute = objectAttribute->Next();
	}

	return (true);
}

bool MaterialObject::ShaderMaterial(void) const
{
	const Attribute *attribute = attributeList.First();
	return ((attribute) && (attribute->GetAttributeType() == kAttributeShader));
}

Attribute *MaterialObject::FindAttribute(AttributeType type, int32 index) const
{
	Attribute *attribute = attributeList.First();
	while (attribute)
	{
		if ((attribute->GetAttributeType() == type) && (--index < 0))
		{
			return (attribute);
		}

		attribute = attribute->Next();
	}

	return (nullptr);
}

// ZYUQURM
