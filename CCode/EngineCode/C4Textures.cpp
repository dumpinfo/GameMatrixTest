 

#include "C4Textures.h"
#include "C4Graphics.h"


using namespace C4;


ResourceDescriptor TextureResource::descriptor("tex", 0, 0, "C4/missing");


int32 Texture::totalTextureCount = 0;
unsigned_int32 Texture::totalTextureMemory = 0;

Mutex Texture::textureMutex;
List<Texture> Texture::textureList;
Map<Texture> Texture::textureHeaderMap;


namespace C4
{
	struct TextureStorageData
	{
		TextureFormat			engineFormat;

		unsigned_int32			renderFormatAlpha;
		unsigned_int32			renderFormat;
		unsigned_int32			pixelSize;

		Render::Decompressor	*decompressorAlpha;
		Render::Decompressor	*decompressor;
	};
}


namespace
{
	const TextureStorageData textureStorageDataTable[kTextureStorageCount] =
	{
		{kTextureRGBA8,		Render::kTextureRGBA8,			Render::kTextureRGBX8,		4,	&Image::DecompressImageRLE_RGBA32,	&Image::DecompressImageRLE_RGBA32},
		{kTextureBGRA8,		Render::kTextureBGRA8,			Render::kTextureBGRX8,		4,	&Image::DecompressImageRLE_RGBA32,	&Image::DecompressImageRLE_RGBA32},
		{kTextureRG8,		Render::kTextureRG8,			Render::kTextureRG8,		2,	&Image::DecompressImageRLE_XY16,	&Image::DecompressImageRLE_XY16},
		{kTextureR8,		Render::kTextureR8,				Render::kTextureR8,			1,	&Image::DecompressImageRLE_X8,		&Image::DecompressImageRLE_X8},
		{kTextureLA8,		Render::kTextureLA8,			Render::kTextureLA8,		2,	&Image::DecompressImageRLE_XY16,	&Image::DecompressImageRLE_XY16},
		{kTextureL8,		Render::kTextureL8,				Render::kTextureL8,			1,	&Image::DecompressImageRLE_X8,		&Image::DecompressImageRLE_X8},
		{kTextureI8,		Render::kTextureI8,				Render::kTextureI8,			1,	&Image::DecompressImageRLE_X8,		&Image::DecompressImageRLE_X8},
		{kTextureBC13,		Render::kTextureBC3,			Render::kTextureBC1,		0,	&Image::DecompressImageRLE_BC3,		&Image::DecompressImageRLE_BC1}
	};
}


TextureResource::TextureResource(const char *name, ResourceCatalog *catalog) : Resource<TextureResource>(name, catalog)
{
	for (machine a = 0; a < kMaxResourceTextureCount; a++)
	{
		owningTexture[a] = nullptr;
	}
}

TextureResource::~TextureResource()
{
}

ResourceResult TextureResource::LoadHeaderData(ResourceLoader *loader, TextureResourceHeader *resourceHeader, TextureHeader **textureHeader) const
{
	ResourceResult result = loader->Read(resourceHeader, 0, sizeof(TextureResourceHeader));
	if (result != kResourceOkay)
	{
		return (result);
	}

	unsigned_int32 size = resourceHeader->headerDataSize;
	char *storage = new char[size];

	result = loader->Read(storage, sizeof(TextureResourceHeader), size);
	if (result != kResourceOkay)
	{
		delete[] storage;
		return (result);
	}

	*textureHeader = reinterpret_cast<TextureHeader *>(storage);
	return (kResourceOkay);
}

ResourceResult TextureResource::LoadImageData(ResourceLoader *loader, const TextureResourceHeader *resourceHeader, const TextureHeader *textureHeader, int32 index, int32 level, void **imageData) const
{
	const TextureMipmapData *mipmapData = textureHeader->GetMipmapData() + level;
	unsigned_int32 size = mipmapData->chainSize;
	char *storage = new char[size];

	unsigned_int32 start = sizeof(TextureResourceHeader) + index * sizeof(TextureHeader) + textureHeader->mipmapDataOffset + level * sizeof(TextureMipmapData) + mipmapData->imageOffset;
	ResourceResult result = loader->Read(storage, start, size);
	if (result != kResourceOkay)
	{
		delete[] storage;
		return (result);
	}

	*imageData = storage;
	return (kResourceOkay);
} 

