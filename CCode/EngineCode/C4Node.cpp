 

#include "C4Node.h"
#include "C4Cameras.h"
#include "C4Triggers.h"
#include "C4Emitters.h"
#include "C4Terrain.h"
#include "C4Water.h"
#include "C4Skybox.h"
#include "C4Fields.h"
#include "C4Joints.h"
#include "C4Blockers.h"
#include "C4World.h"
#include "C4Manipulator.h"
#include "C4Configuration.h"


using namespace C4;


namespace C4
{
	struct ConnectorData
	{
		ConnectorKey	connectorKey;
		Node			*targetNode;

		ConnectorData(const ConnectorKey& key, Node *node)
		{
			connectorKey = key;
			targetNode = node;
		}
	};


	template class Creatable<Node>;
}


const char C4::kConnectorKeyMember[] = "%Member";


NodeTree::NodeTree()
{
	prevBranch = nullptr;
	nextBranch = nullptr;
	firstSubbranch = nullptr;
	lastSubbranch = nullptr;

	branchFlag = false;
	activeUpdateFlags = Node::kUpdateTransform;
	currentUpdateFlags = Node::kUpdateTransform;
	subtreeUpdateFlags = 0;
}

NodeTree::~NodeTree()
{
	PurgeSubtree();

	if (branchFlag)
	{
		GetSuperNode()->RemoveBranch(this);
	}

	while (firstSubbranch)
	{
		RemoveBranch(firstSubbranch);
	}
}

inline Node *NodeTree::GetFirstSubbranch(void) const
{
	return (static_cast<Node *>(firstSubbranch));
}

inline Node *NodeTree::GetNextBranch(void) const
{
	return (static_cast<Node *>(nextBranch));
}

void NodeTree::RemoveBranch(NodeTree *branch)
{
	NodeTree *prev = branch->prevBranch;
	NodeTree *next = branch->nextBranch;

	if (prev)
	{
		prev->nextBranch = next;
	}

	if (next)
	{
		next->prevBranch = prev;
	}

	if (firstSubbranch == branch)
	{
		firstSubbranch = next;
	}

	if (lastSubbranch == branch)
	{
		lastSubbranch = prev; 
	}

	branch->prevBranch = nullptr; 
	branch->nextBranch = nullptr;
	branch->branchFlag = false; 
}

void NodeTree::InvalidateUpdateFlags(unsigned_int32 flags) 
{
	currentUpdateFlags |= flags; 
 
	NodeTree *node = this;
	for (;;)
	{
		NodeTree *super = node->Tree<Node>::GetSuperNode(); 
		if (!super)
		{
			break;
		}

		if (!node->branchFlag)
		{
			super->AddBranch(node);
		}

		node = super;

		if ((node->subtreeUpdateFlags & flags) == flags)
		{
			break;
		}

		node->subtreeUpdateFlags |= flags;
	}
}

void NodeTree::PropagateUpdateFlags(unsigned_int32 flags)
{
	NodeTree *node = this;
	for (;;)
	{
		if ((node->subtreeUpdateFlags & flags) == flags)
		{
			break;
		}

		node->subtreeUpdateFlags |= flags;

		NodeTree *super = node->Tree<Node>::GetSuperNode();
		if (!super)
		{
			break;
		}

		if (!node->branchFlag)
		{
			super->AddBranch(node);
		}

		node = super;
	}
}

void NodeTree::Detach(void)
{
	NodeTree *super = Tree<Node>::GetSuperNode();
	if (super)
	{
		if (branchFlag)
		{
			super->RemoveBranch(this);
		}

		Tree<Node>::Detach();
	}
}

void NodeTree::AppendSubnode(Node *node)
{
	node->NodeTree::Detach();
	Tree<Node>::AppendSubnode(node);
	node->Invalidate();
}

void NodeTree::PrependSubnode(Node *node)
{
	node->NodeTree::Detach();
	Tree<Node>::PrependSubnode(node);
	node->Invalidate();
}

void NodeTree::InsertSubnodeBefore(Node *node, Node *before)
{
	node->NodeTree::Detach();
	Tree<Node>::InsertSubnodeBefore(node, before);
	node->Invalidate();
}

void NodeTree::InsertSubnodeAfter(Node *node, Node *after)
{
	node->NodeTree::Detach();
	Tree<Node>::InsertSubnodeAfter(node, after);
	node->Invalidate();
}

void NodeTree::RemoveSubnode(Node *node)
{
	if (node->branchFlag)
	{
		RemoveBranch(node);
	}

	Tree<Node>::RemoveSubnode(node);
}

void NodeTree::SetActiveUpdateFlags(unsigned_int32 flags)
{
	activeUpdateFlags = flags;
	currentUpdateFlags = flags;

	NodeTree *super = Tree<Node>::GetSuperNode();
	if (super)
	{
		if (!branchFlag)
		{
			super->AddBranch(this);
		}

		super->PropagateUpdateFlags(flags);
	}
}

void NodeTree::AddBranch(NodeTree *branch)
{
	branch->branchFlag = true;

	if (lastSubbranch)
	{
		lastSubbranch->nextBranch = branch;
		branch->prevBranch = lastSubbranch;
		lastSubbranch = branch;
	}
	else
	{
		firstSubbranch = branch;
		lastSubbranch = branch;
	}
}

void NodeTree::RemoveAllBranches(void)
{
	NodeTree *branch = firstSubbranch;
	while (branch)
	{
		NodeTree *next = branch->nextBranch;

		branch->RemoveAllBranches();

		branch->prevBranch = nullptr;
		branch->nextBranch = nullptr;
		branch->branchFlag = false;

		branch = next;
	}

	firstSubbranch = nullptr;
	lastSubbranch = nullptr;
}

void NodeTree::CancelUpdate(void)
{
	if (branchFlag)
	{
		GetSuperNode()->RemoveBranch(this);
	}
}

void NodeTree::Invalidate(void)
{
	unsigned_int32 flags = currentUpdateFlags | activeUpdateFlags;
	currentUpdateFlags = flags;

	NodeTree *super = Tree<Node>::GetSuperNode();
	if (super)
	{
		if (!branchFlag)
		{
			super->AddBranch(this);
		}

		super->PropagateUpdateFlags(flags | subtreeUpdateFlags);
	}

	NodeTree *node = Tree<Node>::GetFirstSubnode();
	while (node)
	{
		node->Invalidate();
		node = node->Next();
	}
}

void NodeTree::Update(void)
{
	currentUpdateFlags = 0;

	if (subtreeUpdateFlags != 0)
	{
		subtreeUpdateFlags = 0;

		for (;;)
		{
			NodeTree *node = firstSubbranch;
			if (!node)
			{
				break;
			}

			RemoveBranch(node);

			node->Update();
			node = node->Next();
		}
	}
}


Node::Node(NodeType type) : Site(kCellNode)
{
	nodeType = type;
	nodeFlags = 0;
	nodeHash = 0;

	perspectiveExclusionMask = 0;

	nodeWorld = nullptr;
	nodeManipulator = nullptr;
	nodeController = nullptr;
	nodeObject = nullptr;
	nodeHub = nullptr;
	propertyObject = nullptr;

	nodeTransform.SetIdentity();
	previousWorldTransform(3,3) = 0.0F;

	visibilityProc = &SphereVisible;
	occlusionProc = &SphereVisible;

	maxSubzoneDepth = kSubzoneDepthUnlimited;
	forcedSubzoneDepth = -1;
}

Node::Node(const Node& node) : Site(kCellNode)
{
	nodeType = node.nodeType;
	nodeFlags = node.nodeFlags;
	nodeHash = node.nodeHash;

	perspectiveExclusionMask = node.perspectiveExclusionMask;

	nodeWorld = nullptr;
	nodeManipulator = nullptr;
	nodeController = nullptr;
	nodeObject = nullptr;
	nodeHub = nullptr;

	Object *object = node.nodeObject;
	if (object)
	{
		Object	*clone;

		if ((!(nodeFlags & kNodeUnsharedObject)) || ((clone = object->Clone()) == nullptr))
		{
			nodeObject = object;
			object->Retain();
		}
		else
		{
			nodeObject = clone;
		}
	}

	const Manipulator *manipulator = node.nodeManipulator;
	if ((manipulator) && (manipulator->GetManipulatorState() & kManipulatorHidden))
	{
		nodeFlags |= kNodeInstanceHidden;
	}

	const Controller *controller = node.nodeController;
	if (controller)
	{
		SetController(controller->Clone());
	}

	const Hub *hub = node.nodeHub;
	if (hub)
	{
		nodeHub = new Hub(this);

		const Connector *connector = hub->GetFirstOutgoingEdge();
		while (connector)
		{
			new Connector(nodeHub, *connector);
			connector = connector->GetNextOutgoingEdge();
		}
	}

	const Property *property = node.GetFirstProperty();
	while (property)
	{
		Property *clone = property->Clone();
		if (clone)
		{
			AddProperty(clone);
		}

		property = property->Next();
	}

	propertyObject = node.propertyObject;
	if (propertyObject)
	{
		propertyObject->Retain();
	}

	nodeTransform = node.nodeTransform;
	previousWorldTransform(3,3) = 0.0F;

	visibilityProc = &SphereVisible;
	occlusionProc = &SphereVisible;

	maxSubzoneDepth = node.maxSubzoneDepth;
	forcedSubzoneDepth = node.forcedSubzoneDepth;
}

