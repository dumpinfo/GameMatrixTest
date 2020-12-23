#ifndef C4Triggers_h
#define C4Triggers_h


//# \component	World Manager
//# \prefix		WorldMgr/


#include "C4Volumes.h"
#include "C4Node.h"


namespace C4
{
	typedef Type	TriggerType;


	enum : ObjectType
	{
		kObjectTrigger		= 'TRIG'
	};


	//# \enum	TriggerType

	enum : TriggerType
	{
		kTriggerBox			= 'BOX ',				//## Box trigger.
		kTriggerCylinder	= 'CYLD',				//## Cylinder trigger.
		kTriggerSphere		= 'SPHR'				//## Sphere trigger.
	};


	//# \enum	TriggerFlags

	enum
	{
		kTriggerActivateDisable			= 1 << 0,	//## The trigger is disabled after the first time it is activated.
		kTriggerContinuouslyActivated	= 1 << 1	//## The trigger is continuously activated instead of being activated only on entry.
	};


	//# \class	TriggerObject		Encapsulates data pertaining to a trigger.
	//
	//# The $TriggerObject$ class encapsulates data pertaining to a trigger.
	//
	//# \def	class TriggerObject : public Object, public VolumeObject
	//
	//# \ctor	TriggerObject(TriggerType type, Volume *volume);
	//
	//# The constructor has protected access. The $TriggerObject$ class can only exist as the base class for another class.
	//
	//# \param	type		The type of the trigger. See below for a list of possible types.
	//# \param	volume		A pointer to the generic volume object representing the trigger.
	//
	//# \desc
	//# The $TriggerObject$ class is the base class for all types of trigger objects, and it stores
	//# data that is common to all of them. Each specific type of trigger has an associated object
	//# that is a subclass of the $TriggerObject$ class.
	//#
	//# A trigger object can have one of the following types.
	//
	//# \table	TriggerType
	//
	//# \base		Object			A $TriggerObject$ is an object that can be shared by multiple trigger nodes.
	//# \privbase	VolumeObject	Used internally by the engine for generic volume objects.
	//
	//# \also	$@Trigger@$
	//
	//# \wiki	Triggers


	//# \function	TriggerObject::GetTriggerType		Returns the specific type of a trigger.
	//
	//# \proto	TriggerType GetTriggerType(void) const;
	//
	//# \desc
	//# The $GetTriggerType$ function returns the specific trigger type, which can be one of the following constants.
	//
	//# \table	TriggerType


	//# \function	TriggerObject::GetTriggerFlags		Returns the trigger flags.
	//
	//# \proto	unsigned_int32 GetTriggerFlags(void) const;
	//
	//# \desc
	//# The $GetTriggerFlags$ function returns the trigger flags, which can be a combination (through logical OR)
	//# of the following constants.
	//
	//# \table	TriggerFlags
	//
	//# The initial value for the trigger flags is $kTriggerActivateDisable$.
	//
	//# \also	$@TriggerObject::SetTriggerFlags@$


	//# \function	TriggerObject::SetTriggerFlags		Sets the trigger flags.
	//
	//# \proto	void SetTriggerFlags(unsigned_int32 flags);
	//
	//# \param	flags		The new trigger flags.
	//
	//# \desc
	//# The $SetTriggerFlags$ function sets the trigger flags to the value specified by the $flags$ parameter,
	//# which can be a combination (through logical OR) of the following constants.
	//
	//# \table	TriggerFlags
	//
	//# The initial value for the trigger flags is $kTriggerActivateDisable$.
	//
	//# \also	$@TriggerObject::GetTriggerFlags@$


	//# \function	TriggerObject::GetTargetConnectorKey	Returns the connector key for the trigger's target node.
	//
	//# \proto	const ConnectorKey& GetTargetConnectorKey(void) const;
	//
	//# \desc
	//# The $GetTargetConnectorKey$ function returns the connector key that is used to find the trigger's target
	//# node. When a trigger is activated and the trigger does not have a controller itself, then the controller
	//# of the connected node is activated.
	//#
	//# See the $@World::ActivateTriggers@$ function for the exact trigger activation behavior.
	//
	//# \also	$@TriggerObject::SetTargetConnectorKey@$


