 

#include "C4Modifiers.h"
#include "C4Configuration.h"
#include "C4World.h"


using namespace C4;


namespace C4
{
	template class Registrable<Modifier, ModifierRegistration>;
}


const NodeType Modifier::modifierNodeType[kModifierNodeTypeCount] =
{
	kNodeWildcard, kNodeBone, kNodeCamera, kNodeEffect, kNodeEmitter, kNodeGeometry, kNodeImpostor, kNodeInstance, kNodeLight, kNodeMarker, kNodeModel, kNodePortal, kNodeSource, kNodeSpace, kNodeTrigger, kNodeZone
};


ModifierRegistration::ModifierRegistration(ModifierType type, const char *name) : Registration<Modifier, ModifierRegistration>(type)
{
	modifierName = name;
}

ModifierRegistration::~ModifierRegistration()
{
}


Modifier::Modifier(ModifierType type)
{
	modifierType = type;
}

Modifier::Modifier(const Modifier& modifier)
{
	modifierType = modifier.modifierType;
}

Modifier::~Modifier()
{
}

Modifier *Modifier::Replicate(void) const
{
	return (nullptr);
}

Modifier *Modifier::New(ModifierType type)
{
	Type	data[2];

	data[0] = type;
	data[1] = 0;

	Unpacker unpacker(data);
	return (Create(unpacker));
}

Modifier *Modifier::CreateUnknown(Unpacker& data, unsigned_int32 unpackFlags)
{
	return (new UnknownModifier(data.GetType()));
}

bool Modifier::ValidInstance(const Instance *instance)
{
	return (true);
}

void Modifier::RegisterStandardModifiers(void)
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	static ModifierReg<UnknownModifier> unknownRegistration(kModifierUnknown, table->GetString(StringID('MDFR', kModifierUnknown)));
	static ModifierReg<AugmentInstanceModifier> augmentInstanceRegistration(kModifierAugmentInstance, table->GetString(StringID('MDFR', kModifierAugmentInstance)));
	static ModifierReg<WakeControllerModifier> wakeControllerRegistration(kModifierWakeController, table->GetString(StringID('MDFR', kModifierWakeController)));
	static ModifierReg<SleepControllerModifier> sleepControllerRegistration(kModifierSleepController, table->GetString(StringID('MDFR', kModifierSleepController)));
	static ModifierReg<ConnectInstanceModifier> connectInstanceRegistration(kModifierConnectInstance, table->GetString(StringID('MDFR', kModifierConnectInstance)));
	static ModifierReg<MoveConnectorInsideModifier> moveConnectorInsideRegistration(kModifierMoveConnectorInside, table->GetString(StringID('MDFR', kModifierMoveConnectorInside)));
	static ModifierReg<MoveConnectorOutsideModifier> moveConnectorOutsideRegistration(kModifierMoveConnectorOutside, table->GetString(StringID('MDFR', kModifierMoveConnectorOutside)));
	static ModifierReg<EnableNodesModifier> enableNodesRegistration(kModifierEnableNodes, table->GetString(StringID('MDFR', kModifierEnableNodes)));
	static ModifierReg<DisableNodesModifier> disableNodesRegistration(kModifierDisableNodes, table->GetString(StringID('MDFR', kModifierDisableNodes)));
	static ModifierReg<DeleteNodesModifier> deleteNodesRegistration(kModifierDeleteNodes, table->GetString(StringID('MDFR', kModifierDeleteNodes)));
	static ModifierReg<EnableInteractivityModifier> enableInteractivityRegistration(kModifierEnableInteractivity, table->GetString(StringID('MDFR', kModifierEnableInteractivity)));
	static ModifierReg<DisableInteractivityModifier> disableInteractivityRegistration(kModifierDisableInteractivity, table->GetString(StringID('MDFR', kModifierDisableInteractivity)));
	static ModifierReg<SetPerspectiveMaskModifier> setPerspectiveMaskRegistration(kModifierSetPerspectiveMask, table->GetString(StringID('MDFR', kModifierSetPerspectiveMask)));
	static ModifierReg<ReplaceMaterialModifier> replaceMaterialRegistration(kModifierReplaceMaterial, table->GetString(StringID('MDFR', kModifierReplaceMaterial)));
	static ModifierReg<RemovePhysicsModifier> removePhysicsRegistration(kModifierRemovePhysics, table->GetString(StringID('MDFR', kModifierRemovePhysics)));

	SetUnknownCreator(&CreateUnknown);
}

void Modifier::PackType(Packer& data) const
{
	data << modifierType;
}

Setting *Modifier::GetNodeTypeSetting(NodeType type)
{
	int32 selection = 0; 
	for (machine a = 1; a < kModifierNodeTypeCount; a++)
	{
		if (type == modifierNodeType[a]) 
		{
			selection = a; 
			break;
		}
	} 

	const StringTable *table = TheInterfaceMgr->GetStringTable(); 
 
	const char *title = table->GetString(StringID('MDFR', 'TYPE'));
	MenuSetting *menu = new MenuSetting('TYPE', selection, title, kModifierNodeTypeCount);

	menu->SetMenuItemString(0, table->GetString(StringID('MDFR', 'TYPE', 'NONE'))); 
	for (machine a = 1; a < kModifierNodeTypeCount; a++)
	{
		menu->SetMenuItemString(a, table->GetString(StringID('MDFR', 'TYPE', modifierNodeType[a])));
	}

	return (menu);
}

bool Modifier::operator ==(const Modifier& modifier) const
{
	return (false);
}

void Modifier::Apply(World *world, Instance *instance)
{
}

bool Modifier::KeepNode(const Node *node) const
{
	return (true);
}


UnknownModifier::UnknownModifier() : Modifier(kModifierUnknown)
{
	unknownType = 0;
	unknownSize = 0;
	unknownData = nullptr;
}

UnknownModifier::UnknownModifier(ModifierType type) : Modifier(kModifierUnknown)
{
	unknownType = type;
	unknownSize = 0;
	unknownData = nullptr;
}

UnknownModifier::UnknownModifier(const UnknownModifier& unknownModifier) : Modifier(unknownModifier)
{
	unknownType = unknownModifier.unknownType;
	unknownSize = unknownModifier.unknownSize;
	unknownData = new char[unknownSize];
	MemoryMgr::CopyMemory(unknownModifier.unknownData, unknownData, unknownSize);
}

UnknownModifier::~UnknownModifier()
{
	delete[] unknownData;
}

Modifier *UnknownModifier::Replicate(void) const
{
	return (new UnknownModifier(*this));
}

bool UnknownModifier::ValidInstance(const Instance *instance)
{
	return (false);
}

void UnknownModifier::PackType(Packer& data) const
{
	data << unknownType;
}

void UnknownModifier::Pack(Packer& data, unsigned_int32 packFlags) const
{
	data.WriteData(unknownData, unknownSize);
}

void UnknownModifier::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	unsigned_int32 size = data.GetUnknownSize() - sizeof(ModifierType);
	unknownSize = size;
	unknownData = new char[size];
	data.ReadData(unknownData, size);
}

int32 UnknownModifier::GetSettingCount(void) const
{
	return (1);
}

Setting *UnknownModifier::GetSetting(int32 index) const
{
	if (index == 0)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();
		const char *title = table->GetString(StringID('MDFR', kModifierUnknown, 'TYPE'));
		return (new InfoSetting('TYPE', Text::TypeToHexCharString(unknownType), title));
	}

	return (nullptr);
}