Node::~Node()
{
	if (propertyObject)
	{
		propertyObject->Release();
	}

	delete nodeController;
	delete nodeManipulator;
	delete nodeHub;

	if (nodeObject)
	{
		nodeObject->Release();
	}
}

Node *Node::Create(Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (data.GetType())
	{
		case kNodeGeneric:

			return (new Node);

		case kNodeCamera:

			return (Camera::Create(++data, unpackFlags));

		case kNodeLight:

			return (Light::Create(++data, unpackFlags));

		case kNodeSource:

			return (Source::Create(++data, unpackFlags));

		case kNodeGeometry:

			return (Geometry::Create(++data, unpackFlags));

		case kNodeInstance:

			return (new Instance);

		case kNodeModel:

			return (Model::Create(++data, unpackFlags));

		case kNodeBone:

			return (new Bone);

		case kNodeMarker:

			return (Marker::Create(++data, unpackFlags));

		case kNodeTrigger:

			return (Trigger::Create(++data, unpackFlags));

		case kNodeEffect:

			return (Effect::Create(++data, unpackFlags));

		case kNodeEmitter:

			return (Emitter::Create(++data, unpackFlags));

		case kNodeSpace:

			return (Space::Create(++data, unpackFlags));

		case kNodePortal:

			return (Portal::Create(++data, unpackFlags));

		case kNodeZone:

			return (Zone::Create(++data, unpackFlags));

		case kNodeShape:

			return (Shape::Create(++data, unpackFlags));

		case kNodeJoint:

			return (Joint::Create(++data, unpackFlags));

		case kNodeField:

			return (Field::Create(++data, unpackFlags));

		case kNodeBlocker:

			return (Blocker::Create(++data, unpackFlags));

		case kNodePhysics:

			return (new PhysicsNode);

		case kNodeSkybox:

			return (new Skybox);

		case kNodeImpostor:

			return (new Impostor);

		case kNodeTerrainBlock:

			return (new TerrainBlock);

		case kNodeWaterBlock:

			return (new WaterBlock);
	}

	return (Creatable<Node>::Create(data, unpackFlags));
}

Node *Node::Replicate(void) const
{
	return (new Node(*this));
}

bool Node::DefaultCloneFilter(const Node *node, void *cookie)
{
	return (!(node->GetNodeFlags() & kNodeCloneInhibit));
}

Node *Node::CloneNode(CloneFilterProc *filterProc, void *filterCookie) const
{
	Node *clone = Replicate();

	Node *subnode = GetFirstSubnode();
	while (subnode)
	{
		if ((*filterProc)(subnode, filterCookie))
		{
			Node *subclone = subnode->CloneNode(filterProc, filterCookie);
			if (subclone)
			{
				clone->AppendSubnode(subclone);
			}
		}

		subnode = subnode->Next();
	}

	return (clone);
}

Node *Node::CloneNode(const Node *root, Node **nodeTable, Array<ConnectorCloneData, 16> *connectorArray, CloneFilterProc *filterProc, void *filterCookie) const
{
	Node *clone = Replicate();
	nodeTable[nodeIndex] = clone;

	if (nodeHub)
	{
		const Connector *nodeConnector = nodeHub->GetFirstOutgoingEdge();
		if (nodeConnector)
		{
			Connector *cloneConnector = clone->nodeHub->GetFirstOutgoingEdge();
			do
			{
				const Node *target = nodeConnector->GetConnectorTarget();
				if ((target) && ((root == target) || (root->Successor(target))) && ((*filterProc)(target, filterCookie)))
				{
					ConnectorCloneData *data = connectorArray->AddElement();
					data->connector = cloneConnector;
					data->linkIndex = target->GetNodeIndex();
				}

				nodeConnector = nodeConnector->GetNextOutgoingEdge();
				cloneConnector = cloneConnector->GetNextOutgoingEdge();
			} while (nodeConnector);
		}
	}

	Node *subnode = GetFirstSubnode();
	while (subnode)
	{
		if ((*filterProc)(subnode, filterCookie))
		{
			Node *subclone = subnode->CloneNode(root, nodeTable, connectorArray, filterProc, filterCookie);
			if (subclone)
			{
				clone->AppendSubnode(subclone);
			}
		}

		subnode = subnode->Next();
	}

	return (clone);
}

Node *Node::Clone(CloneFilterProc *filterProc, void *filterCookie) const
{
	nodeIndex = 0;
	int32 count = 1;
	bool connectorFlag = ((nodeHub) && (nodeHub->HasOutgoingConnection()));

	Node *subnode = GetFirstSubnode();
	while (subnode)
	{
		if ((*filterProc)(subnode, filterCookie))
		{
			subnode->nodeIndex = count++;
			const Hub *hub = subnode->nodeHub;
			connectorFlag |= ((hub) && (hub->HasOutgoingConnection()));

			subnode = GetNextNode(subnode);
		}
		else
		{
			subnode = GetNextLevelNode(subnode);
		}
	}

	if (!connectorFlag)
	{
		return (CloneNode(filterProc, filterCookie));
	}

	Array<ConnectorCloneData, 16>	connectorArray;

	Node **nodeTable = new Node *[count];
	Node *clone = CloneNode(this, nodeTable, &connectorArray, filterProc, filterCookie);

	for (const ConnectorCloneData& cloneData : connectorArray)
	{
		cloneData.connector->SetConnectorTarget(nodeTable[cloneData.linkIndex]);
	}

	delete[] nodeTable;
	return (clone);
}

void Node::CloneSubtree(Node *root) const
{
	int32 count = 0;
	bool connectorFlag = false;

	Node *subnode = GetFirstSubnode();
	while (subnode)
	{
		if (DefaultCloneFilter(subnode))
		{
			subnode->nodeIndex = count++;
			const Hub *hub = subnode->nodeHub;
			connectorFlag |= ((hub) && (hub->HasOutgoingConnection()));

			subnode = GetNextNode(subnode);
		}
		else
		{
			subnode = GetNextLevelNode(subnode);
		}
	}

	if (!connectorFlag)
	{
		subnode = GetFirstSubnode();
		while (subnode)
		{
			if (DefaultCloneFilter(subnode))
			{
				root->AppendSubnode(subnode->CloneNode());
			}

			subnode = subnode->Next();
		}
	}
	else
	{
		Array<ConnectorCloneData, 16>	connectorArray;

		Node **nodeTable = new Node *[count];

		subnode = GetFirstSubnode();
		while (subnode)
		{
			if (DefaultCloneFilter(subnode))
			{
				root->AppendSubnode(subnode->CloneNode(this, nodeTable, &connectorArray));
			}

			subnode = subnode->Next();
		}

		for (const ConnectorCloneData& cloneData : connectorArray)
		{
			cloneData.connector->SetConnectorTarget(nodeTable[cloneData.linkIndex]);
		}

		delete[] nodeTable;
	}
}

bool Node::LinkedNodePackable(unsigned_int32 packFlags) const
{
	if (packFlags & kPackSettings)
	{
		return (false);
	}

	return ((!(packFlags & kPackSelected)) || (GetManipulator()->Selected()));
}

void Node::PackType(Packer& data) const
{
	data << nodeType;
}

void Node::Prepack(List<Object> *linkList) const
{
	if (nodeObject)
	{
		linkList->Append(nodeObject);
	}

	if (nodeController)
	{
		nodeController->Prepack(linkList);
	}

	const Property *property = propertyMap.First();
	while (property)
	{
		if (!(property->GetPropertyFlags() & kPropertyNonpersistent))
		{
			property->Prepack(linkList);
		}

		property = property->Next();
	}

	if (propertyObject)
	{
		linkList->Append(propertyObject);
	}
}

