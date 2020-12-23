#pragma once
#include "../Leadwerks.h"

namespace Leadwerks
{
	class Entity;
	class Output;
	
	class Actor : public Object
	{
	public:
		Actor(Script* script, Entity* entity=NULL);
		~Actor();
		
		std::list<Output*> outputs;
		std::list<Output*> inputs;
		std::list<Argument*> arguments;//arguments that use this actor as a source of data
		
		Entity* entity;
		
		//Script* script;
		//virtual void SetVec3(const float name, const Vec3& v);
		virtual void SetInt(const std::string& name, const int i);
		virtual void SetVec3(const std::string& name, const Vec3& v);
		virtual void SetVec4(const std::string& name, const Vec4& v);
		virtual void SetObject(const std::string& name, Object* o);
		virtual Object* GetObject(const std::string& name);
		virtual void SetString(const std::string& name,const std::string& value);
		virtual std::string GetString(const std::string& name);
		virtual void SetBool(const std::string& name, const bool b);
		virtual void SetFloat(const std::string& name, const float f);
		virtual bool CallFunction(const std::string& name);
		virtual std::string Debug();
		virtual void CallOutputs(const std::string& name);
	};
	
	Actor* LoadActor(const std::string& path, Entity* entity=NULL, const int flags=0);
}
