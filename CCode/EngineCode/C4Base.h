 

#ifndef C4Base_h
#define C4Base_h


//# \component	System Utilities
//# \prefix		System/


#include "C4Defines.h"

#if C4SIMD

	#include "C4Simd.h"

#endif


namespace C4
{
	enum
	{
		kEngineInternalVersion	= 74
	};


	typedef unsigned_int32	EngineResult;


	enum : EngineResult
	{
		kEngineOkay				= 0
	};


	enum
	{
		kManagerEngine			= 'C4',
		kManagerTime			= 'TM',
		kManagerFile			= 'FL',
		kManagerResource		= 'RS',
		kManagerInput			= 'IN',
		kManagerDisplay			= 'DS',
		kManagerGraphics		= 'GR',
		kManagerSound			= 'SD',
		kManagerAudioCapture	= 'AC',
		kManagerInterface		= 'IF',
		kManagerMovie			= 'MV',
		kManagerNetwork			= 'NW',
		kManagerMessage			= 'MG',
		kManagerWorld			= 'WD',
		kManagerPlugin			= 'PL'
	};


	inline unsigned_int32 GetResultManager(EngineResult result)
	{
		return (result >> 16);
	}

	inline unsigned_int32 GetResultCode(EngineResult result)
	{
		return (result & 0xFFFF);
	}


	#if C4DEBUG

		C4API void Fatal(const char *c);
		C4API void Assert(bool b, const char *c);

	#else

		inline void Fatal(const char *c)
		{
		}

		#if C4VISUALC

			#define Assert(b, c) __assume(b)

		#else

			inline void Assert(bool b, const char *c)
			{
			}

		#endif

	#endif


	inline int32 Abs(int32 x)
	{
		int32 a = x >> 31;
		return ((x ^ a) - a);
	}

	inline int64 Abs64(int64 x)
	{
		int64 a = x >> 63;
		return ((x ^ a) - a); 
	}

	inline int32 Sgn(int32 x) 
	{
		return ((x >> 31) - (-x >> 31)); 
	}

	inline int64 Sgn64(int64 x) 
	{
		return ((x >> 63) - (-x >> 63)); 
	} 

	inline int32 Min(int32 x, int32 y)
	{
		int32 a = x - y; 
		return (x - (a & ~(a >> 31)));
	}

	inline int64 Min64(int64 x, int64 y)
	{
		int64 a = x - y;
		return (x - (a & ~(a >> 63)));
	}

	inline int32 Max(int32 x, int32 y)
	{
		int32 a = x - y;
		return (x - (a & (a >> 31)));
	}

	inline int64 Max64(int64 x, int64 y)
	{
		int64 a = x - y;
		return (x - (a & (a >> 63)));
	}

	inline int32 MinZero(int32 x)
	{
		return (x & (x >> 31));
	}

	inline int64 MinZero64(int64 x)
	{
		return (x & (x >> 63));
	}

	inline int32 MaxZero(int32 x)
	{
		return (x & ~(x >> 31));
	}

	inline int64 MaxZero64(int64 x)
	{
		return (x & ~(x >> 63));
	}

	template <int32 mod> inline int32 IncMod(int32 x)
	{
		return ((x + 1) & ((x - (mod - 1)) >> 31));
	}

	template <int32 mod> inline int32 DecMod(int32 x)
	{
		x--;
		return (x + ((x >> 31) & mod));
	}

	inline int32 OverflowZero(int32 x, int32 y)
	{
		return (x & ((x - y) >> 31));
	}

	template <typename type> inline void Exchange(type& x, type& y)
	{
		x ^= y;
		y ^= x;
		x ^= y;
	}

	template <> inline void Exchange<float>(float& x, float& y)
	{
		float f = x;
		x = y;
		y = f;
	}


	#if C4VISUALC

		inline int32 Cntlz(unsigned long n)
		{
			unsigned long	x;

			if (_BitScanReverse(&x, n) == 0)
			{
				return (32);
			}

			return (31 - x);
		}

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#else //]

		inline int32 Cntlz(unsigned_int32 n)
		{
			return ((n != 0) ? __builtin_clz(n) : 32);
		}

	#endif


	inline unsigned_int32 Power2Floor(unsigned_int32 n)
	{
		return (0x80000000U >> Cntlz(n));
	}

	inline unsigned_int32 Power2Ceil(unsigned_int32 n)
	{
		return ((unsigned_int32) (1 << (32 - Cntlz(n - 1))));
	}