void UnknownModifier::SetSetting(const Setting *setting)
{
}


AugmentInstanceModifier::AugmentInstanceModifier() : Modifier(kModifierAugmentInstance)
{
	worldName[0] = 0;
}

AugmentInstanceModifier::AugmentInstanceModifier(const char *name) : Modifier(kModifierAugmentInstance)
{
	worldName = name;
}

AugmentInstanceModifier::AugmentInstanceModifier(const AugmentInstanceModifier& augmentInstanceModifier) : Modifier(augmentInstanceModifier)
{
	worldName = augmentInstanceModifier.worldName;
}

AugmentInstanceModifier::~AugmentInstanceModifier()
{
}

Modifier *AugmentInstanceModifier::Replicate(void) const
{
	return (new AugmentInstanceModifier(*this));
}

void AugmentInstanceModifier::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Modifier::Pack(data, packFlags);

	PackHandle handle = data.BeginChunk('WRLD');
	data << worldName;
	data.EndChunk(handle);

	data << TerminatorChunk;
}

void AugmentInstanceModifier::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Modifier::Unpack(data, unpackFlags);
	UnpackChunkList<AugmentInstanceModifier>(data, unpackFlags);
}

bool AugmentInstanceModifier::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'WRLD':

			data >> worldName;
			return (true);
	}

	return (false);
}

int32 AugmentInstanceModifier::GetSettingCount(void) const
{
	return (1);
}

Setting *AugmentInstanceModifier::GetSetting(int32 index) const
{
	if (index == 0)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();

		const char *title = table->GetString(StringID('MDFR', kModifierAugmentInstance, 'WRLD'));
		const char *picker = table->GetString(StringID('MDFR', kModifierAugmentInstance, 'PICK'));
		return (new ResourceSetting('WRLD', worldName, title, picker, WorldResource::GetDescriptor()));
	}

	return (nullptr);
}

void AugmentInstanceModifier::SetSetting(const Setting *setting)
{
	if (setting->GetSettingIdentifier() == 'WRLD')
	{
		worldName = static_cast<const ResourceSetting *>(setting)->GetResourceName();
	}
}

bool AugmentInstanceModifier::operator ==(const Modifier& modifier) const
{
	if (modifier.GetModifierType() == kModifierAugmentInstance)
	{
		const AugmentInstanceModifier *augmentInstanceModifier = static_cast<const AugmentInstanceModifier *>(&modifier);

		if (augmentInstanceModifier->worldName == worldName)
		{
			return (true);
		}
	}

	return (false);
}

void AugmentInstanceModifier::Apply(World *world, Instance *instance)
{
	Node *worldRoot = world->NewInstancedWorld(worldName);
	if (worldRoot)
	{
		Node *root = instance;

		Node *node = instance->GetFirstSubnode();
		while (node)
		{
			const Controller *controller = node->GetController();
			if ((controller) && (controller->GetBaseControllerType() == kControllerRigidBody))
			{
				root = node;
				break;
			}

			node = node->Next();
		}

		for (;;)
		{
			node = worldRoot->GetFirstSubnode();
			if (!node)
			{
				break;
			}

			root->AppendSubnode(node);
		}

		delete worldRoot;
	}
}


WakeControllerModifier::WakeControllerModifier() : Modifier(kModifierWakeController)
{
}

WakeControllerModifier::WakeControllerModifier(const WakeControllerModifier& wakeControllerModifier) : Modifier(wakeControllerModifier)
{
}

WakeControllerModifier::~WakeControllerModifier()
{
}

Modifier *WakeControllerModifier::Replicate(void) const
{
	return (new WakeControllerModifier(*this));
}

bool WakeControllerModifier::operator ==(const Modifier& modifier) const
{
	if (modifier.GetModifierType() == kModifierWakeController)
	{
		return (true);
	}

	return (false);
}

void WakeControllerModifier::Apply(World *world, Instance *instance)
{
	const Node *node = instance->GetFirstSubnode();
	while (node)
	{
		Controller *controller = node->GetController();
		if (controller)
		{
			controller->SetControllerFlags(controller->GetControllerFlags() & ~kControllerAsleep);
		}

		node = node->Next();
	}
}


SleepControllerModifier::SleepControllerModifier() : Modifier(kModifierSleepController)
{
}

SleepControllerModifier::SleepControllerModifier(const SleepControllerModifier& sleepControllerModifier) : Modifier(sleepControllerModifier)
{
}

SleepControllerModifier::~SleepControllerModifier()
{
}

Modifier *SleepControllerModifier::Replicate(void) const
{
	return (new SleepControllerModifier(*this));
}

bool SleepControllerModifier::operator ==(const Modifier& modifier) const
{
	if (modifier.GetModifierType() == kModifierSleepController)
	{
		return (true);
	}

	return (false);
}

void SleepControllerModifier::Apply(World *world, Instance *instance)
{
	const Node *node = instance->GetFirstSubnode();
	while (node)
	{
		Controller *controller = node->GetController();
		if (controller)
		{
			controller->SetControllerFlags(controller->GetControllerFlags() | kControllerAsleep);
		}

		node = node->Next();
	}
}


ConnectInstanceModifier::ConnectInstanceModifier() : Modifier(kModifierConnectInstance)
{
	connectorKey[0] = 0;
	targetNodeName[0] = 0;
}

ConnectInstanceModifier::ConnectInstanceModifier(const char *key, const char *name) : Modifier(kModifierConnectInstance)
{
	connectorKey = key;
	targetNodeName = name;
}

ConnectInstanceModifier::ConnectInstanceModifier(const ConnectInstanceModifier& connectInstanceModifier) : Modifier(connectInstanceModifier)
{
	connectorKey = connectInstanceModifier.connectorKey;
	targetNodeName = connectInstanceModifier.targetNodeName;
}

ConnectInstanceModifier::~ConnectInstanceModifier()
{
}

Modifier *ConnectInstanceModifier::Replicate(void) const
{
	return (new ConnectInstanceModifier(*this));
}

void ConnectInstanceModifier::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Modifier::Pack(data, packFlags);

	PackHandle handle = data.BeginChunk('CONN');
	data << connectorKey;
	data.EndChunk(handle);

	handle = data.BeginChunk('NAME');
	data << targetNodeName;
	data.EndChunk(handle);

	data << TerminatorChunk;
}

void ConnectInstanceModifier::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Modifier::Unpack(data, unpackFlags);
	UnpackChunkList<ConnectInstanceModifier>(data, unpackFlags);
}

bool ConnectInstanceModifier::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'CONN':

			data >> connectorKey;
			return (true);

		case 'NAME':

			data >> targetNodeName;
			return (true);
	}

	return (false);
}

int32 ConnectInstanceModifier::GetSettingCount(void) const
{
	return (2);
}

Setting *ConnectInstanceModifier::GetSetting(int32 index) const
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('MDFR', kModifierConnectInstance, 'CONN'));
		return (new TextSetting('CONN', connectorKey, title, kMaxConnectorKeyLength, &Connector::ConnectorKeyFilter));
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('MDFR', kModifierConnectInstance, 'NAME'));
		return (new TextSetting('NAME', targetNodeName, title, kMaxModifierNodeNameLength));
	}

	return (nullptr);
}

