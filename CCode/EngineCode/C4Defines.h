 

#ifndef C4Defines_h
#define C4Defines_h


#define C4VERSION					"4.5"


#define C4DEMO						0


#if defined(C4WINDOWS)

	#undef C4WINDOWS

	#define C4WINDOWS				1
	#define C4MACOS					0
	#define C4LINUX					0
	#define C4IOS					0
	#define C4PS4					0
	#define C4PS3 					0

	#define C4DESKTOP				1
	#define C4MOBILE				0
	#define C4CONSOLE				0

	#define C4INTEL					1
	#define C4POWERPC				0
	#define C4ARM					0

	#define C4SSE					1
	#define C4SSE3					0
	#define C4SSE4					0
	#define C4AVX					0
	#define C4NEON					0
	#define C4ALTIVEC				0

	#define C4BIGENDIAN				0
	#define C4LITTLEENDIAN			1

	#define C4POSIX					0
	#define C4XAUDIO				1
	#define C4XINPUT				1

	#define C4VULKAN				0
	#define C4OPENGL				1
	#define C4GLCORE				1
	#define C4GLES					0
	#define C4GLES3					0
	#define C4METAL					0
	#define C4PSSL					0
	#define C4CG					0

	#define C4THROW

	#define C4_ENGINE_CONFIG_FILE	"engine"
	#define C4_INPUT_CONFIG_FILE	"input"

#elif defined(C4MACOS)

	#undef C4MACOS

	#define C4WINDOWS				0
	#define C4MACOS					1
	#define C4LINUX					0
	#define C4IOS					0
	#define C4PS4					0
	#define C4PS3 					0

	#define C4DESKTOP				1
	#define C4MOBILE				0
	#define C4CONSOLE				0

	#define C4INTEL					1
	#define C4POWERPC				0
	#define C4ARM					0

	#define C4SSE					1
	#define C4SSE3					0
	#define C4SSE4					0
	#define C4AVX					0
	#define C4NEON					0
	#define C4ALTIVEC				0

	#define C4BIGENDIAN				0
	#define C4LITTLEENDIAN			1

	#define C4POSIX					1
	#define C4XAUDIO				0
	#define C4XINPUT				0

	#define C4VULKAN				0
	#define C4OPENGL				1
	#define C4GLCORE				1
	#define C4GLES					0
	#define C4GLES3					0
	#define C4METAL					0
	#define C4PSSL					0
	#define C4CG					0

	#define C4THROW					throw()
 
	#define C4_ENGINE_CONFIG_FILE	"engine"
	#define C4_INPUT_CONFIG_FILE	"input"
 
#elif defined(C4LINUX)
 
	#undef C4LINUX

	#define C4WINDOWS				0 
	#define C4MACOS					0
	#define C4LINUX					1 
	#define C4IOS					0 
	#define C4PS4					0
	#define C4PS3 					0

	#define C4DESKTOP				1 
	#define C4MOBILE				0
	#define C4CONSOLE				0

	#define C4INTEL					1
	#define C4POWERPC				0
	#define C4ARM					0

	#define C4SSE					1
	#define C4SSE3					0
	#define C4SSE4					0
	#define C4AVX					0
	#define C4NEON					0
	#define C4ALTIVEC				0

	#define C4BIGENDIAN				0
	#define C4LITTLEENDIAN			1

	#define C4POSIX					1
	#define C4XAUDIO				0
	#define C4XINPUT				0

	#define C4VULKAN				0
	#define C4OPENGL				1
	#define C4GLCORE				1
	#define C4GLES					0
	#define C4GLES3					0
	#define C4METAL					0
	#define C4PSSL					0
	#define C4CG					0

	#define C4THROW

	#define C4_ENGINE_CONFIG_FILE	"engine"
	#define C4_INPUT_CONFIG_FILE	"input"

#elif defined(C4IOS) //[ MOBILE

	// -- Mobile code hidden --

#elif defined(C4PS4) //[ PS4

	// -- PS4 code hidden --

#elif defined(C4PS3) //[ PS3

	// -- PS3 code hidden --

#else //]

	#error	One of C4WINDOWS, C4MACOS, C4LINUX, C4IOS, C4PS4, or C4PS3 must be defined.

#endif


#if defined(C4OPTIMIZED)

	#undef C4OPTIMIZED

	#define C4OPTIMIZED				1
	#define C4DEBUG					0

#elif defined(C4DEBUG)

	#undef C4DEBUG

	#define C4OPTIMIZED				0
	#define C4DEBUG					1

#else

	#error	Either C4OPTIMIZED or C4DEBUG must be defined.

#endif


#define C4RECORDABLE C4DESKTOP


#include "C4Options.h"


