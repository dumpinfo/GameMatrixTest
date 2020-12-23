 

#include "C4Water.h"
#include "C4World.h"
#include "C4Scripts.h"


using namespace C4;


namespace
{
	enum
	{
		kWaterTimeStep			= 16,
		kWaterDecayStepCount	= 500
	};

	const float kWaterDeltaTime = (float) kWaterTimeStep;
}


namespace C4
{
	template <> Heap EngineMemory<WaveGenerator>::heap("WaveGenerator", 8192);
	template class EngineMemory<WaveGenerator>;
}


WaveGenerator::WaveGenerator(WaveGeneratorType type)
{
	waveGeneratorType = type;
	waveGeneratorFlags = kWaveGeneratorUpdate;
}

WaveGenerator::WaveGenerator(const WaveGenerator& waveGenerator)
{
	waveGeneratorType = waveGenerator.waveGeneratorType;
	waveGeneratorFlags = kWaveGeneratorUpdate;
}

WaveGenerator::~WaveGenerator()
{
}

WaveGenerator *WaveGenerator::Create(Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (data.GetType())
	{
		case kWaveGeneratorPoint:

			return (new PointWaveGenerator);

		case kWaveGeneratorLine:

			return (new LineWaveGenerator);

		case kWaveGeneratorRandom:

			return (new RandomWaveGenerator);
	}

	return (nullptr);
}

void WaveGenerator::PackType(Packer& data) const
{
	data << waveGeneratorType;
}

void WaveGenerator::Pack(Packer& data, unsigned_int32 packFlags) const
{
	data << ChunkHeader('FLAG', 4);
	data << waveGeneratorFlags;

	data << TerminatorChunk;
}

void WaveGenerator::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	UnpackChunkList<WaveGenerator>(data, unpackFlags);
}

bool WaveGenerator::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> waveGeneratorFlags;
			waveGeneratorFlags |= kWaveGeneratorUpdate;
			return (true);
	}

	return (false);
}

void WaveGenerator::Update(const WaterController *controller)
{
	waveGeneratorFlags &= ~kWaveGeneratorUpdate;
}

 
PointWaveGenerator::PointWaveGenerator() : WaveGenerator(kWaveGeneratorPoint)
{
} 

PointWaveGenerator::PointWaveGenerator(const Point2D& position, float radius, float delta, int32 time) : WaveGenerator(kWaveGeneratorPoint) 
{
	generatorPosition = position;
	generatorRadius = radius; 

	elevationDelta = delta; 
	timeStepCount = time; 
}

PointWaveGenerator::PointWaveGenerator(const PointWaveGenerator& pointWaveGenerator) : WaveGenerator(pointWaveGenerator)
{ 
	generatorPosition = pointWaveGenerator.generatorPosition;
	generatorRadius = pointWaveGenerator.generatorRadius;

	elevationDelta = pointWaveGenerator.elevationDelta;
	timeStepCount = pointWaveGenerator.timeStepCount;
}

PointWaveGenerator::~PointWaveGenerator()
{
}

void PointWaveGenerator::Pack(Packer& data, unsigned_int32 packFlags) const
{
	WaveGenerator::Pack(data, packFlags);

	data << ChunkHeader('POSI', sizeof(Point2D));
	data << generatorPosition;

	data << ChunkHeader('RADI', 4);
	data << generatorRadius;

	data << ChunkHeader('DELT', 4);
	data << elevationDelta;

	data << ChunkHeader('TIME', 4);
	data << timeStepCount;

	data << TerminatorChunk;
}

void PointWaveGenerator::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	WaveGenerator::Unpack(data, unpackFlags);
	UnpackChunkList<PointWaveGenerator>(data, unpackFlags);
}

bool PointWaveGenerator::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'POSI':

			data >> generatorPosition;
			return (true);

		case 'RADI':

			data >> generatorRadius;
			return (true);

		case 'DELT':

			data >> elevationDelta;
			return (true);

		case 'TIME':

			data >> timeStepCount;
			return (true);
	}

	return (false);
}

void PointWaveGenerator::Update(const WaterController *controller)
{
	WaveGenerator::Update(controller);

	const WaterBlock *block = controller->GetTargetNode();
	const Integer2D& blockSize = block->GetBlockSize();

	const WaterBlockObject *object = block->GetObject();
	int32 dimension = object->GetWaterFieldDimension();
	waterRowLength = dimension + 1;

	float scale = object->GetWaterFieldScale();
	float inverseScale = 1.0F / scale;
	waterScale = scale;

	Point2D position = generatorPosition * inverseScale;
	Integer2D center((int32) (position.x + 0.5F), (int32) (position.y + 0.5F));

	int32 radius = (int32) PositiveCeil(generatorRadius * inverseScale);
	Integer2D min(MaxZero(center.x - radius), MaxZero(center.y - radius));
	Integer2D max(Min(center.x + radius, blockSize.x * dimension), Min(center.y + radius, blockSize.y * dimension));

	int32 count = 0;
	Integer2D minBlockCoord = min / dimension;
	Integer2D maxBlockCoord(Min(max.x / dimension, minBlockCoord.x + 1), Min(max.y / dimension, minBlockCoord.y + 1));

	for (machine v = minBlockCoord.y; v <= maxBlockCoord.y; v++)
	{
		for (machine u = minBlockCoord.x; u <= maxBlockCoord.x; u++)
		{
			const WaterGeometry *geometry = block->GetWaterGeometry(u, v);
			if (geometry)
			{
				WaterData *data = &waterData[count];
				data->waterGeometry = geometry;

				Integer2D base(u * dimension, v * dimension);
				Integer2D localMin = min - base;
				Integer2D localMax = max - base;
				data->generatorCenter = center - base;
				data->generatorBox.Set(Integer2D(MaxZero(localMin.x), MaxZero(localMin.y)), Integer2D(Min(localMax.x, dimension), Min(localMax.y, dimension)));

				if ((u == maxBlockCoord.x) || (!geometry->GetAdjacentWater(kWaterDirectionEast)))
				{
					data->generatorBox.max.x = Min(data->generatorBox.max.x, dimension - 1);
				}

				if ((u == minBlockCoord.x) || (!geometry->GetAdjacentWater(kWaterDirectionWest)))
				{
					data->generatorBox.min.x = Max(data->generatorBox.min.x, 1);
				}

				if ((v == maxBlockCoord.y) || (!geometry->GetAdjacentWater(kWaterDirectionNorth)))
				{
					data->generatorBox.max.y = Min(data->generatorBox.max.y, dimension - 1);
				}

				if ((v == minBlockCoord.y) || (!geometry->GetAdjacentWater(kWaterDirectionSouth)))
				{
					data->generatorBox.min.y = Max(data->generatorBox.min.y, 1);
				}

				if ((data->generatorBox.max.x >= data->generatorBox.min.x) && (data->generatorBox.max.y >= data->generatorBox.min.y))
				{
					count++;
				}
			}
		}
	}

	waterCount = count;
}

bool PointWaveGenerator::Execute(const WaterController *controller)
{
	const WaterData *data = waterData;
	for (machine a = 0; a < waterCount; a++)
	{
		if (data[a].waterGeometry->GetWaterState() != kWaterAwake)
		{
			return ((timeStepCount == 0) || (--timeStepCount > 0));
		}
	}

	unsigned_int32 parity = controller->GetMoveParity();

	float r2 = generatorRadius * generatorRadius;
	float f = elevationDelta / r2;

	for (machine a = 0; a < waterCount; a++)
	{
		const WaterGeometry *geometry = data->waterGeometry;
		float *const (& elevation)[2] = geometry->GetWaterElevation();

		for (machine j = data->generatorBox.min.y; j <= data->generatorBox.max.y; j++)
		{
			float dy = (float) (j - data->generatorCenter.y) * waterScale;

			int32 k = j * waterRowLength;
			float *restrict elev1 = elevation[parity] + k;
			float *restrict elev2 = elevation[parity ^ 1] + k;

			for (machine i = data->generatorBox.min.x; i <= data->generatorBox.max.x; i++)
			{
				float dx = (float) (i - data->generatorCenter.x) * waterScale;

				float dh = FmaxZero(r2 - dx * dx - dy * dy) * f;
				elev1[i] += dh;
				elev2[i] += dh * 0.5F;
			}
		}

		data++;
	}

	return ((timeStepCount == 0) || (--timeStepCount > 0));
}


LineWaveGenerator::LineWaveGenerator() : WaveGenerator(kWaveGeneratorLine)
{
}

LineWaveGenerator::LineWaveGenerator(const Point2D& p1, const Point2D& p2, float radius, float delta, int32 time) : WaveGenerator(kWaveGeneratorLine)
{
	generatorPosition[0] = p1;
	generatorPosition[1] = p2;
	generatorRadius = radius;

	elevationDelta = delta;
	timeStepCount = time;
}

LineWaveGenerator::LineWaveGenerator(const LineWaveGenerator& lineWaveGenerator) : WaveGenerator(lineWaveGenerator)
{
	generatorPosition[0] = lineWaveGenerator.generatorPosition[0];
	generatorPosition[1] = lineWaveGenerator.generatorPosition[1];
	generatorRadius = lineWaveGenerator.generatorRadius;

	elevationDelta = lineWaveGenerator.elevationDelta;
	timeStepCount = lineWaveGenerator.timeStepCount;
}

LineWaveGenerator::~LineWaveGenerator()
{
}

void LineWaveGenerator::Pack(Packer& data, unsigned_int32 packFlags) const
{
	WaveGenerator::Pack(data, packFlags);

	data << ChunkHeader('POSI', sizeof(Point2D) * 2);
	data << generatorPosition[0];
	data << generatorPosition[1];

	data << ChunkHeader('RADI', 4);
	data << generatorRadius;

	data << ChunkHeader('DELT', 4);
	data << elevationDelta;

	data << ChunkHeader('TIME', 4);
	data << timeStepCount;

	data << TerminatorChunk;
}

void LineWaveGenerator::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	WaveGenerator::Unpack(data, unpackFlags);
	UnpackChunkList<LineWaveGenerator>(data, unpackFlags);
}

bool LineWaveGenerator::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'POSI':

			data >> generatorPosition[0];
			data >> generatorPosition[1];
			return (true);

		case 'RADI':

			data >> generatorRadius;
			return (true);

		case 'DELT':

			data >> elevationDelta;
			return (true);

		case 'TIME':

			data >> timeStepCount;
			return (true);
	}

	return (false);
}

