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


#ifndef C4SoundTool_h
#define C4SoundTool_h


#include "C4Plugins.h"
#include "C4StringTable.h"
#include "C4FilePicker.h"


extern "C"
{
	C4MODULEEXPORT C4::Plugin *CreatePlugin(void);
}


namespace C4
{
	class SoundTool : public Plugin, public Singleton<SoundTool>
	{
		private:

			StringTable						stringTable;

			CommandObserver<SoundTool>		soundCommandObserver;
			Command							soundCommand;
			MenuItemWidget					soundMenuItem;

			CommandObserver<SoundTool>		importSoundCommandObserver;
			Command							importSoundCommand;
			MenuItemWidget					importSoundMenuItem;

			Link<FilePicker>				soundPicker;
			Link<FilePicker>				importSoundPicker;

			static void SoundPicked(FilePicker *picker, void *cookie);
			void HandleOpenSoundMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleSoundCommand(Command *command, const char *text);

			static void ImportSoundPicked(FilePicker *picker, void *cookie);
			void HandleImportSoundMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleImportSoundCommand(Command *command, const char *text);

		public:

			SoundTool();
			~SoundTool();

			const StringTable *GetStringTable(void) const
			{
				return (&stringTable);
			}
	};


	extern SoundTool *TheSoundTool;
}


#endif

// ZYUQURM
