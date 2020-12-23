#pragma once
#include "../../../Leadwerks.h"

namespace Leadwerks
{
	class Body;
	class Shape;
	class AssetReference;
	class Entity;
	class Surface;
	class Collision;
	
	const int MAX_PHYSICS_THREADS = 32;
	
	//Default collision types
	const int COLLISION_NONE = 0;
	const int COLLISION_PROP = 1;
	const int COLLISION_SCENE = 2;
	const int COLLISION_CHARACTER = 3;
	const int COLLISION_TRIGGER = 4;
	const int COLLISION_DEBRIS = 5;
	
	//Collision constants
	const int COLLISION_COLLIDE = 1;
	
	class PhysicsDriver : public Driver//lua
	{
	public:
		//std::map<iVec2,int> collisionresponse;
		std::vector<Collision> collisions[MAX_PHYSICS_THREADS];
		int collisioncount[MAX_PHYSICS_THREADS];
		int collisionresponse[100][100];
		Shape* Picksphere;
		
		PhysicsDriver();
		virtual ~PhysicsDriver();
		
        virtual void RecordCollision(Body* body0, Body* body1, const Vec3& position, const Vec3& normal, const float speed, const int threadindex);
		virtual void ClearCollisionResponses();
		virtual void SetCollisionResponse(const int collisiontype0, const int collisiontype1, const int response);
		virtual int GetCollisionResponse(const int collisiontype0, const int collisiontype1);
		//virtual void Update(const float step = 1.0)=0;
		virtual Simulation* CreateSimulation()=0;
		//virtual Shape* CreateShapeBox(const float posx, const float posy, const float posz, const float rotx, const float roty, const float rotz, const float width, const float height, const float depth)=0;
		//std::map<std::string,AssetReference*> loadedassets;
		virtual Shape* CreateShape()=0;
		virtual Body* CreateBody()=0;
        virtual Joint* CreateJoint()=0;
		/*virtual Shape* CreateShapeBox(Entity* entity)=0;
		virtual Shape* CreateShapeSphere(Entity* entity)=0;
		virtual Shape* CreateShapeCone(Entity* entity)=0;
		virtual Shape* CreateShapeCylinder(Entity* entity)=0;
		virtual Shape* CreateShapePolyMesh(Entity* entity)=0;
		virtual Shape* CreateShapeConvexHull(Entity* entity, const float tolerance=0.01)=0;
		virtual Shape* CreateShapeConvexHull(Surface* surface)=0;
		virtual Shape* CreateShapePolyMesh(Surface* surface)=0;
		virtual Shape* CreateShapePolyMesh(Brush* brush)=0;
        virtual Shape* CreateShapeCompound(Shape**, const int count)=0;
        virtual Shape* CreateShapeCylinder(const float radius, const float height)=0;
		virtual Shape* CreateShapeSphere(const float posx, const float posy, const float posz, const float rotx, const float roty, const float rotz, const float width, const float height, const float depth)=0;
		virtual Shape* CreateShapeCone(const float radius, const float height)=0;
		virtual Shape* CreateShapeConvexDecomposition(Entity* entity)=0;*/
        
        virtual Shape* CreateShapeBox(const float x, const float y, const float z, const float pitch, const float yaw, const float roll, const float width, const float height, const float depth)=0;
        virtual Shape* CreateShapeCylinder(const float x, const float y, const float z, const float pitch, const float yaw, const float roll, const float width, const float height, const float depth)=0;
        virtual Shape* CreateShapeCone(const float x, const float y, const float z, const float pitch, const float yaw, const float roll, const float width, const float height, const float depth)=0;
        virtual Shape* CreateShapeSphere(const float x, const float y, const float z, const float pitch, const float yaw, const float roll, const float width, const float height, const float depth)=0;
        virtual Shape* CreateShapeConvexHull(Surface* surf, Shape* existingshape)=0;
        virtual Shape* CreateShapePolyMesh(Surface* surface, Shape* existingshape)=0;
        virtual Shape* CreateShapeConvexDecomposition(Surface* surface)=0;
		virtual Shape* CreateShapeHeightfield(const int resolution, unsigned short* elevationData, const float scale, const float verticalscale, unsigned char* offsetData) = 0;
		virtual Shape* CreateShapeConvexHull(std::vector<float> points, Shape* existingshape)=0;
		virtual Shape* CreateShapeCapsule(const float x, const float y, const float z, const float pitch, const float yaw, const float roll, const float width, const float height, const float depth) = 0;

		/*virtual Shape* CreateShapeSphere(const float width, const float height, const float depth)=0;
		virtual Shape* CreateShapeCylinder(const float radius, const float height)=0;
		virtual Shape* CreateShapeCone(const float radius, const float height)=0;
		virtual Shape* CreateShapeConvexHull(Surface* surface)=0;
		virtual Shape* CreateShapePolySoup(Surface* surface, const bool optimize)=0;
		virtual Shape* CreateShapeDecomposedMesh(Surface* surface)=0;
		virtual Shape* CreateShapeCompound(std::vector<Shape*>& shapes)=0;*/
		virtual CharacterController* CreateCharacterController(Entity* entity, const float radius, const float height, const float stepheight, const float maxslope, const float crouchheight)=0;
        
		static PhysicsDriver* GetCurrent();//lua
		static void SetCurrent(PhysicsDriver* driver);//lua
		
		virtual Vehicle* CreateVehicle(Entity* entity)=0;
	};
	
	extern PhysicsDriver* _currentPhysicsDriver;
}
