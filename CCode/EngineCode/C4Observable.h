 

#ifndef C4Observable_h
#define C4Observable_h


//# \component	Utility Library
//# \prefix		Utilities/


#include "C4List.h"


namespace C4
{
	struct NullClass
	{
	};


	template <class observableType, class eventType = typename observableType::ObservableEventType> class ObserverBase : public ListElement<ObserverBase<observableType, eventType>>
	{
		private:

			NullClass		*observerObject;
			void			(NullClass::*callbackFunction)(observableType *, eventType);

		protected:

			ObserverBase(NullClass *observer, void (NullClass::*callback)(observableType *, eventType))
			{
				observerObject = observer;
				callbackFunction = callback;
			}

			~ObserverBase()
			{
			}

		public:

			void InvokeCallback(observableType *observable, eventType event)
			{
				(observerObject->*callbackFunction)(observable, event);
			}
	};


	template <class observableType> class ObserverBase<observableType, void> : public ListElement<ObserverBase<observableType, void>>
	{
		private:

			NullClass		*observerObject;
			void			(NullClass::*callbackFunction)(observableType *);

		protected:

			ObserverBase(NullClass *observer, void (NullClass::*callback)(observableType *))
			{
				observerObject = observer;
				callbackFunction = callback;
			}

			~ObserverBase()
			{
			}

		public:

			void InvokeCallback(observableType *observable)
			{
				(observerObject->*callbackFunction)(observable);
			}
	};


	//# \class	Observable		The base class for objects that can be observed for events.
	//
	//# An object inherits from the $Observable$ class when it can be observed, and the $Observable$ class
	//# provides a mechanism for notifying the observers of the object when events occur.
	//
	//# \def	template <class observableType, class eventType = void> class Observable
	//
	//# \tparam		observableType		The type of the class that can be observed. This parameter should
	//#									be the type of the class that inherits directly from the $Observable$ class.
	//# \tparam		eventType			The type of an event object that is sent to observers when an event occurs.
	//#									If there is no event object, then this parameter should be omitted.
	//
	//# \ctor	Observable();
	//
	//# The constructor has protected access takes no parameters.
	//# The $Observable$ class can only exist as a base class for another class.
	//
	//# \desc
	//# The $Observable$ class maintains a list of observers that are listening to an observable object
	//# for event notifications. An observable object inherits from the $Observable$ class.
	//#
	//# Observers are installed by calling the $@Observable::AddObserver@$ function for the observable object.
	//# When an event occurs, the observable object calls the $@Observable::PostEvent@$ function to sent an
	//# event to its current observers.
	//
	//# \also	$@Observer@$
	//# \also	$@ExclusiveObservable@$ 


	//# \function	Observable::HasObservers	Returns a boolean value indicating whether an observable object has any observers. 
	//
	//# \proto	bool HasObservers(void) const; 
	//
	//# \desc
	//# The $HasObservers$ function returns $true$ if the observable object for which it is called has 
	//# at least one observer installed. Otherwise, the return value is $false$.
	// 
	//# \also	$@Observable::AddObserver@$ 
	//# \also	$@Observable::RemoveObserver@$


	//# \function	Observable::AddObserver		Installs an observer on an observable object. 
	//
	//# \proto	void AddObserver(ObserverType *observer);
	//
	//# \param	observer	The observer to install.
	//
	//# \desc
	//# The $AddObserver$ function installs the observer specified by the $observer$ parameter on an
	//# observable object. The observer should have the type $Observer<observerType, observableType>$,
	//# where $observerType$ is the type of the object that is notified when an event occurs, and
	//# $observableType$ is the type of the object being observed.
	//#
	//# An observer can only observe one object at a time, so if the observer specified by the
	//# $observer$ parameter is already watching a different object at the time the $AddObserver$ function
	//# is called, then it is no longer watching that object after the $AddObserver$ function returns.
	//#
	//# When an observable object is destroyed, all of its observers are automatically removed.
	//
	//# \also	$@Observable::HasObservers@$
	//# \also	$@Observable::RemoveObserver@$


