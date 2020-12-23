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


#ifndef C4ScriptEditor_h
#define C4ScriptEditor_h


#include "C4Viewports.h"
#include "C4Expressions.h"


namespace C4
{
	enum
	{
		kScriptEditorModeTool,
		kScriptEditorModeMethod,
		kScriptEditorModeEvent,
		kScriptEditorModeSection,
		kScriptEditorModeFiber,
		kScriptEditorModeCount
	};


	enum
	{
		kScriptToolGraphSelect = -1,
		kScriptToolMethodMove,
		kScriptToolViewportScroll,
		kScriptToolViewportZoom,
		kScriptToolCount
	};


	enum
	{
		kScriptMenuUndo,
		kScriptMenuCut,
		kScriptMenuCopy,
		kScriptMenuPaste,
		kScriptMenuClear,
		kScriptMenuDuplicate,

		kScriptMenuGetInfo,
		kScriptMenuCycleFiberCondition,

		kScriptMenuItemCount
	};


	enum
	{
		kMethodColorNormal,
		kMethodColorInitial,
		kMethodColorEvent,
		kMethodColorDead,
		kMethodColorError,
		kMethodColorCount
	};


	enum
	{
		kMethodWidgetSelected			= 1 << 0,
		kMethodWidgetTempSelected		= 1 << 1
	};


	enum
	{
		kFiberWidgetSelected			= 1 << 0,
		kFiberWidgetHilited				= 1 << 1
	};


	enum
	{
		kScriptSectionWidgetSelected	= 1 << 0
	};


	enum
	{
		kScriptEditorWidget				= 1 << 0,
		kScriptEditorUnsaved			= 1 << 1,
		kScriptEditorUndoPending		= 1 << 2,
		kScriptEditorUpdateMenus		= 1 << 3,
		kScriptEditorUpdateGraph		= 1 << 4,
		kScriptEditorUpdateGrid			= 1 << 5
	};


	enum
	{
		kWidgetMethod				= 'meth',
		kWidgetFiber				= 'fibr',
		kWidgetScriptSection		= 'scsc'
	};


	class MethodWidget;
	class FiberWidget;
	class ScriptSectionWidget;
	class GraphInfoWindow; 
	class ScriptEditor;
	class Dialog;
 

	class MethodReference : public ListElement<MethodReference> 
	{
		private:
 
			MethodWidget	*reference;
 
		public: 

			MethodReference(MethodWidget *widget)
			{
				reference = widget; 
			}

			MethodWidget *GetMethodWidget(void) const
			{
				return (reference);
			}
	};


	class FiberReference : public ListElement<FiberReference>
	{
		private:

			FiberWidget		*reference;

		public:

			FiberReference(FiberWidget *widget)
			{
				reference = widget;
			}

			FiberWidget *GetFiberWidget(void) const
			{
				return (reference);
			}
	};


	class ScriptSectionReference : public ListElement<ScriptSectionReference>
	{
		private:

			ScriptSectionWidget		*reference;

		public:

			ScriptSectionReference(ScriptSectionWidget *widget)
			{
				reference = widget;
			}

			ScriptSectionWidget *GetSectionElement(void) const
			{
				return (reference);
			}
	};


	class ScriptOperation : public ListElement<ScriptOperation>
	{
		protected:

			ScriptOperation();

		public:

			virtual ~ScriptOperation();

			virtual void Restore(ScriptEditor *scriptEditor) = 0;
	};


	class CreateScriptOperation : public ScriptOperation
	{
		private:

			List<MethodReference>			createdMethodList;
			List<FiberReference>			createdFiberList;
			List<ScriptSectionReference>	createdSectionList;

		public:

			CreateScriptOperation(MethodWidget *method);
			CreateScriptOperation(FiberWidget *fiber);
			CreateScriptOperation(ScriptSectionWidget *section);
			CreateScriptOperation(const List<MethodWidget> *methodList, const List<FiberWidget> *fiberList, const List<ScriptSectionWidget> *sectionList);
			~CreateScriptOperation();

			void Restore(ScriptEditor *scriptEditor);
	};


	class DeleteScriptOperation : public ScriptOperation
	{
		private:

			List<MethodWidget>			deletedMethodList;
			List<FiberWidget>			deletedFiberList;
			List<ScriptSectionWidget>	deletedSectionList;

