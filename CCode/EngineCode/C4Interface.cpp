 

#include "C4Interface.h"
#include "C4FilePicker.h"
#include "C4ToolWindows.h"
#include "C4Input.h"
#include "C4World.h"


using namespace C4;


namespace
{
	const float kStripHeight = 30.0F;
}


InterfaceMgr *C4::TheInterfaceMgr = nullptr;


namespace C4
{
	template <> InterfaceMgr Manager<InterfaceMgr>::managerObject(0);
	template <> InterfaceMgr **Manager<InterfaceMgr>::managerPointer = &TheInterfaceMgr;

	template <> const char *const Manager<InterfaceMgr>::resultString[] =
	{
		nullptr
	};

	template <> const unsigned_int32 Manager<InterfaceMgr>::resultIdentifier[] =
	{
		0
	};

	template class Manager<InterfaceMgr>;
}


#if C4LINUX

	InterfaceMgr::KeycodeList InterfaceMgr::keycodeList = {0};

#endif


ResourceDescriptor PanelResource::descriptor("pan");


PanelResource::PanelResource(const char *name, ResourceCatalog *catalog) : Resource<PanelResource>(name, catalog)
{
}

PanelResource::~PanelResource()
{
}


WindowEventHandler::WindowEventHandler(HandlerProc *proc, void *cookie)
{
	handlerProc = proc;
	handlerCookie = cookie;
}


C4::Cursor::Cursor() : Renderable(kRenderTriangleStrip),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
{
	Initialize();
}

C4::Cursor::Cursor(const char *name) :
		Renderable(kRenderTriangleStrip),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		textureAttribute(name)
{
	Initialize();
	Update();
}

C4::Cursor::~Cursor()
{
}

void C4::Cursor::Initialize(void)
{
	SetIdentityTransform();

	#if !C4MOBILE

		SetAmbientBlendState(kBlendInterpolate);
		SetTransformable(this);

		SetVertexCount(4);
		SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(CursorVertex));
		SetVertexAttributeArray(kArrayPosition, 0, 2);
		SetVertexAttributeArray(kArrayTexcoord, sizeof(Point2D), 2);
		vertexBuffer.Establish(sizeof(CursorVertex) * 4);

		attributeList.Append(&textureAttribute);
		SetMaterialAttributeList(&attributeList);
 
	#endif
}
 
void C4::Cursor::Update(void)
{ 
	#if !C4MOBILE

		volatile CursorVertex *restrict vertex = vertexBuffer.BeginUpdateSync<CursorVertex>(); 

		const Texture *texture = textureAttribute.GetTexture(); 
		float width = (float) texture->GetTextureWidth(); 
		float height = (float) texture->GetTextureHeight();

		cursorOffset = -texture->GetImageCenter();
		cursorSize.Set(width, height); 

		vertex[0].position.Set(cursorOffset.x, cursorOffset.y);
		vertex[0].texcoord.Set(0.0F, 1.0F);

		vertex[1].position.Set(cursorOffset.x, cursorOffset.y + height);
		vertex[1].texcoord.Set(0.0F, 0.0F);

		vertex[2].position.Set(cursorOffset.x + width, cursorOffset.y);
		vertex[2].texcoord.Set(1.0F, 1.0F);

		vertex[3].position.Set(cursorOffset.x + width, cursorOffset.y + height);
		vertex[3].texcoord.Set(1.0F, 0.0F);

		vertexBuffer.EndUpdateSync();

	#endif
}


StripBoard::StripBoard(const Vector2D& size) :
		Board(size),
		stripWidget(size),
		menuButton(Vector2D(46.0F, 46.0F), Point2D(0.75F, 0.6875F), Point2D(0.875F, 0.8125F))
{
	menuButton.SetHiliteTexcoordOffset(Vector2D(0.0F, -0.1875F));
	menuButton.SetMenuPositionOffset(Vector2D(0.0F, 3.0F));
	menuButton.SetMenu(TheInterfaceMgr->GetToolsMenu());
	menuButton.SetBalloon(kBalloonResource, "C4/Menu");

	SetActiveUpdateFlags(GetActiveUpdateFlags() | kUpdateStructure);
}

StripBoard::~StripBoard()
{
}

void StripBoard::HandleStructureUpdate(void)
{
	int32 totalCount = 0;
	int32 visibleCount = 0;

	WindowButtonWidget *widget = windowButtonList.First();
	while (widget)
	{
		totalCount++;
		visibleCount += widget->GetWindow()->Visible();

		widget = widget->Next();
	}

	if (totalCount > 0)
	{
		float width = Fmin(Floor((GetWidgetSize().x - 54.0F) / (float) visibleCount), 160.0F);
		float x = 54.0F;

		widget = windowButtonList.First();
		while (widget)
		{
			if (widget->GetWindow()->Visible())
			{
				widget->Show();
				widget->SetWidgetSize(Vector2D(width, widget->GetWidgetSize().y));
				widget->SetWidgetPosition(Point3D(x, 1.0F, 0.0F));
				widget->Invalidate();

				x += width;
			}
			else
			{
				widget->Hide();
			}

			widget = widget->Next();
		}
	}
}

void StripBoard::SetWidgetSize(const Vector2D& size)
{
	Board::SetWidgetSize(size);
	stripWidget.SetWidgetSize(size);
}

void StripBoard::Preprocess(void)
{
	AppendSubnode(&stripWidget);

	menuButton.SetWidgetPosition(Point3D(4.0F, (stripWidget.GetWidgetSize().y - menuButton.GetWidgetSize().y) * 0.5F, 0.0F));
	AppendSubnode(&menuButton);

	Board::Preprocess();
}

WindowButtonWidget *StripBoard::AddWindow(Window *window)
{
	WindowButtonWidget *widget = new WindowButtonWidget(window);
	windowButtonList.Append(widget);

	AppendNewSubnode(widget);
	Invalidate();

	return (widget);
}

void StripBoard::HideEmpty(void)
{
	WindowButtonWidget *widget = windowButtonList.First();
	while (widget)
	{
		if (widget->GetWindow()->Visible())
		{
			return;
		}

		widget = widget->Next();
	}

	Hide();
	TheInterfaceMgr->SetInputMode();
}


