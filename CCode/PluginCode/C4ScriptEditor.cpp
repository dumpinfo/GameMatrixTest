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


#include "C4ScriptEditor.h"
#include "C4WorldEditor.h"
#include "C4Dialog.h"
#include "C4Paths.h"


using namespace C4;


namespace
{
	enum
	{
		kMaxScriptOperationCount = 20
	};


	enum
	{
		kWidgetPartOutput		= 'OUTP',
		kWidgetPartCurve		= 'CURV'
	};


	const float kMethodBoxWidth = 120.0F;
	const float kMethodBoxHeight = 48.0F;
	const float kMethodBoxThickness = 12.0F;
	const float kMethodBoxPadding = 40.0F;

	const float kOutputDotRadius = 7.5F;

	const float kConditionBoxWidth = 40.0F;
	const float kConditionBoxHeight = 16.0F;

	const float kSectionTitleHeight = 18.0F;
	const float kMinSectionSize = 40.0F;


	const TextureHeader fiberTextureHeader =
	{
		kTexture2D,
		kTextureForceHighQuality,
		kTextureSemanticDiffuse,
		kTextureSemanticTransparency,
		kTextureI8,
		16, 8, 1,
		{kTextureClamp, kTextureRepeat, kTextureClamp},
		5
	};


	const unsigned_int8 fiberTextureImage[171] =
	{
		0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0xFF, 0x40, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xFF, 0xC0, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0xFF, 0xFF, 0xFF, 0x40, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xFF, 0xFF, 0xFF, 0xC0, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x40, 0x00,
		0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0x00,
		0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00,
		0x00, 0xFF, 0x00, 0x00, 0x00, 0x70, 0x10, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x10, 0xF0, 0x80, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x80, 0xFF, 0xF0, 0x10,
		0x00, 0xFF, 0x00, 0x00, 0x70, 0xFF, 0x80, 0x30,
		0xFF, 0x00, 0x5C, 0x24,
		0x00, 0x00, 0xBC, 0x6C,
		0xFF, 0x6A,
		0xFF
	};
}


SharedVertexBuffer MethodWidget::methodIndexBuffer(kVertexBufferIndex | kVertexBufferStatic);
SharedVertexBuffer MethodWidget::backgroundVertexBuffer(kVertexBufferAttribute | kVertexBufferStatic);
SharedVertexBuffer MethodWidget::outputVertexBuffer(kVertexBufferAttribute | kVertexBufferStatic);

SharedVertexBuffer FiberWidget::conditionIndexBuffer(kVertexBufferIndex | kVertexBufferStatic);

SharedVertexBuffer ScriptSectionWidget::sectionIndexBuffer(kVertexBufferIndex | kVertexBufferStatic);

ScriptGraph ScriptEditor::editorClipboard;


ScriptOperation::ScriptOperation()
{
}

ScriptOperation::~ScriptOperation()
{
}


CreateScriptOperation::CreateScriptOperation(MethodWidget *method)
{
	createdMethodList.Append(new MethodReference(method));
}

CreateScriptOperation::CreateScriptOperation(FiberWidget *fiber)
{
	createdFiberList.Append(new FiberReference(fiber)); 
}

CreateScriptOperation::CreateScriptOperation(ScriptSectionWidget *section) 
{
	createdSectionList.Append(new ScriptSectionReference(section)); 
}

CreateScriptOperation::CreateScriptOperation(const List<MethodWidget> *methodList, const List<FiberWidget> *fiberList, const List<ScriptSectionWidget> *sectionList) 
{
	MethodWidget *methodWidget = methodList->First(); 
	while (methodWidget) 
	{
		createdMethodList.Append(new MethodReference(methodWidget));
		methodWidget = methodWidget->Next();
	} 

	FiberWidget *fiberWidget = fiberList->First();
	while (fiberWidget)
	{
		createdFiberList.Append(new FiberReference(fiberWidget));
		fiberWidget = fiberWidget->Next();
	}

	ScriptSectionWidget *sectionWidget = sectionList->First();
	while (sectionWidget)
	{
		createdSectionList.Append(new ScriptSectionReference(sectionWidget));
		sectionWidget = sectionWidget->Next();
	}
}

CreateScriptOperation::~CreateScriptOperation()
{
}

void CreateScriptOperation::Restore(ScriptEditor *scriptEditor)
{
	const FiberReference *fiberReference = createdFiberList.First();
	while (fiberReference)
	{
		scriptEditor->DeleteFiber(fiberReference->GetFiberWidget());
		fiberReference = fiberReference->Next();
	}

	const MethodReference *methodReference = createdMethodList.First();
	while (methodReference)
	{
		scriptEditor->DeleteMethod(methodReference->GetMethodWidget());
		methodReference = methodReference->Next();
	}

	const ScriptSectionReference *sectionReference = createdSectionList.First();
	while (sectionReference)
	{
		scriptEditor->DeleteSection(sectionReference->GetSectionElement());
		sectionReference = sectionReference->Next();
	}
}


DeleteScriptOperation::DeleteScriptOperation(List<MethodWidget> *methodList, List<FiberWidget> *fiberList, List<ScriptSectionWidget> *sectionList)
{
	for (;;)
	{
		MethodWidget *widget = methodList->First();
		if (!widget)
		{
			break;
		}

		deletedMethodList.Append(widget);
	}

	for (;;)
	{
		FiberWidget *widget = fiberList->First();
		if (!widget)
		{
			break;
		}

		deletedFiberList.Append(widget);
	}

	for (;;)
	{
		ScriptSectionWidget *widget = sectionList->First();
		if (!widget)
		{
			break;
		}

		deletedSectionList.Append(widget);
	}
}

DeleteScriptOperation::~DeleteScriptOperation()
{
	const FiberWidget *fiberWidget = deletedFiberList.First();
	while (fiberWidget)
	{
		delete fiberWidget->GetScriptFiber();
		fiberWidget = fiberWidget->Next();
	}

	const MethodWidget *methodWidget = deletedMethodList.First();
	while (methodWidget)
	{
		delete methodWidget->GetScriptMethod();
		methodWidget = methodWidget->Next();
	}

	const ScriptSectionWidget *sectionWidget = deletedSectionList.First();
	while (sectionWidget)
	{
		delete sectionWidget->GetSectionMethod();
		sectionWidget = sectionWidget->Next();
	}
}

void DeleteScriptOperation::Restore(ScriptEditor *scriptEditor)
{
	scriptEditor->UnselectAll();

	for (;;)
	{
		MethodWidget *widget = deletedMethodList.First();
		if (!widget)
		{
			break;
		}

		scriptEditor->ReattachMethod(widget);
	}

	for (;;)
	{
		FiberWidget *widget = deletedFiberList.First();
		if (!widget)
		{
			break;
		}

		scriptEditor->ReattachFiber(widget);
	}

	for (;;)
	{
		ScriptSectionWidget *widget = deletedSectionList.First();
		if (!widget)
		{
			break;
		}

		scriptEditor->ReattachSection(widget);
	}
}


MoveScriptOperation::MoveScriptOperation(const List<MethodWidget> *methodList, const List<ScriptSectionWidget> *sectionList)
{
	MethodWidget *widget = methodList->First();
	while (widget)
	{
		movedMethodList.Append(new MovedMethodReference(widget));
		widget = widget->Next();
	}

	ScriptSectionWidget *sectionWidget = sectionList->First();
	while (sectionWidget)
	{
		movedSectionList.Append(new MovedSectionReference(sectionWidget));
		sectionWidget = sectionWidget->Next();
	}
}

MoveScriptOperation::~MoveScriptOperation()
{
}

MoveScriptOperation::MovedMethodReference::MovedMethodReference(MethodWidget *widget) : MethodReference(widget)
{
	position = widget->GetScriptMethod()->GetMethodPosition();
}

MoveScriptOperation::MovedSectionReference::MovedSectionReference(ScriptSectionWidget *widget) : ScriptSectionReference(widget)
{
	position = widget->GetSectionMethod()->GetMethodPosition();
}

void MoveScriptOperation::Restore(ScriptEditor *scriptEditor)
{
	const MethodReference *reference = movedMethodList.First();
	while (reference)
	{
		const MovedMethodReference *moved = static_cast<const MovedMethodReference *>(reference);

		MethodWidget *widget = moved->GetMethodWidget();
		Method *method = widget->GetScriptMethod();

		const Point2D& p = moved->GetPosition();
		method->SetMethodPosition(Point2D(p.x, p.y));
		widget->SetWidgetPosition(Point3D(p.x, p.y, 0.0F));
		widget->Invalidate();

		scriptEditor->RebuildFiberWidgets(method);

		reference = reference->Next();
	}

	const ScriptSectionReference *sectionReference = movedSectionList.First();
	while (sectionReference)
	{
		const MovedSectionReference *moved = static_cast<const MovedSectionReference *>(sectionReference);

		ScriptSectionWidget *widget = moved->GetSectionElement();
		SectionMethod *section = widget->GetSectionMethod();

		const Point2D& p = moved->GetPosition();
		section->SetMethodPosition(Point2D(p.x, p.y));
		widget->SetWidgetPosition(Point3D(p.x, p.y, 0.0F));
		widget->Invalidate();

		sectionReference = sectionReference->Next();
	}
}


ResizeScriptOperation::ResizeScriptOperation(ScriptSectionWidget *widget)
{
	sectionWidget = widget;

	const SectionMethod *section = widget->GetSectionMethod();
	sectionWidth = section->GetSectionWidth();
	sectionHeight = section->GetSectionHeight();
}

ResizeScriptOperation::~ResizeScriptOperation()
{
}

void ResizeScriptOperation::Restore(ScriptEditor *scriptEditor)
{
	SectionMethod *section = sectionWidget->GetSectionMethod();
	section->SetSectionSize(sectionWidth, sectionHeight);

	sectionWidget->SetWidgetSize(Vector2D(sectionWidth, sectionHeight));
	sectionWidget->Invalidate();
}


FiberScriptOperation::FiberScriptOperation(const List<FiberWidget> *selectionList)
{
	FiberWidget *widget = selectionList->First();
	while (widget)
	{
		fiberList.Append(new CycledReference(widget));
		widget = widget->Next();
	}
}

FiberScriptOperation::~FiberScriptOperation()
{
}

FiberScriptOperation::CycledReference::CycledReference(FiberWidget *widget) : FiberReference(widget)
{
	flags = widget->GetScriptFiber()->GetFiberFlags();
}

void FiberScriptOperation::Restore(ScriptEditor *scriptEditor)
{
	const FiberReference *reference = fiberList.First();
	while (reference)
	{
		const CycledReference *cycled = static_cast<const CycledReference *>(reference);

		FiberWidget *widget = cycled->GetFiberWidget();
		widget->GetScriptFiber()->SetFiberFlags(cycled->GetFlags());
		widget->UpdateContent();

		reference = reference->Next();
	}
}


MethodWidget::MethodWidget(ScriptEditor *editor, Method *method, const MethodRegistration *registration) :
		TextWidget(kWidgetMethod, nullptr, "font/Normal"),
		methodVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		methodRenderable(kRenderIndexedTriangles),
		backgroundColorAttribute(kAttributeMutable),
		backgroundTextureAttribute("ScriptEditor/Graph"),
		backgroundRenderable(kRenderTriangleStrip),
		outputColorAttribute(kAttributeMutable),
		outputTextureAttribute("ScriptEditor/Output"),
		outputRenderable(kRenderTriangleStrip)
{
	scriptEditor = editor;
	scriptMethod = method;
	methodRegistration = registration;

	methodWidgetState = 0;
	viewportScale = 1.0F;

	SetWidgetSize(Vector2D(kMethodBoxWidth, kMethodBoxHeight));
	SetTextAlignment(kTextAlignCenter);
	SetTextFlags(kTextWrapped);
	SetTextLeading(-2.0F);
	SetRenderLineCount(4);

	UpdateText();

	InitRenderable(&methodRenderable);
	methodRenderable.SetVertexCount(16);
	methodRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &methodVertexBuffer, sizeof(MethodVertex));
	methodRenderable.SetVertexAttributeArray(kArrayPosition, 0, 2);
	methodRenderable.SetVertexAttributeArray(kArrayColor, sizeof(Point2D), 4);
	methodVertexBuffer.Establish(sizeof(MethodVertex) * 16);

	if (methodIndexBuffer.Retain() == 1)
	{
		static const Triangle methodTriangle[16] =
		{
			{{ 0,  4,  1}}, {{ 1,  4,  5}}, {{ 1,  5,  2}}, {{ 2,  5,  6}},
			{{ 2,  6,  3}}, {{ 3,  6,  7}}, {{ 3,  7,  0}}, {{ 0,  7,  4}},
			{{ 8, 12,  9}}, {{ 9, 12, 13}}, {{ 9, 13, 10}}, {{10, 13, 14}},
			{{10, 14, 11}}, {{11, 14, 15}}, {{11, 15,  8}}, {{ 8, 15, 12}}
		};

		methodIndexBuffer.Establish(sizeof(Triangle) * 16, methodTriangle);
	}

	methodRenderable.SetPrimitiveCount(8);
	methodRenderable.SetVertexBuffer(kVertexBufferIndexArray, &methodIndexBuffer);

	InitRenderable(&backgroundRenderable);
	backgroundRenderable.SetVertexCount(4);
	backgroundRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &backgroundVertexBuffer, sizeof(BackgroundVertex));
	backgroundRenderable.SetVertexAttributeArray(kArrayPosition, 0, 2);
	backgroundRenderable.SetVertexAttributeArray(kArrayTexcoord, sizeof(Point2D), 2);

	if (backgroundVertexBuffer.Retain() == 1)
	{
		static const BackgroundVertex backgroundVertex[4] =
		{
			{Point2D(-9.0F, -5.0F), Point2D(0.0F, 66.0F)},
			{Point2D(-9.0F, 61.0F), Point2D(0.0F, 0.0F)},
			{Point2D(129.0F, -5.0F), Point2D(138.0F, 66.0F)},
			{Point2D(129.0F, 61.0F), Point2D(138.0F, 0.0F)}
		};

		backgroundVertexBuffer.Establish(sizeof(BackgroundVertex) * 4, backgroundVertex);
	}

	backgroundAttributeList.Append(&backgroundColorAttribute);
	backgroundAttributeList.Append(&backgroundTextureAttribute);
	backgroundRenderable.SetMaterialAttributeList(&backgroundAttributeList);

	InitRenderable(&outputRenderable);
	outputRenderable.SetVertexCount(4);
	outputRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &outputVertexBuffer, sizeof(OutputVertex));
	outputRenderable.SetVertexAttributeArray(kArrayPosition, 0, 2);
	outputRenderable.SetVertexAttributeArray(kArrayTexcoord, sizeof(Point2D), 2);

	if (outputVertexBuffer.Retain() == 1)
	{
		static const OutputVertex outputVertex[4] =
		{
			{Point2D(51.0F, 40.0F), Point2D(0.0F, 17.0F)},
			{Point2D(51.0F, 57.0F), Point2D(0.0F, 0.0F)},
			{Point2D(68.0F, 40.0F), Point2D(17.0F, 17.0F)},
			{Point2D(68.0F, 57.0F), Point2D(17.0F, 0.0F)}
		};

		outputVertexBuffer.Establish(sizeof(OutputVertex) * 4, outputVertex);
	}

	outputAttributeList.Append(&outputColorAttribute);
	outputAttributeList.Append(&outputTextureAttribute);
	outputRenderable.SetMaterialAttributeList(&outputAttributeList);

	UpdateOutputColor(false);
	UpdateColor(kMethodColorNormal);
}

MethodWidget::~MethodWidget()
{
	outputVertexBuffer.Release();
	backgroundVertexBuffer.Release();
	methodIndexBuffer.Release();
}

bool MethodWidget::CalculateBoundingBox(Box2D *box) const
{
	box->min.Set(0.0F, 0.0F);
	box->max.Set(kMethodBoxWidth, kMethodBoxHeight + kOutputDotRadius);
	return (true);
}

void MethodWidget::UpdateOutputColor(bool hilite)
{
	if (hilite)
	{
		const ColorRGB& hiliteColor = TheInterfaceMgr->GetInterfaceColor(kInterfaceColorHilite).GetColorRGB();
		outputColorAttribute.SetDiffuseColor(ColorRGBA(hiliteColor, 1.0F));
	}
	else
	{
		outputColorAttribute.SetDiffuseColor(ColorRGBA(0.9325F, 0.9325F, 0.9325F, 1.0F));
	}
}

void MethodWidget::UpdateColor(int32 index)
{
	static const ConstColorRGBA interiorColor[kMethodColorCount] =
	{
		{1.0F, 1.0F, 1.0F, 1.0F},
		{0.625F, 1.0F, 0.625F, 1.0F},
		{0.625F, 0.875F, 1.0F, 1.0F},
		{1.0F, 0.625F, 0.625F, 1.0F},
		{1.0F, 1.0F, 0.25F, 1.0F}
	};

	backgroundColorAttribute.SetDiffuseColor(interiorColor[index]);
}

void MethodWidget::UpdateText(void)
{
	String<>	text;

	const char *output = scriptMethod->GetOutputValueName();
	if (output[0] != 0)
	{
		text = output;
		text += " = \n";
	}

	bool standard = true;
	MethodType type = scriptMethod->GetMethodType();

	if (type == kMethodEvent)
	{
		const EventMethod *eventMethod = static_cast<EventMethod *>(scriptMethod);
		text += TheWorldEditor->GetStringTable()->GetString(StringID('SCPT', 'EVNT', eventMethod->GetScriptEvent()));
		standard = false;
	}
	else if (type == kMethodExpression)
	{
		const char *expr = static_cast<ExpressionMethod *>(scriptMethod)->GetExpressionText();
		if ((expr) && (expr[0] != 0))
		{
			text += expr;
			standard = false;
		}
	}
	else if (type == kMethodFunction)
	{
		const Function *function = static_cast<FunctionMethod *>(scriptMethod)->GetFunction();
		if (function)
		{
			const ControllerRegistration *controllerRegistration = Controller::FindRegistration(function->GetControllerType());
			const FunctionRegistration *functionRegistration = controllerRegistration->FindFunctionRegistration(function->GetFunctionType());
			text += functionRegistration->GetFunctionName();
			standard = false;
		}
	}

	if (standard)
	{
		text += methodRegistration->GetMethodName();
	}

	const ConnectorKey& key = scriptMethod->GetTargetKey();
	if (key[0] != 0)
	{
		const StringTable *table = TheWorldEditor->GetStringTable();

		text += "\n(";

		if (key[0] == '$')
		{
			text += table->GetString(StringID('SCPT', 'TARG', Text::StringToType(&key[1])));
		}
		else
		{
			text += table->GetString(StringID('SCPT', 'TARG', 'CONN'));
			text += key;
		}

		text += ')';
	}

	SetText(text);
	SplitLines();
	SetTextRenderOffset(Vector3D(0.0F, 21.0F - (float) Min(GetLineCount(), 4) * 6.0F, 0.0F));
}

void MethodWidget::Select(unsigned_int32 state)
{
	methodWidgetState |= kMethodWidgetSelected | state;
	methodRenderable.SetPrimitiveCount(16);
}

void MethodWidget::Unselect(void)
{
	methodWidgetState &= ~(kMethodWidgetSelected | kMethodWidgetTempSelected);
	methodRenderable.SetPrimitiveCount(8);
}

WidgetPart MethodWidget::TestPosition(const Point3D& position) const
{
	float x = position.x - kMethodBoxWidth * 0.5F;
	float y = position.y - kMethodBoxHeight;
	if (x * x + y * y < kOutputDotRadius * kOutputDotRadius)
	{
		return (kWidgetPartOutput);
	}

	return ((position.y < kMethodBoxHeight) ? kWidgetPartInterior : kWidgetPartNone);
}

