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


#include "C4TextureTool.h"
#include "C4TextureViewer.h"
#include "C4TextureGenerator.h"
#include "C4TerrainPalette.h"
#include "C4Targa.h"
#include "C4World.h"


using namespace C4;


TextureTool *C4::TheTextureTool = nullptr;


C4::Plugin *CreatePlugin(void)
{
	return (new TextureTool);
}


TextureTool::TextureTool() :
		Singleton<TextureTool>(TheTextureTool),
		stringTable("TextureTool/strings"),

		textureCommandObserver(this, &TextureTool::HandleTextureCommand),
		textureCommand("texture", &textureCommandObserver),
		textureMenuItem(stringTable.GetString(StringID('VIEW', 'MCMD')), WidgetObserver<TextureTool>(this, &TextureTool::HandleOpenTextureMenuItem)),

		importTextureCommandObserver(this, &TextureTool::HandleImportTextureCommand),
		importTextureCommand("itexture", &importTextureCommandObserver),
		importTextureMenuItem(stringTable.GetString(StringID('IMPT', 'MCMD')), WidgetObserver<TextureTool>(this, &TextureTool::HandleImportTextureMenuItem)),

		terrainPaletteCommandObserver(this, &TextureTool::HandleTerrainPaletteCommand),
		terrainPaletteCommand("texpal", &terrainPaletteCommandObserver),
		terrainPaletteMenuItem(stringTable.GetString(StringID('TPAL', 'MCMD')), WidgetObserver<TextureTool>(this, &TextureTool::HandleTerrainPaletteMenuItem)),

		generateTexturesCommandObserver(this, &TextureTool::HandleGenerateTexturesCommand),
		generateTexturesCommand("gentex", &generateTexturesCommandObserver),
		generateTexturesMenuItem(stringTable.GetString(StringID('TGEN', 'MCMD')), WidgetObserver<TextureTool>(this, &TextureTool::HandleGenerateTexturesMenuItem)),

		updateTexturesCommandObserver(this, &TextureTool::HandleUpdateTexturesCommand),
		updateTexturesCommand("updatetextures", &updateTexturesCommandObserver)
{
	TheEngine->AddCommand(&textureCommand);
	TheEngine->AddCommand(&importTextureCommand);
	TheEngine->AddCommand(&terrainPaletteCommand);
	TheEngine->AddCommand(&generateTexturesCommand);
	TheEngine->AddCommand(&updateTexturesCommand);

	ThePluginMgr->AddToolMenuItem(&textureMenuItem);
	ThePluginMgr->AddToolMenuItem(&importTextureMenuItem);
	ThePluginMgr->AddToolMenuItem(&terrainPaletteMenuItem);
	ThePluginMgr->AddToolMenuItem(&generateTexturesMenuItem);
}

TextureTool::~TextureTool()
{
	FilePicker *picker = texturePicker;
	delete picker;

	picker = targaPicker;
	delete picker;

	ImportTextureWindow::windowList.Purge();
	TextureWindow::windowList.Purge();

	delete TheTerrainPaletteWindow;
	delete TheTextureGeneratorWindow;
}

void TextureTool::TexturePicked(FilePicker *picker, void *cookie)
{
	ResourceName	name;

	if (picker)
	{
		name = picker->GetResourceName();
	}
	else
	{
		name = static_cast<const char *>(cookie);
	}

	ResourceResult result = TextureWindow::Open(name);
	if (result != kResourceOkay)
	{
		const StringTable *table = TheTextureTool->GetStringTable();
		String<kMaxCommandLength> output(table->GetString(StringID('VIEW', 'NRES')));
		output += name;
		Engine::Report(output);
	}
}

void TextureTool::HandleOpenTextureMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	FilePicker *picker = texturePicker;
	if (picker)
	{
		TheInterfaceMgr->SetActiveWindow(picker);
	}
	else 
	{
		const char *title = stringTable.GetString(StringID('VIEW', 'OPEN'));
 
		picker = new TexturePicker('TXTR', title, TheResourceMgr->GetGenericCatalog(), TextureResource::GetDescriptor());
		picker->SetCompletionProc(&TexturePicked); 

		texturePicker = picker;
		TheInterfaceMgr->AddWidget(picker); 
	}
} 
 
void TextureTool::HandleTextureCommand(Command *command, const char *text)
{
	if (*text != 0)
	{ 
		ResourceName	name;

		Text::ReadString(text, name, kMaxResourceNameLength);
		TexturePicked(nullptr, &name[0]);
	}
	else
	{
		HandleOpenTextureMenuItem(nullptr, nullptr);
	}
}

void TextureTool::ImportTexturePicked(FilePicker *picker, void *cookie)
{
	if (picker)
	{
		ResourceName name(picker->GetFileName());
		name[Text::GetResourceNameLength(name)] = 0;
		ImportTextureWindow::Open(name);
	}
	else
	{
		ImportTextureWindow::Open(static_cast<const char *>(cookie));
	}
}