		public:

			DeleteScriptOperation(List<MethodWidget> *methodList, List<FiberWidget> *fiberList, List<ScriptSectionWidget> *sectionList);
			~DeleteScriptOperation();

			void Restore(ScriptEditor *scriptEditor);
	};


	class MoveScriptOperation : public ScriptOperation
	{
		private:

			class MovedMethodReference : public MethodReference
			{
				private:

					Point2D		position;

				public:

					MovedMethodReference(MethodWidget *widget);

					const Point2D& GetPosition(void) const
					{
						return (position);
					}
			};

			class MovedSectionReference : public ScriptSectionReference
			{
				private:

					Point2D		position;

				public:

					MovedSectionReference(ScriptSectionWidget *widget);

					const Point2D& GetPosition(void) const
					{
						return (position);
					}
			};

			List<MethodReference>			movedMethodList;
			List<ScriptSectionReference>	movedSectionList;

		public:

			MoveScriptOperation(const List<MethodWidget> *methodList, const List<ScriptSectionWidget> *sectionList);
			~MoveScriptOperation();

			void Restore(ScriptEditor *scriptEditor);
	};


	class ResizeScriptOperation : public ScriptOperation
	{
		private:

			ScriptSectionWidget		*sectionWidget;
			float					sectionWidth;
			float					sectionHeight;

		public:

			ResizeScriptOperation(ScriptSectionWidget *widget);
			~ResizeScriptOperation();

			void Restore(ScriptEditor *shaderEditor);
	};


	class FiberScriptOperation : public ScriptOperation
	{
		private:

			class CycledReference : public FiberReference
			{
				private:

					unsigned_int32		flags;

				public:

					CycledReference(FiberWidget *widget);

					unsigned_int32 GetFlags(void) const
					{
						return (flags);
					}
			};

			List<FiberReference>		fiberList;

		public:

			FiberScriptOperation(const List<FiberWidget> *selectionList);
			~FiberScriptOperation();

			void Restore(ScriptEditor *scriptEditor);
	};


	class MethodWidget final : public TextWidget, public ListElement<MethodWidget>
	{
		private:

			struct MethodVertex
			{
				Point2D		position;
				ColorRGBA	color;
			};

			struct BackgroundVertex
			{
				Point2D		position;
				Point2D		texcoord;
			};

			struct OutputVertex
			{
				Point2D		position;
				Point2D		texcoord;
			};

			ScriptEditor					*scriptEditor;
			Method							*scriptMethod;
			const MethodRegistration		*methodRegistration;

			unsigned_int32					methodWidgetState;
			float							viewportScale;

			Point3D							originalPosition;
			float							sortPosition;

			VertexBuffer					methodVertexBuffer;
			static SharedVertexBuffer		methodIndexBuffer;
			Renderable						methodRenderable;

			static SharedVertexBuffer		backgroundVertexBuffer;
			List<Attribute>					backgroundAttributeList;
			DiffuseAttribute				backgroundColorAttribute;
			DiffuseTextureAttribute			backgroundTextureAttribute;
			Renderable						backgroundRenderable;

			static SharedVertexBuffer		outputVertexBuffer;
			List<Attribute>					outputAttributeList;
			DiffuseAttribute				outputColorAttribute;
			DiffuseTextureAttribute			outputTextureAttribute;
			Renderable						outputRenderable;

			bool CalculateBoundingBox(Box2D *box) const override;

		public:

			MethodWidget(ScriptEditor *editor, Method *method, const MethodRegistration *registration);
			~MethodWidget();

			using ListElement<MethodWidget>::Previous;
			using ListElement<MethodWidget>::Next;

			Method *GetScriptMethod(void) const
			{
				return (scriptMethod);
			}

			const MethodRegistration *GetMethodRegistration(void) const
			{
				return (methodRegistration);
			}

			unsigned_int32 GetMethodWidgetState(void) const
			{
				return (methodWidgetState);
			}

			void SetMethodWidgetState(unsigned_int32 state)
			{
				methodWidgetState = state;
			}

			void SetViewportScale(float scale)
			{
				viewportScale = scale;
				SetBuildFlag();
			}