void LineWaveGenerator::Update(const WaterController *controller)
{
	Fixed	lineOffset;

	WaveGenerator::Update(controller);

	const WaterBlock *block = controller->GetTargetNode();
	const Integer2D& blockSize = block->GetBlockSize();

	const WaterBlockObject *object = block->GetObject();
	int32 dimension = object->GetWaterFieldDimension();
	waterRowLength = dimension + 1;

	float scale = object->GetWaterFieldScale();
	float inverseScale = 1.0F / scale;
	waterScale = scale;

	int32 radius = (int32) PositiveCeil(generatorRadius * inverseScale);

	Point2D p1 = generatorPosition[0] * inverseScale;
	Point2D p2 = generatorPosition[1] * inverseScale;
	float dx = p2.x - p1.x;
	float dy = p2.y - p1.y;

	if (Fabs(dx) > Fabs(dy))
	{
		slopeFlag = true;

		if (dx < 0.0F)
		{
			dx = -dx;
			dy = -dy;
			Point2D q = p1;
			p1 = p2;
			p2 = q;
		}

		float m = dy / dx;
		lineSlope = (Fixed) (m * 65536.0F);
		lineOffset = (Fixed) ((p1.y - p1.x * m) * 65536.0F);
		slopeRadius = radius * (Fixed) (Sqrt(m * m + 1.0F) * 65536.0F);
	}
	else
	{
		slopeFlag = false;

		if (dy < 0.0F)
		{
			dx = -dx;
			dy = -dy;
			Point2D q = p1;
			p1 = p2;
			p2 = q;
		}

		float m = dx / dy;
		lineSlope = (Fixed) (m * 65536.0F);
		lineOffset = (Fixed) ((p1.x - p1.y * m) * 65536.0F);
		slopeRadius = radius * (Fixed) (Sqrt(m * m + 1.0F) * 65536.0F);
	}

	Integer2D endpoint1((int32) (p1.x + 0.5F), (int32) (p1.y + 0.5F));
	Integer2D endpoint2((int32) (p2.x + 0.5F), (int32) (p2.y + 0.5F));

	Point2D q1((float) endpoint1.x, (float) endpoint1.y);
	Point2D q2((float) endpoint2.x, (float) endpoint2.y);

	lineDirection = q2 - q1;
	float m = Magnitude(lineDirection);
	lineDirection /= m;
	lineLength = m * scale;

	Integer2D min(MaxZero(Min(endpoint1.x, endpoint2.x) - radius), MaxZero(Min(endpoint1.y, endpoint2.y) - radius));
	Integer2D max(Min(Max(endpoint1.x, endpoint2.x) + radius, blockSize.x * dimension), Min(Max(endpoint1.y, endpoint2.y) + radius, blockSize.y * dimension));

	int32 count = 0;
	Integer2D minBlockCoord = min / dimension;
	Integer2D maxBlockCoord(Min(max.x / dimension, minBlockCoord.x + 1), Min(max.y / dimension, minBlockCoord.y + 1));

	for (machine v = minBlockCoord.y; v <= maxBlockCoord.y; v++)
	{
		for (machine u = minBlockCoord.x; u <= maxBlockCoord.x; u++)
		{
			const WaterGeometry *geometry = block->GetWaterGeometry(u, v);
			if (geometry)
			{
				WaterData *data = &waterData[count];
				data->waterGeometry = geometry;

				Integer2D base(u * dimension, v * dimension);

				if (slopeFlag)
				{
					data->lineOffset = lineOffset - (base.y << 16) + lineSlope * base.x;
				}
				else
				{
					data->lineOffset = lineOffset - (base.x << 16) + lineSlope * base.y;
				}

				Integer2D localMin = min - base;
				Integer2D localMax = max - base;
				data->generatorEndpoint[0] = endpoint1 - base;
				data->generatorEndpoint[1] = endpoint2 - base;
				data->generatorBox.Set(Integer2D(MaxZero(localMin.x), MaxZero(localMin.y)), Integer2D(Min(localMax.x, dimension), Min(localMax.y, dimension)));

				if ((u == maxBlockCoord.x) || (!geometry->GetAdjacentWater(kWaterDirectionEast)))
				{
					data->generatorBox.max.x = Min(data->generatorBox.max.x, dimension - 1);
				}

				if ((u == minBlockCoord.x) || (!geometry->GetAdjacentWater(kWaterDirectionWest)))
				{
					data->generatorBox.min.x = Max(data->generatorBox.min.x, 1);
				}

				if ((v == maxBlockCoord.y) || (!geometry->GetAdjacentWater(kWaterDirectionNorth)))
				{
					data->generatorBox.max.y = Min(data->generatorBox.max.y, dimension - 1);
				}

				if ((v == minBlockCoord.y) || (!geometry->GetAdjacentWater(kWaterDirectionSouth)))
				{
					data->generatorBox.min.y = Max(data->generatorBox.min.y, 1);
				}

				if ((data->generatorBox.max.x >= data->generatorBox.min.x) && (data->generatorBox.max.y >= data->generatorBox.min.y))
				{
					count++;
				}
			}
		}
	}

	waterCount = count;
}

bool LineWaveGenerator::Execute(const WaterController *controller)
{
	const WaterData *data = waterData;
	for (machine a = 0; a < waterCount; a++)
	{
		if (data[a].waterGeometry->GetWaterState() != kWaterAwake)
		{
			return ((timeStepCount == 0) || (--timeStepCount > 0));
		}
	}

	unsigned_int32 parity = controller->GetMoveParity();

	float r2 = generatorRadius * generatorRadius;
	float f = elevationDelta / r2;

	for (machine a = 0; a < waterCount; a++)
	{
		const WaterGeometry *geometry = data->waterGeometry;
		float *const (& elevation)[2] = geometry->GetWaterElevation();

		if (slopeFlag)
		{
			for (machine i = data->generatorBox.min.x; i <= data->generatorBox.max.x; i++)
			{
				float dx = (float) (i - data->generatorEndpoint[0].x) * waterScale;

				Fixed line = lineSlope * i + data->lineOffset;
				int32 jmin = Max((line - slopeRadius) >> 16, data->generatorBox.min.y);
				int32 jmax = Min((line + slopeRadius + 65535) >> 16, data->generatorBox.max.y);

				int32 k = jmin * waterRowLength + i;
				float *restrict elev1 = elevation[parity];
				float *restrict elev2 = elevation[parity ^ 1];

				for (machine j = jmin; j <= jmax; j++)
				{
					float	dh;

					float dy = (float) (j - data->generatorEndpoint[0].y) * waterScale;

					float d = dx * lineDirection.x + dy * lineDirection.y;
					if (d < 0.0F)
					{
						dh = FmaxZero(r2 - dx * dx - dy * dy) * f;
					}
					else if (d > lineLength)
					{
						float x = (float) (i - data->generatorEndpoint[1].x) * waterScale;
						float y = (float) (j - data->generatorEndpoint[1].y) * waterScale;
						dh = FmaxZero(r2 - x * x - y * y) * f;
					}
					else
					{
						dh = FmaxZero(r2 + d * d - dx * dx - dy * dy) * f;
					}

					elev1[k] += dh;
					elev2[k] += dh * 0.5F;

					k += waterRowLength;
				}
			}
		}
		else
		{
			for (machine j = data->generatorBox.min.y; j <= data->generatorBox.max.y; j++)
			{
				float dy = (float) (j - data->generatorEndpoint[0].y) * waterScale;

				Fixed line = lineSlope * j + data->lineOffset;
				int32 imin = Max((line - slopeRadius) >> 16, data->generatorBox.min.x);
				int32 imax = Min((line + slopeRadius + 65535) >> 16, data->generatorBox.max.x);

				int32 k = j * waterRowLength;
				float *restrict elev1 = elevation[parity] + k;
				float *restrict elev2 = elevation[parity ^ 1] + k;

				for (machine i = imin; i <= imax; i++)
				{
					float	dh;

					float dx = (float) (i - data->generatorEndpoint[0].x) * waterScale;

					float d = dx * lineDirection.x + dy * lineDirection.y;
					if (d < 0.0F)
					{
						dh = FmaxZero(r2 - dx * dx - dy * dy) * f;
					}
					else if (d > lineLength)
					{
						float x = (float) (i - data->generatorEndpoint[1].x) * waterScale;
						float y = (float) (j - data->generatorEndpoint[1].y) * waterScale;
						dh = FmaxZero(r2 - x * x - y * y) * f;
					}
					else
					{
						dh = FmaxZero(r2 + d * d - dx * dx - dy * dy) * f;
					}

					elev1[i] += dh;
					elev2[i] += dh * 0.5F;
				}
			}
		}

		data++;
	}

	return ((timeStepCount == 0) || (--timeStepCount > 0));
}


RandomWaveGenerator::RandomWaveGenerator() : WaveGenerator(kWaveGeneratorRandom)
{
}

RandomWaveGenerator::RandomWaveGenerator(float delta, int32 count) : WaveGenerator(kWaveGeneratorRandom)
{
	elevationDelta = delta;
	positionCount = count;
}

RandomWaveGenerator::RandomWaveGenerator(const RandomWaveGenerator& randomWaveGenerator) : WaveGenerator(randomWaveGenerator)
{
	elevationDelta = randomWaveGenerator.elevationDelta;
	positionCount = randomWaveGenerator.positionCount;
}

RandomWaveGenerator::~RandomWaveGenerator()
{
}

void RandomWaveGenerator::Pack(Packer& data, unsigned_int32 packFlags) const
{
	WaveGenerator::Pack(data, packFlags);

	data << ChunkHeader('DELT', 4);
	data << elevationDelta;

	data << ChunkHeader('PCNT', 4);
	data << positionCount;

	data << TerminatorChunk;
}

void RandomWaveGenerator::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	WaveGenerator::Unpack(data, unpackFlags);
	UnpackChunkList<RandomWaveGenerator>(data, unpackFlags);
}

bool RandomWaveGenerator::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'DELT':

			data >> elevationDelta;
			return (true);

		case 'PCNT':

			data >> positionCount;
			return (true);
	}

	return (false);
}

void RandomWaveGenerator::Update(const WaterController *controller)
{
	const WaterBlock *block = controller->GetTargetNode();
	const WaterBlockObject *object = block->GetObject();
	int32 dimension = object->GetWaterFieldDimension();
	waterRowLength = dimension + 1;

	executeCount = 3;
}

bool RandomWaveGenerator::Execute(const WaterController *controller)
{
	if ((controller->GetMoveParity() == 0) && ((++executeCount & 3) == 0))
	{
		int32 randomRow = waterRowLength - 4;
		int32 randomSize = randomRow * randomRow;

		const WaterGeometry *geometry = controller->GetFirstMovingWaterGeometry();
		while (geometry)
		{
			float *const (& elevation)[2] = geometry->GetWaterElevation();

			for (machine b = 0; b < positionCount; b++)
			{
				int32 k = Math::Random(randomSize);
				int32 j = k / randomRow;
				int32 i = k - j * randomRow;
				int32 position = (j + 2) * waterRowLength + (i + 2);

				float *restrict elev1 = elevation[0] + position;
				float *restrict elev2 = elevation[1] + position;

				float delta = Math::RandomFloat(elevationDelta) - elevationDelta * 0.5F;
				float d1 = delta * 0.25F;
				float d2 = delta * 0.125F;

				elev1[0] += delta;
				elev1[-1] += d1;
				elev1[1] += d1;
				elev1[waterRowLength] += d1;
				elev1[-waterRowLength] += d1;

				elev2[0] += delta * 0.5F;
				elev2[-1] += d2;
				elev2[1] += d2;
				elev2[waterRowLength] += d2;
				elev2[-waterRowLength] += d2;
			}

			geometry = geometry->Next();
		}
	}

	return (true);
}


WaterBlockObject::WaterBlockObject() : Object(kObjectWaterBlock)
{
}

WaterBlockObject::WaterBlockObject(int32 dimension, float scale, float horizon, const Range<float>& landRange) : Object(kObjectWaterBlock)
{
	waterFieldDimension = dimension;
	waterFieldScale = scale;

	waterHorizonDistance = horizon;

	landElevationRange = landRange;
	landTestElevation = landRange.max;
}

WaterBlockObject::~WaterBlockObject()
{
}

void WaterBlockObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	data << ChunkHeader('WDIM', 4);
	data << waterFieldDimension;

	data << ChunkHeader('SCAL', 4);
	data << waterFieldScale;

	data << ChunkHeader('HRZN', 4);
	data << waterHorizonDistance;

	data << ChunkHeader('LRNG', sizeof(Range<float>));
	data << landElevationRange;

	data << ChunkHeader('LTST', 4);
	data << landTestElevation;

	data << TerminatorChunk;
}

void WaterBlockObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	UnpackChunkList<WaterBlockObject>(data, unpackFlags);
}

bool WaterBlockObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'WDIM':

			data >> waterFieldDimension;
			return (true);

		case 'SCAL':

			data >> waterFieldScale;
			return (true);

		case 'HRZN':

			data >> waterHorizonDistance;
			return (true);

		case 'LRNG':

			data >> landElevationRange;
			return (true);

		case 'LTST':

			data >> landTestElevation;
			return (true);
	}

	return (false);
}

int32 WaterBlockObject::GetCategoryCount(void) const
{
	return (1);
}

Type WaterBlockObject::GetCategoryType(int32 index, const char **title) const
{
	if (index == 0)
	{
		*title = TheInterfaceMgr->GetStringTable()->GetString(StringID(kObjectWaterBlock));
		return (kObjectWaterBlock);
	}

	return (0);
}

int32 WaterBlockObject::GetCategorySettingCount(Type category) const
{
	if (category == kObjectWaterBlock)
	{
		return (4);
	}

	return (0);
}

Setting *WaterBlockObject::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kObjectWaterBlock)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();

		if (index == 0)
		{
			const char *title = table->GetString(StringID(kObjectWaterBlock, 'WBLK'));
			return (new HeadingSetting(kObjectWaterBlock, title));
		}

		if (index == 1)
		{
			const char *title = table->GetString(StringID(kObjectWaterBlock, 'WBLK', 'LMIN'));
			return (new TextSetting('LMIN', landElevationRange.min, title));
		}

		if (index == 2)
		{
			const char *title = table->GetString(StringID(kObjectWaterBlock, 'WBLK', 'LMAX'));
			return (new TextSetting('LMAX', landElevationRange.max, title));
		}

		if (index == 3)
		{
			const char *title = table->GetString(StringID(kObjectWaterBlock, 'WBLK', 'LTST'));
			return (new TextSetting('LTST', landTestElevation, title));
		}
	}

	return (nullptr);
}

void WaterBlockObject::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kObjectWaterBlock)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'LMIN')
		{
			landElevationRange.min = Fmin(Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText()), -1.0F);
		}
		else if (identifier == 'LMAX')
		{
			landElevationRange.max = Fmax(Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText()), 1.0F);
		}
		else if (identifier == 'LTST')
		{
			landTestElevation = Fmax(Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText()), 1.0F);
		}
	}
}

