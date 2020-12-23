 

#ifndef C4Engine_h
#define C4Engine_h


//# \component	System Utilities
//# \prefix		System/

//# \import		C4Types.h


#include "C4Time.h"
#include "C4Resources.h"
#include "C4Variables.h"
#include "C4Commands.h"
#include "C4OpenDDL.h"

#if C4WINDOWS

	#include "C4Wintab.h"

#endif


namespace C4
{
	enum : EngineResult
	{
		kEngineModuleLoadFailed			= (kManagerEngine << 16) | 0x0001,
		kEngineModuleInitFailed			= (kManagerEngine << 16) | 0x0002,
		kEngineModuleCreatorMissing		= (kManagerEngine << 16) | 0x0003
	};


	//# \enum	ReportFlags

	enum
	{
		kReportLog				= 1 << 0,		//## The text is written to the standard log file.
		kReportError			= 1 << 1,		//## The text should be considered to be an error message.
		kReportSuccess			= 1 << 2,		//## The text should be considered to be a success message.
		kReportHeading			= 1 << 3,		//## The text should be formatted as a heading.
		kReportFormatted		= 1 << 4		//## The text is preformatted and should be output with no changes to its spacing.
	};


	//# \enum	EngineFlags

	enum
	{
		kEngineForeground		= 1 << 0,		//## The engine is currently the foreground process.
		kEngineVisible			= 1 << 1,		//## The engine is currently visible (not minimized or otherwise hidden).
		kEngineQuit				= 1 << 2		//## The engine is in the process of quitting.
	};


	enum
	{
		kProcessorSSE			= 1 << 0,
		kProcessorSSE2			= 1 << 1,
		kProcessorSSE3			= 1 << 2,
		kProcessorNeon			= 1 << 3,
		kProcessorAltivec		= 1 << 4
	};


	enum ModuleType
	{
		kModuleApplication,
		kModulePlugin
	};


	class Application;
	class ApplicationModule;
	class DomainNameResolver;
	class DeferredTask;


	//# \class	Reporter	Handles messages passed to the system report chain.
	//
	//# The $Reporter$ class handles messages passed to the system report chain.
	//
	//# \def	class Reporter : public ListElement<Reporter>
	//
	//# \ctor	Reporter(ReportProc *proc, void *cookie = nullptr);
	//
	//# \param	proc	The procedure to invoke when a message is reported.
	//# \param	cookie	The cookie that is passed to the report procedure as its last parameter.
	//
	//# \desc
	//# The $Reporter$ class encapsulates a procedure that is invoked when the
	//# $@Engine::Report@$ function is called. Once an instance of the $Reporter$ class has
	//# been constructed, it can be installed by calling the $@Engine::InstallReporter@$ function.
	//#
	//# When a message is reported, the procedures corresponding to all installed reporters are
	//# invoked. The $ReportProc$ type is defined as follows.
	//
	//# \code	typedef void ReportProc(const char *text, unsigned_int32 flags, void *cookie);
	//
	//# The $text$ and $flags$ parameters passed to the procedure pointed to by the
	//# $proc$ parameter match those passed to the $@Engine::Report@$ function. The $cookie$ 
	//# parameter passed to the procedure pointed to by the $proc$ parameter matches the $cookie$
	//# parameter passed to the constructor of the $Reporter$ class.
	//# 
	//# A reporter is uninstalled by destroying its associated class instance.
	// 
	//# \base	Utilities/ListElement<Reporter>		Used internally to store all instances of $Reporter$ in a list.

 
	class Reporter : public ListElement<Reporter>
	{ 
		public: 

			typedef void ReportProc(const char *, unsigned_int32, void *);

		private: 

			ReportProc		*reportProc;
			void			*reportCookie;

		public:

			C4API Reporter(ReportProc *proc, void *cookie = nullptr);
			C4API ~Reporter();

			void Report(const char *text, unsigned_int32 flags) const
			{
				(*reportProc)(text, flags, reportCookie);
			}
	};


