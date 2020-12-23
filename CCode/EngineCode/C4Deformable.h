 

#ifndef C4Deformable_h
#define C4Deformable_h


//# \component	Physics Manager
//# \prefix		PhysicsMgr/


#include "C4Body.h"
#include "C4Primitives.h"


namespace C4
{
	typedef Type RopeGeometryType;


	enum : PrimitiveType
	{
		kPrimitiveRope						= 'ROPE',
		kPrimitiveCloth						= 'CLTH'
	};


	enum : RopeGeometryType
	{
		kRopeGeometryTube					= 'TUBE',
		kRopeGeometryCross					= 'CROS'
	};


	enum : ControllerType
	{
		kControllerDeformableBody			= 'DFRM',
		kControllerRope						= 'ROPE',
		kControllerCloth					= 'CLTH'
	};


	enum : FunctionType
	{
		kFunctionBreakRopeAttachment		= 'BRAT'
	};


	//# \enum	DeformableBodyFlags

	enum
	{
		kDeformableBodyForceFieldInhibit	= 1 << 0		//## The deformable body is not affected by force fields. The global gravity force is still applied.
	};


	enum
	{
		kDeformableBodyAutoAsleep			= 1 << 0
	};


	enum
	{
		kRopeGeometrySwapLateralTexcoords	= 1 << 0,
		kRopeGeometryOffsetCrossTexcoords	= 1 << 1
	};


	enum
	{
		kMinRopeSubdivX				= 3,
		kMaxRopeSubdivX				= 255,
		kMinRopeSubdivY				= 4,
		kMaxRopeSubdivY				= 32
	};


	enum
	{
		kRopeAttachmentCount		= 2
	};


	enum
	{
		kMinClothSubdiv				= 3,
		kMaxClothSubdiv				= 47
	};


	enum
	{
		kClothLowerLeftCorner		= 1 << 0,
		kClothLowerRightCorner		= 1 << 1,
		kClothUpperRightCorner		= 1 << 2,
		kClothUpperLeftCorner		= 1 << 3,
		kClothBottomEdge			= 1 << 4,
		kClothRightEdge				= 1 << 5,
		kClothTopEdge				= 1 << 6,
		kClothLeftEdge				= 1 << 7
	};

 
	C4API extern const char kConnectorKeyBlocker[];
	C4API extern const char kConnectorKeyWind[];
 

	class PhysicsController; 
	class ForceFieldThreadData;
	class Field;
 

	//# \class	DeformableBodyController		Manages a dynamically deformable geometry. 
	// 
	//# The $DeformableBodyController$ class manages a dynamically deformable geometry.
	//
	//# \def	class DeformableBodyController : public BodyController, public ListElement<DeformableBodyController>
	// 
	//# \ctor	DeformableBodyController(ControllerType type);
	//
	//# The constructor has protected access. The $DeformableBodyController$ class can only exist as the base class for another class.
	//
	//# \param	type	The specific controller type of the subclass.
	//
	//# \desc
	//# The $DeformableBodyController$ class is the base class for controllers that manage some kind of
	//# dynamically deformable geometry.
	//
	//# \base	BodyController										A $DeformableBodyController$ is a specific type of body controller.
	//# \base	Utilities/ListElement<DeformableBodyController>		Used internally by the Physics Manager.


	class DeformableBodyController : public BodyController, public ListElement<DeformableBodyController>
	{
		friend class PhysicsController;

		protected:

			typedef Array<const Field *, 16>	FieldArray;

		private:

			unsigned_int32		deformableBodyFlags;
			unsigned_int32		deformableBodyState;

			int32				updateTime;
			int32				warmStartTime;
			int32				autoSleepTime;

			float				particleMass;
			float				particleVolume;
			float				particleRadius;

			float				halfInverseParticleMass;

			float				volumeMultiplier;
			float				internalResistance;
			Vector3D			windVelocity;

			Link<Node>			blockerNode;
			Link<Node>			windFieldNode;

			BatchJob			stepSimulationJob;

			void QueryCellForceFields(Site *site, ForceFieldThreadData *threadData, FieldArray& fieldArray) const;

		protected:

			DeformableBodyController(ControllerType type, Job::ExecuteProc *stepProc, void *cookie);
			DeformableBodyController(const DeformableBodyController& deformableController, Job::ExecuteProc *stepProc, void *cookie);

			void ResetUpdateTime(void)
			{
				updateTime = 0;
			}

