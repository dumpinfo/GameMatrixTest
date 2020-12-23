 

#include "C4Viewports.h"
#include "C4World.h"


using namespace C4;


ViewportWidget::ViewportWidget(WidgetType type, Camera *camera) :
		RenderableWidget(type, kRenderTriangleStrip),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
{
	viewportIndex = 0;
	viewportCamera = camera;

	Initialize();
}

ViewportWidget::ViewportWidget(WidgetType type, Camera *camera, const Vector2D& size) :
		RenderableWidget(type, kRenderTriangleStrip, size),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
{
	viewportIndex = 0;
	viewportCamera = camera;

	Initialize();
}

ViewportWidget::ViewportWidget(const ViewportWidget& viewportWidget, Camera *camera) :
		RenderableWidget(viewportWidget),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
{
	viewportIndex = viewportWidget.viewportIndex;
	viewportCamera = camera;

	Initialize();
}

ViewportWidget::~ViewportWidget()
{
}

void ViewportWidget::Initialize(void)
{
	mouseEventProc = nullptr;
	trackTaskProc = nullptr;
	renderProc = nullptr;
	overlayProc = nullptr;

	textureValidFlag = false;

	SetWidgetUsage(kWidgetMouseWheel | kWidgetMultiaxisMouse | kWidgetTrackRightMouse | kWidgetTrackMiddleMouse);
}

void ViewportWidget::Pack(Packer& data, unsigned_int32 packFlags) const
{
	RenderableWidget::Pack(data, packFlags);

	data << TerminatorChunk;
}

void ViewportWidget::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	RenderableWidget::Unpack(data, unpackFlags);
	UnpackChunkList<ViewportWidget>(data, unpackFlags);
}

bool ViewportWidget::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	return (false);
}

void ViewportWidget::SetWidgetSize(const Vector2D& size)
{
	RenderableWidget::SetWidgetSize(size);

	if (!GetManipulator())
	{
		DeallocateTexture();
	}
}

void ViewportWidget::Preprocess(void)
{
	RenderableWidget::Preprocess();

	SetAmbientBlendState(kBlendReplace);

	SetVertexCount(4);
	SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(ViewportVertex));
	SetVertexAttributeArray(kArrayPosition, 0, 2);
	SetVertexAttributeArray(kArrayTexcoord, sizeof(Point2D), 2);
	vertexBuffer.Establish(sizeof(ViewportVertex) * 4);

	attributeList.Append(&textureAttribute);
	SetMaterialAttributeList(&attributeList);

	if (GetManipulator())
	{
		textureAttribute.SetTexture("C4/checker");
		textureValidFlag = true;
	} 
}

void ViewportWidget::Build(void) 
{
	volatile ViewportVertex *restrict vertex = vertexBuffer.BeginUpdate<ViewportVertex>(); 

	float w = GetWidgetSize().x;
	float h = GetWidgetSize().y; 

	vertex[0].position.Set(0.0F, 0.0F); 
	vertex[1].position.Set(0.0F, h); 
	vertex[2].position.Set(w, 0.0F);
	vertex[3].position.Set(w, h);

	if (GetManipulator()) 
	{
		w *= 0.03125F;
		h *= 0.03125F;
	}

	vertex[0].texcoord.Set(0.0F, h);
	vertex[1].texcoord.Set(0.0F, 0.0F);
	vertex[2].texcoord.Set(w, h);
	vertex[3].texcoord.Set(w, 0.0F);

	vertexBuffer.EndUpdate();
}

void ViewportWidget::AllocateTexture(void)
{
	textureAttribute.SetTexture(nullptr, nullptr);

	textureHeader.textureType = kTextureRectangle;
	textureHeader.textureFlags = kTextureRenderTarget;
	textureHeader.colorSemantic = kTextureSemanticNone;
	textureHeader.alphaSemantic = kTextureSemanticNone;
	textureHeader.imageFormat = kTextureRGBA8;
	textureHeader.imageWidth = (int32) GetWidgetSize().x;
	textureHeader.imageHeight = (int32) GetWidgetSize().y;
	textureHeader.imageDepth = 1;
	textureHeader.wrapMode[0] = kTextureClamp;
	textureHeader.wrapMode[1] = kTextureClamp;
	textureHeader.wrapMode[2] = kTextureClamp;
	textureHeader.mipmapCount = 1;
	textureHeader.mipmapDataOffset = 0;
	textureHeader.auxiliaryDataSize = 0;
	textureHeader.auxiliaryDataOffset = 0;

	textureAttribute.SetTexture(&textureHeader);
	textureValidFlag = false;

	InvalidateShaderData();
}