	//# \function	TriggerObject::SetTargetConnectorKey	Sets the connector key for the trigger's target node.
	//
	//# \proto	void SetTargetConnectorKey(const char *key);
	//
	//# \param	key		The new connector key.
	//
	//# \desc
	//# The $SetTargetConnectorKey$ function sets the connector key that is used to find the trigger's target
	//# node. When a trigger is activated and the trigger does not have a controller itself, then the controller
	//# of the connected node is activated.
	//#
	//# See the $@World::ActivateTriggers@$ function for the exact trigger activation behavior.
	//
	//# \also	$@TriggerObject::GetTargetConnectorKey@$


	class TriggerObject : public Object, public VolumeObject
	{
		friend class WorldMgr;

		private:

			TriggerType		triggerType;
			unsigned_int32	triggerFlags;

			ConnectorKey	activationConnectorKey;
			ConnectorKey	deactivationConnectorKey;

			static TriggerObject *Create(Unpacker& data, unsigned_int32 unpackFlags);

		protected:

			TriggerObject(TriggerType type, Volume *volume);
			~TriggerObject();

		public:

			TriggerType GetTriggerType(void) const
			{
				return (triggerType);
			}

			unsigned_int32 GetTriggerFlags(void) const
			{
				return (triggerFlags);
			}

			void SetTriggerFlags(unsigned_int32 flags)
			{
				triggerFlags = flags;
			}

			const ConnectorKey& GetActivationConnectorKey(void) const
			{
				return (activationConnectorKey);
			}

			void SetActivationConnectorKey(const char *key)
			{
				activationConnectorKey = key;
			}

			const ConnectorKey& GetDeactivationConnectorKey(void) const
			{
				return (deactivationConnectorKey);
			}

			void SetDeactivationConnectorKey(const char *key)
			{
				deactivationConnectorKey = key;
			}

			void PackType(Packer& data) const override;
			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;

			int32 GetCategoryCount(void) const override;
			Type GetCategoryType(int32 index, const char **title) const override;
			int32 GetCategorySettingCount(Type category) const override;
			Setting *GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const override;
			void SetCategorySetting(Type category, const Setting *setting) override;

			int32 GetObjectSize(float *size) const override;
			void SetObjectSize(const float *size) override;

			virtual bool IntersectSegment(const Point3D& p1, const Point3D& p2, float radius = 0.0F) const;
	};


	//# \class	BoxTriggerObject		Encapsulates data pertaining to a box trigger.
	//
	//# The $BoxTriggerObject$ class encapsulates data pertaining to a box trigger.
	//
	//# \def	class BoxTriggerObject final : public TriggerObject, public BoxVolume
	//
	//# \ctor	BoxTriggerObject(const Vector3D& size);
	//
	//# \param	size	The size of the box.
	//
	//# \desc
	//# The $BoxTriggerObject$ class encapsulates a trigger volume shaped like a box whose dimensions
	//# are specified by the $size$ parameter.
	//
	//# \base	TriggerObject		A $BoxTriggerObject$ is an object that can be shared by multiple box trigger nodes.
	//# \base	BoxVolume			A $BoxTriggerObject$ is represented by a generic box volume.
	//
	//# \also	$@BoxTrigger@$


	class BoxTriggerObject final : public TriggerObject, public BoxVolume
	{
		friend class TriggerObject;

		private:

			BoxTriggerObject();
			~BoxTriggerObject();

		public:

			BoxTriggerObject(const Vector3D& size);

			bool IntersectSegment(const Point3D& p1, const Point3D& p2, float radius = 0.0F) const override;
	};


