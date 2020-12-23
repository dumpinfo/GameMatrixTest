 

#ifndef C4Attributes_h
#define C4Attributes_h


//# \component	Graphics Manager
//# \prefix		GraphicsMgr/


#include "C4Textures.h"


namespace C4
{
	//# \tree	Attribute
	//
	//# \node	DiffuseAttribute
	//# \node	SpecularAttribute
	//# \node	EmissionAttribute
	//# \node	EnvironmentAttribute
	//# \node	ReflectionAttribute
	//# \node	RefractionAttribute
	//# \node	MicrofacetAttribute
	//# \node	TextureAttribute
	//# \sub
	//#		\node	DiffuseTextureAttribute
	//#		\node	SpecularTextureAttribute
	//#		\node	EmissionTextureAttribute
	//#		\node	NormalTextureAttribute
	//#		\node	HorizonTextureAttribute
	//#		\node	OpacityTextureAttribute
	//#		\node	EnvironmentTextureAttribute
	//# \end
	//# \node	ShaderAttribute


	typedef Type	AttributeType;


	//# \enum	AttributeType

	enum : AttributeType
	{
		kAttributeReference				= 'REFR',
		kAttributeDiffuse				= 'DIFF',		//## Diffuse reflection color.
		kAttributeSpecular				= 'SPEC',		//## Specular reflection color and power.
		kAttributeEmission				= 'EMIS',		//## Emission color.
		kAttributeEnvironment			= 'ENVR',		//## Environment reflection color.
		kAttributeReflection			= 'RFLC',		//## Reflection buffer parameters.
		kAttributeRefraction			= 'RFRC',		//## Refraction buffer parameters.
		kAttributeMicrofacet			= 'MFCT',		//## Microfacet reflection parameters.
		kAttributeDiffuseTexture		= 'TDIF',		//## Diffuse color texture.
		kAttributeSpecularTexture		= 'TSPC',		//## Specular color texture.
		kAttributeEmissionTexture		= 'TEMS',		//## Emission texture.
		kAttributeNormalTexture			= 'TNRM',		//## Normal texture.
		kAttributeHorizonTexture		= 'HRZN',		//## Horizon texture.
		kAttributeOpacityTexture		= 'OPAC',		//## Opacity texture.
		kAttributeEnvironmentTexture	= 'TENV',		//## Environment texture override.
		kAttributeFireTexture			= 'FIRE',
		kAttributeDeltaDepth			= 'DLTA',
		kAttributeDepthRamp				= 'DRMP',
		kAttributeShader				= 'SHDR'		//## Shader graph.
	};


	//# \enum	AttributeFlags

	enum
	{
		kAttributeMutable				= 1 << 0		//## The constant data in the attribute, such as a color, is mutable. This means that the data is loaded as a shader parameter instead of being inlined as a literal constant.
	};


	//# \class	Attribute	The base class for all material attributes.
	//
	//# The $Attribute$ class is the base class for all material attributes.
	//
	//# \def	class Attribute : public ListElement<Attribute>, public Packable
	//
	//# \ctor	Attribute(AttributeType type, unsigned_int32 flags = 0);
	//
	//# The constructor has protected access. The $Attribute$ class can only exist as the base class for another class.
	//
	//# \desc
	//# The $Attribute$ class is the base class for all material attributes. A list of material attributes is supplied to
	//# the $@RenderSegment@$ class by either attaching a material with the $@RenderSegment::SetMaterialObjectPointer@$ function
	//# or by setting an auxiliary attribute list using the $@RenderSegment::SetMaterialAttributeList@$ function. These
	//# material attributes describe to the Graphics Manager how a renderable object should be shaded.
	//#
	//# A $@WorldMgr/Geometry@$ node can have a $@MaterialObject@$ object attached to it that holds a list of
	//# material attributes. In this case, the list of material attributes is automatically applied to the $@Renderable@$
	//# base class of the $@WorldMgr/Geometry@$ node when it is preprocessed.
	//
	//# \base	Utilities/ListElement<Attribute>	Attributes are stored in a list by the $@MaterialObject@$ class.
	//#												The $@Renderable@$ class also uses lists of attributes.
	//# \base	ResourceMgr/Packable				Attributes can be packed for storage in resources.
	//
	//# \also	$@Renderable@$
	//# \also	$@MaterialObject@$
	//# \also	$@WorldMgr/Geometry@$

 
	//# \function	Attribute::GetAttributeType		Returns the type of an attribute.
	//
	//# \proto	AttributeType GetAttributeType(void) const; 
	//
	//# \desc 
	//# The $GetAttributeType$ function returns the type of an attribute, which can be one of the following constants.
	//
	//# \table	AttributeType 

 
	//# \function	Attribute::GetAttributeFlags	Returns the attribute flags. 
	//
	//# \proto	unsigned_int32 GetAttributeFlags(void) const;
	//
	//# \desc 
	//# The $GetAttributeFlags$ function returns the attribute flags, which can be a combination (through logical OR) of the following constants.
	//
	//# \table	AttributeFlags
	//
	//# \also	$@Attribute::SetAttributeFlags@$


	//# \function	Attribute::SetAttributeFlags	Sets the attribute flags.
	//
	//# \proto	void SetAttributeFlags(unsigned_int32 flags);
	//
	//# \param	flags	The new attribute flags.
	//
	//# \desc
	//# The $SetAttributeFlags$ function sets the attribute flags to those specified by the $flags$ parameter. The flags can be a combination
	//# (through logical OR) of the following constants.
	//
	//# \table	AttributeFlags
	//
	//# The initial value of the flags is set when the attribute is constructed, and it is set to zero by default if omitted.
	//
	//# \also	$@Attribute::GetAttributeFlags@$


	class Attribute : public ListElement<Attribute>, public Packable
	{
		friend class MaterialObject;

		private:

			AttributeType	attributeType;
			unsigned_int32	attributeFlags;

			virtual Attribute *Replicate(void) const = 0;

			static Attribute *Create(Unpacker& data, unsigned_int32 unpackFlags);

		protected:

			Attribute(AttributeType type, unsigned_int32 flags = 0);
			Attribute(const Attribute& attribute);

		public:

			virtual ~Attribute();

			AttributeType GetAttributeType(void) const
			{
				return (attributeType);
			}

