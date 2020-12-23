 

#include "C4ConfigData.h"
#include "C4FilePicker.h"
#include "C4ToolWindows.h"
#include "C4Input.h"


using namespace C4;


ResourceDescriptor ConfigResource::descriptor("cfg", kResourceTerminatorByte);


ConfigResource::ConfigResource(const char *name, ResourceCatalog *catalog) : Resource<ConfigResource>(name, catalog)
{
}

ConfigResource::~ConfigResource()
{
}


VariableStructure::VariableStructure() : Structure(kStructureVariable)
{
}

VariableStructure::~VariableStructure()
{
}

bool VariableStructure::ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value)
{
	if (identifier == "name")
	{
		*type = kDataString;
		*value = &variableName;
		return (true);
	}

	return (false);
}

bool VariableStructure::ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const
{
	return (structure->GetStructureType() == kDataString);
}

DataResult VariableStructure::ProcessData(DataDescription *dataDescription)
{
	if (variableName[0] != 0)
	{
		const Structure *structure = GetFirstSubnode();
		if (structure)
		{
			const DataStructure<StringDataType> *dataStructure = static_cast<const DataStructure<StringDataType> *>(structure);
			if (dataStructure->GetDataElementCount() == 1)
			{
				Variable *variable = TheEngine->GetVariable(variableName);
				if (!variable)
				{
					variable = new Variable(variableName);
					TheEngine->AddVariable(variable);
				}

				variable->SetValue(dataStructure->GetDataElement(0));
				return (kDataOkay);
			}
		}
	}

	return (kDataConfigVariableInvalid);
}


VisitStructure::VisitStructure() : Structure(kStructureVisit)
{
	visitType = 0;
}

VisitStructure::~VisitStructure()
{
}

bool VisitStructure::ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value)
{
	if (identifier == "type")
	{
		*type = kDataUnsignedInt32;
		*value = &visitType;
		return (true);
	}

	return (false);
}

bool VisitStructure::ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const
{
	return (structure->GetStructureType() == kDataString);
}

DataResult VisitStructure::ProcessData(DataDescription *dataDescription)
{ 
	if (visitType != 0)
	{
		const Structure *structure = GetFirstSubnode(); 
		if (structure)
		{ 
			const DataStructure<StringDataType> *dataStructure = static_cast<const DataStructure<StringDataType> *>(structure);
			if (dataStructure->GetDataElementCount() == 1)
			{ 
				FilePicker::SetVisit(visitType, dataStructure->GetDataElement(0));
				return (kDataOkay); 
			} 
		}
	}

	return (kDataConfigVisitInvalid); 
}


HistoryStructure::HistoryStructure() : Structure(kStructureHistory)
{
}

HistoryStructure::~HistoryStructure()
{
}

bool HistoryStructure::ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const
{
	return (structure->GetStructureType() == kDataString);
}

DataResult HistoryStructure::ProcessData(DataDescription *dataDescription)
{
	const Structure *structure = GetFirstSubnode();
	while (structure)
	{
		const DataStructure<StringDataType> *dataStructure = static_cast<const DataStructure<StringDataType> *>(structure);
		int32 count = dataStructure->GetDataElementCount();
		for (machine a = 0; a < count; a++)
		{
			ConsoleWindow::AddCommandHistory(dataStructure->GetDataElement(a));
		}

		structure = structure->Next();
	}

	return (kDataOkay);
}


DeviceStructure::DeviceStructure() : Structure(kStructureDevice)
{
}

DeviceStructure::~DeviceStructure()
{
}

bool DeviceStructure::ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value)
{
	if (identifier == "name")
	{
		*type = kDataString;
		*value = &deviceName;
		return (true);
	}

	if (identifier == "type")
	{
		*type = kDataUnsignedInt32;
		*value = &deviceType;
		return (true);
	}

	return (false);
}

bool DeviceStructure::ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const
{
	return (structure->GetStructureType() == kStructureControl);
}

DataResult DeviceStructure::ProcessData(DataDescription *dataDescription)
{
	const InputDevice *device = nullptr;

	if (deviceName[0] != 0)
	{
		if (deviceType != 0)
		{
			device = TheInputMgr->FindDevice(deviceType, deviceName);
		}
		else
		{
			device = TheInputMgr->FindDevice(deviceName);
		}
	}

	if ((!device) && (deviceType != 0))
	{
		device = TheInputMgr->FindDevice(deviceType);
	}

	if (device)
	{
		const Structure *structure = GetFirstSubnode();
		while (structure)
		{
			InputControl *control = device->FindControl(static_cast<const ControlStructure *>(structure)->controlName);
			if (control)
			{
				control->SetControlAction(nullptr);

				const Structure *substruct = structure->GetFirstSubnode();
				if (substruct)
				{
					substruct = substruct->GetFirstSubnode();
					if (substruct)
					{
						StructureType type = substruct->GetStructureType();

						if (type == kDataUnsignedInt32)
						{
							const DataStructure<UnsignedInt32DataType> *dataStructure = static_cast<const DataStructure<UnsignedInt32DataType> *>(substruct);
							if (dataStructure->GetDataElementCount() == 1)
							{
								Action *action = TheInputMgr->FindAction(dataStructure->GetDataElement(0));
								if (action)
								{
									control->SetControlAction(action);
								}
							}
						}
						else if (type == kDataString)
						{
							const DataStructure<StringDataType> *dataStructure = static_cast<const DataStructure<StringDataType> *>(substruct);
							if (dataStructure->GetDataElementCount() == 1)
							{
								CommandAction *action = new CommandAction(dataStructure->GetDataElement(0));
								TheInputMgr->AddAction(action);
								control->SetControlAction(action);
							}
						}
					}
				}
			}

			structure = structure->Next();
		}
	}

	return (kDataOkay);
}