void Node::Pack(Packer& data, unsigned_int32 packFlags) const
{
	data << ChunkHeader('FLAG', 4);
	data << unsigned_int32(nodeFlags & kNodeFlagsMask);

	data << ChunkHeader('XFRM', sizeof(Transform4D));
	data << nodeTransform;

	if (nodeHash != 0)
	{
		data << ChunkHeader('HASH', 4);
		data << nodeHash;
	}

	data << ChunkHeader('EXCL', 4);
	data << perspectiveExclusionMask;

	if (maxSubzoneDepth != kSubzoneDepthUnlimited)
	{
		data << ChunkHeader('MSZD', 4);
		data << maxSubzoneDepth;
	}

	if (forcedSubzoneDepth >= 0)
	{
		data << ChunkHeader('FSZD', 4);
		data << forcedSubzoneDepth;
	}

	if (nodeController)
	{
		PackHandle handle = data.BeginChunk('CTRL');
		nodeController->PackType(data);
		nodeController->Pack(data, packFlags);
		data.EndChunk(handle);
	}

	if (nodeHub)
	{
		const Connector *connector = nodeHub->GetFirstOutgoingEdge();
		while (connector)
		{
			PackHandle handle = data.BeginChunk('CNNC');
			data << connector->GetConnectorKey();

			int32 connectedNodeIndex = -1;
			const Node *node = connector->GetConnectorTarget();
			if ((node) && (node->LinkedNodePackable(packFlags)))
			{
				if (!(node->GetNodeFlags() & kNodeNonpersistent))
				{
					connectedNodeIndex = node->GetNodeIndex();
				}
				else if (connector->GetConnectorFlags() & kConnectorSaveFinishPersistent)
				{
					const Node *super = node->GetSuperNode();
					while (super)
					{
						if (!(super->GetNodeFlags() & kNodeNonpersistent))
						{
							connectedNodeIndex = super->GetNodeIndex();
							break;
						}

						super = super->GetSuperNode();
					}
				}
			}

			data << connectedNodeIndex;
			data.EndChunk(handle);

			connector = connector->GetNextOutgoingEdge();
		}
	}

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

	if (!(packFlags & kPackSettings))
	{
		if (propertyObject)
		{
			data << ChunkHeader('POBJ', 4);
			data << propertyObject->GetObjectIndex();
		}

		if ((nodeManipulator) && (packFlags & kPackEditor))
		{
			PackHandle handle = data.BeginChunk('MTOR');
			nodeManipulator->Pack(data, packFlags);
			data.EndChunk(handle);
		}

		int32 superIndex = -1;
		const Node *super = GetSuperNode();

		if (packFlags & kPackSelected)
		{
			while (super)
			{
				if (super->GetManipulator()->Selected())
				{
					superIndex = super->nodeIndex;
					break;
				}

				super = super->GetSuperNode();
			}
		}
		else if (super)
		{
			superIndex = super->nodeIndex;
		}

		data << ChunkHeader('INDX', 8);
		data << superIndex;

		int32 objectIndex = (nodeObject) ? nodeObject->GetObjectIndex() : -1;
		data << objectIndex;
	}

	data << TerminatorChunk;
}

void Node::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	UnpackChunkList<Node>(data, unpackFlags);

	if (unpackFlags & kUnpackNonpersistent)
	{
		nodeFlags |= kNodeNonpersistent;
	}
}

bool Node::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> nodeFlags;
			return (true);

		case 'XFRM':

			data >> nodeTransform;
			return (true);

		case 'HASH':

			data >> nodeHash;
			return (true);

		case 'EXCL':

			data >> perspectiveExclusionMask;
			return (true);

		case 'MSZD':

			data >> maxSubzoneDepth;
			return (true);

		case 'FSZD':

			data >> forcedSubzoneDepth;
			return (true);

		case 'CTRL':
		{
			Controller *controller = Controller::Create(data, unpackFlags);
			if (controller)
			{
				controller->Unpack(++data, unpackFlags);
				SetController(controller);
				return (true);
			}

			break;
		}

		case 'CNNC':
		{
			ConnectorKey	key;
			int32			index;

			data >> key;
			data >> index;

			#if C4LEGACY

				if (key == "%Ambient")
				{
					return (true);
				}

			#endif

			if (!nodeHub)
			{
				new Hub(this);
			}

			Connector *connector = new Connector(nodeHub, key);
			data.AddNodeLink(index, &ConnectorLinkProc, connector);
			return (true);
		}

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

		case 'POBJ':
		{
			int32	index;

			data >> index;
			data.AddObjectLink(index, &PropertyObjectLinkProc, this);
			return (true);
		}

		case 'MTOR':

			if (unpackFlags & kUnpackEditor)
			{
				Manipulator *manipulator = Manipulator::Create(this);
				if (manipulator)
				{
					manipulator->Unpack(data, unpackFlags);
					nodeManipulator = manipulator;
					return (true);
				}
			}

			break;

		case 'INDX':

			data >> superIndex;
			data >> nodeObjectIndex;
			return (true);
	}

	return (false);
}

void *Node::BeginSettingsUnpack(void)
{
	nodeHash = 0;
	perspectiveExclusionMask = 0;
	maxSubzoneDepth = kSubzoneDepthUnlimited;
	forcedSubzoneDepth = -1;

	delete nodeController;
	nodeController = nullptr;

	Array<ConnectorData, 4> *array = nullptr;
	if (nodeHub)
	{
		array = new Array<ConnectorData, 4>;

		Connector *connector = nodeHub->GetFirstOutgoingEdge();
		while (connector)
		{
			Connector *next = connector->GetNextOutgoingEdge();

			Node *node = connector->GetConnectorTarget();
			if (node)
			{
				array->AddElement(ConnectorData(connector->GetConnectorKey(), node));
			}

			delete connector;
			connector = next;
		}
	}

	propertyMap.Purge();
	return (array);
}

void Node::EndSettingsUnpack(void *cookie)
{
	Array<ConnectorData, 4> *array = static_cast<Array<ConnectorData, 4> *>(cookie);
	if (array)
	{
		if (nodeHub)
		{
			for (const ConnectorData& data : *array)
			{
				Connector *connector = nodeHub->GetFirstOutgoingEdge();
				while (connector)
				{
					if (connector->GetConnectorKey() == data.connectorKey)
					{
						if (connector->GetFinishElement() == nodeHub)
						{
							connector->SetConnectorTarget(data.targetNode);
						}

						break;
					}

					connector = connector->GetNextOutgoingEdge();
				}
			}
		}

		delete array;
	}
}

void Node::ConnectorLinkProc(Node *node, void *cookie)
{
	static_cast<Connector *>(cookie)->SetConnectorTarget(node);
}

void Node::PropertyObjectLinkProc(Object *object, void *cookie)
{
	static_cast<Node *>(cookie)->propertyObject = static_cast<PropertyObject *>(object);
	object->Retain();
}

int32 Node::GetCategoryCount(void) const
{
	return (GetSuperNode() ? 1 : 0);
}

Type Node::GetCategoryType(int32 index, const char **title) const
{
	if (index == 0)
	{
		*title = TheInterfaceMgr->GetStringTable()->GetString(StringID('NODE'));
		return ('NODE');
	}

	return (0);
}

int32 Node::GetCategorySettingCount(Type category) const
{
	if (category == 'NODE')
	{
		return (12);
	}

	return (0);
}

Setting *Node::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == 'NODE')
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();

		if (index == 0)
		{
			const char *title = table->GetString(StringID('NODE', 'NODE'));
			return (new HeadingSetting('NODE', title));
		}

		if (index == 1)
		{
			const char *name = nullptr;
			const Property *property = GetProperty(kPropertyName);
			if (property)
			{
				name = static_cast<const NameProperty *>(property)->GetNodeName();
			}

			const char *title = table->GetString(StringID('NODE', 'NODE', 'NAME'));
			return (new TextSetting('NAME', name, title, kMaxNodeNameLength));
		}

		if (index == 2)
		{
			const char *title = table->GetString(StringID('NODE', 'NODE', 'DSAB'));
			return (new BooleanSetting('DSAB', ((nodeFlags & kNodeDisabled) != 0), title));
		}

		if (index == 3)
		{
			const char *title = table->GetString(StringID('NODE', 'NODE', 'DENO'));
			return (new BooleanSetting('DENO', ((nodeFlags & kNodeDirectEnableOnly) != 0), title));
		}

		if (index == 4)
		{
			const char *title = table->GetString(StringID('NODE', 'NODE', 'AMBP'));
			return (new BooleanSetting('AMBP', ((nodeFlags & kNodeSubzoneAmbient) != 0), title));
		}

		if (index == 5)
		{
			const char *title = table->GetString(StringID('NODE', 'NODE', 'MSZD'));
			return (new IntegerSetting('MSZD', maxSubzoneDepth, title, 0, 5, 1, nullptr, table->GetString(StringID('NODE', 'NODE', 'MSZD', 'UNLM'))));
		}

		if (index == 6)
		{
			const char *title = table->GetString(StringID('NODE', 'NODE', 'FSZD'));
			return (new IntegerSetting('FSZD', forcedSubzoneDepth, title, -1, 4, 1, table->GetString(StringID('NODE', 'NODE', 'FSZD', 'NONE')), nullptr));
		}

		if (index == 7)
		{
			const char *title = table->GetString(StringID('NODE', 'ANIM'));
			return (new HeadingSetting('ANIM', title));
		}

		if (index == 8)
		{
			const char *title = table->GetString(StringID('NODE', 'ANIM', 'ANME'));
			return (new BooleanSetting('ANME', ((nodeFlags & kNodeAnimateInhibit) == 0), title));
		}

		if (index == 9)
		{
			const char *title = table->GetString(StringID('NODE', 'ANIM', 'ASUB'));
			return (new BooleanSetting('ASUB', ((nodeFlags & kNodeAnimateSubtree) != 0), title));
		}

		if (index == 10)
		{
			const char *title = table->GetString(StringID('NODE', 'ANIM', 'DXFM'));
			return (new BooleanSetting('DXFM', ((nodeFlags & kNodeTransformAnimationInhibit) != 0), title));
		}

		if (index == 11)
		{
			const char *title = table->GetString(StringID('NODE', 'ANIM', 'DMWT'));
			return (new BooleanSetting('DMWT', ((nodeFlags & kNodeMorphWeightAnimationInhibit) != 0), title));
		}
	}

	return (nullptr);
}

