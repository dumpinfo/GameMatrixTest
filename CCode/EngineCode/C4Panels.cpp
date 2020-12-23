 

#include "C4Panels.h"
#include "C4Paint.h"
#include "C4Movies.h"


using namespace C4;


namespace
{
	enum
	{
		kPanelEngaged		= 1 << 0
	};
}


CameraPortalObject::CameraPortalObject() : PortalObject(kPortalCamera)
{
	minDetailLevel = 0;
	detailLevelBias = 0.0F;
}

CameraPortalObject::CameraPortalObject(const Vector2D& size, int32 width, int32 height) : PortalObject(kPortalCamera, size)
{
	viewportWidth = width;
	viewportHeight = height;

	minDetailLevel = 0;
	detailLevelBias = 0.0F;
}

CameraPortalObject::~CameraPortalObject()
{
}

void CameraPortalObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	PortalObject::Pack(data, packFlags);

	data << ChunkHeader('VPRT', 8);
	data << viewportWidth;
	data << viewportHeight;

	data << ChunkHeader('MLEV', 4);
	data << minDetailLevel;

	data << ChunkHeader('BIAS', 4);
	data << detailLevelBias;

	data << TerminatorChunk;
}

void CameraPortalObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	PortalObject::Unpack(data, unpackFlags);
	UnpackChunkList<CameraPortalObject>(data, unpackFlags);
}

bool CameraPortalObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'VPRT':

			data >> viewportWidth;
			data >> viewportHeight;
			return (true);

		case 'MLEV':

			data >> minDetailLevel;
			return (true);

		case 'BIAS':

			data >> detailLevelBias;
			return (true);
	}

	return (false);
}


CameraPortal::CameraPortal() : Portal(kPortalCamera)
{
	cameraTexture = nullptr;
	renderSizeProc = nullptr;
}

CameraPortal::CameraPortal(const Vector2D& size, int32 width, int32 height) : Portal(kPortalCamera)
{
	SetNewObject(new CameraPortalObject(size, width, height));

	cameraTexture = nullptr;
	renderSizeProc = nullptr;
}

CameraPortal::CameraPortal(const CameraPortal& cameraPortal) : Portal(cameraPortal)
{
	cameraTexture = nullptr; 
	renderSizeProc = nullptr;
}
 
CameraPortal::~CameraPortal()
{ 
	if (cameraTexture)
	{
		cameraTexture->Release(); 
	}
} 
 
Node *CameraPortal::Replicate(void) const
{
	return (new CameraPortal(*this));
} 

void CameraPortal::SetCameraTexture(Texture *texture)
{
	if (texture != cameraTexture)
	{
		if (cameraTexture)
		{
			cameraTexture->Release();
		}

		texture->Retain();
		cameraTexture = texture;
	}
}


PanelController::PanelController() :
		ScriptController(kControllerPanel),
		keyboardEventHandler(&HandleKeyboardEvent, this),
		rootWidget(Vector2D(256.0F, 256.0F))
{
	SetBaseControllerType(kControllerPanel);

	panelState = 0;
	panelSleepTime = 1;
	keyboardWidget = nullptr;
}

PanelController::PanelController(const PanelController& panelController) :
		ScriptController(panelController),
		panelCursor(panelController.panelCursor.GetTextureName()),
		keyboardEventHandler(&HandleKeyboardEvent, this),
		rootWidget(panelController.rootWidget.GetWidgetSize())
{
	panelState = 0;
	panelSleepTime = panelController.panelSleepTime;
	keyboardWidget = nullptr;

	const Widget *widget = panelController.rootWidget.GetFirstSubnode();
	while (widget)
	{
		rootWidget.AppendSubnode(widget->Clone());
		widget = widget->Next();
	}
}

PanelController::~PanelController()
{
}

Controller *PanelController::Replicate(void) const
{
	return (new PanelController(*this));
}

bool PanelController::ValidNode(const Node *node)
{
	return ((node->GetNodeType() == kNodeEffect) && (static_cast<const Effect *>(node)->GetEffectType() == kEffectPanel));
}

void PanelController::RegisterFunctions(ControllerRegistration *registration)
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	static FunctionReg<EnableWidgetFunction> enableWidgetRegistration(registration, kFunctionEnableWidget, table->GetString(StringID('CTRL', kControllerPanel, kFunctionEnableWidget)), kFunctionRemote | kFunctionJournaled);
	static FunctionReg<DisableWidgetFunction> disableWidgetRegistration(registration, kFunctionDisableWidget, table->GetString(StringID('CTRL', kControllerPanel, kFunctionDisableWidget)), kFunctionRemote | kFunctionJournaled);
	static FunctionReg<ShowWidgetFunction> showWidgetRegistration(registration, kFunctionShowWidget, table->GetString(StringID('CTRL', kControllerPanel, kFunctionShowWidget)), kFunctionRemote | kFunctionJournaled);
	static FunctionReg<HideWidgetFunction> hideWidgetRegistration(registration, kFunctionHideWidget, table->GetString(StringID('CTRL', kControllerPanel, kFunctionHideWidget)), kFunctionRemote | kFunctionJournaled);
	static FunctionReg<ActivateWidgetFunction> activateWidgetRegistration(registration, kFunctionActivateWidget, table->GetString(StringID('CTRL', kControllerPanel, kFunctionActivateWidget)));
	static FunctionReg<SetMutatorStateFunction> setMutatorStateRegistration(registration, kFunctionSetMutatorState, table->GetString(StringID('CTRL', kControllerPanel, kFunctionSetMutatorState)), kFunctionRemote | kFunctionJournaled);
	static FunctionReg<GetWidgetColorFunction> getWidgetColorRegistration(registration, kFunctionGetWidgetColor, table->GetString(StringID('CTRL', kControllerPanel, kFunctionGetWidgetColor)), kFunctionOutputValue);
	static FunctionReg<SetWidgetColorFunction> setWidgetColorRegistration(registration, kFunctionSetWidgetColor, table->GetString(StringID('CTRL', kControllerPanel, kFunctionSetWidgetColor)), kFunctionRemote | kFunctionJournaled);
	static FunctionReg<GetWidgetImageFunction> getWidgetImageRegistration(registration, kFunctionGetWidgetImage, table->GetString(StringID('CTRL', kControllerPanel, kFunctionGetWidgetImage)), kFunctionOutputValue);
	static FunctionReg<SetWidgetImageFunction> setWidgetImageRegistration(registration, kFunctionSetWidgetImage, table->GetString(StringID('CTRL', kControllerPanel, kFunctionSetWidgetImage)), kFunctionRemote | kFunctionJournaled);
	static FunctionReg<GetWidgetTextFunction> getWidgetTextRegistration(registration, kFunctionGetWidgetText, table->GetString(StringID('CTRL', kControllerPanel, kFunctionGetWidgetText)), kFunctionOutputValue);
	static FunctionReg<SetWidgetTextFunction> setWidgetTextRegistration(registration, kFunctionSetWidgetText, table->GetString(StringID('CTRL', kControllerPanel, kFunctionSetWidgetText)), kFunctionRemote | kFunctionJournaled);
	static FunctionReg<GetWidgetValueFunction> getWidgetValueRegistration(registration, kFunctionGetWidgetValue, table->GetString(StringID('CTRL', kControllerPanel, kFunctionGetWidgetValue)), kFunctionOutputValue);
	static FunctionReg<SetWidgetValueFunction> setWidgetValueRegistration(registration, kFunctionSetWidgetValue, table->GetString(StringID('CTRL', kControllerPanel, kFunctionSetWidgetValue)), kFunctionRemote | kFunctionJournaled);
	static FunctionReg<SetCameraConnectorKeyFunction> setCameraConnectorKeyRegistration(registration, kFunctionSetCameraConnectorKey, table->GetString(StringID('CTRL', kControllerPanel, kFunctionSetCameraConnectorKey)), kFunctionRemote | kFunctionJournaled);

	PaintWidget::RegisterFunctions(registration);
	MovieWidget::RegisterFunctions(registration);
}

void PanelController::Prepack(List<Object> *linkList) const
{
	ScriptController::Prepack(linkList);

	const Widget *widget = rootWidget.GetFirstSubnode();
	while (widget)
	{
		widget->Prepack(linkList);
		widget = rootWidget.GetNextNode(widget);
	}
}

void PanelController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	ScriptController::Pack(data, packFlags);

	data << ChunkHeader('SIZE', sizeof(Vector2D));
	data << rootWidget.GetWidgetSize();

	data << ChunkHeader('SLEP', 4);
	data << panelSleepTime;

	PackHandle handle = data.BeginChunk('CURS');
	data << panelCursor.GetTextureName();
	data.EndChunk(handle);

	int32 widgetCount = 0;
	rootWidget.widgetIndex = -1;

	const Widget *widget = rootWidget.GetFirstSubnode();
	while (widget)
	{
		widget->widgetIndex = widgetCount++;
		widget = rootWidget.GetNextNode(widget);
	}

	handle = data.BeginChunk('WDGT');
	data << widgetCount;

	widget = rootWidget.GetFirstSubnode();
	while (widget)
	{
		PackHandle section = data.BeginSection();
		widget->PackType(data);
		widget->Pack(data, packFlags);
		data.EndSection(section);

		widget = rootWidget.GetNextNode(widget);
	}

	data.EndChunk(handle);
	data << TerminatorChunk;
}

void PanelController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	ScriptController::Unpack(data, unpackFlags);
	UnpackChunkList<PanelController>(data, unpackFlags);
}

bool PanelController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'SIZE':

			data >> rootWidget.GetWidgetSize();
			return (true);

		case 'SLEP':

			data >> panelSleepTime;
			return (true);

		case 'CURS':
		{
			ResourceName	textureName;

			data >> textureName;
			if (textureName[0] != 0)
			{
				panelCursor.SetTexture(textureName);
			}

			return (true);
		}

		case 'WDGT':
		{
			int32	widgetCount;

			data >> widgetCount;
			Widget **widgetTable = new Widget *[widgetCount];

			for (machine a = 0; a < widgetCount; a++)
			{
				unsigned_int32	size;

				data >> size;
				const void *mark = data.GetPointer();

				Widget *widget = Widget::Create(data, unpackFlags);
				if (widget)
				{
					widget->Unpack(++data, unpackFlags);
					widgetTable[a] = widget;
				}
				else
				{
					data.Skip(mark, size);
					widgetTable[a] = nullptr;
				}
			}

			for (machine a = 0; a < widgetCount; a++)
			{
				Widget *widget = widgetTable[a];
				if (widget)
				{
					int32 index = widget->superIndex;
					if (index >= 0)
					{
						Widget *super = widgetTable[index];
						if (super)
						{
							super->AppendSubnode(widget);
						}
						else
						{
							delete widget;
							widgetTable[a] = nullptr;
						}
					}
					else
					{
						rootWidget.AppendSubnode(widget);
					}
				}
			}

			delete[] widgetTable;
			return (true);
		}
	}

	return (false);
}

