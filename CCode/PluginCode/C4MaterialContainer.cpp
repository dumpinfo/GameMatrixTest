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


#include "C4MaterialContainer.h"
#include "C4WorldEditor.h"


using namespace C4;


namespace
{
	const float kTexcoordSpeedFactor = 1.0F / 120000.0F;


	enum
	{
		kMaterialDiffuseTexture1,
		kMaterialDiffuseTexture2,
		kMaterialNormalTexture1,
		kMaterialNormalTexture2,
		kMaterialSpecularTexture,
		kMaterialEmissionTexture,
		kMaterialEnvironmentTexture,
		kMaterialOpacityTexture,
		kMaterialTextureCount
	};


	const Type materialTextureIdentifier[kMaterialTextureCount] =
	{
		'TX1M', 'TX2M', 'NM1M', 'NM2M', 'SPCM', 'EMSM', 'ENVM', 'OPCM'
	};

	const AttributeType materialTextureAttributeType[kMaterialTextureCount] =
	{
		kAttributeDiffuseTexture, kAttributeDiffuseTexture, kAttributeNormalTexture, kAttributeNormalTexture, kAttributeSpecularTexture, kAttributeEmissionTexture, kAttributeEnvironmentTexture, kAttributeOpacityTexture
	};

	const unsigned_int8 materialTextureAttributeIndex[kMaterialTextureCount] =
	{
		0, 1, 0, 1, 0, 0, 0, 0
	};
}


MaterialContainer::MaterialContainer()
{
	materialObject = nullptr;
	usageCount = 0;
	materialName[0] = 0;
	previewType = kPrimitivePlate;
}

MaterialContainer::MaterialContainer(MaterialObject *object, const char *name)
{
	if (object)
	{
		object->Retain();
	}
	else
	{
		object = new MaterialObject;
		object->AddAttribute(new DiffuseAttribute(K::white));

		if (!name)
		{
			name = TheWorldEditor->GetStringTable()->GetString(StringID('MATL', 'DFLT'));
		}
	}

	materialObject = object;

	usageCount = 0;
	previewType = kPrimitivePlate;

	if (name)
	{
		materialName = name;
	}
	else
	{
		materialName[0] = 0;
	}
}

MaterialContainer::MaterialContainer(const MaterialContainer& materialContainer)
{
	materialObject = materialContainer.materialObject->Clone();
	previewType = materialContainer.previewType;

	usageCount = 0;
	materialName[0] = 0;
}

MaterialContainer::~MaterialContainer()
{
	if (materialObject)
	{
		materialObject->Release();
	}
}

void MaterialContainer::Pack(Packer& data, unsigned_int32 packFlags) const 
{
	if (materialObject)
	{ 
		data << ChunkHeader('MOBJ', 4);
		data << materialObject->GetObjectIndex(); 
	}

	data << ChunkHeader('PRIM', 4); 
	data << previewType;
 
	PackHandle handle = data.BeginChunk('NAME'); 
	data << materialName;
	data.EndChunk(handle);

	data << TerminatorChunk; 
}

void MaterialContainer::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	UnpackChunkList<MaterialContainer>(data, unpackFlags);
}

bool MaterialContainer::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'MOBJ':
		{
			int32	objectIndex;

			data >> objectIndex;
			data.AddObjectLink(objectIndex, &MaterialObjectLinkProc, this);
			return (true);
		}

		case 'PRIM':

			data >> previewType;
			return (true);

		case 'NAME':

			data >> materialName;
			return (true);
	}

	return (false);
}

void MaterialContainer::MaterialObjectLinkProc(Object *object, void *cookie)
{
	MaterialContainer *container = static_cast<MaterialContainer *>(cookie);
	container->SetMaterialObject(static_cast<MaterialObject *>(object));
}

int32 MaterialContainer::GetCategoryCount(void) const
{
	return (5);
}

Type MaterialContainer::GetCategoryType(int32 index, const char **title) const
{
	const StringTable *table = TheWorldEditor->GetStringTable();

	if (index == 0)
	{
		*title = table->GetString(StringID('MATL', 'DIFF'));
		return ('DIFF');
	}

	if (index == 1)
	{
		*title = table->GetString(StringID('MATL', 'SPEC'));
		return ('SPEC');
	}

	if (index == 2)
	{
		*title = table->GetString(StringID('MATL', 'AMBT'));
		return ('AMBT');
	}

	if (index == 3)
	{
		*title = table->GetString(StringID('MATL', 'FLAG'));
		return ('FLAG');
	}

	if (index == 4)
	{
		*title = table->GetString(StringID('MATL', 'TEXC'));
		return ('TEXC');
	}

	return (0);
}

int32 MaterialContainer::GetCategorySettingCount(Type category) const
{
	if (category == 'DIFF')
	{
		return (17);
	}

	if (category == 'SPEC')
	{
		return (10);
	}

	if (category == 'AMBT')
	{
		return (16);
	}

	if (category == 'FLAG')
	{
		return (20);
	}

	if (category == 'TEXC')
	{
		return (18);
	}

	return (0);
}

Setting *MaterialContainer::GetTextureMapSetting(Type category, int32 index) const
{
	Type identifier = materialTextureIdentifier[index];

	const TextureAttribute *attribute = static_cast<TextureAttribute *>(materialObject->FindAttribute(materialTextureAttributeType[index], materialTextureAttributeIndex[index]));
	const char *name = (attribute) ? attribute->GetTextureName() : "";

	const StringTable *table = TheWorldEditor->GetStringTable();
	const char *title = table->GetString(StringID('MATL', category, identifier));
	const char *picker = table->GetString(StringID('MATL', 'PICK', identifier));
	return (new ResourceSetting(identifier, name, title, picker, TextureResource::GetDescriptor()));
}

Setting *MaterialContainer::GetTexcoordInputSetting(Type category, int32 index) const
{
	Type identifier = materialTextureIdentifier[index] - 4;

	const TextureAttribute *attribute = static_cast<TextureAttribute *>(materialObject->FindAttribute(materialTextureAttributeType[index], materialTextureAttributeIndex[index]));
	int32 selection = (attribute) ? attribute->GetTexcoordIndex() : 0;

	const StringTable *table = TheWorldEditor->GetStringTable();
	const char *title = table->GetString(StringID('MATL', category, identifier));
	MenuSetting *menu = new MenuSetting(identifier, selection, title, 2);

	menu->SetMenuItemString(0, table->GetString(StringID('MATL', 'ITEX', 'TEX1')));
	menu->SetMenuItemString(1, table->GetString(StringID('MATL', 'ITEX', 'TEX2')));

	return (menu);
}