void MethodWidget::Build(void)
{
	volatile MethodVertex *restrict vertex = methodVertexBuffer.BeginUpdate<MethodVertex>();

	float thickness = (viewportScale < 4.0F) ? viewportScale * 2.0F : viewportScale;

	vertex[0].position.Set(0.0F, 0.0F);
	vertex[1].position.Set(0.0F, kMethodBoxHeight);
	vertex[2].position.Set(kMethodBoxWidth, kMethodBoxHeight);
	vertex[3].position.Set(kMethodBoxWidth, 0.0F);

	vertex[4].position.Set(-thickness, -thickness);
	vertex[5].position.Set(-thickness, kMethodBoxHeight + thickness);
	vertex[6].position.Set(kMethodBoxWidth + thickness, kMethodBoxHeight + thickness);
	vertex[7].position.Set(kMethodBoxWidth + thickness, -thickness);

	vertex[8].position.Set(-thickness, -thickness);
	vertex[9].position.Set(-thickness, kMethodBoxHeight + thickness);
	vertex[10].position.Set(kMethodBoxWidth + thickness, kMethodBoxHeight + thickness);
	vertex[11].position.Set(kMethodBoxWidth + thickness, -thickness);

	thickness = Fmax(kMethodBoxThickness, viewportScale * 4.0F);

	vertex[12].position.Set(-thickness, -thickness);
	vertex[13].position.Set(-thickness, kMethodBoxHeight + thickness);
	vertex[14].position.Set(kMethodBoxWidth + thickness, kMethodBoxHeight + thickness);
	vertex[15].position.Set(kMethodBoxWidth + thickness, -thickness);

	for (machine a = 0; a < 8; a++)
	{
		vertex[a].color.Set(0.0F, 0.0F, 0.0F, 1.0F);
	}

	const ColorRGB& hiliteColor = TheInterfaceMgr->GetInterfaceColor(kInterfaceColorHilite).GetColorRGB();
	for (machine a = 8; a < 16; a++)
	{
		vertex[a].color.Set(hiliteColor, 0.625F);
	}

	methodVertexBuffer.EndUpdate();

	TextWidget::Build();
}

void MethodWidget::Render(List<Renderable> *renderList)
{
	renderList->Append(&backgroundRenderable);
	renderList->Append(&methodRenderable);
	renderList->Append(&outputRenderable);
	TextWidget::Render(renderList);
}


FiberWidget::FiberWidget(ScriptEditor *editor, Fiber *fiber) :
		RenderableWidget(kWidgetFiber, kRenderTriangleStrip),
		fiberVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		fiberColorAttribute(ColorRGBA(0.0F, 0.0F, 0.0F, 1.0F), kAttributeMutable),
		fiberTextureAttribute(&fiberTextureHeader, fiberTextureImage),
		selectionVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		selectionColorAttribute(kAttributeMutable),
		selectionTextureAttribute(&fiberTextureHeader, fiberTextureImage),
		selectionRenderable(kRenderTriangleStrip),
		conditionVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		conditionRenderable(kRenderIndexedTriangles)
{
	scriptEditor = editor;
	scriptFiber = fiber;

	fiberWidgetState = 0;
	fiberHiliteCount = 0;

	viewportScale = 1.0F;
	conditionText = nullptr;

	SetAmbientBlendState(kBlendPremultInterp);
	SetShaderFlags(kShaderAmbientEffect | kShaderVertexPolyboard | kShaderLinearPolyboard | kShaderOrthoPolyboard);

	SetVertexCount(70);
	SetVertexBuffer(kVertexBufferAttributeArray, &fiberVertexBuffer, sizeof(FiberVertex));
	SetVertexAttributeArray(kArrayPosition, 0, 2);
	SetVertexAttributeArray(kArrayTangent, sizeof(Point2D), 4);
	SetVertexAttributeArray(kArrayTexcoord, sizeof(Point2D) + sizeof(Vector4D), 2);
	fiberVertexBuffer.Establish(sizeof(FiberVertex) * 70);

	fiberAttributeList.Append(&fiberColorAttribute);
	fiberAttributeList.Append(&fiberTextureAttribute);
	SetMaterialAttributeList(&fiberAttributeList);

	selectionRenderable.SetAmbientBlendState(kBlendInterpolate);
	selectionRenderable.SetShaderFlags(kShaderAmbientEffect | kShaderVertexPolyboard | kShaderLinearPolyboard | kShaderOrthoPolyboard);

	selectionRenderable.SetVertexCount(70);
	selectionRenderable.SetVertexBuffer(kVertexBufferAttributeArray0, &fiberVertexBuffer, sizeof(FiberVertex));
	selectionRenderable.SetVertexBuffer(kVertexBufferAttributeArray1, &selectionVertexBuffer, sizeof(SelectionVertex));
	selectionRenderable.SetVertexBufferArrayFlags((1 << kArrayTangent) | (1 << kArrayTexcoord));
	selectionRenderable.SetVertexAttributeArray(kArrayPosition, 0, 2);
	selectionRenderable.SetVertexAttributeArray(kArrayTangent, 0, 4);
	selectionRenderable.SetVertexAttributeArray(kArrayTexcoord, sizeof(Vector4D), 2);
	selectionVertexBuffer.Establish(sizeof(SelectionVertex) * 70);

	selectionAttributeList.Append(&selectionColorAttribute);
	selectionAttributeList.Append(&selectionTextureAttribute);
	selectionRenderable.SetMaterialAttributeList(&selectionAttributeList);

	conditionRenderable.SetVertexCount(12);
	conditionRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &conditionVertexBuffer, sizeof(ConditionVertex));
	conditionRenderable.SetVertexAttributeArray(kArrayPosition, 0, 2);
	conditionRenderable.SetVertexAttributeArray(kArrayColor, sizeof(Point2D), 3);
	conditionVertexBuffer.Establish(sizeof(ConditionVertex) * 12);

	if (conditionIndexBuffer.Retain() == 1)
	{
		static const Triangle conditionTriangle[10] =
		{
			{{ 0,  1,  2}}, {{ 0,  2,  3}}, {{ 4,  8,  5}}, {{ 5,  8,  9}}, {{ 5,  9,  6}},
			{{ 6,  9, 10}}, {{ 6, 10,  7}}, {{ 7, 10, 11}}, {{ 7, 11,  4}}, {{ 4, 11,  8}}
		};

		conditionIndexBuffer.Establish(sizeof(Triangle) * 10, conditionTriangle);
	}

	conditionRenderable.SetPrimitiveCount(10);
	conditionRenderable.SetVertexBuffer(kVertexBufferIndexArray, &conditionIndexBuffer);

	UpdateContent();
}

FiberWidget::~FiberWidget()
{
	conditionIndexBuffer.Release();
}

bool FiberWidget::CalculateBoundingBox(Box2D *box) const
{
	if (GetBuildFlag())
	{
		return (false);
	}

	float xmin = fiberVertex[0].position.x;
	float xmax = fiberVertex[0].position.x;
	float ymin = fiberVertex[0].position.y;
	float ymax = fiberVertex[0].position.y;

	for (machine a = 2; a < 70; a += 2)
	{
		const Point2D& p = fiberVertex[a].position;
		xmin = Fmin(xmin, p.x);
		xmax = Fmax(xmax, p.x);
		ymin = Fmin(ymin, p.y);
		ymax = Fmax(ymax, p.y);
	}

	box->min.Set(xmin - 4.0F, ymin - 4.0F);
	box->max.Set(xmax + 4.0F, ymax + 4.0F);
	return (true);
}

void FiberWidget::UpdateContent(void)
{
	unsigned_int32 flags = scriptFiber->GetFiberFlags();
	if (flags & (kFiberConditionTrue | kFiberConditionFalse))
	{
		const char		*string;

		if (flags & kFiberConditionTrue)
		{
			fiberColorAttribute.SetDiffuseColor(ColorRGBA(0.0F, 0.75F, 0.0F, 1.0F));
			string = "T";
		}
		else
		{
			fiberColorAttribute.SetDiffuseColor(ColorRGBA(0.75F, 0.0F, 0.0F, 1.0F));
			string = "F";
		}

		if (conditionText)
		{
			conditionText->SetText(string);
		}
		else
		{
			conditionText = new TextWidget(Vector2D(kConditionBoxWidth, kConditionBoxHeight), string, "font/Normal");
			conditionText->SetTextAlignment(kTextAlignCenter);
			conditionText->SetWidgetState(kWidgetDisabled);
			AppendNewSubnode(conditionText);
		}

		SetBuildFlag();
	}
	else
	{
		fiberColorAttribute.SetDiffuseColor(K::black);

		delete conditionText;
		conditionText = nullptr;
	}
}

void FiberWidget::Select(void)
{
	fiberWidgetState |= kFiberWidgetSelected;
	selectionColorAttribute.SetDiffuseColor(TheInterfaceMgr->GetInterfaceColor(kInterfaceColorHilite));
}

void FiberWidget::Unselect(void)
{
	unsigned_int32 state = fiberWidgetState;
	fiberWidgetState = state & ~kFiberWidgetSelected;

	if (state & kFiberWidgetHilited)
	{
		selectionColorAttribute.SetDiffuseColor(ColorRGBA(0.75F, 0.75F, 0.75F, 1.0F));
	}
}

void FiberWidget::Hilite(void)
{
	unsigned_int32 state = fiberWidgetState;
	fiberWidgetState = state | kFiberWidgetHilited;
	fiberHiliteCount++;

	if (!(state & kFiberWidgetSelected))
	{
		selectionColorAttribute.SetDiffuseColor(ColorRGBA(0.75F, 0.75F, 0.75F, 1.0F));
	}
}

void FiberWidget::Unhilite(void)
{
	if (--fiberHiliteCount == 0)
	{
		fiberWidgetState &= ~kFiberWidgetHilited;
	}
}

void FiberWidget::HandleDelete(void)
{
	fiberWidgetState &= ~(kFiberWidgetSelected | kFiberWidgetHilited);
	fiberHiliteCount = 0;
}

WidgetPart FiberWidget::TestPosition(const Point3D& position) const
{
	for (machine a = 0; a <= 66; a += 2)
	{
		if (a != 64)
		{
			Point3D q1 = fiberVertex[a].position;
			Point3D q2 = fiberVertex[a + 2].position;
			Vector3D dq = q2 - q1;

			if (Math::SquaredDistancePointToLine(position, q1, dq) < 16.0F)
			{
				float m = SquaredMag(dq);
				float d = (position - q1) * dq * InverseSqrt(m);
				if ((d >= 0.0F) && (d * d < m))
				{
					return (kWidgetPartCurve);
				}
			}
		}
	}

	return (kWidgetPartNone);
}

void FiberWidget::Build(void)
{
	Invalidate();

	bool looping = ((scriptFiber->GetFiberFlags() & kFiberLooping) != 0);

	Point2D p1 = scriptFiber->GetStartElement()->GetMethodPosition() + Vector2D(kMethodBoxWidth * 0.5F, kMethodBoxHeight + 2.0F);
	Point2D p5 = scriptFiber->GetFinishElement()->GetMethodPosition() + Vector2D(kMethodBoxWidth * 0.5F, -2.0F);
	Point2D texpoint = p1;

	float dx = (1.0F - Exp(Fnabs(p5.x - p1.x) * 0.01F)) * (kMethodBoxWidth * 0.25F);
	float dy = (p5.y - p1.y - 8.0F) * K::one_over_3;

	float f = (p1.x < p5.x) ? 1.0F : -1.0F;
	dx *= f;

	if (dy >= 8.0F)
	{
		p5.x -= dx;

		Point2D p2(p1.x + dx, p1.y + dy);
		Point2D p3(p5.x - dx, p5.y - dy);

		Vector2D tangent = Normalize(p5 - p3);
		Point2D p4 = p5 - tangent * 9.0F;

		fiberVertex[68].position = p5;
		fiberVertex[68].tangent.Set(tangent.x, tangent.y, 0.0F, -3.0F);
		fiberVertex[68].texcoord.Set(0.5, 0.0625F);

		fiberVertex[69].position = p5;
		fiberVertex[69].tangent.Set(tangent.x, tangent.y, 0.0F, 4.0F);
		fiberVertex[69].texcoord.Set(0.9375F, 0.0625F);

		BezierPathComponent path(p1, p2, p3, p4);

		float u = 0.0F;
		float texcoord = 0.0F;

		for (machine a = 0; a <= 64; a += 2)
		{
			Point3D p = path.BezierPathComponent::GetPosition(u);
			Vector3D t = path.BezierPathComponent::GetTangent(u);
			t.Normalize();

			fiberVertex[a].position = p.GetPoint2D();
			fiberVertex[a + 1].position = p.GetPoint2D();
			fiberVertex[a].tangent.Set(t, -3.0F);
			fiberVertex[a + 1].tangent.Set(t, 4.0F);

			if (looping)
			{
				texcoord += Magnitude(p.GetPoint2D() - texpoint) * 0.0625F;
				texpoint = p.GetPoint2D();
			}

			fiberVertex[a].texcoord.Set(0.0F, texcoord);
			fiberVertex[a + 1].texcoord.Set(0.4375F, texcoord);

			u += 0.03125F;
		}
	}
	else
	{
		Point2D		p2, p3, pa, pb;

		if (dy < Fnabs(p1.x - p5.x) * 0.25F)
		{
			p5.x -= dx;
			f *= kMethodBoxWidth;

			p2.Set(p1.x - f, p1.y + kMethodBoxHeight);
			p3.Set(p5.x - f, p5.y - kMethodBoxHeight);

			pa.Set(p2.x - f, p1.y);
			pb.Set(p3.x - f, p5.y);
		}
		else
		{
			p5.x -= dx;

			p2.Set(p1.x + dx, p1.y + kMethodBoxHeight);
			p3.Set(p5.x - dx, p5.y - kMethodBoxHeight);

			pa.Set(p2.x + dx, p2.y);
			pb.Set(p3.x - dx, p3.y);
		}

		Point2D pc = (pa + pb) * 0.5F;

		Vector2D tangent = Normalize(p5 - p3);
		Point2D p4 = p5 - tangent * 9.0F;

		fiberVertex[68].position = p5;
		fiberVertex[68].tangent.Set(tangent.x, tangent.y, 0.0F, -3.0F);
		fiberVertex[68].texcoord.Set(0.5, 0.0625F);

		fiberVertex[69].position = p5;
		fiberVertex[69].tangent.Set(tangent.x, tangent.y, 0.0F, 4.0F);
		fiberVertex[69].texcoord.Set(0.9375F, 0.0625F);

		BezierPathComponent path1(p1, p2, pa, pc);
		BezierPathComponent path2(pc, pb, p3, p4);

		float u = 0.0F;
		float texcoord = 0.0F;

		for (machine a = 0; a < 32; a += 2)
		{
			Point3D p = path1.BezierPathComponent::GetPosition(u);
			Vector3D t = path1.BezierPathComponent::GetTangent(u);
			t.Normalize();

			fiberVertex[a].position = p.GetPoint2D();
			fiberVertex[a + 1].position = p.GetPoint2D();
			fiberVertex[a].tangent.Set(t, -3.0F);
			fiberVertex[a + 1].tangent.Set(t, 4.0F);

			if (looping)
			{
				texcoord += Magnitude(p.GetPoint2D() - texpoint) * 0.0625F;
				texpoint = p.GetPoint2D();
			}

			fiberVertex[a].texcoord.Set(0.0F, texcoord);
			fiberVertex[a + 1].texcoord.Set(0.4375F, texcoord);

			u += 0.0625F;
		}

		u = 0.0F;
		for (machine a = 32; a <= 64; a += 2)
		{
			Point3D p = path2.BezierPathComponent::GetPosition(u);
			Vector3D t = path2.BezierPathComponent::GetTangent(u);
			t.Normalize();

			fiberVertex[a].position = p.GetPoint2D();
			fiberVertex[a + 1].position = p.GetPoint2D();
			fiberVertex[a].tangent.Set(t, -3.0F);
			fiberVertex[a + 1].tangent.Set(t, 4.0F);

			if (looping)
			{
				texcoord += Magnitude(p.GetPoint2D() - texpoint) * 0.0625F;
				texpoint = p.GetPoint2D();
			}

			fiberVertex[a].texcoord.Set(0.0F, texcoord);
			fiberVertex[a + 1].texcoord.Set(0.4375F, texcoord);

			u += 0.0625F;
		}
	}

	fiberVertex[66].position = fiberVertex[64].position;
	fiberVertex[66].tangent = fiberVertex[64].tangent;
	fiberVertex[66].texcoord.Set(0.5, 0.9375F);

	fiberVertex[67].position = fiberVertex[65].position;
	fiberVertex[67].tangent = fiberVertex[65].tangent;
	fiberVertex[67].texcoord.Set(0.9375F, 0.9375F);

	fiberVertexBuffer.UpdateBuffer(0, sizeof(FiberVertex) * 70, fiberVertex);

	volatile SelectionVertex *restrict selectionVertex = selectionVertexBuffer.BeginUpdate<SelectionVertex>();

	for (machine a = 0; a < 70; a += 2)
	{
		selectionVertex[a].tangent.Set(fiberVertex[a].tangent.GetVector3D(), -15.0F);
		selectionVertex[a].texcoord.Set(0.0F, 0.0F);
		selectionVertex[a + 1].tangent.Set(fiberVertex[a + 1].tangent.GetVector3D(), 16.0F);
		selectionVertex[a + 1].texcoord.Set(0.4375F, 0.0F);
	}

	selectionVertexBuffer.EndUpdate();

	if (conditionText)
	{
		volatile ConditionVertex *restrict conditionVertex = conditionVertexBuffer.BeginUpdate<ConditionVertex>();

		float thickness = viewportScale;
		float x = Floor(fiberVertex[32].position.x) - kConditionBoxWidth * 0.5F;
		float y = Floor(fiberVertex[32].position.y) - kConditionBoxHeight * 0.5F;

		conditionVertex[0].position.Set(x, y);
		conditionVertex[1].position.Set(x, y + kConditionBoxHeight);
		conditionVertex[2].position.Set(x + kConditionBoxWidth, y + kConditionBoxHeight);
		conditionVertex[3].position.Set(x + kConditionBoxWidth, y);

		conditionVertex[4].position.Set(x, y);
		conditionVertex[5].position.Set(x, y + kConditionBoxHeight);
		conditionVertex[6].position.Set(x + kConditionBoxWidth, y + kConditionBoxHeight);
		conditionVertex[7].position.Set(x + kConditionBoxWidth, y);

		conditionVertex[8].position.Set(x - thickness, y - thickness);
		conditionVertex[9].position.Set(x - thickness, y + kConditionBoxHeight + thickness);
		conditionVertex[10].position.Set(x + kConditionBoxWidth + thickness, y + kConditionBoxHeight + thickness);
		conditionVertex[11].position.Set(x + kConditionBoxWidth + thickness, y - thickness);

		for (machine a = 0; a < 4; a++)
		{
			conditionVertex[a].color.Set(0.875F, 0.875F, 0.875F);
		}

		for (machine a = 4; a < 12; a++)
		{
			conditionVertex[a].color.Set(0.0F, 0.0F, 0.0F);
		}

		conditionVertexBuffer.EndUpdate();

		conditionText->SetWidgetPosition(Point3D(x, y + (kConditionBoxHeight * 0.5F - 6.0F), 0.0F));
		conditionText->Update();
	}
}

void FiberWidget::Render(List<Renderable> *renderList)
{
	if (fiberWidgetState & (kFiberWidgetSelected | kFiberWidgetHilited))
	{
		renderList->Append(&selectionRenderable);
	}

	RenderableWidget::Render(renderList);

	if (conditionText)
	{
		renderList->Append(&conditionRenderable);
	}
}


ScriptSectionWidget::ScriptSectionWidget(ScriptEditor *editor, SectionMethod *method) :
		TextWidget(kWidgetScriptSection, method->GetSectionComment(), "font/Gui"),
		sectionVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		sectionRenderable(kRenderIndexedTriangles)
{
	scriptEditor = editor;
	sectionMethod = method;

	sectionWidgetState = 0;
	viewportScale = 1.0F;

	float width = Fmax(method->GetSectionWidth(), kMinSectionSize);
	float height = Fmax(method->GetSectionHeight(), kMinSectionSize);
	SetWidgetSize(Vector2D(width, height));

	SetTextFlags(kTextClipped);
	SetTextFormatExclusionMask(0);
	SetTextAlignment(kTextAlignCenter);
	SetTextRenderOffset(Vector3D(1.0F, 2.0F, 0.0F));

	sectionRenderable.SetAmbientBlendState(kBlendInterpolate);
	sectionRenderable.SetTransformable(this);

	sectionRenderable.SetVertexCount(27);
	sectionRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &sectionVertexBuffer, sizeof(SectionVertex));
	sectionRenderable.SetVertexAttributeArray(kArrayPosition, 0, 2);
	sectionRenderable.SetVertexAttributeArray(kArrayColor, sizeof(Point2D), 4);
	sectionVertexBuffer.Establish(sizeof(SectionVertex) * 27);

	if (sectionIndexBuffer.Retain() == 1)
	{
		static const Triangle sectionTriangle[21] =
		{
			{{ 0,  1,  2}}, {{ 0,  2,  3}}, {{ 4,  5,  6}}, {{ 4,  6,  7}},
			{{ 8, 12,  9}}, {{ 9, 12, 13}}, {{ 9, 13, 10}}, {{10, 13, 14}},
			{{10, 14, 11}}, {{11, 14, 15}}, {{11, 15,  8}}, {{ 8, 15, 12}},
			{{16, 17, 18}},
			{{19, 23, 20}}, {{20, 23, 24}}, {{20, 24, 21}}, {{21, 24, 25}},
			{{21, 25, 22}}, {{22, 25, 26}}, {{22, 26, 19}}, {{19, 26, 23}}
		};

		sectionIndexBuffer.Establish(sizeof(Triangle) * 21, sectionTriangle);
	}

	sectionRenderable.SetPrimitiveCount(13);
	sectionRenderable.SetVertexBuffer(kVertexBufferIndexArray, &sectionIndexBuffer);
}