void *PanelController::BeginSettingsUnpack(void)
{
	rootWidget.PurgeSubtree();
	return (ScriptController::BeginSettingsUnpack());
}

int32 PanelController::GetSettingCount(void) const
{
	return (4);
}

Setting *PanelController::GetSetting(int32 index) const
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('CTRL', 'PANL', 'INVS'));
		return (new TextSetting('INVS', (float) (panelSleepTime - 1) * 0.001F, title));
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('CTRL', 'PANL', 'WIDE'));
		return (new IntegerSetting('WIDE', (int32) rootWidget.GetWidgetSize().x, title, 16, 2048, 16));
	}

	if (index == 2)
	{
		const char *title = table->GetString(StringID('CTRL', 'PANL', 'HIGH'));
		return (new IntegerSetting('HIGH', (int32) rootWidget.GetWidgetSize().y, title, 16, 2048, 16));
	}

	if (index == 3)
	{
		const char *title = table->GetString(StringID('CTRL', 'PANL', 'CURS'));
		const char *picker = table->GetString(StringID('CTRL', 'PANL', 'PICK'));
		return (new ResourceSetting('CURS', panelCursor.GetTextureName(), title, picker, TextureResource::GetDescriptor()));
	}

	return (nullptr);
}

void PanelController::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'INVS')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		panelSleepTime = MaxZero((int32) (Text::StringToFloat(text) * 1000.0F)) + 1;
	}
	else if (identifier == 'WIDE')
	{
		rootWidget.GetWidgetSize().x = (float) static_cast<const IntegerSetting *>(setting)->GetIntegerValue();
	}
	else if (identifier == 'HIGH')
	{
		rootWidget.GetWidgetSize().y = (float) static_cast<const IntegerSetting *>(setting)->GetIntegerValue();
	}
	else if (identifier == 'CURS')
	{
		panelCursor.SetTexture(static_cast<const ResourceSetting *>(setting)->GetResourceName());
	}
}

void PanelController::SendInitialStateMessages(Player *player) const
{
	const Widget *widget = rootWidget.GetFirstSubnode();
	while (widget)
	{
		widget->SendInitialStateMessages(player);
		widget = rootWidget.GetNextNode(widget);
	}
}

void PanelController::Preprocess(void)
{
	ScriptController::Preprocess();
	SetScriptFlags(kScriptReentrant);

	savedInputMode = 0;

	PanelEffect *panelEffect = static_cast<PanelEffect *>(GetTargetNode());
	const Vector2D& externalPanelSize = panelEffect->GetObject()->GetExternalPanelSize();
	const Vector2D& internalPanelSize = rootWidget.GetWidgetSize();
	panelEffect->panelScale.Set(externalPanelSize.x / internalPanelSize.x, externalPanelSize.y / internalPanelSize.y);

	if (panelEffect->panelInvisibleTime == 0)
	{
		panelEffect->panelInvisibleTime = panelSleepTime;
	}

	panelCursor.SetRenderableFlags(kRenderableStructureVelocityZero | kRenderableStructureDepthZero | kRenderableFogInhibit);

	rootWidget.SetPanelController(this);
	rootWidget.Preprocess();
}

void PanelController::Move(void)
{
	PanelEffect *panelEffect = static_cast<PanelEffect *>(GetTargetNode());

	int32 time = panelEffect->panelInvisibleTime;
	if (time <= panelSleepTime)
	{
		panelEffect->panelInvisibleTime = time + TheTimeMgr->GetDeltaTime();

		rootWidget.Move();

		if (panelState & kPanelEngaged)
		{
			if (panelCursor.GetTexture())
			{
				panelEffect->panelCursor = &panelCursor;
			}
		}
		else
		{
			panelEffect->panelCursor = nullptr;
		}
	}

	ScriptController::Move();
}

void PanelController::HandleInteractionEvent(InteractionEventType type, const Point3D *position, Node *initiator)
{
	PanelMouseEventData		eventData;

	eventData.eventFlags = 0;
	eventData.initiatorNode = initiator;

	PanelEffect *panelEffect = static_cast<PanelEffect *>(GetTargetNode());

	unsigned_int32 state = panelState;
	if (position)
	{
		const Vector2D& externalPanelSize = panelEffect->GetObject()->GetExternalPanelSize();
		const Vector2D& internalPanelSize = rootWidget.GetWidgetSize();

		float x = Fmin(FmaxZero(position->x / externalPanelSize.x * internalPanelSize.x), internalPanelSize.x);
		float y = Fmin(FmaxZero((1.0F - position->y / externalPanelSize.y) * internalPanelSize.y), internalPanelSize.y);
		Point3D panelPosition(x, y, 0.0F);

		if (trackWidget)
		{
			eventData.mousePosition = trackWidget->GetInverseWorldTransform() * panelPosition;
			eventData.widgetPart = trackPart;

			if (type == kInteractionEventTrack)
			{
				eventData.eventType = kEventMouseMoved;
				panelEffect->cursorPosition = panelPosition;
				trackWidget->HandleMouseEvent(&eventData);
			}
			else if (type == kInteractionEventDeactivate)
			{
				eventData.eventType = kEventMouseUp;
				trackWidget->HandleMouseEvent(&eventData);
				trackWidget = nullptr;
			}
		}
		else
		{
			if (type == kInteractionEventEngage)
			{
				panelState = state | kPanelEngaged;
				panelEffect->cursorPosition = panelPosition;
			}
			else if (type == kInteractionEventTrack)
			{
				panelEffect->cursorPosition = panelPosition;

				Widget *prevHoverWidget = hoverWidget;
				Widget *newHoverWidget = rootWidget.DetectWidget(panelPosition);
				if (newHoverWidget)
				{
					if (newHoverWidget != prevHoverWidget)
					{
						hoverWidget = newHoverWidget;

						if (prevHoverWidget)
						{
							prevHoverWidget->PostWidgetEvent(WidgetEventData(kEventWidgetEndHover));
						}

						newHoverWidget->PostWidgetEvent(WidgetEventData(kEventWidgetBeginHover));
					}
				}
				else if (prevHoverWidget)
				{
					hoverWidget = nullptr;
					prevHoverWidget->PostWidgetEvent(WidgetEventData(kEventWidgetEndHover));
				}
			}
			else if (type == kInteractionEventActivate)
			{
				Widget *activeWidget = rootWidget.DetectWidget(panelPosition, 0, &eventData.widgetPart);
				if (activeWidget)
				{
					Widget *prevHoverWidget = hoverWidget;
					if (prevHoverWidget)
					{
						hoverWidget = nullptr;
						prevHoverWidget->PostWidgetEvent(WidgetEventData(kEventWidgetEndHover));
					}

					trackWidget = activeWidget;
					trackPart = eventData.widgetPart;

					if (activeWidget->GetWidgetUsage() & kWidgetKeyboardFocus)
					{
						rootWidget.SetFocusWidget(activeWidget);
					}

					eventData.eventType = kEventMouseDown;
					eventData.mousePosition = activeWidget->GetInverseWorldTransform() * panelPosition;
					activeWidget->HandleMouseEvent(&eventData);
				}
			}
		}
	}
	else
	{
		if (type == kInteractionEventDisengage)
		{
			panelState = state & ~kPanelEngaged;

			Widget *widget = hoverWidget;
			if (widget)
			{
				hoverWidget = nullptr;
				widget->PostWidgetEvent(WidgetEventData(kEventWidgetEndHover));
			}

			widget = trackWidget;
			if (widget)
			{
				trackWidget = nullptr;

				eventData.eventType = kEventMouseUp;
				eventData.mousePosition = widget->GetInverseWorldTransform() * panelEffect->cursorPosition;
				eventData.widgetPart = trackPart;
				widget->HandleMouseEvent(&eventData);
			}

			if (keyboardWidget)
			{
				EndKeyboardInteraction();
			}
		}
	}
}

void PanelController::BeginKeyboardInteraction(Widget *widget)
{
	keyboardWidget = widget;
	TheEngine->InstallKeyboardEventHandler(&keyboardEventHandler);

	if (savedInputMode == 0)
	{
		savedInputMode = TheInputMgr->GetInputMode();
		TheInputMgr->SetInputMode(savedInputMode & ~kInputKeyboardActive);
	}
}

void PanelController::EndKeyboardInteraction(void)
{
	keyboardWidget = nullptr;
	keyboardEventHandler.Detach();

	TheInputMgr->SetInputMode(savedInputMode);
	savedInputMode = 0;
}

bool PanelController::HandleKeyboardEvent(const KeyboardEventData *eventData, void *cookie)
{
	PanelController *panelController = static_cast<PanelController *>(cookie);

	if ((eventData->eventType == kEventKeyDown) && (eventData->keyCode == kKeyCodeEscape))
	{
		panelController->EndKeyboardInteraction();
	}
	else
	{
		panelController->keyboardWidget->HandleKeyboardEvent(eventData);
	}

	return (true);
}

void PanelController::ExtendAnimationTime(float time)
{
	PanelEffect *panelEffect = static_cast<PanelEffect *>(GetTargetNode());
	panelEffect->panelInvisibleTime = Min(panelEffect->panelInvisibleTime, -((int32) time + 1));
}


CameraWidget::CameraWidget() : ImageWidget(kWidgetCamera)
{
	cameraMinDetailLevel = 0;
	cameraDetailLevelBias = 0.0F;

	cameraPortal = nullptr;
	SetWidgetUsage(kWidgetGeneratedImage);
}

