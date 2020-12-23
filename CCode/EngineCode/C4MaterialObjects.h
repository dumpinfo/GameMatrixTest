 

#ifndef C4MaterialObjects_h
#define C4MaterialObjects_h


//# \component	Graphics Manager
//# \prefix		GraphicsMgr/


#include "C4Objects.h"
#include "C4Attributes.h"


namespace C4
{
	typedef Type	MaterialType;
	typedef Type	SubstanceType;
	typedef Type	TextureBlendMode;


	enum : ObjectType
	{
		kObjectMaterial				= 'MATL'
	};


	enum : MaterialType
	{
		kMaterialGeneric			= 0
	};


	enum : SubstanceType
	{
		kSubstanceNone				= 0,
		kSubstanceFlesh				= 1
	};


	enum
	{
		kMaxMaterialTexcoordCount	= 2
	};


	//# \enum	MaterialFlags

	enum
	{
		kMaterialTwoSided					= 1 << 0,			//## The material is rendered two-sided.
		kMaterialAlphaTest					= 1 << 1,			//## Use alpha testing with the material.
		kMaterialEmissionGlow				= 1 << 2,			//## The emission term contributes to the glow post-processing effect.
		kMaterialSpecularBloom				= 1 << 3,			//## The specular term contributes to the bloom post-processing effect.
		kMaterialVertexOcclusion			= 1 << 4,			//## Apply per-vertex ambient occlusion using the alpha channel of the vertex color.
		kMaterialAlphaCoverage				= 1 << 5,			//## Use the alpha value to control multisample coverage.
		kMaterialSampleShading				= 1 << 6,			//## Enable per-sample fragment shading if available. (This can have a significant performance impact.)
		kMaterialOcclusionInhibit			= 1 << 7,			//## Do not apply screen-space ambient occlusion (SSAO).
		kMaterialRadiositySpaceInhibit		= 1 << 8,			//## Always render without applying radiosity spaces.
		kMaterialAnimateTexcoord0			= 1 << 16,			//## Apply animation to the primary texcoord.
		kMaterialAnimateTexcoord1			= 1 << 17,			//## Apply animation to the secondary texcoord.
		kMaterialAlphaSemanticInhibit		= 1 << 31,
		kMaterialShaderStateMask			= kMaterialTwoSided | kMaterialEmissionGlow | kMaterialSpecularBloom | kMaterialAlphaCoverage | kMaterialSampleShading
	};


	//# \enum	TextureBlendMode

	enum : TextureBlendMode
	{
		kTextureBlendAdd						= 'ADD ',		//## Add the texture values.
		kTextureBlendAverage					= 'AVG ',		//## Average the texture values.
		kTextureBlendMultiply					= 'MULT',		//## Multiply the texture values.
		kTextureBlendVertexAlpha				= 'VTXA',		//## Blend the texture values using the interpolated vertex alpha.
		kTextureBlendPrimaryAlpha				= 'PRMA',		//## Blend the texture values using the alpha channel from the primary texture.
		kTextureBlendSecondaryAlpha				= 'SCDA',		//## Blend the texture values using the alpha channel from the secondary texture.
		kTextureBlendPrimaryInverseAlpha		= 'PRIA',		//## Blend the texture values using the inverse of the alpha channel from the primary texture.
		kTextureBlendSecondaryInverseAlpha		= 'SCIA'		//## Blend the texture values using the inverse of the alpha channel from the secondary texture.
	};


