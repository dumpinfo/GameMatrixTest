 

#ifndef C4Forces_h
#define C4Forces_h


//# \component	Physics Manager
//# \prefix		PhysicsMgr/


#include "C4Creation.h"
#include "C4Configurable.h"
#include "C4Connector.h"


namespace C4
{
	//# \tree	Force
	//
	//# \node	FluidForce
	//# \node	WindForce


	typedef Type	ForceType;


	enum : ForceType
	{
		kForceUnknown	= '!UNK',
		kForceGravity	= 'GRAV',
		kForceFluid		= 'FLUD',
		kForceWind		= 'WIND'
	};


	class Force;
	class Field;
	class RigidBodyController;
	class DeformableBodyController;
	class WaterBlock;


	//# \class	ForceRegistration		Manages internal registration information for a custom force type.
	//
	//# The $ForceRegistration$ class manages internal registration information for a custom force type.
	//
	//# \def	class ForceRegistration : public Registration<Force, ForceRegistration>
	//
	//# \ctor	ForceRegistration(ForceType type, const char *name);
	//
	//# \param	type	The force type.
	//# \param	name	The force name.
	//
	//# \desc
	//# The $ForceRegistration$ class is abstract and serves as the common base class for the template class
	//# $@ForceReg@$. A custom force is registered with the engine by instantiating an object of type
	//# $ForceReg<classType>$, where $classType$ is the type of the force subclass being registered.
	//
	//# \base	System/Registration<Force, ForceRegistration>	A force registration is a specific type of registration object.
	//
	//# \also	$@ForceReg@$
	//# \also	$@Force@$


	//# \function	ForceRegistration::GetForceType		Returns the registered force type.
	//
	//# \proto	ForceType GetForceType(void) const;
	//
	//# \desc
	//# The $GetForceType$ function returns the force type for a particular force registration.
	//# The force type is established when the force registration is constructed.
	//
	//# \also	$@ForceRegistration::GetForceName@$


	//# \function	ForceRegistration::GetForceName		Returns the human-readable force name.
	//
	//# \proto	const char *GetForceName(void) const;
	//
	//# \desc
	//# The $GetForceName$ function returns the human-readable force name for a particular force registration.
	//# The force name is established when the force registration is constructed.
	//
	//# \also	$@ForceRegistration::GetForceType@$


	class ForceRegistration : public Registration<Force, ForceRegistration>
	{
		private:

			const char		*forceName;

		protected:

			C4API ForceRegistration(ForceType type, const char *name);

		public:

			C4API ~ForceRegistration();

			ForceType GetForceType(void) const
			{
				return (GetRegistrableType()); 
			}

			const char *GetForceName(void) const 
			{
				return (forceName); 
			}

			virtual bool ValidField(const Field *field) const = 0; 
	};
 
 
	//# \class	ForceReg	 Represents a custom force type.
	//
	//# The $ForceReg$ class represents a custom force type.
	// 
	//# \def	template <class classType> class ForceReg : public ForceRegistration
	//
	//# \tparam	classType	The custom force class.
	//
	//# \ctor	ForceReg(ForceType type, const char *name);
	//
	//# \param	type	The force type.
	//# \param	name	The force name.
	//
	//# \desc
	//# The $ForceReg$ template class is used to advertise the existence of a custom force type.
	//# The World Manager uses a force registration to construct a custom force, and the World Editor
	//# examines a force registration to determine what type of field a custom force can be assigned to.
	//# The act of instantiating a $ForceReg$ object automatically registers the corresponding force
	//# type. The force type is unregistered when the $ForceReg$ object is destroyed.
	//#
	//# No more than one force registration should be created for each distinct force type.
	//
	//# \base	ForceRegistration	All specific force registration classes share the common base class $ForceRegistration$.
	//
	//# \also	$@Force@$


	template <class classType> class ForceReg : public ForceRegistration
	{
		public:

			ForceReg(ForceType type, const char *name) : ForceRegistration(type, name)
			{
			}

			Force *Create(void) const
			{
				return (new classType);
			}

			bool ValidField(const Field *field) const override
			{
				return (classType::ValidField(field));
			}
	};