ScriptSectionWidget::~ScriptSectionWidget()
{
	sectionIndexBuffer.Release();
}

void ScriptSectionWidget::UpdateContent(void)
{
	const char *comment = sectionMethod->GetSectionComment();
	if ((comment) && (comment[0] != 0))
	{
		SetText(comment);
	}
	else
	{
		SetText(nullptr);
	}
}

void ScriptSectionWidget::Select(void)
{
	sectionWidgetState |= kScriptSectionWidgetSelected;
	sectionRenderable.SetPrimitiveCount(21);
}

void ScriptSectionWidget::Unselect(void)
{
	sectionWidgetState &= ~kScriptSectionWidgetSelected;
	sectionRenderable.SetPrimitiveCount(13);
}

WidgetPart ScriptSectionWidget::TestPosition(const Point3D& position) const
{
	if (position.y < kSectionTitleHeight)
	{
		return (kWidgetPartTitle);
	}

	if ((position.x > GetWidgetSize().x - 8.0F) && (position.y > GetWidgetSize().y - 8.0F))
	{
		return (kWidgetPartResize);
	}

	return (kWidgetPartNone);
}

void ScriptSectionWidget::Build(void)
{
	volatile SectionVertex *restrict sectionVertex = sectionVertexBuffer.BeginUpdate<SectionVertex>();

	float width = GetWidgetSize().x;
	float height = GetWidgetSize().y;

	sectionVertex[0].position.Set(0.0F, 0.0F);
	sectionVertex[1].position.Set(0.0F, kSectionTitleHeight);
	sectionVertex[2].position.Set(width, kSectionTitleHeight);
	sectionVertex[3].position.Set(width, 0.0F);

	sectionVertex[4].position.Set(0.0F, kSectionTitleHeight);
	sectionVertex[5].position.Set(0.0F, height);
	sectionVertex[6].position.Set(width, height);
	sectionVertex[7].position.Set(width, kSectionTitleHeight);

	sectionVertex[8].position.Set(0.0F, 0.0F);
	sectionVertex[9].position.Set(0.0F, height);
	sectionVertex[10].position.Set(width, height);
	sectionVertex[11].position.Set(width, 0.0F);

	sectionVertex[12].position.Set(-viewportScale, -viewportScale);
	sectionVertex[13].position.Set(-viewportScale, height + viewportScale);
	sectionVertex[14].position.Set(width + viewportScale, height + viewportScale);
	sectionVertex[15].position.Set(width + viewportScale, -viewportScale);

	sectionVertex[16].position.Set(width - 8.0F, height);
	sectionVertex[17].position.Set(width, height);
	sectionVertex[18].position.Set(width, height - 8.0F);

	sectionVertex[19].position.Set(-viewportScale, -viewportScale);
	sectionVertex[20].position.Set(-viewportScale, height + viewportScale);
	sectionVertex[21].position.Set(width + viewportScale, height + viewportScale);
	sectionVertex[22].position.Set(width + viewportScale, -viewportScale);

	float thickness = Fmax(kMethodBoxThickness, viewportScale * 4.0F);

	sectionVertex[23].position.Set(-thickness, -thickness);
	sectionVertex[24].position.Set(-thickness, height + thickness);
	sectionVertex[25].position.Set(width + thickness, height + thickness);
	sectionVertex[26].position.Set(width + thickness, -thickness);

	for (machine a = 0; a < 4; a++)
	{
		sectionVertex[a].color.Set(0.75F, 0.75F, 0.75F, 1.0F);
	}

	const ColorRGBA& sectionColor = sectionMethod->GetSectionColor();
	for (machine a = 4; a < 8; a++)
	{
		sectionVertex[a].color = sectionColor;
	}

	for (machine a = 8; a < 16; a++)
	{
		sectionVertex[a].color.Set(0.0F, 0.0F, 0.0F, 1.0F);
	}

	for (machine a = 16; a < 19; a++)
	{
		sectionVertex[a].color.Set(0.625F, 0.625F, 0.625F, 1.0F);
	}

	const ColorRGB& hiliteColor = TheInterfaceMgr->GetInterfaceColor(kInterfaceColorHilite).GetColorRGB();
	for (machine a = 19; a < 27; a++)
	{
		sectionVertex[a].color.Set(hiliteColor, 0.625F);
	}

	sectionVertexBuffer.EndUpdate();

	TextWidget::Build();
}

void ScriptSectionWidget::Render(List<Renderable> *renderList)
{
	renderList->Append(&sectionRenderable);
	TextWidget::Render(renderList);
}


EditorScriptGraph::EditorScriptGraph(ScriptEditor *editor)
{
	scriptEditor = editor;

	viewportCameraPosition.Set(0.0F, 0.0F, 0.0F);
	viewportOrthoScale = 1.0F;
	viewportInitFlag = false;

	graphRoot = new Widget;
	sectionRoot = new Widget;
}

EditorScriptGraph::EditorScriptGraph(ScriptEditor *editor, const ScriptGraph& scriptGraph) : ScriptGraph(scriptGraph)
{
	scriptEditor = editor;

	viewportCameraPosition.Set(0.0F, 0.0F, 0.0F);
	viewportOrthoScale = 1.0F;
	viewportInitFlag = true;

	graphRoot = new Widget;
	sectionRoot = new Widget;

	BuildScriptGraph();
}

EditorScriptGraph::~EditorScriptGraph()
{
	delete sectionRoot;
	delete graphRoot;
}

int32 EditorScriptGraph::GetSettingCount(void) const
{
	return (((scriptEditor->GetEditorState() & kScriptEditorWidget) == 0) ? 3 : 5);
}

Setting *EditorScriptGraph::GetSetting(int32 index) const
{
	const StringTable *table = TheWorldEditor->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('SCPT', 'SCPT', 'NAME'));
		return (new TextSetting('NAME', GetScriptName(), title, kMaxScriptNameLength));
	}

	if (!(scriptEditor->GetEditorState() & kScriptEditorWidget))
	{
		if (index < 3)
		{
			static const EventType scriptEvent[2] =
			{
				kEventControllerActivate, kEventControllerDeactivate
			};

			EventType eventType = scriptEvent[index - 1];
			const char *title = table->GetString(StringID('SCPT', 'SCPT', eventType));
			return (new BooleanSetting(eventType, HandlesEvent(eventType), title));
		}
	}
	else
	{
		if (index < 5)
		{
			static const EventType scriptEvent[4] =
			{
				kEventWidgetActivate, kEventWidgetChange, kEventWidgetBeginHover, kEventWidgetEndHover
			};

			EventType eventType = scriptEvent[index - 1];
			const char *title = table->GetString(StringID('SCPT', 'SCPT', eventType));
			return (new BooleanSetting(eventType, HandlesEvent(eventType), title));
		}
	}

	return (nullptr);
}

void EditorScriptGraph::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'NAME')
	{
		SetScriptName(static_cast<const TextSetting *>(setting)->GetText());
	}
	else
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			GetScriptEventArray()->AddElement(identifier);
		}
	}
}

void *EditorScriptGraph::BeginSettings(void)
{
	GetScriptEventArray()->Purge();
	return (nullptr);
}

void EditorScriptGraph::BuildScriptGraph(void)
{
	Method *method = GetFirstElement();
	while (method)
	{
		MethodType type = method->GetMethodType();
		if (type != kMethodSection)
		{
			MethodWidget *widget = new MethodWidget(scriptEditor, method, Method::FindRegistration(type));
			graphRoot->AppendSubnode(widget);
			methodWidgetList.Append(widget);

			Point3D position = method->GetMethodPosition();
			widget->SetWidgetPosition(position);
		}
		else
		{
			SectionMethod *section = static_cast<SectionMethod *>(method);
			ScriptSectionWidget *widget = new ScriptSectionWidget(scriptEditor, section);
			sectionRoot->AppendSubnode(widget);
			sectionWidgetList.Append(widget);

			Point3D position = method->GetMethodPosition();
			widget->SetWidgetPosition(position);
			widget->SetWidgetSize(Vector2D(section->GetSectionWidth(), section->GetSectionHeight()));
		}

		method = method->GetNextElement();
	}

	method = GetFirstElement();
	while (method)
	{
		Fiber *fiber = method->GetFirstIncomingEdge();
		while (fiber)
		{
			FiberWidget *widget = new FiberWidget(scriptEditor, fiber);
			graphRoot->PrependSubnode(widget);
			fiberWidgetList.Append(widget);

			fiber = fiber->GetNextIncomingEdge();
		}

		method = method->GetNextElement();
	}

	graphRoot->Preprocess();
	sectionRoot->Preprocess();

	graphRoot->Update();
	sectionRoot->Update();
}

void EditorScriptGraph::SaveViewportCameraPosition(const OrthoViewportWidget *viewport)
{
	viewportCameraPosition = viewport->GetViewportCamera()->GetNodePosition();
	viewportOrthoScale = viewport->GetOrthoScale().x;
}

void EditorScriptGraph::RestoreViewportCameraPosition(OrthoViewportWidget *viewport)
{
	if (viewportInitFlag)
	{
		Box2D	box;

		viewportInitFlag = false;
		scriptEditor->AddEditorState(kScriptEditorUpdateGraph);

		bool firstMethod = true;
		const Widget *widget = graphRoot->GetFirstSubnode();
		if (widget)
		{
			do
			{
				if (widget->GetWidgetType() == kWidgetMethod)
				{
					const Point3D& position = widget->GetWidgetPosition();

					if (firstMethod)
					{
						firstMethod = false;
						box.Set(position.GetPoint2D(), Point2D(position.x + kMethodBoxWidth, position.y + kMethodBoxHeight));
					}
					else
					{
						box.Union(position.GetPoint2D());
						box.Union(Point2D(position.x + kMethodBoxWidth, position.y + kMethodBoxHeight));
					}
				}

				widget = widget->Next();
			} while (widget);

			viewportCameraPosition.Set(Floor((box.min.x + box.max.x) * 0.5F), Floor(box.min.y + (viewport->GetWidgetSize().y - kMethodBoxHeight) * 0.5F), 0.0F);
		}
	}

	viewport->GetViewportCamera()->SetNodePosition(viewportCameraPosition);
	scriptEditor->UpdateViewportScale(viewportOrthoScale);
}


ScriptPage::ScriptPage(ScriptEditor *editor, const char *panelName) : Page(panelName)
{
	scriptEditor = editor;
}

ScriptPage::~ScriptPage()
{
}


ScriptGraphsPage::ScriptGraphsPage(ScriptEditor *editor) :
		ScriptPage(editor, "ScriptEditor/Scripts"),
		listWidgetObserver(this, &ScriptGraphsPage::HandleListWidgetEvent),
		newButtonObserver(this, &ScriptGraphsPage::HandleNewButtonEvent),
		deleteButtonObserver(this, &ScriptGraphsPage::HandleDeleteButtonEvent)
{
}

ScriptGraphsPage::~ScriptGraphsPage()
{
}

ScriptGraphsPage::GraphWidget::GraphWidget(const Vector2D& size, EditorScriptGraph *graph) : TextWidget(size, nullptr, "font/Normal")
{
	scriptGraph = graph;

	const char *name = graph->GetScriptName();
	if (name[0] != 0)
	{
		SetText(name);
	}
	else
	{
		SetText(TheWorldEditor->GetStringTable()->GetString(StringID('SCPT', 'UNAM')));
	}
}

ScriptGraphsPage::GraphWidget::~GraphWidget()
{
}

void ScriptGraphsPage::Preprocess(void)
{
	ScriptPage::Preprocess();

	listWidget = static_cast<ListWidget *>(FindWidget("List"));
	listWidget->SetObserver(&listWidgetObserver);

	newButton = static_cast<IconButtonWidget *>(FindWidget("New"));
	deleteButton = static_cast<IconButtonWidget *>(FindWidget("Delete"));
	newButton->SetObserver(&newButtonObserver);
	deleteButton->SetObserver(&deleteButtonObserver);

	BuildGraphList();
}

void ScriptGraphsPage::BuildGraphList(void)
{
	listWidget->PurgeListItems();

	int32 count = 0;
	const ScriptGraph *currentGraph = GetScriptEditor()->GetCurrentScriptGraph();
	Vector2D size = listWidget->GetNaturalListItemSize();

	ScriptGraph *graph = GetScriptEditor()->GetScriptGraphList()->First();
	while (graph)
	{
		listWidget->AppendListItem(new GraphWidget(size, static_cast<EditorScriptGraph *>(graph)));

		if (graph == currentGraph)
		{
			listWidget->SelectListItem(count);
		}

		count++;
		graph = graph->Next();
	}

	if (count > 1)
	{
		deleteButton->Enable();
	}
	else
	{
		deleteButton->Disable();
	}
}

void ScriptGraphsPage::HandleListWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	EventType eventType = eventData->eventType;

	if (eventType == kEventWidgetActivate)
	{
		ScriptEditor *scriptEditor = GetScriptEditor();
		const GraphWidget *graphWidget = static_cast<GraphWidget *>(listWidget->GetFirstSelectedListItem());
		scriptEditor->AddSubwindow(new GraphInfoWindow(scriptEditor, graphWidget->scriptGraph));
	}
	else if (eventType == kEventWidgetChange)
	{
		const GraphWidget *graphWidget = static_cast<GraphWidget *>(listWidget->GetFirstSelectedListItem());
		GetScriptEditor()->SetCurrentScriptGraph(graphWidget->scriptGraph);
	}
}

void ScriptGraphsPage::HandleNewButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		ScriptEditor *scriptEditor = GetScriptEditor();
		GraphInfoWindow *window = new GraphInfoWindow(scriptEditor);
		window->SetCompletionProc(&NewGraphComplete, this);
		scriptEditor->AddSubwindow(window);
	}
}

void ScriptGraphsPage::HandleDeleteButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		const GraphWidget *graphWidget = static_cast<GraphWidget *>(listWidget->GetFirstSelectedListItem());
		if (graphWidget)
		{
			const StringTable *table = TheWorldEditor->GetStringTable();

			const char *title = table->GetString(StringID('SCPT', 'DELT'));
			const char *okayText = table->GetString(StringID('BTTN', 'DELT'));
			const char *cancelText = table->GetString(StringID('BTTN', 'CANC'));

			Dialog *dialog = new Dialog(Vector2D(440.0F, 120.0F), title, okayText, cancelText);
			dialog->SetCompletionProc(&DeleteDialogComplete, this);

			ImageWidget *image = new ImageWidget(Vector2D(64.0F, 64.0F), "C4/warning");
			image->SetWidgetPosition(Point3D(12.0F, 12.0F, 0.0F));
			dialog->AppendSubnode(image);

			TextWidget *text = new TextWidget(Vector2D(336.0F, 0.0F), table->GetString(StringID('SCPT', 'DCFM')), "font/Gui");
			text->SetTextFlags(kTextWrapped);
			text->SetWidgetPosition(Point3D(88.0F, 16.0F, 0.0F));
			dialog->AppendSubnode(text);

			GetScriptEditor()->AddSubwindow(dialog);
		}
	}
}

void ScriptGraphsPage::NewGraphComplete(GraphInfoWindow *window, void *cookie)
{
	ScriptGraphsPage *scriptGraphsPage = static_cast<ScriptGraphsPage *>(cookie);

	ListWidget *listWidget = scriptGraphsPage->listWidget;
	listWidget->SelectListItem(listWidget->GetListItemCount() - 1, true);
}

void ScriptGraphsPage::DeleteDialogComplete(Dialog *dialog, void *cookie)
{
	ScriptGraphsPage *scriptGraphsPage = static_cast<ScriptGraphsPage *>(cookie);

	int32 status = dialog->GetDialogStatus();
	if (status == kDialogOkay)
	{
		const GraphWidget *graphWidget = static_cast<GraphWidget *>(scriptGraphsPage->listWidget->GetFirstSelectedListItem());
		delete graphWidget->scriptGraph;

		scriptGraphsPage->BuildGraphList();
		scriptGraphsPage->listWidget->SelectListItem(0, true);
	}
}


ScriptMethodsPage::ScriptMethodsPage(ScriptEditor *editor) :
		ScriptPage(editor, "ScriptEditor/Methods"),
		multipaneWidgetObserver(this, &ScriptMethodsPage::HandleMultipaneWidgetEvent),
		methodListWidgetObserver(this, &ScriptMethodsPage::HandleMethodListWidgetEvent),
		eventListWidgetObserver(this, &ScriptMethodsPage::HandleEventListWidgetEvent)
{
}

ScriptMethodsPage::~ScriptMethodsPage()
{
}

ScriptMethodsPage::ToolWidget::ToolWidget(const Vector2D& size, const MethodRegistration *registration) : TextWidget(size, GetMethodName(registration), "font/Normal")
{
	methodRegistration = registration;
}

ScriptMethodsPage::ToolWidget::~ToolWidget()
{
}

String<127> ScriptMethodsPage::ToolWidget::GetMethodName(const MethodRegistration *registration)
{
	String<127> name(registration->GetMethodName());

	for (char *text = name;; text++)
	{
		int32 c = *text;
		if (c == 0)
		{
			break;
		}

		if (c < 32)
		{
			*text = 32;
		}
	}

	return (name);
}

ScriptMethodsPage::EventWidget::EventWidget(const Vector2D& size, EventType eventType) : TextWidget(size, TheWorldEditor->GetStringTable()->GetString(StringID('SCPT', 'EVNT', eventType)), "font/Normal")
{
	scriptEvent = eventType;
}

ScriptMethodsPage::EventWidget::~EventWidget()
{
}

void ScriptMethodsPage::Preprocess(void)
{
	static const char *const listIdentifier[kMethodPaneCount] =
	{
		"Basic", "Standard", "Custom", "Events"
	};

	ScriptPage::Preprocess();

	multipaneWidget = static_cast<MultipaneWidget *>(FindWidget("Pane"));
	multipaneWidget->SetObserver(&multipaneWidgetObserver);

	for (machine a = 0; a < kMethodPaneEvents; a++)
	{
		listWidget[a] = static_cast<ListWidget *>(FindWidget(listIdentifier[a]));
		listWidget[a]->SetObserver(&methodListWidgetObserver);
	}

	Vector2D size = listWidget[0]->GetNaturalListItemSize();

	const MethodRegistration *registration = Method::GetFirstRegistration();
	while (registration)
	{
		if (registration->GetMethodName()[0] != 0)
		{
			ToolWidget *widget = new ToolWidget(size, registration);

			MethodGroup group = registration->GetMethodGroup();
			if (group == 'BASC')
			{
				listWidget[kMethodPaneBasic]->InsertSortedListItem(widget);
			}
			else if (group == 'STND')
			{
				listWidget[kMethodPaneStandard]->InsertSortedListItem(widget);
			}
			else
			{
				listWidget[kMethodPaneCustom]->InsertSortedListItem(widget);
			}
		}

		registration = registration->Next();
	}

	listWidget[kMethodPaneEvents] = static_cast<ListWidget *>(FindWidget(listIdentifier[kMethodPaneEvents]));
	listWidget[kMethodPaneEvents]->SetObserver(&eventListWidgetObserver);

	if (!(GetScriptEditor()->GetEditorState() & kScriptEditorWidget))
	{
		listWidget[kMethodPaneEvents]->AppendListItem(new EventWidget(size, kEventControllerActivate));
		listWidget[kMethodPaneEvents]->AppendListItem(new EventWidget(size, kEventControllerDeactivate));
	}
	else
	{
		listWidget[kMethodPaneEvents]->AppendListItem(new EventWidget(size, kEventWidgetActivate));
		listWidget[kMethodPaneEvents]->AppendListItem(new EventWidget(size, kEventWidgetChange));
		listWidget[kMethodPaneEvents]->AppendListItem(new EventWidget(size, kEventWidgetBeginHover));
		listWidget[kMethodPaneEvents]->AppendListItem(new EventWidget(size, kEventWidgetEndHover));
	}
}

