 

#ifndef C4Particles_h
#define C4Particles_h


//# \component	Effect Manager
//# \prefix		EffectMgr/


#include "C4Effects.h"
#include "C4Emitters.h"
#include "C4Voxels.h"
#include "C4Threads.h"


namespace C4
{
	typedef Type	ParticleSystemType;


	enum : ParticleSystemType
	{
		kParticleSystemStarField			= 'STAR'
	};


	//# \enum	ParticleSystemFlags

	enum
	{
		kParticleSystemSelfDestruct			= 1 << 0,	//## The particle system should delete itself once all the particles have died.
		kParticleSystemPointSprite			= 1 << 1,	//## The particles should be rendered using point sprite hardware if available.
		kParticleSystemNonpersistent		= 1 << 4,	//## The particles are not saved when the particle system is serialized.
		kParticleSystemStaticBoundingBox	= 1 << 5,	//## The particle system has a static bounding box that is calculated by the specific subclass, and the bounding box should bot be recalculated automatically based on the actual particle positions. If this flag is set, then the subclass must override the $@WorldMgr/Node::CalculateBoundingBox@$ function.
		kParticleSystemFreezeInvisible		= 1 << 6,	//## The particle system is not animated when it was not rendered on the previous frame.
		kParticleSystemUnfreezeDynamic		= 1 << 7,	//## The particle system is always animated if it has been invalidated since the previous frame.
		kParticleSystemObjectSpace			= 1 << 8,	//## The particle positions are given in object-space coordinates, and the $@GraphicsMgr/Renderable::SetTransformable@$ function has been used to set the transformable to the particle system itself.
		kParticleSystemSoftDepth			= 1 << 9,	//## The particles fade out as they get close to scene geometry to avoid depth-testing artifacts.
		kParticleSystemDepthRamp			= 1 << 10	//## The particles fade out within a range of camera-space depths to avoid blinking out when they pass through the near plane.
	};


	//# \enum	StarFieldFlags

	enum
	{
		kStarFieldAboveHorizon				= 1 << 0	//## The star field renders only stars that are above the horizon in world space.
	};


	C4API extern const char kConnectorKeyEmitter[];


	class ParticleSystem;
	class FrustumCamera;
	class DiffuseTextureAttribute;


	struct StarData
	{
		float		rightAscension;
		float		declination;
		float		magnitude;
	};


	class StarFieldResource : public Resource<StarFieldResource>
	{
		friend class Resource<StarFieldResource>;

		private:

			static ResourceDescriptor	descriptor;

			~StarFieldResource();

		public:

			StarFieldResource(const char *name, ResourceCatalog *catalog);

			int32 GetStarCount(void) const
			{
				return (static_cast<const int32 *>(GetData())[1]);
			}

			const StarData *GetStarData(void) const
			{
				return (reinterpret_cast<const StarData *>(&static_cast<const int32 *>(GetData())[2]));
			}
	};


	//# \struct	Particle	Contains state information about a particle.
	//
	//# The $Particle$ structure contains state information about a particle.
	//
	//# \def	struct Particle
	//
	//# \data	Particle
	//
	//# \also	$@TexcoordParticle@$
	//# \also	$@QuadParticle@$ 
	//# \also	$@PolyParticle@$
	//# \also	$@FireParticle@$
	//# \also	$@BlobParticle@$ 

 
	//# \function	Particle::GetPreviousParticle		Returns the previous particle in a particle system.
	//
	//# \proto	Particle *GetPreviousParticle(void) const; 
	//
	//# \desc 
	//# The $GetPreviousParticle$ function returns the previous particle in the list of particles currently used 
	//# by a particle system. If the particle for which this function is called is the first particle in the particle
	//# system, then the return value is $nullptr$.
	//
	//# \also	$@Particle::GetNextParticle@$ 
	//# \also	$@ParticleSystem::GetFirstParticle@$
	//# \also	$@ParticleSystem::GetLastParticle@$


	//# \function	Particle::GetNextParticle			Returns the next particle in a particle system.
	//
	//# \proto	Particle *GetNextParticle(void) const;
	//
	//# \desc
	//# The $GetNextParticle$ function returns the next particle in the list of particles currently used
	//# by a particle system. If the particle for which this function is called is the last particle in the particle
	//# system, then the return value is $nullptr$.
	//
	//# \also	$@Particle::GetPreviousParticle@$
	//# \also	$@ParticleSystem::GetFirstParticle@$
	//# \also	$@ParticleSystem::GetLastParticle@$


	//# \member		Particle

	struct Particle
	{
		Particle			*prevParticle;		//## A pointer to the previous particle in the system. This is $nullptr$ for the first particle.
		Particle			*nextParticle;		//## A pointer to the next particle in the system. This is $nullptr$ for the last particle.
		unsigned_int32		particleIndex;		//## The unique index of the particle in the particle pool. This is in the range [0,&nbsp;<i>n</i>&nbsp;&minus;&nbsp;1], where <i>n</i> is the total number of particles in the pool.

		int32				emitTime;			//## The time remaining before the particle is emitted, in milliseconds.
		int32				lifeTime;			//## The time remaining (once the particle is emitted) before the particle dies, in milliseconds.

		float				radius;				//## The radius of the particle.
		ColorRGBA			color;				//## The color and transparency of the particle.

		union
		{
			UnsignedFixed	orientation;		//## The orientation of the particle as an unsigned 8.24-bit fixed-point angle. The top 8 bits divide the circle into 256 angles of 360/256 degrees. This field is only used for point particles and should not be set for other types of particle systems.
			float			height;				//## The vertical height of a fire particle. This field is only used for fire particles and should not be set for other types of particle systems.
		};

		Point3D				position;			//## The position of the particle. This is normally in world-space coordinates, but can be in object-space coordinates if the $kParticleSystemObjectSpace$ flag is set for the particle system.
		Vector3D			velocity;			//## The velocity of the particle. Like the position, this is in world-space coordinates unless the $kParticleSystemObjectSpace$ flag is set.

		Particle *GetPreviousParticle(void) const
		{
			return (prevParticle);
		}

		Particle *GetNextParticle(void) const
		{
			return (nextParticle);
		}

		void Pack(Packer& data) const
		{
			data << emitTime;
			data << lifeTime;
			data << radius;
			data << color;
			data << orientation;
			data << position;
			data << velocity;
		}

		void Unpack(Unpacker& data)
		{
			data >> emitTime;
			data >> lifeTime;
			data >> radius;
			data >> color;
			data >> orientation;
			data >> position;
			data >> velocity;
		}
	};


	//# \struct	TexcoordParticle	Contains state information about a particle with animated texture coordinates.
	//
	//# The $TexcoordParticle$ structure extends the $@Particle@$ structure and contains extra state information for a particle with animated texture coordinates.
	//
	//# \def	struct TexcoordParticle : Particle
	//
	//# \data	TexcoordParticle
	//
	//# \desc
	//# The $TexcoordParticle$ structure, or a subclass of this structure, must be used in the particle pool assigned to a $@TexcoordPointParticleSystem@$ effect.
	//
	//# \base	Particle		The $TexcoordParticle$ structure extends the $Particle$ structure.


	//# \member		TexcoordParticle

	struct TexcoordParticle : Particle
	{
		Vector2D		texcoordScale;		//## The scale to apply to the texture coordinates.
		Vector2D		texcoordOffset;		//## The offset to apply to the texture coordinates.

