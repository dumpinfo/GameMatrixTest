 

#ifndef C4Input_h
#define C4Input_h


//# \component	Input Manager
//# \prefix		InputMgr/


#include "C4Variables.h"

#if C4WINDOWS && C4FASTBUILD

	#include "C4DirectInput.h"

#endif

#if C4IOS //[ MOBILE

	// -- Mobile code hidden --

#endif //]

namespace C4
{
	//# \tree	InputControl
	//
	//# \node	ButtonControl
	//# \sub
	//#		\node	KeyButtonControl
	//#		\node	GenericButtonControl
	//#		\node	TouchButtonControl
	//# \end
	//# \node	AxisControl
	//# \sub
	//#		\node	LinearAxisControl
	//#		\node	RotationAxisControl
	//#		\node	DeltaAxisControl
	//# \end
	//# \node	SliderControl
	//# \node	DirectionalControl


	//# \tree	InputDevice
	//
	//#	\node	MouseDevice
	//#	\node	KeyboardDevice
	//#	\node	JoystickDevice
	//# \node	TouchDevice
	//#	\node	GameDevice
	//#	\node	OrbisDevice
	//#	\node	CellDevice
	//#	\node	XboxDevice


	typedef EngineResult		InputResult;
	typedef unsigned_int32		InputMode;

	typedef Type	InputDeviceType;
	typedef Type	InputControlType;
	typedef Type	ActionType;


	enum : InputResult
	{
		kInputOkay				= kEngineOkay,
		kInputInitFailed		= (kManagerInput << 16) | 0x0001
	};


	enum
	{
		kMaxInputDeviceNameLength		= 255,
		kMaxInputControlNameLength		= 127,
		kMaxInputFeedbackNameLength		= 127
	};


	typedef String<kMaxInputDeviceNameLength>		InputDeviceName;
	typedef String<kMaxInputControlNameLength>		InputControlName;
	typedef String<kMaxInputFeedbackNameLength>		InputFeedbackName;


	enum
	{
		kKeyboardUsageCount				= 232
	};


	#if C4LINUX

		enum
		{
			kKeyboardRawCodeBase		= 8,
			kKeyboardRawCodeCount		= 248
		};

	#elif C4IOS //[ MOBILE

		// -- Mobile code hidden --

	#elif C4PS4 //[ PS4 

		// -- PS4 code hidden --
 
	#elif C4PS3 //[ PS3
 
		// -- PS3 code hidden --

	#endif //] 

 
	#if C4XINPUT 

		enum
		{
			kXboxButtonUp, 
			kXboxButtonDown,
			kXboxButtonLeft,
			kXboxButtonRight,
			kXboxButtonStart,
			kXboxButtonBack,
			kXboxButtonLeftStick,
			kXboxButtonRightStick,
			kXboxButtonLB,
			kXboxButtonRB,
			kXboxButtonA,
			kXboxButtonB,
			kXboxButtonX,
			kXboxButtonY,
			kXboxButtonCount
		};

		enum
		{
			kXboxTriggerLeft,
			kXboxTriggerRight,
			kXboxTriggerCount
		};

		enum
		{
			kXboxAxisLeftX,
			kXboxAxisLeftY,
			kXboxAxisRightX,
			kXboxAxisRightY,
			kXboxAxisCount
		};

	#endif


	//# \enum	InputDeviceType

	enum : InputDeviceType
	{
		kInputDeviceMouse				= 'MOUS',		//## Mouse device.
		kInputDeviceKeyboard			= 'KYBD',		//## Keyboard device.
		kInputDeviceJoystick			= 'JSTK',		//## Joystick or other game device (desktop only).
		kInputDeviceTouch				= 'TOCH',		//## Generic touch screen device (mobile only).
		kInputDeviceGame				= 'GAME',		//## External game controller device (mobile only).
		kInputDeviceOrbis				= 'ORBS',		//## PlayStation 4 game controller device (PS4 only).
		kInputDeviceCell				= 'CELL',		//## PlayStation 3 game controller device (PS3 only).
		kInputDeviceXbox				= 'XBOX'		//## Xbox controller (Windows only).
	};


	//# \enum	InputControlType

	enum : InputControlType
	{
		kInputControlGroup				= 0,			//## Group of input controls.
		kInputControlKeyButton			= 'KEYB',		//## Key button control.
		kInputControlGenericButton		= 'BTTN',		//## Generic button control.
		kInputControlTouchButton		= 'TOCH',		//## Touch button control (mobile only).
		kInputControlLinearAxis			= 'AXIS',		//## Linear axis control.
		kInputControlRotationAxis		= 'RTAX',		//## Rotation axis control.
		kInputControlDeltaAxis			= 'DELT',		//## Delta axis control.
		kInputControlSlider				= 'SLID',		//## Slider control.
		kInputControlDirectional		= 'DPAD'		//## Directional pad control.
	};


	enum : ActionType
	{
		kActionConsole			= 'CONS',
		kActionEscape			= 'ESCP',
		kActionMouseX			= 'MOSX',
		kActionMouseY			= 'MOSY'
	};


	enum
	{
		kActionImmutable		= 1 << 0,
		kActionPersistent		= 1 << 1
	};


	//# \enum	MouseFlags

	enum
	{
		kMouseInverted			= 1 << 0,		//## Movement along the vertical axis of the mouse is negated.
		kMouseSmooth			= 1 << 1		//## Mouse movements are smoothed over multiple input events.
	};


	//# \enum	InputMode

	enum
	{
		kInputInactive			= 0,															//## No input is processed by the Input Manager. This flag should be specified by itself to disable the Input Manager.
		kInputMouseActive		= 1 << 0,														//## Mouse input is processed by the Input Manager.
		kInputKeyboardActive	= 1 << 1,														//## Keyboard input is processed by the Input Manager.
		kInputGameActive		= 1 << 2,														//## Joystick and game controller input is processed by the Input Manager.
		kInputAllActive			= kInputMouseActive | kInputKeyboardActive | kInputGameActive,	//## All input is processed by the Input Manager. Specifying this flag is equivalent to specifying $kInputMouseActive$, $kInputKeyboardActive$, and $kInputGameActive$.
		kInputConfiguration		= 0xFFFFFFFF
	};


	enum InputAxis
	{
		kInputAxisNone = -1,
		kInputAxisX,
		kInputAxisY,
		kInputAxisZ
	};


	class InputDevice;


	//# \class	Action		Represents an action that can be triggered by an input control.
	//
	//# The $Action$ class represents an action that can be triggered by an input control.
	//
	//# \def	class Action : public LinkTarget<Action>, public ListElement<Action>, public Memory<InputDevice>
	//
	//# \ctor	explicit Action(ActionType type);
	//
	//# The constructor has protected access. An $Action$ class can only exist as the base class for a more specific type of action.
	//
	//# \param	type	The type of the action.
	//
	//# \desc
	//# An application typically creates several $Action$ objects representing the various actions that a player
	//# can perform in a game. Examples of actions are forward and backward movements and firing a weapon. Once an
	//# action object is created, it must be registered with the Input Manager by calling the $@InputMgr::AddAction@$
	//# function.
	//#
	//# An action object is assigned to an input control by calling the $@InputControl::SetControlAction@$ function.
	//# One action object may be assigned to multiple input controls. After an action object is assigned to an input
	//# control, it can receive calls to its virtual member functions in response to user input to that input control.
	//#
	//# Each action must have a unique type. An application can choose any 32-bit identifier that does not consist entirely
	//# of uppercase letters and numbers to pass in the $type$ paramater of the $Action$ constructor.
	//
	//# \base	Utilities/LinkTarget<Action>	Used internally by the Input Manager.
	//# \base	Utilities/ListElement<Action>	Used internally by the Input Manager.
	//# \base	MemoryMgr/Memory<InputDevice>	Actions are allocated in a dedicated heap.
	//
	//# \also	$@InputControl@$
	//# \also	$@InputMgr::AddAction@$
	//# \also	$@InputMgr::RemoveAction@$


	//# \function	Action::GetActionType		Returns the type of action.
	//
	//# \proto	ActionType GetActionType(void) const;
	//
	//# \desc
	//# The $GetActionType$ function returns the action type that was established when the action object was constructed.


	//# \function	Action::Begin		Called when an associated input control is activated.
	//
	//# \proto	virtual void Begin(void);
	//
	//# \desc
	//# The $Begin$ function is called when an input control to which the action object is assigned is activated by the user.
	//# For example, when a key is pressed, a mouse button is clicked, or a button on a joystick is pressed, the $Begin$
	//# function is called for any action object assigned to the control. When the user releases the control, the
	//# $@Action::End@$ function is called for the action object.
	//
	//# \also	$@Action::End@$
	//# \also	$@Action::Move@$
	//# \also	$@Action::Update@$


