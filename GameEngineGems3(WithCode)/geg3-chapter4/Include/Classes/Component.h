#pragma once
#include "../Leadwerks.h"

namespace Leadwerks
{
	class Connection;
	class ComponentBase;
    
	class Component : public Object//lua
	{
	public:
        ComponentBase* base;
		Entity* entity;
		int hooks;
		std::list<Connection*> output;
		std::list<Connection*> input;
		
		Component();
		virtual ~Component();
		
        //virtual bool ContainsFunction(const std::string& name);
		virtual void SetObject(const std::string& name, Object* o, const bool takeownership=false);
		virtual void SetObject(const std::string& name, Object o);
		virtual void SetString(const std::string& name, const std::string& s);
		virtual void SetFloat(const std::string& name, const float f);
		virtual void SetInt(const std::string& name, const int i);
		virtual void SetBool(const std::string& name, const bool b);
		virtual std::string GetClassName();
		virtual Connection* AddOutput(const std::string& outputname, Component* target, const std::string& inputname);
		virtual void CallOutputs(const std::string& outputname);//lua
		
		static Component* Load(const std::string& path, const int flags = 0, const uint64_t fileid=0);
	};
}
