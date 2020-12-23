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


#ifndef MGConfiguration_h
#define MGConfiguration_h


#include "C4Input.h"
#include "C4Viewports.h"
#include "MGInterface.h"


namespace C4
{
	class SoldierController;


	enum : WidgetType
	{
		kWidgetInput		= 'inpt'
	};


	class CrosshairWidget final : public ImageWidget
	{
		private:

			int32				crosshairValue;

			VertexBuffer		vertexBuffer;
			List<Attribute>		attributeList;
			DiffuseAttribute	diffuseAttribute;
			Renderable			hiliteRenderable;

		public:

			CrosshairWidget(int32 value);
			~CrosshairWidget();

			int32 GetValue(void) const
			{
				return (crosshairValue);
			}

			void SetValue(int32 value);

			void Preprocess(void) override;
			void Build(void) override;
			void Render(List<Renderable> *renderList) override;

			void HandleMouseEvent(const PanelMouseEventData *eventData) override;
	};


	class PlayerSettingsWindow : public GameWindow, public Completable<PlayerSettingsWindow>, public Singleton<PlayerSettingsWindow>
	{
		private:

			PushButtonWidget		*okayButton;
			PushButtonWidget		*cancelButton;
			//PushButtonWidget		*customizeButton;

			EditTextWidget			*playerNameBox;

			CheckWidget				*weaponSwitchBox;
			CheckWidget				*betterWeaponBox;
			CheckWidget				*ammoSwitchBox;
			CheckWidget				*chatOpenBox;

			ColorWidget				*crossColorBox;
			SliderWidget			*crossSizeSlider;
			TextWidget				*crossSizeText;

			CrosshairWidget			*crosshairBox;

			PlayerSettingsWindow();

		public:

			~PlayerSettingsWindow();

			static void Open(void);

			void Preprocess(void) override;
			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};


	class CustomizeCharacterWindow : public GameWindow
	{
		private:

			int32					playerStyle[kPlayerStyleCount];

			PushButtonWidget		*okayButton;
			PushButtonWidget		*cancelButton;

			SliderWidget			*lightSlider;
			SliderWidget			*colorSlider;
			SliderWidget			*textureSlider;
			SliderWidget			*helmetSlider;

			TextWidget				*lightText;
			TextWidget				*colorText; 
			TextWidget				*textureText;
			TextWidget				*helmetText;
 
			FrustumViewportWidget	*modelViewport;
 
			float					cameraAzimuth;
			bool					trackFlag;
			Point3D					previousPosition; 

			World					*modelWorld; 
			Zone					*zoneNode; 
			Light					*lightNode;
			Model					*modelNode;
			SoldierController		*modelController;
 
			static void ViewportHandleMouseEvent(const MouseEventData *eventData, ViewportWidget *viewport, void *cookie);
			static void ViewportRender(List<Renderable> *renderList, ViewportWidget *viewport, void *cookie);

		public:

			CustomizeCharacterWindow();
			~CustomizeCharacterWindow();

			void Preprocess(void) override;
			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};


	class GraphicsSettingsWindow : public GameWindow, public Completable<GraphicsSettingsWindow>, public Singleton<GraphicsSettingsWindow>
	{
		private:

			PushButtonWidget		*okayButton;
			PushButtonWidget		*cancelButton;
			PushButtonWidget		*applyButton;

			ListWidget				*resolutionList;
			CheckWidget				*fullscreenBox;
			PopupMenuWidget			*refreshSyncMenu;
			PopupMenuWidget			*multisamplingMenu;
			SliderWidget			*brightnessSlider;
			TextWidget				*brightnessText;

			PopupMenuWidget			*lightDetailMenu;
			PopupMenuWidget			*textureDetailMenu;
			SliderWidget			*anisotropySlider;
			TextWidget				*anisotropyText;

			CheckWidget				*parallaxBox;
			CheckWidget				*horizonBox;
			CheckWidget				*ambientBumpBox;
			CheckWidget				*terrainBumpBox;
			CheckWidget				*terrainDetailBox;