	//# \class	Substance	Encapsulates a substance type.
	//
	//# The $Substance$ class encapsulates a substance type.
	//
	//# \def	class Substance : public MapElement<Substance>
	//
	//# \ctor	Substance(SubstanceType type, const char *name);
	//
	//# \param	type	A unique 32-bit identifier that represents the type of the substance.
	//# \param	name	The name of the substance that is displayed in the World Editor.
	//#					The string to which this points must remain available in memory as int32 as the $Substance$ object exists.
	//
	//# \desc
	//# The $Substance$ class is used to hold information about a particular substance, such as wood or metal,
	//# that can be associated with a material object or a physics shape. An instance of the $Substance$ class
	//# is typically created by the application for each different type of substance and registered with the engine
	//# by calling the $@MaterialObject::RegisterSubstance@$ function. A subclass of the $Substance$ class can
	//# be used to store additional application-specific information about each substance.
	//
	//# \base	Utilities/MapElement<Substance>		Used internally.
	//
	//# \also	$@MaterialObject::GetMaterialSubstance@$ 
	//# \also	$@MaterialObject::SetMaterialSubstance@$
	//# \also	$@MaterialObject::GetFirstRegisteredSubstance@$
	//# \also	$@MaterialObject::FindRegisteredSubstance@$ 
	//# \also	$@MaterialObject::RegisterSubstance@$
	//# \also	$@PhysicsMgr/ShapeObject::GetShapeSubstance@$ 
	//# \also	$@PhysicsMgr/ShapeObject::SetShapeSubstance@$

 
	class Substance : public MapElement<Substance>
	{ 
		private: 

			SubstanceType		substanceType;
			const char			*substanceName;
 
		public:

			typedef SubstanceType KeyType;

			C4API Substance(SubstanceType type, const char *name);
			C4API virtual ~Substance();

			KeyType GetKey(void) const
			{
				return (substanceType);
			}

			SubstanceType GetSubstanceType(void) const
			{
				return (substanceType);
			}

			const char *GetSubstanceName(void) const
			{
				return (substanceName);
			}
	};


	class MaterialRegistration : public MapElement<MaterialRegistration>
	{
		private:

			MaterialType		materialType;
			const char			*resourceName;
			const void			*materialCookie;

		public:

			typedef MaterialType KeyType;

			C4API MaterialRegistration(MaterialType type, const char *name, const void *cookie = nullptr);
			C4API ~MaterialRegistration();

			KeyType GetKey(void) const
			{
				return (materialType);
			}

			MaterialType GetMaterialType(void) const
			{
				return (materialType);
			}

			const char *GetResourceName(void) const
			{
				return (resourceName);
			}

			const void *GetMaterialCookie(void) const
			{
				return (materialCookie);
			}
	};


	class MaterialResource : public Resource<MaterialResource>
	{
		friend class Resource<MaterialResource>;

		private:

			static C4API ResourceDescriptor		descriptor;

			~MaterialResource();

		public:

			C4API MaterialResource(const char *name, ResourceCatalog *catalog);

			int32 GetEndian(void) const
			{
				return (static_cast<const int32 *>(GetData())[0]);
			}

			int32 GetVersion(void) const
			{
				return (static_cast<const int32 *>(GetData())[1]);
			}

			const void *GetMaterialData(void) const
			{
				return (&static_cast<const int32 *>(GetData())[3]);
			}
	};


	//# \class	MaterialObject		Encapsulates a set of material properties.
	//
	//# The $MaterialObject$ class encapsulates a set of material properties.
	//
	//# \def	class MaterialObject : public Object
	//
	//# \ctor	MaterialObject();
	//
	//# \desc
	//# The $MaterialObject$ class holds all of the information pertaining to a paticular material used for shading.
	//# Individual material attributes are added to a material object using the $@MaterialObject::AddAttribute@$ function.
	//#
	//# One material object can be shared by many $@WorldMgr/Geometry@$ nodes in a world. A material object is assigned to
	//# a $@WorldMgr/Geometry@$ node by calling the $@WorldMgr/Geometry::SetMaterialObject@$ function. This increments the material object's
	//# reference count, so it is okay to release the material object afterwards. Since the $MaterialObject$ class is a
	//# shared object, it is released by calling the $@Utilities/Shared::Release@$ function.
	//
	//# \base	WorldMgr/Object		A $MaterialObject$ is an object that can be shared by multiple geometry nodes.
	//
	//# \also	$@Attribute@$
	//# \also	$@WorldMgr/Geometry@$