void ScriptMethodsPage::HandleMultipaneWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		int32 selection = static_cast<MultipaneWidget *>(widget)->GetSelection();

		for (machine a = 0; a < kMethodPaneCount; a++)
		{
			if (a == selection)
			{
				listWidget[a]->Show();
			}
			else
			{
				listWidget[a]->Hide();
			}
		}

		GetScriptEditor()->SelectDefaultTool();
	}
}

void ScriptMethodsPage::HandleMethodListWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		ToolWidget *toolWidget = static_cast<ToolWidget *>(static_cast<ListWidget *>(widget)->GetFirstSelectedListItem());
		GetScriptEditor()->SelectMethodTool(toolWidget->methodRegistration);
	}
}

void ScriptMethodsPage::HandleEventListWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		EventWidget *eventWidget = static_cast<EventWidget *>(static_cast<ListWidget *>(widget)->GetFirstSelectedListItem());
		GetScriptEditor()->SelectEventTool(eventWidget->scriptEvent);
	}
}

void ScriptMethodsPage::UnselectTool(void)
{
	for (machine a = 0; a < kMethodPaneCount; a++)
	{
		listWidget[a]->UnselectAllListItems();
	}
}


ScriptVariablesPage::ScriptVariablesPage(ScriptEditor *editor) :
		ScriptPage(editor, "ScriptEditor/Variables"),
		listWidgetObserver(this, &ScriptVariablesPage::HandleListWidgetEvent),
		newButtonObserver(this, &ScriptVariablesPage::HandleNewButtonEvent),
		deleteButtonObserver(this, &ScriptVariablesPage::HandleDeleteButtonEvent)
{
}

ScriptVariablesPage::~ScriptVariablesPage()
{
}

ScriptVariablesPage::VariableWidget::VariableWidget(const Vector2D& size, Value *value) : TextWidget(size, value->GetValueName(), "font/Normal")
{
	variableValue = value;
}

ScriptVariablesPage::VariableWidget::~VariableWidget()
{
}

void ScriptVariablesPage::Preprocess(void)
{
	ScriptPage::Preprocess();

	listWidget = static_cast<ListWidget *>(FindWidget("List"));
	listWidget->SetObserver(&listWidgetObserver);

	newButton = static_cast<IconButtonWidget *>(FindWidget("New"));
	deleteButton = static_cast<IconButtonWidget *>(FindWidget("Delete"));
	newButton->SetObserver(&newButtonObserver);
	deleteButton->SetObserver(&deleteButtonObserver);

	BuildVariableList();
}

void ScriptVariablesPage::BuildVariableList(void)
{
	listWidget->PurgeListItems();
	deleteButton->Disable();

	Vector2D size = listWidget->GetNaturalListItemSize();

	Value *value = GetScriptEditor()->GetValueMap()->First();
	while (value)
	{
		listWidget->AppendListItem(new VariableWidget(size, value));
		value = value->Next();
	}
}

void ScriptVariablesPage::HandleListWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	EventType eventType = eventData->eventType;

	if (eventType == kEventWidgetActivate)
	{
		ScriptEditor *scriptEditor = GetScriptEditor();
		const VariableWidget *variableWidget = static_cast<VariableWidget *>(listWidget->GetFirstSelectedListItem());
		scriptEditor->AddSubwindow(new VariableInfoWindow(scriptEditor, variableWidget->variableValue));
	}
	else if (eventType == kEventWidgetChange)
	{
		deleteButton->Enable();
	}
}

void ScriptVariablesPage::HandleNewButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		ScriptEditor *scriptEditor = GetScriptEditor();
		scriptEditor->AddSubwindow(new VariableInfoWindow(scriptEditor));
	}
}

void ScriptVariablesPage::HandleDeleteButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		const VariableWidget *variableWidget = static_cast<VariableWidget *>(listWidget->GetFirstSelectedListItem());
		if (variableWidget)
		{
			delete variableWidget->variableValue;
			BuildVariableList();
		}
	}
}


MethodInfoWindow::MethodInfoWindow(ScriptEditor *editor) : Window("ScriptEditor/MethodInfo")
{
	scriptEditor = editor;
	methodWidget = editor->GetFirstSelectedMethod();
	controllerTarget = editor->GetTargetNode();

	currentFunction = nullptr;
	functionTable = nullptr;

	currentSettingData = nullptr;
	settingDataTable = nullptr;
}

MethodInfoWindow::~MethodInfoWindow()
{
}

MethodInfoWindow::TargetWidget::TargetWidget(const Vector2D& size, const char *text, const char *font, const char *key) : TextWidget(size, text, font)
{
	connectorKey = key;
}

MethodInfoWindow::TargetWidget::~TargetWidget()
{
}

void MethodInfoWindow::Preprocess(void)
{
	Window::Preprocess();

	okayButton = static_cast<PushButtonWidget *>(FindWidget("OK"));
	cancelButton = static_cast<PushButtonWidget *>(FindWidget("Cancel"));

	configurationWidget = static_cast<ConfigurationWidget *>(FindWidget("Config"));

	targetList = static_cast<ListWidget *>(FindWidget("Target"));
	auxiliaryList = static_cast<ListWidget *>(FindWidget("Aux"));
	clearButton = static_cast<PushButtonWidget *>(FindWidget("Clear"));

	expressionBox = static_cast<EditTextWidget *>(FindWidget("Expr"));

	outputBox = static_cast<EditTextWidget *>(FindWidget("Output"));
	outputText = static_cast<TextWidget *>(FindWidget("OutputText"));

	Method *method = methodWidget->GetScriptMethod();
	outputBox->SetText(method->GetOutputValueName());
	BuildTargetList(method);

	MethodType type = method->GetMethodType();
	if (type == kMethodFunction)
	{
		auxiliaryList->Show();
		FindWidget("Funcset")->Show();
		FindWidget("Functions")->Show();

		UpdateFunctionList(static_cast<FunctionMethod *>(method));
	}
	else if (type == kMethodSetting)
	{
		auxiliaryList->Show();
		clearButton->Show();
		FindWidget("Catset")->Show();
		FindWidget("Categories")->Show();

		UpdateCategoryList(static_cast<SettingMethod *>(method));
	}
	else
	{
		FindWidget("Methset")->Show();

		if (type == kMethodExpression)
		{
			expressionBox->Show();
			FindWidget("ExprText")->Show();

			expressionBox->SetText(static_cast<ExpressionMethod *>(method)->GetExpressionText());
			expressionBox->SetRenderLineCount(4);
		}
		else
		{
			configurationWidget->BuildConfiguration(method);
		}
	}

	if (Method::FindRegistration(type)->GetMethodFlags() & kMethodOutputValue)
	{
		outputBox->Show();
		outputText->Show();
	}

	const Setting *input = method->GetFirstInputValue();
	while (input)
	{
		const Setting *setting = configurationWidget->FindSetting(input->GetSettingIdentifier());
		if (setting)
		{
			setting->GetSettingInterface()->SetValueName(input->GetSettingValueName());
		}

		input = input->Next();
	}

	SetNextFocusWidget();
}

const Node *MethodInfoWindow::GetTargetNode(void) const
{
	const TargetWidget *targetWidget = static_cast<TargetWidget *>(targetList->GetFirstSelectedListItem());
	if (targetWidget)
	{
		const ConnectorKey& key = targetWidget->connectorKey;

		if (key[0] != '$')
		{
			return (controllerTarget->GetConnectedNode(key));
		}

		if (key == kConnectorKeyController)
		{
			return (controllerTarget);
		}
	}

	return (nullptr);
}

void MethodInfoWindow::BuildTargetList(const Method *method)
{
	MethodType type = method->GetMethodType();
	unsigned_int32 flags = Method::FindRegistration(type)->GetMethodFlags();
	if (!(flags & kMethodNoTarget))
	{
		bool enableControllerTarget = !(flags & kMethodNoSelfTarget);
		if (!enableControllerTarget)
		{
			Controller *controller = controllerTarget->GetController();
			enableControllerTarget = ((controller) && (controller->GetControllerType() != kControllerScript));
		}

		const StringTable *table = TheWorldEditor->GetStringTable();
		Vector2D size = targetList->GetNaturalListItemSize();
		const char *font = targetList->GetFontName();

		ConnectorKey targetKey = method->GetTargetKey();
		int32 index = 0;

		if (enableControllerTarget)
		{
			targetList->AppendListItem(new TargetWidget(size, table->GetString(StringID('SCPT', 'TARG', 'CTRL')), font, kConnectorKeyController));
			if (targetKey == kConnectorKeyController)
			{
				targetList->SelectListItem(index);
			}

			index++;
		}

		if (type != kMethodFunction)
		{
			targetList->AppendListItem(new TargetWidget(size, table->GetString(StringID('SCPT', 'TARG', 'INTR')), font, kConnectorKeyInitiator));
			if (targetKey == kConnectorKeyInitiator)
			{
				targetList->SelectListItem(index);
			}

			index++;

			targetList->AppendListItem(new TargetWidget(size, table->GetString(StringID('SCPT', 'TARG', 'TRIG')), font, kConnectorKeyTrigger));
			if (targetKey == kConnectorKeyTrigger)
			{
				targetList->SelectListItem(index);
			}

			index++;
		}

		const Hub *hub = controllerTarget->GetHub();
		if (hub)
		{
			const char *string = table->GetString(StringID('SCPT', 'TARG', 'CONN'));

			const Connector *connector = hub->GetFirstOutgoingEdge();
			while (connector)
			{
				const ConnectorKey& key = connector->GetConnectorKey();
				if (key[0] != 0)
				{
					targetList->AppendListItem(new TargetWidget(size, String<16 + kMaxConnectorKeyLength>(string) += key, font, key));
					if (targetKey == key)
					{
						targetList->SelectListItem(index);
					}

					index++;
				}

				connector = connector->GetNextOutgoingEdge();
			}
		}
	}
}

void MethodInfoWindow::UpdateFunctionList(FunctionMethod *method)
{
	auxiliaryList->PurgeListItems();
	controllerRegistration = nullptr;

	const Node *targetNode = GetTargetNode();
	if (targetNode)
	{
		if (targetNode->GetNodeType() == kNodeInstance)
		{
			Node *node = static_cast<const Instance *>(targetNode)->FindExtractableNode();
			if (node)
			{
				targetNode = node;
			}
		}

		const Controller *controller = targetNode->GetController();
		if (controller)
		{
			ControllerType controllerType = controller->GetControllerType();

			controllerRegistration = Controller::FindRegistration(controller->GetControllerType());
			if (controllerRegistration)
			{
				int32 count = 0;
				int32 selection = -1;

				const FunctionRegistration *functionRegistration = controllerRegistration->GetFirstFunctionRegistration();
				while (functionRegistration)
				{
					auxiliaryList->AppendListItem(functionRegistration->GetFunctionName());

					if (controllerRegistration->GetControllerType() == controllerType)
					{
						Function *function = method->GetFunction();
						if ((function) && (function->GetFunctionType() == functionRegistration->GetFunctionType()))
						{
							selection = count;
						}
					}

					count++;
					functionRegistration = functionRegistration->Next();
				}

				functionCount = count;
				if (count != 0)
				{
					functionTable = new Function *[count];
					for (machine a = 0; a < count; a++)
					{
						functionTable[a] = nullptr;
					}

					if (selection != -1)
					{
						auxiliaryList->SelectListItem(selection);
						SelectFunction(method, selection);
					}
				}
			}
		}
	}
}

void MethodInfoWindow::SelectFunction(FunctionMethod *method, int32 index, bool commit, bool save)
{
	if (currentFunction)
	{
		if (commit)
		{
			configurationWidget->CommitConfiguration(currentFunction);

			if (save)
			{
				const Setting *setting = configurationWidget->GetFirstSetting();
				while (setting)
				{
					if (setting->GetSettingValueName()[0] != 0)
					{
						Setting *clone = setting->Clone();
						if (clone)
						{
							method->AddInputValue(clone);
						}
					}

					setting = setting->Next();
				}
			}
		}

		configurationWidget->ReleaseConfiguration();
		currentFunction = nullptr;
	}

	if (index >= 0)
	{
		const FunctionRegistration *functionRegistration = controllerRegistration->GetFunctionRegistration(index);

		Function *function = functionTable[index];
		if (!function)
		{
			if (functionRegistration)
			{
				FunctionType functionType = functionRegistration->GetFunctionType();
				function = controllerRegistration->CreateFunction(functionType);

				currentFunction = function;
				functionTable[index] = function;

				Function *methodFunction = method->GetFunction();
				if ((methodFunction) && (methodFunction->GetFunctionType() == functionType))
				{
					function = methodFunction;
				}

				configurationWidget->BuildConfiguration(function);
			}
		}
		else
		{
			currentFunction = function;
			configurationWidget->BuildConfiguration(function);
		}

		if (functionRegistration->GetFunctionFlags() & kFunctionOutputValue)
		{
			outputBox->Show();
			outputText->Show();
		}
		else
		{
			outputBox->Hide();
			outputText->Hide();
		}
	}
	else
	{
		if (functionTable)
		{
			const Function *function = method->GetFunction();
			for (machine a = 0; a < functionCount; a++)
			{
				if (functionTable[a] != function)
				{
					delete functionTable[a];
				}
			}

			delete[] functionTable;
			functionTable = nullptr;
		}

		outputBox->Hide();
		outputText->Hide();
	}
}

void MethodInfoWindow::UpdateCategoryList(SettingMethod *method)
{
	auxiliaryList->PurgeListItems();
	controllerRegistration = nullptr;

	const Node *targetNode = GetTargetNode();
	if (targetNode)
	{
		const Object *object = targetNode->GetObject();
		if (object)
		{
			settingObject = object;

			int32 count = object->GetCategoryCount();
			categoryCount = count;

			if (count != 0)
			{
				settingDataTable = new SettingData *[count];

				int32 selection = -1;
				Type categoryType = method->GetSettingCategoryType();

				for (machine a = 0; a < count; a++)
				{
					const char	*title;

					settingDataTable[a] = nullptr;

					if (object->GetCategoryType(a, &title) == categoryType)
					{
						selection = a;
					}

					auxiliaryList->AppendListItem(title);
				}

				if (selection != -1)
				{
					auxiliaryList->SelectListItem(selection);
					SelectCategory(method, selection);
				}
			}
		}
	}
}

void MethodInfoWindow::SelectCategory(SettingMethod *method, int32 index, bool commit, bool save)
{
	if (currentSettingData)
	{
		if (commit)
		{
			currentSettingData->settingList.Purge();

			const Setting *setting = configurationWidget->GetFirstSetting();
			while (setting)
			{
				bool active = setting->GetSettingInterface()->ExtractCurrentSetting();
				const char *valueName = setting->GetSettingValueName();
				if (valueName[0] == 0)
				{
					if (active)
					{
						Setting *clone = setting->Clone();
						if (clone)
						{
							currentSettingData->settingList.Append(clone);
						}
					}
				}
				else
				{
					if (save)
					{
						Setting *clone = setting->Clone();
						if (clone)
						{
							method->AddInputValue(clone);
						}
					}
				}

				setting = setting->Next();
			}
		}

		configurationWidget->ReleaseConfiguration();
		currentSettingData = nullptr;
	}

	if (index >= 0)
	{
		SettingData *data = settingDataTable[index];
		if (!data)
		{
			const char	*title;

			data = new SettingData;
			settingDataTable[index] = data;

			Type categoryType = settingObject->GetCategoryType(index, &title);
			data->categoryType = categoryType;

			if (method->GetSettingCategoryType() == categoryType)
			{
				const Setting *setting = method->GetFirstSetting();
				while (setting)
				{
					Setting *clone = setting->Clone();
					if (clone)
					{
						data->settingList.Append(clone);
					}

					setting = setting->Next();
				}

				setting = method->GetFirstInputValue();
				while (setting)
				{
					Setting *clone = setting->Clone();
					if (clone)
					{
						data->settingList.Append(clone);
					}

					setting = setting->Next();
				}
			}
		}

		currentSettingData = data;
		configurationWidget->BuildCategoryConfiguration(settingObject, data->categoryType);

		Setting *configSetting = configurationWidget->GetFirstSetting();
		while (configSetting)
		{
			Type identifier = configSetting->GetSettingIdentifier();

			const Setting *setting = data->settingList.First();
			while (setting)
			{
				if (setting->GetSettingIdentifier() == identifier)
				{
					break;
				}

				setting = setting->Next();
			}

			if (setting)
			{
				configSetting->Copy(setting);
				configSetting->GetSettingInterface()->UpdateCurrentSetting();
			}
			else
			{
				configSetting->GetSettingInterface()->SetIndeterminantValue();
			}

			configSetting = configSetting->Next();
		}
	}
	else
	{
		if (settingDataTable)
		{
			for (machine a = 0; a < categoryCount; a++)
			{
				delete settingDataTable[a];
			}

			delete[] settingDataTable;
			settingDataTable = nullptr;
		}
	}
}

void MethodInfoWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	Method *method = methodWidget->GetScriptMethod();
	EventType eventType = eventData->eventType;

	if (eventType == kEventWidgetActivate)
	{
		if (widget == okayButton)
		{
			method->PurgeInputValues();

			const TargetWidget *targetWidget = static_cast<TargetWidget *>(targetList->GetFirstSelectedListItem());
			method->SetTargetKey((targetWidget) ? targetWidget->connectorKey : "");

			if (outputBox->Visible())
			{
				method->SetOutputValueName(outputBox->GetText());
			}
			else
			{
				method->SetOutputValueName("");
			}

			MethodType type = method->GetMethodType();
			if (type == kMethodFunction)
			{
				FunctionMethod *functionMethod = static_cast<FunctionMethod *>(method);
				functionMethod->SetFunction(currentFunction);
				SelectFunction(functionMethod, -1, true, true);
			}
			else if (type == kMethodSetting)
			{
				SettingMethod *settingMethod = static_cast<SettingMethod *>(method);

				SettingData *data = currentSettingData;
				if (data)
				{
					settingDataTable[auxiliaryList->GetFirstSelectedIndex()] = nullptr;
				}

				SelectCategory(settingMethod, -1, true, true);
				settingMethod->PurgeSettings();

				if (data)
				{
					settingMethod->SetSettingCategoryType(data->categoryType);

					for (;;)
					{
						Setting *setting = data->settingList.First();
						if (!setting)
						{
							break;
						}

						settingMethod->AddSetting(setting);
					}
				}
				else
				{
					settingMethod->SetSettingCategoryType(0);
				}

				delete data;
			}
			else if (type == kMethodExpression)
			{
				static_cast<ExpressionMethod *>(method)->SetExpressionText(expressionBox->GetText());
			}
			else
			{
				configurationWidget->CommitConfiguration(method);

				const Setting *setting = configurationWidget->GetFirstSetting();
				while (setting)
				{
					if (setting->GetSettingValueName()[0] != 0)
					{
						Setting *clone = setting->Clone();
						if (clone)
						{
							method->AddInputValue(clone);
						}
					}

					setting = setting->Next();
				}
			}

			methodWidget->UpdateText();
			scriptEditor->AddEditorState(kScriptEditorUnsaved | kScriptEditorUpdateGraph);
			Close();
		}
		else if (widget == cancelButton)
		{
			MethodType type = method->GetMethodType();
			if (type == kMethodFunction)
			{
				SelectFunction(static_cast<FunctionMethod *>(method), -1);
			}
			else if (type == kMethodSetting)
			{
				SelectCategory(static_cast<SettingMethod *>(method), -1);
			}

			Close();
		}
		else if (widget == clearButton)
		{
			if (currentSettingData)
			{
				currentSettingData->settingList.Purge();

				Setting *setting = configurationWidget->GetFirstSetting();
				while (setting)
				{
					SettingInterface *settingInterface = setting->GetSettingInterface();
					settingInterface->SetIndeterminantValue();
					settingInterface->SetValueName(nullptr);
					setting = setting->Next();
				}
			}
		}
		else if (widget == targetList)
		{
			MethodType type = method->GetMethodType();
			if ((type != kMethodFunction) && (type != kMethodSetting))
			{
				okayButton->Activate();
			}
		}
	}
	else if (eventType == kEventWidgetChange)
	{
		if (widget == targetList)
		{
			MethodType type = method->GetMethodType();

			if (type == kMethodFunction)
			{
				FunctionMethod *functionMethod = static_cast<FunctionMethod *>(method);
				SelectFunction(functionMethod, -1);
				UpdateFunctionList(functionMethod);
			}
			else if (type == kMethodSetting)
			{
				SettingMethod *settingMethod = static_cast<SettingMethod *>(method);
				SelectCategory(settingMethod, -1);
				UpdateCategoryList(settingMethod);
			}
		}
		else if (widget == auxiliaryList)
		{
			if (method->GetMethodType() == kMethodFunction)
			{
				SelectFunction(static_cast<FunctionMethod *>(method), auxiliaryList->GetFirstSelectedIndex());
			}
			else
			{
				SelectCategory(static_cast<SettingMethod *>(method), auxiliaryList->GetFirstSelectedIndex());
			}
		}
	}
}