		TexcoordParticle *GetPreviousParticle(void) const
		{
			return (static_cast<TexcoordParticle *>(prevParticle));
		}

		TexcoordParticle *GetNextParticle(void) const
		{
			return (static_cast<TexcoordParticle *>(nextParticle));
		}

		void Pack(Packer& data) const
		{
			Particle::Pack(data);
			data << texcoordScale;
			data << texcoordOffset;
		}

		void Unpack(Unpacker& data)
		{
			Particle::Unpack(data);
			data >> texcoordScale;
			data >> texcoordOffset;
		}
	};


	//# \struct	QuadParticle	Contains state information about an oriented quad particle.
	//
	//# The $QuadParticle$ structure extends the $@Particle@$ structure and contains extra state information for an oriented quad particle.
	//
	//# \def	struct QuadParticle : Particle
	//
	//# \data	QuadParticle
	//
	//# \desc
	//# The $QuadParticle$ structure, or a subclass of this structure, must be used in the particle pool assigned to a $@QuadParticleSystem@$ effect.
	//
	//# \base	Particle		The $QuadParticle$ structure extends the $Particle$ structure.


	//# \member		QuadParticle

	struct QuadParticle : Particle
	{
		Quaternion		transform;		//## A quaternion representing the orientation of the quad.

		QuadParticle *GetPreviousParticle(void) const
		{
			return (static_cast<QuadParticle *>(prevParticle));
		}

		QuadParticle *GetNextParticle(void) const
		{
			return (static_cast<QuadParticle *>(nextParticle));
		}

		void Pack(Packer& data) const
		{
			Particle::Pack(data);
			data << transform;
		}

		void Unpack(Unpacker& data)
		{
			Particle::Unpack(data);
			data >> transform;
		}
	};


	//# \struct	PolyParticle	Contains state information about a polyboard particle.
	//
	//# The $PolyParticle$ structure extends the $@Particle@$ structure and contains extra state information for a polyboard particle.
	//
	//# \def	struct PolyParticle
	//
	//# \data	PolyParticle : Particle
	//
	//# \desc
	//# The $PolyParticle$ structure, or a subclass of this structure, must be used in the particle pool assigned to a $@PolyboardParticleSystem@$ effect.
	//
	//# \base	Particle		The $PolyParticle$ structure extends the $Particle$ structure.


	//# \member		PolyParticle

	struct PolyParticle : Particle
	{
		Vector3D	tangent;		//## The world-space tangent direction for the particle.
		float		texcoord;		//## The <i>t</i> texture coordinate for the particle.
		bool		terminator;		//## Indicates whether the particle is the last in a polyboard.

		PolyParticle *GetPreviousParticle(void) const
		{
			return (static_cast<PolyParticle *>(prevParticle));
		}

		PolyParticle *GetNextParticle(void) const
		{
			return (static_cast<PolyParticle *>(nextParticle));
		}

		void Pack(Packer& data) const
		{
			Particle::Pack(data);

			data << tangent;
			data << texcoord;
			data << terminator;
		}

		void Unpack(Unpacker& data)
		{
			Particle::Unpack(data);

			data >> tangent;
			data >> texcoord;
			data >> terminator;
		}
	};


	//# \struct	FireParticle	Contains state information about a fire particle.
	//
	//# The $FireParticle$ structure extends the $@Particle@$ structure and contains extra state information for a fire particle.
	//
	//# \def	struct FireParticle : Particle
	//
	//# \data	FireParticle
	//
	//# \desc
	//# The $FireParticle$ structure, or a subclass of this structure, must be used in the particle pool assigned to a $@FireParticleSystem@$ effect.
	//
	//# \base	Particle		The $FireParticle$ structure extends the $Particle$ structure.


	//# \member		FireParticle

	struct FireParticle : Particle
	{
		float		intensity;				//## The fire animation intensity.
		Vector2D	noiseOffset;			//## A random offset used when accessing the noise texture to give each flame a unique appearance.
		Vector2D	noiseVelocity[3];		//## A set of three noise texture coordinate velocities.

		FireParticle *GetPreviousParticle(void) const
		{
			return (static_cast<FireParticle *>(prevParticle));
		}

		FireParticle *GetNextParticle(void) const
		{
			return (static_cast<FireParticle *>(nextParticle));
		}

		void Pack(Packer& data) const
		{
			Particle::Pack(data);

			data << intensity;
			data << noiseOffset;
			data << noiseVelocity[0];
			data << noiseVelocity[1];
			data << noiseVelocity[2];
		}

		void Unpack(Unpacker& data)
		{
			Particle::Unpack(data);

			data >> intensity;
			data >> noiseOffset;
			data >> noiseVelocity[0];
			data >> noiseVelocity[1];
			data >> noiseVelocity[2];
		}
	};


	//# \struct	BlobParticle	Contains state information about a blob particle.
	//
	//# The $BlobParticle$ structure extends the $@Particle@$ structure and contains extra state information for a blob particle.
	//
	//# \def	struct BlobParticle : Particle, ListElement<BlobParticle>
	//
	//# \data	BlobParticle
	//
	//# \desc
	//# The $BlobParticle$ structure, or a subclass of this structure, must be used in the particle pool assigned to a $@BlobParticleSystem@$ effect.
	//
	//# \base	Particle								The $BlobParticle$ structure extends the $Particle$ structure.
	//# \base	Utilities/ListElement<BlobParticle>		Used internally of the $@BlobParticleSystem@$ class.


	//# \member		BlobParticle

	struct BlobParticle : Particle, ListElement<BlobParticle>
	{
		Vector3D	scaleAxis;			//## The world-space axis along which the blob is scaled. This must be a unit vector.
		float		inverseScale;		//## The reciprocal of the scale applied in the direction given by $scaleAxis$. This must be greater than or equal to 1.0.

		Box3D		bounds;
		bool		terminator;

		BlobParticle *GetPreviousParticle(void) const
		{
			return (static_cast<BlobParticle *>(prevParticle));
		}

		BlobParticle *GetNextParticle(void) const
		{
			return (static_cast<BlobParticle *>(nextParticle));
		}

		void Pack(Packer& data) const
		{
			Particle::Pack(data);

			data << scaleAxis;
			data << inverseScale;
		}

		void Unpack(Unpacker& data)
		{
			Particle::Unpack(data);

			data >> scaleAxis;
			data >> inverseScale;
		}
	};


	//# \class	ParticleSystemRegistration		Manages internal registration information for a custom particle system type.
	//
	//# The $ParticleSystemRegistration$ class manages internal registration information for a custom particle system type.
	//
	//# \def	class ParticleSystemRegistration : public Registration<ParticleSystem, ParticleSystemRegistration>
	//
	//# \ctor	ParticleSystemRegistration(ParticleSystemType type, const char *name);
	//
	//# \param	type		The particle system type.
	//# \param	name		The particle system name.
	//
	//# \desc
	//# The $ParticleSystemRegistration$ class is abstract and serves as the common base class for the template class
	//# $@ParticleSystemReg@$. A custom particle system is registered with the engine by instantiating an object of type
	//# $ParticleSystemReg<classType>$, where $classType$ is the type of the particle system subclass being registered.
	//
	//# \base	System/Registration<ParticleSystem, ParticleSystemRegistration>		A particle system registration is a specific type of registration object.
	//
	//# \also	$@ParticleSystemReg@$
	//# \also	$@ParticleSystem@$


