 

#ifndef C4Memory_h
#define C4Memory_h


//# \component	Memory Manager
//# \prefix		MemoryMgr/


#include "C4Base.h"


namespace C4
{
	enum
	{
		kMemoryDefaultPoolSize	= 262144,
		kMemoryAlignment		= 16,
		kMemoryAlignMask		= kMemoryAlignment - 1,
		kMemorySafetySize		= 12,
		kMemoryGuardByteCount	= 16
	};


	enum
	{
		kMemoryListMinLogSize	= 4,
		kMemoryListMaxLogSize	= 16,
		kMemoryFreeListCount	= kMemoryListMaxLogSize - kMemoryListMinLogSize + 1
	};


	enum
	{
		kMemoryBlockUsed		= 1 << 0,
		kMemoryBlockSystem		= 1 << 1,
		kMemoryBlockArray		= 1 << 2
	};


	//# \enum	HeapFlags

	enum
	{
		kHeapMutexless			= 1 << 0		//## Do not use a mutex to protect the heap from simultaneous access from multiple threads.
	};


	class Heap;
	struct MemPoolHeader;


	struct MemBlockHeader
	{
		MemPoolHeader		*owningPool;
		MemBlockHeader		*prevBlock;
		MemBlockHeader		*nextBlock;
		MemBlockHeader		*prevFreeBlock;
		MemBlockHeader		*nextFreeBlock;

		unsigned_int32		blockFlags;
		unsigned_int32		logicalSize;
		unsigned_int32		physicalSize;

		#if C4PTR64

			int32			reserved1[3];

		#endif

		#if C4LEAK_DETECTION

			const char		*allocFile;
			int32			allocLine;

			#if C4PTR64

				int32		reserved2;

			#else

				int32		reserved2[2];

			#endif

		#endif

		#if C4DEBUG_MEMORY

			unsigned_int8	headGuard[kMemoryGuardByteCount];

			void Initialize(void);
			void Terminate(bool array);
			void CheckGuards(void) const;

		#else

			void Initialize(void)
			{
			}

			void Terminate(bool) 
			{
			}
 
		#endif
 
		char *GetMemory(void)
		{
			return (reinterpret_cast<char *>(this + 1)); 
		}
 
		const char *GetMemory(void) const 
		{
			return (reinterpret_cast<const char *>(this + 1));
		}
	}; 


	struct MemPoolHeader
	{
		Heap				*owningHeap;
		MemPoolHeader		*prevPool;
		MemPoolHeader		*nextPool;
		unsigned_int32		blockCount;

		#if C4PTR64

			int32			reserved;

		#endif

		MemBlockHeader *GetFirstBlock(void)
		{
			return (reinterpret_cast<MemBlockHeader *>(this + 1));
		}

		const MemBlockHeader *GetFirstBlock(void) const
		{
			return (reinterpret_cast<const MemBlockHeader *>(this + 1));
		}

		bool OnlyPoolAllocated(void) const
		{
			return (nextPool - prevPool == 0);
		}
	};


	static_assert((sizeof(MemBlockHeader) & 15) == 0, "sizeof(MemBlockHeader) must be a multiple of 16");
	static_assert((sizeof(MemPoolHeader) & 15) == 0, "sizeof(MemPoolHeader) must be a multiple of 16");


	//# \class	Heap		Encapsulates a memory allocation heap.
	//
	//# The $Heap$ class encapsulates a memory allocation heap.
	//
	//# \def	class Heap
	//
	//# \param	name	The name of the heap.
	//# \param	pool	The size of allocation pools allocated in the heap.
	//
	//# \ctor	Heap(const char *name = nullptr, unsigned_int32 pool = kMemoryDefaultPoolSize, unsigned_int32 flags = 0);
	//
	//# \param	name	The name of the heap.
	//# \param	pool	The size of the memory pools allocated in the heap, in bytes.
	//# \param	flags	The heap flags. See below for possible values.
	//
	//# \desc
	//# The $Heap$ class organizes a set of memory pools from which blocks of memory are allocated by the
	//# Memory Manager. Heaps are generally managed internally by the engine, but it is possible to create
	//# new dedicated heaps for custom class types by subclassing from the $@Memory@$ class.
	//#
	//# The $flags$ parameter can be a combination (through logical OR) of the following constants.
	//
	//# \table	HeapFlags
	//
	//# \also	$@Memory@$
	//# \also	$@MemoryMgr::CalculatePoolSize@$


