 

#include "C4Engine.h"
#include "C4ConfigData.h"
#include "C4Application.h"
#include "C4AudioCapture.h"
#include "C4World.h"
#include "C4ToolWindows.h"
#include "C4Logo.h"


using namespace C4;


#if !C4DESKTOP

	extern "C"
	{
		C4::Application *CreateApplication(void);
	}

#endif


Engine *C4::TheEngine = nullptr;


namespace C4
{
	template <> Engine Manager<Engine>::managerObject(0);
	template <> Engine **Manager<Engine>::managerPointer = &TheEngine;

	template <> const char *const Manager<Engine>::resultString[] =
	{
		nullptr,
		"Module failed to load",
		"Module failed to initialize",
		"Construction function missing"
	};

	template <> const unsigned_int32 Manager<Engine>::resultIdentifier[] =
	{
		0, 'MLOD', 'MINT', 'MCON'
	};

	template class Manager<Engine>;
}


List<Reporter> Engine::reporterList;

unsigned_int32 Engine::mouseButtonMask = 0;
Point3D Engine::lastMousePosition;


Reporter::Reporter(ReportProc *proc, void *cookie)
{
	reportProc = proc;
	reportCookie = cookie;
}

Reporter::~Reporter()
{
}


MouseEventHandler::MouseEventHandler(HandlerProc *proc, void *cookie)
{
	handlerProc = proc;
	handlerCookie = cookie;
}


KeyboardEventHandler::KeyboardEventHandler(HandlerProc *proc, void *cookie)
{
	handlerProc = proc;
	handlerCookie = cookie;
}


#if C4MACOS

	@interface WindowDelegate : NSObject<NSWindowDelegate>
	{
	}
	@end

	@implementation WindowDelegate

	- (BOOL) windowShouldClose: (id) sender
	{
		if ((TheInterfaceMgr) && (TheInterfaceMgr->QuitEnabled()))
		{
			TheEngine->Quit();
			return (NO);
		}

		return (YES);
	}

	- (NSApplicationPresentationOptions) window: (NSWindow *) window willUseFullScreenPresentationOptions: (NSApplicationPresentationOptions) proposedOptions
	{
		return (NSApplicationPresentationHideDock | NSApplicationPresentationHideMenuBar | NSApplicationPresentationFullScreen); 
	}

	- (void) windowDidEnterFullScreen: (NSNotification *) notification 
	{
		TheDisplayMgr->AdjustEngineWindow(); 
	}

	- (void) windowDidExitFullScreen: (NSNotification *) notification 
	{
		TheDisplayMgr->AdjustEngineWindow(); 
	} 

	@end

 
	@interface EngineWindow : NSWindow
	{
		@public

			WindowDelegate		*windowDelegate;
	}
	@end

	@implementation EngineWindow

	- (void) close
	{
		[self setDelegate: nil];
		[windowDelegate release];

		[super close];
	}

	- (void) mouseDown: (NSEvent *) event
	{
		TheEngine->HandleMouseEvent(event);
	}

	- (void) mouseUp: (NSEvent *) event
	{
		TheEngine->HandleMouseEvent(event);
	}

	- (void) mouseDragged: (NSEvent *) event
	{
		TheEngine->HandleMouseEvent(event);
	}

	- (void) rightMouseDown: (NSEvent *) event
	{
		TheEngine->HandleMouseEvent(event);
	}

	- (void) rightMouseUp: (NSEvent *) event
	{
		TheEngine->HandleMouseEvent(event);
	}

	- (void) rightMouseDragged: (NSEvent *) event
	{
		TheEngine->HandleMouseEvent(event);
	}

	- (void) otherMouseDown: (NSEvent *) event
	{
		TheEngine->HandleMouseEvent(event);
	}

	- (void) otherMouseUp: (NSEvent *) event
	{
		TheEngine->HandleMouseEvent(event);
	}

	- (void) otherMouseDragged: (NSEvent *) event
	{
		TheEngine->HandleMouseEvent(event);
	}

	- (void) mouseMoved: (NSEvent *) event
	{
		TheEngine->HandleMouseEvent(event);
	}

	- (void) mouseEntered: (NSEvent *) event
	{
		if (!(TheDisplayMgr->GetDisplayFlags() & kDisplayFullscreen))
		{
			TheDisplayMgr->HideCursor();
		}
	}

	- (void) mouseExited: (NSEvent *) event
	{
		if (!(TheDisplayMgr->GetDisplayFlags() & kDisplayFullscreen))
		{
			TheDisplayMgr->ShowCursor();
		}
	}

	- (void) scrollWheel: (NSEvent *) event
	{
		TheEngine->HandleMouseEvent(event);
	}

	- (void) keyDown: (NSEvent *) event
	{
		TheEngine->HandleKeyboardEvent(event);
	}

	- (void) keyUp: (NSEvent *) event
	{
		TheEngine->HandleKeyboardEvent(event);
	}

	- (void) tabletPoint: (NSEvent *) event
	{
		TheEngine->HandleTabletEvent(event);
	}

	- (void) tabletProximity: (NSEvent *) event
	{
		TheEngine->HandleTabletEvent(event);
	}

	- (BOOL) canBecomeKeyWindow
	{
		return (YES);
	}

	- (BOOL) canBecomeMainWindow
	{
		return (YES);
	}

	@end

#endif


Engine::Engine(int) :

		#if C4LOG_FILE

			logger(&Logger, this),

		#endif

		inputConfigTask(&ExecuteInputConfig, this),
		gameModuleTask(&ChangeGameModule, this),

		gameModuleObserver(this, &Engine::HandleGameModuleEvent),
		timeCommandObserver(this, &Engine::HandleTimeCommand),
		shotCommandObserver(this, &Engine::HandleShotCommand),
		recordCommandObserver(this, &Engine::HandleRecordCommand),
		undefCommandObserver(this, &Engine::HandleUndefCommand),
		bindCommandObserver(this, &Engine::HandleBindCommand),
		unbindCommandObserver(this, &Engine::HandleUnbindCommand),
		loadCommandObserver(this, &Engine::HandleLoadCommand),
		unloadCommandObserver(this, &Engine::HandleUnloadCommand),
		cmdCommandObserver(this, &Engine::HandleCmdCommand),
		varCommandObserver(this, &Engine::HandleVarCommand),
		execCommandObserver(this, &Engine::HandleExecCommand),
		importCommandObserver(this, &Engine::HandleImportCommand),
		sayCommandObserver(this, &Engine::HandleSayCommand),
		addressCommandObserver(this, &Engine::HandleAddressCommand),
		resolveCommandObserver(this, &Engine::HandleResolveCommand),
		disconnectCommandObserver(this, &Engine::HandleDisconnectCommand),

		#if C4STATS

			statCommandObserver(this, &Engine::HandleStatCommand),

		#endif

		#if C4DIAGS

			wireCommandObserver(this, &Engine::HandleWireCommand),
			normCommandObserver(this, &Engine::HandleNormCommand),
			tangCommandObserver(this, &Engine::HandleTangCommand),
			lrgnCommandObserver(this, &Engine::HandleLrgnCommand),
			srgnCommandObserver(this, &Engine::HandleSrgnCommand),
			doffCommandObserver(this, &Engine::HandleDoffCommand),
			spthCommandObserver(this, &Engine::HandleSpthCommand),
			bodyCommandObserver(this, &Engine::HandleBodyCommand),
			ctacCommandObserver(this, &Engine::HandleCtacCommand),
			fbufCommandObserver(this, &Engine::HandleFbufCommand),
			smapCommandObserver(this, &Engine::HandleSmapCommand),
			netCommandObserver(this, &Engine::HandleNetCommand),
			extCommandObserver(this, &Engine::HandleExtCommand),
			rsrcCommandObserver(this, &Engine::HandleRsrcCommand),
			heapCommandObserver(this, &Engine::HandleHeapCommand),

		#endif

		quitCommandObserver(this, &Engine::HandleQuitCommand)
{
}

Engine::~Engine()
{
}

EngineResult Engine::Construct(void)
{
	engineFlags = kEngineForeground | kEngineVisible;

	#if C4DESKTOP

		backgroundSleepTime = 0;

	#endif

	#if C4WINDOWS

		deadKeyFlag = false;
		wheelDeltaAccum = 0;

	#elif C4MACOS

		wheelDeltaAccum = 0.0F;

	#endif

	InitializeProcessorData();

	#if C4LOG_FILE

		InstallReporter(&logger);

	#endif

	AddCommand(new Command("time", &timeCommandObserver));
	AddCommand(new Command("shot", &shotCommandObserver));
	AddCommand(new Command("record", &recordCommandObserver));
	AddCommand(new Command("undef", &undefCommandObserver));
	AddCommand(new Command("bind", &bindCommandObserver));
	AddCommand(new Command("unbind", &unbindCommandObserver));
	AddCommand(new Command("load", &loadCommandObserver));
	AddCommand(new Command("unload", &unloadCommandObserver));
	AddCommand(new Command("cmd", &cmdCommandObserver));
	AddCommand(new Command("var", &varCommandObserver));
	AddCommand(new Command("exec", &execCommandObserver));
	AddCommand(new Command("import", &importCommandObserver));
	AddCommand(new Command("say", &sayCommandObserver));
	AddCommand(new Command("address", &addressCommandObserver));
	AddCommand(new Command("resolve", &resolveCommandObserver));
	AddCommand(new Command("disconnect", &disconnectCommandObserver));

	#if C4STATS

		AddCommand(new Command("stat", &statCommandObserver));

	#endif

	#if C4DIAGS

		AddCommand(new Command("wire", &wireCommandObserver));
		AddCommand(new Command("norm", &normCommandObserver));
		AddCommand(new Command("tang", &tangCommandObserver));
		AddCommand(new Command("lrgn", &lrgnCommandObserver));
		AddCommand(new Command("srgn", &srgnCommandObserver));
		AddCommand(new Command("doff", &doffCommandObserver));
		AddCommand(new Command("spth", &spthCommandObserver));
		AddCommand(new Command("body", &bodyCommandObserver));
		AddCommand(new Command("ctac", &ctacCommandObserver));
		AddCommand(new Command("fbuf", &fbufCommandObserver));
		AddCommand(new Command("smap", &smapCommandObserver));
		AddCommand(new Command("net", &netCommandObserver));
		AddCommand(new Command("ext", &extCommandObserver));
		AddCommand(new Command("rsrc", &rsrcCommandObserver));
		AddCommand(new Command("heap", &heapCommandObserver));

	#endif

	AddCommand(new Command("quit", &quitCommandObserver));

	return (kEngineOkay);
}

