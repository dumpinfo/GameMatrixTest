 

#ifndef C4LightObjects_h
#define C4LightObjects_h


//# \component	Graphics Manager
//# \prefix		GraphicsMgr/


#include "C4Objects.h"
#include "C4Bounding.h"
#include "C4Textures.h"


namespace C4
{
	typedef Type	LightType;


	//# \enum	LightType

	enum : LightType
	{
		kLightInfinite				= 'INFT',	//## Infinite light with parallel rays and no attenuation.
		kLightPoint					= 'PONT',	//## Point light with omnidirectional rays and distance attenuation.
		kLightCube					= 'CUBE',	//## Point light with a projected cube texture.
		kLightSpot					= 'SPOT'	//## Spot light with a projected 2D texture.
	};


	//# \enum	LightFlags

	enum
	{
		kLightStatic				= 1 << 0,	//## The light is static (does not move).
		kLightShadowInhibit			= 1 << 1,	//## The light does not cast shadows.
		kLightGenerator				= 1 << 2,	//## The light generates a projected texture.
		kLightUnified				= 1 << 3,	//## The light can be combined with ambient lighting to produce unified shaders.
		kLightPortalInhibit			= 1 << 4,	//## The light does not shine through portals.
		kLightConfined				= 1 << 6,	//## The light is confined to a small sphere (point light only).
		kLightInstanceShadowSpace	= 1 << 7,	//## If the light is not directly connected to a shadow space, then it should use the shadow space connected by an instance super node.
		kLightRootZoneInhibit		= 1 << 9	//## The light's illumination tree should be rooted only in zones that connect to it and not its containing zone (infinite light only).
	};


	enum
	{
		kMaxShadowCascadeCount		= 4
	};


	class Texture;


	struct LightShadowData
	{
		Vector3D		shadowSize;
		Vector3D		inverseShadowSize;
		Point3D			shadowPosition;

		float			texelSize;

		float			minDepth;
		Antivector4D	nearPlane;
		Antivector4D	farPlane;
		Antivector4D	cascadePlane;

		Point3D			cascadePolygon[4];
	};


	//# \class	LightObject		Encapsulates data pertaining to a light source.
	//
	//# The $LightObject$ class encapsulates data pertaining to a light source.
	//
	//# \def	class LightObject : public Object
	//
	//# \ctor	LightObject(LightType type, LightType base, const ColorRGB& color);
	//
	//# The constructor has protected access. The $LightObject$ class can only exist as the base class for another class.
	//
	//# \param	type	The type of the light source. See below for a list of possible types.
	//# \param	base	The base type of the light source. This should be $kLightInfinite$ or $kLightPoint$.
	//# \param	color	The color of light emitted by the light source.
	//
	//# \desc
	//# The $LightObject$ class is the base class for all types of light objects, and it stores
	//# data that is common to all of them. Each specific type of light has an associated object
	//# that is a subclass of the $LightObject$ class.
	//#
	//# A light object can have one of the following types.
	//
	//# \table	LightType
	//
	//# \base	WorldMgr/Object		A $LightObject$ is an object that can be shared by multiple light nodes.
	//
	//# \also	$@WorldMgr/Light@$
	//
	//# \wiki	Lights


	//# \function	LightObject::GetLightType		Returns the specific type of a light. 
	//
	//# \proto	LightType GetLightType(void) const;
	// 
	//# \desc
	//# The $GetLightType$ function returns the specific light type, which can be one of the following constants. 
	//
	//# \table	LightType
	// 
	//# All of the light types are divided into two categories, and the general category that a light object
	//# falls into can be determined by calling the $@LightObject::GetBaseLightType@$ function. 
	// 
	//# \also	$@LightObject::GetBaseLightType@$


	//# \function	LightObject::GetBaseLightType		Returns the base type of a light. 
	//
	//# \proto	LightType GetBaseLightType(void) const;
	//
	//# \desc
	//# All of the light types are divided into two categories, lights that are directional and have infinite
	//# range and lights that are localized and have finite range. The $GetBaseLightType$ function returns the base
	//# light type, which can only be $kLightInfinite$ or $kLightPoint$. This represents which general category
	//# the light object falls into. The more specific type of light can be determined by calling the
	//# $@LightObject::GetLightType@$ function.
	//#
	//# The $@InfiniteLightObject@$ class has the $kLightInfinite$ base type, and the $@PointLightObject@$,
	//# $@CubeLightObject@$, and $@SpotLightObject@$ classes have the $kLightPoint$ base type.
	//
	//# \also	$@LightObject::GetLightType@$