void Node::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == 'NODE')
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'NAME')
		{
			const char *name = static_cast<const TextSetting *>(setting)->GetText();
			Property *property = GetProperty(kPropertyName);

			if (name[0] != 0)
			{
				if (property)
				{
					static_cast<NameProperty *>(property)->SetNodeName(name);
					nodeHash = Text::Hash(name);
				}
				else
				{
					AddProperty(new NameProperty(name));
				}
			}
			else
			{
				delete property;
				nodeHash = 0;
			}
		}
		else if (identifier == 'DSAB')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				nodeFlags |= kNodeDisabled;
			}
			else
			{
				nodeFlags &= ~kNodeDisabled;
			}
		}
		else if (identifier == 'DENO')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				nodeFlags |= kNodeDirectEnableOnly;
			}
			else
			{
				nodeFlags &= ~kNodeDirectEnableOnly;
			}
		}
		else if (identifier == 'AMBP')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				nodeFlags |= kNodeSubzoneAmbient;
			}
			else
			{
				nodeFlags &= ~kNodeSubzoneAmbient;
			}
		}
		else if (identifier == 'MSZD')
		{
			int32 maxDepth = static_cast<const IntegerSetting *>(setting)->GetIntegerValue();
			maxSubzoneDepth = (maxDepth < 5) ? maxDepth : kSubzoneDepthUnlimited;
		}
		else if (identifier == 'FSZD')
		{
			forcedSubzoneDepth = static_cast<const IntegerSetting *>(setting)->GetIntegerValue();
		}
		else if (identifier == 'ANME')
		{
			if (!static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				nodeFlags |= kNodeAnimateInhibit;
			}
			else
			{
				nodeFlags &= ~kNodeAnimateInhibit;
			}
		}
		else if (identifier == 'ASUB')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				nodeFlags |= kNodeAnimateSubtree;
			}
			else
			{
				nodeFlags &= ~kNodeAnimateSubtree;
			}
		}
		else if (identifier == 'DXFM')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				nodeFlags |= kNodeTransformAnimationInhibit;
			}
			else
			{
				nodeFlags &= ~kNodeTransformAnimationInhibit;
			}
		}
		else if (identifier == 'DMWT')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				nodeFlags |= kNodeMorphWeightAnimationInhibit;
			}
			else
			{
				nodeFlags &= ~kNodeMorphWeightAnimationInhibit;
			}
		}
	}
}

Setting *Node::GetPerspectiveMaskSetting(int32 index) const
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('NODE', 'PRSP'));
		return (new HeadingSetting('PRSP', title));
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('NODE', 'PRSP', 'DPRM'));
		return (new BooleanSetting('DPRM', ((perspectiveExclusionMask & kPerspectivePrimary) == 0), title));
	}

	if (index == 2)
	{
		const char *title = table->GetString(StringID('NODE', 'PRSP', 'DRFL'));
		return (new BooleanSetting('DRFL', ((perspectiveExclusionMask & kPerspectiveReflection) == 0), title));
	}

	if (index == 3)
	{
		const char *title = table->GetString(StringID('NODE', 'PRSP', 'DRFR'));
		return (new BooleanSetting('DRFR', ((perspectiveExclusionMask & kPerspectiveRefraction) == 0), title));
	}

	if (index == 4)
	{
		const char *title = table->GetString(StringID('NODE', 'PRSP', 'RPRM'));
		return (new BooleanSetting('RPRM', (((perspectiveExclusionMask >> kPerspectiveRemotePortalShift) & kPerspectivePrimary) == 0), title));
	}

	if (index == 5)
	{
		const char *title = table->GetString(StringID('NODE', 'PRSP', 'RRFL'));
		return (new BooleanSetting('RRFL', (((perspectiveExclusionMask >> kPerspectiveRemotePortalShift) & kPerspectiveReflection) == 0), title));
	}

	if (index == 6)
	{
		const char *title = table->GetString(StringID('NODE', 'PRSP', 'RRFR'));
		return (new BooleanSetting('RRFR', (((perspectiveExclusionMask >> kPerspectiveRemotePortalShift) & kPerspectiveRefraction) == 0), title));
	}

	if (index == 7)
	{
		const char *title = table->GetString(StringID('NODE', 'PRSP', 'CPRM'));
		return (new BooleanSetting('CPRM', (((perspectiveExclusionMask >> kPerspectiveCameraWidgetShift) & kPerspectivePrimary) == 0), title));
	}

	if (index == 8)
	{
		const char *title = table->GetString(StringID('NODE', 'PRSP', 'CRFL'));
		return (new BooleanSetting('CRFL', (((perspectiveExclusionMask >> kPerspectiveCameraWidgetShift) & kPerspectiveReflection) == 0), title));
	}

	if (index == 9)
	{
		const char *title = table->GetString(StringID('NODE', 'PRSP', 'CRFR'));
		return (new BooleanSetting('CRFR', (((perspectiveExclusionMask >> kPerspectiveCameraWidgetShift) & kPerspectiveRefraction) == 0), title));
	}

	if (index == 10)
	{
		const char *title = table->GetString(StringID('NODE', 'PRSP', 'RDSY'));
		return (new BooleanSetting('RDSY', ((perspectiveExclusionMask & kPerspectiveRadiositySpace) == 0), title));
	}

	return (nullptr);
}

bool Node::SetPerspectiveMaskSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'DPRM')
	{
		if (!static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			perspectiveExclusionMask |= kPerspectivePrimary;
		}
		else
		{
			perspectiveExclusionMask &= ~kPerspectivePrimary;
		}

		return (true);
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

		return (true);
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

		return (true);
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

		return (true);
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

		return (true);
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

		return (true);
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

		return (true);
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

		return (true);
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

		return (true);
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

	return (false);
}

void Node::StopMotion(void)
{
	previousWorldTransform(3,3) = 0.0F;

	if (nodeController)
	{
		nodeController->StopMotion();
	}

	Node *subnode = GetFirstSubnode();
	while (subnode)
	{
		subnode->StopMotion();
		subnode = subnode->Next();
	}
}

void Node::Enable(void)
{
	nodeFlags &= ~kNodeDisabled;

	Node *subnode = GetFirstSubnode();
	while (subnode)
	{
		unsigned_int32 flags = subnode->nodeFlags;
		if (!(flags & kNodeDirectEnableOnly))
		{
			subnode->nodeFlags = flags & ~kNodeDisabled;
		}

		subnode = GetNextNode(subnode);
	}
}

void Node::Disable(void)
{
	nodeFlags |= kNodeDisabled;

	Node *subnode = GetFirstSubnode();
	while (subnode)
	{
		unsigned_int32 flags = subnode->nodeFlags;
		if (!(flags & kNodeDirectEnableOnly))
		{
			subnode->nodeFlags = flags | kNodeDisabled;
		}

		subnode = GetNextNode(subnode);
	}
}

void Node::SetPersistent(void)
{
	nodeFlags &= ~kNodeNonpersistent;

	Node *subnode = GetFirstSubnode();
	while (subnode)
	{
		subnode->nodeFlags &= ~kNodeNonpersistent;
		subnode = GetNextNode(subnode);
	}
}

void Node::SetNonpersistent(void)
{
	nodeFlags |= kNodeNonpersistent;

	Node *subnode = GetFirstSubnode();
	while (subnode)
	{
		subnode->nodeFlags |= kNodeNonpersistent;
		subnode = GetNextNode(subnode);
	}
}

void Node::SetObject(Object *object)
{
	if (nodeObject != object)
	{
		if (nodeObject)
		{
			nodeObject->Release();
		}

		if (object)
		{
			object->Retain();
		}

		nodeObject = object;
	}
}

void Node::SetController(Controller *controller)
{
	if (nodeController != controller)
	{
		if (nodeController)
		{
			nodeController->SetTargetNode(nullptr);
		}

		if (controller)
		{
			controller->SetTargetNode(this);
		}

		nodeController = controller;
	}
}

Connector *Node::GetConnector(const char *key) const
{
	if (key[0] != 0)
	{
		const Hub *hub = nodeHub;
		if (hub)
		{
			Connector *connector = hub->GetFirstOutgoingEdge();
			while (connector)
			{
				if (connector->GetConnectorKey() == key)
				{
					return (connector);
				}

				connector = connector->GetNextOutgoingEdge();
			}
		}
	}

	return (nullptr);
}

Node *Node::GetConnectedNode(const char *key) const
{
	if (key[0] != 0)
	{
		const Hub *hub = nodeHub;
		if (hub)
		{
			const Connector *connector = hub->GetFirstOutgoingEdge();
			while (connector)
			{
				if (connector->GetConnectorKey() == key)
				{
					const Hub *finish = connector->GetFinishElement();
					if (finish != hub)
					{
						return (finish->GetNode());
					}

					break;
				}

				connector = connector->GetNextOutgoingEdge();
			}
		}
	}

	return (nullptr);
}

