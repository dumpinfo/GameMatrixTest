 

#ifndef C4Methods_h
#define C4Methods_h


//# \component	Controller System
//# \prefix		Controller/


#include "C4Node.h"
#include "C4Values.h"
#include "C4Controller.h"
#include "C4Modifiers.h"
#include "C4Configuration.h"


namespace C4
{
	typedef Type	MethodType;
	typedef Type	MethodGroup;


	enum : MethodType
	{
		kMethodNull							= 0,
		kMethodUnknown						= '!UNK',
		kMethodSection						= 'SECT',
		kMethodExpression					= 'EXPR',
		kMethodFunction						= 'FUNC',
		kMethodSetting						= 'SETT',
		kMethodActivate						= 'ACVT',
		kMethodDeactivate					= 'DACT',
		kMethodTerminate					= 'TERM',
		kMethodGetVariable					= 'GVAR',
		kMethodSetVariable					= 'SVAR',
		kMethodSetColorValue				= 'SCOL',
		kMethodSetVectorValue				= 'SVEC',
		kMethodEvent						= 'EVNT',
		kMethodTime							= 'TIME',
		kMethodDelay						= 'DLAY',

		kMethodRandomInteger				= 'RNDI',
		kMethodRandomFloat					= 'RNDF',
		kMethodGetStringLength				= 'SLEN',
		kMethodWakeController				= 'WAKE',
		kMethodSleepController				= 'SLEP',
		kMethodGetWakeState					= 'GWAK',
		kMethodEnableNode					= 'ENAB',
		kMethodDisableNode					= 'DSAB',
		kMethodGetEnableState				= 'GENA',
		kMethodDeleteNode					= 'DELT',
		kMethodEnableInteractivity			= 'EINT',
		kMethodDisableInteractivity			= 'DINT',
		kMethodGetInteractivityState		= 'GINT',
		kMethodShowGeometry					= 'SGEO',
		kMethodHideGeometry					= 'HGEO',
		kMethodGetVisibilityState			= 'GVIS',
		kMethodActivateTrigger				= 'ATRG',
		kMethodDeactivateTrigger			= 'DTRG',
		kMethodBreakJoint					= 'BJNT',
		kMethodRemoveModifiers				= 'RMOD',
		kMethodGetNodePosition				= 'NPOS',
		kMethodGetWorldPosition				= 'WPOS',
		kMethodPlaySound					= 'SOND',
		kMethodPlaySource					= 'PSRC',
		kMethodStopSource					= 'SSRC',
		kMethodVarySourceVolume				= 'FSRC',
		kMethodVarySourceFrequency			= 'QSRC',
		kMethodSetMaterialColor				= 'MATL',
		kMethodSetShaderScalarParameter		= 'SPRM',
		kMethodSetShaderVectorParameter		= 'VPRM',
		kMethodSetShaderColorParameter		= 'CPRM'
	};


	C4API extern const char kConnectorKeyController[];
	C4API extern const char kConnectorKeyInitiator[];
	C4API extern const char kConnectorKeyTrigger[];


	//# \enum	MethodFlags

	enum
	{
		kMethodNoTarget				= 1 << 0,		//## The method does not operate on a target node. This flag prevents a target node from being assigned in the Script Editor.
		kMethodNoSelfTarget			= 1 << 1,		//## The method cannot operate on the node to which its script controller is attached. This flag prevents the controller target from being selected as the target node in the Script Editor.
		kMethodNoMessage			= 1 << 2,		//## The method does not generate any controller messages. This flag prevents a generic controller from being assigned to the target node in the Script Editor.
		kMethodOutputValue			= 1 << 3		//## The method generates an output value that can be stored in a script variable. This flag allows an output variable to be assigned in the Script Editor.
	};


	enum
	{
		kMethodResult				= 1 << 0,
		kMethodDead					= 1 << 1
	};


	enum
	{
		kFiberReady					= 1 << 0,
		kFiberDead					= 1 << 1 
	};

 
	enum
	{ 
		kFiberLooping				= 1 << 0,
		kFiberConditionTrue			= 1 << 1,
		kFiberConditionFalse		= 1 << 2 
	};
 
 
	class Fiber;
	class Method;
	class ScriptState;
	class Source; 


	//# \class	MethodRegistration		Manages internal registration information for a custom script method type.
	//
	//# The $MethodRegistration$ class manages internal registration information for a custom script method type.
	//
	//# \def	class MethodRegistration : public Registration<Method, MethodRegistration>
	//
	//# \ctor	MethodRegistration(MethodType type, const char *name, unsigned_int32 flags = 0, MethodGroup group = 0);
	//
	//# \param	type		The method type.
	//# \param	name		The method name.
	//# \param	flags		The method flags. See below for possible values.
	//# \param	group		The group to which the method belongs. This should be 0 for all custom methods.
	//
	//# \desc
	//# The $MethodRegistration$ class is abstract and serves as the common base class for the template class
	//# $@MethodReg@$. A custom script method is registered with the engine by instantiating an object of type
	//# $MethodReg<classType>$, where $classType$ is the type of the method subclass being registered.
	//#
	//# The $flags$ parameter can be a combination (through logical OR) of the following constants.
	//
	//# \table	MethodFlags
	//
	//# \base	System/Registration<Method, MethodRegistration>		A method registration is a specific type of registration object.
	//
	//# \also	$@MethodReg@$
	//# \also	$@Method@$


