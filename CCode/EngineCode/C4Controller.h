 

#ifndef C4Controller_h
#define C4Controller_h


//# \component	Controller System
//# \prefix		Controller/


#include "C4Creation.h"
#include "C4Configurable.h"
#include "C4Attributes.h"
#include "C4Connector.h"
#include "C4Messages.h"
#include "C4Time.h"


namespace C4
{
	//# \tree	Controller
	//
	//# \node	PhysicsMgr/MovementController
	//# \node	PhysicsMgr/OscillationController
	//# \node	PhysicsMgr/RotationController
	//# \node	PhysicsMgr/SpinController
	//# \node	PhysicsMgr/PhysicsController
	//# \node	PhysicsMgr/BodyController
	//# \sub
	//#		\node	PhysicsMgr/RigidBodyController
	//#		\sub
	//#			\node	PhysicsMgr/CharacterController
	//#		\end
	//#		\node	PhysicsMgr/DeformableBodyController
	//#		\sub
	//#			\node	PhysicsMgr/RopeController
	//#			\node	PhysicsMgr/ClothController
	//#		\end
	//# \end
	//# \node	PhysicsMgr/WaterController
	//# \node	ScriptController
	//# \sub
	//#		\node	EffectMgr/PanelController
	//# \end
	//# \node	WorldMgr/MorphController
	//# \sub
	//#		\node	WorldMgr/SkinController
	//# \end
	//# \node	WorldMgr/AnimationController
	//# \node	DistributionController
	//# \node	ShaderParameterController
	//# \node	StrobeController
	//# \node	FlickerController
	//# \node	FlashController


	typedef Type	FunctionType;
	typedef Type	ControllerType;


	enum : ControllerType
	{
		kControllerGeneric				= 0,
		kControllerUnknown				= '!UNK',
		kControllerDistribution			= 'DSTB',
		kControllerShaderParameter		= 'SHPM',
		kControllerStrobe				= 'STRB',
		kControllerFlicker				= 'FLCK',
		kControllerFlash				= 'FLSH'
	};


	enum
	{
		kControllerUnassigned			= -1
	};


	//# \enum	FunctionFlags

	enum
	{
		kFunctionRemote					= 1 << 0,		//## The function executes on remote machines instead of only on the server.
		kFunctionJournaled				= 1 << 1,		//## The message used to execute a remote function should be journaled.
		kFunctionOutputValue			= 1 << 2		//## The function generates an output value that can be stored in a script variable.
	};


	//# \enum	ControllerFlags

	enum
	{
		kControllerUpdate				= 1 << 0,		//## The controller needs to be updated (read-only flag).
		kControllerAsleep				= 1 << 1,		//## The controller is asleep, and thus its $Move$ function is not called (read-only flag).
		kControllerLocal				= 1 << 2,		//## The controller operates autonomously and does receive messages from remote machines.
		kControllerMoveInhibit			= 1 << 3,		//## The controller's $Move$ function is never called, even if the controller is awake.
		kControllerPhysicsSimulation	= 1 << 4		//## The controller is a global physics simulation controller. This should only be set by a $Controller$ subclass that acts as the main interface between the engine and a physics library, and it indicates to the World Manager that the controller should be given special treatment as the sole physics controller in a world.
	};


	enum : EventType
	{
		kEventControllerActivate		= 'CTAC', 
		kEventControllerDeactivate		= 'CTDA'
	};
 

	//# \enum	InteractionEventType 

	enum InteractionEventType
	{ 
		kInteractionEventEngage,						//## The user has begun looking at an interactive object.
		kInteractionEventDisengage,						//## The user has stopped looking at an interactive object. 
		kInteractionEventActivate,						//## The user has explicitly activated an interactive object (e.g., with a mouse click). 
		kInteractionEventDeactivate,					//## The user has released the input control used to activate an interactive object.
		kInteractionEventTrack							//## The user has changed the interaction position.
	};
 

	//# \enum	DistributionFlags

	enum
	{
		kDistributionSelf				= 1 << 0,		//## Distribute messages to the target node itself.
		kDistributionSubnodes			= 1 << 1,		//## Distribute messages to all immediate subnodes of the target node.
		kDistributionConnectors			= 1 << 2		//## Distribute messages to all nodes linked through connectors belonging to the target node.
	};


	enum
	{
		kFlickerInitialized				= 1 << 0
	};


	//# \enum	FlashFlags

	enum
	{
		kFlashInitialized				= 1 << 0,
		kFlashNonpersistent				= 1 << 1		//## The light node is deleted after the flash effect has finished.
	};


	class Function;
	class FunctionMethod;
	class ScriptState;
	class Controller;
	class ControllerRegistration;


	//# \class	FunctionRegistration	Contains information about an application-defined controller function.
	//
	//# The $FunctionRegistration$ class contains information about an application-defined controller function.
	//
	//# \def	class FunctionRegistration : public MapElement<FunctionRegistration>
	//
	//# \ctor	FunctionRegistration(ControllerRegistration *reg, FunctionType type, const char *name, unsigned_int32 flags = 0);
	//
	//# \param	reg		A pointer to the registration object for the controller to which the function pertains.
	//# \param	type	The function type.
	//# \param	name	The function name.
	//# \param	flags	The function flags.
	//
	//# \desc
	//# The $FunctionRegistration$ class is abstract and serves as the common base class for the template class
	//# $@FunctionReg@$. A custom function is registered with the engine by instantiating an object of type
	//# $FunctionReg<classType>$, where $classType$ is the type of the function subclass being registered.
	//#
	//# The $flags$ parameter can be a combination (through logical OR) of the following constants.
	//
	//# \table	FunctionFlags
	//
	//# \base	Utilities/MapElement<FunctionRegistration>	Registration objects are stored in a map container by the engine.
	//
	//# \also	$@FunctionReg@$
	//# \also	$@Function@$
	//# \also	$@Controller@$
	//# \also	$@ControllerRegistration@$


	//# \function	FunctionRegistration::GetFunctionType		Returns the registered function type.
	//
	//# \proto	FunctionType GetFunctionType(void) const;
	//
	//# \desc
	//# The $GetKey$ function returns the function type for a particular function registration.
	//# The function type is established when the function registration is constructed.
	//
	//# \also	$@FunctionRegistration::GetFunctionName@$


	//# \function	FunctionRegistration::GetFunctionName		Returns the human-readable function name.
	//
	//# \proto	const char *GetFunctionName(void) const;
	//
	//# \desc
	//# The $GetFunctionName$ function returns the human-readable function name for a particular function registration.
	//# The function name is established when the function registration is constructed.
	//
	//# \also	$@FunctionRegistration::GetFunctionType@$


	class FunctionRegistration : public MapElement<FunctionRegistration>
	{
		private:

			FunctionType	functionType;
			unsigned_int32	functionFlags;
			const char		*functionName;

		protected:

			C4API FunctionRegistration(ControllerRegistration *reg, FunctionType type, const char *name, unsigned_int32 flags = 0);

		public:

			typedef FunctionType KeyType;

			C4API ~FunctionRegistration();

			KeyType GetKey(void) const
			{
				return (functionType);
			}

			FunctionType GetFunctionType(void) const
			{
				return (functionType);
			}

			unsigned_int32 GetFunctionFlags(void) const
			{
				return (functionFlags);
			}

			const char *GetFunctionName(void) const
			{
				return (functionName);
			}

			virtual Function *Create(void) const = 0;
	};


	//# \class	FunctionReg		Represents a custom function type.
	//
	//# The $FunctionReg$ class represents a custom function type.
	//
	//# \def	template <class classType> class FunctionReg : public FunctionRegistration
	//
	//# \tparam	classType	The custom function class.
	//
	//# \ctor	FunctionReg(ControllerRegistration *reg, FunctionType type, const char *name, unsigned_int32 flags = 0);
	//
	//# \param	reg		A pointer to the registration object for the controller to which the function pertains.
	//# \param	type	The function type.
	//# \param	name	The function name.
	//# \param	flags	The function flags.
	//
	//# \desc
	//# The $FunctionReg$ template class is used to advertise the existence of a custom function type for a particular.
	//# type of controller. The World Manager uses a function registration to construct a custom function, and the
	//# Script Editor displays a list of register functions for the type of controller that each method operates on.
	//# The act of instantiating a $FunctionReg$ object automatically registers the corresponding function
	//# type. The function type is unregistered when the $FunctionReg$ object is destroyed.
	//#
	//# The $flags$ parameter can be a combination (through logical OR) of the following constants.
	//
	//# \table	FunctionFlags
	//
	//# No more than one function registration should be created for each distinct function type.
	//
	//# \base	FunctionRegistration	All specific function registration classes share the common base class $FunctionRegistration$.
	//
	//# \also	$@Function@$
	//# \also	$@Controller@$


	template <class classType> class FunctionReg : public FunctionRegistration
	{
		public:

			FunctionReg(ControllerRegistration *reg, FunctionType type, const char *name, unsigned_int32 flags = 0) : FunctionRegistration(reg, type, name, flags)
			{
			}

			Function *Create(void) const
			{
				return (new classType);
			}
	};


	//# \class	Function	The base class for all controller function objects.
	//
	//# Every function object that is exposed by a controller is a subclass of the $Function$ class.
	//
	//# \def	class Function : public Completable<Function>, public Packable, public Configurable
	//
	//# \ctor	Function(FunctionType funcType, ControllerType contType);
	//
	//# \param	funcType	The function type.
	//# \param	contType	The controller type to which the function applies.
	//
	//# \desc
	//# The $Function$ class is the base class for all controller function objects. A $Function$ subclass represents
	//# a specific function, associated with a particular type of controller, that can be called from a script.
	//
	//# \base	Utilities/Completable<Property>			A function calls its completion procedure when it has finished.
	//# \base	ResourceMgr/Packable					Function objects can be packed for storage in resources.
	//# \base	InterfaceMgr/Configurable				Function objects can define configurable parameters that are exposed
	//#													as user interface widgets in the Script Editor.
	//
	//# \also	$@FunctionReg@$
	//# \also	$@Controller@$


	//# \function	Function::GetFunctionType		Returns the function type.
	//
	//# \proto	FunctionType GetFunctionType(void) const;
	//
	//# \desc
	//# The $GetFunctionType$ function returns the function type.
	//
	//# \also	$@FunctionReg@$


	class Function : public Completable<Function>, public Packable, public Configurable
	{
		private:

			FunctionType		functionType;
			ControllerType		controllerType;

			virtual Function *Replicate(void) const = 0;

		protected:

			C4API Function(FunctionType funcType, ControllerType contType);
			C4API Function(const Function& function);

		public:

			C4API ~Function();

			FunctionType GetFunctionType(void) const
			{
				return (functionType);
			}

			ControllerType GetControllerType(void) const
			{
				return (controllerType);
			}

			Function *Clone(void) const
			{
				return (Replicate());
			}

			C4API void Pack(Packer& data, unsigned_int32 packFlags) const override;
			C4API void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			C4API virtual void Compress(Compressor& data) const;
			C4API virtual bool Decompress(Decompressor& data);

			C4API virtual bool OverridesFunction(const Function *function) const;

			C4API virtual void Preprocess(Controller *controller, FunctionMethod *method, const ScriptState *state);
			C4API virtual void Execute(Controller *controller, FunctionMethod *method, const ScriptState *state);
			C4API virtual void Resume(Controller *controller, FunctionMethod *method, const ScriptState *state);
	};


	//# \class	ControllerRegistration		Manages internal registration information for a custom controller type.
	//
	//# The $ControllerRegistration$ class manages internal registration information for a custom controller type.
	//
	//# \def	class ControllerRegistration : public Registration<Controller, ControllerRegistration>
	//
	//# \ctor	ControllerRegistration(ControllerType type, const char *name);
	//
	//# \param	type	The controller type.
	//# \param	name	The controller name.
	//
	//# \desc
	//# The $ControllerRegistration$ class is abstract and serves as the common base class for the template class
	//# $@ControllerReg@$. A custom controller is registered with the engine by instantiating an object of type
	//# $ControllerReg<classType>$, where $classType$ is the type of the controller subclass being registered.
	//
	//# \base	System/Registration<Controller, ControllerRegistration>		A controller registration is a specific type of registration object.
	//
	//# \also	$@ControllerReg@$
	//# \also	$@Controller@$


	//# \function	ControllerRegistration::GetControllerType		Returns the registered controller type.
	//
	//# \proto	ControllerType GetControllerType(void) const;
	//
	//# \desc
	//# The $GetControllerType$ function returns the controller type for a particular controller registration.
	//# The controller type is established when the controller registration is constructed.
	//
	//# \also	$@ControllerRegistration::GetControllerName@$


	//# \function	ControllerRegistration::GetControllerName		Returns the human-readable controller name.
	//
	//# \proto	const char *GetControllerName(void) const;
	//
	//# \desc
	//# The $GetControllerName$ function returns the human-readable controller name for a particular controller registration.
	//# The controller name is established when the controller registration is constructed.
	//
	//# \also	$@ControllerRegistration::GetControllerType@$


	//# \function	ControllerRegistration::GetFirstFunctionRegistration	Returns the first function registration.
	//
	//# \proto	FunctionRegistration *GetFirstFunctionRegistration(void);
	//
	//# \desc
	//# The $GetFirstFunctionRegistration$ function returns a pointer to the registration object corresponding
	//# to the first function registration for the controller type represented by the controller registration object.
	//# The entire list of registrations can be iterated by calling the $@Utilities/MapElement::Next@$ function on
	//# the returned object and continuing until $nullptr$ is returned.
	//
	//# \also	$@ControllerRegistration::FindFunctionRegistration@$


