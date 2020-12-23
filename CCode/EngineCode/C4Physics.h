 

#ifndef C4Physics_h
#define C4Physics_h


//# \component	Physics Manager
//# \prefix		PhysicsMgr/

//# \import		C4Node.h


#include "C4Shapes.h"
#include "C4Contacts.h"
#include "C4Deformable.h"

#if C4DIAGS

	#include "C4Renderable.h"

#endif


namespace C4
{
	typedef Type	RigidBodyType;


	enum : ControllerType
	{
		kControllerRigidBody			= 'BODY',
		kControllerRagdoll				= 'RGDL',
		kControllerPhysics				= 'PHYS'
	};


	enum : FunctionType
	{
		kFunctionSetGravity				= 'SGRV'
	};


	enum : RigidBodyType
	{
		kRigidBodyGeneric				= 0
	};


	//# \enum	RigidBodyFlags

	enum
	{
		kRigidBodyKeepAwake				= 1 << 0,		//## The rigid body is never put to sleep. For performance reasons, this flag should be set only when absolutely necessary.
		kRigidBodyPartialSleep			= 1 << 1,		//## When the rigid body is put to sleep for the physics simulation, it is not put to sleep as a controller in general.
		kRigidBodyFixedOrientation		= 1 << 2,		//## The rigid body never rotates in the physics simulation, and thus always preserves its original orientation in space.
		kRigidBodyDisabledContact		= 1 << 3,		//## New collision contacts made with the rigid body should be created in the disabled state. This is useful if a collision will always result in the destruction of the rigid body.
		kRigidBodyLocalSimulation		= 1 << 4,		//## The rigid body is only simulated locally on each machine, and the server does not transmit information about the rigid body to the clients.
		kRigidBodyForceFieldInhibit		= 1 << 5,		//## The rigid body is not affected by force fields. The global gravity force is still applied.
		kRigidBodyImmovable				= 1 << 6		//## The rigid body cannot be moved in any way. This flag also implies $kRigidBodyFixedOrientation$.
	};


	enum
	{
		kRigidBodyAsleep				= 1 << 0
	};


	//# \enum	RigidBodyStatus

	enum RigidBodyStatus
	{
		kRigidBodyUnchanged,			//## No change was made to the rigid body or its contacts.
		kRigidBodyContactsBroken,		//## One or more contacts with the rigid body were broken or may have been broken.
		kRigidBodyDestroyed				//## The rigid body was destroyed.
	};


	enum
	{
		kPhysicsCounterRigidBody,
		kPhysicsCounterBuoyancy,
		kPhysicsCounterGeometryIntersection,
		kPhysicsCounterShapeIntersection,
		kPhysicsCounterConstraintSolverIsland,
		kPhysicsCounterDeformableBodyMove,
		kPhysicsCounterDeformableBodyUpdate,
		kPhysicsCounterCount
	};


	C4API extern const char kConnectorKeyPhysics[];


	class ConstraintSolverJob;
	class WaterBlock;
	class Model;

	#if C4DIAGS

		class RigidBodyRenderable;

	#endif 


	struct BodyHitData : ShapeHitData 
	{
		const Shape		*shape; 
	};

 
	//# \class	RigidBodyController		Manages a rigid body in a physics simulation.
	// 
	//# The $RigidBodyController$ class manages a rigid body in a physics simulation. 
	//
	//# \def	class RigidBodyController : public BodyController, public ListElement<RigidBodyController>, public SnapshotSender
	//
	//# \ctor	RigidBodyController(); 
	//# \ctor	RigidBodyController(ControllerType type);
	//
	//# \param	type	The type of controller when the object being constructed is a subclass of $RigidBodyController$.
	//
	//# \desc
	//# The $RigidBodyController$ class manages a rigid body in a physics simulation.
	//
	//# \base		BodyController								A $RigidBodyController$ is a specific type of body controller.
	//# \base		Utilities/ListElement<RigidBodyController>	Used internally by the Physics Manager.
	//# \base		MessageMgr/SnapshotSender					Rigid bodies send periodic updates to client machines.
	//
	//# \also	$@PhysicsController@$
	//# \also	$@Shape@$
	//# \also	$@Joint@$
	//# \also	$@Force@$
	//# \also	$@Field@$


	//# \function	RigidBodyController::GetRigidBodyFlags		Returns the rigid body flags.
	//
	//# \proto	unsigned_int32 GetRigidBodyFlags(void) const;
	//
	//# \desc
	//# The $GetRigidBodyFlags$ function returns the rigid body flags, which can be a combination
	//# (through logical OR) of the following constants.
	//
	//# \table	RigidBodyFlags
	//
	//# \also	$@RigidBodyController::SetRigidBodyFlags@$


	//# \function	RigidBodyController::SetRigidBodyFlags		Sets the rigid body flags.
	//
	//# \proto	void SetRigidBodyFlags(unsigned_int32 flags);
	//
	//# \param	flags	The new rigid body flags. See below for possible values.
	//
	//# \desc
	//# The $SetRigidBodyFlags$ function sets the rigid body flags. The $flags$ parameter can be a combination
	//# (through logical OR) of the following constants.
	//
	//# \table	RigidBodyFlags
	//
	//# The initial value of the rigid body flags is 0.
	//
	//# \also	$@RigidBodyController::GetRigidBodyFlags@$


	//# \function	RigidBodyController::GetRestitutionCoefficient		Returns the restitution coefficient for a rigid body.
	//
	//# \proto	float GetRestitutionCoefficient(void) const;
	//
	//# \desc
	//# The $GetRestitutionCoefficient$ function returns the restitution coefficient for a rigid body.
	//
	//# \also	$@RigidBodyController::SetRestitutionCoefficient@$


	//# \function	RigidBodyController::SetRestitutionCoefficient		Sets the restitution coefficient for a rigid body.
	//
	//# \proto	void SetRestitutionCoefficient(float restitution);
	//
	//# \param	restitution		The new restitution coefficient.
	//
	//# \desc
	//# The $SetRestitutionCoefficient$ function sets the restitution coefficient for a rigid body to the
	//# value specified by the $restitution$ parameter. This value determines how much a rigid body bounces
	//# when it collides with another rigid body or static geometry, and it should be in the range [0.0,&nbsp;1.0].
	//# A value of 0.0 means that all of the rigid body's energy is lost in a collision, and a value of 1.0
	//# means that collisions produce a completely elastic response.
	//#
	//# The initial value of the restitution coefficient is 0.0.
	//
	//# \also	$@RigidBodyController::GetRestitutionCoefficient@$


	//# \function	RigidBodyController::GetFrictionCoefficient		Returns the friction coefficient for a rigid body.
	//
	//# \proto	float GetFrictionCoefficient(void) const;
	//
	//# \desc
	//# The $GetFrictionCoefficient$ function returns the friction coefficient for a rigid body.
	//
	//# \also	$@RigidBodyController::SetFrictionCoefficient@$
	//# \also	$@RigidBodyController::GetSpinFrictionMultiplier@$
	//# \also	$@RigidBodyController::SetSpinFrictionMultiplier@$
	//# \also	$@RigidBodyController::GetRollingResistance@$
	//# \also	$@RigidBodyController::SetRollingResistance@$


	//# \function	RigidBodyController::SetFrictionCoefficient		Sets the friction coefficient for a rigid body.
	//
	//# \proto	void SetFrictionCoefficient(float friction);
	//
	//# \param	friction		The new friction coefficient.
	//
	//# \desc
	//# The $SetFrictionCoefficient$ function sets the friction coefficient for a rigid body to the value
	//# specified by the $friction$ parameter. This value determines how much frictional force is exerted
	//# on a rigid body when it is in contact with another rigid body or static geometry. Typical values
	//# for the $friction$ parameter are in the range [0.0,&nbsp;0.5].
	//#
	//# The initial value of the friction coefficient is 0.25.
	//
	//# \also	$@RigidBodyController::GetFrictionCoefficient@$
	//# \also	$@RigidBodyController::GetSpinFrictionMultiplier@$
	//# \also	$@RigidBodyController::SetSpinFrictionMultiplier@$
	//# \also	$@RigidBodyController::GetRollingResistance@$
	//# \also	$@RigidBodyController::SetRollingResistance@$


	//# \function	RigidBodyController::GetSpinFrictionMultiplier		Returns the spin friction multiplier for a rigid body.
	//
	//# \proto	float GetSpinFrictionMultiplier(void) const;
	//
	//# \desc
	//# The $GetSpinFrictionMultiplier$ function returns the spin friction multiplier for a rigid body.
	//
	//# \also	$@RigidBodyController::SetSpinFrictionMultiplier@$
	//# \also	$@RigidBodyController::GetFrictionCoefficient@$
	//# \also	$@RigidBodyController::SetFrictionCoefficient@$
	//# \also	$@RigidBodyController::GetRollingResistance@$
	//# \also	$@RigidBodyController::SetRollingResistance@$


	//# \function	RigidBodyController::SetSpinFrictionMultiplier		Sets the spin friction multiplier for a rigid body.
	//
	//# \proto	void SetSpinFrictionMultiplier(float spin);
	//
	//# \param	spin		The new spin friction multiplier.
	//
	//# \desc
	//# The $SetSpinFrictionMultiplier$ function sets the spin friction multiplier for a rigid body to the
	//# value specified by the $spin$ parameter. This value multiplies the ordinary friction coefficient and,
	//# if nonzero, applies a frictional force to a rigid body that is generally spinning on a single contact.
	//# Since such a contact point would have very little tangential velocity, ordinary friction would do little
	//# to slow the motion, so the spin friction must be applied to oppose the angular velocity. Typical values
	//# for the $spin$ parameter are in the range [0.0,&nbsp;0.5].
	//#
	//# The initial value of the spin friction multiplier is 0.0.
	//
	//# \also	$@RigidBodyController::GetSpinFrictionMultiplier@$
	//# \also	$@RigidBodyController::GetFrictionCoefficient@$
	//# \also	$@RigidBodyController::SetFrictionCoefficient@$
	//# \also	$@RigidBodyController::GetRollingResistance@$
	//# \also	$@RigidBodyController::SetRollingResistance@$