	//# \function	ParticleSystemRegistration::GetParticleSystemType		Returns the registered particle system type.
	//
	//# \proto	ParticleSystemType GetParticleSystemType(void) const;
	//
	//# \desc
	//# The $GetParticleSystemType$ function returns the particle system type for a particular particle system registration.
	//# The particle system type is established when the particle system registration is constructed.
	//
	//# \also	$@ParticleSystemRegistration::GetParticleSystemName@$


	//# \function	ParticleSystemRegistration::GetParticleSystemName		Returns the human-readable particle system name.
	//
	//# \proto	const char *GetParticleSystemName(void) const;
	//
	//# \desc
	//# The $GetParticleSystemName$ function returns the human-readable particle system name for a particular particle system registration.
	//# The particle system name is established when the particle system registration is constructed.
	//
	//# \also	$@ParticleSystemRegistration::GetParticleSystemType@$


	class ParticleSystemRegistration : public Registration<ParticleSystem, ParticleSystemRegistration>
	{
		private:

			const char		*particleSystemName;

		public:

			C4API ParticleSystemRegistration(ParticleSystemType type, const char *name);
			C4API ~ParticleSystemRegistration();

			ParticleSystemType GetParticleSystemType(void) const
			{
				return (GetRegistrableType());
			}

			const char *GetParticleSystemName(void) const
			{
				return (particleSystemName);
			}
	};


	//# \class	ParticleSystemReg	 Represents a custom particle system type.
	//
	//# The $ParticleSystemReg$ class represents a custom particle system type.
	//
	//# \def	template <class classType> class ParticleSystemReg : public ParticleSystemRegistration
	//
	//# \tparam	classType	The custom particle system class.
	//
	//# \ctor	ParticleSystemReg(ParticleSystemType type, const char *name);
	//
	//# \param	type		The particle system type.
	//# \param	name		The particle system name.
	//
	//# \desc
	//# The $ParticleSystemReg$ template class is used to advertise the existence of a custom particle system type.
	//# The World Manager uses a particle system registration to construct a custom particle system. The act of
	//# instantiating a $ParticleSystemReg$ object automatically registers the corresponding particle system type.
	//# The particle system type is unregistered when the $ParticleSystemReg$ object is destroyed.
	//#
	//# No more than one particle system registration should be created for each distinct particle system type.
	//
	//# \base	ParticleSystemRegistration		All specific particle system registration classes share the common base class $ParticleSystemRegistration$.
	//
	//# \also	$@ParticleSystem@$


	template <class classType> class ParticleSystemReg : public ParticleSystemRegistration
	{
		public:

			ParticleSystemReg(ParticleSystemType type, const char *name) : ParticleSystemRegistration(type, name)
			{
			}

			ParticleSystem *Create(void) const
			{
				return (new classType);
			}
	};


	class ParticlePoolBase
	{
		friend class ParticleSystem;

		private:

			Particle		*particlePool;
			unsigned_int32	particleDataSize;

			int32			totalParticleCount;
			int32			activeParticleCount;

			Particle		*firstFreeParticle;
			Particle		*lastFreeParticle;

			void Reset(void);

		protected:

			C4API ParticlePoolBase(int32 count, Particle *pool, unsigned_int32 size);

			Particle *GetParticlePool(void) const
			{
				return (particlePool);
			}

			C4API Particle *NewParticle(void);

		public:

			C4API virtual ~ParticlePoolBase();

			int32 GetTotalParticleCount(void) const
			{
				return (totalParticleCount);
			}

			virtual void PackParticlePool(Packer& data, const Particle *firstParticle) const = 0;
			virtual void UnpackParticlePool(Unpacker& data, int32 count, Particle **firstParticle, Particle **lastParticle) = 0;
	};


	//# \class	ParticlePool		Handles storage for particles in a particle system.
	//
	//# The $ParticlePool$ class handles storage for particles in a particle system.
	//
	//# \def	template <class type = Particle> class ParticlePool : public ParticlePoolBase
	//
	//# \tparam		type	The structure containing the state data for each particle. This must be $@Particle@$ or one of its subclasses.
	//
	//# \ctor	ParticlePool(int32 count, type *pool);
	//
	//# \param	count		The total number of particles in the pool.
	//# \param	pool		A pointer to an array of structures that hold the particle state data.
	//#						This must point to an array of $count$ structures of the type specified by the $type$ template parameter.
	//
	//# \desc
	//# The $ParticlePool$ class encapsulates an array of particle structures that are used with a single particle system.
	//# These structures hold the current state of all the particles in a particle system. A pointer to a particle pool is
	//# passed to the constructors of the various particle system subclasses.
	//
	//# \privbase	ParticlePoolBase	Used internally to encapsulate common functionality that is independent of the template parameter.
	//
	//# \also	$@ParticleSystem@$
	//# \also	$@Particle@$


	//# \function	ParticlePool::NewParticle		Returns an unused particle from a particle pool.
	//
	//# \proto	type *NewParticle(void);
	//
	//# \desc
	//# The $NewParticle$ function returns a pointer to an unused particle structure in a particle pool. This function should
	//# be called every time a new particle needs to be created in a particle system, and the returned structure should be
	//# filled with the initial state of the new particle. The particle can than be added to the particle system associated with
	//# the particle pool by calling the $@ParticleSystem::AddParticle@$ or $@ParticleSystem::AddFarthestParticle@$ function.
	//#
	//# When a particle is no longer needed, it should be freed by calling the $@ParticleSystem::FreeParticle@$ function.
	//# This makes the particle eligible to be returned by the $NewParticle$ function again.
	//#
	//# If there are no particles available in the particle pool, then the $NewParticle$ function returns $nullptr$.
	//
	//# \also	$@ParticleSystem::AddParticle@$
	//# \also	$@ParticleSystem::AddFarthestParticle@$
	//# \also	$@ParticleSystem::FreeParticle@$


	template <class type = Particle> class ParticlePool : public ParticlePoolBase
	{
		public:

			ParticlePool(int32 count, type *pool) : ParticlePoolBase(count, pool, sizeof(type))
			{
			}

			type *NewParticle(void)
			{
				return (static_cast<type *>(ParticlePoolBase::NewParticle()));
			}

			void PackParticlePool(Packer& data, const Particle *firstParticle) const;
			void UnpackParticlePool(Unpacker& data, int32 count, Particle **firstParticle, Particle **lastParticle);
	};

	template <class type> void ParticlePool<type>::PackParticlePool(Packer& data, const Particle *firstParticle) const
	{
		const type *particle = static_cast<const type *>(firstParticle);
		do
		{
			particle->Pack(data);
			particle = static_cast<const type *>(particle->nextParticle);
		} while (particle);
	}

	template <class type> void ParticlePool<type>::UnpackParticlePool(Unpacker& data, int32 count, Particle **firstParticle, Particle **lastParticle)
	{
		Assert(count != 0, "ParticlePool<type>::UnpackParticlePool, count cannot be 0\n");

		type *particle = static_cast<type *>(GetParticlePool());
		*firstParticle = particle;

		Particle *previous = nullptr;
		for (machine a = 0; a < count; a++)
		{
			particle->prevParticle = previous;
			particle->nextParticle = particle + 1;
			particle->particleIndex = a;
			particle->Unpack(data);

			previous = particle;
			particle++;
		}

		previous->nextParticle = nullptr;
		*lastParticle = previous;
	}