	//# \function	Action::End			Called when an associated input control is deactivated.
	//
	//# \proto	virtual void End(void);
	//
	//# \desc
	//# The $End$ function is called when an input control to which the action object is assigned is deactivated by the user.
	//# For example, when a key, mouse button, or joystick button is released, the $End$ function is called for any action
	//# object assigned to the control. The $@Action::Begin@$ function would have previously been called for the action object
	//# at the time that the user activated the control.
	//
	//# \also	$@Action::Begin@$
	//# \also	$@Action::Move@$
	//# \also	$@Action::Update@$


	//# \function	Action::Move		Called when an associated input control has new directional data.
	//
	//# \proto	virtual void Move(int32 value);
	//
	//# \param	value	The new control data.
	//
	//# \desc
	//# The $Move$ function is called when a directional pad input control to which the action object is assigned is manipulated
	//# by the user. If the user lets go of the directional pad (allowing it to return to the center position), then the $value$
	//# parameter is &minus;1. Otherwise, the $value$ parameter is in the range [0,&nbsp;7], where 0 corresponds to straight up,
	//# and higher values represent directions in 45-degree increments moving clockwise. For instance, 1 corresponds to
	//# halfway between right and up, 2 means directly to the right, and 6 means directly to the left.
	//
	//# \also	$@Action::Begin@$
	//# \also	$@Action::End@$
	//# \also	$@Action::Update@$


	//# \function	Action::Update		Called when an associated input control has new axis data.
	//
	//# \proto	virtual void Update(float value);
	//
	//# \param	value	The new control data.
	//
	//# \desc
	//# The $Update$ function is called when the position of an analog axis control to which the action object is assigned
	//# is changed by the user. For an absolute axis, the $value$ parameter is in the range [&minus;1.0F,&nbsp;1.0F], where
	//# 0.0F corresponds to the center position. For a relative axis (such as a mouse axis), the $value$ parameter
	//# represents the relative displacement without normalization to any predefined range. For a slider axis, the
	//# value parameter is in the range [0.0F,&nbsp;1.0F].
	//
	//# \also	$@Action::Begin@$
	//# \also	$@Action::End@$
	//# \also	$@Action::Move@$


	class Action : public LinkTarget<Action>, public ListElement<Action>, public EngineMemory<InputDevice>
	{
		private:

			ActionType			actionType;
			unsigned_int32		actionFlags;

			int32				activeCount;

		protected:

			C4API explicit Action(ActionType type);

		public:

			C4API virtual ~Action();

			ActionType GetActionType(void) const
			{
				return (actionType);
			}

			unsigned_int32 GetActionFlags(void) const
			{
				return (actionFlags);
			}

			void SetActionFlags(unsigned_int32 flags)
			{
				actionFlags = flags;
			}

			int32 GetActiveCount(void) const
			{
				return (activeCount);
			}

			void SetActiveCount(int32 count)
			{
				activeCount = count;
			}

			C4API virtual void Begin(void);
			C4API virtual void End(void);
			C4API virtual void Move(int32 value);
			C4API virtual void Update(float value);
	};


	class ConsoleAction : public Action
	{
		public:

			ConsoleAction();
			~ConsoleAction();

			void Begin(void);
	};


	class EscapeAction : public Action
	{
		public:

			EscapeAction();
			~EscapeAction();

			void Begin(void);
	};


	class MouseAction : public Action
	{
		public:

			MouseAction(ActionType type);
			~MouseAction();

			void Update(float value);
	};


	class CommandAction : public Action
	{
		private:

			String<kMaxVariableValueLength>		command;

		public:

			CommandAction(const char *cmd);
			~CommandAction();

			const char *GetCommand(void) const
			{
				return (command);
			}

			void Begin(void);
	};


	//# \class	InputControl	Encapsulates an individual input device control.
	//
	//# \def	class InputControl : public Tree<InputControl>, public Memory<InputDevice>
	//
	//# \desc
	//# The $InputControl$ class represents an individual input control for an input device.
	//# An input control can have one of the following types.
	//
	//# \table	InputControlType
	//
	//# Events for an input control are communicated to the application through $@Action@$ objects.
	//# An action is assigned to an input control using the $@InputControl::SetControlAction@$ function.
	//
	//# \base	Utilities/Tree<InputMgr>		Input controls are organized in a tree hierarchy.
	//# \base	MemoryMgr/Memory<InputDevice>	Input controls are allocated in a dedicated heap.
	//
	//# \also	$@Action@$
	//# \also	$@InputDevice@$


	//# \function	InputControl::GetControlType		Returns the type of an input control.
	//
	//# \proto	InputControlType GetControlType(void) const;
	//
	//# \desc
	//# The $GetControlType$ function returns the type of an input control, which can be one of
	//# the following constants.
	//
	//# \table	InputControlType
	//
	//# \also	$@InputControl::GetControlHidUsage@$
	//# \also	$@InputControl::GetControlName@$


	//# \function	InputControl::GetControlHidUsage	Returns the 32-bit HID usage value for an input control.
	//
	//# \proto	unsigned_int32 GetControlHidUsage(void) const;
	//
	//# \desc
	//# The $GetControlHidUsage$ function returns the 32-bit HID usage value for an input control. The high
	//# 16 bits contain the HID usage page, and the low 16 bits contain the HID usage value within that page.
	//#
	//# Input controls having the type $kInputControlGroup$ always have a HID usage value of zero.
	//#
	//# See the input device subclasses for more information about HID usages for the controls belonging to
	//# each particular type of device.
	//
	//# \also	$@InputControl::GetControlType@$
	//# \also	$@InputControl::GetControlName@$
	//# \also	$@InputDevice::GetDeviceHidUsage@$


	//# \function	InputControl::GetControlName		Returns the name of an input control.
	//
	//# \proto	const char *GetControlName(void) const;
	//
	//# \desc
	//# The $GetControlName$ function returns a pointer to the name of an input control.
	//
	//# \also	$@InputControl::GetControlType@$
	//# \also	$@InputControl::GetControlHidUsage@$


	//# \function	InputControl::GetControlAction		Returns the action assigned to an input control.
	//
	//# \proto	Action *GetControlAction(void) const;
	//
	//# \desc
	//# The $GetControlAction$ function returns a pointer to the $@Action@$ object assigned to an input control.
	//# If no action is assigned to a control, then the return value is $nullptr$.
	//
	//# \also	$@InputControl::SetControlAction@$
	//# \also	$@Action@$


	//# \function	InputControl::SetControlAction		Assigns an action to an input control.
	//
	//# \proto	void SetControlAction(Action *action);
	//
	//# \param	action		The action to assign to the input control. This can be $nullptr$ to remove any
	//#						previously assigned action.
	//
	//# \desc
	//# The $SetControlAction$ function assigns an $@Action@$ object to an input control. Once an action
	//# has been assigned, its member functions are called whenever the input control processes an event.
	//
	//# \also	$@InputControl::GetControlAction@$
	//# \also	$@Action@$


	class InputControl : public Tree<InputControl>, public EngineMemory<InputDevice>
	{
		friend class InputDevice;

		#if C4WINDOWS

			friend class DirectInputDevice;

		#endif

		private:

			InputDevice					*owningDevice;

			InputControlType			controlType;
			unsigned_int32				controlHidUsage;

			Link<Action>				controlAction;
			Link<Action>				activeAction;

			InputControlName			controlName;

			#if C4WINDOWS

				GUID					dataGuid;
				DWORD					dataType;
				DWORD					dataFlags;
				DWORD					dataOffset;

			#elif C4MACOS

				IOHIDElementCookie		controlCookie;
				bool					controlActive;

			#endif

		protected:

			#if C4WINDOWS

				InputControl(InputDevice *device, InputControlType type, unsigned_int32 usage, const DIDEVICEOBJECTINSTANCEW *instance);

			#endif

			InputControl(InputDevice *device, InputControlType type, unsigned_int32 usage, const char *name);

			void SetActiveAction(Action *action)
			{
				activeAction = action;
			}

		public:

			InputControl(InputDevice *device);
			virtual ~InputControl();

			InputDevice *GetOwningDevice(void) const
			{
				return (owningDevice);
			}

			InputControlType GetControlType(void) const
			{
				return (controlType);
			}

			unsigned_int32 GetControlHidUsage(void) const
			{
				return (controlHidUsage);
			}

			const char *GetControlName(void) const
			{
				return (controlName);
			}

			Action *GetControlAction(void) const
			{
				return (controlAction);
			}

			C4API void SetControlAction(Action *action);

			#if C4MACOS

				void Activate(IOHIDQueueInterface **deviceQueue);
				void Deactivate(IOHIDQueueInterface **deviceQueue);

			#endif

			virtual void HandleNormalEvent(int32 value);
			virtual bool HandleConfigEvent(int32 value);
	};


	//# \class	ButtonControl	Encapsulates a button input control.
	//
	//# \def	class ButtonControl : public InputControl
	//
	//# \desc
	//# The $ButtonControl$ class is the base class for all input controls that are buttons.
	//#
	//# If an action is assigned to a $ButtonControl$ object, an internal counter for the action is incremented when the button is pressed,
	//# and it is decremented when the button is released. When the counter is incremented to 1, the action's $@Action::Begin@$ function
	//# is called, and when the counter is decremented to 0, the action's $@Action::End@$ function is called. This mechanism allows an
	//# action to be assigned to multiple buttons without redundant events being reported.
	//
	//# \base	InputControl	A $ButtonControl$ is a specific type of input control.
	//
	//# \also	$@Action::Begin@$
	//# \also	$@Action::End@$


