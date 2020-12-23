#pragma once

#include "../Leadwerks.h"

namespace Leadwerks
{
	class SoundReference : public AssetReference
	{
	public:
		
		Bank* data;
		float length;
		int channels;
		int format;
		int frequency;
		int bitrate;
		
		SoundReference();
		virtual ~SoundReference();

		virtual bool Reload(const int mode);
		virtual void Reset();
		virtual AssetReference* Copy();
		virtual Asset* Instance();
	};
}