			CheckWidget				*structureBox;
			CheckWidget				*motionBlurBox;
			CheckWidget				*distortionBox;
			CheckWidget				*glowBloomBox;
			CheckWidget				*occlusionBox;

			float					originalBrightness;

			GraphicsSettingsWindow();

			float GetBrightnessMultiplier(void) const;
			bool ChangeDisplayMode(void);

		public:

			~GraphicsSettingsWindow();

			static void Open(void);

			void Preprocess(void) override;
			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};


	class AudioSettingsWindow : public GameWindow, public Completable<AudioSettingsWindow>, public Singleton<AudioSettingsWindow>
	{
		private:

			int32					originalEffectsVolume;
			int32					originalMusicVolume;
			int32					originalVoiceVolume;

			PushButtonWidget		*okayButton;
			PushButtonWidget		*cancelButton;

			SliderWidget			*effectsSlider;
			TextWidget				*effectsText;
			SliderWidget			*musicSlider;
			TextWidget				*musicText;
			CheckWidget				*reverbBox;

			CheckWidget				*voiceReceiveBox;
			CheckWidget				*voiceSendBox;
			SliderWidget			*voiceSlider;
			TextWidget				*voiceText;

		public:

			AudioSettingsWindow();
			~AudioSettingsWindow();

			static void Open(void);

			void Preprocess(void) override;
			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};


	class InputWidget final : public RenderableWidget
	{
		private:

			enum
			{
				kInputBoxHilited		= 1 << 0,
				kInputBoxConfigActive	= 1 << 1
			};

			struct InputVertex
			{
				Point2D		position;
				ColorRGBA	color;
			};

			const ListWidget			*controlList;

			Action						*inputAction;
			InputControl				*inputControl;

			ColorRGBA					dynamicBackgroundColor;

			VertexBuffer				vertexBuffer;
			static SharedVertexBuffer	indexBuffer;

			PulsateMutator				pulsateMutator;
			TextWidget					textWidget;

		public:

			InputWidget(const ListWidget *list, Action *action);
			~InputWidget();

			Action *GetInputAction(void) const
			{
				return (inputAction);
			}

			InputControl *GetInputControl(void) const
			{
				return (inputControl);
			}

			void SetDynamicWidgetColor(const ColorRGBA& color, WidgetColorType type = kWidgetColorDefault) override;

			void Preprocess(void) override;
			void Build(void) override;
			void HandleMouseEvent(const PanelMouseEventData *eventData) override;

			void SetInputControl(InputControl *control, float value = 0.0F);

			void BeginConfig(void);
			void EndConfig(void);
	};


	class ControlSettingsWindow : public GameWindow, public Completable<ControlSettingsWindow>, public Singleton<ControlSettingsWindow>
	{
		private:

			InputWidget				*configWidget;

			PushButtonWidget		*okayButton;
			PushButtonWidget		*cancelButton;

			ListWidget				*controlList;

			SliderWidget			*mouseSlider;
			TextWidget				*mouseText;
			CheckWidget				*invertMouseBox;
			CheckWidget				*smoothMouseBox;

			SliderWidget			*controllerSlider;
			TextWidget				*controllerText;
			CheckWidget				*invertXAxisBox;
			CheckWidget				*invertYAxisBox;

			static void ConfigProc(InputControl *control, float value, void *cookie);

		public:

			ControlSettingsWindow();
			~ControlSettingsWindow();

			void SetConfigWidget(InputWidget *widget)
			{
				configWidget = widget;
			}

			static void Open(void);

			void Preprocess(void) override;

			bool HandleKeyboardEvent(const KeyboardEventData *eventData) override;
			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;

			void SelectControl(const InputWidget *inputWidget);
			void ClearDuplicateControls(const InputControl *control, const InputWidget *inputWidget);
	};


	extern PlayerSettingsWindow *ThePlayerSettingsWindow;
	extern GraphicsSettingsWindow *TheGraphicsSettingsWindow;
	extern AudioSettingsWindow *TheAudioSettingsWindow;
	extern ControlSettingsWindow *TheControlSettingsWindow;
}


#endif

// ZYUQURM