ControlStructure::ControlStructure() : Structure(kStructureControl)
{
}

ControlStructure::~ControlStructure()
{
}

bool ControlStructure::ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value)
{
	if (identifier == "name")
	{
		*type = kDataString;
		*value = &controlName;
		return (true);
	}

	return (false);
}

bool ControlStructure::ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const
{
	return (structure->GetStructureType() == kStructureAction);
}


ActionStructure::ActionStructure() : Structure(kStructureAction)
{
}

ActionStructure::~ActionStructure()
{
}

bool ActionStructure::ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const
{
	StructureType type = structure->GetStructureType();
	return ((type == kDataUnsignedInt32) || (type == kDataString));
}


ConfigDataDescription::ConfigDataDescription()
{
}

ConfigDataDescription::~ConfigDataDescription()
{
}

Structure *ConfigDataDescription::CreateStructure(const String<>& identifier) const
{
	if (identifier == "Variable")
	{
		return (new VariableStructure);
	}

	if (identifier == "Visit")
	{
		return (new VisitStructure);
	}

	if (identifier == "History")
	{
		return (new HistoryStructure);
	}

	if (identifier == "Device")
	{
		return (new DeviceStructure);
	}

	if (identifier == "Control")
	{
		return (new ControlStructure);
	}

	if (identifier == "Action")
	{
		return (new ActionStructure);
	}

	return (nullptr);
}

bool ConfigDataDescription::ValidateTopLevelStructure(const Structure *structure) const
{
	StructureType type = structure->GetStructureType();
	return ((type == kStructureVariable) || (type == kStructureVisit) || (type == kStructureHistory) || (type == kStructureDevice));
}

DataResult ConfigDataDescription::Execute(const char *name, ResourceCatalog *catalog)
{
	ConfigResource *resource = ConfigResource::Get(name, 0, catalog);
	if (resource)
	{
		ConfigDataDescription	configData;

		DataResult result = configData.ProcessText(resource->GetText());

		resource->Release();
		return (result);
	}

	return (kDataConfigResourceMissing);
}

void ConfigDataDescription::WriteEngineConfig(const char *name)
{
	File			file;
	ResourcePath	path;

	TheResourceMgr->GetConfigCatalog()->GetResourcePath(ConfigResource::GetDescriptor(), name, &path);
	if (file.Open(path, kFileCreate) == kFileOkay)
	{
		const Variable *variable = TheEngine->GetFirstVariable();
		while (variable)
		{
			if (!(variable->GetVariableFlags() & kVariableNonpersistent))
			{
				file << "Variable (name = \"" << variable->GetName() << "\") {string {\"";
				file << String<>(variable->GetValue()).EncodeEscapeSequences();
				file << "\"}}\n";
			}

			variable = variable->Next();
		}

		const FilePickerVisit *visit = FilePicker::GetFirstVisit();
		if (visit)
		{
			file << "\n";
			do
			{
				file << "Visit (type = '" << Text::TypeToString(visit->GetVisitType()) << "') {string {\"";
				file << String<>(visit->GetVisitPath()).EncodeEscapeSequences();
				file << "\"}}\n";

				visit = visit->Next();
			} while (visit);
		}

		int32 count = ConsoleWindow::GetCommandHistoryCount();
		if (count > 0)
		{
			file << "\nHistory\n{\n\tstring\n\t{\n\t\t\"";

			for (machine a = 0; a < count; a++)
			{
				file << String<>(ConsoleWindow::GetCommandHistory(a)).EncodeEscapeSequences();

				if (a < count - 1)
				{
					file << "\",\n\t\t\"";
				}
			}

			file << "\"\n\t}\n}\n";
		}
	}
}

void ConfigDataDescription::WriteInputConfig(const char *name)
{
	File			file;
	ResourcePath	path;

	TheResourceMgr->GetConfigCatalog()->GetResourcePath(ConfigResource::GetDescriptor(), name, &path);
	if (file.Open(path, kFileCreate) == kFileOkay)
	{
		InputDevice *device = TheInputMgr->GetFirstDevice();
		while (device)
		{
			if (device->Previous())
			{
				file << "\n";
			}

			file << "Device (name = \"";
			file << String<>(device->GetDeviceName()).EncodeEscapeSequences();
			file << "\", type = '" << Text::TypeToString(device->GetDeviceType()) << "')\n{\n";

			InputControl *control = device->GetFirstControl();
			while (control)
			{
				Action *action = control->GetControlAction();
				if (action)
				{
					unsigned_int32 type = action->GetActionType();
					if (type != 0)
					{
						if (!(action->GetActionFlags() & kActionImmutable))
						{
							file << "\tControl (name = \"";
							file << String<>(control->GetControlName()).EncodeEscapeSequences();
							file << "\")\n\t{\n\t\tAction {unsigned_int32 {'" << Text::TypeToString(type) << "'}}\n\t}\n";
						}
					}
					else
					{
						CommandAction *cmd = static_cast<CommandAction *>(action);

						file << "\tControl (name = \"";
						file << String<>(control->GetControlName()).EncodeEscapeSequences();
						file << "\")\n\t{\n\t\tAction {string {\"";
						file << String<>(cmd->GetCommand()).EncodeEscapeSequences();
						file << "\"}}\n\t}\n";
					}
				}

				control = device->GetNextControl(control);
			}

			file << "}\n";
			device = device->Next();
		}
	}
}

// ZYUQURM