void ViewportWidget::DeallocateTexture(void)
{
	textureAttribute.SetTexture(nullptr, nullptr);
	textureValidFlag = false;

	InvalidateShaderData();
}

void ViewportWidget::Render(List<Renderable> *renderList)
{
	const CameraObject		*graphicsCameraObject;
	const Transformable		*graphicsCameraTransformable;

	bool cameraFlag = false;

	if (renderProc)
	{
		if (!textureValidFlag)
		{
			cameraFlag = true;

			TheGraphicsMgr->Draw(renderList);
			renderList->RemoveAll();

			graphicsCameraObject = TheGraphicsMgr->GetCameraObject();
			graphicsCameraTransformable = TheGraphicsMgr->GetCameraTransformable();
			SetGraphicsCamera(true);

			(*renderProc)(renderList, this, renderCookie);

			TheGraphicsMgr->Draw(renderList);
			renderList->RemoveAll();

			Texture *texture = textureAttribute.GetTexture();
			if (texture)
			{
				const Point3D& p = GetWorldPosition();
				float w = GetWidgetSize().x;
				float h = GetWidgetSize().y;

				TheGraphicsMgr->CopyRenderTarget(texture, Rect((int32) p.x, (int32) p.y, (int32) (p.x + w), (int32) (p.y + h)));
				textureValidFlag = true;
			}
		}
		else
		{
			RenderableWidget::Render(renderList);
		}
	}
	else
	{
		RenderableWidget::Render(renderList);
	}

	if (overlayProc)
	{
		if (!cameraFlag)
		{
			cameraFlag = true;

			TheGraphicsMgr->Draw(renderList);
			renderList->RemoveAll();

			graphicsCameraObject = TheGraphicsMgr->GetCameraObject();
			graphicsCameraTransformable = TheGraphicsMgr->GetCameraTransformable();
			SetGraphicsCamera(false);
		}

		(*overlayProc)(renderList, this, overlayCookie);

		TheGraphicsMgr->Draw(renderList);
		renderList->RemoveAll();
	}

	if (cameraFlag)
	{
		TheGraphicsMgr->SetCamera(graphicsCameraObject, graphicsCameraTransformable);
	}
}

void ViewportWidget::HandleMouseEvent(const PanelMouseEventData *eventData)
{
	if (mouseEventProc)
	{
		(*mouseEventProc)(eventData, this, mouseEventCookie);
	}
}

void ViewportWidget::TrackTask(WidgetPart widgetPart, const Point3D& mousePosition)
{
	if (trackTaskProc)
	{
		(*trackTaskProc)(mousePosition, this, trackTaskCookie);
	}
}


OrthoViewportWidget::OrthoViewportWidget(WidgetType type) : ViewportWidget(type, &orthoCamera)
{
	orthoScale.Set(1.0F, 1.0F);
}

OrthoViewportWidget::OrthoViewportWidget(const Vector2D& size, const Vector2D& scale) : OrthoViewportWidget(kWidgetOrthoViewport, size, scale)
{
}

OrthoViewportWidget::OrthoViewportWidget(WidgetType type, const Vector2D& size, const Vector2D& scale) : ViewportWidget(type, &orthoCamera, size)
{
	orthoScale = scale;
}

OrthoViewportWidget::OrthoViewportWidget(const OrthoViewportWidget& orthoViewportWidget) : ViewportWidget(orthoViewportWidget, &orthoCamera)
{
	orthoScale = orthoViewportWidget.orthoScale;
}

OrthoViewportWidget::~OrthoViewportWidget()
{
}

Widget *OrthoViewportWidget::Replicate(void) const
{
	return (new OrthoViewportWidget(*this));
}

