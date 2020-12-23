 

#ifndef C4Modifiers_h
#define C4Modifiers_h


//# \component	World Manager
//# \prefix		WorldMgr/


#include "C4Node.h"


namespace C4
{
	typedef Type	ModifierType;


	enum : ModifierType
	{
		kModifierUnknown					= '!UNK',
		kModifierAugmentInstance			= 'AGMT',
		kModifierWakeController				= 'WAKE',
		kModifierSleepController			= 'SLEP',
		kModifierConnectInstance			= 'CONN',
		kModifierMoveConnectorInside		= 'MCON',
		kModifierMoveConnectorOutside		= 'MCNO',
		kModifierEnableNodes				= 'ENAB',
		kModifierDisableNodes				= 'DSAB',
		kModifierDeleteNodes				= 'DELT',
		kModifierEnableInteractivity		= 'EINT',
		kModifierDisableInteractivity		= 'DINT',
		kModifierSetPerspectiveMask			= 'PSRP',
		kModifierReplaceMaterial			= 'MATL',
		kModifierRemovePhysics				= 'RPHY'
	};


	enum
	{
		kMaxModifierNodeNameLength			= 15
	};


	typedef String<kMaxModifierNodeNameLength> ModifierNodeName;


	class Modifier;
	class Instance;


	//# \class	ModifierRegistration	Manages internal registration information for a custom modifier type.
	//
	//# The $ModifierRegistration$ class manages internal registration information for a custom modifier type.
	//
	//# \def	class ModifierRegistration : public Registration<Modifier, ModifierRegistration>
	//
	//# \ctor	ModifierRegistration(ModifierType type, const char *name);
	//
	//# \param	type	The modifier type.
	//# \param	name	The modifier name.
	//
	//# \desc
	//# The $ModifierRegistration$ class is abstract and serves as the common base class for the template class
	//# $@ModifierReg@$. A custom modifier is registered with the engine by instantiating an object of type
	//# $ModifierReg<classType>$, where $classType$ is the type of the modifier subclass being registered.
	//
	//# \base	System/Registration<Modifier, ModifierRegistration>		A modifier registration is a specific type of registration object.
	//
	//# \also	$@ModifierReg@$
	//# \also	$@Modifier@$


	//# \function	ModifierRegistration::GetModifierType		Returns the registered modifier type.
	//
	//# \proto	ModifierType GetModifierType(void) const;
	//
	//# \desc
	//# The $GetModifierType$ function returns the modifier type for a particular modifier registration.
	//# The modifier type is established when the modifier registration is constructed.
	//
	//# \also	$@ModifierRegistration::GetModifierName@$


	//# \function	ModifierRegistration::GetModifierName		Returns the human-readable modifier name.
	//
	//# \proto	const char *GetModifierName(void) const;
	//
	//# \desc
	//# The $GetModifierName$ function returns the human-readable modifier name for a particular modifier registration.
	//# The modifier name is established when the modifier registration is constructed.
	//
	//# \also	$@ModifierRegistration::GetModifierType@$


	class ModifierRegistration : public Registration<Modifier, ModifierRegistration>
	{
		private:

			const char		*modifierName;

		protected:
 
			C4API ModifierRegistration(ModifierType type, const char *name);

		public: 

			C4API ~ModifierRegistration(); 

			ModifierType GetModifierType(void) const
			{ 
				return (GetRegistrableType());
			} 
 
			const char *GetModifierName(void) const
			{
				return (modifierName);
			} 

			virtual bool ValidInstance(const Instance *instance) const = 0;
	};