void TextureResource::ReleaseHeaderData(TextureHeader *textureHeader)
{ 
	delete[] reinterpret_cast<char *>(textureHeader);
} 

void TextureResource::ReleaseImageData(void *imageData)
{ 
	delete[] static_cast<char *>(imageData);
} 
 

Texture::Texture(TextureResource *resource, int32 index)
{
	textureResource = resource; 
	textureIndex = index;

	resource->SetOwningTexture(index, this);

	textureInfo.textureHeader = nullptr;
	textureInfo.imagePointer = nullptr;
	imagePointerOffset = 0;

	activeFlag = false;
	impostorClipFlag = false;
}

Texture::Texture(const TextureHeader *header, const void *image)
{
	textureResource = nullptr;
	textureIndex = 0;

	textureInfo.textureHeader = header;
	textureInfo.imagePointer = image;
	imagePointerOffset = 0;

	activeFlag = false;
	impostorClipFlag = false;
}

Texture::~Texture()
{
	textureMutex.Acquire();
	ListElement<Texture>::Detach();
	MapElement<Texture>::Detach();
	textureMutex.Release();

	Deactivate();

	TextureResource *resource = textureResource;
	if (resource)
	{
		resource->SetOwningTexture(textureIndex, nullptr);
		resource->Release();
	}
}

unsigned_int32 Texture::GetTextureTarget(void)
{
	const GraphicsCapabilities *capabilities = TheGraphicsMgr->GetCapabilities();

	unsigned_int32 target = Render::kTextureTarget2D;
	int32 maxTextureSize = capabilities->maxTextureSize;
	int32 biggestDimension = Max(textureWidth, textureHeight);

	switch (textureType)
	{
		case kTexture3D:

			target = Render::kTextureTarget3D;
			maxTextureSize = capabilities->max3DTextureSize;
			biggestDimension = Max(biggestDimension, textureDepth);
			break;

		case kTextureCube:

			target = Render::kTextureTargetCube;
			maxTextureSize = capabilities->maxCubeTextureSize;
			break;

		case kTextureRectangle:

			target = Render::kTextureTargetRectangle;
			break;

		case kTextureArray2D:

			target = Render::kTextureTargetArray2D;
			break;
	}

	int32 skipCount = 0;
	unsigned_int32 flags = textureFlags;
	if (!(flags & kTextureForceHighQuality))
	{
		skipCount = TheGraphicsMgr->GetTextureDetailLevel();
		if (flags & kTextureImagePalette)
		{
			skipCount = Max(skipCount, TheGraphicsMgr->GetPaletteDetailLevel());
		}

		biggestDimension >>= skipCount;
	}

	while (biggestDimension > maxTextureSize)
	{
		biggestDimension >>= 1;
		skipCount++;
	}

	baseMipmapLevel = (unsigned_int16) Min(skipCount, mipmapLevelCount - 1);
	return (target);
}

const TextureStorageData *Texture::GetTextureStorageData(TextureFormat imageFormat)
{
	const TextureStorageData *storageData = textureStorageDataTable;
	for (machine a = 0; a < kTextureStorageCount; a++)
	{
		if (storageData->engineFormat == imageFormat)
		{
			break;
		}

		storageData++;
	}

	return (storageData);
}

Render::Decompressor *Texture::GetDecompressor(const TextureHeader *textureHeader, const TextureMipmapData *mipmapData)
{
	CompressionType type = mipmapData->compressionType;
	if (type == kTextureCompressionGeneral)
	{
		return (&Comp::DecompressData);
	}
	else if (type == kTextureCompressionRunLength)
	{
		const TextureStorageData *storageData = GetTextureStorageData(textureHeader->imageFormat);
		return ((textureHeader->alphaSemantic != kTextureSemanticNone) ? storageData->decompressorAlpha : storageData->decompressor);
	}

	return (nullptr);
}

unsigned_int32 Texture::GetTextureWrapMode(TextureWrap mode)
{
	switch (mode)
	{
		case kTextureClamp:

			return (Render::kWrapClampToEdge);

		case kTextureClampBorder:

			return (Render::kWrapClampToBorder);

		case kTextureMirrorRepeat:

			return (Render::kWrapMirrorRepeat);

		case kTextureMirrorClamp:

			return (Render::kWrapMirrorClampToEdge);

		case kTextureMirrorClampBorder:

			return (Render::kWrapMirrorClampToBorder);
	}

	return (Render::kWrapRepeat);
}

