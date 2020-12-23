#ifndef C4Widgets_h
#define C4Widgets_h


//# \component	Interface Manager
//# \prefix		InterfaceMgr/


#include "C4Node.h"
#include "C4Fonts.h"
#include "C4Mutators.h"
#include "C4Renderable.h"


namespace C4
{
	//# \tree	Widget
	//
	//# \node	RenderableWidget
	//# \sub
	//#		\node	LineWidget
	//#		\node	BorderWidget
	//#		\node	TextWidget
	//#		\sub
	//#			\node	EditTextWidget
	//#			\sub
	//#				\node	PasswordWidget
	//#			\end
	//#			\node	CheckWidget
	//#			\node	RadioWidget
	//#			\node	PushButtonWidget
	//#			\node	TextButtonWidget
	//#			\sub
	//#				\node	HyperlinkWidget
	//#			\end
	//#			\node	PopupMenuWidget
	//#		\end
	//#		\node	QuadWidget
	//#		\sub
	//#			\node	ImageWidget
	//#			\sub
	//#				\node	IconButtonWidget
	//#				\node	PaintWidget
	//#				\node	EffectMgr/CameraWidget
	//#				\node	BrowserPlugin/BrowserWidget
	//#			\end
	//#			\node	MovieMgr/MovieWidget
	//#		\end
	//#		\node	ColorWidget
	//#		\node	ProgressWidget
	//#		\node	SliderWidget
	//#		\node	ScrollWidget
	//#		\node	ListWidget
	//#		\sub
	//#			\node	TreeWidget
	//#		\end
	//#		\node	TreeItemWidget
	//#		\node	TableWidget
	//#		\node	MultipaneWidget
	//#		\node	DividerWidget
	//#		\node	MenuBarWidget
	//#		\node	ConfigurationWidget
	//# \end
	//
	//# \node	ViewportWidget
	//# \sub
	//#		\node	OrthoViewportWidget
	//#		\node	FrustumViewportWidget
	//#		\sub
	//#			\node	WorldViewportWidget
	//#		\end
	//# \end
	//
	//# \node	RootWidget
	//# \sub
	//#		\node	Panel
	//#		\node	Board
	//#		\node	Window
	//#		\sub
	//#			\node	Dialog
	//#			\node	ColorPicker
	//#			\node	FilePicker
	//#		\end
	//#		\node	Page
	//#		\node	Menu
	//# \end


	enum
	{
		kMaxWidgetKeyLength		= 15
	};


	typedef Type	WidgetType;
	typedef Type	BalloonType;
	typedef Type	WidgetPart;
	typedef Type	TextFormatTag;


	typedef String<kMaxWidgetKeyLength>		WidgetKey;


	enum : WidgetType
	{
		kWidgetGeneric				= 0,
		kWidgetRoot					= 'ROOT',
		kWidgetPanel				= 'PANL',
		kWidgetBoard				= 'BOAR',
		kWidgetWindow				= 'WIND',
		kWidgetBalloon				= 'BLLN',
		kWidgetPage					= 'PAGE',
		kWidgetBook					= 'BOOK',
		kWidgetLine					= 'LINE',
		kWidgetBorder				= 'BRDR',
		kWidgetText					= 'TEXT',
		kWidgetEditText				= 'EDIT',
		kWidgetPassword				= 'PASS',
		kWidgetQuad					= 'QUAD',
		kWidgetImage				= 'IMAG',
		kWidgetCheck				= 'CHCK',
		kWidgetRadio				= 'RDIO',
		kWidgetGuiButton			= 'BTTN',
		kWidgetPushButton			= 'PUSH',
		kWidgetIconButton			= 'ICON',
		kWidgetTextButton			= 'TBUT',
		kWidgetHyperlink			= 'LINK',
		kWidgetColor				= 'COLR',
		kWidgetProgress				= 'PROG',
		kWidgetSlider				= 'SLID',
		kWidgetScroll				= 'SCRL',
		kWidgetList					= 'LIST',
		kWidgetTree					= 'TREE',
		kWidgetTreeItem				= 'TRIT',
		kWidgetTable				= 'TABL',
		kWidgetMultipane			= 'MPAN',
		kWidgetDivider				= 'DVDR',
		kWidgetWindowButton			= 'WBUT',
		kWidgetWindowFrame			= 'WFRM',
		kWidgetPageFrame			= 'PFRM',
		kWidgetBalloonFrame			= 'BFRM',
		kWidgetStrip				= 'STRP'
	};


	enum : WidgetPart
	{
		kWidgetPartNone				= 0,
		kWidgetPartInterior			= 'INTR',
		kWidgetPartIndicator		= 'INDC',
		kWidgetPartSlider			= 'SLID',
		kWidgetPartUpButton			= 'UPBT',
		kWidgetPartDownButton		= 'DNBT',
		kWidgetPartPageUp			= 'PGUP',
		kWidgetPartPageDown			= 'PGDN',
		kWidgetPartTitle			= 'TITL',
		kWidgetPartResize			= 'RSIZ'
	};


	//# \enum	WidgetRegistrationFlags

	enum
	{
		kWidgetPanelOnly			= 1 << 0		//## The widget can only appear in a panel effect.
	};


	//# \enum	WidgetState

	enum
	{
		kWidgetDisabled				= 1 << 0,		//## The widget is disabled and does not receive events.
		kWidgetHidden				= 1 << 1,		//## The widget is hidden and is not rendered.
		kWidgetHilited				= 1 << 2,		//## The widget is in the highlighted state.
		kWidgetCollapsed			= 1 << 3,		//## The widget is in the collapsed state.
		kWidgetBackground			= 1 << 4,		//## The widget is in the background state.
		kWidgetSelected				= 1 << 5,		//## The widget is selected in a list or table.
		kWidgetInactive				= 1 << 6,		//## The widget is inactive in a list or table.
		kWidgetFocus				= 1 << 7,		//## The widget currently has the keyboard focus.
		kWidgetNonpersistent		= 1 << 31
	};


	//# \enum	WidgetUsage

	enum
	{
		kWidgetKeyboardFocus		= 1 << 0,		//## The widget is capable of acquiring the keyboard focus.
		kWidgetMouseWheel			= 1 << 1,		//## The widget can handle mouse wheel events.
		kWidgetMultiaxisMouse		= 1 << 2,		//## The widget can handle multiaxis mouse events.
		kWidgetTrackInhibit			= 1 << 3,		//## The widget is not tracked after any mouse down event.
		kWidgetTrackRightMouse		= 1 << 4,		//## The widget is tracked after a right mouse button click. (Ignored if $kWidgetTrackInhibit$ is set.)
		kWidgetTrackMiddleMouse		= 1 << 5,		//## The widget is tracked after a middle mouse button click. (Ignored if $kWidgetTrackInhibit$ is set.)
		kWidgetGeneratedImage		= 1 << 6		//## The primary texture map of an $@ImageWidget@$ is a generated texture, and the name of any texture resource should not be saved.
	};


	//# \enum	WidgetColorType

	enum : WidgetColorType
	{
		kWidgetColorDefault			= 0,			//## The default color for the widget. This is usually equivalent to one of the other types of colors, but which one varies with the type of the widget. See the documentation for the individual $@Widget@$ subclasses for information about the default color.
		kWidgetColorText			= 'TEXT',		//## The color of any text shown by the widget.
		kWidgetColorLine			= 'LINE',		//## The color of any lines shown by the widget.
		kWidgetColorBorder			= 'BRDR',		//## The color of the widget's border.
		kWidgetColorBackground		= 'BKGD',		//## The color of the widget's background.
		kWidgetColorButton			= 'BTTN',		//## The color of any buttons belonging to the widget.
		kWidgetColorHilite			= 'HLIT',		//## The color used to highlight parts of the widget.
		kWidgetColorFocus			= 'FOCS',		//## The color used for the widget's focus glow.
		kWidgetColorCaret			= 'CART'		//## The color of the insertion caret.
	};


	enum
	{
		kWidgetOverrideLineColor	= 1 << 0,
		kWidgetOverrideButtonColor	= 1 << 1,
		kWidgetOverrideHiliteColor	= 1 << 2,
		kWidgetOverrideFocusColor	= 1 << 3
	};


	enum
	{
		kWidgetAnchorNone			= 0x00,
		kWidgetAnchorLeft			= 0x01,
		kWidgetAnchorTop			= 0x01,
		kWidgetAnchorRight			= 0x02,
		kWidgetAnchorBottom			= 0x02,
		kWidgetAnchorCenter			= 0x03
	};


	//# \enum	LineStyle

	enum
	{
		kLineSolid,				//## A solid line.
		kLineDotted1,			//## A dotted line with one-pixel-wide dots.
		kLineDotted2,			//## A dotted line with two-pixel-wide dots.
		kLineDashed,			//## A dashed line.
		kLineStyleCount
	};


	//# \enum	TextFlags

	enum
	{
		kTextUnformatted			= 1 << 0,		//## Formatting tags in the text should not be applied, but rendered as ordinary text.
		kTextWrapped				= 1 << 1,		//## The text should be rendered as multiple lines within the text box specified by the $@Widget::SetWidgetSize@$ function.
		kTextClipped				= 1 << 2		//## The text should be clipped against the left and right edges of the text box. This flag has no effect if the $kTextWrapped$ flag is set.
	};


	//# \enum	TextFormatExclusionMask

	enum
	{
		kTextFormatColor			= 1 << 0,		//## Ignore color formatting tags.
		kTextFormatAlignment		= 1 << 1,		//## Ignore alignment formatting tags.
		kTextFormatWholeState		= 1 << 2
	};


	//# \enum	TextAlignment

	enum TextAlignment
	{
		kTextAlignLeft,			//## Align text left.
		kTextAlignCenter,		//## Align text center.
		kTextAlignRight			//## Align text right.
	};


	//# \enum	EditTextFlags

	enum
	{
		kEditTextOverflow				= 1 << 0,		//## Allow the entered text to overflow the available width of the edit box.
		kEditTextMultiline				= 1 << 1,		//## Display a multiline edit box.
		kEditTextReadOnly				= 1 << 2,		//## The edit box is read-only. Text can be selected and copied, but it cannot be modified by the user.
		kEditTextChangeSelectAll		= 1 << 3,		//## If the text is changed by the application when the widget has the focus, then automatically select all.
		kEditTextDoubleClickSelectAll	= 1 << 4,		//## A double-click always causes the entire string of text to be selected.
		kEditTextRenderPlain			= 1 << 5,		//## The edit box background, border, and focus glow are not rendered.
		kEditTextFocusPlain				= 1 << 6,		//## The appearance of the edit box is not augmented when it has the focus.
		kEditTextEditClear				= 1 << 7		//## If the user makes any edit to the text, then the entire contents of the edit box are first cleared. This flag is also automatically cleared when an edit occurs in order to enable further edits.
	};


	//# \enum	CheckFlags

	enum
	{
		kCheckUseHighlightColor			= 1 << 0,		//## The check box uses the highlight color when it is selected.
		kCheckTextInactive				= 1 << 1		//## Only the check box itself can be clicked to change the value, not the text.
	};


	//# \enum	RadioFlags

	enum
	{
		kRadioUseHighlightColor			= 1 << 0,		//## The radio button uses the highlight color when it is selected.
		kRadioTextInactive				= 1 << 1		//## Only the radio button itself can be clicked to change the value, not the text.
	};


	//# \enum	PushButtonFlags

	enum
	{
		kPushButtonPrimary				= 1 << 0		//## The push button should have the appearance of the primary button in a window.
	};


	//# \enum	ActivationKey

	enum : Type
	{
		kActivationKeyNone				= 0,
		kActivationKeyEnter				= 'ENTR',
		kActivationKeyEscape			= 'ESCP'
	};


	//# \enum	IconButtonFlags

	enum
	{
		kIconButtonSticky				= 1 << 0		//## The icon button stays selected when clicked.
	};


	//# \enum	SliderStyle

	enum
	{
		kSliderRound,			//## Use a round indicator.
		kSliderSquare,			//## Use a square indicator.
		kSliderStyleCount
	};


	//# \enum	ScrollFlags

	enum
	{
		kScrollHorizontal				= 1 << 0		//## Render buttons for a horizontal scroll bar.
	};


	//# \enum	ListFlags

	enum
	{
		kListMultipleSelection			= 1 << 0,		//## Allow multiple items to be selected in a list simultaneously.
		kListFocusPlain					= 1 << 1,		//## The appearance of the list is not augmented when it has the focus.
		kListItemsEnabled				= 1 << 2		//## The items in the list are left enabled to allow user interaction.
	};


	//# \enum	TableFlags

	enum
	{
		kTableMultipleSelection			= 1 << 0,		//## Allow multiple items to be selected in a table simultaneously.
		kTableFocusPlain				= 1 << 1,		//## The appearance of the table is not augmented when it has the focus.
		kTableItemsEnabled				= 1 << 2		//## The items in the table are left enabled to allow user interaction.
	};


	//# \enum	BalloonType

	enum : BalloonType
	{
		kBalloonNone					= 0,			//## The widget does not have a balloon.
		kBalloonText					= 'TEXT',		//## The balloon string contains the text to be displayed.
		kBalloonResource				= 'RSRC'		//## The balloon string contains the name of the panel resource to be loaded.
	};


	//# \enum	WindowFlags

	enum
	{
		kWindowPlain					= 1 << 0,		//## The window is rendered plain with no frame.
		kWindowCloseBox					= 1 << 1,		//## The window has a close button. (Ignored if $kWindowPlain$ is set.)
		kWindowResizable				= 1 << 2,		//## The window has a resize handle. (Ignored if $kWindowPlain$ is set.)
		kWindowBackground				= 1 << 3,		//## The window has a filled background.
		kWindowCenter					= 1 << 4,		//## The window is initially centered on the screen. (Ignored if either $kWindowFullHorizontal$ or $kWindowFullVertical$ is set.)
		kWindowMaximize					= 1 << 5,		//## The window is initially as large as possible. (The specified size is ignored.)
		kWindowEvenSize					= 1 << 6,		//## The window must have even width and height. (Only used if $kWindowResizable$ is set.)
		kWindowStrip					= 1 << 7,		//## The window appears in the strip.
		kWindowPassive					= 1 << 8,		//## The window does not need to receive keyboard input.
		kWindowModal					= 1 << 9,		//## The window is modal. (Automatically set by the $@Window::AddSubwindow@$ function.)
		kWindowFullHorizontal			= 1 << 10,		//## The window is scaled so that it fills the display horizontally, and it is centered vertically.
		kWindowFullVertical				= 1 << 11,		//## The window is scaled so that it fills the display vertically, and it is centered horizontally. (Ignored if $kWindowFullHorizontal$ is set.)
		kWindowEditor					= 1 << 31
	};


	enum
	{
		kWidgetValueIndeterminant		= -1,
		kWidgetValueNone				= -2
	};


	enum : EventType
	{
		kEventWidgetActivate			= 'WGAC',
		kEventWidgetChange				= 'WGCH',
		kEventWidgetBeginHover			= 'WGBH',
		kEventWidgetEndHover			= 'WGEH'
	};


	enum
	{
		kWidgetManipulatorSelected		= 1 << 0,
		kWidgetManipulatorHidden		= 1 << 1,
		kWidgetManipulatorBaseState		= 1 << 2
	};


	class Widget;
	class TreeWidget;
	class BookWidget;
	class RootWidget;
	class Window;
	class ColorPicker;
	struct PanelResourceHeader;
	class PanelController;
	class ScriptObject;
	class Player;


	//# \struct	WidgetEventData		Contains information about a widget event.
	//
	//# The $WidgetEventData$ structure contains information about a widget event.
	//
	//# \def	struct WidgetEventData
	//
	//# \ctor	WidgetEventData(EventType type, Node *initiator = nullptr);
	//
	//# \param	type		The initial value of the $eventType$ field.
	//# \param	initiator	The initial value of the $initiatorNode$ field.
	//
	//# \data	WidgetEventData


	//# \member		WidgetEventData

	struct WidgetEventData
	{
		EventType		eventType;			//## The type of the event.
		Node			*initiatorNode;		//## The node that caused the event to occur (in-game panel effects only).

		WidgetEventData() = default;

		WidgetEventData(EventType type, Node *initiator = nullptr)
		{
			eventType = type;
			initiatorNode = initiator;
		}
	};


	//# \struct	PanelMouseEventData		Contains information about a mouse event in a panel.
	//
	//# The $PanelMouseEventData$ structure contains information about a mouse event in a panel.
	//
	//# \def	struct PanelMouseEventData : MouseEventData
	//
	//# \data	PanelMouseEventData
	//
	//# \base	Utilities/MouseEventData	The $WidgetEventData$ structure is an extension of $MouseEventData$.


	//# \member		PanelMouseEventData

	struct PanelMouseEventData : MouseEventData
	{
		WidgetPart		widgetPart;			//## The widget part involved in the event.
		Node			*initiatorNode;		//## The node that caused the event to occur (in-game panel effects only).
	};


	struct TextFormatState
	{
		TextAlignment	textAlignment;
		Color4C			textColor;

		TextFormatState& Copy(const TextFormatState& format)
		{
			textColor = format.textColor;
			return (*this);
		}
	};


	//# \class	WidgetRegistration		Manages internal registration information for a custom widget type.
	//
	//# The $WidgetRegistration$ class manages internal registration information for a custom widget type.
	//
	//# \def	class WidgetRegistration : public Registration<Widget, WidgetRegistration>
	//
	//# \ctor	WidgetRegistration(WidgetType type, const char *name, const char *icon, unsigned_int32 flags);
	//
	//# \param	type		The widget type.
	//# \param	name		The widget name.
	//# \param	icon		The resource name for the icon texture used to represent the widget in the Panel Editor.
	//# \param	flags		The widget registration flags.
	//
	//# \desc
	//# The $WidgetRegistration$ class is abstract and serves as the common base class for the template class
	//# $@WidgetReg@$. A custom widget is registered with the engine by instantiating an object of type
	//# $WidgetReg<classType>$, where $classType$ is the type of the widget subclass being registered.
	//
	//# \base	System/Registration<Widget, WidgetRegistration>		A widget registration is a specific type of registration object.
	//
	//# \also	$@WidgetReg@$
	//# \also	$@Widget@$


	//# \function	WidgetRegistration::GetWidgetType		Returns the registered widget type.
	//
	//# \proto	WidgetType GetWidgetType(void) const;
	//
	//# \desc
	//# The $GetWidgetType$ function returns the widget type for a particular widget registration.
	//# The widget type is established when the widget registration is constructed.
	//
	//# \also	$@WidgetRegistration::GetWidgetName@$
	//# \also	$@WidgetRegistration::GetIconTextureName@$


	//# \function	WidgetRegistration::GetWidgetFlags		Returns the widget registration flags.
	//
	//# \proto	unsigned_int32 GetWidgetFlags(void) const;
	//
	//# \desc
	//# The $GetWidgetFlags$ function returns the flags that were assigned to the widget type
	//# when the widget registration was created. The flags can be a combination (through logical
	//# OR) of the following constants.
	//
	//# \table	WidgetRegistrationFlags
	//
	//# \also	$@WidgetRegistration::GetWidgetType@$
	//# \also	$@WidgetRegistration::GetWidgetName@$


	//# \function	WidgetRegistration::GetWidgetName		Returns the human-readable widget name.
	//
	//# \proto	const char *GetWidgetName(void) const;
	//
	//# \desc
	//# The $GetWidgetName$ function returns the human-readable widget name for a particular widget registration.
	//# The widget name is established when the widget registration is constructed.
	//
	//# \also	$@WidgetRegistration::GetWidgetType@$
	//# \also	$@WidgetRegistration::GetIconTextureName@$


	//# \function	WidgetRegistration::GetIconTextureName		Returns the icon texture name for a widget.
	//
	//# \proto	const char *GetIconTextureName(void) const;
	//
	//# \desc
	//# The $GetIconTextureName$ function returns the icon texture name for a particular widget registration.
	//# The icon texture name is established when the widget registration is constructed.
	//
	//# \also	$@WidgetRegistration::GetWidgetType@$
	//# \also	$@WidgetRegistration::GetWidgetName@$


	class WidgetRegistration : public Registration<Widget, WidgetRegistration>
	{
		private:

			unsigned_int32	widgetFlags;

			const char		*widgetName;
			const char		*iconTextureName;

		public:

			C4API WidgetRegistration(WidgetType type, const char *name, const char *icon, unsigned_int32 flags);
			C4API ~WidgetRegistration();

			WidgetType GetWidgetType(void) const
			{
				return (GetRegistrableType());
			}

			unsigned_int32 GetWidgetFlags(void) const
			{
				return (widgetFlags);
			}

			const char *GetWidgetName(void) const
			{
				return (widgetName);
			}

			const char *GetIconTextureName(void) const
			{
				return (iconTextureName);
			}
	};


	//# \class	WidgetReg	 Represents a custom widget type.
	//
	//# The $WidgetReg$ class represents a custom widget type.
	//
	//# \def	template <class classType> class WidgetReg : public WidgetRegistration
	//
	//# \tparam	classType	The custom widget class.
	//
	//# \ctor	WidgetReg(WidgetType type, const char *name, const char *icon, unsigned_int32 flags = 0);
	//
	//# \param	type		The widget type.
	//# \param	name		The widget name.
	//# \param	icon		The resource name for the icon texture used to represent the widget in the Panel Editor.
	//# \param	flags		The widget registration flags.
	//
	//# \desc
	//# The $WidgetReg$ template class is used to advertise the existence of a custom widget type.
	//# The Interface Manager uses a widget registration to construct a custom widget. The act of instantiating a
	//# $WidgetReg$ object automatically registers the corresponding widget type. The widget type is unregistered
	//# when the $WidgetReg$ object is destroyed.
	//#
	//# The $flags$ parameter is optional and assigns special properties to the widget registration.
	//# It can be a combination (through logical OR) of the following constants.
	//
	//# \table	WidgetRegistrationFlags
	//
	//# No more than one widget registration should be created for each distinct widget type.
	//
	//# \base	WidgetRegistration		All specific widget registration classes share the common base class $WidgetRegistration$.
	//
	//# \also	$@Widget@$


	template <class classType> class WidgetReg : public WidgetRegistration
	{
		public:

			WidgetReg(WidgetType type, const char *name, const char *icon, unsigned_int32 flags = 0) : WidgetRegistration(type, name, icon, flags)
			{
			}

			Widget *Create(void) const
			{
				return (new classType);
			}
	};


	class WidgetManipulator
	{
		private:

			Widget				*targetWidget;
			unsigned_int32		manipulatorState;

		protected:

			C4API WidgetManipulator(Widget *widget);

		public:

			C4API virtual ~WidgetManipulator();

			Widget *GetTargetWidget(void) const
			{
				return (targetWidget);
			}

			unsigned_int32 GetManipulatorState(void) const
			{
				return (manipulatorState);
			}

			void SetManipulatorState(unsigned_int32 state)
			{
				manipulatorState = state;
			}

			bool Selected(void) const
			{
				return ((manipulatorState & kWidgetManipulatorSelected) != 0);
			}

			bool Hidden(void) const
			{
				return ((manipulatorState & kWidgetManipulatorHidden) != 0);
			}

			C4API virtual void Invalidate(void);
	};


	//# \class	Widget		Represents a widget in a user interface.
	//
	//# The $Widget$ class represents a widget in a user interface.
	//
	//# \def	class Widget : public Tree<Widget>, public ListElement<Widget>, public HashTableElement<Widget>,
	//# \def2	public Transformable, public ExclusiveObservable<Widget, const WidgetEventData *>, public LinkTarget<Widget>,
	//# \def2	public Packable, public Configurable, public Registrable<Widget, WidgetRegistration>
	//
	//# \ctor	Widget(WidgetType type, const Vector2D& size);
	//# \ctor	Widget(WidgetType type = kWidgetGeneric);
	//
	//# \param	type	The widget type.
	//# \param	size	The size of the widget, in pixels.
	//
	//# \desc
	//# The $Widget$ class is the base class for all user interface widgets. It contains information about the widget's identification,
	//# the general state of the widget, the widget's transform and size, the type of help balloon attached to the widget, and the
	//# list of mutators that may be dynamically animating the widget.
	//#
	//# Most types of widgets are subclasses of the $@RenderableWidget@$ class, which is a direct subclass of the $Widget$ class,
	//# because it contains additional functionality needed for widgets that are actually displayed. Some kinds of widgets are only
	//# containers for other widgets, however, and they are subclassed directly from the $Widget$ class. An instance of the $Widget$
	//# class can act as a group for other widgets by constructing a $Widget$ object with the default $kWidgetGeneric$ specified for
	//# the $type$ parameter.
	//#
	//# When the user interacts with a widget through the mouse or keyboard, the widget receives calls to its $@Widget::HandleMouseEvent@$
	//# and $@Widget::HandleKeyboardEvent@$ functions. A widget subclass overrides these functions and handles the events appropriately.
	//# When the widget needs to inform an observer that the state of the widget has changed in some way or that the user has activated
	//# the widget, it posts a widget event using the $@Widget::PostWidgetEvent@$ function. Any observer installed by the
	//# $@Utilities/ExclusiveObservable::SetObserver@$ function then receives the widget event and may take whatever action is necessary.
	//
	//# \base	Utilities/Tree<Widget>											Widgets are organized in a tree hierarchy.
	//# \base	Utilities/ListElement<Widget>									Used internally by the Interface Manager.
	//# \base	Utilities/HashTableElement<Widget>								Widgets with nontrivial keys are organized in a hash table.
	//# \base	Utilities/Transformable											Holds the object-to-world transform for a widget.
	//# \base	Utilities/ExclusiveObservable<Widget, const WidgetEventData *>	Widgets can be observed for state changes by a single observer.
	//# \base	Utilities/LinkTarget<Widget>									Used internally by the Interface Manager.
	//# \base	ResourceMgr/Packable											Widgets can be packed for storage in resources.
	//# \base	InterfaceMgr/Configurable										Widgets can define configurable parameters that are exposed
	//#																			as user interface widgets in the Panel Editor.
	//# \base	System/Registrable<Widget, WidgetRegistration>					Custom widget types can be registered with the engine.
	//
	//# \also	$@WidgetReg@$
	//# \also	$@EffectMgr/PanelEffect@$
	//# \also	$@EffectMgr/PanelController@$
	//# \also	$@Mutator@$
	//
	//# \wiki	Panel_Editor	Panel Editor
	//# \wiki	Widgets


	//# \function	Widget::GetWidgetType		Returns the widget type.
	//
	//# \proto	WidgetType GetWidgetType(void) const;
	//
	//# \desc
	//# The $GetWidgetType$ function returns the widget type.


	//# \function	Widget::GetWidgetKey		Returns the widget key.
	//
	//# \proto	const WidgetKey& GetWidgetKey(void) const;
	//
	//# \desc
	//# The $GetWidgetKey$ function returns the widget key. The widget key is a string having up to 15
	//# single-byte characters that can be used to identify one or more widgets in a panel. (The widget
	//# key does not need to be unique.) The initial key for a widget is the empty string.
	//
	//# \also	$@Widget::SetWidgetKey@$
	//# \also	$@Widget::GetNextWidgetWithSameKey@$
	//# \also	$@RootWidget::FindWidget@$


	//# \function	Widget::SetWidgetKey		Sets the widget key.
	//
	//# \proto	void SetWidgetKey(const char *key);
	//
	//# \param	key		The new widget key. This is a string up to 15 bytes in length, not counting the null terminator.
	//
	//# \desc
	//# The $SetWidgetKey$ function sets the widget key to the string specified by the $key$ parameter.
	//# The widget key is a string having up to 15 single-byte characters that can be used to identify
	//# one or more widgets in a panel. (The widget key does not need to be unique.) The initial key for
	//# a widget is the empty string.
	//
	//# \also	$@Widget::GetWidgetKey@$
	//# \also	$@Widget::GetNextWidgetWithSameKey@$
	//# \also	$@RootWidget::FindWidget@$


	//# \function	Widget::GetNextWidgetWithSameKey	Returns the next widget having the same key.
	//
	//# \proto	Widget *GetNextWidgetWithSameKey(void) const;
	//
	//# \desc
	//# The $GetNextWidgetWithSameKey$ function returns a pointer to the next widget having the same key as the
	//# widget for which this function is called. If there are no more widgets having the same key, then the
	//# return value is $nullptr$. This function is typically called iteratively after a call to the
	//# $@RootWidget::FindWidget@$ function to get the first widget having a particular key.
	//#
	//# If the widget for which the $GetNextWidgetWithSameKey$ function is called has the empty string for its key,
	//# then the return value is always $nullptr$, not the next widget having the empty string for its key.
	//
	//# \also	$@Widget::GetWidgetKey@$
	//# \also	$@Widget::SetWidgetKey@$
	//# \also	$@RootWidget::FindWidget@$


