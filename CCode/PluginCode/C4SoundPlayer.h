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


#ifndef C4SoundPlayer_h
#define C4SoundPlayer_h


#include "C4Sound.h"
#include "C4Widgets.h"


namespace C4
{
	enum
	{
		kWidgetSound	= 'sond'
	};


	class SoundWidget final : public RenderableWidget
	{
		private:

			int32				quadCount;

			VertexBuffer		vertexBuffer;
			List<Attribute>		attributeList;
			DiffuseAttribute	diffuseAttribute;

		public:

			SoundWidget(const Vector2D& size, int32 count, const ColorRGBA& color);
			~SoundWidget();

			void Preprocess(void) override;

			void BuildSound(const Sound *sound, int32 channel = 0);
	};


	class SoundWindow : public Window, public ListElement<SoundWindow>
	{
		friend class SoundTool;

		private:

			ResourceName				resourceName;
			Sound						*soundObject;
			bool						streamingFlag;

			int32						soundTime;
			int32						soundDuration;

			IconButtonWidget			*playButton;
			IconButtonWidget			*stopButton;
			CheckWidget					*loopWidget;
			SliderWidget				*volumeWidget;
			ProgressWidget				*progressWidget;
			TextWidget					*timeWidget;

			LineWidget					*markLeft;
			LineWidget					*markRight;
			SoundWidget					*soundLeft;
			SoundWidget					*soundRight;

			static List<SoundWindow>	windowList;

			static void SoundComplete(Sound *sound, void *cookie);

			void UpdateSoundTime(int32 time);

		public:

			SoundWindow(const char *name);
			~SoundWindow();

			const char *GetResourceName(void) const
			{
				return (resourceName);
			}

			static SoundWindow *Open(const char *name);

			void Preprocess(void) override;
			void Move(void) override;

			bool HandleKeyboardEvent(const KeyboardEventData *eventData) override;
			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};
}


#endif

// ZYUQURM