void Texture::InitializeTextureObject(const TextureHeader *header)
{
	Construct(GetTextureTarget());

	SetSWrapMode(GetTextureWrapMode(header->wrapMode[0]));
	SetTWrapMode(GetTextureWrapMode(header->wrapMode[1]));
	if (GetTextureTargetIndex() == Render::kTextureTarget3D)
	{
		SetRWrapMode(GetTextureWrapMode(header->wrapMode[2]));
	}

	unsigned_int32 flags = textureFlags;
	if (flags & kTextureFilterInhibit)
	{
		SetFilterModes((mipmapLevelCount > 1) ? Render::kFilterNearestMipmapNearest : Render::kFilterNearest, Render::kFilterNearest);
	}
	else
	{
		SetFilterModes((mipmapLevelCount > 1) ? Render::kFilterLinearMipmapLinear : Render::kFilterLinear, Render::kFilterLinear);

		if ((!(flags & kTextureAnisotropicFilterInhibit)) && (GetTextureTargetIndex() != Render::kTextureTarget3D))
		{
			const GraphicsCapabilities *capabilities = TheGraphicsMgr->GetCapabilities();
			SetMaxAnisotropy(Fmin((float) TheGraphicsMgr->GetTextureFilterAnisotropy(), capabilities->maxTextureAnisotropy));
		}
	}
}

void Texture::TerminateTextureObject(const void *)
{
	Destruct();
}

const void *Texture::ProcessAuxiliaryData(const TextureHeader *header)
{
	const void *pointerTextureData = nullptr;

	if (header->auxiliaryDataOffset != 0)
	{
		const ChunkHeader *chunk = header->GetAuxiliaryData();
		unsigned_int32 size = header->auxiliaryDataSize;

		while (size > 0)
		{
			switch (chunk->chunkType)
			{
				case kTextureChunkParallaxScale:
				{
					float scale = *reinterpret_cast<const float *>(chunk + 1);
					floatTextureData[0] = scale / (float) textureWidth;
					floatTextureData[1] = scale / (float) textureHeight;
					break;
				}

				case kTextureChunkImageCenter:
				{
					const float *center = reinterpret_cast<const float *>(chunk + 1);
					floatTextureData[0] = center[0];
					floatTextureData[1] = center[1];
					break;
				}

				case kTextureChunkPaletteSize:
				{
					const unsigned_int32 *paletteSize = reinterpret_cast<const unsigned_int32 *>(chunk + 1);
					integerTextureData[0] = paletteSize[0];
					integerTextureData[1] = paletteSize[1];
					break;
				}

				case kTextureChunkImpostorSize:
				{
					const float *impostorSize = reinterpret_cast<const float *>(chunk + 1);
					floatTextureData[0] = impostorSize[0];
					floatTextureData[1] = impostorSize[1];
					break;
				}

				case kTextureChunkImpostorClipData:
				{
					const float *clipData = reinterpret_cast<const float *>(chunk + 1);
					floatTextureData[2] = clipData[0];
					floatTextureData[3] = clipData[1];
					floatTextureData[4] = clipData[2];
					floatTextureData[5] = clipData[3];
					impostorClipFlag = true;
					break;
				}

				case kTextureChunkReferenceList:
				{
					const int32 *pointer = reinterpret_cast<const int32 *>(chunk + 1);
					pointerTextureData = pointer;
					int32 count = *pointer;

					textureType = kTextureArray2D;
					textureDepth = count;

					if (count <= 9)
					{
						integerTextureData[0] = 3;
						integerTextureData[1] = 3;
					}
					else if (count <= 18)
					{
						integerTextureData[0] = 6;
						integerTextureData[1] = 3;
					}
					else
					{
						integerTextureData[0] = 6;
						integerTextureData[1] = 6;
					}

					textureFlags |= kTextureImagePalette;
					break;
				}
			}

			size -= sizeof(ChunkHeader) + chunk->chunkSize;
			chunk = chunk->GetNextChunk();
		}
	}

	return (pointerTextureData);
}