#if defined(_MSC_VER)

	#pragma warning(3: 4706)			// assignment within conditional expression
	#pragma warning(disable: 4061)		// enumerator 'identifier' in switch of enum 'enumeration' is not explicitly handled by a case label
	#pragma warning(disable: 4062)		// enumerator 'identifier' in switch of enum 'enumeration' is not handled
	#pragma warning(disable: 4100)		// 'identifier' : unreferenced formal parameter
	#pragma warning(disable: 4244)		// conversion from 'type1' to 'type2', possible loss of data
	#pragma warning(disable: 4245)		// 'conversion' : conversion from 'type1' to 'type2', signed/unsigned mismatch
	#pragma warning(disable: 4265)		// 'class' : class has virtual functions, but destructor is not virtual
	#pragma warning(disable: 4266)		// 'function' : no override available for virtual member function from base 'type'; function is hidden
	#pragma warning(disable: 4310)		// cast truncates constant value
	#pragma warning(disable: 4316)		// object allocated on the heap may not be aligned 16
	#pragma warning(disable: 4324)		// 'struct_name' : structure was padded due to __declspec(align())
	#pragma warning(disable: 4355)		// 'this' : used in base member initializer list
	#pragma warning(disable: 4365)		// 'action' : conversion from 'type_1' to 'type_2', signed/unsigned mismatch
	#pragma warning(disable: 4370)		// 'class' : layout of class has changed from a previous version of the compiler due to better packing
	#pragma warning(disable: 4371)		// 'class' : layout of class may have changed from a previous version of the compiler due to better packing of member 'member_name'
	#pragma warning(disable: 4389)		// 'operator' : signed/unsigned mismatch
	#pragma warning(disable: 4505)		// 'function' : unreferenced local function has been removed
	#pragma warning(disable: 4512)		// 'class' : assignment operator could not be generated
	#pragma warning(disable: 4514)		// 'function' : unreferenced inline function has been removed
	#pragma warning(disable: 4522)		// 'class' : multiple assignment operators specified
	#pragma warning(disable: 4625)		// 'derived class' : copy constructor could not be generated because a base class copy constructor is inaccessible
	#pragma warning(disable: 4626)		// 'derived class' : assignment operator could not be generated because a base class assignment operator is inaccessible
	#pragma warning(disable: 4640)		// 'instance' : construction of local static object is not thread-safe
	#pragma warning(disable: 4701)		// potentially uninitialized local variable 'name' used
	#pragma warning(disable: 4703)		// potentially uninitialized local pointer variable 'name' used
	#pragma warning(disable: 4710)		// 'function' : function not inlined
	#pragma warning(disable: 4800)		// 'type' : forcing value to bool 'true' or 'false' (performance warning)
	#pragma warning(disable: 4804)		// 'operation' : unsafe use of type 'bool' in operation
	#pragma warning(disable: 4805)		// 'operation' : unsafe mix of type 'type' and type 'type' in operation
	#pragma warning(disable: 4820)		// 'bytes' bytes padding added after construct 'member_name'

	#include "C4PrefixWindows.h"

	#define C4VISUALC				1

	#if defined(_WIN64)

		#define C4PTR64				1

	#else

		#define C4PTR64				0

	#endif

	typedef signed char				int8;
	typedef unsigned char			unsigned_int8;

	typedef short					int16;
	typedef unsigned short			unsigned_int16;

	typedef int						int32;
	typedef unsigned int			unsigned_int32;

	typedef __int64					int64;
	typedef unsigned __int64		unsigned_int64;

	#if C4PTR64

		typedef __int64				machine;
		typedef unsigned __int64	unsigned_machine;

		typedef __int64				machine_int;
		typedef unsigned __int64	unsigned_machine_int;

	#else

		typedef long				machine;
		typedef unsigned long		unsigned_machine;

		typedef long				machine_int;
		typedef unsigned long		unsigned_machine_int;

	#endif

	#define alignas(n) __declspec(align(n))

	#define __attribute__(x)
	#define restrict __restrict

	#pragma pointers_to_members(full_generality, multiple_inheritance)

#elif defined(__ORBIS__) //[ PS4

	// -- PS4 code hidden --

#elif defined(__SNC__) //[ PS3

	// -- PS3 code hidden --

#elif defined(__GNUC__) //]

	#if C4MACOS

		#include "C4PrefixMacOS.h"

	#elif C4IOS

		#include "C4PrefixIOS.h"

	#elif C4LINUX

		#include "C4PrefixLinux.h"

	#endif

	#define C4VISUALC				0

	#if defined(__LP64__)

		#define C4PTR64				1

	#else

		#define C4PTR64				0

	#endif

	typedef signed char				int8;
	typedef unsigned char			unsigned_int8;

	typedef short					int16;
	typedef unsigned short			unsigned_int16;

	typedef int						int32;
	typedef unsigned int			unsigned_int32;

	typedef long long				int64;
	typedef unsigned long long		unsigned_int64;

	#if C4PTR64

		typedef long long			machine;
		typedef unsigned long long	unsigned_machine;

		typedef long long			machine_int;
		typedef unsigned long long	unsigned_machine_int;

	#else

		typedef long				machine;
		typedef unsigned long		unsigned_machine;

		typedef long				machine_int;
		typedef unsigned long		unsigned_machine_int;

	#endif

	#define restrict __restrict__

	#define __cdecl

#endif


#if C4PTR64

	typedef unsigned_int64			machine_address;

#else

	typedef unsigned_int32			machine_address;

#endif


#if C4WINDOWS

	#ifdef C4ENGINEMODULE

		#define C4API __declspec(dllexport)

	#else

		#define C4API __declspec(dllimport)

	#endif

	#define C4MODULEEXPORT __declspec(dllexport)
	#define C4MODULEIMPORT __declspec(dllimport)

#elif C4MACOS || C4LINUX

	#ifdef C4ENGINEMODULE

		#define C4API __attribute__((visibility("default")))

	#else

		#define C4API

	#endif

	#define C4MODULEEXPORT __attribute__((visibility("default")))
	#define C4MODULEIMPORT

#else

	#define C4API

	#define C4MODULEEXPORT
	#define C4MODULEIMPORT

#endif


#endif

// ZYUQURM