	//# \function	Heap::GetTotalSize		Returns the total size of allocated blocks in a heap.
	//
	//# \proto	unsigned_int32 GetTotalSize(void) const;
	//
	//# \desc
	//# The $GetTotalSize$ function returns the total number of bytes allocated by a heap. This size
	//# does not include per-block space overhead needed by the Memory Manager or any block padding.


	class Heap
	{
		friend class MemoryMgr;

		private:

			const char			*heapName;

			Heap				*nextHeap;
			MemPoolHeader		*firstPool;
			MemBlockHeader		*firstFreeBlock[kMemoryFreeListCount];

			unsigned_int32		totalSize;
			unsigned_int32		maxBlockSize;
			unsigned_int32		poolSize;

			unsigned_int32		heapFlags;
			Mutex				heapMutex;

			void RegisterFreeBlock(MemBlockHeader *block);
			void UnregisterFreeBlock(MemBlockHeader *block, unsigned_machine index);
			void UnregisterFreeBlock(MemBlockHeader *block);

			MemPoolHeader *NewMemPool(unsigned_int32 blockSize, unsigned_int32 flags);
			void ReleaseMemPool(MemPoolHeader *pool);

			MemBlockHeader *SplitMemBlock(MemBlockHeader *block, unsigned_int32 logicalSize, unsigned_int32 physicalSize, unsigned_int32 flags, unsigned_machine index);

		public:

			C4API Heap(const char *name = nullptr, unsigned_int32 pool = kMemoryDefaultPoolSize, unsigned_int32 flags = 0);
			C4API ~Heap();

			const char *GetHeapName(void) const
			{
				return (heapName);
			}

			const Heap *GetNextHeap(void) const
			{
				return (nextHeap);
			}

			const MemPoolHeader *GetFirstPool(void) const
			{
				return (firstPool);
			}

			unsigned_int32 GetTotalSize(void) const
			{
				return (totalSize);
			}

			template <typename type> type *New(unsigned_int32 size)
			{
				MemBlockHeader *bh = NewMemBlock(size, 0);

				#if C4LEAK_DETECTION

					bh->allocFile = "";
					bh->allocLine = 0;

				#endif

				return (reinterpret_cast<type *>(bh + 1));
			}

			void Delete(void *ptr)
			{
				if (ptr)
				{
					ReleaseMemBlock(static_cast<MemBlockHeader *>(ptr) - 1, false);
				}
			}

			static unsigned_int32 GetMemBlockSize(const void *ptr)
			{
				return ((reinterpret_cast<const MemBlockHeader *>(ptr) - 1)->logicalSize);
			}

			C4API MemBlockHeader *NewMemBlock(unsigned_int32 logicalSize, unsigned_int32 flags);
			C4API static void ReleaseMemBlock(MemBlockHeader *block, bool array);
	};


	//# \class	MemoryMgr	The Memory Manager class.
	//
	//# \def	class MemoryMgr
	//
	//# \desc
	//# The $MemoryMgr$ class is responsible for handling memory allocation and deallocation. The Memory
	//# Manager supplies custom overrides for the $new$ and $delete$ operators that are optimized for
	//# typical allocation patterns arising in games and virtual reality software.
	//
	//# \also	$@Memory@$


	//# \function	MemoryMgr::CopyMemory		Copies a block of memory.
	//
	//# \proto	static void CopyMemory(const void *source, void *dest, unsigned_int32 size);
	//
	//# \param	source		A pointer to the beginning of the source.
	//# \param	dest		A pointer to the beginning of the destination.
	//# \param	size		The number of bytes to copy.
	//
	//# \desc
	//# The $CopyMemory$ function copies the number of bytes specified by the $size$ parameter from the
	//# memory location specified by the $source$ parameter to the memory location specified by the $dest$
	//# parameter.
	//#
	//# If the $source$ and $dest$ pointers are not separated by at least $size$ bytes, then the result of
	//# the copy operation is undefined. That is, the source and destination memory buffers should not overlap.
	//
	//# \also	$@MemoryMgr::FillMemory@$
	//# \also	$@MemoryMgr::ClearMemory@$


