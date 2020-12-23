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


#include "C4PanelEditor.h"
#include "C4ScriptEditor.h"
#include "C4WorldEditor.h"
#include "C4Dialog.h"
#include "C4Movies.h"


using namespace C4;


namespace
{
	enum
	{
		kMaxPanelOperationCount = 20
	};


	const bool widgetMenuItemSelectFlag[kPanelMenuItemCount] =
	{
		false,		// kPanelMenuUndo
		true,		// kPanelMenuCut
		true,		// kPanelMenuCopy
		false,		// kPanelMenuPaste
		true,		// kPanelMenuClear
		true,		// kPanelMenuDuplicate
		true,		// kPanelMenuBringToFront
		true,		// kPanelMenuBringForward
		true,		// kPanelMenuSendBackward
		true,		// kPanelMenuSendToBack
		true,		// kPanelMenuHideSelection
		true,		// kPanelMenuLockSelection
		true,		// kPanelMenuGroupSelection
		true,		// kPanelMenuUngroupSelection
		true,		// kPanelMenuResetRotation
		true,		// kPanelMenuResetTexcoords
		true,		// kPanelMenuAutoScaleTexture
		true,		// kPanelMenuEditScript
		true,		// kPanelMenuDeleteScript
		true,		// kPanelMenuNudgeLeft
		true,		// kPanelMenuNudgeRight
		true,		// kPanelMenuNudgeUp
		true,		// kPanelMenuNudgeDown
		true,		// kPanelMenuAlignLeftSides
		true,		// kPanelMenuAlignRightSides
		true,		// kPanelMenuAlignTopSides
		true,		// kPanelMenuAlignBottomSides
		true,		// kPanelMenuAlignHorizontalCenters
		true		// kPanelMenuAlignVerticalCenters
	};


	const bool widgetMenuItemImageFlag[kPanelMenuItemCount] =
	{
		false,		// kPanelMenuUndo
		false,		// kPanelMenuCut
		false,		// kPanelMenuCopy
		false,		// kPanelMenuPaste
		false,		// kPanelMenuClear
		false,		// kPanelMenuDuplicate
		false,		// kPanelMenuBringToFront
		false,		// kPanelMenuBringForward
		false,		// kPanelMenuSendBackward
		false,		// kPanelMenuSendToBack
		false,		// kPanelMenuHideSelection
		false,		// kPanelMenuLockSelection
		false,		// kPanelMenuGroupSelection
		false,		// kPanelMenuUngroupSelection
		false,		// kPanelMenuResetRotation
		true,		// kPanelMenuResetTexcoords
		true,		// kPanelMenuAutoScaleTexture
		false,		// kPanelMenuEditScript
		false,		// kPanelMenuDeleteScript
		false,		// kPanelMenuNudgeLeft
		false,		// kPanelMenuNudgeRight
		false,		// kPanelMenuNudgeUp
		false,		// kPanelMenuNudgeDown
		false,		// kPanelMenuAlignLeftSides
		false,		// kPanelMenuAlignRightSides
		false,		// kPanelMenuAlignTopSides
		false,		// kPanelMenuAlignBottomSides
		false,		// kPanelMenuAlignHorizontalCenters
		false		// kPanelMenuAlignVerticalCenters
	};


	const TextureHeader widgetTextureHeader =
	{
		kTexture2D,
		kTextureForceHighQuality,
		kTextureSemanticDiffuse,
		kTextureSemanticTransparency,
		kTextureI8,
		16, 16, 1,
		{kTextureRepeat, kTextureClamp, kTextureClamp},
		1
	};


	const unsigned_int8 widgetTextureImage[256] =
	{ 
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
		0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
}


Widget				PanelEditor::editorClipboard;
List<PanelEditor>	PanelEditor::windowList;


EditorWidgetManipulator::EditorWidgetManipulator(Widget *widget) :
		WidgetManipulator(widget),
		widgetVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		widgetColorAttribute(K::black),
		widgetTextureAttribute(&widgetTextureHeader, widgetTextureImage),
		widgetRenderable(kRenderQuads, kRenderDepthInhibit)
{
	SetManipulatorState(kWidgetManipulatorBuild);

	if (widget->GetWidgetType() != kWidgetGeneric)
	{
		widgetRenderable.SetTransformable(widget);
	}

	widgetRenderable.SetAmbientBlendState(kBlendInterpolate);

	widgetAttributeList.Append(&widgetColorAttribute);
	widgetAttributeList.Append(&widgetTextureAttribute);
	widgetRenderable.SetMaterialAttributeList(&widgetAttributeList);

	widgetRenderable.SetVertexCount(48);
	widgetRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &widgetVertexBuffer, sizeof(WidgetVertex));
	widgetRenderable.SetVertexAttributeArray(kArrayPosition, 0, 2);
	widgetRenderable.SetVertexAttributeArray(kArrayTexcoord, sizeof(Point2D), 2);
	widgetVertexBuffer.Establish(sizeof(WidgetVertex) * 48);
}

EditorWidgetManipulator::~EditorWidgetManipulator()
{
}

void EditorWidgetManipulator::Invalidate(void)
{
	SetManipulatorState(GetManipulatorState() | kWidgetManipulatorBuild);
}

Box2D EditorWidgetManipulator::GetWidgetBounds(void) const
{
	const Widget *widget = GetTargetWidget();

	if (widget->GetWidgetType() != kWidgetGeneric)
	{
		return (Box2D(Point2D(0.0F, 0.0F), Zero2D + widget->GetWidgetSize()));
	}

	const Box2D *box = widget->GetBoundingBox();
	if (box)
	{
		return (Box2D(Point2D(box->min.x - 2.0F, box->min.y - 2.0F), Point2D(box->max.x + 2.0F, box->max.y + 2.0F)));
	}

	return (Box2D(Point2D(0.0F, 0.0F), Point2D(0.0F, 0.0F)));
}

void EditorWidgetManipulator::GetHandlePositions(Point2D *handle) const
{
	Box2D box = GetWidgetBounds();
	float sx = box.max.x - 1.0F;
	float sy = box.max.y - 1.0F;

	handle[0].Set(box.min.x, box.min.y);
	handle[1].Set(box.min.x, sy);
	handle[2].Set(sx, sy);
	handle[3].Set(sx, box.min.y);

	float hx = Floor((box.min.x + sx) * 0.5F);
	float hy = Floor((box.min.y + sy) * 0.5F);

	handle[4].Set(box.min.x, hy);
	handle[5].Set(hx, sy);
	handle[6].Set(sx, hy);
	handle[7].Set(hx, box.min.y);
}

void EditorWidgetManipulator::BuildHandle(volatile WidgetVertex *vertex, float x, float y, float scale)
{
	float f1 = scale * -3.0F;
	float f2 = scale * 4.0F;

	vertex[0].position.Set(x + f1, y + f1);
	vertex[0].texcoord.Set(0.0F, 0.5F);

	vertex[1].position.Set(x + f1, y + f2);
	vertex[1].texcoord.Set(0.0F, 0.9375F);

	vertex[2].position.Set(x + f2, y + f2);
	vertex[2].texcoord.Set(0.4375F, 0.9375F);

	vertex[3].position.Set(x + f2, y + f1);
	vertex[3].texcoord.Set(0.4375F, 0.5F);
}

void EditorWidgetManipulator::Build(float scale)
{
	Point2D		handle[8];

	Widget *widget = GetTargetWidget();

	if (Hidden())
	{
		widget->SetEditorState(widget->GetEditorState() | kWidgetHidden);
	}
	else
	{
		widget->SetEditorState(widget->GetEditorState() & ~kWidgetHidden);
	}

	volatile WidgetVertex *restrict widgetVertex = widgetVertexBuffer.BeginUpdate<WidgetVertex>();

	Box2D box = GetWidgetBounds();
	float sx = box.max.x - 1.0F;
	float sy = box.max.y - 1.0F;
	float tx = sx - box.min.x;
	float ty = sy - box.min.y;
	float f1 = -scale;
	float f2 = scale * 2.0F;
	float dt = (widget->GetWidgetType() != kWidgetGeneric) ? 0.0F : 0.25F;

	widgetVertex[0].position.Set(box.min.x, box.min.y + f1);
	widgetVertex[0].texcoord.Set(0.0F, dt);
	widgetVertex[1].position.Set(box.min.x, box.min.y + f2);
	widgetVertex[1].texcoord.Set(0.0F, dt + 0.1875F);
	widgetVertex[2].position.Set(sx, box.min.y + f2);
	widgetVertex[2].texcoord.Set(tx * 0.0625F, dt + 0.1875F);
	widgetVertex[3].position.Set(sx, box.min.y + f1);
	widgetVertex[3].texcoord.Set(tx * 0.0625F, dt);

	widgetVertex[4].position.Set(box.min.x, sy + f1);
	widgetVertex[4].texcoord.Set(0.0F, dt);
	widgetVertex[5].position.Set(box.min.x, sy + f2);
	widgetVertex[5].texcoord.Set(0.0F, dt + 0.1875F);
	widgetVertex[6].position.Set(sx, sy + f2);
	widgetVertex[6].texcoord.Set(tx * 0.0625F, dt + 0.1875F);
	widgetVertex[7].position.Set(sx, sy + f1);
	widgetVertex[7].texcoord.Set(tx * 0.0625F, dt);

	widgetVertex[8].position.Set(box.min.x + f1, box.min.y);
	widgetVertex[8].texcoord.Set(0.0F, dt);
	widgetVertex[9].position.Set(box.min.x + f1, sy);
	widgetVertex[9].texcoord.Set(ty * 0.0625F, dt);
	widgetVertex[10].position.Set(box.min.x + f2, sy);
	widgetVertex[10].texcoord.Set(ty * 0.0625F, dt + 0.1875F);
	widgetVertex[11].position.Set(box.min.x + f2, box.min.y);
	widgetVertex[11].texcoord.Set(0.0F, dt + 0.1875F);

	widgetVertex[12].position.Set(sx + f1, box.min.y);
	widgetVertex[12].texcoord.Set(0.0F, dt);
	widgetVertex[13].position.Set(sx + f1, sy);
	widgetVertex[13].texcoord.Set(ty * 0.0625F, dt);
	widgetVertex[14].position.Set(sx + f2, sy);
	widgetVertex[14].texcoord.Set(ty * 0.0625F, dt + 0.1875F);
	widgetVertex[15].position.Set(sx + f2, box.min.y);
	widgetVertex[15].texcoord.Set(0.0F, dt + 0.1875F);

	GetHandlePositions(handle);

	volatile WidgetVertex *restrict vertex = widgetVertex + 16;
	for (machine a = 0; a < 8; a++)
	{
		BuildHandle(vertex, handle[a].x, handle[a].y, scale);
		vertex += 4;
	}

	widgetVertexBuffer.EndUpdate();
}

void EditorWidgetManipulator::Install(Widget *root, bool recursive)
{
	if (!(root->GetWidgetState() & kWidgetNonpersistent))
	{
		WidgetManipulator *manipulator = root->GetManipulator();
		if (!manipulator)
		{
			root->SetManipulator(new EditorWidgetManipulator(root));
		}

		if (recursive)
		{
			Widget *widget = root->GetFirstSubnode();
			while (widget)
			{
				Install(widget);
				widget = widget->Next();
			}
		}
	}
}


PanelOperation::PanelOperation()
{
}

PanelOperation::~PanelOperation()
{
}


CreatePanelOperation::CreatePanelOperation(Widget *widget)
{
	createList.Append(new WidgetReference(widget));
}

CreatePanelOperation::CreatePanelOperation(const List<EditorWidgetManipulator> *selectionList)
{
	const EditorWidgetManipulator *manipulator = selectionList->First();
	while (manipulator)
	{
		createList.Append(new WidgetReference(manipulator->GetTargetWidget()));
		manipulator = manipulator->Next();
	}
}

CreatePanelOperation::~CreatePanelOperation()
{
}

void CreatePanelOperation::Restore(PanelEditor *panelEditor)
{
	const WidgetReference *reference = createList.First();
	while (reference)
	{
		panelEditor->DeleteWidget(reference->GetWidget());
		reference = reference->Next();
	}
}


DeletePanelOperation::DeletePanelOperation(Widget *rootWidget)
{
	Widget *widget = rootWidget->GetFirstSubnode();
	while (widget)
	{
		WidgetManipulator *manipulator = widget->GetManipulator();
		if ((manipulator) && (manipulator->Selected()))
		{
			deleteList.Append(new DeleteReference(widget));
		}

		widget = rootWidget->GetNextNode(widget);
	}
}

DeletePanelOperation::~DeletePanelOperation()
{
	const WidgetReference *reference = deleteList.First();
	while (reference)
	{
		Widget *widget = reference->GetWidget();
		delete widget;

		reference = reference->Next();
	}
}

DeletePanelOperation::DeleteReference::DeleteReference(Widget *widget) : WidgetReference(widget)
{
	superWidget = widget->GetSuperNode();
	precedingWidget = widget->Previous();
}

void DeletePanelOperation::Restore(PanelEditor *panelEditor)
{
	const DeleteReference *reference = static_cast<const DeleteReference *>(deleteList.First());
	while (reference)
	{
		reference->GetSuperWidget()->InsertSubnodeAfter(reference->GetWidget(), reference->GetPrecedingWidget());
		reference = static_cast<const DeleteReference *>(reference->Next());
	}

	deleteList.Purge();
	panelEditor->GetRootWidget()->Preprocess();
}


TransformPanelOperation::TransformPanelOperation(const List<EditorWidgetManipulator> *selectionList)
{
	const EditorWidgetManipulator *manipulator = selectionList->First();
	while (manipulator)
	{
		transformList.Append(new TransformReference(manipulator->GetTargetWidget()));
		manipulator = manipulator->Next();
	}
}

TransformPanelOperation::~TransformPanelOperation()
{
}

TransformPanelOperation::TransformReference::TransformReference(Widget *widget) : WidgetReference(widget)
{
	widgetTransform = widget->GetWidgetTransform();
	widgetSize = widget->GetWidgetSize();
}

void TransformPanelOperation::Restore(PanelEditor *panelEditor)
{
	const WidgetReference *reference = transformList.First();
	while (reference)
	{
		const TransformReference *transformed = static_cast<const TransformReference *>(reference);

		Widget *widget = transformed->GetWidget();
		widget->SetWidgetTransform(transformed->GetTransform());
		widget->SetWidgetSize(transformed->GetSize());
		widget->Invalidate();

		reference = reference->Next();
	}

	panelEditor->InvalidateTransform();
}


WarpPanelOperation::WarpPanelOperation(const List<EditorWidgetManipulator> *selectionList)
{
	const EditorWidgetManipulator *manipulator = selectionList->First();
	while (manipulator)
	{
		Widget *widget = manipulator->GetTargetWidget();

		WidgetType type = widget->GetWidgetType();
		if (type == kWidgetImage)
		{
			warpList.Append(new WarpReference(static_cast<ImageWidget *>(widget)));
		}
		else if (type == kWidgetText)
		{
			scaleList.Append(new ScaleReference(static_cast<TextWidget *>(widget)));
		}

		manipulator = manipulator->Next();
	}
}

WarpPanelOperation::~WarpPanelOperation()
{
}

WarpPanelOperation::WarpReference::WarpReference(ImageWidget *widget) : WidgetReference(widget)
{
	imageOffset = widget->GetImageOffset();
	imageScale = widget->GetImageScale();
}

WarpPanelOperation::ScaleReference::ScaleReference(TextWidget *widget) : WidgetReference(widget)
{
	textScale = widget->GetTextScale();
}

void WarpPanelOperation::Restore(PanelEditor *panelEditor)
{
	const WidgetReference *reference = warpList.First();
	while (reference)
	{
		const WarpReference *warpReference = static_cast<const WarpReference *>(reference);

		ImageWidget *widget = static_cast<ImageWidget *>(warpReference->GetWidget());
		widget->SetImageOffset(warpReference->GetOffset());
		widget->SetImageScale(warpReference->GetScale());

		reference = reference->Next();
	}

	reference = scaleList.First();
	while (reference)
	{
		const ScaleReference *scaleReference = static_cast<const ScaleReference *>(reference);

		TextWidget *widget = static_cast<TextWidget *>(scaleReference->GetWidget());
		widget->SetTextScale(scaleReference->GetScale());

		reference = reference->Next();
	}
}


ArrangePanelOperation::ArrangePanelOperation(Widget *rootWidget)
{
	Widget *widget = rootWidget->GetFirstSubnode();
	while (widget)
	{
		WidgetManipulator *manipulator = widget->GetManipulator();
		if ((manipulator) && (manipulator->Selected()))
		{
			arrangeList.Append(new ArrangeReference(widget));
		}

		widget = rootWidget->GetNextNode(widget);
	}
}

ArrangePanelOperation::~ArrangePanelOperation()
{
}

ArrangePanelOperation::ArrangeReference::ArrangeReference(Widget *widget) : WidgetReference(widget)
{
	precedingWidget = widget->Previous();
}

void ArrangePanelOperation::Restore(PanelEditor *panelEditor)
{
	const WidgetReference *reference = arrangeList.First();
	while (reference)
	{
		Widget *widget = reference->GetWidget();
		widget->GetSuperNode()->InsertSubnodeAfter(widget, static_cast<const ArrangeReference *>(reference)->GetPrecedingWidget());

		reference = reference->Next();
	}

	panelEditor->GetRootWidget()->Preprocess();
}