void Texture::Activate(const TextureHeader *header, const void *image)
{
	Render::TextureUploadData	uploadData;

	textureType = header->textureType;
	textureFlags = header->textureFlags;
	textureWidth = header->imageWidth;
	textureHeight = header->imageHeight;
	textureDepth = header->imageDepth;

	baseMipmapLevel = 0;
	mipmapLevelCount = (unsigned_int16) header->mipmapCount;

	alphaSemantic = header->alphaSemantic;
	imageFormat = header->imageFormat;

	GraphicsMgr::SyncRenderTask(&Texture::InitializeTextureObject, this, header);

	bool alpha = (header->alphaSemantic != kTextureSemanticNone);
	const TextureStorageData *storageData = GetTextureStorageData(header->imageFormat);
	unsigned_int32 renderFormat = (alpha) ? storageData->renderFormatAlpha : storageData->renderFormat;
	unsigned_int32 encoding = Render::kTextureEncodingLinear;

	if ((textureFlags & kTextureSrgbColor) && ((storageData->pixelSize & 3) == 0))
	{
		encoding = Render::kTextureEncodingSrgb;
	}

	if (textureFlags & kTextureRenderTarget)
	{
		renderFormat = (alpha) ? Render::kTextureRenderBufferRGBA8 : Render::kTextureRenderBufferRGB8;
	}
	else if ((!image) && (!(textureFlags & kTextureExternalStorage)))
	{
		if (header->mipmapDataOffset == 0)
		{
			image = header + 1;
		}
		else
		{
			image = header->GetMipmapData()->GetMipmapImage();
		}
	}

	image = static_cast<const char *>(image) + imagePointerOffset;
	unsigned_int32 memorySize = 0;
	uploadData.memorySize = &memorySize;

	switch (textureType)
	{
		case kTexture2D:
		{
			unsigned_int32 width = textureWidth;
			unsigned_int32 height = textureHeight;
			int32 mipmapCount = mipmapLevelCount;

			uploadData.format = renderFormat;
			uploadData.encoding = encoding;
			uploadData.width = width;
			uploadData.height = height;
			uploadData.mipmapCount = mipmapCount;

			for (machine level = 0; level < mipmapCount; level++)
			{
				unsigned_int32 size = width * height * storageData->pixelSize;
				uploadData.imageData[level].image = image;
				uploadData.imageData[level].size = size;
				uploadData.imageData[level].decompressor = nullptr;
				image = static_cast<const char *>(image) + size;

				width = Max(width >> 1, 1);
				height = Max(height >> 1, 1);
			}

			GraphicsMgr::SyncRenderTask(&TextureObject::SetImage2D, static_cast<TextureObject *>(this), &uploadData);
			break;
		}

		case kTexture3D:
		{
			unsigned_int32 width = textureWidth;
			unsigned_int32 height = textureHeight;
			unsigned_int32 depth = textureDepth;
			int32 mipmapCount = mipmapLevelCount;

			uploadData.format = renderFormat;
			uploadData.encoding = encoding;
			uploadData.width = width;
			uploadData.height = height;
			uploadData.depth = depth;
			uploadData.mipmapCount = mipmapCount;

			for (machine level = 0; level < mipmapCount; level++)
			{
				unsigned_int32 size = width * height * depth * storageData->pixelSize;
				uploadData.imageData[level].image = image;
				uploadData.imageData[level].size = size;
				uploadData.imageData[level].decompressor = nullptr;
				image = static_cast<const char *>(image) + size;

				width = Max(width >> 1, 1);
				height = Max(height >> 1, 1);
				depth = Max(depth >> 1, 1);
			}

			GraphicsMgr::SyncRenderTask(&TextureObject::SetImage3D, static_cast<TextureObject *>(this), &uploadData);
			break;
		}

		case kTextureCube:
		{
			unsigned_int32 width = textureWidth;
			int32 mipmapCount = mipmapLevelCount;

			uploadData.format = renderFormat;
			uploadData.encoding = encoding;
			uploadData.width = width;
			uploadData.mipmapCount = mipmapCount;

			for (machine level = 0; level < mipmapCount; level++)
			{
				for (machine component = 0; component < 6; component++)
				{
					unsigned_int32 size = width * width * storageData->pixelSize;
					uploadData.imageData[level * 6 + component].image = image;
					uploadData.imageData[level * 6 + component].size = size;
					uploadData.imageData[level * 6 + component].decompressor = nullptr;
					image = static_cast<const char *>(image) + size;
				}

				width = Max(width >> 1, 1);
			}

			GraphicsMgr::SyncRenderTask(&TextureObject::SetImageCube, static_cast<TextureObject *>(this), &uploadData);
			break;
		}

		case kTextureRectangle:

			if (image)
			{
				unsigned_int32 width = textureWidth;
				unsigned_int32 height = textureHeight;

				uploadData.format = renderFormat;
				uploadData.encoding = encoding;
				uploadData.width = width;
				uploadData.height = height;
				uploadData.rowLength = width;

				uploadData.imageData[0].image = image;
				uploadData.imageData[0].size = width * height * storageData->pixelSize;
				uploadData.imageData[0].decompressor = nullptr;

				GraphicsMgr::SyncRenderTask(&TextureObject::SetImageRect, static_cast<TextureObject *>(this), &uploadData);
			}
			else
			{
				Render::TextureAllocationData	allocationData;

				allocationData.memorySize = &memorySize;
				allocationData.format = renderFormat;
				allocationData.encoding = encoding;
				allocationData.width = textureWidth;
				allocationData.height = textureHeight;
				allocationData.renderBuffer = false;

				GraphicsMgr::SyncRenderTask(&TextureObject::AllocateStorageRect, static_cast<TextureObject *>(this), &allocationData);
			}

			break;
	}

	activeFlag = true;
	textureMemorySize = memorySize;

	textureMutex.Acquire();

	totalTextureCount++;
	totalTextureMemory += memorySize;

	if (!GetOwningList())
	{
		textureList.Append(this);
	}

	textureMutex.Release();
}

