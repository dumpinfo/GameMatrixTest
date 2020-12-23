#pragma once

#include "../OpenALSoundDriver.h"

namespace Leadwerks
{
	class OpenALSource : public Source
	{
	public:
		int buffer;
		int updateiterations;
		
		OpenALSource();
		virtual ~OpenALSource(); 
		
		virtual void SetTime(const float time);
		virtual float GetTime();
		virtual void Play();
		virtual void Pause();
		virtual void Resume();
		virtual void Cue();
		virtual void Stop();
		virtual void SetSound(Sound* sound);
		virtual int GetState();
		virtual void SetDirection(const Vec3& direction);
		virtual void SetConeAngles(const float inner, const float outer);
		virtual void SetVolume(const float volume);
		virtual void SetPitch(const float pitch);
		virtual void SetVelocity(const float velocity);
		virtual void SetRange(const float range);
		virtual void SetLoopMode(const bool loop);
		virtual void SetPosition(const Vec3& position);
		virtual float GetPitch();
		virtual float GetVolume();
	};
}
