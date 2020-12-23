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


#include "C4TerrainBuilders.h"
#include "C4WorldEditor.h"
#include "C4Targa.h"


using namespace C4;


ResourceDescriptor RawResource::descriptor("raw");


RawResource::RawResource(const char *name, ResourceCatalog *catalog) : Resource<RawResource>(name, catalog)
{
}

RawResource::~RawResource()
{
}


TerrainBuilder::TerrainBuilder(TerrainBuilderType type)
{
	terrainBuilderType = type;
}

TerrainBuilder::~TerrainBuilder()
{
}


FlatPlaneTerrainBuilder::FlatPlaneTerrainBuilder() : TerrainBuilder(kTerrainBuilderFlatPlane)
{
	planeSize.Set(128, 128);
	emptyHeight = kTerrainDimension * 2;
	solidDepth = kTerrainDimension * 2;
}

FlatPlaneTerrainBuilder::~FlatPlaneTerrainBuilder()
{
}

int32 FlatPlaneTerrainBuilder::GetSettingCount(void) const
{
	return (TerrainBuilder::GetSettingCount() + 5);
}

Setting *FlatPlaneTerrainBuilder::GetSetting(int32 index) const
{
	int32 count = TerrainBuilder::GetSettingCount();
	if (index < count)
	{
		return (TerrainBuilder::GetSetting(index));
	}

	const StringTable *table = TheWorldEditor->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('TBLD', kTerrainBuilderFlatPlane, 'PARM'));
		return (new HeadingSetting('PARM', title));
	}

	if (index == count + 1)
	{
		const char *title = table->GetString(StringID('TBLD', kTerrainBuilderFlatPlane, 'XSIZ'));
		return (new TextSetting('XSIZ', Text::IntegerToString(planeSize.x), title, 4, &EditTextWidget::NumberFilter));
	}

	if (index == count + 2)
	{
		const char *title = table->GetString(StringID('TBLD', kTerrainBuilderFlatPlane, 'YSIZ'));
		return (new TextSetting('YSIZ', Text::IntegerToString(planeSize.y), title, 4, &EditTextWidget::NumberFilter));
	}

	if (index == count + 3)
	{
		const char *title = table->GetString(StringID('TBLD', kTerrainBuilderFlatPlane, 'POSI'));
		return (new TextSetting('POSI', Text::IntegerToString(emptyHeight), title, 4, &EditTextWidget::NumberFilter));
	}

	if (index == count + 4)
	{
		const char *title = table->GetString(StringID('TBLD', kTerrainBuilderFlatPlane, 'NEGA'));
		return (new TextSetting('NEGA', Text::IntegerToString(solidDepth), title, 4, &EditTextWidget::NumberFilter));
	}

	return (nullptr);
}

void FlatPlaneTerrainBuilder::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'XSIZ')
	{
		planeSize.x = Max(Text::StringToInteger(static_cast<const TextSetting *>(setting)->GetText()), kTerrainDimension);
	}
	else if (identifier == 'YSIZ')
	{
		planeSize.y = Max(Text::StringToInteger(static_cast<const TextSetting *>(setting)->GetText()), kTerrainDimension);
	} 
	else if (identifier == 'POSI')
	{
		emptyHeight = Max(Text::StringToInteger(static_cast<const TextSetting *>(setting)->GetText()), 1); 
	}
	else if (identifier == 'NEGA') 
	{
		solidDepth = Max(Text::StringToInteger(static_cast<const TextSetting *>(setting)->GetText()), 1);
	} 
	else
	{ 
		TerrainBuilder::SetSetting(setting); 
	}
}

bool FlatPlaneTerrainBuilder::BuildTerrain(TerrainBlock *block) 
{
	int32 blockSizeX = (planeSize.x + (kTerrainDimension - 1)) >> kTerrainLogDimension;
	int32 blockSizeY = (planeSize.y + (kTerrainDimension - 1)) >> kTerrainLogDimension;
	int32 blockSizeZ = (emptyHeight + solidDepth + (kTerrainDimension - 1)) >> kTerrainLogDimension;

	block->SetBlockSize(Integer3D(blockSizeX, blockSizeY, blockSizeZ));

	TerrainBlockObject *object = block->GetObject();
	object->SetVoxelScale(object->GetVoxelScale() / (float) Max(blockSizeX, blockSizeY));

	block->SetBlockToHorizontalPlane(solidDepth);
	return (true);
}


TargaHeightFieldTerrainBuilder::TargaHeightFieldTerrainBuilder() : TerrainBuilder(kTerrainBuilderTargaHeightField)
{
	imageName[0] = 0;
	heightScale = 100.0F;
	emptyHeight = kTerrainDimension * 2;
	solidDepth = kTerrainDimension * 2;
}

