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


#include "C4Landscaping.h"
#include "C4WorldEditor.h"
#include "C4World.h"


using namespace C4;


LandscapingGroup::LandscapingGroup()
{
	instanceDensity = 1;
	groupOverlapAllowance = 1.0F;
	nongroupOverlapAllowance = 1.0F;
}

LandscapingGroup::LandscapingGroup(const char *name) : groupName(name)
{
	instanceDensity = 1;
	groupOverlapAllowance = 1.0F;
	nongroupOverlapAllowance = 1.0F;
}

LandscapingGroup::LandscapingGroup(const LandscapingGroup& landscapingGroup) :
		groupName(landscapingGroup.groupName),
		worldNameArray(landscapingGroup.worldNameArray),
		placementAdjuster(landscapingGroup.placementAdjuster)
{
	instanceDensity = landscapingGroup.instanceDensity;
	groupOverlapAllowance = landscapingGroup.groupOverlapAllowance;
	nongroupOverlapAllowance = landscapingGroup.nongroupOverlapAllowance;

	const Modifier *modifier = landscapingGroup.modifierList.First();
	while (modifier)
	{
		modifierList.Append(modifier->Clone());
		modifier = modifier->Next();
	}
}

LandscapingGroup::~LandscapingGroup()
{
}

void LandscapingGroup::Pack(Packer& data, unsigned_int32 packFlags) const
{
	PackHandle handle = data.BeginChunk('NAME');
	data << groupName;
	data.EndChunk(handle);

	int32 count = worldNameArray.GetElementCount();
	for (machine a = 0; a < count; a++)
	{
		handle = data.BeginChunk('WRLD');
		data << worldNameArray[a];
		data.EndChunk(handle);
	}

	data << ChunkHeader('DENS', 4);
	data << instanceDensity;

	data << ChunkHeader('OVLP', 8);
	data << groupOverlapAllowance;
	data << nongroupOverlapAllowance;

	const Modifier *modifier = modifierList.First();
	while (modifier)
	{
		handle = data.BeginChunk('MDFR');
		modifier->PackType(data);
		modifier->Pack(data, packFlags);
		data.EndChunk(handle);

		modifier = modifier->Next();
	}

	handle = data.BeginChunk('AJST');
	placementAdjuster.Pack(data, packFlags);
	data.EndChunk(handle);

	data << TerminatorChunk;
}

void LandscapingGroup::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	UnpackChunkList<LandscapingGroup>(data, unpackFlags);
}

bool LandscapingGroup::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'NAME':

			data >> groupName;
			return (true);

		case 'WRLD':
		{
			int32 count = worldNameArray.GetElementCount();
			worldNameArray.SetElementCount(count + 1);
			data >> worldNameArray[count]; 
			return (true);
		}
 
		case 'DENS':
 
			data >> instanceDensity;
			return (true);
 
		case 'OVLP':
 
			data >> groupOverlapAllowance; 
			data >> nongroupOverlapAllowance;
			return (true);

		case 'MDFR': 
		{
			Modifier *modifier = Modifier::Create(data, unpackFlags);
			if (modifier)
			{
				modifier->Unpack(++data, unpackFlags);
				modifierList.Append(modifier);
				return (true);
			}

			break;
		}

		case 'AJST':

			placementAdjuster.Unpack(data, unpackFlags);
			return (true);
	}

	return (false);
}

int32 LandscapingGroup::GetSettingCount(void) const
{
	return (placementAdjuster.GetSettingCount() + 5);
}

Setting *LandscapingGroup::GetSetting(int32 index) const
{
	if (index < 5)
	{
		const StringTable *table = TheWorldEditor->GetStringTable();

		if (index == 0)
		{
			const char *title = table->GetString(StringID('LAND', 'INST'));
			return (new HeadingSetting('INST', title));
		}

		if (index == 1)
		{
			const char *title = table->GetString(StringID('LAND', 'DENS'));
			return (new TextSetting('DENS', Text::IntegerToString(instanceDensity), title, 4, &EditTextWidget::NumberFilter));
		}

		if (index == 2)
		{
			const char *title = table->GetString(StringID('LAND', 'OGRP'));
			return (new IntegerSetting('OGRP', (int32) (groupOverlapAllowance * 100.0F + 0.5F), title, 0, 100, 1));
		}

		if (index == 3)
		{
			const char *title = table->GetString(StringID('LAND', 'OOUT'));
			return (new IntegerSetting('OOUT', (int32) (nongroupOverlapAllowance * 100.0F + 0.5F), title, 0, 100, 1));
		}

		if (index == 4)
		{
			const char *title = table->GetString(StringID('LAND', 'AJST'));
			return (new HeadingSetting('AJST', title));
		}
	}
	else
	{
		return (placementAdjuster.GetSetting(index - 5));
	}

	return (nullptr);
}

void LandscapingGroup::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'DENS')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		instanceDensity = Max(Text::StringToInteger(text), 1);
	}
	else if (identifier == 'OGRP')
	{
		int32 value = static_cast<const IntegerSetting *>(setting)->GetIntegerValue();
		groupOverlapAllowance = (float) value * 0.01F;
	}
	else if (identifier == 'OOUT')
	{
		int32 value = static_cast<const IntegerSetting *>(setting)->GetIntegerValue();
		nongroupOverlapAllowance = (float) value * 0.01F;
	}
	else
	{
		placementAdjuster.SetSetting(setting);
	}
}

bool LandscapingGroup::AddWorldName(const ResourceName& name)
{
	int32 count = worldNameArray.GetElementCount();
	for (machine a = 0; a < count; a++)
	{
		if (Text::CompareTextCaseless(worldNameArray[a], name))
		{
			return (false);
		}
	}

	worldNameArray.AddElement(name);
	return (true);
}