			unsigned_int32 GetAttributeFlags(void) const
			{
				return (attributeFlags);
			}

			void SetAttributeFlags(unsigned_int32 flags)
			{
				attributeFlags = flags;
			}

			Attribute *Clone(void) const
			{
				return (Replicate());
			}

			void PackType(Packer& data) const override;
			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			virtual bool operator ==(const Attribute& attribute) const;

			virtual void SetAttributeColor(const ColorRGBA& color);
	};


	class ReferenceAttribute final : public Attribute
	{
		private:

			const Attribute		*attributeReference;

			ReferenceAttribute(const ReferenceAttribute& referenceAttribute);

			Attribute *Replicate(void) const override;

		public:

			C4API ReferenceAttribute();
			C4API explicit ReferenceAttribute(const Attribute *attribute);
			C4API ~ReferenceAttribute();

			const Attribute *GetReference(void) const
			{
				return (attributeReference);
			}

			void SetReference(const Attribute *attribute)
			{
				attributeReference = attribute;
			}

			bool operator ==(const Attribute& attribute) const override;
	};


	//# \class	DiffuseAttribute		Material attribute for a diffuse color.
	//
	//# The $DiffuseAttribute$ class represents the material attribute for a diffuse color.
	//
	//# \def	class DiffuseAttribute final : public Attribute
	//
	//# \ctor	explicit DiffuseAttribute(const ColorRGBA& color, unsigned_int32 flags = 0);
	//
	//# \param	color	The initial diffuse color.
	//# \param	flags	The initial attribute flags. See the $@Attribute::SetAttributeFlags@$ function.
	//
	//# \desc
	//# The $DiffuseAttribute$ class represents the material attribute for a diffuse color.
	//# The diffuse color is applied during both the ambient rendering pass and each lighting pass.
	//
	//# \base	Attribute	A $DiffuseAttribute$ is a specific type of attribute.
	//
	//# \also	$@Math/ColorRGBA@$


	//# \function	DiffuseAttribute::GetDiffuseColor		Returns the diffuse color.
	//
	//# \proto	const ColorRGBA& GetDiffuseColor(void) const;
	//
	//# \desc
	//# The $GetDiffuseColor$ function returns the diffuse color stored in a diffuse material attribute.
	//
	//# \also	$@DiffuseAttribute::SetDiffuseColor@$
	//# \also	$@DiffuseAttribute::SetDiffuseAlpha@$


	//# \function	DiffuseAttribute::SetDiffuseColor		Sets the diffuse color.
	//
	//# \proto	void SetDiffuseColor(const ColorRGBA& color);
	//
	//# \param	color	The new diffuse color.
	//
	//# \desc
	//# The $SetDiffuseColor$ attribute sets the diffuse color stored in a diffuse material attribute to the value given by the $color$ parameter.
	//# A renderable object to which the affected attribute applies will subsequently be rendered using the new color.
	//
	//# \also	$@DiffuseAttribute::SetDiffuseAlpha@$
	//# \also	$@DiffuseAttribute::GetDiffuseColor@$


	//# \function	DiffuseAttribute::SetDiffuseAlpha		Sets the alpha component of the diffuse color.
	//
	//# \proto	void SetDiffuseAlpha(float alpha);
	//
	//# \param	alpha	The new diffuse alpha component.
	//
	//# \desc
	//# The $SetDiffuseAlpha$ attribute sets the alpha component of the diffuse color stored in a diffuse material attribute to the value given by the $alpha$ parameter.
	//# The red, green, and blue components are not changed.
	//# A renderable object to which the affected attribute applies will subsequently be rendered using the new color.
	//
	//# \also	$@DiffuseAttribute::SetDiffuseColor@$
	//# \also	$@DiffuseAttribute::GetDiffuseColor@$


	class DiffuseAttribute final : public Attribute
	{
		private:

			ColorRGBA		diffuseColor;

			Attribute *Replicate(void) const override;

		public:

			C4API explicit DiffuseAttribute(unsigned_int32 flags = 0);
			C4API explicit DiffuseAttribute(const ColorRGBA& color, unsigned_int32 flags = 0);
			C4API DiffuseAttribute(const DiffuseAttribute& diffuseAttribute);
			C4API ~DiffuseAttribute();

			const ColorRGBA& GetDiffuseColor(void) const
			{
				return (diffuseColor);
			}

			void SetDiffuseColor(const ColorRGBA& color)
			{
				diffuseColor = color;
			}

			void SetDiffuseAlpha(float alpha)
			{
				diffuseColor.alpha = alpha;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			bool operator ==(const Attribute& attribute) const override;

			void SetAttributeColor(const ColorRGBA& color) override;
	};


	//# \class	SpecularAttribute		Material attribute for a specular color.
	//
	//# The $SpecularAttribute$ class represents the material attribute for a specular color.
	//
	//# \def	class SpecularAttribute final : public Attribute
	//
	//# \ctor	SpecularAttribute(const ColorRGBA& color, float exponent, unsigned_int32 flags = 0);
	//
	//# \param	color		The initial specular color.
	//# \param	exponent	The initial specular exponent.
	//# \param	flags		The initial attribute flags. See the $@Attribute::SetAttributeFlags@$ function.
	//
	//# \desc
	//# The $SpecularAttribute$ class represents the material attribute for a specular color.
	//# The specular color each lighting pass, but does not contribute to the ambient pass.
	//# The presence of a specular material attribute determines whether a specular term is
	//# calculated when an object is rendered.
	//
	//# \base	Attribute	A $SpecularAttribute$ is a specific type of attribute.
	//
	//# \also	$@Math/ColorRGBA@$


	//# \function	SpecularAttribute::GetSpecularColor		Returns the specular color.
	//
	//# \proto	const ColorRGBA& GetSpecularColor(void) const;
	//
	//# \desc
	//# The $GetSpecularColor$ function returns the specular color stored in a specular material attribute.
	//
	//# \also	$@SpecularAttribute::SetSpecularColor@$
	//# \also	$@SpecularAttribute::GetSpecularExponent@$
	//# \also	$@SpecularAttribute::SetSpecularExponent@$


