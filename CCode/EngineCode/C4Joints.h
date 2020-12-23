 

#ifndef C4Joints_h
#define C4Joints_h


//# \component	Physics Manager
//# \prefix		PhysicsMgr/


#include "C4Contacts.h"
#include "C4Node.h"


namespace C4
{
	typedef Type	JointType;


	enum : ObjectType
	{
		kObjectJoint			= 'JONT'
	};


	//# \enum	JointType

	enum : JointType
	{
		kJointSpherical			= 'SPHR',		//## Spherical joint.
		kJointUniversal			= 'UNIV',		//## Universal joint.
		kJointDiscal			= 'DISC',		//## Discal joint.
		kJointRevolute			= 'RVLT',		//## Revolute joint.
		kJointCylindrical		= 'CYLD',		//## Cylindrical joint.
		kJointPrismatic			= 'PRSM'		//## Prismatic joint.
	};


	//# \enum	JointFlags

	enum
	{
		kJointBreakable					= 1 << 0,		//## The joint can be broken if enough force is applied.
		kJointConstrainTwistPosition	= 1 << 1,		//## The twist positions of bodies connected to the joint are constrained (universal joint only).
		kJointLimitDistance				= 1 << 2,		//## The joint enforces a limited translation distance (cylindrical and prismatic joints only).
		kJointLimitAngle				= 1 << 3		//## The joint enforces a limited rotation angle (all joints except prismatic).
	};


	C4API extern const char kConnectorKeyBody1[];
	C4API extern const char kConnectorKeyBody2[];


	//# \class	JointObject		Encapsulates data pertaining to a physics joint.
	//
	//# The $JointObject$ class encapsulates data pertaining to a physics joint.
	//
	//# \def	class JointObject : public Object
	//
	//# \ctor	JointObject(JointType type);
	//
	//# The constructor has protected access. The $JointObject$ class can only exist as the base class for another class.
	//
	//# \param	type	The type of the joint. See below for a list of possible types.
	//
	//# \desc
	//# The $JointObject$ class encapsulates data describing a joint. A joint can be used to connect a rigid body
	//# to another rigid body or to static geometry, and the type of joint determines what kind of motion is allowed
	//# for the connected rigid bodies.
	//#
	//# A joint object can be of one of the following types.
	//
	//# \table	JointType
	//
	//# \base	WorldMgr/Object		A $JointObject$ is an object that can be shared by multiple joint nodes.
	//
	//# \also	$@Joint@$
	//# \also	$@RigidBodyController@$


	//# \function	JointObject::GetJointType		Returns the specific type of a joint.
	//
	//# \proto	JointType GetJointType(void) const;
	//
	//# \desc
	//# The $GetJointType$ function returns the specific joint type, which can be one of the following constants.
	//
	//# \table	JointType


	//# \function	JointObject::GetJointFlags		Returns the joint flags.
	//
	//# \proto	unsigned_int32 GetJointFlags(void) const;
	//
	//# \desc
	//# The $GetJointFlags$ function returns the joint flags, which can be a combination
	//# (through logical OR) of the following constants.
	//
	//# \table	JointFlags
	//
	//# \also	$@JointObject::SetJointFlags@$

 
	//# \function	JointObject::SetJointFlags		Sets the joint flags.
	//
	//# \proto	void SetJointFlags(unsigned_int32 flags); 
	//
	//# \param	flags	The new joint flags. See below for possible values. 
	//
	//# \desc
	//# The $SetJointFlags$ function sets the joint flags. The $flags$ parameter can be a combination 
	//# (through logical OR) of the following constants.
	// 
	//# \table	JointFlags 
	//
	//# The initial value of the joint flags is 0.
	//
	//# \also	$@JointObject::GetJointFlags@$ 


	//# \function	JointObject::GetBreakingForce	Returns the breaking force for a joint.
	//
	//# \proto	float GetBreakingForce(void) const;
	//
	//# \desc
	//# The $GetBreakingForce$ function returns the breaking force for a joint, measured in kilonewtons (kN).
	//# The breaking force is only used if the $kJointBreakable$ flag is set for the joint.
	//
	//# \also	$@JointObject::SetBreakingForce@$
	//# \also	$@JointObject::GetJointFlags@$
	//# \also	$@JointObject::SetJointFlags@$


