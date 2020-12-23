 

#ifndef C4Compression_h
#define C4Compression_h


#include "C4Types.h"


namespace C4
{
	namespace Comp
	{
		C4API unsigned_int32 CompressData(const void *input, unsigned_int32 dataSize, unsigned_int8 *restrict code);
		C4API void DecompressData(const unsigned_int8 *restrict code, unsigned_int32 codeSize, void *output);
	}
}


#endif

// ZYUQURM
