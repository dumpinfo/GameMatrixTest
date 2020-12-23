 

#ifndef C4Blockers_h
#define C4Blockers_h


//# \component	Physics Manager
//# \prefix		PhysicsMgr/


#include "C4Volumes.h"
#include "C4Node.h"


namespace C4
{
	typedef Type	BlockerType;


	enum : ObjectType
	{
		kObjectBlocker			= 'BLKR'
	};


	//# \enum	BlockerType

	enum : BlockerType
	{
		kBlockerPlate			= 'PLAT',		//## Plate blocker.
		kBlockerBox				= 'BOX ',		//## Box blocker.
		kBlockerCylinder		= 'CYLD',		//## Cylinder blocker.
		kBlockerSphere			= 'SPHR',		//## Sphere blocker.
		kBlockerCapsule			= 'CPSL'		//## Capsule blocker.
	};


	//# \class	BlockerObject		Encapsulates data pertaining to a blocker volume.
	//
	//# The $BlockerObject$ class encapsulates data pertaining to a blocker volume.
	//
	//# \def	class BlockerObject : public Object, public VolumeObject
	//
	//# \ctor	BlockerObject(BlockerType type, Volume *volume);
	//
	//# The constructor has protected access. The $BlockerObject$ class can only exist as the base class for another class.
	//
	//# \param	type		The type of the blocker volume. See below for a list of possible types.
	//# \param	volume		A pointer to the generic volume object representing the blocker.
	//
	//# \desc
	//# The $BlockerObject$ class encapsulates data describing a volume used as a rope and cloth blocker in a physics simulation.
	//#
	//# A blocker object can be of one of the following types.
	//
	//# \table	BlockerType
	//
	//# \base		WorldMgr/Object		A $BlockerObject$ is an object that can be shared by multiple blocker nodes.
	//# \privbase	VolumeObject		Used internally by the engine for generic volume objects.
	//
	//# \also	$@Blocker@$


	//# \function	BlockerObject::GetBlockerType		Returns the specific type of a blocker.
	//
	//# \proto	BlockerType GetBlockerType(void) const;
	//
	//# \desc
	//# The $GetBlockerType$ function returns the specific blocker type, which can be one of the following constants.
	//
	//# \table	BlockerType


	class BlockerObject : public Object, public VolumeObject
	{
		friend class WorldMgr;

		private:

			BlockerType			blockerType;

			static BlockerObject *Create(Unpacker& data, unsigned_int32 unpackFlags);

		protected:

			BlockerObject(BlockerType type, Volume *volume);
			~BlockerObject();

		public:

			BlockerType GetBlockerType(void) const
			{
				return (blockerType);
			}

			void PackType(Packer& data) const override;
			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetObjectSize(float *size) const override;
			void SetObjectSize(const float *size) override;

			virtual void ApplyBlocker(int32 count, float radius, Point3D *restrict position, const Point3D *previous, const Transform4D& transform, const Transform4D& inverseTransform) const = 0; 
	};

 
	//# \class	PlateBlockerObject		Encapsulates data pertaining to a plate blocker.
	// 
	//# The $PlateBlockerObject$ class encapsulates data pertaining to a plate blocker.
	//
	//# \def	class PlateBlockerObject final : public BlockerObject, public PlateVolume 
	//
	//# \ctor	PlateBlockerObject(const Vector2D& size); 
	// 
	//# \param	size	The size of the plate.
	//
	//# \desc
	//# The $PlateBlockerObject$ class encapsulates a blocker volume shaped like a plate whose dimensions 
	//# are specified by the $size$ parameter.
	//
	//# \base	BlockerObject			A $PlateBlockerObject$ is an object that can be shared by multiple plate blocker nodes.
	//# \base	WorldMgr/PlateVolume	A $PlateBlockerObject$ is represented by a generic plate volume.
	//
	//# \also	$@PlateBlocker@$


	class PlateBlockerObject final : public BlockerObject, public PlateVolume
	{
		friend class BlockerObject;

		private:

			PlateBlockerObject();
			~PlateBlockerObject();

		public:

			PlateBlockerObject(const Vector2D& size);

			void ApplyBlocker(int32 count, float radius, Point3D *restrict position, const Point3D *previous, const Transform4D& transform, const Transform4D& inverseTransform) const override;
	};