	//# \function	JointObject::SetBreakingForce	Sets the breaking force for a joint.
	//
	//# \proto	void SetBreakingForce(float force);
	//
	//# \param	force	The new breaking force.
	//
	//# \desc
	//# The $SetBreakingForce$ function sets the breaking force for a joint, measured in kilonewtons (kN).
	//# The breaking force is only used if the $kJointBreakable$ flag is set for the joint.
	//#
	//# The initial value of the breaking force is 0, so it should be explicitly set to a positive value at the
	//# time when a joint is made breakable. Otherwise, the joint will tend to fall apart on the first simulation step.
	//
	//# \also	$@JointObject::GetBreakingForce@$
	//# \also	$@JointObject::GetJointFlags@$
	//# \also	$@JointObject::SetJointFlags@$


	class JointObject : public Object
	{
		friend class WorldMgr;

		private:

			JointType			jointType;
			unsigned_int32		jointFlags;

			int32				solverMultiplier;
			float				breakingForce;
			float				distanceLimit;
			float				angleLimit;

			static JointObject *Create(Unpacker& data, unsigned_int32 unpackFlags);

		protected:

			JointObject(JointType type);
			~JointObject();

		public:

			JointType GetJointType(void) const
			{
				return (jointType);
			}

			unsigned_int32 GetJointFlags(void) const
			{
				return (jointFlags);
			}

			void SetJointFlags(unsigned_int32 flags)
			{
				jointFlags = flags;
			}

			int32 GetConstraintSolverMultiplier(void) const
			{
				return (solverMultiplier);
			}

			void SetConstraintSolverMultiplier(int32 multiplier)
			{
				solverMultiplier = multiplier;
			}

			float GetBreakingForce(void) const
			{
				return (breakingForce);
			}

			void SetBreakingForce(float force)
			{
				breakingForce = force;
			}

			float GetDistanceLimit(void) const
			{
				return (distanceLimit);
			}

			void SetDistanceLimit(float limit)
			{
				distanceLimit = limit;
			}

			float GetAngleLimit(void) const
			{
				return (angleLimit);
			}

			void SetAngleLimit(float limit)
			{
				angleLimit = limit;
			}

			void PackType(Packer& data) const override;
			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetCategoryCount(void) const override;
			Type GetCategoryType(int32 index, const char **title) const override;
			int32 GetCategorySettingCount(Type category) const override;
			Setting *GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const override;
			void SetCategorySetting(Type category, const Setting *setting) override;
	};


	//# \class	SphericalJointObject	Encapsulates data pertaining to a spherical joint.
	//
	//# The $SphericalJointObject$ class encapsulates data pertaining to a spherical joint.
	//
	//# \def	class SphericalJointObject : public JointObject
	//
	//# \ctor	SphericalJointObject();
	//
	//# \desc
	//# The $SphericalJointObject$ class encapsulates a physics joint that allows rotation about all three axes.
	//# See the $@SphericalJoint@$ class for a description of the joint kinematics.
	//
	//# \base	JointObject		A $SphericalJointObject$ is an object that can be shared by multiple spherical joint nodes.
	//
	//# \also	$@SphericalJoint@$


	class SphericalJointObject : public JointObject
	{
		public:

			SphericalJointObject(JointType type);
			~SphericalJointObject();

		public:

			SphericalJointObject();

			int32 GetCategorySettingCount(Type category) const override;
			Setting *GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const override;
			void SetCategorySetting(Type category, const Setting *setting) override;
	};


	//# \class	UniversalJointObject	Encapsulates data pertaining to a universal joint.
	//
	//# The $UniversalJointObject$ class encapsulates data pertaining to a universal joint.
	//
	//# \def	class UniversalJointObject final : public SphericalJointObject
	//
	//# \ctor	UniversalJointObject();
	//
	//# \desc
	//# The $UniversalJointObject$ class encapsulates a physics joint that allows rotation about the two non-radial axes.
	//# See the $@UniversalJoint@$ class for a description of the joint kinematics.
	//
	//# \base	SphericalJointObject	A $UniversalJointObject$ is an object that can be shared by multiple universal joint nodes.
	//
	//# \also	$@UniversalJoint@$


	class UniversalJointObject final : public SphericalJointObject
	{
		private:

			~UniversalJointObject();

		public:

			UniversalJointObject();

			int32 GetCategorySettingCount(Type category) const override;
			Setting *GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const override;
			void SetCategorySetting(Type category, const Setting *setting) override;
	};


