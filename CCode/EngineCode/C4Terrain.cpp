 

#include "C4Terrain.h"
#include "C4World.h"


using namespace C4;


namespace
{
	const Voxel emptySubchannelValue[kTerrainSubchannelCount] =
	{
		0x7F, 0x00, 0x00
	};

	const Voxel solidSubchannelValue[kTerrainSubchannelCount] =
	{
		-0x7F, -0x01, 0x01
	};
}


Heap TerrainStorage::terrainHeap("Terrain", 65536);


VoxelMap::VoxelMap(const Integer3D& size, const Integer3D& origin, const VoxelBox& blockBox, const VoxelBox& voxelBox)
{
	voxelBase = new Voxel[size.x * size.y * size.z];

	rowSize = size.x;
	deckSize = size.x * size.y;
	voxelOrigin = voxelBase + (deckSize * origin.z + rowSize * origin.y + origin.x);

	blockBounds = blockBox;
	voxelBounds = voxelBox;
}

VoxelMap::~VoxelMap()
{
	delete[] voxelBase;
}

Voxel VoxelMap::GetFilteredVoxel(int32 x, int32 y, int32 z) const
{
	int32 x1 = Min(Max(x, voxelBounds.min.x), voxelBounds.max.x);
	int32 y1 = Min(Max(y, voxelBounds.min.y), voxelBounds.max.y);
	int32 z1 = Min(Max(z, voxelBounds.min.z), voxelBounds.max.z);

	int32 x0 = Min(Max(x - 1, voxelBounds.min.x), voxelBounds.max.x);
	int32 x2 = Min(Max(x + 1, voxelBounds.min.x), voxelBounds.max.x);
	int32 y0 = Min(Max(y - 1, voxelBounds.min.y), voxelBounds.max.y);
	int32 y2 = Min(Max(y + 1, voxelBounds.min.y), voxelBounds.max.y);
	int32 z0 = Min(Max(z - 1, voxelBounds.min.z), voxelBounds.max.z);
	int32 z2 = Min(Max(z + 1, voxelBounds.min.z), voxelBounds.max.z);

	int32 u = voxelOrigin[z1 * deckSize + y1 * rowSize + x1];
	int32 f1 = voxelOrigin[z1 * deckSize + y1 * rowSize + x0] + voxelOrigin[z1 * deckSize + y1 * rowSize + x2];
	int32 f2 = voxelOrigin[z1 * deckSize + y0 * rowSize + x1] + voxelOrigin[z1 * deckSize + y2 * rowSize + x1];
	int32 f3 = voxelOrigin[z0 * deckSize + y1 * rowSize + x1] + voxelOrigin[z2 * deckSize + y1 * rowSize + x1];
	u = ((u + f1) + (f2 + f3)) * 2;

	int32 e1 = voxelOrigin[z1 * deckSize + y0 * rowSize + x0] + voxelOrigin[z1 * deckSize + y0 * rowSize + x2];
	int32 e2 = voxelOrigin[z1 * deckSize + y2 * rowSize + x0] + voxelOrigin[z1 * deckSize + y2 * rowSize + x2];
	int32 e3 = voxelOrigin[z0 * deckSize + y1 * rowSize + x0] + voxelOrigin[z0 * deckSize + y1 * rowSize + x2];
	int32 e4 = voxelOrigin[z2 * deckSize + y1 * rowSize + x0] + voxelOrigin[z2 * deckSize + y1 * rowSize + x2];
	int32 e5 = voxelOrigin[z0 * deckSize + y0 * rowSize + x1] + voxelOrigin[z0 * deckSize + y2 * rowSize + x1];
	int32 e6 = voxelOrigin[z2 * deckSize + y0 * rowSize + x1] + voxelOrigin[z2 * deckSize + y2 * rowSize + x1];
	u += (e1 + e2) + (e3 + e4) + (e5 + e6);

	return ((Voxel) (u / 26));
}

Voxel VoxelMap::GetMinHorizontalVoxel(int32 x, int32 y, int32 z, Fixed bias) const
{
	int32 x1 = Min(Max(x, voxelBounds.min.x), voxelBounds.max.x);
	int32 y1 = Min(Max(y, voxelBounds.min.y), voxelBounds.max.y);
	int32 z1 = Min(Max(z, voxelBounds.min.z), voxelBounds.max.z);
	int32 z2 = Min(Max(z + Sgn(bias), voxelBounds.min.z), voxelBounds.max.z);

	int32 x0 = Min(Max(x - 1, voxelBounds.min.x), voxelBounds.max.x);
	int32 x2 = Min(Max(x + 1, voxelBounds.min.x), voxelBounds.max.x);
	int32 y0 = Min(Max(y - 1, voxelBounds.min.y), voxelBounds.max.y);
	int32 y2 = Min(Max(y + 1, voxelBounds.min.y), voxelBounds.max.y);

	int32 u1 = voxelOrigin[z1 * deckSize + y1 * rowSize + x1];
	int32 u2 = voxelOrigin[z1 * deckSize + y1 * rowSize + x0];
	int32 u3 = voxelOrigin[z1 * deckSize + y1 * rowSize + x2];
	int32 u4 = voxelOrigin[z1 * deckSize + y0 * rowSize + x1];
	int32 u5 = voxelOrigin[z1 * deckSize + y2 * rowSize + x1];
	int32 u6 = voxelOrigin[z1 * deckSize + y0 * rowSize + x0];
	int32 u7 = voxelOrigin[z1 * deckSize + y0 * rowSize + x2];
	int32 u8 = voxelOrigin[z1 * deckSize + y2 * rowSize + x0];
	int32 u9 = voxelOrigin[z1 * deckSize + y2 * rowSize + x2];

	int32 v1 = voxelOrigin[z2 * deckSize + y1 * rowSize + x1];
	int32 v2 = voxelOrigin[z2 * deckSize + y1 * rowSize + x0];
	int32 v3 = voxelOrigin[z2 * deckSize + y1 * rowSize + x2];
	int32 v4 = voxelOrigin[z2 * deckSize + y0 * rowSize + x1];
	int32 v5 = voxelOrigin[z2 * deckSize + y2 * rowSize + x1];
	int32 v6 = voxelOrigin[z2 * deckSize + y0 * rowSize + x0];
	int32 v7 = voxelOrigin[z2 * deckSize + y0 * rowSize + x2];
	int32 v8 = voxelOrigin[z2 * deckSize + y2 * rowSize + x0]; 
	int32 v9 = voxelOrigin[z2 * deckSize + y2 * rowSize + x2];

	Fixed t = Abs(bias); 
	u1 = (u1 * (256 - t) + v1 * t) >> 8;
	u2 = (u2 * (256 - t) + v2 * t) >> 8; 
	u3 = (u3 * (256 - t) + v3 * t) >> 8;
	u4 = (u4 * (256 - t) + v4 * t) >> 8;
	u5 = (u5 * (256 - t) + v5 * t) >> 8; 
	u6 = (u6 * (256 - t) + v6 * t) >> 8;
	u7 = (u7 * (256 - t) + v7 * t) >> 8; 
	u8 = (u8 * (256 - t) + v8 * t) >> 8; 
	u9 = (u9 * (256 - t) + v9 * t) >> 8;
	return ((Voxel) Min(Min(Min(Min(u1, u2), Min(u3, u4)), Min(Min(u5, u6), Min(u7, u8))), u9));
}
 
Voxel VoxelMap::GetMaxHorizontalVoxel(int32 x, int32 y, int32 z, Fixed bias) const
{
	int32 x1 = Min(Max(x, voxelBounds.min.x), voxelBounds.max.x);
	int32 y1 = Min(Max(y, voxelBounds.min.y), voxelBounds.max.y);
	int32 z1 = Min(Max(z, voxelBounds.min.z), voxelBounds.max.z);
	int32 z2 = Min(Max(z + Sgn(bias), voxelBounds.min.z), voxelBounds.max.z);

	int32 x0 = Min(Max(x - 1, voxelBounds.min.x), voxelBounds.max.x);
	int32 x2 = Min(Max(x + 1, voxelBounds.min.x), voxelBounds.max.x);
	int32 y0 = Min(Max(y - 1, voxelBounds.min.y), voxelBounds.max.y);
	int32 y2 = Min(Max(y + 1, voxelBounds.min.y), voxelBounds.max.y);

	int32 u1 = voxelOrigin[z1 * deckSize + y1 * rowSize + x1];
	int32 u2 = voxelOrigin[z1 * deckSize + y1 * rowSize + x0];
	int32 u3 = voxelOrigin[z1 * deckSize + y1 * rowSize + x2];
	int32 u4 = voxelOrigin[z1 * deckSize + y0 * rowSize + x1];
	int32 u5 = voxelOrigin[z1 * deckSize + y2 * rowSize + x1];
	int32 u6 = voxelOrigin[z1 * deckSize + y0 * rowSize + x0];
	int32 u7 = voxelOrigin[z1 * deckSize + y0 * rowSize + x2];
	int32 u8 = voxelOrigin[z1 * deckSize + y2 * rowSize + x0];
	int32 u9 = voxelOrigin[z1 * deckSize + y2 * rowSize + x2];

	int32 v1 = voxelOrigin[z2 * deckSize + y1 * rowSize + x1];
	int32 v2 = voxelOrigin[z2 * deckSize + y1 * rowSize + x0];
	int32 v3 = voxelOrigin[z2 * deckSize + y1 * rowSize + x2];
	int32 v4 = voxelOrigin[z2 * deckSize + y0 * rowSize + x1];
	int32 v5 = voxelOrigin[z2 * deckSize + y2 * rowSize + x1];
	int32 v6 = voxelOrigin[z2 * deckSize + y0 * rowSize + x0];
	int32 v7 = voxelOrigin[z2 * deckSize + y0 * rowSize + x2];
	int32 v8 = voxelOrigin[z2 * deckSize + y2 * rowSize + x0];
	int32 v9 = voxelOrigin[z2 * deckSize + y2 * rowSize + x2];

	Fixed t = Abs(bias);
	u1 = (u1 * (256 - t) + v1 * t) >> 8;
	u2 = (u2 * (256 - t) + v2 * t) >> 8;
	u3 = (u3 * (256 - t) + v3 * t) >> 8;
	u4 = (u4 * (256 - t) + v4 * t) >> 8;
	u5 = (u5 * (256 - t) + v5 * t) >> 8;
	u6 = (u6 * (256 - t) + v6 * t) >> 8;
	u7 = (u7 * (256 - t) + v7 * t) >> 8;
	u8 = (u8 * (256 - t) + v8 * t) >> 8;
	u9 = (u9 * (256 - t) + v9 * t) >> 8;
	return ((Voxel) Max(Max(Max(Max(u1, u2), Max(u3, u4)), Max(Max(u5, u6), Max(u7, u8))), u9));
}

void VoxelMap::SetUnsignedVoxelOctet(int32 x, int32 y, int32 z, UnsignedVoxel v) const
{
	int32 x1 = Max(x - 1, voxelBounds.min.x);
	int32 y1 = Max(y - 1, voxelBounds.min.y);
	int32 z1 = Max(z - 1, voxelBounds.min.z);

	int32 d = z * deckSize;
	unsignedVoxelOrigin[d + y * rowSize + x] = v;
	unsignedVoxelOrigin[d + y * rowSize + x1] = v;
	unsignedVoxelOrigin[d + y1 * rowSize + x] = v;
	unsignedVoxelOrigin[d + y1 * rowSize + x1] = v;

	int32 d1 = z1 * deckSize;
	unsignedVoxelOrigin[d1 + y * rowSize + x] = v;
	unsignedVoxelOrigin[d1 + y * rowSize + x1] = v;
	unsignedVoxelOrigin[d1 + y1 * rowSize + x] = v;
	unsignedVoxelOrigin[d1 + y1 * rowSize + x1] = v;
}

Vector3D VoxelMap::CalculateNormal(const Integer3D& coord) const
{
	int32 x1 = Min(Max(coord.x, voxelBounds.min.x), voxelBounds.max.x);
	int32 y1 = Min(Max(coord.y, voxelBounds.min.y), voxelBounds.max.y);
	int32 z1 = Min(Max(coord.z, voxelBounds.min.z), voxelBounds.max.z);

	int32 x0 = Min(Max(coord.x - 1, voxelBounds.min.x), voxelBounds.max.x);
	int32 x2 = Min(Max(coord.x + 1, voxelBounds.min.x), voxelBounds.max.x);
	int32 y0 = Min(Max(coord.y - 1, voxelBounds.min.y), voxelBounds.max.y);
	int32 y2 = Min(Max(coord.y + 1, voxelBounds.min.y), voxelBounds.max.y);
	int32 z0 = Min(Max(coord.z - 1, voxelBounds.min.z), voxelBounds.max.z);
	int32 z2 = Min(Max(coord.z + 1, voxelBounds.min.z), voxelBounds.max.z);

	Voxel v000 = voxelOrigin[z0 * deckSize + y0 * rowSize + x0];
	Voxel v100 = voxelOrigin[z0 * deckSize + y0 * rowSize + x1];
	Voxel v200 = voxelOrigin[z0 * deckSize + y0 * rowSize + x2];
	Voxel v010 = voxelOrigin[z0 * deckSize + y1 * rowSize + x0];
	Voxel v110 = voxelOrigin[z0 * deckSize + y1 * rowSize + x1];
	Voxel v210 = voxelOrigin[z0 * deckSize + y1 * rowSize + x2];
	Voxel v020 = voxelOrigin[z0 * deckSize + y2 * rowSize + x0];
	Voxel v120 = voxelOrigin[z0 * deckSize + y2 * rowSize + x1];
	Voxel v220 = voxelOrigin[z0 * deckSize + y2 * rowSize + x2];

	Voxel v001 = voxelOrigin[z1 * deckSize + y0 * rowSize + x0];
	Voxel v101 = voxelOrigin[z1 * deckSize + y0 * rowSize + x1];
	Voxel v201 = voxelOrigin[z1 * deckSize + y0 * rowSize + x2];
	Voxel v011 = voxelOrigin[z1 * deckSize + y1 * rowSize + x0];
	Voxel v211 = voxelOrigin[z1 * deckSize + y1 * rowSize + x2];
	Voxel v021 = voxelOrigin[z1 * deckSize + y2 * rowSize + x0];
	Voxel v121 = voxelOrigin[z1 * deckSize + y2 * rowSize + x1];
	Voxel v221 = voxelOrigin[z1 * deckSize + y2 * rowSize + x2];

	Voxel v002 = voxelOrigin[z2 * deckSize + y0 * rowSize + x0];
	Voxel v102 = voxelOrigin[z2 * deckSize + y0 * rowSize + x1];
	Voxel v202 = voxelOrigin[z2 * deckSize + y0 * rowSize + x2];
	Voxel v012 = voxelOrigin[z2 * deckSize + y1 * rowSize + x0];
	Voxel v112 = voxelOrigin[z2 * deckSize + y1 * rowSize + x1];
	Voxel v212 = voxelOrigin[z2 * deckSize + y1 * rowSize + x2];
	Voxel v022 = voxelOrigin[z2 * deckSize + y2 * rowSize + x0];
	Voxel v122 = voxelOrigin[z2 * deckSize + y2 * rowSize + x1];
	Voxel v222 = voxelOrigin[z2 * deckSize + y2 * rowSize + x2];

	int32 dx = (v211 - v011) * 4 + (v201 + v221 + v210 + v212 - v001 - v021 - v010 - v012) * 2 + v200 + v220 + v202 + v222 - v000 - v020 - v002 - v022;
	int32 dy = (v121 - v101) * 4 + (v021 + v221 + v120 + v122 - v001 - v201 - v100 - v102) * 2 + v020 + v220 + v022 + v222 - v000 - v200 - v002 - v202;
	int32 dz = (v112 - v110) * 4 + (v012 + v212 + v102 + v122 - v010 - v210 - v100 - v120) * 2 + v002 + v202 + v022 + v222 - v000 - v200 - v020 - v220;

	return (Vector3D((float) dx, (float) dy, (float) dz).Normalize());
}

Vector3D VoxelMap::CalculateNormal(const Integer3D& coord0, const Integer3D& coord1, Fixed t, Fixed u) const
{
	Vector3D n0 = CalculateNormal(coord0);
	Vector3D n1 = CalculateNormal(coord1);
	return ((n0 * (float) t + n1 * (float) u).Normalize());
}


TerrainStorage::TerrainStorage()
{
	channelCount = 1;

	for (machine a = 0; a < kMaxTerrainChannelCount; a++)
	{
		for (machine b = 0; b < kTerrainSubchannelCount; b++)
		{
			voxelData[a][b] = nullptr;
		}
	}
}

TerrainStorage::TerrainStorage(const Integer3D& size)
{
	channelCount = 1;

	for (machine a = 0; a < kMaxTerrainChannelCount; a++)
	{
		for (machine b = 0; b < kTerrainSubchannelCount; b++)
		{
			voxelData[a][b] = nullptr;
		}
	}

	SetNewBlockSize(size);
}

TerrainStorage::TerrainStorage(const TerrainStorage& terrainStorage)
{
	channelCount = 1;

	for (machine a = 0; a < kMaxTerrainChannelCount; a++)
	{
		for (machine b = 0; b < kTerrainSubchannelCount; b++)
		{
			voxelData[a][b] = nullptr;
		}
	}

	SetNewBlockSize(terrainStorage.blockSize);
	CopyNewBlock(&terrainStorage);
}

TerrainStorage::~TerrainStorage()
{
	ClearBlock();

	for (machine a = 0; a < kMaxTerrainChannelCount; a++)
	{
		for (machine b = 0; b < kTerrainSubchannelCount; b++)
		{
			delete[] voxelData[a][b];
		}
	}
}

