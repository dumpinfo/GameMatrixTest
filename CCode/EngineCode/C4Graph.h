 

#ifndef C4Graph_h
#define C4Graph_h


//# \component	Utility Library
//# \prefix		Utilities/


#include "C4List.h"


namespace C4
{
	class GraphElementBase;
	template <class, class> class GraphElement;
	template <class, class> class Graph;


	class GraphEdgeStart : public ListElement<GraphEdgeStart>
	{
		friend class GraphElementBase;

		private:

			GraphElementBase	*startElement;

			GraphEdgeStart(const GraphEdgeStart&) = delete;
			GraphEdgeStart& operator =(const GraphEdgeStart&) = delete;

		protected:

			GraphEdgeStart(GraphElementBase *start);

			~GraphEdgeStart()
			{
			}

			GraphElementBase *GetStartElement(void) const
			{
				return (startElement);
			}

			void SetStartElement(GraphElementBase *start);
			void Attach(void);
	};


	class GraphEdgeFinish : public GraphEdgeStart, public ListElement<GraphEdgeFinish>
	{
		friend class GraphElementBase;
		friend class GraphBase;

		private:

			GraphElementBase	*finishElement;

		protected:

			GraphEdgeFinish(GraphElementBase *start, GraphElementBase *finish);

			~GraphEdgeFinish()
			{
			}

			GraphElementBase *GetFinishElement(void) const
			{
				return (finishElement);
			}

			void SetFinishElement(GraphElementBase *finish);

		public:

			void Detach(void)
			{
				ListElement<GraphEdgeStart>::Detach();
				ListElement<GraphEdgeFinish>::Detach();
			}

			void Attach(void);
	};


	class GraphElementBase : public ListElement<GraphElementBase>
	{
		friend class GraphEdgeStart;
		friend class GraphEdgeFinish;
		friend class GraphBase;

		private:

			List<GraphEdgeStart>	outgoingEdgeList;
			List<GraphEdgeFinish>	incomingEdgeList;

			GraphElementBase(const GraphElementBase&) = delete;
			GraphElementBase& operator =(const GraphElementBase&) = delete;

		protected:

			C4API GraphElementBase();
			C4API ~GraphElementBase(); 

			GraphEdgeStart *GetFirstOutgoingEdge(void) const
			{ 
				return (outgoingEdgeList.First());
			} 

			GraphEdgeStart *GetLastOutgoingEdge(void) const
			{ 
				return (outgoingEdgeList.Last());
			} 
 
			GraphEdgeFinish *GetFirstIncomingEdge(void) const
			{
				return (incomingEdgeList.First());
			} 

			GraphEdgeFinish *GetLastIncomingEdge(void) const
			{
				return (incomingEdgeList.Last());
			}

			GraphEdgeStart *GetOutgoingEdge(int32 index) const
			{
				return (outgoingEdgeList[index]);
			}

			GraphEdgeFinish *GetIncomingEdge(int32 index) const
			{
				return (incomingEdgeList[index]);
			}

			C4API GraphEdgeStart *FindOutgoingEdge(const GraphElementBase *finish) const;
			C4API static GraphEdgeStart *FindNextOutgoingEdge(const GraphElementBase *finish, const GraphEdgeStart *previous);

			C4API GraphEdgeFinish *FindIncomingEdge(const GraphElementBase *start) const;
			C4API static GraphEdgeFinish *FindNextIncomingEdge(const GraphElementBase *start, const GraphEdgeFinish *previous);

		public:

			int32 GetOutgoingEdgeCount(void) const
			{
				return (outgoingEdgeList.GetElementCount());
			}

			int32 GetIncomingEdgeCount(void) const
			{
				return (incomingEdgeList.GetElementCount());
			}

			bool Isolated(void) const
			{
				return ((outgoingEdgeList.Empty()) && (incomingEdgeList.Empty()));
			}

			void AddOutgoingEdge(GraphEdgeStart *edge)
			{
				outgoingEdgeList.Append(edge);
				edge->startElement = this;
			}

			void AddIncomingEdge(GraphEdgeFinish *edge)
			{
				incomingEdgeList.Append(edge);
				edge->finishElement = this;
			}

			void PurgeOutgoingEdges(void)
			{
				outgoingEdgeList.Purge();
			}

			void PurgeIncomingEdges(void)
			{
				incomingEdgeList.Purge();
			}
	};