void Engine::Destruct(void)
{
	mouseEventHandlerList.RemoveAll();
	keyboardEventHandlerList.RemoveAll();

	reporterList.RemoveAll();

	variableMap.Purge();
	commandMap.Purge();
}

#if C4WINDOWS

	EngineResult Engine::Initialize(const char *name, HINSTANCE instance, const char *commandLine)

#else

	EngineResult Engine::Initialize(const char *name, const char *commandLine)

#endif

{
	applicationName = name;

	multiaxisMouseActiveCount = 0;
	tabletActiveCount = 0;
	stylusPressure = 0.0F;

	#if C4WINDOWS

		wchar_t		wideName[256];

		engineInstance = instance;

		windowClass.cbSize = sizeof(WNDCLASSEXW);
		windowClass.style = CS_OWNDC;
		windowClass.lpfnWndProc = &WindowProc;
		windowClass.cbClsExtra = 0;
		windowClass.cbWndExtra = 0;
		windowClass.hInstance = engineInstance;
		windowClass.hIcon = LoadIconA(engineInstance, MAKEINTRESOURCE(1));
		windowClass.hCursor = LoadCursorA(nullptr, IDC_ARROW);
		windowClass.hbrBackground = nullptr;
		windowClass.lpszMenuName = nullptr;
		windowClass.lpszClassName = L"C4";
		windowClass.hIconSm = nullptr;
		RegisterClassExW(&windowClass);

		int32 len = Min(Text::GetTextLength(name), 255);
		for (machine a = 0; a < len; a++)
		{
			wideName[a] = name[a];
		}

		wideName[len] = 0;

		engineWindow = CreateWindowExW(0, L"C4", wideName, WS_POPUP | WS_CLIPCHILDREN, 0, 0, 640, 480, nullptr, nullptr, engineInstance, nullptr);

		SetFocus(engineWindow);
		SetCursor(windowClass.hCursor);

		CoInitializeEx(nullptr, COINIT_MULTITHREADED | COINIT_SPEED_OVER_MEMORY);

	#elif C4MACOS

		NSUInteger windowStyle = NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask;
		EngineWindow *window = [[EngineWindow alloc] initWithContentRect: NSMakeRect(0, 0, 640, 480) styleMask: windowStyle backing: NSBackingStoreBuffered defer: NO];
		engineWindow = window;

		WindowDelegate *delegate = [WindowDelegate alloc];
		[window setDelegate: delegate];
		window->windowDelegate = delegate;

		CFStringRef string = CFStringCreateWithCStringNoCopy(kCFAllocatorDefault, TheEngine->GetApplicationName(), kCFStringEncodingUTF8, kCFAllocatorNull);
		[window setTitle: (NSString *) string];
		CFRelease(string);

		[window setReleasedWhenClosed: NO];
		[window setAcceptsMouseMovedEvents: YES];
		[window setBackgroundColor: [NSColor blackColor]];
		[window setAnimationBehavior: NSWindowAnimationBehaviorNone];

	#elif C4LINUX

		XSetWindowAttributes	attributes;

		engineDisplay = XOpenDisplay(nullptr);

		attributes.event_mask = ButtonPressMask | ButtonReleaseMask | PointerMotionMask | KeyPressMask | KeyReleaseMask;
		engineWindow = XCreateWindow(engineDisplay, DefaultRootWindow(engineDisplay), 0, 0, 640, 480, 0, CopyFromParent, InputOutput, CopyFromParent, CWEventMask, &attributes);
		XStoreName(engineDisplay, engineWindow, name);

		deleteWindowAtom = XInternAtom(engineDisplay, "WM_DELETE_WINDOW", false);
		XSetWMProtocols(engineDisplay, engineWindow, &deleteWindowAtom, 1);

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]

	EngineResult result = ConstructManagers(commandLine);
	if (result != kEngineOkay)
	{
		#if C4LOG_FILE

			Report("<br/><br/>", kReportLog);
			LogResult(result);

		#endif

		Terminate();
		return (result);
	}

	result = LoadApplicationModule();
	if (result != kEngineOkay)
	{
		Terminate();
		return (result);
	}

	TheTimeMgr->ResetTime();
	return (kEngineOkay);
}

void Engine::Terminate(void)
{
	delete TheApplication;
	if (ThePluginMgr)
	{
		ThePluginMgr->PurgePlugins();
	}

	UnloadApplicationModule();
	DestroyManagers();

	#if C4WINDOWS

		CoUninitialize();

		DestroyWindow(engineWindow);
		UnregisterClassW(L"C4", engineInstance);

	#elif MACOS

		[displayWindow close];
		[displayWindow release];

	#elif C4LINUX

		XDestroyWindow(engineDisplay, engineWindow);
		XCloseDisplay(engineDisplay);

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]
}

void Engine::InitializeProcessorData(void)
{
	processorFlags = 0;

	#if C4WINDOWS

		SYSTEM_INFO		systemInfo;

		GetSystemInfo(&systemInfo);
		processorCount = systemInfo.dwNumberOfProcessors;

		if (IsProcessorFeaturePresent(PF_XMMI_INSTRUCTIONS_AVAILABLE))
		{
			processorFlags |= kProcessorSSE;
			if (IsProcessorFeaturePresent(PF_XMMI64_INSTRUCTIONS_AVAILABLE))
			{
				processorFlags |= kProcessorSSE2;
				if (IsProcessorFeaturePresent(PF_SSE3_INSTRUCTIONS_AVAILABLE))
				{
					processorFlags |= kProcessorSSE3;
				}
			}
		}

	#elif C4MACOS || C4IOS

		size_t		size;
		int			name[2];

		size = 4;
		name[0] = CTL_HW;
		name[1] = HW_NCPU;
		if (sysctl(name, 2, &processorCount, &size, nullptr, 0) != 0)
		{
			processorCount = 1;
		}

		#if C4MACOS

			SInt32		result;

			processorFlags |= kProcessorSSE | kProcessorSSE2;

			size = 4;
			if ((sysctlbyname("hw.optional.sse3", &result, &size, nullptr, 0) == 0) && (result != 0))
			{
				processorFlags |= kProcessorSSE3;
			}

		#else //[ MOBILE

			// -- Mobile code hidden --

		#endif //]

	#elif C4LINUX

		int		eax, ebx, ecx, edx;

		processorCount = Max(sysconf(_SC_NPROCESSORS_ONLN), 1);

		__cpuid(1, eax, ebx, ecx, edx);
		if (edx & bit_SSE)
		{
			processorFlags |= kProcessorSSE;
			if (edx & bit_SSE2)
			{
				processorFlags |= kProcessorSSE2;
				if (ecx & bit_SSE3)
				{
					processorFlags |= kProcessorSSE3;
				}
			}
		}

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]
}

#if C4PS3 //[ PS3

	// -- PS3 code hidden --

#endif //]

EngineResult Engine::ConstructManagers(const char *commandLine)
{
	FileMgr::New();
	TimeMgr::New();

	#if C4MACOS

		openglBundle = nullptr;

		NSFileManager *fileManager = [NSFileManager defaultManager];
		NSArray *urlArray = [fileManager URLsForDirectory: NSLibraryDirectory inDomains: NSSystemDomainMask];

		NSUInteger urlCount = [urlArray count];
		for (unsigned_machine a = 0; a < urlCount; a++)
		{
			NSError		*error;

			NSURL *folderURL = [urlArray objectAtIndex: a];
			NSURL *frameworkURL = [[NSURL alloc] initWithString: @"Frameworks/OpenGL.framework" relativeToURL: folderURL];
			if ([frameworkURL checkResourceIsReachableAndReturnError: &error] == YES)
			{
				openglBundle = CFBundleCreate(kCFAllocatorDefault, (CFURLRef) frameworkURL);
				[frameworkURL release];
				break;
			}

			[frameworkURL release];
		}

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]

	ResourceMgr::New();

	#if C4LOG_FILE

		BeginLog();

	#endif

	if (ConfigDataDescription::Execute(C4_ENGINE_CONFIG_FILE, TheResourceMgr->GetConfigCatalog()) != kDataOkay)
	{
		ConfigDataDescription::Execute(C4_ENGINE_CONFIG_FILE);
	}

	ExecuteText(commandLine);

	#if !C4PS3

		EngineResult result = DisplayMgr::New();
		if (result != kEngineOkay)
		{
			return (result);
		}

	#else //[ PS3

		// -- PS3 code hidden --

	#endif //]

	result = SoundMgr::New();
	if (result != kEngineOkay)
	{
		return (result);
	}

	AudioCaptureMgr::New();

	result = InputMgr::New();
	if (result != kEngineOkay)
	{
		return (result);
	}

	JobMgr::New();
	InterfaceMgr::New();
	MovieMgr::New();
	NetworkMgr::New();
	MessageMgr::New();
	WorldMgr::New();
	PluginMgr::New();
	ConsoleWindow::New();

	return (kEngineOkay);
}

void Engine::DestroyManagers(void)
{
	delete TheConsoleWindow;

	PluginMgr::Delete();
	WorldMgr::Delete();
	MessageMgr::Delete();
	NetworkMgr::Delete();
	MovieMgr::Delete();
	InterfaceMgr::Delete();
	JobMgr::Delete();
	InputMgr::Delete();
	AudioCaptureMgr::Delete();
	SoundMgr::Delete();
	DisplayMgr::Delete();
	ResourceMgr::Delete();

	#if C4LOG_FILE

		EndLog();

	#endif

	#if C4MACOS

		CFBundleRef bundle = openglBundle;
		if (bundle)
		{
			openglBundle = nullptr;
			CFRelease(bundle);
		}

	#endif

	TimeMgr::Delete();

	#if !C4MACOS || !C4LEAK_DETECTION

		FileMgr::Delete();

	#endif
}

