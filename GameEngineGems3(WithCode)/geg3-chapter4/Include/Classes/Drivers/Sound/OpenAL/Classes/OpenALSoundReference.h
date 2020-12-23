#pragma once

#include "../OpenALSoundDriver.h"

namespace Leadwerks
{
	class OpenALSoundReference : public SoundReference
	{
	public:
		ALuint buffer;
		
		OpenALSoundReference();
		virtual ~OpenALSoundReference();
		
		virtual bool Reload(const int flags=0);
		virtual void Reset();
	};
}