	//# \function	Observable::RemoveObserver	Removes an observer from an observable object.
	//
	//# \proto	void RemoveObserver(ObserverType *observer);
	//
	//# \param	observer	The observer to remove.
	//
	//# \desc
	//# The $RemoveObserver$ function removes the observer specified by the $observer$ parameter from
	//# an observable object. The observer must have been previously installed through a call to the
	//# $@Observable::AddObserver@$ function.
	//#
	//# When an observable object is destroyed, all of its observers are automatically removed.
	//
	//# \also	$@Observable::HasObservers@$
	//# \also	$@Observable::AddObserver@$


	//# \function	Observable::PostEvent		Posts an event to an observable object.
	//
	//# \proto	virtual void PostEvent(eventType event);
	//
	//# \param	event		The event to post.
	//
	//# \desc
	//# The $PostEvent$ function posts the event specified by the $event$ parameter to an observable object.
	//# This function is normally called by the observable object itself. When this function is called, the
	//# event is sent to each observer currently installed. (The order in which observers are invoked is undefined.)
	//#
	//# If $eventType$ is $void$ for an observable object, then the $event$ parameter should simply be omitted.
	//
	//# \also	$@Observable::HasObservers@$
	//# \also	$@Observable::AddObserver@$
	//# \also	$@Observable::RemoveObserver@$


	template <class observableType, class eventType = void> class Observable
	{
		public:

			typedef eventType									ObservableEventType;
			typedef ObserverBase<observableType, eventType>		ObserverType;

		private:

			List<ObserverType>		observerList;

		protected:

			Observable()
			{
			}

			~Observable()
			{
				observerList.RemoveAll();
			}

		public:

			bool HasObservers(void) const
			{
				return (!observerList.Empty());
			}

			void AddObserver(ObserverType *observer)
			{
				observerList.Append(observer);
			}

			void RemoveObserver(ObserverType *observer)
			{
				observerList.Remove(observer);
			}

			virtual void PostEvent(eventType event);
	};

	template <class observableType, class eventType> void Observable<observableType, eventType>::PostEvent(eventType event)
	{
		ObserverBase<observableType, eventType> *observer = observerList.First();
		while (observer)
		{
			ObserverBase<observableType, eventType> *next = observer->Next();
			observer->InvokeCallback(static_cast<observableType *>(this), event);
			observer = next;
		}
	}


	template <class observableType> class Observable<observableType, void>
	{
		public:

			typedef void									ObservableEventType;
			typedef ObserverBase<observableType, void>		ObserverType;

		private:

			List<ObserverType>		observerList;

		protected:

			Observable()
			{
			}

			~Observable()
			{
				observerList.RemoveAll();
			}

		public:

			bool HasObservers(void) const
			{
				return (!observerList.Empty());
			}

			void AddObserver(ObserverType *observer)
			{
				observerList.Append(observer);
			}

			void RemoveObserver(ObserverType *observer)
			{
				observerList.Remove(observer);
			}

			virtual void PostEvent(void);
	};

	template <class observableType> void Observable<observableType, void>::PostEvent(void)
	{
		ObserverBase<observableType, void> *observer = observerList.First();
		while (observer)
		{
			ObserverBase<observableType, void> *next = observer->Next();
			observer->InvokeCallback(static_cast<observableType *>(this));
			observer = next;
		}
	}