	//# \function	ControllerRegistration::FindFunctionRegistration		Returns a specific function registration.
	//
	//# \proto	FunctionRegistration *FindFunctionRegistration(Type type);
	//
	//# \param	type	The type of the subclass.
	//
	//# \desc
	//# The $FindFunctionRegistration$ function returns a pointer to the registration object corresponding
	//# to the function type specified by the $type$ parameter for the controller type represented by the
	//# controller registration object. If no such registration exists, then the return value is $nullptr$.
	//
	//# \also	$@ControllerRegistration::GetFirstFunctionRegistration@$


	class ControllerRegistration : public Registration<Controller, ControllerRegistration>
	{
		friend class FunctionRegistration;

		private:

			const char					*controllerName;

			Map<FunctionRegistration>	functionMap;

		protected:

			C4API ControllerRegistration(ControllerType type, const char *name);

		public:

			C4API ~ControllerRegistration();

			ControllerType GetControllerType(void) const
			{
				return (GetRegistrableType());
			}

			const char *GetControllerName(void) const
			{
				return (controllerName);
			}

			FunctionRegistration *GetFirstFunctionRegistration(void) const
			{
				return (functionMap.First());
			}

			FunctionRegistration *FindFunctionRegistration(FunctionType type) const
			{
				return (functionMap.Find(type));
			}

			FunctionRegistration *GetFunctionRegistration(int32 index) const
			{
				return (functionMap[index]);
			}

			int32 GetFunctionRegistrationCount(void) const
			{
				return (functionMap.GetElementCount());
			}

			C4API Function *CreateFunction(FunctionType type) const;

			virtual bool ValidNode(const Node *node) const = 0;
	};


	//# \class	ControllerReg	 Represents a custom controller type.
	//
	//# The $ControllerReg$ class represents a custom controller type.
	//
	//# \def	template <class classType> class ControllerReg : public ControllerRegistration
	//
	//# \tparam	classType	The custom controller class.
	//
	//# \ctor	ControllerReg(ControllerType type, const char *name);
	//
	//# \param	type	The controller type.
	//# \param	name	The controller name.
	//
	//# \desc
	//# The $ControllerReg$ template class is used to advertise the existence of a custom controller type.
	//# The World Manager uses a controller registration to construct a custom controller, and the World Editor
	//# examines a controller registration to determine what type of node a custom controller can be assigned to.
	//# The act of instantiating a $ControllerReg$ object automatically registers the corresponding controller
	//# type. The controller type is unregistered when the $ControllerReg$ object is destroyed.
	//#
	//# No more than one controller registration should be created for each distinct controller type.
	//
	//# \base	ControllerRegistration	All specific controller registration classes share the common base class $ControllerRegistration$.
	//
	//# \also	$@Controller@$


	template <class classType> class ControllerReg : public ControllerRegistration
	{
		public:

			ControllerReg(ControllerType type, const char *name) : ControllerRegistration(type, name)
			{
			}

			Controller *Create(void) const
			{
				return (new classType);
			}

			bool ValidNode(const Node *node) const override
			{
				return ((GetControllerName()) && (classType::ValidNode(node)));
			}
	};


	//# \class	Controller		Manages a dynamic node in a world.
	//
	//# The $Controller$ class manages a dynamic node in a world.
	//
	//# \def	class Controller : public HashTableElement<Controller>, public ListElement<Controller>, public Observable<Controller, EventType>,
	//# \def2	public Packable, public Configurable, public Registrable<Controller, ControllerRegistration>
	//
	//# \ctor	Controller(ControllerType type = kControllerGeneric);
	//
	//# \param	type		The controller type.
	//
	//# \desc
	//# The $Controller$ class is the general mechanism through which dynamic nodes are managed in a world.
	//# Any node that moves for almost any reason is controlled by a specialized subclass of the $Controller$ class.
	//# The $Controller$ class also serves as the point of communication for nodes that need to be synchronized in
	//# a multiplayer environment.
	//
	//# \base	Utilities/HashTableElement<Controller>					Used internally by the World Manager.
	//# \base	Utilities/ListElement<Controller>						Used internally by the World Manager.
	//# \base	Utilities/Observable<Controller, EventType>				Controllers can post events that are handled by observers.
	//# \base	ResourceMgr/Packable									Controllers can be packed for storage in resources.
	//# \base	InterfaceMgr/Configurable								Controllers can define configurable parameters that are exposed as user interface widgets in the World Editor.
	//# \base	System/Registrable<Controller, ControllerRegistration>	Custom controller types can be registered with the engine.
	//
	//# \also	$@ControllerReg@$


	//# \function	Controller::New		Constructs a new controller of a particular type.
	//
	//# \proto	static Controller *New(ControllerType type);
	//
	//# \param	type	The type of controller to construct.
	//
	//# \desc
	//# The $New$ function constructs a new controller having the type specified by the $type$ parameter.
	//
	//# \also	$@ControllerReg@$


	//# \function	Controller::GetControllerType		Returns the controller type.
	//
	//# \proto	ControllerType GetControllerType(void) const;
	//
	//# \desc
	//# The $GetControllerType$ function returns the controller type.
	//
	//# \also	$@ControllerReg@$


	//# \function	Controller::GetControllerFlags		Returns the controller flags.
	//
	//# \proto	unsigned_int32 GetControllerFlags(void) const;
	//
	//# \desc
	//# The $GetControllerFlags$ function returns the controller flags, which can be a combination (through
	//# logical OR) of the following constants.
	//
	//# \table	ControllerFlags
	//
	//# \also	$@Controller::SetControllerFlags@$


	//# \function	Controller::SetControllerFlags		Sets the controller flags.
	//
	//# \proto	void GetControllerFlags(unsigned_int32 flags);
	//
	//# \param	flags	The new controller flags.
	//
	//# \desc
	//# The $SetControllerFlags$ function sets the controller flags to the value specified by the $flags$
	//# parameter, which can be a combination (through logical OR) of the following constants.
	//
	//# \table	ControllerFlags
	//
	//# \also	$@Controller::GetControllerFlags@$


	//# \function	Controller::GetControllerIndex		Returns the World Manager controller index.
	//
	//# \proto	int32 GetControllerIndex(void) const;
	//
	//# \desc
	//# The $GetControllerIndex$ function returns the controller index that is assigned by the World Manager.
	//# Every controller is assigned a unique index by the server machine in a multiplayer game so that a
	//# particular controller can be identified on every machine in the game. A controller index is
	//# passed to the constructor of any $@MessageMgr/ControllerMessage@$ object in order to specify the
	//# message's destination.
	//
	//# \also	$@ControllerReg@$
	//# \also	$@MessageMgr/ControllerMessage@$


	//# \function	Controller::GetTargetNode		Returns the node to which a controller is attached.
	//
	//# \proto	Node *GetTargetNode(void) const;
	//
	//# \desc
	//# The $GetTargetNode$ function returns the node to which a controller is attached.
	//# A controller is attached to a node using the $@WorldMgr/Node::SetController@$ function.
	//
	//# \also	$@WorldMgr/Node::SetController@$


