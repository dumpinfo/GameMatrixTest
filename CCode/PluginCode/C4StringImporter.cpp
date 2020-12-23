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


#include "C4StringImporter.h"


using namespace C4;


/*
	struct $entry (id = "Entry")
	{
		property (id = "id")
		{
			type {unsigned_int32}
		}

		data
		{
			type {string}
			instances {int32 {0, 1}}
			elements {int32 {1, 1}}
		}

		sub
		{
			ref {$entry}
		}
	}
*/


StringImporter *C4::TheStringImporter = nullptr;


ResourceDescriptor TextResource::descriptor("txt", kResourceTerminatorByte);


C4::Plugin *CreatePlugin(void)
{
	return (new StringImporter);
}


TextResource::TextResource(const char *name, ResourceCatalog *catalog) : Resource<TextResource>(name, catalog)
{
}

TextResource::~TextResource()
{
}


EntryStructure::EntryStructure() : Structure(kStructureEntry)
{
	entryIdentifier = -1;
	entryString = nullptr;
}

EntryStructure::~EntryStructure()
{
}

bool EntryStructure::ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value)
{
	if (identifier == "id")
	{
		*type = kDataUnsignedInt32;
		*value = &entryIdentifier;
		return (true);
	}

	return (false);
}

bool EntryStructure::ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const
{
	StructureType type = structure->GetStructureType();
	return ((type == kDataString) || (type == kStructureEntry));
}

DataResult EntryStructure::ProcessData(DataDescription *dataDescription)
{
	DataResult result = Structure::ProcessData(dataDescription);
	if (result == kDataOkay)
	{
		const Structure *structure = GetFirstSubnode();
		while (structure)
		{
			if (structure->GetStructureType() == kDataString)
			{
				if (entryString)
				{
					return (kDataExtraneousSubstructure);
				}

				const DataStructure<StringDataType> *dataStructure = static_cast<const DataStructure<StringDataType> *>(structure);
				if (dataStructure->GetDataElementCount() != 1)
				{
					return (kDataInvalidDataFormat);
				}

				entryString = &dataStructure->GetDataElement(0);
			} 

			structure = structure->Next();
		} 
	}
 
	return (result);
}
 

StringTableDataDescription::StringTableDataDescription() 
{ 
}

StringTableDataDescription::~StringTableDataDescription()
{ 
}

Structure *StringTableDataDescription::CreateStructure(const String<>& identifier) const
{
	if (identifier == "Entry")
	{
		return (new EntryStructure);
	}

	return (nullptr);
}

bool StringTableDataDescription::WriteStringTable(const Structure *root, File *file)
{
	bool result = false;

	const Structure *structure = root->GetFirstSubnode();
	while (structure)
	{
		if (structure->GetStructureType() == kStructureEntry)
		{
			const EntryStructure *entry = static_cast<const EntryStructure *>(structure);
			unsigned_int32 identifier = entry->GetIdentifier();
			if (identifier != -1)
			{
				result = true;

				StringHeader header(identifier);
				unsigned_int32 headerPosition = (unsigned_int32) file->GetPosition();
				file->Write(&header, sizeof(StringHeader));

				unsigned_int32 length = 1;
				const String<> *string = entry->GetString();
				if (string)
				{
					length += string->Length();
					file->Write(&(*string)[0], length);
					file->WritePad(4);
				}
				else
				{
					static const int32 zero = 0;
					file->Write(&zero, 4);
				}

				if (WriteStringTable(entry, file))
				{
					header.SetFirstSubstringOffset(sizeof(StringHeader) + ((length + 3) & ~3));
				}

				unsigned_int32 position = (unsigned_int32) file->GetPosition();
				if (entry->Next())
				{
					header.SetNextStringOffset(position - headerPosition);
				}

				file->SetPosition(headerPosition);
				file->Write(&header, sizeof(StringHeader));
				file->SetPosition(position);
			}
		}

		structure = structure->Next();
	}

	return (result);
}

void StringTableDataDescription::WriteResourceFile(const char *name) const
{
	File			file;
	ResourcePath	path;

	TheResourceMgr->GetGenericCatalog()->GetResourcePath(StringTableResource::GetDescriptor(), name, &path);
	TheResourceMgr->CreateDirectoryPath(path);

	if (file.Open(path, kFileCreate) == kFileOkay)
	{
		int32 endian = 1;
		file.Write(&endian, 4);

		WriteStringTable(GetRootStructure(), &file);
	}
}


StringImporter::StringImporter() :
		Singleton<StringImporter>(TheStringImporter),
		stringTable("StringImporter/strings"),
		importStringCommandObserver(this, &StringImporter::HandleImportStringCommand),
		importStringCommand("istring", &importStringCommandObserver),
		importStringMenuItem(stringTable.GetString(StringID('MCMD')), WidgetObserver<StringImporter>(this, &StringImporter::HandleImportStringMenuItem))
{
	TheEngine->AddCommand(&importStringCommand);
	ThePluginMgr->AddToolMenuItem(&importStringMenuItem);
}

StringImporter::~StringImporter()
{
	FilePicker *picker = importStringPicker;
	delete picker;
}

void StringImporter::ImportStringPicked(FilePicker *picker, void *cookie)
{
	ResourceName	name;

	if (picker)
	{
		name = picker->GetFileName();
		name[Text::GetResourceNameLength(name)] = 0;
	}
	else
	{
		name = static_cast<const char *>(cookie);
	}

	TextResource *textResource = TextResource::Get(name, 0, ThePluginMgr->GetImportCatalog());
	if (textResource)
	{
		StringTableDataDescription		stringTableDescription;

		const char *text = static_cast<const char *>(textResource->GetData());
		DataResult result = stringTableDescription.ProcessText(text);
		if (result == kDataOkay)
		{
			stringTableDescription.WriteResourceFile(name);
		}
		else
		{
			const StringTable *table = TheStringImporter->GetStringTable();

			const char *title = table->GetString(StringID('ERRR'));
			const char *message = table->GetString(StringID('EMES'));

			String<> error(table->GetString(StringID('LINE')));
			((error += stringTableDescription.GetErrorLine()) += ": ") += Engine::GetDataResultString(result);

			ErrorDialog *dialog = new ErrorDialog(title, message, error, ResourcePath(name) += TextResource::GetDescriptor()->GetExtension());
			TheInterfaceMgr->AddWidget(dialog);
		}

		textResource->Release();
	}
	else
	{
		const StringTable *table = TheStringImporter->GetStringTable();
		String<kMaxCommandLength> output(table->GetString(StringID('NRES')));
		output += name;
		Engine::Report(output);
	}
}

void StringImporter::HandleImportStringMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	FilePicker *picker = importStringPicker;
	if (picker)
	{
		TheInterfaceMgr->SetActiveWindow(picker);
	}
	else
	{
		const char *title = stringTable.GetString(StringID('OPEN'));

		picker = new FilePicker('ISTR', title, ThePluginMgr->GetImportCatalog(), TextResource::GetDescriptor());
		picker->SetCompletionProc(&ImportStringPicked);

		importStringPicker = picker;
		TheInterfaceMgr->AddWidget(picker);
	}
}

void StringImporter::HandleImportStringCommand(Command *command, const char *text)
{
	if (*text != 0)
	{
		ResourceName	name;

		Text::ReadString(text, name, kMaxResourceNameLength);
		ImportStringPicked(nullptr, &name[0]);
	}
	else
	{
		HandleImportStringMenuItem(nullptr, nullptr);
	}
}

// ZYUQURM
