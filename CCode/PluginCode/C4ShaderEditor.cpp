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


#include "C4ShaderEditor.h"
#include "C4WorldEditor.h"
#include "C4Dialog.h"
#include "C4World.h"


using namespace C4;


namespace
{
	enum
	{
		kMaxShaderOperationCount = 20
	};


	enum
	{
		kWidgetPartPort			= 'PRT0',
		kWidgetPartOutput		= 'OUTP',
		kWidgetPartCurve		= 'CURV'
	};


	const float kProcessBoxWidth = 120.0F;
	const float kProcessBoxHeight = 98.0F;
	const float kProcessBoxThickness = 12.0F;
	const float kProcessBoxPaddingX = 72.0F;
	const float kProcessBoxPaddingY = 24.0F;

	const float kOutputDotRadius = 7.5F;

	const float kPortBoxWidth = 32.0F;
	const float kPortBoxHeight = 16.0F;
	const float kPortBoxOutdent = -8.0F;

	const float kSwizzleBoxWidth = 40.0F;
	const float kSwizzleBoxHeight = 16.0F;

	const float kSectionTitleHeight = 18.0F;
	const float kMinSectionSize = 40.0F;


	const TextureHeader portTextureHeader =
	{
		kTexture2D,
		kTextureForceHighQuality,
		kTextureSemanticEmission,
		kTextureSemanticNone,
		kTextureL8,
		8, 8, 1,
		{kTextureRepeat, kTextureRepeat, kTextureRepeat},
		4
	};


	const TextureHeader routeTextureHeader =
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


	const unsigned_int8 portTextureImage[85] =
	{
		0xFF, 0xFF, 0xFF, 0xFF, 0xC8, 0xC8, 0xC8, 0xC8,
		0xFF, 0xFF, 0xFF, 0xC8, 0xC8, 0xC8, 0xC8, 0xFF,
		0xFF, 0xFF, 0xC8, 0xC8, 0xC8, 0xC8, 0xFF, 0xFF,
		0xFF, 0xC8, 0xC8, 0xC8, 0xC8, 0xFF, 0xFF, 0xFF,
		0xC8, 0xC8, 0xC8, 0xC8, 0xFF, 0xFF, 0xFF, 0xFF,
		0xC8, 0xC8, 0xC8, 0xFF, 0xFF, 0xFF, 0xFF, 0xC8,
		0xC8, 0xC8, 0xFF, 0xFF, 0xFF, 0xFF, 0xC8, 0xC8,
		0xC8, 0xFF, 0xFF, 0xFF, 0xFF, 0xC8, 0xC8, 0xC8,
		0xFF, 0xF2, 0xC8, 0xD6,
		0xF2, 0xC8, 0xD6, 0xFF,
		0xC8, 0xD6, 0xFF, 0xF2,
		0xD6, 0xFF, 0xF2, 0xC8,
		0xEB, 0xDD,
		0xDD, 0xEB,
		0xE4
	};


	const unsigned_int8 routeTextureImage[171] =
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
 
 
SharedVertexBuffer ProcessWidget::processIndexBuffer(kVertexBufferIndex | kVertexBufferStatic);
SharedVertexBuffer ProcessWidget::backgroundVertexBuffer(kVertexBufferAttribute | kVertexBufferStatic);
SharedVertexBuffer ProcessWidget::outputVertexBuffer(kVertexBufferAttribute | kVertexBufferStatic);
SharedVertexBuffer ProcessWidget::portIndexBuffer(kVertexBufferIndex | kVertexBufferStatic); 

SharedVertexBuffer RouteWidget::swizzleIndexBuffer(kVertexBufferIndex | kVertexBufferStatic);

SharedVertexBuffer ShaderSectionWidget::sectionIndexBuffer(kVertexBufferIndex | kVertexBufferStatic);

ShaderGraph ShaderEditor::editorClipboard;


ShaderOperation::ShaderOperation()
{
	coupledFlag = false;
}

ShaderOperation::~ShaderOperation()
{
}


CreateShaderOperation::CreateShaderOperation(ProcessWidget *process)
{
	createdProcessList.Append(new ProcessReference(process));
}

CreateShaderOperation::CreateShaderOperation(RouteWidget *route)
{
	createdRouteList.Append(new RouteReference(route));
}

CreateShaderOperation::CreateShaderOperation(ShaderSectionWidget *section)
{
	createdSectionList.Append(new ShaderSectionReference(section));
}

CreateShaderOperation::CreateShaderOperation(const List<ProcessWidget> *processList, const List<RouteWidget> *routeList, const List<ShaderSectionWidget> *sectionList)
{
	ProcessWidget *processWidget = processList->First();
	while (processWidget)
	{
		createdProcessList.Append(new ProcessReference(processWidget));
		processWidget = processWidget->Next();
	}

	RouteWidget *routeWidget = routeList->First();
	while (routeWidget)
	{
		createdRouteList.Append(new RouteReference(routeWidget));
		routeWidget = routeWidget->Next();
	}

	ShaderSectionWidget *sectionWidget = sectionList->First();
	while (sectionWidget)
	{
		createdSectionList.Append(new ShaderSectionReference(sectionWidget));
		sectionWidget = sectionWidget->Next();
	}
}

CreateShaderOperation::~CreateShaderOperation()
{
}

void CreateShaderOperation::Restore(ShaderEditor *shaderEditor)
{
	const RouteReference *routeReference = createdRouteList.First();
	while (routeReference)
	{
		shaderEditor->DeleteRoute(routeReference->GetRouteWidget());
		routeReference = routeReference->Next();
	}

	const ProcessReference *processReference = createdProcessList.First();
	while (processReference)
	{
		shaderEditor->DeleteProcess(processReference->GetProcessWidget());
		processReference = processReference->Next();
	}

	const ShaderSectionReference *sectionReference = createdSectionList.First();
	while (sectionReference)
	{
		shaderEditor->DeleteSection(sectionReference->GetSectionWidget());
		sectionReference = sectionReference->Next();
	}
}


DeleteShaderOperation::DeleteShaderOperation(List<ProcessWidget> *processList, List<RouteWidget> *routeList, List<ShaderSectionWidget> *sectionList)
{
	if (processList)
	{
		for (;;)
		{
			ProcessWidget *widget = processList->First();
			if (!widget)
			{
				break;
			}

			deletedProcessList.Append(widget);
		}
	}

	if (routeList)
	{
		for (;;)
		{
			RouteWidget *widget = routeList->First();
			if (!widget)
			{
				break;
			}

			deletedRouteList.Append(widget);
		}
	}

	if (sectionList)
	{
		for (;;)
		{
			ShaderSectionWidget *widget = sectionList->First();
			if (!widget)
			{
				break;
			}

			deletedSectionList.Append(widget);
		}
	}
}

DeleteShaderOperation::~DeleteShaderOperation()
{
	const RouteWidget *routeWidget = deletedRouteList.First();
	while (routeWidget)
	{
		delete routeWidget->GetShaderRoute();
		routeWidget = routeWidget->Next();
	}

	const ProcessWidget *processWidget = deletedProcessList.First();
	while (processWidget)
	{
		delete processWidget->GetShaderProcess();
		processWidget = processWidget->Next();
	}

	const ShaderSectionWidget *sectionWidget = deletedSectionList.First();
	while (sectionWidget)
	{
		delete sectionWidget->GetSectionProcess();
		sectionWidget = sectionWidget->Next();
	}
}

void DeleteShaderOperation::Restore(ShaderEditor *shaderEditor)
{
	shaderEditor->UnselectAll();

	for (;;)
	{
		ProcessWidget *widget = deletedProcessList.First();
		if (!widget)
		{
			break;
		}

		shaderEditor->ReattachProcess(widget);
	}

	for (;;)
	{
		RouteWidget *widget = deletedRouteList.First();
		if (!widget)
		{
			break;
		}

		shaderEditor->ReattachRoute(widget);
	}

	for (;;)
	{
		ShaderSectionWidget *widget = deletedSectionList.First();
		if (!widget)
		{
			break;
		}

		shaderEditor->ReattachSection(widget);
	}
}


MoveShaderOperation::MoveShaderOperation(const List<ProcessWidget> *processList, const List<ShaderSectionWidget> *sectionList)
{
	ProcessWidget *processWidget = processList->First();
	while (processWidget)
	{
		movedProcessList.Append(new MovedProcessReference(processWidget));
		processWidget = processWidget->Next();
	}

	ShaderSectionWidget *sectionWidget = sectionList->First();
	while (sectionWidget)
	{
		movedSectionList.Append(new MovedSectionReference(sectionWidget));
		sectionWidget = sectionWidget->Next();
	}
}

MoveShaderOperation::~MoveShaderOperation()
{
}

MoveShaderOperation::MovedProcessReference::MovedProcessReference(ProcessWidget *element) : ProcessReference(element)
{
	position = element->GetShaderProcess()->GetProcessPosition();
}

MoveShaderOperation::MovedSectionReference::MovedSectionReference(ShaderSectionWidget *element) : ShaderSectionReference(element)
{
	position = element->GetSectionProcess()->GetProcessPosition();
}

void MoveShaderOperation::Restore(ShaderEditor *shaderEditor)
{
	const ProcessReference *processReference = movedProcessList.First();
	while (processReference)
	{
		const MovedProcessReference *moved = static_cast<const MovedProcessReference *>(processReference);

		ProcessWidget *widget = moved->GetProcessWidget();
		Process *process = widget->GetShaderProcess();

		const Point2D& p = moved->GetPosition();
		process->SetProcessPosition(Point2D(p.x, p.y));
		widget->SetWidgetPosition(Point3D(p.x, p.y, 0.0F));
		widget->Invalidate();

		shaderEditor->RebuildRouteWidgets(process);

		processReference = processReference->Next();
	}

	const ShaderSectionReference *sectionReference = movedSectionList.First();
	while (sectionReference)
	{
		const MovedSectionReference *moved = static_cast<const MovedSectionReference *>(sectionReference);

		ShaderSectionWidget *widget = moved->GetSectionWidget();
		SectionProcess *section = widget->GetSectionProcess();

		const Point2D& p = moved->GetPosition();
		section->SetProcessPosition(Point2D(p.x, p.y));
		widget->SetWidgetPosition(Point3D(p.x, p.y, 0.0F));
		widget->Invalidate();

		sectionReference = sectionReference->Next();
	}
}


ResizeShaderOperation::ResizeShaderOperation(ShaderSectionWidget *widget)
{
	sectionWidget = widget;

	const SectionProcess *section = widget->GetSectionProcess();
	sectionWidth = section->GetSectionWidth();
	sectionHeight = section->GetSectionHeight();
}

ResizeShaderOperation::~ResizeShaderOperation()
{
}

void ResizeShaderOperation::Restore(ShaderEditor *shaderEditor)
{
	SectionProcess *section = sectionWidget->GetSectionProcess();
	section->SetSectionSize(sectionWidth, sectionHeight);

	sectionWidget->SetWidgetSize(Vector2D(sectionWidth, sectionHeight));
	sectionWidget->Invalidate();
}


RouteShaderOperation::RouteShaderOperation(const List<RouteWidget> *selectionList)
{
	RouteWidget *widget = selectionList->First();
	while (widget)
	{
		routeList.Append(new DetailedReference(widget));
		widget = widget->Next();
	}
}

RouteShaderOperation::~RouteShaderOperation()
{
}

RouteShaderOperation::DetailedReference::DetailedReference(RouteWidget *widget) : RouteReference(widget)
{
	flags = widget->GetShaderRoute()->GetRouteFlags();
}

void RouteShaderOperation::Restore(ShaderEditor *shaderEditor)
{
	const RouteReference *reference = routeList.First();
	while (reference)
	{
		const DetailedReference *detailed = static_cast<const DetailedReference *>(reference);

		RouteWidget *widget = detailed->GetRouteWidget();
		widget->GetShaderRoute()->SetRouteFlags(detailed->GetFlags());
		widget->Rebuild();

		reference = reference->Next();
	}
}


ProcessWidget::ProcessWidget(ShaderEditor *editor, Process *process, const ProcessRegistration *registration) :
		TextWidget(kWidgetProcess, nullptr, "font/Gui"),
		processVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		processRenderable(kRenderIndexedTriangles),
		backgroundColorAttribute(kAttributeMutable),
		backgroundTextureAttribute("ShaderEditor/Graph"),
		backgroundRenderable(kRenderTriangleStrip),
		outputColorAttribute(kAttributeMutable),
		outputTextureAttribute("ShaderEditor/Output"),
		outputRenderable(kRenderTriangleStrip),
		portVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		portTextureAttribute(&portTextureHeader, portTextureImage),
		portRenderable(kRenderIndexedTriangles)
{
	shaderEditor = editor;
	shaderProcess = process;
	processRegistration = registration;

	processWidgetState = 0;
	viewportScale = 1.0F;
	hilitePortIndex = -1;

	SetWidgetSize(Vector2D(kProcessBoxWidth, kProcessBoxHeight));
	SetTextAlignment(kTextAlignCenter);
	SetTextFlags(kTextWrapped);
	SetTextLeading(-2.0F);
	SetRenderLineCount(4);
	SetTextRenderOffset(Vector3D(0.0F, 1.0F, 0.0F));

	commentText = nullptr;
	valueText = nullptr;
	colorBox = nullptr;
	textureBox = nullptr;

	UpdateContent();

	InitRenderable(&processRenderable);
	processRenderable.SetVertexCount(16);
	processRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &processVertexBuffer, sizeof(ProcessVertex));
	processRenderable.SetVertexAttributeArray(kArrayPosition, 0, 2);
	processRenderable.SetVertexAttributeArray(kArrayColor, sizeof(Point2D), 4);
	processVertexBuffer.Establish(sizeof(ProcessVertex) * 16);

	if (processIndexBuffer.Retain() == 1)
	{
		static const Triangle processTriangle[16] =
		{
			{{ 0,  4,  1}}, {{ 1,  4,  5}}, {{ 1,  5,  2}}, {{ 2,  5,  6}},
			{{ 2,  6,  3}}, {{ 3,  6,  7}}, {{ 3,  7,  0}}, {{ 0,  7,  4}},
			{{ 8, 12,  9}}, {{ 9, 12, 13}}, {{ 9, 13, 10}}, {{10, 13, 14}},
			{{10, 14, 11}}, {{11, 14, 15}}, {{11, 15,  8}}, {{ 8, 15, 12}}
		};

		processIndexBuffer.Establish(sizeof(Triangle) * 16, processTriangle);
	}

	processRenderable.SetPrimitiveCount(8);
	processRenderable.SetVertexBuffer(kVertexBufferIndexArray, &processIndexBuffer);

	InitRenderable(&backgroundRenderable);
	backgroundRenderable.SetVertexCount(4);
	backgroundRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &backgroundVertexBuffer, sizeof(BackgroundVertex));
	backgroundRenderable.SetVertexAttributeArray(kArrayPosition, 0, 2);
	backgroundRenderable.SetVertexAttributeArray(kArrayTexcoord, sizeof(Point2D), 2);

	if (backgroundVertexBuffer.Retain() == 1)
	{
		static const BackgroundVertex backgroundVertex[4] =
		{
			{Point2D(-9.0F, -5.0F), Point2D(0.0F, 116.0F)},
			{Point2D(-9.0F, 111.0F), Point2D(0.0F, 0.0F)},
			{Point2D(129.0F, -5.0F), Point2D(138.0F, 116.0F)},
			{Point2D(129.0F, 111.0F), Point2D(138.0F, 0.0F)}
		};

		backgroundVertexBuffer.Establish(sizeof(BackgroundVertex) * 4, backgroundVertex);
	}

	backgroundAttributeList.Append(&backgroundColorAttribute);
	backgroundAttributeList.Append(&backgroundTextureAttribute);
	backgroundRenderable.SetMaterialAttributeList(&backgroundAttributeList);

	float rb = (process->GetBaseProcessType() != kProcessOutput) ? 1.0F : 0.625F;
	backgroundColorAttribute.SetDiffuseColor(ColorRGBA(rb, 1.0F, rb, 1.0F));

	if (outputVertexBuffer.Retain() == 1)
	{
		static const OutputVertex outputVertex[4] =
		{
			{Point2D(113.0F, 41.0F), Point2D(0.0F, 17.0F)},
			{Point2D(113.0F, 58.0F), Point2D(0.0F, 0.0F)},
			{Point2D(130.0F, 41.0F), Point2D(17.0F, 17.0F)},
			{Point2D(130.0F, 58.0F), Point2D(17.0F, 0.0F)}
		};

		outputVertexBuffer.Establish(sizeof(OutputVertex) * 4, outputVertex);
	}

	ProcessType type = process->GetBaseProcessType();
	if ((type != kProcessOutput) && (type != kProcessTerminal))
	{
		InitRenderable(&outputRenderable);
		outputRenderable.SetVertexCount(4);
		outputRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &outputVertexBuffer, sizeof(OutputVertex));
		outputRenderable.SetVertexAttributeArray(kArrayPosition, 0, 2);
		outputRenderable.SetVertexAttributeArray(kArrayTexcoord, sizeof(Point2D), 2);

		outputAttributeList.Append(&outputColorAttribute);
		outputAttributeList.Append(&outputTextureAttribute);
		outputRenderable.SetMaterialAttributeList(&outputAttributeList);

		UpdateOutputColor(false);
	}
	else
	{
		outputRenderable.SetVertexCount(0);
	}

	int32 portCount = process->GetPortCount();
	portRenderable.SetVertexCount(portCount * 12);

	if (portIndexBuffer.Retain() == 1)
	{
		static const Triangle portTriangle[kMaxProcessPortCount * 10] =
		{
			{{ 0,  1,  2}}, {{ 0,  2,  3}}, {{ 4,  8,  5}}, {{ 5,  8,  9}}, {{ 5,  9,  6}},
			{{ 6,  9, 10}}, {{ 6, 10,  7}}, {{ 7, 10, 11}}, {{ 7, 11,  4}}, {{ 4, 11,  8}},
			{{12, 13, 14}}, {{12, 14, 15}}, {{16, 20, 17}}, {{17, 20, 21}}, {{17, 21, 18}},
			{{18, 21, 22}}, {{18, 22, 19}}, {{19, 22, 23}}, {{19, 23, 16}}, {{16, 23, 20}},
			{{24, 25, 26}}, {{24, 26, 27}}, {{28, 32, 29}}, {{29, 32, 33}}, {{29, 33, 30}},
			{{30, 33, 34}}, {{30, 34, 31}}, {{31, 34, 35}}, {{31, 35, 28}}, {{28, 35, 32}},
			{{36, 37, 38}}, {{36, 38, 39}}, {{40, 44, 41}}, {{41, 44, 45}}, {{41, 45, 42}},
			{{42, 45, 46}}, {{42, 46, 43}}, {{43, 46, 47}}, {{43, 47, 40}}, {{40, 47, 44}}
		};

		portIndexBuffer.Establish(sizeof(Triangle) * kMaxProcessPortCount * 10, portTriangle);
	}

	if (portCount != 0)
	{
		InitRenderable(&portRenderable);
		portRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &portVertexBuffer, sizeof(PortVertex));
		portRenderable.SetVertexAttributeArray(kArrayPosition, 0, 2);
		portRenderable.SetVertexAttributeArray(kArrayColor, sizeof(Point2D), 4);
		portRenderable.SetVertexAttributeArray(kArrayTexcoord, sizeof(Point2D) + sizeof(ColorRGBA), 2);
		portVertexBuffer.Establish(sizeof(PortVertex) * kMaxProcessPortCount * 12);

		portRenderable.SetPrimitiveCount(portCount * 10);
		portRenderable.SetVertexBuffer(kVertexBufferIndexArray, &portIndexBuffer);

		portAttributeList.Append(&portTextureAttribute);
		portRenderable.SetMaterialAttributeList(&portAttributeList);

		float dy = PositiveFloor(kProcessBoxHeight / (float) (portCount + 1));
		float y = dy - (kPortBoxHeight * 0.5F - 2.0F) + (float) (portCount > 1);

		for (machine port = 0; port < portCount; port++)
		{
			TextWidget *text = new TextWidget(Vector2D(kPortBoxWidth, kPortBoxHeight), process->GetPortName(port), "font/Normal");
			text->SetTextAlignment(kTextAlignCenter);
			text->SetWidgetState(kWidgetDisabled);
			text->SetWidgetPosition(Point3D(kPortBoxWidth * -0.5F + kPortBoxOutdent, y, 0.0F));
			portText[port] = text;
			AppendSubnode(text);

			y += dy;
		}
	}
}