	//# \class	GraphEdge		The base class for edges connecting elements in a directed graph.
	//
	//# The $GraphEdge$ class is the base class for edges connecting elements in a directed graph.
	//
	//# \def	template <class elementType, class edgeType> class GraphEdge : public GraphEdgeFinish
	//
	//# \tparam		elementType		The type of the class that can be stored as an element in the graph.
	//# \tparam		edgeType		The type of the class that represents the edges in the graph. This parameter should
	//#								be the type of the class that inherits directly from the $GraphEdge$ class.
	//
	//# \ctor	GraphEdge(GraphElement<elementType, edgeType> *start, GraphElement<elementType, edgeType> *finish);
	//
	//# \param	start	A pointer to the element at the start of the edge.
	//# \param	finish	A pointer to the element at the finish of the edge.
	//
	//# The constructor has protected access. The $GraphEdge$ class can only exist
	//# as a base class for another class.
	//
	//# \desc
	//# The $GraphEdge$ class should be declared as a base class for objects that represent edges in a directed graph.
	//# The $edgeType$ template parameter should match the class type of such objects. The $elementType$ template
	//# parameter should match the type of the class representing the elements in the graph.
	//#
	//# The $start$ and $finish$ parameters specify the graph elements at the start and finish of the directed edge.
	//# These may not be $nullptr$. When a $GraphEdge$ instance is constructed, the edge automatically becomes an
	//# outgoing edge for the element specified by the $start$ parameter and an incoming edge for the element specified
	//# by the $finish$ parameter.
	//
	//# \privbase	GraphEdgeFinish		Used internally to encapsulate common functionality that is independent
	//#									of the template parameter.
	//
	//# \also	$@GraphElement@$
	//# \also	$@Graph@$


	//# \function	GraphEdge::GetStartElement		Returns the graph element at which an edge starts.
	//
	//# \proto		elementType *GetStartElement(void) const;
	//
	//# \desc
	//# The $GetStartElement$ function returns the graph element at which an edge starts.
	//
	//# \also	$@GraphEdge::SetStartElement@$
	//# \also	$@GraphEdge::GetFinishElement@$
	//# \also	$@GraphEdge::SetFinishElement@$


	//# \function	GraphEdge::SetStartElement		Sets the graph element at which an edge starts.
	//
	//# \proto		void SetStartElement(elementType *start);
	//
	//# \param	start	The new start element. This may not be $nullptr$.
	//
	//# \desc
	//# The $SetStartElement$ function sets the graph element at which an edge starts to that specified by the $start$ parameter.
	//# The edge is removed from the list of outgoing edges for its previous start element, and it is added to the list of
	//# outgoing edges for its new start element.
	//
	//# \also	$@GraphEdge::GetStartElement@$
	//# \also	$@GraphEdge::GetFinishElement@$
	//# \also	$@GraphEdge::SetFinishElement@$


	//# \function	GraphEdge::GetFinishElement		Returns the graph element at which an edge finishes.
	//
	//# \proto		elementType *GetFinishElement(void) const;
	//
	//# \desc
	//# The $GetFinishElement$ function returns the graph element at which an edge finishes.
	//
	//# \also	$@GraphEdge::SetFinishElement@$
	//# \also	$@GraphEdge::GetStartElement@$
	//# \also	$@GraphEdge::SetStartElement@$


	//# \function	GraphEdge::SetFinishElement		Sets the graph element at which an edge finishes.
	//
	//# \proto		void SetFinishElement(elementType *finish);
	//
	//# \param	finish	The new finish element. This may not be $nullptr$.
	//
	//# \desc
	//# The $SetFinishElement$ function sets the graph element at which an edge finishes to that specified by the $finish$ parameter.
	//# The edge is removed from the list of incoming edges for its previous finish element, and it is added to the list of
	//# incoming edges for its new finish element.
	//
	//# \also	$@GraphEdge::GetFinishElement@$
	//# \also	$@GraphEdge::GetStartElement@$
	//# \also	$@GraphEdge::SetStartElement@$


