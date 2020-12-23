 

#ifndef C4Contacts_h
#define C4Contacts_h


//# \component	Physics Manager
//# \prefix		PhysicsMgr/


#include "C4Body.h"
#include "C4Inertia.h"

#if C4DIAGS

	#include "C4Renderable.h"

#endif


namespace C4
{
	typedef Type	ContactType;


	enum : ContactType
	{
		kContactRigidBody		= 'BODY',
		kContactGeometry		= 'GEOM',
		kContactDeformable		= 'DFRM',
		kContactJoint			= 'JONT',
		kContactStaticJoint		= 'SJNT'
	};


	enum
	{
		kContactNotificationOutgoing	= 1 << 0,
		kContactNotificationIncoming	= 1 << 1
	};


	class Joint;
	class Shape;
	class Geometry;
	class RigidBodyController;
	struct IntersectionData;

	#if C4DIAGS

		class ContactRenderable;

	#endif


	struct Jacobian
	{
		Vector3D		linear;
		Antivector3D	angular;

		Jacobian() = default;

		Jacobian(const Vector3D& lin, const Antivector3D& ang)
		{
			linear = lin;
			angular = ang;
		}

		Jacobian(float inverseMass, const InertiaTensor& inverseInertiaTensor, const Jacobian& jacobian)
		{
			linear = jacobian.linear * inverseMass;
			angular = inverseInertiaTensor * jacobian.angular;
		}

		Jacobian& Set(const Vector3D& lin, const Antivector3D& ang)
		{
			linear = lin;
			angular = ang;
			return (*this);
		}
	};

	inline float operator *(const Jacobian& j1, const Jacobian& j2)
	{
		return (j1.linear * j2.linear + j1.angular * j2.angular);
	}


	struct Constraint
	{
		Jacobian		row[2];
		float			bias;
		int32			index;
		Range<float>	range;
	};


	struct StaticConstraint
	{
		Jacobian		row;
		float			bias;
		int32			index;
		Range<float>	range; 
	};

 
	struct Subcontact
	{ 
		bool				activeFlag;

		int32				inactiveStepCount; 
		int32				lastUpdateStep;
 
		unsigned_int32		triangleIndex; 

		Point3D				alphaPosition;
		Antivector3D		alphaNormal;
		Vector3D			alphaTangent[2]; 

		Point3D				betaPosition;
		Antivector3D		betaNormal;
		Vector3D			betaTangent[2];

		float				impactSpeed;
		float				bounceSpeed;
	};


	class Contact : public GraphEdge<Body, Contact>, public Packable, public Memory<Contact>
	{
		private:

			ContactType			contactType;

			bool				enabledFlag;
			bool				deadFlag;

			unsigned_int32		notificationMask;
			float				contactParam;
			int32				solverMultiplier;

			static void StartBodyLinkProc(Node *node, void *cookie);
			static void FinishBodyLinkProc(Node *node, void *cookie);

		protected:

			Contact(ContactType type, Body *body1, Body *body2, unsigned_int32 mask = 0);

			void SetContactParam(float param)
			{
				contactParam = param;
			}

			void SetConstraintSolverMultiplier(int32 multiplier)
			{
				solverMultiplier = multiplier;
			}

		public:

			virtual ~Contact();

			ContactType GetContactType(void) const
			{
				return (contactType);
			}

			bool Enabled(void) const
			{
				return (enabledFlag);
			}

			void Enable(void)
			{
				enabledFlag = true;
			}

			void Disable(void)
			{
				enabledFlag = false;
			}

			bool Dead(void) const
			{
				return (deadFlag);
			}

			void Kill(void)
			{
				enabledFlag = false;
				deadFlag = true;
				notificationMask = 0;
			}

			unsigned_int32 GetNotificationMask(void) const
			{
				return (notificationMask);
			}

			bool TakeNotificationFlag(unsigned flag)
			{
				unsigned_int32 mask = notificationMask;
				notificationMask = mask & ~flag;
				return ((mask & flag) != 0);
			}

			float GetContactParam(void) const
			{
				return (contactParam);
			}