	//# \div
	//# \function	MaterialObject::GetMaterialSubstance	Returns the material substance type.
	//
	//# \proto	SubstanceType GetMaterialSubstance(void) const;
	//
	//# \desc
	//# The $GetMaterialSubstance$ function returns the material substance type. This information can be used
	//# directly, or it can be passed to the $@MaterialObject::FindRegisteredSubstance@$ function to obtain a
	//# registered $@Substance@$ object.
	//#
	//# The default substance type assigned to a material object is $kSubstanceNone$.
	//
	//# \also	$@MaterialObject::SetMaterialSubstance@$
	//# \also	$@MaterialObject::FindRegisteredSubstance@$


	//# \function	MaterialObject::SetMaterialSubstance	Sets the material substance type.
	//
	//# \proto	void SetMaterialSubstance(SubstanceType substance);
	//
	//# \param	substance	The new material substance type. Pass the constant $kSubstanceNone$ to indicate no substance.
	//
	//# \desc
	//# The $SetMaterialSubstance$ function sets the material substance type to the 32-bit identifier specified by the
	//# $substance$ parameter. The substance type typically corresponds to a substance that was previously registered
	//# with the engine using the $@MaterialObject::RegisterSubstance@$ function.
	//
	//# \also	$@MaterialObject::GetMaterialSubstance@$
	//# \also	$@MaterialObject::RegisterSubstance@$


	//# \function	MaterialObject::GetMaterialFlags		Returns the material flags.
	//
	//# \proto	unsigned_int32 GetMaterialFlags(void) const;
	//
	//# \desc
	//# The $GetMaterialFlags$ function returns the material flags, which can be a combination (through logical OR) of the
	//# following constants.
	//
	//# \table	MaterialFlags
	//
	//# \also	$@MaterialObject::SetMaterialFlags@$


	//# \function	MaterialObject::SetMaterialFlags		Sets the material flags.
	//
	//# \proto	void SetMaterialFlags(unsigned_int32 flags);
	//
	//# \param	flags	The new material flags.
	//
	//# \desc
	//# The $GetMaterialFlags$ function sets the material flags, which can be a combination (through logical OR) of the
	//# following constants.
	//
	//# \table	MaterialFlags
	//
	//# \also	$@MaterialObject::GetMaterialFlags@$


	//# \function	MaterialObject::Clone		Clones a material object.
	//
	//# \proto	MaterialObject *Clone(void) const;
	//
	//# \desc
	//# The $Clone$ function makes a copy of a material object and returns a pointer to the duplicate material object.
	//# In addition to the material properties, each attribute belonging to the original material object is cloned
	//# and added to the attribute list for the new material object.


	//# \div
	//# \function	MaterialObject::GetAttributeList		Returns a pointer to the material's attribute list.
	//
	//# \proto	List<Attribute> *GetAttributeList(void);
	//# \proto	const List<Attribute> *GetAttributeList(void) const;
	//
	//# \desc
	//# The $GetAttributeList$ function returns a pointer to the attribute list belonging to a material object.
	//
	//# \also	$@Utilities/List@$
	//# \also	$@Attribute@$
	//# \also	$@MaterialObject::GetFirstAttribute@$
	//# \also	$@MaterialObject::FindAttribute@$
	//# \also	$@MaterialObject::AddAttribute@$
	//# \also	$@MaterialObject::PurgeAttributes@$


	//# \function	MaterialObject::GetFirstAttribute		Returns a pointer to the first material attribute.
	//
	//# \proto	Attribute *GetFirstAttribute(void) const;
	//
	//# \desc
	//# The $GetFirstAttribute$ function returns a pointer to the first attribute belonging to a material object.
	//# If the material object contains no attributes, then the return value is $nullptr$.
	//# The $@Utilities/ListElement::Next@$ function can be used to iterate through all of the attributes
	//# belonging to a material object.
	//
	//# \also	$@Attribute@$
	//# \also	$@MaterialObject::GetAttributeList@$
	//# \also	$@MaterialObject::FindAttribute@$
	//# \also	$@MaterialObject::AddAttribute@$
	//# \also	$@MaterialObject::PurgeAttributes@$