	//# \class	MouseEventHandler	Encapsulates a mouse event handler function.
	//
	//# The $MouseEventHandler$ class encapsulates a mouse event handler function.
	//
	//# \def	class MouseEventHandler : public ListElement<MouseEventHandler>
	//
	//# \ctor	MouseEventHandler(HandlerProc *proc, void *cookie = nullptr);
	//
	//# \param	proc	The procedure to invoke when a mouse event occurs.
	//# \param	cookie	The cookie that is passed to the event handler as its last parameter.
	//
	//# \desc
	//# The $MouseEventHandler$ class encapsulates a procedure that is invoked when a
	//# mouse event occurs. Once an instance of the $MouseEventHandler$ class has
	//# been constructed, it can be installed by calling the $@Engine::InstallMouseEventHandler@$ function.
	//#
	//# When a mouse event occurs, the procedures corresponding to all installed mouse event handlers are
	//# invoked. The $HandlerProc$ type is defined as follows.
	//
	//# \code	typedef bool HandlerProc(const MouseEventData *eventData, void *cookie);
	//
	//# The $eventType$ field of the $@Utilities/MouseEventData@$ structure specifies what type of mouse event
	//# occurred and can be one of the following constants.
	//
	//# \value	kEventMouseDown						The left mouse button was pressed.
	//# \value	kEventMouseUp						The left mouse button was released.
	//# \value	kEventRightMouseDown				The right mouse button was pressed.
	//# \value	kEventRightMouseUp					The right mouse button was released.
	//# \value	kEventMiddleMouseDown				The middle mouse button was pressed.
	//# \value	kEventMiddleMouseUp					The middle mouse button was released.
	//# \value	kEventMouseMoved					The mouse location was moved.
	//# \value	kEventMouseWheel					The mouse wheel was moved.
	//# \value	kEventMultiaxisMouseTranslation		The translation rate changed for a multiaxis mouse device.
	//# \value	kEventMultiaxisMouseRotation		The rotation rate changed for a multiaxis mouse device.
	//# \value	kEventMultiaxisMouseButtonState		The button state changed for a multiaxis mouse device.
	//
	//# \desc
	//# For mouse down, mouse up, mouse moved, and mouse wheel events, the $mousePosition$ field of the $@Utilities/MouseEventData@$
	//# structure specifies the screen coordinates at which the mouse event occurred. For the $kEventMouseWheel$ event, the $wheelDelta$
	//# member specifies how far the wheel was moved in the $x$ and $y$ directions. (Currently, the $x$ delta is always zero.)
	//#
	//# For the $kEventMultiaxisMouseTranslation$ and $kEventMultiaxisMouseRotation$ events, the $mousePosition$ field contains the
	//# 3D translation or rotation rate information. For the $kEventMultiaxisMouseButtonState$ event, the $eventFlags$ field of the
	//# $@Utilities/MouseEventData@$ structure contains the state of all 32 possible buttons, with the lowest bit representing button 1
	//# and the highest bit representing button 32.
	//#
	//# The $cookie$ parameter is the value passed to the $MouseEventHandler$ constructor.
	//#
	//# The value returned by the handler specifies whether the mouse event was successfully handled. If the handler
	//# returns $true$, then the mouse event is considered handled, and no further mouse event handlers will be called
	//# for the same event. If the handler returns $false$, then the event is passed to the next mouse event handler.
	//#
	//# A mouse event handler is uninstalled by destroying its associated class instance.
	//
	//# \base	Utilities/ListElement<MouseEventHandler>	Used internally to store all instances of $MouseEventHandler$ in a list.


	class MouseEventHandler : public ListElement<MouseEventHandler>
	{
		public:

			typedef bool HandlerProc(const MouseEventData *, void *);

		private:

			HandlerProc		*handlerProc;
			void			*handlerCookie;

		public:

			C4API MouseEventHandler(HandlerProc *proc, void *cookie = nullptr);

			bool HandleEvent(const MouseEventData *eventData) const
			{
				return ((*handlerProc)(eventData, handlerCookie));
			}
	};