bool LandscapingGroup::RemoveWorldName(const ResourceName& name)
{
	int32 count = worldNameArray.GetElementCount();
	for (machine a = 0; a < count; a++)
	{
		if (Text::CompareTextCaseless(worldNameArray[a], name))
		{
			worldNameArray.RemoveElement(a);
			return (true);
		}
	}

	return (false);
}


LandscapingBrush::LandscapingBrush(const LandscapingBrush *brush)
{
	if (brush)
	{
		CopyBrush(brush);
	}
}

LandscapingBrush::~LandscapingBrush()
{
}

void LandscapingBrush::Pack(Packer& data, unsigned_int32 packFlags) const
{
	const LandscapingGroup *group = groupList.First();
	while (group)
	{
		PackHandle handle = data.BeginChunk('GRUP');
		group->Pack(data, packFlags);
		data.EndChunk(handle);

		group = group->Next();
	}

	data << TerminatorChunk;
}

void LandscapingBrush::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	UnpackChunkList<LandscapingBrush>(data, unpackFlags);
}

bool LandscapingBrush::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'GRUP':
		{
			LandscapingGroup *group = new LandscapingGroup;
			group->Unpack(data, unpackFlags);
			groupList.Append(group);
			return (true);
		}
	}

	return (false);
}

void LandscapingBrush::CopyBrush(const LandscapingBrush *brush)
{
	groupList.Purge();

	const LandscapingGroup *group = brush->groupList.First();
	while (group)
	{
		groupList.Append(new LandscapingGroup(*group));
		group = group->Next();
	}
}


LandscapingPage::LandscapingPage() :
		EditorPage(kEditorPageLandscaping, "WorldEditor/page/Landscaping", kEditorBookEarth),
		brushListWidgetObserver(this, &LandscapingPage::HandleBrushListWidgetEvent),
		menuButtonObserver(this, &LandscapingPage::HandleMenuButtonEvent)
{
}

LandscapingPage::~LandscapingPage()
{
}

LandscapingPage::BrushWidget::BrushWidget() : TextWidget(Zero2D, nullptr, "font/Normal")
{
}

LandscapingPage::BrushWidget::BrushWidget(const Vector2D& size, const char *name, const LandscapingBrush *brush) :
		TextWidget(size, name, "font/Normal"),
		landscapingBrush(brush)
{
}

LandscapingPage::BrushWidget::~BrushWidget()
{
}

void LandscapingPage::BrushWidget::Pack(Packer& data, unsigned_int32 packFlags) const
{
	PackHandle handle = data.BeginChunk('NAME');
	data << GetText();
	data.EndChunk(handle);

	handle = data.BeginChunk('BRSH');
	landscapingBrush.Pack(data, packFlags);
	data.EndChunk(handle);

	data << TerminatorChunk;
}

void LandscapingPage::BrushWidget::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	UnpackChunkList<BrushWidget>(data, unpackFlags);
}

bool LandscapingPage::BrushWidget::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'NAME':

			SetText(data.ReadString());
			return (true);

		case 'BRSH':

			landscapingBrush.Unpack(data, unpackFlags);
			return (true);
	}

	return (false);
}

void LandscapingPage::Pack(Packer& data, unsigned_int32 packFlags) const
{
	EditorPage::Pack(data, packFlags);

	const BrushWidget *brushWidget = brushWidgetMap.First();
	while (brushWidget)
	{
		PackHandle handle = data.BeginChunk('BRSH');
		brushWidget->Pack(data, packFlags);
		data.EndChunk(handle);

		brushWidget = brushWidget->MapElement<BrushWidget>::Next();
	}

	data << TerminatorChunk;
}

void LandscapingPage::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	EditorPage::Unpack(data, unpackFlags);
	UnpackChunkList<LandscapingPage>(data, unpackFlags);
}

bool LandscapingPage::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'BRSH':
		{
			BrushWidget *brushWidget = new BrushWidget;
			brushWidget->Unpack(data, unpackFlags);
			brushWidgetMap.Insert(brushWidget);
			return (true);
		}
	}

	return (false);
}