	//# \function	MaterialObject::FindAttribute		Returns an attribute having a specific type.
	//
	//# \proto	Attribute *FindAttribute(AttributeType type, int32 index = 0) const;
	//
	//# \param	type	The type of attribute to find.
	//# \param	index	The index of the attribute of the given type to return.
	//
	//# \desc
	//# The $FindAttribute$ function searches a material object's attribute list for attributes having the type
	//# specified by the $type$ parameter and returns the <i>n</i>-th instance of any such attribute, where
	//# <i>n</i> is the zero-based index specified by the $index$ parameter. (Specifying 0 for the $index$
	//# parameter causes the first matching attribute to be returned.) If no such attribute exists in the
	//# material object, then the return value is $nullptr$.
	//
	//# \also	$@Attribute@$
	//# \also	$@MaterialObject::GetAttributeList@$
	//# \also	$@MaterialObject::GetFirstAttribute@$


	//# \function	MaterialObject::AddAttribute		Adds an attribute to a material.
	//
	//# \proto	void AddAttribute(Attribute *attribute);
	//
	//# \param	attribute	A pointer to the attribute to add.
	//
	//# \desc
	//# The $AddAttribute$ function adds the attribute specified by the $attribute$ parameter to a material object.
	//
	//# \also	$@Attribute@$
	//# \also	$@MaterialObject::GetAttributeList@$
	//# \also	$@MaterialObject::GetFirstAttribute@$
	//# \also	$@MaterialObject::PurgeAttributes@$


	//# \function	MaterialObject::PurgeAttributes		Purges all of a material's attributes.
	//
	//# \proto	void PurgeAttributes(void);
	//
	//# \desc
	//# The $PurgeAttributes$ function deletes all of the attributes belonging to a material object.
	//
	//# \also	$@Attribute@$
	//# \also	$@MaterialObject::GetAttributeList@$
	//# \also	$@MaterialObject::GetFirstAttribute@$
	//# \also	$@MaterialObject::AddAttribute@$


	//# \div
	//# \function	MaterialObject::GetTextureBlendMode		Returns the texture blend mode.
	//
	//# \proto	TextureBlendMode GetTextureBlendMode(void) const;
	//
	//# \desc
	//# The $GetTextureBlendMode$ function returns the mode used when blending two diffuse texture maps
	//# in a material. The blend mode can be one of the following constants.
	//
	//# \table	TextureBlendMode
	//
	//# \also	$@MaterialObject::SetTextureBlendMode@$


	//# \function	MaterialObject::SetTextureBlendMode		Sets the texture blend mode.
	//
	//# \proto	void SetTextureBlendMode(TextureBlendMode mode);
	//
	//# \param	mode	The new texture blend mode.
	//
	//# \desc
	//# The $GetTextureBlendMode$ function sets the mode used when blending two diffuse texture maps
	//# in a material. The blend mode can be one of the following constants.
	//
	//# \table	TextureBlendMode
	//
	//# \also	$@MaterialObject::GetTextureBlendMode@$


	//# \function	MaterialObject::GetTexcoordScale		Returns the scale for a texcoord.
	//
	//# \proto	const Vector2D& GetTexcoordScale(int32 index) const;
	//
	//# \param	index	The texcoord index. This must be 0 or 1.
	//
	//# \desc
	//# The $GetTexcoordScale$ function returns the scale that is applied to the texcoord specified by
	//# the $index$ parameter. The <i>x</i> coordinate of the returned vector multiplies the <i>s</i>
	//# texture coordinate, and the <i>y</i> coordinate of the returned vector multiplies the <i>t</i>
	//# texture coordinate. The default scale is (1,1).
	//
	//# \also	$@MaterialObject::SetTexcoordScale@$
	//# \also	$@MaterialObject::GetTexcoordOffset@$
	//# \also	$@MaterialObject::SetTexcoordOffset@$