			const Point3D& GetOriginalPosition(void) const
			{
				return (originalPosition);
			}

			void SaveOriginalPosition(void)
			{
				originalPosition = GetWidgetPosition();
			}

			float GetSortPosition(void) const
			{
				return (sortPosition);
			}

			void SetSortPosition(float position)
			{
				sortPosition = position;
			}

			void UpdateOutputColor(bool hilite);
			void UpdateColor(int32 index);
			void UpdateText(void);

			void Select(unsigned_int32 state);
			void Unselect(void);

			WidgetPart TestPosition(const Point3D& position) const override;

			void Build(void) override;
			void Render(List<Renderable> *renderList) override;
	};


	class FiberWidget final : public RenderableWidget, public ListElement<FiberWidget>
	{
		private:

			struct FiberVertex
			{
				Point2D		position;
				Vector4D	tangent;
				Point2D		texcoord;
			};

			struct SelectionVertex
			{
				Vector4D	tangent;
				Point2D		texcoord;
			};

			struct ConditionVertex
			{
				Point2D		position;
				ColorRGB	color;
			};

			ScriptEditor				*scriptEditor;
			Fiber						*scriptFiber;

			unsigned_int32				fiberWidgetState;
			int32						fiberHiliteCount;

			float						viewportScale;

			TextWidget					*conditionText;

			VertexBuffer				fiberVertexBuffer;
			List<Attribute>				fiberAttributeList;
			DiffuseAttribute			fiberColorAttribute;
			DiffuseTextureAttribute		fiberTextureAttribute;

			VertexBuffer				selectionVertexBuffer;
			List<Attribute>				selectionAttributeList;
			DiffuseAttribute			selectionColorAttribute;
			DiffuseTextureAttribute		selectionTextureAttribute;
			Renderable					selectionRenderable;

			VertexBuffer				conditionVertexBuffer;
			static SharedVertexBuffer	conditionIndexBuffer;
			Renderable					conditionRenderable;

			FiberVertex					fiberVertex[70];

			bool CalculateBoundingBox(Box2D *box) const override;

		public:

			FiberWidget(ScriptEditor *editor, Fiber *fiber);
			~FiberWidget();

			using ListElement<FiberWidget>::Previous;
			using ListElement<FiberWidget>::Next;

			Fiber *GetScriptFiber(void) const
			{
				return (scriptFiber);
			}

			void Rebuild(void)
			{
				SetBuildFlag();
				Invalidate();
			}

			unsigned_int32 GetFiberWidgetState(void) const
			{
				return (fiberWidgetState);
			}

			void SetFiberWidgetState(unsigned_int32 state)
			{
				fiberWidgetState = state;
			}

			void SetViewportScale(float scale)
			{
				viewportScale = scale;
				SetBuildFlag();
			}

			void UpdateContent(void);

			void Select(void);
			void Unselect(void);

			void Hilite(void);
			void Unhilite(void);

			void HandleDelete(void);

			WidgetPart TestPosition(const Point3D& position) const override;

			void Build(void) override;
			void Render(List<Renderable> *renderList) override;
	};


	class ScriptSectionWidget final : public TextWidget, public ListElement<ScriptSectionWidget>
	{
		private:

			struct SectionVertex
			{
				Point2D		position;
				ColorRGBA	color;
			};

			ScriptEditor				*scriptEditor;
			SectionMethod				*sectionMethod;

			unsigned_int32				sectionWidgetState;
			float						viewportScale;

			Point3D						originalPosition;

			VertexBuffer				sectionVertexBuffer;
			static SharedVertexBuffer	sectionIndexBuffer;
			Renderable					sectionRenderable;

		public:

			ScriptSectionWidget(ScriptEditor *editor, SectionMethod *method);
			~ScriptSectionWidget();

			using ListElement<ScriptSectionWidget>::Previous;
			using ListElement<ScriptSectionWidget>::Next;

			SectionMethod *GetSectionMethod(void) const
			{
				return (sectionMethod);
			}

			unsigned_int32 GetSectionElementState(void) const
			{
				return (sectionWidgetState);
			}

			void SetSectionElementState(unsigned_int32 state)
			{
				sectionWidgetState = state;
			}

			void SetViewportScale(float scale)
			{
				viewportScale = scale;
				SetBuildFlag();
			}

