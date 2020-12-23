#pragma once
#include "../Leadwerks.h"

namespace Leadwerks
{
	class ComponentBase : public Asset
	{
	public:
		std::string classname;
		std::map<std::string,std::string> variabletypes;
		
		ComponentBase();
		virtual ~ComponentBase();
		
		virtual void Initialize(Entity* entity);
        virtual Asset* Copy();
        virtual Component* Instance();
		virtual std::string GetClassName();
		virtual bool Reload(const int flags=0);
		virtual void Reset();
	};
}