	//# \class	Observer		Encapsulates an object pointer and callback function for an observer.
	//
	//# The $Observer$ class encapsulates an object pointer and callback function for an observer.
	//
	//# \def	template <class observerType, class observableType> class Observer : public ObserverBase<observableType, typename observableType::ObservableEventType>
	//
	//# \tparam		observerType		The type of the class for which a notification function is called when an event occurs.
	//# \tparam		observableType		The type of the class that is being observed. This parameter should
	//#									be the type of the class that inherits directly from the $Observable$ class.
	//
	//# \ctor	template <class eventType> Observer(observerType *observer, void (observerType::*callback)(observableType *, eventType));
	//# \ctor	Observer(observerType *observer, void (observerType::*callback)(observableType *));
	//
	//# \param	observer	A pointer to the object for which a notification function is called when an event occurs.
	//# \param	callback	A pointer to a member function (the notification function) of the object specified by the $observer$ parameter that is called when an event occurs.
	//
	//# \desc
	//# The $Observer$ class encapsulates a member function of an observer object. Observers are installed on observable objects
	//# by calling the $@Observable::AddObserver@$ function. Once installed, the member function stored in the observer object is
	//# called each time an event is posted for the observable object.
	//#
	//# The first parameter of the member function belonging to the observer object is a pointer to the object being observed.
	//# If the event type of the observable object is not $void$, then the event is passed as the second parameter to the member function.
	//# Otherwise, the member function should not take a second parameter.
	//
	//# \privbase	ObserverBase<observableType, typename observableType::ObservableEventType>	Used internally.
	//
	//# \also	$@Observable@$


	template <class observerType, class observableType> class Observer : public ObserverBase<observableType, typename observableType::ObservableEventType>
	{
		public:

			template <class eventType> Observer(observerType *observer, void (observerType::*callback)(observableType *, eventType)) : ObserverBase<observableType, eventType>(reinterpret_cast<NullClass *>(observer), reinterpret_cast<void (NullClass::*)(observableType *, eventType)>(callback))
			{
			}

			Observer(observerType *observer, void (observerType::*callback)(observableType *)) : ObserverBase<observableType>(reinterpret_cast<NullClass *>(observer), reinterpret_cast<void (NullClass::*)(observableType *)>(callback))
			{
			}
	};


	template <class observableType, class eventType = typename observableType::ObservableEventType> class ExclusiveObserverBase : public LinkTarget<ExclusiveObserverBase<observableType, eventType>>
	{
		private:

			NullClass		*observerObject;
			void			(NullClass::*callbackFunction)(observableType *, eventType);

		protected:

			ExclusiveObserverBase(NullClass *observer, void (NullClass::*callback)(observableType *, eventType))
			{
				observerObject = observer;
				callbackFunction = callback;
			}

		public:

			ExclusiveObserverBase(const ExclusiveObserverBase *observer)
			{
				if (observer)
				{
					observerObject = observer->observerObject;
					callbackFunction = observer->callbackFunction;
				}
				else
				{
					callbackFunction = nullptr;
				}
			}

			~ExclusiveObserverBase()
			{
			}

			void InvokeCallback(observableType *observable, eventType event)
			{
				(observerObject->*callbackFunction)(observable, event);
			}
	};


	template <class observableType> class ExclusiveObserverBase<observableType, void> : public LinkTarget<ExclusiveObserverBase<observableType, void>>
	{
		private:

			NullClass		*observerObject;
			void			(NullClass::*callbackFunction)(observableType *);

		protected:

			ExclusiveObserverBase(NullClass *observer, void (NullClass::*callback)(observableType *))
			{
				observerObject = observer;
				callbackFunction = callback;
			}

		public:

			ExclusiveObserverBase(const ExclusiveObserverBase *observer)
			{
				if (observer)
				{
					observerObject = observer->observerObject;
					callbackFunction = observer->callbackFunction;
				}
				else
				{
					callbackFunction = nullptr;
				}
			}

			~ExclusiveObserverBase()
			{
			}

			void InvokeCallback(observableType *observable)
			{
				if (callbackFunction)
				{
					(observerObject->*callbackFunction)(observable);
				}
			}
	};


