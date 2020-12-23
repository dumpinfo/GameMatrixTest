namespace Leadwerks
{
	class Channel;
    class SoundDriver;
    
	class Source : public Object//lua
	{
	public:
		SoundDriver* driver;
		float volume;
		float pitch;
		float range;
		float velocity;
		bool loopmode;
		Vec3 position;
		Vec3 direction;
		int elapsed;
        Sound* sound;
		int state;
		std::list<Source*>::iterator link;
		Channel* channel;
		float time;
		Vec2 coneangle;
		float minvolume;
		bool autorelease;
		
		Source();
		virtual ~Source();
		
		virtual void SetTime(const float time)=0;//lua
		virtual float GetTime()=0;//lua
		virtual void Play()=0;//lua
		virtual void Cue()=0;//lua
		virtual void Pause()=0;//lua
		virtual void Resume()=0;//lua
		virtual void Stop()=0;//lua
		virtual void SetDirection(const Vec3& direction)=0;
		virtual void SetConeAngles(const float inner, const float outer)=0;
		virtual void SetSound(Sound* sound)=0;//lua
		virtual int GetState()=0;//lua
		virtual void SetVolume(const float volume)=0;//lua
		virtual void SetPitch(const float pitch)=0;//lua
		virtual void SetVelocity(const float velocity)=0;//lua
		virtual void SetRange(const float range)=0;//lua
		virtual void SetLoopMode(const bool loop)=0;//lua
		virtual void SetPosition(const Vec3& position)=0;//lua
		virtual std::string GetClassName();
		virtual float GetPitch()=0;//lua
		virtual float GetVolume()=0;//lua
		
		static Source* Create();//lua
		
		static const int Stopped;//lua
		static const int Playing;//lua
		static const int Paused;//lua
		
		Entity* owner;
	};
}