	//# \function	MethodRegistration::GetMethodType		Returns the registered method type.
	//
	//# \proto	MethodType GetMethodType(void) const;
	//
	//# \desc
	//# The $GetMethodType$ function returns the method type for a particular method registration.
	//# The method type is established when the method registration is constructed.
	//
	//# \also	$@MethodRegistration::GetMethodFlags@$
	//# \also	$@MethodRegistration::GetMethodName@$


	//# \function	MethodRegistration::GetMethodFlags		Returns the method flags.
	//
	//# \proto	unsigned_int32 GetMethodFlags(void) const;
	//
	//# \desc
	//# The $GetMethodFlags$ function returns the method flags for a particular method registration.
	//# The method flags are established when the method registration is constructed and can be a
	//# combination (through logical OR) of the following constants.
	//
	//# \table	MethodFlags
	//
	//# \also	$@MethodRegistration::GetMethodType@$
	//# \also	$@MethodRegistration::GetMethodName@$


	//# \function	MethodRegistration::GetMethodName		Returns the human-readable method name.
	//
	//# \proto	const char *GetMethodName(int32 index) const;
	//
	//# \param	index	The part of the name to return. This must be 0 or 1.
	//
	//# \desc
	//# The $GetMethodName$ function returns the human-readable method name for a particular method registration.
	//# The method name is established when the method registration is constructed. Method names can have two
	//# parts, and the $index$ parameter specifies which part to return. If a method name only has one part and
	//# $index$ is 1, then $nullptr$ is returned.
	//
	//# \also	$@MethodRegistration::GetMethodType@$
	//# \also	$@MethodRegistration::GetMethodFlags@$


	class MethodRegistration : public Registration<Method, MethodRegistration>
	{
		private:

			unsigned_int32		methodFlags;
			const char			*methodName;
			MethodGroup			methodGroup;

		protected:

			C4API MethodRegistration(MethodType type, const char *name, unsigned_int32 flags = 0, MethodGroup group = 0);

		public:

			C4API ~MethodRegistration();

			MethodType GetMethodType(void) const
			{
				return (GetRegistrableType());
			}

			unsigned_int32 GetMethodFlags(void) const
			{
				return (methodFlags);
			}

			const char *GetMethodName(void) const
			{
				return (methodName);
			}

			MethodGroup GetMethodGroup(void) const
			{
				return (methodGroup);
			}
	};


	//# \class	MethodReg	 Represents a custom script method type.
	//
	//# The $MethodReg$ class represents a custom script method type.
	//
	//# \def	template <class classType> class MethodReg : public MethodRegistration
	//
	//# \tparam	classType	The custom method class.
	//
	//# \ctor	MethodReg(MethodType type, const char *name, unsigned_int32 flags = 0, MethodGroup group = 0);
	//
	//# \param	type		The method type.
	//# \param	name		The method name.
	//# \param	flags		The method flags. See below for possible values.
	//# \param	group		The group to which the method belongs. This should be 0 for all custom methods.
	//
	//# \desc
	//# The $MethodReg$ template class is used to advertise the existence of a custom script method type.
	//# The World Manager uses a method registration to construct a custom method, and the Script Editor
	//# displays a list of registered methods. The act of instantiating a $MethodReg$ object automatically
	//# registers the corresponding method type. The method type is unregistered when the $MethodReg$ object is destroyed.
	//#
	//# The $flags$ parameter specifies special properties of the method type and can be a combination (through
	//# logical OR) of the following constants.
	//
	//# \table	MethodFlags
	//
	//# No more than one method registration should be created for each distinct method type.
	//
	//# \base	MethodRegistration		All specific method registration classes share the common base class $MethodRegistration$.
	//
	//# \also	$@Method@$


	template <class classType> class MethodReg : public MethodRegistration
	{
		public:

			MethodReg(MethodType type, const char *name, unsigned_int32 flags = 0, MethodGroup group = 0) : MethodRegistration(type, name, flags, group)
			{
			}

			Method *Create(void) const
			{
				return (new classType);
			}
	};


	class Fiber : public GraphEdge<Method, Fiber>, public ListElement<Fiber>, public Packable, public EngineMemory<ScriptState>
	{
		private:

			unsigned_int32		fiberFlags;
			unsigned_int32		fiberState;

		public:

			C4API Fiber(Method *start, Method *finish);
			C4API Fiber(const Fiber& fiber, Method *start, Method *finish);
			C4API ~Fiber();

			unsigned_int32 GetFiberFlags(void) const
			{
				return (fiberFlags);
			}

			void SetFiberFlags(unsigned_int32 flags)
			{
				fiberFlags = flags;
			}

			unsigned_int32 GetFiberState(void) const
			{
				return (fiberState);
			}

			void SetFiberState(unsigned_int32 state)
			{
				fiberState = state;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
	};


