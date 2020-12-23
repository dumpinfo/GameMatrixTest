 

#include "C4Display.h"
#include "C4Graphics.h"
#include "C4Engine.h"

#if C4MACOS

	#include "C4Interface.h"

#endif


using namespace C4;


DisplayMgr *C4::TheDisplayMgr = nullptr;


namespace C4
{
	template <> DisplayMgr Manager<DisplayMgr>::managerObject(0);
	template <> DisplayMgr **Manager<DisplayMgr>::managerPointer = &TheDisplayMgr;

	template <> const char *const Manager<DisplayMgr>::resultString[] =
	{
		nullptr,
		"DisplayMgr initialization failed"
	};

	template <> const unsigned_int32 Manager<DisplayMgr>::resultIdentifier[] =
	{
		0, 'INIT'
	};

	template class Manager<DisplayMgr>;
}


#if C4IOS //[ MOBILE

	// -- Mobile code hidden --

#endif //]


DisplayEventHandler::DisplayEventHandler(HandlerProc *proc, void *cookie)
{
	handlerProc = proc;
	handlerCookie = cookie;
}


#if C4WINDOWS

	DisplayMode::DisplayMode(int32 width, int32 height, int32 frequency)
	{
		displayWidth = width;
		displayHeight = height;
		displayFrequency = frequency;
	}

#elif C4MACOS

	DisplayMode::DisplayMode(CGDisplayModeRef mode, int32 width, int32 height, int32 frequency)
	{
		displayWidth = width;
		displayHeight = height;
		displayFrequency = frequency;
		displayMode = CGDisplayModeRetain(mode);
	}

#elif C4LINUX

	DisplayMode::DisplayMode(int32 index, int32 width, int32 height, int32 frequency)
	{
		displayWidth = width;
		displayHeight = height;
		displayFrequency = frequency;
		sizeIndex = index;
	}

#elif C4IOS //[ MOBILE

	// -- Mobile code hidden --

#elif C4PS4 //[ PS4

	// -- PS4 code hidden --

#elif C4PS3 //[ PS3

	// -- PS3 code hidden --

#endif //]

DisplayMode::~DisplayMode()
{
	#if C4MACOS

		CGDisplayModeRelease(displayMode);

	#endif 
}

 
#if C4WINDOWS
 
	C4::Display::Display(const char *name)
	{
		displayName = name; 

		for (DWORD modeNum = 0;; modeNum++) 
		{ 
			DEVMODE		devMode;

			devMode.dmSize = sizeof(DEVMODE);
			devMode.dmDriverExtra = 0; 

			if (!EnumDisplaySettingsA(name, modeNum, &devMode))
			{
				break;
			}

			int32 width = devMode.dmPelsWidth;
			int32 height = devMode.dmPelsHeight;
			if ((width >= 1024) && (height >= 768) && (width >= height) && (devMode.dmBitsPerPel >= 32))
			{
				int32 frequency = 0;
				if (devMode.dmFields & DM_DISPLAYFREQUENCY)
				{
					frequency = devMode.dmDisplayFrequency;
					if ((frequency < 60) || (frequency > 75))
					{
						frequency = 0;
					}
				}

				DisplayMode *displayMode = displayModeList.First();
				while (displayMode)
				{
					if ((displayMode->GetDisplayWidth() == width) && (displayMode->GetDisplayHeight() == height))
					{
						if (frequency <= displayMode->GetDisplayFrequency())
						{
							goto next;
						}

						delete displayMode;
						break;
					}

					displayMode = displayMode->Next();
				}

				displayMode = new DisplayMode(width, height, frequency);
				displayModeList.Append(displayMode);
			}

			next:;
		}
	}