void OrthoViewportWidget::Pack(Packer& data, unsigned_int32 packFlags) const
{
	ViewportWidget::Pack(data, packFlags);

	data << TerminatorChunk;
}

void OrthoViewportWidget::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	ViewportWidget::Unpack(data, unpackFlags);
	UnpackChunkList<OrthoViewportWidget>(data, unpackFlags);
}

bool OrthoViewportWidget::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	return (false);
}

void OrthoViewportWidget::SetGraphicsCamera(bool clear)
{
	const Point3D& p = GetWorldPosition();
	float w = GetWidgetSize().x;
	float h = GetWidgetSize().y;

	OrthoCameraObject *object = orthoCamera.GetObject();
	object->SetViewRect(Rect((int32) p.x, (int32) p.y, (int32) (p.x + w), (int32) (p.y + h)));

	float x = PositiveFloor(w * 0.5F) * orthoScale.x;
	float y = PositiveFloor(h * 0.5F) * orthoScale.y;
	object->SetOrthoRect(-x, w * orthoScale.x - x, -y, h * orthoScale.y - y);

	object->SetClearFlags((clear) ? kClearColorBuffer | kClearDepthStencilBuffer : kClearDepthStencilBuffer);

	orthoCamera.Invalidate();
	orthoCamera.Update();

	TheGraphicsMgr->SetCamera(object, &orthoCamera);
}

void OrthoViewportWidget::SetZoomScale(const Vector2D& scale, const Point3D& position, bool integer)
{
	Vector2D dp = (position.GetVector2D() - GetWidgetSize() * 0.5F) & (orthoScale - scale);
	orthoScale = scale;

	const Point3D& cameraPosition = orthoCamera.GetNodePosition();
	const Vector3D& right = orthoCamera.GetNodeTransform()[0];
	const Vector3D& down = orthoCamera.GetNodeTransform()[1];

	if (!integer)
	{
		orthoCamera.SetNodePosition(cameraPosition + right * dp.x + down * dp.y);
	}
	else
	{
		float x = Floor(cameraPosition.x + right.x * dp.x + down.x * dp.y);
		float y = Floor(cameraPosition.y + right.y * dp.x + down.y * dp.y);
		orthoCamera.SetNodePosition(Point3D(x, y, cameraPosition.z));
	}
}


FrustumViewportWidget::FrustumViewportWidget(WidgetType type) :
		ViewportWidget(type, &frustumCamera),
		frustumCamera(2.0F, 1.0F)
{
	cameraAzimuth = 0.0F;
	cameraAltitude = 0.0F;
}

FrustumViewportWidget::FrustumViewportWidget(const Vector2D& size, float focalLength) : FrustumViewportWidget(kWidgetFrustumViewport, size, focalLength)
{
}

FrustumViewportWidget::FrustumViewportWidget(WidgetType type, const Vector2D& size, float focalLength) :
		ViewportWidget(type, &frustumCamera, size),
		frustumCamera(focalLength, 1.0F)
{
	cameraAzimuth = 0.0F;
	cameraAltitude = 0.0F;
}

FrustumViewportWidget::FrustumViewportWidget(const FrustumViewportWidget& frustumViewportWidget) :
		ViewportWidget(frustumViewportWidget, &frustumCamera),
		frustumCamera(frustumViewportWidget.frustumCamera.GetObject()->GetFocalLength(), 1.0F)
{
	cameraAzimuth = 0.0F;
	cameraAltitude = 0.0F;
}

FrustumViewportWidget::~FrustumViewportWidget()
{
}

Widget *FrustumViewportWidget::Replicate(void) const
{
	return (new FrustumViewportWidget(*this));
}

void FrustumViewportWidget::Pack(Packer& data, unsigned_int32 packFlags) const
{
	ViewportWidget::Pack(data, packFlags);

	data << TerminatorChunk;
}

void FrustumViewportWidget::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	ViewportWidget::Unpack(data, unpackFlags);
	UnpackChunkList<FrustumViewportWidget>(data, unpackFlags);
}

bool FrustumViewportWidget::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	return (false);
}