	//# \function	Controller::ValidNode		Returns a boolean value indicating whether the controller can be assigned to a particular node.
	//
	//# \proto	static bool ValidNode(const Node *node);
	//
	//# \desc
	//# The $ValidNode$ function should be redefined by controller subclasses. Its implementation should examine the
	//# node pointed to by the $node$ parameter and return $true$ if the controller type can be used with the node.
	//# If the controller type cannot be used, the $ValidNode$ function should return $false$. If the $ValidNode$
	//# function is not redefined for a registered subclass of the $Controller$ class, then that controller type
	//# can be assigned to any node.


	//# \function	Controller::InstanceExtractable		Returns a boolean value indicating whether the controller's target node should be extracted from an instanced world.
	//
	//# \proto	virtual bool InstanceExtractable(void) const;
	//
	//# \desc
	//# The $InstanceExtractable$ function indicates whether a controller contains state that must be preserved when a
	//# world is saved and restored. By default, this function returns <code>false</code>. If a subclass of the $Controller$
	//# class overrides this function and returns <code>true</code>, then it causes the nodes of any instanced world
	//# containing the controller to be extracted and placed in the enclosing world. This allows the controller's state
	//# to be saved and restored with the enclosing world instead of being reloaded from the original instanced world.
	//
	//# \also	$@WorldMgr/Instance@$


	//# \function	Controller::Activate		Called when a controller is activated by some kind of trigger.
	//
	//# \proto	virtual void Activate(Node *initiator, Node *trigger);
	//
	//# \param	initiator	The node that activated the trigger. This can be $nullptr$.
	//# \param	trigger		A pointer to the node that caused the controller to be activated. This can be $nullptr$.
	//
	//# \desc
	//# The $Activate$ function is called when some kind of trigger in the world causes a controller to be activated.
	//# This function can be called because a $@WorldMgr/Trigger@$ node was activated, or it can be called by a script.
	//# A controller subclass may perform whatever action is appropriate in response to the activation.
	//#
	//# The default implementation performs no action, so any override of the $Activate$ function does not need to
	//# call the base class counterpart.
	//
	//# \also	$@Controller::Deactivate@$
	//# \also	$@WorldMgr/World::ActivateTriggers@$
	//# \also	$@WorldMgr/Trigger@$


	//# \function	Controller::Deactivate		Called when the trigger node that activated a controller is deactivated.
	//
	//# \proto	virtual void Deactivate(Node *initiator, Node *trigger);
	//
	//# \param	initiator	The node that deactivated the trigger. This can be $nullptr$.
	//# \param	trigger		A pointer to the node that caused the controller to be deactivated. This can be $nullptr$.
	//
	//# \desc
	//# The $Deactivate$ function is called when a $@WorldMgr/Trigger@$ node that previously activated the controller
	//# becomes deactivated. A controller subclass may perform whatever action is appropriate in response to the deactivation.
	//#
	//# The default implementation performs no action, so any override of the $Deactivate$ function does not need to
	//# call the base class counterpart.
	//
	//# \also	$@Controller::Activate@$
	//# \also	$@WorldMgr/World::ActivateTriggers@$
	//# \also	$@WorldMgr/Trigger@$


	//# \div
	//# \function	Controller::Preprocess		Performs any preprocessing that a controller needs to do before being used in a world.
	//
	//# \proto	virtual void Preprocess(void);
	//
	//# \desc
	//# The $Preprocess$ function is called when the node to which a controller is attached is preprocessed.
	//# A controller subclass may perform whatever action is necessary to initialize the controller.
	//#
	//# Any override of the $Preprocess$ function should always call the base class counterpart,
	//# or the controller will not function correctly.
	//
	//# \special
	//# It is often the case that a controller will not want to perform certain initialization while it is being preprocessed inside
	//# the World Editor, but only when a game is actually being played. A controller can determine whether it is running inside the
	//# World Editor by calling the $GetManipulator$ function for its target node. If the return value is not $nullptr$, then the
	//# controller is running inside the World Editor. If the return value is $nullptr$, then the world to which the target node
	//# belongs is actually being played.
	//
	//# \also	$@WorldMgr/Node::Preprocess@$


	//# \function	Controller::Wake		Wakes a sleeping controller.
	//
	//# \proto	virtual void Wake(void);
	//
	//# \desc
	//# The $Wake$ function wakes a controller that was previously placed in the sleeping state by the $@Controller::Sleep@$ function.
	//# The $@Controller::Move@$ function is called only for controllers that are currently in the non-sleeping state.
	//#
	//# Any override of the $Wake$ function should always call the base class counterpart.
	//
	//# \also	$@Controller::Sleep@$
	//# \also	$@Controller::Move@$


	//# \function	Controller::Sleep		Puts a controller to sleep.
	//
	//# \proto	virtual void Sleep(void);
	//
	//# \desc
	//# The $Sleep$ function puts a controller to sleep so that it receives no processing time. The $@Controller::Move@$ function
	//# is not called for controllers that are currently in the sleeping state. A controller can be returned to the non-sleeping
	//# state by calling the $@Controller::Wake@$ function.
	//#
	//# Any override of the $Sleep$ function should always call the base class counterpart.
	//
	//# \also	$@Controller::Wake@$
	//# \also	$@Controller::Move@$


	//# \function	Controller::Move		Performs any per-frame movement or processing that a controller needs to do.
	//
	//# \proto	virtual void Move(void);
	//
	//# \desc
	//# The $Move$ function is called once per frame for all non-sleeping controllers in a world.
	//# A controller subclass may perform whatever action is appropriate to move its target node.
	//#
	//# The default implementation performs no action, so any override of the $Move$ function does not need to
	//# call the base class counterpart.
	//
	//# \also	$@Controller::Sleep@$
	//# \also	$@Controller::Wake@$


	//# \function	Controller::Update		Performs any processing that must be done before the node to which a controller is attached is rendered.
	//
	//# \proto	virtual void Update(void);
	//
	//# \desc
	//# The $Update$ function is called when a controller has been invalidated and its target node is about to be
	//# rendered. This gives the controller an opportunity to perform any calculations that could be deferred until
	//# its target node actually became visible.
	//#
	//# Any override of the $Update$ function should always call the base class counterpart to clear the update flag.
	//
	//# \also	$@Controller::Invalidate@$


	//# \function	Controller::SetDetailLevel		Called when the detail level for the target node changes.
	//
	//# \proto	virtual void SetDetailLevel(int32 level);
	//
	//# \param	level	The new detail level.
	//
	//# \desc
	//# The $SetDetailLevel$ function is called when a controller is attached to a geometry node, and the level of
	//# detail for that geometry node changes. The new level of detail is given by the $level$ parameter.
	//#
	//# Any override of the $SetDetailLevel$ function should always call the base class counterpart.


