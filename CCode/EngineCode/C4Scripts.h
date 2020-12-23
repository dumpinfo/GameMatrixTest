 

#ifndef C4Scripts_h
#define C4Scripts_h


//# \component	Controller System
//# \prefix		Controller/


#include "C4Methods.h"


namespace C4
{
	typedef Array<EventType, 4>		ScriptEventArray;


	enum : ObjectType
	{
		kObjectScript				= 'SCPT'
	};


	enum : ControllerType
	{
		kControllerScript			= 'SCPT'
	};


	//# \enum	ScriptFlags

	enum
	{
		kScriptInitialExecute		= 1 << 0,		//## The script executes immediately when it is loaded.
		kScriptLooping				= 1 << 1,		//## The script always restarts when it finishes running. Methods are not preprocessed again, and variables with script scope retain their values.
		kScriptReentrant			= 1 << 2,		//## Multiple instances of the script can run simultaneously.
		kScriptUniqueInitiators		= 1 << 3		//## Each instance of a running script must have a unique initiator.
	};


	enum
	{
		kScriptAsleep				= 1 << 0
	};


	enum
	{
		kMaxScriptNameLength		= 31
	};


	class ScriptController;


	class ScriptGraph : public Graph<Method, Fiber>, public ListElement<ScriptGraph>, public Packable
	{
		friend class ScriptState;

		private:

			ScriptEventArray				eventArray;
			String<kMaxScriptNameLength>	scriptName;

			ScriptGraph(const ScriptGraph *scriptGraph);

		public:

			C4API ScriptGraph();
			C4API ScriptGraph(const ScriptGraph& scriptGraph);
			C4API ~ScriptGraph();

			ScriptEventArray *GetScriptEventArray(void)
			{
				return (&eventArray);
			}

			const ScriptEventArray *GetScriptEventArray(void) const
			{
				return (&eventArray);
			}

			const char *GetScriptName(void) const
			{
				return (scriptName);
			}

			void SetScriptName(const char *name)
			{
				scriptName = name;
			}

			C4API void Pack(Packer& data, unsigned_int32 packFlags) const override;
			C4API void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			C4API void *BeginSettingsUnpack(void) override;

			void PackScript(Packer& data, unsigned_int32 packFlags) const;
			Method **UnpackScript(Unpacker& data, unsigned_int32 unpackFlags);

			C4API bool HandlesEvent(EventType eventType) const;
	}; 


	class ScriptObject : public Object 
	{
		private: 

			List<ScriptGraph>		scriptGraphList;
			Map<Value>				valueMap; 

			~ScriptObject(); 
 
		public:

			C4API ScriptObject();
			C4API ScriptObject(const ScriptObject *object); 

			ScriptGraph *GetFirstScriptGraph(void) const
			{
				return (scriptGraphList.First());
			}

			ScriptGraph *GetScriptGraph(int32 index) const
			{
				return (scriptGraphList[index]);
			}

			void AddScriptGraph(ScriptGraph *graph)
			{
				scriptGraphList.Append(graph);
			}

			void PurgeScriptGraphs(void)
			{
				scriptGraphList.Purge();
			}

			Value *GetFirstValue(void) const
			{
				return (valueMap.First());
			}

			bool AddValue(Value *value)
			{
				return (valueMap.Insert(value));
			}

			Value *GetValue(const char *name) const
			{
				return (valueMap.Find(name));
			}

			void PurgeValues(void)
			{
				valueMap.Purge();
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;
	};