int32 WaterBlockObject::GetObjectSize(float *size) const
{
	size[0] = waterFieldScale;
	return (1);
}

void WaterBlockObject::SetObjectSize(const float *size)
{
	waterFieldScale = size[0];
}


WaterBlock::WaterBlock() : Node(kNodeWaterBlock)
{
	waterGeometryTable = nullptr;
}

WaterBlock::WaterBlock(const Integer2D& size, int32 dimension, float scale, float horizon, const Range<float>& landRange) : Node(kNodeWaterBlock)
{
	blockSize = size;
	waterGeometryTable = nullptr;

	SetNewObject(new WaterBlockObject(dimension, scale, horizon, landRange));
	SetController(new WaterController);
}

WaterBlock::WaterBlock(const WaterBlock& waterBlock) : Node(waterBlock)
{
	blockSize = waterBlock.blockSize;
	waterGeometryTable = nullptr;
}

WaterBlock::~WaterBlock()
{
	delete[] waterGeometryTable;
}

Node *WaterBlock::Replicate(void) const
{
	return (new WaterBlock(*this));
}

void WaterBlock::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Node::Pack(data, packFlags);

	data << ChunkHeader('SIZE', sizeof(Integer2D));
	data << blockSize;

	data << TerminatorChunk;
}

void WaterBlock::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Node::Unpack(data, unpackFlags);
	UnpackChunkList<WaterBlock>(data, unpackFlags);
}

bool WaterBlock::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'SIZE':

			data >> blockSize;
			return (true);
	}

	return (false);
}

bool WaterBlock::CalculateBoundingBox(Box3D *box) const
{
	box->min.Set(0.0F, 0.0F, 0.0F);
	box->max = GetBlockBoxSize();
	return (true);
}

void WaterBlock::Preprocess(void)
{
	delete[] waterGeometryTable;

	int32 geometryCount = blockSize.x * blockSize.y;
	waterGeometryTable = new WaterGeometry *[geometryCount];
	for (machine a = 0; a < geometryCount; a++)
	{
		waterGeometryTable[a] = nullptr;
	}

	Node *node = GetFirstSubnode();
	while (node)
	{
		if (node->GetNodeType() == kNodeGeometry)
		{
			Geometry *geometry = static_cast<Geometry *>(node);
			if (geometry->GetGeometryType() == kGeometryWater)
			{
				WaterGeometry *waterGeometry = static_cast<WaterGeometry *>(geometry);
				const Integer2D& geometryCoord = waterGeometry->GetObject()->GetGeometryCoord();
				waterGeometryTable[geometryCoord.y * blockSize.x + geometryCoord.x] = waterGeometry;
			}
		}

		node = node->Next();
	}

	Node::Preprocess();
}

void WaterBlock::Neutralize(void)
{
	delete[] waterGeometryTable;
	waterGeometryTable = nullptr;

	Node::Neutralize();
}

Vector2D WaterBlock::GetBlockBoxSize(void) const
{
	const WaterBlockObject *object = GetObject();
	int32 dimension = object->GetWaterFieldDimension();
	float scale = object->GetWaterFieldScale();
	return (Vector2D((float) (blockSize.x * dimension) * scale, (float) (blockSize.y * dimension) * scale));
}

WaterGeometry *WaterBlock::GetWaterGeometry(int32 u, int32 v) const
{
	if (((unsigned_int32) u < (unsigned_int32) blockSize.x) && ((unsigned_int32) v < (unsigned_int32) blockSize.y))
	{
		return (waterGeometryTable[v * blockSize.x + u]);
	}

	return (nullptr);
}

float WaterBlock::GetWaterElevation(const Point2D& position) const
{
	const WaterBlockObject *object = GetObject();
	int32 dimension = object->GetWaterFieldDimension();
	float inverseScale = 1.0F / object->GetWaterFieldScale();

	int32 i = (int32) (position.x * inverseScale + 0.5F);
	int32 j = (int32) (position.y * inverseScale + 0.5F);
	int32 u = i / dimension;
	int32 v = j / dimension;

	if (((unsigned_int32) u < (unsigned_int32) blockSize.x) && ((unsigned_int32) v < (unsigned_int32) blockSize.y))
	{
		const WaterGeometry *geometry = waterGeometryTable[v * blockSize.x + u];
		if (geometry)
		{
			unsigned_int32 parity = static_cast<const WaterController *>(GetController())->GetMoveParity();
			const float *elevation = geometry->GetWaterElevation()[parity];
			return (elevation[(j - v * dimension) * (dimension + 1) + (i - u * dimension)]);
		}
	}

	return (0.0F);
}

float WaterBlock::GetFilteredWaterElevation(const Point2D& position) const
{
	const WaterBlockObject *object = GetObject();
	int32 dimension = object->GetWaterFieldDimension();
	float inverseScale = 1.0F / object->GetWaterFieldScale();

	int32 i = (int32) (position.x * inverseScale + 0.5F);
	int32 j = (int32) (position.y * inverseScale + 0.5F);
	int32 u = i / dimension;
	int32 v = j / dimension;

	if (((unsigned_int32) u < (unsigned_int32) blockSize.x) && ((unsigned_int32) v < (unsigned_int32) blockSize.y))
	{
		const WaterGeometry *geometry = waterGeometryTable[v * blockSize.x + u];
		if (geometry)
		{
			unsigned_int32 parity = static_cast<const WaterController *>(GetController())->GetMoveParity();
			const float *elevation = geometry->GetWaterElevation()[parity];

			i -= u * dimension;
			j -= v * dimension;
			int32 row = dimension + 1;
			int32 k = j * row;

			float h1 = elevation[k + i];

			float h2 = elevation[k + MaxZero(i - 1)];
			float h3 = elevation[k + Min(i + 1, dimension)];
			float h4 = elevation[MaxZero(j - 1) * row + i];
			float h5 = elevation[Min(j + 1, dimension) * row + i];

			return (((h1 + h2) + (h3 + h4) + h5) * 0.2F);
		}
	}

	return (0.0F);
}


WaterGeometryObject::WaterGeometryObject() : GeometryObject(kGeometryWater)
{
}

WaterGeometryObject::WaterGeometryObject(const Integer2D& coord) : GeometryObject(kGeometryWater)
{
	geometryCoord = coord;

	SetGeometryFlags((GetGeometryFlags() | kGeometryMarkingInhibit) & ~kGeometryCastShadows);
	SetCollisionExclusionMask(kCollisionExcludeAll);
}

WaterGeometryObject::~WaterGeometryObject()
{
}

void WaterGeometryObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	GeometryObject::Pack(data, packFlags);

	data << ChunkHeader('GCRD', sizeof(Integer2D));
	data << geometryCoord;

	data << TerminatorChunk;
}

void WaterGeometryObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	GeometryObject::Unpack(data, unpackFlags);
	UnpackChunkList<WaterGeometryObject>(data, unpackFlags);
}

bool WaterGeometryObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'GCRD':

			data >> geometryCoord;
			return (true);
	}

	return (false);
}

void WaterGeometryObject::Build(Geometry *geometry)
{
	ArrayDescriptor		desc[3];

	const WaterBlockObject *blockObject = static_cast<WaterGeometry *>(geometry)->GetBlockNode()->GetObject();
	int32 dimension = blockObject->GetWaterFieldDimension();

	int32 vertexCount = (dimension + 1) * (dimension + 1);
	int32 triangleCount = dimension * dimension * 2;

	desc[0].identifier = kArrayPosition;
	desc[0].elementCount = vertexCount;
	desc[0].elementSize = sizeof(Point2D);
	desc[0].componentCount = 2;

	desc[1].identifier = kArrayNormal;
	desc[1].elementCount = vertexCount;
	desc[1].elementSize = sizeof(Vector4D);
	desc[1].componentCount = 4;

	desc[2].identifier = kArrayPrimitive;
	desc[2].elementCount = triangleCount;
	desc[2].elementSize = sizeof(Triangle);
	desc[2].componentCount = 1;

	SetGeometryLevelCount(1);
	Mesh *mesh = GetGeometryLevel(0);
	mesh->AllocateStorage(vertexCount, 3, desc);

	float scale = blockObject->GetWaterFieldScale();
	Point2D *restrict position = mesh->GetArray<Point2D>(kArrayPosition) - 1;

	for (machine j = 0; j <= dimension; j++)
	{
		float y = (float) (geometryCoord.y * dimension + j) * scale;
		for (machine i = 0; i <= dimension; i++)
		{
			float x = (float) (geometryCoord.x * dimension + i) * scale;
			(++position)->Set(x, y);
		}
	}

	Vector4D *restrict normal = mesh->GetArray<Vector4D>(kArrayNormal);
	for (machine a = 0; a < vertexCount; a++)
	{
		normal[a].Set(0.0F, 0.0F, 1.0F, 0.0F);
	}

	Triangle *restrict triangle = mesh->GetArray<Triangle>(kArrayPrimitive);

	for (machine j = 0; j < dimension; j++)
	{
		int32 k = j * (dimension + 1);
		for (machine i = 0; i < dimension; i++)
		{
			if (((i + j) & 1) == 0)
			{
				triangle[0].Set(k + i, k + i + 1, k + i + dimension + 1);
				triangle[1].Set(k + i + 1, k + i + dimension + 2, k + i + dimension + 1);
			}
			else
			{
				triangle[0].Set(k + i, k + i + 1, k + i + dimension + 2);
				triangle[1].Set(k + i, k + i + dimension + 2, k + i + dimension + 1);
			}

			triangle += 2;
		}
	}
}


HorizonWaterGeometryObject::HorizonWaterGeometryObject() : GeometryObject(kGeometryHorizonWater)
{
	SetGeometryFlags((GetGeometryFlags() | kGeometryMarkingInhibit) & ~kGeometryCastShadows);
	SetCollisionExclusionMask(kCollisionExcludeAll);
}

HorizonWaterGeometryObject::HorizonWaterGeometryObject(int32 direction) : GeometryObject(kGeometryHorizonWater)
{
	waterDirection = direction;

	SetGeometryFlags((GetGeometryFlags() | kGeometryMarkingInhibit) & ~kGeometryCastShadows);
	SetCollisionExclusionMask(kCollisionExcludeAll);
}

HorizonWaterGeometryObject::~HorizonWaterGeometryObject()
{
}

void HorizonWaterGeometryObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	GeometryObject::Pack(data, packFlags);

	data << ChunkHeader('DIRC', 4);
	data << waterDirection;

	data << TerminatorChunk;
}

void HorizonWaterGeometryObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	GeometryObject::Unpack(data, unpackFlags);
	UnpackChunkList<HorizonWaterGeometryObject>(data, unpackFlags);
}

bool HorizonWaterGeometryObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'DIRC':

			data >> waterDirection;
			return (true);
	}

	return (false);
}