TargaHeightFieldTerrainBuilder::~TargaHeightFieldTerrainBuilder()
{
}

int32 TargaHeightFieldTerrainBuilder::GetSettingCount(void) const
{
	return (TerrainBuilder::GetSettingCount() + 6);
}

Setting *TargaHeightFieldTerrainBuilder::GetSetting(int32 index) const
{
	int32 count = TerrainBuilder::GetSettingCount();
	if (index < count)
	{
		return (TerrainBuilder::GetSetting(index));
	}

	const StringTable *table = TheWorldEditor->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('TBLD', kTerrainBuilderTargaHeightField, 'FILE'));
		return (new HeadingSetting('FILE', title));
	}

	if (index == count + 1)
	{
		const char *title = table->GetString(StringID('TBLD', kTerrainBuilderTargaHeightField, 'IMAG'));
		const char *picker = table->GetString(StringID('TBLD', kTerrainBuilderTargaHeightField, 'PICK'));
		return (new ResourceSetting('IMAG', imageName, title, picker, TargaResource::GetDescriptor(), nullptr, kResourceSettingGenericPath | kResourceSettingImportCatalog));
	}

	if (index == count + 2)
	{
		const char *title = table->GetString(StringID('TBLD', kTerrainBuilderTargaHeightField, 'PARM'));
		return (new HeadingSetting('PARM', title));
	}

	if (index == count + 3)
	{
		const char *title = table->GetString(StringID('TBLD', kTerrainBuilderTargaHeightField, 'SCAL'));
		return (new TextSetting('SCAL', heightScale, title));
	}

	if (index == count + 4)
	{
		const char *title = table->GetString(StringID('TBLD', kTerrainBuilderTargaHeightField, 'POSI'));
		return (new TextSetting('POSI', Text::IntegerToString(emptyHeight), title, 4, &EditTextWidget::NumberFilter));
	}

	if (index == count + 5)
	{
		const char *title = table->GetString(StringID('TBLD', kTerrainBuilderTargaHeightField, 'NEGA'));
		return (new TextSetting('NEGA', Text::IntegerToString(solidDepth), title, 4, &EditTextWidget::NumberFilter));
	}

	return (nullptr);
}

void TargaHeightFieldTerrainBuilder::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'IMAG')
	{
		imageName = static_cast<const ResourceSetting *>(setting)->GetResourceName();
		imageName[Text::GetResourceNameLength(imageName)] = 0;
	}
	else if (setting->GetSettingIdentifier() == 'SCAL')
	{
		heightScale = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
	}
	else if (setting->GetSettingIdentifier() == 'POSI')
	{
		emptyHeight = Text::StringToInteger(static_cast<const TextSetting *>(setting)->GetText());
	}
	else if (setting->GetSettingIdentifier() == 'NEGA')
	{
		solidDepth = Text::StringToInteger(static_cast<const TextSetting *>(setting)->GetText());
	}
	else
	{
		TerrainBuilder::SetSetting(setting);
	}
}

unsigned_int8 *TargaHeightFieldTerrainBuilder::GenerateHeightField(const Color4C *image, const Integer2D& size, int32 fieldWidth, int32 fieldHeight)
{
	unsigned_int8 *heightField = new unsigned_int8[fieldWidth * fieldHeight];
	unsigned_int8 *field = heightField;

	for (machine y = 0; y < fieldHeight; y++)
	{
		for (machine x = 0; x < fieldWidth; x++)
		{
			field[x] = (unsigned_int8) Max(Max(image->GetRed(), image->GetGreen()), image->GetBlue());
			image++;
		}

		field += fieldWidth;
		image += size.x - fieldWidth;
	}

	return (heightField);
}

bool TargaHeightFieldTerrainBuilder::BuildTerrain(TerrainBlock *block)
{
	void				*image;
	Integer2D			size;
	unsigned_int32		flags;

	bool result = false;

	ImageImportPlugin *targaImporter = ThePluginMgr->GetTargaImageImportPlugin();
	if (targaImporter->ImportImageFile((String<>(ThePluginMgr->GetImportCatalog()->GetRootPath()) += imageName) += ".tga", &image, &size, &flags) == kEngineOkay)
	{
		int32 blockSizeX = size.x >> kTerrainLogDimension;
		int32 blockSizeY = size.y >> kTerrainLogDimension;
		if ((blockSizeX > 0) && (blockSizeY > 0))
		{
			TerrainBlockObject *object = block->GetObject();
			float scale = object->GetVoxelScale() / (float) Max(blockSizeX, blockSizeY);
			object->SetVoxelScale(scale);

			int32 height = Max((int32) (heightScale / scale), 1);
			int32 blockSizeZ = (emptyHeight + solidDepth + height + (kTerrainDimension - 1)) >> kTerrainLogDimension;
			block->SetBlockSize(Integer3D(blockSizeX, blockSizeY, blockSizeZ));

			unsigned_int8 *field = GenerateHeightField(static_cast<Color4C *>(image), size, blockSizeX << kTerrainLogDimension, blockSizeY << kTerrainLogDimension);
			if (field)
			{
				block->SetBlockToHeightField(field, height, solidDepth * 255);

				delete[] field;
				result = true;
			}
		}

		targaImporter->ReleaseImageData(image);
	}

	return (result);
}


