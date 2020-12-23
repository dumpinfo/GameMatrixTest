 

#ifndef C4Water_h
#define C4Water_h


//# \component	Physics Manager
//# \prefix		PhysicsMgr/


#include "C4Geometries.h"
#include "C4Controller.h"


namespace C4
{
	typedef Type	WaveGeneratorType;


	//# \enum	WaveGeneratorType

	enum : WaveGeneratorType
	{
		kWaveGeneratorPoint				= 'PONT',		//## Point wave generator. This generates a circular wave about a single point.
		kWaveGeneratorLine				= 'LINE',		//## Line wave generator. This generates a wave along a line segment.
		kWaveGeneratorRandom			= 'RAND'		//## Random wave generator. This generates random waves over an entire water block.
	};


	enum : ObjectType
	{
		kObjectWaterBlock				= 'WBLK'
	};


	enum : NodeType
	{
		kNodeWaterBlock					= 'WBLK'
	};


	enum : GeometryType
	{
		kGeometryWater					= 'WATR',
		kGeometryHorizonWater			= 'HWAT'
	};


	enum : ControllerType
	{
		kControllerWater				= 'WATR'
	};


	enum : FunctionType
	{
		kFunctionGeneratePointWave		= 'PONT',
		kFunctionGenerateLineWave		= 'LINE'
	};


	//## WaveGeneratorFlags

	enum
	{
		kWaveGeneratorUpdate			= 1 << 0,		//## The wave generator needs to be updated (read-only flag).
		kWaveGeneratorNonpersistent		= 1 << 1		//## The wave generator is nonpersistent, meaning that it will be automatically destroyed when it finishes running.
	};


	enum
	{
		kWaterRandomWaves				= 1 << 0
	};


	enum
	{
		kWaterDirectionEast,
		kWaterDirectionWest,
		kWaterDirectionNorth,
		kWaterDirectionSouth
	};


	enum
	{
		kWaterAwake,
		kWaterDecaying,
		kWaterAsleep
	};


	class WaterGeometry;
	class WaterController;


	//# \class	WaveGenerator		The base class for wave generators.
	//
	//# The $WaveGenerator$ class is the base class for wave generators.
	//
	//# \def	class WaveGenerator : public ListElement<WaveGenerator>, public Packable, public Memory<WaveGenerator>
	// 
	//# \ctor	WaveGenerator(WaveGeneratorType type);
	//
	//# \param	type	The type of the wave generator. 
	//
	//# \desc 
	//# The $WaveGenerator$ class is the base class for more specific types of wave generators.
	//
	//# \base	Utilities/ListElement<WaveGenerator>	Used internally by the Physics Manager. 
	//# \base	ResourceMgr/Packable					Wave generators can be packed for storage in resources.
	//# \base	MemoryMgr/Memory<WaveGenerator>			Wave generators are stored in a dedicated heap. 
	// 
	//# \also	$@PointWaveGenerator@$
	//# \also	$@LineWaveGenerator@$
	//# \also	$@WaterController@$
 

	//# \function	WaveGenerator::GetWaveGeneratorType		Returns the wave generator type.
	//
	//# \proto	WaveGeneratorType GetWaveGeneratorType(void) const;
	//
	//# \desc
	//# The $GetWaveGeneratorType$ function returns the wave generator type, which can be one of the following constants.
	//
	//# \table	WaveGeneratorType


	//# \function	WaveGenerator::GetWaveGeneratorFlags	Returns the wave generator flags.
	//
	//# \proto	unsigned_int32 GetWaveGeneratorFlags(void) const;
	//
	//# \desc
	//# The $GetWaveGeneratorFlags$ function returns the wave generator flags, which can be a combination (through logical OR) of the following constants.
	//
	//# \table	WaveGeneratorFlags
	//
	//# \also	$@WaveGenerator::SetWaveGeneratorFlags@$


	//# \function	WaveGenerator::SetWaveGeneratorFlags	Sets the wave generator flags.
	//
	//# \proto	void SetWaveGeneratorFlags(unsigned_int32 flags);
	//
	//# \param	flags	The new wave generator flags.
	//
	//# \desc
	//# The $SetWaveGeneratorFlags$ function sets the wave generator flags, which can be a combination (through logical OR) of the following constants.
	//
	//# \table	WaveGeneratorFlags
	//
	//# \also	$@WaveGenerator::GetWaveGeneratorFlags@$