#elif C4MACOS

	C4::Display::Display(CGDirectDisplayID id)
	{
		displayID = id;

		CFArrayRef array = CGDisplayCopyAllDisplayModes(id, nullptr);
		CFIndex count = CFArrayGetCount(array);

		for (machine a = 0; a < count; a++)
		{
			const void *value = CFArrayGetValueAtIndex(array, a);
			if (CFGetTypeID(value) == CGDisplayModeGetTypeID())
			{
				CGDisplayModeRef mode = (CGDisplayModeRef) value;

				int32 width = CGDisplayModeGetWidth(mode);
				int32 height = CGDisplayModeGetHeight(mode);
				CFStringRef encoding = CGDisplayModeCopyPixelEncoding(mode);

				if ((width >= 1024) && (height >= 768) && (width >= height) && (CFStringCompare(encoding, CFSTR(IO32BitDirectPixels), 0) == kCFCompareEqualTo))
				{
					int32 frequency = CGDisplayModeGetRefreshRate(mode);

					DisplayMode *displayMode = displayModeList.First();
					while (displayMode)
					{
						if ((displayMode->GetDisplayWidth() == width) && (displayMode->GetDisplayHeight() == height))
						{
							if (frequency <= displayMode->GetDisplayFrequency())
							{
								goto next;
							}

							delete displayMode;
							break;
						}

						displayMode = displayMode->Next();
					}

					displayMode = new DisplayMode(mode, width, height, frequency);
					displayModeList.Append(displayMode);
				}

				next:
				CFRelease(encoding);
			}
		}

		CFRelease(array);
	}

#elif C4LINUX

	C4::Display::Display(::Display *display, int32 index)
	{
		int		count;

		screenIndex = index;

		const XRRScreenSize *sizeArray = XRRSizes(display, index, &count);
		for (machine a = 0; a < count; a++)
		{
			int32 width = sizeArray[a].width;
			int32 height = sizeArray[a].height;
			if ((width >= 1024) && (height >= 768) && (width >= height))
			{
				displayModeList.Append(new DisplayMode(a, width, height));
			}
		}
	}

#elif C4IOS //[ MOBILE

	// -- Mobile code hidden --

#elif C4PS4 //[ PS4

	// -- PS4 code hidden --

#elif C4PS3 //[ PS3

	// -- PS3 code hidden --

#endif //]

C4::Display::~Display()
{
}

const DisplayMode *C4::Display::FindDisplayMode(int32 width, int32 height) const
{
	const DisplayMode *mode = displayModeList.First();
	while (mode)
	{
		if ((mode->GetDisplayWidth() == width) && (mode->GetDisplayHeight() == height))
		{
			break;
		}

		mode = mode->Next();
	}

	return (mode);
}


DisplayMgr::DisplayMgr(int)
{
}

DisplayMgr::~DisplayMgr()
{
}

