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


#include "C4NodeInfo.h"
#include "C4WorldEditor.h"
#include "C4EditorSupport.h"
#include "C4ScriptEditor.h"
#include "C4PanelEditor.h"
#include "C4Deformable.h"
#include "C4Fields.h"
#include "C4World.h"


using namespace C4;


NodeInfoPane::NodeInfoPane(NodeInfoType type, const char *title, const char *panelName)
{
	nodeInfoType = type;
	paneTitle = title;

	visitedFlag = false;
	initialFocusWidget = nullptr;

	Load(panelName);
}

NodeInfoPane::~NodeInfoPane()
{
}

void NodeInfoPane::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
}


CategoryInfoPane::CategoryInfoPane(NodeInfoType type, Type category, const char *title, const char *panelName) : NodeInfoPane(type, title, panelName)
{
	nodeInfoType = category;
}

CategoryInfoPane::~CategoryInfoPane()
{
}

void CategoryInfoPane::Preprocess(void)
{
	NodeInfoPane::Preprocess();

	configurationWidget = static_cast<ConfigurationWidget *>(FindWidget("Config"));
}

void CategoryInfoPane::BuildSettings(const Configurable *configurable)
{
	configurationWidget->BuildCategoryConfiguration(configurable, nodeInfoType);
}

void CategoryInfoPane::CommitSettings(Configurable *configurable)
{
	configurationWidget->CommitCategoryConfiguration(configurable, nodeInfoType);
}


FireEffectInfoPane::FireEffectInfoPane(Type category, const char *title) :
		CategoryInfoPane(kNodeInfoObjectCategory, category, title, "WorldEditor/FireInfo"),
		configurationObserver(this, &FireEffectInfoPane::HandleConfigurationEvent)
{
	previewWorld = nullptr;
}

FireEffectInfoPane::~FireEffectInfoPane()
{
	delete previewWorld;
}

void FireEffectInfoPane::Preprocess(void)
{
	CategoryInfoPane::Preprocess();

	previewViewport = static_cast<FrustumViewportWidget *>(FindWidget("Viewport"));
	previewViewport->SetRenderProc(&RenderPreview, this);

	FrustumCamera *camera = previewViewport->GetViewportCamera();
	camera->SetNodeTransform(Transform4D(0.0F, 0.0F, 1.0F, -1.0F, -1.0F, 0.0F, 0.0F, 0.0F, 0.0F, -1.0F, 0.0F, 0.5F));
	camera->GetObject()->SetClearColor(K::transparent);

	Zone *previewZone = new InfiniteZone;
	ZoneObject *zoneObject = new InfiniteZoneObject;
	previewZone->SetObject(zoneObject);
	zoneObject->Release();

	previewWorld = new World(previewZone, kWorldViewport | kWorldClearColor | kWorldMotionBlurInhibit | kWorldListenerInhibit);
	previewWorld->Preprocess();
	previewWorld->SetCamera(camera);
	previewWorld->SetRenderSize(256, 256);

	previewFire = nullptr;
}

void FireEffectInfoPane::HandleConfigurationEvent(SettingInterface *settingInterface)
{
	FireEffectObject *object = previewFire->GetObject();
	GetConfigurationWidget()->CommitCategoryConfiguration(object, kEffectFire);
 
	previewFire->Preprocess();
	previewFire->InvalidateShaderData();
} 

void FireEffectInfoPane::RenderPreview(List<Renderable> *renderList, ViewportWidget *viewport, void *cookie) 
{
	FireEffectInfoPane *pane = static_cast<FireEffectInfoPane *>(cookie);
 
	pane->previewWorld->Update();
	pane->previewWorld->BeginRendering(); 
	pane->previewWorld->Render(); 
	pane->previewWorld->EndRendering();
}

void FireEffectInfoPane::BuildSettings(const Configurable *configurable) 
{
	CategoryInfoPane::BuildSettings(configurable);

	if (!previewFire)
	{
		const FireEffectObject *object = static_cast<const FireEffectObject *>(configurable);

		float radius = object->GetFireRadius() / object->GetFireHeight();
		previewFire = new FireEffect(radius, 1.0F, object->GetFireIntensity(), object->GetFireSpeed(), object->GetFireTextureName());
		previewWorld->AddNewNode(previewFire);

		GetConfigurationWidget()->SetObserver(&configurationObserver);
	}
}


PropertyData::PropertyData(const PropertyRegistration *registration, const Property *property)
{
	propertyRegistration = registration;

	originalProperty = property;
	newProperty = nullptr;
}

PropertyData::~PropertyData()
{
	if ((newProperty) && (!newProperty->GetOwningMap()))
	{
		delete newProperty;
	}
}


PropertyInfoPane::PropertyInfoPane() : NodeInfoPane(kNodeInfoProperties, TheWorldEditor->GetStringTable()->GetString(StringID('INFO', 'PANE', 'PROP')), "WorldEditor/PropertyInfo")
{
	selectedProperty = nullptr;
}

PropertyInfoPane::~PropertyInfoPane()
{
}

void PropertyInfoPane::Preprocess(void)
{
	NodeInfoPane::Preprocess();

	availableListWidget = static_cast<ListWidget *>(FindWidget("Available"));
	assignedListWidget = static_cast<ListWidget *>(FindWidget("Assigned"));
	assignButton = static_cast<PushButtonWidget *>(FindWidget("Assign"));
	removeButton = static_cast<PushButtonWidget *>(FindWidget("Remove"));
	configurationWidget = static_cast<ConfigurationWidget *>(FindWidget("Config"));

	SetInitialFocusWidget(availableListWidget);
}