ScriptSectionInfoWindow::ScriptSectionInfoWindow(ScriptEditor *editor) : Window("ScriptEditor/SectionInfo")
{
	scriptEditor = editor;
	sectionWidget = editor->GetFirstSelectedSection();
}

ScriptSectionInfoWindow::~ScriptSectionInfoWindow()
{
}

void ScriptSectionInfoWindow::Preprocess(void)
{
	Window::Preprocess();

	okayButton = static_cast<PushButtonWidget *>(FindWidget("OK"));
	cancelButton = static_cast<PushButtonWidget *>(FindWidget("Cancel"));

	configurationWidget = static_cast<ConfigurationWidget *>(FindWidget("Config"));
	configurationWidget->BuildConfiguration(sectionWidget->GetSectionMethod());

	SetNextFocusWidget();
}

void ScriptSectionInfoWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		if (widget == okayButton)
		{
			configurationWidget->CommitConfiguration(sectionWidget->GetSectionMethod());
			sectionWidget->UpdateContent();

			scriptEditor->AddEditorState(kScriptEditorUnsaved);
			Close();
		}
		else if (widget == cancelButton)
		{
			Close();
		}
	}
}


GraphInfoWindow::GraphInfoWindow(ScriptEditor *editor, EditorScriptGraph *graph) : Window("ScriptEditor/GraphInfo")
{
	scriptEditor = editor;

	if (graph)
	{
		newGraphFlag = false;
	}
	else
	{
		newGraphFlag = true;
		graph = new EditorScriptGraph(editor);

		if (!(editor->GetEditorState() & kScriptEditorWidget))
		{
			graph->GetScriptEventArray()->AddElement(kEventControllerActivate);
		}
		else
		{
			graph->GetScriptEventArray()->AddElement(kEventWidgetActivate);
		}
	}

	scriptGraph = graph;
}

GraphInfoWindow::~GraphInfoWindow()
{
	if (newGraphFlag)
	{
		delete scriptGraph;
	}
}

void GraphInfoWindow::Preprocess(void)
{
	Window::Preprocess();

	okayButton = static_cast<PushButtonWidget *>(FindWidget("OK"));
	cancelButton = static_cast<PushButtonWidget *>(FindWidget("Cancel"));

	configurationWidget = static_cast<ConfigurationWidget *>(FindWidget("Config"));
	configurationWidget->BuildConfiguration(scriptGraph);

	SetNextFocusWidget();
}

void GraphInfoWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		if (widget == okayButton)
		{
			configurationWidget->CommitConfiguration(scriptGraph);

			if (newGraphFlag)
			{
				newGraphFlag = false;

				scriptEditor->GetScriptGraphList()->Append(scriptGraph);
				scriptEditor->GetGraphsPage()->BuildGraphList();
			}
			else
			{
				scriptEditor->GetGraphsPage()->BuildGraphList();
			}

			scriptEditor->AddEditorState(kScriptEditorUnsaved);

			CallCompletionProc();
			Close();
		}
		else if (widget == cancelButton)
		{
			Close();
		}
	}
}


VariableInfoWindow::VariableInfoWindow(ScriptEditor *editor, Value *value) :
		Window("ScriptEditor/VariableInfo"),
		configurationObserver(this, &VariableInfoWindow::HandleConfigurationEvent)
{
	scriptEditor = editor;
	originalValue = value;
}

VariableInfoWindow::~VariableInfoWindow()
{
}

void VariableInfoWindow::Preprocess(void)
{
	Window::Preprocess();

	okayButton = static_cast<PushButtonWidget *>(FindWidget("OK"));
	cancelButton = static_cast<PushButtonWidget *>(FindWidget("Cancel"));

	if (originalValue)
	{
		currentValue = originalValue->Clone();
	}
	else
	{
		currentValue = new BooleanValue;
		okayButton->Disable();
	}

	configurationWidget = static_cast<ConfigurationWidget *>(FindWidget("Config"));
	configurationWidget->SetObserver(&configurationObserver);
	configurationWidget->BuildConfiguration(currentValue);

	SetNextFocusWidget();
}

void VariableInfoWindow::HandleConfigurationEvent(SettingInterface *settingInterface)
{
	Value *oldValue = currentValue;
	ValueType oldType = oldValue->GetValueType();
	configurationWidget->CommitConfiguration(oldValue);

	ValueType newType = oldValue->GetValueType();
	if (newType != oldType)
	{
		Value *newValue = Value::New(newType);
		newValue->SetValueName(oldValue->GetValueName());
		newValue->SetValueScope(oldValue->GetValueScope());

		delete oldValue;
		currentValue = newValue;

		configurationWidget->ReleaseConfiguration();
		configurationWidget->BuildConfiguration(newValue);
	}

	const char *name = currentValue->GetValueName();
	unsigned_int32 c = name[0];

	if ((c - 65 < 26U) || (c - 97 < 26U))
	{
		if ((!Text::CompareText(name, "true")) && (!Text::CompareText(name, "false")))
		{
			const Value *value = scriptEditor->FindValue(name);
			if ((!value) || (value == originalValue))
			{
				okayButton->Enable();
				return;
			}
		}
	}

	okayButton->Disable();
}

void VariableInfoWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		if (widget == okayButton)
		{
			delete originalValue;

			scriptEditor->GetValueMap()->Insert(currentValue);
			scriptEditor->GetVariablesPage()->BuildVariableList();
			scriptEditor->AddEditorState(kScriptEditorUnsaved);

			Close();
		}
		else if (widget == cancelButton)
		{
			delete currentValue;
			Close();
		}
	}
}


ScriptEditor::ScriptEditor(const Node *target, ScriptController *controller, ScriptObject *object, bool widgetFlag) :
		Window("ScriptEditor/Window"),
		toolButtonObserver(this, &ScriptEditor::HandleToolButtonEvent),
		dragRect(ColorRGBA(0.5F, 0.5F, 0.5F, 1.0F)),
		fiberVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		fiberColorAttribute(ColorRGBA(0.0F, 0.0F, 0.0F, 1.0F)),
		fiberTextureAttribute(&fiberTextureHeader, fiberTextureImage),
		fiberRenderable(kRenderQuads)
{
	targetNode = target;
	scriptController = controller;
	scriptObject = object;
	object->Retain();

	editorState = (widgetFlag) ? kScriptEditorWidget : 0;
	currentScriptGraph = nullptr;

	SetMinWindowSize(Vector2D(640.0F, 512.0F));
	SetStripIcon("ScriptEditor/window");

	const ScriptGraph *scriptGraph = object->GetFirstScriptGraph();
	while (scriptGraph)
	{
		scriptGraphList.Append(new EditorScriptGraph(this, *scriptGraph));
		scriptGraph = scriptGraph->Next();
	}

	const Value *value = object->GetFirstValue();
	while (value)
	{
		scriptValueMap.Insert(value->Clone());
		value = value->Next();
	}

	value = controller->GetFirstValue();
	while (value)
	{
		scriptValueMap.Insert(value->Clone());
		value = value->Next();
	}
}

ScriptEditor::~ScriptEditor()
{
	TheInterfaceMgr->SetCursor(nullptr);

	scriptObject->Release();
}

void ScriptEditor::SetWidgetSize(const Vector2D& size)
{
	Window::SetWidgetSize(size);
	PositionWidgets();

	editorState |= kScriptEditorUpdateGrid;
}

void ScriptEditor::Preprocess(void)
{
	static const char *const toolIdentifier[kScriptToolCount] =
	{
		"Move", "Scroll", "Zoom"
	};

	Window::Preprocess();

	scriptViewport = static_cast<OrthoViewportWidget *>(FindWidget("Viewport"));
	viewportBorder = static_cast<BorderWidget *>(FindWidget("Border"));

	scriptViewport->SetMouseEventProc(&ViewportHandleMouseEvent, this);
	scriptViewport->SetTrackTaskProc(&ViewportTrackTask, this);
	scriptViewport->SetRenderProc(&ViewportRender, this);

	OrthoCamera *camera = scriptViewport->GetViewportCamera();
	CameraObject *cameraObject = camera->GetObject();
	cameraObject->SetClearFlags(kClearColorBuffer);
	cameraObject->SetClearColor(K::white);
	cameraObject->SetNearDepth(-1.0F);
	cameraObject->SetFarDepth(1.0F);

	for (machine a = 0; a < kScriptToolCount; a++)
	{
		IconButtonWidget *widget = static_cast<IconButtonWidget *>(FindWidget(toolIdentifier[a]));
		widget->SetObserver(&toolButtonObserver);
		toolButton[a] = widget;
	}

	sectionButton = static_cast<IconButtonWidget *>(FindWidget("Section"));

	currentMode = kScriptEditorModeTool;
	currentTool = kScriptToolMethodMove;
	currentMethodReg = nullptr;
	toolTracking = false;
	boxSelectFlag = false;

	menuBar = static_cast<MenuBarWidget *>(FindWidget("Menu"));

	graphsPage = new ScriptGraphsPage(this);
	methodsPage = new ScriptMethodsPage(this);
	variablesPage = new ScriptVariablesPage(this);

	bookWidget = new BookWidget(Vector2D(192.0F, 0.0F));
	bookWidget->AppendPage(graphsPage);
	bookWidget->AppendPage(methodsPage);
	bookWidget->AppendPage(variablesPage);
	bookWidget->OrganizePages();
	AppendSubnode(bookWidget);

	PositionWidgets();
	BuildMenus();

	viewportGrid.SetGridLineSpacing(16.0F);
	viewportGrid.SetMajorLineInterval(2);
	viewportGrid.SetMinorLineColor(ColorRGB(0.9375F, 0.9375F, 0.9375F));
	viewportGrid.SetMajorLineColor(ColorRGB(0.90625F, 0.90625F, 0.90625F));
	viewportGrid.SetAxisLineColor(ColorRGB(0.90625F, 0.90625F, 0.90625F));

	fiberRenderable.SetShaderFlags(kShaderAmbientEffect | kShaderVertexPolyboard | kShaderLinearPolyboard | kShaderOrthoPolyboard);
	fiberRenderable.SetAmbientBlendState(kBlendPremultInterp);

	fiberRenderable.SetVertexCount(4);
	fiberRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &fiberVertexBuffer, sizeof(FiberVertex));
	fiberRenderable.SetVertexAttributeArray(kArrayPosition, 0, 2);
	fiberRenderable.SetVertexAttributeArray(kArrayTangent, sizeof(Point2D), 4);
	fiberRenderable.SetVertexAttributeArray(kArrayTexcoord, sizeof(Point2D) + sizeof(Vector4D), 2);
	fiberVertexBuffer.Establish(sizeof(FiberVertex) * 4);

	fiberAttributeList.Append(&fiberColorAttribute);
	fiberAttributeList.Append(&fiberTextureAttribute);
	fiberRenderable.SetMaterialAttributeList(&fiberAttributeList);

	fiberVertex[0].texcoord.Set(0.0F, 1.0F);
	fiberVertex[1].texcoord.Set(0.4375F, 1.0F);
	fiberVertex[2].texcoord.Set(0.4375F, 1.0F);
	fiberVertex[3].texcoord.Set(0.0F, 1.0F);

	editorState |= kScriptEditorUpdateMenus | kScriptEditorUpdateGrid;
	SetCurrentScriptGraph(static_cast<EditorScriptGraph *>(scriptGraphList.First()));

	bookWidget->Preprocess();
}

void ScriptEditor::PositionWidgets(void)
{
	float width = GetWidgetSize().x;
	float height = GetWidgetSize().y;

	const Point3D& position = scriptViewport->GetWidgetPosition();
	Vector2D viewportSize(width - position.x - 4.0F, height - position.y - 4.0F);

	scriptViewport->SetWidgetSize(viewportSize);
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
}

void ScriptEditor::BuildMenus(void)
{
	const StringTable *table = TheWorldEditor->GetStringTable();

	// Script Menu

	scriptMenu = new PulldownMenuWidget(table->GetString(StringID('SCPT', 'MENU', 'SCPT')));
	menuBar->AppendMenu(scriptMenu);

	scriptMenu->AppendMenuItem(new MenuItemWidget(table->GetString(StringID('SCPT', 'MENU', 'SCPT', 'CLOS')), WidgetObserver<ScriptEditor>(this, &ScriptEditor::HandleCloseMenuItem), Shortcut('W')));
	scriptMenu->AppendMenuItem(new MenuItemWidget(table->GetString(StringID('SCPT', 'MENU', 'SCPT', 'SAVE')), WidgetObserver<ScriptEditor>(this, &ScriptEditor::HandleSaveScriptMenuItem), Shortcut('S')));

	// Edit Menu

	editMenu = new PulldownMenuWidget(table->GetString(StringID('SCPT', 'MENU', 'EDIT')));
	menuBar->AppendMenu(editMenu);

	MenuItemWidget *widget = new MenuItemWidget(table->GetString(StringID('SCPT', 'MENU', 'EDIT', 'UNDO')), WidgetObserver<ScriptEditor>(this, &ScriptEditor::HandleUndoMenuItem), Shortcut('Z'));
	scriptMenuItem[kScriptMenuUndo] = widget;
	widget->Disable();
	editMenu->AppendMenuItem(widget);

	editMenu->AppendMenuItem(new MenuItemWidget(kLineSolid));

	widget = new MenuItemWidget(table->GetString(StringID('SCPT', 'MENU', 'EDIT', 'CUT ')), WidgetObserver<ScriptEditor>(this, &ScriptEditor::HandleCutMenuItem), Shortcut('X'));
	scriptMenuItem[kScriptMenuCut] = widget;
	editMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('SCPT', 'MENU', 'EDIT', 'COPY')), WidgetObserver<ScriptEditor>(this, &ScriptEditor::HandleCopyMenuItem), Shortcut('C'));
	scriptMenuItem[kScriptMenuCopy] = widget;
	editMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('SCPT', 'MENU', 'EDIT', 'PAST')), WidgetObserver<ScriptEditor>(this, &ScriptEditor::HandlePasteMenuItem), Shortcut('V'));
	scriptMenuItem[kScriptMenuPaste] = widget;
	if (editorClipboard.Empty())
	{
		widget->Disable();
	}

	editMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('SCPT', 'MENU', 'EDIT', 'CLER')), WidgetObserver<ScriptEditor>(this, &ScriptEditor::HandleClearMenuItem), Shortcut(kKeyCodeDelete, kShortcutUnmodified));
	scriptMenuItem[kScriptMenuClear] = widget;
	editMenu->AppendMenuItem(widget);

	editMenu->AppendMenuItem(new MenuItemWidget(kLineSolid));
	editMenu->AppendMenuItem(new MenuItemWidget(table->GetString(StringID('SCPT', 'MENU', 'EDIT', 'SALL')), WidgetObserver<ScriptEditor>(this, &ScriptEditor::HandleSelectAllMenuItem), Shortcut('A')));

	widget = new MenuItemWidget(table->GetString(StringID('SCPT', 'MENU', 'EDIT', 'DUPL')), WidgetObserver<ScriptEditor>(this, &ScriptEditor::HandleDuplicateMenuItem), Shortcut('D'));
	scriptMenuItem[kScriptMenuDuplicate] = widget;
	editMenu->AppendMenuItem(widget);

	// Method Menu

	methodMenu = new PulldownMenuWidget(table->GetString(StringID('SCPT', 'MENU', 'MTHD')));
	menuBar->AppendMenu(methodMenu);

	widget = new MenuItemWidget(table->GetString(StringID('SCPT', 'MENU', 'MTHD', 'INFO')), WidgetObserver<ScriptEditor>(this, &ScriptEditor::HandleGetInfoMenuItem), Shortcut('I'));
	scriptMenuItem[kScriptMenuGetInfo] = widget;
	widget->Disable();
	methodMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('SCPT', 'MENU', 'MTHD', 'FCON')), WidgetObserver<ScriptEditor>(this, &ScriptEditor::HandleCycleFiberConditionMenuItem), Shortcut('F'));
	scriptMenuItem[kScriptMenuCycleFiberCondition] = widget;
	widget->Disable();
	methodMenu->AppendMenuItem(widget);
}

void ScriptEditor::SetCurrentScriptGraph(EditorScriptGraph *scriptGraph)
{
	if (currentScriptGraph)
	{
		currentScriptGraph->SaveViewportCameraPosition(scriptViewport);
	}

	editorState |= kScriptEditorUpdateMenus | kScriptEditorUpdateGrid;
	currentScriptGraph = scriptGraph;

	scriptGraph->RestoreViewportCameraPosition(scriptViewport);
}

void ScriptEditor::UpdateScriptGraph(void)
{
	List<Reference<Method> >	initialList;

	MethodWidget *methodWidget = currentScriptGraph->methodWidgetList.First();
	for (machine a = 0; a < 2; a++)
	{
		while (methodWidget)
		{
			Method *method = methodWidget->GetScriptMethod();
			method->SetMethodIndex(-1);

			if (!method->GetFirstIncomingEdge())
			{
				methodWidget->UpdateColor((method->GetMethodType() != kMethodEvent) ? kMethodColorInitial : kMethodColorEvent);
				initialList.Append(new Reference<Method>(method));
			}
			else
			{
				methodWidget->UpdateColor((method->GetMethodType() != kMethodEvent) ? kMethodColorNormal : kMethodColorEvent);
			}

			methodWidget = methodWidget->Next();
		}

		methodWidget = currentScriptGraph->selectedMethodList.First();
	}

	Reference<Method> *reference = initialList.First();
	while (reference)
	{
		TraverseScriptGraph(reference->GetTarget(), 0);
		reference = reference->Next();
	}

	methodWidget = currentScriptGraph->methodWidgetList.First();
	for (machine a = 0; a < 2; a++)
	{
		while (methodWidget)
		{
			const Method *method = methodWidget->GetScriptMethod();
			if (method->GetMethodIndex() < 0)
			{
				methodWidget->UpdateColor(kMethodColorDead);
			}
			else if (DetectMethodError(method))
			{
				methodWidget->UpdateColor(kMethodColorError);
			}

			methodWidget = methodWidget->Next();
		}

		methodWidget = currentScriptGraph->selectedMethodList.First();
	}

	FiberWidget *fiberWidget = currentScriptGraph->fiberWidgetList.First();
	for (machine a = 0; a < 2; a++)
	{
		while (fiberWidget)
		{
			Fiber *fiber = fiberWidget->GetScriptFiber();

			unsigned_int32 oldFlags = fiber->GetFiberFlags();
			unsigned_int32 newFlags = oldFlags;

			const Method *start = fiber->GetStartElement();
			const Method *finish = fiber->GetFinishElement();

			int32 index = finish->GetMethodIndex();
			if ((index >= 0) && (index < start->GetMethodIndex()) && (currentScriptGraph->Predecessor(finish, start)))
			{
				newFlags |= kFiberLooping;
			}
			else
			{
				newFlags &= ~kFiberLooping;
			}

			if (newFlags != oldFlags)
			{
				fiber->SetFiberFlags(newFlags);
				fiberWidget->Rebuild();
			}

			fiberWidget = fiberWidget->Next();
		}

		fiberWidget = currentScriptGraph->selectedFiberList.First();
	}
}