void Node::SetConnectedNode(const char *key, Node *node)
{
	const Hub *hub = nodeHub;
	if (hub)
	{
		Connector *connector = hub->GetFirstOutgoingEdge();
		while (connector)
		{
			if (connector->GetConnectorKey() == key)
			{
				connector->SetConnectorTarget(node);
				return;
			}

			connector = connector->GetNextOutgoingEdge();
		}
	}

	if (node)
	{
		AddConnector(key, node);
	}
}

void Node::AddConnector(const char *key, Node *node)
{
	if (!nodeHub)
	{
		new Hub(this);
	}

	if (node)
	{
		Hub *finish = node->nodeHub;
		if (!finish)
		{
			finish = new Hub(node);
		}

		new Connector(nodeHub, finish, key);
	}
	else
	{
		new Connector(nodeHub, key);
	}
}

bool Node::RemoveConnector(const char *key)
{
	if (nodeHub)
	{
		Connector *connector = nodeHub->FindOutgoingConnector(key);
		if (connector)
		{
			delete connector;

			if (nodeHub->Isolated())
			{
				delete nodeHub;
			}

			return (true);
		}
	}

	return (false);
}

int32 Node::GetInternalConnectorCount(void) const
{
	return (0);
}

const char *Node::GetInternalConnectorKey(int32 index) const
{
	return (nullptr);
}

void Node::ProcessInternalConnectors(void)
{
}

bool Node::ValidConnectedNode(const ConnectorKey& key, const Node *node) const
{
	if (key == kConnectorKeyMember)
	{
		NodeType type = node->GetNodeType();
		if (type == kNodeZone)
		{
			return (true);
		}

		if (type == kNodeMarker)
		{
			const Marker *marker = static_cast<const Marker *>(node);
			if (marker->GetMarkerType() == kMarkerConnection)
			{
				return (true);
			}
		}

		return (false);
	}

	return (true);
}

bool Node::AddProperty(Property *property)
{
	bool result = propertyMap.Insert(property);

	if ((result) && (property->GetPropertyType() == kPropertyName))
	{
		nodeHash = Text::Hash(static_cast<NameProperty *>(property)->GetNodeName());
	}

	return (result);
}

void Node::SetPropertyObject(PropertyObject *object)
{
	PropertyObject *prevObject = propertyObject;
	if (prevObject != object)
	{
		if (prevObject)
		{
			prevObject->Release();
		}

		if (object)
		{
			object->Retain();
		}

		propertyObject = object;
	}
}

const char *Node::GetNodeName(void) const
{
	const Property *property = GetProperty(kPropertyName);
	if (property)
	{
		return (static_cast<const NameProperty *>(property)->GetNodeName());
	}

	return (nullptr);
}

void Node::SetNodeName(const char *name)
{
	Property *property = GetProperty(kPropertyName);
	if (property)
	{
		static_cast<NameProperty *>(property)->SetNodeName(name);
		nodeHash = Text::Hash(name);
	}
	else
	{
		AddProperty(new NameProperty(name));
	}
}

C4::Zone *Node::GetOwningZone(void) const
{
	Node *super = GetSuperNode();
	while (super)
	{
		if (super->GetNodeType() == kNodeZone)
		{
			break;
		}

		super = super->GetSuperNode();
	}

	return (static_cast<Zone *>(super));
}

void Node::EstablishVisibility(void)
{
	if (nodeFlags & kNodeVisibilitySite)
	{
		unsigned_int32 updateFlags = GetActiveUpdateFlags() | kUpdateVisibility;
		if (!(nodeFlags & kNodeIsolatedVisibility))
		{
			updateFlags |= kUpdatePostprocess;
		}

		SetActiveUpdateFlags(updateFlags);

		SetVisibilityProc(&BoxVisible);
		SetOcclusionProc(&BoxVisible);

		InitializeVisibility();
	}
}

void Node::InitializeVisibility(void)
{
	PurgeVisibility();

	if (!(nodeFlags & kNodeIndependentVisibility))
	{
		Node *super = GetSuperNode();
		while (super)
		{
			if (super->GetNodeType() == kNodeZone)
			{
				break;
			}

			if (super->GetNodeFlags() & kNodeVisibilitySite)
			{
				new Bond(super, this);
				break;
			}

			super = super->GetSuperNode();
		}
	}
}

void Node::DismantleVisibility(void)
{
	PurgeVisibility();
	SetActiveUpdateFlags(GetActiveUpdateFlags() & ~kUpdateVisibility);
}

void Node::Preload(void)
{
	Node *subnode = GetFirstSubnode();
	while (subnode)
	{
		subnode->Preload();
		subnode = subnode->Next();
	}
}

void Node::Preinitialize(void)
{
	Box3D			box;
	BoundingSphere	sphere;

	nodeFlags |= kNodePreinitialized;

	HandleTransformUpdate();

	// To be consistent with ordinary updates, preinitialize the subnodes after
	// calculating the world transform and before calculating the bounding volumes.
	// (Some nodes also depend on this initialization order.)

	Node *node = GetFirstSubnode();
	while (node)
	{
		node->Preinitialize();
		node = node->Next();
	}

	if (CalculateBoundingBox(&box))
	{
		SetWorldBoundingBox(Transform(box, GetWorldTransform()));
	}

	if (CalculateBoundingSphere(&sphere))
	{
		worldBoundingSphere.SetCenter(GetWorldTransform() * sphere.GetCenter());
		worldBoundingSphere.SetRadius(sphere.GetRadius());
	}
}

void Node::Preprocess(void)
{
	if (!(nodeFlags & kNodePreinitialized))
	{
		Preinitialize();
	}

	Node *super = GetSuperNode();
	if (super)
	{
		nodeWorld = super->GetWorld();
	}

	if (nodeWorld)
	{
		nodeWorld->GetLoadContext()->loadProgress++;
	}

	ProcessInternalConnectors();

	if (nodeController)
	{
		nodeController->Preprocess();
	}

	if (nodeManipulator)
	{
		nodeManipulator->Preprocess();
	}

	EstablishVisibility();

	Node *subnode = GetFirstSubnode();
	while (subnode)
	{
		subnode->Preprocess();
		subnode = subnode->Next();
	}
}

void Node::Neutralize(void)
{
	Node *subnode = GetLastSubnode();
	while (subnode)
	{
		subnode->Neutralize();
		subnode = subnode->Previous();
	}

	if (nodeManipulator)
	{
		nodeManipulator->Neutralize();
	}

	if (nodeController)
	{
		nodeController->Neutralize();
	}

	DismantleVisibility();

	// Only clear the world pointer if this isn't the root node so that the existing
	// world pointer can be re-established for subnodes on a subsequent Preprocess().

	if (GetSuperNode())
	{
		nodeWorld = nullptr;
	}
}

void Node::ProcessObjectSettings(void)
{
}

void Node::Invalidate(void)
{
	NodeTree::Invalidate();

	if (nodeManipulator)
	{
		nodeManipulator->Invalidate();
	}

	Node *node = this;
	for (;;)
	{
		const Bond *bond = node->GetFirstIncomingEdge();
		if (!bond)
		{
			break;
		}

		Site *site = bond->GetStartElement();
		if (site->GetCellIndex() >= 0)
		{
			break;
		}

		node = static_cast<Node *>(site);
		if ((node->GetNodeFlags() & (kNodeVisibilitySite | kNodeIsolatedVisibility)) != kNodeVisibilitySite)
		{
			break;
		}

		node->InvalidateUpdateFlags(kUpdatePostprocess);
	}
}

void Node::Update(void)
{
	UpdateTransform();
	UpdatePostprocess();
	UpdateVisibility();

	RemoveAllBranches();
}

void Node::UpdateTransform(void)
{
	unsigned_int32 flags = GetCurrentUpdateFlags();
	if (flags & kUpdateTransform)
	{
		Box3D			box;
		BoundingSphere	sphere;

		SetCurrentUpdateFlags(flags & ~kUpdateTransform);

		HandleTransformUpdate();

		if (CalculateBoundingBox(&box))
		{
			SetWorldBoundingBox(Transform(box, GetWorldTransform()));
		}

		if (CalculateBoundingSphere(&sphere))
		{
			worldBoundingSphere.SetCenter(GetWorldTransform() * sphere.GetCenter());
			worldBoundingSphere.SetRadius(sphere.GetRadius());
		}
	}

	flags = GetSubtreeUpdateFlags();
	if (flags & kUpdateTransform)
	{
		SetSubtreeUpdateFlags(flags & ~kUpdateTransform);

		Node *node = GetFirstSubbranch();
		while (node)
		{
			node->UpdateTransform();
			node = node->GetNextBranch();
		}
	}
}

void Node::UpdatePostprocess(void)
{
	unsigned_int32 flags = GetSubtreeUpdateFlags();
	if (flags & kUpdatePostprocess)
	{
		SetSubtreeUpdateFlags(flags & ~kUpdatePostprocess);

		Node *node = GetFirstSubbranch();
		while (node)
		{
			node->UpdatePostprocess();
			node = node->GetNextBranch();
		}
	}

	flags = GetCurrentUpdateFlags();
	if (flags & kUpdatePostprocess)
	{
		SetCurrentUpdateFlags(flags & ~kUpdatePostprocess);
		HandlePostprocessUpdate();
	}
}

