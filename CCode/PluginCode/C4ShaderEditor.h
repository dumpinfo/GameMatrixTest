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


#ifndef C4ShaderEditor_h
#define C4ShaderEditor_h


#include "C4Shaders.h"
#include "C4Primitives.h"
#include "C4Viewports.h"
#include "C4Configuration.h"
#include "C4EditorBase.h"


namespace C4
{
	enum
	{
		kShaderEditorModeTool,
		kShaderEditorModeProcess,
		kShaderEditorModeSection,
		kShaderEditorModeRoute,
		kShaderEditorModeCount
	};


	enum
	{
		kShaderToolGraphSelect = -1,
		kShaderToolProcessMove,
		kShaderToolViewportScroll,
		kShaderToolViewportZoom,
		kShaderToolCount
	};


	enum
	{
		kShaderMenuUndo,
		kShaderMenuCut,
		kShaderMenuCopy,
		kShaderMenuPaste,
		kShaderMenuClear,
		kShaderMenuDuplicate,

		kShaderMenuGetInfo,
		kShaderMenuToggleDetailLevel,
		kShaderMenuShowAmbientShader,
		kShaderMenuShowLightShader,

		kShaderMenuItemCount
	};


	enum
	{
		kProcessWidgetSelected			= 1 << 0,
		kProcessWidgetTempSelected		= 1 << 1
	};


	enum
	{
		kRouteWidgetSelected			= 1 << 0,
		kRouteWidgetHilited				= 1 << 1
	};


	enum
	{
		kShaderSectionWidgetSelected	= 1 << 0
	};


	enum
	{
		kShaderEditorUnsaved			= 1 << 0,
		kShaderEditorUndoPending		= 1 << 1,
		kShaderEditorUpdateMenus		= 1 << 2,
		kShaderEditorUpdateGraph		= 1 << 3,
		kShaderEditorUpdateGrid			= 1 << 4
	};


	enum
	{
		kWidgetProcess					= 'proc',
		kWidgetRoute					= 'rout',
		kWidgetShaderSection			= 'shsc'
	};


	class MaterialWindow;
	class ProcessWidget;
	class RouteWidget;
	class ShaderSectionWidget;
	class ShaderEditor;
	class PrimitiveGeometry;
	class Dialog;


	class ProcessReference : public ListElement<ProcessReference>
	{
		private: 

			ProcessWidget	*reference;
 
		public:
 
			ProcessReference(ProcessWidget *widget)
			{
				reference = widget; 
			}
 
			ProcessWidget *GetProcessWidget(void) const 
			{
				return (reference);
			}
	}; 


	class RouteReference : public ListElement<RouteReference>
	{
		private:

			RouteWidget		*reference;

		public:

			RouteReference(RouteWidget *widget)
			{
				reference = widget;
			}

			RouteWidget *GetRouteWidget(void) const
			{
				return (reference);
			}
	};


	class ShaderSectionReference : public ListElement<ShaderSectionReference>
	{
		private:

			ShaderSectionWidget		*reference;

		public:

			ShaderSectionReference(ShaderSectionWidget *widget)
			{
				reference = widget;
			}

			ShaderSectionWidget *GetSectionWidget(void) const
			{
				return (reference);
			}
	};


	class ShaderOperation : public ListElement<ShaderOperation>
	{
		private:

			bool	coupledFlag;

		protected:

			ShaderOperation();

		public:

			virtual ~ShaderOperation();

			bool Coupled(void) const
			{
				return (coupledFlag);
			}

			void SetCoupledFlag(bool flag)
			{
				coupledFlag = flag;
			}

			virtual void Restore(ShaderEditor *shaderEditor) = 0;
	};


	class CreateShaderOperation : public ShaderOperation
	{
		private:

			List<ProcessReference>			createdProcessList;
			List<RouteReference>			createdRouteList;
			List<ShaderSectionReference>	createdSectionList;

		public:

