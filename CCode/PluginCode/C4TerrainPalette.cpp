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


#include "C4TerrainPalette.h"


using namespace C4;


TerrainPaletteWindow *C4::TheTerrainPaletteWindow = nullptr;


TerrainPaletteWindow::TerrainPaletteWindow() :
		Window("TextureTool/TerrainPalette"),
		Singleton<TerrainPaletteWindow>(TheTerrainPaletteWindow)
{
}

TerrainPaletteWindow::~TerrainPaletteWindow()
{
}

void TerrainPaletteWindow::Open(void)
{
	if (TheTerrainPaletteWindow)
	{
		TheInterfaceMgr->SetActiveWindow(TheTerrainPaletteWindow);
	}
	else
	{
		TheInterfaceMgr->AddWidget(new TerrainPaletteWindow);
	}
}

void TerrainPaletteWindow::Preprocess(void)
{
	Window::Preprocess();

	saveButton = static_cast<PushButtonWidget *>(FindWidget("Save"));
	cancelButton = static_cast<PushButtonWidget *>(FindWidget("Cancel"));
	addButton = static_cast<PushButtonWidget *>(FindWidget("Add"));
	removeButton = static_cast<PushButtonWidget *>(FindWidget("Remove"));
	textureList = static_cast<ListWidget *>(FindWidget("List"));
}

void TerrainPaletteWindow::OutputTexturePicked(FilePicker *picker, void *cookie)
{
	Array<ResourceName>		inputNameArray;

	TerrainPaletteWindow *window = static_cast<TerrainPaletteWindow *>(cookie);
	const ListWidget *textureList = window->textureList;

	const Widget *listItem = textureList->GetFirstListItem();
	while (listItem)
	{
		const Widget *widget = listItem->GetFirstSubnode();
		while (widget)
		{
			if (widget->GetWidgetType() == kWidgetText)
			{
				const TextWidget *textWidget = static_cast<const TextWidget *>(widget);
				inputNameArray.AddElement(ResourceName(textWidget->GetText()));
				break;
			}

			widget = widget->Next();
		}

		listItem = listItem->Next();
	}

	ResourcePath outputName(picker->GetFileName());
	outputName[Text::GetResourceNameLength(outputName)] = 0;
	GenerateTerrainPalette(outputName, inputNameArray);

	delete window;
}

void TerrainPaletteWindow::PaletteEntryPicked(FilePicker *picker, void *cookie)
{
	TerrainPaletteWindow *window = static_cast<TerrainPaletteWindow *>(cookie);
	const StringTable *table = TheTextureTool->GetStringTable();

	ListWidget *textureList = window->textureList;
	float listWidth = textureList->GetWidgetSize().x;

	int32 count = picker->GetFileNameCount();
	for (machine a = 0; a < count; a++)
	{
		ResourceName name = picker->GetResourceName(a);
		if (name[0] != 0)
		{
			Widget *widget = new Widget;

			ImageWidget *imageWidget = new ImageWidget(Vector2D(48.0F, 48.0F), name);
			imageWidget->SetImageBlendState(kBlendReplace);
			imageWidget->SetWidgetPosition(Point3D(4.0F, 4.0F, 0.0F));
			widget->AppendSubnode(imageWidget);

			BorderWidget *borderWidget = new BorderWidget(Vector2D(48.0F, 48.0F));
			borderWidget->SetWidgetPosition(Point3D(4.0F, 4.0F, 0.0F));
			widget->AppendSubnode(borderWidget);

			TextWidget *textWidget = new TextWidget(Vector2D(listWidth - 56.0F, 14.0F), name, "font/Gui"); 
			textWidget->SetWidgetPosition(Point3D(60.0F, 14.0F, 0.0F));
			widget->AppendSubnode(textWidget);
 
			const Texture *texture = imageWidget->GetTexture();
			String<127> string(table->GetString(StringID('TYPE', texture->GetTextureType()))); 
			(string += ", ") += texture->GetTextureWidth();
			(string += " \xC3\x97 ") += texture->GetTextureHeight();		// U+00D7
			(string += ", ") += table->GetString(StringID('FORM', texture->GetImageFormat())); 

			TextureSemantic alpha = texture->GetAlphaSemantic(); 
			(string += ", ") += table->GetString(StringID('ALFA', (alpha == kTextureSemanticNone) ? 'NONE' : alpha)); 

			textWidget = new TextWidget(Vector2D(listWidth - 56.0F, 14.0F), string, "font/Normal");
			textWidget->SetWidgetPosition(Point3D(60.0F, 28.0F, 0.0F));
			widget->AppendSubnode(textWidget); 

			textureList->AppendListItem(widget);
		}
	}

	if (textureList->GetFirstListItem())
	{
		window->saveButton->Enable();
	}
}

void TerrainPaletteWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	EventType eventType = eventData->eventType;
	if (eventType == kEventWidgetActivate)
	{
		if (widget == saveButton)
		{
			const char *title = TheTextureTool->GetStringTable()->GetString(StringID('TPAL', 'SAVE'));
			const ResourceDescriptor *descriptor = TextureResource::GetDescriptor();
			FilePicker *picker = new FilePicker(descriptor->GetType(), title, TheResourceMgr->GetGenericCatalog(), descriptor, nullptr, kFilePickerSave);
			picker->SetCompletionProc(&OutputTexturePicked, this);
			AddSubwindow(picker);
		}
		else if (widget == cancelButton)
		{
			Close();
		}
		else if (widget == addButton)
		{
			const char *title = TheTextureTool->GetStringTable()->GetString(StringID('TPAL', 'ADDT'));
			TexturePicker *picker = new TexturePicker('TPAL', title, TheResourceMgr->GetGenericCatalog(), TextureResource::GetDescriptor(), nullptr, kFilePickerMultipleSelection);
			picker->SetCompletionProc(&PaletteEntryPicked, this);
			AddSubwindow(picker);
		}
		else if (widget == removeButton)
		{
			for (;;)
			{
				Widget *listItem = textureList->GetFirstSelectedListItem();
				if (!listItem)
				{
					break;
				}

				textureList->RemoveListItem(listItem);
				delete listItem;
			}

			if (!textureList->GetFirstListItem())
			{
				saveButton->Disable();
			}

			removeButton->Disable();
		}
	}
	else if (eventType == kEventWidgetChange)
	{
		if (widget == textureList)
		{
			if (textureList->GetFirstSelectedListItem())
			{
				removeButton->Enable();
			}
			else
			{
				removeButton->Disable();
			}
		}
	}
}

