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


#include "C4ModelViewer.h"
#include "C4WorldEditor.h"
#include "C4World.h"
#include "C4ConfigData.h"
#include "C4ModelManipulators.h"
#include "C4GeometryManipulators.h"


using namespace C4;


namespace
{
	enum
	{
		kFreeCameraForward		= 1 << 0,
		kFreeCameraBackward		= 1 << 1,
		kFreeCameraLeft			= 1 << 2,
		kFreeCameraRight		= 1 << 3,
		kFreeCameraUp			= 1 << 4,
		kFreeCameraDown			= 1 << 5
	};
}


List<ModelWindow> ModelWindow::windowList;


LimitWidget::LimitWidget(const Vector2D& size) :
		RenderableWidget(kWidgetLimit, kRenderQuads, size),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
{
	limitValue = 0;
	maxLimitValue = 1;

	minLimitPosition = 0;
	maxLimitPosition = 1;

	SetWidgetColor(ColorRGBA(0.25F, 0.25F, 0.25F, 1.0F));
}

LimitWidget::~LimitWidget()
{
}

void LimitWidget::SetValue(int32 value)
{
	value = Min(Max(value, minLimitPosition), maxLimitPosition);
	if (limitValue != value)
	{
		limitValue = value;
		SetBuildFlag();
	}
}

void LimitWidget::SetMaxValue(int32 maxValue)
{
	maxValue = MaxZero(maxValue);
	if (maxLimitValue != maxValue)
	{
		maxLimitValue = maxValue;
		SetBuildFlag();
	}
}

float LimitWidget::GetIndicatorPosition(void) const
{
	return (PositiveFloor((GetWidgetSize().x - 8.0F) * (float) Min(MaxZero(limitValue), maxLimitValue) / (float) Max(maxLimitValue, 1)) + 4.0F);
}

int32 LimitWidget::GetPositionValue(float x) const
{
	int32 value = (int32) ((x - 4.0F) * (float) maxLimitValue / (GetWidgetSize().x - 8.0F) + 0.5F);
	return (Max(Min(value, maxLimitPosition), minLimitPosition));
}

WidgetPart LimitWidget::TestPosition(const Point3D& position) const
{
	if (position.y >= GetWidgetSize().y - 12.0F)
	{
		float x = GetIndicatorPosition();
		if ((position.x >= x - 6.0F) && (position.x < x + 6.0F))
		{
			return (kWidgetPartIndicator);
		}
	}

	return (kWidgetPartNone);
}

void LimitWidget::Preprocess(void)
{
	RenderableWidget::Preprocess();

	SetVertexCount(8);
	SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(LimitVertex));
	SetVertexAttributeArray(kArrayPosition, 0, 2);
	SetVertexAttributeArray(kArrayColor, sizeof(Point2D), 4);
	SetVertexAttributeArray(kArrayTexcoord, sizeof(Point2D) + sizeof(ColorRGBA), 2);
	vertexBuffer.Establish(sizeof(LimitVertex) * 8);
 
	SetMaterialObjectPointer(TheInterfaceMgr->GetMaterialObjectPointer());
}
 
void LimitWidget::Build(void)
{ 
	volatile LimitVertex *restrict vertex = vertexBuffer.BeginUpdate<LimitVertex>();

	float x = GetIndicatorPosition(); 
	float h = GetWidgetSize().y;
 
	vertex[0].position.Set(x - 1.0F, 0.0F); 
	vertex[1].position.Set(x - 1.0F, h - 6.0F);
	vertex[2].position.Set(x + 1.0F, h - 6.0F);
	vertex[3].position.Set(x + 1.0F, 0.0F);
 
	vertex[4].position.Set(x - 6.0F, h - 12.0F);
	vertex[5].position.Set(x - 6.0F, h);
	vertex[6].position.Set(x + 6.0F, h);
	vertex[7].position.Set(x + 6.0F, h - 12.0F);

	const ColorRGBA& color = GetWidgetColor();
	for (machine a = 0; a < 4; a++)
	{
		vertex[a].color = color;
	}

	float alpha = (GetGlobalWidgetState() & kWidgetDisabled) ? 0.5F : 1.0F;
	ColorRGB rgb = TheInterfaceMgr->GetInterfaceColor(kInterfaceColorButton).GetColorRGB();
	if (GetWidgetState() & kWidgetHilited)
	{
		rgb *= 0.625F;
	}

	for (machine a = 4; a < 8; a++)
	{
		vertex[a].color.Set(rgb, alpha);
	}

	vertex[0].texcoord.Set(0.984375F, 0.359375F);
	vertex[1].texcoord.Set(0.984375F, 0.359375F);
	vertex[2].texcoord.Set(0.984375F, 0.359375F);
	vertex[3].texcoord.Set(0.984375F, 0.359375F);

	vertex[4].texcoord.Set(0.0F, 1.0F);
	vertex[5].texcoord.Set(0.0F, 0.875F);
	vertex[6].texcoord.Set(0.125F, 0.875F);
	vertex[7].texcoord.Set(0.125F, 1.0F);

	vertexBuffer.EndUpdate();
}

void LimitWidget::HandleMouseEvent(const PanelMouseEventData *eventData)
{
	EventType eventType = eventData->eventType;

	if (eventType == kEventMouseDown)
	{
		dragPosition = eventData->mousePosition.x - GetIndicatorPosition();
		SetWidgetState(GetWidgetState() | kWidgetHilited);
	}
	else if (eventType == kEventMouseMoved)
	{
		int32 value = GetPositionValue(eventData->mousePosition.x - dragPosition);
		if (limitValue != value)
		{
			limitValue = value;
			SetBuildFlag();
			PostWidgetEvent(WidgetEventData(kEventWidgetChange));
		}
	}
	else if (eventType == kEventMouseUp)
	{
		SetWidgetState(GetWidgetState() & ~kWidgetHilited);
	}
}


CueWidget::CueWidget() :
		TextWidget(kWidgetCue, Vector2D(12.0F, 12.0F), nullptr, "font/Normal"),
		cueVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		cueRenderable(kRenderQuads)
{
	cueType = 0;
	cueValue = 0;
	maxCueValue = 1;
}

CueWidget::CueWidget(CueType type, int32 value, int32 maxValue) :
		TextWidget(kWidgetCue, Vector2D(12.0F, 12.0F), nullptr, "font/Normal"),
		cueVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		cueRenderable(kRenderQuads)
{
	cueType = type;
	SetText(Text::TypeToString(type));

	cueValue = value;
	maxCueValue = maxValue;
}

CueWidget::~CueWidget()
{
}

int32 CueWidget::GetSettingCount(void) const
{
	return (1);
}

Setting *CueWidget::GetSetting(int32 index) const
{
	if (index == 0)
	{
		const StringTable *table = TheWorldEditor->GetStringTable();
		const char *title = table->GetString(StringID('MODL', 'CTYP'));
		return (new TextSetting('CTYP', Text::TypeToString(cueType), title, 4));
	}

	return (nullptr);
}

void CueWidget::SetSetting(const Setting *setting)
{
	if (setting->GetSettingIdentifier() == 'CTYP')
	{
		cueType = Text::StringToType(static_cast<const TextSetting *>(setting)->GetText());
		SetText(Text::TypeToString(cueType));
	}
}

void CueWidget::SetValue(int32 value)
{
	if (cueValue != value)
	{
		cueValue = value;
		SetBuildFlag();
	}
}

void CueWidget::SetMaxValue(int32 maxValue)
{
	maxValue = MaxZero(maxValue);
	if (maxCueValue != maxValue)
	{
		maxCueValue = maxValue;
		SetBuildFlag();
	}
}

float CueWidget::GetIndicatorPosition(void) const
{
	return (PositiveFloor((GetWidgetSize().x - 8.0F) * (float) Min(MaxZero(cueValue), maxCueValue) / (float) Max(maxCueValue, 1)) + 4.0F);
}

int32 CueWidget::GetPositionValue(float x) const
{
	int32 value = (int32) ((x - 4.0F) * (float) maxCueValue / (GetWidgetSize().x - 8.0F) + 0.5F);
	return (MaxZero(Min(value, maxCueValue)));
}

WidgetPart CueWidget::TestPosition(const Point3D& position) const
{
	if (position.y >= GetWidgetSize().y - 12.0F)
	{
		float x = GetIndicatorPosition();
		if ((position.x >= x - 6.0F) && (position.x < x + 6.0F))
		{
			return (kWidgetPartIndicator);
		}
	}

	return (kWidgetPartNone);
}

void CueWidget::Preprocess(void)
{
	TextWidget::Preprocess();

	SetTextFlags(0);
	SetTextAlignment(kTextAlignCenter);

	InitRenderable(&cueRenderable);
	cueRenderable.SetVertexCount(8);
	cueRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &cueVertexBuffer, sizeof(CueVertex));
	cueRenderable.SetVertexAttributeArray(kArrayPosition, 0, 2);
	cueRenderable.SetVertexAttributeArray(kArrayColor, sizeof(Point2D), 4);
	cueRenderable.SetVertexAttributeArray(kArrayTexcoord, sizeof(Point2D) + sizeof(ColorRGBA), 2);
	cueVertexBuffer.Establish(sizeof(CueVertex) * 8);

	cueRenderable.SetMaterialObjectPointer(TheInterfaceMgr->GetMaterialObjectPointer());
}

