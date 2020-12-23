 

#ifndef C4Interface_h
#define C4Interface_h


//# \component	Interface Manager
//# \prefix		InterfaceMgr/

//# \import		C4Widgets.h


#include "C4CameraObjects.h"
#include "C4StringTable.h"
#include "C4Display.h"
#include "C4Engine.h"
#include "C4Fonts.h"
#include "C4Menus.h"


namespace C4
{
	enum
	{
		kInterfaceColorDesktop,
		kInterfaceColorButton,
		kInterfaceColorHilite,
		kInterfaceColorWindowBack,
		kInterfaceColorBalloonBack,
		kInterfaceColorMenuBack,
		kInterfaceColorPageBack,
		kInterfaceColorWindowFrame,
		kInterfaceColorPageFrame,
		kInterfaceColorStripFrame,
		kInterfaceColorWindowTitle,
		kInterfaceColorMenuTitle,
		kInterfaceColorPageTitle,
		kInterfaceColorStripTitle,
		kInterfaceColorStripButton,
		kInterfaceColorCount
	};


	enum : EventType
	{
		kEventWindowAdd			= 'WNAD',
		kEventWindowRemove		= 'WNRM',
		kEventWindowChange		= 'WNCH'
	};


	enum
	{
		kInterfaceQuitDisabled			= 1 << 0,
		kInterfaceConsoleDisabled		= 1 << 1
	};


	//# \enum	InputManagementMode

	enum InputManagementMode
	{
		kInputManagementManual,			//## The Interface Manager does not change the Input Manager mode when windows are created and destroyed.
		kInputManagementAutomatic		//## The Interface Manager automatically changes the Input Manager mode when windows are created and destroyed.
	};


	struct PanelResourceHeader
	{
		int32				endian;
		int32				version;
		int32				widgetCount;
		unsigned_int32		auxiliaryDataSize;

		const char *GetAuxiliaryData(void) const
		{
			return (reinterpret_cast<const char *>(this + 1));
		}

		const char *GetWidgetData(void) const
		{
			return (reinterpret_cast<const char *>(this + 1) + auxiliaryDataSize);
		}
	};


	class PanelResource : public Resource<PanelResource>
	{
		friend class Resource<PanelResource>;

		private:

			static C4API ResourceDescriptor		descriptor;

			~PanelResource();

		public:

			C4API PanelResource(const char *name, ResourceCatalog *catalog);

			const PanelResourceHeader *GetPanelResourceHeader(void) const
			{
				return (static_cast<const PanelResourceHeader *>(GetData())); 
			}
	};
 

	//# \struct	WindowEventData		Contains information about a window event. 
	//
	//# The $WindowEventData$ structure contains information about a window event.
	// 
	//# \def	struct WindowEventData
	// 
	//# \data	WindowEventData 


	//# \member		WindowEventData
 
	struct WindowEventData
	{
		EventType	eventType;			//## The type of the event.
		Window		*eventWindow;		//## The window associated with the event.

		WindowEventData(EventType type, Window *window)
		{
			eventType = type;
			eventWindow = window;
		}
	};


	//# \class	WindowEventHandler		Encapsulates a window event handler function.
	//
	//# The $WindowEventHandler$ class encapsulates a window event handler function.
	//
	//# \def	class WindowEventHandler : public ListElement<WindowEventHandler>
	//
	//# \ctor	WindowEventHandler(HandlerProc *proc, void *cookie = nullptr);
	//
	//# \param	proc	The procedure to invoke when a window event occurs.
	//# \param	cookie	The cookie that is passed to the event handler as its last parameter.
	//
	//# \desc
	//# The $WindowEventHandler$ class encapsulates a procedure that is invoked when a
	//# window event occurs. Once an instance of the $WindowEventHandler$ class has
	//# been constructed, it can be installed by calling the $@InterfaceMgr::InstallWindowEventHandler@$ function.
	//#
	//# When a window event occurs, the procedures corresponding to all installed window event handlers are
	//# invoked. The $HandlerProc$ type is defined as follows.
	//
	//# \code	typedef void HandlerProc(const WindowEventData *eventData, void *cookie);
	//
	//# The $eventType$ field of the $@WindowEventData@$ structure specifies what type of window event occurred
	//# and can be one of the following constants.
	//
	//# \value	kEventWindowAdd			A window was added to the interface.
	//# \value	kEventWindowRemove		A window was removed from the interface.
	//# \value	kEventWindowChange		A window's enable state or visibility state was changed.
	//
	//# \desc
	//# The $eventWindow$ field of the $@WindowEventData@$ structure specifies the window for which the event occurred.
	//# The $cookie$ parameter is the value passed to the $WindowEventHandler$ constructor.
	//#
	//# A window event handler is uninstalled by destroying its associated class instance.
	//
	//# \base	Utilities/ListElement<WindowEventHandler>	Used internally to store all instances of $WindowEventHandler$ in a list.