	//# \function	Controller::Invalidate		Indicates that a controller needs to be updated.
	//
	//# \proto	void Invalidate(void);
	//
	//# \desc
	//# The $Invalidate$ function should be called to indicate that a controller should be updated before the next
	//# time it is rendered. When a controller's target node is visible and the controller has been invalidated,
	//# the $@Controller::Update@$ function is called by the World Manager.
	//
	//# \also	$@Controller::Update@$


	//# \div
	//# \function	Controller::CreateMessage		Called to create a cotroller-defined message.
	//
	//# \proto	virtual ControllerMessage *CreateMessage(ControllerMessageType type) const;
	//
	//# \param	type	The controller-specific type of the message to construct.
	//
	//# \desc
	//# The $CreateMessage$ function is called when the Message Manager needs to construct a network message
	//# for a particular controller. The overriding implementation should examine the $type$ parameter and
	//# return a newly constructed instance of the appropriate message class. If the type is not recognized,
	//# then the base class counterpart should be called to construct the message.
	//
	//# \also	$@Controller::ReceiveMessage@$
	//# \also	$@MessageMgr/ControllerMessage@$


	//# \function	Controller::ReceiveMessage		Called to process a controller-defined message.
	//
	//# \proto	virtual void ReceiveMessage(const ControllerMessage *message);
	//
	//# \param	message		The message to process.
	//
	//# \desc
	//# The $ReceiveMessage$ function is called when the Message Manager successfully receives a network message
	//# for a particular controller. The overriding implementation should examine the type of the message and
	//# process it if the type is recognized. Otherwise, the base class counterpart should be called to process
	//# the message.
	//
	//# \also	$@Controller::CreateMessage@$
	//# \also	$@MessageMgr/ControllerMessage@$


	//# \function	Controller::SendInitialStateMessages	Called to send messages containing the controller's state to a new player.
	//
	//# \proto	virtual void SendInitialStateMessages(Player *player) const;
	//
	//# \param	player		The player to which initial state messages should be sent.
	//
	//# \desc
	//# The $SendInitialStateMessages$ function is called for every controller in a world when a new client
	//# machine joins a multiplayer game. This function should send any messages necessary to synchronize the
	//# initial state of the controller on the new client by calling the $@MessageMgr/Player::SendMessage@$
	//# for the player specified by the $player$ parameter.
	//#
	//# This function is called on the server machine after the $kPlayerInitialized$ event has been sent to
	//# the $@System/Application::HandlePlayerEvent@$ function and before the message journal is transmitted
	//# to a new client machine.
	//
	//# \also	$@MessageMgr/Player::SendMessage@$


	//# \function	Controller::HandleInteractionEvent		Called to handle an event for an interactive node.
	//
	//# \proto	virtual void HandleInteractionEvent(InteractionEventType type, const Point3D *position, Node *initiator = nullptr);
	//
	//# \param	type		The event type. See the list of possible events below.
	//# \param	position	The position, in the target node's local coordinate system, where the event occurred.
	//# \param	initiator	The node that caused the event to occur.
	//
	//# \desc
	//# The $HandleInteractionEvent$ function is called when an event needs to be handled for an interactive node.
	//# The $type$ parameter specifies the type of event that occurred and can be one of the following constants.
	//
	//# \table	InteractionEventType
	//
	//# By default, a controller activates itself when the $kInteractionEventActivate$ event is received, and no
	//# action is taken for other types of events.


	class Controller : public HashTableElement<Controller>, public ListElement<Controller>, public Observable<Controller, EventType>, public Packable, public Configurable, public Registrable<Controller, ControllerRegistration>
	{
		friend class Node;

		private:

			ControllerType				controllerType;
			ControllerType				baseControllerType;

			unsigned_int32				controllerFlags;
			int32						controllerIndex;

			Node						*targetNode;

			List<ControllerMessage>		journaledMessageList;

			C4API virtual Controller *Replicate(void) const;

			static Controller *CreateUnknown(Unpacker& data, unsigned_int32 unpackFlags);

			void SetTargetNode(Node *node);

		protected:

			C4API Controller(const Controller& controller);

			void SetBaseControllerType(ControllerType type)
			{
				baseControllerType = type;
			}

		public:

			typedef int32 KeyType;

			enum
			{
				kEventControllerColorUpdate				= 'CTCU'
			};

			enum
			{
				kControllerMessageSetting				= 255,
				kControllerMessageFunction				= 254,
				kControllerMessageWake					= 253,
				kControllerMessageSleep					= 252,
				kControllerMessageEnableNode			= 251,
				kControllerMessageDisableNode			= 250,
				kControllerMessageDeleteNode			= 249,
				kControllerMessageEnableInteractivity	= 248,
				kControllerMessageDisableInteractivity	= 247,
				kControllerMessageShowGeometry			= 246,
				kControllerMessageHideGeometry			= 245,
				kControllerMessageBreakJoint			= 244,
				kControllerMessagePlaySource			= 243,
				kControllerMessageStopSource			= 242,
				kControllerMessageMaterialColor			= 241,
				kControllerMessageShaderParameter		= 240
			};

			C4API Controller(ControllerType type = kControllerGeneric);
			C4API virtual ~Controller();

			int32 GetKey(void) const
			{
				return (controllerIndex);
			}

			static unsigned_int32 Hash(KeyType key)
			{
				return (key * 4813);
			}

			ControllerType GetControllerType(void) const
			{
				return (controllerType);
			}

			ControllerType GetBaseControllerType(void) const
			{
				return (baseControllerType);
			}

			unsigned_int32 GetControllerFlags(void) const
			{
				return (controllerFlags);
			}

			void SetControllerFlags(unsigned_int32 flags)
			{
				controllerFlags = flags;
			}

			int32 GetControllerIndex(void) const
			{
				return (controllerIndex);
			}

			void SetControllerIndex(int32 index)
			{
				controllerIndex = index;
			}

			Node *GetTargetNode(void) const
			{
				return (targetNode);
			}

			bool Asleep(void) const
			{
				return ((controllerFlags & kControllerAsleep) != 0);
			}

			void Invalidate(void)
			{
				controllerFlags |= kControllerUpdate;
			}

			ControllerMessage *GetFirstJournaledMessage(void) const
			{
				return (journaledMessageList.First());
			}

			void AddJournaledMessage(ControllerMessage *message)
			{
				journaledMessageList.Append(message);
			}

			Controller *Clone(void) const
			{
				return (Replicate());
			}

			C4API static Controller *New(ControllerType type);

			C4API static bool ValidNode(const Node *node);
			static void RegisterStandardControllers(void);

			C4API void PackType(Packer& data) const override;
			C4API void Pack(Packer& data, unsigned_int32 packFlags) const override;
			C4API void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			C4API virtual void Preprocess(void);
			C4API virtual void Neutralize(void);

