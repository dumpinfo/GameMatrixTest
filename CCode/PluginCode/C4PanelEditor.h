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


#ifndef C4PanelEditor_h
#define C4PanelEditor_h


#include "C4Viewports.h"
#include "C4Panels.h"


namespace C4
{
	enum
	{
		kPanelEditorModeTool,
		kPanelEditorModeWidget,
		kPanelEditorModeCount
	};


	enum
	{
		kPanelEditorShowGridlines		= 1 << 0,
		kPanelEditorSnapToGrid			= 1 << 1
	};


	enum
	{
		kPanelToolWidgetSelect,
		kPanelToolWidgetMove,
		kPanelToolWidgetRotate,
		kPanelToolWidgetResize,
		kPanelToolTextureOffset,
		kPanelToolTextureScale,
		kPanelToolViewportScroll,
		kPanelToolViewportZoom,
		kPanelToolCount
	};


	enum
	{
		kPanelMenuUndo,
		kPanelMenuCut,
		kPanelMenuCopy,
		kPanelMenuPaste,
		kPanelMenuClear,
		kPanelMenuDuplicate,

		kPanelMenuBringToFront,
		kPanelMenuBringForward,
		kPanelMenuSendBackward,
		kPanelMenuSendToBack,
		kPanelMenuHideSelection,
		kPanelMenuLockSelection,
		kPanelMenuGroupSelection,
		kPanelMenuUngroupSelection,
		kPanelMenuResetRotation,
		kPanelMenuResetTexcoords,
		kPanelMenuAutoScaleTexture,
		kPanelMenuEditScript,
		kPanelMenuDeleteScript,

		kPanelMenuNudgeLeft,
		kPanelMenuNudgeRight,
		kPanelMenuNudgeUp,
		kPanelMenuNudgeDown,
		kPanelMenuAlignLeftSides,
		kPanelMenuAlignRightSides,
		kPanelMenuAlignTopSides,
		kPanelMenuAlignBottomSides,
		kPanelMenuAlignHorizontalCenters,
		kPanelMenuAlignVerticalCenters,

		kPanelMenuItemCount
	};


	enum
	{
		kWidgetManipulatorTempSelected		= kWidgetManipulatorBaseState << 0,
		kWidgetManipulatorLocked			= kWidgetManipulatorBaseState << 1,
		kWidgetManipulatorBuild				= kWidgetManipulatorBaseState << 2
	};


	enum
	{
		kPanelEditorUnsaved					= 1 << 0,
		kPanelEditorUndoPending				= 1 << 1,
		kPanelEditorNudgeCombine			= 1 << 2,
		kPanelEditorUpdateMenus				= 1 << 3,
		kPanelEditorUpdateSettings			= 1 << 4,
		kPanelEditorUpdateTransformPage		= 1 << 5,
		kPanelEditorUpdateGrid				= 1 << 6
	};


	class PanelEditor;
	class ScriptEditor;
	class Dialog;
 

	class EditorWidgetManipulator : public WidgetManipulator, public ListElement<EditorWidgetManipulator>
	{ 
		private:
 
			struct WidgetVertex
			{
				Point2D		position; 
				Point2D		texcoord;
			}; 
 
			Transform4D					originalTransform;
			Vector2D					originalOffset;
			Vector2D					originalScale;
			Vector2D					originalSize; 

			VertexBuffer				widgetVertexBuffer;
			List<Attribute>				widgetAttributeList;
			DiffuseAttribute			widgetColorAttribute;
			DiffuseTextureAttribute		widgetTextureAttribute;
			Renderable					widgetRenderable;

			void BuildHandle(volatile WidgetVertex *vertex, float x, float y, float scale);

		public:

			EditorWidgetManipulator(Widget *widget);
			~EditorWidgetManipulator();

			const Transform4D& GetOriginalTransform(void) const
			{
				return (originalTransform);
			}

			void SaveOriginalTransform(void)
			{
				originalTransform = GetTargetWidget()->GetWidgetTransform();
			}

			const Point3D& GetOriginalPosition(void) const
			{
				return (originalTransform.GetTranslation());
			}

			void SaveOriginalPosition(void)
			{
				originalTransform.SetTranslation(GetTargetWidget()->GetWidgetPosition());
			}

			const Vector2D& GetOriginalOffset(void) const
			{
				return (originalOffset);
			}

			void SaveOriginalOffset(void)
			{
				originalOffset = static_cast<ImageWidget *>(GetTargetWidget())->GetImageOffset();
			}

			const Vector2D& GetOriginalScale(void) const
			{
				return (originalScale);
			}