void TerrainStorage::DeleteVoxelData(char *data)
{
	machine_address ptr = reinterpret_cast<machine_address>(data) & ~machine_address(1);
	terrainHeap.Delete(reinterpret_cast<char *>(ptr));
}

void TerrainStorage::ClearChannel(int32 channel)
{
	for (machine b = 0; b < kTerrainSubchannelCount; b++)
	{
		char **subchannelData = voxelData[channel][b];
		if (subchannelData)
		{
			int32 count = blockSize.x * blockSize.y * blockSize.z;
			for (machine c = count - 1; c >= 0; c--)
			{
				DeleteVoxelData(subchannelData[c]);
				subchannelData[c] = nullptr;
			}
		}
	}
}

void TerrainStorage::ClearBlock(void)
{
	for (machine a = 0; a < kMaxTerrainChannelCount; a++)
	{
		ClearChannel(a);
	}
}

void TerrainStorage::SetBlockSize(const Integer3D& size)
{
	ClearBlock();

	for (machine a = 0; a < kMaxTerrainChannelCount; a++)
	{
		for (machine b = 0; b < kTerrainSubchannelCount; b++)
		{
			delete[] voxelData[a][b];
			voxelData[a][b] = nullptr;
		}
	}

	SetNewBlockSize(size);
}

void TerrainStorage::SetNewBlockSize(const Integer3D& size)
{
	blockSize = size;
	int32 count = size.x * size.y * size.z;

	for (machine b = 0; b < kTerrainSubchannelCount; b++)
	{
		char **subchannelData = new char *[count];
		voxelData[0][b] = subchannelData;
		for (machine c = 0; c < count; c++)
		{
			subchannelData[c] = nullptr;
		}
	}
}

void TerrainStorage::CopyBlock(const TerrainStorage *storage)
{
	SetBlockSize(storage->blockSize);
	CopyNewBlock(storage);
}

void TerrainStorage::CopyNewBlock(const TerrainStorage *storage)
{
	int32 count = blockSize.x * blockSize.y * blockSize.z;

	for (machine b = 0; b < kTerrainSubchannelCount; b++)
	{
		const char *const *subchannelData = storage->voxelData[0][b];
		if (subchannelData)
		{
			for (machine c = 0; c < count; c++)
			{
				const char *compressedData = subchannelData[c];
				if (compressedData)
				{
					if (GetPointerAddress(compressedData) != 1)
					{
						unsigned_int32 size = Heap::GetMemBlockSize(compressedData);
						unsigned_int32 paddedSize = (size + 3) & ~3;

						char *data = terrainHeap.New<char>(paddedSize);
						voxelData[0][b][c] = data;

						MemoryMgr::CopyMemory(compressedData, data, size);
						for (unsigned_machine d = size; d < paddedSize; d++)
						{
							data[d] = 0;
						}
					}
					else
					{
						voxelData[0][b][c] = reinterpret_cast<char *>(1);
					}
				}
			}
		}
	}
}


TerrainBlockObject::TerrainBlockObject() : Object(kObjectTerrainBlock)
{
}

TerrainBlockObject::TerrainBlockObject(float scale) : Object(kObjectTerrainBlock)
{
	voxelScale = scale;
}

TerrainBlockObject::~TerrainBlockObject()
{
}

void TerrainBlockObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	data << ChunkHeader('SCAL', 4);
	data << voxelScale;

	data << TerminatorChunk;
}

void TerrainBlockObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	UnpackChunkList<TerrainBlockObject>(data, unpackFlags);
}

bool TerrainBlockObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'SCAL':

			data >> voxelScale;
			return (true);
	}

	return (false);
}

int32 TerrainBlockObject::GetObjectSize(float *size) const
{
	size[0] = voxelScale;
	return (1);
}

void TerrainBlockObject::SetObjectSize(const float *size)
{
	voxelScale = size[0];
}


TerrainBlock::TerrainBlock() : Node(kNodeTerrainBlock)
{
	MemoryMgr::ClearMemory(materialUsage, kTerrainMaterialUsageTableSize);
	materialUsage[0] = true;

	terrainMaterial[0].primaryMaterial.Set(0, 0, 0);
	terrainMaterial[0].secondaryMaterial.Set(1, 1, 1);
}

TerrainBlock::TerrainBlock(const Integer3D& size, float scale, const TerrainMaterial *material) :
		Node(kNodeTerrainBlock),
		TerrainStorage(size)
{
	SetNewObject(new TerrainBlockObject(scale));

	MemoryMgr::ClearMemory(materialUsage, kTerrainMaterialUsageTableSize);
	materialUsage[0] = true;

	terrainMaterial[0].primaryMaterial = material->primaryMaterial;
	terrainMaterial[0].secondaryMaterial = material->secondaryMaterial;
}

TerrainBlock::TerrainBlock(const TerrainBlock& terrainBlock) : Node(terrainBlock)
{
	MemoryMgr::CopyMemory(terrainBlock.materialUsage, materialUsage, kTerrainMaterialUsageTableSize);

	for (machine a = 0; a < kMaxTerrainMaterialCount; a++)
	{
		if (materialUsage[a])
		{
			terrainMaterial[a].primaryMaterial = terrainBlock.terrainMaterial[a].primaryMaterial;
			terrainMaterial[a].secondaryMaterial = terrainBlock.terrainMaterial[a].secondaryMaterial;
		}
	}
}

TerrainBlock::~TerrainBlock()
{
}

Node *TerrainBlock::Replicate(void) const
{
	return (new TerrainBlock(*this));
}

void TerrainBlock::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Node::Pack(data, packFlags);

	data << ChunkHeader('SIZE', sizeof(Integer3D));
	data << blockSize;

	data << ChunkHeader('MUSG', kTerrainMaterialUsageTableSize);
	data.WriteData(materialUsage, kTerrainMaterialUsageTableSize);

	int32 count = 0;
	for (machine a = 0; a < kMaxTerrainMaterialCount; a++)
	{
		count += materialUsage[a];
	}

	data << ChunkHeader('MATL', count * (4 + sizeof(Color4C) * 2) + 4);
	data << count;

	for (machine a = 0; a < kMaxTerrainMaterialCount; a++)
	{
		if (materialUsage[a])
		{
			data << int32(a);
			const TerrainMaterial *material = &terrainMaterial[a];
			data << material->primaryMaterial;
			data << material->secondaryMaterial;
		}
	}

	count = blockSize.x * blockSize.y * blockSize.z;

	for (machine b = 0; b < kTerrainSubchannelCount; b++)
	{
		const char *const *subchannelData = voxelData[0][b];
		if (subchannelData)
		{
			for (machine c = 0; c < count; c++)
			{
				const char *compressedData = subchannelData[c];
				if (compressedData)
				{
					if (GetPointerAddress(compressedData) != 1)
					{
						unsigned_int32 size = Heap::GetMemBlockSize(compressedData);

						data << ChunkHeader('DAT0' + b, size + 8);
						data << int32(c);
						data << size;

						data.WriteData(compressedData, size);
					}
					else
					{
						data << ChunkHeader('SLD0' + b, 4);
						data << int32(c);
					}
				}
			}
		}
	}

	data << TerminatorChunk;
}

void TerrainBlock::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Node::Unpack(data, unpackFlags);
	UnpackChunkList<TerrainBlock>(data, unpackFlags);
}

bool TerrainBlock::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	unsigned_int32 type = chunkHeader->chunkType;
	switch (type)
	{
		case 'SIZE':
		{
			Integer3D	size;

			data >> size;
			SetNewBlockSize(size);
			return (true);
		}

		case 'MUSG':

			data.ReadData(materialUsage, kTerrainMaterialUsageTableSize);
			return (true);

		case 'MATL':
		{
			int32	count;

			data >> count;
			for (machine a = 0; a < count; a++)
			{
				int32	index;

				data >> index;
				TerrainMaterial *material = &terrainMaterial[index];
				data >> material->primaryMaterial;
				data >> material->secondaryMaterial;
			}

			return (true);
		}

		case 'DAT0':
		case 'DAT1':
		case 'DAT2':
		{
			int32			index;
			unsigned_int32	size;

			data >> index;
			data >> size;

			unsigned_int32 paddedSize = (size + 3) & ~3;
			char *compressedData = terrainHeap.New<char>(paddedSize);
			voxelData[0][type - 'DAT0'][index] = compressedData;

			data.ReadData(compressedData, size);
			for (unsigned_machine a = size; a < paddedSize; a++)
			{
				compressedData[a] = 0;
			}

			return (true);
		}

		case 'SLD0':
		case 'SLD1':
		case 'SLD2':
		{
			int32	index;

			data >> index;
			voxelData[0][type - 'SLD0'][index] = reinterpret_cast<char *>(1);
			return (true);
		}
	}

	return (false);
}

void *TerrainBlock::BeginSettingsUnpack(void)
{
	ClearBlock();

	for (machine a = 0; a < kMaxTerrainChannelCount; a++)
	{
		for (machine b = 0; b < kTerrainSubchannelCount; b++)
		{
			delete[] voxelData[a][b];
			voxelData[a][b] = nullptr;
		}
	}

	return (Node::BeginSettingsUnpack());
}

bool TerrainBlock::CalculateBoundingBox(Box3D *box) const
{
	box->min.Set(0.0F, 0.0F, 0.0F);
	box->max = GetBlockBoxSize();
	return (true);
}

void TerrainBlock::ResizeBlock(const Integer3D& size, const Integer3D& offset)
{
	char	**newVoxelData[kMaxTerrainChannelCount][kTerrainSubchannelCount];

	int32 count = size.x * size.y * size.z;

	for (machine a = 0; a < kMaxTerrainChannelCount; a++)
	{
		for (machine b = 0; b < kTerrainSubchannelCount; b++)
		{
			if (voxelData[a][b])
			{
				char **newSubchannelData = new char *[count];
				newVoxelData[a][b] = newSubchannelData;

				int32 index = 0;
				char **subchannelData = voxelData[a][b];

				for (machine k = 0; k < size.z; k++)
				{
					int32 z = k - offset.z;
					for (machine j = 0; j < size.y; j++)
					{
						int32 y = j - offset.y;
						for (machine i = 0; i < size.x; i++)
						{
							int32 x = i - offset.x;
							if (((unsigned_int32) x < (unsigned_int32) blockSize.x) && ((unsigned_int32) y < (unsigned_int32) blockSize.y) && ((unsigned_int32) z < (unsigned_int32) blockSize.z))
							{
								int32 d = (z * blockSize.y + y) * blockSize.x + x;
								newSubchannelData[index] = subchannelData[d];
								subchannelData[d] = nullptr;
							}
							else
							{
								newSubchannelData[index] = nullptr;
							}

							index++;
						}
					}
				}
			}
			else
			{
				newVoxelData[a][b] = nullptr;
			}
		}
	}

	ClearBlock();

	for (machine a = 0; a < kMaxTerrainChannelCount; a++)
	{
		for (machine b = 0; b < kTerrainSubchannelCount; b++)
		{
			delete[] voxelData[a][b];
			voxelData[a][b] = newVoxelData[a][b];
		}
	}

	blockSize = size;
}

int32 TerrainBlock::CountConstantValues(const Voxel *voxel, int32 rowSize, int32 deckSize, int32 i, int32 j, int32 k)
{
	Voxel v = voxel[k * deckSize + j * rowSize + i];
	int32 count = 1;

	for (;; count++)
	{
		i++;
		j += i >> kTerrainLogDimension;
		k += j >> kTerrainLogDimension;
		i &= kTerrainDimension - 1;
		j &= kTerrainDimension - 1;

		if (k >= kTerrainDimension)
		{
			break;
		}

		if (voxel[k * deckSize + j * rowSize + i] != v)
		{
			break;
		}
	}

	return (count);
}

int32 TerrainBlock::CountVaryingValues(const Voxel *voxel, int32 rowSize, int32 deckSize, int32 i, int32 j, int32 k)
{
	Voxel v = voxel[k * deckSize + j * rowSize + i];
	int32 count = 1;

	for (;; count++)
	{
		i++;
		j += i >> kTerrainLogDimension;
		k += j >> kTerrainLogDimension;
		i &= kTerrainDimension - 1;
		j &= kTerrainDimension - 1;

		if (k >= kTerrainDimension)
		{
			break;
		}

		Voxel w = voxel[k * deckSize + j * rowSize + i];
		if (w == v)
		{
			return (count - 1);
		}

		v = w;
	}

	return (count);
}

unsigned_int32 TerrainBlock::CompressVoxelData(const Voxel *voxel, int32 rowSize, int32 deckSize, char *data)
{
	const char *base = data;

	int32 i = 0;
	int32 j = 0;
	int32 k = 0;

	do
	{
		int32 count = CountConstantValues(voxel, rowSize, deckSize, i, j, k);
		if (count > 1)
		{
			data[0] = (char) (count >> 8) | 0x80;
			data[1] = (char) count;
			data[2] = (char) voxel[k * deckSize + j * rowSize + i];
			data += 3;

			i += count;
			j += i >> kTerrainLogDimension;
			k += j >> kTerrainLogDimension;
			i &= kTerrainDimension - 1;
			j &= kTerrainDimension - 1;
		}
		else
		{
			count = CountVaryingValues(voxel, rowSize, deckSize, i, j, k);

			data[0] = (char) (count >> 8);
			data[1] = (char) count;
			data += 2;

			do
			{
				*data++ = (char) voxel[k * deckSize + j * rowSize + i];

				i++;
				j += i >> kTerrainLogDimension;
				k += j >> kTerrainLogDimension;
				i &= kTerrainDimension - 1;
				j &= kTerrainDimension - 1;

			} while (--count > 0);
		}
	} while (k < kTerrainDimension);

	return ((unsigned_int32) (data - base));
}

void TerrainBlock::DecompressVoxelData(const char *data, Voxel *voxel, int32 rowSize, int32 deckSize)
{
	int32 i = 0;
	int32 j = 0;
	int32 k = 0;

	do
	{
		const unsigned_int8 *code = reinterpret_cast<const unsigned_int8 *>(data);
		unsigned_int16 d = (code[0] << 8) | code[1];
		int32 count = d & 0x7FFF;
		data += 2;

		if ((d & 0x8000) != 0)
		{
			Voxel v = *reinterpret_cast<const Voxel *>(data);
			do
			{
				voxel[k * deckSize + j * rowSize + i] = v;

				i++;
				j += i >> kTerrainLogDimension;
				k += j >> kTerrainLogDimension;
				i &= kTerrainDimension - 1;
				j &= kTerrainDimension - 1;

			} while (--count > 0);

			data++;
		}
		else
		{
			do
			{
				voxel[k * deckSize + j * rowSize + i] = *reinterpret_cast<const Voxel *>(data);
				data++;

				i++;
				j += i >> kTerrainLogDimension;
				k += j >> kTerrainLogDimension;
				i &= kTerrainDimension - 1;
				j &= kTerrainDimension - 1;

			} while (--count > 0);
		}
	} while (k < kTerrainDimension);
}

void TerrainBlock::LoadVoxels(int32 subchannel, const char *const *subchannelData, const Integer3D& coord, Voxel *voxel, int32 rowSize, int32 deckSize) const
{
	const char *data = subchannelData[(coord.z * blockSize.y + coord.y) * blockSize.x + coord.x];
	if (data)
	{
		if (GetPointerAddress(data) != 1)
		{
			DecompressVoxelData(data, voxel, rowSize, deckSize);
		}
		else
		{
			Voxel v = solidSubchannelValue[subchannel];

			for (machine k = 0; k < kTerrainDimension; k++)
			{
				for (machine j = 0; j < kTerrainDimension; j++)
				{
					for (machine i = 0; i < kTerrainDimension; i++)
					{
						voxel[i] = v;
					}

					voxel += rowSize;
				}

				voxel += deckSize - rowSize * kTerrainDimension;
			}
		}
	}
	else
	{
		Voxel v = emptySubchannelValue[subchannel];

		for (machine k = 0; k < kTerrainDimension; k++)
		{
			for (machine j = 0; j < kTerrainDimension; j++)
			{
				for (machine i = 0; i < kTerrainDimension; i++)
				{
					voxel[i] = v;
				}

				voxel += rowSize;
			}

			voxel += deckSize - rowSize * kTerrainDimension;
		}
	}
}

void TerrainBlock::StoreVoxels(int32 subchannel, const Integer3D& coord, const Voxel *voxel, int32 rowSize, int32 deckSize)
{
	static char buffer[kTerrainDimension * kTerrainDimension * kTerrainDimension * 2];

	int32 index = (coord.z * blockSize.y + coord.y) * blockSize.x + coord.x;
	char **subchannelData = voxelData[0][subchannel];
	DeleteVoxelData(subchannelData[index]);

	unsigned_int32 size = CompressVoxelData(voxel, rowSize, deckSize, buffer);
	if (size == 3)
	{
		Voxel v = buffer[2];
		if (v == emptySubchannelValue[subchannel])
		{
			subchannelData[index] = nullptr;
			return;
		}
		else if (v == solidSubchannelValue[subchannel])
		{
			subchannelData[index] = reinterpret_cast<char *>(1);
			return;
		}
	}

	unsigned_int32 paddedSize = (size + 3) & ~3;
	char *data = terrainHeap.New<char>(paddedSize);
	subchannelData[index] = data;

	MemoryMgr::CopyMemory(buffer, data, size);
	for (unsigned_machine a = size; a < paddedSize; a++)
	{
		data[a] = 0;
	}
}

