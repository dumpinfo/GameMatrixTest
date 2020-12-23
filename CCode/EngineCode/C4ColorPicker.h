 

#ifndef C4ColorPicker_h
#define C4ColorPicker_h


//# \component	Interface Manager
//# \prefix		InterfaceMgr/


#include "C4Interface.h"


namespace C4
{
	//# \enum	ColorPickerFlags

	enum
	{
		kColorPickerAlpha	= 1 << 0		//## Allow the alpha value to be changed.
	};


	//# \class	ColorPicker		Encapsulates a color picker box.
	//
	//# A $ColorPicker$ class encapsulates a color picker box.
	//
	//# \def	class ColorPicker : public Window, public LinkTarget<ColorPicker>, public Completable<ColorPicker>
	//
	//# \ctor	ColorPicker(const char *title, const ColorRGBA& color, unsigned_int32 flags = 0);
	//
	//# \param	title		The color picker's displayed title.
	//# \param	color		The initial color that is shown in the color picker.
	//# \param	flags		Flags that affect the appearance and behavior of the color picker. See below for a list of possible values.
	//
	//# \desc
	//# The $ColorPicker$ class provides a standard interface for selecting colors. When the user selects a color,
	//# the color picker's completion procedure is called. The completion procedure can then retrieve the new color
	//# by calling the $@ColorPicker::GetColor@$ function.
	//#
	//# Options that control the appearance and behavior of the color picker can be controlled by specifying a
	//# combination (through logical OR) of the following constants in the $flags$ parameter.
	//
	//# \table	ColorPickerFlags
	//
	//# If the color picker is cancelled, then the completion procedure is not called.
	//
	//# \base	Window								The $ColorPicker$ class is a specific type of window.
	//# \base	Utilities/LinkTarget<ColorPicker>	Color pickers support smart linking with the $@Utilities/Link@$ class.
	//# \base	Utilities/Completable<ColorPicker>	The completion procedure is called when the color picker is dismissed.


	//# \function	ColorPicker::GetColor		Returns the color that is currently displayed.
	//
	//# \proto	const ColorRGBA& GetColor(void) const;
	//
	//# \desc
	//# The $GetColor$ function returns the color that is currently displayed in a color picker box.


	class ColorPicker : public Window, public LinkTarget<ColorPicker>, public Completable<ColorPicker>
	{
		private:

			ColorRGBA						originalColor;
			ColorRGBA						currentColor;
			ColorRGBA						brightColor;

			unsigned_int32					colorPickerFlags;

			PushButtonWidget				*okayButton;
			PushButtonWidget				*cancelButton;
			PushButtonWidget				*resetButton;

			QuadWidget						*originalColorBox;
			QuadWidget						*newColorBox;

			SliderWidget					*redSlider;
			SliderWidget					*greenSlider;
			SliderWidget					*blueSlider;
			SliderWidget					*alphaSlider;
			SliderWidget					*brightnessSlider;

			QuadWidget						*redGradient;
			QuadWidget						*greenGradient;
			QuadWidget						*blueGradient;
			QuadWidget						*alphaGradient;
			QuadWidget						*brightnessGradient;

			EditTextWidget					*redBox;
			EditTextWidget					*greenBox;
			EditTextWidget					*blueBox;
			EditTextWidget					*alphaBox;

			WidgetObserver<ColorPicker>		channelTextObserver;
			WidgetObserver<ColorPicker>		channelSliderObserver;
			WidgetObserver<ColorPicker>		brightnessSliderObserver;

			void UpdateColor(void);
			void UpdateBrightness(void);
			void UpdateGradients(void);

			void HandleChannelTextEvent(Widget *widget, const WidgetEventData *eventData); 
			void HandleChannelSliderEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleBrightnessSliderEvent(Widget *widget, const WidgetEventData *eventData);
 
		public:
 
			C4API ColorPicker(const char *title, const ColorRGBA& color, unsigned_int32 flags = 0);
			C4API ~ColorPicker();
 
			const ColorRGBA& GetColor(void) const
			{ 
				return (currentColor); 
			}

			void Preprocess(void) override;
			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override; 
	};
}


#endif

// ZYUQURM
