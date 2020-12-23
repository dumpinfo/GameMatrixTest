 

#ifndef C4Properties_h
#define C4Properties_h


//# \component	World Manager
//# \prefix		WorldMgr/


#include "C4Objects.h"
#include "C4Textures.h"
#include "C4Configurable.h"


namespace C4
{
	enum
	{
		kMaxNodeNameLength = 63
	};


	typedef Type							PropertyType;
	typedef String<kMaxNodeNameLength>		NodeName;


	enum : ObjectType
	{
		kObjectProperty			= 'PROP'
	};


	enum : PropertyType
	{
		kPropertyUnknown		= '!UNK',
		kPropertyName			= 'NAME',
		kPropertyClear			= 'CLER',
		kPropertyFarClip		= 'FCLP',
		kPropertyImpostor		= 'IPST',
		kPropertyInteraction	= 'INTR',
		kPropertyAcoustics		= 'ACST'
	};


	//# \enum	PropertyFlags

	enum
	{
		kPropertyNonpersistent	= 1 << 0,		//## The property is skipped during world serialization.
		kPropertyDisabled		= 1 << 1		//## The property is disabled (but is still returned by $@Node::GetProperty@$).
	};


	class Property;


	//# \class	PropertyRegistration	Manages internal registration information for a custom property type.
	//
	//# The $PropertyRegistration$ class manages internal registration information for a custom property type.
	//
	//# \def	class PropertyRegistration : public Registration<Property, PropertyRegistration>
	//
	//# \ctor	PropertyRegistration(PropertyType type, const char *name);
	//
	//# \param	type	The property type.
	//# \param	name	The property name.
	//
	//# \desc
	//# The $PropertyRegistration$ class is abstract and serves as the common base class for the template class
	//# $@PropertyReg@$. A custom property is registered with the engine by instantiating an object of type
	//# $PropertyReg<classType>$, where $classType$ is the type of the property subclass being registered.
	//
	//# \base	System/Registration<Property, PropertyRegistration>		A property registration is a specific type of registration object.
	//
	//# \also	$@PropertyReg@$
	//# \also	$@Property@$


	//# \function	PropertyRegistration::GetPropertyType		Returns the registered property type.
	//
	//# \proto	PropertyType GetPropertyType(void) const;
	//
	//# \desc
	//# The $GetPropertyType$ function returns the property type for a particular property registration.
	//# The property type is established when the property registration is constructed.
	//
	//# \also	$@PropertyRegistration::GetPropertyName@$


	//# \function	PropertyRegistration::GetPropertyName		Returns the human-readable property name.
	//
	//# \proto	const char *GetPropertyName(void) const;
	//
	//# \desc
	//# The $GetPropertyName$ function returns the human-readable property name for a particular property registration.
	//# The property name is established when the property registration is constructed.
	//
	//# \also	$@PropertyRegistration::GetPropertyType@$


	class PropertyRegistration : public Registration<Property, PropertyRegistration>
	{ 
		private:

			const char		*propertyName; 

		protected: 

			C4API PropertyRegistration(PropertyType type, const char *name);
 
		public:
 
			C4API ~PropertyRegistration(); 

			PropertyType GetPropertyType(void) const
			{
				return (GetRegistrableType()); 
			}

			const char *GetPropertyName(void) const
			{
				return (propertyName);
			}

			virtual bool ValidNode(const Node *node) const = 0;
	};


	//# \class	PropertyReg		 Represents a custom property type.
	//
	//# The $PropertyReg$ class represents a custom property type.
	//
	//# \def	template <class classType> class PropertyReg : public PropertyRegistration
	//
	//# \tparam	classType	The custom property class.
	//
	//# \ctor	PropertyReg(PropertyType type, const char *name);
	//
	//# \param	type	The property type.
	//# \param	name	The property name.
	//
	//# \desc
	//# The $PropertyReg$ template class is used to advertise the existence of a custom property type.
	//# The World Manager uses a property registration to construct a custom property, and the World Editor
	//# examines a property registration to determine what type of node a custom property can be assigned to.
	//# The act of instantiating a $PropertyReg$ object automatically registers the corresponding property
	//# type. The property type is unregistered when the $PropertyReg$ object is destroyed.
	//#
	//# No more than one property registration should be created for each distinct property type.
	//
	//# \base	PropertyRegistration	All specific property registration classes share the common base class $PropertyRegistration$.
	//
	//# \also	$@Property@$