EngineResult DisplayMgr::Construct(void)
{
	#if C4OCULUS

		 Oculus::Initialize();

	#endif

	#if C4WINDOWS

		for (DWORD devNum = 0;; devNum++)
		{
			DISPLAY_DEVICEA		device;

			device.cb = sizeof(DISPLAY_DEVICEA);
			if (!EnumDisplayDevicesA(nullptr, devNum, &device, 0))
			{
				break;
			}

			Display *display = new Display(device.DeviceName);
			if (device.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
			{
				displayList.Prepend(display);
			}
			else
			{
				displayList.Append(display);
			}
		}

	#elif C4MACOS

		CGDisplayCount	count;

		CGGetActiveDisplayList(0, nullptr, &count);
		CGDirectDisplayID *table = new CGDirectDisplayID[count];

		CGGetActiveDisplayList(count, table, &count);
		for (machine a = 0; a < count; a++)
		{
			Display *display = new Display(table[a]);
			displayList.Append(display);
		}

		delete[] table;

	#elif C4LINUX

		char bitmap[8] = {0};
		XColor color = {0};

		::Display *engineDisplay = TheEngine->GetEngineDisplay();
		int32 count = ScreenCount(engineDisplay);
		int32 primary = DefaultScreen(engineDisplay);
		::Window window = DefaultRootWindow(engineDisplay);

		screenConfig = XRRGetScreenInfo(engineDisplay, window);
		originalSize = XRRConfigCurrentConfiguration(screenConfig, &originalRotation);
		originalRate = XRRConfigCurrentRate(screenConfig);

		for (machine a = 0; a < count; a++)
		{
			Display *display = new Display(engineDisplay, a);
			if (a == primary)
			{
				displayList.Prepend(display);
			}
			else
			{
				displayList.Append(display);
			}
		}

		Pixmap sourcePixmap = XCreateBitmapFromData(engineDisplay, window, bitmap, 8, 8);
		Pixmap maskPixmap = XCreateBitmapFromData(engineDisplay, window, bitmap, 8, 8);

		emptyCursor = XCreatePixmapCursor(engineDisplay, sourcePixmap, maskPixmap, &color, &color, 0, 0);

		XFreePixmap(engineDisplay, maskPixmap);
		XFreePixmap(engineDisplay, sourcePixmap);

	#elif C4IOS //[ MOBILE

		// -- Mobile code hidden --

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]

	currentDisplay = displayList.First();
	if (!currentDisplay)
	{
		return (kDisplayInitFailed);
	}

	#if C4WINDOWS

		displayWindow = nullptr;

	#endif

	displayFlags = 0;
	displayChanged = false;
	cursorVisible = true;

	#if !C4MOBILE

		Variable *samplesVar = TheEngine->InitVariable("displaySamples", "4", kVariablePermanent);

	#else //[ MOBILE

		// -- Mobile code hidden --

	#endif //]

	Variable *syncVar = TheEngine->InitVariable("displaySync", "0", kVariablePermanent);
	Variable *tearVar = TheEngine->InitVariable("displayTear", "0", kVariablePermanent);

	#if C4OCULUS

		Variable *oculusVar = TheEngine->InitVariable("displayOculus", "0", kVariablePermanent);

	#endif

	int32 samples = samplesVar->GetIntegerValue();

	#if C4DESKTOP

		Variable *widthVar = TheEngine->InitVariable("displayWidth", "1024", kVariablePermanent);
		Variable *heightVar = TheEngine->InitVariable("displayHeight", "768", kVariablePermanent);
		Variable *fullVar = TheEngine->InitVariable("displayFull", "0", kVariablePermanent);

		int32 width = widthVar->GetIntegerValue();
		int32 height = heightVar->GetIntegerValue();

		unsigned_int32 flags = (fullVar->GetIntegerValue() != 0) ? kDisplayFullscreen : 0;

	#elif C4IOS //[ MOBILE

		// -- Mobile code hidden --

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]

	if (syncVar->GetIntegerValue() != 0)
	{
		flags |= kDisplayRefreshSync;
		if (tearVar->GetIntegerValue() != 0)
		{
			flags |= kDisplaySyncTear;
		}
	}

	#if C4OCULUS

		if (oculusVar->GetIntegerValue() != 0)
		{
			width = Oculus::GetFullFrameWidth();
			height = Oculus::GetFullFrameHeight();
			flags |= kDisplayOculus;
		}

	#endif

	for (;;)
	{
		EngineResult result = SetDisplayMode(width, height, samples, flags);
		if (result == kEngineOkay)
		{
			break;
		}

		#if C4DESKTOP

			if ((width != 1024) || (height != 768) || (flags & kDisplayFullscreen))
			{
				width = 1024;
				height = 768;
				flags &= ~kDisplayFullscreen;
				continue;
			}

		#endif

		return (result);
	}

	return (kEngineOkay);
}

void DisplayMgr::Destruct(void)
{
	GraphicsMgr::Delete();

	ShowCursor();

	#if C4WINDOWS

		if (displayWindow)
		{
			DestroyWindow(displayWindow);
		}

		if (displayChanged)
		{
			ChangeDisplaySettingsA(nullptr, 0);
		}

	#elif C4LINUX

		if (displayChanged)
		{
			::Display *engineDisplay = TheEngine->GetEngineDisplay();
			::Window window = DefaultRootWindow(engineDisplay);
			XRRSetScreenConfigAndRate(engineDisplay, screenConfig, window, originalSize, originalRotation, originalRate, CurrentTime);
		}

		XRRFreeScreenConfigInfo(screenConfig);

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#endif //]

	displayList.Purge();

	#if C4OCULUS

		 Oculus::Terminate();

	#endif
}