void PropertyInfoPane::SelectAvailableProperty(int32 index)
{
	const PropertyRegistration *registration = validPropertyArray[index];
	PropertyType type = registration->GetPropertyType();

	const PropertyData *data = propertyDataList.First();
	while (data)
	{
		if (data->GetPropertyRegistration()->GetPropertyType() == type)
		{
			assignButton->Disable();
			return;
		}

		data = data->Next();
	}

	assignButton->Enable();
}

void PropertyInfoPane::SelectAssignedProperty(int32 index)
{
	if (selectedProperty)
	{
		configurationWidget->CommitConfiguration(selectedProperty->GetNewProperty());
		configurationWidget->ReleaseConfiguration();
		selectedProperty = nullptr;
	}

	if (index >= 0)
	{
		PropertyData *data = propertyDataList[index];
		selectedProperty = data;

		Property *newProperty = data->GetNewProperty();
		if (newProperty)
		{
			configurationWidget->BuildConfiguration(newProperty);
		}
		else
		{
			newProperty = Property::New(data->GetPropertyRegistration()->GetPropertyType());
			data->SetNewProperty(newProperty);

			const Property *property = data->GetOriginalProperty();
			if (!property)
			{
				property = newProperty;
			}

			configurationWidget->BuildConfiguration(property);
		}

		removeButton->Enable();
	}
}

void PropertyInfoPane::BuildSettings(const Configurable *configurable)
{
	const Node *node = static_cast<const Node *>(configurable);

	const PropertyRegistration *registration = Property::GetFirstRegistration();
	while (registration)
	{
		if (registration->ValidNode(node))
		{
			validPropertyArray.AddElement(registration);
			availableListWidget->AppendListItem(registration->GetPropertyName());
		}

		registration = registration->Next();
	}

	const Property *property = node->GetFirstProperty();
	const PropertyObject *propertyObject = node->GetPropertyObject();
	for (;;)
	{
		while (property)
		{
			registration = Property::FindRegistration(property->GetPropertyType());
			if (registration)
			{
				PropertyData *data = new PropertyData(registration, property);
				propertyDataList.Append(data);

				assignedListWidget->AppendListItem(registration->GetPropertyName());
			}

			property = property->Next();
		}

		if (!propertyObject)
		{
			break;
		}

		property = propertyObject->GetFirstProperty();
		propertyObject = nullptr;
	}
}

void PropertyInfoPane::CommitSettings(Configurable *configurable)
{
	SelectAssignedProperty(-1);

	Node *node = static_cast<Node *>(configurable);

	Property *property = node->GetFirstProperty();
	PropertyObject *propertyObject = node->GetPropertyObject();
	for (;;)
	{
		while (property)
		{
			Property *next = property->Next();
			PropertyType type = property->GetPropertyType();

			if (Property::FindRegistration(type))
			{
				const PropertyData *data = propertyDataList.First();
				while (data)
				{
					if (data->GetPropertyRegistration()->GetPropertyType() == type)
					{
						break;
					}

					data = data->Next();
				}

				if (!data)
				{
					delete property;
				}
			}

			property = next;
		}

		if (!propertyObject)
		{
			break;
		}

		property = propertyObject->GetFirstProperty();
		propertyObject = nullptr;
	}

	propertyObject = node->GetPropertyObject();

	PropertyData *data = propertyDataList.First();
	while (data)
	{
		Property *newProperty = data->GetNewProperty();
		if (newProperty)
		{
			delete data->GetOriginalProperty();

			if (newProperty->SharedProperty())
			{
				if (!propertyObject)
				{
					propertyObject = new PropertyObject;
					node->SetPropertyObject(propertyObject);
					propertyObject->Release();
				}

				propertyObject->AddProperty(newProperty);
			}
			else
			{
				node->AddProperty(newProperty);
			}
		}

		data = data->Next();
	}

	if ((propertyObject) && (!propertyObject->GetFirstProperty()))
	{
		if (propertyObject->GetReferenceCount() == 1)
		{
			node->SetPropertyObject(nullptr);
		}
		else
		{
			Node *root = node->GetRootNode();
			Node *subnode = root->GetFirstSubnode();
			while (subnode)
			{
				PropertyObject *object = subnode->GetPropertyObject();
				if (object == propertyObject)
				{
					subnode->SetPropertyObject(nullptr);
				}

				subnode = root->GetNextNode(subnode);
			}
		}
	}
}

void PropertyInfoPane::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	EventType eventType = eventData->eventType;

	if (eventType == kEventWidgetActivate)
	{
		if ((widget == assignButton) || (widget == availableListWidget))
		{
			const PropertyRegistration *registration = validPropertyArray[availableListWidget->GetFirstSelectedIndex()];
			if (registration)
			{
				PropertyData *data = new PropertyData(registration, nullptr);
				propertyDataList.Append(data);

				assignedListWidget->AppendListItem(registration->GetPropertyName());

				int32 index = assignedListWidget->GetListItemCount() - 1;
				assignedListWidget->SelectListItem(index);
				SelectAssignedProperty(index);

				assignButton->Disable();
			}
		}
		else if (widget == removeButton)
		{
			delete propertyDataList[assignedListWidget->GetFirstSelectedIndex()];

			Widget *listItem = assignedListWidget->GetFirstSelectedListItem();
			assignedListWidget->RemoveListItem(listItem);
			delete listItem;

			configurationWidget->ReleaseConfiguration();
			selectedProperty = nullptr;

			removeButton->Disable();

			int32 selection = availableListWidget->GetFirstSelectedIndex();
			if (selection >= 0)
			{
				SelectAvailableProperty(selection);
			}
		}
	}
	else if (eventType == kEventWidgetChange)
	{
		if (widget == availableListWidget)
		{
			SelectAvailableProperty(availableListWidget->GetFirstSelectedIndex());
		}
		else if (widget == assignedListWidget)
		{
			SelectAssignedProperty(assignedListWidget->GetFirstSelectedIndex());
		}
	}
}


