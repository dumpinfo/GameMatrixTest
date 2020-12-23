 

#include "C4Buffer.h"


using namespace C4;


namespace
{
	enum
	{
		kBlockFreeFlag = 1 << 31
	};


	inline unsigned_int32 GetFreeListIndex(unsigned_int32 size)
	{
		return (Min(MaxZero(31 - kBufferListMinLogSize - Cntlz(size & ~kBlockFreeFlag)), kBufferFreeListCount - 1));
	}
}


ManagedBuffer::ManagedBuffer(unsigned_int32 size, unsigned_int32 blockCount, unsigned_int32 alignment)
{
	bufferAlignment = alignment - 1;

	for (machine a = 0; a < kBufferFreeListCount; a++)
	{
		firstFreeBlock[a] = nullptr;
	}
	
	firstFreeBlock[GetFreeListIndex(size)] = &blockTable[0];
	firstDeadBlock = &blockTable[1];

	firstReleasedBlock = nullptr;
	lastReleasedBlock = nullptr;

	#if C4DEBUG

		usedBlockCount = 0;
		usedBlockTotalSize = 0;

	#endif

	blockTable[0].blockStart = 0;
	blockTable[0].blockSize = size | kBlockFreeFlag;
	blockTable[0].prevBlock = nullptr;
	blockTable[0].nextBlock = nullptr;
	blockTable[0].prevFreeBlock = nullptr;
	blockTable[0].nextFreeBlock = nullptr;

	for (unsigned_machine a = 2; a < blockCount; a++)
	{
		blockTable[a - 1].nextBlock = &blockTable[a];
	}
	
	blockTable[blockCount - 1].nextBlock = nullptr;
}

ManagedBuffer::~ManagedBuffer()
{
}

ManagedBuffer *ManagedBuffer::New(unsigned_int32 size, unsigned_int32 blockCount, unsigned_int32 alignment)
{
	char *storage = new char[sizeof(ManagedBuffer) + (blockCount - 1) * sizeof(ManagedBlock)];
	return (new(storage) ManagedBuffer(size, blockCount, alignment));
}

void ManagedBuffer::Release(ManagedBuffer *buffer)
{
	buffer->~ManagedBuffer();
	delete[] reinterpret_cast<char *>(buffer);
}

void ManagedBuffer::RegisterFreeBlock(ManagedBlock *block)
{
	unsigned_int32 index = GetFreeListIndex(block->blockSize);
	ManagedBlock *firstBlock = firstFreeBlock[index];
	if (firstBlock)
	{
		firstBlock->prevFreeBlock = block;
	}

	block->prevFreeBlock = nullptr;
	block->nextFreeBlock = firstBlock;
	firstFreeBlock[index] = block;
}

void ManagedBuffer::UnregisterFreeBlock(ManagedBlock *block, unsigned_int32 index)
{
	ManagedBlock *prevBlock = block->prevFreeBlock;
	ManagedBlock *nextBlock = block->nextFreeBlock;

	if (firstFreeBlock[index] == block)
	{
		firstFreeBlock[index] = nextBlock;
	}
	
	if (prevBlock)
	{
		prevBlock->nextFreeBlock = nextBlock; 
	}
	
	if (nextBlock) 
	{
		nextBlock->prevFreeBlock = prevBlock; 
	}
}
 
inline void ManagedBuffer::UnregisterFreeBlock(ManagedBlock *block)
{ 
	UnregisterFreeBlock(block, GetFreeListIndex(block->blockSize)); 
}

ManagedBlock *ManagedBuffer::SplitBlock(ManagedBlock *block, unsigned_int32 size, unsigned_int32 index)
{ 
	#if C4DEBUG

		usedBlockCount++;
		usedBlockTotalSize += size;

	#endif

	UnregisterFreeBlock(block, index);

	unsigned_int32 blockSize = block->blockSize & ~kBlockFreeFlag;
	unsigned_int32 freeSize = blockSize - size;

	if (freeSize >= (1 << kBufferListMinLogSize))
	{
		ManagedBlock *freeBlock = firstDeadBlock;
		if (freeBlock)
		{
			firstDeadBlock = freeBlock->nextBlock;

			ManagedBlock *nextBlock = block->nextBlock;
			if (nextBlock)
			{
				nextBlock->prevBlock = freeBlock;
			}
			
			block->nextBlock = freeBlock;

			freeBlock->blockStart = block->blockStart + size;
			freeBlock->blockSize = freeSize | kBlockFreeFlag;
			freeBlock->prevBlock = block;
			freeBlock->nextBlock = nextBlock;
			RegisterFreeBlock(freeBlock);

			block->blockSize = size;
			return (block);
		}
	}

	block->blockSize = blockSize;
	return (block);
}

