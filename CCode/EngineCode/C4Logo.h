 

#ifndef C4Logo_h
#define C4Logo_h


#include "C4Movies.h"


namespace C4
{
	extern const char LogoImage[];


	class Sound;


	class LogoWindow : public Window, public Completable<LogoWindow>
	{
		private:

			MovieWidget			*logoWidget;

			bool				completeFlag;
			ColorRGBA			desktopColor;

			static void MovieComplete(Movie *movie, void *cookie);

		public:

			C4API LogoWindow();
			C4API ~LogoWindow();

			void Preprocess(void) override;
			void Move(void) override;

			bool HandleKeyboardEvent(const KeyboardEventData *eventData) override;
			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
			void Close(void) override;
	};
}


#endif

// ZYUQURM