	//# \class	Method		Represents an action that can be placed in a script.
	//
	//# The $Method$ class represents an action that can be placed in a script.
	//
	//# \def	class Method : public GraphElement<Method, Fiber>, public Completable<Method>, public Registrable<Method, MethodRegistration>,
	//# \def2	public Packable, public Configurable, public Memory<ScriptState>
	//
	//# \ctor	Method(MethodType type = kMethodNull);
	//
	//# \param	type		The method type.
	//
	//# \desc
	//# The $Method$ class is the base class for all classes representing an action that can be placed in a script.
	//
	//# \base	Utilities/GraphElement<Method, Fiber>			Methods are stored in a directed graph to make up a script.
	//# \base	Utilities/Completable<Method>					A method calls its completion procedure when it finishes executing.
	//# \base	System/Registrable<Method, MethodRegistration>	Custom method types can be registered with the engine.
	//# \base	ResourceMgr/Packable							Methods can be packed for storage in resources.
	//# \base	InterfaceMgr/Configurable						Methods can define configurable parameters that are exposed
	//#															as user interface widgets in the Script Editor.
	//# \base	MemoryMgr/Memory<ScriptState>					Script methods are stored in a dedicated heap.
	//
	//# \also	$@MethodReg@$
	//
	//# \wiki	Methods


	//# \function	Method::GetMethodType		Returns the method type.
	//
	//# \proto	MethodType GetMethodType(void) const;
	//
	//# \desc
	//# The $GetMethodType$ function returns the method type.
	//
	//# \also	$@MethodReg@$


	//# \function	Method::GetTargetNode		Returns the target node for a method.
	//
	//# \proto	Node *GetTargetNode(const ScriptState *state) const;
	//
	//# \param	state	The current script state.
	//
	//# \desc
	//# The $GetTargetNode$ function can be called by a method class to retrieve a pointer to the target
	//# node for the method. This function is normally called from within an override of the $@Method::Execute@$
	//# function. The return value can be $nullptr$, and in such a case, the method should generally skip any
	//# actions it would have performed on the target node.
	//
	//# \also	$@ScriptState@$


	//# \function	Method::SetMethodResult		Sets the boolean result for a method.
	//
	//# \proto	void SetMethodResult(bool result);
	//
	//# \param	result		The boolean result.
	//
	//# \desc
	//# The $SetMethodResult$ function sets the boolean result for a method. This function should be
	//# called from within the overridden $@Method::Execute@$ function for script methods that need
	//# to specify a boolean result. If the $SetMethodResult$ function is not called, then the boolean
	//# result for a method is true by default.
	//#
	//# If the $@Method::SetOutputValue@$ function is called, then the boolean result is automatically
	//# set by converting the output value to a boolean value. To specify a boolean result independently,
	//# call the $SetMethodResult$ function after calling the $SetOutputValue$ function.
	//
	//# \also	$@Method::SetOutputValue@$
	//# \also	$@Method::Execute@$


	//# \function	Method::SetOutputValue		Sets the output value for a method.
	//
	//# \proto	void SetOutputValue(const ScriptState *state, bool v);
	//# \proto	void SetOutputValue(const ScriptState *state, int32 v);
	//# \proto	void SetOutputValue(const ScriptState *state, float v);
	//# \proto	void SetOutputValue(const ScriptState *state, const char *v);
	//# \proto	void SetOutputValue(const ScriptState *state, const ColorRGBA& v);
	//# \proto	void SetOutputValue(const ScriptState *state, const Vector3D& v);
	//
	//# \param	state		A pointer to the current script state.
	//# \param	v			The output value for the method.
	//
	//# \desc
	//# The $SetOutputValue$ function sets the output value for a method. This function should be
	//# called from within the overridden $@Method::Execute@$ function for script methods that need
	//# to specify an output value. The output value is stored in the method's output variable (after
	//# a possible type conversion) if the output variable has been specified and exists in the script.
	//# Otherwise, the output value is simply discarded. In any case, the method's boolean result is
	//# automatically set by converting the output value to a boolean value.
	//#
	//# The $state$ parameter should be set to the same value passed into the $state$ parameter of
	//# the $@Method::Execute@$ function.
	//
	//# \also	$@Method::SetMethodResult@$
	//# \also	$@Method::Execute@$


	//# \function	Method::Execute		Executes a script method.
	//
	//# \proto	virtual void Execute(const ScriptState *state);
	//
	//# \param	state	The current script state.
	//
	//# \desc
	//# The $Execute$ function is called by the script controller when it is time to execute a script
	//# method. The override of this function should perform whatever actions are necessary to execute
	//# the method and then call the $@Utilities/Completable::CallCompletionProc@$ function to indicate
	//# that the method has finished executing.
	//#
	//# A method subclass may call the $@Method::SetMethodResult@$ function or the $@Method::SetOutputValue@$
	//# function to specify the output value of the method.
	//
	//# \also	$@Method::Resume@$
	//# \also	$@Method::Stop@$
	//# \also	$@Method::SetMethodResult@$
	//# \also	$@Method::SetOutputValue@$
	//# \also	$@ScriptState@$