	//# \class	ParticleSystem		Represents a particle system node in a world.
	//
	//# The $ParticleSystem$ class represents a particle system node in a world.
	//
	//# \def	class ParticleSystem : public Effect, public Registrable<ParticleSystem, ParticleSystemRegistration>
	//
	//# \ctor	ParticleSystem(ParticleSystemType type, ParticlePoolBase *pool, ParticleStyle style = kParticlePoint,
	//# \ctor2	const char *textureName = nullptr);
	//
	//# The constructor has protected access. The $ParticleSystem$ class can only exist as the base class for a more specific type of particle effect.
	//
	//# \param	type			The type of particle system.
	//# \param	pool			The particle pool from which this effect creates particles.
	//# \param	textureName		The name of the texture map to use. If this is $nullptr$, then the default particle texture is used.
	//
	//# \desc
	//# The $ParticleSystem$ class is the base class for all types of particle-based effects. A custom particle system is defined by
	//# creating a subclass of one of the more specific subclasses of the $ParticleSystem$ class.
	//
	//# \base	Effect															A $ParticleSystem$ node is a specific type of effect.
	//# \base	System/Registrable<ParticleSystem, ParticleSystemRegistration>	Custom particle system types can be registered with the engine.
	//# \base	InterfaceMgr/Configurable										Particle systems can define configurable parameters that are exposed as user interface widgets in the World Editor.
	//
	//# \also	$@Particle@$
	//# \also	$@ParticlePool@$
	//# \also	$@PointParticleSystem@$
	//# \also	$@TexcoordPointParticleSystem@$
	//# \also	$@InfinitePointParticleSystem@$
	//# \also	$@LineParticleSystem@$
	//# \also	$@QuadParticleSystem@$
	//# \also	$@PolyboardParticleSystem@$
	//# \also	$@FireParticleSystem@$
	//# \also	$@BlobParticleSystem@$


	//# \function	ParticleSystem::GetParticleSystemType		Returns the particle system type.
	//
	//# \proto	ParticleSystemType GetParticleSystemType(void) const;
	//
	//# \desc
	//# The $GetParticleSystemType$ function returns the specific particle system type.


	//# \function	ParticleSystem::GetParticleSystemFlags		Returns the particle system flags.
	//
	//# \proto	unsigned_int32 GetParticleSystemFlags(void) const;
	//
	//# \desc
	//# The $GetParticleSystemFlags$ function returns the particle system flags, which can be a combination (through logical OR)
	//# of the following constants.
	//
	//# \table	ParticleSystemFlags
	//
	//# \also	$@ParticleSystem::SetParticleSystemFlags@$


	//# \function	ParticleSystem::SetParticleSystemFlags		Sets the particle system flags.
	//
	//# \proto	void SetParticleSystemFlags(unsigned_int32 flags);
	//
	//# \param	flags	The new particle system flags.
	//
	//# \desc
	//# The $GetParticleSystemFlags$ function sets the particle system flags to the value specified by the $flags$ parameter.
	//# The particle system flags can be a combination (through logical OR) of the following constants.
	//
	//# \table	ParticleSystemFlags
	//
	//# The initial value of the particle system flags is 0.
	//
	//# \also	$@ParticleSystem::GetParticleSystemFlags@$


	//# \function	ParticleSystem::GetFirstParticle		Returns the first particle in a particle system.
	//
	//# \proto	Particle *GetFirstParticle(void) const;
	//
	//# \desc
	//# The $GetFirstParticle$ function returns the first particle belonging to the particle system. All of the particles
	//# in the particle system can be iterated by calling the $@Particle::GetNextParticle@$ function until $nullptr$
	//# is returned.
	//
	//# \also	$@Particle@$
	//# \also	$@Particle::GetNextParticle@$
	//# \also	$@ParticleSystem::GetLastParticle@$
	//# \also	$@ParticleSystem::AddParticle@$
	//# \also	$@ParticleSystem::AddFarthestParticle@$
	//# \also	$@ParticleSystem::FreeParticle@$


	//# \function	ParticleSystem::GetLastParticle		Returns the last particle in a particle system.
	//
	//# \proto	Particle *GetLastParticle(void) const;
	//
	//# \desc
	//# The $GetLastParticle$ function returns the last particle belonging to the particle system. All of the particles
	//# in the particle system can be iterated by calling the $@Particle::GetPreviousParticle@$ function until $nullptr$
	//# is returned.
	//
	//# \also	$@Particle@$
	//# \also	$@Particle::GetPreviousParticle@$
	//# \also	$@ParticleSystem::GetFirstParticle@$
	//# \also	$@ParticleSystem::AddParticle@$
	//# \also	$@ParticleSystem::AddFarthestParticle@$
	//# \also	$@ParticleSystem::FreeParticle@$


	//# \function	ParticleSystem::AddParticle			Adds a new particle to a particle system.
	//
	//# \proto	void AddParticle(Particle *particle);
	//
	//# \param	particle	The particle to add to the system.
	//
	//# \desc
	//# The $AddParticle$ function adds the particle specified by the $particle$ parameter to a particle system.
	//# The new particle would typically have been allocated by calling the $@ParticlePool::NewParticle@$ function
	//# and then filling in the initial state of the $@Particle@$ structure (or one of its subclasses).
	//
	//# \also	$@Particle@$
	//# \also	$@ParticleSystem::AddFarthestParticle@$
	//# \also	$@ParticleSystem::FreeParticle@$
	//# \also	$@ParticleSystem::GetFirstParticle@$
	//# \also	$@ParticleSystem::GetLastParticle@$
	//# \also	$@ParticlePool::NewParticle@$


	//# \function	ParticleSystem::AddFarthestParticle		Adds a new particle to a particle system and makes it first to render.
	//
	//# \proto	void AddFarthestParticle(Particle *particle);
	//
	//# \param	particle	The particle to add to the system.
	//
	//# \desc
	//# The $AddFarthestParticle$ function adds the particle specified by the $particle$ parameter to a particle system and ensures
	//# that it will be the last particle rendered. The new particle would typically have been allocated by calling the
	//# $@ParticlePool::NewParticle@$ function and then filling in the initial state of the $@Particle@$ structure (or one of its subclasses).
	//
	//# \also	$@Particle@$
	//# \also	$@ParticleSystem::AddParticle@$
	//# \also	$@ParticleSystem::FreeParticle@$
	//# \also	$@ParticleSystem::GetFirstParticle@$
	//# \also	$@ParticleSystem::GetLastParticle@$
	//# \also	$@ParticlePool::NewParticle@$


	//# \function	ParticleSystem::FreeParticle		Removes a particle from a particle system.
	//
	//# \proto	void FreeParticle(Particle *particle);
	//
	//# \param	particle	The particle to remove from the system.
	//
	//# \desc
	//# The $FreeParticle$ function removes the particle specified by the $particle$ parameter from a particle system and
	//# makes it available in the particle pool.
	//
	//# \also	$@Particle@$
	//# \also	$@ParticleSystem::AddParticle@$
	//# \also	$@ParticleSystem::AddFarthestParticle@$
	//# \also	$@ParticleSystem::GetFirstParticle@$
	//# \also	$@ParticleSystem::GetLastParticle@$