#if C4LOG_FILE

	void Engine::BeginLog(void)
	{
		String<127>		date;
		String<127>		time;

		ResourcePath path(TheResourceMgr->GetSystemCatalog()->GetRootPath());
		logFile.Open(path += "C4Log.html", kFileCreate);

		Report( "<html>\r\n"
				"<head>\r\n<title>C4 Log File</title>\r\n"
					"<style type=\"text/css\">\r\n"
						"body {background-color: #F0F0F0; font-family: arial; font-size: 10pt;}\r\n"
						"table.data {background-color: white; border-top: solid 1px #444;}\r\n"
						"table.data th {width: 240px; font-size: 10pt; font-weight: bold; text-align: left; vertical-align: top; padding: 4px 6px 4px 4px; border-left: solid 1px #444; border-right: solid 1px #444; border-bottom: solid 1px #444;}\r\n"
						"table.data td {width: 512px; font-size: 10pt; vertical-align: top; padding: 4px 4px 4px 6px; border-right: solid 1px #444; border-bottom: solid 1px #444;}\r\n"
						"table.source {border: solid 1px #444; margin: 16px 0px 16px 0px;}\r\n"
						"td.line {vertical-align: top; text-align: right; font-family: 'courier new', fixed; font-size: 10pt; background-color: #AAA; width: auto; border: 0px; padding: 1px 4px 1px 4px;}\r\n"
						"td.source {vertical-align: top; text-align: left; font-family: 'courier new', fixed; font-size: 10pt; background-color: white; width: auto; border: 0px; padding: 1px 8px 1px 8px;}\r\n"
					"</style>\r\n"
				"</head>\r\n"
				"<body>\r\n",
			kReportLog);

		Report("<table cellspacing=\"0\" cellpadding=\"0\" style=\"margin-bottom: 12px;\">\r\n", kReportLog);
		Report("<tr><th style=\"padding-right: 20px;\"><img style=\"width: 128px; height: 105px;\" src=\"data:image/png;base64,", kReportLog);
		Report(LogoImage, kReportLog);
		Report("\" /></th>\r\n", kReportLog);

		Report("<td style=\"vertical-align: top;\"><div style=\"font-family: tahoma, arial; font-size: 36pt; font-weight: bold; text-shadow: 0px 2px 3px #444;\">C4 Engine</div><div style=\"margin-top: 0.5em; padding-left: 0.25em;\">Version ", kReportLog);
		Report(C4VERSION, kReportLog);

		#if C4DEBUG

			Report(" (Debug)", kReportLog);

		#endif

		Report("</div></td></tr></table>", kReportLog);

		Report("<table class=\"data\" cellspacing=\"0\" cellpadding=\"0\">\r\n", kReportLog);

		Report("</td></tr>\r\n<tr><th>Time stamp</th><td>", kReportLog);
		TimeMgr::GetDateTimeStrings(&date, &time);
		Report(date += "<br/>", kReportLog);
		Report(time, kReportLog);

		Report("</td></tr>\r\n<tr><th>Operating system</th><td>", kReportLog);

		#if C4WINDOWS

			IWbemLocator	*wbemLocator;

			if (SUCCEEDED(CoCreateInstance(__uuidof(WbemLocator), 0, CLSCTX_INPROC_SERVER, __uuidof(IWbemLocator), (void **) &wbemLocator)))
			{
				IWbemServices	*wbemServices;

				BSTR resourceString = SysAllocString(L"root\\cimv2");
				if (SUCCEEDED(wbemLocator->ConnectServer(resourceString, nullptr, nullptr, nullptr, 0, nullptr, nullptr, &wbemServices)))
				{
					IEnumWbemClassObject	*wbemEnumerator;

					CoSetProxyBlanket(wbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE);

					BSTR classString = SysAllocString(L"Win32_OperatingSystem");
					if (SUCCEEDED(wbemServices->CreateInstanceEnum(classString, WBEM_FLAG_RETURN_IMMEDIATELY | WBEM_FLAG_FORWARD_ONLY, nullptr, &wbemEnumerator)))
					{
						IWbemClassObject	*wbemObject;
						ULONG				count;
						VARIANT				variant;

						if ((SUCCEEDED(wbemEnumerator->Next(1000, 1, &wbemObject, &count))) && (count != 0))
						{
							if (SUCCEEDED(wbemObject->Get(L"Caption", 0, &variant, nullptr, nullptr)))
							{
								if (variant.vt == VT_BSTR)
								{
									Report(String<>(variant.bstrVal), kReportLog);
								}

								VariantClear(&variant);
							}

							if (SUCCEEDED(wbemObject->Get(L"CSDVersion", 0, &variant, nullptr, nullptr)))
							{
								if (variant.vt == VT_BSTR)
								{
									Report(", ", kReportLog);
									Report(String<>(variant.bstrVal), kReportLog);
								}

								VariantClear(&variant);
							}

							wbemObject->Release();
						}

						wbemEnumerator->Release();
					}

					SysFreeString(classString);
					wbemServices->Release();
				}

				SysFreeString(resourceString);
				wbemLocator->Release();
			}

		#elif C4MACOS

			NSString *version = [[NSProcessInfo processInfo] operatingSystemVersionString];
			Report(String<255>("Mac OS X ") += [version UTF8String], kReportLog);

		#elif C4LINUX

			size_t	size;
			int		name[2];

			String<255> version("Linux ");

			size = 249;
			name[0] = CTL_KERN;
			name[1] = KERN_VERSION;
			if (sysctl(name, 2, &version[6], &size, nullptr, 0) == 0)
			{
				version[size + 6] = 0;
				Report(version, kReportLog);
			}

		#elif C4IOS //[ MOBILE

			// -- Mobile code hidden --

		#elif C4PS4 //[ PS4

			// -- PS4 code hidden --

		#elif C4PS3 //[ PS3

			// -- PS3 code hidden --

		#endif //]

		Report("</td></tr>\r\n<tr><th>Hardware</th><td>", kReportLog);

		#if C4WINDOWS

			HKEY	keyHandle;

			if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_QUERY_VALUE, &keyHandle) == ERROR_SUCCESS)
			{
				DWORD	type;
				DWORD	size;
				BYTE	data[256];

				bool line = false;

				size = 255;
				if ((RegQueryValueExA(keyHandle, "ProcessorNameString", 0, &type, data, &size) == ERROR_SUCCESS) && (type == REG_SZ))
				{
					data[size] = 0;
					Report(reinterpret_cast<char *>(data), kReportLog);
					line = true;
				}

				size = 255;
				if ((RegQueryValueExA(keyHandle, "Identifier", 0, &type, data, &size) == ERROR_SUCCESS) && (type == REG_SZ))
				{
					data[size] = 0;

					if (line)
					{
						Report("<br/>", kReportLog);
					}

					Report(reinterpret_cast<char *>(data), kReportLog);
					line = true;
				}

				size = 255;
				if ((RegQueryValueExA(keyHandle, "~MHz", 0, &type, data, &size) == ERROR_SUCCESS) && (type == REG_DWORD))
				{
					if (line)
					{
						Report("<br/>", kReportLog);
					}

					Report(String<31>(*reinterpret_cast<unsigned_int32 *>(data)) += " MHz", kReportLog);
				}
			}

		#elif C4MACOS || C4IOS

			size_t	size;
			int64	result;
			int		name[2];
			char	string[256];

			bool line = false;

			size = 255;
			name[0] = CTL_HW;
			name[1] = HW_MODEL;
			if (sysctl(name, 2, string, &size, nullptr, 0) == 0)
			{
				string[size] = 0;
				Report(string, kReportLog);
				line = true;
			}

			size = 255;
			name[0] = CTL_HW;
			name[1] = HW_MACHINE;
			if (sysctl(name, 2, string, &size, nullptr, 0) == 0)
			{
				string[size] = 0;

				if (line)
				{
					Report("<br/>", kReportLog);
				}

				Report(string, kReportLog);
				line = true;
			}

			size = 8;
			if (sysctlbyname("hw.cpufrequency", &result, &size, nullptr, 0) == 0)
			{
				if (line)
				{
					Report("<br/>", kReportLog);
				}

				Report(String<31>(result / 1000000) += " MHz", kReportLog);
			}

		#elif C4LINUX

			Report("<div style=\"height: 128px; overflow: auto;\"><pre style=\"white-space: pre-wrap;\">", kReportLog);

			int fileDesc = open("/proc/cpuinfo", O_RDONLY, 0);

			char *buffer = new char[4096];
			size = read(fileDesc, buffer, 4095);
			buffer[size] = 0;

			Report(buffer, kReportLog);
			delete[] buffer;

			close(fileDesc);

			Report("</pre></div>", kReportLog);

		#elif C4PS4 //[ PS4

			// -- PS4 code hidden --

		#elif C4PS3 //[ PS3

			// -- PS3 code hidden --

		#endif //]

		Report("</td></tr>\r\n<tr><th>Processor features</th><td>", kReportLog);

		if (processorFlags & kProcessorSSE)
		{
			Report("SSE ", kReportLog);

			if (processorFlags & kProcessorSSE2)
			{
				Report("SSE2 ", kReportLog);

				if (processorFlags & kProcessorSSE3)
				{
					Report("SSE3 ", kReportLog);
				}
			}
		}
		else if (processorFlags & kProcessorNeon)
		{
			Report("Neon ", kReportLog);
		}
		else if (processorFlags & kProcessorAltivec)
		{
			Report("Altivec ", kReportLog);
		}
		else
		{
			Report("&nbsp;", kReportLog);
		}

		Report("</td></tr>\r\n<tr><th>Processor count</th><td>", kReportLog);
		Report(Text::IntegerToString(processorCount), kReportLog);

		Report("</td></tr>\r\n<tr><th>Memory</th><td>", kReportLog);

		#if C4WINDOWS

			MEMORYSTATUSEX		memoryStatus;

			memoryStatus.dwLength = sizeof(MEMORYSTATUSEX);
			GlobalMemoryStatusEx(&memoryStatus);
			Report(String<31>(int64(((memoryStatus.ullTotalPhys >> 20) + 31) & ~31)) += " MB", kReportLog);

		#elif C4MACOS || C4IOS

			int64		value;

			size = 8;
			name[0] = CTL_HW;
			name[1] = HW_MEMSIZE;
			if (sysctl(name, 2, &value, &size, nullptr, 0) == 0)
			{
				Report(String<31>(((value >> 20) + 31) & ~31) += " MB", kReportLog);
			}

		#elif C4LINUX

			int64 value = int64(sysconf(_SC_PHYS_PAGES)) * sysconf(_SC_PAGE_SIZE);
			Report(String<31>(((value >> 20) + 31) & ~31) += " MB", kReportLog);

		#elif C4PS4 //[ PS4

			// -- PS4 code hidden --

		#elif C4PS3 //[ PS3

			// -- PS3 code hidden --

		#endif //]

		Report("</td></tr>\r\n</table>\r\n", kReportLog);
	}

	void Engine::EndLog(void)
	{
		if (logFile.Open())
		{
			Report("</body></html>\r\n", kReportLog);
			logFile.Close();
		}
	}

	void Engine::Logger(const char *text, unsigned_int32 flags, void *cookie)
	{
		if (flags & kReportLog)
		{
			File& file = static_cast<Engine *>(cookie)->logFile;

			if (flags & kReportHeading)
			{
				file << "<div style=\"width: 888px; font-family: tahoma, arial; font-size: 17pt; font-weight: bold; color: black; text-shadow: 0px 2px 3px #555; margin: 32px 0px 15px 0px; padding-left: 0.5em; border-top: solid 1px #666; border-bottom: solid 1px #666; background-color: #AAA; background: -webkit-linear-gradient(top, #CCC, #888); background: -moz-linear-gradient(top, #CCC, #888); background: -ms-linear-gradient(top, #CCC, #888);\">";
				file << text << "</div>\r\n";
			}
			else if (flags & kReportFormatted)
			{
				file << "\r\n<br/><pre>\r\n" << text << "</pre>\r\n";
			}
			else
			{
				if (flags & kReportError)
				{
					file << "<span style=\"color: #800000;\">" << text << "</span>";
				}
				else if (flags & kReportSuccess)
				{
					file << "<span style=\"color: #006000;\">" << text << "</span>";
				}
				else
				{
					file << text;
				}
			}
		}
	}

	void Engine::LogSource(const char *source)
	{
		String<>	string;

		Report("<table cellspacing=\"0\" cellpadding=\"0\" class=\"source\">\r\n", kReportLog);

		int32 line = 1;
		for (;;)
		{
			Report("<tr><td class=\"line\">", kReportLog);
			Report(Text::IntegerToString(line), kReportLog);
			Report("</td><td class=\"source\">", kReportLog);

			int32 len = Text::FindChar(source, 10);
			if (len < 0)
			{
				len = Text::GetTextLength(source);
			}

			int32 xpos = 1;
			int32 start = 0;
			for (machine a = 0; a < len; a++)
			{
				if (source[a] == 9)
				{
					static const char tabs[] = "&nbsp;&nbsp;&nbsp;&nbsp;";

					int32 size = a - start;
					xpos += size;
					Report(string.Set(source + start, size), kReportLog);

					int32 count = 4 - (xpos & 3);
					Report(&tabs[(4 - count) * 6], kReportLog);

					xpos += count;
					start = a + 1;
				}
			}

			if (start != len)
			{
				Report(string.Set(source + start, len - start), kReportLog);
			}

			Report("</td></tr>\r\n", kReportLog);

			if (source[len] == 0)
			{
				break;
			}

			line++;
			source += len + 1;
		}

		Report("</table>\r\n", kReportLog);
	}

	void Engine::LogResult(EngineResult result)
	{
		if (result == kEngineOkay)
		{
			Report("<b>Success</b>", kReportLog | kReportSuccess);
		}
		else
		{
			String<63> message("<b>Error:</b> ");
			message += Text::IntegerToHexString8(result);
			Report(message += "<br/>", kReportLog);

			switch (GetResultManager(result))
			{
				case kManagerEngine:

					Report(Engine::GetInternalResultString(result), kReportLog | kReportError);
					break;

				case kManagerTime:

					Report(TimeMgr::GetInternalResultString(result), kReportLog | kReportError);
					break;

				case kManagerFile:

					Report(FileMgr::GetInternalResultString(result), kReportLog | kReportError);
					break;

				case kManagerResource:

					Report(ResourceMgr::GetInternalResultString(result), kReportLog | kReportError);
					break;

				case kManagerInput:

					Report(InputMgr::GetInternalResultString(result), kReportLog | kReportError);
					break;

				case kManagerDisplay:

					Report(DisplayMgr::GetInternalResultString(result), kReportLog | kReportError);
					break;

				case kManagerGraphics:

					Report(GraphicsMgr::GetInternalResultString(result), kReportLog | kReportError);
					break;

				case kManagerSound:

					Report(SoundMgr::GetInternalResultString(result), kReportLog | kReportError);
					break;

				case kManagerAudioCapture:

					Report(AudioCaptureMgr::GetInternalResultString(result), kReportLog | kReportError);
					break;

				case kManagerInterface:

					Report(InterfaceMgr::GetInternalResultString(result), kReportLog | kReportError);
					break;

				case kManagerMovie:

					Report(MovieMgr::GetInternalResultString(result), kReportLog | kReportError);
					break;

				case kManagerNetwork:

					Report(NetworkMgr::GetInternalResultString(result), kReportLog | kReportError);
					break;

				case kManagerMessage:

					Report(MessageMgr::GetInternalResultString(result), kReportLog | kReportError);
					break;

				case kManagerWorld:

					Report(WorldMgr::GetInternalResultString(result), kReportLog | kReportError);
					break;

				case kManagerPlugin:

					Report(PluginMgr::GetInternalResultString(result), kReportLog | kReportError);
					break;
			}
		}
	}

