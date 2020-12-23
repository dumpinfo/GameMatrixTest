 

#include "C4Base.h"


using namespace C4;


#if C4DEBUG

	void C4::Fatal(const char *message)
	{
		#if C4WINDOWS

			__debugbreak();

		#elif C4POSIX

			raise(SIGTRAP);

		#elif C4PS4 //[ PS4

			// -- PS4 code hidden --

		#elif C4PS3 //[ PS3

			// -- PS3 code hidden --

		#endif //]
	}

	void C4::Assert(bool condition, const char *message)
	{
		if (!condition)
		{
			Fatal(message);
		}
	}

#endif

// ZYUQURM