void ConnectInstanceModifier::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'CONN')
	{
		connectorKey = static_cast<const TextSetting *>(setting)->GetText();
	}
	else if (identifier == 'NAME')
	{
		targetNodeName = static_cast<const TextSetting *>(setting)->GetText();
	}
}

bool ConnectInstanceModifier::operator ==(const Modifier& modifier) const
{
	if (modifier.GetModifierType() == kModifierConnectInstance)
	{
		const ConnectInstanceModifier *connectInstanceModifier = static_cast<const ConnectInstanceModifier *>(&modifier);

		if (connectInstanceModifier->connectorKey != connectorKey)
		{
			return (false);
		}

		if (connectInstanceModifier->targetNodeName == targetNodeName)
		{
			return (true);
		}
	}

	return (false);
}

void ConnectInstanceModifier::Apply(World *world, Instance *instance)
{
	if (!instance->GetManipulator())
	{
		const Hub *hub = instance->GetHub();
		if (hub)
		{
			unsigned_int32 hash = Text::Hash(targetNodeName);

			Connector *connector = hub->GetFirstOutgoingEdge();
			while (connector)
			{
				if (connector->GetConnectorKey() == connectorKey)
				{
					Node *node = instance->GetFirstSubnode();
					while (node)
					{
						if (node->GetNodeHash() == hash)
						{
							connector->SetConnectorTarget(node);
							break;
						}

						node = instance->GetNextNode(node);
					}

					break;
				}

				connector = connector->GetNextOutgoingEdge();
			}
		}
	}
}


MoveConnectorInsideModifier::MoveConnectorInsideModifier() : Modifier(kModifierMoveConnectorInside)
{
	incomingConnectorKey[0] = 0;
	targetNodeName[0] = 0;
}

MoveConnectorInsideModifier::MoveConnectorInsideModifier(const char *incomingKey, const char *targetName) : Modifier(kModifierMoveConnectorInside)
{
	incomingConnectorKey = incomingKey;
	targetNodeName = targetName;
}

MoveConnectorInsideModifier::MoveConnectorInsideModifier(const MoveConnectorInsideModifier& moveConnectorInsideModifier) : Modifier(moveConnectorInsideModifier)
{
	incomingConnectorKey = moveConnectorInsideModifier.incomingConnectorKey;
	targetNodeName = moveConnectorInsideModifier.targetNodeName;
}

MoveConnectorInsideModifier::~MoveConnectorInsideModifier()
{
}

Modifier *MoveConnectorInsideModifier::Replicate(void) const
{
	return (new MoveConnectorInsideModifier(*this));
}

void MoveConnectorInsideModifier::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Modifier::Pack(data, packFlags);

	PackHandle handle = data.BeginChunk('CONN');
	data << incomingConnectorKey;
	data.EndChunk(handle);

	handle = data.BeginChunk('NAME');
	data << targetNodeName;
	data.EndChunk(handle);

	data << TerminatorChunk;
}

void MoveConnectorInsideModifier::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Modifier::Unpack(data, unpackFlags);
	UnpackChunkList<MoveConnectorInsideModifier>(data, unpackFlags);
}

bool MoveConnectorInsideModifier::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'CONN':

			data >> incomingConnectorKey;
			return (true);

		case 'NAME':

			data >> targetNodeName;
			return (true);
	}

	return (false);
}

int32 MoveConnectorInsideModifier::GetSettingCount(void) const
{
	return (2);
}

Setting *MoveConnectorInsideModifier::GetSetting(int32 index) const
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('MDFR', kModifierMoveConnectorInside, 'CONN'));
		return (new TextSetting('CONN', incomingConnectorKey, title, kMaxConnectorKeyLength, &Connector::ConnectorKeyFilter));
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('MDFR', kModifierMoveConnectorInside, 'NAME'));
		return (new TextSetting('NAME', targetNodeName, title, kMaxModifierNodeNameLength));
	}

	return (nullptr);
}

void MoveConnectorInsideModifier::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'CONN')
	{
		incomingConnectorKey = static_cast<const TextSetting *>(setting)->GetText();
	}
	else if (identifier == 'NAME')
	{
		targetNodeName = static_cast<const TextSetting *>(setting)->GetText();
	}
}

bool MoveConnectorInsideModifier::operator ==(const Modifier& modifier) const
{
	if (modifier.GetModifierType() == kModifierMoveConnectorInside)
	{
		const MoveConnectorInsideModifier *moveConnectorInsideModifier = static_cast<const MoveConnectorInsideModifier *>(&modifier);

		if (moveConnectorInsideModifier->incomingConnectorKey != incomingConnectorKey)
		{
			return (false);
		}

		if (moveConnectorInsideModifier->targetNodeName == targetNodeName)
		{
			return (true);
		}
	}

	return (false);
}

void MoveConnectorInsideModifier::Apply(World *world, Instance *instance)
{
	if (!instance->GetManipulator())
	{
		const Hub *hub = instance->GetHub();
		if (hub)
		{
			unsigned_int32 hash = Text::Hash(targetNodeName);

			Connector *connector = hub->GetFirstIncomingEdge();
			while (connector)
			{
				Connector *next = connector->GetNextIncomingEdge();

				if (connector->GetConnectorKey() == incomingConnectorKey)
				{
					Node *node = instance->GetFirstSubnode();
					while (node)
					{
						if (node->GetNodeHash() == hash)
						{
							connector->SetConnectorTarget(node);
							connector->SetConnectorFlags(kConnectorSaveFinishPersistent);
							break;
						}

						node = instance->GetNextNode(node);
					}
				}

				connector = next;
			}
		}
	}
}


MoveConnectorOutsideModifier::MoveConnectorOutsideModifier() : Modifier(kModifierMoveConnectorOutside)
{
	incomingConnectorKey[0] = 0;
	targetConnectorKey[0] = 0;
}

MoveConnectorOutsideModifier::MoveConnectorOutsideModifier(const char *incomingKey, const char *targetKey) : Modifier(kModifierMoveConnectorOutside)
{
	incomingConnectorKey = incomingKey;
	targetConnectorKey = targetKey;
}

MoveConnectorOutsideModifier::MoveConnectorOutsideModifier(const MoveConnectorOutsideModifier& moveConnectorOutsideModifier) : Modifier(moveConnectorOutsideModifier)
{
	incomingConnectorKey = moveConnectorOutsideModifier.incomingConnectorKey;
	targetConnectorKey = moveConnectorOutsideModifier.targetConnectorKey;
}

MoveConnectorOutsideModifier::~MoveConnectorOutsideModifier()
{
}

Modifier *MoveConnectorOutsideModifier::Replicate(void) const
{
	return (new MoveConnectorOutsideModifier(*this));
}

void MoveConnectorOutsideModifier::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Modifier::Pack(data, packFlags);

	PackHandle handle = data.BeginChunk('CONN');
	data << incomingConnectorKey;
	data.EndChunk(handle);

	handle = data.BeginChunk('TARG');
	data << targetConnectorKey;
	data.EndChunk(handle);

	data << TerminatorChunk;
}

void MoveConnectorOutsideModifier::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Modifier::Unpack(data, unpackFlags);
	UnpackChunkList<MoveConnectorOutsideModifier>(data, unpackFlags);
}

