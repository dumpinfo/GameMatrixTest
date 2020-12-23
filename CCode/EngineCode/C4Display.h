 

#ifndef C4Display_h
#define C4Display_h


//# \component	Graphics Manager
//# \prefix		GraphicsMgr/


#include "C4Types.h"
#include "C4Oculus.h"


namespace C4
{
	typedef EngineResult	DisplayResult;


	enum : DisplayResult
	{
		kDisplayInitFailed		= (kManagerDisplay << 16) | 0x0001
	};


	//# \enum	DisplayFlags

	enum
	{
		kDisplayFullscreen		= 1 << 0,		//## The engine is shown full-screen, and no operating system controls are visible.
		kDisplayRefreshSync		= 1 << 1,		//## Refresh sync is turned on, meaning that framebuffer display is delayed until the monitor finishes its current refresh cycle to prevent tearing.
		kDisplaySyncTear		= 1 << 2,		//## Tearing is enabled if the frame rate is not high enough to sync to the refresh rate. (Ignored if $kDisplayRefreshSync$ is not set.)
		kDisplayOculus			= 1 << 3
	};


	enum : EventType
	{
		kEventDisplayChange		= 'DSCH'
	};


	class Variable;


	//# \struct	DisplayEventData		Contains information about a display event.
	//
	//# The $DisplayEventData$ structure contains information about a display event.
	//
	//# \def	struct DisplayEventData
	//
	//# \data	DisplayEventData


	//# \member		DisplayEventData

	struct DisplayEventData
	{
		EventType		eventType;			//## The type of the event.

		DisplayEventData(EventType type)
		{
			eventType = type;
		}
	};


	//# \class	DisplayEventHandler		Encapsulates a display event handler function.
	//
	//# The $DisplayEventHandler$ class encapsulates a display event handler function.
	//
	//# \def	class DisplayEventHandler : public ListElement<DisplayEventHandler>
	//
	//# \ctor	DisplayEventHandler(HandlerProc *proc, void *cookie = nullptr);
	//
	//# \param	proc	The procedure to invoke when a display event occurs.
	//# \param	cookie	The cookie that is passed to the event handler as its last parameter.
	//
	//# \desc
	//# The $DisplayEventHandler$ class encapsulates a procedure that is invoked when a
	//# display event occurs. Once an instance of the $DisplayEventHandler$ class has
	//# been constructed, it can be installed by calling the $@DisplayMgr::InstallDisplayEventHandler@$ function.
	//#
	//# When a display event occurs, the procedures corresponding to all installed display event handlers are
	//# invoked. The $HandlerProc$ type is defined as follows.
	//
	//# \code	typedef void HandlerProc(const DisplayEventData *eventData, void *cookie);
	//
	//# The $eventType$ field of the $@DisplayEventData@$ structure specifies what type of display event occurred
	//# and can be one of the following constants.
	//
	//# \value	kEventDisplayChange		The dimensions of the display have changed.
	//
	//# \desc
	//# The $cookie$ parameter is the value passed to the $DisplayEventHandler$ constructor.
	//#
	//# A display event handler is uninstalled by destroying its associated class instance.
	//
	//# \base	Utilities/ListElement<DisplayEventHandler>		Used internally to store all instances of $DisplayEventHandler$ in a list.


	class DisplayEventHandler : public ListElement<DisplayEventHandler>
	{ 
		public:

			typedef void HandlerProc(const DisplayEventData *, void *); 

		private: 

			HandlerProc		*handlerProc;
			void			*handlerCookie; 

		public: 
 
			C4API DisplayEventHandler(HandlerProc *proc, void *cookie = nullptr);

			void HandleEvent(const DisplayEventData *eventData) const
			{ 
				(*handlerProc)(eventData, handlerCookie);
			}
	};


	class DisplayMode : public ListElement<DisplayMode>
	{
		private:

			int32		displayWidth;
			int32		displayHeight;
			int32		displayFrequency;

			#if C4MACOS

				CGDisplayModeRef	displayMode;

			#elif C4LINUX

				int32				sizeIndex;

			#elif C4PS3 //[ PS3

				// -- PS3 code hidden --

			#endif //]

		public:

			#if C4WINDOWS

				DisplayMode(int32 width, int32 height, int32 frequency = 0);

			#elif C4MACOS

				DisplayMode(CGDisplayModeRef mode, int32 width, int32 height, int32 frequency = 0);