ResourceResult Texture::Activate(void)
{
	if (activeFlag)
	{
		return (kResourceOkay);
	}

	if (textureInfo.textureHeader)
	{
		Activate(textureInfo.textureHeader, textureInfo.imagePointer);
		return (kResourceOkay);
	}

	ResourceLoader				loader;
	void						*imageData;
	TextureHeader				*textureHeader;
	TextureResourceHeader		resourceHeader;
	Render::TextureUploadData	uploadData;

	ResourceResult result = textureResource->OpenLoader(&loader);
	if (result != kResourceOkay)
	{
		return (result);
	}

	result = textureResource->LoadHeaderData(&loader, &resourceHeader, &textureHeader);
	if (result != kResourceOkay)
	{
		return (result);
	}

	int32 textureLoadIndex = Min(textureIndex, resourceHeader.textureCount - 1);
	textureHeader += textureLoadIndex;

	textureType = textureHeader->textureType;
	textureFlags = textureHeader->textureFlags;
	textureWidth = textureHeader->imageWidth;
	textureHeight = textureHeader->imageHeight;
	textureDepth = textureHeader->imageDepth;

	baseMipmapLevel = 0;
	mipmapLevelCount = (unsigned_int16) textureHeader->mipmapCount;

	alphaSemantic = textureHeader->alphaSemantic;
	imageFormat = textureHeader->imageFormat;

	floatTextureData[0] = 0.0F;
	floatTextureData[1] = 0.0F;
	integerTextureData[0] = 0;
	integerTextureData[1] = 0;

	const void *pointerTextureData = ProcessAuxiliaryData(textureHeader);
	GraphicsMgr::SyncRenderTask(&Texture::InitializeTextureObject, this, textureHeader);

	const TextureStorageData *storageData = GetTextureStorageData(textureHeader->imageFormat);
	uploadData.format = (textureHeader->alphaSemantic != kTextureSemanticNone) ? storageData->renderFormatAlpha : storageData->renderFormat;
	uploadData.encoding = Render::kTextureEncodingLinear;

	if ((textureFlags & kTextureSrgbColor) && ((storageData->pixelSize & 3) == 0))
	{
		uploadData.encoding = Render::kTextureEncodingSrgb;
	}

	unsigned_int32 memorySize = 0;
	uploadData.memorySize = &memorySize;

	if (!(textureFlags & kTextureReferenceList))
	{
		int32 componentCount = (textureType != kTextureCube) ? 1 : 6;
		int32 mipmapDataSkipCount = baseMipmapLevel * componentCount;

		result = textureResource->LoadImageData(&loader, &resourceHeader, textureHeader, textureLoadIndex, mipmapDataSkipCount, &imageData);
		if (result != kResourceOkay)
		{
			TextureResource::ReleaseHeaderData(textureHeader);
			return (result);
		}

		const char *image = static_cast<char *>(imageData);
		int32 mipmapCount = mipmapLevelCount - baseMipmapLevel;
		const TextureMipmapData *mipmapData = textureHeader->GetMipmapData() + mipmapDataSkipCount;

		switch (textureType)
		{
			case kTexture2D:
			{
				for (machine level = 0; level < mipmapCount; level++)
				{
					unsigned_int32 size = mipmapData->imageSize;
					uploadData.imageData[level].image = image;
					uploadData.imageData[level].size = size;
					uploadData.imageData[level].decompressor = GetDecompressor(textureHeader, mipmapData);
					image += size;
					mipmapData++;
				}

				uploadData.width = textureWidth >> baseMipmapLevel;
				uploadData.height = textureHeight >> baseMipmapLevel;
				uploadData.mipmapCount = mipmapCount;

				GraphicsMgr::SyncRenderTask((storageData->engineFormat == kTextureBC13) ? &TextureObject::SetCompressedImage2D : &TextureObject::SetImage2D, static_cast<TextureObject *>(this), &uploadData);
				break;
			}

			case kTexture3D:
			{
				for (machine level = 0; level < mipmapCount; level++)
				{
					unsigned_int32 size = mipmapData->imageSize;
					uploadData.imageData[level].image = image;
					uploadData.imageData[level].size = size;
					uploadData.imageData[level].decompressor = GetDecompressor(textureHeader, mipmapData);
					image += size;
					mipmapData++;
				}

				uploadData.width = textureWidth >> baseMipmapLevel;
				uploadData.height = textureHeight >> baseMipmapLevel;
				uploadData.depth = textureDepth >> baseMipmapLevel;
				uploadData.mipmapCount = mipmapCount;

				GraphicsMgr::SyncRenderTask(&TextureObject::SetImage3D, static_cast<TextureObject *>(this), &uploadData);
				break;
			}

			case kTextureCube:
			{
				int32 i = 0;
				for (machine level = 0; level < mipmapCount; level++)
				{
					for (machine component = 0; component < 6; component++)
					{
						unsigned_int32 size = mipmapData->imageSize;
						uploadData.imageData[i].image = image;
						uploadData.imageData[i].size = size;
						uploadData.imageData[i].decompressor = GetDecompressor(textureHeader, mipmapData);
						image += size;
						mipmapData++;
						i++;
					}
				}

				uploadData.width = textureWidth >> baseMipmapLevel;
				uploadData.mipmapCount = mipmapCount;

				GraphicsMgr::SyncRenderTask((storageData->engineFormat == kTextureBC13) ? &TextureObject::SetCompressedImageCube : &TextureObject::SetImageCube, static_cast<TextureObject *>(this), &uploadData);
				break;
			}

			case kTextureRectangle:
			{
				uploadData.width = textureWidth;
				uploadData.height = textureHeight;
				uploadData.rowLength = textureWidth;

				uploadData.imageData[0].image = image;
				uploadData.imageData[0].size = mipmapData->imageSize;
				uploadData.imageData[0].decompressor = GetDecompressor(textureHeader, mipmapData);

				GraphicsMgr::SyncRenderTask(&TextureObject::SetImageRect, static_cast<TextureObject *>(this), &uploadData);
				break;
			}

			case kTextureArray2D:
			{
				for (machine level = 0; level < mipmapCount; level++)
				{
					unsigned_int32 size = mipmapData->imageSize;
					uploadData.imageData[level].image = image;
					uploadData.imageData[level].size = size;
					uploadData.imageData[level].decompressor = GetDecompressor(textureHeader, mipmapData);
					image += size;
					mipmapData++;
				}

				uploadData.width = textureWidth >> baseMipmapLevel;
				uploadData.height = textureHeight >> baseMipmapLevel;
				uploadData.depth = textureDepth;
				uploadData.mipmapCount = mipmapCount;

				GraphicsMgr::SyncRenderTask((storageData->engineFormat == kTextureBC13) ? &TextureObject::SetCompressedImageArray2D : &TextureObject::SetImageArray2D, static_cast<TextureObject *>(this), &uploadData);
				break;
			}
		}

		TextureResource::ReleaseImageData(imageData);
	}
	else
	{
		const int32 *pointer = static_cast<const int32 *>(pointerTextureData);
		const ResourceName *textureName = reinterpret_cast<const ResourceName *>(pointer + 1);

		unsigned_int32 width = textureHeader->imageWidth >> baseMipmapLevel;
		unsigned_int32 height = textureHeader->imageHeight >> baseMipmapLevel;

		int32 storageSize = Image::CalculateBlockMipmapChainSize2D(width, height, mipmapLevelCount - baseMipmapLevel);
		if (alphaSemantic == kTextureSemanticNone)
		{
			storageSize >>= 1;
		}

		int32 entryCount = textureDepth;
		storageSize *= entryCount;
		unsigned_int8 *storage = new unsigned_int8[storageSize];

		for (machine a = 0; a < entryCount; a++)
		{
			result = LoadReferencedArrayImage(textureName[a], storage, a, entryCount);
			if (result != kResourceOkay)
			{
				break;
			}
		}

		unsigned_int32 blockSize = (alphaSemantic == kTextureSemanticNone) ? 8 : 16;
		int32 mipmapCount = mipmapLevelCount - baseMipmapLevel;
		unsigned_int8 *image = storage;

		for (machine level = 0; level < mipmapCount; level++)
		{
			int32 blockCount = ((width + 3) >> 2) * ((height + 3) >> 2);
			unsigned_int32 size = blockCount * blockSize * entryCount;

			uploadData.imageData[level].image = image;
			uploadData.imageData[level].size = size;
			uploadData.imageData[level].decompressor = nullptr;

			width = Max(width >> 1, 1);
			height = Max(height >> 1, 1);
			image += size;
		}

		uploadData.width = textureWidth >> baseMipmapLevel;
		uploadData.height = textureHeight >> baseMipmapLevel;
		uploadData.depth = entryCount;
		uploadData.mipmapCount = mipmapCount;

		GraphicsMgr::SyncRenderTask(&TextureObject::SetCompressedImageArray2D, static_cast<TextureObject *>(this), &uploadData);
		delete[] storage;
	}

	TextureResource::ReleaseHeaderData(textureHeader - textureLoadIndex);

	activeFlag = true;
	textureMemorySize = memorySize;

	textureMutex.Acquire();

	totalTextureCount++;
	totalTextureMemory += memorySize;

	if (!GetOwningList())
	{
		textureList.Append(this);
	}

	textureMutex.Release();
	return (kResourceOkay);
}