	//# \function	RigidBodyController::GetRollingResistance		Returns the rolling resistance for a rigid body.
	//
	//# \proto	float GetRollingResistance(void) const;
	//
	//# \desc
	//# The $GetRollingResistance$ function returns the rolling resistance for a rigid body.
	//
	//# \also	$@RigidBodyController::SetRollingResistance@$
	//# \also	$@RigidBodyController::GetFrictionCoefficient@$
	//# \also	$@RigidBodyController::SetFrictionCoefficient@$
	//# \also	$@RigidBodyController::GetSpinFrictionMultiplier@$
	//# \also	$@RigidBodyController::SetSpinFrictionMultiplier@$


	//# \function	RigidBodyController::SetRollingResistance		Sets the rolling resistance for a rigid body.
	//
	//# \proto	void SetSpinFrictionMultiplier(float resistance);
	//
	//# \param	resistance		The new rolling resistance.
	//
	//# \desc
	//# The $SetRollingResistance$ function sets the rolling resistance for a rigid body to the value
	//# specified by the $resistance$ parameter. When nonzero, the rolling resistance generates a torque
	//# that opposes the rolling motion of a rigid body. Typical values for the $resistance$ parameter are
	//# in the range [0.0,&nbsp;0.05].
	//#
	//# The initial value of the rolling resistance is 0.0.
	//
	//# \also	$@RigidBodyController::GetRollingResistance@$
	//# \also	$@RigidBodyController::GetFrictionCoefficient@$
	//# \also	$@RigidBodyController::SetFrictionCoefficient@$
	//# \also	$@RigidBodyController::GetSpinFrictionMultiplier@$
	//# \also	$@RigidBodyController::SetSpinFrictionMultiplier@$


	//# \function	RigidBodyController::GetSleepBoxSize		Returns the sleep box size for a rigid body.
	//
	//# \proto	float GetSleepBoxSize(void) const;
	//
	//# \desc
	//# The $GetSleepBoxSize$ function returns the size of the sleep boxes used to determine when a rigid body
	//# can be put to sleep.
	//
	//# \also	$@RigidBodyController::SetSleepBoxSize@$


	//# \function	RigidBodyController::SetSleepBoxSize		Sets the sleep box size for a rigid body.
	//
	//# \proto	float SetSleepBoxSize(float size) const;
	//
	//# \param	size	The new size of the sleep box.
	//
	//# \desc
	//# The $SetSleepBoxSize$ function sets the size of the sleep boxes used to determine when a rigid body
	//# can be put to sleep. The initial value is given by the $kRigidBodySleepBoxSize$ constant. Larger values
	//# increase the tendancy for a rigid body to be put to sleep when it experiences only small motions.
	//
	//# \also	$@RigidBodyController::GetSleepBoxSize@$


	//# \div
	//# \function	RigidBodyController::GetCollisionKind		Returns the collision kind for a rigid body.
	//
	//# \proto	unsigned_int32 GetCollisionKind(void) const;
	//
	//# \desc
	//# The $GetCollisionKind$ function returns the collision kind for a rigid body.
	//#
	//# See the $@RigidBodyController::SetCollisionKind@$ function for an explanation of collision kinds.
	//
	//# \also	$@RigidBodyController::SetCollisionKind@$
	//# \also	$@RigidBodyController::GetCollisionExclusionMask@$
	//# \also	$@RigidBodyController::SetCollisionExclusionMask@$
	//# \also	$@RigidBodyController::ValidRigidBodyCollision@$
	//# \also	$@RigidBodyController::ValidGeometryCollision@$


	//# \function	RigidBodyController::SetCollisionKind		Sets the collision kind for a rigid body.
	//
	//# \proto	void SetCollisionKind(unsigned_int32 kind);
	//
	//# \param	kind	The new collision kind.
	//
	//# \desc
	//# The $SetCollisionKind$ function sets the collision kind for a rigid body. The collision kind is a 32-bit
	//# value that typically has a single bit set to 1, and the rest set to 0. However, values with more than one bit
	//# set are allowed. The following collision kinds are defined by the engine.
	//
	//# \table	CollisionKind
	//
	//# User-defined collision kinds should always be single bit values greater than or equal to $kCollisionBaseKind$.
	//# New collision kinds would typically be defined by setting the first one equal to $kCollisionBaseKind$, the second
	//# one equal to $kCollisionBaseKind << 1$, the third one equal to $kCollisionBaseKind << 2$, and so on.
	//#
	//# The initial collision kind for a rigid body is $kCollisionRigidBody$.
	//
	//# \also	$@RigidBodyController::GetCollisionKind@$
	//# \also	$@RigidBodyController::GetCollisionExclusionMask@$
	//# \also	$@RigidBodyController::SetCollisionExclusionMask@$
	//# \also	$@RigidBodyController::ValidRigidBodyCollision@$
	//# \also	$@RigidBodyController::ValidGeometryCollision@$


	//# \function	RigidBodyController::GetCollisionExclusionMask		Returns the collision exclusion mask for a rigid body.
	//
	//# \proto	unsigned_int32 GetCollisionExclusionMask(void) const;
	//
	//# \desc
	//# The $GetCollisionExclusionMask$ function returns the collision exclusion mask for a rigid body.
	//#
	//# See the $@RigidBodyController::SetCollisionExclusionMask@$ function for an explanation of collision exclusion masks.
	//
	//# \also	$@RigidBodyController::SetCollisionExclusionMask@$
	//# \also	$@RigidBodyController::GetCollisionKind@$
	//# \also	$@RigidBodyController::SetCollisionKind@$
	//# \also	$@RigidBodyController::ValidRigidBodyCollision@$
	//# \also	$@RigidBodyController::ValidGeometryCollision@$


	//# \function	RigidBodyController::SetCollisionExclusionMask		Sets the collision exclusion mask for a rigid body.
	//
	//# \proto	void GetCollisionExclusionMask(unsigned_int32 mask);
	//
	//# \param	mask	The new collision exclusion mask.
	//
	//# \desc
	//# The $SetCollisionExclusionMask$ function sets the collision exclusion mask for a rigid body to the value specified
	//# by the $mask$ parameter. The exclusion mask can be any 32-bit value that is a combination (through logical OR) of
	//# collision kind values. For any bits that are set, the default $@RigidBodyController::ValidRigidBodyCollision@$ function
	//# does not allow collisions with any rigid bodies having the corresponding collision kind.
	//#
	//# The initial collision exclusion mask is 0, meaning that all collisions are allowed.
	//
	//# \also	$@RigidBodyController::GetCollisionExclusionMask@$
	//# \also	$@RigidBodyController::GetCollisionKind@$
	//# \also	$@RigidBodyController::SetCollisionKind@$
	//# \also	$@RigidBodyController::ValidRigidBodyCollision@$
	//# \also	$@RigidBodyController::ValidGeometryCollision@$


	//# \function	RigidBodyController::GetBodyVolume		Returns the volume of a rigid body.
	//
	//# \proto	float GetBodyVolume(void) const;
	//
	//# \desc
	//# The $GetBodyVolume$ function returns the total volume occupied by all of the shapes composing a
	//# rigid body in cubic meters (m<sup>3</sup>). If any shapes overlap, then the full volume of each shape is included
	//# in the total volume for the rigid body.
	//
	//# \also	$@RigidBodyController::GetBodyMass@$


	//# \function	RigidBodyController::GetBodyMass		Returns the mass of a rigid body.
	//
	//# \proto	float GetBodyMass(void) const;
	//
	//# \desc
	//# The $GetBodyMass$ function returns the total mass of the shapes composing a rigid body in metric tons
	//# (i.e., in thousands of kilograms). If any shapes overlap, then the full mass of each shape is included
	//# in the total mass for the rigid body.
	//
	//# \also	$@RigidBodyController::GetBodyVolume@$
	//# \also	$@ShapeObject::GetShapeDensity@$
	//# \also	$@ShapeObject::SetShapeDensity@$


	//# \function	RigidBodyController::GetBodyCenterOfMass	Returns the body-space center of mass of a rigid body.
	//
	//# \proto	const Point3D& GetBodyCenterOfMass(void) const;
	//
	//# \desc
	//# The $GetBodyCenterOfMass$ function returns the center of mass of a rigid body in the local coordinate space
	//# for the node to which the rigid body controller is assigned. The body-space center of mass is a constant.
	//
	//# \also	$@RigidBodyController::GetWorldCenterOfMass@$
	//# \also	$@RigidBodyController::GetBodyVolume@$
	//# \also	$@RigidBodyController::GetBodyMass@$


	//# \function	RigidBodyController::GetWorldCenterOfMass	Returns the world-space center of mass of a rigid body.
	//
	//# \proto	const Point3D& GetWorldCenterOfMass(void) const;
	//
	//# \desc
	//# The $GetWorldCenterOfMass$ function returns the center of mass of a rigid body in world-space coordinates.
	//
	//# \also	$@RigidBodyController::GetBodyCenterOfMass@$
	//# \also	$@RigidBodyController::GetBodyVolume@$
	//# \also	$@RigidBodyController::GetBodyMass@$