	//# \function	LightObject::GetLightColor		Returns the light color.
	//
	//# \proto	const ColorRGB& GetLightColor(void) const;
	//
	//# \desc
	//# The $GetLightColor$ function returns the light color.
	//
	//# \also	$@Math/ColorRGB@$
	//# \also	$@LightObject::SetLightColor@$


	//# \function	LightObject::SetLightColor		Sets the light color.
	//
	//# \proto	void SetLightColor(const ColorRGB& color);
	//
	//# \param	color	The new light color.
	//
	//# \desc
	//# The $SetLightColor$ function sets the light color. The light color may be changed at any time,
	//# and doing so has immediate effect.
	//#
	//# To turn a light off, the $kNodeDisabled$ flag should be set for the $@WorldMgr/Light@$ node by
	//# calling the $@WorldMgr/Node::SetNodeFlags@$ function. This results in much better performance than
	//# setting the light's color to black.
	//
	//# \also	$@Math/ColorRGB@$
	//# \also	$@LightObject::GetLightColor@$


	//# \function	LightObject::GetLightFlags		Returns the light flags.
	//
	//# \proto	unsigned_int32 GetLightFlags(void) const;
	//
	//# \desc
	//# The $GetLightFlags$ function returns the light flags, which can be a combination (through logical OR) of the following constants.
	//
	//# \table	LightFlags
	//
	//# \also	$@LightObject::SetLightFlags@$


	//# \function	LightObject::SetLightFlags		Sets the light flags.
	//
	//# \proto	void SetLightFlags(unsigned_int32 flags);
	//
	//# \param	flags	The new light flags.
	//
	//# \desc
	//# The $SetLightFlags$ function sets the light flags to the value specified by the $flags$ parameter,
	//# which can be a combination (through logical OR) of the following constants.
	//
	//# \table	LightFlags
	//
	//# The $kLightGenerator$ flag only pertains to depth lights, cube lights, and spot lights. It indicates
	//# that a projected texture should be generated for a light during the development process.
	//
	//# \also	$@LightObject::GetLightFlags@$


	class LightObject : public Object
	{
		friend class Object;

		private:

			LightType			lightType;
			LightType			baseLightType;

			ColorRGB			lightColor;
			unsigned_int32		lightFlags;
			int32				minDetailLevel;

			static LightObject *Create(Unpacker& data, unsigned_int32 unpackFlags);

		protected:

			LightObject(LightType type, LightType base);
			LightObject(LightType type, LightType base, const ColorRGB& color);
			LightObject(const LightObject& lightObject);
			virtual ~LightObject();

		public:

			LightType GetLightType(void) const
			{
				return (lightType);
			}

			LightType GetBaseLightType(void) const
			{
				return (baseLightType);
			}

			const ColorRGB& GetLightColor(void) const
			{
				return (lightColor);
			}

			void SetLightColor(const ColorRGB& color)
			{
				lightColor = color;
			}

			unsigned_int32 GetLightFlags(void) const
			{
				return (lightFlags);
			}

			void SetLightFlags(unsigned_int32 flags)
			{
				lightFlags = flags;
			}

			int32 GetMinDetailLevel(void) const
			{
				return (minDetailLevel);
			}

			void SetMinDetailLevel(int32 level)
			{
				minDetailLevel = level;
			}

			void PackType(Packer& data) const override;
			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetCategoryCount(void) const override;
			Type GetCategoryType(int32 index, const char **title) const override;
			int32 GetCategorySettingCount(Type category) const override;
			Setting *GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const override;
			void SetCategorySetting(Type category, const Setting *setting) override;
	};