void Node::UpdateVisibility(void)
{
	unsigned_int32 flags = GetCurrentUpdateFlags();
	if (flags & kUpdateVisibility)
	{
		SetCurrentUpdateFlags(flags & ~kUpdateVisibility);
		HandleVisibilityUpdate();
	}

	flags = GetSubtreeUpdateFlags();
	if (flags & kUpdateVisibility)
	{
		SetSubtreeUpdateFlags(flags & ~kUpdateVisibility);

		Node *node = GetFirstSubbranch();
		while (node)
		{
			node->UpdateVisibility();
			node = node->GetNextBranch();
		}
	}
}

void Node::HandleTransformUpdate(void)
{
	const Node *super = GetSuperNode();

	if (previousWorldTransform(3,3) != 0.0F)
	{
		previousWorldTransform = GetWorldTransform();

		if (super)
		{
			SetWorldTransform(super->GetWorldTransform() * nodeTransform);
		}
		else
		{
			SetWorldTransform(nodeTransform);
		}
	}
	else
	{
		if (super)
		{
			SetWorldTransform(super->GetWorldTransform() * nodeTransform);
		}
		else
		{
			SetWorldTransform(nodeTransform);
		}

		previousWorldTransform = GetWorldTransform();
	}
}

void Node::HandlePostprocessUpdate(void)
{
	const Bond *bond = GetFirstOutgoingEdge();
	if (bond)
	{
		Box3D box = bond->GetFinishElement()->GetWorldBoundingBox();
		for (;;)
		{
			bond = bond->GetNextOutgoingEdge();
			if (!bond)
			{
				break;
			}

			box.Union(bond->GetFinishElement()->GetWorldBoundingBox());
		}

		SetWorldBoundingBox(box);
	}
}

void Node::HandleVisibilityUpdate(void)
{
	PurgeVisibility();

	if (!(nodeFlags & kNodeIndependentVisibility))
	{
		Node *super = GetSuperNode();
		while (super)
		{
			if (super->GetNodeFlags() & kNodeVisibilitySite)
			{
				new Bond(super, this);
				break;
			}

			if (super->GetNodeType() == kNodeZone)
			{
				static_cast<Zone *>(super)->InsertZoneTreeSite(kCellGraphGeometry, this, maxSubzoneDepth, forcedSubzoneDepth);
				break;
			}

			super = super->GetSuperNode();
		}
	}
	else
	{
		Node *super = GetSuperNode();
		while (super)
		{
			if (super->GetNodeType() == kNodeZone)
			{
				static_cast<Zone *>(super)->InsertZoneTreeSite(kCellGraphGeometry, this, maxSubzoneDepth, forcedSubzoneDepth);
				break;
			}

			super = super->GetSuperNode();
		}
	}
}

bool Node::CalculateBoundingBox(Box3D *box) const
{
	return (false);
}

bool Node::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	return (false);
}

bool Node::AlwaysVisible(const Node *node, const VisibilityRegion *region)
{
	return (true);
}

bool Node::AlwaysVisible(const Node *node, const VisibilityRegion *region, const List<OcclusionRegion> *occlusionList)
{
	return (true);
}

bool Node::BoxVisible(const Node *node, const VisibilityRegion *region)
{
	return (region->BoxVisible(node->GetWorldBoundingBox()));
}

bool Node::BoxVisible(const Node *node, const VisibilityRegion *region, const List<OcclusionRegion> *occlusionList)
{
	const Box3D& box = node->GetWorldBoundingBox();
	if (region->BoxVisible(box))
	{
		const OcclusionRegion *occluder = occlusionList->First();
		while (occluder)
		{
			if (occluder->BoxOccluded(box))
			{
				return (false);
			}

			occluder = occluder->Next();
		}

		return (true);
	}

	return (false);
}

bool Node::SphereVisible(const Node *node, const VisibilityRegion *region)
{
	const BoundingSphere *sphere = node->GetBoundingSphere();
	return ((sphere) && (region->SphereVisible(sphere->GetCenter(), sphere->GetRadius())));
}

bool Node::SphereVisible(const Node *node, const VisibilityRegion *region, const List<OcclusionRegion> *occlusionList)
{
	const BoundingSphere *sphere = node->GetBoundingSphere();
	if (sphere)
	{
		const Point3D& center = sphere->GetCenter();
		float radius = sphere->GetRadius();

		if (region->SphereVisible(center, radius))
		{
			const OcclusionRegion *occluder = occlusionList->First();
			while (occluder)
			{
				if (occluder->SphereOccluded(center, radius))
				{
					return (false);
				}

				occluder = occluder->Next();
			}

			return (true);
		}
	}

	return (false);
}

void Node::PrepackNodeObjects(List<Object> *linkList) const
{
	List<Object>	list;

	Prepack(&list);
	for (;;)
	{
		Object *object = list.First();
		if (!object)
		{
			break;
		}

		object->Prepack(&list);
		linkList->Append(object);
	}
}

FileResult Node::PackTree(File *file, unsigned_int32 packFlags) const
{
	int32			fileHeader[2];
	List<Object>	objectList;

	fileHeader[0] = 1;
	fileHeader[1] = kEngineInternalVersion;

	FileResult result = file->Write(fileHeader, 8);
	if (result != kFileOkay)
	{
		return (result);
	}

	int32 controllerCount = 0;
	int32 nodeCount = 0;

	const Node *node = this;
	do
	{
		if (!(node->GetNodeFlags() & kNodeNonpersistent))
		{
			node->nodeIndex = nodeCount++;
			node->PrepackNodeObjects(&objectList);

			Controller *controller = node->GetController();
			if (controller)
			{
				if (packFlags & kPackInitialize)
				{
					controller->SetControllerIndex(kControllerUnassigned);
					if (!(controller->GetControllerFlags() & kControllerLocal))
					{
						controllerCount++;
					}
				}
				else
				{
					controllerCount = Max(controller->GetControllerIndex() + 1, controllerCount);
				}
			}

			node = GetNextNode(node);
		}
		else
		{
			node->nodeIndex = -1;

			Node *subnode = node->GetFirstSubnode();
			while (subnode)
			{
				subnode->nodeIndex = -1;
				subnode = node->GetNextNode(subnode);
			}

			node = GetNextLevelNode(node);
		}
	} while (node);

	int32 objectCount = 0;
	Object *object = objectList.First();
	while (object)
	{
		object->SetObjectIndex(objectCount);
		objectCount++;

		object = object->Next();
	}

	file->Write(&controllerCount, 4);
	file->Write(&objectCount, 4);
	file->Write(&nodeCount, 4);

	int32 offsetCount = objectCount + 1;
	file->Write(&offsetCount, 4);

	unsigned_int32 *objectOffset = new unsigned_int32[offsetCount];
	MemoryMgr::ClearMemory(objectOffset, offsetCount * 4);
	result = file->Write(objectOffset, offsetCount * 4);

	if (result != kFileOkay)
	{
		delete[] objectOffset;
		objectList.RemoveAll();
		return (result);
	}

	Buffer buffer(kPackageDefaultSize);

	unsigned_int32 offset = 24 + offsetCount * 4;
	objectOffset[0] = offset;

	for (machine a = 0; a < objectCount; a++)
	{
		object = objectList.First();

		Package package(buffer, kPackageDefaultSize);
		Packer packer(&package);

		PackHandle handle = packer.BeginSection();
		object->PackType(packer);
		object->Pack(packer, packFlags);
		packer.EndSection(handle);

		unsigned_int32 size = package.GetSize();
		offset += size;
		objectOffset[a + 1] = offset;

		result = file->Write(package.GetStorage(), size);
		if (result != kFileOkay)
		{
			break;
		}

		objectList.Remove(object);
	}

	if (result != kFileOkay)
	{
		delete[] objectOffset;
		objectList.RemoveAll();
		return (result);
	}

	Node *super = GetSuperNode();
	if (super)
	{
		super->nodeIndex = -1;
	}

	node = this;
	do
	{
		if (!(node->GetNodeFlags() & kNodeNonpersistent))
		{
			Package package(buffer, kPackageDefaultSize);
			Packer packer(&package);

			PackHandle handle = packer.BeginSection();
			node->PackType(packer);
			node->Pack(packer, packFlags);
			packer.EndSection(handle);

			result = file->Write(package.GetStorage(), package.GetSize());
			if (result != kFileOkay)
			{
				break;
			}

			node = GetNextNode(node);
		}
		else
		{
			node = GetNextLevelNode(node);
		}
	} while (node);

	if (result == kFileOkay)
	{
		file->SetPosition(24);
		result = file->Write(objectOffset, offsetCount * 4);
	}

	delete[] objectOffset;
	return (result);
}