bool MoveConnectorOutsideModifier::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'CONN':

			data >> incomingConnectorKey;
			return (true);

		case 'TARG':

			data >> targetConnectorKey;
			return (true);
	}

	return (false);
}

int32 MoveConnectorOutsideModifier::GetSettingCount(void) const
{
	return (2);
}

Setting *MoveConnectorOutsideModifier::GetSetting(int32 index) const
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('MDFR', kModifierMoveConnectorOutside, 'CONN'));
		return (new TextSetting('CONN', incomingConnectorKey, title, kMaxConnectorKeyLength, &Connector::ConnectorKeyFilter));
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('MDFR', kModifierMoveConnectorOutside, 'TARG'));
		return (new TextSetting('TARG', targetConnectorKey, title, kMaxConnectorKeyLength, &Connector::ConnectorKeyFilter));
	}

	return (nullptr);
}

void MoveConnectorOutsideModifier::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'CONN')
	{
		incomingConnectorKey = static_cast<const TextSetting *>(setting)->GetText();
	}
	else if (identifier == 'TARG')
	{
		targetConnectorKey = static_cast<const TextSetting *>(setting)->GetText();
	}
}

bool MoveConnectorOutsideModifier::operator ==(const Modifier& modifier) const
{
	if (modifier.GetModifierType() == kModifierMoveConnectorOutside)
	{
		const MoveConnectorOutsideModifier *moveConnectorOutsideModifier = static_cast<const MoveConnectorOutsideModifier *>(&modifier);

		if (moveConnectorOutsideModifier->incomingConnectorKey != incomingConnectorKey)
		{
			return (false);
		}

		if (moveConnectorOutsideModifier->targetConnectorKey == targetConnectorKey)
		{
			return (true);
		}
	}

	return (false);
}

void MoveConnectorOutsideModifier::Apply(World *world, Instance *instance)
{
	if (!instance->GetManipulator())
	{
		Node *targetNode = instance->GetConnectedNode(targetConnectorKey);
		if (targetNode)
		{
			const Hub *hub = instance->GetHub();
			Connector *connector = hub->GetFirstIncomingEdge();
			while (connector)
			{
				Connector *next = connector->GetNextIncomingEdge();

				if (connector->GetConnectorKey() == incomingConnectorKey)
				{
					connector->SetConnectorTarget(targetNode);
					connector->SetConnectorFlags(kConnectorSaveFinishPersistent);
				}

				connector = next;
			}
		}
	}
}


EnableNodesModifier::EnableNodesModifier() : Modifier(kModifierEnableNodes)
{
	nodeType = kNodeWildcard;
	nodeHash = 0;
	nodeName[0] = 0;
}

EnableNodesModifier::EnableNodesModifier(NodeType type, const char *name) : Modifier(kModifierEnableNodes)
{
	nodeType = type;
	nodeName = name;
	nodeHash = Text::Hash(name);
}

EnableNodesModifier::EnableNodesModifier(const EnableNodesModifier& enableNodesModifier) : Modifier(enableNodesModifier)
{
	nodeType = enableNodesModifier.nodeType;
	nodeName = enableNodesModifier.nodeName;
	nodeHash = Text::Hash(nodeName);
}

EnableNodesModifier::~EnableNodesModifier()
{
}

Modifier *EnableNodesModifier::Replicate(void) const
{
	return (new EnableNodesModifier(*this));
}

void EnableNodesModifier::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Modifier::Pack(data, packFlags);

	data << ChunkHeader('TYPE', 4);
	data << nodeType;

	PackHandle handle = data.BeginChunk('NAME');
	data << nodeName;
	data.EndChunk(handle);

	data << TerminatorChunk;
}

void EnableNodesModifier::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Modifier::Unpack(data, unpackFlags);
	UnpackChunkList<EnableNodesModifier>(data, unpackFlags);
}

bool EnableNodesModifier::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'TYPE':

			data >> nodeType;
			return (true);

		case 'NAME':

			data >> nodeName;
			nodeHash = Text::Hash(nodeName);
			return (true);
	}

	return (false);
}

int32 EnableNodesModifier::GetSettingCount(void) const
{
	return (2);
}

Setting *EnableNodesModifier::GetSetting(int32 index) const
{
	if (index == 0)
	{
		return (GetNodeTypeSetting(nodeType));
	}

	if (index == 1)
	{
		const char *title = TheInterfaceMgr->GetStringTable()->GetString(StringID('MDFR', 'NAME'));
		return (new TextSetting('NAME', nodeName, title, kMaxModifierNodeNameLength));
	}

	return (nullptr);
}

void EnableNodesModifier::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'TYPE')
	{
		nodeType = modifierNodeType[static_cast<const MenuSetting *>(setting)->GetMenuSelection()];
	}
	else if (identifier == 'NAME')
	{
		nodeName = static_cast<const TextSetting *>(setting)->GetText();
		nodeHash = Text::Hash(nodeName);
	}
}

void EnableNodesModifier::SetNodeName(const char *name)
{
	nodeName = name;
	nodeHash = Text::Hash(nodeName);
}

bool EnableNodesModifier::operator ==(const Modifier& modifier) const
{
	if (modifier.GetModifierType() == kModifierEnableNodes)
	{
		const EnableNodesModifier *enableNodesModifier = static_cast<const EnableNodesModifier *>(&modifier);

		if (enableNodesModifier->nodeType != nodeType)
		{
			return (false);
		}

		if (enableNodesModifier->nodeHash == nodeHash)
		{
			return (true);
		}
	}

	return (false);
}

void EnableNodesModifier::Apply(World *world, Instance *instance)
{
	NodeType type = nodeType;
	unsigned_int32 hash = nodeHash;

	Node *node = instance->GetFirstSubnode();
	while (node)
	{
		if ((type == kNodeWildcard) || (node->GetNodeType() == type))
		{
			if ((hash == 0) || (node->GetNodeHash() == hash))
			{
				node->Enable();
				node = instance->GetNextLevelNode(node);
				continue;
			}
		}

		node = instance->GetNextNode(node);
	}
}


DisableNodesModifier::DisableNodesModifier() : Modifier(kModifierDisableNodes)
{
	nodeType = kNodeWildcard;
	nodeHash = 0;
	nodeName[0] = 0;
}

DisableNodesModifier::DisableNodesModifier(NodeType type, const char *name) : Modifier(kModifierDisableNodes)
{
	nodeType = type;
	nodeName = name;
	nodeHash = Text::Hash(name);
}

DisableNodesModifier::DisableNodesModifier(const DisableNodesModifier& disableNodesModifier) : Modifier(disableNodesModifier)
{
	nodeType = disableNodesModifier.nodeType;
	nodeName = disableNodesModifier.nodeName;
	nodeHash = Text::Hash(nodeName);
}

DisableNodesModifier::~DisableNodesModifier()
{
}

Modifier *DisableNodesModifier::Replicate(void) const
{
	return (new DisableNodesModifier(*this));
}

void DisableNodesModifier::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Modifier::Pack(data, packFlags);

	data << ChunkHeader('TYPE', 4);
	data << nodeType;

	PackHandle handle = data.BeginChunk('NAME');
	data << nodeName;
	data.EndChunk(handle);

	data << TerminatorChunk;
}

void DisableNodesModifier::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Modifier::Unpack(data, unpackFlags);
	UnpackChunkList<DisableNodesModifier>(data, unpackFlags);
}