void CueWidget::Build(void)
{
	float x = GetIndicatorPosition();
	SetTextRenderOffset(Vector3D(x - PositiveFloor(GetWidgetSize().x * 0.5F), 12.0F, 0.0F));

	TextWidget::Build();

	volatile CueVertex *restrict vertex = cueVertexBuffer.BeginUpdate<CueVertex>();

	ColorRGB rgb(1.0F, 1.0F, 0.0F);
	float alpha = (GetGlobalWidgetState() & kWidgetDisabled) ? 0.5F : 1.0F;
	if (GetWidgetState() & kWidgetHilited)
	{
		rgb *= 0.625F;
	}

	vertex[0].position.Set(x - 1.0F, -57.0F);
	vertex[0].color.Set(0.375F, 0.375F, 0.375F, 1.0F);
	vertex[0].texcoord.Set(0.984375F, 0.359375F);

	vertex[1].position.Set(x - 1.0F, 6.0F);
	vertex[1].color.Set(0.375F, 0.375F, 0.375F, 1.0F);
	vertex[1].texcoord.Set(0.984375F, 0.359375F);

	vertex[2].position.Set(x + 1.0F, 6.0F);
	vertex[2].color.Set(0.375F, 0.375F, 0.375F, 1.0F);
	vertex[2].texcoord.Set(0.984375F, 0.359375F);

	vertex[3].position.Set(x + 1.0F, -57.0F);
	vertex[3].color.Set(0.375F, 0.375F, 0.375F, 1.0F);
	vertex[3].texcoord.Set(0.984375F, 0.359375F);

	vertex[4].position.Set(x - 6.0F, 0.0F);
	vertex[4].color.Set(rgb, alpha);
	vertex[4].texcoord.Set(0.0F, 1.0F);

	vertex[5].position.Set(x - 6.0F, 12.0F);
	vertex[5].color.Set(rgb, alpha);
	vertex[5].texcoord.Set(0.0F, 0.875F);

	vertex[6].position.Set(x + 6.0F, 12.0F);
	vertex[6].color.Set(rgb, alpha);
	vertex[6].texcoord.Set(0.125F, 0.875F);

	vertex[7].position.Set(x + 6.0F, 0.0F);
	vertex[7].color.Set(rgb, alpha);
	vertex[7].texcoord.Set(0.125F, 1.0F);

	cueVertexBuffer.EndUpdate();
}

void CueWidget::Render(List<Renderable> *renderList)
{
	TextWidget::Render(renderList);
	renderList->Append(&cueRenderable);
}

void CueWidget::HandleMouseEvent(const PanelMouseEventData *eventData)
{
	EventType eventType = eventData->eventType;

	if (eventType == kEventMouseDown)
	{
		dragPosition = eventData->mousePosition.x - GetIndicatorPosition();
		SetWidgetState(GetWidgetState() | kWidgetHilited);
	}
	else if (eventType == kEventMouseMoved)
	{
		int32 value = GetPositionValue(eventData->mousePosition.x - dragPosition);
		if (cueValue != value)
		{
			const CueWidget *cueWidget = ListElement<CueWidget>::GetOwningList()->First();
			while (cueWidget)
			{
				if ((cueWidget != this) && (cueWidget->GetValue() == value))
				{
					return;
				}

				cueWidget = cueWidget->Next();
			}

			cueValue = value;
			SetBuildFlag();
			PostWidgetEvent(WidgetEventData(kEventWidgetChange));
		}
	}
	else if (eventType == kEventMouseUp)
	{
		SetWidgetState(GetWidgetState() & ~kWidgetHilited);
	}
}


CueInfoWindow::CueInfoWindow(ModelWindow *window, CueWidget *cue) :
		Window("ModelViewer/CueInfo")
{
	modelWindow = window;

	if (!cue)
	{
		cue = new CueWidget;
	}

	cueWidget = cue;
}

CueInfoWindow::~CueInfoWindow()
{
}

void CueInfoWindow::Preprocess(void)
{
	Window::Preprocess();

	okayButton = static_cast<PushButtonWidget *>(FindWidget("OK"));
	cancelButton = static_cast<PushButtonWidget *>(FindWidget("Cancel"));

	configurationWidget = static_cast<ConfigurationWidget *>(FindWidget("Config"));
	configurationWidget->BuildConfiguration(cueWidget);

	SetNextFocusWidget();
}

void CueInfoWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		if (widget == okayButton)
		{
			configurationWidget->CommitConfiguration(cueWidget);

			if (!cueWidget->ListElement<CueWidget>::GetOwningList())
			{
				modelWindow->AddCue(cueWidget);
			}

			Close();
		}
		else if (widget == cancelButton)
		{
			if (!cueWidget->ListElement<CueWidget>::GetOwningList())
			{
				delete cueWidget;
			}

			Close();
		}
	}
}


AnimationPicker::AnimationPicker(const char *subdir) : SceneImportPicker(nullptr, kSceneImportAnimation, kFilePickerMultipleSelection, subdir, "WorldEditor/ImportAnimation")
{
}

AnimationPicker::~AnimationPicker()
{
}

void AnimationPicker::Preprocess(void)
{
	SceneImportPicker::Preprocess();

	anchorXYBox = static_cast<CheckWidget *>(FindWidget("AnchXY"));
	anchorZBox = static_cast<CheckWidget *>(FindWidget("AnchZ"));
	freezeRootBox = static_cast<CheckWidget *>(FindWidget("Freeze"));
	preserveMissingBox = static_cast<CheckWidget *>(FindWidget("Preserve"));
	rotationMenu = static_cast<PopupMenuWidget *>(FindWidget("Rotate"));
}


ModelPage::ModelPage(ModelWindow *window, const char *panelName) : Page(panelName)
{
	modelWindow = window;
}

ModelPage::~ModelPage()
{
}


ModelInfoPage::ModelInfoPage(ModelWindow *window) : ModelPage(window, "ModelViewer/Info")
{
}

ModelInfoPage::~ModelInfoPage()
{
}

void ModelInfoPage::Preprocess(void)
{
	ModelPage::Preprocess();

	int32 nodeCount = 1;
	int32 geometryCount = 0;
	int32 boneCount = 0;
	int32 vertexCount = 0;
	int32 primitiveCount = 0;

	Model *model = GetModelWindow()->GetModel();
	Node *node = model->GetFirstSubnode();
	while (node)
	{
		NodeType type = node->GetNodeType();
		if (type == kNodeGeometry)
		{
			geometryCount++;

			const Mesh *level = static_cast<const Geometry *>(node)->GetObject()->GetGeometryLevel(0);
			vertexCount += level->GetVertexCount();
			primitiveCount += level->GetPrimitiveCount();
		}
		else if (type == kNodeBone)
		{
			node->SetManipulator(new BoneManipulator(static_cast<Bone *>(node)));
			boneCount++;
		}

		nodeCount++;
		node = model->GetNextNode(node);
	}

	static_cast<TextWidget *>(FindWidget("Nodes"))->SetText(String<7>(nodeCount));
	static_cast<TextWidget *>(FindWidget("Geometries"))->SetText(String<7>(geometryCount));
	static_cast<TextWidget *>(FindWidget("Bones"))->SetText(String<7>(boneCount));
	static_cast<TextWidget *>(FindWidget("Vertices"))->SetText(String<7>(vertexCount));
	static_cast<TextWidget *>(FindWidget("Primitives"))->SetText(String<7>(primitiveCount));
}


ModelAnimationPage::ModelAnimationPage(ModelWindow *window) :
		ModelPage(window, "ModelViewer/Animation"),
		playButtonObserver(this, &ModelAnimationPage::HandlePlayButtonEvent),
		stopButtonObserver(this, &ModelAnimationPage::HandleStopButtonEvent),
		loopBoxObserver(this, &ModelAnimationPage::HandleLoopBoxEvent),
		oscillateBoxObserver(this, &ModelAnimationPage::HandleOscillateBoxEvent),
		reverseBoxObserver(this, &ModelAnimationPage::HandleReverseBoxEvent),
		animationListObserver(this, &ModelAnimationPage::HandleAnimationListEvent),
		importButtonObserver(this, &ModelAnimationPage::HandleImportButtonEvent)
{
}

ModelAnimationPage::~ModelAnimationPage()
{
}

void ModelAnimationPage::Preprocess(void)
{
	ModelPage::Preprocess();

	playButton = static_cast<IconButtonWidget *>(FindWidget("Play"));
	playButton->SetObserver(&playButtonObserver);

	stopButton = static_cast<IconButtonWidget *>(FindWidget("Stop"));
	stopButton->SetObserver(&stopButtonObserver);

	loopBox = static_cast<CheckWidget *>(FindWidget("Loop"));
	loopBox->SetObserver(&loopBoxObserver);

	oscillateBox = static_cast<CheckWidget *>(FindWidget("Oscillate"));
	oscillateBox->SetObserver(&oscillateBoxObserver);

	reverseBox = static_cast<CheckWidget *>(FindWidget("Reverse"));
	reverseBox->SetObserver(&reverseBoxObserver);

	animationList = static_cast<ListWidget *>(FindWidget("Animation"));
	animationList->SetObserver(&animationListObserver);

	importButton = static_cast<PushButtonWidget *>(FindWidget("Import"));
	if (TheWorldEditor->GetSceneImportPluginList()->Empty())
	{
		importButton->Disable();
	}
	else
	{
		importButton->SetObserver(&importButtonObserver);
	}

	GetOwningWindow()->SetFocusWidget(animationList);
	BuildAnimationList();
}

void ModelAnimationPage::BuildAnimationList(void)
{
	Map<FileReference>		fileMap;

	animationList->PurgeListItems();

	TheResourceMgr->GetGenericCatalog()->BuildResourceMap(AnimationResource::GetDescriptor(), GetModelWindow()->GetModelDirectory(), &fileMap);
	FileReference *reference = fileMap.First();
	while (reference)
	{
		if (!(reference->GetFlags() & kFileDirectory))
		{
			ResourceName name(reference->GetName());
			name[Text::GetResourceNameLength(name)] = 0;
			animationList->AppendListItem(name);
		}

		reference = reference->Next();
	}
}

