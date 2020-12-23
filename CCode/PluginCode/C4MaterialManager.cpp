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


#include "C4MaterialManager.h"
#include "C4EditorSupport.h"
#include "C4WorldEditor.h"
#include "C4Graphics.h"


using namespace C4;


MaterialWidget::MaterialWidget(const Vector2D& size, MaterialContainer *container) :
		Widget(kWidgetMaterial, size),
		borderWidget(size),
		previewWidget(size, container->GetMaterialObject()),
		textWidget(Vector2D(size.x + 14.0F, 12.0F), container->GetMaterialName(), "font/Normal")
{
	materialContainer = container;

	SetWidgetUsage(kWidgetTrackInhibit);
}

MaterialWidget::~MaterialWidget()
{
}

bool MaterialWidget::CalculateBoundingBox(Box2D *box) const
{
	box->min.Set(-8.0F, -8.0F);
	box->max.Set(GetWidgetSize().x + 8.0F, GetWidgetSize().y + 16.0F);
	return (true);
}

void MaterialWidget::Preprocess(void)
{
	Widget::Preprocess();

	borderWidget.Disable();
	AppendNewSubnode(&borderWidget);

	textWidget.SetWidgetPosition(Point3D(-7.0F, GetWidgetSize().y + 2.0F, 0.0F));
	textWidget.SetTextAlignment(kTextAlignCenter);
	textWidget.Disable();
	AppendNewSubnode(&textWidget);

	previewWidget.Disable();
	AppendNewSubnode(&previewWidget);
	previewWidget.SetMaterial(materialContainer->GetMaterialObject());
	previewWidget.SetPreviewGeometry(materialContainer->GetPreviewType());
}

void MaterialWidget::HandleMouseEvent(const PanelMouseEventData *eventData)
{
	if (eventData->eventType == kEventMouseDown)
	{
		if (eventData->eventFlags & kMouseDoubleClick)
		{
			PostWidgetEvent(WidgetEventData(kEventWidgetActivate));
		}
	}
}

void MaterialWidget::SetMaterial(MaterialContainer *container)
{
	materialContainer = container;
	previewWidget.SetMaterial(container->GetMaterialObject());
	previewWidget.SetPreviewGeometry(container->GetPreviewType());
	textWidget.SetText(container->GetMaterialName());
}

void MaterialWidget::SetPreviewGeometry(PrimitiveType type)
{
	previewWidget.SetPreviewGeometry(type);
}

void MaterialWidget::SetMaterialName(const char *name)
{
	textWidget.SetText(name);
}

void MaterialWidget::UpdatePreview(void)
{
	previewWidget.UpdatePreview();
}


MaterialExporter::MaterialExporter(MaterialContainer *container, const char *title) : FilePicker('MATL', title, TheResourceMgr->GetGenericCatalog(), MaterialResource::GetDescriptor(), nullptr, kFilePickerSave)
{
	materialContainer = container;
}

MaterialExporter::~MaterialExporter()
{
}

void MaterialExporter::Preprocess(void)
{
	FilePicker::Preprocess();
	SetFileName(materialContainer->GetMaterialName());
}


MaterialWindow::MaterialWindow(Editor *editor) : 
		Window("WorldEditor/MaterialManager"),
		materialTableObserver(this, &MaterialWindow::HandleMaterialTableEvent),
		configurationObserver(this, &MaterialWindow::HandleConfigurationEvent) 
{
	worldEditor = editor; 

	materialList = editor->GetEditorObject()->UpdateMaterialList(editor->GetRootNode());
	selectedMaterial = nullptr; 
}
 
MaterialWindow::~MaterialWindow() 
{
}