InterfaceMgr::InterfaceMgr(int) :
		widgetCreator(&CreateWidget),
		windowEventHandler(&HandleWindowEvent, this),
		mouseEventHandler(&HandleMouseEvent, this),
		keyboardEventHandler(&HandleKeyboardEvent, this),
		displayEventHandler(&HandleDisplayEvent, this),
		desktopColorObserver(this, &InterfaceMgr::HandleDesktopColorEvent),
		buttonColorObserver(this, &InterfaceMgr::HandleButtonColorEvent),
		hiliteColorObserver(this, &InterfaceMgr::HandleHiliteColorEvent),
		windowBackColorObserver(this, &InterfaceMgr::HandleWindowBackColorEvent),
		balloonBackColorObserver(this, &InterfaceMgr::HandleBalloonBackColorEvent),
		menuBackColorObserver(this, &InterfaceMgr::HandleMenuBackColorEvent),
		pageBackColorObserver(this, &InterfaceMgr::HandlePageBackColorEvent),
		windowFrameColorObserver(this, &InterfaceMgr::HandleWindowFrameColorEvent),
		pageFrameColorObserver(this, &InterfaceMgr::HandlePageFrameColorEvent),
		stripFrameColorObserver(this, &InterfaceMgr::HandleStripFrameColorEvent),
		windowTitleColorObserver(this, &InterfaceMgr::HandleWindowTitleColorEvent),
		menuTitleColorObserver(this, &InterfaceMgr::HandleMenuTitleColorEvent),
		pageTitleColorObserver(this, &InterfaceMgr::HandlePageTitleColorEvent),
		stripTitleColorObserver(this, &InterfaceMgr::HandleStripTitleColorEvent),
		stripButtonColorObserver(this, &InterfaceMgr::HandleStripButtonColorEvent)
{
}

InterfaceMgr::~InterfaceMgr()
{
}

EngineResult InterfaceMgr::Construct(void)
{
	Widget::InstallCreator(&widgetCreator);
	InstallWindowEventHandler(&windowEventHandler);
	TheEngine->InstallMouseEventHandler(&mouseEventHandler);
	TheEngine->InstallKeyboardEventHandler(&keyboardEventHandler);
	TheDisplayMgr->InstallDisplayEventHandler(&displayEventHandler);

	int32 displayWidth = TheDisplayMgr->GetDisplayWidth();
	int32 displayHeight = TheDisplayMgr->GetDisplayHeight();
	desktopSize.Set((float) displayWidth, (float) displayHeight - kStripHeight);

	interfaceCamera = new OrthoCameraObject;
	interfaceCamera->SetNearDepth(-1.0F);
	interfaceCamera->SetFarDepth(1.0F);

	#if C4OCULUS

		if (TheDisplayMgr->GetFullFrameWidth() != displayWidth)
		{
			interfaceCamera->SetProjectionOffset(Oculus::GetLensCenter());
		}

	#endif

	cameraTransformable.SetWorldTransform(Transform4D(1.0F, 0.0F, 0.0F, 0.0F, 0.0F, -1.0F, 0.0F, 0.0F, 0.0F, 0.0F, -1.0F, 0.0F));

	arrowCursor = new Cursor("C4/arrow");
	stringTable = new StringTable("C4/engine");

	materialObject = new MaterialObject;
	materialObject->AddAttribute(new DiffuseTextureAttribute("C4/gui1"));
	materialObject->AddAttribute(new EmissionTextureAttribute("C4/gui2"));

	TheEngine->InitVariable("desktopColor", "404040", kVariablePermanent, &desktopColorObserver);
	TheEngine->InitVariable("buttonColor", "40D0FF", kVariablePermanent, &buttonColorObserver);
	TheEngine->InitVariable("hiliteColor", "00C080", kVariablePermanent, &hiliteColorObserver);
	TheEngine->InitVariable("windowBackColor", "A0A0A0", kVariablePermanent, &windowBackColorObserver);
	TheEngine->InitVariable("balloonBackColor", "EDDBB8", kVariablePermanent, &balloonBackColorObserver);
	TheEngine->InitVariable("menuBackColor", "C0C0C0F7", kVariablePermanent, &menuBackColorObserver);
	TheEngine->InitVariable("pageBackColor", "787878", kVariablePermanent, &pageBackColorObserver);
	TheEngine->InitVariable("windowFrameColor", "A0B0C0", kVariablePermanent, &windowFrameColorObserver);
	TheEngine->InitVariable("pageFrameColor", "A0B0C0", kVariablePermanent, &pageFrameColorObserver);
	TheEngine->InitVariable("stripFrameColor", "C0C0D0", kVariablePermanent, &stripFrameColorObserver);
	TheEngine->InitVariable("windowTitleColor", "FFFFFF", kVariablePermanent, &windowTitleColorObserver);
	TheEngine->InitVariable("menuTitleColor", "000000", kVariablePermanent, &menuTitleColorObserver);
	TheEngine->InitVariable("pageTitleColor", "FFFFFF", kVariablePermanent, &pageTitleColorObserver);
	TheEngine->InitVariable("stripTitleColor", "000000", kVariablePermanent, &stripTitleColorObserver);
	TheEngine->InitVariable("stripButtonColor", "40FFD0", kVariablePermanent, &stripButtonColorObserver);

	toolsMenu = new PulldownMenuWidget(nullptr);

	stripBoard = new StripBoard(Vector2D(desktopSize.x, kStripHeight));
	stripBoard->SetWidgetPosition(Point3D(0.0F, desktopSize.y, 0.0F));
	stripBoard->Preprocess();
	stripBoard->Hide();

	rootWidget = new Widget;
	windowRoot = new Widget;

	currentCursor = arrowCursor;
	cursorPosition.Set(0.0F, 0.0F, 0.0F);
	cursorVisible = true;

	#if C4WINDOWS

		doubleClickTime = ::GetDoubleClickTime();
		caretBlinkTime = ::GetCaretBlinkTime();

	#elif C4MACOS

		doubleClickTime = (int32) ([NSEvent doubleClickInterval] * 1000.0);
		caretBlinkTime = 560;

	#elif C4LINUX

		doubleClickTime = 200;
		caretBlinkTime = 350;

		::Display *display = TheEngine->GetEngineDisplay();
		keycodeList.leftShiftKeycode = XKeysymToKeycode(display, XK_Shift_L);
		keycodeList.rightShiftKeycode = XKeysymToKeycode(display, XK_Shift_R);
		keycodeList.leftAltKeycode = XKeysymToKeycode(display, XK_Alt_L);
		keycodeList.rightAltKeycode = XKeysymToKeycode(display, XK_Alt_R);
		keycodeList.leftControlKeycode = XKeysymToKeycode(display, XK_Control_L);
		keycodeList.rightControlKeycode = XKeysymToKeycode(display, XK_Control_R);

	#elif C4IOS //[ MOBILE

		// -- Mobile code hidden --

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#endif //]

	previousClickTime = -doubleClickTime - 1;

	interfaceFlags = 0;
	inputManagementMode = kInputManagementManual;

	ReadSystemClipboard();
	return (kEngineOkay);
}