	//# \class	ScriptState		Represents the state of a running script.
	//
	//# The $ScriptState$ class represents the state of a running script.
	//
	//# \def	class ScriptState : public ListElement<ScriptState>, public Packable, public Memory<ScriptState>
	//
	//# \ctor	ScriptState(ScriptController *controller);
	//
	//# \param	controller		The script controller that owns the script.
	//
	//# \desc
	//# The $ScriptState$ class represents the current state of a running script. A $ScriptState$ object is passed
	//# to the $@Method::Execute@$ and $@Method::Resume@$ functions so that the implementations of those functions
	//# can pass them to other functions that require the script state.
	//
	//# \base	Utilities/ListElement<ScriptState>		Used internally by a script.
	//# \base	ResourceMgr/Packable					Script state can be packed for storage in resources.
	//# \base	MemoryMgr/Memory<ScriptState>			Script state objects are stored in a dedicated heap.
	//
	//# \also	$@Method::Execute@$
	//# \also	$@Method::Resume@$


	//# \function	ScriptState::GetTriggerNode		Returns the trigger node for a script.
	//
	//# \proto	Node *GetTriggerNode(void) const;
	//
	//# \desc
	//# The $GetTriggerNode$ function returns the trigger node that was activated, causing the particular script instance
	//# represented by a $ScriptState$ object to run. If the script was not started by a trigger node being activated, then there
	//# is no trigger node, and the return value is $nullptr$.
	//#
	//# Note that if a particular method targets the trigger node, then it will be returned by the $@Method::GetTargetNode@$ function.
	//
	//# \also	$@ScriptState::GetInitiatorNode@$
	//# \also	$@Method::GetTargetNode@$


	//# \function	ScriptState::GetInitiatorNode		Returns the initiator node for a script.
	//
	//# \proto	Node *GetInitiatorNode(void) const;
	//
	//# \desc
	//# The $GetInitiatorNode$ function returns the node that originally caused the particular script instance represented by a
	//# $ScriptState$ object to run. If a trigger caused the script to run, then the initiator node is the node that interacted with the
	//# trigger, activating it or deactivating it. If the script was started by direct activation through another script, then the
	//# initiator node is the node that caused that other script to run. The initiator node can be $nullptr$.
	//#
	//# Note that if a particular method targets the trigger node, then it will be returned by the $@Method::GetTargetNode@$ function.
	//
	//# \also	$@ScriptState::GetTriggerNode@$
	//# \also	$@Method::GetTargetNode@$


	//# \function	ScriptState::GetScriptTime		Returns the time elapsed since a script began running.
	//
	//# \proto	float GetScriptTime(void) const;
	//
	//# \desc
	//# The $GetScriptTime$ function returns the time elapsed, as a floating-point value measured in seconds, since a script
	//# began running. The script time is updated once per frame, so all methods executed during the same frame observe the
	//# same script time. During the first frame in which a script is running, the script time is guaranteed to be exactly 0.0.


	//# \function	ScriptState::GetValue		Returns a script variable.
	//
	//# \proto	Value *GetValue(const char *name) const;
	//
	//# \param	name	The name of the value to retrieve.
	//
	//# \desc
	//# The $GetValue$ function returns the script variable specified by the $name$ parameter. If no variable by that name exists,
	//# then the return value is $nullptr$.
	//
	//# \also	$@Value@$


	class ScriptState : public ListElement<ScriptState>, public Packable, public EngineMemory<ScriptState>
	{
		private:

			ScriptController			*scriptController;
			ScriptObject				*scriptObject;

			Link<Node>					initiatorNodeLink;
			Link<Node>					triggerNodeLink;

			EventType					scriptEvent;
			float						scriptTime;

			List<Reference<Method>>		loopList;
			List<Reference<Method>>		readyList;
			List<Reference<Method>>		executingList;
			List<Reference<Method>>		completeList;

			ScriptGraph					scriptGraph;
			Map<Value>					valueMap;

			static void ScriptObjectLinkProc(Object *object, void *cookie);
			static void InitiatorLinkProc(Node *node, void *cookie);
			static void TriggerLinkProc(Node *node, void *cookie);

			void ExecuteMethod(Method *method, bool dead = false);
			static void MethodComplete(Method *method, void *cookie);

			void StartScript(void);

		public:

			ScriptState(ScriptController *controller);
			ScriptState(ScriptController *controller, ScriptObject *object, const ScriptGraph *graph);
			~ScriptState();