			void SaveOriginalImageScale(void)
			{
				originalScale = static_cast<ImageWidget *>(GetTargetWidget())->GetImageScale();
			}

			void SaveOriginalTextScale(void)
			{
				originalScale.y = static_cast<TextWidget *>(GetTargetWidget())->GetTextScale();
			}

			const Vector2D& GetOriginalSize(void) const
			{
				return (originalSize);
			}

			void SaveOriginalSize(void)
			{
				originalSize = GetTargetWidget()->GetWidgetSize();
			}

			Renderable *GetWidgetRenderable(void)
			{
				return (&widgetRenderable);
			}

			void Invalidate(void) override;

			Box2D GetWidgetBounds(void) const;
			void GetHandlePositions(Point2D *handle) const;

			void Build(float scale);

			static void Install(Widget *root, bool recursive = true);
	};


	class WidgetReference : public ListElement<WidgetReference>
	{
		private:

			Widget		*reference;

		public:

			WidgetReference(Widget *widget)
			{
				reference = widget;
			}

			Widget *GetWidget(void) const
			{
				return (reference);
			}
	};


	class PanelOperation : public ListElement<PanelOperation>
	{
		protected:

			PanelOperation();

		public:

			virtual ~PanelOperation();

			virtual void Restore(PanelEditor *panelEditor) = 0;
	};


	class CreatePanelOperation : public PanelOperation
	{
		private:

			List<WidgetReference>	createList;

		public:

			CreatePanelOperation(Widget *widget);
			CreatePanelOperation(const List<EditorWidgetManipulator> *selectionList);
			~CreatePanelOperation();

			void Restore(PanelEditor *panelEditor);
	};


	class DeletePanelOperation : public PanelOperation
	{
		private:

			class DeleteReference : public WidgetReference
			{
				private:

					Widget		*superWidget;
					Widget		*precedingWidget;

				public:

					DeleteReference(Widget *widget);

					Widget *GetSuperWidget(void) const
					{
						return (superWidget);
					}

					Widget *GetPrecedingWidget(void) const
					{
						return (precedingWidget);
					}
			};

			List<WidgetReference>	deleteList;

		public:

			DeletePanelOperation(Widget *rootWidget);
			~DeletePanelOperation();

			void Restore(PanelEditor *panelEditor);
	};


	class TransformPanelOperation : public PanelOperation
	{
		private:

			class TransformReference : public WidgetReference
			{
				private:

					Transform4D		widgetTransform;
					Vector2D		widgetSize;

				public:

					TransformReference(Widget *widget);

					const Transform4D& GetTransform(void) const
					{
						return (widgetTransform);
					}

					const Vector2D& GetSize(void) const
					{
						return (widgetSize);
					}
			};

			List<WidgetReference>	transformList;

		public:

			TransformPanelOperation(const List<EditorWidgetManipulator> *selectionList);
			~TransformPanelOperation();

			void Restore(PanelEditor *panelEditor);
	};


	class WarpPanelOperation : public PanelOperation
	{
		private:

			class WarpReference : public WidgetReference
			{
				private:

					Vector2D	imageOffset;
					Vector2D	imageScale;

				public:

					WarpReference(ImageWidget *widget);

					const Vector2D& GetOffset(void) const
					{
						return (imageOffset);
					}

					const Vector2D& GetScale(void) const
					{
						return (imageScale);
					}
			};

			class ScaleReference : public WidgetReference
			{
				private:

					float		textScale;

				public:

					ScaleReference(TextWidget *widget);

					float GetScale(void) const
					{
						return (textScale);
					}
			};

			List<WidgetReference>	warpList;
			List<WidgetReference>	scaleList;

		public:

			WarpPanelOperation(const List<EditorWidgetManipulator> *selectionList);
			~WarpPanelOperation();

			void Restore(PanelEditor *panelEditor);
	};


	class ArrangePanelOperation : public PanelOperation
	{
		private:

			class ArrangeReference : public WidgetReference
			{
				private:

					Widget		*precedingWidget;

				public:

					ArrangeReference(Widget *widget);

					Widget *GetPrecedingWidget(void) const
					{
						return (precedingWidget);
					}
			};

			List<WidgetReference>	arrangeList;

		public:

			ArrangePanelOperation(Widget *rootWidget);
			~ArrangePanelOperation();

			void Restore(PanelEditor *panelEditor);
	};


	class GroupPanelOperation : public PanelOperation
	{
		private:

			class GroupReference : public WidgetReference
			{
				private:

					Widget			*precedingWidget;
					Transform4D		widgetTransform;

				public:

					GroupReference(Widget *widget);