	//# \class	DiscalJointObject	Encapsulates data pertaining to a discal joint.
	//
	//# The $DiscalJointObject$ class encapsulates data pertaining to a discal joint.
	//
	//# \def	class DiscalJointObject : public JointObject
	//
	//# \ctor	DiscalJointObject();
	//
	//# \desc
	//# The $DiscalJointObject$ class encapsulates a physics joint that allows rotation about the radial axis and one non-radial axis.
	//# See the $@DiscalJoint@$ class for a description of the joint kinematics.
	//
	//# \base	JointObject		A $DiscalJointObject$ is an object that can be shared by multiple discal joint nodes.
	//
	//# \also	$@DiscalJoint@$


	class DiscalJointObject : public JointObject
	{
		protected:

			DiscalJointObject(JointType type);
			~DiscalJointObject();

		public:

			DiscalJointObject();

			int32 GetCategorySettingCount(Type category) const override;
			Setting *GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const override;
			void SetCategorySetting(Type category, const Setting *setting) override;
	};


	//# \class	RevoluteJointObject		Encapsulates data pertaining to a revolute joint.
	//
	//# The $RevoluteJointObject$ class encapsulates data pertaining to a revolute joint.
	//
	//# \def	class RevoluteJointObject final : public DiscalJointObject
	//
	//# \ctor	RevoluteJointObject();
	//
	//# \desc
	//# The $RevoluteJointObject$ class encapsulates a physics joint that allows rotation about a single axis.
	//# See the $@RevoluteJoint@$ class for a description of the joint kinematics.
	//
	//# \base	DiscalJointObject		A $RevoluteJointObject$ is an object that can be shared by multiple revolute joint nodes.
	//
	//# \also	$@RevoluteJoint@$


	class RevoluteJointObject final : public DiscalJointObject
	{
		private:

			~RevoluteJointObject();

		public:

			RevoluteJointObject();
	};


	//# \class	CylindricalJointObject		Encapsulates data pertaining to a cylindrical joint.
	//
	//# The $CylindricalJointObject$ class encapsulates data pertaining to a cylindrical joint.
	//
	//# \def	class CylindricalJointObject : public DiscalJointObject
	//
	//# \ctor	CylindricalJointObject();
	//
	//# \desc
	//# The $CylindricalJointObject$ class encapsulates a physics joint that allows rotation about a single axis and translation along the same axis.
	//# See the $@CylindricalJoint@$ class for a description of the joint kinematics.
	//
	//# \base	DiscalJointObject		A $CylindricalJointObject$ is an object that can be shared by multiple cylindrical joint nodes.
	//
	//# \also	$@CylindricalJoint@$


	class CylindricalJointObject : public DiscalJointObject
	{
		protected:

			CylindricalJointObject(JointType type);
			~CylindricalJointObject();

		public:

			CylindricalJointObject();

			int32 GetCategorySettingCount(Type category) const override;
			Setting *GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const override;
			void SetCategorySetting(Type category, const Setting *setting) override;
	};


	//# \class	PrismaticJointObject	Encapsulates data pertaining to a prismatic joint.
	//
	//# The $PrismaticJointObject$ class encapsulates data pertaining to a prismatic joint.
	//
	//# \def	class PrismaticJointObject final : public CylindricalJointObject
	//
	//# \ctor	PrismaticJointObject();
	//
	//# \desc
	//# The $PrismaticJointObject$ class encapsulates a physics joint that allows translation along a single axis.
	//# See the $@PrismaticJoint@$ class for a description of the joint kinematics.
	//
	//# \base	CylindricalJointObject		A $PrismaticJointObject$ is an object that can be shared by multiple prismatic joint nodes.
	//
	//# \also	$@PrismaticJoint@$


	class PrismaticJointObject final : public CylindricalJointObject
	{
		private:

			~PrismaticJointObject();

		public:

			PrismaticJointObject();

			int32 GetCategorySettingCount(Type category) const override;
			Setting *GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const override;
			void SetCategorySetting(Type category, const Setting *setting) override;
	};


	//# \class	Joint		Represents a joint node in a world.
	//
	//# The $Joint$ class represents a joint node in a world.
	//
	//# \def	class Joint : public Node
	//
	//# \ctor	Joint(JointType type);
	//
	//# The constructor has protected access. A $Joint$ class can only exist as the base class for a more specific type of joint.
	//
	//# \param	type	The type of the joint. See below for a list of possible types.
	//
	//# \desc
	//# The $Joint$ class represents a joint node in a world. A joint can be used to connect a rigid body
	//# to another rigid body or to static geometry, and the type of joint determines what kind of motion is allowed
	//# for the connected rigid bodies.
	//#
	//# A joint node can be of one of the following types.
	//
	//# \table	JointType
	//
	//# \base	WorldMgr/Node		A $Joint$ node is a scene graph node.
	//
	//# \also	$@JointObject@$
	//# \also	$@RigidBodyController@$


