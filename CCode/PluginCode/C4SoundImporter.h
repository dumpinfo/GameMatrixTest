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


#ifndef C4SoundImporter_h
#define C4SoundImporter_h


#include "C4Plugins.h"
#include "C4Sound.h"


namespace C4
{
	class WaveResource : public Resource<WaveResource>
	{
		friend class Resource<WaveResource>;

		private:

			static ResourceDescriptor		descriptor;

			const WaveHeader	*waveHeader;
			const Sample		*sampleData;
			unsigned_int32		sampleCount;

			~WaveResource();

			void Preprocess(void) override;

		public:

			WaveResource(const char *name, ResourceCatalog *catalog);

			const WaveHeader *GetWaveHeader(void) const
			{
				return (waveHeader);
			}

			const Sample *GetSampleData(void) const
			{
				return (sampleData);
			}

			unsigned_int32 GetSampleCount(void) const
			{
				return (sampleCount);
			}
	};
}


#endif

// ZYUQURM