#endif

const char *Engine::GetExternalResultString(EngineResult result)
{
	if (TheInterfaceMgr)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();

		unsigned_int32 manager = GetResultManager(result);
		unsigned_int32 identifier = 0;

		switch (manager)
		{
			case kManagerEngine:

				identifier = Engine::GetExternalResultIdentifier(result);
				break;

			case kManagerTime:

				identifier = TimeMgr::GetExternalResultIdentifier(result);
				break;

			case kManagerFile:

				identifier = FileMgr::GetExternalResultIdentifier(result);
				break;

			case kManagerResource:

				identifier = ResourceMgr::GetExternalResultIdentifier(result);
				break;

			case kManagerInput:

				identifier = InputMgr::GetExternalResultIdentifier(result);
				break;

			case kManagerDisplay:

				identifier = DisplayMgr::GetExternalResultIdentifier(result);
				break;

			case kManagerGraphics:

				identifier = GraphicsMgr::GetExternalResultIdentifier(result);
				break;

			case kManagerSound:

				identifier = SoundMgr::GetExternalResultIdentifier(result);
				break;

			case kManagerAudioCapture:

				identifier = AudioCaptureMgr::GetExternalResultIdentifier(result);
				break;

			case kManagerInterface:

				identifier = InterfaceMgr::GetExternalResultIdentifier(result);
				break;

			case kManagerMovie:

				identifier = MovieMgr::GetExternalResultIdentifier(result);
				break;

			case kManagerNetwork:

				identifier = NetworkMgr::GetExternalResultIdentifier(result);
				break;

			case kManagerMessage:

				identifier = MessageMgr::GetExternalResultIdentifier(result);
				break;

			case kManagerWorld:

				identifier = WorldMgr::GetExternalResultIdentifier(result);
				break;

			case kManagerPlugin:

				identifier = PluginMgr::GetExternalResultIdentifier(result);
				break;
		}

		if (identifier != 0)
		{
			manager = manager | (manager << 16);
			return (table->GetString(StringID('RSLT', manager, identifier)));
		}
	}

	return (nullptr);
}

const char *Engine::GetDataResultString(DataResult result)
{
	if (TheInterfaceMgr)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();
		return (table->GetString(StringID('RSLT', 'DATA', result)));
	}

	return (nullptr);
}

EngineResult Engine::LoadApplicationModule(void)
{
	String<kMaxFileNameLength>	moduleName;

	#ifdef GAME_MODULE_NAME

		moduleName = GAME_MODULE_NAME;

	#else

		Variable *variable = nullptr;
		for (machine a = 0; a < 2; a++)
		{
			variable = GetVariable("gameModuleName");
			if (variable)
			{
				variable->SetVariableFlags(kVariableNonpersistent);
				variable->AddObserver(&gameModuleObserver);
				moduleName = variable->GetValue();
				break;
			}

			ConfigDataDescription::Execute("game");
		}

		if (!variable)
		{
			moduleName = "Game";
		}

	#endif

	#if C4LOG_FILE

		Report("Application Module", kReportLog | kReportHeading);

		Report("<table class=\"data\" cellspacing=\"0\" cellpadding=\"0\">\r\n<tr><th>", kReportLog);
		Report(moduleName, kReportLog);
		Report("</th><td>\r\n", kReportLog);

	#endif

	ApplicationModule *module = new ApplicationModule;
	EngineResult result = module->Load(moduleName);
	if (result == kEngineOkay)
	{
		applicationModule = module;
	}
	else
	{
		delete module;
		applicationModule = nullptr;
	}

	#if C4LOG_FILE

		LogResult(result);
		Report("</table>\r\n", kReportLog);
		Report("", kReportLog | kReportHeading);

	#endif

	if (result == kEngineOkay)
	{
		TheTimeMgr->AddTask(&inputConfigTask);
	}

	return (result);
}

void Engine::UnloadApplicationModule(void)
{
	delete applicationModule;
	applicationModule = nullptr;
}

void Engine::ExecuteInputConfig(DeferredTask *task, void *cookie)
{
	if (ConfigDataDescription::Execute(C4_INPUT_CONFIG_FILE, TheResourceMgr->GetConfigCatalog()) != kDataOkay)
	{
		ConfigDataDescription::Execute(C4_INPUT_CONFIG_FILE);
	}
}

void Engine::ChangeGameModule(DeferredTask *task, void *cookie)
{
	Engine *engine = static_cast<Engine *>(cookie);

	delete TheApplication;
	engine->UnloadApplicationModule();

	if (engine->LoadApplicationModule() != kEngineOkay)
	{
		engine->Quit();
	}
}

void Engine::HandleGameModuleEvent(Variable *variable)
{
	TheTimeMgr->AddTask(&gameModuleTask);
}