			CreateShaderOperation(ProcessWidget *process);
			CreateShaderOperation(RouteWidget *route);
			CreateShaderOperation(ShaderSectionWidget *section);
			CreateShaderOperation(const List<ProcessWidget> *processList, const List<RouteWidget> *routeList, const List<ShaderSectionWidget> *sectionList);
			~CreateShaderOperation();

			void Restore(ShaderEditor *shaderEditor);
	};


	class DeleteShaderOperation : public ShaderOperation
	{
		private:

			List<ProcessWidget>			deletedProcessList;
			List<RouteWidget>			deletedRouteList;
			List<ShaderSectionWidget>	deletedSectionList;

		public:

			DeleteShaderOperation(List<ProcessWidget> *processList, List<RouteWidget> *routeList, List<ShaderSectionWidget> *sectionList);
			~DeleteShaderOperation();

			void Restore(ShaderEditor *shaderEditor);
	};


	class MoveShaderOperation : public ShaderOperation
	{
		private:

			class MovedProcessReference : public ProcessReference
			{
				private:

					Point2D		position;

				public:

					MovedProcessReference(ProcessWidget *widget);

					const Point2D& GetPosition(void) const
					{
						return (position);
					}
			};

			class MovedSectionReference : public ShaderSectionReference
			{
				private:

					Point2D		position;

				public:

					MovedSectionReference(ShaderSectionWidget *widget);

					const Point2D& GetPosition(void) const
					{
						return (position);
					}
			};

			List<ProcessReference>			movedProcessList;
			List<ShaderSectionReference>	movedSectionList;

		public:

			MoveShaderOperation(const List<ProcessWidget> *processList, const List<ShaderSectionWidget> *sectionList);
			~MoveShaderOperation();

			void Restore(ShaderEditor *shaderEditor);
	};


	class ResizeShaderOperation : public ShaderOperation
	{
		private:

			ShaderSectionWidget		*sectionWidget;
			float					sectionWidth;
			float					sectionHeight;

		public:

			ResizeShaderOperation(ShaderSectionWidget *widget);
			~ResizeShaderOperation();

			void Restore(ShaderEditor *shaderEditor);
	};


	class RouteShaderOperation : public ShaderOperation
	{
		private:

			class DetailedReference : public RouteReference
			{
				private:

					unsigned_int32		flags;

				public:

					DetailedReference(RouteWidget *widget);

					unsigned_int32 GetFlags(void) const
					{
						return (flags);
					}
			};

			List<RouteReference>		routeList;

		public:

			RouteShaderOperation(const List<RouteWidget> *selectionList);
			~RouteShaderOperation();

			void Restore(ShaderEditor *shaderEditor);
	};


	class ProcessWidget final : public TextWidget, public ListElement<ProcessWidget>
	{
		private:

			struct ProcessVertex
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

			struct PortVertex
			{
				Point2D		position;
				ColorRGBA	color;
				Point2D		texcoord;
			};

			ShaderEditor					*shaderEditor;
			Process							*shaderProcess;
			const ProcessRegistration		*processRegistration;

			unsigned_int32					processWidgetState;
			float							viewportScale;

			Point3D							originalPosition;
			float							sortPosition;

			int32							hilitePortIndex;

			TextWidget						*commentText;
			TextWidget						*valueText;
			QuadWidget						*colorBox;
			ImageWidget						*textureBox;

			TextWidget						*portText[kMaxProcessPortCount];

			VertexBuffer					processVertexBuffer;
			static SharedVertexBuffer		processIndexBuffer;
			Renderable						processRenderable;

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

			VertexBuffer					portVertexBuffer;
			static SharedVertexBuffer		portIndexBuffer;
			List<Attribute>					portAttributeList;
			DiffuseTextureAttribute			portTextureAttribute;
			Renderable						portRenderable;

			bool CalculateBoundingBox(Box2D *box) const override;

		public:

			ProcessWidget(ShaderEditor *editor, Process *process, const ProcessRegistration *registration);
			~ProcessWidget();

