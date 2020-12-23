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


#include "C4TextureGenerator.h"
#include "C4Cameras.h"
#include "C4Skybox.h"
#include "C4World.h"


using namespace C4;


TextureGeneratorWindow *C4::TheTextureGeneratorWindow = nullptr;


TextureGenerator::TextureGenerator(World *world, const char *name) : TextureImporter(name)
{
	currentWorld = world;
	filterSize = 0;
}

TextureGenerator::~TextureGenerator()
{
}

void TextureGenerator::GetOutputTexturePath(ResourcePath *path) const
{
	const char *name = GetTextureName();
	if (name[0] == '/')
	{
		TheResourceMgr->GetGenericCatalog()->GetResourcePath(TextureResource::GetDescriptor(), &name[1], path);
	}
	else
	{
		TheResourceMgr->GetGenericCatalog()->GetResourcePath(TextureResource::GetDescriptor(), name, currentWorld->GetResourceLocation(), path);
	}
}

void TextureGenerator::FilterCubeTexture(Color4C *image, int32 pixelCount, int32 width)
{
	struct RemapData
	{
		int8	x[4];
		int8	y[4];
	};

	static const int8 faceRemap[6][4] =
	{
		{4, 5, 3, 2},
		{5, 4, 3, 2},
		{1, 0, 4, 5},
		{1, 0, 5, 4},
		{1, 0, 3, 2},
		{0, 1, 3, 2}
	};

	static const RemapData remapData[6][4] =
	{
		{{{ 1,  0,  1,  0}, { 0,  1,  0,  0}},
		 {{ 1,  0, -1,  0}, { 0,  1,  0,  0}},
		 {{ 0, -1,  2, -1}, { 1,  0,  0,  0}},
		 {{ 0,  1,  1,  0}, {-1,  0,  1, -1}}},

		{{{ 1,  0,  1,  0}, { 0,  1,  0,  0}},
		 {{ 1,  0, -1,  0}, { 0,  1,  0,  0}},
		 {{ 0,  1, -1,  0}, {-1,  0,  1, -1}},
		 {{ 0, -1,  0, -1}, { 1,  0,  0,  0}}},

		{{{ 0,  1,  0,  0}, {-1,  0,  0, -1}},
		 {{ 0, -1,  1, -1}, { 1,  0, -1,  0}},
		 {{ 1,  0,  0,  0}, { 0,  1, -1,  0}},
		 {{-1,  0,  1, -1}, { 0, -1,  0, -1}}},

		{{{ 0, -1,  1, -1}, { 1,  0,  1,  0}},
		 {{ 0,  1,  0,  0}, {-1,  0,  2, -1}},
		 {{-1,  0,  1, -1}, { 0, -1,  2, -1}},
		 {{ 1,  0,  0,  0}, { 0,  1,  1,  0}}},

		{{{ 1,  0,  1,  0}, { 0,  1,  0,  0}},
		 {{ 1,  0, -1,  0}, { 0,  1,  0,  0}},
		 {{ 1,  0,  0,  0}, { 0,  1, -1,  0}},
		 {{ 1,  0,  0,  0}, { 0,  1,  1,  0}}},

		{{{ 1,  0,  1,  0}, { 0,  1,  0,  0}},
		 {{ 1,  0, -1,  0}, { 0,  1,  0,  0}},
		 {{-1,  0,  1, -1}, { 0, -1,  2, -1}},
		 {{-1,  0,  1, -1}, { 0, -1,  0, -1}}}
	};

	for (machine a = 0; a < 6; a++)
	{
		Color4C *destin = image + pixelCount * (11 - a);

		for (machine y = 0; y < width; y++)
		{
			Color4C *dst = destin + y * width;

			int32 jmin = y - filterSize;
			int32 jmax = y + filterSize + 1;

			for (machine x = 0; x < width; x++)
			{
				unsigned_int32 red = 0;
				unsigned_int32 green = 0; 
				unsigned_int32 blue = 0;
				unsigned_int32 alpha = 0;
 
				int32 imin = x - filterSize;
				int32 imax = x + filterSize + 1; 

				for (machine j = jmin; j < jmax; j++)
				{ 
					for (machine i = imin; i < imax; i++)
					{ 
						machine face = a; 
						machine ip = i;
						machine jp = j;

						for (machine r = 0; r < 2; r++) 
						{
							if (ip < 0)
							{
								const RemapData *data = &remapData[face][0];
								face = faceRemap[face][0];
								machine t = data->x[0] * ip + data->x[1] * jp + data->x[2] * width + data->x[3];
								jp = data->y[0] * ip + data->y[1] * jp + data->y[2] * width + data->y[3];
								ip = t;
							}

							if (ip >= width)
							{
								const RemapData *data = &remapData[face][1];
								face = faceRemap[face][1];
								machine t = data->x[0] * ip + data->x[1] * jp + data->x[2] * width + data->x[3];
								jp = data->y[0] * ip + data->y[1] * jp + data->y[2] * width + data->y[3];
								ip = t;
							}

							if (jp < 0)
							{
								const RemapData *data = &remapData[face][3];
								face = faceRemap[face][3];
								machine t = data->x[0] * ip + data->x[1] * jp + data->x[2] * width + data->x[3];
								jp = data->y[0] * ip + data->y[1] * jp + data->y[2] * width + data->y[3];
								ip = t;
							}

							if (jp >= width)
							{
								const RemapData *data = &remapData[face][2];
								face = faceRemap[face][2];
								machine t = data->x[0] * ip + data->x[1] * jp + data->x[2] * width + data->x[3];
								jp = data->y[0] * ip + data->y[1] * jp + data->y[2] * width + data->y[3];
								ip = t;
							}
						}

						const Color4C *source = image + pixelCount * (5 - face);
						const Color4C& color = source[jp * width + ip];

						red += color.GetRed();
						green += color.GetGreen();
						blue += color.GetBlue();
						alpha += color.GetAlpha();
					}
				}

				int32 d = (imax - imin) * (jmax - jmin);
				red /= d;
				green /= d;
				blue /= d;
				alpha /= d;

				dst[x].Set(red, green, blue, alpha);
			}
		}
	}
}