void ModelAnimationPage::AnimationPickerProc(FilePicker *picker, void *cookie)
{
	static const float rotationTable[4] =
	{
		0.0F, K::tau_over_4, -K::tau_over_4, K::tau_over_2
	};

	AnimationImportData		importData;

	ModelAnimationPage *page = static_cast<ModelAnimationPage *>(cookie);
	page->StopAnimation();

	const AnimationPicker *animationPicker = static_cast<AnimationPicker *>(picker);
	SceneImportPlugin *importPlugin = animationPicker->GetSceneImportPlugin();

	TheEngine->GetVariable("sceneImportType")->SetValue(&importPlugin->GetImportResourceDescriptor(kSceneImportAnimation)->GetExtension()[1]);
	ConfigDataDescription::WriteEngineConfig();

	unsigned_int32 flags = 0;

	if (animationPicker->GetAnchorXYFlag())
	{
		flags |= kAnimationImportAnchorXY;
	}

	if (animationPicker->GetAnchorZFlag())
	{
		flags |= kAnimationImportAnchorZ;
	}

	if (animationPicker->GetFreezeRootFlag())
	{
		flags |= kAnimationImportFreezeRoot;
	}

	if (animationPicker->GetPreserveMissingFlag())
	{
		flags |= kAnimationImportPreserveMissing;
	}

	importData.importFlags = flags;
	importData.importRotation = rotationTable[animationPicker->GetRotationIndex()];

	ModelWindow *modelWindow = page->GetModelWindow();
	ResourceName listName("");

	int32 count = picker->GetFileNameCount();
	for (machine a = 0; a < count; a++)
	{
		ResourceName importName(picker->GetFileName(a));
		importName[Text::GetResourceNameLength(importName)] = 0;

		ResourceName resourceName(modelWindow->GetModelDirectory());
		resourceName += &importName[Text::GetDirectoryPathLength(importName)];

		if (animationPicker->GetSceneImportPlugin()->ImportAnimation(modelWindow, importName, resourceName, modelWindow->GetModel(), &importData))
		{
			if (listName[0] == 0)
			{
				listName = &importName[Text::GetDirectoryPathLength(importName)];
			}
		}
	}

	if (listName[0] != 0)
	{
		ListWidget *list = page->animationList;
		list->PurgeListItems();

		modelWindow->GetFrameAnimator()->SetAnimation(nullptr);
		TheResourceMgr->ReleaseCache(AnimationResource::GetDescriptor());

		page->BuildAnimationList();

		const Widget *listItem = list->GetFirstListItem();
		int32 selection = 0;
		while (listItem)
		{
			if (Text::CompareText(listName, static_cast<const TextWidget *>(listItem)->GetText()))
			{
				list->SelectListItem(selection);
				modelWindow->ActivateAnimation();
				break;
			}

			selection++;
			listItem = listItem->Next();
		}
	}
}

void ModelAnimationPage::AnimationCompleteProc(Interpolator *interpolator, void *cookie)
{
	ModelAnimationPage *page = static_cast<ModelAnimationPage *>(cookie);
	page->StopAnimation();
}

void ModelAnimationPage::HandlePlayButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		unsigned_int32 mode = (reverseBox->GetValue() == 0) ? kInterpolatorForward : kInterpolatorBackward;

		if (loopBox->GetValue() != 0)
		{
			mode |= kInterpolatorLoop;
		}

		if (oscillateBox->GetValue() != 0)
		{
			mode |= kInterpolatorOscillate;
		}

		FrameAnimator *frameAnimator = GetModelWindow()->GetFrameAnimator();
		Interpolator *interpolator = frameAnimator->GetFrameInterpolator();
		interpolator->SetCompletionProc(&AnimationCompleteProc, this);
		interpolator->SetMode(mode);

		if (mode == kInterpolatorForward)
		{
			if (interpolator->GetValue() == interpolator->GetRange().max)
			{
				interpolator->SetValue(interpolator->GetRange().min);
			}
		}
		else if (mode == kInterpolatorBackward)
		{
			if (interpolator->GetValue() == interpolator->GetRange().min)
			{
				interpolator->SetValue(interpolator->GetRange().max);
			}
		}

		playButton->Disable();
		stopButton->Enable();

		GetModelWindow()->SetMenuUpdateFlag();
	}
}

void ModelAnimationPage::HandleStopButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		const ModelWindow *modelWindow = GetModelWindow();
		if (!modelWindow->GetDiagnosticFlag(kModelDiagnosticMotionBlur))
		{
			Model *model = modelWindow->GetModel();
			Node *node = model->GetFirstSubnode();
			while (node)
			{
				if (node->GetNodeType() == kNodeGeometry)
				{
					Controller *controller = node->GetController();
					if (controller)
					{
						controller->Invalidate();
					}
				}

				node = model->GetNextNode(node);
			}

			model->Invalidate();
			model->StopMotion();
		}

		FrameAnimator *frameAnimator = modelWindow->GetFrameAnimator();
		frameAnimator->GetFrameInterpolator()->SetMode(kInterpolatorStop);

		stopButton->Disable();
		playButton->Enable();

		GetModelWindow()->SetMenuUpdateFlag();
	}
}

void ModelAnimationPage::HandleLoopBoxEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		FrameAnimator *frameAnimator = GetModelWindow()->GetFrameAnimator();
		Interpolator *interpolator = frameAnimator->GetFrameInterpolator();

		unsigned_int32 mode = interpolator->GetMode();
		if (mode != kInterpolatorStop)
		{
			interpolator->SetMode(mode ^ kInterpolatorLoop);
		}
	}
}

void ModelAnimationPage::HandleOscillateBoxEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		FrameAnimator *frameAnimator = GetModelWindow()->GetFrameAnimator();
		Interpolator *interpolator = frameAnimator->GetFrameInterpolator();
		unsigned_int32 mode = interpolator->GetMode();
		if (mode != kInterpolatorStop)
		{
			mode &= ~(kInterpolatorForward | kInterpolatorBackward);
			mode |= (reverseBox->GetValue() == 0) ? kInterpolatorForward : kInterpolatorBackward;
			interpolator->SetMode(mode ^ kInterpolatorOscillate);
		}
	}
}

void ModelAnimationPage::HandleReverseBoxEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		FrameAnimator *frameAnimator = GetModelWindow()->GetFrameAnimator();
		Interpolator *interpolator = frameAnimator->GetFrameInterpolator();

		unsigned_int32 mode = interpolator->GetMode();
		if (mode != kInterpolatorStop)
		{
			interpolator->SetMode(mode ^ (kInterpolatorForward | kInterpolatorBackward));
		}
	}
}

void ModelAnimationPage::HandleAnimationListEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		GetModelWindow()->SelectAnimation();
	}
}

void ModelAnimationPage::HandleImportButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		GetModelWindow()->ImportAnimation();
	}
}

void ModelAnimationPage::ReselectAnimation(const char *name)
{
	int32 index = 0;
	const Widget *widget = animationList->GetFirstListItem();
	while (widget)
	{
		if (Text::CompareText(static_cast<const TextWidget *>(widget)->GetText(), name))
		{
			animationList->SelectListItem(index);
			break;
		}

		index++;
		widget = widget->Next();
	}
}


ModelDisplayPage::ModelDisplayPage(ModelWindow *window) :
		ModelPage(window, "ModelViewer/Display"),
		biasSliderObserver(this, &ModelDisplayPage::HandleBiasSliderEvent)
{
}

ModelDisplayPage::~ModelDisplayPage()
{
}

void ModelDisplayPage::Preprocess(void)
{
	static const char *const diagnosticIdentifier[kModelDiagnosticCount] =
	{
		"Darkness", "Wire", "Normals", "Tangents", "Skeleton", "Motion"
	};

	ModelPage::Preprocess();

	FindWidget("Lod")->SetObserver(&biasSliderObserver);
	biasText = static_cast<TextWidget *>(FindWidget("Bias"));

	for (machine a = 0; a < kModelDiagnosticCount; a++)
	{
		diagnosticBox[a] = static_cast<CheckWidget *>(FindWidget(diagnosticIdentifier[a]));
	}
}

void ModelDisplayPage::HandleBiasSliderEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		int32 bias = static_cast<SliderWidget *>(widget)->GetValue() - 50;
		float b = (float) bias * 0.2F;

		if (bias == 0)
		{
			biasText->SetText("0.0");
		}
		else if (bias > 0)
		{
			biasText->SetText(String<15>("+") + Text::FloatToString(b));
		}
		else
		{
			biasText->SetText(String<15>("\xE2\x88\x92") + Text::FloatToString(-b));		// U+2212
		}

		Model *model = GetModelWindow()->GetModel();
		Node *node = model->GetFirstSubnode();
		while (node)
		{
			if (node->GetNodeType() == kNodeGeometry)
			{
				Geometry *geometry = static_cast<Geometry *>(node);
				geometry->GetObject()->SetGeometryDetailBias(b);

				Controller *controller = geometry->GetController();
				if (controller)
				{
					controller->Invalidate();
				}
			}

			node = model->GetNextNode(node);
		}

		model->Invalidate();
		model->StopMotion();
	}
}


ModelWindow::ModelWindow(const char *name, const ModelResource *resource, const ResourceLocation *location) :
		Window("ModelViewer/Window"),
		cueWidgetObserver(this, &ModelWindow::HandleCueWidgetEvent)
{
	resourceName = name;
	resourceLocation.GetPath() = location->GetPath();
	windowList.Append(this);

	ResourcePath title(name);
	SetWindowTitle(title += ModelResource::GetDescriptor()->GetExtension());
	SetStripTitle(&title[Text::GetDirectoryPathLength(title)]);
	SetStripIcon("ModelViewer/window");

	SetMinWindowSize(Vector2D(760.0F, 600.0F));

	animationName[0] = 0;
	environmentWorld = nullptr;

	lightAzimuth = -K::tau_over_8;
	lightAltitude = K::tau_over_8;

	modelNode = static_cast<Model *>(Node::UnpackTree(resource->GetData()));

	viewerState = kModelViewerUpdateMenus;
	currentTool = kModelToolOrbit;
	toolTracking = false;

	frameAnimator = new FrameAnimator(modelNode);
}

