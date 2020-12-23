#include "../../Leadwerks.h"

/*
Coding Standards
-All Leadwerks objects are derived from the Object class.
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

namespace Leadwerks
{
	//Declare any other classes we use
	class Attractor;
	
	class Emitter : public Entity//lua
	{
	public:
		int particlecount;
		std::vector<Particle> particle; //Particles stored here
		std::vector<bool> activeList; //keeps track of active particles, used in deletion
		int activeCount;
		int duration;
		int shapeId;
		Surface* surface;
		Vec3 velocity[2]; //0 = min & 1 = max
		Vec3 acceleration; 
		Vec4 colorStep;
		bool everupdated;
		float rotationSpeed[2];
		bool rotationByVelocity;
		int releaseQuantity;
		float interval;
		bool loop;
		bool paused;
		bool autorelease;
        std::vector<Vec2> alphacontrolpoints;
		std::vector<Vec2> scalecontrolpoints;
		Vec2 maxScale;
        Vec4 hsv[2];
		Vec4 particlecolor[2];
		std::vector<Entity*> adjacententities;

		//std::list<Attractor*> attractors;
		int viewMode;
		bool type1; 
		Vec3 manualFacingDirection;
		
		

		//texture animation members
		int rows;
		int columns;
		int cycles;



		//Emitter Members

		Vec3 volume;
		std::list<Emitter*>::iterator link;
		
		//Constructor / destructor
		Emitter();
		virtual ~Emitter();
		

        
		//DLLExport functions
		virtual void SetPosition(const float x, const float y, const float z, const bool global);
		virtual void SetEmissionShape(const int ShapeId=0);//lua
		virtual int GetEmissionShape();//lua
		virtual void SetEmissionVolume(const float x,const float y,const float z);//lua
		virtual Vec3 GetEmissionVolume();//lua
		virtual void SetVelocity(const float x, const float y, const float z, const int index=0);//lua
		virtual Vec3 GetVelocity(const int index=0);//lua
		virtual void SetAcceleration(const float x, const float y, const float z);//lua
		virtual Vec3 GetAcceleration();//lua
		virtual void SetRotationSpeed(const float speed=0.0f);//lua
		virtual float GetRotationSpeed();//lua
		virtual void SetRotationByVelocityMode(const bool setByVel);//lua
		virtual bool GetRotationByVelocityMode();//lua
		virtual void SetReleaseQuantity(const int quantity);//lua
		virtual int GetReleaseQuantity();//lua
		virtual void SetInterval(const float interval);//lua
		virtual float GetInterval();//lua
		virtual void SetLoopMode(const bool Loop, const bool autorelease=false);//lua
		virtual bool GetLoopMode();//lua
		virtual void SetParticleCount(const int ParticleCount);//lua
		virtual int GetParticleCount();//lua
		virtual void SetDuration(const int Duration=3000);//lua
		virtual int GetDuration();//lua
		virtual void Pause();//lua
		virtual void Play();//lua
		virtual bool GetPaused();//lua
		virtual void SetMaxScale(const float x);//lua
		virtual void SetMaxScale(const float x, const float y);//lua
		virtual Vec2 GetMaxScale();//lua

		//Class functions
        virtual float GetAlphaControlPointTime(const int index);
		virtual float GetScaleControlPointTime(const int index);
        virtual float GetAlphaControlPointValue(const int index);
        virtual float GetScaleControlPointValue(const int index);
        virtual int CountAlphaControlPoints(); //lua
		virtual int CountScaleControlPoints();  //lua
        virtual void ClearAlphaControlPoints();//lua
        virtual void ClearScaleControlPoints();   //lua     
		virtual void AddAlphaControlPoint(const float time, const float alpha);//lua
        virtual float GetAlphaAtTime(const float time);//lua
		virtual void AddScaleControlPoint(const float time, const float scale);//lua
        virtual float GetScaleAtTime(const float time);//lua
		virtual void SetFacingDirection(const float x, const float y, const float z);//lua
		virtual Vec3 GetFacingDirection();//lua
		virtual void SetAnimationRows(const int Rows);//lua
		virtual int GetAnimationRows();//lua
		virtual void SetAnimationColumns(const int Columns);//lua
		virtual int GetAnimationColumns();//lua
		virtual void SetAnimationCycles(const int Cycles);//lua
		virtual int GetAnimationCycles();//lua
		virtual void UpdateMatrix();

		virtual void Reset();//lua
		virtual void Update();//lua
	//	virtual void Initialize();
		virtual void Draw(Camera* camera, const bool recursive, const bool drawsorted);
		virtual void UpdateAABB(const int mode);
		virtual void SetEmissionVolume(const Vec3& Volume);//lua
		virtual void SetParticleColor(const float r, const float g, const float b, const float a, const int mode);//lua
		virtual Vec4 GetParticleColor(const int mode);//lua

		//virtual void SetColor(const float r, const float g, const float b, const float a, const int index);
		//virtual Vec4 GetColor(const int index);
		virtual void SetMaterial(Material* newMaterial,const bool recursive = false);

		//for when we have multiple view modes
		//0 = billboard, 1 == X-axis, 2 = Y-axis, 3 = Z-axis 4 = manual-direction
		virtual void SetViewMode(const int viewmode=0);//lua
		virtual int GetViewMode();//lua
		
		//Required entity abstract class functions
		virtual Entity* Copy(const bool recursive = true, const bool callstartfunction=true);
		virtual Entity* Instance(const bool recursive = true, const bool callstartfunction=true);
		virtual void CopyTo(Emitter* emitter,const int mode);
		virtual int GetClass();
		virtual std::string GetClassName();

		//Static functions
		static Emitter* Create(const int particlecount=10, Entity* parent=NULL);//lua
		static Vec3 RGBToHSV(const float r, const float g, const float b);
		static void ForEachEntityInAABBDoCallback(Entity* entity, Object* extra);

		//Private
		//virtual Vec4 CalculateColorStep(const int mode);
		//virtual float CalculateRotationStep();
		//virtual bool CheckShape(const float x, const float y, const float z);

	};
}