	class WindowEventHandler : public ListElement<WindowEventHandler>
	{
		public:

			typedef void HandlerProc(const WindowEventData *, void *);

		private:

			HandlerProc		*handlerProc;
			void			*handlerCookie;

		public:

			C4API WindowEventHandler(HandlerProc *proc, void *cookie = nullptr);

			void HandleEvent(const WindowEventData *eventData) const
			{
				(*handlerProc)(eventData, handlerCookie);
			}
	};


	//# \class	Cursor		Encapsulates a cursor rendered in a user interface.
	//
	//# The $Cursor$ class encapsulates a cursor rendered in a user interface.
	//
	//# \def	class Cursor : public Renderable, public Transformable
	//
	//# \ctor	Cursor(const char *name);
	//
	//# \param	name	The name of the cursor's texture map.
	//
	//# \desc
	//# The $Cursor$ class encapsulates the texture image and transform for a cursor that is
	//# rendered in a user interface. Once a cursor has been created, it can be set as the current
	//# cursor for the main user interface by calling the $@InterfaceMgr::SetCursor@$ function.
	//
	//# \note
	//# Cursors are not shown on mobile platforms.
	//
	//# \base	GraphicsMgr/Renderable		A cursor is a renderable object.
	//# \base	Utilities/Transformable		Holds the cursor-to-world transform for a cursor.
	//
	//# \also	$@InterfaceMgr::SetCursor@$


	class Cursor : public Renderable, public Transformable
	{
		private:

			struct CursorVertex
			{
				Point2D		position;
				Point2D		texcoord;
			};

			Vector2D					cursorOffset;
			Vector2D					cursorSize;

			VertexBuffer				vertexBuffer;
			List<Attribute>				attributeList;
			DiffuseTextureAttribute		textureAttribute;

			void Initialize(void);
			C4API void Update(void);

		public:

			C4API Cursor();
			C4API Cursor(const char *name);
			C4API ~Cursor();

			const ResourceName& GetTextureName(void) const
			{
				return (textureAttribute.GetTextureName());
			}

			const Texture *GetTexture(void) const
			{
				return (textureAttribute.GetTexture());
			}

			void SetTexture(const char *name)
			{
				textureAttribute.SetTexture(name);
				Update();
			}

			const Vector2D& GetCursorOffset(void) const
			{
				return (cursorOffset);
			}

			const Vector2D& GetCursorSize(void) const
			{
				return (cursorSize);
			}
	};


	class StripBoard : public Board
	{
		private:

			StripWidget					stripWidget;
			MenuButtonWidget			menuButton;

			List<WindowButtonWidget>	windowButtonList;

			void HandleStructureUpdate(void) override;

		public:

			StripBoard(const Vector2D& size);
			~StripBoard();

			void SetWidgetSize(const Vector2D& size) override;
			void Preprocess(void) override;

			WindowButtonWidget *AddWindow(Window *window);

			C4API void HideEmpty(void);
	};


	//# \class	InterfaceMgr	The Interface Manager class.
	//
	//# \def	class InterfaceMgr : public Manager<InterfaceMgr>
	//
	//# \desc
	//# The $InterfaceMgr$ class encapsulates the user interface features of the C4 Engine.
	//# The single instance of the Interface Manager is constructed during an application's initialization
	//# and destroyed at termination.
	//#
	//# The Interface Manager's member functions are accessed through the global pointer $TheInterfaceMgr$.
	//
	//# \also	$@Window@$
	//# \also	$@Widget@$