CameraWidget::CameraWidget(const Vector2D& size) : ImageWidget(kWidgetCamera, size)
{
	cameraConnectorKey[0] = 0;

	cameraMinDetailLevel = 0;
	cameraDetailLevelBias = 0.0F;

	cameraPortal = nullptr;
	SetWidgetUsage(kWidgetGeneratedImage);
}

CameraWidget::CameraWidget(const CameraWidget& cameraWidget) : ImageWidget(cameraWidget)
{
	cameraConnectorKey = cameraWidget.cameraConnectorKey;

	cameraMinDetailLevel = cameraWidget.cameraMinDetailLevel;
	cameraDetailLevelBias = cameraWidget.cameraDetailLevelBias;

	cameraPortal = nullptr;
}

CameraWidget::~CameraWidget()
{
	delete cameraPortal;
}

Widget *CameraWidget::Replicate(void) const
{
	return (new CameraWidget(*this));
}

void CameraWidget::Pack(Packer& data, unsigned_int32 packFlags) const
{
	ImageWidget::Pack(data, packFlags);

	PackHandle handle = data.BeginChunk('CCON');
	data << cameraConnectorKey;
	data.EndChunk(handle);

	data << ChunkHeader('MLEV', 4);
	data << cameraMinDetailLevel;

	data << ChunkHeader('BIAS', 4);
	data << cameraDetailLevelBias;

	data << TerminatorChunk;
}

void CameraWidget::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	ImageWidget::Unpack(data, unpackFlags);
	UnpackChunkList<CameraWidget>(data, unpackFlags);
}

bool CameraWidget::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'CCON':

			data >> cameraConnectorKey;
			return (true);

		case 'MLEV':

			data >> cameraMinDetailLevel;
			return (true);

		case 'BIAS':

			data >> cameraDetailLevelBias;
			return (true);
	}

	return (false);
}

int32 CameraWidget::GetSettingCount(void) const
{
	return (ImageWidget::GetSettingCount() + 4);
}

Setting *CameraWidget::GetSetting(int32 index) const
{
	int32 count = ImageWidget::GetSettingCount();
	if (index < count)
	{
		return (ImageWidget::GetSetting(index));
	}

	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetCamera, 'SETT'));
		return (new HeadingSetting(kWidgetCamera, title));
	}

	if (index == count + 1)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetCamera, 'CCON'));
		return (new TextSetting('CCON', cameraConnectorKey, title, kMaxConnectorKeyLength, &Connector::ConnectorKeyFilter));
	}

	if (index == count + 2)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetCamera, 'MLEV'));
		return (new IntegerSetting('MLEV', cameraMinDetailLevel, title, 0, 3, 1));
	}

	if (index == count + 3)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetCamera, 'BIAS'));
		return (new TextSetting('BIAS', cameraDetailLevelBias, title));
	}

	return (nullptr);
}

void CameraWidget::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'CCON')
	{
		cameraConnectorKey = static_cast<const TextSetting *>(setting)->GetText();
	}
	else if (identifier == 'MLEV')
	{
		cameraMinDetailLevel = static_cast<const IntegerSetting *>(setting)->GetIntegerValue();
	}
	else if (identifier == 'BIAS')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		cameraDetailLevelBias = Text::StringToFloat(text);
	}
	else
	{
		ImageWidget::SetSetting(setting);
	}
}

void CameraWidget::SetTargetCamera(void)
{
	Node *node = GetPanelController()->GetTargetNode()->GetConnectedNode(cameraConnectorKey);
	cameraPortal->SetTargetCamera(((node) && (node->GetNodeType() == kNodeCamera)) ? static_cast<FrustumCamera *>(node) : nullptr);
}

void CameraWidget::SetRenderSize(int32 width, int32 height, void *cookie)
{
	CameraWidget *widget = static_cast<CameraWidget *>(cookie);

	const Vector2D& size = widget->GetWidgetSize();
	float w = (float) width;
	float h = (float) height;

	widget->SetImageScale(Vector2D(w / size.x, h / size.y));
}

void CameraWidget::SetWidgetState(unsigned_int32 state)
{
	if (cameraPortal)
	{
		if (state & kWidgetHidden)
		{
			cameraPortal->Disable();
		}
		else
		{
			cameraPortal->Enable();
		}
	}

	ImageWidget::SetWidgetState(state);
}

void CameraWidget::Preprocess(void)
{
	ImageWidget::Preprocess();

	if (cameraPortal)
	{
		SetTexture(0, nullptr, nullptr);

		delete cameraPortal;
		cameraPortal = nullptr;
	}

	RootWidget *root = GetRootWidget();
	if (root)
	{
		root->RemoveMovingWidget(this);
	}

	if (!GetManipulator())
	{
		PanelController *controller = GetPanelController();
		if ((controller) && (!controller->GetTargetNode()->GetManipulator()))
		{
			if (root)
			{
				root->AddMovingWidget(this);
			}

			const Vector2D& size = GetWidgetSize();
			int32 width = (int32) size.x;
			int32 height = (int32) size.y;

			textureHeader.textureType = kTextureRectangle;
			textureHeader.textureFlags = kTextureRenderTarget | kTextureAnisotropicFilterInhibit;
			textureHeader.colorSemantic = kTextureSemanticNone;
			textureHeader.alphaSemantic = kTextureSemanticNone;
			textureHeader.imageFormat = kTextureRGBA8;
			textureHeader.imageWidth = width;
			textureHeader.imageHeight = height;
			textureHeader.imageDepth = 1;
			textureHeader.wrapMode[0] = kTextureClamp;
			textureHeader.wrapMode[1] = kTextureClamp;
			textureHeader.wrapMode[2] = kTextureClamp;
			textureHeader.mipmapCount = 1;
			textureHeader.mipmapDataOffset = 0;
			textureHeader.auxiliaryDataSize = 0;
			textureHeader.auxiliaryDataOffset = 0;

			SetTexture(0, &textureHeader);
			SetImageScale(Vector2D(1.0F, 1.0F));

			PanelEffect *effect = static_cast<PanelEffect *>(controller->GetTargetNode());
			cameraPortal = new CameraPortal(effect->GetObject()->GetExternalPanelSize(), width, height);

			CameraPortalObject *object = cameraPortal->GetObject();
			object->SetMinDetailLevel(cameraMinDetailLevel);
			object->SetDetailLevelBias(cameraDetailLevelBias);

			cameraPortal->SetRenderSizeProc(&SetRenderSize, this);
			cameraPortal->SetCameraTexture(GetTexture());
			cameraPortal->SetNodeFlags((Visible()) ? kNodeNonpersistent : kNodeNonpersistent | kNodeDisabled);
			cameraPortal->SetPerspectiveExclusionMask(effect->GetPerspectiveExclusionMask());
			effect->AppendSubnode(cameraPortal);

			SetTargetCamera();
		}
	}

	if (!cameraPortal)
	{
		SetTexture(0, "C4/checker");
	}
}

void CameraWidget::Move(void)
{
	ImageWidget::Move();
	SetTargetCamera();
}

void CameraWidget::Build(void)
{
	if (!cameraPortal)
	{
		const Vector2D& size = GetWidgetSize();
		SetImageScale(Vector2D(size.x * 0.03125F, size.y * 0.03125F));
	}

	ImageWidget::Build();
}


WidgetFunction::WidgetFunction(FunctionType type) : Function(type, kControllerPanel)
{
	widgetKey[0] = 0;
}

WidgetFunction::WidgetFunction(const WidgetFunction& widgetFunction) : Function(widgetFunction)
{
	widgetKey = widgetFunction.widgetKey;
}

WidgetFunction::~WidgetFunction()
{
}

void WidgetFunction::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Function::Pack(data, packFlags);

	PackHandle handle = data.BeginChunk('WKEY');
	data << widgetKey;
	data.EndChunk(handle);

	data << TerminatorChunk;
}

void WidgetFunction::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Function::Unpack(data, unpackFlags);
	UnpackChunkList<WidgetFunction>(data, unpackFlags);
}

bool WidgetFunction::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'WKEY':

			data >> widgetKey;
			return (true);
	}

	return (false);
}

void WidgetFunction::Compress(Compressor& data) const
{
	Function::Compress(data);

	data << widgetKey;
}

bool WidgetFunction::Decompress(Decompressor& data)
{
	if (Function::Decompress(data))
	{
		data >> widgetKey;
		return (true);
	}

	return (false);
}

int32 WidgetFunction::GetSettingCount(void) const
{
	return (1);
}

Setting *WidgetFunction::GetSetting(int32 index) const
{
	if (index == 0)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();
		const char *title = table->GetString(StringID('CTRL', kControllerPanel, 'FUNC', 'IDNT'));
		return (new TextSetting('IDNT', widgetKey, title, kMaxWidgetKeyLength));
	}

	return (nullptr);
}

void WidgetFunction::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'IDNT')
	{
		widgetKey = static_cast<const TextSetting *>(setting)->GetText();
	}
}

bool WidgetFunction::OverridesFunction(const Function *function) const
{
	if (function->GetFunctionType() == GetFunctionType())
	{
		const WidgetFunction *widgetFunction = static_cast<const WidgetFunction *>(function);
		return (widgetFunction->GetWidgetKey() == widgetKey);
	}

	return (false);
}


EnableWidgetFunction::EnableWidgetFunction() : WidgetFunction(kFunctionEnableWidget)
{
}

EnableWidgetFunction::EnableWidgetFunction(const EnableWidgetFunction& enableWidgetFunction) : WidgetFunction(enableWidgetFunction)
{
}

EnableWidgetFunction::~EnableWidgetFunction()
{
}

Function *EnableWidgetFunction::Replicate(void) const
{
	return (new EnableWidgetFunction(*this));
}

bool EnableWidgetFunction::OverridesFunction(const Function *function) const
{
	FunctionType type = function->GetFunctionType();
	if ((type == kFunctionEnableWidget) || (type == kFunctionDisableWidget))
	{
		return (static_cast<const WidgetFunction *>(function)->GetWidgetKey() == GetWidgetKey());
	}

	return (false);
}

void EnableWidgetFunction::Execute(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	const Panel *root = static_cast<PanelController *>(controller)->GetRootWidget();
	Widget *widget = root->FindWidget(GetWidgetKey());
	while (widget)
	{
		widget->SetWidgetState(widget->GetWidgetState() & ~kWidgetDisabled);
		widget = widget->GetNextWidgetWithSameKey();
	}

	CallCompletionProc();
}