void LandscapingPage::Preprocess(void)
{
	EditorPage::Preprocess();

	additiveButton = static_cast<IconButtonWidget *>(FindWidget("Add"));
	subtractiveButton = static_cast<IconButtonWidget *>(FindWidget("Subtract"));
	densitySlider = static_cast<SliderWidget *>(FindWidget("Density"));

	brushListWidget = static_cast<ListWidget *>(FindWidget("List"));
	brushListWidget->SetWidgetUsage(brushListWidget->GetWidgetUsage() & ~kWidgetKeyboardFocus);
	brushListWidget->SetObserver(&brushListWidgetObserver);

	Vector2D size = brushListWidget->GetNaturalListItemSize();
	BrushWidget *brushWidget = brushWidgetMap.First();
	while (brushWidget)
	{
		brushWidget->SetWidgetSize(size);
		brushListWidget->AppendListItem(brushWidget);

		brushWidget = brushWidget->MapElement<BrushWidget>::Next();
	}

	menuButton = static_cast<IconButtonWidget *>(FindWidget("Menu"));
	menuButton->SetObserver(&menuButtonObserver);

	const StringTable *table = TheWorldEditor->GetStringTable();

	landscapingMenuItemList.Append(new MenuItemWidget(table->GetString(StringID('PAGE', kEditorPageLandscaping, 'NWLB')), WidgetObserver<LandscapingPage>(this, &LandscapingPage::HandleNewBrushMenuItemEvent)));

	MenuItemWidget *widget = new MenuItemWidget(table->GetString(StringID('PAGE', kEditorPageLandscaping, 'DLLB')), WidgetObserver<LandscapingPage>(this, &LandscapingPage::HandleDeleteBrushMenuItemEvent));
	landscapingMenuItem[kLandscapingMenuDeleteBrush] = widget;
	landscapingMenuItemList.Append(widget);

	widget = new MenuItemWidget(table->GetString(StringID('PAGE', kEditorPageLandscaping, 'RNLB')), WidgetObserver<LandscapingPage>(this, &LandscapingPage::HandleRenameBrushMenuItemEvent));
	landscapingMenuItem[kLandscapingMenuRenameBrush] = widget;
	landscapingMenuItemList.Append(widget);

	landscapingMenuItemList.Append(new MenuItemWidget(kLineSolid));
	landscapingMenuItemList.Append(new MenuItemWidget(table->GetString(StringID('PAGE', kEditorPageLandscaping, 'IMLB')), WidgetObserver<LandscapingPage>(this, &LandscapingPage::HandleImportBrushMenuItemEvent)));

	widget = new MenuItemWidget(table->GetString(StringID('PAGE', kEditorPageLandscaping, 'EXLB')), WidgetObserver<LandscapingPage>(this, &LandscapingPage::HandleExportBrushMenuItemEvent));
	landscapingMenuItem[kLandscapingMenuExportBrush] = widget;
	landscapingMenuItemList.Append(widget);
}

bool LandscapingPage::BrushNameAllowed(const char *name) const
{
	const BrushWidget *brushWidget = brushWidgetMap.First();
	while (brushWidget)
	{
		if (Text::CompareTextCaseless(brushWidget->GetText(), name))
		{
			return (false);
		}

		brushWidget = brushWidget->MapElement<BrushWidget>::Next();
	}

	return (true);
}

LandscapingBrush *LandscapingPage::AddBrush(const char *name, const LandscapingBrush *brush)
{
	BrushWidget *brushWidget = new BrushWidget(brushListWidget->GetNaturalListItemSize(), name, brush);
	brushListWidget->InsertSortedListItem(brushWidget);
	brushWidgetMap.Insert(brushWidget);
	return (&brushWidget->landscapingBrush);
}

void LandscapingPage::HandleNewBrushMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData)
{
	GetEditor()->AddSubwindow(new LandscapingEditor(this));
}

void LandscapingPage::HandleDeleteBrushMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData)
{
}

void LandscapingPage::HandleRenameBrushMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData)
{
}

void LandscapingPage::HandleImportBrushMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData)
{
}

void LandscapingPage::HandleExportBrushMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData)
{
}

void LandscapingPage::HandleBrushListWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		BrushWidget *brushWidget = static_cast<BrushWidget *>(brushListWidget->GetFirstSelectedListItem());
		GetEditor()->AddSubwindow(new LandscapingEditor(this, &brushWidget->landscapingBrush));
	}
}

void LandscapingPage::HandleMenuButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		if (brushListWidget->GetFirstSelectedListItem())
		{
			landscapingMenuItem[kLandscapingMenuDeleteBrush]->Enable();
			landscapingMenuItem[kLandscapingMenuRenameBrush]->Enable();
			landscapingMenuItem[kLandscapingMenuExportBrush]->Enable();
		}
		else
		{
			landscapingMenuItem[kLandscapingMenuDeleteBrush]->Disable();
			landscapingMenuItem[kLandscapingMenuRenameBrush]->Disable();
			landscapingMenuItem[kLandscapingMenuExportBrush]->Disable();
		}

		Menu *menu = new Menu(kMenuContextual, &landscapingMenuItemList);
		menu->SetWidgetPosition(menuButton->GetWorldPosition() + Vector3D(25.0F, 0.0F, 0.0F));
		TheInterfaceMgr->SetActiveMenu(menu);
	}
}

void LandscapingPage::Engage(Editor *editor, void *cookie)
{
}

void LandscapingPage::Disengage(Editor *editor, void *cookie)
{
}

bool LandscapingPage::BeginTool(Editor *editor, EditorTrackData *trackData)
{
	return (false);
}

bool LandscapingPage::TrackTool(Editor *editor, EditorTrackData *trackData)
{
	return (false);
}

bool LandscapingPage::EndTool(Editor *editor, EditorTrackData *trackData)
{
	return (false);
}


LandscapingEditor::LandscapingEditor(LandscapingPage *page, LandscapingBrush *brush) :
		Window("LandscapingEditor/Window"),
		landscapingBrush(brush),
		instanceConfigurationObserver(this, &LandscapingEditor::HandleInstanceConfigurationEvent),
		modifierConfigurationObserver(this, &LandscapingEditor::HandleModifierConfigurationEvent)
{
	editorState = 0;

	landscapingPage = page;
	originalBrush = brush;

	SetMinWindowSize(Vector2D(960.0F, 564.0F));

	landscapingWorld = nullptr;
	toolTracking = false;

	lightAzimuth = -K::tau_over_8;
	lightAltitude = K::tau_over_8;
}

LandscapingEditor::~LandscapingEditor()
{
	delete landscapingWorld;
}

LandscapingEditor::BrushWindow::BrushWindow(LandscapingPage *page, LandscapingEditor *editor) : Window("LandscapingEditor/Brush")
{
	landscapingPage = page;
	landscapingEditor = editor;
}

LandscapingEditor::BrushWindow::~BrushWindow()
{
}

