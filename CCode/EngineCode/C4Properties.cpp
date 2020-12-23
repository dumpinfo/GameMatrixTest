 

#include "C4Properties.h"
#include "C4Effects.h"
#include "C4Impostors.h"
#include "C4Configuration.h"


using namespace C4;


namespace C4
{
	template class Registrable<Property, PropertyRegistration>;
}


PropertyRegistration::PropertyRegistration(PropertyType type, const char *name) : Registration<Property, PropertyRegistration>(type)
{
	propertyName = name;
}

PropertyRegistration::~PropertyRegistration()
{
}


Property::Property(PropertyType type)
{
	propertyType = type;
	propertyFlags = 0;
}

Property::Property(const Property& property)
{
	propertyType = property.propertyType;
	propertyFlags = property.propertyFlags;
}

Property::~Property()
{
}

Property *Property::Replicate(void) const
{
	return (new Property(*this));
}

Property *Property::New(PropertyType type)
{
	Type	data[2];

	data[0] = type;
	data[1] = 0;

	Unpacker unpacker(data);
	return (Create(unpacker));
}

Property *Property::Create(Unpacker& data, unsigned_int32 unpackFlags)
{
	if (data.GetType() == kPropertyName)
	{
		return (new NameProperty);
	}

	return (Registrable<Property, PropertyRegistration>::Create(data, unpackFlags));
}

Property *Property::CreateUnknown(Unpacker& data, unsigned_int32 unpackFlags)
{
	return (new UnknownProperty(data.GetType()));
}

bool Property::ValidNode(const Node *node)
{
	return (true);
}

void Property::RegisterStandardProperties(void)
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	static PropertyReg<UnknownProperty> unknownRegistration(kPropertyUnknown, table->GetString(StringID('PROP', kPropertyUnknown)));
	static PropertyReg<ClearProperty> clearRegistration(kPropertyClear, table->GetString(StringID('PROP', kPropertyClear)));
	static PropertyReg<FarClipProperty> farClipRegistration(kPropertyFarClip, table->GetString(StringID('PROP', kPropertyFarClip)));
	static PropertyReg<InteractionProperty> interactionRegistration(kPropertyInteraction, table->GetString(StringID('PROP', kPropertyInteraction)));
	static PropertyReg<AcousticsProperty> acousticsRegistration(kPropertyAcoustics, table->GetString(StringID('PROP', kPropertyAcoustics)));
	static PropertyReg<ImpostorProperty> impostorRegistration(kPropertyImpostor, table->GetString(StringID('PROP', kPropertyImpostor)));

	SetUnknownCreator(&CreateUnknown);
}

void Property::PackType(Packer& data) const
{
	data << propertyType;
}

void Property::Pack(Packer& data, unsigned_int32 packFlags) const
{
	data << ChunkHeader('DATA', 4);
	data << propertyFlags;
 
	data << TerminatorChunk;
}
 
void Property::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{ 
	UnpackChunkList<Property>(data, unpackFlags);
}
 
bool Property::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{ 
	switch (chunkHeader->chunkType) 
	{
		case 'DATA':

			data >> propertyFlags; 
			return (true);
	}

	return (false);
}

bool Property::SharedProperty(void) const
{
	return (false);
}


PropertyObject::PropertyObject() : Object(kObjectProperty)
{
}

PropertyObject::~PropertyObject()
{
}

void PropertyObject::Prepack(List<Object> *linkList) const
{
	const Property *property = propertyMap.First();
	while (property)
	{
		if (!(property->GetPropertyFlags() & kPropertyNonpersistent))
		{
			property->Prepack(linkList);
		}

		property = property->Next();
	}
}

void PropertyObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	const Property *property = propertyMap.First();
	while (property)
	{
		if (!(property->GetPropertyFlags() & kPropertyNonpersistent))
		{
			PackHandle handle = data.BeginChunk('PROP');
			property->PackType(data);
			property->Pack(data, packFlags);
			data.EndChunk(handle);
		}

		property = property->Next();
	}

	data << TerminatorChunk;
}

void PropertyObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	UnpackChunkList<PropertyObject>(data, unpackFlags);
}

bool PropertyObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'PROP':
		{
			Property *property = Property::Create(data, unpackFlags);
			if (property)
			{
				property->Unpack(++data, unpackFlags);
				propertyMap.Insert(property);
				return (true);
			}

			break;
		}
	}

	return (false);
}

void *PropertyObject::BeginSettingsUnpack(void)
{
	propertyMap.Purge();
	return (nullptr);
}


UnknownProperty::UnknownProperty() : Property(kPropertyUnknown)
{
	unknownType = 0;
	unknownSize = 0;
	unknownData = nullptr;
}

UnknownProperty::UnknownProperty(PropertyType type) : Property(kPropertyUnknown)
{
	unknownType = type;
	unknownSize = 0;
	unknownData = nullptr;
}

UnknownProperty::UnknownProperty(const UnknownProperty& unknownProperty) : Property(unknownProperty)
{
	unknownType = unknownProperty.unknownType;
	unknownSize = unknownProperty.unknownSize;
	unknownData = new char[unknownSize];
	MemoryMgr::CopyMemory(unknownProperty.unknownData, unknownData, unknownSize);
}

UnknownProperty::~UnknownProperty()
{
	delete[] unknownData;
}

Property *UnknownProperty::Replicate(void) const
{
	return (new UnknownProperty(*this));
}

bool UnknownProperty::ValidNode(const Node *node)
{
	return (false);
}

void UnknownProperty::PackType(Packer& data) const
{
	data << unknownType;
}

void UnknownProperty::Pack(Packer& data, unsigned_int32 packFlags) const
{
	data.WriteData(unknownData, unknownSize);
}

void UnknownProperty::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	unsigned_int32 size = data.GetUnknownSize() - sizeof(PropertyType);
	unknownSize = size;
	unknownData = new char[size];
	data.ReadData(unknownData, size);
}

int32 UnknownProperty::GetSettingCount(void) const
{
	return (1);
}

Setting *UnknownProperty::GetSetting(int32 index) const
{
	if (index == 0)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();
		const char *title = table->GetString(StringID('PROP', kPropertyUnknown, 'TYPE'));
		return (new InfoSetting('TYPE', Text::TypeToHexCharString(unknownType), title));
	}

	return (nullptr);
}

void UnknownProperty::SetSetting(const Setting *setting)
{
}


NameProperty::NameProperty() : Property(kPropertyName)
{
}

NameProperty::NameProperty(const NameProperty& nameProperty) : Property(nameProperty)
{
	nodeName = nameProperty.nodeName;
}

NameProperty::NameProperty(const char *name) : Property(kPropertyName)
{
	nodeName = name;
}

NameProperty::~NameProperty()
{
}

Property *NameProperty::Replicate(void) const
{
	return (new NameProperty(*this));
}

void NameProperty::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Property::Pack(data, packFlags);

	PackHandle handle = data.BeginChunk('NAME');
	data << nodeName;
	data.EndChunk(handle);

	data << TerminatorChunk;
}

void NameProperty::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Property::Unpack(data, unpackFlags);
	UnpackChunkList<NameProperty>(data, unpackFlags);
}

bool NameProperty::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'NAME':

			data >> nodeName;
			return (true);
	}

	return (false);
}


ClearProperty::ClearProperty() : Property(kPropertyClear)
{
	clearColor.Set(0.0F, 0.0F, 0.0F, 0.0F);
}

ClearProperty::ClearProperty(const ClearProperty& clearProperty) : Property(clearProperty)
{
	clearColor = clearProperty.clearColor;
}

ClearProperty::ClearProperty(const ColorRGBA& color) : Property(kPropertyClear)
{
	clearColor = color;
}

ClearProperty::~ClearProperty()
{
}

Property *ClearProperty::Replicate(void) const
{
	return (new ClearProperty(*this));
}

bool ClearProperty::ValidNode(const Node *node)
{
	return ((node->GetNodeType() == kNodeZone) && (!node->GetSuperNode()));
}