ModifierInfoPane::ModifierInfoPane() : NodeInfoPane(kNodeInfoModifiers, TheWorldEditor->GetStringTable()->GetString(StringID('INFO', 'PANE', 'MDFR')), "WorldEditor/ModifierInfo")
{
	selectedModifier = nullptr;
}

ModifierInfoPane::~ModifierInfoPane()
{
}

void ModifierInfoPane::Preprocess(void)
{
	NodeInfoPane::Preprocess();

	availableListWidget = static_cast<ListWidget *>(FindWidget("Available"));
	assignedListWidget = static_cast<ListWidget *>(FindWidget("Assigned"));
	assignButton = static_cast<PushButtonWidget *>(FindWidget("Assign"));
	removeButton = static_cast<PushButtonWidget *>(FindWidget("Remove"));
	configurationWidget = static_cast<ConfigurationWidget *>(FindWidget("Config"));

	SetInitialFocusWidget(availableListWidget);
}

void ModifierInfoPane::SelectAssignedModifier(int32 index)
{
	if (selectedModifier)
	{
		configurationWidget->CommitConfiguration(selectedModifier);
		configurationWidget->ReleaseConfiguration();
		selectedModifier = nullptr;
	}

	if (index >= 0)
	{
		Modifier *modifier = modifierList[index];
		if (modifier)
		{
			selectedModifier = modifier;
			configurationWidget->BuildConfiguration(modifier);
		}

		removeButton->Enable();
	}
}

void ModifierInfoPane::BuildSettings(const Configurable *configurable)
{
	const Instance *instance = static_cast<const Instance *>(configurable);

	const ModifierRegistration *registration = Modifier::GetFirstRegistration();
	while (registration)
	{
		if (registration->ValidInstance(instance))
		{
			validModifierArray.AddElement(registration);
			availableListWidget->AppendListItem(registration->GetModifierName());
		}

		registration = registration->Next();
	}

	const StringTable *table = TheWorldEditor->GetStringTable();

	const Modifier *modifier = instance->GetFirstModifier();
	while (modifier)
	{
		Modifier *clone = modifier->Clone();
		modifierList.Append(clone);

		registration = Modifier::FindRegistration(clone->GetModifierType());
		if (registration)
		{
			const char *name = (registration) ? registration->GetModifierName() : table->GetString(StringID('INFO', 'UKWN'));
			assignedListWidget->AppendListItem(name);
		}

		modifier = modifier->Next();
	}
}

void ModifierInfoPane::CommitSettings(Configurable *configurable)
{
	SelectAssignedModifier(-1);

	Instance *instance = static_cast<Instance *>(configurable);
	instance->PurgeModifiers();

	for (;;)
	{
		Modifier *modifier = modifierList.First();
		if (!modifier)
		{
			break;
		}

		instance->AddModifier(modifier);
	}
}

void ModifierInfoPane::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	EventType eventType = eventData->eventType;

	if (eventType == kEventWidgetActivate)
	{
		if ((widget == assignButton) || (widget == availableListWidget))
		{
			const ModifierRegistration *registration = validModifierArray[availableListWidget->GetFirstSelectedIndex()];
			if (registration)
			{
				Modifier *modifier = Modifier::New(registration->GetModifierType());
				modifierList.Append(modifier);

				assignedListWidget->AppendListItem(registration->GetModifierName());

				int32 index = assignedListWidget->GetListItemCount() - 1;
				assignedListWidget->SelectListItem(index);
				SelectAssignedModifier(index);
			}
		}
		else if (widget == removeButton)
		{
			delete modifierList[assignedListWidget->GetFirstSelectedIndex()];

			Widget *listItem = assignedListWidget->GetFirstSelectedListItem();
			assignedListWidget->RemoveListItem(listItem);
			delete listItem;

			configurationWidget->ReleaseConfiguration();
			selectedModifier = nullptr;

			removeButton->Disable();
		}
	}
	else if (eventType == kEventWidgetChange)
	{
		if (widget == availableListWidget)
		{
			if (availableListWidget->GetFirstSelectedListItem())
			{
				assignButton->Enable();
			}
		}
		else if (widget == assignedListWidget)
		{
			SelectAssignedModifier(assignedListWidget->GetFirstSelectedIndex());
		}
	}
}


ConnectorInfoPane::ConnectorInfoPane() : NodeInfoPane(kNodeInfoConnectors, TheWorldEditor->GetStringTable()->GetString(StringID('INFO', 'PANE', 'CONN')), "WorldEditor/ConnectorInfo")
{
}

ConnectorInfoPane::~ConnectorInfoPane()
{
}

