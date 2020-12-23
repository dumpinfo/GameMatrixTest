 

#ifndef C4Connector_h
#define C4Connector_h


//# \component	World Manager
//# \prefix		WorldMgr/


#include "C4Types.h"


namespace C4
{
	typedef Type	NodeType;


	enum
	{
		kMaxConnectorKeyLength	= 15
	};

	typedef String<kMaxConnectorKeyLength>	ConnectorKey;


	enum
	{
		kConnectorSaveFinishPersistent		= 1 << 0
	};


	class Hub;
	class Node;


	//# \class	Connector	Represents an edge in the graph of hubs and connectors.
	//
	//# The $Connector$ class represents an edge in the graph of hubs and connectors.
	//
	//# \def	class Connector : public GraphEdge<Hub, Connector>
	//
	//# \ctor	Connector(Hub *hub, const char *key);
	//
	//# \param	hub		The hub for which the connector is an outgoing edge.
	//# \param	key		The key value for the connector. This is a string up to 15 bytes in length, not counting the null terminator.
	//
	//# \desc
	//# Every node may have a $@Hub@$ to which one or more connectors are attached, allowing the node to link to other nodes in a scene.
	//# An instance of the $Connector$ class is used to represent each connector attached to a node's connector hub.
	//#
	//# $Connector$ objects are not normally constructed directly, but are instead attached to a node's connector hub by
	//# calling the $@Node::AddConnector@$ function.
	//#
	//# When a $Connector$ object is first constructed, both the start and finish elements for the connector are set to the hub
	//# specified by the $hub$ parameter. This indicates that the connector is not connected to another node.
	//
	//# \base	Utilities/GraphEdge<Hub, Connector>		Connectors form the edges in a directed graph in which the nodes are hubs.
	//
	//# \also	$@Hub@$
	//# \also	$@Node::GetHub@$
	//# \also	$@Node::AddConnector@$
	//# \also	$@Node::GetConnectedNode@$
	//# \also	$@Node::SetConnectedNode@$
	//
	//# \wiki	Connectors


	//# \function	Connector::GetConnectorKey		Returns the key value for a connector.
	//
	//# \proto	const ConnectorKey& GetConnectorKey(void) const;
	//
	//# \desc
	//# The $GetConnectorKey$ function returns the key value for a connector. This is the same key used to identify a
	//# connector by the $@Node::GetConnectedNode@$ function.
	//
	//# \also	$@Node::GetConnectedNode@$
	//# \also	$@Node::SetConnectedNode@$


	class Connector : public GraphEdge<Hub, Connector>, public EngineMemory<Connector>
	{
		private:

			ConnectorKey		connectorKey;
			unsigned_int32		connectorFlags;

		public:

			C4API Connector(Hub *hub, const char *key);
			Connector(Hub *start, Hub *finish, const char *key);
			Connector(Hub *hub, const Connector& connector);
			C4API ~Connector();

			const ConnectorKey& GetConnectorKey(void) const
			{
				return (connectorKey);
			}

			void SetConnectorKey(const char *key)
			{
				connectorKey = key;
			} 

			unsigned_int32 GetConnectorFlags(void) const
			{ 
				return (connectorFlags);
			} 

			void SetConnectorFlags(unsigned_int32 flags)
			{ 
				connectorFlags = flags;
			} 
 
			C4API Node *GetConnectorTarget(void) const;
			C4API void SetConnectorTarget(Node *node);

			C4API static bool ConnectorKeyFilter(unsigned_int32 code); 
	};


	//# \class	Hub		Represents a node in the graph of hubs and connectors.
	//
	//# The $Hub$ class represents a node in the graph of hubs and connectors.
	//
	//# \def	class Hub : public GraphElement<Hub, Connector>
	//
	//# \ctor	Hub(Node *node);
	//
	//# \param	node		The node associated with the hub.
	//
	//# \desc
	//# Every node may have a $@Hub@$ to which one or more connectors are attached, allowing the node to link to other nodes in a scene.
	//# Whenever a connector links two nodes, the nodes at both ends of the connector have an instance of the $Hub$ class attached to them.
	//# The $Hub$ objects are created automatically when connections are made through functions such as $@Node::AddConnector@$ and
	//# $@Node::SetConnectedNode@$.
	//
	//# \base	Utilities/GraphElement<Hub, Connector>		Hubs form the elements in a directed graph in which the edges are connectors.
	//
	//# \also	$@Connector@$
	//# \also	$@Node::GetHub@$


	class Hub : public GraphElement<Hub, Connector>, public EngineMemory<Connector>
	{
		private:

			Node	*hubNode;

		public:

			Hub(Node *node);
			~Hub();

			Node *GetNode(void) const
			{
				return (hubNode);
			}

			C4API bool HasOutgoingConnection(void) const;
			C4API bool HasIncomingConnection(void) const;

			C4API void BreakAllOutgoingConnections(void);
			C4API void BreakAllIncomingConnections(void);

			C4API Connector *FindOutgoingConnector(const char *key) const;
			C4API Connector *FindIncomingConnector(const char *key) const;
	};
}


#endif

// ZYUQURM
