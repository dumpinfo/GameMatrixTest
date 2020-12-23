 

#ifndef C4Sequence_h
#define C4Sequence_h


#include "C4Plugins.h"


namespace C4
{
	struct SequenceHeader
	{
		unsigned_int32		endian;
		unsigned_int32		headerSize;
		int32				imageWidth;
		int32				imageHeight;
	};


	class SequenceResource : public Resource<SequenceResource>
	{
		friend class Resource<SequenceResource>;

		private:

			static C4API ResourceDescriptor		descriptor;

			~SequenceResource();

		public:

			C4API SequenceResource(const char *name, ResourceCatalog *catalog);

			const SequenceHeader *GetSequenceHeader(void) const
			{
				return (static_cast<const SequenceHeader *>(GetData()));
			}
	};


	class SequenceImageImportPlugin : public ImageImportPlugin
	{
		public:

			SequenceImageImportPlugin();
			~SequenceImageImportPlugin();

			const char *GetImageTypeName(void) const override;
			const ResourceDescriptor *GetImageResourceDescriptor(void) const override;

			ImageFormat GetImageFormat(void) const override;
			EngineResult GetImageFileInfo(const char *name, Integer2D *size, unsigned_int32 *flags = nullptr, int32 *count = nullptr) override;
			EngineResult ImportImageFile(const char *name, void **image, Integer2D *size, unsigned_int32 *flags = nullptr, int32 index = 0) override;
			void ReleaseImageData(void *image) override;
	};
}


#endif

// ZYUQURM
