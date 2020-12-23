 

#ifndef C4SpaceObjects_h
#define C4SpaceObjects_h


//# \component	World Manager
//# \prefix		WorldMgr/


#include "C4Objects.h"
#include "C4Volumes.h"
#include "C4Textures.h"


namespace C4
{
	typedef Type	SpaceType;
	typedef Type	FogFunction;


	//# \enum	SpaceType

	enum : SpaceType
	{
		kSpaceFog				= 'FOG ',		//## Fog space.
		kSpaceShadow			= 'SHAD',		//## Shadow space.
		kSpaceRadiosity			= 'RDSY',		//## Radiosity space.
		kSpaceAcoustics			= 'ACST',		//## Acoustics space.
		kSpaceOcclusion			= 'OCCL',		//## Occlusion space.
		kSpacePaint				= 'PANT',		//## Paint space.
		kSpacePhysics			= 'PHYS'		//## Physics space.
	};


	//# \enum	FogSpaceFlags

	enum
	{
		kFogSpaceDistanceOcclusion		= 1 << 0,	//## Apply distance occlusion parallel to the fog plane when the camera is inside the fog volume.
		kFogSpaceDepthOcclusion			= 1 << 1	//## Apply distance occlusion perpendicular to the fog plane for all camera positions.
	};


	//# \enum	FogFunction

	enum : FogFunction
	{
		kFogFunctionConstant			= 'CNST',	//## Use constant fog density.
		kFogFunctionLinear				= 'LINR'	//## Use linearly increasing fog density.
	};


	//# \enum	PaintImageFlags

	enum
	{
		kPaintImageMipmaps				= 1 << 0	//## Generate mipmaps for the paint image.
	};


	class Texture;


	//# \class	SpaceObject		Encapsulates data pertaining to a space.
	//
	//# The $SpaceObject$ class encapsulates data pertaining to a space.
	//
	//# \def	class SpaceObject : public Object, public VolumeObject
	//
	//# \ctor	SpaceObject(SpaceType type, Volume *volume);
	//
	//# The constructor has protected access. The $SpaceObject$ class can only exist as the base class for another class.
	//
	//# \param	type		The type of the space. See below for a list of possible types.
	//# \param	volume		A pointer to the generic volume object representing the space.
	//
	//# \desc
	//# The $SpaceObject$ class is the base class for all types of space objects, and it stores
	//# data that is common to all of them. Each specific type of space has an associated object
	//# that is a subclass of the $SpaceObject$ class.
	//#
	//# A space object can have one of the following types.
	//
	//# \table	SpaceType
	//
	//# \base		Object			A $SpaceObject$ is an object that can be shared by multiple space nodes.
	//# \privbase	VolumeObject	Used internally by the engine for generic volume objects.
	//
	//# \also	$@Space@$
	//
	//# \wiki	Spaces


	//# \function	SpaceObject::GetSpaceType		Returns the specific type of a space.
	//
	//# \proto	SpaceType GetSpaceType(void) const;
	//
	//# \desc
	//# The $GetSpaceType$ function returns the specific space type, which can be one of the following constants.
	//
	//# \table	SpaceType
 

	class SpaceObject : public Object, public VolumeObject
	{ 
		friend class Object;
 
		private:

			SpaceType	spaceType; 

			static SpaceObject *Create(Unpacker& data, unsigned_int32 unpackFlags); 
 
		protected:

			SpaceObject(SpaceType type, Volume *volume);
			~SpaceObject(); 

		public:

			SpaceType GetSpaceType(void) const
			{
				return (spaceType);
			}

			void PackType(Packer& data) const override;
			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetObjectSize(float *size) const override;
			void SetObjectSize(const float *size) override;
	};


	//# \class	FogSpaceObject		Encapsulates data pertaining to a fog space.
	//
	//# The $FogSpaceObject$ class encapsulates data pertaining to a fog space.
	//
	//# \def	class FogSpaceObject : public SpaceObject, public PlateVolume
	//
	//# \ctor	FogSpaceObject(const Vector2D& size);
	//
	//# \param	size	The size of the plate.
	//
	//# \desc
	//
	//# \base	SpaceObject		A $FogSpaceObject$ is an object that can be shared by multiple fog space nodes.
	//# \base	PlateVolume		A $FogSpaceObject$ is represented by a generic plate volume.
	//
	//# \also	$@FogSpace@$