			int32 GetConstraintSolverMultiplier(void) const
			{
				return (solverMultiplier);
			}

			static Contact *Create(Unpacker& data, unsigned_int32 unpackFlags, Body *nullBody);

			void PackType(Packer& data) const override;
			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			virtual bool NonpersistentFinishNode(void) const;

			virtual void InitializeConstraints(void);
			virtual void InitializeRepeatConstraints(void);

			virtual void ApplyVelocityConstraints(void);
			virtual void ApplyFrictionConstraints(void);
			virtual void ApplyPositionConstraints(void);
	};


	class ConstraintContact : public Contact
	{
		friend class RigidBodyController;

		public:

			enum
			{
				kMaxSubcontactCount				= 4,
				kMaxContactConstraintCount		= kMaxSubcontactCount * 4
			};

		protected:

			float		cumulativeImpulse[kMaxContactConstraintCount];
			float		appliedImpulse[kMaxContactConstraintCount];

			ConstraintContact(ContactType type, Body *body1, Body *body2, unsigned_int32 mask = 0);

		public:

			~ConstraintContact();

			float GetAppliedImpulse(int32 index) const
			{
				return (appliedImpulse[index]);
			}

			void InitializeConstraints(void) override;
			void InitializeRepeatConstraints(void) override;

			static float CalculateVelocityImpulse(const RigidBodyController *rigidBody, const StaticConstraint *constraint);
			static Vector2D CalculateVelocityImpulse2D(const RigidBodyController *rigidBody, const StaticConstraint *constraint);
			static Vector3D CalculateVelocityImpulse3D(const RigidBodyController *rigidBody, const StaticConstraint *constraint);
			static Vector4D CalculateVelocityImpulse4D(const RigidBodyController *rigidBody, const StaticConstraint *constraint);

			static float CalculateVelocityImpulse(const RigidBodyController *rigidBody1, const RigidBodyController *rigidBody2, const Constraint *constraint);
			static Vector2D CalculateVelocityImpulse2D(const RigidBodyController *rigidBody1, const RigidBodyController *rigidBody2, const Constraint *constraint);
			static Vector3D CalculateVelocityImpulse3D(const RigidBodyController *rigidBody1, const RigidBodyController *rigidBody2, const Constraint *constraint);
			static Vector4D CalculateVelocityImpulse4D(const RigidBodyController *rigidBody1, const RigidBodyController *rigidBody2, const Constraint *constraint);

			static float CalculatePositionImpulse(const RigidBodyController *rigidBody, const StaticConstraint *constraint);
			static Vector2D CalculatePositionImpulse2D(const RigidBodyController *rigidBody, const StaticConstraint *constraint);
			static Vector3D CalculatePositionImpulse3D(const RigidBodyController *rigidBody, const StaticConstraint *constraint);
			static Vector4D CalculatePositionImpulse4D(const RigidBodyController *rigidBody, const StaticConstraint *constraint);

			static float CalculatePositionImpulse(const RigidBodyController *rigidBody1, const RigidBodyController *rigidBody2, const Constraint *constraint);
			static Vector2D CalculatePositionImpulse2D(const RigidBodyController *rigidBody1, const RigidBodyController *rigidBody2, const Constraint *constraint);
			static Vector3D CalculatePositionImpulse3D(const RigidBodyController *rigidBody1, const RigidBodyController *rigidBody2, const Constraint *constraint);
			static Vector4D CalculatePositionImpulse4D(const RigidBodyController *rigidBody1, const RigidBodyController *rigidBody2, const Constraint *constraint);

			float AccumulateConstraintImpulse(int32 index, float multiplier, const Range<float>& range);

			void SolveVelocityConstraints(RigidBodyController *rigidBody, int32 count, const StaticConstraint *constraint);
			void SolveVelocityConstraints(RigidBodyController *rigidBody1, RigidBodyController *rigidBody2, int32 count, const Constraint *constraint);
			static void SolvePositionConstraints(RigidBodyController *rigidBody, int32 count, const StaticConstraint *constraint);
			static void SolvePositionConstraints(RigidBodyController *rigidBody1, RigidBodyController *rigidBody2, int32 count, const Constraint *constraint);
	};