	//# \div
	//# \function	Widget::GetWidgetState		Returns the widget state.
	//
	//# \proto	unsigned_int32 GetWidgetState(void) const;
	//
	//# \desc
	//# The $GetWidgetState$ function returns the widget state, which can be a combination (through
	//# logical OR) of the following constants.
	//
	//# \table	WidgetState
	//
	//# A widget may be implicitly disabled or hidden due to one of its predecessors in the widget tree having
	//# the disabled or hidden state. The $@Widget::GetGlobalWidgetState@$ function can be used to quickly
	//# determine whether this is the case.
	//
	//# \also	$@Widget::SetWidgetState@$
	//# \also	$@Widget::GetGlobalWidgetState@$
	//# \also	$@Widget::Enabled@$
	//# \also	$@Widget::Visible@$


	//# \function	Widget::SetWidgetState		Sets the widget state.
	//
	//# \proto	virtual void SetWidgetState(unsigned_int32 state);
	//
	//# \param	state	The new widget state.
	//
	//# \desc
	//# The $SetWidgetState$ function sets the widget state to the value specified by the $state$ parameter,
	//# which can be a combination (through logical OR) of the following constants.
	//
	//# \table	WidgetState
	//
	//# The widget state should not normally be set directly, except by subclasses of $Widget$. Instead, functions
	//# such as $@Widget::Enable@$, $@Widget::Disable@$, $@Widget::Show@$, and $@Widget::Hide@$ that set the state
	//# implicitly should be called.
	//
	//# \also	$@Widget::Enable@$
	//# \also	$@Widget::Disable@$
	//# \also	$@Widget::Show@$
	//# \also	$@Widget::Hide@$
	//# \also	$@Widget::GetWidgetState@$
	//# \also	$@Widget::GetGlobalWidgetState@$
	//# \also	$@Widget::Enabled@$
	//# \also	$@Widget::Visible@$


	//# \function	Widget::GetGlobalWidgetState	Returns the global widget state.
	//
	//# \proto	unsigned_int32 GetGlobalWidgetState(void) const;
	//
	//# \desc
	//# The $GetGlobalWidgetState$ function returns the global widget state, which can be a combination (through
	//# logical OR) of the following constants.
	//
	//# \table	WidgetState
	//
	//# The global widget state is calculated by taking the logical OR of the states of the widget for which the
	//# $GetGlobalWidgetState$ function is called and all of its predecessors in the widget tree. This is useful
	//# for determining whether a widget is hidden or disabled either by itself or due to one of its predecessors
	//# being hidden or disabled.
	//
	//# \also	$@Widget::GetWidgetState@$
	//# \also	$@Widget::SetWidgetState@$


	//# \function	Widget::Enabled		Returns a boolean value indicating whether a widget is enabled.
	//
	//# \proto	bool Enabled(void) const;
	//
	//# \desc
	//# The $Enabled$ function returns $true$ if the widget is enabled and $false$ if it is disabled.
	//# Calling the $Enabled$ function is equivalent to calling the $@Widget::GetWidgetState@$ function and
	//# testing that the $kWidgetDisabled$ state is not set.
	//#
	//# A widget that is disabled does not receive input events, but it is still rendered, sometimes in a
	//# dimmed appearance depending on the type of the widget.
	//#
	//# A widget may be implicitly disabled due to one of its predecessors in the widget tree having the
	//# disabled state. In this case, the $Enabled$ function can still return $true$ if the widget itself
	//# is not disabled, but the widget will still be unable to receive input events. You can quickly
	//# determine whether a widget is either explicitly or implicitly disabled by calling the
	//# $@Widget::GetGlobalWidgetState@$ function and testing the $kWidgetDisabled$ state.
	//
	//# \also	$@Widget::Enable@$
	//# \also	$@Widget::Disable@$
	//# \also	$@Widget::GetWidgetState@$
	//# \also	$@Widget::SetWidgetState@$
	//# \also	$@Widget::Visible@$


	//# \function	Widget::Visible		Returns a boolean value indicating whether a widget is visible.
	//
	//# \proto	bool Visible(void) const;
	//
	//# \desc
	//# The $Visible$ function returns $true$ if the widget is visible and $false$ if it is hidden.
	//# Calling the $Visible$ function is equivalent to calling the $@Widget::GetWidgetState@$ function and
	//# testing that the $kWidgetHidden$ state is not set.
	//#
	//# A widget that is hidden is not rendered and does not receive input events.
	//#
	//# A widget may be implicitly hidden due to one of its predecessors in the widget tree having the
	//# hidden state. In this case, the $Visible$ function can still return $true$ if the widget itself
	//# is not hidden, but the widget will still be invisible. You can quickly determine whether a widget
	//# is either explicitly or implicitly hidden by calling the $@Widget::GetGlobalWidgetState@$ function
	//# and testing the $kWidgetHidden$ state.
	//
	//# \also	$@Widget::Show@$
	//# \also	$@Widget::Hide@$
	//# \also	$@Widget::GetWidgetState@$
	//# \also	$@Widget::SetWidgetState@$
	//# \also	$@Widget::Enabled@$


	//# \function	Widget::Enable		Enables a widget.
	//
	//# \proto	virtual void Enable(void);
	//
	//# \desc
	//# The $Enable$ function enables a widget so that it can receive input events.
	//#
	//# Widgets are enabled by default.
	//
	//# \also	$@Widget::Disable@$
	//# \also	$@Widget::Enabled@$
	//# \also	$@Widget::GetWidgetState@$
	//# \also	$@Widget::SetWidgetState@$
	//# \also	$@Widget::Show@$
	//# \also	$@Widget::Hide@$


	//# \function	Widget::Disable		Disables a widget.
	//
	//# \proto	virtual void Disable(void);
	//
	//# \desc
	//# The $Disable$ function disables a widget, preventing it from receiving input events. The widget is
	//# still rendered, but possibly in a dimmed appearance depending on the type of the widget. Subnodes of
	//# a disabled widget are also considered to be disabled and do not receive input events.
	//
	//# \also	$@Widget::Enable@$
	//# \also	$@Widget::Enabled@$
	//# \also	$@Widget::GetWidgetState@$
	//# \also	$@Widget::SetWidgetState@$
	//# \also	$@Widget::Show@$
	//# \also	$@Widget::Hide@$


	//# \function	Widget::Show		Shows a widget.
	//
	//# \proto	virtual void Show(void);
	//
	//# \desc
	//# The $Show$ function makes a widget visible so that it is rendered.
	//#
	//# Widgets are visible by default.
	//
	//# \also	$@Widget::Hide@$
	//# \also	$@Widget::Visible@$
	//# \also	$@Widget::GetWidgetState@$
	//# \also	$@Widget::SetWidgetState@$
	//# \also	$@Widget::Enable@$
	//# \also	$@Widget::Disable@$


	//# \function	Widget::Hide		Hides a widget.
	//
	//# \proto	virtual void Hide(void);
	//
	//# \desc
	//# The $Hide$ function makes a widget invisible, preventing it from being rendered.
	//# Subnodes of a hidden widget are also considered to be hidden and are not rendered.
	//
	//# \also	$@Widget::Show@$
	//# \also	$@Widget::Visible@$
	//# \also	$@Widget::GetWidgetState@$
	//# \also	$@Widget::SetWidgetState@$
	//# \also	$@Widget::Enable@$
	//# \also	$@Widget::Disable@$


	//# \div
	//# \function	Widget::GetWidgetSize		Returns the widget size.
	//
	//# \proto	const Vector2D& GetWidgetSize(void) const;
	//
	//# \desc
	//# The $GetWidgetSize$ function returns the widget size, in pixels.
	//
	//# \also	$@Widget::SetWidgetSize@$


	//# \function	Widget::SetWidgetSize		Sets the widget width and height.
	//
	//# \proto	virtual void SetWidgetSize(const Vector2D& size);
	//
	//# \param	size	The new widget size, in pixels.
	//
	//# \desc
	//# The $SetWidgetSize$ function sets the size of a widget. The <i>x</i> and <i>y</y> components of
	//# the $size$ parameter specify the width and height of the widget in pixels.
	//
	//# \also	$@Widget::GetWidgetSize@$


	//# \function	Widget::GetWidgetTransform		Returns a widget's transform.
	//
	//# \proto	const Transform4D& GetWidgetTransform(void) const;
	//
	//# \desc
	//# The $GetWidgetTransform$ function returns a widget's transform.
	//
	//# \also	$@Widget::SetWidgetTransform@$
	//# \also	$@Widget::GetWidgetPosition@$
	//# \also	$@Math/Transform4D@$


	//# \function	Widget::SetWidgetTransform		Sets a widget's transform.
	//
	//# \proto	void SetWidgetTransform(const Transform4D& transform);
	//# \proto	void SetWidgetTransform(const Matrix3D& matrix, const Point3D& position);
	//# \proto	void SetWidgetTransform(const Vector3D& c1, const Vector3D& c2, const Vector3D& c3, const Point3D& c4);
	//
	//# \param	transform	The new transform.
	//# \param	matrix		The new upper-left 3&times;3 portion of the transform.
	//# \param	position	The new position.
	//# \param	c1			The first column of the 4D transform.
	//# \param	c2			The second column of the 4D transform.
	//# \param	c3			The third column of the 4D transform.
	//# \param	c4			The fourth column of the 4D transform.
	//
	//# \desc
	//# The $SetWidgetTransform$ function sets a widget's transform.
	//
	//# \also	$@Widget::GetWidgetTransform@$
	//# \also	$@Widget::SetWidgetMatrix3D@$
	//# \also	$@Widget::SetWidgetPosition@$
	//# \also	$@Math/Transform4D@$


	//# \function	Widget::SetWidgetMatrix3D		Sets the upper-left 3&times;3 portion of a widget's transform.
	//
	//# \proto	void SetWidgetMatrix3D(const Matrix3D& matrix);
	//# \proto	void SetWidgetMatrix3D(const Vector3D& c1, const Vector3D& c2, const Vector3D& c3);
	//
	//# \param	matrix		The new upper-left 3&times;3 portion of the transform.
	//# \param	c1			The first column of the 3&times;3 matrix.
	//# \param	c2			The second column of the 3&times;3 matrix.
	//# \param	c3			The third column of the 3&times;3 matrix.
	//
	//# \desc
	//# The $SetWidgetMatrix3D$ function sets the upper-left 3&times;3 portion of a widget's transform without
	//# effecting the translation portion in the fourth column of the matrix.
	//
	//# \also	$@Widget::SetWidgetTransform@$
	//# \also	$@Widget::SetWidgetPosition@$


	//# \function	Widget::GetWidgetPosition		Returns a widget's position.
	//
	//# \proto	const Point3D& GetWidgetPosition(void) const;
	//
	//# \desc
	//# The $GetWidgetPosition$ function returns a widget's position.
	//
	//# \also	$@Widget::SetWidgetPosition@$
	//# \also	$@Widget::GetWidgetTransform@$
	//# \also	$@Math/Point3D@$


	//# \function	Widget::SetWidgetPosition		Sets a widget's position.
	//
	//# \proto	void SetWidgetPosition(const Point3D& position);
	//
	//# \param	position	The new position.
	//
	//# \desc
	//# The $SetWidgetPosition$ function sets a widget's position without affecting the rest of the widget's transform.
	//
	//# \also	$@Widget::GetWidgetPosition@$
	//# \also	$@Widget::SetWidgetTransform@$
	//# \also	$@Widget::SetWidgetMatrix3D@$


	//# \div
	//# \function	Widget::GetWidgetColor		Returns a widget color.
	//
	//# \proto	virtual const ColorRGBA& GetWidgetColor(WidgetColorType type = kWidgetColorDefault) const override;
	//
	//# \param	type	The type of the color to retrieve. See below for possible values.
	//
	//# \desc
	//# The $GetWidgetColor$ function returns the widget color corresponding to the type specified by the $type$ parameter.
	//# The type can be one of the following constants.
	//
	//# \table	WidgetColorType
	//
	//# If the widget does not support the color specified by the $type$ parameter, then the default color for the widget is returned.
	//
	//# \also	$@Widget::SetWidgetColor@$
	//# \also	$@Widget::SetWidgetAlpha@$
	//# \also	$@Math/ColorRGBA@$


	//# \function	Widget::SetWidgetColor		Sets a widget color.
	//
	//# \proto	virtual void SetWidgetColor(const ColorRGBA& color, WidgetColorType type = kWidgetColorDefault) override;
	//
	//# \param	color	The new widget color.
	//# \param	type	The type of the color to set. See below for possible values.
	//
	//# \desc
	//# The $SetWidgetColor$ function sets the widget color corresponding to the type specified by the $type$ parameter.
	//# The type can be one of the following constants.
	//
	//# \table	WidgetColorType
	//
	//# If the widget does not support the color specified by the $type$ parameter, then no change is made to any of the widget's colors.
	//
	//# \also	$@Widget::GetWidgetColor@$
	//# \also	$@Widget::SetWidgetAlpha@$
	//# \also	$@Math/ColorRGBA@$


	//# \function	Widget::SetWidgetAlpha		Sets the alpha channel of a widget color.
	//
	//# \proto	virtual void SetWidgetAlpha(float alpha, WidgetColorType type = kWidgetColorDefault) override;
	//
	//# \param	alpha	The new alpha value for a widget color.
	//# \param	type	The type of the color that is affected. See below for possible values.
	//
	//# \desc
	//# The $SetWidgetAlpha$ function sets the alpha channel of the widget color corresponding to the type specified by the $type$ parameter.
	//# The type can be one of the following constants.
	//
	//# \table	WidgetColorType
	//
	//# The red, green, and blue channels of the target color are not affected. If the widget does not support the color specified by the
	//# $type$ parameter, then no change is made to any of the widget's colors.
	//
	//# \also	$@Widget::GetWidgetColor@$
	//# \also	$@Widget::SetWidgetColor@$
	//# \also	$@Math/ColorRGBA@$


	//# \div
	//# \function	Widget::GetRootWidget		Returns the root widget in a tree.
	//
	//# \proto	RootWidget *GetRootWidget(void) const;
	//
	//# \desc
	//# The $GetRootWidget$ function returns a pointer to the root widget for the tree containing the widget for
	//# which this function is called. If there is no root widget, then the return value is $nullptr$.
	//
	//# \also	$@Widget::GetOwningWindow@$
	//# \also	$@RootWidget@$


	//# \function	Widget::GetOwningWindow		Returns the window to which a widget belongs.
	//
	//# \proto	Window *GetOwningWindow(void) const;
	//
	//# \desc
	//# The $GetOwningWindow$ function returns a pointer to the window to which a widget belongs. If the widget
	//# is not part of a window, then the return value is $nullptr$.
	//#
	//# Note that the $GetOwningWindow$ function does not necessarily return the same widget as the $@Widget::GetRootWidget@$
	//# function when a widget is in a window. It is possible for a window to contain other types of root widgets that
	//# contain widgets themselves.
	//
	//# \also	$@Widget::GetRootWidget@$
	//# \also	$@Window@$


	//# \function	Widget::GetPanelController		Returns the panel controller to which a widget belongs.
	//
	//# \proto	PanelController *GetPanelController(void) const;
	//
	//# \desc
	//# The $GetPanelController$ function returns the panel controller to which a widget belongs. If the widget
	//# is not part of an in-game panel effect, then the return value is $nullptr$.
	//
	//# \also	$@EffectMgr/PanelController@$


	//# \div
	//# \function	Widget::AddMutator		Adds a mutator to a widget.
	//
	//# \proto	void AddMutator(Mutator *mutator);
	//
	//# \param	mutator		The mutator to add to the widget.
	//
	//# \desc
	//# The $AddMutator$ function adds a mutator to a widget. A mutator can be removed from a widget
	//# by either deleting the $@Mutator@$ object or by calling the $@Widget::RemoveMutator@$ function.
	//
	//# \also	$@Widget::RemoveMutator@$
	//# \also	$@Widget::GetFirstMutator@$
	//# \also	$@Widget::GetLastMutator@$
	//# \also	$@Mutator@$


	//# \function	Widget::RemoveMutator	Removes a mutator from a widget.
	//
	//# \proto	void RemoveMutator(Mutator *mutator);
	//
	//# \param	mutator		The mutator to remove from the widget.
	//
	//# \desc
	//# The $RemoveMutator$ function removes a mutator from a widget. A mutator can be added to a widget
	//# by calling the $@Widget::AddMutator@$ function.
	//
	//# \also	$@Widget::AddMutator@$
	//# \also	$@Widget::GetFirstMutator@$
	//# \also	$@Widget::GetLastMutator@$
	//# \also	$@Mutator@$


	//# \function	Widget::GetFirstMutator		Returns the first mutator for a widget.
	//
	//# \proto	Mutator *GetFirstMutator(void) const;
	//
	//# \desc
	//# The $GetFirstMutator$ function returns the first mutator attached to a widget. The entire list of
	//# mutators can be iterated by calling the $@Utilities/ListElement::Next@$ function on the returned object.
	//# If a widget has no mutators, then the $GetFirstMutator$ returns $nullptr$.
	//
	//# \also	$@Widget::GetLastMutator@$
	//# \also	$@Widget::AddMutator@$
	//# \also	$@Widget::RemoveMutator@$
	//# \also	$@Mutator@$


	//# \function	Widget::GetLastMutator		Returns the last mutator for a widget.
	//
	//# \proto	Mutator *GetLastMutator(void) const;
	//
	//# \desc
	//# The $GetLastMutator$ function returns the last mutator attached to a widget. The entire list of
	//# mutators can be iterated by calling the $@Utilities/ListElement::Previous@$ function on the returned object.
	//# If a widget has no mutators, then the $GetLastMutator$ returns $nullptr$.
	//
	//# \also	$@Widget::GetFirstMutator@$
	//# \also	$@Widget::AddMutator@$
	//# \also	$@Widget::RemoveMutator@$
	//# \also	$@Mutator@$


	//# \div
	//# \function	Widget::GetBalloonType		Returns the balloon type for a widget.
	//
	//# \proto	BalloonType GetBalloonType(void) const;
	//
	//# \desc
	//# The $GetBalloonType$ function returns the balloon type for a widget, which can be one of the following constants.
	//
	//# \table	BalloonType
	//
	//# \also	$@Widget::GetBalloonString@$
	//# \also	$@Widget::SetBalloon@$


	//# \function	Widget::GetBalloonString	Returns the balloon string for a widget.
	//
	//# \proto	const char *GetBalloonString(void) const;
	//
	//# \desc
	//# The $GetBalloonString$ function returns the balloon string for a widget. The meaning of the balloon string
	//# depends on the widget's balloon type.
	//
	//# \also	$@Widget::GetBalloonType@$
	//# \also	$@Widget::SetBalloon@$


	//# \function	Widget::SetBalloon			Sets the balloon type and string for a widget.
	//
	//# \proto	void SetBalloon(BalloonType type, const char *string = "");
	//
	//# \param	type	The balloon type. See below for possible values.
	//# \param	string	The balloon string. The default empty string should only be used if the $type$ parameter is $kBalloonNone$.
	//
	//# \desc
	//# The $SetBalloon$ function sets the balloon type and string for a widget. The balloon type specified by the
	//# $type$ parameter can be one of the following constants.
	//
	//# \table	BalloonType
	//
	//# If a widget has a balloon, then it is displayed after the cursor has hovered over the widget for a short period of time.
	//# Balloons are not displayed for widgets that are disabled.
	//#
	//# The meaning of the balloon string specified by the $string$ parameter depends on the balloon type. If the balloon type
	//# is $kBalloonText$, then the balloon string itself is shown in the balloon. If the balloon type is $kBalloonResource$,
	//# then the balloon string names a panel resource that is loaded and displayed inside the balloon.
	//
	//# \also	$@Widget::GetBalloonType@$
	//# \also	$@Widget::GetBalloonString@$


	//# \div
	//# \function	Widget::SetBuildFlag		Indicates that a widget needs to be rebuilt.
	//
	//# \proto	void SetBuildFlag(void);
	//
	//# \desc
	//# The $SetBuildFlag$ function should be called by a $Widget$ subclass whenever some kind of state changes
	//# that requires the widget to be rebuilt. After the build flag has been set, the Interface Manager will call
	//# the widget's $@Widget::Build@$ function before the widget is next rendered.
	//#
	//# The build flag is initially set for all widgets that are subclasses of the $@RenderableWidget@$ class.
	//
	//# \also	$@Widget::Build@$


	//# \function	Widget::Build		Builds the geometric structure of a widget.
	//
	//# \proto	virtual void Build(void);
	//
	//# \desc
	//# The $Build$ function is called by the Interface Manager when a widget needs to be built before it can be
	//# rendered. An implementation of this function should build the geometric structure of a widget by filling in
	//# vertex arrays as necessary for the $@GraphicsMgr/Renderable@$ base class of the $@RenderableWidget@$ class.
	//#
	//# A widget indicates that it needs to be rebuilt by calling the $@Widget::SetBuildFlag@$ function. For subclasses
	//# of the $@RenderableWidget@$ class, the build flag is initially set during preprocessing.
	//
	//# \also	$@Widget::SetBuildFlag@$


	//# \div
	//# \function	Widget::TestPosition	Returns the widget part corresponding to a mouse position.
	//
	//# \proto	virtual WidgetPart TestPosition(const Point3D& position) const override;
	//
	//# \desc
	//# The $TestPosition$ function is called by the Interface Manager to determine what part of a widget was
	//# hit by a mouse event. The $position$ parameter specifies the position of the mouse in the local coordinate
	//# system of the widget. A subclass implementation should return a widget part code corresponding to the
	//# part of the widget in which the mouse position lies or the value $kWidgetPartNone$ if the mouse position
	//# did not hit an interactive part of the widget.
	//#
	//# The $TestPosition$ function is only called if the mouse position falls within the bounding box for the widget.
	//# The default implementation of the $TestPosition$ function always returns the value $kWidgetPartInterior$, and
	//# this does not need to be overridden unless a widget needs to make a distinction among multiple parts of a widget
	//# for its own reference in the $@Widget::HandleMouseEvent@$ function or $@Widget::TrackTask@$ function.
	//#
	//# A widget part code is a nonzero 32-bit identifier, normally represented as a four-character code. Identifiers
	//# consisting entirely of uppercase letters and numerical characters are reserved for use by the engine.
	//
	//# \also	$@Widget::HandleMouseEvent@$
	//# \also	$@Widget::TrackTask@$


	//# \function	Widget::HandleMouseEvent	Called when the user interacts with a widget using the mouse.
	//
	//# \proto	virtual void HandleMouseEvent(const PanelMouseEventData *eventData);
	//
	//# \param	eventData	A pointer to the event data.
	//
	//# \desc
	//# The $HandleMouseEvent$ function should be overridden by subclasses of the $@Widget@$ class that need to respond
	//# to mouse interaction from the user. This function is called by the Interface Manager when the user uses the
	//# mouse inside the widget's bounding box and the value returned by the widget's $@Widget::TestPosition@$ function
	//# is not $kWidgetPartNone$.
	//#
	//# The $eventType$ field of the $@PanelMouseEventData@$ structure can be one of the following constants.
	//
	//# \value	kEventMouseDown			The user pressed the primary mouse button.
	//# \value	kEventMouseUp			The user released the primary mouse button.
	//# \value	kEventRightMouseDown	The user pressed the right or secondary mouse button.
	//# \value	kEventRightMouseUp		The user released the right or secondary mouse button.
	//# \value	kEventMiddleMouseDown	The user pressed the middle or tertiary mouse button.
	//# \value	kEventMiddleMouseUp		The user released the middle or tertiary mouse button.
	//# \value	kEventMouseMoved		The user moved the mouse while one of the buttons is still pressed.
	//# \value	kEventMouseWheel		The user moved the mouse wheel.
	//
	//# \desc
	//# The $mousePosition$ field of the $@PanelMouseEventData@$ structure contains the position of the mouse in the
	//# local coordinate system of the widget. (The $z$ component of the position is always zero.)
	//#
	//# After one of the mouse down events has been received, the Interface Manager sends the $kEventMouseMoved$ event
	//# to the $HandleMouseEvent$ function any time the mouse is actually moved until the button is released, at which
	//# time a corresponding mouse up event is sent. Additionally, while a mouse button is down, the Interface Manager
	//# calls the $@Widget::TrackTask@$ function once per frame regardless of whether the mouse has moved.
	//#
	//# The $kEventMouseWheel$ event is only received if the $kWidgetMouseWheel$ usage flag has been set by the widget
	//# subclass implementation through a call to the $@Widget::SetWidgetUsage@$ function. For a mouse wheel event, the
	//# $y$ component of the $wheelDelta$ field of the $@PanelMouseEventData@$ structure contains a positive or
	//# negative movement value, and the $x$ component is always zero.
	//
	//# \also	$@Widget::TestPosition@$
	//# \also	$@Widget::HandleKeyboardEvent@$
	//# \also	$@Widget::PostWidgetEvent@$
	//# \also	$@Widget::GetWidgetUsage@$
	//# \also	$@Widget::SetWidgetUsage@$
	//# \also	$@PanelMouseEventData@$


	//# \function	Widget::HandleKeyboardEvent		Called when the user interacts with a widget using the keyboard.
	//
	//# \proto	virtual bool HandleKeyboardEvent(const KeyboardEventData *eventData);
	//
	//# \param	eventData	A pointer to the event data.
	//
	//# \desc
	//# The $HandleKeyboardEvent$ function should be overridden by subclasses of the $@Widget@$ class that need to respond
	//# to keyboard interaction from the user. This function is called by the Interface Manager when the user uses the
	//# keyboard while a widget has the keyboard focus. A widget subclass implementation indicates that it is able to receive
	//# the keyboard focus by specifying the $kWidgetKeyboardFocus$ usage flag in a call to the $@Widget::SetWidgetUsage@$ function.
	//#
	//# The $eventType$ field of the $@Utilities/KeyboardEventData@$ structure can be one of the following constants.
	//
	//# \value	kEventKeyDown		The user pressed a key. If the key is held in long enough for auto-repeat, then this event can be received more than once before the corresponding key up event is received.
	//# \value	kEventKeyUp			The user released a key.
	//# \value	kEventKeyCommand	The user pressed a key while the Control key was pressed (or on the Mac, when the Command key was pressed).
	//
	//# \desc
	//# An implementation of the $HandleKeyboardEvent$ function should return $true$ if it handled the event and $false$ otherwise.
	//# For subclasses of the $@Window@$ class, the implementation should call the $HandleKeyboardEvent$ function for the $@Window@$
	//# base class when a keyboard event is not handled, and it should return whatever value was returned by that function.
	//
	//# \also	$@Widget::HandleMouseEvent@$
	//# \also	$@Widget::PostWidgetEvent@$
	//# \also	$@Widget::GetWidgetUsage@$
	//# \also	$@Widget::SetWidgetUsage@$
	//# \also	$@Utilities/KeyboardEventData@$


	//# \function	Widget::PostWidgetEvent		Posts a widget event.
	//
	//# \proto	void PostWidgetEvent(const WidgetEventData& eventData);
	//
	//# \param	eventData	The widget event data.
	//
	//# \desc
	//# The $PostWidgetEvent$ function should be called by a widget subclass implementation when the observers of a widget need
	//# to be informed of some kind of change due to user interaction. The $eventType$ field of the $@WidgetEventData@$ structure
	//# can be one of the following constants.
	//
	//# \value	kEventWidgetActivate	The user activated the widget with the intention of causing some kind of permanent action to take place. For example, the user pressed a pushbutton.
	//# \value	kEventWidgetChange		The user caused some state of the widget to change, but not in a way that is intended to cause a permanent action to take place. For example, the user changed the state of a check box.
	//
	//# \desc
	//# A widget should always return immediately after calling the $PostWidgetEvent$ function because the observer that receives
	//# the event is allowed to destroy the widget in response to the event.
	//
	//# \also	$@Widget::Activate@$
	//# \also	$@Widget::HandleMouseEvent@$
	//# \also	$@Widget::HandleKeyboardEvent@$
	//# \also	$@WidgetEventData@$


	//# \function	Widget::Activate		Posts an activate widget event.
	//
	//# \proto	void Activate(Node *initiator = nullptr);
	//
	//# \param	initiator	The node that caused the widget to be activated.
	//
	//# \desc
	//# The $Activate$ function should be called when a widget should be activated. Tthe $Activate$ function is provided
	//# for convenience, and calling it is equivalent to making the following call:
	//
	//# \source
	//# PostWidgetEvent(WidgetEventData(kEventWidgetActivate, initiator));
	//
	//# \desc
	//# The $initiator$ parameter should be set to a node that caused the widget to be activated, if any. If the $Activate$
	//# function is called from a script method, for example, then the $initiator$ parameter should be set to the node returned
	//# by the $@Controller/ScriptState::GetInitiatorNode@$ function.
	//
	//# A widget should always return immediately after calling the $Activate$ function because the observer that receives
	//# the event is allowed to destroy the widget in response to the event.
	//
	//# \also	$@Widget::PostWidgetEvent@$
	//# \also	$@Widget::HandleMouseEvent@$
	//# \also	$@Widget::HandleKeyboardEvent@$
	//# \also	$@WidgetEventData@$


