 

#include "C4Markers.h"
#include "C4World.h"
#include "C4Paths.h"
#include "C4Configuration.h"


using namespace C4;


Map<LocatorRegistration> LocatorMarker::registrationMap;


C4::Marker::Marker(MarkerType type) : Node(kNodeMarker)
{
	markerType = type;
}

C4::Marker::Marker(const Marker& marker) : Node(marker)
{
	markerType = marker.markerType;
}

C4::Marker::~Marker()
{
}

C4::Marker *C4::Marker::Create(Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (data.GetType())
	{
		case kMarkerLocator:

			return (new LocatorMarker);

		case kMarkerConnection:

			return (new ConnectionMarker);

		case kMarkerCube:

			return (new CubeMarker);

		case kMarkerShader:

			return (new ShaderMarker);

		case kMarkerPath:

			return (new PathMarker);
	}

	return (nullptr);
}

void C4::Marker::PackType(Packer& data) const
{
	Node::PackType(data);
	data << markerType;
}

void C4::Marker::Preprocess(void)
{
	Node::Preprocess();

	Zone *zone = GetOwningZone();
	if (zone)
	{
		zone->AddMarker(this);
	}
}

void C4::Marker::Neutralize(void)
{
	ListElement<Marker>::Detach();
	Node::Neutralize();
}


LocatorRegistration::LocatorRegistration(LocatorType type, const char *name)
{
	locatorType = type;
	locatorName = name;

	LocatorMarker::registrationMap.Insert(this);
}

LocatorRegistration::~LocatorRegistration()
{
}


LocatorMarker::LocatorMarker() : Marker(kMarkerLocator)
{
}

LocatorMarker::LocatorMarker(LocatorType type) : Marker(kMarkerLocator)
{
	locatorType = type;
}

LocatorMarker::LocatorMarker(const LocatorMarker& locatorMarker) : Marker(locatorMarker) 
{
	locatorType = locatorMarker.locatorType;
} 

LocatorMarker::~LocatorMarker() 
{
}
 
Node *LocatorMarker::Replicate(void) const
{ 
	return (new LocatorMarker(*this)); 
}

void LocatorMarker::Pack(Packer& data, unsigned_int32 packFlags) const
{ 
	Marker::Pack(data, packFlags);

	data << ChunkHeader('TYPE', 4);
	data << locatorType;

	data << TerminatorChunk;
}

void LocatorMarker::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Marker::Unpack(data, unpackFlags);
	UnpackChunkList<LocatorMarker>(data, unpackFlags);
}

bool LocatorMarker::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'TYPE':

			data >> locatorType;
			return (true);
	}

	return (false);
}

int32 LocatorMarker::GetCategoryCount(void) const
{
	return (Marker::GetCategoryCount() + 1);
}

Type LocatorMarker::GetCategoryType(int32 index, const char **title) const
{
	int32 count = Marker::GetCategoryCount();
	if (index == count)
	{
		*title = TheInterfaceMgr->GetStringTable()->GetString(StringID(kMarkerLocator));
		return (kMarkerLocator);
	}

	return (Marker::GetCategoryType(index, title));
}

int32 LocatorMarker::GetCategorySettingCount(Type category) const
{
	if (category == kMarkerLocator)
	{
		return (2);
	}

	return (Marker::GetCategorySettingCount(category));
}

Setting *LocatorMarker::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kMarkerLocator)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();

		if (index == 0)
		{
			const char *title = table->GetString(StringID(kMarkerLocator, 'LOCA'));
			return (new HeadingSetting(kMarkerLocator, title));
		}

		if (index == 1)
		{
			const char *title = table->GetString(StringID(kMarkerLocator, 'LOCA', 'TYPE'));
			return (new TextSetting('TYPE', Text::TypeToString(locatorType), title, 4));
		}

		return (nullptr);
	}

	return (Marker::GetCategorySetting(category, index, flags));
}

void LocatorMarker::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kMarkerLocator)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'TYPE')
		{
			locatorType = Text::StringToType(static_cast<const TextSetting *>(setting)->GetText());
		}
	}
	else
	{
		Marker::SetCategorySetting(category, setting);
	}
}


ConnectionMarker::ConnectionMarker() : Marker(kMarkerConnection)
{
}

ConnectionMarker::ConnectionMarker(const ConnectionMarker& connectionMarker) : Marker(connectionMarker)
{
}

ConnectionMarker::~ConnectionMarker()
{
}

Node *ConnectionMarker::Replicate(void) const
{
	return (new ConnectionMarker(*this));
}


CubeMarker::CubeMarker() : Marker(kMarkerCube)
{
}

CubeMarker::CubeMarker(const char *name, TextureFormat format, int32 size) : Marker(kMarkerCube)
{
	cubeFlags = 0;
	cubeSize = size;

	textureFormat = format;
	textureName = name;
}

CubeMarker::CubeMarker(const CubeMarker& cubeMarker) : Marker(cubeMarker)
{
	cubeFlags = cubeMarker.cubeFlags;
	cubeSize = cubeMarker.cubeSize;

	textureFormat = cubeMarker.textureFormat;
	textureName = cubeMarker.textureName;
}

CubeMarker::~CubeMarker()
{
}