VoxelMap *TerrainBlock::OpenBuildVoxelMap(int32 subchannel, const Integer3D& coord, int32 level) const
{
	VoxelBox	blockBounds, voxelBounds;

	int32 m = 1 << level;
	int32 xmin = MaxZero(coord.x - 1);
	int32 xmax = Min(coord.x + m, blockSize.x - 1);
	int32 ymin = MaxZero(coord.y - 1);
	int32 ymax = Min(coord.y + m, blockSize.y - 1);
	int32 zmin = MaxZero(coord.z - 1);
	int32 zmax = Min(coord.z + m, blockSize.z - 1);

	blockBounds.min.Set(xmin, ymin, zmin);
	blockBounds.max.Set(xmax, ymax, zmax);

	int32 maxBounds = m * kTerrainDimension + 1;
	voxelBounds.min.Set((xmin - coord.x) * m, (ymin - coord.y) * m, (zmin - coord.z) * m);
	voxelBounds.max.Set(Min((xmax - coord.x + 1) * kTerrainDimension - 1, maxBounds), Min((ymax - coord.y + 1) * kTerrainDimension - 1, maxBounds), Min((zmax - coord.z + 1) * kTerrainDimension - 1, maxBounds));

	Integer3D size((xmax - xmin + 1) * kTerrainDimension, (ymax - ymin + 1) * kTerrainDimension, (zmax - zmin + 1) * kTerrainDimension);
	Integer3D origin((coord.x - xmin) * kTerrainDimension, (coord.y - ymin) * kTerrainDimension, (coord.z - zmin) * kTerrainDimension);

	VoxelMap *voxelMap = new VoxelMap(size, origin, blockBounds, voxelBounds);

	int32 rowSize = voxelMap->GetRowSize();
	int32 deckSize = voxelMap->GetDeckSize();
	Voxel *base = voxelMap->GetVoxelDataBase();

	for (machine k = zmin; k <= zmax; k++)
	{
		for (machine j = ymin; j <= ymax; j++)
		{
			for (machine i = xmin; i <= xmax; i++)
			{
				Voxel *voxel = base + ((k - zmin) * deckSize + (j - ymin) * rowSize + (i - xmin)) * kTerrainDimension;
				LoadVoxels(subchannel, voxelData[0][subchannel], Integer3D(i, j, k), voxel, rowSize, deckSize);
			}
		}
	}

	return (voxelMap);
}

void TerrainBlock::CloseBuildVoxelMap(VoxelMap *voxelMap)
{
	delete voxelMap;
}

VoxelMap *TerrainBlock::OpenVoxelMap(int32 subchannel, const Integer3D& vmin, const Integer3D& vmax) const
{
	VoxelBox	blockBounds, voxelBounds;

	int32 xmin = MaxZero(Min((vmin.x >> kTerrainLogDimension) - 1, blockSize.x - 1));
	int32 xmax = MaxZero(Min((vmax.x >> kTerrainLogDimension) + 1, blockSize.x - 1));
	int32 ymin = MaxZero(Min((vmin.y >> kTerrainLogDimension) - 1, blockSize.y - 1));
	int32 ymax = MaxZero(Min((vmax.y >> kTerrainLogDimension) + 1, blockSize.y - 1));
	int32 zmin = MaxZero(Min((vmin.z >> kTerrainLogDimension) - 1, blockSize.z - 1));
	int32 zmax = MaxZero(Min((vmax.z >> kTerrainLogDimension) + 1, blockSize.z - 1));

	blockBounds.min.Set(xmin, ymin, zmin);
	blockBounds.max.Set(xmax, ymax, zmax);

	voxelBounds.min.Set(Max(vmin.x, xmin << kTerrainLogDimension), Max(vmin.y, ymin << kTerrainLogDimension), Max(vmin.z, zmin << kTerrainLogDimension));
	voxelBounds.max.Set(Min(vmax.x, (xmax << kTerrainLogDimension) + (kTerrainDimension - 1)), Min(vmax.y, (ymax << kTerrainLogDimension) + (kTerrainDimension - 1)), Min(vmax.z, (zmax << kTerrainLogDimension) + (kTerrainDimension - 1)));

	Integer3D size((xmax - xmin + 1) * kTerrainDimension, (ymax - ymin + 1) * kTerrainDimension, (zmax - zmin + 1) * kTerrainDimension);
	Integer3D origin(-(xmin << kTerrainLogDimension), -(ymin << kTerrainLogDimension), -(zmin << kTerrainLogDimension));

	VoxelMap *voxelMap = new VoxelMap(size, origin, blockBounds, voxelBounds);

	int32 rowSize = voxelMap->GetRowSize();
	int32 deckSize = voxelMap->GetDeckSize();
	Voxel *base = voxelMap->GetVoxelDataBase();

	for (machine k = zmin; k <= zmax; k++)
	{
		for (machine j = ymin; j <= ymax; j++)
		{
			for (machine i = xmin; i <= xmax; i++)
			{
				Voxel *voxel = base + ((k - zmin) * deckSize + (j - ymin) * rowSize + (i - xmin)) * kTerrainDimension;
				LoadVoxels(subchannel, voxelData[0][subchannel], Integer3D(i, j, k), voxel, rowSize, deckSize);
			}
		}
	}

	return (voxelMap);
}

void TerrainBlock::CloseVoxelMap(int32 subchannel, VoxelMap *voxelMap)
{
	const VoxelBox& blockBounds = voxelMap->GetBlockBounds();
	int32 xmin = blockBounds.min.x;
	int32 ymin = blockBounds.min.y;
	int32 zmin = blockBounds.min.z;
	int32 xmax = blockBounds.max.x;
	int32 ymax = blockBounds.max.y;
	int32 zmax = blockBounds.max.z;

	int32 rowSize = voxelMap->GetRowSize();
	int32 deckSize = voxelMap->GetDeckSize();
	const VoxelBox& voxelBounds = voxelMap->GetVoxelBounds();
	const Voxel *base = voxelMap->GetVoxelDataBase();

	for (machine k = zmin; k <= zmax; k++)
	{
		int32 z = k << kTerrainLogDimension;
		if ((z <= voxelBounds.max.z + 1) && (z + (kTerrainDimension - 1) >= voxelBounds.min.z - 1))
		{
			for (machine j = ymin; j <= ymax; j++)
			{
				int32 y = j << kTerrainLogDimension;
				if ((y <= voxelBounds.max.y + 1) && (y + (kTerrainDimension - 1) >= voxelBounds.min.y - 1))
				{
					for (machine i = xmin; i <= xmax; i++)
					{
						int32 x = i << kTerrainLogDimension;
						if ((x <= voxelBounds.max.x + 1) && (x + (kTerrainDimension - 1) >= voxelBounds.min.x - 1))
						{
							const Voxel *voxel = base + ((k - zmin) * deckSize + (j - ymin) * rowSize + (i - xmin)) * kTerrainDimension;
							StoreVoxels(subchannel, Integer3D(i, j, k), voxel, rowSize, deckSize);
						}
					}
				}
			}
		}
	}

	delete voxelMap;
}

char *TerrainBlock::SaveVoxelData(int32 subchannel, const Integer3D& coord) const
{
	int32 index = (coord.z * blockSize.y + coord.y) * blockSize.x + coord.x;
	char *data = voxelData[0][subchannel][index];

	if ((data) && (GetPointerAddress(data) != 1))
	{
		unsigned_int32 size = Heap::GetMemBlockSize(data);
		char *copy = terrainHeap.New<char>(size);
		MemoryMgr::CopyMemory(data, copy, size);
		data = copy;
	}

	return (data);
}

void TerrainBlock::RestoreVoxelData(int32 subchannel, const Integer3D& coord, char *data)
{
	char **subchannelData = voxelData[0][subchannel];
	int32 index = (coord.z * blockSize.y + coord.y) * blockSize.x + coord.x;
	DeleteVoxelData(subchannelData[index]);
	subchannelData[index] = data;
}

void TerrainBlock::SetBlockToHorizontalPlane(int32 planeZ)
{
	ClearBlock();

	char **subchannelData = voxelData[0][kTerrainSubchannelDensity];
	int32 solidCount = planeZ / kTerrainDimension * blockSize.x * blockSize.y;
	for (machine a = 0; a < solidCount; a++)
	{
		subchannelData[a] = reinterpret_cast<char *>(1);
	}

	int32 h = planeZ & (kTerrainDimension - 1);
	if (h != 0)
	{
		int32 negativeCount = h * (kTerrainDimension * kTerrainDimension);
		int32 positiveCount = kTerrainDimension * kTerrainDimension * kTerrainDimension - negativeCount;

		int32 planeCount = blockSize.x * blockSize.y + solidCount;
		for (machine a = solidCount; a < planeCount; a++)
		{
			char *data = terrainHeap.New<char>(8);
			subchannelData[a] = data;

			data[0] = (char) ((negativeCount >> 8) | 0x80);
			data[1] = (char) negativeCount;
			data[2] = -0x7F;
			data[3] = (char) ((positiveCount >> 8) | 0x80);
			data[4] = (char) positiveCount;
			data[5] = 0x7F;
			data[6] = 0;
			data[7] = 0;
		}
	}
}

void TerrainBlock::SetBlockToHeightField(const unsigned_int8 *heightField, int32 heightScale, int32 heightOffset)
{
	ClearBlock();

	Voxel *voxel = new Voxel[kTerrainDimension * kTerrainDimension * kTerrainDimension];

	for (machine k = 0; k < blockSize.z; k++)
	{
		Fixed heightBase = ((k * kTerrainDimension) << 8) - heightOffset;

		for (machine j = 0; j < blockSize.y; j++)
		{
			int32 dj = (j < blockSize.y - 1);
			for (machine i = 0; i < blockSize.x; i++)
			{
				int32 di = (i < blockSize.x - 1);
				const unsigned_int8 *height = heightField + ((j * blockSize.x * kTerrainDimension + i) * kTerrainDimension);

				for (machine y = 0; y < kTerrainDimension; y++)
				{
					int32 y1 = Max(y - 1, -j);
					int32 y2 = y + dj;

					for (machine x = 0; x < kTerrainDimension; x++)
					{
						int32 x1 = Max(x - 1, -i);
						int32 x2 = x + di;

						float dx = (float) (height[y * blockSize.x * kTerrainDimension + x2] - height[y * blockSize.x * kTerrainDimension + x1]);
						float dy = (float) (height[y2 * blockSize.x * kTerrainDimension + x] - height[y1 * blockSize.x * kTerrainDimension + x]);

						Fixed f = (Fixed) (InverseSqrt(dx * dx + dy * dy + 1.0F) * 256.0F + 0.5F);
						f = Min(Max(f, 0x0020), 0x0100);

						Fixed h = height[y * blockSize.x * kTerrainDimension + x] * heightScale - heightBase;

						Voxel *column = voxel + (y * kTerrainDimension + x);
						for (machine z = 0; z < kTerrainDimension; z++)
						{
							int32 v = Min(Max((((z << 8) - h) * f) >> 9, -0x7F), 0x7F);
							column[z * (kTerrainDimension * kTerrainDimension)] = (Voxel) v;
						}
					}
				}

				StoreVoxels(kTerrainSubchannelDensity, Integer3D(i, j, k), voxel, kTerrainDimension, kTerrainDimension * kTerrainDimension);
			}
		}
	}

	delete[] voxel;
}

void TerrainBlock::SetBlockToHeightField(const unsigned_int16 *heightField, int32 heightScale, int32 heightOffset)
{
	ClearBlock();

	Voxel *voxel = new Voxel[kTerrainDimension * kTerrainDimension * kTerrainDimension];

	for (machine k = 0; k < blockSize.z; k++)
	{
		Fixed heightBase = ((k * kTerrainDimension) << 16) - heightOffset;

		for (machine j = 0; j < blockSize.y; j++)
		{
			int32 dj = (j < blockSize.y - 1);
			for (machine i = 0; i < blockSize.x; i++)
			{
				int32 di = (i < blockSize.x - 1);
				const unsigned_int16 *height = heightField + ((j * blockSize.x * kTerrainDimension + i) * kTerrainDimension);

				for (machine y = 0; y < kTerrainDimension; y++)
				{
					int32 y1 = Max(y - 1, -j);
					int32 y2 = y + dj;

					for (machine x = 0; x < kTerrainDimension; x++)
					{
						int32 x1 = Max(x - 1, -i);
						int32 x2 = x + di;

						float dx = (float) (height[y * blockSize.x * kTerrainDimension + x2] - height[y * blockSize.x * kTerrainDimension + x1]);
						float dy = (float) (height[y2 * blockSize.x * kTerrainDimension + x] - height[y1 * blockSize.x * kTerrainDimension + x]);

						Fixed f = (Fixed) (InverseSqrt(dx * dx + dy * dy + 1.0F) * 256.0F + 0.5F);
						f = Min(Max(f, 0x0020), 0x0100);

						Fixed h = height[y * blockSize.x * kTerrainDimension + x] * heightScale - heightBase;

						Voxel *column = voxel + (y * kTerrainDimension + x);
						for (machine z = 0; z < kTerrainDimension; z++)
						{
							int32 v = Min(Max(((((z << 16) - h) >> 8) * f) >> 9, -0x7F), 0x7F);
							column[z * (kTerrainDimension * kTerrainDimension)] = (Voxel) v;
						}
					}
				}

				StoreVoxels(kTerrainSubchannelDensity, Integer3D(i, j, k), voxel, kTerrainDimension, kTerrainDimension * kTerrainDimension);
			}
		}
	}

	delete[] voxel;
}

TerrainGeometry *TerrainBlock::FindTerrainGeometry(const Node *root, const Integer3D& coord, int32 level)
{
	Node *node = root->GetFirstSubnode();
	while (node)
	{
		if (node->GetNodeType() == kNodeGeometry)
		{
			Geometry *geometry = static_cast<Geometry *>(node);
			if (geometry->GetGeometryType() == kGeometryTerrain)
			{
				TerrainGeometry *terrain = static_cast<TerrainGeometry *>(geometry);
				const TerrainGeometryObject *object = terrain->GetObject();

				int32 d = object->GetDetailLevel();
				unsigned_int32 mask = ~((1 << d) - 1);
				const Integer3D& c = object->GetGeometryCoord();

				if (((coord.x & mask) == c.x) && ((coord.y & mask) == c.y) && ((coord.z & mask) == c.z))
				{
					if (d == level)
					{
						return (terrain);
					}

					return (FindTerrainGeometry(terrain, coord, level));
				}
			}
		}

		node = node->Next();
	}

	return (nullptr);
}

Node *TerrainBlock::FindTerrainSuperNode(Node *root, const Integer3D& coord, int32 level)
{
	Node *node = root->GetFirstSubnode();
	while (node)
	{
		if (node->GetNodeType() == kNodeGeometry)
		{
			Geometry *geometry = static_cast<Geometry *>(node);
			if (geometry->GetGeometryType() == kGeometryTerrain)
			{
				TerrainGeometry *terrain = static_cast<TerrainGeometry *>(geometry);
				const TerrainGeometryObject *object = terrain->GetObject();

				int32 d = object->GetDetailLevel();
				if (d > level)
				{
					unsigned_int32 mask = ~((1 << d) - 1);
					const Integer3D& c = object->GetGeometryCoord();

					if (((coord.x & mask) == c.x) && ((coord.y & mask) == c.y) && ((coord.z & mask) == c.z))
					{
						if (d == level + 1)
						{
							return (terrain);
						}

						return (FindTerrainSuperNode(terrain, coord, level));
					}
				}
			}
		}

		node = node->Next();
	}

	return (root);
}

void TerrainBlock::AddTerrainGeometry(TerrainGeometry *terrainGeometry)
{
	const TerrainGeometryObject *terrainGeometryObject = terrainGeometry->GetObject();
	const Integer3D& coord = terrainGeometryObject->GetGeometryCoord();
	int32 level = terrainGeometryObject->GetDetailLevel();

	Node *super = FindTerrainSuperNode(this, coord, level);
	unsigned_int32 mask = ~((1 << level) - 1);

	Node *node = super->GetFirstSubnode();
	while (node)
	{
		Node *next = node->Next();

		if (node->GetNodeType() == kNodeGeometry)
		{
			Geometry *geometry = static_cast<Geometry *>(node);
			if (geometry->GetGeometryType() == kGeometryTerrain)
			{
				TerrainGeometry *terrain = static_cast<TerrainGeometry *>(geometry);
				const TerrainGeometryObject *object = terrain->GetObject();

				const Integer3D& c = object->GetGeometryCoord();
				if (((c.x & mask) == coord.x) && ((c.y & mask) == coord.y) && ((c.z & mask) == coord.z))
				{
					if (object->GetDetailLevel() < level)
					{
						terrainGeometry->AppendSubnode(terrain);
					}
				}
			}
		}

		node = next;
	}

	super->AppendSubnode(terrainGeometry);
}

void TerrainBlock::ProcessStructure(void)
{
	Node *node = GetFirstSubnode();
	while (node)
	{
		if (node->GetNodeType() == kNodeGeometry)
		{
			Geometry *geometry = static_cast<Geometry *>(node);
			if (geometry->GetGeometryType() == kGeometryTerrain)
			{
				TerrainGeometry *terrain = static_cast<TerrainGeometry *>(geometry);
				const TerrainGeometryObject *object = terrain->GetObject();

				int32 level = object->GetDetailLevel();
				if (level > 0)
				{
					static_cast<TerrainLevelGeometry *>(terrain)->ProcessStructure();
					if (level == 1)
					{
						node = GetNextLevelNode(node);
						continue;
					}
				}
			}
		}

		node = GetNextNode(node);
	}
}

