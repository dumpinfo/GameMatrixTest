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


#ifndef C4TerrainPalette_h
#define C4TerrainPalette_h


#include "C4TextureImporter.h"


namespace C4
{
	class TerrainPaletteWindow : public Window, public Singleton<TerrainPaletteWindow>
	{
		private:

			PushButtonWidget	*saveButton;
			PushButtonWidget	*cancelButton;

			PushButtonWidget	*addButton;
			PushButtonWidget	*removeButton;

			ListWidget			*textureList;

			static void OutputTexturePicked(FilePicker *picker, void *cookie);
			static void PaletteEntryPicked(FilePicker *picker, void *cookie);

		public:

			TerrainPaletteWindow();
			~TerrainPaletteWindow();

			static void Open(void);

			void Preprocess(void) override;
			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;

			static void GenerateTerrainPalette(const char *outputName, const Array<ResourceName>& inputNameArray);
	};


	extern TerrainPaletteWindow *TheTerrainPaletteWindow;
}


#endif

// ZYUQURM
