 

#ifndef C4Creation_h
#define C4Creation_h


//# \component	System Utilities
//# \prefix		System/


#include "C4Packing.h"


namespace C4
{
	//# \class	 Creator		Encapsulates a creator function for application-defined subclasses.
	//
	//# The $Creator$ class template encapsulates a creator function for application-defined subclasses.
	//
	//# \def	template <class type, typename param = Unpacker&> class Creator : public ListElement<Creator<type, param>>
	//
	//# \tparam		type	The type of the base class for the application-defined class.
	//# \tparam		param	The type of the data passed to the creator function.
	//
	//# \ctor	Creator(CreateProc *proc);
	//
	//# \param		proc	A pointer to the creator function.
	//
	//# \desc
	//# The $Creator$ class template encapsulates a creator function that is called when the engine
	//# encounters an object that is a subclass of the template parameter, but is of an application-defined type.
	//# The $CreateProc$ type is defined as follows.
	//
	//# \code	typedef type *CreateProc(param data, unsigned_int32 flags);
	//
	//# An application should define a function having this signature, create a $Creator$ instance to
	//# encapsulate it, and then install the creator object by calling the $@Creatable::InstallCreator@$ function.
	//#
	//# When the engine calls the creator function specified by the $proc$ parameter, it passes in the custom
	//# data type specified by the $param$ template parameter, which is normally a reference to an $@ResourceMgr/Unpacker@$
	//# object. The application should examine this data (normally by calling the $@ResourceMgr/Unpacker::GetType@$
	//# function) and return a newly created class instance corresponding to that type if it is recognized. If the type is
	//# not recognized, then the function should return $nullptr$ to indicate that the remaining installed creator
	//# objects are to be given the opportunity to create the subclass.
	//
	//# \base	Utilities/ListElement<Creator<type, param>>		The $Creatable$ class holds a list of $Creator$ instances.
	//
	//# \also	$@Creatable@$


	template <class type, typename param = Unpacker&> class Creator : public ListElement<Creator<type, param>>
	{
		public:

			typedef type *CreateProc(param, unsigned_int32);

		private:

			CreateProc	*createProc;

		public:

			Creator(CreateProc *proc)
			{
				createProc = proc;
			}

			type *Create(param data, unsigned_int32 flags = 0)
			{
				return ((*createProc)(data, flags));
			}
	};


	//# \class	 Creatable		The base class for class types that can be extended by application-defined subclasses.
	//
	//# The $Creatable$ class template is the base class for class types that can be extended by
	//# application-defined subclasses.
	//
	//# \def	template <class type, typename param = Unpacker&> class Creatable
	//
	//# \tparam		type	The type of the base class of a hierarchy of extensible classes.
	//# \tparam		param	The type of the data passed to the creator functions.
	//
	//# \ctor	Creatable()
	//
	//# \desc
	//# The $Creatable$ class template appears as a base class for types of objects that can have custom subclasses
	//# defined by an application. So that the engine can create these custom subclasses when necessary
	//# (e.g., when loading a world), an application installs a special creator object by calling the
	//# $@Creatable::InstallCreator@$ function. This object has a member function that is responsible
	//# for returning a pointer to a newly created instance of the subclass of a given type.
	//
	//# \also	$@Creator@$
	//# \also	$@Registrable@$


	//# \function	Creatable::InstallCreator		Installs a creator object for application-defined subclasses of the template parameter.
	//
	//# \proto	static void InstallCreator(Creator<type, param> *creator);
	//
	//# \param	creator		A pointer to the $@Creator@$ object corresponding to the template parameter.
	// 
	//# \desc
	//# The $InstallCreator$ function installs a creator object whose responsibility it is to create
	//# new instances of application-defined subclasses of the template parameter. The creator object 
	//# encapsulates a function that is called by the engine when it encounters a custom type.
	//# 
	//# Once the creator object is installed, it must continue to exist in order to function. A creator
	//# object is uninstalled by simply deleting it or by calling the $@Creatable::RemoveCreator@$ function.
	// 
	//# \also	$@Creatable::RemoveCreator@$
	//# \also	$@Creator@$ 
 

