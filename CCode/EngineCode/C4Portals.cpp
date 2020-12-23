 

#include "C4Portals.h"
#include "C4Zones.h"
#include "C4Panels.h"
#include "C4Configuration.h"


using namespace C4;


const char C4::kConnectorKeyZone[] = "%Zone";


PortalObject::PortalObject(PortalType type) : Object(kObjectPortal)
{
	portalType = type;
	portalFlags = 0;

	vertexCount = 0;
}

PortalObject::PortalObject(PortalType type, int32 count) : Object(kObjectPortal)
{
	portalType = type;
	portalFlags = 0;

	vertexCount = count;
}

PortalObject::PortalObject(PortalType type, const Vector2D& size) : Object(kObjectPortal)
{
	portalType = type;
	portalFlags = 0;

	SetPortalSize(size);
}

PortalObject::~PortalObject()
{
}

PortalObject *PortalObject::Create(Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (data.GetType())
	{
		case kPortalDirect:

			return (new DirectPortalObject);

		case kPortalRemote:

			return (new RemotePortalObject);

		case kPortalCamera:

			return (new CameraPortalObject);

		case kPortalOcclusion:

			return (new OcclusionPortalObject);
	}

	return (nullptr);
}

void PortalObject::PackType(Packer& data) const
{
	Object::PackType(data);
	data << portalType;
}

void PortalObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	data << ChunkHeader('DATA', 4);
	data << portalFlags;

	data << ChunkHeader('VERT', 4 + vertexCount * sizeof(Point3D));
	data << vertexCount;
	data.WriteArray(vertexCount, portalVertex);

	data << TerminatorChunk;
}

void PortalObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	UnpackChunkList<PortalObject>(data, unpackFlags);
}

bool PortalObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'DATA':

			data >> portalFlags;
			return (true);

		case 'VERT':

			data >> vertexCount;

			#if C4LEGACY 

				if (vertexCount > kMaxPortalVertexCount)
				{ 
					Point3D		deadVertex[kMaxPortalVertexCount];
 
					data.ReadArray(kMaxPortalVertexCount, portalVertex);
					data.ReadArray(vertexCount - kMaxPortalVertexCount, deadVertex);
					vertexCount = kMaxPortalVertexCount; 
				}
				else 
				{ 
					data.ReadArray(vertexCount, portalVertex);
				}

			#else 

				data.ReadArray(vertexCount, portalVertex);

			#endif

			return (true);
	}

	return (false);
}

void PortalObject::SetPortalSize(const Vector2D& size)
{
	vertexCount = 4;

	float x = size.x;
	float y = size.y;

	portalVertex[0].Set(0.0F, 0.0F, 0.0F);
	portalVertex[1].Set(x, 0.0F, 0.0F);
	portalVertex[2].Set(x, y, 0.0F);
	portalVertex[3].Set(0.0F, y, 0.0F);
}


DirectPortalObject::DirectPortalObject() : PortalObject(kPortalDirect)
{
}

DirectPortalObject::DirectPortalObject(int32 count) : PortalObject(kPortalDirect, count)
{
}

DirectPortalObject::DirectPortalObject(const Vector2D& size) : PortalObject(kPortalDirect, size)
{
}

DirectPortalObject::~DirectPortalObject()
{
}

int32 DirectPortalObject::GetCategoryCount(void) const
{
	return (1);
}

Type DirectPortalObject::GetCategoryType(int32 index, const char **title) const
{
	if (index == 0)
	{
		*title = TheInterfaceMgr->GetStringTable()->GetString(StringID(kObjectPortal));
		return (kObjectPortal);
	}

	return (0);
}

int32 DirectPortalObject::GetCategorySettingCount(Type category) const
{
	int32 count = PortalObject::GetCategorySettingCount(category);
	if (category == kObjectPortal)
	{
		count += 7;
	}

	return (count);
}