			const ScriptController *GetScriptController(void) const
			{
				return (scriptController);
			}

			Node *GetTriggerNode(void) const
			{
				return (triggerNodeLink);
			}

			Node *GetInitiatorNode(void) const
			{
				return (initiatorNodeLink);
			}

			EventType GetScriptEvent(void) const
			{
				return (scriptEvent);
			}

			float GetScriptTime(void) const
			{
				return (scriptTime);
			}

			void Prepack(List<Object> *linkList) const override;
			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;

			Node *GetScriptControllerTarget(void) const;

			C4API Value *GetValue(const char *name) const;

			void Preprocess(void);

			void ExecuteScript(Node *initiator, Node *trigger, EventType eventType);
			void ResumeScript(void);

			void ScriptTask(void);
	};


	//# \class	ScriptController		Manages a scripted node in a world.
	//
	//# The $ScriptController$ class manages a scripted node in a world.
	//
	//# \def	class ScriptController : public Controller
	//
	//# \ctor	ScriptController();
	//
	//# \desc
	//# The $ScriptController$ class manages a scripted node in a world.
	//
	//# \base	Controller		The $ScriptController$ class is a specialized controller.
	//
	//# \also	$@Method@$
	//# \also	$@ScriptState@$


	class ScriptController : public Controller
	{
		private:

			unsigned_int32			scriptFlags;
			unsigned_int32			scriptState;

			ScriptObject			*scriptObject;
			Map<Value>				valueMap;

			List<ScriptState>		executeList;
			List<ScriptState>		resumeList;

			DeferredTask			initialExecuteTask;
			DeferredTask			initialResumeTask;

			Controller *Replicate(void) const override;

			static void ScriptObjectLinkProc(Object *object, void *cookie);

			static void InitialExecuteTask(DeferredTask *event, void *cookie);
			static void InitialResumeTask(DeferredTask *event, void *cookie);

		protected:

			C4API ScriptController(ControllerType type);
			C4API ScriptController(const ScriptController& scriptController);

		public:

			C4API ScriptController();
			C4API ~ScriptController();

			unsigned_int32 GetScriptFlags(void) const
			{
				return (scriptFlags);
			}

			void SetScriptFlags(unsigned_int32 flags)
			{
				scriptFlags = flags;
			}

			bool ScriptAsleep(void) const
			{
				return ((scriptState & kScriptAsleep) != 0);
			}

			ScriptObject *GetScriptObject(void) const
			{
				return (scriptObject);
			}

			Value *GetFirstValue(void) const
			{
				return (valueMap.First());
			}

			bool AddValue(Value *value)
			{
				return (valueMap.Insert(value));
			}

			Value *GetValue(const char *name) const
			{
				return (valueMap.Find(name));
			}

			void PurgeValues(void)
			{
				valueMap.Purge();
			}

			void Terminate(void)
			{
				executeList.Purge();
			}

			C4API void Prepack(List<Object> *linkList) const override;
			C4API void Pack(Packer& data, unsigned_int32 packFlags) const override;
			C4API void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			C4API int32 GetSettingCount(void) const override;
			C4API Setting *GetSetting(int32 index) const override;
			C4API void SetSetting(const Setting *setting) override;

			C4API void Preprocess(void) override;
			C4API void Move(void) override;

			C4API void Wake(void) override;
			C4API void Sleep(void) override;

			C4API void Activate(Node *initiator, Node *trigger) override;
			C4API void Deactivate(Node *initiator, Node *trigger) override;

			C4API void ExecuteScript(ScriptObject *object, Node *initiator = nullptr, Node *trigger = nullptr, EventType eventType = kEventNone);
			C4API void SetScriptObject(ScriptObject *object);
	};


	inline Node *ScriptState::GetScriptControllerTarget(void) const
	{
		return (scriptController->GetTargetNode());
	}
}


#endif

// ZYUQURM