GroupPanelOperation::GroupPanelOperation(Widget *group, const Widget *rootWidget)
{
	groupWidget = group;

	Widget *widget = rootWidget->GetFirstSubnode();
	while (widget)
	{
		if (widget->GetManipulator()->Selected())
		{
			groupList.Append(new GroupReference(widget));
		}

		widget = widget->Next();
	}
}

GroupPanelOperation::~GroupPanelOperation()
{
}

GroupPanelOperation::GroupReference::GroupReference(Widget *widget) : WidgetReference(widget)
{
	precedingWidget = widget->Previous();
	widgetTransform = widget->GetWidgetTransform();
}

void GroupPanelOperation::Restore(PanelEditor *panelEditor)
{
	RootWidget *rootWidget = panelEditor->GetRootWidget();

	const WidgetReference *reference = groupList.First();
	while (reference)
	{
		const GroupReference *groupReference = static_cast<const GroupReference *>(reference);

		Widget *widget = groupReference->GetWidget();
		rootWidget->InsertSubnodeAfter(widget, groupReference->GetPrecedingWidget());
		widget->SetWidgetTransform(groupReference->GetTransform());

		reference = reference->Next();
	}

	panelEditor->DeleteWidget(groupWidget);
	rootWidget->Preprocess();
}


UngroupPanelOperation::UngroupPanelOperation(const List<EditorWidgetManipulator> *selectionList)
{
	const EditorWidgetManipulator *manipulator = selectionList->First();
	while (manipulator)
	{
		ungroupList.Append(new UngroupReference(manipulator->GetTargetWidget()));
		manipulator = manipulator->Next();
	}
}

UngroupPanelOperation::~UngroupPanelOperation()
{
	const WidgetReference *ungroupWidget = ungroupList.First();
	while (ungroupWidget)
	{
		delete ungroupWidget->GetWidget();
		ungroupWidget = ungroupWidget->Next();
	}
}

UngroupPanelOperation::TransformReference::TransformReference(Widget *widget) : WidgetReference(widget)
{
	widgetTransform = widget->GetWidgetTransform();
}

UngroupPanelOperation::UngroupReference::UngroupReference(Widget *widget) : WidgetReference(widget)
{
	precedingWidget = widget->Previous();

	Widget *subnode = widget->GetFirstSubnode();
	while (subnode)
	{
		subnodeList.Append(new TransformReference(subnode));
		subnode = subnode->Next();
	}
}

void UngroupPanelOperation::Restore(PanelEditor *panelEditor)
{
	RootWidget *rootWidget = panelEditor->GetRootWidget();

	const WidgetReference *reference = ungroupList.First();
	while (reference)
	{
		const UngroupReference *ungroupReference = static_cast<const UngroupReference *>(reference);

		Widget *group = ungroupReference->GetWidget();
		rootWidget->InsertSubnodeAfter(group, ungroupReference->GetPrecedingWidget());

		const WidgetReference *subnodeReference = ungroupReference->GetFirstSubnodeReference();
		while (subnodeReference)
		{
			Widget *widget = subnodeReference->GetWidget();
			widget->SetWidgetTransform(static_cast<const TransformReference *>(subnodeReference)->GetTransform());
			group->AppendSubnode(widget);

			subnodeReference = subnodeReference->Next();
		}

		reference = reference->Next();
	}

	rootWidget->Preprocess();
}


TexturePanelOperation::TexturePanelOperation(const List<EditorWidgetManipulator> *selectionList)
{
	const EditorWidgetManipulator *manipulator = selectionList->First();
	while (manipulator)
	{
		Widget *widget = manipulator->GetTargetWidget();
		if (widget->GetWidgetType() == kWidgetImage)
		{
			textureList.Append(new TextureReference(static_cast<ImageWidget *>(widget)));
		}

		manipulator = manipulator->Next();
	}
}

TexturePanelOperation::~TexturePanelOperation()
{
}

TexturePanelOperation::TextureReference::TextureReference(ImageWidget *widget) : WidgetReference(widget)
{
	textureName = widget->GetTextureName();
}

void TexturePanelOperation::Restore(PanelEditor *panelEditor)
{
	const WidgetReference *reference = textureList.First();
	while (reference)
	{
		const TextureReference *textureReference = static_cast<const TextureReference *>(reference);

		ImageWidget *widget = static_cast<ImageWidget *>(textureReference->GetWidget());
		widget->SetTexture(0, textureReference->GetTextureName());

		reference = reference->Next();
	}
}


ScriptPanelOperation::ScriptPanelOperation(const List<EditorWidgetManipulator> *selectionList)
{
	const EditorWidgetManipulator *manipulator = selectionList->First();
	while (manipulator)
	{
		Widget *widget = manipulator->GetTargetWidget();
		if (widget->GetScriptObject())
		{
			scriptList.Append(new ScriptReference(widget));
		}

		manipulator = manipulator->Next();
	}
}

ScriptPanelOperation::~ScriptPanelOperation()
{
}

ScriptPanelOperation::ScriptReference::ScriptReference(Widget *widget) : WidgetReference(widget)
{
	scriptObject = widget->GetScriptObject();
	scriptObject->Retain();
}

ScriptPanelOperation::ScriptReference::~ScriptReference()
{
	scriptObject->Release();
}

void ScriptPanelOperation::Restore(PanelEditor *panelEditor)
{
	const WidgetReference *reference = scriptList.First();
	while (reference)
	{
		const ScriptReference *scriptReference = static_cast<const ScriptReference *>(reference);
		scriptReference->GetWidget()->SetScriptObject(scriptReference->GetScriptObject());

		reference = reference->Next();
	}
}


PanelPage::PanelPage(PanelEditor *editor, const char *panelName) : Page(panelName)
{
	panelEditor = editor;
}

PanelPage::~PanelPage()
{
}


PanelWidgetsPage::PanelWidgetsPage(PanelEditor *editor) :
		PanelPage(editor, "PanelEditor/Widgets"),
		listWidgetObserver(this, &PanelWidgetsPage::HandleListWidgetEvent)
{
}

PanelWidgetsPage::~PanelWidgetsPage()
{
}

PanelWidgetsPage::ToolWidget::ToolWidget(const Vector2D& size, const WidgetRegistration *registration) :
		TextWidget(size, registration->GetWidgetName(), "font/Normal"),
		imageWidget(Vector2D(16.0F, 16.0F), registration->GetIconTextureName())
{
	widgetRegistration = registration;

	imageWidget.SetWidgetPosition(Point3D(-20.0F, -2.0F, 0.0F));
	AppendSubnode(&imageWidget);
}

PanelWidgetsPage::ToolWidget::~ToolWidget()
{
}

void PanelWidgetsPage::Preprocess(void)
{
	PanelPage::Preprocess();

	listWidget = static_cast<ListWidget *>(FindWidget("List"));
	listWidget->SetObserver(&listWidgetObserver);

	Vector2D size = listWidget->GetNaturalListItemSize();
	const PanelEffect *targetNode = GetPanelEditor()->GetTargetNode();

	const WidgetRegistration *registration = Widget::GetFirstRegistration();
	while (registration)
	{
		if ((targetNode) || ((registration->GetWidgetFlags() & kWidgetPanelOnly) == 0))
		{
			ToolWidget *widget = new ToolWidget(size, registration);
			listWidget->InsertSortedListItem(widget);
		}

		registration = registration->Next();
	}
}

void PanelWidgetsPage::HandleListWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		PanelEditor *editor = GetPanelEditor();
		editor->SetFocusWidget(nullptr);

		ToolWidget *toolWidget = static_cast<ToolWidget *>(static_cast<ListWidget *>(widget)->GetFirstSelectedListItem());
		editor->SelectWidgetTool(toolWidget->widgetRegistration);
	}
}

void PanelWidgetsPage::UnselectWidgetTool(void)
{
	listWidget->UnselectAllListItems();
}


PanelTransformPage::PanelTransformPage(PanelEditor *editor) :
		PanelPage(editor, "PanelEditor/Transform"),
		positionTextObserver(this, &PanelTransformPage::HandlePositionTextEvent),
		rotationTextObserver(this, &PanelTransformPage::HandleRotationTextEvent),
		sizeTextObserver(this, &PanelTransformPage::HandleSizeTextEvent)
{
	operationFlag = false;
}

PanelTransformPage::~PanelTransformPage()
{
}

void PanelTransformPage::Preprocess(void)
{
	PanelPage::Preprocess();

	xpositionText = static_cast<EditTextWidget *>(FindWidget("Xpos"));
	ypositionText = static_cast<EditTextWidget *>(FindWidget("Ypos"));
	xpositionText->SetObserver(&positionTextObserver);
	ypositionText->SetObserver(&positionTextObserver);

	rotationText = static_cast<EditTextWidget *>(FindWidget("Rot"));
	rotationText->SetObserver(&rotationTextObserver);

	xsizeText = static_cast<EditTextWidget *>(FindWidget("Xsize"));
	ysizeText = static_cast<EditTextWidget *>(FindWidget("Ysize"));
	xsizeText->SetObserver(&sizeTextObserver);
	ysizeText->SetObserver(&sizeTextObserver);
}

void PanelTransformPage::HandlePositionTextEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		PanelEditor *editor = GetPanelEditor();
		const List<EditorWidgetManipulator> *selectionList = editor->GetSelectionList();

		if (!operationFlag)
		{
			operationFlag = true;
			editor->AddOperation(new TransformPanelOperation(selectionList));
		}

		WidgetManipulator *manipulator = selectionList->First();
		Widget *targetWidget = manipulator->GetTargetWidget();

		float x = Text::StringToFloat(xpositionText->GetText());
		float y = Text::StringToFloat(ypositionText->GetText());

		targetWidget->SetWidgetPosition(Point3D(x, y, 0.0F));
		targetWidget->Invalidate();
	}
}

void PanelTransformPage::HandleRotationTextEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		PanelEditor *editor = GetPanelEditor();
		const List<EditorWidgetManipulator> *selectionList = editor->GetSelectionList();

		if (!operationFlag)
		{
			operationFlag = true;
			editor->AddOperation(new TransformPanelOperation(selectionList));
		}

		WidgetManipulator *manipulator = selectionList->First();
		Widget *targetWidget = manipulator->GetTargetWidget();

		float t = Text::StringToFloat(rotationText->GetText()) * K::radians;

		targetWidget->SetWidgetMatrix3D(Matrix3D().SetRotationAboutZ(t));
		targetWidget->Invalidate();
	}
}

void PanelTransformPage::HandleSizeTextEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		PanelEditor *editor = GetPanelEditor();
		const List<EditorWidgetManipulator> *selectionList = editor->GetSelectionList();

		if (!operationFlag)
		{
			operationFlag = true;
			editor->AddOperation(new TransformPanelOperation(selectionList));
		}

		WidgetManipulator *manipulator = selectionList->First();
		Widget *targetWidget = manipulator->GetTargetWidget();

		float x = Fmax(Text::StringToFloat(xsizeText->GetText()), 1.0F);
		float y = Fmax(Text::StringToFloat(ysizeText->GetText()), 1.0F);

		targetWidget->SetWidgetSize(Vector2D(x, y));
		targetWidget->Invalidate();
	}
}

String<15> PanelTransformPage::FloatToString(float num)
{
	String<15> string = Text::FloatToString(num);
	int32 len = string.Length() - 2;
	if ((len > 0) && (string[len] == '.') && (string[len + 1] == '0'))
	{
		string[len] = 0;
	}

	return (string);
}

void PanelTransformPage::UpdateTransform(Widget *widget)
{
	operationFlag = false;

	if (widget)
	{
		const Transform4D& transform = widget->GetWidgetTransform();

		xpositionText->SetText(FloatToString(transform(0,3)));
		ypositionText->SetText(FloatToString(transform(1,3)));

		float t = Acos(transform(0,0));
		if (transform(1,0) < 0.0F)
		{
			t = -t;
		}

		rotationText->SetText(FloatToString(t * K::degrees));

		const Vector2D& size = widget->GetWidgetSize();
		xsizeText->SetText(FloatToString(size.x));
		ysizeText->SetText(FloatToString(size.y));

		xpositionText->Enable();
		ypositionText->Enable();
		rotationText->Enable();
		xsizeText->Enable();
		ysizeText->Enable();
	}
	else
	{
		xpositionText->SetText(nullptr);
		ypositionText->SetText(nullptr);
		rotationText->SetText(nullptr);
		xsizeText->SetText(nullptr);
		ysizeText->SetText(nullptr);

		xpositionText->Disable();
		ypositionText->Disable();
		rotationText->Disable();
		xsizeText->Disable();
		ysizeText->Disable();
	}
}


PanelGridPage::PanelGridPage(PanelEditor *editor) :
		PanelPage(editor, "PanelEditor/Grid"),
		gridButtonObserver(this, &PanelGridPage::HandleGridButtonEvent),
		snapButtonObserver(this, &PanelGridPage::HandleSnapButtonEvent)
{
}

PanelGridPage::~PanelGridPage()
{
}

void PanelGridPage::Preprocess(void)
{
	PanelPage::Preprocess();

	gridButton = static_cast<IconButtonWidget *>(FindWidget("Grid"));
	gridButton->SetObserver(&gridButtonObserver);

	snapButton = static_cast<IconButtonWidget *>(FindWidget("Snap"));
	snapButton->SetObserver(&snapButtonObserver);
}

void PanelGridPage::HandleGridButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		PanelEditor *editor = GetPanelEditor();
		unsigned_int32 flags = editor->GetPanelEditorFlags() ^ kPanelEditorShowGridlines;
		editor->SetPanelEditorFlags(flags);

		gridButton->SetValue((flags & kPanelEditorShowGridlines) != 0);
	}
}

void PanelGridPage::HandleSnapButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		PanelEditor *editor = GetPanelEditor();
		unsigned_int32 flags = editor->GetPanelEditorFlags() ^ kPanelEditorSnapToGrid;
		editor->SetPanelEditorFlags(flags);

		snapButton->SetValue((flags & kPanelEditorSnapToGrid) != 0);
	}
}


WindowSettingsWindow::WindowSettingsWindow(PanelEditor *editor) : Window("PanelEditor/Settings")
{
	panelEditor = editor;
}

WindowSettingsWindow::~WindowSettingsWindow()
{
}

void WindowSettingsWindow::Preprocess(void)
{
	Window::Preprocess();

	okayButton = static_cast<PushButtonWidget *>(FindWidget("OK"));
	cancelButton = static_cast<PushButtonWidget *>(FindWidget("Cancel"));

	configurationWidget = static_cast<ConfigurationWidget *>(FindWidget("Config"));
	configurationWidget->BuildConfiguration(panelEditor->GetRootWidget());

	SetNextFocusWidget();
}

void WindowSettingsWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		if (widget == okayButton)
		{
			configurationWidget->CommitConfiguration(panelEditor->GetRootWidget());
			panelEditor->SetModifiedFlag();
			panelEditor->InvalidateGrid();
			Close();
		}
		else if (widget == cancelButton)
		{
			Close();
		}
	}
}


PanelEditor::PanelEditor(const PanelEffect *node, const char *name) :
		Window("PanelEditor/Window"),
		toolButtonObserver(this, &PanelEditor::HandleToolButtonEvent),
		textBoxObserver(this, &PanelEditor::HandleTextBoxEvent),
		widgetConfigurationObserver(this, &PanelEditor::HandleWidgetConfigurationEvent),
		mutatorConfigurationObserver(this, &PanelEditor::HandleMutatorConfigurationEvent),
		dragRect(ColorRGBA(0.5F, 0.5F, 0.5F, 1.0F))
{
	windowList.Append(this);

	SetMinWindowSize(Vector2D(960.0F, 688.0F));
	SetStripIcon("PanelEditor/window");

	targetNode = node;
	if (node)
	{
		resourceName[0] = 0;
		targetPanel = static_cast<PanelController *>(node->GetController())->GetRootWidget();

		rootWidget = new Panel(targetPanel->GetWidgetSize());

		const Widget *widget = targetPanel->GetFirstSubnode();
		while (widget)
		{
			Widget *clone = widget->Clone();
			EditorWidgetManipulator::Install(clone);
			rootWidget->AppendSubnode(clone);

			widget = widget->Next();
		}
	}
	else
	{
		Window *window = new Window(Vector2D(512.0F, 512.0F));
		rootWidget = window;

		resourceName[0] = 0;
		targetPanel = nullptr;

		if (name)
		{
			PanelResource *resource = PanelResource::Get(name, kResourceIgnorePackFiles, nullptr, &resourceLocation);
			if (resource)
			{
				window->UnpackTree(resource->GetPanelResourceHeader(), kUnpackEditor);
				resource->Release();

				Widget *widget = window->GetFirstSubnode();
				while (widget)
				{
					EditorWidgetManipulator::Install(widget);
					widget = widget->Next();
				}

				resourceName = name;
				ResourcePath title(name);
				SetWindowTitle(title += PanelResource::GetDescriptor()->GetExtension());
				SetStripTitle(&title[Text::GetDirectoryPathLength(title)]);
			}
		}

		window->SetWindowFlags(window->GetWindowFlags() | kWindowEditor);
	}

	rootWidget->Preprocess();
	rootWidget->Update();
}

PanelEditor::~PanelEditor()
{
	TheInterfaceMgr->SetCursor(nullptr);

	delete rootWidget;
}

