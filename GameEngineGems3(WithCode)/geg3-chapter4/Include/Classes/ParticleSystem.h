#include "../../Leadwerks3D.h"

/*
Coding Standards
-All Leadwerks3D objects are derived from the Object class.
-All classes have a constructor and virtual destructor, even if they don't do anything.
-All class functions are always virtual.
-All class members that are pointers are always initialized to NULL in the constructor.
-All class members and functions are always public.
-Classes are always declared in the following order:
	-Members
	-Constructor(s) & destructor
	-Functions
	-Static members
	-Static functions
-In .cpp files, the constructor(s) is/are always at the top of the file, followed by the destructor.
*/

namespace Leadwerks3D
{
	//Declare any other classes we use
	class Particle;
	class Emitter;
	class Attractor;
	
	class ParticleSystem : public Object
	{
	public:
		
		//Members
		int particlecount;
		std::vector<Particle> particle;//Particles stored here
		std::vector<bool> activeList;
		int activeCount;
		int duration;
		float deviation;
		Surface* surface;
		//Shader* shader;
		//Material* material;
		Vec3 position;
		Vec3 velocity[2]; //0 = min & 1 = max
		Vec3 acceleration[2]; 
		Vec4 color[2]; //0 = initial color & 1  = final color
		Vec4 colorStep;
		Vec3 scale[2];
		Vec3 scaleStep;
		float rotation[2];
		float rotationStep;
		Vec3 positionOffset;
		Emitter* emitter;  
		AABB aabb;
		bool fadeIn;
		float fadeStart;
		bool linearOut;
		bool rotationByVelocity;
		int releaseQuantity;
		float interval;
		bool loop;
		bool paused;

		std::list<Attractor*> attractors;
		bool type1; 
		
		
		//Constructor / destructor
		ParticleSystem();
		virtual ~ParticleSystem();
		
		//Class functions
		virtual void Reset();

		//Function to update particles
		virtual void Update();		
		virtual void Draw(Camera* camera, const bool drawsorted);
		virtual void SetMaterial(Material* material);
		virtual Material* GetMaterial();
		virtual void SetColor(const float r, const float g, const float b, const float a, const int index);
		virtual Vec4 GetColor(const int index);
		virtual void SetPosition(const float x, const float y, const float z, const bool global);
		virtual Vec3 GetPosition();
		virtual void SetScale(const float x, const float y, const float z, const int index);
		virtual Vec3 GetScale(const int index);
		virtual void SetVelocity(const float x, const float y, const float z, const int index);
		virtual Vec3 GetVelocity(const int index);
		virtual void SetAcceleration(const float x, const float y, const float z, const int index);
		virtual Vec3 GetAcceleration(const int index);
		virtual void SetRotation(const float angle, const int index);
		virtual float GetRotation(const int index);
		virtual void SetPositionOffset(const float x, const float y, const float z);
		virtual Vec3 GetPositionOffset();
		virtual Vec4 CalculateColorStep(const int mode);
		virtual float CalculateRotationStep();
		virtual Vec3 CalculateScaleStep();
		virtual void SetRotationByVelocity(const bool setByVel);
		virtual void SetFadeIn(const bool FadeIn);
		virtual void SetLinearOut(const bool LinearOut);
		virtual void SetReleaseQuantity(const int quantity);
		virtual int GetReleaseQuantity();
		virtual void SetInterval(const float interval);
		virtual float GetInterval();
		virtual void Initialize();
		virtual void SetLoop(const bool Loop);
		virtual bool GetLoop();
		virtual void SetParticleCount(const int ParticleCount);
		virtual int GetParticleCount();
		virtual void SetDuration(const int Duration);
		virtual int GetDuration();
		

		
		//Static functions
		static ParticleSystem* Create(const int particlecount=100, const int duration = 1000,Emitter* emitter =NULL);
	};
}
