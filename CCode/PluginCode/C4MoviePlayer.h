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


#ifndef C4MoviePlayer_h
#define C4MoviePlayer_h


#include "C4Movies.h"


namespace C4
{
	class MovieWindow : public Window, public ListElement<MovieWindow>
	{
		friend class MovieTool;

		private:

			ResourcePath				resourceName;
			Vector2D					movieSize;

			Movie						*movieObject;

			IconButtonWidget			*playButton;
			IconButtonWidget			*stopButton;
			CheckWidget					*loopWidget;
			SliderWidget				*sliderWidget;
			TextWidget					*timeWidget;

			static List<MovieWindow>	windowList;

			static void MovieComplete(Movie *movie, void *cookie);

			void UpdateMovieTime(MovieTime time);

		public:

			MovieWindow(const char *name, const Vector2D& size);
			~MovieWindow();

			static MovieResult Open(const char *name);

			void Preprocess(void) override;
			void Move(void) override;

			bool HandleKeyboardEvent(const KeyboardEventData *eventData) override;
			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};
}


#endif

// ZYUQURM