RawHeightFieldTerrainBuilder::RawHeightFieldTerrainBuilder() : TerrainBuilder(kTerrainBuilderRawHeightField)
{
	imageName[0] = 0;
	imageWidth = kTerrainDimension;
	imageHeight = kTerrainDimension;
	channelCount = 1;
	channelDepth = 8;

	heightScale = 100.0F;
	emptyHeight = kTerrainDimension * 2;
	solidDepth = kTerrainDimension * 2;
}

RawHeightFieldTerrainBuilder::~RawHeightFieldTerrainBuilder()
{
}

int32 RawHeightFieldTerrainBuilder::GetSettingCount(void) const
{
	return (TerrainBuilder::GetSettingCount() + 10);
}

Setting *RawHeightFieldTerrainBuilder::GetSetting(int32 index) const
{
	int32 count = TerrainBuilder::GetSettingCount();
	if (index < count)
	{
		return (TerrainBuilder::GetSetting(index));
	}

	const StringTable *table = TheWorldEditor->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('TBLD', kTerrainBuilderRawHeightField, 'FILE'));
		return (new HeadingSetting('FILE', title));
	}

	if (index == count + 1)
	{
		const char *title = table->GetString(StringID('TBLD', kTerrainBuilderRawHeightField, 'IMAG'));
		const char *picker = table->GetString(StringID('TBLD', kTerrainBuilderRawHeightField, 'PICK'));
		return (new ResourceSetting('IMAG', imageName, title, picker, RawResource::GetDescriptor(), nullptr, kResourceSettingGenericPath | kResourceSettingImportCatalog));
	}

	if (index == count + 2)
	{
		const char *title = table->GetString(StringID('TBLD', kTerrainBuilderRawHeightField, 'WIDE'));
		return (new IntegerSetting('WIDE', imageWidth, title, 16, 1024, 16));
	}

	if (index == count + 3)
	{
		const char *title = table->GetString(StringID('TBLD', kTerrainBuilderRawHeightField, 'HIGH'));
		return (new IntegerSetting('HIGH', imageHeight, title, 16, 1024, 16));
	}

	if (index == count + 4)
	{
		const char *title = table->GetString(StringID('TBLD', kTerrainBuilderRawHeightField, 'CHAN'));
		MenuSetting *menu = new MenuSetting('CHAN', channelCount - 1, title, 4);

		menu->SetMenuItemString(0, table->GetString(StringID('TBLD', kTerrainBuilderRawHeightField, 'CHAN', '1111')));
		menu->SetMenuItemString(1, table->GetString(StringID('TBLD', kTerrainBuilderRawHeightField, 'CHAN', '2222')));
		menu->SetMenuItemString(2, table->GetString(StringID('TBLD', kTerrainBuilderRawHeightField, 'CHAN', '3333')));
		menu->SetMenuItemString(3, table->GetString(StringID('TBLD', kTerrainBuilderRawHeightField, 'CHAN', '4444')));

		return (menu);
	}

	if (index == count + 5)
	{
		const char *title = table->GetString(StringID('TBLD', kTerrainBuilderRawHeightField, 'DPTH'));
		MenuSetting *menu = new MenuSetting('DPTH', (channelDepth == 16), title, 2);

		menu->SetMenuItemString(0, table->GetString(StringID('TBLD', kTerrainBuilderRawHeightField, 'DPTH', '8888')));
		menu->SetMenuItemString(1, table->GetString(StringID('TBLD', kTerrainBuilderRawHeightField, 'DPTH', '1616')));

		return (menu);
	}

	if (index == count + 6)
	{
		const char *title = table->GetString(StringID('TBLD', kTerrainBuilderRawHeightField, 'PARM'));
		return (new HeadingSetting('PARM', title));
	}

	if (index == count + 7)
	{
		const char *title = table->GetString(StringID('TBLD', kTerrainBuilderRawHeightField, 'SCAL'));
		return (new TextSetting('SCAL', heightScale, title));
	}

	if (index == count + 8)
	{
		const char *title = table->GetString(StringID('TBLD', kTerrainBuilderRawHeightField, 'POSI'));
		return (new TextSetting('POSI', Text::IntegerToString(emptyHeight), title, 4, &EditTextWidget::NumberFilter));
	}

	if (index == count + 9)
	{
		const char *title = table->GetString(StringID('TBLD', kTerrainBuilderRawHeightField, 'NEGA'));
		return (new TextSetting('NEGA', Text::IntegerToString(solidDepth), title, 4, &EditTextWidget::NumberFilter));
	}

	return (nullptr);
}