	//# \class	KeyboardEventHandler	Encapsulates a keyboard event handler function.
	//
	//# The $KeyboardEventHandler$ class encapsulates a keyboard event handler function.
	//
	//# \def	class KeyboardEventHandler : public ListElement<KeyboardEventHandler>
	//
	//# \ctor	KeyboardEventHandler(HandlerProc *proc, void *cookie = nullptr);
	//
	//# \param	proc	The procedure to invoke when a keyboard event occurs.
	//# \param	cookie	The cookie that is passed to the event handler as its last parameter.
	//
	//# \desc
	//# The $KeyboardEventHandler$ class encapsulates a procedure that is invoked when a
	//# keyboard event occurs. Once an instance of the $KeyboardEventHandler$ class has
	//# been constructed, it can be installed by calling the $@Engine::InstallKeyboardEventHandler@$ function.
	//#
	//# When a keyboard event occurs, the procedures corresponding to all installed keyboard event handlers are
	//# invoked. The $HandlerProc$ type is defined as follows.
	//
	//# \code	typedef bool HandlerProc(const KeyboardEventData *eventData, void *cookie);
	//
	//# The $eventType$ field of the $@Utilities/KeyboardEventData@$ structure specifies what type of keyboard event occurred
	//# and can be one of the following constants.
	//
	//# \value	kEventKeyDown		A key was pressed. If a key is held down long enough to trigger auto-repeat, then this
	//#								event is received each time a character is generated.
	//# \value	kEventKeyUp			A key was released.
	//# \value	kEventKeyCommand	A command key combination was pressed. This means the user held in the control key
	//#								(under Windows) or the command key (under Mac OS) while pressing another key.
	//
	//# \desc
	//# The $keyCode$ field of the $@Utilities/KeyboardEventData@$ structure specifies either the Unicode value corresponding
	//# to the character that was involved in the event or one of the following values for special keys.
	//
	//# \table	KeyCode
	//
	//# Special keys always have codes in the ranges [0x01, 0x1F] or [0x80, 0x9F], and keyboard event handlers should not
	//# attempt to handle codes in these ranges as ordinary characters.
	//#
	//# The $modifierKeys$ field specifies which modifier keys were held down when the event occurred. It can be zero
	//# or a combination (through logical OR) of the following values.
	//
	//# \table	KeyboardModifiers
	//
	//# The $cookie$ parameter is the value passed to the $KeyboardEventHandler$ constructor.
	//#
	//# The value returned by the handler specifies whether the keyboard event was successfully handled. If the handler
	//# returns $true$, then the keybaord event is considered handled, and no further keyboard event handlers will be called
	//# for the same event. If the handler returns $false$, then the event is passed to the next keyboard event handler.
	//#
	//# A keyboard event handler is uninstalled by destroying its associated class instance.
	//
	//# \base	Utilities/ListElement<KeyboardEventHandler>		Used internally to store all instances of $KeyboardEventHandler$ in a list.


	class KeyboardEventHandler : public ListElement<KeyboardEventHandler>
	{
		public:

			typedef bool HandlerProc(const KeyboardEventData *, void *);

		private:

			HandlerProc		*handlerProc;
			void			*handlerCookie;

		public:

			C4API KeyboardEventHandler(HandlerProc *proc, void *cookie = nullptr);

			bool HandleEvent(const KeyboardEventData *eventData) const
			{
				return ((*handlerProc)(eventData, handlerCookie));
			}
	};


	//# \class	Engine		The primary engine manager object.
	//
	//# The $Engine$ class is the primary engine manager object.
	//
	//# \def	class Engine : public Manager<Engine>
	//
	//# \desc
	//# The $Engine$ class is the primary engine manager object. It controls all of the other manager objects,
	//# provides low-level system utilities, and manages system commands and variables. The single instance of
	//# the $Engine$ class is automatically constructed during an application's initialization and
	//# destroyed at termination.
	//#
	//# The $Engine$ class's member functions are accessed through the global pointer $TheEngine$.


	//# \function	Engine::GetEngineFlags		Returns flags indicating the current engine state.
	//
	//# \proto	unsigned_int32 GetEngineFlags(void) const;
	//
	//# \desc
	//# The $GetEngineFlags$ function returns a combination (through logical OR) of the following constants.
	//
	//# \table	EngineFlags
	//
	//# \also	$@Reporter@$
	//# \also	$@Engine::Report@$