void FrustumViewportWidget::SetGraphicsCamera(bool clear)
{
	const Point3D& p = GetWorldPosition();
	float w = GetWidgetSize().x;
	float h = GetWidgetSize().y;

	FrustumCameraObject *object = frustumCamera.GetObject();
	object->SetViewRect(Rect((int32) p.x, (int32) p.y, (int32) (p.x + w), (int32) (p.y + h)));
	object->SetAspectRatio(h / w);

	object->SetClearFlags((clear) ? kClearColorBuffer | kClearDepthStencilBuffer : kClearDepthStencilBuffer);

	frustumCamera.Invalidate();
	frustumCamera.Update();

	TheGraphicsMgr->SetCamera(object, &frustumCamera);
}

void FrustumViewportWidget::SetCameraTransform(float azm, float alt, const Point3D& position)
{
	cameraAzimuth = azm;
	cameraAltitude = alt;

	Vector2D t = CosSin(azm);
	Vector2D p = CosSin(alt);
	Vector3D view(t.x * p.x, t.y * p.x, p.y);
	Vector3D right(t.y, -t.x, 0.0F);
	Vector3D down = view % right;

	frustumCamera.SetNodeTransform(Transform4D(right, down, view, position));
	InvalidateTexture();
}

void FrustumViewportWidget::SetCameraPosition(const Point3D& position)
{
	frustumCamera.SetNodePosition(position);
	InvalidateTexture();
}


WorldViewportWidget::WorldViewportWidget() : FrustumViewportWidget(kWidgetWorldViewport)
{
	viewportWorld = nullptr;
}

WorldViewportWidget::WorldViewportWidget(const Vector2D& size, float focalLength) : FrustumViewportWidget(kWidgetWorldViewport, size, focalLength)
{
	viewportWorld = nullptr;

	cameraDistance = 1.0F;
	cameraTarget.Set(0.0F, 0.0F, 0.0F);
}

WorldViewportWidget::~WorldViewportWidget()
{
	delete viewportWorld;
}

Widget *WorldViewportWidget::Replicate(void) const
{
	return (new WorldViewportWidget(*this));
}

void WorldViewportWidget::Preprocess(void)
{
	FrustumViewportWidget::Preprocess();

	SetRenderProc(&ViewportRender, this);
}

void WorldViewportWidget::EnableCameraOrbit(const Point3D& target, float distance)
{
	cameraTarget = target;
	cameraDistance = distance;
	maxCameraDistance = distance;

	trackFlag = false;
	SetMouseEventProc(&ViewportHandleMouseEvent, this);
}

void WorldViewportWidget::DisableCameraOrbit(void)
{
	SetMouseEventProc(nullptr);
}

void WorldViewportWidget::SetCameraAngles(float azm, float alt)
{
	Vector2D t = CosSin(azm);
	Vector2D p = CosSin(alt) * cameraDistance;
	SetCameraTransform(azm, alt, cameraTarget + Vector3D(-t.x * p.x, -t.y * p.x, -p.y));
}

void WorldViewportWidget::LoadWorld(const char *name)
{
	// Don't delete the previous world until after the new world is loaded so that any
	// resources used by both worlds don't get unloaded and immediately loaded again.

	World *previousWorld = viewportWorld;

	viewportWorld = new World(name, kWorldViewport | kWorldClearColor | kWorldMotionBlurInhibit | kWorldListenerInhibit);
	if (viewportWorld->Preprocess() == kWorldOkay)
	{
		viewportWorld->SetRenderSize((int32) GetWidgetSize().x, (int32) GetWidgetSize().y);

		FrustumCamera *camera = GetViewportCamera();
		camera->GetObject()->SetClearColor(ColorRGBA(0.0F, 0.125F, 0.1F, 0.0F));
		viewportWorld->SetCamera(camera);
	}
	else
	{
		delete viewportWorld;
		viewportWorld = nullptr;
	}

	delete previousWorld;
}

void WorldViewportWidget::UnloadWorld(void)
{
	delete viewportWorld;
	viewportWorld = nullptr;
}