void LandscapingEditor::BrushWindow::Preprocess(void)
{
	Window::Preprocess();

	okayButton = static_cast<PushButtonWidget *>(FindWidget("OK"));
	cancelButton = static_cast<PushButtonWidget *>(FindWidget("Cancel"));

	nameBox = static_cast<EditTextWidget *>(FindWidget("Name"));
	SetFocusWidget(nameBox);
}

void LandscapingEditor::BrushWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	EventType eventType = eventData->eventType;
	if (eventType == kEventWidgetActivate)
	{
		if (widget == okayButton)
		{
			CallCompletionProc();
			Close();
		}
		else if (widget == cancelButton)
		{
			Close();
		}
	}
	else if (eventType == kEventWidgetChange)
	{
		if (widget == nameBox)
		{
			const char *name = nameBox->GetText();
			if (name[0] != 0)
			{
				if (landscapingPage->BrushNameAllowed(name))
				{
					okayButton->Enable();
				}
				else
				{
					okayButton->Disable();
				}
			}
			else
			{
				okayButton->Disable();
			}
		}
	}
}

LandscapingEditor::GroupWidget::GroupWidget(const Vector2D& size, LandscapingGroup *group) : TextWidget(size, group->GetGroupName(), "font/Normal")
{
	landscapingGroup = group;
}

LandscapingEditor::GroupWidget::~GroupWidget()
{
}

LandscapingEditor::GroupWindow::GroupWindow(LandscapingEditor *editor, const char *name) :
		Window("LandscapingEditor/Group"),
		groupName(name)
{
	landscapingEditor = editor;
}

LandscapingEditor::GroupWindow::~GroupWindow()
{
}

void LandscapingEditor::GroupWindow::Preprocess(void)
{
	Window::Preprocess();

	okayButton = static_cast<PushButtonWidget *>(FindWidget("OK"));
	cancelButton = static_cast<PushButtonWidget *>(FindWidget("Cancel"));

	nameBox = static_cast<EditTextWidget *>(FindWidget("Name"));
	nameBox->SetText(groupName);
	SetFocusWidget(nameBox);
}

void LandscapingEditor::GroupWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	EventType eventType = eventData->eventType;
	if (eventType == kEventWidgetActivate)
	{
		if (widget == okayButton)
		{
			CallCompletionProc();
			Close();
		}
		else if (widget == cancelButton)
		{
			Close();
		}
	}
	else if (eventType == kEventWidgetChange)
	{
		if (widget == nameBox)
		{
			const char *name = nameBox->GetText();
			if (name[0] != 0)
			{
				if ((groupName == name) || (landscapingEditor->GroupNameAllowed(name)))
				{
					okayButton->Enable();
				}
				else
				{
					okayButton->Disable();
				}
			}
			else
			{
				okayButton->Disable();
			}
		}
	}
}

LandscapingEditor::ModifierWidget::ModifierWidget(const Vector2D& size, Modifier *modifier) : TextWidget(size, Modifier::FindRegistration(modifier->GetModifierType())->GetModifierName(), "font/Normal")
{
	instanceModifier = modifier;
}

LandscapingEditor::ModifierWidget::~ModifierWidget()
{
}

LandscapingEditor::ModifierWindow::ModifierWindow(LandscapingEditor *editor) : Window("LandscapingEditor/Modifier")
{
	landscapingEditor = editor;
}

LandscapingEditor::ModifierWindow::~ModifierWindow()
{
}

void LandscapingEditor::ModifierWindow::Preprocess(void)
{
	Window::Preprocess();

	okayButton = static_cast<PushButtonWidget *>(FindWidget("OK"));
	cancelButton = static_cast<PushButtonWidget *>(FindWidget("Cancel"));

	modifierListWidget = static_cast<ListWidget *>(FindWidget("List"));
	SetFocusWidget(modifierListWidget);

	const ModifierRegistration *registration = Modifier::GetFirstRegistration();
	while (registration)
	{
		modifierListWidget->AppendListItem(registration->GetModifierName());
		registration = registration->Next();
	}
}

void LandscapingEditor::ModifierWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	EventType eventType = eventData->eventType;
	if (eventType == kEventWidgetActivate)
	{
		if ((widget == okayButton) || (widget == modifierListWidget))
		{
			CallCompletionProc();
			Close();
		}
		else if (widget == cancelButton)
		{
			Close();
		}
	}
	else if (eventType == kEventWidgetChange)
	{
		if (widget == modifierListWidget)
		{
			okayButton->Enable();
		}
	}
}

ModifierType LandscapingEditor::ModifierWindow::GetModifierType(void) const
{
	int32 index = modifierListWidget->GetFirstSelectedListItem()->GetNodeIndex();
	const ModifierRegistration *registration = Modifier::GetRegistration(index);
	return (registration->GetModifierType());
}

void LandscapingEditor::SetWidgetSize(const Vector2D& size)
{
	Window::SetWidgetSize(size);

	PositionWidgets();
	const Vector2D& viewportSize = landscapingViewport->GetWidgetSize();
	landscapingWorld->SetRenderSize((int32) viewportSize.x, (int32) viewportSize.y);
}