			#elif C4LINUX

				DisplayMode(int32 index, int32 width, int32 height, int32 frequency = 0);

			#elif C4IOS //[ MOBILE

				// -- Mobile code hidden --

			#elif C4PS4 //[ PS4

				// -- PS4 code hidden --

			#elif C4PS3 //[ PS3

				// -- PS3 code hidden --

			#endif //]

			~DisplayMode();

			int32 GetDisplayWidth(void) const
			{
				return (displayWidth);
			}

			int32 GetDisplayHeight(void) const
			{
				return (displayHeight);
			}

			int32 GetDisplayFrequency(void) const
			{
				return (displayFrequency);
			}

			#if C4MACOS

				CGDisplayModeRef GetDisplayMode(void) const
				{
					return (displayMode);
				}

			#elif C4LINUX

				int32 GetSizeIndex(void) const
				{
					return (sizeIndex);
				}

			#elif C4PS3 //[ PS3

				// -- PS3 code hidden --

			#endif //]
	};


	class Display : public ListElement<Display>
	{
		private:

			#if C4WINDOWS

				String<31>			displayName;

			#elif C4MACOS

				CGDirectDisplayID	displayID;

			#elif C4LINUX

				int32				screenIndex;

			#elif C4PS3 //[ PS3

				// -- PS3 code hidden --

			#endif //]

			List<DisplayMode>		displayModeList;

		public:

			#if C4WINDOWS

				Display(const char *name);

				const char *GetDisplayName(void) const
				{
					return (displayName);
				}

			#elif C4MACOS

				Display(CGDirectDisplayID id);

				CGDirectDisplayID GetDisplayID(void) const
				{
					return (displayID);
				}

			#elif C4LINUX

				Display(::Display *display, int32 index);

				int32 GetScreenIndex(void) const
				{
					return (screenIndex);
				}

			#elif C4IOS //[ MOBILE

				// -- Mobile code hidden --

			#elif C4PS4 //[ PS4

				// -- PS4 code hidden --

			#elif C4PS3 //[ PS3

				// -- PS3 code hidden --

			#endif //]

			~Display();

			const DisplayMode *GetFirstDisplayMode(void) const
			{
				return (displayModeList.First());
			}

			const DisplayMode *GetDisplayMode(int32 index) const
			{
				return (displayModeList[index]);
			}

			const DisplayMode *FindDisplayMode(int32 width, int32 height) const;
	};


	//# \class	DisplayMgr		The Display Manager class.
	//
	//# \def	class DisplayMgr : public Manager<DisplayMgr>
	//
	//# \desc
	//# The $DisplayMgr$ class encapsulates the display management functionality of the C4 Engine.
	//# The single instance of the Display Manager is constructed during an application's initialization
	//# and destroyed at termination.
	//#
	//# The Display Manager's member functions are accessed through the global pointer $TheDisplayMgr$.


	//# \function	DisplayMgr::GetDisplayWidth		Returns the current width of the display.
	//
	//# \proto	int32 GetDisplayWidth(void) const;
	//
	//# \desc
	//# The $GetDisplayWidth$ function returns the current width of the display, in pixels.
	//
	//# \also	$@DisplayMgr::GetDisplayHeight@$
	//# \also	$@InterfaceMgr/InterfaceMgr::GetDesktopSize@$


	//# \function	DisplayMgr::GetDisplayHeight	Returns the current height of the display.
	//
	//# \proto	int32 GetDisplayHeight(void) const;
	//
	//# \desc
	//# The $GetDisplayHeight$ function returns the current height of the display, in pixels.
	//
	//# \also	$@DisplayMgr::GetDisplayWidth@$
	//# \also	$@InterfaceMgr/InterfaceMgr::GetDesktopSize@$