	//# \div
	//# \function	RigidBodyController::GetLinearVelocity		Returns the current linear velocity of a rigid body.
	//
	//# \proto	const Vector3D& GetLinearVelocity(void) const;
	//
	//# \desc
	//# The $GetLinearVelocity$ function returns the current linear velocity for a rigid body in world-space coordinates,
	//# measured in meters per second (m/s).
	//
	//# \also	$@RigidBodyController::SetLinearVelocity@$
	//# \also	$@RigidBodyController::GetAngularVelocity@$
	//# \also	$@RigidBodyController::SetAngularVelocity@$
	//# \also	$@RigidBodyController::GetOriginalLinearVelocity@$
	//# \also	$@RigidBodyController::GetOriginalAngularVelocity@$
	//# \also	$@RigidBodyController::CalculateWorldPositionVelocity@$


	//# \function	RigidBodyController::SetLinearVelocity		Sets the current linear velocity of a rigid body.
	//
	//# \proto	void SetLinearVelocity(const Vector3D& velocity);
	//
	//# \param	velocity	The new linear velocity, in world-space coordinates.
	//
	//# \desc
	//# The $SetLinearVelocity$ function sets the current linear velocity for a rigid body to that specified by the
	//# $velocity$ parameter. The velocity vector is specified in world-space coordinates, and it is
	//# measured in meters per second (m/s).
	//
	//# \also	$@RigidBodyController::GetLinearVelocity@$
	//# \also	$@RigidBodyController::GetAngularVelocity@$
	//# \also	$@RigidBodyController::SetAngularVelocity@$
	//# \also	$@RigidBodyController::GetOriginalLinearVelocity@$
	//# \also	$@RigidBodyController::GetOriginalAngularVelocity@$


	//# \function	RigidBodyController::GetAngularVelocity		Returns the current angular velocity of a rigid body.
	//
	//# \proto	const Antivector3D& GetAngularVelocity(void) const;
	//
	//# \desc
	//# The $GetAngularVelocity$ function returns the current angular velocity for a rigid body in world-space coordinates,
	//# measured in radians per second (rad/s).
	//
	//# \also	$@RigidBodyController::SetAngularVelocity@$
	//# \also	$@RigidBodyController::GetLinearVelocity@$
	//# \also	$@RigidBodyController::SetLinearVelocity@$
	//# \also	$@RigidBodyController::GetOriginalLinearVelocity@$
	//# \also	$@RigidBodyController::GetOriginalAngularVelocity@$
	//# \also	$@RigidBodyController::CalculateWorldPositionVelocity@$


	//# \function	RigidBodyController::SetAngularVelocity		Sets the current angular velocity of a rigid body.
	//
	//# \proto	void SetAngularVelocity(const Antivector3D& velocity);
	//
	//# \param	velocity	The new angular velocity, in world-space coordinates.
	//
	//# \desc
	//# The $SetAngularVelocity$ function sets the current angular velocity for a rigid body to that specified by the
	//# $velocity$ parameter. The velocity vector is specified in world-space coordinates, and it is
	//# measured in radians per second (rad/s).
	//
	//# \also	$@RigidBodyController::GetAngularVelocity@$
	//# \also	$@RigidBodyController::GetLinearVelocity@$
	//# \also	$@RigidBodyController::SetLinearVelocity@$
	//# \also	$@RigidBodyController::GetOriginalLinearVelocity@$
	//# \also	$@RigidBodyController::GetOriginalAngularVelocity@$


	//# \function	RigidBodyController::GetOriginalLinearVelocity		Returns the linear velocity that a rigid body had at the beginning of the simulation step.
	//
	//# \proto	const Vector3D& GetOriginalLinearVelocity(void) const;
	//
	//# \desc
	//# The $GetOriginalLinearVelocity$ function returns the linear velocity that a rigid body had at the beginning of the most recent simulation step,
	//# before any contact forces were applied. The velocity is returned in world-space coordinates, and it's measured in meters per second (m/s).
	//
	//# \also	$@RigidBodyController::GetOriginalAngularVelocity@$
	//# \also	$@RigidBodyController::GetLinearVelocity@$
	//# \also	$@RigidBodyController::GetAngularVelocity@$


	//# \function	RigidBodyController::GetOriginalAngularVelocity		Returns the angular velocity that a rigid body had at the beginning of the simulation step.
	//
	//# \proto	const Antivector3D& GetOriginalAngularVelocity(void) const;
	//
	//# \desc
	//# The $GetOriginalAngularVelocity$ function returns the angular velocity that a rigid body had at the beginning of the most recent simulation step,
	//# before any contact forces were applied. The velocity is returned in world-space coordinates, and it's measured in radians per second (rad/s).
	//
	//# \also	$@RigidBodyController::GetOriginalLinearVelocity@$
	//# \also	$@RigidBodyController::GetLinearVelocity@$
	//# \also	$@RigidBodyController::GetAngularVelocity@$


	//# \function	RigidBodyController::CalculateWorldPositionVelocity		Returns the velocity of an arbitrary point due to the motion of a rigid body.
	//
	//# \proto	Vector3D CalculateWorldPositionVelocity(const Point3D& position) const;
	//
	//# \param	position	A position in world-space coordinates.
	//
	//# \desc
	//# The $CalculateWorldPositionVelocity$ function returns the velocity that the point specified by the $position$ parameter would have
	//# if it represented a static position on the surface of a rigid body. The calculated velocity accounts for the linear velocity of the rigid
	//# body's center of mass and the additional tangential velocity of the surface point due to the angular velocity of the rigid body.
	//# The velocity is returned in world-space coordinates, and it's measured in meters per second (m/s).
	//
	//# \also	$@RigidBodyController::GetLinearVelocity@$
	//# \also	$@RigidBodyController::GetAngularVelocity@$


	//# \div
	//# \function	RigidBodyController::GetExternalForce		Returns the external force acting on a rigid body.
	//
	//# \proto	const Vector3D& GetExternalForce(void) const;
	//
	//# \desc
	//# The $GetExternalForce$ function returns the external force, in world-space coordinates, acting on a rigid body's center of mass.
	//# The force is measured in kilonewtons (kN).
	//
	//# \also	$@RigidBodyController::SetExternalForce@$
	//# \also	$@RigidBodyController::GetExternalTorque@$
	//# \also	$@RigidBodyController::SetExternalTorque@$
	//# \also	$@RigidBodyController::ApplyImpulse@$


	//# \function	RigidBodyController::SetExternalForce		Sets the external force acting on a rigid body.
	//
	//# \proto	void SetExternalForce(const Vector2D& force);
	//# \proto	void SetExternalForce(const Vector3D& force);
	//
	//# \param	force		The new external force, in world-space coordinates.
	//
	//# \desc
	//# The $SetExternalForce$ function sets the external force, in world-space coordinates, acting on a rigid body's center of mass
	//# to the force specified by the $force$ parameter. The force is measured in kilonewtons (kN). If a 2D vector is specified, then
	//# the <i>z</i> coordinate of the force is zero.
	//#
	//# Once an external force is established, it is applied continuously until it is removed. The $@RigidBodyController::ApplyImpulse@$
	//# function can be used to exert a one-time instantaneous force on a rigid body.
	//#
	//# The initial value of the external force is (0,0,0).
	//
	//# \also	$@RigidBodyController::SetExternalForce@$
	//# \also	$@RigidBodyController::GetExternalTorque@$
	//# \also	$@RigidBodyController::SetExternalTorque@$
	//# \also	$@RigidBodyController::ApplyImpulse@$


	//# \function	RigidBodyController::GetExternalTorque		Returns the external torque acting on a rigid body.
	//
	//# \proto	const Antivector3D& GetExternalTorque(void) const;
	//
	//# \desc
	//# The $GetExternalTorque$ function returns the external torque, in world-space coordinates, acting on a rigid body about its center of mass.
	//# The torque is measured in kilonewtons times meters (kN&middot;m).
	//
	//# \also	$@RigidBodyController::SetExternalTorque@$
	//# \also	$@RigidBodyController::GetExternalForce@$
	//# \also	$@RigidBodyController::SetExternalForce@$
	//# \also	$@RigidBodyController::ApplyImpulse@$


	//# \function	RigidBodyController::SetExternalTorque		Sets the external torque acting on a rigid body.
	//
	//# \proto	void SetExternalTorque(const Antivector3D& torque);
	//
	//# \param	torque		The new external torque, in world-space coordinates.
	//
	//# \desc
	//# The $SetExternalTorque$ function sets the external torque, in world-space coordinates, acting on a rigid body about its center of mass
	//# to the torque specified by the $torque$ parameter. The torque is measured in kilonewtons times meters (kN&middot;m).
	//#
	//# Once an external torque is established, it is applied continuously until it is removed. The $@RigidBodyController::ApplyImpulse@$
	//# function can be used to exert a one-time instantaneous torque on a rigid body.
	//#
	//# The initial value of the external torque is (0,0,0).
	//
	//# \also	$@RigidBodyController::GetExternalTorque@$
	//# \also	$@RigidBodyController::GetExternalForce@$
	//# \also	$@RigidBodyController::SetExternalForce@$
	//# \also	$@RigidBodyController::ApplyImpulse@$