void RawHeightFieldTerrainBuilder::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'IMAG')
	{
		imageName = static_cast<const ResourceSetting *>(setting)->GetResourceName();
		imageName[Text::GetResourceNameLength(imageName)] = 0;
	}
	else if (setting->GetSettingIdentifier() == 'WIDE')
	{
		imageWidth = static_cast<const IntegerSetting *>(setting)->GetIntegerValue();
	}
	else if (setting->GetSettingIdentifier() == 'HIGH')
	{
		imageHeight = static_cast<const IntegerSetting *>(setting)->GetIntegerValue();
	}
	else if (setting->GetSettingIdentifier() == 'CHAN')
	{
		channelCount = static_cast<const MenuSetting *>(setting)->GetMenuSelection() + 1;
	}
	else if (setting->GetSettingIdentifier() == 'DPTH')
	{
		channelDepth = static_cast<const MenuSetting *>(setting)->GetMenuSelection() * 8 + 8;
	}
	else if (setting->GetSettingIdentifier() == 'SCAL')
	{
		heightScale = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
	}
	else if (setting->GetSettingIdentifier() == 'POSI')
	{
		emptyHeight = Text::StringToInteger(static_cast<const TextSetting *>(setting)->GetText());
	}
	else if (setting->GetSettingIdentifier() == 'NEGA')
	{
		solidDepth = Text::StringToInteger(static_cast<const TextSetting *>(setting)->GetText());
	}
	else
	{
		TerrainBuilder::SetSetting(setting);
	}
}

unsigned_int8 *RawHeightFieldTerrainBuilder::GenerateHeightField8(const unsigned_int8 *data, int32 fieldWidth, int32 fieldHeight) const
{
	unsigned_int8 *heightField = new unsigned_int8[fieldWidth * fieldHeight];
	unsigned_int8 *field = heightField;

	int32 rowSize = fieldWidth * channelCount;
	data += rowSize * (fieldHeight - 1);

	if (channelCount == 1)
	{
		for (machine y = 0; y < fieldHeight; y++)
		{
			for (machine x = 0; x < fieldWidth; x++)
			{
				field[x] = data[x];
			}

			field += fieldWidth;
			data -= rowSize;
		}
	}
	else if (channelCount == 2)
	{
		for (machine y = 0; y < fieldHeight; y++)
		{
			for (machine x = 0; x < fieldWidth; x++)
			{
				int32 r = data[0];
				int32 g = data[1];
				data += 2;

				field[x] = (unsigned_int8) Max(r, g);
			}

			field += fieldWidth;
			data -= rowSize * 2;
		}
	}
	else if (channelCount == 3)
	{
		for (machine y = 0; y < fieldHeight; y++)
		{
			for (machine x = 0; x < fieldWidth; x++)
			{
				int32 r = data[0];
				int32 g = data[1];
				int32 b = data[2];
				data += 3;

				field[x] = (unsigned_int8) Max(Max(r, g), b);
			}

			field += fieldWidth;
			data -= rowSize * 2;
		}
	}
	else
	{
		for (machine y = 0; y < fieldHeight; y++)
		{
			for (machine x = 0; x < fieldWidth; x++)
			{
				int32 r = data[0];
				int32 g = data[1];
				int32 b = data[2];
				int32 a = data[3];
				data += 4;

				field[x] = (unsigned_int8) Max(Max(Max(r, g), b), a);
			}

			field += fieldWidth;
			data -= rowSize * 2;
		}
	}

	return (heightField);
}