	//# \function	InterfaceMgr::InstallWindowEventHandler		Installs a window event handler.
	//
	//# \proto	void InstallWindowEventHandler(WindowEventHandler *handler);
	//
	//# \param	handler		The event handler to install.
	//
	//# \desc
	//# The $InstallWindowEventHandler$ function installs a window event handler that is invoked whenever
	//# a window event occurs. Multiple window event handlers may be installed simultaneously, and each
	//# one is invoked when an event occurs.
	//#
	//# A window event handler is uninstalled by destroying its associated class instance.
	//
	//# \also	$@WindowEventHandler@$


	//# \function	InterfaceMgr::AddWidget		Adds a user interface element to the desktop.
	//
	//# \proto	void AddWidget(Widget *widget);
	//
	//# \param	widget		The interface widget to add.
	//
	//# \desc
	//# The $AddWidget$ function adds a user interface widget to the desktop. The $widget$ parameter can
	//# be a pointer to any instance of the $@Widget@$ class, but is normally a $@Window@$ or $@Board@$.
	//#
	//# The widget specified by the $widget$ parameter is preprocessed before the $AddWidget$ function returns.
	//#
	//# An interface widget is removed from the desktop by destroying it or calling the $@InterfaceMgr::RemoveWidget@$ function.
	//
	//# \also	$@InterfaceMgr::RemoveWidget@$
	//# \also	$@Window::AddSubwindow@$


	//# \function	InterfaceMgr::RemoveWidget		Removes a user interface element from the desktop.
	//
	//# \proto	void RemoveWidget(Widget *widget);
	//
	//# \param	widget		The interface widget to remove.
	//
	//# \desc
	//# The $RemoveWidget$ function removes a user interface widget from the desktop. The widget pointed to
	//# by the $widget$ parameter should be one that was previously added to the desktop using the $@InterfaceMgr::AddWidget@$ function.
	//
	//# \also	$@InterfaceMgr::AddWidget@$


	//# \div
	//# \function	InterfaceMgr::ConsoleEnabled		Returns a boolean value indicated whether the console is enabled.
	//
	//# \proto	bool ConsoleEnabled(void) const;
	//
	//# \desc
	//# The $ConsoleEnabled$ function returns $true$ if the console is currently enabled and $false$ otherwise.
	//# The console is enabled by default when the engine starts up. The console can be disabled by calling the
	//# $@InterfaceMgr::DisableConsole@$ function.
	//
	//# \also	$@InterfaceMgr::EnableConsole@$
	//# \also	$@InterfaceMgr::DisableConsole@$


	//# \function	InterfaceMgr::EnableConsole		Enables the console.
	//
	//# \proto	void EnableConsole(void);
	//
	//# \desc
	//# The $EnableConsole$ function enables access to the console when the keyboard is in interface event mode.
	//# To enable the console when the keyboard is in game input mode, call the $@InputMgr/InputMgr::SetConsoleProc@$
	//# function as well.
	//#
	//# The console is enabled by default when the engine starts up. The console can be disabled by calling the
	//# $@InterfaceMgr::DisableConsole@$ function.
	//
	//# \also	$@InterfaceMgr::DisableConsole@$
	//# \also	$@InterfaceMgr::ConsoleEnabled@$
	//# \also	$@InputMgr/InputMgr::SetConsoleProc@$


	//# \function	InterfaceMgr::DisableConsole	Disables the console.
	//
	//# \proto	void DisableConsole(void);
	//
	//# \desc
	//# The $DisableConsole$ function disables access to the console when the keyboard is in interface event mode.
	//# To disable the console when the keyboard is in game input mode, call the $@InputMgr/InputMgr::SetConsoleProc@$
	//# function as well, passing $nullptr$ as its first parameter.
	//
	//# \also	$@InterfaceMgr::DisableConsole@$
	//# \also	$@InterfaceMgr::ConsoleEnabled@$
	//# \also	$@InputMgr/InputMgr::SetConsoleProc@$