			const Point3D& GetOriginalPosition(void) const
			{
				return (originalPosition);
			}

			void SaveOriginalPosition(void)
			{
				originalPosition = GetWidgetPosition();
			}

			void UpdateContent(void);

			void Select(void);
			void Unselect(void);

			WidgetPart TestPosition(const Point3D& position) const override;

			void Build(void) override;
			void Render(List<Renderable> *renderList) override;
	};


	class EditorScriptGraph : public ScriptGraph, public Configurable
	{
		friend class ScriptEditor;

		private:

			ScriptEditor					*scriptEditor;

			Point3D							viewportCameraPosition;
			float							viewportOrthoScale;
			bool							viewportInitFlag;

			Widget							*graphRoot;
			Widget							*sectionRoot;

			List<MethodWidget>				methodWidgetList;
			List<FiberWidget>				fiberWidgetList;
			List<ScriptSectionWidget>		sectionWidgetList;

			List<MethodWidget>				selectedMethodList;
			List<FiberWidget>				selectedFiberList;
			List<ScriptSectionWidget>		selectedSectionList;

			List<ScriptOperation>			operationList;

		public:

			EditorScriptGraph(ScriptEditor *editor);
			EditorScriptGraph(ScriptEditor *editor, const ScriptGraph& scriptGraph);
			~EditorScriptGraph();

			bool MethodSelected(const MethodWidget *widget) const
			{
				return (selectedMethodList.Member(widget));
			}

			bool FiberSelected(const FiberWidget *widget) const
			{
				return (selectedFiberList.Member(widget));
			}

			bool SectionSelected(const ScriptSectionWidget *widget) const
			{
				return (selectedSectionList.Member(widget));
			}

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;
			void *BeginSettings(void) override;

			void BuildScriptGraph(void);

			void SaveViewportCameraPosition(const OrthoViewportWidget *viewport);
			void RestoreViewportCameraPosition(OrthoViewportWidget *viewport);
	};


	class ScriptPage : public Page
	{
		private:

			ScriptEditor	*scriptEditor;

		protected:

			ScriptPage(ScriptEditor *editor, const char *panelName);

		public:

			~ScriptPage();

			ScriptEditor *GetScriptEditor(void) const
			{
				return (scriptEditor);
			}
	};


	class ScriptGraphsPage : public ScriptPage
	{
		private:

			class GraphWidget final : public TextWidget
			{
				public:

					EditorScriptGraph		*scriptGraph;

					GraphWidget(const Vector2D& size, EditorScriptGraph *graph);
					~GraphWidget();
			};

			ListWidget								*listWidget;
			WidgetObserver<ScriptGraphsPage>		listWidgetObserver;

			IconButtonWidget						*newButton;
			IconButtonWidget						*deleteButton;
			WidgetObserver<ScriptGraphsPage>		newButtonObserver;
			WidgetObserver<ScriptGraphsPage>		deleteButtonObserver;

			void HandleListWidgetEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleNewButtonEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleDeleteButtonEvent(Widget *widget, const WidgetEventData *eventData);

			static void NewGraphComplete(GraphInfoWindow *window, void *cookie);
			static void DeleteDialogComplete(Dialog *dialog, void *cookie);

		public:

			ScriptGraphsPage(ScriptEditor *editor);
			~ScriptGraphsPage();

			void SelectScriptGraph(int32 index)
			{
				listWidget->SelectListItem(index, true);
			}

			void Preprocess(void) override;

			void BuildGraphList(void);
	};


	class ScriptMethodsPage : public ScriptPage
	{
		private:

			enum
			{
				kMethodPaneBasic,
				kMethodPaneStandard,
				kMethodPaneCustom,
				kMethodPaneEvents,
				kMethodPaneCount
			};

			class ToolWidget final : public TextWidget
			{
				public:

					const MethodRegistration	*methodRegistration;

					ToolWidget(const Vector2D& size, const MethodRegistration *registration);
					~ToolWidget();

					static String<127> GetMethodName(const MethodRegistration *registration);
			};

			class EventWidget final : public TextWidget
			{
				public:

					EventType		scriptEvent;

					EventWidget(const Vector2D& size, EventType eventType);
					~EventWidget();
			};