void HorizonWaterGeometryObject::Build(Geometry *geometry)
{
	ArrayDescriptor		desc[5];

	const WaterBlock *block = static_cast<HorizonWaterGeometry *>(geometry)->GetBlockNode();
	const WaterBlockObject *blockObject = block->GetObject();
	int32 dimension = blockObject->GetWaterFieldDimension();

	const Integer2D& blockSize = block->GetBlockSize();
	int32 boundaryCount = ((waterDirection < kWaterDirectionNorth) ? blockSize.y : blockSize.x) * dimension + 1;

	int32 vertexCount = boundaryCount + 9;
	int32 triangleCount = boundaryCount + 7;

	desc[0].identifier = kArrayPosition;
	desc[0].elementCount = vertexCount;
	desc[0].elementSize = sizeof(Point2D);
	desc[0].componentCount = 2;

	desc[1].identifier = kArrayPosition1;
	desc[1].elementCount = vertexCount;
	desc[1].elementSize = sizeof(Point3D);
	desc[1].componentCount = 3;

	desc[2].identifier = kArrayNormal;
	desc[2].elementCount = vertexCount;
	desc[2].elementSize = sizeof(Vector3D);
	desc[2].componentCount = 3;

	desc[3].identifier = kArrayTangent;
	desc[3].elementCount = vertexCount;
	desc[3].elementSize = sizeof(Vector3D);
	desc[3].componentCount = 3;

	desc[4].identifier = kArrayPrimitive;
	desc[4].elementCount = triangleCount;
	desc[4].elementSize = sizeof(Triangle);
	desc[4].componentCount = 1;

	SetGeometryLevelCount(1);
	Mesh *mesh = GetGeometryLevel(0);
	mesh->AllocateStorage(vertexCount, 5, desc);

	float scale = blockObject->GetWaterFieldScale();
	float horizon = blockObject->GetWaterHorizonDistance();
	Point2D center((float) (blockSize.x * dimension) * scale * 0.5F, (float) (blockSize.y * dimension) * scale * 0.5F);

	Point2D *restrict position = mesh->GetArray<Point2D>(kArrayPosition) - 1;
	const ConstVector2D *trig = Math::GetTrigTable();

	if (waterDirection == kWaterDirectionEast)
	{
		float x = (float) (blockSize.x * dimension) * scale;
		for (machine j = 0; j < boundaryCount; j++)
		{
			(++position)->Set(x, (float) j * scale);
		}

		for (machine a = 0; a < 9; a++)
		{
			const Vector2D& cs = trig[(a * 8 - 32) & 255];
			(++position)->Set(center.x + horizon * cs.x, center.y + horizon * cs.y);
		}
	}
	else if (waterDirection == kWaterDirectionWest)
	{
		for (machine j = 0; j < boundaryCount; j++)
		{
			(++position)->Set(0.0F, (float) (boundaryCount - 1 - j) * scale);
		}

		for (machine a = 0; a < 9; a++)
		{
			const Vector2D& cs = trig[a * 8 + 96];
			(++position)->Set(center.x + horizon * cs.x, center.y + horizon * cs.y);
		}
	}
	else if (waterDirection == kWaterDirectionNorth)
	{
		float y = (float) (blockSize.y * dimension) * scale;
		for (machine i = 0; i < boundaryCount; i++)
		{
			(++position)->Set((float) (boundaryCount - 1 - i) * scale, y);
		}

		for (machine a = 0; a < 9; a++)
		{
			const Vector2D& cs = trig[a * 8 + 32];
			(++position)->Set(center.x + horizon * cs.x, center.y + horizon * cs.y);
		}
	}
	else
	{
		for (machine i = 0; i < boundaryCount; i++)
		{
			(++position)->Set((float) i * scale, 0.0F);
		}

		for (machine a = 0; a < 9; a++)
		{
			const Vector2D& cs = trig[a * 8 + 160];
			(++position)->Set(center.x + horizon * cs.x, center.y + horizon * cs.y);
		}
	}

	float h = blockObject->GetLandElevationRange().min;

	Point3D *restrict elevation = mesh->GetArray<Point3D>(kArrayPosition1);
	Vector3D *restrict normal = mesh->GetArray<Vector3D>(kArrayNormal);
	Vector3D *restrict tangent = mesh->GetArray<Vector3D>(kArrayTangent);
	for (machine a = 0; a < vertexCount; a++)
	{
		elevation[a].Set(0.0F, 0.0F, h);
		normal[a].Set(0.0F, 0.0F, 1.0F);
		tangent[a].Set(1.0F, 0.0F, 0.0F);
	}

	Triangle *restrict triangle = mesh->GetArray<Triangle>(kArrayPrimitive);
	int32 k = boundaryCount;
	int32 n = (k - 1) >> 3;
	int32 i = 0;

	for (machine a = 0; a < 4; a++)
	{
		for (machine b = 0; b < n; b++)
		{
			triangle->Set(k, i + 1, i);
			triangle++;
			i++;
		}

		triangle->Set(k, k + 1, i);
		triangle++;
		k++;
	}

	for (machine a = 4; a < 8; a++)
	{
		triangle->Set(k, k + 1, i);
		triangle++;
		k++;

		for (machine b = 0; b < n; b++)
		{
			triangle->Set(k, i + 1, i);
			triangle++;
			i++;
		}
	}
}


WaterGeometry::WaterGeometry() :
		Geometry(kGeometryWater),
		waterMoveJob(&JobMoveWater, this),
		waterUpdateJob(&JobUpdateWater, &FinalizeWaterUpdate, this),
		dynamicVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
{
	blockNode = nullptr;

	waterState = kWaterAsleep;
	waterMultiplier = 0.0F;

	for (machine a = 0; a < 4; a++)
	{
		diagonalMultiplier[a] = 0.0F;
	}

	fieldStorage = nullptr;
	waterUpdateFlag = false;
	waterInvisibleTime = 0;
}

WaterGeometry::WaterGeometry(WaterBlock *block, const Integer2D& coord) :
		Geometry(kGeometryWater),
		waterMoveJob(&JobMoveWater, this),
		waterUpdateJob(&JobUpdateWater, &FinalizeWaterUpdate, this),
		dynamicVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
{
	blockNode = block;
	SetPerspectiveExclusionMask(kPerspectiveReflection | kPerspectiveRefraction);

	waterState = kWaterAsleep;
	waterMultiplier = 0.0F;

	for (machine a = 0; a < 4; a++)
	{
		diagonalMultiplier[a] = 0.0F;
	}

	fieldStorage = nullptr;
	waterUpdateFlag = false;
	waterInvisibleTime = 0;

	SetNewObject(new WaterGeometryObject(coord));
}

WaterGeometry::WaterGeometry(const WaterGeometry& waterGeometry) :
		Geometry(waterGeometry),
		waterMoveJob(&JobMoveWater, this),
		waterUpdateJob(&JobUpdateWater, &FinalizeWaterUpdate, this),
		dynamicVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
{
	blockNode = nullptr;

	waterState = kWaterAsleep;
	waterMultiplier = 0.0F;

	for (machine a = 0; a < 4; a++)
	{
		diagonalMultiplier[a] = 0.0F;
	}

	fieldStorage = nullptr;
	waterUpdateFlag = false;
	waterInvisibleTime = 0;
}

WaterGeometry::~WaterGeometry()
{
	delete[] fieldStorage;
}

Node *WaterGeometry::Replicate(void) const
{
	return (new WaterGeometry(*this));
}

void WaterGeometry::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Geometry::Pack(data, packFlags);

	if (!GetManipulator())
	{
		data << ChunkHeader('STAT', 4);
		data << waterState;

		if (waterState == kWaterDecaying)
		{
			data << ChunkHeader('DCAY', 4);
			data << decayStepIndex;

			data << ChunkHeader('MULT', 4);
			data << waterMultiplier;
		}
	}

	if (fieldStorage)
	{
		data << ChunkHeader('FELD', 4 + waterVertexCount * 8);
		data << waterVertexCount;

		data.WriteArray(waterVertexCount, waterElevation[0]);
		data.WriteArray(waterVertexCount, waterElevation[1]);
	}

	data << TerminatorChunk;
}

void WaterGeometry::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Geometry::Unpack(data, unpackFlags);
	UnpackChunkList<WaterGeometry>(data, unpackFlags);
}

bool WaterGeometry::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'STAT':

			data >> waterState;
			return (true);

		case 'DCAY':

			data >> decayStepIndex;
			return (true);

		case 'MULT':

			data >> waterMultiplier;
			return (true);

		case 'FELD':

			data >> waterVertexCount;
			AllocateFieldStorage();

			data.ReadArray(waterVertexCount, waterElevation[0]);
			data.ReadArray(waterVertexCount, waterElevation[1]);

			waterUpdateFlag = true;
			return (true);
	}

	return (false);
}

void *WaterGeometry::BeginSettingsUnpack(void)
{
	if (fieldStorage)
	{
		delete[] fieldStorage;
		fieldStorage = nullptr;
	}

	return (Geometry::BeginSettingsUnpack());
}

void WaterGeometry::AllocateFieldStorage(void)
{
	fieldStorage = new char[waterVertexCount * (8 + sizeof(Vector4D))];

	int32 vertexCount = waterVertexCount;
	waterElevation[0] = reinterpret_cast<float *>(fieldStorage);
	waterElevation[1] = waterElevation[0] + vertexCount;

	Vector4D *normal = reinterpret_cast<Vector4D *>(waterElevation[1] + vertexCount);
	waterNormal = normal;

	for (machine a = 0; a < vertexCount; a++)
	{
		normal[a].Set(0.0F, 0.0F, 1.0F, 0.0F);
	}
}

void WaterGeometry::LoadWaterElevation(const float *const (& elevation)[2])
{
	if (!fieldStorage)
	{
		AllocateFieldStorage();
	}

	MemoryMgr::CopyMemory(elevation[0], waterElevation[0], waterVertexCount * 4);
	MemoryMgr::CopyMemory(elevation[1], waterElevation[1], waterVertexCount * 4);

	waterUpdateFlag = true;
}

void WaterGeometry::ClearWaterElevation(void)
{
	if (fieldStorage)
	{
		delete[] fieldStorage;
		fieldStorage = nullptr;

		waterUpdateFlag = false;
		InitVertexBuffer(&dynamicVertexBuffer);
	}
}

bool WaterGeometry::CalculateBoundingBox(Box3D *box) const
{
	const WaterBlock *block = blockNode;
	if (block)
	{
		const WaterBlockObject *blockObject = block->GetObject();
		int32 dimension = blockObject->GetWaterFieldDimension();
		float scale = blockObject->GetWaterFieldScale();

		const WaterGeometryObject *object = GetObject();
		const Integer2D& geometryCoord = object->GetGeometryCoord();

		float h = scale * 2.0F;
		float size = (float) dimension * scale;
		float x = (float) geometryCoord.x * size;
		float y = (float) geometryCoord.y * size;

		box->min.Set(x, y, -h);
		box->max.Set(x + size, y + size, h);
		return (true);
	}

	return (false);
}

bool WaterGeometry::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	const WaterBlock *block = blockNode;
	if (block)
	{
		const WaterBlockObject *blockObject = block->GetObject();
		int32 dimension = blockObject->GetWaterFieldDimension();
		float scale = blockObject->GetWaterFieldScale();

		const WaterGeometryObject *object = GetObject();
		const Integer2D& geometryCoord = object->GetGeometryCoord();

		float size = (float) dimension * scale;
		float x = (float) geometryCoord.x * size;
		float y = (float) geometryCoord.y * size;
		float sx = size * 0.5F;
		float sy = size * 0.5F;

		sphere->SetCenter(x + sx, y + sy, 0.0F);
		sphere->SetRadius(Sqrt(sx * sx + sy * sy));
		return (true);
	}

	return (false);
}

void WaterGeometry::Preinitialize(void)
{
	Node *node = GetSuperNode();
	if ((node) && (node->GetNodeType() == kNodeWaterBlock))
	{
		blockNode = static_cast<WaterBlock *>(node);
	}

	Geometry::Preinitialize();
}

void WaterGeometry::Preprocess(void)
{
	const WaterBlock *block = blockNode;
	if (block)
	{
		waterController = static_cast<WaterController *>(block->GetController());
		if (waterController)
		{
			const WaterBlockObject *blockObject = block->GetObject();
			int32 dimension = blockObject->GetWaterFieldDimension();
			waterFieldWidth = dimension + 1;
			waterFieldHeight = dimension + 1;
			waterVertexCount = waterFieldWidth * waterFieldHeight;

			SetVertexBufferArrayFlags(1 << kArrayNormal);
			SetVertexBuffer(kVertexBufferAttributeArray1, &dynamicVertexBuffer, sizeof(Vector4D));
			SetVertexAttributeArray(kArrayNormal, 0, 4);

			if (!dynamicVertexBuffer.Active())
			{
				dynamicVertexBuffer.Establish(waterVertexCount * sizeof(Vector4D));
				InitVertexBuffer(&dynamicVertexBuffer);
			}

			if ((!GetManipulator()) && (!fieldStorage))
			{
				AllocateFieldStorage();

				for (machine a = 0; a < waterVertexCount; a++)
				{
					waterElevation[0][a] = 0.0F;
					waterElevation[1][a] = 0.0F;
				}
			}

			const Integer2D& geometryCoord = GetObject()->GetGeometryCoord();
			adjacentWater[kWaterDirectionEast] = block->GetWaterGeometry(geometryCoord.x + 1, geometryCoord.y);
			adjacentWater[kWaterDirectionWest] = block->GetWaterGeometry(geometryCoord.x - 1, geometryCoord.y);
			adjacentWater[kWaterDirectionNorth] = block->GetWaterGeometry(geometryCoord.x, geometryCoord.y + 1);
			adjacentWater[kWaterDirectionSouth] = block->GetWaterGeometry(geometryCoord.x, geometryCoord.y - 1);
			diagonalWater[kWaterDirectionNortheast] = block->GetWaterGeometry(geometryCoord.x + 1, geometryCoord.y + 1);
			diagonalWater[kWaterDirectionNorthwest] = block->GetWaterGeometry(geometryCoord.x - 1, geometryCoord.y + 1);
			diagonalWater[kWaterDirectionSoutheast] = block->GetWaterGeometry(geometryCoord.x + 1, geometryCoord.y - 1);
			diagonalWater[kWaterDirectionSouthwest] = block->GetWaterGeometry(geometryCoord.x - 1, geometryCoord.y - 1);

			int32 deadCount = 0;
			if (adjacentWater[kWaterDirectionSouth])
			{
				if ((!diagonalWater[kWaterDirectionSouthwest]) && (adjacentWater[kWaterDirectionWest]))
				{
					deadCornerPosition[deadCount++] = 0;
				}

				if ((!diagonalWater[kWaterDirectionSoutheast]) && (adjacentWater[kWaterDirectionEast]))
				{
					deadCornerPosition[deadCount++] = dimension;
				}
			}

			if (adjacentWater[kWaterDirectionNorth])
			{
				if ((!diagonalWater[kWaterDirectionNorthwest]) && (adjacentWater[kWaterDirectionWest]))
				{
					deadCornerPosition[deadCount++] = dimension * waterFieldWidth;
				}

				if ((!diagonalWater[kWaterDirectionNortheast]) && (adjacentWater[kWaterDirectionEast]))
				{
					deadCornerPosition[deadCount++] = dimension * waterFieldWidth + dimension;
				}
			}

			deadCornerCount = deadCount;

			unsigned_int32 state = waterState;
			if (state == kWaterAwake)
			{
				SetWaterMultiplier(1.0F);
			}
			else if (state == kWaterDecaying)
			{
				SetWaterMultiplier(waterMultiplier);
			}
		}
	}

	Geometry::Preprocess();
	SetShaderFlags(GetShaderFlags() | (kShaderNormalizeBasisVectors | kShaderGenerateTexcoord | kShaderGenerateTangent | kShaderWaterElevation));

	const MaterialObject *materialObject = GetMaterialObject(0);
	if (materialObject)
	{
		SetTexcoordParameterPointer(&materialObject->GetTexcoordGeneration());
	}
	else
	{
		SetNullTexcoordParameterPointer();
	}
}