bool DisableNodesModifier::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'TYPE':

			data >> nodeType;
			return (true);

		case 'NAME':

			data >> nodeName;
			nodeHash = Text::Hash(nodeName);
			return (true);
	}

	return (false);
}

int32 DisableNodesModifier::GetSettingCount(void) const
{
	return (2);
}

Setting *DisableNodesModifier::GetSetting(int32 index) const
{
	if (index == 0)
	{
		return (GetNodeTypeSetting(nodeType));
	}

	if (index == 1)
	{
		const char *title = TheInterfaceMgr->GetStringTable()->GetString(StringID('MDFR', 'NAME'));
		return (new TextSetting('NAME', nodeName, title, kMaxModifierNodeNameLength));
	}

	return (nullptr);
}

void DisableNodesModifier::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'TYPE')
	{
		nodeType = modifierNodeType[static_cast<const MenuSetting *>(setting)->GetMenuSelection()];
	}
	else if (identifier == 'NAME')
	{
		nodeName = static_cast<const TextSetting *>(setting)->GetText();
		nodeHash = Text::Hash(nodeName);
	}
}

void DisableNodesModifier::SetNodeName(const char *name)
{
	nodeName = name;
	nodeHash = Text::Hash(nodeName);
}

bool DisableNodesModifier::operator ==(const Modifier& modifier) const
{
	if (modifier.GetModifierType() == kModifierDisableNodes)
	{
		const DisableNodesModifier *disableNodesModifier = static_cast<const DisableNodesModifier *>(&modifier);

		if (disableNodesModifier->nodeType != nodeType)
		{
			return (false);
		}

		if (disableNodesModifier->nodeHash == nodeHash)
		{
			return (true);
		}
	}

	return (false);
}

void DisableNodesModifier::Apply(World *world, Instance *instance)
{
	NodeType type = nodeType;
	unsigned_int32 hash = nodeHash;

	Node *node = instance->GetFirstSubnode();
	while (node)
	{
		if ((type == kNodeWildcard) || (node->GetNodeType() == type))
		{
			if ((hash == 0) || (node->GetNodeHash() == hash))
			{
				node->Disable();
				node = instance->GetNextLevelNode(node);
				continue;
			}
		}

		node = instance->GetNextNode(node);
	}
}


DeleteNodesModifier::DeleteNodesModifier() : Modifier(kModifierDeleteNodes)
{
	nodeType = kNodeWildcard;
	nodeHash = 0;
	nodeName[0] = 0;
}

DeleteNodesModifier::DeleteNodesModifier(NodeType type, const char *name) : Modifier(kModifierDeleteNodes)
{
	nodeType = type;
	nodeName = name;
	nodeHash = Text::Hash(name);
}

DeleteNodesModifier::DeleteNodesModifier(const DeleteNodesModifier& deleteNodesModifier) : Modifier(deleteNodesModifier)
{
	nodeType = deleteNodesModifier.nodeType;
	nodeName = deleteNodesModifier.nodeName;
	nodeHash = Text::Hash(nodeName);
}

DeleteNodesModifier::~DeleteNodesModifier()
{
}

Modifier *DeleteNodesModifier::Replicate(void) const
{
	return (new DeleteNodesModifier(*this));
}

void DeleteNodesModifier::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Modifier::Pack(data, packFlags);

	data << ChunkHeader('TYPE', 4);
	data << nodeType;

	PackHandle handle = data.BeginChunk('NAME');
	data << nodeName;
	data.EndChunk(handle);

	data << TerminatorChunk;
}

void DeleteNodesModifier::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Modifier::Unpack(data, unpackFlags);
	UnpackChunkList<DeleteNodesModifier>(data, unpackFlags);
}

bool DeleteNodesModifier::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'TYPE':

			data >> nodeType;
			return (true);

		case 'NAME':

			data >> nodeName;
			nodeHash = Text::Hash(nodeName);
			return (true);
	}

	return (false);
}

int32 DeleteNodesModifier::GetSettingCount(void) const
{
	return (2);
}

Setting *DeleteNodesModifier::GetSetting(int32 index) const
{
	if (index == 0)
	{
		return (GetNodeTypeSetting(nodeType));
	}

	if (index == 1)
	{
		const char *title = TheInterfaceMgr->GetStringTable()->GetString(StringID('MDFR', 'NAME'));
		return (new TextSetting('NAME', nodeName, title, kMaxModifierNodeNameLength));
	}

	return (nullptr);
}

void DeleteNodesModifier::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'TYPE')
	{
		nodeType = modifierNodeType[static_cast<const MenuSetting *>(setting)->GetMenuSelection()];
	}
	else if (identifier == 'NAME')
	{
		nodeName = static_cast<const TextSetting *>(setting)->GetText();
		nodeHash = Text::Hash(nodeName);
	}
}

void DeleteNodesModifier::SetNodeName(const char *name)
{
	nodeName = name;
	nodeHash = Text::Hash(nodeName);
}

bool DeleteNodesModifier::operator ==(const Modifier& modifier) const
{
	if (modifier.GetModifierType() == kModifierDeleteNodes)
	{
		const DeleteNodesModifier *deleteNodesModifier = static_cast<const DeleteNodesModifier *>(&modifier);

		if (deleteNodesModifier->nodeType != nodeType)
		{
			return (false);
		}

		if (deleteNodesModifier->nodeHash == nodeHash)
		{
			return (true);
		}
	}

	return (false);
}

bool DeleteNodesModifier::KeepNode(const Node *node) const
{
	NodeType type = nodeType;
	if ((type == kNodeWildcard) || (node->GetNodeType() == type))
	{
		unsigned_int32 hash = nodeHash;
		return ((hash != 0) && (node->GetNodeHash() != hash));
	}

	return (true);
}


EnableInteractivityModifier::EnableInteractivityModifier() : Modifier(kModifierEnableInteractivity)
{
	nodeHash = 0;
	nodeName[0] = 0;
}

EnableInteractivityModifier::EnableInteractivityModifier(const char *name) : Modifier(kModifierEnableInteractivity)
{
	nodeName = name;
	nodeHash = Text::Hash(name);
}

EnableInteractivityModifier::EnableInteractivityModifier(const EnableInteractivityModifier& enableInteractivityModifier) : Modifier(enableInteractivityModifier)
{
	nodeName = enableInteractivityModifier.nodeName;
	nodeHash = Text::Hash(nodeName);
}

EnableInteractivityModifier::~EnableInteractivityModifier()
{
}

Modifier *EnableInteractivityModifier::Replicate(void) const
{
	return (new EnableInteractivityModifier(*this));
}

void EnableInteractivityModifier::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Modifier::Pack(data, packFlags);

	PackHandle handle = data.BeginChunk('NAME');
	data << nodeName;
	data.EndChunk(handle);

	data << TerminatorChunk;
}

void EnableInteractivityModifier::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Modifier::Unpack(data, unpackFlags);
	UnpackChunkList<EnableInteractivityModifier>(data, unpackFlags);
}

bool EnableInteractivityModifier::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'NAME':

			data >> nodeName;
			nodeHash = Text::Hash(nodeName);
			return (true);
	}

	return (false);
}

int32 EnableInteractivityModifier::GetSettingCount(void) const
{
	return (1);
}

