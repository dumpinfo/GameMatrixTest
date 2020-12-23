#pragma once
#include "../Leadwerks.h"

namespace Leadwerks
{
	class Component;
	
	class Argument
	{
	public:
		Component* component;
		std::string functionname;
		
		Argument();
		virtual ~Argument();
		
		virtual bool PushResult();
	};
}
