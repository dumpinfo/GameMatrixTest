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


#include "C4TextureViewer.h"
#include "C4TextureTool.h"
#include "C4Graphics.h"


using namespace C4;


List<TextureWindow> TextureWindow::windowList;


TextureWindow::TextureWindow(const char *name, TextureResource *resource, const TextureHeader *header) :
		Window(CalculateViewportSize(header) + Vector2D(196.0F, 8.0F), nullptr, kWindowCloseBox | kWindowBackground | kWindowCenter | kWindowStrip)
{
	resourceName = name;
	windowList.Append(this);

	textureResource = resource;
	textureHeader = header;

	ResourcePath title(name);
	SetWindowTitle(title += TextureResource::GetDescriptor()->GetExtension());
	SetStripTitle(&title[Text::GetDirectoryPathLength(title)]);
	SetStripIcon("TextureTool/window");

	mipmapLevel = 0;
}

TextureWindow::~TextureWindow()
{
	textureResource->Release();
}

ResourceResult TextureWindow::Open(const char *name)
{
	TextureWindow *window = windowList.First();
	while (window)
	{
		if (window->resourceName == name)
		{
			TheInterfaceMgr->SetActiveWindow(window);
			return (kResourceOkay);
		}

		window = window->ListElement<TextureWindow>::Next();
	}

	TextureResource *resource = TextureResource::Get(name);
	if (!resource)
	{
		return (kResourceNotFound);
	}

	TheInterfaceMgr->AddWidget(new TextureWindow(name, resource, resource->GetTextureHeader()));
	return (kResourceOkay);
}

const Vector2D& TextureWindow::CalculateViewportSize(const TextureHeader *header)
{
	float width = (float) header->imageWidth;
	float height = (float) header->imageHeight;

	if (header->textureType == kTextureCube)
	{
		width *= 4.0F;
		height *= 3.0F;
	}

	const Vector2D& desktopSize = TheInterfaceMgr->GetDesktopSize();
	float w = Fmin(width, desktopSize.x - 196.0F);
	float h = Fmin(height, desktopSize.y - 64.0F);

	float scale = Fmin(w / width, h / height);
	imageSize.Set(Floor(width * scale), Floor(height * scale));

	viewportSize.Set(Fmax(imageSize.x, 128.0F), Fmax(imageSize.y, 200.0F));
	return (viewportSize);
}

