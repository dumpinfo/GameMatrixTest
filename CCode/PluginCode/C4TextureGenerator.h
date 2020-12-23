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


#ifndef C4TextureGeneration_h
#define C4TextureGeneration_h


#include "C4TextureImporter.h"


namespace C4
{
	enum
	{
		kGenerateLightProjector		= 1 << 0,
		kGenerateEnvironmentMap		= 1 << 1,
		kGenerateImpostorImage		= 1 << 2
	};


	class ImpostorProperty;


	class TextureGenerator : public TextureImporter
	{
		private:

			World		*currentWorld;
			int32		filterSize;

			void GetOutputTexturePath(ResourcePath *path) const;

			void FilterCubeTexture(Color4C *image, int32 pixelCount, int32 width);

		public:

			TextureGenerator(World *world, const char *name);
			~TextureGenerator();

			void SetFilterSize(int32 size)
			{
				filterSize = size;
			}

			EngineResult GenerateCubeTexture(Node *node, int32 width, TextureFormat format);
			EngineResult GenerateSpotTexture(Node *node, float apex, float aspect, int32 width, TextureFormat format);
			EngineResult GenerateImpostorTexture(Node *node, const ImpostorProperty *property);
	};


	class TextureGeneratorWindow : public Window, public Singleton<TextureGeneratorWindow>
	{
		private:

			PushButtonWidget	*generateButton;
			PushButtonWidget	*cancelButton;

			CheckWidget			*lightBox;
			CheckWidget			*environmentBox;
			CheckWidget			*impostorBox;

			TextureGeneratorWindow();

		public:

			~TextureGeneratorWindow();

			static void Open(void);

			void Preprocess(void) override;
			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};


	extern TextureGeneratorWindow *TheTextureGeneratorWindow;
}


#endif

// ZYUQURM