	//# \function	Joint::GetJointType		Returns the specific type of a joint.
	//
	//# \proto	JointType GetJointType(void) const;
	//
	//# \desc
	//# The $GetJointType$ function returns the specific joint type, which can be one of the following constants.
	//
	//# \table	JointType


	//# \function	Joint::GetFirstConnectedRigidBody		Returns the first rigid body connected to a joint.
	//
	//# \proto	Node *GetFirstConnectedRigidBody(void) const;
	//
	//# \desc
	//# The $GetFirstConnectedRigidBody$ function returns the first rigid body connected to a joint.
	//# If there is no first rigid body connected to the joint, then the return value is $nullptr$.
	//
	//# \also	$@Joint::SetFirstConnectedRigidBody@$
	//# \also	$@Joint::GetSecondConnectedRigidBody@$
	//# \also	$@Joint::SetSecondConnectedRigidBody@$
	//# \also	$@Joint::BreakJoint@$


	//# \function	Joint::SetFirstConnectedRigidBody		Sets the first rigid body connected to a joint.
	//
	//# \proto	void SetFirstConnectedRigidBody(Node *node);
	//
	//# \param	node	The node representing the first rigid body.
	//
	//# \desc
	//# The $SetFirstConnectedRigidBody$ function sets the first rigid body connected to a joint to the node specified
	//# by the $node$ parameter. This node must have a rigid body controller assigned to it in order to be affected
	//# by the joint.
	//#
	//# The first rigid body connector is initially set to $nullptr$.
	//
	//# \also	$@Joint::GetFirstConnectedRigidBody@$
	//# \also	$@Joint::GetSecondConnectedRigidBody@$
	//# \also	$@Joint::SetSecondConnectedRigidBody@$
	//# \also	$@Joint::BreakJoint@$


	//# \function	Joint::GetSecondConnectedRigidBody		Returns the second rigid body connected to a joint.
	//
	//# \proto	Node *GetSecondConnectedRigidBody(void) const;
	//
	//# \desc
	//# The $GetSecondConnectedRigidBody$ function returns the second rigid body connected to a joint.
	//# If there is no second rigid body connected to the joint, then the return value is $nullptr$.
	//
	//# \also	$@Joint::SetSecondConnectedRigidBody@$
	//# \also	$@Joint::GetFirstConnectedRigidBody@$
	//# \also	$@Joint::SetFirstConnectedRigidBody@$
	//# \also	$@Joint::BreakJoint@$


	//# \function	Joint::SetSecondConnectedRigidBody		Sets the second rigid body connected to a joint.
	//
	//# \proto	void SetSecondConnectedRigidBody(Node *node);
	//
	//# \param	node	The node representing the second rigid body.
	//
	//# \desc
	//# The $SetSecondConnectedRigidBody$ function sets the second rigid body connected to a joint to the node specified
	//# by the $node$ parameter. This node must have a rigid body controller assigned to it in order to be affected
	//# by the joint.
	//#
	//# If the first rigid body connector is $nullptr$, then the second rigid body connector is ignored. When connecting
	//# only one node to a joint, the first rigid body connector should always be used.
	//#
	//# The second rigid body connector is initially set to $nullptr$.
	//
	//# \also	$@Joint::GetSecondConnectedRigidBody@$
	//# \also	$@Joint::GetFirstConnectedRigidBody@$
	//# \also	$@Joint::SetFirstConnectedRigidBody@$
	//# \also	$@Joint::BreakJoint@$


	//# \function	Joint::BreakJoint		Breaks a joint's connections.
	//
	//# \proto	void BreakJoint(void);
	//
	//# \desc
	//# The $BreakJoint$ function breaks the connections to the first and second rigid bodies influenced by a joint,
	//# if they exist, and deletes the internal contact used by the Physics Manager. The built-in connectors belonging
	//# to the joint node continue to exist, but they are no longer linked to other nodes after this function returns.
	//# Any rigid bodies that were connected to the joint are awakened, and this process continues recursively through
	//# all bodies reachable in the contact graph.
	//
	//# \also	$@Joint::GetFirstConnectedRigidBody@$
	//# \also	$@Joint::GetSecondConnectedRigidBody@$
	//# \also	$@Joint::GetFirstConnectedRigidBody@$
	//# \also	$@Joint::SetFirstConnectedRigidBody@$