	class CollisionContact : public ConstraintContact
	{
		friend class Shape;

		private:

			int32				cachedSimplexVertexCount;
			Point3D				cachedAlphaVertex[4];
			Point3D				cachedBetaVertex[4];

		protected:

			int32				subcontactCount;
			Subcontact			subcontact[kMaxSubcontactCount];

			#if C4DIAGS

				Link<ContactRenderable>		contactVectorRenderable;
				Link<ContactRenderable>		contactPointRenderable;

			#endif

			CollisionContact(ContactType type, Body *body1, Body *body2, unsigned_int32 mask = 0);
			~CollisionContact();

			static float CalculateContactArea(const Point3D& p0, const Point3D& p1, const Point3D& p2, const Point3D& p3);

			#if C4DIAGS

				void BuildContactRenderables(const Node *node1, const Node *node2);

			#endif

		public:

			int32 GetSubcontactCount(void) const
			{
				return (subcontactCount);
			}

			const Subcontact *GetSubcontact(int32 index) const
			{
				return (&subcontact[index]);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
	};


	//# \class	GeometryContact		Represents a contact between a rigid body and a geometry.
	//
	//# The $GeometryContact$ class represents a contact between a rigid body and a geometry.
	//
	//# \def	class GeometryContact final : public CollisionContact
	//
	//# \desc
	//# The $GeometryContact$ class represents a contact between a single shape belonging to a
	//# rigid bodies and a geometry node. Instances of the $GeometryContact$ class are only created
	//# internally by the Physics Manager.
	//#
	//# When a rigid body makes a new contact with a geometry node, the $@RigidBodyController::HandleNewGeometryContact@$
	//# function is called for the rigid body.
	//#
	//# \privbase	CollisionContact	Used internally by the Physics Manager.
	//
	//# \also	$@RigidBodyContact@$
	//# \also	$@RigidBodyController::HandleNewGeometryContact@$


	//# \function	GeometryContact::GetContactGeometry		Returns the geometry node involved in a contact.
	//
	//# \proto	Geometry *GetContactGeometry(void) const;
	//
	//# \desc
	//# The $GetContactGeometry$ function returns a pointer to the geometry node involved in a contact.


	//# \function	GeometryContact::GetRigidBodyContactPosition		Returns the node-space contact position for the rigid body.
	//
	//# \proto	const Point3D& GetRigidBodyContactPosition(void) const;
	//
	//# \desc
	//# The $GetRigidBodyContactPosition$ function returns the contact position on the geometry node in the coordinate space of the
	//# rigid body.
	//
	//# \also	$@GeometryContact::GetGeometryContactPosition@$
	//# \also	$@GeometryContact::GetRigidBodyContactNormal@$


	//# \function	GeometryContact::GetGeometryContactPosition		Returns the node-space contact position for the geometry.
	//
	//# \proto	const Point3D& GetGeometryContactPosition(void) const;
	//
	//# \desc
	//# The $GetGeometryContactPosition$ function returns the contact position on the rigid body in the coordinate space of the
	//# geometry node.
	//
	//# \also	$@GeometryContact::GetRigidBodyContactPosition@$
	//# \also	$@GeometryContact::GetRigidBodyContactNormal@$


	//# \function	GeometryContact::GetRigidBodyContactNormal		Returns the node-space contact normal for the rigid body.
	//
	//# \proto	const Antivector3D& GetRigidBodyContactNormal(void) const;
	//
	//# \desc
	//# The $GetRigidBodyContactNormal$ function returns the contact normal in the coordinate space of the rigid body.
	//
	//# \also	$@GeometryContact::GetRigidBodyContactPosition@$
	//# \also	$@GeometryContact::GetGeometryContactPosition@$


	class GeometryContact final : public CollisionContact
	{
		friend class Contact;

		private:

			Geometry			*contactGeometry;
			unsigned_int32		contactSignature;

			GeometryContact(Body *nullBody);

			static void GeometryLinkProc(Node *node, void *cookie);