DisableWidgetFunction::DisableWidgetFunction() : WidgetFunction(kFunctionDisableWidget)
{
}

DisableWidgetFunction::DisableWidgetFunction(const DisableWidgetFunction& disableWidgetFunction) : WidgetFunction(disableWidgetFunction)
{
}

DisableWidgetFunction::~DisableWidgetFunction()
{
}

Function *DisableWidgetFunction::Replicate(void) const
{
	return (new DisableWidgetFunction(*this));
}

bool DisableWidgetFunction::OverridesFunction(const Function *function) const
{
	FunctionType type = function->GetFunctionType();
	if ((type == kFunctionEnableWidget) || (type == kFunctionDisableWidget))
	{
		return (static_cast<const WidgetFunction *>(function)->GetWidgetKey() == GetWidgetKey());
	}

	return (false);
}

void DisableWidgetFunction::Execute(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	const Panel *root = static_cast<PanelController *>(controller)->GetRootWidget();
	Widget *widget = root->FindWidget(GetWidgetKey());
	while (widget)
	{
		widget->SetWidgetState(widget->GetWidgetState() | kWidgetDisabled);
		widget = widget->GetNextWidgetWithSameKey();
	}

	CallCompletionProc();
}


ShowWidgetFunction::ShowWidgetFunction() : WidgetFunction(kFunctionShowWidget)
{
}

ShowWidgetFunction::~ShowWidgetFunction()
{
}

ShowWidgetFunction::ShowWidgetFunction(const ShowWidgetFunction& showWidgetFunction) : WidgetFunction(showWidgetFunction)
{
}

Function *ShowWidgetFunction::Replicate(void) const
{
	return (new ShowWidgetFunction(*this));
}

bool ShowWidgetFunction::OverridesFunction(const Function *function) const
{
	FunctionType type = function->GetFunctionType();
	if ((type == kFunctionShowWidget) || (type == kFunctionHideWidget))
	{
		return (static_cast<const WidgetFunction *>(function)->GetWidgetKey() == GetWidgetKey());
	}

	return (false);
}

void ShowWidgetFunction::Execute(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	const Panel *root = static_cast<PanelController *>(controller)->GetRootWidget();
	Widget *widget = root->FindWidget(GetWidgetKey());
	while (widget)
	{
		widget->SetWidgetState(widget->GetWidgetState() & ~kWidgetHidden);
		widget = widget->GetNextWidgetWithSameKey();
	}

	CallCompletionProc();
}


HideWidgetFunction::HideWidgetFunction() : WidgetFunction(kFunctionHideWidget)
{
}

HideWidgetFunction::HideWidgetFunction(const HideWidgetFunction& hideWidgetFunction) : WidgetFunction(hideWidgetFunction)
{
}

HideWidgetFunction::~HideWidgetFunction()
{
}

Function *HideWidgetFunction::Replicate(void) const
{
	return (new HideWidgetFunction(*this));
}

bool HideWidgetFunction::OverridesFunction(const Function *function) const
{
	FunctionType type = function->GetFunctionType();
	if ((type == kFunctionShowWidget) || (type == kFunctionHideWidget))
	{
		return (static_cast<const WidgetFunction *>(function)->GetWidgetKey() == GetWidgetKey());
	}

	return (false);
}

void HideWidgetFunction::Execute(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	const Panel *root = static_cast<PanelController *>(controller)->GetRootWidget();
	Widget *widget = root->FindWidget(GetWidgetKey());
	while (widget)
	{
		widget->SetWidgetState(widget->GetWidgetState() | kWidgetHidden);
		widget = widget->GetNextWidgetWithSameKey();
	}

	CallCompletionProc();
}


ActivateWidgetFunction::ActivateWidgetFunction() : WidgetFunction(kFunctionActivateWidget)
{
}

ActivateWidgetFunction::ActivateWidgetFunction(const ActivateWidgetFunction& activateWidgetFunction) : WidgetFunction(activateWidgetFunction)
{
}

ActivateWidgetFunction::~ActivateWidgetFunction()
{
}

Function *ActivateWidgetFunction::Replicate(void) const
{
	return (new ActivateWidgetFunction(*this));
}

void ActivateWidgetFunction::Execute(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	PanelController *panelController = static_cast<PanelController *>(controller);
	const Panel *root = panelController->GetRootWidget();

	Widget *widget = root->FindWidget(GetWidgetKey());
	while (widget)
	{
		widget->Activate(state->GetInitiatorNode());
		widget = widget->GetNextWidgetWithSameKey();
	}

	CallCompletionProc();
}


SetMutatorStateFunction::SetMutatorStateFunction() : WidgetFunction(kFunctionSetMutatorState)
{
	mutatorKey[0] = 0;
	mutatorState = 0;
}

SetMutatorStateFunction::SetMutatorStateFunction(const SetMutatorStateFunction& setMutatorStateFunction) : WidgetFunction(setMutatorStateFunction)
{
	mutatorKey = setMutatorStateFunction.mutatorKey;
	mutatorState = setMutatorStateFunction.mutatorState;
}

SetMutatorStateFunction::~SetMutatorStateFunction()
{
}

Function *SetMutatorStateFunction::Replicate(void) const
{
	return (new SetMutatorStateFunction(*this));
}

void SetMutatorStateFunction::Pack(Packer& data, unsigned_int32 packFlags) const
{
	WidgetFunction::Pack(data, packFlags);

	PackHandle handle = data.BeginChunk('MKEY');
	data << mutatorKey;
	data.EndChunk(handle);

	data << ChunkHeader('STAT', 4);
	data << mutatorState;

	data << TerminatorChunk;
}

void SetMutatorStateFunction::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	WidgetFunction::Unpack(data, unpackFlags);
	UnpackChunkList<SetMutatorStateFunction>(data, unpackFlags);
}

bool SetMutatorStateFunction::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'MKEY':

			data >> mutatorKey;
			return (true);

		case 'STAT':

			data >> mutatorState;
			return (true);
	}

	return (false);
}

void SetMutatorStateFunction::Compress(Compressor& data) const
{
	WidgetFunction::Compress(data);

	data << mutatorState;
	data << mutatorKey;
}

bool SetMutatorStateFunction::Decompress(Decompressor& data)
{
	if (WidgetFunction::Decompress(data))
	{
		data >> mutatorState;
		data >> mutatorKey;
		return (true);
	}

	return (false);
}

int32 SetMutatorStateFunction::GetSettingCount(void) const
{
	return (WidgetFunction::GetSettingCount() + 3);
}

Setting *SetMutatorStateFunction::GetSetting(int32 index) const
{
	int32 count = WidgetFunction::GetSettingCount();
	if (index < count)
	{
		return (WidgetFunction::GetSetting(index));
	}

	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerPanel, 'FUNC', 'MUTR'));
		return (new TextSetting('MUTR', mutatorKey, title, kMaxMutatorKeyLength));
	}

	if (index == count + 1)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerPanel, kFunctionSetMutatorState, 'DSAB'));
		return (new BooleanSetting('DSAB', ((mutatorState & kMutatorDisabled) != 0), title));
	}

	if (index == count + 2)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerPanel, kFunctionSetMutatorState, 'RVRS'));
		return (new BooleanSetting('RVRS', ((mutatorState & kMutatorReverse) != 0), title));
	}

	return (nullptr);
}

void SetMutatorStateFunction::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'MUTR')
	{
		mutatorKey = static_cast<const TextSetting *>(setting)->GetText();
	}
	else if (identifier == 'DSAB')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			mutatorState |= kMutatorDisabled;
		}
		else
		{
			mutatorState &= ~kMutatorDisabled;
		}
	}
	else if (identifier == 'RVRS')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			mutatorState |= kMutatorReverse;
		}
		else
		{
			mutatorState &= ~kMutatorReverse;
		}
	}
	else
	{
		WidgetFunction::SetSetting(setting);
	}
}

bool SetMutatorStateFunction::OverridesFunction(const Function *function) const
{
	if (WidgetFunction::OverridesFunction(function))
	{
		const SetMutatorStateFunction *setMutatorStateFunction = static_cast<const SetMutatorStateFunction *>(function);
		return (setMutatorStateFunction->GetMutatorKey() == mutatorKey);
	}

	return (false);
}

void SetMutatorStateFunction::Execute(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	const Panel *root = static_cast<PanelController *>(controller)->GetRootWidget();
	const Widget *widget = root->FindWidget(GetWidgetKey());
	while (widget)
	{
		Mutator *mutator = widget->GetFirstMutator();
		while (mutator)
		{
			if (mutator->GetMutatorKey() == mutatorKey)
			{
				mutator->SetMutatorState(mutatorState);
			}

			mutator = mutator->Next();
		}

		widget = widget->GetNextWidgetWithSameKey();
	}

	CallCompletionProc();
}


GetWidgetColorFunction::GetWidgetColorFunction() : WidgetFunction(kFunctionGetWidgetColor)
{
	colorType = kWidgetColorDefault;
}

GetWidgetColorFunction::GetWidgetColorFunction(const GetWidgetColorFunction& getWidgetColorFunction) : WidgetFunction(getWidgetColorFunction)
{
	colorType = getWidgetColorFunction.colorType;
}

GetWidgetColorFunction::~GetWidgetColorFunction()
{
}

Function *GetWidgetColorFunction::Replicate(void) const
{
	return (new GetWidgetColorFunction(*this));
}

void GetWidgetColorFunction::Pack(Packer& data, unsigned_int32 packFlags) const
{
	WidgetFunction::Pack(data, packFlags);

	data << colorType;
}

void GetWidgetColorFunction::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	WidgetFunction::Unpack(data, unpackFlags);

	data >> colorType;
}

void GetWidgetColorFunction::Compress(Compressor& data) const
{
	WidgetFunction::Compress(data);

	data << colorType;
}

bool GetWidgetColorFunction::Decompress(Decompressor& data)
{
	if (WidgetFunction::Decompress(data))
	{
		data >> colorType;
		return (true);
	}

	return (false);
}

int32 GetWidgetColorFunction::GetSettingCount(void) const
{
	return (WidgetFunction::GetSettingCount() + 1);
}