	//# \function	InterfaceMgr::SetInputManagementMode	Sets the input management mode.
	//
	//# \proto	SetInputManagementMode(InputManagementMode mode);
	//
	//# \param	mode		The new input management mode.
	//
	//# \desc
	//# The $SetInputManagementMode$ function determines whether the Interface Manager modifies the current
	//# Input Manager mode when windows are created and destroyed. The $mode$ parameter can be one of the
	//# following constants.
	//
	//# \table	InputManagementMode
	//
	//# If the mode is $kInputManagementAutomatic$, then the Interface Manager sets the Input Manager mode to
	//# $kInputAllActive$ whenever there are no windows visible that are capable of receiving user input.
	//# When there is at least one input-receiving window visible, the Interface Manager automatically sets
	//# the Input Manager mode to $kInputInactive$, meaning that mouse and keyboard events are sent to the
	//# currently active window and are not processed by the Input Manager.
	//#
	//# If the mode is $kInputManagementManual$, then the Interface Manager does not modify the Input
	//# Manager mode regardless of whether input-receiving windows are visible.
	//#
	//# A window is considered able to receive input if it does not have the $kWindowPassive$ window flag
	//# set (see the $@Window@$ class).
	//#
	//# The default input management mode when the engine starts up is $kInputManagementManual$.
	//
	//# \also	$@InputMgr/InputMgr::SetInputMode@$
	//# \also	$@Window@$


	//# \div
	//# \function	InterfaceMgr::GetDesktopSize		Returns the size of the desktop area.
	//
	//# \proto	const Vector2D& GetDesktopSize(void) const;
	//
	//# \desc
	//# The $GetDesktopSize$ function returns a 2D vector containing the size of the desktop, in pixels.
	//# The desktop size excludes the area needed for the strip at the bottom of the display, and thus
	//# represents the largest area that a window should normally occupy.
	//#
	//# The full size of the display can be queried using the $@GraphicsMgr/DisplayMgr::GetDisplayWidth@$
	//# and $@GraphicsMgr/DisplayMgr::GetDisplayHeight@$ functions.
	//
	//# \also	$@GraphicsMgr/DisplayMgr::GetDisplayWidth@$
	//# \also	$@GraphicsMgr/DisplayMgr::GetDisplayHeight@$


	//# \div
	//# \function	InterfaceMgr::SetCursor		Sets the cursor that is rendered by the Interface Manager.
	//
	//# \proto	void SetCursor(Cursor *cursor);
	//
	//# \param	cursor		A pointer to the cursor to be used. This can be $nullptr$.
	//
	//# \desc
	//# The $SetCursor$ function sets the current cursor to that specified by the $cursor$ parameter. If the
	//# $cursor$ parameter is $nullptr$, then the current cursor becomes the default arrow cursor.
	//
	//# \also	$@InterfaceMgr::CursorVisible@$
	//# \also	$@InterfaceMgr::HideCursor@$
	//# \also	$@InterfaceMgr::ShowCursor@$
	//# \also	$@Cursor@$


	//# \function	InterfaceMgr::CursorVisible		Returns the cursor visibility state.
	//
	//# \proto	bool CursorVisible(void) const;
	//
	//# \desc
	//# The $CursorVisible$ function returns a boolean value indicating whether the cursor is currently visible.
	//# The visibility state of the cursor can be changed by calling the $@InterfaceMgr::HideCursor@$ and
	//# $@InterfaceMgr::ShowCursor@$ functions.
	//
	//# \also	$@InterfaceMgr::HideCursor@$
	//# \also	$@InterfaceMgr::ShowCursor@$
	//# \also	$@InterfaceMgr::SetCursor@$


	//# \function	InterfaceMgr::HideCursor		Hides the cursor.
	//
	//# \proto	void HideCursor(void);
	//
	//# \desc
	//# The $HideCursor$ function hides the cursor rendered by the Interface Manager. The cursor is not rendered
	//# again until the $@InterfaceMgr::ShowCursor@$ function is called.
	//#
	//# The $HideCursor$ and $ShowCursor$ functions are not counted, so multiple calls to $HideCursor$ will not prevent
	//# the first call to $ShowCursor$ from showing the cursor.
	//
	//# \also	$@InterfaceMgr::ShowCursor@$
	//# \also	$@InterfaceMgr::CursorVisible@$
	//# \also	$@InterfaceMgr::SetCursor@$