	class ButtonControl : public InputControl
	{
		private:

			unsigned_int32		dataMask;

		protected:

			#if C4WINDOWS

				ButtonControl(InputDevice *device, InputControlType type, unsigned_int32 usage, const DIDEVICEOBJECTINSTANCEW *instance);

			#endif

			ButtonControl(InputDevice *device, InputControlType type, unsigned_int32 usage, const char *name);

		public:

			~ButtonControl();

			void HandleNormalEvent(int32 value);
			bool HandleConfigEvent(int32 value);
	};


	//# \class	KeyButtonControl		Encapsulates a key button input control.
	//
	//# \def	class KeyButtonControl : public ButtonControl
	//
	//# \desc
	//# The $KeyButtonControl$ class represents a single key on a keyboard device. If an action is assigned
	//# to a $KeyButtonControl$ object, then events are reported as described for the $@ButtonControl@$ base class.
	//
	//# \base	ButtonControl	A $KeyButtonControl$ is a specific type of button control.


	class KeyButtonControl : public ButtonControl
	{
		public:

			#if C4WINDOWS

				KeyButtonControl(InputDevice *device, unsigned_int32 usage, const DIDEVICEOBJECTINSTANCEW *instance);

			#endif

			KeyButtonControl(InputDevice *device, unsigned_int32 usage, const char *name);
			~KeyButtonControl();
	};


	//# \class	GenericButtonControl		Encapsulates a generic button input control.
	//
	//# \def	class GenericButtonControl : public ButtonControl
	//
	//# \desc
	//# The $GenericButtonControl$ class represents any button on an input device that is not a key on a keyboard.
	//# If an action is assigned to a $GenericButtonControl$ object, then events are reported as described for the
	//# $@ButtonControl@$ base class.
	//
	//# \base	ButtonControl	A $GenericButtonControl$ is a specific type of button control.


	class GenericButtonControl : public ButtonControl
	{
		public:

			#if C4WINDOWS

				GenericButtonControl(InputDevice *device, const DIDEVICEOBJECTINSTANCEW *instance);

			#endif

			GenericButtonControl(InputDevice *device, unsigned_int32 usage, const char *name);
			~GenericButtonControl();
	};


	#if C4IOS //[ MOBILE

		// -- Mobile code hidden --

	#endif //]


	//# \class	AxisControl		Encapsulates an axis input control.
	//
	//# \def	class AxisControl : public InputControl
	//
	//# \desc
	//# The $AxisControl$ class is the base class for all input controls that are based on a two-way axis.
	//#
	//# If an action is assigned to an $AxisControl$ object, then the action's $@Action::Update@$ function is called
	//# when the position of the axis control changes. For an absolute axis, the value passed to the $Update$ function
	//# is normalized to the range [&minus;1.0F,&nbsp;1.0F], where 0.0F corresponds to the center position. For a relative
	//# axis (such as a mouse axis), the $value$ parameter represents the relative displacement without normalization to
	//# any predefined range.
	//
	//# \base	InputControl	A $AxisControl$ is a specific type of input control.
	//
	//# \also	$@Action::Update@$


	class AxisControl : public InputControl
	{
		private:

			InputAxis		controlAxis;

			float			minValue;
			float			maxValue;
			float			centerValue;
			float			deadZone;
			float			normalizer;

		protected:

			#if C4WINDOWS

				AxisControl(InputDevice *device, InputControlType type, InputAxis axis, const DIDEVICEOBJECTINSTANCEW *instance);

			#endif

			AxisControl(InputDevice *device, InputControlType type, InputAxis axis, unsigned_int32 usage, const char *name);

		public:

			~AxisControl();

			InputAxis GetControlAxis(void) const
			{
				return (controlAxis);
			}

			void SetRange(float vmin, float vmax, float dead);

			void HandleNormalEvent(int32 value);
			bool HandleConfigEvent(int32 value);
	};


	//# \class	LinearAxisControl		Encapsulates a linear axis input control.
	//
	//# \def	class LinearAxisControl : public AxisControl
	//
	//# \desc
	//# The $LinearAxisControl$ class represents an input control that uses a linear axis.
	//# If an action is assigned to a $LinearAxisControl$ object, then events are reported as described for
	//# the $AxisControl$ base class.
	//
	//# \base	AxisControl		A $LinearAxisControl$ is a specific type of axis control.


	class LinearAxisControl : public AxisControl
	{
		public:

			#if C4WINDOWS

				LinearAxisControl(InputDevice *device, InputAxis axis, const DIDEVICEOBJECTINSTANCEW *instance);

			#endif

			LinearAxisControl(InputDevice *device, InputAxis axis, unsigned_int32 usage, const char *name);
			~LinearAxisControl();
	};


	//# \class	RotationAxisControl		Encapsulates a rotation axis input control.
	//
	//# \def	class RotationAxisControl : public AxisControl
	//
	//# \desc
	//# The $RotationAxisControl$ class represents an input control that uses a rotational axis.
	//# If an action is assigned to a $RotationAxisControl$ object, then events are reported as described for
	//# the $AxisControl$ base class.
	//
	//# \base	AxisControl		A $RotationAxisControl$ is a specific type of axis control.


	class RotationAxisControl : public AxisControl
	{
		public:

			#if C4WINDOWS

				RotationAxisControl(InputDevice *device, InputAxis axis, const DIDEVICEOBJECTINSTANCEW *instance);

			#endif

			RotationAxisControl(InputDevice *device, InputAxis axis, unsigned_int32 usage, const char *name);
			~RotationAxisControl();
	};


	//# \class	DeltaAxisControl		Encapsulates a delta axis input control.
	//
	//# \def	class DeltaAxisControl : public AxisControl
	//
	//# \desc
	//# The $DeltaAxisControl$ class represents an input control that uses a relative linear axis.
	//# If an action is assigned to a $DeltaAxisControl$ object, then events are reported as described for
	//# the $AxisControl$ base class.
	//
	//# \base	AxisControl		A $DeltaAxisControl$ is a specific type of axis control.


	class DeltaAxisControl : public AxisControl
	{
		public:

			#if C4WINDOWS

				DeltaAxisControl(InputDevice *device, InputAxis axis, const DIDEVICEOBJECTINSTANCEW *instance);

			#endif

			DeltaAxisControl(InputDevice *device, InputAxis axis, unsigned_int32 usage, const char *name);
			~DeltaAxisControl();

			void HandleNormalEvent(int32 value);
			bool HandleConfigEvent(int32 value);
	};


	//# \class	SliderControl		Encapsulates a slider input control.
	//
	//# \def	class SliderControl : public InputControl
	//
	//# \desc
	//# The $SliderControl$ class represents an input control that uses a one-way axis, or slider.
	//#
	//# If an action is assigned to a $SliderControl$ object, then the action's $@Action::Update@$ function is called
	//# when the position of the slider changes. The value passed to the $Update$ function is normalized to the range [0.0F,&nbsp;1.0F].
	//
	//# \base	InputControl	A $SliderControl$ is a specific type of input control.
	//
	//# \also	$@Action::Update@$


	class SliderControl : public InputControl
	{
		private:

			float			maxValue;
			float			threshold;
			float			normalizer;

		public:

			#if C4WINDOWS

				SliderControl(InputDevice *device, const DIDEVICEOBJECTINSTANCEW *instance);

			#endif

			SliderControl(InputDevice *device, unsigned_int32 usage, const char *name);
			~SliderControl();

			void SetRange(float vmin, float vmax, float thresh);

			void HandleNormalEvent(int32 value);
			bool HandleConfigEvent(int32 value);
	};


	//# \class	DirectionalControl		Encapsulates a directional pad input control.
	//
	//# \def	class DirectionalControl : public InputControl
	//
	//# \desc
	//# The $DirectionalControl$ class represents an input control that is a directional pad.
	//#
	//# If an action is assigned to a $DirectionalControl$ object, then the action's $@Action::Move@$ function is called
	//# when the state of the directional pad changes. If the user lets go of the directional pad (allowing it to return to
	//# the center position), then the value passed to the $Move$ function is &minus;1. Otherwise, the value is in the range
	//# [0,&nbsp;7], where 0 corresponds to straight up, and higher values represent directions in 45-degree increments moving
	//# clockwise. For instance, 1 corresponds to halfway between right and up, 2 means directly to the right, and 6 means directly to the left.
	//
	//# \base	InputControl	A $DirectionalControl$ is a specific type of input control.
	//
	//# \also	$@Action::Move@$


	class DirectionalControl : public InputControl
	{
		private:

			int32	divider;

		public:

			#if C4WINDOWS

				DirectionalControl(InputDevice *device, const DIDEVICEOBJECTINSTANCEW *instance);

			#endif