	//# \function	MemoryMgr::FillMemory		Fills a block of memory with an 8-bit value.
	//
	//# \proto	static void FillMemory(void *ptr, unsigned_int32 size, unsigned_int8 value);
	//
	//# \param	ptr			A pointer to the beginning of the block to fill.
	//# \param	size		The number of bytes to fill.
	//# \param	value		The value to which each byte will be set.
	//
	//# \desc
	//# The $FillMemory$ function sets $size$ bytes of memory beginning at the pointer $ptr$ to the 8-bit value
	//# specified by the $value$ parameter.
	//
	//# \also	$@MemoryMgr::ClearMemory@$
	//# \also	$@MemoryMgr::CopyMemory@$


	//# \function	MemoryMgr::ClearMemory		Clears a block of memory to zero.
	//
	//# \proto	static void ClearMemory(void *ptr, unsigned_int32 size);
	//
	//# \param	ptr			A pointer to the beginning of the block to clear.
	//# \param	size		The number of bytes to clear.
	//
	//# \desc
	//# The $ClearMemory$ function sets $size$ bytes of memory beginning at the pointer $ptr$ to the value zero.
	//
	//# \also	$@MemoryMgr::FillMemory@$
	//# \also	$@MemoryMgr::CopyMemory@$


	//# \function	MemoryMgr::CalculatePoolSize		Calculates the size that a pool needs to be in order to
	//#													store a given number of equal-size blocks.
	//
	//# \proto	static unsigned_int32 CalculatePoolSize(int32 blockCount, unsigned_int32 size);
	//
	//# \param	blockCount		The number of blocks that can be stored in each pool.
	//# \param	size			The size of each block, in bytes.
	//
	//# \desc
	//# The $CalculatePoolSize$ function returns the minimum size that an allocation pool must have in order to
	//# accomodate $blockCount$ allocations of the size specified by the $size$ parameter. The return value
	//# accounts for the space overhead needed by the Memory Manager for each block and the overhead needed for
	//# the pool itself. This function is useful for calculating the pool size passed to the constructor for a
	//# dedicated heap. (See the $@Heap@$ class.)
	//
	//# \also	$@Memory@$
	//# \also	$@Heap@$


	class MemoryMgr
	{
		friend class Heap;

		private:

			static C4API Heap			mainHeap;

			static Heap					*firstHeap;
			static Heap					*lastHeap;

			#if C4LEAK_DETECTION

				static int32				allocationIndex;

				static C4API const char		*currentFile;
				static C4API int32			currentLine;

			#endif

			#if C4DEBUG_MEMORY || C4LEAK_DETECTION

				static MemBlockHeader		*firstSystemBlock;
				static MemBlockHeader		*lastSystemBlock;

				static Mutex				systemMutex;

			#endif

			#if C4DEBUG_MEMORY

				static unsigned_int32 GetPhysicalSize(unsigned_int32 size)
				{
					return ((size + kMemoryGuardByteCount + kMemoryAlignMask) & ~kMemoryAlignMask);
				}

			#else

				static unsigned_int32 GetPhysicalSize(unsigned_int32 size)
				{
					return ((size + kMemoryAlignMask) & ~kMemoryAlignMask);
				}

			#endif