int32 TerrainBlock::GetTerrainMaterialIndex(const TerrainMaterial *material)
{
	int32 freeSlot = -1;
	for (machine a = 0; a < kMaxTerrainMaterialCount; a++)
	{
		if (materialUsage[a])
		{
			const TerrainMaterial *m = &terrainMaterial[a];
			if ((m->primaryMaterial == material->primaryMaterial) && (m->secondaryMaterial == material->secondaryMaterial))
			{
				return (a);
			}
		}
		else if (freeSlot < 0)
		{
			freeSlot = a;
		}
	}

	if (freeSlot >= 0)
	{
		TerrainMaterial *m = &terrainMaterial[freeSlot];
		m->primaryMaterial = material->primaryMaterial;
		m->secondaryMaterial = material->secondaryMaterial;

		materialUsage[freeSlot] = true;
		return (freeSlot);
	}

	return (-1);
}

void TerrainBlock::OptimizeTerrainMaterials(void)
{
	MemoryMgr::ClearMemory(materialUsage, kTerrainMaterialUsageTableSize);

	const char *const *subchannelData = voxelData[0][kTerrainSubchannelMaterial];

	int32 dataCount = blockSize.x * blockSize.y * blockSize.z;
	for (machine a = 0; a < dataCount; a++)
	{
		const char *data = subchannelData[a];
		if (data)
		{
			if (GetPointerAddress(data) != 1)
			{
				int32 i = 0;
				int32 j = 0;
				int32 k = 0;

				do
				{
					const unsigned_int8 *code = reinterpret_cast<const unsigned_int8 *>(data);
					unsigned_int16 d = (code[0] << 8) | code[1];
					int32 count = d & 0x7FFF;
					data += 2;

					i += count;
					j += i >> kTerrainLogDimension;
					k += j >> kTerrainLogDimension;
					i &= kTerrainDimension - 1;
					j &= kTerrainDimension - 1;

					if ((d & 0x8000) != 0)
					{
						UnsignedVoxel v = *reinterpret_cast<const UnsignedVoxel *>(data);
						materialUsage[v] = true;
						data++;
					}
					else
					{
						do
						{
							UnsignedVoxel v = *reinterpret_cast<const UnsignedVoxel *>(data);
							materialUsage[v] = true;
							data++;

						} while (--count > 0);
					}
				} while (k < kTerrainDimension);
			}
			else
			{
				materialUsage[solidSubchannelValue[kTerrainSubchannelMaterial]] = true;
			}
		}
		else
		{
			materialUsage[emptySubchannelValue[kTerrainSubchannelMaterial]] = true;
		}
	}
}


TerrainGeometryObject::TerrainGeometryObject() : GeometryObject(kGeometryTerrain)
{
	detailLevel = 0;

	SetStaticSurfaceData(1, staticSurfaceData);
}

TerrainGeometryObject::TerrainGeometryObject(const Integer3D& coord) : GeometryObject(kGeometryTerrain)
{
	geometryCoord = coord;
	detailLevel = 0;

	SetGeometryFlags(GetGeometryFlags() | kGeometryMarkingFullPolygon);

	SetStaticSurfaceData(1, staticSurfaceData, true);
	staticSurfaceData[0].textureAlignData[0].alignMode = kTextureAlignObjectPlane;
	staticSurfaceData[0].textureAlignData[0].alignPlane.Set(1.0F, 0.0F, 0.0F, 0.0F);
	staticSurfaceData[0].textureAlignData[1].alignMode = kTextureAlignObjectPlane;
	staticSurfaceData[0].textureAlignData[1].alignPlane.Set(0.0F, 1.0F, 0.0F, 0.0F);
}

TerrainGeometryObject::TerrainGeometryObject(const Integer3D& coord, int32 level) : GeometryObject(kGeometryTerrain)
{
	geometryCoord = coord;
	detailLevel = level;

	SetGeometryFlags(GetGeometryFlags() | (kGeometryMarkingFullPolygon | kGeometryMarkingInhibit));
	SetCollisionExclusionMask(kCollisionExcludeAll);

	SetStaticSurfaceData(1, staticSurfaceData, true);
	staticSurfaceData[0].textureAlignData[0].alignMode = kTextureAlignObjectPlane;
	staticSurfaceData[0].textureAlignData[0].alignPlane.Set(1.0F, 0.0F, 0.0F, 0.0F);
	staticSurfaceData[0].textureAlignData[1].alignMode = kTextureAlignObjectPlane;
	staticSurfaceData[0].textureAlignData[1].alignPlane.Set(0.0F, 1.0F, 0.0F, 0.0F);
}

TerrainGeometryObject::~TerrainGeometryObject()
{
}

void TerrainGeometryObject::PackType(Packer& data) const
{
	GeometryObject::PackType(data);
	data << int32(0);
}

void TerrainGeometryObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	GeometryObject::Pack(data, packFlags);

	data << ChunkHeader('GCRD', sizeof(Integer3D));
	data << geometryCoord;

	data << ChunkHeader('LEVL', 4);
	data << detailLevel;

	data << ChunkHeader('BBOX', sizeof(Box3D));
	data << boundingBox;

	data << ChunkHeader('TEXP', sizeof(Vector4D));
	data << texcoordParameter;

	data << TerminatorChunk;
}

void TerrainGeometryObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	GeometryObject::Unpack(data, unpackFlags);
	UnpackChunkList<TerrainGeometryObject>(data, unpackFlags);
}

bool TerrainGeometryObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'GCRD':

			data >> geometryCoord;
			return (true);

		case 'LEVL':

			data >> detailLevel;
			return (true);

		case 'BBOX':

			data >> boundingBox;
			return (true);

		case 'TEXP':

			data >> texcoordParameter;
			return (true);
	}

	return (false);
}

bool TerrainGeometryObject::ExteriorSphere(const Point3D& center, float radius) const
{
	return (boundingBox.ExteriorSphere(center, radius));
}

bool TerrainGeometryObject::ExteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const
{
	return (boundingBox.ExteriorSweptSphere(p1, p2, radius));
}

unsigned_int16 TerrainGeometryObject::AddSharedVertex(const BuildVertex *sharedVertex, UnsignedVoxel material, BuildVertex *vertexStorage, int32& globalVertexCount)
{
	for (;;)
	{
		unsigned_int16 index = sharedVertex->nextIndex;
		if (index == 0)
		{
			index = (unsigned_int16) globalVertexCount++;
			sharedVertex->nextIndex = index;

			BuildVertex *buildVertex = &vertexStorage[index];
			buildVertex->position0 = sharedVertex->position0;
			buildVertex->normal = sharedVertex->normal;
			buildVertex->blend = sharedVertex->blend;
			buildVertex->material = material;
			buildVertex->nextIndex = 0;

			return (index);
		}

		sharedVertex = &vertexStorage[index];
		if (sharedVertex->material == material)
		{
			return (index);
		}
	}
}

void TerrainGeometryObject::Build(Geometry *geometry)
{
	using namespace Transvoxel;

	ArrayDescriptor		desc[5];

	BuildStorage *buildStorage = new BuildStorage;

	const TerrainBlock *block = static_cast<TerrainGeometry *>(geometry)->GetBlockNode();
	VoxelMap *densityMap = block->OpenBuildVoxelMap(kTerrainSubchannelDensity, geometryCoord);
	VoxelMap *blendMap = block->OpenBuildVoxelMap(kTerrainSubchannelBlend, geometryCoord);
	VoxelMap *materialMap = block->OpenBuildVoxelMap(kTerrainSubchannelMaterial, geometryCoord);

	int32 globalVertexCount = 0;
	int32 globalTriangleCount = 0;

	unsigned_int8 deckParity = 0;
	unsigned_int16 deltaMask = 0;

	for (machine k = 0; k < kTerrainDimension; k++)
	{
		DeckStorage& currentDeck = buildStorage->deckStorage[deckParity];
		DeckStorage& previousDeck = buildStorage->deckStorage[deckParity ^ 1];

		for (machine j = 0; j < kTerrainDimension; j++)
		{
			for (machine i = 0; i < kTerrainDimension; i++)
			{
				Voxel	density[8];

				Voxel mainDensity = densityMap->GetVoxel(i + 1, j + 1, k + 1);
				UnsignedVoxel mainBlend = blendMap->GetUnsignedVoxel(i + 1, j + 1, k + 1);
				UnsignedVoxel material = materialMap->GetUnsignedVoxel(i, j, k);

				if (mainDensity == 0)
				{
					unsigned_int16 index = (unsigned_int16) globalVertexCount++;
					currentDeck[j][i][0] = index;

					BuildVertex *buildVertex = &buildStorage->vertexStorage[index];
					buildVertex->position0.Set((i + 1) << kVoxelFractionSize, (j + 1) << kVoxelFractionSize, (k + 1) << kVoxelFractionSize);
					buildVertex->normal = densityMap->CalculateNormal(Integer3D(i + 1, j + 1, k + 1));
					buildVertex->blend = mainBlend;
					buildVertex->material = material;
					buildVertex->nextIndex = 0;
				}

				density[0] = densityMap->GetVoxel(i, j, k);
				density[1] = densityMap->GetVoxel(i + 1, j, k);
				density[2] = densityMap->GetVoxel(i, j + 1, k);
				density[3] = densityMap->GetVoxel(i + 1, j + 1, k);
				density[4] = densityMap->GetVoxel(i, j, k + 1);
				density[5] = densityMap->GetVoxel(i + 1, j, k + 1);
				density[6] = densityMap->GetVoxel(i, j + 1, k + 1);
				density[7] = mainDensity;

				unsigned_int32 code = ((density[0] >> 7) & 0x01) | ((density[1] >> 6) & 0x02) | ((density[2] >> 5) & 0x04) | ((density[3] >> 4) & 0x08)
								   | ((density[4] >> 3) & 0x10) | ((density[5] >> 2) & 0x20) | ((density[6] >> 1) & 0x40) | (mainDensity & 0x80);

				if ((code ^ ((mainDensity >> 7) & 0xFF)) != 0)
				{
					UnsignedVoxel	blend[8];
					unsigned_int16	globalVertexIndex[12];

					blend[0] = blendMap->GetUnsignedVoxel(i, j, k);
					blend[1] = blendMap->GetUnsignedVoxel(i + 1, j, k);
					blend[2] = blendMap->GetUnsignedVoxel(i, j + 1, k);
					blend[3] = blendMap->GetUnsignedVoxel(i + 1, j + 1, k);
					blend[4] = blendMap->GetUnsignedVoxel(i, j, k + 1);
					blend[5] = blendMap->GetUnsignedVoxel(i + 1, j, k + 1);
					blend[6] = blendMap->GetUnsignedVoxel(i, j + 1, k + 1);
					blend[7] = mainBlend;

					const RegularCellData *cellData = &regularCellData[0][regularCellClass[code]];

					int32 triangleCount = cellData->GetTriangleCount();
					if (globalTriangleCount + triangleCount > kMaxTerrainTriangleCount)
					{
						goto end;
					}

					int32 vertexCount = cellData->GetVertexCount();
					const unsigned_int16 *vertexArray = regularVertexData[code];

					for (machine a = 0; a < vertexCount; a++)
					{
						unsigned_int16	index;

						unsigned_int16 vertexData = vertexArray[a];
						unsigned_int16 v0 = (vertexData >> 4) & 0x0F;
						unsigned_int16 v1 = vertexData & 0x0F;

						int32 d0 = density[v0];
						int32 d1 = density[v1];
						Fixed t = (d1 << kVoxelFractionSize) / (d1 - d0);

						Integer3D coord0(i + (v0 & 1), j + ((v0 >> 1) & 1), k + ((v0 >> 2) & 1));
						Integer3D coord1(i + (v1 & 1), j + ((v1 >> 1) & 1), k + ((v1 >> 2) & 1));

						if ((t & (kVoxelFixedUnit - 1)) != 0)
						{
							unsigned_int16 edgeIndex = (vertexData >> 8) & 0x0F;
							unsigned_int16 deltaCode = (vertexData >> 12) & 0x0F;

							if ((deltaCode & deltaMask) == deltaCode)
							{
								if (deltaCode & 4)
								{
									index = previousDeck[j - ((deltaCode >> 1) & 1)][i - (deltaCode & 1)][edgeIndex];
								}
								else
								{
									index = currentDeck[j - (deltaCode >> 1)][i - (deltaCode & 1)][edgeIndex];
								}
							}
							else
							{
								index = (unsigned_int16) globalVertexCount++;
								BuildVertex *buildVertex = &buildStorage->vertexStorage[index];

								Fixed u = kVoxelFixedUnit - t;
								buildVertex->position0 = coord0 * t + coord1 * u;
								buildVertex->normal = densityMap->CalculateNormal(coord0, coord1, t, u);
								buildVertex->blend = (UnsignedVoxel) ((blend[v0] * t + blend[v1] * u) >> kVoxelFractionSize);
								buildVertex->material = material;
								buildVertex->nextIndex = 0;

								if (v1 == 7)
								{
									currentDeck[j][i][edgeIndex] = index;
								}

								globalVertexIndex[a] = index;
								continue;
							}
						}
						else if (t == 0)
						{
							if (v1 == 7)
							{
								index = currentDeck[j][i][0];
							}
							else
							{
								unsigned_int16 deltaCode = v1 ^ 7;
								if ((deltaCode & deltaMask) == deltaCode)
								{
									if (deltaCode & 4)
									{
										index = previousDeck[j - ((deltaCode >> 1) & 1)][i - (deltaCode & 1)][0];
									}
									else
									{
										index = currentDeck[j - (deltaCode >> 1)][i - (deltaCode & 1)][0];
									}
								}
								else
								{
									index = (unsigned_int16) globalVertexCount++;
									BuildVertex *buildVertex = &buildStorage->vertexStorage[index];

									buildVertex->position0 = coord1 << kVoxelFractionSize;
									buildVertex->normal = densityMap->CalculateNormal(coord1);
									buildVertex->blend = blend[v1];
									buildVertex->material = material;
									buildVertex->nextIndex = 0;

									globalVertexIndex[a] = index;
									continue;
								}
							}
						}
						else
						{
							unsigned_int16 deltaCode = v0 ^ 7;
							if ((deltaCode & deltaMask) == deltaCode)
							{
								if (deltaCode & 4)
								{
									index = previousDeck[j - ((deltaCode >> 1) & 1)][i - (deltaCode & 1)][0];
								}
								else
								{
									index = currentDeck[j - (deltaCode >> 1)][i - (deltaCode & 1)][0];
								}
							}
							else
							{
								index = (unsigned_int16) globalVertexCount++;
								BuildVertex *buildVertex = &buildStorage->vertexStorage[index];

								buildVertex->position0 = coord0 << kVoxelFractionSize;
								buildVertex->normal = densityMap->CalculateNormal(coord0);
								buildVertex->blend = blend[v0];
								buildVertex->material = material;
								buildVertex->nextIndex = 0;

								globalVertexIndex[a] = index;
								continue;
							}
						}

						const BuildVertex *sharedVertex = &buildStorage->vertexStorage[index];
						if (sharedVertex->material != material)
						{
							index = AddSharedVertex(sharedVertex, material, buildStorage->vertexStorage, globalVertexCount);
						}

						globalVertexIndex[a] = index;
					}

					if (material != kDeadTerrainMaterialIndex)
					{
						const unsigned_int8 *localVertexIndex = cellData->vertexIndex;
						BuildTriangle *restrict buildTriangle = &buildStorage->triangleStorage[globalTriangleCount];

						for (machine a = 0; a < triangleCount; a++)
						{
							buildTriangle->triangle.index[0] = globalVertexIndex[localVertexIndex[0]];
							buildTriangle->triangle.index[1] = globalVertexIndex[localVertexIndex[1]];
							buildTriangle->triangle.index[2] = globalVertexIndex[localVertexIndex[2]];

							localVertexIndex += 3;
							buildTriangle++;
						}

						globalTriangleCount += triangleCount;
					}
				}

				deltaMask |= 1;
			}

			deltaMask = (deltaMask | 2) & 6;
		}

		deltaMask = 4;
		deckParity ^= 1;
	}

	end:
	TerrainBlock::CloseBuildVoxelMap(materialMap);
	TerrainBlock::CloseBuildVoxelMap(blendMap);
	TerrainBlock::CloseBuildVoxelMap(densityMap);

	SetGeometryLevelCount(1);

	int32 finalVertexCount = 0;
	for (machine a = 0; a < globalVertexCount; a++)
	{
		BuildVertex *buildVertex = &buildStorage->vertexStorage[a];
		if (buildVertex->material != kDeadTerrainMaterialIndex)
		{
			const Fixed3D& vertex = buildVertex->position0;
			if (((vertex.x | vertex.y | vertex.z) >= 0) && (Max(Max(vertex.x, vertex.y), vertex.z) <= (kTerrainDimension << kVoxelFractionSize)))
			{
				unsigned_int32 status = 1;

				status |= (vertex.x >> (kTerrainLogDimension + kVoxelFractionSize - 1)) & 2;
				status |= (vertex.y >> (kTerrainLogDimension + kVoxelFractionSize - 2)) & 4;
				status |= (vertex.z >> (kTerrainLogDimension + kVoxelFractionSize - 3)) & 8;

				buildVertex->statusFlags = (unsigned_int8) status;
				buildVertex->remapIndex = (unsigned_int16) finalVertexCount;
				finalVertexCount++;
				continue;
			}
		}

		buildVertex->statusFlags = 0;
	}

	int32 finalTriangleCount = 0;
	for (machine a = 0; a < globalTriangleCount; a++)
	{
		BuildTriangle *buildTriangle = &buildStorage->triangleStorage[a];
		int32 i1 = buildTriangle->triangle.index[0];
		int32 i2 = buildTriangle->triangle.index[1];
		int32 i3 = buildTriangle->triangle.index[2];

		const BuildVertex *bv1 = &buildStorage->vertexStorage[i1];
		const BuildVertex *bv2 = &buildStorage->vertexStorage[i2];
		const BuildVertex *bv3 = &buildStorage->vertexStorage[i3];

		Fixed3D nrml = (bv2->position0 - bv1->position0) % (bv3->position0 - bv1->position0);
		unsigned_int32 statusMask = ((nrml.x >> 31) & 2) | ((nrml.y >> 31) & 4) | ((nrml.z >> 31) & 8) | 1;
		bool include = (((nrml.x | nrml.y | nrml.z) != 0) && ((bv1->statusFlags & bv2->statusFlags & bv3->statusFlags & statusMask) == 1));

		buildTriangle->inclusionFlag = include;
		finalTriangleCount += include;
	}

	desc[0].identifier = kArrayPosition;
	desc[0].elementCount = finalVertexCount;
	desc[0].elementSize = sizeof(Point3D);
	desc[0].componentCount = 3;

	desc[1].identifier = kArrayNormal;
	desc[1].elementCount = finalVertexCount;
	desc[1].elementSize = sizeof(Vector3D);
	desc[1].componentCount = 3;

	desc[2].identifier = kArrayColor0;
	desc[2].elementCount = finalVertexCount;
	desc[2].elementSize = 4;
	desc[2].componentCount = 1;

	desc[3].identifier = kArrayColor1;
	desc[3].elementCount = finalVertexCount;
	desc[3].elementSize = 4;
	desc[3].componentCount = 1;

	desc[4].identifier = kArrayPrimitive;
	desc[4].elementCount = finalTriangleCount;
	desc[4].elementSize = sizeof(Triangle);
	desc[4].componentCount = 1;

	Mesh *mesh = GetGeometryLevel(0);
	mesh->AllocateStorage(finalVertexCount, 5, desc);

	Fixed gx = geometryCoord.x << (kTerrainLogDimension + kVoxelFractionSize);
	Fixed gy = geometryCoord.y << (kTerrainLogDimension + kVoxelFractionSize);
	Fixed gz = geometryCoord.z << (kTerrainLogDimension + kVoxelFractionSize);
	float scale = block->GetObject()->GetVoxelScale() * (1.0F / (float) kVoxelFixedUnit);

	Point3D *restrict position = mesh->GetArray<Point3D>(kArrayPosition);
	Vector3D *restrict normal = mesh->GetArray<Vector3D>(kArrayNormal);
	Color4C *restrict color0 = mesh->GetArray<Color4C>(kArrayColor0);
	Color4C *restrict color1 = mesh->GetArray<Color4C>(kArrayColor1);

	for (machine a = 0; a < globalVertexCount; a++)
	{
		const BuildVertex *buildVertex = &buildStorage->vertexStorage[a];
		if (buildVertex->statusFlags & 1)
		{
			float px = (float) (buildVertex->position0.x + gx) * scale;
			float py = (float) (buildVertex->position0.y + gy) * scale;
			float pz = (float) (buildVertex->position0.z + gz) * scale;
			position->Set(px, py, pz);
			position++;

			*normal = buildVertex->normal;
			normal++;

			const TerrainMaterial *terrainMaterial = block->GetTerrainMaterial(buildVertex->material);
			color0->Set(terrainMaterial->primaryMaterial.GetBlue(), terrainMaterial->secondaryMaterial.GetBlue(), buildVertex->blend, 255);
			color1->Set(terrainMaterial->primaryMaterial.GetRed(), terrainMaterial->primaryMaterial.GetGreen(), terrainMaterial->secondaryMaterial.GetRed(), terrainMaterial->secondaryMaterial.GetGreen());
			color0++;
			color1++;
		}
	}

	if (finalVertexCount != 0)
	{
		boundingBox.Calculate(finalVertexCount, mesh->GetArray<Point3D>(kArrayPosition));

		Triangle *restrict triangle = mesh->GetArray<Triangle>(kArrayPrimitive);
		for (machine a = 0; a < globalTriangleCount; a++)
		{
			const BuildTriangle *buildTriangle = &buildStorage->triangleStorage[a];
			if (buildTriangle->inclusionFlag)
			{
				triangle->index[0] = buildStorage->vertexStorage[buildTriangle->triangle.index[0]].remapIndex;
				triangle->index[1] = buildStorage->vertexStorage[buildTriangle->triangle.index[1]].remapIndex;
				triangle->index[2] = buildStorage->vertexStorage[buildTriangle->triangle.index[2]].remapIndex;
				triangle++;
			}
		}

		BuildCollisionData();
	}

	Vector3D blockSize = block->GetBlockBoxSize();
	float bx = 1.0F / blockSize.x;
	float by = 1.0F / blockSize.y;

	texcoordParameter.Set(bx, by, 0.0F, 0.0F);
	staticSurfaceData[0].textureAlignData[0].alignPlane.Set(bx, 0.0F, 0.0F, 0.0F);
	staticSurfaceData[0].textureAlignData[1].alignPlane.Set(0.0F, by, 0.0F, 0.0F);

	delete buildStorage;
}