	//# \function	RigidBodyController::GetExternalLinearResistance		Returns the external resistive force acting on the linear velocity of a rigid body.
	//
	//# \proto	const Vector3D& GetExternalLinearResistance(void) const;
	//
	//# \desc
	//# The $GetExternalLinearResistance$ function returns the external resistive force, in world-space coordinates, acting on the linear
	//# velocity of a rigid body. The resistive force is measured in kilonewtons per meter-per-second (kN&middot;s&middot;m<sup>&minus;1</sup>).
	//
	//# \also	$@RigidBodyController::SetExternalLinearResistance@$
	//# \also	$@RigidBodyController::GetExternalForce@$
	//# \also	$@RigidBodyController::SetExternalForce@$


	//# \function	RigidBodyController::SetExternalLinearResistance		Sets the external resistive force acting on the linear velocity of a rigid body.
	//
	//# \proto	void SetExternalLinearResistance(const Vector2D& resistance);
	//# \proto	void SetExternalLinearResistance(const Vector3D& resistance);
	//
	//# \param	resistance		The new external resistive force, in world-space coordinates.
	//
	//# \desc
	//# The $SetExternalLinearResistance$ function sets the external resistive force, in world-space coordinates, acting on the linear
	//# velocity of a rigid body to the resistance specified by the $resistance$ parameter. The resistive force is measured in kilonewtons
	//# per meter-per-second (kN&middot;s&middot;m<sup>&minus;1</sup>). If a 2D vector is specified, then the <i>z</i> coordinate of the resistance is zero.
	//#
	//# Once an external resistance is established, it is applied continuously until it is removed. The force due to the resistance is calculated
	//# by multiplying the current linear velocity by the external linear resistance componentwise. This force is then subtracted from the total
	//# force applied to a rigid body.
	//#
	//# The initial value of the external resistive force is (0,0,0).
	//
	//# \also	$@RigidBodyController::GetExternalLinearResistance@$
	//# \also	$@RigidBodyController::GetExternalForce@$
	//# \also	$@RigidBodyController::SetExternalForce@$


	//# \function	RigidBodyController::ApplyImpulse		Applies an impulse to a rigid body.
	//
	//# \proto	void ApplyImpulse(const Vector3D& impulse);
	//# \proto	void ApplyImpulse(const Vector3D& impulse, const Point3D& position);
	//
	//# \param	impulse		The impulse to apply, in body-space coordinates.
	//# \param	position	The body-space position to which the impulse is applied.
	//
	//# \desc
	//# The $ApplyImpulse$ function applies a one-time instantaneous impulse to a rigid body. The impulse is specified by the $impulse$
	//# parameter and is measured in kilonewtons times seconds (kN&middot;s). This function causes a force to be applied to the rigid body's
	//# center of mass during the next simulation step. A torque is also applied to the rigid body when the difference between the $position$
	//# parameter and the rigid body's center of mass is not parallel to the direction of the impulse.
	//#
	//# The $impulse$ and $position$ parameters are specified in the body-space coordinates for the target node of the rigid body controller.
	//# If the $position$ parameter is omitted, then the impulse is applied to the rigid body's center of mass.
	//#
	//# If the $ApplyImpulse$ function is called multiple times for the same rigid body between simulation steps, then the forces and
	//# torques that get applied are accumulated.
	//
	//# \also	$@RigidBodyController::GetExternalForce@$
	//# \also	$@RigidBodyController::SetExternalForce@$
	//# \also	$@RigidBodyController::GetExternalTorque@$
	//# \also	$@RigidBodyController::SetExternalTorque@$


	//# \function	RigidBodyController::SetRigidBodyTransform		Sets the node transform for a rigid body.
	//
	//# \proto	void SetRigidBodyTransform(const Transform4D& transform);
	//
	//# \param	transform		The new node transform for the rigid body.
	//
	//# \desc
	//# The $SetRigidBodyTransform$ function sets the node transform for a rigid body. This function should be called
	//# instead of the $@WorldMgr/Node::SetNodeTransform@$ function to change the node transform for any node under the control
	//# of a $RigidBodyController$.
	//#
	//# The upper-left 3&nbsp;&times;&nbsp;3 portion of the matrix specified by the $transform$ parameter must be right-handed and orthogonal.
	//
	//# \warning
	//# The $SetRigidBodyTransform$ function should not ordinarily be called to set the transform of a rigid body except
	//# in special circumstances, such as to set the initial transform of a newly created rigid body or to reinitialize the
	//# transform of a rigid body after it has been teleported to a new location. Otherwise, calling the $SetRigidBodyTransform$
	//# function can interfere with the normal processing of the Physics Manager and cause graphical features such as motion
	//# blur not to render correctly for the rigid body.
	//
	//# \also	$@RigidBodyController::SetRigidBodyPosition@$
	//# \also	$@RigidBodyController::SetRigidBodyMatrix3D@$


	//# \function	RigidBodyController::SetRigidBodyMatrix3D		Sets the node rotation matrix for a rigid body.
	//
	//# \proto	void SetRigidBodyMatrix3D(const Matrix3D& matrix);
	//
	//# \param	matrix		The new node rotation matrix for the rigid body.
	//
	//# \desc
	//# The $SetRigidBodyMatrix3D$ function sets the node rotation matrix for a rigid body. This function should be called
	//# instead of the $@WorldMgr/Node::SetNodeMatrix3D@$ function to change the node rotation matrix for any node under the control
	//# of a $RigidBodyController$.
	//#
	//# The matrix specified by the $matrix$ parameter must be right-handed and orthogonal.
	//
	//# \warning
	//# The $SetRigidBodyMatrix3D$ function should not ordinarily be called to set the transform of a rigid body except
	//# in special circumstances, such as to set the initial transform of a newly created rigid body or to reinitialize the
	//# transform of a rigid body after it has been teleported to a new location. Otherwise, calling the $SetRigidBodyMatrix3D$
	//# function can interfere with the normal processing of the Physics Manager and cause graphical features such as motion
	//# blur not to render correctly for the rigid body.
	//
	//# \also	$@RigidBodyController::SetRigidBodyTransform@$
	//# \also	$@RigidBodyController::SetRigidBodyPosition@$


	//# \function	RigidBodyController::SetRigidBodyPosition		Sets the node position for a rigid body.
	//
	//# \proto	void SetRigidBodyPosition(const Point3D& position);
	//
	//# \param	position		The new node position for the rigid body.
	//
	//# \desc
	//# The $SetRigidBodyPosition$ function sets the node position for a rigid body. This function should be called
	//# instead of the $@WorldMgr/Node::SetNodePosition@$ function to change the node position for any node under the control
	//# of a $RigidBodyController$.
	//
	//# \warning
	//# The $SetRigidBodyPosition$ function should not ordinarily be called to set the transform of a rigid body except
	//# in special circumstances, such as to set the initial transform of a newly created rigid body or to reinitialize the
	//# transform of a rigid body after it has been teleported to a new location. Otherwise, calling the $SetRigidBodyPosition$
	//# function can interfere with the normal processing of the Physics Manager and cause graphical features such as motion
	//# blur not to render correctly for the rigid body.
	//
	//# \also	$@RigidBodyController::SetRigidBodyTransform@$
	//# \also	$@RigidBodyController::SetRigidBodyMatrix3D@$


	//# \div
	//# \function	RigidBodyController::CalculateSubmergedVolume		Calculates the submerged volume of a rigid body.
	//
	//# \proto	float CalculateSubmergedVolume(const Antivector4D& plane, Point3D *submergedCentroid) const;
	//
	//# \param	plane				The planar boundary, in body-space coordinates.
	//# \param	submergedCentroid	A pointer to the location where the submerged centroid is returned.
	//
	//# \desc
	//# The $CalculateSubmergedVolume$ function determines what portion of a rigid body lies on the positive side of
	//# the plane specified by the $plane$ parameter. For each shape composing the rigid body, the exact volume of the
	//# part lying on the positive side of the plane is calculated, and the results are summed to produce the return
	//# value for this function. The geometric centroid of the submerged volume of each shape is also calculated, and the
	//# volume-weighted sum of these centroids, in body-space coordinates, is returned through the $submergedCentroid$ parameter.


	//# \div
	//# \function	RigidBodyController::ValidRigidBodyCollision		Returns a boolean value indicating whether a collision with another rigid body would be valid.
	//
	//# \proto	virtual bool ValidRigidBodyCollision(const RigidBodyController *body) const;
	//
	//# \param	body	A pointer to another rigid body with which a collision might occur.
	//
	//# \desc
	//# The $ValidRigidBodyCollision$ function returns a boolean value indicating whether a collision between the rigid body for
	//# which it is called and the rigid body specified by the $body$ parameter should be considered valid. This function
	//# can be overridden in a subclass of $RigidBodyController$ to implement arbitrary collision masking. The default
	//# implementation in the base class checks whether the collision kind of the rigid body for which $ValidRigidBodyCollision$
	//# is called is excluded by the collision exclusion mask for the rigid body specified by the $body$ parameter.
	//# The function returns $true$ if a collision between the two bodies is allowed, and it returns $false$ if such a
	//# collision should never occur.
	//#
	//# When a collision might occur between two rigid bodies, the $ValidRigidBodyCollision$ function is called twice, one time
	//# for each rigid body with the other rigid body passed as the $body$ parameter.
	//
	//# \also	$@RigidBodyController::ValidGeometryCollision@$
	//# \also	$@RigidBodyController::GetCollisionKind@$
	//# \also	$@RigidBodyController::SetCollisionKind@$
	//# \also	$@RigidBodyController::GetCollisionExclusionMask@$
	//# \also	$@RigidBodyController::SetCollisionExclusionMask@$