			using ListElement<ProcessWidget>::Previous;
			using ListElement<ProcessWidget>::Next;

			Process *GetShaderProcess(void) const
			{
				return (shaderProcess);
			}

			unsigned_int32 GetProcessWidgetState(void) const
			{
				return (processWidgetState);
			}

			void SetProcessWidgetState(unsigned_int32 state)
			{
				processWidgetState = state;
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

			void HilitePort(int32 index)
			{
				hilitePortIndex = index;
				SetBuildFlag();
			}

			void UpdateOutputColor(bool hilite);
			void UpdateContent(void);

			void Select(unsigned_int32 state);
			void Unselect(void);

			WidgetPart TestPosition(const Point3D& position) const override;

			void Build(void) override;
			void Render(List<Renderable> *renderList) override;
	};


	class RouteWidget final : public RenderableWidget, public ListElement<RouteWidget>
	{
		private:

			struct RouteVertex
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

			struct SwizzleVertex
			{
				Point2D		position;
				ColorRGB	color;
			};

			ShaderEditor				*shaderEditor;
			Route						*shaderRoute;

			unsigned_int32				routeWidgetState;
			int32						routeHiliteCount;

			float						viewportScale;

			TextWidget					*swizzleText;

			VertexBuffer				routeVertexBuffer;
			List<Attribute>				routeAttributeList;
			DiffuseAttribute			routeColorAttribute;
			DiffuseTextureAttribute		routeTextureAttribute;

			VertexBuffer				selectionVertexBuffer;
			List<Attribute>				selectionAttributeList;
			DiffuseAttribute			selectionColorAttribute;
			DiffuseTextureAttribute		selectionTextureAttribute;
			Renderable					selectionRenderable;

			VertexBuffer				swizzleVertexBuffer;
			static SharedVertexBuffer	swizzleIndexBuffer;
			Renderable					swizzleRenderable;

			RouteVertex					routeVertex[70];

			bool CalculateBoundingBox(Box2D *box) const override;

		public:

			RouteWidget(ShaderEditor *editor, Route *route);
			~RouteWidget();

			using ListElement<RouteWidget>::Previous;
			using ListElement<RouteWidget>::Next;

			Route *GetShaderRoute(void) const
			{
				return (shaderRoute);
			}

			void Rebuild(void)
			{
				SetBuildFlag();
				Invalidate();
			}

			unsigned_int32 GetRouteWidgetState(void) const
			{
				return (routeWidgetState);
			}