Setting *GetWidgetColorFunction::GetSetting(int32 index) const
{
	int32 count = WidgetFunction::GetSettingCount();
	if (index < count)
	{
		return (WidgetFunction::GetSetting(index));
	}

	if (index == count)
	{
		int32 selection = 0;
		for (machine a = 1; a < Widget::kWidgetColorCount; a++)
		{
			if (colorType == Widget::widgetColorType[a])
			{
				selection = a;
				break;
			}
		}

		const StringTable *table = TheInterfaceMgr->GetStringTable();
		const char *title = table->GetString(StringID('WDGT', 'WDGT', 'CTYP'));
		MenuSetting *menu = new MenuSetting('CTYP', selection, title, Widget::kWidgetColorCount);
		menu->SetMenuItemString(0, table->GetString(StringID('WDGT', 'WDGT', 'CTYP', 'DFLT')));
		for (machine a = 1; a < Widget::kWidgetColorCount; a++)
		{
			menu->SetMenuItemString(a, table->GetString(StringID('WDGT', 'WDGT', 'CTYP', Widget::widgetColorType[a])));
		}

		return (menu);
	}

	return (nullptr);
}

void GetWidgetColorFunction::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'CTYP')
	{
		colorType = Widget::widgetColorType[static_cast<const MenuSetting *>(setting)->GetMenuSelection()];
	}
	else
	{
		WidgetFunction::SetSetting(setting);
	}
}

void GetWidgetColorFunction::Execute(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	const Panel *root = static_cast<PanelController *>(controller)->GetRootWidget();
	const Widget *widget = root->FindWidget(GetWidgetKey());
	if (widget)
	{
		method->SetOutputValue(state, widget->GetWidgetColor(colorType));
	}
	else
	{
		method->SetOutputValue(state, K::black);
	}

	CallCompletionProc();
}


SetWidgetColorFunction::SetWidgetColorFunction() : WidgetFunction(kFunctionSetWidgetColor)
{
	widgetColor.Set(1.0F, 1.0F, 1.0F, 1.0F);
	colorType = kWidgetColorDefault;
}

SetWidgetColorFunction::SetWidgetColorFunction(const SetWidgetColorFunction& setWidgetColorFunction) : WidgetFunction(setWidgetColorFunction)
{
	widgetColor = setWidgetColorFunction.widgetColor;
	colorType = setWidgetColorFunction.colorType;
}

SetWidgetColorFunction::~SetWidgetColorFunction()
{
}

Function *SetWidgetColorFunction::Replicate(void) const
{
	return (new SetWidgetColorFunction(*this));
}

void SetWidgetColorFunction::Pack(Packer& data, unsigned_int32 packFlags) const
{
	WidgetFunction::Pack(data, packFlags);

	data << widgetColor;
	data << colorType;
}

void SetWidgetColorFunction::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	WidgetFunction::Unpack(data, unpackFlags);

	data >> widgetColor;
	data >> colorType;
}

void SetWidgetColorFunction::Compress(Compressor& data) const
{
	WidgetFunction::Compress(data);

	data << widgetColor;
	data << colorType;
}

bool SetWidgetColorFunction::Decompress(Decompressor& data)
{
	if (WidgetFunction::Decompress(data))
	{
		data >> widgetColor;
		data >> colorType;
		return (true);
	}

	return (false);
}

int32 SetWidgetColorFunction::GetSettingCount(void) const
{
	return (WidgetFunction::GetSettingCount() + 2);
}

Setting *SetWidgetColorFunction::GetSetting(int32 index) const
{
	int32 count = WidgetFunction::GetSettingCount();
	if (index < count)
	{
		return (WidgetFunction::GetSetting(index));
	}

	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerPanel, kFunctionSetWidgetColor, 'COLR'));
		const char *picker = table->GetString(StringID('CTRL', kControllerPanel, kFunctionSetWidgetColor, 'PICK'));
		return (new ColorSetting('COLR', widgetColor, title, picker, kColorPickerAlpha));
	}

	if (index == count + 1)
	{
		int32 selection = 0;
		for (machine a = 1; a < Widget::kWidgetColorCount; a++)
		{
			if (colorType == Widget::widgetColorType[a])
			{
				selection = a;
				break;
			}
		}

		const char *title = table->GetString(StringID('WDGT', 'WDGT', 'CTYP'));
		MenuSetting *menu = new MenuSetting('CTYP', selection, title, Widget::kWidgetColorCount);
		menu->SetMenuItemString(0, table->GetString(StringID('WDGT', 'WDGT', 'CTYP', 'DFLT')));
		for (machine a = 1; a < Widget::kWidgetColorCount; a++)
		{
			menu->SetMenuItemString(a, table->GetString(StringID('WDGT', 'WDGT', 'CTYP', Widget::widgetColorType[a])));
		}

		return (menu);
	}

	return (nullptr);
}

void SetWidgetColorFunction::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'COLR')
	{
		widgetColor = static_cast<const ColorSetting *>(setting)->GetColor();
	}
	else if (identifier == 'CTYP')
	{
		colorType = Widget::widgetColorType[static_cast<const MenuSetting *>(setting)->GetMenuSelection()];
	}
	else
	{
		WidgetFunction::SetSetting(setting);
	}
}

bool SetWidgetColorFunction::OverridesFunction(const Function *function) const
{
	if (WidgetFunction::OverridesFunction(function))
	{
		const SetWidgetColorFunction *setWidgetColorFunction = static_cast<const SetWidgetColorFunction *>(function);
		return (setWidgetColorFunction->GetColorType() == colorType);
	}

	return (false);
}

void SetWidgetColorFunction::Execute(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	const Panel *root = static_cast<PanelController *>(controller)->GetRootWidget();
	Widget *widget = root->FindWidget(GetWidgetKey());
	while (widget)
	{
		widget->SetWidgetColor(widgetColor, colorType);
		widget = widget->GetNextWidgetWithSameKey();
	}

	CallCompletionProc();
}


GetWidgetImageFunction::GetWidgetImageFunction() : WidgetFunction(kFunctionGetWidgetImage)
{
}

GetWidgetImageFunction::GetWidgetImageFunction(const GetWidgetImageFunction& getWidgetImageFunction) : WidgetFunction(getWidgetImageFunction)
{
}

GetWidgetImageFunction::~GetWidgetImageFunction()
{
}

Function *GetWidgetImageFunction::Replicate(void) const
{
	return (new GetWidgetImageFunction(*this));
}

void GetWidgetImageFunction::Execute(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	const Panel *root = static_cast<PanelController *>(controller)->GetRootWidget();
	const Widget *widget = root->FindWidget(GetWidgetKey());
	if ((widget) && (widget->GetWidgetType() == kWidgetImage))
	{
		method->SetOutputValue(state, static_cast<const ImageWidget *>(widget)->GetTextureName());
	}
	else
	{
		method->SetOutputValue(state, "");
	}

	CallCompletionProc();
}


SetWidgetImageFunction::SetWidgetImageFunction() : WidgetFunction(kFunctionSetWidgetImage)
{
	textureName[0] = 0;
}

SetWidgetImageFunction::SetWidgetImageFunction(const SetWidgetImageFunction& setWidgetImageFunction) : WidgetFunction(setWidgetImageFunction)
{
	textureName = setWidgetImageFunction.textureName;
}

SetWidgetImageFunction::~SetWidgetImageFunction()
{
}

Function *SetWidgetImageFunction::Replicate(void) const
{
	return (new SetWidgetImageFunction(*this));
}

void SetWidgetImageFunction::Pack(Packer& data, unsigned_int32 packFlags) const
{
	WidgetFunction::Pack(data, packFlags);

	PackHandle handle = data.BeginChunk('TNAM');
	data << textureName;
	data.EndChunk(handle);

	data << TerminatorChunk;
}

void SetWidgetImageFunction::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	WidgetFunction::Unpack(data, unpackFlags);
	UnpackChunkList<SetWidgetImageFunction>(data, unpackFlags);
}

bool SetWidgetImageFunction::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'TNAM':

			data >> textureName;
			return (true);
	}

	return (false);
}

void SetWidgetImageFunction::Compress(Compressor& data) const
{
	WidgetFunction::Compress(data);

	data << textureName;
}

bool SetWidgetImageFunction::Decompress(Decompressor& data)
{
	if (WidgetFunction::Decompress(data))
	{
		data >> textureName;
		return (true);
	}

	return (false);
}

int32 SetWidgetImageFunction::GetSettingCount(void) const
{
	return (WidgetFunction::GetSettingCount() + 1);
}

Setting *SetWidgetImageFunction::GetSetting(int32 index) const
{
	int32 count = WidgetFunction::GetSettingCount();
	if (index < count)
	{
		return (WidgetFunction::GetSetting(index));
	}

	if (index == count)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();
		const char *title = table->GetString(StringID('CTRL', kControllerPanel, kFunctionSetWidgetImage, 'TNAM'));
		const char *picker = table->GetString(StringID('CTRL', kControllerPanel, kFunctionSetWidgetImage, 'TPCK'));
		return (new ResourceSetting('TNAM', textureName, title, picker, TextureResource::GetDescriptor()));
	}

	return (nullptr);
}

void SetWidgetImageFunction::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'TNAM')
	{
		textureName = static_cast<const ResourceSetting *>(setting)->GetResourceName();
	}
	else
	{
		WidgetFunction::SetSetting(setting);
	}
}

void SetWidgetImageFunction::Execute(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	const Panel *root = static_cast<PanelController *>(controller)->GetRootWidget();
	Widget *widget = root->FindWidget(GetWidgetKey());
	while (widget)
	{
		if (widget->GetWidgetType() == kWidgetImage)
		{
			static_cast<ImageWidget *>(widget)->SetTexture(0, textureName);
		}

		widget = widget->GetNextWidgetWithSameKey();
	}

	CallCompletionProc();
}


GetWidgetTextFunction::GetWidgetTextFunction() : WidgetFunction(kFunctionGetWidgetText)
{
}

GetWidgetTextFunction::GetWidgetTextFunction(const GetWidgetTextFunction& getWidgetTextFunction) : WidgetFunction(getWidgetTextFunction)
{
}

GetWidgetTextFunction::~GetWidgetTextFunction()
{
}