Setting *DirectPortalObject::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kObjectPortal)
	{
		int32 i = index - PortalObject::GetCategorySettingCount(kObjectPortal);
		if (i >= 0)
		{
			const StringTable *table = TheInterfaceMgr->GetStringTable();

			if (i == 0)
			{
				const char *title = table->GetString(StringID(kObjectPortal, kPortalDirect));
				return (new HeadingSetting('PDIR', title));
			}

			if (i == 1)
			{
				const char *title = table->GetString(StringID(kObjectPortal, kPortalDirect, 'LITE'));
				return (new BooleanSetting('LITE', ((GetPortalFlags() & kPortalLightInhibit) != 0), title));
			}

			if (i == 2)
			{
				const char *title = table->GetString(StringID(kObjectPortal, kPortalDirect, 'STAT'));
				return (new BooleanSetting('STAT', ((GetPortalFlags() & kPortalStaticLightInhibit) != 0), title));
			}

			if (i == 3)
			{
				const char *title = table->GetString(StringID(kObjectPortal, kPortalDirect, 'SHAD'));
				return (new BooleanSetting('SHAD', ((GetPortalFlags() & kPortalShadowMapInhibit) != 0), title));
			}

			if (i == 4)
			{
				const char *title = table->GetString(StringID(kObjectPortal, kPortalDirect, 'SRGN'));
				return (new BooleanSetting('SRGN', ((GetPortalFlags() & kPortalFullShadowRegion) != 0), title));
			}

			if (i == 5)
			{
				const char *title = table->GetString(StringID(kObjectPortal, kPortalDirect, 'NFOG'));
				return (new BooleanSetting('NFOG', ((GetPortalFlags() & kPortalFogInhibit) != 0), title));
			}

			if (i == 6)
			{
				const char *title = table->GetString(StringID(kObjectPortal, kPortalDirect, 'SORC'));
				return (new BooleanSetting('SORC', ((GetPortalFlags() & kPortalSourceInhibit) != 0), title));
			}

			return (nullptr);
		}
	}

	return (PortalObject::GetCategorySetting(category, index, flags));
}

void DirectPortalObject::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kObjectPortal)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'LITE')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				SetPortalFlags(GetPortalFlags() | kPortalLightInhibit);
			}
			else
			{
				SetPortalFlags(GetPortalFlags() & ~kPortalLightInhibit);
			}
		}
		else if (identifier == 'STAT')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				SetPortalFlags(GetPortalFlags() | kPortalStaticLightInhibit);
			}
			else
			{
				SetPortalFlags(GetPortalFlags() & ~kPortalStaticLightInhibit);
			}
		}
		else if (identifier == 'SHAD')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				SetPortalFlags(GetPortalFlags() | kPortalShadowMapInhibit);
			}
			else
			{
				SetPortalFlags(GetPortalFlags() & ~kPortalShadowMapInhibit);
			}
		}
		else if (identifier == 'SRGN')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				SetPortalFlags(GetPortalFlags() | kPortalFullShadowRegion);
			}
			else
			{
				SetPortalFlags(GetPortalFlags() & ~kPortalFullShadowRegion);
			}
		}
		else if (identifier == 'NFOG')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				SetPortalFlags(GetPortalFlags() | kPortalFogInhibit);
			}
			else
			{
				SetPortalFlags(GetPortalFlags() & ~kPortalFogInhibit);
			}
		}
		else if (identifier == 'SORC')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				SetPortalFlags(GetPortalFlags() | kPortalSourceInhibit);
			}
			else
			{
				SetPortalFlags(GetPortalFlags() & ~kPortalSourceInhibit);
			}
		}
		else
		{
			PortalObject::SetCategorySetting(kObjectPortal, setting);
		}
	}
}


RemotePortalObject::RemotePortalObject() : PortalObject(kPortalRemote)
{
	portalBuffer = kPortalBufferPrimary;
	portalClearColor.Set(0.0F, 0.0F, 0.0F, 0.0F);
	portalPlaneOffset = 0.0F;

	minDetailLevel = 0;
	detailLevelBias = 0.0F;
	farClipDepth = 1000.0F;

	focalLengthMultiplier = 1.0F;

	localConnectorKey = "Local";
	remoteConnectorKey = "Remote";
}

RemotePortalObject::RemotePortalObject(const Vector2D& size) : PortalObject(kPortalRemote, size)
{
	portalBuffer = kPortalBufferPrimary;
	portalClearColor.Set(0.0F, 0.0F, 0.0F, 0.0F);
	portalPlaneOffset = 0.0F;

	minDetailLevel = 0;
	detailLevelBias = 0.0F;
	farClipDepth = 1000.0F;

	focalLengthMultiplier = 1.0F;

	localConnectorKey = "Local";
	remoteConnectorKey = "Remote";
}

RemotePortalObject::~RemotePortalObject()
{
}

void RemotePortalObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	PortalObject::Pack(data, packFlags);

	data << ChunkHeader('BUFF', 4);
	data << portalBuffer;

	data << ChunkHeader('CLER', sizeof(ColorRGBA));
	data << portalClearColor;

	data << ChunkHeader('OFST', 4);
	data << portalPlaneOffset;

	data << ChunkHeader('MLEV', 4);
	data << minDetailLevel;

	data << ChunkHeader('BIAS', 4);
	data << detailLevelBias;

	data << ChunkHeader('DPTH', 4);
	data << farClipDepth;

	data << ChunkHeader('FMUL', 4);
	data << focalLengthMultiplier;

	PackHandle handle = data.BeginChunk('LKEY');
	data << localConnectorKey;
	data.EndChunk(handle);

	handle = data.BeginChunk('RKEY');
	data << remoteConnectorKey;
	data.EndChunk(handle);

	data << TerminatorChunk;
}

void RemotePortalObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	PortalObject::Unpack(data, unpackFlags);
	UnpackChunkList<RemotePortalObject>(data, unpackFlags);
}

bool RemotePortalObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'BUFF':

			data >> portalBuffer;
			return (true);

		case 'CLER':

			data >> portalClearColor;
			return (true);

		case 'OFST':

			data >> portalPlaneOffset;
			return (true);

		case 'MLEV':

			data >> minDetailLevel;
			return (true);

		case 'BIAS':

			data >> detailLevelBias;
			return (true);

		case 'DPTH':

			data >> farClipDepth;
			return (true);

		case 'FMUL':

			data >> focalLengthMultiplier;
			return (true);

		case 'LKEY':

			data >> localConnectorKey;
			return (true);

		case 'RKEY':

			data >> remoteConnectorKey;
			return (true);
	}

	return (false);
}

int32 RemotePortalObject::GetCategoryCount(void) const
{
	return (1);
}

Type RemotePortalObject::GetCategoryType(int32 index, const char **title) const
{
	if (index == 0)
	{
		*title = TheInterfaceMgr->GetStringTable()->GetString(StringID(kObjectPortal));
		return (kObjectPortal);
	}

	return (0);
}

int32 RemotePortalObject::GetCategorySettingCount(Type category) const
{
	int32 count = PortalObject::GetCategorySettingCount(category);
	if (category == kObjectPortal)
	{
		count += 19;
	}

	return (count);
}

