 

#ifndef C4List_h
#define C4List_h


//# \component	Utility Library
//# \prefix		Utilities/


#include "C4Memory.h"


namespace C4
{
	class ListBase;
	template <class> class List;


	class ListElementBase
	{
		friend class ListBase;

		private:

			ListElementBase		*prevListElement;
			ListElementBase		*nextListElement;
			ListBase			*owningList;

			ListElementBase(const ListElementBase&) = delete;
			ListElementBase& operator =(const ListElementBase&) = delete;

		protected:

			ListElementBase()
			{
				prevListElement = nullptr;
				nextListElement = nullptr;
				owningList = nullptr;
			}

			C4API virtual ~ListElementBase();

			ListElementBase *Previous(void) const
			{
				return (prevListElement);
			}

			ListElementBase *Next(void) const
			{
				return (nextListElement);
			}

			ListBase *GetOwningList(void) const
			{
				return (owningList);
			}

		public:

			C4API int32 GetListIndex(void) const;

			C4API virtual void Detach(void);
	};


	class ListBase
	{
		friend class ListElementBase;

		private:

			ListElementBase		*firstListElement;
			ListElementBase		*lastListElement;

			ListBase(const ListBase&) = delete;
			ListBase& operator =(const ListBase&) = delete;

		protected:

			ListBase()
			{
				firstListElement = nullptr;
				lastListElement = nullptr;
			}

			C4API ~ListBase();

			C4API ListElementBase *operator [](machine index) const;

			ListElementBase *First(void) const
			{
				return (firstListElement);
			}

			ListElementBase *Last(void) const
			{
				return (lastListElement);
			}

			bool Member(const ListElementBase *element) const
			{
				return (element->owningList == this); 
			}

			C4API void Prepend(ListElementBase *element); 
			C4API void Append(ListElementBase *element);
 
			C4API void InsertBefore(ListElementBase *element, ListElementBase *before);
			C4API void InsertAfter(ListElementBase *element, ListElementBase *after);
 
			C4API void Remove(ListElementBase *element);
 
		public: 

			bool Empty(void) const
			{
				return (!firstListElement); 
			}

			C4API int32 GetElementCount(void) const;

			C4API void RemoveAll(void);
			C4API void Purge(void);
	};


	//# \class	ListElement		The base class for objects that can be stored in a list.
	//
	//# Objects inherit from the $ListElement$ class so that they can be stored in a list.
	//
	//# \def	template <class type> class ListElement : public ListElementBase
	//
	//# \tparam		type	The type of the class that can be stored in a list. This parameter should be the
	//#						type of the class that inherits directly from the $ListElement$ class.
	//
	//# \ctor	ListElement();
	//
	//# The constructor has protected access takes no parameters. The $ListElement$ class can only exist
	//# as a base class for another class.
	//
	//# \desc
	//# The $ListElement$ class should be declared as a base class for objects that need to be stored in a list.
	//# The $type$ template parameter should match the class type of such objects, and these objects can be
	//# stored in a $@List@$ container declared with the same $type$ template parameter.
	//
	//# \privbase	ListElementBase		Used internally to encapsulate common functionality that is independent
	//#									of the template parameter.
	//
	//# \also	$@List@$


	//# \function	ListElement::Previous		Returns the previous element in a list.
	//
	//# \proto	type *Previous(void) const;
	//
	//# \desc
	//# The $Previous$ function returns a pointer to the element immediately preceding an object in its
	//# owning list. If the object is the first element in a list, or the object does not belong to a
	//# list, then the return value is $nullptr$.
	//
	//# \also	$@ListElement::Next@$


	//# \function	ListElement::Next		Returns the next element in a list.
	//
	//# \proto	type *Next(void) const;
	//
	//# \desc
	//# The $Next$ function returns a pointer to the element immediately succeeding an object in its
	//# owning list. If the object is the last element in a list, or the object does not belong to a
	//# list, then the return value is $nullptr$.
	//
	//# \also	$@ListElement::Previous@$


	//# \function	ListElement::GetOwningList		Returns the list to which an object belongs.
	//
	//# \proto	List<type> *GetOwningList(void) const;
	//
	//# \desc
	//# The $GetOwningList$ function returns a pointer to the $@List@$ container to which an object belongs.
	//# If the object is not a member of a list, then the return value is $nullptr$.
	//
	//# \also	$@List::Member@$


	//# \function	ListElement::Detach		Removes an object from any list to which it belongs.
	//
	//# \proto	virtual void Detach(void);
	//
	//# \desc
	//# The $Detach$ function removes an object from its owning list. If the object is not a member of
	//# a list, then the $Detach$ function has no effect.
	//
	//# \also	$@List::Remove@$


	template <class type> class ListElement : public ListElementBase
	{
		protected:

			ListElement() {}

		public:

			type *Previous(void) const
			{
				return (static_cast<type *>(static_cast<ListElement<type> *>(ListElementBase::Previous())));
			}

			type *Next(void) const
			{
				return (static_cast<type *>(static_cast<ListElement<type> *>(ListElementBase::Next())));
			}

			List<type> *GetOwningList(void) const
			{
				return (static_cast<List<type> *>(ListElementBase::GetOwningList()));
			}
	};


