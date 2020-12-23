 

#include "C4Connector.h"
#include "C4Node.h"


using namespace C4;


namespace C4
{
	template <> Heap EngineMemory<Connector>::heap("Connector", 16384);
	template class EngineMemory<Connector>;
}


Connector::Connector(Hub *hub, const char *key) : GraphEdge<Hub, Connector>(hub, hub)
{
	connectorKey = key;
	connectorFlags = 0;
}

Connector::Connector(Hub *start, Hub *finish, const char *key) : GraphEdge<Hub, Connector>(start, finish)
{
	connectorKey = key;
	connectorFlags = 0;
}

Connector::Connector(Hub *hub, const Connector& connector) : GraphEdge<Hub, Connector>(hub, hub)
{
	connectorKey = connector.connectorKey;
	connectorFlags = 0;
}

Connector::~Connector()
{
}

Node *Connector::GetConnectorTarget(void) const
{
	const Hub *finish = GetFinishElement();
	if (finish != GetStartElement())
	{
		return (finish->GetNode());
	}

	return (nullptr);
}

void Connector::SetConnectorTarget(Node *node)
{
	if (node)
	{
		Hub *finish = node->GetHub();
		if (!finish)
		{
			finish = new Hub(node);
		}

		SetFinishElement(finish);
	}
	else
	{
		Hub *finish = GetFinishElement();
		SetFinishElement(GetStartElement());

		if (finish->Isolated())
		{
			delete finish;
		}
	}
}

bool Connector::ConnectorKeyFilter(unsigned_int32 code)
{
	return (code - '$' > 2U);
}


Hub::Hub(Node *node)
{
	hubNode = node;
	node->nodeHub = this;
}

Hub::~Hub()
{
	Connector *connector = GetFirstIncomingEdge();
	while (connector)
	{
		Connector *next = connector->GetNextIncomingEdge();

		Hub *start = connector->GetStartElement();
		if (start != this)
		{
			connector->SetFinishElement(start);
		}

		connector = next;
	}

	hubNode->nodeHub = nullptr;
} 

bool Hub::HasOutgoingConnection(void) const
{ 
	const Connector *connector = GetFirstOutgoingEdge();
	while (connector) 
	{
		if (connector->GetFinishElement() != this)
		{ 
			return (true);
		} 
 
		connector = connector->GetNextOutgoingEdge();
	}

	return (false); 
}

bool Hub::HasIncomingConnection(void) const
{
	const Connector *connector = GetFirstIncomingEdge();
	while (connector)
	{
		if (connector->GetStartElement() != this)
		{
			return (true);
		}

		connector = connector->GetNextIncomingEdge();
	}

	return (false);
}

void Hub::BreakAllOutgoingConnections(void)
{
	Connector *connector = GetFirstOutgoingEdge();
	while (connector)
	{
		Connector *next = connector->GetNextOutgoingEdge();
		if (connector->GetFinishElement() != this)
		{
			connector->SetConnectorTarget(nullptr);
		}

		connector = next;
	}

	// At this point, the hub itself may have been deleted by the last call to the
	// SetConnectorTarget() function if there are no longer any connectors attached to it.
}

void Hub::BreakAllIncomingConnections(void)
{
	Connector *connector = GetFirstIncomingEdge();
	while (connector)
	{
		Connector *next = connector->GetNextIncomingEdge();
		if (connector->GetStartElement() != this)
		{
			connector->SetConnectorTarget(nullptr);
		}

		connector = next;
	}

	// At this point, the hub itself may have been deleted by the last call to the
	// SetConnectorTarget() function if there are no longer any connectors attached to it.
}

Connector *Hub::FindOutgoingConnector(const char *key) const
{
	Connector *connector = GetFirstOutgoingEdge();
	while (connector)
	{
		if (connector->GetConnectorKey() == key)
		{
			return (connector);
		}

		connector = connector->GetNextOutgoingEdge();
	}

	return (nullptr);
}

Connector *Hub::FindIncomingConnector(const char *key) const
{
	Connector *connector = GetFirstIncomingEdge();
	while (connector)
	{
		if (connector->GetConnectorKey() == key)
		{
			return (connector);
		}

		connector = connector->GetNextIncomingEdge();
	}

	return (nullptr);
}

// ZYUQURM