TerrainLevelGeometryObject::TerrainLevelGeometryObject()
{
	borderRenderData.multiRenderMask = 0;
}

TerrainLevelGeometryObject::TerrainLevelGeometryObject(const Integer3D& coord, int32 level) : TerrainGeometryObject(coord, level)
{
	borderRenderData.multiRenderMask = 0;
}

TerrainLevelGeometryObject::~TerrainLevelGeometryObject()
{
}

void TerrainLevelGeometryObject::PackType(Packer& data) const
{
	GeometryObject::PackType(data);
	data << int32(1);
}

void TerrainLevelGeometryObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	TerrainGeometryObject::Pack(data, packFlags);

	unsigned_int32 renderMask = borderRenderData.multiRenderMask;
	if (renderMask != 0)
	{
		data << ChunkHeader('MRND', 56);
		data << borderRenderData.baseTriangleCount;
		data << renderMask;

		data.WriteArray(12, borderRenderData.multiRenderData);
	}

	data << TerminatorChunk;
}

void TerrainLevelGeometryObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	TerrainGeometryObject::Unpack(data, unpackFlags);
	UnpackChunkList<TerrainLevelGeometryObject>(data, unpackFlags);
}

bool TerrainLevelGeometryObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'MRND':

			data >> borderRenderData.baseTriangleCount;
			data >> borderRenderData.multiRenderMask;

			data.ReadArray(12, borderRenderData.multiRenderData);
			return (true);
	}

	return (false);
}

void *TerrainLevelGeometryObject::BeginSettingsUnpack(void)
{
	borderRenderData.multiRenderMask = 0;
	return (TerrainGeometryObject::BeginSettingsUnpack());
}

void TerrainLevelGeometryObject::SaveBorderRenderData(TerrainBorderRenderData *data) const
{
	data->baseTriangleCount = borderRenderData.baseTriangleCount;
	data->multiRenderMask = borderRenderData.multiRenderMask;

	for (machine a = 0; a < 12; a++)
	{
		data->multiRenderData[a] = borderRenderData.multiRenderData[a];
	}
}

void TerrainLevelGeometryObject::RestoreBorderRenderData(const TerrainBorderRenderData *data)
{
	borderRenderData.baseTriangleCount = data->baseTriangleCount;
	borderRenderData.multiRenderMask = data->multiRenderMask;

	for (machine a = 0; a < 12; a++)
	{
		borderRenderData.multiRenderData[a] = data->multiRenderData[a];
	}
}

void TerrainLevelGeometryObject::OffsetBorderRenderData(int32 delta)
{
	borderRenderData.baseTriangleCount += delta;

	for (machine a = 0; a < 6; a++)
	{
		borderRenderData.multiRenderData[a * 2] += delta;
	}
}

void TerrainLevelGeometryObject::CalculateSecondaryPosition(int32 level, BuildVertex *buildVertex)
{
	Fixed3D		dp;

	unsigned_int32 xb = 0x80;
	unsigned_int32 yb = 0x80;
	unsigned_int32 zb = 0x80;

	Fixed minCell = kVoxelFixedUnit << level;
	Fixed maxCell = (kTerrainDimension - 1) << (kVoxelFractionSize + level);
	Fixed width = minCell >> 2;

	const Vector3D& normal = buildVertex->normal;

	Fixed x = buildVertex->position0.x;
	if (x < minCell)
	{
		if (x < width)
		{
			xb = 0x00;
		}

		Fixed dx = (width * (minCell - x)) >> (kVoxelFractionSize + level);
		Vector3D tangent(1.0F - normal.x * normal.x, -normal.x * normal.y, -normal.x * normal.z);
		dp.x = (Fixed) (tangent.x * 256.0F) * dx;
		dp.y = (Fixed) (tangent.y * 256.0F) * dx;
		dp.z = (Fixed) (tangent.z * 256.0F) * dx;
	}
	else if (x > maxCell)
	{
		if (x > maxCell + minCell - width)
		{
			xb = 0xFF;
		}

		Fixed dx = (width * (minCell * (kTerrainDimension - 1) - x)) >> (kVoxelFractionSize + level);
		Vector3D tangent(1.0F - normal.x * normal.x, -normal.x * normal.y, -normal.x * normal.z);
		dp.x = (Fixed) (tangent.x * 256.0F) * dx;
		dp.y = (Fixed) (tangent.y * 256.0F) * dx;
		dp.z = (Fixed) (tangent.z * 256.0F) * dx;
	}
	else
	{
		dp.Set(0, 0, 0);
	}

	Fixed y = buildVertex->position0.y;
	if (y < minCell)
	{
		if (y < width)
		{
			yb = 0x00;
		}

		Fixed dy = (width * (minCell - y)) >> (kVoxelFractionSize + level);
		Vector3D tangent(-normal.x * normal.y, 1.0F - normal.y * normal.y, -normal.y * normal.z);
		dp.x += (Fixed) (tangent.x * 256.0F) * dy;
		dp.y += (Fixed) (tangent.y * 256.0F) * dy;
		dp.z += (Fixed) (tangent.z * 256.0F) * dy;
	}
	else if (y > maxCell)
	{
		if (y > maxCell + minCell - width)
		{
			yb = 0xFF;
		}

		Fixed dy = (width * (minCell * (kTerrainDimension - 1) - y)) >> (kVoxelFractionSize + level);
		Vector3D tangent(-normal.x * normal.y, 1.0F - normal.y * normal.y, -normal.y * normal.z);
		dp.x += (Fixed) (tangent.x * 256.0F) * dy;
		dp.y += (Fixed) (tangent.y * 256.0F) * dy;
		dp.z += (Fixed) (tangent.z * 256.0F) * dy;
	}

	Fixed z = buildVertex->position0.z;
	if (z < minCell)
	{
		if (z < width)
		{
			zb = 0x00;
		}

		Fixed dz = (width * (minCell - z)) >> (kVoxelFractionSize + level);
		Vector3D tangent(-normal.x * normal.z, -normal.y * normal.z, 1.0F - normal.z * normal.z);
		dp.x += (Fixed) (tangent.x * 256.0F) * dz;
		dp.y += (Fixed) (tangent.y * 256.0F) * dz;
		dp.z += (Fixed) (tangent.z * 256.0F) * dz;
	}
	else if (z > maxCell)
	{
		if (z > maxCell + minCell - width)
		{
			zb = 0xFF;
		}

		Fixed dz = (width * (minCell * (kTerrainDimension - 1) - z)) >> (kVoxelFractionSize + level);
		Vector3D tangent(-normal.x * normal.z, -normal.y * normal.z, 1.0F - normal.z * normal.z);
		dp.x += (Fixed) (tangent.x * 256.0F) * dz;
		dp.y += (Fixed) (tangent.y * 256.0F) * dz;
		dp.z += (Fixed) (tangent.z * 256.0F) * dz;
	}

	buildVertex->position1 = buildVertex->position0 + (dp >> kVoxelFractionSize);
	buildVertex->border.Set(xb, yb, zb, 0);
}

void TerrainLevelGeometryObject::FindSurfaceCrossingEdge(const VoxelMap *densityMap, const VoxelMap *blendMap, int32 level, Integer3D& coord0, Integer3D& coord1, int32& d0, int32& d1, UnsignedVoxel& b0, UnsignedVoxel& b1)
{
	Integer3D coord2 = (coord0 + coord1) >> 1;

	int32 d2 = densityMap->GetVoxel(coord2.x, coord2.y, coord2.z);
	UnsignedVoxel b2 = blendMap->GetUnsignedVoxel(coord2.x, coord2.y, coord2.z);

	if ((d0 ^ d2) & 0x80)
	{
		coord1 = coord2;
		d1 = d2;
		b1 = b2;
	}
	else
	{
		coord0 = coord2;
		d0 = d2;
		b0 = b2;
	}

	if (level > 1)
	{
		FindSurfaceCrossingEdge(densityMap, blendMap, level - 1, coord0, coord1, d0, d1, b0, b1);
	}
}

bool TerrainLevelGeometryObject::NontrivialCell(const VoxelMap *densityMap, machine x, machine y, machine z, machine dv)
{
	Voxel d0 = densityMap->GetVoxel(x, y, z);
	Voxel d1 = densityMap->GetVoxel(x + dv, y, z);
	Voxel d2 = densityMap->GetVoxel(x, y + dv, z);
	Voxel d3 = densityMap->GetVoxel(x + dv, y + dv, z);
	Voxel d4 = densityMap->GetVoxel(x, y, z + dv);
	Voxel d5 = densityMap->GetVoxel(x + dv, y, z + dv);
	Voxel d6 = densityMap->GetVoxel(x, y + dv, z + dv);
	Voxel d7 = densityMap->GetVoxel(x + dv, y + dv, z + dv);

	unsigned_int32 code = ((d0 >> 7) & 0x01) | ((d1 >> 6) & 0x02) | ((d2 >> 5) & 0x04) | ((d3 >> 4) & 0x08)
					   | ((d4 >> 3) & 0x10) | ((d5 >> 2) & 0x20) | ((d6 >> 1) & 0x40) | (d7 & 0x80);

	return ((code ^ ((d7 >> 7) & 0xFF)) != 0);
}

bool TerrainLevelGeometryObject::FindBestMaterial(const VoxelMap *densityMap, const VoxelMap *materialMap, machine x, machine y, machine z, machine dv, UnsignedVoxel *material)
{
	UnsignedVoxel	cellMaterial[8];
	unsigned_int8	cellWeight[8];

	int32 materialCount = 0;
	dv >>= 1;

	if (dv == 1)
	{
		for (machine a = 0; a < 7; a++)
		{
			machine i = x + (a & 1);
			machine j = y + ((a >> 1) & 1);
			machine k = z + ((a >> 2) & 1);

			if (NontrivialCell(densityMap, i, j, k, 1))
			{
				UnsignedVoxel m = materialMap->GetUnsignedVoxel(i, j, k);
				for (machine b = 0; b < materialCount; b++)
				{
					if (cellMaterial[b] == m)
					{
						cellWeight[b]++;
						goto next1;
					}
				}

				cellMaterial[materialCount] = m;
				cellWeight[materialCount] = 1;
				materialCount++;
			}

			next1:;
		}
	}
	else
	{
		for (machine a = 0; a < 7; a++)
		{
			UnsignedVoxel	m;

			machine i = x + (a & 1) * dv;
			machine j = y + ((a >> 1) & 1) * dv;
			machine k = z + ((a >> 2) & 1) * dv;

			if (FindBestMaterial(densityMap, materialMap, i, j, k, dv, &m))
			{
				for (machine b = 0; b < materialCount; b++)
				{
					if (cellMaterial[b] == m)
					{
						cellWeight[b]++;
						goto next2;
					}
				}

				cellMaterial[materialCount] = m;
				cellWeight[materialCount] = 1;
				materialCount++;
			}

			next2:;
		}
	}

	if (materialCount != 0)
	{
		machine bestIndex = 0;
		unsigned_int32 bestWeight = cellWeight[0];

		for (machine a = 1; a < materialCount; a++)
		{
			unsigned_int32 weight = cellWeight[a];
			if (weight > bestWeight)
			{
				bestWeight = weight;
				bestIndex = a;
			}
		}

		*material = cellMaterial[bestIndex];
		return (true);
	}

	return (false);
}

