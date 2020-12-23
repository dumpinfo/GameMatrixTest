#pragma once

#include "../Leadwerks.h"

namespace Leadwerks
{
	class Sound : public Asset//lua
	{
	public:
		
		Bank* data;
		float length;
		int channels;
		int format;
		int frequency;
		int bitrate;
		
		Sound();
		virtual ~Sound();
        
		virtual bool Reload(const int mode);
		virtual void Reset();
		virtual Asset* Copy();
        virtual float GetLength();//lua
        virtual void Play();//lua
		
        static const int Mono8;//lua
        static const int Stereo8;//lua
        static const int Mono16;//lua
        static const int Stereo16;//lua
        static const int Mono32;//lua
        static const int Stereo32;//lua
        
		static Sound* Load(const std::string& path, const int flags = 0, const uint64_t fileid=0);//lua
		virtual std::string GetClassName();
	};
}
