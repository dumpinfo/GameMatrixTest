//=============================================================
//
// C4 Engine version 4.5
// Copyright 1999-2015, by Terathon Software LLC
//
// This file is part of the C4 Engine and is provided under the
// terms of the license agreement entered by the registed user.
//
// Unauthorized redistribution of source code is strictly
// prohibited. Violators will be prosecuted.
//
//=============================================================


#ifndef C4MaterialContainer_h
#define C4MaterialContainer_h


#include "C4Primitives.h"
#include "C4EditorBase.h"


namespace C4
{
	class MaterialContainer : public ListElement<MaterialContainer>, public Packable, public Configurable
	{
		friend class EditorObject;
		friend class MaterialWindow;

		private:

			MaterialObject		*materialObject;
			int32				usageCount;

			PrimitiveType		previewType;
			ResourceName		materialName;

			bool				primaryDiffuseTexture;
			bool				primaryNormalTexture;

			MaterialContainer();
			MaterialContainer(const MaterialContainer& materialContainer);

			static void MaterialObjectLinkProc(Object *object, void *cookie);

			Setting *GetTextureMapSetting(Type category, int32 index) const;
			Setting *GetTexcoordInputSetting(Type category, int32 index) const;
			bool SetTextureMapSetting(const Setting *setting, AttributeType type);
			void SetTexcoordInputSetting(const Setting *setting, AttributeType type, int32 index);

		public:

			C4EDITORAPI MaterialContainer(MaterialObject *object, const char *name = nullptr);
			C4EDITORAPI ~MaterialContainer();

			MaterialObject *GetMaterialObject(void) const
			{
				return (materialObject);
			}

			int32 GetUsageCount(void) const
			{
				return (usageCount);
			}

			void SetUsageCount(int32 count)
			{
				usageCount = count;
			}

			PrimitiveType GetPreviewType(void) const
			{
				return (previewType);
			}

			void SetPreviewType(PrimitiveType type)
			{
				previewType = type;
			}

			const ResourceName& GetMaterialName(void) const
			{
				return (materialName);
			}

			void SetMaterialName(const char *name)
			{
				materialName = name;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetCategoryCount(void) const override;
			Type GetCategoryType(int32 index, const char **title) const override;
			int32 GetCategorySettingCount(Type category) const override;
			Setting *GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const override;
			void SetCategorySetting(Type category, const Setting *setting) override;

			C4EDITORAPI void SetMaterialObject(MaterialObject *object);
	};
}


#endif

// ZYUQURM
