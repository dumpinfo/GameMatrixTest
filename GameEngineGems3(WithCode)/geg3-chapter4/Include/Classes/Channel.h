#pragma once

#include "../Leadwerks.h"

namespace Leadwerks
{
    class SoundDriver;
    
	class Channel : public Object
	{
	public:
		bool available;
		SoundDriver* driver;
		
		Channel();
		virtual ~Channel();
		
		virtual void PutBack();
	};
}
