#pragma once

#include "../../Leadwerks.h"
#include "Classes/NewtonDynamicsSimulation.h"
#include "Classes/NewtonDynamicsBody.h"
#include "Classes/NewtonDynamicsShape.h"
#include "Classes/NewtonDynamicsCharacterController.h"
#include "Classes/NewtonDynamicsJoint.h"
#include "Classes/NewtonDynamicsJoint.h"
#include "Classes/NewtonDynamicsVehicle.h"

namespace Leadwerks
{
	const int NewtonDynamicsMaxThreads = 32;
	
	class Shape;
    class Joint;
    class Body;
	
	class NewtonDynamicsPhysicsDriver : public PhysicsDriver//lua
	{
	public:
        NewtonCollision* spherecastcollision;
		Material* debugmaterial;
		float displacementscale;

		NewtonDynamicsPhysicsDriver();
		virtual ~NewtonDynamicsPhysicsDriver();
		
		virtual Simulation* CreateSimulation();
		//virtual void Update(const float step);
		virtual Body* CreateBody();
        virtual Joint* CreateJoint();
		//virtual Shape* CreateShapeBox(Entity* entity);
		//virtual Shape* CreateShapeSphere(Entity* entity);
		//virtual Shape* CreateShapeCone(Entity* entity);
		//virtual Shape* CreateShapeCylinder(Entity* entity);
		virtual Shape* CreateShape();
		//virtual Shape* CreateShapeConvexHull(Entity* entity, const float tolerance=0.01);
		//virtual Shape* CreateShapeBox(const float posx, const float posy, const float posz, const float rotx, const float roty, const float rotz, const float width, const float height, const float depth);
		//virtual Shape* CreateShapeSphere(const float posx, const float posy, const float posz, const float rotx, const float roty, const float rotz, const float width, const float height, const float depth);
		//virtual Shape* CreateShapePolyMesh(Entity* entity);
		//virtual Shape* CreateShapeConvexHull(Surface* surface);
		//virtual Shape* CreateShapePolyMesh(Surface* surface);
		//virtual Shape* CreateShapePolyMesh(Brush* brush);
        //virtual Shape* CreateShapeCompound(Shape**, const int count);
        virtual CharacterController* CreateCharacterController(Entity* entity, const float radius, const float height, const float stepheight, const float maxslope, const float crouchheight);
		virtual Shape* CreateShapeConvexHull(std::vector<float> points, Shape* existingshape);
        virtual Shape* CreateShapeBox(const float x, const float y, const float z, const float pitch, const float yaw, const float roll, const float width, const float height, const float depth);
        virtual Shape* CreateShapeCylinder(const float x, const float y, const float z, const float pitch, const float yaw, const float roll, const float width, const float height, const float depth);
        virtual Shape* CreateShapeCone(const float x, const float y, const float z, const float pitch, const float yaw, const float roll, const float width, const float height, const float depth);
        virtual Shape* CreateShapeSphere(const float x, const float y, const float z, const float pitch, const float yaw, const float roll, const float width, const float height, const float depth);
		virtual Shape* CreateShapeCapsule(const float x, const float y, const float z, const float pitch, const float yaw, const float roll, const float width, const float height, const float depth);
		virtual Shape* CreateShapeConvexHull(Surface* surf, Shape* existingshape);
        virtual Shape* CreateShapePolyMesh(Surface* surface, Shape* existingshape);
        virtual Shape* CreateShapeConvexDecomposition(Surface* surface);
        virtual Shape* CreateShapeHeightfield(const int resolution, unsigned short* elevationData, const float scale, const float verticalscale, unsigned char* offsetData);
		virtual bool CreateShapeBox(NewtonDynamicsShape* shape, float x, const float y, const float z, const float pitch, const float yaw, const float roll, const float width, const float height, const float depth);
		virtual bool CreateShapeCone(NewtonDynamicsShape* shape, float x, const float y, const float z, const float pitch, const float yaw, const float roll, const float width, const float height, const float depth);
		virtual bool CreateShapeSphere(NewtonDynamicsShape* shape, float x, const float y, const float z, const float pitch, const float yaw, const float roll, const float width, const float height, const float depth);
		virtual bool CreateShapeCylinder(NewtonDynamicsShape* shape, float x, const float y, const float z, const float pitch, const float yaw, const float roll, const float width, const float height, const float depth);

		static NewtonDynamicsPhysicsDriver* Create();//lua
		
		virtual Vehicle* CreateVehicle(Entity* entity);
		
        //virtual Shape* CreateShapeCylinder(const float radius, const float height);
		//virtual Shape* CreateShapeCone(const float radius, const float height);
        //virtual Shape* CreateShapeConvexDecomposition(Entity* entity);
	};
}