	//# \class	ModifierReg		 Represents a custom modifier type.
	//
	//# The $ModifierReg$ class represents a custom modifier type.
	//
	//# \def	template <class classType> class ModifierReg : public ModifierRegistration
	//
	//# \tparam	classType	The custom modifier class.
	//
	//# \ctor	ModifierReg(ModifierType type, const char *name);
	//
	//# \param	type	The modifier type.
	//# \param	name	The modifier name.
	//
	//# \desc
	//# The $ModifierReg$ template class is used to advertise the existence of a custom modifier type.
	//# The World Manager uses a modifier registration to construct a custom modifier, and the World Editor
	//# examines a modifier registration to determine what type of instance a custom modifier can be assigned to.
	//# The act of instantiating a $ModifierReg$ object automatically registers the corresponding modifier
	//# type. The modifier type is unregistered when the $ModifierReg$ object is destroyed.
	//#
	//# No more than one modifier registration should be created for each distinct modifier type.
	//
	//# \base	ModifierRegistration	All specific modifier registration classes share the common base class $ModifierRegistration$.
	//
	//# \also	$@Modifier@$


	template <class classType> class ModifierReg : public ModifierRegistration
	{
		public:

			ModifierReg(ModifierType type, const char *name) : ModifierRegistration(type, name)
			{
			}

			Modifier *Create(void) const
			{
				return (new classType);
			}

			bool ValidInstance(const Instance *instance) const override
			{
				return ((GetModifierName()) && (classType::ValidInstance(instance)));
			}
	};


	//# \class	Modifier		The base class for all modifier objects.
	//
	//# Every modifier that can be attached to an instance node is a subclass of the $Modifier$ class.
	//
	//# \def	class Modifier : public ListElement<Modifier>, public Packable, public Configurable, public Registrable<Modifier, ModifierRegistration>
	//
	//# \ctor	Modifier(ModifierType type);
	//
	//# \param	type	The modifier type.
	//
	//# \desc
	//# The $Modifier$ class is an object attached to an instance node that causes the instanced world to be altered in
	//# some way when it is loaded into the scene. An application may define its own custom modifiers, and they become
	//# visible in the World Editor.
	//#
	//# A custom modifier type is defined by creating a subclass of the $Modifier$ class. For the modifier
	//# type to be visible in the World Editor, it is also necessary to construct an associated $@ModifierReg@$ object.
	//# All registered custom modifiers should override the $operator ==$ function so that modifiers can be compared
	//# for equality by the World Editor.
	//#
	//# A custom modifier can expose its data to the World Editor by implementing the functions of the
	//# $@InterfaceMgr/Configurable@$ base class.
	//
	//# \base	Utilities/ListElement<Modifier>							Modifiers are stored in a list attached to an instance node.
	//# \base	ResourceMgr/Packable									Modifiers can be packed for storage in resources.
	//# \base	InterfaceMgr/Configurable								Modifiers can define configurable parameters that are exposed as user interface widgets in the World Editor.
	//# \base	System/Registrable<Modifier, ModifierRegistration>		Custom modifier types can be registered with the engine.
	//
	//# \also	$@Instance::GetFirstModifier@$
	//# \also	$@Instance::AddModifier@$
	//# \also	$@ModifierReg@$
	//
	//# \wiki	Modifiers


	//# \function	Modifier::GetModifierType		Returns the modifier type.
	//
	//# \proto	ModifierType GetModifierType(void) const;
	//
	//# \desc
	//# The $GetModifierType$ function returns the modifier type.
	//
	//# \also	$@ModifierReg@$


	//# \function	Modifier::ValidInstance		Returns a boolean value indicating whether the modifier can be assigned to a particular instance node.
	//
	//# \proto	static bool ValidInstance(const Instance *instance);
	//
	//# \param	instance	The instance node to be tested for validity.
	//
	//# \desc
	//# The $ValidInstance$ function can be redefined by modifier subclasses. Its implementation should examine the
	//# instance node pointed to by the $instance$ parameter and return $true$ if the modifier type can be used with the node.
	//# If the modifier type cannot be used, the $ValidInstance$ function should return $false$. If the $ValidInstance$
	//# function is not redefined for a registered subclass of the $Modifier$ class, then that modifier type
	//# can be assigned to any instance node.
	//#
	//# Note that returning $false$ from an implementation of the $ValidInstance$ function does not guarantee that a
	//# particular modifier subclass will not be attached to an instance node, but only that it will not appear in the
	//# list of available modifiers in the World Editor for the instance given by the $instance$ parameter. An implementation
	//# of the $@Modifier::Apply@$ function should make no assumptions about whether the instance node satisfies any
	//# validity requirements checked by the $ValidInstance$ function.