	//# \function	DisplayMgr::SetDisplayMode		Sets the current display mode.
	//
	//# \proto	EngineResult SetDisplayMode(int32 width, int32 height, int32 samples, unsigned_int32 flags);
	//
	//# \param	width		The horizontal resolution of the display, in pixels.
	//# \param	height		The vertical resolution of the display, in pixels.
	//# \param	samples		The number of antialiasing samples. A value of 1 means no antialiasing.
	//# \param	flags		The display flags. See below for possible values.
	//
	//# \desc
	//# The $SetDisplayMode$ function sets the current display mode, establishing the display resolution, the number of
	//# samples used for antialiasing, and state such as whether the engine window is shown full-screen.
	//#
	//# The $width$ and $height$ parameters define the screen resolution. If the $kDisplayFullscreen$ flag is specified
	//# in the $flags$ parameter, but the display does not natively support the resolution specified, then this function
	//# returns $kDisplayInitFailed$ and no changes are made. In windowed mode, any resolution is accepted, but the width
	//# and height should always be at least 640 &times; 480.
	//#
	//# The $samples$ parameter specifies the number of samples per pixel used for hardware antialiasing. If the value is 1,
	//# then antialiasing is turned off. Otherwise, the hardware is configured for multisample antialiasing using the greatest
	//# number of samples no larger than the value of the $samples$ parameter that is supported by the hardware.
	//#
	//# The $flags$ parameter controls various options for the display mode. It can be a combination (through logical OR)
	//# of the following constants.
	//
	//# \table	DisplayFlags
	//
	//# \also	$@DisplayMgr::GetDisplayWidth@$
	//# \also	$@InterfaceMgr/InterfaceMgr::GetDesktopSize@$


	//# \function	DisplayMgr::InstallDisplayEventHandler		Installs a display event handler.
	//
	//# \proto	void InstallDisplayEventHandler(DisplayEventHandler *handler);
	//
	//# \param	handler		The event handler to install.
	//
	//# \desc
	//# The $InstallDisplayEventHandler$ function installs a display event handler that is invoked whenever
	//# a display event occurs. Multiple display event handlers may be installed simultaneously, and each
	//# one is invoked when an event occurs.
	//#
	//# A display event handler is uninstalled by destroying its associated class instance.
	//
	//# \also	$@DisplayEventHandler@$


	class DisplayMgr : public Manager<DisplayMgr>
	{
		private:

			List<Display>					displayList;
			Display							*currentDisplay;

			int32							fullFrameWidth;
			int32							fullFrameHeight;
			int32							displayWidth;
			int32							displayHeight;
			int32							displaySamples;
			unsigned_int32					displayFlags;

			List<DisplayEventHandler>		displayEventHandlerList;

			bool							displayChanged;
			bool							cursorVisible;

			#if C4WINDOWS

				HWND						displayWindow;

			#elif C4MACOS

				NSOpenGLView				*displayView;

			#elif C4LINUX

				XRRScreenConfiguration		*screenConfig;
				SizeID						originalSize;
				Rotation					originalRotation;
				short						originalRate;

				::Cursor					emptyCursor;

			#elif C4IOS //[ MOBILE

				// -- Mobile code hidden --

			#elif C4PS4 //[ PS4

				// -- PS4 code hidden --

			#endif //]

		public:

			DisplayMgr(int);
			~DisplayMgr();

			EngineResult Construct(void);
			void Destruct(void);

			Display *GetFirstDisplay(void) const
			{
				return (displayList.First());
			}

			Display *GetCurrentDisplay(void) const
			{
				return (currentDisplay);
			}

			int32 GetFullFrameWidth(void) const
			{
				return (fullFrameWidth);
			}

			int32 GetFullFrameHeight(void) const
			{
				return (fullFrameHeight);
			}

			int32 GetDisplayWidth(void) const
			{
				return (displayWidth);
			}

			int32 GetDisplayHeight(void) const
			{
				return (displayHeight);
			}

			int32 GetDisplaySamples(void) const
			{
				return (displaySamples);
			}

			unsigned_int32 GetDisplayFlags(void) const
			{
				return (displayFlags);
			}

			void InstallDisplayEventHandler(DisplayEventHandler *handler)
			{
				displayEventHandlerList.Append(handler);
			}

			#if C4WINDOWS

				HWND GetDisplayWindow(void) const
				{
					return (displayWindow);
				}

			#elif C4MACOS

				NSOpenGLView *GetDisplayView(void) const
				{
					return (displayView);
				}

				void AdjustEngineWindow(void);

			#elif C4IOS //[ MOBILE

				// -- Mobile code hidden --

			#elif C4PS4 //[ PS4

				// -- PS4 code hidden --

			#endif //]

			C4API EngineResult SetDisplayMode(int32 width, int32 height, int32 samples, unsigned_int32 flags);

			void ShowCursor(void);
			void HideCursor(void);
	};


	C4API extern DisplayMgr *TheDisplayMgr;
}


#endif

// ZYUQURM