	inline int16 ReadLittleEndianS16(const int16 *ptr)
	{
		#if C4LITTLEENDIAN

			return (*ptr);

		#elif C4POWERPC

			return ((int16) __lhbrx((void *) ptr));

		#else

			const int8 *s = reinterpret_cast<const int8 *>(ptr);
			const unsigned_int8 *u = reinterpret_cast<const unsigned_int8 *>(ptr);
			return (u[0] | (s[1] << 8));

		#endif
	}

	inline unsigned_int16 ReadLittleEndianU16(const unsigned_int16 *ptr)
	{
		#if C4LITTLEENDIAN

			return (*ptr);

		#elif C4POWERPC

			return (__lhbrx((void *) ptr));

		#else

			const unsigned_int8 *u = reinterpret_cast<const unsigned_int8 *>(ptr);
			return (u[0] | (u[1] << 8));

		#endif
	}

	inline int32 ReadLittleEndianS32(const int32 *ptr)
	{
		#if C4LITTLEENDIAN

			return (*ptr);

		#elif C4POWERPC

			return ((int32) __lwbrx((void *) ptr));

		#else

			const int8 *s = reinterpret_cast<const int8 *>(ptr);
			const unsigned_int8 *u = reinterpret_cast<const unsigned_int8 *>(ptr);
			return (u[0] | (u[1] << 8) | (u[2] << 16) | (s[3] << 24));

		#endif
	}

	inline unsigned_int32 ReadLittleEndianU32(const unsigned_int32 *ptr)
	{
		#if C4LITTLEENDIAN

			return (*ptr);

		#elif C4POWERPC

			return (__lwbrx((void *) ptr));

		#else

			const unsigned_int8 *u = reinterpret_cast<const unsigned_int8 *>(ptr);
			return (u[0] | (u[1] << 8) | (u[2] << 16) | (u[3] << 24));

		#endif
	}

	inline float ReadLittleEndianF32(const float *ptr)
	{
		#if C4LITTLEENDIAN

			return (*ptr);

		#elif C4POWERPC

			unsigned_int32 x = __lwbrx((void *) ptr);
			return (*reinterpret_cast<float *>(&x));

		#else

			const unsigned_int8 *u = reinterpret_cast<const unsigned_int8 *>(ptr);
			unsigned_int32 x = u[0] | (u[1] << 8) | (u[2] << 16) | (u[3] << 24);
			return (*reinterpret_cast<float *>(&x));

		#endif
	}


	inline void WriteLittleEndianS16(int16 *ptr, int16 data)
	{
		#if C4LITTLEENDIAN

			*ptr = data;

		#elif C4POWERPC

			__sthbrx(ptr, data);

		#else

			unsigned_int8 *u = reinterpret_cast<unsigned_int8 *>(ptr);
			u[0] = (unsigned_int8) data;
			u[1] = (unsigned_int8) (data >> 8);

		#endif
	}

	inline void WriteLittleEndianU16(unsigned_int16 *ptr, unsigned_int16 data)
	{
		#if C4LITTLEENDIAN

			*ptr = data;

		#elif C4POWERPC

			__sthbrx(ptr, data);

		#else

			unsigned_int8 *u = reinterpret_cast<unsigned_int8 *>(ptr);
			u[0] = (unsigned_int8) data;
			u[1] = (unsigned_int8) (data >> 8);

		#endif
	}

	inline void WriteLittleEndianS32(int32 *ptr, int32 data)
	{
		#if C4LITTLEENDIAN

			*ptr = data;

		#elif C4POWERPC

			__stwbrx(ptr, data);

		#else

			unsigned_int8 *u = reinterpret_cast<unsigned_int8 *>(ptr);
			u[0] = (unsigned_int8) data;
			u[1] = (unsigned_int8) (data >> 8);
			u[2] = (unsigned_int8) (data >> 16);
			u[3] = (unsigned_int8) (data >> 24);

		#endif
	}

	inline void WriteLittleEndianU32(unsigned_int32 *ptr, unsigned_int32 data)
	{
		#if C4LITTLEENDIAN

			*ptr = data;

		#elif C4POWERPC

			__stwbrx(ptr, data);

		#else

			unsigned_int8 *u = reinterpret_cast<unsigned_int8 *>(ptr);
			u[0] = (unsigned_int8) data;
			u[1] = (unsigned_int8) (data >> 8);
			u[2] = (unsigned_int8) (data >> 16);
			u[3] = (unsigned_int8) (data >> 24);

		#endif
	}


	inline unsigned_int32 ReadBigEndianU32(const unsigned_int32 *ptr)
	{
		#if C4BIGENDIAN

			return (*ptr);

		#else

			const unsigned_int8 *u = reinterpret_cast<const unsigned_int8 *>(ptr);
			return ((u[0] << 24) | (u[1] << 16) | (u[2] << 8) | u[3]);

		#endif
	}