EngineResult TextureGenerator::GenerateCubeTexture(Node *node, int32 width, TextureFormat format)
{
	CubeCamera	cubeCamera;

	node->AppendNewSubnode(&cubeCamera);
	currentWorld->SetCamera(&cubeCamera);
	currentWorld->SetRenderSize(width, width);
	cubeCamera.GetObject()->SetNearDepth(0.001F);

	int32 pixelCount = width * width;
	Color4C *image = new Color4C[pixelCount * 12];

	for (machine a = 0; a < 6; a++)
	{
		cubeCamera.SetFaceIndex(a);

		currentWorld->Update();
		currentWorld->BeginRendering();
		currentWorld->Render();
		currentWorld->EndRendering();

		TheGraphicsMgr->ReadImageBuffer(Rect(0, 0, width, width), image + pixelCount * (5 - a), width);
	}

	if (filterSize == 0)
	{
		SetTextureImage(0, width, width * 6, image);
	}
	else
	{
		FilterCubeTexture(image, pixelCount, width);
		SetTextureImage(0, width, width * 6, image + pixelCount * 6);
	}

	delete[] image;

	TextureHeader *header = GetTextureHeader();
	header->textureType = kTextureCube;
	header->imageFormat = format;
	header->wrapMode[0] = kTextureClamp;
	header->wrapMode[1] = kTextureClamp;
	header->wrapMode[2] = kTextureClamp;

	return (ImportTextureImage());
}

