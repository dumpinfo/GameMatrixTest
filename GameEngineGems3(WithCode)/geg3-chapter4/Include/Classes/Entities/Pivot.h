#pragma once

#include "../../Leadwerks.h"

namespace Leadwerks
{
		
	class Entity;
	
	class Pivot : public Entity//lua
	{
	public:
		Pivot();
		virtual ~Pivot();

		virtual Entity* Copy(const bool recursive = true, const bool callstartfunction=true);
		virtual Entity* Instance(const bool recursive = true, const bool callstartfunction=true);
		virtual std::string GetClassName();

		static Pivot* Create(Entity* parent=NULL);//lua
	};
}