ConnectorInfoPane::ConnectorData::ConnectorData(Widget *item, Connector *conn)
{
	listItem = item;
	connector = conn;

	checkWidget = new CheckWidget(Vector2D(16.0F, 16.0F));
	checkWidget->SetWidgetPosition(Point3D(4.0F, 5.0F, 0.0F));
	item->AppendSubnode(checkWidget);

	textWidget = new EditTextWidget(Vector2D(192.0F, 13.0F), kMaxConnectorKeyLength, "font/Gui");
	textWidget->SetEditTextFlags(kEditTextOverflow | kEditTextDoubleClickSelectAll);
	textWidget->SetFilterProc(&Connector::ConnectorKeyFilter);
	textWidget->SetWidgetPosition(Point3D(36.0F, 5.0F, 0.0F));
	item->AppendSubnode(textWidget);
}

ConnectorInfoPane::ConnectorData::~ConnectorData()
{
	delete listItem;
}

void ConnectorInfoPane::Preprocess(void)
{
	NodeInfoPane::Preprocess();

	availableListWidget = static_cast<ListWidget *>(FindWidget("Available"));
	assignedListWidget = static_cast<ListWidget *>(FindWidget("Assigned"));
	customListWidget = static_cast<ListWidget *>(FindWidget("Custom"));
	assignButton = static_cast<PushButtonWidget *>(FindWidget("Assign"));
	removeButton = static_cast<PushButtonWidget *>(FindWidget("Remove"));
	newButton = static_cast<PushButtonWidget *>(FindWidget("New"));
	deleteButton = static_cast<PushButtonWidget *>(FindWidget("Delete"));

	SetInitialFocusWidget(customListWidget);
}

bool ConnectorInfoPane::InternalConnectorAssigned(const char *key) const
{
	const Widget *listItem = assignedListWidget->GetFirstListItem();
	while (listItem)
	{
		const char *text = static_cast<const TextWidget *>(listItem)->GetText();
		if (Text::CompareText(text, key))
		{
			return (true);
		}

		listItem = listItem->Next();
	}

	return (false);
}

void ConnectorInfoPane::BuildSettings(const Configurable *configurable)
{
	const Node *node = static_cast<const Node *>(configurable);

	int32 count = node->GetInternalConnectorCount();
	for (machine a = 0; a < count; a++)
	{
		const char *key = node->GetInternalConnectorKey(a);
		if (key)
		{
			availableListWidget->AppendListItem(key);
		}
	}

	const Hub *hub = node->GetHub();
	if (hub)
	{
		Connector *connector = hub->GetFirstOutgoingEdge();
		while (connector)
		{
			const ConnectorKey& key = connector->GetConnectorKey();
			if (key[0] != '%')
			{
				Widget *listItem = new Widget;
				ConnectorData *data = new ConnectorData(listItem, connector);
				connectorList.Append(data);

				data->textWidget->SetText(key);

				customListWidget->AppendListItem(listItem);
				listItem->Enable();
			}
			else if (hub->FindOutgoingConnector(key))
			{
				assignedListWidget->AppendListItem(key);
			}

			connector = connector->GetNextOutgoingEdge();
		}
	}
}

void ConnectorInfoPane::CommitSettings(Configurable *configurable)
{
	Node *node = static_cast<Node *>(configurable);

	EditorManipulator *manipulator = Editor::GetManipulator(node);
	manipulator->UnselectConnector();

	const Hub *hub = node->GetHub();
	if (hub)
	{
		Connector *connector = hub->GetFirstOutgoingEdge();
		while (connector)
		{
			Connector *next = connector->GetNextOutgoingEdge();

			const char *key = connector->GetConnectorKey();
			if (key[0] != '%')
			{
				const ConnectorData *data = connectorList.First();
				while (data)
				{
					if (data->connector == connector)
					{
						break;
					}

					data = data->Next();
				}

				if (!data)
				{
					delete connector;
				}
			}
			else if (!InternalConnectorAssigned(key))
			{
				delete connector;
			}

			connector = next;
		}
	}

	const ConnectorData *data = connectorList.First();
	while (data)
	{
		const char *key = data->textWidget->GetText();
		if (key[0] != 0)
		{
			Connector *connector = data->connector;
			if (connector)
			{
				connector->SetConnectorKey(key);
			}
			else
			{
				node->AddConnector(key);
			}
		}

		data = data->Next();
	}

	const Widget *listItem = assignedListWidget->GetFirstListItem();
	while (listItem)
	{
		const char *key = static_cast<const TextWidget *>(listItem)->GetText();
		if ((!hub) || (!hub->FindOutgoingConnector(key)))
		{
			node->AddConnector(key);
		}

		listItem = listItem->Next();
	}

	manipulator->UpdateConnectors();
}

