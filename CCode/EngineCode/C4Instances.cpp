 

#include "C4Instances.h"
#include "C4World.h"
#include "C4Configuration.h"


using namespace C4;


Instance::Instance() : Node(kNodeInstance)
{
}

Instance::Instance(const char *name) : Node(kNodeInstance)
{
	worldName = name;
}

Instance::Instance(const Instance& instance) : Node(instance)
{
	worldName = instance.worldName;

	const Modifier *modifier = instance.GetFirstModifier();
	while (modifier)
	{
		Modifier *clone = modifier->Clone();
		if (clone)
		{
			AddModifier(clone);
		}

		modifier = modifier->Next();
	}
}

Instance::~Instance()
{
}

Node *Instance::Replicate(void) const
{
	return (new Instance(*this));
}

void Instance::Prepack(List<Object> *linkList) const
{
	Node::Prepack(linkList);

	const Modifier *modifier = modifierList.First();
	while (modifier)
	{
		modifier->Prepack(linkList);
		modifier = modifier->Next();
	}
}

void Instance::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Node::Pack(data, packFlags);

	PackHandle handle = data.BeginChunk('WRLD');
	data << worldName;
	data.EndChunk(handle);

	const Modifier *modifier = modifierList.First();
	while (modifier)
	{
		handle = data.BeginChunk('MDFR');
		modifier->PackType(data);
		modifier->Pack(data, packFlags);
		data.EndChunk(handle);

		modifier = modifier->Next();
	}

	data << TerminatorChunk;
}

void Instance::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Node::Unpack(data, unpackFlags);
	UnpackChunkList<Instance>(data, unpackFlags);
}

bool Instance::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'WRLD':

			data >> worldName;
			return (true);

		case 'MDFR':
		{
			Modifier *modifier = Modifier::Create(data, unpackFlags);
			if (modifier)
			{
				modifier->Unpack(++data, unpackFlags);
				modifierList.Append(modifier);
				return (true);
			} 

			break;
		} 
	}
 
	return (false);
}
 
void *Instance::BeginSettingsUnpack(void)
{ 
	modifierList.Purge(); 
	return (Node::BeginSettingsUnpack());
}

int32 Instance::GetCategoryCount(void) const 
{
	return (Node::GetCategoryCount() + 1);
}

Type Instance::GetCategoryType(int32 index, const char **title) const
{
	int32 count = Node::GetCategoryCount();
	if (index == count)
	{
		*title = TheInterfaceMgr->GetStringTable()->GetString(StringID(kNodeInstance));
		return (kNodeInstance);
	}

	return (Node::GetCategoryType(index, title));
}

int32 Instance::GetCategorySettingCount(Type category) const
{
	if (category == kNodeInstance)
	{
		return (2);
	}

	return (Node::GetCategorySettingCount(category));
}

Setting *Instance::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kNodeInstance)
	{
		if (flags & kConfigurationScript)
		{
			return (nullptr);
		}

		const StringTable *table = TheInterfaceMgr->GetStringTable();

		if (index == 0)
		{
			const char *title = table->GetString(StringID(kNodeInstance, 'INST'));
			return (new HeadingSetting('INST', title));
		}

		if (index == 1)
		{
			const char *title = table->GetString(StringID(kNodeInstance, 'INST', 'WRLD'));
			const char *picker = table->GetString(StringID(kNodeInstance, 'INST', 'PICK'));
			return (new ResourceSetting('WRLD', worldName, title, picker, WorldResource::GetDescriptor()));
		}

		return (nullptr);
	}

	return (Node::GetCategorySetting(category, index, flags));
}

void Instance::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kNodeInstance)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'WRLD')
		{
			worldName = static_cast<const ResourceSetting *>(setting)->GetResourceName();
		}
	}
	else
	{
		Node::SetCategorySetting(category, setting);
	}
}

int32 Instance::GetInternalConnectorCount(void) const
{
	return (2);
}

const char *Instance::GetInternalConnectorKey(int32 index) const
{
	if (index == 0)
	{
		return (kConnectorKeyShadow);
	}
	else if (index == 1)
	{
		return (kConnectorKeyPaint);
	}

	return (nullptr);
}

bool Instance::ValidConnectedNode(const ConnectorKey& key, const Node *node) const
{
	if (key == kConnectorKeyShadow)
	{
		if (node->GetNodeType() == kNodeSpace)
		{
			return (static_cast<const Space *>(node)->GetSpaceType() == kSpaceShadow);
		}

		return (false);
	}

	if (key == kConnectorKeyPaint)
	{
		if (node->GetNodeType() == kNodeSpace)
		{
			return (static_cast<const Space *>(node)->GetSpaceType() == kSpacePaint);
		}

		return (false);
	}

	return (Node::ValidConnectedNode(key, node));
}

bool Instance::ModifierCloneFilter(const Node *node, void *cookie)
{
	const Instance *instance = static_cast<Instance *>(cookie);

	const Modifier *modifier = instance->modifierList.First();
	do
	{
		if (!modifier->KeepNode(node))
		{
			return (false);
		}

		modifier = modifier->Next();
	} while (modifier);

	return (true);
}

