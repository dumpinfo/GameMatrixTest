 

#ifndef C4Commands_h
#define C4Commands_h


//# \component	System Utilities
//# \prefix		System/


#include "C4Types.h"


namespace C4
{
	enum
	{
		kMaxCommandNameLength		= 31,
		kMaxCommandLength			= 255
	};


	class Engine;


	//# \class	Command		Stores a system command.
	//
	//# The $Command$ class stores a system command.
	//
	//# \def	class Command : public MapElement<Command>, public Observable<Command, const char *>
	//
	//# \ctor	Command(const char *name, ObserverType *observer);
	//
	//# \param	name		A pointer to the name of the command.
	//# \param	observer	An observer to invoke when the command is executed.
	//
	//# \desc
	//# The $Command$ class encapsulates the name and function pointer for a system command. The name of a system
	//# command is a text string containing up to $kMaxCommandNameLength$ characters. The $observer$ parameter specifies
	//# an observer that is invoked when the command with that name is executed.
	//
	//# The $const char *$ event parameter passed to an observer points to the string following the command name and any whitespace
	//# on the command line when the command is executed. This string can be parsed by the observer and interpreted as
	//# command parameters.
	//#
	//# Once a command has been created, it can be registered with the engine by calling the $@Engine::AddCommand@$ function.
	//# A command is unregistered by simply destroying it.
	//
	//# \base	Utilities/MapElement<Command>					Used internally to store all commands in a map.
	//# \base	Utilities/Observable<Command, const char *>		The observers of a command are invoked when the command is executed.
	//
	//# \also	$@Engine::AddCommand@$
	//# \also	$@Engine::GetCommand@$
	//# \also	$@Engine::GetFirstCommand@$


	//# \function	Command::GetName		Returns the command name.
	//
	//# \proto	const char *GetName(void) const;
	//
	//# \desc
	//# The $GetName$ function returns a pointer to the name of a command.
	//
	//# \also	$@Engine::GetCommand@$


	class Command : public MapElement<Command>, public Observable<Command, const char *>
	{
		private:

			String<kMaxCommandNameLength>	commandName;

		public:

			typedef ConstCharKey KeyType;

			C4API Command(const char *name, ObserverType *observer);
			C4API virtual ~Command();

			KeyType GetKey(void) const
			{
				return (commandName);
			}

			const char *GetName(void) const
			{
				return (commandName);
			}
	};


	template <class observerType> class CommandObserver : public Observer<observerType, Command>
	{
		public:

			CommandObserver(observerType *observer, void (observerType::*callback)(Command *, const char *)) : Observer<observerType, Command>(observer, callback)
			{
			}
	};
}


#endif 

// ZYUQURM