void ScriptEditor::TraverseScriptGraph(Method *method, int32 depth)
{
	method->SetMethodIndex(Max(method->GetMethodIndex(), depth));
	method->SetMethodState(1);

	const Fiber *fiber = method->GetFirstOutgoingEdge();
	while (fiber)
	{
		Method *finish = fiber->GetFinishElement();
		if (finish->GetMethodState() == 0)
		{
			TraverseScriptGraph(finish, depth + 1);
		}

		fiber = fiber->GetNextOutgoingEdge();
	}

	method->SetMethodState(0);
}

bool ScriptEditor::DetectMethodError(const Method *method)
{
	if (method->GetMethodType() == kMethodExpression)
	{
		const ExpressionMethod *expressionMethod = static_cast<const ExpressionMethod *>(method);
		if ((expressionMethod->GetExpressionText()) && (!expressionMethod->GetEvaluatorRoot()))
		{
			return (true);
		}
	}

	return (false);
}

void ScriptEditor::ReattachMethod(MethodWidget *widget)
{
	currentScriptGraph->AddElement(widget->GetScriptMethod());
	currentScriptGraph->graphRoot->AppendSubnode(widget);
	currentScriptGraph->methodWidgetList.Append(widget);

	widget->SetViewportScale(scriptViewport->GetOrthoScale().x);
}

void ScriptEditor::ReattachFiber(FiberWidget *widget)
{
	widget->GetScriptFiber()->Attach();
	currentScriptGraph->graphRoot->PrependSubnode(widget);
	currentScriptGraph->fiberWidgetList.Append(widget);

	widget->SetViewportScale(scriptViewport->GetOrthoScale().x);
}

void ScriptEditor::ReattachSection(ScriptSectionWidget *widget)
{
	currentScriptGraph->AddElement(widget->GetSectionMethod());
	currentScriptGraph->sectionRoot->AppendSubnode(widget);
	currentScriptGraph->sectionWidgetList.Append(widget);

	widget->SetViewportScale(scriptViewport->GetOrthoScale().x);
}

void ScriptEditor::RebuildFiberWidgets(const Method *method)
{
	FiberWidget *fiberWidget = currentScriptGraph->fiberWidgetList.First();
	while (fiberWidget)
	{
		const Fiber *fiber = fiberWidget->GetScriptFiber();
		if ((fiber->GetStartElement() == method) || (fiber->GetFinishElement() == method))
		{
			fiberWidget->Rebuild();
		}

		fiberWidget = fiberWidget->Next();
	}

	fiberWidget = currentScriptGraph->selectedFiberList.First();
	while (fiberWidget)
	{
		const Fiber *fiber = fiberWidget->GetScriptFiber();
		if ((fiber->GetStartElement() == method) || (fiber->GetFinishElement() == method))
		{
			fiberWidget->Rebuild();
		}

		fiberWidget = fiberWidget->Next();
	}
}

void ScriptEditor::SelectDefaultTool(void)
{
	if ((currentMode != kScriptEditorModeTool) || (currentTool != kScriptToolMethodMove))
	{
		UnselectCurrentTool();
		toolButton[kScriptToolMethodMove]->SetValue(1);

		currentMode = kScriptEditorModeTool;
		currentTool = kScriptToolMethodMove;
	}
}

void ScriptEditor::SelectMethodTool(const MethodRegistration *registration)
{
	if ((currentMode != kScriptEditorModeMethod) || (currentMethodReg != registration))
	{
		if ((currentMode != kScriptEditorModeMethod) && (currentMode != kScriptEditorModeEvent))
		{
			UnselectCurrentTool();
		}

		currentMode = kScriptEditorModeMethod;
		currentTool = 0;
		currentMethodReg = registration;
	}
}

void ScriptEditor::SelectEventTool(EventType eventType)
{
	if ((currentMode != kScriptEditorModeEvent) || (currentEventType != eventType))
	{
		if ((currentMode != kScriptEditorModeMethod) && (currentMode != kScriptEditorModeEvent))
		{
			UnselectCurrentTool();
		}

		currentMode = kScriptEditorModeEvent;
		currentTool = 0;
		currentEventType = eventType;
	}
}

void ScriptEditor::UnselectCurrentTool(void)
{
	switch (currentMode)
	{
		case kScriptEditorModeTool:

			toolButton[currentTool]->SetValue(0);
			break;

		case kScriptEditorModeMethod:
		case kScriptEditorModeEvent:

			methodsPage->UnselectTool();
			break;

		case kScriptEditorModeSection:

			sectionButton->SetValue(0);
			break;
	}
}

void ScriptEditor::UpdateViewportScale(float scale, const Point3D *position)
{
	scale = Clamp(scale, 1.0F, 8.0F);

	if (position)
	{
		scriptViewport->SetZoomScale(Vector2D(scale, scale), *position, true);
	}
	else
	{
		scriptViewport->SetOrthoScale(Vector2D(scale, scale));
	}

	editorState |= kScriptEditorUpdateGrid;

	MethodWidget *method = currentScriptGraph->methodWidgetList.First();
	while (method)
	{
		method->SetViewportScale(scale);
		method = method->Next();
	}

	method = currentScriptGraph->selectedMethodList.First();
	while (method)
	{
		method->SetViewportScale(scale);
		method = method->Next();
	}

	FiberWidget *fiber = currentScriptGraph->fiberWidgetList.First();
	while (fiber)
	{
		fiber->SetViewportScale(scale);
		fiber = fiber->Next();
	}

	fiber = currentScriptGraph->selectedFiberList.First();
	while (fiber)
	{
		fiber->SetViewportScale(scale);
		fiber = fiber->Next();
	}

	ScriptSectionWidget *section = currentScriptGraph->sectionWidgetList.First();
	while (section)
	{
		section->SetViewportScale(scale);
		section = section->Next();
	}

	section = currentScriptGraph->selectedSectionList.First();
	while (section)
	{
		section->SetViewportScale(scale);
		section = section->Next();
	}
}

void ScriptEditor::SelectMethod(MethodWidget *methodWidget, unsigned_int32 state)
{
	currentScriptGraph->selectedMethodList.Append(methodWidget);
	methodWidget->Select(state);

	const Method *method = methodWidget->GetScriptMethod();

	FiberWidget *fiberWidget = currentScriptGraph->fiberWidgetList.First();
	while (fiberWidget)
	{
		const Fiber *fiber = fiberWidget->GetScriptFiber();
		if ((fiber->GetStartElement() == method) || (fiber->GetFinishElement() == method))
		{
			fiberWidget->Hilite();
		}

		fiberWidget = fiberWidget->Next();
	}

	editorState |= kScriptEditorUpdateMenus;
}

void ScriptEditor::UnselectMethod(MethodWidget *methodWidget)
{
	currentScriptGraph->methodWidgetList.Append(methodWidget);
	methodWidget->Unselect();

	const Method *method = methodWidget->GetScriptMethod();

	FiberWidget *fiberWidget = currentScriptGraph->fiberWidgetList.First();
	while (fiberWidget)
	{
		const Fiber *fiber = fiberWidget->GetScriptFiber();
		if ((fiber->GetStartElement() == method) || (fiber->GetFinishElement() == method))
		{
			fiberWidget->Unhilite();
		}

		fiberWidget = fiberWidget->Next();
	}

	editorState |= kScriptEditorUpdateMenus;
}

void ScriptEditor::SelectFiber(FiberWidget *fiberWidget)
{
	currentScriptGraph->selectedFiberList.Append(fiberWidget);
	fiberWidget->Select();

	editorState |= kScriptEditorUpdateMenus;
}

void ScriptEditor::UnselectFiber(FiberWidget *fiberWidget)
{
	currentScriptGraph->fiberWidgetList.Append(fiberWidget);
	fiberWidget->Unselect();

	editorState |= kScriptEditorUpdateMenus;
}

void ScriptEditor::SelectSection(ScriptSectionWidget *sectionWidget)
{
	currentScriptGraph->selectedSectionList.Append(sectionWidget);
	sectionWidget->Select();

	editorState |= kScriptEditorUpdateMenus;
}

void ScriptEditor::UnselectSection(ScriptSectionWidget *sectionWidget)
{
	currentScriptGraph->sectionWidgetList.Append(sectionWidget);
	sectionWidget->Unselect();

	editorState |= kScriptEditorUpdateMenus;
}

void ScriptEditor::SelectAll(void)
{
	for (;;)
	{
		FiberWidget *widget = currentScriptGraph->selectedFiberList.First();
		if (!widget)
		{
			break;
		}

		UnselectFiber(widget);
	}

	for (;;)
	{
		MethodWidget *widget = currentScriptGraph->methodWidgetList.First();
		if (!widget)
		{
			break;
		}

		SelectMethod(widget);
	}

	for (;;)
	{
		ScriptSectionWidget *widget = currentScriptGraph->sectionWidgetList.First();
		if (!widget)
		{
			break;
		}

		SelectSection(widget);
	}
}

void ScriptEditor::UnselectAll(void)
{
	for (;;)
	{
		MethodWidget *widget = currentScriptGraph->selectedMethodList.First();
		if (!widget)
		{
			break;
		}

		UnselectMethod(widget);
	}

	for (;;)
	{
		FiberWidget *widget = currentScriptGraph->selectedFiberList.First();
		if (!widget)
		{
			break;
		}

		UnselectFiber(widget);
	}

	for (;;)
	{
		ScriptSectionWidget *widget = currentScriptGraph->selectedSectionList.First();
		if (!widget)
		{
			break;
		}

		UnselectSection(widget);
	}
}

void ScriptEditor::UnselectAllTemp(void)
{
	MethodWidget *widget = currentScriptGraph->selectedMethodList.First();
	while (widget)
	{
		MethodWidget *next = widget->Next();

		if (widget->GetMethodWidgetState() & kMethodWidgetTempSelected)
		{
			UnselectMethod(widget);
		}

		widget = next;
	}
}

void ScriptEditor::DeleteMethod(MethodWidget *methodWidget, List<MethodWidget> *deletedMethodList, List<FiberWidget> *deletedFiberList)
{
	Method *method = methodWidget->GetScriptMethod();

	FiberWidget *fiberWidget = currentScriptGraph->fiberWidgetList.First();
	while (fiberWidget)
	{
		FiberWidget *next = fiberWidget->Next();

		Fiber *fiber = fiberWidget->GetScriptFiber();
		if ((fiber->GetStartElement() == method) || (fiber->GetFinishElement() == method))
		{
			if (deletedFiberList)
			{
				fiber->GraphEdge<Method, Fiber>::Detach();

				fiberWidget->HandleDelete();
				fiberWidget->Widget::Detach();

				deletedFiberList->Append(fiberWidget);
			}
			else
			{
				delete fiber;
				delete fiberWidget;
			}
		}

		fiberWidget = next;
	}

	if (deletedMethodList)
	{
		currentScriptGraph->RemoveElement(method);

		methodWidget->Unselect();
		methodWidget->Widget::Detach();

		deletedMethodList->Append(methodWidget);
	}
	else
	{
		delete method;
		delete methodWidget;
	}

	editorState |= kScriptEditorUpdateMenus | kScriptEditorUpdateGraph;
}

void ScriptEditor::DeleteFiber(FiberWidget *fiberWidget, List<FiberWidget> *deletedFiberList)
{
	Fiber *fiber = fiberWidget->GetScriptFiber();
	if (deletedFiberList)
	{
		fiber->GraphEdge<Method, Fiber>::Detach();

		fiberWidget->HandleDelete();
		fiberWidget->Widget::Detach();

		deletedFiberList->Append(fiberWidget);
	}
	else
	{
		delete fiber;
		delete fiberWidget;
	}

	editorState |= kScriptEditorUpdateMenus | kScriptEditorUpdateGraph;
}

void ScriptEditor::DeleteSection(ScriptSectionWidget *sectionWidget, List<ScriptSectionWidget> *deletedSectionList)
{
	if (deletedSectionList)
	{
		currentScriptGraph->RemoveElement(sectionWidget->GetSectionMethod());

		sectionWidget->Unselect();
		sectionWidget->Widget::Detach();

		deletedSectionList->Append(sectionWidget);
	}
	else
	{
		delete sectionWidget->GetSectionMethod();
		delete sectionWidget;
	}

	editorState |= kScriptEditorUpdateMenus | kScriptEditorUpdateGraph;
}

void ScriptEditor::AddOperation(ScriptOperation *operation)
{
	List<ScriptOperation> *operationList = &currentScriptGraph->operationList;
	if (operationList->GetElementCount() >= kMaxScriptOperationCount)
	{
		delete operationList->First();
	}

	operationList->Append(operation);

	scriptMenuItem[kScriptMenuUndo]->Enable();
	editorState |= kScriptEditorUnsaved;
}

void ScriptEditor::HandleCloseMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	Close();
}

void ScriptEditor::HandleSaveScriptMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	scriptObject->PurgeScriptGraphs();

	const ScriptGraph *scriptGraph = scriptGraphList.First();
	while (scriptGraph)
	{
		ScriptGraph *savedScriptGraph = new ScriptGraph(*scriptGraph);
		scriptObject->AddScriptGraph(savedScriptGraph);

		const Method *method = scriptGraph->GetFirstElement();
		while (method)
		{
			// If an event method appears in the script, make sure the script graph advertises
			// that it can handle the type of event.

			if (method->GetMethodType() == kMethodEvent)
			{
				EventType eventType = static_cast<const EventMethod *>(method)->GetScriptEvent();
				if (!savedScriptGraph->HandlesEvent(eventType))
				{
					savedScriptGraph->GetScriptEventArray()->AddElement(eventType);
				}
			}

			// If the method needs to send messages to its target node's controller, assign a
			// generic controller to the connected node if it doesn't already have a controller.

			const ConnectorKey& key = method->GetTargetKey();
			if (key[0] != '$')
			{
				Node *node = targetNode->GetConnectedNode(key);
				if ((node) && (!node->GetController()))
				{
					const MethodRegistration *registration = Method::FindRegistration(method->GetMethodType());
					if (!(registration->GetMethodFlags() & kMethodNoMessage))
					{
						node->SetController(new Controller);
					}
				}
			}

			method = method->GetNextElement();
		}

		scriptGraph = scriptGraph->Next();
	}

	scriptObject->PurgeValues();
	scriptController->PurgeValues();

	const Value *value = scriptValueMap.First();
	while (value)
	{
		if (value->GetValueScope() != kValueScopeController)
		{
			scriptObject->AddValue(value->Clone());
		}
		else
		{
			scriptController->AddValue(value->Clone());
		}

		value = value->Next();
	}

	editorState &= ~kScriptEditorUnsaved;
}

void ScriptEditor::HandleUndoMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	List<ScriptOperation> *operationList = &currentScriptGraph->operationList;
	ScriptOperation *operation = operationList->Last();
	if (operation)
	{
		operation->Restore(this);
		delete operation;
	}

	editorState |= kScriptEditorUnsaved | kScriptEditorUpdateMenus | kScriptEditorUpdateGraph;
}

void ScriptEditor::HandleCutMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	HandleCopyMenuItem(nullptr, nullptr);
	HandleClearMenuItem(nullptr, nullptr);
}

void ScriptEditor::HandleCopyMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	editorClipboard.Purge();

	const MethodWidget *methodWidget = currentScriptGraph->methodWidgetList.First();
	while (methodWidget)
	{
		methodWidget->GetScriptMethod()->SetCloneMethod(nullptr);
		methodWidget = methodWidget->Next();
	}

	methodWidget = currentScriptGraph->selectedMethodList.First();
	while (methodWidget)
	{
		Method *method = methodWidget->GetScriptMethod();
		Method *clone = method->Clone();
		method->SetCloneMethod(clone);
		editorClipboard.AddElement(clone);

		methodWidget = methodWidget->Next();
	}

	methodWidget = currentScriptGraph->selectedMethodList.First();
	while (methodWidget)
	{
		const Method *method = methodWidget->GetScriptMethod();
		Method *finish = method->GetCloneMethod();

		const Fiber *fiber = method->GetFirstIncomingEdge();
		while (fiber)
		{
			Method *start = fiber->GetStartElement()->GetCloneMethod();
			if (start)
			{
				new Fiber(*fiber, start, finish);
			}

			fiber = fiber->GetNextIncomingEdge();
		}

		methodWidget = methodWidget->Next();
	}

	ScriptSectionWidget *sectionWidget = currentScriptGraph->selectedSectionList.First();
	while (sectionWidget)
	{
		Method *clone = sectionWidget->GetSectionMethod()->Clone();
		editorClipboard.AddElement(clone);

		sectionWidget = sectionWidget->Next();
	}

	MenuItemWidget *widget = scriptMenuItem[kScriptMenuPaste];
	if (editorClipboard.Empty())
	{
		widget->Disable();
	}
	else
	{
		widget->Enable();
	}
}

void ScriptEditor::HandlePasteMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	List<MethodWidget>			methodList;
	List<FiberWidget>			fiberList;
	List<ScriptSectionWidget>	sectionList;

	UnselectAll();

	ScriptGraph graph(editorClipboard);

	float xmin = K::infinity;
	float ymin = K::infinity;
	float xmax = K::minus_infinity;
	float ymax = K::minus_infinity;

	Method *method = graph.GetFirstElement();
	while (method)
	{
		MethodType type = method->GetMethodType();
		if (type != kMethodSection)
		{
			const Point2D& p = method->GetMethodPosition();
			xmin = Fmin(xmin, p.x);
			xmax = Fmax(xmax, p.x + kMethodBoxWidth);
			ymin = Fmin(ymin, p.y);
			ymax = Fmax(ymax, p.y + kMethodBoxHeight);
		}
		else
		{
			const Point2D& p = method->GetMethodPosition();

			const SectionMethod *section = static_cast<SectionMethod *>(method);
			float width = section->GetSectionWidth();
			float height = section->GetSectionHeight();

			xmin = Fmin(xmin, p.x);
			xmax = Fmax(xmax, p.x + width);
			ymin = Fmin(ymin, p.y);
			ymax = Fmax(ymax, p.y + height);
		}

		method = method->GetNextElement();
	}

	const Box2D *box = currentScriptGraph->graphRoot->GetBoundingBox();
	float dx = (box) ? box->max.x + (kMethodBoxPadding + 16.0F) - xmin : 0.0F;
	dx = Floor((dx + 4.0F) * 0.125F) * 8.0F;

	float xcen = xmax + dx + 8.0F;
	float ycen = (ymin + ymax) * 0.5F;
	ShowGraphPosition(xcen, ycen);

	method = graph.GetFirstElement();
	while (method)
	{
		const Point2D& p = method->GetMethodPosition();
		method->SetMethodPosition(Point2D(p.x + dx, p.y));
		method = method->GetNextElement();
	}

	method = graph.GetFirstElement();
	while (method)
	{
		MethodType type = method->GetMethodType();
		Point3D position = method->GetMethodPosition();

		if (type != kMethodSection)
		{
			MethodWidget *widget = new MethodWidget(this, method, Method::FindRegistration(type));
			widget->SetViewportScale(scriptViewport->GetOrthoScale().x);
			currentScriptGraph->graphRoot->AppendNewSubnode(widget);
			widget->SetWidgetPosition(position);
			methodList.Append(widget);
		}
		else
		{
			ScriptSectionWidget *widget = new ScriptSectionWidget(this, static_cast<SectionMethod *>(method));
			widget->SetViewportScale(scriptViewport->GetOrthoScale().x);
			currentScriptGraph->sectionRoot->AppendNewSubnode(widget);
			widget->SetWidgetPosition(position);
			sectionList.Append(widget);
		}

		method = method->GetNextElement();
	}

	method = graph.GetFirstElement();
	while (method)
	{
		Fiber *fiber = method->GetFirstIncomingEdge();
		while (fiber)
		{
			FiberWidget *widget = new FiberWidget(this, fiber);
			widget->SetViewportScale(scriptViewport->GetOrthoScale().x);
			currentScriptGraph->graphRoot->PrependSubnode(widget);
			fiberList.Append(widget);
			widget->Preprocess();

			fiber = fiber->GetNextIncomingEdge();
		}

		method = method->GetNextElement();
	}

	for (;;)
	{
		method = graph.GetFirstElement();
		if (!method)
		{
			break;
		}

		currentScriptGraph->AddElement(method);
	}

	AddOperation(new CreateScriptOperation(&methodList, &fiberList, &sectionList));

	for (;;)
	{
		FiberWidget *widget = fiberList.First();
		if (!widget)
		{
			break;
		}

		currentScriptGraph->fiberWidgetList.Append(widget);
	}

	for (;;)
	{
		MethodWidget *widget = methodList.First();
		if (!widget)
		{
			break;
		}

		SelectMethod(widget);
	}

	for (;;)
	{
		ScriptSectionWidget *widget = sectionList.First();
		if (!widget)
		{
			break;
		}

		SelectSection(widget);
	}

	currentScriptGraph->graphRoot->Update();
	currentScriptGraph->sectionRoot->Update();
	editorState |= kScriptEditorUpdateGraph;
}

