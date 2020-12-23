 

#include "C4Sequence.h"


using namespace C4;


ResourceDescriptor SequenceResource::descriptor("seq");


SequenceResource::SequenceResource(const char *name, ResourceCatalog *catalog) : Resource<SequenceResource>(name, catalog)
{
}

SequenceResource::~SequenceResource()
{
}


SequenceImageImportPlugin::SequenceImageImportPlugin()
{
}

SequenceImageImportPlugin::~SequenceImageImportPlugin()
{
}

const char *SequenceImageImportPlugin::GetImageTypeName(void) const
{
	return ("Sequence");
}

const ResourceDescriptor *SequenceImageImportPlugin::GetImageResourceDescriptor(void) const
{
	return (SequenceResource::GetDescriptor());
}

ImageFormat SequenceImageImportPlugin::GetImageFormat(void) const
{
	return (kImageFormatYCbCr);
}

EngineResult SequenceImageImportPlugin::GetImageFileInfo(const char *name, Integer2D *size, unsigned_int32 *flags, int32 *count)
{
	File	file;

	EngineResult result = file.Open(name);
	if (result == kFileOkay)
	{
		SequenceHeader		header;

		result = file.Read(&header, sizeof(SequenceHeader));
		if (result == kFileOkay)
		{
			int32 width = header.imageWidth;
			int32 height = header.imageHeight;
			size->Set(width, height);

			if (flags)
			{
				*flags = 0;
			}

			if (count)
			{
				int32 pixelCount = width * height;
				unsigned_int32 imageSize = pixelCount + (pixelCount >> 1);
				*count = (int32) ((file.GetSize() - header.headerSize) / imageSize);
			}
		}
	}

	return (result);
}

EngineResult SequenceImageImportPlugin::ImportImageFile(const char *name, void **image, Integer2D *size, unsigned_int32 *flags, int32 index)
{
	File	file;

	EngineResult result = file.Open(name);
	if (result == kFileOkay)
	{
		SequenceHeader		header;

		result = file.Read(&header, sizeof(SequenceHeader));
		if (result == kFileOkay)
		{
			int32 width = header.imageWidth;
			int32 height = header.imageHeight;
			size->Set(width, height);

			if (flags)
			{
				*flags = 0;
			}

			int32 pixelCount = width * height;
			unsigned_int32 imageSize = pixelCount + (pixelCount >> 1);

			int32 imageCount = (int32) ((file.GetSize() - header.headerSize) / imageSize);
			if ((index >= 0) && (index < imageCount))
			{ 
				int8 *pixel = new int8[imageSize];
				*image = pixel;
 
				file.SetPosition(int64(header.headerSize) + int64(imageSize) * index);
				result = file.Read(pixel, imageSize); 
			}
			else
			{ 
				result = kPluginBadImageIndex;
			} 
		} 
	}

	return (result);
} 

void SequenceImageImportPlugin::ReleaseImageData(void *image)
{
	delete[] static_cast<int8 *>(image);
}

// ZYUQURM