	template <class classType> class PropertyReg : public PropertyRegistration
	{
		public:

			PropertyReg(PropertyType type, const char *name) : PropertyRegistration(type, name)
			{
			}

			Property *Create(void) const
			{
				return (new classType);
			}

			bool ValidNode(const Node *node) const override
			{
				return ((GetPropertyName()) && (classType::ValidNode(node)));
			}
	};


	//# \class	Property	The base class for all property objects.
	//
	//# Every property that can be attached to a scene graph node is a subclass of the $Property$ class.
	//
	//# \def	class Property : public MapElement<Property>, public Packable, public Configurable, public Registrable<Property, PropertyRegistration>
	//
	//# \ctor	Property(PropertyType type);
	//
	//# \param	type	The property type.
	//
	//# \desc
	//# The $Property$ class is a generic container for any kind of special information that can be attached
	//# to a scene graph node. An application may define its own custom properties, and they become visible in the
	//# World Editor.
	//#
	//# A custom property type is usually defined by creating a subclass of the $Property$ class. For the property
	//# type to be visible in the World Editor, it is also necessary to construct an associated $@PropertyReg@$ object.
	//# Although a subclass is normally created for custom property types, using the base class alone is allowed if
	//# the property does not need to carry any information beyond its type. (It may be sufficient for the application
	//# to simply detect that a particular type of property exists for a node.) In this case, the custom property type
	//# still needs to be registered using the $@PropertyReg@$ template, but the $classType$ template parameter may
	//# simply be $Property$.
	//#
	//# A custom property can expose its data to the World Editor by implementing the functions of the
	//# $@InterfaceMgr/Configurable@$ base class.
	//
	//# \base	Utilities/MapElement<Property>						Properties are stored in a map.
	//# \base	ResourceMgr/Packable								Properties can be packed for storage in resources.
	//# \base	InterfaceMgr/Configurable							Properties can define configurable parameters that are exposed
	//#																as user interface widgets in the World Editor.
	//# \base	System/Registrable<Property, PropertyRegistration>	Custom property types can be registered with the engine.
	//
	//# \also	$@Node::GetFirstProperty@$
	//# \also	$@Node::GetProperty@$
	//# \also	$@Node::AddProperty@$
	//# \also	$@PropertyObject@$
	//# \also	$@PropertyReg@$


	//# \function	Property::GetPropertyType		Returns the property type.
	//
	//# \proto	PropertyType GetPropertyType(void) const;
	//
	//# \desc
	//# The $GetPropertyType$ function returns the property type.
	//
	//# \also	$@PropertyReg@$


	//# \function	Property::GetPropertyFlags		Returns the property flags.
	//
	//# \proto	unsigned_int32 GetPropertyFlags(void) const;
	//
	//# \desc
	//# The $GetPropertyFlags$ function returns the property flags, which can be zero or a combination
	//# (through logical OR) of the following constants.
	//
	//# \table	PropertyFlags
	//
	//# \also	$@Property::SetPropertyFlags@$


	//# \function	Property::SetPropertyFlags		Returns the property flags.
	//
	//# \proto	void SetPropertyFlags(unsigned_int32 flags);
	//
	//# \param	flags	The new property flags.
	//
	//# \desc
	//# The $SetPropertyFlags$ function sets the property flags to the value specified by the $flags$ parameter,
	//# which can be zero or a combination (through logical OR) of the following constants.
	//
	//# \table	PropertyFlags
	//
	//# If the $kPropertyNonpersistent$ flag is set, then the property will not be written when the world it
	//# belongs to is saved, and it will no longer exist when the world is reloaded.
	//
	//# \also	$@Property::GetPropertyFlags@$


	//# \function	Property::ValidNode		Returns a boolean value indicating whether a property can be assigned to a particular node.
	//
	//# \proto	static bool ValidNode(const Node *node);
	//
	//# \desc
	//# The $ValidNode$ function should be redefined by property subclasses. Its implementation should examine the
	//# node pointed to by the $node$ parameter and return $true$ if the property type can be used with the node.
	//# If the property type cannot be used, the $ValidNode$ function should return $false$. If the $ValidNode$
	//# function is not redefined for a registered subclass of the $Property$ class, then that property type
	//# can be assigned to any node.