			DirectionalControl(InputDevice *device, unsigned_int32 usage, const char *name);
			~DirectionalControl();

			void HandleNormalEvent(int32 value);
			bool HandleConfigEvent(int32 value);
	};


	class InputFeedback : public ListElement<InputFeedback>, public EngineMemory<InputDevice>
	{
		private:

			InputFeedbackName		feedbackName;

		public:

			#if C4WINDOWS

				InputFeedback(const DIEFFECTINFOW *effect);

			#else

				InputFeedback(const char *name);

			#endif

			~InputFeedback();

			const char *GetFeedbackName(void) const
			{
				return (feedbackName);
			}
	};


	//# \class	InputDevice		Encapsulates an individual input device.
	//
	//# \def	class InputDevice : public ListElement<InputDevice>, public Memory<InputDevice>
	//
	//# \desc
	//# The $InputDevice$ class encapsulates an individual input device that is available for user input.
	//# The types of input devices that can be represented by the $InputDevice$ class include mice, keyboards,
	//# and any kind of game controller. The particular type of input device can be determined by calling
	//# the $@InputDevice::GetDeviceType@$ function.
	//#
	//# Each input device has a tree of input controls attached to it, and this tree always contains at least
	//# one item. Iteration over all of the input controls is done by using the $@InputDevice::GetFirstControl@$
	//# and $@InputDevice::GetNextControl@$ functions.
	//
	//# \base	Utilities/ListElement<InputDevice>	Used internally by the Input Manager.
	//# \base	MemoryMgr/Memory<InputDevice>		Input devices are allocated in a dedicated heap.
	//
	//# \also	$@MouseDevice@$
	//# \also	$@KeyboardDevice@$
	//# \also	$@JoystickDevice@$
	//# \also	$@TouchDevice@$
	//# \also	$@GameDevice@$
	//# \also	$@OrbisDevice@$
	//# \also	$@CellDevice@$
	//# \also	$@XboxDevice@$
	//# \also	$@InputControl@$


	//# \function	InputDevice::GetDeviceType		Returns the type of an input device.
	//
	//# \proto	InputDeviceType GetDeviceType(void) const;
	//
	//# \desc
	//# The $GetDeviceType$ function returns the type of an input device, which can be one of the following values.
	//
	//# \table	InputDeviceType
	//
	//# \also	$@InputDevice::GetDeviceHidUsage@$
	//# \also	$@InputDevice::GetDeviceName@$


	//# \function	InputDevice::GetDeviceHidUsage		Returns the HID usage of an input device.
	//
	//# \proto	unsigned_int32 GetDeviceHidUsage(void) const;
	//
	//# \desc
	//# The $GetDeviceHidUsage$ function returns the 32-bit HID usage value for an input device. The high 16 bits
	//# contain the usage page, and the low 16 bits contain the specific usage value within that page. The Input
	//# Manager recognizes input devices having the following usage pages defined by the HID Specification.
	//
	//# \value	0x0001		Generic Desktop Controls
	//# \value	0x0002		Simulation Controls
	//# \value	0x0004		Sport Controls
	//# \value	0x0005		Game Controls
	//
	//# \desc
	//# Mice, keyboards, joysticks, and most other game input devices belong to the Generic Desktop Controls page.
	//# However, keep in mind that many of the controls belonging to those devices belong to other pages. See the
	//# specific input device subclasses for more information about HID usages for the controls belonging to each
	//# particular type of device.
	//
	//# \also	$@InputDevice::GetDeviceType@$
	//# \also	$@InputDevice::GetDeviceName@$
	//# \also	$@InputControl::GetControlHidUsage@$


	//# \function	InputDevice::GetDeviceName		Returns the name of an input device.
	//
	//# \proto	const char *GetDeviceName(void) const;
	//
	//# \desc
	//# The $GetDeviceName$ function returns a pointer to a string containing the name of an input device.
	//
	//# \also	$@InputDevice::GetDeviceType@$
	//# \also	$@InputDevice::GetDeviceHidUsage@$


	//# \function	InputDevice::GetFirstControl	Returns the first control belonging to an input device.
	//
	//# \proto	InputControl *GetFirstControl(void) const;
	//
	//# \desc
	//# The $GetFirstControl$ function returns a pointer to the first input control belonging to an input device.
	//# All input devices have at least one input control, so the return value is never $nullptr$.
	//#
	//# Input controls are organized into a tree structure. All of the input controls belonging to an input device
	//# can be retrieved by calling the $GetFirstControl$ function function and then repeatedly calling the
	//# $@InputDevice::GetNextControl@$ function as shown in the following code.
	//
	//# \source
	//# InputControl *control = device->GetFirstControl();\n
	//# do\n
	//# {\n
	//#	\t...\n
	//#	\tcontrol = device->GetNextControl(control);\n
	//# } while (control);
	//
	//# \also	$@InputDevice::GetNextControl@$
	//# \also	$@InputDevice::FindControl@$
	//# \also	$@InputControl@$


	//# \function	InputDevice::GetNextControl		Returns the next control in an input device's control tree.
	//
	//# \proto	InputControl *GetNextControl(const InputControl *control) const;
	//
	//# \param	control		The most recently visited control.
	//
	//# \desc
	//# The $GetNextControl$ function returns the next input control in an input device's control tree following
	//# the input control specified by the $control$ parameter. See the $@InputDevice::GetFirstControl@$ function
	//# for information about retrieving all input controls for an input device.
	//
	//# \also	$@InputDevice::GetFirstControl@$
	//# \also	$@InputDevice::FindControl@$
	//# \also	$@InputControl@$


	//# \function	InputDevice::FindControl		Returns the control having a given HID usage or name.
	//
	//# \proto	InputControl *FindControl(unsigned_int32 usage) const;
	//# \proto	InputControl *FindControl(const char *name) const;
	//
	//# \param	usage	The 32-bit HID usage value of the control to find.
	//# \param	name	The name of the control to find.
	//
	//# \desc
	//# The $FindControl$ function searches for an input control having either the 32-bit HID usage value specified
	//# by the $usage$ parameter or the name specified by the $name$ parameter. If a matching input control is
	//# found, then a pointer to it is returned. Otherwise, the return value is $nullptr$.
	//
	//# \also	$@InputDevice::GetFirstControl@$
	//# \also	$@InputDevice::GetNextControl@$
	//# \also	$@InputControl@$


	class InputDevice : public ListElement<InputDevice>, public EngineMemory<InputDevice>
	{
		private:

			InputDeviceType					deviceType;
			unsigned_int32					deviceHidUsage;
			bool							deviceActive;

		protected:

			InputControl					controlTree;
			List<InputFeedback>				feedbackList;

			#if C4MACOS

				IOCFPlugInInterface			**pluginInterface;
				IOHIDDeviceInterface		**deviceInterface;
				IOHIDQueueInterface			**deviceQueue;

			#elif C4PS3 //[ PS3

				// -- PS3 code hidden --

			#endif //]

			InputDeviceName					deviceName;

			#if C4WINDOWS

				InputDevice(InputDeviceType type, unsigned_int32 usage, const DIDEVICEINSTANCEW *instance);
				InputDevice(InputDeviceType type, unsigned_int32 usage, const char *name);

			#elif C4MACOS

				InputDevice(InputDeviceType type, unsigned_int32 usage, io_object_t object, CFMutableDictionaryRef properties);

				void BuildControlTree(InputControl *root, CFMutableDictionaryRef dictionary);

			#elif C4LINUX

				InputDevice(InputDeviceType type, unsigned_int32 usage, const char *name);

			#elif C4IOS //[ MOBILE

				// -- Mobile code hidden --

			#elif C4PS4 //[ PS4

				// -- PS4 code hidden --

			#elif C4PS3 //[ PS3

				// -- PS3 code hidden --

			#endif //]

			virtual void Activate(void);
			virtual void Deactivate(void);

		public:

			virtual ~InputDevice();

			InputDeviceType GetDeviceType(void) const
			{
				return (deviceType);
			}

			unsigned_int32 GetDeviceHidUsage(void) const
			{
				return (deviceHidUsage);
			}

			bool DeviceActive(void) const
			{
				return (deviceActive);
			}

			#if C4PS3 //[ PS3

				// -- PS3 code hidden --

			#endif //]

			const char *GetDeviceName(void) const
			{
				return (deviceName);
			}

			InputControl *GetFirstControl(void) const
			{
				return (controlTree.GetNextNode(&controlTree));
			}

			InputControl *GetNextControl(const InputControl *control) const
			{
				return (controlTree.GetNextNode(control));
			}

			InputFeedback *GetFirstFeedback(void) const
			{
				return (feedbackList.First());
			}

			C4API InputControl *FindControl(unsigned_int32 usage) const;
			C4API InputControl *FindControl(const char *name) const;

			void ResetActions(void) const;

			virtual void SetInputMode(InputMode mode);
			virtual bool ProcessEvents(InputMode mode);
	};


	#if C4WINDOWS

		class DirectInputDevice : public InputDevice
		{
			protected:

				IDirectInputDevice8W		*deviceInstance;

