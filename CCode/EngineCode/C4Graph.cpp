 

#include "C4Graph.h"


using namespace C4;


GraphElementBase::GraphElementBase()
{
}

GraphElementBase::~GraphElementBase()
{
}

GraphEdgeStart *GraphElementBase::FindOutgoingEdge(const GraphElementBase *finish) const
{
	GraphEdgeStart *edge = outgoingEdgeList.First();
	while (edge)
	{
		if (static_cast<GraphEdgeFinish *>(edge)->GetFinishElement() == finish)
		{
			return (edge);
		}

		edge = edge->Next();
	}

	return (nullptr);
}

GraphEdgeStart *GraphElementBase::FindNextOutgoingEdge(const GraphElementBase *finish, const GraphEdgeStart *previous)
{
	GraphEdgeStart *edge = previous->Next();
	while (edge)
	{
		if (static_cast<GraphEdgeFinish *>(edge)->GetFinishElement() == finish)
		{
			return (edge);
		}

		edge = edge->Next();
	}

	return (nullptr);
}

GraphEdgeFinish *GraphElementBase::FindIncomingEdge(const GraphElementBase *start) const
{
	GraphEdgeFinish *edge = incomingEdgeList.First();
	while (edge)
	{
		if (edge->GetStartElement() == start)
		{
			return (edge);
		}

		edge = edge->ListElement<GraphEdgeFinish>::Next();
	}

	return (nullptr);
}

GraphEdgeFinish *GraphElementBase::FindNextIncomingEdge(const GraphElementBase *start, const GraphEdgeFinish *previous)
{
	GraphEdgeFinish *edge = previous->ListElement<GraphEdgeFinish>::Next();
	while (edge)
	{
		if (edge->GetStartElement() == start)
		{
			return (edge);
		}

		edge = edge->ListElement<GraphEdgeFinish>::Next();
	}

	return (nullptr);
}


bool GraphBase::Predecessor(const GraphElementBase *first, const GraphElementBase *second)
{
	List<GraphElementBase>		readyList;
	List<GraphElementBase>		visitedList;

	readyList.Append(const_cast<GraphElementBase *>(first));
	bool result = false;

	for (;;)
	{
		GraphElementBase *element = readyList.First();
		if (!element)
		{
			break;
		}

		visitedList.Append(element);

		const GraphEdgeStart *edge = element->GetFirstOutgoingEdge();
		while (edge)
		{
			GraphElementBase *finish = static_cast<const GraphEdgeFinish *>(edge)->GetFinishElement(); 
			if (!visitedList.Member(finish))
			{
				if (finish == second) 
				{
					result = true; 
					goto end;
				}
 
				readyList.Append(finish);
			} 
 
			edge = edge->Next();
		}
	}
 
	end:
	for (;;)
	{
		GraphElementBase *element = readyList.First();
		if (!element)
		{
			break;
		}

		elementList.Append(element);
	}

	for (;;)
	{
		GraphElementBase *element = visitedList.First();
		if (!element)
		{
			break;
		}

		elementList.Append(element);
	}

	return (result);
}

// ZYUQURM