ManagedBlock *ManagedBuffer::NewBlock(unsigned_int32 size)
{
	size = (size + bufferAlignment) & ~bufferAlignment;
	unsigned_int32 index = GetFreeListIndex(size);

	ManagedBlock *freeBlock = firstFreeBlock[index];
	while (freeBlock)
	{
		if ((freeBlock->blockSize & ~kBlockFreeFlag) >= size)
		{
			return (SplitBlock(freeBlock, size, index));
		}
		
		freeBlock = freeBlock->nextFreeBlock;
	}

	for (unsigned_machine a = index + 1; a < kBufferFreeListCount; a++)
	{
		freeBlock = firstFreeBlock[a];
		if (freeBlock)
		{
			return (SplitBlock(freeBlock, size, a));
		}
	}

	return (nullptr);
}

void ManagedBuffer::ReleaseBlock(ManagedBlock *block)
{
	block->releaseCount = 2;
	block->nextReleasedBlock = nullptr;

	ManagedBlock *last = lastReleasedBlock;
	if (last)
	{
		last->nextReleasedBlock = block;
	}
	else
	{
		firstReleasedBlock = block;
	}

	lastReleasedBlock = block;
}

void ManagedBuffer::DestroyBlock(ManagedBlock *block)
{
	#if C4DEBUG

		usedBlockCount--;
		usedBlockTotalSize -= block->blockSize;

	#endif

	ManagedBlock *nextBlock = block->nextBlock;
	ManagedBlock *prevBlock = block->prevBlock;

	if ((nextBlock) && (nextBlock->blockSize & kBlockFreeFlag))
	{
		UnregisterFreeBlock(nextBlock);
		ManagedBlock *lastBlock = nextBlock->nextBlock;

		if ((prevBlock) && (prevBlock->blockSize & kBlockFreeFlag))
		{
			UnregisterFreeBlock(prevBlock);

			if (lastBlock)
			{
				lastBlock->prevBlock = prevBlock;
			}
			
			prevBlock->nextBlock = lastBlock;

			prevBlock->blockSize = (prevBlock->blockSize + block->blockSize + nextBlock->blockSize) | kBlockFreeFlag;
			RegisterFreeBlock(prevBlock);

			nextBlock->nextBlock = firstDeadBlock;
			block->nextBlock = nextBlock;
			firstDeadBlock = block;
		}
		else
		{
			if (lastBlock)
			{
				lastBlock->prevBlock = block;
			}
			
			block->nextBlock = lastBlock;

			block->blockSize = (block->blockSize + nextBlock->blockSize) | kBlockFreeFlag;
			RegisterFreeBlock(block);

			nextBlock->nextBlock = firstDeadBlock;
			firstDeadBlock = nextBlock;
		}
	}
	else if ((prevBlock) && (prevBlock->blockSize & kBlockFreeFlag))
	{
		UnregisterFreeBlock(prevBlock);

		if (nextBlock)
		{
			nextBlock->prevBlock = prevBlock;
		}
		
		prevBlock->nextBlock = nextBlock;

		prevBlock->blockSize = (prevBlock->blockSize + block->blockSize) | kBlockFreeFlag;
		RegisterFreeBlock(prevBlock);

		block->nextBlock = firstDeadBlock;
		firstDeadBlock = block;
	}
	else
	{
		block->blockSize |= kBlockFreeFlag;
		RegisterFreeBlock(block);
	}
}

void ManagedBuffer::BufferTask(void)
{
	ManagedBlock *last = nullptr;

	ManagedBlock *block = firstReleasedBlock;
	while (block)
	{
		ManagedBlock *next = block->nextReleasedBlock;

		int32 count = block->releaseCount - 1;
		if (count > 0)
		{
			block->releaseCount = count;
			last = block;
		}
		else
		{
			if (firstReleasedBlock == block)
			{
				firstReleasedBlock = next;
			}
			
			DestroyBlock(block);
		}

		block = next;
	}

	lastReleasedBlock = last;
}

// ZYUQURM