int32 TerrainLevelGeometryObject::ChooseTriangulation(int32 cellClass, const VoxelMap *densityMap, machine x, machine y, machine z, machine dv, const unsigned_int16 *globalVertexIndex, const BuildVertex *buildVertex)
{
	using namespace Transvoxel;

	dv >>= 1;
	x += dv;
	y += dv;
	z += dv;

	float voxel = (float) densityMap->GetVoxel(x, y, z) * K::one_over_127;
	Fixed3D center(x << kVoxelFractionSize, y << kVoxelFractionSize, z << kVoxelFractionSize);

	const InternalEdgeData *edgeData = &regularInternalEdgeData[0][cellClass];
	int32 count = edgeData->edgeCount;

	float best1 = 2.0F;

	for (machine a = 0; a < count; a++)
	{
		const unsigned_int8 *edge = edgeData->vertexIndex[a];
		Fixed3D endpoint1 = buildVertex[globalVertexIndex[edge[0]]].position0 - center;
		Fixed3D endpoint2 = buildVertex[globalVertexIndex[edge[1]]].position0 - center;
		Fixed3D adjacent1 = buildVertex[globalVertexIndex[edge[2]]].position0 - center;
		Fixed3D adjacent2 = buildVertex[globalVertexIndex[edge[3]]].position0 - center;

		Point3D p1((float) endpoint1.x * K::one_over_256, (float) endpoint1.y * K::one_over_256, (float) endpoint1.z * K::one_over_256);
		Point3D p2((float) endpoint2.x * K::one_over_256, (float) endpoint2.y * K::one_over_256, (float) endpoint2.z * K::one_over_256);
		Point3D q1((float) adjacent1.x * K::one_over_256, (float) adjacent1.y * K::one_over_256, (float) adjacent1.z * K::one_over_256);
		Point3D q2((float) adjacent2.x * K::one_over_256, (float) adjacent2.y * K::one_over_256, (float) adjacent2.z * K::one_over_256);

		Vector3D v = p2 - p1;
		float p1_dot_v = p1 * v;
		float d = Sqrt(p1 * p1 - p1_dot_v * p1_dot_v / (v * v));
		if (v % (q2 - q1) * p1 > 0.0F)
		{
			d = -d;
		}

		d = Fabs(d - voxel);
		if (d < best1)
		{
			best1 = d;
		}
	}

	edgeData = &regularInternalEdgeData[1][cellClass];
	count = edgeData->edgeCount;

	float best2 = 2.0F;

	for (machine a = 0; a < count; a++)
	{
		const unsigned_int8 *edge = edgeData->vertexIndex[a];
		Fixed3D endpoint1 = buildVertex[globalVertexIndex[edge[0]]].position0 - center;
		Fixed3D endpoint2 = buildVertex[globalVertexIndex[edge[1]]].position0 - center;
		Fixed3D adjacent1 = buildVertex[globalVertexIndex[edge[2]]].position0 - center;
		Fixed3D adjacent2 = buildVertex[globalVertexIndex[edge[3]]].position0 - center;

		Point3D p1((float) endpoint1.x * K::one_over_256, (float) endpoint1.y * K::one_over_256, (float) endpoint1.z * K::one_over_256);
		Point3D p2((float) endpoint2.x * K::one_over_256, (float) endpoint2.y * K::one_over_256, (float) endpoint2.z * K::one_over_256);
		Point3D q1((float) adjacent1.x * K::one_over_256, (float) adjacent1.y * K::one_over_256, (float) adjacent1.z * K::one_over_256);
		Point3D q2((float) adjacent2.x * K::one_over_256, (float) adjacent2.y * K::one_over_256, (float) adjacent2.z * K::one_over_256);

		Vector3D v = p2 - p1;
		float p1_dot_v = p1 * v;
		float d = Sqrt(p1 * p1 - p1_dot_v * p1_dot_v / (v * v));
		if (v % (q2 - q1) * p1 > 0.0F)
		{
			d = -d;
		}

		d = Fabs(d - voxel);
		if (d < best2)
		{
			best2 = d;
		}
	}

	const RegularCellData *cellData = &regularCellData[0][cellClass];
	const unsigned_int8 *indexArray = cellData->vertexIndex;
	count = cellData->GetTriangleCount();

	for (machine a = 0; a < count; a++)
	{
		Fixed3D vertex1 = buildVertex[globalVertexIndex[indexArray[0]]].position0 - center;
		Fixed3D vertex2 = buildVertex[globalVertexIndex[indexArray[1]]].position0 - center;
		Fixed3D vertex3 = buildVertex[globalVertexIndex[indexArray[2]]].position0 - center;

		Point3D p1((float) vertex1.x * K::one_over_256, (float) vertex1.y * K::one_over_256, (float) vertex1.z * K::one_over_256);
		Point3D p2((float) vertex2.x * K::one_over_256, (float) vertex2.y * K::one_over_256, (float) vertex2.z * K::one_over_256);
		Point3D p3((float) vertex3.x * K::one_over_256, (float) vertex3.y * K::one_over_256, (float) vertex3.z * K::one_over_256);

		Vector3D v1 = p2 - p1;
		Vector3D v2 = p3 - p1;

		Vector3D normal = v1 % v2;
		Vector3D n1 = normal % v1;
		if (n1 * p1 < 0.0F)
		{
			Vector3D n2 = normal % (p3 - p2);
			if (n2 * p2 < 0.0F)
			{
				Vector3D n3 = v2 % normal;
				if (n3 * p3 < 0.0F)
				{
					float d = -(normal * p1);
					d = Fabs(d - voxel);
					if (d < best1)
					{
						best1 = d;
					}
				}
			}
		}

		indexArray += 3;
	}

	cellData = &regularCellData[1][cellClass];
	indexArray = cellData->vertexIndex;
	count = cellData->GetTriangleCount();

	for (machine a = 0; a < count; a++)
	{
		Fixed3D vertex1 = buildVertex[globalVertexIndex[indexArray[0]]].position0 - center;
		Fixed3D vertex2 = buildVertex[globalVertexIndex[indexArray[1]]].position0 - center;
		Fixed3D vertex3 = buildVertex[globalVertexIndex[indexArray[2]]].position0 - center;

		Point3D p1((float) vertex1.x * K::one_over_256, (float) vertex1.y * K::one_over_256, (float) vertex1.z * K::one_over_256);
		Point3D p2((float) vertex2.x * K::one_over_256, (float) vertex2.y * K::one_over_256, (float) vertex2.z * K::one_over_256);
		Point3D p3((float) vertex3.x * K::one_over_256, (float) vertex3.y * K::one_over_256, (float) vertex3.z * K::one_over_256);

		Vector3D v1 = p2 - p1;
		Vector3D v2 = p3 - p1;

		Vector3D normal = v1 % v2;
		Vector3D n1 = normal % v1;
		if (n1 * p1 < 0.0F)
		{
			Vector3D n2 = normal % (p3 - p2);
			if (n2 * p2 < 0.0F)
			{
				Vector3D n3 = v2 % normal;
				if (n3 * p3 < 0.0F)
				{
					float d = -(normal * p1);
					d = Fabs(d - voxel);
					if (d < best2)
					{
						best2 = d;
					}
				}
			}
		}

		indexArray += 3;
	}

	return (best2 < best1);
}

unsigned_int16 TerrainLevelGeometryObject::AddSharedVertex(const BuildVertex *sharedVertex, UnsignedVoxel material, BuildVertex *vertexStorage, int32& globalVertexCount)
{
	for (;;)
	{
		unsigned_int16 index = sharedVertex->nextIndex;
		if (index == 0)
		{
			index = (unsigned_int16) globalVertexCount++;
			sharedVertex->nextIndex = index;

			BuildVertex *buildVertex = &vertexStorage[index];
			buildVertex->position0 = sharedVertex->position0;
			buildVertex->position1 = sharedVertex->position1;
			buildVertex->normal = sharedVertex->normal;
			buildVertex->border = sharedVertex->border;
			buildVertex->blend = sharedVertex->blend;
			buildVertex->material = material;
			buildVertex->nextIndex = 0;

			return (index);
		}

		sharedVertex = &vertexStorage[index];
		if (sharedVertex->material == material)
		{
			return (index);
		}
	}
}

void TerrainLevelGeometryObject::Build(Geometry *geometry)
{
	using namespace Transvoxel;

	ArrayDescriptor		desc[7];

	LevelBuildStorage *buildStorage = new LevelBuildStorage;

	const TerrainBlock *block = static_cast<TerrainGeometry *>(geometry)->GetBlockNode();
	int32 detailLevel = GetDetailLevel();

	VoxelMap *densityMap = block->OpenBuildVoxelMap(kTerrainSubchannelDensity, GetGeometryCoord(), detailLevel);
	VoxelMap *blendMap = block->OpenBuildVoxelMap(kTerrainSubchannelBlend, GetGeometryCoord(), detailLevel);
	VoxelMap *materialMap = block->OpenBuildVoxelMap(kTerrainSubchannelMaterial, GetGeometryCoord(), detailLevel);

	int32 globalVertexCount = 0;
	int32 globalTriangleCount = 0;

	unsigned_int8 deckParity = 0;
	unsigned_int16 deltaMask = 0;

	machine dv = 1 << detailLevel;

	for (machine k = 0; k < kTerrainDimension; k++)
	{
		machine z = k << detailLevel;

		DeckStorage& currentDeck = buildStorage->deckStorage[deckParity];
		DeckStorage& previousDeck = buildStorage->deckStorage[deckParity ^ 1];

		for (machine j = 0; j < kTerrainDimension; j++)
		{
			machine y = j << detailLevel;

			for (machine i = 0; i < kTerrainDimension; i++)
			{
				UnsignedVoxel	material;
				Voxel			density[8];

				machine x = i << detailLevel;

				Voxel mainDensity = densityMap->GetVoxel(x + dv, y + dv, z + dv);
				UnsignedVoxel mainBlend = blendMap->GetUnsignedVoxel(x + dv, y + dv, z + dv);
				if (!FindBestMaterial(densityMap, materialMap, x, y, z, dv, &material))
				{
					material = materialMap->GetUnsignedVoxel(x + dv, y + dv, z + dv);
				}

				if (mainDensity == 0)
				{
					unsigned_int16 index = (unsigned_int16) globalVertexCount++;
					currentDeck[j][i][0] = index;

					BuildVertex *buildVertex = &buildStorage->vertexStorage[index];
					buildVertex->position0.Set((x + dv) << kVoxelFractionSize, (y + dv) << kVoxelFractionSize, (z + dv) << kVoxelFractionSize);
					buildVertex->normal = densityMap->CalculateNormal(Integer3D(x + dv, y + dv, z + dv));
					buildVertex->blend = mainBlend;
					buildVertex->material = material;
					buildVertex->nextIndex = 0;
				}

				density[0] = densityMap->GetVoxel(x, y, z);
				density[1] = densityMap->GetVoxel(x + dv, y, z);
				density[2] = densityMap->GetVoxel(x, y + dv, z);
				density[3] = densityMap->GetVoxel(x + dv, y + dv, z);
				density[4] = densityMap->GetVoxel(x, y, z + dv);
				density[5] = densityMap->GetVoxel(x + dv, y, z + dv);
				density[6] = densityMap->GetVoxel(x, y + dv, z + dv);
				density[7] = mainDensity;

				unsigned_int32 code = ((density[0] >> 7) & 0x01) | ((density[1] >> 6) & 0x02) | ((density[2] >> 5) & 0x04) | ((density[3] >> 4) & 0x08)
								   | ((density[4] >> 3) & 0x10) | ((density[5] >> 2) & 0x20) | ((density[6] >> 1) & 0x40) | (mainDensity & 0x80);

				if ((code ^ ((mainDensity >> 7) & 0xFF)) != 0)
				{
					UnsignedVoxel	blend[8];
					unsigned_int16	globalVertexIndex[12];

					blend[0] = blendMap->GetUnsignedVoxel(x, y, z);
					blend[1] = blendMap->GetUnsignedVoxel(x + dv, y, z);
					blend[2] = blendMap->GetUnsignedVoxel(x, y + dv, z);
					blend[3] = blendMap->GetUnsignedVoxel(x + dv, y + dv, z);
					blend[4] = blendMap->GetUnsignedVoxel(x, y, z + dv);
					blend[5] = blendMap->GetUnsignedVoxel(x + dv, y, z + dv);
					blend[6] = blendMap->GetUnsignedVoxel(x, y + dv, z + dv);
					blend[7] = mainBlend;

					unsigned_int8 cellClass = regularCellClass[code];
					const RegularCellData *cellData = &regularCellData[0][cellClass];

					int32 triangleCount = cellData->GetTriangleCount();
					if (globalTriangleCount + triangleCount > kMaxTerrainTriangleCount)
					{
						goto end;
					}

					int32 vertexCount = cellData->GetVertexCount();
					const unsigned_int16 *vertexArray = regularVertexData[code];

					for (machine a = 0; a < vertexCount; a++)
					{
						unsigned_int16	index;

						unsigned_int16 vertexData = vertexArray[a];
						unsigned_int16 v0 = (vertexData >> 4) & 0x0F;
						unsigned_int16 v1 = vertexData & 0x0F;

						int32 d0 = density[v0];
						int32 d1 = density[v1];
						Fixed t = (d1 << kVoxelFractionSize) / (d1 - d0);

						Integer3D coord0(x + dv * (v0 & 1), y + dv * ((v0 >> 1) & 1), z + dv * ((v0 >> 2) & 1));
						Integer3D coord1(x + dv * (v1 & 1), y + dv * ((v1 >> 1) & 1), z + dv * ((v1 >> 2) & 1));

						if ((t & (kVoxelFixedUnit - 1)) != 0)
						{
							unsigned_int16 edgeIndex = (vertexData >> 8) & 0x0F;
							unsigned_int16 deltaCode = (vertexData >> 12) & 0x0F;

							if ((deltaCode & deltaMask) == deltaCode)
							{
								if (deltaCode & 4)
								{
									index = previousDeck[j - ((deltaCode >> 1) & 1)][i - (deltaCode & 1)][edgeIndex];
								}
								else
								{
									index = currentDeck[j - (deltaCode >> 1)][i - (deltaCode & 1)][edgeIndex];
								}
							}
							else
							{
								index = (unsigned_int16) globalVertexCount++;
								BuildVertex *buildVertex = &buildStorage->vertexStorage[index];

								UnsignedVoxel b0 = blend[v0];
								UnsignedVoxel b1 = blend[v1];

								FindSurfaceCrossingEdge(densityMap, blendMap, detailLevel, coord0, coord1, d0, d1, b0, b1);

								t = (d1 << kVoxelFractionSize) / (d1 - d0);
								Fixed u = kVoxelFixedUnit - t;

								buildVertex->position0 = coord0 * t + coord1 * u;
								buildVertex->normal = densityMap->CalculateNormal(coord0, coord1, t, u);
								buildVertex->blend = (UnsignedVoxel) ((b0 * t + b1 * u) >> kVoxelFractionSize);
								buildVertex->material = material;
								buildVertex->nextIndex = 0;

								if (v1 == 7)
								{
									currentDeck[j][i][edgeIndex] = index;
								}

								globalVertexIndex[a] = index;
								continue;
							}
						}
						else if (t == 0)
						{
							if (v1 == 7)
							{
								index = currentDeck[j][i][0];
							}
							else
							{
								unsigned_int16 deltaCode = v1 ^ 7;
								if ((deltaCode & deltaMask) == deltaCode)
								{
									if (deltaCode & 4)
									{
										index = previousDeck[j - ((deltaCode >> 1) & 1)][i - (deltaCode & 1)][0];
									}
									else
									{
										index = currentDeck[j - (deltaCode >> 1)][i - (deltaCode & 1)][0];
									}
								}
								else
								{
									index = (unsigned_int16) globalVertexCount++;
									BuildVertex *buildVertex = &buildStorage->vertexStorage[index];

									buildVertex->position0 = coord1 << kVoxelFractionSize;
									buildVertex->normal = densityMap->CalculateNormal(coord1);
									buildVertex->blend = blend[v1];
									buildVertex->material = material;
									buildVertex->nextIndex = 0;

									globalVertexIndex[a] = index;
									continue;
								}
							}
						}
						else
						{
							unsigned_int16 deltaCode = v0 ^ 7;
							if ((deltaCode & deltaMask) == deltaCode)
							{
								if (deltaCode & 4)
								{
									index = previousDeck[j - ((deltaCode >> 1) & 1)][i - (deltaCode & 1)][0];
								}
								else
								{
									index = currentDeck[j - (deltaCode >> 1)][i - (deltaCode & 1)][0];
								}
							}
							else
							{
								index = (unsigned_int16) globalVertexCount++;
								BuildVertex *buildVertex = &buildStorage->vertexStorage[index];

								buildVertex->position0 = coord0 << kVoxelFractionSize;
								buildVertex->normal = densityMap->CalculateNormal(coord0);
								buildVertex->blend = blend[v0];
								buildVertex->material = material;
								buildVertex->nextIndex = 0;

								globalVertexIndex[a] = index;
								continue;
							}
						}

						const BuildVertex *sharedVertex = &buildStorage->vertexStorage[index];
						if (sharedVertex->material != material)
						{
							index = AddSharedVertex(sharedVertex, material, buildStorage->vertexStorage, globalVertexCount);
						}

						globalVertexIndex[a] = index;
					}

					if (material != kDeadTerrainMaterialIndex)
					{
						cellData += ChooseTriangulation(cellClass, densityMap, x, y, z, dv, globalVertexIndex, buildStorage->vertexStorage) * 16;

						const unsigned_int8 *localVertexIndex = cellData->vertexIndex;
						BuildTriangle *restrict buildTriangle = &buildStorage->triangleStorage[globalTriangleCount];

						for (machine a = 0; a < triangleCount; a++)
						{
							buildTriangle->triangle.index[0] = globalVertexIndex[localVertexIndex[0]];
							buildTriangle->triangle.index[1] = globalVertexIndex[localVertexIndex[1]];
							buildTriangle->triangle.index[2] = globalVertexIndex[localVertexIndex[2]];

							localVertexIndex += 3;
							buildTriangle++;
						}

						globalTriangleCount += triangleCount;
					}
				}

				deltaMask |= 1;
			}

			deltaMask = (deltaMask | 2) & 6;
		}

		deltaMask = 4;
		deckParity ^= 1;
	}

	end:
	int32 interiorVertexCount = globalVertexCount;
	int32 interiorTriangleCount = globalTriangleCount;

	unsigned_int32 renderMask = 0;
	for (machine a = 0; a < 6; a++)
	{
		int32 start = globalTriangleCount;
		BuildMipBorder(a, densityMap, blendMap, materialMap, globalVertexCount, globalTriangleCount, buildStorage);

		int32 count = globalTriangleCount - start;
		if (count != 0)
		{
			renderMask |= 1 << a;
		}

		borderRenderData.multiRenderData[a * 2] = start;
		borderRenderData.multiRenderData[a * 2 + 1] = count;
	}

	borderRenderData.multiRenderMask = renderMask;

	TerrainBlock::CloseBuildVoxelMap(materialMap);
	TerrainBlock::CloseBuildVoxelMap(blendMap);
	TerrainBlock::CloseBuildVoxelMap(densityMap);

	SetGeometryLevelCount(1);

	Fixed maxVoxelCoord = kTerrainDimension << (kVoxelFractionSize + detailLevel);
	unsigned_int32 maxVoxelCoordShift = kTerrainLogDimension + kVoxelFractionSize + detailLevel;

	int32 finalVertexCount = 0;
	for (machine a = 0; a < interiorVertexCount; a++)
	{
		BuildVertex *buildVertex = &buildStorage->vertexStorage[a];
		if (buildVertex->material != kDeadTerrainMaterialIndex)
		{
			const Fixed3D& vertex = buildVertex->position0;
			if (((vertex.x | vertex.y | vertex.z) >= 0) && (Max(Max(vertex.x, vertex.y), vertex.z) <= maxVoxelCoord))
			{
				unsigned_int32 status = 1;

				status |= (vertex.x >> (maxVoxelCoordShift - 1)) & 2;
				status |= (vertex.y >> (maxVoxelCoordShift - 2)) & 4;
				status |= (vertex.z >> (maxVoxelCoordShift - 3)) & 8;

				buildVertex->statusFlags = (unsigned_int8) status;
				buildVertex->remapIndex = (unsigned_int16) finalVertexCount;
				finalVertexCount++;

				CalculateSecondaryPosition(detailLevel, buildVertex);
				continue;
			}
		}

		buildVertex->statusFlags = 0;
	}

	for (machine a = interiorVertexCount; a < globalVertexCount; a++)
	{
		BuildVertex *buildVertex = &buildStorage->vertexStorage[a];
		if (buildVertex->material != kDeadTerrainMaterialIndex)
		{
			buildVertex->statusFlags = 1;
			buildVertex->remapIndex = (unsigned_int16) finalVertexCount;
			finalVertexCount++;
		}
		else
		{
			buildVertex->statusFlags = 0;
		}
	}

	int32 finalTriangleCount = 0;
	for (machine a = 0; a < interiorTriangleCount; a++)
	{
		BuildTriangle *buildTriangle = &buildStorage->triangleStorage[a];
		int32 i1 = buildTriangle->triangle.index[0];
		int32 i2 = buildTriangle->triangle.index[1];
		int32 i3 = buildTriangle->triangle.index[2];

		const BuildVertex *bv1 = &buildStorage->vertexStorage[i1];
		const BuildVertex *bv2 = &buildStorage->vertexStorage[i2];
		const BuildVertex *bv3 = &buildStorage->vertexStorage[i3];

		Fixed3D nrml = (bv2->position1 - bv1->position1) % (bv3->position1 - bv1->position1);
		unsigned_int32 statusMask = ((nrml.x >> 31) & 2) | ((nrml.y >> 31) & 4) | ((nrml.z >> 31) & 8) | 1;
		bool include = (((nrml.x | nrml.y | nrml.z) != 0) && ((bv1->statusFlags & bv2->statusFlags & bv3->statusFlags & statusMask) == 1));

		buildTriangle->inclusionFlag = include;
		finalTriangleCount += include;
	}

	borderRenderData.baseTriangleCount = finalTriangleCount;
	int32 triangleOffset = finalTriangleCount - interiorTriangleCount;
	finalTriangleCount += globalTriangleCount - interiorTriangleCount;

	for (machine a = 0; a < 6; a++)
	{
		borderRenderData.multiRenderData[a * 2] += triangleOffset;
	}

	desc[0].identifier = kArrayPosition0;
	desc[0].elementCount = finalVertexCount;
	desc[0].elementSize = sizeof(Point3D);
	desc[0].componentCount = 3;

	desc[1].identifier = kArrayPosition1;
	desc[1].elementCount = finalVertexCount;
	desc[1].elementSize = sizeof(Point3D);
	desc[1].componentCount = 3;

	desc[2].identifier = kArrayNormal;
	desc[2].elementCount = finalVertexCount;
	desc[2].elementSize = sizeof(Vector3D);
	desc[2].componentCount = 3;

	desc[3].identifier = kArrayColor0;
	desc[3].elementCount = finalVertexCount;
	desc[3].elementSize = 4;
	desc[3].componentCount = 1;

	desc[4].identifier = kArrayColor1;
	desc[4].elementCount = finalVertexCount;
	desc[4].elementSize = 4;
	desc[4].componentCount = 1;

	desc[5].identifier = kArrayColor2;
	desc[5].elementCount = finalVertexCount;
	desc[5].elementSize = 4;
	desc[5].componentCount = 1;

	desc[6].identifier = kArrayPrimitive;
	desc[6].elementCount = finalTriangleCount;
	desc[6].elementSize = sizeof(Triangle);
	desc[6].componentCount = 1;

	Mesh *mesh = GetGeometryLevel(0);
	mesh->AllocateStorage(finalVertexCount, 7, desc);

	const Integer3D& geometryCoord = GetGeometryCoord();
	Fixed gx = geometryCoord.x << (kTerrainLogDimension + kVoxelFractionSize);
	Fixed gy = geometryCoord.y << (kTerrainLogDimension + kVoxelFractionSize);
	Fixed gz = geometryCoord.z << (kTerrainLogDimension + kVoxelFractionSize);
	float scale = block->GetObject()->GetVoxelScale() * (1.0F / (float) kVoxelFixedUnit);

	Point3D *restrict position0 = mesh->GetArray<Point3D>(kArrayPosition0);
	Point3D *restrict position1 = mesh->GetArray<Point3D>(kArrayPosition1);
	Vector3D *restrict normal = mesh->GetArray<Vector3D>(kArrayNormal);
	Color4C *restrict color0 = mesh->GetArray<Color4C>(kArrayColor0);
	Color4C *restrict color1 = mesh->GetArray<Color4C>(kArrayColor1);
	Color4C *restrict color2 = mesh->GetArray<Color4C>(kArrayColor2);

	for (machine a = 0; a < globalVertexCount; a++)
	{
		const BuildVertex *buildVertex = &buildStorage->vertexStorage[a];
		if (buildVertex->statusFlags & 1)
		{
			float px = (float) (buildVertex->position0.x + gx) * scale;
			float py = (float) (buildVertex->position0.y + gy) * scale;
			float pz = (float) (buildVertex->position0.z + gz) * scale;
			position0->Set(px, py, pz);
			position0++;

			float qx = (float) (buildVertex->position1.x + gx) * scale;
			float qy = (float) (buildVertex->position1.y + gy) * scale;
			float qz = (float) (buildVertex->position1.z + gz) * scale;
			position1->Set(qx, qy, qz);
			position1++;

			*normal = buildVertex->normal;
			normal++;

			const TerrainMaterial *terrainMaterial = block->GetTerrainMaterial(buildVertex->material);
			color0->Set(terrainMaterial->primaryMaterial.GetBlue(), terrainMaterial->secondaryMaterial.GetBlue(), buildVertex->blend, 255);
			color1->Set(terrainMaterial->primaryMaterial.GetRed(), terrainMaterial->primaryMaterial.GetGreen(), terrainMaterial->secondaryMaterial.GetRed(), terrainMaterial->secondaryMaterial.GetGreen());
			color0++;
			color1++;

			*color2 = buildVertex->border;
			color2++;
		}
	}

	if (finalVertexCount != 0)
	{
		GetBoundingBox().Calculate(finalVertexCount, mesh->GetArray<Point3D>(kArrayPosition0));

		Triangle *restrict triangle = mesh->GetArray<Triangle>(kArrayPrimitive);
		for (machine a = 0; a < interiorTriangleCount; a++)
		{
			const BuildTriangle *buildTriangle = &buildStorage->triangleStorage[a];
			if (buildTriangle->inclusionFlag)
			{
				triangle->index[0] = buildStorage->vertexStorage[buildTriangle->triangle.index[0]].remapIndex;
				triangle->index[1] = buildStorage->vertexStorage[buildTriangle->triangle.index[1]].remapIndex;
				triangle->index[2] = buildStorage->vertexStorage[buildTriangle->triangle.index[2]].remapIndex;
				triangle++;
			}
		}

		for (machine a = interiorTriangleCount; a < globalTriangleCount; a++)
		{
			const BuildTriangle *buildTriangle = &buildStorage->triangleStorage[a];
			triangle->index[0] = buildStorage->vertexStorage[buildTriangle->triangle.index[0]].remapIndex;
			triangle->index[1] = buildStorage->vertexStorage[buildTriangle->triangle.index[1]].remapIndex;
			triangle->index[2] = buildStorage->vertexStorage[buildTriangle->triangle.index[2]].remapIndex;
			triangle++;
		}
	}

	Vector3D blockSize = block->GetBlockBoxSize();
	SetTexcoordParameter(1.0F / blockSize.x, 1.0F / blockSize.y, 0.0F, 0.0F);

	delete buildStorage;
}