			void SetParticleVolume(float volume)
			{
				particleVolume = volume;
			}

			void SetParticleRadius(float radius)
			{
				particleRadius = radius;
			}

			unsigned_int32 GetDeformableBodyState(void) const
			{
				return (deformableBodyState);
			}

			void SetDeformableBodyState(unsigned_int32 state)
			{
				deformableBodyState = state;
			}

			const float& GetHalfInverseParticleMass(void) const
			{
				return (halfInverseParticleMass);
			}

			const Node *GetBlockerNode(void) const
			{
				return (blockerNode);
			}

			const Node *GetWindFieldNode(void) const
			{
				return (windFieldNode);
			}

			void RecursiveWake(void) override;
			virtual void AutoSleep(void);

			Vector3D CalculateGravityForce(void) const;

			void QueryForceFields(FieldArray& fieldArray, int32 threadIndex) const;
			void ApplyForceFields(const ImmutableArray<const Field *>& fieldArray, int32 count, const Point3D *position, const SimdVector3D *velocity, SimdVector3D *restrict force);

		public:

			~DeformableBodyController();

			using ListElement<DeformableBodyController>::Previous;
			using ListElement<DeformableBodyController>::Next;

			unsigned_int32 GetDeformableBodyFlags(void) const
			{
				return (deformableBodyFlags);
			}

			void SetDeformableBodyFlags(unsigned_int32 flags)
			{
				deformableBodyFlags = flags;
			}

			int32 GetWarmStartTime(void) const
			{
				return (warmStartTime);
			}

			void SetWarmStartTime(int32 time)
			{
				warmStartTime = time;
			}

			int32 GetAutoSleepTime(void) const
			{
				return (autoSleepTime);
			}

			void SetAutoSleepTime(int32 time)
			{
				autoSleepTime = time;
			}

			const float& GetParticleMass(void) const
			{
				return (particleMass);
			}

			void SetParticleMass(float mass)
			{
				particleMass = mass;
			}

			const float& GetParticleVolume(void) const
			{
				return (particleVolume);
			}

			const float& GetParticleRadius(void) const
			{
				return (particleRadius);
			}

			const float& GetVolumeMultiplier(void) const
			{
				return (volumeMultiplier);
			}

			void SetVolumeMultiplier(float multiplier)
			{
				volumeMultiplier = multiplier;
			}

			const float& GetInternalResistance(void) const
			{
				return (internalResistance);
			}

			void SetInternalResistance(float resistance)
			{
				internalResistance = resistance;
			}

			const Vector3D& GetWindVelocity(void) const
			{
				return (windVelocity);
			}

			void SetWindVelocity(const Vector3D& velocity)
			{
				windVelocity = velocity;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Preprocess(void) override;
			C4API void Sleep(void) override;

			void Move(void) override;
			void Update(void) override;
	};


	//# \class	RopeGeometryObject		Encapsulates data pertaining to a rope primitive.
	//
	//# The $RopeGeometryObject$ class encapsulates data pertaining to a rope primitive.
	//
	//# \def	class RopeGeometryObject final : public PrimitiveGeometryObject
	//
	//# \ctor	RopeGeometryObject(float length, float radius, int32 count);
	//
	//# \param	length		The length of the rope.
	//# \param	radius		The radius of the rope.
	//# \param	count		The number of point masses in the rope.
	//
	//# \desc
	//
	//# \base	WorldMgr/PrimitiveGeometryObject	A $RopeGeometryObject$ is an object that can be shared by multiple rope geometry nodes.
	//
	//# \also	$@RopeGeometry@$
	//# \also	$@RopeController@$


	//# \function	RopeGeometryObject::GetRopeLength		Returns the rope length.
	//
	//# \proto	float GetRopeLength(void) const;
	//
	//# \desc
	//
	//# \also	$@RopeGeometryObject::SetRopeLength@$
	//# \also	$@RopeGeometryObject::GetRopeRadius@$
	//# \also	$@RopeGeometryObject::SetRopeRadius@$


	//# \function	RopeGeometryObject::SetRopeLength		Sets the rope length.
	//
	//# \proto	void SetRopeLength(float length);
	//
	//# \param	length		The new rope length.
	//
	//# \desc
	//
	//# \also	$@RopeGeometryObject::GetRopeLength@$
	//# \also	$@RopeGeometryObject::GetRopeRadius@$
	//# \also	$@RopeGeometryObject::SetRopeRadius@$