	//# \function	RigidBodyController::ValidGeometryCollision		Returns a boolean value indicating whether a collision with a geometry node would be valid.
	//
	//# \proto	virtual bool ValidGeometryCollision(const Geometry *geometry) const;
	//
	//# \param	geometry	A pointer to a geometry node with which a collision might occur.
	//
	//# \desc
	//# The $ValidGeometryCollision$ function returns a boolean value indicating whether a collision between the rigid body for
	//# which it is called and the geometry node specified by the $geometry$ parameter should be considered valid. This function
	//# can be overridden in a subclass of $RigidBodyController$ to implement arbitrary collision masking. The default
	//# implementation in the base class checks whether the collision kind of the rigid body for which $ValidRigidBodyCollision$
	//# is called is excluded by the collision exclusion mask for the geometry object attached to the node specified by the
	//# $geometry$ parameter. The function returns $true$ if a collision with the geometry node is allowed, and it returns $false$
	//# if such a collision should never occur.
	//
	//# \also	$@RigidBodyController::ValidRigidBodyCollision@$
	//# \also	$@RigidBodyController::GetCollisionKind@$
	//# \also	$@RigidBodyController::SetCollisionKind@$
	//# \also	$@RigidBodyController::GetCollisionExclusionMask@$
	//# \also	$@RigidBodyController::SetCollisionExclusionMask@$
	//# \also	$@WorldMgr/GeometryObject::GetCollisionExclusionMask@$
	//# \also	$@WorldMgr/GeometryObject::SetCollisionExclusionMask@$


	//# \function	RigidBodyController::HandleNewRigidBodyContact		Called when a new contact is made with another rigid body.
	//
	//# \proto	virtual RigidBodyStatus HandleNewRigidBodyContact(const RigidBodyContact *contact, RigidBodyController *contactBody);
	//
	//# \param	contact			The new contact.
	//# \param	contactBody		The rigid body with which contact was made.
	//
	//# \desc
	//# The $HandleNewRigidBodyContact$ function is called by the Physics Manager when a rigid body makes a new contact with
	//# another rigid body. This function can be overridden in a subclass of $RigidBodyController$ in order to carry out a
	//# specialized response to a collision.
	//#
	//# The $contact$ parameter specifies the newly created $@RigidBodyContact@$ object, which is an edge in the contact graph maintained
	//# by the Physics Manager. The rigid body for which the $HandleNewRigidBodyContact$ function is called can be either the start element
	//# or finish element for this edge. The $contactBody$ parameter specifies the other rigid body involved in the new contact, which is
	//# always on the opposite end of the contact edge relative to the rigid body for which the $HandleNewRigidBodyContact$ function is called.
	//#
	//# When a collision occurs between two rigid bodies, the $HandleNewRigidBodyContact$ function is called once for each rigid body.
	//# The order of the two calls is not defined, so any overridden function should not depend on the $HandleNewRigidBodyContact$ function
	//# being called for the start element of the $contact$ parameter before the finish element or vice-versa.
	//#
	//# An overridden $HandleNewRigidBodyContact$ function can call the $@RigidBodyContact::GetWorldContactPosition@$ function to obtain
	//# the world-space position and normal corresponding to one of the rigid bodies involving in the contact.
	//#
	//# The $HandleNewRigidBodyContact$ function should return one of the following constants.
	//
	//# \table	RigidBodyStatus
	//
	//# An implementation of the $HandleNewRigidBodyContact$ function is allowed to destroy the contact specified by the $contact$ parameter
	//# using the $delete$ operator. In this case, the function must return $kRigidBodyContactsBroken$.
	//#
	//# The default implementation of the $HandleNewRigidBodyContact$ function calls the $@WorldMgr/World::HandleNewRigidBodyContact@$ function.
	//
	//# \also	$@RigidBodyController::HandleNewGeometryContact@$
	//# \also	$@WorldMgr/World::HandleNewRigidBodyContact@$
	//# \also	$@RigidBodyController::PurgeContacts@$
	//# \also	$@RigidBodyContact@$


	//# \function	RigidBodyController::HandleNewGeometryContact		Called when a new contact is made with a geometry node.
	//
	//# \proto	virtual RigidBodyStatus HandleNewGeometryContact(const GeometryContact *contact);
	//
	//# \param	contact			The new contact.
	//
	//# \desc
	//# The $HandleNewGeometryContact$ function is called by the Physics Manager when a rigid body makes a new contact with
	//# a geometry node. This function can be overridden in a subclass of $RigidBodyController$ in order to carry out a
	//# specialized response to a collision.
	//#
	//# The $contact$ parameter specifies the newly created $@GeometryContact@$ object, which is an edge in the contact graph maintained
	//# by the Physics Manager. The rigid body for which the $HandleNewGeometryContact$ function is called is always the start element of this
	//# edge, and a special null body is the finish element.
	//#
	//# The $HandleNewGeometryContact$ function should return one of the following constants.
	//
	//# \table	RigidBodyStatus
	//
	//# An implementation of the $HandleNewGeometryContact$ function is allowed to destroy the contact specified by the $contact$ parameter
	//# using the $delete$ operator. In this case, the function must return $kRigidBodyContactsBroken$.
	//#
	//# If the implementation of the $HandleNewGeometryContact$ function destroys the geometry node referenced by the contact (retrieved
	//# with the $@GeometryContact::GetContactGeometry@$ function), then the $@PhysicsController::PurgeGeometryContacts@$ should be called
	//# for the same geometry node, and the $HandleNewGeometryContact$ function should return $kRigidBodyContactsBroken$.
	//# This is demonstrated by the following code:
	//
	//# \source
	//# Geometry *geometry = contact->GetContactGeometry();\n
	//# GetPhysicsController()->PurgeGeometryContacts(geometry);\n
	//# delete geometry;\n
	//# ...\n
	//# return (kRigidBodyContactsBroken);
	//
	//# \desc
	//# The default implementation of the $HandleNewGeometryContact$ function calls the $@WorldMgr/World::HandleNewGeometryContact@$ function.
	//
	//# \also	$@RigidBodyController::HandleNewRigidBodyContact@$
	//# \also	$@WorldMgr/World::HandleNewGeometryContact@$
	//# \also	$@RigidBodyController::PurgeContacts@$
	//# \also	$@PhysicsController::PurgeGeometryContacts@$
	//# \also	$@GeometryContact@$


	//# \function	RigidBodyController::PurgeContacts		Purges all contacts for a ridig body.
	//
	//# \proto	void PurgeContacts(void);
	//
	//# \desc
	//# The $PurgeContacts$ function purges all contacts for a rigid body. If this function is called from within an overridden
	//# $@RigidBodyController::HandleNewRigidBodyContact@$ or $@RigidBodyController::HandleNewGeometryContact@$ function, then
	//# the value returned by those functions should be $kRigidBodyContactsBroken$.
	//
	//# \also	$@PhysicsController::PurgeGeometryContacts@$


	//# \div
	//# \function	RigidBodyController::HandlePhysicsSpaceExit		Called when a rigid body exits the volume enclosed by the physics space.
	//
	//# \proto	virtual void HandlePhysicsSpaceExit(void);
	//
	//# \desc
	//# The $HandlePhysicsSpaceExit$ function is called by the Physics Manager when a rigid body exits the volume enclosed by
	//# the physics space. An overiding implementation is allowed to destroy the rigid body using the $delete this$ expression.
	//#
	//# The default implementation of the $HandlePhysicsSpaceExit$ function calls the $@WorldMgr/World::HandlePhysicsSpaceExit@$ function.
	//#
	//# A world may contain at most one active physics space, and the world's physics node must be connected to it through a
	//# connector having the type given by the global constant $kConnectorKeyPhysics$.
	//
	//# \also	$@WorldMgr/World::HandlePhysicsSpaceExit@$


	class RigidBodyController : public BodyController, public ListElement<RigidBodyController>, public SnapshotSender
	{
		friend class PhysicsController;

		private:

			struct SleepState
			{
				int32		sleepStepCount;

				float		boxMultiplier;
				float		axisMultiplier;

				Box3D		centerSleepBox;
				Box3D		axisSleepBox[2];
			};

			RigidBodyType			rigidBodyType;
			unsigned_int32			rigidBodyFlags;
			unsigned_int32			rigidBodyState;

			volatile int32			queryThreadFlags;

			float					restitutionCoefficient;
			float					frictionCoefficient;
			float					spinFrictionMultiplier;
			float					rollingResistance;

			unsigned_int32			collisionKind;
			unsigned_int32			collisionExclusionMask;

			const WaterBlock		*submergedWaterBlock;

			List<Shape>				shapeList;
			List<Shape>				internalShapeList;

			float					bodyVolume;
			float					bodyMass;
			Point3D					bodyCenterOfMass;
			InertiaTensor			bodyInertiaTensor;

			float					inverseBodyMass;
			float					inverseBodyScalarInertia;
			InertiaTensor			inverseWorldInertiaTensor;

			float					boundingRadius;
			Box3D					boundingBox;

			bool					repeatCollisionFlag;
			Box3D					bodyCollisionBox;

			Vector3D				linearVelocity;
			Antivector3D			angularVelocity;
			Vector3D				movementVelocity;
			Vector3D				transientLinearVelocity;
			Antivector3D			transientAngularVelocity;
			float					velocityMultiplier;

			Vector3D				originalLinearVelocity;
			Antivector3D			originalAngularVelocity;
			Vector3D				initialLinearVelocity;
			Antivector3D			initialAngularVelocity;
			Vector3D				linearCorrection;
			Antivector3D			angularCorrection;
			float					maxLinearCorrection;
			float					maxAngularCorrection;

