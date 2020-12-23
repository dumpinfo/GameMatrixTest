#pragma once

#include "../Leadwerks.h"

namespace Leadwerks
{
	class Entity;
	class Argument;
	
	class Connection
	{
	public:
		Component* target;
		Component* source;
		std::string outputname;
		std::string inputname;
		std::vector<Argument> arguments;
		std::list<Connection*>::iterator inputlink;
		std::list<Connection*>::iterator outputlink;
		
		Connection();
		virtual ~Connection();
		
		void SetArgument(Component* component, const std::string& functionname, const int index);
		//void AddArgument(Component* component, const std::string& functionname);
		virtual bool Fire();
	};
}