void Texture::Deactivate(void)
{
	if (activeFlag)
	{
		activeFlag = false;

		textureMutex.Acquire();
		totalTextureCount--;
		totalTextureMemory -= textureMemorySize;
		textureMutex.Release();

		GraphicsMgr::SyncRenderTask(&Texture::TerminateTextureObject, this);
	}
}

ResourceResult Texture::LoadReferencedArrayImage(const char *name, unsigned_int8 *finalImage, int32 entryIndex, int32 entryCount) const
{
	ResourceLoader				loader;
	void						*imageData;
	TextureHeader				*textureHeader;
	TextureResourceHeader		resourceHeader;

	TextureResource *resource = TextureResource::Get(name, kResourceDeferLoad);
	ResourceResult result = resource->OpenLoader(&loader);
	if (result == kResourceOkay)
	{
		result = textureResource->LoadHeaderData(&loader, &resourceHeader, &textureHeader);
		if (result == kResourceOkay)
		{
			result = textureResource->LoadImageData(&loader, &resourceHeader, textureHeader, 0, baseMipmapLevel, &imageData);
			if (result == kResourceOkay)
			{
				const unsigned_int8 *image = static_cast<unsigned_int8 *>(imageData);
				int32 mipmapCount = textureHeader->mipmapCount - baseMipmapLevel;
				const TextureMipmapData *mipmapData = textureHeader->GetMipmapData() + baseMipmapLevel;

				int32 width = textureHeader->imageWidth >> baseMipmapLevel;
				int32 height = textureHeader->imageHeight >> baseMipmapLevel;
				unsigned_int32 blockSize = (alphaSemantic == kTextureSemanticNone) ? 8 : 16;

				for (machine level = 0; level < mipmapCount; level++)
				{
					int32 blockCount = ((width + 3) >> 2) * ((height + 3) >> 2);
					finalImage += blockCount * blockSize * entryIndex;

					unsigned_int32 size = mipmapData->imageSize;
					Render::Decompressor *decompressor = GetDecompressor(textureHeader, mipmapData);

					if (decompressor)
					{
						(*decompressor)(image, size, finalImage);
					}
					else
					{
						MemoryMgr::CopyMemory(image, finalImage, size);
					}

					width = Max(width >> 1, 1);
					height = Max(height >> 1, 1);
					mipmapData++;

					image += size;
					finalImage += blockCount * blockSize * (entryCount - entryIndex);
				}

				TextureResource::ReleaseImageData(imageData);
			}

			TextureResource::ReleaseHeaderData(textureHeader);
		}
	}

	resource->Release();
	return (result);
}

