#pragma once

#include "../OpenALSoundDriver.h"

namespace Leadwerks
{
	class Listener;
	
	class OpenALListener : public Listener
	{
		virtual void UpdateMatrix();
		virtual Entity* Instance(const bool b,const bool callstartfunction);
		virtual Entity* Copy(const bool b,const bool callstartfunction);
	};
}
