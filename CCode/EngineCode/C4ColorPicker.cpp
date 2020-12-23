 

#include "C4ColorPicker.h"


using namespace C4;


ColorPicker::ColorPicker(const char *title, const ColorRGBA& color, unsigned_int32 flags) :
		Window((flags & kColorPickerAlpha) ? "C4/ColorRGBA" : "C4/ColorRGB"),
		channelTextObserver(this, &ColorPicker::HandleChannelTextEvent),
		channelSliderObserver(this, &ColorPicker::HandleChannelSliderEvent),
		brightnessSliderObserver(this, &ColorPicker::HandleBrightnessSliderEvent)
{
	SetWindowTitle(title);

	originalColor = color;
	currentColor = color;
	brightColor = color;

	colorPickerFlags = flags;
}

ColorPicker::~ColorPicker()
{
}

void ColorPicker::Preprocess(void)
{
	Window::Preprocess();

	okayButton = static_cast<PushButtonWidget *>(FindWidget("OK"));
	cancelButton = static_cast<PushButtonWidget *>(FindWidget("Cancel"));
	resetButton = static_cast<PushButtonWidget *>(FindWidget("Reset"));

	originalColorBox = static_cast<QuadWidget *>(FindWidget("Original"));
	newColorBox = static_cast<QuadWidget *>(FindWidget("New"));

	redSlider = static_cast<SliderWidget *>(FindWidget("RedSlider"));
	greenSlider = static_cast<SliderWidget *>(FindWidget("GreenSlider"));
	blueSlider = static_cast<SliderWidget *>(FindWidget("BlueSlider"));
	brightnessSlider = static_cast<SliderWidget *>(FindWidget("BrightSlider"));

	redGradient = static_cast<QuadWidget *>(FindWidget("RedBar"));
	greenGradient = static_cast<QuadWidget *>(FindWidget("GreenBar"));
	blueGradient = static_cast<QuadWidget *>(FindWidget("BlueBar"));
	brightnessGradient = static_cast<QuadWidget *>(FindWidget("BrightBar"));

	redBox = static_cast<EditTextWidget *>(FindWidget("Red"));
	greenBox = static_cast<EditTextWidget *>(FindWidget("Green"));
	blueBox = static_cast<EditTextWidget *>(FindWidget("Blue"));

	originalColorBox->SetWidgetColor(ColorRGBA(currentColor.GetColorRGB(), 1.0F));
	newColorBox->SetWidgetColor(ColorRGBA(currentColor.GetColorRGB(), 1.0F));

	redBox->SetObserver(&channelTextObserver);
	greenBox->SetObserver(&channelTextObserver);
	blueBox->SetObserver(&channelTextObserver);

	redSlider->SetObserver(&channelSliderObserver);
	greenSlider->SetObserver(&channelSliderObserver);
	blueSlider->SetObserver(&channelSliderObserver);
	brightnessSlider->SetObserver(&brightnessSliderObserver);

	int32 red = (int32) (currentColor.red * 255.0F + 0.5F);
	redSlider->SetValue(red);
	redBox->SetText(String<3>(red));

	int32 green = (int32) (currentColor.green * 255.0F + 0.5F);
	greenSlider->SetValue(green);
	greenBox->SetText(String<3>(green));

	int32 blue = (int32) (currentColor.blue * 255.0F + 0.5F);
	blueSlider->SetValue(blue);
	blueBox->SetText(String<3>(blue));

	brightnessSlider->SetValue(Max(Max(red, green), blue));
	brightnessGradient->SetVertexColor(0, ColorRGBA(0.0F, 0.0F, 0.0F, 1.0F));
	brightnessGradient->SetVertexColor(1, ColorRGBA(0.0F, 0.0F, 0.0F, 1.0F));

	if (colorPickerFlags & kColorPickerAlpha)
	{
		alphaSlider = static_cast<SliderWidget *>(FindWidget("AlphaSlider"));
		alphaGradient = static_cast<QuadWidget *>(FindWidget("AlphaBar"));
		alphaBox = static_cast<EditTextWidget *>(FindWidget("Alpha"));

		alphaBox->SetObserver(&channelTextObserver);
		alphaSlider->SetObserver(&channelSliderObserver);

		int32 alpha = (int32) (currentColor.alpha * 255.0F + 0.5F);
		alphaSlider->SetValue(alpha);
		alphaBox->SetText(String<3>(alpha));

		alphaGradient->SetVertexColor(0, ColorRGBA(0.0F, 0.0F, 0.0F, 1.0F));
		alphaGradient->SetVertexColor(1, ColorRGBA(0.0F, 0.0F, 0.0F, 1.0F));
	}

	SetFocusWidget(redBox);
	UpdateGradients();
}