void TextureWindow::Preprocess(void)
{
	Window::Preprocess();

	BorderWidget *border = new BorderWidget(viewportSize);
	border->SetWidgetPosition(Point3D(4.0F, 4.0F, 0.0F));
	AppendNewSubnode(border);

	colorBackground = new QuadWidget(viewportSize, ColorRGBA(0.0F, 0.0F, 0.0F, 1.0F));
	colorBackground->SetWidgetPosition(Point3D(4.0F, 4.0F, 0.0F));
	AppendNewSubnode(colorBackground);

	checkerBackground = new ImageWidget(viewportSize, "C4/checker");
	checkerBackground->SetWidgetPosition(Point3D(4.0F, 4.0F, 0.0F));
	checkerBackground->SetImageScale(viewportSize * 0.0625F);
	checkerBackground->Hide();
	AppendNewSubnode(checkerBackground);

	TextureType type = textureHeader->textureType;
	if (type != kTextureCube)
	{
		textureImage[0] = new ImageWidget(imageSize, resourceName); 
		textureImage[0]->SetImageBlendState(kBlendReplace);
		AppendNewSubnode(textureImage[0]);
 
		if (type == kTexture3D)
		{ 
			textureImage[0]->SetImagePCoordinate(0.5F / (float) textureHeader->imageDepth);
		}
	} 
	else
	{ 
		textureImage[0] = new ImageWidget(imageSize, resourceName); 
		textureImage[0]->SetImageBlendState(kBlendReplace);
		AppendNewSubnode(textureImage[0]);

		textureImage[1] = new ImageWidget(imageSize, resourceName); 
		textureImage[1]->SetImageBlendState(kBlendReplace);
		textureImage[1]->SetCubeFaceIndex(1);
		AppendNewSubnode(textureImage[1]);

		textureImage[2] = new ImageWidget(imageSize, resourceName);
		textureImage[2]->SetImageBlendState(kBlendReplace);
		textureImage[2]->SetCubeFaceIndex(2);
		AppendNewSubnode(textureImage[2]);

		textureImage[3] = new ImageWidget(imageSize, resourceName);
		textureImage[3]->SetImageBlendState(kBlendReplace);
		textureImage[3]->SetCubeFaceIndex(3);
		AppendNewSubnode(textureImage[3]);

		textureImage[4] = new ImageWidget(imageSize, resourceName);
		textureImage[4]->SetImageBlendState(kBlendReplace);
		textureImage[4]->SetCubeFaceIndex(4);
		AppendNewSubnode(textureImage[4]);

		textureImage[5] = new ImageWidget(imageSize, resourceName);
		textureImage[5]->SetImageBlendState(kBlendReplace);
		textureImage[5]->SetCubeFaceIndex(5);
		AppendNewSubnode(textureImage[5]);
	}

	SetImagePosition();

	Widget *group = new Widget;
	group->SetWidgetPosition(Point3D(GetWidgetSize().x - 180.0F, 0.0F, 0.0F));
	group->Load("TextureTool/TextureViewer");
	AppendNewSubnode(group);

	const StringTable *table = TheTextureTool->GetStringTable();

	static_cast<TextWidget *>(FindWidget("Type"))->SetText(table->GetString(StringID('TYPE', type)));
	static_cast<TextWidget *>(FindWidget("Format"))->SetText(table->GetString(StringID('FORM', textureHeader->imageFormat)));

	TextureSemantic alpha = textureHeader->alphaSemantic;
	static_cast<TextWidget *>(FindWidget("Alpha"))->SetText(table->GetString(StringID('ALFA', (alpha == kTextureSemanticNone) ? 'NONE' : alpha)));

	String<63> string(textureHeader->imageWidth);
	string += " \xC3\x97 ";		// U+00D7
	string += textureHeader->imageHeight;

	if (type == kTexture3D)
	{
		string += " \xC3\x97 ";
		string += textureHeader->imageDepth;
	}

	static_cast<TextWidget *>(FindWidget("Size"))->SetText(string);

	if (textureHeader->alphaSemantic == kTextureSemanticParallax)
	{
		const ChunkHeader *chunk = textureHeader->GetAuxiliaryData();
		while (chunk)
		{
			if (chunk->chunkType == kTextureChunkParallaxScale)
			{
				static_cast<TextWidget *>(FindWidget("Parallax"))->SetText(Text::FloatToString(*reinterpret_cast<const float *>(chunk + 1) * 2.0F));
				break;
			}

			chunk = chunk->GetNextChunk();
		}
	}
	else
	{
		static_cast<TextWidget *>(FindWidget("PlaxText"))->Hide();
		static_cast<TextWidget *>(FindWidget("Parallax"))->Hide();
	}

	flipBox = static_cast<CheckWidget *>(FindWidget("Flip"));
	if (type == kTextureCube)
	{
		flipBox->Disable();
	}

	blendBox = static_cast<CheckWidget *>(FindWidget("Blend"));
	backgroundMenu = static_cast<PopupMenuWidget *>(FindWidget("Background"));
}

void TextureWindow::SetImagePosition(void)
{
	if (textureHeader->textureType != kTextureCube)
	{
		float w = Fmin((float) Max(textureHeader->imageWidth >> mipmapLevel, 1), imageSize.x);
		float h = Fmin((float) Max(textureHeader->imageHeight >> mipmapLevel, 1), imageSize.y);

		textureImage[0]->SetWidgetSize(Vector2D(w, h));
		textureImage[0]->SetWidgetPosition(Point3D(Floor((viewportSize.x - w) * 0.5F) + 4.0F, Floor((viewportSize.y - h) * 0.5F) + 4.0F, 0.0F));
		textureImage[0]->Invalidate();
	}
	else
	{
		float s = Fmin((float) Max(textureHeader->imageWidth >> mipmapLevel, 1), imageSize.x * 0.25F);
		float cx = Floor(viewportSize.x * 0.375F - s * 0.5F) + 4.0F;
		float cy = Floor(viewportSize.y * 0.5F - s * 0.5F) + 4.0F;
		Vector2D size(s, s);

		textureImage[0]->SetWidgetSize(size);
		textureImage[0]->SetWidgetPosition(Point3D(cx, cy, 0.0F));
		textureImage[0]->Invalidate();

		textureImage[1]->SetWidgetSize(size);
		textureImage[1]->SetWidgetPosition(Point3D(cx - s, cy, 0.0F));
		textureImage[1]->Invalidate();

		textureImage[2]->SetWidgetSize(size);
		textureImage[2]->SetWidgetPosition(Point3D(cx + s, cy, 0.0F));
		textureImage[2]->Invalidate();

		textureImage[3]->SetWidgetSize(size);
		textureImage[3]->SetWidgetPosition(Point3D(cx + s * 2.0F, cy, 0.0F));
		textureImage[3]->Invalidate();

		textureImage[4]->SetWidgetSize(size);
		textureImage[4]->SetWidgetPosition(Point3D(cx, cy - s, 0.0F));
		textureImage[4]->Invalidate();

		textureImage[5]->SetWidgetSize(size);
		textureImage[5]->SetWidgetPosition(Point3D(cx, cy + s, 0.0F));
		textureImage[5]->Invalidate();
	}
}