	//# \function	ParticleSystem::AnimateParticles		Called once per frame to update the positions of the particles.
	//
	//# \proto	virtual void AnimateParticles(void);
	//
	//# \desc
	//# The $AnimateParticles$ function can be overridden by subclasses of the $ParticleSystem$ class so that
	//# they can implement custom particle motion. The $AnimateParticles$ function can update any properties
	//# of the particles in the system, including position, color, and radius. Particles may also be created
	//# or destroyed inside this function.
	//#
	//# The default implementation of the $AnimateParticles$ function moves each particle according to its
	//# current velocity. It also decreases each particle's life time by the amount of time passed since the
	//# previous frame and destroys any particle whose life time reaches zero.
	//#
	//# If there are no particles in the particle system when the $AnimateParticles$ function returns and the
	//# $kParticleSystemSelfDestruct$ flag is set, then the particle system is deleted immediately after the
	//# $AnimateParticles$ function returns.
	//
	//# \also	$@Particle@$
	//# \also	$@ParticleSystem::GetFirstParticle@$
	//# \also	$@ParticleSystem::GetLastParticle@$
	//# \also	$@ParticleSystem::AddParticle@$
	//# \also	$@ParticleSystem::AddFarthestParticle@$
	//# \also	$@ParticleSystem::FreeParticle@$
	//# \also	$@ParticleSystem::SetParticleSystemFlags@$


	class ParticleSystem : public Effect, public Registrable<ParticleSystem, ParticleSystemRegistration>
	{
		private:

			ParticleSystemType		particleSystemType;
			unsigned_int32			particleSystemFlags;

			bool					animateFlag;

			Emitter					*connectedEmitter;
			ParticlePoolBase		*particlePool;

			Particle				*firstUsedParticle;
			Particle				*lastUsedParticle;

			const Box3D				*boundingBoxPointer;
			Box3D					defaultBoundingBox;
			Box3D					objectBoundingBox;

			MaterialObject			*materialObject;
			List<Attribute>			attributeList;
			DeltaDepthAttribute		deltaDepthAttribute;
			DepthRampAttribute		depthRampAttribute;

			void Initialize(void);

			static void MaterialObjectLinkProc(Object *object, void *cookie);

			C4API bool CalculateBoundingBox(Box3D *box) const override;
			C4API bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

			int32 RenderFireParticles(const FrustumCamera *camera);
			int32 RenderPolyboardParticles(const FrustumCamera *camera);

		protected:

			class ParticleJob : public BatchJob
			{
				public:

					const FrustumCamera		*camera;

					volatile void			*attributeBuffer;
					volatile void			*indexBuffer;

					ParticleJob(ExecuteProc *execProc, FinalizeProc *finalProc, void *cookie);
			};

			ParticleSystem(ParticleSystemType type, ParticlePoolBase *pool, RenderType renderType, unsigned_int32 renderState);
			ParticleSystem(const ParticleSystem& particleSystem, ParticlePoolBase *pool);

			void SetAnimateFlag(void)
			{
				animateFlag = true;
			}

			int32 GetTotalParticleCount(void) const
			{
				return (particlePool->GetTotalParticleCount());
			}

			Particle *GetFirstParticle(void) const
			{
				return (firstUsedParticle);
			}

			Particle *GetLastParticle(void) const
			{
				return (lastUsedParticle);
			}

			void AddAttribute(Attribute *attribute)
			{
				attributeList.Append(attribute);
			}

		public:

			C4API ~ParticleSystem();

			using Creatable<ParticleSystem>::InstallCreator;
			using Registrable<ParticleSystem, ParticleSystemRegistration>::FindRegistration;

			ParticleSystemType GetParticleSystemType(void) const
			{
				return (particleSystemType);
			}

			unsigned_int32 GetParticleSystemFlags(void) const
			{
				return (particleSystemFlags);
			}

			void SetParticleSystemFlags(unsigned_int32 flags)
			{
				particleSystemFlags = flags;
			}

			Emitter *GetConnectedEmitter(void) const
			{
				return (connectedEmitter);
			}

			MaterialObject *GetMaterialObject(void) const
			{
				return (materialObject);
			}

			float GetSoftDepthScale(void) const
			{
				return (deltaDepthAttribute.GetDeltaScale());
			}

			void SetSoftDepthScale(float scale)
			{
				deltaDepthAttribute.SetDeltaScale(scale);
			}

			const Range<float>& GetDepthRampRange(void) const
			{
				return (depthRampAttribute.GetRampRange());
			}

			void SetDepthRampRange(const Range<float>& range)
			{
				depthRampAttribute.SetRampRange(range);
			}

			C4API static ParticleSystem *New(ParticleSystemType type);

			static void RegisterStandardParticleSystems(void);

			C4API void PackType(Packer& data) const override;
			C4API void Prepack(List<Object> *linkList) const override;
			C4API void Pack(Packer& data, unsigned_int32 packFlags) const override;
			C4API void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			C4API int32 GetInternalConnectorCount(void) const override;
			C4API const char *GetInternalConnectorKey(int32 index) const override;
			C4API void ProcessInternalConnectors(void) override;
			C4API bool ValidConnectedNode(const ConnectorKey& key, const Node *node) const override;
			C4API void SetConnectedEmitter(Emitter *emitter);

			C4API void SetMaterialObject(MaterialObject *object);

			C4API void Invalidate(void) override;
			C4API void Preprocess(void) override;
			C4API void Move(void) override;

			C4API void AddParticle(Particle *particle);
			C4API void AddFarthestParticle(Particle *particle);
			C4API void FreeParticle(Particle *particle);
			C4API void FreeAllParticles(void);

			C4API virtual void AnimateParticles(void);
	};


	//# \class	PointParticleSystem		Represents a point-based particle system node in a world.
	//
	//# The $PointParticleSystem$ class represents a point-based particle system node in a world.
	//
	//# \def	class PointParticleSystem : public ParticleSystem
	//
	//# \ctor	PointParticleSystem(ParticleSystemType type, ParticlePoolBase *pool, const char *textureName = nullptr);
	//
	//# The constructor has protected access. The $PointParticleSystem$ class can only exist as the base class for a more specific type of particle effect.
	//
	//# \param	type			The type of particle system.
	//# \param	pool			The particle pool from which this effect creates particles.
	//# \param	textureName		The name of the texture map to use. If this is $nullptr$, then the default particle texture is used.
	//
	//# \desc
	//# The $PointParticleSystem$ class is the base class for all point-based particle effects.
	//
	//# \base	ParticleSystem		A $PointParticleSystem$ node is a specific type of particle system.
	//
	//# \also	$@ParticlePool@$
	//# \also	$@Particle@$


	class PointParticleSystem : public ParticleSystem
	{
		private:

			struct PointVertex
			{
				Point3D			position;
				ColorRGBA		color;
				Point2D			texcoord;
				Vector4D		billboard;
			};

			struct SpriteVertex
			{
				Point3D			position;
				ColorRGBA		color;
				float			radius;
			};

			bool						spriteFlag;
			ParticleJob					renderJob;

			VertexBuffer				vertexBuffer;
			DiffuseTextureAttribute		textureAttribute;

			static void JobRenderParticles(Job *job, void *cookie);
			static void FinalizeParticles(Job *job, void *cookie);

		protected:

			C4API PointParticleSystem(ParticleSystemType type, ParticlePoolBase *pool, const char *textureName = nullptr);
			C4API PointParticleSystem(const PointParticleSystem& pointParticleSystem, ParticlePoolBase *pool);

		public:

			C4API ~PointParticleSystem();

			C4API void Preprocess(void) override;
			C4API void Render(const FrustumCamera *camera, List<Renderable> *effectList) override;
	};