void ClearProperty::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Property::Pack(data, packFlags);

	data << ChunkHeader('COLR', sizeof(ColorRGBA));
	data << clearColor;

	data << TerminatorChunk;
}

void ClearProperty::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Property::Unpack(data, unpackFlags);
	UnpackChunkList<ClearProperty>(data, unpackFlags);
}

bool ClearProperty::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'COLR':

			data >> clearColor;
			return (true);
	}

	return (false);
}

int32 ClearProperty::GetSettingCount(void) const
{
	return (1);
}

Setting *ClearProperty::GetSetting(int32 index) const
{
	if (index == 0)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();
		const char *title = table->GetString(StringID('PROP', kPropertyClear, 'COLR'));
		const char *picker = table->GetString(StringID('PROP', kPropertyClear, 'PICK'));
		return (new ColorSetting('COLR', clearColor, title, picker));
	}

	return (nullptr);
}

void ClearProperty::SetSetting(const Setting *setting)
{
	if (setting->GetSettingIdentifier() == 'COLR')
	{
		clearColor = static_cast<const ColorSetting *>(setting)->GetColor();
	}
}


FarClipProperty::FarClipProperty() : Property(kPropertyFarClip)
{
	farClipDepth = 1000.0F;
}

FarClipProperty::FarClipProperty(const FarClipProperty& farClipProperty) : Property(farClipProperty)
{
	farClipDepth = farClipProperty.farClipDepth;
}

FarClipProperty::FarClipProperty(float depth) : Property(kPropertyFarClip)
{
	farClipDepth = depth;
}

FarClipProperty::~FarClipProperty()
{
}

Property *FarClipProperty::Replicate(void) const
{
	return (new FarClipProperty(*this));
}

bool FarClipProperty::ValidNode(const Node *node)
{
	return ((node->GetNodeType() == kNodeZone) && (!node->GetSuperNode()));
}

void FarClipProperty::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Property::Pack(data, packFlags);

	data << ChunkHeader('DPTH', 4);
	data << farClipDepth;

	data << TerminatorChunk;
}

void FarClipProperty::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Property::Unpack(data, unpackFlags);
	UnpackChunkList<FarClipProperty>(data, unpackFlags);
}

bool FarClipProperty::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'DPTH':

			data >> farClipDepth;
			return (true);
	}

	return (false);
}

int32 FarClipProperty::GetSettingCount(void) const
{
	return (1);
}

Setting *FarClipProperty::GetSetting(int32 index) const
{
	if (index == 0)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();
		const char *title = table->GetString(StringID('PROP', kPropertyFarClip, 'DPTH'));
		return (new TextSetting('DPTH', farClipDepth, title));
	}

	return (nullptr);
}

void FarClipProperty::SetSetting(const Setting *setting)
{
	if (setting->GetSettingIdentifier() == 'DPTH')
	{
		farClipDepth = Fmax(Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText()), 1.0F);
	}
}


InteractionProperty::InteractionProperty() : Property(kPropertyInteraction)
{
}

InteractionProperty::InteractionProperty(const InteractionProperty& interactionProperty) : Property(interactionProperty)
{
}

InteractionProperty::~InteractionProperty()
{
}

Property *InteractionProperty::Replicate(void) const
{
	return (new InteractionProperty(*this));
}

bool InteractionProperty::ValidNode(const Node *node)
{
	NodeType type = node->GetNodeType();

	if (type == kNodeGeometry)
	{
		return (true);
	}

	if ((type == kNodeEffect) && (static_cast<const Effect *>(node)->GetEffectType() == kEffectPanel))
	{
		return (true);
	}

	return (false);
}

int32 InteractionProperty::GetSettingCount(void) const
{
	return (1);
}

Setting *InteractionProperty::GetSetting(int32 index) const
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('PROP', kPropertyInteraction, 'DSAB'));
		return (new BooleanSetting('DSAB', ((GetPropertyFlags() & kPropertyDisabled) != 0), title));
	}

	return (nullptr);
}