	//# \function	Modifier::Apply			Applies a modifier to an instanced world.
	//
	//# \proto	virtual void Apply(World *world, Instance *instance);
	//
	//# \param	world		The main world inside which the instanced world has been expanded.
	//# \param	instance	The instance node to which the modifier is attached.
	//
	//# \desc
	//# The $Apply$ function is called for each modifier attached to an instance node immediately after the instanced
	//# world is loaded. The $Apply$ function should be overridden by subclasses of the $@Modifier@$ class, and it can
	//# make any changes to the subnodes of the instance node specified by the $instance$ parameter that are necessary
	//# to implement the modifier's functionality.
	//#
	//# Note that the instanced world will be preprocessed after all modifiers have been applied. The $Apply$ function
	//# itself should not call the $@Node::Preprocess@$ function for any new nodes that it creates, and it should not
	//# call the $@Node::AppendNewSubnode@$ function to add nodes to the instanced world because that function calls the
	//# $Preprocess$ function. (The $AppendSubnode$ function should be called instead.)
	//#
	//# A modifier should not make changes to any $@Object@$ classes attached to a node because they are shared
	//# among all copies of the instanced world. Any changes made to these objects would affect all instances
	//# and not just the one to which the modifier is applied.
	//#
	//# The $Apply$ function may not delete the instance node specified by the $instance$ parameter.
	//#
	//# The default implementation of the $Apply$ function performs no action.
	//
	//# \also	$@Modifier::KeepNode@$


	//# \function	Modifier::KeepNode		Returns a boolean value indicating whether a node should be included in a particular copy of an instanced world.
	//
	//# \proto	virtual bool KeepNode(const Node *node) const;
	//
	//# \param	node		A pointer to the node that should be tested for inclusion.
	//
	//# \desc
	//# The $KeepNode$ function is called for each modifier attached to an instance node every time a new copy of
	//# the instanced world is created. This function is called for every subnode of the original, unaltered copy of the
	//# instanced world, and it should return $true$ if that subnode should be copied into the new instance or $false$
	//# if that subnode and its entire subtree should be skipped so that they don't appear in the new instance.
	//#
	//# The default implementation of the $KeepNode$ function always returns $true$ so that the entire instanced world
	//# is always copied into the new instance.
	//
	//# \also	$@Modifier::Apply@$


	class Modifier : public ListElement<Modifier>, public Packable, public Configurable, public Registrable<Modifier, ModifierRegistration>
	{
		friend class Node;

		private:

			ModifierType		modifierType;

			C4API virtual Modifier *Replicate(void) const;

			static Modifier *CreateUnknown(Unpacker& data, unsigned_int32 unpackFlags);

		protected:

			enum
			{
				kModifierNodeTypeCount = 16
			};

			static const NodeType modifierNodeType[kModifierNodeTypeCount];

			C4API Modifier(const Modifier& modifier);

			static Setting *GetNodeTypeSetting(NodeType type);

		public:

			typedef ModifierType KeyType;

			C4API Modifier(ModifierType type);
			C4API virtual ~Modifier();

			ModifierType GetModifierType(void) const
			{
				return (modifierType);
			}

			Modifier *Clone(void) const
			{
				return (Replicate());
			}

			C4API static Modifier *New(ModifierType type);

			C4API static bool ValidInstance(const Instance *instance);
			static void RegisterStandardModifiers(void);

			C4API void PackType(Packer& data) const override;

			C4API virtual bool operator ==(const Modifier& modifier) const;

			C4API virtual void Apply(World *world, Instance *instance);
			C4API virtual bool KeepNode(const Node *node) const;
	};


	class UnknownModifier : public Modifier
	{
		friend class Modifier;
		friend class ModifierReg<UnknownModifier>;

		private:

			ModifierType		unknownType;
			unsigned_int32		unknownSize;
			char				*unknownData;

			UnknownModifier();
			UnknownModifier(ModifierType type);
			UnknownModifier(const UnknownModifier& unknownModifier);