void WorldViewportWidget::ViewportHandleMouseEvent(const MouseEventData *eventData, ViewportWidget *viewport, void *cookie)
{
	WorldViewportWidget *worldViewport = static_cast<WorldViewportWidget *>(cookie);

	EventType eventType = eventData->eventType;
	if (eventType == kEventMouseDown)
	{
		worldViewport->trackFlag = true;
		worldViewport->previousPosition = eventData->mousePosition;
	}
	else if (eventType == kEventMouseMoved)
	{
		if (worldViewport->trackFlag)
		{
			float azm = worldViewport->GetCameraAzimuth() + (worldViewport->previousPosition.x - eventData->mousePosition.x) * 0.03125F;
			if (azm < -K::tau_over_2)
			{
				azm += K::tau;
			}
			else if (azm > K::tau_over_2)
			{
				azm -= K::tau;
			}

			float alt = worldViewport->GetCameraAltitude() + (worldViewport->previousPosition.y - eventData->mousePosition.y) * 0.03125F;
			alt = Clamp(alt, -K::tau_over_4, K::tau_over_4);

			worldViewport->previousPosition = eventData->mousePosition;
			worldViewport->SetCameraAngles(azm, alt);
		}
	}
	else if (eventType == kEventMouseWheel)
	{
		float maxDistance = worldViewport->maxCameraDistance;
		float distance = worldViewport->cameraDistance - eventData->wheelDelta.y * (maxDistance * 0.0625F);
		worldViewport->cameraDistance = Clamp(distance, maxDistance * 0.125F, maxDistance);
		worldViewport->SetCameraAngles(worldViewport->GetCameraAzimuth(), worldViewport->GetCameraAltitude());
	}
	else
	{
		worldViewport->trackFlag = false;
	}
}

void WorldViewportWidget::ViewportRender(List<Renderable> *renderList, ViewportWidget *viewport, void *cookie)
{
	WorldViewportWidget *worldViewport = static_cast<WorldViewportWidget *>(cookie);

	World *world = worldViewport->viewportWorld;
	if (world)
	{
		unsigned_int32 mask = TheGraphicsMgr->GetTargetDisableMask();
		TheGraphicsMgr->SetTargetDisableMask(mask | ((1 << kRenderTargetReflection) | (1 << kRenderTargetRefraction)));

		world->Update();
		world->BeginRendering();
		world->Render();
		world->EndRendering();

		TheGraphicsMgr->SetTargetDisableMask(mask);
	}
}


Grid::Grid() :
		Renderable(kRenderLines),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
{
	gridFlags = 0;
	gridLineSpacing = 0.1F;
	majorLineInterval = 10;

	axisLineColor.Set(0x80, 0x80, 0x80, 0xFF);
	majorLineColor.Set(0x40, 0x40, 0x40, 0xFF);
	minorLineColor.Set(0x40, 0x40, 0x40, 0xFF);
	boundingBoxColor.Set(0x00, 0x00, 0x00, 0xFF);

	gridStorage = nullptr;
	gridStorageCount = 0;

	SetShaderFlags(kShaderAmbientEffect);
	SetAmbientBlendState(kBlendReplace | kBlendAlphaPreserve);

	SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(GridVertex));
	SetVertexAttributeArray(kArrayPosition, 0, 2);
	SetVertexAttributeArray(kArrayColor, sizeof(Point2D), 1);
}

Grid::~Grid()
{
	delete[] gridStorage;
}