Setting *RemotePortalObject::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kObjectPortal)
	{
		int32 i = index - PortalObject::GetCategorySettingCount(kObjectPortal);
		if (i >= 0)
		{
			const StringTable *table = TheInterfaceMgr->GetStringTable();

			if (i == 0)
			{
				const char *title = table->GetString(StringID(kObjectPortal, kPortalRemote));
				return (new HeadingSetting('PREM', title));
			}

			if (i == 1)
			{
				const char *title = table->GetString(StringID(kObjectPortal, kPortalRemote, 'NSKY'));
				return (new BooleanSetting('NSKY', ((GetPortalFlags() & kPortalSkyboxInhibit) != 0), title));
			}

			if (i == 2)
			{
				const char *title = table->GetString(StringID(kObjectPortal, kPortalRemote, 'DIST'));
				return (new BooleanSetting('DIST', ((GetPortalFlags() & kPortalDistant) != 0), title));
			}

			if (i == 3)
			{
				const char *title = table->GetString(StringID(kObjectPortal, kPortalRemote, 'RLIT'));
				return (new BooleanSetting('RLIT', ((GetPortalFlags() & kPortalAllowRemoteLight) != 0), title));
			}

			if (i == 4)
			{
				const char *title = table->GetString(StringID(kObjectPortal, kPortalRemote, 'OBLQ'));
				return (new BooleanSetting('OBLQ', ((GetPortalFlags() & kPortalObliqueFrustum) != 0), title));
			}

			if (i == 5)
			{
				const char *title = table->GetString(StringID(kObjectPortal, kPortalRemote, 'FZON'));
				return (new BooleanSetting('FZON', ((GetPortalFlags() & kPortalForceCameraZone) != 0), title));
			}

			if (i == 6)
			{
				const char *title = table->GetString(StringID(kObjectPortal, kPortalRemote, 'RCUR'));
				return (new BooleanSetting('RCUR', ((GetPortalFlags() & kPortalRecursive) != 0), title));
			}

			if (i == 7)
			{
				const char *title = table->GetString(StringID(kObjectPortal, kPortalRemote, 'BUFF'));

				int32 selection = 0;
				if (portalBuffer == kPortalBufferReflection)
				{
					selection = 1;
				}
				else if (portalBuffer == kPortalBufferRefraction)
				{
					selection = 2;
				}

				MenuSetting *menu = new MenuSetting('BUFF', selection, title, 3);

				menu->SetMenuItemString(0, table->GetString(StringID(kObjectPortal, kPortalRemote, 'BUFF', 'PRIM')));
				menu->SetMenuItemString(1, table->GetString(StringID(kObjectPortal, kPortalRemote, 'BUFF', 'REFL')));
				menu->SetMenuItemString(2, table->GetString(StringID(kObjectPortal, kPortalRemote, 'BUFF', 'REFR')));

				return (menu);
			}

			if (i == 8)
			{
				const char *title = table->GetString(StringID(kObjectPortal, kPortalDirect, 'SHAD'));
				return (new BooleanSetting('SHAD', ((GetPortalFlags() & kPortalShadowMapInhibit) != 0), title));
			}

			if (i == 9)
			{
				const char *title = table->GetString(StringID(kObjectPortal, kPortalRemote, 'RUSH'));
				return (new BooleanSetting('RUSH', ((GetPortalFlags() & kPortalSeparateShadowMap) == 0), title));
			}

			if (i == 10)
			{
				const char *title = table->GetString(StringID(kObjectPortal, kPortalRemote, 'MXDP'));
				return (new BooleanSetting('MXDP', ((GetPortalFlags() & kPortalShadowMapMaxDepth) != 0), title));
			}

			if (i == 11)
			{
				const char *title = table->GetString(StringID(kObjectPortal, kPortalRemote, 'CLER'));
				const char *picker = table->GetString(StringID(kObjectPortal, kPortalRemote, 'CPCK'));
				return (new CheckColorSetting('CLER', ((GetPortalFlags() & kPortalOverrideClearColor) != 0), portalClearColor, title, picker));
			}

			if (i == 12)
			{
				const char *title = table->GetString(StringID(kObjectPortal, kPortalRemote, 'OFST'));
				return (new TextSetting('OFST', portalPlaneOffset, title));
			}

			if (i == 13)
			{
				const char *title = table->GetString(StringID(kObjectPortal, kPortalRemote, 'MLEV'));
				return (new IntegerSetting('MLEV', minDetailLevel, title, 0, 3, 1));
			}

			if (i == 14)
			{
				const char *title = table->GetString(StringID(kObjectPortal, kPortalRemote, 'BIAS'));
				return (new TextSetting('BIAS', detailLevelBias, title));
			}

			if (i == 15)
			{
				const char *title = table->GetString(StringID(kObjectPortal, kPortalRemote, 'DPTH'));
				return (new TextSetting('DPTH', farClipDepth, title));
			}

			if (i == 16)
			{
				const char *title = table->GetString(StringID(kObjectPortal, kPortalRemote, 'FMUL'));
				return (new TextSetting('FMUL', focalLengthMultiplier, title));
			}

			if (i == 17)
			{
				const char *title = table->GetString(StringID(kObjectPortal, kPortalRemote, 'LKEY'));
				return (new TextSetting('LKEY', localConnectorKey, title, kMaxConnectorKeyLength, &Connector::ConnectorKeyFilter));
			}

			if (i == 18)
			{
				const char *title = table->GetString(StringID(kObjectPortal, kPortalRemote, 'RKEY'));
				return (new TextSetting('RKEY', remoteConnectorKey, title, kMaxConnectorKeyLength, &Connector::ConnectorKeyFilter));
			}

			return (nullptr);
		}
	}

	return (PortalObject::GetCategorySetting(category, index, flags));
}