	class Joint : public Node
	{
		friend class Node;
		friend class JointContact;
		friend class StaticJointContact;

		private:

			JointType				jointType;

			float					breakingImpulse;
			float					distanceLimit;
			Vector2D				angleLimitCosSin;

			bool					initializedFlag;
			bool					constrainTwistFlag;
			bool					limitDistanceFlag;
			bool					limitAngleFlag;

			PhysicsController		*physicsController;

			Link<JointContact>		jointContact;
			Transform4D				bodyJointTransform[2];

			static Joint *Create(Unpacker& data, unsigned_int32 unpackFlags);

			RigidBodyController *GetRigidBody(const Node *node, Transform4D *jointTransform) const;

		protected:

			Joint(JointType type);
			Joint(const Joint& joint);

			float GetBreakingImpulse(void) const
			{
				return (breakingImpulse);
			}

			float GetDistanceLimit(void) const
			{
				return (distanceLimit);
			}

			const Vector2D& GetAngleLimitCosSin(void) const
			{
				return (angleLimitCosSin);
			}

			bool GetConstrainTwistFlag(void) const
			{
				return (constrainTwistFlag);
			}

			bool GetLimitDistanceFlag(void) const
			{
				return (limitDistanceFlag);
			}

			bool GetLimitAngleFlag(void) const
			{
				return (limitAngleFlag);
			}

			JointContact *GetJointContact(void) const
			{
				return (jointContact);
			}

			const Point3D& GetBodyJointPosition(int32 index) const
			{
				return (bodyJointTransform[index].GetTranslation());
			}

			const Vector3D& GetBodyJointTangent(int32 index) const
			{
				return (bodyJointTransform[index][0]);
			}

			const Vector3D& GetBodyJointBitangent(int32 index) const
			{
				return (bodyJointTransform[index][1]);
			}

			const Vector3D& GetBodyJointAxis(int32 index) const
			{
				return (bodyJointTransform[index][2]);
			}

			void HandleBrokenJoint(void);

		public:

			virtual ~Joint();

			JointType GetJointType(void) const
			{
				return (jointType);
			}

			JointObject *GetObject(void) const
			{
				return (static_cast<JointObject *>(Node::GetObject()));
			}

			PhysicsController *GetPhysicsController(void) const
			{
				return (physicsController);
			}

			Node *GetFirstConnectedRigidBody(void) const
			{
				return (GetConnectedNode(kConnectorKeyBody1));
			}

			Node *GetSecondConnectedRigidBody(void) const
			{
				return (GetConnectedNode(kConnectorKeyBody2));
			}

			void PackType(Packer& data) const override;
			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetInternalConnectorCount(void) const override;
			const char *GetInternalConnectorKey(int32 index) const override;
			bool ValidConnectedNode(const ConnectorKey& key, const Node *node) const override;
			C4API void SetFirstConnectedRigidBody(Node *node);
			C4API void SetSecondConnectedRigidBody(Node *node);

			void Preprocess(void) override;
			void Neutralize(void) override;

			C4API void BreakJoint(void);

			virtual void ApplyStaticVelocityConstraints(ConstraintContact *contact) = 0;
			virtual void ApplyStaticPositionConstraints(ConstraintContact *contact) = 0;
			virtual void ApplyVelocityConstraints(ConstraintContact *contact) = 0;
			virtual void ApplyPositionConstraints(ConstraintContact *contact) = 0;
	};


	//# \class	SphericalJoint		Represents a spherical joint node in a world.
	//
	//# The $SphericalJoint$ class represents a spherical joint node in a world.
	//
	//# \def	class SphericalJoint : public Joint
	//
	//# \ctor	SphericalJoint();
	//
	//# \desc
	//# The $SphericalJoint$ class represents a spherical joint node in a world. A spherical joint
	//# has three rotational degrees of freedom.
	//#
	//# When a spherical joint is used to connect a rigid body to static geometry, it allows rotation
	//# about any axis, but maintains a constant distance between the rigid body and the joint position.
	//#
	//# When a spherical joint is used to connect two rigid bodies, the original joint position is saved
	//# in node-space coordinates for both rigid bodies. The rigid bodies are allowed to rotate about any axis,
	//# but the two saved joint positions are required to coincide in world-space, maintaining the original
	//# distances between the rigid bodies and the original joint position.
	//
	//# \base	Joint		A spherical joint is a specific type of joint.
	//
	//# \also	$@SphericalJointObject@$