EngineResult DisplayMgr::SetDisplayMode(int32 width, int32 height, int32 samples, unsigned_int32 flags)
{
	const DisplayMode *mode = currentDisplay->FindDisplayMode(width, height);
	if ((flags & kDisplayFullscreen) && (!mode))
	{
		return (kDisplayInitFailed);
	}

	GraphicsMgr::Delete();
	ShowCursor();

	#if C4WINDOWS

		if (displayWindow)
		{
			DestroyWindow(displayWindow);
			displayWindow = nullptr;
		}

		HWND mainWindow = TheEngine->GetEngineWindow();

		if (flags & kDisplayFullscreen)
		{
			DEVMODE		devMode;

			MemoryMgr::ClearMemory(&devMode, sizeof(DEVMODE));
			devMode.dmSize = sizeof(DEVMODE);
			devMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
			devMode.dmBitsPerPel = 32;
			devMode.dmPelsWidth = width;
			devMode.dmPelsHeight = height;

			if (ChangeDisplaySettingsA(&devMode, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
			{
				return (kDisplayInitFailed);
			}

			displayChanged = true;

			SetWindowPos(mainWindow, HWND_NOTOPMOST, 0, 0, width, height, SWP_NOCOPYBITS);
			SetWindowLongPtrA(mainWindow, GWL_STYLE, WS_POPUP | WS_CLIPCHILDREN);
			SetWindowPos(mainWindow, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
		}
		else
		{
			RECT	windowRect;

			if (displayChanged)
			{
				displayChanged = false;
				ChangeDisplaySettingsA(nullptr, 0);
			}

			windowRect.left = GetSystemMetrics(SM_CXFULLSCREEN) / 2 - width / 2;
			windowRect.right = windowRect.left + width;
			windowRect.top = GetSystemMetrics(SM_CYFULLSCREEN) / 2 - height / 2;
			windowRect.bottom = windowRect.top + height;
			AdjustWindowRectEx(&windowRect, WS_BORDER | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, false, 0);

			SetWindowPos(mainWindow, HWND_NOTOPMOST, windowRect.left, windowRect.top, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, SWP_NOCOPYBITS);
			SetWindowLongPtrA(mainWindow, GWL_STYLE, WS_CLIPCHILDREN | WS_BORDER | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU);
			SetWindowPos(mainWindow, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
		}

		displayWindow = CreateWindowExW(0, L"C4", L"C4", WS_CHILD | WS_VISIBLE, 0, 0, width, height, mainWindow, nullptr, TheEngine->GetEngineInstance(), nullptr);

		fullFrameWidth = width;
		fullFrameHeight = height;
		displayWidth = width;
		displayHeight = height;
		displaySamples = samples;
		displayFlags = flags;

		#if C4OCULUS

			if (flags & kDisplayOculus)
			{
				displayWidth = Oculus::GetDisplayWidth();
				displayHeight = Oculus::GetDisplayHeight();
			}

		#endif

		EngineResult result = GraphicsMgr::New();
		if (result != kEngineOkay)
		{
			DestroyWindow(displayWindow);
			displayWindow = nullptr;
			return (result);
		}

	#elif C4MACOS

		NSRect		windowRect;

		windowRect.origin.x = 0.0F;
		windowRect.origin.y = 0.0F;
		windowRect.size.width = (CGFloat) width;
		windowRect.size.height = (CGFloat) height;

		NSWindow *window = TheEngine->GetEngineWindow();
		[window setContentSize: windowRect.size];

		if (flags & kDisplayFullscreen)
		{
			[window setOpaque: YES];
			[window setCollectionBehavior: NSWindowCollectionBehaviorFullScreenPrimary];
			[window setFrame: [[window screen] frame] display: NO animate: NO];

			if (!([window styleMask] & NSFullScreenWindowMask))
			{
				[window toggleFullScreen: nil];
			}
		}
		else
		{
			if ([window styleMask] & NSFullScreenWindowMask)
			{
				[window toggleFullScreen: nil];
			}

			[window setOpaque: NO];
			[window setCollectionBehavior: NSWindowCollectionBehaviorDefault];

			[window setFrame: [window frameRectForContentRect: windowRect] display: NO animate: NO];
			[window center];
		}

		NSOpenGLView *view = [[NSOpenGLView alloc] initWithFrame: windowRect];
		displayView = view;

		[view addTrackingRect: windowRect owner: window userData: nil assumeInside: NO];
		[window setContentView: view];
		[view release];

		[window makeMainWindow];
		[window makeKeyAndOrderFront: nil];

		fullFrameWidth = width;
		fullFrameHeight = height;
		displayWidth = width;
		displayHeight = height;
		displaySamples = samples;
		displayFlags = flags;

		EngineResult result = GraphicsMgr::New();
		if (result != kEngineOkay)
		{
			return (result);
		}

	#elif C4LINUX

		XSetWindowAttributes	windowAttributes;

		::Display *engineDisplay = TheEngine->GetEngineDisplay();
		::Window engineWindow = TheEngine->GetEngineWindow();
		::Window rootWindow = DefaultRootWindow(engineDisplay);

		XUnmapWindow(engineDisplay, engineWindow);

		if (flags & kDisplayFullscreen)
		{
			if (XRRSetScreenConfig(engineDisplay, screenConfig, rootWindow, mode->GetSizeIndex(), RR_Rotate_0, CurrentTime) != 0)
			{
				return (kDisplayInitFailed);
			}

			displayChanged = true;

			XMoveResizeWindow(engineDisplay, engineWindow, 0, 0, width, height);

			windowAttributes.override_redirect = true;
			XChangeWindowAttributes(engineDisplay, engineWindow, CWOverrideRedirect, &windowAttributes);
		}
		else
		{
			if (displayChanged)
			{
				displayChanged = false;

				XRRSetScreenConfigAndRate(engineDisplay, screenConfig, rootWindow, originalSize, originalRotation, originalRate, CurrentTime);

				windowAttributes.override_redirect = false;
				XChangeWindowAttributes(engineDisplay, engineWindow, CWOverrideRedirect, &windowAttributes);
			}

			Screen *screen = ScreenOfDisplay(engineDisplay, currentDisplay->GetScreenIndex());
			XMoveResizeWindow(engineDisplay, engineWindow, WidthOfScreen(screen) / 2 - width / 2, HeightOfScreen(screen) / 2 - height / 2, width, height);
		}

		fullFrameWidth = width;
		fullFrameHeight = height;
		displayWidth = width;
		displayHeight = height;
		displaySamples = samples;
		displayFlags = flags;

		EngineResult result = GraphicsMgr::New();
		if (result != kEngineOkay)
		{
			return (result);
		}

		XSizeHints *sizeHints = XAllocSizeHints();
		sizeHints->flags = PMinSize | PMaxSize;
		sizeHints->min_width = width;
		sizeHints->min_height = height;
		sizeHints->max_width = width;
		sizeHints->max_height = height;
		XSetWMNormalHints(engineDisplay, engineWindow, sizeHints);
		XFree(sizeHints);

		XMapRaised(engineDisplay, engineWindow);

		if (flags & kDisplayFullscreen)
		{
			XSetInputFocus(engineDisplay, engineWindow, RevertToPointerRoot, CurrentTime);
		}

	#elif C4IOS //[ MOBILE

		// -- Mobile code hidden --

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]

	HideCursor();

	DisplayEventData eventData(kEventDisplayChange);

	const DisplayEventHandler *handler = displayEventHandlerList.First();
	while (handler)
	{
		const DisplayEventHandler *next = handler->Next();
		handler->HandleEvent(&eventData);
		handler = next;
	}

	return (kEngineOkay);
}

#if C4MACOS

	void DisplayMgr::AdjustEngineWindow(void)
	{
		NSWindow *window = TheEngine->GetEngineWindow();

		if (displayFlags & kDisplayFullscreen)
		{
			[window setFrame: [[window screen] frame] display: NO animate: NO];
		}
		else
		{
			NSRect rect = NSMakeRect(0.0F, 0.0F, (CGFloat) fullFrameWidth, (CGFloat) fullFrameHeight);
			[window setFrame: [window frameRectForContentRect: rect] display: NO animate: NO];
			[window center];
		}
	}

#endif

void DisplayMgr::ShowCursor(void)
{
	if (!cursorVisible)
	{
		cursorVisible = true;

		#if C4WINDOWS

			::ShowCursor(true);

		#elif C4MACOS

			[NSCursor unhide];

		#elif C4LINUX

			XUndefineCursor(TheEngine->GetEngineDisplay(), TheEngine->GetEngineWindow());

		#endif
	}
}

void DisplayMgr::HideCursor(void)
{
	if (cursorVisible)
	{
		cursorVisible = false;

		#if C4WINDOWS

			::ShowCursor(false);

		#elif C4MACOS

			[NSCursor hide];

		#elif C4LINUX

			XDefineCursor(TheEngine->GetEngineDisplay(), TheEngine->GetEngineWindow(), emptyCursor);

		#endif
	}
}

// ZYUQURM