	//# \function	GraphEdge::GetPreviousOutgoingEdge		Returns the previous outgoing edge for a graph element.
	//
	//# \proto		edgeType *GetPreviousOutgoingEdge(void) const;
	//
	//# \desc
	//# The $GetPreviousOutgoingEdge$ function returns the previous outgoing edge for the graph element
	//# at which the edge for which the function is called starts. If there is no previous outgoing edge,
	//# then the return value is $nullptr$.
	//#
	//# To iterate through all outgoing edges for a graph element using this function, first call the
	//# $@GraphElement::GetLastOutgoingEdge@$ function for the graph element, and then call the $GetPreviousOutgoingEdge$
	//# function iteratively for the returned edges until $nullptr$ is returned.
	//
	//# \also	$@GraphEdge::GetNextOutgoingEdge@$
	//# \also	$@GraphEdge::GetPreviousIncomingEdge@$
	//# \also	$@GraphEdge::GetNextIncomingEdge@$
	//# \also	$@GraphElement::GetLastOutgoingEdge@$


	//# \function	GraphEdge::GetNextOutgoingEdge		Returns the next outgoing edge for a graph element.
	//
	//# \proto		edgeType *GetNextOutgoingEdge(void) const;
	//
	//# \desc
	//# The $GetNextOutgoingEdge$ function returns the next outgoing edge for the graph element
	//# at which the edge for which the function is called starts. If there is no next outgoing edge,
	//# then the return value is $nullptr$.
	//#
	//# To iterate through all outgoing edges for a graph element using this function, first call the
	//# $@GraphElement::GetFirstOutgoingEdge@$ function for the graph element, and then call the $GetNextOutgoingEdge$
	//# function iteratively for the returned edges until $nullptr$ is returned.
	//
	//# \also	$@GraphEdge::GetPreviousOutgoingEdge@$
	//# \also	$@GraphEdge::GetPreviousIncomingEdge@$
	//# \also	$@GraphEdge::GetNextIncomingEdge@$
	//# \also	$@GraphElement::GetFirstOutgoingEdge@$


	//# \function	GraphEdge::GetPreviousIncomingEdge		Returns the previous incoming edge for a graph element.
	//
	//# \proto		edgeType *GetPreviousIncomingEdge(void) const;
	//
	//# \desc
	//# The $GetPreviousIncomingEdge$ function returns the previous incoming edge for the graph element
	//# at which the edge for which the function is called finishes. If there is no previous incoming edge,
	//# then the return value is $nullptr$.
	//#
	//# To iterate through all incoming edges for a graph element using this function, first call the
	//# $@GraphElement::GetLastIncomingEdge@$ function for the graph element, and then call the $GetPreviousIncomingEdge$
	//# function iteratively for the returned edges until $nullptr$ is returned.
	//
	//# \also	$@GraphEdge::GetNextIncomingEdge@$
	//# \also	$@GraphEdge::GetPreviousOutgoingEdge@$
	//# \also	$@GraphEdge::GetNextOutgoingEdge@$
	//# \also	$@GraphElement::GetLastIncomingEdge@$


	//# \function	GraphEdge::GetNextIncomingEdge		Returns the next incoming edge for a graph element.
	//
	//# \proto		edgeType *GetNextIncomingEdge(void) const;
	//
	//# \desc
	//# The $GetNextIncomingEdge$ function returns the next incoming edge for the graph element
	//# at which the edge for which the function is called finishes. If there is no next incoming edge,
	//# then the return value is $nullptr$.
	//#
	//# To iterate through all incoming edges for a graph element using this function, first call the
	//# $@GraphElement::GetFirstIncomingEdge@$ function for the graph element, and then call the $GetNextIncomingEdge$
	//# function iteratively for the returned edges until $nullptr$ is returned.
	//
	//# \also	$@GraphEdge::GetPreviousIncomingEdge@$
	//# \also	$@GraphEdge::GetPreviousOutgoingEdge@$
	//# \also	$@GraphEdge::GetNextOutgoingEdge@$
	//# \also	$@GraphElement::GetFirstIncomingEdge@$