	//# \function	MaterialObject::SetTexcoordScale		Sets the scale for a texcoord.
	//
	//# \proto	void SetTexcoordScale(int32 index, const Vector2D& scale);
	//
	//# \param	index	The texcoord index. This must be 0 or 1.
	//# \param	scale	The new texcoord scale.
	//
	//# \desc
	//# The $SetTexcoordScale$ function sets the scale that is applied to the texcoord specified by
	//# the $index$ parameter. The <i>x</i> coordinate of the $scale$ parameter multiplies the <i>s</i>
	//# texture coordinate, and the <i>y</i> coordinate of the $scale$ parameter multiplies the <i>t</i>
	//# texture coordinate. The default scale is (1,1).
	//
	//# \also	$@MaterialObject::GetTexcoordScale@$
	//# \also	$@MaterialObject::GetTexcoordOffset@$
	//# \also	$@MaterialObject::SetTexcoordOffset@$


	//# \function	MaterialObject::GetTexcoordOffset		Returns the offset for a texcoord.
	//
	//# \proto	const Vector2D& GetTexcoordOffset(int32 index) const;
	//
	//# \param	index	The texcoord index. This must be 0 or 1.
	//
	//# \desc
	//# The $GetTexcoordOffset$ function returns the offset that is applied to the texcoord specified by
	//# the $index$ parameter. The <i>x</i> coordinate of the returned vector is added to the <i>s</i>
	//# texture coordinate, and the <i>y</i> coordinate of the returned vector is added to the <i>t</i>
	//# texture coordinate. The default offset is (0,0).
	//
	//# \also	$@MaterialObject::SetTexcoordOffset@$
	//# \also	$@MaterialObject::GetTexcoordScale@$
	//# \also	$@MaterialObject::SetTexcoordScale@$


	//# \function	MaterialObject::SetTexcoordOffset		Sets the offset for a texcoord.
	//
	//# \proto	void SetTexcoordOffset(int32 index, const Vector2D& offset);
	//
	//# \param	index	The texcoord index. This must be 0 or 1.
	//# \param	offset	The new texcoord offset.
	//
	//# \desc
	//# The $SetTexcoordOffset$ function sets the offset that is applied to the texcoord specified by
	//# the $index$ parameter. The <i>x</i> coordinate of the $offset$ parameter is added to the <i>s</i>
	//# texture coordinate, and the <i>y</i> coordinate of the $offset$ parameter is added to the <i>t</i>
	//# texture coordinate. The default offset is (0,0).
	//
	//# \also	$@MaterialObject::GetTexcoordOffset@$
	//# \also	$@MaterialObject::GetTexcoordScale@$
	//# \also	$@MaterialObject::SetTexcoordScale@$


	//# \function	MaterialObject::GetTexcoordVelocity		Returns the animation velocity for a texcoord.
	//
	//# \proto	const Vector2D& GetTexcoordVelocity(int32 index) const;
	//
	//# \param	index	The texcoord index. This must be 0 or 1.
	//
	//# \desc
	//# The $GetTexcoordVelocity$ function returns the animation velocity that is applied to the texcoord specified by
	//# the $index$ parameter. The <i>x</i> coordinate of the returned vector is the velocity of the <i>s</i>
	//# texture coordinate, and the <i>y</i> coordinate of the returned vector is the velocity of the <i>t</i>
	//# texture coordinate. The texcoord velocity is multiplied by time in milliseconds to determine the texcoord
	//# offset. The default velocity is (0,0).
	//
	//# \also	$@MaterialObject::SetTexcoordVelocity@$


