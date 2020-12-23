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


#ifndef C4ModelViewer_h
#define C4ModelViewer_h


#include "C4Models.h"
#include "C4Animation.h"
#include "C4Viewports.h"
#include "C4EditorPlugins.h"


namespace C4
{
	enum
	{
		kModelToolOrbit,
		kModelToolLight,
		kModelToolHand,
		kModelToolGlass,
		kModelToolFree,
		kModelToolCount
	};


	enum
	{
		kModelDiagnosticDarkness,
		kModelDiagnosticWireframe,
		kModelDiagnosticNormals,
		kModelDiagnosticTangents,
		kModelDiagnosticSkeleton,
		kModelDiagnosticMotionBlur,
		kModelDiagnosticCount
	};


	enum
	{
		kCueMenuInsertCue,
		kCueMenuDeleteCue,
		kCueMenuDeleteAllCues,
		kCueMenuGetCueInfo,
		kCueMenuItemCount
	};


	enum
	{
		kModelViewerUnsaved			= 1 << 0,
		kModelViewerUpdateMenus		= 1 << 1
	};


	enum
	{
		kWidgetLimit				= 'LIMT',
		kWidgetCue					= 'CUE '
	};


	class InfiniteLight;
	class ConfigurationWidget;
	class ModelWindow;


	class LimitWidget final : public RenderableWidget
	{
		private:

			struct LimitVertex
			{
				Point2D		position;
				ColorRGBA	color;
				Point2D		texcoord;
			};

			int32				limitValue;
			int32				maxLimitValue;

			int32				minLimitPosition;
			int32				maxLimitPosition;

			float				dragPosition;

			VertexBuffer		vertexBuffer;

			int32 GetPositionValue(float x) const;

		public:

			LimitWidget(const Vector2D& size);
			~LimitWidget();

			int32 GetValue(void) const
			{
				return (limitValue);
			}

			int32 GetMaxValue(void) const
			{
				return (maxLimitValue);
			} 

			int32 GetMinLimitPosition(void) const
			{ 
				return (minLimitPosition);
			} 

			int32 GetMaxLimitPosition(void) const
			{ 
				return (maxLimitPosition);
			} 
 
			void SetLimitRange(int32 min, int32 max)
			{
				minLimitPosition = min;
				maxLimitPosition = max; 
			}

			void SetValue(int32 value);
			void SetMaxValue(int32 maxValue);

			WidgetPart TestPosition(const Point3D& position) const override;
			float GetIndicatorPosition(void) const;

			void Preprocess(void) override;
			void Build(void) override;

			void HandleMouseEvent(const PanelMouseEventData *eventData) override;
	};


	class CueWidget final : public TextWidget, public ListElement<CueWidget>
	{
		private:

			struct CueVertex
			{
				Point2D		position;
				ColorRGBA	color;
				Point2D		texcoord;
			};

			CueType				cueType;

			int32				cueValue;
			int32				maxCueValue;

			float				dragPosition;

			VertexBuffer		cueVertexBuffer;
			Renderable			cueRenderable;

			int32 GetPositionValue(float x) const;

		public:

			CueWidget();
			CueWidget(CueType type, int32 value, int32 maxValue);
			~CueWidget();

			using ListElement<CueWidget>::Previous;
			using ListElement<CueWidget>::Next;

			CueType GetCueType(void) const
			{
				return (cueType);
			}

			int32 GetValue(void) const
			{
				return (cueValue);
			}

			int32 GetMaxValue(void) const
			{
				return (maxCueValue);
			}

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void SetValue(int32 value);
			void SetMaxValue(int32 maxValue);

			WidgetPart TestPosition(const Point3D& position) const override;
			float GetIndicatorPosition(void) const;

			void Preprocess(void) override;
			void Build(void) override;
			void Render(List<Renderable> *renderList) override;

			void HandleMouseEvent(const PanelMouseEventData *eventData) override;
	};


	class CueInfoWindow : public Window
	{
		private:

			ModelWindow				*modelWindow;
			CueWidget				*cueWidget;

			PushButtonWidget		*okayButton;
			PushButtonWidget		*cancelButton;

			ConfigurationWidget		*configurationWidget;

		public:

			CueInfoWindow(ModelWindow *window, CueWidget *cue = nullptr);
			~CueInfoWindow();

			void Preprocess(void) override;
			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};


	class AnimationPicker : public SceneImportPicker
	{
		private:

			CheckWidget			*anchorXYBox;
			CheckWidget			*anchorZBox;
			CheckWidget			*freezeRootBox;
			CheckWidget			*preserveMissingBox;
			PopupMenuWidget		*rotationMenu;

		public:

			AnimationPicker(const char *subdir);
			~AnimationPicker();

			bool GetAnchorXYFlag(void) const
			{
				return (anchorXYBox->GetValue() != 0);
			}

			bool GetAnchorZFlag(void) const
			{
				return (anchorZBox->GetValue() != 0);
			}

			bool GetFreezeRootFlag(void) const
			{
				return (freezeRootBox->GetValue() != 0);
			}

			bool GetPreserveMissingFlag(void) const
			{
				return (preserveMissingBox->GetValue() != 0);
			}

			int32 GetRotationIndex(void) const
			{
				return (rotationMenu->GetSelection());
			}

			void Preprocess(void) override;
	};


	class ModelPage : public Page
	{
		private:

			ModelWindow		*modelWindow;

		protected:

			ModelPage(ModelWindow *window, const char *panelName);

		public:

			~ModelPage();

			ModelWindow *GetModelWindow(void) const
			{
				return (modelWindow);
			}
	};


	class ModelInfoPage : public ModelPage
	{
		public:

			ModelInfoPage(ModelWindow *window);
			~ModelInfoPage();

			void Preprocess(void) override;
	};


	class ModelAnimationPage : public ModelPage
	{
		private:

			IconButtonWidget		*playButton;
			IconButtonWidget		*stopButton;

			CheckWidget				*loopBox;
			CheckWidget				*oscillateBox;
			CheckWidget				*reverseBox;

			ListWidget				*animationList;
			PushButtonWidget		*importButton;

			WidgetObserver<ModelAnimationPage>		playButtonObserver;
			WidgetObserver<ModelAnimationPage>		stopButtonObserver;

			WidgetObserver<ModelAnimationPage>		loopBoxObserver;
			WidgetObserver<ModelAnimationPage>		oscillateBoxObserver;
			WidgetObserver<ModelAnimationPage>		reverseBoxObserver;

			WidgetObserver<ModelAnimationPage>		animationListObserver;
			WidgetObserver<ModelAnimationPage>		importButtonObserver;

			void BuildAnimationList(void);

			static void AnimationCompleteProc(Interpolator *interpolator, void *cookie);

			void HandlePlayButtonEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleStopButtonEvent(Widget *widget, const WidgetEventData *eventData);

			void HandleLoopBoxEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleOscillateBoxEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleReverseBoxEvent(Widget *widget, const WidgetEventData *eventData);

			void HandleAnimationListEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleImportButtonEvent(Widget *widget, const WidgetEventData *eventData);

		public:

			ModelAnimationPage(ModelWindow *window);
			~ModelAnimationPage();

			void EnablePlayButton(void)
			{
				playButton->Enable();
			}

			void StopAnimation(void)
			{
				stopButton->Activate();
			}

			const Widget *GetSelectedAnimation(void) const
			{
				return (animationList->GetFirstSelectedListItem());
			}

			void Preprocess(void) override;

			static void AnimationPickerProc(FilePicker *picker, void *cookie);
			void ReselectAnimation(const char *name);
	};


	class ModelDisplayPage : public ModelPage
	{
		private:

			TextWidget				*biasText;
			CheckWidget				*diagnosticBox[kModelDiagnosticCount];

			WidgetObserver<ModelDisplayPage>	biasSliderObserver;

			void HandleBiasSliderEvent(Widget *widget, const WidgetEventData *eventData);

		public:

			ModelDisplayPage(ModelWindow *window);
			~ModelDisplayPage();

			bool GetDiagnosticFlag(int32 index) const
			{
				return (diagnosticBox[index]->GetValue() != 0);
			}

			void Preprocess(void) override;
	};


	class ModelWindow : public Window, public ListElement<ModelWindow>
	{
		private:

			ResourceName					resourceName;
			ResourceName					animationName;
			ResourceLocation				resourceLocation;