	//# \function	Property::SharedProperty		Returns a boolean value indicating whether a property is supposed to be shared.
	//
	//# \proto	virtual bool SharedProperty(void) const;
	//
	//# \desc
	//# The $SharedProperty$ function should be overridden by any $Property$ subclass that implements a shared property.
	//# Whereas ordinary properties are attached directly to a node, shared properties are stored in a $@PropertyObject@$ that
	//# is shared by multiple nodes. When a node is copied, properties attached directly to the node are copied to the new node,
	//# but the property object is simply shared by the new node without any copying.
	//#
	//# The $SharedProperty$ function should return $true$ if the property should be stored in a property object, and it should
	//# return $false$ if the property should be attached directly to a node. The default implementation returns $false$.
	//
	//# \also	$@PropertyObject@$


	class Property : public MapElement<Property>, public Packable, public Configurable, public Registrable<Property, PropertyRegistration>
	{
		friend class PropertyObject;
		friend class Node;

		private:

			PropertyType		propertyType;
			unsigned_int32		propertyFlags;

			C4API virtual Property *Replicate(void) const;

			static Property *Create(Unpacker& data, unsigned_int32 unpackFlags = 0);
			static Property *CreateUnknown(Unpacker& data, unsigned_int32 unpackFlags);

		protected:

			C4API Property(const Property& property);

		public:

			typedef PropertyType KeyType;

			C4API Property(PropertyType type);
			C4API virtual ~Property();

			KeyType GetKey(void) const
			{
				return (propertyType);
			}

			PropertyType GetPropertyType(void) const
			{
				return (propertyType);
			}

			unsigned_int32 GetPropertyFlags(void) const
			{
				return (propertyFlags);
			}

			void SetPropertyFlags(unsigned_int32 flags)
			{
				propertyFlags = flags;
			}

			Property *Clone(void) const
			{
				return (Replicate());
			}

			C4API static Property *New(PropertyType type);

			C4API static bool ValidNode(const Node *node);
			static void RegisterStandardProperties(void);

			C4API void PackType(Packer& data) const override;
			C4API void Pack(Packer& data, unsigned_int32 packFlags) const override;
			C4API void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			C4API virtual bool SharedProperty(void) const;
	};


	//# \class	PropertyObject		A shared storage container for properties.
	//
	//# \def	class PropertyObject : public Object
	//
	//# \ctor	PropertyObject();
	//
	//# \desc
	//# The $PropertyObject$ class is a shared storage container for properties. It is attached to a node so that
	//# specific properties can be shared among all instances of that node.
	//
	//# Each type of property should either always be attached directly to a node or always be stored in a property
	//# object. The $@Property::SharedProperty@$ function should be overridden by property subclasses to indicate
	//# which kind of property they are.
	//
	//# \base	Object		A $PropertyObject$ is an object that can be shared by multiple nodes.
	//
	//# \also	$@Property@$
	//# \also	$@Node::GetPropertyObject@$
	//# \also	$@Node::SetPropertyObject@$


	//# \function	PropertyObject::GetProperty		Returns the property of a given type that is stored in a property object.
	//
	//# \proto	Property *GetProperty(PropertyType type) const;
	//
	//# \param	type	The property type.
	//
	//# \desc
	//# The $GetProperty$ function returns the property stored in a property object having the type specified
	//# by the $type$ parameter. If no such property exists, then the return value is $nullptr$.
	//
	//# \also	$@PropertyObject::GetFirstProperty@$
	//# \also	$@PropertyObject::AddProperty@$
	//# \also	$@Property@$


	//# \function	PropertyObject::GetFirstProperty		Returns the first property stored in a property object.
	//
	//# \proto	Property *GetFirstProperty(void) const;
	//
	//# \desc
	//# The $GetFirstProperty$ function returns the first property stored in a property object. All of the
	//# properties stored in a property object can be iterated by repeatedly calling the $@Utilities/ListElement::Next@$
	//# function on the returned pointer. If no properties are stored in a property object, then the return
	//# value is $nullptr$.
	//
	//# \also	$@PropertyObject::GetProperty@$
	//# \also	$@PropertyObject::AddProperty@$
	//# \also	$@Property@$


	//# \function	PropertyObject::AddProperty		Stores a property in a property object.
	//
	//# \proto	void AddProperty(Property *property);
	//
	//# \param	property	The property to attach.
	//
	//# \desc
	//# The $AddProperty$ function stores the property specified by the $property$ parameter in a property object.
	//# A property can be stored in only one property object at a time, so the property is removed from any other property
	//# object in which it may have previously been stored.
	//
	//# \also	$@PropertyObject::GetFirstProperty@$
	//# \also	$@PropertyObject::GetProperty@$
	//# \also	$@Property@$


