 

#ifndef C4Textures_h
#define C4Textures_h


//# \component	Graphics Manager
//# \prefix		GraphicsMgr/


#include "C4Resources.h"
#include "C4Packing.h"
#include "C4Compression.h"
#include "C4Image.h"
#include "C4Render.h"


namespace C4
{
	typedef Type	TextureType;
	typedef Type	TextureWrap;
	typedef Type	TextureFormat;
	typedef Type	TextureSemantic;
	typedef Type	CompressionType;


	enum
	{
		kMaxResourceTextureCount	= 3
	};


	//# \enum	TextureFlags

	enum
	{
		kTextureDynamic						= 1 << 0,
		kTextureRenderTarget				= 1 << 1,
		kTextureExternalStorage				= 1 << 2,
		kTextureSrgbColor					= 1 << 7,		//## The color values are encoded in sRGB space (ignored for 1-channel and 2-channel textures).
		kTextureFilterInhibit				= 1 << 8,		//## Do not filter the texture (bilinear, trilinear, or anisotropic).
		kTextureAnisotropicFilterInhibit	= 1 << 9,		//## Do not apply anisotropic filtering to the texture (but still allow ordinary bilinear or trilinear filtering).
		kTextureForceHighQuality			= 1 << 11,		//## Always use the highest resolution image available, even if a lower quality level is selected in the Graphics Manager.
		kTextureImagePalette				= 1 << 16,
		kTextureReferenceList				= 1 << 17
	};


	//# \enum	TextureType

	enum : TextureType
	{
		kTexture2D						= '2D2D',		//## 2D texture map.
		kTexture3D						= '3D3D',		//## 3D texture map.
		kTextureCube					= 'CUBE',		//## Cube texture map.
		kTextureRectangle				= 'RECT',		//## Rectangle texture map.
		kTextureArray2D					= 'AR2D'		//## Array 2D texture map.
	};


	//# \enum	TextureWrap

	enum : TextureWrap
	{
		kTextureRepeat					= 'REPT',		//## Repeat.
		kTextureClamp					= 'CLMP',		//## Clamp to edge.
		kTextureClampBorder				= 'CBDR',		//## Clamp to border.
		kTextureMirrorRepeat			= 'MRPT',		//## Mirror and repeat.
		kTextureMirrorClamp				= 'MCLP',		//## Mirror once and clamp to edge.
		kTextureMirrorClampBorder		= 'MCBD'		//## Mirror once and clamp to border.
	};


	//# \enum	TextureFormat

	enum : TextureFormat
	{
		kTextureRGBA8					= 'RGBA',		//## 32-bit RGBA.
		kTextureBGRA8					= 'BGRA',		//## 32-bit BGRA.
		kTextureRG8						= 'RDGN',		//## 16-bit red-green.
		kTextureR8						= 'RED ',		//## 8-bit red.
		kTextureLA8						= 'LMAL',		//## 16-bit luminance-alpha.
		kTextureL8						= 'LUMI',		//## 8-bit luminance.
		kTextureI8						= 'INTS',		//## 8-bit intensity.
		kTextureBC13					= 'S3TC',		//## BC1 or BC3 compressed.
		kTextureStorageCount			= 9
	};


	//# \enum	TextureSemantic

	enum : TextureSemantic
	{
		kTextureSemanticNone			= 0,			//## No meaning.
		kTextureSemanticData			= 'DATA',		//## Generic data.
		kTextureSemanticDiffuse			= 'DIFF',		//## Diffuse color map.
		kTextureSemanticSpecular		= 'GLOS',		//## Specular color map.
		kTextureSemanticEmission		= 'EMIS',		//## Emission color map.
		kTextureSemanticOpacity			= 'OPAC',		//## Refractive opacity map.
		kTextureSemanticEnvironment		= 'ENVR',		//## Environment map.
		kTextureSemanticLight			= 'LGHT',		//## Light projection.
		kTextureSemanticShadow			= 'SHAD',
		kTextureSemanticNormal			= 'NRML',		//## Normal map. 
		kTextureSemanticParallax		= 'PLAX',		//## Parallax data.
		kTextureSemanticHorizon			= 'HRZN',		//## Horizon map.
		kTextureSemanticRadiosity		= 'RDSY', 
		kTextureSemanticVector			= 'VECT',
		kTextureSemanticAttenuation		= 'ATTN', 
		kTextureSemanticTransparency	= 'ALFA',		//## Transparency.
		kTextureSemanticOcclusion		= 'OCCL',		//## Ambient occlusion.
		kTextureSemanticGlow			= 'GLOW',		//## Glow intensity. 
		kTextureSemanticLuminance		= 'LUMI',
		kTextureSemanticChrominance		= 'CHRM' 
	}; 