void WaterGeometry::InitVertexBuffer(VertexBuffer *vertexBuffer)
{
	volatile Vector4D *restrict normal = static_cast<volatile Vector4D *>(vertexBuffer->BeginUpdateSync());

	int32 vertexCount = waterVertexCount;
	for (machine a = 0; a < vertexCount; a++)
	{
		normal[a].Set(0.0F, 0.0F, 1.0F, 0.0F);
	}

	vertexBuffer->EndUpdateSync();
}

void WaterGeometry::SetWaterMultiplier(float multiplier)
{
	waterMultiplier = multiplier;

	for (machine a = 0; a < 4; a++)
	{
		WaterGeometry *diagonal = diagonalWater[a];
		if (diagonal)
		{
			diagonal->diagonalMultiplier[a ^ 3] = multiplier;
		}
	}
}

void WaterGeometry::JobMoveWater(Job *job, void *cookie)
{
	WaterGeometry *geometry = static_cast<WaterGeometry *>(cookie);

	const WaterController *controller = geometry->waterController;
	float k1 = controller->GetWaterConstant(0);
	float k2 = controller->GetWaterConstant(1);
	float k3 = controller->GetWaterConstant(2);

	unsigned_int32 parity = controller->GetMoveParity();
	const float *elevation1 = geometry->waterElevation[parity];
	float *elevation2 = geometry->waterElevation[parity ^ 1];

	int32 width = geometry->waterFieldWidth;
	int32 height = geometry->waterFieldHeight;
	int32 wm1 = width - 1;
	int32 hm1 = height - 1;

	const WaterGeometry *eastWater = geometry->adjacentWater[kWaterDirectionEast];
	const WaterGeometry *westWater = geometry->adjacentWater[kWaterDirectionWest];
	const WaterGeometry *northWater = geometry->adjacentWater[kWaterDirectionNorth];
	const WaterGeometry *southWater = geometry->adjacentWater[kWaterDirectionSouth];

	float multiplier = geometry->waterMultiplier;

	if (eastWater)
	{
		const float *eastElevation = eastWater->waterElevation[parity] + 1;
		float eastMultiplier = eastWater->waterMultiplier;

		if (westWater)
		{
			const float *westElevation = westWater->waterElevation[parity] + (width - 2);
			float westMultiplier = westWater->waterMultiplier;

			if (southWater)
			{
				const float *southElevation = southWater->waterElevation[parity] + (height - 2) * width;
				float southMultiplier = multiplier * southWater->waterMultiplier;

				const float *z1 = elevation1;
				float *restrict z2 = elevation2;

				float m = southMultiplier * westMultiplier * geometry->diagonalMultiplier[kWaterDirectionSouthwest];
				z2[0] = (z1[0] * k1 + z2[0] * k2 + ((westElevation[0] + z1[1]) + (southElevation[0] + z1[width])) * k3) * m;
				z1++;
				z2++;

				for (machine i = 1; i < wm1; i++)
				{
					z2[0] = (z1[0] * k1 + z2[0] * k2 + ((z1[-1] + z1[1]) + (southElevation[i] + z1[width])) * k3) * southMultiplier;
					z1++;
					z2++;
				}

				m = southMultiplier * eastMultiplier * geometry->diagonalMultiplier[kWaterDirectionSoutheast];
				z2[0] = (z1[0] * k1 + z2[0] * k2 + ((z1[-1] + eastElevation[0]) + (southElevation[wm1] + z1[width])) * k3) * m;
			}

			eastElevation += width;
			westElevation += width;

			for (machine j = 1; j < hm1; j++)
			{
				int32 k = j * width;
				const float *z1 = &elevation1[k];
				float *restrict z2 = &elevation2[k];

				float m = multiplier * westMultiplier;
				z2[0] = (z1[0] * k1 + z2[0] * k2 + ((westElevation[0] + z1[1]) + (z1[-width] + z1[width])) * k3) * m;
				z1++;
				z2++;

				for (machine i = 1; i < wm1; i++)
				{
					z2[0] = z1[0] * k1 + z2[0] * k2 + ((z1[-1] + z1[1]) + (z1[-width] + z1[width])) * k3;
					z1++;
					z2++;
				}

				m = multiplier * eastMultiplier;
				z2[0] = (z1[0] * k1 + z2[0] * k2 + ((z1[-1] + eastElevation[0]) + (z1[-width] + z1[width])) * k3) * m;

				eastElevation += width;
				westElevation += width;
			}

			if (northWater)
			{
				const float *northElevation = northWater->waterElevation[parity] + width;
				float northMultiplier = multiplier * northWater->waterMultiplier;

				int32 k = hm1 * width;
				const float *z1 = &elevation1[k];
				float *restrict z2 = &elevation2[k];

				float m = northMultiplier * westMultiplier * geometry->diagonalMultiplier[kWaterDirectionNorthwest];
				z2[0] = (z1[0] * k1 + z2[0] * k2 + ((westElevation[0] + z1[1]) + (z1[-width] + northElevation[0])) * k3) * m;
				z1++;
				z2++;

				for (machine i = 1; i < wm1; i++)
				{
					z2[0] = (z1[0] * k1 + z2[0] * k2 + ((z1[-1] + z1[1]) + (z1[-width] + northElevation[i])) * k3) * northMultiplier;
					z1++;
					z2++;
				}

				m = northMultiplier * eastMultiplier * geometry->diagonalMultiplier[kWaterDirectionNortheast];
				z2[0] = (z1[0] * k1 + z2[0] * k2 + ((z1[-1] + eastElevation[0]) + (z1[-width] + northElevation[wm1])) * k3) * m;
			}
		}
		else
		{
			if (southWater)
			{
				const float *southElevation = southWater->waterElevation[parity] + (height - 2) * width;
				float southMultiplier = multiplier * southWater->waterMultiplier;

				const float *z1 = &elevation1[1];
				float *restrict z2 = &elevation2[1];

				for (machine i = 1; i < wm1; i++)
				{
					z2[0] = (z1[0] * k1 + z2[0] * k2 + ((z1[-1] + z1[1]) + (southElevation[i] + z1[width])) * k3) * southMultiplier;
					z1++;
					z2++;
				}

				float m = southMultiplier * eastMultiplier * geometry->diagonalMultiplier[kWaterDirectionSoutheast];
				z2[0] = (z1[0] * k1 + z2[0] * k2 + ((z1[-1] + eastElevation[0]) + (southElevation[wm1] + z1[width])) * k3) * m;
			}

			eastElevation += width;

			for (machine j = 1; j < hm1; j++)
			{
				int32 k = j * width + 1;
				const float *z1 = &elevation1[k];
				float *restrict z2 = &elevation2[k];

				for (machine i = 1; i < wm1; i++)
				{
					z2[0] = z1[0] * k1 + z2[0] * k2 + ((z1[-1] + z1[1]) + (z1[-width] + z1[width])) * k3;
					z1++;
					z2++;
				}

				float m = multiplier * eastMultiplier;
				z2[0] = (z1[0] * k1 + z2[0] * k2 + ((z1[-1] + eastElevation[0]) + (z1[-width] + z1[width])) * k3) * m;

				eastElevation += width;
			}

			if (northWater)
			{
				const float *northElevation = northWater->waterElevation[parity] + width;
				float northMultiplier = multiplier * northWater->waterMultiplier;

				int32 k = hm1 * width + 1;
				const float *z1 = &elevation1[k];
				float *restrict z2 = &elevation2[k];

				for (machine i = 1; i < wm1; i++)
				{
					z2[0] = (z1[0] * k1 + z2[0] * k2 + ((z1[-1] + z1[1]) + (z1[-width] + northElevation[i])) * k3) * northMultiplier;
					z1++;
					z2++;
				}

				float m = northMultiplier * eastMultiplier * geometry->diagonalMultiplier[kWaterDirectionNortheast];
				z2[0] = (z1[0] * k1 + z2[0] * k2 + ((z1[-1] + eastElevation[0]) + (z1[-width] + northElevation[wm1])) * k3) * m;
			}
		}
	}
	else if (westWater)
	{
		const float *westElevation = westWater->waterElevation[parity] + (width - 2);
		float westMultiplier = westWater->waterMultiplier;

		if (southWater)
		{
			const float *southElevation = southWater->waterElevation[parity] + (height - 2) * width;
			float southMultiplier = multiplier * southWater->waterMultiplier;

			const float *z1 = elevation1;
			float *restrict z2 = elevation2;

			float m = southMultiplier * westMultiplier * geometry->diagonalMultiplier[kWaterDirectionSouthwest];
			z2[0] = (z1[0] * k1 + z2[0] * k2 + ((westElevation[0] + z1[1]) + (southElevation[0] + z1[width])) * k3) * m;
			z1++;
			z2++;

			for (machine i = 1; i < wm1; i++)
			{
				z2[0] = (z1[0] * k1 + z2[0] * k2 + ((z1[-1] + z1[1]) + (southElevation[i] + z1[width])) * k3) * southMultiplier;
				z1++;
				z2++;
			}
		}

		westElevation += width;

		for (machine j = 1; j < hm1; j++)
		{
			int32 k = j * width;
			const float *z1 = &elevation1[k];
			float *restrict z2 = &elevation2[k];

			float m = multiplier * westMultiplier;
			z2[0] = (z1[0] * k1 + z2[0] * k2 + ((westElevation[0] + z1[1]) + (z1[-width] + z1[width])) * k3) * m;
			z1++;
			z2++;

			for (machine i = 1; i < wm1; i++)
			{
				z2[0] = z1[0] * k1 + z2[0] * k2 + ((z1[-1] + z1[1]) + (z1[-width] + z1[width])) * k3;
				z1++;
				z2++;
			}

			westElevation += width;
		}

		if (northWater)
		{
			const float *northElevation = northWater->waterElevation[parity] + width;
			float northMultiplier = multiplier * northWater->waterMultiplier;

			int32 k = hm1 * width;
			const float *z1 = &elevation1[k];
			float *restrict z2 = &elevation2[k];

			float m = northMultiplier * westMultiplier * geometry->diagonalMultiplier[kWaterDirectionNorthwest];
			z2[0] = (z1[0] * k1 + z2[0] * k2 + ((westElevation[0] + z1[1]) + (z1[-width] + northElevation[0])) * k3) * m;
			z1++;
			z2++;

			for (machine i = 1; i < wm1; i++)
			{
				z2[0] = (z1[0] * k1 + z2[0] * k2 + ((z1[-1] + z1[1]) + (z1[-width] + northElevation[i])) * k3) * northMultiplier;
				z1++;
				z2++;
			}
		}
	}
	else
	{
		if (southWater)
		{
			const float *southElevation = southWater->waterElevation[parity] + (height - 2) * width;
			float southMultiplier = multiplier * southWater->waterMultiplier;

			const float *z1 = &elevation1[1];
			float *restrict z2 = &elevation2[1];

			for (machine i = 1; i < wm1; i++)
			{
				z2[0] = (z1[0] * k1 + z2[0] * k2 + ((z1[-1] + z1[1]) + (southElevation[i] + z1[width])) * k3) * southMultiplier;
				z1++;
				z2++;
			}
		}

		for (machine j = 1; j < hm1; j++)
		{
			int32 k = j * width + 1;
			const float *z1 = &elevation1[k];
			float *restrict z2 = &elevation2[k];

			for (machine i = 1; i < wm1; i++)
			{
				z2[0] = z1[0] * k1 + z2[0] * k2 + ((z1[-1] + z1[1]) + (z1[-width] + z1[width])) * k3;
				z1++;
				z2++;
			}
		}

		if (northWater)
		{
			const float *northElevation = northWater->waterElevation[parity] + width;
			float northMultiplier = multiplier * northWater->waterMultiplier;

			int32 k = hm1 * width + 1;
			const float *z1 = &elevation1[k];
			float *restrict z2 = &elevation2[k];

			for (machine i = 1; i < wm1; i++)
			{
				z2[0] = (z1[0] * k1 + z2[0] * k2 + ((z1[-1] + z1[1]) + (z1[-width] + northElevation[i])) * k3) * northMultiplier;
				z1++;
				z2++;
			}
		}
	}

	int32 count = geometry->deadCornerCount;
	for (machine a = 0; a < count; a++)
	{
		elevation2[geometry->deadCornerPosition[a]] = 0.0F;
	}
}