	//# \class	InfiniteLightObject		Encapsulates data pertaining to an infinite light source.
	//
	//# The $InfiniteLightObject$ class encapsulates data pertaining to an infinite light source.
	//
	//# \def	class InfiniteLightObject final : public LightObject
	//
	//# \ctor	InfiniteLightObject(const ColorRGB& color);
	//
	//# \param	color	The color of light emitted by the light source.
	//
	//# \desc
	//# An infinite light source illuminates a world with parallel rays of light having no distance attenuation,
	//# and it casts shadows using cascaded shadow maps. The $InfiniteLightObject$ class contains information
	//# specific to the infinite light type.
	//
	//# \base	LightObject		An $InfiniteLightObject$ is a specific type of light object.
	//
	//# \also	$@WorldMgr/InfiniteLight@$
	//
	//# \wiki	Infinite_Light	Infinite Light


	//# \function	InfiniteLightObject::GetCascadeRangeArray		Returns the cascade range array for shadow mapping.
	//
	//# \proto	Range<float> *GetCascadeRangeArray(void);
	//# \proto	const Range<float> *GetCascadeRangeArray(void) const;
	//
	//# \desc
	//# The $GetCascadeRangeArray$ function returns a pointer to an array of ranges that correspond to the
	//# minimum and maximum distances covered by each cascade of the light's shadow map. The number of entries
	//# in the array is given by the constant $kMaxShadowCascadeCount$.
	//
	//# \also	$@Utilities/Range@$


	class InfiniteLightObject final : public LightObject
	{
		friend class LightObject;

		private:

			Range<float>		cascadeRange[kMaxShadowCascadeCount];

			InfiniteLightObject();
			InfiniteLightObject(const InfiniteLightObject& infiniteLightObject);
			~InfiniteLightObject();

			Object *Replicate(void) const override;

		public:

			InfiniteLightObject(const ColorRGB& color);

			Range<float> *GetCascadeRangeArray(void)
			{
				return (cascadeRange);
			}

			const Range<float> *GetCascadeRangeArray(void) const
			{
				return (cascadeRange);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetCategorySettingCount(Type category) const override;
			Setting *GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const override;
			void SetCategorySetting(Type category, const Setting *setting) override;
	};


	//# \class	PointLightObject		Encapsulates data pertaining to a point light source.
	//
	//# The $PointLightObject$ class encapsulates data pertaining to a point light source.
	//
	//# \def	class PointLightObject : public LightObject
	//
	//# \ctor	PointLightObject(const ColorRGB& color, float range);
	//
	//# \param	color	The color of light emitted by the light source.
	//# \param	range	The spherical range of the light source.
	//
	//# \desc
	//# A point light source illuminates a world from a single point with distance attenuation causing its
	//# intensity to fall to zero at its maximum range. The $PointLightObject$ class contains information
	//# specific to the point light type.
	//
	//# \base	LightObject		A $PointLightObject$ is a specific type of light object.
	//
	//# \also	$@WorldMgr/PointLight@$


	//# \function	PointLightObject::GetLightRange		Returns the spherical range of the light source.
	//
	//# \proto	float GetLightRange(void) const;
	//
	//# \desc
	//# The $GetLightRange$ function returns the range of the point light source. The light attenuation
	//# is zero at this distance from the light source, and no illumination is applied outside this range.
	//
	//# \also	$@PointLightObject::SetLightRange@$


	//# \function	PointLightObject::SetLightRange		Sets the spherical range of the light source.
	//
	//# \proto	void SetLightRange(float range);
	//
	//# \param	range	The spherical range of the light source.
	//
	//# \desc
	//# The $SetLightRange$ function sets the range of the point light source. The light attenuation
	//# is zero at this distance from the light source, and no illumination is applied outside this range.
	//
	//# \also	$@PointLightObject::GetLightRange@$


	class PointLightObject : public LightObject
	{
		friend class LightObject;

		private:

			float			lightRange;
			Range<float>	fadeDistance;

			float			minShadowDistance;
			float			confinementRadius;

			PointLightObject();

			Object *Replicate(void) const override;

		protected:

			PointLightObject(LightType type);
			PointLightObject(LightType type, const ColorRGB& color, float range);
			PointLightObject(const PointLightObject& pointLightObject);
			~PointLightObject();