Texture *Texture::Get(const char *name, int32 index)
{
	TextureResource *resource = TextureResource::Get(name, kResourceDeferLoad);

	Texture *texture = resource->GetOwningTexture(index);
	if (texture)
	{
		textureMutex.Acquire();
		texture->Retain();
		textureMutex.Release();

		resource->Release();
		return (texture);
	}

	texture = new Texture(resource, index);

	if (texture->Activate() == kResourceOkay)
	{
		return (texture);
	}

	delete texture;
	return (nullptr);
}

Texture *Texture::Get(const TextureHeader *header, const void *image)
{
	TextureInfo		info;

	info.textureHeader = header;
	info.imagePointer = image;

	textureMutex.Acquire();

	Texture *texture = textureHeaderMap.Find(info);
	if (texture)
	{
		texture->Retain();
		textureMutex.Release();
		return (texture);
	}

	if (header->textureFlags & kTextureDynamic)
	{
		texture = new Texture(header, image);
		textureHeaderMap.Insert(texture);
		textureMutex.Release();
		return (texture);
	}

	textureMutex.Release();

	texture = new Texture(header, image);

	if (texture->Activate() == kResourceOkay)
	{
		textureMutex.Acquire();
		textureHeaderMap.Insert(texture);
		textureMutex.Release();
		return (texture);
	}

	delete texture;
	return (nullptr);
}

