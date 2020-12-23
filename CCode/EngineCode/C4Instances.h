 

#ifndef C4Instances_h
#define C4Instances_h


//# \component	World Manager
//# \prefix		WorldMgr/


#include "C4Node.h"
#include "C4Modifiers.h"


namespace C4
{
	//# \class	Instance		Represents an instanced world node in a world.
	//
	//# The $Instance$ class represents an instanced world node in a world.
	//
	//# \def	class Instance final : public Node
	//
	//# \ctor	Instance(const char *name);
	//
	//# \param	name		The name of the instanced world.
	//
	//# \desc
	//# The $Instance$ class represents an instance node from which another world
	//# (the instanced world) can be expanded.
	//
	//# \base	Node	An instance is a type of node.
	//
	//# \wiki	Models_and_Instanced_Worlds		Models and Instanced Worlds
	//# \wiki	Worlds_Page						Worlds Page


	//# \function	Instance::GetWorldName		Returns the name of an instanced world.
	//
	//# \proto	const ResourceName& GetWorldName(void) const;
	//
	//# \desc
	//# The $GetWorldName$ function returns the name of the world resource referenced by an instance node.
	//
	//# \also	$@Instance::SetWorldName@$


	//# \function	Instance::SetWorldName		Sets the name of an instanced world.
	//
	//# \proto	void SetWorldName(const char *name);
	//
	//# \param	name	The name of the instanced world.
	//
	//# \desc
	//# The $SetWorldName$ function sets the name of the world resource referenced by an instance node
	//# to that specified by the $name$ parameter.
	//
	//# \also	$@Instance::GetWorldName@$


	//# \function	Instance::GetFirstModifier	Returns the first modifier for an instanced world.
	//
	//# \proto	Modifier *GetFirstModifier(void) const;
	//
	//# \desc
	//# The $GetFirstModifier$ function returns the first modifier attached to an instanced world.
	//# If the instance has no modifiers, then the return value is $nullptr$.
	//
	//# \also	$@Instance::AddModifier@$
	//# \also	$@Modifier@$


	//# \function	Instance::AddModifier		Adds a modifier to an instanced world.
	//
	//# \proto	void AddModifier(Modifier *modifier);
	//
	//# \param	modifier	The modifier to add to the instance.
	//
	//# \desc
	//# The $AddModifier$ function adds the modifier specified by the $modifier$ parameter to an
	//# instanced world.
	//
	//# \also	$@Instance::GetFirstModifier@$
	//# \also	$@Modifier@$


	class Instance final : public Node
	{
		friend class Node;

		private:

			List<Modifier>		modifierList;
			ResourceName		worldName;

			Instance();
			Instance(const Instance& instance);

			Node *Replicate(void) const override;

			static bool ModifierCloneFilter(const Node *node, void *cookie);

		public:
 
			C4API Instance(const char *name);
			C4API ~Instance();
 
			Modifier *GetFirstModifier(void) const
			{ 
				return (modifierList.First());
			}
 
			void AddModifier(Modifier *modifier)
			{ 
				modifierList.Append(modifier); 
			}

			void PurgeModifiers(void)
			{ 
				modifierList.Purge();
			}

			const ResourceName& GetWorldName(void) const
			{
				return (worldName);
			}

			void SetWorldName(const char *name)
			{
				worldName = name;
			}

			void Prepack(List<Object> *linkList) const override;
			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;

			int32 GetCategoryCount(void) const override;
			Type GetCategoryType(int32 index, const char **title) const override;
			int32 GetCategorySettingCount(Type category) const override;
			Setting *GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const override;
			void SetCategorySetting(Type category, const Setting *setting) override;

			int32 GetInternalConnectorCount(void) const override;
			const char *GetInternalConnectorKey(int32 index) const override;
			bool ValidConnectedNode(const ConnectorKey& key, const Node *node) const override;

			C4API Node *FindExtractableNode(void) const;

			C4API bool Expand(World *world);
			C4API void Extract(World *world);
			C4API void Collapse(void);
	};
}


#endif

// ZYUQURM