void TerrainPaletteWindow::GenerateTerrainPalette(const char *outputName, const Array<ResourceName>& inputNameArray)
{
	ResourcePath			outputPath;
	TextureHeader			referenceTextureHeader;
	TextureHeader			*textureHeader;
	TextureResourceHeader	resourceHeader;
	ChunkHeader				chunkHeader;
	File					outputFile;

	int32 count = inputNameArray.GetElementCount();

	referenceTextureHeader.textureType = kTexture2D;
	referenceTextureHeader.textureFlags = kTextureReferenceList;
	referenceTextureHeader.imageDepth = 1;
	referenceTextureHeader.wrapMode[0] = kTextureRepeat;
	referenceTextureHeader.wrapMode[1] = kTextureRepeat;
	referenceTextureHeader.wrapMode[2] = kTextureRepeat;
	referenceTextureHeader.mipmapDataOffset = 0;
	referenceTextureHeader.auxiliaryDataSize = sizeof(ChunkHeader) + 4 + sizeof(ResourceName) * count;
	referenceTextureHeader.auxiliaryDataOffset = sizeof(TextureHeader);

	for (machine a = 0; a < count; a++)
	{
		ResourceLoader		loader;

		const ResourceName& name = inputNameArray[a];
		TextureResource *resource = TextureResource::Get(name, kResourceDeferLoad);

		ResourceResult result = resource->OpenLoader(&loader);
		if (result != kResourceOkay)
		{
			resource->Release();

			Engine::Report((String<>("Error opening ") += name) += ".");
			return;
		}

		result = resource->LoadHeaderData(&loader, &resourceHeader, &textureHeader);
		if (result != kResourceOkay)
		{
			resource->Release();

			Engine::Report((String<>("Error opening ") += name) += ".");
			return;
		}

		if ((textureHeader->textureType != kTexture2D) || (textureHeader->textureFlags & kTextureReferenceList))
		{
			TextureResource::ReleaseHeaderData(textureHeader);
			resource->Release();

			Engine::Report("Textures must be 2D.");
			return;
		}

		if (textureHeader->imageWidth != textureHeader->imageHeight)
		{
			TextureResource::ReleaseHeaderData(textureHeader);
			resource->Release();

			Engine::Report("Textures must be square.");
			return;
		}

		if (textureHeader->imageWidth < 32)
		{
			TextureResource::ReleaseHeaderData(textureHeader);
			resource->Release();

			Engine::Report("Texture dimensions must be at least 32.");
			return;
		}

		if (a == 0)
		{
			if (textureHeader->imageFormat != kTextureBC13)
			{
				TextureResource::ReleaseHeaderData(textureHeader);
				resource->Release();

				Engine::Report("Textures must be block compressed.");
				return;
			}

			referenceTextureHeader.colorSemantic = textureHeader->colorSemantic;
			referenceTextureHeader.alphaSemantic = textureHeader->alphaSemantic;
			referenceTextureHeader.imageFormat = textureHeader->imageFormat;
			referenceTextureHeader.imageWidth = textureHeader->imageWidth;
			referenceTextureHeader.imageHeight = textureHeader->imageHeight;
			referenceTextureHeader.mipmapCount = textureHeader->mipmapCount;
		}
		else
		{
			if ((textureHeader->colorSemantic != referenceTextureHeader.colorSemantic) || (textureHeader->alphaSemantic != referenceTextureHeader.alphaSemantic) || (textureHeader->imageFormat != referenceTextureHeader.imageFormat))
			{
				TextureResource::ReleaseHeaderData(textureHeader);
				resource->Release();

				Engine::Report("Texture formats must match.");
				return;
			}

			if ((textureHeader->imageWidth != referenceTextureHeader.imageWidth) || (textureHeader->imageHeight != referenceTextureHeader.imageHeight) || (textureHeader->mipmapCount != referenceTextureHeader.mipmapCount))
			{
				TextureResource::ReleaseHeaderData(textureHeader);
				resource->Release();

				Engine::Report("Texture sizes must match.");
				return;
			}
		}

		TextureResource::ReleaseHeaderData(textureHeader);
		resource->Release();
	}

	resourceHeader.endian = 1;
	resourceHeader.headerDataSize = sizeof(TextureHeader) + referenceTextureHeader.auxiliaryDataSize;
	resourceHeader.textureCount = 1;

	TheResourceMgr->GetGenericCatalog()->GetResourcePath(TextureResource::GetDescriptor(), outputName, &outputPath);

	TheResourceMgr->CreateDirectoryPath(outputPath);
	if (outputFile.Open(outputPath, kFileCreate) != kFileOkay)
	{
		Engine::Report("Error writing output texture.");
		return;
	}

	outputFile.Write(&resourceHeader, sizeof(TextureResourceHeader));
	outputFile.Write(&referenceTextureHeader, sizeof(TextureHeader));

	chunkHeader.chunkType = kTextureChunkReferenceList;
	chunkHeader.chunkSize = 4 + sizeof(ResourceName) * count;

	outputFile.Write(&chunkHeader, sizeof(ChunkHeader));
	outputFile.Write(&count, 4);

	for (machine a = 0; a < count; a++)
	{
		ResourceName	name;

		MemoryMgr::ClearMemory(name, sizeof(ResourceName));
		name = inputNameArray[a];
		outputFile.Write(&name, sizeof(ResourceName));
	}
}

// ZYUQURM