Setting *EnableInteractivityModifier::GetSetting(int32 index) const
{
	if (index == 0)
	{
		const char *title = TheInterfaceMgr->GetStringTable()->GetString(StringID('MDFR', 'NAME'));
		return (new TextSetting('NAME', nodeName, title, kMaxModifierNodeNameLength));
	}

	return (nullptr);
}

void EnableInteractivityModifier::SetSetting(const Setting *setting)
{
	if (setting->GetSettingIdentifier() == 'NAME')
	{
		nodeName = static_cast<const TextSetting *>(setting)->GetText();
		nodeHash = Text::Hash(nodeName);
	}
}

void EnableInteractivityModifier::SetNodeName(const char *name)
{
	nodeName = name;
	nodeHash = Text::Hash(nodeName);
}

bool EnableInteractivityModifier::operator ==(const Modifier& modifier) const
{
	if (modifier.GetModifierType() == kModifierEnableInteractivity)
	{
		const EnableInteractivityModifier *enableInteractivityModifier = static_cast<const EnableInteractivityModifier *>(&modifier);

		if (enableInteractivityModifier->nodeHash == nodeHash)
		{
			return (true);
		}
	}

	return (false);
}

void EnableInteractivityModifier::Apply(World *world, Instance *instance)
{
	if (nodeHash == 0)
	{
		Node *node = instance->GetFirstSubnode();
		while (node)
		{
			Property *property = node->GetProperty(kPropertyInteraction);
			if (property)
			{
				property->SetPropertyFlags(property->GetPropertyFlags() & ~kPropertyDisabled);
			}

			node = instance->GetNextNode(node);
		}
	}
	else
	{
		Node *node = instance->GetFirstSubnode();
		while (node)
		{
			if (node->GetNodeHash() == nodeHash)
			{
				Property *property = node->GetProperty(kPropertyInteraction);
				if (property)
				{
					property->SetPropertyFlags(property->GetPropertyFlags() & ~kPropertyDisabled);
				}
			}

			node = instance->GetNextNode(node);
		}
	}
}


DisableInteractivityModifier::DisableInteractivityModifier() : Modifier(kModifierDisableInteractivity)
{
	nodeHash = 0;
	nodeName[0] = 0;
}

DisableInteractivityModifier::DisableInteractivityModifier(const char *name) : Modifier(kModifierDisableInteractivity)
{
	nodeName = name;
	nodeHash = Text::Hash(name);
}

DisableInteractivityModifier::DisableInteractivityModifier(const DisableInteractivityModifier& disableInteractivityModifier) : Modifier(disableInteractivityModifier)
{
	nodeName = disableInteractivityModifier.nodeName;
	nodeHash = Text::Hash(nodeName);
}

DisableInteractivityModifier::~DisableInteractivityModifier()
{
}

Modifier *DisableInteractivityModifier::Replicate(void) const
{
	return (new DisableInteractivityModifier(*this));
}

void DisableInteractivityModifier::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Modifier::Pack(data, packFlags);

	PackHandle handle = data.BeginChunk('NAME');
	data << nodeName;
	data.EndChunk(handle);

	data << TerminatorChunk;
}

void DisableInteractivityModifier::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Modifier::Unpack(data, unpackFlags);
	UnpackChunkList<DisableInteractivityModifier>(data, unpackFlags);
}

bool DisableInteractivityModifier::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'NAME':

			data >> nodeName;
			nodeHash = Text::Hash(nodeName);
			return (true);
	}

	return (false);
}

int32 DisableInteractivityModifier::GetSettingCount(void) const
{
	return (1);
}

Setting *DisableInteractivityModifier::GetSetting(int32 index) const
{
	if (index == 0)
	{
		const char *title = TheInterfaceMgr->GetStringTable()->GetString(StringID('MDFR', 'NAME'));
		return (new TextSetting('NAME', nodeName, title, kMaxModifierNodeNameLength));
	}

	return (nullptr);
}

void DisableInteractivityModifier::SetSetting(const Setting *setting)
{
	if (setting->GetSettingIdentifier() == 'NAME')
	{
		nodeName = static_cast<const TextSetting *>(setting)->GetText();
		nodeHash = Text::Hash(nodeName);
	}
}

void DisableInteractivityModifier::SetNodeName(const char *name)
{
	nodeName = name;
	nodeHash = Text::Hash(nodeName);
}

bool DisableInteractivityModifier::operator ==(const Modifier& modifier) const
{
	if (modifier.GetModifierType() == kModifierDisableInteractivity)
	{
		const DisableInteractivityModifier *disableInteractivityModifier = static_cast<const DisableInteractivityModifier *>(&modifier);

		if (disableInteractivityModifier->nodeHash == nodeHash)
		{
			return (true);
		}
	}

	return (false);
}

void DisableInteractivityModifier::Apply(World *world, Instance *instance)
{
	if (nodeHash == 0)
	{
		Node *node = instance->GetFirstSubnode();
		while (node)
		{
			Property *property = node->GetProperty(kPropertyInteraction);
			if (property)
			{
				property->SetPropertyFlags(property->GetPropertyFlags() | kPropertyDisabled);
			}

			node = instance->GetNextNode(node);
		}
	}
	else
	{
		Node *node = instance->GetFirstSubnode();
		while (node)
		{
			if (node->GetNodeHash() == nodeHash)
			{
				Property *property = node->GetProperty(kPropertyInteraction);
				if (property)
				{
					property->SetPropertyFlags(property->GetPropertyFlags() | kPropertyDisabled);
				}
			}

			node = instance->GetNextNode(node);
		}
	}
}


SetPerspectiveMaskModifier::SetPerspectiveMaskModifier() : Modifier(kModifierSetPerspectiveMask)
{
	perspectiveExclusionMask = 0;

	nodeType = kNodeWildcard;
	nodeHash = 0;
	nodeName[0] = 0;
}

SetPerspectiveMaskModifier::SetPerspectiveMaskModifier(unsigned_int32 exclusionMask, NodeType type, const char *name) : Modifier(kModifierSetPerspectiveMask)
{
	perspectiveExclusionMask = exclusionMask;

	nodeType = type;
	nodeName = name;
	nodeHash = Text::Hash(name);
}

SetPerspectiveMaskModifier::SetPerspectiveMaskModifier(const SetPerspectiveMaskModifier& setPerspectiveMaskModifier) : Modifier(setPerspectiveMaskModifier)
{
	perspectiveExclusionMask = setPerspectiveMaskModifier.perspectiveExclusionMask;

	nodeType = setPerspectiveMaskModifier.nodeType;
	nodeName = setPerspectiveMaskModifier.nodeName;
	nodeHash = Text::Hash(nodeName);
}

SetPerspectiveMaskModifier::~SetPerspectiveMaskModifier()
{
}

Modifier *SetPerspectiveMaskModifier::Replicate(void) const
{
	return (new SetPerspectiveMaskModifier(*this));
}

void SetPerspectiveMaskModifier::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Modifier::Pack(data, packFlags);

	data << ChunkHeader('EXCL', 4);
	data << perspectiveExclusionMask;

	data << ChunkHeader('TYPE', 4);
	data << nodeType;

	PackHandle handle = data.BeginChunk('NAME');
	data << nodeName;
	data.EndChunk(handle);

	data << TerminatorChunk;
}

void SetPerspectiveMaskModifier::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Modifier::Unpack(data, unpackFlags);
	UnpackChunkList<SetPerspectiveMaskModifier>(data, unpackFlags);
}