	//# \function	WaveGenerator::Invalidate		Invalidates a wave generator.
	//
	//# \proto	void Invalidate(void);
	//
	//# \desc
	//# The $Invalidate$ function should be called after the position or size of a wave generator is changed to indicate that it needs to be updated
	//# before it is executed again.


	class WaveGenerator : public ListElement<WaveGenerator>, public Packable, public EngineMemory<WaveGenerator>
	{
		friend class WaterController;

		private:

			WaveGeneratorType		waveGeneratorType;
			unsigned_int32			waveGeneratorFlags;

			static WaveGenerator *Create(Unpacker& data, unsigned_int32 unpackFlags);

		protected:

			WaveGenerator(WaveGeneratorType type);
			WaveGenerator(const WaveGenerator& waveGenerator);

		public:

			virtual ~WaveGenerator();

			WaveGeneratorType GetWaveGeneratorType(void) const
			{
				return (waveGeneratorType);
			}

			unsigned_int32 GetWaveGeneratorFlags(void) const
			{
				return (waveGeneratorFlags);
			}

			void SetWaveGeneratorFlags(unsigned_int32 flags)
			{
				waveGeneratorFlags = flags;
			}

			void Invalidate(void)
			{
				waveGeneratorFlags |= kWaveGeneratorUpdate;
			}

			void PackType(Packer& data) const override;
			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			virtual void Update(const WaterController *controller);
			virtual bool Execute(const WaterController *controller) = 0;
	};


	//# \class	PointWaveGenerator		A wave generator that generates a circular wave about a single point.
	//
	//# The $PointWaveGenerator$ class represents a wave generator that generates a circular wave about a single point.
	//
	//# \def	class PointWaveGenerator final : public WaveGenerator
	//
	//# \ctor	PointWaveGenerator(const Point2D& position, float radius, float delta, int32 time);
	//
	//# \param	position	The block-relative position of the center of the waves.
	//# \param	radius		The radius of the area affected by the wave generator.
	//# \param	delta		The difference in elevation applied to the water surface during each time step. This can be positive or negative.
	//# \param	time		The number of time steps for which the wave generator runs. If this is zero, then the wave generator never stops.
	//
	//# \desc
	//# The $PointWaveGenerator$ class generates a circular wave about a single point in a water block.
	//
	//# \base	WaveGenerator		A $PointWaveGenerator$ is a specific type of wave generator.
	//
	//# \also	$@PointWaveGenerator@$
	//# \also	$@LineWaveGenerator@$


	//# \function	PointWaveGenerator::GetGeneratorPosition	Returns the position of a point wave generator.
	//
	//# \proto	const Point2D& GetGeneratorPosition(void) const;
	//
	//# \desc
	//# The $GetGeneratorPosition$ function returns the position of a point wave generator in the coordinate system of the water block.
	//
	//# \also	$@PointWaveGenerator::SetGeneratorPosition@$
	//# \also	$@PointWaveGenerator::GetGeneratorRadius@$
	//# \also	$@PointWaveGenerator::SetGeneratorRadius@$


	//# \function	PointWaveGenerator::SetGeneratorPosition	Sets the position of a point wave generator.
	//
	//# \proto	void SetGeneratorPosition(const Point2D& position);
	//
	//# \param	position	The new wave generator position.
	//
	//# \desc
	//# The $SetGeneratorPosition$ function sets the position of a point wave generator to that specified by the $position$ parameter.
	//# The position coordinates should be specified in the local coordinate system of the water block to which the generator pertains.
	//#
	//# If the position of a wave generator is changed after it is already running, then the $@WaveGenerator::Invalidate@$ function should
	//# also be called to indicate that the wave generator needs to be updated.
	//
	//# \also	$@PointWaveGenerator::GetGeneratorPosition@$
	//# \also	$@PointWaveGenerator::GetGeneratorRadius@$
	//# \also	$@PointWaveGenerator::SetGeneratorRadius@$


