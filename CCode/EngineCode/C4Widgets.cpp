#include "C4Widgets.h"
#include "C4Viewports.h"
#include "C4Paint.h"
#include "C4Movies.h"
#include "C4Graphics.h"


using namespace C4;


namespace
{
	enum
	{
		kTagAlignLeft			= 'LEFT',
		kTagAlignCenter			= 'CENT',
		kTagAlignRight			= 'RGHT',
		kTagResetFormat			= 'INIT',
		kTagSaveFormat			= 'SAVE',
		kTagRestoreFormat		= 'RSTR'
	};


	enum
	{
		kEditTextCaretVisible	= 1 << 0,
		kEditTextCaretHidden	= 1 << 1,
		kEditTextShowCaret		= 1 << 2,
		kEditTextCaretMemory	= 1 << 3,
		kEditTextSelection		= 1 << 4,
		kEditTextDragSelect		= 1 << 5,
		kEditTextSelectWord		= 1 << 6
	};
}


const WidgetColorType Widget::widgetColorType[kWidgetColorCount] =
{
	kWidgetColorDefault, kWidgetColorText, kWidgetColorLine, kWidgetColorBorder, kWidgetColorBackground,
	kWidgetColorButton, kWidgetColorHilite, kWidgetColorFocus, kWidgetColorCaret
};


const TextureHeader LineWidget::lineTextureHeader =
{
	kTexture2D,
	kTextureForceHighQuality,
	kTextureSemanticDiffuse,
	kTextureSemanticTransparency,
	kTextureI8,
	8, 16, 1,
	{kTextureRepeat, kTextureClamp, kTextureClamp},
	1
};


const unsigned_int8 LineWidget::lineTextureImage[128] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


char TextWidget::emptyString[1] = "";


const Type EditTextWidget::filterTypeTable[kFilterTypeCount] =
{
	kFilterNumber, kFilterSignedNumber, kFilterFloatingPoint, kFilterAlphanumeric, kFilterIdentifier
};

EditTextWidget::FilterProc *const EditTextWidget::filterProcTable[kFilterTypeCount] =
{
	&EditTextWidget::NumberFilter, &EditTextWidget::SignedNumberFilter, &EditTextWidget::FloatNumberFilter, &EditTextWidget::AlphanumericFilter, &EditTextWidget::IdentifierFilter
};


SharedVertexBuffer WindowFrameWidget::indexBuffer(kVertexBufferIndex | kVertexBufferStatic);
SharedVertexBuffer BalloonFrameWidget::indexBuffer(kVertexBufferIndex | kVertexBufferStatic);
SharedVertexBuffer PageFrameWidget::indexBuffer(kVertexBufferIndex | kVertexBufferStatic);


namespace C4
{
	template class Registrable<Widget, WidgetRegistration>;
}


WidgetRegistration::WidgetRegistration(WidgetType type, const char *name, const char *icon, unsigned_int32 flags) : Registration<Widget, WidgetRegistration>(type)
{
	widgetFlags = flags;
	widgetName = name;
	iconTextureName = icon;
}

WidgetRegistration::~WidgetRegistration()
{
}


WidgetManipulator::WidgetManipulator(Widget *widget)
{
	targetWidget = widget;
	manipulatorState = 0;
}

WidgetManipulator::~WidgetManipulator()
{
}

void WidgetManipulator::Invalidate(void)
{
}


Widget::Widget(WidgetType type, const Vector2D& size)
{
	widgetType = type;
	baseWidgetType = kWidgetGeneric;

	widgetKey[0] = 0;
	widgetState[0] = 0;
	widgetState[1] = 0;

	widgetUsage = 0;
	widgetAnchorX = kWidgetAnchorNone;
	widgetAnchorY = kWidgetAnchorNone;

	buildFlag = false;
	activeUpdateFlags = kUpdateTransform | kUpdateBoundingBox;
	currentUpdateFlags = kUpdateTransform | kUpdateBoundingBox;
	subtreeUpdateFlags = 0;

	defaultColorType = kWidgetColorDefault;
	widgetColor.Set(1.0F, 1.0F, 1.0F);

	widgetSize = size;
	widgetTransform.SetIdentity();

	widgetBoundingBox.min.Set(0.0F, 0.0F);
	widgetBoundingBox.max.Set(0.0F, 0.0F);
	boundingBoxPointer = nullptr;

	panelController = nullptr;
	widgetManipulator = nullptr;
	scriptObject = nullptr;

	balloonType = kBalloonNone;
}

Widget::Widget(WidgetType type) : Widget(type, Zero2D)
{
}

Widget::Widget(const Widget& widget)
{
	widgetType = widget.widgetType;
	baseWidgetType = widget.baseWidgetType;

	widgetKey = widget.widgetKey;
	widgetState[0] = widget.widgetState[0];
	widgetState[1] = 0;

	widgetUsage = widget.widgetUsage;
	widgetAnchorX = widget.widgetAnchorX;
	widgetAnchorY = widget.widgetAnchorY;

	buildFlag = false;
	activeUpdateFlags = kUpdateTransform | kUpdateBoundingBox;
	currentUpdateFlags = kUpdateTransform | kUpdateBoundingBox;
	subtreeUpdateFlags = 0;

	defaultColorType = widget.defaultColorType;
	widgetColor = widget.widgetColor;

	widgetSize = widget.widgetSize;
	widgetTransform = widget.widgetTransform;

	widgetBoundingBox.min.Set(0.0F, 0.0F);
	widgetBoundingBox.max.Set(0.0F, 0.0F);
	boundingBoxPointer = nullptr;

	panelController = nullptr;
	widgetManipulator = nullptr;

	scriptObject = widget.scriptObject;
	if (scriptObject)
	{
		scriptObject->Retain();
	}

	const Mutator *mutator = widget.mutatorList.First();
	while (mutator)
	{
		Mutator *clone = mutator->Clone();
		AddMutator(clone);

		mutator = mutator->Next();
	}

	balloonType = widget.balloonType;
	balloonString = widget.balloonString;
}

Widget::~Widget()
{
	delete widgetBalloon.GetTarget();

	if (scriptObject)
	{
		scriptObject->Release();
	}

	delete widgetManipulator;

	unsigned_int32 flags = activeUpdateFlags & kUpdateBoundingBox;
	if (flags != 0)
	{
		Widget *widget = GetSuperNode();
		while (widget)
		{
			widget->currentUpdateFlags |= flags;
			widget->subtreeUpdateFlags |= flags;

			widget = widget->GetSuperNode();
		}
	}
}

Widget *Widget::New(WidgetType type)
{
	Type	data[2];

	data[0] = type;
	data[1] = 0;

	Unpacker unpacker(data);
	return (Create(unpacker));
}

Widget *Widget::Replicate(void) const
{
	return (new Widget(*this));
}

Widget *Widget::Clone(void) const
{
	Widget *widget = Replicate();

	const Widget *subnode = GetFirstSubnode();
	while (subnode)
	{
		if (!(subnode->GetWidgetState() & kWidgetNonpersistent))
		{
			widget->AppendSubnode(subnode->Clone());
		}

		subnode = subnode->Next();
	}

	return (widget);
}

void Widget::RegisterStandardWidgets(void)
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	static WidgetReg<LineWidget> lineRegistration(kWidgetLine, table->GetString(StringID('WDGT', kWidgetLine)), "PanelEditor/widget/Line");
	static WidgetReg<BorderWidget> borderRegistration(kWidgetBorder, table->GetString(StringID('WDGT', kWidgetBorder)), "PanelEditor/widget/Border");
	static WidgetReg<TextWidget> textRegistration(kWidgetText, table->GetString(StringID('WDGT', kWidgetText)), "PanelEditor/widget/Text");
	static WidgetReg<EditTextWidget> editTextRegistration(kWidgetEditText, table->GetString(StringID('WDGT', kWidgetEditText)), "PanelEditor/widget/EditText");
	static WidgetReg<PasswordWidget> passwordRegistration(kWidgetPassword, table->GetString(StringID('WDGT', kWidgetPassword)), "PanelEditor/widget/Password");
	static WidgetReg<QuadWidget> quadRegistration(kWidgetQuad, table->GetString(StringID('WDGT', kWidgetQuad)), "PanelEditor/widget/Quad");
	static WidgetReg<ImageWidget> imageRegistration(kWidgetImage, table->GetString(StringID('WDGT', kWidgetImage)), "PanelEditor/widget/Image");
	static WidgetReg<CheckWidget> checkRegistration(kWidgetCheck, table->GetString(StringID('WDGT', kWidgetCheck)), "PanelEditor/widget/Check");
	static WidgetReg<RadioWidget> radioRegistration(kWidgetRadio, table->GetString(StringID('WDGT', kWidgetRadio)), "PanelEditor/widget/Radio");
	static WidgetReg<PushButtonWidget> pushButtonRegistration(kWidgetPushButton, table->GetString(StringID('WDGT', kWidgetPushButton)), "PanelEditor/widget/PushButton");
	static WidgetReg<IconButtonWidget> iconButtonRegistration(kWidgetIconButton, table->GetString(StringID('WDGT', kWidgetIconButton)), "PanelEditor/widget/IconButton");
	static WidgetReg<TextButtonWidget> textButtonRegistration(kWidgetTextButton, table->GetString(StringID('WDGT', kWidgetTextButton)), "PanelEditor/widget/TextButton");
	static WidgetReg<HyperlinkWidget> hyperlinkRegistration(kWidgetHyperlink, table->GetString(StringID('WDGT', kWidgetHyperlink)), "PanelEditor/widget/Hyperlink");
	static WidgetReg<ColorWidget> colorRegistration(kWidgetColor, table->GetString(StringID('WDGT', kWidgetColor)), "PanelEditor/widget/Color");
	static WidgetReg<ProgressWidget> progressRegistration(kWidgetProgress, table->GetString(StringID('WDGT', kWidgetProgress)), "PanelEditor/widget/Progress");
	static WidgetReg<SliderWidget> sliderRegistration(kWidgetSlider, table->GetString(StringID('WDGT', kWidgetSlider)), "PanelEditor/widget/Slider");
	static WidgetReg<ScrollWidget> scrollRegistration(kWidgetScroll, table->GetString(StringID('WDGT', kWidgetScroll)), "PanelEditor/widget/Scroll");
	static WidgetReg<ListWidget> listRegistration(kWidgetList, table->GetString(StringID('WDGT', kWidgetList)), "PanelEditor/widget/List");
	static WidgetReg<TreeWidget> treeRegistration(kWidgetTree, table->GetString(StringID('WDGT', kWidgetTree)), "PanelEditor/widget/Tree");
	static WidgetReg<TableWidget> tableRegistration(kWidgetTable, table->GetString(StringID('WDGT', kWidgetTable)), "PanelEditor/widget/Table");
	static WidgetReg<MultipaneWidget> multipaneRegistration(kWidgetMultipane, table->GetString(StringID('WDGT', kWidgetMultipane)), "PanelEditor/widget/Multipane");
	static WidgetReg<PopupMenuWidget> popupMenuRegistration(kWidgetPopupMenu, table->GetString(StringID('WDGT', kWidgetPopupMenu)), "PanelEditor/widget/PopupMenu");
	static WidgetReg<MenuBarWidget> menuBarRegistration(kWidgetMenuBar, table->GetString(StringID('WDGT', kWidgetMenuBar)), "PanelEditor/widget/MenuBar");
	static WidgetReg<DividerWidget> dividerRegistration(kWidgetDivider, table->GetString(StringID('WDGT', kWidgetDivider)), "PanelEditor/widget/Divider");
	static WidgetReg<OrthoViewportWidget> orthoViewportRegistration(kWidgetOrthoViewport, table->GetString(StringID('WDGT', kWidgetOrthoViewport)), "PanelEditor/widget/Viewport");
	static WidgetReg<FrustumViewportWidget> frustumViewportRegistration(kWidgetFrustumViewport, table->GetString(StringID('WDGT', kWidgetFrustumViewport)), "PanelEditor/widget/Viewport");
	static WidgetReg<WorldViewportWidget> worldViewportRegistration(kWidgetWorldViewport, table->GetString(StringID('WDGT', kWidgetWorldViewport)), "PanelEditor/widget/World");
	static WidgetReg<ConfigurationWidget> configurationRegistration(kWidgetConfiguration, table->GetString(StringID('WDGT', kWidgetConfiguration)), "PanelEditor/widget/Configuration");
	static WidgetReg<PaintWidget> paintRegistration(kWidgetPaint, table->GetString(StringID('WDGT', kWidgetPaint)), "PanelEditor/widget/Paint");
	static WidgetReg<MovieWidget> movieRegistration(kWidgetMovie, table->GetString(StringID('WDGT', kWidgetMovie)), "PanelEditor/widget/Movie");
	static WidgetReg<CameraWidget> cameraRegistration(kWidgetCamera, table->GetString(StringID('WDGT', kWidgetCamera)), "PanelEditor/widget/Camera", kWidgetPanelOnly);
}

void Widget::Detach(void)
{
	ClearFocus();
	ListElement<Widget>::Detach();

	Widget *widget = GetSuperNode();
	if (widget)
	{
		unsigned_int32 flags = activeUpdateFlags & kUpdateBoundingBox;
		if (flags != 0)
		{
			do
			{
				widget->currentUpdateFlags |= flags;
				widget->subtreeUpdateFlags |= flags;

				widget = widget->GetSuperNode();
			} while (widget);
		}

		Tree<Widget>::Detach();
	}
}

void Widget::AppendSubnode(Widget *node)
{
	node->Detach();
	Tree<Widget>::AppendSubnode(node);
	node->Invalidate();
}

void Widget::PrependSubnode(Widget *node)
{
	node->Detach();
	Tree<Widget>::PrependSubnode(node);
	node->Invalidate();
}

void Widget::InsertSubnodeBefore(Widget *node, Widget *before)
{
	node->Detach();
	Tree<Widget>::InsertSubnodeBefore(node, before);
	node->Invalidate();
}

void Widget::InsertSubnodeAfter(Widget *node, Widget *after)
{
	node->Detach();
	Tree<Widget>::InsertSubnodeAfter(node, after);
	node->Invalidate();
}

void Widget::RemoveSubnode(Widget *node)
{
	ClearFocus();

	unsigned_int32 flags = node->GetActiveUpdateFlags() & kUpdateBoundingBox;
	if (flags != 0)
	{
		Widget *widget = this;
		do
		{
			widget->currentUpdateFlags |= flags;
			widget->subtreeUpdateFlags |= flags;

			widget = widget->GetSuperNode();
		} while (widget);
	}

	Tree<Widget>::RemoveSubnode(node);
}

void Widget::PackType(Packer& data) const
{
	data << widgetType;
}

void Widget::Prepack(List<Object> *linkList) const
{
	if (scriptObject)
	{
		linkList->Append(scriptObject);
	}
}

void Widget::Pack(Packer& data, unsigned_int32 packFlags) const
{
	data << ChunkHeader('SUPR', 4);
	const Widget *superNode = GetSuperNode();
	int32 index = ((superNode) && (superNode->GetBaseWidgetType() != kWidgetRoot)) ? superNode->widgetIndex : -1;
	data << index;

	if (widgetKey[0] != 0)
	{
		PackHandle handle = data.BeginChunk('KEY ');
		data << widgetKey;
		data.EndChunk(handle);
	}

	data << ChunkHeader('STAT', 4);
	data << widgetState[0];

	data << ChunkHeader('COLR', sizeof(ColorRGBA));
	data << widgetColor;

	data << ChunkHeader('ANCH', 8);
	data << unsigned_int32(widgetAnchorX);
	data << unsigned_int32(widgetAnchorY);

	data << ChunkHeader('SIZE', sizeof(Vector2D));
	data << widgetSize;

	data << ChunkHeader('XFRM', sizeof(Transform4D));
	data << widgetTransform;

	if ((scriptObject) && (!(packFlags & kPackSettings)))
	{
		data << ChunkHeader('SCPT', 4);
		data << scriptObject->GetObjectIndex();
	}

	const Mutator *mutator = mutatorList.First();
	while (mutator)
	{
		PackHandle handle = data.BeginChunk('MUTR');
		mutator->PackType(data);
		mutator->Pack(data, packFlags);
		data.EndChunk(handle);

		mutator = mutator->Next();
	}

	if (balloonType != kBalloonNone)
	{
		PackHandle handle = data.BeginChunk('BLLN');
		data << balloonType;
		data << balloonString;
		data.EndChunk(handle);
	}

	data << TerminatorChunk;
}

void Widget::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	UnpackChunkList<Widget>(data, unpackFlags);
}

bool Widget::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'SUPR':

			data >> superIndex;
			return (true);

		case 'KEY ':

			data >> widgetKey;
			return (true);

		case 'STAT':

			data >> widgetState[0];
			return (true);

		case 'COLR':
		{
			ColorRGBA	color;

			data >> color;
			SetWidgetColor(color);
			return (true);
		}

		case 'ANCH':
		{
			unsigned_int32		anchorX, anchorY;

			data >> anchorX;
			data >> anchorY;
			widgetAnchorX = (unsigned_int8) anchorX;
			widgetAnchorY = (unsigned_int8) anchorY;
			return (true);
		}

		case 'SIZE':

			data >> widgetSize;
			return (true);

		case 'XFRM':

			data >> widgetTransform;
			return (true);

		case 'SCPT':
		{
			int32	objectIndex;

			data >> objectIndex;
			data.AddObjectLink(objectIndex, &ScriptObjectLinkProc, this);
			return (true);
		}

		case 'MUTR':
		{
			Mutator *mutator = Mutator::Create(data);
			if (mutator)
			{
				mutator->Unpack(++data, unpackFlags);
				AddMutator(mutator);
				return (true);
			}

			break;
		}

		case 'BLLN':

			data >> balloonType;
			data >> balloonString;
			break;
	}

	return (false);
}

void *Widget::BeginSettingsUnpack(void)
{
	widgetKey[0] = 0;
	balloonType = kBalloonNone;

	if (scriptObject)
	{
		scriptObject->Release();
		scriptObject = nullptr;
	}

	mutatorList.Purge();
	return (nullptr);
}

void Widget::ScriptObjectLinkProc(Object *object, void *cookie)
{
	Widget *widget = static_cast<Widget *>(cookie);
	widget->scriptObject = static_cast<ScriptObject *>(object);
	object->Retain();
}

int32 Widget::GetSettingCount(void) const
{
	return (9);
}

Setting *Widget::GetSetting(int32 index) const
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('WDGT', 'WDGT', 'SETT'));
		return (new HeadingSetting('WDGT', title));
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('WDGT', 'WDGT', 'IDNT'));
		return (new TextSetting('IDNT', widgetKey, title, kMaxWidgetKeyLength));
	}

	if (index == 2)
	{
		const char *title = table->GetString(StringID('WDGT', 'WDGT', 'WCOL'));
		const char *picker = table->GetString(StringID('WDGT', 'WDGT', 'CPCK'));
		return (new ColorSetting('WCOL', widgetColor, title, picker, kColorPickerAlpha));
	}

	if (index == 3)
	{
		const char *title = table->GetString(StringID('WDGT', 'WDGT', 'DSAB'));
		return (new BooleanSetting('DSAB', ((widgetState[0] & kWidgetDisabled) != 0), title));
	}

	if (index == 4)
	{
		const char *title = table->GetString(StringID('WDGT', 'WDGT', 'HIDN'));
		return (new BooleanSetting('HIDN', ((widgetState[0] & kWidgetHidden) != 0), title));
	}

	if (index == 5)
	{
		int32 selection = 0;
		if (widgetAnchorX == kWidgetAnchorLeft)
		{
			selection = 1;
		}
		else if (widgetAnchorX == kWidgetAnchorRight)
		{
			selection = 2;
		}
		else if (widgetAnchorX == kWidgetAnchorCenter)
		{
			selection = 3;
		}

		const char *title = table->GetString(StringID('WDGT', 'WDGT', 'ANCX'));
		MenuSetting *menu = new MenuSetting('ANCX', selection, title, 4);

		menu->SetMenuItemString(0, table->GetString(StringID('WDGT', 'WDGT', 'ANCX', 'NONE')));
		menu->SetMenuItemString(1, table->GetString(StringID('WDGT', 'WDGT', 'ANCX', 'LEFT')));
		menu->SetMenuItemString(2, table->GetString(StringID('WDGT', 'WDGT', 'ANCX', 'RGHT')));
		menu->SetMenuItemString(3, table->GetString(StringID('WDGT', 'WDGT', 'ANCX', 'CENT')));

		return (menu);
	}

	if (index == 6)
	{
		int32 selection = 0;
		if (widgetAnchorY == kWidgetAnchorTop)
		{
			selection = 1;
		}
		else if (widgetAnchorY == kWidgetAnchorBottom)
		{
			selection = 2;
		}
		else if (widgetAnchorY == kWidgetAnchorCenter)
		{
			selection = 3;
		}

		const char *title = table->GetString(StringID('WDGT', 'WDGT', 'ANCY'));
		MenuSetting *menu = new MenuSetting('ANCY', selection, title, 4);

		menu->SetMenuItemString(0, table->GetString(StringID('WDGT', 'WDGT', 'ANCY', 'NONE')));
		menu->SetMenuItemString(1, table->GetString(StringID('WDGT', 'WDGT', 'ANCY', 'TOP ')));
		menu->SetMenuItemString(2, table->GetString(StringID('WDGT', 'WDGT', 'ANCY', 'BTTM')));
		menu->SetMenuItemString(3, table->GetString(StringID('WDGT', 'WDGT', 'ANCY', 'CENT')));

		return (menu);
	}

	if (index == 7)
	{
		int32 selection = 0;
		if (balloonType == kBalloonText)
		{
			selection = 1;
		}
		else if (balloonType == kBalloonResource)
		{
			selection = 2;
		}

		const char *title = table->GetString(StringID('WDGT', 'WDGT', 'BTYP'));
		MenuSetting *menu = new MenuSetting('BTYP', selection, title, 3);

		menu->SetMenuItemString(0, table->GetString(StringID('WDGT', 'WDGT', 'BTYP', 'NONE')));
		menu->SetMenuItemString(1, table->GetString(StringID('WDGT', 'WDGT', 'BTYP', 'TEXT')));
		menu->SetMenuItemString(2, table->GetString(StringID('WDGT', 'WDGT', 'BTYP', 'RSRC')));

		return (menu);
	}

	if (index == 8)
	{
		const char *title = table->GetString(StringID('WDGT', 'WDGT', 'BSTR'));
		const char *picker = table->GetString(StringID('WDGT', 'WDGT', 'BPCK'));
		return (new ResourceSetting('BSTR', balloonString, title, picker, PanelResource::GetDescriptor()));
	}

	return (nullptr);
}

void Widget::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'IDNT')
	{
		widgetKey = static_cast<const TextSetting *>(setting)->GetText();
	}
	else if (identifier == 'WCOL')
	{
		SetWidgetColor(static_cast<const ColorSetting *>(setting)->GetColor());
	}
	else if (identifier == 'DSAB')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			SetWidgetState(widgetState[0] | kWidgetDisabled);
		}
		else
		{
			SetWidgetState(widgetState[0] & ~kWidgetDisabled);
		}
	}
	else if (identifier == 'HIDN')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			SetWidgetState(widgetState[0] | kWidgetHidden);
		}
		else
		{
			SetWidgetState(widgetState[0] & ~kWidgetHidden);
		}
	}
	else if (identifier == 'ANCX')
	{
		widgetAnchorX = (unsigned_int8) static_cast<const MenuSetting *>(setting)->GetMenuSelection();
	}
	else if (identifier == 'ANCY')
	{
		widgetAnchorY = (unsigned_int8) static_cast<const MenuSetting *>(setting)->GetMenuSelection();
	}
	else if (identifier == 'BTYP')
	{
		int32 selection = static_cast<const MenuSetting *>(setting)->GetMenuSelection();
		if (selection == 1)
		{
			balloonType = kBalloonText;
		}
		else if (selection == 2)
		{
			balloonType = kBalloonResource;
		}
		else
		{
			balloonType = 0;
		}
	}
	else if (identifier == 'BSTR')
	{
		balloonString = static_cast<const ResourceSetting *>(setting)->GetResourceName();
	}
}

void Widget::SetActiveUpdateFlags(unsigned_int32 flags)
{
	activeUpdateFlags = flags;
	currentUpdateFlags = flags;

	Widget *widget = GetSuperNode();
	while (widget)
	{
		widget->currentUpdateFlags |= flags;
		widget->subtreeUpdateFlags |= flags;

		widget = widget->GetSuperNode();
	}
}

void Widget::Invalidate(void)
{
	if (widgetManipulator)
	{
		widgetManipulator->Invalidate();
	}

	unsigned_int32 flags = currentUpdateFlags | activeUpdateFlags;
	currentUpdateFlags = flags;

	Widget *widget = GetSuperNode();
	if (widget)
	{
		flags |= subtreeUpdateFlags;

		do
		{
			widget->currentUpdateFlags |= flags;
			widget->subtreeUpdateFlags |= flags;

			widget = widget->GetSuperNode();
		} while (widget);
	}

	widget = GetFirstSubnode();
	while (widget)
	{
		widget->Invalidate();
		widget = widget->Next();
	}
}

void Widget::Update(void)
{
	UpdateStructure();
	UpdateTransform();
	UpdateBoundingBox();
}

void Widget::UpdateStructure(void)
{
	unsigned_int32 flags = currentUpdateFlags;
	if (flags & kUpdateStructure)
	{
		currentUpdateFlags = flags & ~kUpdateStructure;
		HandleStructureUpdate();
	}

	flags = subtreeUpdateFlags;
	if (flags & kUpdateStructure)
	{
		subtreeUpdateFlags = flags & ~kUpdateStructure;

		Widget *widget = GetFirstSubnode();
		while (widget)
		{
			widget->UpdateStructure();
			widget = widget->Next();
		}
	}
}

void Widget::UpdateTransform(void)
{
	unsigned_int32 flags = currentUpdateFlags;
	if (flags & kUpdateTransform)
	{
		currentUpdateFlags = flags & ~kUpdateTransform;
		HandleTransformUpdate();
	}

	flags = subtreeUpdateFlags;
	if (flags & kUpdateTransform)
	{
		subtreeUpdateFlags = flags & ~kUpdateTransform;

		Widget *widget = GetFirstSubnode();
		while (widget)
		{
			widget->UpdateTransform();
			widget = widget->Next();
		}
	}
}

void Widget::UpdateBoundingBox(void)
{
	unsigned_int32 flags = currentUpdateFlags;
	if (flags & kUpdateBoundingBox)
	{
		currentUpdateFlags = flags & ~kUpdateBoundingBox;
		Box2D *boxPointer = nullptr;

		if (CalculateBoundingBox(&widgetBoundingBox))
		{
			worldBoundingBox = Transform(widgetBoundingBox, GetWorldTransform());
			boxPointer = &worldBoundingBox;
		}

		flags = subtreeUpdateFlags;
		Widget *widget = GetFirstSubnode();
		while (widget)
		{
			if (flags & kUpdateBoundingBox)
			{
				widget->UpdateBoundingBox();
			}

			const Box2D *box = widget->GetBoundingBox();
			if (box)
			{
				if (boxPointer)
				{
					boxPointer->Union(*box);
				}
				else
				{
					worldBoundingBox = *box;
					boxPointer = &worldBoundingBox;
				}
			}

			widget = widget->Next();
		}

		subtreeUpdateFlags = flags & ~kUpdateBoundingBox;
		boundingBoxPointer = boxPointer;
	}
	else
	{
		flags = subtreeUpdateFlags;
		if (flags & kUpdateBoundingBox)
		{
			subtreeUpdateFlags = flags & ~kUpdateBoundingBox;

			Widget *widget = GetFirstSubnode();
			while (widget)
			{
				widget->UpdateBoundingBox();
				widget = widget->Next();
			}
		}
	}
}

void Widget::HandleStructureUpdate(void)
{
}

void Widget::HandleTransformUpdate(void)
{
	Widget *widget = GetSuperNode();
	if (widget)
	{
		SetWorldTransform(widget->GetWorldTransform() * widgetTransform);
	}
	else
	{
		SetWorldTransform(widgetTransform);
	}
}

bool Widget::CalculateBoundingBox(Box2D *box) const
{
	if (Fmin(widgetSize.x, widgetSize.y) > 0.0F)
	{
		box->min.Set(0.0F, 0.0F);
		box->max = widgetSize;
		return (true);
	}

	return (false);
}

RootWidget *Widget::GetRootWidget(void) const
{
	Widget *widget = GetSuperNode();
	while (widget)
	{
		if (widget->GetBaseWidgetType() == kWidgetRoot)
		{
			return (static_cast<RootWidget *>(widget));
		}

		widget = widget->GetSuperNode();
	}

	return (nullptr);
}

C4::Window *Widget::GetOwningWindow(void) const
{
	Widget *widget = GetSuperNode();
	while (widget)
	{
		if (widget->GetWidgetType() == kWidgetWindow)
		{
			return (static_cast<Window *>(widget));
		}

		widget = widget->GetSuperNode();
	}

	return (nullptr);
}

unsigned_int32 Widget::GetGlobalWidgetState(void) const
{
	unsigned_int32 state = widgetState[0];

	const Widget *widget = GetSuperNode();
	while (widget)
	{
		state |= widget->widgetState[0];
		widget = widget->GetSuperNode();
	}

	return (state);
}

void Widget::SetWidgetKey(const char *key)
{
	widgetKey = key;
	if (key[0] != 0)
	{
		RootWidget *root = GetRootWidget();
		if (root)
		{
			root->AddKeyedWidget(this);
		}
	}
	else
	{
		HashTable<Widget>::Remove(this);
	}
}

Widget *Widget::GetNextWidgetWithSameKey(void) const
{
	Widget *widget = HashTableElement<Widget>::Next();
	if ((widget) && (widget->GetWidgetKey() == widgetKey))
	{
		return (widget);
	}

	return (nullptr);
}

void Widget::SetScriptObject(ScriptObject *object)
{
	if (scriptObject != object)
	{
		if (scriptObject)
		{
			scriptObject->Release();
		}

		if (object)
		{
			object->Retain();
		}

		scriptObject = object;
	}
}

void Widget::ExecuteScript(Node *initiator, EventType eventType)
{
	if (panelController)
	{
		ScriptObject *object = scriptObject;
		if (object)
		{
			panelController->ExecuteWidgetScript(object, initiator, eventType);
		}
	}
}

void Widget::PostWidgetEvent(const WidgetEventData& eventData)
{
	if (!(widgetState[0] & (kWidgetDisabled | kWidgetHidden)))
	{
		if (!PostEvent(&eventData))
		{
			Window *window = GetOwningWindow();
			if (window)
			{
				window->HandleWidgetEvent(this, &eventData);
			}
		}
	}
}

void Widget::ClearFocus(void)
{
	Widget *widget = this;
	for (;;)
	{
		RootWidget *root = widget->GetRootWidget();
		if (!root)
		{
			break;
		}

		Widget *focus = root->GetFocusWidget();
		if ((focus) && ((focus == this) || (Successor(focus))))
		{
			root->SetFocusWidget(nullptr);
			break;
		}

		widget = root;
	}
}

void Widget::SetWidgetState(unsigned_int32 state)
{
	if (widgetState[0] != state)
	{
		widgetState[0] = state;

		Widget *widget = this;
		do
		{
			widget->buildFlag = true;
			widget = GetNextNode(widget);
		} while (widget);
	}
}

void Widget::SetWidgetSize(const Vector2D& size)
{
	widgetSize = size;
	buildFlag = true;
}

const ColorRGBA& Widget::GetWidgetColor(WidgetColorType type) const
{
	return (widgetColor);
}

void Widget::SetWidgetColor(const ColorRGBA& color, WidgetColorType type)
{
	if ((type == kWidgetColorDefault) || (type == defaultColorType))
	{
		widgetColor = color;
		buildFlag = true;
	}

	SetDynamicWidgetColor(color, type);
}

void Widget::SetWidgetAlpha(float alpha, WidgetColorType type)
{
	if ((type == kWidgetColorDefault) || (type == defaultColorType))
	{
		widgetColor.alpha = alpha;
		buildFlag = true;
	}

	SetDynamicWidgetAlpha(alpha, type);
}

void Widget::SetDynamicWidgetColor(const ColorRGBA& color, WidgetColorType type)
{
}

void Widget::SetDynamicWidgetAlpha(float alpha, WidgetColorType type)
{
}

WidgetPart Widget::TestPosition(const Point3D& position) const
{
	return (kWidgetPartInterior);
}

Widget *Widget::DetectWidget(const Point3D& position, unsigned_int32 usage, WidgetPart *widgetPart)
{
	const Box2D *box = GetBoundingBox();
	if ((box) && (box->Contains(position.GetPoint2D())))
	{
		if ((widgetState[0] & (kWidgetDisabled | kWidgetHidden)) == 0)
		{
			Widget *widget = GetLastSubnode();
			while (widget)
			{
				Widget *result = widget->DetectWidget(position, usage, widgetPart);
				if (result)
				{
					return (result);
				}

				widget = widget->Previous();
			}

			if ((widgetUsage & usage) == usage)
			{
				Point3D p = GetInverseWorldTransform() * position;
				if (widgetBoundingBox.Contains(p.GetPoint2D()))
				{
					WidgetPart part = TestPosition(p);
					if (part != kWidgetPartNone)
					{
						if (widgetPart)
						{
							*widgetPart = part;
						}

						return (this);
					}
				}
			}
		}
	}

	return (nullptr);
}

void Widget::Preprocess(void)
{
	Widget *super = GetSuperNode();
	if (super)
	{
		panelController = super->GetPanelController();
	}

	if (widgetKey[0] != 0)
	{
		RootWidget *root = GetRootWidget();
		if (root)
		{
			root->AddKeyedWidget(this);
		}
	}

	Mutator *mutator = mutatorList.First();
	if (mutator)
	{
		RootWidget *root = GetRootWidget();
		if (root)
		{
			root->AddMovingWidget(this);
			do
			{
				mutator->Preprocess();
				mutator = mutator->Next();
			} while (mutator);
		}
	}

	Widget *widget = GetFirstSubnode();
	while (widget)
	{
		widget->Preprocess();
		widget = widget->Next();
	}
}

void Widget::Move(void)
{
	Mutator *mutator = mutatorList.First();
	while (mutator)
	{
		Mutator *next = mutator->Next();

		if ((mutator->GetMutatorState() & (kMutatorDisabled | kMutatorTerminated)) == 0)
		{
			mutator->Move();
		}

		mutator = next;
	}
}

void Widget::Show(void)
{
	SetWidgetState(widgetState[0] & ~kWidgetHidden);
}

void Widget::Hide(void)
{
	ClearFocus();
	SetWidgetState(widgetState[0] | kWidgetHidden);
}

void Widget::Enable(void)
{
	SetWidgetState(widgetState[0] & ~kWidgetDisabled);
}

void Widget::Disable(void)
{
	ClearFocus();
	SetWidgetState(widgetState[0] | kWidgetDisabled);
}

void Widget::EnterForeground(void)
{
	Widget *widget = GetFirstSubnode();
	while (widget)
	{
		widget->EnterForeground();
		widget = widget->Next();
	}
}

void Widget::EnterBackground(void)
{
	Widget *widget = GetFirstSubnode();
	while (widget)
	{
		widget->EnterBackground();
		widget = widget->Next();
	}
}

void Widget::Build(void)
{
}

void Widget::Render(List<Renderable> *renderList)
{
}

void Widget::RenderTree(List<Renderable> *renderList)
{
	unsigned_int32 state = widgetState[(widgetManipulator != nullptr)];
	if (!(state & kWidgetHidden))
	{
		if (buildFlag)
		{
			Build();

			// The build flag must be cleared after the call to the Build() function,
			// not before, because the flag might be set by an implementation of Build().

			buildFlag = false;
		}

		Render(renderList);

		Widget *widget = GetFirstSubnode();
		while (widget)
		{
			widget->RenderTree(renderList);
			widget = widget->Next();
		}
	}
}

void Widget::SendInitialStateMessages(Player *player) const
{
}

void Widget::HandleMouseEvent(const PanelMouseEventData *eventData)
{
	if (eventData->eventType == kEventMouseDown)
	{
		ExecuteScript(eventData->initiatorNode, kEventWidgetActivate);
	}
}

bool Widget::HandleKeyboardEvent(const KeyboardEventData *eventData)
{
	return (false);
}

void Widget::TrackTask(WidgetPart widgetPart, const Point3D& mousePosition)
{
}

void Widget::FocusTask(void)
{
}

FileResult Widget::PackTree(File *file, unsigned_int32 packFlags) const
{
	PanelResourceHeader		resourceHeader;

	int32 widgetCount = 0;
	const Widget *widget = GetFirstSubnode();
	while (widget)
	{
		if (!(widget->widgetState[0] & kWidgetNonpersistent))
		{
			widget->widgetIndex = widgetCount++;
			widget = GetNextNode(widget);
		}
		else
		{
			widget = GetNextLevelNode(widget);
		}
	}

	resourceHeader.endian = 1;
	resourceHeader.version = kEngineInternalVersion;
	resourceHeader.widgetCount = widgetCount;
	resourceHeader.auxiliaryDataSize = 0;

	Buffer buffer(kPackageDefaultSize);

	if (baseWidgetType == kWidgetRoot)
	{
		FileResult result = file->Write(&resourceHeader, sizeof(PanelResourceHeader) - 4);
		if (result != kFileOkay)
		{
			return (result);
		}

		Package package(buffer, kPackageDefaultSize);
		Packer packer(&package);

		PackHandle handle = packer.BeginSection();
		static_cast<const RootWidget *>(this)->PackAuxiliaryData(packer);
		packer << TerminatorChunk;
		packer.EndSection(handle);

		result = file->Write(package.GetStorage(), package.GetSize());
		if (result != kFileOkay)
		{
			return (result);
		}
	}
	else
	{
		FileResult result = file->Write(&resourceHeader, sizeof(PanelResourceHeader));
		if (result != kFileOkay)
		{
			return (result);
		}
	}

	widget = GetFirstSubnode();
	while (widget)
	{
		if (!(widget->widgetState[0] & kWidgetNonpersistent))
		{
			Package package(buffer, kPackageDefaultSize);
			Packer packer(&package);

			PackHandle handle = packer.BeginSection();
			widget->PackType(packer);
			widget->Pack(packer, packFlags);
			packer.EndSection(handle);

			FileResult result = file->Write(package.GetStorage(), package.GetSize());
			if (result != kFileOkay)
			{
				return (result);
			}

			widget = GetNextNode(widget);
		}
		else
		{
			widget = GetNextLevelNode(widget);
		}
	}

	return (kFileOkay);
}

void Widget::UnpackTree(const PanelResourceHeader *data, unsigned_int32 unpackFlags)
{
	if ((baseWidgetType == kWidgetRoot) && (data->auxiliaryDataSize != 0))
	{
		Unpacker unpacker(data->GetAuxiliaryData(), data->version);
		static_cast<RootWidget *>(this)->UnpackAuxiliaryData(unpacker);
	}

	Unpacker unpacker(data->GetWidgetData(), data->version);

	int32 widgetCount = data->widgetCount;
	if (widgetCount != 0)
	{
		Widget **widgetTable = new Widget *[widgetCount];

		for (machine a = 0; a < widgetCount; a++)
		{
			unsigned_int32	size;

			unpacker >> size;
			const void *mark = unpacker.GetPointer();

			Widget *widget = Create(unpacker, unpackFlags);
			if (widget)
			{
				widget->Unpack(++unpacker, unpackFlags);
				widgetTable[a] = widget;
			}
			else
			{
				unpacker.Skip(mark, size);
				widgetTable[a] = nullptr;
			}
		}

		for (machine a = 0; a < widgetCount; a++)
		{
			Widget *widget = widgetTable[a];
			if (widget)
			{
				int32 superIndex = widget->superIndex;
				if (superIndex < 0)
				{
					AppendSubnode(widget);
				}
				else
				{
					Widget *super = widgetTable[superIndex];
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
			}
		}

		delete[] widgetTable;
	}
}

bool Widget::Load(const char *name)
{
	PanelResource *resource = PanelResource::Get(name);
	if (resource)
	{
		#if C4LOG_RESOURCES

			TheResourceMgr->IncrementResourceLogLevel();

		#endif

		UnpackTree(resource->GetPanelResourceHeader());
		resource->Release();

		#if C4LOG_RESOURCES

			TheResourceMgr->DecrementResourceLogLevel();

		#endif

		return (true);
	}

	return (false);
}


RenderableWidget::RenderableWidget(WidgetType type, RenderType renderType) :
		Widget(type),
		Renderable(renderType),
		widgetObserver(this, &RenderableWidget::HandleWidgetEvent)
{
}

RenderableWidget::RenderableWidget(WidgetType type, RenderType renderType, const Vector2D& size) :
		Widget(type, size),
		Renderable(renderType),
		widgetObserver(this, &RenderableWidget::HandleWidgetEvent)
{
}

RenderableWidget::RenderableWidget(const RenderableWidget& renderableWidget) :
		Widget(renderableWidget),
		Renderable(renderableWidget.GetRenderType()),
		widgetObserver(this, &RenderableWidget::HandleWidgetEvent)
{
}

RenderableWidget::~RenderableWidget()
{
}

void RenderableWidget::HandleTransformUpdate(void)
{
	Widget::HandleTransformUpdate();

	const RootWidget *root = GetRootWidget();
	if ((root) && (root->GetWidgetType() == kWidgetPanel))
	{
		const Transform4D& renderTransform = static_cast<const Panel *>(root)->GetRenderTransform();
		renderTransformable.SetWorldTransform(renderTransform * GetWorldTransform());
	}
	else
	{
		renderTransformable.SetWorldTransform(GetWorldTransform());
	}
}

void RenderableWidget::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	ExecuteScript(eventData->initiatorNode, eventData->eventType);
}

void RenderableWidget::Preprocess(void)
{
	Widget::Preprocess();

	SetBuildFlag();
	InitRenderable(this);

	if (GetPanelController())
	{
		SetObserver(&widgetObserver);
	}
}

void RenderableWidget::InitRenderable(Renderable *renderable)
{
	renderable->SetRenderableFlags(kRenderableFogInhibit);
	renderable->SetAmbientBlendState(kBlendInterpolate | kBlendAlphaPreserve);
	renderable->SetTransformable(&renderTransformable);
}

void RenderableWidget::BuildBorder(const Box2D& box, volatile BorderVertex *restrict vertex, int32 style)
{
	float w = (box.max.x - box.min.x + 4.0F) * 0.125F;
	float h = (box.max.y - box.min.y + 4.0F) * 0.125F;
	float t = (float) style * 0.25F;

	vertex[0].position.Set(box.min.x - 2.0F, box.min.y - 2.0F);
	vertex[0].texcoord.Set(0.0F, t);
	vertex[1].position.Set(box.min.x + 1.0F, box.min.y + 1.0F);
	vertex[1].texcoord.Set(0.375F, t + 0.1875F);
	vertex[2].position.Set(box.max.x - 1.0F, box.min.y + 1.0F);
	vertex[2].texcoord.Set(w - 0.375F, t + 0.1875F);
	vertex[3].position.Set(box.max.x + 2.0F, box.min.y - 2.0F);
	vertex[3].texcoord.Set(w, t);

	vertex[4].position.Set(box.min.x + 1.0F, box.max.y - 1.0F);
	vertex[4].texcoord.Set(0.375F, t);
	vertex[5].position.Set(box.min.x - 2.0F, box.max.y + 2.0F);
	vertex[5].texcoord.Set(0.0F, t + 0.1875F);
	vertex[6].position.Set(box.max.x + 2.0F, box.max.y + 2.0F);
	vertex[6].texcoord.Set(w, t + 0.1875F);
	vertex[7].position.Set(box.max.x - 1.0F, box.max.y - 1.0F);
	vertex[7].texcoord.Set(w - 0.375F, t);

	vertex[8].position.Set(box.min.x - 2.0F, box.min.y - 2.0F);
	vertex[8].texcoord.Set(0.0F, t);
	vertex[9].position.Set(box.min.x - 2.0F, box.max.y + 2.0F);
	vertex[9].texcoord.Set(h, t);
	vertex[10].position.Set(box.min.x + 1.0F, box.max.y - 1.0F);
	vertex[10].texcoord.Set(h - 0.375F, t + 0.1875F);
	vertex[11].position.Set(box.min.x + 1.0F, box.min.y + 1.0F);
	vertex[11].texcoord.Set(0.375F, t + 0.1875F);

	vertex[12].position.Set(box.max.x - 1.0F, box.min.y + 1.0F);
	vertex[12].texcoord.Set(0.375F, t);
	vertex[13].position.Set(box.max.x - 1.0F, box.max.y - 1.0F);
	vertex[13].texcoord.Set(h - 0.375F, t);
	vertex[14].position.Set(box.max.x + 2.0F, box.max.y + 2.0F);
	vertex[14].texcoord.Set(h, t + 0.1875F);
	vertex[15].position.Set(box.max.x + 2.0F, box.min.y - 2.0F);
	vertex[15].texcoord.Set(0.0F, t + 0.1875F);
}

void RenderableWidget::BuildGlow(const Box2D& box, volatile GlowVertex *restrict vertex)
{
	vertex[0].position.Set(box.min.x - 5.0F, box.min.y - 5.0F);
	vertex[0].texcoord.Set(0.96875F, 0.4375F);
	vertex[1].position.Set(box.min.x - 5.0F, box.min.y - 1.0F);
	vertex[1].texcoord.Set(0.96875F, 0.4296875F);
	vertex[2].position.Set(box.min.x - 1.0F, box.min.y - 1.0F);
	vertex[2].texcoord.Set(0.9765625F, 0.4296875F);
	vertex[3].position.Set(box.min.x - 1.0F, box.min.y - 5.0F);
	vertex[3].texcoord.Set(0.9765625F, 0.4375F);

	vertex[4].position.Set(box.min.x - 5.0F, box.min.y - 1.0F);
	vertex[4].texcoord.Set(0.96875F, 0.4296875F);
	vertex[5].position.Set(box.min.x - 5.0F, box.max.y + 1.0F);
	vertex[5].texcoord.Set(0.96875F, 0.4140625F);
	vertex[6].position.Set(box.min.x - 1.0F, box.max.y + 1.0F);
	vertex[6].texcoord.Set(0.9765625F, 0.4140625F);
	vertex[7].position.Set(box.min.x - 1.0F, box.min.y - 1.0F);
	vertex[7].texcoord.Set(0.9765625F, 0.4296875F);

	vertex[8].position.Set(box.min.x - 5.0F, box.max.y + 1.0F);
	vertex[8].texcoord.Set(0.96875F, 0.4140625F);
	vertex[9].position.Set(box.min.x - 5.0F, box.max.y + 5.0F);
	vertex[9].texcoord.Set(0.96875F, 0.40625F);
	vertex[10].position.Set(box.min.x - 1.0F, box.max.y + 5.0F);
	vertex[10].texcoord.Set(0.9765625F, 0.40625F);
	vertex[11].position.Set(box.min.x - 1.0F, box.max.y + 1.0F);
	vertex[11].texcoord.Set(0.9765625F, 0.4140625F);

	vertex[12].position.Set(box.min.x - 1.0F, box.max.y + 1.0F);
	vertex[12].texcoord.Set(0.9765625F, 0.4140625F);
	vertex[13].position.Set(box.min.x - 1.0F, box.max.y + 5.0F);
	vertex[13].texcoord.Set(0.9765625F, 0.40625F);
	vertex[14].position.Set(box.max.x + 1.0F, box.max.y + 5.0F);
	vertex[14].texcoord.Set(0.9921875F, 0.40625F);
	vertex[15].position.Set(box.max.x + 1.0F, box.max.y + 1.0F);
	vertex[15].texcoord.Set(0.9921875F, 0.4140625F);

	vertex[16].position.Set(box.max.x + 1.0F, box.max.y + 1.0F);
	vertex[16].texcoord.Set(0.9921875F, 0.4140625F);
	vertex[17].position.Set(box.max.x + 1.0F, box.max.y + 5.0F);
	vertex[17].texcoord.Set(0.9921875F, 0.40625F);
	vertex[18].position.Set(box.max.x + 5.0F, box.max.y + 5.0F);
	vertex[18].texcoord.Set(1.0F, 0.40625F);
	vertex[19].position.Set(box.max.x + 5.0F, box.max.y + 1.0F);
	vertex[19].texcoord.Set(1.0F, 0.4140625F);

	vertex[20].position.Set(box.max.x + 1.0F, box.min.y - 1.0F);
	vertex[20].texcoord.Set(0.9921875F, 0.4296875F);
	vertex[21].position.Set(box.max.x + 1.0F, box.max.y + 1.0F);
	vertex[21].texcoord.Set(0.9921875F, 0.4140625F);
	vertex[22].position.Set(box.max.x + 5.0F, box.max.y + 1.0F);
	vertex[22].texcoord.Set(1.0F, 0.4140625F);
	vertex[23].position.Set(box.max.x + 5.0F, box.min.y - 1.0F);
	vertex[23].texcoord.Set(1.0F, 0.4296875F);

	vertex[24].position.Set(box.max.x + 1.0F, box.min.y - 5.0F);
	vertex[24].texcoord.Set(0.9921875F, 0.4375F);
	vertex[25].position.Set(box.max.x + 1.0F, box.min.y - 1.0F);
	vertex[25].texcoord.Set(0.9921875F, 0.4296875F);
	vertex[26].position.Set(box.max.x + 5.0F, box.min.y - 1.0F);
	vertex[26].texcoord.Set(1.0F, 0.4296875F);
	vertex[27].position.Set(box.max.x + 5.0F, box.min.y - 5.0F);
	vertex[27].texcoord.Set(1.0F, 0.4375F);

	vertex[28].position.Set(box.min.x - 1.0F, box.min.y - 5.0F);
	vertex[28].texcoord.Set(0.9765625F, 0.4375F);
	vertex[29].position.Set(box.min.x - 1.0F, box.min.y - 1.0F);
	vertex[29].texcoord.Set(0.9765625F, 0.4296875F);
	vertex[30].position.Set(box.max.x + 1.0F, box.min.y - 1.0F);
	vertex[30].texcoord.Set(0.9921875F, 0.4296875F);
	vertex[31].position.Set(box.max.x + 1.0F, box.min.y - 5.0F);
	vertex[31].texcoord.Set(0.9921875F, 0.4375F);
}

void RenderableWidget::Render(List<Renderable> *renderList)
{
	if (GetVertexCount() != 0)
	{
		renderList->Append(this);
	}
}


LineWidget::LineWidget() :
		RenderableWidget(kWidgetLine, kRenderTriangleStrip),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		colorAttribute(kAttributeMutable),
		textureAttribute(&lineTextureHeader, lineTextureImage)
{
	lineStyle = kLineSolid;

	SetDefaultColorType(kWidgetColorLine);
}

LineWidget::LineWidget(const Vector2D& size, int32 style, const ColorRGBA& color) :
		RenderableWidget(kWidgetLine, kRenderTriangleStrip, size),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		colorAttribute(kAttributeMutable),
		textureAttribute(&lineTextureHeader, lineTextureImage)
{
	lineStyle = style;

	SetDefaultColorType(kWidgetColorLine);
	RenderableWidget::SetWidgetColor(color);
}

LineWidget::LineWidget(const LineWidget& lineWidget) :
		RenderableWidget(lineWidget),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		colorAttribute(lineWidget.colorAttribute),
		textureAttribute(&lineTextureHeader, lineTextureImage)
{
	lineStyle = lineWidget.lineStyle;
}

LineWidget::~LineWidget()
{
}

Widget *LineWidget::Replicate(void) const
{
	return (new LineWidget(*this));
}

void LineWidget::Pack(Packer& data, unsigned_int32 packFlags) const
{
	RenderableWidget::Pack(data, packFlags);

	data << ChunkHeader('STYL', 4);
	data << lineStyle;

	data << TerminatorChunk;
}

void LineWidget::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	RenderableWidget::Unpack(data, unpackFlags);
	UnpackChunkList<LineWidget>(data, unpackFlags);
}

bool LineWidget::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'STYL':

			data >> lineStyle;
			return (true);
	}

	return (false);
}

int32 LineWidget::GetSettingCount(void) const
{
	return (RenderableWidget::GetSettingCount() + 2);
}

Setting *LineWidget::GetSetting(int32 index) const
{
	int32 count = RenderableWidget::GetSettingCount();
	if (index < count)
	{
		return (RenderableWidget::GetSetting(index));
	}

	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetLine, 'SETT'));
		return (new HeadingSetting(kWidgetLine, title));
	}

	if (index == count + 1)
	{
		int32 selection = MaxZero(Min(lineStyle, kLineStyleCount - 1));

		const char *title = table->GetString(StringID('WDGT', kWidgetLine, 'STYL'));
		MenuSetting *menu = new MenuSetting('LSTY', selection, title, kLineStyleCount);

		menu->SetMenuItemString(0, table->GetString(StringID('WDGT', kWidgetLine, 'STYL', 'SOLD')));
		menu->SetMenuItemString(1, table->GetString(StringID('WDGT', kWidgetLine, 'STYL', 'DOT1')));
		menu->SetMenuItemString(2, table->GetString(StringID('WDGT', kWidgetLine, 'STYL', 'DOT2')));
		menu->SetMenuItemString(3, table->GetString(StringID('WDGT', kWidgetLine, 'STYL', 'DASH')));

		return (menu);
	}

	return (nullptr);
}

void LineWidget::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'LSTY')
	{
		lineStyle = static_cast<const MenuSetting *>(setting)->GetMenuSelection();
	}
	else
	{
		RenderableWidget::SetSetting(setting);
	}
}

void LineWidget::SetDynamicWidgetColor(const ColorRGBA& color, WidgetColorType type)
{
	if ((type == kWidgetColorDefault) || (type == kWidgetColorLine))
	{
		colorAttribute.SetDiffuseColor(color);
	}
}

void LineWidget::SetDynamicWidgetAlpha(float alpha, WidgetColorType type)
{
	if ((type == kWidgetColorDefault) || (type == kWidgetColorLine))
	{
		colorAttribute.SetDiffuseAlpha(alpha);
	}
}

void LineWidget::Preprocess(void)
{
	RenderableWidget::Preprocess();

	SetVertexCount(4);
	SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(LineVertex));
	SetVertexAttributeArray(kArrayPosition, 0, 2);
	SetVertexAttributeArray(kArrayTexcoord, sizeof(Point2D), 2);
	vertexBuffer.Establish(sizeof(LineVertex) * 4);

	attributeList.Append(&colorAttribute);
	attributeList.Append(&textureAttribute);
	SetMaterialAttributeList(&attributeList);
}

void LineWidget::Build(void)
{
	volatile LineVertex *restrict vertex = vertexBuffer.BeginUpdate<LineVertex>();

	float w = GetWidgetSize().x;
	float s = w * 0.125F;
	float t = (float) lineStyle * 0.25F;

	vertex[0].position.Set(0.0F, -1.0F);
	vertex[0].texcoord.Set(0.0F, t);

	vertex[1].position.Set(0.0F, 2.0F);
	vertex[1].texcoord.Set(0.0F, t + 0.1875F);

	vertex[2].position.Set(w, -1.0F);
	vertex[2].texcoord.Set(s, t);

	vertex[3].position.Set(w, 2.0F);
	vertex[3].texcoord.Set(s, t + 0.1875F);

	vertexBuffer.EndUpdate();
}


BorderWidget::BorderWidget() :
		RenderableWidget(kWidgetBorder, kRenderQuads),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		colorAttribute(kAttributeMutable),
		textureAttribute(&LineWidget::lineTextureHeader, LineWidget::lineTextureImage)
{
	lineStyle = kLineSolid;

	SetDefaultColorType(kWidgetColorBorder);
}

BorderWidget::BorderWidget(const Vector2D& size, int32 style, const ColorRGBA& color) :
		RenderableWidget(kWidgetBorder, kRenderQuads, size),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		colorAttribute(kAttributeMutable),
		textureAttribute(&LineWidget::lineTextureHeader, LineWidget::lineTextureImage)
{
	lineStyle = style;

	SetDefaultColorType(kWidgetColorBorder);
	RenderableWidget::SetWidgetColor(color);
}

BorderWidget::BorderWidget(const BorderWidget& borderWidget) :
		RenderableWidget(borderWidget),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		colorAttribute(borderWidget.colorAttribute),
		textureAttribute(&LineWidget::lineTextureHeader, LineWidget::lineTextureImage)
{
	lineStyle = borderWidget.lineStyle;
}

BorderWidget::~BorderWidget()
{
}

Widget *BorderWidget::Replicate(void) const
{
	return (new BorderWidget(*this));
}

void BorderWidget::Pack(Packer& data, unsigned_int32 packFlags) const
{
	RenderableWidget::Pack(data, packFlags);

	data << ChunkHeader('STYL', 4);
	data << lineStyle;

	data << TerminatorChunk;
}

void BorderWidget::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	RenderableWidget::Unpack(data, unpackFlags);
	UnpackChunkList<BorderWidget>(data, unpackFlags);
}

bool BorderWidget::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'STYL':

			data >> lineStyle;
			return (true);
	}

	return (false);
}

int32 BorderWidget::GetSettingCount(void) const
{
	return (RenderableWidget::GetSettingCount() + 2);
}

Setting *BorderWidget::GetSetting(int32 index) const
{
	int32 count = RenderableWidget::GetSettingCount();
	if (index < count)
	{
		return (RenderableWidget::GetSetting(index));
	}

	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetBorder, 'SETT'));
		return (new HeadingSetting(kWidgetBorder, title));
	}

	if (index == count + 1)
	{
		int32 selection = MaxZero(Min(lineStyle, kLineStyleCount - 1));

		const char *title = table->GetString(StringID('WDGT', kWidgetLine, 'STYL'));
		MenuSetting *menu = new MenuSetting('BSTY', selection, title, 4);

		menu->SetMenuItemString(0, table->GetString(StringID('WDGT', kWidgetLine, 'STYL', 'SOLD')));
		menu->SetMenuItemString(1, table->GetString(StringID('WDGT', kWidgetLine, 'STYL', 'DOT1')));
		menu->SetMenuItemString(2, table->GetString(StringID('WDGT', kWidgetLine, 'STYL', 'DOT2')));
		menu->SetMenuItemString(3, table->GetString(StringID('WDGT', kWidgetLine, 'STYL', 'DASH')));

		return (menu);
	}

	return (nullptr);
}

void BorderWidget::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'BSTY')
	{
		lineStyle = static_cast<const MenuSetting *>(setting)->GetMenuSelection();
	}
	else
	{
		RenderableWidget::SetSetting(setting);
	}
}

void BorderWidget::SetDynamicWidgetColor(const ColorRGBA& color, WidgetColorType type)
{
	if ((type == kWidgetColorDefault) || (type == kWidgetColorBorder))
	{
		colorAttribute.SetDiffuseColor(color);
	}
}

void BorderWidget::SetDynamicWidgetAlpha(float alpha, WidgetColorType type)
{
	if ((type == kWidgetColorDefault) || (type == kWidgetColorBorder))
	{
		colorAttribute.SetDiffuseAlpha(alpha);
	}
}

void BorderWidget::Preprocess(void)
{
	RenderableWidget::Preprocess();

	SetVertexCount(16);
	SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(BorderVertex));
	SetVertexAttributeArray(kArrayPosition, 0, 2);
	SetVertexAttributeArray(kArrayTexcoord, sizeof(Point2D), 2);
	vertexBuffer.Establish(sizeof(BorderVertex) * 16);

	attributeList.Append(&colorAttribute);
	attributeList.Append(&textureAttribute);
	SetMaterialAttributeList(&attributeList);
}

void BorderWidget::Build(void)
{
	volatile BorderVertex *restrict borderVertex = vertexBuffer.BeginUpdate<BorderVertex>();
	BuildBorder(Box2D(Zero2D, Zero2D + GetWidgetSize()), borderVertex, lineStyle);
	vertexBuffer.EndUpdate();
}


TextWidget::TextWidget(const char *text, const char *font) : TextWidget(kWidgetText, Zero2D, text, font)
{
}

TextWidget::TextWidget(const Vector2D& size, const char *text, const char *font) : TextWidget(kWidgetText, size, text, font)
{
}

TextWidget::TextWidget(WidgetType type, const char *text, const char *font) : TextWidget(type, Zero2D, text, font)
{
}

TextWidget::TextWidget(WidgetType type, const Vector2D& size, const char *text, const char *font) :
		RenderableWidget(type, kRenderQuads, size),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
{
	SetBaseWidgetType(kWidgetText);

	SetDefaultColorType(kWidgetColorText);
	RenderableWidget::SetWidgetColor(K::black);

	if (font)
	{
		textFont = Font::Get(font);
		fontName = font;
	}
	else
	{
		textFont = nullptr;
		fontName[0] = 0;
	}

	textFlags = kTextClipped;
	splitFlag = true;

	textRenderOffset.Set(0.0F, 0.0F, 0.0F);

	firstRenderLine = 0;
	renderLineCount = 0;
	formatExclusionMask = kTextFormatAlignment;

	initialFormat.textAlignment = kTextAlignLeft;
	initialFormat.textColor.Set(0, 0, 0, 255);

	textScale = 1.0F;
	textLeading = 0.0F;

	textStorage = emptyString;
	textStorageSize = 0;
	textLength = 0;
	glyphCount = 0;

	AllocateTextStorage(text);
}

TextWidget::TextWidget(const TextWidget& textWidget) :
		RenderableWidget(textWidget),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
{
	const char *font = textWidget.fontName;
	if (font[0] != 0)
	{
		textFont = Font::Get(font);
		fontName = font;
	}
	else
	{
		textFont = nullptr;
		fontName[0] = 0;
	}

	textFlags = textWidget.textFlags;
	splitFlag = true;

	textRenderOffset = textWidget.textRenderOffset;

	firstRenderLine = textWidget.firstRenderLine;
	renderLineCount = textWidget.renderLineCount;

	textScale = textWidget.textScale;
	textLeading = textWidget.textLeading;

	formatExclusionMask = textWidget.formatExclusionMask;
	initialFormat = textWidget.initialFormat;

	textStorage = emptyString;
	textStorageSize = 0;

	AllocateTextStorage(textWidget.GetText());
}

TextWidget::~TextWidget()
{
	ReleaseTextStorage();

	if (textFont)
	{
		textFont->Release();
	}
}

Widget *TextWidget::Replicate(void) const
{
	return (new TextWidget(*this));
}

void TextWidget::Pack(Packer& data, unsigned_int32 packFlags) const
{
	RenderableWidget::Pack(data, packFlags);

	const char *text = textStorage;
	if (text != emptyString)
	{
		PackHandle handle = data.BeginChunk('STRG');
		data << text;
		data.EndChunk(handle);
	}

	PackHandle handle = data.BeginChunk('FONT');
	data << fontName;
	data.EndChunk(handle);

	data << ChunkHeader('FLAG', 4);
	data << textFlags;

	data << ChunkHeader('ALGN', 4);
	int32 alignment = initialFormat.textAlignment;
	data << alignment;

	data << ChunkHeader('COLR', sizeof(Color4C));
	data << initialFormat.textColor;

	if (textScale != 1.0F)
	{
		data << ChunkHeader('SCAL', 4);
		data << textScale;
	}

	if (textLeading != 0.0F)
	{
		data << ChunkHeader('LEAD', 4);
		data << textLeading;
	}

	data << TerminatorChunk;
}

void TextWidget::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	RenderableWidget::Unpack(data, unpackFlags);
	UnpackChunkList<TextWidget>(data, unpackFlags);
}

bool TextWidget::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'STRG':

			AllocateTextStorage(data.ReadString());
			return (true);

		case 'FONT':

			data >> fontName;

			if (fontName[0])
			{
				textFont = Font::Get(fontName);
			}

			return (true);

		case 'FLAG':

			data >> textFlags;
			return (true);

		case 'ALGN':
		{
			int32	alignment;

			data >> alignment;
			initialFormat.textAlignment = static_cast<TextAlignment>(alignment);
			return (true);
		}

		case 'COLR':

			data >> initialFormat.textColor;
			return (true);

		case 'SCAL':

			data >> textScale;
			return (true);

		case 'LEAD':

			data >> textLeading;
			return (true);
	}

	return (false);
}

void *TextWidget::BeginSettingsUnpack(void)
{
	textScale = 1.0F;
	textLeading = 0.0F;

	ReleaseTextStorage();
	InvalidateShaderData();
	vertexBuffer.Establish(0);

	if (textFont)
	{
		textFont->Release();
		textFont = nullptr;
	}

	return (Widget::BeginSettingsUnpack());
}

int32 TextWidget::GetSettingCount(void) const
{
	return (RenderableWidget::GetSettingCount() + 6);
}

Setting *TextWidget::GetSetting(int32 index) const
{
	int32 count = RenderableWidget::GetSettingCount();
	if (index < count)
	{
		return (RenderableWidget::GetSetting(index));
	}

	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetText, 'SETT'));
		return (new HeadingSetting(kWidgetText, title));
	}

	if (index == count + 1)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetText, 'FONT'));
		const char *picker = table->GetString(StringID('WDGT', kWidgetText, 'FPCK'));
		return (new ResourceSetting('FONT', fontName, title, picker, FontResource::GetDescriptor()));
	}

	if (index == count + 2)
	{
		int32 selection = 0;
		TextAlignment alignment = initialFormat.textAlignment;
		if (alignment == kTextAlignCenter)
		{
			selection = 1;
		}
		else if (alignment == kTextAlignRight)
		{
			selection = 2;
		}

		const char *title = table->GetString(StringID('WDGT', kWidgetText, 'ALGN'));
		MenuSetting *menu = new MenuSetting('ALGN', selection, title, 3);

		menu->SetMenuItemString(0, table->GetString(StringID('WDGT', kWidgetText, 'ALGN', 'LEFT')));
		menu->SetMenuItemString(1, table->GetString(StringID('WDGT', kWidgetText, 'ALGN', 'CENT')));
		menu->SetMenuItemString(2, table->GetString(StringID('WDGT', kWidgetText, 'ALGN', 'RGHT')));

		return (menu);
	}

	if (index == count + 3)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetText, 'TSCL'));
		return (new TextSetting('TSCL', textScale, title));
	}

	if (index == count + 4)
	{
		if (GetWidgetType() != kWidgetText)
		{
			return (nullptr);
		}

		const char *title = table->GetString(StringID('WDGT', kWidgetText, 'LEAD'));
		return (new TextSetting('LEAD', textLeading, title));
	}

	if (index == count + 5)
	{
		if (GetWidgetType() != kWidgetText)
		{
			return (nullptr);
		}

		const char *title = table->GetString(StringID('WDGT', kWidgetText, 'SNGL'));
		return (new BooleanSetting('SNGL', ((textFlags & kTextWrapped) == 0), title));
	}

	return (nullptr);
}

void TextWidget::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'FONT')
	{
		SetFont(static_cast<const ResourceSetting *>(setting)->GetResourceName());
	}
	else if (identifier == 'ALGN')
	{
		int32 selection = static_cast<const MenuSetting *>(setting)->GetMenuSelection();
		if (selection == 1)
		{
			initialFormat.textAlignment = kTextAlignCenter;
		}
		else if (selection == 2)
		{
			initialFormat.textAlignment = kTextAlignRight;
		}
		else
		{
			initialFormat.textAlignment = kTextAlignLeft;
		}
	}
	else if (identifier == 'TSCL')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		textScale = Text::StringToFloat(text);
	}
	else if (identifier == 'LEAD')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		textLeading = Text::StringToFloat(text);
	}
	else if (identifier == 'SNGL')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			textFlags = kTextClipped;
		}
		else
		{
			textFlags = kTextWrapped;
		}
	}
	else
	{
		Widget::SetSetting(setting);
	}
}

void TextWidget::AllocateTextStorage(const char *text, int32 max)
{
	unsigned_int32 textSize = 0;
	int32 vertexCount = 0;

	if (text)
	{
		const char *byte = text;
		int32 count = 0;

		for (;;)
		{
			unsigned_int32	code;

			byte += Text::ReadGlyphCodeUTF8(byte, &code);
			if (code == 0)
			{
				textSize = (unsigned_int32) (byte - text);
				break;
			}

			if ((code > 0x0020) && (code != 0x00A0))
			{
				vertexCount += 4;
			}

			if (++count == max)
			{
				textSize = (unsigned_int32) (byte - text + 1);
				break;
			}
		}

		textLength = textSize - 1;
		glyphCount = count;
	}
	else
	{
		textLength = 0;
		glyphCount = 0;
	}

	textSize = (textSize + 15) & ~15;

	if ((textSize > textStorageSize) || (textSize == 0))
	{
		ReleaseTextStorage();

		textStorageSize = textSize;
		if (textSize != 0)
		{
			textStorage = new char[textSize];
		}
	}

	if (textStorage != emptyString)
	{
		Text::CopyText(text, textStorage, textLength);
	}

	vertexBuffer.Establish(vertexCount * sizeof(TextVertex));
	InvalidateVertexData();
}

void TextWidget::ReleaseTextStorage(void)
{
	if (textStorage != emptyString)
	{
		delete[] textStorage;
		textStorage = emptyString;
	}
}

void TextWidget::SetDynamicWidgetColor(const ColorRGBA& color, WidgetColorType type)
{
	if ((type == kWidgetColorDefault) || (type == kWidgetColorText))
	{
		initialFormat.textColor.Set(color);
		SetBuildFlag();
	}
}

void TextWidget::SetDynamicWidgetAlpha(float alpha, WidgetColorType type)
{
	if ((type == kWidgetColorDefault) || (type == kWidgetColorText))
	{
		initialFormat.textColor.SetAlpha((unsigned_int32) (alpha * 255.0F));
		SetBuildFlag();
	}
}

void TextWidget::SetWidgetSize(const Vector2D& size)
{
	RenderableWidget::SetWidgetSize(size);
	splitFlag = true;
}

void TextWidget::Preprocess(void)
{
	RenderableWidget::Preprocess();

	SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(TextVertex));
	SetVertexAttributeArray(kArrayPosition, 0, 2);
	SetVertexAttributeArray(kArrayColor, sizeof(Point2D), 1);
	SetVertexAttributeArray(kArrayTexcoord, sizeof(Point2D) + sizeof(Color4C), 2);

	attributeList.Append(&referenceAttribute);
	SetMaterialAttributeList(&attributeList);
}

void TextWidget::SetTextFlags(unsigned_int32 flags)
{
	textFlags = flags;
	splitFlag = true;
	SetBuildFlag();
}

void TextWidget::SetTextRenderOffset(const Vector3D& offset)
{
	textRenderOffset = offset;
	splitFlag = true;
	SetBuildFlag();
}

void TextWidget::SetFirstRenderLine(int32 index)
{
	firstRenderLine = index;
	SetBuildFlag();
}

void TextWidget::SetRenderLineCount(int32 count)
{
	renderLineCount = count;
	SetBuildFlag();
}

void TextWidget::SetText(const char *text, int32 max)
{
	AllocateTextStorage(text, max);
	splitFlag = true;
	SetBuildFlag();
}

void TextWidget::AppendText(const char *text, int32 max)
{
	String<> string(textStorage);
	SetText(string += text, max);
}

void TextWidget::PrependText(const char *text, int32 max)
{
	String<> string(text);
	SetText(string += textStorage, max);
}

void TextWidget::SetFont(const char *font)
{
	Font *prevFont = textFont;

	if (font)
	{
		textFont = Font::Get(font);
		fontName = font;
	}
	else
	{
		textFont = nullptr;
		fontName[0] = 0;
	}

	if (textFont != prevFont)
	{
		InvalidateShaderData();
		splitFlag = true;
		SetBuildFlag();
	}

	if (prevFont)
	{
		prevFont->Release();
	}
}

void TextWidget::SetTextFormatExclusionMask(unsigned_int32 mask)
{
	formatExclusionMask = mask;
	SetBuildFlag();
}

void TextWidget::SetTextAlignment(TextAlignment alignment)
{
	initialFormat.textAlignment = alignment;
	splitFlag = true;
	SetBuildFlag();
}

void TextWidget::SetTextScale(float scale)
{
	textScale = scale;
	splitFlag = true;
	SetBuildFlag();
}

void TextWidget::SetTextLeading(float leading)
{
	textLeading = leading;
	splitFlag = true;
	SetBuildFlag();
}

bool TextWidget::ProcessFormatTag(TextFormatTag tag, unsigned_int32 mask, TextFormatState *format, TextFormatState *savedFormat) const
{
	if ((tag >> 24) == '#')
	{
		if ((mask & kTextFormatColor) == 0)
		{
			unsigned_int32 red = (tag >> 16) & 0xFF;
			if (red - '0' < 10U)
			{
				red -= 48;
			}
			else if (red - 'A' < 6U)
			{
				red -= 55;
			}
			else
			{
				red = 0;
			}

			unsigned_int32 green = (tag >> 8) & 0xFF;
			if (green - '0' < 10U)
			{
				green -= 48;
			}
			else if (green - 'A' < 6U)
			{
				green -= 55;
			}
			else
			{
				green = 0;
			}

			unsigned_int32 blue = tag & 0xFF;
			if (blue - '0' < 10U)
			{
				blue -= 48;
			}
			else if (blue - 'A' < 6U)
			{
				blue -= 55;
			}
			else
			{
				blue = 0;
			}

			red = (red << 4) | red;
			green = (green << 4) | green;
			blue = (blue << 4) | blue;

			format->textColor.SetRed(red);
			format->textColor.SetGreen(green);
			format->textColor.SetBlue(blue);
		}

		return (true);
	}

	switch (tag)
	{
		case kTagAlignLeft:

			if ((mask & kTextFormatAlignment) == 0)
			{
				format->textAlignment = kTextAlignLeft;
			}

			return (true);

		case kTagAlignCenter:

			if ((mask & kTextFormatAlignment) == 0)
			{
				format->textAlignment = kTextAlignCenter;
			}

			return (true);

		case kTagAlignRight:

			if ((mask & kTextFormatAlignment) == 0)
			{
				format->textAlignment = kTextAlignRight;
			}

			return (true);

		case kTagResetFormat:

			if ((mask & kTextFormatWholeState) == 0)
			{
				format->Copy(initialFormat);
			}

			return (true);

		case kTagSaveFormat:

			if ((mask & kTextFormatWholeState) == 0)
			{
				savedFormat->Copy(*format);
			}

			return (true);

		case kTagRestoreFormat:

			if ((mask & kTextFormatWholeState) == 0)
			{
				format->Copy(*savedFormat);
			}

			return (true);
	}

	return (false);
}

TextFormatTag TextWidget::FindFormatTag(const char *text, int32 *offset) const
{
	if (textFlags & kTextUnformatted)
	{
		*offset = textLength;
	}
	else
	{
		int32 count = 0;
		for (;;)
		{
			char c = text[count];
			if (c == 0)
			{
				break;
			}

			count++;

			if (c == '[')
			{
				unsigned_int32 x = text[count];
				if (x == 0)
				{
					break;
				}

				if (x == '[')
				{
					continue;
				}

				count++;

				unsigned_int32 y = text[count];
				if (y == 0)
				{
					break;
				}

				if (y == '[')
				{
					continue;
				}

				count++;

				unsigned_int32 z = text[count];
				if (z == 0)
				{
					break;
				}

				if (z == '[')
				{
					continue;
				}

				count++;

				unsigned_int32 w = text[count];
				if (w == 0)
				{
					break;
				}

				if (w == '[')
				{
					continue;
				}

				count++;

				unsigned_int32 k = text[count];
				if (k == 0)
				{
					break;
				}

				if (k == '[')
				{
					continue;
				}

				if (k == ']')
				{
					if (x - 'a' < 26U)
					{
						x -= 32;
					}

					if (y - 'a' < 26U)
					{
						y -= 32;
					}

					if (z - 'a' < 26U)
					{
						z -= 32;
					}

					if (w - 'a' < 26U)
					{
						w -= 32;
					}

					TextFormatTag tag = (x << 24) | (y << 16) | (z << 8) | w;
					if (ProcessFormatTag(tag, ~0, nullptr, nullptr))
					{
						*offset = count - 5;
						return (tag);
					}
				}

				count++;
			}
		}

		*offset = count;
	}

	return (0);
}

float TextWidget::GetAlignedTextWidth(const char *text, int32 length) const
{
	int32 totalCount = 0;
	int32 textCount = 0;

	float width = 0.0F;
	for (;;)
	{
		int32	offset;

		TextFormatTag tag = FindFormatTag(&text[totalCount], &offset);

		int32 count = Min(offset, length);
		if (count != 0)
		{
			width += textFont->GetTextWidthUTF8(&text[totalCount], count);
			totalCount += count;
			textCount = totalCount;
		}

		if ((tag == 0) || (length <= offset) || (tag == kTagAlignLeft) || (tag == kTagAlignCenter) || (tag == kTagAlignRight))
		{
			break;
		}

		totalCount += 6;
		length -= offset + 6;
	}

	while (--textCount >= 0)
	{
		unsigned_int32 code = *reinterpret_cast<const unsigned_int8 *>(&text[textCount]);
		if ((code != 0x0020) && (code != 0x000D) && (code != 0x000A))
		{
			break;
		}

		const FontHeader *fontHeader = textFont->GetFontHeader();
		width -= fontHeader->GetGlyphData()[textFont->GetGlyphIndex(code)].glyphWidth + fontHeader->fontSpacing;
	}

	return (width * textScale);
}

float TextWidget::GetFormattedTextWidth(void) const
{
	const char *text = textStorage;
	if ((text[0] != 0) && (textFont))
	{
		return (GetAlignedTextWidth(text, textLength));
	}

	return (0.0F);
}

float TextWidget::GetStartingPosition(const char *text, int32 length, TextAlignment alignment) const
{
	if (alignment == kTextAlignLeft)
	{
		return (0.0F);
	}

	float dw = GetWidgetSize().x - GetAlignedTextWidth(text, length);
	if (alignment == kTextAlignCenter)
	{
		return (Floor(dw * 0.5F));
	}

	return (dw);
}

int32 TextWidget::GetTextLengthFitWidth(const char *text, float width, float *used) const
{
	int32 length = 0;
	float total = 0.0F;

	width /= textScale;
	float w = width;

	for (bool partialWord = true;; partialWord = false)
	{
		int32	offset;
		float	actual;
		bool	hardBreak;

		TextFormatTag tag = FindFormatTag(text, &offset);
		int32 count = textFont->GetBrokenTextLengthFitWidthUTF8(text, offset, w, partialWord, &actual, &hardBreak);

		length += count;
		total += actual;

		if ((tag == 0) || (hardBreak) || (count < offset))
		{
			break;
		}

		text += offset + 6;
		length += 6;

		if ((tag == kTagAlignLeft) || (tag == kTagAlignCenter) || (tag == kTagAlignRight))
		{
			w = width;
			total = width;
		}
		else
		{
			w -= actual + textFont->GetFontHeader()->fontSpacing;
		}
	}

	*used = Fmin(total, width);
	return (length);
}

void TextWidget::SplitLines(void)
{
	if (splitFlag)
	{
		splitFlag = false;

		const char *text = GetText();
		if (text)
		{
			lineEndArray.Clear();

			int32 position = 0;
			float maxWidth = 0.0F;
			for (;;)
			{
				float	width;

				int32 length = GetTextLengthFitWidth(&text[position], GetWidgetSize().x, &width);
				if (length == 0)
				{
					length = Text::GetTextLength(&text[position]);
				}

				position += length;
				maxWidth = Fmax(maxWidth, width);

				lineEndArray.AddElement(position);

				if (text[position] == 0)
				{
					break;
				}
			}

			maxLineWidth = maxWidth;
		}
		else
		{
			lineEndArray.SetElementCount(1);
			lineEndArray[0] = 0;
			maxLineWidth = 0.0F;
		}
	}
}

void TextWidget::BuildLine(const char *text, int32 length, const Vector3D& renderOffset, volatile TextVertex *restrict& vertex, TextFormatState *format, TextFormatState *savedFormat)
{
	float x = GetStartingPosition(text, length, format->textAlignment) + renderOffset.x;
	float y = renderOffset.y;

	const FontHeader *fontHeader = textFont->GetFontHeader();
	float height = fontHeader->fontHeight * textScale;
	float spacing = fontHeader->fontSpacing * textScale;

	for (;;)
	{
		int32	offset;

		TextFormatTag tag = FindFormatTag(text, &offset);
		int32 end = Min(offset, length);
		length -= end;

		if (!(textFlags & kTextClipped))
		{
			for (machine a = 0; a < end;)
			{
				unsigned_int32	code;

				int32 read = Text::ReadGlyphCodeUTF8(text, &code);

				const GlyphData *data = &fontHeader->GetGlyphData()[textFont->GetGlyphIndex(code)];
				float width = data->glyphWidth * textScale;

				if ((code > 0x0020) && (code != 0x00A0))
				{
					float left = x + fontHeader->shadowOffset[0].x * textScale;
					float top = y + fontHeader->shadowOffset[0].y * textScale;
					float right = x + width + fontHeader->shadowOffset[1].x * textScale;
					float bottom = y + height + fontHeader->shadowOffset[1].y * textScale;

					vertex[0].position.Set(left, top);
					vertex[1].position.Set(left, bottom);
					vertex[2].position.Set(right, bottom);
					vertex[3].position.Set(right, top);

					for (machine b = 0; b < 4; b++)
					{
						vertex[b].color = format->textColor;
					}

					vertex[0].texcoord = data->glyphTexcoord[0];
					vertex[1].texcoord.Set(data->glyphTexcoord[0].x, data->glyphTexcoord[1].y);
					vertex[2].texcoord = data->glyphTexcoord[1];
					vertex[3].texcoord.Set(data->glyphTexcoord[1].x, data->glyphTexcoord[0].y);

					vertex += 4;
				}

				x += width + spacing;
				text += read;
				a += read;
			}
		}
		else
		{
			float textBoxWidth = GetWidgetSize().x;

			for (machine a = 0; a < end;)
			{
				unsigned_int32	code;

				int32 read = Text::ReadGlyphCodeUTF8(text, &code);

				const GlyphData *data = &fontHeader->GetGlyphData()[textFont->GetGlyphIndex(code)];
				float width = data->glyphWidth * textScale;

				if ((code > 0x0020) && (code != 0x00A0) && (x > -width) && (x < textBoxWidth))
				{
					float top = y + fontHeader->shadowOffset[0].y * textScale;
					float bottom = y + height + fontHeader->shadowOffset[1].y * textScale;

					float x1 = x + fontHeader->shadowOffset[0].x * textScale;
					float x2 = x + width + fontHeader->shadowOffset[1].x * textScale;
					float s1 = data->glyphTexcoord[0].x;
					float s2 = data->glyphTexcoord[1].x;
					float ds = s2 - s1;

					if (x1 < 0.0F)
					{
						s1 += -x1 / (width + (fontHeader->shadowOffset[1].x - fontHeader->shadowOffset[0].x) * textScale) * ds;
						x1 = 0.0F;
					}

					if (x2 > textBoxWidth)
					{
						s2 -= (x2 - textBoxWidth) / (width + (fontHeader->shadowOffset[1].x - fontHeader->shadowOffset[0].x) * textScale) * ds;
						x2 = textBoxWidth;
					}

					vertex[0].position.Set(x1, top);
					vertex[1].position.Set(x1, bottom);
					vertex[2].position.Set(x2, bottom);
					vertex[3].position.Set(x2, top);

					for (machine b = 0; b < 4; b++)
					{
						vertex[b].color = format->textColor;
					}

					vertex[0].texcoord.Set(s1, data->glyphTexcoord[0].y);
					vertex[1].texcoord.Set(s1, data->glyphTexcoord[1].y);
					vertex[2].texcoord.Set(s2, data->glyphTexcoord[1].y);
					vertex[3].texcoord.Set(s2, data->glyphTexcoord[0].y);

					vertex += 4;
				}

				x += width + spacing;
				text += read;
				a += read;
			}
		}

		if ((length == 0) || (tag == 0))
		{
			break;
		}

		text += 6;
		length -= 6;

		TextAlignment alignment = format->textAlignment;
		ProcessFormatTag(tag, formatExclusionMask, format, savedFormat);

		if (format->textAlignment != alignment)
		{
			x = GetStartingPosition(text, length, format->textAlignment) + renderOffset.x;
		}
	}
}

void TextWidget::Build(void)
{
	if ((vertexBuffer.Active()) && (textFont))
	{
		TextFormatState		savedFormat;

		volatile TextVertex *restrict textVertex = vertexBuffer.BeginUpdate<TextVertex>();
		volatile TextVertex *textStart = textVertex;

		const char *text = textStorage;
		TextFormatState format = initialFormat;

		if (textFlags & kTextWrapped)
		{
			SplitLines();

			int32 firstLine = MaxZero(firstRenderLine);
			int32 lastLine = lineEndArray.GetElementCount();

			if (renderLineCount != 0)
			{
				lastLine = Min(lastLine, firstLine + renderLineCount);
			}

			lastLine = Max(lastLine - 1, firstLine);

			Vector3D renderOffset = textRenderOffset;
			float lineSpacing = GetLineSpacing();

			int32 begin = (firstLine == 0) ? 0 : lineEndArray[firstLine - 1];
			text += begin;

			for (machine a = firstLine; a <= lastLine; a++)
			{
				int32 end = lineEndArray[a];
				int32 length = end - begin;

				BuildLine(text, length, renderOffset, textVertex, &format, &savedFormat);

				renderOffset.y += lineSpacing;
				text += length;
				begin = end;
			}
		}
		else
		{
			BuildLine(text, textLength, textRenderOffset, textVertex, &format, &savedFormat);
		}

		vertexBuffer.EndUpdate();
		SetVertexCount((int32) (textVertex - textStart));

		referenceAttribute.SetReference(textFont->GetTextureAttribute());
	}
	else
	{
		SetVertexCount(0);
	}
}


EditTextWidget::EditTextWidget(WidgetType type) :
		TextWidget(type),
		structureVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		backgroundColorAttribute(kAttributeMutable),
		backgroundRenderable(kRenderTriangleStrip),
		borderColorAttribute(kAttributeMutable),
		borderTextureAttribute(&LineWidget::lineTextureHeader, LineWidget::lineTextureImage),
		borderRenderable(kRenderQuads),
		focusColorAttribute(kAttributeMutable),
		focusRenderable(kRenderQuads),
		caretColorAttribute(kAttributeMutable),
		caretRenderable(kRenderTriangleStrip),
		hiliteVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		hiliteColorAttribute(kAttributeMutable),
		hiliteRenderable(kRenderQuads)
{
	SetRenderLineCount(1);
	SetEditTextFlags(0);
	editTextState = 0;
	caretPosition = 0;

	filterType = kFilterNone;
	filterProc = nullptr;

	paddingSize = 2.0F;
	colorOverrideFlags = 0;

	backgroundColor.Set(1.0F, 1.0F, 1.0F);
	borderColor.Set(0.25F, 0.25F, 0.25F);
	hiliteColor.Set(0.0F, 0.0F, 0.0F);
	focusColor.Set(0.0F, 0.0F, 0.0F);
	caretColor.Set(0.0F, 0.0F, 0.0F);

	SetWidgetUsage(kWidgetKeyboardFocus);
}

EditTextWidget::EditTextWidget(const Vector2D& size, int32 maxGlyph, const char *font) : EditTextWidget(kWidgetEditText, size, maxGlyph, font)
{
}

EditTextWidget::EditTextWidget(WidgetType type, const Vector2D& size, int32 maxGlyph, const char *font) :
		TextWidget(type, size, nullptr, font),
		structureVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		backgroundColorAttribute(kAttributeMutable),
		backgroundRenderable(kRenderTriangleStrip),
		borderColorAttribute(kAttributeMutable),
		borderTextureAttribute(&LineWidget::lineTextureHeader, LineWidget::lineTextureImage),
		borderRenderable(kRenderQuads),
		focusColorAttribute(kAttributeMutable),
		focusRenderable(kRenderQuads),
		caretColorAttribute(kAttributeMutable),
		caretRenderable(kRenderTriangleStrip),
		hiliteVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		hiliteColorAttribute(kAttributeMutable),
		hiliteRenderable(kRenderQuads)
{
	maxGlyphCount = maxGlyph;

	SetRenderLineCount(1);
	SetEditTextFlags(0);
	editTextState = 0;
	caretPosition = 0;

	filterType = kFilterNone;
	filterProc = nullptr;

	paddingSize = 2.0F;
	colorOverrideFlags = 0;

	backgroundColor.Set(1.0F, 1.0F, 1.0F);
	borderColor.Set(0.25F, 0.25F, 0.25F);
	hiliteColor.Set(0.0F, 0.0F, 0.0F);
	focusColor.Set(0.0F, 0.0F, 0.0F);
	caretColor.Set(0.0F, 0.0F, 0.0F);

	SetWidgetUsage(kWidgetKeyboardFocus);
}

EditTextWidget::EditTextWidget(const EditTextWidget& editTextWidget) :
		TextWidget(editTextWidget),
		structureVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		backgroundColorAttribute(kAttributeMutable),
		backgroundRenderable(kRenderTriangleStrip),
		borderColorAttribute(kAttributeMutable),
		borderTextureAttribute(&LineWidget::lineTextureHeader, LineWidget::lineTextureImage),
		borderRenderable(kRenderQuads),
		focusColorAttribute(kAttributeMutable),
		focusRenderable(kRenderQuads),
		caretColorAttribute(kAttributeMutable),
		caretRenderable(kRenderTriangleStrip),
		hiliteVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		hiliteColorAttribute(kAttributeMutable),
		hiliteRenderable(kRenderQuads)
{
	SetEditTextFlags(editTextWidget.editTextFlags);
	editTextState = 0;
	caretPosition = 0;

	maxGlyphCount = editTextWidget.maxGlyphCount;
	filterType = editTextWidget.filterType;
	filterProc = editTextWidget.filterProc;

	paddingSize = editTextWidget.paddingSize;
	colorOverrideFlags = editTextWidget.colorOverrideFlags;

	backgroundColor = editTextWidget.backgroundColor;
	borderColor = editTextWidget.borderColor;
	hiliteColor = editTextWidget.hiliteColor;
	focusColor = editTextWidget.focusColor;
	caretColor = editTextWidget.caretColor;
}

EditTextWidget::~EditTextWidget()
{
}

Widget *EditTextWidget::Replicate(void) const
{
	return (new EditTextWidget(*this));
}

void EditTextWidget::Pack(Packer& data, unsigned_int32 packFlags) const
{
	TextWidget::Pack(data, packFlags);

	data << ChunkHeader('FLAG', 4);
	data << editTextFlags;

	data << ChunkHeader('MGLY', 4);
	data << maxGlyphCount;

	data << ChunkHeader('FILT', 4);
	data << filterType;

	data << ChunkHeader('PADD', 4);
	data << paddingSize;

	data << ChunkHeader('OVER', 4);
	data << colorOverrideFlags;

	data << ChunkHeader('BGDC', sizeof(ColorRGBA));
	data << backgroundColor;

	data << ChunkHeader('BRDC', sizeof(ColorRGBA));
	data << borderColor;

	if (colorOverrideFlags & kWidgetOverrideHiliteColor)
	{
		data << ChunkHeader('HILC', sizeof(ColorRGBA));
		data << hiliteColor;
	}

	if (colorOverrideFlags & kWidgetOverrideFocusColor)
	{
		data << ChunkHeader('FOCC', sizeof(ColorRGBA));
		data << focusColor;
	}

	data << ChunkHeader('CRTC', sizeof(ColorRGBA));
	data << caretColor;

	data << TerminatorChunk;
}

void EditTextWidget::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	TextWidget::Unpack(data, unpackFlags);
	UnpackChunkList<EditTextWidget>(data, unpackFlags);
}

bool EditTextWidget::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':
		{
			unsigned_int32	flags;

			data >> flags;
			SetEditTextFlags(flags);
			return (true);
		}

		case 'MGLY':

			data >> maxGlyphCount;
			return (true);

		case 'FILT':

			data >> filterType;
			return (true);

		case 'PADD':

			data >> paddingSize;
			return (true);

		case 'OVER':

			data >> colorOverrideFlags;
			return (true);

		case 'BGDC':

			data >> backgroundColor;
			return (true);

		case 'BRDC':

			data >> borderColor;
			return (true);

		case 'HILC':

			data >> hiliteColor;
			return (true);

		case 'FOCC':

			data >> focusColor;
			return (true);

		case 'CRTC':

			data >> caretColor;
			return (true);
	}

	return (false);
}

void *EditTextWidget::BeginSettingsUnpack(void)
{
	colorOverrideFlags = 0;
	return (TextWidget::BeginSettingsUnpack());
}

int32 EditTextWidget::GetSettingCount(void) const
{
	return (TextWidget::GetSettingCount() + 15);
}

Setting *EditTextWidget::GetSetting(int32 index) const
{
	int32 count = TextWidget::GetSettingCount();
	if (index < count)
	{
		return (TextWidget::GetSetting(index));
	}

	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetEditText, 'SETT'));
		return (new HeadingSetting(kWidgetEditText, title));
	}

	if (index == count + 1)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetEditText, 'MAXL'));
		return (new TextSetting('MAXL', Text::IntegerToString(maxGlyphCount), title, 4, &NumberFilter));
	}

	if (index == count + 2)
	{
		int32 selection = 0;
		if (filterType != kFilterNone)
		{
			for (machine a = 0; a < kFilterTypeCount; a++)
			{
				if (filterType == filterTypeTable[a])
				{
					selection = a + 1;
					break;
				}
			}
		}

		const char *title = table->GetString(StringID('WDGT', kWidgetEditText, 'FILT'));
		MenuSetting *menu = new MenuSetting('FILT', selection, title, kFilterTypeCount + 1);

		menu->SetMenuItemString(0, table->GetString(StringID('WDGT', kWidgetEditText, 'FILT', 'NONE')));
		menu->SetMenuItemString(1, table->GetString(StringID('WDGT', kWidgetEditText, 'FILT', kFilterNumber)));
		menu->SetMenuItemString(2, table->GetString(StringID('WDGT', kWidgetEditText, 'FILT', kFilterSignedNumber)));
		menu->SetMenuItemString(3, table->GetString(StringID('WDGT', kWidgetEditText, 'FILT', kFilterFloatingPoint)));
		menu->SetMenuItemString(4, table->GetString(StringID('WDGT', kWidgetEditText, 'FILT', kFilterAlphanumeric)));
		menu->SetMenuItemString(5, table->GetString(StringID('WDGT', kWidgetEditText, 'FILT', kFilterIdentifier)));

		return (menu);
	}

	if (index == count + 3)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetEditText, 'OVER'));
		return (new BooleanSetting('OVER', ((editTextFlags & kEditTextOverflow) != 0), title));
	}

	if (index == count + 4)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetEditText, 'MULT'));
		return (new BooleanSetting('MULT', ((editTextFlags & kEditTextMultiline) != 0), title));
	}

	if (index == count + 5)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetEditText, 'READ'));
		return (new BooleanSetting('READ', ((editTextFlags & kEditTextReadOnly) != 0), title));
	}

	if (index == count + 6)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetEditText, 'CHNG'));
		return (new BooleanSetting('CHNG', ((editTextFlags & kEditTextChangeSelectAll) != 0), title));
	}

	if (index == count + 7)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetEditText, 'DOUB'));
		return (new BooleanSetting('DOUB', ((editTextFlags & kEditTextDoubleClickSelectAll) != 0), title));
	}

	if (index == count + 8)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetEditText, 'REND'));
		return (new BooleanSetting('ETRP', ((editTextFlags & kEditTextRenderPlain) != 0), title));
	}

	if (index == count + 9)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetEditText, 'FPLN'));
		return (new BooleanSetting('ETFP', ((editTextFlags & kEditTextFocusPlain) != 0), title));
	}

	if (index == count + 10)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetEditText, 'BGDC'));
		const char *picker = table->GetString(StringID('WDGT', kWidgetEditText, 'BGDP'));
		return (new ColorSetting('ETBG', backgroundColor, title, picker, kColorPickerAlpha));
	}

	if (index == count + 11)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetEditText, 'BRDC'));
		const char *picker = table->GetString(StringID('WDGT', kWidgetEditText, 'BRDP'));
		return (new ColorSetting('ETBR', borderColor, title, picker, kColorPickerAlpha));
	}

	if (index == count + 12)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetEditText, 'HILC'));
		const char *picker = table->GetString(StringID('WDGT', kWidgetEditText, 'HILP'));
		return (new CheckColorSetting('ETHC', ((colorOverrideFlags & kWidgetOverrideHiliteColor) != 0), hiliteColor, title, picker, kColorPickerAlpha));
	}

	if (index == count + 13)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetEditText, 'FOCC'));
		const char *picker = table->GetString(StringID('WDGT', kWidgetEditText, 'FOCP'));
		return (new CheckColorSetting('ETFC', ((colorOverrideFlags & kWidgetOverrideFocusColor) != 0), focusColor, title, picker, kColorPickerAlpha));
	}

	if (index == count + 14)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetEditText, 'CRTC'));
		const char *picker = table->GetString(StringID('WDGT', kWidgetEditText, 'CRTP'));
		return (new ColorSetting('CRTC', caretColor, title, picker, kColorPickerAlpha));
	}

	return (nullptr);
}

void EditTextWidget::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'MAXL')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		maxGlyphCount = Text::StringToInteger(text);
	}
	else if (identifier == 'FILT')
	{
		int32 selection = static_cast<const MenuSetting *>(setting)->GetMenuSelection();
		if (selection == 0)
		{
			filterType = kFilterNone;
		}
		else
		{
			filterType = filterTypeTable[selection - 1];
		}
	}
	else if (identifier == 'OVER')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			editTextFlags |= kEditTextOverflow;
		}
		else
		{
			editTextFlags &= ~kEditTextOverflow;
		}
	}
	else if (identifier == 'MULT')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			editTextFlags |= kEditTextMultiline;
		}
		else
		{
			editTextFlags &= ~kEditTextMultiline;
		}
	}
	else if (identifier == 'READ')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			editTextFlags |= kEditTextReadOnly;
		}
		else
		{
			editTextFlags &= ~kEditTextReadOnly;
		}
	}
	else if (identifier == 'CHNG')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			editTextFlags |= kEditTextChangeSelectAll;
		}
		else
		{
			editTextFlags &= ~kEditTextChangeSelectAll;
		}
	}
	else if (identifier == 'DOUB')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			editTextFlags |= kEditTextDoubleClickSelectAll;
		}
		else
		{
			editTextFlags &= ~kEditTextDoubleClickSelectAll;
		}
	}
	else if (identifier == 'ETRP')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			editTextFlags |= kEditTextRenderPlain;
		}
		else
		{
			editTextFlags &= ~kEditTextRenderPlain;
		}
	}
	else if (identifier == 'ETFP')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			editTextFlags |= kEditTextFocusPlain;
		}
		else
		{
			editTextFlags &= ~kEditTextFocusPlain;
		}
	}
	else if (identifier == 'ETBG')
	{
		backgroundColor = static_cast<const ColorSetting *>(setting)->GetColor();
	}
	else if (identifier == 'ETBR')
	{
		borderColor = static_cast<const ColorSetting *>(setting)->GetColor();
	}
	else if (identifier == 'ETHC')
	{
		const CheckColorSetting *checkColorSetting = static_cast<const CheckColorSetting *>(setting);
		if (checkColorSetting->GetCheckValue() != 0)
		{
			colorOverrideFlags |= kWidgetOverrideHiliteColor;
			hiliteColor = checkColorSetting->GetColor();
		}
		else
		{
			colorOverrideFlags &= ~kWidgetOverrideHiliteColor;
			SetDefaultHiliteColor();
		}
	}
	else if (identifier == 'ETFC')
	{
		const CheckColorSetting *checkColorSetting = static_cast<const CheckColorSetting *>(setting);
		if (checkColorSetting->GetCheckValue() != 0)
		{
			colorOverrideFlags |= kWidgetOverrideFocusColor;
			focusColor = checkColorSetting->GetColor();
		}
		else
		{
			colorOverrideFlags &= ~kWidgetOverrideFocusColor;
			SetDefaultFocusColor();
		}
	}
	else if (identifier == 'CRTC')
	{
		caretColor = static_cast<const ColorSetting *>(setting)->GetColor();
	}
	else
	{
		TextWidget::SetSetting(setting);
	}
}

const ColorRGBA& EditTextWidget::GetWidgetColor(WidgetColorType type) const
{
	if (type == kWidgetColorBackground)
	{
		return (backgroundColor);
	}

	if (type == kWidgetColorBorder)
	{
		return (borderColor);
	}

	if (type == kWidgetColorHilite)
	{
		return (hiliteColor);
	}

	if (type == kWidgetColorFocus)
	{
		return (focusColor);
	}

	if (type == kWidgetColorCaret)
	{
		return (caretColor);
	}

	return (TextWidget::GetWidgetColor(type));
}

void EditTextWidget::SetWidgetColor(const ColorRGBA& color, WidgetColorType type)
{
	if (type == kWidgetColorBackground)
	{
		backgroundColor = color;
	}
	else if (type == kWidgetColorBorder)
	{
		borderColor = color;
	}
	else if (type == kWidgetColorHilite)
	{
		hiliteColor = color;
		colorOverrideFlags |= kWidgetOverrideHiliteColor;
	}
	else if (type == kWidgetColorFocus)
	{
		focusColor = color;
		colorOverrideFlags |= kWidgetOverrideFocusColor;
	}
	else if (type == kWidgetColorCaret)
	{
		caretColor = color;
	}

	TextWidget::SetWidgetColor(color, type);
}

void EditTextWidget::SetWidgetAlpha(float alpha, WidgetColorType type)
{
	if (type == kWidgetColorBorder)
	{
		borderColor.alpha = alpha;
	}
	else if (type == kWidgetColorBackground)
	{
		backgroundColor.alpha = alpha;
	}
	else if (type == kWidgetColorHilite)
	{
		hiliteColor.alpha = alpha;
		colorOverrideFlags |= kWidgetOverrideHiliteColor;
	}
	else if (type == kWidgetColorFocus)
	{
		focusColor.alpha = alpha;
		colorOverrideFlags |= kWidgetOverrideFocusColor;
	}
	else if (type == kWidgetColorCaret)
	{
		caretColor.alpha = alpha;
	}

	TextWidget::SetWidgetAlpha(alpha, type);
}

void EditTextWidget::SetDynamicWidgetColor(const ColorRGBA& color, WidgetColorType type)
{
	if (type == kWidgetColorBorder)
	{
		borderColorAttribute.SetDiffuseColor(color);
	}
	else if (type == kWidgetColorBackground)
	{
		backgroundColorAttribute.SetDiffuseColor(color);
	}
	else if (type == kWidgetColorHilite)
	{
		hiliteColorAttribute.SetDiffuseColor(ColorRGBA(color.GetColorRGB(), (GetWidgetState() & kWidgetBackground) ? color.alpha * 0.5F : color.alpha));
	}
	else if (type == kWidgetColorFocus)
	{
		focusColorAttribute.SetDiffuseColor(color);
	}
	else if (type == kWidgetColorCaret)
	{
		caretColorAttribute.SetDiffuseColor(color);
	}
	else
	{
		TextWidget::SetDynamicWidgetColor(color, type);
	}
}

void EditTextWidget::SetDynamicWidgetAlpha(float alpha, WidgetColorType type)
{
	if (type == kWidgetColorBorder)
	{
		borderColorAttribute.SetDiffuseAlpha(alpha);
	}
	else if (type == kWidgetColorBackground)
	{
		backgroundColorAttribute.SetDiffuseAlpha(alpha);
	}
	else if (type == kWidgetColorHilite)
	{
		hiliteColorAttribute.SetDiffuseAlpha((GetWidgetState() & kWidgetBackground) ? alpha * 0.5F : alpha);
	}
	else if (type == kWidgetColorFocus)
	{
		focusColorAttribute.SetDiffuseAlpha(alpha);
	}
	else if (type == kWidgetColorCaret)
	{
		caretColorAttribute.SetDiffuseAlpha(alpha);
	}
	else
	{
		TextWidget::SetDynamicWidgetAlpha(alpha, type);
	}
}

void EditTextWidget::SetDefaultHiliteColor(void)
{
	hiliteColor = TheInterfaceMgr->GetInterfaceColor(kInterfaceColorHilite);
}

void EditTextWidget::SetDefaultFocusColor(void)
{
	focusColor.Set(TheInterfaceMgr->GetInterfaceColor(kInterfaceColorHilite).GetColorRGB() * 0.625F, 1.0F);
}

bool EditTextWidget::CalculateBoundingBox(Box2D *box) const
{
	const Vector2D& size = GetWidgetSize();
	box->min.Set(-paddingSize, -paddingSize);
	box->max.Set(size.x + paddingSize, size.y + paddingSize);
	return (true);
}

void EditTextWidget::Preprocess(void)
{
	if (!(colorOverrideFlags & kWidgetOverrideHiliteColor))
	{
		SetDefaultHiliteColor();
	}

	if (!(colorOverrideFlags & kWidgetOverrideFocusColor))
	{
		SetDefaultFocusColor();
	}

	TextWidget::Preprocess();

	const Font *font = GetFont();
	if (font)
	{
		SetRenderLineCount(Max((int32) (GetWidgetSize().y / GetLineSpacing()), 1));
	}

	if (filterType != kFilterNone)
	{
		for (machine a = 0; a < kFilterTypeCount; a++)
		{
			if (filterType == filterTypeTable[a])
			{
				filterProc = filterProcTable[a];
				break;
			}
		}
	}

	structureVertexBuffer.Establish(sizeof(Point2D) * 8 + sizeof(BorderVertex) * 16 + sizeof(GlowVertex) * 32);

	InitRenderable(&backgroundRenderable);
	backgroundRenderable.SetVertexCount(4);
	backgroundRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &structureVertexBuffer, sizeof(Point2D));
	backgroundRenderable.SetVertexAttributeArray(kArrayPosition, 0, 2);

	backgroundAttributeList.Append(&backgroundColorAttribute);
	backgroundRenderable.SetMaterialAttributeList(&backgroundAttributeList);

	InitRenderable(&borderRenderable);
	borderRenderable.SetVertexCount(16);
	borderRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &structureVertexBuffer, sizeof(BorderVertex));
	borderRenderable.SetVertexAttributeArray(kArrayPosition, sizeof(Point2D) * 4, 2);
	borderRenderable.SetVertexAttributeArray(kArrayTexcoord, sizeof(Point2D) * 5, 2);

	borderAttributeList.Append(&borderColorAttribute);
	borderAttributeList.Append(&borderTextureAttribute);
	borderRenderable.SetMaterialAttributeList(&borderAttributeList);

	InitRenderable(&focusRenderable);
	focusRenderable.SetVertexCount(32);
	focusRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &structureVertexBuffer, sizeof(GlowVertex));
	focusRenderable.SetVertexAttributeArray(kArrayPosition, sizeof(Point2D) * 4 + sizeof(BorderVertex) * 16, 2);
	focusRenderable.SetVertexAttributeArray(kArrayTexcoord, sizeof(Point2D) * 5 + sizeof(BorderVertex) * 16, 2);

	focusAttributeList.Append(&focusColorAttribute);
	focusRenderable.SetMaterialAttributeList(&focusAttributeList);
	focusRenderable.SetMaterialObjectPointer(TheInterfaceMgr->GetMaterialObjectPointer());

	InitRenderable(&caretRenderable);
	caretRenderable.SetVertexCount(4);
	caretRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &structureVertexBuffer, sizeof(Point2D));
	caretRenderable.SetVertexAttributeArray(kArrayPosition, sizeof(Point2D) * 4 + sizeof(BorderVertex) * 16 + sizeof(GlowVertex) * 32, 2);

	caretAttributeList.Append(&caretColorAttribute);
	caretRenderable.SetMaterialAttributeList(&caretAttributeList);

	InitRenderable(&hiliteRenderable);
	hiliteRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &hiliteVertexBuffer, sizeof(Point2D));
	hiliteRenderable.SetVertexAttributeArray(kArrayPosition, 0, 2);

	hiliteAttributeList.Append(&hiliteColorAttribute);
	hiliteRenderable.SetMaterialAttributeList(&hiliteAttributeList);
}

void EditTextWidget::EnterForeground(void)
{
	TextWidget::EnterForeground();

	unsigned_int32 state = GetWidgetState();
	if ((state & kWidgetFocus) && (!(editTextState & kEditTextSelection)))
	{
		editTextState |= kEditTextCaretVisible;
		caretBlinkTime = TheInterfaceMgr->GetCaretBlinkTime();
	}

	SetWidgetState(state & ~kWidgetBackground);
}

void EditTextWidget::EnterBackground(void)
{
	TextWidget::EnterBackground();

	editTextState &= ~kEditTextCaretVisible;
	SetWidgetState(GetWidgetState() | kWidgetBackground);
}

bool EditTextWidget::NumberFilter(unsigned_int32 code)
{
	return (code - 0x0030 < 10U);
}

bool EditTextWidget::SignedNumberFilter(unsigned_int32 code)
{
	return ((code - 0x0030 < 10U) || (code == '-'));
}

bool EditTextWidget::FloatNumberFilter(unsigned_int32 code)
{
	return ((code - 0x0030 < 10U) || (code == '-') || (code == '.') || (code == 'e') || (code == 'E'));
}

bool EditTextWidget::AlphanumericFilter(unsigned_int32 code)
{
	return ((code - 0x0041 < 26U) || (code - 0x0061 < 26U) || (code - 0x0030 < 10U));
}

bool EditTextWidget::IdentifierFilter(unsigned_int32 code)
{
	return ((code - 0x0041 < 26U) || (code - 0x0061 < 26U) || (code - 0x0030 < 10U) || (code == '_'));
}

void EditTextWidget::SetEditTextFlags(unsigned_int32 flags)
{
	editTextFlags = flags;
	SetTextFlags(kTextUnformatted | ((flags & kEditTextMultiline) ? kTextWrapped : kTextClipped));
	SetTextRenderOffset(Zero3D);

	unsigned_int32 usage = GetWidgetUsage();
	if (flags & kEditTextMultiline)
	{
		usage |= kWidgetMouseWheel;
	}
	else
	{
		usage &= ~kWidgetMouseWheel;
	}

	SetWidgetUsage(usage);
}

void EditTextWidget::SetPaddingSize(float size)
{
	paddingSize = size;
	SetBuildFlag();
}

void EditTextWidget::GetSelection(int32 *begin, int32 *end)
{
	if (editTextState & kEditTextSelection)
	{
		*begin = selectionBegin;
		*end = selectionEnd;
	}
	else
	{
		int32 position = caretPosition;
		*begin = position;
		*end = position;
	}
}

void EditTextWidget::SetSelection(int32 begin, int32 end)
{
	int32 length = GetTextLength();
	begin = Min(MaxZero(begin), length);
	end = Min(MaxZero(end), length);

	caretPosition = end;
	selectionBegin = begin;
	selectionEnd = end;
	selectionDirection = 1;

	if (begin == end)
	{
		caretBlinkTime = TheInterfaceMgr->GetCaretBlinkTime();
		editTextState = (editTextState & ~kEditTextSelection) | kEditTextCaretVisible;
	}
	else
	{
		editTextState = (editTextState | kEditTextSelection) & ~kEditTextCaretVisible;
	}

	SetBuildFlag();
}

void EditTextWidget::SelectAll(void)
{
	caretPosition = 0;

	unsigned_int32 state = editTextState;
	int32 length = GetTextLength();
	if (length != 0)
	{
		selectionBegin = 0;
		selectionEnd = length;
		selectionDirection = -1;

		editTextState = (state | kEditTextSelection) & ~kEditTextCaretVisible;
		SetBuildFlag();
	}
	else if (state & kEditTextSelection)
	{
		caretBlinkTime = TheInterfaceMgr->GetCaretBlinkTime();
		editTextState = (state & ~kEditTextSelection) | kEditTextCaretVisible;
		SetBuildFlag();
	}
}

void EditTextWidget::SetText(const char *text, int32 max)
{
	if (max == 0)
	{
		max = maxGlyphCount;
	}
	else
	{
		max = Min(max, maxGlyphCount);
	}

	TextWidget::SetText(text, max);

	if (text)
	{
		if ((editTextFlags & kEditTextChangeSelectAll) && (GetWidgetState() & kWidgetFocus))
		{
			SelectAll();
		}
		else if (editTextState & kEditTextSelection)
		{
			SetSelection(selectionBegin, selectionEnd);
		}
		else
		{
			caretPosition = Min(caretPosition, GetTextLength());
		}
	}
	else
	{
		caretPosition = 0;
		editTextState &= ~kEditTextSelection;
	}
}

void EditTextWidget::AppendText(const char *text, int32 max)
{
	if (max == 0)
	{
		max = maxGlyphCount;
	}
	else
	{
		max = Min(max, maxGlyphCount);
	}

	TextWidget::AppendText(text, max);

	if ((editTextFlags & kEditTextChangeSelectAll) && (GetWidgetState() & kWidgetFocus))
	{
		SelectAll();
	}
}

void EditTextWidget::PrependText(const char *text, int32 max)
{
	if (max == 0)
	{
		max = maxGlyphCount;
	}
	else
	{
		max = Min(max, maxGlyphCount);
	}

	TextWidget::PrependText(text, max);

	if ((editTextFlags & kEditTextChangeSelectAll) && (GetWidgetState() & kWidgetFocus))
	{
		SelectAll();
	}
}

void EditTextWidget::InsertGlyph(unsigned_int32 code)
{
	if ((!filterProc) || ((*filterProc)(code)))
	{
		if (editTextState & kEditTextSelection)
		{
			ClearSelection();
		}

		if (GetGlyphCount() < maxGlyphCount)
		{
			const Font *font = GetFont();
			if (font)
			{
				int32 count = Text::GetGlyphCodeByteCountUTF8(code);
				if (count != 0)
				{
					if ((editTextFlags & (kEditTextOverflow | kEditTextMultiline)) || (font->GetTextWidthUTF8(GetText()) + font->GetFullGlyphWidth(code) <= GetWidgetSize().x))
					{
						char	codeUTF8[4];

						codeUTF8[Text::WriteGlyphCodeUTF8(codeUTF8, code)] = 0;
						InsertGlyphCode(codeUTF8);
					}
				}
			}
		}
	}
}

void EditTextWidget::InsertGlyphCode(const char *codeUTF8)
{
	const char *text = GetText();

	String<> string(text, caretPosition);
	string += codeUTF8;
	int32 length = string.Length();
	string += &text[caretPosition];
	caretPosition = length;

	TextWidget::SetText(string);
}

void EditTextWidget::RemoveGlyphCode(int32 count)
{
	const char *text = GetText();

	String<> string(text, caretPosition);
	string += &text[caretPosition + count];
	TextWidget::SetText(string);
}

void EditTextWidget::ClearSelection(void)
{
	editTextState &= ~kEditTextSelection;
	caretPosition = selectionBegin;

	const char *text = GetText();
	String<> string(text, selectionBegin);
	string += &text[selectionEnd];

	TextWidget::SetText(string);
}

void EditTextWidget::ClearAll(void)
{
	editTextFlags &= ~kEditTextEditClear;
	editTextState &= ~kEditTextSelection;
	caretPosition = 0;

	TextWidget::SetText("");
}

int32 EditTextWidget::CalculateTextPosition(const Point3D& mousePosition) const
{
	const Font *font = GetFont();
	float x = mousePosition.x + font->GetGlyphWidth(0x0020) * 0.5F;

	if (editTextFlags & kEditTextMultiline)
	{
		int32 lineIndex = GetFirstRenderLine() + MaxZero(Min((int32) (mousePosition.y / GetLineSpacing()), GetRenderLineCount() - 1));
		int32 lastLine = MaxZero(GetLineCount() - 1);

		if (lineIndex > lastLine)
		{
			return (GetTextLength());
		}

		int32 lineBegin = GetLineBegin(lineIndex);
		return (Min(font->GetTextLengthFitWidthUTF8(GetText() + lineBegin, x) + lineBegin, GetLineEnd(lineIndex)));
	}

	return (font->GetTextLengthFitWidthUTF8(GetText(), x - GetTextRenderOffset().x));
}

int32 EditTextWidget::CalculateLineIndex(int32 position) const
{
	int32 count = GetLineCount();
	for (machine a = 0; a < count; a++)
	{
		if (position < GetLineEnd(a))
		{
			return (a);
		}
	}

	return (MaxZero(count - 1));
}

void EditTextWidget::UpdateSelection(const Point3D& mousePosition)
{
	int32 position = CalculateTextPosition(mousePosition);

	if (position == selectionAnchor)
	{
		if (!(editTextState & kEditTextCaretVisible))
		{
			caretPosition = selectionAnchor;
			selectionBegin = selectionAnchor;
			selectionEnd = selectionAnchor;

			unsigned_int32 state = editTextState;
			if (state & kEditTextSelectWord)
			{
				ExpandSelection();
			}

			if (selectionBegin == selectionEnd)
			{
				editTextState = (state & ~kEditTextSelection) | kEditTextCaretVisible;
			}
			else
			{
				editTextState = (editTextState & ~kEditTextCaretVisible) | kEditTextSelection;
			}

			SetBuildFlag();
		}
	}
	else if (position < selectionAnchor)
	{
		if ((!(editTextState & kEditTextSelection)) || (selectionBegin != position))
		{
			caretPosition = position;
			selectionBegin = position;
			selectionEnd = selectionAnchor;
			selectionDirection = -1;

			unsigned_int32 state = editTextState;
			editTextState = (editTextState & ~kEditTextCaretVisible) | kEditTextSelection;

			if (state & kEditTextSelectWord)
			{
				ExpandSelection();
			}

			SetBuildFlag();
		}
	}
	else
	{
		if ((!(editTextState & kEditTextSelection)) || (selectionEnd != position))
		{
			caretPosition = position;
			selectionEnd = position;
			selectionBegin = selectionAnchor;
			selectionDirection = 1;

			unsigned_int32 state = editTextState;
			editTextState = (editTextState & ~kEditTextCaretVisible) | kEditTextSelection;

			if (state & kEditTextSelectWord)
			{
				ExpandSelection();
			}

			SetBuildFlag();
		}
	}
}

void EditTextWidget::ExpandSelection(void)
{
	int32 begin = selectionBegin;
	int32 end = selectionEnd;

	const unsigned_int8 *text = reinterpret_cast<const unsigned_int8 *>(GetText());
	const int8 *state = Data::identifierCharState;

	if ((begin == end) || (state[text[MaxZero(end - 1)]] == 1))
	{
		for (;; end++)
		{
			if (state[text[end]] != 1)
			{
				break;
			}
		}
	}

	if ((begin == end) || (state[text[begin]] == 1))
	{
		while (begin > 0)
		{
			int32 x = begin - 1;
			if (state[text[x]] != 1)
			{
				break;
			}

			begin = x;
		}
	}

	selectionBegin = begin;
	selectionEnd = end;
}

void EditTextWidget::SetWidgetState(unsigned_int32 state)
{
	if ((GetWidgetState() ^ state) & kWidgetFocus)
	{
		if (state & kWidgetFocus)
		{
			int32 length = GetTextLength();
			if (length != 0)
			{
				editTextState |= kEditTextSelection;
				caretPosition = 0;
				selectionBegin = 0;
				selectionEnd = length;
			}
			else
			{
				editTextState |= kEditTextCaretVisible;
				caretBlinkTime = TheInterfaceMgr->GetCaretBlinkTime();
			}
		}
		else
		{
			editTextState &= ~(kEditTextCaretVisible | kEditTextSelection);
		}
	}

	TextWidget::SetWidgetState(state);
}

void EditTextWidget::Build(void)
{
	volatile Point2D *restrict backgroundVertex = structureVertexBuffer.BeginUpdate<Point2D>();

	const Vector2D& size = GetWidgetSize();
	float p = -paddingSize;
	float w = size.x - p;
	float h = size.y - p;
	Box2D box(Point2D(p, p), Point2D(w, h));

	backgroundVertex[0].Set(p, p);
	backgroundVertex[1].Set(p, h);
	backgroundVertex[2].Set(w, p);
	backgroundVertex[3].Set(w, h);

	volatile BorderVertex *restrict borderVertex = reinterpret_cast<volatile BorderVertex *>(backgroundVertex + 4);
	BuildBorder(box, borderVertex);

	volatile GlowVertex *restrict focusVertex = reinterpret_cast<volatile GlowVertex *>(borderVertex + 16);
	BuildGlow(box, focusVertex);

	backgroundColorAttribute.SetDiffuseColor(backgroundColor);
	borderColorAttribute.SetDiffuseColor(borderColor);
	EditTextWidget::SetDynamicWidgetColor(hiliteColor, kWidgetColorHilite);
	focusColorAttribute.SetDiffuseColor(focusColor);
	caretColorAttribute.SetDiffuseColor(caretColor);

	const Font *font = GetFont();
	if (font)
	{
		const char *text = GetText();

		float x = 0.0F;
		float y = 0.0F;

		if (editTextFlags & kEditTextMultiline)
		{
			SplitLines();
			int32 lineIndex = CalculateLineIndex(caretPosition);

			int32 firstLine = GetFirstRenderLine();
			int32 lineCount = GetRenderLineCount();

			unsigned_int32 state = editTextState;
			if (lineIndex < firstLine)
			{
				if (state & kEditTextShowCaret)
				{
					editTextState = state & ~(kEditTextCaretHidden | kEditTextShowCaret);
					SetFirstRenderLine(lineIndex);
				}
				else
				{
					editTextState = state | kEditTextCaretHidden;
				}
			}
			else if (lineIndex >= firstLine + lineCount)
			{
				if (state & kEditTextShowCaret)
				{
					editTextState = state & ~(kEditTextCaretHidden | kEditTextShowCaret);
					SetFirstRenderLine(lineIndex - lineCount + 1);
				}
				else
				{
					editTextState = state | kEditTextCaretHidden;
				}
			}
			else
			{
				editTextState = state & ~kEditTextCaretHidden;
			}

			int32 begin = GetLineBegin(lineIndex);
			x = font->GetTextWidthUTF8(text + begin, caretPosition - begin);
			y = (float) (lineIndex - GetFirstRenderLine()) * GetLineSpacing();
		}
		else
		{
			float offset = GetTextRenderOffset().x;
			float caretDistance = font->GetTextWidthUTF8(text, caretPosition);

			if (caretDistance + offset < 0.0F)
			{
				offset = -caretDistance;
			}
			else if (caretDistance + offset > size.x)
			{
				offset = size.x - caretDistance;
			}

			if (offset > 0.0F)
			{
				offset = 0.0F;
			}
			else
			{
				float width = FminZero(size.x - font->GetTextWidthUTF8(text));
				offset = Fmax(offset, width);
			}

			x = caretDistance + offset;
			SetTextRenderOffset(Vector3D(offset, 0.0F, 0.0F));
		}

		volatile Point2D *restrict caretVertex = reinterpret_cast<volatile Point2D *>(focusVertex + 32);

		const FontHeader *fontHeader = font->GetFontHeader();
		float height = fontHeader->fontHeight * 0.875F + fontHeader->fontBaseline * 0.125F;

		caretVertex[0].Set(x, y);
		caretVertex[1].Set(x, y + height - 1.0F);
		caretVertex[2].Set(x + 1.0F, y);
		caretVertex[3].Set(x + 1.0F, y + height - 1.0F);

		if (editTextState & kEditTextSelection)
		{
			if (editTextFlags & kEditTextMultiline)
			{
				int32 hiliteBegin = selectionBegin;
				int32 hiliteEnd = selectionEnd;

				int32 firstLine = GetFirstRenderLine();
				int32 beginLine = Max(CalculateLineIndex(hiliteBegin), firstLine);
				int32 endLine = Min(CalculateLineIndex(hiliteEnd), firstLine + GetRenderLineCount() - 1);

				hiliteVertexBuffer.Establish(sizeof(Point2D) * (endLine - beginLine + 1) * 4);
				volatile Point2D *restrict hiliteVertex = hiliteVertexBuffer.BeginUpdate<Point2D>();

				float lineSpacing = GetLineSpacing();
				y = (float) (beginLine - firstLine) * lineSpacing;

				int32 vertexCount = 0;
				for (machine a = beginLine; a <= endLine; a++)
				{
					int32 lineBegin = GetLineBegin(a);
					int32 lineEnd = GetLineEnd(a);

					int32 i1 = MaxZero(hiliteBegin - lineBegin);
					int32 i2 = Min(hiliteEnd, lineEnd) - lineBegin;
					if (i2 <= i1)
					{
						break;
					}

					const char *t = text + lineBegin;
					float x1 = FmaxZero(font->GetTextWidthUTF8(t, i1)) - 1.0F;
					float x2 = Fmin(font->GetTextWidthUTF8(t, i2), size.x) + 1.0F;

					hiliteVertex[0].Set(x1, y - 1.0F);
					hiliteVertex[1].Set(x1, y + height);
					hiliteVertex[2].Set(x2, y + height);
					hiliteVertex[3].Set(x2, y - 1.0F);

					hiliteVertex += 4;
					vertexCount += 4;
					y += lineSpacing;
				}

				hiliteVertexBuffer.EndUpdate();
				hiliteRenderable.SetVertexCount(vertexCount);
			}
			else
			{
				hiliteVertexBuffer.Establish(sizeof(Point2D) * 4);
				volatile Point2D *restrict hiliteVertex = hiliteVertexBuffer.BeginUpdate<Point2D>();

				float offset = GetTextRenderOffset().x;
				float x1 = FmaxZero(font->GetTextWidthUTF8(text, selectionBegin) + offset) - 1.0F;
				float x2 = Fmin(font->GetTextWidthUTF8(text, selectionEnd) + offset, size.x) + 1.0F;

				hiliteVertex[0].Set(x1, -1.0F);
				hiliteVertex[1].Set(x1, height);
				hiliteVertex[2].Set(x2, height);
				hiliteVertex[3].Set(x2, -1.0F);

				hiliteVertexBuffer.EndUpdate();
				hiliteRenderable.SetVertexCount(4);
			}

			hiliteRenderable.InvalidateVertexData();
		}
	}

	structureVertexBuffer.EndUpdate();

	if (editTextFlags & kEditTextReadOnly)
	{
		editTextState |= kEditTextCaretHidden;
	}

	TextWidget::Build();
}

void EditTextWidget::Render(List<Renderable> *renderList)
{
	if (!(editTextFlags & kEditTextRenderPlain))
	{
		renderList->Append(&backgroundRenderable);
		renderList->Append(&borderRenderable);

		if ((GetWidgetState() & kWidgetFocus) && (!(editTextFlags & kEditTextFocusPlain)))
		{
			renderList->Append(&focusRenderable);
		}
	}

	if (GetFont())
	{
		if (editTextState & kEditTextSelection)
		{
			renderList->Append(&hiliteRenderable);
		}

		TextWidget::Render(renderList);

		if ((editTextState & (kEditTextCaretVisible | kEditTextCaretHidden)) == kEditTextCaretVisible)
		{
			renderList->Append(&caretRenderable);
		}
	}
}

void EditTextWidget::HandleMouseEvent(const PanelMouseEventData *eventData)
{
	EventType eventType = eventData->eventType;
	if (eventType == kEventMouseDown)
	{
		const Font *font = GetFont();
		if (font)
		{
			int32 position = CalculateTextPosition(eventData->mousePosition);
			unsigned_int32 state = editTextState;

			if (!InterfaceMgr::GetShiftKey())
			{
				if ((eventData->eventFlags & kMouseDoubleClick) && (!(state & kEditTextSelection)))
				{
					if (editTextFlags & kEditTextDoubleClickSelectAll)
					{
						SelectAll();
					}
					else
					{
						editTextState = state | (kEditTextSelectWord | kEditTextDragSelect);
						selectionBegin = position;
						selectionEnd = position;

						ExpandSelection();
						SetSelection(selectionBegin, selectionEnd);
					}
				}
				else
				{
					caretPosition = position;
					selectionAnchor = position;
					editTextState = (state & ~kEditTextSelection) | (kEditTextCaretVisible | kEditTextDragSelect);
					SetBuildFlag();
				}
			}
			else
			{
				if (state & kEditTextSelection)
				{
					if (position < selectionBegin)
					{
						caretPosition = position;
						selectionBegin = position;
						selectionAnchor = selectionEnd;
						selectionDirection = -1;
						editTextState = state | kEditTextDragSelect;
						SetBuildFlag();
					}
					else if (position > selectionEnd)
					{
						caretPosition = position;
						selectionEnd = position;
						selectionAnchor = selectionBegin;
						selectionDirection = 1;
						editTextState = state | kEditTextDragSelect;
						SetBuildFlag();
					}
				}
				else
				{
					if (position < caretPosition)
					{
						selectionBegin = position;
						selectionEnd = caretPosition;
						selectionAnchor = caretPosition;
						caretPosition = position;
						selectionDirection = -1;
						editTextState = (state & ~kEditTextCaretVisible) | (kEditTextSelection | kEditTextDragSelect);
					}
					else if (position > caretPosition)
					{
						selectionBegin = caretPosition;
						selectionEnd = position;
						selectionAnchor = caretPosition;
						caretPosition = position;
						selectionDirection = 1;
						editTextState = (state & ~kEditTextCaretVisible) | (kEditTextSelection | kEditTextDragSelect);
					}
					else
					{
						caretPosition = position;
						selectionAnchor = position;
						editTextState = (state & ~kEditTextSelection) | (kEditTextCaretVisible | kEditTextDragSelect);
					}

					SetBuildFlag();
				}
			}

			scrollTime = 0;

			PanelController *controller = GetPanelController();
			if (controller)
			{
				controller->BeginKeyboardInteraction(this);
			}
		}
	}
	else if (eventType == kEventMouseUp)
	{
		editTextState = editTextState & ~(kEditTextCaretMemory | kEditTextDragSelect | kEditTextSelectWord);
		caretBlinkTime = TheInterfaceMgr->GetCaretBlinkTime();
	}
	else if (eventType == kEventMouseMoved)
	{
		if (editTextState & kEditTextDragSelect)
		{
			UpdateSelection(eventData->mousePosition);
		}
	}
	else if (eventType == kEventMouseWheel)
	{
		if (editTextFlags & kEditTextMultiline)
		{
			int32 firstLine = GetFirstRenderLine();
			int32 limit = MaxZero(GetLineCount() - GetRenderLineCount());

			int32 line = Min(MaxZero(firstLine - (int32) eventData->wheelDelta.y), limit);
			if (line != firstLine)
			{
				SetFirstRenderLine(line);
				SetBuildFlag();
			}
		}
	}
}

void EditTextWidget::HandleHomeKey(unsigned_int32 modifierKeys)
{
	if (editTextState & kEditTextSelection)
	{
		if (modifierKeys & kModifierKeyShift)
		{
			if (selectionDirection < 0)
			{
				if (caretPosition > 0)
				{
					caretPosition = 0;
					selectionBegin = 0;
				}
			}
			else
			{
				if (selectionBegin == 0)
				{
					editTextState &= ~kEditTextSelection;
				}
				else
				{
					selectionEnd = selectionBegin;
					selectionBegin = 0;
					selectionDirection = -1;
				}

				caretPosition = selectionEnd;
			}
		}
		else
		{
			caretPosition = 0;
			editTextState &= ~kEditTextSelection;
		}

		SetBuildFlag();
	}
	else if (caretPosition > 0)
	{
		if (modifierKeys & kModifierKeyShift)
		{
			selectionBegin = 0;
			selectionEnd = caretPosition;
			selectionDirection = -1;

			editTextState = (editTextState | kEditTextSelection) & ~kEditTextCaretVisible;
		}

		caretPosition = 0;
		SetBuildFlag();
	}
}

void EditTextWidget::HandleEndKey(unsigned_int32 modifierKeys)
{
	int32 length = GetTextLength();

	if (editTextState & kEditTextSelection)
	{
		if (modifierKeys & kModifierKeyShift)
		{
			if (selectionDirection >= 0)
			{
				if (caretPosition < length)
				{
					caretPosition = length;
					selectionEnd = length;
				}
			}
			else
			{
				if (selectionEnd == length)
				{
					editTextState &= ~kEditTextSelection;
				}
				else
				{
					selectionBegin = selectionEnd;
					selectionEnd = length;
					selectionDirection = 1;
				}

				caretPosition = selectionBegin;
			}
		}
		else
		{
			caretPosition = length;
			editTextState &= ~kEditTextSelection;
		}

		SetBuildFlag();
	}
	else if (caretPosition < length)
	{
		if (modifierKeys & kModifierKeyShift)
		{
			selectionBegin = caretPosition;
			selectionEnd = length;
			selectionDirection = 1;

			editTextState = (editTextState | kEditTextSelection) & ~kEditTextCaretVisible;
		}

		caretPosition = length;
		SetBuildFlag();
	}
}

void EditTextWidget::HandleLeftArrowKey(unsigned_int32 modifierKeys)
{
	if (editTextState & kEditTextSelection)
	{
		if (modifierKeys & kModifierKeyShift)
		{
			if (selectionDirection < 0)
			{
				if (caretPosition > 0)
				{
					caretPosition -= Text::GetPreviousGlyphByteCountUTF8(&GetText()[caretPosition], caretPosition);
					selectionBegin = caretPosition;
				}
			}
			else
			{
				selectionEnd -= Text::GetPreviousGlyphByteCountUTF8(&GetText()[selectionEnd], selectionEnd - selectionBegin);
				if (selectionEnd == selectionBegin)
				{
					editTextState &= ~kEditTextSelection;
				}

				caretPosition = selectionEnd;
			}
		}
		else
		{
			caretPosition = selectionBegin;
			editTextState &= ~kEditTextSelection;
		}

		SetBuildFlag();
	}
	else if (caretPosition > 0)
	{
		int32 count = Text::GetPreviousGlyphByteCountUTF8(&GetText()[caretPosition], caretPosition);

		if (modifierKeys & kModifierKeyShift)
		{
			selectionBegin = caretPosition - count;
			selectionEnd = caretPosition;
			selectionDirection = -1;

			editTextState = (editTextState | kEditTextSelection) & ~kEditTextCaretVisible;
		}

		caretPosition -= count;
		SetBuildFlag();
	}
}

void EditTextWidget::HandleRightArrowKey(unsigned_int32 modifierKeys)
{
	if (editTextState & kEditTextSelection)
	{
		if (modifierKeys & kModifierKeyShift)
		{
			if (selectionDirection >= 0)
			{
				if (caretPosition < GetTextLength())
				{
					caretPosition += Text::GetNextGlyphByteCountUTF8(&GetText()[caretPosition], GetTextLength() - caretPosition);
					selectionEnd = caretPosition;
				}
			}
			else
			{
				selectionBegin += Text::GetNextGlyphByteCountUTF8(&GetText()[selectionBegin], selectionEnd - selectionBegin);
				if (selectionBegin == selectionEnd)
				{
					editTextState &= ~kEditTextSelection;
				}

				caretPosition = selectionBegin;
			}
		}
		else
		{
			caretPosition = selectionEnd;
			editTextState &= ~kEditTextSelection;
		}

		SetBuildFlag();
	}
	else
	{
		int32 length = GetTextLength();
		if (caretPosition < length)
		{
			int32 count = Text::GetNextGlyphByteCountUTF8(&GetText()[caretPosition], length - caretPosition);

			if (modifierKeys & kModifierKeyShift)
			{
				selectionBegin = caretPosition;
				selectionEnd = caretPosition + count;
				selectionDirection = 1;

				editTextState = (editTextState | kEditTextSelection) & ~kEditTextCaretVisible;
			}

			caretPosition += count;
			SetBuildFlag();
		}
	}
}

void EditTextWidget::HandleUpArrowKey(unsigned_int32 modifierKeys)
{
	int32 index = CalculateLineIndex(caretPosition);
	if (index > 0)
	{
		const Font *font = GetFont();
		int32 lineBegin = GetLineEnd(index - 1);

		const char *text = GetText();
		unsigned_int32 state = editTextState;

		if (!(state & kEditTextCaretMemory))
		{
			editTextState = state | kEditTextCaretMemory;
			caretMemory = font->GetTextWidthUTF8(text + lineBegin, caretPosition - lineBegin);
		}

		float width = caretMemory + font->GetGlyphWidth(0x0020) * 0.5F;

		int32 prevBegin = GetLineBegin(index - 1);
		int32 position = Min(font->GetTextLengthFitWidthUTF8(text + prevBegin, width) + prevBegin, lineBegin);

		if ((position == lineBegin) && (lineBegin > 0) && (text[lineBegin - 1] == 0x0020))
		{
			position--;
		}

		if (editTextState & kEditTextSelection)
		{
			if (modifierKeys & kModifierKeyShift)
			{
				if (selectionDirection < 0)
				{
					caretPosition = position;
					selectionBegin = position;
				}
				else
				{
					caretPosition = Max(position, selectionBegin);
					if (caretPosition == selectionBegin)
					{
						editTextState &= ~kEditTextSelection;
					}
					else
					{
						selectionEnd = caretPosition;
					}
				}
			}
			else
			{
				caretPosition = selectionBegin;
				editTextState &= ~kEditTextSelection;
			}
		}
		else
		{
			if (modifierKeys & kModifierKeyShift)
			{
				selectionBegin = position;
				selectionEnd = caretPosition;
				selectionDirection = -1;

				editTextState = (editTextState | kEditTextSelection) & ~kEditTextCaretVisible;
			}

			caretPosition = position;
		}

		SetBuildFlag();
	}
	else
	{
		if (modifierKeys & kModifierKeyShift)
		{
			if (editTextState & kEditTextSelection)
			{
				caretPosition = selectionBegin = 0;
				SetBuildFlag();
			}
		}
		else
		{
			if (editTextState & kEditTextSelection)
			{
				caretPosition = selectionBegin;
				editTextState &= ~kEditTextSelection;
			}
			else
			{
				caretPosition = 0;
			}

			SetBuildFlag();
		}
	}
}

void EditTextWidget::HandleDownArrowKey(unsigned_int32 modifierKeys)
{
	int32 index = CalculateLineIndex(caretPosition);
	int32 lastLine = GetLineCount() - 1;
	if (index < lastLine)
	{
		const Font *font = GetFont();
		int32 lineBegin = GetLineBegin(index);

		const char *text = GetText();
		unsigned_int32 state = editTextState;

		if (!(state & kEditTextCaretMemory))
		{
			editTextState = state | kEditTextCaretMemory;
			caretMemory = font->GetTextWidthUTF8(text + lineBegin, caretPosition - lineBegin);
		}

		float width = caretMemory + font->GetGlyphWidth(0x0020) * 0.5F;

		int32 nextBegin = GetLineEnd(index);
		int32 lineEnd = GetLineEnd(index + 1);
		int32 position = Min(font->GetTextLengthFitWidthUTF8(text + nextBegin, width) + nextBegin, lineEnd);

		if ((position == lineEnd) && (lineEnd > 0) && (text[lineEnd - 1] == 0x0020))
		{
			position--;
		}

		if (editTextState & kEditTextSelection)
		{
			if (modifierKeys & kModifierKeyShift)
			{
				if (selectionDirection >= 0)
				{
					caretPosition = position;
					selectionEnd = position;
				}
				else
				{
					caretPosition = Min(position, selectionEnd);
					if (caretPosition == selectionEnd)
					{
						editTextState &= ~kEditTextSelection;
					}
					else
					{
						selectionBegin = caretPosition;
					}
				}
			}
			else
			{
				caretPosition = selectionEnd;
				editTextState &= ~kEditTextSelection;
			}
		}
		else
		{
			if (modifierKeys & kModifierKeyShift)
			{
				selectionBegin = caretPosition;
				selectionEnd = position;
				selectionDirection = 1;

				editTextState = (editTextState | kEditTextSelection) & ~kEditTextCaretVisible;
			}

			caretPosition = position;
		}

		SetBuildFlag();
	}
	else
	{
		if (modifierKeys & kModifierKeyShift)
		{
			if (editTextState & kEditTextSelection)
			{
				caretPosition = selectionEnd = GetLineEnd(lastLine);
				SetBuildFlag();
			}
		}
		else
		{
			if (editTextState & kEditTextSelection)
			{
				caretPosition = selectionEnd;
				editTextState &= ~kEditTextSelection;
			}
			else
			{
				caretPosition = GetLineEnd(lastLine);
			}

			SetBuildFlag();
		}
	}
}

bool EditTextWidget::HandleKeyboardEvent(const KeyboardEventData *eventData)
{
	EventType eventType = eventData->eventType;
	if (eventType == kEventKeyDown)
	{
		if (!(editTextFlags & kEditTextReadOnly))
		{
			bool post = true;

			unsigned_int32 keyCode = eventData->keyCode;
			if (keyCode == kKeyCodeBackspace)
			{
				if (editTextFlags & kEditTextEditClear)
				{
					ClearAll();
				}
				else
				{
					if (editTextState & kEditTextSelection)
					{
						ClearSelection();
					}
					else if (caretPosition > 0)
					{
						int32 count = Text::GetPreviousGlyphByteCountUTF8(&GetText()[caretPosition], caretPosition);
						caretPosition -= count;
						RemoveGlyphCode(count);
					}
				}

				editTextState &= ~kEditTextCaretMemory;
			}
			else if (keyCode == kKeyCodeDelete)
			{
				if (editTextFlags & kEditTextEditClear)
				{
					ClearAll();
				}
				else
				{
					if (editTextState & kEditTextSelection)
					{
						ClearSelection();
						editTextState &= ~kEditTextCaretMemory;
					}
					else
					{
						int32 length = GetTextLength();
						if (caretPosition < length)
						{
							int32 count = Text::GetNextGlyphByteCountUTF8(&GetText()[caretPosition], length - caretPosition);
							RemoveGlyphCode(count);
						}
					}
				}
			}
			else if (keyCode == kKeyCodeHome)
			{
				HandleHomeKey(eventData->modifierKeys);
				editTextState &= ~kEditTextCaretMemory;
				post = false;
			}
			else if (keyCode == kKeyCodeEnd)
			{
				HandleEndKey(eventData->modifierKeys);
				editTextState &= ~kEditTextCaretMemory;
				post = false;
			}
			else if (keyCode == kKeyCodeLeftArrow)
			{
				HandleLeftArrowKey(eventData->modifierKeys);
				editTextState &= ~kEditTextCaretMemory;
				post = false;
			}
			else if (keyCode == kKeyCodeRightArrow)
			{
				HandleRightArrowKey(eventData->modifierKeys);
				editTextState &= ~kEditTextCaretMemory;
				post = false;
			}
			else if (keyCode == kKeyCodeUpArrow)
			{
				if (editTextFlags & kEditTextMultiline)
				{
					HandleUpArrowKey(eventData->modifierKeys);
				}

				post = false;
			}
			else if (keyCode == kKeyCodeDownArrow)
			{
				if (editTextFlags & kEditTextMultiline)
				{
					HandleDownArrowKey(eventData->modifierKeys);
				}

				post = false;
			}
			else if ((keyCode >= 0x0020) && (keyCode - 0x007F >= 0x0021U))
			{
				if (editTextFlags & kEditTextEditClear)
				{
					ClearAll();
				}

				InsertGlyph(keyCode);
				editTextState &= ~kEditTextCaretMemory;
			}
			else
			{
				return (false);
			}

			if (!(editTextState & kEditTextSelection))
			{
				caretBlinkTime = TheInterfaceMgr->GetCaretBlinkTime();
				editTextState |= kEditTextCaretVisible;

				if (post)
				{
					PostWidgetEvent(WidgetEventData(kEventWidgetChange));
				}
			}

			editTextState |= kEditTextShowCaret;
		}

		return (true);
	}
	else if (eventType == kEventKeyCommand)
	{
		unsigned_int32 keyCode = eventData->keyCode;

		if (keyCode == 'A')
		{
			SelectAll();
			return (true);
		}
		else if ((keyCode == 'C') || (keyCode == 'X'))
		{
			if (editTextState & kEditTextSelection)
			{
				String<>& clipboard = TheInterfaceMgr->GetClipboard();
				clipboard.Set(&GetText()[selectionBegin], selectionEnd - selectionBegin);

				if ((keyCode == 'X') && (!(editTextFlags & kEditTextReadOnly)))
				{
					if (editTextFlags & kEditTextEditClear)
					{
						ClearAll();
					}
					else
					{
						ClearSelection();
					}

					caretBlinkTime = TheInterfaceMgr->GetCaretBlinkTime();
					editTextState |= kEditTextCaretVisible;
					PostWidgetEvent(WidgetEventData(kEventWidgetChange));
				}
			}

			editTextState = (editTextState | kEditTextShowCaret) & ~kEditTextCaretMemory;
			return (true);
		}
		else if ((keyCode == 'V') && (!(editTextFlags & kEditTextReadOnly)))
		{
			const String<>& clipboard = TheInterfaceMgr->GetClipboard();
			if (clipboard.Length() > 0)
			{
				if (editTextFlags & kEditTextEditClear)
				{
					ClearAll();
				}
				else if (editTextState & kEditTextSelection)
				{
					ClearSelection();
				}

				for (machine a = 0;;)
				{
					unsigned_int32	glyph;

					a += Text::ReadGlyphCodeUTF8(&clipboard[a], &glyph);
					if (glyph == 0)
					{
						break;
					}

					InsertGlyph(glyph);
				}

				caretBlinkTime = TheInterfaceMgr->GetCaretBlinkTime();
				editTextState = (editTextState | (kEditTextCaretVisible | kEditTextShowCaret)) & ~kEditTextCaretMemory;
				PostWidgetEvent(WidgetEventData(kEventWidgetChange));
			}

			return (true);
		}
	}

	return (false);
}

void EditTextWidget::TrackTask(WidgetPart widgetPart, const Point3D& mousePosition)
{
	if (editTextState & kEditTextDragSelect)
	{
		int32 dt = TheTimeMgr->GetSystemDeltaTime();

		if (editTextFlags & kEditTextMultiline)
		{
			if (mousePosition.y < 0.0F)
			{
				scrollTime += dt;
				int32 d = scrollTime >> 5;
				scrollTime -= d << 5;

				if (d != 0)
				{
					SetFirstRenderLine(MaxZero(GetFirstRenderLine() - d));
					UpdateSelection(mousePosition);
					SetBuildFlag();
				}
			}
			else if (mousePosition.y > GetWidgetSize().y)
			{
				scrollTime += dt;
				int32 d = scrollTime >> 5;
				scrollTime -= d << 5;

				if (d != 0)
				{
					int32 firstLine = GetFirstRenderLine();
					int32 limit = MaxZero(GetLineCount() - GetRenderLineCount());

					SetFirstRenderLine(Min(firstLine + d, limit));
					UpdateSelection(mousePosition);
					SetBuildFlag();
				}
			}
			else
			{
				scrollTime = 0;
			}
		}
		else
		{
			float offset = GetTextRenderOffset().x;

			if (mousePosition.x < 0.0F)
			{
				scrollTime += dt;
				int32 d = scrollTime >> 5;
				scrollTime -= d << 5;

				if (d != 0)
				{
					SetTextRenderOffset(Vector3D(offset + 6.0F, 0.0F, 0.0F));
					UpdateSelection(mousePosition);
					SetBuildFlag();
				}
			}
			else if (mousePosition.x > GetWidgetSize().x)
			{
				scrollTime += dt;
				int32 d = scrollTime >> 5;
				scrollTime -= d << 5;

				if (d != 0)
				{
					SetTextRenderOffset(Vector3D(offset - 6.0F, 0.0F, 0.0F));
					UpdateSelection(mousePosition);
					SetBuildFlag();
				}
			}
			else
			{
				scrollTime = 0;
			}
		}
	}
}

void EditTextWidget::FocusTask(void)
{
	unsigned_int32 state = editTextState;
	if (!(state & (kEditTextSelection | kEditTextDragSelect)))
	{
		int32 time = caretBlinkTime - TheTimeMgr->GetSystemDeltaTime();
		if (time < 0)
		{
			editTextState = state ^ kEditTextCaretVisible;
			time = TheInterfaceMgr->GetCaretBlinkTime();
		}

		caretBlinkTime = time;
	}
}


PasswordWidget::PasswordWidget() : EditTextWidget(kWidgetPassword)
{
	passwordFlags = 0;
	passwordString[0] = 0;

	SetEditTextFlags(kEditTextDoubleClickSelectAll);
}

PasswordWidget::PasswordWidget(const Vector2D& size, int32 maxGlyph, const char *font, bool secure) : EditTextWidget(kWidgetPassword, size, maxGlyph, font)
{
	passwordFlags = (secure) ? kPasswordSecure : 0;
	passwordString[0] = 0;

	SetEditTextFlags(kEditTextDoubleClickSelectAll);
}

PasswordWidget::PasswordWidget(const PasswordWidget& passwordWidget) : EditTextWidget(passwordWidget)
{
	passwordFlags = passwordWidget.passwordFlags;
	passwordString[0] = 0;

	SetEditTextFlags(kEditTextDoubleClickSelectAll);
}

PasswordWidget::~PasswordWidget()
{
	if (passwordFlags & kPasswordLocked)
	{
		MemoryMgr::ClearMemory(&passwordString, kMaxPasswordLength);
		MemoryMgr::UnlockMemory(&passwordString, sizeof(PasswordString));
	}
}

Widget *PasswordWidget::Replicate(void) const
{
	return (new PasswordWidget(*this));
}

void PasswordWidget::Pack(Packer& data, unsigned_int32 packFlags) const
{
	EditTextWidget::Pack(data, packFlags);

	data << ChunkHeader('FLAG', 4);
	data << unsigned_int32(passwordFlags & ~kPasswordLocked);

	data << TerminatorChunk;
}

void PasswordWidget::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	EditTextWidget::Unpack(data, unpackFlags);

	#if C4LEGACY

		if (data.GetVersion() >= 63)
		{

	#endif

			UnpackChunkList<PasswordWidget>(data, unpackFlags);

	#if C4LEGACY

		}

	#endif
}

bool PasswordWidget::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> passwordFlags;
			return (true);
	}

	return (false);
}

int32 PasswordWidget::GetSettingCount(void) const
{
	return (EditTextWidget::GetSettingCount() + 2);
}

Setting *PasswordWidget::GetSetting(int32 index) const
{
	int32 count = EditTextWidget::GetSettingCount();
	if (index < count)
	{
		return (EditTextWidget::GetSetting(index));
	}

	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetPassword, 'SETT'));
		return (new HeadingSetting(kWidgetPassword, title));
	}

	if (index == count + 1)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetPassword, 'SECR'));
		return (new BooleanSetting('SECR', ((passwordFlags & kPasswordSecure) != 0), title));
	}

	return (nullptr);
}

void PasswordWidget::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'SECR')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			passwordFlags |= kPasswordSecure;
		}
		else
		{
			passwordFlags &= ~kPasswordSecure;
		}
	}
	else
	{
		EditTextWidget::SetSetting(setting);
	}
}

void PasswordWidget::Preprocess(void)
{
	EditTextWidget::Preprocess();

	if (!GetManipulator())
	{
		unsigned_int32 flags = passwordFlags;
		if (flags & kPasswordSecure)
		{
			passwordFlags = flags | kPasswordLocked;
			MemoryMgr::LockMemory(&passwordString, sizeof(PasswordString));
		}
	}
}

void PasswordWidget::SetText(const char *text, int32 max)
{
	static char dotText[kMaxPasswordLength * 3 + 1];

	if (max == 0)
	{
		max = GetMaxGlyphCount();
	}
	else
	{
		max = Min(max, GetMaxGlyphCount());
	}

	int32 length = 0;
	int32 count = 0;
	for (;;)
	{
		unsigned_int32	code;

		int32 size = Text::ReadGlyphCodeUTF8(&text[length], &code);
		if ((code == 0) || (length + size > kMaxPasswordLength))
		{
			break;
		}

		length += size;

		if (++count == max)
		{
			break;
		}
	}

	passwordString.Set(text, length);

	char *c = dotText;
	for (machine a = 0; a < count; a++)
	{
		c[0] = '\xE2';		// U+2022
		c[1] = '\x80';
		c[2] = '\xA2';
		c += 3;
	}

	c[0] = 0;
	EditTextWidget::SetText(dotText);
}

void PasswordWidget::AppendText(const char *text, int32 max)
{
	String<kMaxPasswordLength * 2> string(passwordString);
	PasswordWidget::SetText(string += text, max);
	MemoryMgr::ClearMemory(&string, kMaxPasswordLength * 2);
}

void PasswordWidget::PrependText(const char *text, int32 max)
{
	String<kMaxPasswordLength * 2> string(text);
	PasswordWidget::SetText(string += passwordString, max);
	MemoryMgr::ClearMemory(&string, kMaxPasswordLength * 2);
}

void PasswordWidget::InsertGlyphCode(const char *codeUTF8)
{
	static const char dotCode[4] = "\xE2\x80\xA2";		// U+2022

	int32 position = Text::GetGlyphStringByteCountUTF8(passwordString, GetCaretPosition() / 3);

	String<kMaxPasswordLength * 2> string(passwordString, position);
	string += codeUTF8;
	string += &passwordString[position];
	passwordString = string;

	EditTextWidget::InsertGlyphCode(dotCode);
}

void PasswordWidget::RemoveGlyphCode(int32 count)
{
	int32 position = Text::GetGlyphStringByteCountUTF8(passwordString, GetCaretPosition() / 3);

	String<kMaxPasswordLength * 2> string(passwordString, position);
	string += &passwordString[position + Text::GetNextGlyphByteCountUTF8(&passwordString[position], passwordString.Length() - position)];
	passwordString = string;

	EditTextWidget::RemoveGlyphCode(count);
}

void PasswordWidget::ClearSelection(void)
{
	int32 begin = Text::GetGlyphStringByteCountUTF8(passwordString, GetSelectionBegin() / 3);
	int32 end = Text::GetGlyphStringByteCountUTF8(passwordString, GetSelectionEnd() / 3);

	String<kMaxPasswordLength * 2> string(passwordString, begin);
	string += &passwordString[end];
	passwordString = string;

	EditTextWidget::ClearSelection();
}


QuadWidget::QuadWidget(WidgetType type) :
		RenderableWidget(type, kRenderTriangleStrip),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
{
	SetBaseWidgetType(kWidgetQuad);

	quadOffset.Set(0.0F, 0.0F);
	quadScale.Set(1.0F, 1.0F);

	for (machine a = 0; a < 4; a++)
	{
		quadColor[a].Set(1.0F, 1.0F, 1.0F, 1.0F);
	}
}

QuadWidget::QuadWidget(const Vector2D& size, const ColorRGBA& color) : QuadWidget(kWidgetQuad, size, color)
{
}

QuadWidget::QuadWidget(WidgetType type, const Vector2D& size, const ColorRGBA& color) :
		RenderableWidget(type, kRenderTriangleStrip, size),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
{
	SetBaseWidgetType(kWidgetQuad);
	RenderableWidget::SetWidgetColor(color);

	quadOffset.Set(0.0F, 0.0F);
	quadScale.Set(1.0F, 1.0F);
}

QuadWidget::QuadWidget(const QuadWidget& quadWidget) :
		RenderableWidget(quadWidget),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
{
	quadOffset = quadWidget.quadOffset;
	quadScale = quadWidget.quadScale;

	for (machine a = 0; a < 4; a++)
	{
		quadColor[a] = quadWidget.quadColor[a];
	}
}

QuadWidget::~QuadWidget()
{
}

Widget *QuadWidget::Replicate(void) const
{
	return (new QuadWidget(*this));
}

void QuadWidget::Pack(Packer& data, unsigned_int32 packFlags) const
{
	RenderableWidget::Pack(data, packFlags);

	data << ChunkHeader('OFST', sizeof(Vector2D));
	data << quadOffset;

	data << ChunkHeader('SCAL', sizeof(Vector2D));
	data << quadScale;

	data << ChunkHeader('COLR', sizeof(ColorRGBA) * 4);
	for (machine a = 0; a < 4; a++)
	{
		data << quadColor[a];
	}

	data << TerminatorChunk;
}

void QuadWidget::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	RenderableWidget::Unpack(data, unpackFlags);
	UnpackChunkList<QuadWidget>(data, unpackFlags);
}

bool QuadWidget::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'OFST':

			data >> quadOffset;
			return (true);

		case 'SCAL':

			data >> quadScale;
			return (true);

		case 'COLR':

			for (machine a = 0; a < 4; a++)
			{
				data >> quadColor[a];
			}

			return (true);
	}

	return (false);
}

void QuadWidget::SetDynamicWidgetColor(const ColorRGBA& color, WidgetColorType type)
{
	if (type == kWidgetColorDefault)
	{
		for (machine a = 0; a < 4; a++)
		{
			quadColor[a] = color;
		}

		SetBuildFlag();
	}
}

void QuadWidget::SetDynamicWidgetAlpha(float alpha, WidgetColorType type)
{
	if (type == kWidgetColorDefault)
	{
		for (machine a = 0; a < 4; a++)
		{
			quadColor[a].alpha = alpha;
		}

		SetBuildFlag();
	}
}

void QuadWidget::Preprocess(void)
{
	RenderableWidget::Preprocess();

	SetVertexCount(4);
	SetVertexBuffer(kVertexBufferAttributeArray0, &vertexBuffer, sizeof(QuadVertex));
	SetVertexAttributeArray(kArrayPosition, 0, 2);
	SetVertexAttributeArray(kArrayColor, sizeof(Point2D), 4);
	vertexBuffer.Establish(sizeof(QuadVertex) * 4);
}

void QuadWidget::SetQuadOffset(const Vector2D& offset)
{
	quadOffset = offset;
	SetBuildFlag();
}

void QuadWidget::SetQuadScale(const Vector2D& scale)
{
	quadScale = scale;
	SetBuildFlag();
}

void QuadWidget::Build(void)
{
	volatile QuadVertex *restrict vertex = vertexBuffer.BeginUpdate<QuadVertex>();

	float x = quadOffset.x;
	float y = quadOffset.y;

	const Vector2D& size = GetWidgetSize();
	float w = size.x * quadScale.x;
	float h = size.y * quadScale.y;

	vertex[0].position.Set(x, y);
	vertex[0].color = quadColor[0];

	vertex[1].position.Set(x, y + h);
	vertex[1].color = quadColor[1];

	vertex[2].position.Set(x + w, y);
	vertex[2].color = quadColor[2];

	vertex[3].position.Set(x + w, y + h);
	vertex[3].color = quadColor[3];

	vertexBuffer.EndUpdate();
}


ImageWidget::ImageWidget(WidgetType type) :
		QuadWidget(type),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
{
	imageOffset.Set(0.0F, 0.0F);
	imageScale.Set(1.0F, 1.0F);
	imageRotation = 0.0F;

	imageBlendState = kBlendInterpolate;

	imagePCoordinate = 0.0F;
	cubeFaceIndex = 0;
}

ImageWidget::ImageWidget(const Vector2D& size) : ImageWidget(kWidgetImage, size)
{
}

ImageWidget::ImageWidget(const Vector2D& size, const char *name) : ImageWidget(kWidgetImage, size, name)
{
}

ImageWidget::ImageWidget(const Vector2D& size, Texture *texture) : ImageWidget(kWidgetImage, size, texture)
{
}

ImageWidget::ImageWidget(const Vector2D& size, const TextureHeader *header, const void *image) : ImageWidget(kWidgetImage, size, header, image)
{
}

ImageWidget::ImageWidget(WidgetType type, const Vector2D& size) :
		QuadWidget(type, size),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
{
	imageOffset.Set(0.0F, 0.0F);
	imageScale.Set(1.0F, 1.0F);
	imageRotation = 0.0F;

	imageBlendState = kBlendInterpolate;

	imagePCoordinate = 0.0F;
	cubeFaceIndex = 0;
}

ImageWidget::ImageWidget(WidgetType type, const Vector2D& size, const char *name) :
		QuadWidget(type, size),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		textureAttribute1(name)
{
	imageOffset.Set(0.0F, 0.0F);
	imageScale.Set(1.0F, 1.0F);
	imageRotation = 0.0F;

	imageBlendState = kBlendInterpolate;

	imagePCoordinate = 0.0F;
	cubeFaceIndex = 0;
}

ImageWidget::ImageWidget(WidgetType type, const Vector2D& size, Texture *texture) :
		QuadWidget(type, size),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		textureAttribute1(texture)
{
	imageOffset.Set(0.0F, 0.0F);
	imageScale.Set(1.0F, 1.0F);
	imageRotation = 0.0F;

	imageBlendState = kBlendInterpolate;

	imagePCoordinate = 0.0F;
	cubeFaceIndex = 0;
}

ImageWidget::ImageWidget(WidgetType type, const Vector2D& size, const TextureHeader *header, const void *image) :
		QuadWidget(type, size),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		textureAttribute1(header, image)
{
	imageOffset.Set(0.0F, 0.0F);
	imageScale.Set(1.0F, 1.0F);
	imageRotation = 0.0F;

	imageBlendState = kBlendInterpolate;

	imagePCoordinate = 0.0F;
	cubeFaceIndex = 0;
}

ImageWidget::ImageWidget(const ImageWidget& imageWidget) :
		QuadWidget(imageWidget),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		textureAttribute1(imageWidget.textureAttribute1),
		textureAttribute2(imageWidget.textureAttribute2)
{
	imageOffset = imageWidget.imageOffset;
	imageScale = imageWidget.imageScale;
	imageRotation = imageWidget.imageRotation;

	imageBlendState = imageWidget.imageBlendState;

	imagePCoordinate = imageWidget.imagePCoordinate;
	cubeFaceIndex = imageWidget.cubeFaceIndex;
}

ImageWidget::~ImageWidget()
{
}

Widget *ImageWidget::Replicate(void) const
{
	return (new ImageWidget(*this));
}

void ImageWidget::Pack(Packer& data, unsigned_int32 packFlags) const
{
	QuadWidget::Pack(data, packFlags);

	if ((imageOffset.x != 0.0F) || (imageOffset.y != 0.0F))
	{
		data << ChunkHeader('OFST', sizeof(Vector2D));
		data << imageOffset;
	}

	if ((imageScale.x != 1.0F) || (imageScale.y != 1.0F))
	{
		data << ChunkHeader('SCAL', sizeof(Vector2D));
		data << imageScale;
	}

	if (imageRotation != 0.0F)
	{
		data << ChunkHeader('ROTA', 4);
		data << imageRotation;
	}

	data << ChunkHeader('BLND', 4);
	data << imageBlendState;

	if (!(GetWidgetUsage() & kWidgetGeneratedImage))
	{
		const char *name = textureAttribute1.GetTextureName();
		if (name[0] != 0)
		{
			PackHandle handle = data.BeginChunk('TXT1');
			data << name;
			data.EndChunk(handle);
		}
	}

	const char *name = textureAttribute2.GetTextureName();
	if (name[0] != 0)
	{
		PackHandle handle = data.BeginChunk('TXT2');
		data << name;
		data.EndChunk(handle);
	}

	data << TerminatorChunk;
}

void ImageWidget::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	QuadWidget::Unpack(data, unpackFlags);
	UnpackChunkList<ImageWidget>(data, unpackFlags);
}

bool ImageWidget::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'OFST':

			data >> imageOffset;
			return (true);

		case 'SCAL':

			data >> imageScale;
			return (true);

		case 'ROTA':

			data >> imageRotation;
			return (true);

		case 'BLND':

			data >> imageBlendState;
			imageBlendState |= kBlendAlphaPreserve;
			return (true);

		case 'TXT1':
		{
			ResourceName	name;

			data >> name;
			textureAttribute1.SetTexture(name);
			return (true);
		}

		case 'TXT2':
		{
			ResourceName	name;

			data >> name;
			textureAttribute2.SetTexture(name);
			return (true);
		}
	}

	return (false);
}

void *ImageWidget::BeginSettingsUnpack(void)
{
	imageOffset.Set(0.0F, 0.0F);
	imageScale.Set(1.0F, 1.0F);
	imageRotation = 0.0F;

	return (QuadWidget::BeginSettingsUnpack());
}

int32 ImageWidget::GetSettingCount(void) const
{
	return (QuadWidget::GetSettingCount() + 8);
}

Setting *ImageWidget::GetSetting(int32 index) const
{
	int32 count = QuadWidget::GetSettingCount();
	if (index < count)
	{
		return (QuadWidget::GetSetting(index));
	}

	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetImage, 'SETT'));
		return (new HeadingSetting(kWidgetImage, title));
	}

	if (index == count + 1)
	{
		if (GetWidgetUsage() & kWidgetGeneratedImage)
		{
			return (nullptr);
		}

		const char *title = table->GetString(StringID('WDGT', kWidgetImage, 'TXT1'));
		const char *picker = table->GetString(StringID('WDGT', kWidgetImage, 'TPK1'));
		return (new ResourceSetting('TXT1', textureAttribute1.GetTextureName(), title, picker, TextureResource::GetDescriptor()));
	}

	if (index == count + 2)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetImage, 'TXT2'));
		const char *picker = table->GetString(StringID('WDGT', kWidgetImage, 'TPK2'));
		return (new ResourceSetting('TXT2', textureAttribute2.GetTextureName(), title, picker, TextureResource::GetDescriptor()));
	}

	if (index == count + 3)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetImage, 'XSCL'));
		return (new TextSetting('XSCL', imageScale.x, title));
	}

	if (index == count + 4)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetImage, 'YSCL'));
		return (new TextSetting('YSCL', imageScale.y, title));
	}

	if (index == count + 5)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetImage, 'XOFF'));
		return (new TextSetting('XOFF', imageOffset.x, title));
	}

	if (index == count + 6)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetImage, 'YOFF'));
		return (new TextSetting('YOFF', imageOffset.y, title));
	}

	if (index == count + 7)
	{
		int32 selection = 0;
		unsigned_int32 blendState = imageBlendState & kBlendColorMask;

		if (blendState == BlendState(kBlendSourceAlpha, kBlendOne))
		{
			selection = 1;
		}
		else if (blendState == kBlendInterpolate)
		{
			selection = 2;
		}
		else if (blendState == kBlendPremultInterp)
		{
			selection = 3;
		}
		else if (blendState == kBlendReplace)
		{
			selection = 4;
		}

		const char *title = table->GetString(StringID('WDGT', kWidgetImage, 'BLND'));
		MenuSetting *menu = new MenuSetting('BLND', selection, title, 5);

		menu->SetMenuItemString(0, table->GetString(StringID('WDGT', kWidgetImage, 'BLND', 'ADD ')));
		menu->SetMenuItemString(1, table->GetString(StringID('WDGT', kWidgetImage, 'BLND', 'ADDA')));
		menu->SetMenuItemString(2, table->GetString(StringID('WDGT', kWidgetImage, 'BLND', 'TERP')));
		menu->SetMenuItemString(3, table->GetString(StringID('WDGT', kWidgetImage, 'BLND', 'PREM')));
		menu->SetMenuItemString(4, table->GetString(StringID('WDGT', kWidgetImage, 'BLND', 'REPL')));

		return (menu);
	}

	return (nullptr);
}

void ImageWidget::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'TXT1')
	{
		SetTexture(0, static_cast<const ResourceSetting *>(setting)->GetResourceName());
	}
	else if (identifier == 'TXT2')
	{
		SetTexture(1, static_cast<const ResourceSetting *>(setting)->GetResourceName());
	}
	else if (identifier == 'XSCL')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		imageScale.x = Text::StringToFloat(text);
	}
	else if (identifier == 'YSCL')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		imageScale.y = Text::StringToFloat(text);
	}
	else if (identifier == 'XOFF')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		imageOffset.x = Text::StringToFloat(text);
	}
	else if (identifier == 'YOFF')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		imageOffset.y = Text::StringToFloat(text);
	}
	else if (identifier == 'BLND')
	{
		static const unsigned_int32 blendTable[5] =
		{
			kBlendAccumulate, BlendState(kBlendSourceAlpha, kBlendOne), kBlendInterpolate, kBlendPremultInterp, kBlendReplace
		};

		imageBlendState = blendTable[static_cast<const MenuSetting *>(setting)->GetMenuSelection()];
		SetAmbientBlendState(imageBlendState);
	}
	else
	{
		QuadWidget::SetSetting(setting);
	}
}

void ImageWidget::Preprocess(void)
{
	QuadWidget::Preprocess();

	SetVertexBufferArrayFlags((1 << kArrayTexcoord0) | (1 << kArrayTexcoord1));
	vertexBuffer.Establish(96);

	unsigned_int32 blendState = imageBlendState;
	if (blendState != kBlendReplace)
	{
		blendState |= kBlendAlphaPreserve;
	}

	SetAmbientBlendState(blendState);

	if ((!textureAttribute1.GetTexture()) && (!(GetWidgetUsage() & kWidgetGeneratedImage)))
	{
		textureAttribute1.SetTexture("");
	}

	attributeList.Append(&textureAttribute1);
	SetMaterialAttributeList(&attributeList);

	textureAttribute2.SetTexcoordIndex(1);
	GetFirstRenderSegment()->SetMaterialState(kMaterialAlphaSemanticInhibit);
}

void ImageWidget::SetTexture(int32 index, const char *name)
{
	if (name[0] == 0)
	{
		name = nullptr;
	}

	(&textureAttribute1)[index].SetTexture(name);
	InvalidateShaderData();
	SetBuildFlag();
}

void ImageWidget::SetTexture(int32 index, Texture *texture)
{
	(&textureAttribute1)[index].SetTexture(texture);
	InvalidateShaderData();
	SetBuildFlag();
}

void ImageWidget::SetTexture(int32 index, const TextureHeader *header, const void *image)
{
	(&textureAttribute1)[index].SetTexture(header, image);
	InvalidateShaderData();
	SetBuildFlag();
}

void ImageWidget::Build(void)
{
	static const ConstPoint2D baseTexcoord[4] =
	{
		{0.0F, 1.0F}, {0.0F, 0.0F}, {1.0F, 1.0F}, {1.0F, 0.0F}
	};

	Transform4D		imageTransform;
	int32			componentCount[2];

	float rotation = imageRotation;
	if (rotation == 0.0F)
	{
		imageTransform.SetIdentity();
	}
	else
	{
		imageTransform.SetRotationAboutZ(rotation);
	}

	float sx = imageScale.x;
	float sy = imageScale.y;
	imageTransform(0,3) = imageOffset.x + (0.5F - (imageTransform(0,0) + imageTransform(0,1)) * 0.5F) * sx;
	imageTransform(1,3) = imageOffset.y + (0.5F - (imageTransform(1,0) + imageTransform(1,1)) * 0.5F) * sy;
	imageTransform(0,0) *= sx;
	imageTransform(0,1) *= sx;
	imageTransform(1,0) *= sy;
	imageTransform(1,1) *= sy;

	const Texture *texture = textureAttribute1.GetTexture();
	if (texture)
	{
		attributeList.Append(&textureAttribute1);

		switch (texture->GetTextureType())
		{
			case kTexture3D:
			case kTextureArray2D:
			{
				Point3D *texcoord = reinterpret_cast<Point3D *>(imageTexcoord[0]);
				for (machine k = 0; k < 4; k++)
				{
					texcoord[k].Set(imageTransform * baseTexcoord[k], imagePCoordinate);
				}

				componentCount[0] = 3;
				break;
			}

			case kTextureCube:
			{
				Point3D *texcoord = reinterpret_cast<Point3D *>(imageTexcoord[0]);

				switch (cubeFaceIndex)
				{
					case 0:

						texcoord[0].Set(1.0F, 1.0F, 1.0F);
						texcoord[1].Set(1.0F, 1.0F, -1.0F);
						texcoord[2].Set(1.0F, -1.0F, 1.0F);
						texcoord[3].Set(1.0F, -1.0F, -1.0F);
						break;

					case 1:

						texcoord[0].Set(-1.0F, 1.0F, 1.0F);
						texcoord[1].Set(-1.0F, 1.0F, -1.0F);
						texcoord[2].Set(1.0F, 1.0F, 1.0F);
						texcoord[3].Set(1.0F, 1.0F, -1.0F);
						break;

					case 2:

						texcoord[0].Set(1.0F, -1.0F, 1.0F);
						texcoord[1].Set(1.0F, -1.0F, -1.0F);
						texcoord[2].Set(-1.0F, -1.0F, 1.0F);
						texcoord[3].Set(-1.0F, -1.0F, -1.0F);
						break;

					case 3:

						texcoord[0].Set(-1.0F, -1.0F, 1.0F);
						texcoord[1].Set(-1.0F, -1.0F, -1.0F);
						texcoord[2].Set(-1.0F, 1.0F, 1.0F);
						texcoord[3].Set(-1.0F, 1.0F, -1.0F);
						break;

					case 4:

						texcoord[0].Set(-1.0F, 1.0F, 1.0F);
						texcoord[1].Set(1.0F, 1.0F, 1.0F);
						texcoord[2].Set(-1.0F, -1.0F, 1.0F);
						texcoord[3].Set(1.0F, -1.0F, 1.0F);
						break;

					case 5:

						texcoord[0].Set(1.0F, 1.0F, -1.0F);
						texcoord[1].Set(-1.0F, 1.0F, -1.0F);
						texcoord[2].Set(1.0F, -1.0F, -1.0F);
						texcoord[3].Set(-1.0F, -1.0F, -1.0F);
						break;
				}

				componentCount[0] = 3;
				break;
			}

			#if C4RENDER_TEXTURE_RECTANGLE

				case kTextureRectangle:
				{
					float w = (float) texture->GetTextureWidth();
					float h = (float) texture->GetTextureHeight();

					Point2D *texcoord = reinterpret_cast<Point2D *>(imageTexcoord[0]);
					for (machine k = 0; k < 4; k++)
					{
						texcoord[k] = imageTransform * baseTexcoord[k];
						texcoord[k].x *= w;
						texcoord[k].y *= h;
					}

					componentCount[0] = 2;
					break;
				}

			#endif

			default:
			{
				Point2D *texcoord = reinterpret_cast<Point2D *>(imageTexcoord[0]);
				for (machine k = 0; k < 4; k++)
				{
					texcoord[k] = imageTransform * baseTexcoord[k];
				}

				componentCount[0] = 2;
				break;
			}
		}
	}
	else
	{
		textureAttribute1.Detach();
		componentCount[0] = 0;
	}

	texture = textureAttribute2.GetTexture();
	if (texture)
	{
		attributeList.Append(&textureAttribute2);

		switch (texture->GetTextureType())
		{
			case kTexture3D:
			case kTextureArray2D:
			{
				Point3D *texcoord = reinterpret_cast<Point3D *>(imageTexcoord[1]);
				for (machine k = 0; k < 4; k++)
				{
					texcoord[k].Set(baseTexcoord[k], imagePCoordinate);
				}

				componentCount[1] = 3;
				break;
			}

			case kTextureCube:
			{
				Point3D *texcoord = reinterpret_cast<Point3D *>(imageTexcoord[1]);

				switch (cubeFaceIndex)
				{
					case 0:

						texcoord[0].Set(1.0F, 1.0F, 1.0F);
						texcoord[1].Set(1.0F, 1.0F, -1.0F);
						texcoord[2].Set(1.0F, -1.0F, 1.0F);
						texcoord[3].Set(1.0F, -1.0F, -1.0F);
						break;

					case 1:

						texcoord[0].Set(-1.0F, 1.0F, 1.0F);
						texcoord[1].Set(-1.0F, 1.0F, -1.0F);
						texcoord[2].Set(1.0F, 1.0F, 1.0F);
						texcoord[3].Set(1.0F, 1.0F, -1.0F);
						break;

					case 2:

						texcoord[0].Set(1.0F, -1.0F, 1.0F);
						texcoord[1].Set(1.0F, -1.0F, -1.0F);
						texcoord[2].Set(-1.0F, -1.0F, 1.0F);
						texcoord[3].Set(-1.0F, -1.0F, -1.0F);
						break;

					case 3:

						texcoord[0].Set(-1.0F, -1.0F, 1.0F);
						texcoord[1].Set(-1.0F, -1.0F, -1.0F);
						texcoord[2].Set(-1.0F, 1.0F, 1.0F);
						texcoord[3].Set(-1.0F, 1.0F, -1.0F);
						break;

					case 4:

						texcoord[0].Set(-1.0F, 1.0F, 1.0F);
						texcoord[1].Set(1.0F, 1.0F, 1.0F);
						texcoord[2].Set(-1.0F, -1.0F, 1.0F);
						texcoord[3].Set(1.0F, -1.0F, 1.0F);
						break;

					case 5:

						texcoord[0].Set(1.0F, 1.0F, -1.0F);
						texcoord[1].Set(-1.0F, 1.0F, -1.0F);
						texcoord[2].Set(1.0F, -1.0F, -1.0F);
						texcoord[3].Set(-1.0F, -1.0F, -1.0F);
						break;
				}

				componentCount[1] = 3;
				break;
			}

			#if C4RENDER_TEXTURE_RECTANGLE

				case kTextureRectangle:
				{
					float w = (float) texture->GetTextureWidth();
					float h = (float) texture->GetTextureHeight();

					Point2D *texcoord = reinterpret_cast<Point2D *>(imageTexcoord[1]);
					for (machine k = 0; k < 4; k++)
					{
						texcoord[k].Set(baseTexcoord[k].x * w, baseTexcoord[k].y * h);
					}

					componentCount[1] = 2;
					break;
				}

			#endif

			default:
			{
				Point2D *texcoord = reinterpret_cast<Point2D *>(imageTexcoord[1]);
				for (machine k = 0; k < 4; k++)
				{
					texcoord[k] = baseTexcoord[k];
				}

				componentCount[1] = 2;
				break;
			}
		}
	}
	else
	{
		textureAttribute2.Detach();
		componentCount[1] = 0;
	}

	SetVertexAttributeArray(kArrayTexcoord0, 0, componentCount[0]);
	SetVertexAttributeArray(kArrayTexcoord1, componentCount[0] * 4, componentCount[1]);

	unsigned_int32 stride = (componentCount[0] + componentCount[1]) * 4;
	SetVertexBuffer(kVertexBufferAttributeArray1, &vertexBuffer, stride);
	vertexBuffer.Establish(stride * 4);
	InvalidateVertexData();

	volatile float *restrict vertexTexcoord = vertexBuffer.BeginUpdate<float>();
	const float *texcoord0 = imageTexcoord[0];
	const float *texcoord1 = imageTexcoord[1];

	for (machine a = 0; a < 4; a++)
	{
		for (machine b = 0; b < componentCount[0]; b++)
		{
			*vertexTexcoord++ = *texcoord0++;
		}

		for (machine b = 0; b < componentCount[1]; b++)
		{
			*vertexTexcoord++ = *texcoord1++;
		}
	}

	vertexBuffer.EndUpdate();
	QuadWidget::Build();
}


CheckWidget::CheckWidget() :
		TextWidget(kWidgetCheck),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		colorAttribute(kAttributeMutable),
		checkRenderable(kRenderTriangleStrip)
{
	checkValue = 0;
	checkFlags = kCheckUseHighlightColor;
	colorOverrideFlags = 0;

	buttonColor.Set(1.0F, 1.0F, 1.0F);
	hiliteColor.Set(0.0F, 0.0F, 0.0F);
}

CheckWidget::CheckWidget(const Vector2D& size, const char *text, const char *font) :
		TextWidget(kWidgetCheck, size, text, font),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		colorAttribute(kAttributeMutable),
		checkRenderable(kRenderTriangleStrip)
{
	checkValue = 0;
	checkFlags = kCheckUseHighlightColor;
	colorOverrideFlags = 0;

	buttonColor.Set(1.0F, 1.0F, 1.0F);
	hiliteColor.Set(0.0F, 0.0F, 0.0F);
}

CheckWidget::CheckWidget(const CheckWidget& checkWidget) :
		TextWidget(checkWidget),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		colorAttribute(kAttributeMutable),
		checkRenderable(kRenderTriangleStrip)
{
	checkValue = checkWidget.checkValue;
	checkFlags = checkWidget.checkFlags;
	colorOverrideFlags = checkWidget.colorOverrideFlags;

	buttonColor = checkWidget.buttonColor;
	hiliteColor = checkWidget.hiliteColor;
}

CheckWidget::~CheckWidget()
{
}

Widget *CheckWidget::Replicate(void) const
{
	return (new CheckWidget(*this));
}

void CheckWidget::SetValue(int32 value, bool post)
{
	if (checkValue != value)
	{
		checkValue = value;
		SetBuildFlag();

		if (post)
		{
			PostWidgetEvent(WidgetEventData(kEventWidgetChange));
		}
	}
}

void CheckWidget::SetCheckFlags(unsigned_int32 flags)
{
	checkFlags = flags;
	SetBuildFlag();
}

void CheckWidget::Pack(Packer& data, unsigned_int32 packFlags) const
{
	TextWidget::Pack(data, packFlags);

	data << ChunkHeader('VALU', 4);
	data << checkValue;

	data << ChunkHeader('FLAG', 4);
	data << checkFlags;

	data << ChunkHeader('OVER', 4);
	data << colorOverrideFlags;

	data << ChunkHeader('BTTC', sizeof(ColorRGBA));
	data << buttonColor;

	if (colorOverrideFlags & kWidgetOverrideHiliteColor)
	{
		data << ChunkHeader('HILC', sizeof(ColorRGBA));
		data << hiliteColor;
	}

	data << TerminatorChunk;
}

void CheckWidget::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	TextWidget::Unpack(data, unpackFlags);
	UnpackChunkList<CheckWidget>(data, unpackFlags);
}

bool CheckWidget::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'VALU':

			data >> checkValue;
			return (true);

		case 'FLAG':

			data >> checkFlags;
			return (true);

		case 'OVER':

			data >> colorOverrideFlags;
			return (true);

		case 'BTTC':

			data >> buttonColor;
			return (true);

		case 'HILC':

			data >> hiliteColor;
			return (true);
	}

	return (false);
}

void *CheckWidget::BeginSettingsUnpack(void)
{
	colorOverrideFlags = 0;
	return (TextWidget::BeginSettingsUnpack());
}

int32 CheckWidget::GetSettingCount(void) const
{
	return (TextWidget::GetSettingCount() + 6);
}

Setting *CheckWidget::GetSetting(int32 index) const
{
	int32 count = TextWidget::GetSettingCount();
	if (index < count)
	{
		return (TextWidget::GetSetting(index));
	}

	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetCheck, 'SETT'));
		return (new HeadingSetting(kWidgetCheck, title));
	}

	if (index == count + 1)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetCheck, 'INIT'));
		return (new BooleanSetting('CHIN', (checkValue != 0), title));
	}

	if (index == count + 2)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetCheck, 'SLCT'));
		return (new BooleanSetting('CHSL', ((checkFlags & kCheckUseHighlightColor) != 0), title));
	}

	if (index == count + 3)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetCheck, 'TXIN'));
		return (new BooleanSetting('CHTI', ((checkFlags & kCheckTextInactive) != 0), title));
	}

	if (index == count + 4)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetCheck, 'BTTC'));
		const char *picker = table->GetString(StringID('WDGT', kWidgetCheck, 'BTTP'));
		return (new ColorSetting('CHBC', buttonColor, title, picker));
	}

	if (index == count + 5)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetCheck, 'HILC'));
		const char *picker = table->GetString(StringID('WDGT', kWidgetCheck, 'HILP'));
		return (new CheckColorSetting('CHHC', ((colorOverrideFlags & kWidgetOverrideHiliteColor) != 0), hiliteColor, title, picker));
	}

	return (nullptr);
}

void CheckWidget::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'CHIN')
	{
		checkValue = static_cast<const BooleanSetting *>(setting)->GetBooleanValue();
	}
	else if (identifier == 'CHSL')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			checkFlags |= kCheckUseHighlightColor;
		}
		else
		{
			checkFlags &= ~kCheckUseHighlightColor;
		}
	}
	else if (identifier == 'CHTI')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			checkFlags |= kCheckTextInactive;
		}
		else
		{
			checkFlags &= ~kCheckTextInactive;
		}
	}
	else if (identifier == 'CHBC')
	{
		buttonColor = static_cast<const ColorSetting *>(setting)->GetColor();
	}
	else if (identifier == 'CHHC')
	{
		const CheckColorSetting *checkColorSetting = static_cast<const CheckColorSetting *>(setting);
		if (checkColorSetting->GetCheckValue() != 0)
		{
			colorOverrideFlags |= kWidgetOverrideHiliteColor;
			hiliteColor = checkColorSetting->GetColor();
		}
		else
		{
			colorOverrideFlags &= ~kWidgetOverrideHiliteColor;
			SetDefaultHiliteColor();
		}
	}
	else
	{
		TextWidget::SetSetting(setting);
	}
}

const ColorRGBA& CheckWidget::GetWidgetColor(WidgetColorType type) const
{
	if (type == kWidgetColorButton)
	{
		return (buttonColor);
	}

	if (type == kWidgetColorHilite)
	{
		return (hiliteColor);
	}

	return (TextWidget::GetWidgetColor(type));
}

void CheckWidget::SetWidgetColor(const ColorRGBA& color, WidgetColorType type)
{
	if (type == kWidgetColorButton)
	{
		buttonColor = color;
		SetBuildFlag();
	}
	else if (type == kWidgetColorHilite)
	{
		hiliteColor = color;
		colorOverrideFlags |= kWidgetOverrideHiliteColor;
		SetBuildFlag();
	}

	TextWidget::SetWidgetColor(color, type);
}

void CheckWidget::SetDynamicWidgetColor(const ColorRGBA& color, WidgetColorType type)
{
	if (type == kWidgetColorButton)
	{
		if ((checkValue == 0) || (!(checkFlags & kCheckUseHighlightColor)))
		{
			float rgb = (GetWidgetState() & kWidgetHilited) ? 0.625F : 1.0F;
			float alpha = (GetGlobalWidgetState() & kWidgetDisabled) ? 0.5F : 1.0F;
			colorAttribute.SetDiffuseColor(ColorRGBA(color.GetColorRGB() * rgb, alpha));
		}
	}
	else if (type == kWidgetColorHilite)
	{
		if (checkValue != 0)
		{
			float rgb = (GetWidgetState() & kWidgetHilited) ? 0.625F : 1.0F;
			float alpha = (GetGlobalWidgetState() & kWidgetDisabled) ? 0.5F : 1.0F;
			colorAttribute.SetDiffuseColor(ColorRGBA(color.GetColorRGB() * rgb, alpha));
		}
	}
	else
	{
		TextWidget::SetDynamicWidgetColor(color, type);
	}
}

void CheckWidget::SetDefaultHiliteColor(void)
{
	hiliteColor = TheInterfaceMgr->GetInterfaceColor(kInterfaceColorButton);
}

WidgetPart CheckWidget::TestPosition(const Point3D& position) const
{
	if (checkFlags & kCheckTextInactive)
	{
		TextAlignment alignment = GetTextAlignment();
		if (alignment == kTextAlignLeft)
		{
			if (position.x > 18.0F)
			{
				return (kWidgetPartNone);
			}
		}
		else if (alignment == kTextAlignRight)
		{
			if (position.x < GetWidgetSize().x - 18.0F)
			{
				return (kWidgetPartNone);
			}
		}
	}

	return (kWidgetPartInterior);
}

void CheckWidget::Preprocess(void)
{
	if (!(colorOverrideFlags & kWidgetOverrideHiliteColor))
	{
		SetDefaultHiliteColor();
	}

	TextWidget::Preprocess();

	InitRenderable(&checkRenderable);
	checkRenderable.SetVertexCount(4);
	checkRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(CheckVertex));
	checkRenderable.SetVertexAttributeArray(kArrayPosition, 0, 2);
	checkRenderable.SetVertexAttributeArray(kArrayTexcoord, sizeof(Point2D), 2);
	vertexBuffer.Establish(sizeof(CheckVertex) * 4);

	attributeList.Append(&colorAttribute);
	checkRenderable.SetMaterialAttributeList(&attributeList);
	checkRenderable.SetMaterialObjectPointer(TheInterfaceMgr->GetMaterialObjectPointer());
}

void CheckWidget::Build(void)
{
	volatile CheckVertex *restrict vertex = vertexBuffer.BeginUpdate<CheckVertex>();

	float y = -1.0F;
	const Font *font = GetFont();
	if (font)
	{
		float baseline = font->GetFontHeader()->fontBaseline;
		y = Fmax(PositiveFloor(baseline * 0.5F) - 8.0F, baseline - 14.0F, -1.0F);
	}

	TextAlignment alignment = GetTextAlignment();
	if (alignment != kTextAlignRight)
	{
		SetTextRenderOffset(Vector3D((alignment == kTextAlignLeft) ? 18.0F : 0.0F, 0.0F, 0.0F));

		vertex[0].position.Set(-1.0F, y);
		vertex[1].position.Set(-1.0F, y + 16.0F);
		vertex[2].position.Set(15.0F, y);
		vertex[3].position.Set(15.0F, y + 16.0F);
	}
	else
	{
		SetTextRenderOffset(Vector3D(-18.0F, 0.0F, 0.0F));

		float x = GetWidgetSize().x + 1.0F;
		vertex[0].position.Set(x - 16.0F, y);
		vertex[1].position.Set(x - 16.0F, y + 16.0F);
		vertex[2].position.Set(x, y);
		vertex[3].position.Set(x, y + 16.0F);
	}

	if (checkValue == 0)
	{
		vertex[0].texcoord.Set(0.0F, 0.8125F);
		vertex[1].texcoord.Set(0.0F, 0.6875F);
		vertex[2].texcoord.Set(0.125F, 0.8125F);
		vertex[3].texcoord.Set(0.125F, 0.6875F);

		CheckWidget::SetDynamicWidgetColor(buttonColor, kWidgetColorButton);
	}
	else
	{
		if (checkValue != kWidgetValueIndeterminant)
		{
			vertex[0].texcoord.Set(0.1875F, 0.8125F);
			vertex[1].texcoord.Set(0.1875F, 0.6875F);
			vertex[2].texcoord.Set(0.3125F, 0.8125F);
			vertex[3].texcoord.Set(0.3125F, 0.6875F);
		}
		else
		{
			vertex[0].texcoord.Set(0.5625F, 0.8125F);
			vertex[1].texcoord.Set(0.5625F, 0.6875F);
			vertex[2].texcoord.Set(0.6875F, 0.8125F);
			vertex[3].texcoord.Set(0.6875F, 0.6875F);
		}

		CheckWidget::SetDynamicWidgetColor((checkFlags & kCheckUseHighlightColor) ? hiliteColor : buttonColor, kWidgetColorHilite);
	}

	TextWidget::SetWidgetAlpha(colorAttribute.GetDiffuseColor().alpha);

	vertexBuffer.EndUpdate();
	TextWidget::Build();
}

void CheckWidget::Render(List<Renderable> *renderList)
{
	renderList->Append(&checkRenderable);
	TextWidget::Render(renderList);
}

void CheckWidget::HandleMouseEvent(const PanelMouseEventData *eventData)
{
	EventType eventType = eventData->eventType;
	unsigned_int32 state = GetWidgetState();

	if (eventType == kEventMouseDown)
	{
		SetWidgetState(state | kWidgetHilited);
	}
	else if (eventType == kEventMouseMoved)
	{
		if ((WidgetContainsPoint(eventData->mousePosition)) && (CheckWidget::TestPosition(eventData->mousePosition) == kWidgetPartInterior))
		{
			state |= kWidgetHilited;
		}
		else
		{
			state &= ~kWidgetHilited;
		}

		SetWidgetState(state);
	}
	else if (eventType == kEventMouseUp)
	{
		SetWidgetState(state & ~kWidgetHilited);
		if ((WidgetContainsPoint(eventData->mousePosition)) && (CheckWidget::TestPosition(eventData->mousePosition) == kWidgetPartInterior))
		{
			checkValue = 1 - MaxZero(checkValue);
			SetBuildFlag();
			PostWidgetEvent(WidgetEventData(kEventWidgetChange));
		}
	}
}


RadioWidget::RadioWidget() :
		TextWidget(kWidgetRadio),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		colorAttribute(kAttributeMutable),
		radioRenderable(kRenderTriangleStrip)
{
	radioValue = 0;
	radioFlags = kRadioUseHighlightColor;
	colorOverrideFlags = 0;

	buttonColor.Set(1.0F, 1.0F, 1.0F);
	hiliteColor.Set(0.0F, 0.0F, 0.0F);
}

RadioWidget::RadioWidget(const Vector2D& size, const char *text, const char *font) :
		TextWidget(kWidgetRadio, size, text, font),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		colorAttribute(kAttributeMutable),
		radioRenderable(kRenderTriangleStrip)
{
	radioValue = 0;
	radioFlags = kRadioUseHighlightColor;
	colorOverrideFlags = 0;

	buttonColor.Set(1.0F, 1.0F, 1.0F);
	hiliteColor.Set(0.0F, 0.0F, 0.0F);
}

RadioWidget::RadioWidget(const RadioWidget& radioWidget) :
		TextWidget(radioWidget),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		colorAttribute(kAttributeMutable),
		radioRenderable(kRenderTriangleStrip)
{
	radioValue = radioWidget.radioValue;
	radioFlags = radioWidget.radioFlags;
	colorOverrideFlags = radioWidget.colorOverrideFlags;

	buttonColor = radioWidget.buttonColor;
	hiliteColor = radioWidget.hiliteColor;
}

RadioWidget::~RadioWidget()
{
}

Widget *RadioWidget::Replicate(void) const
{
	return (new RadioWidget(*this));
}

void RadioWidget::SetValue(int32 value, bool post)
{
	if (radioValue != value)
	{
		radioValue = value;
		SetBuildFlag();

		if (value == 1)
		{
			Widget *group = GetSuperNode();
			if (group)
			{
				Widget *widget = group->GetFirstSubnode();
				while (widget)
				{
					if ((widget != this) && (widget->GetWidgetType() == kWidgetRadio))
					{
						RadioWidget *radioWidget = static_cast<RadioWidget *>(widget);
						radioWidget->SetValue(0);
					}

					widget = widget->Next();
				}
			}
		}

		if (post)
		{
			PostWidgetEvent(WidgetEventData(kEventWidgetChange));
		}
	}
}

void RadioWidget::SetRadioFlags(unsigned_int32 flags)
{
	radioFlags = flags;
	SetBuildFlag();
}

void RadioWidget::Pack(Packer& data, unsigned_int32 packFlags) const
{
	TextWidget::Pack(data, packFlags);

	data << ChunkHeader('VALU', 4);
	data << radioValue;

	data << ChunkHeader('FLAG', 4);
	data << radioFlags;

	data << ChunkHeader('OVER', 4);
	data << colorOverrideFlags;

	data << ChunkHeader('BTTC', sizeof(ColorRGBA));
	data << buttonColor;

	if (colorOverrideFlags & kWidgetOverrideHiliteColor)
	{
		data << ChunkHeader('HILC', sizeof(ColorRGBA));
		data << hiliteColor;
	}

	data << TerminatorChunk;
}

void RadioWidget::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	TextWidget::Unpack(data, unpackFlags);
	UnpackChunkList<RadioWidget>(data, unpackFlags);
}

bool RadioWidget::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'VALU':

			data >> radioValue;
			return (true);

		case 'FLAG':

			data >> radioFlags;
			return (true);

		case 'OVER':

			data >> colorOverrideFlags;
			return (true);

		case 'BTTC':

			data >> buttonColor;
			return (true);

		case 'HILC':

			data >> hiliteColor;
			return (true);
	}

	return (false);
}

void *RadioWidget::BeginSettingsUnpack(void)
{
	colorOverrideFlags = 0;
	return (TextWidget::BeginSettingsUnpack());
}

int32 RadioWidget::GetSettingCount(void) const
{
	return (TextWidget::GetSettingCount() + 6);
}

Setting *RadioWidget::GetSetting(int32 index) const
{
	int32 count = TextWidget::GetSettingCount();
	if (index < count)
	{
		return (TextWidget::GetSetting(index));
	}

	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetRadio, 'SETT'));
		return (new HeadingSetting(kWidgetRadio, title));
	}

	if (index == count + 1)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetRadio, 'INIT'));
		return (new BooleanSetting('RDIN', (radioValue != 0), title));
	}

	if (index == count + 2)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetRadio, 'SLCT'));
		return (new BooleanSetting('RDSL', ((radioFlags & kRadioUseHighlightColor) != 0), title));
	}

	if (index == count + 3)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetRadio, 'TXIN'));
		return (new BooleanSetting('RDTI', ((radioFlags & kRadioTextInactive) != 0), title));
	}

	if (index == count + 4)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetRadio, 'BTTC'));
		const char *picker = table->GetString(StringID('WDGT', kWidgetRadio, 'BTTP'));
		return (new ColorSetting('RDBC', buttonColor, title, picker));
	}

	if (index == count + 5)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetRadio, 'HILC'));
		const char *picker = table->GetString(StringID('WDGT', kWidgetRadio, 'HILP'));
		return (new CheckColorSetting('RDHC', ((colorOverrideFlags & kWidgetOverrideHiliteColor) != 0), hiliteColor, title, picker));
	}

	return (nullptr);
}

void RadioWidget::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'RDIN')
	{
		radioValue = static_cast<const BooleanSetting *>(setting)->GetBooleanValue();
	}
	else if (identifier == 'RDSL')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			radioFlags |= kRadioUseHighlightColor;
		}
		else
		{
			radioFlags &= ~kRadioUseHighlightColor;
		}
	}
	else if (identifier == 'RDTI')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			radioFlags |= kRadioTextInactive;
		}
		else
		{
			radioFlags &= ~kRadioTextInactive;
		}
	}
	else if (identifier == 'RDBC')
	{
		buttonColor = static_cast<const ColorSetting *>(setting)->GetColor();
	}
	else if (identifier == 'RDHC')
	{
		const CheckColorSetting *checkColorSetting = static_cast<const CheckColorSetting *>(setting);
		if (checkColorSetting->GetCheckValue() != 0)
		{
			colorOverrideFlags |= kWidgetOverrideHiliteColor;
			hiliteColor = checkColorSetting->GetColor();
		}
		else
		{
			colorOverrideFlags &= ~kWidgetOverrideHiliteColor;
			SetDefaultHiliteColor();
		}
	}
	else
	{
		TextWidget::SetSetting(setting);
	}
}

const ColorRGBA& RadioWidget::GetWidgetColor(WidgetColorType type) const
{
	if (type == kWidgetColorButton)
	{
		return (buttonColor);
	}

	if (type == kWidgetColorHilite)
	{
		return (hiliteColor);
	}

	return (TextWidget::GetWidgetColor(type));
}

void RadioWidget::SetWidgetColor(const ColorRGBA& color, WidgetColorType type)
{
	if (type == kWidgetColorButton)
	{
		buttonColor = color;
	}
	else if (type == kWidgetColorHilite)
	{
		hiliteColor = color;
		colorOverrideFlags |= kWidgetOverrideHiliteColor;
	}

	TextWidget::SetWidgetColor(color, type);
}

void RadioWidget::SetDynamicWidgetColor(const ColorRGBA& color, WidgetColorType type)
{
	if (type == kWidgetColorButton)
	{
		if ((radioValue == 0) || (!(radioFlags & kRadioUseHighlightColor)))
		{
			float rgb = (GetWidgetState() & kWidgetHilited) ? 0.625F : 1.0F;
			float alpha = (GetGlobalWidgetState() & kWidgetDisabled) ? 0.5F : 1.0F;
			colorAttribute.SetDiffuseColor(ColorRGBA(color.GetColorRGB() * rgb, alpha));
		}
	}
	else if (type == kWidgetColorHilite)
	{
		if (radioValue != 0)
		{
			float rgb = (GetWidgetState() & kWidgetHilited) ? 0.625F : 1.0F;
			float alpha = (GetGlobalWidgetState() & kWidgetDisabled) ? 0.5F : 1.0F;
			colorAttribute.SetDiffuseColor(ColorRGBA(color.GetColorRGB() * rgb, alpha));
		}
	}
	else
	{
		TextWidget::SetDynamicWidgetColor(color, type);
	}
}

void RadioWidget::SetDefaultHiliteColor(void)
{
	hiliteColor = TheInterfaceMgr->GetInterfaceColor(kInterfaceColorButton);
}

WidgetPart RadioWidget::TestPosition(const Point3D& position) const
{
	if (radioFlags & kRadioTextInactive)
	{
		TextAlignment alignment = GetTextAlignment();
		if (alignment == kTextAlignLeft)
		{
			if (position.x > 18.0F)
			{
				return (kWidgetPartNone);
			}
		}
		else if (alignment == kTextAlignRight)
		{
			if (position.x < GetWidgetSize().x - 18.0F)
			{
				return (kWidgetPartNone);
			}
		}
	}

	return (kWidgetPartInterior);
}

void RadioWidget::Preprocess(void)
{
	if (!(colorOverrideFlags & kWidgetOverrideHiliteColor))
	{
		SetDefaultHiliteColor();
	}

	TextWidget::Preprocess();

	InitRenderable(&radioRenderable);
	radioRenderable.SetVertexCount(4);
	radioRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(RadioVertex));
	radioRenderable.SetVertexAttributeArray(kArrayPosition, 0, 2);
	radioRenderable.SetVertexAttributeArray(kArrayTexcoord, sizeof(Point2D), 2);
	vertexBuffer.Establish(sizeof(RadioVertex) * 4);

	attributeList.Append(&colorAttribute);
	radioRenderable.SetMaterialAttributeList(&attributeList);
	radioRenderable.SetMaterialObjectPointer(TheInterfaceMgr->GetMaterialObjectPointer());
}

void RadioWidget::Build(void)
{
	volatile RadioVertex *restrict vertex = vertexBuffer.BeginUpdate<RadioVertex>();

	float y = -1.0F;
	const Font *font = GetFont();
	if (font)
	{
		float baseline = font->GetFontHeader()->fontBaseline;
		y = Fmax(PositiveFloor(baseline * 0.5F) - 8.0F, baseline - 14.0F, -1.0F);
	}

	TextAlignment alignment = GetTextAlignment();
	if (alignment != kTextAlignRight)
	{
		SetTextRenderOffset(Vector3D((alignment == kTextAlignLeft) ? 18.0F : 0.0F, 0.0F, 0.0F));

		vertex[0].position.Set(-1.0F, y);
		vertex[1].position.Set(-1.0F, y + 16.0F);
		vertex[2].position.Set(15.0F, y);
		vertex[3].position.Set(15.0F, y + 16.0F);
	}
	else
	{
		SetTextRenderOffset(Vector3D(-18.0F, 0.0F, 0.0F));

		float x = GetWidgetSize().x + 1.0F;
		vertex[0].position.Set(x - 16.0F, y);
		vertex[1].position.Set(x - 16.0F, y + 16.0F);
		vertex[2].position.Set(x, y);
		vertex[3].position.Set(x, y + 16.0F);
	}

	if (radioValue == 0)
	{
		vertex[0].texcoord.Set(0.0F, 1.0F);
		vertex[1].texcoord.Set(0.0F, 0.875F);
		vertex[2].texcoord.Set(0.125F, 1.0F);
		vertex[3].texcoord.Set(0.125F, 0.875F);

		RadioWidget::SetDynamicWidgetColor(buttonColor, kWidgetColorButton);
	}
	else
	{
		vertex[0].texcoord.Set(0.75F, 1.0F);
		vertex[1].texcoord.Set(0.75F, 0.875F);
		vertex[2].texcoord.Set(0.875F, 1.0F);
		vertex[3].texcoord.Set(0.875F, 0.875F);

		RadioWidget::SetDynamicWidgetColor((radioFlags & kRadioUseHighlightColor) ? hiliteColor : buttonColor, kWidgetColorHilite);
	}

	TextWidget::SetWidgetAlpha(colorAttribute.GetDiffuseColor().alpha);

	vertexBuffer.EndUpdate();
	TextWidget::Build();
}

void RadioWidget::Render(List<Renderable> *renderList)
{
	renderList->Append(&radioRenderable);
	TextWidget::Render(renderList);
}

void RadioWidget::HandleMouseEvent(const PanelMouseEventData *eventData)
{
	EventType eventType = eventData->eventType;
	unsigned_int32 state = GetWidgetState();

	if (eventType == kEventMouseDown)
	{
		SetWidgetState(state | kWidgetHilited);
	}
	else if (eventType == kEventMouseMoved)
	{
		if ((WidgetContainsPoint(eventData->mousePosition)) && (RadioWidget::TestPosition(eventData->mousePosition) == kWidgetPartInterior))
		{
			state |= kWidgetHilited;
		}
		else
		{
			state &= ~kWidgetHilited;
		}

		SetWidgetState(state);
	}
	else if (eventType == kEventMouseUp)
	{
		SetWidgetState(state & ~kWidgetHilited);

		if ((WidgetContainsPoint(eventData->mousePosition)) && (RadioWidget::TestPosition(eventData->mousePosition) == kWidgetPartInterior))
		{
			SetValue(1, true);
		}
	}
}


GuiButtonWidget::GuiButtonWidget(WidgetType type) :
		RenderableWidget(type, kRenderTriangleStrip),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		colorAttribute(kAttributeMutable)
{
	SetDefaultColorType(kWidgetColorButton);

	texcoordOffset.Set(0.0F, 0.0F);
}

GuiButtonWidget::GuiButtonWidget(const Vector2D& size, const Point2D& minTex, const Point2D& maxTex) : GuiButtonWidget(kWidgetGuiButton, size, minTex, maxTex)
{
}

GuiButtonWidget::GuiButtonWidget(WidgetType type, const Vector2D& size, const Point2D& minTex, const Point2D& maxTex) :
		RenderableWidget(type, kRenderTriangleStrip, size),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		colorAttribute(kAttributeMutable)
{
	SetDefaultColorType(kWidgetColorButton);

	texcoordRange.Set(minTex, maxTex);
	texcoordOffset.Set(0.0F, 0.0F);
}

GuiButtonWidget::GuiButtonWidget(const GuiButtonWidget& guiButtonWidget) :
		RenderableWidget(guiButtonWidget),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		colorAttribute(kAttributeMutable)
{
	texcoordRange = guiButtonWidget.texcoordRange;
	texcoordOffset.Set(0.0F, 0.0F);
}

GuiButtonWidget::~GuiButtonWidget()
{
}

Widget *GuiButtonWidget::Replicate(void) const
{
	return (new GuiButtonWidget(*this));
}

void GuiButtonWidget::Preprocess(void)
{
	RenderableWidget::Preprocess();

	SetVertexCount(4);
	SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(ButtonVertex));
	SetVertexAttributeArray(kArrayPosition, 0, 2);
	SetVertexAttributeArray(kArrayTexcoord, sizeof(Point2D), 2);
	vertexBuffer.Establish(sizeof(ButtonVertex) * 4);

	attributeList.Append(&colorAttribute);
	SetMaterialAttributeList(&attributeList);
	SetMaterialObjectPointer(TheInterfaceMgr->GetMaterialObjectPointer());
}

void GuiButtonWidget::Build(void)
{
	volatile ButtonVertex *restrict vertex = vertexBuffer.BeginUpdate<ButtonVertex>();

	const Vector2D& size = GetWidgetSize();
	float dx = texcoordOffset.x + (GetWidgetState() & kWidgetCollapsed) ? -0.1875F : 0.0F;
	float dy = texcoordOffset.y;

	vertex[0].position.Set(0.0F, 0.0F);
	vertex[0].texcoord.Set(texcoordRange.min.x + dx, texcoordRange.max.y + dy);

	vertex[1].position.Set(0.0F, size.y);
	vertex[1].texcoord.Set(texcoordRange.min.x + dx, texcoordRange.min.y + dy);

	vertex[2].position.Set(size.x, 0.0F);
	vertex[2].texcoord.Set(texcoordRange.max.x + dx, texcoordRange.max.y + dy);

	vertex[3].position.Set(size.x, size.y);
	vertex[3].texcoord.Set(texcoordRange.max.x + dx, texcoordRange.min.y + dy);

	vertexBuffer.EndUpdate();

	float alpha = (GetGlobalWidgetState() & kWidgetDisabled) ? 0.5F : 1.0F;
	if (GetWidgetState() & kWidgetHilited)
	{
		colorAttribute.SetDiffuseColor(ColorRGBA(RenderableWidget::GetWidgetColor().GetColorRGB() * 0.625F, alpha));
	}
	else
	{
		colorAttribute.SetDiffuseColor(ColorRGBA(RenderableWidget::GetWidgetColor().GetColorRGB(), alpha));
	}
}

void GuiButtonWidget::HandleMouseEvent(const PanelMouseEventData *eventData)
{
	EventType eventType = eventData->eventType;
	unsigned_int32 state = GetWidgetState();

	if (eventType == kEventMouseDown)
	{
		SetWidgetState(state | kWidgetHilited);
	}
	else if (eventType == kEventMouseMoved)
	{
		if (WidgetContainsPoint(eventData->mousePosition))
		{
			state |= kWidgetHilited;
		}
		else
		{
			state &= ~kWidgetHilited;
		}

		SetWidgetState(state);
	}
	else if (eventType == kEventMouseUp)
	{
		SetWidgetState(state & ~kWidgetHilited);

		if (WidgetContainsPoint(eventData->mousePosition))
		{
			Activate(eventData->initiatorNode);
		}
	}
}


PushButtonWidget::PushButtonWidget() :
		TextWidget(kWidgetPushButton),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		colorAttribute(kAttributeMutable),
		buttonRenderable(kRenderQuads)
{
	pushButtonFlags = 0;
	activationKey = kActivationKeyNone;
	buttonColor.Set(1.0F, 1.0F, 1.0F);

	SetTextAlignment(kTextAlignCenter);
}

PushButtonWidget::PushButtonWidget(const Vector2D& size, const char *text, const char *font) :
		TextWidget(kWidgetPushButton, size, text, font),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		colorAttribute(kAttributeMutable),
		buttonRenderable(kRenderQuads)
{
	pushButtonFlags = 0;
	activationKey = kActivationKeyNone;
	buttonColor.Set(1.0F, 1.0F, 1.0F);

	SetTextAlignment(kTextAlignCenter);
}

PushButtonWidget::PushButtonWidget(const PushButtonWidget& pushButtonWidget) :
		TextWidget(pushButtonWidget),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		colorAttribute(kAttributeMutable),
		buttonRenderable(kRenderQuads)
{
	pushButtonFlags = pushButtonWidget.pushButtonFlags;
	activationKey = pushButtonWidget.activationKey;
	buttonColor = pushButtonWidget.buttonColor;
}

PushButtonWidget::~PushButtonWidget()
{
}

Widget *PushButtonWidget::Replicate(void) const
{
	return (new PushButtonWidget(*this));
}

void PushButtonWidget::SetPushButtonFlags(unsigned_int32 flags)
{
	pushButtonFlags = flags;
	SetBuildFlag();
}

void PushButtonWidget::Pack(Packer& data, unsigned_int32 packFlags) const
{
	TextWidget::Pack(data, packFlags);

	data << ChunkHeader('FLAG', 4);
	data << pushButtonFlags;

	data << ChunkHeader('AKEY', 4);
	data << activationKey;

	data << ChunkHeader('BTTC', sizeof(ColorRGBA));
	data << buttonColor;

	data << TerminatorChunk;
}

void PushButtonWidget::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	TextWidget::Unpack(data, unpackFlags);
	UnpackChunkList<PushButtonWidget>(data, unpackFlags);
}

bool PushButtonWidget::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> pushButtonFlags;
			return (true);

		case 'AKEY':

			data >> activationKey;
			return (true);

		case 'BTTC':

			data >> buttonColor;
			return (true);
	}

	return (false);
}

int32 PushButtonWidget::GetSettingCount(void) const
{
	return (TextWidget::GetSettingCount() + 4);
}

Setting *PushButtonWidget::GetSetting(int32 index) const
{
	int32 count = TextWidget::GetSettingCount();
	if (index < count)
	{
		return (TextWidget::GetSetting(index));
	}

	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetPushButton, 'SETT'));
		return (new HeadingSetting(kWidgetPushButton, title));
	}

	if (index == count + 1)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetPushButton, 'PRIM'));
		return (new BooleanSetting('PRIM', ((pushButtonFlags & kPushButtonPrimary) != 0), title));
	}

	if (index == count + 2)
	{
		int32 selection = 0;
		if (activationKey == kActivationKeyEnter)
		{
			selection = 1;
		}
		else if (activationKey == kActivationKeyEscape)
		{
			selection = 2;
		}

		const char *title = table->GetString(StringID('WDGT', kWidgetPushButton, 'AKEY'));
		MenuSetting *menu = new MenuSetting('AKEY', selection, title, 3);
		menu->SetMenuItemString(0, table->GetString(StringID('WDGT', kWidgetPushButton, 'AKEY', 'NONE')));
		menu->SetMenuItemString(1, table->GetString(StringID('WDGT', kWidgetPushButton, 'AKEY', kActivationKeyEnter)));
		menu->SetMenuItemString(2, table->GetString(StringID('WDGT', kWidgetPushButton, 'AKEY', kActivationKeyEscape)));

		return (menu);
	}

	if (index == count + 3)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetPushButton, 'BTTC'));
		const char *picker = table->GetString(StringID('WDGT', kWidgetPushButton, 'BTTP'));
		return (new ColorSetting('PBBC', buttonColor, title, picker));
	}

	return (nullptr);
}

void PushButtonWidget::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'PRIM')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			pushButtonFlags |= kPushButtonPrimary;
		}
		else
		{
			pushButtonFlags &= ~kPushButtonPrimary;
		}
	}
	else if (identifier == 'AKEY')
	{
		int32 selection = static_cast<const MenuSetting *>(setting)->GetMenuSelection();
		if (selection == 0)
		{
			activationKey = kActivationKeyNone;
		}
		else if (selection == 1)
		{
			activationKey = kActivationKeyEnter;
		}
		else if (selection == 2)
		{
			activationKey = kActivationKeyEscape;
		}
	}
	else if (identifier == 'PBBC')
	{
		if (pushButtonFlags & kPushButtonPrimary)
		{
			SetPrimaryButtonColor();
		}
		else
		{
			buttonColor = static_cast<const ColorSetting *>(setting)->GetColor();
		}
	}
	else
	{
		TextWidget::SetSetting(setting);
	}
}

const ColorRGBA& PushButtonWidget::GetWidgetColor(WidgetColorType type) const
{
	if (type == kWidgetColorButton)
	{
		return (buttonColor);
	}

	return (TextWidget::GetWidgetColor(type));
}

void PushButtonWidget::SetWidgetColor(const ColorRGBA& color, WidgetColorType type)
{
	if (type == kWidgetColorButton)
	{
		buttonColor = color;
	}

	TextWidget::SetWidgetColor(color, type);
}

void PushButtonWidget::SetDynamicWidgetColor(const ColorRGBA& color, WidgetColorType type)
{
	if (type == kWidgetColorButton)
	{
		float rgb = (GetWidgetState() & kWidgetHilited) ? 0.625F : 1.0F;
		float alpha = (GetGlobalWidgetState() & kWidgetDisabled) ? 0.5F : 1.0F;
		colorAttribute.SetDiffuseColor(ColorRGBA(color.GetColorRGB() * rgb, alpha));
	}
	else
	{
		TextWidget::SetDynamicWidgetColor(color, type);
	}
}

void PushButtonWidget::SetPrimaryButtonColor(void)
{
	buttonColor = TheInterfaceMgr->GetInterfaceColor(kInterfaceColorButton);
}

void PushButtonWidget::Preprocess(void)
{
	if (pushButtonFlags & kPushButtonPrimary)
	{
		SetPrimaryButtonColor();
	}

	TextWidget::Preprocess();

	InitRenderable(&buttonRenderable);
	buttonRenderable.SetVertexCount(12);
	buttonRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(ButtonVertex));
	buttonRenderable.SetVertexAttributeArray(kArrayPosition, 0, 2);
	buttonRenderable.SetVertexAttributeArray(kArrayTexcoord, sizeof(Point2D), 2);
	vertexBuffer.Establish(sizeof(ButtonVertex) * 12);

	attributeList.Append(&colorAttribute);
	buttonRenderable.SetMaterialAttributeList(&attributeList);
	buttonRenderable.SetMaterialObjectPointer(TheInterfaceMgr->GetMaterialObjectPointer());

	Type key = activationKey;
	if (key != kActivationKeyNone)
	{
		Window *window = GetOwningWindow();
		if (window)
		{
			window->SetActivationKeyWidget(key, this);
		}
	}
}

void PushButtonWidget::Build(void)
{
	volatile ButtonVertex *restrict vertex = vertexBuffer.BeginUpdate<ButtonVertex>();

	float width = GetWidgetSize().x;
	float height = GetWidgetSize().y;

	const Font *font = GetFont();
	if (font)
	{
		float h = font->GetFontHeader()->fontHeight * GetTextScale();
		SetTextRenderOffset(Vector3D(0.0F, FmaxZero(Floor((height - h) * 0.5F - 1.0F)), 0.0F));
	}

	float y1 = -1.0F;
	float y2 = height + 1.0F;
	float x1 = -1.0F;
	float x2 = Fmin(height * 0.5F, width * 0.5F);
	float x3 = width - x2;
	float x4 = width + 1.0F;
	float ds = Fmin((x3 - x2) / (y2 - y1) * 0.125F, 0.375F);

	vertex[0].position.Set(x1, y1);
	vertex[0].texcoord.Set(0.0F, 0.4375F);
	vertex[1].position.Set(x1, y2);
	vertex[1].texcoord.Set(0.0F, 0.3125F);
	vertex[2].position.Set(x2, y2);
	vertex[2].texcoord.Set(0.0625F, 0.3125F);
	vertex[3].position.Set(x2, y1);
	vertex[3].texcoord.Set(0.0625F, 0.4375F);

	vertex[4].position.Set(x2, y1);
	vertex[4].texcoord.Set(0.0625F, 0.4375F);
	vertex[5].position.Set(x2, y2);
	vertex[5].texcoord.Set(0.0625F, 0.3125F);
	vertex[6].position.Set(x3, y2);
	vertex[6].texcoord.Set(0.0625F + ds, 0.3125F);
	vertex[7].position.Set(x3, y1);
	vertex[7].texcoord.Set(0.0625F + ds, 0.4375F);

	vertex[8].position.Set(x3, y1);
	vertex[8].texcoord.Set(0.4375F, 0.4375F);
	vertex[9].position.Set(x3, y2);
	vertex[9].texcoord.Set(0.4375F, 0.3125F);
	vertex[10].position.Set(x4, y2);
	vertex[10].texcoord.Set(0.5F, 0.3125F);
	vertex[11].position.Set(x4, y1);
	vertex[11].texcoord.Set(0.5F, 0.4375F);

	vertexBuffer.EndUpdate();

	PushButtonWidget::SetDynamicWidgetColor(buttonColor, kWidgetColorButton);
	TextWidget::SetWidgetAlpha(colorAttribute.GetDiffuseColor().alpha);

	TextWidget::Build();
}

void PushButtonWidget::Render(List<Renderable> *renderList)
{
	renderList->Append(&buttonRenderable);
	TextWidget::Render(renderList);
}

void PushButtonWidget::HandleMouseEvent(const PanelMouseEventData *eventData)
{
	EventType eventType = eventData->eventType;
	unsigned_int32 state = GetWidgetState();

	if (eventType == kEventMouseDown)
	{
		SetWidgetState(state | kWidgetHilited);
	}
	else if (eventType == kEventMouseMoved)
	{
		if (WidgetContainsPoint(eventData->mousePosition))
		{
			state |= kWidgetHilited;
		}
		else
		{
			state &= ~kWidgetHilited;
		}

		SetWidgetState(state);
	}
	else if (eventType == kEventMouseUp)
	{
		SetWidgetState(state & ~kWidgetHilited);

		if (WidgetContainsPoint(eventData->mousePosition))
		{
			Activate(eventData->initiatorNode);
		}
	}
}


IconButtonWidget::IconButtonWidget() :
		ImageWidget(kWidgetIconButton),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		colorAttribute(kAttributeMutable),
		buttonRenderable(kRenderTriangleStrip)
{
	iconButtonValue = 0;
	iconButtonFlags = 0;
	buttonColor.Set(1.0F, 1.0F, 1.0F);
}

IconButtonWidget::IconButtonWidget(const Vector2D& size, const char *name) :
		ImageWidget(kWidgetIconButton, size, name),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		colorAttribute(kAttributeMutable),
		buttonRenderable(kRenderTriangleStrip)
{
	iconButtonValue = 0;
	iconButtonFlags = 0;
	buttonColor.Set(1.0F, 1.0F, 1.0F);
}

IconButtonWidget::IconButtonWidget(const IconButtonWidget& iconButtonWidget) :
		ImageWidget(iconButtonWidget),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		colorAttribute(kAttributeMutable),
		buttonRenderable(kRenderTriangleStrip)
{
	iconButtonValue = iconButtonWidget.iconButtonValue;
	iconButtonFlags = iconButtonWidget.iconButtonFlags;
	buttonColor = iconButtonWidget.buttonColor;
}

IconButtonWidget::~IconButtonWidget()
{
}

Widget *IconButtonWidget::Replicate(void) const
{
	return (new IconButtonWidget(*this));
}

void IconButtonWidget::SetValue(int32 value, bool post)
{
	if (iconButtonValue != value)
	{
		iconButtonValue = value;
		SetBuildFlag();

		if (post)
		{
			PostWidgetEvent(WidgetEventData(kEventWidgetChange));
		}
	}
}

void IconButtonWidget::Pack(Packer& data, unsigned_int32 packFlags) const
{
	ImageWidget::Pack(data, packFlags);

	data << ChunkHeader('VALU', 4);
	data << iconButtonValue;

	data << ChunkHeader('FLAG', 4);
	data << iconButtonFlags;

	data << ChunkHeader('BTTC', sizeof(ColorRGBA));
	data << buttonColor;

	data << TerminatorChunk;
}

void IconButtonWidget::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	ImageWidget::Unpack(data, unpackFlags);
	UnpackChunkList<IconButtonWidget>(data, unpackFlags);
}

bool IconButtonWidget::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'VALU':

			data >> iconButtonValue;
			return (true);

		case 'FLAG':

			data >> iconButtonFlags;
			return (true);

		case 'BTTC':

			data >> buttonColor;
			return (true);
	}

	return (false);
}

int32 IconButtonWidget::GetSettingCount(void) const
{
	return (ImageWidget::GetSettingCount() + 4);
}

Setting *IconButtonWidget::GetSetting(int32 index) const
{
	int32 count = ImageWidget::GetSettingCount();
	if (index < count)
	{
		return (ImageWidget::GetSetting(index));
	}

	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetIconButton, 'SETT'));
		return (new HeadingSetting(kWidgetIconButton, title));
	}

	if (index == count + 1)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetIconButton, 'STCK'));
		return (new BooleanSetting('STCK', ((iconButtonFlags & kIconButtonSticky) != 0), title));
	}

	if (index == count + 2)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetIconButton, 'INIT'));
		return (new BooleanSetting('IBIN', (iconButtonValue != 0), title));
	}

	if (index == count + 3)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetIconButton, 'BTTC'));
		const char *picker = table->GetString(StringID('WDGT', kWidgetIconButton, 'BTTP'));
		return (new ColorSetting('IBBC', buttonColor, title, picker));
	}

	return (nullptr);
}

void IconButtonWidget::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'STCK')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			iconButtonFlags |= kIconButtonSticky;
		}
		else
		{
			iconButtonFlags &= ~kIconButtonSticky;
		}
	}
	else if (identifier == 'IBIN')
	{
		iconButtonValue = static_cast<const BooleanSetting *>(setting)->GetBooleanValue();
	}
	else if (identifier == 'IBBC')
	{
		buttonColor = static_cast<const ColorSetting *>(setting)->GetColor();
	}
	else
	{
		ImageWidget::SetSetting(setting);
	}
}

const ColorRGBA& IconButtonWidget::GetWidgetColor(WidgetColorType type) const
{
	if (type == kWidgetColorButton)
	{
		return (buttonColor);
	}

	return (ImageWidget::GetWidgetColor(type));
}

void IconButtonWidget::SetWidgetColor(const ColorRGBA& color, WidgetColorType type)
{
	if (type == kWidgetColorButton)
	{
		buttonColor = color;
	}

	ImageWidget::SetWidgetColor(color, type);
}

void IconButtonWidget::SetDynamicWidgetColor(const ColorRGBA& color, WidgetColorType type)
{
	if (type == kWidgetColorButton)
	{
		float rgb = ((iconButtonValue != 0) || (GetWidgetState() & kWidgetHilited)) ? 0.625F : 1.0F;
		float alpha = (GetGlobalWidgetState() & kWidgetDisabled) ? 0.5F : 1.0F;
		colorAttribute.SetDiffuseColor(ColorRGBA(color.GetColorRGB() * rgb, alpha));
	}
	else
	{
		ImageWidget::SetDynamicWidgetColor(color, type);
	}
}

void IconButtonWidget::Preprocess(void)
{
	ImageWidget::Preprocess();

	InitRenderable(&buttonRenderable);
	buttonRenderable.SetVertexCount(4);
	buttonRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(ButtonVertex));
	buttonRenderable.SetVertexAttributeArray(kArrayPosition, 0, 2);
	buttonRenderable.SetVertexAttributeArray(kArrayTexcoord, sizeof(Point2D), 2);
	vertexBuffer.Establish(sizeof(ButtonVertex) * 4);

	attributeList.Append(&colorAttribute);
	buttonRenderable.SetMaterialAttributeList(&attributeList);
	buttonRenderable.SetMaterialObjectPointer(TheInterfaceMgr->GetMaterialObjectPointer());
}

void IconButtonWidget::Build(void)
{
	volatile ButtonVertex *restrict vertex = vertexBuffer.BeginUpdate<ButtonVertex>();

	float width = GetWidgetSize().x;
	float height = GetWidgetSize().y;

	float m = Fmax(Fmin(width, height), 32.0F);
	float dx = m * 0.15625F;
	float dy1 = m * 0.09375F;
	float dy2 = m * 0.21875F;

	vertex[0].position.Set(-dx, -dy1);
	vertex[0].texcoord.Set(0.0F, 0.8125F);

	vertex[1].position.Set(-dx, height + dy2);
	vertex[1].texcoord.Set(0.0F, 0.6875F);

	vertex[2].position.Set(width + dx, -dy1);
	vertex[2].texcoord.Set(0.125F, 0.8125F);

	vertex[3].position.Set(width + dx, height + dy2);
	vertex[3].texcoord.Set(0.125F, 0.6875F);

	vertexBuffer.EndUpdate();

	IconButtonWidget::SetDynamicWidgetColor(buttonColor, kWidgetColorButton);
	ImageWidget::SetWidgetAlpha(colorAttribute.GetDiffuseColor().alpha);

	ImageWidget::Build();
}

void IconButtonWidget::Render(List<Renderable> *renderList)
{
	renderList->Append(&buttonRenderable);
	ImageWidget::Render(renderList);
}

void IconButtonWidget::HandleMouseEvent(const PanelMouseEventData *eventData)
{
	EventType eventType = eventData->eventType;
	unsigned_int32 state = GetWidgetState();

	if (eventType == kEventMouseDown)
	{
		SetWidgetState(state | kWidgetHilited);
	}
	else if (eventType == kEventMouseMoved)
	{
		if (WidgetContainsPoint(eventData->mousePosition))
		{
			state |= kWidgetHilited;
		}
		else
		{
			state &= ~kWidgetHilited;
		}

		SetWidgetState(state);
	}
	else if (eventType == kEventMouseUp)
	{
		SetWidgetState(state & ~kWidgetHilited);
		if (WidgetContainsPoint(eventData->mousePosition))
		{
			if (iconButtonFlags & kIconButtonSticky)
			{
				if (iconButtonValue != 1)
				{
					iconButtonValue = 1;
					SetBuildFlag();
				}

				PostWidgetEvent(WidgetEventData(kEventWidgetChange));
			}
			else
			{
				Activate(eventData->initiatorNode);
			}
		}
	}
}


TextButtonWidget::TextButtonWidget() : TextWidget(kWidgetTextButton)
{
}

TextButtonWidget::TextButtonWidget(const Vector2D& size, const char *text, const char *font) : TextButtonWidget(kWidgetTextButton, size, text, font)
{
}

TextButtonWidget::TextButtonWidget(WidgetType type, const char *text, const char *font) : TextButtonWidget(type, Zero2D, text, font)
{
}

TextButtonWidget::TextButtonWidget(WidgetType type, const Vector2D& size, const char *text, const char *font) : TextWidget(type, size, text, font)
{
	hiliteColor.Set(1.0F, 1.0F, 1.0F, 1.0F);
}

TextButtonWidget::TextButtonWidget(const TextButtonWidget& textButtonWidget) : TextWidget(textButtonWidget)
{
	hiliteColor = textButtonWidget.hiliteColor;
}

TextButtonWidget::~TextButtonWidget()
{
}

Widget *TextButtonWidget::Replicate(void) const
{
	return (new TextButtonWidget(*this));
}

void TextButtonWidget::Pack(Packer& data, unsigned_int32 packFlags) const
{
	TextWidget::Pack(data, packFlags);

	data << ChunkHeader('HILC', sizeof(ColorRGBA));
	data << hiliteColor;

	data << TerminatorChunk;
}

void TextButtonWidget::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	TextWidget::Unpack(data, unpackFlags);
	UnpackChunkList<TextButtonWidget>(data, unpackFlags);
}

bool TextButtonWidget::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'HILC':

			data >> hiliteColor;
			return (true);
	}

	return (false);
}

int32 TextButtonWidget::GetSettingCount(void) const
{
	return (TextWidget::GetSettingCount() + 2);
}

Setting *TextButtonWidget::GetSetting(int32 index) const
{
	int32 count = TextWidget::GetSettingCount();
	if (index < count)
	{
		return (TextWidget::GetSetting(index));
	}

	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetTextButton, 'SETT'));
		return (new HeadingSetting(kWidgetTextButton, title));
	}

	if (index == count + 1)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetTextButton, 'HILC'));
		const char *picker = table->GetString(StringID('WDGT', kWidgetTextButton, 'HILP'));
		return (new ColorSetting('TBHC', hiliteColor, title, picker));
	}

	return (nullptr);
}

void TextButtonWidget::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'TBHC')
	{
		hiliteColor = static_cast<const ColorSetting *>(setting)->GetColor();
	}
	else
	{
		TextWidget::SetSetting(setting);
	}
}

const ColorRGBA& TextButtonWidget::GetWidgetColor(WidgetColorType type) const
{
	if (type == kWidgetColorHilite)
	{
		return (hiliteColor);
	}

	return (TextWidget::GetWidgetColor(type));
}

void TextButtonWidget::SetWidgetColor(const ColorRGBA& color, WidgetColorType type)
{
	if (type == kWidgetColorHilite)
	{
		hiliteColor = color;
	}

	TextWidget::SetWidgetColor(color, type);
}

void TextButtonWidget::SetWidgetAlpha(float alpha, WidgetColorType type)
{
	if (type == kWidgetColorHilite)
	{
		hiliteColor.alpha = alpha;
	}

	TextWidget::SetWidgetAlpha(alpha, type);
}

void TextButtonWidget::HandleMouseEvent(const PanelMouseEventData *eventData)
{
	EventType eventType = eventData->eventType;

	if (eventType == kEventMouseDown)
	{
		TextWidget::SetDynamicWidgetColor(hiliteColor);
	}
	else if (eventType == kEventMouseMoved)
	{
		const ColorRGBA& color = (WidgetContainsPoint(eventData->mousePosition)) ? hiliteColor : RenderableWidget::GetWidgetColor();
		if (GetInitialTextColor() != Color4C(color))
		{
			TextWidget::SetDynamicWidgetColor(color);
		}
	}
	else if (eventType == kEventMouseUp)
	{
		TextWidget::SetDynamicWidgetColor(RenderableWidget::GetWidgetColor());

		if (WidgetContainsPoint(eventData->mousePosition))
		{
			Activate(eventData->initiatorNode);
		}
	}
}


HyperlinkWidget::HyperlinkWidget(const char *text, const char *font, const char *hyperlink) :
		TextButtonWidget(kWidgetHyperlink, text, font),
		hyperlinkObserver(this, &HyperlinkWidget::HandleHyperlinkEvent)
{
	if (hyperlink)
	{
		hyperlinkAddress = hyperlink;
	}
}

HyperlinkWidget::HyperlinkWidget(const Vector2D& size, const char *text, const char *font, const char *hyperlink) :
		TextButtonWidget(kWidgetHyperlink, size, text, font),
		hyperlinkObserver(this, &HyperlinkWidget::HandleHyperlinkEvent)
{
	if (hyperlink)
	{
		hyperlinkAddress = hyperlink;
	}
}

HyperlinkWidget::HyperlinkWidget(const HyperlinkWidget& hyperlinkWidget) :
		TextButtonWidget(hyperlinkWidget),
		hyperlinkObserver(this, &HyperlinkWidget::HandleHyperlinkEvent)
{
	hyperlinkAddress = hyperlinkWidget.hyperlinkAddress;
}

HyperlinkWidget::~HyperlinkWidget()
{
}

Widget *HyperlinkWidget::Replicate(void) const
{
	return (new HyperlinkWidget(*this));
}

void HyperlinkWidget::Pack(Packer& data, unsigned_int32 packFlags) const
{
	TextButtonWidget::Pack(data, packFlags);

	if (hyperlinkAddress[0] != 0)
	{
		PackHandle handle = data.BeginChunk('LINK');
		data << hyperlinkAddress;
		data.EndChunk(handle);
	}

	data << TerminatorChunk;
}

void HyperlinkWidget::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	TextButtonWidget::Unpack(data, unpackFlags);
	UnpackChunkList<HyperlinkWidget>(data, unpackFlags);
}

bool HyperlinkWidget::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'LINK':

			data >> hyperlinkAddress;
			return (true);
	}

	return (false);
}

void *HyperlinkWidget::BeginSettingsUnpack(void)
{
	hyperlinkAddress.Purge();
	return (TextButtonWidget::BeginSettingsUnpack());
}

int32 HyperlinkWidget::GetSettingCount(void) const
{
	return (TextButtonWidget::GetSettingCount() + 2);
}

Setting *HyperlinkWidget::GetSetting(int32 index) const
{
	int32 count = TextButtonWidget::GetSettingCount();
	if (index < count)
	{
		return (TextButtonWidget::GetSetting(index));
	}

	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetHyperlink, 'SETT'));
		return (new HeadingSetting(kWidgetHyperlink, title));
	}

	if (index == count + 1)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetHyperlink, 'HURL'));
		return (new TextSetting('HURL', hyperlinkAddress, title, 511));
	}

	return (nullptr);
}

void HyperlinkWidget::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'HURL')
	{
		hyperlinkAddress = static_cast<const TextSetting *>(setting)->GetText();
	}
	else
	{
		TextButtonWidget::SetSetting(setting);
	}
}

void HyperlinkWidget::Preprocess(void)
{
	TextButtonWidget::Preprocess();

	SetObserver(&hyperlinkObserver);
}

void HyperlinkWidget::HandleHyperlinkEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		if (hyperlinkAddress[0] != 0)
		{
			TheEngine->OpenExternalWebBrowser(hyperlinkAddress);
		}
	}
}


ColorWidget::ColorWidget() :
		RenderableWidget(kWidgetColor, kRenderQuads),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		borderColorAttribute(kAttributeMutable),
		borderTextureAttribute(&LineWidget::lineTextureHeader, LineWidget::lineTextureImage),
		focusColorAttribute(kAttributeMutable),
		focusRenderable(kRenderQuads),
		quadRenderable(kRenderTriangleStrip)
{
	colorValue.Set(1.0F, 1.0F, 1.0F, 1.0F);
	colorPickerFlags = kColorPickerAlpha;
	colorPickerTitle[0] = 0;

	SetDefaultColorType(kWidgetColorBorder);
	RenderableWidget::SetWidgetColor(K::black);

	colorOverrideFlags = 0;
	focusColor.Set(0.0F, 0.0F, 0.0F);
}

ColorWidget::ColorWidget(const Vector2D& size, const ColorRGBA& color) :
		RenderableWidget(kWidgetColor, kRenderQuads, size),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		borderColorAttribute(kAttributeMutable),
		borderTextureAttribute(&LineWidget::lineTextureHeader, LineWidget::lineTextureImage),
		focusColorAttribute(kAttributeMutable),
		focusRenderable(kRenderQuads),
		quadRenderable(kRenderTriangleStrip)
{
	colorValue = color;
	colorPickerFlags = kColorPickerAlpha;
	colorPickerTitle[0] = 0;

	SetDefaultColorType(kWidgetColorBorder);
	RenderableWidget::SetWidgetColor(K::black);

	colorOverrideFlags = 0;
	focusColor.Set(0.0F, 0.0F, 0.0F);
}

ColorWidget::ColorWidget(const ColorWidget& colorWidget) :
		RenderableWidget(colorWidget),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		borderColorAttribute(kAttributeMutable),
		borderTextureAttribute(&LineWidget::lineTextureHeader, LineWidget::lineTextureImage),
		focusColorAttribute(kAttributeMutable),
		focusRenderable(kRenderQuads),
		quadRenderable(kRenderTriangleStrip)
{
	colorValue = colorWidget.colorValue;
	colorPickerFlags = colorWidget.colorPickerFlags;
	colorPickerTitle = colorWidget.colorPickerTitle;

	colorOverrideFlags = colorWidget.colorOverrideFlags;
	focusColor = colorWidget.focusColor;
}

ColorWidget::~ColorWidget()
{
}

Widget *ColorWidget::Replicate(void) const
{
	return (new ColorWidget(*this));
}

void ColorWidget::SetValue(const ColorRGBA& value, bool post)
{
	if (colorValue != value)
	{
		colorValue = value;
		SetBuildFlag();

		if (post)
		{
			PostWidgetEvent(WidgetEventData(kEventWidgetChange));
		}
	}
}

void ColorWidget::Pack(Packer& data, unsigned_int32 packFlags) const
{
	RenderableWidget::Pack(data, packFlags);

	data << ChunkHeader('VALU', sizeof(ColorRGBA));
	data << colorValue;

	data << ChunkHeader('PICK', 4);
	data << colorPickerFlags;

	PackHandle handle = data.BeginChunk('TITL');
	data << colorPickerTitle;
	data.EndChunk(handle);

	data << ChunkHeader('OVER', 4);
	data << colorOverrideFlags;

	if (colorOverrideFlags & kWidgetOverrideFocusColor)
	{
		data << ChunkHeader('FOCC', sizeof(ColorRGBA));
		data << focusColor;
	}

	data << TerminatorChunk;
}

void ColorWidget::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	RenderableWidget::Unpack(data, unpackFlags);
	UnpackChunkList<ColorWidget>(data, unpackFlags);
}

bool ColorWidget::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'VALU':

			data >> colorValue;
			return (true);

		case 'PICK':

			data >> colorPickerFlags;
			return (true);

		case 'TITL':

			data >> colorPickerTitle;
			return (true);

		case 'OVER':

			data >> colorOverrideFlags;
			return (true);

		case 'FOCC':

			data >> focusColor;
			return (true);
	}

	return (false);
}

void *ColorWidget::BeginSettingsUnpack(void)
{
	colorOverrideFlags = 0;
	return (RenderableWidget::BeginSettingsUnpack());
}

void ColorWidget::ColorPickerComplete(ColorPicker *colorPicker, void *cookie)
{
	ColorWidget *colorWidget = static_cast<ColorWidget *>(cookie);
	colorWidget->colorValue = colorPicker->GetColor();

	colorWidget->SetBuildFlag();
	colorWidget->PostWidgetEvent(WidgetEventData(kEventWidgetChange));
}

int32 ColorWidget::GetSettingCount(void) const
{
	return (RenderableWidget::GetSettingCount() + 5);
}

Setting *ColorWidget::GetSetting(int32 index) const
{
	int32 count = RenderableWidget::GetSettingCount();
	if (index < count)
	{
		return (RenderableWidget::GetSetting(index));
	}

	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetColor, 'SETT'));
		return (new HeadingSetting(kWidgetColor, title));
	}

	if (index == count + 1)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetColor, 'INIT'));
		const char *picker = table->GetString(StringID('WDGT', kWidgetColor, 'PICK'));
		return (new ColorSetting('COLI', colorValue, title, picker, kColorPickerAlpha));
	}

	if (index == count + 2)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetColor, 'TITL'));
		return (new TextSetting('CPTL', colorPickerTitle, title, kMaxColorPickerTitleLength));
	}

	if (index == count + 3)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetColor, 'ALFA'));
		return (new BooleanSetting('ALFA', ((colorPickerFlags & kColorPickerAlpha) != 0), title));
	}

	if (index == count + 4)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetColor, 'FOCC'));
		const char *picker = table->GetString(StringID('WDGT', kWidgetColor, 'FOCP'));
		return (new CheckColorSetting('CWFC', ((colorOverrideFlags & kWidgetOverrideFocusColor) != 0), focusColor, title, picker, kColorPickerAlpha));
	}

	return (nullptr);
}

void ColorWidget::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'COLI')
	{
		colorValue = static_cast<const ColorSetting *>(setting)->GetColor();
	}
	else if (identifier == 'CPTL')
	{
		colorPickerTitle = static_cast<const TextSetting *>(setting)->GetText();
	}
	else if (identifier == 'ALFA')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			colorPickerFlags |= kColorPickerAlpha;
		}
		else
		{
			colorPickerFlags &= ~kColorPickerAlpha;
		}
	}
	else if (identifier == 'CWFC')
	{
		const CheckColorSetting *checkColorSetting = static_cast<const CheckColorSetting *>(setting);
		if (checkColorSetting->GetCheckValue() != 0)
		{
			colorOverrideFlags |= kWidgetOverrideFocusColor;
			focusColor = checkColorSetting->GetColor();
		}
		else
		{
			colorOverrideFlags &= ~kWidgetOverrideFocusColor;
			SetDefaultFocusColor();
		}
	}
	else
	{
		RenderableWidget::SetSetting(setting);
	}
}

const ColorRGBA& ColorWidget::GetWidgetColor(WidgetColorType type) const
{
	if (type == kWidgetColorFocus)
	{
		return (focusColor);
	}

	return (RenderableWidget::GetWidgetColor(type));
}

void ColorWidget::SetWidgetColor(const ColorRGBA& color, WidgetColorType type)
{
	if (type == kWidgetColorFocus)
	{
		focusColor = color;
		colorOverrideFlags |= kWidgetOverrideFocusColor;
	}

	RenderableWidget::SetWidgetColor(color, type);
}

void ColorWidget::SetWidgetAlpha(float alpha, WidgetColorType type)
{
	if (type == kWidgetColorFocus)
	{
		focusColor.alpha = alpha;
		colorOverrideFlags |= kWidgetOverrideFocusColor;
	}

	RenderableWidget::SetWidgetAlpha(alpha, type);
}

void ColorWidget::SetDynamicWidgetColor(const ColorRGBA& color, WidgetColorType type)
{
	if ((type == kWidgetColorDefault) || (type == kWidgetColorBorder))
	{
		borderColorAttribute.SetDiffuseColor(color);
	}
	else if (type == kWidgetColorFocus)
	{
		focusColorAttribute.SetDiffuseColor(color);
	}
	else
	{
		RenderableWidget::SetDynamicWidgetColor(color, type);
	}
}

void ColorWidget::SetDynamicWidgetAlpha(float alpha, WidgetColorType type)
{
	if ((type == kWidgetColorDefault) || (type == kWidgetColorBorder))
	{
		borderColorAttribute.SetDiffuseAlpha(alpha);
	}
	else if (type == kWidgetColorFocus)
	{
		focusColorAttribute.SetDiffuseAlpha(alpha);
	}
	else
	{
		RenderableWidget::SetDynamicWidgetAlpha(alpha, type);
	}
}

void ColorWidget::SetDefaultFocusColor(void)
{
	focusColor.Set(TheInterfaceMgr->GetInterfaceColor(kInterfaceColorButton).GetColorRGB() * 0.625F, 1.0F);
}

void ColorWidget::Preprocess(void)
{
	if (!(colorOverrideFlags & kWidgetOverrideFocusColor))
	{
		SetDefaultFocusColor();
	}

	RenderableWidget::Preprocess();

	vertexBuffer.Establish(sizeof(BorderVertex) * 16 + sizeof(GlowVertex) * 32 + sizeof(QuadVertex) * 4);

	SetVertexCount(16);
	SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(BorderVertex));
	SetVertexAttributeArray(kArrayPosition, 0, 2);
	SetVertexAttributeArray(kArrayTexcoord, sizeof(Point2D), 2);

	borderAttributeList.Append(&borderColorAttribute);
	borderAttributeList.Append(&borderTextureAttribute);
	SetMaterialAttributeList(&borderAttributeList);

	InitRenderable(&focusRenderable);
	focusRenderable.SetVertexCount(32);
	focusRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(GlowVertex));
	focusRenderable.SetVertexAttributeArray(kArrayPosition, sizeof(BorderVertex) * 16, 2);
	focusRenderable.SetVertexAttributeArray(kArrayTexcoord, sizeof(BorderVertex) * 16 + sizeof(Point2D), 2);

	focusAttributeList.Append(&focusColorAttribute);
	focusRenderable.SetMaterialAttributeList(&focusAttributeList);
	focusRenderable.SetMaterialObjectPointer(TheInterfaceMgr->GetMaterialObjectPointer());

	InitRenderable(&quadRenderable);
	quadRenderable.SetVertexCount(4);
	quadRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(QuadVertex));
	quadRenderable.SetVertexAttributeArray(kArrayPosition, sizeof(BorderVertex) * 16 + sizeof(GlowVertex) * 32, 2);
	quadRenderable.SetVertexAttributeArray(kArrayColor, sizeof(BorderVertex) * 16 + sizeof(GlowVertex) * 32 + sizeof(Point2D), 3);
}

void ColorWidget::Build(void)
{
	borderColorAttribute.SetDiffuseColor(RenderableWidget::GetWidgetColor());
	focusColorAttribute.SetDiffuseColor(focusColor);

	Box2D box(Zero2D, Zero2D + GetWidgetSize());

	volatile BorderVertex *restrict borderVertex = vertexBuffer.BeginUpdate<BorderVertex>();
	BuildBorder(box, borderVertex);

	volatile GlowVertex *restrict focusVertex = reinterpret_cast<volatile GlowVertex *>(borderVertex + 16);
	BuildGlow(box, focusVertex);

	volatile QuadVertex *restrict quadVertex = reinterpret_cast<volatile QuadVertex *>(focusVertex + 32);

	const ColorRGB& color = colorValue.GetColorRGB();
	float w = GetWidgetSize().x;
	float h = GetWidgetSize().y;

	quadVertex[0].position.Set(0.0F, 0.0F);
	quadVertex[0].color = color;

	quadVertex[1].position.Set(0.0F, h);
	quadVertex[1].color = color;

	quadVertex[2].position.Set(w, 0.0F);
	quadVertex[2].color = color;

	quadVertex[3].position.Set(w, h);
	quadVertex[3].color = color;

	vertexBuffer.EndUpdate();
}

void ColorWidget::Render(List<Renderable> *renderList)
{
	RenderableWidget::Render(renderList);

	renderList->Append(&quadRenderable);

	if (GetWidgetState() & kWidgetHilited)
	{
		renderList->Append(&focusRenderable);
	}
}

void ColorWidget::HandleMouseEvent(const PanelMouseEventData *eventData)
{
	EventType eventType = eventData->eventType;
	unsigned_int32 state = GetWidgetState();

	if (eventType == kEventMouseDown)
	{
		SetWidgetState(state | kWidgetHilited);
	}
	else if (eventType == kEventMouseMoved)
	{
		if (WidgetContainsPoint(eventData->mousePosition))
		{
			state |= kWidgetHilited;
		}
		else
		{
			state &= ~kWidgetHilited;
		}

		SetWidgetState(state);
	}
	else if (eventType == kEventMouseUp)
	{
		SetWidgetState(state & ~kWidgetHilited);
		if (WidgetContainsPoint(eventData->mousePosition))
		{
			Window *window = GetOwningWindow();
			if (window)
			{
				ColorPicker *colorPicker = new ColorPicker(colorPickerTitle, colorValue, colorPickerFlags);
				colorPicker->SetCompletionProc(&ColorPickerComplete, this);
				window->AddSubwindow(colorPicker);
			}
			else
			{
				PostWidgetEvent(WidgetEventData(kEventWidgetChange));
			}
		}
	}
}


ProgressWidget::ProgressWidget() :
		RenderableWidget(kWidgetProgress, kRenderQuads),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
{
	progressValue = 0;
	maxProgressValue = 1;

	SetDefaultColorType(kWidgetColorBackground);

	colorOverrideFlags = 0;
	hiliteColor.Set(0.0F, 0.0F, 0.0F);
}

ProgressWidget::ProgressWidget(const Vector2D& size) :
		RenderableWidget(kWidgetProgress, kRenderQuads, size),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
{
	progressValue = 0;
	maxProgressValue = 1;

	SetDefaultColorType(kWidgetColorBackground);

	colorOverrideFlags = 0;
	hiliteColor.Set(0.0F, 0.0F, 0.0F);
}

ProgressWidget::ProgressWidget(const ProgressWidget& progressWidget) :
		RenderableWidget(progressWidget),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
{
	progressValue = progressWidget.progressValue;
	maxProgressValue = progressWidget.maxProgressValue;

	colorOverrideFlags = progressWidget.colorOverrideFlags;
	hiliteColor = progressWidget.hiliteColor;
}

ProgressWidget::~ProgressWidget()
{
}

Widget *ProgressWidget::Replicate(void) const
{
	return (new ProgressWidget(*this));
}

void ProgressWidget::SetValue(int32 value, bool post)
{
	if (progressValue != value)
	{
		progressValue = value;
		SetBuildFlag();

		if (post)
		{
			PostWidgetEvent(WidgetEventData(kEventWidgetChange));
		}
	}
}

void ProgressWidget::SetMaxValue(int32 maxValue)
{
	if (maxProgressValue != maxValue)
	{
		maxProgressValue = maxValue;
		SetBuildFlag();
	}
}

void ProgressWidget::SetProgress(int32 value, int32 maxValue)
{
	if ((progressValue != value) || (maxProgressValue != maxValue))
	{
		progressValue = value;
		maxProgressValue = maxValue;
		SetBuildFlag();
	}
}

void ProgressWidget::Pack(Packer& data, unsigned_int32 packFlags) const
{
	RenderableWidget::Pack(data, packFlags);

	data << ChunkHeader('VALU', 4);
	data << progressValue;

	data << ChunkHeader('MAXV', 4);
	data << maxProgressValue;

	data << ChunkHeader('OVER', 4);
	data << colorOverrideFlags;

	if (colorOverrideFlags & kWidgetOverrideHiliteColor)
	{
		data << ChunkHeader('HILC', sizeof(ColorRGBA));
		data << hiliteColor;
	}

	data << TerminatorChunk;
}

void ProgressWidget::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	RenderableWidget::Unpack(data, unpackFlags);
	UnpackChunkList<ProgressWidget>(data, unpackFlags);
}

bool ProgressWidget::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'VALU':

			data >> progressValue;
			return (true);

		case 'MAXV':

			data >> maxProgressValue;
			return (true);

		case 'OVER':

			data >> colorOverrideFlags;
			return (true);

		case 'HILC':

			data >> hiliteColor;
			return (true);
	}

	return (false);
}

void *ProgressWidget::BeginSettingsUnpack(void)
{
	colorOverrideFlags = 0;
	return (RenderableWidget::BeginSettingsUnpack());
}

int32 ProgressWidget::GetSettingCount(void) const
{
	return (RenderableWidget::GetSettingCount() + 4);
}

Setting *ProgressWidget::GetSetting(int32 index) const
{
	int32 count = RenderableWidget::GetSettingCount();
	if (index < count)
	{
		return (RenderableWidget::GetSetting(index));
	}

	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetProgress, 'SETT'));
		return (new HeadingSetting(kWidgetProgress, title));
	}

	if (index == count + 1)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetProgress, 'INIT'));
		return (new TextSetting('PRIN', Text::IntegerToString(progressValue), title, 9, &EditTextWidget::NumberFilter));
	}

	if (index == count + 2)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetProgress, 'MAXV'));
		return (new TextSetting('PRMX', Text::IntegerToString(maxProgressValue), title, 9, &EditTextWidget::NumberFilter));
	}

	if (index == count + 3)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetProgress, 'HILC'));
		const char *picker = table->GetString(StringID('WDGT', kWidgetProgress, 'HILP'));
		return (new CheckColorSetting('PRHC', ((colorOverrideFlags & kWidgetOverrideHiliteColor) != 0), hiliteColor, title, picker, kColorPickerAlpha));
	}

	return (nullptr);
}

void ProgressWidget::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'PRIN')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		progressValue = Text::StringToInteger(text);
	}
	else if (identifier == 'PRMX')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		maxProgressValue = Max(Text::StringToInteger(text), 1);
	}
	else if (identifier == 'PRHC')
	{
		const CheckColorSetting *checkColorSetting = static_cast<const CheckColorSetting *>(setting);
		if (checkColorSetting->GetCheckValue() != 0)
		{
			colorOverrideFlags |= kWidgetOverrideHiliteColor;
			hiliteColor = checkColorSetting->GetColor();
		}
		else
		{
			colorOverrideFlags &= ~kWidgetOverrideHiliteColor;
			SetDefaultHiliteColor();
		}
	}
	else
	{
		RenderableWidget::SetSetting(setting);
	}
}

const ColorRGBA& ProgressWidget::GetWidgetColor(WidgetColorType type) const
{
	if (type == kWidgetColorHilite)
	{
		return (hiliteColor);
	}

	return (RenderableWidget::GetWidgetColor(type));
}

void ProgressWidget::SetWidgetColor(const ColorRGBA& color, WidgetColorType type)
{
	if (type == kWidgetColorHilite)
	{
		hiliteColor = color;
		colorOverrideFlags |= kWidgetOverrideHiliteColor;
	}

	RenderableWidget::SetWidgetColor(color, type);
}

void ProgressWidget::SetWidgetAlpha(float alpha, WidgetColorType type)
{
	if (type == kWidgetColorHilite)
	{
		hiliteColor.alpha = alpha;
		colorOverrideFlags |= kWidgetOverrideHiliteColor;
	}

	RenderableWidget::SetWidgetAlpha(alpha, type);
}

void ProgressWidget::SetDynamicWidgetColor(const ColorRGBA& color, WidgetColorType type)
{
	if ((type == kWidgetColorDefault) || (type == kWidgetColorBackground))
	{
		dynamicBackgroundColor = color;
		SetBuildFlag();
	}
	else if (type == kWidgetColorHilite)
	{
		dynamicHiliteColor = color;
		SetBuildFlag();
	}
	else
	{
		RenderableWidget::SetDynamicWidgetColor(color, type);
	}
}

void ProgressWidget::SetDynamicWidgetAlpha(float alpha, WidgetColorType type)
{
	if ((type == kWidgetColorDefault) || (type == kWidgetColorBackground))
	{
		dynamicBackgroundColor.alpha = alpha;
		SetBuildFlag();
	}
	else if (type == kWidgetColorHilite)
	{
		dynamicHiliteColor.alpha = alpha;
		SetBuildFlag();
	}
	else
	{
		RenderableWidget::SetDynamicWidgetAlpha(alpha, type);
	}
}

void ProgressWidget::SetDefaultHiliteColor(void)
{
	hiliteColor = TheInterfaceMgr->GetInterfaceColor(kInterfaceColorHilite);
}

void ProgressWidget::Preprocess(void)
{
	if (!(colorOverrideFlags & kWidgetOverrideHiliteColor))
	{
		SetDefaultHiliteColor();
	}

	dynamicBackgroundColor = GetWidgetColor();
	dynamicHiliteColor = hiliteColor;

	RenderableWidget::Preprocess();

	SetVertexCount(8);
	SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(ProgressVertex));
	SetVertexAttributeArray(kArrayPosition, 0, 2);
	SetVertexAttributeArray(kArrayColor, sizeof(Point2D), 4);
	SetVertexAttributeArray(kArrayTexcoord, sizeof(Point2D) + sizeof(ColorRGBA), 2);
	vertexBuffer.Establish(sizeof(ProgressVertex) * 8);

	SetMaterialObjectPointer(TheInterfaceMgr->GetMaterialObjectPointer());
}

void ProgressWidget::Build(void)
{
	volatile ProgressVertex *restrict vertex = vertexBuffer.BeginUpdate<ProgressVertex>();

	float w = GetWidgetSize().x;
	float h = GetWidgetSize().y;
	float x = w * (float) Min(MaxZero(progressValue), maxProgressValue) / (float) Max(maxProgressValue, 1);

	vertex[0].position.Set(0.0F, 0.0F);
	vertex[0].color = dynamicHiliteColor;
	vertex[0].texcoord.Set(0.25F, 0.4296875F);

	vertex[1].position.Set(0.0F, h);
	vertex[1].color = dynamicHiliteColor;
	vertex[1].texcoord.Set(0.25F, 0.328125F);

	vertex[2].position.Set(x, h);
	vertex[2].color = dynamicHiliteColor;
	vertex[2].texcoord.Set(0.25F, 0.328125F);

	vertex[3].position.Set(x, 0.0F);
	vertex[3].color = dynamicHiliteColor;
	vertex[3].texcoord.Set(0.25F, 0.4296875F);

	vertex[4].position.Set(x, 0.0F);
	vertex[4].color = dynamicBackgroundColor;
	vertex[4].texcoord.Set(0.25F, 0.4296875F);

	vertex[5].position.Set(x, h);
	vertex[5].color = dynamicBackgroundColor;
	vertex[5].texcoord.Set(0.25F, 0.328125F);

	vertex[6].position.Set(w, h);
	vertex[6].color = dynamicBackgroundColor;
	vertex[6].texcoord.Set(0.25F, 0.328125F);

	vertex[7].position.Set(w, 0.0F);
	vertex[7].color = dynamicBackgroundColor;
	vertex[7].texcoord.Set(0.25F, 0.4296875F);

	vertexBuffer.EndUpdate();
}


SliderWidget::SliderWidget() :
		RenderableWidget(kWidgetSlider, kRenderQuads),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
{
	sliderStyle = kSliderRound;

	sliderValue = 0;
	maxSliderValue = 1;

	SetDefaultColorType(kWidgetColorBackground);

	colorOverrideFlags = 0;
	buttonColor.Set(0.0F, 0.0F, 0.0F);
}

SliderWidget::SliderWidget(const Vector2D& size) :
		RenderableWidget(kWidgetSlider, kRenderQuads, size),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
{
	sliderStyle = kSliderRound;

	sliderValue = 0;
	maxSliderValue = 1;

	SetDefaultColorType(kWidgetColorBackground);

	colorOverrideFlags = 0;
	buttonColor.Set(0.0F, 0.0F, 0.0F);
}

SliderWidget::SliderWidget(const SliderWidget& sliderWidget) :
		RenderableWidget(sliderWidget),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
{
	sliderStyle = sliderWidget.sliderStyle;

	sliderValue = sliderWidget.sliderValue;
	maxSliderValue = sliderWidget.maxSliderValue;

	colorOverrideFlags = sliderWidget.colorOverrideFlags;
	buttonColor = sliderWidget.buttonColor;
}

SliderWidget::~SliderWidget()
{
}

Widget *SliderWidget::Replicate(void) const
{
	return (new SliderWidget(*this));
}

void SliderWidget::SetValue(int32 value, bool post)
{
	if (sliderValue != value)
	{
		sliderValue = value;
		SetBuildFlag();

		if (post)
		{
			PostWidgetEvent(WidgetEventData(kEventWidgetChange));
		}
	}
}

void SliderWidget::SetMaxValue(int32 maxValue)
{
	if (maxSliderValue != maxValue)
	{
		maxSliderValue = maxValue;
		SetBuildFlag();
	}
}

void SliderWidget::Pack(Packer& data, unsigned_int32 packFlags) const
{
	RenderableWidget::Pack(data, packFlags);

	data << ChunkHeader('STYL', 4);
	data << sliderStyle;

	data << ChunkHeader('VALU', 4);
	data << sliderValue;

	data << ChunkHeader('MAXV', 4);
	data << maxSliderValue;

	data << ChunkHeader('OVER', 4);
	data << colorOverrideFlags;

	if (colorOverrideFlags & kWidgetOverrideButtonColor)
	{
		data << ChunkHeader('BTTC', sizeof(ColorRGBA));
		data << buttonColor;
	}

	data << TerminatorChunk;
}

void SliderWidget::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	RenderableWidget::Unpack(data, unpackFlags);
	UnpackChunkList<SliderWidget>(data, unpackFlags);
}

bool SliderWidget::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'STYL':

			data >> sliderStyle;
			return (true);

		case 'VALU':

			data >> sliderValue;
			return (true);

		case 'MAXV':

			data >> maxSliderValue;
			return (true);

		case 'OVER':

			data >> colorOverrideFlags;
			return (true);

		case 'BTTC':

			data >> buttonColor;
			return (true);
	}

	return (false);
}

void *SliderWidget::BeginSettingsUnpack(void)
{
	colorOverrideFlags = 0;
	return (RenderableWidget::BeginSettingsUnpack());
}

int32 SliderWidget::GetSettingCount(void) const
{
	return (RenderableWidget::GetSettingCount() + 5);
}

Setting *SliderWidget::GetSetting(int32 index) const
{
	int32 count = RenderableWidget::GetSettingCount();
	if (index < count)
	{
		return (RenderableWidget::GetSetting(index));
	}

	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetSlider, 'SETT'));
		return (new HeadingSetting(kWidgetSlider, title));
	}

	if (index == count + 1)
	{
		int32 selection = MaxZero(Min(sliderStyle, kSliderStyleCount - 1));

		const char *title = table->GetString(StringID('WDGT', kWidgetSlider, 'STYL'));
		MenuSetting *menu = new MenuSetting('SSTY', selection, title, kSliderStyleCount);

		menu->SetMenuItemString(0, table->GetString(StringID('WDGT', kWidgetSlider, 'STYL', 'ROND')));
		menu->SetMenuItemString(1, table->GetString(StringID('WDGT', kWidgetSlider, 'STYL', 'SQAR')));

		return (menu);
	}

	if (index == count + 2)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetSlider, 'INIT'));
		return (new TextSetting('SLIN', Text::IntegerToString(sliderValue), title, 9, &EditTextWidget::NumberFilter));
	}

	if (index == count + 3)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetSlider, 'MAXV'));
		return (new TextSetting('SLMX', Text::IntegerToString(maxSliderValue), title, 9, &EditTextWidget::NumberFilter));
	}

	if (index == count + 4)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetSlider, 'BTTC'));
		const char *picker = table->GetString(StringID('WDGT', kWidgetSlider, 'BTTP'));
		return (new CheckColorSetting('SLBC', ((colorOverrideFlags & kWidgetOverrideButtonColor) != 0), buttonColor, title, picker, kColorPickerAlpha));
	}

	return (nullptr);
}

void SliderWidget::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'SSTY')
	{
		sliderStyle = static_cast<const MenuSetting *>(setting)->GetMenuSelection();
	}
	else if (identifier == 'SLIN')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		sliderValue = Text::StringToInteger(text);
	}
	else if (identifier == 'SLMX')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		maxSliderValue = Max(Text::StringToInteger(text), 1);
	}
	else if (identifier == 'SLBC')
	{
		const CheckColorSetting *checkColorSetting = static_cast<const CheckColorSetting *>(setting);
		if (checkColorSetting->GetCheckValue() != 0)
		{
			colorOverrideFlags |= kWidgetOverrideButtonColor;
			buttonColor = checkColorSetting->GetColor();
		}
		else
		{
			colorOverrideFlags &= ~kWidgetOverrideButtonColor;
			SetDefaultButtonColor();
		}
	}
	else
	{
		RenderableWidget::SetSetting(setting);
	}
}

const ColorRGBA& SliderWidget::GetWidgetColor(WidgetColorType type) const
{
	if (type == kWidgetColorButton)
	{
		return (buttonColor);
	}

	return (RenderableWidget::GetWidgetColor(type));
}

void SliderWidget::SetWidgetColor(const ColorRGBA& color, WidgetColorType type)
{
	if (type == kWidgetColorButton)
	{
		buttonColor = color;
		colorOverrideFlags |= kWidgetOverrideButtonColor;
		SetBuildFlag();
	}

	RenderableWidget::SetWidgetColor(color, type);
}

void SliderWidget::SetDynamicWidgetColor(const ColorRGBA& color, WidgetColorType type)
{
	if ((type == kWidgetColorDefault) || (type == kWidgetColorBackground))
	{
		dynamicBackgroundColor = color;
		SetBuildFlag();
	}
	else if (type == kWidgetColorButton)
	{
		dynamicButtonColor = color;
		SetBuildFlag();
	}
	else
	{
		RenderableWidget::SetDynamicWidgetColor(color, type);
	}
}

void SliderWidget::SetDynamicWidgetAlpha(float alpha, WidgetColorType type)
{
	if ((type == kWidgetColorDefault) || (type == kWidgetColorBackground))
	{
		dynamicBackgroundColor.alpha = alpha;
		SetBuildFlag();
	}
	else if (type == kWidgetColorButton)
	{
		dynamicButtonColor.alpha = alpha;
		SetBuildFlag();
	}
	else
	{
		RenderableWidget::SetDynamicWidgetAlpha(alpha, type);
	}
}

void SliderWidget::SetDefaultButtonColor(void)
{
	buttonColor = TheInterfaceMgr->GetInterfaceColor(kInterfaceColorButton);
}

bool SliderWidget::CalculateBoundingBox(Box2D *box) const
{
	box->min.Set(-4.0F, 1.0F);
	box->max.Set(GetWidgetSize().x + 4.0F, 17.0F);
	return (true);
}

WidgetPart SliderWidget::TestPosition(const Point3D& position) const
{
	if (sliderValue != kWidgetValueIndeterminant)
	{
		float x = GetIndicatorPosition();
		if ((position.x >= x - 8.0F) && (position.x <= x + 8.0F))
		{
			return (kWidgetPartIndicator);
		}
	}

	if ((position.x >= 0.0F) && (position.x <= GetWidgetSize().x) && (position.y >= 4.0F) && (position.y <= 12.0F))
	{
		return (kWidgetPartSlider);
	}

	return (kWidgetPartNone);
}

float SliderWidget::GetIndicatorPosition(void) const
{
	return (Floor((GetWidgetSize().x - 8.0F) * (float) Min(MaxZero(sliderValue), maxSliderValue) / (float) Max(maxSliderValue, 1)) + 4.0F);
}

int32 SliderWidget::GetPositionValue(float x) const
{
	int32 value = (int32) ((x - 4.0F) * (float) maxSliderValue / (GetWidgetSize().x - 8.0F) + 0.5F);
	return (MaxZero(Min(value, maxSliderValue)));
}

void SliderWidget::Preprocess(void)
{
	if (!(colorOverrideFlags & kWidgetOverrideButtonColor))
	{
		SetDefaultButtonColor();
	}

	dynamicBackgroundColor = GetWidgetColor();
	dynamicButtonColor = buttonColor;

	RenderableWidget::Preprocess();

	SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(SliderVertex));
	SetVertexAttributeArray(kArrayPosition, 0, 2);
	SetVertexAttributeArray(kArrayColor, sizeof(Point2D), 4);
	SetVertexAttributeArray(kArrayTexcoord, sizeof(Point2D) + sizeof(ColorRGBA), 2);
	vertexBuffer.Establish(sizeof(SliderVertex) * 16);

	SetMaterialObjectPointer(TheInterfaceMgr->GetMaterialObjectPointer());
}

void SliderWidget::Build(void)
{
	volatile SliderVertex *restrict vertex = vertexBuffer.BeginUpdate<SliderVertex>();

	float dx = GetWidgetSize().x + 2.0F;
	float ds = Fmin((dx - 6.0F) / 48.0F, 0.375F);

	float alpha = (GetGlobalWidgetState() & kWidgetDisabled) ? 0.5F : 1.0F;
	ColorRGBA color(dynamicBackgroundColor.GetColorRGB(), dynamicBackgroundColor.alpha * alpha);

	vertex[0].position.Set(-1.0F, 5.0F);
	vertex[0].color = color;
	vertex[0].texcoord.Set(0.0F, 0.25F);

	vertex[1].position.Set(-1.0F, 11.0F);
	vertex[1].color = color;
	vertex[1].texcoord.Set(0.0F, 0.125F);

	vertex[2].position.Set(2.0F, 11.0F);
	vertex[2].color = color;
	vertex[2].texcoord.Set(0.0625F, 0.125F);

	vertex[3].position.Set(2.0F, 5.0F);
	vertex[3].color = color;
	vertex[3].texcoord.Set(0.0625F, 0.25F);

	vertex[4].position.Set(2.0F, 5.0F);
	vertex[4].color = color;
	vertex[4].texcoord.Set(0.0625F, 0.25F);

	vertex[5].position.Set(2.0F, 11.0F);
	vertex[5].color = color;
	vertex[5].texcoord.Set(0.0625F, 0.125F);

	vertex[6].position.Set(dx - 4.0F, 11.0F);
	vertex[6].color = color;
	vertex[6].texcoord.Set(0.0625F + ds, 0.125F);

	vertex[7].position.Set(dx - 4.0F, 5.0F);
	vertex[7].color = color;
	vertex[7].texcoord.Set(0.0625F + ds, 0.25F);

	vertex[8].position.Set(dx - 4.0F, 5.0F);
	vertex[8].color = color;
	vertex[8].texcoord.Set(0.4375F, 0.25F);

	vertex[9].position.Set(dx - 4.0F, 11.0F);
	vertex[9].color = color;
	vertex[9].texcoord.Set(0.4375F, 0.125F);

	vertex[10].position.Set(dx - 1.0F, 11.0F);
	vertex[10].color = color;
	vertex[10].texcoord.Set(0.5F, 0.125F);

	vertex[11].position.Set(dx - 1.0F, 5.0F);
	vertex[11].color = color;
	vertex[11].texcoord.Set(0.5F, 0.25F);

	if (sliderValue != kWidgetValueIndeterminant)
	{
		SetVertexCount(16);

		float x = GetIndicatorPosition() - 8.0F;

		float rgb = (GetWidgetState() & kWidgetHilited) ? 0.625F : 1.0F;
		alpha = (GetGlobalWidgetState() & kWidgetDisabled) ? 0.5F : 1.0F;
		color.Set(dynamicButtonColor.GetColorRGB() * rgb, dynamicButtonColor.alpha * alpha);

		vertex[12].position.Set(x, 1.0F);
		vertex[12].color = color;

		vertex[13].position.Set(x, 17.0F);
		vertex[13].color = color;

		vertex[14].position.Set(x + 16.0F, 17.0F);
		vertex[14].color = color;

		vertex[15].position.Set(x + 16.0F, 1.0F);
		vertex[15].color = color;

		if (sliderStyle == kSliderRound)
		{
			vertex[12].texcoord.Set(0.0F, 1.0F);
			vertex[13].texcoord.Set(0.0F, 0.875F);
			vertex[14].texcoord.Set(0.125F, 0.875F);
			vertex[15].texcoord.Set(0.125F, 1.0F);
		}
		else
		{
			vertex[12].texcoord.Set(0.0F, 0.8125F);
			vertex[13].texcoord.Set(0.0F, 0.6875F);
			vertex[14].texcoord.Set(0.125F, 0.6875F);
			vertex[15].texcoord.Set(0.125F, 0.8125F);
		}
	}
	else
	{
		SetVertexCount(12);
	}

	vertexBuffer.EndUpdate();
}

void SliderWidget::HandleMouseEvent(const PanelMouseEventData *eventData)
{
	EventType eventType = eventData->eventType;
	if (eventType == kEventMouseDown)
	{
		if (eventData->widgetPart == kWidgetPartIndicator)
		{
			dragPosition = eventData->mousePosition.x - GetIndicatorPosition();
			SetWidgetState(GetWidgetState() | kWidgetHilited);
		}
		else
		{
			int32 value = GetPositionValue(eventData->mousePosition.x);
			if (sliderValue != value)
			{
				sliderValue = value;
				SetBuildFlag();
				PostWidgetEvent(WidgetEventData(kEventWidgetChange));
			}
		}
	}
	else if (eventType == kEventMouseMoved)
	{
		if (eventData->widgetPart == kWidgetPartIndicator)
		{
			int32 value = GetPositionValue(eventData->mousePosition.x - dragPosition);
			if (sliderValue != value)
			{
				sliderValue = value;
				SetBuildFlag();
				PostWidgetEvent(WidgetEventData(kEventWidgetChange));
			}
		}
	}
	else if (eventType == kEventMouseUp)
	{
		SetWidgetState(GetWidgetState() & ~kWidgetHilited);
	}
}


ScrollWidget::ScrollWidget() :
		RenderableWidget(kWidgetScroll, kRenderQuads),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
{
	scrollFlags = 0;
	scrollValue = 0;
	maxScrollValue = 1;
	pageDistance = 2;

	SetDefaultColorType(kWidgetColorBackground);
	buttonColor.Set(1.0F, 1.0F, 1.0F);

	indicatorHilite = false;
	upButtonHilite = false;
	downButtonHilite = false;
	pageUpHilite = false;
	pageDownHilite = false;
}

ScrollWidget::ScrollWidget(const Vector2D& size) :
		RenderableWidget(kWidgetScroll, kRenderQuads, size),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
{
	scrollFlags = 0;
	scrollValue = 0;
	maxScrollValue = 1;
	pageDistance = 2;

	SetDefaultColorType(kWidgetColorBackground);
	buttonColor.Set(1.0F, 1.0F, 1.0F);

	indicatorHilite = false;
	upButtonHilite = false;
	downButtonHilite = false;
	pageUpHilite = false;
	pageDownHilite = false;
}

ScrollWidget::ScrollWidget(const ScrollWidget& scrollWidget) :
		RenderableWidget(scrollWidget),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
{
	scrollFlags = scrollWidget.scrollFlags;
	scrollValue = scrollWidget.scrollValue;
	maxScrollValue = scrollWidget.maxScrollValue;
	pageDistance = scrollWidget.pageDistance;
	buttonColor = scrollWidget.buttonColor;

	indicatorHilite = false;
	upButtonHilite = false;
	downButtonHilite = false;
	pageUpHilite = false;
	pageDownHilite = false;
}

ScrollWidget::~ScrollWidget()
{
}

Widget *ScrollWidget::Replicate(void) const
{
	return (new ScrollWidget(*this));
}

void ScrollWidget::SetValue(int32 value, bool post)
{
	if (scrollValue != value)
	{
		scrollValue = value;
		SetBuildFlag();

		if (post)
		{
			PostWidgetEvent(WidgetEventData(kEventWidgetChange));
		}
	}
}

void ScrollWidget::SetMaxValue(int32 maxValue)
{
	if (maxScrollValue != maxValue)
	{
		maxScrollValue = maxValue;
		SetBuildFlag();
	}
}

void ScrollWidget::Pack(Packer& data, unsigned_int32 packFlags) const
{
	RenderableWidget::Pack(data, packFlags);

	data << ChunkHeader('FLAG', 4);
	data << scrollFlags;

	data << ChunkHeader('VALU', 4);
	data << scrollValue;

	data << ChunkHeader('MAXV', 4);
	data << maxScrollValue;

	data << ChunkHeader('PAGE', 4);
	data << pageDistance;

	data << ChunkHeader('BTTC', sizeof(ColorRGBA));
	data << buttonColor;

	data << TerminatorChunk;
}

void ScrollWidget::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	RenderableWidget::Unpack(data, unpackFlags);
	UnpackChunkList<ScrollWidget>(data, unpackFlags);
}

bool ScrollWidget::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> scrollFlags;
			return (true);

		case 'VALU':

			data >> scrollValue;
			return (true);

		case 'MAXV':

			data >> maxScrollValue;
			return (true);

		case 'PAGE':

			data >> pageDistance;
			return (true);

		case 'BTTC':

			data >> buttonColor;
			return (true);
	}

	return (false);
}

int32 ScrollWidget::GetSettingCount(void) const
{
	return (RenderableWidget::GetSettingCount() + 6);
}

Setting *ScrollWidget::GetSetting(int32 index) const
{
	int32 count = RenderableWidget::GetSettingCount();
	if (index < count)
	{
		return (RenderableWidget::GetSetting(index));
	}

	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetScroll, 'SETT'));
		return (new HeadingSetting(kWidgetScroll, title));
	}

	if (index == count + 1)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetScroll, 'HORZ'));
		return (new BooleanSetting('HORZ', ((scrollFlags & kScrollHorizontal) != 0), title));
	}

	if (index == count + 2)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetScroll, 'INIT'));
		return (new TextSetting('SCIN', Text::IntegerToString(scrollValue), title, 9, &EditTextWidget::NumberFilter));
	}

	if (index == count + 3)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetScroll, 'MAXV'));
		return (new TextSetting('SCMX', Text::IntegerToString(maxScrollValue), title, 9, &EditTextWidget::NumberFilter));
	}

	if (index == count + 4)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetScroll, 'PAGE'));
		return (new TextSetting('PAGE', Text::IntegerToString(pageDistance), title, 9, &EditTextWidget::NumberFilter));
	}

	if (index == count + 5)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetScroll, 'BTTC'));
		const char *picker = table->GetString(StringID('WDGT', kWidgetScroll, 'BTTP'));
		return (new ColorSetting('SCBC', buttonColor, title, picker, kColorPickerAlpha));
	}

	return (nullptr);
}

void ScrollWidget::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'HORZ')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			scrollFlags |= kScrollHorizontal;
		}
		else
		{
			scrollFlags &= ~kScrollHorizontal;
		}
	}
	else if (identifier == 'SCIN')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		scrollValue = Text::StringToInteger(text);
	}
	else if (identifier == 'SCMX')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		maxScrollValue = Max(Text::StringToInteger(text), 1);
	}
	else if (identifier == 'PAGE')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		pageDistance = Max(Text::StringToInteger(text), 1);
	}
	else if (identifier == 'SCBC')
	{
		buttonColor = static_cast<const ColorSetting *>(setting)->GetColor();
	}
	else
	{
		RenderableWidget::SetSetting(setting);
	}
}

const ColorRGBA& ScrollWidget::GetWidgetColor(WidgetColorType type) const
{
	if (type == kWidgetColorButton)
	{
		return (buttonColor);
	}

	return (RenderableWidget::GetWidgetColor(type));
}

void ScrollWidget::SetWidgetColor(const ColorRGBA& color, WidgetColorType type)
{
	if (type == kWidgetColorButton)
	{
		buttonColor = color;
	}

	RenderableWidget::SetWidgetColor(color, type);
}

void ScrollWidget::SetDynamicWidgetColor(const ColorRGBA& color, WidgetColorType type)
{
	if ((type == kWidgetColorDefault) || (type == kWidgetColorBackground))
	{
		dynamicBackgroundColor = color;
		SetBuildFlag();
	}
	else if (type == kWidgetColorButton)
	{
		dynamicButtonColor = color;
		SetBuildFlag();
	}
	else
	{
		RenderableWidget::SetDynamicWidgetColor(color, type);
	}
}

void ScrollWidget::SetDynamicWidgetAlpha(float alpha, WidgetColorType type)
{
	if ((type == kWidgetColorDefault) || (type == kWidgetColorBackground))
	{
		dynamicBackgroundColor.alpha = alpha;
		SetBuildFlag();
	}
	else if (type == kWidgetColorButton)
	{
		dynamicButtonColor.alpha = alpha;
		SetBuildFlag();
	}
	else
	{
		RenderableWidget::SetDynamicWidgetAlpha(alpha, type);
	}
}

bool ScrollWidget::CalculateBoundingBox(Box2D *box) const
{
	box->min.Set(0.0F, 0.0F);
	box->max.Set(16.0F, GetWidgetSize().y);
	return (true);
}

WidgetPart ScrollWidget::TestPosition(const Point3D& position) const
{
	if (maxScrollValue > 0)
	{
		if (position.y < 16.0F)
		{
			return (kWidgetPartUpButton);
		}

		if (position.y > GetWidgetSize().y - 16.0F)
		{
			return (kWidgetPartDownButton);
		}

		float top = GetIndicatorPosition();
		if (position.y < top)
		{
			return (kWidgetPartPageUp);
		}

		if (position.y > top + 15.0F)
		{
			return (kWidgetPartPageDown);
		}

		return (kWidgetPartIndicator);
	}

	return (kWidgetPartNone);
}

float ScrollWidget::GetIndicatorPosition(void) const
{
	return (Floor((GetWidgetSize().y - 48.0F) * (float) Min(MaxZero(scrollValue), maxScrollValue) / (float) Max(maxScrollValue, 1)) + 17.0F);
}

void ScrollWidget::Preprocess(void)
{
	dynamicBackgroundColor = GetWidgetColor();
	dynamicButtonColor = buttonColor;

	RenderableWidget::Preprocess();

	SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(ScrollVertex));
	SetVertexAttributeArray(kArrayPosition, 0, 2);
	SetVertexAttributeArray(kArrayColor, sizeof(Point2D), 4);
	SetVertexAttributeArray(kArrayTexcoord, sizeof(Point2D) + sizeof(ColorRGBA), 2);
	vertexBuffer.Establish(sizeof(ScrollVertex) * 20);

	SetMaterialObjectPointer(TheInterfaceMgr->GetMaterialObjectPointer());
}

void ScrollWidget::Build(void)
{
	volatile ScrollVertex *restrict vertex = vertexBuffer.BeginUpdate<ScrollVertex>();

	float height = Fmax(GetWidgetSize().y, 48.0F);

	if (!(scrollFlags & kScrollHorizontal))
	{
		vertex[8].position.Set(-1.0F, 0.0F);
		vertex[9].position.Set(-1.0F, 18.0F);
		vertex[10].position.Set(17.0F, 18.0F);
		vertex[11].position.Set(17.0F, 0.0F);

		vertex[12].position.Set(-1.0F, height - 16.0F);
		vertex[13].position.Set(-1.0F, height + 2.0F);
		vertex[14].position.Set(17.0F, height + 2.0F);
		vertex[15].position.Set(17.0F, height - 16.0F);
	}
	else
	{
		vertex[8].position.Set(-2.0F, -1.0F);
		vertex[9].position.Set(-2.0F, 17.0F);
		vertex[10].position.Set(16.0F, 17.0F);
		vertex[11].position.Set(16.0F, -1.0F);

		vertex[12].position.Set(-2.0F, height - 17.0F);
		vertex[13].position.Set(-2.0F, height + 1.0F);
		vertex[14].position.Set(16.0F, height + 1.0F);
		vertex[15].position.Set(16.0F, height - 17.0F);
	}

	float rgb = (upButtonHilite) ? 0.625F : 1.0F;
	ColorRGBA up(dynamicButtonColor.GetColorRGB() * rgb, dynamicButtonColor.alpha);
	for (machine a = 8; a < 12; a++)
	{
		vertex[a].color = up;
	}

	rgb = (downButtonHilite) ? 0.625F : 1.0F;
	ColorRGBA down(dynamicButtonColor.GetColorRGB() * rgb, dynamicButtonColor.alpha);
	for (machine a = 12; a < 16; a++)
	{
		vertex[a].color = down;
	}

	float t2 = Fmax(1.0F - height * 0.001953125F, 0.501953125F);
	if (maxScrollValue > 0)
	{
		SetVertexCount(20);

		float y = GetIndicatorPosition() + 8.0F;
		if (!(scrollFlags & kScrollHorizontal))
		{
			vertex[16].position.Set(-1.0F, y - 9.0F);
			vertex[17].position.Set(-1.0F, y + 9.0F);
			vertex[18].position.Set(17.0F, y + 9.0F);
			vertex[19].position.Set(17.0F, y - 9.0F);

			vertex[16].texcoord.Set(0.0F, 0.8125F);
			vertex[17].texcoord.Set(0.0F, 0.6875F);
			vertex[18].texcoord.Set(0.125F, 0.6875F);
			vertex[19].texcoord.Set(0.125F, 0.8125F);
		}
		else
		{
			vertex[16].position.Set(-2.0F, y - 10.0F);
			vertex[17].position.Set(-2.0F, y + 8.0F);
			vertex[18].position.Set(16.0F, y + 8.0F);
			vertex[19].position.Set(16.0F, y - 10.0F);

			vertex[16].texcoord.Set(0.0F, 0.6875F);
			vertex[17].texcoord.Set(0.125F, 0.6875F);
			vertex[18].texcoord.Set(0.125F, 0.8125F);
			vertex[19].texcoord.Set(0.0F, 0.8125F);
		}

		rgb = (indicatorHilite) ? 0.625F : 1.0F;
		ColorRGBA ind(dynamicButtonColor.GetColorRGB() * rgb, dynamicButtonColor.alpha);
		for (machine a = 16; a < 20; a++)
		{
			vertex[a].color = ind;
		}

		vertex[0].position.Set(0.0F, 0.0F);
		vertex[1].position.Set(0.0F, y);
		vertex[2].position.Set(16.0F, y);
		vertex[3].position.Set(16.0F, 0.0F);
		vertex[4].position.Set(0.0F, y);
		vertex[5].position.Set(0.0F, height);
		vertex[6].position.Set(16.0F, height);
		vertex[7].position.Set(16.0F, y);

		rgb = (pageUpHilite) ? 0.625F : 1.0F;
		up.Set(dynamicBackgroundColor.GetColorRGB() * rgb, dynamicBackgroundColor.alpha);
		for (machine a = 0; a < 4; a++)
		{
			vertex[a].color = up;
		}

		rgb = (pageDownHilite) ? 0.625F : 1.0F;
		down.Set(dynamicBackgroundColor.GetColorRGB() * rgb, dynamicBackgroundColor.alpha);
		for (machine a = 4; a < 8; a++)
		{
			vertex[a].color = down;
		}

		float t1 = Fmax(1.0F - y * 0.001953125F, 0.501953125F);
		vertex[0].texcoord.Set(0.96875F, 1.0F);
		vertex[1].texcoord.Set(0.96875F, t1);
		vertex[2].texcoord.Set(1.0F, t1);
		vertex[3].texcoord.Set(1.0F, 1.0F);

		vertex[4].texcoord.Set(0.96875F, t1);
		vertex[5].texcoord.Set(0.96875F, t2);
		vertex[6].texcoord.Set(1.0F, t2);
		vertex[7].texcoord.Set(1.0F, t1);
	}
	else
	{
		SetVertexCount(16);

		vertex[0].position.Set(0.0F, 0.0F);
		vertex[1].position.Set(0.0F, 0.0F);
		vertex[2].position.Set(16.0F, 0.0F);
		vertex[3].position.Set(16.0F, 0.0F);

		vertex[4].position.Set(0.0F, 0.0F);
		vertex[5].position.Set(0.0F, height);
		vertex[6].position.Set(16.0F, height);
		vertex[7].position.Set(16.0F, 0.0F);

		vertex[0].texcoord.Set(0.96875F, 1.0F);
		vertex[1].texcoord.Set(0.96875F, 1.0F);
		vertex[2].texcoord.Set(1.0F, 1.0F);
		vertex[3].texcoord.Set(1.0F, 1.0F);

		vertex[4].texcoord.Set(0.96875F, 1.0F);
		vertex[5].texcoord.Set(0.96875F, t2);
		vertex[6].texcoord.Set(1.0F, t2);
		vertex[7].texcoord.Set(1.0F, 1.0F);

		for (machine a = 0; a < 8; a ++)
		{
			vertex[a].color = dynamicBackgroundColor;
		}
	}

	if (!(scrollFlags & kScrollHorizontal))
	{
		vertex[8].texcoord.Set(0.0F, 0.625F);
		vertex[9].texcoord.Set(0.0F, 0.5F);
		vertex[10].texcoord.Set(0.125F, 0.5F);
		vertex[11].texcoord.Set(0.125F, 0.625F);

		vertex[12].texcoord.Set(0.1875F, 0.625F);
		vertex[13].texcoord.Set(0.1875F, 0.5F);
		vertex[14].texcoord.Set(0.3125F, 0.5F);
		vertex[15].texcoord.Set(0.3125F, 0.625F);
	}
	else
	{
		vertex[8].texcoord.Set(0.375F, 0.5F);
		vertex[9].texcoord.Set(0.5F, 0.5F);
		vertex[10].texcoord.Set(0.5F, 0.625F);
		vertex[11].texcoord.Set(0.375F, 0.625F);

		vertex[12].texcoord.Set(0.5625F, 0.5F);
		vertex[13].texcoord.Set(0.6875F, 0.5F);
		vertex[14].texcoord.Set(0.6875F, 0.625F);
		vertex[15].texcoord.Set(0.5625F, 0.625F);
	}

	vertexBuffer.EndUpdate();
}

void ScrollWidget::Advance(WidgetPart widgetPart)
{
	int32 value = scrollValue;

	switch (widgetPart)
	{
		case kWidgetPartUpButton:

			value = MaxZero(value - 1);
			break;

		case kWidgetPartDownButton:

			value = Min(value + 1, maxScrollValue);
			break;

		case kWidgetPartPageUp:

			value = MaxZero(value - pageDistance);
			break;

		case kWidgetPartPageDown:

			value = Min(value + pageDistance, maxScrollValue);
			break;
	}

	if (scrollValue != value)
	{
		scrollValue = value;
		SetBuildFlag();
		PostWidgetEvent(WidgetEventData(kEventWidgetChange));
	}
}

void ScrollWidget::HandleMouseEvent(const PanelMouseEventData *eventData)
{
	WidgetPart widgetPart = eventData->widgetPart;
	bool advanceFlag = false;

	EventType eventType = eventData->eventType;
	if (eventType == kEventMouseDown)
	{
		if (widgetPart == kWidgetPartIndicator)
		{
			dragPosition = eventData->mousePosition.y - GetIndicatorPosition();
		}
		else
		{
			advanceFlag = true;
			advanceTime = TheTimeMgr->GetSystemAbsoluteTime() + 100;
		}
	}
	else if (eventType == kEventMouseMoved)
	{
		if (widgetPart == kWidgetPartIndicator)
		{
			float y = eventData->mousePosition.y - dragPosition;
			int32 value = (int32) ((y - 16.0F) * (float) maxScrollValue / (GetWidgetSize().y - 47.0F) + 0.5F);
			value = MaxZero(Min(value, maxScrollValue));

			if (scrollValue != value)
			{
				scrollValue = value;
				SetBuildFlag();
				PostWidgetEvent(WidgetEventData(kEventWidgetChange));
				return;
			}
		}
		else
		{
			if ((WidgetContainsPoint(eventData->mousePosition)) && (ScrollWidget::TestPosition(eventData->mousePosition) == widgetPart))
			{
				int32 time = advanceTime;
				if ((int32) TheTimeMgr->GetSystemAbsoluteTime() - time >= 50)
				{
					advanceFlag = true;
					advanceTime = time + 50;
				}
			}
			else
			{
				widgetPart = kWidgetPartNone;
			}
		}
	}
	else
	{
		widgetPart = kWidgetPartNone;
	}

	switch (widgetPart)
	{
		case kWidgetPartNone:

			if (indicatorHilite | upButtonHilite | downButtonHilite | pageUpHilite | pageDownHilite)
			{
				indicatorHilite = false;
				upButtonHilite = false;
				downButtonHilite = false;
				pageUpHilite = false;
				pageDownHilite = false;
				SetBuildFlag();
			}

			break;

		case kWidgetPartIndicator:

			if (!indicatorHilite)
			{
				indicatorHilite = true;
				SetBuildFlag();
			}

			break;

		case kWidgetPartUpButton:

			if (!upButtonHilite)
			{
				upButtonHilite = true;
				SetBuildFlag();
			}

			break;

		case kWidgetPartDownButton:

			if (!downButtonHilite)
			{
				downButtonHilite = true;
				SetBuildFlag();
			}

			break;

		case kWidgetPartPageUp:

			if (!pageUpHilite)
			{
				pageUpHilite = true;
				SetBuildFlag();
			}

			break;

		case kWidgetPartPageDown:

			if (!pageDownHilite)
			{
				pageDownHilite = true;
				SetBuildFlag();
			}

			break;
	}

	if (advanceFlag)
	{
		Advance(widgetPart);
	}
}

void ScrollWidget::TrackTask(WidgetPart widgetPart, const Point3D& mousePosition)
{
	int32 time = advanceTime;
	if ((int32) TheTimeMgr->GetSystemAbsoluteTime() - time >= 50)
	{
		advanceTime = time + 50;

		bool advanceFlag = false;
		if (ScrollWidget::TestPosition(mousePosition) == widgetPart)
		{
			switch (widgetPart)
			{
				case kWidgetPartUpButton:

					advanceFlag = upButtonHilite;
					break;

				case kWidgetPartDownButton:

					advanceFlag = downButtonHilite;
					break;

				case kWidgetPartPageUp:

					advanceFlag = pageUpHilite;
					break;

				case kWidgetPartPageDown:

					advanceFlag = pageDownHilite;
					break;
			}
		}
		else if (pageUpHilite | pageDownHilite)
		{
			pageUpHilite = false;
			pageDownHilite = false;
			SetBuildFlag();
		}

		if (advanceFlag)
		{
			Advance(widgetPart);
		}
	}
}


ListWidget::ListWidget(WidgetType type) :
		RenderableWidget(type, kRenderQuads),
		scrollObserver(this, &ListWidget::HandleScrollEvent),
		listVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		structureVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		borderColorAttribute(kAttributeMutable),
		borderTextureAttribute(&LineWidget::lineTextureHeader, LineWidget::lineTextureImage),
		borderRenderable(kRenderQuads),
		focusColorAttribute(kAttributeMutable),
		focusRenderable(kRenderQuads),
		scrollWidget(Zero2D)
{
	listFlags = 0;
	itemSpacing = 13.0F;
	itemOffset.Set(2.0F, 0.0F);

	colorOverrideFlags = 0;
	backgroundColor.Set(1.0F, 1.0F, 1.0F);
	hiliteColor.Set(0.0F, 0.0F, 0.0F);
	focusColor.Set(0.0F, 0.0F, 0.0F);

	fontName[0] = 0;

	preprocessFlag = false;
	listUpdateFlags = 0;

	listItemCount = 0;
	clickItemIndex = -1;

	displayItemIndex = 0;
	selectionVertex = nullptr;

	SetActiveUpdateFlags(GetActiveUpdateFlags() | kUpdateStructure);
	SetWidgetUsage(kWidgetKeyboardFocus | kWidgetMouseWheel);

	SetDefaultColorType(kWidgetColorBorder);
	RenderableWidget::SetWidgetColor(K::black);
}

ListWidget::ListWidget(const Vector2D& size, float spacing, const char *font) : ListWidget(kWidgetList, size, spacing, font)
{
}

ListWidget::ListWidget(WidgetType type, const Vector2D& size, float spacing, const char *font) :
		RenderableWidget(kWidgetList, kRenderQuads, size),
		scrollObserver(this, &ListWidget::HandleScrollEvent),
		listVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		structureVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		borderColorAttribute(kAttributeMutable),
		borderTextureAttribute(&LineWidget::lineTextureHeader, LineWidget::lineTextureImage),
		borderRenderable(kRenderQuads),
		focusColorAttribute(kAttributeMutable),
		focusRenderable(kRenderQuads),
		scrollWidget(Vector2D(16.0F, size.y))
{
	listFlags = 0;
	itemSpacing = spacing;
	itemOffset.Set(2.0F, 0.0F);

	colorOverrideFlags = 0;
	backgroundColor.Set(1.0F, 1.0F, 1.0F);
	hiliteColor.Set(0.0F, 0.0F, 0.0F);
	focusColor.Set(0.0F, 0.0F, 0.0F);

	fontName = font;

	preprocessFlag = false;
	listUpdateFlags = 0;

	listItemCount = 0;
	clickItemIndex = -1;

	displayItemIndex = 0;
	selectionVertex = nullptr;

	SetWidgetUsage(kWidgetKeyboardFocus | kWidgetMouseWheel);
	SetActiveUpdateFlags(GetActiveUpdateFlags() | kUpdateStructure);

	SetDefaultColorType(kWidgetColorBorder);
	RenderableWidget::SetWidgetColor(K::black);
}

ListWidget::ListWidget(const ListWidget& listWidget) :
		RenderableWidget(listWidget),
		scrollObserver(this, &ListWidget::HandleScrollEvent),
		listVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		structureVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		borderColorAttribute(kAttributeMutable),
		borderTextureAttribute(&LineWidget::lineTextureHeader, LineWidget::lineTextureImage),
		borderRenderable(kRenderQuads),
		focusColorAttribute(kAttributeMutable),
		focusRenderable(kRenderQuads),
		scrollWidget(listWidget.scrollWidget.GetWidgetSize())
{
	listFlags = listWidget.listFlags;
	itemSpacing = listWidget.itemSpacing;
	itemOffset = listWidget.itemOffset;

	colorOverrideFlags = listWidget.colorOverrideFlags;
	backgroundColor = listWidget.backgroundColor;
	hiliteColor = listWidget.hiliteColor;
	focusColor = listWidget.focusColor;

	fontName = listWidget.fontName;

	preprocessFlag = false;
	listUpdateFlags = 0;

	listItemCount = 0;
	clickItemIndex = -1;

	displayItemIndex = 0;
	selectionVertex = nullptr;

	SetActiveUpdateFlags(GetActiveUpdateFlags() | kUpdateStructure);
}

ListWidget::~ListWidget()
{
	delete[] selectionVertex;
}

Widget *ListWidget::Replicate(void) const
{
	return (new ListWidget(*this));
}

void ListWidget::Pack(Packer& data, unsigned_int32 packFlags) const
{
	RenderableWidget::Pack(data, packFlags);

	data << ChunkHeader('FLAG', 4);
	data << listFlags;

	data << ChunkHeader('SPAC', 4);
	data << itemSpacing;

	data << ChunkHeader('OFST', sizeof(Vector2D));
	data << itemOffset;

	data << ChunkHeader('OVER', 4);
	data << colorOverrideFlags;

	data << ChunkHeader('BGDC', sizeof(ColorRGBA));
	data << backgroundColor;

	if (colorOverrideFlags & kWidgetOverrideHiliteColor)
	{
		data << ChunkHeader('HILC', sizeof(ColorRGBA));
		data << hiliteColor;
	}

	if (colorOverrideFlags & kWidgetOverrideFocusColor)
	{
		data << ChunkHeader('FOCC', sizeof(ColorRGBA));
		data << focusColor;
	}

	PackHandle handle = data.BeginChunk('FONT');
	data << fontName;
	data.EndChunk(handle);

	data << TerminatorChunk;
}

void ListWidget::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	RenderableWidget::Unpack(data, unpackFlags);
	UnpackChunkList<ListWidget>(data, unpackFlags);
}

bool ListWidget::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> listFlags;
			return (true);

		case 'SPAC':

			data >> itemSpacing;
			return (true);

		case 'OFST':

			data >> itemOffset;
			return (true);

		case 'OVER':

			data >> colorOverrideFlags;
			return (true);

		case 'BGDC':

			data >> backgroundColor;
			return (true);

		case 'HILC':

			data >> hiliteColor;
			return (true);

		case 'FOCC':

			data >> focusColor;
			return (true);

		case 'FONT':

			data >> fontName;
			return (true);
	}

	return (false);
}

void *ListWidget::BeginSettingsUnpack(void)
{
	colorOverrideFlags = 0;
	return (RenderableWidget::BeginSettingsUnpack());
}

int32 ListWidget::GetSettingCount(void) const
{
	return (RenderableWidget::GetSettingCount() + 11);
}

Setting *ListWidget::GetSetting(int32 index) const
{
	int32 count = RenderableWidget::GetSettingCount();
	if (index < count)
	{
		return (RenderableWidget::GetSetting(index));
	}

	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetList, 'SETT'));
		return (new HeadingSetting(kWidgetList, title));
	}

	if (index == count + 1)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetList, 'SPAC'));
		return (new TextSetting('LISP', itemSpacing, title));
	}

	if (index == count + 2)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetList, 'OFFX'));
		return (new TextSetting('LOFX', itemOffset.x, title));
	}

	if (index == count + 3)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetList, 'OFFY'));
		return (new TextSetting('LOFY', itemOffset.y, title));
	}

	if (index == count + 4)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetList, 'MULT'));
		return (new BooleanSetting('LMSL', ((listFlags & kListMultipleSelection) != 0), title));
	}

	if (index == count + 5)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetList, 'FPLN'));
		return (new BooleanSetting('LFPL', ((listFlags & kListFocusPlain) != 0), title));
	}

	if (index == count + 6)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetList, 'ITEN'));
		return (new BooleanSetting('LIEN', ((listFlags & kListItemsEnabled) != 0), title));
	}

	if (index == count + 7)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetList, 'BGDC'));
		const char *picker = table->GetString(StringID('WDGT', kWidgetList, 'BGDP'));
		return (new ColorSetting('LIBG', backgroundColor, title, picker, kColorPickerAlpha));
	}

	if (index == count + 8)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetList, 'HILC'));
		const char *picker = table->GetString(StringID('WDGT', kWidgetList, 'HILP'));
		return (new CheckColorSetting('LIHC', ((colorOverrideFlags & kWidgetOverrideHiliteColor) != 0), hiliteColor, title, picker, kColorPickerAlpha));
	}

	if (index == count + 9)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetList, 'FOCC'));
		const char *picker = table->GetString(StringID('WDGT', kWidgetList, 'FOCP'));
		return (new CheckColorSetting('LIFC', ((colorOverrideFlags & kWidgetOverrideFocusColor) != 0), focusColor, title, picker, kColorPickerAlpha));
	}

	if (index == count + 10)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetList, 'FONT'));
		const char *picker = table->GetString(StringID('WDGT', kWidgetList, 'PICK'));
		return (new ResourceSetting('LFNT', fontName, title, picker, FontResource::GetDescriptor()));
	}

	return (nullptr);
}

void ListWidget::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'LISP')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		itemSpacing = Fmax(Text::StringToFloat(text), 1.0F);
	}
	else if (identifier == 'LOFX')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		itemOffset.x = Text::StringToFloat(text);
	}
	else if (identifier == 'LOFY')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		itemOffset.y = Text::StringToFloat(text);
	}
	else if (identifier == 'LMSL')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			listFlags |= kListMultipleSelection;
		}
		else
		{
			listFlags &= ~kListMultipleSelection;
		}
	}
	else if (identifier == 'LFPL')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			listFlags |= kListFocusPlain;
		}
		else
		{
			listFlags &= ~kListFocusPlain;
		}
	}
	else if (identifier == 'LIEN')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			listFlags |= kListItemsEnabled;
		}
		else
		{
			listFlags &= ~kListItemsEnabled;
		}
	}
	else if (identifier == 'LIBG')
	{
		backgroundColor = static_cast<const ColorSetting *>(setting)->GetColor();
	}
	else if (identifier == 'LIHC')
	{
		const CheckColorSetting *checkColorSetting = static_cast<const CheckColorSetting *>(setting);
		if (checkColorSetting->GetCheckValue() != 0)
		{
			colorOverrideFlags |= kWidgetOverrideHiliteColor;
			hiliteColor = checkColorSetting->GetColor();
		}
		else
		{
			colorOverrideFlags &= ~kWidgetOverrideHiliteColor;
			SetDefaultHiliteColor();
		}
	}
	else if (identifier == 'LIFC')
	{
		const CheckColorSetting *checkColorSetting = static_cast<const CheckColorSetting *>(setting);
		if (checkColorSetting->GetCheckValue() != 0)
		{
			colorOverrideFlags |= kWidgetOverrideFocusColor;
			focusColor = checkColorSetting->GetColor();
		}
		else
		{
			colorOverrideFlags &= ~kWidgetOverrideFocusColor;
			SetDefaultFocusColor();
		}
	}
	else if (identifier == 'LFNT')
	{
		fontName = static_cast<const ResourceSetting *>(setting)->GetResourceName();
	}
	else
	{
		RenderableWidget::SetSetting(setting);
	}
}

const ColorRGBA& ListWidget::GetWidgetColor(WidgetColorType type) const
{
	if (type == kWidgetColorBackground)
	{
		return (backgroundColor);
	}

	if (type == kWidgetColorHilite)
	{
		return (hiliteColor);
	}

	if (type == kWidgetColorFocus)
	{
		return (focusColor);
	}

	return (RenderableWidget::GetWidgetColor(type));
}

void ListWidget::SetWidgetColor(const ColorRGBA& color, WidgetColorType type)
{
	if (type == kWidgetColorBackground)
	{
		backgroundColor = color;
	}
	else if (type == kWidgetColorHilite)
	{
		hiliteColor = color;
		colorOverrideFlags |= kWidgetOverrideHiliteColor;
	}
	else if (type == kWidgetColorFocus)
	{
		focusColor = color;
		colorOverrideFlags |= kWidgetOverrideFocusColor;
	}

	RenderableWidget::SetWidgetColor(color, type);
}

void ListWidget::SetWidgetAlpha(float alpha, WidgetColorType type)
{
	if (type == kWidgetColorBackground)
	{
		backgroundColor.alpha = alpha;
	}
	else if (type == kWidgetColorHilite)
	{
		hiliteColor.alpha = alpha;
		colorOverrideFlags |= kWidgetOverrideHiliteColor;
	}
	else if (type == kWidgetColorFocus)
	{
		focusColor.alpha = alpha;
		colorOverrideFlags |= kWidgetOverrideFocusColor;
	}

	RenderableWidget::SetWidgetAlpha(alpha, type);
}

void ListWidget::SetDynamicWidgetColor(const ColorRGBA& color, WidgetColorType type)
{
	if ((type == kWidgetColorDefault) || (type == kWidgetColorBorder))
	{
		borderColorAttribute.SetDiffuseColor(color);
	}
	else if (type == kWidgetColorBackground)
	{
		dynamicBackgroundColor = color;
		SetBuildFlag();
	}
	else if (type == kWidgetColorHilite)
	{
		dynamicHiliteColor = color;
		SetBuildFlag();
	}
	else if (type == kWidgetColorFocus)
	{
		focusColorAttribute.SetDiffuseColor(color);
	}
	else
	{
		RenderableWidget::SetDynamicWidgetColor(color, type);
	}
}

void ListWidget::SetDynamicWidgetAlpha(float alpha, WidgetColorType type)
{
	if ((type == kWidgetColorDefault) || (type == kWidgetColorBorder))
	{
		borderColorAttribute.SetDiffuseAlpha(alpha);
	}
	else if (type == kWidgetColorBackground)
	{
		dynamicBackgroundColor.alpha = alpha;
		SetBuildFlag();
	}
	else if (type == kWidgetColorHilite)
	{
		dynamicHiliteColor.alpha = alpha;
		SetBuildFlag();
	}
	else if (type == kWidgetColorFocus)
	{
		focusColorAttribute.SetDiffuseAlpha(alpha);
	}
	else
	{
		RenderableWidget::SetDynamicWidgetAlpha(alpha, type);
	}
}

void ListWidget::SetDefaultHiliteColor(void)
{
	hiliteColor = TheInterfaceMgr->GetInterfaceColor(kInterfaceColorHilite);
}

void ListWidget::SetDefaultFocusColor(void)
{
	focusColor.Set(TheInterfaceMgr->GetInterfaceColor(kInterfaceColorHilite).GetColorRGB() * 0.625F, 1.0F);
}

void ListWidget::SetWidgetSize(const Vector2D& size)
{
	RenderableWidget::SetWidgetSize(size);

	scrollWidget.SetWidgetSize(Vector2D(16.0F, size.y));
	scrollWidget.SetWidgetPosition(Point3D(FmaxZero(size.x - 16.0F), 0.0F, 0.0F));
	scrollWidget.Invalidate();

	SetListUpdateFlags(kListUpdateVisibility);
}

WidgetPart ListWidget::TestPosition(const Point3D& position) const
{
	if (position.x < GetWidgetSize().x - 16.0F)
	{
		return (kWidgetPartInterior);
	}

	return (kWidgetPartNone);
}

void ListWidget::Preprocess(void)
{
	if (!(colorOverrideFlags & kWidgetOverrideHiliteColor))
	{
		SetDefaultHiliteColor();
	}

	if (!(colorOverrideFlags & kWidgetOverrideFocusColor))
	{
		SetDefaultFocusColor();
	}

	dynamicBackgroundColor = backgroundColor;
	dynamicHiliteColor = hiliteColor;

	itemGroup.SetWidgetState(itemGroup.GetWidgetState() | kWidgetNonpersistent);
	AppendSubnode(&itemGroup);

	scrollWidget.SetWidgetSize(Vector2D(16.0F, GetWidgetSize().y));
	scrollWidget.SetWidgetPosition(Point3D(FmaxZero(GetWidgetSize().x - 16.0F), 0.0F, 0.0F));
	scrollWidget.SetWidgetState(scrollWidget.GetWidgetState() | kWidgetNonpersistent);
	scrollWidget.SetObserver(&scrollObserver);
	AppendSubnode(&scrollWidget);

	RenderableWidget::Preprocess();

	SetVertexBuffer(kVertexBufferAttributeArray, &listVertexBuffer, sizeof(ListVertex));
	SetVertexAttributeArray(kArrayPosition, 0, 2);
	SetVertexAttributeArray(kArrayColor, sizeof(Point2D), 4);

	structureVertexBuffer.Establish(sizeof(BorderVertex) * 16 + sizeof(GlowVertex) * 32);

	InitRenderable(&borderRenderable);
	borderRenderable.SetVertexCount(16);
	borderRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &structureVertexBuffer, sizeof(BorderVertex));
	borderRenderable.SetVertexAttributeArray(kArrayPosition, 0, 2);
	borderRenderable.SetVertexAttributeArray(kArrayTexcoord, sizeof(Point2D), 2);

	borderAttributeList.Append(&borderColorAttribute);
	borderAttributeList.Append(&borderTextureAttribute);
	borderRenderable.SetMaterialAttributeList(&borderAttributeList);

	InitRenderable(&focusRenderable);
	focusRenderable.SetVertexCount(32);
	focusRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &structureVertexBuffer, sizeof(GlowVertex));
	focusRenderable.SetVertexAttributeArray(kArrayPosition, sizeof(BorderVertex) * 16, 2);
	focusRenderable.SetVertexAttributeArray(kArrayTexcoord, sizeof(BorderVertex) * 16 + sizeof(Point2D), 2);

	focusAttributeList.Append(&focusColorAttribute);
	focusRenderable.SetMaterialAttributeList(&focusAttributeList);
	focusRenderable.SetMaterialObjectPointer(TheInterfaceMgr->GetMaterialObjectPointer());

	preprocessFlag = true;
	listUpdateFlags = kListUpdatePlacement | kListUpdateVisibility | kListUpdateSelection;
}

void ListWidget::EnterForeground(void)
{
	RenderableWidget::EnterForeground();
	SetWidgetState(GetWidgetState() & ~kWidgetBackground);
}

void ListWidget::EnterBackground(void)
{
	RenderableWidget::EnterBackground();
	SetWidgetState(GetWidgetState() | kWidgetBackground);
}

void ListWidget::UpdatePlacement(void)
{
	listUpdateFlags &= ~kListUpdatePlacement;

	float y = itemOffset.y;
	Widget *widget = itemGroup.GetFirstSubnode();
	while (widget)
	{
		widget->SetWidgetPosition(Point3D(itemOffset.x, y, 0.0F));

		y += itemSpacing;
		widget = widget->Next();
	}

	UpdateVisibility();
}

void ListWidget::UpdateVisibility(void)
{
	listUpdateFlags &= ~kListUpdateVisibility;

	displayItemCount = (int32) (GetWidgetSize().y / itemSpacing);
	scrollWidget.SetPageDistance(Max(displayItemCount - 1, 1));

	int32 maxIndex = MaxZero(listItemCount - displayItemCount);
	scrollWidget.SetMaxValue(maxIndex);

	displayItemIndex = Min(displayItemIndex, maxIndex);
	scrollWidget.SetValue(displayItemIndex);

	displayItem = nullptr;

	delete[] selectionVertex;
	int32 vertexCount = displayItemCount * 4;
	selectionVertex = new Point2D[vertexCount];
	listVertexBuffer.Establish(sizeof(ListVertex) * (vertexCount + 4));
	InvalidateVertexData();

	machine index = 0;
	Widget *widget = itemGroup.GetFirstSubnode();
	while (widget)
	{
		if ((unsigned_int32) (index - displayItemIndex) < (unsigned_int32) displayItemCount)
		{
			widget->Show();
			if (index == displayItemIndex)
			{
				displayItem = widget;
			}
		}
		else
		{
			widget->Hide();
		}

		index++;
		widget = widget->Next();
	}

	itemGroup.SetWidgetPosition(Point3D(0.0F, -itemSpacing * (float) displayItemIndex, 0.0F));
	itemGroup.Invalidate();

	UpdateSelection();
}

void ListWidget::UpdateSelection(void)
{
	listUpdateFlags &= ~kListUpdateSelection;

	int32 selectCount = 0;
	const Widget *widget = displayItem;

	for (machine a = 0; a < displayItemCount; a++)
	{
		if (!widget)
		{
			break;
		}

		if (widget->GetWidgetState() & kWidgetSelected)
		{
			float y = (float) a * itemSpacing;
			float w = GetWidgetSize().x;

			Point2D *vertex = &selectionVertex[selectCount * 4];
			vertex[0].Set(0.0F, y);
			vertex[1].Set(0.0F, y + itemSpacing);
			vertex[2].Set(w, y + itemSpacing);
			vertex[3].Set(w, y);

			selectCount++;
		}

		widget = widget->Next();
	}

	SetVertexCount(selectCount * 4 + 4);
	SetBuildFlag();
}

void ListWidget::HandleStructureUpdate(void)
{
	if (listUpdateFlags != 0)
	{
		if (listUpdateFlags & kListUpdatePlacement)
		{
			UpdatePlacement();
		}
		else if (listUpdateFlags & kListUpdateVisibility)
		{
			UpdateVisibility();
		}
		else if (listUpdateFlags & kListUpdateSelection)
		{
			UpdateSelection();
		}
	}
}

void ListWidget::Build(void)
{
	borderColorAttribute.SetDiffuseColor(RenderableWidget::GetWidgetColor());
	focusColorAttribute.SetDiffuseColor(focusColor);

	volatile ListVertex *restrict listVertex = listVertexBuffer.BeginUpdate<ListVertex>();

	float w = GetWidgetSize().x;
	float h = GetWidgetSize().y;

	listVertex[0].position.Set(0.0F, 0.0F);
	listVertex[1].position.Set(0.0F, h);
	listVertex[2].position.Set(w - 16.0F, h);
	listVertex[3].position.Set(w - 16.0F, 0.0F);

	for (machine a = 0; a < 4; a++)
	{
		listVertex[a].color = dynamicBackgroundColor;
	}

	float alpha = (GetWidgetState() & kWidgetBackground) ? 0.5F : 1.0F;
	ColorRGBA color(dynamicHiliteColor.GetColorRGB(), dynamicHiliteColor.alpha * alpha);

	int32 count = GetVertexCount();
	for (machine a = 4; a < count; a++)
	{
		listVertex[a].position = selectionVertex[a - 4];
		listVertex[a].color = color;
	}

	listVertexBuffer.EndUpdate();

	Box2D box(Zero2D, Point2D(Fmax(w, 16.0F), h));

	volatile BorderVertex *restrict borderVertex = structureVertexBuffer.BeginUpdate<BorderVertex>();
	BuildBorder(box, borderVertex);

	volatile GlowVertex *restrict focusVertex = reinterpret_cast<volatile GlowVertex *>(borderVertex + 16);
	BuildGlow(box, focusVertex);

	structureVertexBuffer.EndUpdate();
}

void ListWidget::Render(List<Renderable> *renderList)
{
	RenderableWidget::Render(renderList);

	renderList->Append(&borderRenderable);

	if ((GetWidgetState() & kWidgetFocus) && (!(listFlags & kListFocusPlain)))
	{
		renderList->Append(&focusRenderable);
	}
}

void ListWidget::HandleScrollEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		displayItemIndex = static_cast<ScrollWidget *>(widget)->GetValue();
		SetListUpdateFlags(kListUpdateVisibility);

		if (GetWidgetUsage() & kWidgetKeyboardFocus)
		{
			RootWidget *root = GetRootWidget();
			if (root)
			{
				root->SetFocusWidget(this);
			}
		}
	}
}

void ListWidget::HandleMouseEvent(const PanelMouseEventData *eventData)
{
	EventType eventType = eventData->eventType;
	if (eventType == kEventMouseDown)
	{
		PanelController *controller = GetPanelController();
		if (controller)
		{
			controller->BeginKeyboardInteraction(this);
		}

		int32 index = (int32) (eventData->mousePosition.y / itemSpacing) + displayItemIndex;
		if ((index < displayItemIndex + displayItemCount) && (index < listItemCount))
		{
			bool activate = ((eventData->eventFlags & kMouseDoubleClick) && (index == clickItemIndex));
			clickItemIndex = index;

			Widget *selection = GetListItem(index);
			if ((selection) && (!(selection->GetWidgetState() & kWidgetInactive)))
			{
				if (!(listFlags & kListMultipleSelection))
				{
					unsigned_int32 state = selection->GetWidgetState();
					if (!(state & kWidgetSelected))
					{
						Widget *widget = itemGroup.GetFirstSubnode();
						while (widget)
						{
							if (widget != selection)
							{
								widget->SetWidgetState(widget->GetWidgetState() & ~kWidgetSelected);
							}
							else
							{
								widget->SetWidgetState(state | kWidgetSelected);
							}

							widget = widget->Next();
						}

						SetListUpdateFlags(kListUpdateSelection);
						PostWidgetEvent(WidgetEventData(kEventWidgetChange));
					}
					else
					{
						if (activate)
						{
							Activate(eventData->initiatorNode);
						}
					}
				}
				else
				{
					bool post = false;

					unsigned_int32 modifiers = InterfaceMgr::GetModifierKeys();
					if ((modifiers & (kModifierKeyShift | kModifierKeyCommand)) == 0)
					{
						Widget *widget = itemGroup.GetFirstSubnode();
						while (widget)
						{
							unsigned_int32 state = widget->GetWidgetState();

							if (widget != selection)
							{
								if (state & kWidgetSelected)
								{
									widget->SetWidgetState(state & ~kWidgetSelected);
									post = true;
								}
							}
							else
							{
								if (!(state & kWidgetSelected))
								{
									widget->SetWidgetState(state | kWidgetSelected);
									post = true;
								}
							}

							widget = widget->Next();
						}
					}
					else if (modifiers & kModifierKeyShift)
					{
						selection->SetWidgetState(selection->GetWidgetState() | kWidgetSelected);

						Widget *widget = GetFirstSelectedListItem();
						const Widget *last = GetLastSelectedListItem();
						while (widget != last)
						{
							widget = widget->Next();
							widget->SetWidgetState(widget->GetWidgetState() | kWidgetSelected);
						}

						post = true;
					}
					else
					{
						selection->SetWidgetState(selection->GetWidgetState() ^ kWidgetSelected);
						post = true;
					}

					if (post)
					{
						SetListUpdateFlags(kListUpdateSelection);
						PostWidgetEvent(WidgetEventData(kEventWidgetChange));
					}
					else if (activate)
					{
						Activate(eventData->initiatorNode);
					}
				}
			}
		}
	}
	else if (eventType == kEventMouseWheel)
	{
		int32 maxIndex = listItemCount - displayItemCount;
		if (maxIndex > 0)
		{
			int32 index = MaxZero(Min(displayItemIndex - (int32) eventData->wheelDelta.y, maxIndex));
			if (index != displayItemIndex)
			{
				displayItemIndex = index;
				SetListUpdateFlags(kListUpdateVisibility);
			}
		}
	}
}

void ListWidget::HandleUpArrow(unsigned_int32 modifierKeys)
{
	int32 index = 0;
	bool post = false;

	Widget *widget = itemGroup.GetFirstSubnode();
	while (widget)
	{
		if (widget->GetWidgetState() & kWidgetSelected)
		{
			break;
		}

		widget = widget->Next();
	}

	if (widget)
	{
		Widget *previous = GetPreviousActiveListItem(widget);
		if (previous)
		{
			previous->SetWidgetState(previous->GetWidgetState() | kWidgetSelected);
			index = previous->GetNodeIndex();
			post = true;
		}
		else
		{
			widget = widget->Next();
		}

		if ((!(listFlags & kListMultipleSelection)) || (!(modifierKeys & kModifierKeyShift)))
		{
			while (widget)
			{
				unsigned_int32 state = widget->GetWidgetState();
				if (state & kWidgetSelected)
				{
					widget->SetWidgetState(state & ~kWidgetSelected);
					post = true;
				}

				widget = widget->Next();
			}
		}
	}
	else
	{
		widget = GetFirstActiveListItem();
		if (widget)
		{
			widget->SetWidgetState(widget->GetWidgetState() | kWidgetSelected);
			index = widget->GetNodeIndex();
			post = true;
		}
	}

	if (post)
	{
		SetListUpdateFlags(kListUpdateSelection);
		RevealListItem(index);
		PostWidgetEvent(WidgetEventData(kEventWidgetChange));
	}
}

void ListWidget::HandleDownArrow(unsigned_int32 modifierKeys)
{
	int32 index = 0;
	bool post = false;

	Widget *widget = itemGroup.GetLastSubnode();
	while (widget)
	{
		if (widget->GetWidgetState() & kWidgetSelected)
		{
			break;
		}

		widget = widget->Previous();
	}

	if (widget)
	{
		Widget *next = GetNextActiveListItem(widget);
		if (next)
		{
			next->SetWidgetState(next->GetWidgetState() | kWidgetSelected);
			index = next->GetNodeIndex();
			post = true;
		}
		else
		{
			widget = widget->Previous();
		}

		if ((!(listFlags & kListMultipleSelection)) || (!(modifierKeys & kModifierKeyShift)))
		{
			while (widget)
			{
				unsigned_int32 state = widget->GetWidgetState();
				if (state & kWidgetSelected)
				{
					widget->SetWidgetState(state & ~kWidgetSelected);
					post = true;
				}

				widget = widget->Previous();
			}
		}
	}
	else
	{
		widget = GetLastActiveListItem();
		if (widget)
		{
			widget->SetWidgetState(widget->GetWidgetState() | kWidgetSelected);
			index = widget->GetNodeIndex();
			post = true;
		}
	}

	if (post)
	{
		SetListUpdateFlags(kListUpdateSelection);
		RevealListItem(index);
		PostWidgetEvent(WidgetEventData(kEventWidgetChange));
	}
}

bool ListWidget::HandleKeyboardEvent(const KeyboardEventData *eventData)
{
	EventType eventType = eventData->eventType;
	if (eventType == kEventKeyDown)
	{
		unsigned_int32 keyCode = eventData->keyCode;

		if (keyCode == kKeyCodeUpArrow)
		{
			HandleUpArrow(eventData->modifierKeys);
			return (true);
		}
		else if (keyCode == kKeyCodeDownArrow)
		{
			HandleDownArrow(eventData->modifierKeys);
			return (true);
		}
		else if (keyCode == kKeyCodePageUp)
		{
			int32 index = MaxZero(displayItemIndex - displayItemCount + 1);

			Widget *widget = GetListItem(Min(index, listItemCount - 1));
			if (widget)
			{
				int32 maxIndex = listItemCount - displayItemCount;
				if (maxIndex > 0)
				{
					int32 item = Min(index, maxIndex);
					if (item != displayItemIndex)
					{
						displayItemIndex = item;
						SetListUpdateFlags(kListUpdateVisibility);
					}
				}

				if (GetSelectedListItemCount() <= 1)
				{
					unsigned_int32 state = widget->GetWidgetState();
					if (!(state & (kWidgetSelected | kWidgetInactive)))
					{
						UnselectAllListItems();
						widget->SetWidgetState(state | kWidgetSelected);
						PostWidgetEvent(WidgetEventData(kEventWidgetChange));
					}
				}
			}

			return (true);
		}
		else if (keyCode == kKeyCodePageDown)
		{
			int32 index = MaxZero(displayItemIndex + displayItemCount - 1);

			Widget *widget = GetListItem(Min(index, listItemCount - 1));
			if (widget)
			{
				int32 maxIndex = listItemCount - displayItemCount;
				if (maxIndex > 0)
				{
					int32 item = Min(index, maxIndex);
					if (item != displayItemIndex)
					{
						displayItemIndex = item;
						SetListUpdateFlags(kListUpdateVisibility);
					}
				}

				if (GetSelectedListItemCount() <= 1)
				{
					unsigned_int32 state = widget->GetWidgetState();
					if (!(state & (kWidgetSelected | kWidgetInactive)))
					{
						UnselectAllListItems();
						widget->SetWidgetState(state | kWidgetSelected);
						PostWidgetEvent(WidgetEventData(kEventWidgetChange));
					}
				}
			}

			return (true);
		}
		else if (keyCode == kKeyCodeHome)
		{
			Widget *widget = GetFirstListItem();
			if (widget)
			{
				if (displayItemIndex != 0)
				{
					displayItemIndex = 0;
					SetListUpdateFlags(kListUpdateVisibility);
				}

				if (GetSelectedListItemCount() <= 1)
				{
					unsigned_int32 state = widget->GetWidgetState();
					if (!(state & (kWidgetSelected | kWidgetInactive)))
					{
						UnselectAllListItems();
						widget->SetWidgetState(state | kWidgetSelected);
						PostWidgetEvent(WidgetEventData(kEventWidgetChange));
					}
				}
			}

			return (true);
		}
		else if (keyCode == kKeyCodeEnd)
		{
			Widget *widget = GetLastListItem();
			if (widget)
			{
				int32 maxIndex = listItemCount - displayItemCount;
				if ((maxIndex > 0) && (displayItemIndex != maxIndex))
				{
					displayItemIndex = maxIndex;
					SetListUpdateFlags(kListUpdateVisibility);
				}

				if (GetSelectedListItemCount() <= 1)
				{
					unsigned_int32 state = widget->GetWidgetState();
					if (!(state & (kWidgetSelected | kWidgetInactive)))
					{
						UnselectAllListItems();
						widget->SetWidgetState(state | kWidgetSelected);
						PostWidgetEvent(WidgetEventData(kEventWidgetChange));
					}
				}
			}

			return (true);
		}
	}
	else if (eventType == kEventKeyCommand)
	{
		if (listFlags & kListMultipleSelection)
		{
			if (eventData->keyCode == 'A')
			{
				SelectAllListItems(true);
				return (true);
			}
		}
	}

	return (false);
}

Widget *ListWidget::GetListItem(int32 index) const
{
	Widget *widget = itemGroup.GetFirstSubnode();
	while (widget)
	{
		if (index == 0)
		{
			return (widget);
		}

		index--;
		widget = widget->Next();
	}

	return (nullptr);
}

void ListWidget::RevealListItem(int32 index)
{
	if ((unsigned_int32) (index - displayItemIndex) >= (unsigned_int32) displayItemCount)
	{
		displayItemIndex = MaxZero(Min(index - displayItemCount / 2, listItemCount - displayItemCount));
		SetListUpdateFlags(kListUpdateVisibility);
	}
}

TextWidget *ListWidget::FindListItem(const char *text, Text::TextComparisonFunction *comparisonFunction) const
{
	Widget *listItem = itemGroup.GetFirstSubnode();
	while (listItem)
	{
		if (listItem->GetWidgetType() == kWidgetText)
		{
			TextWidget *textWidget = static_cast<TextWidget *>(listItem);
			if ((*comparisonFunction)(text, textWidget->GetText()))
			{
				return (textWidget);
			}
		}

		listItem = listItem->Next();
	}

	return (nullptr);
}

int32 ListWidget::GetActiveListItemCount(void) const
{
	int32 count = 0;

	Widget *widget = itemGroup.GetFirstSubnode();
	while (widget)
	{
		if (!(widget->GetWidgetState() & kWidgetInactive))
		{
			count++;
		}

		widget = widget->Next();
	}

	return (count);
}

Widget *ListWidget::GetActiveListItem(int32 index) const
{
	Widget *widget = itemGroup.GetFirstSubnode();
	while (widget)
	{
		if (!(widget->GetWidgetState() & kWidgetInactive))
		{
			if (index == 0)
			{
				return (widget);
			}

			index--;
		}

		widget = widget->Next();
	}

	return (nullptr);
}

Widget *ListWidget::GetFirstActiveListItem(void) const
{
	Widget *widget = itemGroup.GetFirstSubnode();
	while (widget)
	{
		if (!(widget->GetWidgetState() & kWidgetInactive))
		{
			return (widget);
		}

		widget = widget->Next();
	}

	return (nullptr);
}

Widget *ListWidget::GetLastActiveListItem(void) const
{
	Widget *widget = itemGroup.GetLastSubnode();
	while (widget)
	{
		if (!(widget->GetWidgetState() & kWidgetInactive))
		{
			return (widget);
		}

		widget = widget->Previous();
	}

	return (nullptr);
}

Widget *ListWidget::GetPreviousActiveListItem(const Widget *widget) const
{
	Widget *previous = widget->Previous();
	while (previous)
	{
		if (!(previous->GetWidgetState() & kWidgetInactive))
		{
			return (previous);
		}

		previous = previous->Previous();
	}

	return (nullptr);
}

Widget *ListWidget::GetNextActiveListItem(const Widget *widget) const
{
	Widget *next = widget->Next();
	while (next)
	{
		if (!(next->GetWidgetState() & kWidgetInactive))
		{
			return (next);
		}

		next = next->Next();
	}

	return (nullptr);
}

void ListWidget::PrependListItem(Widget *widget)
{
	itemGroup.PrependSubnode(widget);
	listItemCount++;

	if (!(listFlags & kListItemsEnabled))
	{
		widget->Disable();
	}

	if (preprocessFlag)
	{
		widget->Preprocess();
	}

	clickItemIndex = -1;
	SetListUpdateFlags(kListUpdatePlacement);
}

void ListWidget::AppendListItem(Widget *widget)
{
	itemGroup.AppendSubnode(widget);
	listItemCount++;

	if (!(listFlags & kListItemsEnabled))
	{
		widget->Disable();
	}

	if (preprocessFlag)
	{
		widget->Preprocess();
	}

	clickItemIndex = -1;
	SetListUpdateFlags(kListUpdatePlacement);
}

void ListWidget::InsertListItemBefore(Widget *widget, Widget *before)
{
	itemGroup.InsertSubnodeBefore(widget, before);
	listItemCount++;

	if (!(listFlags & kListItemsEnabled))
	{
		widget->Disable();
	}

	if (preprocessFlag)
	{
		widget->Preprocess();
	}

	clickItemIndex = -1;
	SetListUpdateFlags(kListUpdatePlacement);
}

void ListWidget::InsertListItemAfter(Widget *widget, Widget *after)
{
	itemGroup.InsertSubnodeAfter(widget, after);
	listItemCount++;

	if (!(listFlags & kListItemsEnabled))
	{
		widget->Disable();
	}

	if (preprocessFlag)
	{
		widget->Preprocess();
	}

	clickItemIndex = -1;
	SetListUpdateFlags(kListUpdatePlacement);
}

void ListWidget::InsertSortedListItem(TextWidget *widget, Text::TextComparisonFunction *comparisonFunction)
{
	Widget *listItem = itemGroup.GetFirstSubnode();
	if (listItem)
	{
		const char *text = widget->GetText();
		do
		{
			if ((listItem->GetWidgetType() == kWidgetText) && ((*comparisonFunction)(text, static_cast<TextWidget *>(listItem)->GetText())))
			{
				itemGroup.InsertSubnodeBefore(widget, listItem);
				break;
			}

			listItem = listItem->Next();
		} while (listItem);

		if (!listItem)
		{
			itemGroup.AppendSubnode(widget);
		}
	}
	else
	{
		itemGroup.AppendSubnode(widget);
	}

	if (!(listFlags & kListItemsEnabled))
	{
		widget->Disable();
	}

	if (preprocessFlag)
	{
		widget->Preprocess();
	}

	listItemCount++;
	clickItemIndex = -1;
	SetListUpdateFlags(kListUpdatePlacement);
}

TextWidget *ListWidget::PrependListItem(const char *text)
{
	TextWidget *widget = new TextWidget(GetNaturalListItemSize(), text, fontName);
	PrependListItem(widget);
	return (widget);
}

TextWidget *ListWidget::AppendListItem(const char *text)
{
	TextWidget *widget = new TextWidget(GetNaturalListItemSize(), text, fontName);
	AppendListItem(widget);
	return (widget);
}

TextWidget *ListWidget::InsertListItemBefore(const char *text, Widget *before)
{
	TextWidget *widget = new TextWidget(GetNaturalListItemSize(), text, fontName);
	InsertListItemBefore(widget, before);
	return (widget);
}

TextWidget *ListWidget::InsertListItemAfter(const char *text, Widget *after)
{
	TextWidget *widget = new TextWidget(GetNaturalListItemSize(), text, fontName);
	InsertListItemAfter(widget, after);
	return (widget);
}

TextWidget *ListWidget::InsertSortedListItem(const char *text, Text::TextComparisonFunction *comparisonFunction)
{
	TextWidget *widget = new TextWidget(GetNaturalListItemSize(), text, fontName);
	InsertSortedListItem(widget, comparisonFunction);
	return (widget);
}

bool ListWidget::RemoveListItem(Widget *widget)
{
	bool change = false;

	unsigned_int32 state = widget->GetWidgetState();
	if (state & kWidgetSelected)
	{
		change = true;
		widget->SetWidgetState(state & ~kWidgetSelected);
	}

	itemGroup.RemoveSubnode(widget);
	listItemCount--;

	clickItemIndex = -1;
	SetListUpdateFlags(kListUpdatePlacement);

	return (change);
}

void ListWidget::PurgeListItems(void)
{
	itemGroup.PurgeSubtree();
	listItemCount = 0;

	clickItemIndex = -1;
	SetListUpdateFlags(kListUpdatePlacement);
}

void ListWidget::PurgeSelectedListItems(void)
{
	Widget *widget = GetFirstSelectedListItem();
	while (widget)
	{
		Widget *next = GetNextSelectedListItem(widget);

		delete widget;
		listItemCount--;

		widget = next;
	}

	clickItemIndex = -1;
	SetListUpdateFlags(kListUpdatePlacement);
}

int32 ListWidget::GetSelectedListItemCount(void) const
{
	int32 count = 0;

	Widget *widget = itemGroup.GetFirstSubnode();
	while (widget)
	{
		if (widget->GetWidgetState() & kWidgetSelected)
		{
			count++;
		}

		widget = widget->Next();
	}

	return (count);
}

Widget *ListWidget::GetSelectedListItem(int32 index) const
{
	Widget *widget = itemGroup.GetFirstSubnode();
	while (widget)
	{
		if (widget->GetWidgetState() & kWidgetSelected)
		{
			if (index == 0)
			{
				return (widget);
			}

			index--;
		}

		widget = widget->Next();
	}

	return (nullptr);
}

Widget *ListWidget::GetFirstSelectedListItem(void) const
{
	Widget *widget = itemGroup.GetFirstSubnode();
	while (widget)
	{
		if (widget->GetWidgetState() & kWidgetSelected)
		{
			return (widget);
		}

		widget = widget->Next();
	}

	return (nullptr);
}

Widget *ListWidget::GetLastSelectedListItem(void) const
{
	Widget *widget = itemGroup.GetLastSubnode();
	while (widget)
	{
		if (widget->GetWidgetState() & kWidgetSelected)
		{
			return (widget);
		}

		widget = widget->Previous();
	}

	return (nullptr);
}

Widget *ListWidget::GetPreviousSelectedListItem(const Widget *widget) const
{
	Widget *previous = widget->Previous();
	while (previous)
	{
		if (previous->GetWidgetState() & kWidgetSelected)
		{
			return (previous);
		}

		previous = previous->Previous();
	}

	return (nullptr);
}

Widget *ListWidget::GetNextSelectedListItem(const Widget *widget) const
{
	Widget *next = widget->Next();
	while (next)
	{
		if (next->GetWidgetState() & kWidgetSelected)
		{
			return (next);
		}

		next = next->Next();
	}

	return (nullptr);
}

int32 ListWidget::GetFirstSelectedIndex(void) const
{
	int32 index = 0;
	Widget *widget = itemGroup.GetFirstSubnode();
	while (widget)
	{
		if (widget->GetWidgetState() & kWidgetSelected)
		{
			return (index);
		}

		index++;
		widget = widget->Next();
	}

	return (-1);
}

int32 ListWidget::GetLastSelectedIndex(void) const
{
	int32 index = 0;
	Widget *widget = itemGroup.GetLastSubnode();
	while (widget)
	{
		if (widget->GetWidgetState() & kWidgetSelected)
		{
			return (index);
		}

		index++;
		widget = widget->Previous();
	}

	return (-1);
}

void ListWidget::SelectListItem(int32 index, bool post)
{
	bool change = false;

	if (!(listFlags & kListMultipleSelection))
	{
		Widget *widget = itemGroup.GetFirstSubnode();
		while (widget)
		{
			unsigned_int32 state = widget->GetWidgetState();
			if (index != 0)
			{
				if (state & kWidgetSelected)
				{
					change = true;
					widget->SetWidgetState(state & ~kWidgetSelected);
				}
			}
			else
			{
				if (!(state & kWidgetSelected))
				{
					change = true;
					widget->SetWidgetState(state | kWidgetSelected);
				}
			}

			index--;
			widget = widget->Next();
		}
	}
	else
	{
		Widget *widget = itemGroup.GetFirstSubnode();
		while (widget)
		{
			if (index == 0)
			{
				unsigned_int32 state = widget->GetWidgetState();
				if (!(state & kWidgetSelected))
				{
					change = true;
					widget->SetWidgetState(state | kWidgetSelected);
				}

				break;
			}

			index--;
			widget = widget->Next();
		}
	}

	SetListUpdateFlags(kListUpdateSelection);

	if (post & change)
	{
		PostWidgetEvent(WidgetEventData(kEventWidgetChange));
	}
}

void ListWidget::UnselectListItem(int32 index, bool post)
{
	bool change = false;

	Widget *widget = itemGroup.GetFirstSubnode();
	while (widget)
	{
		if (index == 0)
		{
			unsigned_int32 state = widget->GetWidgetState();
			if (state & kWidgetSelected)
			{
				change = true;
				widget->SetWidgetState(state & ~kWidgetSelected);
			}

			break;
		}

		index--;
		widget = widget->Next();
	}

	SetListUpdateFlags(kListUpdateSelection);

	if (post & change)
	{
		PostWidgetEvent(WidgetEventData(kEventWidgetChange));
	}
}

void ListWidget::SelectAllListItems(bool post)
{
	bool change = false;
	Widget *widget = itemGroup.GetFirstSubnode();

	if (listFlags & kListMultipleSelection)
	{
		while (widget)
		{
			unsigned_int32 state = widget->GetWidgetState();
			if (!(state & kWidgetSelected))
			{
				change = true;
				widget->SetWidgetState(state | kWidgetSelected);
			}

			widget = widget->Next();
		}
	}
	else if (widget)
	{
		unsigned_int32 state = widget->GetWidgetState();
		if (!(state & kWidgetSelected))
		{
			change = true;
			widget->SetWidgetState(state | kWidgetSelected);
		}

		widget = widget->Next();
		while (widget)
		{
			state = widget->GetWidgetState();
			if (state & kWidgetSelected)
			{
				change = true;
				widget->SetWidgetState(state & ~kWidgetSelected);
			}

			widget = widget->Next();
		}
	}

	SetListUpdateFlags(kListUpdateSelection);

	if (post & change)
	{
		PostWidgetEvent(WidgetEventData(kEventWidgetChange));
	}
}

void ListWidget::UnselectAllListItems(bool post)
{
	bool change = false;

	Widget *widget = itemGroup.GetFirstSubnode();
	while (widget)
	{
		unsigned_int32 state = widget->GetWidgetState();
		if (state & kWidgetSelected)
		{
			change = true;
			widget->SetWidgetState(state & ~kWidgetSelected);
		}

		widget = widget->Next();
	}

	SetListUpdateFlags(kListUpdateSelection);

	if (post & change)
	{
		PostWidgetEvent(WidgetEventData(kEventWidgetChange));
	}
}

void ListWidget::SetDisplayIndex(int32 index)
{
	displayItemIndex = MaxZero(Min(index, listItemCount - displayItemCount));
	SetListUpdateFlags(kListUpdateVisibility);
}


TreeItemWidget::TreeItemWidget(Widget *widget) :
		RenderableWidget(kWidgetTreeItem, kRenderQuads),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		colorAttribute(ColorRGBA(0.0F, 0.0F, 0.0F, 1.0F), kAttributeMutable),
		textureAttribute(&LineWidget::lineTextureHeader, LineWidget::lineTextureImage),
		collapseButton(Vector2D(11.0F, 11.0F), Point2D(0.5625F, 0.6875F), Point2D(0.6875F, 0.8125F)),
		collapseObserver(this, &TreeItemWidget::HandleCollapseEvent)
{
	treeWidget = nullptr;
	itemWidget = widget;

	SetWidgetState(kWidgetCollapsed);
	SetDefaultColorType(kWidgetColorButton);

	collapseButton.SetWidgetState(kWidgetCollapsed);
	collapseButton.SetObserver(&collapseObserver);
}

TreeItemWidget::TreeItemWidget(const char *text, const char *font) : TreeItemWidget(new TextWidget(text, font))
{
}

TreeItemWidget::~TreeItemWidget()
{
	delete itemWidget;
}

void TreeItemWidget::SetDynamicWidgetColor(const ColorRGBA& color, WidgetColorType type)
{
	if ((type == kWidgetColorDefault) || (type == kWidgetColorButton))
	{
		collapseButton.SetDynamicWidgetColor(color);
	}
	else if (type == kWidgetColorLine)
	{
		colorAttribute.SetDiffuseColor(color);
	}
}

void TreeItemWidget::SetDynamicWidgetAlpha(float alpha, WidgetColorType type)
{
	if ((type == kWidgetColorDefault) || (type == kWidgetColorButton))
	{
		collapseButton.SetDynamicWidgetAlpha(alpha);
	}
	else if (type == kWidgetColorLine)
	{
		colorAttribute.SetDiffuseAlpha(alpha);
	}
}

void TreeItemWidget::Preprocess(void)
{
	if (treeWidget)
	{
		int32 depth = Tree<TreeItemWidget>::GetNodeDepth();
		const TreeItemWidget *node = this;
		int32 vertexCount = 0;

		if (depth != 0)
		{
			vertexCount = 8;
			for (machine a = 1; a < depth; a++)
			{
				node = node->Tree<TreeItemWidget>::GetSuperNode();
				if (node->Tree<TreeItemWidget>::Next())
				{
					vertexCount += 4;
				}
			}
		}

		SetVertexCount(vertexCount);

		if (vertexCount != 0)
		{
			SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(LineVertex));
			SetVertexAttributeArray(kArrayPosition, 0, 2);
			SetVertexAttributeArray(kArrayTexcoord, sizeof(Point2D), 2);
			vertexBuffer.Establish(sizeof(LineVertex) * vertexCount);

			attributeList.Append(&colorAttribute);
			attributeList.Append(&textureAttribute);
			SetMaterialAttributeList(&attributeList);
		}

		float x = (float) depth * treeWidget->GetIndentSpacing() + 21.0F;
		Vector2D naturalSize = treeWidget->GetNaturalListItemSize();

		itemWidget->SetWidgetSize(Vector2D(FmaxZero(naturalSize.x - x), naturalSize.y));
		itemWidget->SetWidgetPosition(Point3D(x, 0.0F, 0.0F));
		Widget::AppendSubnode(itemWidget);

		if (Tree<TreeItemWidget>::GetFirstSubnode())
		{
			collapseButton.SetWidgetPosition(Point3D(x - 17.0F, 1.0F, 0.0F));
			Widget::AppendSubnode(&collapseButton);
		}
	}
	else
	{
		SetVertexCount(0);
	}

	RenderableWidget::Preprocess();
}

void TreeItemWidget::Build(void)
{
	int32 vertexCount = GetVertexCount();
	if (vertexCount != 0)
	{
		volatile LineVertex *restrict vertex = vertexBuffer.BeginUpdate<LineVertex>();

		float indent = treeWidget->GetIndentSpacing();
		float d = indent - 6.0F;

		float b = -treeWidget->GetItemOffset().y;
		float h = itemWidget->GetWidgetSize().y;
		float x = itemWidget->GetWidgetPosition().x - (indent + 12.0F);
		float y = Floor(h * 0.5F) + b - 1.0F;

		float s = d * 0.125F + 0.125F;
		float t1 = (GetWidgetPosition().y + b) * 0.125F;
		float t2 = t1 + h * 0.125F;

		vertex[0].position.Set(x, y - 1.0F);
		vertex[0].texcoord.Set(0.125F, 0.1875F);

		vertex[1].position.Set(x, y + 2.0F);
		vertex[1].texcoord.Set(0.125F, 0.375F);

		vertex[2].position.Set(x + d, y + 2.0F);
		vertex[2].texcoord.Set(s, 0.375F);

		vertex[3].position.Set(x + d, y - 1.0F);
		vertex[3].texcoord.Set(s, 0.1875F);

		vertex += 4;

		int32 depth = Tree<TreeItemWidget>::GetNodeDepth();
		const TreeItemWidget *node = this;

		for (machine a = 0; a < depth; a++)
		{
			if (node->Tree<TreeItemWidget>::Next())
			{
				vertex[0].position.Set(x + 2.0F, b);
				vertex[0].texcoord.Set(t1, 0.1875F);

				vertex[1].position.Set(x - 1.0F, b);
				vertex[1].texcoord.Set(t1, 0.375F);

				vertex[2].position.Set(x - 1.0F, b + h);
				vertex[2].texcoord.Set(t2, 0.375F);

				vertex[3].position.Set(x + 2.0F, b + h);
				vertex[3].texcoord.Set(t2, 0.1875F);

				vertex += 4;
			}
			else if (a == 0)
			{
				vertex[0].position.Set(x + 2.0F, b);
				vertex[0].texcoord.Set(t1, 0.1875F);

				vertex[1].position.Set(x - 1.0F, b);
				vertex[1].texcoord.Set(t1, 0.375F);

				vertex[2].position.Set(x - 1.0F, y + 2.0F);
				vertex[2].texcoord.Set(t1 + (y + 2.0F - b) * 0.125F, 0.375F);

				vertex[3].position.Set(x + 2.0F, y + 2.0F);
				vertex[3].texcoord.Set(t1 + (y + 2.0F - b) * 0.125F, 0.1875F);

				vertex += 4;
			}

			x -= indent;
			node = node->Tree<TreeItemWidget>::GetSuperNode();
		}

		vertexBuffer.EndUpdate();
	}
}

void TreeItemWidget::HandleCollapseEvent(Widget *widget, const WidgetEventData *eventData)
{
	if ((eventData->eventType == kEventWidgetActivate) && (treeWidget))
	{
		if (GetWidgetState() & kWidgetCollapsed)
		{
			widget->SetWidgetState(widget->GetWidgetState() & ~kWidgetCollapsed);
			treeWidget->ExpandTreeItem(this);
		}
		else
		{
			widget->SetWidgetState(widget->GetWidgetState() | kWidgetCollapsed);
			treeWidget->CollapseTreeItem(this, true);
		}
	}
}


TreeWidget::TreeWidget() : ListWidget(kWidgetTree)
{
}

TreeWidget::TreeWidget(const Vector2D& size, float spacing, const char *font) : ListWidget(kWidgetTree, size, spacing, font)
{
	indentSpacing = 14.0F;
}

TreeWidget::TreeWidget(const TreeWidget& treeWidget) : ListWidget(treeWidget)
{
	indentSpacing = treeWidget.indentSpacing;
}

TreeWidget::~TreeWidget()
{
}

Widget *TreeWidget::Replicate(void) const
{
	return (new TreeWidget(*this));
}

void TreeWidget::Pack(Packer& data, unsigned_int32 packFlags) const
{
	ListWidget::Pack(data, packFlags);

	data << ChunkHeader('INDT', 4);
	data << indentSpacing;

	data << TerminatorChunk;
}

void TreeWidget::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	ListWidget::Unpack(data, unpackFlags);
	UnpackChunkList<TreeWidget>(data, unpackFlags);
}

bool TreeWidget::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'INDT':

			data >> indentSpacing;
			return (true);
	}

	return (false);
}

int32 TreeWidget::GetSettingCount(void) const
{
	return (ListWidget::GetSettingCount() + 2);
}

Setting *TreeWidget::GetSetting(int32 index) const
{
	int32 count = ListWidget::GetSettingCount();
	if (index < count)
	{
		return (ListWidget::GetSetting(index));
	}

	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetTree, 'SETT'));
		return (new HeadingSetting(kWidgetTree, title));
	}

	if (index == count + 1)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetTree, 'INDT'));
		return (new TextSetting('TRIN', indentSpacing, title));
	}

	return (nullptr);
}

void TreeWidget::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'TRIN')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		indentSpacing = Fmax(Text::StringToFloat(text), 1.0F);
	}
	else
	{
		ListWidget::SetSetting(setting);
	}
}

void TreeWidget::PrependTreeItem(TreeItemWidget *item)
{
	item->treeWidget = this;
	PrependListItem(item);

	if (!item->Enabled())
	{
		item->Enable();
		item->itemWidget->Disable();
	}
}

void TreeWidget::AppendTreeItem(TreeItemWidget *item)
{
	item->treeWidget = this;
	AppendListItem(item);

	if (!item->Enabled())
	{
		item->Enable();
		item->itemWidget->Disable();
	}
}

void TreeWidget::InsertTreeItemBefore(TreeItemWidget *item, TreeItemWidget *before)
{
	item->treeWidget = this;
	InsertListItemBefore(item, before);

	if (!item->Enabled())
	{
		item->Enable();
		item->itemWidget->Disable();
	}
}

void TreeWidget::InsertTreeItemAfter(TreeItemWidget *item, TreeItemWidget *after)
{
	item->treeWidget = this;
	InsertListItemAfter(item, after);

	if (!item->Enabled())
	{
		item->Enable();
		item->itemWidget->Disable();
	}
}

TreeItemWidget *TreeWidget::InsertSubtreeItems(TreeItemWidget *item)
{
	if (!(item->GetWidgetState() & kWidgetCollapsed))
	{
		TreeItemWidget *subitem = item->Tree<TreeItemWidget>::GetFirstSubnode();
		while (subitem)
		{
			InsertTreeItemAfter(subitem, item);
			item = InsertSubtreeItems(subitem);

			subitem = subitem->Tree<TreeItemWidget>::Next();
		}
	}

	return (item);
}

bool TreeWidget::RemoveSubtreeItems(TreeItemWidget *item)
{
	bool change = false;

	if (!(item->GetWidgetState() & kWidgetCollapsed))
	{
		TreeItemWidget *subitem = item->Tree<TreeItemWidget>::GetFirstSubnode();
		while (subitem)
		{
			change |= RemoveListItem(subitem);
			change |= RemoveSubtreeItems(subitem);

			subitem = subitem->Tree<TreeItemWidget>::Next();
		}
	}

	return (change);
}

void TreeWidget::ExpandTreeItem(TreeItemWidget *item)
{
	unsigned_int32 state = item->GetWidgetState();
	if (state & kWidgetCollapsed)
	{
		item->SetWidgetState(state & ~kWidgetCollapsed);

		TreeItemWidget *subitem = item->Tree<TreeItemWidget>::GetFirstSubnode();
		while (subitem)
		{
			InsertTreeItemAfter(subitem, item);
			item = InsertSubtreeItems(subitem);

			subitem = subitem->Tree<TreeItemWidget>::Next();
		}
	}
}

void TreeWidget::CollapseTreeItem(TreeItemWidget *item, bool post)
{
	unsigned_int32 state = item->GetWidgetState();
	if (!(state & kWidgetCollapsed))
	{
		item->SetWidgetState(state | kWidgetCollapsed);

		bool change = false;

		TreeItemWidget *subitem = item->Tree<TreeItemWidget>::GetFirstSubnode();
		while (subitem)
		{
			change |= RemoveListItem(subitem);
			change |= RemoveSubtreeItems(subitem);

			subitem = subitem->Tree<TreeItemWidget>::Next();
		}

		if (post & change)
		{
			PostWidgetEvent(WidgetEventData(kEventWidgetChange));
		}
	}
}


TableWidget::TableWidget() :
		RenderableWidget(kWidgetTable, kRenderQuads),
		scrollObserver(this, &TableWidget::HandleScrollEvent),
		tableVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		structureVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		borderColorAttribute(kAttributeMutable),
		borderTextureAttribute(&LineWidget::lineTextureHeader, LineWidget::lineTextureImage),
		borderRenderable(kRenderQuads),
		focusColorAttribute(kAttributeMutable),
		focusRenderable(kRenderQuads),
		scrollWidget(Zero2D)
{
	tableFlags = 0;
	columnCount = 1;

	cellSize.Set(32.0F, 32.0F);
	itemOffset.Set(0.0F, 0.0F);
	hiliteInset.Set(0.0F, 0.0F);

	colorOverrideFlags = 0;
	backgroundColor.Set(1.0F, 1.0F, 1.0F);
	hiliteColor.Set(0.0F, 0.0F, 0.0F);
	focusColor.Set(0.0F, 0.0F, 0.0F);

	preprocessFlag = false;
	tableUpdateFlags = 0;

	tableItemCount = 0;
	clickItemIndex = -1;

	displayRowIndex = 0;
	selectionVertex = nullptr;

	SetActiveUpdateFlags(GetActiveUpdateFlags() | kUpdateStructure);
	SetWidgetUsage(kWidgetKeyboardFocus | kWidgetMouseWheel);

	SetDefaultColorType(kWidgetColorBorder);
	RenderableWidget::SetWidgetColor(K::black);
}

TableWidget::TableWidget(const Vector2D& size, int32 columns, const Vector2D& cell) :
		RenderableWidget(kWidgetTable, kRenderQuads, size),
		scrollObserver(this, &TableWidget::HandleScrollEvent),
		tableVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		structureVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		borderColorAttribute(kAttributeMutable),
		borderTextureAttribute(&LineWidget::lineTextureHeader, LineWidget::lineTextureImage),
		borderRenderable(kRenderQuads),
		focusColorAttribute(kAttributeMutable),
		focusRenderable(kRenderQuads),
		scrollWidget(Vector2D(16.0F, size.y))
{
	tableFlags = 0;
	columnCount = columns;

	cellSize = cell;
	itemOffset.Set(0.0F, 0.0F);
	hiliteInset.Set(0.0F, 0.0F);

	colorOverrideFlags = 0;
	backgroundColor.Set(1.0F, 1.0F, 1.0F);
	hiliteColor.Set(0.0F, 0.0F, 0.0F);
	focusColor.Set(0.0F, 0.0F, 0.0F);

	preprocessFlag = false;
	tableUpdateFlags = 0;

	tableItemCount = 0;
	clickItemIndex = -1;

	displayRowIndex = 0;
	selectionVertex = nullptr;

	SetWidgetUsage(kWidgetKeyboardFocus | kWidgetMouseWheel);
	SetActiveUpdateFlags(GetActiveUpdateFlags() | kUpdateStructure);

	SetDefaultColorType(kWidgetColorBorder);
	RenderableWidget::SetWidgetColor(K::black);
}

TableWidget::TableWidget(const TableWidget& tableWidget) :
		RenderableWidget(tableWidget),
		scrollObserver(this, &TableWidget::HandleScrollEvent),
		tableVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		structureVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		borderColorAttribute(kAttributeMutable),
		borderTextureAttribute(&LineWidget::lineTextureHeader, LineWidget::lineTextureImage),
		borderRenderable(kRenderQuads),
		focusColorAttribute(kAttributeMutable),
		focusRenderable(kRenderQuads),
		scrollWidget(tableWidget.scrollWidget.GetWidgetSize())
{
	tableFlags = tableWidget.tableFlags;
	columnCount = tableWidget.columnCount;

	cellSize = tableWidget.cellSize;
	itemOffset = tableWidget.itemOffset;
	hiliteInset = tableWidget.hiliteInset;

	colorOverrideFlags = tableWidget.colorOverrideFlags;
	backgroundColor = tableWidget.backgroundColor;
	hiliteColor = tableWidget.hiliteColor;
	focusColor = tableWidget.focusColor;

	preprocessFlag = false;
	tableUpdateFlags = 0;

	tableItemCount = 0;
	clickItemIndex = -1;

	displayRowIndex = 0;
	selectionVertex = nullptr;

	SetActiveUpdateFlags(GetActiveUpdateFlags() | kUpdateStructure);
}

TableWidget::~TableWidget()
{
	delete[] selectionVertex;
}

Widget *TableWidget::Replicate(void) const
{
	return (new TableWidget(*this));
}

void TableWidget::Pack(Packer& data, unsigned_int32 packFlags) const
{
	RenderableWidget::Pack(data, packFlags);

	data << ChunkHeader('FLAG', 4);
	data << tableFlags;

	data << ChunkHeader('CLMN', 4);
	data << columnCount;

	data << ChunkHeader('CELL', sizeof(Vector2D));
	data << cellSize;

	data << ChunkHeader('OFST', sizeof(Vector2D));
	data << itemOffset;

	data << ChunkHeader('INST', sizeof(Vector2D));
	data << hiliteInset;

	data << ChunkHeader('OVER', 4);
	data << colorOverrideFlags;

	data << ChunkHeader('BGDC', sizeof(ColorRGBA));
	data << backgroundColor;

	if (colorOverrideFlags & kWidgetOverrideHiliteColor)
	{
		data << ChunkHeader('HILC', sizeof(ColorRGBA));
		data << hiliteColor;
	}

	if (colorOverrideFlags & kWidgetOverrideFocusColor)
	{
		data << ChunkHeader('FOCC', sizeof(ColorRGBA));
		data << focusColor;
	}

	data << TerminatorChunk;
}

void TableWidget::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	RenderableWidget::Unpack(data, unpackFlags);
	UnpackChunkList<TableWidget>(data, unpackFlags);
}

bool TableWidget::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> tableFlags;
			return (true);

		case 'CLMN':

			data >> columnCount;
			return (true);

		case 'CELL':

			data >> cellSize;
			return (true);

		case 'OFST':

			data >> itemOffset;
			return (true);

		case 'INST':

			data >> hiliteInset;
			return (true);

		case 'OVER':

			data >> colorOverrideFlags;
			return (true);

		case 'BGDC':

			data >> backgroundColor;
			return (true);

		case 'HILC':

			data >> hiliteColor;
			return (true);

		case 'FOCC':

			data >> focusColor;
			return (true);
	}

	return (false);
}

void *TableWidget::BeginSettingsUnpack(void)
{
	colorOverrideFlags = 0;
	return (RenderableWidget::BeginSettingsUnpack());
}

int32 TableWidget::GetSettingCount(void) const
{
	return (RenderableWidget::GetSettingCount() + 14);
}

Setting *TableWidget::GetSetting(int32 index) const
{
	int32 count = RenderableWidget::GetSettingCount();
	if (index < count)
	{
		return (RenderableWidget::GetSetting(index));
	}

	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetTable, 'SETT'));
		return (new HeadingSetting(kWidgetTable, title));
	}

	if (index == count + 1)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetTable, 'CLMN'));
		return (new TextSetting('CLMN', Text::IntegerToString(columnCount), title, 3, &EditTextWidget::NumberFilter));
	}

	if (index == count + 2)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetTable, 'CELX'));
		return (new TextSetting('CELX', cellSize.x, title));
	}

	if (index == count + 3)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetTable, 'CELY'));
		return (new TextSetting('CELY', cellSize.y, title));
	}

	if (index == count + 4)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetTable, 'OFFX'));
		return (new TextSetting('TOFX', itemOffset.x, title));
	}

	if (index == count + 5)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetTable, 'OFFY'));
		return (new TextSetting('TOFY', itemOffset.y, title));
	}

	if (index == count + 6)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetTable, 'INSX'));
		return (new TextSetting('TINX', hiliteInset.x, title));
	}

	if (index == count + 7)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetTable, 'INSY'));
		return (new TextSetting('TINY', hiliteInset.y, title));
	}

	if (index == count + 8)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetTable, 'MULT'));
		return (new BooleanSetting('TMSL', ((tableFlags & kTableMultipleSelection) != 0), title));
	}

	if (index == count + 9)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetTable, 'FPLN'));
		return (new BooleanSetting('TFPL', ((tableFlags & kTableFocusPlain) != 0), title));
	}

	if (index == count + 10)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetTable, 'ITEN'));
		return (new BooleanSetting('TIEN', ((tableFlags & kTableItemsEnabled) != 0), title));
	}

	if (index == count + 11)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetTable, 'BGDC'));
		const char *picker = table->GetString(StringID('WDGT', kWidgetTable, 'BGDP'));
		return (new ColorSetting('TIBG', backgroundColor, title, picker, kColorPickerAlpha));
	}

	if (index == count + 12)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetTable, 'HILC'));
		const char *picker = table->GetString(StringID('WDGT', kWidgetTable, 'HILP'));
		return (new CheckColorSetting('TIHC', ((colorOverrideFlags & kWidgetOverrideHiliteColor) != 0), hiliteColor, title, picker, kColorPickerAlpha));
	}

	if (index == count + 13)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetTable, 'FOCC'));
		const char *picker = table->GetString(StringID('WDGT', kWidgetTable, 'FOCP'));
		return (new CheckColorSetting('TIFC', ((colorOverrideFlags & kWidgetOverrideFocusColor) != 0), focusColor, title, picker, kColorPickerAlpha));
	}

	return (nullptr);
}

void TableWidget::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'CLMN')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		columnCount = Max(Text::StringToInteger(text), 1);
		SetTableUpdateFlags(kTableUpdateVisibility);
	}
	else if (identifier == 'CELX')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		cellSize.x = Fmax(Text::StringToFloat(text), 1.0F);
	}
	else if (identifier == 'CELY')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		cellSize.y = Fmax(Text::StringToFloat(text), 1.0F);
	}
	else if (identifier == 'TOFX')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		itemOffset.x = Text::StringToFloat(text);
	}
	else if (identifier == 'TOFY')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		itemOffset.y = Text::StringToFloat(text);
	}
	else if (identifier == 'TINX')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		hiliteInset.x = FmaxZero(Text::StringToFloat(text));
	}
	else if (identifier == 'TINY')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		hiliteInset.y = FmaxZero(Text::StringToFloat(text));
	}
	else if (identifier == 'TMSL')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			tableFlags |= kTableMultipleSelection;
		}
		else
		{
			tableFlags &= ~kTableMultipleSelection;
		}
	}
	else if (identifier == 'TFPL')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			tableFlags |= kTableFocusPlain;
		}
		else
		{
			tableFlags &= ~kTableFocusPlain;
		}
	}
	else if (identifier == 'TIEN')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			tableFlags |= kTableItemsEnabled;
		}
		else
		{
			tableFlags &= ~kTableItemsEnabled;
		}
	}
	else if (identifier == 'TIBG')
	{
		backgroundColor = static_cast<const ColorSetting *>(setting)->GetColor();
	}
	else if (identifier == 'TIHC')
	{
		const CheckColorSetting *checkColorSetting = static_cast<const CheckColorSetting *>(setting);
		if (checkColorSetting->GetCheckValue() != 0)
		{
			colorOverrideFlags |= kWidgetOverrideHiliteColor;
			hiliteColor = checkColorSetting->GetColor();
		}
		else
		{
			colorOverrideFlags &= ~kWidgetOverrideHiliteColor;
			SetDefaultHiliteColor();
		}
	}
	else if (identifier == 'TIFC')
	{
		const CheckColorSetting *checkColorSetting = static_cast<const CheckColorSetting *>(setting);
		if (checkColorSetting->GetCheckValue() != 0)
		{
			colorOverrideFlags |= kWidgetOverrideFocusColor;
			focusColor = checkColorSetting->GetColor();
		}
		else
		{
			colorOverrideFlags &= ~kWidgetOverrideFocusColor;
			SetDefaultFocusColor();
		}
	}
	else
	{
		RenderableWidget::SetSetting(setting);
	}
}

const ColorRGBA& TableWidget::GetWidgetColor(WidgetColorType type) const
{
	if (type == kWidgetColorBackground)
	{
		return (backgroundColor);
	}

	if (type == kWidgetColorHilite)
	{
		return (hiliteColor);
	}

	if (type == kWidgetColorFocus)
	{
		return (focusColor);
	}

	return (RenderableWidget::GetWidgetColor(type));
}

void TableWidget::SetWidgetColor(const ColorRGBA& color, WidgetColorType type)
{
	if (type == kWidgetColorBackground)
	{
		backgroundColor = color;
	}
	else if (type == kWidgetColorHilite)
	{
		hiliteColor = color;
		colorOverrideFlags |= kWidgetOverrideHiliteColor;
	}
	else if (type == kWidgetColorFocus)
	{
		focusColor = color;
		colorOverrideFlags |= kWidgetOverrideFocusColor;
	}

	RenderableWidget::SetWidgetColor(color, type);
}

void TableWidget::SetWidgetAlpha(float alpha, WidgetColorType type)
{
	if (type == kWidgetColorBackground)
	{
		backgroundColor.alpha = alpha;
	}
	else if (type == kWidgetColorHilite)
	{
		hiliteColor.alpha = alpha;
		colorOverrideFlags |= kWidgetOverrideHiliteColor;
	}
	else if (type == kWidgetColorFocus)
	{
		focusColor.alpha = alpha;
		colorOverrideFlags |= kWidgetOverrideFocusColor;
	}

	RenderableWidget::SetWidgetAlpha(alpha, type);
}

void TableWidget::SetDynamicWidgetColor(const ColorRGBA& color, WidgetColorType type)
{
	if ((type == kWidgetColorDefault) || (type == kWidgetColorBorder))
	{
		borderColorAttribute.SetDiffuseColor(color);
	}
	else if (type == kWidgetColorBackground)
	{
		dynamicBackgroundColor = color;
		SetBuildFlag();
	}
	else if (type == kWidgetColorHilite)
	{
		dynamicHiliteColor = color;
		SetBuildFlag();
	}
	else if (type == kWidgetColorFocus)
	{
		focusColorAttribute.SetDiffuseColor(color);
	}
	else
	{
		RenderableWidget::SetDynamicWidgetColor(color, type);
	}
}

void TableWidget::SetDynamicWidgetAlpha(float alpha, WidgetColorType type)
{
	if ((type == kWidgetColorDefault) || (type == kWidgetColorBorder))
	{
		borderColorAttribute.SetDiffuseAlpha(alpha);
	}
	else if (type == kWidgetColorBackground)
	{
		dynamicBackgroundColor.alpha = alpha;
		SetBuildFlag();
	}
	else if (type == kWidgetColorHilite)
	{
		dynamicHiliteColor.alpha = alpha;
		SetBuildFlag();
	}
	else if (type == kWidgetColorFocus)
	{
		focusColorAttribute.SetDiffuseAlpha(alpha);
	}
	else
	{
		RenderableWidget::SetDynamicWidgetAlpha(alpha, type);
	}
}

void TableWidget::SetDefaultHiliteColor(void)
{
	hiliteColor = TheInterfaceMgr->GetInterfaceColor(kInterfaceColorHilite);
}

void TableWidget::SetDefaultFocusColor(void)
{
	focusColor.Set(TheInterfaceMgr->GetInterfaceColor(kInterfaceColorHilite).GetColorRGB() * 0.625F, 1.0F);
}

void TableWidget::SetWidgetSize(const Vector2D& size)
{
	RenderableWidget::SetWidgetSize(size);

	scrollWidget.SetWidgetSize(Vector2D(16.0F, size.y));
	scrollWidget.SetWidgetPosition(Point3D(FmaxZero(size.x - 16.0F), 0.0F, 0.0F));
	scrollWidget.Invalidate();

	SetTableUpdateFlags(kTableUpdateVisibility);
}

WidgetPart TableWidget::TestPosition(const Point3D& position) const
{
	if (position.x < GetWidgetSize().x - 16.0F)
	{
		return (kWidgetPartInterior);
	}

	return (kWidgetPartNone);
}

void TableWidget::Preprocess(void)
{
	if (!(colorOverrideFlags & kWidgetOverrideHiliteColor))
	{
		SetDefaultHiliteColor();
	}

	if (!(colorOverrideFlags & kWidgetOverrideFocusColor))
	{
		SetDefaultFocusColor();
	}

	dynamicBackgroundColor = backgroundColor;
	dynamicHiliteColor = hiliteColor;

	itemGroup.SetWidgetState(itemGroup.GetWidgetState() | kWidgetNonpersistent);
	AppendSubnode(&itemGroup);

	scrollWidget.SetWidgetSize(Vector2D(16.0F, GetWidgetSize().y));
	scrollWidget.SetWidgetPosition(Point3D(FmaxZero(GetWidgetSize().x - 16.0F), 0.0F, 0.0F));
	scrollWidget.SetWidgetState(scrollWidget.GetWidgetState() | kWidgetNonpersistent);
	scrollWidget.SetObserver(&scrollObserver);
	AppendSubnode(&scrollWidget);

	RenderableWidget::Preprocess();

	SetVertexBuffer(kVertexBufferAttributeArray, &tableVertexBuffer, sizeof(TableVertex));
	SetVertexAttributeArray(kArrayPosition, 0, 2);
	SetVertexAttributeArray(kArrayColor, sizeof(Point2D), 4);

	structureVertexBuffer.Establish(sizeof(BorderVertex) * 16 + sizeof(GlowVertex) * 32);

	InitRenderable(&borderRenderable);
	borderRenderable.SetVertexCount(16);
	borderRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &structureVertexBuffer, sizeof(BorderVertex));
	borderRenderable.SetVertexAttributeArray(kArrayPosition, 0, 2);
	borderRenderable.SetVertexAttributeArray(kArrayTexcoord, sizeof(Point2D), 2);

	borderAttributeList.Append(&borderColorAttribute);
	borderAttributeList.Append(&borderTextureAttribute);
	borderRenderable.SetMaterialAttributeList(&borderAttributeList);

	InitRenderable(&focusRenderable);
	focusRenderable.SetVertexCount(32);
	focusRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &structureVertexBuffer, sizeof(GlowVertex));
	focusRenderable.SetVertexAttributeArray(kArrayPosition, sizeof(BorderVertex) * 16, 2);
	focusRenderable.SetVertexAttributeArray(kArrayTexcoord, sizeof(BorderVertex) * 16 + sizeof(Point2D), 2);

	focusAttributeList.Append(&focusColorAttribute);
	focusRenderable.SetMaterialAttributeList(&focusAttributeList);
	focusRenderable.SetMaterialObjectPointer(TheInterfaceMgr->GetMaterialObjectPointer());

	preprocessFlag = true;
	tableUpdateFlags = kTableUpdatePlacement | kTableUpdateVisibility | kTableUpdateSelection;
}

void TableWidget::EnterForeground(void)
{
	RenderableWidget::EnterForeground();
	SetWidgetState(GetWidgetState() & ~kWidgetBackground);
}

void TableWidget::EnterBackground(void)
{
	RenderableWidget::EnterBackground();
	SetWidgetState(GetWidgetState() | kWidgetBackground);
}

inline int32 TableWidget::GetTableRowCount(void) const
{
	return ((tableItemCount + columnCount - 1) / columnCount);
}

void TableWidget::UpdatePlacement(void)
{
	tableUpdateFlags &= ~kTableUpdatePlacement;

	machine column = 0;
	Point3D position(itemOffset, 0.0F);

	Widget *widget = itemGroup.GetFirstSubnode();
	while (widget)
	{
		widget->SetWidgetPosition(position);

		position.x += cellSize.x;
		if (++column == columnCount)
		{
			column = 0;
			position.x = itemOffset.x;
			position.y += cellSize.y;
		}

		widget = widget->Next();
	}

	UpdateVisibility();
}

void TableWidget::UpdateVisibility(void)
{
	tableUpdateFlags &= ~kTableUpdateVisibility;

	displayRowCount = (int32) (GetWidgetSize().y / cellSize.y);
	scrollWidget.SetPageDistance(Max(displayRowCount - 1, 1));

	int32 maxIndex = MaxZero(GetTableRowCount() - displayRowCount);
	scrollWidget.SetMaxValue(maxIndex);

	displayRowIndex = Min(displayRowIndex, maxIndex);
	scrollWidget.SetValue(displayRowIndex);

	displayItem = nullptr;

	delete[] selectionVertex;
	int32 vertexCount = displayRowCount * columnCount * 4;
	selectionVertex = new Point2D[vertexCount];
	tableVertexBuffer.Establish(sizeof(TableVertex) * (vertexCount + 4));
	InvalidateVertexData();

	machine row = 0;
	machine column = 0;

	Widget *widget = itemGroup.GetFirstSubnode();
	while (widget)
	{
		if ((unsigned_int32) (row - displayRowIndex) < (unsigned_int32) displayRowCount)
		{
			widget->Show();

			if ((row == displayRowIndex) && (column == 0))
			{
				displayItem = widget;
			}
		}
		else
		{
			widget->Hide();
		}

		if (++column == columnCount)
		{
			column = 0;
			row++;
		}

		widget = widget->Next();
	}

	itemGroup.SetWidgetPosition(Point3D(0.0F, -cellSize.y * (float) displayRowIndex, 0.0F));
	itemGroup.Invalidate();

	UpdateSelection();
}

void TableWidget::UpdateSelection(void)
{
	tableUpdateFlags &= ~kTableUpdateSelection;

	int32 selectCount = 0;
	const Widget *widget = displayItem;

	int32 displayCount = displayRowCount * columnCount;
	for (machine a = 0; a < displayCount; a++)
	{
		if (!widget)
		{
			break;
		}

		if (widget->GetWidgetState() & kWidgetSelected)
		{
			int32 row = a / columnCount;
			int32 column = a - row * columnCount;

			float x1 = (float) column * cellSize.x + hiliteInset.x;
			float y1 = (float) row * cellSize.y + hiliteInset.y;
			float x2 = x1 + cellSize.x - hiliteInset.x * 2.0F;
			float y2 = y1 + cellSize.y - hiliteInset.y * 2.0F;

			Point2D *vertex = &selectionVertex[selectCount * 4];
			vertex[0].Set(x1, y1);
			vertex[1].Set(x1, y2);
			vertex[2].Set(x2, y2);
			vertex[3].Set(x2, y1);

			selectCount++;
		}

		widget = widget->Next();
	}

	SetVertexCount(selectCount * 4 + 4);
	SetBuildFlag();
}

void TableWidget::HandleStructureUpdate(void)
{
	if (tableUpdateFlags != 0)
	{
		if (tableUpdateFlags & kTableUpdatePlacement)
		{
			UpdatePlacement();
		}
		else if (tableUpdateFlags & kTableUpdateVisibility)
		{
			UpdateVisibility();
		}
		else if (tableUpdateFlags & kTableUpdateSelection)
		{
			UpdateSelection();
		}
	}
}

void TableWidget::Build(void)
{
	borderColorAttribute.SetDiffuseColor(RenderableWidget::GetWidgetColor());
	focusColorAttribute.SetDiffuseColor(focusColor);

	volatile TableVertex *restrict tableVertex = tableVertexBuffer.BeginUpdate<TableVertex>();

	float w = GetWidgetSize().x;
	float h = GetWidgetSize().y;

	tableVertex[0].position.Set(0.0F, 0.0F);
	tableVertex[1].position.Set(0.0F, h);
	tableVertex[2].position.Set(w - 16.0F, h);
	tableVertex[3].position.Set(w - 16.0F, 0.0F);

	for (machine a = 0; a < 4; a++)
	{
		tableVertex[a].color = dynamicBackgroundColor;
	}

	float alpha = (GetWidgetState() & kWidgetBackground) ? 0.5F : 1.0F;
	ColorRGBA color(dynamicHiliteColor.GetColorRGB(), dynamicHiliteColor.alpha * alpha);

	int32 count = GetVertexCount();
	for (machine a = 4; a < count; a++)
	{
		tableVertex[a].position = selectionVertex[a - 4];
		tableVertex[a].color = color;
	}

	tableVertexBuffer.EndUpdate();

	Box2D box(Zero2D, Point2D(Fmax(w, 16.0F), h));

	volatile BorderVertex *restrict borderVertex = structureVertexBuffer.BeginUpdate<BorderVertex>();
	BuildBorder(box, borderVertex);

	volatile GlowVertex *restrict focusVertex = reinterpret_cast<volatile GlowVertex *>(borderVertex + 16);
	BuildGlow(box, focusVertex);

	structureVertexBuffer.EndUpdate();
}

void TableWidget::Render(List<Renderable> *renderList)
{
	RenderableWidget::Render(renderList);

	renderList->Append(&borderRenderable);

	if ((GetWidgetState() & kWidgetFocus) && (!(tableFlags & kTableFocusPlain)))
	{
		renderList->Append(&focusRenderable);
	}
}

void TableWidget::HandleScrollEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		displayRowIndex = static_cast<ScrollWidget *>(widget)->GetValue();
		SetTableUpdateFlags(kTableUpdateVisibility);

		if (GetWidgetUsage() & kWidgetKeyboardFocus)
		{
			RootWidget *root = GetRootWidget();
			if (root)
			{
				root->SetFocusWidget(this);
			}
		}
	}
}

void TableWidget::HandleMouseEvent(const PanelMouseEventData *eventData)
{
	EventType eventType = eventData->eventType;
	if (eventType == kEventMouseDown)
	{
		PanelController *controller = GetPanelController();
		if (controller)
		{
			controller->BeginKeyboardInteraction(this);
		}

		int32 row = (int32) (eventData->mousePosition.y / cellSize.y) + displayRowIndex;
		int32 column = Min((int32) (eventData->mousePosition.x / cellSize.x), columnCount);
		int32 index = row * columnCount + column;

		if ((row < displayRowIndex + displayRowCount) && (index < tableItemCount))
		{
			bool activate = ((eventData->eventFlags & kMouseDoubleClick) && (index == clickItemIndex));
			clickItemIndex = index;

			Widget *selection = GetTableItem(index);
			if (!(selection->GetWidgetState() & kWidgetInactive))
			{
				if (!(tableFlags & kTableMultipleSelection))
				{
					unsigned_int32 state = selection->GetWidgetState();
					if (!(state & kWidgetSelected))
					{
						Widget *widget = itemGroup.GetFirstSubnode();
						while (widget)
						{
							if (widget != selection)
							{
								widget->SetWidgetState(widget->GetWidgetState() & ~kWidgetSelected);
							}
							else
							{
								widget->SetWidgetState(state | kWidgetSelected);
							}

							widget = widget->Next();
						}

						SetTableUpdateFlags(kTableUpdateSelection);
						PostWidgetEvent(WidgetEventData(kEventWidgetChange));
					}
					else
					{
						if (activate)
						{
							Activate(eventData->initiatorNode);
						}
					}
				}
				else
				{
					bool post = false;

					if (!InterfaceMgr::GetShiftKey())
					{
						Widget *widget = itemGroup.GetFirstSubnode();
						while (widget)
						{
							unsigned_int32 state = widget->GetWidgetState();

							if (widget != selection)
							{
								if (state & kWidgetSelected)
								{
									widget->SetWidgetState(state & ~kWidgetSelected);
									post = true;
								}
							}
							else
							{
								if (!(state & kWidgetSelected))
								{
									widget->SetWidgetState(state | kWidgetSelected);
									post = true;
								}
							}

							widget = widget->Next();
						}
					}
					else
					{
						selection->SetWidgetState(selection->GetWidgetState() ^ kWidgetSelected);
						post = true;
					}

					if (post)
					{
						SetTableUpdateFlags(kTableUpdateSelection);
						PostWidgetEvent(WidgetEventData(kEventWidgetChange));
					}
					else if (activate)
					{
						Activate(eventData->initiatorNode);
					}
				}
			}
		}
	}
	else if (eventType == kEventMouseWheel)
	{
		int32 maxIndex = GetTableRowCount() - displayRowCount;
		if (maxIndex > 0)
		{
			int32 index = MaxZero(Min(displayRowIndex - (int32) eventData->wheelDelta.y, maxIndex));
			if (index != displayRowIndex)
			{
				displayRowIndex = index;
				SetTableUpdateFlags(kTableUpdateVisibility);
			}
		}
	}
}

void TableWidget::HandleUpArrow(unsigned_int32 modifierKeys)
{
	int32 index = 0;
	bool post = false;

	Widget *widget = itemGroup.GetFirstSubnode();
	while (widget)
	{
		if (widget->GetWidgetState() & kWidgetSelected)
		{
			break;
		}

		widget = widget->Next();
	}

	if (widget)
	{
		Widget *previous = GetPreviousActiveTableColumnItem(widget);
		if (previous)
		{
			previous->SetWidgetState(previous->GetWidgetState() | kWidgetSelected);
			index = previous->GetNodeIndex();
			post = true;
		}
		else
		{
			widget = widget->Next();
		}

		if ((!(tableFlags & kTableMultipleSelection)) || (!(modifierKeys & kModifierKeyShift)))
		{
			while (widget)
			{
				unsigned_int32 state = widget->GetWidgetState();
				if (state & kWidgetSelected)
				{
					widget->SetWidgetState(state & ~kWidgetSelected);
					post = true;
				}

				widget = widget->Next();
			}
		}
	}
	else
	{
		widget = GetFirstActiveTableItem();
		if (widget)
		{
			widget->SetWidgetState(widget->GetWidgetState() | kWidgetSelected);
			index = widget->GetNodeIndex();
			post = true;
		}
	}

	if (post)
	{
		SetTableUpdateFlags(kTableUpdateSelection);
		RevealTableItem(index);
		PostWidgetEvent(WidgetEventData(kEventWidgetChange));
	}
}

void TableWidget::HandleDownArrow(unsigned_int32 modifierKeys)
{
	int32 index = 0;
	bool post = false;

	Widget *widget = itemGroup.GetLastSubnode();
	while (widget)
	{
		if (widget->GetWidgetState() & kWidgetSelected)
		{
			break;
		}

		widget = widget->Previous();
	}

	if (widget)
	{
		Widget *next = GetNextActiveTableColumnItem(widget);
		if (next)
		{
			next->SetWidgetState(next->GetWidgetState() | kWidgetSelected);
			index = next->GetNodeIndex();
			post = true;
		}
		else
		{
			widget = widget->Previous();
		}

		if ((!(tableFlags & kTableMultipleSelection)) || (!(modifierKeys & kModifierKeyShift)))
		{
			while (widget)
			{
				unsigned_int32 state = widget->GetWidgetState();
				if (state & kWidgetSelected)
				{
					widget->SetWidgetState(state & ~kWidgetSelected);
					post = true;
				}

				widget = widget->Previous();
			}
		}
	}
	else
	{
		widget = GetLastActiveTableItem();
		if (widget)
		{
			widget->SetWidgetState(widget->GetWidgetState() | kWidgetSelected);
			index = widget->GetNodeIndex();
			post = true;
		}
	}

	if (post)
	{
		SetTableUpdateFlags(kTableUpdateSelection);
		RevealTableItem(index);
		PostWidgetEvent(WidgetEventData(kEventWidgetChange));
	}
}

void TableWidget::HandleLeftArrow(unsigned_int32 modifierKeys)
{
	bool post = false;

	int32 index = 0;
	Widget *widget = itemGroup.GetFirstSubnode();
	while (widget)
	{
		if (widget->GetWidgetState() & kWidgetSelected)
		{
			break;
		}

		index++;
		widget = widget->Next();
	}

	if (widget)
	{
		int32 row = index / columnCount;

		Widget *previous = GetPreviousActiveTableItem(widget);
		if (previous)
		{
			int32 i = previous->GetNodeIndex();
			int32 r = i / columnCount;
			if (r == row)
			{
				previous->SetWidgetState(previous->GetWidgetState() | kWidgetSelected);
				index = i;
				post = true;
			}
			else
			{
				widget = widget->Next();
			}
		}
		else
		{
			widget = widget->Next();
		}

		if ((!(tableFlags & kTableMultipleSelection)) || (!(modifierKeys & kModifierKeyShift)))
		{
			while (widget)
			{
				unsigned_int32 state = widget->GetWidgetState();
				if (state & kWidgetSelected)
				{
					widget->SetWidgetState(state & ~kWidgetSelected);
					post = true;
				}

				widget = widget->Next();
			}
		}
	}
	else
	{
		widget = GetFirstActiveTableItem();
		if (widget)
		{
			widget->SetWidgetState(widget->GetWidgetState() | kWidgetSelected);
			index = widget->GetNodeIndex();
			post = true;
		}
	}

	if (post)
	{
		SetTableUpdateFlags(kTableUpdateSelection);
		RevealTableItem(index);
		PostWidgetEvent(WidgetEventData(kEventWidgetChange));
	}
}

void TableWidget::HandleRightArrow(unsigned_int32 modifierKeys)
{
	bool post = false;

	int32 index = tableItemCount - 1;
	Widget *widget = itemGroup.GetLastSubnode();
	while (widget)
	{
		if (widget->GetWidgetState() & kWidgetSelected)
		{
			break;
		}

		index--;
		widget = widget->Previous();
	}

	if (widget)
	{
		int32 row = index / columnCount;

		Widget *next = GetNextActiveTableItem(widget);
		if (next)
		{
			int32 i = next->GetNodeIndex();
			int32 r = i / columnCount;
			if (r == row)
			{
				next->SetWidgetState(next->GetWidgetState() | kWidgetSelected);
				index = i;
				post = true;
			}
			else
			{
				widget = widget->Previous();
			}
		}
		else
		{
			widget = widget->Previous();
		}

		if ((!(tableFlags & kTableMultipleSelection)) || (!(modifierKeys & kModifierKeyShift)))
		{
			while (widget)
			{
				unsigned_int32 state = widget->GetWidgetState();
				if (state & kWidgetSelected)
				{
					widget->SetWidgetState(state & ~kWidgetSelected);
					post = true;
				}

				widget = widget->Previous();
			}
		}
	}
	else
	{
		widget = GetLastActiveTableItem();
		if (widget)
		{
			widget->SetWidgetState(widget->GetWidgetState() | kWidgetSelected);
			index = widget->GetNodeIndex();
			post = true;
		}
	}

	if (post)
	{
		SetTableUpdateFlags(kTableUpdateSelection);
		RevealTableItem(index);
		PostWidgetEvent(WidgetEventData(kEventWidgetChange));
	}
}

bool TableWidget::HandleKeyboardEvent(const KeyboardEventData *eventData)
{
	if (eventData->eventType == kEventKeyDown)
	{
		unsigned_int32 keyCode = eventData->keyCode;

		if (keyCode == kKeyCodeUpArrow)
		{
			HandleUpArrow(eventData->modifierKeys);
			return (true);
		}
		else if (keyCode == kKeyCodeDownArrow)
		{
			HandleDownArrow(eventData->modifierKeys);
			return (true);
		}
		else if (keyCode == kKeyCodeLeftArrow)
		{
			HandleLeftArrow(eventData->modifierKeys);
			return (true);
		}
		else if (keyCode == kKeyCodeRightArrow)
		{
			HandleRightArrow(eventData->modifierKeys);
			return (true);
		}
		else if (keyCode == kKeyCodePageUp)
		{
			int32 row = MaxZero(displayRowIndex - displayRowCount + 1);
			if (displayRowIndex != row)
			{
				displayRowIndex = row;
				SetTableUpdateFlags(kTableUpdateVisibility);
			}

			return (true);
		}
		else if (keyCode == kKeyCodePageDown)
		{
			int32 row = Min(displayRowIndex + displayRowCount - 1, GetTableRowCount() - displayRowCount);
			if (displayRowIndex != row)
			{
				displayRowIndex = row;
				SetTableUpdateFlags(kTableUpdateVisibility);
			}

			return (true);
		}
		else if (keyCode == kKeyCodeHome)
		{
			if (displayRowIndex != 0)
			{
				displayRowIndex = 0;
				SetTableUpdateFlags(kTableUpdateVisibility);
			}

			return (true);
		}
		else if (keyCode == kKeyCodeEnd)
		{
			int32 row = MaxZero(GetTableRowCount() - displayRowCount);
			if (displayRowIndex != row)
			{
				displayRowIndex = row;
				SetTableUpdateFlags(kTableUpdateVisibility);
			}

			return (true);
		}
	}

	return (false);
}

void TableWidget::SetColumnCount(int32 columns)
{
	columnCount = columns;
	SetTableUpdateFlags(kTableUpdateVisibility);
}

Widget *TableWidget::GetTableItem(int32 index) const
{
	Widget *widget = itemGroup.GetFirstSubnode();
	while (widget)
	{
		if (index == 0)
		{
			return (widget);
		}

		index--;
		widget = widget->Next();
	}

	return (nullptr);
}

void TableWidget::RevealTableItem(int32 index)
{
	int32 row = index / columnCount;
	if ((unsigned_int32) (row - displayRowIndex) >= (unsigned_int32) displayRowCount)
	{
		displayRowIndex = MaxZero(Min(row - displayRowCount / 2, GetTableRowCount() - displayRowCount));
		SetTableUpdateFlags(kTableUpdateVisibility);
	}
}

int32 TableWidget::GetActiveTableItemCount(void) const
{
	int32 count = 0;

	Widget *widget = itemGroup.GetFirstSubnode();
	while (widget)
	{
		if (!(widget->GetWidgetState() & kWidgetInactive))
		{
			count++;
		}

		widget = widget->Next();
	}

	return (count);
}

Widget *TableWidget::GetActiveTableItem(int32 index) const
{
	Widget *widget = itemGroup.GetFirstSubnode();
	while (widget)
	{
		if (!(widget->GetWidgetState() & kWidgetInactive))
		{
			if (index == 0)
			{
				return (widget);
			}

			index--;
		}

		widget = widget->Next();
	}

	return (nullptr);
}

Widget *TableWidget::GetFirstActiveTableItem(void) const
{
	Widget *widget = itemGroup.GetFirstSubnode();
	while (widget)
	{
		if (!(widget->GetWidgetState() & kWidgetInactive))
		{
			return (widget);
		}

		widget = widget->Next();
	}

	return (nullptr);
}

Widget *TableWidget::GetLastActiveTableItem(void) const
{
	Widget *widget = itemGroup.GetLastSubnode();
	while (widget)
	{
		if (!(widget->GetWidgetState() & kWidgetInactive))
		{
			return (widget);
		}

		widget = widget->Previous();
	}

	return (nullptr);
}

Widget *TableWidget::GetPreviousActiveTableItem(const Widget *widget) const
{
	Widget *previous = widget->Previous();
	while (previous)
	{
		if (!(previous->GetWidgetState() & kWidgetInactive))
		{
			return (previous);
		}

		previous = previous->Previous();
	}

	return (nullptr);
}

Widget *TableWidget::GetNextActiveTableItem(const Widget *widget) const
{
	Widget *next = widget->Next();
	while (next)
	{
		if (!(next->GetWidgetState() & kWidgetInactive))
		{
			return (next);
		}

		next = next->Next();
	}

	return (nullptr);
}

Widget *TableWidget::GetPreviousActiveTableColumnItem(const Widget *widget) const
{
	Widget *previous = const_cast<Widget *>(widget);
	for (;;)
	{
		for (machine a = 0; a < columnCount; a++)
		{
			previous = previous->Previous();
			if (!previous)
			{
				goto end;
			}
		}

		if (!(previous->GetWidgetState() & kWidgetInactive))
		{
			return (previous);
		}
	}

	end:
	return (nullptr);
}

Widget *TableWidget::GetNextActiveTableColumnItem(const Widget *widget) const
{
	Widget *next = const_cast<Widget *>(widget);
	for (;;)
	{
		for (machine a = 0; a < columnCount; a++)
		{
			next = next->Next();
			if (!next)
			{
				goto end;
			}
		}

		if (!(next->GetWidgetState() & kWidgetInactive))
		{
			return (next);
		}
	}

	end:
	return (nullptr);
}

void TableWidget::PrependTableItem(Widget *widget)
{
	itemGroup.PrependSubnode(widget);
	tableItemCount++;

	if (!(tableFlags & kTableItemsEnabled))
	{
		widget->Disable();
	}

	if (preprocessFlag)
	{
		widget->Preprocess();
	}

	clickItemIndex = -1;
	SetTableUpdateFlags(kTableUpdatePlacement);
}

void TableWidget::AppendTableItem(Widget *widget)
{
	itemGroup.AppendSubnode(widget);
	tableItemCount++;

	if (!(tableFlags & kTableItemsEnabled))
	{
		widget->Disable();
	}

	if (preprocessFlag)
	{
		widget->Preprocess();
	}

	clickItemIndex = -1;
	SetTableUpdateFlags(kTableUpdatePlacement);
}

void TableWidget::InsertTableItemBefore(Widget *widget, Widget *before)
{
	itemGroup.InsertSubnodeBefore(widget, before);
	tableItemCount++;

	if (!(tableFlags & kTableItemsEnabled))
	{
		widget->Disable();
	}

	if (preprocessFlag)
	{
		widget->Preprocess();
	}

	clickItemIndex = -1;
	SetTableUpdateFlags(kTableUpdatePlacement);
}

void TableWidget::InsertTableItemAfter(Widget *widget, Widget *after)
{
	itemGroup.InsertSubnodeAfter(widget, after);
	tableItemCount++;

	if (!(tableFlags & kTableItemsEnabled))
	{
		widget->Disable();
	}

	if (preprocessFlag)
	{
		widget->Preprocess();
	}

	clickItemIndex = -1;
	SetTableUpdateFlags(kTableUpdatePlacement);
}

void TableWidget::RemoveTableItem(Widget *widget)
{
	itemGroup.RemoveSubnode(widget);
	tableItemCount--;

	clickItemIndex = -1;
	SetTableUpdateFlags(kTableUpdatePlacement);
}

void TableWidget::PurgeTableItems(void)
{
	itemGroup.PurgeSubtree();
	tableItemCount = 0;

	clickItemIndex = -1;
	SetTableUpdateFlags(kTableUpdatePlacement);
}

void TableWidget::PurgeSelectedTableItems(void)
{
	Widget *widget = GetFirstSelectedTableItem();
	while (widget)
	{
		Widget *next = GetNextSelectedTableItem(widget);

		delete widget;
		tableItemCount--;

		widget = next;
	}

	clickItemIndex = -1;
	SetTableUpdateFlags(kTableUpdatePlacement);
}

int32 TableWidget::GetSelectedTableItemCount(void) const
{
	int32 count = 0;

	Widget *widget = itemGroup.GetFirstSubnode();
	while (widget)
	{
		if (widget->GetWidgetState() & kWidgetSelected)
		{
			count++;
		}

		widget = widget->Next();
	}

	return (count);
}

Widget *TableWidget::GetSelectedTableItem(int32 index) const
{
	Widget *widget = itemGroup.GetFirstSubnode();
	while (widget)
	{
		if (widget->GetWidgetState() & kWidgetSelected)
		{
			if (index == 0)
			{
				return (widget);
			}

			index--;
		}

		widget = widget->Next();
	}

	return (nullptr);
}

Widget *TableWidget::GetFirstSelectedTableItem(void) const
{
	Widget *widget = itemGroup.GetFirstSubnode();
	while (widget)
	{
		if (widget->GetWidgetState() & kWidgetSelected)
		{
			return (widget);
		}

		widget = widget->Next();
	}

	return (nullptr);
}

Widget *TableWidget::GetLastSelectedTableItem(void) const
{
	Widget *widget = itemGroup.GetLastSubnode();
	while (widget)
	{
		if (widget->GetWidgetState() & kWidgetSelected)
		{
			return (widget);
		}

		widget = widget->Previous();
	}

	return (nullptr);
}

Widget *TableWidget::GetPreviousSelectedTableItem(const Widget *widget) const
{
	Widget *previous = widget->Previous();
	while (previous)
	{
		if (previous->GetWidgetState() & kWidgetSelected)
		{
			return (previous);
		}

		previous = previous->Previous();
	}

	return (nullptr);
}

Widget *TableWidget::GetNextSelectedTableItem(const Widget *widget) const
{
	Widget *next = widget->Next();
	while (next)
	{
		if (next->GetWidgetState() & kWidgetSelected)
		{
			return (next);
		}

		next = next->Next();
	}

	return (nullptr);
}

int32 TableWidget::GetFirstSelectedIndex(void) const
{
	int32 index = 0;
	Widget *widget = itemGroup.GetFirstSubnode();
	while (widget)
	{
		if (widget->GetWidgetState() & kWidgetSelected)
		{
			return (index);
		}

		index++;
		widget = widget->Next();
	}

	return (-1);
}

int32 TableWidget::GetLastSelectedIndex(void) const
{
	int32 index = 0;
	Widget *widget = itemGroup.GetLastSubnode();
	while (widget)
	{
		if (widget->GetWidgetState() & kWidgetSelected)
		{
			return (index);
		}

		index++;
		widget = widget->Previous();
	}

	return (-1);
}

void TableWidget::SelectTableItem(int32 index, bool post)
{
	bool change = false;

	if (!(tableFlags & kTableMultipleSelection))
	{
		Widget *widget = itemGroup.GetFirstSubnode();
		while (widget)
		{
			unsigned_int32 state = widget->GetWidgetState();
			if (index != 0)
			{
				if (state & kWidgetSelected)
				{
					change = true;
					widget->SetWidgetState(state & ~kWidgetSelected);
				}
			}
			else
			{
				if (!(state & kWidgetSelected))
				{
					change = true;
					widget->SetWidgetState(state | kWidgetSelected);
				}
			}

			index--;
			widget = widget->Next();
		}
	}
	else
	{
		Widget *widget = itemGroup.GetFirstSubnode();
		while (widget)
		{
			if (index == 0)
			{
				unsigned_int32 state = widget->GetWidgetState();
				if (!(state & kWidgetSelected))
				{
					change = true;
					widget->SetWidgetState(state | kWidgetSelected);
				}

				break;
			}

			index--;
			widget = widget->Next();
		}
	}

	SetTableUpdateFlags(kTableUpdateSelection);

	if (post & change)
	{
		PostWidgetEvent(WidgetEventData(kEventWidgetChange));
	}
}

void TableWidget::UnselectTableItem(int32 index, bool post)
{
	bool change = false;

	Widget *widget = itemGroup.GetFirstSubnode();
	while (widget)
	{
		if (index == 0)
		{
			unsigned_int32 state = widget->GetWidgetState();
			if (state & kWidgetSelected)
			{
				change = true;
				widget->SetWidgetState(state & ~kWidgetSelected);
			}

			break;
		}

		index--;
		widget = widget->Next();
	}

	SetTableUpdateFlags(kTableUpdateSelection);

	if (post & change)
	{
		PostWidgetEvent(WidgetEventData(kEventWidgetChange));
	}
}

void TableWidget::SelectAllTableItems(bool post)
{
	bool change = false;
	Widget *widget = itemGroup.GetFirstSubnode();

	if (tableFlags & kTableMultipleSelection)
	{
		while (widget)
		{
			unsigned_int32 state = widget->GetWidgetState();
			if (!(state & kWidgetSelected))
			{
				change = true;
				widget->SetWidgetState(state | kWidgetSelected);
			}

			widget = widget->Next();
		}
	}
	else
	{
		if (widget)
		{
			unsigned_int32 state = widget->GetWidgetState();
			if (!(state & kWidgetSelected))
			{
				change = true;
				widget->SetWidgetState(state | kWidgetSelected);
			}

			widget = widget->Next();
			while (widget)
			{
				state = widget->GetWidgetState();
				if (state & kWidgetSelected)
				{
					change = true;
					widget->SetWidgetState(state & ~kWidgetSelected);
				}

				widget = widget->Next();
			}
		}
	}

	SetTableUpdateFlags(kTableUpdateSelection);

	if (post & change)
	{
		PostWidgetEvent(WidgetEventData(kEventWidgetChange));
	}
}

void TableWidget::UnselectAllTableItems(bool post)
{
	bool change = false;

	Widget *widget = itemGroup.GetFirstSubnode();
	while (widget)
	{
		unsigned_int32 state = widget->GetWidgetState();
		if (state & kWidgetSelected)
		{
			change = true;
			widget->SetWidgetState(state & ~kWidgetSelected);
		}

		widget = widget->Next();
	}

	SetTableUpdateFlags(kTableUpdateSelection);

	if (post & change)
	{
		PostWidgetEvent(WidgetEventData(kEventWidgetChange));
	}
}

void TableWidget::SetDisplayRow(int32 row)
{
	displayRowIndex = MaxZero(Min(row, GetTableRowCount() - displayRowCount));
	SetTableUpdateFlags(kTableUpdateVisibility);
}


MultipaneWidget::MultipaneWidget() :
		RenderableWidget(kWidgetMultipane, kRenderIndexedTriangles),
		multipaneVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		multipaneIndexBuffer(kVertexBufferIndex | kVertexBufferDynamic),
		borderVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		borderColorAttribute(kAttributeMutable),
		borderTextureAttribute(&LineWidget::lineTextureHeader, LineWidget::lineTextureImage),
		borderRenderable(kRenderQuads)
{
	multipaneSelection = kWidgetValueNone;
	colorOverrideFlags = 0;
	borderColor.Set(0.0F, 0.0F, 0.0F);
	hiliteColor.Set(0.0F, 0.0F, 0.0F);
	fontName[0] = 0;

	paneCount = 0;
	preprocessFlag = false;

	SetActiveUpdateFlags(GetActiveUpdateFlags() | kUpdateStructure);
	SetWidgetUsage(kWidgetTrackInhibit);

	SetDefaultColorType(kWidgetColorBackground);
	RenderableWidget::SetWidgetColor(ColorRGBA(0.75F, 0.75F, 0.75F, 1.0F));
}

MultipaneWidget::MultipaneWidget(const Vector2D& size, const char *font) :
		RenderableWidget(kWidgetMultipane, kRenderIndexedTriangles, size),
		multipaneVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		multipaneIndexBuffer(kVertexBufferIndex | kVertexBufferDynamic),
		borderVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		borderColorAttribute(kAttributeMutable),
		borderTextureAttribute(&LineWidget::lineTextureHeader, LineWidget::lineTextureImage),
		borderRenderable(kRenderQuads)
{
	multipaneSelection = kWidgetValueNone;
	colorOverrideFlags = 0;
	borderColor.Set(0.0F, 0.0F, 0.0F);
	hiliteColor.Set(0.0F, 0.0F, 0.0F);
	fontName = font;

	paneCount = 0;
	preprocessFlag = false;

	SetActiveUpdateFlags(GetActiveUpdateFlags() | kUpdateStructure);
	SetWidgetUsage(kWidgetTrackInhibit);

	SetDefaultColorType(kWidgetColorBackground);
	RenderableWidget::SetWidgetColor(ColorRGBA(0.75F, 0.75F, 0.75F, 1.0F));
}

MultipaneWidget::MultipaneWidget(const MultipaneWidget& multipaneWidget) :
		RenderableWidget(multipaneWidget),
		multipaneVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		multipaneIndexBuffer(kVertexBufferIndex | kVertexBufferDynamic),
		borderVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		borderColorAttribute(kAttributeMutable),
		borderTextureAttribute(&LineWidget::lineTextureHeader, LineWidget::lineTextureImage),
		borderRenderable(kRenderQuads)
{
	multipaneSelection = multipaneWidget.multipaneSelection;
	colorOverrideFlags = multipaneWidget.colorOverrideFlags;
	borderColor = multipaneWidget.borderColor;
	hiliteColor = multipaneWidget.hiliteColor;
	fontName = multipaneWidget.fontName;

	paneCount = 0;
	preprocessFlag = false;

	const Widget *widget = multipaneWidget.paneGroup.GetFirstSubnode();
	while (widget)
	{
		AppendPane(widget->Clone());
		widget = widget->Next();
	}

	SetActiveUpdateFlags(GetActiveUpdateFlags() | kUpdateStructure);
	SetWidgetUsage(kWidgetTrackInhibit);
}

MultipaneWidget::~MultipaneWidget()
{
}

Widget *MultipaneWidget::Replicate(void) const
{
	return (new MultipaneWidget(*this));
}

void MultipaneWidget::Pack(Packer& data, unsigned_int32 packFlags) const
{
	RenderableWidget::Pack(data, packFlags);

	data << ChunkHeader('SLCT', 4);
	data << multipaneSelection;

	data << ChunkHeader('OVER', 4);
	data << colorOverrideFlags;

	data << ChunkHeader('BRDC', sizeof(ColorRGBA));
	data << borderColor;

	if (colorOverrideFlags & kWidgetOverrideHiliteColor)
	{
		data << ChunkHeader('HILC', sizeof(ColorRGBA));
		data << hiliteColor;
	}

	PackHandle handle = data.BeginChunk('FONT');
	data << fontName;
	data.EndChunk(handle);

	const Widget *widget = paneGroup.GetFirstSubnode();
	while (widget)
	{
		handle = data.BeginChunk('PANE');
		widget->Pack(data, packFlags);
		data.EndChunk(handle);

		widget = widget->Next();
	}

	data << TerminatorChunk;
}

void MultipaneWidget::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	RenderableWidget::Unpack(data, unpackFlags);
	UnpackChunkList<MultipaneWidget>(data, unpackFlags);
}

bool MultipaneWidget::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'SLCT':

			data >> multipaneSelection;
			return (true);

		case 'OVER':

			data >> colorOverrideFlags;
			return (true);

		case 'BRDC':

			data >> borderColor;
			return (true);

		case 'HILC':

			data >> hiliteColor;
			return (true);

		case 'FONT':

			data >> fontName;
			return (true);

		case 'PANE':
		{
			TextWidget *widget = new TextWidget;
			widget->Unpack(data, unpackFlags);
			AppendPane(widget);
			return (true);
		}
	}

	return (false);
}

void *MultipaneWidget::BeginSettingsUnpack(void)
{
	paneGroup.PurgeSubtree();
	paneCount = 0;

	colorOverrideFlags = 0;
	return (RenderableWidget::BeginSettingsUnpack());
}

int32 MultipaneWidget::GetSettingCount(void) const
{
	return (RenderableWidget::GetSettingCount() + 6);
}

Setting *MultipaneWidget::GetSetting(int32 index) const
{
	int32 count = RenderableWidget::GetSettingCount();
	if (index < count)
	{
		return (RenderableWidget::GetSetting(index));
	}

	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetMultipane, 'SETT'));
		return (new HeadingSetting(kWidgetMultipane, title));
	}

	if (index == count + 1)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetMultipane, 'FONT'));
		const char *picker = table->GetString(StringID('WDGT', kWidgetMultipane, 'PICK'));
		return (new ResourceSetting('PFNT', fontName, title, picker, FontResource::GetDescriptor()));
	}

	if (index == count + 2)
	{
		String<>	string;

		const Widget *widget = paneGroup.GetFirstSubnode();
		while (widget)
		{
			if (string[0] != 0)
			{
				string += ';';
			}

			string += static_cast<const TextWidget *>(widget)->GetText();

			widget = widget->Next();
		}

		const char *title = table->GetString(StringID('WDGT', kWidgetMultipane, 'PANE'));
		return (new TextSetting('PANE', string, title, 1023));
	}

	if (index == count + 3)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetMultipane, 'SLCT'));
		return (new TextSetting('PSLT', Text::IntegerToString(Max(multipaneSelection, -1)), title, 2, &EditTextWidget::SignedNumberFilter));
	}

	if (index == count + 4)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetMultipane, 'BRDC'));
		const char *picker = table->GetString(StringID('WDGT', kWidgetMultipane, 'BRDP'));
		return (new ColorSetting('MPBD', borderColor, title, picker, kColorPickerAlpha));
	}

	if (index == count + 5)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetMultipane, 'HILC'));
		const char *picker = table->GetString(StringID('WDGT', kWidgetMultipane, 'HILP'));
		return (new CheckColorSetting('MPHC', ((colorOverrideFlags & kWidgetOverrideHiliteColor) != 0), hiliteColor, title, picker, kColorPickerAlpha));
	}

	return (nullptr);
}

void MultipaneWidget::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'PFNT')
	{
		fontName = static_cast<const ResourceSetting *>(setting)->GetResourceName();
	}
	else if (identifier == 'PANE')
	{
		paneCount = 0;
		paneGroup.PurgeSubtree();
		Invalidate();

		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		if (text[0] != 0)
		{
			for (;;)
			{
				int32 len = Text::FindChar(text, ';');
				if (len >= 0)
				{
					AppendPane(String<>(text, len));
					text += len + 1;
				}
				else
				{
					AppendPane(text);
					break;
				}
			}
		}
	}
	else if (identifier == 'PSLT')
	{
		int32 selection = Text::StringToInteger(static_cast<const TextSetting *>(setting)->GetText());
		if (selection < 0)
		{
			selection = kWidgetValueNone;
		}

		if (!GetManipulator())
		{
			SetSelection(selection);
		}
		else
		{
			multipaneSelection = selection;
		}
	}
	else if (identifier == 'MPBD')
	{
		borderColor = static_cast<const ColorSetting *>(setting)->GetColor();
	}
	else if (identifier == 'MPHC')
	{
		const CheckColorSetting *checkColorSetting = static_cast<const CheckColorSetting *>(setting);
		if (checkColorSetting->GetCheckValue() != 0)
		{
			colorOverrideFlags |= kWidgetOverrideHiliteColor;
			hiliteColor = checkColorSetting->GetColor();
		}
		else
		{
			colorOverrideFlags &= ~kWidgetOverrideHiliteColor;
			SetDefaultHiliteColor();
		}
	}
	else
	{
		RenderableWidget::SetSetting(setting);
	}
}

void MultipaneWidget::SetSelection(int32 selection, bool post)
{
	if (multipaneSelection != selection)
	{
		multipaneSelection = selection;
		SetBuildFlag();

		if (post)
		{
			PostWidgetEvent(WidgetEventData(kEventWidgetChange));
		}
	}
}

const ColorRGBA& MultipaneWidget::GetWidgetColor(WidgetColorType type) const
{
	if (type == kWidgetColorBorder)
	{
		return (borderColor);
	}

	if (type == kWidgetColorHilite)
	{
		return (hiliteColor);
	}

	return (RenderableWidget::GetWidgetColor(type));
}

void MultipaneWidget::SetWidgetColor(const ColorRGBA& color, WidgetColorType type)
{
	if (type == kWidgetColorBorder)
	{
		borderColor = color;
	}
	else if (type == kWidgetColorHilite)
	{
		hiliteColor = color;
		colorOverrideFlags |= kWidgetOverrideHiliteColor;
	}

	RenderableWidget::SetWidgetColor(color, type);
}

void MultipaneWidget::SetWidgetAlpha(float alpha, WidgetColorType type)
{
	if (type == kWidgetColorBorder)
	{
		borderColor.alpha = alpha;
	}
	else if (type == kWidgetColorHilite)
	{
		hiliteColor.alpha = alpha;
		colorOverrideFlags |= kWidgetOverrideHiliteColor;
	}

	RenderableWidget::SetWidgetAlpha(alpha, type);
}

void MultipaneWidget::SetDynamicWidgetColor(const ColorRGBA& color, WidgetColorType type)
{
	if ((type == kWidgetColorDefault) || (type == kWidgetColorBackground))
	{
		dynamicBackgroundColor = color;
		SetBuildFlag();
	}
	else if (type == kWidgetColorBorder)
	{
		borderColorAttribute.SetDiffuseColor(color);
	}
	else if (type == kWidgetColorHilite)
	{
		dynamicHiliteColor = color;
		SetBuildFlag();
	}
	else
	{
		RenderableWidget::SetDynamicWidgetColor(color, type);
	}
}

void MultipaneWidget::SetDynamicWidgetAlpha(float alpha, WidgetColorType type)
{
	if ((type == kWidgetColorDefault) || (type == kWidgetColorBackground))
	{
		dynamicBackgroundColor.alpha = alpha;
		SetBuildFlag();
	}
	else if (type == kWidgetColorBorder)
	{
		borderColorAttribute.SetDiffuseAlpha(alpha);
	}
	else if (type == kWidgetColorHilite)
	{
		dynamicHiliteColor.alpha = alpha;
		SetBuildFlag();
	}
	else
	{
		RenderableWidget::SetDynamicWidgetAlpha(alpha, type);
	}
}

void MultipaneWidget::SetDefaultHiliteColor(void)
{
	hiliteColor.Set(TheInterfaceMgr->GetInterfaceColor(kInterfaceColorHilite).GetColorRGB() * 0.75F, 1.0F);
}

void MultipaneWidget::Preprocess(void)
{
	if (!(colorOverrideFlags & kWidgetOverrideHiliteColor))
	{
		SetDefaultHiliteColor();
	}

	dynamicBackgroundColor = GetWidgetColor();
	dynamicHiliteColor = hiliteColor;

	paneGroup.SetWidgetState(paneGroup.GetWidgetState() | kWidgetNonpersistent);
	AppendSubnode(&paneGroup);

	RenderableWidget::Preprocess();

	SetMaterialObjectPointer(TheInterfaceMgr->GetMaterialObjectPointer());

	SetVertexBuffer(kVertexBufferAttributeArray, &multipaneVertexBuffer, sizeof(MultipaneVertex));
	SetVertexBuffer(kVertexBufferIndexArray, &multipaneIndexBuffer);
	SetVertexAttributeArray(kArrayPosition, 0, 2);
	SetVertexAttributeArray(kArrayColor, sizeof(Point2D), 4);
	SetVertexAttributeArray(kArrayTexcoord, sizeof(Point2D) + sizeof(ColorRGBA), 2);

	InitRenderable(&borderRenderable);
	borderRenderable.SetVertexCount(20);
	borderRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &borderVertexBuffer, sizeof(BorderVertex));
	borderRenderable.SetVertexAttributeArray(kArrayPosition, 0, 2);
	borderRenderable.SetVertexAttributeArray(kArrayTexcoord, sizeof(Point2D), 2);
	borderVertexBuffer.Establish(sizeof(BorderVertex) * 20);

	borderAttributeList.Append(&borderColorAttribute);
	borderAttributeList.Append(&borderTextureAttribute);
	borderRenderable.SetMaterialAttributeList(&borderAttributeList);

	preprocessFlag = true;
}

void MultipaneWidget::HandleStructureUpdate(void)
{
	if (paneCount != 0)
	{
		SetBuildFlag();
	}

	int32 vertexCount = paneCount * 15;
	int32 primitiveCount = paneCount * 12;

	SetVertexCount(vertexCount);
	multipaneVertexBuffer.Establish(sizeof(MultipaneVertex) * vertexCount);
	multipaneIndexBuffer.Establish(sizeof(Triangle) * primitiveCount);
	InvalidateVertexData();

	int32 count = 0;
	float x = 11.0F;
	float height = 0.0F;

	TextWidget *widget = static_cast<TextWidget *>(paneGroup.GetFirstSubnode());
	while (widget)
	{
		if (widget->Visible())
		{
			count++;

			float w = widget->GetFormattedTextWidth();
			float h = widget->GetFont()->GetFontHeader()->fontHeight + 2.0F;
			height = Fmax(height, h);

			widget->SetWidgetSize(Vector2D(w, h));
			widget->SetWidgetPosition(Point3D(x, -h, 0.0F));
			x += w + 15.0F;
		}

		widget = static_cast<TextWidget *>(widget->Widget::Next());
	}

	visiblePaneCount = count;
	titleHeight = height;

	paneGroup.Invalidate();
}

bool MultipaneWidget::CalculateBoundingBox(Box2D *box) const
{
	box->min.Set(0.0F, -titleHeight - 2.0F);
	box->max.Set(GetWidgetSize().x, 0.0F);
	return (true);
}

void MultipaneWidget::BuildPane(const Widget *widget, volatile MultipaneVertex *restrict vertex, volatile Triangle *restrict triangle, int32 index, float y)
{
	vertex += index * 15;

	float x = widget->GetWidgetPosition().x - 7.0F;
	float w = widget->GetWidgetSize().x + 14.0F;

	float s = Fmin(w - 8.0F, 56.0F) * 0.001953125F + 0.7578125F;
	float t = 0.2421875F - Fmin(-y - 4.0F, 56.0F) * 0.001953125F;

	vertex[0].position.Set(x - 8.0F, y - 4.0F);
	vertex[0].texcoord.Set(0.734375F, 0.2578125F);
	vertex[1].position.Set(x + 4.0F, y - 4.0F);
	vertex[1].texcoord.Set(0.7578125F, 0.2578125F);
	vertex[2].position.Set(x + w - 4.0F, y - 4.0F);
	vertex[2].texcoord.Set(s, 0.2578125F);
	vertex[3].position.Set(x - 8.0F, y + 4.0F);
	vertex[3].texcoord.Set(0.734375F, 0.2421875F);
	vertex[4].position.Set(x + 4.0F, y + 4.0F);
	vertex[4].texcoord.Set(0.7578125F, 0.2421875F);
	vertex[5].position.Set(x + w - 4.0F, y + 4.0F);
	vertex[5].texcoord.Set(s, 0.2421875F);
	vertex[6].position.Set(x - 8.0F, 0.0F);
	vertex[6].texcoord.Set(0.734375F, t);
	vertex[7].position.Set(x + 4.0F, 0.0F);
	vertex[7].texcoord.Set(0.7578125F, t);
	vertex[8].position.Set(x + w - 4.0F, 0.0F);
	vertex[8].texcoord.Set(s, t);

	vertex[9].position.Set(x + w - 4.0F, y - 4.0F);
	vertex[9].texcoord.Set(0.8671875F, 0.2578125F);
	vertex[10].position.Set(x + w + 8.0F, y - 4.0F);
	vertex[10].texcoord.Set(0.890625F, 0.2578125F);
	vertex[11].position.Set(x + w - 4.0F, y + 4.0F);
	vertex[11].texcoord.Set(0.8671875F, 0.2421875F);
	vertex[12].position.Set(x + w + 8.0F, y + 4.0F);
	vertex[12].texcoord.Set(0.890625F, 0.2421875F);
	vertex[13].position.Set(x + w - 4.0F, 0.0F);
	vertex[13].texcoord.Set(0.8671875F, t);
	vertex[14].position.Set(x + w + 8.0F, 0.0F);
	vertex[14].texcoord.Set(0.890625F, t);

	int32 i = index * 15;
	triangle += index * 12;

	triangle[0].Set(i, i + 3, i + 4);
	triangle[1].Set(i, i + 4, i + 1);
	triangle[2].Set(i + 1, i + 4, i + 5);
	triangle[3].Set(i + 1, i + 5, i + 2);
	triangle[4].Set(i + 3, i + 6, i + 7);
	triangle[5].Set(i + 3, i + 7, i + 4);
	triangle[6].Set(i + 4, i + 7, i + 8);
	triangle[7].Set(i + 4, i + 8, i + 5);
	triangle[8].Set(i + 9, i + 11, i + 12);
	triangle[9].Set(i + 9, i + 12, i + 10);
	triangle[10].Set(i + 11, i + 13, i + 14);
	triangle[11].Set(i + 11, i + 14, i + 12);
}

void MultipaneWidget::Build(void)
{
	float selectLeft = 1.0F;
	float selectRight = 1.0F;

	SetPrimitiveCount(visiblePaneCount * 12);
	if (visiblePaneCount != 0)
	{
		volatile MultipaneVertex *restrict multipaneVertex = multipaneVertexBuffer.BeginUpdate<MultipaneVertex>();
		volatile Triangle *restrict multipaneTriangle = multipaneIndexBuffer.BeginUpdate<Triangle>();

		int32 count = 0;
		int32 index = paneCount - 1;
		float y = -titleHeight - 2.0F;

		Widget *widget = paneGroup.GetLastSubnode();
		while (widget)
		{
			if (widget->Visible())
			{
				if (index != multipaneSelection)
				{
					BuildPane(widget, multipaneVertex, multipaneTriangle, count, y);
					count++;
				}
				else
				{
					selectLeft = widget->GetWidgetPosition().x - 7.0F;
					selectRight = selectLeft + widget->GetWidgetSize().x + 14.0F;

					BuildPane(widget, multipaneVertex, multipaneTriangle, visiblePaneCount - 1, y);
				}
			}

			index--;
			widget = widget->Previous();
		}

		int32 totalCount = visiblePaneCount * 15;
		int32 unselectedCount = totalCount - (multipaneSelection != kWidgetValueNone) * 15;

		for (machine a = 0; a < unselectedCount; a++)
		{
			multipaneVertex[a].color = dynamicBackgroundColor;
		}

		for (machine a = unselectedCount; a < totalCount; a++)
		{
			multipaneVertex[a].color = dynamicHiliteColor;
		}

		multipaneIndexBuffer.EndUpdate();
		multipaneVertexBuffer.EndUpdate();
	}

	volatile BorderVertex *restrict borderVertex = borderVertexBuffer.BeginUpdate<BorderVertex>();

	float width = GetWidgetSize().x;
	float height = GetWidgetSize().y;
	float w = (width + 4.0F) * 0.125F;
	float l = (selectLeft + 2.0F) * 0.125F;
	float r = (selectRight + 2.0F) * 0.125F;
	float h = (height + 4.0F) * 0.125F;

	borderVertex[0].position.Set(-2.0F, -2.0F);
	borderVertex[0].texcoord.Set(0.0F, 0.0F);
	borderVertex[1].position.Set(1.0F, 1.0F);
	borderVertex[1].texcoord.Set(0.375F, 0.1875F);
	borderVertex[2].position.Set(selectLeft, 1.0F);
	borderVertex[2].texcoord.Set(l, 0.1875F);
	borderVertex[3].position.Set(selectLeft, -2.0F);
	borderVertex[3].texcoord.Set(l, 0.0F);

	borderVertex[4].position.Set(selectRight, -2.0F);
	borderVertex[4].texcoord.Set(r, 0.0F);
	borderVertex[5].position.Set(selectRight, 1.0F);
	borderVertex[5].texcoord.Set(r, 0.1875F);
	borderVertex[6].position.Set(width - 1.0F, 1.0F);
	borderVertex[6].texcoord.Set(w - 0.375F, 0.1875F);
	borderVertex[7].position.Set(width + 2.0F, -2.0F);
	borderVertex[7].texcoord.Set(w, 0.0F);

	borderVertex[8].position.Set(1.0F, height - 1.0F);
	borderVertex[8].texcoord.Set(0.375F, 0.0F);
	borderVertex[9].position.Set(-2.0F, height + 2.0F);
	borderVertex[9].texcoord.Set(0.0F, 0.1875F);
	borderVertex[10].position.Set(width + 2.0F, height + 2.0F);
	borderVertex[10].texcoord.Set(w, 0.1875F);
	borderVertex[11].position.Set(width - 1.0F, height - 1.0F);
	borderVertex[11].texcoord.Set(w - 0.375F, 0.0F);

	borderVertex[12].position.Set(-2.0F, -2.0F);
	borderVertex[12].texcoord.Set(0.0F, 0.0F);
	borderVertex[13].position.Set(-2.0F, height + 2.0F);
	borderVertex[13].texcoord.Set(h, 0.0F);
	borderVertex[14].position.Set(1.0F, height - 1.0F);
	borderVertex[14].texcoord.Set(h - 0.375F, 0.1875F);
	borderVertex[15].position.Set(1.0F, 1.0F);
	borderVertex[15].texcoord.Set(0.375F, 0.1875F);

	borderVertex[16].position.Set(width - 1.0F, 1.0F);
	borderVertex[16].texcoord.Set(0.375F, 0.0F);
	borderVertex[17].position.Set(width - 1.0F, height - 1.0F);
	borderVertex[17].texcoord.Set(h - 0.375F, 0.0F);
	borderVertex[18].position.Set(width + 2.0F, height + 2.0F);
	borderVertex[18].texcoord.Set(h, 0.1875F);
	borderVertex[19].position.Set(width + 2.0F, -2.0F);
	borderVertex[19].texcoord.Set(0.0F, 0.1875F);

	borderVertexBuffer.EndUpdate();

	borderColorAttribute.SetDiffuseColor(borderColor);
}

void MultipaneWidget::Render(List<Renderable> *renderList)
{
	RenderableWidget::Render(renderList);
	renderList->Append(&borderRenderable);
}

void MultipaneWidget::HandleMouseEvent(const PanelMouseEventData *eventData)
{
	if (eventData->eventType == kEventMouseDown)
	{
		float xpos = eventData->mousePosition.x;

		int32 index = 0;
		const Widget *widget = paneGroup.GetFirstSubnode();
		while (widget)
		{
			if (widget->Visible())
			{
				float x = widget->GetWidgetPosition().x - 7.0F;
				float w = widget->GetWidgetSize().x + 14.0F;

				if ((xpos >= x) && (xpos <= x + w))
				{
					if (multipaneSelection != index)
					{
						SetSelection(index);
						PostWidgetEvent(WidgetEventData(kEventWidgetChange));
					}

					break;
				}
			}

			index++;
			widget = widget->Next();
		}
	}
}

void MultipaneWidget::PrependPane(Widget *widget)
{
	paneGroup.PrependSubnode(widget);

	widget->Disable();

	if (preprocessFlag)
	{
		widget->Preprocess();
	}

	paneCount++;
	Invalidate();
}

void MultipaneWidget::AppendPane(Widget *widget)
{
	paneGroup.AppendSubnode(widget);

	widget->Disable();

	if (preprocessFlag)
	{
		widget->Preprocess();
	}

	paneCount++;
	Invalidate();
}

void MultipaneWidget::AppendPane(const char *title)
{
	AppendPane(new TextWidget(title, fontName));
}

void MultipaneWidget::PrependPane(const char *title)
{
	PrependPane(new TextWidget(title, fontName));
}

void MultipaneWidget::DeletePane(int32 index)
{
	if (multipaneSelection == index)
	{
		multipaneSelection = kWidgetValueNone;
	}

	Widget *widget = paneGroup.GetFirstSubnode();
	while (widget)
	{
		if (index == 0)
		{
			delete widget;
			Invalidate();
			SetBuildFlag();
			break;
		}

		index--;
		widget = widget->Next();
	}
}

void MultipaneWidget::ShowPane(int32 index)
{
	Widget *widget = paneGroup.GetFirstSubnode();
	while (widget)
	{
		if (index == 0)
		{
			widget->Show();
			Invalidate();
			SetBuildFlag();
			break;
		}

		index--;
		widget = widget->Next();
	}
}

void MultipaneWidget::HidePane(int32 index)
{
	if (multipaneSelection == index)
	{
		multipaneSelection = kWidgetValueNone;
	}

	Widget *widget = paneGroup.GetFirstSubnode();
	while (widget)
	{
		if (index == 0)
		{
			widget->Hide();
			Invalidate();
			SetBuildFlag();
			break;
		}

		index--;
		widget = widget->Next();
	}
}

const char *MultipaneWidget::GetPaneTitle(int32 index) const
{
	const Widget *widget = paneGroup.GetFirstSubnode();
	while (widget)
	{
		if (index == 0)
		{
			return (static_cast<const TextWidget *>(widget)->GetText());
		}

		index--;
		widget = widget->Next();
	}

	return (nullptr);
}


DividerWidget::DividerWidget() :
		RenderableWidget(kWidgetDivider, kRenderTriangleStrip),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		colorAttribute(TheInterfaceMgr->GetInterfaceColor(kInterfaceColorWindowFrame))
{
}

DividerWidget::DividerWidget(const Vector2D& size) :
		RenderableWidget(kWidgetDivider, kRenderTriangleStrip, size),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		colorAttribute(TheInterfaceMgr->GetInterfaceColor(kInterfaceColorWindowFrame))
{
}

DividerWidget::DividerWidget(const DividerWidget& dividerWidget) :
		RenderableWidget(dividerWidget),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		colorAttribute(dividerWidget.colorAttribute)
{
}

DividerWidget::~DividerWidget()
{
}

Widget *DividerWidget::Replicate(void) const
{
	return (new DividerWidget(*this));
}

void DividerWidget::Preprocess(void)
{
	RenderableWidget::Preprocess();

	SetVertexCount(4);
	SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(DividerVertex));
	SetVertexAttributeArray(kArrayPosition, 0, 2);
	SetVertexAttributeArray(kArrayTexcoord, sizeof(Point2D), 2);
	vertexBuffer.Establish(sizeof(DividerVertex) * 4);

	attributeList.Append(&colorAttribute);
	SetMaterialAttributeList(&attributeList);
	SetMaterialObjectPointer(TheInterfaceMgr->GetMaterialObjectPointer());
}

void DividerWidget::Build(void)
{
	volatile DividerVertex *restrict vertex = vertexBuffer.BeginUpdate<DividerVertex>();

	float w = GetWidgetSize().x;

	vertex[0].position.Set(0.0F, 6.0F);
	vertex[0].texcoord.Set(0.57421875F, 0.3125F);

	vertex[1].position.Set(w, 6.0F);
	vertex[1].texcoord.Set(0.67578125F, 0.3125F);

	vertex[2].position.Set(0.0F, 0.0F);
	vertex[2].texcoord.Set(0.57421875F, 0.32421875F);

	vertex[3].position.Set(w, 0.0F);
	vertex[3].texcoord.Set(0.67578125F, 0.32421875F);

	vertexBuffer.EndUpdate();
}


WindowFrameWidget::WindowFrameWidget() :
		RenderableWidget(kWidgetWindowFrame, kRenderIndexedTriangles),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		colorAttribute(kAttributeMutable)
{
	Initialize();
}

WindowFrameWidget::WindowFrameWidget(const Vector2D& size) :
		RenderableWidget(kWidgetWindowFrame, kRenderIndexedTriangles, size),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		colorAttribute(kAttributeMutable)
{
	Initialize();
}

WindowFrameWidget::~WindowFrameWidget()
{
	indexBuffer.Release();
}

void WindowFrameWidget::Initialize(void)
{
	if (indexBuffer.Retain() == 1)
	{
		static const Triangle frameTriangle[20] =
		{
			{{ 0,  1,  2}}, {{ 0,  2,  3}}, {{ 3,  2,  5}}, {{ 3,  5,  4}},
			{{ 4,  5,  6}}, {{ 4,  6,  7}}, {{ 1,  9, 12}}, {{ 1, 12,  8}},
			{{ 9, 10, 11}}, {{ 9, 11, 12}}, {{12, 11, 15}}, {{12, 15, 14}},
			{{14, 15, 16}}, {{14, 16, 17}}, {{13, 14, 17}}, {{13, 17,  6}},
			{{18, 19, 20}}, {{20, 19, 21}}, {{22, 20, 23}}, {{23, 20, 21}}
		};

		indexBuffer.Establish(sizeof(Triangle) * 20, frameTriangle);
	}

	RenderableWidget::SetWidgetColor(TheInterfaceMgr->GetInterfaceColor(kInterfaceColorWindowFrame));
}

bool WindowFrameWidget::CalculateBoundingBox(Box2D *box) const
{
	box->min.Set(-5.0F, -22.0F);
	box->max.Set(GetWidgetSize().x + 5.0F, GetWidgetSize().y + 5.0F);
	return (true);
}

WidgetPart WindowFrameWidget::TestPosition(const Point3D& position) const
{
	if (position.y < 0.0F)
	{
		return (kWidgetPartTitle);
	}

	Window *window = GetOwningWindow();
	if ((window) && (window->GetWindowFlags() & kWindowResizable))
	{
		const Vector2D& size = GetWidgetSize();
		if ((position.x >= size.x - 3.0F) && (position.y >= size.y))
		{
			return (kWidgetPartResize);
		}

		if ((position.x >= size.x) && (position.y >= size.y - 3.0F))
		{
			return (kWidgetPartResize);
		}
	}

	return (kWidgetPartNone);
}

void WindowFrameWidget::Preprocess(void)
{
	RenderableWidget::Preprocess();

	SetVertexCount(24);
	SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(FrameVertex));
	SetVertexAttributeArray(kArrayPosition, 0, 2);
	SetVertexAttributeArray(kArrayTexcoord, sizeof(Point2D), 2);
	vertexBuffer.Establish(sizeof(FrameVertex) * 24);

	const Window *window = GetOwningWindow();
	if ((window) && (window->GetWindowFlags() & kWindowResizable))
	{
		SetPrimitiveCount(20);
	}
	else
	{
		SetPrimitiveCount(16);
	}

	SetVertexBuffer(kVertexBufferIndexArray, &indexBuffer);

	attributeList.Append(&colorAttribute);
	SetMaterialAttributeList(&attributeList);
	SetMaterialObjectPointer(TheInterfaceMgr->GetMaterialObjectPointer());
}

void WindowFrameWidget::Build(void)
{
	volatile FrameVertex *restrict vertex = vertexBuffer.BeginUpdate<FrameVertex>();

	float width = GetWidgetSize().x;
	float height = GetWidgetSize().y;

	vertex[0].position.Set(-16.0F, -30.0F);
	vertex[1].position.Set(-16.0F, 0.0F);
	vertex[2].position.Set(10.0F, 0.0F);
	vertex[3].position.Set(10.0F, -30.0F);
	vertex[4].position.Set(width - 10.0F, -30.0F);
	vertex[5].position.Set(width - 10.0F, 0.0F);
	vertex[6].position.Set(width + 16.0F, 0.0F);
	vertex[7].position.Set(width + 16.0F, -30.0F);

	vertex[0].texcoord.Set(0.54296875F, 0.44921875F);
	vertex[1].texcoord.Set(0.54296875F, 0.390625F);
	vertex[2].texcoord.Set(0.59375F, 0.390625F);
	vertex[3].texcoord.Set(0.59375F, 0.44921875F);
	vertex[4].texcoord.Set(0.65625F, 0.44921875F);
	vertex[5].texcoord.Set(0.65625F, 0.390625F);
	vertex[6].texcoord.Set(0.70703125F, 0.390625F);
	vertex[7].texcoord.Set(0.70703125F, 0.44921875F);

	vertex[8].position.Set(0.0F, 0.0F);
	vertex[9].position.Set(-16.0F, height);
	vertex[10].position.Set(-16.0F, height + 23.0F);
	vertex[11].position.Set(0.0F, height + 23.0F);
	vertex[12].position.Set(0.0F, height);

	vertex[8].texcoord.Set(0.57421875F, 0.390625F);
	vertex[9].texcoord.Set(0.54296875F, 0.32421875F);
	vertex[10].texcoord.Set(0.54296875F, 0.279296875F);
	vertex[11].texcoord.Set(0.57421875F, 0.279296875F);
	vertex[12].texcoord.Set(0.57421875F, 0.32421875F);

	vertex[13].position.Set(width, 0.0F);
	vertex[14].position.Set(width, height);
	vertex[15].position.Set(width, height + 23.0F);
	vertex[16].position.Set(width + 16.0F, height + 23.0F);
	vertex[17].position.Set(width + 16.0F, height);

	vertex[13].texcoord.Set(0.67578125F, 0.390625F);
	vertex[14].texcoord.Set(0.67578125F, 0.32421875F);
	vertex[15].texcoord.Set(0.67578125F, 0.279296875F);
	vertex[16].texcoord.Set(0.70703125F, 0.279296875F);
	vertex[17].texcoord.Set(0.70703125F, 0.32421875F);

	vertex[18].position.Set(width - 3.0F, height + 2.0F);
	vertex[19].position.Set(width - 3.0F, height + 4.0F);
	vertex[20].position.Set(width + 2.0F, height + 2.0F);
	vertex[21].position.Set(width + 4.0F, height + 4.0F);
	vertex[22].position.Set(width + 2.0F, height - 3.0F);
	vertex[23].position.Set(width + 4.0F, height - 3.0F);

	for (machine a = 18; a < 24; a++)
	{
		vertex[a].texcoord.Set(0.625F, 0.359375F);
	}

	vertexBuffer.EndUpdate();

	colorAttribute.SetDiffuseColor(RenderableWidget::GetWidgetColor());
}

void WindowFrameWidget::HandleMouseEvent(const PanelMouseEventData *eventData)
{
	Window *window = GetOwningWindow();
	if (window)
	{
		WidgetPart widgetPart = eventData->widgetPart;
		EventType eventType = eventData->eventType;

		if (widgetPart == kWidgetPartTitle)
		{
			if (eventType == kEventMouseDown)
			{
				dragPosition = eventData->mousePosition;
			}
			else if (eventType == kEventMouseMoved)
			{
				Vector3D delta = eventData->mousePosition - dragPosition;

				float y = window->GetWorldPosition().y;
				if (y + delta.y < 26.0F)
				{
					delta.y = 26.0F - y;
				}

				float bottom = TheInterfaceMgr->GetDesktopSize().y;
				if (y + delta.y > bottom)
				{
					delta.y = bottom - y;
				}

				window->SetWidgetPosition(window->GetWidgetPosition() + delta);
				window->Invalidate();

				Window *subwindow = window->GetFirstSubwindow();
				while (subwindow)
				{
					subwindow->SetWidgetPosition(subwindow->GetWidgetPosition() - delta);
					subwindow = subwindow->Next();
				}
			}
		}
		else if (widgetPart == kWidgetPartResize)
		{
			if (eventType == kEventMouseDown)
			{
				dragPosition = eventData->mousePosition;
				dragSize = window->GetWidgetSize();
			}
			else if (eventType == kEventMouseMoved)
			{
				Vector2D size = dragSize + (eventData->mousePosition.GetPoint2D() - dragPosition.GetPoint2D());
				size.x = Fmax(size.x, window->GetMinWindowSize().x);
				size.y = Fmax(size.y, window->GetMinWindowSize().y);

				size.y -= FmaxZero(GetWorldPosition().y + size.y - TheInterfaceMgr->GetDesktopSize().y);

				if (window->GetWindowFlags() & kWindowEvenSize)
				{
					size.x = PositiveFloor(size.x * 0.5F) * 2.0F;
					size.y = PositiveFloor(size.y * 0.5F) * 2.0F;
				}

				window->SetWidgetSize(size);
				window->Invalidate();
			}
		}
	}
}


BalloonFrameWidget::BalloonFrameWidget() :
		RenderableWidget(kWidgetBalloonFrame, kRenderIndexedTriangles),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		colorAttribute(kAttributeMutable)
{
	Initialize();
}

BalloonFrameWidget::BalloonFrameWidget(const Vector2D& size) :
		RenderableWidget(kWidgetBalloonFrame, kRenderIndexedTriangles, size),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		colorAttribute(kAttributeMutable)
{
	Initialize();
}

BalloonFrameWidget::~BalloonFrameWidget()
{
	indexBuffer.Release();
}

void BalloonFrameWidget::Initialize(void)
{
	if (indexBuffer.Retain() == 1)
	{
		static const Triangle frameTriangle[20] =
		{
			{{ 0,  3,  4}}, {{ 0,  4,  1}}, {{ 1,  4,  5}}, {{ 1,  5,  2}},
			{{ 3,  6,  7}}, {{ 3,  7,  4}}, {{ 4,  7,  8}}, {{ 4,  8,  5}},
			{{ 9, 11, 12}}, {{ 9, 12, 10}}, {{11, 13, 14}}, {{11, 14, 12}},
			{{15, 18, 19}}, {{15, 19, 16}}, {{16, 19, 20}}, {{16, 20, 17}},
			{{21, 23, 24}}, {{21, 24, 22}}, {{25, 26, 28}}, {{26, 27, 28}}
		};

		indexBuffer.Establish(sizeof(Triangle) * 20, frameTriangle);
	}

	wedgeLocation = 0;
	wedgeOffset = 0.0F;

	SetDefaultColorType(kWidgetColorBackground);
	RenderableWidget::SetWidgetColor(TheInterfaceMgr->GetInterfaceColor(kInterfaceColorBalloonBack));
}

void BalloonFrameWidget::Preprocess(void)
{
	RenderableWidget::Preprocess();

	SetVertexCount(29);
	SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(FrameVertex));
	SetVertexAttributeArray(kArrayPosition, 0, 2);
	SetVertexAttributeArray(kArrayTexcoord, sizeof(Point2D), 2);
	vertexBuffer.Establish(sizeof(FrameVertex) * 29);

	SetPrimitiveCount(20);
	SetVertexBuffer(kVertexBufferIndexArray, &indexBuffer);

	attributeList.Append(&colorAttribute);
	SetMaterialAttributeList(&attributeList);
	SetMaterialObjectPointer(TheInterfaceMgr->GetMaterialObjectPointer());
}

void BalloonFrameWidget::Build(void)
{
	volatile FrameVertex *restrict vertex = vertexBuffer.BeginUpdate<FrameVertex>();

	float width = GetWidgetSize().x;
	float height = GetWidgetSize().y;

	float s = Fmin(width - 8.0F, 56.0F) * 0.001953125F + 0.7578125F;
	float t = 0.2421875F - Fmin(height - 8.0F, 56.0F) * 0.001953125F;

	vertex[0].position.Set(-8.0F, -4.0F);
	vertex[1].position.Set(4.0F, -4.0F);
	vertex[2].position.Set(width - 4.0F, -4.0F);
	vertex[3].position.Set(-8.0F, 4.0F);
	vertex[4].position.Set(4.0F, 4.0F);
	vertex[5].position.Set(width - 4.0F, 4.0F);
	vertex[6].position.Set(-8.0F, height - 4.0F);
	vertex[7].position.Set(4.0F, height - 4.0F);
	vertex[8].position.Set(width - 4.0F, height - 4.0F);

	vertex[0].texcoord.Set(0.734375F, 0.2578125F);
	vertex[1].texcoord.Set(0.7578125F, 0.2578125F);
	vertex[2].texcoord.Set(s, 0.2578125F);
	vertex[3].texcoord.Set(0.734375F, 0.2421875F);
	vertex[4].texcoord.Set(0.7578125F, 0.2421875F);
	vertex[5].texcoord.Set(s, 0.2421875F);
	vertex[6].texcoord.Set(0.734375F, t);
	vertex[7].texcoord.Set(0.7578125F, t);
	vertex[8].texcoord.Set(s, t);

	vertex[9].position.Set(width - 4.0F, -4.0F);
	vertex[10].position.Set(width + 8.0F, -4.0F);
	vertex[11].position.Set(width - 4.0F, 4.0F);
	vertex[12].position.Set(width + 8.0F, 4.0F);
	vertex[13].position.Set(width - 4.0F, height - 4.0F);
	vertex[14].position.Set(width + 8.0F, height - 4.0F);

	vertex[9].texcoord.Set(0.8671875F, 0.2578125F);
	vertex[10].texcoord.Set(0.890625F, 0.2578125F);
	vertex[11].texcoord.Set(0.8671875F, 0.2421875F);
	vertex[12].texcoord.Set(0.890625F, 0.2421875F);
	vertex[13].texcoord.Set(0.8671875F, t);
	vertex[14].texcoord.Set(0.890625F, t);

	vertex[15].position.Set(-8.0F, height - 4.0F);
	vertex[16].position.Set(4.0F, height - 4.0F);
	vertex[17].position.Set(width - 4.0F, height - 4.0F);
	vertex[18].position.Set(-8.0F, height + 12.0F);
	vertex[19].position.Set(4.0F, height + 12.0F);
	vertex[20].position.Set(width - 4.0F, height + 12.0F);

	vertex[15].texcoord.Set(0.734375F, 0.1328125F);
	vertex[16].texcoord.Set(0.7578125F, 0.1328125F);
	vertex[17].texcoord.Set(s, 0.1328125F);
	vertex[18].texcoord.Set(0.734375F, 0.1015625F);
	vertex[19].texcoord.Set(0.7578125F, 0.1015625F);
	vertex[20].texcoord.Set(s, 0.1015625F);

	vertex[21].position.Set(width - 4.0F, height - 4.0F);
	vertex[22].position.Set(width + 8.0F, height - 4.0F);
	vertex[23].position.Set(width - 4.0F, height + 12.0F);
	vertex[24].position.Set(width + 8.0F, height + 12.0F);

	vertex[21].texcoord.Set(0.8671875F, 0.1328125F);
	vertex[22].texcoord.Set(0.890625F, 0.1328125F);
	vertex[23].texcoord.Set(0.8671875F, 0.1015625F);
	vertex[24].texcoord.Set(0.890625F, 0.1015625F);

	float offset = Clamp(wedgeOffset, -12.0F, width - 39.0F);
	if (wedgeLocation == 0)
	{
		vertex[25].position.Set(16.0F + offset, -10.0F);
		vertex[26].position.Set(16.0F + offset, 0.0F);
		vertex[27].position.Set(35.0F + offset, 0.0F);
		vertex[28].position.Set(35.0F + offset, -10.0F);

		vertex[25].texcoord.Set(0.98046875F, 0.212890625F);
		vertex[26].texcoord.Set(1.0F, 0.212890625F);
		vertex[27].texcoord.Set(1.0F, 0.25F);
		vertex[28].texcoord.Set(0.98046875F, 0.25F);
	}
	else
	{
		vertex[25].position.Set(16.0F + offset, height);
		vertex[26].position.Set(16.0F + offset, height + 10.0F);
		vertex[27].position.Set(35.0F + offset, height + 10.0F);
		vertex[28].position.Set(35.0F + offset, height);

		vertex[25].texcoord.Set(1.0F, 0.212890625F);
		vertex[26].texcoord.Set(0.98046875F, 0.212890625F);
		vertex[27].texcoord.Set(0.98046875F, 0.25F);
		vertex[28].texcoord.Set(1.0F, 0.25F);
	}

	vertexBuffer.EndUpdate();

	colorAttribute.SetDiffuseColor(RenderableWidget::GetWidgetColor());
}


PageFrameWidget::PageFrameWidget() :
		RenderableWidget(kWidgetPageFrame, kRenderIndexedTriangles),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		colorAttribute(kAttributeMutable)
{
	Initialize();
}

PageFrameWidget::PageFrameWidget(const Vector2D& size) :
		RenderableWidget(kWidgetPageFrame, kRenderIndexedTriangles, size),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		colorAttribute(kAttributeMutable)
{
	Initialize();
}

PageFrameWidget::~PageFrameWidget()
{
	indexBuffer.Release();
}

void PageFrameWidget::Initialize(void)
{
	if (indexBuffer.Retain() == 1)
	{
		static const Triangle frameTriangle[16] =
		{
			{{ 0,  1,  2}}, {{ 0,  2,  3}}, {{ 3,  2,  5}}, {{ 3,  5,  4}},
			{{ 4,  5,  6}}, {{ 4,  6,  7}}, {{ 1,  8, 11}}, {{ 1, 11,  2}},
			{{ 5, 12, 15}}, {{ 5, 15,  6}}, {{ 8,  9, 10}}, {{ 8, 10, 11}},
			{{11, 10, 13}}, {{11, 13, 12}}, {{12, 13, 14}}, {{12, 14, 15}}
		};

		indexBuffer.Establish(sizeof(Triangle) * 16, frameTriangle);
	}

	SetDefaultColorType(kWidgetColorBackground);
	RenderableWidget::SetWidgetColor(TheInterfaceMgr->GetInterfaceColor(kInterfaceColorPageFrame));
}

bool PageFrameWidget::CalculateBoundingBox(Box2D *box) const
{
	box->min.Set(0.0F, -16.0F);
	box->max = GetWidgetSize();
	return (true);
}

WidgetPart PageFrameWidget::TestPosition(const Point3D& position) const
{
	if (position.y < 0.0F)
	{
		return (kWidgetPartTitle);
	}

	return (kWidgetPartNone);
}

void PageFrameWidget::Preprocess(void)
{
	RenderableWidget::Preprocess();

	SetVertexCount(16);
	SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(FrameVertex));
	SetVertexAttributeArray(kArrayPosition, 0, 2);
	SetVertexAttributeArray(kArrayTexcoord, sizeof(Point2D), 2);
	vertexBuffer.Establish(sizeof(FrameVertex) * 16);

	SetPrimitiveCount(16);
	SetVertexBuffer(kVertexBufferIndexArray, &indexBuffer);

	attributeList.Append(&colorAttribute);
	SetMaterialAttributeList(&attributeList);
	SetMaterialObjectPointer(TheInterfaceMgr->GetMaterialObjectPointer());
}

void PageFrameWidget::Build(void)
{
	volatile FrameVertex *restrict vertex = vertexBuffer.BeginUpdate<FrameVertex>();

	float width = GetWidgetSize().x;
	float height = GetWidgetSize().y;

	if (height > 0.0F)
	{
		vertex[0].position.Set(-3.0F, -17.0F);
		vertex[1].position.Set(-3.0F, 0.0F);
		vertex[2].position.Set(0.0F, 0.0F);
		vertex[3].position.Set(0.0F, -17.0F);

		vertex[4].position.Set(width, -17.0F);
		vertex[5].position.Set(width, 0.0F);
		vertex[6].position.Set(width + 3.0F, 0.0F);
		vertex[7].position.Set(width + 3.0F, -17.0F);

		vertex[8].position.Set(-3.0F, height);
		vertex[9].position.Set(-3.0F, height + 5.0F);
		vertex[10].position.Set(0.0F, height + 5.0F);
		vertex[11].position.Set(0.0F, height);

		vertex[12].position.Set(width, height);
		vertex[13].position.Set(width, height + 5.0F);
		vertex[14].position.Set(width + 3.0F, height + 5.0F);
		vertex[15].position.Set(width + 3.0F, height);

		vertex[0].texcoord.Set(0.55859375F, 0.25F);
		vertex[1].texcoord.Set(0.55859375F, 0.216796875F);
		vertex[2].texcoord.Set(0.564453125F, 0.216796875F);
		vertex[3].texcoord.Set(0.564453125F, 0.25F);

		vertex[4].texcoord.Set(0.685546875F, 0.25F);
		vertex[5].texcoord.Set(0.685546875F, 0.216796875F);
		vertex[6].texcoord.Set(0.69140625F, 0.216796875F);
		vertex[7].texcoord.Set(0.69140625F, 0.25F);
	}
	else
	{
		vertex[0].position.Set(-3.0F, -17.0F);
		vertex[1].position.Set(-3.0F, -1.0F);
		vertex[2].position.Set(0.0F, -1.0F);
		vertex[3].position.Set(0.0F, -17.0F);

		vertex[4].position.Set(width, -17.0F);
		vertex[5].position.Set(width, -1.0F);
		vertex[6].position.Set(width + 3.0F, -1.0F);
		vertex[7].position.Set(width + 3.0F, -17.0F);

		vertex[8].position.Set(-3.0F, -1.0F);
		vertex[9].position.Set(-3.0F, 4.0F);
		vertex[10].position.Set(0.0F, 4.0F);
		vertex[11].position.Set(0.0F, -1.0F);

		vertex[12].position.Set(width, -1.0F);
		vertex[13].position.Set(width, 4.0F);
		vertex[14].position.Set(width + 3.0F, 4.0F);
		vertex[15].position.Set(width + 3.0F, -1.0F);

		vertex[0].texcoord.Set(0.55859375F, 0.25F);
		vertex[1].texcoord.Set(0.55859375F, 0.21875F);
		vertex[2].texcoord.Set(0.564453125F, 0.21875F);
		vertex[3].texcoord.Set(0.564453125F, 0.25F);

		vertex[4].texcoord.Set(0.685546875F, 0.25F);
		vertex[5].texcoord.Set(0.685546875F, 0.21875F);
		vertex[6].texcoord.Set(0.69140625F, 0.21875F);
		vertex[7].texcoord.Set(0.69140625F, 0.25F);
	}

	vertex[8].texcoord.Set(0.55859375F, 0.126953125F);
	vertex[9].texcoord.Set(0.55859375F, 0.1171875F);
	vertex[10].texcoord.Set(0.564453125F, 0.1171875F);
	vertex[11].texcoord.Set(0.564453125F, 0.126953125F);

	vertex[12].texcoord.Set(0.685546875F, 0.126953125F);
	vertex[13].texcoord.Set(0.685546875F, 0.1171875F);
	vertex[14].texcoord.Set(0.69140625F, 0.1171875F);
	vertex[15].texcoord.Set(0.69140625F, 0.126953125F);

	vertexBuffer.EndUpdate();

	colorAttribute.SetDiffuseColor(RenderableWidget::GetWidgetColor());
}

void PageFrameWidget::HandleMouseEvent(const PanelMouseEventData *eventData)
{
	if (eventData->widgetPart == kWidgetPartTitle)
	{
		Widget *rootWidget = GetRootWidget();
		if ((rootWidget) && (rootWidget->GetWidgetType() == kWidgetPage))
		{
			Page *page = static_cast<Page *>(rootWidget);
			BookWidget *bookWidget = page->GetOwningBook();
			if (bookWidget)
			{
				EventType eventType = eventData->eventType;

				if (eventType == kEventMouseDown)
				{
					dragPosition = eventData->mousePosition.y;
					bookWidget->BeginPageDrag(page);
				}
				else if (eventType == kEventMouseUp)
				{
					bookWidget->EndPageDrag(page);
				}
			}
		}
	}
}

void PageFrameWidget::TrackTask(WidgetPart widgetPart, const Point3D& mousePosition)
{
	if (widgetPart == kWidgetPartTitle)
	{
		Widget *rootWidget = GetRootWidget();
		if ((rootWidget) && (rootWidget->GetWidgetType() == kWidgetPage))
		{
			Page *page = static_cast<Page *>(rootWidget);
			if (page->dragFlag)
			{
				BookWidget *bookWidget = page->GetOwningBook();
				if (bookWidget)
				{
					const Point3D& position = page->GetWidgetPosition();
					page->SetWidgetPosition(Point3D(position.x, position.y + (mousePosition.y - dragPosition), position.z));
					page->Invalidate();

					bookWidget->UpdatePageDrag(page);
				}
			}
		}
	}
}


StripWidget::StripWidget(const Vector2D& size) :
		RenderableWidget(kWidgetStrip, kRenderQuads, size),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		colorAttribute(TheInterfaceMgr->GetInterfaceColor(kInterfaceColorStripFrame))
{
}

StripWidget::~StripWidget()
{
}

void StripWidget::Preprocess(void)
{
	RenderableWidget::Preprocess();

	SetVertexCount(4);
	SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(StripVertex));
	SetVertexAttributeArray(kArrayPosition, 0, 2);
	SetVertexAttributeArray(kArrayTexcoord, sizeof(Point2D), 2);
	vertexBuffer.Establish(sizeof(StripVertex) * 4);

	attributeList.Append(&colorAttribute);
	SetMaterialAttributeList(&attributeList);
	SetMaterialObjectPointer(TheInterfaceMgr->GetMaterialObjectPointer());
}

void StripWidget::Build(void)
{
	volatile StripVertex *restrict vertex = vertexBuffer.BeginUpdate<StripVertex>();

	float w = GetWidgetSize().x;
	float h = GetWidgetSize().y;

	vertex[0].position.Set(0.0F, 0.0F);
	vertex[0].texcoord.Set(0.5F, 0.0625F);

	vertex[1].position.Set(0.0F, h);
	vertex[1].texcoord.Set(0.5F, 0.0F);

	vertex[2].position.Set(w, h);
	vertex[2].texcoord.Set(0.75F, 0.0F);

	vertex[3].position.Set(w, 0.0F);
	vertex[3].texcoord.Set(0.75F, 0.0625F);

	vertexBuffer.EndUpdate();
}


RootWidget::RootWidget(WidgetType type) :
		Widget(type),
		widgetHashTable(16, 4)
{
	SetBaseWidgetType(kWidgetRoot);
	widgetMoveParity = 0;
}

RootWidget::RootWidget(WidgetType type, const Vector2D& size) :
		Widget(type, size),
		widgetHashTable(16, 4)
{
	SetBaseWidgetType(kWidgetRoot);
	widgetMoveParity = 0;
}

RootWidget::RootWidget(const RootWidget& rootWidget) :
		Widget(rootWidget),
		widgetHashTable(16, 4)
{
	widgetMoveParity = 0;
}

RootWidget::~RootWidget()
{
	widgetHashTable.RemoveAll();
}

void RootWidget::Preprocess(void)
{
	Widget::Preprocess();

	if (widgetMoveList[widgetMoveParity].First())
	{
		RootWidget *root = GetRootWidget();
		if (root)
		{
			root->AddMovingWidget(this);
		}
	}
}

void RootWidget::Move(void)
{
	unsigned_int32 parity = widgetMoveParity;
	List<Widget> *currentList = &widgetMoveList[parity];
	List<Widget> *nextList = &widgetMoveList[parity ^ 1];

	for (;;)
	{
		Widget *widget = currentList->First();
		if (!widget)
		{
			break;
		}

		nextList->Append(widget);
		widget->Move();
	}

	widgetMoveParity = parity ^ 1;
}

void RootWidget::PackAuxiliaryData(Packer& data) const
{
	data << ChunkHeader('SIZE', sizeof(Vector2D));
	data << GetWidgetSize();
}

void RootWidget::UnpackAuxiliaryData(Unpacker& data)
{
	for (;;)
	{
		ChunkHeader		chunkHeader;

		data >> chunkHeader;
		if (chunkHeader.chunkType == 0)
		{
			break;
		}

		const void *mark = data.GetPointer();
		if (!UnpackAuxiliaryChunk(&chunkHeader, data))
		{
			data.Skip(mark, chunkHeader.chunkSize);
		}
	}
}

bool RootWidget::UnpackAuxiliaryChunk(const ChunkHeader *chunkHeader, Unpacker& data)
{
	switch (chunkHeader->chunkType)
	{
		case 'SIZE':

			data >> GetWidgetSize();
			return (true);
	}

	return (false);
}

void RootWidget::SetFocusWidget(Widget *widget)
{
	Widget *focus = focusWidget;
	if (focus != widget)
	{
		if (focus)
		{
			focus->SetWidgetState(focus->GetWidgetState() & ~kWidgetFocus);
		}

		if (widget)
		{
			widget->SetWidgetState(widget->GetWidgetState() | kWidgetFocus);
		}

		focusWidget = widget;
	}
}

Widget *RootWidget::SetNextFocusWidget(void)
{
	const Widget *focus = focusWidget;
	Widget *widget = (focus) ? GetNextNode(focus) : GetFirstSubnode();
	while (widget != focus)
	{
		if (!widget)
		{
			widget = GetFirstSubnode();
		}

		if ((widget->GetWidgetUsage() & kWidgetKeyboardFocus) && (!(widget->GetGlobalWidgetState() & (kWidgetDisabled | kWidgetHidden))))
		{
			break;
		}

		widget = GetNextNode(widget);
	}

	if (widget)
	{
		SetFocusWidget(widget);
	}

	return (widget);
}

Widget *RootWidget::SetPreviousFocusWidget(void)
{
	const Widget *focus = focusWidget;
	Widget *widget = (focus) ? GetPreviousNode(focus) : GetRightmostNode();
	while (widget != focus)
	{
		if (!widget)
		{
			widget = GetRightmostNode();
		}

		if ((widget->GetWidgetUsage() & kWidgetKeyboardFocus) && (!(widget->GetGlobalWidgetState() & (kWidgetDisabled | kWidgetHidden))))
		{
			break;
		}

		widget = GetPreviousNode(widget);
	}

	if (widget)
	{
		SetFocusWidget(widget);
	}

	return (widget);
}


Panel::Panel(const Vector2D& size) : RootWidget(kWidgetPanel, size)
{
	renderTransform.SetIdentity();
}

Panel::~Panel()
{
}

void Panel::Move(void)
{
	RootWidget::Move();

	Widget *widget = GetFocusWidget();
	if (widget)
	{
		widget->FocusTask();
	}
}


Board::Board() : RootWidget(kWidgetBoard)
{
}

Board::Board(const Vector2D& size) : RootWidget(kWidgetBoard, size)
{
}

Board::~Board()
{
}


C4::Window::Window(const char *panelName) :
		RootWidget(kWidgetWindow),
		closeObserver(this, &Window::HandleCloseEvent),
		titleWidget(Vector2D(0.0F, 21.0F), nullptr, "font/Title"),
		closeWidget(Vector2D(18.0F, 18.0F), Point2D(0.1875F, 0.6875F), Point2D(0.3125F, 0.8125F))
{
	windowFlags = 0;
	minWindowSize.Set(32.0F, 32.0F);

	Load(panelName);

	stripTitle = titleWidget.GetText();
	stripIcon = "C4/window";

	for (machine a = 0; a < kMaxBackgroundQuadCount; a++)
	{
		new(backgroundWidget[a]) QuadWidget(Zero2D);
	}
}

C4::Window::Window(const Vector2D& size, const char *title, unsigned_int32 flags) :
		RootWidget(kWidgetWindow, size),
		closeObserver(this, &Window::HandleCloseEvent),
		titleWidget(Vector2D(0.0F, 21.0F), title, "font/Title"),
		closeWidget(Vector2D(18.0F, 18.0F), Point2D(0.1875F, 0.6875F), Point2D(0.3125F, 0.8125F)),
		frameWidget(size)
{
	windowFlags = flags;
	minWindowSize.Set(32.0F, 32.0F);

	if (title)
	{
		stripTitle = title;
	}
	else
	{
		stripTitle[0] = 0;
	}

	stripIcon = "C4/window";

	for (machine a = 0; a < kMaxBackgroundQuadCount; a++)
	{
		new(backgroundWidget[a]) QuadWidget(size);
	}
}

C4::Window::~Window()
{
	if (GetSuperNode())
	{
		TheInterfaceMgr->RemoveWidget(this);
	}

	delete windowButton.GetTarget();

	for (machine a = kMaxBackgroundQuadCount - 1; a >= 0; a--)
	{
		backgroundWidget[a]->~QuadWidget();
	}
}

void C4::Window::Detach(void)
{
	ListElement<Window>::Detach();
	RootWidget::Detach();
}

void C4::Window::PackAuxiliaryData(Packer& data) const
{
	RootWidget::PackAuxiliaryData(data);

	PackHandle handle = data.BeginChunk('TITL');
	data << titleWidget.GetText();
	data.EndChunk(handle);

	data << ChunkHeader('FLAG', 4);
	data << (windowFlags & ~kWindowEditor);
}

bool C4::Window::UnpackAuxiliaryChunk(const ChunkHeader *chunkHeader, Unpacker& data)
{
	switch (chunkHeader->chunkType)
	{
		case 'TITL':

			titleWidget.SetText(data.ReadString());
			return (true);

		case 'FLAG':

			data >> windowFlags;
			return (true);
	}

	return (RootWidget::UnpackAuxiliaryChunk(chunkHeader, data));
}

int32 C4::Window::GetSettingCount(void) const
{
	return (13);
}

Setting *C4::Window::GetSetting(int32 index) const
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetWindow, 'WIDE'));
		return (new TextSetting('WIDE', Text::IntegerToString((int32) GetWidgetSize().x), title, 5, &EditTextWidget::NumberFilter));
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetWindow, 'HIGH'));
		return (new TextSetting('HIGH', Text::IntegerToString((int32) GetWidgetSize().y), title, 5, &EditTextWidget::NumberFilter));
	}

	if (index == 2)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetWindow, 'TITL'));
		return (new TextSetting('TITL', titleWidget.GetText(), title, 127));
	}

	if (index == 3)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetWindow, 'PLAN'));
		return (new BooleanSetting('PLAN', ((windowFlags & kWindowPlain) != 0), title));
	}

	if (index == 4)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetWindow, 'CLOS'));
		return (new BooleanSetting('CLOS', ((windowFlags & kWindowCloseBox) != 0), title));
	}

	if (index == 5)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetWindow, 'RSIZ'));
		return (new BooleanSetting('RSIZ', ((windowFlags & kWindowResizable) != 0), title));
	}

	if (index == 6)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetWindow, 'BKGD'));
		return (new BooleanSetting('BKGD', ((windowFlags & kWindowBackground) != 0), title));
	}

	if (index == 7)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetWindow, 'CENT'));
		return (new BooleanSetting('CENT', ((windowFlags & kWindowCenter) != 0), title));
	}

	if (index == 8)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetWindow, 'MXMZ'));
		return (new BooleanSetting('MXMZ', ((windowFlags & kWindowMaximize) != 0), title));
	}

	if (index == 9)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetWindow, 'EVEN'));
		return (new BooleanSetting('EVEN', ((windowFlags & kWindowEvenSize) != 0), title));
	}

	if (index == 10)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetWindow, 'STRP'));
		return (new BooleanSetting('STRP', ((windowFlags & kWindowStrip) != 0), title));
	}

	if (index == 11)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetWindow, 'PASS'));
		return (new BooleanSetting('PASS', ((windowFlags & kWindowPassive) != 0), title));
	}

	if (index == 12)
	{
		int32 selection = 0;
		if (windowFlags & kWindowFullHorizontal)
		{
			selection = 1;
		}
		else if (windowFlags & kWindowFullVertical)
		{
			selection = 2;
		}

		const char *title = table->GetString(StringID('WDGT', kWidgetWindow, 'SCAL'));
		MenuSetting *menu = new MenuSetting('SCAL', selection, title, 3);

		menu->SetMenuItemString(0, table->GetString(StringID('WDGT', kWidgetWindow, 'SCAL', 'NOSC')));
		menu->SetMenuItemString(1, table->GetString(StringID('WDGT', kWidgetWindow, 'SCAL', 'HORZ')));
		menu->SetMenuItemString(2, table->GetString(StringID('WDGT', kWidgetWindow, 'SCAL', 'VERT')));
		return (menu);
	}

	return (nullptr);
}

void C4::Window::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'WIDE')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		GetWidgetSize().x = (float) Max(Text::StringToInteger(text), 1);
	}
	else if (identifier == 'HIGH')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		GetWidgetSize().y = (float) Max(Text::StringToInteger(text), 1);
	}
	else if (identifier == 'TITL')
	{
		titleWidget.SetText(static_cast<const TextSetting *>(setting)->GetText());
	}
	else if (identifier == 'PLAN')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			windowFlags |= kWindowPlain;
		}
		else
		{
			windowFlags &= ~kWindowPlain;
		}
	}
	else if (identifier == 'CLOS')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			windowFlags |= kWindowCloseBox;
		}
		else
		{
			windowFlags &= ~kWindowCloseBox;
		}
	}
	else if (identifier == 'RSIZ')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			windowFlags |= kWindowResizable;
		}
		else
		{
			windowFlags &= ~kWindowResizable;
		}
	}
	else if (identifier == 'BKGD')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			windowFlags |= kWindowBackground;
		}
		else
		{
			windowFlags &= ~kWindowBackground;
		}
	}
	else if (identifier == 'CENT')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			windowFlags |= kWindowCenter;
		}
		else
		{
			windowFlags &= ~kWindowCenter;
		}
	}
	else if (identifier == 'MXMZ')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			windowFlags |= kWindowMaximize;
		}
		else
		{
			windowFlags &= ~kWindowMaximize;
		}
	}
	else if (identifier == 'EVEN')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			windowFlags |= kWindowEvenSize;
		}
		else
		{
			windowFlags &= ~kWindowEvenSize;
		}
	}
	else if (identifier == 'STRP')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			windowFlags |= kWindowStrip;
		}
		else
		{
			windowFlags &= ~kWindowStrip;
		}
	}
	else if (identifier == 'PASS')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			windowFlags |= kWindowPassive;
		}
		else
		{
			windowFlags &= ~kWindowPassive;
		}
	}
	else if (identifier == 'SCAL')
	{
		unsigned_int32 flags = windowFlags & ~(kWindowFullHorizontal | kWindowFullVertical);

		int32 selection = static_cast<const MenuSetting *>(setting)->GetMenuSelection();
		if (selection == 1)
		{
			flags |= kWindowFullHorizontal;
		}
		else if (selection == 2)
		{
			flags |= kWindowFullVertical;
		}

		windowFlags = flags;
	}
}

void C4::Window::HandleCloseEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		Close();
	}
}

void C4::Window::SetWidgetSize(const Vector2D& size)
{
	Vector2D prevSize = GetWidgetSize();
	RootWidget::SetWidgetSize(size);

	frameWidget.SetWidgetSize(size);
	backgroundWidget[0]->SetWidgetSize(size);

	if (windowFlags & kWindowCloseBox)
	{
		titleWidget.GetWidgetSize().x = size.x - 20.0F;

		closeWidget.SetWidgetPosition(Point3D(size.x - 18.0F, -20.0F, 0.0F));
		closeWidget.Invalidate();
	}
	else
	{
		titleWidget.GetWidgetSize().x = size.x - 2.0F;
	}

	Widget *widget = GetFirstSubnode();
	while (widget)
	{
		unsigned_int32 anchorX = widget->GetWidgetAnchorX();
		unsigned_int32 anchorY = widget->GetWidgetAnchorY();
		if ((anchorX | anchorY) != 0)
		{
			Point3D position = widget->GetWidgetPosition();

			if (anchorX == kWidgetAnchorRight)
			{
				position.x += size.x - prevSize.x;
			}
			else if (anchorX == kWidgetAnchorCenter)
			{
				position.x = Floor((size.x - widget->GetWidgetSize().x) * 0.5F);
			}

			if (anchorY == kWidgetAnchorBottom)
			{
				position.y += size.y - prevSize.y;
			}
			else if (anchorY == kWidgetAnchorCenter)
			{
				position.y = Floor((size.y - widget->GetWidgetSize().y) * 0.5F);
			}

			widget->SetWidgetPosition(position);
			widget->Invalidate();
		}

		widget = GetNextNode(widget);
	}
}

void C4::Window::ScaleWindow(void)
{
	float displayWidth = (float) TheDisplayMgr->GetDisplayWidth();
	float displayHeight = (float) TheDisplayMgr->GetDisplayHeight();
	const Vector2D& size = GetWidgetSize();

	if (windowFlags & kWindowFullHorizontal)
	{
		float scale = displayWidth / size.x;
		SetWidgetTransform(Vector3D(scale, 0.0F, 0.0F), Vector3D(0.0F, scale, 0.0F), Vector3D(0.0F, 0.0F, scale), Point3D(0.0F, Floor((displayHeight - size.y * scale) * 0.5F), 0.0F));
	}
	else if (windowFlags & kWindowFullVertical)
	{
		float scale = displayHeight / size.y;
		SetWidgetTransform(Vector3D(scale, 0.0F, 0.0F), Vector3D(0.0F, scale, 0.0F), Vector3D(0.0F, 0.0F, scale), Point3D(Floor((displayWidth - size.x * scale) * 0.5F), 0.0F, 0.0F));
	}

	Invalidate();
}

void C4::Window::CenterWindow(void)
{
	float top = (windowFlags & kWindowPlain) ? 0.0F : 19.0F;

	const Vector2D& desktopSize = TheInterfaceMgr->GetDesktopSize();
	float x = Floor((desktopSize.x - GetWidgetSize().x) * 0.5F);
	float y = Fmax(Floor((desktopSize.y - top - GetWidgetSize().y) * 0.5F + top), top);
	SetWidgetPosition(Point3D(x, y, 0.0F));

	Invalidate();
}

void C4::Window::Preprocess(void)
{
	unsigned_int32 flags = windowFlags;
	if (!(flags & kWindowEditor))
	{
		if (flags & (kWindowFullHorizontal | kWindowFullVertical))
		{
			ScaleWindow();
		}
		else if (flags & kWindowMaximize)
		{
			const Vector2D& desktopSize = TheInterfaceMgr->GetDesktopSize();

			if (flags & kWindowPlain)
			{
				SetWidgetPosition(Point3D(2.0F, 2.0F, 0.0F));
				RootWidget::SetWidgetSize(Vector2D(desktopSize.x - 4.0F, desktopSize.y - 4.0F));
			}
			else
			{
				SetWidgetPosition(Point3D(8.0F, 26.0F, 0.0F));
				RootWidget::SetWidgetSize(Vector2D(desktopSize.x - 16.0F, desktopSize.y - 34.0F));
			}
		}
		else if (flags & kWindowCenter)
		{
			CenterWindow();
		}

		if (!(flags & kWindowPlain))
		{
			frameWidget.SetWidgetSize(GetWidgetSize());

			titleWidget.SetWidgetState(kWidgetDisabled);
			titleWidget.SetWidgetColor(TheInterfaceMgr->GetInterfaceColor(kInterfaceColorWindowTitle));
			titleWidget.SetWidgetPosition(Point3D(1.0F, -21.0F, 0.0F));
			titleWidget.GetWidgetSize().x = GetWidgetSize().x - 2.0F;

			PrependSubnode(&frameWidget);
			InsertSubnodeAfter(&titleWidget, &frameWidget);

			if (flags & kWindowCloseBox)
			{
				titleWidget.GetWidgetSize().x -= 18.0F;

				closeWidget.SetObserver(&closeObserver);
				closeWidget.SetWidgetColor(ColorRGBA(1.0F, 0.125F, 0.25F, 1.0F));
				closeWidget.SetWidgetPosition(Point3D(GetWidgetSize().x - 18.0F, -20.0F, 0.0F));
				InsertSubnodeAfter(&closeWidget, &titleWidget);
			}
		}

		const ColorRGBA& color = TheInterfaceMgr->GetInterfaceColor(kInterfaceColorWindowBack);
		for (machine a = 0; a < kMaxBackgroundQuadCount; a++)
		{
			backgroundWidget[a]->SetWidgetColor(color);
		}

		if (flags & kWindowBackground)
		{
			backgroundWidget[0]->SetWidgetSize(GetWidgetSize());
			PrependSubnode(backgroundWidget[0]);
		}

		if (flags & kWindowStrip)
		{
			windowButton = TheInterfaceMgr->GetStrip()->AddWindow(this);
		}
	}

	RootWidget::Preprocess();
}

void C4::Window::Move(void)
{
	RootWidget::Move();

	Widget *widget = GetFocusWidget();
	if ((widget) && (TheInterfaceMgr->GetActiveWindow() == this))
	{
		widget->FocusTask();
	}

	Window *subwindow = subwindowList.First();
	while (subwindow)
	{
		Window *next = subwindow->Next();
		subwindow->Move();
		subwindow = next;
	}
}

void C4::Window::Show(void)
{
	RootWidget::Show();

	TheInterfaceMgr->PostWindowEvent(WindowEventData(kEventWindowChange, this));
}

void C4::Window::Hide(void)
{
	RootWidget::Hide();

	TheInterfaceMgr->PostWindowEvent(WindowEventData(kEventWindowChange, this));
}

void C4::Window::EnterForeground(void)
{
	RootWidget::EnterForeground();

	if (windowFlags & kWindowStrip)
	{
		TheInterfaceMgr->GetStrip()->Show();

		Widget *widget = windowButton;
		if (widget)
		{
			static_cast<WindowButtonWidget *>(widget)->SetValue(1);
		}
	}
}

void C4::Window::EnterBackground(void)
{
	RootWidget::EnterBackground();

	Widget *widget = windowButton;
	if (widget)
	{
		static_cast<WindowButtonWidget *>(widget)->SetValue(0);
	}
}

void C4::Window::SetWindowTitle(const char *title)
{
	titleWidget.SetText(title);
	stripTitle = title;
}

void C4::Window::SetStripTitle(const char *title)
{
	stripTitle = title;

	Widget *widget = windowButton;
	if (widget)
	{
		static_cast<WindowButtonWidget *>(widget)->SetTitle(title);
	}
}

void C4::Window::SetBackgroundQuad(int32 index, const Point3D& position, const Vector2D& size)
{
	QuadWidget *quad = backgroundWidget[index];
	quad->SetWidgetPosition(position);
	quad->SetWidgetSize(size);

	if (!quad->GetSuperNode())
	{
		PrependSubnode(quad);
		quad->Preprocess();
	}
	else
	{
		quad->Invalidate();
	}
}

void C4::Window::AddSubwindow(Window *window)
{
	AppendNewSubnode(window);
	subwindowList.Append(window);

	unsigned_int32 flags = window->GetWindowFlags();
	window->windowFlags = flags | kWindowModal;

	if (flags & (kWindowMaximize | kWindowCenter))
	{
		window->SetWidgetPosition(Zero3D + (window->GetWidgetPosition() - GetWorldPosition()));
	}

	if (windowFlags & (kWindowFullHorizontal | kWindowFullVertical))
	{
		float scale = 1.0F / GetWidgetTransform()(0,0);
		window->SetWidgetTransform(Vector3D(scale, 0.0F, 0.0F), Vector3D(0.0F, scale, 0.0F), Vector3D(0.0F, 0.0F, scale), window->GetWidgetPosition() * scale);
	}

	if (TheInterfaceMgr->GetActiveWindow() == this)
	{
		TheInterfaceMgr->SetActiveWindow(window);
	}

	TheInterfaceMgr->PostWindowEvent(WindowEventData(kEventWindowAdd, window));
}

Widget *C4::Window::GetActivationKeyWidget(Type key) const
{
	if (key == kActivationKeyEnter)
	{
		return (enterKeyWidget);
	}

	if (key == kActivationKeyEscape)
	{
		return (escapeKeyWidget);
	}

	return (nullptr);
}

void C4::Window::SetActivationKeyWidget(Type key, Widget *widget)
{
	if (key == kActivationKeyEnter)
	{
		enterKeyWidget = widget;
	}

	if (key == kActivationKeyEscape)
	{
		escapeKeyWidget = widget;
	}
}

bool C4::Window::HandleKeyboardEvent(const KeyboardEventData *eventData)
{
	EventType eventType = eventData->eventType;
	if (eventType == kEventKeyDown)
	{
		unsigned_int32 keyCode = eventData->keyCode;

		if (keyCode == kKeyCodeTab)
		{
			if (!(eventData->modifierKeys & kModifierKeyShift))
			{
				SetNextFocusWidget();
			}
			else
			{
				SetPreviousFocusWidget();
			}

			return (true);
		}

		if (keyCode == kKeyCodeEnter)
		{
			Widget *widget = enterKeyWidget;
			if (widget)
			{
				widget->Activate();
				return (true);
			}
		}

		if (keyCode == kKeyCodeEscape)
		{
			Widget *widget = escapeKeyWidget;
			if (widget)
			{
				widget->Activate();
				return (true);
			}
		}

		Widget *widget = GetFocusWidget();
		if (widget)
		{
			return (widget->HandleKeyboardEvent(eventData));
		}
	}
	else if (eventType == kEventKeyCommand)
	{
		Widget *widget = GetFocusWidget();
		if ((widget) && (widget->HandleKeyboardEvent(eventData)))
		{
			return (true);
		}

		widget = commandWidget;
		if ((widget) && (widget->Enabled()))
		{
			return (widget->HandleKeyboardEvent(eventData));
		}
	}

	return (false);
}

void C4::Window::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
}

void C4::Window::Close(void)
{
	delete this;
}


WindowButtonWidget::WindowButtonWidget(Window *window) :
		RenderableWidget(kWidgetWindowButton, kRenderQuads, Vector2D(0.0F, 32.0F)),
		targetWindow(window),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		colorAttribute(kAttributeMutable),
		textWidget(window->GetStripTitle(), "font/Normal"),
		imageWidget(Vector2D(28.0F, 28.0F), window->GetStripIcon())
{
	windowButtonValue = 0;
}

WindowButtonWidget::~WindowButtonWidget()
{
	TheInterfaceMgr->GetStrip()->Invalidate();
}

void WindowButtonWidget::SetValue(int32 value)
{
	if (windowButtonValue != value)
	{
		windowButtonValue = value;
		SetBuildFlag();
	}
}

void WindowButtonWidget::SetTitle(const char *title)
{
	textWidget.SetText(title);
}

void WindowButtonWidget::SetWidgetSize(const Vector2D& size)
{
	RenderableWidget::SetWidgetSize(size);

	textWidget.SetWidgetSize(Vector2D(FmaxZero(size.x - 42.0F), 16.0F));
}

void WindowButtonWidget::Preprocess(void)
{
	textWidget.Disable();
	imageWidget.Disable();
	AppendSubnode(&textWidget);
	AppendSubnode(&imageWidget);

	RenderableWidget::Preprocess();

	SetVertexCount(12);
	SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(ButtonVertex));
	SetVertexAttributeArray(kArrayPosition, 0, 2);
	SetVertexAttributeArray(kArrayTexcoord, sizeof(Point2D), 2);
	vertexBuffer.Establish(sizeof(ButtonVertex) * 12);

	attributeList.Append(&colorAttribute);
	SetMaterialAttributeList(&attributeList);
	SetMaterialObjectPointer(TheInterfaceMgr->GetMaterialObjectPointer());

	float height = GetWidgetSize().y;
	float h = textWidget.GetFont()->GetFontHeader()->fontHeight;

	textWidget.SetWidgetPosition(Point3D(38.0F, FmaxZero(Floor((height - h) * 0.5F - 1.0F)), 0.0F));
	imageWidget.SetWidgetPosition(Point3D(6.0F, (height - imageWidget.GetWidgetSize().y) * 0.5F - 2.0F, 0.0F));

	textWidget.SetWidgetColor(TheInterfaceMgr->GetInterfaceColor(kInterfaceColorStripTitle));
}

void WindowButtonWidget::Build(void)
{
	volatile ButtonVertex *restrict vertex = vertexBuffer.BeginUpdate<ButtonVertex>();

	float width = GetWidgetSize().x;
	float height = GetWidgetSize().y;

	float y1 = -1.0F;
	float y2 = height + 1.0F;
	float x1 = -1.0F;
	float x2 = Fmin(height * 0.5F, width * 0.5F);
	float x3 = width - x2;
	float x4 = width + 1.0F;
	float ds = Fmin((x3 - x2) / (y2 - y1) * 0.125F, 0.375F);

	vertex[0].position.Set(x1, y1);
	vertex[1].position.Set(x1, y2);
	vertex[2].position.Set(x2, y2);
	vertex[3].position.Set(x2, y1);

	vertex[0].texcoord.Set(0.0F, 0.25F);
	vertex[1].texcoord.Set(0.0F, 0.125F);
	vertex[2].texcoord.Set(0.0625F, 0.125F);
	vertex[3].texcoord.Set(0.0625F, 0.25F);

	vertex[4].position.Set(x2, y1);
	vertex[5].position.Set(x2, y2);
	vertex[6].position.Set(x3, y2);
	vertex[7].position.Set(x3, y1);

	vertex[4].texcoord.Set(0.0625F, 0.25F);
	vertex[5].texcoord.Set(0.0625F, 0.125F);
	vertex[6].texcoord.Set(0.0625F + ds, 0.125F);
	vertex[7].texcoord.Set(0.0625F + ds, 0.25F);

	vertex[8].position.Set(x3, y1);
	vertex[9].position.Set(x3, y2);
	vertex[10].position.Set(x4, y2);
	vertex[11].position.Set(x4, y1);

	vertex[8].texcoord.Set(0.4375F, 0.25F);
	vertex[9].texcoord.Set(0.4375F, 0.125F);
	vertex[10].texcoord.Set(0.5F, 0.125F);
	vertex[11].texcoord.Set(0.5F, 0.25F);

	vertexBuffer.EndUpdate();

	float alpha = (GetGlobalWidgetState() & kWidgetDisabled) ? 0.5F : 1.0F;

	const ColorRGBA *color = (windowButtonValue != 0) ? &TheInterfaceMgr->GetInterfaceColor(kInterfaceColorStripButton) : &K::white;
	if (GetWidgetState() & kWidgetHilited)
	{
		colorAttribute.SetDiffuseColor(ColorRGBA(color->GetColorRGB() * 0.625F, alpha));
	}
	else
	{
		colorAttribute.SetDiffuseColor(ColorRGBA(color->GetColorRGB(), alpha));
	}
}

void WindowButtonWidget::HandleMouseEvent(const PanelMouseEventData *eventData)
{
	EventType eventType = eventData->eventType;
	unsigned_int32 state = GetWidgetState();

	if (eventType == kEventMouseDown)
	{
		SetWidgetState(state | kWidgetHilited);
	}
	else if (eventType == kEventMouseMoved)
	{
		if (WidgetContainsPoint(eventData->mousePosition))
		{
			state |= kWidgetHilited;
		}
		else
		{
			state &= ~kWidgetHilited;
		}

		SetWidgetState(state);
	}
	else if (eventType == kEventMouseUp)
	{
		SetWidgetState(state & ~kWidgetHilited);
		if (WidgetContainsPoint(eventData->mousePosition))
		{
			TheInterfaceMgr->SetActiveWindow(static_cast<Window *>(targetWindow.GetTarget()));
		}
	}
}


Balloon::Balloon(BalloonType type, const char *string) : RootWidget(kWidgetBalloon)
{
	if (type == kBalloonText)
	{
		TextWidget *widget = new TextWidget(string, "font/Normal");
		widget->SetTextFlags(kTextWrapped);
		widget->SetTextLeading(-2.0F);

		float width = widget->GetFormattedTextWidth();
		float h = widget->GetFont()->GetLineSpacing() - 2.0F;
		float n = PositiveCeil(Sqrt(width / (h * 1.5F)));
		float w = Clamp(PositiveCeil(width / n), 128.0F, width);

		Vector2D size(w, 0.0F);
		widget->SetWidgetSize(size);
		widget->SplitLines();

		size.Set(widget->GetMaxLineWidth() + 16.0F, (float) widget->GetLineCount() * h + 8.0F);
		SetWidgetSize(size);

		frameWidget.SetWidgetSize(size);
		AppendSubnode(&frameWidget);

		widget->SetWidgetPosition(Point3D(8.0F, 5.0F, 0.0F));
		AppendSubnode(widget);
	}
	else if (type == kBalloonResource)
	{
		Load(string);

		frameWidget.SetWidgetSize(GetWidgetSize());
		PrependSubnode(&frameWidget);
	}
}

Balloon::~Balloon()
{
}


Page::Page(const char *panelName) :
		RootWidget(kWidgetPage),
		collapseObserver(this, &Page::HandleCollapseEvent),
		titleWidget(Vector2D(0.0F, 15.0F), nullptr, "font/Page"),
		collapseWidget(Vector2D(12.0F, 12.0F), Point2D(0.5625F, 0.6875F), Point2D(0.6875F, 0.8125F)),
		backgroundWidget(Zero2D)
{
	windowFlags = 0;
	owningBook = nullptr;
	dragFlag = false;

	Load(panelName);
}

Page::Page(const Vector2D& size, const char *title, unsigned_int32 flags) :
		RootWidget(kWidgetPage, size),
		collapseObserver(this, &Page::HandleCollapseEvent),
		titleWidget(Vector2D(0.0F, 15.0F), title, "font/Page"),
		collapseWidget(Vector2D(12.0F, 12.0F), Point2D(0.5625F, 0.6875F), Point2D(0.6875F, 0.8125F)),
		frameWidget(size),
		backgroundWidget(size)
{
	windowFlags = flags;
	owningBook = nullptr;
	dragFlag = false;
}

Page::~Page()
{
}

bool Page::UnpackAuxiliaryChunk(const ChunkHeader *chunkHeader, Unpacker& data)
{
	switch (chunkHeader->chunkType)
	{
		case 'TITL':

			titleWidget.SetText(data.ReadString());
			break;

		case 'FLAG':

			data >> windowFlags;
			break;
	}

	return (RootWidget::UnpackAuxiliaryChunk(chunkHeader, data));
}

void Page::Preprocess(void)
{
	frameWidget.SetWidgetSize(GetWidgetSize());

	titleWidget.SetWidgetState(kWidgetDisabled);
	titleWidget.SetWidgetColor(TheInterfaceMgr->GetInterfaceColor(kInterfaceColorWindowTitle));
	titleWidget.SetWidgetPosition(Point3D(3.0F, -15.0F, 0.0F));
	titleWidget.GetWidgetSize().x = GetWidgetSize().x - 19.0F;

	PrependSubnode(&titleWidget);
	PrependSubnode(&frameWidget);

	collapseWidget.SetObserver(&collapseObserver);
	collapseWidget.SetWidgetColor(ColorRGBA(0.0F, 1.0F, 0.25F, 1.0F));
	collapseWidget.SetWidgetPosition(Point3D(GetWidgetSize().x - 13.0F, -14.0F, 0.0F));
	InsertSubnodeBefore(&collapseWidget, &titleWidget);

	if (windowFlags & kWindowBackground)
	{
		backgroundWidget.SetWidgetColor(TheInterfaceMgr->GetInterfaceColor(kInterfaceColorPageBack));
		backgroundWidget.SetWidgetSize(GetWidgetSize());
		PrependSubnode(&backgroundWidget);
	}

	RootWidget::Preprocess();
}

void Page::Collapse(void)
{
	unsigned_int32 state = GetWidgetState();
	if (!(state & kWidgetCollapsed))
	{
		SetWidgetState(state | kWidgetCollapsed);
		collapseWidget.SetWidgetState(collapseWidget.GetWidgetState() | kWidgetCollapsed);

		pageSize = GetWidgetSize();
		SetWidgetSize(Vector2D(pageSize.x, 0.0F));
		frameWidget.SetWidgetSize(Vector2D(GetWidgetSize().x, 0.0F));

		backgroundWidget.Widget::Detach();

		Widget *widget = titleWidget.Widget::Next();
		while (widget)
		{
			Widget *next = widget->Next();

			RemoveSubnode(widget);
			storageWidget.AppendSubnode(widget);

			widget = next;
		}

		Invalidate();
	}
}

void Page::Expand(void)
{
	unsigned_int32 state = GetWidgetState();
	if (state & kWidgetCollapsed)
	{
		SetWidgetState(state & ~kWidgetCollapsed);
		collapseWidget.SetWidgetState(collapseWidget.GetWidgetState() & ~kWidgetCollapsed);

		SetWidgetSize(pageSize);
		frameWidget.SetWidgetSize(pageSize);

		if (windowFlags & kWindowBackground)
		{
			AppendSubnode(&backgroundWidget);
		}

		for (;;)
		{
			Widget *widget = storageWidget.GetFirstSubnode();
			if (!widget)
			{
				break;
			}

			AppendSubnode(widget);
		}

		Invalidate();
	}
}

void Page::HandleCollapseEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		bool all = ((owningBook) && (InterfaceMgr::GetCommandKey()));

		if (GetWidgetState() & kWidgetCollapsed)
		{
			if (all)
			{
				Page *page = owningBook->GetFirstPage();
				while (page)
				{
					page->Expand();
					page = page->Next();
				}
			}
			else
			{
				Expand();
			}
		}
		else
		{
			if (all)
			{
				Page *page = owningBook->GetFirstPage();
				while (page)
				{
					page->Collapse();
					page = page->Next();
				}
			}
			else
			{
				Collapse();
			}
		}

		if (owningBook)
		{
			owningBook->OrganizePages();
		}
	}
}


BookWidget::BookWidget(const Vector2D& size, float top, float bottom) : Widget(kWidgetBook, size)
{
	topMargin = top;
	bottomMargin = bottom;
	totalPageHeight = 0.0F;

	SetWidgetUsage(kWidgetMouseWheel);

	pageGroup.SetWidgetPosition(Point3D(0.0F, top, 0.0F));
	AppendSubnode(&pageGroup);
}

BookWidget::~BookWidget()
{
}

void BookWidget::SetWidgetSize(const Vector2D& size)
{
	Widget::SetWidgetSize(size);
	Invalidate();
}

void BookWidget::UpdateBoundingBox(void)
{
	Widget::UpdateBoundingBox();

	Box2D *boundingBox = GetBoundingBoxPointer();
	if (boundingBox)
	{
		float y = GetWorldPosition().y;
		boundingBox->min.y = Fmax(boundingBox->min.y, y);
		boundingBox->max.y = Fmin(boundingBox->max.y, y + GetWidgetSize().y);
	}
}

void BookWidget::RenderTree(List<Renderable> *renderList)
{
	TheGraphicsMgr->Draw(renderList);
	renderList->RemoveAll();

	const Window *window = GetOwningWindow();
	const Point3D& windowPosition = window->GetWorldPosition();
	int32 left = (int32) windowPosition.x;
	int32 right = left + (int32) window->GetWidgetSize().x;

	const Point3D& position = GetWorldPosition();
	Rect rect(left, (int32) position.y - 1, right, (int32) (position.y + GetWidgetSize().y) + 1);
	TheGraphicsMgr->BeginClip(rect);

	Widget::RenderTree(renderList);
	TheGraphicsMgr->Draw(renderList);
	renderList->RemoveAll();

	TheGraphicsMgr->EndClip();
}

void BookWidget::Move(void)
{
	float t = Fmin(pageMoveTime + TheTimeMgr->GetFloatDeltaTime() * 0.01F, 1.0F);
	pageMoveTime = t;

	t = t * t * (3.0F - t * 2.0F);

	Page *page = pageList.First();
	while (page)
	{
		if (!page->dragFlag)
		{
			float y = page->movePosition[0] * (1.0F - t) + page->movePosition[1] * t;

			const Point3D& position = page->GetWidgetPosition();
			page->SetWidgetPosition(Point3D(position.x, y, position.z));
			page->Invalidate();
		}

		page = page->Next();
	}
}

void BookWidget::HandleMouseEvent(const PanelMouseEventData *eventData)
{
	if (eventData->eventType == kEventMouseWheel)
	{
		float y = pageGroup.GetWidgetPosition().y;
		y = Clamp(y + eventData->wheelDelta.y * 32.0F, GetWidgetSize().y - totalPageHeight - bottomMargin, topMargin);
		pageGroup.SetWidgetPosition(Point3D(0.0F, y, 0.0F));
		pageGroup.Invalidate();
	}
}

void BookWidget::BeginPageDrag(Page *dragPage)
{
	dragPage->dragFlag = true;

	Page *page = pageList.First();
	while (page)
	{
		float y = page->GetWidgetPosition().y;
		page->originalPosition = y;
		page->movePosition[0] = y;
		page->movePosition[1] = y;

		page = page->Next();
	}

	pageMoveTime = 1.0F;
	dragPreviousPage = dragPage->Previous();

	BringToFront(dragPage);
	GetRootWidget()->AddMovingWidget(this);

	pageList.Remove(dragPage);
}

void BookWidget::EndPageDrag(Page *dragPage)
{
	dragPage->dragFlag = false;

	Page *previous = FindPreviousPage(dragPage);
	if (previous)
	{
		pageList.InsertAfter(dragPage, previous);
	}
	else
	{
		pageList.Prepend(dragPage);
	}

	pageGroup.SetWidgetPosition(Point3D(0.0F, Floor(pageGroup.GetWidgetPosition().y), 0.0F));
	OrganizePages();

	GetRootWidget()->RemoveMovingWidget(this);
}

void BookWidget::UpdatePageDrag(Page *dragPage)
{
	float y = pageGroup.GetWidgetTransform().GetRow(1) ^ dragPage->GetWidgetPosition();
	if (y < 0.0F)
	{
		float speed = Fmin(TheTimeMgr->GetFloatDeltaTime(), 1.0F) * 0.05F;
		y = pageGroup.GetWidgetPosition().y - y * speed;
		pageGroup.SetWidgetPosition(Point3D(0.0F, Fmin(y, topMargin), 0.0F));
	}
	else
	{
		float height = GetWidgetSize().y;
		if (y > height)
		{
			float speed = Fmin(TheTimeMgr->GetFloatDeltaTime(), 1.0F) * 0.05F;
			y = pageGroup.GetWidgetPosition().y - (y - height) * speed;
			pageGroup.SetWidgetPosition(Point3D(0.0F, Fmax(y, height - totalPageHeight - bottomMargin), 0.0F));
		}
		else
		{
			pageGroup.SetWidgetPosition(Point3D(0.0F, Floor(pageGroup.GetWidgetPosition().y), 0.0F));
		}
	}

	pageGroup.Invalidate();

	Page *previousPage = FindPreviousPage(dragPage);
	if (dragPreviousPage != previousPage)
	{
		dragPreviousPage = previousPage;
		pageMoveTime = 0.0F;

		y = 16.0F;
		Page *page = pageList.First();
		while (page)
		{
			const Page *prev = page->Previous();
			if (prev == previousPage)
			{
				y += dragPage->GetWidgetSize().y + 21.0F;
			}

			page->movePosition[0] = page->GetWidgetPosition().y;
			page->movePosition[1] = y;

			y += page->GetWidgetSize().y + 21.0F;
			page = page->Next();
		}
	}
}

Page *BookWidget::FindPreviousPage(const Page *dragPage) const
{
	float top = dragPage->GetWidgetPosition().y;
	float bottom = top + dragPage->GetWidgetSize().y;

	Page *previous = dragPreviousPage;
	if (previous)
	{
		Page *page = pageList.First();
		do
		{
			float y = page->movePosition[1] + page->GetWidgetSize().y * 0.5F;
			if (top < y)
			{
				return (page->Previous());
			}

			if (page == previous)
			{
				break;
			}

			page = page->Next();
		} while (page);
	}

	Page *next = (previous) ? previous->Next() : pageList.First();
	if (next)
	{
		Page *page = pageList.Last();
		do
		{
			float y = page->movePosition[1] + page->GetWidgetSize().y * 0.5F;
			if (bottom > y)
			{
				return (page);
			}

			if (page == next)
			{
				break;
			}

			page = page->Previous();
		} while (page);
	}

	return (previous);
}

void BookWidget::AppendPage(Page *page)
{
	pageGroup.AppendSubnode(page);
	pageList.Append(page);
	page->owningBook = this;
}

void BookWidget::PrependPage(Page *page)
{
	pageGroup.AppendSubnode(page);
	pageList.Prepend(page);
	page->owningBook = this;
}

void BookWidget::RemovePage(Page *page)
{
	page->Tree<Widget>::Detach();
	page->ListElement<Page>::Detach();
	page->owningBook = nullptr;
}

void BookWidget::OrganizePages(void)
{
	float y = 16.0F;
	Page *page = pageList.First();
	while (page)
	{
		page->SetWidgetPosition(Point3D(0.0F, y, 0.0F));
		y += page->GetWidgetSize().y + 21.0F;

		page = page->Next();
	}

	totalPageHeight = y - 21.0F;

	y = Clamp(pageGroup.GetWidgetPosition().y, GetWidgetSize().y - totalPageHeight - bottomMargin, topMargin);
	pageGroup.SetWidgetPosition(Point3D(0.0F, y, 0.0F));
	pageGroup.Invalidate();
}

// ZYUQURM