	template <class elementType, class edgeType> class GraphEdge : public GraphEdgeFinish
	{
		protected:

			GraphEdge(GraphElement<elementType, edgeType> *start, GraphElement<elementType, edgeType> *finish) : GraphEdgeFinish(start, finish)
			{
			}

		public:

			elementType *GetStartElement(void) const
			{
				return (static_cast<elementType *>(static_cast<GraphElement<elementType, edgeType> *>(GraphEdgeStart::GetStartElement())));
			}

			void SetStartElement(elementType *start)
			{
				GraphEdgeStart::SetStartElement(start);
			}

			elementType *GetFinishElement(void) const
			{
				return (static_cast<elementType *>(static_cast<GraphElement<elementType, edgeType> *>(GraphEdgeFinish::GetFinishElement())));
			}

			void SetFinishElement(elementType *finish)
			{
				GraphEdgeFinish::SetFinishElement(finish);
			}

			edgeType *GetPreviousOutgoingEdge(void) const
			{
				return (static_cast<edgeType *>(static_cast<GraphEdge<elementType, edgeType> *>(ListElement<GraphEdgeStart>::Previous())));
			}

			edgeType *GetNextOutgoingEdge(void) const
			{
				return (static_cast<edgeType *>(static_cast<GraphEdge<elementType, edgeType> *>(ListElement<GraphEdgeStart>::Next())));
			}

			edgeType *GetPreviousIncomingEdge(void) const
			{
				return (static_cast<edgeType *>(static_cast<GraphEdge<elementType, edgeType> *>(ListElement<GraphEdgeFinish>::Previous())));
			}

			edgeType *GetNextIncomingEdge(void) const
			{
				return (static_cast<edgeType *>(static_cast<GraphEdge<elementType, edgeType> *>(ListElement<GraphEdgeFinish>::Next())));
			}
	};


	//# \class	GraphElement		The base class for elements in a directed graph.
	//
	//# The $GraphElement$ class is the base class for elements in a directed graph.
	//
	//# \def	template <class elementType, class edgeType> class GraphElement : public GraphElementBase
	//
	//# \tparam		elementType		The type of the class that can be stored as an element in the graph. This parameter should
	//#								be the type of the class that inherits directly from the $GraphElement$ class.
	//# \tparam		edgeType		The type of the class that represents the edges in the graph.
	//
	//# \ctor	GraphElement();
	//# \ctor	GraphElement(Graph<elementType, edgeType> *graph);
	//
	//# \param	graph	The graph to which the element should be added.
	//
	//# The constructor has protected access. The $GraphElement$ class can only exist
	//# as a base class for another class.
	//
	//# \desc
	//# The $GraphElement$ class should be declared as a base class for objects that represent elements in a directed graph.
	//# The $elementType$ template parameter should match the class type of such objects. The $edgeType$ template
	//# parameter should match the type of the class representing the edges in the graph.
	//#
	//# If the $graph$ parameter is specified, then the element is added to the graph object that it points to.
	//
	//# \privbase	GraphElementBase	Used internally to encapsulate common functionality that is independent
	//#								of the template parameter.
	//
	//# \also	$@GraphEdge@$
	//# \also	$@Graph@$


	//# \function	GraphElement::GetFirstOutgoingEdge		Returns the first outgoing edge for a graph element.
	//
	//# \proto		edgeType *GetFirstOutgoingEdge(void) const;
	//
	//# \desc
	//# The $GetFirstOutgoingEdge$ function returns the first outgoing edge for a graph element.
	//# If there are no outgoing edges, then the return value is $nullptr$.
	//#
	//# To iterate through all outgoing edges for a graph element, first call the $@GraphElement::GetFirstOutgoingEdge@$
	//# function, and then call the $GraphEdge::GetNextOutgoingEdge$ function iteratively for the returned edges
	//# until $nullptr$ is returned.
	//
	//# \also	$@GraphElement::GetLastOutgoingEdge@$
	//# \also	$@GraphElement::GetFirstIncomingEdge@$
	//# \also	$@GraphElement::GetLastIncomingEdge@$
	//# \also	$@GraphEdge::GetNextOutgoingEdge@$


	//# \function	GraphElement::GetLastOutgoingEdge		Returns the last outgoing edge for a graph element.
	//
	//# \proto		edgeType *GetLastOutgoingEdge(void) const;
	//
	//# \desc
	//# The $GetLastOutgoingEdge$ function returns the last outgoing edge for a graph element.
	//# If there are no outgoing edges, then the return value is $nullptr$.
	//#
	//# To iterate through all outgoing edges for a graph element, first call the $@GraphElement::GetLastOutgoingEdge@$
	//# function, and then call the $GraphEdge::GetPreviousOutgoingEdge$ function iteratively for the returned edges
	//# until $nullptr$ is returned.
	//
	//# \also	$@GraphElement::GetFirstOutgoingEdge@$
	//# \also	$@GraphElement::GetFirstIncomingEdge@$
	//# \also	$@GraphElement::GetLastIncomingEdge@$
	//# \also	$@GraphEdge::GetPreviousOutgoingEdge@$