void MaterialWindow::Preprocess(void) 
{
	static const char *const paneIdentifier[kMaterialPaneCount] =
	{
		"Diffuse", "Specular", "Ambient", "Flags", "Texcoords"
	};

	Window::Preprocess();

	closeButton = static_cast<PushButtonWidget *>(FindWidget("Close"));
	newButton = static_cast<PushButtonWidget *>(FindWidget("New"));
	deleteButton = static_cast<PushButtonWidget *>(FindWidget("Delete"));
	duplicateButton = static_cast<PushButtonWidget *>(FindWidget("Duplicate"));
	importButton = static_cast<PushButtonWidget *>(FindWidget("Import"));
	exportButton = static_cast<PushButtonWidget *>(FindWidget("Export"));
	cleanupButton = static_cast<PushButtonWidget *>(FindWidget("Cleanup"));
	shaderButton = static_cast<PushButtonWidget *>(FindWidget("Shader"));

	tableWidget = static_cast<TableWidget *>(FindWidget("Table"));
	tableWidget->SetObserver(&materialTableObserver);
	SetFocusWidget(tableWidget);

	multipaneWidget = static_cast<MultipaneWidget *>(FindWidget("Pane"));

	for (machine a = 0; a < kMaterialPaneCount; a++)
	{
		configurationWidget[a] = static_cast<ConfigurationWidget *>(FindWidget(paneIdentifier[a]));
	}

	const Widget *widget = FindWidget("Preview");
	previewWidget = new PreviewWidget(widget->GetWidgetSize(), worldEditor->GetSelectedMaterial()->GetMaterialObject());
	previewWidget->SetWidgetPosition(widget->GetWidgetPosition());
	AppendNewSubnode(previewWidget);

	previewMenuWidget = static_cast<PopupMenuWidget *>(FindWidget("Menu"));
	materialNameWidget = static_cast<EditTextWidget *>(FindWidget("Name"));
	referenceCountWidget = static_cast<TextWidget *>(FindWidget("Ref"));

	UpdateMaterialList(worldEditor->GetSelectedMaterial());
}

void MaterialWindow::HandleConfigurationEvent(SettingInterface *settingInterface)
{
	CommitMaterialSettings(selectedMaterial->GetMaterialContainer());
	HandleMaterialModification();
}

void MaterialWindow::CommitMaterialSettings(MaterialContainer *container)
{
	MaterialObject *object = container->GetMaterialObject();
	if (!object->ShaderMaterial())
	{
		object->PurgeAttributes();

		configurationWidget[kMaterialPaneDiffuse]->CommitCategoryConfiguration(container, 'DIFF');
		configurationWidget[kMaterialPaneSpecular]->CommitCategoryConfiguration(container, 'SPEC');
		configurationWidget[kMaterialPaneAmbient]->CommitCategoryConfiguration(container, 'AMBT');
	}

	configurationWidget[kMaterialPaneFlags]->CommitCategoryConfiguration(container, 'FLAG');
	configurationWidget[kMaterialPaneTexcoords]->CommitCategoryConfiguration(container, 'TEXC');
}

void MaterialWindow::SelectMaterial(MaterialWidget *materialWidget)
{
	selectedMaterial = materialWidget;

	MaterialContainer *container = materialWidget->GetMaterialContainer();
	previewWidget->SetMaterial(container->GetMaterialObject());
	previewWidget->SetPreviewGeometry(container->GetPreviewType());
	UpdateMaterialConfiguration(container);

	if (container->GetUsageCount() != 0)
	{
		deleteButton->Disable();
	}
	else
	{
		deleteButton->Enable();
	}
}

void MaterialWindow::UpdatePaneVisibility(void)
{
	int32 selection = multipaneWidget->GetSelection();
	for (machine a = 0; a < kMaterialPaneCount; a++)
	{
		if (a != selection)
		{
			configurationWidget[a]->Hide();
		}
		else
		{
			configurationWidget[a]->Show();
		}
	}

	Widget *widget = GetFocusWidget();
	if ((widget) && (widget->GetGlobalWidgetState() & kWidgetHidden))
	{
		SetFocusWidget(nullptr);
	}
}