	enum : CompressionType
	{ 
		kTextureCompressionNone			= 0,
		kTextureCompressionGeneral		= 'COMP',
		kTextureCompressionRunLength	= 'CRLE'
	};


	enum : unsigned_int32
	{
		kTextureChunkParallaxScale		= 'PLAX',
		kTextureChunkImageCenter		= 'CENT',
		kTextureChunkPaletteSize		= 'PLTS',
		kTextureChunkImpostorSize		= 'IMPS',
		kTextureChunkImpostorClipData	= 'IMPC',
		kTextureChunkReferenceList		= 'REFR'
	};


	class Texture;
	struct TextureStorageData;


	struct TextureMipmapData
	{
		int32				imageOffset;
		unsigned_int32		imageSize;
		unsigned_int32		chainSize;
		CompressionType		compressionType;

		void *GetMipmapImage(void)
		{
			return (reinterpret_cast<char *>(this) + imageOffset);
		}

		const void *GetMipmapImage(void) const
		{
			return (reinterpret_cast<const char *>(this) + imageOffset);
		}
	};


	//# \struct	TextureHeader		Contains information about a texture map.
	//
	//# The $TextureHeader$ structure contains information about a texture map.
	//
	//# \def	struct TextureHeader
	//
	//# \data	TextureHeader
	//
	//# \desc
	//# The $TextureHeader$ structure contains information about a texture map and is useful when
	//# creating a texture image in memory. A texture header can be filled in and passed to the
	//# $@Texture::Get@$ function to create a new texture object.
	//#
	//# The $textureType$ member can be one of the following constants.
	//
	//# \table	TextureType
	//
	//# The $textureFlags$ member can be a combination (through logical OR) of the following constants.
	//
	//# \table	TextureFlags
	//
	//# The $colorSemantic$ and $alphaSemantic$ members denote the type of information that is stored
	//# in the color channels and the alpha channel of the texture image, respectively. They can each
	//# be one of the following constants.
	//
	//# \table	TextureSemantic
	//
	//# The $imageFormat$ member can be one of the following constants.
	//
	//# \table	TextureFormat
	//
	//# The $wrapMode$ array contains the wrap mode for each of the three texture mapping axes. They
	//# can each be one of the following constants unless $textureType$ is $kTextureRectangle$, in which
	//# case the wrap modes must all be $kTextureClamp$.
	//
	//# \table	TextureWrap
	//
	//# \also	$@Texture::Get@$


	//# \member		TextureHeader

	struct TextureHeader
	{
		TextureType			textureType;			//## The texture type (1D, 2D, 3D, cube, rectangle, or 2D array).
		unsigned_int32		textureFlags;			//## The texture image flags.

		TextureSemantic		colorSemantic;			//## The semantic usage for the color channels.
		TextureSemantic		alphaSemantic;			//## The semantic usage for the alpha channel.
		TextureFormat		imageFormat;			//## The format of the texture image.

		int32				imageWidth;				//## The width of the texture map.
		int32				imageHeight;			//## The height of the texture map.
		int32				imageDepth;				//## The depth of the texture map (value is 1 for a 2D texture).

		TextureWrap			wrapMode[3];			//## The wrap mode for each coordinate.

		int32				mipmapCount;			//## The number of mipmap levels.
		int32				mipmapDataOffset;		//## The offset to the mipmap data array, in bytes relative to the beginning of the $TextureHeader$ structure. This should be 0 for textures created in memory.

		unsigned_int32		auxiliaryDataSize;		//## The size of the auxiliary data, in bytes. This should be 0 for textures created in memory.
		int32				auxiliaryDataOffset;	//## The offset to the auxiliary data, in bytes relative to the beginning of the $TextureHeader$ structure. This should be 0 for textures created in memory.