void InterfaceMgr::Destruct(void)
{
	// Set input managament mode to manual to prevent input mode from being changed as windows are destroyed.

	inputManagementMode = kInputManagementManual;

	delete activeMenu;
	delete windowRoot;
	delete rootWidget;
	delete stripBoard;
	delete toolsMenu;

	materialObject->Release();
	delete stringTable;
	delete arrowCursor;

	interfaceCamera->Release();

	displayEventHandler.Detach();
	keyboardEventHandler.Detach();
	mouseEventHandler.Detach();
	Widget::RemoveCreator(&widgetCreator);

	WriteSystemClipboard();
	clipboard.Purge();

	FilePicker::PurgeVisits();
}

Widget *InterfaceMgr::CreateWidget(Unpacker& data, unsigned_int32 unpackFlags)
{
	if (data.GetType() == kWidgetGeneric)
	{
		return (new Widget);
	}

	return (nullptr);
}

void InterfaceMgr::HandleDesktopColorEvent(Variable *variable)
{
	ColorRGB	color;

	color.SetHexString(variable->GetValue());
	interfaceColor[kInterfaceColorDesktop] = color;
	interfaceCamera->SetClearColor(color);
}

void InterfaceMgr::HandleButtonColorEvent(Variable *variable)
{
	interfaceColor[kInterfaceColorButton] = ColorRGBA().SetHexString(variable->GetValue());
}

void InterfaceMgr::HandleHiliteColorEvent(Variable *variable)
{
	interfaceColor[kInterfaceColorHilite] = ColorRGBA().SetHexString(variable->GetValue());
}

void InterfaceMgr::HandleWindowBackColorEvent(Variable *variable)
{
	interfaceColor[kInterfaceColorWindowBack] = ColorRGBA().SetHexString(variable->GetValue());
}

void InterfaceMgr::HandleBalloonBackColorEvent(Variable *variable)
{
	interfaceColor[kInterfaceColorBalloonBack] = ColorRGBA().SetHexString(variable->GetValue());
}

void InterfaceMgr::HandleMenuBackColorEvent(Variable *variable)
{
	interfaceColor[kInterfaceColorMenuBack] = ColorRGBA().SetHexString(variable->GetValue());
}

void InterfaceMgr::HandlePageBackColorEvent(Variable *variable)
{
	interfaceColor[kInterfaceColorPageBack] = ColorRGBA().SetHexString(variable->GetValue());
}

void InterfaceMgr::HandleWindowFrameColorEvent(Variable *variable)
{
	interfaceColor[kInterfaceColorWindowFrame] = ColorRGBA().SetHexString(variable->GetValue());
}

void InterfaceMgr::HandlePageFrameColorEvent(Variable *variable)
{
	interfaceColor[kInterfaceColorPageFrame] = ColorRGBA().SetHexString(variable->GetValue());
}

void InterfaceMgr::HandleStripFrameColorEvent(Variable *variable)
{
	interfaceColor[kInterfaceColorStripFrame] = ColorRGBA().SetHexString(variable->GetValue());
}

void InterfaceMgr::HandleWindowTitleColorEvent(Variable *variable)
{
	interfaceColor[kInterfaceColorWindowTitle] = ColorRGBA().SetHexString(variable->GetValue());
}

void InterfaceMgr::HandleMenuTitleColorEvent(Variable *variable)
{
	interfaceColor[kInterfaceColorMenuTitle] = ColorRGBA().SetHexString(variable->GetValue());
}

void InterfaceMgr::HandlePageTitleColorEvent(Variable *variable)
{
	interfaceColor[kInterfaceColorPageTitle] = ColorRGBA().SetHexString(variable->GetValue());
}

void InterfaceMgr::HandleStripTitleColorEvent(Variable *variable)
{
	interfaceColor[kInterfaceColorStripTitle] = ColorRGBA().SetHexString(variable->GetValue());
}

void InterfaceMgr::HandleStripButtonColorEvent(Variable *variable)
{
	interfaceColor[kInterfaceColorStripButton] = ColorRGBA().SetHexString(variable->GetValue());
}

bool InterfaceMgr::GetShiftKey(void)
{
	#if C4WINDOWS

		return (GetAsyncKeyState(VK_SHIFT) < 0);

	#elif C4MACOS

		return (([NSEvent modifierFlags] & NSShiftKeyMask) != 0);

	#elif C4LINUX

		char	keymap[32];

		XQueryKeymap(TheEngine->GetEngineDisplay(), keymap);

		const KeycodeList *list = &keycodeList;
		int32 left = list->leftShiftKeycode;
		int32 right = list->rightShiftKeycode;
		return (((keymap[left >> 3] & (1 << (left & 7))) != 0) || ((keymap[right >> 3] & (1 << (right & 7))) != 0));

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#else //]

		return (false);

	#endif
}