	//# \function	PointWaveGenerator::GetGeneratorRadius		Returns the radius of a point wave generator.
	//
	//# \proto	const Point2D& GetGeneratorPosition(void) const;
	//
	//# \desc
	//# The $GetGeneratorRadius$ function returns the radius of a point wave generator.
	//
	//# \also	$@PointWaveGenerator::SetGeneratorRadius@$
	//# \also	$@PointWaveGenerator::GetGeneratorPosition@$
	//# \also	$@PointWaveGenerator::SetGeneratorPosition@$


	//# \function	PointWaveGenerator::SetGeneratorRadius		Sets the radius of a point wave generator.
	//
	//# \proto	void SetGeneratorRadius(float radius);
	//
	//# \param	radius		The new wave generator radius.
	//
	//# \desc
	//# The $SetGeneratorRadius$ function sets the radius of a point wave generator to that specified by the $radius$ parameter.
	//# The radius determines the area inside which vertices on the water surface are affected by the wave generator.
	//#
	//# If the radius of a wave generator is changed after it is already running, then the $@WaveGenerator::Invalidate@$ function should
	//# also be called to indicate that the wave generator needs to be updated.
	//
	//# \also	$@PointWaveGenerator::GetGeneratorRadius@$
	//# \also	$@PointWaveGenerator::GetGeneratorPosition@$
	//# \also	$@PointWaveGenerator::SetGeneratorPosition@$


	class PointWaveGenerator final : public WaveGenerator
	{
		friend class WaveGenerator;

		private:

			struct WaterData
			{
				const WaterGeometry		*waterGeometry;
				Integer2D				generatorCenter;
				Range<Integer2D>		generatorBox;
			};

			Point2D			generatorPosition;
			float			generatorRadius;

			float			elevationDelta;
			int32			timeStepCount;

			int32			waterRowLength;
			float			waterScale;

			int32			waterCount;
			WaterData		waterData[4];

			PointWaveGenerator();

		public:

			C4API PointWaveGenerator(const Point2D& position, float radius, float delta, int32 time);
			C4API PointWaveGenerator(const PointWaveGenerator& pointWaveGenerator);
			C4API ~PointWaveGenerator();

			const Point2D& GetGeneratorPosition(void) const
			{
				return (generatorPosition);
			}

			void SetGeneratorPosition(const Point2D& position)
			{
				generatorPosition = position;
			}

			float GetGeneratorRadius(void) const
			{
				return (generatorRadius);
			}

			void SetGeneratorRadius(float radius)
			{
				generatorRadius = radius;
			}

			float GetElevationDelta(void) const
			{
				return (elevationDelta);
			}

			void SetElevationDelta(float delta)
			{
				elevationDelta = delta;
			}

			int32 GetTimeStepCount(void) const
			{
				return (timeStepCount);
			}

			void SetTimeStepCount(int32 time)
			{
				timeStepCount = time;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Update(const WaterController *controller) override;
			bool Execute(const WaterController *controller) override;
	};


	//# \class	LineWaveGenerator		A wave generator that generates a wave along a line segment.
	//
	//# The $LineWaveGenerator$ class represents a wave generator that generates a wave along a line segment.
	//
	//# \def	class LineWaveGenerator final : public WaveGenerator
	//
	//# \ctor	LineWaveGenerator(const Point2D& p1, const Point2D& p2, float radius, float delta, int32 time);
	//
	//# \param	p1			The block-relative position of one endpoint of the line segment.
	//# \param	p2			The block-relative position of the other endpoint of the line segment.
	//# \param	radius		The radius of the area affected by the wave generator.
	//# \param	delta		The difference in elevation applied to the water surface during each time step. This can be positive or negative.
	//# \param	time		The number of time steps for which the wave generator runs. If this is zero, then the wave generator never stops.
	//
	//# \desc
	//# The $LineWaveGenerator$ class generates a wave along a line segment in a water block.
	//
	//# \base	WaveGenerator		A $LineWaveGenerator$ is a specific type of wave generator.
	//
	//# \also	$@PointWaveGenerator@$
	//# \also	$@LineWaveGenerator@$