ProcessWidget::~ProcessWidget()
{
	portIndexBuffer.Release();
	outputVertexBuffer.Release();
	backgroundVertexBuffer.Release();
	processIndexBuffer.Release();
}

bool ProcessWidget::CalculateBoundingBox(Box2D *box) const
{
	box->min.Set(kPortBoxWidth * -0.5F + kPortBoxOutdent, 0.0F);
	box->max.Set(kProcessBoxWidth + kOutputDotRadius, kProcessBoxHeight);
	return (true);
}

void ProcessWidget::UpdateOutputColor(bool hilite)
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

void ProcessWidget::UpdateContent(void)
{
	SetText(processRegistration->GetProcessName());

	const char *comment = shaderProcess->GetProcessComment();
	if ((comment) && (comment[0] != 0))
	{
		if (commentText)
		{
			commentText->SetText(comment);
		}
		else
		{
			commentText = new TextWidget(Vector2D(kProcessBoxWidth - 6.0F, 13.0F), comment, "font/Normal");
			commentText->SetTextFlags(kTextClipped);
			commentText->SetTextFormatExclusionMask(0);
			commentText->SetWidgetState(kWidgetDisabled);
			commentText->SetWidgetPosition(Point3D(3.0F, kProcessBoxHeight - 13.0F, 0.0F));
			AppendNewSubnode(commentText);
		}
	}
	else
	{
		delete commentText;
		commentText = nullptr;
	}

	ProcessType base = shaderProcess->GetBaseProcessType();
	if (base == kProcessConstant)
	{
		ProcessType type = shaderProcess->GetProcessType();
		if (type == kProcessScalar)
		{
			const ScalarProcess *scalarProcess = static_cast<ScalarProcess *>(shaderProcess);
			String<15> string = Text::FloatToString(scalarProcess->GetScalarValue());

			if (valueText)
			{
				valueText->SetText(string);
			}
			else
			{
				valueText = new TextWidget(Vector2D(kProcessBoxWidth, 13.0F), string, "font/Normal");
				valueText->SetTextAlignment(kTextAlignCenter);
				valueText->SetWidgetState(kWidgetDisabled);
				valueText->SetWidgetPosition(Point3D(0.0F, 42.0F, 0.0F));
				AppendNewSubnode(valueText);
			}
		}
		else if (type == kProcessVector)
		{
			const VectorProcess *vectorProcess = static_cast<VectorProcess *>(shaderProcess);
			const Vector4D& value = vectorProcess->GetVectorValue();

			String<255> string("x = ");
			string += Text::FloatToString(value.x);
			string += "\ny = ";
			string += Text::FloatToString(value.y);
			string += "\nz = ";
			string += Text::FloatToString(value.z);
			string += "\nw = ";
			string += Text::FloatToString(value.w);

			if (valueText)
			{
				valueText->SetText(string);
			}
			else
			{
				valueText = new TextWidget(Vector2D(kProcessBoxWidth - 16.0F, 52.0F), string, "font/Normal");
				valueText->SetTextFlags(kTextWrapped);
				valueText->SetWidgetState(kWidgetDisabled);
				valueText->SetWidgetPosition(Point3D(kProcessBoxWidth * 0.5F - 44.0F, 24.0F, 0.0F));
				AppendNewSubnode(valueText);
			}
		}
		else if (type == kProcessColor)
		{
			const ColorProcess *colorProcess = static_cast<ColorProcess *>(shaderProcess);
			ColorRGBA color(colorProcess->GetColorValue().GetColorRGB(), 1.0F);

			if (colorBox)
			{
				colorBox->SetWidgetColor(color);
			}
			else
			{
				colorBox = new QuadWidget(Vector2D(64.0F, 64.0F), color);
				colorBox->SetWidgetState(kWidgetDisabled);
				colorBox->SetWidgetPosition(Point3D(kProcessBoxWidth * 0.5F - 32.0F, 17.0F, 0.0F));
				AppendNewSubnode(colorBox);
			}
		}
	}
	else if (base == kProcessTextureMap)
	{
		const TextureMapProcess *textureMapProcess = static_cast<TextureMapProcess *>(shaderProcess);
		const char *name = textureMapProcess->GetTextureName();

		float width = 64.0F;
		float height = 64.0F;

		Texture *texture = Texture::Get(name);
		if (texture)
		{
			float w = (float) texture->GetTextureWidth();
			float h = (float) texture->GetTextureHeight();

			if (w >= h)
			{
				height = width * h / w;
			}
			else
			{
				width = height * w / h;
			}
		}

		delete textureBox;
		textureBox = new ImageWidget(Vector2D(width, height), name);
		textureBox->SetImageBlendState(kBlendReplace);
		textureBox->SetWidgetState(kWidgetDisabled);
		AppendNewSubnode(textureBox);

		Point3D p(Floor((kProcessBoxWidth - width) * 0.5F), Floor(49.0F - height * 0.5F), 0.0F);
		textureBox->SetWidgetPosition(p);
		textureBox->Invalidate();

		if (texture)
		{
			texture->Release();
		}
	}
	else
	{
		if (!textureBox)
		{
			const char *name = nullptr;
			switch (shaderProcess->GetProcessType())
			{
				case kProcessAbsolute:
					name = "ShaderEditor/process/Abs";
					break;
				case kProcessAdd:
					name = "ShaderEditor/process/Add";
					break;
				case kProcessSubtract:
					name = "ShaderEditor/process/Sub";
					break;
				case kProcessAverage:
					name = "ShaderEditor/process/Avg";
					break;
				case kProcessInvert:
					name = "ShaderEditor/process/Inv";
					break;
				case kProcessExpand:
					name = "ShaderEditor/process/Vex";
					break;
				case kProcessMultiply:
					name = "ShaderEditor/process/Mul";
					break;
				case kProcessMultiplyAdd:
					name = "ShaderEditor/process/Mad";
					break;
				case kProcessLerp:
					name = "ShaderEditor/process/Lrp";
					break;
				case kProcessDivide:
					name = "ShaderEditor/process/Div";
					break;
				case kProcessDot3:
				case kProcessDot4:
					name = "ShaderEditor/process/Dot";
					break;
				case kProcessCross:
					name = "ShaderEditor/process/Xpd";
					break;
				case kProcessReciprocal:
					name = "ShaderEditor/process/Rcp";
					break;
				case kProcessReciprocalSquareRoot:
					name = "ShaderEditor/process/Rsq";
					break;
				case kProcessSquareRoot:
					name = "ShaderEditor/process/Sqrt";
					break;
				case kProcessMagnitude:
					name = "ShaderEditor/process/Mag";
					break;
				case kProcessNormalize:
					name = "ShaderEditor/process/Nrm";
					break;
				case kProcessFloor:
					name = "ShaderEditor/process/Flr";
					break;
				case kProcessRound:
					name = "ShaderEditor/process/Rnd";
					break;
				case kProcessFraction:
					name = "ShaderEditor/process/Frc";
					break;
				case kProcessSaturate:
					name = "ShaderEditor/process/Sat";
					break;
				case kProcessMinimum:
					name = "ShaderEditor/process/Min";
					break;
				case kProcessMaximum:
					name = "ShaderEditor/process/Max";
					break;
				case kProcessSetLessThan:
					name = "ShaderEditor/process/Slt";
					break;
				case kProcessSetGreaterThan:
					name = "ShaderEditor/process/Sgt";
					break;
				case kProcessSetLessEqual:
					name = "ShaderEditor/process/Sle";
					break;
				case kProcessSetGreaterEqual:
					name = "ShaderEditor/process/Sge";
					break;
				case kProcessSetEqual:
					name = "ShaderEditor/process/Seq";
					break;
				case kProcessSetNotEqual:
					name = "ShaderEditor/process/Sne";
					break;
				case kProcessSine:
					name = "ShaderEditor/process/Sin";
					break;
				case kProcessCosine:
					name = "ShaderEditor/process/Cos";
					break;
				case kProcessExp2:
					name = "ShaderEditor/process/Exp";
					break;
				case kProcessLog2:
					name = "ShaderEditor/process/Log";
					break;
				case kProcessPower:
					name = "ShaderEditor/process/Pow";
					break;
				case kProcessDiffuse:
					name = "ShaderEditor/process/Diffuse";
					break;
				case kProcessSpecular:
					name = "ShaderEditor/process/Specular";
					break;
				case kProcessReflectVector:
					name = "ShaderEditor/process/Reflect";
					break;
				case kProcessLinearRamp:
					name = "ShaderEditor/process/Ramp";
					break;
				case kProcessSmoothParameter:
					name = "ShaderEditor/process/Smooth";
					break;
				case kProcessSteepParameter:
					name = "ShaderEditor/process/Steep";
					break;
			}

			if (name)
			{
				Texture *texture = Texture::Get(name);
				if (texture)
				{
					float width = (float) texture->GetTextureWidth();
					float height = (float) texture->GetTextureHeight();

					textureBox = new ImageWidget(Vector2D(width, height), name);
					textureBox->SetWidgetState(kWidgetDisabled);
					textureBox->SetWidgetPosition(Point3D(Floor((kProcessBoxWidth - width) * 0.5F), Floor((kProcessBoxHeight - height) * 0.5F), 0.0F));
					AppendNewSubnode(textureBox);

					texture->Release();
				}
			}
		}
	}
}

void ProcessWidget::Select(unsigned_int32 state)
{
	processWidgetState |= kProcessWidgetSelected | state;
	processRenderable.SetPrimitiveCount(16);
}

void ProcessWidget::Unselect(void)
{
	processWidgetState &= ~(kProcessWidgetSelected | kProcessWidgetTempSelected);
	processRenderable.SetPrimitiveCount(8);
}

WidgetPart ProcessWidget::TestPosition(const Point3D& position) const
{
	ProcessType type = shaderProcess->GetBaseProcessType();
	if ((type != kProcessOutput) && (type != kProcessTerminal))
	{
		float x = position.x - kProcessBoxWidth;
		float y = position.y - kProcessBoxHeight * 0.5F;
		if (x * x + y * y < kOutputDotRadius * kOutputDotRadius)
		{
			return (kWidgetPartOutput);
		}
	}

	int32 portCount = shaderProcess->GetPortCount();
	if ((portCount != 0) && (position.x < kPortBoxWidth * 0.5F + kPortBoxOutdent))
	{
		float dy = PositiveFloor(kProcessBoxHeight / (float) (portCount + 1));
		float y = dy - kPortBoxHeight * 0.5F + (float) (portCount > 1);

		for (machine a = 0; a < portCount; a++)
		{
			if ((position.y >= y) && (position.y <= y + kPortBoxHeight))
			{
				return (kWidgetPartPort + a);
			}

			y += dy;
		}
	}

	return (((position.x >= 0.0F) && (position.x < kProcessBoxWidth)) ? kWidgetPartInterior : kWidgetPartNone);
}

void ProcessWidget::Build(void)
{
	volatile ProcessVertex *restrict processVertex = processVertexBuffer.BeginUpdate<ProcessVertex>();

	float thickness = (viewportScale < 4.0F) ? viewportScale * 2.0F : viewportScale;

	processVertex[0].position.Set(0.0F, 0.0F);
	processVertex[1].position.Set(0.0F, kProcessBoxHeight);
	processVertex[2].position.Set(kProcessBoxWidth, kProcessBoxHeight);
	processVertex[3].position.Set(kProcessBoxWidth, 0.0F);

	processVertex[4].position.Set(-thickness, -thickness);
	processVertex[5].position.Set(-thickness, kProcessBoxHeight + thickness);
	processVertex[6].position.Set(kProcessBoxWidth + thickness, kProcessBoxHeight + thickness);
	processVertex[7].position.Set(kProcessBoxWidth + thickness, -thickness);

	processVertex[8].position.Set(-thickness, -thickness);
	processVertex[9].position.Set(-thickness, kProcessBoxHeight + thickness);
	processVertex[10].position.Set(kProcessBoxWidth + thickness, kProcessBoxHeight + thickness);
	processVertex[11].position.Set(kProcessBoxWidth + thickness, -thickness);

	thickness = Fmax(kProcessBoxThickness, viewportScale * 4.0F);

	processVertex[12].position.Set(-thickness, -thickness);
	processVertex[13].position.Set(-thickness, kProcessBoxHeight + thickness);
	processVertex[14].position.Set(kProcessBoxWidth + thickness, kProcessBoxHeight + thickness);
	processVertex[15].position.Set(kProcessBoxWidth + thickness, -thickness);

	for (machine a = 0; a < 8; a++)
	{
		processVertex[a].color.Set(0.0F, 0.0F, 0.0F, 1.0F);
	}

	const ColorRGB& hiliteColor = TheInterfaceMgr->GetInterfaceColor(kInterfaceColorHilite).GetColorRGB();
	for (machine a = 8; a < 16; a++)
	{
		processVertex[a].color.Set(hiliteColor, 0.625F);
	}

	processVertexBuffer.EndUpdate();

	int32 portCount = shaderProcess->GetPortCount();
	if (portCount != 0)
	{
		volatile PortVertex *restrict portVertex = portVertexBuffer.BeginUpdate<PortVertex>();

		float dy = PositiveFloor(kProcessBoxHeight / (float) (portCount + 1));
		float y = dy - kPortBoxHeight * 0.5F + (float) (portCount > 1);

		for (machine a = 0; a < portCount; a++)
		{
			portVertex[0].position.Set(kPortBoxWidth * -0.5F + kPortBoxOutdent, y);
			portVertex[1].position.Set(kPortBoxWidth * -0.5F + kPortBoxOutdent, y + kPortBoxHeight);
			portVertex[2].position.Set(kPortBoxWidth * 0.5F + kPortBoxOutdent, y + kPortBoxHeight);
			portVertex[3].position.Set(kPortBoxWidth * 0.5F + kPortBoxOutdent, y);

			portVertex[4].position.Set(kPortBoxWidth * -0.5F + kPortBoxOutdent, y);
			portVertex[5].position.Set(kPortBoxWidth * -0.5F + kPortBoxOutdent, y + kPortBoxHeight);
			portVertex[6].position.Set(kPortBoxWidth * 0.5F + kPortBoxOutdent, y + kPortBoxHeight);
			portVertex[7].position.Set(kPortBoxWidth * 0.5F + kPortBoxOutdent, y);

			portVertex[8].position.Set(kPortBoxWidth * -0.5F + kPortBoxOutdent - viewportScale, y - viewportScale);
			portVertex[9].position.Set(kPortBoxWidth * -0.5F + kPortBoxOutdent - viewportScale, y + kPortBoxHeight + viewportScale);
			portVertex[10].position.Set(kPortBoxWidth * 0.5F + kPortBoxOutdent + viewportScale, y + kPortBoxHeight + viewportScale);
			portVertex[11].position.Set(kPortBoxWidth * 0.5F + kPortBoxOutdent + viewportScale, y - viewportScale);

			if (shaderProcess->GetPortFlags(a) & kProcessPortOptional)
			{
				float w = kPortBoxWidth * 0.125F;
				float h = kPortBoxHeight * 0.125F;
				float s = viewportScale * 0.125F;

				portVertex[0].texcoord.Set(0.0F, 0.0F);
				portVertex[1].texcoord.Set(0.0F, h);
				portVertex[2].texcoord.Set(w, h);
				portVertex[3].texcoord.Set(w, 0.0F);

				portVertex[4].texcoord.Set(0.0F, 0.0F);
				portVertex[5].texcoord.Set(0.0F, h);
				portVertex[6].texcoord.Set(w, h);
				portVertex[7].texcoord.Set(w, 0.0F);

				portVertex[8].texcoord.Set(-s, -s);
				portVertex[9].texcoord.Set(-s, h + s);
				portVertex[10].texcoord.Set(w + s, h + s);
				portVertex[11].texcoord.Set(w + s, -s);
			}
			else
			{
				for (machine b = 0; b < 12; b++)
				{
					portVertex[b].texcoord.Set(0.125F, 0.125F);
				}
			}

			if (a == hilitePortIndex)
			{
				for (machine b = 0; b < 4; b++)
				{
					portVertex[b].color.Set(hiliteColor, 1.0F);
				}
			}
			else
			{
				for (machine b = 0; b < 4; b++)
				{
					portVertex[b].color.Set(0.9325F, 0.9325F, 0.9325F, 1.0F);
				}
			}

			for (machine b = 4; b < 12; b++)
			{
				portVertex[b].color.Set(0.0F, 0.0F, 0.0F, 1.0F);
			}

			portVertex += 12;
			y += dy;
		}

		portVertexBuffer.EndUpdate();
	}

	TextWidget::Build();
}

void ProcessWidget::Render(List<Renderable> *renderList)
{
	renderList->Append(&backgroundRenderable);
	renderList->Append(&processRenderable);

	if (outputRenderable.GetVertexCount() != 0)
	{
		renderList->Append(&outputRenderable);
	}

	if (portRenderable.GetVertexCount() != 0)
	{
		renderList->Append(&portRenderable);
	}

	TextWidget::Render(renderList);
}