ModelWindow::~ModelWindow()
{
	TheInterfaceMgr->SetCursor(nullptr);

	delete frameAnimator;
	delete environmentWorld;

	TheResourceMgr->ReleaseCache(AnimationResource::GetDescriptor());
}

ResourceResult ModelWindow::Open(const char *name)
{
	ResourceLocation	location;

	ModelWindow *window = windowList.First();
	while (window)
	{
		if (window->resourceName == name)
		{
			TheInterfaceMgr->SetActiveWindow(window);
			return (kResourceOkay);
		}

		window = window->ListElement<ModelWindow>::Next();
	}

	ModelResource *resource = ModelResource::Get(name, kResourceIgnorePackFiles, nullptr, &location);
	if (!resource)
	{
		return (kResourceNotFound);
	}

	window = new ModelWindow(name, resource, &location);
	resource->Release();

	TheInterfaceMgr->AddWidget(window);
	return (kResourceOkay);
}

void ModelWindow::SetWidgetSize(const Vector2D& size)
{
	Window::SetWidgetSize(size);

	PositionWidgets();
	environmentWorld->SetRenderSize((int32) viewportSize.x, (int32) viewportSize.y);
}

void ModelWindow::Preprocess(void)
{
	static const char *const toolIdentifier[kModelToolCount] =
	{
		"Orbit", "Light", "Scroll", "Zoom", "Free"
	};

	Window::Preprocess();

	modelViewport = static_cast<FrustumViewportWidget *>(FindWidget("Viewport"));
	viewportBorder = static_cast<BorderWidget *>(FindWidget("Border"));

	modelViewport->SetMouseEventProc(&ViewportHandleMouseEvent, this);
	modelViewport->SetTrackTaskProc(&ViewportTrackTask, this);
	modelViewport->SetRenderProc(&ViewportRender, this);
	modelViewport->SetCameraTransform(K::tau_over_2, 0.0F, Point3D(2.0F, 0.0F, 1.0F));

	frameLabel = FindWidget("FrameLabel");
	cuesLabel = FindWidget("CuesLabel");
	cuesLine = FindWidget("CuesLine");

	frameSlider = static_cast<SliderWidget *>(FindWidget("FrameBar"));
	frameText = static_cast<TextWidget *>(FindWidget("Frame"));
	beginText = static_cast<TextWidget *>(FindWidget("Begin"));
	endText = static_cast<TextWidget *>(FindWidget("End"));

	for (machine a = 0; a < kModelToolCount; a++)
	{
		toolButton[a] = static_cast<IconButtonWidget *>(FindWidget(toolIdentifier[a]));
	}

	menuBar = static_cast<MenuBarWidget *>(FindWidget("Menu"));

	cuesGroup = new Widget;
	InsertSubnodeAfter(cuesGroup, cuesLine);
	cuesGroup->Preprocess();

	beginLimit = new LimitWidget(Vector2D(1.0F, 36.0F));
	InsertSubnodeBefore(beginLimit, frameSlider);
	beginLimit->Preprocess();
	beginLimit->Hide();

	endLimit = new LimitWidget(Vector2D(1.0F, 36.0F));
	InsertSubnodeBefore(endLimit, frameSlider);
	endLimit->Preprocess();
	endLimit->Hide();

	infoPage = new ModelInfoPage(this);
	animationPage = new ModelAnimationPage(this);
	displayPage = new ModelDisplayPage(this);

	bookWidget = new BookWidget(Vector2D(163.0F, 0.0F));
	bookWidget->AppendPage(infoPage);
	bookWidget->AppendPage(animationPage);
	bookWidget->AppendPage(displayPage);
	bookWidget->OrganizePages();
	AppendNewSubnode(bookWidget);

	PositionWidgets();
	BuildMenus();

	SetEnvironmentWorld(TheEngine->GetVariable("modelEnvironment")->GetValue());
}

ResourcePath ModelWindow::GetModelDirectory(void) const
{
	ResourcePath directory(resourceLocation.GetPath());
	directory += '/';
	directory += resourceName;
	directory[Text::GetDirectoryPathLength(directory)] = 0;
	return (directory);
}

void ModelWindow::PositionWidgets(void)
{
	float width = GetWidgetSize().x;
	float height = GetWidgetSize().y;

	const Point3D& position = modelViewport->GetWidgetPosition();
	viewportSize.Set(width - position.x - 4.0F, height - position.y - 100.0F);

	modelViewport->SetWidgetSize(viewportSize);
	viewportBorder->SetWidgetSize(viewportSize);

	float menuBarHeight = menuBar->GetWidgetSize().y;
	menuBar->SetWidgetSize(Vector2D(width, menuBarHeight));

	bookWidget->SetWidgetPosition(Point3D(4.0F, menuBarHeight + 31.0F, 0.0F));
	bookWidget->SetWidgetSize(Vector2D(bookWidget->GetWidgetSize().x, height - menuBarHeight - 35.0F));
	bookWidget->Invalidate();

	float x = position.x;
	float y = position.y;
	float w = viewportSize.x;
	float h = viewportSize.y;

	SetBackgroundQuad(0, Point3D(0.0F, 0.0F, 0.0F), Vector2D(x - 1.0F, height));
	SetBackgroundQuad(1, Point3D(x + w + 1.0F, 0.0F, 0.0F), Vector2D(width - x - w - 1.0F, height));
	SetBackgroundQuad(2, Point3D(x - 1.0F, 0.0F, 0.0F), Vector2D(width - x + 1.0F, y));
	SetBackgroundQuad(3, Point3D(x - 1.0F, y + h + 1.0F, 0.0F), Vector2D(width - x + 1.0F, height - y - h - 1.0F));

	x += 52.0F;
	y += h;
	w -= 60.0F;

	frameLabel->SetWidgetPosition(Point3D(x - 56.0F, y + 17.0F, 0.0F));
	frameLabel->Invalidate();

	cuesLabel->SetWidgetPosition(Point3D(x - 56.0F, y + 70.0F, 0.0F));
	cuesLabel->Invalidate();

	cuesLine->SetWidgetPosition(Point3D(x, y + 76.0F, 0.0F));
	cuesLine->SetWidgetSize(Vector2D(w, 1.0F));
	cuesLine->Invalidate();

	CueWidget *cueWidget = cueWidgetList.First();
	while (cueWidget)
	{
		PositionCueWidget(cueWidget);
		cueWidget->Invalidate();
		cueWidget = cueWidget->Next();
	}

	frameSlider->SetWidgetPosition(Point3D(x, y + 16.0F, 0.0F));
	frameSlider->SetWidgetSize(Vector2D(w, frameSlider->GetWidgetSize().y));
	frameSlider->Invalidate();

	beginLimit->SetWidgetPosition(Point3D(x, y + 14.0F, 0.0F));
	beginLimit->SetWidgetSize(Vector2D(w, beginLimit->GetWidgetSize().y));
	beginLimit->Invalidate();

	endLimit->SetWidgetPosition(Point3D(x, y + 14.0F, 0.0F));
	endLimit->SetWidgetSize(Vector2D(w, endLimit->GetWidgetSize().y));
	endLimit->Invalidate();

	frameText->SetWidgetPosition(Point3D(frameText->GetWidgetPosition().x, y + 3.0F, 0.0F));
	frameText->Invalidate();

	UpdateLimitText();
}

void ModelWindow::PositionCueWidget(CueWidget *cueWidget)
{
	const Point3D& position = cuesLine->GetWidgetPosition();
	const Vector2D& size = cuesLine->GetWidgetSize();

	cueWidget->SetWidgetPosition(Point3D(position.x, position.y - 5.0F, 0.0F));
	cueWidget->SetWidgetSize(Vector2D(size.x, cueWidget->GetWidgetSize().y));
}

void ModelWindow::UpdateLimitText(void)
{
	beginText->SetText(String<7>(beginLimit->GetValue()));
	const Point3D& beginPosition = beginLimit->GetWidgetPosition();
	beginText->SetWidgetPosition(Point3D(beginPosition.x + beginLimit->GetIndicatorPosition() - beginText->GetWidgetSize().x * 0.5F, beginPosition.y + beginLimit->GetWidgetSize().y + 2.0F, 0.0F));
	beginText->Invalidate();

	endText->SetText(String<7>(endLimit->GetValue()));
	const Point3D& endPosition = endLimit->GetWidgetPosition();
	endText->SetWidgetPosition(Point3D(endPosition.x + endLimit->GetIndicatorPosition() - endText->GetWidgetSize().x * 0.5F, endPosition.y + endLimit->GetWidgetSize().y + 2.0F, 0.0F));
	endText->Invalidate();
}

