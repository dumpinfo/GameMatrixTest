 

#include "C4LightObjects.h"
#include "C4Configuration.h"


using namespace C4;


LightObject::LightObject(LightType type, LightType base) : Object(kObjectLight)
{
	lightType = type;
	baseLightType = base;

	minDetailLevel = 0;
}

LightObject::LightObject(LightType type, LightType base, const ColorRGB& color) : Object(kObjectLight)
{
	lightType = type;
	baseLightType = base;

	lightColor = color;
	lightFlags = 0;
	minDetailLevel = 0;
}

LightObject::LightObject(const LightObject& lightObject) : Object(kObjectLight)
{
	lightType = lightObject.lightType;
	baseLightType = lightObject.baseLightType;

	lightColor = lightObject.lightColor;
	lightFlags = lightObject.lightFlags;
	minDetailLevel = lightObject.minDetailLevel;
}

LightObject::~LightObject()
{
}

LightObject *LightObject::Create(Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (data.GetType())
	{
		case kLightInfinite:

		#if C4LEGACY

			case 'DPTH':

		#endif

			return (new InfiniteLightObject);

		#if C4LEGACY

			case 'LAND':
			{
				InfiniteLightObject *object = new InfiniteLightObject;
				object->GetCascadeRangeArray()[0].min = -1.0F;
				return (object);
			}

		#endif

		case kLightPoint:

			return (new PointLightObject);

		case kLightCube:

			return (new CubeLightObject);

		case kLightSpot:

			return (new SpotLightObject);
	}

	return (nullptr);
}

void LightObject::PackType(Packer& data) const
{
	Object::PackType(data);
	data << lightType;
}

void LightObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	data << ChunkHeader('COLR', sizeof(ColorRGB));
	data << lightColor;

	data << ChunkHeader('FLAG', 4);
	data << lightFlags;

	data << ChunkHeader('MDET', 4);
	data << minDetailLevel;

	data << TerminatorChunk;
}

void LightObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags) 
{
	UnpackChunkList<LightObject>(data, unpackFlags);
} 

bool LightObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags) 
{
	switch (chunkHeader->chunkType)
	{ 
		case 'COLR':
 
			data >> lightColor; 
			return (true);

		case 'FLAG':
 
			data >> lightFlags;
			return (true);

		case 'MDET':

			data >> minDetailLevel;
			return (true);
	}

	return (false);
}

int32 LightObject::GetCategoryCount(void) const
{
	return (1);
}

Type LightObject::GetCategoryType(int32 index, const char **title) const
{
	if (index == 0)
	{
		*title = TheInterfaceMgr->GetStringTable()->GetString(StringID(kObjectLight));
		return (kObjectLight);
	}

	return (0);
}

int32 LightObject::GetCategorySettingCount(Type category) const
{
	if (category == kObjectLight)
	{
		return (9);
	}

	return (0);
}

Setting *LightObject::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kObjectLight)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();

		if (index == 0)
		{
			const char *title = table->GetString(StringID(kObjectLight, 'ILMN'));
			return (new HeadingSetting('ILMN', title));
		}

		if (index == 1)
		{
			const char *title = table->GetString(StringID(kObjectLight, 'ILMN', 'COLR'));
			const char *picker = table->GetString(StringID(kObjectLight, 'ILMN', 'PICK'));

			ColorRGB color = lightColor;
			float bright = Fmax(color.red, color.green, color.blue);
			if (bright > 1.0F)
			{
				color /= bright;
			}

			return (new ColorSetting('COLR', color, title, picker));
		}

		if (index == 2)
		{
			if (flags & kConfigurationScript)
			{
				return (nullptr);
			}

			const char *title = table->GetString(StringID(kObjectLight, 'ILMN', 'SCAL'));
			float bright = Fmax(lightColor.red, lightColor.green, lightColor.blue);
			return (new TextSetting('SCAL', Fmax(bright, 1.0F), title));
		}

		if (index == 3)
		{
			if (flags & kConfigurationScript)
			{
				return (nullptr);
			}

			const char *title = table->GetString(StringID(kObjectLight, 'ILMN', 'MDET'));
			return (new IntegerSetting('MDET', minDetailLevel, title, 0, 3, 1));
		}

		if (index == 4)
		{
			const char *title = table->GetString(StringID(kObjectLight, 'FLAG'));
			return (new HeadingSetting('FLAG', title));
		}

		if (index == 5)
		{
			if (flags & kConfigurationScript)
			{
				return (nullptr);
			}

			const char *title = table->GetString(StringID(kObjectLight, 'FLAG', 'STAT'));
			return (new BooleanSetting('STAT', ((lightFlags & kLightStatic) != 0), title));
		}

		if (index == 6)
		{
			const char *title = table->GetString(StringID(kObjectLight, 'FLAG', 'SHAD'));
			return (new BooleanSetting('SHAD', ((lightFlags & kLightShadowInhibit) == 0), title));
		}

		if (index == 7)
		{
			const char *title = table->GetString(StringID(kObjectLight, 'FLAG', 'PORT'));
			return (new BooleanSetting('PORT', ((lightFlags & kLightPortalInhibit) == 0), title));
		}

		if (index == 8)
		{
			if (flags & kConfigurationScript)
			{
				return (nullptr);
			}

			const char *title = table->GetString(StringID(kObjectLight, 'FLAG', 'ISPC'));
			return (new BooleanSetting('ISPC', ((lightFlags & kLightInstanceShadowSpace) != 0), title));
		}
	}

	return (nullptr);
}