bool InterfaceMgr::GetOptionKey(void)
{
	#if C4WINDOWS

		return (GetAsyncKeyState(VK_MENU) < 0);

	#elif C4MACOS

		return (([NSEvent modifierFlags] & NSAlternateKeyMask) != 0);

	#elif C4LINUX

		char	keymap[32];

		XQueryKeymap(TheEngine->GetEngineDisplay(), keymap);

		const KeycodeList *list = &keycodeList;
		int32 left = list->leftAltKeycode;
		int32 right = list->rightAltKeycode;
		return (((keymap[left >> 3] & (1 << (left & 7))) != 0) || ((keymap[right >> 3] & (1 << (right & 7))) != 0));

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#else //]

		return (false);

	#endif
}

bool InterfaceMgr::GetCommandKey(void)
{
	#if C4WINDOWS

		return (GetAsyncKeyState(VK_CONTROL) < 0);

	#elif C4MACOS

		return (([NSEvent modifierFlags] & NSCommandKeyMask) != 0);

	#elif C4LINUX

		char	keymap[32];

		XQueryKeymap(TheEngine->GetEngineDisplay(), keymap);

		const KeycodeList *list = &keycodeList;
		int32 left = list->leftControlKeycode;
		int32 right = list->rightControlKeycode;
		return (((keymap[left >> 3] & (1 << (left & 7))) != 0) || ((keymap[right >> 3] & (1 << (right & 7))) != 0));

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#else //]

		return (false);

	#endif
}

unsigned_int32 InterfaceMgr::GetModifierKeys(void)
{
	#if C4WINDOWS

		unsigned_int32 flags = (GetAsyncKeyState(VK_SHIFT) < 0) ? kModifierKeyShift : 0;

		if (GetAsyncKeyState(VK_MENU) < 0)
		{
			flags |= kModifierKeyOption;
		}

		if (GetAsyncKeyState(VK_CONTROL) < 0)
		{
			flags |= kModifierKeyCommand;
		}

		return (flags);

	#elif C4MACOS

		NSUInteger modifiers = [NSEvent modifierFlags];

		unsigned_int32 flags = ((modifiers & NSShiftKeyMask) != 0) ? kModifierKeyShift : 0;

		if ((modifiers & NSAlternateKeyMask) != 0)
		{
			flags |= kModifierKeyOption;
		}

		if ((modifiers & NSCommandKeyMask) != 0)
		{
			flags |= kModifierKeyCommand;
		}

		return (flags);

	#elif C4LINUX

		char	keymap[32];

		XQueryKeymap(TheEngine->GetEngineDisplay(), keymap);

		const KeycodeList *list = &keycodeList;
		int32 left = list->leftShiftKeycode;
		int32 right = list->rightShiftKeycode;
		unsigned_int32 flags = (((keymap[left >> 3] & (1 << (left & 7))) != 0) || ((keymap[right >> 3] & (1 << (right & 7))) != 0)) ? kModifierKeyShift : 0;

		left = list->leftAltKeycode;
		right = list->rightAltKeycode;
		if (((keymap[left >> 3] & (1 << (left & 7))) != 0) || ((keymap[right >> 3] & (1 << (right & 7))) != 0))
		{
			flags |= kModifierKeyOption;
		}

		left = list->leftControlKeycode;
		right = list->rightControlKeycode;
		if (((keymap[left >> 3] & (1 << (left & 7))) != 0) || ((keymap[right >> 3] & (1 << (right & 7))) != 0))
		{
			flags |= kModifierKeyCommand;
		}

		return (flags);

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#else //]

		return (0);

	#endif
}

void InterfaceMgr::PostWindowEvent(const WindowEventData& eventData)
{
	const WindowEventHandler *handler = windowEventHandlerList.First();
	while (handler)
	{
		const WindowEventHandler *next = handler->Next();
		handler->HandleEvent(&eventData);
		handler = next;
	}
}

void InterfaceMgr::HandleWindowEvent(const WindowEventData *eventData, void *cookie)
{
	InterfaceMgr *interfaceMgr = static_cast<InterfaceMgr *>(cookie);

	switch (eventData->eventType)
	{
		case kEventWindowAdd:

			interfaceMgr->SetInputMode();
			break;

		case kEventWindowRemove:

			interfaceMgr->SetInputMode();
			break;

		case kEventWindowChange:
		{
			Window *window = eventData->eventWindow;

			if (window == interfaceMgr->GetActiveWindow())
			{
				if (!window->Visible())
				{
					interfaceMgr->SelectActiveWindow();
				}
			}

			if (window->GetWindowButton())
			{
				interfaceMgr->stripBoard->Invalidate();
			}

			interfaceMgr->SetInputMode();
			break;
		}
	}
}

void InterfaceMgr::HandleDisplayEvent(const DisplayEventData *eventData, void *cookie)
{
	InterfaceMgr *interfaceMgr = static_cast<InterfaceMgr *>(cookie);

	if (eventData->eventType == kEventDisplayChange)
	{
		StripBoard *stripBoard = interfaceMgr->stripBoard;
		float h = stripBoard->GetWidgetSize().y;

		Vector2D& desktopSize = interfaceMgr->desktopSize;
		desktopSize.Set((float) TheDisplayMgr->GetDisplayWidth(), (float) TheDisplayMgr->GetDisplayHeight() - h);

		stripBoard->SetWidgetSize(Vector2D(desktopSize.x, h));
		stripBoard->SetWidgetPosition(Point3D(0.0F, desktopSize.y, 0.0F));
		stripBoard->Invalidate();

		Window *window = interfaceMgr->windowList.First();
		while (window)
		{
			unsigned_int32 flags = window->GetWindowFlags();
			if (flags & (kWindowFullHorizontal | kWindowFullVertical))
			{
				window->ScaleWindow();
			}
			else if (flags & kWindowCenter)
			{
				window->CenterWindow();
			}
			else
			{
				const Point3D& position = window->GetWorldPosition();
				if ((position.x > desktopSize.x - 4.0F) || (position.y > desktopSize.y - 4.0F))
				{
					float right = position.x + window->GetWidgetSize().x + 8.0F;
					float bottom = position.y + window->GetWidgetSize().y + 8.0F;
					float dx = (right > desktopSize.x) ? desktopSize.x - right : 0.0F;
					float dy = (bottom > desktopSize.y) ? desktopSize.y - bottom : 0.0F;

					window->SetWidgetPosition(window->GetWidgetPosition() + Vector3D(dx, dy, 0.0F));
					window->Invalidate();
				}
			}

			window = window->Next();
		}
	}
}