void Texture::UpdateImage(void)
{
	if (GetOwningList())
	{
		const char *image = static_cast<const char *>(textureInfo.imagePointer) + imagePointerOffset;

		if (textureType == kTexture2D)
		{
			UpdateImage2D(0, 0, textureWidth, textureHeight, textureWidth, image);
		}
		else if (textureType == kTexture3D)
		{
			UpdateImage3D(0, 0, 0, textureWidth, textureHeight, textureDepth, image);
		}
		else if (textureType == kTextureRectangle)
		{
			UpdateImageRect(0, 0, textureWidth, textureHeight, textureWidth, image);
		}
	}
	else
	{
		Activate();
	}
}

void Texture::UpdateRect(const Rect& rect)
{
	if (GetOwningList())
	{
		const char *image = static_cast<const char *>(textureInfo.imagePointer);
		if (!image)
		{
			image = static_cast<const char *>(textureInfo.textureHeader->GetMipmapData()->GetMipmapImage());
		}

		image += imagePointerOffset;

		if (textureType == kTextureRectangle)
		{
			UpdateImageRect(rect.left, rect.top, rect.Width(), rect.Height(), textureWidth, image);
		}
		else if (mipmapLevelCount == 1)
		{
			UpdateImage2D(rect.left, rect.top, rect.Width(), rect.Height(), textureWidth, image);
		}
		else
		{
			UpdateImage2D(textureWidth, textureHeight, mipmapLevelCount, image);
		}
	}
	else
	{
		Activate();
	}
}

void Texture::UpdateRect(const Rect& rect, int32 pitch, const void *image)
{
	if (GetOwningList())
	{
		if (textureType == kTextureRectangle)
		{
			UpdateImageRect(rect.left, rect.top, rect.Width(), rect.Height(), pitch, image);
		}
		else
		{
			UpdateImage2D(rect.left, rect.top, rect.Width(), rect.Height(), pitch, image);
		}
	}
	else
	{
		Activate();
	}
}

void Texture::DeactivateAll(void)
{
	Texture *texture = textureList.First();
	while (texture)
	{
		texture->Deactivate();
		texture = texture->ListElement<Texture>::Next();
	}
}

void Texture::ReactivateAll(void)
{
	Texture *texture = textureList.First();
	while (texture)
	{
		texture->Activate();
		texture = texture->ListElement<Texture>::Next();
	}
}

void Texture::Reload(const char *name)
{
	Texture *texture = textureList.Last();
	while (texture)
	{
		TextureResource *resource = texture->textureResource;
		if ((resource) && (resource->GetName() == name))
		{
			texture->Deactivate();
			texture->Activate();

			GraphicsMgr::ResetShaders();
			break;
		}

		texture = texture->ListElement<Texture>::Previous();
	}
}

// ZYUQURM
