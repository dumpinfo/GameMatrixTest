 

#ifndef C4Shared_h
#define C4Shared_h


//# \component	Utility Library
//# \prefix		Utilities/


#include "C4Memory.h"


namespace C4
{
	//# \class	Shared	The base class for reference-counted objects.
	//
	//# Objects inherit from the $Shared$ class when they contain shared data and
	//# need to be reference counted.
	//
	//# \def	class Shared
	//
	//# \ctor	Shared();
	//
	//# The constructor has protected access and takes no parameters.
	//# The $Shared$ class can only exist as a base class for another class.
	//
	//# \desc
	//# The $Shared$ class encapsulates a reference count for objects that can be shared.
	//# Upon construction, the object's reference count is initialized to 1.
	//
	//# \important
	//# The destructor of the $Shared$ class does not have public access, and the destructors
	//# of any subclasses of the $Shared$ class should not have public access. Shared objects
	//# must be released by calling the $@Shared::Release@$ function.
	//
	//# \also	$@AutoRelease@$


	//# \function	Shared::GetReferenceCount		Returns an object's current reference count.
	//
	//# \proto		int32 GetReferenceCount(void) const;
	//
	//# \desc
	//# The $GetReferenceCount$ function returns the current reference count.
	//# When a shared object is constructed, its initial reference count is 1.
	//
	//# \also		$@Shared::Retain@$
	//# \also		$@Shared::Release@$


	//# \function	Shared::Retain		Increments an object's reference count.
	//
	//# \proto		int32 Retain(void);
	//
	//# \desc
	//# The $Retain$ function increments the reference count. For each call to $Retain$
	//# made for a particular shared object, a balancing call to $@Shared::Release@$
	//# decrements the reference count without destroying the object. The return value
	//# of the $Retain$ function is the new reference count.
	//
	//# \also		$@Shared::GetReferenceCount@$
	//# \also		$@Shared::Release@$


	//# \function	Shared::Release		Decrements an object's reference count.
	//
	//# \proto		virtual int32 Release(void);
	//
	//# \desc
	//# The $Release$ function decrements the reference count. If the reference count
	//# becomes zero, then the object is destroyed. The return value of the $Release$
	//# function is the new reference count.
	//
	//# \also		$@Shared::GetReferenceCount@$
	//# \also		$@Shared::Retain@$


	class Shared
	{
		private:

			int32		referenceCount;

			Shared(const Shared&) = delete;
			Shared& operator =(const Shared&) = delete;

		protected:

			Shared()
			{
				referenceCount = 1;
			}

			virtual ~Shared() {}

		public:

			int32 GetReferenceCount(void) const
			{
				return (referenceCount);
			}
 
			int32 Retain(void)
			{
				return (++referenceCount); 
			}
 
			virtual int32 Release(void)
			{
				int32 count = --referenceCount; 
				if (count == 0)
				{ 
					delete this; 
				}

				return (count);
			} 
	};


	//# \class	AutoRelease		A helper class the wraps a pointer to a shared object.
	//
	//# \def	template <class type> class AutoRelease
	//
	//# \tparam	type	The type of object to which the $AutoRelease$ object refers.
	//
	//# \ctor	explicit AutoRelease(type *ptr);
	//
	//# \param	ptr		A pointer to the shared object that is wrapped by the $AutoRelease$ object.
	//
	//# \desc
	//# The $AutoRelease$ class template is a helper class that wraps a pointer to a shared
	//# object. When an $AutoRelease$ object is destroyed (usually by going out of scope),
	//# the $Release$ function is automatically called for the object that was passed into
	//# the $ptr$ parameter when the $AutoRelease$ object was constructed.
	//#
	//# An $AutoRelease$ object behaves like a pointer to an object of the type given by the
	//# $type$ template parameter. An $AutoRelease$ object can be passed as a function parameter
	//# wherever a pointer to $type$ is expected, and the $->$ operator can be used to access
	//# members of the object that the $AutoRelease$ object wraps.
	//
	//# \also	$@Shared@$


	template <class type> class AutoRelease
	{
		private:

			type	*reference;

		public:

			explicit AutoRelease(type *ptr)
			{
				reference = ptr;
			}

			AutoRelease(const AutoRelease& ar)
			{
				reference = ar.reference;
				reference->Retain();
			}

			~AutoRelease()
			{
				reference->Release();
			}

			operator type *(void) const
			{
				return (reference);
			}

			type *const *operator &(void) const
			{
				return (&reference);
			}

			type *operator ->(void) const
			{
				return (reference);
			}
	};
}


#endif

// ZYUQURM