	//# \function	LineWaveGenerator::GetGeneratorPosition		Returns an endpoint position of a line wave generator.
	//
	//# \proto	const Point2D& GetGeneratorPosition(int32 index) const;
	//
	//# \param	index	The index of the endpoint to return. This must be 0 or 1.
	//
	//# \desc
	//# The $GetGeneratorPosition$ function returns an endpoint position of a line wave in the coordinate system of the water block.
	//# The $index$ parameter specifies which endpoint position is returned.
	//
	//# \also	$@LineWaveGenerator::SetGeneratorPosition@$
	//# \also	$@LineWaveGenerator::GetGeneratorRadius@$
	//# \also	$@LineWaveGenerator::SetGeneratorRadius@$


	//# \function	LineWaveGenerator::SetGeneratorPosition		Sets an endpoint position of a line wave generator.
	//
	//# \proto	void SetGeneratorPosition(int32 index, const Point2D& position);
	//
	//# \param	index		The index of the endpoint to set. This must be 0 or 1.
	//# \param	position	The new wave generator endpoint position.
	//
	//# \desc
	//# The $SetGeneratorPosition$ function sets an endpoint position of a line wave generator to that specified by the $position$ parameter.
	//# The position coordinates should be specified in the local coordinate system of the water block to which the generator pertains.
	//# The $index$ parameter specifies which endpoint position is set.
	//#
	//# If an endpoint position of a wave generator is changed after it is already running, then the $@WaveGenerator::Invalidate@$ function should
	//# also be called to indicate that the wave generator needs to be updated.
	//
	//# \also	$@LineWaveGenerator::GetGeneratorPosition@$
	//# \also	$@LineWaveGenerator::GetGeneratorRadius@$
	//# \also	$@LineWaveGenerator::SetGeneratorRadius@$


	//# \function	LineWaveGenerator::GetGeneratorRadius		Returns the radius of a line wave generator.
	//
	//# \proto	const Point2D& GetGeneratorPosition(void) const;
	//
	//# \desc
	//# The $GetGeneratorRadius$ function returns the radius of a line wave generator.
	//
	//# \also	$@LineWaveGenerator::SetGeneratorRadius@$
	//# \also	$@LineWaveGenerator::GetGeneratorPosition@$
	//# \also	$@LineWaveGenerator::SetGeneratorPosition@$


	//# \function	LineWaveGenerator::SetGeneratorRadius		Sets the radius of a line wave generator.
	//
	//# \proto	void SetGeneratorRadius(float radius);
	//
	//# \param	radius		The new wave generator radius.
	//
	//# \desc
	//# The $SetGeneratorRadius$ function sets the radius of a line wave generator to that specified by the $radius$ parameter.
	//# The radius determines the area inside which vertices on the water surface are affected by the wave generator.
	//#
	//# If the radius of a wave generator is changed after it is already running, then the $@WaveGenerator::Invalidate@$ function should
	//# also be called to indicate that the wave generator needs to be updated.
	//
	//# \also	$@LineWaveGenerator::GetGeneratorRadius@$
	//# \also	$@LineWaveGenerator::GetGeneratorPosition@$
	//# \also	$@LineWaveGenerator::SetGeneratorPosition@$


	class LineWaveGenerator final : public WaveGenerator
	{
		friend class WaveGenerator;

		private:

			struct WaterData
			{
				const WaterGeometry		*waterGeometry;
				Integer2D				generatorEndpoint[2];
				Range<Integer2D>		generatorBox;
				Fixed					lineOffset;
			};

			Point2D			generatorPosition[2];
			float			generatorRadius;

			float			elevationDelta;
			int32			timeStepCount;

			int32			waterRowLength;
			float			waterScale;

			bool			slopeFlag;
			Fixed			lineSlope;
			Fixed			slopeRadius;

			Vector2D		lineDirection;
			float			lineLength;

			int32			waterCount;
			WaterData		waterData[4];

			LineWaveGenerator();

		public:

			C4API LineWaveGenerator(const Point2D& p1, const Point2D& p2, float radius, float delta, int32 time);
			C4API LineWaveGenerator(const LineWaveGenerator& lineWaveGenerator);
			C4API ~LineWaveGenerator();

			const Point2D& GetGeneratorPosition(int32 index) const
			{
				return (generatorPosition[index]);
			}

			void SetGeneratorPosition(int32 index, const Point2D& position)
			{
				generatorPosition[index] = position;
			}

			float GetGeneratorRadius(void) const
			{
				return (generatorRadius);
			}

			void SetGeneratorRadius(float radius)
			{
				generatorRadius = radius;
			}

			float GetElevationDelta(void) const
			{
				return (elevationDelta);
			}

			void SetElevationDelta(float delta)
			{
				elevationDelta = delta;
			}

			int32 GetTimeStepCount(void) const
			{
				return (timeStepCount);
			}

			void SetTimeStepCount(int32 time)
			{
				timeStepCount = time;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Update(const WaterController *controller) override;
			bool Execute(const WaterController *controller) override;
	};


	class RandomWaveGenerator final : public WaveGenerator
	{
		friend class WaveGenerator;

		private:

			float			elevationDelta;
			int32			positionCount;

			int32			waterRowLength;
			unsigned_int32	executeCount;

			RandomWaveGenerator();

		public:

			C4API RandomWaveGenerator(float delta, int32 count);
			C4API RandomWaveGenerator(const RandomWaveGenerator& randomWaveGenerator);
			C4API ~RandomWaveGenerator();

			float GetElevationDelta(void) const
			{
				return (elevationDelta);
			}

			void SetElevationDelta(float delta)
			{
				elevationDelta = delta;
			}

			int32 GetPositionCount(void) const
			{
				return (positionCount);
			}

			void SetPositionCount(int32 time)
			{
				positionCount = time;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Update(const WaterController *controller) override;
			bool Execute(const WaterController *controller) override;
	};


	class WaterBlockObject final : public Object
	{
		friend class WorldMgr;

		private:

			int32			waterFieldDimension;
			float			waterFieldScale;

			float			waterHorizonDistance;

			Range<float>	landElevationRange;
			float			landTestElevation;

			WaterBlockObject();
			~WaterBlockObject();

		public:

			WaterBlockObject(int32 dimension, float scale, float horizon, const Range<float>& landRange);

			int32 GetWaterFieldDimension(void) const
			{
				return (waterFieldDimension);
			}

			void SetWaterFieldScale(int32 dimension)
			{
				waterFieldDimension = dimension;
			}

			float GetWaterFieldScale(void) const
			{
				return (waterFieldScale);
			}

			void SetWaterFieldScale(float scale)
			{
				waterFieldScale = scale;
			}

			float GetWaterHorizonDistance(void) const
			{
				return (waterHorizonDistance);
			}

			void SetWaterHorizonDistance(float scale)
			{
				waterHorizonDistance = scale;
			}

			const Range<float>& GetLandElevationRange(void) const
			{
				return (landElevationRange);
			}

			void SetLandElevationRange(const Range<float>& landRange)
			{
				landElevationRange = landRange;
			}

			float GetLandTestElevation(void) const
			{
				return (landTestElevation);
			}

			void SetLandTestElevation(float elevation)
			{
				landTestElevation = elevation;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetCategoryCount(void) const override;
			Type GetCategoryType(int32 index, const char **title) const override;
			int32 GetCategorySettingCount(Type category) const override;
			Setting *GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const override;
			void SetCategorySetting(Type category, const Setting *setting) override;

			int32 GetObjectSize(float *size) const override;
			void SetObjectSize(const float *size) override;
	};


	class WaterBlock final : public Node
	{
		friend class Node;

		private:

			Integer2D			blockSize;
			WaterGeometry		**waterGeometryTable;

			WaterBlock();
			WaterBlock(const WaterBlock& waterBlock);

			Node *Replicate(void) const override;

			bool CalculateBoundingBox(Box3D *box) const override;

		public:

			C4API WaterBlock(const Integer2D& size, int32 dimension, float scale, float horizon, const Range<float>& landRange);
			C4API ~WaterBlock();

