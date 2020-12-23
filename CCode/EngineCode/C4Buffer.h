 

#ifndef C4Buffer_h
#define C4Buffer_h


#include "C4Memory.h"


namespace C4
{
	enum
	{
		kBufferListMinLogSize	= 8,
		kBufferListMaxLogSize	= 25,
		kBufferFreeListCount	= kBufferListMaxLogSize - kBufferListMinLogSize + 1
	};


	struct ManagedBlock
	{
		unsigned_int32		blockStart;
		unsigned_int32		blockSize;

		ManagedBlock		*prevBlock;
		ManagedBlock		*nextBlock;
		ManagedBlock		*prevFreeBlock;
		ManagedBlock		*nextFreeBlock;

		int32				releaseCount;
		ManagedBlock		*nextReleasedBlock;
	};


	class ManagedBuffer
	{
		private:

			ManagedBlock			*firstFreeBlock[kBufferFreeListCount];
			ManagedBlock			*firstDeadBlock;

			ManagedBlock			*firstReleasedBlock;
			ManagedBlock			*lastReleasedBlock;

			unsigned_int32			bufferAlignment;

			#if C4DEBUG

				unsigned_int32		usedBlockCount;
				unsigned_int32		usedBlockTotalSize;

			#endif

			ManagedBlock			blockTable[1];

			ManagedBuffer(unsigned_int32 size, unsigned_int32 blockCount, unsigned_int32 alignment);
			~ManagedBuffer();

			void RegisterFreeBlock(ManagedBlock *block);
			void UnregisterFreeBlock(ManagedBlock *block, unsigned_int32 index);
			void UnregisterFreeBlock(ManagedBlock *block);

			ManagedBlock *SplitBlock(ManagedBlock *block, unsigned_int32 size, unsigned_int32 index);
			void DestroyBlock(ManagedBlock *block);

		public:

			static ManagedBuffer *New(unsigned_int32 size, unsigned_int32 blockCount, unsigned_int32 alignment = 256);
			static void Release(ManagedBuffer *buffer);

			ManagedBlock *NewBlock(unsigned_int32 size);
			void ReleaseBlock(ManagedBlock *block);

			void BufferTask(void);
	};
}


#endif

// ZYUQURM