		public:

			PointLightObject(const ColorRGB& color, float range);

			float GetLightRange(void) const
			{
				return (lightRange);
			}

			void SetLightRange(float range)
			{
				lightRange = range;
			}

			const Range<float>& GetFadeDistance(void) const
			{
				return (fadeDistance);
			}

			void SetFadeDistance(const Range<float>& distance)
			{
				fadeDistance = distance;
			}

			float GetMinShadowDistance(void) const
			{
				return (minShadowDistance);
			}

			void SetMinShadowDistance(float distance)
			{
				minShadowDistance = distance;
			}

			float GetConfinementRadius(void) const
			{
				return (confinementRadius);
			}

			void SetConfinementRadius(float radius)
			{
				confinementRadius = radius;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetCategorySettingCount(Type category) const override;
			Setting *GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const override;
			void SetCategorySetting(Type category, const Setting *setting) override;

			int32 GetObjectSize(float *size) const override;
			void SetObjectSize(const float *size) override;
	};


	//# \class	ProjectionLightObject		Encapsulates data pertaining to a point light source having a projected texture map.
	//
	//# The $PointLightObject$ class encapsulates data pertaining to a point light source having a projected texture map.
	//
	//# \def	class ProjectionLightObject : public PointLightObject
	//
	//# \ctor	ProjectionLightObject(LightType type, const ColorRGB& color, float range, const char *name);
	//
	//# The constructor has protected access. The $ProjectionLightObject$ class can only exist as the base class for another class.
	//
	//# \param	type	The type of light source. This must be either $kLightCube$ or $kLightSpot$.
	//# \param	color	The color of light emitted by the light source.
	//# \param	range	The spherical range of the light source.
	//# \param	name	The name of the projected texture map.
	//
	//# \desc
	//
	//# \base	PointLightObject		A $ProjectionLightObject$ is a specific type of point light object.
	//
	//# \also	$@CubeLightObject@$
	//# \also	$@SpotLightObject@$


	//# \function	ProjectionLightObject::GetTextureSize		Returns the size of the projected texture map.
	//
	//# \proto	int32 GetTextureSize(void) const;
	//
	//# \desc
	//
	//# \also	$@ProjectionLightObject::SetTextureSize@$
	//# \also	$@ProjectionLightObject::GetProjectionMap@$
	//# \also	$@ProjectionLightObject::GetProjectionName@$


	//# \function	ProjectionLightObject::SetTextureSize		Sets the size of the projected texture map.
	//
	//# \proto	void SetTextureSize(int32 size);
	//
	//# \param	size	The new texture size.
	//
	//# \desc
	//
	//# \also	$@ProjectionLightObject::GetTextureSize@$


	//# \function	ProjectionLightObject::GetProjectionMap		Returns a pointer to the $@Texture@$ object for the projected texture map.
	//
	//# \proto	Texture *const& GetProjectionMap(void) const;
	//
	//# \desc
	//
	//# \also	$@ProjectionLightObject::GetProjectionName@$
	//# \also	$@ProjectionLightObject::GetTextureSize@$


	//# \function	ProjectionLightObject::GetProjectionName	Returns the name of the projected texture map.
	//
	//# \proto	const ResourceName& GetProjectionName(void) const;
	//
	//# \desc
	//
	//# \also	$@ProjectionLightObject::GetProjectionMap@$
	//# \also	$@ProjectionLightObject::GetTextureSize@$


	class ProjectionLightObject : public PointLightObject
	{
		private:

			Texture				*projectionMap;

			int32				textureSize;
			TextureFormat		textureFormat;
			ResourceName		projectionName;

		protected:

			ProjectionLightObject(LightType type);
			ProjectionLightObject(LightType type, const ColorRGB& color, float range, const char *name);
			ProjectionLightObject(const ProjectionLightObject& projectionLightObject);
			~ProjectionLightObject();

		public:

			Texture *const& GetProjectionMap(void) const
			{
				return (projectionMap);
			}

			int32 GetTextureSize(void) const
			{
				return (textureSize);
			}

			void SetTextureSize(int32 size)
			{
				textureSize = size;
			}