	//# \function	Engine::GetCommand			Returns a system command.
	//
	//# \proto	Command *GetCommand(const char *name) const;
	//
	//# \param	name	The name of the command.
	//
	//# \desc
	//# The $GetCommand$ function returns the system command matching the name specified by the $name$ parameter.
	//# If no such command exists, then the return value is $nullptr$.
	//
	//# \also	$@Engine::GetFirstCommand@$
	//# \also	$@Engine::AddCommand@$
	//# \also	$@Command@$


	//# \function	Engine::GetFirstCommand		Returns the first system command.
	//
	//# \proto	const Command *GetFirstCommand(void) const;
	//
	//# \desc
	//# The $GetFirstCommand$ function returns the first system command as determined by lexicographical ordering
	//# on the command name. The $@Utilities/MapElement::Next@$ function can be used to iterate through all commands
	//# currently registered with the engine.
	//
	//# \also	$@Engine::GetCommand@$
	//# \also	$@Engine::AddCommand@$
	//# \also	$@Command@$


	//# \function	Engine::AddCommand			Registers a system command.
	//
	//# \proto	bool AddCommand(Command *command);
	//
	//# \param	command		A pointer to the command to register.
	//
	//# \desc
	//# The $AddCommand$ function registers the system command specified by the $command$ parameter with the engine.
	//# If the command is successfully registered, then the return value is $true$. If another command with the same
	//# name has already been registered, then the return value is $false$, and the new command is not registered.
	//
	//# \also	$@Engine::GetCommand@$
	//# \also	$@Engine::GetFirstCommand@$
	//# \also	$@Command@$


	//# \function	Engine::GetVariable			Returns a system variable.
	//
	//# \proto	Variable *GetVariable(const char *name) const;
	//
	//# \param	name	The name of the variable.
	//
	//# \desc
	//# The $GetVariable$ function returns the system variable matching the name specified by the $name$ parameter.
	//# If no such variable exists, then the return value is $nullptr$.
	//
	//# \also	$@Engine::GetFirstVariable@$
	//# \also	$@Engine::AddVariable@$
	//# \also	$@Variable@$


	//# \function	Engine::GetFirstVariable	Returns the first system variable.
	//
	//# \proto	const Variable *GetFirstVariable(void) const;
	//
	//# \desc
	//# The $GetFirstVariable$ function returns the first system variable as determined by lexicographical ordering
	//# on the variable name. The $@Utilities/MapElement::Next@$ function can be used to iterate through all variables
	//# currently registered with the engine.
	//
	//# \also	$@Engine::GetVariable@$
	//# \also	$@Engine::AddVariable@$
	//# \also	$@Variable@$


	//# \function	Engine::AddVariable			Registers a system variable.
	//
	//# \proto	bool AddVariable(Variable *variable);
	//
	//# \param	variable	A pointer to the variable to register.
	//
	//# \desc
	//# The $AddVariable$ function registers the system variable specified by the $variable$ parameter with the engine.
	//# If the variable is successfully registered, then the return value is $true$. If another variable with the same
	//# name has already been registered, then the return value is $false$, and the new variable is not registered.
	//
	//# \also	$@Engine::GetVariable@$
	//# \also	$@Engine::GetFirstVariable@$
	//# \also	$@Variable@$


	//# \function	Engine::InstallReporter		Installs a report handler.
	//
	//# \proto	static void InstallReporter(Reporter *reporter);
	//
	//# \param	reporter	The reporter to install.
	//
	//# \desc
	//# The $InstallReporter$ function installs a reporter whose associated report procedure
	//# is invoked whenever the $@Engine::Report@$ function is called. Multiple reporters may
	//# be installed simultaneously, and each corresponding report procedure is called when a
	//# message is reported to the engine.
	//#
	//# A reporter is uninstalled by destroying its associated class instance.
	//
	//# \also	$@Reporter@$
	//# \also	$@Engine::Report@$