			MultipaneWidget							*multipaneWidget;
			WidgetObserver<ScriptMethodsPage>		multipaneWidgetObserver;

			ListWidget								*listWidget[kMethodPaneCount];
			WidgetObserver<ScriptMethodsPage>		methodListWidgetObserver;
			WidgetObserver<ScriptMethodsPage>		eventListWidgetObserver;

			void HandleMultipaneWidgetEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleMethodListWidgetEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleEventListWidgetEvent(Widget *widget, const WidgetEventData *eventData);

		public:

			ScriptMethodsPage(ScriptEditor *editor);
			~ScriptMethodsPage();

			void Preprocess(void) override;

			void UnselectTool(void);
	};


	class ScriptVariablesPage : public ScriptPage
	{
		private:

			class VariableWidget final : public TextWidget
			{
				public:

					Value	*variableValue;

					VariableWidget(const Vector2D& size, Value *value);
					~VariableWidget();
			};

			ListWidget								*listWidget;
			WidgetObserver<ScriptVariablesPage>		listWidgetObserver;

			IconButtonWidget						*newButton;
			IconButtonWidget						*deleteButton;
			WidgetObserver<ScriptVariablesPage>		newButtonObserver;
			WidgetObserver<ScriptVariablesPage>		deleteButtonObserver;

			void HandleListWidgetEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleNewButtonEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleDeleteButtonEvent(Widget *widget, const WidgetEventData *eventData);

		public:

			ScriptVariablesPage(ScriptEditor *editor);
			~ScriptVariablesPage();

			void Preprocess(void) override;

			void BuildVariableList(void);
	};


	class MethodInfoWindow : public Window
	{
		private:

			struct SettingData
			{
				Type			categoryType;
				List<Setting>	settingList;
			};

			class TargetWidget final : public TextWidget
			{
				public:

					ConnectorKey	connectorKey;

					TargetWidget(const Vector2D& size, const char *text, const char *font, const char *key);
					~TargetWidget();
			};

			ScriptEditor					*scriptEditor;
			MethodWidget					*methodWidget;
			const Node						*controllerTarget;

			const ControllerRegistration	*controllerRegistration;

			int32							functionCount;
			Function						*currentFunction;
			Function						**functionTable;

			int32							categoryCount;
			SettingData						*currentSettingData;
			SettingData						**settingDataTable;
			const Object					*settingObject;

			PushButtonWidget				*okayButton;
			PushButtonWidget				*cancelButton;

			ConfigurationWidget				*configurationWidget;

			ListWidget						*targetList;
			ListWidget						*auxiliaryList;
			PushButtonWidget				*clearButton;

			EditTextWidget					*expressionBox;

			EditTextWidget					*outputBox;
			TextWidget						*outputText;

			const Node *GetTargetNode(void) const;

			void BuildTargetList(const Method *method);

			void UpdateFunctionList(FunctionMethod *method);
			void SelectFunction(FunctionMethod *method, int32 index, bool commit = true, bool save = false);

			void UpdateCategoryList(SettingMethod *method);
			void SelectCategory(SettingMethod *method, int32 index, bool commit = true, bool save = false);

		public:

			MethodInfoWindow(ScriptEditor *editor);
			~MethodInfoWindow();

			void Preprocess(void) override;
			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};


	class ScriptSectionInfoWindow : public Window
	{
		private:

			ScriptEditor			*scriptEditor;
			ScriptSectionWidget		*sectionWidget;

			PushButtonWidget		*okayButton;
			PushButtonWidget		*cancelButton;

			ConfigurationWidget		*configurationWidget;

		public:

			ScriptSectionInfoWindow(ScriptEditor *editor);
			~ScriptSectionInfoWindow();

			void Preprocess(void) override;
			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};


	class GraphInfoWindow : public Window, public Completable<GraphInfoWindow>
	{
		private:

			ScriptEditor			*scriptEditor;
			EditorScriptGraph		*scriptGraph;
			bool					newGraphFlag;

			PushButtonWidget		*okayButton;
			PushButtonWidget		*cancelButton;
			ConfigurationWidget		*configurationWidget;

		public:

			GraphInfoWindow(ScriptEditor *editor, EditorScriptGraph *graph = nullptr);
			~GraphInfoWindow();

			void Preprocess(void) override;
			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};