PanelEditor *PanelEditor::Open(const char *name)
{
	PanelEditor *window = nullptr;

	if (name)
	{
		window = windowList.First();
		while (window)
		{
			if (window->resourceName == name)
			{
				TheInterfaceMgr->SetActiveWindow(window);
				return (window);
			}

			window = window->ListElement<PanelEditor>::Next();
		}
	}

	PanelEditor *editor = new PanelEditor(nullptr, name);
	if ((name) && (editor->GetResourceName()[0] == 0))
	{
		delete editor;
		return (nullptr);
	}

	TheInterfaceMgr->AddWidget(editor);
	return (editor);
}

void PanelEditor::SetWidgetSize(const Vector2D& size)
{
	Window::SetWidgetSize(size);
	PositionWidgets();

	editorState |= kPanelEditorUpdateGrid;
}

void PanelEditor::Preprocess(void)
{
	static const char *const toolIdentifier[kPanelToolCount] =
	{
		"Select", "Move", "Rotate", "Resize", "Offset", "Scale", "Scroll", "Zoom"
	};

	Window::Preprocess();

	panelViewport = static_cast<OrthoViewportWidget *>(FindWidget("Viewport"));
	viewportBorder = static_cast<BorderWidget *>(FindWidget("Border"));

	panelViewport->SetMouseEventProc(&ViewportHandleMouseEvent, this);
	panelViewport->SetTrackTaskProc(&ViewportTrackTask, this);
	panelViewport->SetRenderProc(&ViewportRender, this);

	OrthoCamera *camera = panelViewport->GetViewportCamera();
	const Vector2D& panelSize = rootWidget->GetWidgetSize();
	camera->SetNodePosition(Point3D(PositiveFloor(panelSize.x * 0.5F), PositiveFloor(panelSize.y * 0.5F), -0.5F));

	CameraObject *cameraObject = camera->GetObject();
	cameraObject->SetClearFlags(kClearColorBuffer | kClearDepthStencilBuffer);
	cameraObject->SetClearColor(K::white);
	cameraObject->SetNearDepth(-1.0F);
	cameraObject->SetFarDepth(1.0F);

	for (machine a = 0; a < kPanelToolCount; a++)
	{
		IconButtonWidget *widget = static_cast<IconButtonWidget *>(FindWidget(toolIdentifier[a]));
		widget->SetObserver(&toolButtonObserver);
		toolButton[a] = widget;
	}

	toolButton[kPanelToolWidgetSelect]->SetValue(1);

	currentMode = kPanelEditorModeTool;
	currentTool = kPanelToolWidgetSelect;
	currentWidgetReg = nullptr;
	toolTracking = false;
	boxSelectFlag = false;

	settingsGroup = FindWidget("Group");
	textStringText = static_cast<TextWidget *>(FindWidget("Text"));
	textStringBox = static_cast<EditTextWidget *>(FindWidget("TextBox"));
	textStringBox->SetRenderLineCount(4);
	textStringBox->SetObserver(&textBoxObserver);

	widgetConfigurationWidget = static_cast<ConfigurationWidget *>(FindWidget("WidgetConfig"));
	widgetConfigurationWidget->SetObserver(&widgetConfigurationObserver);

	mutatorConfigurationWidget = static_cast<ConfigurationWidget *>(FindWidget("MutatorConfig"));
	mutatorConfigurationWidget->SetObserver(&mutatorConfigurationObserver);

	mutatorMenu = static_cast<PopupMenuWidget *>(FindWidget("MutatorMenu"));
	mutatorListWidget = static_cast<ListWidget *>(FindWidget("MutatorList"));
	addMutatorButton = static_cast<PushButtonWidget *>(FindWidget("Add"));
	removeMutatorButton = static_cast<PushButtonWidget *>(FindWidget("Remove"));

	menuBar = static_cast<MenuBarWidget *>(FindWidget("Menu"));

	widgetsPage = new PanelWidgetsPage(this);
	transformPage = new PanelTransformPage(this);
	gridPage = new PanelGridPage(this);

	bookWidget = new BookWidget(Vector2D(163.0F, 0.0F));
	bookWidget->AppendPage(widgetsPage);
	bookWidget->AppendPage(transformPage);
	bookWidget->AppendPage(gridPage);
	bookWidget->OrganizePages();
	AppendNewSubnode(bookWidget);

	PositionWidgets();
	BuildMenus();

	editorFlags = kPanelEditorShowGridlines | kPanelEditorSnapToGrid;
	editorState = kPanelEditorUpdateMenus | kPanelEditorUpdateGrid;

	viewportGrid.SetGridFlags(Grid::kGridShowBoundingBox);
	viewportGrid.SetGridLineSpacing(8.0F);
	viewportGrid.SetMinorLineColor(ColorRGB(0.9375F, 0.9375F, 0.9375F));
	viewportGrid.SetMajorLineColor(ColorRGB(0.8125F, 0.8125F, 0.8125F));
	viewportGrid.SetAxisLineColor(ColorRGB(0.8125F, 0.8125F, 0.8125F));
	viewportGrid.SetBoundingBoxColor(ColorRGB(0.25F, 0.25F, 0.25F));
}

void PanelEditor::PositionWidgets(void)
{
	float width = GetWidgetSize().x;
	float height = GetWidgetSize().y;

	const Point3D& position = panelViewport->GetWidgetPosition();
	Vector2D viewportSize(width - position.x - 309.0F, height - position.y - 4.0F);

	panelViewport->SetWidgetSize(viewportSize);
	viewportBorder->SetWidgetSize(viewportSize);

	settingsGroup->SetWidgetPosition(Point3D(width - 303.0F, 24.0F, 0.0F));
	settingsGroup->Invalidate();

	float menuBarHeight = menuBar->GetWidgetSize().y;
	menuBar->SetWidgetSize(Vector2D(width, menuBarHeight));

	bookWidget->SetWidgetPosition(Point3D(4.0F, menuBarHeight + 4.0F, 0.0F));
	bookWidget->SetWidgetSize(Vector2D(bookWidget->GetWidgetSize().x, height - menuBarHeight - 8.0F));
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

void PanelEditor::BuildMenus(void)
{
	const StringTable *table = TheWorldEditor->GetStringTable();

	// Panel Menu

	panelMenu = new PulldownMenuWidget(table->GetString(StringID('PANL', 'MENU', 'PANL')));
	menuBar->AppendMenu(panelMenu);

	panelMenu->AppendMenuItem(new MenuItemWidget(table->GetString(StringID('PANL', 'MENU', 'PANL', 'CLOS')), WidgetObserver<PanelEditor>(this, &PanelEditor::HandleCloseMenuItem), Shortcut('W')));
	panelMenu->AppendMenuItem(new MenuItemWidget(table->GetString(StringID('PANL', 'MENU', 'PANL', 'SAVE')), WidgetObserver<PanelEditor>(this, &PanelEditor::HandleSavePanelMenuItem), Shortcut('S')));

	MenuItemWidget *widget = new MenuItemWidget(table->GetString(StringID('PANL', 'MENU', 'PANL', 'SVAS')), WidgetObserver<PanelEditor>(this, &PanelEditor::HandleSavePanelAsMenuItem), Shortcut('S', kShortcutShift));
	if (targetNode)
	{
		widget->Disable();
	}

	panelMenu->AppendMenuItem(widget);
	panelMenu->AppendMenuItem(new MenuItemWidget(kLineSolid));

	widget = new MenuItemWidget(table->GetString(StringID('PANL', 'MENU', 'PANL', 'SETT')), WidgetObserver<PanelEditor>(this, &PanelEditor::HandleWindowSettingsMenuItem), Shortcut('P'));
	if (targetNode)
	{
		widget->Disable();
	}

	panelMenu->AppendMenuItem(widget);

	// Edit Menu

	editMenu = new PulldownMenuWidget(table->GetString(StringID('PANL', 'MENU', 'EDIT')));
	menuBar->AppendMenu(editMenu);

	widget = new MenuItemWidget(table->GetString(StringID('PANL', 'MENU', 'EDIT', 'UNDO')), WidgetObserver<PanelEditor>(this, &PanelEditor::HandleUndoMenuItem), Shortcut('Z'));
	widgetMenuItem[kPanelMenuUndo] = widget;
	widget->Disable();
	editMenu->AppendMenuItem(widget);

	editMenu->AppendMenuItem(new MenuItemWidget(kLineSolid));

	widget = new MenuItemWidget(table->GetString(StringID('PANL', 'MENU', 'EDIT', 'CUT ')), WidgetObserver<PanelEditor>(this, &PanelEditor::HandleCutMenuItem), Shortcut('X'));
	widgetMenuItem[kPanelMenuCut] = widget;
	editMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('PANL', 'MENU', 'EDIT', 'COPY')), WidgetObserver<PanelEditor>(this, &PanelEditor::HandleCopyMenuItem), Shortcut('C'));
	widgetMenuItem[kPanelMenuCopy] = widget;
	editMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('PANL', 'MENU', 'EDIT', 'PAST')), WidgetObserver<PanelEditor>(this, &PanelEditor::HandlePasteMenuItem), Shortcut('V'));
	widgetMenuItem[kPanelMenuPaste] = widget;
	if (!editorClipboard.GetFirstSubnode())
	{
		widget->Disable();
	}

	editMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('PANL', 'MENU', 'EDIT', 'CLER')), WidgetObserver<PanelEditor>(this, &PanelEditor::HandleClearMenuItem), Shortcut(kKeyCodeDelete, kShortcutUnmodified));
	widgetMenuItem[kPanelMenuClear] = widget;
	editMenu->AppendMenuItem(widget);

	editMenu->AppendMenuItem(new MenuItemWidget(kLineSolid));
	editMenu->AppendMenuItem(new MenuItemWidget(table->GetString(StringID('PANL', 'MENU', 'EDIT', 'SALL')), WidgetObserver<PanelEditor>(this, &PanelEditor::HandleSelectAllMenuItem), Shortcut('A')));

	widget = new MenuItemWidget(table->GetString(StringID('PANL', 'MENU', 'EDIT', 'DUPL')), WidgetObserver<PanelEditor>(this, &PanelEditor::HandleDuplicateMenuItem), Shortcut('D'));
	widgetMenuItem[kPanelMenuDuplicate] = widget;
	editMenu->AppendMenuItem(widget);

	// Widget Menu

	widgetMenu = new PulldownMenuWidget(table->GetString(StringID('PANL', 'MENU', 'WDGT')));
	menuBar->AppendMenu(widgetMenu);

	widget = new MenuItemWidget(table->GetString(StringID('PANL', 'MENU', 'WDGT', 'FRNT')), WidgetObserver<PanelEditor>(this, &PanelEditor::HandleBringToFrontMenuItem), Shortcut(']', kShortcutShift));
	widgetMenuItem[kPanelMenuBringToFront] = widget;
	widgetMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('PANL', 'MENU', 'WDGT', 'FRWD')), WidgetObserver<PanelEditor>(this, &PanelEditor::HandleBringForwardMenuItem), Shortcut(']'));
	widgetMenuItem[kPanelMenuBringForward] = widget;
	widgetMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('PANL', 'MENU', 'WDGT', 'BKWD')), WidgetObserver<PanelEditor>(this, &PanelEditor::HandleSendBackwardMenuItem), Shortcut('['));
	widgetMenuItem[kPanelMenuSendBackward] = widget;
	widgetMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('PANL', 'MENU', 'WDGT', 'BACK')), WidgetObserver<PanelEditor>(this, &PanelEditor::HandleSendToBackMenuItem), Shortcut('[', kShortcutShift));
	widgetMenuItem[kPanelMenuSendToBack] = widget;
	widgetMenu->AppendMenuItem(widget);

	widgetMenu->AppendMenuItem(new MenuItemWidget(kLineSolid));

	widget = new MenuItemWidget(table->GetString(StringID('PANL', 'MENU', 'WDGT', 'HIDE')), WidgetObserver<PanelEditor>(this, &PanelEditor::HandleHideSelectionMenuItem), Shortcut('H'));
	widgetMenuItem[kPanelMenuHideSelection] = widget;
	widgetMenu->AppendMenuItem(widget);

	widgetMenu->AppendMenuItem(new MenuItemWidget(table->GetString(StringID('PANL', 'MENU', 'WDGT', 'UHID')), WidgetObserver<PanelEditor>(this, &PanelEditor::HandleUnhideAllMenuItem), Shortcut('H', kShortcutShift)));
	widgetMenu->AppendMenuItem(new MenuItemWidget(kLineSolid));

	widget = new MenuItemWidget(table->GetString(StringID('PANL', 'MENU', 'WDGT', 'LOCK')), WidgetObserver<PanelEditor>(this, &PanelEditor::HandleLockSelectionMenuItem), Shortcut('L'));
	widgetMenuItem[kPanelMenuLockSelection] = widget;
	widgetMenu->AppendMenuItem(widget);

	widgetMenu->AppendMenuItem(new MenuItemWidget(table->GetString(StringID('PANL', 'MENU', 'WDGT', 'ULCK')), WidgetObserver<PanelEditor>(this, &PanelEditor::HandleUnlockAllMenuItem), Shortcut('L', kShortcutShift)));
	widgetMenu->AppendMenuItem(new MenuItemWidget(kLineSolid));

	widget = new MenuItemWidget(table->GetString(StringID('PANL', 'MENU', 'WDGT', 'GRUP')), WidgetObserver<PanelEditor>(this, &PanelEditor::HandleGroupSelectionMenuItem), Shortcut('G'));
	widgetMenuItem[kPanelMenuGroupSelection] = widget;
	widgetMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('PANL', 'MENU', 'WDGT', 'UGRP')), WidgetObserver<PanelEditor>(this, &PanelEditor::HandleUngroupSelectionMenuItem), Shortcut('G', kShortcutShift));
	widgetMenuItem[kPanelMenuUngroupSelection] = widget;
	widgetMenu->AppendMenuItem(widget);

	widgetMenu->AppendMenuItem(new MenuItemWidget(kLineSolid));

	widget = new MenuItemWidget(table->GetString(StringID('PANL', 'MENU', 'WDGT', 'RROT')), WidgetObserver<PanelEditor>(this, &PanelEditor::HandleResetRotationMenuItem), Shortcut('R'));
	widgetMenuItem[kPanelMenuResetRotation] = widget;
	widgetMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('PANL', 'MENU', 'WDGT', 'RTEX')), WidgetObserver<PanelEditor>(this, &PanelEditor::HandleResetTexcoordsMenuItem), Shortcut('T'));
	widgetMenuItem[kPanelMenuResetTexcoords] = widget;
	widgetMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('PANL', 'MENU', 'WDGT', 'ASCL')), WidgetObserver<PanelEditor>(this, &PanelEditor::HandleAutoScaleTextureMenuItem), Shortcut('F'));
	widgetMenuItem[kPanelMenuAutoScaleTexture] = widget;
	widgetMenu->AppendMenuItem(widget);

	widgetMenu->AppendMenuItem(new MenuItemWidget(kLineSolid));

	widget = new MenuItemWidget(table->GetString(StringID('PANL', 'MENU', 'WDGT', 'ESCR')), WidgetObserver<PanelEditor>(this, &PanelEditor::HandleEditScriptMenuItem), Shortcut('E'));
	widgetMenuItem[kPanelMenuEditScript] = widget;
	widgetMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('PANL', 'MENU', 'WDGT', 'DSCR')), WidgetObserver<PanelEditor>(this, &PanelEditor::HandleDeleteScriptMenuItem));
	widgetMenuItem[kPanelMenuDeleteScript] = widget;
	widgetMenu->AppendMenuItem(widget);

	// Arrange Menu

	arrangeMenu = new PulldownMenuWidget(table->GetString(StringID('PANL', 'MENU', 'ARNG')));
	menuBar->AppendMenu(arrangeMenu);

	widget = new MenuItemWidget(table->GetString(StringID('PANL', 'MENU', 'ARNG', 'NUGL')), WidgetObserver<PanelEditor>(this, &PanelEditor::HandleNudgeLeftMenuItem), Shortcut(kKeyCodeLeftArrow));
	widgetMenuItem[kPanelMenuNudgeLeft] = widget;
	arrangeMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('PANL', 'MENU', 'ARNG', 'NUGR')), WidgetObserver<PanelEditor>(this, &PanelEditor::HandleNudgeRightMenuItem), Shortcut(kKeyCodeRightArrow));
	widgetMenuItem[kPanelMenuNudgeRight] = widget;
	arrangeMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('PANL', 'MENU', 'ARNG', 'NUGU')), WidgetObserver<PanelEditor>(this, &PanelEditor::HandleNudgeUpMenuItem), Shortcut(kKeyCodeUpArrow));
	widgetMenuItem[kPanelMenuNudgeUp] = widget;
	arrangeMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('PANL', 'MENU', 'ARNG', 'NUGD')), WidgetObserver<PanelEditor>(this, &PanelEditor::HandleNudgeDownMenuItem), Shortcut(kKeyCodeDownArrow));
	widgetMenuItem[kPanelMenuNudgeDown] = widget;
	arrangeMenu->AppendMenuItem(widget);

	arrangeMenu->AppendMenuItem(new MenuItemWidget(kLineSolid));

	widget = new MenuItemWidget(table->GetString(StringID('PANL', 'MENU', 'ARNG', 'ALNL')), WidgetObserver<PanelEditor>(this, &PanelEditor::HandleAlignLeftSidesMenuItem), Shortcut(kKeyCodeLeftArrow, kShortcutShift));
	widgetMenuItem[kPanelMenuAlignLeftSides] = widget;
	arrangeMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('PANL', 'MENU', 'ARNG', 'ALNR')), WidgetObserver<PanelEditor>(this, &PanelEditor::HandleAlignRightSidesMenuItem), Shortcut(kKeyCodeRightArrow, kShortcutShift));
	widgetMenuItem[kPanelMenuAlignRightSides] = widget;
	arrangeMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('PANL', 'MENU', 'ARNG', 'ALNT')), WidgetObserver<PanelEditor>(this, &PanelEditor::HandleAlignTopSidesMenuItem), Shortcut(kKeyCodeUpArrow, kShortcutShift));
	widgetMenuItem[kPanelMenuAlignTopSides] = widget;
	arrangeMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('PANL', 'MENU', 'ARNG', 'ALNB')), WidgetObserver<PanelEditor>(this, &PanelEditor::HandleAlignBottomSidesMenuItem), Shortcut(kKeyCodeDownArrow, kShortcutShift));
	widgetMenuItem[kPanelMenuAlignBottomSides] = widget;
	arrangeMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('PANL', 'MENU', 'ARNG', 'ALNH')), WidgetObserver<PanelEditor>(this, &PanelEditor::HandleAlignHorizontalCentersMenuItem));
	widgetMenuItem[kPanelMenuAlignHorizontalCenters] = widget;
	arrangeMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('PANL', 'MENU', 'ARNG', 'ALNV')), WidgetObserver<PanelEditor>(this, &PanelEditor::HandleAlignVerticalCentersMenuItem));
	widgetMenuItem[kPanelMenuAlignVerticalCenters] = widget;
	arrangeMenu->AppendMenuItem(widget);
}