	//# \function	SpecularAttribute::SetSpecularColor		Sets the specular color.
	//
	//# \proto	void SetSpecularColor(const ColorRGBA& color);
	//
	//# \param	color	The new specular color. The alpha component is ignored.
	//
	//# \desc
	//# The $SetSpecularColor$ attribute sets the specular color stored in a specular material attribute to the value given by the $color$ parameter.
	//# A renderable object to which the affected attribute applies will subsequently be rendered using the new color.
	//# The alpha component of the specular color does not participate in shading calculations.
	//
	//# \also	$@SpecularAttribute::GetSpecularColor@$
	//# \also	$@SpecularAttribute::GetSpecularExponent@$
	//# \also	$@SpecularAttribute::SetSpecularExponent@$


	//# \function	SpecularAttribute::GetSpecularExponent		Returns the specular exponent.
	//
	//# \proto	const float& GetSpecularExponent(void) const;
	//
	//# \desc
	//# The $GetSpecularExponent$ function returns the specular exponent stored in a specular material attribute.
	//
	//# \also	$@SpecularAttribute::SetSpecularExponent@$
	//# \also	$@SpecularAttribute::GetSpecularColor@$
	//# \also	$@SpecularAttribute::SetSpecularColor@$


	//# \function	SpecularAttribute::SetSpecularExponent		Sets the specular exponent.
	//
	//# \proto	void SetSpecularExponent(float exponent);
	//
	//# \param	exponent	The new specular exponent.
	//
	//# \desc
	//# The $SetSpecularExponent$ function sets the specular exponent stored in a specular material attribute to the value given by the $exponent$ parameter.
	//# A renderable object to which the affected attribute applies will <i>not</i> automatically be rendered using the new exponent.
	//# The change will only take effect once the $@Renderable::InvalidateShaderData@$ function has been called for the affected object.


	class SpecularAttribute final : public Attribute
	{
		private:

			ColorRGBA		specularColor;
			float			specularExponent;

			Attribute *Replicate(void) const override;

		public:

			C4API explicit SpecularAttribute(unsigned_int32 flags = 0);
			C4API SpecularAttribute(const ColorRGBA& color, float exponent, unsigned_int32 flags = 0);
			C4API SpecularAttribute(const SpecularAttribute& specularAttribute);
			C4API ~SpecularAttribute();

			const ColorRGBA& GetSpecularColor(void) const
			{
				return (specularColor);
			}

			void SetSpecularColor(const ColorRGBA& color)
			{
				specularColor = color;
			}

			const float& GetSpecularExponent(void) const
			{
				return (specularExponent);
			}

			void SetSpecularExponent(float exponent)
			{
				specularExponent = exponent;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			bool operator ==(const Attribute& attribute) const override;

			void SetAttributeColor(const ColorRGBA& color) override;
	};


	//# \class	EmissionAttribute		Material attribute for an emission color.
	//
	//# The $EmissionAttribute$ class represents the material attribute for an emission color.
	//
	//# \def	class EmissionAttribute final : public Attribute
	//
	//# \ctor	explicit EmissionAttribute(const ColorRGBA& color, unsigned_int32 flags = 0);
	//
	//# \param	color	The initial emission color.
	//# \param	flags	The initial attribute flags. See the $@Attribute::SetAttributeFlags@$ function.
	//
	//# \desc
	//# The $EmissionAttribute$ class represents the material attribute for an emission color.
	//# The emission color is applied during the ambient rendering pass only.
	//
	//# \base	Attribute	An $EmissionAttribute$ is a specific type of attribute.
	//
	//# \also	$@Math/ColorRGBA@$


	//# \function	EmissionAttribute::GetEmissionColor		Returns the emission color.
	//
	//# \proto	const ColorRGBA& GetEmissionColor(void) const;
	//
	//# \desc
	//# The $GetEmissionColor$ function returns the emission color stored in an emission material attribute.
	//
	//# \also	$@EmissionAttribute::SetEmissionColor@$


	//# \function	EmissionAttribute::SetEmissionColor		Sets the emission color.
	//
	//# \proto	void SetEmissionColor(const ColorRGBA& color);
	//
	//# \param	color	The new emission color.
	//
	//# \desc
	//# The $SetEmissionColor$ attribute sets the emission color stored in an emission material attribute to the value given by the $color$ parameter.
	//# A renderable object to which the affected attribute applies will subsequently be rendered using the new color.
	//
	//# \also	$@EmissionAttribute::GetEmissionColor@$


	class EmissionAttribute final : public Attribute
	{
		private:

			ColorRGBA		emissionColor;

			Attribute *Replicate(void) const override;

		public:

			C4API explicit EmissionAttribute(unsigned_int32 flags = 0);
			C4API explicit EmissionAttribute(const ColorRGBA& color, unsigned_int32 flags = 0);
			C4API EmissionAttribute(const EmissionAttribute& emissionAttribute);
			C4API ~EmissionAttribute();

			const ColorRGBA& GetEmissionColor(void) const
			{
				return (emissionColor);
			}

			void SetEmissionColor(const ColorRGBA& color)
			{
				emissionColor = color;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			bool operator ==(const Attribute& attribute) const override;

			void SetAttributeColor(const ColorRGBA& color) override;
	};


	//# \class	EnvironmentAttribute		Material attribute for an environment color.
	//
	//# The $EnvironmentAttribute$ class represents the material attribute for an environment color.
	//
	//# \def	class EnvironmentAttribute final : public Attribute
	//
	//# \ctor	explicit EnvironmentAttribute(const ColorRGBA& color, unsigned_int32 flags = 0);
	//
	//# \param	color	The initial environment color.
	//# \param	flags	The initial attribute flags. See the $@Attribute::SetAttributeFlags@$ function.
	//
	//# \desc
	//# The $EnvironmentAttribute$ class represents the material attribute for an environment color.
	//# The environment color is applied during the ambient rendering pass only.
	//
	//# \base	Attribute	An $EnvironmentAttribute$ is a specific type of attribute.
	//
	//# \also	$@Math/ColorRGBA@$


	//# \function	EnvironmentAttribute::GetEnvironmentColor		Returns the environment color.
	//
	//# \proto	const ColorRGBA& GetEnvironmentColor(void) const;
	//
	//# \desc
	//# The $GetEnvironmentColor$ function returns the environment color stored in an environment material attribute.
	//
	//# \also	$@EnvironmentAttribute::SetEnvironmentColor@$