void Node::PackTree(Package *package, unsigned_int32 packFlags) const
{
	List<Object>	objectList;

	Packer packer(package);

	int32 endian = 1;
	packer << endian;

	int32 version = kEngineInternalVersion;
	packer << version;

	bool select = ((packFlags & kPackSelected) != 0);

	int32 controllerCount = 0;
	int32 nodeCount = 0;

	const Node *node = this;
	do
	{
		if (!(node->GetNodeFlags() & kNodeNonpersistent))
		{
			bool include = !select;
			if (!include)
			{
				const Manipulator *manipulator = node->GetManipulator();
				if ((manipulator) && (manipulator->Selected()))
				{
					include = true;
				}
			}

			if (include)
			{
				node->nodeIndex = nodeCount++;
				node->PrepackNodeObjects(&objectList);

				Controller *controller = node->GetController();
				if (controller)
				{
					if (packFlags & kPackInitialize)
					{
						controller->SetControllerIndex(kControllerUnassigned);
						if (!(controller->GetControllerFlags() & kControllerLocal))
						{
							controllerCount++;
						}
					}
					else
					{
						controllerCount = Max(controller->GetControllerIndex() + 1, controllerCount);
					}
				}
			}
			else
			{
				node->nodeIndex = -1;
			}

			node = GetNextNode(node);
		}
		else
		{
			node->nodeIndex = -1;

			Node *subnode = node->GetFirstSubnode();
			while (subnode)
			{
				subnode->nodeIndex = -1;
				subnode = node->GetNextNode(subnode);
			}

			node = GetNextLevelNode(node);
		}
	} while (node);

	int32 objectCount = 0;
	Object *object = objectList.First();
	while (object)
	{
		object->SetObjectIndex(objectCount);
		objectCount++;

		object = object->Next();
	}

	packer << controllerCount;
	packer << objectCount;
	packer << nodeCount;

	int32 offsetCount = 0;
	packer << offsetCount;

	for (;;)
	{
		object = objectList.First();
		if (!object)
		{
			break;
		}

		PackHandle handle = packer.BeginSection();
		object->PackType(packer);
		object->Pack(packer, packFlags);
		packer.EndSection(handle);

		objectList.Remove(object);
	}

	Node *super = GetSuperNode();
	if (super)
	{
		super->nodeIndex = -1;
	}

	node = this;
	do
	{
		if (!(node->GetNodeFlags() & kNodeNonpersistent))
		{
			bool include = !select;
			if (!include)
			{
				const Manipulator *manipulator = node->GetManipulator();
				if ((manipulator) && (manipulator->Selected()))
				{
					include = true;
				}
			}

			if (include)
			{
				PackHandle handle = packer.BeginSection();
				node->PackType(packer);
				node->Pack(packer, packFlags);
				packer.EndSection(handle);
			}

			node = GetNextNode(node);
		}
		else
		{
			node = GetNextLevelNode(node);
		}
	} while (node);
}

Object **Node::LoadOriginalObjects(const ResourceName& name, World *previousWorld, int32 newObjectCount, int32 *originalObjectCount, int32 *totalObjectCount)
{
	ResourceLoader		loader;
	WorldHeader			worldHeader;
	int32				*offsetTable;

	WorldResource *resource = WorldResource::Get(name, kResourceDeferLoad);

	ResourceResult result = resource->OpenLoader(&loader);
	if (result != kResourceOkay)
	{
		resource->Release();
		return (nullptr);
	}

	result = resource->LoadObjectOffsetTable(&loader, &worldHeader, &offsetTable);
	if (result != kResourceOkay)
	{
		resource->Release();
		return (nullptr);
	}

	int32 objectCount = worldHeader.objectCount;
	int32 totalCount = Max(objectCount, newObjectCount);
	*originalObjectCount = objectCount;
	*totalObjectCount = totalCount;

	Object **objectTable = new Object *[totalCount];
	MemoryMgr::ClearMemory(objectTable, totalCount * sizeof(Object *));

	if ((previousWorld) && (Text::CompareTextCaseless(previousWorld->GetWorldName(), name)))
	{
		List<Object>	objectList;

		Node *root = previousWorld->GetRootNode();
		const Node *node = root;
		do
		{
			if (!(node->GetNodeFlags() & kNodeNonpersistent))
			{
				node->PrepackNodeObjects(&objectList);
				node = root->GetNextNode(node);
			}
			else
			{
				node = root->GetNextLevelNode(node);
			}
		} while (node);

		for (;;)
		{
			Object *object = objectList.First();
			if (!object)
			{
				break;
			}

			int32 index = object->GetObjectIndex();
			if ((index >= 0) && (!object->GetModifiedFlag()))
			{
				object->Retain();
				objectTable[index] = object;
			}

			objectList.Remove(object);
		}

		delete previousWorld;

		for (machine a = 0; a < objectCount; a++)
		{
			if (!objectTable[a])
			{
				char			*objectData;
				unsigned_int32	size;

				if (resource->LoadObject(&loader, a, offsetTable, &objectData) == kResourceOkay)
				{
					Unpacker unpacker(objectData, worldHeader.version);

					unpacker >> size;
					Object *object = Object::Create(unpacker);
					if (object)
					{
						object->Unpack(++unpacker, 0);
						object->SetObjectIndex(a);
						objectTable[a] = object;
					}

					delete[] objectData;
				}
			}
		}
	}
	else
	{
		char	*objectData;

		delete previousWorld;

		if (resource->LoadAllObjects(&loader, &worldHeader, offsetTable, &objectData) == kResourceOkay)
		{
			Unpacker unpacker(objectData, worldHeader.version);

			for (machine a = 0; a < objectCount; a++)
			{
				unsigned_int32	size;

				unpacker >> size;
				const void *mark = unpacker.GetPointer();

				Object *object = Object::Create(unpacker);
				if (object)
				{
					object->Unpack(++unpacker, 0);
					object->SetObjectIndex(a);
					objectTable[a] = object;
				}
				else
				{
					unpacker.Skip(mark, size);
					objectTable[a] = nullptr;
				}
			}

			delete[] objectData;
		}
	}

	delete[] offsetTable;
	resource->Release();

	return (objectTable);
}

Node *Node::LoadNodeTable(Unpacker& unpacker, unsigned_int32 unpackFlags, int32 nodeCount, int32 objectCount, Object **objectTable)
{
	Node *root = nullptr;
	Node **nodeTable = new Node *[nodeCount];
	for (machine a = 0; a < nodeCount; a++)
	{
		unsigned_int32	size;

		unpacker >> size;
		const void *mark = unpacker.GetPointer();

		Node *node = Create(unpacker, unpackFlags);
		if (node)
		{
			node->Unpack(++unpacker, unpackFlags);
			nodeTable[a] = node;
		}
		else
		{
			unpacker.Skip(mark, size);
			nodeTable[a] = nullptr;
		}
	}

	for (machine a = 0; a < nodeCount; a++)
	{
		Node *node = nodeTable[a];
		if (node)
		{
			int32 superIndex = node->superIndex;
			if (superIndex >= 0)
			{
				Node *super = nodeTable[superIndex];
				if (super)
				{
					super->AppendSubnode(node);
				}
				else
				{
					delete node;
					nodeTable[a] = nullptr;
					continue;
				}
			}
			else
			{
				root = node;
			}

			int32 objectIndex = node->nodeObjectIndex;
			if (objectIndex >= 0)
			{
				Object *object = objectTable[objectIndex];
				if (object)
				{
					node->SetObject(object);
				}
				else if (node != root)
				{
					delete node;
					nodeTable[a] = nullptr;
				}
			}
		}
	}

	ObjectLink *objectLink = unpacker.GetFirstObjectLink();
	while (objectLink)
	{
		int32 index = objectLink->GetObjectIndex();
		objectLink->CallLinkProc((index >= 0) ? objectTable[index] : nullptr);
		objectLink = objectLink->Next();
	}

	NodeLink *nodeLink = unpacker.GetFirstNodeLink();
	while (nodeLink)
	{
		int32 index = nodeLink->GetNodeIndex();
		nodeLink->CallLinkProc((index >= 0) ? nodeTable[index] : nullptr);
		nodeLink = nodeLink->Next();
	}

	for (machine a = objectCount - 1; a >= 0; a--)
	{
		Object *object = objectTable[a];
		if (object)
		{
			object->Release();
		}
	}

	delete[] nodeTable;
	delete[] objectTable;

	return (root);
}

Node *Node::UnpackTree(const void *data, unsigned_int32 unpackFlags)
{
	int32	controllerCount;
	int32	objectCount;
	int32	nodeCount;
	int32	offsetCount;

	const int32 *format = static_cast<const int32 *>(data);
	Unpacker unpacker(format + 2, format[1]);

	unpacker >> controllerCount;
	unpacker >> objectCount;
	unpacker >> nodeCount;

	unpacker >> offsetCount;
	unpacker += offsetCount * 4;

	Object **objectTable = new Object *[objectCount];
	for (machine a = 0; a < objectCount; a++)
	{
		unsigned_int32	size;

		unpacker >> size;
		const void *mark = unpacker.GetPointer();

		Object *object = Object::Create(unpacker, unpackFlags);
		if (object)
		{
			object->Unpack(++unpacker, unpackFlags);

			if ((unpackFlags & (kUnpackNonpersistent | kUnpackExternal)) == 0)
			{
				object->SetObjectIndex(a);
			}

			objectTable[a] = object;
		}
		else
		{
			unpacker.Skip(mark, size);
			objectTable[a] = nullptr;
		}
	}

	return (LoadNodeTable(unpacker, unpackFlags, nodeCount, objectCount, objectTable));
}