void ScriptEditor::HandleClearMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	List<MethodWidget>			deletedMethodList;
	List<FiberWidget>			deletedFiberList;
	List<ScriptSectionWidget>	deletedSectionList;

	FiberWidget *fiberWidget = currentScriptGraph->selectedFiberList.First();
	while (fiberWidget)
	{
		FiberWidget *next = fiberWidget->Next();
		DeleteFiber(fiberWidget, &deletedFiberList);
		fiberWidget = next;
	}

	MethodWidget *methodWidget = currentScriptGraph->selectedMethodList.First();
	while (methodWidget)
	{
		MethodWidget *next = methodWidget->Next();
		DeleteMethod(methodWidget, &deletedMethodList, &deletedFiberList);
		methodWidget = next;
	}

	ScriptSectionWidget *sectionWidget = currentScriptGraph->selectedSectionList.First();
	while (sectionWidget)
	{
		ScriptSectionWidget *next = sectionWidget->Next();
		DeleteSection(sectionWidget, &deletedSectionList);
		sectionWidget = next;
	}

	if ((!deletedMethodList.Empty()) || (!deletedFiberList.Empty()) || (!deletedSectionList.Empty()))
	{
		AddOperation(new DeleteScriptOperation(&deletedMethodList, &deletedFiberList, &deletedSectionList));
	}
}

void ScriptEditor::HandleSelectAllMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	SelectAll();
}

void ScriptEditor::HandleDuplicateMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	HandleCopyMenuItem(nullptr, nullptr);
	HandlePasteMenuItem(nullptr, nullptr);
}

void ScriptEditor::HandleGetInfoMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	const MethodWidget *methodWidget = currentScriptGraph->selectedMethodList.First();
	if (methodWidget)
	{
		if ((methodWidget->GetScriptMethod()->GetMethodType() != kMethodEvent) && (currentScriptGraph->selectedMethodList.Last() == methodWidget))
		{
			AddSubwindow(new MethodInfoWindow(this));
		}

		return;
	}

	const ScriptSectionWidget *sectionWidget = currentScriptGraph->selectedSectionList.First();
	if (sectionWidget)
	{
		if (currentScriptGraph->selectedSectionList.Last() == sectionWidget)
		{
			AddSubwindow(new ScriptSectionInfoWindow(this));
		}

		return;
	}
}

void ScriptEditor::HandleCycleFiberConditionMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	const List<FiberWidget> *fiberList = &currentScriptGraph->selectedFiberList;
	AddOperation(new FiberScriptOperation(fiberList));

	FiberWidget *widget = fiberList->First();
	while (widget)
	{
		Fiber *fiber = widget->GetScriptFiber();

		unsigned_int32 flags = fiber->GetFiberFlags();
		if (flags & kFiberConditionTrue)
		{
			flags = (flags & ~kFiberConditionTrue) | kFiberConditionFalse;
		}
		else if (flags & kFiberConditionFalse)
		{
			flags &= ~(kFiberConditionTrue | kFiberConditionFalse);
		}
		else
		{
			flags |= kFiberConditionTrue;
		}

		fiber->SetFiberFlags(flags);
		widget->UpdateContent();

		widget = widget->Next();
	}
}

Point3D ScriptEditor::ViewportToGraphPosition(const Point3D& p) const
{
	const Vector2D& scale = scriptViewport->GetOrthoScale();
	OrthoCamera *camera = scriptViewport->GetViewportCamera();
	const Point3D& position = camera->GetNodePosition();

	const OrthoCameraObject *object = camera->GetObject();
	return (Point3D(p.x * scale.x + object->GetOrthoRectLeft() + position.x, p.y * scale.y + object->GetOrthoRectTop() + position.y, 0.0F));
}

Point3D ScriptEditor::AlignPositionToGrid(const Point3D& p)
{
	float x = Floor((p.x + 4.0F) * 0.125F) * 8.0F;
	float y = Floor((p.y + 4.0F) * 0.125F) * 8.0F;
	return (Point3D(x, y, 0.0F));
}

void ScriptEditor::ShowGraphPosition(float x, float y)
{
	const Vector2D& scale = scriptViewport->GetOrthoScale();
	OrthoCamera *camera = scriptViewport->GetViewportCamera();
	const Point3D& position = camera->GetNodePosition();

	float w = scriptViewport->GetWidgetSize().x * scale.x;
	float h = scriptViewport->GetWidgetSize().y * scale.y;

	float left = position.x - w * 0.5F;
	float right = left + w;
	float top = position.y - h * 0.5F;
	float bottom = top + h;

	float dx = 0.0F;
	if (x < left)
	{
		dx = x - left;
	}
	else if (x > right)
	{
		dx = x - right;
	}

	float dy = 0.0F;
	if (y < top)
	{
		dy = y - top;
	}
	else if (y > bottom)
	{
		dy = y - bottom;
	}

	camera->SetNodePosition(position + Vector3D(Floor(dx + 0.5F), Floor(dy + 0.5F), 0.0F));
	editorState |= kScriptEditorUpdateGrid;
}

void ScriptEditor::AutoScroll(const Point3D& p)
{
	float w = scriptViewport->GetWidgetSize().x;
	float h = scriptViewport->GetWidgetSize().y;

	if ((p.x < 0.0F) || (p.y < 0.0F) || (p.x > w) || (p.y > h))
	{
		float dt = TheTimeMgr->GetSystemFloatDeltaTime() * 0.01F;
		float dx = 0.0F;
		float dy = 0.0F;

		if (p.x < 0.0F)
		{
			dx = Fmax((p.x - 15.0F) * dt, w * -0.5F);
		}
		else if (p.x > w)
		{
			dx = Fmin((p.x - w + 16.0F) * dt, w * 0.5F);
		}

		if (p.y < 0.0F)
		{
			dy = Fmax((float) (p.y - 15.0F) * dt, h * -0.5F);
		}
		else if (p.y > h)
		{
			dy = Fmin((float) (p.y - h + 16.0F) * dt, h * 0.5F);
		}

		const Vector2D& scale = scriptViewport->GetOrthoScale();
		OrthoCamera *camera = scriptViewport->GetViewportCamera();
		camera->SetNodePosition(camera->GetNodePosition() + Vector3D(Floor(dx * scale.x + 0.5F), Floor(dy * scale.y + 0.5F), 0.0F));
		editorState |= kScriptEditorUpdateGrid;
	}
}

MethodWidget *ScriptEditor::FindMethodWidget(const Point3D& position) const
{
	WidgetPart	part;

	Widget *widget = currentScriptGraph->graphRoot->DetectWidget(position, 0, &part);
	if ((widget) && (part == kWidgetPartInterior))
	{
		return (static_cast<MethodWidget *>(widget));
	}

	return (nullptr);
}

void ScriptEditor::SortSelectedMethodSublist(List<MethodWidget> *list, float dmin, float dmax)
{
	MethodWidget *widget = list->Last();
	if ((list->First() == widget) || (dmax - dmin < 4.0F))
	{
		while (widget)
		{
			MethodWidget *prev = widget->Previous();
			currentScriptGraph->selectedMethodList.Append(widget);
			widget = prev;
		}
	}
	else
	{
		List<MethodWidget>		nearList;

		float dminFar = dmax;
		float dmaxNear = dmin;
		float avg = (dmin + dmax) * 0.5F;

		widget = list->First();
		do
		{
			MethodWidget *next = widget->Next();
			float d = widget->GetSortPosition();
			if (d < avg)
			{
				dmaxNear = Fmax(dmaxNear, d);
				nearList.Append(widget);
			}
			else
			{
				dminFar = Fmin(dminFar, d);
			}

			widget = next;
		} while (widget);

		SortSelectedMethodSublist(list, dminFar, dmax);
		SortSelectedMethodSublist(&nearList, dmin, dmaxNear);
	}
}

void ScriptEditor::SortSelectedMethodList(float dx, float dy)
{
	List<MethodWidget>		widgetList;

	float dmin = K::infinity;
	float dmax = K::minus_infinity;

	MethodWidget *widget = currentScriptGraph->selectedMethodList.First();
	while (widget)
	{
		MethodWidget *next = widget->Next();

		const Point3D& position = widget->GetWidgetPosition();
		float d = position.x * dx + position.y * dy;
		dmin = Fmin(dmin, d);
		dmax = Fmax(dmax, d);

		widget->SetSortPosition(d);
		widgetList.Append(widget);

		widget = next;
	}

	SortSelectedMethodSublist(&widgetList, dmin, dmax);
}

bool ScriptEditor::BoxIntersectsMethodWidget(const Point3D& p1, const Point3D& p2, const MethodWidget *widget)
{
	const Point3D& position = widget->GetWidgetPosition();

	float x1 = Fmin(p1.x, p2.x);
	float x2 = Fmax(p1.x, p2.x);
	float y1 = Fmin(p1.y, p2.y);
	float y2 = Fmax(p1.y, p2.y);

	if ((x2 < position.x) || (x1 > position.x + kMethodBoxWidth))
	{
		return (false);
	}

	if ((y2 < position.y) || (y1 > position.y + kMethodBoxHeight))
	{
		return (false);
	}

	return (true);
}

bool ScriptEditor::MethodBoxIntersectsAnyMethodWidget(float x, float y, const MethodWidget *exclude)
{
	Point3D p1(x - kMethodBoxPadding, y - kMethodBoxPadding, 0.0F);
	Point3D p2(x + kMethodBoxWidth + kMethodBoxPadding, y + kMethodBoxHeight + kMethodBoxPadding, 0.0F);

	const MethodWidget *widget = currentScriptGraph->methodWidgetList.First();
	while (widget)
	{
		if ((widget != exclude) && (BoxIntersectsMethodWidget(p1, p2, widget)))
		{
			return (true);
		}

		widget = widget->Next();
	}

	widget = currentScriptGraph->selectedMethodList.First();
	while (widget)
	{
		if ((widget != exclude) && (BoxIntersectsMethodWidget(p1, p2, widget)))
		{
			return (true);
		}

		widget = widget->Next();
	}

	return (false);
}

void ScriptEditor::BeginTool(const Point3D& p, unsigned_int32 eventFlags)
{
	previousPoint = p;
	anchorPoint = p;

	int32 tool = trackingTool;
	if (tool == kScriptToolMethodMove)
	{
		WidgetPart	part;

		bool moveable = false;
		bool shift = InterfaceMgr::GetShiftKey();
		anchorPosition = ViewportToGraphPosition(p);
		previousPosition = anchorPosition;

		Widget *widget = currentScriptGraph->graphRoot->DetectWidget(previousPosition, 0, &part);
		if (widget)
		{
			if (part == kWidgetPartCurve)
			{
				FiberWidget *fiberWidget = static_cast<FiberWidget *>(widget);
				bool selected = currentScriptGraph->FiberSelected(fiberWidget);
				if (shift)
				{
					if (!selected)
					{
						SelectFiber(fiberWidget);
					}
					else
					{
						UnselectFiber(fiberWidget);
					}
				}
				else
				{
					if (!selected)
					{
						UnselectAll();
						SelectFiber(fiberWidget);
					}
				}
			}
			else if (part == kWidgetPartOutput)
			{
				UnselectAll();

				MethodWidget *methodWidget = static_cast<MethodWidget *>(widget);
				methodWidget->UpdateOutputColor(true);

				fiberStartMethod = methodWidget;
				fiberFinishMethod = nullptr;

				fiberVertex[0].position = anchorPosition.GetPoint2D();
				fiberVertex[1].position = anchorPosition.GetPoint2D();
				fiberVertex[2].position = anchorPosition.GetPoint2D();
				fiberVertex[3].position = anchorPosition.GetPoint2D();

				fiberVertex[0].tangent.Set(1.0F, 0.0F, 0.0F, -3.0F);
				fiberVertex[1].tangent.Set(1.0F, 0.0F, 0.0F, 4.0F);
				fiberVertex[2].tangent.Set(1.0F, 0.0F, 0.0F, 4.0F);
				fiberVertex[3].tangent.Set(1.0F, 0.0F, 0.0F, -3.0F);

				fiberVertexBuffer.UpdateBuffer(0, sizeof(FiberVertex) * 4, fiberVertex);

				trackingMode = kScriptEditorModeFiber;
			}
			else
			{
				MethodWidget *methodWidget = static_cast<MethodWidget *>(widget);
				bool selected = currentScriptGraph->MethodSelected(methodWidget);
				if (shift)
				{
					if (!selected)
					{
						SelectMethod(methodWidget, kMethodWidgetTempSelected);
					}
					else
					{
						UnselectMethod(methodWidget);
					}
				}
				else
				{
					if (!selected)
					{
						UnselectAll();
						SelectMethod(methodWidget, kMethodWidgetTempSelected);
					}

					if (eventFlags & kMouseDoubleClick)
					{
						HandleGetInfoMenuItem(nullptr, nullptr);
						return;
					}
				}

				moveable = true;
			}
		}
		else
		{
			widget = currentScriptGraph->sectionRoot->DetectWidget(previousPosition, 0, &part);
			if (widget)
			{
				ScriptSectionWidget *sectionWidget = static_cast<ScriptSectionWidget *>(widget);
				bool selected = currentScriptGraph->SectionSelected(sectionWidget);

				if (part == kWidgetPartTitle)
				{
					if (shift)
					{
						if (!selected)
						{
							SelectSection(sectionWidget);
						}
						else
						{
							UnselectSection(sectionWidget);
						}
					}
					else
					{
						if (!selected)
						{
							UnselectAll();
							SelectSection(sectionWidget);
						}

						if (eventFlags & kMouseDoubleClick)
						{
							HandleGetInfoMenuItem(nullptr, nullptr);
							return;
						}
					}

					moveable = true;
				}
				else if (part == kWidgetPartResize)
				{
					if (!selected)
					{
						UnselectAll();
						SelectSection(sectionWidget);
					}

					sectionTrackWidget = sectionWidget;
					anchorPosition = sectionWidget->GetWidgetPosition();
					trackingMode = kScriptEditorModeSection;
					trackingTool = 0;

					AddOperation(new ResizeScriptOperation(sectionWidget));
				}
			}
			else
			{
				if (!shift)
				{
					UnselectAll();
				}

				trackingTool = kScriptToolGraphSelect;
			}
		}

		if (moveable)
		{
			MethodWidget *methodWidget = currentScriptGraph->selectedMethodList.First();
			while (methodWidget)
			{
				methodWidget->SaveOriginalPosition();
				methodWidget = methodWidget->Next();
			}

			ScriptSectionWidget *sectionWidget = currentScriptGraph->selectedSectionList.First();
			while (sectionWidget)
			{
				sectionWidget->SaveOriginalPosition();
				sectionWidget = sectionWidget->Next();
			}

			editorState |= kScriptEditorUndoPending;
		}
	}

	toolTracking = true;
}

void ScriptEditor::TrackTool(const Point3D& p)
{
	int32 tool = trackingTool;
	if (tool == kScriptToolGraphSelect)
	{
		float dx = Fabs(p.x - anchorPoint.x);
		float dy = Fabs(p.y - anchorPoint.y);

		if ((boxSelectFlag) || (dx > 3.0F) || (dy > 3.0F))
		{
			boxSelectFlag = true;
			AutoScroll(p);

			Point3D position = ViewportToGraphPosition(p);
			if (position != previousPosition)
			{
				previousPosition = position;
				dragRect.Build(anchorPosition.GetPoint2D(), position.GetPoint2D(), scriptViewport->GetOrthoScale().x);

				if ((dx >= 1.0F) && (dy >= 1.0F))
				{
					UnselectAllTemp();

					MethodWidget *widget = currentScriptGraph->methodWidgetList.First();
					while (widget)
					{
						MethodWidget *next = widget->Next();

						if (BoxIntersectsMethodWidget(anchorPosition, position, widget))
						{
							SelectMethod(widget, kMethodWidgetTempSelected);
						}

						widget = next;
					}
				}
			}
		}
	}
	else if (tool == kScriptToolMethodMove)
	{
		AutoScroll(p);

		Point3D position = ViewportToGraphPosition(p);
		if (position != previousPosition)
		{
			unsigned_int32 state = editorState;
			if (state & kScriptEditorUndoPending)
			{
				editorState = state & ~kScriptEditorUndoPending;
				AddOperation(new MoveScriptOperation(&currentScriptGraph->selectedMethodList, &currentScriptGraph->selectedSectionList));
			}

			SortSelectedMethodList(position.x - previousPosition.x, position.y - previousPosition.y);

			previousPosition = position;
			float dx = position.x - anchorPosition.x;
			float dy = position.y - anchorPosition.y;

			MethodWidget *methodWidget = currentScriptGraph->selectedMethodList.First();
			while (methodWidget)
			{
				Point3D q = methodWidget->GetOriginalPosition();
				q.x += dx;
				q.y += dy;
				q = AlignPositionToGrid(q);

				bool clear = !MethodBoxIntersectsAnyMethodWidget(q.x, q.y, methodWidget);
				if (!clear)
				{
					const Point3D& currentPosition = methodWidget->GetWidgetPosition();

					if (!MethodBoxIntersectsAnyMethodWidget(q.x, currentPosition.y, methodWidget))
					{
						clear = true;
						q.y = currentPosition.y;
					}

					if ((!clear) && (!MethodBoxIntersectsAnyMethodWidget(currentPosition.x, q.y, methodWidget)))
					{
						clear = true;
						q.x = currentPosition.x;
					}
				}

				if (clear)
				{
					Method *method = methodWidget->GetScriptMethod();

					method->SetMethodPosition(q.GetPoint2D());
					methodWidget->SetWidgetPosition(q);
					methodWidget->Invalidate();

					RebuildFiberWidgets(method);
				}

				methodWidget = methodWidget->Next();
			}

			ScriptSectionWidget *sectionWidget = currentScriptGraph->selectedSectionList.First();
			while (sectionWidget)
			{
				Point3D q = sectionWidget->GetOriginalPosition();
				q.x += dx;
				q.y += dy;
				q = AlignPositionToGrid(q);

				SectionMethod *section = sectionWidget->GetSectionMethod();

				section->SetMethodPosition(q.GetPoint2D());
				sectionWidget->SetWidgetPosition(q);
				sectionWidget->Invalidate();

				sectionWidget = sectionWidget->Next();
			}
		}
	}
	else if (tool == kScriptToolViewportScroll)
	{
		float dx = previousPoint.x - p.x;
		float dy = previousPoint.y - p.y;

		if ((dx != 0.0F) || (dy != 0.0F))
		{
			OrthoCamera *camera = scriptViewport->GetViewportCamera();
			const Vector2D& scale = scriptViewport->GetOrthoScale();
			camera->SetNodePosition(camera->GetNodePosition() + Vector3D(Floor(dx * scale.x + 0.5F), Floor(dy * scale.y + 0.5F), 0.0F));
			editorState |= kScriptEditorUpdateGrid;
		}
	}
	else if (tool == kScriptToolViewportZoom)
	{
		float dy = previousPoint.y - p.y;
		if (dy != 0.0F)
		{
			UpdateViewportScale(scriptViewport->GetOrthoScale().x * Exp(dy * -0.01F));
		}
	}

	previousPoint = p;
}

void ScriptEditor::EndTool(const Point3D& p)
{
	boxSelectFlag = false;
	editorState &= ~kScriptEditorUndoPending;

	MethodWidget *widget = currentScriptGraph->selectedMethodList.First();
	while (widget)
	{
		widget->SetMethodWidgetState(widget->GetMethodWidgetState() & ~kMethodWidgetTempSelected);
		widget = widget->Next();
	}
}