	class VariableInfoWindow : public Window
	{
		private:

			ScriptEditor			*scriptEditor;
			Value					*originalValue;
			Value					*currentValue;

			PushButtonWidget		*okayButton;
			PushButtonWidget		*cancelButton;

			ConfigurationWidget							*configurationWidget;
			ConfigurationObserver<VariableInfoWindow>	configurationObserver;

			void HandleConfigurationEvent(SettingInterface *settingInterface);

		public:

			VariableInfoWindow(ScriptEditor *editor, Value *value = nullptr);
			~VariableInfoWindow();

			void Preprocess(void) override;
			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};


	class ScriptEditor : public Window, public Completable<ScriptEditor>
	{
		private:

			struct FiberVertex
			{
				Point2D		position;
				Vector4D	tangent;
				Point2D		texcoord;
			};

			const Node						*targetNode;
			ScriptController				*scriptController;
			ScriptObject					*scriptObject;

			List<ScriptGraph>				scriptGraphList;
			Map<Value>						scriptValueMap;

			unsigned_int32					editorState;
			EditorScriptGraph				*currentScriptGraph;

			int32							currentMode;
			int32							currentTool;
			const MethodRegistration		*currentMethodReg;
			EventType						currentEventType;

			int32							trackingMode;
			int32							trackingTool;
			bool							toolTracking;
			bool							boxSelectFlag;
			Point3D							previousPoint;
			Point3D							previousPosition;
			Point3D							anchorPoint;
			Point3D							anchorPosition;

			MethodWidget					*fiberStartMethod;
			MethodWidget					*fiberFinishMethod;

			ScriptSectionWidget				*sectionTrackWidget;

			OrthoViewportWidget				*scriptViewport;
			BorderWidget					*viewportBorder;

			IconButtonWidget				*toolButton[kScriptToolCount];
			IconButtonWidget				*sectionButton;

			MenuBarWidget					*menuBar;
			PulldownMenuWidget				*scriptMenu;
			PulldownMenuWidget				*editMenu;
			PulldownMenuWidget				*methodMenu;
			MenuItemWidget					*scriptMenuItem[kScriptMenuItemCount];

			BookWidget						*bookWidget;
			ScriptGraphsPage				*graphsPage;
			ScriptMethodsPage				*methodsPage;
			ScriptVariablesPage				*variablesPage;

			WidgetObserver<ScriptEditor>	toolButtonObserver;

			Grid							viewportGrid;
			DragRect						dragRect;

			VertexBuffer					fiberVertexBuffer;
			List<Attribute>					fiberAttributeList;
			DiffuseAttribute				fiberColorAttribute;
			DiffuseTextureAttribute			fiberTextureAttribute;
			Renderable						fiberRenderable;
			FiberVertex						fiberVertex[4];

			static ScriptGraph				editorClipboard;

			void PositionWidgets(void);
			void BuildMenus(void);

			void BuildScriptGraph(void);
			void UpdateScriptGraph(void);

			static void TraverseScriptGraph(Method *method, int32 depth);
			static bool DetectMethodError(const Method *method);

			void UnselectCurrentTool(void);

			void AddOperation(ScriptOperation *operation);

			void HandleCloseMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleSaveScriptMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleUndoMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleCutMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleCopyMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandlePasteMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleClearMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleSelectAllMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleDuplicateMenuItem(Widget *menuItem, const WidgetEventData *eventData);

			void HandleGetInfoMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleCycleFiberConditionMenuItem(Widget *menuItem, const WidgetEventData *eventData);

			Point3D ViewportToGraphPosition(const Point3D& p) const;
			static Point3D AlignPositionToGrid(const Point3D& p);
			void ShowGraphPosition(float x, float y);
			void AutoScroll(const Point3D& p);

			MethodWidget *FindMethodWidget(const Point3D& position) const;

			void SortSelectedMethodSublist(List<MethodWidget> *list, float dmin, float dmax);
			void SortSelectedMethodList(float dx, float dy);

			static bool BoxIntersectsMethodWidget(const Point3D& p1, const Point3D& p2, const MethodWidget *widget);
			bool MethodBoxIntersectsAnyMethodWidget(float x, float y, const MethodWidget *exclude = nullptr);