RouteWidget::RouteWidget(ShaderEditor *editor, Route *route) :
		RenderableWidget(kWidgetRoute, kRenderTriangleStrip),
		routeVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		routeColorAttribute(ColorRGBA(0.0F, 0.0F, 0.0F, 1.0F)),
		routeTextureAttribute(&routeTextureHeader, routeTextureImage),
		selectionVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		selectionColorAttribute(kAttributeMutable),
		selectionTextureAttribute(&routeTextureHeader, routeTextureImage),
		selectionRenderable(kRenderTriangleStrip),
		swizzleVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		swizzleRenderable(kRenderIndexedTriangles)
{
	shaderEditor = editor;
	shaderRoute = route;

	routeWidgetState = 0;
	routeHiliteCount = 0;

	viewportScale = 1.0F;
	swizzleText = nullptr;

	SetAmbientBlendState(kBlendPremultInterp);
	SetShaderFlags(kShaderAmbientEffect | kShaderVertexPolyboard | kShaderLinearPolyboard | kShaderOrthoPolyboard);

	routeAttributeList.Append(&routeColorAttribute);
	routeAttributeList.Append(&routeTextureAttribute);
	SetMaterialAttributeList(&routeAttributeList);

	SetVertexCount(70);
	SetVertexBuffer(kVertexBufferAttributeArray, &routeVertexBuffer, sizeof(RouteVertex));
	SetVertexAttributeArray(kArrayPosition, 0, 2);
	SetVertexAttributeArray(kArrayTangent, sizeof(Point2D), 4);
	SetVertexAttributeArray(kArrayTexcoord, sizeof(Point2D) + sizeof(Vector4D), 2);
	routeVertexBuffer.Establish(sizeof(RouteVertex) * 70);

	selectionRenderable.SetAmbientBlendState(kBlendInterpolate);
	selectionRenderable.SetShaderFlags(kShaderAmbientEffect | kShaderVertexPolyboard | kShaderLinearPolyboard | kShaderOrthoPolyboard);

	selectionRenderable.SetVertexCount(70);
	selectionRenderable.SetVertexBuffer(kVertexBufferAttributeArray0, &routeVertexBuffer, sizeof(RouteVertex));
	selectionRenderable.SetVertexBuffer(kVertexBufferAttributeArray1, &selectionVertexBuffer, sizeof(SelectionVertex));
	selectionRenderable.SetVertexBufferArrayFlags((1 << kArrayTangent) | (1 << kArrayTexcoord));
	selectionRenderable.SetVertexAttributeArray(kArrayPosition, 0, 2);
	selectionRenderable.SetVertexAttributeArray(kArrayTangent, 0, 4);
	selectionRenderable.SetVertexAttributeArray(kArrayTexcoord, sizeof(Vector4D), 2);
	selectionVertexBuffer.Establish(sizeof(SelectionVertex) * 70);

	selectionAttributeList.Append(&selectionColorAttribute);
	selectionAttributeList.Append(&selectionTextureAttribute);
	selectionRenderable.SetMaterialAttributeList(&selectionAttributeList);

	swizzleRenderable.SetVertexCount(12);
	swizzleRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &swizzleVertexBuffer, sizeof(SwizzleVertex));
	swizzleRenderable.SetVertexAttributeArray(kArrayPosition, 0, 2);
	swizzleRenderable.SetVertexAttributeArray(kArrayColor, sizeof(Point2D), 3);
	swizzleVertexBuffer.Establish(sizeof(SwizzleVertex) * 12);

	if (swizzleIndexBuffer.Retain() == 1)
	{
		static const Triangle swizzleTriangle[10] =
		{
			{{ 0,  1,  2}}, {{ 0,  2,  3}}, {{ 4,  8,  5}}, {{ 5,  8,  9}}, {{ 5,  9,  6}},
			{{ 6,  9, 10}}, {{ 6, 10,  7}}, {{ 7, 10, 11}}, {{ 7, 11,  4}}, {{ 4, 11,  8}}
		};

		swizzleIndexBuffer.Establish(sizeof(Triangle) * 10, swizzleTriangle);
	}

	swizzleRenderable.SetPrimitiveCount(10);
	swizzleRenderable.SetVertexBuffer(kVertexBufferIndexArray, &swizzleIndexBuffer);

	UpdateContent();
}

RouteWidget::~RouteWidget()
{
	swizzleIndexBuffer.Release();
}

bool RouteWidget::CalculateBoundingBox(Box2D *box) const
{
	float xmin = routeVertex[0].position.x;
	float xmax = routeVertex[0].position.x;
	float ymin = routeVertex[0].position.y;
	float ymax = routeVertex[0].position.y;

	for (machine a = 2; a < 70; a += 2)
	{
		const Point2D& p = routeVertex[a].position;
		xmin = Fmin(xmin, p.x);
		xmax = Fmax(xmax, p.x);
		ymin = Fmin(ymin, p.y);
		ymax = Fmax(ymax, p.y);
	}

	box->min.Set(xmin - 4.0F, ymin - 4.0F);
	box->max.Set(xmax + 4.0F, ymax + 4.0F);
	return (true);
}

void RouteWidget::UpdateContent(void)
{
	bool negation = shaderRoute->GetRouteNegation();
	unsigned_int32 swizzle = shaderRoute->GetRouteSwizzle();

	if ((negation) || (swizzle != 'xyzw'))
	{
		char	string[8];

		char *s = string;
		if (negation)
		{
			s[0] = '\xE2';		// U+2212
			s[1] = '\x88';
			s[2] = '\x92';
			s += 3;
		}

		if (swizzle != 'xyzw')
		{
			if (swizzle == 'xxxx')
			{
				*s++ = 'x';
			}
			else if (swizzle == 'yyyy')
			{
				*s++ = 'y';
			}
			else if (swizzle == 'zzzz')
			{
				*s++ = 'z';
			}
			else if (swizzle == 'wwww')
			{
				*s++ = 'w';
			}
			else if (swizzle == 'rrrr')
			{
				*s++ = 'r';
			}
			else if (swizzle == 'gggg')
			{
				*s++ = 'g';
			}
			else if (swizzle == 'bbbb')
			{
				*s++ = 'b';
			}
			else if (swizzle == 'aaaa')
			{
				*s++ = 'a';
			}
			else
			{
				s[0] = (char) (swizzle >> 24);
				s[1] = (char) (swizzle >> 16);
				s[2] = (char) (swizzle >> 8);
				s[3] = (char) swizzle;
				s += 4;
			}
		}

		*s = 0;

		if (swizzleText)
		{
			swizzleText->SetText(string);
		}
		else
		{
			swizzleText = new TextWidget(Vector2D(kSwizzleBoxWidth, kSwizzleBoxHeight), string, "font/Normal");
			swizzleText->SetTextAlignment(kTextAlignCenter);
			swizzleText->SetWidgetState(kWidgetDisabled);
			AppendNewSubnode(swizzleText);
		}

		SetBuildFlag();
	}
	else
	{
		delete swizzleText;
		swizzleText = nullptr;
	}
}

void RouteWidget::Select(void)
{
	routeWidgetState |= kRouteWidgetSelected;
	selectionColorAttribute.SetDiffuseColor(TheInterfaceMgr->GetInterfaceColor(kInterfaceColorHilite));
}

void RouteWidget::Unselect(void)
{
	unsigned_int32 state = routeWidgetState;
	routeWidgetState = state & ~kRouteWidgetSelected;

	if (state & kRouteWidgetHilited)
	{
		selectionColorAttribute.SetDiffuseColor(ColorRGBA(0.75F, 0.75F, 0.75F, 1.0F));
	}
}

void RouteWidget::Hilite(void)
{
	unsigned_int32 state = routeWidgetState;
	routeWidgetState = state | kRouteWidgetHilited;
	routeHiliteCount++;

	if (!(state & kRouteWidgetSelected))
	{
		selectionColorAttribute.SetDiffuseColor(ColorRGBA(0.75F, 0.75F, 0.75F, 1.0F));
	}
}

void RouteWidget::Unhilite(void)
{
	if (--routeHiliteCount == 0)
	{
		routeWidgetState &= ~kRouteWidgetHilited;
	}
}

void RouteWidget::HandleDelete(void)
{
	routeWidgetState &= ~(kRouteWidgetSelected | kRouteWidgetHilited);
	routeHiliteCount = 0;
}