			C4API virtual bool InstanceExtractable(void) const;

			C4API virtual ControllerMessage *CreateMessage(ControllerMessageType type) const;
			C4API virtual void ReceiveMessage(const ControllerMessage *message);
			C4API virtual void SendInitialStateMessages(Player *player) const;

			C4API virtual void Wake(void);
			C4API virtual void Sleep(void);

			C4API virtual void Move(void);
			C4API virtual void StopMotion(void);
			C4API virtual void Update(void);

			C4API virtual void SetDetailLevel(int32 level);

			C4API virtual void Activate(Node *initiator, Node *trigger);
			C4API virtual void Deactivate(Node *initiator, Node *trigger);

			C4API virtual void HandleInteractionEvent(InteractionEventType type, const Point3D *position, Node *initiator = nullptr);

			C4API virtual const void *GetControllerEventData(EventType event) const;
	};


	class UnknownController : public Controller
	{
		friend class Controller;
		friend class ControllerReg<UnknownController>;

		private:

			ControllerType		unknownType;
			unsigned_int32		unknownSize;
			char				*unknownData;

			UnknownController();
			UnknownController(ControllerType type);
			UnknownController(const UnknownController& unknownController);

			Controller *Replicate(void) const override;

		public:

			~UnknownController();

			static bool ValidNode(const Node *node);

			void PackType(Packer& data) const override;
			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;
	};


	class SettingMessage : public ControllerMessage
	{
		friend class Controller;

		private:

			Type		settingCategory;
			Setting		*messageSetting;

			SettingMessage(int32 controllerIndex);

		public:

			C4API SettingMessage(int32 controllerIndex, Type category, const Setting *setting);
			C4API ~SettingMessage();

			Type GetSettingCategory(void) const
			{
				return (settingCategory);
			}

			const Setting *GetSetting(void) const
			{
				return (messageSetting);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			void HandleControllerMessage(Controller *controller) const;
			bool OverridesMessage(const ControllerMessage *message) const;
	};


	class FunctionMessage : public ControllerMessage
	{
		friend class Controller;

		private:

			Function	*messageFunction;

			FunctionMessage(int32 controllerIndex);

		public:

			C4API FunctionMessage(int32 controllerIndex, const Function *function);
			C4API ~FunctionMessage();

			Function *GetFunction(void) const
			{
				return (messageFunction);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			void HandleControllerMessage(Controller *controller) const;
			bool OverridesMessage(const ControllerMessage *message) const;
	};


	class WakeSleepMessage : public ControllerMessage
	{
		public:

			C4API WakeSleepMessage(ControllerMessageType type, int32 controllerIndex);
			C4API ~WakeSleepMessage();

			void HandleControllerMessage(Controller *controller) const;
			bool OverridesMessage(const ControllerMessage *message) const;
	};


	class NodeEnableDisableMessage : public ControllerMessage
	{
		public:

			C4API NodeEnableDisableMessage(ControllerMessageType type, int32 controllerIndex);
			C4API ~NodeEnableDisableMessage();

			void HandleControllerMessage(Controller *controller) const;
			bool OverridesMessage(const ControllerMessage *message) const;
	};


	class DeleteNodeMessage : public ControllerMessage
	{
		public:

			C4API DeleteNodeMessage(int32 controllerIndex);
			C4API ~DeleteNodeMessage();

			void HandleControllerMessage(Controller *controller) const;
	};


	class NodeInteractivityMessage : public ControllerMessage
	{
		public:

			C4API NodeInteractivityMessage(ControllerMessageType type, int32 controllerIndex);
			C4API ~NodeInteractivityMessage();

			void HandleControllerMessage(Controller *controller) const;
			bool OverridesMessage(const ControllerMessage *message) const;
	};


	class GeometryVisibilityMessage : public ControllerMessage
	{
		public:

			C4API GeometryVisibilityMessage(ControllerMessageType type, int32 controllerIndex);
			C4API ~GeometryVisibilityMessage();

			void HandleControllerMessage(Controller *controller) const;
			bool OverridesMessage(const ControllerMessage *message) const;
	};


	class BreakJointMessage : public ControllerMessage
	{
		public:

			C4API BreakJointMessage(int32 controllerIndex);
			C4API ~BreakJointMessage();

			void HandleControllerMessage(Controller *controller) const;
			bool OverridesMessage(const ControllerMessage *message) const;
	};


	class SourcePlayStopMessage : public ControllerMessage
	{
		public:

			C4API SourcePlayStopMessage(ControllerMessageType type, int32 controllerIndex);
			C4API ~SourcePlayStopMessage();

			void HandleControllerMessage(Controller *controller) const;
			bool OverridesMessage(const ControllerMessage *message) const;
	};


	class MaterialColorMessage : public ControllerMessage
	{
		friend class Controller;

		private:

			AttributeType	attributeType;
			ColorRGBA		materialColor;

			MaterialColorMessage(int32 controllerIndex);

		public:

			C4API MaterialColorMessage(int32 controllerIndex, AttributeType type, const ColorRGBA& color);
			C4API ~MaterialColorMessage();

			AttributeType GetAttributeType(void) const
			{
				return (attributeType);
			}

			const ColorRGBA& GetMaterialColor(void) const
			{
				return (materialColor);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			void HandleControllerMessage(Controller *controller) const;
			bool OverridesMessage(const ControllerMessage *message) const;
	};


	class ShaderParameterMessage : public ControllerMessage
	{
		friend class Controller;

		private:

			int32			parameterSlot;
			Vector4D		parameterValue;

			ShaderParameterMessage(int32 controllerIndex);

		public:

			C4API ShaderParameterMessage(int32 controllerIndex, int32 slot, const Vector4D& param);
			C4API ~ShaderParameterMessage();

			int32 GetParameterSlot(void) const
			{
				return (parameterSlot);
			}

			const Vector4D& GetParameterValue(void) const
			{
				return (parameterValue);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			void HandleControllerMessage(Controller *controller) const;
			bool OverridesMessage(const ControllerMessage *message) const;
	};


	//# \class	DistributionController		Distributes messages to controllers belonging to subnodes or connected nodes.
	//
	//# The $DistributionController$ class acts as a special intermediary that distributes messages to controllers belonging to subnodes or connected nodes.
	//
	//# \def	class DistributionController : public Controller
	//
	//# \ctor	DistributionController(unsigned_int32 flags);
	//
	//# \param	flags		The distribution flags. See below for possible values.
	//
	//# \desc
	//# The $DistributionController$ class can be assigned to a node to make it act as an intermediary through which
	//# controller messages are passed to subnodes, connected nodes, or both. Messages can optionally be received and
	//# processed by the distribution controller itself. The $flags$ parameter determines which sets of nodes receive
	//# messages that are initially sent to the distribution controller, and its value can be a combination (through
	//# logical OR) of the following constants.
	//
	//# \table	DistributionFlags
	//
	//# \base	Controller/Controller		A $DistributionController$ is a specific type of controller.