void LightObject::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kObjectLight)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'COLR')
		{
			lightColor = static_cast<const ColorSetting *>(setting)->GetColor();
		}
		else if (identifier == 'SCAL')
		{
			lightColor *= Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
		}
		else if (identifier == 'MDET')
		{
			minDetailLevel = static_cast<const IntegerSetting *>(setting)->GetIntegerValue();
		}
		else if (identifier == 'STAT')
		{
			unsigned_int32 flags = lightFlags & ~kLightConfined;
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				lightFlags = flags | kLightStatic;
			}
			else
			{
				lightFlags = flags & ~kLightStatic;
			}
		}
		else if (identifier == 'SHAD')
		{
			if (!static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				lightFlags |= kLightShadowInhibit;
			}
			else
			{
				lightFlags &= ~kLightShadowInhibit;
			}
		}
		else if (identifier == 'PORT')
		{
			if (!static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				lightFlags |= kLightPortalInhibit;
			}
			else
			{
				lightFlags &= ~kLightPortalInhibit;
			}
		}
		else if (identifier == 'ISPC')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				lightFlags |= kLightInstanceShadowSpace;
			}
			else
			{
				lightFlags &= ~kLightInstanceShadowSpace;
			}
		}
	}
}


InfiniteLightObject::InfiniteLightObject() : LightObject(kLightInfinite, kLightInfinite)
{
	#if C4LEGACY

		cascadeRange[0].min = 0.0F;		// A negative value means this was converted from an old landscape light.

	#endif
}

InfiniteLightObject::InfiniteLightObject(const ColorRGB& color) : LightObject(kLightInfinite, kLightInfinite, color)
{
	SetLightFlags(GetLightFlags() | kLightUnified);

	cascadeRange[0].Set(0.0F, 20.0F);
	cascadeRange[1].Set(10.0F, 80.0F);
	cascadeRange[2].Set(60.0F, 250.0F);
	cascadeRange[3].Set(220.0F, 500.0F);
}

InfiniteLightObject::InfiniteLightObject(const InfiniteLightObject& infiniteLightObject) : LightObject(infiniteLightObject)
{
	for (machine a = 0; a < kMaxShadowCascadeCount; a++)
	{
		cascadeRange[a] = infiniteLightObject.cascadeRange[a];
	}
}

InfiniteLightObject::~InfiniteLightObject()
{
}

Object *InfiniteLightObject::Replicate(void) const
{
	return (new InfiniteLightObject(*this));
}

void InfiniteLightObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	LightObject::Pack(data, packFlags);

	for (machine a = 0; a < kMaxShadowCascadeCount; a++)
	{
		data << ChunkHeader('CSC1' + a, 8);
		data << cascadeRange[a];
	}

	data << TerminatorChunk;
}

void InfiniteLightObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	LightObject::Unpack(data, unpackFlags);

	#if C4LEGACY

		if (data.GetVersion() >= 61)
		{

	#endif

			UnpackChunkList<InfiniteLightObject>(data, unpackFlags);

	#if C4LEGACY

		}
		else if (cascadeRange[0].min < 0.0F)
		{
			SetLightFlags(GetLightFlags() & ~kLightShadowInhibit);
			UnpackChunkList<InfiniteLightObject>(data, unpackFlags);
		}
		else
		{
			cascadeRange[0].Set(0.0F, 20.0F);
			cascadeRange[1].Set(10.0F, 80.0F);
			cascadeRange[2].Set(60.0F, 250.0F);
			cascadeRange[3].Set(220.0F, 500.0F);
		}

	#endif
}