	//# \function	EnvironmentAttribute::SetEnvironmentColor		Sets the environment color.
	//
	//# \proto	void SetEnvironmentColor(const ColorRGBA& color);
	//
	//# \param	color	The new environment color.
	//
	//# \desc
	//# The $SetEnvironmentColor$ attribute sets the environment color stored in an environment material attribute to the value given by the $color$ parameter.
	//# A renderable object to which the affected attribute applies will subsequently be rendered using the new color.
	//
	//# \also	$@EnvironmentAttribute::GetEnvironmentColor@$


	class EnvironmentAttribute final : public Attribute
	{
		private:

			ColorRGBA		environmentColor;

			Attribute *Replicate(void) const override;

		public:

			C4API explicit EnvironmentAttribute(unsigned_int32 flags = 0);
			C4API explicit EnvironmentAttribute(const ColorRGBA& color, unsigned_int32 flags = 0);
			C4API EnvironmentAttribute(const EnvironmentAttribute& environmentAttribute);
			C4API ~EnvironmentAttribute();

			const ColorRGBA& GetEnvironmentColor(void) const
			{
				return (environmentColor);
			}

			void SetEnvironmentColor(const ColorRGBA& color)
			{
				environmentColor = color;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			bool operator ==(const Attribute& attribute) const override;

			void SetAttributeColor(const ColorRGBA& color) override;
	};


	//# \class	ReflectionAttribute		Material attribute for a reflection color.
	//
	//# The $ReflectionAttribute$ class represents the material attribute for a reflection color.
	//
	//# \def	class ReflectionAttribute final : public Attribute
	//
	//# \ctor	ReflectionAttribute(const ColorRGBA& color, float factor, float scale, unsigned_int32 flags = 0);
	//
	//# \param	color		The initial reflection color.
	//# \param	factor		The normal incidence reflectivity factor.
	//# \param	scale		The warp offset scale.
	//# \param	flags		The initial attribute flags. See the $@Attribute::SetAttributeFlags@$ function.
	//
	//# \desc
	//#
	//
	//# \base	Attribute	A $ReflectionAttribute$ is a specific type of attribute.
	//
	//# \also	$@Math/ColorRGBA@$


	//# \function	ReflectionAttribute::GetReflectionColor		Sets the reflection color.
	//
	//# \proto	const ColorRGBA& GetReflectionColor(void) const;
	//
	//# \desc
	//# The $GetReflectionColor$ function returns the reflection color stored in a reflection material attribute.
	//
	//# \also	$@ReflectionAttribute::SetReflectionColor@$


	//# \function	ReflectionAttribute::SetReflectionColor		Sets the reflection color.
	//
	//# \proto	void SetReflectionColor(const ColorRGBA& color);
	//
	//# \param	color	The new reflection color.
	//
	//# \desc
	//# The $SetReflectionColor$ attribute sets the reflection color stored in a reflection material attribute to the value given by the $color$ parameter.
	//# A renderable object to which the affected attribute applies will subsequently be rendered using the new color.
	//
	//# \also	$@ReflectionAttribute::GetReflectionColor@$


	class ReflectionAttribute final : public Attribute
	{
		public:

			struct ReflectionParams
			{
				float		normalIncidenceReflectivity;
				float		reflectionOffsetScale;
			};

		private:

			ColorRGBA			reflectionColor;
			ReflectionParams	reflectionParams;

			Attribute *Replicate(void) const override;

		public:

			C4API explicit ReflectionAttribute(unsigned_int32 flags = 0);
			C4API ReflectionAttribute(const ColorRGBA& color, float factor, float scale, unsigned_int32 flags = 0);
			C4API ReflectionAttribute(const ReflectionAttribute& reflectionAttribute);
			C4API ~ReflectionAttribute();

			const ColorRGBA& GetReflectionColor(void) const
			{
				return (reflectionColor);
			}

			void SetReflectionColor(const ColorRGBA& color)
			{
				reflectionColor = color;
			}

			const ReflectionParams *GetReflectionParams(void) const
			{
				return (&reflectionParams);
			}

			void SetNormalIncidenceReflectivity(float normalReflect)
			{
				reflectionParams.normalIncidenceReflectivity = normalReflect;
			}

			void SetReflectionOffsetScale(float scale)
			{
				reflectionParams.reflectionOffsetScale = scale;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			bool operator ==(const Attribute& attribute) const override;

			void SetAttributeColor(const ColorRGBA& color) override;
	};


	//# \class	RefractionAttribute		Material attribute for a refraction color.
	//
	//# The $RefractionAttribute$ class represents the material attribute for a refraction color.
	//
	//# \def	class RefractionAttribute final : public Attribute
	//
	//# \ctor	RefractionAttribute(const ColorRGBA& color, float scale, unsigned_int32 flags = 0);
	//
	//# \param	color		The initial refraction color.
	//# \param	scale		The warp offset scale.
	//# \param	flags		The initial attribute flags. See the $@Attribute::SetAttributeFlags@$ function.
	//
	//# \desc
	//#
	//
	//# \base	Attribute	A $RefractionAttribute$ is a specific type of attribute.
	//
	//# \also	$@Math/ColorRGBA@$


	//# \function	RefractionAttribute::GetRefractionColor		Sets the refraction color.
	//
	//# \proto	const ColorRGBA& GetRefractionColor(void) const;
	//
	//# \desc
	//# The $GetRefractionColor$ function returns the refraction color stored in a refraction material attribute.
	//
	//# \also	$@RefractionAttribute::SetRefractionColor@$


	//# \function	RefractionAttribute::SetRefractionColor		Sets the refraction color.
	//
	//# \proto	void SetRefractionColor(const ColorRGBA& color);
	//
	//# \param	color	The new refraction color.
	//
	//# \desc
	//# The $SetRefractionColor$ attribute sets the refraction color stored in a refraction material attribute to the value given by the $color$ parameter.
	//# A renderable object to which the affected attribute applies will subsequently be rendered using the new color.
	//
	//# \also	$@RefractionAttribute::GetRefractionColor@$


	class RefractionAttribute final : public Attribute
	{
		public:

			struct RefractionParams
			{
				float		refractionOffsetScale;
			};

		private:

			ColorRGBA			refractionColor;
			RefractionParams	refractionParams;

			Attribute *Replicate(void) const override;

		public:

			C4API explicit RefractionAttribute(unsigned_int32 flags = 0);
			C4API RefractionAttribute(const ColorRGBA& color, float scale, unsigned_int32 flags = 0);
			C4API RefractionAttribute(const RefractionAttribute& refractionAttribute);
			C4API ~RefractionAttribute();

			const ColorRGBA& GetRefractionColor(void) const
			{
				return (refractionColor);
			}

			void SetRefractionColor(const ColorRGBA& color)
			{
				refractionColor = color;
			}

			const RefractionParams *GetRefractionParams(void) const
			{
				return (&refractionParams);
			}

			void SetRefractionOffsetScale(float scale)
			{
				refractionParams.refractionOffsetScale = scale;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			bool operator ==(const Attribute& attribute) const override;

			void SetAttributeColor(const ColorRGBA& color) override;
	};


	//# \class	MicrofacetAttribute		Material attribute for microfacet shading.
	//
	//# The $MicrofacetAttribute$ class represents the material attribute for microfacet shading.
	//
	//# \def	class MicrofacetAttribute final : public Attribute
	//
	//# \ctor	MicrofacetAttribute(const ColorRGBA& color, const Vector2D& slope, float reflectivity, unsigned_int32 flags = 0);
	//
	//# \param	color			The microfacet color.
	//# \param	slope			The average microfacet slope in the two tangent directions.
	//# \param	reflectivity	The microfacet reflectivity.
	//# \param	flags			The initial attribute flags. See the $@Attribute::SetAttributeFlags@$ function.
	//
	//# \desc
	//#
	//
	//# \base	Attribute	A $MicrofacetAttribute$ is a specific type of attribute.
	//
	//# \also	$@Math/ColorRGBA@$


	class MicrofacetAttribute final : public Attribute
	{
		public:

			enum
			{
				kMicrofacetTextureSize = 32
			};

			class MicrofacetTexture : public Shared, public LinkTarget<MicrofacetTexture>
			{
				private:

					Texture		*texture;
					ColorRGB	microfacetColor;

				protected:

					MicrofacetTexture(const ColorRGB& color);
					~MicrofacetTexture();

					void SetTexture(const TextureHeader *header, const void *image)
					{
						texture = Texture::Get(header, image);
					}

				public:

					const ColorRGB& GetMicrofacetColor(void) const
					{
						return (microfacetColor);
					}

					const Texture *GetTextureObject(void) const
					{
						return (texture);
					}
			};

			class IsotropicMicrofacetTexture : public MicrofacetTexture, public HashTableElement<IsotropicMicrofacetTexture>
			{
				public:

					struct KeyType
					{
						ColorRGB	color;
						float		slope;
					};

				private:

					KeyType		textureKey;
					Color4C		textureImage[kMicrofacetTextureSize * kMicrofacetTextureSize];

				public:

					IsotropicMicrofacetTexture(const ColorRGB& color, float slope, float threshold);
					~IsotropicMicrofacetTexture();

					const KeyType& GetKey(void) const
					{
						return (textureKey);
					}

					static unsigned_int32 Hash(const KeyType& key)
					{
						return (MaxZero((int32) (key.slope * 100.0F + (key.color.red + key.color.green + key.color.blue) * 16.0F)));
					}
			};

			class AnisotropicMicrofacetTexture : public MicrofacetTexture, public HashTableElement<AnisotropicMicrofacetTexture>
			{
				public:

					struct KeyType
					{
						ColorRGB	color;
						Vector2D	slope;
					};

				private:

					KeyType		textureKey;
					Color4C		textureImage[kMicrofacetTextureSize * kMicrofacetTextureSize * kMicrofacetTextureSize];

				public:

					AnisotropicMicrofacetTexture(const ColorRGB& color, const Vector2D& slope, float threshold);
					~AnisotropicMicrofacetTexture();

					const KeyType& GetKey(void) const
					{
						return (textureKey);
					}

					static unsigned_int32 Hash(const KeyType& key)
					{
						return (MaxZero((int32) ((key.slope.x + key.slope.y) * 50.0F + (key.color.red + key.color.green + key.color.blue) * 16.0F)));
					}
			};

			struct MicrofacetParams
			{
				ColorRGBA							microfacetColor;
				Vector2D							microfacetSlope;
				mutable float						microfacetThreshold;
				mutable Link<MicrofacetTexture>		microfacetTexture;

				void Invalidate(void)
				{
					MicrofacetTexture *texture = microfacetTexture;
					if (texture)
					{
						texture->Release();
						microfacetTexture = nullptr;
					}
				}
			};

		private:

			float				microfacetReflectivity;
			MicrofacetParams	microfacetParams;

			static const TextureHeader isotropicMicrofacetTextureHeader;
			static const TextureHeader anisotropicMicrofacetTextureHeader;

			static Storage<HashTable<IsotropicMicrofacetTexture>>		isotropicHashTable;
			static Storage<HashTable<AnisotropicMicrofacetTexture>>		anisotropicHashTable;

			Attribute *Replicate(void) const override;

			static float CalculateThreshold(const MicrofacetParams *params);
			static ColorRGB CalculateRefractionIndex(const ColorRGB& color);
			static float CalculateFresnelTerm(float L_dot_H, float refractionIndex);

		public:

			C4API explicit MicrofacetAttribute(unsigned_int32 flags = 0);
			C4API MicrofacetAttribute(const ColorRGBA& color, const Vector2D& slope, float reflectivity, unsigned_int32 flags = 0);
			C4API MicrofacetAttribute(const MicrofacetAttribute& microfacetAttribute);
			C4API ~MicrofacetAttribute();

			const MicrofacetParams *GetMicrofacetParams(void) const
			{
				return (&microfacetParams);
			}

			void SetMicrofacetColor(const ColorRGBA& color)
			{
				microfacetParams.microfacetColor = color;
				microfacetParams.Invalidate();
			}

			void SetMicrofacetSlope(const Vector2D& slope)
			{
				microfacetParams.microfacetSlope = slope;
				microfacetParams.Invalidate();
			}

			float GetMicrofacetReflectivity(void) const
			{
				return (microfacetReflectivity);
			}

			void SetMicrofacetReflectivity(float reflectivity)
			{
				microfacetReflectivity = reflectivity;
			}

			static void Initialize(void);
			static void Terminate(void);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			bool operator ==(const Attribute& attribute) const override;