void ScriptEditor::BeginSection(const Point3D& p)
{
	UnselectAll();

	anchorPosition = AlignPositionToGrid(ViewportToGraphPosition(p));
	previousPosition = anchorPosition;

	SectionMethod *section = new SectionMethod;
	currentScriptGraph->AddElement(section);

	ScriptSectionWidget *widget = new ScriptSectionWidget(this, section);
	widget->SetViewportScale(scriptViewport->GetOrthoScale().x);
	currentScriptGraph->sectionRoot->AppendNewSubnode(widget);
	currentScriptGraph->sectionWidgetList.Append(widget);
	sectionTrackWidget = widget;

	section->SetMethodPosition(Point2D(anchorPosition.x, anchorPosition.y));
	widget->SetWidgetPosition(anchorPosition);

	SelectSection(widget);
	AddOperation(new CreateScriptOperation(widget));

	toolTracking = true;
}

void ScriptEditor::TrackSection(const Point3D& p)
{
	AutoScroll(p);

	Point3D position = AlignPositionToGrid(ViewportToGraphPosition(p));
	if (position != previousPosition)
	{
		float width = Fmax(position.x - anchorPosition.x, kMinSectionSize);
		float height = Fmax(position.y - anchorPosition.y, kMinSectionSize);

		SectionMethod *section = sectionTrackWidget->GetSectionMethod();
		section->SetSectionSize(width, height);

		sectionTrackWidget->SetWidgetSize(Vector2D(width, height));
		sectionTrackWidget->Invalidate();

		previousPosition = position;
	}
}

void ScriptEditor::EndSection(const Point3D& p)
{
	TrackSection(p);
	toolButton[kScriptToolMethodMove]->SetValue(1, true);
}

void ScriptEditor::BeginFiber(const Point3D& p)
{
	anchorPosition = ViewportToGraphPosition(p);
	previousPosition = anchorPosition;

	MethodWidget *widget = FindMethodWidget(previousPosition);
	if (widget)
	{
		UnselectAll();
		SelectMethod(widget);

		fiberStartMethod = widget;
		fiberFinishMethod = nullptr;

		fiberVertex[0].position = anchorPosition.GetPoint2D();
		fiberVertex[1].position = anchorPosition.GetPoint2D();
		fiberVertex[2].position = anchorPosition.GetPoint2D();
		fiberVertex[3].position = anchorPosition.GetPoint2D();

		fiberVertex[0].tangent.Set(1.0F, 0.0F, 0.0F, -3.0F);
		fiberVertex[1].tangent.Set(1.0F, 0.0F, 0.0F, 4.0F);
		fiberVertex[2].tangent.Set(1.0F, 0.0F, 0.0F, 4.0F);
		fiberVertex[3].tangent.Set(1.0F, 0.0F, 0.0F, -3.0F);

		fiberVertexBuffer.UpdateBuffer(0, sizeof(FiberVertex) * 4, fiberVertex);

		toolTracking = true;
	}
}

void ScriptEditor::TrackFiber(const Point3D& p)
{
	AutoScroll(p);

	Point3D position = ViewportToGraphPosition(p);
	if (position != previousPosition)
	{
		previousPosition = position;

		fiberVertex[2].position = position.GetPoint2D();
		fiberVertex[3].position = position.GetPoint2D();

		Vector3D tangent = Normalize(position - anchorPosition);
		fiberVertex[0].tangent.Set(tangent, -3.0F);
		fiberVertex[1].tangent.Set(tangent, 4.0F);
		fiberVertex[2].tangent.Set(tangent, 4.0F);
		fiberVertex[3].tangent.Set(tangent, -3.0F);

		fiberVertexBuffer.UpdateBuffer(0, sizeof(FiberVertex) * 4, fiberVertex);

		if (fiberFinishMethod)
		{
			UnselectMethod(fiberFinishMethod);
			fiberFinishMethod = nullptr;
		}

		MethodWidget *widget = FindMethodWidget(position);
		if ((widget) && (widget != fiberStartMethod))
		{
			const Method *start = fiberStartMethod->GetScriptMethod();
			const Method *finish = widget->GetScriptMethod();

			const Fiber *fiber = start->GetFirstOutgoingEdge();
			while (fiber)
			{
				if (fiber->GetFinishElement() == finish)
				{
					return;
				}

				fiber = fiber->GetNextOutgoingEdge();
			}

			SelectMethod(widget);
			fiberFinishMethod = widget;
		}
	}
}

void ScriptEditor::EndFiber(const Point3D& p)
{
	fiberStartMethod->UpdateOutputColor(false);

	if (fiberFinishMethod)
	{
		UnselectAll();

		Method *start = fiberStartMethod->GetScriptMethod();
		Method *finish = fiberFinishMethod->GetScriptMethod();

		Fiber *fiber = new Fiber(start, finish);

		if (start->GetMethodType() == kMethodEvent)
		{
			fiber->SetFiberFlags(kFiberConditionTrue);
		}

		FiberWidget *widget = new FiberWidget(this, fiber);
		widget->SetViewportScale(scriptViewport->GetOrthoScale().x);
		currentScriptGraph->graphRoot->PrependSubnode(widget);
		currentScriptGraph->fiberWidgetList.Append(widget);
		widget->Preprocess();

		SelectFiber(widget);
		AddOperation(new CreateScriptOperation(widget));

		editorState |= kScriptEditorUpdateGraph;
	}
}

void ScriptEditor::CreateMethod(const Point3D& p)
{
	Point3D position = AlignPositionToGrid(ViewportToGraphPosition(p));

	if (!MethodBoxIntersectsAnyMethodWidget(position.x, position.y))
	{
		Method			*method;
		MethodWidget	*widget;

		UnselectAll();

		if (currentMode == kScriptEditorModeMethod)
		{
			method = Method::New(currentMethodReg->GetMethodType());
			widget = new MethodWidget(this, method, currentMethodReg);
		}
		else
		{
			method = new EventMethod(currentEventType);
			widget = new MethodWidget(this, method, nullptr);
		}

		currentScriptGraph->AddElement(method);
		widget->SetViewportScale(scriptViewport->GetOrthoScale().x);
		currentScriptGraph->graphRoot->AppendNewSubnode(widget);
		currentScriptGraph->methodWidgetList.Append(widget);

		method->SetMethodPosition(Point2D(position.x, position.y));
		widget->SetWidgetPosition(position);

		SelectMethod(widget);
		AddOperation(new CreateScriptOperation(widget));

		toolButton[kScriptToolMethodMove]->SetValue(1, true);
		editorState |= kScriptEditorUpdateGraph;
	}
}

void ScriptEditor::ViewportHandleMouseEvent(const MouseEventData *eventData, ViewportWidget *viewport, void *cookie)
{
	ScriptEditor *scriptEditor = static_cast<ScriptEditor *>(cookie);

	EventType eventType = eventData->eventType;
	if ((eventType == kEventMouseDown) || (eventType == kEventMiddleMouseDown))
	{
		int32 mode = scriptEditor->currentMode;
		int32 tool = scriptEditor->currentTool;

		if ((eventType == kEventMiddleMouseDown) || (InterfaceMgr::GetOptionKey()))
		{
			mode = kScriptEditorModeTool;
			tool = kScriptToolViewportScroll;
		}

		scriptEditor->trackingMode = mode;
		scriptEditor->trackingTool = tool;
		scriptEditor->toolTracking = false;

		if (mode == kScriptEditorModeTool)
		{
			scriptEditor->BeginTool(eventData->mousePosition, eventData->eventFlags);
		}
		else if ((mode == kScriptEditorModeMethod) || (mode == kScriptEditorModeEvent))
		{
			scriptEditor->CreateMethod(eventData->mousePosition);
		}
		else if (mode == kScriptEditorModeSection)
		{
			scriptEditor->BeginSection(eventData->mousePosition);
		}
		else if (mode == kScriptEditorModeFiber)
		{
			scriptEditor->BeginFiber(eventData->mousePosition);
		}
	}
	else if ((eventType == kEventMouseUp) || (eventType == kEventMiddleMouseUp))
	{
		if (scriptEditor->toolTracking)
		{
			int32 mode = scriptEditor->trackingMode;
			if (mode == kScriptEditorModeTool)
			{
				scriptEditor->EndTool(eventData->mousePosition);
			}
			else if (mode == kScriptEditorModeSection)
			{
				scriptEditor->EndSection(eventData->mousePosition);
			}
			else if (mode == kScriptEditorModeFiber)
			{
				scriptEditor->EndFiber(eventData->mousePosition);
			}

			scriptEditor->toolTracking = false;
		}
	}
	else if (eventType == kEventMouseWheel)
	{
		if (!scriptEditor->toolTracking)
		{
			OrthoViewportWidget *orthoViewport = static_cast<OrthoViewportWidget *>(viewport);
			scriptEditor->UpdateViewportScale(orthoViewport->GetOrthoScale().x * Exp(eventData->wheelDelta.y * -0.16F), &eventData->mousePosition);
		}
	}
}

void ScriptEditor::ViewportTrackTask(const Point3D& position, ViewportWidget *viewport, void *cookie)
{
	ScriptEditor *scriptEditor = static_cast<ScriptEditor *>(cookie);
	if (scriptEditor->toolTracking)
	{
		int32 mode = scriptEditor->trackingMode;
		if (mode == kScriptEditorModeTool)
		{
			scriptEditor->TrackTool(position);
		}
		else if (mode == kScriptEditorModeSection)
		{
			scriptEditor->TrackSection(position);
		}
		else if (mode == kScriptEditorModeFiber)
		{
			scriptEditor->TrackFiber(position);
		}
	}
}

void ScriptEditor::ViewportRender(List<Renderable> *renderList, ViewportWidget *viewport, void *cookie)
{
	ScriptEditor *scriptEditor = static_cast<ScriptEditor *>(cookie);

	unsigned_int32 state = scriptEditor->editorState;
	if (state & kScriptEditorUpdateGrid)
	{
		scriptEditor->editorState = state & ~kScriptEditorUpdateGrid;

		const OrthoViewportWidget *orthoViewport = scriptEditor->scriptViewport;
		const OrthoCamera *camera = orthoViewport->GetViewportCamera();
		const OrthoCameraObject *cameraObject = camera->GetObject();

		const Point3D& position = camera->GetNodePosition();
		float xmin = cameraObject->GetOrthoRectLeft() + position.x;
		float xmax = cameraObject->GetOrthoRectRight() + position.x;
		float ymin = cameraObject->GetOrthoRectTop() + position.y;
		float ymax = cameraObject->GetOrthoRectBottom() + position.y;

		scriptEditor->viewportGrid.Build(Point2D(xmin, ymin), Point2D(xmax, ymax), orthoViewport->GetOrthoScale().x);
	}

	renderList->Append(&scriptEditor->viewportGrid);

	Widget *widget = scriptEditor->currentScriptGraph->sectionRoot;
	widget->Update();
	widget->RenderTree(renderList);

	widget = scriptEditor->currentScriptGraph->graphRoot;
	widget->Update();
	widget->RenderTree(renderList);

	if (scriptEditor->boxSelectFlag)
	{
		renderList->Append(&scriptEditor->dragRect);
	}
	else if ((scriptEditor->toolTracking) && (scriptEditor->trackingMode == kScriptEditorModeFiber))
	{
		renderList->Append(&scriptEditor->fiberRenderable);
	}
}

void ScriptEditor::HandleToolButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		SetFocusWidget(nullptr);

		for (machine a = 0; a < kScriptToolCount; a++)
		{
			if (widget == toolButton[a])
			{
				if ((currentMode != kScriptEditorModeTool) || (currentTool != a))
				{
					UnselectCurrentTool();

					currentMode = kScriptEditorModeTool;
					currentTool = a;
				}

				break;
			}
		}
	}
}

void ScriptEditor::CloseDialogComplete(Dialog *dialog, void *cookie)
{
	ScriptEditor *scriptEditor = static_cast<ScriptEditor *>(cookie);

	int32 status = dialog->GetDialogStatus();
	if (status == kDialogOkay)
	{
		scriptEditor->HandleSaveScriptMenuItem(nullptr, nullptr);
		scriptEditor->CallCompletionProc();
		scriptEditor->Window::Close();
	}
	else if (status == kDialogIgnore)
	{
		scriptEditor->CallCompletionProc();
		scriptEditor->Window::Close();
	}
}

void ScriptEditor::Move(void)
{
	if ((TheInterfaceMgr->GetActiveWindow() == this) && (!TheInterfaceMgr->GetActiveMenu()))
	{
		const Point3D& position = TheInterfaceMgr->GetCursorPosition();

		bool toolCursor = toolTracking;
		if (!toolCursor)
		{
			Vector2D vp = position.GetVector2D() - scriptViewport->GetWorldPosition().GetVector2D();
			if ((vp.x >= 0.0F) && (vp.x < scriptViewport->GetWidgetSize().x) && (vp.y >= 0.0F) && (vp.y < scriptViewport->GetWidgetSize().y))
			{
				toolCursor = true;
			}
		}

		int32 cursorIndex = kEditorCursorArrow;
		if (toolCursor)
		{
			int32 mode = currentMode;
			int32 tool = currentTool;

			if (!toolTracking)
			{
				if (InterfaceMgr::GetOptionKey())
				{
					mode = kScriptEditorModeTool;
					tool = kScriptToolViewportScroll;
				}
			}
			else
			{
				if ((trackingMode != kScriptEditorModeSection) || (currentMode == kScriptEditorModeSection))
				{
					mode = trackingMode;
					tool = trackingTool;
				}
			}

			if (mode == kScriptEditorModeTool)
			{
				if (tool == kScriptToolViewportScroll)
				{
					cursorIndex = (toolTracking) ? kEditorCursorDrag : kEditorCursorHand;
				}
				else if (tool == kScriptToolViewportZoom)
				{
					cursorIndex = kEditorCursorGlass;
				}
			}
			else
			{
				cursorIndex = kEditorCursorCross;
				if ((!toolTracking) && (mode == kScriptEditorModeMethod))
				{
					Point3D p = AlignPositionToGrid(ViewportToGraphPosition(position - scriptViewport->GetWorldPosition().GetVector3D()));
					if (MethodBoxIntersectsAnyMethodWidget(p.x, p.y))
					{
						cursorIndex = kEditorCursorStop;
					}
				}
			}
		}

		TheInterfaceMgr->SetCursor(TheWorldEditor->GetEditorCursor(cursorIndex));
	}

	if (editorState & kScriptEditorUpdateMenus)
	{
		bool methodSelection = !currentScriptGraph->selectedMethodList.Empty();
		bool fiberSelection = !currentScriptGraph->selectedFiberList.Empty();
		bool sectionSelection = !currentScriptGraph->selectedSectionList.Empty();

		MenuItemWidget *getInfoItem = scriptMenuItem[kScriptMenuGetInfo];
		getInfoItem->Disable();

		if ((methodSelection) || (sectionSelection))
		{
			scriptMenuItem[kScriptMenuCut]->Enable();
			scriptMenuItem[kScriptMenuCopy]->Enable();
			scriptMenuItem[kScriptMenuDuplicate]->Enable();

			if (methodSelection)
			{
				if (!(fiberSelection | sectionSelection))
				{
					const MethodWidget *methodWidget = currentScriptGraph->selectedMethodList.First();
					if ((methodWidget->GetScriptMethod()->GetMethodType() != kMethodEvent) && (currentScriptGraph->selectedMethodList.Last() == methodWidget))
					{
						getInfoItem->Enable();
					}
				}
			}
			else
			{
				if ((!fiberSelection) && (currentScriptGraph->selectedSectionList.Last() == currentScriptGraph->selectedSectionList.First()))
				{
					getInfoItem->Enable();
				}
			}
		}
		else
		{
			scriptMenuItem[kScriptMenuCut]->Disable();
			scriptMenuItem[kScriptMenuCopy]->Disable();
			scriptMenuItem[kScriptMenuDuplicate]->Disable();
		}

		MenuItemWidget *menuItem = scriptMenuItem[kScriptMenuCycleFiberCondition];
		if (fiberSelection)
		{
			menuItem->Enable();
		}
		else
		{
			menuItem->Disable();
		}

		menuItem = scriptMenuItem[kScriptMenuUndo];
		if (!currentScriptGraph->operationList.Empty())
		{
			menuItem->Enable();
		}
		else
		{
			menuItem->Disable();
		}

		menuItem = scriptMenuItem[kScriptMenuClear];
		if (methodSelection | fiberSelection | sectionSelection)
		{
			menuItem->Enable();
		}
		else
		{
			menuItem->Disable();
		}
	}

	if (editorState & kScriptEditorUpdateGraph)
	{
		UpdateScriptGraph();
	}

	editorState &= ~(kScriptEditorUpdateMenus | kScriptEditorUpdateGraph);

	currentScriptGraph->graphRoot->Move();
	currentScriptGraph->sectionRoot->Move();
	Window::Move();
}

void ScriptEditor::EnterForeground(void)
{
	Window::EnterForeground();

	if (!editorClipboard.Empty())
	{
		scriptMenuItem[kScriptMenuPaste]->Enable();
	}
}

void ScriptEditor::EnterBackground(void)
{
	Window::EnterBackground();
	TheInterfaceMgr->SetCursor(nullptr);
}

bool ScriptEditor::HandleKeyboardEvent(const KeyboardEventData *eventData)
{
	EventType eventType = eventData->eventType;

	if (eventType == kEventKeyDown)
	{
		if (toolTracking)
		{
			return (true);
		}

		unsigned_int32 keyCode = eventData->keyCode;
		if ((keyCode >= '1') && (keyCode <= '7'))
		{
			if (keyCode == '1')
			{
				toolButton[kScriptToolMethodMove]->SetValue(1, true);
			}
			else if (keyCode == '6')
			{
				toolButton[kScriptToolViewportScroll]->SetValue(1, true);
			}
			else if (keyCode == '7')
			{
				toolButton[kScriptToolViewportZoom]->SetValue(1, true);
			}

			return (true);
		}
		else if ((keyCode == kKeyCodeDelete) || (keyCode == kKeyCodeBackspace))
		{
			HandleClearMenuItem(nullptr, nullptr);
			return (true);
		}
	}
	else if (eventType == kEventKeyCommand)
	{
		if (toolTracking)
		{
			return (true);
		}

		unsigned_int32 keyCode = eventData->keyCode;
		if ((keyCode >= '1') && (keyCode <= '9'))
		{
			int32 index = keyCode - '1';
			if (index < scriptGraphList.GetElementCount())
			{
				graphsPage->SelectScriptGraph(index);
			}
		}
	}

	return (Window::HandleKeyboardEvent(eventData));
}

void ScriptEditor::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		if (widget == sectionButton)
		{
			SetFocusWidget(nullptr);

			if (currentMode != kScriptEditorModeSection)
			{
				UnselectCurrentTool();

				currentMode = kScriptEditorModeSection;
				currentTool = 0;
			}
		}
	}
}

void ScriptEditor::Close(void)
{
	if (editorState & kScriptEditorUnsaved)
	{
		const StringTable *table = TheWorldEditor->GetStringTable();

		const char *title = table->GetString(StringID('SCPT', 'SAVE'));
		const char *okayText = table->GetString(StringID('BTTN', 'SAVE'));
		const char *cancelText = table->GetString(StringID('BTTN', 'CANC'));
		const char *ignoreText = table->GetString(StringID('BTTN', 'DSAV'));

		Dialog *dialog = new Dialog(Vector2D(342.0F, 120.0F), title, okayText, cancelText, ignoreText);
		dialog->SetCompletionProc(&CloseDialogComplete, this);
		dialog->SetIgnoreKeyCode('n');

		PushButtonWidget *button = dialog->GetIgnoreButton();
		const Point3D& position = button->GetWidgetPosition();
		button->SetWidgetPosition(Point3D(position.x + 8.0F, position.y, position.z));
		button->SetWidgetSize(Vector2D(100.0F, button->GetWidgetSize().y));

		ImageWidget *image = new ImageWidget(Vector2D(64.0F, 64.0F), "C4/warning");
		image->SetWidgetPosition(Point3D(12.0F, 12.0F, 0.0F));
		dialog->AppendSubnode(image);

		TextWidget *text = new TextWidget(Vector2D(242.0F, 0.0F), table->GetString(StringID('SCPT', 'CFRM')), "font/Gui");
		text->SetTextFlags(kTextWrapped);
		text->SetWidgetPosition(Point3D(88.0F, 16.0F, 0.0F));
		dialog->AppendSubnode(text);

		AddSubwindow(dialog);
	}
	else
	{
		CallCompletionProc();
		Window::Close();
	}
}

// ZYUQURM