			unsigned_int32					viewerState;

			Vector2D						viewportSize;
			FrustumViewportWidget			*modelViewport;
			BorderWidget					*viewportBorder;

			Widget							*frameLabel;
			Widget							*cuesLabel;
			Widget							*cuesLine;
			Widget							*cuesGroup;

			SliderWidget					*frameSlider;
			LimitWidget						*beginLimit;
			LimitWidget						*endLimit;
			TextWidget						*frameText;
			TextWidget						*beginText;
			TextWidget						*endText;

			IconButtonWidget				*toolButton[kModelToolCount];

			MenuBarWidget					*menuBar;
			PulldownMenuWidget				*modelMenu;
			PulldownMenuWidget				*cueMenu;
			MenuItemWidget					*saveAnimationItem;
			MenuItemWidget					*cueMenuItem[kCueMenuItemCount];

			BookWidget						*bookWidget;
			ModelInfoPage					*infoPage;
			ModelAnimationPage				*animationPage;
			ModelDisplayPage				*displayPage;

			List<CueWidget>					cueWidgetList;
			WidgetObserver<ModelWindow>		cueWidgetObserver;

			World							*environmentWorld;
			Zone							*zoneNode;
			InfiniteLight					*lightNode;
			Model							*modelNode;
			FrameAnimator					*frameAnimator;

			int32							currentTool;
			int32							trackTool;
			bool							toolTracking;
			Point3D							previousPosition;

			float							lightAzimuth;
			float							lightAltitude;

			float							freeCameraSpeed;
			unsigned_int32					freeCameraFlags;

			static List<ModelWindow>		windowList;

			void PositionWidgets(void);
			void PositionCueWidget(CueWidget *cueWidget);
			void UpdateLimitText(void);

			void BuildMenus(void);

			void HandleCloseMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleSaveAnimationMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleSaveModelMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleInsertCueMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleDeleteCueMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleDeleteAllCuesMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleGetCueInfoMenuItem(Widget *menuItem, const WidgetEventData *eventData);

			static void EnvironmentPickerProc(FilePicker *picker, void *cookie);
			void HandleSelectEnvironmentMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void SetEnvironmentWorld(const char *name);

			void HandleCueWidgetEvent(Widget *widget, const WidgetEventData *eventData);

			int32 GetActiveTool(void) const;
			void TrackGlassTool(ViewportWidget *viewport, float dy);

			static void ViewportHandleMouseEvent(const MouseEventData *eventData, ViewportWidget *viewport, void *cookie);
			static void ViewportTrackTask(const Point3D& position, ViewportWidget *viewport, void *cookie);
			static void ViewportRender(List<Renderable> *renderList, ViewportWidget *viewport, void *cookie);

			static void SelectConfirmDialogComplete(Dialog *dialog, void *cookie);
			static void ImportConfirmDialogComplete(Dialog *dialog, void *cookie);
			static void CloseConfirmDialogComplete(Dialog *dialog, void *cookie);

			void DisplayConfirmationDialog(Dialog::CompletionProc *proc);

		public:

			ModelWindow(const char *name, const ModelResource *resource, const ResourceLocation *location);
			~ModelWindow();

			static void PurgeWindowList(void)
			{
				windowList.Purge();
			}

			void SetMenuUpdateFlag(void)
			{
				viewerState |= kModelViewerUpdateMenus;
			}

			Model *GetModel(void) const
			{
				return (modelNode);
			}

			FrameAnimator *GetFrameAnimator(void) const
			{
				return (frameAnimator);
			}

			bool GetDiagnosticFlag(int32 index) const
			{
				return (displayPage->GetDiagnosticFlag(index));
			}

			static ResourceResult Open(const char *name);

			ResourcePath GetModelDirectory(void) const;

			void SelectAnimation(void);
			void ImportAnimation(void);
			void ActivateAnimation(void);
			int32 GetAnimationFrame(void) const;
			void AddCue(CueWidget *cueWidget);

			void SetWidgetSize(const Vector2D& size) override;

			void Preprocess(void) override;
			void Move(void) override;

			void EnterBackground(void) override;

			bool HandleKeyboardEvent(const KeyboardEventData *eventData) override;
			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
			void Close(void) override;
	};
}


#endif

// ZYUQURM