	//# \class	ExclusiveObservable		The base class for objects that can be observed for events by a single observer only.
	//
	//# An object inherits from the $ExclusiveObservable$ class when it can be observed by a single observer only,
	//# and the $ExclusiveObservable$ class provides a mechanism for notifying the observer of the object when events occur.
	//
	//# \def	template <class observableType, class eventType = void> class ExclusiveObservable
	//
	//# \tparam		observableType		The type of the class that can be observed. This parameter should
	//#									be the type of the class that inherits directly from the $ExclusiveObservable$ class.
	//# \tparam		eventType			The type of an event object that is sent to the observer when an event occurs.
	//#									If there is no event object, then this parameter should be omitted.
	//
	//# \ctor	ExclusiveObservable();
	//
	//# The constructor has protected access takes no parameters.
	//# The $ExclusiveObservable$ class can only exist as a base class for another class.
	//
	//# \desc
	//# The $ExclusiveObservable$ class maintains a link to a single observer that is listening to an observable object
	//# for event notifications. An observable object inherits from the $ExclusiveObservable$ class.
	//#
	//# The exclusive observer are installed by calling the $@ExclusiveObservable::SetObserver@$ function for the observable object.
	//# When an event occurs, the observable object calls the $@ExclusiveObservable::PostEvent@$ function to sent an
	//# event to its current observer.
	//
	//# \also	$@ExclusiveObserver@$
	//# \also	$@Observable@$


	//# \function	ExclusiveObservable::HasObserver	Returns a boolean value indicating whether an observable object has an observer.
	//
	//# \proto	bool HasObserver(void) const;
	//
	//# \desc
	//# The $HasObserver$ function returns $true$ if the observable object for which it is called has
	//# an observer installed. Otherwise, the return value is $false$.
	//
	//# \also	$@ExclusiveObservable::SetObserver@$


	//# \function	ExclusiveObservable::SetObserver	Installs an observer on an observable object.
	//
	//# \proto	void SetObserver(ObserverType *observer);
	//
	//# \param	observer	The observer to install.
	//
	//# \desc
	//# The $SetObserver$ function installs the observer specified by the $observer$ parameter on an
	//# observable object. The observer should have the type $ExclusiveObserver<observerType, observableType>$,
	//# where $observerType$ is the type of the object that is notified when an event occurs, and
	//# $observableType$ is the type of the object being observed. If an observer is already installed
	//# on the object at the time the $SetObserver$ function is called, then it is replaced by the new observer.
	//#
	//# Even though each exclusive observable object can only be observed by one observer at a time, an exclusive
	//# observer can observe any number of objects simultaneously.
	//
	//# \also	$@ExclusiveObservable::HasObserver@$


	//# \function	ExclusiveObservable::PostEvent		Posts an event to an observable object.
	//
	//# \proto	virtual bool PostEvent(eventType event);
	//
	//# \param	event		The event to post.
	//
	//# \desc
	//# The $PostEvent$ function posts the event specified by the $event$ parameter to an observable object.
	//# This function is normally called by the observable object itself. When this function is called and there
	//# is an observer installed, the event is sent to the observer, and $true$ is returned. If there is no
	//# observer installed, then the return value is $false$.
	//#
	//# If $eventType$ is $void$ for an observable object, then the $event$ parameter should simply be omitted.
	//
	//# \also	$@ExclusiveObservable::HasObserver@$
	//# \also	$@ExclusiveObservable::SetObserver@$


	template <class observableType, class eventType = void> class ExclusiveObservable
	{
		public:

			typedef eventType											ObservableEventType;
			typedef ExclusiveObserverBase<observableType, eventType>	ObserverType;

		private:

			Link<ObserverType>		exclusiveObserver;

		protected:

			ExclusiveObservable()
			{
			}

			~ExclusiveObservable()
			{
			}

		public:

			bool HasObserver(void) const
			{
				return (exclusiveObserver.GetTarget() != nullptr);
			}

			void SetObserver(ObserverType *observer)
			{
				exclusiveObserver = observer;
			}

			virtual bool PostEvent(eventType event);
	};