	//# \function	Widget::TrackTask		Called once per frame while the mouse interacts with a widget.
	//
	//# \proto	virtual void TrackTask(WidgetPart widgetPart, const Point3D& mousePosition);
	//
	//# \param	widgetPart		The part of the widget in which the user originally clicked.
	//# \param	mousePosition	The current mouse position in the widget's local coordinate system.
	//
	//# \desc
	//# The $TrackTask$ function is called once per frame by the Interface Manager in between the times when mouse down and
	//# mouse up events are sent to a widget through the $@Widget::HandleMouseEvent@$ function. This allows the widget's
	//# implementation to perform any kind of periodic task necessary while the user continues to hold the mouse button down
	//# without necessarily moving it.
	//#
	//# The $widgetPart$ parameter always specifies the part of the widget that was associated with the original mouse down
	//# event and does not reflect the part of the widget associated with the current position of the mouse.
	//
	//# \also	$@Widget::HandleMouseEvent@$


	//# \function	Widget::FocusTask		Called once per frame while a widget has the keyboard focus.
	//
	//# \proto	virtual void FocusTask(void);
	//
	//# \desc
	//# The $FocusTask$ function is called once per frame by the Interface Manager while a widget has the keyboard focus.
	//# This allows the widget's implementation to perform any kind of periodic task necessary, such as blinking a cursor.
	//
	//# \also	$@Widget::HandleKeyboardEvent@$


	//# \function	Widget::GetWidgetUsage		Returns the widget usage flags.
	//
	//# \proto	unsigned_int32 GetWidgetUsage(void) const;
	//
	//# \desc
	//# The $GetWidgetUsage$ function returns the widget usage flags, which can be a combination (through logical OR)
	//# of the following constants.
	//
	//# \table	WidgetUsage
	//
	//# \also	$@Widget::SetWidgetUsage@$
	//# \also	$@Widget::HandleMouseEvent@$
	//# \also	$@Widget::HandleKeyboardEvent@$


	//# \function	Widget::SetWidgetUsage		Sets the widget usage flags.
	//
	//# \proto	void SetWidgetUsage(unsigned_int32 usage);
	//
	//# \param	usage	The new widget usage flags.
	//
	//# \desc
	//# The $SetWidgetUsage$ function sets the widget usage flags to the value specified by the $usage$ parameter,
	//# which can be a combination (through logical OR) of the following constants.
	//
	//# \table	WidgetUsage
	//
	//# The initial value of the usage flags for all widgets is 0. A subclass implementation typically calls the $SetWidgetUsage$
	//# function in order to specify what kinds of events it can handle.
	//
	//# \also	$@Widget::GetWidgetUsage@$
	//# \also	$@Widget::HandleMouseEvent@$
	//# \also	$@Widget::HandleKeyboardEvent@$


	//# \div
	//# \function	Widget::Load	Loads a panel resource.
	//
	//# \proto	bool Load(const char *name);
	//
	//# \param	name	The name of the panel resource to load.
	//
	//# \desc
	//# The $Load$ function loads the panel resource specified by the $name$ parameter beneath the widget for which the function
	//# is called. That is, each top-level widget in the panel resource becomes a subnode of the widget for which the $Load$
	//# function is called. If the panel resource is successfully loaded, then the return value is $true$. If the panel resource
	//# does not exist, then the return value is $false$.
	//#
	//# Widgets created by the $Load$ function have not yet been preprocessed when this function returns.
	//
	//# \also	$@Window@$


	class Widget : public Tree<Widget>, public ListElement<Widget>, public HashTableElement<Widget>, public Transformable, public ExclusiveObservable<Widget, const WidgetEventData *>, public LinkTarget<Widget>, public Packable, public Configurable, public Registrable<Widget, WidgetRegistration>
	{
		friend class PanelController;

		public:

			typedef ConstCharKey KeyType;

			enum
			{
				kWidgetColorCount = 9
			};

			static const WidgetColorType widgetColorType[kWidgetColorCount];

		private:

			WidgetType				widgetType;
			WidgetType				baseWidgetType;

			WidgetKey				widgetKey;
			unsigned_int32			widgetState[2];

			unsigned_int32			widgetUsage;
			unsigned_int8			widgetAnchorX;
			unsigned_int8			widgetAnchorY;

			bool					buildFlag;
			unsigned_int8			activeUpdateFlags;
			unsigned_int8			currentUpdateFlags;
			unsigned_int8			subtreeUpdateFlags;

			WidgetColorType			defaultColorType;
			ColorRGBA				widgetColor;

			Vector2D				widgetSize;
			Transform4D				widgetTransform;

			Box2D					widgetBoundingBox;
			Box2D					worldBoundingBox;
			Box2D					*boundingBoxPointer;

			PanelController			*panelController;
			WidgetManipulator		*widgetManipulator;
			ScriptObject			*scriptObject;

			List<Mutator>			mutatorList;

			BalloonType				balloonType;
			String<>				balloonString;
			Link<Widget>			widgetBalloon;

			union
			{
				mutable int32		widgetIndex;
				mutable int32		superIndex;
			};

			C4API virtual Widget *Replicate(void) const;

			static void ScriptObjectLinkProc(Object *object, void *cookie);

			void ClearFocus(void);

		protected:

			enum
			{
				kUpdateStructure		= 1 << 0,
				kUpdateTransform		= 1 << 1,
				kUpdateBoundingBox		= 1 << 2
			};

			C4API Widget(WidgetType type, const Vector2D& size);
			C4API Widget(const Widget& widget);

			void SetBaseWidgetType(WidgetType type)
			{
				baseWidgetType = type;
			}

			void SetDefaultColorType(WidgetColorType type)
			{
				defaultColorType = type;
			}

			bool GetBuildFlag(void) const
			{
				return (buildFlag);
			}

			void SetBuildFlag(void)
			{
				buildFlag = true;
			}

			unsigned_int32 GetActiveUpdateFlags(void) const
			{
				return (activeUpdateFlags);
			}

			Box2D *GetBoundingBoxPointer(void)
			{
				return (boundingBoxPointer);
			}

			C4API void UpdateStructure(void);
			C4API void UpdateTransform(void);
			C4API virtual void UpdateBoundingBox(void);

			C4API virtual void HandleStructureUpdate(void);
			C4API virtual void HandleTransformUpdate(void);

			C4API virtual bool CalculateBoundingBox(Box2D *box) const;

		public:

			C4API Widget(WidgetType type = kWidgetGeneric);
			C4API virtual ~Widget();

			using Tree<Widget>::Previous;
			using Tree<Widget>::Next;

			KeyType GetKey(void) const
			{
				return (widgetKey);
			}

			WidgetType GetWidgetType(void) const
			{
				return (widgetType);
			}

			WidgetType GetBaseWidgetType(void) const
			{
				return (baseWidgetType);
			}

			static unsigned_int32 Hash(const KeyType& key)
			{
				return (Text::Hash(key));
			}

			const WidgetKey& GetWidgetKey(void) const
			{
				return (widgetKey);
			}

			unsigned_int32 GetWidgetState(void) const
			{
				return (widgetState[0]);
			}

			unsigned_int32 GetEditorState(void) const
			{
				return (widgetState[1]);
			}

			void SetEditorState(unsigned_int32 state)
			{
				widgetState[1] = state;
			}

			bool Enabled(void) const
			{
				return (!(widgetState[0] & kWidgetDisabled));
			}

			bool Visible(void) const
			{
				return (!(widgetState[0] & kWidgetHidden));
			}

			unsigned_int32 GetWidgetUsage(void) const
			{
				return (widgetUsage);
			}

			void SetWidgetUsage(unsigned_int32 usage)
			{
				widgetUsage = usage;
			}

			unsigned_int32 GetWidgetAnchorX(void) const
			{
				return (widgetAnchorX);
			}

			void SetWidgetAnchorX(unsigned_int32 anchor)
			{
				widgetAnchorX = (unsigned_int8) anchor;
			}

			unsigned_int32 GetWidgetAnchorY(void) const
			{
				return (widgetAnchorY);
			}

			void SetWidgetAnchorY(unsigned_int32 anchor)
			{
				widgetAnchorY = (unsigned_int8) anchor;
			}

			Vector2D& GetWidgetSize(void)
			{
				return (widgetSize);
			}

			const Vector2D& GetWidgetSize(void) const
			{
				return (widgetSize);
			}

			const Transform4D& GetWidgetTransform(void) const
			{
				return (widgetTransform);
			}

			void SetWidgetTransform(const Transform4D& transform)
			{
				widgetTransform = transform;
			}

			void SetWidgetTransform(const Matrix3D& matrix, const Point3D& position)
			{
				widgetTransform.Set(matrix, position);
			}

			void SetWidgetTransform(const Vector3D& c1, const Vector3D& c2, const Vector3D& c3, const Point3D& c4)
			{
				widgetTransform.Set(c1, c2, c3, c4);
			}

			void SetWidgetMatrix3D(const Matrix3D& matrix)
			{
				widgetTransform.SetMatrix3D(matrix);
			}

			void SetWidgetMatrix3D(const Vector3D& c1, const Vector3D& c2, const Vector3D& c3)
			{
				widgetTransform.SetMatrix3D(c1, c2, c3);
			}

			const Point3D& GetWidgetPosition(void) const
			{
				return (widgetTransform.GetTranslation());
			}

			void SetWidgetPosition(const Point3D& position)
			{
				widgetTransform.SetTranslation(position);
			}

			bool WidgetContainsPoint(const Point3D& p) const
			{
				return (widgetBoundingBox.Contains(p.GetPoint2D()));
			}

			const Box2D *GetBoundingBox(void) const
			{
				return (boundingBoxPointer);
			}

			PanelController *GetPanelController(void) const
			{
				return (panelController);
			}

			void SetPanelController(PanelController *controller)
			{
				panelController = controller;
			}

			ScriptObject *GetScriptObject(void) const
			{
				return (scriptObject);
			}

			WidgetManipulator *GetManipulator(void) const
			{
				return (widgetManipulator);
			}

			void SetManipulator(WidgetManipulator *manipulator)
			{
				widgetManipulator = manipulator;
			}

			void AppendNewSubnode(Widget *widget)
			{
				AppendSubnode(widget);
				widget->Preprocess();
			}

			void AddMutator(Mutator *mutator)
			{
				mutatorList.Append(mutator);
				mutator->targetWidget = this;
			}

			void RemoveMutator(Mutator *mutator)
			{
				mutator->targetWidget = nullptr;
				mutatorList.Remove(mutator);
			}

			Mutator *GetFirstMutator(void) const
			{
				return (mutatorList.First());
			}

			Mutator *GetLastMutator(void) const
			{
				return (mutatorList.Last());
			}

			BalloonType GetBalloonType(void) const
			{
				return (balloonType);
			}

			const char *GetBalloonString(void) const
			{
				return (balloonString);
			}

			void SetBalloon(BalloonType type, const char *string = "")
			{
				balloonType = type;
				balloonString = string;
			}

			Widget *GetWidgetBalloon(void) const
			{
				return (widgetBalloon.GetTarget());
			}

			void SetWidgetBalloon(Widget *balloon)
			{
				widgetBalloon = balloon;
			}

			void Activate(Node *initiator = nullptr)
			{
				PostWidgetEvent(WidgetEventData(kEventWidgetActivate, initiator));
			}

			C4API static Widget *New(WidgetType type);
			C4API Widget *Clone(void) const;

			static void RegisterStandardWidgets(void);

			C4API void Detach(void) override;
			C4API void AppendSubnode(Widget *node) override;
			C4API void PrependSubnode(Widget *node) override;
			C4API void InsertSubnodeBefore(Widget *node, Widget *before) override;
			C4API void InsertSubnodeAfter(Widget *node, Widget *after) override;
			C4API void RemoveSubnode(Widget *node) override;

			C4API void PackType(Packer& data) const override;
			C4API void Prepack(List<Object> *linkList) const override;
			C4API void Pack(Packer& data, unsigned_int32 packFlags) const override;
			C4API void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			C4API void *BeginSettingsUnpack(void) override;

			C4API int32 GetSettingCount(void) const override;
			C4API Setting *GetSetting(int32 index) const override;
			C4API void SetSetting(const Setting *setting) override;

			C4API void SetActiveUpdateFlags(unsigned_int32 flags);
			C4API virtual void Invalidate(void);
			C4API virtual void Update(void);

			C4API RootWidget *GetRootWidget(void) const;
			C4API Window *GetOwningWindow(void) const;

			C4API unsigned_int32 GetGlobalWidgetState(void) const;

			C4API void SetWidgetKey(const char *key);
			C4API Widget *GetNextWidgetWithSameKey(void) const;

			C4API void SetScriptObject(ScriptObject *object);
			C4API void ExecuteScript(Node *initiator, EventType eventType);

			C4API void PostWidgetEvent(const WidgetEventData& eventData);

			C4API virtual void SetWidgetState(unsigned_int32 state);

			C4API virtual const ColorRGBA& GetWidgetColor(WidgetColorType type = kWidgetColorDefault) const;
			C4API virtual void SetWidgetColor(const ColorRGBA& color, WidgetColorType type = kWidgetColorDefault);
			C4API virtual void SetWidgetAlpha(float alpha, WidgetColorType type = kWidgetColorDefault);
			C4API virtual void SetDynamicWidgetColor(const ColorRGBA& color, WidgetColorType type = kWidgetColorDefault);
			C4API virtual void SetDynamicWidgetAlpha(float alpha, WidgetColorType type = kWidgetColorDefault);

			C4API virtual void SetWidgetSize(const Vector2D& size);

			C4API virtual WidgetPart TestPosition(const Point3D& position) const;
			C4API Widget *DetectWidget(const Point3D& position, unsigned_int32 usage = 0, WidgetPart *widgetPart = nullptr);

			C4API virtual void Preprocess(void);
			C4API virtual void Move(void);

			C4API virtual void Show(void);
			C4API virtual void Hide(void);
			C4API virtual void Enable(void);
			C4API virtual void Disable(void);

			C4API virtual void EnterForeground(void);
			C4API virtual void EnterBackground(void);

			C4API virtual void Build(void);
			C4API virtual void Render(List<Renderable> *renderList);
			C4API virtual void RenderTree(List<Renderable> *renderList);

			C4API virtual void SendInitialStateMessages(Player *player) const;

			C4API virtual void HandleMouseEvent(const PanelMouseEventData *eventData);
			C4API virtual bool HandleKeyboardEvent(const KeyboardEventData *eventData);

			C4API virtual void TrackTask(WidgetPart widgetPart, const Point3D& mousePosition);
			C4API virtual void FocusTask(void);

			C4API FileResult PackTree(File *file, unsigned_int32 packFlags = 0) const;
			C4API void UnpackTree(const PanelResourceHeader *data, unsigned_int32 unpackFlags = 0);
			C4API bool Load(const char *name);
	};


	template <class observerType> class WidgetObserver : public ExclusiveObserver<observerType, Widget>
	{
		public:

			WidgetObserver(observerType *observer, void (observerType::*callback)(Widget *, const WidgetEventData *)) : ExclusiveObserver<observerType, Widget>(observer, callback)
			{
			}
	};


	//# \class	RenderableWidget	The base class for all renderable widgets.
	//
	//# Every renderable user interface widget is a subclass of the $RenderableWidget$ class.
	//
	//# \def	class RenderableWidget : public Widget, public Renderable
	//
	//# \ctor	RenderableWidget(WidgetType type, RenderType renderType);
	//# \ctor	RenderableWidget(WidgetType type, RenderType renderType, const Vector2D& size);
	//
	//# The constructor has protected access. The $RenderableWidget$ class can only be used as the base
	//# class for other interface widgets.
	//
	//# \param	type			The type of the widget.
	//# \param	renderType		The primitive type of the renderable object. See the $@GraphicsMgr/Renderable@$ base class.
	//# \param	size			The size of the widget, in pixels.
	//
	//# \desc
	//#
	//
	//# \base	Widget						Renderable widgets are user interface widgets.
	//# \base	GraphicsMgr/Renderable		A $RenderableWidget$ is a renderable object.


	class RenderableWidget : public Widget, public Renderable
	{
		private:

			Transformable						renderTransformable;
			WidgetObserver<RenderableWidget>	widgetObserver;

			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData);

		protected:

			struct BorderVertex
			{
				Point2D		position;
				Point2D		texcoord;
			};

			struct GlowVertex
			{
				Point2D		position;
				Point2D		texcoord;
			};

			C4API RenderableWidget(WidgetType type, RenderType renderType);
			C4API RenderableWidget(WidgetType type, RenderType renderType, const Vector2D& size);
			C4API RenderableWidget(const RenderableWidget& renderableWidget);

			const Transformable *GetRenderTransformable(void) const
			{
				return (&renderTransformable);
			}

			C4API void HandleTransformUpdate(void) override;

			C4API void InitRenderable(Renderable *renderable);

			C4API static void BuildBorder(const Box2D& box, volatile BorderVertex *restrict vertex, int32 style = kLineSolid);
			C4API static void BuildGlow(const Box2D& box, volatile GlowVertex *restrict vertex);

		public:

			C4API ~RenderableWidget();