			Modifier *Replicate(void) const override;

		public:

			~UnknownModifier();

			static bool ValidInstance(const Instance *instance);

			void PackType(Packer& data) const override;
			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;
	};


	class AugmentInstanceModifier final : public Modifier
	{
		friend class ModifierReg<AugmentInstanceModifier>;

		private:

			ResourceName	worldName;

			AugmentInstanceModifier();
			AugmentInstanceModifier(const AugmentInstanceModifier& augmentInstanceModifier);

			Modifier *Replicate(void) const override;

		public:

			C4API AugmentInstanceModifier(const char *name);
			C4API ~AugmentInstanceModifier();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			bool operator ==(const Modifier& modifier) const override;

			void Apply(World *world, Instance *instance) override;
	};


	class WakeControllerModifier final : public Modifier
	{
		private:

			WakeControllerModifier(const WakeControllerModifier& wakeControllerModifier);

			Modifier *Replicate(void) const override;

		public:

			C4API WakeControllerModifier();
			C4API ~WakeControllerModifier();

			bool operator ==(const Modifier& modifier) const override;

			void Apply(World *world, Instance *instance) override;
	};


	class SleepControllerModifier final : public Modifier
	{
		private:

			SleepControllerModifier(const SleepControllerModifier& sleepControllerModifier);

			Modifier *Replicate(void) const override;

		public:

			C4API SleepControllerModifier();
			C4API ~SleepControllerModifier();

			bool operator ==(const Modifier& modifier) const override;

			void Apply(World *world, Instance *instance) override;
	};


	class ConnectInstanceModifier final : public Modifier
	{
		friend class ModifierReg<ConnectInstanceModifier>;

		private:

			ConnectorKey			connectorKey;
			ModifierNodeName		targetNodeName;

			ConnectInstanceModifier();
			ConnectInstanceModifier(const ConnectInstanceModifier& connectInstanceModifier);

			Modifier *Replicate(void) const override;

		public:

			C4API ConnectInstanceModifier(const char *key, const char *name);
			C4API ~ConnectInstanceModifier();

			const ConnectorKey& GetConnectorKey(void) const
			{
				return (connectorKey);
			}

			void SetConnectorKey(const ConnectorKey& key)
			{
				connectorKey = key;
			}

			const ModifierNodeName& GetTargetNodeName(void) const
			{
				return (targetNodeName);
			}

			void SetTargetNodeName(const char *name)
			{
				targetNodeName = name;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			bool operator ==(const Modifier& modifier) const override;

			void Apply(World *world, Instance *instance) override;
	};


	class MoveConnectorInsideModifier final : public Modifier
	{
		friend class ModifierReg<MoveConnectorInsideModifier>;

		private:

			ConnectorKey			incomingConnectorKey;
			ModifierNodeName		targetNodeName;

			MoveConnectorInsideModifier();
			MoveConnectorInsideModifier(const MoveConnectorInsideModifier& moveConnectorInsideModifier);

			Modifier *Replicate(void) const override;

		public:

			C4API MoveConnectorInsideModifier(const char *incomingKey, const char *targetName);
			C4API ~MoveConnectorInsideModifier();

			const ConnectorKey& GetIncomingConnectorKey(void) const
			{
				return (incomingConnectorKey);
			}

			void SetIncomingConnectorKey(const ConnectorKey& incomingKey)
			{
				incomingConnectorKey = incomingKey;
			}

			const ModifierNodeName& GetTargetNodeName(void) const
			{
				return (targetNodeName);
			}

			void SetTargetNodeName(const char *targetName)
			{
				targetNodeName = targetName;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			bool operator ==(const Modifier& modifier) const override;

			void Apply(World *world, Instance *instance) override;
	};


	class MoveConnectorOutsideModifier final : public Modifier
	{
		friend class ModifierReg<MoveConnectorOutsideModifier>;

		private:

			ConnectorKey		incomingConnectorKey;
			ConnectorKey		targetConnectorKey;