	//# \function	RopeGeometryObject::GetRopeRadius		Returns the rope radius.
	//
	//# \proto	float GetRopeRadius(void) const;
	//
	//# \desc
	//
	//# \also	$@RopeGeometryObject::SetRopeRadius@$
	//# \also	$@RopeGeometryObject::GetRopeLength@$
	//# \also	$@RopeGeometryObject::SetRopeLength@$


	//# \function	RopeGeometryObject::SetRopeRadius		Sets the rope radius.
	//
	//# \proto	void SetRopeRadius(float radius);
	//
	//# \param	radius		The new rope radius.
	//
	//# \desc
	//
	//# \also	$@RopeGeometryObject::GetRopeRadius@$
	//# \also	$@RopeGeometryObject::GetRopeLength@$
	//# \also	$@RopeGeometryObject::SetRopeLength@$


	class RopeGeometryObject final : public PrimitiveGeometryObject
	{
		friend class PrimitiveGeometryObject;

		private:

			float				ropeLength;
			float				ropeRadius;

			RopeGeometryType	ropeGeometryType;
			unsigned_int32		ropeGeometryFlags;

			float				texcoordScale;

			SurfaceData			staticSurfaceData[3];

			RopeGeometryObject();
			~RopeGeometryObject();

		public:

			RopeGeometryObject(float length, float radius, int32 count);

			float GetRopeLength(void) const
			{
				return (ropeLength);
			}

			void SetRopeLength(float length)
			{
				ropeLength = length;
			}

			const float& GetRopeRadius(void) const
			{
				return (ropeRadius);
			}

			void SetRopeRadius(float radius)
			{
				ropeRadius = radius;
			}

			RopeGeometryType GetRopeGeometryType(void) const
			{
				return (ropeGeometryType);
			}

			void SetRopeGeometryType(RopeGeometryType type)
			{
				ropeGeometryType = type;
			}

			unsigned_int32 GetRopeGeometryFlags(void) const
			{
				return (ropeGeometryFlags);
			}

			void SetRopeGeometryFlags(unsigned_int32 flags)
			{
				ropeGeometryFlags = flags;
			}

			float GetTexcoordScale(void) const
			{
				return (texcoordScale);
			}

			void SetTexcoordScale(float scale)
			{
				texcoordScale = scale;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetCategorySettingCount(Type category) const override;
			Setting *GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const override;
			void SetCategorySetting(Type category, const Setting *setting) override;

			int32 GetObjectSize(float *size) const override;
			void SetObjectSize(const float *size) override;

			void Build(Geometry *geometry) override;
	};


	//# \class	RopeGeometry		Represents a rope primitive node in a world.
	//
	//# The $RopeGeometry$ class represents a rope primitive node in a world.
	//
	//# \def	class RopeGeometry final : public PrimitiveGeometry
	//
	//# \ctor	RopeGeometry(const Vector2D& size, int32 width, int32 height);
	//
	//# \param	length		The length of the rope.
	//# \param	radius		The radius of the rope.
	//# \param	count		The number of point masses in the rope.
	//
	//# \desc
	//
	//# \base	WorldMgr/PrimitiveGeometry		A rope is a specific type of primitive geometry.
	//
	//# \also	$@RopeGeometryObject@$
	//# \also	$@RopeController@$


	class RopeGeometry final : public PrimitiveGeometry
	{
		friend class PrimitiveGeometry;

		private:

			const Box3D			*ropeBoundingBox;

			RopeGeometry();
			RopeGeometry(const RopeGeometry& ropeGeometry);

			Node *Replicate(void) const override;

			bool CalculateBoundingBox(Box3D *box) const override;
			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		public:

			C4API RopeGeometry(float length, float radius, int32 count);
			C4API ~RopeGeometry();

			RopeGeometryObject *GetObject(void) const
			{
				return (static_cast<RopeGeometryObject *>(Node::GetObject()));
			}

			int32 GetInternalConnectorCount(void) const override;
			const char *GetInternalConnectorKey(int32 index) const override;
			bool ValidConnectedNode(const ConnectorKey& key, const Node *node) const override;

			void Preprocess(void) override;
	};


	//# \class	RopeController		Manages a dynamic rope geometry.
	//
	//# The $RopeController$ class manages a dynamic rope geometry.
	//
	//# \def	class RopeController final : public DeformableBodyController
	//
	//# \ctor	RopeController();
	//
	//# \desc
	//# The $RopeController$ class encapsulates the dynamic rope simulation controller that is automatically
	//# assigned to a $@RopeGeometry@$ node.
	//
	//# \base	DeformableBodyController		A $RopeController$ is a specific type of deformable controller.
	//
	//# \wiki	Rope_Controller		Rope Controller