	//# \function	GraphElement::GetFirstIncomingEdge		Returns the first incoming edge for a graph element.
	//
	//# \proto		edgeType *GetFirstIncomingEdge(void) const;
	//
	//# \desc
	//# The $GetFirstIncomingEdge$ function returns the first incoming edge for a graph element.
	//# If there are no incoming edges, then the return value is $nullptr$.
	//#
	//# To iterate through all incoming edges for a graph element, first call the $@GraphElement::GetFirstIncomingEdge@$
	//# function, and then call the $GraphEdge::GetNextIncomingEdge$ function iteratively for the returned edges
	//# until $nullptr$ is returned.
	//
	//# \also	$@GraphElement::GetLastIncomingEdge@$
	//# \also	$@GraphElement::GetFirstOutgoingEdge@$
	//# \also	$@GraphElement::GetLastOutgoingEdge@$
	//# \also	$@GraphEdge::GetNextIncomingEdge@$


	//# \function	GraphElement::GetLastIncomingEdge		Returns the last incoming edge for a graph element.
	//
	//# \proto		edgeType *GetLastIncomingEdge(void) const;
	//
	//# \desc
	//# The $GetLastIncomingEdge$ function returns the last incoming edge for a graph element.
	//# If there are no incoming edges, then the return value is $nullptr$.
	//#
	//# To iterate through all incoming edges for a graph element, first call the $@GraphElement::GetLastIncomingEdge@$
	//# function, and then call the $GraphEdge::GetPreviousIncomingEdge$ function iteratively for the returned edges
	//# until $nullptr$ is returned.
	//
	//# \also	$@GraphElement::GetFirstIncomingEdge@$
	//# \also	$@GraphElement::GetFirstOutgoingEdge@$
	//# \also	$@GraphElement::GetLastOutgoingEdge@$
	//# \also	$@GraphEdge::GetPreviousIncomingEdge@$


	//# \function	GraphElement::FindOutgoingEdge		Returns the outgoing edge with a specific finish element.
	//
	//# \proto		edgeType *FindOutgoingEdge(const GraphElementBase *finish) const;
	//
	//# \param	finish		The graph element at which the edge finishes.
	//
	//# \desc
	//# The $FindOutgoingEdge$ function searches for an outgoing edge starting at the graph element for which
	//# the function is called and finishing at the graph element specified by the $finish$ parameter. If such
	//# an edge is found, then it is returned. Otherwise, the return value is $nullptr$.
	//
	//# \also	$@GraphElement::FindIncomingEdge@$


	//# \function	GraphElement::FindIncomingEdge		Returns the incoming edge with a specific start element.
	//
	//# \proto		edgeType *FindIncomingEdge(const GraphElementBase *start) const;
	//
	//# \param	start		The graph element at which the edge start.
	//
	//# \desc
	//# The $FindIncomingEdge$ function searches for an incoming edge starting at the graph element specified
	//# by the $start$ parameter and finishing at the graph element for which the function is called. If such
	//# an edge is found, then it is returned. Otherwise, the return value is $nullptr$.
	//
	//# \also	$@GraphElement::FindOutgoingEdge@$