void ConnectorInfoPane::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	EventType eventType = eventData->eventType;

	if (eventType == kEventWidgetActivate)
	{
		if ((widget == assignButton) || (widget == availableListWidget))
		{
			const Widget *listItem = availableListWidget->GetFirstSelectedListItem();
			if (listItem)
			{
				const char *key = static_cast<const TextWidget *>(listItem)->GetText();
				if (!InternalConnectorAssigned(key))
				{
					assignedListWidget->AppendListItem(key);
					assignButton->Disable();
				}
			}
		}
		else if (widget == removeButton)
		{
			Widget *assignedWidget = assignedListWidget->GetFirstSelectedListItem();
			if (assignedWidget)
			{
				const char *key = static_cast<const TextWidget *>(assignedWidget)->GetText();

				const Widget *availableWidget = availableListWidget->GetFirstSelectedListItem();
				if ((availableWidget) && (Text::CompareText(key, static_cast<const TextWidget *>(availableWidget)->GetText())))
				{
					assignButton->Enable();
				}

				assignedListWidget->RemoveListItem(assignedWidget);
				delete assignedWidget;
			}

			removeButton->Disable();
		}
		else if (widget == newButton)
		{
			Widget *listItem = new Widget;
			ConnectorData *data = new ConnectorData(listItem);
			connectorList.Append(data);

			customListWidget->AppendListItem(listItem);
			listItem->Enable();

			customListWidget->RevealListItem(customListWidget->GetListItemCount() - 1);
			GetOwningWindow()->SetFocusWidget(data->textWidget);
		}
		else if (widget == deleteButton)
		{
			ConnectorData *data = connectorList.First();
			while (data)
			{
				ConnectorData *next = data->Next();

				if (data->checkWidget->GetValue() != 0)
				{
					customListWidget->RemoveListItem(data->listItem);
					delete data;
				}

				data = next;
			}
		}
	}
	else if (eventType == kEventWidgetChange)
	{
		if (widget == availableListWidget)
		{
			const Widget *listItem = availableListWidget->GetFirstSelectedListItem();
			if (listItem)
			{
				const char *key = static_cast<const TextWidget *>(listItem)->GetText();
				if (!InternalConnectorAssigned(key))
				{
					assignButton->Enable();
					return;
				}
			}

			assignButton->Disable();
		}
		else if (widget == assignedListWidget)
		{
			if (assignedListWidget->GetFirstSelectedListItem())
			{
				removeButton->Enable();
			}
			else
			{
				removeButton->Disable();
			}
		}
	}
}


ControllerInfoPane::ControllerInfoPane() : NodeInfoPane(kNodeInfoController, TheWorldEditor->GetStringTable()->GetString(StringID('INFO', 'PANE', 'CTRL')), "WorldEditor/ControllerInfo")
{
	targetNode = nullptr;
}

ControllerInfoPane::~ControllerInfoPane()
{
	if (targetNode)
	{
		Controller *controller = targetNode->GetController();

		int32 count = controllerCount;
		for (machine a = 0; a < count; a++)
		{
			if (controllerTable[a] != controller)
			{
				delete controllerTable[a];
			}
		}

		delete[] controllerTable;
	}
}

void ControllerInfoPane::Preprocess(void)
{
	NodeInfoPane::Preprocess();

	controllerListWidget = static_cast<ListWidget *>(FindWidget("List"));
	scriptButton = static_cast<PushButtonWidget *>(FindWidget("Script"));
	panelButton = static_cast<PushButtonWidget *>(FindWidget("Panel"));
	configurationWidget = static_cast<ConfigurationWidget *>(FindWidget("Config"));

	SetInitialFocusWidget(controllerListWidget);
	selectionIndex = -1;
}

void ControllerInfoPane::SelectController(int32 index)
{
	if (index != selectionIndex)
	{
		if (selectionIndex >= 0)
		{
			configurationWidget->CommitConfiguration(controllerTable[selectionIndex]);
			configurationWidget->ReleaseConfiguration();

			scriptButton->Hide();
			panelButton->Hide();
		}

		selectionIndex = index;
		if (index >= 0)
		{
			const ControllerRegistration *registration = validControllerArray[index];
			ControllerType type = registration->GetControllerType();

			Controller *controller = controllerTable[index];
			if (!controller)
			{
				Controller *targetController = targetNode->GetController();

				if ((targetController) && (targetController->GetControllerType() == type))
				{
					controller = targetController;
				}

				if (!controller)
				{
					controller = Controller::New(type);
				}

				controllerTable[index] = controller;
			}

			configurationWidget->BuildConfiguration(controller);

			type = controller->GetBaseControllerType();
			if (type == kControllerScript)
			{
				scriptButton->Show();
			}
			else if (type == kControllerPanel)
			{
				panelButton->Show();
			}
		}
	}
}

void ControllerInfoPane::BuildSettings(const Configurable *configurable)
{
	const Node *node = static_cast<const Node *>(configurable);
	targetNode = node;

	const Controller *controller = node->GetController();
	ControllerType controllerType = (controller) ? controller->GetControllerType() : 0xFFFFFFFF;
	bool locked = ((Editor::GetManipulator(node)->GetManipulatorFlags() & kManipulatorLockedController) != 0);

	int32 count = 0;
	const ControllerRegistration *registration = Controller::GetFirstRegistration();
	while (registration)
	{
		if (registration->ValidNode(node))
		{
			if ((!locked) || (registration->GetControllerType() == controllerType))
			{
				validControllerArray.AddElement(registration);
				count++;
			}
		}

		registration = registration->Next();
	}

	controllerCount = count;
	controllerTable = new Controller *[count];

	TextWidget *noneItem = controllerListWidget->AppendListItem(TheWorldEditor->GetStringTable()->GetString(StringID('INFO', 'NONE')));

	if (locked)
	{
		noneItem->SetWidgetState(kWidgetInactive);
		noneItem->SetWidgetAlpha(0.5F);
	}
	else
	{
		controllerListWidget->SelectListItem(0);
	}

	for (machine a = 0; a < count; a++)
	{
		controllerTable[a] = nullptr;

		registration = validControllerArray[a];
		controllerListWidget->AppendListItem(registration->GetControllerName());

		if (registration->GetControllerType() == controllerType)
		{
			controllerListWidget->SelectListItem(a + 1);
			SelectController(a);
		}
	}
}