	//# \class	BoxBlockerObject		Encapsulates data pertaining to a box blocker.
	//
	//# The $BoxBlockerObject$ class encapsulates data pertaining to a box blocker.
	//
	//# \def	class BoxBlockerObject final : public BlockerObject, public BoxVolume
	//
	//# \ctor	BoxBlockerObject(const Vector3D& size);
	//
	//# \param	size	The size of the box.
	//
	//# \desc
	//# The $BoxBlockerObject$ class encapsulates a blocker volume shaped like a box whose dimensions
	//# are specified by the $size$ parameter.
	//
	//# \base	BlockerObject			A $BoxBlockerObject$ is an object that can be shared by multiple box blocker nodes.
	//# \base	WorldMgr/BoxVolume		A $BoxBlockerObject$ is represented by a generic box volume.
	//
	//# \also	$@BoxBlocker@$


	class BoxBlockerObject final : public BlockerObject, public BoxVolume
	{
		friend class BlockerObject;

		private:

			BoxBlockerObject();
			~BoxBlockerObject();

		public:

			BoxBlockerObject(const Vector3D& size);

			void ApplyBlocker(int32 count, float radius, Point3D *restrict position, const Point3D *previous, const Transform4D& transform, const Transform4D& inverseTransform) const override;
	};


	//# \class	CylinderBlockerObject		Encapsulates data pertaining to a cylinder blocker.
	//
	//# The $CylinderBlockerObject$ class encapsulates data pertaining to a cylinder blocker.
	//
	//# \def	class CylinderBlockerObject final : public BlockerObject, public CylinderVolume
	//
	//# \ctor	CylinderBlockerObject(const Vector2D& size, float height);
	//
	//# \param	size	The size of the cylinder base.
	//# \param	height	The height of the cylinder.
	//
	//# \desc
	//# The $CylinderBlockerObject$ class encapsulates a blocker volume shaped like a cylinder whose dimensions
	//# are specified by the $size$ and $height$ parameters.
	//
	//# \base	BlockerObject				A $CylinderBlockerObject$ is an object that can be shared by multiple cylinder blocker nodes.
	//# \base	WorldMgr/CylinderVolume		A $CylinderBlockerObject$ is represented by a generic cylinder volume.
	//
	//# \also	$@CylinderBlocker@$


	class CylinderBlockerObject final : public BlockerObject, public CylinderVolume
	{
		friend class BlockerObject;

		private:

			CylinderBlockerObject();
			~CylinderBlockerObject();

		public:

			CylinderBlockerObject(const Vector2D& size, float height);

			void ApplyBlocker(int32 count, float radius, Point3D *restrict position, const Point3D *previous, const Transform4D& transform, const Transform4D& inverseTransform) const override;
	};


	//# \class	SphereBlockerObject		Encapsulates data pertaining to a sphere blocker.
	//
	//# The $SphereBlockerObject$ class encapsulates data pertaining to a sphere blocker.
	//
	//# \def	class SphereBlockerObject final : public BlockerObject, public SphereVolume
	//
	//# \ctor	SphereBlockerObject(const Vector3D& size);
	//
	//# \param	size	The size of the sphere.
	//
	//# \desc
	//# The $SphereBlockerObject$ class encapsulates a blocker volume shaped like a sphere whose dimensions
	//# are specified by the $size$ parameter.
	//
	//# \base	BlockerObject				A $SphereBlockerObject$ is an object that can be shared by multiple sphere blocker nodes.
	//# \base	WorldMgr/SphereVolume		A $SphereBlockerObject$ is represented by a generic sphere volume.
	//
	//# \also	$@SphereBlocker@$


	class SphereBlockerObject final : public BlockerObject, public SphereVolume
	{
		friend class BlockerObject;

		private:

			SphereBlockerObject();
			~SphereBlockerObject();

		public:

			SphereBlockerObject(const Vector3D& size);

			void ApplyBlocker(int32 count, float radius, Point3D *restrict position, const Point3D *previous, const Transform4D& transform, const Transform4D& inverseTransform) const override;
	};


