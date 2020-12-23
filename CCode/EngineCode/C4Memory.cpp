 

#include "C4Memory.h"

#if C4LEAK_DETECTION

	#undef new

#endif

#include "C4Threads.h"


using namespace C4;


namespace
{
	enum
	{
		kMemoryGuardValue		= 0xAA,
		kMemoryInitializeValue	= 0x7FCCCCCC,
		kMemoryTerminateValue	= 0x7FDDDDDD
	};


	inline unsigned_machine GetFreeListIndex(unsigned_int32 size)
	{
		return (Min(MaxZero(31 - kMemoryListMinLogSize - Cntlz(size)), kMemoryFreeListCount - 1));
	}


	#if C4DEBUG_MEMORY

		void InitMemory(void *ptr, unsigned_int32 size, unsigned_int32 value)
		{
			unsigned_int32 count = size >> 2;
			unsigned_int32 *dst = static_cast<unsigned_int32 *>(ptr);
			for (unsigned_machine a = 0; a < count; a++)
			{
				*dst++ = value;
			}

			count = size & 3;
			unsigned_int8 *d = reinterpret_cast<unsigned_int8 *>(dst);
			for (unsigned_machine a = 0; a < count; a++)
			{
				d[a] = (unsigned_int8) value;
			}
		}

	#endif
}


Heap MemoryMgr::mainHeap("MemoryMgr");

Heap *MemoryMgr::firstHeap = nullptr;
Heap *MemoryMgr::lastHeap = nullptr;

#if C4LEAK_DETECTION

	const char *MemoryMgr::currentFile = nullptr;
	int32 MemoryMgr::currentLine = 0;

#endif

#if C4DEBUG_MEMORY || C4LEAK_DETECTION

	MemBlockHeader *MemoryMgr::firstSystemBlock = nullptr;
	MemBlockHeader *MemoryMgr::lastSystemBlock = nullptr;

	Mutex MemoryMgr::systemMutex;

#endif


#if C4DEBUG_MEMORY

	void MemBlockHeader::Initialize(void)
	{
		char *memory = GetMemory();

		unsigned_int8 *tailGuard = reinterpret_cast<unsigned_int8 *>(memory + logicalSize);
		for (machine a = 0; a < kMemoryGuardByteCount; a++)
		{
			headGuard[a] = kMemoryGuardValue;
			tailGuard[a] = kMemoryGuardValue;
		}

		InitMemory(memory, logicalSize, kMemoryInitializeValue);
	}

	void MemBlockHeader::Terminate(bool array)
	{
		if (blockFlags & kMemoryBlockUsed)
		{
			CheckGuards();
		}
		else
		{
			Fatal("MemBlockHeader::Terminate(), invalid memory block released\n");
		} 

		#if !C4MACOS
 
			// We can't check for operator consistency on the Mac because the OS itself uses the wrong operators.
 
			Assert(array ^ ((blockFlags & kMemoryBlockArray) == 0), "MemBlockHeader::Terminate(), wrong operator used to release memory\n");

		#endif 

		InitMemory(GetMemory(), logicalSize, kMemoryTerminateValue); 
	} 

	void MemBlockHeader::CheckGuards(void) const
	{
		const unsigned_int8 *tailGuard = reinterpret_cast<const unsigned_int8 *>(this + 1) + logicalSize; 
		for (machine a = 0; a < kMemoryGuardByteCount; a++)
		{
			Assert(headGuard[a] == kMemoryGuardValue, "MemBlockHeader::CheckGuards(), memory block underwrite detected\n");
			Assert(tailGuard[a] == kMemoryGuardValue, "MemBlockHeader::CheckGuards(), memory block overwrite detected\n");
		}
	}

#endif


Heap::Heap(const char *name, unsigned_int32 pool, unsigned_int32 flags)
{
	heapName = name;
	heapFlags = flags;

	totalSize = 0;
	maxBlockSize = pool >> 2;
	poolSize = pool;

	nextHeap = nullptr;
	firstPool = nullptr;
	for (machine a = 0; a < kMemoryFreeListCount; a++)
	{
		firstFreeBlock[a] = nullptr;
	}

	Heap *heap = MemoryMgr::lastHeap;
	if (heap)
	{
		heap->nextHeap = this;
	}
	else
	{
		MemoryMgr::firstHeap = this;
	}

	MemoryMgr::lastHeap = this;
}