void InterfaceMgr::AddWidget(Widget *widget)
{
	widget->Preprocess();

	if (widget->GetWidgetType() == kWidgetWindow)
	{
		Window *window = static_cast<Window *>(widget);
		if ((window->GetWidgetState() & (kWidgetDisabled | kWidgetHidden)) == 0)
		{
			SetActiveWindow(window);
		}
		else
		{
			windowRoot->AppendSubnode(window);
			windowList.Append(window);
		}

		PostWindowEvent(WindowEventData(kEventWindowAdd, window));
	}
	else
	{
		rootWidget->AppendSubnode(widget);
	}
}

void InterfaceMgr::RemoveWidget(Widget *widget)
{
	if (widget->GetWidgetType() == kWidgetWindow)
	{
		Window *window = static_cast<Window *>(widget);
		Window *owner = window->GetOwningWindow();
		window->Detach();

		if (GetActiveWindow() == window)
		{
			activeWindow = nullptr;

			if (owner)
			{
				SetActiveWindow(owner);
			}
			else
			{
				SelectActiveWindow();
			}
		}

		PostWindowEvent(WindowEventData(kEventWindowRemove, window));
	}
	else
	{
		widget->Detach();
	}
}

void InterfaceMgr::BringToFront(Window *window)
{
	Window *owner = window->GetOwningWindow();
	if (owner)
	{
		owner->BringToFront(window);
		BringToFront(owner);
	}
	else
	{
		windowRoot->AppendSubnode(window);
		windowList.Append(window);
	}
}

void InterfaceMgr::SetActiveWindow(Window *window)
{
	Window *previousActiveWindow = GetActiveWindow();
	if (previousActiveWindow != window)
	{
		CancelHover();
		wheelWidget = nullptr;

		if (previousActiveWindow)
		{
			previousActiveWindow->EnterBackground();
			activeWindow = nullptr;
		}

		if (window)
		{
			Window *subwindow = window->GetFirstSubwindow();
			while (subwindow)
			{
				if (subwindow->GetWindowFlags() & kWindowModal)
				{
					window = subwindow;
					subwindow = subwindow->GetFirstSubwindow();
					continue;
				}

				subwindow = subwindow->Next();
			}

			BringToFront(window);
			activeWindow = window;
			window->EnterForeground();
		}
	}
}

void InterfaceMgr::SelectActiveWindow(void)
{
	Window *window = windowList.Last();
	while (window)
	{
		if ((window->GetWidgetState() & (kWidgetDisabled | kWidgetHidden)) == 0)
		{
			SetActiveWindow(window);
			return;
		}

		window = window->Previous();
	}

	window = GetActiveWindow();
	if (window)
	{
		window->EnterBackground();
		activeWindow = nullptr;
	}
}

void InterfaceMgr::SetActiveMenu(Menu *menu)
{
	Menu *previousActiveMenu = GetActiveMenu();
	if (previousActiveMenu != menu)
	{
		delete previousActiveMenu;
		CancelHover();

		if (menu)
		{
			menu->Preprocess();
			activeMenu = menu;
			trackWidget = menu;
		}
	}
}

void InterfaceMgr::SetInputMode(void)
{
	if (inputManagementMode == kInputManagementAutomatic)
	{
		if ((stripBoard->Visible()) || (EnabledInputWindow()))
		{
			TheInputMgr->SetInputMode(kInputInactive);
			ShowCursor();
		}
		else if (!(TheEngine->GetEngineFlags() & kEngineQuit))
		{
			TheInputMgr->SetInputMode(kInputAllActive);
			HideCursor();
		}
	}
}

bool InterfaceMgr::EnabledInputWindow(void) const
{
	Window *window = windowList.First();
	while (window)
	{
		if (((window->GetWidgetState() & (kWidgetDisabled | kWidgetHidden)) == 0) && (!(window->GetWindowFlags() & kWindowPassive)))
		{
			return (true);
		}

		window = window->Next();
	}

	return (false);
}

bool InterfaceMgr::HandleMouseDownEvent(const MouseEventData *eventData)
{
	PanelMouseEventData		panelEventData;

	EventType eventType = eventData->eventType;
	panelEventData.eventType = eventType;
	panelEventData.eventFlags = 0;

	cursorPosition.Set(Fmin(FmaxZero(eventData->mousePosition.x), (float) TheDisplayMgr->GetDisplayWidth()), Fmin(FmaxZero(eventData->mousePosition.y), (float) TheDisplayMgr->GetDisplayHeight()), 0.0F);

	unsigned_int32 time = TheTimeMgr->GetSystemAbsoluteTime();
	if (eventType == kEventMouseDown)
	{
		if (((int32) (time - previousClickTime) < doubleClickTime) && (SquaredMag(cursorPosition.GetVector2D() - previousClickPosition.GetVector2D()) < 144.0F))
		{
			panelEventData.eventFlags = kMouseDoubleClick;
			previousClickTime = time - doubleClickTime;
		}
		else
		{
			previousClickTime = time;
		}
	}
	else
	{
		previousClickTime = time - doubleClickTime;
	}

	previousClickPosition = cursorPosition;

	if (!trackWidget)
	{
		Widget *widget = (stripBoard->Visible()) ? stripBoard->DetectWidget(cursorPosition, 0, &panelEventData.widgetPart) : nullptr;
		if (!widget)
		{
			widget = windowRoot->DetectWidget(cursorPosition, 0, &panelEventData.widgetPart);
		}

		if ((widget) && (panelEventData.widgetPart != kWidgetPartNone))
		{
			unsigned_int32 usage = widget->GetWidgetUsage();

			Window *window = widget->GetOwningWindow();
			if (window)
			{
				if (GetActiveWindow() != window)
				{
					SetActiveWindow(window);
					if (GetActiveWindow() != window)
					{
						if ((widget->GetWidgetType() != kWidgetWindowFrame) || (panelEventData.widgetPart != kWidgetPartTitle))
						{
							return (true);
						}
					}
				}

				if (usage & kWidgetKeyboardFocus)
				{
					window->SetFocusWidget(widget);
				}
			}

			if (!(usage & kWidgetTrackInhibit))
			{
				if ((eventType == kEventMouseDown) || ((eventType == kEventRightMouseDown) && (usage & kWidgetTrackRightMouse)) || ((eventType == kEventMiddleMouseDown) && (usage & kWidgetTrackMiddleMouse)))
				{
					CancelHover();

					trackWidget = widget;
					trackPart = panelEventData.widgetPart;
				}
			}

			panelEventData.mousePosition = widget->GetInverseWorldTransform() * cursorPosition;
			widget->HandleMouseEvent(&panelEventData);
			return (true);
		}
	}
	else if (trackWidget->GetWidgetType() == kWidgetMenu)
	{
		panelEventData.mousePosition = trackWidget->GetInverseWorldTransform() * cursorPosition;
		panelEventData.widgetPart = kWidgetPartInterior;
		trackWidget->HandleMouseEvent(&panelEventData);
		return (true);
	}

	return (false);
}