EngineResult TextureGenerator::GenerateSpotTexture(Node *node, float apex, float aspect, int32 width, TextureFormat format)
{
	FrustumCamera frustumCamera(apex, aspect);
	node->AppendNewSubnode(&frustumCamera);
	currentWorld->SetCamera(&frustumCamera);
	currentWorld->SetRenderSize(width, width);

	currentWorld->Update();
	currentWorld->BeginRendering();
	currentWorld->Render();
	currentWorld->EndRendering();

	int32 pixelCount = width * width;
	Color4C *image = new Color4C[pixelCount * 2];
	TheGraphicsMgr->ReadImageBuffer(Rect(0, 0, width, width), image, width);

	if (filterSize == 0)
	{
		SetTextureImage(0, width, width, image);
	}
	else
	{
		const Color4C *source = image;
		Color4C *destin = image + pixelCount;

		for (machine y = 0; y < width; y++)
		{
			Color4C *dst = destin + y * width;

			int32 jmin = MaxZero(y - filterSize);
			int32 jmax = Min(y + filterSize + 1, width);

			for (machine x = 0; x < width; x++)
			{
				unsigned_int32 red = 0;
				unsigned_int32 green = 0;
				unsigned_int32 blue = 0;
				unsigned_int32 alpha = 0;

				int32 imin = MaxZero(x - filterSize);
				int32 imax = Min(x + filterSize + 1, width);

				for (machine j = jmin; j < jmax; j++)
				{
					const Color4C *src = source + j * width;

					for (machine i = imin; i < imax; i++)
					{
						const Color4C& color = src[i];
						red += color.GetRed();
						green += color.GetGreen();
						blue += color.GetBlue();
						alpha += color.GetAlpha();
					}
				}

				int32 d = (imax - imin) * (jmax - jmin);
				red /= d;
				green /= d;
				blue /= d;
				alpha /= d;

				dst[x].Set(red, green, blue, alpha);
			}
		}

		SetTextureImage(0, width, width, destin);
	}

	delete[] image;

	TextureHeader *header = GetTextureHeader();
	header->imageFormat = format;
	header->wrapMode[0] = kTextureClampBorder;
	header->wrapMode[1] = kTextureClampBorder;
	header->wrapMode[2] = kTextureClampBorder;

	return (ImportTextureImage());
}