void ColorPicker::UpdateColor(void)
{ 
	newColorBox->SetWidgetColor(ColorRGBA(currentColor.GetColorRGB(), 1.0F));
}
 
void ColorPicker::UpdateBrightness(void)
{ 
	int32 m = Max(Max(redSlider->GetValue(), greenSlider->GetValue()), blueSlider->GetValue());
	if (m != 0)
	{ 
		brightColor.Set(currentColor.GetColorRGB() * (255.0F / (float) m), currentColor.alpha);
	} 
 
	brightnessSlider->SetValue(m);
	UpdateGradients();
}
 
void ColorPicker::UpdateGradients(void)
{
	float r = currentColor.red;
	float g = currentColor.green;
	float b = currentColor.blue;

	redGradient->SetVertexColor(0, ColorRGBA(0.0F, g, b, 1.0F));
	redGradient->SetVertexColor(1, ColorRGBA(0.0F, g, b, 1.0F));
	redGradient->SetVertexColor(2, ColorRGBA(1.0F, g, b, 1.0F));
	redGradient->SetVertexColor(3, ColorRGBA(1.0F, g, b, 1.0F));

	greenGradient->SetVertexColor(0, ColorRGBA(r, 0.0F, b, 1.0F));
	greenGradient->SetVertexColor(1, ColorRGBA(r, 0.0F, b, 1.0F));
	greenGradient->SetVertexColor(2, ColorRGBA(r, 1.0F, b, 1.0F));
	greenGradient->SetVertexColor(3, ColorRGBA(r, 1.0F, b, 1.0F));

	blueGradient->SetVertexColor(0, ColorRGBA(r, g, 0.0F, 1.0F));
	blueGradient->SetVertexColor(1, ColorRGBA(r, g, 0.0F, 1.0F));
	blueGradient->SetVertexColor(2, ColorRGBA(r, g, 1.0F, 1.0F));
	blueGradient->SetVertexColor(3, ColorRGBA(r, g, 1.0F, 1.0F));

	r = brightColor.red;
	g = brightColor.green;
	b = brightColor.blue;

	float f = Fmax(r, g, b);
	if (f > K::one_over_256)
	{
		f = 1.0F / f;
		r *= f;
		g *= f;
		b *= f;
	}
	else
	{
		r = 1.0F;
		g = 1.0F;
		b = 1.0F;
	}

	brightnessGradient->SetVertexColor(2, ColorRGBA(r, g, b, 1.0F));
	brightnessGradient->SetVertexColor(3, ColorRGBA(r, g, b, 1.0F));
}

void ColorPicker::HandleChannelTextEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		if (widget == redBox)
		{
			int32 red = Text::StringToInteger(redBox->GetText());
			if (red > 255)
			{
				redBox->SetText("255");
				red = 255;
			}

			if (redSlider->GetValue() != red)
			{
				currentColor.red = (float) red * K::one_over_255;
				redSlider->SetValue(red);
				UpdateColor();
			}
		}
		else if (widget == greenBox)
		{
			int32 green = Text::StringToInteger(greenBox->GetText());
			if (green > 255)
			{
				greenBox->SetText("255");
				green = 255;
			}

			if (greenSlider->GetValue() != green)
			{
				currentColor.green = (float) green * K::one_over_255;
				greenSlider->SetValue(green);
				UpdateColor();
			}
		}
		else if (widget == blueBox)
		{
			int32 blue = Text::StringToInteger(blueBox->GetText());
			if (blue > 255)
			{
				blueBox->SetText("255");
				blue = 255;
			}

			if (blueSlider->GetValue() != blue)
			{
				currentColor.blue = (float) blue * K::one_over_255;
				blueSlider->SetValue(blue);
				UpdateColor();
			}
		}
		else if ((colorPickerFlags & kColorPickerAlpha) && (widget == alphaBox))
		{
			int32 alpha = Text::StringToInteger(alphaBox->GetText());
			if (alpha > 255)
			{
				alphaBox->SetText("255");
				alpha = 255;
			}

			if (alphaSlider->GetValue() != alpha)
			{
				currentColor.alpha = (float) alpha * K::one_over_255;
				alphaSlider->SetValue(alpha);
				UpdateColor();
			}
		}

		UpdateBrightness();
	}
}