void MaterialWindow::UpdateMaterialConfiguration(const MaterialContainer *container)
{
	if (!container->GetMaterialObject()->ShaderMaterial())
	{
		configurationWidget[kMaterialPaneDiffuse]->ReleaseConfiguration();
		configurationWidget[kMaterialPaneSpecular]->ReleaseConfiguration();
		configurationWidget[kMaterialPaneAmbient]->ReleaseConfiguration();

		configurationWidget[kMaterialPaneDiffuse]->BuildCategoryConfiguration(container, 'DIFF');
		configurationWidget[kMaterialPaneSpecular]->BuildCategoryConfiguration(container, 'SPEC');
		configurationWidget[kMaterialPaneAmbient]->BuildCategoryConfiguration(container, 'AMBT');

		configurationWidget[kMaterialPaneDiffuse]->SetObserver(&configurationObserver);
		configurationWidget[kMaterialPaneSpecular]->SetObserver(&configurationObserver);
		configurationWidget[kMaterialPaneAmbient]->SetObserver(&configurationObserver);

		multipaneWidget->ShowPane(kMaterialPaneDiffuse);
		multipaneWidget->ShowPane(kMaterialPaneSpecular);
		multipaneWidget->ShowPane(kMaterialPaneAmbient);
	}
	else
	{
		multipaneWidget->HidePane(kMaterialPaneDiffuse);
		multipaneWidget->HidePane(kMaterialPaneSpecular);
		multipaneWidget->HidePane(kMaterialPaneAmbient);

		if (multipaneWidget->GetSelection() == kWidgetValueNone)
		{
			multipaneWidget->SetSelection(3);
		}
	}

	configurationWidget[kMaterialPaneFlags]->ReleaseConfiguration();
	configurationWidget[kMaterialPaneTexcoords]->ReleaseConfiguration();

	configurationWidget[kMaterialPaneFlags]->BuildCategoryConfiguration(container, 'FLAG');
	configurationWidget[kMaterialPaneTexcoords]->BuildCategoryConfiguration(container, 'TEXC');

	configurationWidget[kMaterialPaneFlags]->SetObserver(&configurationObserver);
	configurationWidget[kMaterialPaneTexcoords]->SetObserver(&configurationObserver);

	UpdatePaneVisibility();
	UpdateMaterialContainerInfo(container);
}

void MaterialWindow::UpdateMaterialContainerInfo(const MaterialContainer *container)
{
	int32 selection = 0;
	switch (container->GetPreviewType())
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

	previewMenuWidget->SetSelection(selection);

	materialNameWidget->SetText(container->GetMaterialName());
	referenceCountWidget->SetText(String<7>(container->GetUsageCount()));

	exportButton->Enable();
}

void MaterialWindow::UpdateMaterialList(MaterialContainer *currentMaterial)
{
	if ((!currentMaterial) && (selectedMaterial))
	{
		currentMaterial = selectedMaterial->GetMaterialContainer();
	}

	tableWidget->PurgeTableItems();
	selectedMaterial = nullptr;

	int32 index = 0;
	int32 selectIndex = -1;

	MaterialContainer *materialContainer = materialList->First();
	while (materialContainer)
	{
		MaterialWidget *materialWidget = new MaterialWidget(Vector2D(64.0F, 64.0F), materialContainer);
		tableWidget->AppendTableItem(materialWidget);

		if (materialContainer == currentMaterial)
		{
			SelectMaterial(materialWidget);
			selectIndex = index;
		}

		index++;
		materialContainer = materialContainer->Next();
	}

	tableWidget->Update();

	if (selectIndex >= 0)
	{
		tableWidget->SelectTableItem(selectIndex);
		tableWidget->RevealTableItem(selectIndex);
	}
}

void MaterialWindow::HandleMaterialTableEvent(Widget *widget, const WidgetEventData *eventData)
{
	EventType eventType = eventData->eventType;

	if (eventType == kEventWidgetActivate)
	{
		Close();
	}
	else if (eventType == kEventWidgetChange)
	{
		Widget *selection = static_cast<TableWidget *>(widget)->GetFirstSelectedTableItem();
		if (selection)
		{
			MaterialWidget *materialWidget = static_cast<MaterialWidget *>(selection);
			if (materialWidget != selectedMaterial)
			{
				SelectMaterial(materialWidget);
			}
		}
	}
}

