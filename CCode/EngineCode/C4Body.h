 

#ifndef C4Body_h
#define C4Body_h


//# \component	Physics Manager
//# \prefix		PhysicsMgr/


#include "C4Controller.h"


namespace C4
{
	typedef Type	BodyType;


	enum
	{
		kBodyNull				= 0,
		kBodyRigid				= 'RIGD',
		kBodyDeformable			= 'DFRM'
	};


	class Contact;
	class PhysicsController;


	class Body : public GraphElement<Body, Contact>
	{
		friend class PhysicsController;

		private:

			BodyType		bodyType;

		protected:

			Body(BodyType type = kBodyNull);
			Body(const Body& body);

		public:

			~Body();

			BodyType GetBodyType(void) const
			{
				return (bodyType);
			}

			virtual void RecursiveWake(void);
	};


	//# \class	BodyController		Manages a body controlled by the physics simulation.
	//
	//# The $BodyController$ class manages a body controlled by the physics simulation.
	//
	//# \def	class BodyController : public Controller, public Body
	//
	//# \ctor	BodyController(ControllerType type, BodyType body);
	//
	//# The constructor has protected access. The $BodyController$ class can only exist as the base class for another class.
	//
	//# \param	type	The specific controller type of the subclass.
	//# \param	body	The type of physical body managed by the controller.
	//
	//# \desc
	//# The $BodyController$ class is the base class for all physical bodies that are under the control
	//# of the Physics Manager.
	//
	//# \base		Controller/Controller		A $BodyController$ is a specific type of controller.
	//# \privbase	Body						Used internally by the Physics Manager.
	//
	//# \also	$@RigidBodyController@$
	//# \also	$@RopeController@$
	//# \also	$@ClothController@$


	//# \function	BodyController::GetPhysicsController		Returns the physics controller to which a body belongs.
	//
	//# \proto	PhysicsController *GetPhysicsController(void) const;
	//
	//# \desc
	//# The $GetPhysicsController$ function returns the physics controller to which a body belongs.
	//# Every body in a world belongs to the same global physics controller. If there is no physics
	//# controller in the world, then this function returns $nullptr$.
	//
	//# \also	$@PhysicsController@$


	//# \function	BodyController::GetGravityMultiplier	Returns the gravity multiplier for a body.
	//
	//# \proto	const float& GetGravityMultiplier(void) const;
	//
	//# \desc
	//# The $GetGravityMultiplier$ function returns the gravity multiplier for a body.
	//
	//# \also	$@BodyController::SetGravityMultiplier@$
	//# \also	$@PhysicsController::GetGravityAcceleration@$
	//# \also	$@PhysicsController::SetGravityAcceleration@$ 


	//# \function	BodyController::SetGravityMultiplier	Sets the gravity multiplier for a body. 
	//
	//# \proto	void SetGravityMultiplier(float multiplier); 
	//
	//# \param	multiplier		The new gravity multiplier.
	// 
	//# \desc
	//# The $SetGravityMultiplier$ function sets the gravity multiplier for a body to the value 
	//# specified by the $multiplier$ parameter. The gravity multiplier scales the force exerted on a 
	//# body by the global gravity set in the $@PhysicsController@$ object. A multiplier of 1.0 means
	//# that the ordinary gravity force is applied, while values higher or lower than 1.0 mean that
	//# proportionately more or less gravity is applied. If the gravity multiplier is 0.0, then no
	//# gravity is applied to the body at all. 
	//#
	//# The initial value of the gravity multiplier is 1.0.
	//
	//# \also	$@BodyController::GetGravityMultiplier@$
	//# \also	$@PhysicsController::GetGravityAcceleration@$
	//# \also	$@PhysicsController::SetGravityAcceleration@$


	//# \function	BodyController::GetFluidDragMultiplier		Returns the fluid drag multiplier for a body.
	//
	//# \proto	const float& GetFluidDragMultiplier(void) const;
	//
	//# \desc
	//# The $GetFluidDragMultiplier$ function returns the fluid drag multiplier for a body.
	//
	//# \also	$@BodyController::SetFluidDragMultiplier@$
	//# \also	$@BodyController::GetWindDragMultiplier@$
	//# \also	$@BodyController::SetWindDragMultiplier@$
	//# \also	$@FluidForce@$
	//# \also	$@WindForce@$


