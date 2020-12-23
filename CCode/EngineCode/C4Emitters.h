 

#ifndef C4Emitters_h
#define C4Emitters_h


//# \component	Effect Manager
//# \prefix		EffectMgr/


#include "C4Volumes.h"
#include "C4Node.h"


namespace C4
{
	typedef Type	EmitterType;


	enum : ObjectType
	{
		kObjectEmitter			= 'EMIT'
	};


	//# \enum	EmitterType

	enum : EmitterType
	{
		kEmitterBox				= 'BOX ',		//## Box emitter.
		kEmitterCylinder		= 'CYLD',		//## Cylinder emitter.
		kEmitterSphere			= 'SPHR'		//## Sphere emitter.
	};


	//# \enum	EmitterFlags

	enum
	{
		kEmitterHeightField		= 1 << 0		//## The emitter contains a height field representing the geometry height relative to the bottom surface.
	};


	//# \class	EmitterObject		Encapsulates data pertaining to an emitter volume.
	//
	//# The $EmitterObject$ class encapsulates data pertaining to an emitter volume.
	//
	//# \def	class EmitterObject : public Object, public VolumeObject
	//
	//# \ctor	EmitterObject(EmitterType type, Volume *volume);
	//
	//# The constructor has protected access. The $EmitterObject$ class can only exist as the base class for another class.
	//
	//# \param	type		The type of the emitter volume. See below for a list of possible types.
	//# \param	volume		A pointer to the generic volume object representing the emitter.
	//
	//# \desc
	//# The $EmitterObject$ class encapsulates data describing a volume used for particle emission.
	//# Its member functions can be used by particle systems to generate random points inside the emitter volume
	//# or on either the top or bottom surface of the emitter volume.
	//
	//# An emitter object can be of one of the following types.
	//
	//# \table	EmitterType
	//
	//# \base		WorldMgr/Object		An $EmitterObject$ is an object that can be shared by multiple emitter nodes.
	//# \privbase	VolumeObject		Used internally by the engine for generic volume objects.
	//
	//# \also	$@Emitter@$


	//# \function	EmitterObject::GetEmitterType		Returns the specific type of an emitter.
	//
	//# \proto	EmitterType GetEmitterType(void) const;
	//
	//# \desc
	//# The $GetEmitterType$ function returns the specific emitter type, which can be one of the following constants.
	//
	//# \table	EmitterType


	//# \function	EmitterObject::GetEmitterSurfaceArea		Returns the area of the top and bottom surfaces of an emitter volume.
	//
	//# \proto	float GetEmitterSurfaceArea(void) const;
	//
	//# \desc
	//# The $GetEmitterSurfaceArea$ function returns the area of the top and bottom surfaces of an emitter volume.
	//
	//# \also	$@EmitterObject::GetTopSurfaceEmissionPoint@$
	//# \also	$@EmitterObject::GetBottomSurfaceEmissionPoint@$


	//# \function	EmitterObject::GetVolumeEmissionPoint		Returns a random point inside an emitter volume.
	//
	//# \proto	Point3D GetVolumeEmissionPoint(void) const;
	//
	//# \desc
	//# The $GetVolumeEmissionPoint$ function returns a uniformly-distributed random point inside an emitter volume.
	//
	//# \also	$@EmitterObject::GetTopSurfaceEmissionPoint@$
	//# \also	$@EmitterObject::GetBottomSurfaceEmissionPoint@$

 
	//# \function	EmitterObject::GetTopSurfaceEmissionPoint		Returns a random point on the top surface of an emitter volume.
	//
	//# \proto	Point3D GetTopSurfaceEmissionPoint(void) const; 
	//
	//# \desc 
	//# The $GetTopSurfaceEmissionPoint$ function returns a uniformly-distributed random point on the top surface
	//# of an emitter volume. The top surface is the portion of the emitter boundary having the largest <i>z</i> coordinates.
	// 
	//# \also	$@EmitterObject::GetVolumeEmissionPoint@$
	//# \also	$@EmitterObject::GetBottomSurfaceEmissionPoint@$ 
	//# \also	$@EmitterObject::GetEmitterSurfaceArea@$ 