void Engine::ResetMouseButtonMask(void)
{
	if (mouseButtonMask != 0)
	{
		MouseEventData	eventData;

		if (mouseButtonMask & 7)
		{
			if (mouseButtonMask == 1)
			{
				eventData.eventType = kEventMouseUp;
			}
			else if (mouseButtonMask == 2)
			{
				eventData.eventType = kEventRightMouseUp;
			}
			else if (mouseButtonMask == 4)
			{
				eventData.eventType = kEventMiddleMouseUp;
			}

			eventData.eventFlags = 0;
			eventData.mousePosition = lastMousePosition;

			const MouseEventHandler *handler = TheEngine->GetFirstMouseEventHandler();
			while (handler)
			{
				const MouseEventHandler *next = handler->Next();

				if (handler->HandleEvent(&eventData))
				{
					break;
				}

				handler = next;
			}
		}

		mouseButtonMask = 0;
	}
}

#if C4WINDOWS

	LRESULT CALLBACK Engine::WindowProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
	{
		switch (message)
		{
			case WM_ACTIVATEAPP:
			case WM_SHOWWINDOW:
			case WM_SYSCOMMAND:
			case WM_DEVICECHANGE:
			case WM_CLOSE:

				if ((window == TheEngine->engineWindow) && (TheEngine->HandleApplicationEvent(message, wparam, lparam)))
				{
					return (0);
				}

				break;

			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP:
			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP:
			case WM_MBUTTONDOWN:
			case WM_MBUTTONUP:
			case WM_MOUSEWHEEL:
			case WM_MOUSEMOVE:

				if (!(TheDisplayMgr->GetDisplayFlags() & kDisplayFullscreen))
				{
					TheDisplayMgr->HideCursor();
				}

				TheEngine->HandleMouseEvent(window, message, wparam, lparam);
				return (0);

			case WM_NCMOUSEMOVE:

				if (!(TheDisplayMgr->GetDisplayFlags() & kDisplayFullscreen))
				{
					TheDisplayMgr->ShowCursor();
				}

				break;

			case WM_KEYDOWN:
			case WM_KEYUP:

				TheEngine->HandleKeyboardEvent(window, message, wparam, lparam);
				return (0);

			case WM_CHAR:

				TheEngine->deadKeyFlag = false;

				if (wparam >= 0x00A0)
				{
					KeyboardEventData	eventData;

					eventData.eventType = kEventKeyDown;
					eventData.keyCode = (unsigned_int32) wparam;
					eventData.modifierKeys = (((lparam >> 16) & 0xFF) != DIK_GRAVE) ? 0 : kModifierKeyConsole;

					const KeyboardEventHandler *handler = TheEngine->keyboardEventHandlerList.First();
					while (handler)
					{
						const KeyboardEventHandler *next = handler->Next();

						if (handler->HandleEvent(&eventData))
						{
							break;
						}

						handler = next;
					}

					return (0);
				}

				break;

			case WM_DEADCHAR:

				TheEngine->deadKeyFlag = true;
				break;

			case WM_SYSKEYDOWN:
			case WM_SYSKEYUP:

				if (InputMgr::GetInternalInputMode() & kInputKeyboardActive)
				{
					return (0);
				}

				break;

			case WM_ERASEBKGND:

				return (1);

			case WM_INPUT:

				if (TheEngine->multiaxisMouseActiveCount > 0)
				{
					TheEngine->HandleMultiaxisMouseEvent(wparam, lparam);
				}

				break;

			case WT_PACKET:
			case WT_PROXIMITY:

				if (TheEngine->tabletActiveCount > 0)
				{
					TheEngine->HandleTabletEvent(message, wparam, lparam);
				}

				return (0);

			case kWindowsMessageResolve:

				DomainNameResolver::ResolveCallback((HANDLE) wparam, WSAGETASYNCERROR(lparam));
				return (0);
		}

		return (DefWindowProcW(window, message, wparam, lparam));
	}

	bool Engine::HandleApplicationEvent(UINT message, WPARAM wparam, LPARAM lparam)
	{
		if (message == WM_ACTIVATEAPP)
		{
			if (wparam)
			{
				engineFlags |= kEngineForeground;

				if ((TheDisplayMgr) && (TheDisplayMgr->GetDisplayFlags() & kDisplayFullscreen))
				{
					ShowWindow(engineWindow, SW_RESTORE);
				}

				if (TheInterfaceMgr)
				{
					TheInterfaceMgr->ReadSystemClipboard();
				}
			}
			else
			{
				engineFlags &= ~kEngineForeground;

				if ((TheDisplayMgr) && (TheDisplayMgr->GetDisplayFlags() & kDisplayFullscreen))
				{
					ShowWindow(engineWindow, SW_MINIMIZE);
				}

				if (TheInterfaceMgr)
				{
					TheInterfaceMgr->WriteSystemClipboard();
				}
			}
		}
		else if (message == WM_SHOWWINDOW)
		{
			if (wparam)
			{
				engineFlags |= kEngineVisible;
			}
			else
			{
				engineFlags &= ~(kEngineForeground | kEngineVisible);
			}
		}
		else if (message == WM_SYSCOMMAND)
		{
			wparam &= 0xFFF0;
			if ((wparam != SC_SCREENSAVE) && (wparam != SC_MONITORPOWER) && (wparam != SC_KEYMENU))
			{
				return (false);
			}
		}
		else if (message == WM_CLOSE)
		{
			if ((TheInterfaceMgr) && (TheInterfaceMgr->QuitEnabled()))
			{
				Quit();
			}
		}

		return (true);
	}

	bool Engine::HandleWindowEvent(UINT message, WPARAM wparam, LPARAM lparam)
	{
		return (false);
	}

	void Engine::HandleMouseEvent(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
	{
		if (!(InputMgr::GetInternalInputMode() & kInputMouseActive))
		{
			MouseEventData		eventData;
			POINT				point;

			point.x = (int16) LOWORD(lparam);
			point.y = (int16) HIWORD(lparam);

			eventData.eventType = kEventNone;

			switch (message)
			{
				case WM_LBUTTONDOWN:

					if ((mouseButtonMask & ~1) == 0)
					{
						mouseButtonMask = 1;
						eventData.eventType = kEventMouseDown;
						SetCapture(window);
					}

					break;

				case WM_LBUTTONUP:

					if (mouseButtonMask == 1)
					{
						mouseButtonMask = 0;
						eventData.eventType = kEventMouseUp;
						ReleaseCapture();
					}

					break;

				case WM_RBUTTONDOWN:

					if ((mouseButtonMask & ~2) == 0)
					{
						mouseButtonMask = 2;
						eventData.eventType = kEventRightMouseDown;
						SetCapture(window);
					}

					break;

				case WM_RBUTTONUP:

					if (mouseButtonMask == 2)
					{
						mouseButtonMask = 0;
						eventData.eventType = kEventRightMouseUp;
						ReleaseCapture();
					}

					break;

				case WM_MBUTTONDOWN:

					if ((mouseButtonMask & ~4) == 0)
					{
						mouseButtonMask = 4;
						eventData.eventType = kEventMiddleMouseDown;
						SetCapture(window);
					}

					break;

				case WM_MBUTTONUP:

					if (mouseButtonMask == 4)
					{
						mouseButtonMask = 0;
						eventData.eventType = kEventMiddleMouseUp;
						ReleaseCapture();
					}

					break;

				case WM_MOUSEWHEEL:
				{
					int32 delta = wheelDeltaAccum + GET_WHEEL_DELTA_WPARAM(wparam);
					int32 k = delta / WHEEL_DELTA;
					if (k != 0)
					{
						eventData.eventType = kEventMouseWheel;
						eventData.wheelDelta.Set(0.0F, (float) k);
						delta -= k * WHEEL_DELTA;
					}

					wheelDeltaAccum = delta;

					ScreenToClient(engineWindow, &point);
					break;
				}

				default:

					eventData.eventType = kEventMouseMoved;
					break;
			}

			if (eventData.eventType != kEventNone)
			{
				eventData.eventFlags = 0;
				eventData.mousePosition.Set((float) point.x, (float) point.y, 0.0F);
				lastMousePosition = eventData.mousePosition;

				const MouseEventHandler *handler = mouseEventHandlerList.First();
				while (handler)
				{
					const MouseEventHandler *next = handler->Next();

					if (handler->HandleEvent(&eventData))
					{
						break;
					}

					handler = next;
				}
			}
		}
	}

	void Engine::HandleKeyboardEvent(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
	{
		if (!(InputMgr::GetInternalInputMode() & kInputKeyboardActive))
		{
			alignas(16) static const unsigned_int8 keyCodeMap[48] =
			{
				0, 0, 0, 0, 0, 0, 0, 0, kKeyCodeBackspace, kKeyCodeTab, 0, 0, 0, kKeyCodeEnter, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, kKeyCodeEscape, 0, 0, 0, 0,
				0, kKeyCodePageUp, kKeyCodePageDown, kKeyCodeEnd, kKeyCodeHome, kKeyCodeLeftArrow, kKeyCodeUpArrow, kKeyCodeRightArrow, kKeyCodeDownArrow, 0, 0, 0, 0, 0, kKeyCodeDelete, 0
			};

			KeyboardEventData	eventData;
			BYTE				keyState[256];

			GetKeyboardState(keyState);
			eventData.modifierKeys = (keyState[VK_SHIFT] & 0x80) ? kModifierKeyShift : 0;

			unsigned_int32 scan = (lparam >> 16) & 0xFF;
			if ((scan == DIK_GRAVE) && (eventData.modifierKeys == 0))
			{
				eventData.modifierKeys = kModifierKeyConsole;
			}

			bool command = false;
			if ((keyState[VK_CONTROL] & 0x80) && (!(keyState[VK_RMENU] & 0x80)))
			{
				command = true;
				keyState[VK_SHIFT] = 0;
				keyState[VK_LSHIFT] = 0;
				keyState[VK_RSHIFT] = 0;
				keyState[VK_CONTROL] = 0;
				keyState[VK_LCONTROL] = 0;
				keyState[VK_RCONTROL] = 0;
			}

			unsigned_int32 code = (wparam < 0x0030) ? keyCodeMap[wparam] : 0;
			if (code == 0)
			{
				if (wparam - VK_F1 >= 24U)
				{
					WCHAR buffer[2] = {0};
					if (ToUnicode((UINT) wparam, scan, keyState, buffer, 2, 0) == 1)
					{
						code = buffer[0];
					}
				}
				else
				{
					code = unsigned_int32(wparam + (kKeyCodeF1 - VK_F1));
				}
			}

			if (code - 1 < 0x007FU)
			{
				eventData.eventType = kEventNone;

				if (message == WM_KEYDOWN)
				{
					if (command)
					{
						eventData.eventType = kEventKeyCommand;
						if (code - 0x0061 < 26U)
						{
							code -= 0x0020;
						}
					}
					else
					{
						if ((code < 0x0020) || (!deadKeyFlag))
						{
							eventData.eventType = kEventKeyDown;
						}
					}
				}
				else if (!command)
				{
					eventData.eventType = kEventKeyUp;
				}

				if (eventData.eventType != kEventNone)
				{
					eventData.keyCode = code;

					const KeyboardEventHandler *handler = keyboardEventHandlerList.First();
					while (handler)
					{
						const KeyboardEventHandler *next = handler->Next();

						if (handler->HandleEvent(&eventData))
						{
							break;
						}

						handler = next;
					}
				}
			}
		}
	}

	void Engine::HandleMultiaxisMouseEvent(WPARAM wparam, LPARAM lparam)
	{
		char	data[256];

		UINT size = 256;
		if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lparam), RID_INPUT, data, &size, sizeof(RAWINPUTHEADER)) - 1 < 256U)
		{
			const RAWINPUT *rawInput = reinterpret_cast<RAWINPUT *>(data);
			if (rawInput->header.dwType == RIM_TYPEHID)
			{
				RID_DEVICE_INFO		deviceInfo;

				deviceInfo.cbSize = sizeof(RID_DEVICE_INFO);
				size = sizeof(RID_DEVICE_INFO);
				if (GetRawInputDeviceInfoA(rawInput->header.hDevice, RIDI_DEVICEINFO, &deviceInfo, &size) - 1 < sizeof(RID_DEVICE_INFO))
				{
					if (deviceInfo.hid.dwVendorId == 0x046D)		// LOGITECH_VENDOR_ID
					{
						MouseEventData	eventData;

						eventData.eventType = kEventNone;
						eventData.eventFlags = 0;

						const BYTE *raw = rawInput->data.hid.bRawData;
						int32 type = raw[0];
						if (type == 1)
						{
							int32 dx = raw[1] | (reinterpret_cast<const char *>(raw)[2] << 8);
							int32 dy = raw[3] | (reinterpret_cast<const char *>(raw)[4] << 8);
							int32 dz = raw[5] | (reinterpret_cast<const char *>(raw)[6] << 8);

							eventData.eventType = kEventMultiaxisMouseTranslation;
							eventData.mousePosition.Set((float) dx * 3.2e-5F, (float) dy * 3.2e-5F, (float) dz * 3.2e-5F);
						}
						else if (type == 2)
						{
							int32 rx = raw[1] | (reinterpret_cast<const char *>(raw)[2] << 8);
							int32 ry = raw[3] | (reinterpret_cast<const char *>(raw)[4] << 8);
							int32 rz = raw[5] | (reinterpret_cast<const char *>(raw)[6] << 8);

							eventData.eventType = kEventMultiaxisMouseRotation;
							eventData.mousePosition.Set((float) rx * 8.0e-6F, (float) ry * 8.0e-6F, (float) rz * 8.0e-6F);
						}
						else if (type == 3)
						{
							eventData.eventType = kEventMultiaxisMouseButtonState;
							eventData.eventFlags = raw[1] | (raw[2] << 8) | (raw[3] << 16) | (raw[4] << 24);
						}

						if (eventData.eventType != kEventNone)
						{
							const MouseEventHandler *handler = mouseEventHandlerList.First();
							while (handler)
							{
								const MouseEventHandler *next = handler->Next();

								if (handler->HandleEvent(&eventData))
								{
									break;
								}

								handler = next;
							}
						}
					}
				}
			}
		}
	}

	void Engine::HandleTabletEvent(UINT message, WPARAM wparam, LPARAM lparam)
	{
		if (!(InputMgr::GetInternalInputMode() & kInputMouseActive))
		{
			if (message == WT_PACKET)
			{
				TabletPacket	packet[8];

				int32 count = WTPacketsGet((HCTX) lparam, 8, packet);
				if (count > 0)
				{
					unsigned_int32 pressure = packet[count - 1].normalPressure - stylusMinPressure;
					stylusPressure = (float) pressure * stylusPressureScale;
				}
			}
			else if (message == WT_PROXIMITY)
			{
				if (LOWORD(lparam) == 0)
				{
					stylusPressure = 0.0F;
				}
			}
		}
	}

	bool Engine::PluginFilter(const char *name, unsigned_int32 flags, void *cookie)
	{
		if (name[0] == '.')
		{
			return (false);
		}

		if (flags & kFileDirectory)
		{
			return (true);
		}

		int32 length = Text::GetTextLength(name);
		if (length < 5)
		{
			return (false);
		}

		return (Text::CompareTextCaseless(&name[length - 4], ".dll"));
	}