void Grid::Build(const Point2D& min, const Point2D& max, float scale)
{
	float offset = scale * 0.5F;
	scale = 1.0F / scale;

	int32 count = 12;
	float spacing = gridLineSpacing;
	if (spacing * scale >= 4.0F)
	{
		int32 imin = (int32) Ceil(min.x / spacing);
		int32 imax = (int32) Floor(max.x / spacing);
		int32 jmin = (int32) Ceil(min.y / spacing);
		int32 jmax = (int32) Floor(max.y / spacing);

		count += (imax - imin + jmax - jmin + 2) * 2;
	}
	else
	{
		spacing *= (float) majorLineInterval;
		if (spacing * scale >= 4.0F)
		{
			int32 imin = (int32) Ceil(min.x / spacing);
			int32 imax = (int32) Floor(max.x / spacing);
			int32 jmin = (int32) Ceil(min.y / spacing);
			int32 jmax = (int32) Floor(max.y / spacing);

			count += (imax - imin + jmax - jmin + 2) * 2;
		}
	}

	count = (count + 63) & ~63;
	if (count > gridStorageCount)
	{
		delete[] gridStorage;
		gridStorageCount = count;
		gridStorage = new GridVertex[count];
	}

	InvalidateVertexData();
	vertexBuffer.Establish(count * sizeof(GridVertex));

	GridVertex *vertex = gridStorage;

	float xmin = min.x + offset;
	float xmax = max.x + offset;
	float ymin = min.y + offset;
	float ymax = max.y + offset;

	spacing = gridLineSpacing;
	if (spacing * scale >= 4.0F)
	{
		int32 imin = (int32) Ceil(min.x / spacing);
		int32 imax = (int32) Floor(max.x / spacing);

		for (machine i = imin; i <= imax; i++)
		{
			if (i % majorLineInterval != 0)
			{
				float x = (float) i * spacing + offset;

				vertex[0].position.Set(x, ymin);
				vertex[1].position.Set(x, ymax);
				vertex[0].color = minorLineColor;
				vertex[1].color = minorLineColor;
				vertex += 2;
			}
		}

		int32 jmin = (int32) Ceil(min.y / spacing);
		int32 jmax = (int32) Floor(max.y / spacing);

		for (machine j = jmin; j <= jmax; j++)
		{
			if (j % majorLineInterval != 0)
			{
				float y = (float) j * spacing + offset;

				vertex[0].position.Set(xmin, y);
				vertex[1].position.Set(xmax, y);
				vertex[0].color = minorLineColor;
				vertex[1].color = minorLineColor;
				vertex += 2;
			}
		}
	}

	spacing *= (float) majorLineInterval;
	if (spacing * scale >= 4.0F)
	{
		int32 imin = (int32) Ceil(min.x / spacing);
		int32 imax = (int32) Floor(max.x / spacing);

		for (machine i = imin; i <= imax; i++)
		{
			if (i != 0)
			{
				float x = (float) i * spacing + offset;

				vertex[0].position.Set(x, ymin);
				vertex[1].position.Set(x, ymax);
				vertex[0].color = majorLineColor;
				vertex[1].color = majorLineColor;
				vertex += 2;
			}
		}

		int32 jmin = (int32) Ceil(min.y / spacing);
		int32 jmax = (int32) Floor(max.y / spacing);

		for (machine j = jmin; j <= jmax; j++)
		{
			if (j != 0)
			{
				float y = (float) j * spacing + offset;

				vertex[0].position.Set(xmin, y);
				vertex[1].position.Set(xmax, y);
				vertex[0].color = majorLineColor;
				vertex[1].color = majorLineColor;
				vertex += 2;
			}
		}
	}

	vertex[0].position.Set(offset, ymin);
	vertex[1].position.Set(offset, ymax);
	vertex[0].color = axisLineColor;
	vertex[1].color = axisLineColor;

	vertex[2].position.Set(xmin, offset);
	vertex[3].position.Set(xmax, offset);
	vertex[2].color = axisLineColor;
	vertex[3].color = axisLineColor;

	vertex += 4;

	if (gridFlags & kGridShowBoundingBox)
	{
		vertex[0].position.Set(boundingBoxMin.x + offset, boundingBoxMin.y + offset);
		vertex[1].position.Set(boundingBoxMin.x + offset, boundingBoxMax.y + offset);
		vertex[2].position.Set(boundingBoxMin.x + offset, boundingBoxMax.y + offset);
		vertex[3].position.Set(boundingBoxMax.x + offset, boundingBoxMax.y + offset);
		vertex[4].position.Set(boundingBoxMax.x + offset, boundingBoxMax.y + offset);
		vertex[5].position.Set(boundingBoxMax.x + offset, boundingBoxMin.y + offset);
		vertex[6].position.Set(boundingBoxMax.x + offset, boundingBoxMin.y + offset);
		vertex[7].position.Set(boundingBoxMin.x + offset, boundingBoxMin.y + offset);

		for (machine a = 0; a < 8; a++)
		{
			vertex[a].color = boundingBoxColor;
		}

		vertex += 8;
	}

	SetVertexCount((int32) (vertex - gridStorage));
	vertexBuffer.UpdateBuffer(0, GetVertexCount() * sizeof(GridVertex), gridStorage);
}