void TerrainLevelGeometryObject::BuildMipBorder(int32 face, const VoxelMap *densityMap, const VoxelMap *blendMap, const VoxelMap *materialMap, int32& globalVertexCount, int32& globalTriangleCount, LevelBuildStorage *buildStorage)
{
	using namespace Transvoxel;

	static const unsigned_int8 voxelDelta[6][4] =
	{
		{0, 1, 0, 1},
		{0, 1, 0, 1},
		{1, 0, 0, 1},
		{1, 0, 0, 1},
		{1, 0, 1, 0},
		{1, 0, 1, 0}
	};

	static const unsigned_int8 voxelStart[6][3] =
	{
		{kTerrainDimension * 2, 0, 0},
		{0, 0, 0},
		{0, kTerrainDimension * 2, 0},
		{0, 0, 0},
		{0, 0, kTerrainDimension * 2},
		{0, 0, 0}
	};

	static const unsigned_int8 faceFlip[6] =
	{
		0x00, 0x80, 0x80, 0x00, 0x00, 0x80
	};

	unsigned_int8 rowParity = 0;
	unsigned_int16 deltaMask = 0;

	machine detailLevel = GetDetailLevel();
	machine dv = 1 << (detailLevel - 1);

	const unsigned_int8 *faceDelta = voxelDelta[face];
	machine dxdi = faceDelta[0] * dv;
	machine dydi = faceDelta[1] * dv;
	machine dydj = faceDelta[2] * dv;
	machine dzdj = faceDelta[3] * dv;

	for (machine j = 0; j < kTerrainDimension; j++)
	{
		machine x = voxelStart[face][0] * dv;
		machine y = voxelStart[face][1] * dv + j * dydj * 2;
		machine z = voxelStart[face][2] * dv + j * dzdj * 2;

		BorderVoxelStorage *restrict currentRow = buildStorage->rowStorage[rowParity];
		BorderVoxelStorage *restrict previousRow = buildStorage->rowStorage[rowParity ^ 1];

		for (machine i = 0; i < kTerrainDimension; i++)
		{
			Integer3D		coord[13];
			Voxel			density[13];
			UnsignedVoxel	blend[13];

			coord[0].Set(x, y, z);
			coord[1].Set(x + dxdi, y + dydi, z);
			coord[2].Set(x + dxdi * 2, y + dydi * 2, z);
			coord[3].Set(x, y + dydj, z + dzdj);
			coord[4].Set(x + dxdi, y + dydi + dydj, z + dzdj);
			coord[5].Set(x + dxdi * 2, y + dydi * 2 + dydj, z + dzdj);
			coord[6].Set(x, y + dydj * 2, z + dzdj * 2);
			coord[7].Set(x + dxdi, y + dydi + dydj * 2, z + dzdj * 2);
			coord[8].Set(x + dxdi * 2, y + dydi * 2 + dydj * 2, z + dzdj * 2);

			for (machine a = 0; a < 9; a++)
			{
				density[a] = densityMap->GetVoxel(coord[a].x, coord[a].y, coord[a].z);
			}

			for (machine a = 0; a < 9; a++)
			{
				blend[a] = blendMap->GetUnsignedVoxel(coord[a].x, coord[a].y, coord[a].z);
			}

			UnsignedVoxel material = materialMap->GetUnsignedVoxel(coord[4].x, coord[4].y, coord[4].z);

			coord[9] = coord[0];
			coord[10] = coord[2];
			coord[11] = coord[6];
			coord[12] = coord[8];

			density[9] = density[0];
			density[10] = density[2];
			density[11] = density[6];
			density[12] = density[8];

			blend[9] = blend[0];
			blend[10] = blend[2];
			blend[11] = blend[6];
			blend[12] = blend[8];

			if (density[8] == 0)
			{
				unsigned_int16 index = (unsigned_int16) globalVertexCount++;
				currentRow[i][0] = index;

				BuildVertex *buildVertex = &buildStorage->vertexStorage[index];
				buildVertex->position0 = coord[8] << kVoxelFractionSize;
				buildVertex->position1 = buildStorage->vertexStorage[index].position0;
				buildVertex->normal = densityMap->CalculateNormal(coord[8]);
				buildVertex->border.Set(0x80, 0x80, 0x80, 0x00);
				buildVertex->blend = blend[8];
				buildVertex->material = material;
				buildVertex->nextIndex = 0;
			}

			if (density[7] == 0)
			{
				unsigned_int16 index = (unsigned_int16) globalVertexCount++;
				currentRow[i][1] = index;

				BuildVertex *buildVertex = &buildStorage->vertexStorage[index];
				buildVertex->position0 = coord[7] << kVoxelFractionSize;
				buildVertex->position1 = buildStorage->vertexStorage[index].position0;
				buildVertex->normal = densityMap->CalculateNormal(coord[7]);
				buildVertex->border.Set(0x80, 0x80, 0x80, 0x00);
				buildVertex->blend = blend[7];
				buildVertex->material = material;
				buildVertex->nextIndex = 0;
			}

			if (density[5] == 0)
			{
				unsigned_int16 index = (unsigned_int16) globalVertexCount++;
				currentRow[i][2] = index;

				BuildVertex *buildVertex = &buildStorage->vertexStorage[index];
				buildVertex->position0 = coord[5] << kVoxelFractionSize;
				buildVertex->position1 = buildStorage->vertexStorage[index].position0;
				buildVertex->normal = densityMap->CalculateNormal(coord[5]);
				buildVertex->border.Set(0x80, 0x80, 0x80, 0x00);
				buildVertex->blend = blend[5];
				buildVertex->material = material;
				buildVertex->nextIndex = 0;
			}

			if (density[12] == 0)
			{
				unsigned_int16 index = (unsigned_int16) globalVertexCount++;
				currentRow[i][7] = index;

				BuildVertex *buildVertex = &buildStorage->vertexStorage[index];
				buildVertex->position0 = coord[12] << kVoxelFractionSize;
				buildVertex->normal = densityMap->CalculateNormal(coord[12]);
				buildVertex->blend = blend[12];
				buildVertex->material = material;
				buildVertex->nextIndex = 0;
				CalculateSecondaryPosition(detailLevel, buildVertex);
			}

			unsigned_int32 code = ((density[0] >> 7) & 0x01) | ((density[1] >> 6) & 0x02) | ((density[2] >> 5) & 0x04) | ((density[5] >> 4) & 0x08)
							   | ((density[8] >> 3) & 0x10) | ((density[7] >> 2) & 0x20) | ((density[6] >> 1) & 0x40) | (density[3] & 0x80) | ((density[4] << 1) & 0x100);

			if ((code ^ ((density[8] >> 7) & 0x1FF)) != 0)
			{
				unsigned_int16	globalVertexIndex[12];

				unsigned_int8 cellClass = transitionCellClass[code];
				const TransitionCellData *cellData = &transitionCellData[cellClass & 0x7F];

				int32 triangleCount = cellData->GetTriangleCount();
				if (globalTriangleCount + triangleCount > kMaxTerrainTriangleCount)
				{
					return;
				}

				int32 vertexCount = cellData->GetVertexCount();
				const unsigned_int16 *vertexArray = transitionVertexData[code];

				for (machine a = 0; a < vertexCount; a++)
				{
					unsigned_int16	index;

					unsigned_int16 vertexData = vertexArray[a];
					unsigned_int16 v0 = (vertexData >> 4) & 0x0F;
					unsigned_int16 v1 = vertexData & 0x0F;

					int32 d0 = density[v0];
					int32 d1 = density[v1];
					Fixed t = (d1 << kVoxelFractionSize) / (d1 - d0);

					if ((t & (kVoxelFixedUnit - 1)) != 0)
					{
						unsigned_int16 edgeIndex = (vertexData >> 8) & 0x0F;
						unsigned_int16 deltaCode = (vertexData >> 12) & 0x0F;

						if ((deltaCode & deltaMask) == deltaCode)
						{
							if (deltaCode & 2)
							{
								index = previousRow[i - (deltaCode & 1)][edgeIndex];
							}
							else
							{
								index = currentRow[i - (deltaCode & 1)][edgeIndex];
							}
						}
						else
						{
							UnsignedVoxel b0 = blend[v0];
							UnsignedVoxel b1 = blend[v1];
							Integer3D coord0 = coord[v0];
							Integer3D coord1 = coord[v1];

							int32 level = detailLevel;
							if (edgeIndex > 7)
							{
								FindSurfaceCrossingEdge(densityMap, blendMap, level, coord0, coord1, d0, d1, b0, b1);
								t = (d1 << kVoxelFractionSize) / (d1 - d0);
							}
							else
							{
								if (level > 1)
								{
									FindSurfaceCrossingEdge(densityMap, blendMap, level - 1, coord0, coord1, d0, d1, b0, b1);
									t = (d1 << kVoxelFractionSize) / (d1 - d0);
								}
							}

							index = (unsigned_int16) globalVertexCount++;
							BuildVertex *buildVertex = &buildStorage->vertexStorage[index];

							Fixed u = kVoxelFixedUnit - t;
							buildVertex->position0 = coord0 * t + coord1 * u;
							buildVertex->blend = (UnsignedVoxel) ((b0 * t + b1 * u) >> kVoxelFractionSize);
							buildVertex->material = material;
							buildVertex->nextIndex = 0;

							if (edgeIndex > 7)
							{
								buildVertex->normal = densityMap->CalculateNormal(coord0, coord1, t, u);
								CalculateSecondaryPosition(detailLevel, buildVertex);
							}
							else
							{
								buildVertex->normal = densityMap->CalculateNormal(coord0, coord1, t, u);
								buildVertex->position1 = buildStorage->vertexStorage[index].position0;
								buildVertex->border.Set(0x80, 0x80, 0x80, 0x00);
							}

							if (deltaCode == 8)
							{
								currentRow[i][edgeIndex] = index;
							}

							globalVertexIndex[a] = index;
							continue;
						}
					}
					else if (t == 0)
					{
						unsigned_int8 cornerData = transitionCornerData[v1];
						unsigned_int16 deltaCode = cornerData >> 4;

						if (deltaCode == 8)
						{
							index = currentRow[i][cornerData & 0x0F];
						}
						else if ((deltaCode & deltaMask) == deltaCode)
						{
							if (deltaCode & 2)
							{
								index = previousRow[i - (deltaCode & 1)][cornerData & 0x0F];
							}
							else
							{
								index = currentRow[i - (deltaCode & 1)][cornerData & 0x0F];
							}
						}
						else
						{
							index = (unsigned_int16) globalVertexCount++;
							BuildVertex *buildVertex = &buildStorage->vertexStorage[index];

							buildVertex->position0 = coord[v1] << kVoxelFractionSize;
							buildVertex->blend = blend[v1];
							buildVertex->material = material;
							buildVertex->nextIndex = 0;

							unsigned_int16 edgeIndex = (vertexData >> 8) & 0x0F;
							if (edgeIndex > 7)
							{
								buildVertex->normal = densityMap->CalculateNormal(coord[v1]);
								CalculateSecondaryPosition(detailLevel, buildVertex);
							}
							else
							{
								buildVertex->normal = densityMap->CalculateNormal(coord[v1]);
								buildVertex->position1 = buildStorage->vertexStorage[index].position0;
								buildVertex->border.Set(0x80, 0x80, 0x80, 0x00);
							}

							globalVertexIndex[a] = index;
							continue;
						}
					}
					else
					{
						unsigned_int8 cornerData = transitionCornerData[v0];
						unsigned_int16 deltaCode = cornerData >> 4;

						if (deltaCode == 8)
						{
							index = currentRow[i][cornerData & 0x0F];
						}
						else if ((deltaCode & deltaMask) == deltaCode)
						{
							if (deltaCode & 2)
							{
								index = previousRow[i - (deltaCode & 1)][cornerData & 0x0F];
							}
							else
							{
								index = currentRow[i - (deltaCode & 1)][cornerData & 0x0F];
							}
						}
						else
						{
							index = (unsigned_int16) globalVertexCount++;
							BuildVertex *buildVertex = &buildStorage->vertexStorage[index];

							buildVertex->position0 = coord[v0] << kVoxelFractionSize;
							buildVertex->blend = blend[v0];
							buildVertex->material = material;
							buildVertex->nextIndex = 0;

							unsigned_int16 edgeIndex = (vertexData >> 8) & 0x0F;
							if (edgeIndex > 7)
							{
								buildVertex->normal = densityMap->CalculateNormal(coord[v0]);
								CalculateSecondaryPosition(detailLevel, buildVertex);
							}
							else
							{
								buildVertex->normal = densityMap->CalculateNormal(coord[v0]);
								buildVertex->position1 = buildStorage->vertexStorage[index].position0;
								buildVertex->border.Set(0x80, 0x80, 0x80, 0x00);
							}

							globalVertexIndex[a] = index;
							continue;
						}
					}

					const BuildVertex *sharedVertex = &buildStorage->vertexStorage[index];
					if (sharedVertex->material != material)
					{
						index = AddSharedVertex(sharedVertex, material, buildStorage->vertexStorage, globalVertexCount);
					}

					globalVertexIndex[a] = index;
				}

				if (material != kDeadTerrainMaterialIndex)
				{
					const unsigned_int8 *localVertexIndex = cellData->vertexIndex;
					BuildTriangle *restrict buildTriangle = &buildStorage->triangleStorage[globalTriangleCount];

					if (((cellClass & 0x80) ^ faceFlip[face]) != 0)
					{
						for (machine a = 0; a < triangleCount; a++)
						{
							buildTriangle->triangle.index[0] = globalVertexIndex[localVertexIndex[0]];
							buildTriangle->triangle.index[1] = globalVertexIndex[localVertexIndex[1]];
							buildTriangle->triangle.index[2] = globalVertexIndex[localVertexIndex[2]];

							localVertexIndex += 3;
							buildTriangle++;
						}
					}
					else
					{
						for (machine a = 0; a < triangleCount; a++)
						{
							buildTriangle->triangle.index[0] = globalVertexIndex[localVertexIndex[0]];
							buildTriangle->triangle.index[1] = globalVertexIndex[localVertexIndex[2]];
							buildTriangle->triangle.index[2] = globalVertexIndex[localVertexIndex[1]];

							localVertexIndex += 3;
							buildTriangle++;
						}
					}

					globalTriangleCount += triangleCount;
				}
			}

			deltaMask |= 1;

			x += dxdi * 2;
			y += dydi * 2;
		}

		deltaMask = 2;
		rowParity ^= 1;
	}
}