			void SetRouteWidgetState(unsigned_int32 state)
			{
				routeWidgetState = state;
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


	class ShaderSectionWidget final : public TextWidget, public ListElement<ShaderSectionWidget>
	{
		private:

			struct SectionVertex
			{
				Point2D		position;
				ColorRGBA	color;
			};

			ShaderEditor				*shaderEditor;
			SectionProcess				*sectionProcess;

			unsigned_int32				sectionWidgetState;
			float						viewportScale;

			Point3D						originalPosition;

			VertexBuffer				sectionVertexBuffer;
			static SharedVertexBuffer	sectionIndexBuffer;
			Renderable					sectionRenderable;

		public:

			ShaderSectionWidget(ShaderEditor *editor, SectionProcess *process);
			~ShaderSectionWidget();

			using ListElement<ShaderSectionWidget>::Previous;
			using ListElement<ShaderSectionWidget>::Next;

			SectionProcess *GetSectionProcess(void) const
			{
				return (sectionProcess);
			}

			unsigned_int32 GetSectionWidgetState(void) const
			{
				return (sectionWidgetState);
			}

			void SetSectionWidgetState(unsigned_int32 state)
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


	class PreviewWidget final : public FrustumViewportWidget
	{
		private:

			float					cameraDistance;

			bool					trackFlag;
			Point3D					previousPosition;

			World					*previewWorld;
			Light					*previewLight;
			PrimitiveGeometry		*previewGeometry;

			MaterialObject			*previewMaterial;

			void SetCameraAngles(float azm, float alt);

			static void ViewportHandleMouseEvent(const MouseEventData *eventData, ViewportWidget *viewport, void *cookie);
			static void ViewportRender(List<Renderable> *renderList, ViewportWidget *viewport, void *cookie);

		public:

			PreviewWidget(const Vector2D& size, MaterialObject *materialObject);
			~PreviewWidget();

			const PrimitiveGeometry *GetPreviewGeometry(void) const
			{
				return (previewGeometry);
			}

			void Preprocess(void) override;

			void SetMaterial(MaterialObject *materialObject);
			void SetPreviewGeometry(PrimitiveType type);

			void UpdatePreview(void);
	};


	class ShaderPage : public Page
	{
		private:

			ShaderEditor	*shaderEditor;

		protected:

			ShaderPage(ShaderEditor *editor, const char *panelName);

		public:

			~ShaderPage();

			ShaderEditor *GetShaderEditor(void) const
			{
				return (shaderEditor);
			}
	};


	class ShaderProcessesPage : public ShaderPage
	{
		private:

			enum
			{
				kProcessPaneBasic,
				kProcessPaneMath,
				kProcessPaneComplex,
				kProcessPaneInterpolants,
				kProcessPaneCount
			};

			class ToolWidget final : public TextWidget
			{
				public:

					const ProcessRegistration	*processRegistration;

					ToolWidget(const Vector2D& size, const ProcessRegistration *registration);
					~ToolWidget();

					static String<127> GetProcessName(const ProcessRegistration *registration);
			};

			MultipaneWidget							*multipaneWidget;
			ListWidget								*listWidget[kProcessPaneCount];

			WidgetObserver<ShaderProcessesPage>		multipaneWidgetObserver;
			WidgetObserver<ShaderProcessesPage>		listWidgetObserver;

			void HandleMultipaneWidgetEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleListWidgetEvent(Widget *widget, const WidgetEventData *eventData);

		public:

			ShaderProcessesPage(ShaderEditor *editor);
			~ShaderProcessesPage();

			void Preprocess(void) override;

			void UnselectProcessTool(void);
	};


	class ShaderPreviewPage : public ShaderPage
	{
		private:

			PrimitiveType						initPrimitiveType;

			MaterialObject						*previewMaterial;
			ShaderAttribute						*previewAttribute;

			PreviewWidget						*previewWidget;
			PopupMenuWidget						*menuWidget;

			WidgetObserver<ShaderPreviewPage>	menuWidgetObserver;

			void HandleMenuWidgetEvent(Widget *widget, const WidgetEventData *eventData);

		public:

			ShaderPreviewPage(ShaderEditor *editor, PrimitiveType primitiveType);
			~ShaderPreviewPage();

			void Preprocess(void) override;

			void UpdatePreviewMaterial(const ShaderGraph *shaderGraph, ShaderResult *result);
			void SetPreviewGeometry(PrimitiveType type);
	};


	class ProcessInfoWindow : public Window
	{
		private:

			ShaderEditor			*shaderEditor;
			ProcessWidget			*processWidget;

			PushButtonWidget		*okayButton;
			PushButtonWidget		*cancelButton;

			ConfigurationWidget		*configurationWidget;

		public:

			ProcessInfoWindow(ShaderEditor *editor);
			~ProcessInfoWindow();

			void Preprocess(void) override;
			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};


	class RouteInfoWindow : public Window
	{
		private:

			ShaderEditor			*shaderEditor;
			RouteWidget				*routeWidget;

			PushButtonWidget		*okayButton;
			PushButtonWidget		*cancelButton;

			ConfigurationWidget		*configurationWidget;

		public:

			RouteInfoWindow(ShaderEditor *editor);
			~RouteInfoWindow();

			void Preprocess(void) override;

			bool HandleKeyboardEvent(const KeyboardEventData *eventData) override;
			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};


	class ShaderSectionInfoWindow : public Window
	{
		private:

			ShaderEditor			*shaderEditor;
			ShaderSectionWidget		*sectionWidget;

			PushButtonWidget		*okayButton;
			PushButtonWidget		*cancelButton;

			ConfigurationWidget		*configurationWidget;

		public:

			ShaderSectionInfoWindow(ShaderEditor *editor);
			~ShaderSectionInfoWindow();

			void Preprocess(void) override;
			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};


	class ShaderEditor : public Window, public Completable<ShaderEditor>
	{
		private:

			struct RouteVertex
			{
				Point2D		position;
				Vector4D	tangent;
				Point2D		texcoord;
			};

			MaterialWindow					*materialWindow;
			MaterialObject					*materialObject;

			ShaderGraph						shaderGraph;

			PrimitiveType					initPrimitiveType;
			unsigned_int32					editorState;

			int32							currentMode;
			int32							currentTool;
			const ProcessRegistration		*currentProcessReg;

			List<ShaderOperation>			operationList;

			int32							trackingMode;
			int32							trackingTool;
			bool							toolTracking;
			bool							boxSelectFlag;
			Point3D							previousPoint;
			Point3D							previousPosition;
			Point3D							anchorPoint;
			Point3D							anchorPosition;

			ProcessWidget					*routeStartProcess;
			ProcessWidget					*routeFinishProcess;
			int32							routePort;

			ShaderSectionWidget				*sectionTrackWidget;

			Widget							*graphRoot;
			Widget							*sectionRoot;

			List<ProcessWidget>				processWidgetList;
			List<RouteWidget>				routeWidgetList;
			List<ShaderSectionWidget>		sectionWidgetList;

			List<ProcessWidget>				selectedProcessList;
			List<RouteWidget>				selectedRouteList;
			List<ShaderSectionWidget>		selectedSectionList;

			OrthoViewportWidget				*shaderViewport;
			BorderWidget					*viewportBorder;

			IconButtonWidget				*toolButton[kShaderToolCount];
			IconButtonWidget				*sectionButton;

			TextWidget						*statusMessage;

			MenuBarWidget					*menuBar;
			PulldownMenuWidget				*shaderMenu;
			PulldownMenuWidget				*editMenu;
			PulldownMenuWidget				*processMenu;
			MenuItemWidget					*shaderMenuItem[kShaderMenuItemCount];

			BookWidget						*bookWidget;
			ShaderProcessesPage				*processesPage;
			ShaderPreviewPage				*previewPage;

			WidgetObserver<ShaderEditor>	toolButtonObserver;

			Grid							viewportGrid;
			DragRect						dragRect;

			VertexBuffer					routeVertexBuffer;
			List<Attribute>					routeAttributeList;
			DiffuseAttribute				routeColorAttribute;
			DiffuseTextureAttribute			routeTextureAttribute;
			Renderable						routeRenderable;
			RouteVertex						routeVertex[4];

			static ShaderGraph				editorClipboard;

			bool ProcessSelected(const ProcessWidget *widget) const
			{
				return (selectedProcessList.Member(widget));
			}

			bool RouteSelected(const RouteWidget *widget) const
			{
				return (selectedRouteList.Member(widget));
			}

			bool SectionSelected(const ShaderSectionWidget *widget) const
			{
				return (selectedSectionList.Member(widget));
			}

			void TranslateAttributes(void);

			void PositionWidgets(void);
			void BuildMenus(void);

			void BuildShaderGraph(void);
			void UpdateShaderGraph(void);

			void UnselectCurrentTool(void);

			void UpdateViewportScale(float scale, const Point3D *position = nullptr);

			void AddOperation(ShaderOperation *operation);

			void HandleCloseMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleSaveShaderMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleUndoMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleCutMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleCopyMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandlePasteMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleClearMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleSelectAllMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleDuplicateMenuItem(Widget *menuItem, const WidgetEventData *eventData);

			void HandleGetInfoMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleToggleDetailLevelMenuItem(Widget *menuItem, const WidgetEventData *eventData);

			Point3D ViewportToGraphPosition(const Point3D& p) const;
			static Point3D AlignPositionToGrid(const Point3D& p);
			void ShowGraphPosition(float x, float y);
			void AutoScroll(const Point3D& p);

			ProcessWidget *FindProcessWidget(const Point3D& position) const;
			ProcessWidget *FindProcessPort(const Point3D& position, int32 *port) const;
			RouteWidget *GetRouteWidget(const Route *route) const;

			void SortSelectedProcessSublist(List<ProcessWidget> *list, float dmin, float dmax);
			void SortSelectedProcessList(float dx, float dy);

			static bool BoxIntersectsProcessWidget(const Point3D& p1, const Point3D& p2, const ProcessWidget *widget);
			bool ProcessBoxIntersectsAnyProcessWidget(float x, float y, const ProcessWidget *exclude = nullptr);

			void BeginTool(const Point3D& p, unsigned_int32 eventFlags);
			void TrackTool(const Point3D& p);
			void EndTool(const Point3D& p);

			void BeginSection(const Point3D& p);
			void TrackSection(const Point3D& p);
			void EndSection(const Point3D& p);

			void TrackRoute(const Point3D& p);
			void EndRoute(const Point3D& p);

			void CreateProcess(const Point3D& p);

			static void ViewportHandleMouseEvent(const MouseEventData *eventData, ViewportWidget *viewport, void *cookie);
			static void ViewportTrackTask(const Point3D& position, ViewportWidget *viewport, void *cookie);
			static void ViewportRender(List<Renderable> *renderList, ViewportWidget *viewport, void *cookie);

			void HandleToolButtonEvent(Widget *widget, const WidgetEventData *eventData);

			static void CloseDialogComplete(Dialog *dialog, void *cookie);

		public:

			C4EDITORAPI ShaderEditor(MaterialWindow *window, MaterialObject *material, PrimitiveType primitiveType = kPrimitiveSphere);
			C4EDITORAPI ~ShaderEditor();

			MaterialObject *GetMaterialObject(void) const
			{
				return (materialObject);
			}

			ProcessWidget *GetFirstSelectedProcess(void) const
			{
				return (selectedProcessList.First());
			}

			RouteWidget *GetFirstSelectedRoute(void) const
			{
				return (selectedRouteList.First());
			}

			ShaderSectionWidget *GetFirstSelectedSection(void) const
			{
				return (selectedSectionList.First());
			}

			void SetModifiedFlag(void)
			{
				editorState |= kShaderEditorUnsaved | kShaderEditorUpdateGraph;
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

			void ReattachProcess(ProcessWidget *widget);
			void ReattachRoute(RouteWidget *widget);
			void ReattachSection(ShaderSectionWidget *widget);

			void RebuildRouteWidgets(const Process *process);

			void SelectProcess(ProcessWidget *processWidget, unsigned_int32 state = 0);
			void UnselectProcess(ProcessWidget *processWidget);
			void SelectRoute(RouteWidget *routeWidget);
			void UnselectRoute(RouteWidget *routeWidget);
			void SelectSection(ShaderSectionWidget *sectionWidget);
			void UnselectSection(ShaderSectionWidget *sectionWidget);
			void SelectAll(void);
			void UnselectAll(void);
			void UnselectAllTemp(void);

			void DeleteProcess(ProcessWidget *processWidget, List<ProcessWidget> *deletedProcessList = nullptr, List<RouteWidget> *deletedRouteList = nullptr);
			void DeleteRoute(RouteWidget *routeWidget, List<RouteWidget> *deletedRouteList = nullptr);
			void DeleteSection(ShaderSectionWidget *sectionWidget, List<ShaderSectionWidget> *deletedSectionList = nullptr);

			void SelectDefaultTool(void);
			void SelectProcessTool(const ProcessRegistration *registration);
	};
}


#endif

// ZYUQURM