EngineResult TextureGenerator::GenerateImpostorTexture(Node *node, const ImpostorProperty *property)
{
	float	bestRadius1, bestRadius2;
	float	bestHeight1, bestHeight2;
	float	clipDistance[2][63];

	float impostorRadius = 0.0F;
	float impostorHeight = 0.0F;
	for (machine b = 0; b < 63; b++)
	{
		clipDistance[0][b] = 0.0F;
		clipDistance[1][b] = 0.0F;
	}

	const ConstVector2D *trig = Math::GetTrigTable();

	Node *subnode = node->GetFirstSubnode();
	while (subnode)
	{
		if (subnode->GetNodeType() == kNodeGeometry)
		{
			Geometry *geometry = static_cast<Geometry *>(subnode);
			const Transform4D& transform = geometry->GetWorldTransform();

			geometry->SetAmbientBlendState(BlendState(kBlendOne, kBlendZero, kBlendOne, kBlendZero));
			geometry->InvalidateShaderData();

			const GeometryObject *object = geometry->GetObject();
			const Mesh *level = object->GetGeometryLevel(0);

			int32 vertexCount = level->GetVertexCount();
			const Point3D *vertex = level->GetArray<Point3D>(kArrayPosition);

			for (machine a = 0; a < vertexCount; a++)
			{
				Point3D p = transform * vertex[a];
				float r = Magnitude(p.GetVector2D());

				impostorRadius = Fmax(impostorRadius, r);
				impostorHeight = Fmax(impostorHeight, p.z);

				for (machine i = 0; i < 63; i++)
				{
					const Vector2D& cs = trig[i + 1];
					float x = r * cs.x;
					float y = p.z * cs.y;

					float& d1 = clipDistance[0][i];
					float& d2 = clipDistance[1][i];
					d1 = Fmax(d1, x - y);
					d2 = Fmax(d2, x + y);
				}
			}
		}

		subnode = node->GetNextNode(subnode);
	}

	float bestArea1 = 0.0F;
	float bestArea2 = 0.0F;

	for (machine i = 0; i < 63; i++)
	{
		const Vector2D& cs = trig[i + 1];
		float x = 1.0F / cs.x;
		float y = 1.0F / cs.y;

		float d1 = clipDistance[0][i];
		float d2 = clipDistance[1][i];

		float r1 = d1 * x;
		float h1 = (cs.x * impostorRadius - d1) * y;
		float r2 = (d2 - cs.y * impostorHeight) * x;
		float h2 = (d2 - cs.x * impostorRadius) * y;

		float area1 = (impostorRadius - r1) * h1;
		if (area1 > bestArea1)
		{
			bestArea1 = area1;
			bestRadius1 = r1;
			bestHeight1 = h1;
		}

		float area2 = (impostorRadius - r2) * (impostorHeight - h2);
		if (area2 > bestArea2)
		{
			bestArea2 = area2;
			bestRadius2 = r2;
			bestHeight2 = h2;
		}
	}

	unsigned_int32 impostorFlags = property->GetImpostorFlags();

	if (impostorFlags & ImpostorProperty::kImpostorKeepBottom)
	{
		bestArea1 = 0.0F;
	}

	if (impostorFlags & ImpostorProperty::kImpostorKeepTop)
	{
		bestArea2 = 0.0F;
	}

	if (bestArea1 > 0.0F)
	{
		float r1 = bestRadius1 / impostorRadius;
		float h1 = bestHeight1 / impostorHeight;

		if (bestArea2 > 0.0F)
		{
			float r2 = bestRadius2 / impostorRadius;
			float h2 = bestHeight2 / impostorHeight;
			SetImpostorClipData(r1, r2, h1, Fmax(h1, h2));
		}
		else
		{
			SetImpostorClipData(r1, 1.0F, h1, h1);
		}
	}
	else if (bestArea2 > 0.0F)
	{
		float r2 = bestRadius2 / impostorRadius;
		float h2 = bestHeight2 / impostorHeight;
		SetImpostorClipData(1.0F, r2, h2, h2);
	}

	impostorHeight *= 0.5F;
	SetImpostorSize(impostorRadius, impostorHeight);

	int32 width = property->GetTextureWidth();
	int32 height = property->GetTextureHeight();

	const float d = 1024.0F;

	FrustumCamera frustumCamera(d / impostorRadius, 1.0F);
	currentWorld->SetCamera(&frustumCamera);
	currentWorld->SetRenderSize(width, height);

	FrustumCameraObject *cameraObject = frustumCamera.GetObject();
	cameraObject->SetClearFlags(kClearColorBuffer | kClearDepthStencilBuffer);
	cameraObject->SetFarDepth(2048.0F);

	int32 pixelCount = width * height * 8;
	Color4C *image = new Color4C[pixelCount];

	Type usage = property->GetTextureUsage();
	if (usage != ImpostorProperty::kImpostorShadowMap)
	{
		const Property *clearProperty = node->GetProperty(kPropertyClear);
		if (clearProperty)
		{
			cameraObject->SetClearColor(static_cast<const ClearProperty *>(clearProperty)->GetClearColor());
		}
		else
		{
			cameraObject->SetClearColor(ColorRGBA(0.0F, 0.0F, 0.0F, 0.0F));
		}

		GraphicsMgr::SetImpostorDepthParams(0.5F / impostorRadius, (impostorRadius - d) * (0.5F / impostorRadius), 0.0F);

		for (machine a = 0; a < 8; a++)
		{
			frustumCamera.SetNodePosition(Point3D(trig[a * 32] * d, impostorHeight));
			frustumCamera.LookAtPoint(Point3D(0.0F, 0.0F, impostorHeight));

			currentWorld->Update();
			cameraObject->SetAspectRatio(impostorHeight / impostorRadius);

			currentWorld->BeginRendering();
			currentWorld->Render();
			currentWorld->EndRendering();

			TheGraphicsMgr->ReadImageBuffer(Rect(0, 0, width, height), image + width * a, width * 8);
		}
	}
	else
	{
		cameraObject->SetClearColor(ColorRGBA(1.0F, 1.0F, 1.0F, 1.0F));

		Color4C *depthImage = new Color4C[pixelCount];

		float elevation = K::tau_over_24;
		for (machine k = 0; k < 4; k++)
		{
			Vector2D v = CosSin(elevation);
			float b = (impostorHeight * v.y - d * 0.5F + Sqrt(d * d * 0.25F + impostorHeight * impostorHeight * v.y * v.y)) / v.y;
			float aspect = (b * v.x * d) / ((d + b * v.y) * impostorRadius);

			GraphicsMgr::SetImpostorDepthParams(0.25F * v.x / impostorRadius, 0.25F - 0.25F * d * v.x / impostorRadius, v.y / v.x);

			for (machine a = 0; a < 8; a++)
			{
				frustumCamera.SetNodePosition(Point3D(trig[a * 32] * (d * v.x), b + d * v.y));
				frustumCamera.LookAtPoint(Point3D(0.0F, 0.0F, b));

				currentWorld->Update();
				cameraObject->SetAspectRatio(aspect);

				currentWorld->BeginRendering();
				currentWorld->Render();
				currentWorld->EndRendering();

				TheGraphicsMgr->ReadImageBuffer(Rect(0, 0, width, height), depthImage + width * a, width * 8);
			}

			const Color4C *src = depthImage;
			unsigned_int8 *dst = reinterpret_cast<unsigned_int8 *>(image) + k;
			for (machine a = 0; a < pixelCount; a++)
			{
				*dst = (unsigned_int8) src[a].GetAlpha();
				dst += 4;
			}

			elevation += K::tau_over_24;
		}

		delete[] depthImage;
	}

	SetTextureImage(0, width * 8, height, image);
	delete[] image;

	unsigned_int32 importFlags = GetTextureImportFlags();
	if (usage == ImpostorProperty::kImpostorColorMap)
	{
		importFlags |= kTextureImportCompressionBC13 | kTextureImportBleedAlphaTest;
	}
	else if (usage == ImpostorProperty::kImpostorNormalMap)
	{
		importFlags |= kTextureImportVectorData | kTextureImportBleedAlphaTest;
	}

	SetTextureImportFlags(importFlags);

	TextureHeader *header = GetTextureHeader();
	if (property->GetImpostorFlags() & ImpostorProperty::kImpostorAlphaChannel)
	{
		header->alphaSemantic = kTextureSemanticTransparency;
	}

	header->wrapMode[1] = kTextureClamp;

	return (ImportTextureImage());
}