	template <class type> class ListIterator
	{
		private:

			type		*iteratorElement;

		public:

			ListIterator(type *element) : iteratorElement(element) {}

			type *operator *(void) const
			{
				return (iteratorElement);
			}

			ListIterator& operator ++(void)
			{
				iteratorElement = iteratorElement->ListElement<type>::Next();
				return (*this);
			}

			bool operator ==(const ListIterator& iterator) const
			{
				return (iteratorElement == iterator.iteratorElement);
			}

			bool operator !=(const ListIterator& iterator) const
			{
				return (iteratorElement != iterator.iteratorElement);
			}
	};


	//# \class	List	A container class that holds a list of objects.
	//
	//# The $List$ class encapsulates a doubly-linked list.
	//
	//# \def	template <class type> class List : public ListBase
	//
	//# \tparam		type	The type of the class that can be stored in the list. The class specified
	//#						by this parameter should inherit directly from the $@ListElement@$ class
	//#						using the same template parameter.
	//
	//# \ctor	List();
	//
	//# \desc
	//# The $List$ class template is a container used to store a homogeneous doubly-linked list of objects.
	//# The class type of objects that are to be stored in the list must be a subclass of the $@ListElement@$
	//# class template using the same template parameter as the $List$ container. A particular object can be a
	//# member of only one list at a time.
	//#
	//# Upon construction, a $List$ object is empty. When a $List$ object is destroyed, all of the members
	//# of the list are also destroyed. To avoid deleting the members of a list when a $List$ object is
	//# destroyed, first call the $@List::RemoveAll@$ function to remove all of the list's members.
	//#
	//# It is possible to iterate over the elements of a list using a range-based for loop.
	//# This is illustrated by the following code, where $list$ is a variable of type $List<type>$.
	//
	//# \source
	//# for (type *element : list)\n
	//# {\n
	//# \t...\n
	//# }
	//
	//# \operator	type *operator [](machine index) const;
	//#				Returns the element of a list whose zero-based index is $index$. If $index$ is
	//#				greater than or equal to the number of elements in the list, then the return
	//#				value is $nullptr$.
	//
	//# \privbase	ListBase	Used internally to encapsulate common functionality that is independent
	//#							of the template parameter.
	//
	//# \also	$@ListElement@$
	//# \also	$@Array@$


	//# \function	List::First		Returns the first element in a list.
	//
	//# \proto	type *First(void) const;
	//
	//# \desc
	//# The $First$ function returns a pointer to the first element in a list. If the list is empty,
	//# then this function returns $nullptr$.
	//
	//# \also	$@List::Last@$


	//# \function	List::Last		Returns the last element in a list.
	//
	//# \proto	type *Last(void) const;
	//
	//# \desc
	//# The $Last$ function returns a pointer to the last element in a list. If the list is empty,
	//# then this function returns $nullptr$.
	//
	//# \also	$@List::First@$


	//# \function	List::Member		Returns a boolean value indicating whether a particular object is
	//#									a member of a list.
	//
	//# \proto	bool Member(const ListElement<type> *element) const;
	//
	//# \param	element		A pointer to the object to test for membership.
	//
	//# \desc
	//# The $Member$ function returns $true$ if the object specified by the $element$ parameter is
	//# a member of the list, and $false$ otherwise.
	//
	//# \also	$@ListElement::GetOwningList@$


	//# \function	List::Empty		Returns a boolean value indicating whether a list is empty.
	//
	//# \proto	bool Empty(void) const;
	//
	//# \desc
	//# The $Empty$ function returns $true$ if the list contains no elements, and $false$ otherwise.
	//
	//# \also	$@List::First@$
	//# \also	$@List::Last@$


	//# \function	List::GetElementCount		Returns the number of elements in a list.
	//
	//# \proto	int32 GetElementCount(void) const;
	//
	//# \desc
	//# The $GetElementCount$ function iterates through the members of a list and returns the count.
	//
	//# \also	$@List::Empty@$


	//# \function	List::Prepend		Adds an object to the beginning of a list.
	//
	//# \proto	void Prepend(ListElement<type> *element);
	//
	//# \param	element		A pointer to the object to add to the list.
	//
	//# \desc
	//# The $Prepend$ function adds the object specified by the $element$ parameter to the beginning of
	//# a list. If the object is already a member of the list, then it is moved to the beginning.
	//#
	//# If the object being added is already a member of a different list of the same type, then it is first
	//# removed from that list before being added to the new list.
	//
	//# \also	$@List::Append@$
	//# \also	$@List::InsertBefore@$
	//# \also	$@List::InsertAfter@$


	//# \function	List::Append		Adds an object to the end of a list.
	//
	//# \proto	void Append(ListElement<type> *element);
	//
	//# \param	element		A pointer to the object to add to the list.
	//
	//# \desc
	//# The $Append$ function adds the object specified by the $element$ parameter to the end of
	//# a list. If the object is already a member of the list, then it is moved to the end.
	//#
	//# If the object being added is already a member of a different list of the same type, then it is first
	//# removed from that list before being added to the new list.
	//
	//# \also	$@List::Prepend@$
	//# \also	$@List::InsertBefore@$
	//# \also	$@List::InsertAfter@$