	inline void WriteBigEndianU32(unsigned_int32 *ptr, unsigned_int32 data)
	{
		#if C4BIGENDIAN

			*ptr = data;

		#else

			unsigned_int8 *u = reinterpret_cast<unsigned_int8 *>(ptr);
			u[0] = (unsigned_int8) (data >> 24);
			u[1] = (unsigned_int8) (data >> 16);
			u[2] = (unsigned_int8) (data >> 8);
			u[3] = (unsigned_int8) data;

		#endif
	}


	inline int32 AtomicAnd(volatile int32 *ptr, int32 x)
	{
		#if C4WINDOWS

			return (_InterlockedAnd(reinterpret_cast<volatile long *>(ptr), x));

		#elif C4MACOS

			return (OSAtomicAnd32Orig(x, reinterpret_cast<volatile uint32_t *>(ptr)));

		#elif C4PS4 //[ PS4

			// -- PS4 code hidden --

		#elif C4PS3 //[ PS3

			// -- PS3 code hidden --

		#else //]

			return (__sync_fetch_and_and(ptr, x));

		#endif
	}

	inline int32 AtomicOr(volatile int32 *ptr, int32 x)
	{
		#if C4WINDOWS

			return (_InterlockedOr(reinterpret_cast<volatile long *>(ptr), x));

		#elif C4MACOS

			return (OSAtomicOr32Orig(x, reinterpret_cast<volatile uint32_t *>(ptr)));

		#elif C4PS4 //[ PS4

			// -- PS4 code hidden --

		#elif C4PS3 //[ PS3

			// -- PS3 code hidden --

		#else //]

			return (__sync_fetch_and_or(ptr, x));

		#endif
	}


	inline machine_address GetPointerAddress(const volatile void *ptr)
	{
		return (reinterpret_cast<machine_address>(ptr));
	}


	//# \class	Mutex	Encapsulates a mutual exclusion object for multithreaded synchronization.
	//
	//# The $Mutex$ class encapsulates a mutual exclusion object for multithreaded synchronization.
	//
	//# \def	class Mutex
	//
	//# \ctor	Mutex();
	//
	//# \desc
	//# The $Mutex$ class defines a platform-independent mutual exclusion object that can be
	//# used for multithreaded synchronization. A mutex can only be owned by one thread at a time
	//# and when properly used prevents simultaneous access to resources by multiple threads.
	//#
	//# Ownership of a mutex is acquired by calling the $@Mutex::Acquire@$ function. If the mutex
	//# is already owned by a different thread when this function is called, then the calling thread
	//# blocks until the mutex becomes available. The $@Mutex::TryAcquire@$ function attempts to
	//# acquire ownership of a mutex, but does not block if acquisition fails. Ownership of a mutex
	//# is relinquished by calling the $@Mutex::Release@$ function.
	//
	//# \warning
	//# If a mutex object is destroyed while a thread is waiting to acquire it, then the
	//# behavior is undefined.
	//
	//# \also	$@Lock@$
	//# \also	$@Signal@$
	//# \also	$@Thread@$


	//# \function	Mutex::Acquire		Acquires exclusive ownership of a mutex.
	//
	//# \proto	void Acquire(void);
	//
	//# \desc
	//# The $Acquire$ function acquires exclusive ownership of a mutex. If the mutex is already owned
	//# by another thread when this function is called, then the calling thread blocks until the mutex
	//# becomes available.
	//#
	//# Ownership of a mutex is relinquished by calling the $@Mutex::Release@$ function from the
	//# same thread that called the $Acquire$ function.
	//#
	//# All mutexes are recursive, meaning that a thread already owning a mutex may reacquire the
	//# same mutex without deadlocking, and the system maintains an internal acquisition count.
	//# Each call to the $Acquire$ function must be balanced by a corresponding call to the
	//# $@Mutex::Release@$ function by the same thread.
	//#
	//# The $@Mutex::TryAcquire@$ function allows the caller to attempt to acquire ownership of a
	//# mutex without blocking if the acquisition attempt fails.
	//
	//# \also	$@Mutex::TryAcquire@$
	//# \also	$@Mutex::Release@$


