#include "../Leadwerks.h"

namespace Leadwerks
{
	//Declare any other classes we use
	class Camera;
	class Transform;
	class Emitter;
	
	//Object is the base class for all Leadwerks classes
	class Particle : public Object
	{
	public:
		
		//Members
		int index;
		Emitter* emitter;
		float life;
		Vec3 position;
		Vec3 spawnPosition;
		Vec3 spawnOffset;
		Vec3 velocity;
		Vec3 prevposition;
		float radius;
		Vec4 currentColor;
		float currentRotation;
		float pausedOffset;
		Vec3 attractionVelocity;
		bool attractorAlphaOverride;
		float randrotation;
		
		//Constructor / destructor
		Particle();
		virtual ~Particle();
		
		virtual void Reset();
		//virtual void Initialize();
		virtual void FaceCamera(Camera* camera);
		virtual void Update();
		virtual void SetPosition();
		virtual void SetSpawnPosition();
		//virtual void CalculateAlpha();

		
		static Vec3 HSVToRGB(const float h, const float s, const float l);
	};
}