WidgetPart RouteWidget::TestPosition(const Point3D& position) const
{
	for (machine a = 0; a <= 66; a += 2)
	{
		if (a != 64)
		{
			Point3D q1 = routeVertex[a].position;
			Point3D q2 = routeVertex[a + 2].position;
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

void RouteWidget::Build(void)
{
	Invalidate();

	bool highDetail = ((shaderRoute->GetRouteFlags() & kRouteHighDetail) != 0);

	Point2D p1 = shaderRoute->GetStartElement()->GetProcessPosition() + Vector2D(kProcessBoxWidth + 2.0F, kProcessBoxHeight * 0.5F);
	Point2D texpoint = p1;

	const Process *finish = shaderRoute->GetFinishElement();
	Point2D p5 = finish->GetProcessPosition();

	int32 portCount = finish->GetPortCount();
	int32 portIndex = shaderRoute->GetRoutePort();
	p5 += Vector2D(kPortBoxWidth * -0.5F + kPortBoxOutdent - 1.0F, (float) (portIndex + 1) * PositiveFloor(kProcessBoxHeight / (float) (portCount + 1)) + (float) (portCount > 1));

	float dx = (p5.x - p1.x - 8.0F) * K::one_over_3;
	float f = (p1.y < p5.y) ? 1.0F : -1.0F;

	if (dx >= 8.0F)
	{
		Point2D p2(p1.x + dx, p1.y);
		Point2D p3(p5.x - dx, p5.y);

		Vector2D tangent = Normalize(p5 - p3);
		Point2D p4 = p5 - tangent * 9.0F;

		routeVertex[68].position = p5;
		routeVertex[68].tangent.Set(tangent.x, tangent.y, 0.0F, -3.0F);
		routeVertex[68].texcoord.Set(0.5, 0.0625F);

		routeVertex[69].position = p5;
		routeVertex[69].tangent.Set(tangent.x, tangent.y, 0.0F, 4.0F);
		routeVertex[69].texcoord.Set(0.9375F, 0.0625F);

		BezierPathComponent path(p1, p2, p3, p4);

		float u = 0.0F;
		float texcoord = 0.0F;

		for (machine a = 0; a <= 64; a += 2)
		{
			Point3D p = path.BezierPathComponent::GetPosition(u);
			Vector3D t = path.BezierPathComponent::GetTangent(u);
			t.Normalize();

			routeVertex[a].position = p.GetPoint2D();
			routeVertex[a + 1].position = p.GetPoint2D();
			routeVertex[a].tangent.Set(t, -3.0F);
			routeVertex[a + 1].tangent.Set(t, 4.0F);

			if (highDetail)
			{
				texcoord += Magnitude(p.GetPoint2D() - texpoint) * 0.0625F;
				texpoint = p.GetPoint2D();
			}

			routeVertex[a].texcoord.Set(0.0F, texcoord);
			routeVertex[a + 1].texcoord.Set(0.4375F, texcoord);

			u += 0.03125F;
		}
	}
	else
	{
		Point2D		p2, p3, pa, pb;

		if (dx < Fnabs(p1.y - p5.y) * 0.5F)
		{
			f *= kProcessBoxHeight;

			p2.Set(p1.x + kProcessBoxWidth * 0.5F, p1.y - f);
			p3.Set(p5.x - kProcessBoxWidth * 0.5F, p5.y - f);

			pa.Set(p1.x, p2.y - f);
			pb.Set(p5.x, p3.y - f);
		}
		else
		{
			p2.Set(p1.x + kProcessBoxWidth, p1.y);
			p3.Set(p5.x - kProcessBoxWidth, p5.y);

			pa.Set(p2.x, p2.y);
			pb.Set(p3.x, p3.y);
		}

		Point2D pc = (pa + pb) * 0.5F;

		Vector2D tangent = Normalize(p5 - p3);
		Point2D p4 = p5 - tangent * 9.0F;

		routeVertex[68].position = p5;
		routeVertex[68].tangent.Set(tangent.x, tangent.y, 0.0F, -3.0F);
		routeVertex[68].texcoord.Set(0.5, 0.0625F);

		routeVertex[69].position = p5;
		routeVertex[69].tangent.Set(tangent.x, tangent.y, 0.0F, 4.0F);
		routeVertex[69].texcoord.Set(0.9375F, 0.0625F);

		BezierPathComponent path1(p1, p2, pa, pc);
		BezierPathComponent path2(pc, pb, p3, p4);

		float u = 0.0F;
		float texcoord = 0.0F;

		for (machine a = 0; a < 32; a += 2)
		{
			Point3D p = path1.BezierPathComponent::GetPosition(u);
			Vector3D t = path1.BezierPathComponent::GetTangent(u);
			t.Normalize();

			routeVertex[a].position = p.GetPoint2D();
			routeVertex[a + 1].position = p.GetPoint2D();
			routeVertex[a].tangent.Set(t, -3.0F);
			routeVertex[a + 1].tangent.Set(t, 4.0F);

			if (highDetail)
			{
				texcoord += Magnitude(p.GetPoint2D() - texpoint) * 0.0625F;
				texpoint = p.GetPoint2D();
			}

			routeVertex[a].texcoord.Set(0.0F, texcoord);
			routeVertex[a + 1].texcoord.Set(0.4375F, texcoord);

			u += 0.0625F;
		}

		u = 0.0F;
		for (machine a = 32; a <= 64; a += 2)
		{
			Point3D p = path2.BezierPathComponent::GetPosition(u);
			Vector3D t = path2.BezierPathComponent::GetTangent(u);
			t.Normalize();

			routeVertex[a].position = p.GetPoint2D();
			routeVertex[a + 1].position = p.GetPoint2D();
			routeVertex[a].tangent.Set(t, -3.0F);
			routeVertex[a + 1].tangent.Set(t, 4.0F);

			if (highDetail)
			{
				texcoord += Magnitude(p.GetPoint2D() - texpoint) * 0.0625F;
				texpoint = p.GetPoint2D();
			}

			routeVertex[a].texcoord.Set(0.0F, texcoord);
			routeVertex[a + 1].texcoord.Set(0.4375F, texcoord);

			u += 0.0625F;
		}
	}

	routeVertex[66].position = routeVertex[64].position;
	routeVertex[66].tangent = routeVertex[64].tangent;
	routeVertex[66].texcoord.Set(0.5, 0.9375F);

	routeVertex[67].position = routeVertex[65].position;
	routeVertex[67].tangent = routeVertex[65].tangent;
	routeVertex[67].texcoord.Set(0.9375F, 0.9375F);

	routeVertexBuffer.UpdateBuffer(0, sizeof(RouteVertex) * 70, routeVertex);

	volatile SelectionVertex *restrict selectionVertex = selectionVertexBuffer.BeginUpdate<SelectionVertex>();

	for (machine a = 0; a < 70; a += 2)
	{
		selectionVertex[a].tangent.Set(routeVertex[a].tangent.GetVector3D(), -15.0F);
		selectionVertex[a].texcoord.Set(0.0F, 0.0F);
		selectionVertex[a + 1].tangent.Set(routeVertex[a + 1].tangent.GetVector3D(), 16.0F);
		selectionVertex[a + 1].texcoord.Set(0.4375F, 0.0F);
	}

	selectionVertexBuffer.EndUpdate();

	if (swizzleText)
	{
		volatile SwizzleVertex *restrict swizzleVertex = swizzleVertexBuffer.BeginUpdate<SwizzleVertex>();

		float thickness = viewportScale;
		float x = Floor(routeVertex[32].position.x) - kSwizzleBoxWidth * 0.5F;
		float y = Floor(routeVertex[32].position.y) - kSwizzleBoxHeight * 0.5F;

		swizzleVertex[0].position.Set(x, y);
		swizzleVertex[1].position.Set(x, y + kSwizzleBoxHeight);
		swizzleVertex[2].position.Set(x + kSwizzleBoxWidth, y + kSwizzleBoxHeight);
		swizzleVertex[3].position.Set(x + kSwizzleBoxWidth, y);

		swizzleVertex[4].position.Set(x, y);
		swizzleVertex[5].position.Set(x, y + kSwizzleBoxHeight);
		swizzleVertex[6].position.Set(x + kSwizzleBoxWidth, y + kSwizzleBoxHeight);
		swizzleVertex[7].position.Set(x + kSwizzleBoxWidth, y);

		swizzleVertex[8].position.Set(x - thickness, y - thickness);
		swizzleVertex[9].position.Set(x - thickness, y + kSwizzleBoxHeight + thickness);
		swizzleVertex[10].position.Set(x + kSwizzleBoxWidth + thickness, y + kSwizzleBoxHeight + thickness);
		swizzleVertex[11].position.Set(x + kSwizzleBoxWidth + thickness, y - thickness);

		for (machine a = 0; a < 4; a++)
		{
			swizzleVertex[a].color.Set(0.875F, 0.875F, 0.875F);
		}

		for (machine a = 4; a < 12; a++)
		{
			swizzleVertex[a].color.Set(0.0F, 0.0F, 0.0F);
		}

		swizzleVertexBuffer.EndUpdate();

		swizzleText->SetWidgetPosition(Point3D(x, y + (kSwizzleBoxHeight * 0.5F - 6.0F), 0.0F));
		swizzleText->Update();
	}
}

void RouteWidget::Render(List<Renderable> *renderList)
{
	if (routeWidgetState & (kRouteWidgetSelected | kRouteWidgetHilited))
	{
		renderList->Append(&selectionRenderable);
	}

	RenderableWidget::Render(renderList);

	if (swizzleText)
	{
		renderList->Append(&swizzleRenderable);
	}
}


ShaderSectionWidget::ShaderSectionWidget(ShaderEditor *editor, SectionProcess *process) :
		TextWidget(kWidgetShaderSection, process->GetProcessComment(), "font/Gui"),
		sectionVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		sectionRenderable(kRenderIndexedTriangles)
{
	shaderEditor = editor;
	sectionProcess = process;

	sectionWidgetState = 0;
	viewportScale = 1.0F;

	float width = Fmax(process->GetSectionWidth(), kMinSectionSize);
	float height = Fmax(process->GetSectionHeight(), kMinSectionSize);
	SetWidgetSize(Vector2D(width, height));

	SetTextFlags(kTextClipped);
	SetTextFormatExclusionMask(0);
	SetTextAlignment(kTextAlignCenter);
	SetTextRenderOffset(Vector3D(1.0F, 2.0F, 0.0F));

	InitRenderable(&sectionRenderable);
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

ShaderSectionWidget::~ShaderSectionWidget()
{
	sectionIndexBuffer.Release();
}

void ShaderSectionWidget::UpdateContent(void)
{
	const char *comment = sectionProcess->GetProcessComment();
	if ((comment) && (comment[0] != 0))
	{
		SetText(comment);
	}
	else
	{
		SetText(nullptr);
	}
}

void ShaderSectionWidget::Select(void)
{
	sectionWidgetState |= kShaderSectionWidgetSelected;
	sectionRenderable.SetPrimitiveCount(21);
}

void ShaderSectionWidget::Unselect(void)
{
	sectionWidgetState &= ~kShaderSectionWidgetSelected;
	sectionRenderable.SetPrimitiveCount(13);
}

WidgetPart ShaderSectionWidget::TestPosition(const Point3D& position) const
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

void ShaderSectionWidget::Build(void)
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

	float thickness = Fmax(kProcessBoxThickness, viewportScale * 4.0F);

	sectionVertex[23].position.Set(-thickness, -thickness);
	sectionVertex[24].position.Set(-thickness, height + thickness);
	sectionVertex[25].position.Set(width + thickness, height + thickness);
	sectionVertex[26].position.Set(width + thickness, -thickness);

	for (machine a = 0; a < 4; a++)
	{
		sectionVertex[a].color.Set(0.75F, 0.75F, 0.75F, 1.0F);
	}

	const ColorRGBA& sectionColor = sectionProcess->GetSectionColor();
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

void ShaderSectionWidget::Render(List<Renderable> *renderList)
{
	renderList->Append(&sectionRenderable);
	TextWidget::Render(renderList);
}


PreviewWidget::PreviewWidget(const Vector2D& size, MaterialObject *materialObject) : FrustumViewportWidget(size, 2.0F)
{
	previewMaterial = materialObject;
	previewWorld = nullptr;
}

PreviewWidget::~PreviewWidget()
{
	delete previewWorld;
}

void PreviewWidget::Preprocess(void)
{
	FrustumViewportWidget::Preprocess();

	SetMouseEventProc(&ViewportHandleMouseEvent, this);
	SetRenderProc(&ViewportRender, this);

	Zone *previewZone = new InfiniteZone;
	ZoneObject *zoneObject = new InfiniteZoneObject;
	previewZone->SetObject(zoneObject);
	zoneObject->Release();

	zoneObject->SetAmbientLight(ColorRGBA(0.375F, 0.375F, 0.375F, 1.0F));
	zoneObject->SetEnvironmentMap("C4/environment");

	previewWorld = new World(previewZone, kWorldViewport | kWorldClearColor | kWorldMotionBlurInhibit | kWorldListenerInhibit);
	previewWorld->SetRenderSize((int32) GetWidgetSize().x, (int32) GetWidgetSize().y);
	previewWorld->Preprocess();

	FrustumCamera *camera = GetViewportCamera();
	camera->GetObject()->SetClearColor(K::transparent);
	previewWorld->SetCamera(camera);

	previewLight = new PointLight(ColorRGB(0.75F, 0.75F, 0.75F), 100.0F);
	previewLight->GetObject()->SetLightFlags(kLightShadowInhibit);
	previewLight->SetNodePosition(Point3D(4.0F, 4.0F, 7.0F));
	previewWorld->AddNewNode(previewLight);

	previewGeometry = nullptr;
}

void PreviewWidget::SetCameraAngles(float azm, float alt)
{
	Vector2D t = CosSin(azm);
	Vector2D p = CosSin(alt) * cameraDistance;
	SetCameraTransform(azm, alt, Point3D(-t.x * p.x, -t.y * p.x, -p.y));
}

void PreviewWidget::SetMaterial(MaterialObject *materialObject)
{
	if (previewMaterial != materialObject)
	{
		previewMaterial = materialObject;

		if (previewGeometry)
		{
			previewGeometry->SetMaterialObject(0, previewMaterial);
			previewGeometry->InvalidateShaderData();
		}
	}
}

void PreviewWidget::SetPreviewGeometry(PrimitiveType type)
{
	delete previewGeometry;

	switch (type)
	{
		case kPrimitivePlate:
		{
			previewGeometry = new PlateGeometry(Vector2D(2.0F, 2.0F));
			previewGeometry->SetNodePosition(Point3D(-1.0F, -1.0F, 0.0F));

			SurfaceData *surfaceData = previewGeometry->GetObject()->GetSurfaceData();
			surfaceData[0].textureAlignData[0].alignMode = kTextureAlignNatural;
			surfaceData[0].textureAlignData[1].alignMode = kTextureAlignNatural;

			cameraDistance = 2.0F;
			SetCameraAngles(K::tau_over_4, -K::tau_over_4);
			break;
		}

		case kPrimitiveBox:
		{
			previewGeometry = new BoxGeometry(Vector3D(1.5F, 1.5F, 1.5F));
			previewGeometry->SetNodePosition(Point3D(-0.75F, -0.75F, -0.75F));

			SurfaceData *surfaceData = previewGeometry->GetObject()->GetSurfaceData();
			for (machine a = 0; a < 6; a++)
			{
				surfaceData[a].textureAlignData[0].alignMode = kTextureAlignNatural;
				surfaceData[a].textureAlignData[1].alignMode = kTextureAlignNatural;
			}

			cameraDistance = 2.5F;
			SetCameraAngles(K::three_tau_over_8, -0.589F);
			break;
		}

		case kPrimitiveSphere:
		case kPrimitiveDome:
		{
			previewGeometry = new SphereGeometry(Vector3D(1.0F, 1.0F, 1.0F));

			SurfaceData *surfaceData = previewGeometry->GetObject()->GetSurfaceData();
			surfaceData[0].textureAlignData[0].alignMode = kTextureAlignNatural;
			surfaceData[0].textureAlignData[1].alignMode = kTextureAlignNatural;
			surfaceData[0].textureAlignData[0].alignPlane.x = 4.0F;
			surfaceData[0].textureAlignData[1].alignPlane.y = 2.0F;

			cameraDistance = 2.5F;
			SetCameraAngles(K::three_tau_over_8, -K::tau_over_16);
			break;
		}

		case kPrimitiveCylinder:
		{
			previewGeometry = new CylinderGeometry(Vector2D(0.875F, 0.875F), 1.75F);
			previewGeometry->SetNodePosition(Point3D(0.0F, 0.0F, -0.875F));

			SurfaceData *surfaceData = previewGeometry->GetObject()->GetSurfaceData();
			surfaceData[0].textureAlignData[0].alignMode = kTextureAlignNatural;
			surfaceData[0].textureAlignData[1].alignMode = kTextureAlignNatural;
			surfaceData[0].textureAlignData[0].alignPlane.x = 3.0F;
			surfaceData[0].textureAlignData[1].alignPlane.y = 1.0F;

			surfaceData[1].textureAlignData[0].alignMode = kTextureAlignNatural;
			surfaceData[1].textureAlignData[1].alignMode = kTextureAlignNatural;
			surfaceData[2].textureAlignData[0].alignMode = kTextureAlignNatural;
			surfaceData[2].textureAlignData[1].alignMode = kTextureAlignNatural;

			cameraDistance = 2.5F;
			SetCameraAngles(K::three_tau_over_8, -0.589F);
			break;
		}

		case kPrimitiveTorus:
		{
			previewGeometry = new TorusGeometry(Vector2D(0.75F, 0.75F), 0.375F);

			SurfaceData *surfaceData = previewGeometry->GetObject()->GetSurfaceData();
			surfaceData[0].textureAlignData[0].alignMode = kTextureAlignNatural;
			surfaceData[0].textureAlignData[1].alignMode = kTextureAlignNatural;
			surfaceData[0].textureAlignData[0].alignPlane.x = 3.0F;
			surfaceData[0].textureAlignData[1].alignPlane.y = 1.0F;

			cameraDistance = 2.5F;
			SetCameraAngles(K::tau_over_4, -K::tau_over_4);
			break;
		}
	}

	previewGeometry->SetMaterialObject(0, previewMaterial);

	PrimitiveGeometryObject *object = previewGeometry->GetObject();
	object->SetCollisionExclusionMask(kCollisionExcludeAll);
	object->SetBuildLevelCount(1);

	previewGeometry->Update();
	object->Build(previewGeometry);
	previewWorld->AddNewNode(previewGeometry);
	previewGeometry->Update();
}

void PreviewWidget::UpdatePreview(void)
{
	PrimitiveGeometry *geometry = previewGeometry;
	if (geometry)
	{
		geometry->InvalidateShaderData();
		geometry->Preprocess();
	}
}

void PreviewWidget::ViewportHandleMouseEvent(const MouseEventData *eventData, ViewportWidget *viewport, void *cookie)
{
	PreviewWidget *previewWidget = static_cast<PreviewWidget *>(cookie);

	EventType eventType = eventData->eventType;
	if (eventType == kEventMouseDown)
	{
		previewWidget->trackFlag = true;
		previewWidget->previousPosition = eventData->mousePosition;
	}
	else if (eventType == kEventMouseMoved)
	{
		if (previewWidget->trackFlag)
		{
			float azm = previewWidget->GetCameraAzimuth() + (previewWidget->previousPosition.x - eventData->mousePosition.x) * 0.03125F;
			if (azm < -K::tau_over_2)
			{
				azm += K::tau;
			}
			else if (azm > K::tau_over_2)
			{
				azm -= K::tau;
			}

			float alt = previewWidget->GetCameraAltitude() + (previewWidget->previousPosition.y - eventData->mousePosition.y) * 0.03125F;
			alt = Clamp(alt, -K::tau_over_4, K::tau_over_4);

			previewWidget->previousPosition = eventData->mousePosition;
			previewWidget->SetCameraAngles(azm, alt);
		}
	}
	else
	{
		previewWidget->trackFlag = false;
	}
}

void PreviewWidget::ViewportRender(List<Renderable> *renderList, ViewportWidget *viewport, void *cookie)
{
	PreviewWidget *previewWidget = static_cast<PreviewWidget *>(cookie);

	if (previewWidget->previewGeometry)
	{
		unsigned_int32 mask = TheGraphicsMgr->GetTargetDisableMask();
		TheGraphicsMgr->SetTargetDisableMask(mask | ((1 << kRenderTargetReflection) | (1 << kRenderTargetRefraction)));

		World *world = previewWidget->previewWorld;
		world->Update();
		world->BeginRendering();
		world->Render();
		world->EndRendering();

		TheGraphicsMgr->SetTargetDisableMask(mask);
	}
}


ShaderPage::ShaderPage(ShaderEditor *editor, const char *panelName) : Page(panelName)
{
	shaderEditor = editor;
}

ShaderPage::~ShaderPage()
{
}


ShaderProcessesPage::ShaderProcessesPage(ShaderEditor *editor) :
		ShaderPage(editor, "ShaderEditor/Processes"),
		multipaneWidgetObserver(this, &ShaderProcessesPage::HandleMultipaneWidgetEvent),
		listWidgetObserver(this, &ShaderProcessesPage::HandleListWidgetEvent)
{
}

ShaderProcessesPage::~ShaderProcessesPage()
{
}

ShaderProcessesPage::ToolWidget::ToolWidget(const Vector2D& size, const ProcessRegistration *registration) : TextWidget(size, GetProcessName(registration), "font/Normal")
{
	processRegistration = registration;
}

ShaderProcessesPage::ToolWidget::~ToolWidget()
{
}

String<127> ShaderProcessesPage::ToolWidget::GetProcessName(const ProcessRegistration *registration)
{
	String<127> name(registration->GetProcessName());

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

void ShaderProcessesPage::Preprocess(void)
{
	static const char *const listIdentifier[kProcessPaneCount] =
	{
		"Basic", "Math", "Complex", "Interp"
	};

	ShaderPage::Preprocess();

	multipaneWidget = static_cast<MultipaneWidget *>(FindWidget("Pane"));
	multipaneWidget->SetObserver(&multipaneWidgetObserver);

	for (machine a = 0; a < kProcessPaneCount; a++)
	{
		listWidget[a] = static_cast<ListWidget *>(FindWidget(listIdentifier[a]));
		listWidget[a]->SetObserver(&listWidgetObserver);
	}

	Vector2D size = listWidget[0]->GetNaturalListItemSize();

	const ProcessRegistration *registration = Process::GetFirstRegistration();
	while (registration)
	{
		if (registration->GetProcessName()[0] != 0)
		{
			ProcessGroup group = registration->GetProcessGroup();
			if (group != 0)
			{
				ToolWidget *widget = new ToolWidget(size, registration);

				if (group == 'BASC')
				{
					listWidget[kProcessPaneBasic]->InsertSortedListItem(widget);
				}
				else if (group == 'MATH')
				{
					listWidget[kProcessPaneMath]->InsertSortedListItem(widget);
				}
				else if (group == 'COMP')
				{
					listWidget[kProcessPaneComplex]->InsertSortedListItem(widget);
				}
				else
				{
					listWidget[kProcessPaneInterpolants]->InsertSortedListItem(widget);
				}
			}
		}

		registration = registration->Next();
	}
}

void ShaderProcessesPage::HandleMultipaneWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		int32 selection = static_cast<MultipaneWidget *>(widget)->GetSelection();

		for (machine a = 0; a < kProcessPaneCount; a++)
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

		GetShaderEditor()->SelectDefaultTool();
	}
}

void ShaderProcessesPage::HandleListWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		ToolWidget *toolWidget = static_cast<ToolWidget *>(static_cast<ListWidget *>(widget)->GetFirstSelectedListItem());
		GetShaderEditor()->SelectProcessTool(toolWidget->processRegistration);
	}
}

void ShaderProcessesPage::UnselectProcessTool(void)
{
	for (machine a = 0; a < kProcessPaneCount; a++)
	{
		listWidget[a]->UnselectAllListItems();
	}
}


ShaderPreviewPage::ShaderPreviewPage(ShaderEditor *editor, PrimitiveType primitiveType) :
		ShaderPage(editor, "ShaderEditor/Preview"),
		menuWidgetObserver(this, &ShaderPreviewPage::HandleMenuWidgetEvent)
{
	initPrimitiveType = primitiveType;
	previewMaterial = new MaterialObject(*editor->GetMaterialObject());
}

ShaderPreviewPage::~ShaderPreviewPage()
{
	previewMaterial->Release();
}

void ShaderPreviewPage::Preprocess(void)
{
	ShaderPage::Preprocess();

	menuWidget = static_cast<PopupMenuWidget *>(FindWidget("Menu"));
	menuWidget->SetObserver(&menuWidgetObserver);

	previewWidget = new PreviewWidget(Vector2D(256.0F, 256.0F), previewMaterial);
	previewWidget->SetWidgetPosition(Point3D(4.0F, 4.0F, 0.0F));
	AppendNewSubnode(previewWidget);

	previewAttribute = new ShaderAttribute;
	previewMaterial->AddAttribute(previewAttribute);

	SetPreviewGeometry(initPrimitiveType);
}

void ShaderPreviewPage::UpdatePreviewMaterial(const ShaderGraph *shaderGraph, ShaderResult *result)
{
	ShaderGraph *graph = previewAttribute->GetShaderGraph();
	graph->Purge();

	ShaderAttribute::CloneShader(shaderGraph, graph);

	if (result)
	{
		static const ShaderType shaderType[2] =
		{
			kShaderUnified, kShaderPointLight
		};

		const PrimitiveGeometry *geometry = previewWidget->GetPreviewGeometry();

		for (machine a = 0; a < 2; a++)
		{
			result[a] = previewAttribute->TestShader(shaderType[a], ShaderKey(kShaderVariantLinearFog, 0, true), geometry, geometry->GetFirstRenderSegment());
		}
	}

	previewWidget->UpdatePreview();
}

void ShaderPreviewPage::SetPreviewGeometry(PrimitiveType type)
{
	int32 selection = 0;
	switch (type)
	{
		case kPrimitiveBox:

			selection = 1;
			break;

		case kPrimitiveSphere:
		case kPrimitiveDome:

			selection = 2;
			break;

		case kPrimitiveCylinder:

			selection = 3;
			break;

		case kPrimitiveTorus:

			selection = 4;
			break;
	}

	menuWidget->SetSelection(selection);
	previewWidget->SetPreviewGeometry(type);
}

void ShaderPreviewPage::HandleMenuWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		static const PrimitiveType primitiveType[5] =
		{
			kPrimitivePlate, kPrimitiveBox, kPrimitiveSphere, kPrimitiveCylinder, kPrimitiveTorus
		};

		previewWidget->SetPreviewGeometry(primitiveType[static_cast<PopupMenuWidget *>(widget)->GetSelection()]);
	}
}


ProcessInfoWindow::ProcessInfoWindow(ShaderEditor *editor) : Window("ShaderEditor/ProcessInfo")
{
	shaderEditor = editor;
	processWidget = editor->GetFirstSelectedProcess();
}

ProcessInfoWindow::~ProcessInfoWindow()
{
}

void ProcessInfoWindow::Preprocess(void)
{
	Window::Preprocess();

	okayButton = static_cast<PushButtonWidget *>(FindWidget("OK"));
	cancelButton = static_cast<PushButtonWidget *>(FindWidget("Cancel"));

	configurationWidget = static_cast<ConfigurationWidget *>(FindWidget("Config"));
	configurationWidget->BuildConfiguration(processWidget->GetShaderProcess());

	SetNextFocusWidget();
}

void ProcessInfoWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		if (widget == okayButton)
		{
			configurationWidget->CommitConfiguration(processWidget->GetShaderProcess());
			processWidget->UpdateContent();

			shaderEditor->SetModifiedFlag();
			Close();
		}
		else if (widget == cancelButton)
		{
			Close();
		}
	}
}


RouteInfoWindow::RouteInfoWindow(ShaderEditor *editor) : Window("ShaderEditor/RouteInfo")
{
	shaderEditor = editor;
	routeWidget = editor->GetFirstSelectedRoute();
}

RouteInfoWindow::~RouteInfoWindow()
{
}

void RouteInfoWindow::Preprocess(void)
{
	Window::Preprocess();

	okayButton = static_cast<PushButtonWidget *>(FindWidget("OK"));
	cancelButton = static_cast<PushButtonWidget *>(FindWidget("Cancel"));

	configurationWidget = static_cast<ConfigurationWidget *>(FindWidget("Config"));
	configurationWidget->BuildConfiguration(routeWidget->GetShaderRoute());

	SetNextFocusWidget();
}

bool RouteInfoWindow::HandleKeyboardEvent(const KeyboardEventData *eventData)
{
	if (eventData->eventType == kEventKeyDown)
	{
		if (eventData->keyCode == '-')
		{
			Setting *setting = configurationWidget->FindSetting('NEGA');
			if (setting)
			{
				BooleanSetting *booleanSetting = static_cast<BooleanSetting *>(setting);
				booleanSetting->SetBooleanValue(!booleanSetting->GetBooleanValue());
				booleanSetting->GetSettingInterface()->UpdateCurrentSetting();
				return (true);
			}
		}
	}

	return (Window::HandleKeyboardEvent(eventData));
}

void RouteInfoWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		if (widget == okayButton)
		{
			configurationWidget->CommitConfiguration(routeWidget->GetShaderRoute());
			routeWidget->UpdateContent();

			shaderEditor->SetModifiedFlag();
			Close();
		}
		else if (widget == cancelButton)
		{
			Close();
		}
	}
}