bool MaterialContainer::SetTextureMapSetting(const Setting *setting, AttributeType type)
{
	const char *name = static_cast<const ResourceSetting *>(setting)->GetResourceName();
	if (name[0] != 0)
	{
		Attribute	*attribute;

		switch (type)
		{
			case kAttributeDiffuseTexture:

				attribute = new DiffuseTextureAttribute(name);
				break;

			case kAttributeSpecularTexture:

				attribute = new SpecularTextureAttribute(name);
				break;

			case kAttributeEmissionTexture:

				attribute = new EmissionTextureAttribute(name);
				break;

			case kAttributeNormalTexture:

				attribute = new NormalTextureAttribute(name);
				break;

			case kAttributeOpacityTexture:

				attribute = new OpacityTextureAttribute(name);
				break;

			case kAttributeEnvironmentTexture:

				attribute = new EnvironmentTextureAttribute(name);
				break;
		}

		materialObject->AddAttribute(attribute);
		return (true);
	}

	return (false);
}

void MaterialContainer::SetTexcoordInputSetting(const Setting *setting, AttributeType type, int32 index)
{
	Attribute *attribute = materialObject->FindAttribute(type, index);

	if (attribute)
	{
		int32 selection = static_cast<const MenuSetting *>(setting)->GetMenuSelection();
		static_cast<TextureAttribute *>(attribute)->SetTexcoordIndex(selection);
	}
}