	//# \function	MaterialObject::SetTexcoordVelocity		Sets the animation velocity for a texcoord.
	//
	//# \proto	void SetTexcoordVelocity(int32 index, const Vector2D& velocity);
	//
	//# \param	index		The texcoord index. This must be 0 or 1.
	//# \param	velocity	The new texcoord animation velocity, in inverse milliseconds.
	//
	//# \desc
	//# The $SetTexcoordVelocity$ function sets the animation velocity that is applied to the texcoord specified by
	//# the $index$ parameter. The <i>x</i> coordinate of the $velocity$ parameter is the velocity of the <i>s</i>
	//# texture coordinate, and the <i>y</i> coordinate of the $velocity$ parameter is the velocity of the <i>t</i>
	//# texture coordinate. The texcoord velocity is multiplied by time in milliseconds to determine the texcoord
	//# offset. The default velocity is (0,0).
	//#
	//# In order for animation to be applied to a particular texcoord, the corresponding material flag must be set.
	//# Set the $@MaterialObject::SetMaterialFlags@$ function.
	//
	//# \special
	//# Each component of the texcoord animation velocity must be an integer when multiplied by 120000 milliseconds
	//# (two minutes). The internal timers used by the engine for texcoord animation have a two minute period, and
	//# a velocity that does not cause a texture to wrap an integer number of times during this period will cause a
	//# visible jump to occur every two minutes.
	//
	//# \also	$@MaterialObject::GetTexcoordVelocity@$
	//# \also	$@MaterialObject::SetMaterialFlags@$


	//# \div
	//# \function	MaterialObject::GetFirstRegisteredSubstance		Returns the first registered substance.
	//
	//# \proto	static const Substance *GetFirstRegisteredSubstance(void);
	//
	//# \desc
	//# The $GetFirstRegisteredSubstance$ function returns a pointer to the $@Substance@$ object corresponding
	//# to the first registered substance type. The entire list of registered substances can be iterated by calling
	//# the $@Utilities/MapElement::Next@$ function on the returned object and continuing until $nullptr$ is returned.
	//
	//# \also	$@MaterialObject::FindRegisteredSubstance@$
	//# \also	$@MaterialObject::RegisterSubstance@$
	//# \also	$@Substance@$


	//# \function	MaterialObject::FindRegisteredSubstance		Returns a specific registered substance.
	//
	//# \proto	static const Substance *FindRegisteredSubstance(SubstanceType type);
	//
	//# \param	type	The type of the substance.
	//
	//# \desc
	//# The $FindRegisteredSubstance$ function returns a pointer to the registered $@Substance@$ object corresponding
	//# to the substance type specified by the $type$ parameter. If no such registration exists, then the return value is $nullptr$.
	//
	//# \also	$@MaterialObject::GetFirstRegisteredSubstance@$
	//# \also	$@MaterialObject::RegisterSubstance@$
	//# \also	$@Substance@$


	//# \function	MaterialObject::RegisterSubstance		Registers a substance with the engine.
	//
	//# \proto	static void RegisterSubstance(Substance *substance);
	//
	//# \param	substance	The substance object to register.
	//
	//# \desc
	//# The $RegisterSubstance$ function registers the $@Substance@$ object specified by the $substance$ parameter with
	//# the engine. The substance type represetned by the $@Substance@$ object must be unique among all previously registered
	//# substances, or else the substance is not registered.
	//
	//# \also	$@MaterialObject::GetFirstRegisteredSubstance@$
	//# \also	$@MaterialObject::FindRegisteredSubstance@$
	//# \also	$@Substance@$


	class MaterialObject : public Object, public MapElement<MaterialObject>
	{
		friend class MaterialRegistration;
		friend class WorldMgr;

		public:

			typedef MaterialType KeyType;

		private:

			MaterialType			materialType;
			SubstanceType			materialSubstance;
			unsigned_int32			materialFlags;

			List<Attribute>			attributeList;

			TextureBlendMode		textureBlendMode;
			Vector4D				texcoordGeneration;