unsigned_int16 *RawHeightFieldTerrainBuilder::GenerateHeightField16(const unsigned_int8 *data, int32 fieldWidth, int32 fieldHeight) const
{
	unsigned_int16 *heightField = new unsigned_int16[fieldWidth * fieldHeight];
	unsigned_int16 *field = heightField;

	int32 rowSize = fieldWidth * channelCount * 2;
	data += rowSize * (fieldHeight - 1);

	if (channelCount == 1)
	{
		for (machine y = 0; y < fieldHeight; y++)
		{
			for (machine x = 0; x < fieldWidth; x++)
			{
				field[x] = data[0] | (data[1] << 8);
				data += 2;
			}

			field += fieldWidth;
			data -= rowSize * 2;
		}
	}
	else if (channelCount == 2)
	{
		for (machine y = 0; y < fieldHeight; y++)
		{
			for (machine x = 0; x < fieldWidth; x++)
			{
				int32 r = data[0] | (data[1] << 8);
				int32 g = data[2] | (data[3] << 8);
				data += 4;

				field[x] = (unsigned_int16) Max(r, g);
			}

			field += fieldWidth;
			data -= rowSize * 2;
		}
	}
	else if (channelCount == 3)
	{
		for (machine y = 0; y < fieldHeight; y++)
		{
			for (machine x = 0; x < fieldWidth; x++)
			{
				int32 r = data[0] | (data[1] << 8);
				int32 g = data[2] | (data[3] << 8);
				int32 b = data[4] | (data[5] << 8);
				data += 6;

				field[x] = (unsigned_int16) Max(Max(r, g), b);
			}

			field += fieldWidth;
			data -= rowSize * 2;
		}
	}
	else
	{
		for (machine y = 0; y < fieldHeight; y++)
		{
			for (machine x = 0; x < fieldWidth; x++)
			{
				int32 r = data[0] | (data[1] << 8);
				int32 g = data[2] | (data[3] << 8);
				int32 b = data[4] | (data[5] << 8);
				int32 a = data[6] | (data[7] << 8);
				data += 8;

				field[x] = (unsigned_int16) Max(Max(Max(r, g), b), a);
			}

			field += fieldWidth;
			data -= rowSize * 2;
		}
	}

	return (heightField);
}

bool RawHeightFieldTerrainBuilder::BuildTerrain(TerrainBlock *block)
{
	bool result = false;

	RawResource *resource = RawResource::Get(imageName, 0, ThePluginMgr->GetImportCatalog());
	if (resource)
	{
		int32 maxPixelCount = resource->GetSize() / (channelCount * channelDepth / 8);

		int32 blockSizeX = imageWidth >> kTerrainLogDimension;
		int32 blockSizeY = Min(imageHeight, maxPixelCount / imageWidth) >> kTerrainLogDimension;
		if (blockSizeY > 0)
		{
			TerrainBlockObject *object = block->GetObject();
			float scale = object->GetVoxelScale() / (float) Max(blockSizeX, blockSizeY);
			object->SetVoxelScale(scale);

			int32 height = Max((int32) (heightScale / scale), 1);
			int32 blockSizeZ = (emptyHeight + solidDepth + height + (kTerrainDimension - 1)) >> kTerrainLogDimension;
			block->SetBlockSize(Integer3D(blockSizeX, blockSizeY, blockSizeZ));

			const unsigned_int8 *data = resource->GetImageData();
			if (channelDepth == 8)
			{
				unsigned_int8 *field = GenerateHeightField8(data, blockSizeX << kTerrainLogDimension, blockSizeY << kTerrainLogDimension);
				block->SetBlockToHeightField(field, height, solidDepth * 255);
				delete[] field;
			}
			else
			{
				unsigned_int16 *field = GenerateHeightField16(data, blockSizeX << kTerrainLogDimension, blockSizeY << kTerrainLogDimension);
				block->SetBlockToHeightField(field, height, solidDepth * 65535);
				delete[] field;
			}

			result = true;
		}

		resource->Release();
	}

	return (result);
}


RawVoxelMapTerrainBuilder::RawVoxelMapTerrainBuilder() : TerrainBuilder(kTerrainBuilderRawVoxelMap)
{
	voxelMapName[0] = 0;
	voxelMapWidth = kTerrainDimension;
	voxelMapHeight = kTerrainDimension;
	voxelMapDepth = kTerrainDimension;
	upDirectionIndex = 2;
}

RawVoxelMapTerrainBuilder::~RawVoxelMapTerrainBuilder()
{
}

int32 RawVoxelMapTerrainBuilder::GetSettingCount(void) const
{
	return (TerrainBuilder::GetSettingCount() + 6);
}