void ControllerInfoPane::CommitSettings(Configurable *configurable)
{
	Node *node = static_cast<Node *>(configurable);
	Controller *originalController = node->GetController();

	int32 index = selectionIndex;
	if (index >= 0)
	{
		Controller *controller = controllerTable[index];
		configurationWidget->CommitConfiguration(controller);

		if (controller->GetBaseControllerType() == kControllerRigidBody)
		{
			Node *subnode = node;
			do
			{
				if (subnode->GetNodeType() == kNodeGeometry)
				{
					GeometryObject *object = static_cast<Geometry *>(subnode)->GetObject();
					object->SetCollisionExclusionMask(kCollisionExcludeAll);
				}

				subnode = node->GetNextNode(subnode);
			} while (subnode);
		}

		if (controller == originalController)
		{
			return;
		}

		node->SetController(controller);
	}
	else
	{
		node->SetController(nullptr);
	}

	if (originalController)
	{
		int32 count = controllerCount;
		for (machine a = 0; a < count; a++)
		{
			if (controllerTable[a] == originalController)
			{
				controllerTable[a] = nullptr;
				break;
			}
		}

		delete originalController;
	}
}

void ControllerInfoPane::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	EventType eventType = eventData->eventType;

	if (eventType == kEventWidgetActivate)
	{
		if (widget == scriptButton)
		{
			Controller *controller = controllerTable[selectionIndex];
			configurationWidget->CommitConfiguration(controller);

			if (controller->GetBaseControllerType() == kControllerScript)
			{
				ScriptController *scriptController = static_cast<ScriptController *>(controller);
				ScriptObject *scriptObject = scriptController->GetScriptObject();
				if (!scriptObject)
				{
					scriptObject = new ScriptObject;

					ScriptGraph *graph = new ScriptGraph;
					graph->GetScriptEventArray()->AddElement(kEventControllerActivate);
					scriptObject->AddScriptGraph(graph);

					scriptController->SetScriptObject(scriptObject);
					scriptObject->Release();
				}

				AddSubwindow(new ScriptEditor(targetNode, scriptController, scriptObject));
			}
		}
		else if (widget == panelButton)
		{
			AddSubwindow(new PanelEditor(static_cast<const PanelEffect *>(targetNode)));
		}
	}
	else if (eventType == kEventWidgetChange)
	{
		if (widget == controllerListWidget)
		{
			SelectController(controllerListWidget->GetFirstSelectedIndex() - 1);
		}
	}
}


ForceInfoPane::ForceInfoPane() : NodeInfoPane(kNodeInfoForce, TheWorldEditor->GetStringTable()->GetString(StringID('INFO', 'PANE', 'FORC')), "WorldEditor/ForceInfo")
{
	targetField = nullptr;
}

ForceInfoPane::~ForceInfoPane()
{
	if (targetField)
	{
		Force *force = targetField->GetForce();

		int32 count = forceCount;
		for (machine a = 0; a < count; a++)
		{
			if (forceTable[a] != force)
			{
				delete forceTable[a];
			}
		}

		delete[] forceTable;
	}
}

void ForceInfoPane::Preprocess(void)
{
	NodeInfoPane::Preprocess();

	forceListWidget = static_cast<ListWidget *>(FindWidget("List"));
	configurationWidget = static_cast<ConfigurationWidget *>(FindWidget("Config"));

	SetInitialFocusWidget(forceListWidget);

	forceListWidget->AppendListItem(TheWorldEditor->GetStringTable()->GetString(StringID('INFO', 'NONE')));
	selectionIndex = -1;
}

void ForceInfoPane::SelectForce(int32 index)
{
	if (index != selectionIndex)
	{
		if (selectionIndex >= 0)
		{
			configurationWidget->CommitConfiguration(forceTable[selectionIndex]);
			configurationWidget->ReleaseConfiguration();
		}

		selectionIndex = index;
		if (index >= 0)
		{
			const ForceRegistration *registration = validForceArray[index];
			ForceType forceType = registration->GetForceType();

			Force *force = forceTable[index];
			if (!force)
			{
				Force *targetForce = targetField->GetForce();

				if ((targetForce) && (targetForce->GetForceType() == forceType))
				{
					force = targetForce;
				}

				if (!force)
				{
					force = Force::New(forceType);
				}

				forceTable[index] = force;
			}

			configurationWidget->BuildConfiguration(force);
		}
	}
}

void ForceInfoPane::BuildSettings(const Configurable *configurable)
{
	const Field *field = static_cast<const Field *>(configurable);
	targetField = field;

	int32 count = 0;
	const ForceRegistration *registration = Force::GetFirstRegistration();
	while (registration)
	{
		if (registration->ValidField(field))
		{
			validForceArray.AddElement(registration);
			count++;
		}

		registration = registration->Next();
	}

	forceCount = count;
	forceTable = new Force *[count];
	for (machine a = 0; a < count; a++)
	{
		forceTable[a] = nullptr;
	}

	forceListWidget->SelectListItem(0);

	const Force *force = field->GetForce();
	for (machine a = 0; a < count; a++)
	{
		registration = validForceArray[a];

		forceListWidget->AppendListItem(registration->GetForceName());
		if ((force) && (force->GetForceType() == registration->GetForceType()))
		{
			forceListWidget->SelectListItem(a + 1);
			SelectForce(a);
		}
	}
}