	//# \function	Creatable::RemoveCreator		Removes a creator object for application-defined subclasses of the template parameter.
	//
	//# \proto	static void RemoveCreator(Creator<type, param> *creator); 
	//
	//# \param	creator		A pointer to the $@Creator@$ object corresponding to the template parameter.
	//
	//# \desc
	//# The $RemoveCreator$ function removes a creator object. The creator object specified by the $creator$
	//# parameter must have previously been installed by calling the $@Creatable::InstallCreator@$ function.
	//#
	//# A creator object can also be removed by simply deleting it.
	//
	//# \also	$@Creatable::InstallCreator@$
	//# \also	$@Creator@$


	template <class type, typename param = Unpacker&> class Creatable
	{
		private:

			static C4API List<Creator<type, param>>		creatorList;
			static type									*(*unknownCreator)(param, unsigned_int32);

		public:

			static void InstallCreator(Creator<type, param> *creator)
			{
				creatorList.Append(creator);
			}

			static void RemoveCreator(Creator<type, param> *creator)
			{
				creatorList.Remove(creator);
			}

			static void SetUnknownCreator(type *(*creator)(param, unsigned_int32))
			{
				unknownCreator = creator;
			}

			C4API static type *Create(param data, unsigned_int32 flags = 0);
	};


	#ifdef C4ENGINEMODULE

		template <class type, typename param> List<Creator<type, param>> Creatable<type, param>::creatorList;
		template <class type, typename param> type *(*Creatable<type, param>::unknownCreator)(param, unsigned_int32) = nullptr;


		template <class type, typename param> type *Creatable<type, param>::Create(param data, unsigned_int32 flags)
		{
			Creator<type, param> *creator = creatorList.First();
			while (creator)
			{
				type *creatable = creator->Create(data, flags);
				if (creatable)
				{
					return (creatable);
				}

				creator = creator->Next();
			}

			if (unknownCreator)
			{
				return ((*unknownCreator)(data, flags));
			}

			return (nullptr);
		}

	#endif


	//# \class	 Registration		The base class for registration objects.
	//
	//# The $Registration$ class template is the base class for registration objects
	//
	//# \def	template <class classType, class regType> class Registration : public MapElement<Registration<classType, regType>>
	//
	//# \tparam		classType	The type of the base class of a hierarchy of extensible classes.
	//# \tparam		regType		The type of the $Registration$ subclass used to register classes derived from $classType$.
	//
	//# \ctor	Registration(Type type);
	//
	//# \param	type	The type of the subclass being registered.
	//
	//# \desc
	//# The $Registration$ class is the base class used by more specific types of registration objects. Registration objects
	//# are used to identify custom types in the engine.
	//
	//# \base	Utilities/MapElement<Registration<classType, regType>>	Registration objects are stored in a map container by the engine.
	//
	//# \also	$@Registration@$


	//# \function	Registration::GetRegistrableType		Returns the type of the registered class.
	//
	//# \proto	Type GetRegistrableType(void) const;
	//
	//# \desc
	//# The $GetRegistrableType$ function returns the unique type identifier for the custom class type that
	//# the registration object represents.
	//
	//# \also	$@Registrable::FindRegistration@$


	template <class classType, class regType> class Registration : public MapElement<Registration<classType, regType>>
	{
		private:

			Type	registrableType;

		protected:

			C4API Registration(Type type);

		public:

			typedef Type	KeyType;

			C4API virtual ~Registration();

			KeyType GetKey(void) const
			{
				return (registrableType);
			}

			Type GetRegistrableType(void) const
			{
				return (registrableType);
			}

			regType *Previous(void) const
			{
				return (static_cast<regType *>(MapElement<Registration<classType, regType>>::Previous()));
			}

			regType *Next(void) const
			{
				return (static_cast<regType *>(MapElement<Registration<classType, regType>>::Next()));
			}

			virtual classType *Create(void) const = 0;
	};


	//# \class	 Registrable		The base class for class types that can have custom subclasses that are registered with the engine.
	//
	//# The $Registrable$ class template is the base class for class types that can have custom subclasses
	//# that are registered with the engine.
	//
	//# \def	template <class classType, class regType> class Registrable : public Creatable<classType>
	//
	//# \tparam		classType	The type of the base class of a hierarchy of extensible classes.
	//# \tparam		regType		The type of the $@Registration@$ subclass used to register classes derived from $classType$.
	//
	//# \ctor	Registrable()
	//
	//# \desc
	//# The $Registrable$ class is the base class for class types that can have custom subclasses that
	//# are registered with the engine.
	//
	//# \base	Creatable<classType>	Registrable objects are a special type of creatable object.
	//
	//# \also	$@Registration@$