void LandscapingEditor::Preprocess(void)
{
	Window::Preprocess();

	landscapingViewport = static_cast<FrustumViewportWidget *>(FindWidget("Viewport"));
	viewportBorder = static_cast<BorderWidget *>(FindWidget("Border"));

	landscapingViewport->SetMouseEventProc(&ViewportHandleMouseEvent, this);
	landscapingViewport->SetTrackTaskProc(&ViewportTrackTask, this);
	landscapingViewport->SetRenderProc(&ViewportRender, this);
	landscapingViewport->SetCameraTransform(K::tau_over_2, 0.0F, Point3D(100.0F, 0.0F, 20.0F));

	settingsGroup = FindWidget("Group");
	multipaneWidget = static_cast<MultipaneWidget *>(FindWidget("Multipane"));

	instanceConfigurationWidget = static_cast<ConfigurationWidget *>(FindWidget("InstanceConfig"));
	instanceConfigurationWidget->SetObserver(&instanceConfigurationObserver);

	groupListWidget = static_cast<ListWidget *>(FindWidget("GroupList"));
	addGroupButton = static_cast<IconButtonWidget *>(FindWidget("AddGroup"));
	removeGroupButton = static_cast<IconButtonWidget *>(FindWidget("RemoveGroup"));

	worldListWidget = static_cast<ListWidget *>(FindWidget("WorldList"));
	addWorldButton = static_cast<IconButtonWidget *>(FindWidget("AddWorld"));
	removeWorldButton = static_cast<IconButtonWidget *>(FindWidget("RemoveWorld"));

	modifierListWidget = static_cast<ListWidget *>(FindWidget("InitList"));
	addModifierButton = static_cast<IconButtonWidget *>(FindWidget("AddInit"));
	removeModifierButton = static_cast<IconButtonWidget *>(FindWidget("RemoveInit"));

	modifierConfigurationWidget = static_cast<ConfigurationWidget *>(FindWidget("InitConfig"));
	modifierConfigurationWidget->SetObserver(&modifierConfigurationObserver);

	menuBar = static_cast<MenuBarWidget *>(FindWidget("Menu"));

	PositionWidgets();
	BuildMenus();

	BuildLandscapingWorld();

	if (originalBrush)
	{
		Vector2D size = groupListWidget->GetNaturalListItemSize();

		LandscapingGroup *group = landscapingBrush.GetFirstGroup();
		while (group)
		{
			GroupWidget *groupWidget = new GroupWidget(size, group);
			groupListWidget->AppendListItem(groupWidget);
			group = group->Next();
		}

		GenerateLandscapingPreview();
	}
}

void LandscapingEditor::PositionWidgets(void)
{
	float width = GetWidgetSize().x;
	float height = GetWidgetSize().y;

	const Point3D& position = landscapingViewport->GetWidgetPosition();
	Vector2D viewportSize(width - position.x - 344.0F, height - position.y - 4.0F);

	landscapingViewport->SetWidgetSize(viewportSize);
	viewportBorder->SetWidgetSize(viewportSize);

	settingsGroup->SetWidgetPosition(Point3D(width - 339.0F, 24.0F, 0.0F));
	settingsGroup->Invalidate();

	float menuBarHeight = menuBar->GetWidgetSize().y;
	menuBar->SetWidgetSize(Vector2D(width, menuBarHeight));

	float x = position.x;
	float y = position.y;
	float w = viewportSize.x;
	float h = viewportSize.y;

	SetBackgroundQuad(0, Point3D(0.0F, 0.0F, 0.0F), Vector2D(x - 1.0F, height));
	SetBackgroundQuad(1, Point3D(x + w + 1.0F, 0.0F, 0.0F), Vector2D(width - x - w - 1.0F, height));
	SetBackgroundQuad(2, Point3D(x - 1.0F, 0.0F, 0.0F), Vector2D(width - x + 1.0F, y));
	SetBackgroundQuad(3, Point3D(x - 1.0F, y + h + 1.0F, 0.0F), Vector2D(width - x + 1.0F, height - y - h - 1.0F));
}

void LandscapingEditor::BuildMenus(void)
{
	const StringTable *table = TheWorldEditor->GetStringTable();

	landscapingMenu = new PulldownMenuWidget(table->GetString(StringID('LAND', 'MENU', 'LAND')));
	menuBar->AppendMenu(landscapingMenu);

	landscapingMenu->AppendMenuItem(new MenuItemWidget(table->GetString(StringID('LAND', 'MENU', 'LAND', 'CLOS')), WidgetObserver<LandscapingEditor>(this, &LandscapingEditor::HandleCloseMenuItem), Shortcut('W')));
	landscapingMenu->AppendMenuItem(new MenuItemWidget(table->GetString(StringID('LAND', 'MENU', 'LAND', 'SAVE')), WidgetObserver<LandscapingEditor>(this, &LandscapingEditor::HandleSaveBrushMenuItem), Shortcut('S')));

	landscapingMenu->AppendMenuItem(new MenuItemWidget(kLineSolid));

	landscapingMenu->AppendMenuItem(new MenuItemWidget(table->GetString(StringID('LAND', 'MENU', 'LAND', 'RGEN')), WidgetObserver<LandscapingEditor>(this, &LandscapingEditor::HandleRegeneratePreviewMenuItem), Shortcut('R')));
}

void LandscapingEditor::HandleInstanceConfigurationEvent(SettingInterface *settingInterface)
{
	LandscapingGroup *group = static_cast<GroupWidget *>(groupListWidget->GetFirstSelectedListItem())->landscapingGroup;
	instanceConfigurationWidget->CommitConfiguration(group);

	SetModifiedFlag();
}

void LandscapingEditor::HandleModifierConfigurationEvent(SettingInterface *settingInterface)
{
	Modifier *modifier = static_cast<ModifierWidget *>(modifierListWidget->GetFirstSelectedListItem())->instanceModifier;
	modifierConfigurationWidget->CommitConfiguration(modifier);

	SetModifiedFlag();
}

void LandscapingEditor::HandleCloseMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	Close();
}