void ForceInfoPane::CommitSettings(Configurable *configurable)
{
	Field *field = static_cast<Field *>(configurable);
	Force *originalForce = field->GetForce();

	int32 index = selectionIndex;
	if (index >= 0)
	{
		Force *force = forceTable[index];
		configurationWidget->CommitConfiguration(force);

		if (force == originalForce)
		{
			return;
		}

		field->SetForce(force);
	}
	else
	{
		field->SetForce(nullptr);
	}

	if (originalForce)
	{
		int32 count = forceCount;
		for (machine a = 0; a < count; a++)
		{
			if (forceTable[a] == originalForce)
			{
				forceTable[a] = nullptr;
				break;
			}
		}

		delete originalForce;
	}
}

void ForceInfoPane::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if ((eventData->eventType == kEventWidgetChange) && (widget == forceListWidget))
	{
		SelectForce(forceListWidget->GetFirstSelectedIndex() - 1);
	}
}


NodeInfoWindow::NodeInfoWindow(Editor *editor) : Window("WorldEditor/NodeInfo")
{
	worldEditor = editor;
}

NodeInfoWindow::~NodeInfoWindow()
{
}

void NodeInfoWindow::Preprocess(void)
{
	Window::Preprocess();

	okayButton = static_cast<PushButtonWidget *>(FindWidget("OK"));
	cancelButton = static_cast<PushButtonWidget *>(FindWidget("Cancel"));
	multipaneWidget = static_cast<MultipaneWidget *>(FindWidget("Pane"));

	const List<NodeReference> *nodeList = worldEditor->GetSelectionList();
	const NodeReference *first = nodeList->First();

	const NodeReference *reference = first;
	while (reference)
	{
		const Node *node = reference->GetNode();

		const Object *object = node->GetObject();
		if (object)
		{
			int32 count = object->GetCategoryCount();
			for (machine a = 0; a < count; a++)
			{
				const char		*title;

				Type category = object->GetCategoryType(a, &title);
				NodeInfoPane *pane = paneList.First();
				while (pane)
				{
					if (pane->GetNodeInfoType() == kNodeInfoObjectCategory)
					{
						CategoryInfoPane *categoryInfoPane = static_cast<CategoryInfoPane *>(pane);
						if (categoryInfoPane->GetNodeInfoType() == category)
						{
							break;
						}
					}

					pane = pane->Next();
				}

				if (!pane)
				{
					pane = (category == kEffectFire) ? new FireEffectInfoPane(category, title) : new CategoryInfoPane(kNodeInfoObjectCategory, category, title);
					paneList.Append(pane);
					AppendNewSubnode(pane);
				}

				pane->BuildSettings(object);
			}
		}

		int32 count = node->GetCategoryCount();
		for (machine a = 0; a < count; a++)
		{
			const char	*title;

			Type category = node->GetCategoryType(a, &title);

			NodeInfoPane *pane = paneList.First();
			while (pane)
			{
				if (pane->GetNodeInfoType() == kNodeInfoNodeCategory)
				{
					CategoryInfoPane *categoryInfoPane = static_cast<CategoryInfoPane *>(pane);
					if (categoryInfoPane->GetNodeInfoType() == category)
					{
						break;
					}
				}

				pane = pane->Next();
			}

			if (!pane)
			{
				pane = new CategoryInfoPane(kNodeInfoNodeCategory, category, title);
				paneList.Append(pane);
				AppendNewSubnode(pane);
			}

			pane->BuildSettings(node);
		}

		reference = reference->Next();
	}

	if (first == nodeList->Last())
	{
		const Node *node = first->GetNode();
		NodeType type = node->GetNodeType();

		if (type == kNodeField)
		{
			NodeInfoPane *pane = new ForceInfoPane;
			paneList.Append(pane);
			AppendNewSubnode(pane);
			pane->BuildSettings(node);
		}
		else if (type == kNodeInstance)
		{
			NodeInfoPane *pane = new ModifierInfoPane;
			paneList.Append(pane);
			AppendNewSubnode(pane);
			pane->BuildSettings(node);
		}

		NodeInfoPane *pane = new PropertyInfoPane;
		AppendNewSubnode(pane);
		pane->BuildSettings(node);
		paneList.Append(pane);

		pane = new ConnectorInfoPane;
		paneList.Append(pane);
		AppendNewSubnode(pane);
		pane->BuildSettings(node);

		if (node->GetSuperNode())
		{
			pane = new ControllerInfoPane;
			paneList.Append(pane);
			AppendNewSubnode(pane);
			pane->BuildSettings(node);
		}
	}

	NodeInfoType selectType = worldEditor->GetNodeInfoType();
	Type selectCategory = worldEditor->GetNodeInfoCategory();
	int32 paneSelection = 0;

	int32 index = 0;
	NodeInfoPane *pane = paneList.First();
	while (pane)
	{
		multipaneWidget->AppendPane(pane->GetPaneTitle());
		pane->SetWidgetPosition(multipaneWidget->GetWidgetPosition());

		if (pane->GetNodeInfoType() == selectType)
		{
			if ((selectType >= kNodeInfoCategoryTypeCount) || (static_cast<CategoryInfoPane *>(pane)->GetNodeInfoType() == selectCategory))
			{
				paneSelection = index;
			}
		}

		index++;
		pane = pane->Next();
	}

	index = 0;
	pane = paneList.First();
	while (pane)
	{
		if (index == paneSelection)
		{
			pane->SetVisitedFlag();
			SetFocusWidget(pane->GetInitialFocusWidget());
		}
		else
		{
			pane->Hide();
		}

		index++;
		pane = pane->Next();
	}

	multipaneWidget->SetSelection(paneSelection);
}