void MaterialWindow::ImportMaterial(FilePicker *picker, void *cookie)
{
	ResourceName name = picker->GetResourceName();
	MaterialResource *resource = MaterialResource::Get(name);
	if (resource)
	{
		Unpacker unpacker(resource->GetMaterialData(), resource->GetVersion());

		MaterialObject *object = new MaterialObject;
		object->Unpack(unpacker, kUnpackEditor);
		resource->Release();

		MaterialContainer *container = new MaterialContainer(object, &name[Text::GetPrefixDirectoryLength(name)]);
		object->Release();

		MaterialWindow *window = static_cast<MaterialWindow *>(cookie);
		window->materialList->Append(container);
		window->UpdateMaterialList(container);
	}
}

void MaterialWindow::ExportMaterial(FilePicker *picker, void *cookie)
{
	const MaterialContainer *container = static_cast<MaterialExporter *>(picker)->GetMaterialContainer();
	MaterialObject *object = container->GetMaterialObject();

	if (object)
	{
		File			file;
		ResourcePath	path;

		object->SetObjectIndex(0);

		ResourceName name(picker->GetFileName());
		name[Text::GetResourceNameLength(name)] = 0;

		TheResourceMgr->GetGenericCatalog()->GetResourcePath(MaterialResource::GetDescriptor(), name, &path);
		TheResourceMgr->CreateDirectoryPath(path);

		if (file.Open(path, kFileCreate) == kFileOkay)
		{
			int32 header[3] = {1, kEngineInternalVersion, 0};
			file.Write(header, 12);

			Buffer buffer(kPackageDefaultSize);
			Package package(buffer, kPackageDefaultSize);

			Packer packer(&package);
			object->Pack(packer, kPackEditor);

			file.Write(package.GetStorage(), package.GetSize());
		}
	}
}

void MaterialWindow::ShaderEditorComplete(ShaderEditor *shaderEditor, void *cookie)
{
	MaterialWindow *window = static_cast<MaterialWindow *>(cookie);
	MaterialContainer *container = window->selectedMaterial->GetMaterialContainer();
	MaterialObject *materialObject = container->GetMaterialObject();

	if (materialObject->FindAttribute(kAttributeShader))
	{
		Attribute *attribute = materialObject->GetFirstAttribute();
		while (attribute)
		{
			Attribute *next = attribute->Next();

			if (attribute->GetAttributeType() != kAttributeShader)
			{
				delete attribute;
			}

			attribute = next;
		}

		window->UpdateMaterialConfiguration(container);
	}
}

void MaterialWindow::HandleMaterialModification(void)
{
	selectedMaterial->UpdatePreview();
	previewWidget->UpdatePreview();

	MaterialContainer *container = selectedMaterial->GetMaterialContainer();
	worldEditor->PostEvent(MaterialEditorEvent(kEditorEventMaterialModified, container->GetMaterialObject()));
}

bool MaterialWindow::HandleKeyboardEvent(const KeyboardEventData *eventData)
{
	EventType eventType = eventData->eventType;

	if (eventType == kEventKeyDown)
	{
		if (eventData->keyCode == kKeyCodeEnter)
		{
			Close();
			return (true);
		}
	}
	else if (eventType == kEventKeyCommand)
	{
		if (eventData->keyCode == 'W')
		{
			Close();
			return (true);
		}
	}

	return (Window::HandleKeyboardEvent(eventData));
}

void MaterialWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	static const PrimitiveType primitiveType[5] =
	{
		kPrimitivePlate, kPrimitiveBox, kPrimitiveSphere, kPrimitiveCylinder, kPrimitiveTorus
	};

	EventType eventType = eventData->eventType;

	if (eventType == kEventWidgetActivate)
	{
		if (widget == closeButton)
		{
			Close();
		}
		else if (widget == newButton)
		{
			MaterialObject *object = new MaterialObject;
			MaterialContainer *container = new MaterialContainer(object);
			object->Release();

			materialList->Append(container);
			UpdateMaterialList(container);
		}
		else if (widget == deleteButton)
		{
			MaterialWidget *materialWidget = selectedMaterial;
			if (materialWidget)
			{
				delete materialWidget->GetMaterialContainer();

				MaterialWidget *newSelection = static_cast<MaterialWidget *>(materialWidget->Next());
				if (!newSelection)
				{
					newSelection = static_cast<MaterialWidget *>(materialWidget->Previous());
				}

				delete materialWidget;

				if (newSelection)
				{
					UpdateMaterialList(newSelection->GetMaterialContainer());
				}
				else
				{
					newButton->Activate();
				}
			}
		}
		else if (widget == duplicateButton)
		{
			MaterialWidget *materialWidget = selectedMaterial;
			if (materialWidget)
			{
				MaterialContainer *container = new MaterialContainer(*materialWidget->GetMaterialContainer());
				materialList->Append(container);
				UpdateMaterialList(container);
				UpdateMaterialConfiguration(container);
			}
		}
		else if (widget == importButton)
		{
			const char *title = TheWorldEditor->GetStringTable()->GetString(StringID('MATL', 'IMPT'));
			FilePicker *picker = new FilePicker('MATL', title, TheResourceMgr->GetGenericCatalog(), MaterialResource::GetDescriptor());
			picker->SetCompletionProc(&ImportMaterial, this);
			AddSubwindow(picker);
		}
		else if (widget == exportButton)
		{
			if (selectedMaterial)
			{
				const char *title = TheWorldEditor->GetStringTable()->GetString(StringID('MATL', 'EXPT'));
				FilePicker *picker = new MaterialExporter(selectedMaterial->GetMaterialContainer(), title);
				picker->SetCompletionProc(&ExportMaterial, this);
				AddSubwindow(picker);
			}
		}
		else if (widget == cleanupButton)
		{
			EditorObject *editorObject = worldEditor->GetEditorObject();
			if (selectedMaterial)
			{
				editorObject->SelectMaterial(selectedMaterial->GetMaterialContainer());
			}

			editorObject->CleanupMaterials(worldEditor->GetRootNode());
			UpdateMaterialList();
		}
		else if (widget == shaderButton)
		{
			MaterialObject *materialObject = selectedMaterial->GetMaterialContainer()->GetMaterialObject();
			ShaderEditor *shaderEditor = new ShaderEditor(this, materialObject, primitiveType[previewMenuWidget->GetSelection()]);
			shaderEditor->SetCompletionProc(&ShaderEditorComplete, this);
			AddSubwindow(shaderEditor);
		}
	}
	else if (eventType == kEventWidgetChange)
	{
		if (widget == multipaneWidget)
		{
			UpdatePaneVisibility();
		}
		else if (widget == previewMenuWidget)
		{
			if (selectedMaterial)
			{
				PrimitiveType type = primitiveType[previewMenuWidget->GetSelection()];
				selectedMaterial->SetPreviewGeometry(type);
				previewWidget->SetPreviewGeometry(type);

				MaterialContainer *container = selectedMaterial->GetMaterialContainer();
				container->SetPreviewType(type);

				worldEditor->PostEvent(MaterialEditorEvent(kEditorEventMaterialModified, container->GetMaterialObject()));
			}
		}
		else if (widget == materialNameWidget)
		{
			if (selectedMaterial)
			{
				const char *name = materialNameWidget->GetText();
				selectedMaterial->SetMaterialName(name);

				MaterialContainer *container = selectedMaterial->GetMaterialContainer();
				container->SetMaterialName(name);

				worldEditor->PostEvent(MaterialEditorEvent(kEditorEventMaterialModified, container->GetMaterialObject()));
			}
		}
	}
}

void MaterialWindow::Close(void)
{
	const MaterialWidget *materialWidget = selectedMaterial;
	if (materialWidget)
	{
		worldEditor->SelectMaterial(materialWidget->GetMaterialContainer());
	}

	Window::Close();
}

// ZYUQURM