void TextureTool::HandleImportTextureMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	FilePicker *picker = targaPicker;
	if (picker)
	{
		TheInterfaceMgr->SetActiveWindow(picker);
	}
	else
	{
		const char *title = stringTable.GetString(StringID('IMPT', 'OPEN'));

		picker = new FilePicker('ITXT', title, ThePluginMgr->GetImportCatalog(), TargaResource::GetDescriptor());
		picker->SetCompletionProc(&ImportTexturePicked);

		targaPicker = picker;
		TheInterfaceMgr->AddWidget(picker);
	}
}

void TextureTool::HandleImportTextureCommand(Command *command, const char *text)
{
	if (*text != 0)
	{
		String<kMaxResourceNameLength>	name;

		text += Text::ReadString(text, name, kMaxResourceNameLength);
		text += Data::GetWhitespaceLength(text);

		if (*text == 0)
		{
			ImportTexturePicked(nullptr, &name[0]);
		}
		else
		{
			TextureImporter importer(name);
			importer.ProcessCommandLine(text);
			importer.ImportTexture(name);
		}
	}
	else
	{
		HandleImportTextureMenuItem(nullptr, nullptr);
	}
}

void TextureTool::HandleTerrainPaletteMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	HandleTerrainPaletteCommand(nullptr, nullptr);
}

void TextureTool::HandleTerrainPaletteCommand(Command *command, const char *text)
{
	ResourceName			outputName;
	Array<ResourceName>		inputNameArray;

	outputName[0] = 0;
	if (text)
	{
		for (;;)
		{
			ResourceName	param;

			text += Data::GetWhitespaceLength(text);
			if (*text == 0)
			{
				break;
			}

			text += Text::ReadString(text, param, kMaxResourceNameLength);

			if (param == "-o")
			{
				text += Data::GetWhitespaceLength(text);
				text += Text::ReadString(text, outputName, kMaxResourceNameLength);
			}
			else
			{
				inputNameArray.AddElement(param);
			}
		}
	}

	if ((outputName[0] == 0) || (inputNameArray.Empty()))
	{
		TerrainPaletteWindow::Open();
		return;
	}

	TerrainPaletteWindow::GenerateTerrainPalette(outputName, inputNameArray);
}

void TextureTool::HandleGenerateTexturesMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	HandleGenerateTexturesCommand(nullptr, nullptr);
}

void TextureTool::HandleGenerateTexturesCommand(Command *command, const char *text)
{
	World *world = TheWorldMgr->GetWorld();
	if (world)
	{
		TextureGeneratorWindow::Open();
	}
}

void TextureTool::HandleUpdateTexturesCommand(Command *command, const char *text)
{
	UpdateTexturesDirectory("");
}

void TextureTool::UpdateTexturesDirectory(const char *directory)
{
	Map<FileReference>		fileMap;

	TheResourceMgr->GetGenericCatalog()->BuildResourceMap(TextureResource::GetDescriptor(), directory, &fileMap);
	FileReference *reference = fileMap.First();
	while (reference)
	{
		String<> path(directory);
		if (directory[0] != 0)
		{
			path += '/';
		}

		if (!(reference->GetFlags() & kFileDirectory))
		{
			ResourceName name(reference->GetName());
			int32 len = Text::GetResourceNameLength(name);
			if ((len > 3) && (name[len - 3] == '-') && (name[len - 2] == 'h') && (name[len - 1] == '1'))
			{
				name[len] = 0;
				path += name;
				UpdateTextureResource(&path[Text::GetPrefixDirectoryLength(path)]);
			}
		}
		else
		{
			UpdateTexturesDirectory(path += reference->GetName());
		}

		reference = reference->Next();
	}
}