bool Instance::Expand(World *world)
{
	if (!GetFirstSubnode())
	{
		if (worldName[0] == 0)
		{
			return (false);
		}

		Modifier *modifier = modifierList.First();
		Node::CloneFilterProc *filterProc = (modifier) ? &ModifierCloneFilter : &Node::DefaultCloneFilter;

		Node *instanceRoot = world->NewInstancedWorld(worldName, filterProc, this);
		if (instanceRoot)
		{
			bool zoneFlag = false;
			for (;;)
			{
				Node *node = instanceRoot->GetFirstSubnode();
				if (!node)
				{
					break;
				}

				AppendSubnode(node);

				if (node->GetNodeType() == kNodeZone)
				{
					zoneFlag = true;
				}
			}

			if (!zoneFlag)
			{
				SetNodeFlags(GetNodeFlags() | (kNodeVisibilitySite | kNodeIndependentVisibility));
			}

			if (!GetManipulator())
			{
				const Hub *instanceHub = instanceRoot->GetHub();
				if (instanceHub)
				{
					Node *node = GetFirstSubnode();
					while (node)
					{
						const Hub *hub = node->GetHub();
						if (hub)
						{
							Connector *connector = hub->GetFirstOutgoingEdge();
							while (connector)
							{
								const Hub *finish = connector->GetFinishElement();
								if (finish == instanceHub)
								{
									if ((node->GetNodeType() == kNodePortal) && (connector->GetConnectorKey() == kConnectorKeyZone))
									{
										static_cast<Portal *>(node)->SetConnectedZone(GetOwningZone());
									}
									else
									{
										connector->SetConnectorTarget(this);
									}
								}

								connector = connector->GetNextOutgoingEdge();
							}
						}

						node = GetNextNode(node);
					}
				}
			}

			delete instanceRoot;

			while (modifier)
			{
				modifier->Apply(world, this);
				modifier = modifier->Next();
			}

			if (!Enabled())
			{
				Disable();
			}
		}
	}

	return (true);
}

Node *Instance::FindExtractableNode(void) const
{
	Node *node = GetFirstSubnode();
	while (node)
	{
		const Controller *controller = node->GetController();
		if ((controller) && (controller->InstanceExtractable()))
		{
			return (node);
		}

		node = node->Next();
	}

	return (nullptr);
}

void Instance::Extract(World *world)
{
	if ((!GetManipulator()) && (!(world->GetWorldFlags() & kWorldRestore)))
	{
		Node *extractableNode = FindExtractableNode();
		if (extractableNode)
		{
			Property *property = GetFirstProperty();
			while (property)
			{
				Property *next = property->Next();
				extractableNode->AddProperty(property);
				property = next;
			}

			const Hub *hub = GetHub();
			if (hub)
			{
				Connector *connector = hub->GetFirstIncomingEdge();
				while (connector)
				{
					Connector *next = connector->GetNextIncomingEdge();

					if (!Successor(connector->GetStartElement()->GetNode()))
					{
						connector->SetConnectorTarget(extractableNode);
					}

					connector = next;
				}
			}

			Zone *dynamicZone = world->GetRootNode();
			Zone *staticZone = dynamicZone;

			Transform4D staticTransform = GetNodeTransform();
			Transform4D dynamicTransform = GetNodeTransform();

			Node *super = GetSuperNode();
			while (super != dynamicZone)
			{
				NodeType type = super->GetNodeType();
				if (type == kNodeZone)
				{
					if (staticZone == dynamicZone)
					{
						staticZone = static_cast<Zone *>(super);
					}
				}
				else if (type == kNodeInstance)
				{
					break;
				}

				const Transform4D& superTransform = super->GetNodeTransform();
				dynamicTransform = superTransform * dynamicTransform;

				if (staticZone == dynamicZone)
				{
					staticTransform = superTransform * staticTransform;
				}

				super = super->GetSuperNode();
			}

			if (super->GetNodeType() != kNodeInstance)
			{
				Node *node = GetFirstSubnode();
				do
				{
					Node *next = node->Next();

					const Controller *controller = node->GetController();
					if ((controller) && (controller->InstanceExtractable()))
					{
						node->SetNodeTransform(dynamicTransform * node->GetNodeTransform());
						dynamicZone->AppendSubnode(node);
					}
					else
					{
						node->SetNodeTransform(staticTransform * node->GetNodeTransform());
						staticZone->AppendSubnode(node);
					}

					node->SetPersistent();

					node = next;
				} while (node);
			}
			else
			{
				Node *node = GetFirstSubnode();
				do
				{
					Node *next = node->Next();

					node->SetNodeTransform(dynamicTransform * node->GetNodeTransform());
					super->AppendSubnode(node);
					node->SetPersistent();

					node = next;
				} while (node);
			}

			const Controller *controller = GetController();
			if (controller)
			{
				// If the instance node has a controller (possibly a script controller), then we keep
				// the node but clear the world name to prevent expansion in a restored world.

				worldName[0] = 0;
			}
			else
			{
				delete this;
			}
		}
	}
}

void Instance::Collapse(void)
{
	PurgeSubtree();
	DismantleVisibility();
	SetNodeFlags(GetNodeFlags() & ~(kNodeVisibilitySite | kNodeIndependentVisibility));
}

// ZYUQURM