Function *GetWidgetTextFunction::Replicate(void) const
{
	return (new GetWidgetTextFunction(*this));
}

void GetWidgetTextFunction::Execute(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	const char *text = "";

	const Panel *root = static_cast<PanelController *>(controller)->GetRootWidget();
	const Widget *widget = root->FindWidget(GetWidgetKey());
	if (widget)
	{
		WidgetType type = widget->GetWidgetType();
		if ((type == kWidgetText) || (type == kWidgetEditText))
		{
			text = static_cast<const TextWidget *>(widget)->GetText();
		}
	}

	method->SetOutputValue(state, text);
	CallCompletionProc();
}


SetWidgetTextFunction::SetWidgetTextFunction() : WidgetFunction(kFunctionSetWidgetText)
{
	textCombineMode = kSetWidgetTextReplace;
	maxResultLength = 0;
	widgetText[0] = 0;
}

SetWidgetTextFunction::SetWidgetTextFunction(const SetWidgetTextFunction& setWidgetTextFunction) : WidgetFunction(setWidgetTextFunction)
{
	textCombineMode = setWidgetTextFunction.textCombineMode;
	maxResultLength = setWidgetTextFunction.maxResultLength;
	widgetText = setWidgetTextFunction.widgetText;
}

SetWidgetTextFunction::~SetWidgetTextFunction()
{
}

Function *SetWidgetTextFunction::Replicate(void) const
{
	return (new SetWidgetTextFunction(*this));
}

void SetWidgetTextFunction::Pack(Packer& data, unsigned_int32 packFlags) const
{
	WidgetFunction::Pack(data, packFlags);

	data << ChunkHeader('MODE', 8);
	data << textCombineMode;
	data << maxResultLength;

	PackHandle handle = data.BeginChunk('TEXT');
	data << widgetText;
	data.EndChunk(handle);

	data << TerminatorChunk;
}

void SetWidgetTextFunction::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	WidgetFunction::Unpack(data, unpackFlags);
	UnpackChunkList<SetWidgetTextFunction>(data, unpackFlags);
}

bool SetWidgetTextFunction::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'MODE':

			data >> textCombineMode;
			data >> maxResultLength;
			return (true);

		case 'TEXT':

			data >> widgetText;
			return (true);
	}

	return (false);
}

void SetWidgetTextFunction::Compress(Compressor& data) const
{
	WidgetFunction::Compress(data);

	data << textCombineMode;
	data << (int16) maxResultLength;
	data << widgetText;
}

bool SetWidgetTextFunction::Decompress(Decompressor& data)
{
	if (WidgetFunction::Decompress(data))
	{
		int16	length;

		data >> textCombineMode;
		data >> length;
		data >> widgetText;

		maxResultLength = length;
		return (length >= 0);
	}

	return (false);
}

int32 SetWidgetTextFunction::GetSettingCount(void) const
{
	return (WidgetFunction::GetSettingCount() + 3);
}

Setting *SetWidgetTextFunction::GetSetting(int32 index) const
{
	int32 count = WidgetFunction::GetSettingCount();
	if (index < count)
	{
		return (WidgetFunction::GetSetting(index));
	}

	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerPanel, kFunctionSetWidgetText, 'TEXT'));
		return (new TextSetting('TEXT', widgetText, title, kMaxFunctionStringLength));
	}

	if (index == count + 1)
	{
		int32 selection = 0;
		if (textCombineMode == kSetWidgetTextAppend)
		{
			selection = 1;
		}
		else if (textCombineMode == kSetWidgetTextPrepend)
		{
			selection = 2;
		}

		const char *title = table->GetString(StringID('CTRL', kControllerPanel, kFunctionSetWidgetText, 'MODE'));
		MenuSetting *menu = new MenuSetting('MODE', selection, title, 3);

		menu->SetMenuItemString(0, table->GetString(StringID('CTRL', kControllerPanel, kFunctionSetWidgetText, 'MODE', kSetWidgetTextReplace)));
		menu->SetMenuItemString(1, table->GetString(StringID('CTRL', kControllerPanel, kFunctionSetWidgetText, 'MODE', kSetWidgetTextAppend)));
		menu->SetMenuItemString(2, table->GetString(StringID('CTRL', kControllerPanel, kFunctionSetWidgetText, 'MODE', kSetWidgetTextPrepend)));

		return (menu);
	}

	if (index == count + 2)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerPanel, kFunctionSetWidgetText, 'LENG'));
		return (new TextSetting('LENG', Text::IntegerToString(maxResultLength), title, 3, &EditTextWidget::NumberFilter));
	}

	return (nullptr);
}

void SetWidgetTextFunction::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'TEXT')
	{
		widgetText = static_cast<const TextSetting *>(setting)->GetText();
	}
	else if (identifier == 'MODE')
	{
		static const Type combineMode[7] =
		{
			kSetWidgetTextReplace, kSetWidgetTextAppend, kSetWidgetTextPrepend
		};

		textCombineMode = combineMode[static_cast<const MenuSetting *>(setting)->GetMenuSelection()];
	}
	else if (identifier == 'LENG')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		maxResultLength = MaxZero(Text::StringToInteger(text));
	}
	else
	{
		WidgetFunction::SetSetting(setting);
	}
}

bool SetWidgetTextFunction::OverridesFunction(const Function *function) const
{
	return ((WidgetFunction::OverridesFunction(function)) && (textCombineMode == kSetWidgetTextReplace));
}

void SetWidgetTextFunction::Execute(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	const Panel *root = static_cast<PanelController *>(controller)->GetRootWidget();
	Widget *widget = root->FindWidget(GetWidgetKey());
	while (widget)
	{
		WidgetType type = widget->GetWidgetType();
		if ((type == kWidgetText) || (type == kWidgetEditText))
		{
			TextWidget *textWidget = static_cast<TextWidget *>(widget);

			if (textCombineMode == kSetWidgetTextAppend)
			{
				textWidget->AppendText(widgetText, maxResultLength);
			}
			else if (textCombineMode == kSetWidgetTextPrepend)
			{
				textWidget->PrependText(widgetText, maxResultLength);
			}
			else
			{
				textWidget->SetText(widgetText);
			}
		}

		widget = widget->GetNextWidgetWithSameKey();
	}

	CallCompletionProc();
}


GetWidgetValueFunction::GetWidgetValueFunction() : WidgetFunction(kFunctionGetWidgetValue)
{
}

GetWidgetValueFunction::GetWidgetValueFunction(const GetWidgetValueFunction& getWidgetValueFunction) : WidgetFunction(getWidgetValueFunction)
{
}

GetWidgetValueFunction::~GetWidgetValueFunction()
{
}

Function *GetWidgetValueFunction::Replicate(void) const
{
	return (new GetWidgetValueFunction(*this));
}

void GetWidgetValueFunction::Execute(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	int32 value = 0;

	const Panel *root = static_cast<PanelController *>(controller)->GetRootWidget();
	const Widget *widget = root->FindWidget(GetWidgetKey());
	if (widget)
	{
		switch (widget->GetWidgetType())
		{
			case kWidgetCheck:

				value = static_cast<const CheckWidget *>(widget)->GetValue();
				break;

			case kWidgetRadio:

				value = static_cast<const RadioWidget *>(widget)->GetValue();
				break;

			case kWidgetProgress:

				value = static_cast<const ProgressWidget *>(widget)->GetValue();
				break;

			case kWidgetSlider:

				value = static_cast<const SliderWidget *>(widget)->GetValue();
				break;

			case kWidgetScroll:

				value = static_cast<const ScrollWidget *>(widget)->GetValue();
				break;

			case kWidgetList:

				value = static_cast<const ListWidget *>(widget)->GetFirstSelectedIndex();
				break;

			case kWidgetMultipane:

				value = static_cast<const MultipaneWidget *>(widget)->GetSelection();
				break;

			case kWidgetPopupMenu:

				value = static_cast<const PopupMenuWidget *>(widget)->GetSelection();
				break;
		}
	}

	method->SetOutputValue(state, value);
	CallCompletionProc();
}


SetWidgetValueFunction::SetWidgetValueFunction() : WidgetFunction(kFunctionSetWidgetValue)
{
	widgetValue = 0;
}

SetWidgetValueFunction::SetWidgetValueFunction(const SetWidgetValueFunction& setWidgetValueFunction) : WidgetFunction(setWidgetValueFunction)
{
	widgetValue = setWidgetValueFunction.widgetValue;
}

SetWidgetValueFunction::~SetWidgetValueFunction()
{
}

Function *SetWidgetValueFunction::Replicate(void) const
{
	return (new SetWidgetValueFunction(*this));
}

void SetWidgetValueFunction::Pack(Packer& data, unsigned_int32 packFlags) const
{
	WidgetFunction::Pack(data, packFlags);

	data << ChunkHeader('VALU', 4);
	data << widgetValue;

	data << TerminatorChunk;
}

void SetWidgetValueFunction::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	WidgetFunction::Unpack(data, unpackFlags);
	UnpackChunkList<SetWidgetValueFunction>(data, unpackFlags);
}

bool SetWidgetValueFunction::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'VALU':

			data >> widgetValue;
			return (true);
	}

	return (false);
}

void SetWidgetValueFunction::Compress(Compressor& data) const
{
	WidgetFunction::Compress(data);

	data << widgetValue;
}

bool SetWidgetValueFunction::Decompress(Decompressor& data)
{
	if (WidgetFunction::Decompress(data))
	{
		data >> widgetValue;
		return (true);
	}

	return (false);
}

int32 SetWidgetValueFunction::GetSettingCount(void) const
{
	return (WidgetFunction::GetSettingCount() + 1);
}

Setting *SetWidgetValueFunction::GetSetting(int32 index) const
{
	int32 count = WidgetFunction::GetSettingCount();
	if (index < count)
	{
		return (WidgetFunction::GetSetting(index));
	}

	if (index == count)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();
		const char *title = table->GetString(StringID('CTRL', kControllerPanel, kFunctionSetWidgetValue, 'VALU'));
		return (new TextSetting('VALU', Text::IntegerToString(widgetValue), title, 11, &EditTextWidget::NumberFilter));
	}

	return (nullptr);
}

void SetWidgetValueFunction::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'VALU')
	{
		widgetValue = Text::StringToInteger(static_cast<const TextSetting *>(setting)->GetText());
	}
	else
	{
		WidgetFunction::SetSetting(setting);
	}
}