void RemotePortalObject::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kObjectPortal)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'NSKY')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				SetPortalFlags(GetPortalFlags() | kPortalSkyboxInhibit);
			}
			else
			{
				SetPortalFlags(GetPortalFlags() & ~kPortalSkyboxInhibit);
			}
		}
		else if (identifier == 'DIST')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				SetPortalFlags(GetPortalFlags() | kPortalDistant);
			}
			else
			{
				SetPortalFlags(GetPortalFlags() & ~kPortalDistant);
			}
		}
		else if (identifier == 'RLIT')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				SetPortalFlags(GetPortalFlags() | kPortalAllowRemoteLight);
			}
			else
			{
				SetPortalFlags(GetPortalFlags() & ~kPortalAllowRemoteLight);
			}
		}
		else if (identifier == 'OBLQ')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				SetPortalFlags(GetPortalFlags() | kPortalObliqueFrustum);
			}
			else
			{
				SetPortalFlags(GetPortalFlags() & ~kPortalObliqueFrustum);
			}
		}
		else if (identifier == 'FZON')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				SetPortalFlags(GetPortalFlags() | kPortalForceCameraZone);
			}
			else
			{
				SetPortalFlags(GetPortalFlags() & ~kPortalForceCameraZone);
			}
		}
		else if (identifier == 'RCUR')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				SetPortalFlags(GetPortalFlags() | kPortalRecursive);
			}
			else
			{
				SetPortalFlags(GetPortalFlags() & ~kPortalRecursive);
			}
		}
		else if (identifier == 'BUFF')
		{
			int32 selection = static_cast<const MenuSetting *>(setting)->GetMenuSelection();
			if (selection == 1)
			{
				portalBuffer = kPortalBufferReflection;
			}
			else if (selection == 2)
			{
				portalBuffer = kPortalBufferRefraction;
			}
			else
			{
				portalBuffer = kPortalBufferPrimary;
			}
		}
		else if (identifier == 'SHAD')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				SetPortalFlags(GetPortalFlags() | kPortalShadowMapInhibit);
			}
			else
			{
				SetPortalFlags(GetPortalFlags() & ~kPortalShadowMapInhibit);
			}
		}
		else if (identifier == 'RUSH')
		{
			if (!static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				SetPortalFlags(GetPortalFlags() | kPortalSeparateShadowMap);
			}
			else
			{
				SetPortalFlags(GetPortalFlags() & ~kPortalSeparateShadowMap);
			}
		}
		else if (identifier == 'MXDP')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				SetPortalFlags(GetPortalFlags() | kPortalShadowMapMaxDepth);
			}
			else
			{
				SetPortalFlags(GetPortalFlags() & ~kPortalShadowMapMaxDepth);
			}
		}
		else if (identifier == 'CLER')
		{
			const CheckColorSetting *checkColorSetting = static_cast<const CheckColorSetting *>(setting);
			if (checkColorSetting->GetCheckValue() != 0)
			{
				SetPortalFlags(GetPortalFlags() | kPortalOverrideClearColor);
				portalClearColor = checkColorSetting->GetColor();
			}
			else
			{
				SetPortalFlags(GetPortalFlags() & ~kPortalOverrideClearColor);
			}
		}
		else if (identifier == 'OFST')
		{
			portalPlaneOffset = FmaxZero(Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText()));
		}
		else if (identifier == 'MLEV')
		{
			minDetailLevel = static_cast<const IntegerSetting *>(setting)->GetIntegerValue();
		}
		else if (identifier == 'BIAS')
		{
			detailLevelBias = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
		}
		else if (identifier == 'DPTH')
		{
			farClipDepth = Fmax(Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText()), 1.0F);
		}
		else if (identifier == 'FMUL')
		{
			focalLengthMultiplier = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
		}
		else if (identifier == 'LKEY')
		{
			localConnectorKey = static_cast<const TextSetting *>(setting)->GetText();
		}
		else if (identifier == 'RKEY')
		{
			remoteConnectorKey = static_cast<const TextSetting *>(setting)->GetText();
		}
		else
		{
			PortalObject::SetCategorySetting(kObjectPortal, setting);
		}
	}
}


OcclusionPortalObject::OcclusionPortalObject() : PortalObject(kPortalOcclusion)
{
}

OcclusionPortalObject::OcclusionPortalObject(const Vector2D& size) : PortalObject(kPortalOcclusion, size)
{
}

OcclusionPortalObject::~OcclusionPortalObject()
{
}


Portal::Portal(PortalType type) : Node(kNodePortal)
{
	portalType = type;
	connectedZone = nullptr;
}

Portal::Portal(const Portal& portal) : Node(portal)
{
	portalType = portal.portalType;
	connectedZone = nullptr;
}

Portal::~Portal()
{
}

Portal *Portal::Create(Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (data.GetType())
	{
		case kPortalDirect:

			return (new DirectPortal);

		case kPortalRemote:

			return (new RemotePortal);

		case kPortalCamera:

			return (new CameraPortal);

		case kPortalOcclusion:

			return (new OcclusionPortal);
	}

	return (nullptr);
}

void Portal::PackType(Packer& data) const
{
	Node::PackType(data);
	data << portalType;
}

void Portal::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Node::Pack(data, packFlags);

	data << TerminatorChunk;
}

void Portal::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Node::Unpack(data, unpackFlags);
	UnpackChunkList<Portal>(data, unpackFlags);
}