	//# \class	CapsuleBlockerObject		Encapsulates data pertaining to a capsule blocker.
	//
	//# The $CapsuleBlockerObject$ class encapsulates data pertaining to a capsule blocker.
	//
	//# \def	class CapsuleBlockerObject final : public BlockerObject, public CapsuleVolume
	//
	//# \ctor	CapsuleBlockerObject(const Vector3D& size, float height);
	//
	//# \param	size	The size of the hemispherical caps of the capsule.
	//# \param	height	The height of the cylindrical interior of the capsule.
	//
	//# \desc
	//# The $CapsuleBlockerObject$ class encapsulates a blocker volume shaped like a capsule whose dimensions
	//# are specified by the $size$ and $height$ parameters.
	//
	//# \base	BlockerObject				A $CapsuleBlockerObject$ is an object that can be shared by multiple capsule blocker nodes.
	//# \base	WorldMgr/CapsuleVolume		A $CapsuleBlockerObject$ is represented by a generic capsule volume.
	//
	//# \also	$@CapsuleBlocker@$


	class CapsuleBlockerObject final : public BlockerObject, public CapsuleVolume
	{
		friend class BlockerObject;

		private:

			CapsuleBlockerObject();
			~CapsuleBlockerObject();

		public:

			CapsuleBlockerObject(const Vector3D& size, float height);

			void ApplyBlocker(int32 count, float radius, Point3D *restrict position, const Point3D *previous, const Transform4D& transform, const Transform4D& inverseTransform) const override;
	};


	//# \class	Blocker		Represents a blocker node in a world.
	//
	//# The $Blocker$ class represents a blocker node in a world.
	//
	//# \def	class Blocker : public Node
	//
	//# \ctor	Blocker(BlockerType type);
	//
	//# The constructor has protected access. A $Blocker$ class can only exist as the base class for a more specific type of blocker.
	//
	//# \param	type	The type of the blocker. See below for a list of possible types.
	//
	//# \desc
	//# The $Blocker$ class represents a blocker node in a world.
	//#
	//# A blocker node can be of one of the following types.
	//
	//# \table	BlockerType
	//
	//# \base	WorldMgr/Node		A $Blocker$ node is a scene graph node.
	//
	//# \also	$@BlockerObject@$


	//# \function	Blocker::GetBlockerType		Returns the specific type of a blocker.
	//
	//# \proto	BlockerType GetBlockerType(void) const;
	//
	//# \desc
	//# The $GetBlockerType$ function returns the specific blocker type, which can be one of the following constants.
	//
	//# \table	BlockerType


	class Blocker : public Node
	{
		friend class Node;

		private:

			BlockerType		blockerType;

			static Blocker *Create(Unpacker& data, unsigned_int32 unpackFlags);

		protected:

			Blocker(BlockerType type);
			Blocker(const Blocker& blocker);

		public:

			virtual ~Blocker();

			BlockerType GetBlockerType(void) const
			{
				return (blockerType);
			}

			BlockerObject *GetObject(void) const
			{
				return (static_cast<BlockerObject *>(Node::GetObject()));
			}

			void PackType(Packer& data) const override;
	};


	//# \class	PlateBlocker		Represents a plate blocker node in a world.
	//
	//# The $PlateBlocker$ class represents a plate blocker node in a world.
	//
	//# \def	class PlateBlocker final : public Blocker
	//
	//# \ctor	PlateBlocker(const Vector2D& size);
	//
	//# \param	size	The size of the plate.
	//
	//# \desc
	//# The $PlateBlocker$ class represents a blocker node that is shaped like a plate
	//# whose dimensions are specified by the $size$ parameter.
	//
	//# \base	Blocker		A plate blocker is a specific type of blocker.
	//
	//# \also	$@PlateBlockerObject@$


	class PlateBlocker final : public Blocker
	{
		friend class Blocker;

		private:

			PlateBlocker();
			PlateBlocker(const PlateBlocker& plateBlocker);

			Node *Replicate(void) const override;

		public:

			C4API PlateBlocker(const Vector2D& size);
			C4API ~PlateBlocker();

			PlateBlockerObject *GetObject(void) const
			{
				return (static_cast<PlateBlockerObject *>(Node::GetObject()));
			}
	};


	//# \class	BoxBlocker		Represents a box blocker node in a world.
	//
	//# The $BoxBlocker$ class represents a box blocker node in a world.
	//
	//# \def	class BoxBlocker final : public Blocker
	//
	//# \ctor	BoxBlocker(const Vector3D& size);
	//
	//# \param	size	The size of the box.
	//
	//# \desc
	//# The $BoxBlocker$ class represents a blocker node that is shaped like a box
	//# whose dimensions are specified by the $size$ parameter.
	//
	//# \base	Blocker		A box blocker is a specific type of blocker.
	//
	//# \also	$@BoxBlockerObject@$