	//# \function	Method::Resume		Resumes a script method after a saved game is loaded.
	//
	//# \proto	virtual void Resume(const ScriptState *state);
	//
	//# \param	state	The current script state.
	//
	//# \desc
	//# The $Resume$ function is called by the script controller after a saved game is loaded if the method
	//# was executing at the time that the game was saved. The override of this function should perform
	//# whatever actions are necessary to resume the execution of the method from the state that it was in
	//# when the method was previously saved.
	//#
	//# It is only necessary to implement the $Resume$ function for methods that take time to execute.
	//# If a method completes execution within the $@Method::Execute@$ function, then it can never be in
	//# the executing state when a game is saved.
	//
	//# \also	$@Method::Execute@$
	//# \also	$@ScriptState@$


	//# \function	Method::Stop		Stops execution along the path containing a method.
	//
	//# \proto	void Stop(void);
	//
	//# \desc
	//# The $Stop$ function can be called from within a method's $@Method::Execute@$ function to indicate
	//# that execution should be terminated along the path in the script graph containing the method.
	//# This effectively prevents any successors of the method from being executed unless they can be
	//# reached through another live path.
	//
	//# \also	$@Method::Execute@$


	class Method : public GraphElement<Method, Fiber>, public Completable<Method>, public Registrable<Method, MethodRegistration>, public Packable, public Configurable, public EngineMemory<ScriptState>
	{
		friend class MethodRegistration;
		friend class ScriptGraph;
		friend class ScriptObject;
		friend class ScriptState;

		private:

			MethodType				methodType;
			unsigned_int32			methodState;
			Reference<Method>		methodReference;

			ConnectorKey			targetKey;

			ValueName				outputValueName;
			List<Setting>			inputValueList;

			union
			{
				mutable int32		methodIndex;
				mutable int32		superIndex;
				mutable Method		*cloneMethod;
			};

			Point2D					methodPosition;

			C4API virtual Method *Replicate(void) const;

			static Method *CreateMethod(Unpacker& data, unsigned_int32 unpackFlags);
			static Method *CreateUnknown(Unpacker& data, unsigned_int32 unpackFlags);

		protected:

			C4API Method(const Method& method);

			C4API virtual void ProcessInputValues(const ScriptState *state);

			C4API Node *GetTargetNode(const ScriptState *state) const;

		public:

			C4API Method(MethodType type = kMethodNull);
			C4API ~Method();

			MethodType GetMethodType(void) const
			{
				return (methodType);
			}

			unsigned_int32 GetMethodState(void) const
			{
				return (methodState);
			}

			void SetMethodState(unsigned_int32 state)
			{
				methodState = state;
			}

			void Stop(void)
			{
				methodState |= kMethodDead;
			}

			void SetMethodResult(bool result)
			{
				methodState = (methodState & ~kMethodResult) | (unsigned_int32) result;
			}

			Reference<Method> *GetMethodReference(void)
			{
				return (&methodReference);
			}

			const ConnectorKey& GetTargetKey(void) const
			{
				return (targetKey);
			}

			void SetTargetKey(const char *key)
			{
				targetKey = key;
			}

			const char *GetOutputValueName(void) const
			{
				return (outputValueName);
			}

			void SetOutputValueName(const char *name)
			{
				outputValueName = name;
			}

			Setting *GetFirstInputValue(void) const
			{
				return (inputValueList.First());
			}

			void AddInputValue(Setting *setting)
			{
				inputValueList.Append(setting);
			}

			void PurgeInputValues(void)
			{
				inputValueList.Purge();
			}

			int32 GetMethodIndex(void) const
			{
				return (methodIndex);
			}

			void SetMethodIndex(int32 index)
			{
				methodIndex = index;
			}

			Method *GetCloneMethod(void) const
			{
				return (cloneMethod);
			}

			void SetCloneMethod(Method *method)
			{
				cloneMethod = method;
			}

			const Point2D& GetMethodPosition(void) const
			{
				return (methodPosition);
			}

			void SetMethodPosition(const Point2D& position)
			{
				methodPosition = position;
			}

			Method *Clone(void) const
			{
				return (Replicate());
			}

			static void RegisterStandardMethods(void);

			C4API static Method *New(MethodType type);

			C4API void PackType(Packer& data) const override;
			C4API void Pack(Packer& data, unsigned_int32 packFlags) const override;
			C4API void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			C4API void *BeginSettingsUnpack(void) override;

			C4API void SetOutputValue(const ScriptState *state, bool v);
			C4API void SetOutputValue(const ScriptState *state, int32 v);
			C4API void SetOutputValue(const ScriptState *state, float v);
			C4API void SetOutputValue(const ScriptState *state, const char *v);
			C4API void SetOutputValue(const ScriptState *state, const ColorRGBA& v);
			C4API void SetOutputValue(const ScriptState *state, const Vector3D& v);
			C4API void SetOutputValue(const ScriptState *state, const Value *v);

			C4API virtual void Preprocess(const ScriptState *state);
			C4API virtual void Execute(const ScriptState *state);
			C4API virtual void Resume(const ScriptState *state);
	};


	class UnknownMethod : public Method
	{
		friend class Method;

		private:

			MethodType			unknownType;
			unsigned_int32		unknownSize;
			char				*unknownData;

			UnknownMethod(MethodType type);
			UnknownMethod(const UnknownMethod& unknownMethod);

			Method *Replicate(void) const override;

		public:

			~UnknownMethod();

			void PackType(Packer& data) const override;
			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;
	};


	class SectionMethod final : public Method
	{
		private:

			float		sectionWidth;
			float		sectionHeight;