			WaterBlockObject *GetObject(void) const
			{
				return (static_cast<WaterBlockObject *>(Node::GetObject()));
			}

			const Integer2D& GetBlockSize(void) const
			{
				return (blockSize);
			}

			void SetBlockSize(const Integer2D& size)
			{
				blockSize = size;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			C4API void Preprocess(void) override;
			C4API void Neutralize(void) override;

			C4API Vector2D GetBlockBoxSize(void) const;
			C4API WaterGeometry *GetWaterGeometry(int32 u, int32 v) const;
			C4API float GetWaterElevation(const Point2D& position) const;
			C4API float GetFilteredWaterElevation(const Point2D& position) const;
	};


	//# \class	WaterGeometryObject		Encapsulates data pertaining to a water geometry.
	//
	//# The $WaterGeometryObject$ class encapsulates data pertaining to a water geometry.
	//
	//# \def	class WaterGeometryObject final : public GeometryObject
	//
	//# \ctor	WaterGeometryObject(const Integer2D& coord);
	//
	//# \param	coord	The coordinates of the water geometry within the water block.
	//
	//# \desc
	//
	//# \base	WorldMgr/GeometryObject		A $WaterGeometryObject$ is an object that can be shared by multiple water geometry nodes.
	//
	//# \also	$@WaterGeometry@$
	//# \also	$@WaterController@$


	//# \function	WaterGeometryObject::GetGeometryCoord		Returns the water geometry coordinates.
	//
	//# \proto	const Integer2D& GetGeometryCoord(void) const;
	//
	//# \desc
	//# The $GetGeometryCoord$ function returns the coordinates of the water geometry within the water block.
	//# These coordinates are established when the water block is created and do not change.


	class WaterGeometryObject final : public GeometryObject
	{
		friend class GeometryObject;

		private:

			Integer2D		geometryCoord;

			WaterGeometryObject();
			~WaterGeometryObject();

		public:

			WaterGeometryObject(const Integer2D& coord);

			const Integer2D& GetGeometryCoord(void) const
			{
				return (geometryCoord);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			C4API void Build(Geometry *geometry);
	};


	class HorizonWaterGeometryObject final : public GeometryObject
	{
		friend class GeometryObject;

		private:

			int32		waterDirection;

			HorizonWaterGeometryObject();
			~HorizonWaterGeometryObject();

		public:

			HorizonWaterGeometryObject(int32 direction);

			int32 GetWaterDirection(void) const
			{
				return (waterDirection);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			C4API void Build(Geometry *geometry);
	};


	//# \class	WaterGeometry		Represents a water geometry node in a world.
	//
	//# The $WaterGeometry$ class represents a water geometry node in a world.
	//
	//# \def	class WaterGeometry final : public Geometry, public ListElement<WaterGeometry>
	//
	//# \ctor	WaterGeometry(WaterBlock *block, const Integer2D& coord);
	//
	//# \param	block		The water block to which the geometry belongs.
	//# \param	coord		The coordinates of the water geometry within the water block.
	//
	//# \desc
	//# The $WaterGeometry$ class represents a single water geometry node within a block
	//# of many water geometries that make up a complete body of water.
	//
	//# \base	WorldMgr/Geometry						Water is a specific type of geometry.
	//# \base	Utilities/ListElement<WaterGeometry>	Used internally by the Physics Manager.
	//
	//# \also	$@WaterGeometryObject@$
	//# \also	$@WaterController@$


	class WaterGeometry final : public Geometry, public ListElement<WaterGeometry>
	{
		friend class Geometry;
		friend class WaterController;

		private:

			enum
			{
				kWaterDirectionNortheast,
				kWaterDirectionNorthwest,
				kWaterDirectionSoutheast,
				kWaterDirectionSouthwest
			};

			WaterBlock				*blockNode;
			WaterController			*waterController;

			const WaterGeometry		*adjacentWater[4];
			WaterGeometry			*diagonalWater[4];

			unsigned_int32			waterState;
			int32					decayStepIndex;
			float					waterMultiplier;
			float					diagonalMultiplier[4];

			int32					waterFieldWidth;
			int32					waterFieldHeight;
			int32					waterVertexCount;

