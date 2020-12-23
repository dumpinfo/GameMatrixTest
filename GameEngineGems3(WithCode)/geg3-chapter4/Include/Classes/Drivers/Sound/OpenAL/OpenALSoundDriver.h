#pragma once

#include "../../../../Leadwerks.h"
#include "Classes/OpenALListener.h"
#include "Classes/OpenALSound.h"
#include "Classes/OpenALChannel.h"
#include "Classes/OpenALSource.h"

namespace Leadwerks
{
	const int OPENALSOUNDDRIVER_MAX_SOURCES = 29;
	
	class OpenALSoundDriver : public SoundDriver//lua
	{
	public:
		int countactivesources;
		ALuint voices[OPENALSOUNDDRIVER_MAX_SOURCES];
		bool voiceusage[OPENALSOUNDDRIVER_MAX_SOURCES];
		Vec3 listener;
		ALCdevice* device;
		ALCcontext* context;
		int updateiterations;

		OpenALSoundDriver();
		virtual ~OpenALSoundDriver();
		
		virtual void Suspend();
		virtual void Resume();
		virtual Listener* CreateListener();
		virtual int GetMaxChannels();
		virtual Channel* CreateChannel();
		virtual Source* CreateSource();
		virtual Sound* CreateSound();
		virtual void Update();
		
		static OpenALSoundDriver* Create();//lua
	};

	void alCheckError(ALCdevice* device=NULL);
}
