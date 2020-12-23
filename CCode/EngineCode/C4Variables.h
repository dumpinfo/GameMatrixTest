 

#ifndef C4Variables_h
#define C4Variables_h


//# \component	System Utilities
//# \prefix		System/


#include "C4Types.h"


namespace C4
{
	//# \enum	VariableFlags

	enum
	{
		kVariableNonpersistent		= 1 << 0,	//## The variable should not be saved across different executions of the engine.
		kVariablePermanent			= 1 << 1	//## The variable cannot be deleted by the user.
	};


	enum
	{
		kMaxVariableNameLength		= 31,
		kMaxVariableValueLength		= 255
	};


	//# \class	Variable	Stores a system variable.
	//
	//# The $Variable$ class stores a system variable.
	//
	//# \def	class Variable : public MapElement<Variable>, public Observable<Variable>
	//
	//# \ctor	explicit Variable(const char *name, unsigned_int32 flags = 0,
	//# \ctor2	Variable::ObserverType *observer = nullptr);
	//
	//# \param	name		A pointer to the name of the variable.
	//# \param	flags		The variable flags. See below for possible values.
	//# \param	observer	An observer that is invoked when the variable is modified.
	//
	//# \desc
	//# The $Variable$ class encapsulates the name and value of a system variable. The value of a system variable is
	//# a text string containing up to $kMaxVariableValueLength$ characters. The initial value of a variable upon
	//# construction is the empty string.
	//#
	//# The $flags$ parameter specifies a set of flags that control properties of the variable. The flags can be
	//# a combination (through logical OR) of the following constants.
	//
	//# \table	VariableFlags
	//
	//# If the $observer$ parameter is specified, then it provides an observer function that is called each time the variable's
	//# value is modified through one of the member functions that set the value.
	//#
	//# Once a variable has been created, it can be registered with the engine by calling the $@Engine::AddVariable@$ function.
	//# A variable is unregistered by simply destroying it.
	//
	//# \base	Utilities/MapElement<Variable>		Used internally to store all variables in a map.
	//# \base	Utilities/Observable<Variable>		Observers can be invoked when the variable is modified.
	//
	//# \also	$@Engine::AddVariable@$
	//# \also	$@Engine::GetVariable@$
	//# \also	$@Engine::GetFirstVariable@$


	//# \function	Variable::GetName		Returns the variable name.
	//
	//# \proto	const char *GetName(void) const;
	//
	//# \desc
	//# The $GetName$ function returns a pointer to the name of a variable.
	//
	//# \also	$@Engine::GetVariable@$


	//# \function	Variable::GetVariableFlags		Returns the variable flags.
	//
	//# \proto	unsigned_int32 GetVariableFlags(void) const;
	//
	//# \desc
	//# The $GetVariableFlags$ function returns the variable flags, which can be a combination (through logical OR) of the following constants.
	//
	//# \table	VariableFlags
	//
	//# \also	$@Variable::SetVariableFlags@$


	//# \function	Variable::SetVariableFlags		Returns the variable flags.
	//
	//# \proto	void SetVariableFlags(unsigned_int32 flags);
	//
	//# \param	flags	The new variable flags.
	//
	//# \desc
	//# The $SetVariableFlags$ function sets the variable flags to the value specified by the $flags$ parameter,
	//# which can be a combination (through logical OR) of the following constants.
	//
	//# \table	VariableFlags
	//
	//# \also	$@Variable::GetVariableFlags@$ 


	//# \function	Variable::GetValue			Returns the variable value as a string. 
	//
	//# \proto	const String<kMaxVariableValueLength>& GetValue(void) const; 
	//
	//# \desc
	//# The $GetValue$ function returns the value of a variable as a string. Since the value is stored internally as a string, 
	//# this is the fastest way to retrieve the value of a variable.
	// 
	//# \also	$@Variable::SetValue@$ 
	//# \also	$@Variable::GetIntegerValue@$
	//# \also	$@Variable::SetIntegerValue@$
	//# \also	$@Variable::GetFloatValue@$
	//# \also	$@Variable::SetFloatValue@$ 