	//# \function	EmitterObject::GetBottomSurfaceEmissionPoint	Returns a random point on the bottom surface of an emitter volume.
	// 
	//# \proto	Point3D GetBottomSurfaceEmissionPoint(void) const;
	//
	//# \desc
	//# The $GetBottomSurfaceEmissionPoint$ function returns a uniformly-distributed random point on the bottom surface
	//# of an emitter volume. The bottom surface is the portion of the emitter boundary having the smallest <i>z</i> coordinates.
	//
	//# \also	$@EmitterObject::GetVolumeEmissionPoint@$
	//# \also	$@EmitterObject::GetTopSurfaceEmissionPoint@$
	//# \also	$@EmitterObject::GetEmitterSurfaceArea@$


	class EmitterObject : public Object, public VolumeObject
	{
		friend class WorldMgr;

		private:

			EmitterType			emitterType;
			unsigned_int32		emitterFlags;

			int32				heightFieldSubdivX;
			int32				heightFieldSubdivY;
			Vector2D			heightFieldScale;

			float				*heightFieldStorage;

			static EmitterObject *Create(Unpacker& data, unsigned_int32 unpackFlags);

		protected:

			EmitterObject(EmitterType type, Volume *volume);
			~EmitterObject();

			float SampleHeightField(float x, float y) const;

		public:

			EmitterType GetEmitterType(void) const
			{
				return (emitterType);
			}

			unsigned_int32 GetEmitterFlags(void) const
			{
				return (emitterFlags);
			}

			void SetEmitterFlags(unsigned_int32 flags)
			{
				emitterFlags = flags;
			}

			int32 GetHeightFieldSubdivX(void) const
			{
				return (heightFieldSubdivX);
			}

			int32 GetHeightFieldSubdivY(void) const
			{
				return (heightFieldSubdivY);
			}

			float *GetHeightFieldStorage(void) const
			{
				return (heightFieldStorage);
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

			C4API void SetHeightFieldSubdiv(int32 subdivX, int32 subdivY);
			C4API void AllocateHeightField(void);
			C4API void DeallocateHeightField(void);

			virtual float GetEmitterRadius(void) const = 0;
			virtual float GetEmitterSurfaceArea(void) const = 0;

			virtual Point3D GetVolumeEmissionPoint(void) const = 0;
			virtual Point3D GetTopSurfaceEmissionPoint(void) const = 0;
			virtual Point3D GetBottomSurfaceEmissionPoint(void) const = 0;
	};


	//# \class	BoxEmitterObject		Encapsulates data pertaining to a box emitter.
	//
	//# The $BoxEmitterObject$ class encapsulates data pertaining to a box emitter.
	//
	//# \def	class BoxEmitterObject final : public EmitterObject, public BoxVolume
	//
	//# \ctor	BoxEmitterObject(const Vector3D& size);
	//
	//# \param	size	The size of the box.
	//
	//# \desc
	//# The $BoxEmitterObject$ class encapsulates an emitter volume shaped like a box whose dimensions
	//# are specified by the $size$ parameter.
	//
	//# \base	EmitterObject			A $BoxEmitterObject$ is an object that can be shared by multiple box emitter nodes.
	//# \base	WorldMgr/BoxVolume		A $BoxEmitterObject$ is represented by a generic box volume.
	//
	//# \also	$@BoxEmitter@$


	class BoxEmitterObject final : public EmitterObject, public BoxVolume
	{
		friend class EmitterObject;

		private:

			BoxEmitterObject();
			~BoxEmitterObject();

		public:

			BoxEmitterObject(const Vector3D& size);

			float GetEmitterRadius(void) const override;
			float GetEmitterSurfaceArea(void) const override;

			Point3D GetVolumeEmissionPoint(void) const override;
			Point3D GetTopSurfaceEmissionPoint(void) const override;
			Point3D GetBottomSurfaceEmissionPoint(void) const override;
	};


