#pragma once

#include "../OpenALSoundDriver.h"

namespace Leadwerks
{
	class OpenALSource;
	
	class OpenALChannel : public Channel
	{
	public:
		ALuint source;//OpenAL source
		
		OpenALChannel();
		virtual ~OpenALChannel();
		
		virtual void Update(OpenALSource* openalsource);
		virtual void PutBack();
	};
}