void InteractionProperty::SetSetting(const Setting *setting)
{
	if (setting->GetSettingIdentifier() == 'DSAB')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			SetPropertyFlags(GetPropertyFlags() | kPropertyDisabled);
		}
		else
		{
			SetPropertyFlags(GetPropertyFlags() & ~kPropertyDisabled);
		}
	}
}


AcousticsProperty::AcousticsProperty() : Property(kPropertyAcoustics)
{
	directVolume = 1.0F;
	directHFVolume = 1.0F;
	reflectionVolume = 1.0F;
	reflectionHFVolume = 1.0F;
}

AcousticsProperty::AcousticsProperty(const AcousticsProperty& acousticsProperty) : Property(acousticsProperty)
{
	directVolume = acousticsProperty.directVolume;
	directHFVolume = acousticsProperty.directHFVolume;
	reflectionVolume = acousticsProperty.reflectionVolume;
	reflectionHFVolume = acousticsProperty.reflectionHFVolume;
}

AcousticsProperty::AcousticsProperty(float direct, float directHF, float reflection, float reflectionHF) : Property(kPropertyAcoustics)
{
	directVolume = direct;
	directHFVolume = directHF;
	reflectionVolume = reflection;
	reflectionHFVolume = reflectionHF;
}

AcousticsProperty::~AcousticsProperty()
{
}

Property *AcousticsProperty::Replicate(void) const
{
	return (new AcousticsProperty(*this));
}

bool AcousticsProperty::ValidNode(const Node *node)
{
	return (node->GetNodeType() == kNodeGeometry);
}

void AcousticsProperty::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Property::Pack(data, packFlags);

	data << ChunkHeader('DRCT', 8);
	data << directVolume;
	data << directHFVolume;

	data << ChunkHeader('REFL', 8);
	data << reflectionVolume;
	data << reflectionHFVolume;

	data << TerminatorChunk;
}

void AcousticsProperty::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Property::Unpack(data, unpackFlags);
	UnpackChunkList<AcousticsProperty>(data, unpackFlags);
}

bool AcousticsProperty::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'DRCT':

			data >> directVolume;
			data >> directHFVolume;
			return (true);

		case 'REFL':

			data >> reflectionVolume;
			data >> reflectionHFVolume;
			return (true);
	}

	return (false);
}

int32 AcousticsProperty::GetSettingCount(void) const
{
	return (4);
}

Setting *AcousticsProperty::GetSetting(int32 index) const
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('PROP', kPropertyAcoustics, 'DRCT'));
		return (new IntegerSetting('DRCT', (int32) (directVolume * 100.0 + 0.5F), title, 0, 100, 1));
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('PROP', kPropertyAcoustics, 'DRCH'));
		return (new IntegerSetting('DRCH', (int32) (directHFVolume * 100.0 + 0.5F), title, 0, 100, 1));
	}

	if (index == 2)
	{
		const char *title = table->GetString(StringID('PROP', kPropertyAcoustics, 'REFL'));
		return (new IntegerSetting('REFL', (int32) (reflectionVolume * 100.0 + 0.5F), title, 0, 100, 1));
	}

	if (index == 3)
	{
		const char *title = table->GetString(StringID('PROP', kPropertyAcoustics, 'REFH'));
		return (new IntegerSetting('REFH', (int32) (reflectionHFVolume * 100.0 + 0.5F), title, 0, 100, 1));
	}

	return (nullptr);
}

void AcousticsProperty::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'DRCT')
	{
		int32 value = static_cast<const IntegerSetting *>(setting)->GetIntegerValue();
		directVolume = (float) value * 0.01F;
	}
	else if (identifier == 'DRCH')
	{
		int32 value = static_cast<const IntegerSetting *>(setting)->GetIntegerValue();
		directHFVolume = (float) value * 0.01F;
	}
	else if (identifier == 'REFL')
	{
		int32 value = static_cast<const IntegerSetting *>(setting)->GetIntegerValue();
		reflectionVolume = (float) value * 0.01F;
	}
	else if (identifier == 'REFH')
	{
		int32 value = static_cast<const IntegerSetting *>(setting)->GetIntegerValue();
		reflectionHFVolume = (float) value * 0.01F;
	}
}

// ZYUQURM