			TextureFormat GetTextureFormat(void) const
			{
				return (textureFormat);
			}

			void SetTextureFormat(TextureFormat format)
			{
				textureFormat = format;
			}

			const ResourceName& GetProjectionName(void) const
			{
				return (projectionName);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;

			int32 GetCategorySettingCount(Type category) const override;
			Setting *GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const override;
			void SetCategorySetting(Type category, const Setting *setting) override;

			virtual void SetProjectionMap(const char *name);
	};


	//# \class	CubeLightObject		Encapsulates data pertaining to a point light source having a projected cube texture map.
	//
	//# The $CubeLightObject$ class encapsulates data pertaining to a point light source having a projected cube texture map.
	//
	//# \def	class CubeLightObject final : public ProjectionLightObject
	//
	//# \ctor	CubeLightObject(const ColorRGB& color, float range, const char *name);
	//
	//# \param	color	The color of light emitted by the light source.
	//# \param	range	The spherical range of the light source.
	//# \param	name	The name of the projected texture map.
	//
	//# \desc
	//# A cube light source is a special type of point light that projects a cube texture onto the world in all directions.
	//# The $CubeLightObject$ class contains information specific to the cube light type.
	//
	//# \base	ProjectionLightObject		A $CubeLightObject$ is a specific type of projection light object.
	//
	//# \also	$@WorldMgr/CubeLight@$


	class CubeLightObject final : public ProjectionLightObject
	{
		friend class LightObject;

		private:

			CubeLightObject();
			CubeLightObject(const CubeLightObject& cubeLightObject);
			~CubeLightObject();

			Object *Replicate(void) const override;

		public:

			CubeLightObject(const ColorRGB& color, float range, const char *name);
	};


	//# \class	SpotLightObject		Encapsulates data pertaining to a spot light source having a projected 2D texture map.
	//
	//# The $SpotLightObject$ class encapsulates data pertaining to a spot light source having a projected 2D texture map.
	//
	//# \def	class SpotLightObject final : public ProjectionLightObject
	//
	//# \ctor	SpotLightObject(const ColorRGB& color, float range, float apex, const char *name);
	//
	//# \param	color	The color of light emitted by the light source.
	//# \param	range	The spherical range of the light source.
	//# \param	apex	The tangent of half the apex angle for the spot light. This determines the light's angle of illumination.
	//# \param	name	The name of the projected texture map.
	//
	//# \desc
	//# A spot light source is a special type of point light that projects a 2D texture onto the world in one direction.
	//# The $SpotLightObject$ class contains information specific to the spot light type.
	//
	//# \base	ProjectionLightObject		A $SpotLightObject$ is a specific type of projection light object.
	//
	//# \also	$@WorldMgr/SpotLight@$


	//# \function	SpotLightObject::GetApexTangent		Returns the tangent of half the apex angle for a spot light.
	//
	//# \proto	float GetApexTangent(void) const;
	//
	//# \desc
	//
	//# \also	$@SpotLightObject::SetApexTangent@$


	//# \function	SpotLightObject::SetApexTangent		Sets the tangent of half the apex angle for a spot light.
	//
	//# \proto	void SetApexTangent(float apex);
	//
	//# \param	apex	The tangent of half the apex angle for the spot light.
	//
	//# \desc
	//
	//# \also	$@SpotLightObject::GetApexTangent@$


	class SpotLightObject final : public ProjectionLightObject
	{
		friend class LightObject;

		private:

			float		apexTangent;
			float		aspectRatio;

			SpotLightObject();
			SpotLightObject(const SpotLightObject& spotLightObject);
			~SpotLightObject();

			Object *Replicate(void) const override;

			void CalculateAspectRatio(void);

		public:

			SpotLightObject(const ColorRGB& color, float range, float apex, const char *name);

			float GetApexTangent(void) const
			{
				return (apexTangent);
			}

			void SetApexTangent(float apex)
			{
				apexTangent = apex;
			}

			float GetAspectRatio(void) const
			{
				return (aspectRatio);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetObjectSize(float *size) const override;
			void SetObjectSize(const float *size) override;

			void SetProjectionMap(const char *name) override;
	};
}


#endif

// ZYUQURM