	//# \function	BodyController::SetFluidDragMultiplier		Sets the fluid drag multiplier for a body.
	//
	//# \proto	void SetFluidDragMultiplier(float multiplier);
	//
	//# \param	multiplier		The new fluid drag multiplier.
	//
	//# \desc
	//# The $SetFluidDragMultiplier$ function sets the fluid drag multiplier for a body to the value
	//# specified by the $multiplier$ parameter. The fluid drag multiplier scales the drag force exerted
	//# on a body by the $@FluidForce@$ class in particular, and any custom force fields involving fluids.
	//# A value of 1.0 means that the ordinary drag force is applied, while values higher or lower than
	//# 1.0 mean that proportionately more or less drag is applied. If the drag multiplier is 0.0, then
	//# no drag is applied to the body at all.
	//#
	//# The initial value of the fluid drag multiplier is 1.0.
	//
	//# \also	$@BodyController::GetFluidDragMultiplier@$
	//# \also	$@BodyController::GetWindDragMultiplier@$
	//# \also	$@BodyController::SetWindDragMultiplier@$
	//# \also	$@FluidForce@$
	//# \also	$@WindForce@$


	//# \function	BodyController::GetWindDragMultiplier		Returns the wind drag multiplier for a body.
	//
	//# \proto	const float& GetWindDragMultiplier(void) const;
	//
	//# \desc
	//# The $GetWindDragMultiplier$ function returns the wind drag multiplier for a body.
	//
	//# \also	$@BodyController::SetWindDragMultiplier@$
	//# \also	$@BodyController::GetFluidDragMultiplier@$
	//# \also	$@BodyController::SetFluidDragMultiplier@$
	//# \also	$@WindForce@$
	//# \also	$@FluidForce@$


	//# \function	BodyController::SetWindDragMultiplier		Sets the wind drag multiplier for a body.
	//
	//# \proto	void SetWindDragMultiplier(float multiplier);
	//
	//# \param	multiplier		The new wind drag multiplier.
	//
	//# \desc
	//# The $SetWindDragMultiplier$ function sets the wind drag multiplier for a body to the value
	//# specified by the $multiplier$ parameter. The wind drag multiplier scales the drag force exerted
	//# on a body by the $@WindForce@$ class in particular, and any custom force fields involving airflow.
	//# A value of 1.0 means that the ordinary drag force is applied, while values higher or lower than
	//# 1.0 mean that proportionately more or less drag is applied. If the drag multiplier is 0.0, then
	//# no drag is applied to the body at all.
	//#
	//# The initial value of the wind drag multiplier is 1.0.
	//
	//# \also	$@BodyController::GetWindDragMultiplier@$
	//# \also	$@BodyController::GetFluidDragMultiplier@$
	//# \also	$@BodyController::SetFluidDragMultiplier@$
	//# \also	$@WindForce@$
	//# \also	$@FluidForce@$


	class BodyController : public Controller, public Body
	{
		private:

			PhysicsController		*physicsController;

			float					gravityMultiplier;
			float					fluidDragMultiplier;
			float					windDragMultiplier;

		protected:

			BodyController(ControllerType type, BodyType body);
			BodyController(const BodyController& bodyController);

		public:

			~BodyController();

			PhysicsController *GetPhysicsController(void) const
			{
				return (physicsController);
			}

			const float& GetGravityMultiplier(void) const
			{
				return (gravityMultiplier);
			}

			void SetGravityMultiplier(float multiplier)
			{
				gravityMultiplier = multiplier;
			}

			const float& GetFluidDragMultiplier(void) const
			{
				return (fluidDragMultiplier);
			}

			void SetFluidDragMultiplier(float multiplier)
			{
				fluidDragMultiplier = multiplier;
			}

			const float& GetWindDragMultiplier(void) const
			{
				return (windDragMultiplier);
			}

			void SetWindDragMultiplier(float multiplier)
			{
				windDragMultiplier = multiplier;
			}

			C4API void Pack(Packer& data, unsigned_int32 packFlags) const override;
			C4API void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			C4API bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			C4API void Preprocess(void) override;
			C4API void Neutralize(void) override;

			C4API bool InstanceExtractable(void) const override;
	};
}


#endif

// ZYUQURM
