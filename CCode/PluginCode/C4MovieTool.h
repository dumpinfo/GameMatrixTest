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


#ifndef C4MovieTool_h
#define C4MovieTool_h


#include "C4Plugins.h"
#include "C4Movies.h"


extern "C"
{
	C4MODULEEXPORT C4::Plugin *CreatePlugin(void);
}


namespace C4
{
	class MovieTool : public Plugin, public Singleton<MovieTool>
	{
		private:

			StringTable						stringTable;

			CommandObserver<MovieTool>		movieCommandObserver;
			Command							movieCommand;
			MenuItemWidget					movieMenuItem;

			CommandObserver<MovieTool>		importMovieCommandObserver;
			Command							importMovieCommand;
			MenuItemWidget					importMovieMenuItem;

			Link<FilePicker>				moviePicker;

			static void MoviePicked(FilePicker *picker, void *cookie);
			void HandleOpenMovieMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleMovieCommand(Command *command, const char *text);

			void HandleImportMovieMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleImportMovieCommand(Command *command, const char *text);

		public:

			MovieTool();
			~MovieTool();

			const StringTable *GetStringTable(void) const
			{
				return (&stringTable);
			}
	};


	extern MovieTool *TheMovieTool;
}


#endif

// ZYUQURM