void WaterGeometry::JobUpdateWater(Job *job, void *cookie)
{
	WaterGeometry *geometry = static_cast<WaterGeometry *>(cookie);
	float scale = geometry->blockNode->GetObject()->GetWaterFieldScale() * 2.0F;
	Vector4D *restrict normal = geometry->waterNormal;

	unsigned_int32 parity = geometry->waterController->GetMoveParity();
	const float *elevation = geometry->waterElevation[parity];

	int32 width = geometry->waterFieldWidth;
	int32 height = geometry->waterFieldHeight;
	int32 wm1 = width - 1;
	int32 hm1 = height - 1;

	const WaterGeometry *eastWater = geometry->adjacentWater[kWaterDirectionEast];
	const WaterGeometry *westWater = geometry->adjacentWater[kWaterDirectionWest];
	const WaterGeometry *northWater = geometry->adjacentWater[kWaterDirectionNorth];
	const WaterGeometry *southWater = geometry->adjacentWater[kWaterDirectionSouth];

	if (eastWater)
	{
		const float *eastElevation = eastWater->waterElevation[parity] + 1;

		if (westWater)
		{
			const float *westElevation = westWater->waterElevation[parity] + (width - 2);

			if (southWater)
			{
				const float *southElevation = southWater->waterElevation[parity] + (height - 2) * width;

				const float *z = elevation;
				Vector4D *restrict nrml = normal;

				float dx = z[1] - westElevation[0];
				float dy = z[width] - southElevation[0];
				nrml->Set(-dx, -dy, scale, z[0]);

				z++;
				nrml++;

				for (machine i = 1; i < wm1; i++)
				{
					dx = z[1] - z[-1];
					dy = z[width] - southElevation[i];
					nrml->Set(-dx, -dy, scale, z[0]);

					z++;
					nrml++;
				}

				dx = eastElevation[0] - z[-1];
				dy = z[width] - southElevation[wm1];
				nrml->Set(-dx, -dy, scale, z[0]);
			}

			eastElevation += width;
			westElevation += width;

			for (machine j = 1; j < hm1; j++)
			{
				int32 k = j * width;
				const float *z = &elevation[k];
				Vector4D *restrict nrml = &normal[k];

				float dx = z[1] - westElevation[0];
				float dy = z[width] - z[-width];
				nrml->Set(-dx, -dy, scale, z[0]);

				z++;
				nrml++;

				for (machine i = 1; i < wm1; i++)
				{
					dx = z[1] - z[-1];
					dy = z[width] - z[-width];
					nrml->Set(-dx, -dy, scale, z[0]);

					z++;
					nrml++;
				}

				dx = eastElevation[0] - z[-1];
				dy = z[width] - z[-width];
				nrml->Set(-dx, -dy, scale, z[0]);

				eastElevation += width;
				westElevation += width;
			}

			if (northWater)
			{
				const float *northElevation = northWater->waterElevation[parity] + width;

				int32 k = hm1 * width;
				const float *z = &elevation[k];
				Vector4D *restrict nrml = &normal[k];

				float dx = z[1] - westElevation[0];
				float dy = northElevation[0] - z[-width];
				nrml->Set(-dx, -dy, scale, z[0]);

				z++;
				nrml++;

				for (machine i = 1; i < wm1; i++)
				{
					dx = z[1] - z[-1];
					dy = northElevation[i] - z[-width];
					nrml->Set(-dx, -dy, scale, z[0]);

					z++;
					nrml++;
				}

				dx = eastElevation[0] - z[-1];
				dy = northElevation[wm1] - z[-width];
				nrml->Set(-dx, -dy, scale, z[0]);
			}
		}
		else
		{
			if (southWater)
			{
				const float *southElevation = southWater->waterElevation[parity] + (height - 2) * width;

				const float *z = &elevation[1];
				Vector4D *restrict nrml = &normal[1];

				for (machine i = 1; i < wm1; i++)
				{
					float dx = z[1] - z[-1];
					float dy = z[width] - southElevation[i];
					nrml->Set(-dx, -dy, scale, z[0]);

					z++;
					nrml++;
				}

				float dx = eastElevation[0] - z[-1];
				float dy = z[width] - southElevation[wm1];
				nrml->Set(-dx, -dy, scale, z[0]);
			}

			eastElevation += width;

			for (machine j = 1; j < hm1; j++)
			{
				int32 k = j * width + 1;
				const float *z = &elevation[k];
				Vector4D *restrict nrml = &normal[k];

				for (machine i = 1; i < wm1; i++)
				{
					float dx = z[1] - z[-1];
					float dy = z[width] - z[-width];
					nrml->Set(-dx, -dy, scale, z[0]);

					z++;
					nrml++;
				}

				float dx = eastElevation[0] - z[-1];
				float dy = z[width] - z[-width];
				nrml->Set(-dx, -dy, scale, z[0]);

				eastElevation += width;
			}

			if (northWater)
			{
				const float *northElevation = northWater->waterElevation[parity] + width;

				int32 k = hm1 * width + 1;
				const float *z = &elevation[k];
				Vector4D *restrict nrml = &normal[k];

				for (machine i = 1; i < wm1; i++)
				{
					float dx = z[1] - z[-1];
					float dy = northElevation[i] - z[-width];
					nrml->Set(-dx, -dy, scale, z[0]);

					z++;
					nrml++;
				}

				float dx = eastElevation[0] - z[-1];
				float dy = northElevation[wm1] - z[-width];
				nrml->Set(-dx, -dy, scale, z[0]);
			}
		}
	}
	else if (westWater)
	{
		const float *westElevation = westWater->waterElevation[parity] + (width - 2);

		if (southWater)
		{
			const float *southElevation = southWater->waterElevation[parity] + (height - 2) * width;

			const float *z = elevation;
			Vector4D *restrict nrml = normal;

			float dx = z[1] - westElevation[0];
			float dy = z[width] - southElevation[0];
			nrml->Set(-dx, -dy, scale, z[0]);

			z++;
			nrml++;

			for (machine i = 1; i < wm1; i++)
			{
				dx = z[1] - z[-1];
				dy = z[width] - southElevation[i];
				nrml->Set(-dx, -dy, scale, z[0]);

				z++;
				nrml++;
			}
		}

		westElevation += width;

		for (machine j = 1; j < hm1; j++)
		{
			int32 k = j * width;
			const float *z = &elevation[k];
			Vector4D *restrict nrml = &normal[k];

			float dx = z[1] - westElevation[0];
			float dy = z[width] - z[-width];
			nrml->Set(-dx, -dy, scale, z[0]);

			z++;
			nrml++;

			for (machine i = 1; i < wm1; i++)
			{
				dx = z[1] - z[-1];
				dy = z[width] - z[-width];
				nrml->Set(-dx, -dy, scale, z[0]);

				z++;
				nrml++;
			}

			westElevation += width;
		}

		if (northWater)
		{
			const float *northElevation = northWater->waterElevation[parity] + width;

			int32 k = hm1 * width;
			const float *z = &elevation[k];
			Vector4D *restrict nrml = &normal[k];

			float dx = z[1] - westElevation[0];
			float dy = northElevation[0] - z[-width];
			nrml->Set(-dx, -dy, scale, z[0]);

			z++;
			nrml++;

			for (machine i = 1; i < wm1; i++)
			{
				dx = z[1] - z[-1];
				dy = northElevation[i] - z[-width];
				nrml->Set(-dx, -dy, scale, z[0]);

				z++;
				nrml++;
			}
		}
	}
	else
	{
		if (southWater)
		{
			const float *southElevation = southWater->waterElevation[parity] + (height - 2) * width;

			const float *z = &elevation[1];
			Vector4D *restrict nrml = &normal[1];

			for (machine i = 1; i < wm1; i++)
			{
				float dx = z[1] - z[-1];
				float dy = z[width] - southElevation[i];
				nrml->Set(-dx, -dy, scale, z[0]);

				z++;
				nrml++;
			}
		}

		for (machine j = 1; j < hm1; j++)
		{
			int32 k = j * width + 1;
			const float *z = &elevation[k];
			Vector4D *restrict nrml = &normal[k];

			for (machine i = 1; i < wm1; i++)
			{
				float dx = z[1] - z[-1];
				float dy = z[width] - z[-width];
				nrml->Set(-dx, -dy, scale, z[0]);

				z++;
				nrml++;
			}
		}

		if (northWater)
		{
			const float *northElevation = northWater->waterElevation[parity] + width;

			int32 k = hm1 * width + 1;
			const float *z = &elevation[k];
			Vector4D *restrict nrml = &normal[k];

			for (machine i = 1; i < wm1; i++)
			{
				float dx = z[1] - z[-1];
				float dy = northElevation[i] - z[-width];
				nrml->Set(-dx, -dy, scale, z[0]);

				z++;
				nrml++;
			}
		}
	}
}

void WaterGeometry::FinalizeWaterUpdate(Job *job, void *cookie)
{
	WaterGeometry *geometry = static_cast<WaterGeometry *>(cookie);
	geometry->dynamicVertexBuffer.UpdateBuffer(0, geometry->waterVertexCount * sizeof(Vector4D), geometry->waterNormal);
}

void WaterGeometry::UpdateWater(float squaredDistance)
{
	float d = waterController->GetMinAsleepDistance();
	if (squaredDistance < d * d)
	{
		if (waterState != kWaterAwake)
		{
			waterState = kWaterAwake;
			waterController->SetAwakeWaterState(this);
		}
	}
	else
	{
		if (waterState == kWaterAwake)
		{
			waterState = kWaterDecaying;
			waterController->SetDecayingWaterState(this);
		}
	}

	waterInvisibleTime = 0;

	if (waterUpdateFlag)
	{
		waterUpdateFlag = false;

		World *world = GetWorld();
		world->SubmitWorldJob(&waterUpdateJob);

		#if C4STATS

			world->IncrementWorldCounter(kWorldCounterWaterUpdate);

		#endif
	}
}

void WaterGeometry::UpdateInvisibleWater(float squaredDistance)
{
	float d = waterController->GetMaxAwakeDistance();
	if (squaredDistance < d * d)
	{
		if (waterState != kWaterAwake)
		{
			waterState = kWaterAwake;
			waterController->SetAwakeWaterState(this);
		}
	}
	else
	{
		if (waterState == kWaterAwake)
		{
			d = waterController->GetMinAsleepDistance();
			if ((!(squaredDistance < d * d)) || (waterInvisibleTime >= waterController->GetWaterSleepTime()))
			{
				waterState = kWaterDecaying;
				waterController->SetDecayingWaterState(this);
			}
		}
	}
}


HorizonWaterGeometry::HorizonWaterGeometry() : Geometry(kGeometryHorizonWater)
{
	blockNode = nullptr;
}

HorizonWaterGeometry::HorizonWaterGeometry(WaterBlock *block, int32 direction) : Geometry(kGeometryHorizonWater)
{
	blockNode = block;
	SetPerspectiveExclusionMask(kPerspectiveReflection | kPerspectiveRefraction);

	SetNewObject(new HorizonWaterGeometryObject(direction));
}

HorizonWaterGeometry::HorizonWaterGeometry(const HorizonWaterGeometry& horizonWaterGeometry) : Geometry(horizonWaterGeometry)
{
	blockNode = nullptr;
}

HorizonWaterGeometry::~HorizonWaterGeometry()
{
}