	class DistributionController : public Controller
	{
		friend class ControllerReg<DistributionController>;

		private:

			unsigned_int32		distributionFlags;

			DistributionController();
			DistributionController(const DistributionController& distributionController);

			Controller *Replicate(void) const override;

		public:

			DistributionController(unsigned_int32 flags);
			~DistributionController();

			unsigned_int32 GetDistributionFlags(void) const
			{
				return (distributionFlags);
			}

			void SetDistributionFlags(unsigned_int32 flags)
			{
				distributionFlags = flags;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void ReceiveMessage(const ControllerMessage *message) override;
	};


	//# \class	ShaderParameterController		Manages a changing shader parameter driven by another controller.
	//
	//# The $ShaderParameterController$ class manages a changing shader parameter driven by another controller.
	//
	//# \def	class ShaderParameterController final : public Controller, public Observer<ShaderParameterController, Controller>
	//
	//# \ctor	ShaderParameterController(int32 slot, const char *connectorKey);
	//
	//# \param	slot			The shader parameter slot. This must be in the range [0,&nbsp;7].
	//# \param	connectorKey	The connector key belonging to the target node through which another controller is observed.
	//
	//# \desc
	//# The $ShaderParameterController$ class is assigned to a $@WorldMgr/Geometry@$ node or $@EffectMgr/Effect@$
	//# node to modify one of the shader parameters used by the node's material. The index of the shader parameter slot
	//# is specified by the $slot$ parameter. When the shader parameter is modified, it affects only the target node
	//# and not all nodes using the same material as the target node.
	//#
	//# The $connectorKey$ parameter specifies a connector belonging to the shader parameter controller's target node
	//# that is connected to another node having a controller that generates a color output. The shader parameter
	//# controller observes the connected node's controller and responds to $kEventControllerColorUpdate$ events by
	//# updating the shader parameter to the color value output by the other controller. The $@StrobeController@$,
	//# $@FlickerController@$, and $@FlashController@$ classes generate such outputs and are capable of driving a
	//# $ShaderParameterController$.
	//
	//# \base	Controller/Controller										A $ShaderParameterController$ is a specific type of controller.
	//# \base	Utilities/Observer<ShaderParameterController, Controller>	A $ShaderParameterController$ observes another controller for color update events.
	//
	//# \also	$@StrobeController@$
	//# \also	$@FlickerController@$
	//# \also	$@FlashController@$


	class ShaderParameterController final : public Controller, public Observer<ShaderParameterController, Controller>
	{
		friend class ControllerReg<ShaderParameterController>;

		private:

			Vector4D		parameterValue;

			int32			parameterSlot;
			ConnectorKey	driverConnectorKey;

			ShaderParameterController();
			ShaderParameterController(const ShaderParameterController& shaderParameterController);

			Controller *Replicate(void) const override;

			void HandleControllerEvent(Controller *controller, ObservableEventType event);

			static const float *GetShaderParameterData(int32 slot, void *cookie);

		public:

			ShaderParameterController(int32 slot, const char *connectorKey);
			~ShaderParameterController();

			int32 GetParameterSlot(void) const
			{
				return (parameterSlot);
			}

			void SetParameterSlot(int32 slot)
			{
				parameterSlot = slot;
			}

			const ConnectorKey& GetDriverConnectorKey(void) const
			{
				return (driverConnectorKey);
			}

			void SetDriverConnectorKey(const ConnectorKey& key)
			{
				driverConnectorKey = key;
			}

			static bool ValidNode(const Node *node);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Preprocess(void) override;
	};


	//# \class	StrobeController		Manages a strobing effect for a light node.
	//
	//# The $StrobeController$ class manages a strobing effect for a light node.
	//
	//# \def	class StrobeController final : public Controller
	//
	//# \ctor	StrobeController(const ColorRGB& color1, const ColorRGB& color2, const Range<float>& holdTime1, const Range<float>& holdTime2, const Range<float>& transitionTime);
	//
	//# \param	color1				The primary color of the strobe effect.
	//# \param	color2				The secondary color of the strobe effect.
	//# \param	holdTime1			The range of times, in milliseconds, that the primary color is active.
	//# \param	holdTime2			The range of times, in milliseconds, that the secondary color is active.
	//# \param	transitionTime		The range of times, in milliseconds, that it takes to transition between the two colors.
	//
	//# \desc
	//# The $StrobeController$ class is assigned to a $@WorldMgr/Light@$ node to make it continuously alternate
	//# between two different colors and hold them for random intervals. The two light colors are specified by the
	//# $color1$ and $color2$ parameters. When either of these colors is set for the target light by the controller,
	//# a random time interval is chosen from the ranges specified by the $holdTime1$ and $holdTime2$ parameters,
	//# respectively, before switching back to the other color. This creates a randomized strobing effect.
	//#
	//# The $transitionTime$ parameter specifies the range of times that it takes to transition from one color to
	//# the other. Each time the color is changed, a random time from this interval is chosen, and the target light's
	//# color is smoothly changed over that time interval. The $transitionTime$ parameter may specify the range
	//# [0,&nbsp;0], in which case the transition is always instantaneous.
	//#
	//# A strobe controller is capable of driving a $@ShaderParameterController@$ attached to another node. The alpha
	//# channel of the strobe controller's output corresponds to the blend of colors currently assigned to the target
	//# light node. When the primary color is set, the alpha value is 0.0. When the secondary color is set, the alpha
	//# value is 1.0. During a transition between the primary and secondary colors, the alpha value represents the
	//# interpolation parameter between the two colors.
	//
	//# \base	Controller/Controller		A $StrobeController$ is a specific type of controller.
	//
	//# \also	$@FlickerController@$
	//# \also	$@FlashController@$
	//# \also	$@ShaderParameterController@$


	class StrobeController final : public Controller
	{
		friend class ControllerReg<StrobeController>;

		private:

			enum
			{
				kStrobeStatic,
				kStrobeHold1,
				kStrobeHold2,
				kStrobeTransition1,
				kStrobeTransition2
			};

			unsigned_int32		strobeState;

			ColorRGBA			colorValue;
			float				colorHoldTime;
			float				transitionAngle;
			float				transitionSpeed;

			ColorRGBA			strobeColor[2];
			Range<float>		strobeHoldTime[2];
			Range<float>		strobeTransitionTime;

			StrobeController();
			StrobeController(const StrobeController& strobeController);

			Controller *Replicate(void) const override;

		public:

			C4API StrobeController(const ColorRGB& color1, const ColorRGB& color2, const Range<float>& holdTime1, const Range<float>& holdTime2, const Range<float>& transitionTime);
			C4API ~StrobeController();

			const ColorRGBA& GetStrobeColor(int32 index) const
			{
				return (strobeColor[index]);
			}

			void SetStrobeColor(int32 index, const ColorRGBA& color)
			{
				strobeColor[index] = color;
			}