void ModelWindow::BuildMenus(void)
{
	const StringTable *table = TheWorldEditor->GetStringTable();

	modelMenu = new PulldownMenuWidget(table->GetString(StringID('MODL', 'MENU', 'MODL')));
	menuBar->AppendMenu(modelMenu);

	modelMenu->AppendMenuItem(new MenuItemWidget(table->GetString(StringID('MODL', 'MENU', 'MODL', 'CLOS')), WidgetObserver<ModelWindow>(this, &ModelWindow::HandleCloseMenuItem), Shortcut('W')));
	modelMenu->AppendMenuItem(new MenuItemWidget(kLineSolid));

	MenuItemWidget *widget = new MenuItemWidget(table->GetString(StringID('MODL', 'MENU', 'MODL', 'SANM')), WidgetObserver<ModelWindow>(this, &ModelWindow::HandleSaveAnimationMenuItem), Shortcut('S'));
	modelMenu->AppendMenuItem(widget);
	saveAnimationItem = widget;
	widget->Disable();

	modelMenu->AppendMenuItem(new MenuItemWidget(table->GetString(StringID('MODL', 'MENU', 'MODL', 'SMDL')), WidgetObserver<ModelWindow>(this, &ModelWindow::HandleSaveModelMenuItem)));
	modelMenu->AppendMenuItem(new MenuItemWidget(kLineSolid));
	modelMenu->AppendMenuItem(new MenuItemWidget(table->GetString(StringID('MODL', 'MENU', 'MODL', 'ENVR')), WidgetObserver<ModelWindow>(this, &ModelWindow::HandleSelectEnvironmentMenuItem), Shortcut('E')));

	cueMenu = new PulldownMenuWidget(table->GetString(StringID('MODL', 'MENU', 'CUE ')));
	menuBar->AppendMenu(cueMenu);

	widget = new MenuItemWidget(table->GetString(StringID('MODL', 'MENU', 'CUE ', 'INSC')), WidgetObserver<ModelWindow>(this, &ModelWindow::HandleInsertCueMenuItem), Shortcut('K'));
	cueMenuItem[kCueMenuInsertCue] = widget;
	cueMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('MODL', 'MENU', 'CUE ', 'DELC')), WidgetObserver<ModelWindow>(this, &ModelWindow::HandleDeleteCueMenuItem), Shortcut('D'));
	cueMenuItem[kCueMenuDeleteCue] = widget;
	cueMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('MODL', 'MENU', 'CUE ', 'DELA')), WidgetObserver<ModelWindow>(this, &ModelWindow::HandleDeleteAllCuesMenuItem), Shortcut('D', kShortcutShift));
	cueMenuItem[kCueMenuDeleteAllCues] = widget;
	cueMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('MODL', 'MENU', 'CUE ', 'INFO')), WidgetObserver<ModelWindow>(this, &ModelWindow::HandleGetCueInfoMenuItem), Shortcut('I'));
	cueMenuItem[kCueMenuGetCueInfo] = widget;
	cueMenu->AppendMenuItem(widget);
}

void ModelWindow::HandleCloseMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	Close();
}

void ModelWindow::HandleSaveAnimationMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	File			file;
	ResourcePath	path;

	TheResourceMgr->GetGenericCatalog()->GetResourcePath(AnimationResource::GetDescriptor(), animationName, &resourceLocation, &path);
	if (file.Open(path, kFileCreate) == kFileOkay)
	{
		const AnimationHeader *animationHeader = frameAnimator->GetAnimationHeader();

		int32 trackCount = 0;
		unsigned_int32 transformTrackSize = 0;
		unsigned_int32 morphWeightTrackSize = 0;

		const TransformTrackHeader *transformTrackHeader = frameAnimator->GetTransformTrackHeader();
		if (transformTrackHeader)
		{
			int32 transformNodeCount = transformTrackHeader->transformNodeCount;
			unsigned_int32 transformTrackHeaderSize = sizeof(TransformTrackHeader) + (transformTrackHeader->bucketCount - 1) * sizeof(TransformTrackHeader::NodeBucket) + transformNodeCount * sizeof(TransformTrackHeader::NodeData);
			transformTrackSize = transformTrackHeaderSize + transformNodeCount * animationHeader->frameCount * sizeof(TransformFrameData);
			trackCount++;
		}

		const MorphWeightTrackHeader *morphWeightTrackHeader = frameAnimator->GetMorphWeightTrackHeader();
		if (morphWeightTrackHeader)
		{
			int32 morphWeightNodeCount = morphWeightTrackHeader->morphWeightNodeCount;
			int32 frameMorphWeightCount = morphWeightTrackHeader->frameMorphWeightCount;
			unsigned_int32 morphWeightTrackHeaderSize = sizeof(MorphWeightTrackHeader) + (morphWeightTrackHeader->bucketCount - 1) * sizeof(MorphWeightTrackHeader::NodeBucket) + morphWeightNodeCount * sizeof(MorphWeightTrackHeader::NodeData);
			morphWeightTrackSize = morphWeightTrackHeaderSize + frameMorphWeightCount * 4 + frameMorphWeightCount * animationHeader->frameCount * 4;
			trackCount++;
		}

		int32 cueCount = cueWidgetList.GetElementCount();
		trackCount += (cueCount != 0);

		unsigned_int32 headerSize = sizeof(AnimationHeader) + (trackCount - 1) * sizeof(AnimationHeader::TrackData);
		char *headerStorage = new char[headerSize];

		AnimationHeader *outputAnimationHeader = reinterpret_cast<AnimationHeader *>(headerStorage);
		outputAnimationHeader->frameCount = animationHeader->frameCount;
		outputAnimationHeader->frameDuration = animationHeader->frameDuration;
		outputAnimationHeader->trackCount = trackCount;

		AnimationHeader::TrackData *trackData = outputAnimationHeader->trackData;
		unsigned_int32 trackOffset = headerSize;

		if (transformTrackSize != 0)
		{
			trackData->trackType = kTrackTransform;
			trackData->trackOffset = trackOffset;

			trackData++;
			trackOffset += transformTrackSize;
		}

		if (morphWeightTrackSize != 0)
		{
			trackData->trackType = kTrackMorphWeight;
			trackData->trackOffset = trackOffset;

			trackData++;
			trackOffset += morphWeightTrackSize;
		}

		if (cueCount != 0)
		{
			trackData->trackType = kTrackCue;
			trackData->trackOffset = trackOffset;
		}

		file.Write(AnimationResource::resourceSignature, 8);
		file.Write(outputAnimationHeader, headerSize);

		delete[] headerStorage;

		if (transformTrackSize != 0)
		{
			file.Write(transformTrackHeader, transformTrackSize);
		}

		if (morphWeightTrackSize != 0)
		{
			file.Write(morphWeightTrackHeader, morphWeightTrackSize);
		}

		if (cueCount != 0)
		{
			List<CueWidget>		sortedList;

			CueWidget *cueWidget = cueWidgetList.First();
			sortedList.Append(cueWidget);

			for (;;)
			{
				cueWidget = cueWidgetList.First();
				if (!cueWidget)
				{
					break;
				}

				int32 value = cueWidget->GetValue();
				CueWidget *sortedWidget = sortedList.First();
				do
				{
					if (value < sortedWidget->GetValue())
					{
						sortedList.InsertBefore(cueWidget, sortedWidget);
						goto next;
					}

					sortedWidget = sortedWidget->Next();
				} while (sortedWidget);

				sortedList.Append(cueWidget);
				next:;
			}

			unsigned_int32 cueTrackSize = sizeof(CueTrackHeader) + cueCount * sizeof(CueData);
			char *cueTrackStorage = new char[cueTrackSize];

			CueTrackHeader *cueTrackHeader = reinterpret_cast<CueTrackHeader *>(cueTrackStorage);
			cueTrackHeader->cueCount = cueCount;

			CueData *cueData = const_cast<CueData *>(cueTrackHeader->GetCueData());
			float frameDuration = animationHeader->frameDuration;

			cueWidget = sortedList.First();
			for (machine a = 0; a < cueCount; a++)
			{
				cueData->cueType = cueWidget->GetCueType();
				cueData->cueTime = (float) cueWidget->GetValue() * frameDuration;

				cueData++;
				cueWidget = cueWidget->Next();
			}

			file.Write(cueTrackHeader, cueTrackSize);
			delete[] cueTrackStorage;

			for (;;)
			{
				cueWidget = sortedList.First();
				if (!cueWidget)
				{
					break;
				}

				cueWidgetList.Append(cueWidget);
			}
		}

		viewerState &= ~kModelViewerUnsaved;
	}
}

void ModelWindow::HandleSaveModelMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	File			file;
	ResourcePath	path;

	TheResourceMgr->GetGenericCatalog()->GetResourcePath(ModelResource::GetDescriptor(), resourceName, &resourceLocation, &path);
	if (file.Open(path, kFileCreate) == kFileOkay)
	{
		modelNode->PackTree(&file, kPackInitialize);
	}
}

void ModelWindow::EnvironmentPickerProc(FilePicker *picker, void *cookie)
{
	ResourceName name(picker->GetResourceName());

	TheEngine->GetVariable("modelEnvironment")->SetValue(name);
	ConfigDataDescription::WriteEngineConfig();

	static_cast<ModelWindow *>(cookie)->SetEnvironmentWorld(name);
}

void ModelWindow::HandleSelectEnvironmentMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	const char *title = TheWorldEditor->GetStringTable()->GetString(StringID('MODL', 'ENVR'));
	FilePicker *picker = new FilePicker('MENV', title, TheResourceMgr->GetGenericCatalog(), WorldResource::GetDescriptor(), "Tools/Modelviewer");
	picker->SetCompletionProc(&EnvironmentPickerProc, this);
	AddSubwindow(picker);
}