void PanelEditor::UpdateSettings(void)
{
	widgetConfigurationWidget->ReleaseConfiguration();

	const EditorWidgetManipulator *manipulator = selectionList.First();
	do
	{
		widgetConfigurationWidget->BuildConfiguration(manipulator->GetTargetWidget());
		manipulator = manipulator->Next();
	} while (manipulator);

	manipulator = selectionList.First();
	const Widget *widget = manipulator->GetTargetWidget();

	if ((!manipulator->Next()) && (widget->GetBaseWidgetType() == kWidgetText))
	{
		char	text[256];

		const char *string = static_cast<const TextWidget *>(widget)->GetText();

		int32 x = 0;
		for (;; string++)
		{
			char c = *string;
			if (c == 0)
			{
				break;
			}

			if ((c == 10) || (c == '\\'))
			{
				text[x] = '\\';
				text[x + 1] = (c == 10) ? 'n' : '\\';
				x += 2;
			}
			else
			{
				text[x] = c;
				x++;
			}
		}

		text[x] = 0;
		textStringBox->SetText(text);
		textStringBox->Show();
		textStringText->Show();
	}
	else
	{
		textStringBox->Hide();
		textStringText->Hide();
	}
}

void PanelEditor::UpdateMutators(const WidgetManipulator *manipulator)
{
	mutatorMenu->PurgeMenuItems();
	mutatorListWidget->PurgeListItems();

	mutatorConfigurationWidget->ReleaseConfiguration();
	validMutatorArray.Clear();

	const Widget *widget = manipulator->GetTargetWidget();

	const MutatorRegistration *registration = Mutator::GetFirstRegistration();
	while (registration)
	{
		if (registration->ValidWidget(widget))
		{
			validMutatorArray.AddElement(registration);
			mutatorMenu->AppendMenuItem(new MenuItemWidget(registration->GetMutatorName()));
		}

		registration = registration->Next();
	}

	if (validMutatorArray.GetElementCount() > 0)
	{
		mutatorMenu->Enable();
		mutatorMenu->SetSelection(0);
		addMutatorButton->Enable();
	}
	else
	{
		mutatorMenu->SetSelection(kWidgetValueNone);
		mutatorMenu->Disable();
		addMutatorButton->Disable();
	}

	removeMutatorButton->Disable();

	const Mutator *mutator = widget->GetFirstMutator();
	while (mutator)
	{
		registration = Mutator::FindRegistration(mutator->GetMutatorType());
		mutatorListWidget->AppendListItem(registration->GetMutatorName());
		mutator = mutator->Next();
	}
}

void PanelEditor::HandleWidgetConfigurationEvent(SettingInterface *settingInterface)
{
	const EditorWidgetManipulator *manipulator = selectionList.First();
	while (manipulator)
	{
		Widget *widget = manipulator->GetTargetWidget();
		widgetConfigurationWidget->CommitConfiguration(widget);
		widget->Invalidate();
		widget->Preprocess();

		manipulator = manipulator->Next();
	}

	editorState |= kPanelEditorUnsaved;
}

void PanelEditor::HandleMutatorConfigurationEvent(SettingInterface *settingInterface)
{
	Mutator *mutator = currentMutator;

	Widget *widget = mutator->GetTargetWidget();
	widget->SetWidgetColor(widget->GetWidgetColor());

	mutatorConfigurationWidget->CommitConfiguration(mutator);
	mutator->Reset();

	editorState |= kPanelEditorUnsaved;
	widget->Preprocess();
}

void PanelEditor::SelectWidgetTool(const WidgetRegistration *registration)
{
	if ((currentMode != kPanelEditorModeWidget) || (currentWidgetReg != registration))
	{
		if (currentMode == kPanelEditorModeTool)
		{
			toolButton[currentTool]->SetValue(0);
		}

		currentMode = kPanelEditorModeWidget;
		currentTool = 0;
		currentWidgetReg = registration;
	}
}

void PanelEditor::UpdateViewportScale(float scale, const Point3D *position)
{
	scale = Clamp(scale, 1.0F, 8.0F);

	if (position)
	{
		panelViewport->SetZoomScale(Vector2D(scale, scale), *position);
	}
	else
	{
		panelViewport->SetOrthoScale(Vector2D(scale, scale));
	}
}

void PanelEditor::ClampViewport(void)
{
	const Vector2D& scale = panelViewport->GetOrthoScale();
	float padding = 16.0F / scale.x;

	const Vector2D& panelSize = rootWidget->GetWidgetSize();
	float xsize = panelSize.x;
	float ysize = panelSize.y;

	OrthoCamera *camera = panelViewport->GetViewportCamera();
	const Point3D& position = camera->GetNodePosition();

	float w = panelViewport->GetWidgetSize().x * 0.5F * scale.x;
	float h = panelViewport->GetWidgetSize().y * 0.5F * scale.y;

	float viewportLeft = position.x - w;
	float viewportRight = position.x + w;
	float viewportTop = position.y + h;
	float viewportBottom = position.y - h;

	if (viewportRight < padding)
	{
		viewportLeft += padding - viewportRight;
		viewportRight = padding;
	}
	else if (viewportLeft > xsize - padding)
	{
		viewportRight -= viewportLeft - xsize + padding;
		viewportLeft = xsize - padding;
	}

	if (viewportTop < padding)
	{
		viewportBottom += padding - viewportTop;
		viewportTop = padding;
	}
	else if (viewportBottom > ysize - padding)
	{
		viewportTop -= viewportBottom - ysize + padding;
		viewportBottom = ysize - padding;
	}

	camera->SetNodePosition(Point3D(Floor((viewportLeft + viewportRight) * 0.5F), Floor((viewportTop + viewportBottom) * 0.5F), 0.5F));
	editorState |= kPanelEditorUpdateGrid;
}

void PanelEditor::SelectWidget(const Widget *widget)
{
	EditorWidgetManipulator *manipulator = GetManipulator(widget);
	unsigned_int32 state = manipulator->GetManipulatorState();

	if ((state & (kWidgetManipulatorSelected | kWidgetManipulatorLocked)) == 0)
	{
		manipulator->SetManipulatorState((state | kWidgetManipulatorSelected) & ~kWidgetManipulatorHidden);
		selectionList.Append(manipulator);
		manipulator->Invalidate();

		editorState = (editorState & ~kPanelEditorNudgeCombine) | (kPanelEditorUpdateMenus | kPanelEditorUpdateSettings | kPanelEditorUpdateTransformPage);
	}
}

void PanelEditor::UnselectWidget(const Widget *widget)
{
	EditorWidgetManipulator *manipulator = GetManipulator(widget);
	if (manipulator)
	{
		unsigned_int32 state = manipulator->GetManipulatorState();

		if (state & kWidgetManipulatorSelected)
		{
			manipulator->SetManipulatorState(state & ~(kWidgetManipulatorSelected | kWidgetManipulatorTempSelected));
			selectionList.Remove(manipulator);

			editorState = (editorState & ~kPanelEditorNudgeCombine) | (kPanelEditorUpdateMenus | kPanelEditorUpdateSettings | kPanelEditorUpdateTransformPage);
		}
	}
}

void PanelEditor::SelectAll(void)
{
	const Widget *widget = rootWidget->GetFirstSubnode();
	while (widget)
	{
		SelectWidget(widget);
		widget = widget->Next();
	}
}

void PanelEditor::UnselectAll(void)
{
	for (;;)
	{
		EditorWidgetManipulator *manipulator = selectionList.First();
		if (!manipulator)
		{
			break;
		}

		UnselectWidget(manipulator->GetTargetWidget());
	}
}

void PanelEditor::UnselectAllTemp(void)
{
	EditorWidgetManipulator *manipulator = selectionList.First();
	while (manipulator)
	{
		EditorWidgetManipulator *next = manipulator->Next();

		if (manipulator->GetManipulatorState() & kWidgetManipulatorTempSelected)
		{
			UnselectWidget(manipulator->GetTargetWidget());
		}

		manipulator = next;
	}
}

bool PanelEditor::SuperWidgetSelected(const Widget *widget) const
{
	const Widget *super = widget->GetSuperNode();
	while (super)
	{
		if (super == rootWidget)
		{
			break;
		}

		const WidgetManipulator *manipulator = super->GetManipulator();
		if (manipulator->Selected())
		{
			return (true);
		}

		super = super->GetSuperNode();
	}

	return (false);
}

void PanelEditor::DeleteWidget(Widget *widget, bool undoable)
{
	Widget *subnode = widget->GetFirstSubnode();
	while (subnode)
	{
		UnselectWidget(subnode);
		subnode = subnode->Next();
	}

	UnselectWidget(widget);

	if (undoable)
	{
		widget->Detach();
	}
	else
	{
		delete widget;
	}
}

void PanelEditor::AddOperation(PanelOperation *operation)
{
	if (operationList.GetElementCount() >= kMaxPanelOperationCount)
	{
		delete operationList.First();
	}

	operationList.Append(operation);

	widgetMenuItem[kPanelMenuUndo]->Enable();
	editorState |= kPanelEditorUnsaved;
}

void PanelEditor::DeleteLastOperation(void)
{
	PanelOperation *operation = operationList.Last();
	delete operation;

	if (operationList.Empty())
	{
		widgetMenuItem[kPanelMenuUndo]->Disable();
	}

	editorState |= kPanelEditorUpdateSettings;
}

void PanelEditor::HandleCloseMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	Close();
}

void PanelEditor::HandleSavePanelMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	if (targetPanel)
	{
		targetPanel->PurgeSubtree();

		const Widget *widget = rootWidget->GetFirstSubnode();
		while (widget)
		{
			targetPanel->AppendSubnode(widget->Clone());
			widget = widget->Next();
		}

		targetPanel->Preprocess();
	}
	else
	{
		if (resourceName[0] == 0)
		{
			HandleSavePanelAsMenuItem(nullptr, nullptr);
		}
		else
		{
			SavePanel();
		}
	}

	editorState &= ~kPanelEditorUnsaved;
}

void PanelEditor::HandleSavePanelAsMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	FilePicker *picker = new FilePicker('PANL', TheWorldEditor->GetStringTable()->GetString(StringID('PANL', 'SAVE')), TheResourceMgr->GetGenericCatalog(), PanelResource::GetDescriptor(), nullptr, kFilePickerSave);
	picker->SetCompletionProc(&SavePickerProc, this);
	AddSubwindow(picker);
}

void PanelEditor::HandleWindowSettingsMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	WindowSettingsWindow *window = new WindowSettingsWindow(this);
	AddSubwindow(window);
}

void PanelEditor::HandleUndoMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	PanelOperation *operation = operationList.Last();
	if (operation)
	{
		operation->Restore(this);
		delete operation;
	}

	if (operationList.Empty())
	{
		widgetMenuItem[kPanelMenuUndo]->Disable();
	}

	editorState |= kPanelEditorUnsaved | kPanelEditorUpdateSettings;
}

void PanelEditor::HandleCutMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	HandleCopyMenuItem(nullptr, nullptr);
	HandleClearMenuItem(nullptr, nullptr);
}

void PanelEditor::HandleCopyMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	editorClipboard.PurgeSubtree();

	const EditorWidgetManipulator *manipulator = selectionList.First();
	while (manipulator)
	{
		const Widget *widget = manipulator->GetTargetWidget();
		if (!SuperWidgetSelected(widget))
		{
			editorClipboard.AppendSubnode(widget->Clone());
		}

		manipulator = manipulator->Next();
	}

	MenuItemWidget *widget = widgetMenuItem[kPanelMenuPaste];
	if (!editorClipboard.GetFirstSubnode())
	{
		widget->Disable();
	}
	else
	{
		widget->Enable();
	}
}

void PanelEditor::HandlePasteMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	UnselectAll();

	float spacing = viewportGrid.GetGridLineSpacing();

	Widget *widget = editorClipboard.GetFirstSubnode();
	while (widget)
	{
		Widget *newItem = widget->Clone();

		const Point3D& position = newItem->GetWidgetPosition();
		newItem->SetWidgetPosition(Point3D(position.x + spacing, position.y - spacing, 0.0F));

		EditorWidgetManipulator::Install(newItem);
		rootWidget->AppendNewSubnode(newItem);
		SelectWidget(newItem);

		widget = widget->Next();
	}

	AddOperation(new CreatePanelOperation(&selectionList));
}

void PanelEditor::HandleClearMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	Widget *group = rootWidget->GetFirstSubnode();
	while (group)
	{
		if ((group->GetWidgetType() == kWidgetGeneric) && (!group->GetManipulator()->Selected()))
		{
			const Widget *widget = group->GetFirstSubnode();
			if (widget)
			{
				do
				{
					const WidgetManipulator *manipulator = widget->GetManipulator();
					if ((!manipulator) || (!manipulator->Selected()))
					{
						goto next;
					}

					widget = widget->Next();
				} while (widget);

				SelectWidget(group);
			}
		}

		next:
		group = group->Next();
	}

	AddOperation(new DeletePanelOperation(rootWidget));

	for (;;)
	{
		const EditorWidgetManipulator *manipulator = selectionList.First();
		if (!manipulator)
		{
			break;
		}

		Widget *widget = manipulator->GetTargetWidget();
		DeleteWidget(widget, true);
	}
}

void PanelEditor::HandleSelectAllMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	SelectAll();
}

void PanelEditor::HandleDuplicateMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	HandleCopyMenuItem(nullptr, nullptr);
	HandlePasteMenuItem(nullptr, nullptr);
}

void PanelEditor::BringWidgetsForward(Widget *root)
{
	Widget *widget = root->GetLastSubnode();
	while (widget)
	{
		Widget *previous = widget->Previous();
		if (!previous)
		{
			break;
		}

		const WidgetManipulator *manipulator = previous->GetManipulator();
		if ((manipulator) && (manipulator->Selected()))
		{
			root->InsertSubnodeAfter(previous, widget);
		}

		widget = widget->Previous();
	}

	widget = root->GetFirstSubnode();
	while (widget)
	{
		if (widget->GetManipulator())
		{
			BringWidgetsForward(widget);
		}

		widget = widget->Next();
	}
}

void PanelEditor::BringWidgetsToFront(Widget *root)
{
	const Widget *last = root->GetLastSubnode();

	Widget *widget = root->GetFirstSubnode();
	while (widget)
	{
		Widget *next = widget->Next();

		const WidgetManipulator *manipulator = widget->GetManipulator();
		if ((manipulator) && (manipulator->Selected()))
		{
			root->AppendSubnode(widget);
		}

		if (widget == last)
		{
			break;
		}

		widget = next;
	}

	widget = root->GetLastSubnode();
	while (widget)
	{
		if (widget->GetManipulator())
		{
			BringWidgetsToFront(widget);
		}

		widget = widget->Previous();
	}
}

void PanelEditor::SendWidgetsBackward(Widget *root)
{
	Widget *widget = root->GetFirstSubnode();
	while (widget)
	{
		Widget *next = widget->Next();
		if (!next)
		{
			break;
		}

		const WidgetManipulator *manipulator = next->GetManipulator();
		if ((manipulator) && (manipulator->Selected()))
		{
			root->InsertSubnodeBefore(next, widget);
		}

		widget = widget->Next();
	}

	widget = root->GetLastSubnode();
	while (widget)
	{
		if (widget->GetManipulator())
		{
			SendWidgetsBackward(widget);
		}

		widget = widget->Previous();
	}
}