			Transform4D				initialTransform;
			Transform4D				finalTransform;
			Transform4D				motionTransform;

			Point3D					initialCenterOfMass;
			Point3D					finalCenterOfMass;

			Vector3D				motionDisplacement;
			Vector3D				motionRotationAxis;
			float					motionRotationAngle;

			Vector3D				externalForce;
			Antivector3D			externalTorque;
			Vector3D				externalLinearResistance;
			float					externalAngularResistance;

			Vector3D				appliedForce;
			Antivector3D			appliedTorque;

			Vector3D				impulseForce;
			Antivector3D			impulseTorque;

			Vector3D				networkDelta[2];
			float					networkDecay[2];
			unsigned_int32			networkParity;

			SleepState				sleepState[2];

			#if C4DIAGS

				Link<RigidBodyRenderable>	rigidBodyRenderable;

			#endif

			C4API Controller *Replicate(void) const override;

			static void WaterBlockLinkProc(Node *node, void *cookie);

			void RecursiveKeepAwake(void);

			RigidBodyContact *FindOutgoingBodyContact(const RigidBodyController *rigidBody, unsigned_int32 startSignature, unsigned_int32 finishSignature) const;
			RigidBodyContact *FindIncomingBodyContact(const RigidBodyController *rigidBody, unsigned_int32 startSignature, unsigned_int32 finishSignature) const;
			bool FindGeometryContact(const Geometry *geometry, unsigned_int32 signature, GeometryContact **matchingContact) const;

			void AdjustDisplacement(float param);
			void KillLaterContacts(float param);

			static void JobDetectGeometryCollision(Job *job, void *cookie);
			static void FinalizeExistingStaticGeometrySingleContact(Job *job, void *cookie);
			static void FinalizeNewStaticGeometrySingleContact(Job *job, void *cookie);
			static void FinalizeExistingStaticGeometryMultipleContact(Job *job, void *cookie);
			static void FinalizeNewStaticGeometryMultipleContact(Job *job, void *cookie);
			static void FinalizeNewDynamicGeometrySingleContact(Job *job, void *cookie);
			static void FinalizeNewDynamicGeometryMultipleContact(Job *job, void *cookie);

			void DetectGeometryCollision(Geometry *geometry, const Point3D& p1, const Point3D& p2);
			void DetectNodeCollision(Node *node, unsigned_int32 stamp, const Point3D& p1, const Point3D& p2);
			void DetectCellCollision(const Site *cell, unsigned_int32 stamp, const Point3D& p1, const Point3D& p2);
			void DetectZoneCollision(const Zone *zone, unsigned_int32 stamp, const Point3D& p1, const Point3D& p2);
			void DetectWorldCollisions(unsigned_int32 stamp);

			void ApplyCellForceFields(Site *cell, const Box3D& box, unsigned_int32 fieldStamp);
			void CalculateAppliedForces(const Vector3D& gravity);

			void InitializeConstraints(void);
			void InitializeRepeatConstraints(void);
			void Finalize(const Box3D *physicsBoundingBox);

		protected:

			C4API RigidBodyController(ControllerType type);
			C4API RigidBodyController(const RigidBodyController& rigidBodyController);

			void SetVelocityMultiplier(float multiplier)
			{
				velocityMultiplier = multiplier;
			}

		public:

			enum
			{
				kRigidBodyMessageSnapshot,
				kRigidBodyMessageWake,
				kRigidBodyMessageSleep,
				kRigidBodyMessageBaseCount
			};

			C4API RigidBodyController();
			C4API ~RigidBodyController();

			using ListElement<RigidBodyController>::Previous;
			using ListElement<RigidBodyController>::Next;
			using ListElement<RigidBodyController>::GetOwningList;

			RigidBodyType GetRigidBodyType(void) const
			{
				return (rigidBodyType);
			}

			void SetRigidBodyType(RigidBodyType type)
			{
				rigidBodyType = type;
			}

			unsigned_int32 GetRigidBodyFlags(void) const
			{
				return (rigidBodyFlags);
			}

			void SetRigidBodyFlags(unsigned_int32 flags)
			{
				rigidBodyFlags = flags;
			}

			bool RigidBodyAsleep(void) const
			{
				return ((rigidBodyState & kRigidBodyAsleep) != 0);
			}

			volatile int32 *GetQueryThreadFlags(void)
			{
				return (&queryThreadFlags);
			}

			float GetRestitutionCoefficient(void) const
			{
				return (restitutionCoefficient);
			}

			void SetRestitutionCoefficient(float restitution)
			{
				restitutionCoefficient = restitution;
			}

			float GetFrictionCoefficient(void) const
			{
				return (frictionCoefficient);
			}

			void SetFrictionCoefficient(float friction)
			{
				frictionCoefficient = friction;
			}

			float GetSpinFrictionMultiplier(void) const
			{
				return (spinFrictionMultiplier);
			}

			void SetSpinFrictionMultiplier(float spin)
			{
				spinFrictionMultiplier = spin;
			}

			float GetRollingResistance(void) const
			{
				return (rollingResistance);
			}

			void SetRollingResistance(float resistance)
			{
				rollingResistance = resistance;
			}

			unsigned_int32 GetCollisionKind(void) const
			{
				return (collisionKind);
			}

			void SetCollisionKind(unsigned_int32 kind)
			{
				collisionKind = kind;
			}

			unsigned_int32 GetCollisionExclusionMask(void) const
			{
				return (collisionExclusionMask);
			}

			void SetCollisionExclusionMask(unsigned_int32 mask)
			{
				collisionExclusionMask = mask;
			}

			const WaterBlock *GetSubmergedWaterBlock(void) const
			{
				return (submergedWaterBlock);
			}

			void SetSubmergedWaterBlock(const WaterBlock *waterBlock)
			{
				submergedWaterBlock = waterBlock;
			}

			Shape *GetFirstShape(void) const
			{
				return (shapeList.First());
			}

			float GetBodyVolume(void) const
			{
				return (bodyVolume);
			}

			float GetBodyMass(void) const
			{
				return (bodyMass);
			}

			const Point3D& GetBodyCenterOfMass(void) const
			{
				return (bodyCenterOfMass);
			}

			float GetInverseBodyMass(void) const
			{
				return (inverseBodyMass);
			}

			const InertiaTensor& GetInverseWorldInertiaTensor(void) const
			{
				return (inverseWorldInertiaTensor);
			}

			float GetBoundingRadius(void) const
			{
				return (boundingRadius);
			}

			const Box3D& GetBoundingBox(void) const
			{
				return (boundingBox);
			}

			const Vector3D& GetLinearVelocity(void) const
			{
				return (linearVelocity);
			}

			void SetLinearVelocity(const Vector3D& velocity)
			{
				linearVelocity = velocity;
			}

			const Antivector3D& GetAngularVelocity(void) const
			{
				return (angularVelocity);
			}

			void SetAngularVelocity(const Antivector3D& velocity)
			{
				angularVelocity = velocity;
			}

			const Vector3D& GetMovementVelocity(void) const
			{
				return (movementVelocity);
			}

			void SetMovementVelocity(const Vector3D& velocity)
			{
				movementVelocity = velocity;
			}

			const Vector3D& GetOriginalLinearVelocity(void) const
			{
				return (originalLinearVelocity);
			}

			const Antivector3D& GetOriginalAngularVelocity(void) const
			{
				return (originalAngularVelocity);
			}

			const Transform4D& GetFinalTransform(void) const
			{
				return (finalTransform);
			}

			const Point3D& GetFinalPosition(void) const
			{
				return (finalTransform.GetTranslation());
			}

			const Point3D& GetWorldCenterOfMass(void) const
			{
				return (finalCenterOfMass);
			}

			const Vector3D& GetExternalForce(void) const
			{
				return (externalForce);
			}

			void SetExternalForce(const Vector2D& force)
			{
				externalForce = force;
			}

			void SetExternalForce(const Vector3D& force)
			{
				externalForce = force;
			}

			const Antivector3D& GetExternalTorque(void) const
			{
				return (externalTorque);
			}

			void SetExternalTorque(const Antivector3D& torque)
			{
				externalTorque = torque;
			}

			const Vector3D& GetExternalLinearResistance(void) const
			{
				return (externalLinearResistance);
			}

			void SetExternalLinearResistance(const Vector2D& resistance)
			{
				externalLinearResistance = resistance;
			}

			void SetExternalLinearResistance(const Vector3D& resistance)
			{
				externalLinearResistance = resistance;
			}

			float GetExternalAngularResistance(void) const
			{
				return (externalAngularResistance);
			}

			void SetExternalAngularResistance(float resistance)
			{
				externalAngularResistance = resistance;
			}

			const Vector3D& GetAppliedForce(void) const
			{
				return (appliedForce);
			}

			const Antivector3D& GetAppliedTorque(void) const
			{
				return (appliedTorque);
			}

			float GetSleepBoxMultiplier(void) const
			{
				return (sleepState[0].boxMultiplier);
			}

			void SetSleepBoxMultiplier(float multiplier)
			{
				sleepState[0].boxMultiplier = multiplier;
				sleepState[1].boxMultiplier = multiplier * 2.0F;
			}

			float GetSleepAxisMultiplier(void) const
			{
				return (sleepState[0].axisMultiplier);
			}

			void SetSleepAxisMultiplier(float multiplier)
			{
				sleepState[0].axisMultiplier = multiplier;
				sleepState[1].axisMultiplier = multiplier;
			}

			Vector3D CalculateWorldPositionVelocity(const Point3D& position) const
			{
				return (linearVelocity + angularVelocity % (position - finalCenterOfMass));
			}