void ColorPicker::HandleChannelSliderEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		SliderWidget *slider = static_cast<SliderWidget *>(widget);

		int32 v = slider->GetValue();
		float f = (float) v * K::one_over_255;

		if (slider == redSlider)
		{
			currentColor.red = f;
			redBox->SetText(String<3>(v));
		}
		else if (slider == greenSlider)
		{
			currentColor.green = f;
			greenBox->SetText(String<3>(v));
		}
		else if (slider == blueSlider)
		{
			currentColor.blue = f;
			blueBox->SetText(String<3>(v));
		}
		else if (slider == alphaSlider)
		{
			currentColor.alpha = f;
			alphaBox->SetText(String<3>(v));
		}

		UpdateColor();
		UpdateBrightness();
	}
}

void ColorPicker::HandleBrightnessSliderEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		SliderWidget *slider = static_cast<SliderWidget *>(widget);

		int32 v = slider->GetValue();
		float f = (float) v * K::one_over_255;

		float red = brightColor.red;
		float green = brightColor.green;
		float blue = brightColor.blue;

		if ((red >= green) && (red >= blue))
		{
			currentColor.red = f;
			redSlider->SetValue(v);
			redBox->SetText(String<3>(v));

			float g = f;
			float b = f;
			if (red > 0.0F)
			{
				g = f * green / red;
				b = f * blue / red;
			}

			v = (int32) (g * 255.0F);
			currentColor.green = g;
			greenSlider->SetValue(v);
			greenBox->SetText(String<3>(v));

			v = (int32) (b * 255.0F);
			currentColor.blue = b;
			blueSlider->SetValue(v);
			blueBox->SetText(String<3>(v));
		}
		else if (green >= blue)
		{
			currentColor.green = f;
			greenSlider->SetValue(v);
			greenBox->SetText(String<3>(v));

			float r = f;
			float b = f;
			if (green > 0.0F)
			{
				r = f * red / green;
				b = f * blue / green;
			}

			v = (int32) (r * 255.0F);
			currentColor.red = r;
			redSlider->SetValue(v);
			redBox->SetText(String<3>(v));

			v = (int32) (b * 255.0F);
			currentColor.blue = b;
			blueSlider->SetValue(v);
			blueBox->SetText(String<3>(v));
		}
		else
		{
			currentColor.blue = f;
			blueSlider->SetValue(v);
			blueBox->SetText(String<3>(v));

			float r = f;
			float g = f;
			if (blue > 0.0F)
			{
				r = f * red / blue;
				g = f * green / blue;
			}

			v = (int32) (r * 255.0F);
			currentColor.red = r;
			redSlider->SetValue(v);
			redBox->SetText(String<3>(v));

			v = (int32) (g * 255.0F);
			currentColor.green = g;
			greenSlider->SetValue(v);
			greenBox->SetText(String<3>(v));
		}

		UpdateColor();
		UpdateGradients();
	}
}

void ColorPicker::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		if (widget == okayButton)
		{
			CallCompletionProc();
			Close();
		}
		else if (widget == cancelButton)
		{
			Close();
		}
		else if (widget == resetButton)
		{
			currentColor = originalColor;

			int32 red = (int32) (currentColor.red * 255.0F);
			redSlider->SetValue(red);
			redBox->SetText(String<3>(red));

			int32 green = (int32) (currentColor.green * 255.0F);
			greenSlider->SetValue(green);
			greenBox->SetText(String<3>(green));

			int32 blue = (int32) (currentColor.blue * 255.0F);
			blueSlider->SetValue(blue);
			blueBox->SetText(String<3>(blue));

			if (colorPickerFlags & kColorPickerAlpha)
			{
				int32 alpha = (int32) (currentColor.alpha * 255.0F);
				alphaSlider->SetValue(alpha);
				alphaBox->SetText(String<3>(alpha));
			}

			UpdateColor();
			UpdateBrightness();
		}
	}
}

// ZYUQURM