bool InfiniteLightObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	unsigned_int32 type = chunkHeader->chunkType;
	unsigned_int32 index = type - 'CSC1';
	if (index < kMaxShadowCascadeCount)
	{
		data >> cascadeRange[index];
		return (true);
	}

	return (false);
}

int32 InfiniteLightObject::GetCategorySettingCount(Type category) const
{
	int32 count = LightObject::GetCategorySettingCount(category);
	if (category == kObjectLight)
	{
		count += 11;
	}

	return (count);
}

Setting *InfiniteLightObject::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kObjectLight)
	{
		int32 i = index - LightObject::GetCategorySettingCount(kObjectLight);
		if (i >= 0)
		{
			if (flags & kConfigurationScript)
			{
				return (nullptr);
			}

			const StringTable *table = TheInterfaceMgr->GetStringTable();

			if (i == 0)
			{
				const char *title = table->GetString(StringID(kObjectLight, 'IFLG'));
				return (new HeadingSetting('IFLG', title));
			}

			if (i == 1)
			{
				if (flags & kConfigurationScript)
				{
					return (nullptr);
				}

				const char *title = table->GetString(StringID(kObjectLight, 'IFLG', 'UNFD'));
				return (new BooleanSetting('UNFD', ((GetLightFlags() & kLightUnified) != 0), title));
			}

			if (i == 2)
			{
				const char *title = table->GetString(StringID(kObjectLight, 'IFLG', 'ROOT'));
				return (new BooleanSetting('ROOT', ((GetLightFlags() & kLightRootZoneInhibit) != 0), title));
			}

			if (i == 3)
			{
				const char *title = table->GetString(StringID(kObjectLight, 'SHAD'));
				return (new HeadingSetting('SHST', title));
			}

			if (i == 4)
			{
				const char *title = table->GetString(StringID(kObjectLight, 'SHAD', 'CSC1'));
				return (new TextSetting('CSC1', cascadeRange[0].max, title));
			}

			if (i == 5)
			{
				const char *title = table->GetString(StringID(kObjectLight, 'SHAD', 'CSC2'));
				return (new TextSetting('CSC2', cascadeRange[1].max, title));
			}

			if (i == 6)
			{
				const char *title = table->GetString(StringID(kObjectLight, 'SHAD', 'CSC3'));
				return (new TextSetting('CSC3', cascadeRange[2].max, title));
			}

			if (i == 7)
			{
				const char *title = table->GetString(StringID(kObjectLight, 'SHAD', 'CSC4'));
				return (new TextSetting('CSC4', cascadeRange[3].max, title));
			}

			if (i == 8)
			{
				const char *title = table->GetString(StringID(kObjectLight, 'SHAD', 'TRN1'));
				return (new TextSetting('TRN1', cascadeRange[0].max - cascadeRange[1].min, title));
			}

			if (i == 9)
			{
				const char *title = table->GetString(StringID(kObjectLight, 'SHAD', 'TRN2'));
				return (new TextSetting('TRN2', cascadeRange[1].max - cascadeRange[2].min, title));
			}

			if (i == 10)
			{
				const char *title = table->GetString(StringID(kObjectLight, 'SHAD', 'TRN3'));
				return (new TextSetting('TRN3', cascadeRange[2].max - cascadeRange[3].min, title));
			}

			return (nullptr);
		}
	}

	return (LightObject::GetCategorySetting(category, index, flags));
}