	//# \function	InterfaceMgr::ShowCursor		Shows the cursor.
	//
	//# \proto	void ShowCursor(void);
	//
	//# \desc
	//# The $ShowCursor$ function shows the cursor rendered by the Interface Manager.
	//#
	//# The $ShowCursor$ and $HideCursor$ functions are not counted, so multiple calls to $HideCursor$ will not prevent
	//# the first call to $ShowCursor$ from showing the cursor.
	//
	//# \also	$@InterfaceMgr::HideCursor@$
	//# \also	$@InterfaceMgr::CursorVisible@$
	//# \also	$@InterfaceMgr::SetCursor@$


	//# \div
	//# \function	InterfaceMgr::GetShiftKey		Returns a boolean value indicating whether the Shift key is pressed.
	//
	//# \proto	static bool GetShiftKey(void);
	//
	//# \desc
	//# The $GetShiftKey$ function returns $true$ if the Shift key is currently pressed and $false$ otherwise.
	//# No distinction is made between left and right Shift keys, and $true$ is returned if either is pressed.
	//#
	//# If a keyboard event is received, then it is not necessary to call the $GetShiftKey$ function to determine whether
	//# the Shift key was pressed for that event. Instead, you should check the $modifierKeys$ field of the
	//# $@Utilities/KeyboardEventData@$ structure and test for the $kModifierKeyShift$ flag.
	//
	//# \also	$@InterfaceMgr::GetOptionKey@$
	//# \also	$@InterfaceMgr::GetCommandKey@$


	//# \function	InterfaceMgr::GetOptionKey		Returns a boolean value indicating whether the option key is pressed.
	//
	//# \proto	static bool GetOptionKey(void);
	//
	//# \desc
	//# The $GetOptionKey$ function returns $true$ if the option key is currently pressed and $false$ otherwise.
	//# No distinction is made between left and right option keys, and $true$ is returned if either is pressed.
	//#
	//# The name "option key" is used as a platform-neutral term, and it refers to different physical keys on different platforms.
	//# Under Windows, the $GetOptionKey$ function returns $true$ if the Alt key is pressed. Under Mac OS X, the $GetOptionKey$
	//# function returns $true$ if the Option key is pressed.
	//
	//# \also	$@InterfaceMgr::GetShiftKey@$
	//# \also	$@InterfaceMgr::GetCommandKey@$


	//# \function	InterfaceMgr::GetCommandKey		Returns a boolean value indicating whether the command key is pressed.
	//
	//# \proto	static bool GetCommandKey(void);
	//
	//# \desc
	//# The $GetCommandKey$ function returns $true$ if the command key is currently pressed and $false$ otherwise.
	//# No distinction is made between left and right command keys, and $true$ is returned if either is pressed.
	//#
	//# The name "command key" is used as a platform-neutral term, and it refers to different physical keys on different platforms.
	//# Under Windows, the $GetCommandKey$ function returns $true$ if the Control key is pressed. Under Mac OS X, the $GetCommandKey$
	//# function returns $true$ if the Command (&#x2318;) key is pressed.
	//
	//# \also	$@InterfaceMgr::GetShiftKey@$
	//# \also	$@InterfaceMgr::GetOptionKey@$


	class InterfaceMgr : public Manager<InterfaceMgr>
	{
		private:

			Creator<Widget>				widgetCreator;
			List<WindowEventHandler>	windowEventHandlerList;

			WindowEventHandler			windowEventHandler;
			MouseEventHandler			mouseEventHandler;
			KeyboardEventHandler		keyboardEventHandler;
			DisplayEventHandler			displayEventHandler;

			Cursor						*arrowCursor;
			StringTable					*stringTable;
			MaterialObject				*materialObject;

			PulldownMenuWidget			*toolsMenu;
			StripBoard					*stripBoard;

			Widget						*rootWidget;
			Widget						*windowRoot;