	class SphericalJoint : public Joint
	{
		friend class Joint;

		private:

			Node *Replicate(void) const override;

		protected:

			enum
			{
				kImpulseDelta,
				kImpulseTwist,
				kImpulseAngleLimit
			};

			SphericalJoint(void *);
			SphericalJoint(JointType type);
			SphericalJoint(const SphericalJoint& sphericalJoint);

			bool GetAngleLimitStaticVelocityConstraint(ConstraintContact *contact, RigidBodyController *rigidBody, const Vector3D& jointDirection, StaticConstraint *constraint) const;
			void ApplyAngleLimitStaticPositionConstraint(ConstraintContact *contact, RigidBodyController *rigidBody, const Vector3D& jointDirection);
			bool GetAngleLimitVelocityConstraint(ConstraintContact *contact, RigidBodyController *rigidBody1, RigidBodyController *rigidBody2, const Vector3D& jointDirection1, const Vector3D& jointDirection2, Constraint *constraint) const;
			void ApplyAngleLimitPositionConstraint(ConstraintContact *contact, RigidBodyController *rigidBody1, RigidBodyController *rigidBody2, const Vector3D& jointDirection1, const Vector3D& jointDirection2);

		public:

			C4API SphericalJoint();
			C4API ~SphericalJoint();

			SphericalJointObject *GetObject(void) const
			{
				return (static_cast<SphericalJointObject *>(Node::GetObject()));
			}

			void ApplyStaticVelocityConstraints(ConstraintContact *contact) override;
			void ApplyStaticPositionConstraints(ConstraintContact *contact) override;
			void ApplyVelocityConstraints(ConstraintContact *contact) override;
			void ApplyPositionConstraints(ConstraintContact *contact) override;
	};


	//# \class	UniversalJoint		Represents a universal joint node in a world.
	//
	//# The $UniversalJoint$ class represents a universal joint node in a world.
	//
	//# \def	class UniversalJoint final : public SphericalJoint
	//
	//# \ctor	UniversalJoint();
	//
	//# \desc
	//# The $UniversalJoint$ class represents a universal joint node in a world. A universal joint
	//# has two rotational degrees of freedom.
	//#
	//# When a universal joint is used to connect a rigid body to static geometry, it allows rotation
	//# about the two non-radial axes and maintains a constant distance between the rigid body and the
	//# joint position.
	//#
	//# When a universal joint is used to connect two rigid bodies, the original joint position is saved
	//# in node-space coordinates for both rigid bodies. The rigid bodies are allowed to rotate about the
	//# two non-radial axes, and the two saved joint positions are required to coincide in world-space,
	//# maintaining the original distances between the rigid bodies and the original joint position.
	//
	//# \base	SphericalJoint		A universal joint is a specific type of spherical joint.
	//
	//# \also	$@UniversalJointObject@$


	class UniversalJoint final : public SphericalJoint
	{
		friend class Joint;

		private:

			UniversalJoint(void *);
			UniversalJoint(const UniversalJoint& universalJoint);

			Node *Replicate(void) const override;

		public:

			C4API UniversalJoint();
			C4API ~UniversalJoint();

			UniversalJointObject *GetObject(void) const
			{
				return (static_cast<UniversalJointObject *>(Node::GetObject()));
			}

			void ApplyStaticVelocityConstraints(ConstraintContact *contact) override;
			void ApplyStaticPositionConstraints(ConstraintContact *contact) override;
			void ApplyVelocityConstraints(ConstraintContact *contact) override;
			void ApplyPositionConstraints(ConstraintContact *contact) override;
	};


	//# \class	DiscalJoint		Represents a discal joint node in a world.
	//
	//# The $DiscalJoint$ class represents a discal joint node in a world.
	//
	//# \def	class DiscalJoint : public Joint
	//
	//# \ctor	DiscalJoint();
	//
	//# \desc
	//# The $DiscalJoint$ class represents a discal joint node in a world. A discal joint has two
	//# rotational degrees of freedom.
	//#
	//# When a discal joint is used to connect a rigid body to static geometry, it allows rotation
	//# about the joint's <i>z</i> axis and the radial axis while maintaining a constant distance
	//# between the rigid body and the joint position.
	//#
	//# When a discal joint is used to connect two rigid bodies, it behaves as a spherical joint.
	//
	//# \base	Joint		A discal joint is a specific type of joint.
	//
	//# \also	$@DiscalJointObject@$