			void SetAttributeColor(const ColorRGBA& color) override;

			static const Texture *GetTextureObject(const MicrofacetParams *params);
	};


	inline bool operator ==(const MicrofacetAttribute::IsotropicMicrofacetTexture::KeyType& x, const MicrofacetAttribute::IsotropicMicrofacetTexture::KeyType& y)
	{
		return ((x.color == y.color) && (x.slope == y.slope));
	}

	inline bool operator ==(const MicrofacetAttribute::AnisotropicMicrofacetTexture::KeyType& x, const MicrofacetAttribute::AnisotropicMicrofacetTexture::KeyType& y)
	{
		return ((x.color == y.color) && (x.slope == y.slope));
	}


	//# \class	TextureAttribute		Material attribute base class for texture maps.
	//
	//# The $TextureAttribute$ class is the base class for attributes that use a texture map.
	//
	//# \def	class TextureAttribute : public Attribute
	//
	//# \ctor	TextureAttribute(AttributeType type, const char *name);
	//# \ctor	TextureAttribute(AttributeType type, Texture *texture);
	//# \ctor	TextureAttribute(AttributeType type, const TextureHeader *header, const void *image = nullptr);
	//
	//# The constructors have protected access. The $TextureAttribute$ class can only exist as the base class
	//# for other material attribute classes that use texture maps.
	//
	//# \param	type		The attribute type. This must be the type of a subclass that inherits from $TextureAttribute$.
	//# \param	name		The name of the texture map to load.
	//# \param	texture		The texture object to use. Specifying this parameter increments the reference count of the texture object.
	//# \param	header		A texture header from which to construct a new texture object.
	//# \param	image		A pointer to a texture image that is used if the texture header does not specify an offset to an image.
	//
	//# \desc
	//# The $TextureAttribute$ class encapsulates information about a texture map for other material attributes.
	//# If a $TextureAttribute$ object is constructed by passing the $name$ parameter, then the texture map
	//# is loaded through the Resource Manager. If a $TextureAttribute$ object is constructed by passing the
	//# $header$ and $image$ parameters, then a new texture map is constructed using the information in
	//# the $@TextureHeader@$ structure. If the $image$ parameter is not $nullptr$, then the texture image
	//# is read from the location to which it points; otherwise, the texture header specifies the location
	//# of the image.
	//
	//# \base	Attribute	A $TextureAttribute$ is a specific type of attribute.
	//
	//# \also	$@DiffuseTextureAttribute@$
	//# \also	$@SpecularTextureAttribute@$
	//# \also	$@EmissionTextureAttribute@$
	//# \also	$@NormalTextureAttribute@$
	//# \also	$@HorizonTextureAttribute@$


	//# \function	TextureAttribute::GetTextureName		Returns the name of the texture map.
	//
	//# \proto	const ResourceName& GetTextureName(void) const;
	//
	//# \desc
	//# The $GetTextureName$ function returns the name of the texture map used by the attribute.
	//# If the texture map does not have a name (because it was not loaded from a resource),
	//# then the name is the empty string.
	//
	//# \also	$@TextureAttribute::GetTexture@$
	//# \also	$@TextureAttribute::SetTexture@$


	//# \function	TextureAttribute::GetTexture		Returns the texture map object.
	//
	//# \proto	Texture *const& GetTexture(void) const;
	//
	//# \desc
	//# The $GetTexture$ function returns a pointer to the texture map object used by the attribute.
	//
	//# \also	$@TextureAttribute::SetTexture@$
	//# \also	$@TextureAttribute::GetTextureName@$


	//# \function	TextureAttribute::SetTexture		Sets the texture map object.
	//
	//# \proto	void SetTexture(const char *name);
	//# \proto	void SetTexture(Texture *texture);
	//# \proto	void SetTexture(const TextureHeader *header, const void *image = nullptr);
	//
	//# \param	name		The name of the texture map to load.
	//# \param	texture		The texture object to use. Specifying this parameter increments the reference count of the texture object.
	//# \param	header		A texture header from which to construct a new texture object.
	//# \param	image		A pointer to a texture image that is used if the texture header does not specify an offset to an image.
	//
	//# \desc
	//# The $SetTexture$ function sets the texture map object used by the attribute.
	//# If the $name$ parameter is passed to this function, then the texture map
	//# is loaded through the Resource Manager. If the $header$ and $image$ parameters are used,
	//# then a new texture map is constructed using the information in the $@TextureHeader@$ structure.
	//# If the $image$ parameter is not $nullptr$, then the texture image is read from the location
	//# to which it points; otherwise, the texture header specifies the location of the image.
	//
	//# \also	$@TextureAttribute::GetTexture@$


	class TextureAttribute : public Attribute
	{
		private:

			ResourceName	textureName;
			Texture			*textureObject;

			int32			texcoordIndex;

		protected:

			TextureAttribute(AttributeType type);
			TextureAttribute(AttributeType type, const char *name);
			TextureAttribute(AttributeType type, Texture *texture);
			TextureAttribute(AttributeType type, const TextureHeader *header, const void *image = nullptr);
			TextureAttribute(AttributeType type, const char *name, const TextureHeader *header, const void *image = nullptr);
			TextureAttribute(const TextureAttribute& textureAttribute);

		public:

			C4API ~TextureAttribute();

			const ResourceName& GetTextureName(void) const
			{
				return (textureName);
			}

			Texture *GetTexture(void) const
			{
				return (textureObject);
			}

			int32 GetTexcoordIndex(void) const
			{
				return (texcoordIndex);
			}

			void SetTexcoordIndex(int32 index)
			{
				texcoordIndex = index;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;

			bool operator ==(const Attribute& attribute) const override;

			C4API void SetTexture(const char *name);
			C4API void SetTexture(Texture *texture);
			C4API void SetTexture(const TextureHeader *header, const void *image = nullptr);
	};