bool SetPerspectiveMaskModifier::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'EXCL':

			data >> perspectiveExclusionMask;
			return (true);

		case 'TYPE':

			data >> nodeType;
			return (true);

		case 'NAME':

			data >> nodeName;
			nodeHash = Text::Hash(nodeName);
			return (true);
	}

	return (false);
}

int32 SetPerspectiveMaskModifier::GetSettingCount(void) const
{
	return (13);
}

Setting *SetPerspectiveMaskModifier::GetSetting(int32 index) const
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == 0)
	{
		return (GetNodeTypeSetting(nodeType));
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('MDFR', 'NAME'));
		return (new TextSetting('NAME', nodeName, title, kMaxModifierNodeNameLength));
	}

	if (index == 2)
	{
		const char *title = table->GetString(StringID('NODE', 'PRSP'));
		return (new HeadingSetting('PRSP', title));
	}

	if (index == 3)
	{
		const char *title = table->GetString(StringID('NODE', 'PRSP', 'DPRM'));
		return (new BooleanSetting('DPRM', ((perspectiveExclusionMask & kPerspectivePrimary) == 0), title));
	}

	if (index == 4)
	{
		const char *title = table->GetString(StringID('NODE', 'PRSP', 'DRFL'));
		return (new BooleanSetting('DRFL', ((perspectiveExclusionMask & kPerspectiveReflection) == 0), title));
	}

	if (index == 5)
	{
		const char *title = table->GetString(StringID('NODE', 'PRSP', 'DRFR'));
		return (new BooleanSetting('DRFR', ((perspectiveExclusionMask & kPerspectiveRefraction) == 0), title));
	}

	if (index == 6)
	{
		const char *title = table->GetString(StringID('NODE', 'PRSP', 'RPRM'));
		return (new BooleanSetting('RPRM', (((perspectiveExclusionMask >> kPerspectiveRemotePortalShift) & kPerspectivePrimary) == 0), title));
	}

	if (index == 7)
	{
		const char *title = table->GetString(StringID('NODE', 'PRSP', 'RRFL'));
		return (new BooleanSetting('RRFL', (((perspectiveExclusionMask >> kPerspectiveRemotePortalShift) & kPerspectiveReflection) == 0), title));
	}

	if (index == 8)
	{
		const char *title = table->GetString(StringID('NODE', 'PRSP', 'RRFR'));
		return (new BooleanSetting('RRFR', (((perspectiveExclusionMask >> kPerspectiveRemotePortalShift) & kPerspectiveRefraction) == 0), title));
	}

	if (index == 9)
	{
		const char *title = table->GetString(StringID('NODE', 'PRSP', 'CPRM'));
		return (new BooleanSetting('CPRM', (((perspectiveExclusionMask >> kPerspectiveCameraWidgetShift) & kPerspectivePrimary) == 0), title));
	}

	if (index == 10)
	{
		const char *title = table->GetString(StringID('NODE', 'PRSP', 'CRFL'));
		return (new BooleanSetting('CRFL', (((perspectiveExclusionMask >> kPerspectiveCameraWidgetShift) & kPerspectiveReflection) == 0), title));
	}

	if (index == 11)
	{
		const char *title = table->GetString(StringID('NODE', 'PRSP', 'CRFR'));
		return (new BooleanSetting('CRFR', (((perspectiveExclusionMask >> kPerspectiveCameraWidgetShift) & kPerspectiveRefraction) == 0), title));
	}

	if (index == 12)
	{
		const char *title = table->GetString(StringID('NODE', 'PRSP', 'RDSY'));
		return (new BooleanSetting('RDSY', ((perspectiveExclusionMask & kPerspectiveRadiositySpace) == 0), title));
	}

	return (nullptr);
}

void SetPerspectiveMaskModifier::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'TYPE')
	{
		nodeType = modifierNodeType[static_cast<const MenuSetting *>(setting)->GetMenuSelection()];
	}
	else if (identifier == 'NAME')
	{
		nodeName = static_cast<const TextSetting *>(setting)->GetText();
		nodeHash = Text::Hash(nodeName);
	}
	else if (identifier == 'DPRM')
	{
		if (!static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			perspectiveExclusionMask |= kPerspectivePrimary;
		}
		else
		{
			perspectiveExclusionMask &= ~kPerspectivePrimary;
		}
	}
	else if (identifier == 'DRFL')
	{
		if (!static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			perspectiveExclusionMask |= kPerspectiveReflection;
		}
		else
		{
			perspectiveExclusionMask &= ~kPerspectiveReflection;
		}
	}
	else if (identifier == 'DRFR')
	{
		if (!static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			perspectiveExclusionMask |= kPerspectiveRefraction;
		}
		else
		{
			perspectiveExclusionMask &= ~kPerspectiveRefraction;
		}
	}
	else if (identifier == 'RPRM')
	{
		if (!static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			perspectiveExclusionMask |= kPerspectivePrimary << kPerspectiveRemotePortalShift;
		}
		else
		{
			perspectiveExclusionMask &= ~(kPerspectivePrimary << kPerspectiveRemotePortalShift);
		}
	}
	else if (identifier == 'RRFL')
	{
		if (!static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			perspectiveExclusionMask |= kPerspectiveReflection << kPerspectiveRemotePortalShift;
		}
		else
		{
			perspectiveExclusionMask &= ~(kPerspectiveReflection << kPerspectiveRemotePortalShift);
		}
	}
	else if (identifier == 'RRFR')
	{
		if (!static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			perspectiveExclusionMask |= kPerspectiveRefraction << kPerspectiveRemotePortalShift;
		}
		else
		{
			perspectiveExclusionMask &= ~(kPerspectiveRefraction << kPerspectiveRemotePortalShift);
		}
	}
	else if (identifier == 'CPRM')
	{
		if (!static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			perspectiveExclusionMask |= kPerspectivePrimary << kPerspectiveCameraWidgetShift;
		}
		else
		{
			perspectiveExclusionMask &= ~(kPerspectivePrimary << kPerspectiveCameraWidgetShift);
		}
	}
	else if (identifier == 'CRFL')
	{
		if (!static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			perspectiveExclusionMask |= kPerspectiveReflection << kPerspectiveCameraWidgetShift;
		}
		else
		{
			perspectiveExclusionMask &= ~(kPerspectiveReflection << kPerspectiveCameraWidgetShift);
		}
	}
	else if (identifier == 'CRFR')
	{
		if (!static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			perspectiveExclusionMask |= kPerspectiveRefraction << kPerspectiveCameraWidgetShift;
		}
		else
		{
			perspectiveExclusionMask &= ~(kPerspectiveRefraction << kPerspectiveCameraWidgetShift);
		}
	}
	else if (identifier == 'RDSY')
	{
		if (!static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			perspectiveExclusionMask |= kPerspectiveRadiositySpace;
		}
		else
		{
			perspectiveExclusionMask &= ~kPerspectiveRadiositySpace;
		}
	}
}

void SetPerspectiveMaskModifier::SetNodeName(const char *name)
{
	nodeName = name;
	nodeHash = Text::Hash(nodeName);
}