TerrainGeometry::TerrainGeometry() : Geometry(kGeometryTerrain)
{
	blockNode = nullptr;
}

TerrainGeometry::TerrainGeometry(TerrainBlock *block, const Integer3D& coord) : Geometry(kGeometryTerrain)
{
	blockNode = block;
	SetNewObject(new TerrainGeometryObject(coord));
}

TerrainGeometry::TerrainGeometry(TerrainBlock *block, const Integer3D& coord, int32 level) : Geometry(kGeometryTerrain)
{
	blockNode = block;
	SetNewObject(new TerrainLevelGeometryObject(coord, level));
}

TerrainGeometry::TerrainGeometry(const TerrainGeometry& terrainGeometry) : Geometry(terrainGeometry)
{
	blockNode = nullptr;
}

TerrainGeometry::~TerrainGeometry()
{
}

Node *TerrainGeometry::Replicate(void) const
{
	return (new TerrainGeometry(*this));
}

void TerrainGeometry::PackType(Packer& data) const
{
	Geometry::PackType(data);
	data << int32(0);
}

void TerrainGeometry::HandleTransformUpdate(void)
{
	Geometry::HandleTransformUpdate();

	const TerrainBlock *block = blockNode;
	if (block)
	{
		const TerrainGeometryObject *object = GetObject();

		float scale = block->GetObject()->GetVoxelScale();
		float radius = (float) ((1 << object->GetDetailLevel()) * (kTerrainDimension / 2)) * scale;
		renderDistance = radius * 4.0F;

		const Integer3D& coord = object->GetGeometryCoord();
		worldCenter = GetWorldPosition() + GetWorldTransform() * Vector3D((float) (coord.x * kTerrainDimension) * scale + radius, (float) (coord.y * kTerrainDimension) * scale + radius, (float) (coord.z * kTerrainDimension) * scale + radius);
	}
}

bool TerrainGeometry::CalculateBoundingBox(Box3D *box) const
{
	*box = GetObject()->GetBoundingBox();
	return (true);
}

bool TerrainGeometry::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	const Box3D& box = GetObject()->GetBoundingBox();
	sphere->SetCenter(box.GetCenter());
	sphere->SetRadius(Magnitude(box.GetSize()) * 0.5F);
	return (true);
}

void TerrainGeometry::Preinitialize(void)
{
	TerrainBlock *block = nullptr;

	Node *node = GetSuperNode();
	while (node)
	{
		if (node->GetNodeType() == kNodeTerrainBlock)
		{
			block = static_cast<TerrainBlock *>(node);
			break;
		}

		node = node->GetSuperNode();
	}

	blockNode = block;

	Geometry::Preinitialize();
}

void TerrainGeometry::Preprocess(void)
{
	SetNodeFlags(GetNodeFlags() | (kNodeVisibilitySite | kNodeIsolatedVisibility));
	Geometry::Preprocess();

	SetShaderFlags(GetShaderFlags() | (kShaderGenerateTexcoord | kShaderGenerateTangent));
	SetTexcoordParameterPointer(GetObject()->GetTexcoordParameter());

	SetActiveUpdateFlags(GetActiveUpdateFlags() | kUpdatePostprocess);
}


TerrainLevelGeometry::TerrainLevelGeometry()
{
}

TerrainLevelGeometry::TerrainLevelGeometry(TerrainBlock *block, const Integer3D& coord, int32 level) : TerrainGeometry(block, coord, level)
{
}

TerrainLevelGeometry::TerrainLevelGeometry(const TerrainLevelGeometry& terrainLevelGeometry) : TerrainGeometry(terrainLevelGeometry)
{
}

TerrainLevelGeometry::~TerrainLevelGeometry()
{
}

Node *TerrainLevelGeometry::Replicate(void) const
{
	return (new TerrainLevelGeometry(*this));
}

void TerrainLevelGeometry::PackType(Packer& data) const
{
	Geometry::PackType(data);
	data << int32(1);
}

void TerrainLevelGeometry::Pack(Packer& data, unsigned_int32 packFlags) const
{
	TerrainGeometry::Pack(data, packFlags);

	for (machine a = 0; a < 6; a++)
	{
		const Node *terrain = neighborTerrain[a];
		if ((terrain) && (terrain->LinkedNodePackable(packFlags)))
		{
			data << ChunkHeader('NGBR', 8);
			data << terrain->GetNodeIndex();
			data << int32(a);
		}
	}

	data << TerminatorChunk;
}

void TerrainLevelGeometry::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	TerrainGeometry::Unpack(data, unpackFlags);
	UnpackChunkList<TerrainLevelGeometry>(data, unpackFlags);
}

bool TerrainLevelGeometry::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'NGBR':
		{
			int32	nodeIndex, neighborIndex;

			data >> nodeIndex;
			data >> neighborIndex;
			data.AddNodeLink(nodeIndex, &TerrainLinkProc, &neighborTerrain[neighborIndex]);
			return (true);
		}
	}

	return (false);
}

void TerrainLevelGeometry::TerrainLinkProc(Node *node, void *cookie)
{
	*static_cast<Link<Node> *>(cookie) = node;
}

bool TerrainLevelGeometry::CalculateBoundingBox(Box3D *box) const
{
	*box = GetObject()->GetBoundingBox();

	const Node *node = GetFirstSubnode();
	while (node)
	{
		if (node->GetNodeType() == kNodeGeometry)
		{
			const Geometry *geometry = static_cast<const Geometry *>(node);
			if (geometry->GetGeometryType() == kGeometryTerrain)
			{
				box->Union(static_cast<const TerrainGeometry *>(geometry)->GetObject()->GetBoundingBox());
			}
		}

		node = node->Next();
	}

	return (true);
}

void TerrainLevelGeometry::Preprocess(void)
{
	TerrainGeometry::Preprocess();

	SetShaderFlags(GetShaderFlags() | kShaderTerrainBorder);

	for (machine a = 0; a < kTerrainStampCount; a++)
	{
		terrainStamp[a] = 0xFFFFFFFF;
		subterrainStamp[a] = 0xFFFFFFFF;
	}

	const TerrainLevelGeometryObject *object = GetObject();
	const TerrainBlock *block = GetBlockNode();

	if (block)
	{
		const Integer3D& coord = object->GetGeometryCoord();
		int32 dc = 1 << object->GetDetailLevel();

		const Integer3D& blockSize = block->GetBlockSize();

		if (neighborTerrain[0])
		{
			defaultBorderState[0].x = 1.0F;
		}
		else
		{
			defaultBorderState[0].x = (coord.x < blockSize.x - dc) ? 0.0F : 1.0F;
		}

		if (neighborTerrain[1])
		{
			defaultBorderState[1].x = 1.0F;
		}
		else
		{
			defaultBorderState[1].x = (coord.x > 0) ? 0.0F : 1.0F;
		}

		if (neighborTerrain[2])
		{
			defaultBorderState[0].y = 1.0F;
		}
		else
		{
			defaultBorderState[0].y = (coord.y < blockSize.y - dc) ? 0.0F : 1.0F;
		}

		if (neighborTerrain[3])
		{
			defaultBorderState[1].y = 1.0F;
		}
		else
		{
			defaultBorderState[1].y = (coord.y > 0) ? 0.0F : 1.0F;
		}

		if (neighborTerrain[4])
		{
			defaultBorderState[0].z = 1.0F;
		}
		else
		{
			defaultBorderState[0].z = (coord.z < blockSize.z - dc) ? 0.0F : 1.0F;
		}

		if (neighborTerrain[5])
		{
			defaultBorderState[1].z = 1.0F;
		}
		else
		{
			defaultBorderState[1].z = (coord.z > 0) ? 0.0F : 1.0F;
		}
	}
	else
	{
		defaultBorderState[0].Set(1.0F, 1.0F, 1.0F);
		defaultBorderState[1].Set(1.0F, 1.0F, 1.0F);
	}

	terrainBorderState[0] = defaultBorderState[0];
	terrainBorderState[1] = defaultBorderState[1];
	SetTerrainParameterPointer(terrainBorderState);

	unsigned_int32 renderMask = object->GetMultiRenderMask();
	if (renderMask != 0)
	{
		SetRenderType(kRenderMaskedMultiIndexedTriangles);

		RenderSegment *segment = GetFirstRenderSegment();
		segment->SetPrimitiveRange(0, object->GetBaseTriangleCount());
		segment->SetMultiRenderData(object->GetMultiRenderData());
		segment->SetMultiRenderMask(0);
	}
}

void TerrainLevelGeometry::ProcessStructure(void)
{
	const TerrainGeometryObject *object = GetObject();
	const Integer3D& coord = object->GetGeometryCoord();
	int32 level = object->GetDetailLevel();
	int32 dc = 1 << level;

	TerrainBlock *block = GetBlockNode();
	neighborTerrain[0] = block->FindTerrainGeometry(Integer3D(coord.x + dc, coord.y, coord.z), level);
	neighborTerrain[1] = block->FindTerrainGeometry(Integer3D(coord.x - dc, coord.y, coord.z), level);
	neighborTerrain[2] = block->FindTerrainGeometry(Integer3D(coord.x, coord.y + dc, coord.z), level);
	neighborTerrain[3] = block->FindTerrainGeometry(Integer3D(coord.x, coord.y - dc, coord.z), level);
	neighborTerrain[4] = block->FindTerrainGeometry(Integer3D(coord.x, coord.y, coord.z + dc), level);
	neighborTerrain[5] = block->FindTerrainGeometry(Integer3D(coord.x, coord.y, coord.z - dc), level);
}

float TerrainLevelGeometry::GetRenderStatus(int32 stampIndex, unsigned_int32 renderStamp) const
{
	if ((terrainStamp[stampIndex] != renderStamp) && (subterrainStamp[stampIndex] == renderStamp))
	{
		return (0.0F);
	}

	return (1.0F);
}

void TerrainLevelGeometry::UpdateBorderState(int32 stampIndex, unsigned_int32 renderStamp)
{
	unsigned_int32 renderMask = 0;

	const TerrainLevelGeometry *terrain = static_cast<TerrainLevelGeometry *>(neighborTerrain[0].GetTarget());
	float state = (terrain) ? terrain->GetRenderStatus(stampIndex, renderStamp) : defaultBorderState[0].x;
	terrainBorderState[0].x = state;
	if (state == 0.0F)
	{
		renderMask |= 1;
	}

	terrain = static_cast<TerrainLevelGeometry *>(neighborTerrain[1].GetTarget());
	state = (terrain) ? terrain->GetRenderStatus(stampIndex, renderStamp) : defaultBorderState[1].x;
	terrainBorderState[1].x = state;
	if (state == 0.0F)
	{
		renderMask |= 2;
	}

	terrain = static_cast<TerrainLevelGeometry *>(neighborTerrain[2].GetTarget());
	state = (terrain) ? terrain->GetRenderStatus(stampIndex, renderStamp) : defaultBorderState[0].y;
	terrainBorderState[0].y = state;
	if (state == 0.0F)
	{
		renderMask |= 4;
	}

	terrain = static_cast<TerrainLevelGeometry *>(neighborTerrain[3].GetTarget());
	state = (terrain) ? terrain->GetRenderStatus(stampIndex, renderStamp) : defaultBorderState[1].y;
	terrainBorderState[1].y = state;
	if (state == 0.0F)
	{
		renderMask |= 8;
	}

	terrain = static_cast<TerrainLevelGeometry *>(neighborTerrain[4].GetTarget());
	state = (terrain) ? terrain->GetRenderStatus(stampIndex, renderStamp) : defaultBorderState[0].z;
	terrainBorderState[0].z = state;
	if (state == 0.0F)
	{
		renderMask |= 16;
	}

	terrain = static_cast<TerrainLevelGeometry *>(neighborTerrain[5].GetTarget());
	state = (terrain) ? terrain->GetRenderStatus(stampIndex, renderStamp) : defaultBorderState[1].z;
	terrainBorderState[1].z = state;
	if (state == 0.0F)
	{
		renderMask |= 32;
	}

	GetFirstRenderSegment()->SetMultiRenderMask(renderMask & GetObject()->GetMultiRenderMask());
}

// ZYUQURM