Setting *MaterialContainer::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	const StringTable *table = TheWorldEditor->GetStringTable();

	if (category == 'DIFF')
	{
		if (index == 0)
		{
			const char *title = table->GetString(StringID('MATL', 'DIFF', 'HDIF'));
			return (new HeadingSetting('HDIF', title));
		}

		if (index == 1)
		{
			const Attribute *attribute = materialObject->FindAttribute(kAttributeDiffuse);
			const ColorRGBA& color = (attribute) ? static_cast<const DiffuseAttribute *>(attribute)->GetDiffuseColor() : K::white;

			const char *title = table->GetString(StringID('MATL', 'DIFF', 'DIFF'));
			const char *picker = table->GetString(StringID('MATL', 'PICK', 'DIFF'));
			return (new CheckColorSetting('DIFF', (attribute != nullptr), color, title, picker, kColorPickerAlpha));
		}

		if (index == 2)
		{
			const char *title = table->GetString(StringID('MATL', 'DIFF', 'HTXT'));
			return (new HeadingSetting('HTXT', title));
		}

		if (index == 3)
		{
			return (GetTextureMapSetting('DIFF', kMaterialDiffuseTexture1));
		}

		if (index == 4)
		{
			return (GetTextureMapSetting('DIFF', kMaterialDiffuseTexture2));
		}

		if (index == 5)
		{
			return (GetTexcoordInputSetting('DIFF', kMaterialDiffuseTexture1));
		}

		if (index == 6)
		{
			return (GetTexcoordInputSetting('DIFF', kMaterialDiffuseTexture2));
		}

		if (index == 7)
		{
			int32 selection = 0;
			switch (materialObject->GetTextureBlendMode())
			{
				case kTextureBlendAverage:

					selection = 1;
					break;

				case kTextureBlendMultiply:

					selection = 2;
					break;

				case kTextureBlendVertexAlpha:

					selection = 3;
					break;

				case kTextureBlendPrimaryAlpha:

					selection = 4;
					break;

				case kTextureBlendSecondaryAlpha:

					selection = 5;
					break;

				case kTextureBlendPrimaryInverseAlpha:

					selection = 6;
					break;

				case kTextureBlendSecondaryInverseAlpha:

					selection = 7;
					break;
			}

			const char *title = table->GetString(StringID('MATL', 'DIFF', 'BLND'));
			MenuSetting *menu = new MenuSetting('BLND', selection, title, 8);

			menu->SetMenuItemString(0, table->GetString(StringID('MATL', 'DIFF', 'BLND', 'ADD ')));
			menu->SetMenuItemString(1, table->GetString(StringID('MATL', 'DIFF', 'BLND', 'AVG ')));
			menu->SetMenuItemString(2, table->GetString(StringID('MATL', 'DIFF', 'BLND', 'MULT')));
			menu->SetMenuItemString(3, table->GetString(StringID('MATL', 'DIFF', 'BLND', 'VTXA')));
			menu->SetMenuItemString(4, table->GetString(StringID('MATL', 'DIFF', 'BLND', 'PRMA')));
			menu->SetMenuItemString(5, table->GetString(StringID('MATL', 'DIFF', 'BLND', 'SCDA')));
			menu->SetMenuItemString(6, table->GetString(StringID('MATL', 'DIFF', 'BLND', 'PRIA')));
			menu->SetMenuItemString(7, table->GetString(StringID('MATL', 'DIFF', 'BLND', 'SCIA')));

			return (menu);
		}

		if (index == 8)
		{
			const char *title = table->GetString(StringID('MATL', 'DIFF', 'HNRM'));
			return (new HeadingSetting('HNRM', title));
		}

		if (index == 9)
		{
			return (GetTextureMapSetting('DIFF', kMaterialNormalTexture1));
		}

		if (index == 10)
		{
			return (GetTextureMapSetting('DIFF', kMaterialNormalTexture2));
		}

		if (index == 11)
		{
			return (GetTexcoordInputSetting('DIFF', kMaterialNormalTexture1));
		}

		if (index == 12)
		{
			return (GetTexcoordInputSetting('DIFF', kMaterialNormalTexture2));
		}

		if (index == 13)
		{
			const char *title = table->GetString(StringID('MATL', 'DIFF', 'HHZN'));
			return (new HeadingSetting('HHZN', title));
		}

		if (index == 14)
		{
			const Attribute *attribute = materialObject->FindAttribute(kAttributeHorizonTexture);

			const char *title = table->GetString(StringID('MATL', 'DIFF', 'HRZN'));
			return (new BooleanSetting('HRZN', (attribute != nullptr), title));
		}

		if (index == 15)
		{
			const Attribute *attribute = materialObject->FindAttribute(kAttributeHorizonTexture);
			unsigned_int32 horizonFlags = (attribute) ? static_cast<const HorizonTextureAttribute *>(attribute)->GetHorizonFlags() : 0;

			const char *title = table->GetString(StringID('MATL', 'DIFF', 'HRZN', 'XINF'));
			return (new BooleanSetting('XINF', ((horizonFlags & kHorizonExcludeInfiniteLight) != 0), title));
		}

		if (index == 16)
		{
			const Attribute *attribute = materialObject->FindAttribute(kAttributeHorizonTexture);
			unsigned_int32 horizonFlags = (attribute) ? static_cast<const HorizonTextureAttribute *>(attribute)->GetHorizonFlags() : 0;

			const char *title = table->GetString(StringID('MATL', 'DIFF', 'HRZN', 'XPNT'));
			return (new BooleanSetting('XPNT', ((horizonFlags & kHorizonExcludePointLight) != 0), title));
		}
	}
	else if (category == 'SPEC')
	{
		if (index == 0)
		{
			const char *title = table->GetString(StringID('MATL', 'SPEC', 'HSPC'));
			return (new HeadingSetting('HSPC', title));
		}

		if (index == 1)
		{
			const Attribute *attribute = materialObject->FindAttribute(kAttributeSpecular);
			const ColorRGBA& color = (attribute) ? static_cast<const SpecularAttribute *>(attribute)->GetSpecularColor() : K::white;

			const char *title = table->GetString(StringID('MATL', 'SPEC', 'SPEC'));
			const char *picker = table->GetString(StringID('MATL', 'PICK', 'SPEC'));
			return (new CheckColorSetting('SPEC', (attribute != nullptr), color, title, picker));
		}

		if (index == 2)
		{
			const Attribute *attribute = materialObject->FindAttribute(kAttributeSpecular);
			int32 exponent = (attribute) ? (int32) static_cast<const SpecularAttribute *>(attribute)->GetSpecularExponent() : 50;

			const char *title = table->GetString(StringID('MATL', 'SPEC', 'EXPN'));
			return (new IntegerSetting('EXPN', exponent, title, 1, 200, 1));
		}

		if (index == 3)
		{
			return (GetTextureMapSetting('SPEC', kMaterialSpecularTexture));
		}

		if (index == 4)
		{
			return (GetTexcoordInputSetting('SPEC', kMaterialSpecularTexture));
		}

		if (index == 5)
		{
			const char *title = table->GetString(StringID('MATL', 'SPEC', 'HMFT'));
			return (new HeadingSetting('HMFT', title));
		}

		if (index == 6)
		{
			const Attribute *attribute = materialObject->FindAttribute(kAttributeMicrofacet);
			const ColorRGBA& color = (attribute) ? static_cast<const MicrofacetAttribute *>(attribute)->GetMicrofacetParams()->microfacetColor : K::white;

			const char *title = table->GetString(StringID('MATL', 'SPEC', 'MFCT'));
			const char *picker = table->GetString(StringID('MATL', 'PICK', 'MFCT'));
			return (new CheckColorSetting('MFCT', (attribute != nullptr), color, title, picker));
		}

		if (index == 7)
		{
			const Attribute *attribute = materialObject->FindAttribute(kAttributeMicrofacet);
			float reflectivity = (attribute) ? static_cast<const MicrofacetAttribute *>(attribute)->GetMicrofacetReflectivity() : 1.0F;

			const char *title = table->GetString(StringID('MATL', 'SPEC', 'MRFL'));
			return (new FloatSetting('MRFL', reflectivity, title, 0.01F, 1.0F, 0.01F));
		}

		if (index == 8)
		{
			const Attribute *attribute = materialObject->FindAttribute(kAttributeMicrofacet);
			float slope = (attribute) ? static_cast<const MicrofacetAttribute *>(attribute)->GetMicrofacetParams()->microfacetSlope.x : 0.5F;

			const char *title = table->GetString(StringID('MATL', 'SPEC', 'SLPX'));
			return (new FloatSetting('SLPX', slope, title, 0.01F, 0.5F, 0.01F));
		}

		if (index == 9)
		{
			const Attribute *attribute = materialObject->FindAttribute(kAttributeMicrofacet);
			float slope = (attribute) ? static_cast<const MicrofacetAttribute *>(attribute)->GetMicrofacetParams()->microfacetSlope.y : 0.5F;

			const char *title = table->GetString(StringID('MATL', 'SPEC', 'SLPY'));
			return (new FloatSetting('SLPY', slope, title, 0.01F, 0.5F, 0.01F));
		}
	}
	else if (category == 'AMBT')
	{
		if (index == 0)
		{
			const char *title = table->GetString(StringID('MATL', 'AMBT', 'HEMS'));
			return (new HeadingSetting('HEMS', title));
		}

		if (index == 1)
		{
			const Attribute *attribute = materialObject->FindAttribute(kAttributeEmission);
			const ColorRGBA& color = (attribute) ? static_cast<const EmissionAttribute *>(attribute)->GetEmissionColor() : K::transparent;

			const char *title = table->GetString(StringID('MATL', 'AMBT', 'EMIS'));
			const char *picker = table->GetString(StringID('MATL', 'PICK', 'EMIS'));
			return (new CheckColorSetting('EMIS', (attribute != nullptr), color, title, picker, kColorPickerAlpha));
		}

		if (index == 2)
		{
			return (GetTextureMapSetting('AMBT', kMaterialEmissionTexture));
		}

		if (index == 3)
		{
			return (GetTexcoordInputSetting('AMBT', kMaterialEmissionTexture));
		}

		if (index == 4)
		{
			const char *title = table->GetString(StringID('MATL', 'AMBT', 'HENV'));
			return (new HeadingSetting('HENV', title));
		}

		if (index == 5)
		{
			const Attribute *attribute = materialObject->FindAttribute(kAttributeEnvironment);
			const ColorRGBA& color = (attribute) ? static_cast<const EnvironmentAttribute *>(attribute)->GetEnvironmentColor() : K::white;

			const char *title = table->GetString(StringID('MATL', 'AMBT', 'ENVC'));
			const char *picker = table->GetString(StringID('MATL', 'PICK', 'ENVC'));
			return (new CheckColorSetting('ENVC', (attribute != nullptr), color, title, picker));
		}

		if (index == 6)
		{
			return (GetTextureMapSetting('AMBT', kMaterialEnvironmentTexture));
		}

		if (index == 7)
		{
			const char *title = table->GetString(StringID('MATL', 'AMBT', 'HRFL'));
			return (new HeadingSetting('HRFL', title));
		}

		if (index == 8)
		{
			const Attribute *attribute = materialObject->FindAttribute(kAttributeReflection);
			const ColorRGBA& color = (attribute) ? static_cast<const ReflectionAttribute *>(attribute)->GetReflectionColor() : K::white;

			const char *title = table->GetString(StringID('MATL', 'AMBT', 'REFL'));
			const char *picker = table->GetString(StringID('MATL', 'PICK', 'REFL'));
			return (new CheckColorSetting('REFL', (attribute != nullptr), color, title, picker));
		}

		if (index == 9)
		{
			const Attribute *attribute = materialObject->FindAttribute(kAttributeReflection);
			int32 reflectivity = (attribute) ? (int32) (static_cast<const ReflectionAttribute *>(attribute)->GetReflectionParams()->normalIncidenceReflectivity * 100.0F) : 100;

			const char *title = table->GetString(StringID('MATL', 'AMBT', 'NINC'));
			return (new IntegerSetting('NINC', reflectivity, title, 0, 100, 1));
		}

		if (index == 10)
		{
			const Attribute *attribute = materialObject->FindAttribute(kAttributeReflection);
			float scale = (attribute) ? static_cast<const ReflectionAttribute *>(attribute)->GetReflectionParams()->reflectionOffsetScale : 1.0F;

			const char *title = table->GetString(StringID('MATL', 'AMBT', 'RFLO'));
			return (new TextSetting('RFLO', scale, title));
		}

		if (index == 11)
		{
			const char *title = table->GetString(StringID('MATL', 'AMBT', 'HRFR'));
			return (new HeadingSetting('HRFR', title));
		}

		if (index == 12)
		{
			const Attribute *attribute = materialObject->FindAttribute(kAttributeRefraction);
			const ColorRGBA& color = (attribute) ? static_cast<const RefractionAttribute *>(attribute)->GetRefractionColor() : K::white;

			const char *title = table->GetString(StringID('MATL', 'AMBT', 'REFR'));
			const char *picker = table->GetString(StringID('MATL', 'PICK', 'REFR'));
			return (new CheckColorSetting('REFR', (attribute != nullptr), color, title, picker));
		}

		if (index == 13)
		{
			const Attribute *attribute = materialObject->FindAttribute(kAttributeRefraction);
			float scale = (attribute) ? static_cast<const RefractionAttribute *>(attribute)->GetRefractionParams()->refractionOffsetScale : 1.0F;

			const char *title = table->GetString(StringID('MATL', 'AMBT', 'RFRO'));
			return (new TextSetting('RFRO', scale, title));
		}

		if (index == 14)
		{
			return (GetTextureMapSetting('AMBT', kMaterialOpacityTexture));
		}

		if (index == 15)
		{
			return (GetTexcoordInputSetting('AMBT', kMaterialOpacityTexture));
		}
	}
	else if (category == 'FLAG')
	{
		if (index == 0)
		{
			const char *title = table->GetString(StringID('MATL', 'FLAG', 'HFLG'));
			return (new HeadingSetting('HFLG', title));
		}

		if (index == 1)
		{
			const char *title = table->GetString(StringID('MATL', 'FLAG', 'TSID'));
			return (new BooleanSetting('TSID', ((materialObject->GetMaterialFlags() & kMaterialTwoSided) != 0), title));
		}

		if (index == 2)
		{
			const char *title = table->GetString(StringID('MATL', 'FLAG', 'ATST'));
			return (new BooleanSetting('ATST', ((materialObject->GetMaterialFlags() & kMaterialAlphaTest) != 0), title));
		}

		if (index == 3)
		{
			const char *title = table->GetString(StringID('MATL', 'FLAG', 'ACOV'));
			return (new BooleanSetting('ACOV', ((materialObject->GetMaterialFlags() & kMaterialAlphaCoverage) != 0), title));
		}

		if (index == 4)
		{
			const char *title = table->GetString(StringID('MATL', 'FLAG', 'SSHD'));
			return (new BooleanSetting('SSHD', ((materialObject->GetMaterialFlags() & kMaterialSampleShading) != 0), title));
		}

		if (index == 5)
		{
			const char *title = table->GetString(StringID('MATL', 'FLAG', 'GLOW'));
			return (new BooleanSetting('GLOW', ((materialObject->GetMaterialFlags() & kMaterialEmissionGlow) != 0), title));
		}

		if (index == 6)
		{
			const char *title = table->GetString(StringID('MATL', 'FLAG', 'BLOM'));
			return (new BooleanSetting('BLOM', ((materialObject->GetMaterialFlags() & kMaterialSpecularBloom) != 0), title));
		}

		if (index == 7)
		{
			const char *title = table->GetString(StringID('MATL', 'FLAG', 'VOCC'));
			return (new BooleanSetting('VOCC', ((materialObject->GetMaterialFlags() & kMaterialVertexOcclusion) != 0), title));
		}

		if (index == 8)
		{
			const char *title = table->GetString(StringID('MATL', 'FLAG', 'SSAO'));
			return (new BooleanSetting('SSAO', ((materialObject->GetMaterialFlags() & kMaterialOcclusionInhibit) == 0), title));
		}

		if (index == 9)
		{
			const char *title = table->GetString(StringID('MATL', 'FLAG', 'RDSY'));
			return (new BooleanSetting('RDSY', ((materialObject->GetMaterialFlags() & kMaterialRadiositySpaceInhibit) == 0), title));
		}

		if (!materialObject->ShaderMaterial())
		{
			if (index == 10)
			{
				const char *title = table->GetString(StringID('MATL', 'FLAG', 'HMUT'));
				return (new HeadingSetting('HMUT', title));
			}

			if (index == 11)
			{
				const Attribute *attribute = materialObject->FindAttribute(kAttributeDiffuse);
				const char *title = table->GetString(StringID('MATL', 'FLAG', 'DIFF'));
				return (new BooleanSetting('MDIF', ((attribute) && ((attribute->GetAttributeFlags() & kAttributeMutable) != 0)), title));
			}

			if (index == 12)
			{
				const Attribute *attribute = materialObject->FindAttribute(kAttributeSpecular);
				const char *title = table->GetString(StringID('MATL', 'FLAG', 'SPEC'));
				return (new BooleanSetting('MSPC', ((attribute) && ((attribute->GetAttributeFlags() & kAttributeMutable) != 0)), title));
			}

			if (index == 13)
			{
				const Attribute *attribute = materialObject->FindAttribute(kAttributeMicrofacet);
				const char *title = table->GetString(StringID('MATL', 'FLAG', 'MFCT'));
				return (new BooleanSetting('MMFC', ((attribute) && ((attribute->GetAttributeFlags() & kAttributeMutable) != 0)), title));
			}

			if (index == 14)
			{
				const Attribute *attribute = materialObject->FindAttribute(kAttributeEmission);
				const char *title = table->GetString(StringID('MATL', 'FLAG', 'EMIS'));
				return (new BooleanSetting('MEMS', ((attribute) && ((attribute->GetAttributeFlags() & kAttributeMutable) != 0)), title));
			}

			if (index == 15)
			{
				const Attribute *attribute = materialObject->FindAttribute(kAttributeEnvironment);
				const char *title = table->GetString(StringID('MATL', 'FLAG', 'ENVC'));
				return (new BooleanSetting('MENV', ((attribute) && ((attribute->GetAttributeFlags() & kAttributeMutable) != 0)), title));
			}

			if (index == 16)
			{
				const Attribute *attribute = materialObject->FindAttribute(kAttributeReflection);
				const char *title = table->GetString(StringID('MATL', 'FLAG', 'REFL'));
				return (new BooleanSetting('MRFL', ((attribute) && ((attribute->GetAttributeFlags() & kAttributeMutable) != 0)), title));
			}

			if (index == 17)
			{
				const Attribute *attribute = materialObject->FindAttribute(kAttributeRefraction);
				const char *title = table->GetString(StringID('MATL', 'FLAG', 'REFR'));
				return (new BooleanSetting('MRFR', ((attribute) && ((attribute->GetAttributeFlags() & kAttributeMutable) != 0)), title));
			}
		}

		if (index == 18)
		{
			const char *title = table->GetString(StringID('MATL', 'FLAG', 'HSUB'));
			return (new HeadingSetting('HSUB', title));
		}

		if (index == 19)
		{
			int32 count = 1;
			int32 selection = 0;
			SubstanceType materialSubstance = materialObject->GetMaterialSubstance();

			const Substance *substance = MaterialObject::GetFirstRegisteredSubstance();
			while (substance)
			{
				if (substance->GetSubstanceType() == materialSubstance)
				{
					selection = count;
				}

				count++;
				substance = substance->Next();
			}

			const char *title = table->GetString(StringID('MATL', 'FLAG', 'SBST'));
			MenuSetting *menu = new MenuSetting('SBST', selection, title, count);
			menu->SetMenuItemString(0, table->GetString(StringID('MATL', 'FLAG', 'SBST', 'NONE')));

			substance = MaterialObject::GetFirstRegisteredSubstance();
			for (machine a = 1; a < count; a++)
			{
				menu->SetMenuItemString(a, substance->GetSubstanceName());
				substance = substance->Next();
			}

			return (menu);
		}
	}
	else if (category == 'TEXC')
	{
		if (index == 0)
		{
			const char *title = table->GetString(StringID('MATL', 'TEXC', 'HTX1'));
			return (new HeadingSetting('HTX1', title));
		}

		if (index == 1)
		{
			const char *title = table->GetString(StringID('MATL', 'TEXC', 'SSCL'));
			return (new TextSetting('SSC1', materialObject->GetTexcoordScale(0).x, title));
		}

		if (index == 2)
		{
			const char *title = table->GetString(StringID('MATL', 'TEXC', 'SOFF'));
			return (new TextSetting('SOF1', materialObject->GetTexcoordOffset(0).x, title));
		}

		if (index == 3)
		{
			const char *title = table->GetString(StringID('MATL', 'TEXC', 'TSCL'));
			return (new TextSetting('TSC1', materialObject->GetTexcoordScale(0).y, title));
		}

		if (index == 4)
		{
			const char *title = table->GetString(StringID('MATL', 'TEXC', 'TOFF'));
			return (new TextSetting('TOF1', materialObject->GetTexcoordOffset(0).y, title));
		}

		if (index == 5)
		{
			const char *title = table->GetString(StringID('MATL', 'TEXC', 'ANIM'));
			return (new BooleanSetting('ANM1', ((materialObject->GetMaterialFlags() & kMaterialAnimateTexcoord0) != 0), title));
		}

		if (index == 6)
		{
			const Vector2D& velocity = materialObject->GetTexcoordVelocity(0);

			const char *title = table->GetString(StringID('MATL', 'TEXC', 'DRCT'));
			float angle = Atan(velocity.y, velocity.x) * K::degrees + 180.5F;
			return (new TextSetting('DRC1', Floor(angle), title));
		}

		if (index == 7)
		{
			const Vector2D& velocity = materialObject->GetTexcoordVelocity(0);

			const char *title = table->GetString(StringID('MATL', 'TEXC', 'SPED'));
			return (new IntegerSetting('SPD1', Max((int32) (Magnitude(velocity) / kTexcoordSpeedFactor + 0.5F), 1), title, 1, 360, 1));
		}

		if (index == 8)
		{
			const char *title = table->GetString(StringID('MATL', 'TEXC', 'HTX2'));
			return (new HeadingSetting('HTX2', title));
		}

		if (index == 9)
		{
			const char *title = table->GetString(StringID('MATL', 'TEXC', 'SSCL'));
			return (new TextSetting('SSC2', materialObject->GetTexcoordScale(1).x, title));
		}

		if (index == 10)
		{
			const char *title = table->GetString(StringID('MATL', 'TEXC', 'SOFF'));
			return (new TextSetting('SOF2', materialObject->GetTexcoordOffset(1).x, title));
		}

		if (index == 11)
		{
			const char *title = table->GetString(StringID('MATL', 'TEXC', 'TSCL'));
			return (new TextSetting('TSC2', materialObject->GetTexcoordScale(1).y, title));
		}

		if (index == 12)
		{
			const char *title = table->GetString(StringID('MATL', 'TEXC', 'TOFF'));
			return (new TextSetting('TOF2', materialObject->GetTexcoordOffset(1).y, title));
		}

		if (index == 13)
		{
			const char *title = table->GetString(StringID('MATL', 'TEXC', 'ANIM'));
			return (new BooleanSetting('ANM2', ((materialObject->GetMaterialFlags() & kMaterialAnimateTexcoord1) != 0), title));
		}

		if (index == 14)
		{
			const Vector2D& velocity = materialObject->GetTexcoordVelocity(1);

			const char *title = table->GetString(StringID('MATL', 'TEXC', 'DRCT'));
			float angle = Atan(velocity.y, velocity.x) * K::degrees + 180.5F;
			return (new TextSetting('DRC2', Floor(angle), title));
		}

		if (index == 15)
		{
			const Vector2D& velocity = materialObject->GetTexcoordVelocity(1);

			const char *title = table->GetString(StringID('MATL', 'TEXC', 'SPED'));
			return (new IntegerSetting('SPD2', Max((int32) (Magnitude(velocity) / kTexcoordSpeedFactor + 0.5F), 1), title, 1, 360, 1));
		}

		if (index == 16)
		{
			const char *title = table->GetString(StringID('MATL', 'TEXC', 'HTWT'));
			return (new HeadingSetting('HTER', title));
		}

		if (index == 17)
		{
			const char *title = table->GetString(StringID('MATL', 'TEXC', 'TWSC'));
			return (new TextSetting('TWSC', materialObject->GetTexcoordGeneration().x, title));
		}
	}

	return (nullptr);
}

