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


#ifndef C4TextureViewer_h
#define C4TextureViewer_h


#include "C4TextureTool.h"


namespace C4
{
	class TextureWindow : public Window, public ListElement<TextureWindow>
	{
		friend class TextureTool;

		private:

			Vector2D					viewportSize;
			Vector2D					imageSize;

			ImageWidget					*textureImage[6];

			CheckWidget					*flipBox;
			CheckWidget					*blendBox;
			PopupMenuWidget				*backgroundMenu;

			QuadWidget					*colorBackground;
			ImageWidget					*checkerBackground;

			int32						mipmapLevel;

			ResourceName				resourceName;
			TextureResource				*textureResource;
			const TextureHeader			*textureHeader;

			static List<TextureWindow>	windowList;

			const Vector2D& CalculateViewportSize(const TextureHeader *header);
			void SetImagePosition(void);

		public:

			TextureWindow(const char *name, TextureResource *resource, const TextureHeader *header);
			~TextureWindow();

			const char *GetResourceName(void) const
			{
				return (resourceName);
			}

			C4TEXTUREAPI static ResourceResult Open(const char *name);

			void Preprocess(void) override;

			bool HandleKeyboardEvent(const KeyboardEventData *eventData) override;
			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};
}


#endif

// ZYUQURM