ShaderSectionInfoWindow::ShaderSectionInfoWindow(ShaderEditor *editor) : Window("ShaderEditor/SectionInfo")
{
	shaderEditor = editor;
	sectionWidget = editor->GetFirstSelectedSection();
}

ShaderSectionInfoWindow::~ShaderSectionInfoWindow()
{
}

void ShaderSectionInfoWindow::Preprocess(void)
{
	Window::Preprocess();

	okayButton = static_cast<PushButtonWidget *>(FindWidget("OK"));
	cancelButton = static_cast<PushButtonWidget *>(FindWidget("Cancel"));

	configurationWidget = static_cast<ConfigurationWidget *>(FindWidget("Config"));
	configurationWidget->BuildConfiguration(sectionWidget->GetSectionProcess());

	SetNextFocusWidget();
}

void ShaderSectionInfoWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		if (widget == okayButton)
		{
			configurationWidget->CommitConfiguration(sectionWidget->GetSectionProcess());
			sectionWidget->UpdateContent();

			shaderEditor->SetModifiedFlag();
			Close();
		}
		else if (widget == cancelButton)
		{
			Close();
		}
	}
}


ShaderEditor::ShaderEditor(MaterialWindow *window, MaterialObject *material, PrimitiveType primitiveType) :
		Window("ShaderEditor/Window"),
		toolButtonObserver(this, &ShaderEditor::HandleToolButtonEvent),
		dragRect(ColorRGBA(0.5F, 0.5F, 0.5F, 1.0F)),
		routeVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		routeColorAttribute(ColorRGBA(0.0F, 0.0F, 0.0F, 1.0F)),
		routeTextureAttribute(&routeTextureHeader, routeTextureImage),
		routeRenderable(kRenderQuads)
{
	materialWindow = window;
	materialObject = material;

	initPrimitiveType = primitiveType;

	SetMinWindowSize(Vector2D(640.0F, 512.0F));
	SetStripIcon("ShaderEditor/window");

	const ShaderAttribute *shaderAttribute = static_cast<ShaderAttribute *>(material->FindAttribute(kAttributeShader));
	if (shaderAttribute)
	{
		ShaderAttribute::CloneShader(shaderAttribute->GetShaderGraph(), &shaderGraph);
	}

	if (shaderGraph.Empty())
	{
		TranslateAttributes();
	}

	graphRoot = new Widget;
	sectionRoot = new Widget;
	BuildShaderGraph();
}

ShaderEditor::~ShaderEditor()
{
	TheInterfaceMgr->SetCursor(nullptr);

	delete sectionRoot;
	delete graphRoot;
}

void ShaderEditor::SetWidgetSize(const Vector2D& size)
{
	Window::SetWidgetSize(size);
	PositionWidgets();

	editorState |= kShaderEditorUpdateGrid;
}

void ShaderEditor::Preprocess(void)
{
	static const char *const toolIdentifier[kShaderToolCount] =
	{
		"Move", "Scroll", "Zoom"
	};

	Window::Preprocess();

	shaderViewport = static_cast<OrthoViewportWidget *>(FindWidget("Viewport"));
	viewportBorder = static_cast<BorderWidget *>(FindWidget("Border"));

	shaderViewport->SetMouseEventProc(&ViewportHandleMouseEvent, this);
	shaderViewport->SetTrackTaskProc(&ViewportTrackTask, this);
	shaderViewport->SetRenderProc(&ViewportRender, this);

	OrthoCamera *camera = shaderViewport->GetViewportCamera();
	CameraObject *cameraObject = camera->GetObject();
	cameraObject->SetClearFlags(kClearColorBuffer);
	cameraObject->SetClearColor(K::white);
	cameraObject->SetNearDepth(-1.0F);
	cameraObject->SetFarDepth(1.0F);

	for (machine a = 0; a < kShaderToolCount; a++)
	{
		IconButtonWidget *widget = static_cast<IconButtonWidget *>(FindWidget(toolIdentifier[a]));
		widget->SetObserver(&toolButtonObserver);
		toolButton[a] = widget;
	}

	sectionButton = static_cast<IconButtonWidget *>(FindWidget("Section"));

	TextWidget *text = new TextWidget(Vector2D(256.0F, 16.0F), nullptr, "font/Gui");
	statusMessage = text;
	AppendNewSubnode(text);

	currentMode = kShaderEditorModeTool;
	currentTool = kShaderToolProcessMove;
	currentProcessReg = nullptr;
	toolTracking = false;
	boxSelectFlag = false;

	menuBar = static_cast<MenuBarWidget *>(FindWidget("Menu"));

	processesPage = new ShaderProcessesPage(this);
	previewPage = new ShaderPreviewPage(this, initPrimitiveType);

	bookWidget = new BookWidget(Vector2D(264.0F, 0.0F));
	bookWidget->AppendPage(processesPage);
	bookWidget->AppendPage(previewPage);
	bookWidget->OrganizePages();
	AppendNewSubnode(bookWidget);

	PositionWidgets();
	BuildMenus();

	editorState = kShaderEditorUpdateMenus | kShaderEditorUpdateGraph | kShaderEditorUpdateGrid;

	viewportGrid.SetGridLineSpacing(16.0F);
	viewportGrid.SetMajorLineInterval(2);
	viewportGrid.SetMinorLineColor(ColorRGB(0.9375F, 0.9375F, 0.9375F));
	viewportGrid.SetMajorLineColor(ColorRGB(0.90625F, 0.90625F, 0.90625F));
	viewportGrid.SetAxisLineColor(ColorRGB(0.90625F, 0.90625F, 0.90625F));

	routeRenderable.SetShaderFlags(kShaderAmbientEffect | kShaderVertexPolyboard | kShaderLinearPolyboard | kShaderOrthoPolyboard);
	routeRenderable.SetAmbientBlendState(kBlendPremultInterp);

	routeRenderable.SetVertexCount(4);
	routeRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &routeVertexBuffer, sizeof(RouteVertex));
	routeRenderable.SetVertexAttributeArray(kArrayPosition, 0, 2);
	routeRenderable.SetVertexAttributeArray(kArrayTangent, sizeof(Point2D), 4);
	routeRenderable.SetVertexAttributeArray(kArrayTexcoord, sizeof(Point2D) + sizeof(Vector4D), 2);
	routeVertexBuffer.Establish(sizeof(RouteVertex) * 4);

	routeAttributeList.Append(&routeColorAttribute);
	routeAttributeList.Append(&routeTextureAttribute);
	routeRenderable.SetMaterialAttributeList(&routeAttributeList);

	routeVertex[0].texcoord.Set(0.0F, 1.0F);
	routeVertex[1].texcoord.Set(0.4375F, 1.0F);
	routeVertex[2].texcoord.Set(0.4375F, 1.0F);
	routeVertex[3].texcoord.Set(0.0F, 1.0F);

	UpdateShaderGraph();
}

void ShaderEditor::PositionWidgets(void)
{
	float width = GetWidgetSize().x;
	float height = GetWidgetSize().y;

	const Point3D& position = shaderViewport->GetWidgetPosition();
	Vector2D viewportSize(width - position.x - 4.0F, height - position.y - 4.0F);

	shaderViewport->SetWidgetSize(viewportSize);
	viewportBorder->SetWidgetSize(viewportSize);

	float menuBarHeight = menuBar->GetWidgetSize().y;
	menuBar->SetWidgetSize(Vector2D(width, menuBarHeight));

	bookWidget->SetWidgetPosition(Point3D(4.0F, menuBarHeight + 31.0F, 0.0F));
	bookWidget->SetWidgetSize(Vector2D(bookWidget->GetWidgetSize().x, height - menuBarHeight - 35.0F));
	bookWidget->Invalidate();

	statusMessage->SetWidgetPosition(Point3D(position.x + 7.0F, position.y + viewportSize.y - 17.0F, 0.0F));
	statusMessage->Invalidate();

	float x = position.x;
	float y = position.y;
	float w = viewportSize.x;
	float h = viewportSize.y;

	SetBackgroundQuad(0, Point3D(0.0F, 0.0F, 0.0F), Vector2D(x - 1.0F, height));
	SetBackgroundQuad(1, Point3D(x + w + 1.0F, 0.0F, 0.0F), Vector2D(width - x - w - 1.0F, height));
	SetBackgroundQuad(2, Point3D(x - 1.0F, 0.0F, 0.0F), Vector2D(width - x + 1.0F, y));
	SetBackgroundQuad(3, Point3D(x - 1.0F, y + h + 1.0F, 0.0F), Vector2D(width - x + 1.0F, height - y - h - 1.0F));
}

void ShaderEditor::BuildMenus(void)
{
	const StringTable *table = TheWorldEditor->GetStringTable();

	// Shader Menu

	shaderMenu = new PulldownMenuWidget(table->GetString(StringID('SHDR', 'MENU', 'SHDR')));
	menuBar->AppendMenu(shaderMenu);

	shaderMenu->AppendMenuItem(new MenuItemWidget(table->GetString(StringID('SHDR', 'MENU', 'SHDR', 'CLOS')), WidgetObserver<ShaderEditor>(this, &ShaderEditor::HandleCloseMenuItem), Shortcut('W')));
	shaderMenu->AppendMenuItem(new MenuItemWidget(table->GetString(StringID('SHDR', 'MENU', 'SHDR', 'SAVE')), WidgetObserver<ShaderEditor>(this, &ShaderEditor::HandleSaveShaderMenuItem), Shortcut('S')));

	// Edit Menu

	editMenu = new PulldownMenuWidget(table->GetString(StringID('SHDR', 'MENU', 'EDIT')));
	menuBar->AppendMenu(editMenu);

	MenuItemWidget *widget = new MenuItemWidget(table->GetString(StringID('SHDR', 'MENU', 'EDIT', 'UNDO')), WidgetObserver<ShaderEditor>(this, &ShaderEditor::HandleUndoMenuItem), Shortcut('Z'));
	shaderMenuItem[kShaderMenuUndo] = widget;
	widget->Disable();
	editMenu->AppendMenuItem(widget);

	editMenu->AppendMenuItem(new MenuItemWidget(kLineSolid));

	widget = new MenuItemWidget(table->GetString(StringID('SHDR', 'MENU', 'EDIT', 'CUT ')), WidgetObserver<ShaderEditor>(this, &ShaderEditor::HandleCutMenuItem), Shortcut('X'));
	shaderMenuItem[kShaderMenuCut] = widget;
	editMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('SHDR', 'MENU', 'EDIT', 'COPY')), WidgetObserver<ShaderEditor>(this, &ShaderEditor::HandleCopyMenuItem), Shortcut('C'));
	shaderMenuItem[kShaderMenuCopy] = widget;
	editMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('SHDR', 'MENU', 'EDIT', 'PAST')), WidgetObserver<ShaderEditor>(this, &ShaderEditor::HandlePasteMenuItem), Shortcut('V'));
	shaderMenuItem[kShaderMenuPaste] = widget;
	if (editorClipboard.Empty())
	{
		widget->Disable();
	}

	editMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('SHDR', 'MENU', 'EDIT', 'CLER')), WidgetObserver<ShaderEditor>(this, &ShaderEditor::HandleClearMenuItem), Shortcut(kKeyCodeDelete, kShortcutUnmodified));
	shaderMenuItem[kShaderMenuClear] = widget;
	editMenu->AppendMenuItem(widget);

	editMenu->AppendMenuItem(new MenuItemWidget(kLineSolid));
	editMenu->AppendMenuItem(new MenuItemWidget(table->GetString(StringID('SHDR', 'MENU', 'EDIT', 'SALL')), WidgetObserver<ShaderEditor>(this, &ShaderEditor::HandleSelectAllMenuItem), Shortcut('A')));

	widget = new MenuItemWidget(table->GetString(StringID('SHDR', 'MENU', 'EDIT', 'DUPL')), WidgetObserver<ShaderEditor>(this, &ShaderEditor::HandleDuplicateMenuItem), Shortcut('D'));
	shaderMenuItem[kShaderMenuDuplicate] = widget;
	editMenu->AppendMenuItem(widget);

	// Process Menu

	processMenu = new PulldownMenuWidget(table->GetString(StringID('SHDR', 'MENU', 'PROC')));
	menuBar->AppendMenu(processMenu);

	widget = new MenuItemWidget(table->GetString(StringID('SHDR', 'MENU', 'PROC', 'INFO')), WidgetObserver<ShaderEditor>(this, &ShaderEditor::HandleGetInfoMenuItem), Shortcut('I'));
	shaderMenuItem[kShaderMenuGetInfo] = widget;
	widget->Disable();
	processMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('SHDR', 'MENU', 'PROC', 'TDET')), WidgetObserver<ShaderEditor>(this, &ShaderEditor::HandleToggleDetailLevelMenuItem), Shortcut('L'));
	shaderMenuItem[kShaderMenuToggleDetailLevel] = widget;
	widget->Disable();
	processMenu->AppendMenuItem(widget);
}

void ShaderEditor::TranslateAttributes(void)
{
	static const ConstPoint2D processPosition[kShaderGraphProcessCount] =
	{
		{-2160.0F,  256.0F},		// kShaderGraphTexcoord1
		{-2160.0F,  384.0F},		// kShaderGraphTexcoord2
		{-1920.0F,  320.0F},		// kShaderGraphParallax
		{-1440.0F, -128.0F},		// kShaderGraphDiffuseColor
		{-1200.0F,  384.0F},		// kShaderGraphSpecularColor
		{-1200.0F,  256.0F},		// kShaderGraphSpecularExponent
		{ -960.0F,    0.0F},		// kShaderGraphMicrofacet
		{-1200.0F,  384.0F},		// kShaderGraphMicrofacetReflectivity
		{ -720.0F,  640.0F},		// kShaderGraphEmissionColor
		{ -720.0F,  960.0F},		// kShaderGraphReflectionColor
		{ -720.0F, 1088.0F},		// kShaderGraphRefractionColor
		{ -720.0F, 1344.0F},		// kShaderGraphEnvironmentColor
		{-1680.0F,    0.0F},		// kShaderGraphTextureMap1
		{-1680.0F,  128.0F},		// kShaderGraphTextureMap2
		{-1680.0F,  384.0F},		// kShaderGraphNormalMap1
		{-1680.0F,  512.0F},		// kShaderGraphNormalMap2
		{-1680.0F,  768.0F},		// kShaderGraphSpecularMap
		{-1680.0F,  960.0F},		// kShaderGraphEmissionMap
		{-1680.0F, 1152.0F},		// kShaderGraphOpacityMap
		{    0.0F,    0.0F},		// kShaderGraphDeltaDepth,
		{    0.0F,    0.0F},		// kShaderGraphDepthLinearRamp,
		{    0.0F,    0.0F},		// kShaderGraphFragmentDepth,
		{    0.0F,    0.0F},		// kShaderGraphDepthMultiply,
		{-2160.0F,    0.0F},		// kShaderGraphVertexColor
		{-1440.0F,   64.0F},		// kShaderGraphTextureCombiner
		{-1440.0F,  448.0F},		// kShaderGraphNormalCombiner
		{-1200.0F, -128.0F},		// kShaderGraphColorMultiply
		{    0.0F,    0.0F},		// kShaderGraphAlphaMultiply,
		{ -960.0F, -128.0F},		// kShaderGraphDiffuseMultiply1
		{ -720.0F,  128.0F},		// kShaderGraphDiffuseMultiply2
		{ -960.0F,  384.0F},		// kShaderGraphSpecularMultiply1
		{ -720.0F,  320.0F},		// kShaderGraphSpecularMultiply2
		{ -720.0F,  -64.0F},		// kShaderGraphOcclusionMultiply
		{ -240.0F,  704.0F},		// kShaderGraphEmissionMultiply
		{ -480.0F,    0.0F},		// kShaderGraphOpacityMultiply
		{ -960.0F, 1152.0F},		// kShaderGraphOpacityInvert
		{ -240.0F, 1088.0F},		// kShaderGraphRefractionMultiply
		{ -240.0F, 1344.0F},		// kShaderGraphEnvironmentMultiply
		{-1080.0F,   64.0F},		// kShaderGraphDiffuseReflection
		{ -960.0F,  256.0F},		// kShaderGraphSpecularReflection
		{ -480.0F,  192.0F},		// kShaderGraphLightSum
		{    0.0F,  320.0F},		// kShaderGraphHorizon
		{ -240.0F,  448.0F},		// kShaderGraphBloom1
		{    0.0F,  448.0F},		// kShaderGraphBloom2
		{  240.0F,  128.0F},		// kShaderGraphAmbientOutput
		{  240.0F,  320.0F},		// kShaderGraphLightOutput
		{  240.0F, -192.0F},		// kShaderGraphAlphaOutput
		{  240.0F,  -64.0F},		// kShaderGraphAlphaTestOutput
		{  240.0F,  640.0F},		// kShaderGraphEmissionOutput
		{  240.0F,  960.0F},		// kShaderGraphReflectionOutput
		{  240.0F, 1088.0F},		// kShaderGraphRefractionOutput
		{  240.0F, 1280.0F},		// kShaderGraphEnvironmentOutput
		{  240.0F, 1408.0F},		// kShaderGraphTerrainEnvironmentOutput
		{  240.0F,  768.0F},		// kShaderGraphGlowOutput
		{  240.0F,  448.0F},		// kShaderGraphBloomOutput
		{  240.0F, 1600.0F}			// kShaderGraphImpostorDepthOutput
	};

	Process		*process[kShaderGraphProcessCount];

	ShaderAttribute::BuildRegularShaderGraph(nullptr, nullptr, materialObject, nullptr, &shaderGraph, process);

	for (machine a = 0; a < kShaderGraphProcessCount; a++)
	{
		if (process[a])
		{
			process[a]->SetProcessPosition(processPosition[a]);
		}
	}

	if (!process[kShaderGraphAmbientOutput])
	{
		AmbientOutputProcess *ambientProcess = new AmbientOutputProcess;
		ambientProcess->SetProcessPosition(processPosition[kShaderGraphAmbientOutput]);
		shaderGraph.AddElement(ambientProcess);
	}

	if (!process[kShaderGraphLightOutput])
	{
		LightOutputProcess *lightProcess = new LightOutputProcess;
		lightProcess->SetProcessPosition(processPosition[kShaderGraphLightOutput]);
		shaderGraph.AddElement(lightProcess);
	}

	if (!process[kShaderGraphAlphaOutput])
	{
		AlphaOutputProcess *alphaProcess = new AlphaOutputProcess;
		alphaProcess->SetProcessPosition(processPosition[kShaderGraphAlphaOutput]);
		shaderGraph.AddElement(alphaProcess);
	}

	if (!process[kShaderGraphAlphaTestOutput])
	{
		AlphaTestOutputProcess *alphaTestProcess = new AlphaTestOutputProcess;
		alphaTestProcess->SetProcessPosition(processPosition[kShaderGraphAlphaTestOutput]);
		shaderGraph.AddElement(alphaTestProcess);
	}

	if (!process[kShaderGraphEmissionOutput])
	{
		EmissionOutputProcess *emissionProcess = new EmissionOutputProcess;
		emissionProcess->SetProcessPosition(processPosition[kShaderGraphEmissionOutput]);
		shaderGraph.AddElement(emissionProcess);
	}

	if (!process[kShaderGraphGlowOutput])
	{
		GlowOutputProcess *glowProcess = new GlowOutputProcess;
		glowProcess->SetProcessPosition(processPosition[kShaderGraphGlowOutput]);
		shaderGraph.AddElement(glowProcess);
	}

	if (!process[kShaderGraphBloomOutput])
	{
		BloomOutputProcess *bloomProcess = new BloomOutputProcess;
		bloomProcess->SetProcessPosition(processPosition[kShaderGraphBloomOutput]);
		shaderGraph.AddElement(bloomProcess);
	}

	if (!process[kShaderGraphReflectionOutput])
	{
		ReflectionOutputProcess *reflectionProcess = new ReflectionOutputProcess;
		reflectionProcess->SetProcessPosition(processPosition[kShaderGraphReflectionOutput]);
		shaderGraph.AddElement(reflectionProcess);
	}

	if (!process[kShaderGraphRefractionOutput])
	{
		RefractionOutputProcess *refractionProcess = new RefractionOutputProcess;
		refractionProcess->SetProcessPosition(processPosition[kShaderGraphRefractionOutput]);
		shaderGraph.AddElement(refractionProcess);
	}

	if (!process[kShaderGraphEnvironmentOutput])
	{
		EnvironmentOutputProcess *environmentProcess = new EnvironmentOutputProcess;
		environmentProcess->SetProcessPosition(processPosition[kShaderGraphEnvironmentOutput]);
		shaderGraph.AddElement(environmentProcess);
	}

	if (!process[kShaderGraphTerrainEnvironmentOutput])
	{
		TerrainEnvironmentOutputProcess *terrainEnvironmentProcess = new TerrainEnvironmentOutputProcess;
		terrainEnvironmentProcess->SetProcessPosition(processPosition[kShaderGraphTerrainEnvironmentOutput]);
		shaderGraph.AddElement(terrainEnvironmentProcess);
	}

	if (!process[kShaderGraphImpostorDepthOutput])
	{
		ImpostorDepthOutputProcess *impostorDepthProcess = new ImpostorDepthOutputProcess;
		impostorDepthProcess->SetProcessPosition(processPosition[kShaderGraphImpostorDepthOutput]);
		shaderGraph.AddElement(impostorDepthProcess);
	}
}