void PanelEditor::SendWidgetsToBack(Widget *root)
{
	const Widget *first = root->GetFirstSubnode();

	Widget *widget = root->GetLastSubnode();
	while (widget)
	{
		Widget *previous = widget->Previous();

		const WidgetManipulator *manipulator = widget->GetManipulator();
		if ((manipulator) && (manipulator->Selected()))
		{
			root->PrependSubnode(widget);
		}

		if (widget == first)
		{
			break;
		}

		widget = previous;
	}

	widget = root->GetFirstSubnode();
	while (widget)
	{
		if (widget->GetManipulator())
		{
			SendWidgetsToBack(widget);
		}

		widget = widget->Next();
	}
}

void PanelEditor::HandleBringForwardMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	AddOperation(new ArrangePanelOperation(rootWidget));

	BringWidgetsForward(rootWidget);
	rootWidget->Preprocess();
}

void PanelEditor::HandleBringToFrontMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	AddOperation(new ArrangePanelOperation(rootWidget));

	BringWidgetsToFront(rootWidget);
	rootWidget->Preprocess();
}

void PanelEditor::HandleSendBackwardMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	AddOperation(new ArrangePanelOperation(rootWidget));

	SendWidgetsBackward(rootWidget);
	rootWidget->Preprocess();
}

void PanelEditor::HandleSendToBackMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	AddOperation(new ArrangePanelOperation(rootWidget));

	SendWidgetsToBack(rootWidget);
	rootWidget->Preprocess();
}

void PanelEditor::HandleHideSelectionMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	EditorWidgetManipulator *manipulator = selectionList.First();
	while (manipulator)
	{
		manipulator->SetManipulatorState(manipulator->GetManipulatorState() | kWidgetManipulatorHidden);
		manipulator->Invalidate();

		manipulator = manipulator->Next();
	}

	UnselectAll();
}

void PanelEditor::HandleUnhideAllMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	const Widget *widget = rootWidget->GetFirstSubnode();
	while (widget)
	{
		WidgetManipulator *manipulator = widget->GetManipulator();
		if (manipulator)
		{
			manipulator->SetManipulatorState(manipulator->GetManipulatorState() & ~kWidgetManipulatorHidden);
			manipulator->Invalidate();

			widget = rootWidget->GetNextNode(widget);
		}
		else
		{
			widget = rootWidget->GetNextLevelNode(widget);
		}
	}
}

void PanelEditor::HandleLockSelectionMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	EditorWidgetManipulator *manipulator = selectionList.First();
	while (manipulator)
	{
		manipulator->SetManipulatorState(manipulator->GetManipulatorState() | kWidgetManipulatorLocked);
		manipulator = manipulator->Next();
	}

	UnselectAll();
}

void PanelEditor::HandleUnlockAllMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	const Widget *widget = rootWidget->GetFirstSubnode();
	while (widget)
	{
		WidgetManipulator *manipulator = widget->GetManipulator();
		if (manipulator)
		{
			manipulator->SetManipulatorState(manipulator->GetManipulatorState() & ~kWidgetManipulatorLocked);

			widget = rootWidget->GetNextNode(widget);
		}
		else
		{
			widget = rootWidget->GetNextLevelNode(widget);
		}
	}
}

void PanelEditor::HandleGroupSelectionMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	Widget *group = new Widget;
	EditorWidgetManipulator::Install(group);

	AddOperation(new GroupPanelOperation(group, rootWidget));

	float xmin = K::infinity;
	float ymin = K::infinity;

	const EditorWidgetManipulator *editorManipulator = selectionList.First();
	while (editorManipulator)
	{
		const Point3D& position = editorManipulator->GetTargetWidget()->GetWidgetPosition();
		xmin = Fmin(xmin, position.x);
		ymin = Fmin(ymin, position.y);

		editorManipulator = editorManipulator->Next();
	}

	Widget *widget = rootWidget->GetFirstSubnode();
	while (widget)
	{
		Widget *next = widget->Next();

		const WidgetManipulator *manipulator = widget->GetManipulator();
		if (manipulator->Selected())
		{
			group->AppendSubnode(widget);
			const Point3D& position = widget->GetWidgetPosition();
			widget->SetWidgetPosition(Point3D(position.x - xmin, position.y - ymin, position.z));
		}

		widget = next;
	}

	group->SetWidgetPosition(Point3D(xmin, ymin, 0.0F));
	rootWidget->AppendNewSubnode(group);
	group->Update();

	UnselectAll();
	SelectWidget(group);
}

void PanelEditor::HandleUngroupSelectionMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	List<EditorWidgetManipulator>		selectList;

	AddOperation(new UngroupPanelOperation(&selectionList));

	for (;;)
	{
		const EditorWidgetManipulator *manipulator = selectionList.First();
		if (!manipulator)
		{
			break;
		}

		Widget *group = manipulator->GetTargetWidget();
		Widget *root = group->GetSuperNode();
		const Transform4D& transform = group->GetWidgetTransform();

		Widget *widget = group->GetFirstSubnode();
		while (widget)
		{
			Widget *next = widget->Next();

			widget->SetWidgetTransform(transform * widget->GetWidgetTransform());
			root->InsertSubnodeBefore(widget, group);
			selectList.Append(GetManipulator(widget));

			widget = next;
		}

		DeleteWidget(group, true);
	}

	for (;;)
	{
		WidgetManipulator *manipulator = selectList.First();
		if (!manipulator)
		{
			break;
		}

		SelectWidget(manipulator->GetTargetWidget());
	}
}

void PanelEditor::HandleResetRotationMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	AddOperation(new TransformPanelOperation(&selectionList));

	const EditorWidgetManipulator *manipulator = selectionList.First();
	while (manipulator)
	{
		Widget *widget = manipulator->GetTargetWidget();

		const Vector2D& size = widget->GetWidgetSize();
		float width = size.x * 0.5F;
		float height = size.y * 0.5F;

		const Transform4D& transform = widget->GetWidgetTransform();
		Point3D center = transform.GetTranslation() + transform[0] * width + transform[1] * height;

		widget->SetWidgetMatrix3D(Identity3D);
		widget->SetWidgetPosition(Point3D(center.x - width, center.y - height, 0.0F));
		widget->Invalidate();

		manipulator = manipulator->Next();
	}
}

void PanelEditor::HandleResetTexcoordsMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	AddOperation(new WarpPanelOperation(&selectionList));

	const EditorWidgetManipulator *manipulator = selectionList.First();
	while (manipulator)
	{
		Widget *widget = manipulator->GetTargetWidget();
		if (widget->GetWidgetType() == kWidgetImage)
		{
			ImageWidget *image = static_cast<ImageWidget *>(widget);

			image->SetImageOffset(Zero2D);
			image->SetImageScale(Vector2D(1.0F, 1.0F));
		}

		manipulator = manipulator->Next();
	}
}

void PanelEditor::HandleAutoScaleTextureMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	AddOperation(new WarpPanelOperation(&selectionList));

	const EditorWidgetManipulator *manipulator = selectionList.First();
	while (manipulator)
	{
		Widget *widget = manipulator->GetTargetWidget();
		if (widget->GetWidgetType() == kWidgetImage)
		{
			ImageWidget *image = static_cast<ImageWidget *>(widget);
			const Vector2D& size = image->GetWidgetSize();
			float imageAspect = size.y / size.x;

			const Texture *texture = image->GetTexture();
			float textureAspect = (float) texture->GetTextureWidth() / (float) texture->GetTextureHeight();

			float sx = image->GetImageScale().x;
			image->SetImageScale(Vector2D(sx, sx * imageAspect * textureAspect));
		}

		manipulator = manipulator->Next();
	}

	editorState |= kPanelEditorUpdateSettings;
}

void PanelEditor::HandleEditScriptMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	Widget *widget = selectionList.First()->GetTargetWidget();

	ScriptObject *scriptObject = widget->GetScriptObject();
	if (!scriptObject)
	{
		scriptObject = new ScriptObject;
		widget->SetScriptObject(scriptObject);
		scriptObject->Release();
	}

	if (!scriptObject->GetFirstScriptGraph())
	{
		ScriptGraph *graph = new ScriptGraph;
		graph->GetScriptEventArray()->AddElement(kEventWidgetActivate);
		scriptObject->AddScriptGraph(graph);
	}

	ScriptEditor *scriptEditor = new ScriptEditor(targetNode, static_cast<PanelController *>(targetNode->GetController()), scriptObject, true);
	scriptEditor->SetCompletionProc(&ScriptEditorComplete, this);
	AddSubwindow(scriptEditor);
}

void PanelEditor::HandleDeleteScriptMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	AddOperation(new ScriptPanelOperation(&selectionList));

	const EditorWidgetManipulator *manipulator = selectionList.First();
	while (manipulator)
	{
		Widget *widget = manipulator->GetTargetWidget();
		widget->SetScriptObject(nullptr);

		manipulator = manipulator->Next();
	}
}

void PanelEditor::HandleNudgeLeftMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	unsigned_int32 state = editorState | kPanelEditorUpdateTransformPage;
	if (!(state & kPanelEditorNudgeCombine))
	{
		state |= kPanelEditorNudgeCombine;
		AddOperation(new TransformPanelOperation(&selectionList));
	}

	editorState = state;

	EditorWidgetManipulator *manipulator = selectionList.First();
	while (manipulator)
	{
		Widget *widget = manipulator->GetTargetWidget();

		const Point3D& position = widget->GetWidgetPosition();
		widget->SetWidgetPosition(Point3D(position.x - 1.0F, position.y, position.z));
		widget->Invalidate();

		manipulator = manipulator->Next();
	}
}

void PanelEditor::HandleNudgeRightMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	unsigned_int32 state = editorState | kPanelEditorUpdateTransformPage;
	if (!(state & kPanelEditorNudgeCombine))
	{
		state |= kPanelEditorNudgeCombine;
		AddOperation(new TransformPanelOperation(&selectionList));
	}

	editorState = state;

	EditorWidgetManipulator *manipulator = selectionList.First();
	while (manipulator)
	{
		Widget *widget = manipulator->GetTargetWidget();

		const Point3D& position = widget->GetWidgetPosition();
		widget->SetWidgetPosition(Point3D(position.x + 1.0F, position.y, position.z));
		widget->Invalidate();

		manipulator = manipulator->Next();
	}
}

void PanelEditor::HandleNudgeUpMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	unsigned_int32 state = editorState | kPanelEditorUpdateTransformPage;
	if (!(state & kPanelEditorNudgeCombine))
	{
		state |= kPanelEditorNudgeCombine;
		AddOperation(new TransformPanelOperation(&selectionList));
	}

	editorState = state;

	EditorWidgetManipulator *manipulator = selectionList.First();
	while (manipulator)
	{
		Widget *widget = manipulator->GetTargetWidget();

		const Point3D& position = widget->GetWidgetPosition();
		widget->SetWidgetPosition(Point3D(position.x, position.y - 1.0F, position.z));
		widget->Invalidate();

		manipulator = manipulator->Next();
	}
}

void PanelEditor::HandleNudgeDownMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	unsigned_int32 state = editorState | kPanelEditorUpdateTransformPage;
	if (!(state & kPanelEditorNudgeCombine))
	{
		state |= kPanelEditorNudgeCombine;
		AddOperation(new TransformPanelOperation(&selectionList));
	}

	editorState = state;

	EditorWidgetManipulator *manipulator = selectionList.First();
	while (manipulator)
	{
		Widget *widget = manipulator->GetTargetWidget();

		const Point3D& position = widget->GetWidgetPosition();
		widget->SetWidgetPosition(Point3D(position.x, position.y + 1.0F, position.z));
		widget->Invalidate();

		manipulator = manipulator->Next();
	}
}

void PanelEditor::HandleAlignLeftSidesMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	AddOperation(new TransformPanelOperation(&selectionList));

	float xmin = K::infinity;
	EditorWidgetManipulator *manipulator = selectionList.First();
	while (manipulator)
	{
		const Widget *widget = manipulator->GetTargetWidget();
		Box3D boundingBox(Zero3D, Point3D(widget->GetWidgetSize(), 0.0F));
		boundingBox.Transform(widget->GetWorldTransform());
		xmin = Fmin(xmin, boundingBox.min.x);

		manipulator = manipulator->Next();
	}

	manipulator = selectionList.First();
	while (manipulator)
	{
		Widget *widget = manipulator->GetTargetWidget();
		Box3D boundingBox(Zero3D, Point3D(widget->GetWidgetSize(), 0.0F));
		boundingBox.Transform(widget->GetWorldTransform());
		float dx = xmin - boundingBox.min.x;

		const Widget *super = widget->GetSuperNode();
		widget->SetWidgetPosition(widget->GetWidgetPosition() + super->GetInverseWorldTransform()[0] * dx);
		widget->Invalidate();

		manipulator = manipulator->Next();
	}
}

void PanelEditor::HandleAlignRightSidesMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	AddOperation(new TransformPanelOperation(&selectionList));

	float xmax = K::minus_infinity;
	EditorWidgetManipulator *manipulator = selectionList.First();
	while (manipulator)
	{
		const Widget *widget = manipulator->GetTargetWidget();
		Box3D boundingBox(Zero3D, Point3D(widget->GetWidgetSize(), 0.0F));
		boundingBox.Transform(widget->GetWorldTransform());
		xmax = Fmax(xmax, boundingBox.max.x);

		manipulator = manipulator->Next();
	}

	manipulator = selectionList.First();
	while (manipulator)
	{
		Widget *widget = manipulator->GetTargetWidget();
		Box3D boundingBox(Zero3D, Point3D(widget->GetWidgetSize(), 0.0F));
		boundingBox.Transform(widget->GetWorldTransform());
		float dx = xmax - boundingBox.max.x;

		const Widget *super = widget->GetSuperNode();
		widget->SetWidgetPosition(widget->GetWidgetPosition() + super->GetInverseWorldTransform()[0] * dx);
		widget->Invalidate();

		manipulator = manipulator->Next();
	}
}

void PanelEditor::HandleAlignTopSidesMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	AddOperation(new TransformPanelOperation(&selectionList));

	float ymin = K::infinity;
	EditorWidgetManipulator *manipulator = selectionList.First();
	while (manipulator)
	{
		const Widget *widget = manipulator->GetTargetWidget();
		Box3D boundingBox(Zero3D, Point3D(widget->GetWidgetSize(), 0.0F));
		boundingBox.Transform(widget->GetWorldTransform());
		ymin = Fmin(ymin, boundingBox.min.y);

		manipulator = manipulator->Next();
	}

	manipulator = selectionList.First();
	while (manipulator)
	{
		Widget *widget = manipulator->GetTargetWidget();
		Box3D boundingBox(Zero3D, Point3D(widget->GetWidgetSize(), 0.0F));
		boundingBox.Transform(widget->GetWorldTransform());
		float dy = ymin - boundingBox.min.y;

		const Widget *super = widget->GetSuperNode();
		widget->SetWidgetPosition(widget->GetWidgetPosition() + super->GetInverseWorldTransform()[1] * dy);
		widget->Invalidate();

		manipulator = manipulator->Next();
	}
}

void PanelEditor::HandleAlignBottomSidesMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	AddOperation(new TransformPanelOperation(&selectionList));

	float ymax = K::minus_infinity;
	EditorWidgetManipulator *manipulator = selectionList.First();
	while (manipulator)
	{
		const Widget *widget = manipulator->GetTargetWidget();
		Box3D boundingBox(Zero3D, Point3D(widget->GetWidgetSize(), 0.0F));
		boundingBox.Transform(widget->GetWorldTransform());
		ymax = Fmax(ymax, boundingBox.max.y);

		manipulator = manipulator->Next();
	}

	manipulator = selectionList.First();
	while (manipulator)
	{
		Widget *widget = manipulator->GetTargetWidget();
		Box3D boundingBox(Zero3D, Point3D(widget->GetWidgetSize(), 0.0F));
		boundingBox.Transform(widget->GetWorldTransform());
		float dy = ymax - boundingBox.max.y;

		const Widget *super = widget->GetSuperNode();
		widget->SetWidgetPosition(widget->GetWidgetPosition() + super->GetInverseWorldTransform()[1] * dy);
		widget->Invalidate();

		manipulator = manipulator->Next();
	}
}

void PanelEditor::HandleAlignHorizontalCentersMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	AddOperation(new TransformPanelOperation(&selectionList));

	float xmin = K::infinity;
	float xmax = K::minus_infinity;
	EditorWidgetManipulator *manipulator = selectionList.First();
	while (manipulator)
	{
		const Widget *widget = manipulator->GetTargetWidget();
		Box3D boundingBox(Zero3D, Point3D(widget->GetWidgetSize(), 0.0F));
		boundingBox.Transform(widget->GetWorldTransform());
		xmin = Fmin(xmin, boundingBox.min.x);
		xmax = Fmax(xmax, boundingBox.max.x);

		manipulator = manipulator->Next();
	}

	float center = (xmin + xmax) * 0.5F;
	manipulator = selectionList.First();
	while (manipulator)
	{
		Widget *widget = manipulator->GetTargetWidget();
		Box3D boundingBox(Zero3D, Point3D(widget->GetWidgetSize(), 0.0F));
		boundingBox.Transform(widget->GetWorldTransform());
		float dx = center - (boundingBox.min.x + boundingBox.max.x) * 0.5F;

		const Widget *super = widget->GetSuperNode();
		Point3D p = widget->GetWidgetPosition() + super->GetInverseWorldTransform()[0] * dx;
		p.Set(Floor(p.x), Floor(p.y), Floor(p.z));
		widget->SetWidgetPosition(p);
		widget->Invalidate();

		manipulator = manipulator->Next();
	}
}

void PanelEditor::HandleAlignVerticalCentersMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	AddOperation(new TransformPanelOperation(&selectionList));

	float ymin = K::infinity;
	float ymax = K::minus_infinity;
	EditorWidgetManipulator *manipulator = selectionList.First();
	while (manipulator)
	{
		const Widget *widget = manipulator->GetTargetWidget();
		Box3D boundingBox(Zero3D, Point3D(widget->GetWidgetSize(), 0.0F));
		boundingBox.Transform(widget->GetWorldTransform());
		ymin = Fmin(ymin, boundingBox.min.y);
		ymax = Fmax(ymax, boundingBox.max.y);

		manipulator = manipulator->Next();
	}

	float center = (ymin + ymax) * 0.5F;
	manipulator = selectionList.First();
	while (manipulator)
	{
		Widget *widget = manipulator->GetTargetWidget();
		Box3D boundingBox(Zero3D, Point3D(widget->GetWidgetSize(), 0.0F));
		boundingBox.Transform(widget->GetWorldTransform());
		float dy = center - (boundingBox.min.y + boundingBox.max.y) * 0.5F;

		const Widget *super = widget->GetSuperNode();
		Point3D p = widget->GetWidgetPosition() + super->GetInverseWorldTransform()[1] * dy;
		p.Set(Floor(p.x), Floor(p.y), Floor(p.z));
		widget->SetWidgetPosition(p);
		widget->Invalidate();

		manipulator = manipulator->Next();
	}
}

void PanelEditor::ScriptEditorComplete(ScriptEditor *scriptEditor, void *cookie)
{
	PanelEditor *panelEditor = static_cast<PanelEditor *>(cookie);

	Widget *widget = panelEditor->selectionList.First()->GetTargetWidget();
	ScriptObject *scriptObject = widget->GetScriptObject();
	if (!scriptObject->GetFirstValue())
	{
		ScriptGraph *scriptGraph = scriptObject->GetFirstScriptGraph();
		while (scriptGraph)
		{
			ScriptGraph *next = scriptGraph->Next();

			if (!scriptGraph->GetFirstElement())
			{
				delete scriptGraph;
			}

			scriptGraph = next;
		}

		if (!scriptObject->GetFirstScriptGraph())
		{
			widget->SetScriptObject(nullptr);
		}
	}

	panelEditor->editorState |= kPanelEditorUnsaved;
}

Point3D PanelEditor::ViewportToWorldPosition(const Point3D& p) const
{
	const Point3D& position = panelViewport->GetViewportCamera()->GetWorldPosition();
	const Vector2D& scale = panelViewport->GetOrthoScale();

	float x = (p.x - panelViewport->GetWidgetSize().x * 0.5F) * scale.x + position.x;
	float y = (p.y - panelViewport->GetWidgetSize().y * 0.5F) * scale.y + position.y;

	return (Point3D(Floor(x), Floor(y), 0.0F));
}

void PanelEditor::AutoScroll(const Point3D& p)
{
	float w = panelViewport->GetWidgetSize().x;
	float h = panelViewport->GetWidgetSize().y;

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

		const Vector2D& scale = panelViewport->GetOrthoScale();
		OrthoCamera *camera = panelViewport->GetViewportCamera();
		camera->SetNodePosition(camera->GetNodePosition() + Vector3D(Floor(dx * scale.x + 0.5F), Floor(dy * scale.y + 0.5F), 0.0F));
		ClampViewport();
	}
}

Widget *PanelEditor::PickWidget(const Point3D& p) const
{
	Widget *widget = rootWidget->GetRightmostNode();
	while (widget)
	{
		if (widget->GetWidgetType() != kWidgetGeneric)
		{
			const EditorWidgetManipulator *manipulator = GetManipulator(widget);
			if ((manipulator) && ((manipulator->GetManipulatorState() & (kWidgetManipulatorHidden | kWidgetManipulatorLocked)) == 0))
			{
				if (manipulator->GetWidgetBounds().Contains((widget->GetInverseWorldTransform() * p).GetPoint2D()))
				{
					Widget *super = widget->GetSuperNode();
					if (super == rootWidget)
					{
						return (widget);
					}

					manipulator = GetManipulator(super);
					if ((manipulator) && ((manipulator->GetManipulatorState() & (kWidgetManipulatorHidden | kWidgetManipulatorLocked)) == 0))
					{
						if (InterfaceMgr::GetCommandKey())
						{
							return (widget);
						}

						return (super);
					}
				}
			}
		}

		widget = rootWidget->GetPreviousNode(widget);
	}

	return (nullptr);
}

Widget *PanelEditor::PickWidgetHandle(const Point3D& p, int32 *index) const
{
	Point2D		handle[8];

	float scale = panelViewport->GetOrthoScale().x;
	float size = 5.0F * scale;

	const EditorWidgetManipulator *manipulator = selectionList.First();
	while (manipulator)
	{
		manipulator->GetHandlePositions(handle);

		Widget *widget = manipulator->GetTargetWidget();
		Vector3D q = (widget->GetWidgetType() != kWidgetGeneric) ? widget->GetInverseWorldTransform() * p : p;

		for (machine a = 0; a < 8; a++)
		{
			if ((Fabs(q.x - handle[a].x) < size) && (Fabs(q.y - handle[a].y) < size))
			{
				*index = a;
				return (widget);
			}
		}

		manipulator = manipulator->Next();
	}

	return (nullptr);
}

void PanelEditor::MoveSelectedWidgets(const Point3D& p)
{
	Point3D position = ViewportToWorldPosition(p);

	if (InterfaceMgr::GetShiftKey())
	{
		float dx = position.x - anchorPosition.x;
		float dy = position.y - anchorPosition.y;

		if (Fabs(dy) > Fabs(dx))
		{
			position.x = anchorPosition.x;
		}
		else
		{
			position.y = anchorPosition.y;
		}
	}

	if (position != previousPosition)
	{
		previousPosition = position;

		unsigned_int32 state = (editorState |= kPanelEditorUpdateTransformPage);
		if (state & kPanelEditorUndoPending)
		{
			editorState &= ~kPanelEditorUndoPending;
			AddOperation(new TransformPanelOperation(&selectionList));
		}

		const Point3D& originalPosition = GetManipulator(pickedWidget)->GetOriginalPosition();
		position = originalPosition + (position - anchorPosition);
		if (editorFlags & kPanelEditorSnapToGrid)
		{
			position.x = SnapToGrid(position.x);
			position.y = SnapToGrid(position.y);
		}
		else
		{
			position.x = Floor(position.x);
			position.y = Floor(position.y);
		}

		pickedWidget->SetWidgetPosition(position);
		pickedWidget->Invalidate();

		Vector3D delta = position - originalPosition;

		EditorWidgetManipulator *manipulator = selectionList.First();
		while (manipulator)
		{
			Widget *widget = manipulator->GetTargetWidget();
			if (widget != pickedWidget)
			{
				widget->SetWidgetPosition(manipulator->GetOriginalPosition() + delta);
				widget->Invalidate();
			}

			manipulator = manipulator->Next();
		}
	}
}

void PanelEditor::RotateSelectedWidgets(const Point3D& p)
{
	Point3D position = ViewportToWorldPosition(p);
	Vector2D v1(anchorPosition.x - rotationCenter.x, anchorPosition.y - rotationCenter.y);
	Vector2D v2(position.x - rotationCenter.x, position.y - rotationCenter.y);

	float angle = Acos(v1 * v2 * InverseSqrt(SquaredMag(v1) * SquaredMag(v2)));
	if (v1.x * v2.y - v1.y * v2.x < 0.0F)
	{
		angle = -angle;
	}

	if (InterfaceMgr::GetShiftKey())
	{
		angle = Floor(angle / K::tau_over_8 + 0.5F) * K::tau_over_8;
	}

	if (angle != previousAngle)
	{
		Matrix3D	rotation;

		previousAngle = angle;

		unsigned_int32 state = (editorState |= kPanelEditorUpdateTransformPage);
		if (state & kPanelEditorUndoPending)
		{
			editorState &= ~kPanelEditorUndoPending;
			AddOperation(new TransformPanelOperation(&selectionList));
		}

		rotation.SetRotationAboutZ(angle);
		Transform4D transform(rotation, rotationCenter - rotation * rotationCenter);

		EditorWidgetManipulator *manipulator = selectionList.First();
		while (manipulator)
		{
			Widget *widget = manipulator->GetTargetWidget();
			widget->SetWidgetTransform(transform * manipulator->GetOriginalTransform());
			widget->Invalidate();

			manipulator = manipulator->Next();
		}

		editorState |= kPanelEditorUpdateTransformPage;
	}
}

void PanelEditor::ResizeSelectedWidgets(const Point3D& p)
{
	Point3D position = ViewportToWorldPosition(p);

	if (position != previousPosition)
	{
		previousPosition = position;

		unsigned_int32 state = (editorState |= kPanelEditorUpdateTransformPage);
		if (state & kPanelEditorUndoPending)
		{
			editorState &= ~kPanelEditorUndoPending;
			AddOperation(new TransformPanelOperation(&selectionList));
		}

		Vector3D worldDelta = position - anchorPosition;
		EditorWidgetManipulator *manipulator = selectionList.First();
		while (manipulator)
		{
			Widget *widget = manipulator->GetTargetWidget();
			if (widget->GetWidgetType() != kWidgetGeneric)
			{
				Vector3D delta = Inverse(widget->GetWorldTransform()) * worldDelta;

				const Point3D& oldPosition = manipulator->GetOriginalPosition();
				const Vector2D& oldSize = manipulator->GetOriginalSize();

				Point3D newPosition = oldPosition;
				Vector2D newSize = oldSize;

				if ((handleIndex == 0) || (handleIndex == 1) || (handleIndex == 4))
				{
					newPosition.x += delta.x;
					if (editorFlags & kPanelEditorSnapToGrid)
					{
						newPosition.x = SnapToGrid(newPosition.x);
					}

					newSize.x = oldPosition.x - newPosition.x + oldSize.x;
				}
				else if ((handleIndex == 2) || (handleIndex == 3) || (handleIndex == 6))
				{
					newSize.x += delta.x;
					if (editorFlags & kPanelEditorSnapToGrid)
					{
						newSize.x = SnapToGrid(newPosition.x + newSize.x) - newPosition.x;
					}
				}

				if ((handleIndex == 0) || (handleIndex == 3) || (handleIndex == 7))
				{
					newPosition.y += delta.y;
					if (editorFlags & kPanelEditorSnapToGrid)
					{
						newPosition.y = SnapToGrid(newPosition.y);
					}

					newSize.y = oldPosition.y - newPosition.y + oldSize.y;
				}
				else if ((handleIndex == 1) || (handleIndex == 2) || (handleIndex == 5))
				{
					newSize.y += delta.y;
					if (editorFlags & kPanelEditorSnapToGrid)
					{
						newSize.y = SnapToGrid(newPosition.y + newSize.y) - newPosition.y;
					}
				}

				float sx = oldPosition.x + oldSize.x - 1.0F;
				if (newPosition.x > sx)
				{
					newPosition.x = sx;
					newSize.x = 1.0F;
				}
				else if (newSize.x < 1.0F)
				{
					newSize.x = 1.0F;
				}

				float sy = oldPosition.y + oldSize.y - 1.0F;
				if (newPosition.y > sy)
				{
					newPosition.y = sy;
					newSize.y = 1.0F;
				}
				else if (newSize.y < 1.0F)
				{
					newSize.y = 1.0F;
				}

				if (InterfaceMgr::GetShiftKey())
				{
					newSize.y = newSize.x * (oldSize.y / oldSize.x);
					if ((handleIndex == 0) || (handleIndex == 3) || (handleIndex == 7))
					{
						newPosition.y = oldPosition.y + oldSize.y - newSize.y;
					}
				}

				widget->SetWidgetPosition(newPosition);
				widget->SetWidgetSize(newSize);
				widget->Invalidate();
			}

			manipulator = manipulator->Next();
		}
	}
}

void PanelEditor::OffsetSelectedWidgets(const Point3D& p)
{
	Point3D position = ViewportToWorldPosition(p);

	if (position != previousPosition)
	{
		previousPosition = position;

		unsigned_int32 state = (editorState |= kPanelEditorUpdateTransformPage);
		if (state & kPanelEditorUndoPending)
		{
			editorState &= ~kPanelEditorUndoPending;
			AddOperation(new WarpPanelOperation(&selectionList));
		}

		Vector3D worldDelta = position - anchorPosition;
		EditorWidgetManipulator *manipulator = selectionList.First();
		while (manipulator)
		{
			Widget *widget = manipulator->GetTargetWidget();
			if (widget->GetWidgetType() == kWidgetImage)
			{
				ImageWidget *image = static_cast<ImageWidget *>(widget);

				const Vector2D& scale = image->GetImageScale();
				const Vector2D& size = image->GetWidgetSize();
				float w = size.x / scale.x;
				float h = size.y / scale.y;

				Vector3D delta = Inverse(image->GetWidgetTransform()) * worldDelta;
				if (InterfaceMgr::GetShiftKey())
				{
					if (Fabs(delta.y) > Fabs(delta.x))
					{
						delta.x = 0.0F;
					}
					else
					{
						delta.y = 0.0F;
					}
				}

				const Vector2D& offset = manipulator->GetOriginalOffset();
				float dx = offset.x * w - delta.x;
				float dy = offset.y * h + delta.y;

				if (editorFlags & kPanelEditorSnapToGrid)
				{
					dx = SnapToGrid(dx);
					dy = SnapToGrid(dy);
				}

				Vector2D newOffset(dx / w, dy / h);
				image->SetImageOffset(newOffset);
			}

			manipulator = manipulator->Next();
		}
	}
}

void PanelEditor::ScaleSelectedWidgets(const Point3D& p)
{
	Point3D position = ViewportToWorldPosition(p);

	if (position != previousPosition)
	{
		previousPosition = position;

		unsigned_int32 state = (editorState |= kPanelEditorUpdateTransformPage);
		if (state & kPanelEditorUndoPending)
		{
			editorState &= ~kPanelEditorUndoPending;
			AddOperation(new WarpPanelOperation(&selectionList));
		}

		Vector3D worldDelta = position - anchorPosition;
		EditorWidgetManipulator *manipulator = selectionList.First();
		while (manipulator)
		{
			Widget *widget = manipulator->GetTargetWidget();
			WidgetType type = widget->GetWidgetType();

			if (type == kWidgetImage)
			{
				const Vector2D& size = widget->GetWidgetSize();
				float w = size.x;
				float h = size.y;

				Vector3D delta = Inverse(widget->GetWidgetTransform()) * worldDelta;
				if (InterfaceMgr::GetShiftKey())
				{
					if (Fabs(delta.y) > Fabs(delta.x))
					{
						delta.x = 0.0F;
					}
					else
					{
						delta.y = 0.0F;
					}
				}

				const Vector2D& scale = manipulator->GetOriginalScale();
				float sx = Log(scale.x) * K::one_over_ln_2 * w - delta.x;
				float sy = Log(scale.y) * K::one_over_ln_2 * h + delta.y;

				if (editorFlags & kPanelEditorSnapToGrid)
				{
					sx = SnapToGrid(sx);
					sy = SnapToGrid(sy);
				}

				Vector2D newScale(Exp(sx / w * K::ln_2), Exp(sy / h * K::ln_2));
				static_cast<ImageWidget *>(widget)->SetImageScale(newScale);
			}
			else if (type == kWidgetText)
			{
				float scale = manipulator->GetOriginalScale().y;
				float s = Log(scale) * K::one_over_ln_2 + worldDelta.y * 0.015625F;
				if (editorFlags & kPanelEditorSnapToGrid)
				{
					s = SnapToGrid(s * 64.0F) * 0.015625F;
				}

				float newScale = Clamp(Exp(s * K::ln_2), 0.25F, 4.0F);
				static_cast<TextWidget *>(widget)->SetTextScale(newScale);
			}

			manipulator = manipulator->Next();
		}
	}
}

void PanelEditor::ResetAllMutators(void) const
{
	const Widget *widget = rootWidget->GetFirstSubnode();
	while (widget)
	{
		Mutator *mutator = widget->GetFirstMutator();
		while (mutator)
		{
			mutator->Reset();
			mutator = mutator->Next();
		}

		widget = widget->Next();
	}
}

void PanelEditor::InvalidateManipulators(void) const
{
	EditorWidgetManipulator *manipulator = selectionList.First();
	while (manipulator)
	{
		manipulator->Invalidate();
		manipulator = manipulator->Next();
	}
}