	//# \function	Mutex::TryAcquire		Attempts to acquire exclusive ownership of a mutex.
	//
	//# \proto	bool TryAcquire(void);
	//
	//# \desc
	//# The $TryAcquire$ function attempts to acquire exclusive ownership of a mutex.
	//# If the acquisition is successful, then the function returns $true$, and the mutex
	//# behaves as if the $@Mutex::Acquire@$ function had been called. If the acquisition fails,
	//# then the function does not block and immediately returns $false$.
	//#
	//# Ownership of a mutex is relinquished by calling the $@Mutex::Release@$ function from the
	//# same thread that called the $TryAcquire$ function.
	//#
	//# If the $TryAcquire$ function is called for a mutex already owned by the calling thread,
	//# then $true$ is returned, and the internal acquisition count for the mutex is incremented.
	//# Each call to the $TryAcquire$ function that returns $true$ must be balanced by a
	//# corresponding call to the $@Mutex::Release@$ function by the same thread.
	//
	//# \also	$@Mutex::Acquire@$
	//# \also	$@Mutex::Release@$


	//# \function	Mutex::Release		Releases ownership of a mutex.
	//
	//# \proto	void Release(void);
	//
	//# \desc
	//# The $Release$ function relinquishes ownership of a mutex that was previously acquired
	//# by calling the $@Mutex::Acquire@$ or $@Mutex::TryAcquire@$ function from the same thread.
	//# Once the $Release$ function has been called to balance each previous acquisition of the mutex,
	//# the mutex is able to be acquired by other threads.
	//
	//# \also	$@Mutex::Acquire@$
	//# \also	$@Mutex::TryAcquire@$


	class Mutex
	{
		#if C4WINDOWS

			private:

				CRITICAL_SECTION	criticalSection;

			public:

				Mutex()
				{
					InitializeCriticalSection(&criticalSection);
				}

				~Mutex()
				{
					DeleteCriticalSection(&criticalSection);
				}

				void Acquire(void)
				{
					EnterCriticalSection(&criticalSection);
				}

				bool TryAcquire(void)
				{
					return (TryEnterCriticalSection(&criticalSection) != 0);
				}

				void Release(void)
				{
					LeaveCriticalSection(&criticalSection);
				}

		#elif C4POSIX

			private:

				pthread_mutex_t		mutexID;

			public:

				Mutex()
				{
					pthread_mutexattr_t		attr;

					pthread_mutexattr_init(&attr);
					pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
					pthread_mutex_init(&mutexID, &attr);
					pthread_mutexattr_destroy(&attr);
				}

				~Mutex()
				{
					pthread_mutex_destroy(&mutexID);
				}

				void Acquire(void)
				{
					pthread_mutex_lock(&mutexID);
				}

				bool TryAcquire(void)
				{
					return (pthread_mutex_trylock(&mutexID) == 0);
				}

				void Release(void)
				{
					pthread_mutex_unlock(&mutexID);
				}

		#elif C4PS4 //[ PS4

			// -- PS4 code hidden --

		#elif C4PS3 //[ PS3

			// -- PS3 code hidden --

		#endif //]
	};


	//# \class	Singleton	The base class for single-instance objects.
	//
	//# The $Singleton$ class template is the base class for types having only one global instance.
	//
	//# \def	template <class type> class Singleton
	//
	//# \tparam		type	The type of the class having only one global instance.
	//
	//# \ctor	Singleton(type *& instance);
	//
	//# The constructor has protected access. The $Singleton$ class can only exist as a base
	//# class for the class of the type given by the $type$ template parameter.
	//
	//# \param	instance	A reference to the global variable that serves as the pointer to the
	//#						single instance of the object whose type is given by the $type$ template parameter.
	//
	//# \desc
	//# The $Singleton$ class template is used to store a pointer to the single global instance of
	//# a class type that should only have one instance.


	template <class type> class Singleton
	{
		private:

			type	**singleton;

		protected:

			Singleton(type *& instance)
			{
				instance = static_cast<type *>(this);
				singleton = &instance;
			}

			~Singleton()
			{
				*singleton = nullptr;
			}
	};


	template <class type> class Manager
	{
		private:

			static type						managerObject;
			static type						**managerPointer;

			static const char *const		resultString[];
			static const unsigned_int32		resultIdentifier[];

		public:

			C4API static EngineResult New(void);
			C4API static void Delete(void);

			static const char *GetInternalResultString(EngineResult result)
			{
				return (resultString[GetResultCode(result)]);
			}

			static unsigned_int32 GetExternalResultIdentifier(EngineResult result)
			{
				return (resultIdentifier[GetResultCode(result)]);
			}
	};

	#ifdef C4ENGINEMODULE

		template <class type> EngineResult Manager<type>::New(void)
		{
			*managerPointer = &managerObject;

			EngineResult result = managerObject.Construct();
			if (result != kEngineOkay)
			{
				*managerPointer = nullptr;
			}

			return (result);
		}

		template <class type> void Manager<type>::Delete(void)
		{
			if (*managerPointer)
			{
				managerObject.Destruct();
				*managerPointer = nullptr;
			}
		}

	#endif
}


#endif

// ZYUQURM