			List<Window>				windowList;
			Link<Widget>				activeWindow;
			Link<Widget>				activeMenu;

			Vector2D					desktopSize;

			OrthoCameraObject			*interfaceCamera;
			Transformable				cameraTransformable;

			Link<Widget>				hoverWidget;
			int32						hoverTime;

			Link<Widget>				trackWidget;
			WidgetPart					trackPart;

			Link<Widget>				wheelWidget;
			Point3D						wheelPosition;

			Cursor						*currentCursor;
			Point3D						cursorPosition;
			bool						cursorVisible;

			unsigned_int32				previousClickTime;
			Point3D						previousClickPosition;

			int32						doubleClickTime;
			int32						caretBlinkTime;

			unsigned_int32				interfaceFlags;
			InputManagementMode			inputManagementMode;

			String<>					clipboard;

			ColorRGBA					interfaceColor[kInterfaceColorCount];

			VariableObserver<InterfaceMgr>		desktopColorObserver;
			VariableObserver<InterfaceMgr>		buttonColorObserver;
			VariableObserver<InterfaceMgr>		hiliteColorObserver;
			VariableObserver<InterfaceMgr>		windowBackColorObserver;
			VariableObserver<InterfaceMgr>		balloonBackColorObserver;
			VariableObserver<InterfaceMgr>		menuBackColorObserver;
			VariableObserver<InterfaceMgr>		pageBackColorObserver;
			VariableObserver<InterfaceMgr>		windowFrameColorObserver;
			VariableObserver<InterfaceMgr>		pageFrameColorObserver;
			VariableObserver<InterfaceMgr>		stripFrameColorObserver;
			VariableObserver<InterfaceMgr>		windowTitleColorObserver;
			VariableObserver<InterfaceMgr>		menuTitleColorObserver;
			VariableObserver<InterfaceMgr>		pageTitleColorObserver;
			VariableObserver<InterfaceMgr>		stripTitleColorObserver;
			VariableObserver<InterfaceMgr>		stripButtonColorObserver;

			#if C4LINUX

				struct KeycodeList
				{
					KeyCode				leftShiftKeycode;
					KeyCode				rightShiftKeycode;
					KeyCode				leftAltKeycode;
					KeyCode				rightAltKeycode;
					KeyCode				leftControlKeycode;
					KeyCode				rightControlKeycode;
				};

				static KeycodeList		keycodeList;

			#endif

			static Widget *CreateWidget(Unpacker& data, unsigned_int32 unpackFlags);

			void HandleDesktopColorEvent(Variable *variable);
			void HandleButtonColorEvent(Variable *variable);
			void HandleHiliteColorEvent(Variable *variable);
			void HandleWindowBackColorEvent(Variable *variable);
			void HandleBalloonBackColorEvent(Variable *variable);
			void HandleMenuBackColorEvent(Variable *variable);
			void HandlePageBackColorEvent(Variable *variable);
			void HandleWindowFrameColorEvent(Variable *variable);
			void HandlePageFrameColorEvent(Variable *variable);
			void HandleStripFrameColorEvent(Variable *variable);
			void HandleWindowTitleColorEvent(Variable *variable);
			void HandleMenuTitleColorEvent(Variable *variable);
			void HandlePageTitleColorEvent(Variable *variable);
			void HandleStripTitleColorEvent(Variable *variable);
			void HandleStripButtonColorEvent(Variable *variable);

			void BringToFront(Window *window);

			Widget *FindHoverWidget(Widget **hoverRoot) const;
			void DisplayBalloon(Widget *widget, Widget *hoverRoot);
			void CancelHover(void);

			bool EnabledInputWindow(void) const;

			bool HandleMouseDownEvent(const MouseEventData *eventData);
			bool HandleMouseUpEvent(const MouseEventData *eventData);
			bool HandleMouseMovedEvent(const MouseEventData *eventData);
			bool HandleMouseWheelEvent(const MouseEventData *eventData);
			bool HandleMultiaxisMouseEvent(const MouseEventData *eventData);