	//# \class	Force		Exerts a force on rigid bodies inside a field.
	//
	//# The $Force$ class exerts a force on rigid bodies inside a field.
	//
	//# \def	class Force : public ListElement<Force>, public Packable, public Configurable, public Registrable<Force, ForceRegistration>
	//
	//# \ctor	Force(ForceType type);
	//
	//# \param	type		The force type.
	//
	//# \desc
	//# The $Force$ class represents a generic force that is exerted on rigid bodies inside a field. A subclass of the $Force$
	//# class implements the $@Force::ApplyForce@$ function in order to define the force that acts upon rigid bodies.
	//#
	//# A $Force$ object is assigned to a $@Field@$ node by calling the $@Field::SetForce@$ function. The field to which a force
	//# is assigned can be retrieved by calling the $@Force::GetTargetField@$ function.
	//
	//# \base	Utilities/ListElement<Force>					Used internally by the World Manager.
	//# \base	ResourceMgr/Packable							Forces can be packed for storage in resources.
	//# \base	InterfaceMgr/Configurable						Forces can define configurable parameters that are exposed
	//#															as user interface widgets in the World Editor.
	//# \base	System/Registrable<Force, ForceRegistration>	Custom force types can be registered with the engine.
	//
	//# \also	$@ForceReg@$


	//# \function	Force::GetForceType		Returns the force type.
	//
	//# \proto	ForceType GetForceType(void) const;
	//
	//# \desc
	//# The $GetForceType$ function returns the force type.
	//
	//# \also	$@ForceReg@$


	//# \function	Force::GetTargetField		Returns the field to which a force is assigned.
	//
	//# \proto	Field *GetTargetField(void) const;
	//
	//# \desc
	//# The $GetTargetField$ function returns the $@Field@$ node to which a force is assigned. If the force is not
	//# assigned to a field, then the return value is $nullptr$.
	//
	//# \also	$@Field::GetForce@$
	//# \also	$@Field::SetForce@$


	//# \function	Force::ValidField		Returns a boolean value indicating whether the force can be assigned to a particular field node.
	//
	//# \proto	static bool ValidField(const Field *field);
	//
	//# \desc
	//# The $ValidField$ function should be redefined by force subclasses. Its implementation should examine the
	//# field node pointed to by the $field$ parameter and return $true$ if the force type can be used with the field.
	//# If the force type cannot be used, the $ValidField$ function should return $false$. If the $ValidField$
	//# function is not redefined for a registered subclass of the $Force$ class, then that force type
	//# can be assigned to any field node.
	//
	//# \also	$@Field@$


	//# \function	Force::ApplyForce		Calculates the force to exert on a rigid body or deformable body.
	//
	//# \proto	bool ApplyForce(RigidBodyController *rigidBody, const Transform4D& worldTransform, Vector3D *restrict force, Antivector3D *restrict torque);
	//
	//# \param	rigidBody		The rigid body to which a force should be applied.
	//# \param	worldTransform	The world transform representing the current state of the rigid body.
	//# \param	force			A pointer to the location where the world-space force to apply should be written.
	//# \param	torque			A pointer to the location where thr world-space torque to apply should be written.
	//
	//# \desc
	//# The Physics Manager calls the $ApplyForce$ function for each rigid body that intersects the field to which the $Force$ object
	//# is assigned. The $ApplyForce$ function should determine what force and torque to exert on the rigid body and return it through
	//# the $force$ and $torque$ parameters. If the $ApplyForce$ function exerts a force, then it should return $true$. If no force or torque
	//# is to be applied, then the $ApplyForce$ function should return $false$.
	//#
	//# The implementation of the $ApplyForce$ function should not base the calculated forces on the world transform of the target node of
	//# the $@RigidBodyController@$ object. The $worldTransform$ parameter holds the world transform that should be used to determine the
	//# correct state of nodes composing the rigid body.
	//
	//# \also	$@Field@$
	//# \also	$@RigidBodyController@$


	class Force : public ListElement<Force>, public Packable, public Configurable, public Registrable<Force, ForceRegistration>
	{
		friend class Field;

		private:

			ForceType		forceType;

			Field			*targetField;

			void SetTargetField(Field *field)
			{
				targetField = field;
			}

			virtual Force *Replicate(void) const = 0;