			void InitializeSubcontact(Subcontact *subcontact, const RigidBodyController *rigidBody, const Point3D& alphaPosition, const IntersectionData *intersectionData) const;

		public:

			GeometryContact(Geometry *geometry, RigidBodyController *rigidBody, const IntersectionData *intersectionData, unsigned_int32 signature);
			~GeometryContact();

			Geometry *GetContactGeometry(void) const
			{
				return (contactGeometry);
			}

			unsigned_int32 GetContactSignature(void) const
			{
				return (contactSignature);
			}

			const Point3D& GetRigidBodyContactPosition(void) const
			{
				return (subcontact[0].alphaPosition);
			}

			const Point3D& GetGeometryContactPosition(void) const
			{
				return (subcontact[0].betaPosition);
			}

			const Antivector3D& GetRigidBodyContactNormal(void) const
			{
				return (subcontact[0].alphaNormal);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			bool NonpersistentFinishNode(void) const override;

			void UpdateContact(const IntersectionData *intersectionData);

			void InitializeConstraints(void) override;
			void ApplyVelocityConstraints(void) override;
			void ApplyFrictionConstraints(void) override;
			void ApplyPositionConstraints(void) override;
	};


	//# \class	RigidBodyContact		Represents a contact between two rigid bodies.
	//
	//# The $RigidBodyContact$ class represents a contact between two rigid bodies.
	//
	//# \def	class RigidBodyContact final : public CollisionContact
	//
	//# \desc
	//# The $RigidBodyContact$ class represents a contact between single shapes belonging to two
	//# different rigid bodies. Instances of the $RigidBodyContact$ class are only created internally
	//# by the Physics Manager.
	//#
	//# When a rigid body makes a new contact with another rigid body, the $@RigidBodyController::HandleNewRigidBodyContact@$
	//# function is called for both rigid bodies.
	//#
	//# \privbase	CollisionContact	Used internally by the Physics Manager.
	//
	//# \also	$@GeometryContact@$
	//# \also	$@RigidBodyController::HandleNewRigidBodyContact@$


	//# \function	RigidBodyContact::GetWorldContactPosition		Returns the world-space contact position and normal vector.
	//
	//# \proto	void GetWorldContactPosition(const RigidBodyController *rigidBody, Point3D *position, Antivector3D *normal) const;
	//
	//# \param	rigidBody		One of the rigid bodies involved in the contact.
	//# \param	position		A pointer to a location that receives the world-space position.
	//# \param	normal			A pointer to a location that receives the world-space normal vector.
	//
	//# \desc
	//# The $GetWorldContactPosition$ function returns the world-space position and normal vector corresponding to one of the
	//# two rigid bodies involved in a rigid body contact. The $rigidBody$ parameter must be a pointer to one of the rigid bodies
	//# connected in the contact graph by the $RigidBodyContact$ object for which this function is called. It would ordinarily
	//# be set to either a pointer to the object for which the $@RigidBodyController::HandleNewRigidBodyContact@$ function has been
	//# called or to the $contactBody$ parameter passed to the $@RigidBodyController::HandleNewRigidBodyContact@$ function.
	//#
	//# The $position$ parameter should specify a location to which the world-space contact position is written. This position
	//# represents the point on the surface of the <i>other</i> rigid body that is closest to the deepest penetration between the
	//# two rigid bodies.
	//#
	//# The $normal$ parameter should specify a location to which the world-space contact normal vector is written. The normal
	//# vector always points outward with respect to the rigid body specified by the $rigidBody$ parameter.
	//# (If the $GetWorldContactPosition$ were to be called for both rigid bodies involved in the contact, the normal vectors
	//# returned would be negatives of each other.)
	//
	//# \also	$@RigidBodyController::HandleNewRigidBodyContact@$


	class RigidBodyContact final : public CollisionContact
	{
		friend class Contact;

		private:

			const Shape			*startShape;
			const Shape			*finishShape;

			unsigned_int32		startSignature;
			unsigned_int32		finishSignature;

			RigidBodyContact(Body *nullBody);