	//# \function	FogSpaceObject::GetFogSpaceFlags	Returns the fog space flags.
	//
	//# \proto	unsigned_int32 GetFogSpaceFlags(void) const;
	//
	//# \desc
	//# The $GetFogSpaceFlags$ function returns the fog space flags, which can be a combination (through logical OR) of the following constants.
	//
	//# \table	FogSpaceFlags
	//
	//# \also	$@FogSpaceObject::SetFogSpaceFlags@$


	//# \function	FogSpaceObject::SetFogSpaceFlags	Sets the fog space flags.
	//
	//# \proto	void SetFogSpaceFlags(unsigned_int32 flags);
	//
	//# \param	flags	The new fog flags.
	//
	//# \desc
	//# The $SetFogSpaceFlags$ function sets the fog space flags to the value specified by the $flags$ parameter,
	//# which can be a combination (through logical OR) of the following constants.
	//
	//# \table	FogSpaceFlags
	//
	//# \also	$@FogSpaceObject::GetFogSpaceFlags@$


	//# \function	FogSpaceObject::GetFogColor		Returns the fog color.
	//
	//# \proto	const ColorRGBA& GetFogColor(void) const;
	//
	//# \desc
	//# The $GetFogColor$ function returns the fog color. The alpha channel of the color is not used.
	//
	//# \also	$@FogSpaceObject::SetFogColor@$


	//# \function	FogSpaceObject::SetFogColor		Sets the fog color.
	//
	//# \proto	void SetFogColor(const ColorRGBA& color);
	//
	//# \param	color	The new fog color.
	//
	//# \desc
	//# The $SetFogColor$ function sets the fog color to that specified by the $color$ parameter.
	//# The alpha channel of the color is not used and should be set to 1.0.
	//
	//# \also	$@FogSpaceObject::GetFogColor@$


	//# \function	FogSpaceObject::GetFogDensity		Returns the fog density.
	//
	//# \proto	float GetFogDensity(void) const;
	//
	//# \desc
	//# The $GetFogDensity$ function returns the fog density. See the $@FogSpaceObject::SetFogFunction@$ function for
	//# a description of how the density is used in calculating fog effects.
	//
	//# \also	$@FogSpaceObject::SetFogDensity@$


	//# \function	FogSpaceObject::SetFogDensity		Sets the fog density.
	//
	//# \proto	void SetFogDensity(float density);
	//
	//# \param	density		The new fog density.
	//
	//# \desc
	//# The $SetFogDensity$ function sets the fog density to the value specified by the $density$ parameter. See the
	//# $@FogSpaceObject::SetFogFunction@$ function for a description of how the density is used in calculating fog effects.
	//
	//# \also	$@FogSpaceObject::GetFogDensity@$


	//# \function	FogSpaceObject::GetFogFunction		Returns the fog function.
	//
	//# \proto	FogFunction GetFogFunction(void) const;
	//
	//# \desc
	//# The $GetFogFunction$ function returns the fog function, which can be one of the following constants.
	//
	//# \table	FogFunction
	//
	//# See the $@FogSpaceObject::SetFogFunction@$ function for a description of how fog functions are used in calculating fog effects.
	//
	//# \also	$@FogSpaceObject::SetFogFunction@$


	//# \function	FogSpaceObject::SetFogFunction		Sets the fog function.
	//
	//# \proto	void SetFogFunction(FogFunction function);
	//
	//# \param	function	The new fog function.
	//
	//# \desc
	//# The $SetFogFunction$ function sets the fog function to that specified by the $function$ parameter,
	//# which can be one of the following constants.
	//
	//# \table	FogFunction
	//
	//# If the fog function is $kFogFunctionConstant$, then the density of the fog is constant everywhere beneath the fog plane.
	//# (This density is specified using the $@FogSpaceObject::SetFogDensity@$ function.) If the fog function is $kFogFunctionLinear$,
	//# then the fog density is given by <i>&rho;z</i>, where <i>&rho;</i> is the fog density, and <i>z</i> is the distance beneath
	//# the fog plane.
	//
	//# \also	$@FogSpaceObject::GetFogFunction@$
	//# \also	$@FogSpaceObject::SetFogDensity@$


