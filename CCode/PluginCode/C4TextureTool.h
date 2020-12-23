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


#ifndef C4TextureTool_h
#define C4TextureTool_h


#include "C4Plugins.h"
#include "C4StringTable.h"
#include "C4FilePicker.h"


#ifdef C4TEXTURE

	#define C4TEXTUREAPI C4MODULEEXPORT


	extern "C"
	{
		C4MODULEEXPORT C4::Plugin *CreatePlugin(void);
	}

#else

	#define C4TEXTUREAPI C4MODULEIMPORT

#endif


namespace C4
{
	class TextureTool : public Plugin, public Singleton<TextureTool>
	{
		private:

			StringTable						stringTable;

			CommandObserver<TextureTool>	textureCommandObserver;
			Command							textureCommand;
			MenuItemWidget					textureMenuItem;

			CommandObserver<TextureTool>	importTextureCommandObserver;
			Command							importTextureCommand;
			MenuItemWidget					importTextureMenuItem;

			CommandObserver<TextureTool>	terrainPaletteCommandObserver;
			Command							terrainPaletteCommand;
			MenuItemWidget					terrainPaletteMenuItem;

			CommandObserver<TextureTool>	generateTexturesCommandObserver;
			Command							generateTexturesCommand;
			MenuItemWidget					generateTexturesMenuItem;

			CommandObserver<TextureTool>	updateTexturesCommandObserver;
			Command							updateTexturesCommand;

			Link<FilePicker>				texturePicker;
			Link<FilePicker>				targaPicker;

			static void TexturePicked(FilePicker *picker, void *cookie);
			void HandleOpenTextureMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleTextureCommand(Command *command, const char *text);

			static void ImportTexturePicked(FilePicker *picker, void *cookie);
			void HandleImportTextureMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleImportTextureCommand(Command *command, const char *text);

			void HandleTerrainPaletteMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleTerrainPaletteCommand(Command *command, const char *text);

			void HandleGenerateTexturesMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleGenerateTexturesCommand(Command *command, const char *text);

			void HandleUpdateTexturesCommand(Command *command, const char *text);
			static void UpdateTexturesDirectory(const char *directory);
			static void UpdateTextureResource(const char *name1);

		public:

			TextureTool();
			~TextureTool();

			const StringTable *GetStringTable(void) const
			{
				return (&stringTable);
			}

			static void GenerateTextures(World *world, unsigned_int32 flags);
	};


	C4TEXTUREAPI extern TextureTool *TheTextureTool;
}


#endif

// ZYUQURM