	//# \function	Variable::SetValue			Sets the variable value as a string.
	//
	//# \proto	void SetValue(const char *value);
	//
	//# \param	value	A pointer to a string that will become the new variable value.
	//
	//# \desc
	//# The $SetValue$ function sets the value of a variable to the string specified by the $value$ parameter. If the string
	//# is longer than $kMaxVariableValueLength$ characters, then it is truncated.
	//#
	//# If the variable has observers installed, then they are invoked after the variable's value is set.
	//
	//# \also	$@Variable::GetValue@$
	//# \also	$@Variable::GetIntegerValue@$
	//# \also	$@Variable::SetIntegerValue@$
	//# \also	$@Variable::GetFloatValue@$
	//# \also	$@Variable::SetFloatValue@$


	//# \function	Variable::GetIntegerValue		Returns the variable value as an integer.
	//
	//# \proto	int32 GetIntegerValue(void) const;
	//
	//# \desc
	//# The $GetIntegerValue$ function returns the value of a variable as an integer.
	//
	//# \also	$@Variable::SetIntegerValue@$
	//# \also	$@Variable::GetFloatValue@$
	//# \also	$@Variable::SetFloatValue@$
	//# \also	$@Variable::GetValue@$
	//# \also	$@Variable::SetValue@$


	//# \function	Variable::SetIntegerValue		Sets the variable value as an integer.
	//
	//# \proto	void SetIntegerValue(int32 value);
	//
	//# \param	value	The new variable value.
	//
	//# \desc
	//# The $SetIntegerValue$ function sets the value of a variable to the integer specified by the $value$ parameter.
	//#
	//# If the variable has observers installed, then they are invoked after the variable's value is set.
	//
	//# \also	$@Variable::GetIntegerValue@$
	//# \also	$@Variable::GetFloatValue@$
	//# \also	$@Variable::SetFloatValue@$
	//# \also	$@Variable::GetValue@$
	//# \also	$@Variable::SetValue@$


	//# \function	Variable::GetFloatValue		Returns the variable value as a floating-point number.
	//
	//# \proto	float GetFloatValue(void) const;
	//
	//# \desc
	//# The $GetFloatValue$ function returns the value of a variable as a floating-point number.
	//
	//# \also	$@Variable::SetFloatValue@$
	//# \also	$@Variable::GetIntegerValue@$
	//# \also	$@Variable::SetIntegerValue@$
	//# \also	$@Variable::GetValue@$
	//# \also	$@Variable::SetValue@$


	//# \function	Variable::SetFloatValue		Returns the variable value as a floating-point number.
	//
	//# \proto	void SetFloatValue(float value);
	//
	//# \param	value	The new variable value.
	//
	//# \desc
	//# The $SetFloatValue$ function sets the value of a variable to the floating-point number specified by the $value$ parameter.
	//#
	//# If the variable has observers installed, then they are invoked after the variable's value is set.
	//
	//# \also	$@Variable::GetFloatValue@$
	//# \also	$@Variable::GetIntegerValue@$
	//# \also	$@Variable::SetIntegerValue@$
	//# \also	$@Variable::GetValue@$
	//# \also	$@Variable::SetValue@$


	class Variable : public MapElement<Variable>, public Observable<Variable>
	{
		private:

			unsigned_int32						variableFlags;
			String<kMaxVariableNameLength>		variableName;
			String<kMaxVariableValueLength>		variableValue;

		public:

			typedef ConstCharKey KeyType;

			C4API explicit Variable(const char *name, unsigned_int32 flags = 0, ObserverType *observer = nullptr);
			C4API virtual ~Variable();

			KeyType GetKey(void) const
			{
				return (variableName);
			}

			unsigned_int32 GetVariableFlags(void) const
			{
				return (variableFlags);
			}

			void SetVariableFlags(unsigned_int32 flags)
			{
				variableFlags = flags;
			}

			const char *GetName(void) const
			{
				return (variableName);
			}

			const String<kMaxVariableValueLength>& GetValue(void) const
			{
				return (variableValue);
			}

			int32 GetIntegerValue(void) const
			{
				return (Text::StringToInteger(variableValue));
			}

			float GetFloatValue(void) const
			{
				return (Text::StringToFloat(variableValue));
			}

			C4API void SetValue(const char *value);
			C4API void SetIntegerValue(int32 value);
			C4API void SetFloatValue(float value);
	};


	template <class observerType> class VariableObserver : public Observer<observerType, Variable>
	{
		public:

			VariableObserver(observerType *observer, void (observerType::*callback)(Variable *)) : Observer<observerType, Variable>(observer, callback)
			{
			}
	};
}


#endif

// ZYUQURM