void ModelWindow::SetEnvironmentWorld(const char *name)
{
	Node *super = modelNode->GetSuperNode();
	if (super)
	{
		super->RemoveSubnode(modelNode);
	}

	delete environmentWorld;

	World *world = nullptr;
	if (name[0] != 0)
	{
		world = new World(name, kWorldViewport | kWorldZeroBackgroundVelocity | kWorldListenerInhibit);
		if (world->Preprocess() == kWorldOkay)
		{
			zoneNode = world->GetRootNode();
		}
		else
		{
			delete world;
			world = nullptr;
		}
	}

	if (!world)
	{
		zoneNode = new InfiniteZone;

		ZoneObject *zoneObject = new InfiniteZoneObject;
		zoneNode->SetObject(zoneObject);
		zoneObject->Release();

		zoneObject->SetAmbientLight(ColorRGBA(0.5F, 0.5F, 0.5F, 1.0F));
		zoneObject->SetEnvironmentMap("C4/environment");

		PlateGeometry *ground = new PlateGeometry(Vector2D(100.0F, 100.0F));
		ground->SetNodePosition(Point3D(-50.0F, -50.0F, 0.0F));

		PlateGeometryObject *groundObject = ground->GetObject();
		groundObject->SetGeometryFlags((groundObject->GetGeometryFlags() | kGeometryMotionBlurInhibit) & ~kGeometryCastShadows);

		MaterialObject *materialObject = new MaterialObject;
		materialObject->AddAttribute(new DiffuseAttribute(ColorRGBA(0.75F, 0.75F, 0.75F, 1.0F)));
		ground->SetMaterialObject(0, materialObject);
		materialObject->Release();

		ground->Update();
		groundObject->Build(ground);
		zoneNode->AppendSubnode(ground);

		world = new World(zoneNode, kWorldViewport | kWorldClearColor | kWorldZeroBackgroundVelocity | kWorldListenerInhibit);
		world->Preprocess();
	}

	environmentWorld = world;

	lightNode = new InfiniteLight(ColorRGB(1.0F, 1.0F, 1.0F));
	lightNode->SetNodeMatrix3D(Matrix3D().SetRotationAboutZ(lightAzimuth) * Matrix3D().SetRotationAboutY(lightAltitude));

	Range<float> *cascadeRange = lightNode->GetObject()->GetCascadeRangeArray();
	cascadeRange[0].Set(0.0F, 5.0F);
	cascadeRange[1].Set(4.0F, 12.0F);
	cascadeRange[2].Set(10.0F, 60.0F);
	cascadeRange[3].Set(56.0F, 128.0F);

	PhysicsNode *physicsNode = new PhysicsNode;
	physicsNode->SetController(new PhysicsController);

	zoneNode->AppendNewSubnode(lightNode);
	zoneNode->AppendNewSubnode(physicsNode);
	zoneNode->AppendSubnode(modelNode);

	modelNode->Preprocess();
	modelNode->StopMotion();

	environmentWorld->SetRenderSize((int32) viewportSize.x, (int32) viewportSize.y);

	FrustumCamera *camera = modelViewport->GetViewportCamera();
	camera->GetObject()->SetClearColor(K::transparent);
	environmentWorld->SetCamera(camera);
}

void ModelWindow::HandleInsertCueMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	AddSubwindow(new CueInfoWindow(this));
}

void ModelWindow::HandleDeleteCueMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	int32 frame = GetAnimationFrame();

	CueWidget *cueWidget = cueWidgetList.First();
	while (cueWidget)
	{
		if (cueWidget->GetValue() == frame)
		{
			delete cueWidget;
			viewerState |= kModelViewerUnsaved;
			break;
		}

		cueWidget = cueWidget->Next();
	}

	SetMenuUpdateFlag();
}

void ModelWindow::HandleDeleteAllCuesMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	cueWidgetList.Purge();
	viewerState |= kModelViewerUnsaved | kModelViewerUpdateMenus;
}

void ModelWindow::HandleGetCueInfoMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	int32 frame = GetAnimationFrame();

	CueWidget *cueWidget = cueWidgetList.First();
	while (cueWidget)
	{
		if (cueWidget->GetValue() == frame)
		{
			AddSubwindow(new CueInfoWindow(this, cueWidget));
			break;
		}

		cueWidget = cueWidget->Next();
	}
}

void ModelWindow::HandleCueWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		viewerState |= kModelViewerUnsaved | kModelViewerUpdateMenus;
	}
}

void ModelWindow::SelectConfirmDialogComplete(Dialog *dialog, void *cookie)
{
	ModelWindow *modelWindow = static_cast<ModelWindow *>(cookie);

	int32 status = dialog->GetDialogStatus();
	if (status == kDialogOkay)
	{
		modelWindow->HandleSaveAnimationMenuItem(nullptr, nullptr);
		modelWindow->ActivateAnimation();
	}
	else if (status == kDialogIgnore)
	{
		modelWindow->ActivateAnimation();
	}
	else
	{
		modelWindow->animationPage->ReselectAnimation(modelWindow->animationName);
	}
}

void ModelWindow::SelectAnimation(void)
{
	if (viewerState & kModelViewerUnsaved)
	{
		DisplayConfirmationDialog(&SelectConfirmDialogComplete);
	}
	else
	{
		ActivateAnimation();
	}
}

void ModelWindow::ImportConfirmDialogComplete(Dialog *dialog, void *cookie)
{
	ModelWindow *modelWindow = static_cast<ModelWindow *>(cookie);

	int32 status = dialog->GetDialogStatus();
	if (status == kDialogOkay)
	{
		modelWindow->HandleSaveAnimationMenuItem(nullptr, nullptr);
	}
	else if (status == kDialogCancel)
	{
		return;
	}

	AnimationPicker *picker = new AnimationPicker(modelWindow->GetModelDirectory());
	picker->SetCompletionProc(&ModelAnimationPage::AnimationPickerProc, modelWindow->animationPage);
	modelWindow->AddSubwindow(picker);
}

void ModelWindow::ImportAnimation(void)
{
	if (viewerState & kModelViewerUnsaved)
	{
		DisplayConfirmationDialog(&ImportConfirmDialogComplete);
	}
	else
	{
		AnimationPicker *picker = new AnimationPicker(GetModelDirectory());
		picker->SetCompletionProc(&ModelAnimationPage::AnimationPickerProc, animationPage);
		AddSubwindow(picker);
	}
}

void ModelWindow::ActivateAnimation(void)
{
	const Widget *widget = animationPage->GetSelectedAnimation();
	if (widget)
	{
		if (!modelNode->GetRootAnimator())
		{
			modelNode->SetRootAnimator(frameAnimator);
		}

		TheResourceMgr->ReleaseCache(AnimationResource::GetDescriptor());

		ResourceName path(resourceName);
		path[Text::GetDirectoryPathLength(path)] = 0;
		path += static_cast<const TextWidget *>(widget)->GetText();

		animationName = path;
		frameAnimator->SetAnimation(path);

		modelNode->Animate();
		modelNode->StopMotion();

		int32 maxValue = frameAnimator->GetAnimationHeader()->frameCount - 1;

		frameSlider->SetValue(0);
		frameSlider->SetMaxValue(maxValue);
		frameSlider->Enable();

		beginLimit->SetLimitRange(0, MaxZero(maxValue - 1));
		beginLimit->SetValue(0);
		beginLimit->SetMaxValue(maxValue);
		beginLimit->Show();
		beginText->Show();

		if (maxValue > 0)
		{
			endLimit->SetLimitRange(1, maxValue);
			endLimit->SetValue(maxValue);
			endLimit->SetMaxValue(maxValue);
			endLimit->Show();
			endText->Show();
		}
		else
		{
			endLimit->Hide();
			endText->Hide();
		}

		UpdateLimitText();
		animationPage->StopAnimation();

		cueWidgetList.Purge();
		const CueTrackHeader *cueTrackHeader = frameAnimator->GetCueTrackHeader();
		if (cueTrackHeader)
		{
			float frameFrequency = frameAnimator->GetFrameFrequency();

			int32 cueCount = cueTrackHeader->cueCount;
			const CueData *cueData = cueTrackHeader->GetCueData();
			for (machine a = 0; a < cueCount; a++)
			{
				int32 value = (int32) (cueData->cueTime * frameFrequency + 0.5F);
				CueWidget *cueWidget = new CueWidget(cueData->cueType, value, maxValue);
				cueWidget->SetObserver(&cueWidgetObserver);
				cueWidgetList.Append(cueWidget);

				PositionCueWidget(cueWidget);
				cuesGroup->AppendNewSubnode(cueWidget);

				cueData++;
			}
		}

		saveAnimationItem->Enable();
		animationPage->EnablePlayButton();

		viewerState = (viewerState | kModelViewerUpdateMenus) & ~kModelViewerUnsaved;
	}
}

int32 ModelWindow::GetAnimationFrame(void) const
{
	return ((int32) (frameAnimator->GetFrameInterpolator()->GetValue() * frameAnimator->GetFrameFrequency() + 0.5F));
}

void ModelWindow::AddCue(CueWidget *cueWidget)
{
	if (!cueWidget->ListElement<CueWidget>::GetOwningList())
	{
		cueWidget->SetObserver(&cueWidgetObserver);
		cueWidgetList.Append(cueWidget);

		cueWidget->SetMaxValue(frameAnimator->GetAnimationHeader()->frameCount - 1);
		cueWidget->SetValue(GetAnimationFrame());

		PositionCueWidget(cueWidget);
		cuesGroup->AppendNewSubnode(cueWidget);
	}

	viewerState |= kModelViewerUnsaved | kModelViewerUpdateMenus;
}

int32 ModelWindow::GetActiveTool(void) const
{
	int32 tool = currentTool;
	if (InterfaceMgr::GetCommandKey())
	{
		if (tool == kModelToolOrbit)
		{
			tool = kModelToolLight;
		}
		else
		{
			tool = kModelToolOrbit;
		}
	}
	else if (InterfaceMgr::GetOptionKey())
	{
		if (tool == kModelToolHand)
		{
			tool = kModelToolGlass;
		}
		else
		{
			tool = kModelToolHand;
		}
	}

	return (tool);
}

void ModelWindow::TrackGlassTool(ViewportWidget *viewport, float dy)
{
	if (dy != 0.0F)
	{
		FrustumViewportWidget *frustumViewport = static_cast<FrustumViewportWidget *>(viewport);
		FrustumCamera *camera = frustumViewport->GetViewportCamera();

		const Vector3D& view = camera->GetWorldTransform()[2];
		Vector3D v = view * (dy * 0.1F);
		camera->SetNodePosition(camera->GetNodePosition() + v);
	}
}