				int32						controlCount;
				int32						buttonCount;

				unsigned_int32				controlTableSize;
				InputControl				**controlTable;

				DirectInputDevice(InputDeviceType type, unsigned_int32 usage, IDirectInput8W *directInput, const DIDEVICEINSTANCEW *instance);

				static BOOL CALLBACK EnumObjectsCallback(const DIDEVICEOBJECTINSTANCEW *instance, void *cookie);

				void BuildDataFormat(void);

				void Activate(void) override;
				void Deactivate(void) override;

			public:

				~DirectInputDevice();

				bool ProcessEvents(InputMode mode) override;
		};

		typedef DirectInputDevice StandardInputDevice;

	#else

		typedef InputDevice StandardInputDevice;

	#endif


	//# \class	MouseDevice		Encapsulates a mouse device.
	//
	//# \def	class MouseDevice final : public InputDevice
	//
	//# \desc
	//# The $MouseDevice$ class encapsulates a mouse device. Each mouse device has a HID usage
	//# value of $0x00010002$ (mouse) or $0x00010001$ (pointer).
	//#
	//# The following table lists the input controls that normally belong to a mouse device and their
	//# corresponding 32-bit HID usage values. A mouse device may have only a subset of these controls,
	//# and it may have additional controls that are not listed here. The axis controls listed in the
	//# table are relative axes represented by the $@DeltaAxisControl@$ class, and the button controls
	//# are generic buttons represented by the $@GenericButtonControl@$ class.
	//
	//# \value	0x00010030		Horizontal movement axis.
	//# \value	0x00010031		Vertical movement axis.
	//# \value	0x00010032		Scroll wheel.
	//# \value	0x00090001		Left button.
	//# \value	0x00090002		Right button.
	//# \value	0x00090003		Middle button.
	//
	//# \base	InputDevice		A $MouseDevice$ is a specific type of input device.
	//
	//# \also	$@KeyboardDevice@$
	//# \also	$@JoystickDevice@$
	//# \also	$@TouchDevice@$
	//# \also	$@GameDevice@$
	//# \also	$@OrbisDevice@$
	//# \also	$@CellDevice@$
	//# \also	$@XboxDevice@$


	class MouseDevice final : public StandardInputDevice
	{
		private:

			#if C4WINDOWS

				void Activate(void) override;

			#elif C4LINUX

				Integer2D				originalPosition;
				Integer2D				currentPosition;
				bool					motionEventFlag;

				GenericButtonControl	leftButton;
				GenericButtonControl	middleButton;
				GenericButtonControl	rightButton;

				DeltaAxisControl		horizontalAxis;
				DeltaAxisControl		verticalAxis;
				DeltaAxisControl		wheelAxis;

				void Activate(void) override;
				void Deactivate(void) override;

			#elif C4PS4 //[ PS4

				// -- PS4 code hidden --

			#elif C4PS3 //[ PS3

				// -- PS3 code hidden --

			#endif //]

		public:

			#if C4WINDOWS

				MouseDevice(IDirectInput8W *directInput, const DIDEVICEINSTANCEW *instance);

			#elif C4MACOS

				MouseDevice(unsigned_int32 usage, io_object_t object, CFMutableDictionaryRef properties);

			#elif C4LINUX

				MouseDevice();

				const Integer2D& GetMouseMotionPosition(void) const
				{
					return (currentPosition);
				}

				bool TakeMouseMotionEventFlag(void)
				{
					bool flag = motionEventFlag;
					motionEventFlag = false;
					return (flag);
				}

				bool ProcessEvents(InputMode mode) override;

				void HandleMouseButtonEvent(InputMode mode, const XButtonEvent *event);
				void HandleMouseMotionEvent(InputMode mode, const XMotionEvent *event);

			#elif C4PS4 //[ PS4

				// -- PS4 code hidden --

			#elif C4PS3 //[ PS3

				// -- PS3 code hidden --

			#endif //]

			~MouseDevice();

			void SetInputMode(InputMode mode) override;
	};


	//# \class	KeyboardDevice		Encapsulates a keyboard device.
	//
	//# \def	class KeyboardDevice final : public InputDevice
	//
	//# \desc
	//# The $KeyboardDevice$ class encapsulates a keyboard device. Each keyboard device has a HID usage
	//# value of $0x00010006$ (keyboard) or $0x00010007$ (keypad).
	//#
	//# The input controls belonging to a keyboard device are always key button controls represented by
	//# the $@KeyButtonControl@$ class. The HID usage page contained in the high 16 bits of the full
	//# 32-bit usage value for each control is always $0x0007$, corresponding to the Keyboard/Keypad
	//# page in the HID Specification. The low 16 bits of the full usage value identify the specific
	//# key represented by each control.
	//
	//# \base	InputDevice		A $KeyboardDevice$ is a specific type of input device.
	//
	//# \also	$@MouseDevice@$
	//# \also	$@JoystickDevice@$
	//# \also	$@TouchDevice@$
	//# \also	$@GameDevice@$
	//# \also	$@OrbisDevice@$
	//# \also	$@CellDevice@$
	//# \also	$@XboxDevice@$


	class KeyboardDevice final : public StandardInputDevice
	{
		private:

			#if C4LINUX

				KeyButtonControl		*keyButton[kKeyboardRawCodeCount];

			#elif C4PS4 //[ PS4

				// -- PS4 code hidden --

			#elif C4PS3 //[ PS3

				// -- PS3 code hidden --

			#endif //]

		public:

			#if C4WINDOWS

				KeyboardDevice(IDirectInput8W *directInput, const DIDEVICEINSTANCEW *instance);

			#elif C4MACOS

				KeyboardDevice(unsigned_int32 usage, io_object_t object, CFMutableDictionaryRef properties);

			#elif C4LINUX

				KeyboardDevice();

				void HandleKeyboardEvent(InputMode mode, const XKeyEvent *event);

			#elif C4PS4 //[ PS4

				// -- PS4 code hidden --

			#elif C4PS3 //[ PS3

				// -- PS3 code hidden --

			#endif //]

			~KeyboardDevice();

			void SetInputMode(InputMode mode) override;
	};


	#if C4DESKTOP

		//# \class	JoystickDevice		Encapsulates a joystick device (desktop only).
		//
		//# \def	class JoystickDevice final : public InputDevice
		//
		//# \desc
		//# The $JoystickDevice$ class encapsulates any generic gaming device, which includes joysticks,
		//# game pads, and genre-specific devices such as steering wheels and rudder pedals. This type of
		//# input device can exist only on desktop platforms.
		//#
		//# The types of the input controls belonging to a joystick device can be rather diverse. Specific
		//# controls such as joystick axes or buttons should be located by examining the HID usage value
		//# assigned to each control, returned by the $@InputControll::GetControlHidUsage@$ function.
		//
		//# \base	InputDevice		A $JoystickDevice$ is a specific type of input device.
		//
		//# \also	$@MouseDevice@$
		//# \also	$@KeyboardDevice@$
		//# \also	$@TouchDevice@$
		//# \also	$@GameDevice@$
		//# \also	$@OrbisDevice@$
		//# \also	$@CellDevice@$
		//# \also	$@XboxDevice@$


		class JoystickDevice final : public StandardInputDevice
		{
			private:

				#if C4WINDOWS

					static BOOL CALLBACK EnumEffectsCallback(const DIEFFECTINFOW *effect, void *cookie);

				#elif C4LINUX

					static js_event						eventData[];

					int									joystickFileDesc;

					Array<GenericButtonControl *, 32>	buttonTable;
					Array<LinearAxisControl *, 32>		axisTable;

					void Activate(void) override;

				#endif

			public:

				#if C4WINDOWS

					JoystickDevice(IDirectInput8W *directInput, const DIDEVICEINSTANCEW *instance);

					bool ProcessEvents(InputMode mode) override;

				#elif C4MACOS

					JoystickDevice(unsigned_int32 usage, io_object_t object, CFMutableDictionaryRef properties);

				#elif C4LINUX

					JoystickDevice(int fileDesc, const char *name);

					bool ProcessEvents(InputMode mode) override;

				#endif

				~JoystickDevice();
		};

	#elif C4IOS //[ MOBILE

