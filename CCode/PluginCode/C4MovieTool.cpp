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


#include "C4MovieTool.h"
#include "C4MovieImporter.h"
#include "C4MoviePlayer.h"


using namespace C4;


MovieTool *C4::TheMovieTool = nullptr;


C4::Plugin *CreatePlugin(void)
{
	return (new MovieTool);
}


MovieTool::MovieTool() :
		Singleton<MovieTool>(TheMovieTool),
		stringTable("MovieTool/strings"),

		movieCommandObserver(this, &MovieTool::HandleMovieCommand),
		movieCommand("movie", &movieCommandObserver),
		movieMenuItem(stringTable.GetString(StringID('PLAY', 'MCMD')), WidgetObserver<MovieTool>(this, &MovieTool::HandleOpenMovieMenuItem)),

		importMovieCommandObserver(this, &MovieTool::HandleImportMovieCommand),
		importMovieCommand("imovie", &importMovieCommandObserver),
		importMovieMenuItem(stringTable.GetString(StringID('IMPT', 'MCMD')), WidgetObserver<MovieTool>(this, &MovieTool::HandleImportMovieMenuItem))
{
	TheEngine->AddCommand(&movieCommand);
	TheEngine->AddCommand(&importMovieCommand);

	ThePluginMgr->AddToolMenuItem(&movieMenuItem);
	ThePluginMgr->AddToolMenuItem(&importMovieMenuItem);
}

MovieTool::~MovieTool()
{
	FilePicker *picker = moviePicker;
	delete picker;

	delete TheMovieProgressWindow;
	delete TheImportMovieWindow;

	MovieWindow::windowList.Purge();
}

void MovieTool::MoviePicked(FilePicker *picker, void *cookie)
{
	ResourceName name((picker) ? &picker->GetResourceName()[0] : static_cast<const char *>(cookie));
	MovieResult result = MovieWindow::Open(name);
	if (result != kMovieOkay)
	{
		String<kMaxCommandLength> output(TheMovieTool->GetStringTable()->GetString(StringID('PLAY', 'NRES')));
		output += name;
		Engine::Report(output);
	}
}

void MovieTool::HandleOpenMovieMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	FilePicker *picker = moviePicker;
	if (picker)
	{
		TheInterfaceMgr->SetActiveWindow(picker);
	}
	else
	{
		const char *title = stringTable.GetString(StringID('PLAY', 'OPEN'));

		picker = new FilePicker('MOVI', title, TheResourceMgr->GetGenericCatalog(), MovieResource::GetDescriptor());
		picker->SetCompletionProc(&MoviePicked);

		moviePicker = picker;
		TheInterfaceMgr->AddWidget(picker);
	}
}

void MovieTool::HandleMovieCommand(Command *command, const char *text)
{
	if (*text != 0)
	{
		ResourceName	name;

		Text::ReadString(text, name, kMaxResourceNameLength);
		MoviePicked(nullptr, &name[0]);
	}
	else
	{
		HandleOpenMovieMenuItem(nullptr, nullptr);
	}
}

void MovieTool::HandleImportMovieMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	ImportMovieWindow::Open();
}

void MovieTool::HandleImportMovieCommand(Command *command, const char *text)
{
	HandleImportMovieMenuItem(nullptr, nullptr); 
}

// ZYUQURM