		TextureMipmapData *GetMipmapData(void)
		{
			return (reinterpret_cast<TextureMipmapData *>(reinterpret_cast<char *>(this) + mipmapDataOffset));
		}

		const TextureMipmapData *GetMipmapData(void) const
		{
			return (reinterpret_cast<const TextureMipmapData *>(reinterpret_cast<const char *>(this) + mipmapDataOffset));
		}

		ChunkHeader *GetAuxiliaryData(void)
		{
			return (reinterpret_cast<ChunkHeader *>(reinterpret_cast<char *>(this) + auxiliaryDataOffset));
		}

		const ChunkHeader *GetAuxiliaryData(void) const
		{
			return (reinterpret_cast<const ChunkHeader *>(reinterpret_cast<const char *>(this) + auxiliaryDataOffset));
		}
	};


	struct TextureResourceHeader
	{
		int32				endian;
		unsigned_int32		headerDataSize;
		int32				textureCount;

		const TextureHeader *GetTextureHeader(int32 index = 0) const
		{
			return (&reinterpret_cast<const TextureHeader *>(this + 1)[index]);
		}
	};


	class TextureResource : public Resource<TextureResource>
	{
		friend class Resource<TextureResource>;

		private:

			static C4API ResourceDescriptor		descriptor;

			Texture		*owningTexture[kMaxResourceTextureCount];

			~TextureResource();

		public:

			C4API TextureResource(const char *name, ResourceCatalog *catalog);

			Texture *GetOwningTexture(int32 index) const
			{
				return (owningTexture[index]);
			}

			void SetOwningTexture(int32 index, Texture *texture)
			{
				owningTexture[index] = texture;
			}

			const TextureResourceHeader *GetTextureResourceHeader(void) const
			{
				return (static_cast<const TextureResourceHeader *>(GetData()));
			}

			const TextureHeader *GetTextureHeader(void) const
			{
				return (reinterpret_cast<const TextureHeader *>(GetTextureResourceHeader() + 1));
			}

			C4API ResourceResult LoadHeaderData(ResourceLoader *loader, TextureResourceHeader *resourceHeader, TextureHeader **textureHeader) const;
			ResourceResult LoadImageData(ResourceLoader *loader, const TextureResourceHeader *resourceHeader, const TextureHeader *textureHeader, int32 index, int32 level, void **imageData) const;

			C4API static void ReleaseHeaderData(TextureHeader *textureHeader);
			static void ReleaseImageData(void *imageData);
	};


	//# \class	Texture		Encapsulates a texture map.
	//
	//# The $Texture$ class encapsulates a texture map.
	//
	//# \def	class Texture : public Render::TextureObject, public Shared, public ListElement<Texture>
	//
	//# \ctor	Texture(TextureResource *resource, int32 index);
	//# \ctor	Texture(const TextureHeader *header, const void *image = nullptr);
	//
	//# \param	name	The name of the texture resource.
	//# \param	index	The index of the texture within the texture resource.
	//# \param	header	A pointer to a $@TextureHeader@$ structure describing the texture.
	//# \param	image	A pointer to the pixel image for the texture. If this is $nullptr$, then the location of the image is given by the information in the texture header.
	//
	//# \desc
	//# The $Texture$ class encapsulates a texture map that is either stored in a resource or created in memory.
	//# Since textures are reference-counted shared objects that could be used by multiple parts of the engine, they
	//# cannot be created using the $new$ operator. They should instead be created by calling the $@Texture::Get@$ function.
	//#
	//# A texture should be released by calling the $@Utilities/Shared::Release@$ function.
	//
	//# \privbase	Render::TextureObject			Used internally by the Graphics Manager.
	//# \base		Utilities/Shared				Texture objects are reference counted.
	//# \base		Utilities/ListElement<Texture>	Used internally by the Graphics Manager.