void InfiniteLightObject::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kObjectLight)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'UNFD')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				SetLightFlags(GetLightFlags() | kLightUnified);
			}
			else
			{
				SetLightFlags(GetLightFlags() & ~kLightUnified);
			}
		}
		else if (identifier == 'ROOT')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				SetLightFlags(GetLightFlags() | kLightRootZoneInhibit);
			}
			else
			{
				SetLightFlags(GetLightFlags() & ~kLightRootZoneInhibit);
			}
		}
		else if (identifier == 'CSC1')
		{
			float value = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
			cascadeRange[0].max = Fmax(value, 2.0F);
		}
		else if (identifier == 'CSC2')
		{
			float value = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
			cascadeRange[1].max = Fmax(value, cascadeRange[0].max + 2.0F);
		}
		else if (identifier == 'CSC3')
		{
			float value = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
			cascadeRange[2].max = Fmax(value, cascadeRange[1].max + 2.0F);
		}
		else if (identifier == 'CSC4')
		{
			float value = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
			cascadeRange[3].max = Fmax(value, cascadeRange[2].max + 2.0F);
		}
		else if (identifier == 'TRN1')
		{
			float value = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
			cascadeRange[1].min = Fmax(cascadeRange[0].max - Fmax(value, 1.0F), cascadeRange[0].min);
		}
		else if (identifier == 'TRN2')
		{
			float value = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
			cascadeRange[2].min = Fmax(cascadeRange[1].max - Fmax(value, 1.0F), cascadeRange[1].min);
		}
		else if (identifier == 'TRN3')
		{
			float value = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
			cascadeRange[3].min = Fmax(cascadeRange[2].max - Fmax(value, 1.0F), cascadeRange[2].min);
		}
		else
		{
			LightObject::SetCategorySetting(kObjectLight, setting);
		}
	}
	else
	{
		LightObject::SetCategorySetting(category, setting);
	}
}


PointLightObject::PointLightObject() : LightObject(kLightPoint, kLightPoint)
{
	fadeDistance.Set(0.0F, 0.0F);
	minShadowDistance = 0.1F;
}

PointLightObject::PointLightObject(LightType type) : LightObject(type, kLightPoint)
{
	fadeDistance.Set(0.0F, 0.0F);
	minShadowDistance = 0.1F;
}

PointLightObject::PointLightObject(LightType type, const ColorRGB& color, float range) : LightObject(type, kLightPoint, color)
{
	lightRange = range;
	fadeDistance.Set(0.0F, 0.0F);
	minShadowDistance = 0.1F;
	confinementRadius = 0.0F;
}

PointLightObject::PointLightObject(const ColorRGB& color, float range) : PointLightObject(kLightPoint, color, range)
{
}

PointLightObject::PointLightObject(const PointLightObject& pointLightObject) : LightObject(pointLightObject)
{
	lightRange = pointLightObject.lightRange;
	fadeDistance = pointLightObject.fadeDistance;
	minShadowDistance = pointLightObject.minShadowDistance;
	confinementRadius = pointLightObject.confinementRadius;
}

PointLightObject::~PointLightObject()
{
}

Object *PointLightObject::Replicate(void) const
{
	return (new PointLightObject(*this));
}

void PointLightObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	LightObject::Pack(data, packFlags);

	data << ChunkHeader('RANG', 4);
	data << lightRange;

	data << ChunkHeader('FADE', sizeof(Range<float>));
	data << fadeDistance;

	data << ChunkHeader('MSHD', 4);
	data << minShadowDistance;

	data << ChunkHeader('CRAD', 4);
	data << confinementRadius;

	data << TerminatorChunk;
}

void PointLightObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	LightObject::Unpack(data, unpackFlags);
	UnpackChunkList<PointLightObject>(data, unpackFlags);
}

bool PointLightObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'RANG':

			data >> lightRange;
			return (true);

		case 'FADE':

			data >> fadeDistance;
			return (true);

		case 'MSHD':

			data >> minShadowDistance;
			return (true);

		case 'CRAD':

			data >> confinementRadius;
			return (true);
	}

	return (false);
}

int32 PointLightObject::GetObjectSize(float *size) const
{
	size[0] = lightRange;
	return (1);
}

void PointLightObject::SetObjectSize(const float *size)
{
	lightRange = size[0];
}

int32 PointLightObject::GetCategorySettingCount(Type category) const
{
	int32 count = LightObject::GetCategorySettingCount(category);
	if (category == kObjectLight)
	{
		count += 3;
	}

	return (count);
}

Setting *PointLightObject::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kObjectLight)
	{
		int32 i = index - LightObject::GetCategorySettingCount(kObjectLight);
		if (i >= 0)
		{
			if (flags & kConfigurationScript)
			{
				return (nullptr);
			}

			const StringTable *table = TheInterfaceMgr->GetStringTable();

			if (i == 0)
			{
				const char *title = table->GetString(StringID(kObjectLight, 'PONT'));
				return (new HeadingSetting('PONT', title));
			}

			if (i == 1)
			{
				const char *title = table->GetString(StringID(kObjectLight, 'PONT', 'FMIN'));
				return (new TextSetting('FMIN', fadeDistance.min, title));
			}

			if (i == 2)
			{
				const char *title = table->GetString(StringID(kObjectLight, 'PONT', 'FMAX'));
				return (new TextSetting('FMAX', fadeDistance.max, title));
			}

			return (nullptr);
		}
	}

	return (LightObject::GetCategorySetting(category, index, flags));
}

