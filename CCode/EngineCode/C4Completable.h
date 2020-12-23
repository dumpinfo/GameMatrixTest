 

#ifndef C4Completable_h
#define C4Completable_h


//# \component	Utility Library
//# \prefix		Utilities/


#include "C4Memory.h"


namespace C4
{
	//# \class	Completable		The base class for objects that can complete a time-consuming task.
	//
	//# Objects inherit from the $Completable$ class when they perform a time-consuming task
	//# and wish to provide a mechanism for notifying the user of the object when such a task
	//# has been completed.
	//
	//# \def	template <class classType, typename cookieType = void *> class Completable
	//
	//# \tparam		classType	The type of the class which performs a completable action.
	//#							This parameter should be the type of the class that inherits directly
	//#							from the $Completable$ class.
	//# \tparam		cookieType	The type of the user-defined data that is passed to the completion
	//#							procedure encapsulated by the $Completable$ class. The cookie itself
	//#							is specified by calling $@Completable::SetCompletionProc@$ or
	//#							$@Completable::SetCompletionCookie@$.
	//
	//# \ctor	Completable();
	//
	//# The constructor has protected access takes no parameters.
	//# The $Completable$ class can only exist as a base class for another class.
	//
	//# \desc
	//# The $Completable$ class encapsulates a completion procedure pointer and a data value which
	//# is passed to this completion procedure. The completion procedure is invoked when the
	//# $@Completable::CallCompletionProc@$ function is called. This usually happens when a subclass
	//# of $Completable$ completes a time consuming task and wishes to notify some other piece of code.
	//#
	//# Upon construction, the completion procedure is set to $nullptr$, causing calls to the
	//# $@Completable::CallCompletionProc@$ function to have no effect.


	//# \function	Completable::GetCompletionProc		Returns the completion procedure for an object.
	//
	//# \proto		CompletionProc *GetCompletionProc(void) const;
	//
	//# \desc
	//# The $GetCompletionProc$ function returns a pointer to the currently installed completion
	//# procedure. If no completion procedure has been installed, then this function returns $nullptr$.
	//# The $CompletionProc$ type is defined as
	//
	//# \code	typedef void CompletionProc(classType *, cookieType);
	//
	//# where $classType$ and $cookieType$ are the template parameters for the $Completable$ class template.
	//
	//# \also	$@Completable::SetCompletionProc@$
	//# \also	$@Completable::GetCompletionCookie@$
	//# \also	$@Completable::SetCompletionCookie@$
	//# \also	$@Completable::CallCompletionProc@$


	//# \function	Completable::SetCompletionProc		Sets the completion procedure for an object.
	//
	//# \proto		void SetCompletionProc(CompletionProc *proc);
	//# \proto		void SetCompletionProc(CompletionProc *proc, cookieType cookie);
	//
	//# \param		proc	A pointer to the completion procedure. The function passed will be invoked
	//#						whenever $@Completable::CallCompletionProc@$ is called (usually from a subclass).
	//#						The $proc$ parameter may be $nullptr$, in which case the object has no completion
	//#						procedure and calls to $@Completable::CallCompletionProc@$ have no effect.
	//# \param		cookie	The cookie that is passed to the completion procedure as its last parameter.
	//
	//# \desc
	//# The $SetCompletionProc$ function sets the completion procedure for an object that inherits from
	//# the $@Completable@$ class template. The $CompletionProc$ type is defined as
	//
	//# \code	typedef void CompletionProc(classType *, cookieType);
	//
	//# where $classType$ and $cookieType$ are the template parameters for the $Completable$ class template.
	//# When a subclass completes a task and wishes to notify the user of the object that the task has
	//# completed, it calls the $@Completable::CallCompletionProc@$ function.
	//
	//# \also	$@Completable::GetCompletionProc@$
	//# \also	$@Completable::GetCompletionCookie@$
	//# \also	$@Completable::SetCompletionCookie@$
	//# \also	$@Completable::CallCompletionProc@$


	//# \function	Completable::GetCompletionCookie	Returns the cookie that is passed to the completion procedure.
	//
	//# \proto		cookieType GetCompletionCookie(void) const;
	//
	//# \desc
	//# The $GetCompletionCookie$ function returns the cookie of type $cookieType$ that is passed to the
	//# completion procedure. The type $cookieType$ is one of the template parameters for the $@Completable@$ class.
	//# The cookie is undefined until it is set through either of the $@Completable::SetCompletionProc@$ or
	//# $@Completable::SetCompletionCookie@$ functions.
	//
	//# \also	$@Completable::GetCompletionProc@$ 
	//# \also	$@Completable::SetCompletionProc@$
	//# \also	$@Completable::SetCompletionCookie@$
	//# \also	$@Completable::CallCompletionProc@$ 

 
	//# \function	Completable::SetCompletionCookie	Sets the cookie that is passed to the completion procedure.
	//
	//# \proto		void SetCompletionCookie(cookieType cookie); 
	//
	//# \param		cookie	The cookie that is passed to the completion procedure as its last parameter. 
	// 
	//# \desc
	//# The $SetCompletionCookie$ function sets the cookie that is passed to the completion procedure for an
	//# object that inherits from the $@Completable@$ class template. When a subclass completes a task and
	//# wishes to notify the user of the object that the task has completed, it calls the 
	//# $@Completable::CallCompletionProc@$ function.
	//
	//# \also	$@Completable::GetCompletionProc@$
	//# \also	$@Completable::SetCompletionProc@$
	//# \also	$@Completable::GetCompletionCookie@$
	//# \also	$@Completable::CallCompletionProc@$


	//# \function	Completable::CallCompletionProc		Invokes the completion procedure for an object.
	//
	//# \proto		void CallCompletionProc(void);
	//
	//# \desc
	//# The $CallCompletionProc$ function invokes the currently installed completion procedure for an object.
	//# If no completion procedure is installed, then calling $CallCompletionProc$ has no effect. A completion
	//# procedure is installed by calling $@Completable::SetCompletionProc@$.
	//
	//# \also	$@Completable::GetCompletionProc@$
	//# \also	$@Completable::SetCompletionProc@$
	//# \also	$@Completable::GetCompletionCookie@$
	//# \also	$@Completable::SetCompletionCookie@$


	template <class classType, typename cookieType = void *> class Completable
	{
		public:

			typedef void CompletionProc(classType *, cookieType);

		private:

			CompletionProc		*completionProc;
			cookieType			completionCookie;

		protected:

			Completable()
			{
				completionProc = nullptr;
			}

		public:

			virtual ~Completable() = default;

			CompletionProc *GetCompletionProc(void) const
			{
				return (completionProc);
			}

			void SetCompletionProc(CompletionProc *proc)
			{
				completionProc = proc;
			}

			cookieType GetCompletionCookie(void) const
			{
				return (completionCookie);
			}

			void SetCompletionCookie(cookieType cookie)
			{
				completionCookie = cookie;
			}

			void SetCompletionProc(CompletionProc *proc, cookieType cookie)
			{
				completionProc = proc;
				completionCookie = cookie;
			}

			void CallCompletionProc(void)
			{
				if (completionProc)
				{
					(*completionProc)(static_cast<classType *>(this), completionCookie);
				}
			}
	};
}


#endif

// ZYUQURM