bool InterfaceMgr::HandleMouseUpEvent(const MouseEventData *eventData)
{
	cursorPosition.Set(Fmin(FmaxZero(eventData->mousePosition.x), (float) TheDisplayMgr->GetDisplayWidth()), Fmin(FmaxZero(eventData->mousePosition.y), (float) TheDisplayMgr->GetDisplayHeight()), 0.0F);

	Widget *widget = trackWidget;
	if (widget)
	{
		PanelMouseEventData		panelEventData;

		if (widget->GetWidgetType() != kWidgetMenu)
		{
			trackWidget = nullptr;
		}

		panelEventData.eventType = eventData->eventType;
		panelEventData.eventFlags = 0;
		panelEventData.mousePosition = widget->GetInverseWorldTransform() * cursorPosition;
		panelEventData.widgetPart = trackPart;
		widget->HandleMouseEvent(&panelEventData);
		return (true);
	}

	return (false);
}

bool InterfaceMgr::HandleMouseMovedEvent(const MouseEventData *eventData)
{
	cursorPosition.Set(Fmin(FmaxZero(eventData->mousePosition.x), (float) TheDisplayMgr->GetDisplayWidth()), Fmin(FmaxZero(eventData->mousePosition.y), (float) TheDisplayMgr->GetDisplayHeight()), 0.0F);

	Widget *widget = trackWidget;
	if (widget)
	{
		PanelMouseEventData		panelEventData;

		panelEventData.eventType = eventData->eventType;
		panelEventData.eventFlags = 0;
		panelEventData.mousePosition = widget->GetInverseWorldTransform() * cursorPosition;
		panelEventData.widgetPart = trackPart;
		widget->HandleMouseEvent(&panelEventData);

		const Widget *menu = activeMenu;
		if (menu)
		{
			MenuBarWidget *menuBar = static_cast<const Menu *>(menu)->GetMenuBar();
			if (menuBar)
			{
				panelEventData.mousePosition = menuBar->GetInverseWorldTransform() * cursorPosition;
				panelEventData.widgetPart = kWidgetPartInterior;
				menuBar->HandleMouseEvent(&panelEventData);
			}
		}

		return (true);
	}

	return (false);
}

bool InterfaceMgr::HandleMouseWheelEvent(const MouseEventData *eventData)
{
	cursorPosition.Set(Fmin(FmaxZero(eventData->mousePosition.x), (float) TheDisplayMgr->GetDisplayWidth()), Fmin(FmaxZero(eventData->mousePosition.y), (float) TheDisplayMgr->GetDisplayHeight()), 0.0F);

	Window *window = GetActiveWindow();
	if (window)
	{
		PanelMouseEventData		panelEventData;

		Widget *widget = wheelWidget;
		if (widget)
		{
			if (SquaredMag(cursorPosition.GetPoint2D() - wheelPosition.GetPoint2D()) < 144.0F)
			{
				Point3D position = widget->GetInverseWorldTransform() * cursorPosition;
				if ((widget->WidgetContainsPoint(position)) && (widget->TestPosition(position) == kWidgetPartInterior))
				{
					goto post;
				}
			}
		}

		widget = window->DetectWidget(cursorPosition, kWidgetMouseWheel, &panelEventData.widgetPart);
		if ((!widget) || (panelEventData.widgetPart == kWidgetPartNone))
		{
			wheelWidget = nullptr;
			return (false);
		}

		wheelWidget = widget;
		wheelPosition = cursorPosition;

		post:
		panelEventData.eventType = kEventMouseWheel;
		panelEventData.eventFlags = 0;
		panelEventData.mousePosition = widget->GetInverseWorldTransform() * cursorPosition;
		panelEventData.wheelDelta = eventData->wheelDelta;

		widget->HandleMouseEvent(&panelEventData);
		return (true);
	}

	return (false);
}

bool InterfaceMgr::HandleMultiaxisMouseEvent(const MouseEventData *eventData)
{
	Window *window = GetActiveWindow();
	if (window)
	{
		PanelMouseEventData		panelEventData;

		Widget *widget = window->DetectWidget(cursorPosition, kWidgetMultiaxisMouse, &panelEventData.widgetPart);
		if ((widget) && (panelEventData.widgetPart != kWidgetPartNone))
		{
			panelEventData.eventType = eventData->eventType;

			if (eventData->eventType != kEventMultiaxisMouseButtonState)
			{
				panelEventData.eventFlags = 0;
				panelEventData.mousePosition = eventData->mousePosition;
			}
			else
			{
				panelEventData.eventFlags = eventData->eventFlags;
				panelEventData.mousePosition.Set(0.0F, 0.0F, 0.0F);
			}

			widget->HandleMouseEvent(&panelEventData);
			return (true);
		}
	}

	return (false);
}