	//# \class	TexcoordPointParticleSystem		Represents a point-based particle system node with animated texture coordinates in a world.
	//
	//# The $TexcoordPointParticleSystem$ class represents an infinite point-based particle system node with animated texture coordinates in a world.
	//
	//# \def	class TexcoordPointParticleSystem : public ParticleSystem
	//
	//# \ctor	TexcoordPointParticleSystem(ParticleSystemType type, ParticlePoolBase *pool, const char *textureName = nullptr);
	//
	//# The constructor has protected access. The $TexcoordPointParticleSystem$ class can only exist as the base class for a more specific type of particle effect.
	//
	//# \param	type			The type of particle system.
	//# \param	pool			The particle pool from which this effect creates particles. The particles in this pool must use the $@TexcoordParticle@$ structure.
	//# \param	textureName		The name of the texture map to use. If this is $nullptr$, then the default particle texture is used.
	//
	//# \desc
	//# The $TexcoordPointParticleSystem$ class is the base class for all point-based particle effects that have animated texture coordinates.
	//
	//# \base	ParticleSystem		A $TexcoordPointParticleSystem$ node is a specific type of particle system.
	//
	//# \also	$@ParticlePool@$
	//# \also	$@TexcoordParticle@$


	class TexcoordPointParticleSystem : public ParticleSystem
	{
		private:

			struct PointVertex
			{
				Point3D			position;
				ColorRGBA		color;
				Point2D			texcoord;
				Vector4D		billboard;
			};

			ParticleJob					renderJob;

			VertexBuffer				vertexBuffer;
			DiffuseTextureAttribute		textureAttribute;

			static void JobRenderParticles(Job *job, void *cookie);
			static void FinalizeParticles(Job *job, void *cookie);

		protected:

			C4API TexcoordPointParticleSystem(ParticleSystemType type, ParticlePoolBase *pool, const char *textureName = nullptr);
			C4API TexcoordPointParticleSystem(const TexcoordPointParticleSystem& texcoordPointParticleSystem, ParticlePoolBase *pool);

		public:

			C4API ~TexcoordPointParticleSystem();

			C4API void Preprocess(void) override;
			C4API void Render(const FrustumCamera *camera, List<Renderable> *effectList) override;
	};


	//# \class	InfinitePointParticleSystem		Represents an infinite point-based particle system node in a world.
	//
	//# The $InfinitePointParticleSystem$ class represents an infinite point-based particle system node in a world.
	//
	//# \def	class InfinitePointParticleSystem : public ParticleSystem
	//
	//# \ctor	InfinitePointParticleSystem(ParticleSystemType type, ParticlePoolBase *pool, const char *textureName = nullptr);
	//
	//# The constructor has protected access. The $InfinitePointParticleSystem$ class can only exist as the base class for a more specific type of particle effect.
	//
	//# \param	type			The type of particle system.
	//# \param	pool			The particle pool from which this effect creates particles.
	//# \param	textureName		The name of the texture map to use. If this is $nullptr$, then the default particle texture is used.
	//
	//# \desc
	//# The $InfinitePointParticleSystem$ class is the base class for all point-based particle effects that are rendered at infinity.
	//
	//# \base	ParticleSystem		An $InfinitePointParticleSystem$ node is a specific type of particle system.
	//
	//# \also	$@ParticlePool@$
	//# \also	$@Particle@$


	class InfinitePointParticleSystem : public ParticleSystem
	{
		private:

			struct PointVertex
			{
				Point3D			position;
				ColorRGBA		color;
				Point2D			texcoord;
				Vector2D		billboard;
			};

			struct SpriteVertex
			{
				Point3D			position;
				ColorRGBA		color;
				float			radius;
			};

			bool						spriteFlag;
			ParticleJob					renderJob;

			VertexBuffer				vertexBuffer;
			DiffuseTextureAttribute		textureAttribute;

			static void JobRenderParticles(Job *job, void *cookie);
			static void FinalizeParticles(Job *job, void *cookie);

		protected:

			C4API InfinitePointParticleSystem(ParticleSystemType type, ParticlePoolBase *pool, const char *textureName = nullptr);
			C4API InfinitePointParticleSystem(const InfinitePointParticleSystem& infinitePointParticleSystem, ParticlePoolBase *pool);

		public:

			C4API ~InfinitePointParticleSystem();

			C4API void Preprocess(void) override;
			C4API void Render(const FrustumCamera *camera, List<Renderable> *effectList) override;
	};


	//# \class	LineParticleSystem		Represents a line-based particle system node in a world.
	//
	//# The $LineParticleSystem$ class represents a line-based particle system node in a world.
	//
	//# \def	class LineParticleSystem : public ParticleSystem
	//
	//# \ctor	LineParticleSystem(ParticleSystemType type, ParticlePoolBase *pool, const char *textureName = nullptr);
	//
	//# The constructor has protected access. The $LineParticleSystem$ class can only exist as the base class for a more specific type of particle effect.
	//
	//# \param	type			The type of particle system.
	//# \param	pool			The particle pool from which this effect creates particles.
	//# \param	textureName		The name of the texture map to use. If this is $nullptr$, then the default particle texture is used.
	//
	//# \desc
	//# The $LineParticleSystem$ class is the base class for all line-based particle effects.
	//
	//# \base	ParticleSystem		A $LineParticleSystem$ node is a specific type of particle system.
	//
	//# \also	$@ParticlePool@$
	//# \also	$@Particle@$


	class LineParticleSystem : public ParticleSystem
	{
		private:

			struct LineVertex
			{
				Point3D			position;
				ColorRGBA		color;
				Vector4D		tangent;
				Point2D			texcoord;
			};

			float						lengthMultiplier;

			ParticleJob					renderJob;

			VertexBuffer				vertexBuffer;
			DiffuseTextureAttribute		textureAttribute;

			static void JobRenderParticles(Job *job, void *cookie);
			static void FinalizeParticles(Job *job, void *cookie);

		protected:

			C4API LineParticleSystem(ParticleSystemType type, ParticlePoolBase *pool, const char *textureName = nullptr);
			C4API LineParticleSystem(const LineParticleSystem& lineParticleSystem, ParticlePoolBase *pool);

		public:

			C4API ~LineParticleSystem();

			float GetLengthMultiplier(void) const
			{
				return (lengthMultiplier);
			}

			void SetLengthMultiplier(float multiplier)
			{
				lengthMultiplier = multiplier;
			}

			C4API void Pack(Packer& data, unsigned_int32 packFlags) const override;
			C4API void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			C4API void Preprocess(void) override;
			C4API void Render(const FrustumCamera *camera, List<Renderable> *effectList) override;
	};


	//# \class	QuadParticleSystem		Represents a quad-based particle system node in a world.
	//
	//# The $QuadParticleSystem$ class represents a quad-based particle system node in a world.
	//
	//# \def	class QuadParticleSystem : public ParticleSystem
	//
	//# \ctor	QuadParticleSystem(ParticleSystemType type, ParticlePoolBase *pool, const char *textureName = nullptr);
	//
	//# The constructor has protected access. The $QuadParticleSystem$ class can only exist as the base class for a more specific type of particle effect.
	//
	//# \param	type			The type of particle system.
	//# \param	pool			The particle pool from which this effect creates particles. The particles in this pool must use the $@QuadParticle@$ structure.
	//# \param	textureName		The name of the texture map to use. If this is $nullptr$, then the default particle texture is used.
	//
	//# \desc
	//# The $QuadParticleSystem$ class is the base class for all quad-based particle effects.
	//
	//# \base	ParticleSystem		A $QuadParticleSystem$ node is a specific type of particle system.
	//
	//# \also	$@ParticlePool@$
	//# \also	$@QuadParticle@$