bool TextureWindow::HandleKeyboardEvent(const KeyboardEventData *eventData)
{
	if (eventData->eventType == kEventKeyCommand)
	{
		unsigned_int32 keyCode = eventData->keyCode;

		if (keyCode == 'M')
		{
			int32 level = mipmapLevel + 1;
			if (level >= textureHeader->mipmapCount)
			{
				level = 0;
			}

			mipmapLevel = level;

			SetImagePosition();
			return (true);
		}
		else if (keyCode == 'D')
		{
			const Texture *texture = textureImage[0]->GetTexture();

			TextureType type = texture->GetTextureType();
			if (type == kTextureArray2D)
			{
				float p = textureImage[0]->GetImagePCoordinate() + 1.0F;
				if (p >= (float) texture->GetTextureDepth())
				{
					p = 0.0F;
				}

				textureImage[0]->SetImagePCoordinate(p);
			}
			else if (type == kTexture3D)
			{
				float d = 1.0F / (float) texture->GetTextureDepth();
				float p = textureImage[0]->GetImagePCoordinate() + d;
				if (p > 1.0F - d * 0.25F)
				{
					p = d * 0.5F;
				}

				textureImage[0]->SetImagePCoordinate(p);
			}
		}
		else if (keyCode == 'F')
		{
			if (flipBox)
			{
				flipBox->SetValue(flipBox->GetValue() ^ 1);
				flipBox->PostWidgetEvent(WidgetEventData(kEventWidgetChange));
			}

			return (true);
		}
		else if (keyCode == 'W')
		{
			Close();
			return (true);
		}
	}

	return (Window::HandleKeyboardEvent(eventData));
}

void TextureWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		if (widget == flipBox)
		{
			textureImage[0]->SetImageOffset(Vector2D(0.0F, 1.0F - textureImage[0]->GetImageOffset().y));
			textureImage[0]->SetImageScale(Vector2D(1.0F, -textureImage[0]->GetImageScale().y));
		}
		else if (widget == blendBox)
		{
			int32 k = (textureHeader->textureType == kTextureCube) ? 6 : 1;
			if (blendBox->GetValue() == 0)
			{
				for (machine a = 0; a < k; a++)
				{
					textureImage[a]->SetAmbientBlendState(kBlendReplace);
					textureImage[a]->InvalidateShaderData();
				}
			}
			else
			{
				for (machine a = 0; a < k; a++)
				{
					textureImage[a]->SetAmbientBlendState(kBlendInterpolate);
					textureImage[a]->InvalidateShaderData();
				}
			}
		}
		else if (widget == backgroundMenu)
		{
			int32 selection = backgroundMenu->GetSelection();
			if (selection < 3)
			{
				checkerBackground->Hide();
				colorBackground->Show();

				if (selection == 0)
				{
					colorBackground->SetWidgetColor(ColorRGBA(0.0F, 0.0F, 0.0F, 1.0F));
				}
				else if (selection == 1)
				{
					colorBackground->SetWidgetColor(K::white);
				}
				else
				{
					colorBackground->SetWidgetColor(ColorRGBA(0.5F, 0.5F, 0.5F, 1.0F));
				}
			}
			else
			{
				checkerBackground->Show();
				colorBackground->Hide();
			}
		}
	}
}

// ZYUQURM
