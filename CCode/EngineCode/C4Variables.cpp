 

#include "C4Variables.h"


using namespace C4;


Variable::Variable(const char *name, unsigned_int32 flags, ObserverType *observer)
{
	variableFlags = flags;
	variableName = name;
	variableValue[0] = 0;

	if (observer)
	{
		AddObserver(observer);
	}
}

Variable::~Variable()
{
}

void Variable::SetValue(const char *value)
{
	variableValue = value;
	PostEvent();
}

void Variable::SetIntegerValue(int32 value)
{
	variableValue = value;
	PostEvent();
}

void Variable::SetFloatValue(float value)
{
	variableValue = value;
	PostEvent();
}

// ZYUQURM