bool Portal::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	return (false);
}

int32 Portal::GetCategorySettingCount(Type category) const
{
	int32 count = Node::GetCategorySettingCount(category);
	if (category == 'NODE')
	{
		count += kPerspectiveMaskSettingCount;
	}

	return (count);
}

Setting *Portal::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == 'NODE')
	{
		int32 i = index - Node::GetCategorySettingCount('NODE');
		if (i >= 0)
		{
			return (GetPerspectiveMaskSetting(i));
		}
	}

	return (Node::GetCategorySetting(category, index, flags));
}

void Portal::SetCategorySetting(Type category, const Setting *setting)
{
	if ((category != 'NODE') || (!SetPerspectiveMaskSetting(setting)))
	{
		Node::SetCategorySetting(category, setting);
	}
}

void Portal::HandleTransformUpdate(void)
{
	Node::HandleTransformUpdate();

	const Transform4D& inverse = GetInverseWorldTransform();
	worldPlane.Set(inverse(2,0), inverse(2,1), inverse(2,2), inverse(2,3));

	const PortalObject *object = GetObject();
	int32 vertexCount = object->GetVertexCount();
	const Point3D *vertex = object->GetVertexArray();

	const Transform4D& transform = GetWorldTransform();
	for (machine a = 0; a < vertexCount; a++)
	{
		worldVertex[a] = transform * vertex[a];
	}

	const Point3D *v1 = &worldVertex[vertexCount - 1];
	for (machine a = 0; a < vertexCount; a++)
	{
		const Point3D *v2 = &worldVertex[a];

		Bivector4D& edge = worldEdgeLine[a];
		edge = *v1 ^ *v2;
		edge.Standardize();

		Vector3D& inward = worldInwardDirection[a];
		inward = worldPlane.GetAntivector3D() % edge.GetTangent();
		inward.Normalize();

		v1 = v2;
	}
}

bool Portal::CalculateBoundingBox(Box3D *box) const
{
	const PortalObject *object = GetObject();
	box->Calculate(object->GetVertexCount(), object->GetVertexArray());
	return (true);
}

bool Portal::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	const PortalObject *object = GetObject();

	int32 count = object->GetVertexCount();
	if (count > 2)
	{
		const Point3D *vertex = object->GetVertexArray();

		Point3D p(0.0F, 0.0F, 0.0F);
		for (machine a = 0; a < count; a++)
		{
			p += vertex[a];
		}

		p /= (float) count;

		float r = 0.0F;
		for (machine a = 0; a < count; a++)
		{
			r = Fmax(r, SquaredMag(vertex[a] - p));
		}

		sphere->SetCenter(p);
		sphere->SetRadius(Sqrt(r));
		return (true);
	}

	return (false);
}

int32 Portal::GetInternalConnectorCount(void) const
{
	return (1);
}

const char *Portal::GetInternalConnectorKey(int32 index) const
{
	if (index == 0)
	{
		return (kConnectorKeyZone);
	}

	return (nullptr);
}

void Portal::ProcessInternalConnectors(void)
{
	connectedZone = static_cast<Zone *>(GetConnectedNode(kConnectorKeyZone));
}

bool Portal::ValidConnectedNode(const ConnectorKey& key, const Node *node) const
{
	if (key == kConnectorKeyZone)
	{
		return (node->GetNodeType() == kNodeZone);
	}

	return (Node::ValidConnectedNode(key, node));
}

void Portal::SetConnectedZone(Zone *zone)
{
	connectedZone = zone;
	SetConnectedNode(kConnectorKeyZone, zone);
}

void Portal::Preprocess(void)
{
	Node::Preprocess();

	Zone *zone = GetOwningZone();
	if (zone)
	{
		if (portalType != kPortalOcclusion)
		{
			zone->AddPortal(this);
		}
		else
		{
			zone->AddOcclusionPortal(this);
		}
	}
}

void Portal::Neutralize(void)
{
	ListElement<Portal>::Detach();
	Node::Neutralize();
}