	//# \class	CylinderTriggerObject		Encapsulates data pertaining to a cylinder trigger.
	//
	//# The $CylinderTriggerObject$ class encapsulates data pertaining to a cylinder trigger.
	//
	//# \def	class CylinderTriggerObject final : public TriggerObject, public CylinderVolume
	//
	//# \ctor	CylinderTriggerObject(const Vector2D& size, float height);
	//
	//# \param	size	The size of the cylinder base.
	//# \param	height	The height of the cylinder.
	//
	//# \desc
	//# The $CylinderTriggerObject$ class encapsulates a trigger volume shaped like a cylinder whose dimensions
	//# are specified by the $size$ and $height$ parameters.
	//
	//# \base	TriggerObject		A $CylinderTriggerObject$ is an object that can be shared by multiple cylinder trigger nodes.
	//# \base	CylinderVolume		A $CylinderTriggerObject$ is represented by a generic cylinder volume.
	//
	//# \also	$@CylinderTrigger@$


	class CylinderTriggerObject final : public TriggerObject, public CylinderVolume
	{
		friend class TriggerObject;

		private:

			CylinderTriggerObject();
			~CylinderTriggerObject();

		public:

			CylinderTriggerObject(const Vector2D& size, float height);

			bool IntersectSegment(const Point3D& p1, const Point3D& p2, float radius = 0.0F) const override;
	};


	//# \class	SphereTriggerObject		Encapsulates data pertaining to a sphere trigger.
	//
	//# The $SphereTriggerObject$ class encapsulates data pertaining to a sphere trigger.
	//
	//# \def	class SphereTriggerObject final : public TriggerObject, public SphereVolume
	//
	//# \ctor	SphereTriggerObject(const Vector3D& size);
	//
	//# \param	size	The size of the sphere.
	//
	//# \desc
	//# The $SphereTriggerObject$ class encapsulates a trigger volume shaped like a sphere whose dimensions
	//# are specified by the $size$ parameter.
	//
	//# \base	TriggerObject		A $SphereTriggerObject$ is an object that can be shared by multiple sphere trigger nodes.
	//# \base	SphereVolume		A $SphereTriggerObject$ is represented by a generic sphere volume.
	//
	//# \also	$@SphereTrigger@$


	class SphereTriggerObject final : public TriggerObject, public SphereVolume
	{
		friend class TriggerObject;

		private:

			SphereTriggerObject();
			~SphereTriggerObject();

		public:

			SphereTriggerObject(const Vector3D& size);

			bool IntersectSegment(const Point3D& p1, const Point3D& p2, float radius = 0.0F) const override;
	};


	//# \class	Trigger		Represents a trigger node in a world.
	//
	//# The $Trigger$ class represents a trigger node in a world.
	//
	//# \def	class Trigger : public Node, public ListElement<Trigger>
	//
	//# \ctor	Trigger(TriggerType type);
	//
	//# The constructor has protected access. A $Trigger$ class can only exist as the base class for a more specific type of trigger.
	//
	//# \param	type	The type of the trigger. See below for a list of possible types.
	//
	//# \desc
	//# The $Trigger$ class is the base class for all trigger nodes. The $type$ parameter can be one of the following
	//# constants, and it is specified by the subclass's constructor.
	//
	//# \table	TriggerType
	//
	//# Triggers are typically activated by calling the $@World::ActivateTriggers@$ function to test a swept sphere
	//# against all trigger volumes in the world. See the $@World::ActivateTriggers@$ function for the exact trigger
	//# activation behavior.
	//
	//# \base	Node							A $Trigger$ node is a scene graph node.
	//# \base	Utilities/ListElement<Trigger>	Used internally by the World Manager.
	//
	//# \also	$@TriggerObject@$
	//
	//# \wiki	Triggers


	//# \function	Trigger::GetTriggerType		Returns the specific type of a trigger.
	//
	//# \proto	TriggerType GetTriggerType(void) const;
	//
	//# \desc
	//# The $GetTriggerType$ function returns the specific trigger type, which can be one of the following constants.
	//
	//# \table	TriggerType


	class Trigger : public Node, public ListElement<Trigger>
	{
		friend class Node;

		private:

			TriggerType			triggerType;

			static Trigger *Create(Unpacker& data, unsigned_int32 unpackFlags);

			bool CalculateBoundingBox(Box3D *box) const override;
			void HandleVisibilityUpdate(void) override;

		protected:

			Trigger(TriggerType type);
			Trigger(const Trigger& trigger);

		public:

			virtual ~Trigger();

			using ListElement<Trigger>::Previous;
			using ListElement<Trigger>::Next;

			TriggerType GetTriggerType(void) const
			{
				return (triggerType);
			}

			TriggerObject *GetObject(void) const
			{
				return (static_cast<TriggerObject *>(Node::GetObject()));
			}

			void PackType(Packer& data) const override;
			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Preprocess(void) override;
			void Neutralize(void) override;

			C4API void Activate(Node *initiator = nullptr);
			C4API void Deactivate(void);
	};


	//# \class	BoxTrigger		Represents a box trigger node in a world.
	//
	//# The $BoxTrigger$ class represents a box trigger node in a world.
	//
	//# \def	class BoxTrigger final : public Trigger
	//
	//# \ctor	BoxTrigger(const Vector3D& size);
	//
	//# \param	size	The size of the box.
	//
	//# \desc
	//# The $BoxTrigger$ class represents a trigger node that is shaped like a box
	//# whose dimensions are specified by the $size$ parameter.
	//
	//# \base	Trigger		A box trigger is a specific type of trigger.
	//
	//# \also	$@BoxTriggerObject@$


	class BoxTrigger final : public Trigger
	{
		friend class Trigger;

		private:

			BoxTrigger();
			BoxTrigger(const BoxTrigger& boxTrigger);

			Node *Replicate(void) const override;

		public:

			C4API BoxTrigger(const Vector3D& size);
			C4API ~BoxTrigger();

			BoxTriggerObject *GetObject(void) const
			{
				return (static_cast<BoxTriggerObject *>(Node::GetObject()));
			}
	};


	//# \class	CylinderTrigger		Represents a cylinder trigger node in a world.
	//
	//# The $CylinderTrigger$ class represents a cylinder trigger node in a world.
	//
	//# \def	class CylinderTrigger final : public Trigger
	//
	//# \ctor	CylinderTrigger(const Vector2D& size, float height);
	//
	//# \param	size	The size of the cylinder base.
	//# \param	height	The height of the cylinder.
	//
	//# \desc
	//# The $CylinderTrigger$ class represents a trigger node that is shaped like a cylinder
	//# whose dimensions are specified by the $size$ and $height$ parameters.
	//
	//# \base	Trigger		A cylinder trigger is a specific type of trigger.
	//
	//# \also	$@CylinderTriggerObject@$


	class CylinderTrigger final : public Trigger
	{
		friend class Trigger;

		private:

			CylinderTrigger();
			CylinderTrigger(const CylinderTrigger& cylinderTrigger);

			Node *Replicate(void) const override;

		public:

			C4API CylinderTrigger(const Vector2D& size, float height);
			C4API ~CylinderTrigger();

			CylinderTriggerObject *GetObject(void) const
			{
				return (static_cast<CylinderTriggerObject *>(Node::GetObject()));
			}
	};


	//# \class	SphereTrigger		Represents a sphere trigger node in a world.
	//
	//# The $SphereTrigger$ class represents a sphere trigger node in a world.
	//
	//# \def	class SphereTrigger final : public Trigger
	//
	//# \ctor	SphereTrigger(const Vector3D& size);
	//
	//# \param	size	The size of the sphere.
	//
	//# \desc
	//# The $SphereTrigger$ class represents a trigger node that is shaped like a sphere
	//# whose dimensions are specified by the $size$ parameter.
	//
	//# \base	Trigger		A sphere trigger is a specific type of trigger.
	//
	//# \also	$@SphereTriggerObject@$


	class SphereTrigger final : public Trigger
	{
		friend class Trigger;

		private:

			SphereTrigger();
			SphereTrigger(const SphereTrigger& sphereTrigger);

			Node *Replicate(void) const override;

		public:

			C4API SphereTrigger(const Vector3D& size);
			C4API ~SphereTrigger();

			SphereTriggerObject *GetObject(void) const
			{
				return (static_cast<SphereTriggerObject *>(Node::GetObject()));
			}
	};
}


#endif

// ZYUQURM