	class DiscalJoint : public Joint
	{
		friend class Joint;

		private:

			Node *Replicate(void) const override;

		protected:

			enum
			{
				kImpulseDelta,
				kImpulseRotationX,
				kImpulseRotationY,
				kImpulseAngleLimit
			};

			DiscalJoint(void *);
			DiscalJoint(JointType type);
			DiscalJoint(const DiscalJoint& discalJoint);

			bool GetAngleLimitStaticVelocityConstraint(ConstraintContact *contact, RigidBodyController *rigidBody, const Vector3D& jointDirection, StaticConstraint *constraint) const;
			void ApplyAngleLimitStaticPositionConstraint(ConstraintContact *contact, RigidBodyController *rigidBody, const Vector3D& jointDirection);
			bool GetAngleLimitVelocityConstraint(ConstraintContact *contact, RigidBodyController *rigidBody1, RigidBodyController *rigidBody2, const Vector3D& jointDirection1, const Vector3D& jointDirection2, Constraint *constraint) const;
			void ApplyAngleLimitPositionConstraint(ConstraintContact *contact, RigidBodyController *rigidBody1, RigidBodyController *rigidBody2, const Vector3D& jointDirection1, const Vector3D& jointDirection2);

		public:

			C4API DiscalJoint();
			C4API ~DiscalJoint();

			DiscalJointObject *GetObject(void) const
			{
				return (static_cast<DiscalJointObject *>(Node::GetObject()));
			}

			void ApplyStaticVelocityConstraints(ConstraintContact *contact) override;
			void ApplyStaticPositionConstraints(ConstraintContact *contact) override;
			void ApplyVelocityConstraints(ConstraintContact *contact) override;
			void ApplyPositionConstraints(ConstraintContact *contact) override;
	};


	//# \class	RevoluteJoint		Represents a revolute joint node in a world.
	//
	//# The $RevoluteJoint$ class represents a revolute joint node in a world.
	//
	//# \def	class RevoluteJoint final : public DiscalJoint
	//
	//# \ctor	RevoluteJoint();
	//
	//# \desc
	//# The $RevoluteJoint$ class represents a revolute joint node in a world. A revolute joint
	//# has one rotational degree of freedom and behaves like a hinge.
	//#
	//# When a revolute joint is used to connect a rigid body to static geometry, it allows rotation
	//# about the joint's <i>z</i> axis and maintains a constant distance between the rigid body and
	//# the joint position.
	//#
	//# When a revolute joint is used to connect two rigid bodies, the original joint position and <i>z</i>
	//# axis direction are saved in node-space coordinates for both rigid bodies. The rigid bodies are allowed
	//# to rotate about the <i>z</i> axis, and the two saved joint positions and two saved <i>z</i> axis
	//# directions are required to coincide in world-space, maintaining the original distances and orientation
	//# between the rigid bodies and the original joint configuration.
	//
	//# \base	DiscalJoint		A revolute joint is a specific type of discal joint.
	//
	//# \also	$@RevoluteJointObject@$


	class RevoluteJoint final : public DiscalJoint
	{
		friend class Joint;

		private:

			RevoluteJoint(void *);
			RevoluteJoint(const RevoluteJoint& revoluteJoint);

			Node *Replicate(void) const override;

		public:

			C4API RevoluteJoint();
			C4API ~RevoluteJoint();

			RevoluteJointObject *GetObject(void) const
			{
				return (static_cast<RevoluteJointObject *>(Node::GetObject()));
			}

			void ApplyStaticVelocityConstraints(ConstraintContact *contact) override;
			void ApplyStaticPositionConstraints(ConstraintContact *contact) override;
			void ApplyVelocityConstraints(ConstraintContact *contact) override;
			void ApplyPositionConstraints(ConstraintContact *contact) override;
	};