Setting *RawVoxelMapTerrainBuilder::GetSetting(int32 index) const
{
	int32 count = TerrainBuilder::GetSettingCount();
	if (index < count)
	{
		return (TerrainBuilder::GetSetting(index));
	}

	const StringTable *table = TheWorldEditor->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('TBLD', kTerrainBuilderRawVoxelMap, 'FILE'));
		return (new HeadingSetting('FILE', title));
	}

	if (index == count + 1)
	{
		const char *title = table->GetString(StringID('TBLD', kTerrainBuilderRawVoxelMap, 'VOXL'));
		const char *picker = table->GetString(StringID('TBLD', kTerrainBuilderRawVoxelMap, 'PICK'));
		return (new ResourceSetting('VOXL', voxelMapName, title, picker, RawResource::GetDescriptor(), nullptr, kResourceSettingGenericPath | kResourceSettingImportCatalog));
	}

	if (index == count + 2)
	{
		const char *title = table->GetString(StringID('TBLD', kTerrainBuilderRawVoxelMap, 'WIDE'));
		return (new IntegerSetting('WIDE', voxelMapWidth, title, 16, 1024, 16));
	}

	if (index == count + 3)
	{
		const char *title = table->GetString(StringID('TBLD', kTerrainBuilderRawVoxelMap, 'HIGH'));
		return (new IntegerSetting('HIGH', voxelMapHeight, title, 16, 1024, 16));
	}

	if (index == count + 4)
	{
		const char *title = table->GetString(StringID('TBLD', kTerrainBuilderRawVoxelMap, 'DEEP'));
		return (new IntegerSetting('DEEP', voxelMapDepth, title, 16, 1024, 16));
	}

	if (index == count + 5)
	{
		const char *title = table->GetString(StringID('TBLD', kTerrainBuilderRawVoxelMap, 'UDIR'));
		MenuSetting *menu = new MenuSetting('UDIR', upDirectionIndex - 1, title, 2);

		menu->SetMenuItemString(0, table->GetString(StringID('TBLD', kTerrainBuilderRawVoxelMap, 'UDIR', 'YYYY')));
		menu->SetMenuItemString(1, table->GetString(StringID('TBLD', kTerrainBuilderRawVoxelMap, 'UDIR', 'ZZZZ')));

		return (menu);
	}

	return (nullptr);
}

void RawVoxelMapTerrainBuilder::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'VOXL')
	{
		voxelMapName = static_cast<const ResourceSetting *>(setting)->GetResourceName();
		voxelMapName[Text::GetResourceNameLength(voxelMapName)] = 0;
	}
	else if (setting->GetSettingIdentifier() == 'WIDE')
	{
		voxelMapWidth = static_cast<const IntegerSetting *>(setting)->GetIntegerValue();
	}
	else if (setting->GetSettingIdentifier() == 'HIGH')
	{
		voxelMapHeight = static_cast<const IntegerSetting *>(setting)->GetIntegerValue();
	}
	else if (setting->GetSettingIdentifier() == 'DEEP')
	{
		voxelMapDepth = static_cast<const IntegerSetting *>(setting)->GetIntegerValue();
	}
	else if (setting->GetSettingIdentifier() == 'UDIR')
	{
		upDirectionIndex = static_cast<const MenuSetting *>(setting)->GetMenuSelection() + 1;
	}
	else
	{
		TerrainBuilder::SetSetting(setting);
	}
}

bool RawVoxelMapTerrainBuilder::BuildTerrain(TerrainBlock *block)
{
	bool result = false;

	RawResource *resource = RawResource::Get(voxelMapName, 0, ThePluginMgr->GetImportCatalog());
	if (resource)
	{
		const float *data = resource->GetVoxelMapData();
		float *rotatedData = nullptr;

		int32 maxVoxelCount = resource->GetSize() >> 2;

		int32 width = voxelMapWidth;
		int32 height = voxelMapHeight;
		int32 depth = Min(voxelMapDepth, maxVoxelCount / (width * height));

		if (upDirectionIndex == 1)
		{
			rotatedData = new float[maxVoxelCount];

			int32 oldHeight = height;
			int32 oldDepth = depth;
			height = oldDepth;
			depth = oldHeight;

			for (machine k = 0; k < depth; k++)
			{
				for (machine j = 0; j < height; j++)
				{
					for (machine i = 0; i < width; i++)
					{
						rotatedData[i * height * depth + j * depth + k] = data[i * oldHeight * oldDepth + k * oldDepth + (oldDepth - 1 - j)];
					}
				}
			}

			data = rotatedData;
		}

		int32 blockSizeX = width >> kTerrainLogDimension;
		int32 blockSizeY = height >> kTerrainLogDimension;
		int32 blockSizeZ = depth >> kTerrainLogDimension;
		if (blockSizeZ > 0)
		{
			TerrainBlockObject *object = block->GetObject();
			float scale = object->GetVoxelScale() / (float) Max(blockSizeX, blockSizeY);
			object->SetVoxelScale(scale);

			block->SetBlockSize(Integer3D(blockSizeX, blockSizeY, blockSizeZ));

			Voxel *voxel = new Voxel[kTerrainDimension * kTerrainDimension * kTerrainDimension];

			for (machine k = 0; k < blockSizeZ; k++)
			{
				for (machine j = 0; j < blockSizeY; j++)
				{
					for (machine i = 0; i < blockSizeX; i++)
					{
						for (machine x = 0; x < kTerrainDimension; x++)
						{
							const float *d1 = data + ((x + i * kTerrainDimension) * height * depth + k * kTerrainDimension);
							for (machine y = 0; y < kTerrainDimension; y++)
							{
								const float *d2 = d1 + (y + j * kTerrainDimension) * depth;
								for (machine z = 0; z < kTerrainDimension; z++)
								{
									int32 v = Min(Max((int32) (d2[z] * -63.5F), -0x7F), 0x7F);
									voxel[(z * kTerrainDimension + y) * kTerrainDimension + x] = (Voxel) v;
								}
							}
						}

						block->StoreVoxels(kTerrainSubchannelDensity, Integer3D(i, j, k), voxel, kTerrainDimension, kTerrainDimension * kTerrainDimension);
					}
				}
			}

			delete[] voxel;
			result = true;
		}

		delete[] rotatedData;
		resource->Release();
	}

	return (result);
}