bool PanelEditor::BoxIntersectsWidget(const Point3D& p1, const Point3D& p2, const Widget *widget)
{
	Point3D		vertex[2][8];
	int8		location[8];

	const Transform4D& transform = widget->GetWorldTransform();
	const Vector2D& size = widget->GetWidgetSize();

	const Point3D& q = transform.GetTranslation();
	Vector3D dx = transform[0] * size.x;
	Vector3D dy = transform[1] * size.y;

	vertex[0][0] = q;
	vertex[0][1] = q + dy;
	vertex[0][2] = q + dx + dy;
	vertex[0][3] = q + dx;

	float sx = (p2.x > p1.x) ? 1.0F : -1.0F;
	float sy = (p2.y > p1.y) ? 1.0F : -1.0F;

	int32 vertexCount = Math::ClipPolygon(4, vertex[0], Antivector4D(sx, 0.0F, 0.0F, -sx * p1.x), location, vertex[1]);
	if (vertexCount == 0)
	{
		return (false);
	}

	vertexCount = Math::ClipPolygon(vertexCount, vertex[1], Antivector4D(-sx, 0.0F, 0.0F, sx * p2.x), location, vertex[0]);
	if (vertexCount == 0)
	{
		return (false);
	}

	vertexCount = Math::ClipPolygon(vertexCount, vertex[0], Antivector4D(0.0F, sy, 0.0F, -sy * p1.y), location, vertex[1]);
	if (vertexCount == 0)
	{
		return (false);
	}

	vertexCount = Math::ClipPolygon(vertexCount, vertex[1], Antivector4D(0.0F, -sy, 0.0F, sy * p2.y), location, vertex[0]);
	return (vertexCount != 0);
}

void PanelEditor::BeginTool(const Point3D& p)
{
	previousPoint = p;
	anchorPoint = p;

	int32 tool = trackingTool;
	if ((tool >= kPanelToolWidgetSelect) && (tool <= kPanelToolTextureScale))
	{
		bool shift = InterfaceMgr::GetShiftKey();
		anchorPosition = ViewportToWorldPosition(p);
		previousPosition = anchorPosition;

		Widget *handleWidget = nullptr;
		if ((tool == kPanelToolWidgetRotate) || (tool == kPanelToolWidgetResize))
		{
			handleWidget = PickWidgetHandle(anchorPosition, &handleIndex);
		}

		Widget *widget = handleWidget;
		if (!widget)
		{
			widget = PickWidget(anchorPosition);
			if (widget)
			{
				if (widget->GetManipulator()->Selected())
				{
					if (shift)
					{
						UnselectWidget(widget);
					}
				}
				else
				{
					if (!shift)
					{
						UnselectAll();
					}

					SelectWidget(widget);
				}
			}
			else
			{
				if (!shift)
				{
					UnselectAll();
				}
			}
		}

		pickedWidget = widget;

		if ((tool == kPanelToolWidgetSelect) || (!widget))
		{
			if (widget)
			{
				WidgetManipulator *manipulator = widget->GetManipulator();
				manipulator->SetManipulatorState(manipulator->GetManipulatorState() | kWidgetManipulatorTempSelected);
			}

			trackingTool = kPanelToolWidgetSelect;
			toolTracking = true;
		}
		else
		{
			switch (tool)
			{
				case kPanelToolWidgetMove:
				{
					EditorWidgetManipulator *manipulator = selectionList.First();
					while (manipulator)
					{
						manipulator->SaveOriginalPosition();
						manipulator = manipulator->Next();
					}

					editorState |= kPanelEditorUndoPending;
					toolTracking = true;
					break;
				}

				case kPanelToolWidgetRotate:
				{
					if (handleWidget)
					{
						EditorWidgetManipulator *manipulator = selectionList.First();
						while (manipulator)
						{
							manipulator->SaveOriginalTransform();
							manipulator->SaveOriginalSize();
							manipulator = manipulator->Next();
						}

						if (handleWidget->GetWidgetType() != kWidgetGeneric)
						{
							const Vector2D& size = handleWidget->GetWidgetSize();
							const Transform4D& transform = handleWidget->GetWidgetTransform();
							rotationCenter = transform.GetTranslation() + transform[0] * (size.x * 0.5F) + transform[1] * (size.y * 0.5F);
						}
						else
						{
							Box2D box = GetManipulator(handleWidget)->GetWidgetBounds();
							rotationCenter = box.GetCenter();
						}

						editorState |= kPanelEditorUndoPending;
						previousAngle = 0.0F;
						toolTracking = true;
					}

					break;
				}

				case kPanelToolWidgetResize:
				{
					if (handleWidget)
					{
						EditorWidgetManipulator *manipulator = selectionList.First();
						while (manipulator)
						{
							manipulator->SaveOriginalPosition();
							manipulator->SaveOriginalSize();
							manipulator = manipulator->Next();
						}

						editorState |= kPanelEditorUndoPending;
						toolTracking = true;
					}

					break;
				}

				case kPanelToolTextureOffset:
				{
					EditorWidgetManipulator *manipulator = selectionList.First();
					while (manipulator)
					{
						if (manipulator->GetTargetWidget()->GetWidgetType() == kWidgetImage)
						{
							manipulator->SaveOriginalOffset();
						}

						manipulator = manipulator->Next();
					}

					editorState |= kPanelEditorUndoPending;
					toolTracking = true;
					break;
				}

				case kPanelToolTextureScale:
				{
					EditorWidgetManipulator *manipulator = selectionList.First();
					while (manipulator)
					{
						WidgetType type = manipulator->GetTargetWidget()->GetWidgetType();
						if (type == kWidgetImage)
						{
							manipulator->SaveOriginalImageScale();
						}
						else if (type == kWidgetText)
						{
							manipulator->SaveOriginalTextScale();
						}

						manipulator = manipulator->Next();
					}

					editorState |= kPanelEditorUndoPending;
					toolTracking = true;
					break;
				}
			}
		}
	}
	else
	{
		toolTracking = true;
	}
}

void PanelEditor::TrackTool(const Point3D& p)
{
	int32 tool = trackingTool;
	if (tool == kPanelToolWidgetSelect)
	{
		float dx = Fabs(p.x - anchorPoint.x);
		float dy = Fabs(p.y - anchorPoint.y);

		if ((boxSelectFlag) || (dx > 3.0F) || (dy > 3.0F))
		{
			boxSelectFlag = true;
			AutoScroll(p);

			Point3D position = ViewportToWorldPosition(p);
			if (position != previousPosition)
			{
				previousPosition = position;
				dragRect.Build(anchorPosition.GetPoint2D(), position.GetPoint2D(), panelViewport->GetOrthoScale().x);

				if ((dx >= 1.0F) && (dy >= 1.0F))
				{
					UnselectAllTemp();

					const Widget *widget = rootWidget->GetFirstSubnode();
					while (widget)
					{
						WidgetManipulator *manipulator = widget->GetManipulator();
						if (widget->GetWidgetType() != kWidgetGeneric)
						{
							if ((manipulator) && ((manipulator->GetManipulatorState() & (kWidgetManipulatorHidden | kWidgetManipulatorLocked)) == 0))
							{
								if (BoxIntersectsWidget(anchorPosition, position, widget))
								{
									Widget *super = widget->GetSuperNode();
									if (super == rootWidget)
									{
										if (!manipulator->Selected())
										{
											SelectWidget(widget);
											manipulator->SetManipulatorState(manipulator->GetManipulatorState() | kWidgetManipulatorTempSelected);
										}
									}
									else
									{
										manipulator = super->GetManipulator();
										if (!manipulator->Selected())
										{
											SelectWidget(super);
											manipulator->SetManipulatorState(manipulator->GetManipulatorState() | kWidgetManipulatorTempSelected);
										}
									}
								}
							}
						}
						else
						{
							if ((manipulator) && ((manipulator->GetManipulatorState() & (kWidgetManipulatorHidden | kWidgetManipulatorLocked)) != 0))
							{
								widget = rootWidget->GetNextLevelNode(widget);
								continue;
							}
						}

						widget = rootWidget->GetNextNode(widget);
					}
				}
			}
		}
	}
	else if (tool == kPanelToolWidgetMove)
	{
		AutoScroll(p);
		MoveSelectedWidgets(p);
	}
	else if (tool == kPanelToolWidgetRotate)
	{
		AutoScroll(p);
		RotateSelectedWidgets(p);
	}
	else if (tool == kPanelToolWidgetResize)
	{
		AutoScroll(p);
		ResizeSelectedWidgets(p);
	}
	else if (tool == kPanelToolTextureOffset)
	{
		AutoScroll(p);
		OffsetSelectedWidgets(p);
	}
	else if (tool == kPanelToolTextureScale)
	{
		AutoScroll(p);
		ScaleSelectedWidgets(p);
	}
	else if (tool == kPanelToolViewportScroll)
	{
		float dx = previousPoint.x - p.x;
		float dy = previousPoint.y - p.y;
		if ((dx != 0.0F) || (dy != 0.0F))
		{
			OrthoCamera *camera = panelViewport->GetViewportCamera();
			const Vector2D& scale = panelViewport->GetOrthoScale();
			camera->SetNodePosition(camera->GetNodePosition() + Vector3D(Floor(dx * scale.x + 0.5F), Floor(dy * scale.y + 0.5F), 0.5F));
			ClampViewport();
		}
	}
	else if (tool == kPanelToolViewportZoom)
	{
		float dy = p.y - previousPoint.y;
		if (dy != 0.0F)
		{
			UpdateViewportScale(panelViewport->GetOrthoScale().x * Exp(dy * 0.02F));
			InvalidateManipulators();
			ClampViewport();
		}
	}

	previousPoint = p;
}

void PanelEditor::EndTool(const Point3D& p)
{
	int32 tool = trackingTool;
	if (tool == kPanelToolWidgetSelect)
	{
		EditorWidgetManipulator *manipulator = selectionList.First();
		while (manipulator)
		{
			manipulator->SetManipulatorState(manipulator->GetManipulatorState() & ~kWidgetManipulatorTempSelected);
			manipulator = manipulator->Next();
		}

		boxSelectFlag = false;
	}
	else if (tool == kPanelToolWidgetMove)
	{
		MoveSelectedWidgets(p);
	}
	else if (tool == kPanelToolWidgetRotate)
	{
		RotateSelectedWidgets(p);
	}
	else if (tool == kPanelToolWidgetResize)
	{
		ResizeSelectedWidgets(p);
	}
	else if (tool == kPanelToolTextureOffset)
	{
		OffsetSelectedWidgets(p);
	}
	else if (tool == kPanelToolTextureScale)
	{
		ScaleSelectedWidgets(p);
	}
}

void PanelEditor::BeginWidget(const Point3D& p)
{
	WidgetType type = currentWidgetReg->GetWidgetType();
	switch (type)
	{
		case kWidgetLine:

			creationWidget = new LineWidget(Zero2D, kLineSolid, K::black);
			break;

		case kWidgetBorder:

			creationWidget = new BorderWidget(Zero2D, kLineSolid, K::black);
			break;

		case kWidgetText:

			creationWidget = new TextWidget(currentWidgetReg->GetWidgetName(), "font/Gui");
			break;

		case kWidgetEditText:

			creationWidget = new EditTextWidget(Zero2D, 255, "font/Gui");
			break;

		case kWidgetPassword:

			creationWidget = new PasswordWidget(Zero2D, PasswordWidget::kMaxPasswordLength, "font/Gui");
			break;

		case kWidgetQuad:

			creationWidget = new QuadWidget(Zero2D, K::white);
			break;

		case kWidgetImage:

			creationWidget = new ImageWidget(Zero2D, "C4/missing");
			break;

		case kWidgetCheck:

			creationWidget = new CheckWidget(Zero2D, currentWidgetReg->GetWidgetName(), "font/Gui");
			break;

		case kWidgetRadio:

			creationWidget = new RadioWidget(Zero2D, currentWidgetReg->GetWidgetName(), "font/Gui");
			break;

		case kWidgetPushButton:

			creationWidget = new PushButtonWidget(Zero2D, currentWidgetReg->GetWidgetName(), "font/Heading");
			break;

		case kWidgetIconButton:

			creationWidget = new IconButtonWidget(Zero2D, "C4/missing");
			break;

		case kWidgetTextButton:

			creationWidget = new TextButtonWidget(Zero2D, currentWidgetReg->GetWidgetName(), "font/Gui");
			break;

		case kWidgetHyperlink:

			creationWidget = new HyperlinkWidget("http://", "font/Gui");
			break;

		case kWidgetList:

			creationWidget = new ListWidget(Zero2D);
			break;

		case kWidgetTree:

			creationWidget = new TreeWidget(Zero2D);
			break;

		case kWidgetMultipane:

			creationWidget = new MultipaneWidget(Zero2D, "font/Gui");
			break;

		case kWidgetPopupMenu:

			creationWidget = new PopupMenuWidget(Zero2D, "font/Gui");
			break;

		case kWidgetMenuBar:

			creationWidget = new MenuBarWidget(Zero2D, "font/Gui");
			break;

		case kWidgetConfiguration:

			creationWidget = new ConfigurationWidget(Zero2D, 0.5F);
			break;

		case kWidgetPaint:

			creationWidget = new PaintWidget(Zero2D, Integer2D(128, 128));
			break;

		case kWidgetMovie:

			creationWidget = new MovieWidget(Zero2D, "");
			break;

		case kWidgetCamera:

			creationWidget = new CameraWidget(Zero2D);
			break;

		default:

			creationWidget = Widget::New(type);
			break;
	}

	EditorWidgetManipulator::Install(creationWidget);
	AddOperation(new CreatePanelOperation(creationWidget));

	Point3D q = ViewportToWorldPosition(p);
	if (editorFlags & kPanelEditorSnapToGrid)
	{
		q.x = SnapToGrid(q.x);
		q.y = SnapToGrid(q.y);
	}

	anchorPosition = q;
	previousSize.Set(0.0F, 0.0F);
	toolTracking = true;

	creationWidget->SetWidgetPosition(q);
	rootWidget->AppendNewSubnode(creationWidget);

	UnselectAll();
	SelectWidget(creationWidget);
}

bool PanelEditor::TrackWidget(const Point3D& p)
{
	AutoScroll(p);

	Point3D anchor = anchorPosition;
	Point3D position = ViewportToWorldPosition(p);

	float dx = position.x - anchor.x;
	float dy = position.y - anchor.y;

	float ax = Fabs(dx);
	float ay = Fabs(dy);

	if (InterfaceMgr::GetShiftKey())
	{
		ax = ay = Fmax(ax, ay);
		dx = (dx < 0.0F) ? -ax : ax;
		dy = (dy < 0.0F) ? -ax : ax;
	}

	if (editorFlags & kPanelEditorSnapToGrid)
	{
		dx = SnapToGrid(dx);
		dy = SnapToGrid(dy);
	}

	if ((dx != previousSize.x) || (dy != previousSize.y))
	{
		previousSize.Set(dx, dy);

		if (dx < 0.0F)
		{
			anchor.x += dx;
			dx = -dx;
		}

		if (dy < 0.0F)
		{
			anchor.y += dy;
			dy = -dy;
		}

		creationWidget->SetWidgetPosition(anchor);
		creationWidget->SetWidgetSize(Vector2D(dx, dy));
		creationWidget->Invalidate();

		editorState |= kPanelEditorUpdateTransformPage;
	}

	return ((dx != 0.0F) && (dy != 0.0F));
}

void PanelEditor::EndWidget(const Point3D& p)
{
	if (TrackWidget(p))
	{
		if (creationWidget->GetBaseWidgetType() == kWidgetText)
		{
			SetFocusWidget(textStringBox);
		}
	}
	else
	{
		UnselectAll();
		delete creationWidget;
		DeleteLastOperation();
	}
}

void PanelEditor::ViewportHandleMouseEvent(const MouseEventData *eventData, ViewportWidget *viewport, void *cookie)
{
	PanelEditor *panelEditor = static_cast<PanelEditor *>(cookie);

	EventType eventType = eventData->eventType;
	if ((eventType == kEventMouseDown) || (eventType == kEventMiddleMouseDown))
	{
		panelEditor->SetFocusWidget(nullptr);

		int32 mode = panelEditor->currentMode;
		int32 tool = panelEditor->currentTool;

		if ((eventType == kEventMiddleMouseDown) || (InterfaceMgr::GetOptionKey()))
		{
			mode = kPanelEditorModeTool;
			tool = kPanelToolViewportScroll;
		}

		panelEditor->trackingMode = mode;
		panelEditor->trackingTool = tool;
		panelEditor->toolTracking = false;

		if (mode == kPanelEditorModeTool)
		{
			panelEditor->BeginTool(eventData->mousePosition);
		}
		else
		{
			panelEditor->BeginWidget(eventData->mousePosition);
		}
	}
	else if ((eventType == kEventMouseUp) || (eventType == kEventMiddleMouseUp))
	{
		if (panelEditor->toolTracking)
		{
			if (panelEditor->trackingMode == kPanelEditorModeTool)
			{
				panelEditor->EndTool(eventData->mousePosition);
			}
			else
			{
				panelEditor->EndWidget(eventData->mousePosition);
			}

			panelEditor->toolTracking = false;
		}
	}
	else if (eventType == kEventMouseWheel)
	{
		if (!panelEditor->toolTracking)
		{
			OrthoViewportWidget *orthoViewport = static_cast<OrthoViewportWidget *>(viewport);
			panelEditor->UpdateViewportScale(orthoViewport->GetOrthoScale().x * Exp(eventData->wheelDelta.y * -0.16F), &eventData->mousePosition);
			panelEditor->InvalidateManipulators();
			panelEditor->ClampViewport();
		}
	}
}

void PanelEditor::ViewportTrackTask(const Point3D& position, ViewportWidget *viewport, void *cookie)
{
	PanelEditor *panelEditor = static_cast<PanelEditor *>(cookie);
	if (panelEditor->toolTracking)
	{
		if (panelEditor->trackingMode == kPanelEditorModeTool)
		{
			panelEditor->TrackTool(position);
		}
		else
		{
			panelEditor->TrackWidget(position);
		}
	}
}