void SetWidgetValueFunction::Execute(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	const Panel *root = static_cast<PanelController *>(controller)->GetRootWidget();
	Widget *widget = root->FindWidget(GetWidgetKey());
	while (widget)
	{
		switch (widget->GetWidgetType())
		{
			case kWidgetCheck:

				static_cast<CheckWidget *>(widget)->SetValue(widgetValue, true);
				break;

			case kWidgetRadio:

				static_cast<RadioWidget *>(widget)->SetValue(widgetValue, true);
				break;

			case kWidgetProgress:

				static_cast<ProgressWidget *>(widget)->SetValue(widgetValue, true);
				break;

			case kWidgetSlider:

				static_cast<SliderWidget *>(widget)->SetValue(widgetValue, true);
				break;

			case kWidgetScroll:

				static_cast<ScrollWidget *>(widget)->SetValue(widgetValue, true);
				break;

			case kWidgetList:

				static_cast<ListWidget *>(widget)->SelectListItem(widgetValue, true);
				break;

			case kWidgetMultipane:

				static_cast<MultipaneWidget *>(widget)->SetSelection(widgetValue, true);
				break;

			case kWidgetPopupMenu:

				static_cast<PopupMenuWidget *>(widget)->SetSelection(widgetValue, true);
				break;
		}

		widget = widget->GetNextWidgetWithSameKey();
	}

	CallCompletionProc();
}


SetCameraConnectorKeyFunction::SetCameraConnectorKeyFunction() : WidgetFunction(kFunctionSetCameraConnectorKey)
{
	cameraConnectorKey[0] = 0;
}

SetCameraConnectorKeyFunction::SetCameraConnectorKeyFunction(const SetCameraConnectorKeyFunction& setCameraConnectorKeyFunction) : WidgetFunction(setCameraConnectorKeyFunction)
{
	cameraConnectorKey = setCameraConnectorKeyFunction.cameraConnectorKey;
}

SetCameraConnectorKeyFunction::~SetCameraConnectorKeyFunction()
{
}

Function *SetCameraConnectorKeyFunction::Replicate(void) const
{
	return (new SetCameraConnectorKeyFunction(*this));
}

void SetCameraConnectorKeyFunction::Pack(Packer& data, unsigned_int32 packFlags) const
{
	WidgetFunction::Pack(data, packFlags);

	data << cameraConnectorKey;
}

void SetCameraConnectorKeyFunction::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	WidgetFunction::Unpack(data, unpackFlags);

	data >> cameraConnectorKey;
}

void SetCameraConnectorKeyFunction::Compress(Compressor& data) const
{
	WidgetFunction::Compress(data);

	data << cameraConnectorKey;
}

bool SetCameraConnectorKeyFunction::Decompress(Decompressor& data)
{
	if (WidgetFunction::Decompress(data))
	{
		data >> cameraConnectorKey;
		return (true);
	}

	return (false);
}

int32 SetCameraConnectorKeyFunction::GetSettingCount(void) const
{
	return (WidgetFunction::GetSettingCount() + 1);
}

Setting *SetCameraConnectorKeyFunction::GetSetting(int32 index) const
{
	int32 count = WidgetFunction::GetSettingCount();
	if (index < count)
	{
		return (WidgetFunction::GetSetting(index));
	}

	if (index == count)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();
		const char *title = table->GetString(StringID('CTRL', kControllerPanel, kFunctionSetCameraConnectorKey, 'CCON'));
		return (new TextSetting('CCON', cameraConnectorKey, title, kMaxConnectorKeyLength, &Connector::ConnectorKeyFilter));
	}

	return (nullptr);
}

void SetCameraConnectorKeyFunction::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'CCON')
	{
		cameraConnectorKey = static_cast<const TextSetting *>(setting)->GetText();
	}
	else
	{
		WidgetFunction::SetSetting(setting);
	}
}

void SetCameraConnectorKeyFunction::Execute(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	const Panel *root = static_cast<PanelController *>(controller)->GetRootWidget();
	Widget *widget = root->FindWidget(GetWidgetKey());
	while (widget)
	{
		if (widget->GetWidgetType() == kWidgetCamera)
		{
			static_cast<CameraWidget *>(widget)->SetCameraConnectorKey(cameraConnectorKey);
		}

		widget = widget->GetNextWidgetWithSameKey();
	}

	CallCompletionProc();
}


PanelEffectObject::PanelEffectObject() : EffectObject(kEffectPanel)
{
	interactionPadding = 0.0F;

	impostorName[0] = 0;
	impostorMinDistance = 10.0F;
	impostorMaxDistance = 20.0F;
}

PanelEffectObject::PanelEffectObject(const Vector2D& size) : EffectObject(kEffectPanel)
{
	panelFlags = kPanelDepthOffset;

	externalPanelSize = size;
	interactionPadding = 0.0F;

	impostorName[0] = 0;
	impostorMinDistance = 10.0F;
	impostorMaxDistance = 20.0F;
}

PanelEffectObject::~PanelEffectObject()
{
}

void PanelEffectObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	EffectObject::Pack(data, packFlags);

	data << ChunkHeader('FLAG', 4);
	data << panelFlags;

	data << ChunkHeader('SIZE', sizeof(Vector2D));
	data << externalPanelSize;

	data << ChunkHeader('PADD', 4);
	data << interactionPadding;

	if (impostorName[0] != 0)
	{
		PackHandle handle = data.BeginChunk('IMPN');
		data << impostorName;
		data.EndChunk(handle);

		data << ChunkHeader('IMPD', 8);
		data << impostorMinDistance;
		data << impostorMaxDistance;
	}

	data << TerminatorChunk;
}

void PanelEffectObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	EffectObject::Unpack(data, unpackFlags);
	UnpackChunkList<PanelEffectObject>(data, unpackFlags);
}

bool PanelEffectObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> panelFlags;
			return (true);

		case 'SIZE':

			data >> externalPanelSize;
			return (true);

		case 'PADD':

			data >> interactionPadding;
			return (true);

		case 'IMPN':

			data >> impostorName;
			return (true);

		case 'IMPD':

			data >> impostorMinDistance;
			data >> impostorMaxDistance;
			return (true);
	}

	return (false);
}

void *PanelEffectObject::BeginSettingsUnpack(void)
{
	impostorName[0] = 0;
	return (EffectObject::BeginSettingsUnpack());
}

int32 PanelEffectObject::GetCategoryCount(void) const
{
	return (1);
}

Type PanelEffectObject::GetCategoryType(int32 index, const char **title) const
{
	if (index == 0)
	{
		*title = TheInterfaceMgr->GetStringTable()->GetString(StringID(kEffectPanel));
		return (kEffectPanel);
	}

	return (0);
}

int32 PanelEffectObject::GetCategorySettingCount(Type category) const
{
	if (category == kEffectPanel)
	{
		return (8);
	}

	return (0);
}

Setting *PanelEffectObject::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kEffectPanel)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();

		if (index == 0)
		{
			const char *title = table->GetString(StringID(kEffectPanel, 'PANL'));
			return (new HeadingSetting('PANL', title));
		}

		if (index == 1)
		{
			const char *title = table->GetString(StringID(kEffectPanel, 'PANL', 'DOFF'));
			return (new BooleanSetting('DOFF', ((panelFlags & kPanelDepthOffset) != 0), title));
		}

		if (index == 2)
		{
			const char *title = table->GetString(StringID(kEffectPanel, 'PANL', 'TSID'));
			return (new BooleanSetting('TSID', ((panelFlags & kPanelTwoSided) != 0), title));
		}

		if (index == 3)
		{
			const char *title = table->GetString(StringID(kEffectPanel, 'PANL', 'RFOG'));
			return (new BooleanSetting('RFOG', ((panelFlags & kPanelRenderFog) != 0), title));
		}

		if (index == 4)
		{
			const char *title = table->GetString(StringID(kEffectPanel, 'PANL', 'PADD'));
			return (new TextSetting('PADD', interactionPadding, title));
		}

		if (index == 5)
		{
			const char *title = table->GetString(StringID(kEffectPanel, 'PANL', 'IPST'));
			const char *picker = table->GetString(StringID(kEffectPanel, 'PANL', 'PICK'));
			return (new ResourceSetting('IPST', impostorName, title, picker, TextureResource::GetDescriptor()));
		}

		if (index == 6)
		{
			const char *title = table->GetString(StringID(kEffectPanel, 'PANL', 'IMIN'));
			return (new TextSetting('IMIN', impostorMinDistance, title));
		}

		if (index == 7)
		{
			const char *title = table->GetString(StringID(kEffectPanel, 'PANL', 'IMAX'));
			return (new TextSetting('IMAX', impostorMaxDistance, title));
		}
	}

	return (nullptr);
}

void PanelEffectObject::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kEffectPanel)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'DOFF')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				panelFlags |= kPanelDepthOffset;
			}
			else
			{
				panelFlags &= ~kPanelDepthOffset;
			}
		}
		else if (identifier == 'TSID')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				panelFlags |= kPanelTwoSided;
			}
			else
			{
				panelFlags &= ~kPanelTwoSided;
			}
		}
		else if (identifier == 'RFOG')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				panelFlags |= kPanelRenderFog;
			}
			else
			{
				panelFlags &= ~kPanelRenderFog;
			}
		}
		else if (identifier == 'PADD')
		{
			interactionPadding = FmaxZero(Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText()));
		}
		else if (identifier == 'IPST')
		{
			impostorName = static_cast<const ResourceSetting *>(setting)->GetResourceName();
		}
		else if (identifier == 'IMIN')
		{
			impostorMinDistance = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
		}
		else if (identifier == 'IMAX')
		{
			impostorMaxDistance = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
		}
	}
}

int32 PanelEffectObject::GetObjectSize(float *size) const
{
	size[0] = externalPanelSize.x;
	size[1] = externalPanelSize.y;
	return (2);
}

void PanelEffectObject::SetObjectSize(const float *size)
{
	externalPanelSize.x = size[0];
	externalPanelSize.y = size[1];
}