	//# \class	CylinderEmitterObject		Encapsulates data pertaining to a cylinder emitter.
	//
	//# The $CylinderEmitterObject$ class encapsulates data pertaining to a cylinder emitter.
	//
	//# \def	class CylinderEmitterObject final : public EmitterObject, public CylinderVolume
	//
	//# \ctor	CylinderEmitterObject(const Vector2D& size, float height);
	//
	//# \param	size	The size of the cylinder base.
	//# \param	height	The height of the cylinder.
	//
	//# \desc
	//# The $CylinderEmitterObject$ class encapsulates an emitter volume shaped like a cylinder whose dimensions
	//# are specified by the $size$ and $height$ parameters.
	//
	//# \base	EmitterObject				A $CylinderEmitterObject$ is an object that can be shared by multiple cylinder emitter nodes.
	//# \base	WorldMgr/CylinderVolume		A $CylinderEmitterObject$ is represented by a generic cylinder volume.
	//
	//# \also	$@CylinderEmitter@$


	class CylinderEmitterObject final : public EmitterObject, public CylinderVolume
	{
		friend class EmitterObject;

		private:

			CylinderEmitterObject();
			~CylinderEmitterObject();

		public:

			CylinderEmitterObject(const Vector2D& size, float height);

			float GetEmitterRadius(void) const override;
			float GetEmitterSurfaceArea(void) const override;

			Point3D GetVolumeEmissionPoint(void) const override;
			Point3D GetTopSurfaceEmissionPoint(void) const override;
			Point3D GetBottomSurfaceEmissionPoint(void) const override;
	};


	//# \class	SphereEmitterObject		Encapsulates data pertaining to a sphere emitter.
	//
	//# The $SphereEmitterObject$ class encapsulates data pertaining to a sphere emitter.
	//
	//# \def	class SphereEmitterObject final : public EmitterObject, public SphereVolume
	//
	//# \ctor	SphereEmitterObject(const Vector3D& size);
	//
	//# \param	size	The size of the sphere.
	//
	//# \desc
	//# The $SphereEmitterObject$ class encapsulates an emitter volume shaped like a sphere whose dimensions
	//# are specified by the $size$ parameter.
	//
	//# \base	EmitterObject				A $SphereEmitterObject$ is an object that can be shared by multiple sphere emitter nodes.
	//# \base	WorldMgr/SphereVolume		A $SphereEmitterObject$ is represented by a generic sphere volume.
	//
	//# \also	$@SphereEmitter@$


	class SphereEmitterObject final : public EmitterObject, public SphereVolume
	{
		friend class EmitterObject;

		private:

			SphereEmitterObject();
			~SphereEmitterObject();

		public:

			SphereEmitterObject(const Vector3D& size);

			float GetEmitterRadius(void) const override;
			float GetEmitterSurfaceArea(void) const override;

			Point3D GetVolumeEmissionPoint(void) const override;
			Point3D GetTopSurfaceEmissionPoint(void) const override;
			Point3D GetBottomSurfaceEmissionPoint(void) const override;
	};


	//# \class	Emitter		Represents an emitter node in a world.
	//
	//# The $Emitter$ class represents an emitter node in a world.
	//
	//# \def	class Emitter : public Node
	//
	//# \ctor	Emitter(EmitterType type);
	//
	//# The constructor has protected access. An $Emitter$ class can only exist as the base class for a more specific type of emitter.
	//
	//# \param	type	The type of the emitter. See below for a list of possible types.
	//
	//# \desc
	//# The $Emitter$ class represents an emitter node in a world. A $@ParticleSystem@$ node can connect to
	//# an emitter node and use the associated $@EmitterObject@$ to generate random points inside the emitter volume.
	//
	//# An emitter node can be of one of the following types.
	//
	//# \table	EmitterType
	//
	//# \base	WorldMgr/Node		An $Emitter$ node is a scene graph node.
	//
	//# \also	$@EmitterObject@$


	//# \function	Emitter::GetEmitterType		Returns the specific type of an emitter.
	//
	//# \proto	EmitterType GetEmitterType(void) const;
	//
	//# \desc
	//# The $GetEmitterType$ function returns the specific emitter type, which can be one of the following constants.
	//
	//# \table	EmitterType


	class Emitter : public Node
	{
		friend class Node;

		private:

			EmitterType		emitterType;

			static Emitter *Create(Unpacker& data, unsigned_int32 unpackFlags);

		protected:

			Emitter(EmitterType type);
			Emitter(const Emitter& emitter);