void PanelEditor::ViewportRender(List<Renderable> *renderList, ViewportWidget *viewport, void *cookie)
{
	PanelEditor *panelEditor = static_cast<PanelEditor *>(cookie);

	const OrthoViewportWidget *orthoViewport = static_cast<OrthoViewportWidget *>(viewport);
	float scale = orthoViewport->GetOrthoScale().x;

	Widget *root = panelEditor->rootWidget;
	root->Update();

	Widget *widget = root->GetFirstSubnode();
	while (widget)
	{
		EditorWidgetManipulator *manipulator = GetManipulator(widget);
		if (manipulator)
		{
			unsigned_int32 state = manipulator->GetManipulatorState();
			if (state & kWidgetManipulatorBuild)
			{
				manipulator->SetManipulatorState(state & ~kWidgetManipulatorBuild);
				manipulator->Build(scale);
			}
		}

		widget = root->GetNextNode(widget);
	}

	if (panelEditor->editorFlags & kPanelEditorShowGridlines)
	{
		unsigned_int32 state = panelEditor->editorState;
		if (state & kPanelEditorUpdateGrid)
		{
			panelEditor->editorState = state & ~kPanelEditorUpdateGrid;

			const OrthoCamera *camera = orthoViewport->GetViewportCamera();
			const OrthoCameraObject *cameraObject = camera->GetObject();

			const Point3D& position = camera->GetNodePosition();
			float xmin = cameraObject->GetOrthoRectLeft() + position.x;
			float xmax = cameraObject->GetOrthoRectRight() + position.x;
			float ymin = cameraObject->GetOrthoRectTop() + position.y;
			float ymax = cameraObject->GetOrthoRectBottom() + position.y;

			panelEditor->viewportGrid.SetBoundingBox(Zero2D, panelEditor->rootWidget->GetWidgetSize());
			panelEditor->viewportGrid.Build(Point2D(xmin, ymin), Point2D(xmax, ymax), scale);
		}

		renderList->Append(&panelEditor->viewportGrid);
	}

	root->RenderTree(renderList);

	EditorWidgetManipulator *manipulator = panelEditor->selectionList.First();
	while (manipulator)
	{
		renderList->Append(manipulator->GetWidgetRenderable());
		manipulator = manipulator->Next();
	}

	if (panelEditor->boxSelectFlag)
	{
		renderList->Append(&panelEditor->dragRect);
	}
}

void PanelEditor::HandleToolButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		SetFocusWidget(nullptr);

		for (machine a = 0; a < kPanelToolCount; a++)
		{
			if (widget == toolButton[a])
			{
				if ((currentMode != kPanelEditorModeTool) || (currentTool != a))
				{
					if (currentMode == kPanelEditorModeTool)
					{
						toolButton[currentTool]->SetValue(0);
					}
					else
					{
						widgetsPage->UnselectWidgetTool();
					}

					currentMode = kPanelEditorModeTool;
					currentTool = a;
				}

				break;
			}
		}
	}
}

void PanelEditor::HandleTextBoxEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		char	string[256];

		const char *text = textStringBox->GetText();

		bool control = false;
		int32 x = 0;
		for (;; text++)
		{
			char c = *text;
			if (c == 0)
			{
				break;
			}

			if (control)
			{
				if (c == 'n')
				{
					c = 10;
				}

				string[x] = c;
				control = false;
				x++;
			}
			else if (c == '\\')
			{
				control = true;
			}
			else
			{
				string[x] = c;
				x++;
			}
		}

		string[x] = 0;

		Widget *textWidget = selectionList.First()->GetTargetWidget();
		static_cast<TextWidget *>(textWidget)->SetText(string);

		editorState |= kPanelEditorUnsaved;
	}
}

bool PanelEditor::SavePanel(void)
{
	File			file;
	ResourcePath	path;

	TheResourceMgr->GetGenericCatalog()->GetResourcePath(PanelResource::GetDescriptor(), resourceName, &resourceLocation, &path);
	TheResourceMgr->CreateDirectoryPath(path);

	FileResult result = file.Open(path, kFileCreate);
	if (result == kFileOkay)
	{
		result = rootWidget->PackTree(&file);
	}

	if (result == kFileOkay)
	{
		editorState &= ~kPanelEditorUnsaved;
		return (true);
	}

	unsigned_int32 id = 'NSAV';

	if (result == kFileLocked)
	{
		id = 'LOCK';
	}
	else if (result == kFileAccessDenied)
	{
		id = 'ACES';
	}
	else if (result == kFileWriteProtected)
	{
		id = 'PROT';
	}
	else if (result == kFileDiskFull)
	{
		id = 'DFUL';
	}

	const StringTable *table = TheWorldEditor->GetStringTable();
	DisplayError(table->GetString(StringID('PANL', 'ERRR', id)));

	return (false);
}

void PanelEditor::SavePickerProc(FilePicker *picker, void *cookie)
{
	ResourceName name(picker->GetFileName());
	name[Text::GetResourceNameLength(name)] = 0;
	int32 len = Text::GetPrefixDirectoryLength(name);

	PanelEditor *panelEditor = static_cast<PanelEditor *>(cookie);
	panelEditor->resourceName = &name[len];
	panelEditor->resourceLocation.GetPath().Set(name, len - 1);
	panelEditor->SavePanel();

	ResourcePath title(panelEditor->resourceName);
	panelEditor->SetWindowTitle(title += PanelResource::GetDescriptor()->GetExtension());
	panelEditor->SetStripTitle(&title[Text::GetDirectoryPathLength(title)]);
}

void PanelEditor::CloseDialogComplete(Dialog *dialog, void *cookie)
{
	PanelEditor *panelEditor = static_cast<PanelEditor *>(cookie);

	int32 status = dialog->GetDialogStatus();
	if (status == kDialogOkay)
	{
		if ((panelEditor->targetPanel) || (panelEditor->resourceName[0] != 0))
		{
			panelEditor->HandleSavePanelMenuItem(nullptr, nullptr);
			panelEditor->Window::Close();
		}
		else
		{
			panelEditor->HandleSavePanelAsMenuItem(nullptr, nullptr);
		}
	}
	else if (status == kDialogIgnore)
	{
		panelEditor->Window::Close();
	}
}

void PanelEditor::DisplayError(const char *string)
{
	const StringTable *table = TheWorldEditor->GetStringTable();

	const char *title = table->GetString(StringID('PANL', 'ERRR'));
	const char *okayText = table->GetString(StringID('BTTN', 'OKAY'));

	Dialog *dialog = new Dialog(Vector2D(342.0F, 120.0F), title, okayText);

	ImageWidget *image = new ImageWidget(Vector2D(64.0F, 64.0F), "C4/error");
	image->SetWidgetPosition(Point3D(12.0F, 12.0F, 0.0F));
	dialog->AppendSubnode(image);

	TextWidget *text = new TextWidget(Vector2D(242.0F, 0.0F), string, "font/Gui");
	text->SetTextFlags(kTextWrapped);
	text->SetWidgetPosition(Point3D(88.0F, 16.0F, 0.0F));
	dialog->AppendSubnode(text);

	AddSubwindow(dialog);
}

void PanelEditor::Move(void)
{
	if ((TheInterfaceMgr->GetActiveWindow() == this) && (!TheInterfaceMgr->GetActiveMenu()))
	{
		bool toolCursor = toolTracking;
		if (!toolCursor)
		{
			const Point3D& position = TheInterfaceMgr->GetCursorPosition();
			Vector2D vp = position.GetVector2D() - panelViewport->GetWorldPosition().GetVector2D();
			if ((vp.x >= 0.0F) && (vp.x < panelViewport->GetWidgetSize().x) && (vp.y >= 0.0F) && (vp.y < panelViewport->GetWidgetSize().y))
			{
				toolCursor = true;
			}
		}

		int32 cursorIndex = kEditorCursorArrow;
		if (toolCursor)
		{
			int32	mode;
			int32	tool;

			if (!toolTracking)
			{
				if (InterfaceMgr::GetOptionKey())
				{
					mode = kPanelEditorModeTool;
					tool = kPanelToolViewportScroll;
				}
				else
				{
					mode = currentMode;
					tool = currentTool;
				}
			}
			else
			{
				mode = trackingMode;
				tool = trackingTool;
			}

			if (mode == kPanelEditorModeTool)
			{
				if (tool == kPanelToolViewportScroll)
				{
					cursorIndex = (toolTracking) ? kEditorCursorDrag : kEditorCursorHand;
				}
				else if (tool == kPanelToolViewportZoom)
				{
					cursorIndex = kEditorCursorGlass;
				}
			}
			else
			{
				cursorIndex = kEditorCursorCross;
			}
		}

		TheInterfaceMgr->SetCursor(TheWorldEditor->GetEditorCursor(cursorIndex));
	}

	if (editorState & kPanelEditorUpdateMenus)
	{
		if (selectionList.Empty())
		{
			for (machine a = 0; a < kPanelMenuItemCount; a++)
			{
				if (widgetMenuItemSelectFlag[a])
				{
					MenuItemWidget *widget = widgetMenuItem[a];
					widget->Disable();
				}
			}
		}
		else
		{
			bool groupEnable = true;
			bool ungroupEnable = true;
			bool imageEnable = false;

			const EditorWidgetManipulator *manipulator = selectionList.First();
			while (manipulator)
			{
				const Widget *widget = manipulator->GetTargetWidget();

				WidgetType type = widget->GetWidgetType();
				if (type == kWidgetGeneric)
				{
					groupEnable = false;
				}
				else
				{
					ungroupEnable = false;
					if (widget->GetSuperNode() != rootWidget)
					{
						groupEnable = false;
					}

					if ((type == kWidgetImage) || (type == kWidgetIconButton))
					{
						imageEnable = true;
					}
				}

				manipulator = manipulator->Next();
			}

			for (machine a = 0; a < kPanelMenuItemCount; a++)
			{
				if (widgetMenuItemSelectFlag[a])
				{
					MenuItemWidget *menuItem = widgetMenuItem[a];
					if ((imageEnable) || (!widgetMenuItemImageFlag[a]))
					{
						menuItem->Enable();
					}
					else
					{
						menuItem->Disable();
					}
				}
			}

			if (groupEnable)
			{
				widgetMenuItem[kPanelMenuGroupSelection]->Enable();
			}
			else
			{
				widgetMenuItem[kPanelMenuGroupSelection]->Disable();
			}

			if (ungroupEnable)
			{
				widgetMenuItem[kPanelMenuUngroupSelection]->Enable();
			}
			else
			{
				widgetMenuItem[kPanelMenuUngroupSelection]->Disable();
			}
		}
	}

	if (editorState & kPanelEditorUpdateSettings)
	{
		bool singleWidget = false;

		const EditorWidgetManipulator *manipulator = selectionList.First();
		if (manipulator)
		{
			singleWidget = !manipulator->Next();
			UpdateSettings();
		}

		if ((manipulator) && (singleWidget))
		{
			if (targetNode)
			{
				widgetMenuItem[kPanelMenuEditScript]->Enable();
				widgetMenuItem[kPanelMenuDeleteScript]->Enable();
			}
			else
			{
				widgetMenuItem[kPanelMenuEditScript]->Disable();
				widgetMenuItem[kPanelMenuDeleteScript]->Disable();
			}

			UpdateMutators(manipulator);
		}
		else
		{
			textStringBox->Hide();
			textStringText->Hide();
			mutatorListWidget->PurgeListItems();
			mutatorMenu->SetSelection(kWidgetValueNone);
			mutatorMenu->Disable();
			addMutatorButton->Disable();
			removeMutatorButton->Disable();

			widgetMenuItem[kPanelMenuEditScript]->Disable();

			if (!manipulator)
			{
				widgetConfigurationWidget->ReleaseConfiguration();
			}

			mutatorConfigurationWidget->ReleaseConfiguration();
		}
	}

	if (editorState & kPanelEditorUpdateTransformPage)
	{
		const EditorWidgetManipulator *manipulator = selectionList.First();
		if ((manipulator) && (!manipulator->Next()))
		{
			transformPage->UpdateTransform(manipulator->GetTargetWidget());
		}
		else
		{
			transformPage->UpdateTransform(nullptr);
		}
	}

	editorState &= ~(kPanelEditorUpdateMenus | kPanelEditorUpdateSettings | kPanelEditorUpdateTransformPage);

	rootWidget->Move();
	Window::Move();
}

void PanelEditor::EnterForeground(void)
{
	Window::EnterForeground();

	if (editorClipboard.GetFirstSubnode())
	{
		widgetMenuItem[kPanelMenuPaste]->Enable();
	}
}

void PanelEditor::EnterBackground(void)
{
	Window::EnterBackground();
	TheInterfaceMgr->SetCursor(nullptr);
}

bool PanelEditor::HandleKeyboardEvent(const KeyboardEventData *eventData)
{
	EventType eventType = eventData->eventType;

	if (eventType == kEventKeyDown)
	{
		if (toolTracking)
		{
			return (true);
		}

		unsigned_int32 keyCode = eventData->keyCode;
		if (keyCode == kKeyCodeEscape)
		{
			SetFocusWidget(nullptr);
			return (true);
		}

		if (!GetFocusWidget())
		{
			if ((keyCode >= '1') && (keyCode <= '7'))
			{
				if (keyCode == '1')
				{
					toolButton[kPanelToolWidgetSelect]->SetValue(1, true);
				}
				else if (keyCode == '2')
				{
					toolButton[kPanelToolWidgetMove]->SetValue(1, true);
				}
				else if (keyCode == '3')
				{
					toolButton[kPanelToolWidgetRotate]->SetValue(1, true);
				}
				else if (keyCode == '4')
				{
					toolButton[kPanelToolWidgetResize]->SetValue(1, true);
				}
				else if (keyCode == '6')
				{
					toolButton[kPanelToolViewportScroll]->SetValue(1, true);
				}
				else if (keyCode == '7')
				{
					toolButton[kPanelToolViewportZoom]->SetValue(1, true);
				}

				return (true);
			}
			else if ((keyCode == kKeyCodeDelete) || (keyCode == kKeyCodeBackspace))
			{
				if (!selectionList.Empty())
				{
					HandleClearMenuItem(nullptr, nullptr);
				}

				return (true);
			}
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

void PanelEditor::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	EventType eventType = eventData->eventType;

	if (eventType == kEventWidgetActivate)
	{
		if (widget == addMutatorButton)
		{
			const MutatorRegistration *registration = validMutatorArray[mutatorMenu->GetSelection()];
			Mutator *mutator = Mutator::New(registration->GetMutatorType());
			currentMutator = mutator;

			Widget *targetWidget = selectionList.First()->GetTargetWidget();
			targetWidget->AddMutator(mutator);

			rootWidget->RemoveMovingWidget(targetWidget);
			targetWidget->Preprocess();

			mutatorListWidget->AppendListItem(registration->GetMutatorName());
			mutatorListWidget->SelectListItem(mutatorListWidget->GetListItemCount() - 1);

			mutatorConfigurationWidget->ReleaseConfiguration();
			mutatorConfigurationWidget->BuildConfiguration(mutator);

			removeMutatorButton->Enable();

			editorState |= kPanelEditorUnsaved;
		}
		else if (widget == removeMutatorButton)
		{
			delete currentMutator;

			Widget *targetWidget = selectionList.First()->GetTargetWidget();
			targetWidget->SetWidgetColor(targetWidget->GetWidgetColor());

			Widget *listItem = mutatorListWidget->GetFirstSelectedListItem();
			mutatorListWidget->RemoveListItem(listItem);
			delete listItem;

			mutatorConfigurationWidget->ReleaseConfiguration();
			removeMutatorButton->Disable();

			editorState |= kPanelEditorUnsaved;
		}
	}
	else if (eventType == kEventWidgetChange)
	{
		if (widget == mutatorListWidget)
		{
			mutatorConfigurationWidget->ReleaseConfiguration();

			int32 selection = mutatorListWidget->GetFirstSelectedIndex();
			if (selection >= 0)
			{
				const Widget *targetWidget = selectionList.First()->GetTargetWidget();
				Mutator *mutator = targetWidget->GetFirstMutator();

				while (selection != 0)
				{
					mutator = mutator->Next();
					selection--;
				}

				currentMutator = mutator;
				mutatorConfigurationWidget->BuildConfiguration(mutator);

				removeMutatorButton->Enable();
			}
		}
	}
}

void PanelEditor::Close(void)
{
	if (editorState & kPanelEditorUnsaved)
	{
		const StringTable *table = TheWorldEditor->GetStringTable();

		const char *title = table->GetString(StringID('PANL', 'SAVE'));
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

		String<127> confirmText(table->GetString(StringID('PANL', 'CFRM')));
		if (resourceName[0] != 0)
		{
			confirmText += resourceName;
		}
		else
		{
			confirmText += table->GetString(StringID('PANL', 'PANL'));
		}

		confirmText += '?';

		TextWidget *text = new TextWidget(Vector2D(242.0F, 0.0F), confirmText, "font/Gui");
		text->SetTextFlags(kTextWrapped);
		text->SetWidgetPosition(Point3D(88.0F, 16.0F, 0.0F));
		dialog->AppendSubnode(text);

		AddSubwindow(dialog);
	}
	else
	{
		Window::Close();
	}
}

// ZYUQURM