			char					*fieldStorage;
			float					*waterElevation[2];
			Vector4D				*waterNormal;

			int32					deadCornerCount;
			int32					deadCornerPosition[4];

			bool					waterUpdateFlag;
			int32					waterInvisibleTime;

			BatchJob				waterMoveJob;
			BatchJob				waterUpdateJob;
			VertexBuffer			dynamicVertexBuffer;

			WaterGeometry();
			WaterGeometry(const WaterGeometry& waterGeometry);

			Node *Replicate(void) const override;

			void AllocateFieldStorage(void);
			void InitVertexBuffer(VertexBuffer *vertexBuffer);

			bool CalculateBoundingBox(Box3D *box) const override;
			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

			void SetWaterMultiplier(float multiplier);

			static void JobMoveWater(Job *job, void *cookie);
			static void JobUpdateWater(Job *job, void *cookie);
			static void FinalizeWaterUpdate(Job *job, void *cookie);

		public:

			C4API WaterGeometry(WaterBlock *block, const Integer2D& coord);
			C4API ~WaterGeometry();

			using ListElement<WaterGeometry>::Previous;
			using ListElement<WaterGeometry>::Next;

			WaterGeometryObject *GetObject(void) const
			{
				return (static_cast<WaterGeometryObject *>(Node::GetObject()));
			}

			WaterBlock *GetBlockNode(void) const
			{
				return (blockNode);
			}

			const WaterGeometry *GetAdjacentWater(int32 direction) const
			{
				return (adjacentWater[direction]);
			}

			unsigned_int32 GetWaterState(void) const
			{
				return (waterState);
			}

			int32 GetWaterVertexCount(void) const
			{
				return (waterVertexCount);
			}

			float *const (& GetWaterElevation(void) const)[2]
			{
				return (waterElevation);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;

			void Preinitialize(void) override;
			void Preprocess(void) override;

			void UpdateWater(float squaredDistance);
			void UpdateInvisibleWater(float squaredDistance);

			C4API void LoadWaterElevation(const float *const (& elevation)[2]);
			C4API void ClearWaterElevation(void);
	};


	class HorizonWaterGeometry final : public Geometry
	{
		friend class Geometry;

		private:

			WaterBlock		*blockNode;

			HorizonWaterGeometry();
			HorizonWaterGeometry(const HorizonWaterGeometry& horizonWaterGeometry);

			Node *Replicate(void) const override;

			bool CalculateBoundingBox(Box3D *box) const override;
			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		public:

			C4API HorizonWaterGeometry(WaterBlock *block, int32 direction);
			C4API ~HorizonWaterGeometry();

			HorizonWaterGeometryObject *GetObject(void) const
			{
				return (static_cast<HorizonWaterGeometryObject *>(Node::GetObject()));
			}

			WaterBlock *GetBlockNode(void) const
			{
				return (blockNode);
			}

			void Preinitialize(void) override;
			void Preprocess(void) override;
	};


	//# \class	WaterController		Manages a dynamic water surface.
	//
	//# The $WaterController$ class manages a dynamic water surface.
	//
	//# \def	class WaterController final : public Controller
	//
	//# \ctor	WaterController();
	//
	//# \desc
	//# The $WaterController$ class is the controller assigned to a water block that manages an entire body of water.
	//
	//# \base	Controller/Controller		A $WaterController$ is a specific type of controller.
	//
	//# \also	$@WaveGenerator@$
	//# \also	$@WaterGeometry@$
	//# \also	$@WaterGeometryObject@$


	//# \function	WaterController::AddWaveGenerator		Adds a wave generator to a water controller.
	//
	//# \proto	void AddWaveGenerator(WaveGenerator *generator);
	//
	//# \param	generator		The wave generator to add.
	//
	//# \desc
	//# The $AddWaveGenerator$ function adds a wave generator to a water controller. The wave generator begins running the next time
	//# the water controller is moved, and it continues until it indicates that it is finished or until the wave generator is
	//# removed with the $@WaterController::RemoveWaveGenerator@$ function.
	//#
	//# If a wave generator is nonpersistent, then it is automatically destroyed by the water controller when it finishes running.
	//
	//# \also	$@WaterController::RemoveWaveGenerator@$
	//# \also	$@WaveGenerator@$