DragRect::DragRect(const ColorRGBA& color) :
		Renderable(kRenderQuads),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		diffuseColor(color)
{
	SetVertexCount(16);
	SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(Point3D));
	SetVertexAttributeArray(kArrayPosition, 0, 3);
	vertexBuffer.Establish(sizeof(Point3D) * 16);

	SetShaderFlags(kShaderAmbientEffect);
	SetAmbientBlendState(kBlendInterpolate | kBlendAlphaPreserve);
	GetFirstRenderSegment()->SetMaterialState(kMaterialTwoSided);

	attributeList.Append(&diffuseColor);
	SetMaterialAttributeList(&attributeList);
}

DragRect::~DragRect()
{
}

void DragRect::Build(const Point2D& p1, const Point2D& p2, float scale)
{
	volatile Point3D *restrict vertex = vertexBuffer.BeginUpdate<Point3D>();

	float x1 = Fmin(p1.x, p2.x);
	float x2 = Fmax(p1.x, p2.x);
	float y1 = Fmin(p1.y, p2.y);
	float y2 = Fmax(p1.y, p2.y);

	vertex[0].Set(x1, y1, 0.0F);
	vertex[1].Set(x1, y1 + scale, 0.0F);
	vertex[2].Set(x2 + scale, y1 + scale, 0.0F);
	vertex[3].Set(x2 + scale, y1, 0.0F);

	vertex[4].Set(x1, y2, 0.0F);
	vertex[5].Set(x1, y2 + scale, 0.0F);
	vertex[6].Set(x2 + scale, y2 + scale, 0.0F);
	vertex[7].Set(x2 + scale, y2, 0.0F);

	vertex[8].Set(x1, y1 + scale, 0.0F);
	vertex[9].Set(x1, y2, 0.0F);
	vertex[10].Set(x1 + scale, y2, 0.0F);
	vertex[11].Set(x1 + scale, y1, 0.0F);

	vertex[12].Set(x2, y1 + scale, 0.0F);
	vertex[13].Set(x2, y2, 0.0F);
	vertex[14].Set(x2 + scale, y2, 0.0F);
	vertex[15].Set(x2 + scale, y1, 0.0F);

	vertexBuffer.EndUpdate();
}

void DragRect::Build(const Point2D& p1, const Point2D& p2, const Vector3D& dx, const Vector3D& dy, float scale)
{
	volatile Point3D *restrict vertex = vertexBuffer.BeginUpdate<Point3D>();

	float x1 = Fmin(p1.x, p2.x);
	float x2 = Fmax(p1.x, p2.x);
	float y1 = Fmin(p1.y, p2.y);
	float y2 = Fmax(p1.y, p2.y);

	vertex[0] = dx * x1 + dy * y1;
	vertex[1] = dx * x1 + dy * (y1 + scale);
	vertex[2] = dx * (x2 + scale) + dy * (y1 + scale);
	vertex[3] = dx * (x2 + scale) + dy * y1;

	vertex[4] = dx * x1 + dy * y2;
	vertex[5] = dx * x1 + dy * (y2 + scale);
	vertex[6] = dx * (x2 + scale) + dy * (y2 + scale);
	vertex[7] = dx * (x2 + scale) + dy * y2;

	vertex[8] = dx * x1 + dy * (y1 + scale);
	vertex[9] = dx * x1 + dy * y2;
	vertex[10] = dx * (x1 + scale) + dy * y2;
	vertex[11] = dx * (x1 + scale) + dy * y1;

	vertex[12] = dx * x2 + dy * (y1 + scale);
	vertex[13] = dx * x2 + dy * y2;
	vertex[14] = dx * (x2 + scale) + dy * y2;
	vertex[15] = dx * (x2 + scale) + dy * y1;

	vertexBuffer.EndUpdate();
}

// ZYUQURM