bool InterfaceMgr::HandleMouseEvent(const MouseEventData *eventData, void *cookie)
{
	InterfaceMgr *interfaceMgr = static_cast<InterfaceMgr *>(cookie);

	switch (eventData->eventType)
	{
		case kEventMouseDown:
		case kEventRightMouseDown:
		case kEventMiddleMouseDown:

			return (interfaceMgr->HandleMouseDownEvent(eventData));

		case kEventMouseUp:
		case kEventRightMouseUp:
		case kEventMiddleMouseUp:

			return (interfaceMgr->HandleMouseUpEvent(eventData));

		case kEventMouseMoved:

			return (interfaceMgr->HandleMouseMovedEvent(eventData));

		case kEventMouseWheel:

			return (interfaceMgr->HandleMouseWheelEvent(eventData));

		case kEventMultiaxisMouseTranslation:
		case kEventMultiaxisMouseRotation:
		case kEventMultiaxisMouseButtonState:

			return (interfaceMgr->HandleMultiaxisMouseEvent(eventData));
	}

	return (false);
}

bool InterfaceMgr::HandleKeyboardEvent(const KeyboardEventData *eventData, void *cookie)
{
	InterfaceMgr *interfaceMgr = static_cast<InterfaceMgr *>(cookie);
	interfaceMgr->previousClickTime = TheTimeMgr->GetSystemAbsoluteTime() - interfaceMgr->doubleClickTime;

	Menu *menu = interfaceMgr->GetActiveMenu();
	if (!menu)
	{
		EventType eventType = eventData->eventType;

		if ((eventData->modifierKeys & kModifierKeyConsole) && (interfaceMgr->ConsoleEnabled()))
		{
			if (eventType == kEventKeyDown)
			{
				if (interfaceMgr->activeWindow != TheConsoleWindow)
				{
					interfaceMgr->SetActiveWindow(TheConsoleWindow);
				}
				else
				{
					TheConsoleWindow->Close();
				}
			}

			return (true);
		}

		Window *window = interfaceMgr->GetActiveWindow();
		if (window)
		{
			if (window->HandleKeyboardEvent(eventData))
			{
				return (true);
			}
		}

		if (eventType == kEventKeyDown)
		{
			if ((eventData->keyCode == kKeyCodeEscape) && (!interfaceMgr->EnabledInputWindow()))
			{
				if (interfaceMgr->stripBoard->Visible())
				{
					interfaceMgr->stripBoard->Hide();
					interfaceMgr->SetInputMode();
					return (true);
				}
			}
		}
		else if (eventType == kEventKeyCommand)
		{
			if (interfaceMgr->toolsMenu->HandleKeyboardEvent(eventData))
			{
				return (true);
			}
		}
	}
	else
	{
		return (menu->HandleKeyboardEvent(eventData));
	}

	return (false);
}

void InterfaceMgr::ReadSystemClipboard(void)
{
	enum
	{
		kMaxClipboardTextLength = 2048
	};

	#if C4WINDOWS

		if (OpenClipboard(nullptr))
		{
			HANDLE data = GetClipboardData(CF_UNICODETEXT);
			if (data)
			{
				const unsigned_int16 *wideText = static_cast<const unsigned_int16 *>(GlobalLock(data));

				int32 length = 0;
				for (machine a = 0;; a++)
				{
					unsigned_int32 code = wideText[a];
					if (code == 0)
					{
						break;
					}

					if (code <= 0x007F)
					{
						length++;
					}
					else if (code <= 0x07FF)
					{
						length += 2;
					}
					else if (code <= 0xFFFF)
					{
						length += 3;
					}
				}

				length = Min(length, kMaxClipboardTextLength);
				clipboard.SetLength(length);

				int32 x = 0;
				char *byte = clipboard;
				for (machine a = 0; x < length; a++)
				{
					x += Text::WriteGlyphCodeUTF8(&byte[x], wideText[a]);
				}

				GlobalUnlock(data);
			}

			CloseClipboard();
		}

	#elif C4MACOS

		PasteboardRef	pasteboard;

		if (PasteboardCreate(kPasteboardClipboard, &pasteboard) == noErr)
		{
			ItemCount	itemCount;

			PasteboardSynchronize(pasteboard);
			if (PasteboardGetItemCount(pasteboard, &itemCount) == noErr)
			{
				for (unsigned_machine itemIndex = 1; itemIndex <= itemCount; itemIndex++)
				{
					PasteboardItemID	identifier;
					CFDataRef			data;

					PasteboardGetItemIdentifier(pasteboard, itemIndex, &identifier);
					if ((PasteboardCopyItemFlavorData(pasteboard, identifier, kUTTypeUTF8PlainText, &data) == noErr) && (data))
					{
						CFIndex len = CFDataGetLength(data);
						const void *text = CFDataGetBytePtr(data);
						clipboard.Set(static_cast<const char *>(text), Min(len, kMaxClipboardTextLength));
						CFRelease(data);
						break;
					}
				}
			}

			CFRelease(pasteboard);
		}

	#endif
}

void InterfaceMgr::WriteSystemClipboard(void)
{
	if (clipboard[0] != 0)
	{
		#if C4WINDOWS

			if (OpenClipboard(TheEngine->GetEngineWindow()))
			{
				EmptyClipboard();

				int32 count = Text::GetGlyphCountUTF8(clipboard);

				HGLOBAL data = GlobalAlloc(GMEM_MOVEABLE, count * 2 + 2);
				if (data)
				{
					unsigned_int16 *wideText = static_cast<unsigned_int16 *>(GlobalLock(data));

					const char *byte = clipboard;
					for (machine a = 0; a < count; a++)
					{
						unsigned_int32	code;

						byte += Text::ReadGlyphCodeUTF8(byte, &code);
						wideText[a] = (unsigned_int16) code;
					}

					wideText[count] = 0;

					GlobalUnlock(data);
					SetClipboardData(CF_UNICODETEXT, data);
				}

				CloseClipboard();
			}

		#elif C4MACOS

			PasteboardRef	pasteboard;

			if (PasteboardCreate(kPasteboardClipboard, &pasteboard) == noErr)
			{
				PasteboardClear(pasteboard);

				const char *text = clipboard;
				CFDataRef data = CFDataCreate(kCFAllocatorDefault, reinterpret_cast<const UInt8 *>(text), clipboard.Length());
				if (data)
				{
					PasteboardPutItemFlavor(pasteboard, nullptr, kUTTypeUTF8PlainText, data, 0);
					CFRelease(data);
				}

				CFRelease(pasteboard);
			}

		#endif
	}
}