	template <class elementType, class edgeType> class GraphElement : public GraphElementBase
	{
		protected:

			GraphElement()
			{
			}

			GraphElement(Graph<elementType, edgeType> *graph)
			{
				graph->AddElement(this);
			}

		public:

			elementType *GetPreviousElement(void) const
			{
				return (static_cast<elementType *>(static_cast<GraphElement<elementType, edgeType> *>(GraphElementBase::Previous())));
			}

			elementType *GetNextElement(void) const
			{
				return (static_cast<elementType *>(static_cast<GraphElement<elementType, edgeType> *>(GraphElementBase::Next())));
			}

			edgeType *GetFirstOutgoingEdge(void) const
			{
				return (static_cast<edgeType *>(static_cast<GraphEdge<elementType, edgeType> *>(GraphElementBase::GetFirstOutgoingEdge())));
			}

			edgeType *GetLastOutgoingEdge(void) const
			{
				return (static_cast<edgeType *>(static_cast<GraphEdge<elementType, edgeType> *>(GraphElementBase::GetLastOutgoingEdge())));
			}

			edgeType *GetFirstIncomingEdge(void) const
			{
				return (static_cast<edgeType *>(static_cast<GraphEdge<elementType, edgeType> *>(GraphElementBase::GetFirstIncomingEdge())));
			}

			edgeType *GetLastIncomingEdge(void) const
			{
				return (static_cast<edgeType *>(static_cast<GraphEdge<elementType, edgeType> *>(GraphElementBase::GetLastIncomingEdge())));
			}

			edgeType *GetOutgoingEdge(int32 index) const
			{
				return (static_cast<edgeType *>(static_cast<GraphEdge<elementType, edgeType> *>(GraphElementBase::GetOutgoingEdge(index))));
			}

			edgeType *GetIncomingEdge(int32 index) const
			{
				return (static_cast<edgeType *>(static_cast<GraphEdge<elementType, edgeType> *>(GraphElementBase::GetIncomingEdge(index))));
			}

			edgeType *FindOutgoingEdge(const GraphElementBase *finish) const
			{
				return (static_cast<edgeType *>(static_cast<GraphEdge<elementType, edgeType> *>(GraphElementBase::FindOutgoingEdge(finish))));
			}

			static edgeType *FindNextOutgoingEdge(const GraphElementBase *finish, const edgeType *previous)
			{
				return (static_cast<edgeType *>(static_cast<GraphEdge<elementType, edgeType> *>(GraphElementBase::FindNextOutgoingEdge(finish, previous))));
			}

			edgeType *FindIncomingEdge(const GraphElementBase *start) const
			{
				return (static_cast<edgeType *>(static_cast<GraphEdge<elementType, edgeType> *>(GraphElementBase::FindIncomingEdge(start))));
			}

			static edgeType *FindNextIncomingEdge(const GraphElementBase *start, const edgeType *previous)
			{
				return (static_cast<edgeType *>(static_cast<GraphEdge<elementType, edgeType> *>(GraphElementBase::FindNextIncomingEdge(start, previous))));
			}
	};


	class GraphBase
	{
		private:

			List<GraphElementBase>		elementList;

		protected:

			GraphBase() {}
			~GraphBase() {}

		public:

			bool Empty(void) const
			{
				return (elementList.Empty());
			}

			int32 GetElementCount(void) const
			{
				return (elementList.GetElementCount());
			}

			GraphElementBase *GetFirstElement(void) const
			{
				return (elementList.First());
			}

			GraphElementBase *GetLastElement(void) const
			{
				return (elementList.Last());
			}

			void AddElement(GraphElementBase *element)
			{
				elementList.Append(element);
			}

			void RemoveElement(GraphElementBase *element)
			{
				element->PurgeIncomingEdges();
				element->PurgeOutgoingEdges();
				elementList.Remove(element);
			}

			void DetachElement(GraphElementBase *element)
			{
				elementList.Remove(element);
			}

			void Purge(void)
			{
				elementList.Purge();
			}

			C4API bool Predecessor(const GraphElementBase *first, const GraphElementBase *second);
	};


	//# \class	Graph		Encapsulates a directed graph.
	//
	//# The $Graph$ class encapsulates a directed graph.
	//
	//# \def	template <class elementType, class edgeType> class Graph : public GraphBase
	//
	//# \tparam		elementType		The type of the class that can be stored as an element in the graph.
	//# \tparam		edgeType		The type of the class that represents the edges in the graph.
	//
	//# \ctor	Graph();
	//
	//# \desc
	//# The $Graph$ class is used to organize a set of elements and edges arranged as a directed graph.
	//
	//# \privbase	GraphBase	Used internally to encapsulate common functionality that is independent
	//#							of the template parameter.
	//
	//# \also	$@GraphElement@$
	//# \also	$@GraphEdge@$


	//# \function	Graph::Empty	Returns a boolean value indicating whether any elements exist in a graph.
	//
	//# \proto		bool Empty(void) const;
	//
	//# \desc
	//# The $Empty$ function returns $true$ if no elements belong to a graph. If at least one element belongs to
	//# a graph, then the return value is $false$.
	//
	//# \also	$@Graph::GetElementCount@$
	//# \also	$@Graph::Purge@$


	//# \function	Graph::GetElementCount		Returns the number of elements in a graph.
	//
	//# \proto		int32 GetElementCount(void) const;
	//
	//# \desc
	//# The $GetElementCount$ function returns the number of elements in a graph.
	//#
	//# If the only information needed is whether a graph contains a nonzero number of elements, then it is
	//# more efficient to call the $@Graph::Empty@$ function.
	//
	//# \also	$@Graph::Empty@$
	//# \also	$@Graph::Purge@$