	class RopeController final : public DeformableBodyController
	{
		private:

			enum
			{
				kRopePositionInitial,
				kRopePositionFinal,
				kRopePositionPrevious,
				kRopePositionCount
			};

			static_assert(kRopePositionFinal == kRopePositionInitial + 1, "kRopePositionFinal must immediately follow kRopePositionInitial");

			struct RopeVertex
			{
				Point3D			position;
				Vector3D		velocity;
				Vector3D		normal;
				Vector4D		tangent;
			};

			int32					particleCount;

			float					stretchSpringConstant;
			float					stretchDamperConstant;
			float					stretchSpringDistance;

			float					bendSpringConstant;
			float					bendDamperConstant;
			float					bendSpringDistance;

			unsigned_int32			attachmentFlags;
			ConnectorKey			attachmentConnectorKey[kRopeAttachmentCount];

			Point3D					*ropePositionArray;
			Vector3D				*ropeVelocityArray;
			Vector3D				*ropeNormalArray;
			Vector4D				*ropeTangentArray[2];

			char					*fieldStorage;
			char					*vertexStorage;

			Vector3D				ropeNormal;

			SimdVector3D			*ropeVelocity;
			SimdVector3D			*ropeBaseForce;
			SimdVector3D			*ropeFieldForce;
			SimdPoint3D				*ropeUpdatePosition;
			Point3D					*ropePosition[kRopePositionCount];
			float					*ropeFlexibility;

			Link<Node>				attachmentNode[kRopeAttachmentCount];
			Link<Node>				rigidBodyNode[kRopeAttachmentCount];

			Box3D					ropeBoundingBox;

			BatchJob				ropeUpdateJob;
			VertexBuffer			ropeVertexBuffer;

			Vector2D				ropeTrigTable[2][kMaxRopeSubdivY];

			static Mutex			ropeMutex;

			RopeController(const RopeController& ropeController);

			Controller *Replicate(void) const override;

			void AllocateFieldStorage(void);
			void InitFlexibility(void);

			void WarmStart(void);
			void Simulate(int32 passCount, const Vector3D& gravityForce, const Vector3D& windVelocity, int32 threadIndex = JobMgr::kMaxWorkerThreadCount);
			static void JobStepSimulation(Job *job, void *cookie);

			static void JobUpdateRopeTube(Job *job, void *cookie);
			static void JobUpdateRopeCross(Job *job, void *cookie);
			static void FinalizeRopeUpdate(Job *job, void *cookie);

			void AutoSleep(void) override;

		public:

			RopeController();
			~RopeController();

			RopeGeometry *GetTargetNode(void) const
			{
				return (static_cast<RopeGeometry *>(Controller::GetTargetNode()));
			}

			float GetStretchSpringConstant(void) const
			{
				return (stretchSpringConstant);
			}

			float GetBendSpringConstant(void) const
			{
				return (bendSpringConstant);
			}

			void SetSpringConstants(float stretch, float bend)
			{
				stretchSpringConstant = stretch;
				bendSpringConstant = bend;
			}

			float GetStretchDamperConstant(void) const
			{
				return (stretchDamperConstant);
			}

			float GetBendDamperConstant(void) const
			{
				return (bendDamperConstant);
			}

			void SetDamperConstants(float stretch, float bend)
			{
				stretchDamperConstant = stretch;
				bendDamperConstant = bend;
			}

			unsigned_int32 GetAttachmentFlags(void) const
			{
				return (attachmentFlags);
			}

			void SetAttachmentFlags(unsigned_int32 flags)
			{
				attachmentFlags = flags;
			}

			const ConnectorKey& GetAttachmentConnectorKey(int32 index) const
			{
				return (attachmentConnectorKey[index]);
			}

			void SetAttachmentConnectorKey(int32 index, const ConnectorKey& key)
			{
				attachmentConnectorKey[index] = key;
			}

			const Box3D *GetRopeBoundingBox(void) const
			{
				return (&ropeBoundingBox);
			}

			static bool ValidNode(const Node *node);
			static void RegisterFunctions(ControllerRegistration *registration);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Preprocess(void) override;
			void Neutralize(void) override;
			void Update(void) override;