	//# \function	List::InsertBefore		Inserts an object before an existing element of a list.
	//
	//# \proto	void InsertBefore(ListElement<type> *element, ListElement<type> *before);
	//
	//# \param	element		A pointer to the object to add to the list.
	//# \param	before		A pointer to the object before which the new object is inserted.
	//#						This parameter must point to an object that is already a member of the list.
	//
	//# \desc
	//# The $InsertBefore$ function adds the object specified by the $element$ parameter to a list at the
	//# position before the object specified by the $before$ parameter. If the object is already a member
	//# of the list, then it is moved to the new position. If the $before$ parameter is $nullptr$, then the
	//# node is added to the end of the list. Otherwise, the $before$ parameter must specify an object that
	//# is already a member of the list for which this function is called.
	//#
	//# If the object being added is already a member of a different list of the same type, then it is first
	//# removed from that list before being added to the new list.
	//
	//# \also	$@List::InsertAfter@$
	//# \also	$@List::Prepend@$
	//# \also	$@List::Append@$


	//# \function	List::InsertAfter		Inserts an object after an existing element of a list.
	//
	//# \proto	void InsertAfter(ListElement<type> *element, ListElement<type> *after);
	//
	//# \param	element		A pointer to the object to add to the list.
	//# \param	after		A pointer to the object after which the new object is inserted.
	//#						This parameter must point to an object that is already a member of the list.
	//
	//# \desc
	//# The $InsertAfter$ function adds the object specified by the $element$ parameter to a list at the
	//# position after the object specified by the $after$ parameter. If the object is already a member
	//# of the list, then it is moved to the new position. If the $after$ parameter is $nullptr$, then the
	//# node is added to the beginning of the list. Otherwise, the $after$ parameter must specify an object that
	//# is already a member of the list for which this function is called.
	//#
	//# If the object being added is already a member of a different list of the same type, then it is first
	//# removed from that list before being added to the new list.
	//
	//# \also	$@List::InsertBefore@$
	//# \also	$@List::Prepend@$
	//# \also	$@List::Append@$


	//# \function	List::Remove		Removes a particular element from a list.
	//
	//# \proto	void Remove(ListElement<type> *element);
	//
	//# \param	element		A pointer to the object to remove from the list.
	//
	//# \desc
	//# The $Remove$ function removes the object specified by the $element$ parameter from a list.
	//# The object must belong to the list for which the $Remove$ function is called, or the internal
	//# links will become corrupted.
	//
	//# \also	$@List::RemoveAll@$
	//# \also	$@List::Purge@$
	//# \also	$@ListElement::Detach@$


	//# \function	List::RemoveAll		Removes all elements from a list.
	//
	//# \proto	void RemoveAll(void);
	//
	//# \desc
	//# The $RemoveAll$ function removes all objects contained in a list, but does not delete them.
	//# The list is subsequently empty.
	//
	//# \also	$@List::Remove@$
	//# \also	$@List::Purge@$
	//# \also	$@ListElement::Detach@$


	//# \function	List::Purge		Deletes all elements in a list.
	//
	//# \proto	void Purge(void);
	//
	//# \desc
	//# The $Purge$ function deletes all objects contained in a list. The list is subsequently empty.
	//# To remove all elements of a list without destroying them, use the $@List::RemoveAll@$ function.
	//
	//# \also	$@List::Remove@$
	//# \also	$@List::RemoveAll@$
	//# \also	$@ListElement::Detach@$


	template <class type> class List : public ListBase
	{
		public:

			List() = default;

			type *operator [](machine index) const
			{
				return (static_cast<type *>(static_cast<ListElement<type> *>(ListBase::operator [](index))));
			}

			type *First(void) const
			{
				return (static_cast<type *>(static_cast<ListElement<type> *>(ListBase::First())));
			}

			type *Last(void) const
			{
				return (static_cast<type *>(static_cast<ListElement<type> *>(ListBase::Last())));
			}

			ListIterator<type> begin(void) const
			{
				return (ListIterator<type>(static_cast<type *>(static_cast<ListElement<type> *>(ListBase::First()))));
			}

			ListIterator<type> end(void) const
			{
				return (ListIterator<type>(nullptr));
			}

			bool Member(const ListElement<type> *element) const
			{
				return (ListBase::Member(element));
			}

			void Prepend(ListElement<type> *element)
			{
				ListBase::Prepend(element);
			}

			void Append(ListElement<type> *element)
			{
				ListBase::Append(element);
			}

			void InsertBefore(ListElement<type> *element, ListElement<type> *before)
			{
				ListBase::InsertBefore(element, before);
			}

			void InsertAfter(ListElement<type> *element, ListElement<type> *after)
			{
				ListBase::InsertAfter(element, after);
			}

			void Remove(ListElement<type> *element)
			{
				ListBase::Remove(element);
			}
	};
}


#endif

// ZYUQURM