					Widget *GetPrecedingWidget(void) const
					{
						return (precedingWidget);
					}

					const Transform4D& GetTransform(void) const
					{
						return (widgetTransform);
					}
			};

			Widget					*groupWidget;
			List<WidgetReference>	groupList;

		public:

			GroupPanelOperation(Widget *group, const Widget *rootWidget);
			~GroupPanelOperation();

			void Restore(PanelEditor *panelEditor);
	};


	class UngroupPanelOperation : public PanelOperation
	{
		private:

			class TransformReference : public WidgetReference
			{
				private:

					Transform4D		widgetTransform;

				public:

					TransformReference(Widget *widget);

					const Transform4D& GetTransform(void) const
					{
						return (widgetTransform);
					}
			};

			class UngroupReference : public WidgetReference
			{
				private:

					Widget					*precedingWidget;
					List<WidgetReference>	subnodeList;

				public:

					UngroupReference(Widget *widget);

					Widget *GetPrecedingWidget(void) const
					{
						return (precedingWidget);
					}

					const WidgetReference *GetFirstSubnodeReference(void) const
					{
						return (subnodeList.First());
					}
			};

			List<WidgetReference>	ungroupList;

		public:

			UngroupPanelOperation(const List<EditorWidgetManipulator> *selectionList);
			~UngroupPanelOperation();

			void Restore(PanelEditor *panelEditor);
	};


	class TexturePanelOperation : public PanelOperation
	{
		private:

			class TextureReference : public WidgetReference
			{
				private:

					ResourceName	textureName;

				public:

					TextureReference(ImageWidget *widget);

					const char *GetTextureName(void) const
					{
						return (textureName);
					}
			};

			List<WidgetReference>	textureList;

		public:

			TexturePanelOperation(const List<EditorWidgetManipulator> *selectionList);
			~TexturePanelOperation();

			void Restore(PanelEditor *panelEditor);
	};


	class ScriptPanelOperation : public PanelOperation
	{
		private:

			class ScriptReference : public WidgetReference
			{
				private:

					ScriptObject	*scriptObject;

				public:

					ScriptReference(Widget *widget);
					~ScriptReference();

					ScriptObject *GetScriptObject(void) const
					{
						return (scriptObject);
					}
			};

			List<WidgetReference>	scriptList;

		public:

			ScriptPanelOperation(const List<EditorWidgetManipulator> *selectionList);
			~ScriptPanelOperation();

			void Restore(PanelEditor *panelEditor);
	};


	class PanelPage : public Page
	{
		private:

			PanelEditor		*panelEditor;

		protected:

			PanelPage(PanelEditor *editor, const char *panelName);

		public:

			~PanelPage();

			PanelEditor *GetPanelEditor(void) const
			{
				return (panelEditor);
			}
	};


	class PanelWidgetsPage : public PanelPage
	{
		private:

			class ToolWidget final : public TextWidget
			{
				private:

					ImageWidget		imageWidget;

				public:

					const WidgetRegistration	*widgetRegistration;

					ToolWidget(const Vector2D& size, const WidgetRegistration *registration);
					~ToolWidget();
			};

			ListWidget							*listWidget;
			WidgetObserver<PanelWidgetsPage>	listWidgetObserver;

			void HandleListWidgetEvent(Widget *widget, const WidgetEventData *eventData);

		public:

			PanelWidgetsPage(PanelEditor *editor);
			~PanelWidgetsPage();

			void Preprocess(void) override;

			void UnselectWidgetTool(void);
	};


	class PanelTransformPage : public PanelPage
	{
		private:

			bool								operationFlag;

			EditTextWidget						*xpositionText;
			EditTextWidget						*ypositionText;
			EditTextWidget						*rotationText;
			EditTextWidget						*xsizeText;
			EditTextWidget						*ysizeText;

			WidgetObserver<PanelTransformPage>	positionTextObserver;
			WidgetObserver<PanelTransformPage>	rotationTextObserver;
			WidgetObserver<PanelTransformPage>	sizeTextObserver;

			static String<15> FloatToString(float num);

			void HandlePositionTextEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleRotationTextEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleSizeTextEvent(Widget *widget, const WidgetEventData *eventData);

		public:

			PanelTransformPage(PanelEditor *editor);
			~PanelTransformPage();

			void Preprocess(void) override;

			void UpdateTransform(Widget *widget);
	};


	class PanelGridPage : public PanelPage
	{
		private:

			IconButtonWidget				*gridButton;
			IconButtonWidget				*snapButton;

			WidgetObserver<PanelGridPage>	gridButtonObserver;
			WidgetObserver<PanelGridPage>	snapButtonObserver;