Node *CubeMarker::Replicate(void) const
{
	return (new CubeMarker(*this));
}

void CubeMarker::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Marker::Pack(data, packFlags);

	data << ChunkHeader('FLAG', 4);
	data << cubeFlags;

	data << ChunkHeader('SIZE', 4);
	data << cubeSize;

	data << ChunkHeader('FORM', 4);
	data << textureFormat;

	PackHandle handle = data.BeginChunk('NAME');
	data << textureName;
	data.EndChunk(handle);

	data << TerminatorChunk;
}

void CubeMarker::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Marker::Unpack(data, unpackFlags);
	UnpackChunkList<CubeMarker>(data, unpackFlags);
}

bool CubeMarker::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> cubeFlags;
			return (true);

		case 'SIZE':

			data >> cubeSize;
			return (true);

		case 'FORM':

			data >> textureFormat;
			return (true);

		case 'NAME':

			data >> textureName;
			return (true);

		#if C4LEGACY

			case 'DATA':

				data >> cubeFlags;
				data >> cubeSize;
				data >> textureFormat;
				data >> textureName;
				return (true);

		#endif
	}

	return (false);
}

int32 CubeMarker::GetCategoryCount(void) const
{
	return (Marker::GetCategoryCount() + 1);
}

Type CubeMarker::GetCategoryType(int32 index, const char **title) const
{
	int32 count = Marker::GetCategoryCount();
	if (index == count)
	{
		*title = TheInterfaceMgr->GetStringTable()->GetString(StringID(kMarkerCube));
		return (kMarkerCube);
	}

	return (Marker::GetCategoryType(index, title));
}

int32 CubeMarker::GetCategorySettingCount(Type category) const
{
	if (category == kMarkerCube)
	{
		return (5);
	}

	return (Marker::GetCategorySettingCount(category));
}

Setting *CubeMarker::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kMarkerCube)
	{
		if (flags & kConfigurationScript)
		{
			return (nullptr);
		}

		const StringTable *table = TheInterfaceMgr->GetStringTable();

		if (index == 0)
		{
			const char *title = table->GetString(StringID(kMarkerCube, 'CUBE'));
			return (new HeadingSetting(kMarkerCube, title));
		}

		if (index == 1)
		{
			const char *title = table->GetString(StringID(kMarkerCube, 'CUBE', 'SIZE'));
			return (new PowerTwoSetting('SIZE', cubeSize, title, 16, 1024));
		}

		if (index == 2)
		{
			int32 selection = 0;
			if (textureFormat == kTextureRGBA8)
			{
				selection = 1;
			}
			else if (textureFormat == kTextureI8)
			{
				selection = 2;
			}

			const char *title = table->GetString(StringID(kMarkerCube, 'CUBE', 'FORM'));
			MenuSetting *menu = new MenuSetting('FORM', selection, title, 3);

			menu->SetMenuItemString(0, table->GetString(StringID(kMarkerCube, 'CUBE', 'FORM', kTextureBC13)));
			menu->SetMenuItemString(1, table->GetString(StringID(kMarkerCube, 'CUBE', 'FORM', kTextureRGBA8)));
			menu->SetMenuItemString(2, table->GetString(StringID(kMarkerCube, 'CUBE', 'FORM', kTextureI8)));

			return (menu);
		}

		if (index == 3)
		{
			const char *title = table->GetString(StringID(kMarkerCube, 'CUBE', 'FILT'));
			return (new BooleanSetting('FILT', ((cubeFlags & kCubeFilter) != 0), title));
		}

		if (index == 4)
		{
			const char *title = table->GetString(StringID(kMarkerCube, 'CUBE', 'TNAM'));
			const char *picker = table->GetString(StringID(kMarkerCube, 'CUBE', 'PICK'));
			return (new ResourceSetting('TNAM', textureName, title, picker, TextureResource::GetDescriptor()));
		}

		return (nullptr);
	}

	return (Marker::GetCategorySetting(category, index, flags));
}

void CubeMarker::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kMarkerCube)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'SIZE')
		{
			cubeSize = static_cast<const PowerTwoSetting *>(setting)->GetIntegerValue();
		}
		else if (identifier == 'FORM')
		{
			int32 selection = static_cast<const MenuSetting *>(setting)->GetMenuSelection();
			if (selection == 0)
			{
				textureFormat = kTextureBC13;
			}
			else if (selection == 1)
			{
				textureFormat = kTextureRGBA8;
			}
			else
			{
				textureFormat = kTextureI8;
			}
		}
		else if (identifier == 'FILT')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				cubeFlags |= kCubeFilter;
			}
			else
			{
				cubeFlags &= ~kCubeFilter;
			}
		}
		else if (identifier == 'TNAM')
		{
			textureName = static_cast<const ResourceSetting *>(setting)->GetResourceName();
		}
	}
	else
	{
		Marker::SetCategorySetting(category, setting);
	}
}


ShaderMarker::ShaderMarker() : Marker(kMarkerShader)
{
}

ShaderMarker::ShaderMarker(const ShaderMarker& shaderMarker) : Marker(shaderMarker)
{
}

ShaderMarker::~ShaderMarker()
{
}

Node *ShaderMarker::Replicate(void) const
{
	return (new ShaderMarker(*this));
}

// ZYUQURM
