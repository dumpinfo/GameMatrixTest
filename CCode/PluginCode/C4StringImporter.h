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


#ifndef C4StringImporter_h
#define C4StringImporter_h


#include "C4Plugins.h"
#include "C4StringTable.h"
#include "C4FilePicker.h"


extern "C"
{
	C4MODULEEXPORT C4::Plugin *CreatePlugin(void);
}


namespace C4
{
	enum
	{
		kStructureEntry		= 'etry'
	};


	class StringInfo;


	class TextResource : public Resource<TextResource>
	{
		friend class Resource<TextResource>;

		private:

			static ResourceDescriptor	descriptor;

			~TextResource();

		public:

			TextResource(const char *name, ResourceCatalog *catalog);
	};


	class EntryStructure : public Structure
	{
		private:

			unsigned_int32		entryIdentifier;
			const String<>		*entryString;

		public:

			EntryStructure();
			~EntryStructure();

			unsigned_int32 GetIdentifier(void) const
			{
				return (entryIdentifier);
			}

			const String<> *GetString(void) const
			{
				return (entryString);
			}

			bool ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value) override;
			bool ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const override;

			DataResult ProcessData(DataDescription *dataDescription) override;
	};


	class StringTableDataDescription : public DataDescription
	{
		private:

			static bool WriteStringTable(const Structure *root, File *file);

		public:

			StringTableDataDescription();
			~StringTableDataDescription();

			Structure *CreateStructure(const String<>& identifier) const override;

			void WriteResourceFile(const char *name) const;
	};


	class StringImporter : public Plugin, public Singleton<StringImporter>
	{
		private:

			StringTable							stringTable;

			CommandObserver<StringImporter>		importStringCommandObserver;
			Command								importStringCommand;
			MenuItemWidget						importStringMenuItem;

			Link<FilePicker>					importStringPicker;

			static void ImportStringPicked(FilePicker *picker, void *cookie); 

			void HandleImportStringMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleImportStringCommand(Command *command, const char *text); 

		public: 

			StringImporter();
			~StringImporter(); 

			const StringTable *GetStringTable(void) const 
			{ 
				return (&stringTable);
			}
	};
 

	extern StringImporter *TheStringImporter;
}


#endif

// ZYUQURM