			const Range<float>& GetStrobeHoldTime(int32 index) const
			{
				return (strobeHoldTime[index]);
			}

			void SetStrobeHoldTime(int32 index, const Range<float>& time)
			{
				strobeHoldTime[index] = time;
			}

			const Range<float>& GetStrobeTransitionTime(void) const
			{
				return (strobeTransitionTime);
			}

			void SetStrobeTransitionTime(const Range<float>& time)
			{
				strobeTransitionTime = time;
			}

			static bool ValidNode(const Node *node);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Preprocess(void) override;
			void Sleep(void) override;
			void Move(void) override;
			void Update(void) override;

			const void *GetControllerEventData(EventType event) const override;
	};


	//# \class	FlickerController		Manages a flickering effect for a light node.
	//
	//# The $FlickerController$ class manages a flickering effect for a light node.
	//
	//# \def	class FlickerController final : public Controller
	//
	//# \ctor	FlickerController(float intensity, float amplitude);
	//
	//# \param	intensity		The intensity of the flickering effect. This should be in the range (0.0,&nbsp;1.0].
	//# \param	amplitude		The maximum displacement distance for motion of the light node.
	//
	//# \desc
	//# The $FlickerController$ class is assigned to a $@WorldMgr/Light@$ node to make it continuously change
	//# intensity and randomly move within a small volume to simulate a flickering light effect that could be
	//# produced by a flame. The $intensity$ parameter specifies the intensity of the changes in the brightness
	//# of the light, and the $amplitude$ parameter specifies the maximum distance that the light's position
	//# is moved from its original position.
	//#
	//# A flicker controller is capable of driving a $@ShaderParameterController@$ attached to another node.
	//# The alpha channel of the flicker controller's output contains the current intensity of the flicker effect.
	//
	//# \base	Controller/Controller		A $FlickerController$ is a specific type of controller.
	//
	//# \also	$@StrobeController@$
	//# \also	$@FlashController@$
	//# \also	$@ShaderParameterController@$


	class FlickerController final : public Controller
	{
		friend class ControllerReg<FlickerController>;

		private:

			unsigned_int32		flickerFlags;

			float				maxFlickerIntensity;
			float				maxMotionAmplitude;

			ColorRGBA			colorValue;

			Point3D				originalPosition;
			ColorRGB			originalColor;

			Interpolator		flickerInterpolator;
			float				flickerIntensity;

			Interpolator		motionInterpolator;
			Vector3D			motionDirection;
			float				motionAmplitude;

			FlickerController();
			FlickerController(const FlickerController& flickerController);

			Controller *Replicate(void) const override;

		public:

			C4API FlickerController(float intensity, float amplitude);
			C4API ~FlickerController();

			float GetMaxFlickerIntensity(void) const
			{
				return (maxFlickerIntensity);
			}

			void SetMaxFlickerIntensity(float intensity)
			{
				maxFlickerIntensity = intensity;
			}

			float GetMaxMotionAmplitude(void) const
			{
				return (maxMotionAmplitude);
			}

			void SetMaxMotionAmplitude(float amplitude)
			{
				maxMotionAmplitude = amplitude;
			}

			static bool ValidNode(const Node *node);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Preprocess(void) override;
			void Neutralize(void) override;
			void Move(void) override;

			const void *GetControllerEventData(EventType event) const override;
	};


	//# \class	FlashController		Manages a light that flashes momentarily.
	//
	//# The $FlashController$ class manages a light that flashes momentarily.
	//
	//# \def	class FlashController final : public Controller
	//
	//# \ctor	FlashController(const ColorRGB& color, float start, int32 duration);
	//
	//# \param	color		The brightest color that the light will attain.
	//# \param	start		The starting intensity of the light (in the range 0.0 to 1.0).
	//# \param	duration	The duration of the flash, in milliseconds.
	//
	//# \desc
	//# The $FlashController$ class is assigned to a $@WorldMgr/Light@$ node to make it flash brightly and
	//# then return to a darkened state. The color specified by the $color$ parameter represents the
	//# brightest color that will be attained by the light. The value of the $start$ parameter represents
	//# the fraction of this brightest color at which the flash effect starts. The flash effect increases
	//# in intensity from the starting value to 100% brightness and then decreases to an intensity of zero.
	//#
	//# A flash controller created with the above constructor is initially in the awake state and starts
	//# running immediately. A flash controller created this way is also in the nonpersistent state, meaning
	//# that it deletes its target node when the intensity reaches zero. The controller can be made persistent
	//# by calling the $@FlashController::SetFlashFlags@$ function.
	//#
	//# A flash controller created in the World Editor is initially in the sleeping state, and it is persistent.
	//# The flash effect starts running when the controller is awakened.
	//#
	//# A flash controller is capable of driving a $@ShaderParameterController@$ attached to another node.
	//# The alpha channel of the flash controller's output contains the current brightness factor of the flash effect.
	//
	//# \base	Controller/Controller		A $FlashController$ is a specific type of controller.
	//
	//# \also	$@StrobeController@$
	//# \also	$@FlickerController@$
	//# \also	$@ShaderParameterController@$


	//# \function	FlashController::GetFlashFlags		Returns the flash flags.
	//
	//# \proto	unsigned_int32 GetFlashFlags(void) const;
	//
	//# \desc
	//# The $GetFlashFlags$ function returns the flash flags, which can be a combination (through logical OR)
	//# of the following constants.
	//#
	//# \table	FlashFlags
	//
	//# \also	$@FlashController::SetFlashFlags@$


	//# \function	FlashController::SetFlashFlags		Sets the flash flags.
	//
	//# \proto	void SetFlashFlags(unsigned_int32 flags);
	//
	//# \param	flags	The new flash flags.
	//
	//# \desc
	//# The $SetFlashFlags$ function sets the flash flags to the value specified by the $flags$ parameter.
	//# This can be a combination (through logical OR) of the following constants.
	//#
	//# \table	FlashFlags
	//
	//# \also	$@FlashController::GetFlashFlags@$


	class FlashController final : public Controller
	{
		friend class ControllerReg<FlashController>;

		private:

			unsigned_int32		flashFlags;
			ColorRGB			flashColor;
			float				flashStart;
			int32				flashDuration;

			Interpolator		flashInterpolator;
			ColorRGBA			colorValue;

			FlashController();
			FlashController(const FlashController& flashController);

			Controller *Replicate(void) const override;

			void Initialize(void);

		public:

			C4API FlashController(const ColorRGB& color, float start, int32 duration);
			C4API ~FlashController();

			unsigned_int32 GetFlashFlags(void) const
			{
				return (flashFlags);
			}

			void SetFlashFlags(unsigned_int32 flags)
			{
				flashFlags = flags;
			}

			static bool ValidNode(const Node *node);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Preprocess(void) override;
			void Wake(void) override;
			void Sleep(void) override;
			void Move(void) override;
	};
}


#endif

// ZYUQURM
