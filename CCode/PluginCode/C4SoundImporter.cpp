//=============================================================
//
// C4 Engine version 4.5
// Copyright 1999-2015, by Terathon Software LLC
//
// This file is part of the C4 Engine and is provided under the
// terms of the license agreement entered by the registed user.
//
// Unauthorized redistribution of source code is strictly
// prohibited. Violators will be prosecuted.
//
//=============================================================


#include "C4SoundImporter.h"


using namespace C4;


namespace
{
	struct RiffChunkHeader
	{
		unsigned_int32	type;
		unsigned_int32	size;
	};
}


ResourceDescriptor WaveResource::descriptor("wav");


WaveResource::WaveResource(const char *name, ResourceCatalog *catalog) : Resource<WaveResource>(name, catalog)
{
}

WaveResource::~WaveResource()
{
}

void WaveResource::Preprocess(void)
{
	char *chunkData = static_cast<char *>(GetData());

	unsigned_int32 position = 12;
	unsigned_int32 resourceSize = GetSize() - sizeof(RiffChunkHeader);
	while (position < resourceSize)
	{
		RiffChunkHeader *chunkHeader = reinterpret_cast<RiffChunkHeader *>(&chunkData[position]);
		position += sizeof(ChunkHeader);

		if (chunkHeader->type == ' tmf')
		{
			WaveHeader *header = reinterpret_cast<WaveHeader *>(chunkHeader + 1);
			waveHeader = header;
		}
		else if (chunkHeader->type == 'atad')
		{
			sampleData = reinterpret_cast<Sample *>(chunkHeader + 1);
			sampleCount = chunkHeader->size / sizeof(Sample);
		}

		position += (chunkHeader->size + 1) & ~1;
	}
}

// ZYUQURM
