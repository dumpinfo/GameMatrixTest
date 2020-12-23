#pragma once

#include "../../Leadwerks.h"

namespace Leadwerks
{
	class Listener : public Entity//lua
	{
	public:
		SoundDriver* sounddriver;
		float range;
		Vec2 radius;
		
		Listener();

		virtual void CopyTo(Entity* entity,const int mode);
		virtual Entity* Copy(const bool recursive = true, const bool callstartfunction=true);
		virtual Entity* Instance(const bool recursive = true, const bool callstartfunction=true);
		virtual std::string GetClassName();
		
		static Listener* Create(Entity* parent=NULL);//lua
	};
}
