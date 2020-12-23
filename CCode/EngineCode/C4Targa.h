 

#ifndef C4Targa_h
#define C4Targa_h


#include "C4Plugins.h"


namespace C4
{
	struct TargaHeader
	{
		unsigned_int8		idLength;
		unsigned_int8		colorMapType;
		unsigned_int8		imageType;
		unsigned_int8		colorMapStartL;
		unsigned_int8		colorMapStartH;
		unsigned_int8		colorMapLengthL;
		unsigned_int8		colorMapLengthH;
		unsigned_int8		colorMapDepth;
		int16				xOffset;
		int16				yOffset;
		int16				width;
		int16				height;
		unsigned_int8		pixelDepth;
		unsigned_int8		imageDescriptor;

		const unsigned_int8 *GetPixelData(void) const
		{
			return (reinterpret_cast<const unsigned_int8 *>(this + 1) + idLength);
		};
	};


	class TargaResource : public Resource<TargaResource>
	{
		friend class Resource<TargaResource>;

		private:

			static C4API ResourceDescriptor		descriptor;

			~TargaResource();

		public:

			C4API TargaResource(const char *name, ResourceCatalog *catalog);

			const TargaHeader *GetTargaHeader(void) const
			{
				return (static_cast<const TargaHeader *>(GetData()));
			}
	};


	class TargaImageImportPlugin : public ImageImportPlugin
	{
		private:

			static void CopyTarga_L8(const unsigned_int8 *data, Color4C *restrict image, int32 pixelCount);
			static void CopyTarga_RGB16(const unsigned_int8 *data, Color4C *restrict image, int32 pixelCount);
			static void CopyTarga_RGB24(const unsigned_int8 *data, Color4C *restrict image, int32 pixelCount);
			static void CopyTarga_RGBA32(const unsigned_int8 *data, Color4C *restrict image, int32 pixelCount);

			static void DecompressTarga_L8(const unsigned_int8 *data, Color4C *restrict image, int32 pixelCount);
			static void DecompressTarga_RGB16(const unsigned_int8 *data, Color4C *restrict image, int32 pixelCount);
			static void DecompressTarga_RGB24(const unsigned_int8 *data, Color4C *restrict image, int32 pixelCount);
			static void DecompressTarga_RGBA32(const unsigned_int8 *data, Color4C *restrict image, int32 pixelCount);

		public:

			TargaImageImportPlugin();
			~TargaImageImportPlugin();

			const char *GetImageTypeName(void) const override;
			const ResourceDescriptor *GetImageResourceDescriptor(void) const override;

			ImageFormat GetImageFormat(void) const override;
			EngineResult GetImageFileInfo(const char *name, Integer2D *size, unsigned_int32 *flags = nullptr, int32 *count = nullptr) override;
			EngineResult ImportImageFile(const char *name, void **image, Integer2D *size, unsigned_int32 *flags = nullptr, int32 index = 0) override;
			void ReleaseImageData(void *image) override;
	};


	class TargaImageExportPlugin : public ImageExportPlugin
	{
		private:

			static unsigned_int32 CompressTarga_BGRA(const Color4C *image, unsigned_int8 *restrict data, int32 width, int32 height);
			static unsigned_int32 CompressTarga_BGR(const Color4C *image, unsigned_int8 *restrict data, int32 width, int32 height);
			static unsigned_int32 CompressTarga_RGBA(const Color4C *image, unsigned_int8 *restrict data, int32 width, int32 height);
			static unsigned_int32 CompressTarga_RGB(const Color4C *image, unsigned_int8 *restrict data, int32 width, int32 height);

		public:

			TargaImageExportPlugin();
			~TargaImageExportPlugin();

			const char *GetImageTypeName(void) const override;
			const ResourceDescriptor *GetImageResourceDescriptor(void) const override;

			EngineResult ExportImageFile(const char *name, const Color4C *image, const Integer2D& size, unsigned_int32 flags = 0); 
	};
}
 

#endif 

// ZYUQURM
