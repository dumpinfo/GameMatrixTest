 

#include "C4Main.h"
#include "C4Threads.h"
#include "C4Engine.h"

#if C4IOS //[ MOBILE

	// -- Mobile code hidden --

#endif //]


#ifndef APPLICATION_NAME

	#define APPLICATION_NAME "C4 Engine"

#endif


using namespace C4;


#if C4WINDOWS

	int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int cmdShow)
	{
		HANDLE mutex = CreateMutexA(nullptr, true, APPLICATION_NAME);
		if (GetLastError() == ERROR_SUCCESS)
		{
			#if C4DEMO

				char	directory[MAX_PATH + 1];

				directory[MAX_PATH] = 0;
				if (GetModuleFileNameA(nullptr, directory, MAX_PATH) != 0)
				{
					for (machine k = Text::GetTextLength(directory) - 1; k > 0; k--)
					{
						if (directory[k] == '\\')
						{
							directory[k] = 0;
							break;
						}
					}

					SetCurrentDirectoryA(directory);
				}

			#endif

			#if C4DEBUG

				WIN32_FIND_DATA		fileData;

				HANDLE handle = FindFirstFileA("Data", &fileData);
				if (handle != INVALID_HANDLE_VALUE)
				{
					FindClose(handle);
				}

				if ((handle == INVALID_HANDLE_VALUE) || ((fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0))
				{
					const char *message =	"The Data directory could not be found.\r\n"
											"Make sure you have set the working directory to \"..\\..\\..\" in the Debugging properties.";

					MessageBoxA(nullptr, message, APPLICATION_NAME, MB_OK | MB_ICONINFORMATION);
					return (0);
				}

			#endif

			Thread::SetThreadName("C4 Main Thread");

			Engine::New();

			EngineResult result = TheEngine->Initialize(APPLICATION_NAME, instance, commandLine);
			if (result == kEngineOkay)
			{
				TheEngine->Run();
				TheEngine->Terminate();
			}
			else
			{
				unsigned_int32 manager = GetResultManager(result);
				if ((manager == kManagerDisplay) || (manager == kManagerGraphics))
				{
					const char *message =	"The engine was unable to create a suitable graphics context.\r\n\r\n"
											"Please make sure that your graphics hardware meets the minimum requirements "
											"and that you have the latest display drivers installed.\r\n\r\n"
											"If you're running in windowed mode for the first time and you see this message, "
											"make sure your desktop color depth is set to 32-bit color.";

					MessageBoxA(nullptr, message, APPLICATION_NAME, MB_OK | MB_ICONINFORMATION);
				}
				else if (manager == kManagerSound)
				{
					const char *message =	"The engine was unable to initialize the sound system.\r\n\r\n"
											"Please make sure that you have the June 2010 (or later) version of DirectX installed.\r\n\r\n"
											"Also make sure that you have a sound driver installed.";

					MessageBoxA(nullptr, message, APPLICATION_NAME, MB_OK | MB_ICONINFORMATION);
				} 
			}

			Engine::Delete(); 

			#if C4LEAK_DETECTION 

				Engine::DumpMemory("Leaks.txt");
 
			#endif
 
			ReleaseMutex(mutex); 
		}

		return (0);
	} 

#elif C4MACOS

	@interface EngineDelegate : NSObject<NSApplicationDelegate>
	{
		@private

			NSTimer		*timer;
	}
	@end

	@implementation EngineDelegate

	- (void) applicationWillFinishLaunching: (NSNotification *) notification
	{
		timer = nil;
		Engine::New();

		EngineResult result = TheEngine->Initialize(APPLICATION_NAME, "");
		if (result == kEngineOkay)
		{
			timer = [NSTimer scheduledTimerWithTimeInterval: 0.0 target: self selector: @selector(runLoop:) userInfo: nil repeats: YES];
		}
		else
		{
			unsigned_int32 manager = GetResultManager(result);
			if ((manager == kManagerDisplay) || (manager == kManagerGraphics))
			{
				NSString *title = @"C4 Engine could not be initialized.";
				NSString *message = @"The engine was unable to create a suitable graphics context.\n\nPlease make sure your graphics hardware meets the minimum requirements.";

				[[NSAlert alertWithMessageText: title defaultButton: nil alternateButton: nil otherButton: nil informativeTextWithFormat: message] runModal];
			}

			[NSApp terminate: nil];
		}
	}

	- (void) applicationWillTerminate: (NSNotification *) notification
	{
		[timer invalidate];

		if (TheEngine)
		{
			TheEngine->Terminate();
		}

		Engine::Delete();

		#if C4LEAK_DETECTION

			Engine::DumpMemory("Leaks.txt");

		#endif
	}

	- (void) applicationDidBecomeActive: (NSNotification *) notification
	{
		if (TheEngine)
		{
			TheEngine->HandleApplicationForegroundEvent();
		}
	}

	- (void) applicationWillResignActive: (NSNotification *) notification
	{
		if (TheEngine)
		{
			TheEngine->HandleApplicationBackgroundEvent();
		}
	}

	- (void) runLoop: (NSTimer *) timer
	{
		if (!TheEngine->Run())
		{
			[NSApp terminate: nil];
		}
	}

	@end

	int main(int argc, const char **argv)
	{
		Thread::SetThreadName("C4 Main Thread");
		return (NSApplicationMain(argc, argv));
	}

#elif C4LINUX

	int main(int argc, const char **argv)
	{
		String<>	commandLine;

		Thread::SetThreadName("C4 Main Thread");

		for (machine a = 1; a < argc; a++)
		{
			commandLine += argv[a];
			commandLine += ' ';
		}

		Engine::New();

		EngineResult result = TheEngine->Initialize(APPLICATION_NAME, commandLine);
		if (result == kEngineOkay)
		{
			TheEngine->Run();
			TheEngine->Terminate();
		}

		Engine::Delete();

		#if C4LEAK_DETECTION

			Engine::DumpMemory("Leaks.txt");

		#endif

		return (0);
	}

#elif C4IOS //[ MOBILE

	// -- Mobile code hidden --

#elif C4PS4 //[ PS4

	// -- PS4 code hidden --

#elif C4PS3 //[ PS3

	// -- PS3 code hidden --

#endif //]

// ZYUQURM