void ModelWindow::ViewportHandleMouseEvent(const MouseEventData *eventData, ViewportWidget *viewport, void *cookie)
{
	ModelWindow *modelWindow = static_cast<ModelWindow *>(cookie);

	EventType eventType = eventData->eventType;
	if (eventType == kEventMouseDown)
	{
		int32 tool = modelWindow->GetActiveTool();

		modelWindow->trackTool = tool;
		modelWindow->toolTracking = true;
		modelWindow->previousPosition = eventData->mousePosition;

		if (tool == kModelToolFree)
		{
			modelWindow->freeCameraSpeed = 0.0F;
			modelWindow->freeCameraFlags = 0;
		}
	}
	else if (eventType == kEventRightMouseDown)
	{
		modelWindow->trackTool = kModelToolFree;
		modelWindow->toolTracking = true;
		modelWindow->previousPosition = eventData->mousePosition;

		modelWindow->freeCameraSpeed = 0.0F;
		modelWindow->freeCameraFlags = 0;
	}
	else if ((eventType == kEventMouseUp) || (eventType == kEventRightMouseUp))
	{
		modelWindow->toolTracking = false;
	}
	else if (eventType == kEventMouseMoved)
	{
		if (modelWindow->toolTracking)
		{
			FrustumViewportWidget *frustumViewport = static_cast<FrustumViewportWidget *>(viewport);

			int32 tool = modelWindow->trackTool;
			if (tool == kModelToolHand)
			{
				float dx = modelWindow->previousPosition.x - eventData->mousePosition.x;
				float dy = modelWindow->previousPosition.y - eventData->mousePosition.y;

				if ((dx != 0.0F) || (dy != 0.0F))
				{
					FrustumCamera *camera = frustumViewport->GetViewportCamera();
					const Vector3D& right = camera->GetWorldTransform()[0];
					const Vector3D& down = camera->GetWorldTransform()[1];
					Vector3D v = right * (dx * 8.0F / frustumViewport->GetWidgetSize().x) + down * (dy * 8.0F / frustumViewport->GetWidgetSize().y);
					camera->SetNodePosition(camera->GetNodePosition() + v);
				}
			}
			else if (tool == kModelToolGlass)
			{
				modelWindow->TrackGlassTool(frustumViewport, modelWindow->previousPosition.y - eventData->mousePosition.y);
			}
			else if (tool == kModelToolOrbit)
			{
				float dx = (modelWindow->previousPosition.x - eventData->mousePosition.x) * (8.0F / frustumViewport->GetWidgetSize().x);
				float dy = InterfaceMgr::GetShiftKey() ? 0.0F : (modelWindow->previousPosition.y - eventData->mousePosition.y) * (8.0F / frustumViewport->GetWidgetSize().y);

				float azm = frustumViewport->GetCameraAzimuth() + dx;
				if (azm < -K::tau_over_2)
				{
					azm += K::tau;
				}
				else if (azm > K::tau_over_2)
				{
					azm -= K::tau;
				}

				float alt0 = frustumViewport->GetCameraAltitude();
				float alt = Clamp(alt0 + dy, -1.45F, 1.45F);

				const Camera *camera = frustumViewport->GetViewportCamera();
				const Vector3D& right = camera->GetNodeTransform()[0];

				Point3D p = camera->GetNodePosition();
				Matrix3D m = Quaternion().SetRotationAboutAxis(alt - alt0, right).GetRotationMatrix();
				p = Matrix3D().SetRotationAboutZ(dx) * (m * p);

				frustumViewport->SetCameraTransform(azm, alt, p);
			}
			else if (tool == kModelToolLight)
			{
				float dx = (modelWindow->previousPosition.x - eventData->mousePosition.x) * (8.0F / frustumViewport->GetWidgetSize().x);
				float dy = (modelWindow->previousPosition.y - eventData->mousePosition.y) * (8.0F / frustumViewport->GetWidgetSize().y);

				float azm = modelWindow->lightAzimuth + dx;
				if (azm < -K::tau_over_2)
				{
					azm += K::tau;
				}
				else if (azm > K::tau_over_2)
				{
					azm -= K::tau;
				}

				float alt = modelWindow->lightAltitude + dy;
				if (alt < -K::tau_over_2)
				{
					alt += K::tau;
				}
				else if (alt > K::tau_over_2)
				{
					alt -= K::tau;
				}

				modelWindow->lightAzimuth = azm;
				modelWindow->lightAltitude = alt;

				modelWindow->lightNode->SetNodeMatrix3D(Matrix3D().SetRotationAboutZ(azm) * Matrix3D().SetRotationAboutY(alt));
				modelWindow->lightNode->Invalidate();
			}
			else if (tool == kModelToolFree)
			{
				float dx = (float) (modelWindow->previousPosition.x - eventData->mousePosition.x) * (8.0F / frustumViewport->GetWidgetSize().x);
				float dy = (float) (modelWindow->previousPosition.y - eventData->mousePosition.y) * (8.0F / frustumViewport->GetWidgetSize().y);

				float azm = frustumViewport->GetCameraAzimuth() + dx;
				if (azm < -K::tau_over_2)
				{
					azm += K::tau;
				}
				else if (azm > K::tau_over_2)
				{
					azm -= K::tau;
				}

				float alt = Clamp(frustumViewport->GetCameraAltitude() + dy, -1.45F, 1.45F);

				Camera *camera = frustumViewport->GetViewportCamera();
				frustumViewport->SetCameraTransform(azm, alt, camera->GetNodePosition());
			}

			modelWindow->previousPosition = eventData->mousePosition;
		}
	}
	else if (eventType == kEventMouseWheel)
	{
		modelWindow->TrackGlassTool(viewport, eventData->wheelDelta.y * 8.0F);
	}
}

void ModelWindow::ViewportTrackTask(const Point3D& position, ViewportWidget *viewport, void *cookie)
{
	ModelWindow *modelWindow = static_cast<ModelWindow *>(cookie);

	if ((modelWindow->toolTracking) && (modelWindow->trackTool == kModelToolFree))
	{
		FrustumViewportWidget *frustumViewport = static_cast<FrustumViewportWidget *>(viewport);
		Camera *camera = frustumViewport->GetViewportCamera();
		Point3D q = camera->GetNodePosition();

		unsigned_int32 flags = modelWindow->freeCameraFlags;
		if (flags != 0)
		{
			float t = TheTimeMgr->GetFloatDeltaTime();
			modelWindow->freeCameraSpeed = Fmin(modelWindow->freeCameraSpeed + t * 1.0e-5F, 0.02F);
			t *= modelWindow->freeCameraSpeed;

			const Vector3D& view = camera->GetNodeTransform()[2];
			const Vector3D& right = camera->GetNodeTransform()[0];

			if (flags & kFreeCameraForward)
			{
				q += view * t;
			}

			if (flags & kFreeCameraBackward)
			{
				q -= view * t;
			}

			if (flags & kFreeCameraRight)
			{
				q += right * t;
			}

			if (flags & kFreeCameraLeft)
			{
				q -= right * t;
			}

			if (flags & kFreeCameraUp)
			{
				q.z += t;
			}

			if (flags & kFreeCameraDown)
			{
				q.z -= t;
			}
		}
		else
		{
			modelWindow->freeCameraSpeed = 0.0F;
		}

		frustumViewport->SetCameraPosition(q);
	}
}

void ModelWindow::ViewportRender(List<Renderable> *renderList, ViewportWidget *viewport, void *cookie)
{
	ManipulatorRenderData	renderData;

	ModelWindow *modelWindow = static_cast<ModelWindow *>(cookie);
	unsigned_int32 flags = (modelWindow->GetDiagnosticFlag(kModelDiagnosticWireframe)) ? kDiagnosticWireframe | kDiagnosticDepthTest : 0;

	if (modelWindow->GetDiagnosticFlag(kModelDiagnosticNormals))
	{
		flags |= kDiagnosticNormals;
	}

	if (modelWindow->GetDiagnosticFlag(kModelDiagnosticTangents))
	{
		flags |= kDiagnosticTangents;
	}

	unsigned_int32 diagnosticFlags = TheGraphicsMgr->GetDiagnosticFlags();
	TheGraphicsMgr->SetDiagnosticFlags(flags);

	if (!modelWindow->GetDiagnosticFlag(kModelDiagnosticDarkness))
	{
		modelWindow->lightNode->Enable();
	}
	else
	{
		modelWindow->lightNode->Disable();
	}

	Model *model = modelWindow->modelNode;
	if (modelWindow->frameAnimator->GetFrameInterpolator()->GetMode() != kInterpolatorStop)
	{
		model->Animate();
	}

	World *world = modelWindow->environmentWorld;
	world->Move();
	world->Update();
	world->BeginRendering();
	world->Render();
	world->EndRendering();

	TheGraphicsMgr->SetDiagnosticFlags(diagnosticFlags);

	renderData.viewportIndex = 0;
	renderData.viewportScale = 0.0F;
	renderData.editorFlags = 0;
	renderData.viewportCamera = viewport->GetViewportCamera();
	renderData.geometryList = nullptr;
	renderData.manipulatorList = renderList;
	renderData.handleList = nullptr;
	renderData.connectorList = nullptr;

	if (modelWindow->GetDiagnosticFlag(kModelDiagnosticSkeleton))
	{
		Node *node = model->GetFirstSubnode();
		while (node)
		{
			if (node->GetNodeType() == kNodeBone)
			{
				EditorManipulator *manipulator = static_cast<EditorManipulator *>(node->GetManipulator());
				manipulator->Render(&renderData);
			}

			node = model->GetNextNode(node);
		}
	}
}