			C4API void Pack(Packer& data, unsigned_int32 packFlags) const override;
			C4API void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			C4API int32 GetSettingCount(void) const override;
			C4API Setting *GetSetting(int32 index) const override;
			C4API void SetSetting(const Setting *setting) override;

			C4API void Preprocess(void) override;
			C4API void Neutralize(void) override;

			C4API ControllerMessage *CreateMessage(ControllerMessageType type) const override;
			C4API void ReceiveMessage(const ControllerMessage *message) override;
			C4API void SendInitialStateMessages(Player *player) const override;
			C4API void SendSnapshot(void);

			C4API void Wake(void) override;
			C4API void Sleep(void) override;
			C4API void RecursiveWake(void) final override;

			void Integrate(void);

			void ApplyVelocityCorrection(const Jacobian& jacobian, float impulse);
			void ApplyLinearVelocityCorrection(const Vector3D& jacobian, float impulse);
			void ApplyAngularVelocityCorrection(const Vector3D& jacobian, float impulse);
			void ApplyPositionCorrection(const Jacobian& jacobian, float impulse);
			void ApplyLinearPositionCorrection(const Vector3D& jacobian, float impulse);
			void ApplyAngularPositionCorrection(const Vector3D& jacobian, float impulse);

			C4API void SetRigidBodyTransform(const Transform4D& transform);
			C4API void SetRigidBodyMatrix3D(const Matrix3D& matrix);
			C4API void SetRigidBodyPosition(const Point3D& position);

			C4API void PurgeContacts(void);

			C4API void ApplyImpulse(const Vector3D& impulse);
			C4API void ApplyImpulse(const Vector3D& impulse, const Point3D& position);

			C4API bool DetectSegmentIntersection(const Point3D& p1, const Point3D& p2, float radius, BodyHitData *bodyHitData) const;
			C4API float CalculateSubmergedVolume(const Antivector4D& plane, Point3D *submergedCentroid) const;

			C4API virtual bool ValidRigidBodyCollision(const RigidBodyController *body) const;
			C4API virtual bool ValidGeometryCollision(const Geometry *geometry) const;

			C4API virtual RigidBodyStatus HandleNewRigidBodyContact(const RigidBodyContact *contact, RigidBodyController *contactBody);
			C4API virtual RigidBodyStatus HandleNewGeometryContact(const GeometryContact *contact);

			C4API virtual void HandlePhysicsSpaceExit(void);
			C4API virtual void HandleWaterSubmergence(void);
	};


	class RigidBodySnapshotMessage : public ControllerMessage
	{
		friend class RigidBodyController;

		private:

			Point3D			rigidBodyPosition;
			Quaternion		rigidBodyRotation;
			Vector3D		rigidBodyLinearVelocity;
			Antivector3D	rigidBodyAngularVelocity;

			RigidBodySnapshotMessage(int32 controllerIndex);

		public:

			RigidBodySnapshotMessage(int32 controllerIndex, const Point3D& position, const Quaternion& rotation, const Vector3D& linearVelocity, const Vector3D& angularVelocity);
			~RigidBodySnapshotMessage();

			const Point3D& GetRigidBodyPosition(void) const
			{
				return (rigidBodyPosition);
			}

			const Quaternion& GetRigidBodyRotation(void) const
			{
				return (rigidBodyRotation);
			}

			const Vector3D& GetRigidBodyLinearVelocity(void) const
			{
				return (rigidBodyLinearVelocity);
			}

			const Antivector3D& GetRigidBodyAngularVelocity(void) const
			{
				return (rigidBodyAngularVelocity);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;
	};


	class RigidBodySleepMessage : public ControllerMessage
	{
		friend class RigidBodyController;

		private:

			Point3D			rigidBodyPosition;
			Quaternion		rigidBodyRotation;

			RigidBodySleepMessage(int32 controllerIndex);

		public:

			RigidBodySleepMessage(int32 controllerIndex, const Point3D& position, const Quaternion& rotation);
			~RigidBodySleepMessage();

			const Point3D& GetRigidBodyPosition(void) const
			{
				return (rigidBodyPosition);
			}

			const Quaternion& GetRigidBodyRotation(void) const
			{
				return (rigidBodyRotation);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;
	};


	//# \class	RagdollController		Manages a set of rigid bodies that compose a ragdoll.
	//
	//# The $RagdollController$ class manages a set of rigid bodies that compose a ragdoll.
	//
	//# \def	class RagdollController final : public RigidBodyController, public LinkTarget<RagdollController>
	//
	//# \ctor	RagdollController();
	//
	//# \desc
	//# The $RagdollController$ class manages a set of rigid bodies that are connected by joints and
	//# collectively make up the parts of a ragdoll.
	//
	//# \base	RigidBodyController							A $RagdollController$ is a specific type of rigid body controller.
	//# \base	Utilities/LinkTarget<RagdollController>		Ragdoll controllers support smart linking with the $@Utilities/Link@$ class.


	class RagdollController final : public RigidBodyController, public LinkTarget<RagdollController>
	{
		private:

			class Association
			{
				public:

					Link<Node>		ragdollNode;
					Node			*modelNode;

					Association(Node *ragdoll, Node *model = nullptr) : ragdollNode(ragdoll)
					{
						modelNode = model;
					}
			};

			Model						*targetModel;
			Array<Association, 16>		associationArray;

			RagdollController(const RagdollController& ragdollController);

			Controller *Replicate(void) const;

			static void ModelLinkProc(Node *node, void *cookie);
			static void AssociationLinkProc(Node *node, void *cookie);

		public:

			RagdollController();
			~RagdollController();

			void SetTargetModel(Model *model)
			{
				targetModel = model;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void ReassociateNodes(void);
			void AttachModel(const ImmutableArray<Node *>& nodeArray, Model *model);
			void UpdateModel(void);

			void HandlePhysicsSpaceExit(void) override;

			C4API void Delete(void);
	};


	//# \class	PhysicsController		Manages the physics simulation for an entire world.
	//
	//# The $PhysicsController$ class manages the physics simulation for an entire world.
	//
	//# \def	class PhysicsController final : public Controller
	//
	//# \ctor	PhysicsController();
	//
	//# \desc
	//# The $PhysicsController$ class manages the physics simulation for an entire world. A physics controller
	//# can only be assigned to a physics node, and exactly one physics node should exist in the root zone of
	//# any world using the features of the Physics Manager.
	//
	//# \base	Controller/Controller	A $PhysicsController$ is a specific type of controller.
	//
	//# \also	$@PhysicsNode@$
	//# \also	$@RigidBodyController@$


	//# \function	PhysicsController::GetMaxLinearSpeed	Returns the maximum linear speed for all rigid bodies.
	//
	//# \proto	float GetMaxLinearSpeed(void) const;
	//
	//# \desc
	//# The $GetMaxLinearSpeed$ function returns the maximum linear speed, measured in meters per second (m/s),
	//# that is allowed for all rigid bodies. If the magnitude of a rigid body's linear velocity ever exceeds
	//# this value, then is it clamped at the end of the simulation step.
	//
	//# \also	$@PhysicsController::SetMaxLinearSpeed@$
	//# \also	$@PhysicsController::GetMaxAngularSpeed@$
	//# \also	$@PhysicsController::SetMaxAngularSpeed@$


	//# \function	PhysicsController::SetMaxLinearSpeed	Sets the maximum linear speed for all rigid bodies.
	//
	//# \proto	void SetMaxLinearSpeed(float speed);
	//
	//# \param	speed	The new maximum linear speed, in meters per second (m/s).
	//
	//# \desc
	//# The $SetMaxLinearSpeed$ function sets the maximum linear speed, measured in meters per second (m/s),
	//# that is allowed for all rigid bodies to the value specified by the $speed$ parameter. If the magnitude
	//# of a rigid body's linear velocity ever exceeds this value, then is it clamped at the end of the simulation step.
	//
	//# \also	$@PhysicsController::GetMaxLinearSpeed@$
	//# \also	$@PhysicsController::GetMaxAngularSpeed@$
	//# \also	$@PhysicsController::SetMaxAngularSpeed@$


	//# \function	PhysicsController::GetMaxAngularSpeed	Returns the maximum angular speed for all rigid bodies.
	//
	//# \proto	float GetMaxAngularSpeed(void) const;
	//
	//# \desc
	//# The $GetMaxAngularSpeed$ function returns the maximum angular speed, measured in radians per second (rad/s),
	//# that is allowed for all rigid bodies. If the magnitude of a rigid body's angular velocity ever exceeds this
	//# value, then is it clamped at the end of the simulation step.
	//
	//# \also	$@PhysicsController::SetMaxAngularSpeed@$
	//# \also	$@PhysicsController::GetMaxLinearSpeed@$
	//# \also	$@PhysicsController::SetMaxLinearSpeed@$


	//# \function	PhysicsController::SetMaxAngularSpeed	Sets the maximum angular speed for all rigid bodies.
	//
	//# \proto	void SetMaxAngularSpeed(float speed);
	//
	//# \param	speed	The new maximum angular speed, in radians per second (rad/s).
	//
	//# \desc
	//# The $SetMaxAngularSpeed$ function sets the maximum angular speed, measured in radians per second (rad/s),
	//# that is allowed for all rigid bodies to the value specified by the $speed$ parameter. If the magnitude
	//# of a rigid body's angular velocity ever exceeds this value, then is it clamped at the end of the simulation step.
	//
	//# \also	$@PhysicsController::GetMaxAngularSpeed@$
	//# \also	$@PhysicsController::GetMaxLinearSpeed@$
	//# \also	$@PhysicsController::SetMaxLinearSpeed@$