			static void *SystemNew(unsigned_int32 size)
			{
				#if C4WINDOWS

					char *ptr = static_cast<char *>(HeapAlloc(GetProcessHeap(), 0, size + (kMemoryAlignment + kMemorySafetySize)));
					Assert(ptr != nullptr, "MemoryMgr::SystemNew(), out of memory\n");

					unsigned_int32 offset = kMemoryAlignment - (GetPointerAddress(ptr) & kMemoryAlignMask);
					ptr[offset - 1] = (char) offset;
					return (ptr + offset);

				#elif C4MACOS || C4IOS

					void *ptr = malloc(size + kMemorySafetySize);
					Assert(ptr != nullptr, "MemoryMgr::SystemNew(), out of memory\n");
					return (ptr);

				#elif C4LINUX

					char *ptr = static_cast<char *>(malloc(size + (kMemoryAlignment + kMemorySafetySize)));
					Assert(ptr != nullptr, "MemoryMgr::SystemNew(), out of memory\n");

					unsigned_int32 offset = kMemoryAlignment - (GetPointerAddress(ptr) & kMemoryAlignMask);
					ptr[offset - 1] = (char) offset;
					return (ptr + offset);

				#elif C4CONSOLE //[ CONSOLE

					// -- Console code hidden --

				#endif //]
			}

			static void SystemRelease(void *ptr)
			{
				#if C4WINDOWS

					char *p = static_cast<char *>(ptr);
					HeapFree(GetProcessHeap(), 0, p - p[-1]);

				#elif C4MACOS || C4IOS

					free(ptr);

				#elif C4LINUX

					char *p = static_cast<char *>(ptr);
					free(p - p[-1]);

				#elif C4CONSOLE //[ CONSOLE

					// -- Console code hidden --

				#endif //]
			}

			static MemBlockHeader *NewSystemBlock(unsigned_int32 size, unsigned_int32 flags);
			static void ReleaseSystemBlock(MemBlockHeader *bh);

		public:

			static Heap *GetMainHeap(void)
			{
				return (&mainHeap);
			}

			static const Heap *GetFirstHeap(void)
			{
				return (firstHeap);
			}

			static const Heap *GetLastHeap(void)
			{
				return (lastHeap);
			}

			#if C4DEBUG_MEMORY || C4LEAK_DETECTION

				static const MemBlockHeader *GetFirstSystemBlock(void)
				{
					return (firstSystemBlock);
				}

			#endif

			#if C4LEAK_DETECTION

				static const char *GetCurrentFile(void)
				{
					return (currentFile);
				}

				static int32 GetCurrentLine(void)
				{
					return (currentLine);
				}

				C4API static bool SetCurrentLocation(const char *file, int32 line);

			#endif

			static unsigned_int32 CalculatePoolSize(int32 blockCount, unsigned_int32 size)
			{
				return (sizeof(MemPoolHeader) + (GetPhysicalSize(size) + sizeof(MemBlockHeader)) * blockCount);
			}

			static void CopyMemory(const void *source, void *dest, unsigned_int32 size)
			{
				memcpy(dest, source, size);
			}

			static void FillMemory(void *ptr, unsigned_int32 size, unsigned_int8 value)
			{
				memset(ptr, value, size);
			}

			static void ClearMemory(void *ptr, unsigned_int32 size)
			{
				memset(ptr, 0, size);
			}

			static void LockMemory(const void *ptr, unsigned_int32 size)
			{
				#if C4WINDOWS

					VirtualLock(const_cast<void *>(ptr), size);

				#elif C4MACOS || C4LINUX

					mlock(ptr, size);

				#endif
			}

			static void UnlockMemory(const void *ptr, unsigned_int32 size)
			{
				#if C4WINDOWS

					VirtualUnlock(const_cast<void *>(ptr), size);

				#elif C4MACOS || C4LINUX

					munlock(ptr, size);

				#endif
			}
	};


	//# \class	Memory		Used to cause objects to be allocated in a dedicated heap.
	//
	//# The $Memory$ class template is used to cause objects to be allocated in a dedicated heap.
	//
	//# \def	template <class type> class Memory
	//
	//# \tparam		type		The type of object with which the dedicated heap is associated.
	//
	//# \ctor	Memory();
	//
	//# The constructor has protected access. The $Memory$ class can only exist as a base class for another class type.
	//
	//# \desc
	//# The $Memory$ class template is used as a base class for objects that are to be allocated in a dedicated heap.
	//# As an example, if all instances of a class $Foo$ should be allocated in a dedicated heap for that class, then
	//# $Foo$ should inherit from $Memory<Foo>$ as follows.
	//
	//# \source
	//# class Foo : public Memory<Foo>
	//
	//# \desc
	//# This causes instances of the class $Foo$ to be allocated in the heap associated with the $Memory<Foo>$ base class
	//# whenever the $new$ operator is used to create them. The heap itself needs to be defined in the program as follows.
	//
	//# \source
	//# template <> Heap C4::Memory<Foo>::heap("HeapName", poolSize, flags);
	//
	//# \desc
	//# (See the constructor for the $@Heap@$ class.)
	//
	//# \also	$@Heap@$