	class QuadParticleSystem : public ParticleSystem
	{
		private:

			struct QuadVertex
			{
				Point3D			position;
				ColorRGBA		color;
				Point2D			texcoord;
			};

			ParticleJob					renderJob;

			VertexBuffer				vertexBuffer;
			DiffuseTextureAttribute		textureAttribute;

			static void JobRenderParticles(Job *job, void *cookie);
			static void FinalizeParticles(Job *job, void *cookie);

		protected:

			C4API QuadParticleSystem(ParticleSystemType type, ParticlePoolBase *pool, const char *textureName = nullptr);
			C4API QuadParticleSystem(const QuadParticleSystem& quadParticleSystem, ParticlePoolBase *pool);

		public:

			C4API ~QuadParticleSystem();

			C4API void Preprocess(void) override;
			C4API void Render(const FrustumCamera *camera, List<Renderable> *effectList) override;
	};


	//# \class	FireParticleSystem		Represents a fire-based particle system node in a world.
	//
	//# The $FireParticleSystem$ class represents a fire-based particle system node in a world.
	//
	//# \def	class FireParticleSystem : public ParticleSystem
	//
	//# \ctor	FireParticleSystem(ParticleSystemType type, ParticlePoolBase *pool, const char *textureName);
	//
	//# The constructor has protected access. The $FireParticleSystem$ class can only exist as the base class for a more specific type of particle effect.
	//
	//# \param	type			The type of particle system.
	//# \param	pool			The particle pool from which this effect creates particles. The particles in this pool must use the $@FireParticle@$ structure.
	//# \param	textureName		The name of the texture map to use.
	//
	//# \desc
	//# The $FireParticleSystem$ class is the base class for all fire-based particle effects.
	//
	//# \base	ParticleSystem		A $FireParticleSystem$ node is a specific type of particle system.
	//
	//# \also	$@ParticlePool@$
	//# \also	$@FireParticle@$


	class FireParticleSystem : public ParticleSystem
	{
		private:

			struct FireVertex
			{
				Point3D			position;
				ColorRGBA		color;
				Vector4D		fire[3];
			};

			ParticleJob					renderJob;

			VertexBuffer				vertexBuffer;
			DiffuseTextureAttribute		textureAttribute;
			DiffuseTextureAttribute		noiseAttribute;

			static void JobRenderParticles(Job *job, void *cookie);
			static void FinalizeParticles(Job *job, void *cookie);

		protected:

			C4API FireParticleSystem(ParticleSystemType type, ParticlePoolBase *pool, const char *textureName);
			C4API FireParticleSystem(const FireParticleSystem& fireParticleSystem, ParticlePoolBase *pool);

		public:

			C4API ~FireParticleSystem();

			C4API void Preprocess(void) override;
			C4API void Render(const FrustumCamera *camera, List<Renderable> *effectList) override;
	};


	//# \class	PolyboardParticleSystem		Represents a polyboard-based particle system node in a world.
	//
	//# The $PolyboardParticleSystem$ class represents a polyboard-based particle system node in a world.
	//
	//# \def	class PolyboardParticleSystem : public ParticleSystem
	//
	//# \ctor	PolyboardParticleSystem(ParticleSystemType type, ParticlePoolBase *pool, const char *textureName = nullptr);
	//
	//# The constructor has protected access. The $PolyboardParticleSystem$ class can only exist as the base class for a more specific type of particle effect.
	//
	//# \param	type			The type of particle system.
	//# \param	pool			The particle pool from which this effect creates particles. The particles in this pool must use the $@PolyParticle@$ structure.
	//# \param	textureName		The name of the texture map to use. If this is $nullptr$, then the default particle texture is used.
	//
	//# \desc
	//# The $PolyboardParticleSystem$ class is the base class for all polyboard-based particle effects.
	//
	//# \base	ParticleSystem		A $PolyboardParticleSystem$ node is a specific type of particle system.
	//
	//# \also	$@ParticlePool@$
	//# \also	$@PolyParticle@$


	class PolyboardParticleSystem : public ParticleSystem
	{
		private:

			struct PolyVertex
			{
				Point3D			position;
				ColorRGBA		color;
				Vector4D		tangent;
				Vector2D		texcoord;
			};

			ParticleJob					renderJob;

			VertexBuffer				attributeVertexBuffer;
			VertexBuffer				indexVertexBuffer;

			DiffuseTextureAttribute		textureAttribute;

			static void JobRenderParticles(Job *job, void *cookie);
			static void FinalizeParticles(Job *job, void *cookie);

		protected:

			C4API PolyboardParticleSystem(ParticleSystemType type, ParticlePoolBase *pool, const char *textureName = nullptr);
			C4API PolyboardParticleSystem(const PolyboardParticleSystem& polyboardParticleSystem, ParticlePoolBase *pool);

		public:

			C4API ~PolyboardParticleSystem();

			C4API void Preprocess(void) override;
			C4API void Render(const FrustumCamera *camera, List<Renderable> *effectList) override;
	};


	//# \class	BlobParticleSystem		Represents a blob-based particle system node in a world.
	//
	//# The $BlobParticleSystem$ class represents a blob-based particle system node in a world.
	//
	//# \def	class BlobParticleSystem : public ParticleSystem
	//
	//# \ctor	BlobParticleSystem(ParticleSystemType type, ParticlePoolBase *pool, float scale, float maxRadius);
	//
	//# The constructor has protected access. The $BlobParticleSystem$ class can only exist as the base class for a more specific type of particle effect.
	//
	//# \param	type		The type of particle system.
	//# \param	pool		The particle pool from which this effect creates particles. The particles in this pool must use the $@BlobParticle@$ structure.
	//# \param	scale		The size of the voxels used to render the blob particles.
	//# \param	maxRadius	The maximum radius of any single blob particle in the particle system.
	//
	//# \desc
	//# The $BlobParticleSystem$ class is the base class for all blob-based particle effects. Blob particles are rendered into a set
	//# of voxel maps that are then triangulated with a fixed-polarity marching cubes implementation. A separate triangulation job is
	//# run for each connected island of blobs, and these jobs are distributed among all available processor cores.
	//#
	//# The value of the $scale$ parameter specifies the distance between voxels. A smaller value produces more highly-tessellated
	//# triangle meshes, which can reduce performance if the scale is too small. A higher value increases performance, but at the cost
	//# of surface smoothness. The value of $scale$ should generally be about the same as the smallest radius that will be used by
	//# any particle in the system.
	//#
	//# The $maxRadius$ parameter specifies the maximum radius that any particle in the system will have. This should generally be
	//# a multiple of the $scale$ parameter for best memory utilization. If any single particle has a radius larger than $maxRadius$,
	//# then the results are undefined. However, multiple particles can combine to form islands that are much larger than this maximum.
	//#
	//# The values of the $scale$ and $maxRadius$ parameters must together satisfy the following restriction, or the particle system
	//# may not render correctly:
	//#
	//# 18 &times; <code>totalParticleCount</code> &times; (&lceil;<code>maxRadius</code> / <code>scale</code>&rceil;)<sup>2</sup> &lt; 65535
	//#
	//# Here, the value of $totalParticleCount$ is the number of particles available in the particle pool.
	//
	//# \base	ParticleSystem		A $BlobParticleSystem$ node is a specific type of particle system.
	//
	//# \also	$@ParticlePool@$
	//# \also	$@BlobParticle@$


