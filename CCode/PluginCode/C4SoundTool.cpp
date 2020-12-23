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


#include "C4SoundTool.h"
#include "C4SoundPlayer.h"
#include "C4SoundImporter.h"


using namespace C4;


SoundTool *C4::TheSoundTool = nullptr;


C4::Plugin *CreatePlugin(void)
{
	return (new SoundTool);
}


SoundTool::SoundTool() :
		Singleton<SoundTool>(TheSoundTool),
		stringTable("SoundTool/strings"),
		soundCommandObserver(this, &SoundTool::HandleSoundCommand),
		soundCommand("sound", &soundCommandObserver),
		soundMenuItem(stringTable.GetString(StringID('PLAY', 'MCMD')), WidgetObserver<SoundTool>(this, &SoundTool::HandleOpenSoundMenuItem)),
		importSoundCommandObserver(this, &SoundTool::HandleImportSoundCommand),
		importSoundCommand("isound", &importSoundCommandObserver),
		importSoundMenuItem(stringTable.GetString(StringID('IMPT', 'MCMD')), WidgetObserver<SoundTool>(this, &SoundTool::HandleImportSoundMenuItem))
{
	TheEngine->AddCommand(&soundCommand);
	TheEngine->AddCommand(&importSoundCommand);

	ThePluginMgr->AddToolMenuItem(&soundMenuItem);
	ThePluginMgr->AddToolMenuItem(&importSoundMenuItem);
}

SoundTool::~SoundTool()
{
	FilePicker *picker = soundPicker;
	delete picker;

	SoundWindow::windowList.Purge();
}

void SoundTool::SoundPicked(FilePicker *picker, void *cookie)
{
	ResourceName	name;

	if (picker)
	{
		name = picker->GetResourceName();
	}
	else
	{
		name = static_cast<const char *>(cookie);
	}

	if (!SoundWindow::Open(name))
	{
		const StringTable *table = TheSoundTool->GetStringTable();
		String<kMaxCommandLength> output(table->GetString(StringID('PLAY', 'NRES')));
		output += name;
		Engine::Report(output);
	}
}

void SoundTool::HandleOpenSoundMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	FilePicker *picker = soundPicker;
	if (picker)
	{
		TheInterfaceMgr->SetActiveWindow(picker);
	}
	else
	{
		const char *title = stringTable.GetString(StringID('PLAY', 'OPEN'));

		picker = new SoundPicker('SOND', title, TheResourceMgr->GetGenericCatalog(), SoundResource::GetDescriptor());
		picker->SetCompletionProc(&SoundPicked);

		soundPicker = picker;
		TheInterfaceMgr->AddWidget(picker);
	}
}

void SoundTool::HandleSoundCommand(Command *command, const char *text)
{
	if (*text != 0)
	{
		ResourceName	name;

		Text::ReadString(text, name, kMaxResourceNameLength);
		SoundPicked(nullptr, &name[0]);
	}
	else
	{
		HandleOpenSoundMenuItem(nullptr, nullptr);
	}
}

void SoundTool::ImportSoundPicked(FilePicker *picker, void *cookie)
{
} 

void SoundTool::HandleImportSoundMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{ 
	FilePicker *picker = importSoundPicker;
	if (picker) 
	{
		TheInterfaceMgr->SetActiveWindow(picker);
	} 
	else
	{ 
		const char *title = stringTable.GetString(StringID('IMPT', 'OPEN')); 

		picker = new FilePicker('ISND', title, ThePluginMgr->GetImportCatalog(), WaveResource::GetDescriptor());
		picker->SetCompletionProc(&ImportSoundPicked);
 
		importSoundPicker = picker;
		TheInterfaceMgr->AddWidget(picker);
	}
}

void SoundTool::HandleImportSoundCommand(Command *command, const char *text)
{
	if (*text != 0)
	{
		ResourceName	name;

		Text::ReadString(text, name, kMaxResourceNameLength);
		ImportSoundPicked(nullptr, &name[0]);
	}
	else
	{
		HandleImportSoundMenuItem(nullptr, nullptr);
	}
}

// ZYUQURM