			void HandleGridButtonEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleSnapButtonEvent(Widget *widget, const WidgetEventData *eventData);

		public:

			PanelGridPage(PanelEditor *editor);
			~PanelGridPage();

			void Preprocess(void) override;
	};


	class WindowSettingsWindow : public Window, public LinkTarget<WindowSettingsWindow>
	{
		private:

			PanelEditor				*panelEditor;

			PushButtonWidget		*okayButton;
			PushButtonWidget		*cancelButton;

			ConfigurationWidget		*configurationWidget;

		public:

			WindowSettingsWindow(PanelEditor *editor);
			~WindowSettingsWindow();

			void Preprocess(void) override;
			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};


	class PanelEditor : public Window, public ListElement<PanelEditor>
	{
		private:

			ResourceName						resourceName;
			ResourceLocation					resourceLocation;

			unsigned_int32						editorFlags;
			unsigned_int32						editorState;

			RootWidget							*rootWidget;
			const PanelEffect					*targetNode;
			Panel								*targetPanel;

			int32								currentMode;
			int32								currentTool;
			const WidgetRegistration			*currentWidgetReg;

			List<EditorWidgetManipulator>		selectionList;
			Widget								*pickedWidget;
			Widget								*creationWidget;

			Mutator								*currentMutator;
			Array<const MutatorRegistration *>	validMutatorArray;

			List<PanelOperation>				operationList;

			int32								trackingMode;
			int32								trackingTool;
			bool								toolTracking;
			bool								boxSelectFlag;
			int32								handleIndex;

			Point3D								previousPoint;
			Vector2D							previousSize;
			float								previousAngle;
			Point3D								previousPosition;
			Point3D								anchorPoint;
			Point3D								anchorPosition;
			Point3D								rotationCenter;

			OrthoViewportWidget					*panelViewport;
			BorderWidget						*viewportBorder;

			IconButtonWidget					*toolButton[kPanelToolCount];

			Widget								*settingsGroup;
			TextWidget							*textStringText;
			EditTextWidget						*textStringBox;

			ConfigurationWidget					*widgetConfigurationWidget;
			ConfigurationWidget					*mutatorConfigurationWidget;

			PopupMenuWidget						*mutatorMenu;
			ListWidget							*mutatorListWidget;
			PushButtonWidget					*addMutatorButton;
			PushButtonWidget					*removeMutatorButton;

			MenuBarWidget						*menuBar;
			PulldownMenuWidget					*panelMenu;
			PulldownMenuWidget					*editMenu;
			PulldownMenuWidget					*widgetMenu;
			PulldownMenuWidget					*arrangeMenu;
			MenuItemWidget						*widgetMenuItem[kPanelMenuItemCount];

			BookWidget							*bookWidget;
			PanelWidgetsPage					*widgetsPage;
			PanelTransformPage					*transformPage;
			PanelGridPage						*gridPage;

			WidgetObserver<PanelEditor>			toolButtonObserver;
			WidgetObserver<PanelEditor>			textBoxObserver;
			ConfigurationObserver<PanelEditor>	widgetConfigurationObserver;
			ConfigurationObserver<PanelEditor>	mutatorConfigurationObserver;

			Grid								viewportGrid;
			DragRect							dragRect;

			static Widget						editorClipboard;
			static List<PanelEditor>			windowList;

			float SnapToGrid(float value) const
			{
				float spacing = viewportGrid.GetGridLineSpacing();
				return (Floor(value / spacing + 0.5F) * spacing);
			}

			void PositionWidgets(void);
			void BuildMenus(void);

			void UpdateSettings(void);
			void UpdateMutators(const WidgetManipulator *manipulator);

			void HandleWidgetConfigurationEvent(SettingInterface *settingInterface);
			void HandleMutatorConfigurationEvent(SettingInterface *settingInterface);

			void UpdateViewportScale(float scale, const Point3D *position = nullptr);
			void ClampViewport(void);

			void SelectWidget(const Widget *widget);
			void UnselectWidget(const Widget *widget);
			void SelectAll(void);
			void UnselectAll(void);
			void UnselectAllTemp(void);

			bool SuperWidgetSelected(const Widget *widget) const;

			static void BringWidgetsForward(Widget *root);
			static void BringWidgetsToFront(Widget *root);
			static void SendWidgetsBackward(Widget *root);
			static void SendWidgetsToBack(Widget *root);

			void HandleCloseMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleSavePanelMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleSavePanelAsMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleWindowSettingsMenuItem(Widget *menuItem, const WidgetEventData *eventData);