BuildTerrainWindow::BuildTerrainWindow(Editor *editor, TerrainBlock *block) : Window("WorldEditor/terrain/Build")
{
	worldEditor = editor;
	blockNode = block;

	currentBuilder = nullptr;
	for (machine a = 0; a < kTerrainBuilderCount; a++)
	{
		terrainBuilder[a] = nullptr;
	}
}

BuildTerrainWindow::~BuildTerrainWindow()
{
	for (machine a = kTerrainBuilderCount - 1; a >= 0; a--)
	{
		delete terrainBuilder[a];
	}
}

void BuildTerrainWindow::Preprocess(void)
{
	Window::Preprocess();

	buildButton = static_cast<PushButtonWidget *>(FindWidget("Build"));
	cancelButton = static_cast<PushButtonWidget *>(FindWidget("Cancel"));

	builderList = static_cast<ListWidget *>(FindWidget("List"));
	configurationWidget = static_cast<ConfigurationWidget *>(FindWidget("Config"));

	const StringTable *table = TheWorldEditor->GetStringTable();
	builderList->AppendListItem(table->GetString(StringID('TBLD', kTerrainBuilderFlatPlane)));
	builderList->AppendListItem(table->GetString(StringID('TBLD', kTerrainBuilderTargaHeightField)));
	builderList->AppendListItem(table->GetString(StringID('TBLD', kTerrainBuilderRawHeightField)));
	builderList->AppendListItem(table->GetString(StringID('TBLD', kTerrainBuilderRawVoxelMap)));

	SetFocusWidget(builderList);
}

void BuildTerrainWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	EventType eventType = eventData->eventType;

	if (eventType == kEventWidgetActivate)
	{
		if (widget == buildButton)
		{
			configurationWidget->CommitConfiguration(currentBuilder);

			terrainBuilder[builderList->GetFirstSelectedIndex()] = nullptr;
			TerrainProgressWindow *window = new TerrainProgressWindow(worldEditor, blockNode, currentBuilder);
			worldEditor->AddSubwindow(window);

			Close();
		}
		else if (widget == cancelButton)
		{
			worldEditor->DeleteNode(blockNode);
			worldEditor->DeleteLastOperation();
			Close();
		}
	}
	else if (eventType == kEventWidgetChange)
	{
		if (widget == builderList)
		{
			if (currentBuilder)
			{
				configurationWidget->CommitConfiguration(currentBuilder);
				configurationWidget->ReleaseConfiguration();
			}

			int32 index = builderList->GetFirstSelectedIndex();
			currentBuilder = terrainBuilder[index];
			if (!currentBuilder)
			{
				if (index == 0)
				{
					currentBuilder = new FlatPlaneTerrainBuilder;
				}
				else if (index == 1)
				{
					currentBuilder = new TargaHeightFieldTerrainBuilder;
				}
				else if (index == 2)
				{
					currentBuilder = new RawHeightFieldTerrainBuilder;
				}
				else if (index == 3)
				{
					currentBuilder = new RawVoxelMapTerrainBuilder;
				}

				terrainBuilder[index] = currentBuilder;
			}

			configurationWidget->BuildConfiguration(currentBuilder);
			buildButton->Enable();
		}
	}
}


TerrainProgressWindow::TerrainProgressWindow(Editor *editor, TerrainBlock *block, TerrainBuilder *builder) :
		Window("WorldEditor/terrain/Progress"),
		terrainJob(&JobBuildTerrain, this)
{
	worldEditor = editor;
	blockNode = block;
	terrainBuilder = builder;

	buildSuccess = false;
	TheJobMgr->SubmitJob(&terrainJob);
}

TerrainProgressWindow::~TerrainProgressWindow()
{
	TheJobMgr->CancelJob(&terrainJob);
	delete terrainBuilder;
}