		public:

			virtual ~Emitter();

			EmitterType GetEmitterType(void) const
			{
				return (emitterType);
			}

			EmitterObject *GetObject(void) const
			{
				return (static_cast<EmitterObject *>(Node::GetObject()));
			}

			void PackType(Packer& data) const override;
	};


	//# \class	BoxEmitter		Represents a box emitter node in a world.
	//
	//# The $BoxEmitter$ class represents a box emitter node in a world.
	//
	//# \def	class BoxEmitter final : public Emitter
	//
	//# \ctor	BoxEmitter(const Vector3D& size);
	//
	//# \param	size	The size of the box.
	//
	//# \desc
	//# The $BoxEmitter$ class represents an emitter node that is shaped like a box whose dimensions are
	//# specified by the $size$ parameter. The associated $@BoxEmitterObject@$ can be used by a
	//# particle system to generate random points inside a box emitter volume.
	//
	//# \base	Emitter		A box emitter is a specific type of emitter.
	//
	//# \also	$@BoxEmitterObject@$


	class BoxEmitter final : public Emitter
	{
		friend class Emitter;

		private:

			BoxEmitter();
			BoxEmitter(const BoxEmitter& boxEmitter);

			Node *Replicate(void) const override;

		public:

			C4API BoxEmitter(const Vector3D& size);
			C4API ~BoxEmitter();

			BoxEmitterObject *GetObject(void) const
			{
				return (static_cast<BoxEmitterObject *>(Node::GetObject()));
			}
	};


	//# \class	CylinderEmitter		Represents a cylinder emitter node in a world.
	//
	//# The $CylinderEmitter$ class represents a cylinder emitter node in a world.
	//
	//# \def	class CylinderEmitter final : public Emitter
	//
	//# \ctor	CylinderEmitter(const Vector2D& size, float height);
	//
	//# \param	size	The size of the cylinder base.
	//# \param	height	The height of the cylinder.
	//
	//# \desc
	//# The $CylinderEmitter$ class represents an emitter node that is shaped like a cylinder whose dimensions are
	//# specified by the $size$ and $height$ parameters. The associated $@CylinderEmitterObject@$ can be used
	//# by a particle system to generate random points inside a cylinder emitter volume.
	//
	//# \base	Emitter		A cylinder emitter is a specific type of emitter.
	//
	//# \also	$@CylinderEmitterObject@$


	class CylinderEmitter final : public Emitter
	{
		friend class Emitter;

		private:

			CylinderEmitter();
			CylinderEmitter(const CylinderEmitter& cylinderEmitter);

			Node *Replicate(void) const override;

		public:

			C4API CylinderEmitter(const Vector2D& size, float height);
			C4API ~CylinderEmitter();

			CylinderEmitterObject *GetObject(void) const
			{
				return (static_cast<CylinderEmitterObject *>(Node::GetObject()));
			}
	};


	//# \class	SphereEmitter		Represents a sphere emitter node in a world.
	//
	//# The $SphereEmitter$ class represents a sphere emitter node in a world.
	//
	//# \def	class SphereEmitter final : public Emitter
	//
	//# \ctor	SphereEmitter(const Vector3D& size);
	//
	//# \param	size	The size of the sphere.
	//
	//# \desc
	//# The $SphereEmitter$ class represents an emitter node that is shaped like a sphere whose dimensions are
	//# specified by the $size$ parameter. The associated $@SphereEmitterObject@$ can be used by a
	//# particle system to generate random points inside a sphere emitter volume.
	//
	//# \base	Emitter		A sphere emitter is a specific type of emitter.
	//
	//# \also	$@SphereEmitterObject@$


	class SphereEmitter final : public Emitter
	{
		friend class Emitter;

		private:

			SphereEmitter();
			SphereEmitter(const SphereEmitter& sphereEmitter);

			Node *Replicate(void) const override;

		public:

			C4API SphereEmitter(const Vector3D& size);
			C4API ~SphereEmitter();

			SphereEmitterObject *GetObject(void) const
			{
				return (static_cast<SphereEmitterObject *>(Node::GetObject()));
			}
	};
}

#endif

// ZYUQURM