			ColorRGBA	sectionColor;
			String<>	sectionComment;

			SectionMethod(const SectionMethod& sectionMethod);

			Method *Replicate(void) const override;

		public:

			C4API SectionMethod();
			C4API ~SectionMethod();

			float GetSectionWidth(void) const
			{
				return (sectionWidth);
			}

			float GetSectionHeight(void) const
			{
				return (sectionHeight);
			}

			void SetSectionSize(float width, float height)
			{
				sectionWidth = width;
				sectionHeight = height;
			}

			const ColorRGBA& GetSectionColor(void) const
			{
				return (sectionColor);
			}

			void SetSectionColor(const ColorRGBA& color)
			{
				sectionColor = color;
			}

			const char *GetSectionComment(void) const
			{
				return (sectionComment);
			}

			void SetSectionComment(const char *comment)
			{
				sectionComment = comment;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;
	};


	class FunctionMethod final : public Method
	{
		private:

			Function		*methodFunction;

			FunctionMethod(const FunctionMethod& functionMethod);

			Method *Replicate(void) const override;

			void ProcessInputValues(const ScriptState *state);

			static void FunctionComplete(Function *function, void *cookie);

		public:

			C4API FunctionMethod();
			C4API ~FunctionMethod();

			Function *GetFunction(void) const
			{
				return (methodFunction);
			}

			void SetFunction(Function *function)
			{
				methodFunction = function;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			void *BeginSettingsUnpack(void) override;

			void Preprocess(const ScriptState *state);
			void Execute(const ScriptState *state) override;
			void Resume(const ScriptState *state) override;
	};


	class SettingMethod final : public Method
	{
		private:

			Type				categoryType;
			List<Setting>		settingList;

			SettingMethod(const SettingMethod& settingMethod);

			Method *Replicate(void) const override;

		public:

			C4API SettingMethod();
			C4API ~SettingMethod();

			Type GetSettingCategoryType(void) const
			{
				return (categoryType);
			}

			void SetSettingCategoryType(Type type)
			{
				categoryType = type;
			}

			Setting *GetFirstSetting(void) const
			{
				return (settingList.First());
			}

			void AddSetting(Setting *setting)
			{
				settingList.Append(setting);
			}

			void PurgeSettings(void)
			{
				settingList.Purge();
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;

			void Execute(const ScriptState *state) override;
	};


	class ActivateMethod final : public Method
	{
		private:

			ActivateMethod(const ActivateMethod& activateMethod);

			Method *Replicate(void) const override;

		public:

			C4API ActivateMethod();
			C4API ~ActivateMethod();

			void Execute(const ScriptState *state) override;
	};


	class DeactivateMethod final : public Method
	{
		private:

			DeactivateMethod(const DeactivateMethod& deactivateMethod);

			Method *Replicate(void) const override;

		public:

			C4API DeactivateMethod();
			C4API ~DeactivateMethod();

			void Execute(const ScriptState *state) override;
	};


	class TerminateMethod final : public Method
	{
		private:

			TerminateMethod(const TerminateMethod& terminateMethod);

			Method *Replicate(void) const override;

		public:

			C4API TerminateMethod();
			C4API ~TerminateMethod();

			void Execute(const ScriptState *state) override;
	};


	class GetVariableMethod final : public Method
	{
		friend class MethodReg<GetVariableMethod>;

		private:

			ValueName		valueName;

			GetVariableMethod();
			GetVariableMethod(const GetVariableMethod& getVariableMethod);

			Method *Replicate(void) const override;

		public:

			C4API GetVariableMethod(const char *name);
			C4API ~GetVariableMethod();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(const ScriptState *state) override;
	};


	class SetVariableMethod final : public Method
	{
		friend class MethodReg<SetVariableMethod>;

		private:

			ValueName		valueName;
			ValueName		sourceName;

			SetVariableMethod();
			SetVariableMethod(const SetVariableMethod& setVariableMethod);

			Method *Replicate(void) const override;

		public:

			C4API SetVariableMethod(const char *name, const char *source);
			C4API ~SetVariableMethod();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(const ScriptState *state) override;
	};


	class SetColorValueMethod final : public Method
	{
		friend class MethodReg<SetColorValueMethod>;

		private:

			ColorRGBA		colorValue;

			SetColorValueMethod();
			SetColorValueMethod(const SetColorValueMethod& setColorValueMethod);

			Method *Replicate(void) const override;

		public:

			C4API SetColorValueMethod(const ColorRGBA& value);
			C4API ~SetColorValueMethod();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(const ScriptState *state) override;
	};


	class SetVectorValueMethod final : public Method
	{
		friend class MethodReg<SetVectorValueMethod>;

		private:

			Vector3D		vectorValue;

			SetVectorValueMethod();
			SetVectorValueMethod(const SetVectorValueMethod& setVectorValueMethod);

			Method *Replicate(void) const override;

		public:

			C4API SetVectorValueMethod(const Vector3D& value);
			C4API ~SetVectorValueMethod();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(const ScriptState *state) override;
	};


	class EventMethod final : public Method
	{
		friend class Method;

		private:

			EventType		scriptEvent;

			EventMethod();
			EventMethod(const EventMethod& eventMethod);