void PointLightObject::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kObjectLight)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'FMIN')
		{
			float value = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
			fadeDistance.min = FmaxZero(value);
		}
		else if (identifier == 'FMAX')
		{
			float value = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
			fadeDistance.max = (value > K::min_float) ? Fmax(value, fadeDistance.min + 1.0F) : 0.0F;
		}
		else
		{
			LightObject::SetCategorySetting(kObjectLight, setting);
		}
	}
	else
	{
		LightObject::SetCategorySetting(category, setting);
	}
}


ProjectionLightObject::ProjectionLightObject(LightType type) : PointLightObject(type)
{
	projectionMap = nullptr;
}

ProjectionLightObject::ProjectionLightObject(LightType type, const ColorRGB& color, float range, const char *name) : PointLightObject(type, color, range)
{
	projectionMap = nullptr;

	textureSize = 128;
	textureFormat = kTextureI8;
	SetProjectionMap(name);
}

ProjectionLightObject::ProjectionLightObject(const ProjectionLightObject& projectionLightObject) : PointLightObject(projectionLightObject)
{
	projectionMap = nullptr;

	textureSize = projectionLightObject.textureSize;
	textureFormat = projectionLightObject.textureFormat;
	SetProjectionMap(projectionLightObject.projectionName);
}

ProjectionLightObject::~ProjectionLightObject()
{
	if (projectionMap)
	{
		projectionMap->Release();
	}
}

void ProjectionLightObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	PointLightObject::Pack(data, packFlags);

	PackHandle handle = data.BeginChunk('PROJ');
	data << projectionName;
	data.EndChunk(handle);

	data << ChunkHeader('SIZE', 4);
	data << textureSize;

	data << ChunkHeader('FORM', 4);
	data << textureFormat;

	data << TerminatorChunk;
}

void ProjectionLightObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	PointLightObject::Unpack(data, unpackFlags);
	UnpackChunkList<ProjectionLightObject>(data, unpackFlags);
}

bool ProjectionLightObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'PROJ':

		#if C4LEGACY

			case 'SHAD':

		#endif

			data >> projectionName;
			SetProjectionMap(projectionName);
			return (true);

		case 'SIZE':

			data >> textureSize;
			return (true);

		case 'FORM':

			data >> textureFormat;
			return (true);
	}

	return (false);
}

void *ProjectionLightObject::BeginSettingsUnpack(void)
{
	if (projectionMap)
	{
		projectionMap->Release();
		projectionMap = nullptr;
	}

	return (PointLightObject::BeginSettingsUnpack());
}

int32 ProjectionLightObject::GetCategorySettingCount(Type category) const
{
	int32 count = PointLightObject::GetCategorySettingCount(category);
	if (category == kObjectLight)
	{
		count += 5;
	}

	return (count);
}

Setting *ProjectionLightObject::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kObjectLight)
	{
		int32 i = index - PointLightObject::GetCategorySettingCount(kObjectLight);
		if (i >= 0)
		{
			if (flags & kConfigurationScript)
			{
				return (nullptr);
			}

			const StringTable *table = TheInterfaceMgr->GetStringTable();

			if (i == 0)
			{
				const char *title = table->GetString(StringID(kObjectLight, 'TXTR'));
				return (new HeadingSetting('TXTR', title));
			}

			if (i == 1)
			{
				const char *title = table->GetString(StringID(kObjectLight, 'TXTR', 'GENR'));
				return (new BooleanSetting('GENR', ((GetLightFlags() & kLightGenerator) != 0), title));
			}

			if (i == 2)
			{
				const char *title = table->GetString(StringID(kObjectLight, 'TXTR', 'SIZE'));
				return (new PowerTwoSetting('SIZE', textureSize, title, 16, 1024));
			}

			if (i == 3)
			{
				const char *title = table->GetString(StringID(kObjectLight, 'TXTR', 'FORM'));
				MenuSetting *menu = new MenuSetting('FORM', (textureFormat != kTextureRGBA8), title, 2);

				menu->SetMenuItemString(0, table->GetString(StringID(kObjectLight, 'TXTR', 'FORM', kTextureRGBA8)));
				menu->SetMenuItemString(1, table->GetString(StringID(kObjectLight, 'TXTR', 'FORM', kTextureI8)));

				return (menu);
			}

			if (i == 4)
			{
				const char *title = table->GetString(StringID(kObjectLight, 'TXTR', 'TNAM'));
				const char *picker = table->GetString(StringID(kObjectLight, 'TXTR', 'PICK'));
				return (new ResourceSetting('TNAM', projectionName, title, picker, TextureResource::GetDescriptor()));
			}

			return (nullptr);
		}
	}

	return (PointLightObject::GetCategorySetting(category, index, flags));
}