bool PanelEffectObject::DetectCollision(const Point3D& p1, const Point3D& p2, float padding, CollisionPoint *collisionPoint) const
{
	float z1 = p1.z;
	float z2 = p2.z;
	if ((z1 > 0.0F) && (z2 < 0.0F))
	{
		float dz = z2 - z1;
		float t = -z1 / dz;
		if ((t > 0.0F) && (t < 1.0F))
		{
			float x = p1.x + (p2.x - p1.x) * t;
			float y = p1.y + (p2.y - p1.y) * t;
			if ((x > -padding) && (x < externalPanelSize.x + padding) && (y > -padding) && (y < externalPanelSize.y + padding))
			{
				collisionPoint->param = t;
				return (true);
			}
		}
	}

	return (false);
}


PanelEffect::PanelEffect() :
		Effect(kEffectPanel, kRenderTriangleStrip),
		impostorWidget(Zero2D),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		diffuseAttribute(kAttributeMutable)
{
	panelInvisibleTime = 0;
	queryThreadFlags = 0;
	panelCursor = nullptr;
}

PanelEffect::PanelEffect(const Vector2D& size) :
		Effect(kEffectPanel, kRenderTriangleStrip),
		impostorWidget(Zero2D),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		diffuseAttribute(kAttributeMutable)
{
	SetNewObject(new PanelEffectObject(size));
	SetController(new PanelController);

	panelInvisibleTime = 0;
	queryThreadFlags = 0;
	panelCursor = nullptr;
}

PanelEffect::PanelEffect(const PanelEffect& panelEffect) :
		Effect(panelEffect),
		impostorWidget(Zero2D),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		diffuseAttribute(kAttributeMutable)
{
	panelInvisibleTime = 0;
	queryThreadFlags = 0;
	panelCursor = nullptr;
}

PanelEffect::~PanelEffect()
{
}

Node *PanelEffect::Replicate(void) const
{
	return (new PanelEffect(*this));
}

void PanelEffect::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Effect::Pack(data, packFlags);

	data << ChunkHeader('TIME', 4);
	data << panelInvisibleTime;

	data << TerminatorChunk;
}

void PanelEffect::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Effect::Unpack(data, unpackFlags);
	UnpackChunkList<PanelEffect>(data, unpackFlags);
}

bool PanelEffect::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'TIME':
		{
			data >> panelInvisibleTime;
			return (true);
		}
	}

	return (false);
}

void PanelEffect::HandleTransformUpdate(void)
{
	Effect::HandleTransformUpdate();

	const Transform4D& m = GetWorldTransform();
	const Vector2D& size = GetObject()->GetExternalPanelSize();

	Vector3D x = m[0] * panelScale.x;
	Vector3D y = m[1] * -panelScale.y;
	Point3D position = GetWorldPosition() + m[1] * size.y;

	Controller *controller = GetController();
	if ((controller) && (controller->GetControllerType() == kControllerPanel))
	{
		Panel *rootWidget = static_cast<PanelController *>(controller)->GetRootWidget();
		rootWidget->SetRenderTransform(x, y, m[2], position);
		rootWidget->Invalidate();
	}

	impostorWidget.SetWidgetTransform(m[0], -m[1], m[2], position);
	impostorWidget.Invalidate();
}

bool PanelEffect::CalculateBoundingBox(Box3D *box) const
{
	box->min.Set(0.0F, 0.0F, 0.0F);
	box->max.Set(GetObject()->GetExternalPanelSize(), 0.0F);
	return (true);
}

bool PanelEffect::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	const PanelEffectObject *object = GetObject();
	const Vector2D& size = object->GetExternalPanelSize();
	float padding = object->GetInteractionPadding();

	float x = size.x * 0.5F;
	float y = size.y * 0.5F;
	sphere->SetCenter(x, y, 0.0F);

	x += padding;
	y += padding;
	sphere->SetRadius(Sqrt(x * x + y * y));

	return (true);
}

void PanelEffect::Preprocess(void)
{
	Effect::Preprocess();

	SetShaderFlags(kShaderAmbientEffect | kShaderAlphaFogFraction);
	SetDepthOffset(0.0078125F, &GetBoundingSphere()->GetCenter());

	SetVertexCount(4);
	SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(Point2D));
	SetVertexAttributeArray(kArrayPosition, 0, 2);
	vertexBuffer.Establish(sizeof(Point2D) * 4);

	volatile Point2D *restrict vertex = vertexBuffer.BeginUpdateSync<Point2D>();

	const Vector2D& size = GetObject()->GetExternalPanelSize();
	vertex[0].Set(0.0F, 0.0F);
	vertex[1].Set(size.x, 0.0F);
	vertex[2].Set(0.0F, size.y);
	vertex[3].Set(size.x, size.y);

	vertexBuffer.EndUpdateSync();

	diffuseAttribute.SetDiffuseColor(K::transparent);
	attributeList.Append(&diffuseAttribute);
	SetMaterialAttributeList(&attributeList);

	SetAmbientBlendState(BlendState(kBlendOne, kBlendInvSourceAlpha, kBlendZero, kBlendOne));
	SetTransformable(this);

	impostorWidget.SetWidgetSize(size);
	impostorWidget.Preprocess();

	PanelEffect::ProcessObjectSettings();
}

void PanelEffect::ProcessObjectSettings(void)
{
	const PanelEffectObject *object = GetObject();
	unsigned_int32 flags = object->GetPanelFlags();

	unsigned_int32 state = kRenderDepthTest | kRenderDepthInhibit;
	if (flags & kPanelDepthOffset)
	{
		state |= kRenderDepthOffset;
	}

	renderState = state;
	materialState = (flags & kPanelTwoSided) ? kMaterialTwoSided : 0;

	if (flags & kPanelRenderFog)
	{
		SetRenderState(renderState);
		GetFirstRenderSegment()->SetMaterialState(materialState);
	}

	const char *impostorName = object->GetImpostorName();
	if (impostorName[0] != 0)
	{
		impostorWidget.SetTexture(0, impostorName);
	}
	else
	{
		impostorWidget.SetTexture(0, nullptr, nullptr);
	}
}

void PanelEffect::Render(const FrustumCamera *camera, List<Renderable> *effectList)
{
	const Transform4D& m = GetInverseWorldTransform();
	const Point3D& p = camera->GetWorldPosition();
	if ((materialState & kMaterialTwoSided) || ((m.GetRow(2) ^ p) > 0.0F))
	{
		const PanelEffectObject *object = GetObject();

		Controller *controller = GetController();
		if ((controller) && (controller->GetControllerType() == kControllerPanel))
		{
			PanelController *panelController = static_cast<PanelController *>(controller);

			bool renderPanel = true;
			bool renderImpostor = false;

			if (impostorWidget.GetTexture())
			{
				float dmin = object->GetImpostorMinDistance();
				float dmax = object->GetImpostorMaxDistance();

				float d = Magnitude(p - GetBoundingSphere()->GetCenter());
				if (d > dmin)
				{
					renderImpostor = true;
					if (d < dmax)
					{
						impostorWidget.SetDynamicWidgetAlpha((d - dmin) / (dmax - dmin), kWidgetColorDefault);
					}
					else
					{
						renderPanel = false;
						impostorWidget.SetDynamicWidgetAlpha(1.0F, kWidgetColorDefault);
					}
				}
			}

			Renderable *opaqueRenderable = effectList[kEffectListOpaque].Last();

			Panel *rootWidget = panelController->GetRootWidget();
			if (renderPanel)
			{
				rootWidget->Update();
				rootWidget->RenderTree(&effectList[kEffectListOpaque]);
			}

			if (renderImpostor)
			{
				impostorWidget.Update();
				impostorWidget.RenderTree(&effectList[kEffectListOpaque]);
			}

			if (panelCursor)
			{
				const Transform4D& transform = rootWidget->GetRenderTransform();
				panelCursor->SetWorldTransform(transform[0], transform[1], transform[2], transform * cursorPosition);

				Renderable *velocityRenderable = effectList[kEffectListVelocity].Last();
				effectList[kEffectListVelocity].Append(panelCursor);

				const Vector2D& cursorOffset = panelCursor->GetCursorOffset();
				const Vector2D& cursorSize = panelCursor->GetCursorSize();
				Point3D minBounds(cursorPosition.x + cursorOffset.x, cursorPosition.y + cursorOffset.y, cursorPosition.z);
				Point3D maxBounds(minBounds.x + cursorSize.x, minBounds.y + cursorSize.y, cursorPosition.z);
				cursorBox.Set(minBounds, maxBounds);
				panelCursor->SetMotionBlurBox(&cursorBox);

				velocityRenderable = (velocityRenderable) ? velocityRenderable->Next() : effectList[kEffectListVelocity].First();
				while (velocityRenderable)
				{
					velocityRenderable->SetRenderState(renderState);
					velocityRenderable->SetDepthOffset(0.0078125F, &GetBoundingSphere()->GetCenter());
					velocityRenderable->GetFirstRenderSegment()->SetMaterialState(materialState | kMaterialAlphaTest);

					velocityRenderable = velocityRenderable->Next();
				}
			}

			opaqueRenderable = (opaqueRenderable) ? opaqueRenderable->Next() : effectList[kEffectListOpaque].First();
			while (opaqueRenderable)
			{
				opaqueRenderable->SetRenderState(renderState);
				opaqueRenderable->SetDepthOffset(0.0078125F, &GetBoundingSphere()->GetCenter());
				opaqueRenderable->GetFirstRenderSegment()->SetMaterialState(materialState);

				opaqueRenderable = opaqueRenderable->Next();
			}
		}

		if (object->GetPanelFlags() & kPanelRenderFog)
		{
			effectList[kEffectListCover].Append(this);
		}

		panelInvisibleTime = MinZero(panelInvisibleTime);
	}
}

bool PanelEffect::DetectCollision(const Point3D& p1, const Point3D& p2, CollisionPoint *collisionPoint) const
{
	const PanelEffectObject *object = GetObject();

	const Controller *controller = GetController();
	if ((controller) && (controller->GetControllerType() == kControllerPanel))
	{
		if (static_cast<const PanelController *>(controller)->GetPanelState() & kPanelEngaged)
		{
			return (object->DetectCollision(p1, p2, object->GetInteractionPadding(), collisionPoint));
		}
	}

	return (object->DetectCollision(p1, p2, 0.0F, collisionPoint));
}

// ZYUQURM