FileResult Node::PackDeltaTree(File *file, const ResourceName& originalName) const
{
	int32			fileHeader[7];
	List<Object>	objectList;

	int32 controllerCount = 0;
	int32 nodeCount = 0;

	const Node *node = this;
	do
	{
		if (!(node->GetNodeFlags() & kNodeNonpersistent))
		{
			node->nodeIndex = nodeCount++;
			node->PrepackNodeObjects(&objectList);

			Controller *controller = node->GetController();
			if (controller)
			{
				controllerCount = Max(controller->GetControllerIndex() + 1, controllerCount);
			}

			node = GetNextNode(node);
		}
		else
		{
			node->nodeIndex = -1;

			Node *subnode = node->GetFirstSubnode();
			while (subnode)
			{
				subnode->nodeIndex = -1;
				subnode = node->GetNextNode(subnode);
			}

			node = GetNextLevelNode(node);
		}
	} while (node);

	int32 objectCount = 0;
	int32 modifiedCount = 0;

	Object *object = objectList.First();
	while (object)
	{
		int32 index = object->GetObjectIndex();
		if (index >= 0)
		{
			objectCount = Max(index + 1, objectCount);

			if (object->GetModifiedFlag())
			{
				modifiedCount++;
			}
		}

		object = object->Next();
	}

	int32 originalCount = objectCount;

	object = objectList.First();
	while (object)
	{
		if (object->GetObjectIndex() < 0)
		{
			object->SetObjectIndex(objectCount);
			object->SetModifiedFlag();
			objectCount++;
			modifiedCount++;
		}

		object = object->Next();
	}

	ResourceName name(originalName);
	unsigned_int32 nameLength = name.Length();
	unsigned_int32 nameSize = (nameLength + 4) & ~3;
	for (unsigned_machine a = nameLength + 1; a < nameSize; a++)
	{
		name[a] = 0;
	}

	fileHeader[0] = 1;
	fileHeader[1] = kEngineInternalVersion;
	fileHeader[2] = controllerCount;
	fileHeader[3] = objectCount;
	fileHeader[4] = modifiedCount;
	fileHeader[5] = nodeCount;
	fileHeader[6] = nameSize;

	FileResult result = file->Write(fileHeader, 28);
	if (result == kFileOkay)
	{
		result = file->Write(&name, nameSize);
	}

	Buffer buffer(kPackageDefaultSize);

	if (result == kFileOkay)
	{
		object = objectList.First();
		while (object)
		{
			if (object->GetModifiedFlag())
			{
				int32 index = object->GetObjectIndex();
				file->Write(&index, 4);

				Package package(buffer, kPackageDefaultSize);
				Packer packer(&package);

				PackHandle handle = packer.BeginSection();
				object->PackType(packer);
				object->Pack(packer, 0);
				packer.EndSection(handle);

				result = file->Write(package.GetStorage(), package.GetSize());
				if (result != kFileOkay)
				{
					break;
				}
			}

			object = object->Next();
		}
	}

	if (result == kFileOkay)
	{
		Node *super = GetSuperNode();
		if (super)
		{
			super->nodeIndex = -1;
		}

		node = this;
		do
		{
			if (!(node->GetNodeFlags() & kNodeNonpersistent))
			{
				Package package(buffer, kPackageDefaultSize);
				Packer packer(&package);

				PackHandle handle = packer.BeginSection();
				node->PackType(packer);
				node->Pack(packer, 0);
				packer.EndSection(handle);

				result = file->Write(package.GetStorage(), package.GetSize());
				if (result != kFileOkay)
				{
					break;
				}

				node = GetNextNode(node);
			}
			else
			{
				node = GetNextLevelNode(node);
			}
		} while (node);
	}

	for (;;)
	{
		object = objectList.First();
		if (!object)
		{
			break;
		}

		if (object->GetObjectIndex() >= originalCount)
		{
			object->SetObjectIndex(-1);
		}

		objectList.Remove(object);
	}

	return (result);
}

Node *Node::UnpackDeltaTree(const void *data, ResourceName& originalName, World *previousWorld)
{
	int32	originalObjectCount;
	int32	totalObjectCount;
	int32	controllerCount;
	int32	objectCount;
	int32	modifiedCount;
	int32	nodeCount;

	const int32 *format = static_cast<const int32 *>(data);
	Unpacker unpacker(format + 2, format[1]);

	unpacker >> controllerCount;
	unpacker >> objectCount;
	unpacker >> modifiedCount;
	unpacker >> nodeCount;

	unpacker >> originalName;
	Object **objectTable = LoadOriginalObjects(originalName, previousWorld, objectCount, &originalObjectCount, &totalObjectCount);
	if (!objectTable)
	{
		return (nullptr);
	}

	for (machine a = 0; a < modifiedCount; a++)
	{
		int32			index;
		unsigned_int32	size;

		unpacker >> index;
		if (index < originalObjectCount)
		{
			Object *originalObject = objectTable[index];
			if (originalObject)
			{
				originalObject->Release();
			}
		}

		unpacker >> size;
		const void *mark = unpacker.GetPointer();

		Object *object = Object::Create(unpacker);
		if (object)
		{
			object->Unpack(++unpacker, 0);
			object->SetObjectIndex(index);
			object->SetModifiedFlag();
			objectTable[index] = object;
		}
		else
		{
			unpacker.Skip(mark, size);
			objectTable[index] = nullptr;
		}
	}

	return (LoadNodeTable(unpacker, 0, nodeCount, totalObjectCount, objectTable));
}


RenderableNode::RenderableNode(NodeType type, RenderType renderType, unsigned_int32 renderState) :
		Node(type),
		Renderable(renderType, renderState)
{
	shaderParameterProc = nullptr;
}

RenderableNode::RenderableNode(const RenderableNode& renderableNode) :
		Node(renderableNode),
		Renderable(renderableNode.GetRenderType(), renderableNode.GetRenderState())
{
	shaderParameterProc = nullptr;
}

RenderableNode::~RenderableNode()
{
}

void RenderableNode::HandlePostprocessUpdate(void)
{
	const Bond *bond = GetFirstOutgoingEdge();
	if (bond)
	{
		Box3D box = Union(GetWorldBoundingBox(), bond->GetFinishElement()->GetWorldBoundingBox());
		for (;;)
		{
			bond = bond->GetNextOutgoingEdge();
			if (!bond)
			{
				break;
			}

			box.Union(bond->GetFinishElement()->GetWorldBoundingBox());
		}

		SetWorldBoundingBox(box);
	}
}

void RenderableNode::SelectAmbientEnvironment(const Node *node)
{
	const ZoneMembershipArray& zoneArray = node->GetZoneMembershipArray();
	int32 zoneCount = zoneArray.GetElementCount();
	if (zoneCount > 0)
	{
		const AmbientEnvironment	*ambientEnvironment;

		if (!(node->GetNodeFlags() & kNodeSubzoneAmbient))
		{
			// Starting with the last zone ensures that the outermost zone has the highest priority.

			ambientEnvironment = zoneArray[zoneCount - 1]->GetAmbientEnvironment();
			int32 shaderTypeDelta = ambientEnvironment->ambientShaderTypeDelta;

			for (machine a = zoneCount - 2; a >= 0; a--)
			{
				const AmbientEnvironment *environment = zoneArray[a]->GetAmbientEnvironment();
				int32 delta = environment->ambientShaderTypeDelta;

				if (delta > shaderTypeDelta)
				{
					ambientEnvironment = environment;
					shaderTypeDelta = delta;
				}
			}
		}
		else
		{
			ambientEnvironment = zoneArray[0]->GetAmbientEnvironment();
			int32 shaderTypeDelta = ambientEnvironment->ambientShaderTypeDelta;

			for (machine a = 1; a < zoneCount; a++)
			{
				const AmbientEnvironment *environment = zoneArray[a]->GetAmbientEnvironment();
				int32 delta = environment->ambientShaderTypeDelta;

				if (delta > shaderTypeDelta)
				{
					ambientEnvironment = environment;
					shaderTypeDelta = delta;
				}
			}
		}

		if (GetAmbientEnvironment() != ambientEnvironment)
		{
			SetAmbientEnvironment(ambientEnvironment);
			InvalidateAmbientShaderData();
		}
	}
}

void RenderableNode::Neutralize(void)
{
	InvalidateShaderData();
	Node::Neutralize();
}

const float *RenderableNode::GetShaderParameterPointer(int32 slot) const
{
	if (shaderParameterProc)
	{
		return ((*shaderParameterProc)(slot, shaderParameterCookie));
	}

	return (nullptr);
}

// ZYUQURM