void ProjectionLightObject::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kObjectLight)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'GENR')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				SetLightFlags(GetLightFlags() | kLightGenerator);
			}
			else
			{
				SetLightFlags(GetLightFlags() & ~kLightGenerator);
			}
		}
		else if (identifier == 'SIZE')
		{
			textureSize = static_cast<const PowerTwoSetting *>(setting)->GetIntegerValue();
		}
		else if (identifier == 'FORM')
		{
			textureFormat = (static_cast<const MenuSetting *>(setting)->GetMenuSelection() == 0) ? kTextureRGBA8 : kTextureI8;
		}
		else if (identifier == 'TNAM')
		{
			SetProjectionMap(static_cast<const ResourceSetting *>(setting)->GetResourceName());
		}
		else
		{
			PointLightObject::SetCategorySetting(kObjectLight, setting);
		}
	}
}

void ProjectionLightObject::SetProjectionMap(const char *name)
{
	Texture *texture = projectionMap;

	projectionName = name;
	projectionMap = Texture::Get(name);
	if (!projectionMap)
	{
		projectionMap = Texture::Get((GetLightType() == kLightCube) ? "C4/cube" : "C4/spot");
	}

	if (texture)
	{
		texture->Release();
	}
}


CubeLightObject::CubeLightObject() : ProjectionLightObject(kLightCube)
{
}

CubeLightObject::CubeLightObject(const ColorRGB& color, float range, const char *name) : ProjectionLightObject(kLightCube, color, range, name)
{
}

CubeLightObject::CubeLightObject(const CubeLightObject& cubeLightObject) : ProjectionLightObject(cubeLightObject)
{
}

CubeLightObject::~CubeLightObject()
{
}

Object *CubeLightObject::Replicate(void) const
{
	return (new CubeLightObject(*this));
}


SpotLightObject::SpotLightObject() : ProjectionLightObject(kLightSpot)
{
}

SpotLightObject::SpotLightObject(const ColorRGB& color, float range, float apex, const char *name) : ProjectionLightObject(kLightSpot, color, range, name)
{
	apexTangent = apex;
	CalculateAspectRatio();
}

SpotLightObject::SpotLightObject(const SpotLightObject& spotLightObject) : ProjectionLightObject(spotLightObject)
{
	apexTangent = spotLightObject.apexTangent;
	CalculateAspectRatio();
}

SpotLightObject::~SpotLightObject()
{
}

Object *SpotLightObject::Replicate(void) const
{
	return (new SpotLightObject(*this));
}

void SpotLightObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	ProjectionLightObject::Pack(data, packFlags);

	data << ChunkHeader('APEX', 4);
	data << apexTangent;

	data << TerminatorChunk;
}

void SpotLightObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	ProjectionLightObject::Unpack(data, unpackFlags);
	UnpackChunkList<SpotLightObject>(data, unpackFlags);
}

bool SpotLightObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'APEX':

			data >> apexTangent;
			return (true);
	}

	return (false);
}

int32 SpotLightObject::GetObjectSize(float *size) const
{
	size[0] = GetLightRange();
	size[1] = apexTangent;
	return (2);
}

void SpotLightObject::SetObjectSize(const float *size)
{
	SetLightRange(size[0]);
	apexTangent = size[1];
}

void SpotLightObject::CalculateAspectRatio(void)
{
	const Texture *texture = GetProjectionMap();
	aspectRatio = (float) texture->GetTextureWidth() / (float) texture->GetTextureHeight();
}

void SpotLightObject::SetProjectionMap(const char *name)
{
	ProjectionLightObject::SetProjectionMap(name);
	CalculateAspectRatio();
}

// ZYUQURM