	//# \function	Texture::Get		Returns a reference to a texture object.
	//
	//# \proto	static Texture *Get(const char *name, int32 index = 0);
	//# \proto	static Texture *Get(const TextureHeader *header, const void *image = nullptr);
	//
	//# \param	name	The name of the texture resource.
	//# \param	index	The index of the texture within the texture resource.
	//# \param	header	A pointer to a $@TextureHeader@$ structure describing the texture.
	//# \param	image	A pointer to the pixel image for the texture. If this is $nullptr$, then the texture image should immediately follow the texture header in memory.
	//
	//# \desc
	//# The $Get$ function returns a reference to a texture map object. A texture map may be stored in a resource
	//# or created in memory, and the location determines which version of the $Get$ function should be called.
	//# If the texture is stored in a resource, then the version of the $Get$ function taking the name of the
	//# resource in the $name$ parameter should be called. If the texture image exists in memory, then the
	//# version of the $Get$ function taking a $@TextureHeader@$ structure should be called.
	//#
	//# If a texture object already exists for the parameters passed to the $Get$ function, then its reference
	//# count is incremented, and a pointer to the existing object is returned. Otherwise, a new texture object
	//# is created with a reference count of 1, and a pointer to the new object is returned. If an attempt is
	//# made to create a texture object for a resource that does not exist, then the return value is a pointer
	//# to a special "missing" texture map.
	//#
	//# When the $header$ and $image$ parameters are used to create a texture map from data stored in memory,
	//# the header and image data must continue to exist in memory for the lifetime of the texture object returned
	//# by the $Get$ function.
	//#
	//# For each successful call to the $Get$ function, there should be a balancing call to the $@Utilities/Shared::Release@$
	//# function that releases the texture object.
	//
	//# \also	$@TextureHeader@$
	//# \also	$@Utilities/Shared::Release@$


	//# \function	Texture::GetTextureWidth		Returns the width of a texture map.
	//
	//# \proto	int32 GetTextureWidth(void) const;
	//
	//# \desc
	//# The $GetTextureWidth$ function returns the width of a texture map, in pixels.
	//
	//# \also	$@Texture::GetTextureHeight@$


	//# \function	Texture::GetTextureHeight		Returns the height of a texture map.
	//
	//# \proto	int32 GetTextureHeight(void) const;
	//
	//# \desc
	//# The $GetTextureHeight$ function returns the height of a texture map, in pixels.
	//
	//# \also	$@Texture::GetTextureWidth@$


	//# \function	Texture::GetTextureHeader		Returns the $@TextureHeader@$ data structure for a texture object.
	//
	//# \proto	const TextureHeader *GetTextureHeader(void) const;
	//
	//# \desc
	//# The $GetTextureHeader$ function returns a pointer to the texture header for a texture object created from
	//# an image in memory. The return value is the pointer that was passed to the $header$ parameter of the
	//# $@Texture::Get@$ function. If the texture object was created from a resource, then the return value is $nullptr$.
	//
	//# \also	$@TextureHeader@$
	//# \also	$@Texture::GetImagePointer@$


	//# \function	Texture::GetImagePointer		Returns a pointer to the image data for a texture object.
	//
	//# \proto	void *GetImagePointer(void) const;
	//
	//# \desc
	//# The $GetImagePointer$ function returns a pointer to the texture image for a texture object created from
	//# an image in memory. The return value is the pointer that was passed to the $image$ parameter of the
	//# $@Texture::Get@$ function. If the texture object was created from a resource, then the return value is $nullptr$.
	//
	//# \also	$@Texture::GetTextureHeader@$


	class Texture : public Render::TextureObject, public Shared, public ListElement<Texture>, public MapElement<Texture>
	{
		public:

			struct TextureInfo
			{
				const TextureHeader		*textureHeader;
				const void				*imagePointer;

				bool operator ==(const TextureInfo& info) const
				{
					return ((textureHeader == info.textureHeader) && (imagePointer == info.imagePointer));
				}

				bool operator <(const TextureInfo& info) const
				{
					return ((textureHeader < info.textureHeader) || ((textureHeader == info.textureHeader) && (imagePointer < info.imagePointer)));
				}

				bool operator >(const TextureInfo& info) const
				{
					return ((textureHeader > info.textureHeader) || ((textureHeader == info.textureHeader) && (imagePointer > info.imagePointer)));
				}
			};

			typedef TextureInfo KeyType;

		private:

			TextureType				textureType;
			unsigned_int32			textureFlags;
			int32					textureWidth;
			int32					textureHeight;
			int32					textureDepth;

			unsigned_int16			baseMipmapLevel;
			unsigned_int16			mipmapLevelCount;