			void InitializeSubcontact(Subcontact *subcontact, RigidBodyController *rigidBody1, RigidBodyController *rigidBody2, const Point3D& alphaPosition, const IntersectionData *intersectionData) const;

		public:

			RigidBodyContact(RigidBodyController *rigidBody1, RigidBodyController *rigidBody2, const Shape *shape1, const Shape *shape2, unsigned_int32 signature1, unsigned_int32 signature2, const IntersectionData *intersectionData);
			~RigidBodyContact();

			const Shape *GetStartShape(void) const
			{
				return (startShape);
			}

			const Shape *GetFinishShape(void) const
			{
				return (finishShape);
			}

			unsigned_int32 GetStartSignature(void) const
			{
				return (startSignature);
			}

			unsigned_int32 GetFinishSignature(void) const
			{
				return (finishSignature);
			}

			const Point3D& GetContactPosition(void) const
			{
				return (subcontact[0].alphaPosition);
			}

			const Antivector3D& GetContactNormal(void) const
			{
				return (subcontact[0].alphaNormal);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			bool NonpersistentFinishNode(void) const override;

			void UpdateContact(const IntersectionData *intersectionData);

			void InitializeConstraints(void) override;
			void ApplyVelocityConstraints(void) override;
			void ApplyFrictionConstraints(void) override;
			void ApplyPositionConstraints(void) override;

			C4API void GetWorldContactPosition(const RigidBodyController *rigidBody, Point3D *position, Antivector3D *normal) const;
	};


	class DeformableContact final : public Contact
	{
		public:

			DeformableContact(Body *body1, Body *body2);
			~DeformableContact();
	};


	class JointContact : public ConstraintContact, public LinkTarget<JointContact>
	{
		friend class Contact;

		private:

			Joint		*contactJoint;

			JointContact(Body *nullBody);

			static void JointLinkProc(Node *node, void *cookie);

			void SetJoint(Joint *joint);

		protected:

			JointContact(ContactType type, Joint *joint, Body *body1, Body *body2);
			JointContact(ContactType type, Body *nullBody);

			Joint *GetContactJoint(void) const
			{
				return (contactJoint);
			}

		public:

			JointContact(Joint *joint, RigidBodyController *body1, RigidBodyController *body2);
			~JointContact();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void ApplyVelocityConstraints(void) override;
			void ApplyPositionConstraints(void) override;
	};


	class StaticJointContact final : public JointContact
	{
		friend class Contact;

		private:

			StaticJointContact(Body *nullBody);

		public:

			StaticJointContact(Joint *joint, RigidBodyController *rigidBody, Body *nullBody);
			~StaticJointContact();

			void ApplyVelocityConstraints(void) override;
			void ApplyPositionConstraints(void) override;
	};


	#if C4DIAGS

		class ContactRenderable : public Renderable, public LinkTarget<ContactRenderable>
		{
			private:

				List<Attribute>				attributeList;
				DiffuseAttribute			colorAttribute;
				DiffuseTextureAttribute		textureAttribute;

			protected:

				VertexBuffer			vertexBuffer;

				ContactRenderable(const ColorRGBA& color, const char *texture);

			public:

				~ContactRenderable();

				virtual void UpdateContact(int32 count, const Subcontact *subcontact) = 0;
		};


		class ContactVectorRenderable final : public ContactRenderable
		{
			private:

				struct ContactVertex
				{
					Point3D		position;
					Vector4D	tangent;
					Point2D		texcoord;
				};

			public:

				ContactVectorRenderable(const Subcontact *subcontact, const ColorRGBA& color);
				~ContactVectorRenderable();

				void UpdateContact(int32 count, const Subcontact *subcontact) override;
		};


		class ContactPointRenderable final : public ContactRenderable
		{
			private:

				struct ContactVertex
				{
					Point3D		position;
					Vector2D	billboard;
					Point2D		texcoord;
				};

			public:

				ContactPointRenderable(const Subcontact *subcontact, const ColorRGBA& color);
				~ContactPointRenderable();

				void UpdateContact(int32 count, const Subcontact *subcontact) override;
		};

	#endif
}


#endif

// ZYUQURM