			static Force *CreateUnknown(Unpacker& data, unsigned_int32 unpackFlags);

		protected:

			C4API Force(ForceType type);
			C4API Force(const Force& force);

		public:

			C4API virtual ~Force();

			ForceType GetForceType(void) const
			{
				return (forceType);
			}

			Field *GetTargetField(void) const
			{
				return (targetField);
			}

			Force *Clone(void) const
			{
				return (Replicate());
			}

			C4API static Force *New(ForceType type);

			C4API static bool ValidField(const Field *field);
			static void RegisterStandardForces(void);

			C4API void PackType(Packer& data) const override;

			C4API virtual void Preprocess(void);
			C4API virtual bool ApplyForce(RigidBodyController *rigidBody, const Transform4D& worldTransform, Vector3D *restrict force, Antivector3D *restrict torque);
			C4API virtual void ApplyForce(DeformableBodyController *deformableBody, const Transformable *transformable, int32 count, const Point3D *position, const SimdVector3D *velocity, SimdVector3D *restrict force);
	};


	class UnknownForce : public Force
	{
		friend class Force;
		friend class ForceReg<UnknownForce>;

		private:

			ForceType			unknownType;
			unsigned_int32		unknownSize;
			char				*unknownData;

			UnknownForce();
			UnknownForce(ForceType type);
			UnknownForce(const UnknownForce& unknownForce);

			Force *Replicate(void) const override;

		public:

			~UnknownForce();

			static bool ValidField(const Field *field);

			void PackType(Packer& data) const override;
			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;
	};


	//# \class	GravityForce	Exerts a gravity force on rigid bodies inside a field.
	//
	//# The $GravityForce$ class exerts a gravity force on rigid bodies inside a field.
	//
	//# \def	class GravityForce : public Force
	//
	//# \ctor	GravityForce();
	//
	//# \desc
	//# The $GravityForce$ class represents a gravity force that produces acceleration inside a field. When a rigid body
	//# intersects a field to which a $GravityForce$ object is assigned, the gravity force is applied to the rigid body
	//# <i>in addition</i> to the global gravity force set for the physics controller.
	//#
	//# The gravity force is always directed downward in the direction of the negative <i>z</i> axis of the field to which
	//# it is assigned.
	//#
	//# For each rigid body, the gravity force experienced in a field is affected by the rigid body's gravity multiplier.
	//
	//# \base	Force		A gravity force is a specific type of force.
	//
	//# \also	$@PhysicsController::GetGravityAcceleration@$
	//# \also	$@PhysicsController::SetGravityAcceleration@$
	//# \also	$@BodyController::GetGravityMultiplier@$
	//# \also	$@BodyController::SetGravityMultiplier@$


	class GravityForce : public Force
	{
		private:

			float		gravityAcceleration;

			GravityForce(const GravityForce& gravityForce);

			Force *Replicate(void) const override;

		public:

			C4API GravityForce();
			C4API ~GravityForce();

			float GetGravityAcceleration(void) const
			{
				return (gravityAcceleration);
			}

			void SetGravityAcceleration(float acceleration)
			{
				gravityAcceleration = acceleration;
			}

			C4API void Pack(Packer& data, unsigned_int32 packFlags) const override;
			C4API void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			C4API int32 GetSettingCount(void) const override;
			C4API Setting *GetSetting(int32 index) const override;
			C4API void SetSetting(const Setting *setting) override;

			bool ApplyForce(RigidBodyController *rigidBody, const Transform4D& worldTransform, Vector3D *restrict force, Antivector3D *restrict torque) override;
			void ApplyForce(DeformableBodyController *deformableBody, const Transformable *transformable, int32 count, const Point3D *position, const SimdVector3D *velocity, SimdVector3D *restrict force) override;
	};