void TextureTool::UpdateTextureResource(const char *name1)
{
	ResourceLocation	resourceLocation;

	TextureResource *resource1 = TextureResource::Get(name1, kResourceIgnorePackFiles, nullptr, &resourceLocation);
	if (resource1)
	{
		int32 baseLength = Text::GetTextLength(name1) - 3;

		ResourcePath name2(name1);
		name2[baseLength + 2] = '2';

		TextureResource *resource2 = TextureResource::Get(name2, kResourceIgnorePackFiles, nullptr, &resourceLocation);
		if (resource2)
		{
			const TextureHeader *textureHeader1 = resource1->GetTextureHeader();
			const TextureHeader *textureHeader2 = resource2->GetTextureHeader();

			if ((textureHeader1->colorSemantic == 'HOR1') && (textureHeader2->colorSemantic == 'HOR2'))
			{
				File			outputFile;
				ResourcePath	outputPath;

				ResourcePath outputName(name1);

				if ((baseLength > 5) && (Text::CompareText(&outputName[baseLength - 5], "-nrml")))
				{
					outputName[baseLength - 4] = 'h';
					outputName[baseLength - 3] = 'r';
					outputName[baseLength - 2] = 'z';
					outputName[baseLength - 1] = 'n';
					outputName[baseLength] = 0;
				}
				else
				{
					outputName[baseLength + 2] = 0;
				}

				TheResourceMgr->GetGenericCatalog()->GetResourcePath(TextureResource::GetDescriptor(), outputName, &resourceLocation, &outputPath);
				if (outputFile.Open(outputPath, kFileCreate) == kFileOkay)
				{
					TextureResourceHeader	outputResourceHeader;
					TextureHeader			outputTextureHeader;

					const TextureMipmapData *mipmapData1 = textureHeader1->GetMipmapData();
					const TextureMipmapData *mipmapData2 = textureHeader2->GetMipmapData();

					int32 width = textureHeader1->imageWidth;
					int32 height = textureHeader1->imageHeight;
					int32 mipmapCount = textureHeader1->mipmapCount;

					outputResourceHeader.endian = 1;
					outputResourceHeader.headerDataSize = sizeof(TextureHeader) + mipmapCount * sizeof(TextureMipmapData);
					outputResourceHeader.textureCount = 1;

					outputTextureHeader.textureType = kTextureArray2D;
					outputTextureHeader.textureFlags = 0;
					outputTextureHeader.colorSemantic = kTextureSemanticHorizon;
					outputTextureHeader.alphaSemantic = kTextureSemanticHorizon;
					outputTextureHeader.imageFormat = kTextureRGBA8;
					outputTextureHeader.imageWidth = width;
					outputTextureHeader.imageHeight = height;
					outputTextureHeader.imageDepth = 2;
					outputTextureHeader.wrapMode[0] = textureHeader1->wrapMode[0];
					outputTextureHeader.wrapMode[1] = textureHeader1->wrapMode[1];
					outputTextureHeader.wrapMode[2] = kTextureClamp;
					outputTextureHeader.mipmapCount = mipmapCount;
					outputTextureHeader.mipmapDataOffset = sizeof(TextureHeader);
					outputTextureHeader.auxiliaryDataSize = 0;
					outputTextureHeader.auxiliaryDataOffset = 0;

					TextureMipmapData *outputMipmapData = new TextureMipmapData[mipmapCount];
					MemoryMgr::ClearMemory(outputMipmapData, mipmapCount * sizeof(TextureMipmapData));

					outputFile.Write(&outputResourceHeader, sizeof(TextureResourceHeader));
					outputFile.Write(&outputTextureHeader, sizeof(TextureHeader));
					outputFile.Write(&outputMipmapData, mipmapCount * sizeof(TextureMipmapData));

					char *imageData = new char[width * height * 8];
					unsigned_int8 *compressedData = new unsigned_int8[width * height * 8];

					int32 outputImageOffset = mipmapCount * sizeof(TextureMipmapData);

					for (machine a = 0; a < mipmapCount; a++)
					{
						unsigned_int32 imageSize = width * height * 4;
						const void *mipmapImage1 = mipmapData1[a].GetMipmapImage();
						const void *mipmapImage2 = mipmapData2[a].GetMipmapImage();

						if (mipmapData1[a].compressionType == kTextureCompressionGeneral)
						{
							Comp::DecompressData(static_cast<const unsigned_int8 *>(mipmapImage1), mipmapData1[a].imageSize, imageData);
						}
						else
						{
							MemoryMgr::CopyMemory(mipmapImage1, imageData, imageSize);
						}

						if (mipmapData2[a].compressionType == kTextureCompressionGeneral)
						{
							Comp::DecompressData(static_cast<const unsigned_int8 *>(mipmapImage2), mipmapData2[a].imageSize, imageData + imageSize);
						}
						else
						{
							MemoryMgr::CopyMemory(mipmapImage2, imageData + imageSize, imageSize);
						}

						unsigned_int32 outputImageSize = Comp::CompressData(imageData, imageSize * 2, compressedData);
						if (outputImageSize != 0)
						{
							outputMipmapData[a].compressionType = kTextureCompressionGeneral;
							outputFile.Write(compressedData, outputImageSize);
						}
						else
						{
							outputMipmapData[a].compressionType = kTextureCompressionNone;
							outputImageSize = imageSize * 2;
							outputFile.Write(imageData, outputImageSize);
						}

						outputMipmapData[a].imageOffset = outputImageOffset;
						outputMipmapData[a].imageSize = outputImageSize;
						outputMipmapData[a].chainSize = outputImageSize;

						for (machine b = 0; b < a; b++)
						{
							outputMipmapData[b].chainSize += outputImageSize;
						}

						width = Max(width >> 1, 1);
						height = Max(height >> 1, 1);
						outputImageOffset += outputImageSize - sizeof(TextureMipmapData);
					}

					outputFile.SetPosition(sizeof(TextureResourceHeader) + sizeof(TextureHeader));
					outputFile.Write(outputMipmapData, mipmapCount * sizeof(TextureMipmapData));

					delete[] compressedData;
					delete[] imageData;
					delete[] outputMipmapData;

					TheResourceMgr->GetGenericCatalog()->GetResourcePath(TextureResource::GetDescriptor(), name1, &resourceLocation, &outputPath);
					FileMgr::DeleteFile(outputPath);

					TheResourceMgr->GetGenericCatalog()->GetResourcePath(TextureResource::GetDescriptor(), name2, &resourceLocation, &outputPath);
					FileMgr::DeleteFile(outputPath);
				}
			}

			resource2->Release();
		}

		resource1->Release();
	}
}

// ZYUQURM