Heap::~Heap()
{
}

void Heap::RegisterFreeBlock(MemBlockHeader *block)
{
	unsigned_machine index = GetFreeListIndex(block->physicalSize);
	MemBlockHeader *firstBlock = firstFreeBlock[index];
	if (firstBlock)
	{
		firstBlock->prevFreeBlock = block;
	}

	block->prevFreeBlock = nullptr;
	block->nextFreeBlock = firstBlock;
	firstFreeBlock[index] = block;
}

void Heap::UnregisterFreeBlock(MemBlockHeader *block, unsigned_machine index)
{
	MemBlockHeader *prevBlock = block->prevFreeBlock;
	MemBlockHeader *nextBlock = block->nextFreeBlock;

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

inline void Heap::UnregisterFreeBlock(MemBlockHeader *block)
{
	UnregisterFreeBlock(block, GetFreeListIndex(block->physicalSize));
}

MemPoolHeader *Heap::NewMemPool(unsigned_int32 blockSize, unsigned_int32 flags)
{
	MemPoolHeader *pool = reinterpret_cast<MemPoolHeader *>(MemoryMgr::SystemNew(poolSize));
	pool->owningHeap = this;
	pool->prevPool = nullptr;
	pool->blockCount = 2;

	if (firstPool)
	{
		firstPool->prevPool = pool;
	}

	pool->nextPool = firstPool;
	firstPool = pool;

	unsigned_int32 physical = MemoryMgr::GetPhysicalSize(blockSize);
	unsigned_int32 freeSize = poolSize - sizeof(MemPoolHeader) - 2 * sizeof(MemBlockHeader) - physical;

	MemBlockHeader *usedBlock = pool->GetFirstBlock();
	MemBlockHeader *freeBlock = reinterpret_cast<MemBlockHeader *>(usedBlock->GetMemory() + physical);

	usedBlock->owningPool = pool;
	usedBlock->prevBlock = nullptr;
	usedBlock->nextBlock = freeBlock;
	usedBlock->blockFlags = flags | kMemoryBlockUsed;
	usedBlock->logicalSize = blockSize;
	usedBlock->physicalSize = physical;
	usedBlock->Initialize();

	freeBlock->owningPool = pool;
	freeBlock->prevBlock = usedBlock;
	freeBlock->nextBlock = nullptr;
	freeBlock->blockFlags = 0;
	freeBlock->logicalSize = freeSize;
	freeBlock->physicalSize = freeSize;
	RegisterFreeBlock(freeBlock);

	return (pool);
}

void Heap::ReleaseMemPool(MemPoolHeader *pool)
{
	MemPoolHeader *prevPool = pool->prevPool;
	MemPoolHeader *nextPool = pool->nextPool;

	if (firstPool == pool)
	{
		firstPool = nextPool;
	}

	if (prevPool)
	{
		prevPool->nextPool = nextPool;
	}

	if (nextPool)
	{
		nextPool->prevPool = prevPool;
	}

	MemoryMgr::SystemRelease(pool);
}

MemBlockHeader *Heap::SplitMemBlock(MemBlockHeader *block, unsigned_int32 logicalSize, unsigned_int32 physicalSize, unsigned_int32 flags, unsigned_machine index)
{
	UnregisterFreeBlock(block, index);

	unsigned_int32 freeSize = block->physicalSize - physicalSize - sizeof(MemBlockHeader);
	if (freeSize >= (1 << kMemoryListMinLogSize))
	{
		MemPoolHeader *pool = block->owningPool;
		pool->blockCount++;

		MemBlockHeader *freeBlock = reinterpret_cast<MemBlockHeader *>(block->GetMemory() + physicalSize);
		MemBlockHeader *nextBlock = block->nextBlock;
		if (nextBlock)
		{
			nextBlock->prevBlock = freeBlock;
		}

		freeBlock->owningPool = pool;
		freeBlock->prevBlock = block;
		freeBlock->nextBlock = nextBlock;
		freeBlock->blockFlags = 0;
		freeBlock->logicalSize = freeSize;
		freeBlock->physicalSize = freeSize;
		RegisterFreeBlock(freeBlock);

		block->nextBlock = freeBlock;
		block->physicalSize = physicalSize;
	}

	block->blockFlags = flags | kMemoryBlockUsed;
	block->logicalSize = logicalSize;
	block->Initialize();

	return (block);
}

MemBlockHeader *Heap::NewMemBlock(unsigned_int32 logicalSize, unsigned_int32 flags)
{
	MemBlockHeader		*newBlock;
	MemBlockHeader		*freeBlock;

	if (logicalSize > maxBlockSize)
	{
		return (MemoryMgr::NewSystemBlock(logicalSize, flags));
	}

	unsigned_int32 physicalSize = MemoryMgr::GetPhysicalSize(logicalSize);
	unsigned_int32 combinedSize = physicalSize + sizeof(MemBlockHeader);
	unsigned_machine index = GetFreeListIndex(combinedSize);

	if (!(heapFlags & kHeapMutexless))
	{
		heapMutex.Acquire();
	}

	for (unsigned_machine a = kMemoryFreeListCount - 1; a > index; a--)
	{
		freeBlock = firstFreeBlock[a];
		if (freeBlock)
		{
			newBlock = SplitMemBlock(freeBlock, logicalSize, physicalSize, flags, a);
			goto end;
		}
	}

	freeBlock = firstFreeBlock[index];
	while (freeBlock)
	{
		if (freeBlock->physicalSize >= combinedSize)
		{
			newBlock = SplitMemBlock(freeBlock, logicalSize, physicalSize, flags, index);
			goto end;
		}

		freeBlock = freeBlock->nextFreeBlock;
	}

	newBlock = NewMemPool(logicalSize, flags)->GetFirstBlock();

	end:
	totalSize += logicalSize;

	if (!(heapFlags & kHeapMutexless))
	{
		heapMutex.Release();
	}

	return (newBlock);
}

void Heap::ReleaseMemBlock(MemBlockHeader *block, bool array)
{
	block->Terminate(array);

	if (block->blockFlags & kMemoryBlockSystem)
	{
		MemoryMgr::ReleaseSystemBlock(block);
		return;
	}

	MemPoolHeader *pool = block->owningPool;
	Heap *heap = pool->owningHeap;

	if (!(heap->heapFlags & kHeapMutexless))
	{
		heap->heapMutex.Acquire();
	}

	heap->totalSize -= block->logicalSize;

	MemBlockHeader *nextBlock = block->nextBlock;
	MemBlockHeader *prevBlock = block->prevBlock;

	if ((nextBlock) && (nextBlock->blockFlags == 0))
	{
		heap->UnregisterFreeBlock(nextBlock);
		MemBlockHeader *lastBlock = nextBlock->nextBlock;

		if ((prevBlock) && (prevBlock->blockFlags == 0))
		{
			heap->UnregisterFreeBlock(prevBlock);
			if (((pool->blockCount -= 2) == 1) && (!pool->OnlyPoolAllocated()))
			{
				heap->ReleaseMemPool(pool);
			}
			else
			{
				if (lastBlock)
				{
					lastBlock->prevBlock = prevBlock;
				}

				prevBlock->nextBlock = lastBlock;

				unsigned_int32 size = prevBlock->physicalSize + block->physicalSize + nextBlock->physicalSize + sizeof(MemBlockHeader) * 2;
				prevBlock->logicalSize = size;
				prevBlock->physicalSize = size;

				heap->RegisterFreeBlock(prevBlock);
			}
		}
		else
		{
			if ((--pool->blockCount == 1) && (!pool->OnlyPoolAllocated()))
			{
				heap->ReleaseMemPool(pool);
			}
			else
			{
				if (lastBlock)
				{
					lastBlock->prevBlock = block;
				}

				block->nextBlock = lastBlock;

				unsigned_int32 size = block->physicalSize + nextBlock->physicalSize + sizeof(MemBlockHeader);
				block->logicalSize = size;
				block->physicalSize = size;

				block->blockFlags = 0;
				heap->RegisterFreeBlock(block);
			}
		}
	}
	else if ((prevBlock) && (prevBlock->blockFlags == 0))
	{
		heap->UnregisterFreeBlock(prevBlock);
		if ((--pool->blockCount == 1) && (!pool->OnlyPoolAllocated()))
		{
			heap->ReleaseMemPool(pool);
		}
		else
		{
			if (nextBlock)
			{
				nextBlock->prevBlock = prevBlock;
			}

			prevBlock->nextBlock = nextBlock;

			unsigned_int32 size = prevBlock->physicalSize + block->physicalSize + sizeof(MemBlockHeader);
			prevBlock->logicalSize = size;
			prevBlock->physicalSize = size;

			heap->RegisterFreeBlock(prevBlock);
		}
	}
	else
	{
		block->blockFlags = 0;
		block->logicalSize = block->physicalSize;
		heap->RegisterFreeBlock(block);
	}

	if (!(heap->heapFlags & kHeapMutexless))
	{
		heap->heapMutex.Release();
	}
}


MemBlockHeader *MemoryMgr::NewSystemBlock(unsigned_int32 size, unsigned_int32 flags)
{
	unsigned_int32 physical = GetPhysicalSize(size);

	MemBlockHeader *block = reinterpret_cast<MemBlockHeader *>(MemoryMgr::SystemNew(physical + sizeof(MemBlockHeader)));
	block->owningPool = nullptr;
	block->blockFlags = flags | (kMemoryBlockUsed | kMemoryBlockSystem);
	block->logicalSize = size;
	block->physicalSize = physical;

	#if C4DEBUG_MEMORY || C4LEAK_DETECTION

		systemMutex.Acquire();

		if (lastSystemBlock)
		{
			block->prevBlock = lastSystemBlock;
			block->nextBlock = nullptr;
			lastSystemBlock->nextBlock = block;
			lastSystemBlock = block;
		}
		else
		{
			block->prevBlock = nullptr;
			block->nextBlock = nullptr;
			firstSystemBlock = block;
			lastSystemBlock = block;
		}

		systemMutex.Release();

		block->Initialize();

	#endif

	return (block);
}

void MemoryMgr::ReleaseSystemBlock(MemBlockHeader *block)
{
	#if C4DEBUG_MEMORY || C4LEAK_DETECTION

		systemMutex.Acquire();

		MemBlockHeader *prevBlock = block->prevBlock;
		MemBlockHeader *nextBlock = block->nextBlock;

		if (prevBlock)
		{
			prevBlock->nextBlock = nextBlock;
		}

		if (nextBlock)
		{
			nextBlock->prevBlock = prevBlock;
		}

		if (block == lastSystemBlock)
		{
			lastSystemBlock = prevBlock;
		}

		if (block == firstSystemBlock)
		{
			firstSystemBlock = nextBlock;
		}

		systemMutex.Release();

	#endif

	MemoryMgr::SystemRelease(block);
}

#if C4LEAK_DETECTION

	bool MemoryMgr::SetCurrentLocation(const char *file, int32 line)
	{
		currentFile = file;
		currentLine = line;
		return (false);
	}

#endif


void *operator new(std::size_t size)
{
	C4::MemBlockHeader *block = C4::MemoryMgr::GetMainHeap()->NewMemBlock((unsigned_int32) size, 0);

	#if C4LEAK_DETECTION

		block->allocFile = C4::MemoryMgr::GetCurrentFile();
		block->allocLine = C4::MemoryMgr::GetCurrentLine();

	#endif

	return (block->GetMemory());
}

void *operator new[](std::size_t size)
{
	C4::MemBlockHeader *block = C4::MemoryMgr::GetMainHeap()->NewMemBlock((unsigned_int32) size, C4::kMemoryBlockArray);

	#if C4LEAK_DETECTION

		block->allocFile = C4::MemoryMgr::GetCurrentFile();
		block->allocLine = C4::MemoryMgr::GetCurrentLine();

	#endif

	return (block->GetMemory());
}

void operator delete(void *ptr) C4THROW
{
	if (ptr)
	{
		C4::Heap::ReleaseMemBlock(static_cast<C4::MemBlockHeader *>(ptr) - 1, false);
	}
}

void operator delete[](void *ptr) C4THROW
{
	if (ptr)
	{
		C4::Heap::ReleaseMemBlock(static_cast<C4::MemBlockHeader *>(ptr) - 1, true);
	}
}

// ZYUQURM