Point3D Portal::CalculateClosestBoundaryPoint(const Point3D& p) const
{
	Vector3D	direction[kMaxPortalVertexCount];
	bool		exterior[kMaxPortalVertexCount];

	int32 vertexCount = GetObject()->GetVertexCount();

	int32 exteriorCount = 0;
	for (machine a = 0; a < vertexCount; a++)
	{
		direction[a] = p - worldVertex[a];
		float d = direction[a] * worldInwardDirection[a];
		bool b = (d < 0.0F);
		exterior[a] = b;
		exteriorCount += b;
	}

	if (exteriorCount != 0)
	{
		const Vector3D *v1 = &direction[vertexCount - 1];
		float d0 = *v1 * worldEdgeLine[vertexCount - 1].GetTangent();

		for (machine a = 0; a < vertexCount; a++)
		{
			const Vector3D *v2 = &direction[a];
			const Vector3D& tangent = worldEdgeLine[a].GetTangent();
			float d1 = *v1 * tangent;

			if (exterior[a])
			{
				float d2 = *v2 * tangent;

				if (d1 > 0.0F)
				{
					if (d2 < 0.0F)
					{
						return (p - *v1 + ProjectOnto(*v1, tangent));
					}
				}
				else
				{
					if (!(d0 < 0.0F))
					{
						return (p - *v1);
					}
				}
			}

			v1 = v2;
			d0 = d1;
		}
	}

	return (p - worldPlane.GetAntivector3D() * (worldPlane ^ p));
}

bool Portal::CalculateClosestBoundaryPoint(const Bivector4D& line, Point3D *result) const
{
	float	edgeProduct[kMaxPortalVertexCount];

	int32 vertexCount = GetObject()->GetVertexCount();

	int32 interiorCount = 0;
	for (machine a = 0; a < vertexCount; a++)
	{
		float d = line ^ worldEdgeLine[a];
		edgeProduct[a] = d;
		interiorCount += (d < 0.0F);
	}

	if (interiorCount == vertexCount)
	{
		return (false);
	}

	float closestDistance = K::infinity;
	const Point3D *v1 = &worldVertex[vertexCount - 1];
	for (machine a = 0; a < vertexCount; a++)
	{
		const Point3D *v2 = &worldVertex[a];

		float d = *v2 * line.GetTangent();
		float d2 = SquaredMag(*v2 - line.GetSupport()) - d * d;
		if (d2 < closestDistance)
		{
			closestDistance = d2;
			*result = *v2;
		}

		d = edgeProduct[a];
		if (!(d < 0.0F))
		{
			const Bivector4D& edgeLine = worldEdgeLine[a];
			const Vector3D& tangent = edgeLine.GetTangent();

			Antivector3D normal = line.GetTangent() ^ tangent;
			Point3D p = (line ^ normal ^ edgeLine).ProjectPoint3D();

			if (((p - *v1) * tangent > 0.0F) && ((p - *v2) * tangent < 0.0F))
			{
				d *= InverseMag(normal);
				d *= d;

				if (d < closestDistance)
				{
					closestDistance = d;
					*result = p;
				}
			}
		}

		v1 = v2;
	}

	return (true);
}


DirectPortal::DirectPortal() : Portal(kPortalDirect)
{
}

DirectPortal::DirectPortal(int32 count) : Portal(kPortalDirect)
{
	SetNewObject(new DirectPortalObject(count));
}

DirectPortal::DirectPortal(const Vector2D& size) : Portal(kPortalDirect)
{
	SetNewObject(new DirectPortalObject(size));
}

DirectPortal::DirectPortal(const DirectPortal& directPortal) : Portal(directPortal)
{
}

DirectPortal::~DirectPortal()
{
}

Node *DirectPortal::Replicate(void) const
{
	return (new DirectPortal(*this));
}

void DirectPortal::Enable(void)
{
	if (GetNodeFlags() & kNodeDisabled)
	{
		const Zone *zone = GetOwningZone();
		if (zone)
		{
			zone->InvalidateLightRegions();
			zone->InvalidateSourceRegions();
		}
	}

	Portal::Enable();
}

void DirectPortal::Disable(void)
{
	if (!(GetNodeFlags() & kNodeDisabled))
	{
		const Zone *zone = GetOwningZone();
		if (zone)
		{
			zone->InvalidateLightRegions();
			zone->InvalidateSourceRegions();
		}
	}

	Portal::Disable();
}

void DirectPortal::Preprocess(void)
{
	Portal::Preprocess();

	if ((!GetManipulator()) && (!GetConnectedZone()))
	{
		Zone *zone = GetOwningZone();
		if (zone)
		{
			zone = zone->GetOwningZone();
			if (zone)
			{
				SetConnectedZone(zone);
			}
		}
	}
}


RemotePortal::RemotePortal() : Portal(kPortalRemote)
{
	previousCameraWorldTransform(3,3) = 0.0F;
}