void LandscapingEditor::HandleSaveBrushMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	if (originalBrush)
	{
		originalBrush->CopyBrush(&landscapingBrush);
		editorState &= ~kLandscapingEditorModified;
	}
	else
	{
		BrushWindow *window = new BrushWindow(landscapingPage, this);
		window->SetCompletionProc(&SaveBrushComplete, this);
		AddSubwindow(window);
	}
}

void LandscapingEditor::HandleRegeneratePreviewMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	GenerateLandscapingPreview();
}

void LandscapingEditor::BuildLandscapingWorld(void)
{
	zoneNode = new InfiniteZone;

	ZoneObject *zoneObject = new InfiniteZoneObject;
	zoneNode->SetObject(zoneObject);
	zoneObject->Release();

	zoneObject->SetAmbientLight(ColorRGBA(0.5F, 0.5F, 0.5F, 1.0F));
	zoneObject->SetEnvironmentMap("C4/environment");

	BoxGeometry *ground = new BoxGeometry(Vector3D(120.0F, 120.0F, 20.0F));
	ground->SetNodePosition(Point3D(-60.0F, -60.0F, -20.0F));
	groundNode = ground;

	BoxGeometryObject *groundObject = ground->GetObject();
	groundObject->SetGeometryFlags((groundObject->GetGeometryFlags() | kGeometryMotionBlurInhibit) & ~kGeometryCastShadows);

	MaterialObject *materialObject = new MaterialObject;
	materialObject->AddAttribute(new DiffuseAttribute(ColorRGBA(0.75F, 0.75F, 0.75F, 1.0F)));
	ground->SetMaterialObject(0, materialObject);
	materialObject->Release();

	ground->Update();
	groundObject->Build(ground);
	zoneNode->AppendSubnode(ground);

	lightNode = new InfiniteLight(ColorRGB(1.0F, 1.0F, 1.0F));
	lightNode->SetNodeMatrix3D(Matrix3D().SetRotationAboutZ(lightAzimuth) * Matrix3D().SetRotationAboutY(lightAltitude));
	zoneNode->AppendSubnode(lightNode);

	previewNode = new Node;
	previewNode->SetNodePosition(Point3D(-50.0F, -50.0F, 0.0F));
	zoneNode->AppendSubnode(previewNode);

	landscapingWorld = new World(zoneNode, kWorldViewport | kWorldClearColor | kWorldZeroBackgroundVelocity | kWorldListenerInhibit);
	landscapingWorld->Preprocess();

	const Vector2D& viewportSize = landscapingViewport->GetWidgetSize();
	landscapingWorld->SetRenderSize((int32) viewportSize.x, (int32) viewportSize.y);

	FrustumCamera *camera = landscapingViewport->GetViewportCamera();
	camera->GetObject()->SetClearColor(ColorRGBA(0.0F, 0.5F, 1.0F, 0.0F));
	landscapingWorld->SetCamera(camera);
}

void LandscapingEditor::GenerateLandscapingPreview(void)
{
	previewNode->PurgeSubtree();

	const LandscapingGroup *group = landscapingBrush.GetFirstGroup();
	while (group)
	{
		int32 worldCount = group->GetWorldNameCount();
		if (worldCount != 0)
		{
			int32 density = group->GetInstanceDensity();
			for (machine a = 0; a < density; a++)
			{
				Instance *node = new Instance(group->GetWorldName(Math::Random(worldCount)));
				previewNode->AppendSubnode(node);

				Transform4D transform(Identity3D, Point3D(Math::RandomFloat(100.0F), Math::RandomFloat(100.0F), 0.0F));
				group->GetPlacementAdjuster()->AdjustPlacement(landscapingWorld, &transform, K::z_unit);
				node->SetNodeTransform(transform);

				node->Expand(landscapingWorld);
				node->Preprocess();
				node->Invalidate();
			}
		}

		group = group->Next();
	}
}

void LandscapingEditor::ShowSettings(void)
{
	multipaneWidget->Show();
	SelectPane(multipaneWidget->GetSelection());
}

void LandscapingEditor::HideSettings(void)
{
	multipaneWidget->Hide();
	SelectPane(-1);
}

void LandscapingEditor::SelectPane(int32 index)
{
	if (index == 0)
	{
		worldListWidget->Show();
		addWorldButton->Show();
		removeWorldButton->Show();
	}
	else
	{
		worldListWidget->Hide();
		addWorldButton->Hide();
		removeWorldButton->Hide();
	}

	if (index == 1)
	{
		instanceConfigurationWidget->Show();
	}
	else
	{
		instanceConfigurationWidget->Hide();
	}

	if (index == 2)
	{
		modifierListWidget->Show();
		addModifierButton->Show();
		removeModifierButton->Show();
		modifierConfigurationWidget->Show();
	}
	else
	{
		modifierListWidget->Hide();
		addModifierButton->Hide();
		removeModifierButton->Hide();
		modifierConfigurationWidget->Hide();
	}
}

void LandscapingEditor::SelectGroup(const LandscapingGroup *group)
{
	ShowSettings();

	BuildWorldList(group);
	BuildModifierList(group);

	instanceConfigurationWidget->ReleaseConfiguration();
	instanceConfigurationWidget->BuildConfiguration(group);

	removeGroupButton->Enable();
}

void LandscapingEditor::SelectModifier(const Modifier *modifier)
{
	modifierConfigurationWidget->ReleaseConfiguration();
	modifierConfigurationWidget->BuildConfiguration(modifier);

	removeModifierButton->Enable();
}

void LandscapingEditor::BuildWorldList(const LandscapingGroup *group)
{
	worldListWidget->PurgeListItems();

	int32 count = group->GetWorldNameCount();
	for (machine a = 0; a < count; a++)
	{
		worldListWidget->InsertSortedListItem(group->GetWorldName(a), &Text::CompareNumberedTextLessThanCaseless);
	}

	removeWorldButton->Disable();
}