			static void HandleWindowEvent(const WindowEventData *eventData, void *cookie);
			static bool HandleMouseEvent(const MouseEventData *eventData, void *cookie);
			static bool HandleKeyboardEvent(const KeyboardEventData *eventData, void *cookie);
			static void HandleDisplayEvent(const DisplayEventData *eventData, void *cookie);

		public:

			InterfaceMgr(int);
			~InterfaceMgr();

			EngineResult Construct(void);
			void Destruct(void);

			const WindowEventHandler *GetFirstWindowEventHandler(void) const
			{
				return (windowEventHandlerList.First());
			}

			void InstallWindowEventHandler(WindowEventHandler *handler)
			{
				windowEventHandlerList.Append(handler);
			}

			const StringTable *GetStringTable(void) const
			{
				return (stringTable);
			}

			MaterialObject *const *GetMaterialObjectPointer(void) const
			{
				return (&materialObject);
			}

			PulldownMenuWidget *GetToolsMenu(void) const
			{
				return (toolsMenu);
			}

			StripBoard *GetStrip(void) const
			{
				return (stripBoard);
			}

			Widget *DetectWidget(const Point3D& position, unsigned_int32 usage = 0, WidgetPart *widgetPart = nullptr) const
			{
				return (windowRoot->DetectWidget(position, usage, widgetPart));
			}

			Window *GetActiveWindow(void) const
			{
				return (static_cast<Window *>(activeWindow.GetTarget()));
			}

			Menu *GetActiveMenu(void) const
			{
				return (static_cast<Menu *>(activeMenu.GetTarget()));
			}

			const Vector2D& GetDesktopSize(void) const
			{
				return (desktopSize);
			}

			void SetCursor(Cursor *cursor)
			{
				currentCursor = (cursor) ? cursor : arrowCursor;
			}

			bool CursorVisible(void) const
			{
				return (cursorVisible);
			}

			void HideCursor(void)
			{
				cursorVisible = false;
			}

			void ShowCursor(void)
			{
				cursorVisible = true;
			}

			const Point3D& GetCursorPosition(void) const
			{
				return (cursorPosition);
			}

			int32 GetDoubleClickTime(void) const
			{
				return (doubleClickTime);
			}

			int32 GetCaretBlinkTime(void) const
			{
				return (caretBlinkTime);
			}

			bool QuitEnabled(void) const
			{
				return ((interfaceFlags & kInterfaceQuitDisabled) == 0);
			}

			void EnableQuit(void)
			{
				interfaceFlags &= ~kInterfaceQuitDisabled;
			}

			void DisableQuit(void)
			{
				interfaceFlags |= kInterfaceQuitDisabled;
			}

			bool ConsoleEnabled(void) const
			{
				return ((interfaceFlags & kInterfaceConsoleDisabled) == 0);
			}

			void EnableConsole(void)
			{
				interfaceFlags &= ~kInterfaceConsoleDisabled;
			}

			void DisableConsole(void)
			{
				interfaceFlags |= kInterfaceConsoleDisabled;
			}

			InputManagementMode GetInputManagementMode(void) const
			{
				return (inputManagementMode);
			}

			void SetInputManagementMode(InputManagementMode mode)
			{
				inputManagementMode = mode;
				SetInputMode();
			}

			String<>& GetClipboard(void)
			{
				return (clipboard);
			}

			const ColorRGBA& GetInterfaceColor(int32 index) const
			{
				return (interfaceColor[index]);
			}

			C4API static bool GetShiftKey(void);
			C4API static bool GetOptionKey(void);
			C4API static bool GetCommandKey(void);
			C4API static unsigned_int32 GetModifierKeys(void);

			void PostWindowEvent(const WindowEventData& eventData);

			C4API void AddWidget(Widget *widget);
			C4API void RemoveWidget(Widget *widget);

			C4API void SetActiveWindow(Window *window);
			C4API void SelectActiveWindow(void);

			C4API void SetActiveMenu(Menu *menu);

			C4API void SetInputMode(void);

			C4API void ReadSystemClipboard(void);
			C4API void WriteSystemClipboard(void);

			void InterfaceTask(void);
			void Render(void);
	};


	C4API extern InterfaceMgr *TheInterfaceMgr;
}


#endif

// ZYUQURM