			MoveConnectorOutsideModifier();
			MoveConnectorOutsideModifier(const MoveConnectorOutsideModifier& moveConnectorOutsideModifier);

			Modifier *Replicate(void) const override;

		public:

			C4API MoveConnectorOutsideModifier(const char *incomingKey, const char *targetKey);
			C4API ~MoveConnectorOutsideModifier();

			const ConnectorKey& GetIncomingConnectorKey(void) const
			{
				return (incomingConnectorKey);
			}

			void SetIncomingConnectorKey(const ConnectorKey& incomingKey)
			{
				incomingConnectorKey = incomingKey;
			}

			const ConnectorKey& GetTargetConnectorKey(void) const
			{
				return (targetConnectorKey);
			}

			void SetTargetConnectorKey(const ConnectorKey& targetKey)
			{
				targetConnectorKey = targetKey;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			bool operator ==(const Modifier& modifier) const override;

			void Apply(World *world, Instance *instance) override;
	};


	class EnableNodesModifier final : public Modifier
	{
		friend class ModifierReg<EnableNodesModifier>;

		private:

			NodeType				nodeType;
			unsigned_int32			nodeHash;
			ModifierNodeName		nodeName;

			EnableNodesModifier();
			EnableNodesModifier(const EnableNodesModifier& enableNodesModifier);

			Modifier *Replicate(void) const override;

		public:

			C4API EnableNodesModifier(NodeType type, const char *name);
			C4API ~EnableNodesModifier();

			NodeType GetNodeType(void) const
			{
				return (nodeType);
			}

			void SetNodeType(NodeType type)
			{
				nodeType = type;
			}

			const ModifierNodeName& GetNodeName(void) const
			{
				return (nodeName);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			C4API void SetNodeName(const char *name);

			bool operator ==(const Modifier& modifier) const override;

			void Apply(World *world, Instance *instance) override;
	};


	class DisableNodesModifier final : public Modifier
	{
		friend class ModifierReg<DisableNodesModifier>;

		private:

			NodeType				nodeType;
			unsigned_int32			nodeHash;
			ModifierNodeName		nodeName;

			DisableNodesModifier();
			DisableNodesModifier(const DisableNodesModifier& disableNodesModifier);

			Modifier *Replicate(void) const override;

		public:

			C4API DisableNodesModifier(NodeType type, const char *name);
			C4API ~DisableNodesModifier();

			NodeType GetNodeType(void) const
			{
				return (nodeType);
			}

			void SetNodeType(NodeType type)
			{
				nodeType = type;
			}

			const ModifierNodeName& GetNodeName(void) const
			{
				return (nodeName);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			C4API void SetNodeName(const char *name);

			bool operator ==(const Modifier& modifier) const override;

			void Apply(World *world, Instance *instance) override;
	};


	class DeleteNodesModifier final : public Modifier
	{
		friend class ModifierReg<DeleteNodesModifier>;

		private:

			NodeType				nodeType;
			unsigned_int32			nodeHash;
			ModifierNodeName		nodeName;

			DeleteNodesModifier();
			DeleteNodesModifier(const DeleteNodesModifier& deleteNodesModifier);

			Modifier *Replicate(void) const override;

		public:

			C4API DeleteNodesModifier(NodeType type, const char *name);
			C4API ~DeleteNodesModifier();

			NodeType GetNodeType(void) const
			{
				return (nodeType);
			}

			void SetNodeType(NodeType type)
			{
				nodeType = type;
			}

			const ModifierNodeName& GetNodeName(void) const
			{
				return (nodeName);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			C4API void SetNodeName(const char *name);

			bool operator ==(const Modifier& modifier) const override;

			bool KeepNode(const Node *node) const override;
	};


	class EnableInteractivityModifier final : public Modifier
	{
		friend class ModifierReg<EnableInteractivityModifier>;

		private:

			unsigned_int32			nodeHash;
			ModifierNodeName		nodeName;

			EnableInteractivityModifier();
			EnableInteractivityModifier(const EnableInteractivityModifier& enableInteractivityModifier);

			Modifier *Replicate(void) const override;

		public:

			C4API EnableInteractivityModifier(const char *name);
			C4API ~EnableInteractivityModifier();

			const ModifierNodeName& GetNodeName(void) const
			{
				return (nodeName);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			C4API void SetNodeName(const char *name);

			bool operator ==(const Modifier& modifier) const override;

			void Apply(World *world, Instance *instance) override;
	};


	class DisableInteractivityModifier final : public Modifier
	{
		friend class ModifierReg<DisableInteractivityModifier>;

		private:

			unsigned_int32			nodeHash;
			ModifierNodeName		nodeName;

			DisableInteractivityModifier();
			DisableInteractivityModifier(const DisableInteractivityModifier& disableInteractivityModifier);

			Modifier *Replicate(void) const override;

		public:

			C4API DisableInteractivityModifier(const char *name);
			C4API ~DisableInteractivityModifier();

			const ModifierNodeName& GetNodeName(void) const
			{
				return (nodeName);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			C4API void SetNodeName(const char *name);

			bool operator ==(const Modifier& modifier) const override;

			void Apply(World *world, Instance *instance) override;
	};


	class SetPerspectiveMaskModifier final : public Modifier
	{
		friend class ModifierReg<SetPerspectiveMaskModifier>;

		private:

			unsigned_int32			perspectiveExclusionMask;

			NodeType				nodeType;
			unsigned_int32			nodeHash;
			ModifierNodeName		nodeName;

			SetPerspectiveMaskModifier();
			SetPerspectiveMaskModifier(const SetPerspectiveMaskModifier& setPerspectiveMaskModifier);

			Modifier *Replicate(void) const override;

		public:

			C4API SetPerspectiveMaskModifier(unsigned_int32 exclusionMask, NodeType type, const char *name);
			C4API ~SetPerspectiveMaskModifier();

			unsigned_int32 GetPerspectiveExclusionMask(void) const
			{
				return (perspectiveExclusionMask);
			}

			void SetPerspectiveExclusionMask(unsigned_int32 exclusionMask)
			{
				perspectiveExclusionMask = exclusionMask;
			}

			NodeType GetNodeType(void) const
			{
				return (nodeType);
			}

			void SetNodeType(NodeType type)
			{
				nodeType = type;
			}

			const ModifierNodeName& GetNodeName(void) const
			{
				return (nodeName);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			C4API void SetNodeName(const char *name);

			bool operator ==(const Modifier& modifier) const override;

			void Apply(World *world, Instance *instance) override;
	};


	class ReplaceMaterialModifier final : public Modifier
	{
		friend class ModifierReg<ReplaceMaterialModifier>;

		private:

			unsigned_int32			nodeHash;
			ModifierNodeName		nodeName;

			MaterialObject			*materialObject;

			ReplaceMaterialModifier();
			ReplaceMaterialModifier(const ReplaceMaterialModifier& replaceMaterialModifier);

			Modifier *Replicate(void) const override;

			static void MaterialObjectLinkProc(Object *object, void *cookie);

		public:

			C4API ReplaceMaterialModifier(const char *name);
			C4API ~ReplaceMaterialModifier();

			const ModifierNodeName& GetNodeName(void) const
			{
				return (nodeName);
			}

			MaterialObject *GetMaterialObject(void) const
			{
				return (materialObject);
			}

			void Prepack(List<Object> *linkList) const override;
			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			C4API void SetNodeName(const char *name);
			C4API void SetMaterialObject(MaterialObject *object);

			bool operator ==(const Modifier& modifier) const override;

			void Apply(World *world, Instance *instance) override;
	};


	class RemovePhysicsModifier final : public Modifier
	{
		private:

			RemovePhysicsModifier(const RemovePhysicsModifier& removePhysicsModifier);

			Modifier *Replicate(void) const override;

		public:

			C4API RemovePhysicsModifier();
			C4API ~RemovePhysicsModifier();

			bool operator ==(const Modifier& modifier) const override;

			void Apply(World *world, Instance *instance) override;
			bool KeepNode(const Node *node) const override;
	};
}


#endif

// ZYUQURM