#elif C4MACOS

	void Engine::HandleApplicationForegroundEvent(void)
	{
		engineFlags |= kEngineForeground;

		if (TheInterfaceMgr)
		{
			TheInterfaceMgr->ReadSystemClipboard();
		}
	}

	void Engine::HandleApplicationBackgroundEvent(void)
	{
		engineFlags &= ~kEngineForeground;

		if (TheInterfaceMgr)
		{
			TheInterfaceMgr->WriteSystemClipboard();
		}
	}

	void Engine::HandleMouseEvent(NSEvent *event)
	{
		if (!(InputMgr::GetInternalInputMode() & kInputMouseActive))
		{
			MouseEventData		eventData;

			NSPoint position = [event locationInWindow];
			eventData.mousePosition.Set(Floor(position.x), (float) TheDisplayMgr->GetDisplayHeight() - Floor(position.y), 0.0F);
			lastMousePosition = eventData.mousePosition;

			bool tabletFlag = (tabletActiveCount > 0);

			NSEventType type = [event type];
			if (type == NSLeftMouseDown)
			{
				if (([event modifierFlags] & NSControlKeyMask) == 0)
				{
					if ((mouseButtonMask & ~1) == 0)
					{
						mouseButtonMask = 1;
						eventData.eventType = kEventMouseDown;
					}
				}
				else
				{
					if ((mouseButtonMask & ~2) == 0)
					{
						mouseButtonMask = 2;
						eventData.eventType = kEventRightMouseDown;
					}
				}
			}
			else if (type == NSRightMouseDown)
			{
				if ((mouseButtonMask & ~2) == 0)
				{
					mouseButtonMask = 2;
					eventData.eventType = kEventRightMouseDown;
				}
			}
			else if (type == NSOtherMouseDown)
			{
				if ([event buttonNumber] == 2)
				{
					if ((mouseButtonMask & ~4) == 0)
					{
						mouseButtonMask = 4;
						eventData.eventType = kEventMiddleMouseDown;
					}
				}
			}
			else if (type == NSLeftMouseUp)
			{
				if (mouseButtonMask == 1)
				{
					mouseButtonMask = 0;
					eventData.eventType = kEventMouseUp;
				}
				else if (mouseButtonMask == 2)
				{
					mouseButtonMask = 0;
					eventData.eventType = kEventRightMouseUp;
				}
			}
			else if (type == NSRightMouseUp)
			{
				if (mouseButtonMask == 2)
				{
					mouseButtonMask = 0;
					eventData.eventType = kEventRightMouseUp;
				}
			}
			else if (type == NSOtherMouseUp)
			{
				if ([event buttonNumber] == 2)
				{
					if (mouseButtonMask == 4)
					{
						mouseButtonMask = 0;
						eventData.eventType = kEventMiddleMouseUp;
					}
				}
			}
			else if (type == NSScrollWheel)
			{
				CGFloat delta = wheelDeltaAccum + [event deltaY];
				float y = Floor(Fabs(delta));
				if (y != 0.0F)
				{
					y *= NonzeroFsgn(delta);
					eventData.eventType = kEventMouseWheel;
					eventData.wheelDelta.Set(0.0F, y);
					delta -= y;
				}

				wheelDeltaAccum = delta;
				tabletFlag = false;
			}
			else
			{
				eventData.eventType = kEventMouseMoved;
			}

			eventData.eventFlags = 0;

			if (tabletFlag)
			{
				short subtype = [event subtype];
				if (subtype == NSTabletPointEventSubtype)
				{
					stylusPressure = [event pressure];
				}
				else if (subtype == NSTabletProximityEventSubtype)
				{
					if ([event isEnteringProximity] == NO)
					{
						stylusPressure = 0.0F;
					}
				}
			}

			const MouseEventHandler *handler = mouseEventHandlerList.First();
			while (handler)
			{
				const MouseEventHandler *next = handler->Next();

				if (handler->HandleEvent(&eventData))
				{
					break;
				}

				handler = next;
			}
		}
	}

	void Engine::HandleKeyboardEvent(NSEvent *event)
	{
		if (!(InputMgr::GetInternalInputMode() & kInputKeyboardActive))
		{
			alignas(16) static const unsigned_int8 keyCodeMap1[32] =
			{
				0, 0, 0, kKeyCodeEnter, 0, 0, 0, 0, kKeyCodeBackspace, kKeyCodeTab, 0, 0, 0, kKeyCodeEnter, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, kKeyCodeTab, 0, kKeyCodeEscape, 0, 0, 0, 0
			};

			alignas(16) static const unsigned_int8 keyCodeMap2[72] =
			{
				kKeyCodeUpArrow, kKeyCodeDownArrow, kKeyCodeLeftArrow, kKeyCodeRightArrow, kKeyCodeF1, kKeyCodeF2, kKeyCodeF3, kKeyCodeF4, kKeyCodeF5, kKeyCodeF6, kKeyCodeF7, kKeyCodeF8, kKeyCodeF9, kKeyCodeF10, kKeyCodeF11, kKeyCodeF12,
				kKeyCodeF13, kKeyCodeF14, kKeyCodeF15, kKeyCodeF16, kKeyCodeF17, kKeyCodeF18, kKeyCodeF19, kKeyCodeF20, kKeyCodeF21, kKeyCodeF22, kKeyCodeF23, kKeyCodeF24, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, kKeyCodeDelete, kKeyCodeHome, 0, kKeyCodeEnd, kKeyCodePageUp, kKeyCodePageDown, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0
			};

			KeyboardEventData		eventData;

			NSEventType type = [event type];
			NSUInteger modifiers = [event modifierFlags];

			eventData.eventType = kEventNone;
			eventData.modifierKeys = (modifiers & NSShiftKeyMask) ? kModifierKeyShift : 0;

			if (([event keyCode] == 0x32) && (eventData.modifierKeys == 0))
			{
				eventData.modifierKeys = kModifierKeyConsole;
			}

			if (type == NSKeyDown)
			{
				eventData.eventType = (modifiers & NSCommandKeyMask) ? kEventKeyCommand : kEventKeyDown;
			}
			else if (type == NSKeyUp)
			{
				eventData.eventType = kEventKeyUp;
			}

			if (eventData.eventType != kEventNone)
			{
				NSString *string = [event characters];
				unsigned_int32 length = [string length];

				for (unsigned_machine a = 0; a < length; a++)
				{
					unichar code = [string characterAtIndex: a];

					if (unsigned_int32(code - 0x0020) >= 0x005FU)
					{
						if (code < 0x0020)
						{
							code = keyCodeMap1[code];
						}
						else if (code == NSDeleteCharacter)
						{
							code = kKeyCodeBackspace;
						}
						else if (code - 0xF700 < 0x0200U)
						{
							if (code < 0xF748)
							{
								code = keyCodeMap2[code - 0xF700];
							}
							else
							{
								code = 0;
							}
						}
					}

					if (eventData.eventType == kEventKeyCommand)
					{
						if (code - 0x0061 < 26U)
						{
							code -= 0x0020;
						}
					}

					if (code != 0)
					{
						eventData.keyCode = code;

						const KeyboardEventHandler *handler = keyboardEventHandlerList.First();
						while (handler)
						{
							const KeyboardEventHandler *next = handler->Next();

							if (handler->HandleEvent(&eventData))
							{
								break;
							}

							handler = next;
						}
					}
				}
			}
		}
	}

	void Engine::HandleTabletEvent(NSEvent *event)
	{
		NSEventType type = [event type];
		if (type == NSTabletPoint)
		{
			stylusPressure = [event pressure];
		}
		else if (type == NSTabletProximity)
		{
			if ([event isEnteringProximity] == NO)
			{
				stylusPressure = 0.0F;
			}
		}
	}

	void *Engine::GetBundleFunctionAddress(CFBundleRef bundle, const char *name)
	{
		CFStringRef string = CFStringCreateWithCStringNoCopy(kCFAllocatorDefault, name, kCFStringEncodingASCII, kCFAllocatorNull);
		void *address = CFBundleGetFunctionPointerForName(bundle, string);
		CFRelease(string);

		return (address);
	}

	bool Engine::PluginFilter(const char *name, unsigned_int32 flags, void *cookie)
	{
		if (name[0] == '.')
		{
			return (false);
		}

		if (flags & kFileDirectory)
		{
			return (true);
		}

		int32 length = Text::GetTextLength(name);
		if (length < 7)
		{
			return (false);
		}

		return (Text::CompareTextCaseless(&name[length - 6], ".dylib"));
	}

