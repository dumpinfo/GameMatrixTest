 

#ifndef C4Main_h
#define C4Main_h


#include "C4Types.h"


#if C4WINDOWS

	int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);

#elif C4MACOS

	int main(int argc, const char **argv);

#elif C4LINUX

	int main(int argc, const char **argv);

#elif C4IOS //[ MOBILE

	// -- Mobile code hidden --

#elif C4PS4 //[ PS4

	// -- PS4 code hidden --

#elif C4PS3 //[ PS3

	// -- PS3 code hidden --

#endif //]


#endif

// ZYUQURM