			Vector2D				texcoordScale[kMaxMaterialTexcoordCount];
			Vector2D				texcoordOffset[kMaxMaterialTexcoordCount];
			Vector2D				texcoordVelocity[kMaxMaterialTexcoordCount];

			static C4API Map<Substance>				substanceMap;
			static C4API Map<MaterialRegistration>	registrationMap;
			static Map<MaterialObject>				materialMap;

			~MaterialObject();

		public:

			C4API MaterialObject();
			C4API MaterialObject(const MaterialObject& materialObject);

			KeyType GetKey(void) const
			{
				return (materialType);
			}

			MaterialType GetMaterialType(void) const
			{
				return (materialType);
			}

			SubstanceType GetMaterialSubstance(void) const
			{
				return (materialSubstance);
			}

			void SetMaterialSubstance(SubstanceType substance)
			{
				materialSubstance = substance;
			}

			unsigned_int32 GetMaterialFlags(void) const
			{
				return (materialFlags);
			}

			void SetMaterialFlags(unsigned_int32 flags)
			{
				materialFlags = flags;
			}

			List<Attribute> *GetAttributeList(void)
			{
				return (&attributeList);
			}

			const List<Attribute> *GetAttributeList(void) const
			{
				return (&attributeList);
			}

			Attribute *GetFirstAttribute(void) const
			{
				return (attributeList.First());
			}

			void AddAttribute(Attribute *attribute)
			{
				attributeList.Append(attribute);
			}

			void PurgeAttributes(void)
			{
				attributeList.Purge();
			}

			TextureBlendMode GetTextureBlendMode(void) const
			{
				return (textureBlendMode);
			}

			void SetTextureBlendMode(TextureBlendMode mode)
			{
				textureBlendMode = mode;
			}

			const Vector4D& GetTexcoordGeneration(void) const
			{
				return (texcoordGeneration);
			}

			void SetTexcoordGeneration(const Vector4D& generation)
			{
				texcoordGeneration = generation;
			}

			const Vector2D& GetTexcoordScale(int32 index) const
			{
				return (texcoordScale[index]);
			}

			void SetTexcoordScale(int32 index, const Vector2D& scale)
			{
				texcoordScale[index] = scale;
			}

			const Vector2D& GetTexcoordOffset(int32 index) const
			{
				return (texcoordOffset[index]);
			}

			void SetTexcoordOffset(int32 index, const Vector2D& offset)
			{
				texcoordOffset[index] = offset;
			}

			const Vector2D& GetTexcoordVelocity(int32 index) const
			{
				return (texcoordVelocity[index]);
			}

			void SetTexcoordVelocity(int32 index, const Vector2D& velocity)
			{
				texcoordVelocity[index] = velocity;
			}

			static const Substance *GetFirstRegisteredSubstance(void)
			{
				return (substanceMap.First());
			}

			static const Substance *FindRegisteredSubstance(SubstanceType type)
			{
				return (substanceMap.Find(type));
			}

			static void RegisterSubstance(Substance *substance)
			{
				substanceMap.Insert(substance);
			}

			static const MaterialRegistration *GetFirstRegisteredMaterial(void)
			{
				return (registrationMap.First());
			}

			static const MaterialRegistration *FindMaterialRegistration(MaterialType type)
			{
				return (registrationMap.Find(type));
			}

			C4API MaterialObject *Clone(void) const;

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;

			C4API static MaterialObject *Get(MaterialType type);
			C4API static MaterialObject *Get(const char *name);
			static void ReleaseCache(void);

			C4API bool operator ==(const MaterialObject& object) const;

			C4API bool ShaderMaterial(void) const;
			C4API Attribute *FindAttribute(AttributeType type, int32 index = 0) const;
	};


	class AutoReleaseMaterial : public AutoRelease<MaterialObject>
	{
		public:

			AutoReleaseMaterial(MaterialType type) : AutoRelease<MaterialObject>(MaterialObject::Get(type))
			{
			}
	};
}


#endif

// ZYUQURM