	template <class type> class EngineMemory
	{
		private:

			static C4API Heap	heap;

		protected:

			EngineMemory() {}
			~EngineMemory() {}

		public:

			void *operator new(std::size_t size)
			{
				MemBlockHeader *bh = heap.NewMemBlock((unsigned_int32) size, 0);

				#if C4LEAK_DETECTION

					bh->allocFile = MemoryMgr::GetCurrentFile();
					bh->allocLine = MemoryMgr::GetCurrentLine();

				#endif

				return (bh + 1);
			}

			void *operator new[](std::size_t size)
			{
				MemBlockHeader *bh = heap.NewMemBlock((unsigned_int32) size, kMemoryBlockArray);

				#if C4LEAK_DETECTION

					bh->allocFile = MemoryMgr::GetCurrentFile();
					bh->allocLine = MemoryMgr::GetCurrentLine();

				#endif

				return (bh + 1);
			}

			void operator delete(void *ptr)
			{
				if (ptr)
				{
					Heap::ReleaseMemBlock(static_cast<MemBlockHeader *>(ptr) - 1, false);
				}
			}

			void operator delete[](void *ptr)
			{
				if (ptr)
				{
					Heap::ReleaseMemBlock(static_cast<MemBlockHeader *>(ptr) - 1, true);
				}
			}

			void *operator new(std::size_t, void *ptr)
			{
				return (ptr);
			}

			void *operator new[](std::size_t, void *ptr)
			{
				return (ptr);
			}

			void operator delete(void *, void *)
			{
			}

			void operator delete[](void *, void *)
			{
			}
	};


	template <class type> class Memory
	{
		private:

			static Heap		heap;

		protected:

			Memory() {}
			~Memory() {}

		public:

			static void *operator new(std::size_t size)
			{
				MemBlockHeader *bh = heap.NewMemBlock((unsigned_int32) size, 0);

				#if C4LEAK_DETECTION

					bh->allocFile = MemoryMgr::GetCurrentFile();
					bh->allocLine = MemoryMgr::GetCurrentLine();

				#endif

				return (bh + 1);
			}

			static void *operator new[](std::size_t size)
			{
				MemBlockHeader *bh = heap.NewMemBlock((unsigned_int32) size, kMemoryBlockArray);

				#if C4LEAK_DETECTION

					bh->allocFile = MemoryMgr::GetCurrentFile();
					bh->allocLine = MemoryMgr::GetCurrentLine();

				#endif

				return (bh + 1);
			}

			static void operator delete(void *ptr)
			{
				if (ptr)
				{
					Heap::ReleaseMemBlock(static_cast<MemBlockHeader *>(ptr) - 1, false);
				}
			}

			static void operator delete[](void *ptr)
			{
				if (ptr)
				{
					Heap::ReleaseMemBlock(static_cast<MemBlockHeader *>(ptr) - 1, true);
				}
			}

			static void *operator new(std::size_t, void *ptr)
			{
				return (ptr);
			}

			static void *operator new[](std::size_t, void *ptr)
			{
				return (ptr);
			}

			static void operator delete(void *, void *)
			{
			}

			static void operator delete[](void *, void *)
			{
			}
	};
}


void *operator new(std::size_t size);
void *operator new[](std::size_t size);
void operator delete(void *ptr) C4THROW;
void operator delete[](void *ptr) C4THROW;


#if C4LEAK_DETECTION

	#define new C4::MemoryMgr::SetCurrentLocation(__FILE__, __LINE__) ? nullptr : new

#endif


#endif

// ZYUQURM