	class BlobParticleSystem : public ParticleSystem
	{
		private:

			typedef unsigned_int16 VoxelStorage[4];

			struct BlobVertex
			{
				Point3D			position;
				Vector3D		normal;
			};

			class BlobJob : public BatchJob
			{
				public:

					BlobParticle			*particle;

					Point3D					basePosition;
					float					voxelScale;

					unsigned_int32			baseVertexIndex;
					unsigned_int32			baseTriangleIndex;

					int32					maxVertexCount;
					int32					maxTriangleCount;

					int32					vertexCount;
					int32					triangleCount;

					volatile BlobVertex		*vertexArray;
					volatile Triangle		*triangleArray;

					BlobJob(ExecuteProc *execProc, void *cookie);
			};

			class BlobBatch : public Batch, public ListElement<BlobBatch>
			{
				public:

					BlobParticleSystem		*particleSystem;
					List<BlobParticle>		multipleList;
					int32					jobCount;

					BlobBatch(BlobParticleSystem *system);
			};

			float						voxelScale;
			float						inverseVoxelScale;

			float						maxParticleRadius;
			int32						maxParticleVertexCount;
			int32						maxParticleTriangleCount;

			char						*particleSystemStorage;
			unsigned_int32				*indexCountArray;
			machine_address				*indexOffsetArray;
			char						*voxelMapStorage;
			unsigned_int32				threadStorageSize;

			VertexBuffer				attributeVertexBuffer;
			VertexBuffer				indexVertexBuffer;

			BlobJob						*jobArray;
			BlobBatch					blobBatch;

			static List<BlobBatch>		batchList;

			float SortParticleList(List<BlobParticle> *inputList, int32 depth, float minValue, float maxValue, int32 index, List<BlobParticle> *outputList);
			void ClusterParticlesX(List<BlobParticle> *inputList, int32 depth, float xmin, float xmax, List<BlobParticle> *singleList, List<BlobParticle> *multipleList);
			void ClusterParticlesY(List<BlobParticle> *inputList, int32 depth, float ymin, float ymax, List<BlobParticle> *singleList, List<BlobParticle> *multipleList);
			void ClusterParticlesZ(List<BlobParticle> *inputList, int32 depth, float zmin, float zmax, List<BlobParticle> *singleList, List<BlobParticle> *multipleList);

			static Vector3D CalculateNormal(const Voxel *voxelMap, const Integer3D& voxelMapSize, const Integer3D& coord);
			static Vector3D CalculateNormal(const Voxel *voxelMap, const Integer3D& voxelMapSize, const Integer3D& coord0, const Integer3D& coord1, Fixed t, Fixed u);

			static void TriangulateBlob(const Voxel *voxelMap, const Integer3D& voxelMapSize, VoxelStorage *const (& deckStorage)[2], BlobJob *blobJob);
			static void SingleRenderVoxels(Voxel *voxelMap, int32 voxelMapSize, float center, float radius, const Vector3D& scaleAxis, float inverseScale);
			static void MultipleRenderVoxels(Voxel *voxelMap, const Integer3D& voxelMapSize, const Point3D& center, float radius, const Vector3D& scaleAxis, float inverseScale);
			static void JobRenderBlob(Job *job, void *cookie);

			void Finalize(List<Renderable> *effectList);

		protected:

			C4API BlobParticleSystem(ParticleSystemType type, ParticlePoolBase *pool);
			C4API BlobParticleSystem(ParticleSystemType type, ParticlePoolBase *pool, float scale, float maxRadius);
			C4API BlobParticleSystem(const BlobParticleSystem& blobParticleSystem, ParticlePoolBase *pool);

		public:

			C4API ~BlobParticleSystem();

			float GetVoxelScale(void) const
			{
				return (voxelScale);
			}

			float GetMaxParticleRadius(void) const
			{
				return (maxParticleRadius);
			}

			C4API void Pack(Packer& data, unsigned_int32 packFlags) const override;
			C4API void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			C4API void Preprocess(void) override;
			C4API void Neutralize(void) override;
			C4API void Render(const FrustumCamera *camera, List<Renderable> *effectList) override;

			static void FinishBatches(List<Renderable> *effectList);
	};


	//# \class	StarField		Renders a star field particle system at infinity.
	//
	//# The $StarField$ class renders a star field particle system at infinity.
	//
	//# \def	class StarField : public InfinitePointParticleSystem
	//
	//# \ctor	StarField(const char *name);
	//
	//# \param	name	The name of the star field resource to use. If this is $nullptr$, then the default star field $C4/stars$ is used.
	//
	//# \desc
	//# The $StarField$ class represents a particle system consisting of stars rendered infinitely far from the camera.
	//# The positions and brightnesses of the stars are read from the star field resource specified by the $name$
	//# parameter. A maximum of 12000 stars are rendered.
	//#
	//# A star field resource has the file extension $.sfd$ and contains a small header followed by a $StarData$ structure for each star.
	//# The first four bytes of a star field resource must contain the 32-bit value $0x00000001$, and the second four bytes must contain
	//# the total number of stars in the resource as a 32-bit value. Little endian byte order must be used throughout the file.
	//# Following the 8-byte header is an array of $StarData$ structures containing information about each star. A single $StarData$
	//# structure is 12 bytes in size and contains three 32-bit floating-point fields in the following order: (1) the right ascension of the
	//# star, in hours east of the vernal equinox, (2) the declination of the star, in degrees north of the celestial equator, and (3) the
	//# apparent magnitude of the star.
	//#
	//# The default star field resource $C4/stars$ contains data for the 12000 brightest stars visible from Earth.
	//
	//# \base	InfinitePointParticleSystem		A star field is a particle system that is rendered at infinity.


	class StarField : public InfinitePointParticleSystem
	{
		friend class ParticleSystemReg<StarField>;

		private:

			enum
			{
				kMaxParticleCount = 12000
			};

			ResourceName		starFieldName;
			unsigned_int32		starFieldFlags;

			bool				initializedFlag;

			ParticlePool<>		particlePool;
			Particle			particleArray[kMaxParticleCount];

			StarField();
			StarField(const StarField& starField);

			Node *Replicate(void) const override;

		public:

			C4API StarField(const char *name);
			C4API ~StarField();

			const ResourceName& GetStarFieldName(void) const
			{
				return (starFieldName);
			}

			void SetStarFieldName(const char *name)
			{
				starFieldName = name;
			}

			unsigned_int32 GetStarFieldFlags(void) const
			{
				return (starFieldFlags);
			}

			void SetStarFieldFlags(unsigned_int32 flags)
			{
				starFieldFlags = flags;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetCategoryCount(void) const override;
			Type GetCategoryType(int32 index, const char **title) const override;
			int32 GetCategorySettingCount(Type category) const override;
			Setting *GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const override;
			void SetCategorySetting(Type category, const Setting *setting) override;

			void Preprocess(void) override;
			void Neutralize(void) override;

			void Move(void) override;
			void Render(const FrustumCamera *camera, List<Renderable> *effectList) override;
	};
}


#endif

// ZYUQURM