Widget *InterfaceMgr::FindHoverWidget(Widget **hoverRoot) const
{
	if (stripBoard->Visible())
	{
		Widget *widget = stripBoard->DetectWidget(cursorPosition);
		if (widget)
		{
			*hoverRoot = stripBoard;
			return (widget);
		}
	}

	Widget *window = activeWindow;
	if (window)
	{
		Widget *widget = window->DetectWidget(cursorPosition);
		if (widget)
		{
			*hoverRoot = window;
			return (widget);
		}
	}

	return (nullptr);
}

void InterfaceMgr::DisplayBalloon(Widget *widget, Widget *hoverRoot)
{
	BalloonType type = widget->GetBalloonType();
	if (type != kBalloonNone)
	{
		Balloon *balloon = new Balloon(type, widget->GetBalloonString());
		widget->SetWidgetBalloon(balloon);

		Point3D position = widget->GetWorldPosition();
		position.x += PositiveFloor(widget->GetWidgetSize().x * 0.5F) - 25.0F;

		float offset = 0.0F;
		if (position.x < 2.0F)
		{
			offset = position.x - 2.0F;
			position.x = 2.0F;
		}
		else
		{
			float right = position.x + balloon->GetWidgetSize().x;
			float maxRight = desktopSize.x - 2.0F;

			if (right > maxRight)
			{
				offset = right - maxRight;
				position.x -= offset;
			}
		}

		int32 location = 0;
		float height = balloon->GetWidgetSize().y + 12.0F;
		if (position.y + widget->GetWidgetSize().y + height > desktopSize.y - 2.0F)
		{
			location = 1;
			position.y -= height;
		}
		else
		{
			position.y += widget->GetWidgetSize().y + 12.0F;
		}

		balloon->SetWidgetPosition(hoverRoot->GetInverseWorldTransform() * position);
		balloon->SetWedgeLocation(location, offset);
		hoverRoot->AppendNewSubnode(balloon);
	}
}

void InterfaceMgr::CancelHover(void)
{
	Widget *widget = hoverWidget;
	if (widget)
	{
		delete widget->GetWidgetBalloon();
		hoverWidget = nullptr;

		widget->PostWidgetEvent(WidgetEventData(kEventWidgetEndHover));
	}
}

void InterfaceMgr::InterfaceTask(void)
{
	if (!trackWidget)
	{
		Widget	*hoverRoot;

		Widget *newHoverWidget = FindHoverWidget(&hoverRoot);
		if (newHoverWidget)
		{
			Widget *prevHoverWidget = hoverWidget;
			if (prevHoverWidget)
			{
				Widget *balloon = prevHoverWidget->GetWidgetBalloon();

				if (newHoverWidget == prevHoverWidget)
				{
					int32 time = hoverTime + TheTimeMgr->GetSystemDeltaTime();
					hoverTime = time;

					if ((!balloon) && (time >= 500))
					{
						DisplayBalloon(newHoverWidget, hoverRoot);
					}
				}
				else
				{
					hoverWidget = newHoverWidget;
					if (!balloon)
					{
						hoverTime = 0;
					}
					else
					{
						delete balloon;
						hoverTime = 300;
					}

					prevHoverWidget->PostWidgetEvent(WidgetEventData(kEventWidgetEndHover));
					newHoverWidget->PostWidgetEvent(WidgetEventData(kEventWidgetBeginHover));
				}
			}
			else
			{
				hoverWidget = newHoverWidget;
				hoverTime = 0;

				newHoverWidget->PostWidgetEvent(WidgetEventData(kEventWidgetBeginHover));
			}
		}
		else
		{
			CancelHover();
		}
	}
	else
	{
		trackWidget->TrackTask(trackPart, trackWidget->GetInverseWorldTransform() * cursorPosition);
	}

	Widget *widget = rootWidget->GetFirstSubnode();
	while (widget)
	{
		Widget *next = widget->Next();
		widget->Move();
		widget = next;
	}

	widget = windowRoot->GetFirstSubnode();
	while (widget)
	{
		Widget *next = widget->Next();
		widget->Move();
		widget = next;
	}
}

void InterfaceMgr::Render(void)
{
	List<Renderable>	renderList;

	int32 width = TheDisplayMgr->GetDisplayWidth();
	int32 height = TheDisplayMgr->GetDisplayHeight();

	interfaceCamera->SetOrthoRect(0.0F, (float) width, (float) height, 0.0F);
	interfaceCamera->SetViewRect(Rect(0, 0, width, height));

	if (!TheWorldMgr->GetWorld())
	{
		interfaceCamera->SetClearFlags(kClearColorBuffer);
		TheGraphicsMgr->SetRenderTarget(kRenderTargetDisplay);
	}

	if (TheGraphicsMgr->GetGraphicsActiveFlags() & kGraphicsActiveTimer)
	{
		TheGraphicsMgr->Timestamp(kTimestampBeginGui);
	}

	TheGraphicsMgr->SetCamera(interfaceCamera, &cameraTransformable);
	interfaceCamera->SetClearFlags(0);

	rootWidget->Update();
	windowRoot->Update();
	stripBoard->Update();

	rootWidget->RenderTree(&renderList);
	windowRoot->RenderTree(&renderList);

	Widget *menu = activeMenu;
	if (menu)
	{
		menu->Update();
		menu->RenderTree(&renderList);
	}

	stripBoard->RenderTree(&renderList);

	#if !C4MOBILE

		if (cursorVisible)
		{
			currentCursor->SetWorldPosition(cursorPosition);
			renderList.Append(currentCursor);
		}

	#endif

	TheGraphicsMgr->Draw(&renderList);
	renderList.RemoveAll();

	if (TheGraphicsMgr->GetGraphicsActiveFlags() & kGraphicsActiveTimer)
	{
		TheGraphicsMgr->Timestamp(kTimestampEndGui);
	}
}

// ZYUQURM