void NodeInfoWindow::CommitSettings(void)
{
	const List<NodeReference> *nodeList = worldEditor->GetSelectionList();
	worldEditor->AddOperation(new NodeInfoOperation(nodeList));

	NodeInfoPane *pane = paneList.First();
	while (pane)
	{
		if (pane->GetVisitedFlag())
		{
			NodeInfoType type = pane->GetNodeInfoType();

			const NodeReference *reference = nodeList->First();
			while (reference)
			{
				Node *node = reference->GetNode();

				if (type == kNodeInfoObjectCategory)
				{
					Object *object = node->GetObject();
					if (object)
					{
						pane->CommitSettings(object);
					}
				}
				else
				{
					pane->CommitSettings(node);

					if ((type == kNodeInfoProperties) && (!node->GetSuperNode()))
					{
						worldEditor->SetProcessPropertiesFlag();
					}
				}

				reference = reference->Next();
			}
		}

		pane = pane->Next();
	}

	const NodeReference *gizmoTarget = worldEditor->GetGizmoTarget();

	const NodeReference *reference = nodeList->First();
	while (reference)
	{
		Node *targetNode = reference->GetNode();
		EditorManipulator *manipulator = Editor::GetManipulator(targetNode);

		manipulator->HandleSettingsUpdate();
		bool preprocessFlag = ((!targetNode->GetSuperNode()) || (!manipulator->PredecessorSelected()));

		NodeType type = targetNode->GetNodeType();
		if (type == kNodeGeometry)
		{
			worldEditor->RebuildGeometry(static_cast<Geometry *>(targetNode));
			preprocessFlag = false;
		}
		else if (type == kNodeEffect)
		{
			Object *object = targetNode->GetObject();

			Node *root = targetNode->GetRootNode();
			Node *node = root;
			while (node)
			{
				if ((node->GetNodeType() == kNodeEffect) && (node->GetObject() == object))
				{
					Effect *effect = static_cast<Effect *>(node);
					effect->Neutralize();

					if (effect->GetEffectType() == kEffectTube)
					{
						static_cast<TubeEffectObject *>(object)->Build();
					}

					effect->Preprocess();
				}

				node = root->GetNextNode(node);
			}

			preprocessFlag = false;
		}
		else if (type == kNodeInstance)
		{
			if (worldEditor->GetEditorObject()->GetEditorFlags() & kEditorExpandWorlds)
			{
				preprocessFlag = false;
			}
		}
		else if (type == kNodeModel)
		{
			if (worldEditor->GetEditorObject()->GetEditorFlags() & kEditorExpandModels)
			{
				preprocessFlag = false;
			}
		}

		if (preprocessFlag)
		{
			targetNode->Neutralize();
			targetNode->Preprocess();
		}

		if ((type != kNodeGeometry) && (reference == gizmoTarget))
		{
			worldEditor->PostEvent(GizmoEditorEvent(kEditorEventGizmoTargetModified, targetNode));
		}

		reference = reference->Next();
	}

	worldEditor->PostEvent(EditorEvent(kEditorEventNodeInfoModified));

	worldEditor->SetUpdateConditionalItemsFlag();
	worldEditor->InvalidateAllViewports();
}

void NodeInfoWindow::RememberSelectedPane(void) const
{
	NodeInfoPane *pane = paneList[multipaneWidget->GetSelection()];
	if (pane)
	{
		NodeInfoType type = pane->GetNodeInfoType();
		NodeInfoType category = (type < kNodeInfoCategoryTypeCount) ? static_cast<CategoryInfoPane *>(pane)->GetNodeInfoType() : 0;
		worldEditor->SetNodeInfoData(type, category);
	}
}

void NodeInfoWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	EventType eventType = eventData->eventType;

	if (eventType == kEventWidgetActivate)
	{
		if (widget == okayButton)
		{
			CommitSettings();
			RememberSelectedPane();
			Close();
		}
		else if (widget == cancelButton)
		{
			RememberSelectedPane();
			Close();
		}
		else
		{
			NodeInfoPane *pane = paneList[multipaneWidget->GetSelection()];
			if (pane)
			{
				pane->HandleWidgetEvent(widget, eventData);
			}
		}
	}
	else
	{
		int32 selection = multipaneWidget->GetSelection();

		if ((eventType == kEventWidgetChange) && (widget == multipaneWidget))
		{
			int32 index = 0;
			NodeInfoPane *pane = paneList.First();
			while (pane)
			{
				if (index == selection)
				{
					pane->Show();
					pane->SetVisitedFlag();
					SetFocusWidget(pane->GetInitialFocusWidget());
				}
				else
				{
					pane->Hide();
				}

				index++;
				pane = pane->Next();
			}
		}
		else
		{
			NodeInfoPane *pane = paneList[selection];
			if (pane)
			{
				pane->HandleWidgetEvent(widget, eventData);
			}
		}
	}
}

// ZYUQURM
