 

#ifndef C4Link_h
#define C4Link_h


//# \component	Utility Library
//# \prefix		Utilities/


#include "C4List.h"


namespace C4
{
	template <class type> class LinkTarget;


	//# \class	Link	Encapsulates a smart pointer to an object.
	//
	//# The $Link$ class template encapsulates a smart pointer to an object.
	//
	//# \def	template <class type> class Link : public ListElement<Link<type>>
	//
	//# \tparam		type	The type of object to which the link may point.
	//
	//# \ctor	Link();
	//# \ctor	explicit Link(type *target);
	//
	//# \param	target	The object to which the link initially points.
	//
	//# \desc
	//# The $Link$ class template provides a mechanism through which a smart pointer to an
	//# object of the type given by the $type$ template parameter can be stored. The link
	//# to an object is automatically reset to $nullptr$ if the target object is destroyed.
	//
	//# A class of type $T$ to which a link can be established must inherit from the class
	//# $LinkTarget<T>$. The $@LinkTarget@$ class template stores a list of all links
	//# that currently point to it. When the link target is destroyed, all of the links
	//# in the list are cleared.
	//
	//# \operator	operator type *(void) const;
	//#				A $Link$ object can be implicitly converted to a pointer to the object
	//#				to which it points.
	//
	//# \operator	type *operator ->(void) const;
	//#				Dereferencing a $Link$ object is equivalent to dereferencing the pointer
	//#				that it holds.
	//
	//# \operator	void operator =(type *target);
	//#				Sets the pointer that the $Link$ object stores.
	//
	//# \base		ListElement<Link<type>>		The set of links to an object are stored in
	//#											a list by the $@LinkTarget@$ class.
	//
	//# \also	$@LinkTarget@$


	//# \function	Link::GetTarget		Returns the target to which a link points.
	//
	//# \proto	type *GetTarget(void) const;
	//
	//# \desc
	//# The $GetTarget$ function returns a pointer to the target to which a link points.
	//# If the $Link$ object does not point to a target, then $GetTarget$ returns $nullptr$.


	template <class type> class Link : public ListElement<Link<type>>
	{
		private:

			type	*linkTarget;

		public:

			Link()
			{
				linkTarget = nullptr;
			}

			Link(const Link& link)
			{
				type *target = link.linkTarget;
				linkTarget = target;
				if (target)
				{
					target->LinkTarget<type>::AddLink(this);
				}
			}

			explicit Link(type *target)
			{
				linkTarget = target;
				if (target)
				{
					target->LinkTarget<type>::AddLink(this);
				}
			}

			operator type *(void) const
			{
				return (linkTarget);
			} 

			type *operator ->(void) const
			{ 
				return (linkTarget);
			} 

			type *GetTarget(void) const
			{ 
				return (linkTarget);
			} 
 
			void operator =(type *target);
	};

 
	//# \class	LinkTarget		Base class for objects that can be the target of a smart pointer.
	//
	//# The $LinkTarget$ class template is the base class for objects that can be the target of a smart pointer.
	//
	//# \def	template <class type> class LinkTarget
	//
	//# \tparam		type	The type of the subclass to which a link may point.
	//
	//# \ctor	LinkTarget();
	//
	//# The constructor has protected access and takes no parameters.
	//# The $LinkTarget$ class may exist only as a base class for another class.
	//
	//# \desc
	//# The $LinkTarget$ class template stores a list of $@Link@$ objects that each currently
	//# point to the object of which the $LinkTarget$ class is a base class. A class of type $T$
	//# inherits from $LinkTarget<T>$ to allow it to be the target of an object of type $Link<T>$.
	//# When the $LinkTarget$ object is destroyed, all of the links currently pointing to it
	//# are automatically cleared to $nullptr$.
	//
	//# \also	$@Link@$


	//# \function	LinkTarget::GetFirstLink		Returns the first link that points to an object.
	//
	//# \proto	Link<type> *GetFirstLink(void) const;
	//
	//# \desc
	//# The $GetFirstLink$ function returns a pointer to the first $@Link@$ object
	//# that is currently pointing to a link target. The $@ListElement::Next@$ function can then
	//# be used to iterate through all links currently pointing to the same link target. If there
	//# are no links currently pointing to a link target, then $GetFirstLink$ returns $nullptr$.


	//# \function	LinkTarget::BreakAllLinks		Breaks all links that point to an object.
	//
	//# \proto	void BreakAllLinks(void);
	//
	//# \desc
	//# The $BreakAllLinks$ function breaks all links that any objects have to a link target.
	//# After calling $BreakAllLinks$ for a particular link target, the $@LinkTarget::GetFirstLink@$
	//# function returns $nullptr$.
	//
	//# \also	$@LinkTarget::GetFirstLink@$


	template <class type> class LinkTarget
	{
		friend class Link<type>;

		protected:

			List<Link<type>>	linkList;

			void AddLink(Link<type> *link)
			{
				linkList.Append(link);
			}

			void RemoveLink(Link<type> *link)
			{
				linkList.Remove(link);
			}

			LinkTarget() {}
			virtual ~LinkTarget();

		public:

			Link<type> *GetFirstLink(void) const
			{
				return (linkList.First());
			}

			void BreakAllLinks(void);
	};


	template <class type> void Link<type>::operator =(type *target)
	{
		if (linkTarget != target)
		{
			if (linkTarget)
			{
				linkTarget->LinkTarget<type>::RemoveLink(this);
			}

			if (target)
			{
				target->LinkTarget<type>::AddLink(this);
			}

			linkTarget = target;
		}
	}


	template <class type> LinkTarget<type>::~LinkTarget()
	{
		for (;;)
		{
			Link<type> *link = linkList.First();
			if (!link)
			{
				break;
			}

			*link = nullptr;
		}
	}

	template <class type> void LinkTarget<type>::BreakAllLinks(void)
	{
		for (;;)
		{
			Link<type> *link = linkList.First();
			if (!link)
			{
				break;
			}

			*link = nullptr;
		}
	}
}


#endif

// ZYUQURM