			Method *Replicate(void) const override;

		public:

			C4API EventMethod(EventType eventType);
			C4API ~EventMethod();

			EventType GetScriptEvent(void) const
			{
				return (scriptEvent);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			void Execute(const ScriptState *state) override;
	};


	class TimeMethod final : public Method
	{
		friend class MethodReg<TimeMethod>;

		private:

			TimeMethod(const TimeMethod& timeMethod);

			Method *Replicate(void) const override;

		public:

			C4API TimeMethod();
			C4API ~TimeMethod();

			void Execute(const ScriptState *state) override;
	};


	class DelayMethod final : public Method
	{
		friend class MethodReg<DelayMethod>;

		private:

			int32		delayTime;
			Timer		timer;

			DelayMethod();
			DelayMethod(const DelayMethod& delayMethod);

			Method *Replicate(void) const override;

			static void DelayComplete(DeferredTask *timer, void *cookie);

		public:

			C4API DelayMethod(int32 time);
			C4API ~DelayMethod();

			int32 GetDelayTime(void) const
			{
				return (delayTime);
			}

			void SetDelayTime(int32 time)
			{
				delayTime = time;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(const ScriptState *state) override;
			void Resume(const ScriptState *state) override;
	};


	class RandomIntegerMethod final : public Method
	{
		private:

			int32	minInteger;
			int32	maxInteger;

			RandomIntegerMethod(const RandomIntegerMethod& randomIntegerMethod);

			Method *Replicate(void) const override;

		public:

			C4API RandomIntegerMethod();
			C4API ~RandomIntegerMethod();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(const ScriptState *state) override;
	};


	class RandomFloatMethod final : public Method
	{
		private:

			float	minFloat;
			float	maxFloat;

			RandomFloatMethod(const RandomFloatMethod& randomFloatMethod);

			Method *Replicate(void) const override;

		public:

			C4API RandomFloatMethod();
			C4API ~RandomFloatMethod();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(const ScriptState *state) override;
	};


	class GetStringLengthMethod final : public Method
	{
		private:

			enum
			{
				kMaxMethodStringLength = 255
			};

			String<kMaxMethodStringLength>		methodString;

			GetStringLengthMethod(const GetStringLengthMethod& getStringLengthMethod);

			Method *Replicate(void) const override;

		public:

			C4API GetStringLengthMethod();
			C4API ~GetStringLengthMethod();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(const ScriptState *state) override;
	};


	class WakeControllerMethod final : public Method
	{
		private:

			WakeControllerMethod(const WakeControllerMethod& wakeControllerMethod);

			Method *Replicate(void) const override;

		public:

			C4API WakeControllerMethod();
			C4API ~WakeControllerMethod();

			void Execute(const ScriptState *state) override;
	};


	class SleepControllerMethod final : public Method
	{
		private:

			SleepControllerMethod(const SleepControllerMethod& sleepControllerMethod);

			Method *Replicate(void) const override;

		public:

			C4API SleepControllerMethod();
			C4API ~SleepControllerMethod();

			void Execute(const ScriptState *state) override;
	};


	class GetWakeStateMethod final : public Method
	{
		private:

			GetWakeStateMethod(const GetWakeStateMethod& getWakeStateMethod);

			Method *Replicate(void) const override;

		public:

			C4API GetWakeStateMethod();
			C4API ~GetWakeStateMethod();

			void Execute(const ScriptState *state) override;
	};


	class EnableNodeMethod final : public Method
	{
		private:

			EnableNodeMethod(const EnableNodeMethod& enableNodeMethod);

			Method *Replicate(void) const override;

		public:

			C4API EnableNodeMethod();
			C4API ~EnableNodeMethod();

			void Execute(const ScriptState *state) override;
	};


	class DisableNodeMethod final : public Method
	{
		private:

			DisableNodeMethod(const DisableNodeMethod& disableNodeMethod);

			Method *Replicate(void) const override;

		public:

			C4API DisableNodeMethod();
			C4API ~DisableNodeMethod();

			void Execute(const ScriptState *state) override;
	};


	class GetEnableStateMethod final : public Method
	{
		private:

			GetEnableStateMethod(const GetEnableStateMethod& getEnableStateMethod);

			Method *Replicate(void) const override;

		public:

			C4API GetEnableStateMethod();
			C4API ~GetEnableStateMethod();

			void Execute(const ScriptState *state) override;
	};


	class DeleteNodeMethod final : public Method
	{
		private:

			DeleteNodeMethod(const DeleteNodeMethod& deleteNodeMethod);

			Method *Replicate(void) const override;

		public:

			C4API DeleteNodeMethod();
			C4API ~DeleteNodeMethod();

			void Execute(const ScriptState *state) override;
	};


	class EnableInteractivityMethod final : public Method
	{
		private:

			EnableInteractivityMethod(const EnableInteractivityMethod& enableInteractivityMethod);

			Method *Replicate(void) const override;

		public:

			C4API EnableInteractivityMethod();
			C4API ~EnableInteractivityMethod();

			void Execute(const ScriptState *state) override;
	};


	class DisableInteractivityMethod final : public Method
	{
		private:

			DisableInteractivityMethod(const DisableInteractivityMethod& disableInteractivityMethod);