void ShaderEditor::BuildShaderGraph(void)
{
	Process *process = shaderGraph.GetFirstElement();
	while (process)
	{
		ProcessType type = process->GetProcessType();
		if (type != kProcessSection)
		{
			ProcessWidget *widget = new ProcessWidget(this, process, Process::FindRegistration(type));
			graphRoot->AppendSubnode(widget);
			processWidgetList.Append(widget);

			Point3D position = process->GetProcessPosition();
			widget->SetWidgetPosition(position);
		}
		else
		{
			SectionProcess *section = static_cast<SectionProcess *>(process);
			ShaderSectionWidget *widget = new ShaderSectionWidget(this, section);
			sectionRoot->AppendSubnode(widget);
			sectionWidgetList.Append(widget);

			Point3D position = process->GetProcessPosition();
			widget->SetWidgetPosition(position);
			widget->SetWidgetSize(Vector2D(section->GetSectionWidth(), section->GetSectionHeight()));
		}

		process = process->GetNextElement();
	}

	process = shaderGraph.GetFirstElement();
	while (process)
	{
		Route *route = process->GetFirstIncomingEdge();
		while (route)
		{
			RouteWidget *widget = new RouteWidget(this, route);
			graphRoot->PrependSubnode(widget);
			routeWidgetList.Append(widget);

			route = route->GetNextIncomingEdge();
		}

		process = process->GetNextElement();
	}

	graphRoot->Preprocess();
	sectionRoot->Preprocess();

	graphRoot->Update();
	sectionRoot->Update();
}

void ShaderEditor::UpdateShaderGraph(void)
{
	static unsigned message[kShaderResultCount] =
	{
		'ICMP', 'TEMP', 'LITR', 'TEXC', 'TXTR'
	};

	ShaderResult	result[2];

	previewPage->UpdatePreviewMaterial(&shaderGraph, result);

	if (result[0] != kShaderOkay)
	{
		statusMessage->SetText(TheWorldEditor->GetStringTable()->GetString(StringID('SHDR', 'MESS', message[result[0] - 1])));
	}
	else if (result[1] != kShaderOkay)
	{
		statusMessage->SetText(TheWorldEditor->GetStringTable()->GetString(StringID('SHDR', 'MESS', message[result[1] - 1])));
	}
	else
	{
		statusMessage->SetText(nullptr);
	}
}

void ShaderEditor::ReattachProcess(ProcessWidget *widget)
{
	shaderGraph.AddElement(widget->GetShaderProcess());
	graphRoot->AppendSubnode(widget);
	processWidgetList.Append(widget);

	widget->SetViewportScale(shaderViewport->GetOrthoScale().x);
}

void ShaderEditor::ReattachRoute(RouteWidget *widget)
{
	widget->GetShaderRoute()->Attach();
	graphRoot->PrependSubnode(widget);
	routeWidgetList.Append(widget);

	widget->SetViewportScale(shaderViewport->GetOrthoScale().x);
}

void ShaderEditor::ReattachSection(ShaderSectionWidget *widget)
{
	shaderGraph.AddElement(widget->GetSectionProcess());
	sectionRoot->AppendSubnode(widget);
	sectionWidgetList.Append(widget);

	widget->SetViewportScale(shaderViewport->GetOrthoScale().x);
}

void ShaderEditor::RebuildRouteWidgets(const Process *process)
{
	RouteWidget *routeWidget = routeWidgetList.First();
	while (routeWidget)
	{
		const Route *route = routeWidget->GetShaderRoute();
		if ((route->GetStartElement() == process) || (route->GetFinishElement() == process))
		{
			routeWidget->Rebuild();
		}

		routeWidget = routeWidget->Next();
	}

	routeWidget = selectedRouteList.First();
	while (routeWidget)
	{
		const Route *route = routeWidget->GetShaderRoute();
		if ((route->GetStartElement() == process) || (route->GetFinishElement() == process))
		{
			routeWidget->Rebuild();
		}

		routeWidget = routeWidget->Next();
	}
}

void ShaderEditor::SelectDefaultTool(void)
{
	if ((currentMode != kShaderEditorModeTool) || (currentTool != kShaderToolProcessMove))
	{
		UnselectCurrentTool();
		toolButton[kShaderToolProcessMove]->SetValue(1);

		currentMode = kShaderEditorModeTool;
		currentTool = kShaderToolProcessMove;
	}
}

void ShaderEditor::SelectProcessTool(const ProcessRegistration *registration)
{
	if ((currentMode != kShaderEditorModeProcess) || (currentProcessReg != registration))
	{
		if (currentMode != kShaderEditorModeProcess)
		{
			UnselectCurrentTool();
		}

		currentMode = kShaderEditorModeProcess;
		currentTool = 0;
		currentProcessReg = registration;
	}
}

void ShaderEditor::UnselectCurrentTool(void)
{
	switch (currentMode)
	{
		case kShaderEditorModeTool:

			toolButton[currentTool]->SetValue(0);
			break;

		case kShaderEditorModeProcess:

			processesPage->UnselectProcessTool();
			break;

		case kShaderEditorModeSection:

			sectionButton->SetValue(0);
			break;
	}
}

void ShaderEditor::UpdateViewportScale(float scale, const Point3D *position)
{
	scale = Clamp(scale, 1.0F, 8.0F);

	if (position)
	{
		shaderViewport->SetZoomScale(Vector2D(scale, scale), *position, true);
	}
	else
	{
		shaderViewport->SetOrthoScale(Vector2D(scale, scale));
	}

	editorState |= kShaderEditorUpdateGrid;

	ProcessWidget *process = processWidgetList.First();
	while (process)
	{
		process->SetViewportScale(scale);
		process = process->Next();
	}

	process = selectedProcessList.First();
	while (process)
	{
		process->SetViewportScale(scale);
		process = process->Next();
	}

	RouteWidget *route = routeWidgetList.First();
	while (route)
	{
		route->SetViewportScale(scale);
		route = route->Next();
	}

	route = selectedRouteList.First();
	while (route)
	{
		route->SetViewportScale(scale);
		route = route->Next();
	}

	ShaderSectionWidget *section = sectionWidgetList.First();
	while (section)
	{
		section->SetViewportScale(scale);
		section = section->Next();
	}

	section = selectedSectionList.First();
	while (section)
	{
		section->SetViewportScale(scale);
		section = section->Next();
	}
}

void ShaderEditor::SelectProcess(ProcessWidget *processWidget, unsigned_int32 state)
{
	selectedProcessList.Append(processWidget);
	processWidget->Select(state);

	const Process *process = processWidget->GetShaderProcess();

	RouteWidget *routeWidget = routeWidgetList.First();
	while (routeWidget)
	{
		const Route *route = routeWidget->GetShaderRoute();
		if ((route->GetStartElement() == process) || (route->GetFinishElement() == process))
		{
			routeWidget->Hilite();
		}

		routeWidget = routeWidget->Next();
	}

	editorState |= kShaderEditorUpdateMenus;
}

void ShaderEditor::UnselectProcess(ProcessWidget *processWidget)
{
	processWidgetList.Append(processWidget);
	processWidget->Unselect();

	const Process *process = processWidget->GetShaderProcess();

	RouteWidget *routeWidget = routeWidgetList.First();
	while (routeWidget)
	{
		const Route *route = routeWidget->GetShaderRoute();
		if ((route->GetStartElement() == process) || (route->GetFinishElement() == process))
		{
			routeWidget->Unhilite();
		}

		routeWidget = routeWidget->Next();
	}

	editorState |= kShaderEditorUpdateMenus;
}

void ShaderEditor::SelectRoute(RouteWidget *routeWidget)
{
	selectedRouteList.Append(routeWidget);
	routeWidget->Select();

	editorState |= kShaderEditorUpdateMenus;
}

void ShaderEditor::UnselectRoute(RouteWidget *routeWidget)
{
	routeWidgetList.Append(routeWidget);
	routeWidget->Unselect();

	editorState |= kShaderEditorUpdateMenus;
}

void ShaderEditor::SelectSection(ShaderSectionWidget *sectionWidget)
{
	selectedSectionList.Append(sectionWidget);
	sectionWidget->Select();

	editorState |= kShaderEditorUpdateMenus;
}

void ShaderEditor::UnselectSection(ShaderSectionWidget *sectionWidget)
{
	sectionWidgetList.Append(sectionWidget);
	sectionWidget->Unselect();

	editorState |= kShaderEditorUpdateMenus;
}

void ShaderEditor::SelectAll(void)
{
	for (;;)
	{
		RouteWidget *widget = selectedRouteList.First();
		if (!widget)
		{
			break;
		}

		UnselectRoute(widget);
	}

	for (;;)
	{
		ProcessWidget *widget = processWidgetList.First();
		if (!widget)
		{
			break;
		}

		SelectProcess(widget);
	}

	for (;;)
	{
		ShaderSectionWidget *widget = sectionWidgetList.First();
		if (!widget)
		{
			break;
		}

		SelectSection(widget);
	}
}

void ShaderEditor::UnselectAll(void)
{
	for (;;)
	{
		ProcessWidget *widget = selectedProcessList.First();
		if (!widget)
		{
			break;
		}

		UnselectProcess(widget);
	}

	for (;;)
	{
		RouteWidget *widget = selectedRouteList.First();
		if (!widget)
		{
			break;
		}

		UnselectRoute(widget);
	}

	for (;;)
	{
		ShaderSectionWidget *widget = selectedSectionList.First();
		if (!widget)
		{
			break;
		}

		UnselectSection(widget);
	}
}

void ShaderEditor::UnselectAllTemp(void)
{
	ProcessWidget *widget = selectedProcessList.First();
	while (widget)
	{
		ProcessWidget *next = widget->Next();

		if (widget->GetProcessWidgetState() & kProcessWidgetTempSelected)
		{
			UnselectProcess(widget);
		}

		widget = next;
	}
}

void ShaderEditor::DeleteProcess(ProcessWidget *processWidget, List<ProcessWidget> *deletedProcessList, List<RouteWidget> *deletedRouteList)
{
	Process *process = processWidget->GetShaderProcess();

	RouteWidget *routeWidget = routeWidgetList.First();
	while (routeWidget)
	{
		RouteWidget *next = routeWidget->Next();

		Route *route = routeWidget->GetShaderRoute();
		if ((route->GetStartElement() == process) || (route->GetFinishElement() == process))
		{
			if (deletedRouteList)
			{
				route->GraphEdge<Process, Route>::Detach();

				routeWidget->HandleDelete();
				routeWidget->Widget::Detach();

				deletedRouteList->Append(routeWidget);
			}
			else
			{
				delete route;
				delete routeWidget;
			}
		}

		routeWidget = next;
	}

	if (deletedProcessList)
	{
		shaderGraph.RemoveElement(process);

		processWidget->Unselect();
		processWidget->Widget::Detach();

		deletedProcessList->Append(processWidget);
	}
	else
	{
		delete process;
		delete processWidget;
	}

	editorState |= kShaderEditorUpdateMenus | kShaderEditorUpdateGraph;
}

void ShaderEditor::DeleteRoute(RouteWidget *routeWidget, List<RouteWidget> *deletedRouteList)
{
	Route *route = routeWidget->GetShaderRoute();
	if (deletedRouteList)
	{
		route->GraphEdge<Process, Route>::Detach();

		routeWidget->HandleDelete();
		routeWidget->Widget::Detach();

		deletedRouteList->Append(routeWidget);
	}
	else
	{
		delete route;
		delete routeWidget;
	}

	editorState |= kShaderEditorUpdateMenus | kShaderEditorUpdateGraph;
}

void ShaderEditor::DeleteSection(ShaderSectionWidget *sectionWidget, List<ShaderSectionWidget> *deletedSectionList)
{
	if (deletedSectionList)
	{
		shaderGraph.RemoveElement(sectionWidget->GetSectionProcess());

		sectionWidget->Unselect();
		sectionWidget->Widget::Detach();

		deletedSectionList->Append(sectionWidget);
	}
	else
	{
		delete sectionWidget->GetSectionProcess();
		delete sectionWidget;
	}

	editorState |= kShaderEditorUpdateMenus | kShaderEditorUpdateGraph;
}

void ShaderEditor::AddOperation(ShaderOperation *operation)
{
	if (operationList.GetElementCount() >= kMaxShaderOperationCount)
	{
		ShaderOperation *oldestOperation = operationList.First();
		for (;;)
		{
			ShaderOperation *next = oldestOperation->Next();
			if ((!next) || (!next->Coupled()))
			{
				break;
			}

			oldestOperation = next;
		}

		do
		{
			ShaderOperation *previous = oldestOperation->Previous();
			delete oldestOperation;
			oldestOperation = previous;
		} while (oldestOperation);
	}

	operationList.Append(operation);

	shaderMenuItem[kShaderMenuUndo]->Enable();
	editorState |= kShaderEditorUnsaved;
}

void ShaderEditor::HandleCloseMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	Close();
}

void ShaderEditor::HandleSaveShaderMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	materialObject->PurgeAttributes();

	ShaderAttribute *shaderAttribute = new ShaderAttribute;
	materialObject->AddAttribute(shaderAttribute);

	ShaderAttribute::CloneShader(&shaderGraph, shaderAttribute->GetShaderGraph());

	editorState &= ~kShaderEditorUnsaved;

	if (materialWindow)
	{
		materialWindow->HandleMaterialModification();
	}
}

void ShaderEditor::HandleUndoMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	for (;;)
	{
		ShaderOperation *operation = operationList.Last();
		if (!operation)
		{
			break;
		}

		bool coupled = operation->Coupled();
		operation->Restore(this);
		delete operation;

		if (!coupled)
		{
			break;
		}
	}

	if (operationList.Empty())
	{
		shaderMenuItem[kShaderMenuUndo]->Disable();
	}

	editorState |= kShaderEditorUnsaved | kShaderEditorUpdateMenus | kShaderEditorUpdateGraph;
}

void ShaderEditor::HandleCutMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	HandleCopyMenuItem(nullptr, nullptr);
	HandleClearMenuItem(nullptr, nullptr);
}

void ShaderEditor::HandleCopyMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	editorClipboard.Purge();

	const ProcessWidget *processWidget = processWidgetList.First();
	while (processWidget)
	{
		processWidget->GetShaderProcess()->SetCloneProcess(nullptr);
		processWidget = processWidget->Next();
	}

	processWidget = selectedProcessList.First();
	while (processWidget)
	{
		Process *process = processWidget->GetShaderProcess();
		if (process->GetBaseProcessType() != kProcessOutput)
		{
			Process *clone = process->Clone();
			process->SetCloneProcess(clone);
			editorClipboard.AddElement(clone);
		}

		processWidget = processWidget->Next();
	}

	processWidget = selectedProcessList.First();
	while (processWidget)
	{
		const Process *process = processWidget->GetShaderProcess();
		if (process->GetBaseProcessType() != kProcessOutput)
		{
			Process *finish = process->GetCloneProcess();

			const Route *route = process->GetFirstIncomingEdge();
			while (route)
			{
				Process *start = route->GetStartElement()->GetCloneProcess();
				if (start)
				{
					new Route(*route, start, finish);
				}

				route = route->GetNextIncomingEdge();
			}
		}

		processWidget = processWidget->Next();
	}

	ShaderSectionWidget *sectionWidget = selectedSectionList.First();
	while (sectionWidget)
	{
		Process *clone = sectionWidget->GetSectionProcess()->Clone();
		editorClipboard.AddElement(clone);

		sectionWidget = sectionWidget->Next();
	}

	MenuItemWidget *widget = shaderMenuItem[kShaderMenuPaste];
	if (editorClipboard.Empty())
	{
		widget->Disable();
	}
	else
	{
		widget->Enable();
	}
}