bool SetPerspectiveMaskModifier::operator ==(const Modifier& modifier) const
{
	if (modifier.GetModifierType() == kModifierSetPerspectiveMask)
	{
		const SetPerspectiveMaskModifier *setPerspectiveMaskModifier = static_cast<const SetPerspectiveMaskModifier *>(&modifier);

		if (setPerspectiveMaskModifier->perspectiveExclusionMask != perspectiveExclusionMask)
		{
			return (false);
		}

		if (setPerspectiveMaskModifier->nodeType != nodeType)
		{
			return (false);
		}

		if (setPerspectiveMaskModifier->nodeHash == nodeHash)
		{
			return (true);
		}
	}

	return (false);
}

void SetPerspectiveMaskModifier::Apply(World *world, Instance *instance)
{
	NodeType type = nodeType;
	unsigned_int32 hash = nodeHash;

	Node *node = instance->GetFirstSubnode();
	while (node)
	{
		if ((type == kNodeWildcard) || (node->GetNodeType() == type))
		{
			if ((hash == 0) || (node->GetNodeHash() == hash))
			{
				node->SetPerspectiveExclusionMask(perspectiveExclusionMask);
			}
		}

		node = instance->GetNextNode(node);
	}
}


ReplaceMaterialModifier::ReplaceMaterialModifier() : Modifier(kModifierReplaceMaterial)
{
	nodeHash = 0;
	nodeName[0] = 0;

	materialObject = nullptr;
}

ReplaceMaterialModifier::ReplaceMaterialModifier(const char *name) : Modifier(kModifierReplaceMaterial)
{
	nodeName = name;
	nodeHash = Text::Hash(name);

	materialObject = nullptr;
}

ReplaceMaterialModifier::ReplaceMaterialModifier(const ReplaceMaterialModifier& replaceMaterialModifier) : Modifier(replaceMaterialModifier)
{
	nodeName = replaceMaterialModifier.nodeName;
	nodeHash = Text::Hash(nodeName);

	materialObject = replaceMaterialModifier.materialObject;
	if (materialObject)
	{
		materialObject->Retain();
	}
}

ReplaceMaterialModifier::~ReplaceMaterialModifier()
{
	if (materialObject)
	{
		materialObject->Release();
	}
}

Modifier *ReplaceMaterialModifier::Replicate(void) const
{
	return (new ReplaceMaterialModifier(*this));
}

void ReplaceMaterialModifier::Prepack(List<Object> *linkList) const
{
	if (materialObject)
	{
		linkList->Append(materialObject);
	}
}

void ReplaceMaterialModifier::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Modifier::Pack(data, packFlags);

	if (nodeName[0] != 0)
	{
		PackHandle handle = data.BeginChunk('NAME');
		data << nodeName;
		data.EndChunk(handle);
	}

	if ((materialObject) && (!(packFlags & kPackSettings)))
	{
		data << ChunkHeader('MATL', 4);
		data << materialObject->GetObjectIndex();
	}

	data << TerminatorChunk;
}

void ReplaceMaterialModifier::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Modifier::Unpack(data, unpackFlags);
	UnpackChunkList<ReplaceMaterialModifier>(data, unpackFlags);
}

bool ReplaceMaterialModifier::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'NAME':

			data >> nodeName;
			nodeHash = Text::Hash(nodeName);
			return (true);

		case 'MATL':
		{
			int32	objectIndex;

			data >> objectIndex;
			data.AddObjectLink(objectIndex, &MaterialObjectLinkProc, this);
			return (true);
		}
	}

	return (false);
}

void *ReplaceMaterialModifier::BeginSettingsUnpack(void)
{
	nodeName[0] = 0;
	return (Modifier::BeginSettingsUnpack());
}

void ReplaceMaterialModifier::MaterialObjectLinkProc(Object *object, void *cookie)
{
	ReplaceMaterialModifier *replaceMaterialModifier = static_cast<ReplaceMaterialModifier *>(cookie);
	replaceMaterialModifier->SetMaterialObject(static_cast<MaterialObject *>(object));
}

int32 ReplaceMaterialModifier::GetSettingCount(void) const
{
	return (1);
}

Setting *ReplaceMaterialModifier::GetSetting(int32 index) const
{
	if (index == 0)
	{
		const char *title = TheInterfaceMgr->GetStringTable()->GetString(StringID('MDFR', 'NAME'));
		return (new TextSetting('NAME', nodeName, title, kMaxModifierNodeNameLength));
	}

	return (nullptr);
}

void ReplaceMaterialModifier::SetSetting(const Setting *setting)
{
	if (setting->GetSettingIdentifier() == 'NAME')
	{
		nodeName = static_cast<const TextSetting *>(setting)->GetText();
		nodeHash = Text::Hash(nodeName);
	}
}

void ReplaceMaterialModifier::SetNodeName(const char *name)
{
	nodeName = name;
	nodeHash = Text::Hash(nodeName);
}

void ReplaceMaterialModifier::SetMaterialObject(MaterialObject *object)
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

bool ReplaceMaterialModifier::operator ==(const Modifier& modifier) const
{
	if (modifier.GetModifierType() == kModifierReplaceMaterial)
	{
		const ReplaceMaterialModifier *replaceMaterialModifier = static_cast<const ReplaceMaterialModifier *>(&modifier);

		if (replaceMaterialModifier->nodeName != nodeName)
		{
			return (false);
		}

		if (replaceMaterialModifier->materialObject == materialObject)
		{
			return (true);
		}
	}

	return (false);
}

void ReplaceMaterialModifier::Apply(World *world, Instance *instance)
{
	if (materialObject)
	{
		if (nodeHash == 0)
		{
			Node *node = instance->GetFirstSubnode();
			while (node)
			{
				if (node->GetNodeType() == kNodeGeometry)
				{
					Geometry *geometry = static_cast<Geometry *>(node);
					geometry->SetMaterialObject(0, materialObject);
				}

				node = instance->GetNextNode(node);
			}
		}
		else
		{
			Node *node = instance->GetFirstSubnode();
			while (node)
			{
				if (node->GetNodeHash() == nodeHash)
				{
					if (node->GetNodeType() == kNodeGeometry)
					{
						Geometry *geometry = static_cast<Geometry *>(node);
						geometry->SetMaterialObject(0, materialObject);
					}
				}

				node = instance->GetNextNode(node);
			}
		}
	}
}


RemovePhysicsModifier::RemovePhysicsModifier() : Modifier(kModifierRemovePhysics)
{
}

RemovePhysicsModifier::RemovePhysicsModifier(const RemovePhysicsModifier& removePhysicsModifier) : Modifier(removePhysicsModifier)
{
}

RemovePhysicsModifier::~RemovePhysicsModifier()
{
}

Modifier *RemovePhysicsModifier::Replicate(void) const
{
	return (new RemovePhysicsModifier(*this));
}

bool RemovePhysicsModifier::operator ==(const Modifier& modifier) const
{
	if (modifier.GetModifierType() == kModifierRemovePhysics)
	{
		return (true);
	}

	return (false);
}

void RemovePhysicsModifier::Apply(World *world, Instance *instance)
{
	const Node *node = instance->GetFirstSubnode();
	while (node)
	{
		const Controller *controller = node->GetController();
		if ((controller) && (controller->GetBaseControllerType() == kControllerRigidBody))
		{
			delete controller;
		}

		node = node->Next();
	}
}

bool RemovePhysicsModifier::KeepNode(const Node *node) const
{
	NodeType type = node->GetNodeType();
	return ((type != kNodeShape) && (type != kNodeJoint));
}

// ZYUQURM