	//# \function	Graph::Purge	Destroys all elements belonging to a graph.
	//
	//# \proto		void Purge(void);
	//
	//# \desc
	//# The $Purge$ function destroys all of the elements belonging to a graph. This process also
	//# causes every edge belonging to the graph to be destroyed. The graph is completely
	//# empty after this function is called.
	//
	//# \also	$@Graph::Empty@$
	//# \also	$@Graph::GetElementCount@$
	//# \also	$@Graph::RemoveElement@$


	//# \function	Graph::AddElement		Adds an element to a graph.
	//
	//# \proto		void AddElement(GraphElement<elementType, edgeType> *element);
	//
	//# \param	element		The element to add to the graph.
	//
	//# \desc
	//# The $AddElement$ function adds the element specified by the $element$ parameter to a graph.
	//# Initially, the element has no incoming or outgoing edges.
	//
	//# \also	$@Graph::RemoveElement@$
	//# \also	$@Graph::Purge@$


	//# \function	Graph::RemoveElement	Removes an element from a graph.
	//
	//# \proto		void RemoveElement(GraphElement<elementType, edgeType> *element);
	//
	//# \param	element		The element to remove from the graph.
	//
	//# \desc
	//# The $RemoveElement$ function removes the element specified by the $element$ parameter from a graph.
	//# This function destroys all incoming and outgoing edges for the element.
	//
	//# \also	$@Graph::AddElement@$
	//# \also	$@Graph::Purge@$


	//# \function	Graph::Predecessor		Returns a boolean value indicating whether one element precedes another.
	//
	//# \proto		bool Predecessor(const GraphElementBase *first, const GraphElementBase *second);
	//
	//# \param	first	The first element in the graph.
	//# \param	second	The second element in the graph.
	//
	//# \desc
	//# The $Predecessor$ function returns $true$ if the element specified by the $first$ parameter precedes the
	//# element specified by the $second$ parameter in a graph, and it returns $false$ otherwise. In order for the
	//# first element to precede the second, there must be a sequence of edges starting at the first element that can
	//# be followed through a sequence of elements that ultimately finishes at the second element.
	//
	//# \also	$@GraphElement::GetFirstOutgoingEdge@$
	//# \also	$@GraphElement::GetLastOutgoingEdge@$
	//# \also	$@GraphElement::GetFirstIncomingEdge@$
	//# \also	$@GraphElement::GetLastIncomingEdge@$


	template <class elementType, class edgeType> class Graph : public GraphBase
	{
		public:

			Graph() = default;
			~Graph() = default;

			elementType *GetFirstElement(void) const
			{
				return (static_cast<elementType *>(static_cast<GraphElement<elementType, edgeType> *>(GraphBase::GetFirstElement())));
			}

			elementType *GetLastElement(void) const
			{
				return (static_cast<elementType *>(static_cast<GraphElement<elementType, edgeType> *>(GraphBase::GetLastElement())));
			}

			void AddElement(GraphElement<elementType, edgeType> *element)
			{
				GraphBase::AddElement(element);
			}

			void RemoveElement(GraphElement<elementType, edgeType> *element)
			{
				GraphBase::RemoveElement(element);
			}
	};


	inline GraphEdgeStart::GraphEdgeStart(GraphElementBase *start)
	{
		startElement = start;
		start->outgoingEdgeList.Append(this);
	}

	inline void GraphEdgeStart::SetStartElement(GraphElementBase *start)
	{
		startElement = start;
		start->outgoingEdgeList.Append(this);
	}

	inline void GraphEdgeStart::Attach(void)
	{
		startElement->outgoingEdgeList.Append(this);
	}


	inline GraphEdgeFinish::GraphEdgeFinish(GraphElementBase *start, GraphElementBase *finish) : GraphEdgeStart(start)
	{
		finishElement = finish;
		finish->incomingEdgeList.Append(this);
	}

	inline void GraphEdgeFinish::SetFinishElement(GraphElementBase *finish)
	{
		finishElement = finish;
		finish->incomingEdgeList.Append(this);
	}

	inline void GraphEdgeFinish::Attach(void)
	{
		GraphEdgeStart::Attach();
		finishElement->incomingEdgeList.Append(this);
	}
}


#endif

// ZYUQURM