			Method *Replicate(void) const override;

		public:

			C4API DisableInteractivityMethod();
			C4API ~DisableInteractivityMethod();

			void Execute(const ScriptState *state) override;
	};


	class GetInteractivityStateMethod final : public Method
	{
		private:

			GetInteractivityStateMethod(const GetInteractivityStateMethod& getInteractivityStateMethod);

			Method *Replicate(void) const override;

		public:

			C4API GetInteractivityStateMethod();
			C4API ~GetInteractivityStateMethod();

			void Execute(const ScriptState *state) override;
	};


	class ShowGeometryMethod final : public Method
	{
		private:

			ShowGeometryMethod(const ShowGeometryMethod& showGeometryMethod);

			Method *Replicate(void) const override;

		public:

			C4API ShowGeometryMethod();
			C4API ~ShowGeometryMethod();

			void Execute(const ScriptState *state) override;
	};


	class HideGeometryMethod final : public Method
	{
		private:

			HideGeometryMethod(const HideGeometryMethod& hideGeometryMethod);

			Method *Replicate(void) const override;

		public:

			C4API HideGeometryMethod();
			C4API ~HideGeometryMethod();

			void Execute(const ScriptState *state) override;
	};


	class GetVisibilityStateMethod final : public Method
	{
		private:

			GetVisibilityStateMethod(const GetVisibilityStateMethod& getVisibilityStateMethod);

			Method *Replicate(void) const override;

		public:

			C4API GetVisibilityStateMethod();
			C4API ~GetVisibilityStateMethod();

			void Execute(const ScriptState *state) override;
	};


	class ActivateTriggerMethod final : public Method
	{
		private:

			ActivateTriggerMethod(const ActivateTriggerMethod& activateTriggerMethod);

			Method *Replicate(void) const override;

		public:

			C4API ActivateTriggerMethod();
			C4API ~ActivateTriggerMethod();

			void Execute(const ScriptState *state) override;
	};


	class DeactivateTriggerMethod final : public Method
	{
		private:

			DeactivateTriggerMethod(const DeactivateTriggerMethod& deactivateTriggerMethod);

			Method *Replicate(void) const override;

		public:

			C4API DeactivateTriggerMethod();
			C4API ~DeactivateTriggerMethod();

			void Execute(const ScriptState *state) override;
	};


	class BreakJointMethod final : public Method
	{
		private:

			BreakJointMethod(const BreakJointMethod& breakJointMethod);

			Method *Replicate(void) const override;

		public:

			C4API BreakJointMethod();
			C4API ~BreakJointMethod();

			void Execute(const ScriptState *state) override;
	};


	class RemoveModifiersMethod final : public Method
	{
		private:

			ModifierType	modifierType;

			RemoveModifiersMethod(const RemoveModifiersMethod& removeModifiersMethod);

			Method *Replicate(void) const override;

		public:

			C4API RemoveModifiersMethod();
			C4API ~RemoveModifiersMethod();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(const ScriptState *state) override;
	};


	class GetNodePositionMethod final : public Method
	{
		private:

			GetNodePositionMethod(const GetNodePositionMethod& getNodePositionMethod);

			Method *Replicate(void) const override;

		public:

			C4API GetNodePositionMethod();
			C4API ~GetNodePositionMethod();

			void Execute(const ScriptState *state) override;
	};


	class GetWorldPositionMethod final : public Method
	{
		private:

			GetWorldPositionMethod(const GetWorldPositionMethod& getWorldPositionMethod);

			Method *Replicate(void) const override;

		public:

			C4API GetWorldPositionMethod();
			C4API ~GetWorldPositionMethod();

			void Execute(const ScriptState *state) override;
	};


	class PlaySoundMethod final : public Method
	{
		friend class MethodReg<PlaySoundMethod>;

		private:

			ResourceName	soundName;
			bool			streamFlag;

			int32			loopCount;

			float			soundVolume;
			int32			fadeTime;

			Sound			*methodSound;

			PlaySoundMethod();
			PlaySoundMethod(const PlaySoundMethod& playSoundMethod);

			Method *Replicate(void) const override;

			static void SoundComplete(Sound *sound, void *cookie);

		public:

			C4API PlaySoundMethod(const char *name, bool stream = false);
			C4API ~PlaySoundMethod();

			const ResourceName& GetSoundName(void) const
			{
				return (soundName);
			}

			void SetSoundName(const char *name)
			{
				soundName = name;
			}

			bool GetStreamFlag(void) const
			{
				return (streamFlag);
			}

			void SetStreamFlag(bool stream)
			{
				streamFlag = stream;
			}

			int32 GetLoopCount(void) const
			{
				return (loopCount);
			}

			void SetLoopCount(int32 count)
			{
				loopCount = count;
			}

			float GetSoundVolume(void) const
			{
				return (soundVolume);
			}

			void SetSoundVolume(float volume)
			{
				soundVolume = volume;
			}

			int32 GetFadeTime(void) const
			{
				return (fadeTime);
			}

			void SetFadeTime(int32 fade)
			{
				fadeTime = fade;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(const ScriptState *state) override;
	};


	class PlaySourceMethod final : public Method
	{
		private:

			Link<Node>		sourceLink;

			PlaySourceMethod(const PlaySourceMethod& playSourceMethod);