	//# \function	WaterController::RemoveWaveGenerator		Removes a wave generator from a water controller.
	//
	//# \proto	void RemoveWaveGenerator(WaveGenerator *generator);
	//
	//# \param	generator		The wave generator to remove.
	//
	//# \desc
	//# The $RemoveWaveGenerator$ function removes a wave generator from a water controller. The wave generator must have previously
	//# been added to the water controller with the $@WaterController::AddWaveGenerator@$ function.
	//
	//# \also	$@WaterController::AddWaveGenerator@$
	//# \also	$@WaveGenerator@$


	class WaterController final : public Controller
	{
		private:

			unsigned_int32			waterFlags;
			float					waterSpeed;
			float					waterViscosity;
			float					waterConstant[3];

			float					maxAwakeDistance;
			float					minAsleepDistance;
			int32					waterSleepTime;

			int32					moveTime;
			unsigned_int32			moveParity;

			List<WaterGeometry>		moveList;
			List<WaterGeometry>		decayList;

			Batch					moveBatch;

			List<WaveGenerator>		generatorList;
			RandomWaveGenerator		randomWaveGenerator;

			WaterController(const WaterController& waterController);

			Controller *Replicate(void) const override;

		public:

			WaterController();
			~WaterController();

			WaterBlock *GetTargetNode(void) const
			{
				return (static_cast<WaterBlock *>(Controller::GetTargetNode()));
			}

			float GetWaterSpeed(void) const
			{
				return (waterSpeed);
			}

			void SetWaterSpeed(float speed)
			{
				waterSpeed = speed;
			}

			float GetWaterViscosity(void) const
			{
				return (waterViscosity);
			}

			void SetWaterViscosity(float viscosity)
			{
				waterViscosity = viscosity;
			}

			float GetWaterConstant(int32 index) const
			{
				return (waterConstant[index]);
			}

			float GetMaxAwakeDistance(void) const
			{
				return (maxAwakeDistance);
			}

			float GetMinAsleepDistance(void) const
			{
				return (minAsleepDistance);
			}

			int32 GetWaterSleepTime(void) const
			{
				return (waterSleepTime);
			}

			unsigned_int32 GetMoveParity(void) const
			{
				return (moveParity);
			}

			const WaterGeometry *GetFirstMovingWaterGeometry(void) const
			{
				return (moveList.First());
			}

			const WaterGeometry *GetFirstDecayingWaterGeometry(void) const
			{
				return (decayList.First());
			}

			void AddWaveGenerator(WaveGenerator *generator)
			{
				generatorList.Append(generator);
			}

			void RemoveWaveGenerator(WaveGenerator *generator)
			{
				generatorList.Remove(generator);
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

			void Move(void) override;
			C4API void Advance(int32 dt);

			C4API void SetAwakeWaterState(WaterGeometry *water);
			C4API void SetDecayingWaterState(WaterGeometry *water);
	};


	class GeneratePointWaveFunction final : public Function
	{
		private:

			ConnectorKey		connectorKey;

			float				generatorRadius;
			float				elevationDelta;
			int32				timeStepCount;

			int32				scriptControllerIndex;

			GeneratePointWaveFunction(const GeneratePointWaveFunction& generatePointWaveFunction);

			Function *Replicate(void) const override;

		public:

			C4API GeneratePointWaveFunction();
			C4API ~GeneratePointWaveFunction();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Preprocess(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
			void Execute(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
	};


	class GenerateLineWaveFunction final : public Function
	{
		private:

			ConnectorKey		connectorKey[2];

			float				generatorRadius;
			float				elevationDelta;
			int32				timeStepCount;

			int32				scriptControllerIndex;

			GenerateLineWaveFunction(const GenerateLineWaveFunction& generateLineWaveFunction);

			Function *Replicate(void) const override;

		public:

			C4API GenerateLineWaveFunction();
			C4API ~GenerateLineWaveFunction();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Preprocess(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
			void Execute(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
	};
}


#endif

// ZYUQURM