Node *HorizonWaterGeometry::Replicate(void) const
{
	return (new HorizonWaterGeometry(*this));
}

bool HorizonWaterGeometry::CalculateBoundingBox(Box3D *box) const
{
	const WaterBlock *block = blockNode;
	if (block)
	{
		const WaterBlockObject *blockObject = block->GetObject();
		int32 dimension = blockObject->GetWaterFieldDimension();
		float scale = blockObject->GetWaterFieldScale();

		const Integer2D& blockSize = block->GetBlockSize();
		float x = (float) (blockSize.x * dimension) * scale;
		float y = (float) (blockSize.y * dimension) * scale;
		float cx = x * 0.5F;
		float cy = y * 0.5F;

		int32 direction = GetObject()->GetWaterDirection();
		float horizon = blockObject->GetWaterHorizonDistance();

		if (direction == kWaterDirectionEast)
		{
			box->min.Set(x, cy - horizon, 0.0F);
			box->max.Set(cx + horizon, cy + horizon, 0.0F);
		}
		else if (direction == kWaterDirectionWest)
		{
			box->min.Set(cx - horizon, cy - horizon, 0.0F);
			box->max.Set(0.0F, cy + horizon, 0.0F);
		}
		else if (direction == kWaterDirectionNorth)
		{
			box->min.Set(cx - horizon, y, 0.0F);
			box->max.Set(cx + horizon, cy + horizon, 0.0F);
		}
		else
		{
			box->min.Set(cx - horizon, cy - horizon, 0.0F);
			box->max.Set(cx + horizon, 0.0F, 0.0F);
		}

		return (true);
	}

	return (false);
}

bool HorizonWaterGeometry::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	const WaterBlock *block = blockNode;
	if (block)
	{
		const WaterBlockObject *blockObject = block->GetObject();
		int32 dimension = blockObject->GetWaterFieldDimension() >> 1;
		float scale = blockObject->GetWaterFieldScale();

		const Integer2D& blockSize = block->GetBlockSize();
		float cx = (float) (blockSize.x * dimension) * scale;
		float cy = (float) (blockSize.y * dimension) * scale;

		sphere->SetCenter(cx, cy, 0.0F);
		sphere->SetRadius(blockObject->GetWaterHorizonDistance());
		return (true);
	}

	return (false);
}

void HorizonWaterGeometry::Preinitialize(void)
{
	Node *node = GetSuperNode();
	if ((node) && (node->GetNodeType() == kNodeWaterBlock))
	{
		blockNode = static_cast<WaterBlock *>(node);
	}

	Geometry::Preinitialize();
}

void HorizonWaterGeometry::Preprocess(void)
{
	Geometry::Preprocess();

	SetShaderFlags(GetShaderFlags() | kShaderGenerateTexcoord);

	const MaterialObject *object = GetMaterialObject(0);
	if (object)
	{
		SetTexcoordParameterPointer(&object->GetTexcoordGeneration());
	}
	else
	{
		SetNullTexcoordParameterPointer();
	}
}


WaterController::WaterController() :
		Controller(kControllerWater),
		randomWaveGenerator(0.03125F, 2)
{
	waterFlags = 0;
	waterSpeed = 6.0e-3F;
	waterViscosity = 2.5e-5F;

	maxAwakeDistance = 500.0F;
	minAsleepDistance = 1000.0F;
	waterSleepTime = 2000;

	moveParity = 0;
}

WaterController::WaterController(const WaterController& waterController) :
		Controller(waterController),
		randomWaveGenerator(waterController.randomWaveGenerator)
{
	waterFlags = waterController.waterFlags;
	waterSpeed = waterController.waterSpeed;
	waterViscosity = waterController.waterViscosity;

	maxAwakeDistance = waterController.maxAwakeDistance;
	minAsleepDistance = waterController.minAsleepDistance;
	waterSleepTime = waterController.waterSleepTime;

	moveParity = 0;
}

WaterController::~WaterController()
{
	moveList.RemoveAll();
}

Controller *WaterController::Replicate(void) const
{
	return (new WaterController(*this));
}

bool WaterController::ValidNode(const Node *node)
{
	return (node->GetNodeType() == kNodeWaterBlock);
}

void WaterController::RegisterFunctions(ControllerRegistration *registration)
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	static FunctionReg<GeneratePointWaveFunction> generatePointWaveRegistration(registration, kFunctionGeneratePointWave, table->GetString(StringID('CTRL', kControllerWater, kFunctionGeneratePointWave)), kFunctionRemote);
	static FunctionReg<GenerateLineWaveFunction> generateLineWaveRegistration(registration, kFunctionGenerateLineWave, table->GetString(StringID('CTRL', kControllerWater, kFunctionGenerateLineWave)), kFunctionRemote);
}

void WaterController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Controller::Pack(data, packFlags);

	data << ChunkHeader('FLAG', 4);
	data << waterFlags;

	data << ChunkHeader('SPED', 4);
	data << waterSpeed;

	data << ChunkHeader('VISC', 4);
	data << waterViscosity;

	data << ChunkHeader('WDST', 4);
	data << maxAwakeDistance;

	data << ChunkHeader('SDST', 4);
	data << minAsleepDistance;

	data << ChunkHeader('SLEP', 4);
	data << waterSleepTime;

	data << ChunkHeader('PRTY', 4);
	data << moveParity;

	if (waterFlags & kWaterRandomWaves)
	{
		PackHandle handle = data.BeginChunk('RWAV');
		randomWaveGenerator.Pack(data, packFlags);
		data.EndChunk(handle);
	}

	const WaveGenerator *generator = generatorList.First();
	while (generator)
	{
		if (generator->GetWaveGeneratorFlags() & kWaveGeneratorNonpersistent)
		{
			PackHandle handle = data.BeginChunk('WGEN');
			generator->PackType(data);
			generator->Pack(data, packFlags);
			data.EndChunk(handle);
		}

		generator = generator->Next();
	}

	data << TerminatorChunk;
}

void WaterController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Controller::Unpack(data, unpackFlags);
	UnpackChunkList<WaterController>(data, unpackFlags);
}

bool WaterController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> waterFlags;
			return (true);

		case 'SPED':

			data >> waterSpeed;
			return (true);

		case 'VISC':

			data >> waterViscosity;
			return (true);

		case 'WDST':

			data >> maxAwakeDistance;
			return (true);

		case 'SDST':

			data >> minAsleepDistance;
			return (true);

		case 'SLEP':

			data >> waterSleepTime;
			return (true);

		case 'PRTY':

			data >> moveParity;
			return (true);

		case 'RWAV':

			randomWaveGenerator.Unpack(data, unpackFlags);
			return (true);

		case 'WGEN':
		{
			WaveGenerator *generator = WaveGenerator::Create(data, unpackFlags);
			if (generator)
			{
				generator->Unpack(++data, unpackFlags);
				generatorList.Append(generator);
				return (true);
			}

			break;
		}

	}

	return (false);
}

void *WaterController::BeginSettingsUnpack(void)
{
	generatorList.Purge();
	return (Controller::BeginSettingsUnpack());
}

int32 WaterController::GetSettingCount(void) const
{
	return (11);
}

Setting *WaterController::GetSetting(int32 index) const
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerWater, 'PARM'));
		return (new HeadingSetting('PARM', title));
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerWater, 'PARM', 'SPED'));
		return (new FloatSetting('SPED', waterSpeed * 1000.0F, title, 0.1F, 20.0F, 0.1F));
	}

	if (index == 2)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerWater, 'PARM', 'VISC'));
		return (new FloatSetting('VISC', waterViscosity * 1000.0F, title, 0.001F, 0.1F, 0.001F));
	}

	if (index == 3)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerWater, 'ACTV'));
		return (new HeadingSetting('ACTV', title));
	}

	if (index == 4)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerWater, 'ACTV', 'WDST'));
		return (new TextSetting('WDST', maxAwakeDistance, title));
	}

	if (index == 5)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerWater, 'ACTV', 'SDST'));
		return (new TextSetting('SDST', minAsleepDistance, title));
	}

	if (index == 6)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerWater, 'ACTV', 'INVS'));
		return (new TextSetting('INVS', (float) waterSleepTime * 0.001F, title));
	}

	if (index == 7)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerWater, 'RAND'));
		return (new HeadingSetting('RAND', title));
	}

	if (index == 8)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerWater, 'RAND', 'RWAV'));
		return (new BooleanSetting('RWAV', ((waterFlags & kWaterRandomWaves) != 0), title));
	}

	if (index == 9)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerWater, 'RAND', 'DELT'));
		return (new TextSetting('DELT', randomWaveGenerator.GetElevationDelta(), title));
	}

	if (index == 10)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerWater, 'RAND', 'PCNT'));
		return (new TextSetting('PCNT', Text::IntegerToString(randomWaveGenerator.GetPositionCount()), title, 2, &EditTextWidget::NumberFilter));
	}

	return (nullptr);
}

void WaterController::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'SPED')
	{
		waterSpeed = static_cast<const FloatSetting *>(setting)->GetFloatValue() * 0.001F;
	}
	else if (identifier == 'VISC')
	{
		waterViscosity = static_cast<const FloatSetting *>(setting)->GetFloatValue() * 0.001F;
	}
	else if (identifier == 'WDST')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		maxAwakeDistance = FmaxZero(Text::StringToFloat(text));
	}
	else if (identifier == 'SDST')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		minAsleepDistance = Fmax(Text::StringToFloat(text), maxAwakeDistance);
	}
	else if (identifier == 'INVS')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		waterSleepTime = MaxZero((int32) (Text::StringToFloat(text) * 1000.0F + 0.5F));
	}
	else if (identifier == 'RWAV')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			waterFlags |= kWaterRandomWaves;
		}
		else
		{
			waterFlags &= ~kWaterRandomWaves;
		}
	}
	else if (identifier == 'DELT')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		randomWaveGenerator.SetElevationDelta(FmaxZero(Text::StringToFloat(text)));
	}
	else if (identifier == 'PCNT')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		randomWaveGenerator.SetPositionCount(Max(Text::StringToInteger(text), 1));
	}
}

void WaterController::Preprocess(void)
{
	Controller::Preprocess();

	const WaterBlock *block = GetTargetNode();
	if (!block->GetManipulator())
	{
		if (waterFlags & kWaterRandomWaves)
		{
			generatorList.Append(&randomWaveGenerator);
		}

		Node *node = block->GetFirstSubnode();
		while (node)
		{
			if (node->GetNodeType() == kNodeGeometry)
			{
				Geometry *geometry = static_cast<Geometry *>(node);
				if (geometry->GetGeometryType() == kGeometryWater)
				{
					WaterGeometry *waterGeometry = static_cast<WaterGeometry *>(geometry);
					unsigned_int32 state = waterGeometry->GetWaterState();
					if (state == kWaterAwake)
					{
						moveList.Append(waterGeometry);
					}
					else if (state == kWaterDecaying)
					{
						decayList.Append(waterGeometry);
					}
				}
			}

			node = node->Next();
		}

		const WaterBlockObject *object = block->GetObject();

		float d = object->GetWaterFieldScale();
		float m1 = waterSpeed * waterSpeed * (kWaterDeltaTime * kWaterDeltaTime) / (d * d);
		float m2 = 0.5F * waterViscosity * kWaterDeltaTime;
		float m3 = 1.0F / (m2 + 1.0F);

		waterConstant[0] = (2.0F - 4.0F * m1) * m3;
		waterConstant[1] = (m2 - 1.0F) * m3;
		waterConstant[2] = m1 * m3;

		moveTime = 0;
	}
}

void WaterController::Neutralize(void)
{
	moveList.RemoveAll();
	randomWaveGenerator.Detach();

	Controller::Neutralize();
}

void WaterController::Move(void)
{
	int32 dt = TheTimeMgr->GetDeltaTime();

	int32 time = moveTime + dt;
	int32 passCount = time / kWaterTimeStep;
	moveTime = time - passCount * kWaterTimeStep;

	if (passCount > 0)
	{
		Advance(dt);
	}
}