void TextureTool::GenerateTextures(World *world, unsigned_int32 flags)
{
	FrustumCamera *camera = world->GetCamera();
	if (camera)
	{
		unsigned_int32 renderOptionFlags = TheGraphicsMgr->GetRenderOptionFlags();
		TheGraphicsMgr->SetRenderOptionFlags(renderOptionFlags & ~(kRenderOptionMotionBlur | kRenderOptionDistortion | kRenderOptionGlowBloom));

		unsigned_int32 worldFlags = world->GetWorldFlags();
		int32 worldRenderWidth = world->GetRenderWidth();
		int32 worldRenderHeight = world->GetRenderHeight();

		world->SetWorldFlags(worldFlags | (kWorldMotionBlurInhibit | kWorldListenerInhibit));

		Node *root = world->GetRootNode();

		if (flags & kGenerateImpostorImage)
		{
			const Property *property = root->GetFirstProperty();
			while (property)
			{
				PropertyType propertyType = property->GetPropertyType();
				if (propertyType == kPropertyImpostor)
				{
					const ImpostorProperty *impostorProperty = static_cast<const ImpostorProperty *>(property);
					const char *name = impostorProperty->GetTextureName();
					if (name[0] != 0)
					{
						TextureGenerator generator(world, name);
						generator.GenerateImpostorTexture(root, impostorProperty);
						break;
					}
				}

				property = property->Next();
			}
		}

		Node *node = root->GetFirstSubnode();
		while (node)
		{
			NodeType nodeType = node->GetNodeType();
			if (nodeType == kNodeLight)
			{
				Light *light = static_cast<Light *>(node);
				if (light->GetObject()->GetLightFlags() & kLightGenerator)
				{
					LightType lightType = light->GetLightType();

					if (flags & kGenerateLightProjector)
					{
						if (lightType == kLightCube)
						{
							CubeLight *cubeLight = static_cast<CubeLight *>(light);
							const CubeLightObject *object = cubeLight->GetObject();

							const ResourceName& name = object->GetProjectionName();
							if (name[0] != 0)
							{
								TextureGenerator generator(world, name);
								generator.SetFilterSize(1);
								generator.GenerateCubeTexture(node, object->GetTextureSize(), object->GetTextureFormat());
							}
						}
						else if (lightType == kLightSpot)
						{
							SpotLight *spotLight = static_cast<SpotLight *>(light);
							const SpotLightObject *object = spotLight->GetObject();

							const ResourceName& name = object->GetProjectionName();
							if (name[0] != 0)
							{
								TextureGenerator generator(world, name);
								generator.SetFilterSize(1);
								generator.GenerateSpotTexture(spotLight, object->GetApexTangent(), object->GetAspectRatio(), object->GetTextureSize(), object->GetTextureFormat());
							}
						}
					}
				}
			}
			else if (nodeType == kNodeMarker)
			{
				Marker *marker = static_cast<Marker *>(node);
				MarkerType markerType = marker->GetMarkerType();

				if (markerType == kMarkerCube)
				{
					if (flags & kGenerateEnvironmentMap)
					{
						CubeMarker *cubeMarker = static_cast<CubeMarker *>(marker);
						TextureGenerator generator(world, cubeMarker->GetTextureName());
						generator.SetFilterSize((cubeMarker->GetCubeFlags() & kCubeFilter) ? 1 : 0);
						generator.GenerateCubeTexture(cubeMarker, cubeMarker->GetCubeSize(), cubeMarker->GetTextureFormat());
					}
				}
			}

			node = root->GetNextNode(node);
		}

		world->SetCamera(camera);
		world->SetWorldFlags(worldFlags);
		world->SetRenderSize(worldRenderWidth, worldRenderHeight);

		TheGraphicsMgr->SetRenderOptionFlags(renderOptionFlags);
	}
}