void LandscapingEditor::BuildModifierList(const LandscapingGroup *group)
{
	modifierListWidget->PurgeListItems();

	Modifier *modifier = group->GetFirstModifier();
	while (modifier)
	{
		modifierListWidget->AppendListItem(new ModifierWidget(modifierListWidget->GetNaturalListItemSize(), modifier));
		modifier = modifier->Next();
	}

	removeModifierButton->Disable();
}

bool LandscapingEditor::GroupNameAllowed(const char *name) const
{
	const LandscapingGroup *group = landscapingBrush.GetFirstGroup();
	while (group)
	{
		if (Text::CompareTextCaseless(group->GetGroupName(), name))
		{
			return (false);
		}

		group = group->Next();
	}

	return (true);
}

void LandscapingEditor::CloseDialogComplete(Dialog *dialog, void *cookie)
{
	LandscapingEditor *landscapingEditor = static_cast<LandscapingEditor *>(cookie);

	int32 status = dialog->GetDialogStatus();
	if (status == kDialogOkay)
	{
		landscapingEditor->HandleSaveBrushMenuItem(nullptr, nullptr);
		if (!(landscapingEditor->editorState & kLandscapingEditorModified))
		{
			landscapingEditor->Window::Close();
		}
	}
	else if (status == kDialogIgnore)
	{
		landscapingEditor->Window::Close();
	}
}

void LandscapingEditor::SaveBrushComplete(BrushWindow *window, void *cookie)
{
	LandscapingEditor *landscapingEditor = static_cast<LandscapingEditor *>(cookie);

	landscapingEditor->originalBrush = landscapingEditor->landscapingPage->AddBrush(window->GetLandscapingBrushName(), &landscapingEditor->landscapingBrush);
	landscapingEditor->editorState &= ~kLandscapingEditorModified;
}

void LandscapingEditor::NewGroupComplete(GroupWindow *window, void *cookie)
{
	LandscapingEditor *landscapingEditor = static_cast<LandscapingEditor *>(cookie);

	const char *name = window->GetLandscapingGroupName();
	LandscapingGroup *group = new LandscapingGroup(name);
	landscapingEditor->landscapingBrush.AddGroup(group);

	ListWidget *listWidget = landscapingEditor->groupListWidget;
	GroupWidget *listItem = new GroupWidget(listWidget->GetNaturalListItemSize(), group);
	listWidget->InsertSortedListItem(listItem, &Text::CompareNumberedTextLessThanCaseless);
	listWidget->SelectListItem(listItem->GetNodeIndex());

	landscapingEditor->SelectGroup(group);
	landscapingEditor->SetModifiedFlag();
}

void LandscapingEditor::RenameGroupComplete(GroupWindow *window, void *cookie)
{
	LandscapingEditor *landscapingEditor = static_cast<LandscapingEditor *>(cookie);

	ListWidget *listWidget = landscapingEditor->groupListWidget;
	GroupWidget *listItem = static_cast<GroupWidget *>(listWidget->GetFirstSelectedListItem());

	const char *name = window->GetLandscapingGroupName();
	listItem->landscapingGroup->SetGroupName(name);
	listItem->SetText(name);

	listWidget->RemoveListItem(listItem);
	listWidget->InsertSortedListItem(listItem, &Text::CompareNumberedTextLessThanCaseless);

	landscapingEditor->SetModifiedFlag();
}

void LandscapingEditor::NewWorldComplete(FilePicker *picker, void *cookie)
{
	LandscapingEditor *landscapingEditor = static_cast<LandscapingEditor *>(cookie);
	LandscapingGroup *group = static_cast<GroupWidget *>(landscapingEditor->groupListWidget->GetFirstSelectedListItem())->landscapingGroup;

	bool rebuild = false;
	int32 count = picker->GetFileNameCount();
	for (machine a = 0; a < count; a++)
	{
		rebuild |= group->AddWorldName(picker->GetResourceName(a));
	}

	if (rebuild)
	{
		landscapingEditor->BuildWorldList(group);
	}
	else
	{
		landscapingEditor->worldListWidget->UnselectAllListItems();
	}

	for (machine a = 0; a < count; a++)
	{
		const Widget *listItem = landscapingEditor->worldListWidget->FindListItem(picker->GetResourceName(a), &Text::CompareTextCaseless);
		if (listItem)
		{
			landscapingEditor->worldListWidget->SelectListItem(listItem->GetNodeIndex());
		}
	}

	landscapingEditor->removeWorldButton->Enable();
	landscapingEditor->SetModifiedFlag();
}

void LandscapingEditor::NewModifierComplete(ModifierWindow *window, void *cookie)
{
	LandscapingEditor *landscapingEditor = static_cast<LandscapingEditor *>(cookie);
	LandscapingGroup *group = static_cast<GroupWidget *>(landscapingEditor->groupListWidget->GetFirstSelectedListItem())->landscapingGroup;

	Modifier *modifier = Modifier::New(window->GetModifierType());
	group->AddModifier(modifier);

	landscapingEditor->BuildModifierList(group);
	landscapingEditor->modifierListWidget->SelectListItem(landscapingEditor->modifierListWidget->GetListItemCount() - 1);
	landscapingEditor->SelectModifier(modifier);

	landscapingEditor->SetModifiedFlag();
}

