 

#ifndef C4Voxels_h
#define C4Voxels_h


#include "C4Types.h"


namespace C4
{
	typedef int8			Voxel;
	typedef unsigned_int8	UnsignedVoxel;


	enum
	{
		kVoxelFractionSize		= 8,
		kVoxelFixedUnit			= 1 << kVoxelFractionSize
	};


	namespace Transvoxel
	{
		struct RegularCellData
		{
			unsigned_int8	geometryCounts;
			unsigned_int8	vertexIndex[15];

			int32 GetVertexCount(void) const
			{
				return (geometryCounts >> 4);
			}

			int32 GetTriangleCount(void) const
			{
				return (geometryCounts & 0x0F);
			}
		};


		struct TransitionCellData
		{
			int32			geometryCounts;
			unsigned_int8	vertexIndex[36];

			int32 GetVertexCount(void) const
			{
				return (geometryCounts >> 4);
			}

			int32 GetTriangleCount(void) const
			{
				return (geometryCounts & 0x0F);
			}
		};


		struct InternalEdgeData
		{
			unsigned_int8	edgeCount;
			unsigned_int8	vertexIndex[4][4];
		};


		alignas(128) extern const unsigned_int8 regularCellClass[256];
		alignas(128) extern const RegularCellData regularCellData[2][16];
		alignas(128) extern const InternalEdgeData regularInternalEdgeData[2][16];
		alignas(128) extern const unsigned_int16 regularVertexData[256][12];

		alignas(128) extern const unsigned_int8 transitionCellClass[512];
		alignas(128) extern const TransitionCellData transitionCellData[56];
		alignas(128) extern const unsigned_int8 transitionCornerData[13];
		alignas(128) extern const unsigned_int16 transitionVertexData[512][12];
	}
}


#endif

// ZYUQURM