	class BoxBlocker final : public Blocker
	{
		friend class Blocker;

		private:

			BoxBlocker();
			BoxBlocker(const BoxBlocker& boxBlocker);

			Node *Replicate(void) const override;

		public:

			C4API BoxBlocker(const Vector3D& size);
			C4API ~BoxBlocker();

			BoxBlockerObject *GetObject(void) const
			{
				return (static_cast<BoxBlockerObject *>(Node::GetObject()));
			}
	};


	//# \class	CylinderBlocker		Represents a cylinder blocker node in a world.
	//
	//# The $CylinderBlocker$ class represents a cylinder blocker node in a world.
	//
	//# \def	class CylinderBlocker final : public Blocker
	//
	//# \ctor	CylinderBlocker(const Vector2D& size, float height);
	//
	//# \param	size	The size of the cylinder base.
	//# \param	height	The height of the cylinder.
	//
	//# \desc
	//# The $CylinderBlocker$ class represents a blocker node that is shaped like a cylinder
	//# whose dimensions are specified by the $size$ and $height$ parameters.
	//
	//# \base	Blocker		A cylinder blocker is a specific type of blocker.
	//
	//# \also	$@CylinderBlockerObject@$


	class CylinderBlocker final : public Blocker
	{
		friend class Blocker;

		private:

			CylinderBlocker();
			CylinderBlocker(const CylinderBlocker& cylinderBlocker);

			Node *Replicate(void) const override;

		public:

			C4API CylinderBlocker(const Vector2D& size, float height);
			C4API ~CylinderBlocker();

			CylinderBlockerObject *GetObject(void) const
			{
				return (static_cast<CylinderBlockerObject *>(Node::GetObject()));
			}
	};


	//# \class	SphereBlocker		Represents a sphere blocker node in a world.
	//
	//# The $SphereBlocker$ class represents a sphere blocker node in a world.
	//
	//# \def	class SphereBlocker final : public Blocker
	//
	//# \ctor	SphereBlocker(const Vector3D& size);
	//
	//# \param	size	The size of the sphere.
	//
	//# \desc
	//# The $SphereBlocker$ class represents a blocker node that is shaped like a sphere
	//# whose dimensions are specified by the $size$ parameter.
	//
	//# \base	Blocker		A sphere blocker is a specific type of blocker.
	//
	//# \also	$@SphereBlockerObject@$


	class SphereBlocker final : public Blocker
	{
		friend class Blocker;

		private:

			SphereBlocker();
			SphereBlocker(const SphereBlocker& sphereBlocker);

			Node *Replicate(void) const override;

		public:

			C4API SphereBlocker(const Vector3D& size);
			C4API ~SphereBlocker();

			SphereBlockerObject *GetObject(void) const
			{
				return (static_cast<SphereBlockerObject *>(Node::GetObject()));
			}
	};


	//# \class	CapsuleBlocker		Represents a capsule blocker node in a world.
	//
	//# The $CapsuleBlocker$ class represents a capsule blocker node in a world.
	//
	//# \def	class CapsuleBlocker final : public Blocker
	//
	//# \ctor	CapsuleBlocker(const Vector3D& size, float height);
	//
	//# \param	size	The size of the hemispherical caps of the capsule.
	//# \param	height	The height of the cylindrical interior of the capsule.
	//
	//# \desc
	//# The $CapsuleBlocker$ class represents a blocker node that is shaped like a capsule
	//# whose dimensions are specified by the $size$ and $height$ parameters.
	//
	//# \base	Blocker		A capsule blocker is a specific type of blocker.
	//
	//# \also	$@CapsuleBlockerObject@$


	class CapsuleBlocker final : public Blocker
	{
		friend class Blocker;

		private:

			CapsuleBlocker();
			CapsuleBlocker(const CapsuleBlocker& capsuleBlocker);

			Node *Replicate(void) const override;

		public:

			C4API CapsuleBlocker(const Vector3D& size, float height);
			C4API ~CapsuleBlocker();

			CapsuleBlockerObject *GetObject(void) const
			{
				return (static_cast<CapsuleBlockerObject *>(Node::GetObject()));
			}
	};
}

#endif

// ZYUQURM