		// -- Mobile code hidden --

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]


	#if C4XINPUT

		//# \class	XboxDevice		Encapsulates an Xbox controller device (Windows only).
		//
		//# \def	class XboxDevice final : public InputDevice
		//
		//# \desc
		//# The $XboxDevice$ class encapsulates an Xbox game controller device. This type of
		//# input device can exist only on the Windows platform.
		//#
		//# The following table lists the input controls belonging to an Xbox device and their
		//# corresponding 32-bit HID usage values. All Xbox devices have exactly the input controls
		//# listed in this table and no others.
		//
		//# \value	0x00010030		Left stick <i>x</i> axis.
		//# \value	0x00010031		Left stick <i>y</i> axis.
		//# \value	0x00010032		Right stick <i>x</i> axis.
		//# \value	0x00010035		Right stick <i>y</i> axis.
		//# \value	0x00010033		Left trigger.
		//# \value	0x00010034		Right trigger.
		//# \value	0x00010090		Directional pad up.
		//# \value	0x00010091		Directional pad down.
		//# \value	0x00010092		Directional pad right.
		//# \value	0x00010093		Directional pad down.
		//# \value	0x00090001		A button.
		//# \value	0x00090002		B button.
		//# \value	0x00090003		X button.
		//# \value	0x00090004		Y button.
		//# \value	0x00090005		LB button.
		//# \value	0x00090006		RB button.
		//# \value	0x00090007		Back button.
		//# \value	0x00090008		Start button.
		//# \value	0x00090009		Left stick button.
		//# \value	0x0009000A		Right stick button.
		//
		//# \base	InputDevice		An $XboxDevice$ is a specific type of input device.
		//
		//# \also	$@MouseDevice@$
		//# \also	$@KeyboardDevice@$
		//# \also	$@JoystickDevice@$
		//# \also	$@TouchDevice@$
		//# \also	$@GameDevice@$
		//# \also	$@OrbisDevice@$
		//# \also	$@CellDevice@$


		class XboxDevice final : public InputDevice
		{
			private:

				int32					deviceIndex;
				unsigned_int32			packetNumber;

				bool					buttonState[kXboxButtonCount];
				unsigned_int8			triggerState[kXboxTriggerCount];
				int16					axisState[kXboxAxisCount];

				GenericButtonControl	*buttonControl[kXboxButtonCount];
				SliderControl			*triggerControl[kXboxTriggerCount];
				LinearAxisControl		*axisControl[kXboxAxisCount];

				char					controlStorage[sizeof(GenericButtonControl) * kXboxButtonCount + sizeof(SliderControl) * kXboxTriggerCount + sizeof(LinearAxisControl) * kXboxAxisCount];

			public:

				XboxDevice(int32 index);
				~XboxDevice();

				int32 GetDeviceIndex(void) const
				{
					return (deviceIndex);
				}

				bool ProcessEvents(InputMode mode) override;
		};

	#endif


	//# \class	InputMgr	The Input Manager class.
	//
	//# \def	class InputMgr : public Manager<InputMgr>
	//
	//# \desc
	//# The $InputMgr$ class encapsulates the input device functionality of the C4 Engine.
	//# The single instance of the Input Manager is constructed during an application's initialization
	//# and destroyed at termination.
	//#
	//# The Input Manager's member functions are accessed through the global pointer $TheInputMgr$.
	//
	//# \also	$@InputDevice@$
	//# \also	$@InputControl@$
	//# \also	$@Action@$


	//# \function	InputMgr::GetFirstDevice	Returns the first input device in the Input Manager's device list.
	//
	//# \proto	InputDevice *GetFirstDevice(void) const;
	//
	//# \desc
	//# The $GetFirstDevice$ function returns the first input device in the Input Manager's device list. The return
	//# value can be $nullptr$ if there are no input devices connected to the computer. The entire list of input
	//# devices can be iterated by repeatedly calling the $@Utilities/ListElement::Next@$ function, beginning with
	//# the device returned by the $GetFirstDevice$ function.
	//
	//# \also	$@InputMgr::FindDevice@$


	//# \function	InputMgr::FindDevice		Returns the input device having a given name or type.
	//
	//# \proto	InputDevice *FindDevice(const char *name) const;
	//# \proto	InputDevice *FindDevice(InputDeviceType type) const;
	//# \proto	InputDevice *FindDevice(InputDeviceType type, const char *name) const;
	//
	//# \param	name	The name of the device to find.
	//# \param	type	The type of the device to find.
	//
	//# \desc
	//# The $FindDevice$ function returns either the first input device having the name matching the $name$ parameter
	//# or the first input device having the type matching the $type$ parameter. If both the $name$ and $type$ parameters
	//# are specified, then the first input device with the matching name and type is returned. If no input device matches
	//# the given parameters, then the return value is $nullptr$.
	//
	//# \also	$@InputMgr::GetFirstDevice@$


	//# \function	InputMgr::AddAction			Registers an action object with the Input Manager.
	//
	//# \proto	void AddAction(Action *action);
	//
	//# \param	action		The action object to register.
	//
	//# \desc
	//# The $AddAction$ function registers an $@Action@$ object with the Input Manager. An action is
	//# normally registered by an application at the time that it is initialized.
	//
	//# \also	$@InputMgr::RemoveAction@$
	//# \also	$@Action@$
	//# \also	$@InputControl@$


	//# \function	InputMgr::RemoveAction		Unregisters an action object with the Input Manager.
	//
	//# \proto	void RemoveAction(Action *action);
	//
	//# \param	action		The action object to unregister.
	//
	//# \desc
	//# The $RemoveAction$ function unregisters an $@Action@$ object that was previously registered
	//# with the Input Manager. When an action is unregistered, any input controls to which the action
	//# was assigned have their actions reset to $nullptr$.
	//#
	//# An action is automatically unregistered if it is destroyed.
	//
	//# \also	$@InputMgr::AddAction@$
	//# \also	$@Action@$
	//# \also	$@InputControl@$


	//# \function	InputMgr::SetInputMode		Sets the current mode in which the Input Manager processes input events.
	//
	//# \proto	void SetInputMode(InputMode mode);
	//
	//# \param	mode	The new input mode.
	//
	//# \desc
	//# The $SetInputMode$ function sets the current mode in which the Input Manager processes input events
	//# from various types of input devices. The $mode$ parameter can be a combination (through logical OR)
	//# of the following constants.
	//
	//# \table	InputMode
	//
	//# If the new input mode specifies that events are to be processed by the Input Manager for a particular
	//# type of device, then events for that type of device are sent to input actions and are not captured by
	//# the Interface Manager.
	//#
	//# The default input mode is $kInputInactive$ when the Input Manager is initialized.
	//
	//# \also	$@InterfaceMgr/InterfaceMgr::SetInputManagementMode@$


	//# \div
	//# \function	InputMgr::GetMouseSensitivity		Returns the global mouse sensitivity.
	//
	//# \proto	int32 GetMouseSensitivity(void) const;
	//
	//# \desc
	//# The $GetMouseSensitivity$ function returns the global mouse sensitivity. This value is used as a multiplier
	//# for the raw mouse movement, and it is applied to the values returned by the $@InputMgr::GetMouseDeltaX@$
	//# and $@InputMgr::GetMouseDeltaY@$ functions. The default value of the sensitivity is 15.
	//
	//# \also	$@InputMgr::SetMouseSensitivity@$
	//# \also	$@InputMgr::GetMouseFlags@$
	//# \also	$@InputMgr::SetMouseFlags@$
	//# \also	$@InputMgr::GetMouseDeltaX@$
	//# \also	$@InputMgr::GetMouseDeltaY@$


	//# \function	InputMgr::SetMouseSensitivity		Sets the global mouse sensitivity.
	//
	//# \proto	void SetMouseSensitivity(int32 sensitivity);
	//
	//# \param	sensitivity		The new mouse sensitivity.
	//
	//# \desc
	//# The $SetMouseSensitivity$ function sets the global mouse sensitivity. This value is used as a multiplier
	//# for the raw mouse movement, and it is applied to the values returned by the $@InputMgr::GetMouseDeltaX@$
	//# and $@InputMgr::GetMouseDeltaY@$ functions. The default value of the sensitivity is 15.
	//#
	//# To change the value of the mouse sensitivity persistently over multiple runs of the engine, change the
	//# value of the $sensitivity$ system variable as follows.
	//
	//# \source
	//# TheEngine->GetVariable("sensitivity")->SetIntegerValue(sensitivity);
	//
	//# \also	$@InputMgr::GetMouseSensitivity@$
	//# \also	$@InputMgr::GetMouseFlags@$
	//# \also	$@InputMgr::SetMouseFlags@$
	//# \also	$@InputMgr::GetMouseDeltaX@$
	//# \also	$@InputMgr::GetMouseDeltaY@$


	//# \function	InputMgr::GetMouseFlags		Returns the mouse input flags.
	//
	//# \proto	unsigned_int32 GetMouseFlags(void) const;
	//
	//# \desc
	//# The $GetMouseFlags$ function returns the mouse input flags, which can be a combination (through logical OR)
	//# of the following constants.
	//
	//# \table	MouseFlags
	//
	//# The default value of the mouse flags is $kMouseSmooth$.
	//
	//# \also	$@InputMgr::SetMouseFlags@$
	//# \also	$@InputMgr::GetMouseSensitivity@$
	//# \also	$@InputMgr::SetMouseSensitivity@$
	//# \also	$@InputMgr::GetMouseDeltaX@$
	//# \also	$@InputMgr::GetMouseDeltaY@$


	//# \function	InputMgr::SetMouseFlags		Sets the mouse input flags.
	//
	//# \proto	void SetMouseFlags(unsigned_int32 flags);
	//
	//# \param	flags	The new mouse flags.
	//
	//# \desc
	//# The $SetMouseFlags$ function sets the mouse input flags to the value specified by the $flags$ parameter,
	//# which can be a combination (through logical OR) of the following constants.
	//
	//# \table	MouseFlags
	//
	//# The default value of the mouse flags is $kMouseSmooth$.
	//#
	//# To change the value of the mouse flags persistently over multiple runs of the engine, change the
	//# values of the $invertMouse$ and $smoothMouse$ system variables as follows.
	//
	//# \source
	//# TheEngine->GetVariable("invertMouse")->SetIntegerValue(invert);\n
	//# TheEngine->GetVariable("smoothMouse")->SetIntegerValue(smooth);
	//
	//# \also	$@InputMgr::GetMouseFlags@$
	//# \also	$@InputMgr::GetMouseSensitivity@$
	//# \also	$@InputMgr::SetMouseSensitivity@$
	//# \also	$@InputMgr::GetMouseDeltaX@$
	//# \also	$@InputMgr::GetMouseDeltaY@$


	//# \function	InputMgr::GetMouseDeltaX		Returns the mouse movement delta on the <i>x</i> axis.
	//
	//# \proto	float GetMouseDeltaX(void) const;
	//
	//# \desc
	//# The $GetMouseDeltaX$ function returns the mouse movement delta on the <i>x</i> axis. The delta value is
	//# expressed in generic units that are scaled to be equivalent across all mouse devices and then multiplied
	//# by the current mouse sensitivity.
	//#
	//# The mouse movement delta is updated by the Input Manager only when the current input mode includes
	//# the $kInputMouseActive$ flag.
	//
	//# \also	$@InputMgr::GetMouseDeltaY@$
	//# \also	$@InputMgr::GetMouseSensitivity@$
	//# \also	$@InputMgr::SetMouseSensitivity@$
	//# \also	$@InputMgr::GetMouseFlags@$
	//# \also	$@InputMgr::SetMouseFlags@$


	//# \function	InputMgr::GetMouseDeltaY		Returns the mouse movement delta on the <i>y</i> axis.
	//
	//# \proto	float GetMouseDeltaY(void) const;
	//
	//# \desc
	//# The $GetMouseDeltaY$ function returns the mouse movement delta on the <i>y</i> axis. The delta value is
	//# expressed in generic units that are scaled to be equivalent across all mouse devices and then multiplied
	//# by the current mouse sensitivity. If the $kMouseInverted$ mouse flag is currently set, then the delta value
	//# is negated before it is returned.
	//#
	//# The mouse movement delta is updated by the Input Manager only when the current input mode includes
	//# the $kInputMouseActive$ flag.
	//
	//# \also	$@InputMgr::GetMouseDeltaX@$
	//# \also	$@InputMgr::GetMouseSensitivity@$
	//# \also	$@InputMgr::SetMouseSensitivity@$
	//# \also	$@InputMgr::GetMouseFlags@$
	//# \also	$@InputMgr::SetMouseFlags@$


	//# \div
	//# \function	InputMgr::GetTouchDevice		Returns the instance of the touch device (mobile only).
	//
	//# \proto	TouchDevice *GetTouchDevice(void) const;
	//
	//# \desc
	//# The $GetTouchDevice$ function returns a pointer to the instance of the touch device created by the Input Manager.
	//# Touch button controls can be added to or removed from this device using the functions of the $@TouchDevice@$ class.
	//
	//# \also	$@TouchDevice@$


	//# \div
	//# \function	InputMgr::GetConsoleProc		Returns the procedure invoked when the console key is pressed.
	//
	//# \proto	InputMgr::KeyProc *GetConsoleProc(void) const;
	//
	//# \desc
	//# The $GetConsoleProc$ function returns a pointer to the function that is called whenever the console key is
	//# pressed and the keyboard is in game input mode. By default, the engine installs a console procedure that
	//# opens the built-in console window.
	//#
	//# The $KeyProc$ type is defined as follows.
	//
	//# \code	typedef void KeyProc(void *);
	//
	//# The $GetConsoleProc$ function is normally used to save the current console procedure before it is changed
	//# or set to $nullptr$ with the $@InputMgr::SetConsoleProc@$ function. In this case, the $@InputMgr::GetConsoleCookie@$
	//# function should also be called to save the current cookie value expected by the console procedure. These two
	//# values can be passed to the $SetConsoleProc$ at a later time to restore the previously installed procedure.
	//
	//# \also	$@InputMgr::GetConsoleCookie@$
	//# \also	$@InputMgr::SetConsoleProc@$
	//# \also	$@InputMgr::GetEscapeProc@$
	//# \also	$@InputMgr::GetEscapeCookie@$
	//# \also	$@InputMgr::SetEscapeProc@$


	//# \function	InputMgr::GetConsoleCookie		Returns the cookie passed to the console procedure.
	//
	//# \proto	void *GetConsoleCookie(void) const;
	//
	//# \desc
	//# The $GetConsoleCookie$ function returns the pointer that was previously passed to the $cookie$ parameter of
	//# the $@InputMgr::SetConsoleProc@$ function.
	//#
	//# The $GetConsoleCookie$ function is normally used to save the current cookie value before it is changed
	//# or set to $nullptr$ with the $@InputMgr::SetConsoleProc@$ function. In this case, the $@InputMgr::GetConsoleProc@$
	//# function should also be called to save the current console procedure. These two values can be passed to the
	//# $SetConsoleProc$ at a later time to restore the previously installed procedure.
	//
	//# \also	$@InputMgr::GetConsoleProc@$
	//# \also	$@InputMgr::SetConsoleProc@$
	//# \also	$@InputMgr::GetEscapeProc@$
	//# \also	$@InputMgr::GetEscapeCookie@$
	//# \also	$@InputMgr::SetEscapeProc@$


	//# \function	InputMgr::SetConsoleProc		Sets the procedure invoked when the console key is pressed.
	//
	//# \proto	void SetConsoleProc(KeyProc *proc, void *cookie = nullptr);
	//
	//# \param	proc		A pointer to the function that is called when the console key is pressed.
	//# \param	cookie		A user-defined value that is passed to the console procedure.
	//
	//# \desc
	//# The $SetConsoleProc$ function sets the function that is called whenever the console key is pressed and
	//# the keyboard is in game input mode to that given by the $proc$ parameter. The $cookie$ parameter specifies
	//# a user-defined value that is passed through to the console procedure.
	//#
	//# By default, the engine installs a console procedure that opens the built-in console window.
	//#
	//# Before changing the current console procedure with the $SetConsoleProc$ function, the previous console
	//# procedure and its cookie can be retrieved by calling the $@InputMgr::GetConsoleProc@$ and $@InputMgr::GetConsoleCookie@$
	//# functions. The values returned by these two functions can be saved and later restored by passing them back to the
	//# $SetConsoleProc$ function.
	//#
	//# If the $proc$ parameter is $nullptr$, then any current console procedure is removed, and no new console procedure
	//# is installed. In this case, pressing the console key when the keyboard is in game input mode has no effect.
	//#
	//# The $SetConsoleProc$ function has no effect on whether the console can be opened when the keyboard is in interface event
	//# mode. To enable and disable the console in interface event mode, call the $@InterfaceMgr/InterfaceMgr::EnableConsole@$
	//# and $@InterfaceMgr/InterfaceMgr::DisableConsole@$ functions.
	//
	//# \also	$@InputMgr::GetConsoleProc@$
	//# \also	$@InputMgr::GetConsoleCookie@$
	//# \also	$@InputMgr::GetEscapeProc@$
	//# \also	$@InputMgr::GetEscapeCookie@$
	//# \also	$@InputMgr::SetEscapeProc@$
	//# \also	$@InterfaceMgr/InterfaceMgr::EnableConsole@$
	//# \also	$@InterfaceMgr/InterfaceMgr::DisableConsole@$


	//# \function	InputMgr::GetEscapeProc		Returns the procedure invoked when the escape key is pressed.
	//
	//# \proto	InputMgr::KeyProc *GetEscapeProc(void) const;
	//
	//# \desc
	//# The $GetEscapeProc$ function returns a pointer to the function that is called whenever the escape key is
	//# pressed and the keyboard is in game input mode. By default, there is no escape procedure installed.
	//#
	//# The $KeyProc$ type is defined as follows.
	//
	//# \code	typedef void KeyProc(void *);
	//
	//# The $GetEscapeProc$ function is normally used to save the current escape procedure before it is changed
	//# or set to $nullptr$ with the $@InputMgr::SetEscapeProc@$ function. In this case, the $@InputMgr::GetEscapeCookie@$
	//# function should also be called to save the current cookie value expected by the escape procedure. These two
	//# values can be passed to the $SetEscapeProc$ at a later time to restore the previously installed procedure.
	//
	//# \also	$@InputMgr::GetEscapeCookie@$
	//# \also	$@InputMgr::SetEscapeProc@$
	//# \also	$@InputMgr::GetConsoleProc@$
	//# \also	$@InputMgr::GetConsoleCookie@$
	//# \also	$@InputMgr::SetConsoleProc@$


	//# \function	InputMgr::GetEscapeCookie		Returns the cookie passed to the escape procedure.
	//
	//# \proto	void *GetEscapeCookie(void) const;
	//
	//# \desc
	//# The $GetEscapeCookie$ function returns the pointer that was previously passed to the $cookie$ parameter of
	//# the $@InputMgr::SetEscapeProc@$ function.
	//#
	//# The $GetEscapeCookie$ function is normally used to save the current cookie value before it is changed
	//# or set to $nullptr$ with the $@InputMgr::SetEscapeProc@$ function. In this case, the $@InputMgr::GetEscapeProc@$
	//# function should also be called to save the current escape procedure. These two values can be passed to the
	//# $SetEscapeProc$ at a later time to restore the previously installed procedure.
	//
	//# \also	$@InputMgr::GetEscapeProc@$
	//# \also	$@InputMgr::SetEscapeProc@$
	//# \also	$@InputMgr::GetConsoleProc@$
	//# \also	$@InputMgr::GetConsoleCookie@$
	//# \also	$@InputMgr::SetConsoleProc@$


	//# \function	InputMgr::SetEscapeProc		Sets the procedure invoked when the escape key is pressed.
	//
	//# \proto	void SetEscapeProc(KeyProc *proc, void *cookie = nullptr);
	//
	//# \param	proc		A pointer to the function that is called when the escape key is pressed.
	//# \param	cookie		A user-defined value that is passed to the escape procedure.
	//
	//# \desc
	//# The $SetEscapeProc$ function sets the function that is called whenever the escape key is pressed and
	//# the keyboard is in game input mode to that given by the $proc$ parameter. The $cookie$ parameter specifies
	//# a user-defined value that is passed through to the escape procedure.
	//#
	//# By default, there is no escape procedure installed, and no action is taken when the user presses the escape key.
	//#
	//# Before changing the current escape procedure with the $SetEscapeProc$ function, the previous escape
	//# procedure and its cookie can be retrieved by calling the $@InputMgr::GetEscapeProc@$ and $@InputMgr::GetEscapeCookie@$
	//# functions. The values returned by these two functions can be saved and later restored by passing them back to the
	//# $SetEscapeProc$ function.
	//#
	//# If the $proc$ parameter is $nullptr$, then any current escape procedure is removed, and no new escape procedure
	//# is installed. In this case, pressing the escape key when the keyboard is in game input mode has no effect.
	//#
	//# The escape procedure is not called when the keyboard is in interface event mode. In this case, a keyboard event with
	//# the key code $kKeyCodeEscape$ is generated.
	//
	//# \also	$@InputMgr::GetEscapeProc@$
	//# \also	$@InputMgr::GetEscapeCookie@$
	//# \also	$@InputMgr::GetConsoleProc@$
	//# \also	$@InputMgr::GetConsoleCookie@$
	//# \also	$@InputMgr::SetConsoleProc@$


	class InputMgr : public Manager<InputMgr>
	{
		friend class MouseAction;

		#if C4PS4 //[ PS4

			// -- PS4 code hidden --

		#endif //]

		public:

			typedef void KeyProc(void *);
			typedef void ConfigProc(InputControl *, float, void *);

		private:

			#if C4WINDOWS

				enum
				{
					kMaxXinputDeviceCount	= 16
				};

				LPDIRECTINPUT8W				directInput;

				int32						xinputDeviceCount;
				unsigned_int32				xinputDeviceGuid[kMaxXinputDeviceCount];

				static const wchar_t *FindDeviceSubstring(const wchar_t *string, const wchar_t *substring);
				static unsigned_int32 ReadDeviceHexString(const wchar_t *string);

				static BOOL CALLBACK EnumDevicesCallback(const DIDEVICEINSTANCEW *instance, void *cookie);

			#endif

			C4API static InputMode			inputMode;
			static InputMode				internalInputMode;

			#if C4LINUX

				MouseDevice					*mouseDevice;
				KeyboardDevice				*keyboardDevice;

			#elif C4IOS //[ MOBILE

				// -- Mobile code hidden --

			#elif C4PS4 //[ PS4

				// -- PS4 code hidden --

			#elif C4PS3 //[ PS3

				// -- PS3 code hidden --

			#endif //]

			List<InputDevice>				deviceList;
			List<Action>					actionList;

			ConsoleAction					consoleAction;
			EscapeAction					escapeAction;
			MouseAction						mouseXAction;
			MouseAction						mouseYAction;

			KeyProc							*consoleProc;
			void							*consoleCookie;

			KeyProc							*escapeProc;
			void							*escapeCookie;

			ConfigProc						*configProc;
			void							*configCookie;

			int32							mouseSensitivity;
			unsigned_int32					mouseFlags;

			float							deltaXMultiplier;
			float							deltaYMultiplier;

			float							prevMouseDeltaX;
			float							prevMouseDeltaY;

			float							mouseDeltaX;
			float							mouseDeltaY;

			VariableObserver<InputMgr>		sensitivityObserver;
			VariableObserver<InputMgr>		invertMouseObserver;
			VariableObserver<InputMgr>		smoothMouseObserver;

			#if C4IOS //[ MOBILE

				// -- Mobile code hidden --

			#endif //]

			void HandleSensitivityEvent(Variable *variable);
			void HandleInvertMouseEvent(Variable *variable);
			void HandleSmoothMouseEvent(Variable *variable);

			#if C4LOG_FILE

				void UpdateLog(void) const;

			#endif

		public:

			InputMgr(int);
			~InputMgr();

			EngineResult Construct(void);
			void Destruct(void);

			static InputMode GetInputMode(void)
			{
				return (inputMode);
			}

			static InputMode GetInternalInputMode(void)
			{
				return (internalInputMode);
			}

			InputDevice *GetFirstDevice(void) const
			{
				return (deviceList.First());
			}

			Action *GetFirstAction(void) const
			{
				return (actionList.First());
			}

			void AddAction(Action *action)
			{
				actionList.Append(action);
			}

			void RemoveAction(Action *action)
			{
				action->BreakAllLinks();
				actionList.Remove(action);
			}

			KeyProc *GetConsoleProc(void) const
			{
				return (consoleProc);
			}

			void *GetConsoleCookie(void) const
			{
				return (consoleCookie);
			}

			void SetConsoleProc(KeyProc *proc, void *cookie = nullptr)
			{
				consoleProc = proc;
				consoleCookie = cookie;
			}

			KeyProc *GetEscapeProc(void) const
			{
				return (escapeProc);
			}

			void *GetEscapeCookie(void) const
			{
				return (escapeCookie);
			}

			void SetEscapeProc(KeyProc *proc, void *cookie = nullptr)
			{
				escapeProc = proc;
				escapeCookie = cookie;
			}

			void SetConfigProc(ConfigProc *proc, void *cookie = nullptr)
			{
				configProc = proc;
				configCookie = cookie;
			}

			void CallConfigProc(InputControl *control, float value)
			{
				if (configProc)
				{
					(*configProc)(control, value, configCookie);
				}
			}

			int32 GetMouseSensitivity(void) const
			{
				return (mouseSensitivity);
			}

			unsigned_int32 GetMouseFlags(void) const
			{
				return (mouseFlags);
			}

			float GetMouseDeltaX(void) const
			{
				return (mouseDeltaX * deltaXMultiplier);
			}

			float GetMouseDeltaY(void) const
			{
				return (mouseDeltaY * deltaYMultiplier);
			}

			#if C4LINUX

				void HandleMouseButtonEvent(const XButtonEvent *event)
				{
					mouseDevice->HandleMouseButtonEvent(inputMode, event);
				}

				void HandleMouseMotionEvent(const XMotionEvent *event)
				{
					mouseDevice->HandleMouseMotionEvent(inputMode, event);
				}

				const Integer2D& GetMouseMotionPosition(void) const
				{
					return (mouseDevice->GetMouseMotionPosition());
				}

				bool GetMouseMotionEventFlag(void) const
				{
					return (mouseDevice->TakeMouseMotionEventFlag());
				}

				void HandleKeyboardEvent(const XKeyEvent *event)
				{
					keyboardDevice->HandleKeyboardEvent(inputMode, event);
				}

			#elif C4IOS //[ MOBILE

				// -- Mobile code hidden --

			#elif C4PS4 //[ PS4

				// -- PS4 code hidden --

			#endif //]

			C4API InputDevice *FindDevice(const char *name) const;
			C4API InputDevice *FindDevice(InputDeviceType type) const;
			C4API InputDevice *FindDevice(InputDeviceType type, const char *name) const;
			C4API Action *FindAction(ActionType type) const;

			C4API void SetInputMode(InputMode mode);
			C4API void SetMouseSensitivity(int32 sensitivity);
			C4API void SetMouseFlags(unsigned_int32 flags);

			C4API InputControl *GetActionControl(const Action *action, int32 index = 0);
			C4API void ClearAllControlActions(void);
			C4API void ResetAllActions(void);

			void InputTask(void);
	};


	C4API extern InputMgr *TheInputMgr;
}


#endif

// ZYUQURM