			C4API void BreakAttachment(int32 index);
	};


	class BreakRopeAttachmentFunction final : public Function
	{
		private:

			bool		breakFlag[kRopeAttachmentCount];

			BreakRopeAttachmentFunction(const BreakRopeAttachmentFunction& breakRopeAttachmentFunction);

			Function *Replicate(void) const override;

		public:

			BreakRopeAttachmentFunction();
			~BreakRopeAttachmentFunction();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
	};


	//# \class	ClothGeometryObject		Encapsulates data pertaining to a cloth primitive.
	//
	//# The $ClothGeometryObject$ class encapsulates data pertaining to a cloth primitive.
	//
	//# \def	class ClothGeometryObject final : public PrimitiveGeometryObject
	//
	//# \ctor	ClothGeometryObject(const Vector2D& size, int32 width, int32 height);
	//
	//# \param	size	The size of the cloth.
	//# \param	width	The width of the cloth lattice.
	//# \param	height	The height of the cloth lattice.
	//
	//# \desc
	//
	//# \base	WorldMgr/PrimitiveGeometryObject	A $ClothGeometryObject$ is an object that can be shared by multiple cloth geometry nodes.
	//
	//# \also	$@ClothGeometry@$
	//# \also	$@ClothController@$


	//# \function	ClothGeometryObject::GetClothSize		Returns the cloth size.
	//
	//# \proto	const Vector2D& GetClothSize(void) const;
	//
	//# \desc
	//
	//# \also	$@ClothGeometryObject::SetClothSize@$


	//# \function	ClothGeometryObject::SetClothSize		Sets the cloth size.
	//
	//# \proto	void SetClothSize(const Vector2D& size);
	//
	//# \param	size	The new cloth size.
	//
	//# \desc
	//
	//# \also	$@ClothGeometryObject::GetClothSize@$


	class ClothGeometryObject final : public PrimitiveGeometryObject
	{
		friend class PrimitiveGeometryObject;

		private:

			Vector2D			clothSize;

			SurfaceData			staticSurfaceData[2];

			ClothGeometryObject();
			~ClothGeometryObject();

		public:

			ClothGeometryObject(const Vector2D& size, int32 width, int32 height);

			const Vector2D& GetClothSize(void) const
			{
				return (clothSize);
			}

			void SetClothSize(const Vector2D& size)
			{
				clothSize = size;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetObjectSize(float *size) const override;
			void SetObjectSize(const float *size) override;

			void Build(Geometry *geometry) override;
	};


	//# \class	ClothGeometry		Represents a cloth primitive node in a world.
	//
	//# The $ClothGeometry$ class represents a cloth primitive node in a world.
	//
	//# \def	class ClothGeometry final : public PrimitiveGeometry
	//
	//# \ctor	ClothGeometry(const Vector2D& size, int32 width, int32 height);
	//
	//# \param	size	The size of the cloth.
	//# \param	width	The width of the cloth lattice.
	//# \param	height	The height of the cloth lattice.
	//
	//# \desc
	//
	//# \base	WorldMgr/PrimitiveGeometry		A cloth is a specific type of primitive geometry.
	//
	//# \also	$@ClothGeometryObject@$
	//# \also	$@ClothController@$


	class ClothGeometry final : public PrimitiveGeometry
	{
		friend class PrimitiveGeometry;

		private:

			const Box3D		*clothBoundingBox;

			ClothGeometry();
			ClothGeometry(const ClothGeometry& clothGeometry);

			Node *Replicate(void) const override;

			bool CalculateBoundingBox(Box3D *box) const override;
			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		public:

			C4API ClothGeometry(const Vector2D& size, int32 width, int32 height);
			C4API ~ClothGeometry();

			ClothGeometryObject *GetObject(void) const
			{
				return (static_cast<ClothGeometryObject *>(Node::GetObject()));
			}

			int32 GetInternalConnectorCount(void) const override;
			const char *GetInternalConnectorKey(int32 index) const override;
			bool ValidConnectedNode(const ConnectorKey& key, const Node *node) const override;

			void Preprocess(void) override;
	};


	//# \class	ClothController		Manages a dynamic cloth geometry.
	//
	//# The $ClothController$ class manages a dynamic cloth geometry.
	//
	//# \def	class ClothController final : public DeformableBodyController
	//
	//# \ctor	ClothController();
	//
	//# \desc
	//# The $ClothController$ class encapsulates the dynamic cloth simulation controller that is automatically
	//# assigned to a $@ClothGeometry@$ node.
	//
	//# \base	DeformableBodyController		A $ClothController$ is a specific type of deformable controller.
	//
	//# \wiki	Cloth_Controller	Cloth Controller