void ShaderEditor::HandlePasteMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	ShaderGraph					graph;
	List<ProcessWidget>			processList;
	List<RouteWidget>			routeList;
	List<ShaderSectionWidget>	sectionList;

	UnselectAll();

	ShaderAttribute::CloneShader(&editorClipboard, &graph);

	float xmin = K::infinity;
	float ymin = K::infinity;
	float xmax = K::minus_infinity;
	float ymax = K::minus_infinity;

	Process *process = graph.GetFirstElement();
	while (process)
	{
		Process *next = process->GetNextElement();

		ProcessType type = process->GetProcessType();
		if (type != kProcessSection)
		{
			const Point2D& p = process->GetProcessPosition();
			xmin = Fmin(xmin, p.x);
			xmax = Fmax(xmax, p.x + kProcessBoxWidth);
			ymin = Fmin(ymin, p.y);
			ymax = Fmax(ymax, p.y + kProcessBoxHeight);
		}
		else
		{
			const Point2D& p = process->GetProcessPosition();

			const SectionProcess *section = static_cast<SectionProcess *>(process);
			float width = section->GetSectionWidth();
			float height = section->GetSectionHeight();

			xmin = Fmin(xmin, p.x);
			xmax = Fmax(xmax, p.x + width);
			ymin = Fmin(ymin, p.y);
			ymax = Fmax(ymax, p.y + height);
		}

		process = next;
	}

	const Box2D *box = graphRoot->GetBoundingBox();
	float dy = (box) ? box->max.y + (kProcessBoxPaddingY + 16.0F) - ymin : 0.0F;
	dy = Floor((dy + 4.0F) * 0.125F) * 8.0F;

	float xcen = (xmin + xmax) * 0.5F;
	float ycen = ymax + dy + 8.0F;
	ShowGraphPosition(xcen, ycen);

	process = graph.GetFirstElement();
	while (process)
	{
		const Point2D& p = process->GetProcessPosition();
		process->SetProcessPosition(Point2D(p.x, p.y + dy));
		process = process->GetNextElement();
	}

	process = graph.GetFirstElement();
	while (process)
	{
		ProcessType type = process->GetProcessType();
		Point3D position = process->GetProcessPosition();

		if (type != kProcessSection)
		{
			ProcessWidget *widget = new ProcessWidget(this, process, Process::FindRegistration(type));
			widget->SetViewportScale(shaderViewport->GetOrthoScale().x);
			graphRoot->AppendNewSubnode(widget);
			widget->SetWidgetPosition(position);
			processList.Append(widget);
		}
		else
		{
			ShaderSectionWidget *widget = new ShaderSectionWidget(this, static_cast<SectionProcess *>(process));
			widget->SetViewportScale(shaderViewport->GetOrthoScale().x);
			sectionRoot->AppendNewSubnode(widget);
			widget->SetWidgetPosition(position);
			sectionList.Append(widget);
		}

		process = process->GetNextElement();
	}

	process = graph.GetFirstElement();
	while (process)
	{
		Route *route = process->GetFirstIncomingEdge();
		while (route)
		{
			RouteWidget *widget = new RouteWidget(this, route);
			widget->SetViewportScale(shaderViewport->GetOrthoScale().x);
			graphRoot->PrependSubnode(widget);
			routeList.Append(widget);
			widget->Preprocess();

			route = route->GetNextIncomingEdge();
		}

		process = process->GetNextElement();
	}

	for (;;)
	{
		process = graph.GetFirstElement();
		if (!process)
		{
			break;
		}

		shaderGraph.AddElement(process);
	}

	AddOperation(new CreateShaderOperation(&processList, &routeList, &sectionList));

	for (;;)
	{
		RouteWidget *widget = routeList.First();
		if (!widget)
		{
			break;
		}

		routeWidgetList.Append(widget);
	}

	for (;;)
	{
		ProcessWidget *widget = processList.First();
		if (!widget)
		{
			break;
		}

		SelectProcess(widget);
	}

	for (;;)
	{
		ShaderSectionWidget *widget = sectionList.First();
		if (!widget)
		{
			break;
		}

		SelectSection(widget);
	}

	graphRoot->Update();
	sectionRoot->Update();
	editorState |= kShaderEditorUpdateGraph;
}

void ShaderEditor::HandleClearMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	List<ProcessWidget>			deletedProcessList;
	List<RouteWidget>			deletedRouteList;
	List<ShaderSectionWidget>	deletedSectionList;

	RouteWidget *routeWidget = selectedRouteList.First();
	while (routeWidget)
	{
		RouteWidget *next = routeWidget->Next();
		DeleteRoute(routeWidget, &deletedRouteList);
		routeWidget = next;
	}

	ProcessWidget *processWidget = selectedProcessList.First();
	while (processWidget)
	{
		ProcessWidget *next = processWidget->Next();

		const Process *process = processWidget->GetShaderProcess();
		if (process->GetBaseProcessType() != kProcessOutput)
		{
			DeleteProcess(processWidget, &deletedProcessList, &deletedRouteList);
		}

		processWidget = next;
	}

	ShaderSectionWidget *sectionWidget = selectedSectionList.First();
	while (sectionWidget)
	{
		ShaderSectionWidget *next = sectionWidget->Next();
		DeleteSection(sectionWidget, &deletedSectionList);
		sectionWidget = next;
	}

	if ((!deletedProcessList.Empty()) || (!deletedRouteList.Empty()) || (!deletedSectionList.Empty()))
	{
		AddOperation(new DeleteShaderOperation(&deletedProcessList, &deletedRouteList, &deletedSectionList));
	}
}

void ShaderEditor::HandleSelectAllMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	SelectAll();
}

void ShaderEditor::HandleDuplicateMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	HandleCopyMenuItem(nullptr, nullptr);
	HandlePasteMenuItem(nullptr, nullptr);
}

void ShaderEditor::HandleGetInfoMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	const ProcessWidget *processWidget = selectedProcessList.First();
	if (processWidget)
	{
		if (selectedProcessList.Last() == processWidget)
		{
			AddSubwindow(new ProcessInfoWindow(this));
		}

		return;
	}

	const RouteWidget *routeWidget = selectedRouteList.First();
	if (routeWidget)
	{
		if (selectedRouteList.Last() == routeWidget)
		{
			AddSubwindow(new RouteInfoWindow(this));
		}

		return;
	}

	const ShaderSectionWidget *sectionWidget = selectedSectionList.First();
	if (sectionWidget)
	{
		if (selectedSectionList.Last() == sectionWidget)
		{
			AddSubwindow(new ShaderSectionInfoWindow(this));
		}

		return;
	}
}

void ShaderEditor::HandleToggleDetailLevelMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	AddOperation(new RouteShaderOperation(&selectedRouteList));

	RouteWidget *routeWidget = selectedRouteList.First();
	while (routeWidget)
	{
		Route *route = routeWidget->GetShaderRoute();
		const Process *process = route->GetFinishElement();
		if (process->GetPortFlags(route->GetRoutePort()) & (kProcessPortOptional | kProcessPortOmissible))
		{
			route->SetRouteFlags(route->GetRouteFlags() ^ kRouteHighDetail);
			routeWidget->Rebuild();
		}

		routeWidget = routeWidget->Next();
	}
}

Point3D ShaderEditor::ViewportToGraphPosition(const Point3D& p) const
{
	const Vector2D& scale = shaderViewport->GetOrthoScale();
	OrthoCamera *camera = shaderViewport->GetViewportCamera();
	const Point3D& position = camera->GetNodePosition();

	const OrthoCameraObject *object = camera->GetObject();
	return (Point3D(p.x * scale.x + object->GetOrthoRectLeft() + position.x, p.y * scale.y + object->GetOrthoRectTop() + position.y, 0.0F));
}

Point3D ShaderEditor::AlignPositionToGrid(const Point3D& p)
{
	float x = Floor((p.x + 4.0F) * 0.125F) * 8.0F;
	float y = Floor((p.y + 4.0F) * 0.125F) * 8.0F;
	return (Point3D(x, y, 0.0F));
}

void ShaderEditor::ShowGraphPosition(float x, float y)
{
	const Vector2D& scale = shaderViewport->GetOrthoScale();
	OrthoCamera *camera = shaderViewport->GetViewportCamera();
	const Point3D& position = camera->GetNodePosition();

	float w = shaderViewport->GetWidgetSize().x * scale.x;
	float h = shaderViewport->GetWidgetSize().y * scale.y;

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
	editorState |= kShaderEditorUpdateGrid;
}

void ShaderEditor::AutoScroll(const Point3D& p)
{
	float w = shaderViewport->GetWidgetSize().x;
	float h = shaderViewport->GetWidgetSize().y;

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

		const Vector2D& scale = shaderViewport->GetOrthoScale();
		OrthoCamera *camera = shaderViewport->GetViewportCamera();
		camera->SetNodePosition(camera->GetNodePosition() + Vector3D(Floor(dx * scale.x + 0.5F), Floor(dy * scale.y + 0.5F), 0.0F));
		editorState |= kShaderEditorUpdateGrid;
	}
}

ProcessWidget *ShaderEditor::FindProcessWidget(const Point3D& position) const
{
	WidgetPart	part;

	Widget *widget = graphRoot->DetectWidget(position, 0, &part);
	if ((widget) && (part == kWidgetPartInterior))
	{
		return (static_cast<ProcessWidget *>(widget));
	}

	return (nullptr);
}

ProcessWidget *ShaderEditor::FindProcessPort(const Point3D& position, int32 *port) const
{
	WidgetPart	part;

	Widget *widget = graphRoot->DetectWidget(position, 0, &part);
	if (widget)
	{
		int32 index = part - kWidgetPartPort;
		if ((index >= 0) && (index < kMaxProcessPortCount))
		{
			*port = index;
			return (static_cast<ProcessWidget *>(widget));
		}
	}

	return (nullptr);
}

RouteWidget *ShaderEditor::GetRouteWidget(const Route *route) const
{
	RouteWidget *widget = routeWidgetList.First();
	while (widget)
	{
		if (widget->GetShaderRoute() == route)
		{
			return (widget);
		}

		widget = widget->Next();
	}

	widget = selectedRouteList.First();
	while (widget)
	{
		if (widget->GetShaderRoute() == route)
		{
			return (widget);
		}

		widget = widget->Next();
	}

	return (nullptr);
}

void ShaderEditor::SortSelectedProcessSublist(List<ProcessWidget> *list, float dmin, float dmax)
{
	ProcessWidget *widget = list->Last();
	if ((list->First() == widget) || (dmax - dmin < 4.0F))
	{
		while (widget)
		{
			ProcessWidget *prev = widget->Previous();
			selectedProcessList.Append(widget);
			widget = prev;
		}
	}
	else
	{
		List<ProcessWidget>		nearList;

		float dminFar = dmax;
		float dmaxNear = dmin;
		float avg = (dmin + dmax) * 0.5F;

		widget = list->First();
		do
		{
			ProcessWidget *next = widget->Next();
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

		SortSelectedProcessSublist(list, dminFar, dmax);
		SortSelectedProcessSublist(&nearList, dmin, dmaxNear);
	}
}

void ShaderEditor::SortSelectedProcessList(float dx, float dy)
{
	List<ProcessWidget>		widgetList;

	float dmin = K::infinity;
	float dmax = K::minus_infinity;

	ProcessWidget *widget = selectedProcessList.First();
	while (widget)
	{
		ProcessWidget *next = widget->Next();

		const Point3D& position = widget->GetWidgetPosition();
		float d = position.x * dx + position.y * dy;
		dmin = Fmin(dmin, d);
		dmax = Fmax(dmax, d);

		widget->SetSortPosition(d);
		widgetList.Append(widget);

		widget = next;
	}

	SortSelectedProcessSublist(&widgetList, dmin, dmax);
}

bool ShaderEditor::BoxIntersectsProcessWidget(const Point3D& p1, const Point3D& p2, const ProcessWidget *widget)
{
	const Point3D& position = widget->GetWidgetPosition();

	float x1 = Fmin(p1.x, p2.x);
	float x2 = Fmax(p1.x, p2.x);
	float y1 = Fmin(p1.y, p2.y);
	float y2 = Fmax(p1.y, p2.y);

	if ((x2 < position.x) || (x1 > position.x + kProcessBoxWidth))
	{
		return (false);
	}

	if ((y2 < position.y) || (y1 > position.y + kProcessBoxHeight))
	{
		return (false);
	}

	return (true);
}

bool ShaderEditor::ProcessBoxIntersectsAnyProcessWidget(float x, float y, const ProcessWidget *exclude)
{
	Point3D p1(x - kProcessBoxPaddingX, y - kProcessBoxPaddingY, 0.0F);
	Point3D p2(x + kProcessBoxWidth + kProcessBoxPaddingX, y + kProcessBoxHeight + kProcessBoxPaddingY, 0.0F);

	const ProcessWidget *widget = processWidgetList.First();
	while (widget)
	{
		if ((widget != exclude) && (BoxIntersectsProcessWidget(p1, p2, widget)))
		{
			return (true);
		}

		widget = widget->Next();
	}

	widget = selectedProcessList.First();
	while (widget)
	{
		if ((widget != exclude) && (BoxIntersectsProcessWidget(p1, p2, widget)))
		{
			return (true);
		}

		widget = widget->Next();
	}

	return (false);
}

void ShaderEditor::BeginTool(const Point3D& p, unsigned_int32 eventFlags)
{
	previousPoint = p;
	anchorPoint = p;

	int32 tool = trackingTool;
	if (tool == kShaderToolProcessMove)
	{
		WidgetPart	part;

		bool moveable = false;
		bool shift = InterfaceMgr::GetShiftKey();
		anchorPosition = ViewportToGraphPosition(p);
		previousPosition = anchorPosition;

		Widget *widget = graphRoot->DetectWidget(previousPosition, 0, &part);
		if (widget)
		{
			if (part == kWidgetPartCurve)
			{
				RouteWidget *routeWidget = static_cast<RouteWidget *>(widget);
				bool selected = RouteSelected(routeWidget);
				if (shift)
				{
					if (!selected)
					{
						SelectRoute(routeWidget);
					}
					else
					{
						UnselectRoute(routeWidget);
					}
				}
				else
				{
					if (!selected)
					{
						UnselectAll();
						SelectRoute(routeWidget);
					}

					if (eventFlags & kMouseDoubleClick)
					{
						HandleGetInfoMenuItem(nullptr, nullptr);
						return;
					}
				}
			}
			else if (part == kWidgetPartOutput)
			{
				UnselectAll();

				ProcessWidget *processWidget = static_cast<ProcessWidget *>(widget);
				processWidget->UpdateOutputColor(true);

				routeStartProcess = processWidget;
				routeFinishProcess = nullptr;

				routeVertex[0].position = anchorPosition.GetPoint2D();
				routeVertex[1].position = anchorPosition.GetPoint2D();
				routeVertex[2].position = anchorPosition.GetPoint2D();
				routeVertex[3].position = anchorPosition.GetPoint2D();

				routeVertex[0].tangent.Set(1.0F, 0.0F, 0.0F, -3.0F);
				routeVertex[1].tangent.Set(1.0F, 0.0F, 0.0F, 4.0F);
				routeVertex[2].tangent.Set(1.0F, 0.0F, 0.0F, 4.0F);
				routeVertex[3].tangent.Set(1.0F, 0.0F, 0.0F, -3.0F);

				routeVertexBuffer.UpdateBuffer(0, sizeof(RouteVertex) * 4, routeVertex);

				trackingMode = kShaderEditorModeRoute;
			}
			else
			{
				ProcessWidget *processWidget = static_cast<ProcessWidget *>(widget);
				bool selected = ProcessSelected(processWidget);
				if (shift)
				{
					if (!selected)
					{
						SelectProcess(processWidget, kProcessWidgetTempSelected);
					}
					else
					{
						UnselectProcess(processWidget);
					}
				}
				else
				{
					if (!selected)
					{
						UnselectAll();
						SelectProcess(processWidget, kProcessWidgetTempSelected);
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
			widget = sectionRoot->DetectWidget(previousPosition, 0, &part);
			if (widget)
			{
				ShaderSectionWidget *sectionWidget = static_cast<ShaderSectionWidget *>(widget);
				bool selected = SectionSelected(sectionWidget);

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
					trackingMode = kShaderEditorModeSection;
					trackingTool = 0;

					AddOperation(new ResizeShaderOperation(sectionWidget));
				}
			}
			else
			{
				if (!shift)
				{
					UnselectAll();
				}

				trackingTool = kShaderToolGraphSelect;
			}
		}

		if (moveable)
		{
			ProcessWidget *processWidget = selectedProcessList.First();
			while (processWidget)
			{
				processWidget->SaveOriginalPosition();
				processWidget = processWidget->Next();
			}

			ShaderSectionWidget *sectionWidget = selectedSectionList.First();
			while (sectionWidget)
			{
				sectionWidget->SaveOriginalPosition();
				sectionWidget = sectionWidget->Next();
			}

			editorState |= kShaderEditorUndoPending;
		}
	}

	toolTracking = true;
}

void ShaderEditor::TrackTool(const Point3D& p)
{
	int32 tool = trackingTool;
	if (tool == kShaderToolGraphSelect)
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
				dragRect.Build(anchorPosition.GetPoint2D(), position.GetPoint2D(), shaderViewport->GetOrthoScale().x);

				if ((dx >= 1.0F) && (dy >= 1.0F))
				{
					UnselectAllTemp();

					ProcessWidget *widget = processWidgetList.First();
					while (widget)
					{
						ProcessWidget *next = widget->Next();

						if (BoxIntersectsProcessWidget(anchorPosition, position, widget))
						{
							SelectProcess(widget, kProcessWidgetTempSelected);
						}

						widget = next;
					}
				}
			}
		}
	}
	else if (tool == kShaderToolProcessMove)
	{
		AutoScroll(p);

		Point3D position = ViewportToGraphPosition(p);
		if (position != previousPosition)
		{
			unsigned_int32 state = editorState;
			if (state & kShaderEditorUndoPending)
			{
				editorState = state & ~kShaderEditorUndoPending;
				AddOperation(new MoveShaderOperation(&selectedProcessList, &selectedSectionList));
			}

			SortSelectedProcessList(position.x - previousPosition.x, position.y - previousPosition.y);

			previousPosition = position;
			float dx = position.x - anchorPosition.x;
			float dy = position.y - anchorPosition.y;

			ProcessWidget *processWidget = selectedProcessList.First();
			while (processWidget)
			{
				Point3D q = processWidget->GetOriginalPosition();
				q.x += dx;
				q.y += dy;
				q = AlignPositionToGrid(q);

				bool clear = !ProcessBoxIntersectsAnyProcessWidget(q.x, q.y, processWidget);
				if (!clear)
				{
					const Point3D& currentPosition = processWidget->GetWidgetPosition();

					if (!ProcessBoxIntersectsAnyProcessWidget(q.x, currentPosition.y, processWidget))
					{
						clear = true;
						q.y = currentPosition.y;
					}

					if ((!clear) && (!ProcessBoxIntersectsAnyProcessWidget(currentPosition.x, q.y, processWidget)))
					{
						clear = true;
						q.x = currentPosition.x;
					}
				}

				if (clear)
				{
					Process *process = processWidget->GetShaderProcess();

					process->SetProcessPosition(q.GetPoint2D());
					processWidget->SetWidgetPosition(q);
					processWidget->Invalidate();

					RebuildRouteWidgets(process);
				}

				processWidget = processWidget->Next();
			}

			ShaderSectionWidget *sectionWidget = selectedSectionList.First();
			while (sectionWidget)
			{
				Point3D q = sectionWidget->GetOriginalPosition();
				q.x += dx;
				q.y += dy;
				q = AlignPositionToGrid(q);

				SectionProcess *section = sectionWidget->GetSectionProcess();

				section->SetProcessPosition(q.GetPoint2D());
				sectionWidget->SetWidgetPosition(q);
				sectionWidget->Invalidate();

				sectionWidget = sectionWidget->Next();
			}
		}
	}
	else if (tool == kShaderToolViewportScroll)
	{
		float dx = previousPoint.x - p.x;
		float dy = previousPoint.y - p.y;

		if ((dx != 0.0F) || (dy != 0.0F))
		{
			OrthoCamera *camera = shaderViewport->GetViewportCamera();
			const Vector2D& scale = shaderViewport->GetOrthoScale();
			camera->SetNodePosition(camera->GetNodePosition() + Vector3D(Floor(dx * scale.x + 0.5F), Floor(dy * scale.y + 0.5F), 0.0F));
			editorState |= kShaderEditorUpdateGrid;
		}
	}
	else if (tool == kShaderToolViewportZoom)
	{
		float dy = previousPoint.y - p.y;
		if (dy != 0.0F)
		{
			UpdateViewportScale(shaderViewport->GetOrthoScale().x * Exp(dy * -0.01F));
		}
	}

	previousPoint = p;
}