			TextureSemantic			alphaSemantic;
			TextureFormat			imageFormat;

			float					floatTextureData[6];
			unsigned_int32			integerTextureData[2];

			bool					activeFlag;
			bool					impostorClipFlag;

			TextureResource			*textureResource;
			int32					textureIndex;

			TextureInfo				textureInfo;
			int32					imagePointerOffset;

			unsigned_int32			textureMemorySize;

			static int32			totalTextureCount;
			static unsigned_int32	totalTextureMemory;

			static Mutex			textureMutex;
			static List<Texture>	textureList;
			static Map<Texture>		textureHeaderMap;

			Texture(TextureResource *resource, int32 index);
			Texture(const TextureHeader *header, const void *image);
			~Texture();

			unsigned_int32 GetTextureTarget(void);
			static const TextureStorageData *GetTextureStorageData(TextureFormat imageFormat);

			static unsigned_int32 GetTextureWrapMode(TextureWrap mode);
			void InitializeTextureObject(const TextureHeader *header);
			void TerminateTextureObject(const void *);

			const void *ProcessAuxiliaryData(const TextureHeader *header);
			void Activate(const TextureHeader *header, const void *image);

			ResourceResult Activate(void);
			void Deactivate(void);

			ResourceResult LoadReferencedArrayImage(const char *name, unsigned_int8 *finalImage, int32 entryIndex, int32 entryCount) const;

		public:

			const KeyType& GetKey(void) const
			{
				return (textureInfo);
			}

			TextureType GetTextureType(void) const
			{
				return (textureType);
			}

			unsigned_int32 GetTextureFlags(void) const
			{
				return (textureFlags);
			}

			int32 GetTextureWidth(void) const
			{
				return (textureWidth);
			}

			int32 GetTextureHeight(void) const
			{
				return (textureHeight);
			}

			int32 GetTextureDepth(void) const
			{
				return (textureDepth);
			}

			TextureSemantic GetAlphaSemantic(void) const
			{
				return (alphaSemantic);
			}

			TextureFormat GetImageFormat(void) const
			{
				return (imageFormat);
			}

			const Vector2D& GetParallaxScale(void) const
			{
				return (*reinterpret_cast<const Vector2D *>(floatTextureData));
			}

			const Point2D& GetImageCenter(void) const
			{
				return (*reinterpret_cast<const Point2D *>(floatTextureData));
			}

			const unsigned_int32 *GetPaletteSize(void) const
			{
				return (integerTextureData);
			}

			const Vector2D& GetImpostorSize(void) const
			{
				return (*reinterpret_cast<const Vector2D *>(floatTextureData));
			}

			const float *GetImpostorClipData(void) const
			{
				return (&floatTextureData[2]);
			}

			bool GetImpostorClipFlag(void) const
			{
				return (impostorClipFlag);
			}

			const TextureResource *GetTextureResource(void) const
			{
				return (textureResource);
			}

			const TextureHeader *GetTextureHeader(void) const
			{
				return (textureInfo.textureHeader);
			}

			const void *GetImagePointer(void) const
			{
				return (textureInfo.imagePointer);
			}

			void SetImagePointerOffset(int32 offset)
			{
				imagePointerOffset = offset;
			}

			static int32 GetTotalTextureCount(void)
			{
				return (totalTextureCount);
			}

			static unsigned_int32 GetTotalTextureMemory(void)
			{
				return (totalTextureMemory);
			}

			C4API static Texture *Get(const char *name, int32 index = 0);
			C4API static Texture *Get(const TextureHeader *header, const void *image = nullptr);

			C4API static Render::Decompressor *GetDecompressor(const TextureHeader *textureHeader, const TextureMipmapData *mipmapData);

			C4API void UpdateImage(void);

			C4API void UpdateRect(const Rect& rect);
			C4API void UpdateRect(const Rect& rect, int32 pitch, const void *image);

			static void DeactivateAll(void);
			static void ReactivateAll(void);
			C4API static void Reload(const char *name);
	};


	class TextureArray : public Render::TextureArrayObject
	{
		public:

			TextureArray()
			{
				Construct();
			}

			~TextureArray()
			{
				Destruct();
			}
	};
}


#endif

// ZYUQURM