void LandscapingEditor::ViewportHandleMouseEvent(const MouseEventData *eventData, ViewportWidget *viewport, void *cookie)
{
	LandscapingEditor *landscapingEditor = static_cast<LandscapingEditor *>(cookie);

	EventType eventType = eventData->eventType;
	if (eventType == kEventMouseDown)
	{
		landscapingEditor->toolTracking = true;
		landscapingEditor->previousPosition = eventData->mousePosition;
	}
	else if ((eventType == kEventMouseUp) || (eventType == kEventRightMouseUp))
	{
		landscapingEditor->toolTracking = false;
	}
	else if (eventType == kEventMouseMoved)
	{
		if (landscapingEditor->toolTracking)
		{
			FrustumViewportWidget *frustumViewport = static_cast<FrustumViewportWidget *>(viewport);

			float dx = (landscapingEditor->previousPosition.x - eventData->mousePosition.x) * (8.0F / frustumViewport->GetWidgetSize().x);
			float dy = InterfaceMgr::GetShiftKey() ? 0.0F : (landscapingEditor->previousPosition.y - eventData->mousePosition.y) * (8.0F / frustumViewport->GetWidgetSize().y);

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

		landscapingEditor->previousPosition = eventData->mousePosition;
	}
}

void LandscapingEditor::ViewportTrackTask(const Point3D& position, ViewportWidget *viewport, void *cookie)
{
	//LandscapingEditor *landscapingEditor = static_cast<LandscapingEditor *>(cookie);
}

void LandscapingEditor::ViewportRender(List<Renderable> *renderList, ViewportWidget *viewport, void *cookie)
{
	LandscapingEditor *landscapingEditor = static_cast<LandscapingEditor *>(cookie);

	World *world = landscapingEditor->landscapingWorld;
	world->Move();
	world->Update();
	world->BeginRendering();
	world->Render();
	world->EndRendering();
}

void LandscapingEditor::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	EventType eventType = eventData->eventType;

	if (eventType == kEventWidgetActivate)
	{
		if (widget == groupListWidget)
		{
			const LandscapingGroup *group = static_cast<GroupWidget *>(groupListWidget->GetFirstSelectedListItem())->landscapingGroup;
			GroupWindow *window = new GroupWindow(this, group->GetGroupName());
			window->SetCompletionProc(&RenameGroupComplete, this);
			AddSubwindow(window);
		}
		else if (widget == addGroupButton)
		{
			GroupWindow *window = new GroupWindow(this);
			window->SetCompletionProc(&NewGroupComplete, this);
			AddSubwindow(window);
		}
		else if (widget == removeGroupButton)
		{
			GroupWidget *listItem = static_cast<GroupWidget *>(groupListWidget->GetFirstSelectedListItem());
			groupListWidget->RemoveListItem(listItem);
			delete listItem->landscapingGroup;
			delete listItem;

			HideSettings();
			SetModifiedFlag();
		}
		else if (widget == addWorldButton)
		{
			const StringTable *table = TheWorldEditor->GetStringTable();
			WorldPicker *picker = new WorldPicker('LAND', table->GetString(StringID('LAND', 'PICK')), TheResourceMgr->GetGenericCatalog(), WorldResource::GetDescriptor(), nullptr, kFilePickerMultipleSelection);
			picker->SetCompletionProc(&NewWorldComplete, this);
			AddSubwindow(picker);
		}
		else if (widget == removeWorldButton)
		{
			Widget *listItem = worldListWidget->GetFirstSelectedListItem();
			while (listItem)
			{
				Widget *next = worldListWidget->GetNextSelectedListItem(listItem);
				worldListWidget->RemoveListItem(listItem);

				LandscapingGroup *group = static_cast<GroupWidget *>(groupListWidget->GetFirstSelectedListItem())->landscapingGroup;
				group->RemoveWorldName(static_cast<TextWidget *>(listItem)->GetText());

				delete listItem;
				listItem = next;
			}

			removeWorldButton->Disable();
			SetModifiedFlag();
		}
		else if (widget == addModifierButton)
		{
			ModifierWindow *window = new ModifierWindow(this);
			window->SetCompletionProc(&NewModifierComplete, this);
			AddSubwindow(window);
		}
		else if (widget == removeModifierButton)
		{
			ModifierWidget *modifierWidget = static_cast<ModifierWidget *>(modifierListWidget->GetFirstSelectedListItem());
			modifierListWidget->RemoveListItem(modifierWidget);
			delete modifierWidget->instanceModifier;
			delete modifierWidget;

			modifierConfigurationWidget->ReleaseConfiguration();
			removeModifierButton->Disable();
			SetModifiedFlag();
		}
	}
	else if (eventType == kEventWidgetChange)
	{
		if (widget == multipaneWidget)
		{
			SelectPane(static_cast<MultipaneWidget *>(widget)->GetSelection());
		}
		else if (widget == groupListWidget)
		{
			SelectGroup(static_cast<GroupWidget *>(groupListWidget->GetFirstSelectedListItem())->landscapingGroup);
		}
		else if (widget == worldListWidget)
		{
			if (worldListWidget->GetFirstSelectedListItem())
			{
				removeWorldButton->Enable();
			}
			else
			{
				removeWorldButton->Disable();
			}
		}
		else if (widget == modifierListWidget)
		{
			SelectModifier(static_cast<ModifierWidget *>(modifierListWidget->GetFirstSelectedListItem())->instanceModifier);
		}
	}
}

void LandscapingEditor::Close(void)
{
	if (editorState & kLandscapingEditorModified)
	{
		const StringTable *table = TheWorldEditor->GetStringTable();

		const char *title = table->GetString(StringID('LAND', 'SAVE'));
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

		TextWidget *text = new TextWidget(Vector2D(242.0F, 0.0F), table->GetString(StringID('LAND', 'CFRM')), "font/Gui");
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