	class ClothController final : public DeformableBodyController
	{
		private:

			enum
			{
				kClothPositionFinal,
				kClothPositionPrevious,
				kClothPositionCount
			};

			struct ClothVertex
			{
				Point3D			position;
				Vector3D		velocity;
				Vector3D		normal;
				Vector4D		tangent;
			};

			struct SpringData
			{
				unsigned_int16		particleIndex1;
				unsigned_int16		particleIndex2;
			};

			int32					particleCount;
			int32					fieldWidth;
			int32					fieldHeight;

			int32					stretchSpringCount;
			int32					shearSpringCount;
			int32					bendSpringCount;

			float					stretchSpringConstant;
			float					stretchDamperConstant;
			float					stretchSpringDistance;
			SpringData				*stretchSpringData;

			float					shearSpringConstant;
			float					shearDamperConstant;
			float					shearSpringDistance;
			SpringData				*shearSpringData;

			float					bendSpringConstant;
			float					bendDamperConstant;
			float					bendSpringDistance;
			SpringData				*bendSpringData;

			unsigned_int32			attachmentFlags;

			Point3D					*clothPositionArray;
			Vector3D				*clothVelocityArray;
			Vector3D				*clothNormalArray;
			Vector4D				*clothTangentArray;

			char					*fieldStorage;
			char					*springStorage;
			char					*vertexStorage;

			SimdVector3D			*clothVelocity;
			SimdVector3D			*clothBaseForce;
			SimdVector3D			*clothFieldForce;
			Point3D					*clothPosition[kClothPositionCount];
			Vector3D				*clothBitangent;
			float					*clothFlexibility;

			Box3D					clothBoundingBox;

			BatchJob				clothUpdateJob;
			VertexBuffer			clothVertexBuffer;

			ClothController(const ClothController& clothController);

			Controller *Replicate(void) const override;

			void AllocateFieldStorage(void);
			void AllocateSpringStorage(void);
			void InitFlexibility(void);

			void WarmStart(void);
			void Simulate(int32 passCount, const Vector3D& gravityForce, const Vector3D& windVelocity, int32 threadIndex = JobMgr::kMaxWorkerThreadCount);
			static void JobStepSimulation(Job *job, void *cookie);

			static void JobUpdateCloth(Job *job, void *cookie);
			static void FinalizeClothUpdate(Job *job, void *cookie);

		public:

			ClothController();
			~ClothController();

			ClothGeometry *GetTargetNode(void) const
			{
				return (static_cast<ClothGeometry *>(Controller::GetTargetNode()));
			}

			float GetClothThickness(void) const
			{
				return (GetParticleRadius() * 2.0F);
			}

			void SetClothThickness(float thickness)
			{
				SetParticleRadius(thickness * 0.5F);
			}

			float GetStretchSpringConstant(void) const
			{
				return (stretchSpringConstant);
			}

			float GetShearSpringConstant(void) const
			{
				return (shearSpringConstant);
			}

			float GetBendSpringConstant(void) const
			{
				return (bendSpringConstant);
			}

			void SetSpringConstants(float stretch, float shear, float bend)
			{
				stretchSpringConstant = stretch;
				shearSpringConstant = shear;
				bendSpringConstant = bend;
			}

			float GetStretchDamperConstant(void) const
			{
				return (stretchDamperConstant);
			}

			float GetShearDamperConstant(void) const
			{
				return (shearDamperConstant);
			}

			float GetBendDamperConstant(void) const
			{
				return (bendDamperConstant);
			}

			void SetDamperConstants(float stretch, float shear, float bend)
			{
				stretchDamperConstant = stretch;
				shearDamperConstant = shear;
				bendDamperConstant = bend;
			}

			unsigned_int32 GetAttachmentFlags(void) const
			{
				return (attachmentFlags);
			}

			void SetAttachmentFlags(unsigned_int32 flags)
			{
				attachmentFlags = flags;
			}

			const Box3D *GetClothBoundingBox(void) const
			{
				return (&clothBoundingBox);
			}

			static bool ValidNode(const Node *node);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Preprocess(void) override;
			void Neutralize(void) override;
			void Update(void) override;
	};
}


#endif

// ZYUQURM