			void BeginTool(const Point3D& p, unsigned_int32 eventFlags);
			void TrackTool(const Point3D& p);
			void EndTool(const Point3D& p);

			void BeginSection(const Point3D& p);
			void TrackSection(const Point3D& p);
			void EndSection(const Point3D& p);

			void BeginFiber(const Point3D& p);
			void TrackFiber(const Point3D& p);
			void EndFiber(const Point3D& p);

			void CreateMethod(const Point3D& p);

			static void ViewportHandleMouseEvent(const MouseEventData *eventData, ViewportWidget *viewport, void *cookie);
			static void ViewportTrackTask(const Point3D& position, ViewportWidget *viewport, void *cookie);
			static void ViewportRender(List<Renderable> *renderList, ViewportWidget *viewport, void *cookie);

			void HandleToolButtonEvent(Widget *widget, const WidgetEventData *eventData);

			static void CloseDialogComplete(Dialog *dialog, void *cookie);

		public:

			ScriptEditor(const Node *target, ScriptController *controller, ScriptObject *object, bool widgetFlag = false);
			~ScriptEditor();

			const Node *GetTargetNode(void) const
			{
				return (targetNode);
			}

			ScriptObject *GetScriptObject(void) const
			{
				return (scriptObject);
			}

			List<ScriptGraph> *GetScriptGraphList(void)
			{
				return (&scriptGraphList);
			}

			Map<Value> *GetValueMap(void)
			{
				return (&scriptValueMap);
			}

			Value *FindValue(const char *name) const
			{
				return (scriptValueMap.Find(name));
			}

			unsigned_int32 GetEditorState(void) const
			{
				return (editorState);
			}

			void AddEditorState(unsigned_int32 state)
			{
				editorState |= state;
			}

			EditorScriptGraph *GetCurrentScriptGraph(void) const
			{
				return (currentScriptGraph);
			}

			MethodWidget *GetFirstSelectedMethod(void) const
			{
				return (currentScriptGraph->selectedMethodList.First());
			}

			ScriptSectionWidget *GetFirstSelectedSection(void) const
			{
				return (currentScriptGraph->selectedSectionList.First());
			}

			ScriptGraphsPage *GetGraphsPage(void) const
			{
				return (graphsPage);
			}

			ScriptVariablesPage *GetVariablesPage(void) const
			{
				return (variablesPage);
			}

			static void ReleaseClipboard(void)
			{
				editorClipboard.Purge();
			}

			void SetWidgetSize(const Vector2D& size) override;

			void Preprocess(void) override;
			void Move(void) override;

			void EnterForeground(void) override;
			void EnterBackground(void) override;

			bool HandleKeyboardEvent(const KeyboardEventData *eventData) override;
			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
			void Close(void) override;

			void SetCurrentScriptGraph(EditorScriptGraph *scriptGraph);

			void ReattachMethod(MethodWidget *widget);
			void ReattachFiber(FiberWidget *widget);
			void ReattachSection(ScriptSectionWidget *widget);

			void RebuildFiberWidgets(const Method *method);

			void UpdateViewportScale(float scale, const Point3D *position = nullptr);

			void SelectMethod(MethodWidget *methodWidget, unsigned_int32 state = 0);
			void UnselectMethod(MethodWidget *methodWidget);
			void SelectFiber(FiberWidget *fiberWidget);
			void UnselectFiber(FiberWidget *fiberWidget);
			void SelectSection(ScriptSectionWidget *sectionWidget);
			void UnselectSection(ScriptSectionWidget *sectionWidget);
			void SelectAll(void);
			void UnselectAll(void);
			void UnselectAllTemp(void);

			void DeleteMethod(MethodWidget *methodWidget, List<MethodWidget> *deletedMethodList = nullptr, List<FiberWidget> *deletedFiberList = nullptr);
			void DeleteFiber(FiberWidget *fiberWidget, List<FiberWidget> *deletedFiberList = nullptr);
			void DeleteSection(ScriptSectionWidget *sectionWidget, List<ScriptSectionWidget> *deletedSectionList = nullptr);

			void SelectDefaultTool(void);
			void SelectMethodTool(const MethodRegistration *registration);
			void SelectEventTool(EventType eventType);
	};
}


#endif

// ZYUQURM