	//# \function	Engine::Report		Reports a text message to the engine.
	//
	//# \proto	static void Report(const char *text, unsigned_int32 flags = 0);
	//
	//# \param	text	The text message to report.
	//# \param	flags	Flags pertaining to the message.
	//
	//# \desc
	//# The $Report$ function reports the text message pointed to by the $text$ parameter to the
	//# engine. This has the effect of invoking the report procedures for any instances of the
	//# $@Reporter@$ class that have been installed using the $@Engine::InstallReporter@$ function.
	//#
	//# The $flags$ parameter specifies options that may be considered by any of the installed
	//# reporters. This parameter can be a combination (through logical OR) of the following constants.
	//
	//# \table	ReportFlags
	//
	//# The engine installs its own reporter that ignores all messages except those that have the
	//# $kReportLog$ flag set. The console window installs a reporter that displays all messages
	//# that do <i>not</i> have the $kReportLog$ flag set.
	//
	//# \also	$@Reporter@$
	//# \also	$@Engine::InstallReporter@$


	//# \function	Engine::InstallMouseEventHandler		Installs a mouse event handler.
	//
	//# \proto	void InstallMouseEventHandler(MouseEventHandler *handler);
	//
	//# \param	handler		The event handler to install.
	//
	//# \desc
	//# The $InstallMouseEventHandler$ function installs a mouse event handler that is invoked whenever
	//# a mouse event occurs. Multiple mouse event handlers may be installed simultaneously, and each
	//# one is invoked when an event occurs.
	//#
	//# A mouse event handler is uninstalled by destroying its associated class instance.
	//
	//# \also	$@MouseEventHandler@$
	//# \also	$@KeyboardEventHandler@$
	//# \also	$@Engine::InstallKeyboardEventHandler@$


	//# \function	Engine::InstallKeyboardEventHandler		Installs a keyboard event handler.
	//
	//# \proto	void InstallKeyboardEventHandler(KeyboardEventHandler *handler, bool last = false);
	//
	//# \param	handler		The event handler to install.
	//# \param	last		Indicates whether the handler is called last.
	//
	//# \desc
	//# The $InstallKeyboardEventHandler$ function installs a keyboard event handler that is invoked whenever
	//# a keyboard event occurs. If any handler reports that it has successfully handled the keyboard event,
	//# then the remaining handlers are not called.
	//#
	//# If the $last$ parameter if $true$, then the event handler specified by the $handler$ parameter is called
	//# after all previously installed keyboard event handlers. Otherwise, the new event handler is called before
	//# all previously installed handlers.
	//#
	//# The engine installs a single keyboard event handler when it starts up to handle user interface interaction.
	//#
	//# A keybaord event handler is uninstalled by destroying its associated class instance.
	//
	//# \also	$@KeyboardEventHandler@$
	//# \also	$@MouseEventHandler@$
	//# \also	$@Engine::InstallMouseEventHandler@$


	//# \function	Engine::Quit			Causes the engine to quit.
	//
	//# \proto	void Quit(void);
	//
	//# \desc
	//# The $Quit$ function causes the engine to quit at the end of the current game loop.


	//# \div
	//# \function	Engine::OpenExternalWebBrowser		Launches the default web browser and navigates to a URL.
	//
	//# \proto	bool OpenExternalWebBrowser(const char *url);
	//
	//# \param	url		The URL to which the web browser should navigate.
	//
	//# \desc
	//# The $OpenExternalWebBrowser$ function launches the default web browser on the local machine and navigates to
	//# the URL specified by the $url$ parameter. Upon success, this function returns $true$. If an error is reported
	//# by the operating system, then the return value is $false$.


	class Engine : public Manager<Engine>
	{
		friend class MouseDevice;

		#if C4LINUX

			friend class KeyboardDevice;

		#elif C4IOS //[ MOBILE

			// -- Mobile code hidden --

		#elif C4PS4 //[ PS4

			// -- PS4 code hidden --

		#elif C4PS3 //[ PS3

			// -- PS3 code hidden --

		#endif //]

		private:

			unsigned_int32					engineFlags;

			int32							processorCount;
			unsigned_int32					processorFlags;

			#if C4DESKTOP

				int32						backgroundSleepTime;

			#endif

			const char						*applicationName;
			ApplicationModule				*applicationModule;

			Map<Command>					commandMap;
			Map<Variable>					variableMap;

			#if C4LOG_FILE

				File						logFile;
				Reporter					logger;

			#endif

			static C4API List<Reporter>		reporterList;

			DeferredTask					inputConfigTask;
			DeferredTask					gameModuleTask;

			List<MouseEventHandler>			mouseEventHandlerList;
			List<KeyboardEventHandler>		keyboardEventHandlerList;