void MaterialContainer::SetCategorySetting(Type category, const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (category == 'DIFF')
	{
		if (identifier == 'DIFF')
		{
			const CheckColorSetting *checkColorSetting = static_cast<const CheckColorSetting *>(setting);
			if (checkColorSetting->GetCheckValue())
			{
				const ColorRGBA& color = checkColorSetting->GetColor();
				materialObject->AddAttribute(new DiffuseAttribute(color));
			}
		}
		else if (identifier == 'TX1M')
		{
			primaryDiffuseTexture = SetTextureMapSetting(setting, kAttributeDiffuseTexture);
		}
		else if (identifier == 'TX2M')
		{
			SetTextureMapSetting(setting, kAttributeDiffuseTexture);
		}
		else if (identifier == 'TX1I')
		{
			SetTexcoordInputSetting(setting, kAttributeDiffuseTexture, 0);
		}
		else if (identifier == 'TX2I')
		{
			SetTexcoordInputSetting(setting, kAttributeDiffuseTexture, primaryDiffuseTexture);
		}
		else if (identifier == 'BLND')
		{
			TextureBlendMode blendMode = kTextureBlendAdd;
			switch (static_cast<const MenuSetting *>(setting)->GetMenuSelection())
			{
				case 1:

					blendMode = kTextureBlendAverage;
					break;

				case 2:

					blendMode = kTextureBlendMultiply;
					break;

				case 3:

					blendMode = kTextureBlendVertexAlpha;
					break;

				case 4:

					blendMode = kTextureBlendPrimaryAlpha;
					break;

				case 5:

					blendMode = kTextureBlendSecondaryAlpha;
					break;

				case 6:

					blendMode = kTextureBlendPrimaryInverseAlpha;
					break;

				case 7:

					blendMode = kTextureBlendSecondaryInverseAlpha;
					break;
			}

			materialObject->SetTextureBlendMode(blendMode);
		}
		else if (identifier == 'NM1M')
		{
			primaryNormalTexture = SetTextureMapSetting(setting, kAttributeNormalTexture);
		}
		else if (identifier == 'NM2M')
		{
			SetTextureMapSetting(setting, kAttributeNormalTexture);
		}
		else if (identifier == 'NM1I')
		{
			SetTexcoordInputSetting(setting, kAttributeNormalTexture, 0);
		}
		else if (identifier == 'NM2I')
		{
			SetTexcoordInputSetting(setting, kAttributeNormalTexture, primaryNormalTexture);
		}
		else if (identifier == 'HRZN')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				const NormalTextureAttribute *normalTexture = static_cast<NormalTextureAttribute *>(materialObject->FindAttribute(kAttributeNormalTexture));
				if (normalTexture)
				{
					ResourceName name(normalTexture->GetTextureName());

					int32 length = name.Length();
					if ((length > 5) && (Text::CompareText(&name[length - 5], "-nrml")))
					{
						name[length - 4] = 'h';
						name[length - 3] = 'r';
						name[length - 2] = 'z';
						name[length - 1] = 'n';
					}
					else
					{
						name += "-h";
					}

					HorizonTextureAttribute *horizonTexture = new HorizonTextureAttribute(name);
					materialObject->AddAttribute(horizonTexture);
					horizonTexture->SetTexcoordIndex(normalTexture->GetTexcoordIndex());
				}
			}
		}
		else if (identifier == 'XINF')
		{
			HorizonTextureAttribute *horizonTexture = static_cast<HorizonTextureAttribute *>(materialObject->FindAttribute(kAttributeHorizonTexture));
			if (horizonTexture)
			{
				if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
				{
					horizonTexture->SetHorizonFlags(horizonTexture->GetHorizonFlags() | kHorizonExcludeInfiniteLight);
				}
				else
				{
					horizonTexture->SetHorizonFlags(horizonTexture->GetHorizonFlags() & ~kHorizonExcludeInfiniteLight);
				}
			}
		}
		else if (identifier == 'XPNT')
		{
			HorizonTextureAttribute *horizonTexture = static_cast<HorizonTextureAttribute *>(materialObject->FindAttribute(kAttributeHorizonTexture));
			if (horizonTexture)
			{
				if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
				{
					horizonTexture->SetHorizonFlags(horizonTexture->GetHorizonFlags() | kHorizonExcludePointLight);
				}
				else
				{
					horizonTexture->SetHorizonFlags(horizonTexture->GetHorizonFlags() & ~kHorizonExcludePointLight);
				}
			}
		}
	}
	else if (category == 'SPEC')
	{
		if (identifier == 'SPEC')
		{
			const CheckColorSetting *checkColorSetting = static_cast<const CheckColorSetting *>(setting);
			if (checkColorSetting->GetCheckValue())
			{
				const ColorRGBA& color = checkColorSetting->GetColor();
				materialObject->AddAttribute(new SpecularAttribute(color, 50.0F));
			}
		}
		else if (identifier == 'EXPN')
		{
			Attribute *attribute = materialObject->FindAttribute(kAttributeSpecular);
			if (attribute)
			{
				float exponent = (float) static_cast<const IntegerSetting *>(setting)->GetIntegerValue();
				static_cast<SpecularAttribute *>(attribute)->SetSpecularExponent(exponent);
			}
		}
		else if (identifier == 'SPCM')
		{
			SetTextureMapSetting(setting, kAttributeSpecularTexture);
		}
		else if (identifier == 'SPCI')
		{
			SetTexcoordInputSetting(setting, kAttributeSpecularTexture, 0);
		}
		else if (identifier == 'MFCT')
		{
			const CheckColorSetting *checkColorSetting = static_cast<const CheckColorSetting *>(setting);
			if (checkColorSetting->GetCheckValue())
			{
				const ColorRGBA& color = checkColorSetting->GetColor();
				materialObject->AddAttribute(new MicrofacetAttribute(color, Vector2D(0.5F, 0.5F), 1.0F));
			}
		}
		else if (identifier == 'MRFL')
		{
			Attribute *attribute = materialObject->FindAttribute(kAttributeMicrofacet);
			if (attribute)
			{
				float reflectivity = static_cast<const FloatSetting *>(setting)->GetFloatValue();
				static_cast<MicrofacetAttribute *>(attribute)->SetMicrofacetReflectivity(reflectivity);
			}
		}
		else if (identifier == 'SLPX')
		{
			Attribute *attribute = materialObject->FindAttribute(kAttributeMicrofacet);
			if (attribute)
			{
				MicrofacetAttribute *microfacetAttribute = static_cast<MicrofacetAttribute *>(attribute);
				Vector2D slope = microfacetAttribute->GetMicrofacetParams()->microfacetSlope;
				slope.x = static_cast<const FloatSetting *>(setting)->GetFloatValue();
				microfacetAttribute->SetMicrofacetSlope(slope);
			}
		}
		else if (identifier == 'SLPY')
		{
			Attribute *attribute = materialObject->FindAttribute(kAttributeMicrofacet);
			if (attribute)
			{
				MicrofacetAttribute *microfacetAttribute = static_cast<MicrofacetAttribute *>(attribute);
				Vector2D slope = microfacetAttribute->GetMicrofacetParams()->microfacetSlope;
				slope.y = static_cast<const FloatSetting *>(setting)->GetFloatValue();
				microfacetAttribute->SetMicrofacetSlope(slope);
			}
		}
	}
	else if (category == 'AMBT')
	{
		if (identifier == 'EMIS')
		{
			const CheckColorSetting *checkColorSetting = static_cast<const CheckColorSetting *>(setting);
			if (checkColorSetting->GetCheckValue())
			{
				const ColorRGBA& color = checkColorSetting->GetColor();
				materialObject->AddAttribute(new EmissionAttribute(color));
			}
		}
		else if (identifier == 'EMSM')
		{
			SetTextureMapSetting(setting, kAttributeEmissionTexture);
		}
		else if (identifier == 'EMSI')
		{
			SetTexcoordInputSetting(setting, kAttributeEmissionTexture, 0);
		}
		else if (identifier == 'ENVC')
		{
			const CheckColorSetting *checkColorSetting = static_cast<const CheckColorSetting *>(setting);
			if (checkColorSetting->GetCheckValue())
			{
				const ColorRGBA& color = checkColorSetting->GetColor();
				materialObject->AddAttribute(new EnvironmentAttribute(color));
			}
		}
		else if (identifier == 'ENVM')
		{
			SetTextureMapSetting(setting, kAttributeEnvironmentTexture);
		}
		else if (identifier == 'REFL')
		{
			const CheckColorSetting *checkColorSetting = static_cast<const CheckColorSetting *>(setting);
			if (checkColorSetting->GetCheckValue())
			{
				const ColorRGBA& color = checkColorSetting->GetColor();
				materialObject->AddAttribute(new ReflectionAttribute(color, 1.0F, 1.0F));
			}
		}
		else if (identifier == 'NINC')
		{
			Attribute *attribute = materialObject->FindAttribute(kAttributeReflection);
			if (attribute)
			{
				float reflectivity = (float) static_cast<const IntegerSetting *>(setting)->GetIntegerValue() * 0.01F;
				static_cast<ReflectionAttribute *>(attribute)->SetNormalIncidenceReflectivity(reflectivity);
			}
		}
		else if (identifier == 'RFLO')
		{
			Attribute *attribute = materialObject->FindAttribute(kAttributeReflection);
			if (attribute)
			{
				float scale = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
				static_cast<ReflectionAttribute *>(attribute)->SetReflectionOffsetScale(scale);
			}
		}
		else if (identifier == 'REFR')
		{
			const CheckColorSetting *checkColorSetting = static_cast<const CheckColorSetting *>(setting);
			if (checkColorSetting->GetCheckValue())
			{
				const ColorRGBA& color = checkColorSetting->GetColor();
				materialObject->AddAttribute(new RefractionAttribute(color, 1.0F));
			}
		}
		else if (identifier == 'RFRO')
		{
			Attribute *attribute = materialObject->FindAttribute(kAttributeRefraction);
			if (attribute)
			{
				float scale = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
				static_cast<RefractionAttribute *>(attribute)->SetRefractionOffsetScale(scale);
			}
		}
		else if (identifier == 'OPCM')
		{
			SetTextureMapSetting(setting, kAttributeOpacityTexture);
		}
		else if (identifier == 'OPCI')
		{
			SetTexcoordInputSetting(setting, kAttributeOpacityTexture, 0);
		}
	}
	else if (category == 'FLAG')
	{
		if (identifier == 'TSID')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				materialObject->SetMaterialFlags(materialObject->GetMaterialFlags() | kMaterialTwoSided);
			}
			else
			{
				materialObject->SetMaterialFlags(materialObject->GetMaterialFlags() & ~kMaterialTwoSided);
			}
		}
		else if (identifier == 'ATST')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				materialObject->SetMaterialFlags(materialObject->GetMaterialFlags() | kMaterialAlphaTest);
			}
			else
			{
				materialObject->SetMaterialFlags(materialObject->GetMaterialFlags() & ~kMaterialAlphaTest);
			}
		}
		else if (identifier == 'ACOV')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				materialObject->SetMaterialFlags(materialObject->GetMaterialFlags() | kMaterialAlphaCoverage);
			}
			else
			{
				materialObject->SetMaterialFlags(materialObject->GetMaterialFlags() & ~kMaterialAlphaCoverage);
			}
		}
		else if (identifier == 'SSHD')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				materialObject->SetMaterialFlags(materialObject->GetMaterialFlags() | kMaterialSampleShading);
			}
			else
			{
				materialObject->SetMaterialFlags(materialObject->GetMaterialFlags() & ~kMaterialSampleShading);
			}
		}
		else if (identifier == 'GLOW')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				materialObject->SetMaterialFlags(materialObject->GetMaterialFlags() | kMaterialEmissionGlow);
			}
			else
			{
				materialObject->SetMaterialFlags(materialObject->GetMaterialFlags() & ~kMaterialEmissionGlow);
			}
		}
		else if (identifier == 'BLOM')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				materialObject->SetMaterialFlags(materialObject->GetMaterialFlags() | kMaterialSpecularBloom);
			}
			else
			{
				materialObject->SetMaterialFlags(materialObject->GetMaterialFlags() & ~kMaterialSpecularBloom);
			}
		}
		else if (identifier == 'VOCC')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				materialObject->SetMaterialFlags(materialObject->GetMaterialFlags() | kMaterialVertexOcclusion);
			}
			else
			{
				materialObject->SetMaterialFlags(materialObject->GetMaterialFlags() & ~kMaterialVertexOcclusion);
			}
		}
		else if (identifier == 'SSAO')
		{
			if (!static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				materialObject->SetMaterialFlags(materialObject->GetMaterialFlags() | kMaterialOcclusionInhibit);
			}
			else
			{
				materialObject->SetMaterialFlags(materialObject->GetMaterialFlags() & ~kMaterialOcclusionInhibit);
			}
		}
		else if (identifier == 'RDSY')
		{
			if (!static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				materialObject->SetMaterialFlags(materialObject->GetMaterialFlags() | kMaterialRadiositySpaceInhibit);
			}
			else
			{
				materialObject->SetMaterialFlags(materialObject->GetMaterialFlags() & ~kMaterialRadiositySpaceInhibit);
			}
		}
		else if (identifier == 'MDIF')
		{
			Attribute *attribute = materialObject->FindAttribute(kAttributeDiffuse);
			if (attribute)
			{
				if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
				{
					attribute->SetAttributeFlags(attribute->GetAttributeFlags() | kAttributeMutable);
				}
				else
				{
					attribute->SetAttributeFlags(attribute->GetAttributeFlags() & ~kAttributeMutable);
				}
			}
		}
		else if (identifier == 'MSPC')
		{
			Attribute *attribute = materialObject->FindAttribute(kAttributeSpecular);
			if (attribute)
			{
				if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
				{
					attribute->SetAttributeFlags(attribute->GetAttributeFlags() | kAttributeMutable);
				}
				else
				{
					attribute->SetAttributeFlags(attribute->GetAttributeFlags() & ~kAttributeMutable);
				}
			}
		}
		else if (identifier == 'MMFC')
		{
			Attribute *attribute = materialObject->FindAttribute(kAttributeMicrofacet);
			if (attribute)
			{
				if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
				{
					attribute->SetAttributeFlags(attribute->GetAttributeFlags() | kAttributeMutable);
				}
				else
				{
					attribute->SetAttributeFlags(attribute->GetAttributeFlags() & ~kAttributeMutable);
				}
			}
		}
		else if (identifier == 'MEMS')
		{
			Attribute *attribute = materialObject->FindAttribute(kAttributeEmission);
			if (attribute)
			{
				if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
				{
					attribute->SetAttributeFlags(attribute->GetAttributeFlags() | kAttributeMutable);
				}
				else
				{
					attribute->SetAttributeFlags(attribute->GetAttributeFlags() & ~kAttributeMutable);
				}
			}
		}
		else if (identifier == 'MENV')
		{
			Attribute *attribute = materialObject->FindAttribute(kAttributeEnvironment);
			if (attribute)
			{
				if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
				{
					attribute->SetAttributeFlags(attribute->GetAttributeFlags() | kAttributeMutable);
				}
				else
				{
					attribute->SetAttributeFlags(attribute->GetAttributeFlags() & ~kAttributeMutable);
				}
			}
		}
		else if (identifier == 'MRFL')
		{
			Attribute *attribute = materialObject->FindAttribute(kAttributeReflection);
			if (attribute)
			{
				if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
				{
					attribute->SetAttributeFlags(attribute->GetAttributeFlags() | kAttributeMutable);
				}
				else
				{
					attribute->SetAttributeFlags(attribute->GetAttributeFlags() & ~kAttributeMutable);
				}
			}
		}
		else if (identifier == 'MRFR')
		{
			Attribute *attribute = materialObject->FindAttribute(kAttributeRefraction);
			if (attribute)
			{
				if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
				{
					attribute->SetAttributeFlags(attribute->GetAttributeFlags() | kAttributeMutable);
				}
				else
				{
					attribute->SetAttributeFlags(attribute->GetAttributeFlags() & ~kAttributeMutable);
				}
			}
		}
		else if (identifier == 'SBST')
		{
			int32 selection = static_cast<const MenuSetting *>(setting)->GetMenuSelection();
			if (selection == 0)
			{
				materialObject->SetMaterialSubstance(kSubstanceNone);
			}
			else
			{
				const Substance *substance = MaterialObject::GetFirstRegisteredSubstance();
				while (substance)
				{
					if (--selection == 0)
					{
						materialObject->SetMaterialSubstance(substance->GetSubstanceType());
						break;
					}

					substance = substance->Next();
				}
			}
		}
	}
	else if (category == 'TEXC')
	{
		if ((identifier == 'SSC1') || (identifier == 'SSC2'))
		{
			int32 index = identifier - 'SSC1';
			Vector2D scale = materialObject->GetTexcoordScale(index);
			scale.x = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
			materialObject->SetTexcoordScale(index, scale);
		}
		else if ((identifier == 'SOF1') || (identifier == 'SOF2'))
		{
			int32 index = identifier - 'SOF1';
			Vector2D offset = materialObject->GetTexcoordOffset(index);
			offset.x = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
			materialObject->SetTexcoordOffset(index, offset);
		}
		else if ((identifier == 'TSC1') || (identifier == 'TSC2'))
		{
			int32 index = identifier - 'TSC1';
			Vector2D scale = materialObject->GetTexcoordScale(index);
			scale.y = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
			materialObject->SetTexcoordScale(index, scale);
		}
		else if ((identifier == 'TOF1') || (identifier == 'TOF2'))
		{
			int32 index = identifier - 'TOF1';
			Vector2D offset = materialObject->GetTexcoordOffset(index);
			offset.y = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
			materialObject->SetTexcoordOffset(index, offset);
		}
		else if ((identifier == 'ANM1') || (identifier == 'ANM2'))
		{
			int32 index = identifier - 'ANM1';
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				materialObject->SetMaterialFlags(materialObject->GetMaterialFlags() | (kMaterialAnimateTexcoord0 << index));
			}
			else
			{
				materialObject->SetMaterialFlags(materialObject->GetMaterialFlags() & ~(kMaterialAnimateTexcoord0 << index));
			}
		}
		else if ((identifier == 'DRC1') || (identifier == 'DRC2'))
		{
			int32 index = identifier - 'DRC1';
			int32 angle = ((int32) (Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText()) * (128.0F / 180.0F)) + 2) & 0xFC;
			materialObject->SetTexcoordVelocity(index, -Math::GetTrigTable()[angle]);
		}
		else if ((identifier == 'SPD1') || (identifier == 'SPD2'))
		{
			int32 index = identifier - 'SPD1';
			float speed = (float) static_cast<const IntegerSetting *>(setting)->GetIntegerValue();
			const Vector2D& velocity = materialObject->GetTexcoordVelocity(index);
			float sx = Floor(speed * Fabs(velocity.x) + 0.5F) * NonzeroFsgn(velocity.x);
			float sy = Floor(speed * Fabs(velocity.y) + 0.5F) * NonzeroFsgn(velocity.y);
			materialObject->SetTexcoordVelocity(index, Vector2D(sx * kTexcoordSpeedFactor, sy * kTexcoordSpeedFactor));
		}
		else if (identifier == 'TWSC')
		{
			float scale = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
			materialObject->SetTexcoordGeneration(Vector4D(scale, scale, 0.0F, 0.0F));
		}
	}
}

void MaterialContainer::SetMaterialObject(MaterialObject *object)
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

// ZYUQURM