void ModelWindow::Move(void)
{
	Window::Move();

	if ((TheInterfaceMgr->GetActiveWindow() == this) && (!TheInterfaceMgr->GetActiveMenu()))
	{
		bool toolCursor = toolTracking;
		if (!toolCursor)
		{
			const Point3D& position = TheInterfaceMgr->GetCursorPosition();
			Vector2D vp = position.GetVector2D() - modelViewport->GetWorldPosition().GetVector2D();
			if ((vp.x >= 0.0F) && (vp.x < modelViewport->GetWidgetSize().x) && (vp.y >= 0.0F) && (vp.y < modelViewport->GetWidgetSize().y))
			{
				toolCursor = true;
			}
		}

		Cursor *cursor = nullptr;
		if (toolCursor)
		{
			int32 tool = (toolTracking) ? trackTool : GetActiveTool();
			int32 cursorIndex = kEditorCursorArrow;

			if (tool == kModelToolOrbit)
			{
				cursorIndex = kEditorCursorCross;
			}
			else if (tool == kModelToolHand)
			{
				cursorIndex = (toolTracking) ? kEditorCursorDrag : kEditorCursorHand;
			}
			else if (tool == kModelToolGlass)
			{
				cursorIndex = kEditorCursorGlass;
			}
			else if (tool == kModelToolLight)
			{
				cursorIndex = kEditorCursorLight;
			}
			else if (tool == kModelToolFree)
			{
				cursorIndex = kEditorCursorFree;
			}

			cursor = TheWorldEditor->GetEditorCursor(cursorIndex);
		}

		TheInterfaceMgr->SetCursor(cursor);
	}

	const AnimationHeader *animationHeader = frameAnimator->GetAnimationHeader();
	if (animationHeader)
	{
		int32 frame = GetAnimationFrame();
		frameSlider->SetValue(frame);
		frameText->SetText(String<7>(frame));

		float x = frameSlider->GetWidgetPosition().x + frameSlider->GetIndicatorPosition();
		frameText->SetWidgetPosition(Point3D(x - frameText->GetWidgetSize().x * 0.5F, frameText->GetWidgetPosition().y, 0.0F));
		frameText->Invalidate();
		frameText->Show();
	}

	unsigned_int32 state = viewerState;
	if (state & kModelViewerUpdateMenus)
	{
		viewerState = state & ~kModelViewerUpdateMenus;

		if ((animationHeader) && (frameAnimator->GetFrameInterpolator()->GetMode() == kInterpolatorStop))
		{
			int32 frame = GetAnimationFrame();

			const CueWidget *cueWidget = cueWidgetList.First();
			while (cueWidget)
			{
				if (cueWidget->GetValue() == frame)
				{
					break;
				}

				cueWidget = cueWidget->Next();
			}

			if (cueWidget)
			{
				cueMenuItem[kCueMenuInsertCue]->Disable();
				cueMenuItem[kCueMenuDeleteCue]->Enable();
				cueMenuItem[kCueMenuGetCueInfo]->Enable();
			}
			else
			{
				cueMenuItem[kCueMenuInsertCue]->Enable();
				cueMenuItem[kCueMenuDeleteCue]->Disable();
				cueMenuItem[kCueMenuGetCueInfo]->Disable();
			}
		}
		else
		{
			cueMenuItem[kCueMenuInsertCue]->Disable();
			cueMenuItem[kCueMenuDeleteCue]->Disable();
			cueMenuItem[kCueMenuGetCueInfo]->Disable();
		}

		if (!cueWidgetList.Empty())
		{
			cueMenuItem[kCueMenuDeleteAllCues]->Enable();
		}
		else
		{
			cueMenuItem[kCueMenuDeleteAllCues]->Disable();
		}
	}
}

void ModelWindow::EnterBackground(void)
{
	Window::EnterBackground();
	TheInterfaceMgr->SetCursor(nullptr);
}

bool ModelWindow::HandleKeyboardEvent(const KeyboardEventData *eventData)
{
	EventType eventType = eventData->eventType;

	if (toolTracking)
	{
		if (trackTool == kModelToolFree)
		{
			unsigned_int32 keyCode = eventData->keyCode;
			if (keyCode - 0x0061 < 26U)
			{
				keyCode -= 0x0020;
			}

			if (eventType == kEventKeyDown)
			{
				if (keyCode == 'W')
				{
					freeCameraFlags |= kFreeCameraForward;
				}
				else if (keyCode == 'S')
				{
					freeCameraFlags |= kFreeCameraBackward;
				}
				else if (keyCode == 'A')
				{
					freeCameraFlags |= kFreeCameraLeft;
				}
				else if (keyCode == 'D')
				{
					freeCameraFlags |= kFreeCameraRight;
				}
				else if (keyCode == ' ')
				{
					freeCameraFlags |= kFreeCameraUp;
				}
				else if (keyCode == 'C')
				{
					freeCameraFlags |= kFreeCameraDown;
				}
			}
			else if (eventType == kEventKeyUp)
			{
				if (keyCode == 'W')
				{
					freeCameraFlags &= ~kFreeCameraForward;
				}
				else if (keyCode == 'S')
				{
					freeCameraFlags &= ~kFreeCameraBackward;
				}
				else if (keyCode == 'A')
				{
					freeCameraFlags &= ~kFreeCameraLeft;
				}
				else if (keyCode == 'D')
				{
					freeCameraFlags &= ~kFreeCameraRight;
				}
				else if (keyCode == ' ')
				{
					freeCameraFlags &= ~kFreeCameraUp;
				}
				else if (keyCode == 'C')
				{
					freeCameraFlags &= ~kFreeCameraDown;
				}
			}
		}

		return (true);
	}

	if (eventType == kEventKeyDown)
	{
		unsigned_int32 keyCode = eventData->keyCode;
		if (keyCode - '0' < 10U)
		{
			if (keyCode == '1')
			{
				toolButton[kModelToolOrbit]->SetValue(1, true);
			}
			else if (keyCode == '2')
			{
				toolButton[kModelToolLight]->SetValue(1, true);
			}
			else if (keyCode == '6')
			{
				toolButton[kModelToolHand]->SetValue(1, true);
			}
			else if (keyCode == '7')
			{
				toolButton[kModelToolGlass]->SetValue(1, true);
			}
			else if (keyCode == '9')
			{
				toolButton[kModelToolFree]->SetValue(1, true);
			}

			return (true);
		}
	}

	return (Window::HandleKeyboardEvent(eventData));
}

void ModelWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		if (widget == frameSlider)
		{
			Interpolator *interpolator = frameAnimator->GetFrameInterpolator();
			float frameDuration = frameAnimator->GetAnimationHeader()->frameDuration;
			interpolator->SetValue(frameSlider->GetValue() * frameDuration);
			animationPage->StopAnimation();

			modelNode->Animate();
			modelNode->StopMotion();

			SetMenuUpdateFlag();
		}
		else if (widget == beginLimit)
		{
			int32 begin = beginLimit->GetValue();
			endLimit->SetLimitRange(begin + 1, endLimit->GetMaxValue());
			float frameDuration = frameAnimator->GetAnimationHeader()->frameDuration;
			frameAnimator->GetFrameInterpolator()->SetRange((float) begin * frameDuration, (float) endLimit->GetValue() * frameDuration);
			UpdateLimitText();
		}
		else if (widget == endLimit)
		{
			int32 end = endLimit->GetValue();
			beginLimit->SetLimitRange(0, end - 1);
			float frameDuration = frameAnimator->GetAnimationHeader()->frameDuration;
			frameAnimator->GetFrameInterpolator()->SetRange((float) beginLimit->GetValue() * frameDuration, (float) end * frameDuration);
			UpdateLimitText();
		}
		else
		{
			for (machine tool = kModelToolOrbit; tool < kModelToolCount; tool++)
			{
				if (widget == toolButton[tool])
				{
					if (currentTool != tool)
					{
						toolButton[currentTool]->SetValue(0);
						currentTool = tool;
					}

					break;
				}
			}
		}
	}
}

void ModelWindow::CloseConfirmDialogComplete(Dialog *dialog, void *cookie)
{
	ModelWindow *modelWindow = static_cast<ModelWindow *>(cookie);

	int32 status = dialog->GetDialogStatus();
	if (status == kDialogOkay)
	{
		modelWindow->HandleSaveAnimationMenuItem(nullptr, nullptr);
		modelWindow->Window::Close();
	}
	else if (status == kDialogIgnore)
	{
		modelWindow->Window::Close();
	}
}

void ModelWindow::DisplayConfirmationDialog(Dialog::CompletionProc *proc)
{
	const StringTable *table = TheWorldEditor->GetStringTable();

	const char *title = table->GetString(StringID('MODL', 'SAVE'));
	const char *okayText = table->GetString(StringID('BTTN', 'SAVE'));
	const char *cancelText = table->GetString(StringID('BTTN', 'CANC'));
	const char *ignoreText = table->GetString(StringID('BTTN', 'DSAV'));

	Dialog *dialog = new Dialog(Vector2D(342.0F, 120.0F), title, okayText, cancelText, ignoreText);
	dialog->SetCompletionProc(proc, this);
	dialog->SetIgnoreKeyCode('n');

	PushButtonWidget *button = dialog->GetIgnoreButton();
	const Point3D& position = button->GetWidgetPosition();
	button->SetWidgetPosition(Point3D(position.x + 8.0F, position.y, position.z));
	button->SetWidgetSize(Vector2D(100.0F, button->GetWidgetSize().y));

	ImageWidget *image = new ImageWidget(Vector2D(64.0F, 64.0F), "C4/warning");
	image->SetWidgetPosition(Point3D(12.0F, 12.0F, 0.0F));
	dialog->AppendSubnode(image);

	String<127> confirmText(table->GetString(StringID('MODL', 'CFM1')));
	confirmText += animationName;
	confirmText += table->GetString(StringID('MODL', 'CFM2'));

	TextWidget *text = new TextWidget(Vector2D(242.0F, 0.0F), confirmText, "font/Gui");
	text->SetTextFlags(kTextWrapped);
	text->SetWidgetPosition(Point3D(88.0F, 16.0F, 0.0F));
	dialog->AppendSubnode(text);

	AddSubwindow(dialog);
}

void ModelWindow::Close(void)
{
	if (viewerState & kModelViewerUnsaved)
	{
		DisplayConfirmationDialog(&CloseConfirmDialogComplete);
	}
	else
	{
		Window::Close();
	}
}

// ZYUQURM