	//# \class	FluidForce		Exerts fluid buoyancy and drag forces on rigid bodies inside a field.
	//
	//# The $FluidForce$ class exerts fluid buoyancy and drag forces on rigid bodies inside a field.
	//
	//# \def	class FluidForce : public Force
	//
	//# \ctor	FluidForce();
	//
	//# \desc
	//# The $FluidForce$ class represents a fluid force that produces buoyancy and drag inside a field. When a rigid body
	//# intersects a field to which a $FluidForce$ object is assigned, the fluid force determines exactly what volume of
	//# the rigid body intersects the field and uses that volume and the densities of the shapes composing the rigid body
	//# to calculate a buoyant force. A drag force is also calculated based on the submerged volume, the rigid body's
	//# drag multiplier, and the difference between the rigid body's velocity and the fluid's current velocity.
	//
	//# \base	Force		A fluid force is a specific type of force.
	//
	//# \also	$@BodyController::GetFluidDragMultiplier@$
	//# \also	$@BodyController::SetFluidDragMultiplier@$


	class FluidForce : public Force
	{
		private:

			float				fluidDensity;
			float				linearDrag;
			float				angularDrag;
			Vector3D			fluidCurrent;

			ConnectorKey		waterConnectorKey;
			const WaterBlock	*waterBlock;

			FluidForce(const FluidForce& fluidForce);

			Force *Replicate(void) const override;

		public:

			C4API FluidForce();
			C4API ~FluidForce();

			float GetFluidDensity(void) const
			{
				return (fluidDensity);
			}

			void SetFluidDensity(float density)
			{
				fluidDensity = density;
			}

			float GetLinearDrag(void) const
			{
				return (linearDrag);
			}

			void SetLinearDrag(float drag)
			{
				linearDrag = drag;
			}

			float GetAngularDrag(void) const
			{
				return (angularDrag);
			}

			void SetAngularDrag(float drag)
			{
				angularDrag = drag;
			}

			const Vector3D& GetFluidCurrent(void) const
			{
				return (fluidCurrent);
			}

			void SetFluidCurrent(const Vector3D& current)
			{
				fluidCurrent = current;
			}

			const ConnectorKey& GetWaterConnectorKey(void) const
			{
				return (waterConnectorKey);
			}

			void SetWaterConnectorKey(const ConnectorKey& key)
			{
				waterConnectorKey = key;
			}

			static bool ValidField(const Field *field);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Preprocess(void) override;

			bool ApplyForce(RigidBodyController *rigidBody, const Transform4D& worldTransform, Vector3D *restrict force, Antivector3D *restrict torque) override;
			void ApplyForce(DeformableBodyController *deformableBody, const Transformable *transformable, int32 count, const Point3D *position, const SimdVector3D *velocity, SimdVector3D *restrict force) override;
	};


	//# \class	WindForce		Exerts a wind drag force on rigid bodies inside a field.
	//
	//# The $WindForce$ class exerts a wind drag force on rigid bodies inside a field.
	//
	//# \def	class WindForce : public Force
	//
	//# \ctor	WindForce();
	//
	//# \desc
	//# The $WindForce$ class represents a wind force that produces drag inside a field. When a rigid body
	//# intersects a field to which a $WindForce$ object is assigned, the wind force calculates a drag force
	//# based on the volume of the rigid body, the rigid body's drag multiplier, and the difference between the
	//# rigid body's velocity and the wind velocity.
	//
	//# \base	Force		A wind force is a specific type of force.
	//
	//# \also	$@BodyController::GetWindDragMultiplier@$
	//# \also	$@BodyController::SetWindDragMultiplier@$


	class WindForce : public Force
	{
		private:

			Vector3D	windVelocity;
			float		windDrag;

			WindForce(const WindForce& windForce);

			Force *Replicate(void) const override;

		public:

			C4API WindForce();
			C4API ~WindForce();

			const Vector3D& GetWindVelocity(void) const
			{
				return (windVelocity);
			}

			void SetWindVelocity(const Vector3D& velocity)
			{
				windVelocity = velocity;
			}

			float GetWindDrag(void) const
			{
				return (windDrag);
			}

			void SetWindDrag(float drag)
			{
				windDrag = drag;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			bool ApplyForce(RigidBodyController *rigidBody, const Transform4D& worldTransform, Vector3D *restrict force, Antivector3D *restrict torque) override;
			void ApplyForce(DeformableBodyController *deformableBody, const Transformable *transformable, int32 count, const Point3D *position, const SimdVector3D *velocity, SimdVector3D *restrict force) override;
	};
}

#endif

// ZYUQURM