			void HandleUndoMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleCutMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleCopyMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandlePasteMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleClearMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleSelectAllMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleDuplicateMenuItem(Widget *menuItem, const WidgetEventData *eventData);

			void HandleBringForwardMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleBringToFrontMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleSendBackwardMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleSendToBackMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleHideSelectionMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleUnhideAllMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleLockSelectionMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleUnlockAllMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleGroupSelectionMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleUngroupSelectionMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleResetRotationMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleResetTexcoordsMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleAutoScaleTextureMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleEditScriptMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleDeleteScriptMenuItem(Widget *menuItem, const WidgetEventData *eventData);

			void HandleNudgeLeftMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleNudgeRightMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleNudgeUpMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleNudgeDownMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleAlignLeftSidesMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleAlignRightSidesMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleAlignTopSidesMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleAlignBottomSidesMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleAlignHorizontalCentersMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleAlignVerticalCentersMenuItem(Widget *menuItem, const WidgetEventData *eventData);

			static void ScriptEditorComplete(ScriptEditor *scriptEditor, void *cookie);

			Point3D ViewportToWorldPosition(const Point3D& p) const;
			void AutoScroll(const Point3D& p);

			Widget *PickWidget(const Point3D& p) const;
			Widget *PickWidgetHandle(const Point3D& p, int32 *index) const;

			void MoveSelectedWidgets(const Point3D& p);
			void RotateSelectedWidgets(const Point3D& p);
			void ResizeSelectedWidgets(const Point3D& p);
			void OffsetSelectedWidgets(const Point3D& p);
			void ScaleSelectedWidgets(const Point3D& p);

			void ResetAllMutators(void) const;
			void InvalidateManipulators(void) const;

			static bool BoxIntersectsWidget(const Point3D& p1, const Point3D& p2, const Widget *widget);

			void BeginTool(const Point3D& p);
			void TrackTool(const Point3D& p);
			void EndTool(const Point3D& p);

			void BeginWidget(const Point3D& p);
			bool TrackWidget(const Point3D& p);
			void EndWidget(const Point3D& p);

			static void ViewportHandleMouseEvent(const MouseEventData *eventData, ViewportWidget *viewport, void *cookie);
			static void ViewportTrackTask(const Point3D& position, ViewportWidget *viewport, void *cookie);
			static void ViewportRender(List<Renderable> *renderList, ViewportWidget *viewport, void *cookie);

			void HandleToolButtonEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleTextBoxEvent(Widget *widget, const WidgetEventData *eventData);

			bool SavePanel(void);
			static void SavePickerProc(FilePicker *picker, void *cookie);
			static void CloseDialogComplete(Dialog *dialog, void *cookie);

			void DisplayError(const char *string);

		public:

			PanelEditor(const PanelEffect *node, const char *name = nullptr);
			~PanelEditor();

			static void PurgeWindowList(void)
			{
				windowList.Purge();
			}

			const char *GetResourceName(void) const
			{
				return (resourceName);
			}

			unsigned_int32 GetPanelEditorFlags(void) const
			{
				return (editorFlags);
			}

			void SetPanelEditorFlags(unsigned_int32 flags)
			{
				editorFlags = flags;
			}

			RootWidget *GetRootWidget(void) const
			{
				return (rootWidget);
			}

			const PanelEffect *GetTargetNode(void) const
			{
				return (targetNode);
			}

			const List<EditorWidgetManipulator> *GetSelectionList(void) const
			{
				return (&selectionList);
			}

			void SetModifiedFlag(void)
			{
				editorState |= kPanelEditorUnsaved;
			}

			void InvalidateTransform(void)
			{
				editorState |= kPanelEditorUpdateTransformPage;
			}

			void InvalidateGrid(void)
			{
				editorState |= kPanelEditorUpdateGrid;
			}

			static void ReleaseClipboard(void)
			{
				editorClipboard.PurgeSubtree();
			}

			static EditorWidgetManipulator *GetManipulator(const Widget *widget)
			{
				return (static_cast<EditorWidgetManipulator *>(widget->GetManipulator()));
			}

			static PanelEditor *Open(const char *name);

			void SetWidgetSize(const Vector2D& size) override;

			void Preprocess(void) override;
			void Move(void) override;

			void EnterForeground(void) override;
			void EnterBackground(void) override;

			bool HandleKeyboardEvent(const KeyboardEventData *eventData) override;
			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
			void Close(void) override;

			void AddOperation(PanelOperation *operation);
			void DeleteLastOperation(void);

			void DeleteWidget(Widget *widget, bool undoable = false);

			void SelectWidgetTool(const WidgetRegistration *registration);
	};
}


#endif

// ZYUQURM