	//# \function	Registrable::GetFirstRegistration		Returns the first registration.
	//
	//# \proto	static regType *GetFirstRegistration(void);
	//
	//# \desc
	//# The $GetFirstRegistration$ function returns a pointer to the registration object corresponding
	//# to the first registration for subclasses of the $classType$ template parameter. The entire list of
	//# registrations can be iterated by calling the $@Utilities/MapElement::Next@$ function on the returned
	//# object and continuing until $nullptr$ is returned.
	//
	//# \also	$@Registrable::GetRegistration@$
	//# \also	$@Registrable::FindRegistration@$
	//# \also	$@Registration@$


	//# \function	Registrable::FindRegistration		Returns a specific registration by its type.
	//
	//# \proto	static regType *FindRegistration(Type type);
	//
	//# \param	type	The type of the subclass.
	//
	//# \desc
	//# The $FindRegistration$ function returns a pointer to the registration object corresponding
	//# to the subclass type specified by the $type$ parameter. If no such registration exists,
	//# then the return value is $nullptr$.
	//
	//# \also	$@Registrable::GetFirstRegistration@$
	//# \also	$@Registrable::GetRegistration@$
	//# \also	$@Registration@$


	//# \function	Registrable::GetRegistrationCount		Returns the number of registration objects.
	//
	//# \proto	static int32 GetRegistrationCount(void);
	//
	//# \desc
	//# The $GetRegistrationCount$ function returns the total number of registration objects for subclasses
	//# of the $classType$ template parameter.
	//
	//# \also	$@Registrable::GetRegistration@$
	//# \also	$@Registrable::GetFirstRegistration@$
	//# \also	$@Registrable::FindRegistration@$
	//# \also	$@Registration@$


	//# \function	Registrable::GetRegistration		Returns a specific registration by its index.
	//
	//# \proto	static regType *GetRegistration(int32 index);
	//
	//# \param	index	The index of the regsitration object in the range [0,&nbsp;<i>n</i>&nbsp;&minus;&nbsp;1], where <i>n</i> is the total number of registration objects.
	//
	//# \desc
	//# The $GetRegistration$ function returns a pointer to the registration object having the index specified
	//# by the $index$ parameter. The total number of registration objects can be determined by calling the
	//# $@Registrable::GetRegistrationCount@$ function. If the $index$ parameter is outside the range
	//# [0,&nbsp;<i>n</i>&nbsp;&minus;&nbsp;1], where <i>n</i> is the value returned by the
	//# $@Registrable::GetRegistrationCount@$ function, then the return value is $nullptr$.
	//
	//# \also	$@Registrable::GetRegistrationCount@$
	//# \also	$@Registrable::GetFirstRegistration@$
	//# \also	$@Registrable::FindRegistration@$
	//# \also	$@Registration@$


	template <class classType, class regType> class Registrable : public Creatable<classType>
	{
		friend class Registration<classType, regType>;

		private:

			static C4API Map<Registration<classType, regType>>		registrationMap;

		public:

			static regType *GetFirstRegistration(void)
			{
				return (static_cast<regType *>(registrationMap.First()));
			}

			static regType *FindRegistration(Type type)
			{
				return (static_cast<regType *>(registrationMap.Find(type)));
			}

			static int32 GetRegistrationCount(void)
			{
				return (registrationMap.GetElementCount());
			}

			static regType *GetRegistration(int32 index)
			{
				return (static_cast<regType *>(registrationMap[index]));
			}

			C4API static classType *Create(Unpacker& data, unsigned_int32 unpackFlags = 0);
	};


	#ifdef C4ENGINEMODULE

		template <class classType, class regType> Map<Registration<classType, regType>> Registrable<classType, regType>::registrationMap;


		template <class classType, class regType> Registration<classType, regType>::Registration(Type type)
		{
			registrableType = type;
			Registrable<classType, regType>::registrationMap.Insert(this);
		}

		template <class classType, class regType> Registration<classType, regType>::~Registration()
		{
		}

		template <class classType, class regType> classType *Registrable<classType, regType>::Create(Unpacker& data, unsigned_int32 unpackFlags)
		{
			Type type = data.GetType();
			regType *registration = FindRegistration(type);
			if (registration)
			{
				return (registration->Create());
			}

			return (Creatable<classType>::Create(data, unpackFlags));
		}

	#endif
}


#endif

// ZYUQURM