void TerrainProgressWindow::JobBuildTerrain(Job *job, void *cookie)
{
	TerrainProgressWindow *window = static_cast<TerrainProgressWindow *>(cookie);

	if (window->terrainBuilder->BuildTerrain(window->blockNode))
	{
		const Integer3D& size0 = window->blockNode->GetBlockSize();
		Integer3D size1((size0.x + 1) & ~1, (size0.y + 1) & ~1, (size0.z + 1) & ~1);
		Integer3D size2((size0.x + 3) & ~3, (size0.y + 3) & ~3, (size0.z + 3) & ~3);

		int32 m = size0.x * size0.y * size0.z + size1.x * size1.y * size1.z / 8 + size2.x * size2.y * size2.z / 64;
		window->geometryArray.Reserve(m);
		job->SetJobMagnitude(m);

		int32 progress = 0;
		for (machine k = 0; k < size2.z; k += 4)
		{
			for (machine j = 0; j < size2.y; j += 4)
			{
				for (machine i = 0; i < size2.x; i += 4)
				{
					TerrainGeometry *geometry = new TerrainLevelGeometry(window->blockNode, Integer3D(i, j, k), 2);
					TerrainGeometryObject *object = geometry->GetObject();
					object->Build(geometry);

					if (object->GetGeometryLevel(0)->GetPrimitiveCount() != 0)
					{
						window->geometryArray.AddElement(geometry);
					}
					else
					{
						delete geometry;
					}

					if (job->Cancelled())
					{
						return;
					}

					job->SetJobProgress(++progress);
				}
			}
		}

		for (machine k = 0; k < size1.z; k += 2)
		{
			for (machine j = 0; j < size1.y; j += 2)
			{
				for (machine i = 0; i < size1.x; i += 2)
				{
					TerrainGeometry *geometry = new TerrainLevelGeometry(window->blockNode, Integer3D(i, j, k), 1);
					TerrainGeometryObject *object = geometry->GetObject();
					object->Build(geometry);

					if (object->GetGeometryLevel(0)->GetPrimitiveCount() != 0)
					{
						window->geometryArray.AddElement(geometry);
					}
					else
					{
						delete geometry;
					}

					if (job->Cancelled())
					{
						return;
					}

					job->SetJobProgress(++progress);
				}
			}
		}

		for (machine k = 0; k < size0.z; k++)
		{
			for (machine j = 0; j < size0.y; j++)
			{
				for (machine i = 0; i < size0.x; i++)
				{
					TerrainGeometry *geometry = new TerrainGeometry(window->blockNode, Integer3D(i, j, k));
					TerrainGeometryObject *object = geometry->GetObject();
					object->Build(geometry);

					if (object->GetGeometryLevel(0)->GetPrimitiveCount() != 0)
					{
						window->geometryArray.AddElement(geometry);
					}
					else
					{
						delete geometry;
					}

					if (job->Cancelled())
					{
						return;
					}

					job->SetJobProgress(++progress);
				}
			}
		}

		Thread::Fence();
		window->buildSuccess = true;
	}
}

void TerrainProgressWindow::Preprocess(void)
{
	Window::Preprocess();

	stopButton = static_cast<PushButtonWidget *>(FindWidget("Stop"));
	progressBar = static_cast<ProgressWidget *>(FindWidget("Progress"));
}

void TerrainProgressWindow::Move(void)
{
	Window::Move();

	if (terrainJob.Complete())
	{
		if (buildSuccess)
		{
			MaterialObject *materialObject = worldEditor->GetSelectedMaterial()->GetMaterialObject();
			for (Geometry *geometry : geometryArray)
			{
				blockNode->AddTerrainGeometry(static_cast<TerrainGeometry *>(geometry));

				EditorManipulator::Install(worldEditor, geometry);
				Editor::GetManipulator(geometry)->InvalidateGraph();

				geometry->SetMaterialObject(0, materialObject);
				geometry->Preprocess();
			}

			blockNode->ProcessStructure();

			worldEditor->PostEvent(GizmoEditorEvent(kEditorEventGizmoTargetInvalidated, blockNode));
			worldEditor->InvalidateAllViewports();
		}
		else
		{
			worldEditor->DeleteNode(blockNode);
			worldEditor->DeleteLastOperation();
		}

		Close();
	}
	else
	{
		progressBar->SetProgress(terrainJob.GetJobProgress(), terrainJob.GetJobMagnitude());
	}
}

void TerrainProgressWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if ((eventData->eventType == kEventWidgetActivate) && (widget == stopButton))
	{
		TheJobMgr->CancelJob(&terrainJob);
	}
}

// ZYUQURM