TextureGeneratorWindow::TextureGeneratorWindow() :
		Window("TextureTool/TextureGenerator"),
		Singleton<TextureGeneratorWindow>(TheTextureGeneratorWindow)
{
}

TextureGeneratorWindow::~TextureGeneratorWindow()
{
}

void TextureGeneratorWindow::Open(void)
{
	if (TheTextureGeneratorWindow)
	{
		TheInterfaceMgr->SetActiveWindow(TheTextureGeneratorWindow);
	}
	else
	{
		TheInterfaceMgr->AddWidget(new TextureGeneratorWindow);
	}
}

void TextureGeneratorWindow::Preprocess(void)
{
	Window::Preprocess();

	generateButton = static_cast<PushButtonWidget *>(FindWidget("Generate"));
	cancelButton = static_cast<PushButtonWidget *>(FindWidget("Cancel"));

	lightBox = static_cast<CheckWidget *>(FindWidget("Light"));
	environmentBox = static_cast<CheckWidget *>(FindWidget("Environ"));
	impostorBox = static_cast<CheckWidget *>(FindWidget("Impostor"));
}

void TextureGeneratorWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		if (widget == generateButton)
		{
			World *world = TheWorldMgr->GetWorld();
			if (world)
			{
				unsigned_int32 flags = 0;

				if (lightBox->GetValue() != 0)
				{
					flags |= kGenerateLightProjector;
				}

				if (environmentBox->GetValue() != 0)
				{
					flags |= kGenerateEnvironmentMap;
				}

				if (impostorBox->GetValue() != 0)
				{
					flags |= kGenerateImpostorImage;
				}

				if (flags != 0)
				{
					TextureTool::GenerateTextures(world, flags);
				}
			}

			Close();
		}
		else if (widget == cancelButton)
		{
			Close();
		}
	}
}

// ZYUQURM