RemotePortal::RemotePortal(const Vector2D& size) : Portal(kPortalRemote)
{
	previousCameraWorldTransform(3,3) = 0.0F;

	SetNewObject(new RemotePortalObject(size));
}

RemotePortal::RemotePortal(const RemotePortal& remotePortal) : Portal(remotePortal)
{
	previousCameraWorldTransform(3,3) = 0.0F;
}

RemotePortal::~RemotePortal()
{
}

Node *RemotePortal::Replicate(void) const
{
	return (new RemotePortal(*this));
}

bool RemotePortal::CalculateRemoteTransform(Transform4D *transform) const
{
	// This is not precalculated because the connected nodes can move dynamically.

	const RemotePortalObject *object = GetObject();

	const Node *remoteNode = GetConnectedNode(object->GetRemoteConnectorKey());
	if (remoteNode)
	{
		const Node *localNode = GetConnectedNode(object->GetLocalConnectorKey());
		if (!localNode)
		{
			localNode = this;
		}

		*transform = remoteNode->GetWorldTransform() * localNode->GetInverseWorldTransform();
		return (false);
	}

	if (object->GetPortalBuffer() != kPortalBufferRefraction)
	{
		transform->SetPlaneReflection(GetInverseWorldTransform().GetRow(2));
		return (false);
	}

	transform->SetIdentity();
	return (true);
}


OcclusionPortal::OcclusionPortal() : Portal(kPortalOcclusion)
{
}

OcclusionPortal::OcclusionPortal(const Vector2D& size) : Portal(kPortalOcclusion)
{
	SetNewObject(new OcclusionPortalObject(size));
}

OcclusionPortal::OcclusionPortal(const OcclusionPortal& occlusionPortal) : Portal(occlusionPortal)
{
}

OcclusionPortal::~OcclusionPortal()
{
}

Node *OcclusionPortal::Replicate(void) const
{
	return (new OcclusionPortal(*this));
}

OcclusionRegion *OcclusionPortal::NewFrustumOcclusionRegion(const FrustumCamera *camera) const
{
	const Point3D& cameraPosition = camera->GetWorldPosition();
	const Vector3D& viewDirection = camera->GetWorldTransform()[2];

	const Antivector4D& portalPlane = GetWorldPlane();
	float distance = portalPlane ^ cameraPosition;
	if ((distance > 0.0F) && ((portalPlane ^ viewDirection) < camera->GetSineHalfField()))
	{
		Point3D		tempVertex[2][kMaxPortalVertexCount + 4];

		int32 vertexCount = GetObject()->GetVertexCount();
		const Point3D *vertex = GetWorldVertexArray();

		for (machine a = 0; a < 4; a++)
		{
			int8	location[kMaxPortalVertexCount + 4];

			Point3D *result = tempVertex[a & 1];
			Antivector4D plane(camera->GetFrustumPlaneNormal(a), cameraPosition);
			vertexCount = Math::ClipPolygon(vertexCount, vertex, plane, location, result);
			if (vertexCount == 0)
			{
				return (nullptr);
			}

			vertex = result;
		}

		Antivector4D plane = -portalPlane;
		return (new OcclusionRegion(camera, vertexCount, vertex, 1, &plane));
	}

	return (nullptr);
}

OcclusionRegion *OcclusionPortal::NewOrthoOcclusionRegion(const OrthoCamera *camera) const
{
	const Vector3D& viewDirection = camera->GetWorldTransform()[2];

	const Antivector4D& portalPlane = GetWorldPlane();
	float distance = portalPlane ^ camera->GetWorldPosition();
	if ((distance > 0.0F) && ((portalPlane ^ viewDirection) < 0.0F))
	{
		int32 vertexCount = GetObject()->GetVertexCount();
		const Point3D *vertex = GetWorldVertexArray();

		OcclusionRegion *occluder = new OcclusionRegion();
		Antivector4D *plane = occluder->GetPlaneArray();

		const Point3D *v1 = &vertex[vertexCount - 1];
		for (machine a = 0; a < vertexCount; a++)
		{
			const Point3D *v2 = &vertex[a];
			Antivector3D normal = Normalize((*v2 - *v1) % viewDirection);
			plane->Set(normal, *v1);
			plane++;
			v1 = v2;
		}

		plane[0] = -portalPlane;

		occluder->SetPlaneCount(vertexCount + 1);
		return (occluder);
	}

	return (nullptr);
}

// ZYUQURM