	class FogSpaceObject : public SpaceObject, public PlateVolume
	{
		friend class SpaceObject;

		private:

			unsigned_int32		fogSpaceFlags;

			ColorRGBA			fogColor;
			float				fogDensity;
			FogFunction			fogFunction;

			float				occlusionIntensity;
			float				occlusionConstant;

			FogSpaceObject();
			~FogSpaceObject();

		public:

			FogSpaceObject(const Vector2D& size);

			unsigned_int32 GetFogSpaceFlags(void) const
			{
				return (fogSpaceFlags);
			}

			void SetFogSpaceFlags(unsigned_int32 flags)
			{
				fogSpaceFlags = flags;
			}

			const ColorRGBA& GetFogColor(void) const
			{
				return (fogColor);
			}

			void SetFogColor(const ColorRGBA& color)
			{
				fogColor = color;
			}

			float GetFogDensity(void) const
			{
				return (fogDensity);
			}

			void SetFogDensity(float density)
			{
				fogDensity = density;
			}

			FogFunction GetFogFunction(void) const
			{
				return (fogFunction);
			}

			void SetFogFunction(FogFunction function)
			{
				fogFunction = function;
			}

			float GetOcclusionIntensity(void) const
			{
				return (occlusionIntensity);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetCategoryCount(void) const override;
			Type GetCategoryType(int32 index, const char **title) const override;
			int32 GetCategorySettingCount(Type category) const override;
			Setting *GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const override;
			void SetCategorySetting(Type category, const Setting *setting) override;

			void SetOcclusionIntensity(float intensity);
			float CalculateOcclusionDistance(float F_wedge_C) const;
			float CalculateOcclusionDepth(float F_wedge_C) const;
	};


	//# \class	ShadowSpaceObject	Encapsulates data pertaining to a shadow space.
	//
	//# The $ShadowSpaceObject$ class encapsulates data pertaining to a shadow space.
	//
	//# \def	class ShadowSpaceObject : public SpaceObject, public BoxVolume
	//
	//# \ctor	ShadowSpaceObject(const Vector3D& size);
	//
	//# \param	size	The size of the box.
	//
	//# \desc
	//
	//# \base	SpaceObject		A $ShadowSpaceObject$ is an object that can be shared by multiple shadow space nodes.
	//# \base	BoxVolume		A $ShadowSpaceObject$ is represented by a generic box volume.
	//
	//# \also	$@ShadowSpace@$


	class ShadowSpaceObject : public SpaceObject, public BoxVolume
	{
		friend class SpaceObject;

		private:

			ShadowSpaceObject();
			~ShadowSpaceObject();

		public:

			ShadowSpaceObject(const Vector3D& size);
	};


	//# \class	RadiositySpaceObject		Encapsulates data pertaining to a radiosity space.
	//
	//# The $RadiositySpaceObject$ class encapsulates data pertaining to a radiosity space.
	//
	//# \def	class RadiositySpaceObject : public SpaceObject, public BoxVolume
	//
	//# \ctor	RadiositySpaceObject(const Vector3D& size, const Integer3D& resolution);
	//
	//# \param	size			The size of the box bounding the radiosity space.
	//# \param	resolution		The resolution of the volumetric lighting texture.
	//
	//# \desc
	//
	//# \base	SpaceObject		A $RadiositySpaceObject$ is an object that can be shared by multiple radiosity space nodes.
	//# \base	BoxVolume		A $RadiositySpaceObject$ is represented by a generic box volume.


	class RadiositySpaceObject : public SpaceObject, public BoxVolume
	{
		friend class SpaceObject;

		private:

			Integer3D			radiosityResolution;
			int32				imageVoxelCount;

			float				intensityScale;
			float				saturationScale;

			Vector3D			texcoordScale;
			Vector3D			texcoordOffset;

			Color4C				*radiosityImage[2];
			Texture				*radiosityTexture[2];
			TextureHeader		textureHeader[2];

			RadiositySpaceObject();
			~RadiositySpaceObject();

			void UpdateRadiosityResolution(void);