void WaterController::Advance(int32 dt)
{
	WaveGenerator *generator = generatorList.First();
	while (generator)
	{
		WaveGenerator *next = generator->Next();

		if (generator->GetWaveGeneratorFlags() & kWaveGeneratorUpdate)
		{
			generator->Update(this);
		}

		if (!generator->Execute(this))
		{
			generatorList.Remove(generator);
			if (generator->GetWaveGeneratorFlags() & kWaveGeneratorNonpersistent)
			{
				delete generator;
			}
		}

		generator = next;
	}

	WaterGeometry *waterGeometry = decayList.First();
	while (waterGeometry)
	{
		WaterGeometry *next = waterGeometry->Next();

		int32 decay = waterGeometry->decayStepIndex;
		if (decay <= kWaterDecayStepCount)
		{
			waterGeometry->decayStepIndex = ++decay;
			decay = Min(decay, kWaterDecayStepCount);
			waterGeometry->SetWaterMultiplier((float) (kWaterDecayStepCount - decay) / (float) (kWaterDecayStepCount - decay + 1));
		}
		else
		{
			waterGeometry->waterState = kWaterAsleep;
			decayList.Remove(waterGeometry);
		}

		waterGeometry = next;
	}

	#if C4STATS

		World *world = GetTargetNode()->GetWorld();

	#endif

	waterGeometry = decayList.First();
	while (waterGeometry)
	{
		#if C4STATS

			world->IncrementWorldCounter(kWorldCounterWaterMove);

		#endif

		waterGeometry->waterUpdateFlag = true;
		waterGeometry->waterInvisibleTime += dt;
		TheJobMgr->SubmitJob(&waterGeometry->waterMoveJob, &moveBatch);

		waterGeometry = waterGeometry->Next();
	}

	waterGeometry = moveList.First();
	while (waterGeometry)
	{
		#if C4STATS

			world->IncrementWorldCounter(kWorldCounterWaterMove);

		#endif

		waterGeometry->waterUpdateFlag = true;
		waterGeometry->waterInvisibleTime += dt;
		TheJobMgr->SubmitJob(&waterGeometry->waterMoveJob, &moveBatch);

		waterGeometry = waterGeometry->Next();
	}

	TheJobMgr->FinishBatch(&moveBatch);
	moveParity ^= 1;
}

void WaterController::SetAwakeWaterState(WaterGeometry *water)
{
	water->SetWaterMultiplier(1.0F);
	moveList.Append(water);
}

void WaterController::SetDecayingWaterState(WaterGeometry *water)
{
	water->SetWaterMultiplier(1.0F);
	water->decayStepIndex = 0;
	decayList.Append(water);
}


GeneratePointWaveFunction::GeneratePointWaveFunction() : Function(kFunctionGeneratePointWave, kControllerWater)
{
	connectorKey[0] = 0;

	generatorRadius = 0.0F;
	elevationDelta = 0.03125F;
	timeStepCount = 1;

	scriptControllerIndex = kControllerUnassigned;
}

GeneratePointWaveFunction::GeneratePointWaveFunction(const GeneratePointWaveFunction& generatePointWaveFunction) : Function(generatePointWaveFunction)
{
	connectorKey = generatePointWaveFunction.connectorKey;

	generatorRadius = generatePointWaveFunction.generatorRadius;
	elevationDelta = generatePointWaveFunction.elevationDelta;
	timeStepCount = generatePointWaveFunction.timeStepCount;

	scriptControllerIndex = generatePointWaveFunction.scriptControllerIndex;
}

GeneratePointWaveFunction::~GeneratePointWaveFunction()
{
}

Function *GeneratePointWaveFunction::Replicate(void) const
{
	return (new GeneratePointWaveFunction(*this));
}

void GeneratePointWaveFunction::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Function::Pack(data, packFlags);

	PackHandle handle = data.BeginChunk('PCON');
	data << connectorKey;
	data.EndChunk(handle);

	data << ChunkHeader('RADI', 4);
	data << generatorRadius;

	data << ChunkHeader('DELT', 4);
	data << elevationDelta;

	data << ChunkHeader('TIME', 4);
	data << timeStepCount;

	data << TerminatorChunk;
}

void GeneratePointWaveFunction::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Function::Unpack(data, unpackFlags);
	UnpackChunkList<GeneratePointWaveFunction>(data, unpackFlags);
}

bool GeneratePointWaveFunction::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'PCON':

			data >> connectorKey;
			return (true);

		case 'RADI':

			data >> generatorRadius;
			return (true);

		case 'DELT':

			data >> elevationDelta;
			return (true);

		case 'TIME':

			data >> timeStepCount;
			return (true);
	}

	return (false);
}

void GeneratePointWaveFunction::Compress(Compressor& data) const
{
	Function::Compress(data);

	data << connectorKey;

	data << generatorRadius;
	data << elevationDelta;
	data << timeStepCount;

	data << scriptControllerIndex;
}

bool GeneratePointWaveFunction::Decompress(Decompressor& data)
{
	if (Function::Decompress(data))
	{
		data >> connectorKey;

		data >> generatorRadius;
		data >> elevationDelta;
		data >> timeStepCount;

		data >> scriptControllerIndex;
		return (true);
	}

	return (false);
}

int32 GeneratePointWaveFunction::GetSettingCount(void) const
{
	return (4);
}

Setting *GeneratePointWaveFunction::GetSetting(int32 index) const
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerWater, kFunctionGeneratePointWave, 'PCON'));
		return (new TextSetting('PCON', connectorKey, title, kMaxConnectorKeyLength, &Connector::ConnectorKeyFilter));
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerWater, kFunctionGeneratePointWave, 'RADI'));
		return (new TextSetting('RADI', generatorRadius, title));
	}

	if (index == 2)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerWater, kFunctionGeneratePointWave, 'DELT'));
		return (new TextSetting('DELT', elevationDelta, title));
	}

	if (index == 3)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerWater, kFunctionGeneratePointWave, 'TIME'));
		return (new TextSetting('TIME', Text::IntegerToString(timeStepCount), title, 4, &EditTextWidget::NumberFilter));
	}

	return (nullptr);
}

void GeneratePointWaveFunction::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'PCON')
	{
		connectorKey = static_cast<const TextSetting *>(setting)->GetText();
	}
	else if (identifier == 'RADI')
	{
		generatorRadius = FmaxZero(Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText()));
	}
	else if (identifier == 'DELT')
	{
		elevationDelta = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
	}
	else if (identifier == 'TIME')
	{
		timeStepCount = Max(Text::StringToInteger(static_cast<const TextSetting *>(setting)->GetText()), 1);
	}
}

void GeneratePointWaveFunction::Preprocess(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	scriptControllerIndex = state->GetScriptController()->GetControllerIndex();
}

void GeneratePointWaveFunction::Execute(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	const Node *block = controller->GetTargetNode();
	const Controller *scriptController = block->GetWorld()->GetController(scriptControllerIndex);
	if (scriptController)
	{
		const Node *node = scriptController->GetTargetNode()->GetConnectedNode(connectorKey);
		if (node)
		{
			Point3D position = block->GetInverseWorldTransform() * node->GetWorldPosition();

			PointWaveGenerator *waveGenerator = new PointWaveGenerator(position.GetPoint2D(), generatorRadius, elevationDelta, timeStepCount);
			waveGenerator->SetWaveGeneratorFlags(waveGenerator->GetWaveGeneratorFlags() | kWaveGeneratorNonpersistent);
			static_cast<WaterController *>(controller)->AddWaveGenerator(waveGenerator);
		}
	}

	CallCompletionProc();
}


GenerateLineWaveFunction::GenerateLineWaveFunction() : Function(kFunctionGenerateLineWave, kControllerWater)
{
	connectorKey[0][0] = 0;
	connectorKey[1][0] = 0;

	generatorRadius = 0.0F;
	elevationDelta = 0.03125F;
	timeStepCount = 1;

	scriptControllerIndex = kControllerUnassigned;
}

GenerateLineWaveFunction::GenerateLineWaveFunction(const GenerateLineWaveFunction& generateLineWaveFunction) : Function(generateLineWaveFunction)
{
	connectorKey[0] = generateLineWaveFunction.connectorKey[0];
	connectorKey[1] = generateLineWaveFunction.connectorKey[1];

	generatorRadius = generateLineWaveFunction.generatorRadius;
	elevationDelta = generateLineWaveFunction.elevationDelta;
	timeStepCount = generateLineWaveFunction.timeStepCount;

	scriptControllerIndex = generateLineWaveFunction.scriptControllerIndex;
}

GenerateLineWaveFunction::~GenerateLineWaveFunction()
{
}

Function *GenerateLineWaveFunction::Replicate(void) const
{
	return (new GenerateLineWaveFunction(*this));
}

void GenerateLineWaveFunction::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Function::Pack(data, packFlags);

	data << ChunkHeader('PCON', 8);
	data << connectorKey[0];
	data << connectorKey[1];

	data << ChunkHeader('RADI', 4);
	data << generatorRadius;

	data << ChunkHeader('DELT', 4);
	data << elevationDelta;

	data << ChunkHeader('TIME', 4);
	data << timeStepCount;

	data << TerminatorChunk;
}

void GenerateLineWaveFunction::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Function::Unpack(data, unpackFlags);
	UnpackChunkList<GenerateLineWaveFunction>(data, unpackFlags);
}

bool GenerateLineWaveFunction::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'PCON':

			data >> connectorKey[0];
			data >> connectorKey[1];
			return (true);

		case 'RADI':

			data >> generatorRadius;
			return (true);

		case 'DELT':

			data >> elevationDelta;
			return (true);

		case 'TIME':

			data >> timeStepCount;
			return (true);
	}

	return (false);
}

void GenerateLineWaveFunction::Compress(Compressor& data) const
{
	Function::Compress(data);

	data << connectorKey[0];
	data << connectorKey[1];

	data << generatorRadius;
	data << elevationDelta;
	data << timeStepCount;

	data << scriptControllerIndex;
}

bool GenerateLineWaveFunction::Decompress(Decompressor& data)
{
	if (Function::Decompress(data))
	{
		data >> connectorKey[0];
		data >> connectorKey[1];

		data >> generatorRadius;
		data >> elevationDelta;
		data >> timeStepCount;

		data >> scriptControllerIndex;
		return (true);
	}

	return (false);
}

int32 GenerateLineWaveFunction::GetSettingCount(void) const
{
	return (5);
}

Setting *GenerateLineWaveFunction::GetSetting(int32 index) const
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerWater, kFunctionGenerateLineWave, 'PCN1'));
		return (new TextSetting('PCN1', connectorKey[0], title, kMaxConnectorKeyLength, &Connector::ConnectorKeyFilter));
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerWater, kFunctionGenerateLineWave, 'PCN2'));
		return (new TextSetting('PCN2', connectorKey[1], title, kMaxConnectorKeyLength, &Connector::ConnectorKeyFilter));
	}

	if (index == 2)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerWater, kFunctionGenerateLineWave, 'RADI'));
		return (new TextSetting('RADI', generatorRadius, title));
	}

	if (index == 3)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerWater, kFunctionGenerateLineWave, 'DELT'));
		return (new TextSetting('DELT', elevationDelta, title));
	}

	if (index == 4)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerWater, kFunctionGenerateLineWave, 'TIME'));
		return (new TextSetting('TIME', Text::IntegerToString(timeStepCount), title, 4, &EditTextWidget::NumberFilter));
	}

	return (nullptr);
}

void GenerateLineWaveFunction::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'PCN1')
	{
		connectorKey[0] = static_cast<const TextSetting *>(setting)->GetText();
	}
	else if (identifier == 'PCN2')
	{
		connectorKey[1] = static_cast<const TextSetting *>(setting)->GetText();
	}
	else if (identifier == 'RADI')
	{
		generatorRadius = FmaxZero(Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText()));
	}
	else if (identifier == 'DELT')
	{
		elevationDelta = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
	}
	else if (identifier == 'TIME')
	{
		timeStepCount = Max(Text::StringToInteger(static_cast<const TextSetting *>(setting)->GetText()), 1);
	}
}

void GenerateLineWaveFunction::Preprocess(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	scriptControllerIndex = state->GetScriptController()->GetControllerIndex();
}

void GenerateLineWaveFunction::Execute(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	const Node *block = controller->GetTargetNode();
	const Controller *scriptController = block->GetWorld()->GetController(scriptControllerIndex);
	if (scriptController)
	{
		const Node *scriptTarget = scriptController->GetTargetNode();
		const Node *node1 = scriptTarget->GetConnectedNode(connectorKey[0]);
		const Node *node2 = scriptTarget->GetConnectedNode(connectorKey[1]);
		if ((node1) && (node2))
		{
			Point3D p1 = block->GetInverseWorldTransform() * node1->GetWorldPosition();
			Point3D p2 = block->GetInverseWorldTransform() * node2->GetWorldPosition();

			LineWaveGenerator *waveGenerator = new LineWaveGenerator(p1.GetPoint2D(), p2.GetPoint2D(), generatorRadius, elevationDelta, timeStepCount);
			waveGenerator->SetWaveGeneratorFlags(waveGenerator->GetWaveGeneratorFlags() | kWaveGeneratorNonpersistent);
			static_cast<WaterController *>(controller)->AddWaveGenerator(waveGenerator);
		}
	}

	CallCompletionProc();
}

// ZYUQURM