#elif C4LINUX

	void Engine::HandleApplicationEvent(const XEvent *event)
	{
		int type = event->type;
		if (type == ClientMessage)
		{
			if (event->xclient.data.l[0] == deleteWindowAtom)
			{
				if ((TheInterfaceMgr) && (TheInterfaceMgr->QuitEnabled()))
				{
					Quit();
				}
			}
		}
		else if (type == MappingNotify)
		{
			int request = event->xmapping.request;
			if ((request == MappingModifier) || (request == MappingKeyboard))
			{
				XRefreshKeyboardMapping(const_cast<XMappingEvent *>(&event->xmapping));
			}
		}
	}

	void Engine::HandleMouseEvent(EventType eventType, const Integer2D& point, int32 delta)
	{
		MouseEventData		eventData;

		eventData.eventType = eventType;
		eventData.eventFlags = 0;

		eventData.mousePosition.Set((float) point.x, (float) point.y, 0.0F);
		eventData.wheelDelta.Set(0.0F, (float) delta);
		lastMousePosition = eventData.mousePosition;

		const MouseEventHandler *handler = mouseEventHandlerList.First();
		while (handler)
		{
			const MouseEventHandler *next = handler->Next();

			if (handler->HandleEvent(&eventData))
			{
				break;
			}

			handler = next;
		}
	}

	void Engine::HandleKeyboardEvent(EventType eventType, unsigned_int32 code, unsigned_int32 modifierKeys)
	{
		if (code < 0x010000)
		{
			KeyboardEventData	eventData;

			if ((code == '`') && (modifierKeys == 0))
			{
				modifierKeys = kModifierKeyConsole;
			}

			eventData.eventType = eventType;
			eventData.keyCode = code;
			eventData.modifierKeys = modifierKeys;

			const KeyboardEventHandler *handler = keyboardEventHandlerList.First();
			while (handler)
			{
				const KeyboardEventHandler *next = handler->Next();

				if (handler->HandleEvent(&eventData))
				{
					break;
				}

				handler = next;
			}
		}
	}

	bool Engine::PluginFilter(const char *name, unsigned_int32 flags, void *cookie)
	{
		if (name[0] == '.')
		{
			return (false);
		}

		if (flags & kFileDirectory)
		{
			return (true);
		}

		int32 length = Text::GetTextLength(name);
		if (length < 4)
		{
			return (false);
		}

		return (Text::CompareTextCaseless(&name[length - 3], ".so"));
	}

#elif C4IOS //[ MOBILE

	// -- Mobile code hidden --

#elif C4PS4 //[ PS4

	// -- PS4 code hidden --

#elif C4PS3 //[ PS3

	// -- PS3 code hidden --

#endif //]

#if C4DESKTOP

	void Engine::GetPluginMap(const char *directory, Map<FileReference> *fileMap) const
	{
		#if !C4MACOS

			String<kMaxFileNameLength> pathName("Plugins");

		#else

			String<kMaxFileNameLength> pathName(TheFileMgr->GetPluginsPath());
			pathName += "Plugins";

		#endif

		if (directory[0] != 0)
		{
			pathName += '/';
		}

		FileMgr::BuildFileMap(pathName += directory, fileMap, &PluginFilter);
	}

#endif

Variable *Engine::InitVariable(const char *name, const char *value, unsigned_int32 flags, Variable::ObserverType *observer)
{
	Variable *variable = GetVariable(name);
	if (!variable)
	{
		variable = new Variable(name, flags, observer);
		variable->SetValue(value);
		AddVariable(variable);
	}
	else
	{
		variable->SetVariableFlags(flags);

		if (observer)
		{
			variable->AddObserver(observer);
			observer->InvokeCallback(variable);
		}
	}

	return (variable);
}

void Engine::ExecuteCommand(const char *text)
{
	int32	length;

	bool variableFlag = false;
	if (text[0] == '$')
	{
		variableFlag = true;
		text++;
	}

	if ((Data::ReadIdentifier(text, &length) == kDataOkay) && (length <= kMaxCommandLength))
	{
		String<kMaxCommandLength>	name;

		Data::ReadIdentifier(text, &length, name);

		text += length;
		text += Data::GetWhitespaceLength(text);

		if (!variableFlag)
		{
			Command *command = commandMap.Find(name);
			if (command)
			{
				command->PostEvent(text);
			}
			else
			{
				Report(String<kMaxCommandLength + 64>("[#FF8]Unrecognized command: [#FFF]") += name, kReportError);
			}
		}
		else
		{
			Variable *variable = GetVariable(name);
			if (*text == '=')
			{
				String<>	value;

				if (!variable)
				{
					variable = new Variable(name);
					AddVariable(variable);
				}

				text++;
				text += Data::GetWhitespaceLength(text);

				if (StringDataType::ParseValue(text, &value) == kDataOkay)
				{
					variable->SetValue(value);
				}
			}
			else
			{
				if (variable)
				{
					String<kMaxCommandLength + kMaxVariableValueLength + 5> string("$");
					string += name;
					string += " = \"";
					string += variable->GetValue();
					string += '\"';
					Report(string);
				}
				else
				{
					Report(String<kMaxCommandLength + 64>("[#FF8]Undefined variable: [#FFF]$") += name, kReportError);
				}
			}
		}
	}
}

void Engine::ExecuteText(const char *text)
{
	if (text)
	{
		for (;;)
		{
			text += Data::GetWhitespaceLength(text);
			if (*text == 0)
			{
				return;
			}

			int32 length = Text::FindUnquotedChar(text, ';');
			if (length < 0)
			{
				ExecuteCommand(text);
				break;
			}

			if (length > 0)
			{
				if (length > 63)
				{
					String<>	line;

					line.Set(text, length);
					ExecuteCommand(line);
				}
				else
				{
					String<63>	line;

					line.Set(text, length);
					ExecuteCommand(line);
				}
			}

			text += length + 1;
		}
	}
}

bool Engine::ExecuteFile(const char *name, ResourceCatalog *catalog)
{
	ConfigResource *config = ConfigResource::Get(name, 0, catalog);
	if (config)
	{
		ExecuteText(config->GetText());
		config->Release();
		return (true);
	}

	return (false);
}

void Engine::Report(const char *text, unsigned_int32 flags)
{
	Reporter *reporter = reporterList.Last();
	while (reporter)
	{
		reporter->Report(text, flags);
		reporter = reporter->Previous();
	}
}