			static unsigned_int32			mouseButtonMask;
			static Point3D					lastMousePosition;

			int32							multiaxisMouseActiveCount;
			int32							tabletActiveCount;
			float							stylusPressure;

			#if C4WINDOWS

				unsigned_int32				stylusMinPressure;
				float						stylusPressureScale;

				WNDCLASSEXW					windowClass;
				HINSTANCE					engineInstance;
				HWND						engineWindow;

				HMODULE						tabletLibrary;
				HCTX						tabletContext;

				bool						deadKeyFlag;
				int32						wheelDeltaAccum;

				static LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

				bool HandleApplicationEvent(UINT message, WPARAM wparam, LPARAM lparam);
				bool HandleWindowEvent(UINT message, WPARAM wparam, LPARAM lparam);
				void HandleMouseEvent(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
				void HandleKeyboardEvent(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
				void HandleMultiaxisMouseEvent(WPARAM wparam, LPARAM lparam);
				void HandleTabletEvent(UINT message, WPARAM wparam, LPARAM lparam);

				static DWORD WINAPI ShellThread(void *cookie);

			#elif C4MACOS

				NSWindow					*engineWindow;
				CFBundleRef					openglBundle;

				CGFloat						wheelDeltaAccum;

			#elif C4LINUX

				::Display					*engineDisplay;
				::Window					engineWindow;

				::Atom						deleteWindowAtom;

				void HandleApplicationEvent(const XEvent *event);
				void HandleMouseEvent(EventType eventType, const Integer2D& point, int32 delta = 0);
				void HandleKeyboardEvent(EventType eventType, unsigned_int32 code, unsigned_int32 modifierKeys);

			#elif C4IOS //[ MOBILE

				// -- Mobile code hidden --

			#elif C4PS4 //[ PS4

				// -- PS4 code hidden --

			#elif C4PS3 //[ PS3

				// -- PS3 code hidden --

			#endif //]

			VariableObserver<Engine>		gameModuleObserver;

			CommandObserver<Engine>			timeCommandObserver;
			CommandObserver<Engine>			shotCommandObserver;
			CommandObserver<Engine>			recordCommandObserver;
			CommandObserver<Engine>			undefCommandObserver;
			CommandObserver<Engine>			bindCommandObserver;
			CommandObserver<Engine>			unbindCommandObserver;
			CommandObserver<Engine>			loadCommandObserver;
			CommandObserver<Engine>			unloadCommandObserver;
			CommandObserver<Engine>			cmdCommandObserver;
			CommandObserver<Engine>			varCommandObserver;
			CommandObserver<Engine>			execCommandObserver;
			CommandObserver<Engine>			importCommandObserver;
			CommandObserver<Engine>			sayCommandObserver;
			CommandObserver<Engine>			addressCommandObserver;
			CommandObserver<Engine>			resolveCommandObserver;
			CommandObserver<Engine>			disconnectCommandObserver;

			#if C4STATS

				CommandObserver<Engine>		statCommandObserver;

			#endif

			#if C4DIAGS

				CommandObserver<Engine>		wireCommandObserver;
				CommandObserver<Engine>		normCommandObserver;
				CommandObserver<Engine>		tangCommandObserver;
				CommandObserver<Engine>		lrgnCommandObserver;
				CommandObserver<Engine>		srgnCommandObserver;
				CommandObserver<Engine>		doffCommandObserver;
				CommandObserver<Engine>		spthCommandObserver;
				CommandObserver<Engine>		bodyCommandObserver;
				CommandObserver<Engine>		ctacCommandObserver;
				CommandObserver<Engine>		fbufCommandObserver;
				CommandObserver<Engine>		smapCommandObserver;
				CommandObserver<Engine>		netCommandObserver;
				CommandObserver<Engine>		extCommandObserver;
				CommandObserver<Engine>		rsrcCommandObserver;
				CommandObserver<Engine>		heapCommandObserver;

			#endif

			CommandObserver<Engine>			quitCommandObserver;

			void InitializeProcessorData(void);

			#if C4PS3 //[ PS3

				// -- PS3 code hidden --

			#endif //]

			EngineResult ConstructManagers(const char *commandLine);
			void DestroyManagers(void);

			#if C4LOG_FILE

				void BeginLog(void);
				void EndLog(void);

				static void Logger(const char *text, unsigned_int32 flags, void *cookie);

			#endif

			EngineResult LoadApplicationModule(void);
			void UnloadApplicationModule(void);

			static void ExecuteInputConfig(DeferredTask *task, void *cookie);
			static void ChangeGameModule(DeferredTask *task, void *cookie);

			void HandleGameModuleEvent(Variable *variable);

			static void ResetMouseButtonMask(void);

			static void ResolverComplete(DomainNameResolver *resolver, void *cookie);

			#if C4DESKTOP

				static bool PluginFilter(const char *name, unsigned_int32 flags, void *cookie);

			#endif

		public:

			Engine(int);
			~Engine();

			EngineResult Construct(void);
			void Destruct(void);

			unsigned_int32 GetEngineFlags(void) const
			{
				return (engineFlags);
			}

			int32 GetProcessorCount(void) const
			{
				return (processorCount);
			}

			unsigned_int32 GetProcessorFlags(void) const
			{
				return (processorFlags);
			}

			const char *GetApplicationName(void) const
			{
				return (applicationName);
			}

			const Command *GetFirstCommand(void) const
			{
				return (commandMap.First());
			}

			Command *GetCommand(const char *name) const
			{
				return (commandMap.Find(name));
			}

			bool AddCommand(Command *command)
			{
				return (commandMap.Insert(command));
			}

			const Variable *GetFirstVariable(void) const
			{
				return (variableMap.First());
			}

			Variable *GetVariable(const char *name) const
			{
				return (variableMap.Find(name));
			}

			bool AddVariable(Variable *variable)
			{
				return (variableMap.Insert(variable));
			}

			static void InstallReporter(Reporter *reporter)
			{
				reporterList.Append(reporter);
			}

			const MouseEventHandler *GetFirstMouseEventHandler(void) const
			{
				return (mouseEventHandlerList.First());
			}

			const KeyboardEventHandler *GetFirstKeyboardEventHandler(void) const
			{
				return (keyboardEventHandlerList.First());
			}

			void InstallMouseEventHandler(MouseEventHandler *handler)
			{
				mouseEventHandlerList.Prepend(handler);
			}

			void InstallKeyboardEventHandler(KeyboardEventHandler *handler, bool last = false)
			{
				if (!last)
				{
					keyboardEventHandlerList.Prepend(handler);
				}
				else
				{
					keyboardEventHandlerList.Append(handler);
				}
			}

			float GetStylusPressure(void)
			{
				return (stylusPressure);
			}

			#if C4WINDOWS

				HINSTANCE GetEngineInstance(void) const
				{
					return (engineInstance);
				}

				HWND GetEngineWindow(void) const
				{
					return (engineWindow);
				}

				EngineResult Initialize(const char *name, HINSTANCE instance, const char *commandLine);
				void Terminate(void);

			#elif C4MACOS

				NSWindow *GetEngineWindow(void) const
				{
					return (engineWindow);
				}

				CFBundleRef GetOpenGLBundle(void) const
				{
					return (openglBundle);
				}

				C4API EngineResult Initialize(const char *name, const char *commandLine);
				C4API void Terminate(void);

				void HandleApplicationForegroundEvent(void);
				void HandleApplicationBackgroundEvent(void);

				void HandleMouseEvent(NSEvent *event);
				void HandleKeyboardEvent(NSEvent *event);
				void HandleTabletEvent(NSEvent *event);

				static void *GetBundleFunctionAddress(CFBundleRef bundle, const char *name);

			#elif C4LINUX

				::Display *GetEngineDisplay(void) const
				{
					return (engineDisplay);
				}

				::Window GetEngineWindow(void) const
				{
					return (engineWindow);
				}

				EngineResult Initialize(const char *name, const char *commandLine);
				void Terminate(void);

			#elif C4IOS //[ MOBILE

				// -- Mobile code hidden --

			#else //]

				EngineResult Initialize(const char *name, const char *commandLine);
				void Terminate(void);

			#endif

			#if C4DESKTOP

				void GetPluginMap(const char *directory, Map<FileReference> *fileMap) const;

			#endif

			C4API Variable *InitVariable(const char *name, const char *value, unsigned_int32 flags = 0, Variable::ObserverType *observer = nullptr);

			C4API void ExecuteCommand(const char *text);
			C4API void ExecuteText(const char *text);
			C4API bool ExecuteFile(const char *name, ResourceCatalog *catalog = nullptr);

			C4API static void Report(const char *text, unsigned_int32 flags = 0);

			#if C4LOG_FILE

				C4API static void LogSource(const char *source);
				C4API static void LogResult(EngineResult result);

			#endif

			C4API static const char *GetExternalResultString(EngineResult result);
			C4API static const char *GetDataResultString(DataResult result);

			C4API bool Run(void);
			C4API void Quit(void);

			C4API void StartMultiaxisMouse(void);
			C4API void StopMultiaxisMouse(void);

			C4API void StartTablet(void);
			C4API void StopTablet(void);

			C4API bool OpenExternalWebBrowser(const char *url);

			#if C4LEAK_DETECTION

				C4API static void DumpMemory(const char *filename);

			#endif

			void HandleTimeCommand(Command *command, const char *text);
			void HandleShotCommand(Command *command, const char *text);
			void HandleRecordCommand(Command *command, const char *text);
			void HandleUndefCommand(Command *command, const char *text);
			void HandleBindCommand(Command *command, const char *text);
			void HandleUnbindCommand(Command *command, const char *text);
			void HandleLoadCommand(Command *command, const char *text);
			void HandleUnloadCommand(Command *command, const char *text);
			void HandleCmdCommand(Command *command, const char *text);
			void HandleVarCommand(Command *command, const char *text);
			void HandleExecCommand(Command *command, const char *text);
			void HandleImportCommand(Command *command, const char *text);
			void HandleSayCommand(Command *command, const char *text);
			void HandleAddressCommand(Command *command, const char *text);
			void HandleResolveCommand(Command *command, const char *text);
			void HandleDisconnectCommand(Command *command, const char *text);

			#if C4STATS

				void HandleStatCommand(Command *command, const char *text);

			#endif

			#if C4DIAGS

				void HandleWireCommand(Command *command, const char *text);
				void HandleNormCommand(Command *command, const char *text);
				void HandleTangCommand(Command *command, const char *text);
				void HandleLrgnCommand(Command *command, const char *text);
				void HandleSrgnCommand(Command *command, const char *text);
				void HandleDoffCommand(Command *command, const char *text);
				void HandleSpthCommand(Command *command, const char *text);
				void HandleBodyCommand(Command *command, const char *text);
				void HandleCtacCommand(Command *command, const char *text);
				void HandleFbufCommand(Command *command, const char *text);
				void HandleSmapCommand(Command *command, const char *text);
				void HandleNetCommand(Command *command, const char *text);
				void HandleExtCommand(Command *command, const char *text);
				void HandleRsrcCommand(Command *command, const char *text);
				void HandleHeapCommand(Command *command, const char *text);

			#endif

			void HandleQuitCommand(Command *command, const char *text);
	};


	class Module
	{
		private:

			ModuleType			moduleType;
			bool				moduleLoaded;

			#if C4WINDOWS

				HMODULE			moduleHandle;

			#elif C4POSIX

				void			*moduleHandle;

			#endif

		protected:

			Module(ModuleType type);

			#if C4WINDOWS

				HMODULE GetModuleHandle(void) const
				{
					return (moduleHandle);
				}

				void *GetFunctionAddress(const char *name) const
				{
					return (GetProcAddress(moduleHandle, name));
				}

			#elif C4POSIX

				void *GetModuleHandle(void) const
				{
					return (moduleHandle);
				}

				void *GetFunctionAddress(const char *name) const
				{
					return (dlsym(moduleHandle, name));
				}

			#endif

		public:

			virtual ~Module();

			virtual EngineResult Load(const char *name);
			void Unload(void);
	};


	class ApplicationModule : public Module
	{
		private:

			typedef Application *CreateProc(void);

		public:

			ApplicationModule();
			~ApplicationModule();

			EngineResult Load(const char *name);
	};


	C4API extern Engine *TheEngine;
}


#endif

// ZYUQURM