			void CreateRadiosityImages(void);
			void CreateRadiosityTextures(void);

		public:

			enum
			{
				kMaxRadiositySpaceResolution = 128
			};

			RadiositySpaceObject(const Vector3D& size, const Integer3D& resolution);

			const Integer3D& GetRadiosityResolution(void) const
			{
				return (radiosityResolution);
			}

			float GetIntensityScale(void) const
			{
				return (intensityScale);
			}

			void SetIntensityScale(float scale)
			{
				intensityScale = scale;
			}

			float GetSaturationScale(void) const
			{
				return (saturationScale);
			}

			void SetSaturationScale(float scale)
			{
				saturationScale = scale;
			}

			const Vector3D& GetTexcoordScale(void) const
			{
				return (texcoordScale);
			}

			const Vector3D& GetTexcoordOffset(void) const
			{
				return (texcoordOffset);
			}

			Color4C *GetRadiosityImage(int32 index) const
			{
				return (radiosityImage[index]);
			}

			const Texture *GetRadiosityTexture(int32 index) const
			{
				return (radiosityTexture[index]);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;

			int32 GetCategoryCount(void) const override;
			Type GetCategoryType(int32 index, const char **title) const override;
			int32 GetCategorySettingCount(Type category) const override;
			Setting *GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const override;
			void SetCategorySetting(Type category, const Setting *setting) override;

			void *BeginSettings(void) override;
			void EndSettings(void *cookie) override;

			C4API void ClearTextureData(void) const;
			C4API void UpdateTextureData(void) const;
	};


	//# \class	AcousticsSpaceObject	Encapsulates data pertaining to an acoustic space.
	//
	//# The $AcousticsSpaceObject$ class encapsulates data pertaining to an acoustic space.
	//
	//# \def	class AcousticsSpaceObject : public SpaceObject, public BoxVolume
	//
	//# \ctor	AcousticsSpaceObject(const Vector3D& size);
	//
	//# \param	size	The size of the box.
	//
	//# \desc
	//
	//# \base	SpaceObject		An $AcousticsSpaceObject$ is an object that can be shared by multiple acoustic space nodes.
	//# \base	BoxVolume		An $AcousticsSpaceObject$ is represented by a generic box volume.
	//
	//# \also	$@AcousticsSpace@$


	class AcousticsSpaceObject : public SpaceObject, public BoxVolume
	{
		friend class SpaceObject;

		private:

			float		reflectionVolume;
			float		reflectionHFVolume;
			float		reverbDecayTime;
			float		mediumHFAbsorption;

			AcousticsSpaceObject();
			~AcousticsSpaceObject();

		public:

			AcousticsSpaceObject(const Vector3D& size);

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

			float GetReverbDecayTime(void) const
			{
				return (reverbDecayTime);
			}

			void SetReverbDecayTime(float time)
			{
				reverbDecayTime = time;
			}

			float GetMediumHFAbsorption(void) const
			{
				return (mediumHFAbsorption);
			}

			void SetMediumHFAbsorption(float absorption)
			{
				mediumHFAbsorption = absorption;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetCategoryCount(void) const override;
			Type GetCategoryType(int32 index, const char **title) const override;
			int32 GetCategorySettingCount(Type category) const override;
			Setting *GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const override;
			void SetCategorySetting(Type category, const Setting *setting) override;
	};


	//# \class	OcclusionSpaceObject	Encapsulates data pertaining to an occlusion space.
	//
	//# The $OcclusionSpaceObject$ class encapsulates data pertaining to an occlusion space.
	//
	//# \def	class OcclusionSpaceObject : public SpaceObject, public BoxVolume
	//
	//# \ctor	OcclusionSpaceObject(const Vector3D& size);
	//
	//# \param	size	The size of the box.
	//
	//# \desc
	//
	//# \base	SpaceObject		An $OcclusionSpaceObject$ is an object that can be shared by multiple occlusion space nodes.
	//# \base	BoxVolume		An $OcclusionSpaceObject$ is represented by a generic box volume.
	//
	//# \also	$@OcclusionSpace@$


	class OcclusionSpaceObject : public SpaceObject, public BoxVolume
	{
		friend class SpaceObject;

		private:

			OcclusionSpaceObject();
			~OcclusionSpaceObject();

		public:

			OcclusionSpaceObject(const Vector3D& size);
	};


	//# \class	PaintSpaceObject	Encapsulates data pertaining to a paint space.
	//
	//# The $PaintSpaceObject$ class encapsulates data pertaining to a paint space.
	//
	//# \def	class PaintSpaceObject : public SpaceObject, public BoxVolume
	//
	//# \ctor	PaintSpaceObject(const Vector3D& size, const Integer2D& resolution);
	//
	//# \param	size		The size of the box.
	//# \param	resolution	The resolution of the paint texture. This must be a power of two between the values of $kPaintMinResolution$ and $kPaintMaxResolution$, inclusive.
	//
	//# \desc
	//
	//# \base	SpaceObject		A $PaintSpaceObject$ is an object that can be shared by multiple paint space nodes.
	//# \base	BoxVolume		A $PaintSpaceObject$ is represented by a generic box volume.
	//
	//# \also	$@PaintSpace@$


	class PaintSpaceObject : public SpaceObject, public BoxVolume
	{
		friend class SpaceObject;

		private:

			struct PaintImageData
			{
				unsigned_int32		imageFlags;
				Integer2D			paintResolution;
				int32				channelCount;
			};

			PaintImageData		paintImageData;
			int32				imagePixelCount;
			int32				imageMipmapCount;

			unsigned_int8		*paintImage;
			Texture				*paintTexture;
			TextureHeader		textureHeader;

			int32				preprocessCount;

			PaintSpaceObject();
			~PaintSpaceObject();

			void GenerateMipmaps(void);
			void CreatePaintImage(void);
			void CreatePaintTexture(void);

		public:

			PaintSpaceObject(const Vector3D& size, const Integer2D& resolution, int32 count);

			unsigned_int32 GetPaintImageFlags(void) const
			{
				return (paintImageData.imageFlags);
			}

			void SetPaintImageFlags(unsigned_int32 flags)
			{
				paintImageData.imageFlags = flags;
			}

			const Integer2D& GetPaintResolution(void) const
			{
				return (paintImageData.paintResolution);
			}

			void SetPaintResolution(const Integer2D& resolution)
			{
				paintImageData.paintResolution = resolution;
			}

			int32 GetChannelCount(void) const
			{
				return (paintImageData.channelCount);
			}

			void SetChannelCount(int32 count)
			{
				paintImageData.channelCount = count;
			}

			void *GetPaintImage(void) const
			{
				return (paintImage);
			}

			const Texture *const *GetPaintTexturePointer(void) const
			{
				return (&paintTexture);
			}

			Texture *GetPaintTexture(void) const
			{
				return (paintTexture);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;

			int32 GetCategoryCount(void) const override;
			Type GetCategoryType(int32 index, const char **title) const override;
			int32 GetCategorySettingCount(Type category) const override;
			Setting *GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const override;
			void SetCategorySetting(Type category, const Setting *setting) override;

			void *BeginSettings(void) override;
			void EndSettings(void *cookie) override;

			void Preprocess(void);
			void Neutralize(void);

			C4API void UpdatePaintTexture(const Rect& bounds);
	};


	//# \class	PhysicsSpaceObject		Encapsulates data pertaining to a physics space.
	//
	//# The $PhysicsSpaceObject$ class encapsulates data pertaining to a physics space.
	//
	//# \def	class PhysicsSpaceObject : public SpaceObject, public BoxVolume
	//
	//# \ctor	PhysicsSpaceObject(const Vector3D& size);
	//
	//# \param	size	The size of the box.
	//
	//# \desc
	//
	//# \base	SpaceObject		A $PhysicsSpaceObject$ is an object that can be shared by multiple physics space nodes.
	//# \base	BoxVolume		A $PhysicsSpaceObject$ is represented by a generic box volume.
	//
	//# \also	$@PhysicsSpace@$


	class PhysicsSpaceObject : public SpaceObject, public BoxVolume
	{
		friend class SpaceObject;

		private:

			PhysicsSpaceObject();
			~PhysicsSpaceObject();

		public:

			PhysicsSpaceObject(const Vector3D& size);
	};
}


#endif

// ZYUQURM