bool Engine::Run(void)
{
	#if C4SIMD && C4SSE

		_mm_setcsr(_mm_getcsr() | 0x8040);

	#endif

	for (;;)
	{
		TheTimeMgr->TimeTask();

		if (engineFlags & kEngineQuit)
		{
			return (false);
		}

		#if C4WINDOWS

			MSG		message;

			while (PeekMessageA(&message, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&message);
				DispatchMessageA(&message);
			}

		#elif C4LINUX

			while (XPending(engineDisplay))
			{
				XEvent		event;

				XNextEvent(engineDisplay, &event);
				switch (event.type)
				{
					case ClientMessage:
					case MappingNotify:

						HandleApplicationEvent(&event);
						break;

					case ButtonPress:
					case ButtonRelease:

						TheInputMgr->HandleMouseButtonEvent(&event.xbutton);
						break;

					case MotionNotify:

						TheInputMgr->HandleMouseMotionEvent(&event.xmotion);
						break;

					case KeyPress:
					case KeyRelease:

						TheInputMgr->HandleKeyboardEvent(&event.xkey);
						break;
				}
			}

			if (TheInputMgr->GetMouseMotionEventFlag())
			{
				HandleMouseEvent(kEventMouseMoved, TheInputMgr->GetMouseMotionPosition());
			}

		#elif C4PS3 //[ PS3

			// -- PS3 code hidden --

		#endif //]

		if (engineFlags & kEngineForeground)
		{
			TheInputMgr->InputTask();
		}

		TheMessageMgr->ReceiveTask();
		TheInterfaceMgr->InterfaceTask();
		TheAudioCaptureMgr->AudioCaptureTask();

		TheApplication->ApplicationTask();
		TheWorldMgr->Move();
		TheMessageMgr->SendTask();
		TheSoundMgr->SoundTask();

		if (engineFlags & kEngineVisible)
		{
			TheWorldMgr->Render();
		}

		#if C4DESKTOP

			bool multiplayerServer = (TheMessageMgr->Multiplayer()) & (TheMessageMgr->Server());
			if ((!multiplayerServer) && (!(engineFlags & kEngineForeground)))
			{
				int32 dt = TheTimeMgr->GetDeltaTime();
				int32 sleepTime = Max(backgroundSleepTime + (40 - dt) / 2, 1);
				backgroundSleepTime = sleepTime;
				Thread::Sleep(sleepTime);
			}

		#endif

		#if C4MACOS || C4IOS

			return (true);

		#endif
	}
}

void Engine::Quit(void)
{
	TheMessageMgr->DisconnectAll();
	engineFlags |= kEngineQuit;
}

void Engine::StartMultiaxisMouse(void)
{
	if (++multiaxisMouseActiveCount == 1)
	{
		#if C4WINDOWS

			RAWINPUTDEVICE		device;

			device.usUsagePage = 1;
			device.usUsage = 8;
			device.dwFlags = 0;
			device.hwndTarget = engineWindow;

			RegisterRawInputDevices(&device, 1, sizeof(RAWINPUTDEVICE));

		#endif
	}
}

void Engine::StopMultiaxisMouse(void)
{
	if (--multiaxisMouseActiveCount == 0)
	{
		#if C4WINDOWS

			RAWINPUTDEVICE		device;

			device.usUsagePage = 1;
			device.usUsage = 8;
			device.dwFlags = RIDEV_REMOVE;
			device.hwndTarget = nullptr;

			RegisterRawInputDevices(&device, 1, sizeof(RAWINPUTDEVICE));

		#endif
	}
}

void Engine::StartTablet(void)
{
	if (++tabletActiveCount == 1)
	{
		#if C4WINDOWS

			tabletLibrary = LoadLibraryA("Wintab32.dll");
			if (tabletLibrary)
			{
				TabletLogContext	logicalContext;

				*(void **) &WTInfoA = GetProcAddress(tabletLibrary, "WTInfoA");
				*(void **) &WTOpenA = GetProcAddress(tabletLibrary, "WTOpenA");
				*(void **) &WTClose = GetProcAddress(tabletLibrary, "WTClose");
				*(void **) &WTPacketsGet = GetProcAddress(tabletLibrary, "WTPacketsGet");

				if (WTInfoA(WTI_DEFSYSCTX, 0, &logicalContext) != 0)
				{
					logicalContext.lcOptions = CXO_SYSTEM | CXO_MESSAGES;
					logicalContext.lcMsgBase = WT_DEFBASE;
					logicalContext.lcPktData = PK_NORMAL_PRESSURE;
					logicalContext.lcMoveMask = PK_NORMAL_PRESSURE;

					tabletContext = WTOpenA(engineWindow, &logicalContext, true);
					if (tabletContext)
					{
						TabletAxis	pressureRange;

						WTInfoA(WTI_DEVICES, DVC_NPRESSURE, &pressureRange);
						stylusMinPressure = pressureRange.axMin;
						stylusPressureScale = 1.0F / (float) (pressureRange.axMax - pressureRange.axMin);
					}
				}
				else
				{
					tabletContext = nullptr;
				}
			}

		#endif
	}
}

void Engine::StopTablet(void)
{
	if (--tabletActiveCount == 0)
	{
		#if C4WINDOWS

			if (tabletLibrary)
			{
				if (tabletContext)
				{
					WTClose(tabletContext);
				}

				FreeLibrary(tabletLibrary);
			}

		#endif
	}

	stylusPressure = 0.0F;
}

bool Engine::OpenExternalWebBrowser(const char *url)
{
	#if C4WINDOWS

		DWORD	code;

		HANDLE threadHandle = CreateThread(nullptr, 0, &ShellThread, &url, 0, nullptr);
		WaitForSingleObjectEx(threadHandle, INFINITE, false);

		bool result = ((GetExitCodeThread(threadHandle, &code)) && (code == 0));

		CloseHandle(threadHandle);
		return (result);

	#elif C4MACOS

		CFURLRef urlRef = CFURLCreateWithBytes(kCFAllocatorDefault, reinterpret_cast<const UInt8 *>(url), Text::GetTextLength(url), kCFStringEncodingUTF8, nullptr);
		const void *arrayValue = urlRef;
		CFArrayRef urlArray = CFArrayCreate(kCFAllocatorDefault, &arrayValue, 1, &kCFTypeArrayCallBacks);

		bool result = (LSOpenURLsWithRole(urlArray, kLSRolesAll, nullptr, nullptr, nullptr, 0) == noErr);

		CFRelease(urlArray);
		CFRelease(urlRef);
		return (result);

	#elif C4IOS //[ MOBILE

		// -- Mobile code hidden --

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#else //]

		return (false);

	#endif
}

#if C4WINDOWS

	DWORD WINAPI Engine::ShellThread(void *cookie)
	{
		SHELLEXECUTEINFOA	info;

		CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

		info.cbSize = sizeof(SHELLEXECUTEINFOA);
		info.fMask = SEE_MASK_NOASYNC | SEE_MASK_FLAG_NO_UI;
		info.hwnd = nullptr;
		info.lpVerb = "open";
		info.lpFile = *reinterpret_cast<const char **>(cookie);
		info.lpParameters = nullptr;
		info.lpDirectory = nullptr;
		info.nShow = SW_SHOWNORMAL;

		DWORD result = 1;
		if (ShellExecuteExA(&info))
		{
			result = ((machine_address) info.hInstApp <= 32);
		}

		CoUninitialize();
		return (result);
	}

#endif

#if C4LEAK_DETECTION

	void Engine::DumpMemory(const char *filename)
	{
		File	file;

		if (file.Open(filename, kFileCreate) == kFileOkay)
		{
			const Heap *heap = MemoryMgr::GetFirstHeap();
			while (heap)
			{
				file << "Heap: " << heap->GetHeapName() << "\r\n";

				int32 poolCount = 0;
				const MemPoolHeader *pool = heap->GetFirstPool();
				while (pool)
				{
					file << "Pool #" << String<15>(poolCount) << "\r\n";

					const MemBlockHeader *block = pool->GetFirstBlock();
					do
					{
						if (block->blockFlags & kMemoryBlockUsed)
						{
							String<15> line(block->allocLine);
							String<15> size(block->logicalSize);
							file << "\t" << block->allocFile << "\t\tline " << (const char *) line << "\t\t" << (const char *) size << " bytes\r\n";
						}

						block = block->nextBlock;
					} while (block);

					poolCount++;
					pool = pool->nextPool;
				}

				file << "\r\n";
				heap = heap->GetNextHeap();
			}

			file << "System Blocks\r\n";
			const MemBlockHeader *block = MemoryMgr::GetFirstSystemBlock();
			while (block)
			{
				String<15> line(block->allocLine);
				String<15> size(block->logicalSize);
				file << "\t" << block->allocFile << "\t\tline " << (const char *) line << "\t\t" << (const char *) size << " bytes\r\n";
				block = block->nextBlock;
			}
		}
	}

#endif


Module::Module(ModuleType type)
{
	moduleType = type;
	moduleLoaded = false;
}

Module::~Module()
{
	#if !C4LEAK_DETECTION

		Unload();

	#endif
}

EngineResult Module::Load(const char *name)
{
	#if C4DESKTOP

		#if C4WINDOWS

			String<kMaxFilePathLength> path((moduleType == kModulePlugin) ? "Plugins\\" : "");
			path += name;
			path += ".dll";

			moduleHandle = LoadLibraryA(path);
			if (!moduleHandle)
			{
				return (kEngineModuleLoadFailed);
			}

		#elif C4MACOS

			FileMgr::FilePath path(&TheFileMgr->GetPluginsPath()[1]);

			if (moduleType == kModulePlugin)
			{
				path += "Plugins/";
			}

			path += name;
			path += ".dylib";

		#elif C4LINUX

			String<kMaxFilePathLength> path((moduleType == kModulePlugin) ? "./Plugins/" : "./");
			path += name;
			path += ".so";

		#endif

		#if C4POSIX

			moduleHandle = dlopen(path, RTLD_NOW | RTLD_GLOBAL);
			if (!moduleHandle)
			{
				const char *error = dlerror();
				if (error)
				{
					Engine::Report(error, kReportLog | kReportError);
					Engine::Report("<br/>\r\n", kReportLog);
				}

				return (kEngineModuleLoadFailed);
			}

		#endif

	#endif

	moduleLoaded = true;
	return (kEngineOkay);
}

void Module::Unload(void)
{
	if (moduleLoaded)
	{
		moduleLoaded = false;

		#if C4DESKTOP

			#if C4WINDOWS

				FreeLibrary(moduleHandle);

			#elif C4POSIX

				dlclose(moduleHandle);

			#endif

		#endif
	}
}


ApplicationModule::ApplicationModule() : Module(kModuleApplication)
{
}

ApplicationModule::~ApplicationModule()
{
}

EngineResult ApplicationModule::Load(const char *name)
{
	EngineResult result = Module::Load(name);
	if (result != kEngineOkay)
	{
		return (result);
	}

	#if C4DESKTOP

		CreateProc *creator = (CreateProc *) GetFunctionAddress("CreateApplication");

	#else

		CreateProc *creator = &CreateApplication;

	#endif

	if (!creator)
	{
		return (kEngineModuleCreatorMissing);
	}

	if (!(*creator)())
	{
		return (kEngineModuleInitFailed);
	}

	return (kEngineOkay);
}

// ZYUQURM