void ShaderEditor::EndTool(const Point3D& p)
{
	boxSelectFlag = false;
	editorState &= ~kShaderEditorUndoPending;

	ProcessWidget *widget = selectedProcessList.First();
	while (widget)
	{
		widget->SetProcessWidgetState(widget->GetProcessWidgetState() & ~kProcessWidgetTempSelected);
		widget = widget->Next();
	}
}

void ShaderEditor::BeginSection(const Point3D& p)
{
	UnselectAll();

	anchorPosition = AlignPositionToGrid(ViewportToGraphPosition(p));
	previousPosition = anchorPosition;

	SectionProcess *section = new SectionProcess;
	shaderGraph.AddElement(section);

	ShaderSectionWidget *widget = new ShaderSectionWidget(this, section);
	widget->SetViewportScale(shaderViewport->GetOrthoScale().x);
	sectionRoot->AppendNewSubnode(widget);
	sectionWidgetList.Append(widget);
	sectionTrackWidget = widget;

	section->SetProcessPosition(Point2D(anchorPosition.x, anchorPosition.y));
	widget->SetWidgetPosition(anchorPosition);

	SelectSection(widget);
	AddOperation(new CreateShaderOperation(widget));

	toolTracking = true;
}

void ShaderEditor::TrackSection(const Point3D& p)
{
	AutoScroll(p);

	Point3D position = AlignPositionToGrid(ViewportToGraphPosition(p));
	if (position != previousPosition)
	{
		float width = Fmax(position.x - anchorPosition.x, kMinSectionSize);
		float height = Fmax(position.y - anchorPosition.y, kMinSectionSize);

		SectionProcess *section = sectionTrackWidget->GetSectionProcess();
		section->SetSectionSize(width, height);

		sectionTrackWidget->SetWidgetSize(Vector2D(width, height));
		sectionTrackWidget->Invalidate();

		previousPosition = position;
	}
}

void ShaderEditor::EndSection(const Point3D& p)
{
	TrackSection(p);
	toolButton[kShaderToolProcessMove]->SetValue(1, true);
}

void ShaderEditor::TrackRoute(const Point3D& p)
{
	AutoScroll(p);

	Point3D position = ViewportToGraphPosition(p);
	if (position != previousPosition)
	{
		int32	port;

		previousPosition = position;

		routeVertex[2].position = position.GetPoint2D();
		routeVertex[3].position = position.GetPoint2D();

		Vector3D tangent = Normalize(position - anchorPosition);
		routeVertex[0].tangent.Set(tangent, -3.0F);
		routeVertex[1].tangent.Set(tangent, 4.0F);
		routeVertex[2].tangent.Set(tangent, 4.0F);
		routeVertex[3].tangent.Set(tangent, -3.0F);

		routeVertexBuffer.UpdateBuffer(0, sizeof(RouteVertex) * 4, routeVertex);

		if (routeFinishProcess)
		{
			routeFinishProcess->HilitePort(-1);
			routeFinishProcess = nullptr;
		}

		ProcessWidget *widget = FindProcessPort(position, &port);
		if ((widget) && (widget != routeStartProcess))
		{
			const Process *start = routeStartProcess->GetShaderProcess();
			const Process *finish = widget->GetShaderProcess();

			const Route *prevRoute = finish->GetFirstIncomingEdge();
			while (prevRoute)
			{
				if ((prevRoute->GetRoutePort() == port) && (prevRoute->GetStartElement() == start))
				{
					return;
				}

				prevRoute = prevRoute->GetNextIncomingEdge();
			}

			if (!shaderGraph.Predecessor(finish, start))
			{
				widget->HilitePort(port);
				routeFinishProcess = widget;
				routePort = port;
			}
		}
	}
}

void ShaderEditor::EndRoute(const Point3D& p)
{
	routeStartProcess->UpdateOutputColor(false);

	if (routeFinishProcess)
	{
		routeFinishProcess->HilitePort(-1);

		Process *start = routeStartProcess->GetShaderProcess();
		Process *finish = routeFinishProcess->GetShaderProcess();

		bool coupledFlag = false;
		const Route *prevRoute = finish->GetFirstIncomingEdge();
		while (prevRoute)
		{
			if (prevRoute->GetRoutePort() == routePort)
			{
				if (prevRoute->GetStartElement() == start)
				{
					return;
				}

				RouteWidget *widget = GetRouteWidget(prevRoute);
				if (widget)
				{
					List<RouteWidget>	routeList;

					DeleteRoute(widget, &routeList);
					AddOperation(new DeleteShaderOperation(nullptr, &routeList, nullptr));
					coupledFlag = true;
					break;
				}
			}

			prevRoute = prevRoute->GetNextIncomingEdge();
		}

		Route *route = new Route(start, finish, routePort);

		RouteWidget *widget = new RouteWidget(this, route);
		widget->SetViewportScale(shaderViewport->GetOrthoScale().x);
		graphRoot->PrependSubnode(widget);
		routeWidgetList.Append(widget);
		widget->Preprocess();

		UnselectAll();
		SelectRoute(widget);

		ShaderOperation *operation = new CreateShaderOperation(widget);
		operation->SetCoupledFlag(coupledFlag);
		AddOperation(operation);

		editorState |= kShaderEditorUpdateGraph;
	}
}

void ShaderEditor::CreateProcess(const Point3D& p)
{
	Point3D position = AlignPositionToGrid(ViewportToGraphPosition(p));

	if (!ProcessBoxIntersectsAnyProcessWidget(position.x, position.y))
	{
		UnselectAll();

		Process *process = Process::New(currentProcessReg->GetProcessType());
		shaderGraph.AddElement(process);

		ProcessWidget *widget = new ProcessWidget(this, process, currentProcessReg);
		widget->SetViewportScale(shaderViewport->GetOrthoScale().x);
		graphRoot->AppendNewSubnode(widget);
		processWidgetList.Append(widget);

		process->SetProcessPosition(Point2D(position.x, position.y));
		widget->SetWidgetPosition(position);

		SelectProcess(widget);
		AddOperation(new CreateShaderOperation(widget));

		toolButton[kShaderToolProcessMove]->SetValue(1, true);
		editorState |= kShaderEditorUpdateGraph;
	}
}

void ShaderEditor::ViewportHandleMouseEvent(const MouseEventData *eventData, ViewportWidget *viewport, void *cookie)
{
	ShaderEditor *shaderEditor = static_cast<ShaderEditor *>(cookie);

	EventType eventType = eventData->eventType;
	if ((eventType == kEventMouseDown) || (eventType == kEventMiddleMouseDown))
	{
		int32 mode = shaderEditor->currentMode;
		int32 tool = shaderEditor->currentTool;

		if ((eventType == kEventMiddleMouseDown) || (InterfaceMgr::GetOptionKey()))
		{
			mode = kShaderEditorModeTool;
			tool = kShaderToolViewportScroll;
		}

		shaderEditor->trackingMode = mode;
		shaderEditor->trackingTool = tool;
		shaderEditor->toolTracking = false;

		if (mode == kShaderEditorModeTool)
		{
			shaderEditor->BeginTool(eventData->mousePosition, eventData->eventFlags);
		}
		else if (mode == kShaderEditorModeProcess)
		{
			shaderEditor->CreateProcess(eventData->mousePosition);
		}
		else if (mode == kShaderEditorModeSection)
		{
			shaderEditor->BeginSection(eventData->mousePosition);
		}
	}
	else if ((eventType == kEventMouseUp) || (eventType == kEventMiddleMouseUp))
	{
		if (shaderEditor->toolTracking)
		{
			int32 mode = shaderEditor->trackingMode;
			if (mode == kShaderEditorModeTool)
			{
				shaderEditor->EndTool(eventData->mousePosition);
			}
			else if (mode == kShaderEditorModeSection)
			{
				shaderEditor->EndSection(eventData->mousePosition);
			}
			else if (mode == kShaderEditorModeRoute)
			{
				shaderEditor->EndRoute(eventData->mousePosition);
			}

			shaderEditor->toolTracking = false;
		}
	}
	else if (eventType == kEventMouseWheel)
	{
		if (!shaderEditor->toolTracking)
		{
			OrthoViewportWidget *orthoViewport = static_cast<OrthoViewportWidget *>(viewport);
			shaderEditor->UpdateViewportScale(orthoViewport->GetOrthoScale().x * Exp(eventData->wheelDelta.y * -0.16F), &eventData->mousePosition);
		}
	}
}

void ShaderEditor::ViewportTrackTask(const Point3D& position, ViewportWidget *viewport, void *cookie)
{
	ShaderEditor *shaderEditor = static_cast<ShaderEditor *>(cookie);
	if (shaderEditor->toolTracking)
	{
		int32 mode = shaderEditor->trackingMode;
		if (mode == kShaderEditorModeTool)
		{
			shaderEditor->TrackTool(position);
		}
		else if (mode == kShaderEditorModeSection)
		{
			shaderEditor->TrackSection(position);
		}
		else if (mode == kShaderEditorModeRoute)
		{
			shaderEditor->TrackRoute(position);
		}
	}
}

void ShaderEditor::ViewportRender(List<Renderable> *renderList, ViewportWidget *viewport, void *cookie)
{
	ShaderEditor *shaderEditor = static_cast<ShaderEditor *>(cookie);

	unsigned_int32 state = shaderEditor->editorState;
	if (state & kShaderEditorUpdateGrid)
	{
		shaderEditor->editorState = state & ~kShaderEditorUpdateGrid;

		const OrthoViewportWidget *orthoViewport = shaderEditor->shaderViewport;
		const OrthoCamera *camera = orthoViewport->GetViewportCamera();
		const OrthoCameraObject *cameraObject = camera->GetObject();

		const Point3D& position = camera->GetNodePosition();
		float xmin = cameraObject->GetOrthoRectLeft() + position.x;
		float xmax = cameraObject->GetOrthoRectRight() + position.x;
		float ymin = cameraObject->GetOrthoRectTop() + position.y;
		float ymax = cameraObject->GetOrthoRectBottom() + position.y;

		shaderEditor->viewportGrid.Build(Point2D(xmin, ymin), Point2D(xmax, ymax), orthoViewport->GetOrthoScale().x);
	}

	renderList->Append(&shaderEditor->viewportGrid);

	Widget *widget = shaderEditor->sectionRoot;
	widget->Update();
	widget->RenderTree(renderList);

	widget = shaderEditor->graphRoot;
	widget->Update();
	widget->RenderTree(renderList);

	if (shaderEditor->boxSelectFlag)
	{
		renderList->Append(&shaderEditor->dragRect);
	}
	else if ((shaderEditor->toolTracking) && (shaderEditor->trackingMode == kShaderEditorModeRoute))
	{
		renderList->Append(&shaderEditor->routeRenderable);
	}
}

void ShaderEditor::HandleToolButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		SetFocusWidget(nullptr);

		for (machine a = 0; a < kShaderToolCount; a++)
		{
			if (widget == toolButton[a])
			{
				if ((currentMode != kShaderEditorModeTool) || (currentTool != a))
				{
					UnselectCurrentTool();

					currentMode = kShaderEditorModeTool;
					currentTool = a;
				}

				break;
			}
		}
	}
}

void ShaderEditor::CloseDialogComplete(Dialog *dialog, void *cookie)
{
	ShaderEditor *shaderEditor = static_cast<ShaderEditor *>(cookie);

	int32 status = dialog->GetDialogStatus();
	if (status == kDialogOkay)
	{
		shaderEditor->HandleSaveShaderMenuItem(nullptr, nullptr);
		shaderEditor->CallCompletionProc();
		shaderEditor->Window::Close();
	}
	else if (status == kDialogIgnore)
	{
		shaderEditor->CallCompletionProc();
		shaderEditor->Window::Close();
	}
}

void ShaderEditor::Move(void)
{
	if ((TheInterfaceMgr->GetActiveWindow() == this) && (!TheInterfaceMgr->GetActiveMenu()))
	{
		const Point3D& position = TheInterfaceMgr->GetCursorPosition();

		bool toolCursor = toolTracking;
		if (!toolCursor)
		{
			Vector2D vp = position.GetVector2D() - shaderViewport->GetWorldPosition().GetVector2D();
			if ((vp.x >= 0.0F) && (vp.x < shaderViewport->GetWidgetSize().x) && (vp.y >= 0.0F) && (vp.y < shaderViewport->GetWidgetSize().y))
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
					mode = kShaderEditorModeTool;
					tool = kShaderToolViewportScroll;
				}
			}
			else
			{
				if ((trackingMode != kShaderEditorModeSection) || (currentMode == kShaderEditorModeSection))
				{
					mode = trackingMode;
					tool = trackingTool;
				}
			}

			if (mode == kShaderEditorModeTool)
			{
				if (tool == kShaderToolViewportScroll)
				{
					cursorIndex = (toolTracking) ? kEditorCursorDrag : kEditorCursorHand;
				}
				else if (tool == kShaderToolViewportZoom)
				{
					cursorIndex = kEditorCursorGlass;
				}
			}
			else
			{
				cursorIndex = kEditorCursorCross;
				if ((!toolTracking) && (mode == kShaderEditorModeProcess))
				{
					Point3D p = AlignPositionToGrid(ViewportToGraphPosition(position - shaderViewport->GetWorldPosition().GetVector3D()));
					if (ProcessBoxIntersectsAnyProcessWidget(p.x, p.y))
					{
						cursorIndex = kEditorCursorStop;
					}
				}
			}
		}

		TheInterfaceMgr->SetCursor(TheWorldEditor->GetEditorCursor(cursorIndex));
	}

	if (editorState & kShaderEditorUpdateMenus)
	{
		if (operationList.Empty())
		{
			shaderMenuItem[kShaderMenuUndo]->Disable();
		}
		else
		{
			shaderMenuItem[kShaderMenuUndo]->Enable();
		}

		bool processSelection = !selectedProcessList.Empty();
		bool routeSelection = !selectedRouteList.Empty();
		bool sectionSelection = !selectedSectionList.Empty();

		MenuItemWidget *getInfoItem = shaderMenuItem[kShaderMenuGetInfo];
		getInfoItem->Disable();

		MenuItemWidget *toggleDetailLevelItem = shaderMenuItem[kShaderMenuToggleDetailLevel];
		toggleDetailLevelItem->Disable();

		if ((processSelection) || (sectionSelection))
		{
			shaderMenuItem[kShaderMenuCut]->Enable();
			shaderMenuItem[kShaderMenuCopy]->Enable();
			shaderMenuItem[kShaderMenuDuplicate]->Enable();

			if (processSelection)
			{
				if ((!(routeSelection | sectionSelection)) && (selectedProcessList.First() == selectedProcessList.Last()))
				{
					getInfoItem->Enable();
				}
			}
			else
			{
				if ((!routeSelection) && (selectedSectionList.First() == selectedSectionList.Last()))
				{
					getInfoItem->Enable();
				}
			}
		}
		else
		{
			shaderMenuItem[kShaderMenuCut]->Disable();
			shaderMenuItem[kShaderMenuCopy]->Disable();
			shaderMenuItem[kShaderMenuDuplicate]->Disable();

			if (routeSelection)
			{
				if (selectedRouteList.First() == selectedRouteList.Last())
				{
					getInfoItem->Enable();
				}

				const RouteWidget *routeWidget = selectedRouteList.First();
				while (routeWidget)
				{
					const Route *route = routeWidget->GetShaderRoute();
					const Process *process = route->GetFinishElement();
					if (process->GetPortFlags(route->GetRoutePort()) & (kProcessPortOptional | kProcessPortOmissible))
					{
						toggleDetailLevelItem->Enable();
						break;
					}

					routeWidget = routeWidget->Next();
				}
			}
		}

		MenuItemWidget *menuItem = shaderMenuItem[kShaderMenuClear];
		if (processSelection | routeSelection | sectionSelection)
		{
			menuItem->Enable();
		}
		else
		{
			menuItem->Disable();
		}
	}

	if (editorState & kShaderEditorUpdateGraph)
	{
		UpdateShaderGraph();
	}

	editorState &= ~(kShaderEditorUpdateMenus | kShaderEditorUpdateGraph);

	graphRoot->Move();
	sectionRoot->Move();
	Window::Move();
}

void ShaderEditor::EnterForeground(void)
{
	Window::EnterForeground();

	if (!editorClipboard.Empty())
	{
		shaderMenuItem[kShaderMenuPaste]->Enable();
	}
}

void ShaderEditor::EnterBackground(void)
{
	Window::EnterBackground();
	TheInterfaceMgr->SetCursor(nullptr);
}

bool ShaderEditor::HandleKeyboardEvent(const KeyboardEventData *eventData)
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
				toolButton[kShaderToolProcessMove]->SetValue(1, true);
			}
			else if (keyCode == '6')
			{
				toolButton[kShaderToolViewportScroll]->SetValue(1, true);
			}
			else if (keyCode == '7')
			{
				toolButton[kShaderToolViewportZoom]->SetValue(1, true);
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
	}

	return (Window::HandleKeyboardEvent(eventData));
}

void ShaderEditor::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		if (widget == sectionButton)
		{
			SetFocusWidget(nullptr);

			if (currentMode != kShaderEditorModeSection)
			{
				UnselectCurrentTool();

				currentMode = kShaderEditorModeSection;
				currentTool = 0;
			}
		}
	}
}

void ShaderEditor::Close(void)
{
	if (editorState & kShaderEditorUnsaved)
	{
		const StringTable *table = TheWorldEditor->GetStringTable();

		const char *title = table->GetString(StringID('SHDR', 'SAVE'));
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

		TextWidget *text = new TextWidget(Vector2D(242.0F, 0.0F), table->GetString(StringID('SHDR', 'CFRM')), "font/Gui");
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