			Method *Replicate(void) const override;

			static void SourceComplete(Source *source, void *cookie);

		public:

			C4API PlaySourceMethod();
			C4API ~PlaySourceMethod();

			void Execute(const ScriptState *state) override;
			void Resume(const ScriptState *state) override;
	};


	class StopSourceMethod final : public Method
	{
		private:

			StopSourceMethod(const StopSourceMethod& stopSourceMethod);

			Method *Replicate(void) const override;

		public:

			C4API StopSourceMethod();
			C4API ~StopSourceMethod();

			void Execute(const ScriptState *state) override;
	};


	class VarySourceVolumeMethod final : public Method
	{
		friend class MethodReg<VarySourceVolumeMethod>;

		private:

			float			volumeTarget;
			int32			variationTime;
			bool			stopFlag;

			VarySourceVolumeMethod();
			VarySourceVolumeMethod(const VarySourceVolumeMethod& varySourceVolumeMethod);

			Method *Replicate(void) const override;

		public:

			C4API VarySourceVolumeMethod(float volume, int32 time, bool stop);
			C4API ~VarySourceVolumeMethod();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(const ScriptState *state) override;
	};


	class VarySourceFrequencyMethod final : public Method
	{
		friend class MethodReg<VarySourceFrequencyMethod>;

		private:

			float			frequencyTarget;
			int32			variationTime;
			bool			stopFlag;

			VarySourceFrequencyMethod();
			VarySourceFrequencyMethod(const VarySourceFrequencyMethod& varySourceFrequencyMethod);

			Method *Replicate(void) const override;

		public:

			C4API VarySourceFrequencyMethod(float frequency, int32 time, bool stop);
			C4API ~VarySourceFrequencyMethod();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(const ScriptState *state) override;
	};


	class SetMaterialColorMethod final : public Method
	{
		friend class MethodReg<SetMaterialColorMethod>;

		private:

			enum
			{
				kMaterialAttributeCount = 7
			};

			AttributeType	attributeType;
			ColorRGBA		materialColor;

			static const AttributeType materialAttributeType[kMaterialAttributeCount];

			SetMaterialColorMethod();
			SetMaterialColorMethod(const SetMaterialColorMethod& setMaterialColorMethod);

			Method *Replicate(void) const override;

		public:

			C4API SetMaterialColorMethod(AttributeType type, const ColorRGBA& color);
			C4API ~SetMaterialColorMethod();

			AttributeType GetAttributeType(void) const
			{
				return (attributeType);
			}

			void SetAttributeType(AttributeType type)
			{
				attributeType = type;
			}

			const ColorRGBA& GetMaterialColor(void) const
			{
				return (materialColor);
			}

			void SetMaterialColor(const ColorRGBA& color)
			{
				materialColor = color;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(const ScriptState *state) override;
	};


	class SetShaderParameterMethod : public Method
	{
		private:

			int32			parameterSlot;

		protected:

			Vector4D		parameterValue;

			SetShaderParameterMethod(MethodType type);
			SetShaderParameterMethod(MethodType type, int32 slot, const Vector4D& param);
			SetShaderParameterMethod(const SetShaderParameterMethod& setShaderParameterMethod);

		public:

			~SetShaderParameterMethod();

			int32 GetParameterSlot(void) const
			{
				return (parameterSlot);
			}

			void SetParameterSlot(int32 slot)
			{
				parameterSlot = slot;
			}

			const Vector4D& GetParameterValue(void) const
			{
				return (parameterValue);
			}

			void SetParameterValue(const Vector4D& param)
			{
				parameterValue = param;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(const ScriptState *state) override;
	};


	class SetShaderScalarParameterMethod final : public SetShaderParameterMethod
	{
		friend class MethodReg<SetShaderScalarParameterMethod>;

		private:

			SetShaderScalarParameterMethod();
			SetShaderScalarParameterMethod(const SetShaderScalarParameterMethod& setShaderScalarParameterMethod);

			Method *Replicate(void) const override;

		public:

			C4API SetShaderScalarParameterMethod(int32 slot, float param);
			C4API ~SetShaderScalarParameterMethod();

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;
	};


	class SetShaderVectorParameterMethod final : public SetShaderParameterMethod
	{
		friend class MethodReg<SetShaderVectorParameterMethod>;

		private:

			SetShaderVectorParameterMethod();
			SetShaderVectorParameterMethod(const SetShaderVectorParameterMethod& setShaderVectorParameterMethod);

			Method *Replicate(void) const override;

		public:

			C4API SetShaderVectorParameterMethod(int32 slot, const Vector4D& param);
			C4API ~SetShaderVectorParameterMethod();

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;
	};


	class SetShaderColorParameterMethod final : public SetShaderParameterMethod
	{
		friend class MethodReg<SetShaderColorParameterMethod>;

		private:

			SetShaderColorParameterMethod();
			SetShaderColorParameterMethod(const SetShaderColorParameterMethod& setShaderColorParameterMethod);

			Method *Replicate(void) const override;

		public:

			C4API SetShaderColorParameterMethod(int32 slot, const ColorRGBA& param);
			C4API ~SetShaderColorParameterMethod();

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;
	};
}


#endif

// ZYUQURM