	template <class observableType, class eventType> bool ExclusiveObservable<observableType, eventType>::PostEvent(eventType event)
	{
		ObserverType *observer = exclusiveObserver;
		if (observer)
		{
			observer->InvokeCallback(static_cast<observableType *>(this), event);
			return (true);
		}

		return (false);
	}


	template <class observableType> class ExclusiveObservable<observableType, void>
	{
		public:

			typedef void											ObservableEventType;
			typedef ExclusiveObserverBase<observableType, void>		ObserverType;

		private:

			Link<ObserverType>		exclusiveObserver;

		protected:

			ExclusiveObservable()
			{
			}

			~ExclusiveObservable()
			{
			}

		public:

			bool HasObserver(void) const
			{
				return (exclusiveObserver.GetTarget() != nullptr);
			}

			void SetObserver(ObserverType *observer)
			{
				exclusiveObserver = observer;
			}

			virtual bool PostEvent(void);
	};

	template <class observableType> bool ExclusiveObservable<observableType, void>::PostEvent(void)
	{
		ObserverType *observer = exclusiveObserver;
		if (observer)
		{
			observer->InvokeCallback(static_cast<observableType *>(this));
			return (true);
		}

		return (false);
	}


	//# \class	ExclusiveObserver		Encapsulates an object pointer and callback function for an exclusive observer.
	//
	//# The $ExclusiveObserver$ class encapsulates an object pointer and callback function for an exclusive observer.
	//
	//# \def	template <class observerType, class observableType> class ExclusiveObserver :
	//# \def2	public ExclusiveObserverBase<observableType, typename observableType::ObservableEventType>
	//
	//# \tparam		observerType		The type of the class for which a notification function is called when an event occurs.
	//# \tparam		observableType		The type of the class that is being observed. This parameter should
	//#									be the type of the class that inherits directly from the $ExclusiveObservable$ class.
	//
	//# \ctor	template <class eventType> ExclusiveObserver(observerType *observer, void (observerType::*callback)(observableType *, eventType));
	//# \ctor	ExclusiveObserver(observerType *observer, void (observerType::*callback)(observableType *));
	//
	//# \param	observer	A pointer to the object for which a notification function is called when an event occurs.
	//# \param	callback	A pointer to a member function (the notification function) of the object specified by the $observer$ parameter that is called when an event occurs.
	//
	//# \desc
	//# The $ExclusiveObserver$ class encapsulates a member function of an observer object. Observers are installed on exclusive observable objects
	//# by calling the $@ExclusiveObservable::SetObserver@$ function. Once installed, the member function stored in the observer object is
	//# called each time an event is posted for the observable object.
	//#
	//# The first parameter of the member function belonging to the observer object is a pointer to the object being observed.
	//# If the event type of the observable object is not $void$, then the event is passed as the second parameter to the member function.
	//# Otherwise, the member function should not take a second parameter.
	//
	//# \privbase	ExclusiveObserverBase<observableType, typename observableType::ObservableEventType>		Used internally.
	//
	//# \also	$@ExclusiveObservable@$


	template <class observerType, class observableType> class ExclusiveObserver : public ExclusiveObserverBase<observableType, typename observableType::ObservableEventType>
	{
		public:

			template <class eventType> ExclusiveObserver(observerType *observer, void (observerType::*callback)(observableType *, eventType)) : ExclusiveObserverBase<observableType, eventType>(reinterpret_cast<NullClass *>(observer), reinterpret_cast<void (NullClass::*)(observableType *, eventType)>(callback))
			{
			}

			ExclusiveObserver(observerType *observer, void (observerType::*callback)(observableType *)) : ExclusiveObserverBase<observableType>(reinterpret_cast<NullClass *>(observer), reinterpret_cast<void (NullClass::*)(observableType *)>(callback))
			{
			}
	};
}


#endif

// ZYUQURM