	class PropertyObject : public Object
	{
		private:

			Map<Property>		propertyMap;

			~PropertyObject();

		public:

			C4API PropertyObject();

			int32 GetPropertyCount(void) const
			{
				return (propertyMap.GetElementCount());
			}

			Property *GetProperty(PropertyType type) const
			{
				return (propertyMap.Find(type));
			}

			Property *GetFirstProperty(void) const
			{
				return (propertyMap.First());
			}

			void AddProperty(Property *property)
			{
				propertyMap.Insert(property);
			}

			void Prepack(List<Object> *linkList) const override;
			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;
	};


	class UnknownProperty : public Property
	{
		friend class Property;
		friend class PropertyReg<UnknownProperty>;

		private:

			PropertyType		unknownType;
			unsigned_int32		unknownSize;
			char				*unknownData;

			UnknownProperty();
			UnknownProperty(PropertyType type);
			UnknownProperty(const UnknownProperty& unknownProperty);

			Property *Replicate(void) const override;

		public:

			~UnknownProperty();

			static bool ValidNode(const Node *node);

			void PackType(Packer& data) const override;
			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;
	};


	class NameProperty : public Property
	{
		friend class Property;

		private:

			NodeName		nodeName;

			NameProperty();
			NameProperty(const NameProperty& nameProperty);

			Property *Replicate(void) const override;

		public:

			C4API NameProperty(const char *name);
			C4API ~NameProperty();

			const NodeName& GetNodeName(void) const
			{
				return (nodeName);
			}

			void SetNodeName(const char *name)
			{
				nodeName = name;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
	};


	class ClearProperty : public Property
	{
		friend class PropertyReg<ClearProperty>;

		private:

			ColorRGBA		clearColor;

			ClearProperty();
			ClearProperty(const ClearProperty& clearProperty);

			Property *Replicate(void) const override;

		public:

			C4API ClearProperty(const ColorRGBA& color);
			C4API ~ClearProperty();

			const ColorRGBA& GetClearColor(void) const
			{
				return (clearColor);
			}

			void SetClearColor(const ColorRGBA& color)
			{
				clearColor = color;
			}

			static bool ValidNode(const Node *node);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;
	};


	class FarClipProperty : public Property
	{
		friend class PropertyReg<FarClipProperty>;

		private:

			float		farClipDepth;

			FarClipProperty();
			FarClipProperty(const FarClipProperty& farClipProperty);

			Property *Replicate(void) const override;

		public:

			C4API FarClipProperty(float depth);
			C4API ~FarClipProperty();

			float GetFarClipDepth(void) const
			{
				return (farClipDepth);
			}

			void SetFarClipDepth(float depth)
			{
				farClipDepth = depth;
			}

			static bool ValidNode(const Node *node);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;
	};


	class InteractionProperty : public Property
	{
		private:

			InteractionProperty(const InteractionProperty& interactionProperty);

			Property *Replicate(void) const override;

		public:

			C4API InteractionProperty();
			C4API ~InteractionProperty();

			static bool ValidNode(const Node *node);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;
	};


	class AcousticsProperty : public Property
	{
		friend class PropertyReg<AcousticsProperty>;

		private:

			float		directVolume;
			float		directHFVolume;
			float		reflectionVolume;
			float		reflectionHFVolume;

			AcousticsProperty();
			AcousticsProperty(const AcousticsProperty& acousticsProperty);

			Property *Replicate(void) const override;

		public:

			C4API AcousticsProperty(float direct, float directHF, float reflection, float reflectionHF);
			C4API ~AcousticsProperty();

			float GetDirectVolume(void) const
			{
				return (directVolume);
			}

			void SetDirectVolume(float volume)
			{
				directVolume = volume;
			}

			float GetDirectHFVolume(void) const
			{
				return (directHFVolume);
			}

			void SetDirectHFVolume(float volume)
			{
				directHFVolume = volume;
			}

			float GetReflectionVolume(void) const
			{
				return (reflectionVolume);
			}

			void SetReflectionVolume(float volume)
			{
				reflectionVolume = volume;
			}

			float GetReflectionHFVolume(void) const
			{
				return (reflectionHFVolume);
			}

			void SetReflectionHFVolume(float volume)
			{
				reflectionHFVolume = volume;
			}

			static bool ValidNode(const Node *node);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;
	};
}


#endif

// ZYUQURM
