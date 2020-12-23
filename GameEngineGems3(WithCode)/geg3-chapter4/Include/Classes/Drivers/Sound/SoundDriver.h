#pragma once

#include "../../../Leadwerks.h"

namespace Leadwerks
{
	class SoundDriver : public Driver//lua
	{
	public:
		//std::map<std::string,AssetReference*> loadedassets;
		std::list<Source*> sources;
		std::vector<Channel*> channels;
		int usedchannels;
		
		SoundDriver();
		virtual ~SoundDriver();
		
		virtual void Suspend()=0;
		virtual void Resume()=0;
		virtual Listener* CreateListener()=0;
		virtual int GetMaxChannels()=0;
		virtual Channel* CreateChannel()=0;
		virtual Channel* RequestChannel();
		virtual Sound* CreateSound()=0;
		virtual Source* CreateSource()=0;
		virtual void Update()=0;
		virtual int CountChannels();
		
		static SoundDriver* GetCurrent();//lua
		static void SetCurrent(SoundDriver* sounddriver);//lua
	};
	
	extern SoundDriver* ActiveSoundDriver;
}