	//# \function	PhysicsController::GetGravityAcceleration	Returns the global acceleration of gravity.
	//
	//# \proto	const Vector3D& GetGravityAcceleration(void) const;
	//
	//# \desc
	//# The $GetGravityAcceleration$ function returns the global world-space acceleration of gravity, measured
	//# in meters per second squared (m/s<sup>2</sup>).
	//
	//# \also	$@PhysicsController::SetGravityAcceleration@$
	//# \also	$@BodyController::GetGravityMultiplier@$
	//# \also	$@BodyController::SetGravityMultiplier@$


	//# \function	PhysicsController::SetGravityAcceleration	Sets the global acceleration of gravity.
	//
	//# \proto	void SetGravityAcceleration(const Vector3D& acceleration);
	//
	//# \param	acceleration	The new acceleration of gravity, in meters per second squared (m/s<sup>2</sup>).
	//
	//# \desc
	//# The $SetGravityAcceleration$ function sets the global world-space acceleration of gravity, measured
	//# in meters per second squared (m/s<sup>2</sup>), to the vector specified by the $acceleration$ parameter.
	//#
	//# The initial value of the gravity acceleration is (0,&nbsp;0,&nbsp;&minus;9.8) m/s<sup>2</sup>.
	//
	//# \also	$@PhysicsController::GetGravityAcceleration@$
	//# \also	$@BodyController::GetGravityMultiplier@$
	//# \also	$@BodyController::SetGravityMultiplier@$


	//# \function	PhysicsController::WakeFieldRigidBodies		Wakes all rigid bodies affected by a force field.
	//
	//# \proto	void WakeFieldRigidBodies(const Field *field);
	//
	//# \param	field		The field node for which rigid bodies are awakened.
	//
	//# \desc
	//# The $WakeFieldRigidBodies$ function searches for all rigid bodies that would be affected by the force field
	//# specified by the $field$ parameter and awakens any that are currently asleep. For each rigid body that is
	//# awakened, all bodies reachable from that rigid body in the contact graph are also awakened recursively.
	//
	//# \also	$@Field@$
	//# \also	$@Force@$


	//# \function	PhysicsController::PurgeGeometryContacts	Purges all contacts with a particular geometry node.
	//
	//# \proto	void PurgeGeometryContacts(const Geometry *geometry);
	//
	//# \param	geometry	The geometry node for which all contacts are purged.
	//
	//# \desc
	//# The $PurgeGeometryContacts$ function purges all contacts that currently exist between any rigid body and the
	//# geometry node specified by the $geometry$ parameter. If this function is called from an overridden
	//# $@RigidBodyController::HandleNewGeometryContact@$ function, then the value returned by that function should
	//# be $kRigidBodyContactsBroken$.
	//
	//# \also	$@RigidBodyController::HandleNewGeometryContact@$
	//# \also	$@RigidBodyController::PurgeContacts@$


	class PhysicsController final : public Controller
	{
		friend class RigidBodyController;

		private:

			Graph<Body, Contact>				physicsGraph;
			Body								nullBody;

			int32								simulationStep;
			int32								simulationTime;
			float								interpolationParam;

			float								maxLinearSpeed;
			float								maxAngularSpeed;

			Vector3D							gravityAcceleration;

			unsigned_int32						rigidBodyParity;
			unsigned_int32						sleepingParity;

			unsigned_int32						geometryCollisionStamp;
			unsigned_int32						fieldApplicationStamp;

			List<RigidBodyController>			rigidBodyList[2];
			List<RigidBodyController>			sleepingList[2];
			List<RigidBodyController>			*simulationList;

			List<DeformableBodyController>		deformableBodyList;
			Batch								deformableBatch;

			Batch								collisionBatch;
			Batch								constraintSolverBatch;
			Array<BatchJob *, 16>				repeatCollisionJobArray;

			Mutex								brokenJointMutex;
			Array<Joint *, 8>					brokenJointArray;

			Array<RagdollController *, 32>		ragdollArray;

			int32								physicsCounter[kPhysicsCounterCount];

			static float SortRigidBodyList(List<RigidBodyController> *inputList, int32 depth, float minValue, float maxValue, int32 index, List<RigidBodyController> *outputList);
			void CollideRigidBodiesX(List<RigidBodyController> *inputList, int32 depth, float xmin, float xmax, List<RigidBodyController> *outputList);
			void CollideRigidBodiesY(List<RigidBodyController> *inputList, int32 depth, float ymin, float ymax, List<RigidBodyController> *outputList);
			void CollideRigidBodiesZ(List<RigidBodyController> *inputList, int32 depth, float zmin, float zmax, List<RigidBodyController> *outputList);

			void DetectBodyCollision(RigidBodyController *alphaBody, RigidBodyController *betaBody);

			static void JobDetectShapeCollision(Job *job, void *cookie);
			static void FinalizeExistingShapeContact(Job *job, void *cookie);
			static void FinalizeNewShapeContact(Job *job, void *cookie);

			static void BuildConstraintIsland(RigidBodyController *rigidBody, ConstraintSolverJob *solverJob);
			static void JobSolveIslandConstraints(Job *job, void *cookie);

		public:

			C4API PhysicsController();
			C4API ~PhysicsController();

			Body *GetNullBody(void)
			{
				return (&nullBody);
			}

			Body *GetFirstBody(void) const
			{
				return (physicsGraph.GetFirstElement());
			}

			int32 GetSimulationStep(void) const
			{
				return (simulationStep);
			}

			float GetInterpolationParam(void) const
			{
				return (interpolationParam);
			}

			float GetMaxLinearSpeed(void) const
			{
				return (maxLinearSpeed);
			}

			void SetMaxLinearSpeed(float speed)
			{
				maxLinearSpeed = speed;
			}

			float GetMaxAngularSpeed(void) const
			{
				return (maxAngularSpeed);
			}

			void SetMaxAngularSpeed(float speed)
			{
				maxAngularSpeed = speed;
			}

			const Vector3D& GetGravityAcceleration(void) const
			{
				return (gravityAcceleration);
			}

			void SetGravityAcceleration(const Vector3D& acceleration)
			{
				gravityAcceleration = acceleration;
			}

			unsigned_int32 IncrementFieldStamp(void)
			{
				return (++fieldApplicationStamp);
			}

			void AddDeformableBody(DeformableBodyController *deformableBody)
			{
				deformableBodyList.Append(deformableBody);
			}

			int32 GetPhysicsCounter(int32 index) const
			{
				return (physicsCounter[index]);
			}

			void IncrementPhysicsCounter(int32 index)
			{
				physicsCounter[index]++;
			}

			static bool ValidNode(const Node *node);
			static void RegisterFunctions(ControllerRegistration *registration);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Preprocess(void) override;

			void AddRigidBody(RigidBodyController *rigidBody);
			void RemoveRigidBody(RigidBodyController *rigidBody);

			void WakeRigidBody(RigidBodyController *rigidBody);
			void SleepRigidBody(RigidBodyController *rigidBody);

			C4API void WakeFieldRigidBodies(const Field *field);
			C4API void PurgeGeometryContacts(const Geometry *geometry);

			void AddBrokenJoint(Joint *joint);

			void Move(void) override;
	};


	class SetGravityFunction final : public Function
	{
		private:

			float		gravityAcceleration;

			SetGravityFunction(const SetGravityFunction& setGravityFunction);

			Function *Replicate(void) const override;

		public:

			SetGravityFunction();
			~SetGravityFunction();

			float GetGravityAcceleration(void) const
			{
				return (gravityAcceleration);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
	};


	//# \class	PhysicsNode		Represents a physics node in a world.
	//
	//# The $PhysicsNode$ class represents a physics node in a world.
	//
	//# \def	class PhysicsNode final : public Node
	//
	//# \ctor	PhysicsNode();
	//
	//# \desc
	//# The $PhysicsNode$ class represents a physics node in a world. Any world using the features of the
	//# Physics Manager should contain exactly one physics node in the root zone, and it should have a physics
	//# controller assigned to it.
	//
	//# \base	WorldMgr/Node	A $PhysicsNode$ is a scene graph node.
	//
	//# \also	$@PhysicsController@$


	class PhysicsNode final : public Node
	{
		private:

			PhysicsSpace		*connectedPhysicsSpace;
			Box3D				physicsBoundingBox;

		public:

			C4API PhysicsNode();
			C4API ~PhysicsNode();

			PhysicsSpace *GetConnectedPhysicsSpace(void) const
			{
				return (connectedPhysicsSpace);
			}

			const Box3D& GetPhysicsBoundingBox(void) const
			{
				return (physicsBoundingBox);
			}

			int32 GetInternalConnectorCount(void) const override;
			const char *GetInternalConnectorKey(int32 index) const override;
			void ProcessInternalConnectors(void) override;
			bool ValidConnectedNode(const ConnectorKey& key, const Node *node) const override;
			void SetConnectedPhysicsSpace(PhysicsSpace *physicsSpace);

			void Preprocess(void) override;
	};


	#if C4DIAGS

		class RigidBodyRenderable : public Renderable, public LinkTarget<RigidBodyRenderable>
		{
			private:

				VertexBuffer				vertexBuffer;
				static SharedVertexBuffer	indexBuffer;

				List<Attribute>				attributeList;
				DiffuseAttribute			diffuseColor;

			public:

				RigidBodyRenderable(const Box3D& box);
				~RigidBodyRenderable();

				void SetCollisionBox(const Box3D& box);
		};

	#endif
}


#endif

// ZYUQURM