			C4API void Preprocess(void) override;
			C4API void Render(List<Renderable> *renderList) override;
	};


	//# \class	LineWidget		The interface widget that displays a straight line.
	//
	//# The $LineWidget$ class represents an interface widget that displays a straight line.
	//
	//# \def	class LineWidget final : public RenderableWidget
	//
	//# \ctor	LineWidget(const Vector2D& size, int32 style = kLineSolid, const ColorRGBA& color = K::black);
	//
	//# \param	size	The size of the line, in pixels.
	//# \param	style	The line style. See below for possible values.
	//# \param	color	The initial color of the line.
	//
	//# \desc
	//# The $LineWidget$ class is used to render a straight line segment. The line is always one pixel thick, and its length is determined
	//# by the width of the widget, given by the $x$ component of the $size$ parameter. The $y$ component of the $size$ parameter should
	//# normally be set to 1.0, but any positive value is valid and does not affect the appearance of the line.
	//#
	//# The line is always rendered horizontally in the widget's local coordinate space. Lines are drawn in different directions by assigning
	//# a rotation to the widget's transform.
	//#
	//# The $style$ parameter determines whether the line is rendered solid, dotted, or dashed. It can be one of the following constants.
	//
	//# \table	LineStyle
	//
	//# The default widget color corresponds to the $kWidgetColorLine$ color type. No other color types are supported by the line widget.
	//
	//# \base	RenderableWidget	All rendered interface widgets are subclasses of $RenderableWidget$.
	//
	//# \also	$@BorderWidget@$
	//# \also	$@QuadWidget@$


	//# \function	LineWidget::GetLineStyle		Returns the line style used by a line widget.
	//
	//# \proto	int32 GetLineStyle(void) const;
	//
	//# \desc
	//# The $GetLineStyle$ function returns the line style used by a line widget. This function returns one of the following constants.
	//
	//# \table	LineStyle
	//
	//# \also	$@LineWidget::SetLineStyle@$


	//# \function	LineWidget::SetLineStyle		Sets the line style used by a line widget.
	//
	//# \proto	void SetLineStyle(int32 style);
	//
	//# \param	style	The new line style. See below for possible values.
	//
	//# \desc
	//# The $SetLineStyle$ function sets the line style used by a line widget. The $style$ parameter should be one of the following constants.
	//
	//# \table	LineStyle
	//
	//# \also	$@LineWidget::GetLineStyle@$


	class LineWidget final : public RenderableWidget
	{
		friend class WidgetReg<LineWidget>;

		private:

			struct LineVertex
			{
				Point2D			position;
				Point2D			texcoord;
			};

			int32						lineStyle;

			VertexBuffer				vertexBuffer;
			List<Attribute>				attributeList;
			DiffuseAttribute			colorAttribute;
			DiffuseTextureAttribute		textureAttribute;

			LineWidget();
			LineWidget(const LineWidget& lineWidget);

			C4API Widget *Replicate(void) const override;

		public:

			static C4API const TextureHeader lineTextureHeader;
			static C4API const unsigned_int8 lineTextureImage[128];

			C4API LineWidget(const Vector2D& size, int32 style = kLineSolid, const ColorRGBA& color = K::black);
			C4API ~LineWidget();

			int32 GetLineStyle(void) const
			{
				return (lineStyle);
			}

			void SetLineStyle(int32 style)
			{
				lineStyle = style;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void SetDynamicWidgetColor(const ColorRGBA& color, WidgetColorType type = kWidgetColorDefault) override;
			void SetDynamicWidgetAlpha(float alpha, WidgetColorType type = kWidgetColorDefault) override;

			void Preprocess(void) override;
			void Build(void) override;
	};


	//# \class	BorderWidget		The interface widget that displays a rectangular border.
	//
	//# The $BorderWidget$ class represents an interface widget that displays a rectangular border.
	//
	//# \def	class BorderWidget final : public RenderableWidget
	//
	//# \ctor	BorderWidget(const Vector2D& size, int32 style = kLineSolid, const ColorRGBA& color = K::black);
	//
	//# \param	size	The size of the border, in pixels.
	//# \param	style	The line style. See below for possible values.
	//# \param	color	The initial color of the border.
	//
	//# \desc
	//# The $BorderWidget$ class is used to render a rectangular border. The border is always one pixel thick, and its dimensions are determined
	//# by the size of the widget.
	//#
	//# The $style$ parameter determines whether the border is rendered solid, dotted, or dashed. It can be one of the following constants.
	//
	//# \table	LineStyle
	//
	//# The default widget color corresponds to the $kWidgetColorBorder$ color type. No other color types are supported by the border widget.
	//
	//# \base	RenderableWidget	All rendered interface widgets are subclasses of $RenderableWidget$.
	//
	//# The default widget color corresponds to the $kWidgetColorBorder$ color type. No other color types are supported by the border widget.
	//
	//# \also	$@LineWidget@$
	//# \also	$@QuadWidget@$


	//# \function	BorderWidget::GetLineStyle		Returns the line style used by a border widget.
	//
	//# \proto	int32 GetLineStyle(void) const;
	//
	//# \desc
	//# The $GetLineStyle$ function returns the line style used by a border widget. This function returns one of the following constants.
	//
	//# \table	LineStyle
	//
	//# \also	$@BorderWidget::SetLineStyle@$


	//# \function	BorderWidget::SetLineStyle		Sets the line style used by a border widget.
	//
	//# \proto	void SetLineStyle(int32 style);
	//
	//# \param	style	The new line style. See below for possible values.
	//
	//# \desc
	//# The $SetLineStyle$ function sets the line style used by a border widget. The $style$ parameter should be one of the following constants.
	//
	//# \table	LineStyle
	//
	//# \also	$@BorderWidget::GetLineStyle@$


	class BorderWidget final : public RenderableWidget
	{
		friend class WidgetReg<BorderWidget>;

		private:

			int32						lineStyle;

			VertexBuffer				vertexBuffer;
			List<Attribute>				attributeList;
			DiffuseAttribute			colorAttribute;
			DiffuseTextureAttribute		textureAttribute;

			BorderWidget();
			BorderWidget(const BorderWidget& borderWidget);

			C4API Widget *Replicate(void) const override;

		public:

			C4API BorderWidget(const Vector2D& size, int32 style = kLineSolid, const ColorRGBA& color = K::black);
			C4API ~BorderWidget();

			int32 GetLineStyle(void) const
			{
				return (lineStyle);
			}

			void SetLineStyle(int32 style)
			{
				lineStyle = style;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void SetDynamicWidgetColor(const ColorRGBA& color, WidgetColorType type = kWidgetColorDefault) override;
			void SetDynamicWidgetAlpha(float alpha, WidgetColorType type = kWidgetColorDefault) override;

			void Preprocess(void) override;
			void Build(void) override;
	};


	//# \class	TextWidget		The interface widget that displays a text string.
	//
	//# The $TextWidget$ class represents an interface widget that displays a text string.
	//
	//# \def	class TextWidget : public RenderableWidget
	//
	//# \ctor	TextWidget(const Vector2D& size, const char *text = nullptr, const char *font = nullptr);
	//# \ctor	TextWidget(const char *text = nullptr, const char *font = nullptr);
	//
	//# \param	size	The size of the text widget, in pixels.
	//# \param	text	The text string that is displayed.
	//# \param	font	The name of the font in which the text is displayed.
	//
	//# \desc
	//# The $TextWidget$ class handles all text rendering in the C4 Engine. A text widget displays a text string
	//# of arbitrary length using a single font. Text can be rendered on a single line, or it can be rendered on multiple
	//# lines in a text box. There are also several formatting tags that can be embedded in the text to control color,
	//# underlining, and alignment.
	//#
	//# If the $text$ parameter is not $nullptr$, then the string that it points to is copied into the text widget.
	//# If the $font$ parameter is not $nullptr$, then the font resource corresponding to the specified path is loaded
	//# and used for the text widget.
	//#
	//# The $size$ parameter specifies the size of a text widget's render box. The text box is used in different
	//# ways depending on the value of the text flags (see $@TextWidget::SetTextFlags@$). By default, only the $kTextClipped$
	//# flag is set, and the text widget renders text on a single line that is clipped on the left and right sides of the
	//# widget's bounds. A text widget can be changed to a multi-line text box configuration by calling the $@TextWidget::SetTextFlags@$
	//# function to set the $kTextWrapped$ flag.
	//#
	//# If the $kTextWrapped$ flag is set, then the width of the text box determines the maximum width of
	//# each rendered line. The width also determines how right- and center-aligned text is positioned.
	//# The height of the text box is not used for anything other than the background size for an editable
	//# text widget. Multi-line text widgets are bottomless unless the rendered line count is set using
	//# the $@TextWidget::SetRenderLineCount@$ function.
	//#
	//# If the $kTextWrapped$ flag is not set, then text is always rendered on a single line. If the
	//# $kTextClipped$ flag is set, then the text is clipped against the left and right edges of the text box.
	//# Otherwise, the width of the text box only affects alignment positioning. If the text box has a width
	//# of 0.0, then the left end, right end, or center of the text will coincide with the text widget's local origin.
	//# (If the $kTextClipped$ flag remains set in this case, however, no text is rendered because it is completely
	//# clipped away.)
	//#
	//# The text stored in a text widget may contain embedded formatting tags. A formatting tag is composed of four
	//# characters enclosed in square brackets. Formatting can be disabled altogether by setting the $kTextUnformatted$
	//# flag with the $@TextWidget::SetTextFlags@$ function, and specific types of formatting can be masked off using
	//# the $@TextWidget::SetTextFormatExclusionMask@$ function. (By default, alignment formatting is masked.)
	//# The following table lists the formatting tags recognized by the text widget.
	//
	//# \value	[#rgb]		Set the text color to the value given by the three hexadecimal digits <i>r</i>, <i>g</i>, and <i>b</i>.
	//# \value	[LEFT]		Set left text alignment.
	//# \value	[RGHT]		Set right text alignment.
	//# \value	[CENT]		Set center text alignment.
	//
	//# \desc
	//# Formatting tags are case insensitive, and valid tags are not rendered regardless of whether they are masked.
	//# Any unrecognized tags appearing in the text are rendered as is.
	//#
	//# The default widget color corresponds to the $kWidgetColorText$ color type and determines the initial color of the text.
	//# The $kWidgetColorLine$ color type is also supported by the text widget, and it controls the color of any underlining.
	//
	//# \base	RenderableWidget	All rendered interface widgets are subclasses of $RenderableWidget$.
	//
	//# \also	$@EditTextWidget@$
	//# \also	$@TextButtonWidget@$


	//# \function	TextWidget::GetText		Returns the text stored in a text widget.
	//
	//# \proto	const char *GetText(void) const;
	//
	//# \desc
	//# The $GetText$ function returns a pointer to the text stored in a text widget. The return value is never
	//# $nullptr$&mdash;if no text has been assigned to the widget yet, then the return value points to an empty string.
	//
	//# \also	$@TextWidget::SetText@$


	//# \function	TextWidget::SetText		Sets the text stored in a text widget.
	//
	//# \proto	virtual void SetText(const char *text, int32 max = 0);
	//
	//# \param	text	A pointer to the text to store in the text widget. This can be $nullptr$.
	//# \param	max		The maximum length of the text that is copied into the text widget. If this is zero, then there is no limit to the length of the text.
	//
	//# \desc
	//# The $SetText$ function sets the text stored in a text widget. If the $text$ parameter is not $nullptr$,
	//# then the string that it points to is copied into the text widget. There is no limit to the length of
	//# the string. If the $text$ parameter is $nullptr$, then any text previously stored in the text widget
	//# is deleted.
	//
	//# \also	$@TextWidget::GetText@$


	//# \function	TextWidget::GetFont		Returns the font used by a text widget.
	//
	//# \proto	Font *GetFont(void) const;
	//
	//# \desc
	//# The $GetFont$ function returns the font used by a text widget. If no font has been assigned, then
	//# the return value is $nullptr$.
	//
	//# \also	$@TextWidget::SetFont@$
	//# \also	$@TextWidget::GetFontName@$
	//# \also	$@Font@$


	//# \function	TextWidget::GetFontName		Returns the name of the font used by a text widget.
	//
	//# \proto	const ResourceName& GetFontName(void) const;
	//
	//# \desc
	//# The $GetFontName$ function returns the name of the font used by a text widget. If no font has been assigned, then
	//# the return value is an empty string.
	//
	//# \also	$@TextWidget::GetFont@$
	//# \also	$@TextWidget::SetFont@$


	//# \function	TextWidget::SetFont		Sets the font used by a text widget.
	//
	//# \proto	void SetFont(const char *font);
	//
	//# \param	font	The name of the font to assign to the text widget. This can be $nullptr$.
	//
	//# \desc
	//# The $SetFont$ function sets the font used by a text widget. If the $font$ parameter is not $nullptr$,
	//# then the specified font resource is loaded (if necessary) and used by the text widget. If the $font$
	//# parameter is $nullptr$, then any font already in use by the text widget is released. (A text widget is
	//# not rendered if no font is assigned to it.)
	//
	//# \also	$@TextWidget::GetFont@$
	//# \also	$@TextWidget::GetFontName@$
	//# \also	$@Font@$


	//# \function	TextWidget::GetTextFlags		Returns the text flags for a text widget.
	//
	//# \proto	unsigned_int32 GetTextFlags(void) const;
	//
	//# \desc
	//# The $GetTextFlags$ function returns the text flags, which can be a combination (through logical OR)
	//# of the following constants.
	//
	//# \table	TextFlags
	//
	//# \also	$@TextWidget::SetTextFlags@$


	//# \function	TextWidget::SetTextFlags		Sets the text flags for a text widget.
	//
	//# \proto	void SetTextFlags(unsigned_int32 flags);
	//
	//# \param	flags	The new text flags.
	//
	//# \desc
	//# The $SetTextFlags$ function sets the text flags to the value specified by the $flags$ parameter,
	//# which can be a combination (through logical OR) of the following constants.
	//
	//# \table	TextFlags
	//
	//# The initial value of the text flags is $kTextClipped$.
	//
	//# \also	$@TextWidget::GetTextFlags@$


	//# \function	TextWidget::GetLineCount		Returns total number of lines of text.
	//
	//# \proto	int32 GetLineCount(void) const;
	//
	//# \desc
	//# The $GetLineCount$ function returns the total number of lines of text stored in a text widget.
	//
	//# \also	$@TextWidget::GetFirstRenderLine@$
	//# \also	$@TextWidget::SetFirstRenderLine@$
	//# \also	$@TextWidget::GetRenderLineCount@$
	//# \also	$@TextWidget::SetRenderLineCount@$


	//# \function	TextWidget::GetFirstRenderLine		Returns the index of the first line rendered in a text box.
	//
	//# \proto	int32 GetFirstRenderLine(void) const;
	//
	//# \desc
	//# The $GetFirstRenderLine$ function returns the index of the first line rendered in a text box.
	//# See the $@TextWidget::SetRenderLineCount@$ function for a discussion of rendering a limited number of lines.
	//
	//# \also	$@TextWidget::SetFirstRenderLine@$
	//# \also	$@TextWidget::GetRenderLineCount@$
	//# \also	$@TextWidget::SetRenderLineCount@$
	//# \also	$@TextWidget::GetLineCount@$


	//# \function	TextWidget::SetFirstRenderLine		Sets the index of the first line rendered in a text box.
	//
	//# \proto	void SetFirstRenderLine(int32 index);
	//
	//# \param	index	The index of the first line to render.
	//
	//# \desc
	//# The $SetFirstRenderLine$ function sets the index of the first line rendered in a text box. The default value is 0.
	//# See the $@TextWidget::SetRenderLineCount@$ function for a discussion of rendering a limited number of lines.
	//
	//# \also	$@TextWidget::GetFirstRenderLine@$
	//# \also	$@TextWidget::GetRenderLineCount@$
	//# \also	$@TextWidget::SetRenderLineCount@$
	//# \also	$@TextWidget::GetLineCount@$


	//# \function	TextWidget::GetRenderLineCount		Returns the number of lines rendered in a text box.
	//
	//# \proto	int32 GetRenderLineCount(void) const;
	//
	//# \desc
	//# The $GetRenderLineCount$ function returns the number of lines rendered in a text box. The default value of 0 means
	//# that there is no limit to the number of lines rendered. See the $@TextWidget::SetRenderLineCount@$ function
	//# for a discussion of rendering a limited number of lines.
	//
	//# \also	$@TextWidget::SetRenderLineCount@$
	//# \also	$@TextWidget::GetFirstRenderLine@$
	//# \also	$@TextWidget::SetFirstRenderLine@$
	//# \also	$@TextWidget::GetLineCount@$


	//# \function	TextWidget::SetRenderLineCount		Sets the number of lines rendered in a text box.
	//
	//# \proto	void SetRenderLineCount(int32 count);
	//
	//# \param	count	The maximum number of lines to render.
	//
	//# \desc
	//# The $SetRenderLineCount$ function sets the number of lines rendered in a text box. This line count is only
	//# considered when the $kTextWrapped$ flag is set (see $@TextWidget::SetTextFlags@$). If the $count$ parameter
	//# is 0 (the default), then there is no limit to the number of lines rendered.
	//#
	//# The index of the first line is set using the $@TextWidget::SetFirstRenderLine@$ function. Beginning with
	//# this line, at most $count$ lines are rendered. Line breaks are determined by wrapping text within the width
	//# of the text box specified upon construction or by using the $@Widget::SetWidgetSize@$ function.
	//
	//# \also	$@TextWidget::GetRenderLineCount@$
	//# \also	$@TextWidget::GetFirstRenderLine@$
	//# \also	$@TextWidget::SetFirstRenderLine@$
	//# \also	$@TextWidget::GetLineCount@$
	//# \also	$@TextWidget::SetTextFlags@$


	//# \function	TextWidget::GetTextFormatExclusionMask		Returns the format exclusion mask for a text widget.
	//
	//# \proto	unsigned_int32 GetTextFormatExclusionMask(void) const;
	//
	//# \desc
	//# The $GetTextFormatExclusionMask$ function returns the format exclusion mask for a text widget, which can
	//# be a combination (through logical OR) of the following constants.
	//
	//# \table	TextFormatExclusionMask
	//
	//# When a bit is set in the format exclusion mask, then the corresponding format tags are not applied in the
	//# rendered text. The default format exclusion mask is the single bit $kTextFormatAlignment$.
	//
	//# \also	$@TextWidget::SetTextFormatExclusionMask@$


	//# \function	TextWidget::SetTextFormatExclusionMask		Sets the format exclusion mask for a text widget.
	//
	//# \proto	void GetTextFormatExclusionMask(unsigned_int32 mask);
	//
	//# \param	mask	The new format exclusion mask.
	//
	//# \desc
	//# The $SetTextFormatExclusionMask$ function sets the format exclusion mask for a text widget, which can
	//# be a combination (through logical OR) of the following constants.
	//
	//# \table	TextFormatExclusionMask
	//
	//# When a bit is set in the format exclusion mask, then the corresponding format tags are not applied in the
	//# rendered text. The default format exclusion mask is the single bit $kTextFormatAlignment$.
	//
	//# \also	$@TextWidget::GetTextFormatExclusionMask@$


	//# \function	TextWidget::GetTextAlignment		Returns the initial alignment for a text widget.
	//
	//# \proto	TextAlignment GetTextAlignment(void) const;
	//
	//# \desc
	//# The $GetTextAlignment$ function returns the initial alignment for a text widget, which can be one of the
	//# following constants.
	//
	//# \table	TextAlignment
	//
	//# The alignment can be changed by tags embedded in the text itself if $kTextFormatAlignment$ is not included
	//# in the format exclusion mask. (See the $@TextWidget::SetTextFormatExclusionMask@$ function.)
	//
	//# \also	$@TextWidget::SetTextAlignment@$
	//# \also	$@TextWidget::SetTextFormatExclusionMask@$


	//# \function	TextWidget::SetTextAlignment		Sets the initial alignment for a text widget.
	//
	//# \proto	void SetTextAlignment(TextAlignment alignment);
	//
	//# \param	alignment	The new text alignment.
	//
	//# \desc
	//# The $SetTextAlignment$ function sets the initial alignment for a text widget, which can be one of the
	//# following constants.
	//
	//# \table	TextAlignment
	//
	//# The default text alignment is $kTextAlignLeft$.
	//#
	//# The alignment can be changed by tags embedded in the text itself if $kTextFormatAlignment$ is not included
	//# in the format exclusion mask. (See the $@TextWidget::SetTextFormatExclusionMask@$ function.)
	//
	//# \also	$@TextWidget::SetTextAlignment@$
	//# \also	$@TextWidget::SetTextFormatExclusionMask@$


	//# \function	TextWidget::GetTextScale		Returns the scale for a text widget.
	//
	//# \proto	float GetTextScale(void) const;
	//
	//# \desc
	//# The $GetTextScale$ function returns the scale for a text widget.
	//
	//# \also	$@TextWidget::SetTextScale@$


	//# \function	TextWidget::SetTextScale		Sets the scale for a text widget.
	//
	//# \proto	void GetTextScale(float scale) const;
	//
	//# \param	scale	The new scale. This should be greater than 0.0.
	//
	//# \desc
	//# The $SetTextScale$ function sets the scale for a text widget. The default scale is 1.0.
	//
	//# \also	$@TextWidget::SetTextScale@$


	//# \function	TextWidget::GetTextLeading		Returns the leading for a text widget.
	//
	//# \proto	float GetTextLeading(void) const;
	//
	//# \desc
	//# The $GetTextLeading$ function returns the leading, in pixels, for a text widget.
	//
	//# \also	$@TextWidget::SetTextLeading@$


	//# \function	TextWidget::SetTextLeading		Sets the leading for a text widget.
	//
	//# \proto	void SetTextLeading(float leading) const;
	//
	//# \param	leading		The new leading, in pixels. This can be any finite value.
	//
	//# \desc
	//# The $SetTextLeading$ function sets the leading for a text widget. The leading is added to the normal line
	//# spacing used when rendering a multi-line text box. The default leading is 0.0, and both positive and negative
	//# values are allowed.
	//
	//# \also	$@TextWidget::GetTextLeading@$


	class TextWidget : public RenderableWidget
	{
		private:

			struct TextVertex
			{
				Point2D		position;
				Color4C		color;
				Point2D		texcoord;
			};

			unsigned_int32			textFlags;
			Vector3D				textRenderOffset;

			int32					firstRenderLine;
			int32					renderLineCount;

			unsigned_int32			formatExclusionMask;
			TextFormatState			initialFormat;

			float					textScale;
			float					textLeading;

			char					*textStorage;
			unsigned_int32			textStorageSize;
			int32					textLength;
			int32					glyphCount;

			bool					splitFlag;
			Array<int32>			lineEndArray;
			float					maxLineWidth;

			VertexBuffer			vertexBuffer;
			List<Attribute>			attributeList;
			ReferenceAttribute		referenceAttribute;

			Font					*textFont;
			ResourceName			fontName;

			static char				emptyString[1];

			C4API Widget *Replicate(void) const override;

			void AllocateTextStorage(const char *text, int32 max = 0);
			void ReleaseTextStorage(void);

			bool ProcessFormatTag(TextFormatTag tag, unsigned_int32 mask, TextFormatState *state, TextFormatState *savedState) const;
			TextFormatTag FindFormatTag(const char *text, int32 *offset) const;

			float GetAlignedTextWidth(const char *text, int32 length) const;
			float GetStartingPosition(const char *text, int32 length, TextAlignment alignment) const;
			int32 GetTextLengthFitWidth(const char *text, float width, float *used) const;

			void BuildLine(const char *text, int32 length, const Vector3D& renderOffset, volatile TextVertex *restrict& vertex, TextFormatState *format, TextFormatState *savedFormat);
			void BuildFlatText(void);
			void BuildWrappedText(void);

		protected:

			float GetLineSpacing(void) const
			{
				return ((textFont->GetLineSpacing() + textLeading) * textScale);
			}

			C4API TextWidget(WidgetType type, const char *text = nullptr, const char *font = nullptr);
			C4API TextWidget(WidgetType type, const Vector2D& size, const char *text = nullptr, const char *font = nullptr);
			C4API TextWidget(const TextWidget& textWidget);

		public:

			C4API TextWidget(const char *text = nullptr, const char *font = nullptr);
			C4API TextWidget(const Vector2D& size, const char *text = nullptr, const char *font = nullptr);
			C4API ~TextWidget();

			unsigned_int32 GetTextFlags(void) const
			{
				return (textFlags);
			}

			const Vector3D& GetTextRenderOffset(void) const
			{
				return (textRenderOffset);
			}

			int32 GetFirstRenderLine(void) const
			{
				return (firstRenderLine);
			}

			int32 GetRenderLineCount(void) const
			{
				return (renderLineCount);
			}

			unsigned_int32 GetTextFormatExclusionMask(void) const
			{
				return (formatExclusionMask);
			}

			TextAlignment GetTextAlignment(void) const
			{
				return (initialFormat.textAlignment);
			}

			const Color4C& GetInitialTextColor(void) const
			{
				return (initialFormat.textColor);
			}

			float GetTextScale(void) const
			{
				return (textScale);
			}

			float GetTextLeading(void) const
			{
				return (textLeading);
			}

			int32 GetTextLength(void) const
			{
				return (textLength);
			}

			int32 GetGlyphCount(void) const
			{
				return (glyphCount);
			}

			const char *GetText(void) const
			{
				return (textStorage);
			}

			Font *GetFont(void) const
			{
				return (textFont);
			}

			const ResourceName& GetFontName(void) const
			{
				return (fontName);
			}

			int32 GetLineCount(void) const
			{
				return (lineEndArray.GetElementCount());
			}

			int32 GetLineBegin(int32 index) const
			{
				return ((index == 0) ? 0 : lineEndArray[index - 1]);
			}

			int32 GetLineEnd(int32 index) const
			{
				return (lineEndArray[index]);
			}

			float GetMaxLineWidth(void) const
			{
				return (maxLineWidth);
			}

			C4API void Pack(Packer& data, unsigned_int32 packFlags) const override;
			C4API void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			C4API void *BeginSettingsUnpack(void) override;

			C4API int32 GetSettingCount(void) const override;
			C4API Setting *GetSetting(int32 index) const override;
			C4API void SetSetting(const Setting *setting) override;

			C4API void SetDynamicWidgetColor(const ColorRGBA& color, WidgetColorType type = kWidgetColorDefault) override;
			C4API void SetDynamicWidgetAlpha(float alpha, WidgetColorType type = kWidgetColorDefault) override;

			C4API void SetWidgetSize(const Vector2D& size) override;

			C4API void SetTextFlags(unsigned_int32 flags);
			C4API void SetTextRenderOffset(const Vector3D& offset);
			C4API void SetFirstRenderLine(int32 index);
			C4API void SetRenderLineCount(int32 count);

			C4API virtual void SetText(const char *text, int32 max = 0);
			C4API virtual void AppendText(const char *text, int32 max = 0);
			C4API virtual void PrependText(const char *text, int32 max = 0);

			C4API void SetFont(const char *font);

			C4API void SetTextFormatExclusionMask(unsigned_int32 mask);
			C4API void SetTextAlignment(TextAlignment alignment);
			C4API void SetTextScale(float scale);
			C4API void SetTextLeading(float leading);

			C4API float GetFormattedTextWidth(void) const;
			C4API void SplitLines(void);

			C4API void Preprocess(void) override;
			C4API void Build(void) override;
	};


	//# \class	EditTextWidget		The interface widget that displays an editable text box.
	//
	//# The $EditTextWidget$ class represents an interface widget that displays an editable text box.
	//
	//# \def	class EditTextWidget : public TextWidget
	//
	//# \ctor	EditTextWidget(const Vector2D& size, int32 maxGlyph, const char *font = nullptr);
	//
	//# \param	size		The size of the text box, in pixels.
	//# \param	maxGlyph	The maximum number of characters that can be entered into the text box.
	//# \param	font		The name of the font in which the text is displayed.
	//
	//# \desc
	//# The $EditTextWidget$ class displays an editable text box that allows a single line or multiple lines of text entry.
	//#
	//# The default widget color corresponds to the $kWidgetColorText$ color type and determines the color of the text.
	//# Other color types supported by the edit text widget are $kWidgetColorBorder$, $kWidgetColorBackground$, $kWidgetColorHilite$,
	//# $kWidgetColorFocus$, and $kWidgetColorCaret$. If a highlight color or focus color have not been explicitly specified, then the
	//# Interface Manager's global highlight color or focus color is used.
	//
	//# \base	TextWidget		The text displayed in the text box is based on the $TextWidget$ class.
	//
	//# \also	$@PasswordWidget@$


	class EditTextWidget : public TextWidget
	{
		friend class WidgetReg<EditTextWidget>;

		public:

			typedef bool FilterProc(unsigned_int32 code);

		private:

			enum
			{
				kFilterNone				= 0,
				kFilterNumber			= 'NUMB',
				kFilterSignedNumber		= 'SNUM',
				kFilterFloatingPoint	= 'FLOT',
				kFilterAlphanumeric		= 'ALPH',
				kFilterIdentifier		= 'IDNT',
				kFilterTypeCount		= 5
			};

			unsigned_int32				editTextFlags;
			unsigned_int32				editTextState;

			int32						maxGlyphCount;
			Type						filterType;
			FilterProc					*filterProc;

			float						paddingSize;
			unsigned_int32				colorOverrideFlags;

			ColorRGBA					backgroundColor;
			ColorRGBA					borderColor;
			ColorRGBA					hiliteColor;
			ColorRGBA					focusColor;
			ColorRGBA					caretColor;

			int32						caretPosition;
			float						caretMemory;
			int32						caretBlinkTime;
			int32						scrollTime;

			int32						selectionAnchor;
			int32						selectionDirection;
			int32						selectionBegin;
			int32						selectionEnd;

			VertexBuffer				structureVertexBuffer;

			List<Attribute>				backgroundAttributeList;
			DiffuseAttribute			backgroundColorAttribute;
			Renderable					backgroundRenderable;

			List<Attribute>				borderAttributeList;
			DiffuseAttribute			borderColorAttribute;
			DiffuseTextureAttribute		borderTextureAttribute;
			Renderable					borderRenderable;

			List<Attribute>				focusAttributeList;
			DiffuseAttribute			focusColorAttribute;
			Renderable					focusRenderable;

			List<Attribute>				caretAttributeList;
			DiffuseAttribute			caretColorAttribute;
			Renderable					caretRenderable;

			VertexBuffer				hiliteVertexBuffer;
			List<Attribute>				hiliteAttributeList;
			DiffuseAttribute			hiliteColorAttribute;
			Renderable					hiliteRenderable;

			static const Type			filterTypeTable[kFilterTypeCount];
			static FilterProc			*const filterProcTable[kFilterTypeCount];

			Widget *Replicate(void) const override;

			void SetDefaultHiliteColor(void);
			void SetDefaultFocusColor(void);

			bool CalculateBoundingBox(Box2D *box) const override;

			void ClearAll(void);

			int32 CalculateTextPosition(const Point3D& mousePosition) const;
			int32 CalculateLineIndex(int32 position) const;

			void UpdateSelection(const Point3D& mousePosition);
			void ExpandSelection(void);

			void HandleHomeKey(unsigned_int32 modifierKeys);
			void HandleEndKey(unsigned_int32 modifierKeys);
			void HandleLeftArrowKey(unsigned_int32 modifierKeys);
			void HandleRightArrowKey(unsigned_int32 modifierKeys);
			void HandleUpArrowKey(unsigned_int32 modifierKeys);
			void HandleDownArrowKey(unsigned_int32 modifierKeys);

			void InsertGlyph(unsigned_int32 code);

		protected:

			EditTextWidget(WidgetType type = kWidgetEditText);
			EditTextWidget(WidgetType type, const Vector2D& size, int32 maxGlyph, const char *font = nullptr);
			EditTextWidget(const EditTextWidget& editTextWidget);

			int32 GetCaretPosition(void) const
			{
				return (caretPosition);
			}

			int32 GetSelectionBegin(void) const
			{
				return (selectionBegin);
			}

			int32 GetSelectionEnd(void) const
			{
				return (selectionEnd);
			}

			virtual void InsertGlyphCode(const char *codeUTF8);
			virtual void RemoveGlyphCode(int32 count);
			virtual void ClearSelection(void);

		public:

			C4API EditTextWidget(const Vector2D& size, int32 maxGlyph, const char *font = nullptr);
			C4API ~EditTextWidget();

			unsigned_int32 GetEditTextFlags(void) const
			{
				return (editTextFlags);
			}

			int32 GetMaxGlyphCount(void) const
			{
				return (maxGlyphCount);
			}

			void SetFilterProc(FilterProc *proc)
			{
				filterProc = proc;
			}

			float GetPaddingSize(void) const
			{
				return (paddingSize);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			const ColorRGBA& GetWidgetColor(WidgetColorType type = kWidgetColorDefault) const override;
			void SetWidgetColor(const ColorRGBA& color, WidgetColorType type = kWidgetColorDefault) override;
			void SetWidgetAlpha(float alpha, WidgetColorType type = kWidgetColorDefault) override;
			void SetDynamicWidgetColor(const ColorRGBA& color, WidgetColorType type = kWidgetColorDefault) override;
			void SetDynamicWidgetAlpha(float alpha, WidgetColorType type = kWidgetColorDefault) override;

			void Preprocess(void) override;

			void EnterForeground(void) override;
			void EnterBackground(void) override;

			C4API static bool NumberFilter(unsigned_int32 code);
			C4API static bool SignedNumberFilter(unsigned_int32 code);
			C4API static bool FloatNumberFilter(unsigned_int32 code);
			C4API static bool AlphanumericFilter(unsigned_int32 code);
			C4API static bool IdentifierFilter(unsigned_int32 code);

			C4API void SetEditTextFlags(unsigned_int32 flags);
			C4API void SetPaddingSize(float offset);

			C4API void GetSelection(int32 *begin, int32 *end);
			C4API void SetSelection(int32 begin, int32 end);
			C4API void SelectAll(void);

			void SetText(const char *text, int32 max = 0);
			void AppendText(const char *text, int32 max = 0);
			void PrependText(const char *text, int32 max = 0);

			void SetWidgetState(unsigned_int32 state) override;

			void Build(void) override;
			void Render(List<Renderable> *renderList) override;

			void HandleMouseEvent(const PanelMouseEventData *eventData) override;
			bool HandleKeyboardEvent(const KeyboardEventData *eventData) override;

			void TrackTask(WidgetPart widgetPart, const Point3D& mousePosition) override;
			void FocusTask(void) override;
	};


	//# \class	PasswordWidget		The interface widget that displays an editable text box for passwords.
	//
	//# The $PasswordWidget$ class represents an interface widget that displays an editable text box for passwords.
	//
	//# \def	class PasswordWidget final : public EditTextWidget
	//
	//# \ctor	PasswordWidget(const Vector2D& size, int32 maxGlyph, const char *font = nullptr, bool secure = false);
	//
	//# \param	size		The size of the text box, in pixels.
	//# \param	maxGlyph	The maximum number of characters that can be entered into the text box.
	//# \param	font		The name of the font in which the password dots are displayed.
	//# \param	secure		Indicates whether security features are enabled.
	//
	//# \desc
	//# The $PasswordWidget$ class is a special type of $@EditTextWidget@$ that is intended for password entry.
	//# The specific characteristics that differentiate a password widget from an ordinary editable text widget
	//# are as follows.
	//#
	//# 1. The text displayed in the text box is always a sequence of dot characters (Unicode value U+2022).<br/>
	//# 2. If the user attempts to copy text from a password widget, then only the dot characters are copied.<br/>
	//# 3. If security features are enabled, the memory occupied by the actual password text is locked in physical
	//# memory during the lifetime of the password widget, and the memory is immediately erased when the password
	//# widget is destroyed. This prevents the password from being written to a swap file or persisting in unallocated memory.
	//#
	//# The $@TextWidget::GetText@$ function cannot retrieve the password string from a password widget. Instead, the
	//# $@PasswordWidget::GetPassword@$ function should be used.
	//
	//# If the $secure$ parameter is $true$, then security features are enabled to prevent the password from being
	//# leaked into the swap file or unallocated memory. The security features cannot be enabled or disabled after
	//# a password widget has been constructed. An application should ensure that no more than one secure password
	//# widget exists at any point in time. This avoids the improbable but possible case that two password widgets use the
	//# same virtual page in memory and the destruction of one password widget inadvertently unlocks the memory used by the
	//# other password widget.
	//
	//# \base	EditTextWidget		The $PasswordWidget$ class is a special type of editable text box.


	//# \function	PasswordWidget::GetPassword		Returns the password entered into the editable text box.
	//
	//# \proto	const PasswordString& GetPassword(void) const;
	//
	//# \desc
	//# The $GetPassword$ function returns a reference to the password string stored in a password widget.
	//# The $PasswordString$ type is defined as a $@Utilities/String@$ object with a maximum length of 63 bytes.
	//#
	//# The $@TextWidget::GetText@$ function cannot be used to retrieve the password because the string displayed
	//# to the user is only filled with dots.


	class PasswordWidget final : public EditTextWidget
	{
		friend class WidgetReg<PasswordWidget>;

		public:

			enum
			{
				kPasswordSecure		= 1 << 0,
				kPasswordLocked		= 1 << 1
			};

			enum
			{
				kMaxPasswordLength = 63
			};

			typedef String<kMaxPasswordLength> PasswordString;

		private:

			unsigned_int32		passwordFlags;
			PasswordString		passwordString;

			PasswordWidget();
			PasswordWidget(const PasswordWidget& passwordWidget);

			Widget *Replicate(void) const override;

			void InsertGlyphCode(const char *codeUTF8);
			void RemoveGlyphCode(int32 count);
			void ClearSelection(void);

		public:

			C4API PasswordWidget(const Vector2D& size, int32 maxGlyph, const char *font = nullptr, bool secure = false);
			C4API ~PasswordWidget();

			const PasswordString& GetPassword(void) const
			{
				return (passwordString);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Preprocess(void) override;

			void SetText(const char *text, int32 max = 0);
			void AppendText(const char *text, int32 max = 0);
			void PrependText(const char *text, int32 max = 0);
	};


	//# \class	QuadWidget		The interface widget that displays a filled quad.
	//
	//# The $QuadWidget$ class represents an interface widget that displays a filled quad.
	//
	//# \def	class QuadWidget : public RenderableWidget
	//
	//# \ctor	QuadWidget(const Vector2D& size, const ColorRGBA& color = K::white);
	//
	//# \param	size	The size of the quad, in pixels.
	//# \param	color	The initial color of the quad.
	//
	//# \desc
	//# The $QuadWidget$ class displays a plain colored quad.
	//#
	//# The default widget color controls the color of the quad. No specific color types are supported by the quad widget.
	//
	//# \base	RenderableWidget	All rendered interface widgets are subclasses of $RenderableWidget$.
	//
	//# \also	$@LineWidget@$
	//# \also	$@BorderWidget@$


	class QuadWidget : public RenderableWidget
	{
		friend class WidgetReg<QuadWidget>;

		private:

			struct QuadVertex
			{
				Point2D		position;
				ColorRGBA	color;
			};

			Vector2D			quadOffset;
			Vector2D			quadScale;
			ColorRGBA			quadColor[4];

			VertexBuffer		vertexBuffer;

			C4API Widget *Replicate(void) const override;

		protected:

			C4API QuadWidget(WidgetType type = kWidgetQuad);
			C4API QuadWidget(WidgetType type, const Vector2D& size, const ColorRGBA& color = K::white);
			C4API QuadWidget(const QuadWidget& quadWidget);

		public:

			C4API QuadWidget(const Vector2D& size, const ColorRGBA& color = K::white);
			C4API ~QuadWidget();

			const Vector2D& GetQuadOffset(void) const
			{
				return (quadOffset);
			}

			const Vector2D& GetQuadScale(void) const
			{
				return (quadScale);
			}

			const ColorRGBA& GetVertexColor(int32 index) const
			{
				return (quadColor[index]);
			}

			void SetVertexColor(int32 index, const ColorRGBA& color)
			{
				quadColor[index] = color;
				SetBuildFlag();
			}

			C4API void Pack(Packer& data, unsigned_int32 packFlags) const override;
			C4API void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			C4API void SetDynamicWidgetColor(const ColorRGBA& color, WidgetColorType type = kWidgetColorDefault) override;
			C4API void SetDynamicWidgetAlpha(float alpha, WidgetColorType type = kWidgetColorDefault) override;

			C4API void Preprocess(void) override;

			C4API void SetQuadOffset(const Vector2D& offset);
			C4API void SetQuadScale(const Vector2D& scale);

			C4API void Build(void) override;
	};


	//# \class	ImageWidget		The interface widget that displays a texture image.
	//
	//# The $ImageWidget$ class represents an interface widget that displays a texture image.
	//
	//# \def	class ImageWidget : public QuadWidget
	//
	//# \ctor	ImageWidget(const Vector2D& size, const char *name);
	//# \ctor	ImageWidget(const Vector2D& size, const TextureHeader *header, const void *image = nullptr);
	//
	//# \param	size	The size of the quad in which the image is rendered, in pixels.
	//# \param	name	The name of the texture resource.
	//# \param	header	A pointer to a texture header describing the texture image.
	//# \param	image	A pointer to the texture image data. If this is $nullptr$, then the texture header
	//#					must be part of a larger block of data containing the texture image.
	//
	//# \desc
	//# The $ImageWidget$ class displays a plain texture image.
	//
	//# \base	QuadWidget		An $ImageWidget$ is a specialized $QuadWidget$.
	//
	//# \also	$@IconButtonWidget@$


	class ImageWidget : public QuadWidget
	{
		friend class WidgetReg<ImageWidget>;

		private:

			Vector2D					imageOffset;
			Vector2D					imageScale;
			float						imageRotation;

			unsigned_int32				imageBlendState;

			float						imagePCoordinate;
			int32						cubeFaceIndex;

			VertexBuffer				vertexBuffer;
			List<Attribute>				attributeList;
			DiffuseTextureAttribute		textureAttribute1;
			DiffuseTextureAttribute		textureAttribute2;

			float						imageTexcoord[2][12];

			C4API Widget *Replicate(void) const override;

		protected:

			C4API ImageWidget(WidgetType type = kWidgetImage);
			C4API ImageWidget(WidgetType type, const Vector2D& size);
			C4API ImageWidget(WidgetType type, const Vector2D& size, const char *name);
			C4API ImageWidget(WidgetType type, const Vector2D& size, Texture *texture);
			C4API ImageWidget(WidgetType type, const Vector2D& size, const TextureHeader *header, const void *image = nullptr);
			C4API ImageWidget(const ImageWidget& imageWidget);

		public:

			C4API ImageWidget(const Vector2D& size);
			C4API ImageWidget(const Vector2D& size, const char *name);
			C4API ImageWidget(const Vector2D& size, Texture *texture);
			C4API ImageWidget(const Vector2D& size, const TextureHeader *header, const void *image = nullptr);
			C4API ~ImageWidget();

			const Vector2D& GetImageOffset(void) const
			{
				return (imageOffset);
			}

			void SetImageOffset(const Vector2D& offset)
			{
				imageOffset = offset;
				SetBuildFlag();
			}

			const Vector2D& GetImageScale(void) const
			{
				return (imageScale);
			}

			void SetImageScale(const Vector2D& scale)
			{
				imageScale = scale;
				SetBuildFlag();
			}

			float GetImageRotation(void) const
			{
				return (imageRotation);
			}

			void SetImageRotation(float rotation)
			{
				imageRotation = rotation;
				SetBuildFlag();
			}

			unsigned_int32 GetImageBlendState(void) const
			{
				return (imageBlendState);
			}

			void SetImageBlendState(unsigned_int32 state)
			{
				imageBlendState = state;
			}

			float GetImagePCoordinate(void) const
			{
				return (imagePCoordinate);
			}

			void SetImagePCoordinate(float p)
			{
				imagePCoordinate = p;
				SetBuildFlag();
			}

			int32 GetCubeFaceIndex(void) const
			{
				return (cubeFaceIndex);
			}

			void SetCubeFaceIndex(int32 index)
			{
				cubeFaceIndex = index;
				SetBuildFlag();
			}

			Texture *GetTexture(int32 index = 0) const
			{
				return ((&textureAttribute1)[index].GetTexture());
			}

			const char *GetTextureName(int32 index = 0) const
			{
				return ((&textureAttribute1)[index].GetTextureName());
			}

			C4API void Pack(Packer& data, unsigned_int32 packFlags) const override;
			C4API void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			C4API void *BeginSettingsUnpack(void) override;

			C4API int32 GetSettingCount(void) const override;
			C4API Setting *GetSetting(int32 index) const override;
			C4API void SetSetting(const Setting *setting) override;

			C4API void Preprocess(void) override;

			C4API void SetTexture(int32 index, const char *name);
			C4API void SetTexture(int32 index, Texture *texture);
			C4API void SetTexture(int32 index, const TextureHeader *header, const void *image = nullptr);

			C4API void Build(void) override;
	};


	//# \class	CheckWidget		The interface widget that displays a check box.
	//
	//# The $CheckWidget$ class represents an interface widget that displays a check box.
	//
	//# \def	class CheckWidget final : public TextWidget
	//
	//# \ctor	CheckWidget(const Vector2D& size, const char *text = nullptr, const char *font = nullptr);
	//
	//# \param	size	The size of the check box widget, in pixels.
	//# \param	text	The text to display with the check box.
	//# \param	font	The name of the font in which the text is displayed.
	//
	//# \desc
	//# The $CheckWidget$ class displays a check box that can be in the checked state or unchecked state.
	//#
	//# The default widget color controls the color of the text through the $@TextWidget@$ base class.
	//# The $kWidgetColorButton$ and $kWidgetColorHilite$ color types are also supported by the check widget, and they control
	//# the color of the check box. The highlight color is applied to the box instead of the button color if the widget is
	//# in the checked state and the $kCheckUseHighlightColor$ flag has been set. If a highlight color has not been explicitly
	//# specified, then the Interface Manager's global button color is used.
	//
	//# \base	TextWidget		The text displayed with the check box is based on the $TextWidget$ class.
	//
	//# \also	$@RadioWidget@$


	class CheckWidget final : public TextWidget
	{
		friend class WidgetReg<CheckWidget>;

		private:

			struct CheckVertex
			{
				Point2D		position;
				Point2D		texcoord;
			};

			int32					checkValue;
			unsigned_int32			checkFlags;
			unsigned_int32			colorOverrideFlags;

			ColorRGBA				buttonColor;
			ColorRGBA				hiliteColor;

			VertexBuffer			vertexBuffer;
			List<Attribute>			attributeList;
			DiffuseAttribute		colorAttribute;
			Renderable				checkRenderable;

			CheckWidget();
			CheckWidget(const CheckWidget& checkWidget);

			C4API Widget *Replicate(void) const override;

			void SetDefaultHiliteColor(void);

		public:

			C4API CheckWidget(const Vector2D& size, const char *text = nullptr, const char *font = nullptr);
			C4API ~CheckWidget();

			int32 GetValue(void) const
			{
				return (checkValue);
			}

			unsigned_int32 GetCheckFlags(void) const
			{
				return (checkFlags);
			}

			C4API void SetValue(int32 value, bool post = false);
			C4API void SetCheckFlags(unsigned_int32 flags);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			const ColorRGBA& GetWidgetColor(WidgetColorType type = kWidgetColorDefault) const override;
			void SetWidgetColor(const ColorRGBA& color, WidgetColorType type = kWidgetColorDefault) override;
			void SetDynamicWidgetColor(const ColorRGBA& color, WidgetColorType type = kWidgetColorDefault) override;

			WidgetPart TestPosition(const Point3D& position) const override;

			void Preprocess(void) override;
			void Build(void) override;
			void Render(List<Renderable> *renderList) override;

			void HandleMouseEvent(const PanelMouseEventData *eventData) override;
	};


	//# \class	RadioWidget		The interface widget that displays a radio button.
	//
	//# The $RadioWidget$ class represents an interface widget that displays a radio button.
	//
	//# \def	class RadioWidget final : public TextWidget
	//
	//# \ctor	RadioWidget(const Vector2D& size, const char *text = nullptr, const char *font = nullptr);
	//
	//# \param	size	The size of the radio button widget, in pixels.
	//# \param	text	The text to display with the radio button.
	//# \param	font	The name of the font in which the text is displayed.
	//
	//# \desc
	//# The $RadioWidget$ class displays a radio button that can be in the selected or unselected state. When a radio button is selected, all other
	//# radio buttons in the same group (or in the entire window if the radio button is not in a group) are automatically unselected.
	//#
	//# The default widget color controls the color of the text through the $@TextWidget@$ base class.
	//# The $kWidgetColorButton$ and $kWidgetColorHilite$ color types are also supported by the radio widget, and they control
	//# the color of the radio button. The highlight color is applied to the button instead of the button color if the widget is
	//# in the selected state and the $kRadioUseHighlightColor$ flag has been set. If a highlight color has not been explicitly
	//# specified, then the Interface Manager's global button color is used.
	//
	//# \base	TextWidget		The text displayed with the radio button is based on the $TextWidget$ class.
	//
	//# \also	$@CheckWidget@$


	class RadioWidget final : public TextWidget
	{
		friend class WidgetReg<RadioWidget>;

		private:

			struct RadioVertex
			{
				Point2D		position;
				Point2D		texcoord;
			};

			int32					radioValue;
			unsigned_int32			radioFlags;
			unsigned_int32			colorOverrideFlags;

			ColorRGBA				buttonColor;
			ColorRGBA				hiliteColor;

			VertexBuffer			vertexBuffer;
			List<Attribute>			attributeList;
			DiffuseAttribute		colorAttribute;
			Renderable				radioRenderable;

			RadioWidget();
			RadioWidget(const RadioWidget& radioWidget);

			C4API Widget *Replicate(void) const override;

			void SetDefaultHiliteColor(void);

		public:

			C4API RadioWidget(const Vector2D& size, const char *text = nullptr, const char *font = nullptr);
			C4API ~RadioWidget();

			int32 GetValue(void) const
			{
				return (radioValue);
			}

			unsigned_int32 GetRadioFlags(void) const
			{
				return (radioFlags);
			}

			C4API void SetValue(int32 value, bool post = false);
			C4API void SetRadioFlags(unsigned_int32 flags);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			const ColorRGBA& GetWidgetColor(WidgetColorType type = kWidgetColorDefault) const override;
			void SetWidgetColor(const ColorRGBA& color, WidgetColorType type = kWidgetColorDefault) override;
			void SetDynamicWidgetColor(const ColorRGBA& color, WidgetColorType type = kWidgetColorDefault) override;

			WidgetPart TestPosition(const Point3D& position) const override;

			void Preprocess(void) override;
			void Build(void) override;
			void Render(List<Renderable> *renderList) override;

			void HandleMouseEvent(const PanelMouseEventData *eventData) override;
	};


	class GuiButtonWidget : public RenderableWidget
	{
		private:

			struct ButtonVertex
			{
				Point2D		position;
				Point2D		texcoord;
			};

			Range<Point2D>			texcoordRange;
			Vector2D				texcoordOffset;

			VertexBuffer			vertexBuffer;
			List<Attribute>			attributeList;
			DiffuseAttribute		colorAttribute;

			Widget *Replicate(void) const override;

		protected:

			GuiButtonWidget(WidgetType type = kWidgetGuiButton);
			GuiButtonWidget(WidgetType type, const Vector2D& size, const Point2D& minTex, const Point2D& maxTex);
			GuiButtonWidget(const GuiButtonWidget& guiButtonWidget);

			const Vector2D& GetTexcoordOffset(void) const
			{
				return (texcoordOffset);
			}

			void SetTexcoordOffset(const Vector2D& offset)
			{
				texcoordOffset = offset;
				SetBuildFlag();
			}

		public:

			C4API GuiButtonWidget(const Vector2D& size, const Point2D& minTex, const Point2D& maxTex);
			C4API ~GuiButtonWidget();

			void Preprocess(void) override;
			void Build(void) override;

			void HandleMouseEvent(const PanelMouseEventData *eventData) override;
	};


	//# \class	PushButtonWidget	The interface widget that displays a push button containing text.
	//
	//# The $PushButtonWidget$ class represents an interface widget that displays a push button containing text.
	//
	//# \def	class PushButtonWidget final : public TextWidget
	//
	//# \ctor	PushButtonWidget(const Vector2D& size, const char *text = nullptr, const char *font = nullptr);
	//
	//# \param	size	The size of the push button widget, in pixels.
	//# \param	text	The text to display inside the push button.
	//# \param	font	The name of the font in which the text is displayed.
	//
	//# \desc
	//# The $PushButtonWidget$ class displays a button with a text string.
	//#
	//# The default widget color controls the color of the text through the $@TextWidget@$ base class.
	//# The $kWidgetColorButton$ color type is also supported by the push button widget, and it controls the color of the button.
	//# If a button color has not been explicitly specified, then the Interface Manager's global button color is used.
	//
	//# \base	TextWidget		The text displayed in the push button is based on the $TextWidget$ class.
	//
	//# \also	$@TextButtonWidget@$


	class PushButtonWidget final : public TextWidget
	{
		friend class WidgetReg<PushButtonWidget>;

		private:

			struct ButtonVertex
			{
				Point2D		position;
				Point2D		texcoord;
			};

			unsigned_int32			pushButtonFlags;
			Type					activationKey;

			ColorRGBA				buttonColor;

			VertexBuffer			vertexBuffer;
			List<Attribute>			attributeList;
			DiffuseAttribute		colorAttribute;
			Renderable				buttonRenderable;

			PushButtonWidget();
			PushButtonWidget(const PushButtonWidget& pushButtonWidget);

			Widget *Replicate(void) const override;

			void SetPrimaryButtonColor(void);

		public:

			C4API PushButtonWidget(const Vector2D& size, const char *text = nullptr, const char *font = nullptr);
			C4API ~PushButtonWidget();

			unsigned_int32 GetPushButtonFlags(void) const
			{
				return (pushButtonFlags);
			}

			Type GetActivationKey(void) const
			{
				return (activationKey);
			}

			void SetActivationKey(Type key)
			{
				activationKey = key;
			}

			C4API void SetPushButtonFlags(unsigned_int32 flags);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			const ColorRGBA& GetWidgetColor(WidgetColorType type = kWidgetColorDefault) const override;
			void SetWidgetColor(const ColorRGBA& color, WidgetColorType type = kWidgetColorDefault) override;
			void SetDynamicWidgetColor(const ColorRGBA& color, WidgetColorType type = kWidgetColorDefault) override;

			void Preprocess(void) override;
			void Build(void) override;
			void Render(List<Renderable> *renderList) override;

			void HandleMouseEvent(const PanelMouseEventData *eventData) override;
	};


	//# \class	IconButtonWidget	The interface widget that displays a push button containing an image.
	//
	//# The $IconButtonWidget$ class represents an interface widget that displays a push button containing an image.
	//
	//# \def	class IconButtonWidget final : public ImageWidget
	//
	//# \ctor	IconButtonWidget(const Vector2D& size, const char *name);
	//
	//# \param	size	The size of the quad in which the image is rendered, in pixels.
	//# \param	name	The name of the texture resource.
	//
	//# \desc
	//# The $IconButtonWidget$ class displays a button with a texture image.
	//#
	//# The default widget color controls the color modulating the image through the $@QuadWidget@$ (indirect) base class.
	//# The $kWidgetColorButton$ color type is also supported by the icon button widget, and it controls the color of the button.
	//
	//# \base	ImageWidget		The image displayed in the push button is based on the $ImageWidget$ class.


	class IconButtonWidget final : public ImageWidget
	{
		friend class WidgetReg<IconButtonWidget>;

		private:

			struct ButtonVertex
			{
				Point2D		position;
				Point2D		texcoord;
			};

			int32					iconButtonValue;
			unsigned_int32			iconButtonFlags;

			ColorRGBA				buttonColor;

			VertexBuffer			vertexBuffer;
			List<Attribute>			attributeList;
			DiffuseAttribute		colorAttribute;
			Renderable				buttonRenderable;

			IconButtonWidget();
			IconButtonWidget(const IconButtonWidget& iconButtonWidget);

			Widget *Replicate(void) const override;

		public:

			C4API IconButtonWidget(const Vector2D& size, const char *name);
			C4API ~IconButtonWidget();

			int32 GetValue(void) const
			{
				return (iconButtonValue);
			}

			unsigned_int32 GetIconButtonFlags(void) const
			{
				return (iconButtonFlags);
			}

			void SetIconButtonFlags(unsigned_int32 flags)
			{
				iconButtonFlags = flags;
			}

			C4API void SetValue(int32 value, bool post = false);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			const ColorRGBA& GetWidgetColor(WidgetColorType type = kWidgetColorDefault) const override;
			void SetWidgetColor(const ColorRGBA& color, WidgetColorType type = kWidgetColorDefault) override;
			void SetDynamicWidgetColor(const ColorRGBA& color, WidgetColorType type = kWidgetColorDefault) override;

			void Preprocess(void) override;
			void Build(void) override;
			void Render(List<Renderable> *renderList) override;

			void HandleMouseEvent(const PanelMouseEventData *eventData) override;
	};


	//# \class	TextButtonWidget	The interface widget that displays an interactive text string.
	//
	//# The $TextButtonWidget$ class represents an interface widget that displays an interactive text string.
	//
	//# \def	class TextButtonWidget : public TextWidget
	//
	//# \ctor	TextButtonWidget(const Vector2D& size, const char *text = nullptr, const char *font = nullptr);
	//
	//# \param	size	The size of the text widget, in pixels.
	//# \param	text	The text string that is displayed.
	//# \param	font	The name of the font in which the text is displayed.
	//
	//# \desc
	//# The $TextButtonWidget$ class displays a clickable text string.
	//#
	//# The default widget color corresponds to the $kWidgetColorText$ color type and determines the color of the text.
	//# The $kWidgetColorHilite$ color type is also supported by the text button widget, and it controls the color of the text
	//# while the user is pressing the button.
	//
	//# \base	TextWidget		The $TextButtonWidget$ class is a special type of text widget.
	//
	//# \also	$@HyperlinkWidget@$


	class TextButtonWidget : public TextWidget
	{
		friend class WidgetReg<TextButtonWidget>;

		private:

			ColorRGBA		hiliteColor;

			TextButtonWidget();

			Widget *Replicate(void) const override;

		protected:

			TextButtonWidget(WidgetType type, const char *text = nullptr, const char *font = nullptr);
			TextButtonWidget(WidgetType type, const Vector2D& size, const char *text = nullptr, const char *font = nullptr);
			TextButtonWidget(const TextButtonWidget& textButtonWidget);

		public:

			C4API TextButtonWidget(const Vector2D& size, const char *text = nullptr, const char *font = nullptr);
			C4API ~TextButtonWidget();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			const ColorRGBA& GetWidgetColor(WidgetColorType type = kWidgetColorDefault) const override;
			void SetWidgetColor(const ColorRGBA& color, WidgetColorType type = kWidgetColorDefault) override;
			void SetWidgetAlpha(float alpha, WidgetColorType type = kWidgetColorDefault) override;

			void HandleMouseEvent(const PanelMouseEventData *eventData) override;
	};


	//# \class	HyperlinkWidget		The interface widget that displays a hyperlink.
	//
	//# The $HyperlinkWidget$ class represents an interface widget that displays a hyperlink.
	//
	//# \def	class HyperlinkWidget final : public TextButtonWidget
	//
	//# \ctor	HyperlinkWidget(const Vector2D& size, const char *text = nullptr, const char *font = nullptr,
	//# \ctor2	const char *hyperlink = nullptr);
	//
	//# \param	size		The size of the text widget, in pixels.
	//# \param	text		The text string that is displayed.
	//# \param	font		The name of the font in which the text is displayed.
	//# \param	hyperlink	A string containing the hyperlink address.
	//
	//# \desc
	//# The $HyperlinkWidget$ widget displays a text button with a hyperlink. Clicking on the button opens the default web browser installed on
	//# the user's computer and navigates to the address stored in the widget.
	//
	//# \base	TextButtonWidget	The $HyperlinkWidget$ class is a special type of text button widget.


	class HyperlinkWidget final : public TextButtonWidget
	{
		private:

			String<>							hyperlinkAddress;
			WidgetObserver<HyperlinkWidget>		hyperlinkObserver;

			HyperlinkWidget(const HyperlinkWidget& hyperlinkWidget);

			Widget *Replicate(void) const override;

			void HandleHyperlinkEvent(Widget *widget, const WidgetEventData *eventData);

		public:

			C4API HyperlinkWidget(const char *text = nullptr, const char *font = nullptr, const char *hyperlink = nullptr);
			C4API HyperlinkWidget(const Vector2D& size, const char *text = nullptr, const char *font = nullptr, const char *hyperlink = nullptr);
			C4API ~HyperlinkWidget();

			const String<>& GetHyperlinkAddress(void) const
			{
				return (hyperlinkAddress);
			}

			void SetHyperlinkAddress(const char *hyperlink)
			{
				hyperlinkAddress = hyperlink;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Preprocess(void) override;
	};


	//# \class	ColorWidget		The interface widget that displays a color picking box.
	//
	//# The $ColorWidget$ class represents an interface widget that displays a color picking box.
	//
	//# \def	class ColorWidget final : public RenderableWidget
	//
	//# \ctor	ColorWidget(const Vector2D& size, const ColorRGBA& color = K::white);
	//
	//# \param	size	The size of the color box, in pixels.
	//# \param	color	The color that is initially displayed in the color box.
	//
	//# \desc
	//# The $ColorWidget$ class displays a color selection box that displays a color picker dialog when clicked.
	//#
	//# The default widget color corresponds to the $kWidgetColorBorder$ color type and determines the color of the border.
	//# The $kWidgetColorFocus$ color type is also supported, and it controls the color of the glow rendered around the box
	//# while the user is clicking on it. If a focus color has not been explicitly specified, then the Interface Manager's
	//# global button color is used.
	//
	//# \base	RenderableWidget	All rendered interface widgets are subclasses of $RenderableWidget$.


	class ColorWidget final : public RenderableWidget
	{
		friend class WidgetReg<ColorWidget>;

		public:

			enum
			{
				kMaxColorPickerTitleLength = 63
			};

			typedef String<kMaxColorPickerTitleLength>	ColorPickerTitle;

		private:

			struct QuadVertex
			{
				Point2D		position;
				ColorRGB	color;
			};

			ColorRGBA					colorValue;
			unsigned_int32				colorPickerFlags;
			ColorPickerTitle			colorPickerTitle;

			unsigned_int32				colorOverrideFlags;
			ColorRGBA					focusColor;

			VertexBuffer				vertexBuffer;

			List<Attribute>				borderAttributeList;
			DiffuseAttribute			borderColorAttribute;
			DiffuseTextureAttribute		borderTextureAttribute;

			List<Attribute>				focusAttributeList;
			DiffuseAttribute			focusColorAttribute;
			Renderable					focusRenderable;

			Renderable					quadRenderable;

			ColorWidget();
			ColorWidget(const ColorWidget& colorWidget);

			Widget *Replicate(void) const override;

			void SetDefaultFocusColor(void);

			static void ColorPickerComplete(ColorPicker *colorPicker, void *cookie);

		public:

			C4API ColorWidget(const Vector2D& size, const ColorRGBA& color = K::white);
			C4API ~ColorWidget();

			const ColorRGBA& GetValue(void) const
			{
				return (colorValue);
			}

			unsigned_int32 GetColorPickerFlags(void) const
			{
				return (colorPickerFlags);
			}

			void SetColorPickerFlags(unsigned_int32 flags)
			{
				colorPickerFlags = flags;
			}

			const ColorPickerTitle& GetColorPickerTitle(void) const
			{
				return (colorPickerTitle);
			}

			void SetColorPickerTitle(const char *title)
			{
				colorPickerTitle = title;
			}

			C4API void SetValue(const ColorRGBA& value, bool post = false);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			const ColorRGBA& GetWidgetColor(WidgetColorType type = kWidgetColorDefault) const override;
			void SetWidgetColor(const ColorRGBA& color, WidgetColorType type = kWidgetColorDefault) override;
			void SetWidgetAlpha(float alpha, WidgetColorType type = kWidgetColorDefault) override;
			void SetDynamicWidgetColor(const ColorRGBA& color, WidgetColorType type = kWidgetColorDefault) override;
			void SetDynamicWidgetAlpha(float alpha, WidgetColorType type = kWidgetColorDefault) override;

			void Preprocess(void) override;
			void Build(void) override;
			void Render(List<Renderable> *renderList) override;

			void HandleMouseEvent(const PanelMouseEventData *eventData) override;
	};


	//# \class	ProgressWidget		The interface widget that displays a progress bar.
	//
	//# The $ProgressWidget$ class represents an interface widget that displays a progress bar.
	//
	//# \def	class ProgressWidget final : public RenderableWidget
	//
	//# \ctor	ProgressWidget(const Vector2D& size);
	//
	//# \param	size	The size of the progress bar, in pixels.
	//
	//# \desc
	//# The $ProgressWidget$ class displays a progress bar.
	//#
	//# The default widget color corresponds to the $kWidgetColorBackground$ color type and determines the background color
	//# of the progress bar. The $kWidgetColorHilite$ color type is also supported, and it controls the color of the filled
	//# portion of the progress bar. If a highlight color has not been explicitly specified, then the Interface Manager's
	//# global highlight color is used.
	//
	//# \base	RenderableWidget	All rendered interface widgets are subclasses of $RenderableWidget$.


	class ProgressWidget final : public RenderableWidget
	{
		friend class WidgetReg<ProgressWidget>;

		private:

			struct ProgressVertex
			{
				Point2D		position;
				ColorRGBA	color;
				Point2D		texcoord;
			};

			int32				progressValue;
			int32				maxProgressValue;

			unsigned_int32		colorOverrideFlags;
			ColorRGBA			hiliteColor;

			ColorRGBA			dynamicBackgroundColor;
			ColorRGBA			dynamicHiliteColor;

			VertexBuffer		vertexBuffer;

			ProgressWidget();
			ProgressWidget(const ProgressWidget& progressWidget);

			Widget *Replicate(void) const override;

			void SetDefaultHiliteColor(void);

		public:

			C4API ProgressWidget(const Vector2D& size);
			C4API ~ProgressWidget();

			int32 GetValue(void) const
			{
				return (progressValue);
			}

			int32 GetMaxValue(void) const
			{
				return (maxProgressValue);
			}

			C4API void SetValue(int32 value, bool post = false);
			C4API void SetMaxValue(int32 maxValue);
			C4API void SetProgress(int32 value, int32 maxValue);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			C4API const ColorRGBA& GetWidgetColor(WidgetColorType type = kWidgetColorDefault) const override;
			C4API void SetWidgetColor(const ColorRGBA& color, WidgetColorType type = kWidgetColorDefault) override;
			C4API void SetWidgetAlpha(float alpha, WidgetColorType type = kWidgetColorDefault) override;
			C4API void SetDynamicWidgetColor(const ColorRGBA& color, WidgetColorType type = kWidgetColorDefault) override;
			C4API void SetDynamicWidgetAlpha(float alpha, WidgetColorType type = kWidgetColorDefault) override;

			void Preprocess(void) override;
			void Build(void) override;
	};


	//# \class	SliderWidget	The interface widget that displays a slider.
	//
	//# The $SliderWidget$ class represents an interface widget that displays a slider.
	//
	//# \def	class SliderWidget final : public RenderableWidget
	//
	//# \ctor	SliderWidget(const Vector2D& size);
	//
	//# \param	size	The size of the slider, in pixels.
	//
	//# \desc
	//# The $SliderWidget$ class displays a slider.
	//#
	//# The default widget color corresponds to the $kWidgetColorBackground$ color type and determines the color
	//# of the slider bar. The $kWidgetColorButton$ color type is also supported, and it controls the color of the slider
	//# indicator button. If a button color has not been explicitly specified, then the Interface Manager's
	//# global button color is used.
	//
	//# \base	RenderableWidget	All rendered interface widgets are subclasses of $RenderableWidget$.


	class SliderWidget final : public RenderableWidget
	{
		friend class WidgetReg<SliderWidget>;

		private:

			struct SliderVertex
			{
				Point2D		position;
				ColorRGBA	color;
				Point2D		texcoord;
			};

			int32				sliderStyle;

			int32				sliderValue;
			int32				maxSliderValue;

			unsigned_int32		colorOverrideFlags;
			ColorRGBA			buttonColor;

			float				dragPosition;

			ColorRGBA			dynamicBackgroundColor;
			ColorRGBA			dynamicButtonColor;

			VertexBuffer		vertexBuffer;

			SliderWidget();
			SliderWidget(const SliderWidget& sliderWidget);

			Widget *Replicate(void) const override;

			void SetDefaultButtonColor(void);

			bool CalculateBoundingBox(Box2D *box) const override;

			int32 GetPositionValue(float x) const;

		public:

			C4API SliderWidget(const Vector2D& size);
			C4API ~SliderWidget();

			int32 GetSliderStyle(void) const
			{
				return (sliderStyle);
			}

			void SetSliderStyle(int32 style)
			{
				sliderStyle = style;
			}

			int32 GetValue(void) const
			{
				return (sliderValue);
			}

			int32 GetMaxValue(void) const
			{
				return (maxSliderValue);
			}

			C4API void SetValue(int32 value, bool post = false);
			C4API void SetMaxValue(int32 maxValue);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			const ColorRGBA& GetWidgetColor(WidgetColorType type = kWidgetColorDefault) const override;
			void SetWidgetColor(const ColorRGBA& color, WidgetColorType type = kWidgetColorDefault) override;
			void SetDynamicWidgetColor(const ColorRGBA& color, WidgetColorType type = kWidgetColorDefault) override;
			void SetDynamicWidgetAlpha(float alpha, WidgetColorType type = kWidgetColorDefault) override;

			WidgetPart TestPosition(const Point3D& position) const override;
			C4API float GetIndicatorPosition(void) const;

			void Preprocess(void) override;
			void Build(void) override;

			void HandleMouseEvent(const PanelMouseEventData *eventData) override;
	};


	//# \class	ScrollWidget	The interface widget that displays a scroll bar.
	//
	//# The $ScrollWidget$ class represents an interface widget that displays a scroll bar.
	//
	//# \def	class ScrollWidget final : public RenderableWidget
	//
	//# \ctor	ScrollWidget(const Vector2D& size);
	//
	//# \param	size	The size of the scroll bar, in pixels.
	//
	//# \desc
	//# The $ScrollWidget$ class displays a scroll bar.
	//#
	//# The default widget color corresponds to the $kWidgetColorBackground$ color type and determines the interior color
	//# of the scroll bar. The $kWidgetColorButton$ color type is also supported, and it controls the color of the up, down,
	//# and indicator buttons.
	//
	//# \base	RenderableWidget	All rendered interface widgets are subclasses of $RenderableWidget$.


	class ScrollWidget final : public RenderableWidget
	{
		friend class WidgetReg<ScrollWidget>;

		private:

			struct ScrollVertex
			{
				Point2D		position;
				ColorRGBA	color;
				Point2D		texcoord;
			};

			unsigned_int32		scrollFlags;

			int32				scrollValue;
			int32				maxScrollValue;

			int32				pageDistance;
			ColorRGBA			buttonColor;

			bool				indicatorHilite;
			bool				upButtonHilite;
			bool				downButtonHilite;
			bool				pageUpHilite;
			bool				pageDownHilite;

			float				dragPosition;
			int32				advanceTime;

			ColorRGBA			dynamicBackgroundColor;
			ColorRGBA			dynamicButtonColor;

			VertexBuffer		vertexBuffer;

			ScrollWidget();
			ScrollWidget(const ScrollWidget& scrollWidget);

			Widget *Replicate(void) const override;

			bool CalculateBoundingBox(Box2D *box) const override;

			void Advance(WidgetPart widgetPart);

		public:

			C4API ScrollWidget(const Vector2D& size);
			C4API ~ScrollWidget();

			unsigned_int32 GetScrollFlags(void) const
			{
				return (scrollFlags);
			}

			void SetScrollFlags(unsigned_int32 flags)
			{
				scrollFlags = flags;
			}

			int32 GetValue(void) const
			{
				return (scrollValue);
			}

			int32 GetMaxValue(void) const
			{
				return (maxScrollValue);
			}

			int32 GetPageDistance(void) const
			{
				return (pageDistance);
			}

			void SetPageDistance(int32 distance)
			{
				pageDistance = distance;
			}

			C4API void SetValue(int32 value, bool post = false);
			C4API void SetMaxValue(int32 maxValue);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			const ColorRGBA& GetWidgetColor(WidgetColorType type = kWidgetColorDefault) const override;
			void SetWidgetColor(const ColorRGBA& color, WidgetColorType type = kWidgetColorDefault) override;
			void SetDynamicWidgetColor(const ColorRGBA& color, WidgetColorType type = kWidgetColorDefault) override;
			void SetDynamicWidgetAlpha(float alpha, WidgetColorType type = kWidgetColorDefault) override;

			WidgetPart TestPosition(const Point3D& position) const override;
			C4API float GetIndicatorPosition(void) const;

			void Preprocess(void) override;
			void Build(void) override;

			void HandleMouseEvent(const PanelMouseEventData *eventData) override;
			void TrackTask(WidgetPart widgetPart, const Point3D& mousePosition) override;
	};


	//# \class	ListWidget		The interface widget that displays a list box.
	//
	//# The $ListWidget$ class represents an interface widget that displays a list box.
	//
	//# \def	class ListWidget : public RenderableWidget
	//
	//# \ctor	ListWidget(const Vector2D& size, float spacing = 13.0F, const char *font = "font/Gui");
	//
	//# \param	size		The size of the list box, in pixels.
	//# \param	spacing		The vertical distance from between one list item and the next.
	//# \param	font		The name of the font in which text-only list items are displayed.
	//
	//# \desc
	//# The $ListWidget$ class is used to display a box that contains a scrollable list of other widgets.
	//#
	//# The default widget color corresponds to the $kWidgetColorBorder$ color type and determines the color of the list's border.
	//# Other color types supported by the list widget are $kWidgetColorBackground$, $kWidgetColorHilite$, and $kWidgetColorFocus$.
	//# If a highlight color or focus color have not been explicitly specified, then the Interface Manager's global highlight color or focus color is used.
	//
	//# \base	RenderableWidget	All rendered interface widgets are subclasses of $RenderableWidget$.
	//
	//# \also	$@TreeWidget@$
	//# \also	$@TableWidget@$


	//# \function	ListWidget::GetListFlags		Returns the list flags.
	//
	//# \proto	unsigned_int32 GetListFlags(void) const;
	//
	//# \desc
	//# The $GetListFlags$ function returns the list flags, which can be a combination (through logical OR) of the following constants.
	//
	//# \table	ListFlags
	//
	//# The initial value of the list flags when a $ListWidget$ object is created is 0.
	//
	//# \also	$@ListWidget::SetListFlags@$


	//# \function	ListWidget::SetListFlags		Sets the list flags.
	//
	//# \proto	void SetListFlags(unsigned_int32 flags);
	//
	//# \param	flags	The new list flags. See below for possible values.
	//
	//# \desc
	//# The $SetListFlags$ function sets the list flags, which can be a combination (through logical OR) of the following constants.
	//
	//# \table	ListFlags
	//
	//# The initial value of the list flags when a $ListWidget$ object is created is 0.
	//
	//# \also	$@ListWidget::GetListFlags@$


	//# \function	ListWidget::GetItemSpacing		Returns the vertical list item spacing.
	//
	//# \proto	float GetItemSpacing(void) const;
	//
	//# \desc
	//# The $GetItemSpacing$ function returns the vertical distance from the top of one list item to the top of the next list item.
	//
	//# \also	$@ListWidget::SetItemSpacing@$
	//# \also	$@ListWidget::GetNaturalListItemSize@$


	//# \function	ListWidget::SetItemSpacing		Sets the vertical list item spacing.
	//
	//# \proto	void SetItemSpacing(float spacing);
	//
	//# \param	spacing		The new item spacing.
	//
	//# \desc
	//# The $SetItemSpacing$ function sets the vertical distance from the top of one list item to the top of the next list item.
	//
	//# \also	$@ListWidget::GetItemSpacing@$
	//# \also	$@ListWidget::GetNaturalListItemSize@$


	//# \function	ListWidget::GetNaturalListItemSize		Returns the natural size for a list item.
	//
	//# \proto	Vector2D GetNaturalListItemSize(void) const;
	//
	//# \desc
	//# The $GetNaturalListItemSize$ function returns the size that a list item should have in order to perfectly fill the
	//# horizontal and vertical space allocated for each item. The horizontal size, in the <i>x</i> coordinate of the returne value,
	//# accounts for the scroll bar and list item offset. The vertical size, in the <i>y</i> coordinate of the return value, is
	//# always equal to the list item spacing.
	//
	//# \also	$@ListWidget::GetItemSpacing@$
	//# \also	$@ListWidget::SetItemSpacing@$


	//# \div
	//# \function	ListWidget::GetListItemCount		Returns the number of items in a list.
	//
	//# \proto	int32 GetListItemCount(void) const;
	//
	//# \desc
	//# The $GetListItemCount$ function returns the number of items in a list.
	//
	//# \also	$@ListWidget::GetListItem@$
	//# \also	$@ListWidget::GetFirstListItem@$
	//# \also	$@ListWidget::GetLastListItem@$


	//# \function	ListWidget::GetListItem				Returns the list item with a specific index.
	//
	//# \proto	Widget *GetListItem(int32 index) const;
	//
	//# \param	index	The index of the list item to return, where the first list item has the index 0.
	//
	//# \desc
	//# The $GetListItem$ function returns a pointer to the list item whose zero-based index is specified by
	//# the $index$ parameter. If the value of $index$ is greater than or equal to the number of items in
	//# the list (or if $index$ is negative), then the return value is $nullptr$.
	//
	//# \also	$@ListWidget::GetFirstListItem@$
	//# \also	$@ListWidget::GetLastListItem@$
	//# \also	$@ListWidget::GetListItemCount@$


	//# \function	ListWidget::GetFirstListItem		Returns the first item in a list.
	//
	//# \proto	Widget *GetFirstListItem(void) const;
	//
	//# \desc
	//# The $GetFirstListItem$ function returns a pointer to the first item in a list. If the list is empty,
	//# then the return value is $nullptr$. The remaining items in a list can be iterated by calling the
	//# $@Utilities/Tree::Next@$ function for the returned widget.
	//
	//# \also	$@ListWidget::GetLastListItem@$
	//# \also	$@ListWidget::GetListItem@$
	//# \also	$@ListWidget::GetListItemCount@$


	//# \function	ListWidget::GetLastListItem			Returns the last item in a list.
	//
	//# \proto	Widget *GetLastListItem(void) const;
	//
	//# \desc
	//# The $GetLastListItem$ function returns a pointer to the last item in a list. If the list is empty,
	//# then the return value is $nullptr$. The remaining items in a list can be iterated by calling the
	//# $@Utilities/Tree::Previous@$ function for the returned widget.
	//
	//# \also	$@ListWidget::GetFirstListItem@$
	//# \also	$@ListWidget::GetListItem@$
	//# \also	$@ListWidget::GetListItemCount@$


	//# \function	ListWidget::SetDisplayIndex		Scrolls a list widget so that a specific item is displayed at the top.
	//
	//# \proto	void SetDisplayIndex(int32 index);
	//
	//# \param	index	The index of the item that should become the first displayed.
	//
	//# \desc
	//# The $SetDisplayIndex$ function scrolls a list widget so that the item having the zero-baed index
	//# specified by the $index$ parameter becomes the first item displayed at the top of the list. This function
	//# makes the best attempt to place this item at the top of the display area, but the index is clamped to the
	//# range of values allowed by the overall number of items in the list.
	//
	//# \also	$@ListWidget::RevealListItem@$
	//# \also	$@ListWidget::GetListItemCount@$


	//# \function	ListWidget::RevealListItem		Scrolls a list widget to reveal a specific item.
	//
	//# \proto	void RevealListItem(int32 index);
	//
	//# \param	index	The index of the item to reveal.
	//
	//# \desc
	//# The $RevealListItem$ function scrolls a list widget to reveal the item having the zero-baed index
	//# specified by the $index$ parameter. The list widget is scrolled far enough to place the revealed
	//# item at the vertical center of the list if possible.
	//#
	//# If the $index$ parameter is not in the range [0,&nbsp;<i>n</i>&nbsp;&minus;1], where <i>n</i> is the number of
	//# items in the list, then no change is made to the appearance of the list widget.
	//
	//# \also	$@ListWidget::SetDisplayIndex@$
	//# \also	$@ListWidget::GetListItemCount@$


	//# \div
	//# \function	ListWidget::GetSelectedListItemCount	Returns the number of items that are currently selected in a list.
	//
	//# \proto	int32 GetSelectedListItemCount(void) const;
	//
	//# \desc
	//# The $GetSelectedListItemCount$ function returns the number of items that are currently selected in a list.
	//
	//# \also	$@ListWidget::GetSelectedListItem@$
	//# \also	$@ListWidget::GetFirstSelectedListItem@$
	//# \also	$@ListWidget::GetLastSelectedListItem@$


	//# \function	ListWidget::GetSelectedListItem			Returns the selected list item with a specific index.
	//
	//# \proto	Widget *GetSelectedListItem(int32 index) const;
	//
	//# \param	index	The index of the selected list item to return, where the first selected list item has the index 0.
	//
	//# \desc
	//# The $GetSelectedListItem$ function returns a pointer to the selected list item whose zero-based index among all selected
	//# list items is specified by the $index$ parameter. If the value of $index$ is greater than or equal to the number of
	//# selected items in the list (or if $index$ is negative), then the return value is $nullptr$.
	//#
	//# Selected list items are indexed in the order in which they appear in the list, and not in the order in which they
	//# were selected.
	//
	//# \also	$@ListWidget::GetFirstSelectedListItem@$
	//# \also	$@ListWidget::GetLastSelectedListItem@$
	//# \also	$@ListWidget::GetSelectedListItemCount@$


	//# \function	ListWidget::GetFirstSelectedListItem	Returns the first selected item in a list.
	//
	//# \proto	Widget *GetFirstSelectedListItem(void) const;
	//
	//# \desc
	//# The $GetFirstSelectedListItem$ function returns a pointer to the first selected item in a list. If there are no
	//# selected items in the list, then the return value is $nullptr$. The remaining selected items in a list can be iterated
	//# by calling the $@ListWidget::GetNextSelectedListItem@$ function for the returned widget.
	//
	//# \also	$@ListWidget::GetNextSelectedListItem@$
	//# \also	$@ListWidget::GetPreviousSelectedListItem@$
	//# \also	$@ListWidget::GetLastSelectedListItem@$
	//# \also	$@ListWidget::GetSelectedListItem@$
	//# \also	$@ListWidget::GetSelectedListItemCount@$


	//# \function	ListWidget::GetLastSelectedListItem		Returns the last selected item in a list.
	//
	//# \proto	Widget *GetLastSelectedListItem(void) const;
	//
	//# \desc
	//# The $GetLastSelectedListItem$ function returns a pointer to the last selected item in a list. If there are no
	//# selected items in the list, then the return value is $nullptr$. The remaining selected items in a list can be iterated
	//# by calling the $@ListWidget::GetPreviousSelectedListItem@$ function for the returned widget.
	//
	//# \also	$@ListWidget::GetPreviousSelectedListItem@$
	//# \also	$@ListWidget::GetNextSelectedListItem@$
	//# \also	$@ListWidget::GetFirstSelectedListItem@$
	//# \also	$@ListWidget::GetSelectedListItem@$
	//# \also	$@ListWidget::GetSelectedListItemCount@$


	//# \function	ListWidget::GetPreviousSelectedListItem		Returns the previous selected item in a list.
	//
	//# \proto	Widget *GetPreviousSelectedListItem(const Widget *widget) const;
	//
	//# \param	widget		A pointer to a selected item for which the previous selected item is returned.
	//
	//# \desc
	//# The $GetPreviousSelectedListItem$ function returns a pointer to the selected list item that precedes the selected
	//# item specified by the $widget$ parameter. If there is no previous selected item (i.e., the item specified by $widget$
	//# is the first selected item), then the return value is $nullptr$.
	//
	//# \also	$@ListWidget::GetNextSelectedListItem@$
	//# \also	$@ListWidget::GetFirstSelectedListItem@$
	//# \also	$@ListWidget::GetLastSelectedListItem@$
	//# \also	$@ListWidget::GetSelectedListItem@$
	//# \also	$@ListWidget::GetSelectedListItemCount@$


	//# \function	ListWidget::GetNextSelectedListItem		Returns the next selected item in a list.
	//
	//# \proto	Widget *GetNextSelectedListItem(const Widget *widget) const;
	//
	//# \param	widget		A pointer to a selected item for which the next selected item is returned.
	//
	//# \desc
	//# The $GetNextSelectedListItem$ function returns a pointer to the selected list item that follows the selected
	//# item specified by the $widget$ parameter. If there is no next selected item (i.e., the item specified by $widget$
	//# is the last selected item), then the return value is $nullptr$.
	//
	//# \also	$@ListWidget::GetPreviousSelectedListItem@$
	//# \also	$@ListWidget::GetFirstSelectedListItem@$
	//# \also	$@ListWidget::GetLastSelectedListItem@$
	//# \also	$@ListWidget::GetSelectedListItem@$
	//# \also	$@ListWidget::GetSelectedListItemCount@$


	//# \function	ListWidget::SelectListItem		Selects an item in a list.
	//
	//# \proto	void SelectListItem(int32 index, bool post = false);
	//
	//# \param	index	The index of the item to select.
	//# \param	post	Indicates whether to post a $kEventWidgetChange$ event.
	//
	//# \desc
	//# The $SelectListItem$ function selects the list item whose zero-based index is specified by the $index$ parameter.
	//# If the $index$ parameter is not in the range [0,&nbsp;<i>n</i>&nbsp;&minus;1], where <i>n</i> is the number of
	//# items in the list, then no new selection is made.
	//#
	//# If the $kListMultipleSelection$ is not set for the list widget, then all list items having an index other than
	//# that specified by the $index$ parameter are unselected. In the case that the index is out of range, this means
	//# that all of the list items are unselected.
	//#
	//# If the $post$ parameter is $true$ and a change was actually made to the selection, then the list widget posts
	//# a $kEventWidgetChange$ event that can be handled by its observers.
	//
	//# \also	$@ListWidget::UnselectListItem@$
	//# \also	$@ListWidget::SelectAllListItems@$
	//# \also	$@ListWidget::UnselectAllListItems@$
	//# \also	$@ListWidget::GetFirstSelectedListItem@$
	//# \also	$@ListWidget::GetLastSelectedListItem@$
	//# \also	$@ListWidget::GetSelectedListItem@$
	//# \also	$@ListWidget::GetSelectedListItemCount@$
	//# \also	$@ListWidget::GetListItemCount@$
	//# \also	$@ListWidget::GetListFlags@$
	//# \also	$@ListWidget::SetListFlags@$
	//# \also	$@ListWidget::RevealListItem@$


	//# \function	ListWidget::UnselectListItem		Unselects an item in a list.
	//
	//# \proto	void UnselectListItem(int32 index, bool post = false);
	//
	//# \param	index	The index of the item to unselect.
	//# \param	post	Indicates whether to post a $kEventWidgetChange$ event.
	//
	//# \desc
	//# The $UnselectListItem$ function unselects the list item whose zero-based index is specified by the $index$ parameter.
	//# If the $index$ parameter is not in the range [0,&nbsp;<i>n</i>&nbsp;&minus;1], where <i>n</i> is the number of
	//# items in the list, then no change to the current selection is made.
	//#
	//# If the $post$ parameter is $true$ and a change was actually made to the selection, then the list widget posts
	//# a $kEventWidgetChange$ event that can be handled by its observers.
	//
	//# \also	$@ListWidget::SelectListItem@$
	//# \also	$@ListWidget::SelectAllListItems@$
	//# \also	$@ListWidget::UnselectAllListItems@$
	//# \also	$@ListWidget::GetFirstSelectedListItem@$
	//# \also	$@ListWidget::GetLastSelectedListItem@$
	//# \also	$@ListWidget::GetSelectedListItem@$
	//# \also	$@ListWidget::GetSelectedListItemCount@$
	//# \also	$@ListWidget::GetListItemCount@$
	//# \also	$@ListWidget::GetListFlags@$
	//# \also	$@ListWidget::SetListFlags@$
	//# \also	$@ListWidget::RevealListItem@$


	//# \function	ListWidget::SelectAllListItems		Selects all items in a list.
	//
	//# \proto	void SelectAllListItems(bool post = false);
	//
	//# \param	post	Indicates whether to post a $kEventWidgetChange$ event.
	//
	//# \desc
	//# The $SelectAllListItems$ function selects all of the items in a list. If any list items were previously
	//# not selected and the $post$ parameter is $true$, then the list widget posts a $kEventWidgetChange$ event
	//# that can be handled by its observers.
	//#
	//# If the $kListMultipleSelection$ is not set for the list widget, then only the first item in the list is
	//# selected, and an event is posted only if this changes the current selection.
	//
	//# \also	$@ListWidget::UnselectAllListItems@$
	//# \also	$@ListWidget::SelectListItem@$
	//# \also	$@ListWidget::UnselectListItem@$
	//# \also	$@ListWidget::GetFirstSelectedListItem@$
	//# \also	$@ListWidget::GetLastSelectedListItem@$
	//# \also	$@ListWidget::GetSelectedListItem@$
	//# \also	$@ListWidget::GetSelectedListItemCount@$
	//# \also	$@ListWidget::GetListItemCount@$
	//# \also	$@ListWidget::GetListFlags@$
	//# \also	$@ListWidget::SetListFlags@$
	//# \also	$@ListWidget::RevealListItem@$


	//# \function	ListWidget::UnselectAllListItems		Unselects all items in a list.
	//
	//# \proto	void UnselectAllListItems(bool post = false);
	//
	//# \param	post	Indicates whether to post a $kEventWidgetChange$ event.
	//
	//# \desc
	//# The $UnselectAllListItems$ function unselects all of the items in a list. If any list items were previously
	//# selected and the $post$ parameter is $true$, then the list widget posts a $kEventWidgetChange$ event
	//# that can be handled by its observers.
	//
	//# \also	$@ListWidget::SelectAllListItems@$
	//# \also	$@ListWidget::SelectListItem@$
	//# \also	$@ListWidget::UnselectListItem@$
	//# \also	$@ListWidget::GetFirstSelectedListItem@$
	//# \also	$@ListWidget::GetLastSelectedListItem@$
	//# \also	$@ListWidget::GetSelectedListItem@$
	//# \also	$@ListWidget::GetSelectedListItemCount@$
	//# \also	$@ListWidget::GetListItemCount@$
	//# \also	$@ListWidget::GetListFlags@$
	//# \also	$@ListWidget::SetListFlags@$
	//# \also	$@ListWidget::RevealListItem@$


	//# \div
	//# \function	ListWidget::PrependListItem		Adds an item to the beginning of a list.
	//
	//# \proto	void PrependListItem(Widget *widget);
	//# \proto	TextWidget *PrependListItem(const char *text);
	//
	//# \param	widget		The widget to add to the list.
	//# \param	text		The text used to add a text widget to the list.
	//
	//# \desc
	//# The $PrependListItem$ function adds an item to the beginning of a list. If the $widget$ parameter is
	//# specified, then that widget is added directly to the list. If the $text$ parameter is specified, then
	//# a new $@TextWidget@$ item is created with the text string and the default font and added to the list.
	//# In this case, a pointer to the new text widget is returned.
	//#
	//# Note that unless that $kListItemsEnabled$ flag has been set with the $@ListWidget::SetListFlags@$ function,
	//# the widget added to the list is disabled before the $PrependListItem$ function returns so that clicks go
	//# through to the list widget. To allow a widget in a list or any of its subwidgets to receive events,
	//# call the $@Widget::Enable@$ function for that widget after adding it to the list.
	//
	//# \also	$@ListWidget::AppendListItem@$


	//# \function	ListWidget::AppendListItem		Adds an item to the end of a list.
	//
	//# \proto	void AppendListItem(Widget *widget);
	//# \proto	TextWidget *AppendListItem(const char *text);
	//
	//# \param	widget		The widget to add to the list.
	//# \param	text		The text used to add a text widget to the list.
	//
	//# \desc
	//# The $AppendListItem$ function adds an item to the end of a list. If the $widget$ parameter is
	//# specified, then that widget is added directly to the list. If the $text$ parameter is specified, then
	//# a new $@TextWidget@$ item is created with the text string and the default font and added to the list.
	//# In this case, a pointer to the new text widget is returned.
	//#
	//# Note that unless that $kListItemsEnabled$ flag has been set with the $@ListWidget::SetListFlags@$ function,
	//# the widget added to the list is disabled before the $AppendListItem$ function returns so that clicks go
	//# through to the list widget. To allow a widget in a list or any of its subwidgets to receive events,
	//# call the $@Widget::Enable@$ function for that widget after adding it to the list.
	//
	//# \also	$@ListWidget::PrependListItem@$


	class ListWidget : public RenderableWidget
	{
		friend class WidgetReg<ListWidget>;

		private:

			enum
			{
				kListUpdatePlacement	= 1 << 0,
				kListUpdateVisibility	= 1 << 1,
				kListUpdateSelection	= 1 << 2
			};

			struct ListVertex
			{
				Point2D		position;
				ColorRGBA	color;
			};

			unsigned_int32					listFlags;
			float							itemSpacing;
			Vector2D						itemOffset;

			unsigned_int32					colorOverrideFlags;
			ColorRGBA						backgroundColor;
			ColorRGBA						hiliteColor;
			ColorRGBA						focusColor;

			ResourceName					fontName;

			bool							preprocessFlag;
			unsigned_int16					listUpdateFlags;

			int32							listItemCount;
			int32							clickItemIndex;

			int32							displayItemCount;
			int32							displayItemIndex;
			Widget							*displayItem;

			Point2D							*selectionVertex;

			ColorRGBA						dynamicBackgroundColor;
			ColorRGBA						dynamicHiliteColor;

			WidgetObserver<ListWidget>		scrollObserver;

			VertexBuffer					listVertexBuffer;
			VertexBuffer					structureVertexBuffer;

			List<Attribute>					borderAttributeList;
			DiffuseAttribute				borderColorAttribute;
			DiffuseTextureAttribute			borderTextureAttribute;
			Renderable						borderRenderable;

			List<Attribute>					focusAttributeList;
			DiffuseAttribute				focusColorAttribute;
			Renderable						focusRenderable;

			Widget							itemGroup;
			ScrollWidget					scrollWidget;

			void SetListUpdateFlags(unsigned_int32 flags)
			{
				listUpdateFlags |= flags;
				Invalidate();
			}

			Widget *Replicate(void) const override;

			void SetDefaultHiliteColor(void);
			void SetDefaultFocusColor(void);

			void UpdatePlacement(void);
			void UpdateVisibility(void);
			void UpdateSelection(void);

			void HandleStructureUpdate(void) override;

			void HandleScrollEvent(Widget *widget, const WidgetEventData *eventData);

			void HandleUpArrow(unsigned_int32 modifierKeys);
			void HandleDownArrow(unsigned_int32 modifierKeys);

		protected:

			ListWidget(WidgetType type = kWidgetList);
			ListWidget(WidgetType type, const Vector2D& size, float spacing, const char *font);
			ListWidget(const ListWidget& listWidget);

		public:

			C4API ListWidget(const Vector2D& size, float spacing = 13.0F, const char *font = "font/Gui");
			C4API ~ListWidget();

			unsigned_int32 GetListFlags(void) const
			{
				return (listFlags);
			}

			void SetListFlags(unsigned_int32 flags)
			{
				listFlags = flags;
			}

			float GetItemSpacing(void) const
			{
				return (itemSpacing);
			}

			void SetItemSpacing(float spacing)
			{
				itemSpacing = spacing;
			}

			const Vector2D& GetItemOffset(void) const
			{
				return (itemOffset);
			}

			void SetItemOffset(const Vector2D& offset)
			{
				itemOffset = offset;
			}

			Vector2D GetNaturalListItemSize(void) const
			{
				return (Vector2D(GetWidgetSize().x - itemOffset.x - 16.0F, itemSpacing));
			}

			const ResourceName& GetFontName(void) const
			{
				return (fontName);
			}

			void SetFont(const char *font)
			{
				fontName = font;
			}

			int32 GetListItemCount(void) const
			{
				return (listItemCount);
			}

			Widget *GetFirstListItem(void) const
			{
				return (itemGroup.GetFirstSubnode());
			}

			Widget *GetLastListItem(void) const
			{
				return (itemGroup.GetLastSubnode());
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			const ColorRGBA& GetWidgetColor(WidgetColorType type = kWidgetColorDefault) const override;
			void SetWidgetColor(const ColorRGBA& color, WidgetColorType type = kWidgetColorDefault) override;
			void SetWidgetAlpha(float alpha, WidgetColorType type = kWidgetColorDefault) override;
			void SetDynamicWidgetColor(const ColorRGBA& color, WidgetColorType type = kWidgetColorDefault) override;
			void SetDynamicWidgetAlpha(float alpha, WidgetColorType type = kWidgetColorDefault) override;

			void SetWidgetSize(const Vector2D& size) override;
			WidgetPart TestPosition(const Point3D& position) const override;
			void Preprocess(void) override;

			void EnterForeground(void) override;
			void EnterBackground(void) override;

			void Build(void) override;
			void Render(List<Renderable> *renderList) override;

			void HandleMouseEvent(const PanelMouseEventData *eventData) override;
			bool HandleKeyboardEvent(const KeyboardEventData *eventData) override;

			C4API Widget *GetListItem(int32 index) const;
			C4API void RevealListItem(int32 index);

			C4API TextWidget *FindListItem(const char *text, Text::TextComparisonFunction *comparisonFunction = &Text::CompareTextCaseless) const;

			C4API int32 GetActiveListItemCount(void) const;
			C4API Widget *GetActiveListItem(int32 index) const;
			C4API Widget *GetFirstActiveListItem(void) const;
			C4API Widget *GetLastActiveListItem(void) const;
			C4API Widget *GetPreviousActiveListItem(const Widget *widget) const;
			C4API Widget *GetNextActiveListItem(const Widget *widget) const;

			C4API void PrependListItem(Widget *widget);
			C4API void AppendListItem(Widget *widget);
			C4API void InsertListItemBefore(Widget *widget, Widget *before);
			C4API void InsertListItemAfter(Widget *widget, Widget *after);
			C4API void InsertSortedListItem(TextWidget *widget, Text::TextComparisonFunction *comparisonFunction = &Text::CompareTextLessThanCaseless);

			C4API TextWidget *PrependListItem(const char *text);
			C4API TextWidget *AppendListItem(const char *text);
			C4API TextWidget *InsertListItemBefore(const char *text, Widget *before);
			C4API TextWidget *InsertListItemAfter(const char *text, Widget *after);
			C4API TextWidget *InsertSortedListItem(const char *text, Text::TextComparisonFunction *comparisonFunction = &Text::CompareTextLessThanCaseless);

			C4API bool RemoveListItem(Widget *widget);
			C4API void PurgeListItems(void);
			C4API void PurgeSelectedListItems(void);

			C4API int32 GetSelectedListItemCount(void) const;
			C4API Widget *GetSelectedListItem(int32 index) const;
			C4API Widget *GetFirstSelectedListItem(void) const;
			C4API Widget *GetLastSelectedListItem(void) const;
			C4API Widget *GetPreviousSelectedListItem(const Widget *widget) const;
			C4API Widget *GetNextSelectedListItem(const Widget *widget) const;

			C4API int32 GetFirstSelectedIndex(void) const;
			C4API int32 GetLastSelectedIndex(void) const;

			C4API void SelectListItem(int32 index, bool post = false);
			C4API void UnselectListItem(int32 index, bool post = false);
			C4API void SelectAllListItems(bool post = false);
			C4API void UnselectAllListItems(bool post = false);

			C4API void SetDisplayIndex(int32 index);
	};


	//# \class	TreeItemWidget		The interface widget that encapsulates items in a tree list.
	//
	//# The $TreeItemWidget$ class represents an interface widget that encapsulates items in a tree list.
	//
	//# \def	class TreeItemWidget : public RenderableWidget, public Tree<TreeItemWidget>
	//
	//# \ctor	TreeItemWidget(Widget *widget);
	//# \ctor	TreeItemWidget(const char *text, const char *font = "font/Gui");
	//
	//# \param	widget		The widget that is displayed in the tree.
	//# \param	text		The text used to create a $@TextWidget@$ to be displayed in the tree.
	//# \param	font		The name of the font in which text-only tree items are displayed.
	//
	//# \desc
	//# The $TreeItemWidget$ class encapsulates each item that is to be displayed inside a $@TreeWidget@$.
	//
	//# \base	RenderableWidget		All rendered interface widgets are subclasses of $RenderableWidget$.
	//# \base	Tree<TreeItemWidget>	Tree item widgets are organized as a tree structure.
	//
	//# \also	$@TreeWidget@$


	class TreeItemWidget : public RenderableWidget, public Tree<TreeItemWidget>
	{
		friend class TreeWidget;

		private:

			struct LineVertex
			{
				Point2D			position;
				Point2D			texcoord;
			};

			TreeWidget					*treeWidget;
			Widget						*itemWidget;

			VertexBuffer				vertexBuffer;
			List<Attribute>				attributeList;
			DiffuseAttribute			colorAttribute;
			DiffuseTextureAttribute		textureAttribute;

			GuiButtonWidget						collapseButton;
			WidgetObserver<TreeItemWidget>		collapseObserver;

			void HandleCollapseEvent(Widget *widget, const WidgetEventData *eventData);

		public:

			C4API TreeItemWidget(Widget *widget);
			C4API TreeItemWidget(const char *text, const char *font = "font/Gui");
			C4API ~TreeItemWidget();

			Widget *GetItemWidget(void) const
			{
				return (itemWidget);
			}

			using Tree<TreeItemWidget>::AppendSubnode;
			using Tree<TreeItemWidget>::PrependSubnode;
			using Tree<TreeItemWidget>::InsertSubnodeBefore;
			using Tree<TreeItemWidget>::InsertSubnodeAfter;

			C4API void SetDynamicWidgetColor(const ColorRGBA& color, WidgetColorType type = kWidgetColorDefault) override;
			C4API void SetDynamicWidgetAlpha(float alpha, WidgetColorType type = kWidgetColorDefault) override;

			C4API void Preprocess(void) override;
			C4API void Build(void) override;
	};


	//# \class	TreeWidget		The interface widget that displays a tree list.
	//
	//# The $TreeWidget$ class represents an interface widget that displays a tree list.
	//
	//# \def	class TreeWidget final : public ListWidget
	//
	//# \ctor	TreeWidget(const Vector2D& size, float spacing = 13.0F, const char *font = "font/Gui");
	//
	//# \param	size		The size of the tree box, in pixels.
	//# \param	spacing		The vertical distance from between one tree item and the next.
	//# \param	font		The name of the font in which text-only tree items are displayed.
	//
	//# \desc
	//# The $TreeWidget$ class is used to display a box that contains a hierarchical tree of other widgets shown in a scrollable list.
	//# The members of a tree widget are always widgets of the type $@TreeItemWidget@$.
	//#
	//# The default widget color corresponds to the $kWidgetColorBorder$ color type and determines the color of the tree's border.
	//# Other color types supported by the tree widget are $kWidgetColorBackground$, $kWidgetColorHilite$, and $kWidgetColorFocus$.
	//# If a highlight color or focus color have not been explicitly specified, then the Interface Manager's global highlight color or focus color is used.
	//
	//# \base	ListWidget		The tree widget is a specific type of $ListWidget$.
	//
	//# \also	$@TreeItemWidget@$
	//# \also	$@TableWidget@$


	//# \function	TreeWidget::GetIndentSpacing		Returns the subitem indent spacing.
	//
	//# \proto	float GetIndentSpacing(void) const;
	//
	//# \desc
	//# The $GetIndentSpacing$ function returns the horizontal distance by which subitems are intended relative to their parent items.
	//
	//# \also	$@TreeWidget::SetIndentSpacing@$


	//# \function	TreeWidget::SetIndentSpacing		Sets the subitem indent spacing.
	//
	//# \proto	void SetIndentSpacing(float spacing);
	//
	//# \param	spacing		The new indent spacing.
	//
	//# \desc
	//# The $SetIndentSpacing$ function sets the horizontal distance by which subitems are intended relative to their parent items.
	//
	//# \also	$@TreeWidget::GetIndentSpacing@$


	//# \function	TreeWidget::PrependTreeItem		Adds an item to the beginning of a tree.
	//
	//# \proto	void PrependListItem(TreeItemWidget *item);
	//
	//# \param	item	The item to add to the tree.
	//
	//# \desc
	//# The $PrependTreeItem$ function adds an item to the beginning of a tree at the top level of the hierarchy.
	//#
	//# Note that unless that $kListItemsEnabled$ flag has been set with the $@ListWidget::SetListFlags@$ function,
	//# the item added to the tree is disabled before the $PrependTreeItem$ function returns so that clicks go
	//# through to the tree widget. To allow an item in a tree or any of its subwidgets to receive events,
	//# call the $@Widget::Enable@$ function for that item after adding it to the tree.
	//
	//# \also	$@TreeWidget::AppendTreeItem@$


	//# \function	TreeWidget::AppendTreeItem		Adds an item to the end of a tree.
	//
	//# \proto	void AppendTreeItem(TreeItemWidget *item);
	//
	//# \param	item	The item to add to the tree.
	//
	//# \desc
	//# The $AppendTreeItem$ function adds an item to the end of a tree at the top level of the hierarchy.
	//#
	//# Note that unless that $kListItemsEnabled$ flag has been set with the $@ListWidget::SetListFlags@$ function,
	//# the item added to the tree is disabled before the $AppendTreeItem$ function returns so that clicks go
	//# through to the tree widget. To allow an item in a tree or any of its subwidgets to receive events,
	//# call the $@Widget::Enable@$ function for that item after adding it to the tree.
	//
	//# \also	$@TreeWidget::PrependTreeItem@$


	class TreeWidget final : public ListWidget
	{
		friend class WidgetReg<TreeWidget>;

		private:

			float		indentSpacing;

			TreeWidget();
			TreeWidget(const TreeWidget& treeWidget);

			using ListWidget::PrependListItem;
			using ListWidget::AppendListItem;
			using ListWidget::InsertListItemBefore;
			using ListWidget::InsertListItemAfter;
			using ListWidget::InsertSortedListItem;

			Widget *Replicate(void) const override;

			TreeItemWidget *InsertSubtreeItems(TreeItemWidget *item);
			bool RemoveSubtreeItems(TreeItemWidget *item);

		public:

			C4API TreeWidget(const Vector2D& size, float spacing = 13.0F, const char *font = "font/Gui");
			C4API ~TreeWidget();

			float GetIndentSpacing(void) const
			{
				return (indentSpacing);
			}

			void SetIndentSpacing(float spacing)
			{
				indentSpacing = spacing;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			C4API void PrependTreeItem(TreeItemWidget *item);
			C4API void AppendTreeItem(TreeItemWidget *item);
			C4API void InsertTreeItemBefore(TreeItemWidget *item, TreeItemWidget *before);
			C4API void InsertTreeItemAfter(TreeItemWidget *item, TreeItemWidget *after);

			C4API void ExpandTreeItem(TreeItemWidget *item);
			C4API void CollapseTreeItem(TreeItemWidget *item, bool post = false);
	};


	//# \class	TableWidget		The interface widget that displays a table.
	//
	//# The $TableWidget$ class represents an interface widget that displays a table.
	//
	//# \def	class TableWidget final : public RenderableWidget
	//
	//# \ctor	TableWidget(const Vector2D& size, int32 columns, const Vector2D& cell);
	//
	//# \param	size		The size of the table, in pixels.
	//# \param	columns		The number of columns in the table.
	//# \param	cell		The size of each cell in the table, in pixels.
	//
	//# \desc
	//# The $TableWidget$ class is used to display a box that contains a scrollable table of other widgets. The table has a fixed number
	//# of columns as specified by the $columns$ parameter.
	//#
	//# The default widget color corresponds to the $kWidgetColorBorder$ color type and determines the color of the table's border.
	//# Other color types supported by the table widget are $kWidgetColorBackground$, $kWidgetColorHilite$, and $kWidgetColorFocus$.
	//# If a highlight color or focus color have not been explicitly specified, then the Interface Manager's global highlight color or focus color is used.
	//
	//# \base	RenderableWidget	All rendered interface widgets are subclasses of $RenderableWidget$.
	//
	//# \also	$@ListWidget@$
	//# \also	$@TreeWidget@$


	//# \function	TableWidget::GetTableFlags		Returns the table flags.
	//
	//# \proto	unsigned_int32 GetTableFlags(void) const;
	//
	//# \desc
	//# The $GetTableFlags$ function returns the table flags, which can be a combination (through logical OR) of the following constants.
	//
	//# \table	TableFlags
	//
	//# The initial value of the table flags when a $TableWidget$ object is created is 0.
	//
	//# \also	$@TableWidget::SetTableFlags@$


	//# \function	TableWidget::SetTableFlags		Sets the table flags.
	//
	//# \proto	void SetTableFlags(unsigned_int32 flags);
	//
	//# \param	flags	The new table flags. See below for possible values.
	//
	//# \desc
	//# The $SetTableFlags$ function sets the table flags, which can be a combination (through logical OR) of the following constants.
	//
	//# \table	TableFlags
	//
	//# The initial value of the table flags when a $TableWidget$ object is created is 0.
	//
	//# \also	$@TableWidget::GetTableFlags@$


	//# \function	TableWidget::GetColumnCount		Returns the number of columns in a table.
	//
	//# \proto	int32 GetColumnCount(void) const;
	//
	//# \desc
	//# The $GetColumnCount$ function returns the number of columns in a table.
	//
	//# \also	$@TableWidget::SetColumnCount@$
	//# \also	$@TableWidget::GetCellSize@$
	//# \also	$@TableWidget::SetCellSize@$


	//# \function	TableWidget::SetColumnCount		Sets the number of columns in a table.
	//
	//# \proto	void SetColumnCount(int32 columns) const;
	//
	//# \param	columns		The new column count for the table.
	//
	//# \desc
	//# The $GetColumnCount$ function sets the number of columns in a table to that specified by the $columns$ parameter.
	//
	//# \also	$@TableWidget::GetColumnCount@$
	//# \also	$@TableWidget::GetCellSize@$
	//# \also	$@TableWidget::SetCellSize@$


	//# \function	TableWidget::GetCellSize		Returns the cell size for a table.
	//
	//# \proto	const Vector2D& GetCellSize(void) const;
	//
	//# \desc
	//# The $GetCellSize$ function returns the cell size for a table.
	//
	//# \also	$@TableWidget::SetCellSize@$


	//# \function	TableWidget::SetCellSize		Sets the cell size for a table.
	//
	//# \proto	void SetCellSize(const Vector2D& cell);
	//
	//# \param	cell	The new cell size.
	//
	//# \desc
	//# The $SetCellSize$ function sets the cell size for a table to that specified by the $cell$ parameter.
	//
	//# \also	$@TableWidget::GetCellSize@$


	//# \div
	//# \function	TableWidget::GetTableItemCount		Returns the number of items in a table.
	//
	//# \proto	int32 GetTableItemCount(void) const;
	//
	//# \desc
	//# The $GetTableItemCount$ function returns the number of items in a table.
	//
	//# \also	$@TableWidget::GetTableItem@$
	//# \also	$@TableWidget::GetFirstTableItem@$
	//# \also	$@TableWidget::GetLastTableItem@$


	//# \function	TableWidget::GetTableItem			Returns the table item with a specific index.
	//
	//# \proto	Widget *GetTableItem(int32 index) const;
	//
	//# \param	index	The index of the table item to return, where the first table item has the index 0.
	//
	//# \desc
	//# The $GetTableItem$ function returns a pointer to the table item whose zero-based index is specified by
	//# the $index$ parameter. If the value of $index$ is greater than or equal to the number of items in
	//# the table (or if $index$ is negative), then the return value is $nullptr$.
	//
	//# \also	$@TableWidget::GetFirstTableItem@$
	//# \also	$@TableWidget::GetLastTableItem@$
	//# \also	$@TableWidget::GetTableItemCount@$


	//# \function	TableWidget::GetFirstTableItem		Returns the first item in a table.
	//
	//# \proto	Widget *GetFirstTableItem(void) const;
	//
	//# \desc
	//# The $GetFirstTableItem$ function returns a pointer to the first item in a table. If the table is empty,
	//# then the return value is $nullptr$. The remaining items in a table can be iterated by calling the
	//# $@Utilities/Tree::Next@$ function for the returned widget.
	//
	//# \also	$@TableWidget::GetLastTableItem@$
	//# \also	$@TableWidget::GetTableItem@$
	//# \also	$@TableWidget::GetTableItemCount@$


	//# \function	TableWidget::GetLastTableItem		Returns the last item in a table.
	//
	//# \proto	Widget *GetLastTableItem(void) const;
	//
	//# \desc
	//# The $GetLastTableItem$ function returns a pointer to the last item in a table. If the table is empty,
	//# then the return value is $nullptr$. The remaining items in a table can be iterated by calling the
	//# $@Utilities/Tree::Previous@$ function for the returned widget.
	//
	//# \also	$@TableWidget::GetFirstTableItem@$
	//# \also	$@TableWidget::GetTableItem@$
	//# \also	$@TableWidget::GetTableItemCount@$


	//# \function	TableWidget::SetDisplayRow		Scrolls a table widget so that a specific row is displayed at the top.
	//
	//# \proto	void SetDisplayRow(int32 row);
	//
	//# \param	row 	The row that should become the first displayed.
	//
	//# \desc
	//# The $SetDisplayRow$ function scrolls a table widget so that the row having the zero-baed index
	//# specified by the $row$ parameter becomes the first row displayed at the top of the table. This function
	//# makes the best attempt to place this row at the top of the display area, but the index is clamped to the
	//# range of values allowed by the overall number of rows in the table.
	//
	//# \also	$@TableWidget::RevealTableItem@$
	//# \also	$@TableWidget::GetTableItemCount@$


	//# \function	TableWidget::RevealTableItem		Scrolls a table widget to reveal a specific item.
	//
	//# \proto	void RevealTableItem(int32 index);
	//
	//# \param	index	The index of the item to reveal.
	//
	//# \desc
	//# The $RevealTableItem$ function scrolls a table widget to reveal the item having the zero-baed index
	//# specified by the $index$ parameter. The table widget is scrolled far enough to place the row containing
	//# the revealed item at the vertical center of the table if possible.
	//#
	//# If the $index$ parameter is not in the range [0,&nbsp;<i>n</i>&nbsp;&minus;1], where <i>n</i> is the number of
	//# items in the table, then no change is made to the appearance of the table widget.
	//
	//# \also	$@TableWidget::SetDisplayRow@$
	//# \also	$@TableWidget::GetTableItemCount@$


	//# \div
	//# \function	TableWidget::GetSelectedTableItemCount		Returns the number of items that are currently selected in a table.
	//
	//# \proto	int32 GetSelectedTableItemCount(void) const;
	//
	//# \desc
	//# The $GetSelectedTableItemCount$ function returns the number of items that are currently selected in a table.
	//
	//# \also	$@TableWidget::GetSelectedTableItem@$
	//# \also	$@TableWidget::GetFirstSelectedTableItem@$
	//# \also	$@TableWidget::GetLastSelectedTableItem@$


	//# \function	TableWidget::GetSelectedTableItem			Returns the selected table item with a specific index.
	//
	//# \proto	Widget *GetSelectedTableItem(int32 index) const;
	//
	//# \param	index	The index of the selected table item to return, where the first selected table item has the index 0.
	//
	//# \desc
	//# The $GetSelectedTableItem$ function returns a pointer to the selected table item whose zero-based index among all selected
	//# table items is specified by the $index$ parameter. If the value of $index$ is greater than or equal to the number of
	//# selected items in the table (or if $index$ is negative), then the return value is $nullptr$.
	//#
	//# Selected table items are indexed in the order in which they appear in the table, and not in the order in which they
	//# were selected.
	//
	//# \also	$@TableWidget::GetFirstSelectedTableItem@$
	//# \also	$@TableWidget::GetLastSelectedTableItem@$
	//# \also	$@TableWidget::GetSelectedTableItemCount@$


	//# \function	TableWidget::GetFirstSelectedTableItem		Returns the first selected item in a table.
	//
	//# \proto	Widget *GetFirstSelectedTableItem(void) const;
	//
	//# \desc
	//# The $GetFirstSelectedTableItem$ function returns a pointer to the first selected item in a table. If there are no
	//# selected items in the table, then the return value is $nullptr$. The remaining selected items in a table can be iterated
	//# by calling the $@TableWidget::GetNextSelectedTableItem@$ function for the returned widget.
	//
	//# \also	$@TableWidget::GetNextSelectedTableItem@$
	//# \also	$@TableWidget::GetPreviousSelectedTableItem@$
	//# \also	$@TableWidget::GetLastSelectedTableItem@$
	//# \also	$@TableWidget::GetSelectedTableItem@$
	//# \also	$@TableWidget::GetSelectedTableItemCount@$


	//# \function	TableWidget::GetLastSelectedTableItem		Returns the last selected item in a table.
	//
	//# \proto	Widget *GetLastSelectedTableItem(void) const;
	//
	//# \desc
	//# The $GetLastSelectedTableItem$ function returns a pointer to the last selected item in a table. If there are no
	//# selected items in the table, then the return value is $nullptr$. The remaining selected items in a table can be iterated
	//# by calling the $@TableWidget::GetPreviousSelectedTableItem@$ function for the returned widget.
	//
	//# \also	$@TableWidget::GetPreviousSelectedTableItem@$
	//# \also	$@TableWidget::GetNextSelectedTableItem@$
	//# \also	$@TableWidget::GetFirstSelectedTableItem@$
	//# \also	$@TableWidget::GetSelectedTableItem@$
	//# \also	$@TableWidget::GetSelectedTableItemCount@$


	//# \function	TableWidget::GetPreviousSelectedTableItem	Returns the previous selected item in a table.
	//
	//# \proto	Widget *GetPreviousSelectedTableItem(const Widget *widget) const;
	//
	//# \param	widget		A pointer to a selected item for which the previous selected item is returned.
	//
	//# \desc
	//# The $GetPreviousSelectedTableItem$ function returns a pointer to the selected table item that precedes the selected
	//# item specified by the $widget$ parameter. If there is no previous selected item (i.e., the item specified by $widget$
	//# is the first selected item), then the return value is $nullptr$.
	//
	//# \also	$@TableWidget::GetNextSelectedTableItem@$
	//# \also	$@TableWidget::GetFirstSelectedTableItem@$
	//# \also	$@TableWidget::GetLastSelectedTableItem@$
	//# \also	$@TableWidget::GetSelectedTableItem@$
	//# \also	$@TableWidget::GetSelectedTableItemCount@$


	//# \function	TableWidget::GetNextSelectedTableItem		Returns the next selected item in a table.
	//
	//# \proto	Widget *GetNextSelectedTableItem(const Widget *widget) const;
	//
	//# \param	widget		A pointer to a selected item for which the next selected item is returned.
	//
	//# \desc
	//# The $GetNextSelectedTableItem$ function returns a pointer to the selected table item that follows the selected
	//# item specified by the $widget$ parameter. If there is no next selected item (i.e., the item specified by $widget$
	//# is the last selected item), then the return value is $nullptr$.
	//
	//# \also	$@TableWidget::GetPreviousSelectedTableItem@$
	//# \also	$@TableWidget::GetFirstSelectedTableItem@$
	//# \also	$@TableWidget::GetLastSelectedTableItem@$
	//# \also	$@TableWidget::GetSelectedTableItem@$
	//# \also	$@TableWidget::GetSelectedTableItemCount@$


	//# \function	TableWidget::SelectTableItem		Selects an item in a table.
	//
	//# \proto	void SelectTableItem(int32 index, bool post = false);
	//
	//# \param	index	The index of the item to select.
	//# \param	post	Indicates whether to post a $kEventWidgetChange$ event.
	//
	//# \desc
	//# The $SelectTableItem$ function selects the table item whose zero-based index is specified by the $index$ parameter.
	//# If the $index$ parameter is not in the range [0,&nbsp;<i>n</i>&nbsp;&minus;1], where <i>n</i> is the number of
	//# items in the table, then no new selection is made.
	//#
	//# If the $kTableMultipleSelection$ is not set for the table widget, then all table items having an index other than
	//# that specified by the $index$ parameter are unselected. In the case that the index is out of range, this means
	//# that all of the table items are unselected.
	//#
	//# If the $post$ parameter is $true$ and a change was actually made to the selection, then the table widget posts
	//# a $kEventWidgetChange$ event that can be handled by its observers.
	//
	//# \also	$@TableWidget::UnselectTableItem@$
	//# \also	$@TableWidget::SelectAllTableItems@$
	//# \also	$@TableWidget::UnselectAllTableItems@$
	//# \also	$@TableWidget::GetFirstSelectedTableItem@$
	//# \also	$@TableWidget::GetLastSelectedTableItem@$
	//# \also	$@TableWidget::GetSelectedTableItem@$
	//# \also	$@TableWidget::GetSelectedTableItemCount@$
	//# \also	$@TableWidget::GetTableItemCount@$
	//# \also	$@TableWidget::GetTableFlags@$
	//# \also	$@TableWidget::SetTableFlags@$
	//# \also	$@TableWidget::RevealTableItem@$


	//# \function	TableWidget::UnselectTableItem		Unselects an item in a table.
	//
	//# \proto	void UnselectTableItem(int32 index, bool post = false);
	//
	//# \param	index	The index of the item to unselect.
	//# \param	post	Indicates whether to post a $kEventWidgetChange$ event.
	//
	//# \desc
	//# The $UnselectTableItem$ function unselects the table item whose zero-based index is specified by the $index$ parameter.
	//# If the $index$ parameter is not in the range [0,&nbsp;<i>n</i>&nbsp;&minus;1], where <i>n</i> is the number of
	//# items in the table, then no change to the current selection is made.
	//#
	//# If the $post$ parameter is $true$ and a change was actually made to the selection, then the table widget posts
	//# a $kEventWidgetChange$ event that can be handled by its observers.
	//
	//# \also	$@TableWidget::SelectTableItem@$
	//# \also	$@TableWidget::SelectAllTableItems@$
	//# \also	$@TableWidget::UnselectAllTableItems@$
	//# \also	$@TableWidget::GetFirstSelectedTableItem@$
	//# \also	$@TableWidget::GetLastSelectedTableItem@$
	//# \also	$@TableWidget::GetSelectedTableItem@$
	//# \also	$@TableWidget::GetSelectedTableItemCount@$
	//# \also	$@TableWidget::GetTableItemCount@$
	//# \also	$@TableWidget::GetTableFlags@$
	//# \also	$@TableWidget::SetTableFlags@$
	//# \also	$@TableWidget::RevealTableItem@$


	//# \function	TableWidget::SelectAllTableItems		Selects all items in a table.
	//
	//# \proto	void SelectAllTableItems(bool post = false);
	//
	//# \param	post	Indicates whether to post a $kEventWidgetChange$ event.
	//
	//# \desc
	//# The $SelectAllTableItems$ function selects all of the items in a table. If any table items were previously
	//# not selected and the $post$ parameter is $true$, then the table widget posts a $kEventWidgetChange$ event
	//# that can be handled by its observers.
	//#
	//# If the $kTableMultipleSelection$ is not set for the table widget, then only the first item in the table is
	//# selected, and an event is posted only if this changes the current selection.
	//
	//# \also	$@TableWidget::UnselectAllTableItems@$
	//# \also	$@TableWidget::SelectTableItem@$
	//# \also	$@TableWidget::UnselectTableItem@$
	//# \also	$@TableWidget::GetFirstSelectedTableItem@$
	//# \also	$@TableWidget::GetLastSelectedTableItem@$
	//# \also	$@TableWidget::GetSelectedTableItem@$
	//# \also	$@TableWidget::GetSelectedTableItemCount@$
	//# \also	$@TableWidget::GetTableItemCount@$
	//# \also	$@TableWidget::GetTableFlags@$
	//# \also	$@TableWidget::SetTableFlags@$
	//# \also	$@TableWidget::RevealTableItem@$


	//# \function	TableWidget::UnselectAllTableItems		Unselects all items in a table.
	//
	//# \proto	void UnselectAllTableItems(bool post = false);
	//
	//# \param	post	Indicates whether to post a $kEventWidgetChange$ event.
	//
	//# \desc
	//# The $UnselectAllTableItems$ function unselects all of the items in a table. If any table items were previously
	//# selected and the $post$ parameter is $true$, then the table widget posts a $kEventWidgetChange$ event
	//# that can be handled by its observers.
	//
	//# \also	$@TableWidget::SelectAllTableItems@$
	//# \also	$@TableWidget::SelectTableItem@$
	//# \also	$@TableWidget::UnselectTableItem@$
	//# \also	$@TableWidget::GetFirstSelectedTableItem@$
	//# \also	$@TableWidget::GetLastSelectedTableItem@$
	//# \also	$@TableWidget::GetSelectedTableItem@$
	//# \also	$@TableWidget::GetSelectedTableItemCount@$
	//# \also	$@TableWidget::GetTableItemCount@$
	//# \also	$@TableWidget::GetTableFlags@$
	//# \also	$@TableWidget::SetTableFlags@$
	//# \also	$@TableWidget::RevealTableItem@$


	//# \div
	//# \function	TableWidget::PrependTableItem		Adds an item to the beginning of a table.
	//
	//# \proto	void PrependTableItem(Widget *widget);
	//
	//# \param	widget		The widget to add to the table.
	//
	//# \desc
	//# The $PrependTableItem$ function adds an item to the beginning of a table. The item specified by the
	//# $widget$ parameter becomes the leftmost item shown in the first row of the table.
	//
	//# \also	$@TableWidget::AppendTableItem@$


	//# \function	TableWidget::AppendTableItem		Adds an item to the end of a table.
	//
	//# \proto	void AppendTableItem(Widget *widget);
	//
	//# \param	widget		The widget to add to the table.
	//
	//# \desc
	//# The $AppendTableItem$ function adds an item to the end of a table. The item specified by the
	//# $widget$ parameter becomes the rightmost item shown in the last row of the table.
	//
	//# \also	$@TableWidget::PrependTableItem@$


	class TableWidget final : public RenderableWidget
	{
		friend class WidgetReg<TableWidget>;

		private:

			enum
			{
				kTableUpdatePlacement	= 1 << 0,
				kTableUpdateVisibility	= 1 << 1,
				kTableUpdateSelection	= 1 << 2
			};

			struct TableVertex
			{
				Point2D		position;
				ColorRGBA	color;
			};

			unsigned_int32					tableFlags;
			int32							columnCount;

			Vector2D						cellSize;
			Vector2D						itemOffset;
			Vector2D						hiliteInset;

			unsigned_int32					colorOverrideFlags;
			ColorRGBA						backgroundColor;
			ColorRGBA						hiliteColor;
			ColorRGBA						focusColor;

			bool							preprocessFlag;
			unsigned_int16					tableUpdateFlags;

			int32							tableItemCount;
			int32							clickItemIndex;

			int32							displayRowCount;
			int32							displayRowIndex;
			Widget							*displayItem;

			Point2D							*selectionVertex;

			ColorRGBA						dynamicBackgroundColor;
			ColorRGBA						dynamicHiliteColor;

			WidgetObserver<TableWidget>		scrollObserver;

			VertexBuffer					tableVertexBuffer;
			VertexBuffer					structureVertexBuffer;

			List<Attribute>					borderAttributeList;
			DiffuseAttribute				borderColorAttribute;
			DiffuseTextureAttribute			borderTextureAttribute;
			Renderable						borderRenderable;

			List<Attribute>					focusAttributeList;
			DiffuseAttribute				focusColorAttribute;
			Renderable						focusRenderable;

			Widget							itemGroup;
			ScrollWidget					scrollWidget;

			TableWidget();
			TableWidget(const TableWidget& tableWidget);

			void SetTableUpdateFlags(unsigned_int32 flags)
			{
				tableUpdateFlags |= flags;
				Invalidate();
			}

			Widget *Replicate(void) const override;

			void SetDefaultHiliteColor(void);
			void SetDefaultFocusColor(void);

			int32 GetTableRowCount(void) const;

			void UpdatePlacement(void);
			void UpdateVisibility(void);
			void UpdateSelection(void);

			void HandleStructureUpdate(void) override;

			void HandleScrollEvent(Widget *widget, const WidgetEventData *eventData);

			void HandleUpArrow(unsigned_int32 modifierKeys);
			void HandleDownArrow(unsigned_int32 modifierKeys);
			void HandleLeftArrow(unsigned_int32 modifierKeys);
			void HandleRightArrow(unsigned_int32 modifierKeys);

		public:

			C4API TableWidget(const Vector2D& size, int32 columns, const Vector2D& cell);
			C4API ~TableWidget();

			unsigned_int32 GetTableFlags(void) const
			{
				return (tableFlags);
			}

			void SetTableFlags(unsigned_int32 flags)
			{
				tableFlags = flags;
			}

			int32 GetColumnCount(void) const
			{
				return (columnCount);
			}

			const Vector2D& GetCellSize(void) const
			{
				return (cellSize);
			}

			void SetCellSize(const Vector2D& cell)
			{
				cellSize = cell;
			}

			const Vector2D& GetItemOffset(void) const
			{
				return (itemOffset);
			}

			void SetItemOffset(const Vector2D& offset)
			{
				itemOffset = offset;
			}

			const Vector2D& GetHiliteInset(void) const
			{
				return (hiliteInset);
			}

			void SetHiliteInset(const Vector2D& inset)
			{
				hiliteInset = inset;
			}

			int32 GetTableItemCount(void) const
			{
				return (tableItemCount);
			}

			Widget *GetFirstTableItem(void) const
			{
				return (itemGroup.GetFirstSubnode());
			}

			Widget *GetLastTableItem(void) const
			{
				return (itemGroup.GetLastSubnode());
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			const ColorRGBA& GetWidgetColor(WidgetColorType type = kWidgetColorDefault) const override;
			void SetWidgetColor(const ColorRGBA& color, WidgetColorType type = kWidgetColorDefault) override;
			void SetWidgetAlpha(float alpha, WidgetColorType type = kWidgetColorDefault) override;
			void SetDynamicWidgetColor(const ColorRGBA& color, WidgetColorType type = kWidgetColorDefault) override;
			void SetDynamicWidgetAlpha(float alpha, WidgetColorType type = kWidgetColorDefault) override;

			void SetWidgetSize(const Vector2D& size) override;
			WidgetPart TestPosition(const Point3D& position) const override;
			void Preprocess(void) override;

			void EnterForeground(void) override;
			void EnterBackground(void) override;

			void Build(void) override;
			void Render(List<Renderable> *renderList) override;

			void HandleMouseEvent(const PanelMouseEventData *eventData) override;
			bool HandleKeyboardEvent(const KeyboardEventData *eventData) override;

			C4API void SetColumnCount(int32 columns);

			C4API Widget *GetTableItem(int32 index) const;
			C4API void RevealTableItem(int32 index);

			C4API int32 GetActiveTableItemCount(void) const;
			C4API Widget *GetActiveTableItem(int32 index) const;
			C4API Widget *GetFirstActiveTableItem(void) const;
			C4API Widget *GetLastActiveTableItem(void) const;
			C4API Widget *GetPreviousActiveTableItem(const Widget *widget) const;
			C4API Widget *GetNextActiveTableItem(const Widget *widget) const;
			C4API Widget *GetPreviousActiveTableColumnItem(const Widget *widget) const;
			C4API Widget *GetNextActiveTableColumnItem(const Widget *widget) const;

			C4API void PrependTableItem(Widget *widget);
			C4API void AppendTableItem(Widget *widget);
			C4API void InsertTableItemBefore(Widget *widget, Widget *before);
			C4API void InsertTableItemAfter(Widget *widget, Widget *after);

			C4API void RemoveTableItem(Widget *widget);
			C4API void PurgeTableItems(void);
			C4API void PurgeSelectedTableItems(void);

			C4API int32 GetSelectedTableItemCount(void) const;
			C4API Widget *GetSelectedTableItem(int32 index) const;
			C4API Widget *GetFirstSelectedTableItem(void) const;
			C4API Widget *GetLastSelectedTableItem(void) const;
			C4API Widget *GetPreviousSelectedTableItem(const Widget *widget) const;
			C4API Widget *GetNextSelectedTableItem(const Widget *widget) const;

			C4API int32 GetFirstSelectedIndex(void) const;
			C4API int32 GetLastSelectedIndex(void) const;

			C4API void SelectTableItem(int32 index, bool post = false);
			C4API void UnselectTableItem(int32 index, bool post = false);
			C4API void SelectAllTableItems(bool post = false);
			C4API void UnselectAllTableItems(bool post = false);

			C4API void SetDisplayRow(int32 row);
	};


	//# \class	MultipaneWidget		The interface widget that displays a multipane box.
	//
	//# The $MultipaneWidget$ class represents an interface widget that displays a multipane box.
	//
	//# \def	class MultipaneWidget final : public RenderableWidget
	//
	//# \ctor	MultipaneWidget(const Vector2D& size, const char *font = "font/Gui");
	//
	//# \param	size	The size of the multipane widget, in pixels.
	//# \param	font	The name of the font in which the pane titles are displayed.
	//
	//# \desc
	//# The $MultipaneWidget$ widget displays a multipane box.
	//#
	//# The default widget color corresponds to the $kWidgetColorBackground$ color type and determines the color of the pane tabs.
	//# Other color types supported by the list widget are $kWidgetColorBorder$ and $kWidgetColorHilite$. If a highlight color has not been
	//# explicitly specified, then the Interface Manager's global highlight color is used.
	//
	//# \base	RenderableWidget	All rendered interface widgets are subclasses of $RenderableWidget$.


	class MultipaneWidget final : public RenderableWidget
	{
		friend class WidgetReg<MultipaneWidget>;

		private:

			struct MultipaneVertex
			{
				Point2D		position;
				ColorRGBA	color;
				Point2D		texcoord;
			};

			int32						multipaneSelection;
			unsigned_int32				colorOverrideFlags;

			ColorRGBA					borderColor;
			ColorRGBA					hiliteColor;

			ResourceName				fontName;

			int32						paneCount;
			int32						visiblePaneCount;
			float						titleHeight;
			bool						preprocessFlag;

			ColorRGBA					dynamicBackgroundColor;
			ColorRGBA					dynamicHiliteColor;

			VertexBuffer				multipaneVertexBuffer;
			VertexBuffer				multipaneIndexBuffer;

			VertexBuffer				borderVertexBuffer;
			List<Attribute>				borderAttributeList;
			DiffuseAttribute			borderColorAttribute;
			DiffuseTextureAttribute		borderTextureAttribute;
			Renderable					borderRenderable;

			Widget						paneGroup;

			MultipaneWidget();
			MultipaneWidget(const MultipaneWidget& multipaneWidget);

			Widget *Replicate(void) const override;

			void SetDefaultHiliteColor(void);

			void HandleStructureUpdate(void) override;
			bool CalculateBoundingBox(Box2D *box) const override;

			void BuildPane(const Widget *widget, volatile MultipaneVertex *restrict vertex, volatile Triangle *restrict triangle, int32 index, float y);

			void PrependPane(Widget *widget);
			void AppendPane(Widget *widget);

		public:

			C4API MultipaneWidget(const Vector2D& size, const char *font = "font/Gui");
			C4API ~MultipaneWidget();

			int32 GetSelection(void) const
			{
				return (multipaneSelection);
			}

			const ResourceName& GetFontName(void) const
			{
				return (fontName);
			}

			void SetFont(const char *font)
			{
				fontName = font;
			}

			int32 GetPaneCount(void) const
			{
				return (paneCount);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			const ColorRGBA& GetWidgetColor(WidgetColorType type = kWidgetColorDefault) const override;
			void SetWidgetColor(const ColorRGBA& color, WidgetColorType type = kWidgetColorDefault) override;
			void SetWidgetAlpha(float alpha, WidgetColorType type = kWidgetColorDefault) override;
			void SetDynamicWidgetColor(const ColorRGBA& color, WidgetColorType type = kWidgetColorDefault) override;
			void SetDynamicWidgetAlpha(float alpha, WidgetColorType type = kWidgetColorDefault) override;

			C4API void SetSelection(int32 selection, bool post = false);

			void Preprocess(void) override;
			void Build(void) override;
			void Render(List<Renderable> *renderList) override;

			void HandleMouseEvent(const PanelMouseEventData *eventData) override;

			C4API void PrependPane(const char *title);
			C4API void AppendPane(const char *title);
			C4API void DeletePane(int32 index);

			C4API void ShowPane(int32 index);
			C4API void HidePane(int32 index);

			C4API const char *GetPaneTitle(int32 index) const;
	};


	//# \class	DividerWidget		The interface widget that displays a divider.
	//
	//# The $DividerWidget$ class represents an interface widget that displays a divider.
	//
	//# \def	class DividerWidget final : public RenderableWidget
	//
	//# \ctor	DividerWidget(const Vector2D& size);
	//
	//# \param	size	The size of the divider widget, in pixels.
	//
	//# \desc
	//# The $DividerWidget$ class displays a window divider bar.
	//#
	//# The widget color is not used by the $DividerWidget$ class.
	//
	//# \base	RenderableWidget	All rendered interface widgets are subclasses of $RenderableWidget$.


	class DividerWidget final : public RenderableWidget
	{
		friend class WidgetReg<DividerWidget>;

		private:

			struct DividerVertex
			{
				Point2D		position;
				Point2D		texcoord;
			};

			VertexBuffer			vertexBuffer;
			List<Attribute>			attributeList;
			DiffuseAttribute		colorAttribute;

			DividerWidget();
			DividerWidget(const DividerWidget& dividerWidget);

			Widget *Replicate(void) const override;

		public:

			C4API DividerWidget(const Vector2D& size);
			C4API ~DividerWidget();

			void Preprocess(void) override;
			void Build(void) override;
	};


	class WindowFrameWidget final : public RenderableWidget
	{
		private:

			struct FrameVertex
			{
				Point2D		position;
				Point2D		texcoord;
			};

			Point3D						dragPosition;
			Vector2D					dragSize;

			VertexBuffer				vertexBuffer;
			static SharedVertexBuffer	indexBuffer;

			List<Attribute>				attributeList;
			DiffuseAttribute			colorAttribute;

			void Initialize(void);

			bool CalculateBoundingBox(Box2D *box) const override;

		public:

			WindowFrameWidget();
			WindowFrameWidget(const Vector2D& size);
			~WindowFrameWidget();

			WidgetPart TestPosition(const Point3D& position) const override;

			void Preprocess(void) override;
			void Build(void) override;

			void HandleMouseEvent(const PanelMouseEventData *eventData) override;
	};


	class BalloonFrameWidget final : public RenderableWidget
	{
		private:

			struct FrameVertex
			{
				Point2D		position;
				Point2D		texcoord;
			};

			int32						wedgeLocation;
			float						wedgeOffset;

			VertexBuffer				vertexBuffer;
			static SharedVertexBuffer	indexBuffer;

			List<Attribute>				attributeList;
			DiffuseAttribute			colorAttribute;

			void Initialize(void);

		public:

			BalloonFrameWidget();
			BalloonFrameWidget(const Vector2D& size);
			~BalloonFrameWidget();

			void SetWedgeLocation(int32 location, float offset)
			{
				wedgeLocation = location;
				wedgeOffset = offset;
			}

			void Preprocess(void) override;
			void Build(void) override;
	};


	class PageFrameWidget final : public RenderableWidget
	{
		private:

			struct FrameVertex
			{
				Point2D		position;
				Point2D		texcoord;
			};

			float						dragPosition;

			VertexBuffer				vertexBuffer;
			static SharedVertexBuffer	indexBuffer;

			List<Attribute>				attributeList;
			DiffuseAttribute			colorAttribute;

			void Initialize(void);

			bool CalculateBoundingBox(Box2D *box) const override;

		public:

			PageFrameWidget();
			PageFrameWidget(const Vector2D& size);
			~PageFrameWidget();

			WidgetPart TestPosition(const Point3D& position) const override;

			void Preprocess(void) override;
			void Build(void) override;

			void HandleMouseEvent(const PanelMouseEventData *eventData) override;
			void TrackTask(WidgetPart widgetPart, const Point3D& mousePosition) override;
	};


	class StripWidget final : public RenderableWidget
	{
		private:

			struct StripVertex
			{
				Point2D		position;
				Point2D		texcoord;
			};

			VertexBuffer			vertexBuffer;
			List<Attribute>			attributeList;
			DiffuseAttribute		colorAttribute;

		public:

			StripWidget(const Vector2D& size);
			~StripWidget();

			void Preprocess(void) override;
			void Build(void) override;
	};


	//# \class	RootWidget		Represents the root in a tree of widgets.
	//
	//# The $RootWidget$ class represents the root in a tree of widgets.
	//
	//# \def	class RootWidget : public Widget
	//
	//# \ctor	RootWidget(WidgetType type);
	//
	//# \param	type	The widget type.
	//
	//# \desc
	//# The $RootWidget$ class serves as the base class for widgets that reside at the root of a tree
	//# of widgets composing a user interface.
	//#
	//# The root widget can be obtained for any widget in a tree by calling the $@Widget::GetRootWidget@$ function.
	//
	//# \base	Widget		A $RootWidget$ is a specific type of widget.
	//
	//# \also	$@Widget::GetRootWidget@$


	//# \function	RootWidget::FindWidget		Finds a widget with a particular key.
	//
	//# \proto	Widget *FindWidget(const char *key) const;
	//
	//# \param	key		The key value of the widget to find.
	//
	//# \desc
	//# The $FindWidget$ function returns a pointer to the first widget in the subtree having a key
	//# matching the $key$ parameter. If no such widget exists, or the $key$ parameter is the empty
	//# string, then the return value is $nullptr$.
	//#
	//# Multiple widgets can have the same key value, and the $FindWidget$ function returns a pointer
	//# to the first widget with the specified key, in the order in which widgets were preprocessed.
	//# To iterate over all widgets having the same key value, call the $@Widget::GetNextWidgetWithSameKey@$
	//# function repeatedly until it returns $nullptr$.
	//#
	//# The $FindWidget$ function can only be called after the root widget has been preprocessed because
	//# the internal structures used to search for widgets are established during the preprocessing step.
	//# Thus, it is common for calls to $FindWidget$ to appear in the $Preprocess$ function implementation
	//# in subclasses of the $@Window@$, $@Board@$, or $@Page@$ classes.
	//
	//# \also	$@Widget::GetNextWidgetWithSameKey@$
	//
	//# \also	$@Window@$
	//# \also	$@Board@$


	class RootWidget : public Widget
	{
		private:

			HashTable<Widget>	widgetHashTable;

			unsigned_int32		widgetMoveParity;
			List<Widget>		widgetMoveList[3];

			Link<Widget>		focusWidget;

		protected:

			RootWidget(WidgetType type);
			RootWidget(WidgetType type, const Vector2D& size);
			RootWidget(const RootWidget& rootWidget);

			C4API virtual bool UnpackAuxiliaryChunk(const ChunkHeader *chunkHeader, Unpacker& data);

			C4API Widget *SetNextFocusWidget(void);
			C4API Widget *SetPreviousFocusWidget(void);

		public:

			~RootWidget();

			void AddKeyedWidget(Widget *widget)
			{
				widgetHashTable.Insert(widget);
			}

			void AddMovingWidget(Widget *widget)
			{
				widgetMoveList[widgetMoveParity].Append(widget);
			}

			void RemoveMovingWidget(Widget *widget)
			{
				widget->ListElement<Widget>::Detach();
			}

			Widget *FindWidget(const char *key) const
			{
				return (widgetHashTable.Find(key));
			}

			Widget *GetFocusWidget(void) const
			{
				return (focusWidget);
			}

			C4API void Preprocess(void) override;
			C4API void Move(void) override;

			C4API virtual void PackAuxiliaryData(Packer& data) const;
			C4API void UnpackAuxiliaryData(Unpacker& data);

			C4API virtual void SetFocusWidget(Widget *widget);
	};


	//# \class	Panel		The base class for the interfaces shown in panel effects.
	//
	//# A $Panel$ object represents an interface shown in a panel effect.
	//
	//# \def	class Panel : public RootWidget
	//
	//# \ctor	Panel(const Vector2D& size);
	//
	//# \param	size	The size of the panel, in pixels.
	//
	//# \desc
	//# The $Panel$ class is used as the root widget for interfaces shown in panel effects. $Panel$ objects are
	//# typically created by the Panel Editor.
	//
	//# \base	RootWidget		A panel serves as a root widget container.


	class Panel : public RootWidget
	{
		private:

			Transform4D		renderTransform;

		public:

			C4API Panel(const Vector2D& size);
			C4API ~Panel();

			const Transform4D& GetRenderTransform(void) const
			{
				return (renderTransform);
			}

			void SetRenderTransform(const Transform4D& transform)
			{
				renderTransform = transform;
			}

			void SetRenderTransform(const Vector3D& c1, const Vector3D& c2, const Vector3D& c3, const Point3D& c4)
			{
				renderTransform.Set(c1, c2, c3, c4);
			}

			void Move(void) override;
	};


	//# \class	Board		The base class for simple display interfaces.
	//
	//# A $Board$ object represents a simple display interface.
	//
	//# \def	class Board : public RootWidget
	//
	//# \ctor	Board(const Vector2D& size);
	//
	//# \param	size	The size of the board, in pixels.
	//
	//# \desc
	//# The $Board$ class is a lightweight root widget that is typically used for interfaces that don't
	//# require user interaction. They are often used for interfaces that simply display information to the user.
	//#
	//# The $Board$ constructor has protected access. It is used by creating a specialized subclass that fills the
	//# interface with its contents by either loading one or more panel resources or explicitly creating widgets.
	//
	//# \base	RootWidget		A board serves as a root widget container.


	class Board : public RootWidget
	{
		protected:

			C4API Board();
			C4API Board(const Vector2D& size);

		public:

			C4API ~Board();
	};


	//# \class	Window		The base class for all windows.
	//
	//# A $Window$ object represents a full-fledged interactive window.
	//
	//# \def	class Window : public RootWidget, public ListElement<Window>
	//
	//# \ctor	Window(const char *panelName);
	//# \ctor	Window(const Vector2D& size, const char *title = nullptr, unsigned_int32 flags = kWindowCloseBox | kWindowBackground);
	//
	//# \param	panelName		The name of the panel resource to load.
	//# \param	size			The size of the window, in pixels.
	//# \param	title			The window's displayed title.
	//# \param	flags			The window flags.
	//
	//# \desc
	//# The $Window$ class is the root widget used for full-fledged interactive windows.
	//#
	//# The $flags$ parameter can be a combination (through logical OR) of the following constants.
	//
	//# \table	WindowFlags
	//
	//# \base	RootWidget						A window serves as a root widget container.
	//# \base	Utilities/ListElement<Window>	Used internally by the Interface Manager.


	//# \function	Window::GetWindowFlags		Returns the window flags.
	//
	//# \proto	unsigned_int32 GetWindowFlags(void) const;
	//
	//# \desc
	//# The $GetWindowFlags$ function returns the window flags. This can be a combination (through logical OR)
	//# of the following constants.
	//
	//# \table	WindowFlags


	//# \function	Window::GetWindowTitle		Returns a window's title.
	//
	//# \proto	const char *GetWindowTitle(void) const;
	//
	//# \desc
	//
	//# \also	$@Window::SetWindowTitle@$


	//# \function	Window::SetWindowTitle		Sets a window's title.
	//
	//# \proto	void SetWindowTitle(const char *title);
	//
	//# \param	title	The new window title.
	//
	//# \desc
	//
	//# \also	$@Window::GetWindowTitle@$


	//# \function	Window::AddSubwindow		Adds a subwindow to a window.
	//
	//# \proto	void AddSubwindow(Window *window);
	//
	//# \param	window		The window to add as a subwindow.
	//
	//# \desc
	//


	class Window : public RootWidget, public ListElement<Window>
	{
		private:

			enum
			{
				kMaxBackgroundQuadCount = 6
			};

			unsigned_int32				windowFlags;
			Vector2D					minWindowSize;

			List<Window>				subwindowList;

			WidgetObserver<Window>		closeObserver;

			Link<Widget>				enterKeyWidget;
			Link<Widget>				escapeKeyWidget;
			Link<Widget>				commandWidget;
			Link<Widget>				windowButton;

			ResourceName				stripTitle;
			ResourceName				stripIcon;

			TextWidget					titleWidget;
			GuiButtonWidget				closeWidget;
			WindowFrameWidget			frameWidget;

			Storage<QuadWidget>			backgroundWidget[kMaxBackgroundQuadCount];

			C4API bool UnpackAuxiliaryChunk(const ChunkHeader *chunkHeader, Unpacker& data) override;

			void HandleCloseEvent(Widget *widget, const WidgetEventData *eventData);

		public:

			C4API Window(const char *panelName);
			C4API Window(const Vector2D& size, const char *title = nullptr, unsigned_int32 flags = kWindowCloseBox | kWindowBackground);
			C4API ~Window();

			using ListElement<Window>::Previous;
			using ListElement<Window>::Next;

			unsigned_int32 GetWindowFlags(void) const
			{
				return (windowFlags);
			}

			void SetWindowFlags(unsigned_int32 flags)
			{
				windowFlags = flags;
			}

			const Vector2D& GetMinWindowSize(void) const
			{
				return (minWindowSize);
			}

			void SetMinWindowSize(const Vector2D& minSize)
			{
				minWindowSize = minSize;
			}

			Window *GetFirstSubwindow(void) const
			{
				return (subwindowList.First());
			}

			Window *GetLastSubwindow(void) const
			{
				return (subwindowList.Last());
			}

			void PurgeSubwindows(void)
			{
				subwindowList.Purge();
			}

			void BringToFront(Window *window)
			{
				AppendSubnode(window);
				subwindowList.Append(window);
			}

			Widget *GetEscapeKeyWidget(void) const
			{
				return (escapeKeyWidget);
			}

			Widget *GetCommandWidget(void) const
			{
				return (commandWidget);
			}

			void SetCommandWidget(Widget *widget)
			{
				commandWidget = widget;
			}

			Widget *GetWindowButton(void) const
			{
				return (windowButton);
			}

			const char *GetStripTitle(void) const
			{
				return (stripTitle);
			}

			const char *GetStripIcon(void) const
			{
				return (stripIcon);
			}

			void SetStripIcon(const char *icon)
			{
				stripIcon = icon;
			}

			const char *GetWindowTitle(void) const
			{
				return (titleWidget.GetText());
			}

			void HideBackgroundQuad(int32 index)
			{
				backgroundWidget[index]->Widget::Detach();
			}

			C4API void Detach(void) override;

			C4API void PackAuxiliaryData(Packer& data) const;

			C4API int32 GetSettingCount(void) const override;
			C4API Setting *GetSetting(int32 index) const override;
			C4API void SetSetting(const Setting *setting) override;

			C4API void SetWidgetSize(const Vector2D& size) override;

			C4API void ScaleWindow(void);
			C4API void CenterWindow(void);

			C4API void Preprocess(void) override;
			C4API void Move(void) override;

			C4API void Show(void) override;
			C4API void Hide(void) override;

			C4API void EnterForeground(void) override;
			C4API void EnterBackground(void) override;

			C4API void SetWindowTitle(const char *title);
			C4API void SetStripTitle(const char *title);

			C4API void SetBackgroundQuad(int32 index, const Point3D& position, const Vector2D& size);

			C4API virtual void AddSubwindow(Window *window);

			C4API Widget *GetActivationKeyWidget(Type key) const;
			C4API void SetActivationKeyWidget(Type key, Widget *widget);

			C4API bool HandleKeyboardEvent(const KeyboardEventData *eventData) override;

			C4API virtual void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData);
			C4API virtual void Close(void);
	};


	class WindowButtonWidget final : public RenderableWidget, public ListElement<WindowButtonWidget>
	{
		private:

			struct ButtonVertex
			{
				Point2D		position;
				Point2D		texcoord;
			};

			int32					windowButtonValue;
			Link<Widget>			targetWindow;

			VertexBuffer			vertexBuffer;
			List<Attribute>			attributeList;
			DiffuseAttribute		colorAttribute;

			TextWidget				textWidget;
			ImageWidget				imageWidget;

		public:

			WindowButtonWidget(Window *window);
			~WindowButtonWidget();

			using ListElement<WindowButtonWidget>::Previous;
			using ListElement<WindowButtonWidget>::Next;

			int32 GetValue(void) const
			{
				return (windowButtonValue);
			}

			Window *GetWindow(void) const
			{
				return (static_cast<Window *>(targetWindow.GetTarget()));
			}

			void SetValue(int32 value);
			void SetTitle(const char *title);

			void SetWidgetSize(const Vector2D& size) override;
			void Preprocess(void) override;
			void Build(void) override;

			void HandleMouseEvent(const PanelMouseEventData *eventData) override;
	};


	class Balloon : public RootWidget
	{
		private:

			BalloonFrameWidget		frameWidget;

		public:

			Balloon(BalloonType type, const char *string);
			~Balloon();

			void SetWedgeLocation(int32 location, float offset)
			{
				frameWidget.SetWedgeLocation(location, offset);
			}
	};


	//# \class	Page		The base class for a tool page.
	//
	//# A $Page$ object represents a tool page.
	//
	//# \def	class Page : public RootWidget, public ListElement<Page>
	//
	//# \ctor	Page(const char *panelName);
	//# \ctor	Page(const Vector2D& size, const char *title = nullptr, unsigned_int32 flags = kWindowCloseBox | kWindowBackground);
	//
	//# \param	panelName		The name of the panel resource to load.
	//# \param	size			The size of the page, in pixels.
	//# \param	title			The page's displayed title.
	//# \param	flags			The window flags.
	//
	//# \desc
	//# The $flags$ parameter accepts the same values as the $flags$ parameter for the $@Window@$ constructor, but only the values
	//# $kWindowCloseBox$ and $kWindowBackground$ are recognized by pages.
	//
	//# \base	RootWidget						A page serves as a root widget container.
	//# \base	Utilities/ListElement<Page>		Used internally by the Interface Manager.


	class Page : public RootWidget, public ListElement<Page>
	{
		friend class BookWidget;
		friend class PageFrameWidget;

		private:

			unsigned_int32			windowFlags;
			BookWidget				*owningBook;

			Vector2D				pageSize;

			bool					dragFlag;
			float					originalPosition;
			float					movePosition[2];

			WidgetObserver<Page>	collapseObserver;

			TextWidget				titleWidget;
			GuiButtonWidget			collapseWidget;
			PageFrameWidget			frameWidget;
			QuadWidget				backgroundWidget;
			Widget					storageWidget;

			C4API bool UnpackAuxiliaryChunk(const ChunkHeader *chunkHeader, Unpacker& data) override;

			void HandleCollapseEvent(Widget *widget, const WidgetEventData *eventData);

		public:

			C4API Page(const char *panelName);
			C4API Page(const Vector2D& size, const char *title = nullptr, unsigned_int32 flags = kWindowCloseBox);
			C4API ~Page();

			using ListElement<Page>::Previous;
			using ListElement<Page>::Next;

			unsigned_int32 GetWindowFlags(void) const
			{
				return (windowFlags);
			}

			BookWidget *GetOwningBook(void) const
			{
				return (owningBook);
			}

			const char *GetPageTitle(void) const
			{
				return (titleWidget.GetText());
			}

			void SetPageTitle(const char *title)
			{
				titleWidget.SetText(title);
			}

			C4API void Preprocess(void) override;

			C4API void Collapse(void);
			C4API void Expand(void);
	};


	class BookWidget final : public Widget, public ListElement<BookWidget>
	{
		private:

			float			topMargin;
			float			bottomMargin;
			float			totalPageHeight;

			float			pageMoveTime;
			Page			*dragPreviousPage;

			List<Page>		pageList;
			Widget			pageGroup;

			void UpdateBoundingBox(void);

			Page *FindPreviousPage(const Page *dragPage) const;

		public:

			C4API BookWidget(const Vector2D& size, float top = 0.0F, float bottom = 0.0F);
			C4API ~BookWidget();

			using ListElement<BookWidget>::Previous;
			using ListElement<BookWidget>::Next;

			Page *GetFirstPage(void) const
			{
				return (pageList.First());
			}

			Page *GetLastPage(void) const
			{
				return (pageList.Last());
			}

			void BringToFront(Page *page)
			{
				pageGroup.AppendSubnode(page);
			}

			C4API void SetWidgetSize(const Vector2D& size) override;
			void RenderTree(List<Renderable> *renderList) override;

			void Move(void) override;
			void HandleMouseEvent(const PanelMouseEventData *eventData) override;

			void BeginPageDrag(Page *dragPage);
			void EndPageDrag(Page *dragPage);
			void UpdatePageDrag(Page *dragPage);

			C4API void AppendPage(Page *page);
			C4API void PrependPage(Page *page);
			C4API void RemovePage(Page *page);

			C4API void OrganizePages(void);
	};
}


#endif

// ZYUQURM
