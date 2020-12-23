#pragma once

#include "../OpenALSoundDriver.h"

namespace Leadwerks
{
	class OpenALSound : public Sound
	{
	public:
		ALuint buffer;
		
		OpenALSound();
		virtual ~OpenALSound();
		
		virtual bool Reload(const int flags=0);
		virtual void Reset();
	};
}