	//# \class	DiffuseTextureAttribute		Material attribute for a diffuse texture map.
	//
	//# The $DiffuseTextureAttribute$ class represents the material attribute for a diffuse texture map.
	//
	//# \def	class DiffuseTextureAttribute final : public TextureAttribute
	//
	//# \ctor	explicit DiffuseTextureAttribute(const char *name);
	//# \ctor	explicit DiffuseTextureAttribute(Texture *texture);
	//# \ctor	DiffuseTextureAttribute(const TextureHeader *header, const void *image = nullptr);
	//
	//# \param	name		The name of the texture map to load.
	//# \param	texture		The texture object to use.
	//# \param	header		A texture header from which to construct a new texture object.
	//# \param	image		A pointer to a texture image that is used if the texture header does not specify an offset to an image.
	//
	//# \desc
	//# The $DiffuseTextureAttribute$ class represents the material attribute for a diffuse texture map.
	//# The diffuse texture map is applied during both the ambient rendering pass and each lighting pass.
	//#
	//# See the $@TextureAttribute@$ class for a description of the differences among the various constructors.
	//
	//# \base	TextureAttribute	All attributes using a texture map are subclasses of $TextureAttribute$.


	class DiffuseTextureAttribute final : public TextureAttribute
	{
		private:

			Attribute *Replicate(void) const override;

		public:

			C4API DiffuseTextureAttribute();
			C4API explicit DiffuseTextureAttribute(const char *name);
			C4API explicit DiffuseTextureAttribute(Texture *texture);
			C4API DiffuseTextureAttribute(const TextureHeader *header, const void *image = nullptr);
			C4API DiffuseTextureAttribute(const char *name, const TextureHeader *header, const void *image = nullptr);
			C4API DiffuseTextureAttribute(const DiffuseTextureAttribute& diffuseTextureAttribute);
			C4API ~DiffuseTextureAttribute();
	};


	//# \class	SpecularTextureAttribute		Material attribute for a specular texture map.
	//
	//# The $SpecularTextureAttribute$ class represents the material attribute for a specular texture map.
	//
	//# \def	class SpecularTextureAttribute final : public TextureAttribute
	//
	//# \ctor	explicit SpecularTextureAttribute(const char *name);
	//# \ctor	explicit SpecularTextureAttribute(Texture *texture);
	//# \ctor	SpecularTextureAttribute(const TextureHeader *header, const void *image = nullptr);
	//
	//# \param	name		The name of the texture map to load.
	//# \param	texture		The texture object to use.
	//# \param	header		A texture header from which to construct a new texture object.
	//# \param	image		A pointer to a texture image that is used if the texture header does not specify an offset to an image.
	//
	//# \desc
	//# The $SpecularTextureAttribute$ class represents the material attribute for a specular texture map.
	//# The specular texture map modulates specular reflection and environment mapping.
	//#
	//# See the $@TextureAttribute@$ class for a description of the differences among the various constructors.
	//
	//# \base	TextureAttribute	All attributes using a texture map are subclasses of $TextureAttribute$.


	class SpecularTextureAttribute final : public TextureAttribute
	{
		private:

			Attribute *Replicate(void) const override;

		public:

			C4API SpecularTextureAttribute();
			C4API explicit SpecularTextureAttribute(const char *name);
			C4API explicit SpecularTextureAttribute(Texture *texture);
			C4API SpecularTextureAttribute(const TextureHeader *header, const void *image = nullptr);
			C4API SpecularTextureAttribute(const SpecularTextureAttribute& specularTextureAttribute);
			C4API ~SpecularTextureAttribute();
	};


	//# \class	EmissionTextureAttribute		Material attribute for an emission texture map.
	//
	//# The $EmissionTextureAttribute$ class represents the material attribute for an emission texture map.
	//
	//# \def	class EmissionTextureAttribute final : public TextureAttribute
	//
	//# \ctor	explicit EmissionTextureAttribute(const char *name);
	//# \ctor	explicit EmissionTextureAttribute(Texture *texture);
	//# \ctor	EmissionTextureAttribute(const TextureHeader *header, const void *image = nullptr);
	//
	//# \param	name		The name of the texture map to load.
	//# \param	texture		The texture object to use.
	//# \param	header		A texture header from which to construct a new texture object.
	//# \param	image		A pointer to a texture image that is used if the texture header does not specify an offset to an image.
	//
	//# \desc
	//# The $EmissionTextureAttribute$ class represents the material attribute for an emission texture map.
	//# The emission texture map is applied only during the ambient rendering pass.
	//#
	//# See the $@TextureAttribute@$ class for a description of the differences among the various constructors.
	//
	//# \base	TextureAttribute	All attributes using a texture map are subclasses of $TextureAttribute$.


	class EmissionTextureAttribute final : public TextureAttribute
	{
		private:

			Attribute *Replicate(void) const override;

		public:

			C4API EmissionTextureAttribute();
			C4API explicit EmissionTextureAttribute(const char *name);
			C4API explicit EmissionTextureAttribute(Texture *texture);
			C4API EmissionTextureAttribute(const TextureHeader *header, const void *image = nullptr);
			C4API EmissionTextureAttribute(const EmissionTextureAttribute& emissionTextureAttribute);
			C4API ~EmissionTextureAttribute();
	};


	//# \class	NormalTextureAttribute		Material attribute for a normal texture map.
	//
	//# The $NormalTextureAttribute$ class represents the material attribute for a normal texture map.
	//
	//# \def	class NormalTextureAttribute final : public TextureAttribute
	//
	//# \ctor	explicit NormalTextureAttribute(const char *name);
	//# \ctor	explicit NormalTextureAttribute(Texture *texture);
	//# \ctor	NormalTextureAttribute(const TextureHeader *header, const void *image = nullptr);
	//
	//# \param	name		The name of the texture map to load.
	//# \param	texture		The texture object to use.
	//# \param	header		A texture header from which to construct a new texture object.
	//# \param	image		A pointer to a texture image that is used if the texture header does not specify an offset to an image.
	//
	//# \desc
	//# The $NormalTextureAttribute$ class represents the material attribute for a normal texture map.
	//# The normal texture map is applied during each lighting pass and is also used for environment-mapped bump mapping.
	//#
	//# See the $@TextureAttribute@$ class for a description of the differences among the various constructors.
	//
	//# \base	TextureAttribute	All attributes using a texture map are subclasses of $TextureAttribute$.


	class NormalTextureAttribute final : public TextureAttribute
	{
		private:

			float	parallaxScale;

			Attribute *Replicate(void) const override;

		public:

			C4API NormalTextureAttribute();
			C4API explicit NormalTextureAttribute(const char *name);
			C4API explicit NormalTextureAttribute(Texture *texture);
			C4API NormalTextureAttribute(const TextureHeader *header, const void *image = nullptr);
			C4API NormalTextureAttribute(const NormalTextureAttribute& normalTextureAttribute);
			C4API ~NormalTextureAttribute();

			const float& GetParallaxScale(void) const
			{
				return (parallaxScale);
			}

			void SetParallaxScale(float scale)
			{
				parallaxScale = scale;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;

			bool operator ==(const Attribute& attribute) const override;
	};


	//# \class	OpacityTextureAttribute		Material attribute for an opacity texture map.
	//
	//# The $OpacityTextureAttribute$ class represents the material attribute for an opacity texture map.
	//
	//# \def	class OpacityTextureAttribute final : public TextureAttribute
	//
	//# \ctor	explicit OpacityTextureAttribute(const char *name);
	//# \ctor	explicit OpacityTextureAttribute(Texture *texture);
	//# \ctor	OpacityTextureAttribute(const TextureHeader *header, const void *image = nullptr);
	//
	//# \param	name		The name of the texture map to load.
	//# \param	texture		The texture object to use.
	//# \param	header		A texture header from which to construct a new texture object.
	//# \param	image		A pointer to a texture image that is used if the texture header does not specify an offset to an image.
	//
	//# \desc
	//# The $OpacityTextureAttribute$ class represents the material attribute for an opacity texture map.
	//# The opacity texture map is applied only during the ambient rendering pass.
	//#
	//# See the $@TextureAttribute@$ class for a description of the differences among the various constructors.
	//
	//# \base	TextureAttribute	All attributes using a texture map are subclasses of $TextureAttribute$.


	class OpacityTextureAttribute final : public TextureAttribute
	{
		private:

			Attribute *Replicate(void) const override;

		public:

			C4API OpacityTextureAttribute();
			C4API explicit OpacityTextureAttribute(const char *name);
			C4API explicit OpacityTextureAttribute(Texture *texture);
			C4API OpacityTextureAttribute(const TextureHeader *header, const void *image = nullptr);
			C4API OpacityTextureAttribute(const OpacityTextureAttribute& opacityTextureAttribute);
			C4API ~OpacityTextureAttribute();
	};


	//# \class	EnvironmentTextureAttribute		Material attribute for an environment texture map.
	//
	//# The $EnvironmentTextureAttribute$ class represents the material attribute for an environment texture map.
	//
	//# \def	class EnvironmentTextureAttribute final : public TextureAttribute
	//
	//# \ctor	explicit EnvironmentTextureAttribute(const char *name);
	//# \ctor	explicit EnvironmentTextureAttribute(Texture *texture);
	//# \ctor	EnvironmentTextureAttribute(const TextureHeader *header, const void *image = nullptr);
	//
	//# \param	name		The name of the texture map to load.
	//# \param	texture		The texture object to use.
	//# \param	header		A texture header from which to construct a new texture object.
	//# \param	image		A pointer to a texture image that is used if the texture header does not specify an offset to an image.
	//
	//# \desc
	//# The $EnvironmentTextureAttribute$ class represents the material attribute for an environment texture map.
	//# If this attribute appears in a renderable object's attribute list, then it overrides any external environment map.
	//# The environment texture map is applied only during the ambient rendering pass.
	//#
	//# See the $@TextureAttribute@$ class for a description of the differences among the various constructors.
	//
	//# \base	TextureAttribute	All attributes using a texture map are subclasses of $TextureAttribute$.


	class EnvironmentTextureAttribute final : public TextureAttribute
	{
		private:

			Attribute *Replicate(void) const override;

		public:

			C4API EnvironmentTextureAttribute();
			C4API explicit EnvironmentTextureAttribute(const char *name);
			C4API explicit EnvironmentTextureAttribute(Texture *texture);
			C4API EnvironmentTextureAttribute(const TextureHeader *header, const void *image = nullptr);
			C4API EnvironmentTextureAttribute(const EnvironmentTextureAttribute& environmentTextureAttribute);
			C4API ~EnvironmentTextureAttribute();
	};


	class FireTextureAttribute final : public TextureAttribute
	{
		public:

			struct FireParams
			{
				float		fireIntensity;
				Vector2D	noiseVelocity[3];
			};

		private:

			FireParams		fireParams;

			Attribute *Replicate(void) const override;

		public:

			C4API FireTextureAttribute();
			C4API FireTextureAttribute(float intensity, const Vector2D& velocity1, const Vector2D& velocity2, const Vector2D& velocity3);
			C4API FireTextureAttribute(const FireTextureAttribute& fireTextureAttribute);
			C4API ~FireTextureAttribute();

			const FireParams *GetFireParams(void) const
			{
				return (&fireParams);
			}

			void SetFireIntensity(float intensity)
			{
				fireParams.fireIntensity = intensity;
			}

			void SetFireSpeed(int32 speed)
			{
				CalculateNoiseVelocities(speed, fireParams.noiseVelocity);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			bool operator ==(const Attribute& attribute) const override;

			C4API static void CalculateNoiseVelocities(int32 speed, Vector2D *velocity);
	};


	class DeltaDepthAttribute final : public Attribute
	{
		private:

			float		deltaScale;

			Attribute *Replicate(void) const override;

		public:

			C4API DeltaDepthAttribute();
			C4API explicit DeltaDepthAttribute(float scale);
			C4API DeltaDepthAttribute(const DeltaDepthAttribute& deltaDepthAttribute);
			C4API ~DeltaDepthAttribute();

			float GetDeltaScale(void) const
			{
				return (deltaScale);
			}

			void SetDeltaScale(float scale)
			{
				deltaScale = scale;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			bool operator ==(const Attribute& attribute) const override;
	};


	class DepthRampAttribute final : public Attribute
	{
		private:

			Range<float>		rampRange;

			Attribute *Replicate(void) const override;

		public:

			C4API DepthRampAttribute();
			C4API DepthRampAttribute(const Range<float>& range);
			C4API DepthRampAttribute(const DepthRampAttribute& depthRampAttribute);
			C4API ~DepthRampAttribute();

			const Range<float>& GetRampRange(void) const
			{
				return (rampRange);
			}

			void SetRampRange(const Range<float>& range)
			{
				rampRange = range;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			bool operator ==(const Attribute& attribute) const override;
	};
}


#endif

// ZYUQURM