	//# \class	CylindricalJoint	Represents a cylindrical joint node in a world.
	//
	//# The $CylindricalJoint$ class represents a cylindrical joint node in a world.
	//
	//# \def	class CylindricalJoint : public DiscalJoint
	//
	//# \ctor	CylindricalJoint();
	//
	//# \desc
	//# The $CylindricalJoint$ class represents a cylindrical joint node in a world. A cylindrical joint
	//# has one translational degree of freedom and one rotational degree of freedom. It behaves like a
	//# piston that allows rotation about the translational axis.
	//#
	//# When a cylindrical joint is used to connect a rigid body to static geometry, it allows only
	//# translation along the joint's <i>z</i> axis and rotation about the joint's <i>z</i> axis.
	//#
	//# When a cylindrical joint is used to connect two rigid bodies, the original <i>z</i> axis direction
	//# is saved in node-space coordinates for both rigid bodies. The rigid bodies are allowed to translate
	//# along the <i>z</i> axis and rotate about the <i>z</i> axis, and the two saved <i>z</i> axis
	//# directions are required to coincide in world-space, maintaining the original orientation between
	//# the rigid bodies and the original joint configuration.
	//
	//# \base	DiscalJoint		A cylindrical joint is a specific type of discal joint.
	//
	//# \also	$@CylindricalJointObject@$


	class CylindricalJoint : public DiscalJoint
	{
		friend class Joint;

		private:

			Node *Replicate(void) const override;

		protected:

			enum
			{
				kImpulseDelta,
				kImpulseRotationX,
				kImpulseRotationY,
				kImpulseRotationZ,
				kImpulseDistanceLimit,
				kImpulseAngleLimit
			};

			CylindricalJoint(void *);
			CylindricalJoint(JointType type);
			CylindricalJoint(const CylindricalJoint& cylindricalJoint);

			void ApplyDistanceLimitStaticVelocityConstraint(ConstraintContact *contact, RigidBodyController *rigidBody, const Vector3D& deltaPosition);
			void ApplyDistanceLimitStaticPositionConstraint(ConstraintContact *contact, RigidBodyController *rigidBody, const Vector3D& deltaPosition);
			void ApplyDistanceLimitVelocityConstraint(ConstraintContact *contact, RigidBodyController *rigidBody1, RigidBodyController *rigidBody2, const Vector3D& zdir, const Vector3D& deltaPosition);
			void ApplyDistanceLimitPositionConstraint(ConstraintContact *contact, RigidBodyController *rigidBody1, RigidBodyController *rigidBody2, const Vector3D& zdir, const Vector3D& deltaPosition);

		public:

			C4API CylindricalJoint();
			C4API ~CylindricalJoint();

			CylindricalJointObject *GetObject(void) const
			{
				return (static_cast<CylindricalJointObject *>(Node::GetObject()));
			}

			void ApplyStaticVelocityConstraints(ConstraintContact *contact) override;
			void ApplyStaticPositionConstraints(ConstraintContact *contact) override;
			void ApplyVelocityConstraints(ConstraintContact *contact) override;
			void ApplyPositionConstraints(ConstraintContact *contact) override;
	};


	//# \class	PrismaticJoint		Represents a prismatic joint node in a world.
	//
	//# The $PrismaticJoint$ class represents a prismatic joint node in a world.
	//
	//# \def	class PrismaticJoint final : public CylindricalJoint
	//
	//# \ctor	PrismaticJoint();
	//
	//# \desc
	//# The $PrismaticJoint$ class represents a prismatic joint node in a world. A prismatic joint
	//# has one translational degree of freedom and behaves like a non-rotating piston.
	//#
	//# When a prismatic joint is used to connect a rigid body to static geometry, it allows only
	//# translation along the joint's <i>z</i> axis.
	//#
	//# When a prismatic joint is used to connect two rigid bodies, the original <i>z</i> axis
	//# direction is saved in node-space coordinates for both rigid bodies. The rigid bodies are
	//# allowed to translate along the <i>z</i> axis, and the two saved <i>z</i> axis directions
	//# are required to coincide in world-space, maintaining the original orientation between the
	//# rigid bodies and the original joint configuration.
	//
	//# \base	CylindricalJoint		A prismatic joint is a specific type of cylindrical joint.
	//
	//# \also	$@PrismaticJointObject@$


	class PrismaticJoint final : public CylindricalJoint
	{
		friend class Joint;

		private:

			PrismaticJoint(void *);
			PrismaticJoint(const PrismaticJoint& prismaticJoint);

			Node *Replicate(void) const override;

		public:

			C4API PrismaticJoint();
			C4API ~PrismaticJoint();

			PrismaticJointObject *GetObject(void) const
			{
				return (static_cast<PrismaticJointObject *>(Node::GetObject()));
			}

			void ApplyStaticVelocityConstraints(ConstraintContact *contact) override;
			void ApplyStaticPositionConstraints(ConstraintContact *contact) override;
			void ApplyVelocityConstraints(ConstraintContact *contact) override;
			void ApplyPositionConstraints(ConstraintContact *contact) override;
	};
}


#endif

// ZYUQURM
