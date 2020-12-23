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


#ifndef C4WorldEditor_h
#define C4WorldEditor_h


//# \component	World Editor
//# \prefix		WorldEditor/


#include "C4EditorViewports.h"
#include "C4EditorCommands.h"
#include "C4EditorPlugins.h"
#include "C4EditorTools.h"
#include "C4EditorOperations.h"
#include "C4NodeInfo.h"
#include "C4MaterialManager.h"
#include "C4SpaceManipulators.h"


#ifdef C4EDITOR

	extern "C"
	{
		C4MODULEEXPORT C4::Plugin *CreatePlugin(void);
	}

#endif


namespace C4
{
	enum
	{
		kEditorWorldUnsaved				= 1 << 0,
		kEditorWaitUpdate				= 1 << 1,
		kEditorSelectionLocked			= 1 << 2,
		kEditorSelectionUpdated			= 1 << 3,
		kEditorProcessProperties		= 1 << 4,
		kEditorRedrawViewports			= 1 << 5,
		kEditorEnableSelectionItems		= 1 << 6,
		kEditorDisableSelectionItems	= 1 << 7,
		kEditorUpdateConditionalItems	= 1 << 8
	};


	enum
	{
		kGizmoMoveEnable				= 1 << 0,
		kGizmoRotateEnable				= 1 << 1,
		kGizmoAxisMoveActive			= 1 << 2,
		kGizmoPlaneMoveActive			= 1 << 3,
		kGizmoRotateActive				= 1 << 4
	};


	enum
	{
		kEditorRenderHandles			= 1 << 0,
		kEditorRenderConnectors			= 1 << 1
	};


	enum
	{
		kEditorFlagBackfaces,
		kEditorFlagExpandWorlds,
		kEditorFlagExpandModels,
		kEditorFlagLighting,
		kEditorFlagCenter,
		kEditorFlagCap,
		kEditorFlagCount
	};


	enum
	{
		kEditorCursorArrow,
		kEditorCursorLock,
		kEditorCursorHand,
		kEditorCursorDrag,
		kEditorCursorGlass,
		kEditorCursorCross,
		kEditorCursorDropper,
		kEditorCursorInsert,
		kEditorCursorRemove,
		kEditorCursorFree,
		kEditorCursorLight,
		kEditorCursorStop,
		kEditorCursorCount
	};


	class EditorTool;
	class EditorPage;
	class EditorObject;
	class Dialog;


	//# \class	Editor		Represents a primary World Editor window.
	//
	//# The $Editor$ class represents a primary World Editor window.
	// 
	//# \def	class Editor : public Window, public Observable<Editor, const EditorEvent&>, public ListElement<Editor>
	//
	//# \ctor	Editor(const char *name); 
	//
	//# \param	name	The resource name of the world. If this is $nullptr$, then a new empty scene is created. 
	//
	//# \desc
	//# The $Editor$ class manages the primary World Editor window for a scene. 
	//
	//# \base	InterfaceMgr/Window									An $Editor$ is a specific type of window. 
	//# \base	Utilities/Observable<Editor, const EditorEvent&>	Editors can be observed for various events. 
	//# \base	Utilities/ListElement<Editor>						Used internally by the World Editor.


	//# \function	Editor::GetResourceName		Returns the resource name for an editor window. 
	//
	//# \proto	const char *GetResourceName(void) const;
	//
	//# \desc
	//# The $GetResourceName$ function returns a pointer to the resource name for the world being edited in an
	//# editor window. If the editor was created as a new scene that has not yet been saved, then the return value
	//# will point to an empty string.


	//# \function	Editor::ImportScene		Imports a scene into an editor window.
	//
	//# \proto	void ImportScene(Node *root);
	//
	//# \param	root	The root node of the scene to import.
	//
	//# \desc
	//# The $ImportScene$ function moves all subnodes of the node specified by the $root$ parameter to the current target
	//# zone in the world being edited. The root node itself is <i>not</i> added to the world and should be deleted
	//# after the $ImportScene$ function is called.
	//
	//# \also	$@EditorPage@$


	class Editor : public Window, public Observable<Editor, const EditorEvent&>, public ListElement<Editor>
	{
		friend class WorldEditor;

		public:

			typedef bool PickFilterProc(const Node *, const PickData *, const void *);
			typedef void PlacementAdjusterProc(const Editor *, Transform4D *, const Vector3D&, void *);

		private:

			ResourceName					resourceName;
			ResourceLocation				resourceLocation;

			unsigned_int32					editorState;
			unsigned_int32					gizmoState;
			unsigned_int32					renderFlags;

			Node							*rootNode;
			World							*editorWorld;
			EditorObject					*editorObject;

			StandardEditorTool				*standardTool[kEditorToolCount];
			NodeReparentTool				*nodeReparentTool;

			EditorTool						*currentTool;
			Cursor							*currentCursor;

			MultipaneWidget					*bookPane;
			Widget							*toolGroup;
			IconButtonWidget				*toolButton[kEditorToolCount];
			IconButtonWidget				*flagButton[kEditorFlagCount];

			MenuBarWidget					*menuBar;
			PulldownMenuWidget				*worldMenu;
			PulldownMenuWidget				*editMenu;
			PulldownMenuWidget				*nodeMenu;
			PulldownMenuWidget				*geometryMenu;
			PulldownMenuWidget				*viewMenu;
			PulldownMenuWidget				*layoutMenu;

			MenuItemWidget					*editorMenuItem[kEditorMenuItemCount];
			MenuItemWidget					*showViewportItem[kEditorViewportCount];

			BookWidget						*bookWidget[kEditorBookCount];
			EditorViewport					*editorViewport[kEditorViewportCount];

			Point3D							viewportPosition[kEditorViewportCount];
			Vector2D						viewportSize[kEditorViewportCount];

			List<NodeReference>				selectionList;
			List<EditorManipulator>			selectedConnectorList;
			const NodeReference				*gizmoTarget;

			PickFilterProc					*pickFilterProc;
			const void						*pickFilterCookie;

			PlacementAdjusterProc			*placementAdjusterProc;
			void							*placementAdjusterCookie;

			List<Operation>					operationList;

			EditorTool						*trackingTool;
			EditorTrackData					editorTrackData;
			Vector2D						scrollFraction;

			NodeInfoType					nodeInfoType;
			Type							nodeInfoCategory;

			int32							viewportMenuIndex;
			List<MenuItemWidget>			viewportMenuItemList;

			static Package					*editorClipboard;
			static Transform4D				transformClipboard;

			static List<Editor>				windowList;

			void PositionWidgets(void);

			void BuildMenus(void);
			void BuildViewportMenu(void);

			void EnableSelectionItems(void);
			void DisableSelectionItems(void);
			void UpdateConditionalItems(void);

			static bool EntireGroupSelected(const Node *root);

			void HandleCloseMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleSaveWorldMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleSaveWorldAsMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleSaveAndPlayWorldMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleImportSceneMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleExportSceneMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleLoadModelResourceMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleSaveModelResourceMenuItem(Widget *menuItem, const WidgetEventData *eventData);

			void HandleUndoMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleCutMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleCopyMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandlePasteMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandlePasteSubnodesMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleClearMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleSelectAllMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleSelectAllZoneMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleSelectAllMaskMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleSelectMaterialMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleSelectSuperNodeMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleSelectSubtreeMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleLockSelectionMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleUnlockSelectionMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleDuplicateMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleCloneMenuItem(Widget *menuItem, const WidgetEventData *eventData);

			void HandleNodeInfoMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleEditControllerMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleGroupMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleResetTransformMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleAlignToGridMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleSetTargetZoneMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleSetInfiniteTargetZoneMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleMoveToTargetZoneMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleConnectNodeMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleUnconnectNodeMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleConnectInfiniteZoneMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleSelectConnectedNodeMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleSelectIncomingConnectingNodesMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleMoveViewportCameraToNodeMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleOpenInstancedWorldMenuItem(Widget *menuItem, const WidgetEventData *eventData);

			void HandleRebuildGeometryMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleRebuildWithNewPathMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleRecalculateNormalsMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleBakeTransformIntoVerticesMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleRepositionMeshOriginMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleSetMaterialMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleRemoveMaterialMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleCombineDetailLevelsMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleSeparateDetailLevelsMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleConvertToGenericMeshMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleMergeGeometryMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleInvertGeometryMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleBooleanGeometryMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleGenerateAmbientOcclusionMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleRemoveAmbientOcclusionMenuItem(Widget *menuItem, const WidgetEventData *eventData);

			void HandleHideSelectionMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleUnhideAllMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleUnhideAllInTargetZoneMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleHideNonTargetZonesMenuItem(Widget *menuItem, const WidgetEventData *eventData);

			void HandleShowBackfacesMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleExpandWorldsMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleExpandModelsMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleRenderLightingMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleDrawFromCenterMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleCapGeometryMenuItem(Widget *menuItem, const WidgetEventData *eventData);

			void HandleShowViewportMenuItem(Widget *menuItem, const WidgetEventData *eventData);

			void HandleChangeViewportMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleFrameAllMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleFrameSelectionMenuItem(Widget *menuItem, const WidgetEventData *eventData);

			bool SaveWorld(bool strip = false);
			static void SavePickerProc(FilePicker *picker, void *cookie);
			static void CloseDialogComplete(Dialog *dialog, void *cookie);

			static void SceneImportPickerProc(FilePicker *picker, void *cookie);
			static void SceneExportPickerProc(FilePicker *picker, void *cookie);
			static void ModelLoadPickerProc(FilePicker *picker, void *cookie);
			static void ModelSavePickerProc(FilePicker *picker, void *cookie);

			void Paste(Node *targetNode);

			void EnableGizmo(const NodeReference *reference);
			void DisableGizmo(const NodeReference *reference);

			static void AssignGenericControllers(const ScriptObject *scriptObject, const ConnectorKey& connectorKey, Node *target, ConnectOperation *connectOperation);

			Node *PickNode(Node *root, Ray *ray, PickData *pickData, PickFilterProc *filterProc = nullptr, const void *filterCookie = nullptr) const;
			Node *PickHandle(int32 viewportIndex, Ray *ray, int32 *index) const;

			void ShowViewport(int32 index);

		public:

			typedef const EditorEvent& ObservableEventType;

			static const float kFrustumRenderScale;
			static const float kFrustumLineScaleMultiplier;

			Editor(const char *name);
			~Editor();

			const char *GetResourceName(void) const
			{
				return (resourceName);
			}

			unsigned_int32 GetEditorState(void) const
			{
				return (editorState);
			}

			void SetWorldUnsavedFlag(void)
			{
				editorState |= kEditorWorldUnsaved;
			}

			void InvalidateSelection(void)
			{
				editorState |= kEditorSelectionUpdated | kEditorRedrawViewports;
			}

			void SetProcessPropertiesFlag(void)
			{
				editorState |= kEditorProcessProperties;
			}

			void SetUpdateConditionalItemsFlag(void)
			{
				editorState |= kEditorUpdateConditionalItems;
			}

			unsigned_int32 GetGizmoState(void) const
			{
				return (gizmoState);
			}

			void SetGizmoState(unsigned_int32 state)
			{
				gizmoState = state;
			}

			unsigned_int32 GetRenderFlags(void) const
			{
				return (renderFlags);
			}

			void SetRenderFlags(unsigned_int32 flags)
			{
				renderFlags = flags;
				editorState |= kEditorRedrawViewports;
			}

			Cursor *GetCurrentCursor(void) const
			{
				return (currentCursor);
			}

			void SetCurrentCursor(Cursor *cursor)
			{
				currentCursor = cursor;
			}

			Zone *GetRootNode(void) const
			{
				return (static_cast<Zone *>(rootNode));
			}

			World *GetEditorWorld(void) const
			{
				return (editorWorld);
			}

			EditorObject *GetEditorObject(void) const
			{
				return (editorObject);
			}

			StandardEditorTool *GetStandardTool(int32 tool) const
			{
				return (standardTool[tool]);
			}

			NodeReparentTool *GetNodeReparentTool(void) const
			{
				return (nodeReparentTool);
			}

			EditorTool *GetCurrentTool(void) const
			{
				return (currentTool);
			}

			void InvalidateNode(Node *node)
			{
				node->Invalidate();
				editorState |= kEditorRedrawViewports;
			}

			const List<NodeReference> *GetSelectionList(void) const
			{
				return (&selectionList);
			}

			NodeReference *GetFirstSelectedNode(void) const
			{
				return (selectionList.First());
			}

			NodeReference *GetLastSelectedNode(void) const
			{
				return (selectionList.Last());
			}

			const List<EditorManipulator> *GetSelectedConnectorList(void) const
			{
				return (&selectedConnectorList);
			}

			const NodeReference *GetGizmoTarget(void) const
			{
				return (gizmoTarget);
			}

			void ClearPickFilterProc(void)
			{
				pickFilterProc = nullptr;
			}

			EditorTool *GetTrackingTool(void) const
			{
				return (trackingTool);
			}

			void SetTrackingTool(EditorTool *tool)
			{
				trackingTool = tool;
			}

			EditorTrackData *GetTrackData(void)
			{
				return (&editorTrackData);
			}

			void ResetScrollFraction(void)
			{
				scrollFraction.Set(0.0F, 0.0F);
			}

			EditorViewport *GetViewport(int32 index) const
			{
				return (editorViewport[index]);
			}

			void InvalidateViewport(int32 index)
			{
				editorViewport[index]->Invalidate();
			}

			void InvalidateAllViewports(void)
			{
				editorState |= kEditorRedrawViewports;
			}

			BookWidget *GetBookWidget(int32 index) const
			{
				return (bookWidget[index]);
			}

			void OpenNodeInfo(void)
			{
				HandleNodeInfoMenuItem(nullptr, nullptr);
			}

			NodeInfoType GetNodeInfoType(void) const
			{
				return (nodeInfoType);
			}

			Type GetNodeInfoCategory(void) const
			{
				return (nodeInfoCategory);
			}

			void SetNodeInfoData(NodeInfoType type, Type category)
			{
				nodeInfoType = type;
				nodeInfoCategory = category;
			}

			void InstallPlacementAdjuster(PlacementAdjusterProc *proc, void *cookie = nullptr)
			{
				placementAdjusterProc = proc;
				placementAdjusterCookie = cookie;
			}

			static EditorManipulator *GetManipulator(const Node *node)
			{
				return (static_cast<EditorManipulator *>(node->GetManipulator()));
			}

			static Transform4D& GetTransformClipboard(void)
			{
				return (transformClipboard);
			}

			static Editor *Open(const char *name);

			void PostEvent(const EditorEvent& event) override;

			C4EDITORAPI Node *FindNode(const char *name) const;

			C4EDITORAPI void SelectNode(Node *node);
			C4EDITORAPI void UnselectNode(Node *node);
			C4EDITORAPI void SelectAll(Node *root);
			C4EDITORAPI void UnselectAll(void);
			C4EDITORAPI void UnselectAllTemp(void);

			C4EDITORAPI void SelectConnector(Node *node, int32 index, bool toggle = false);
			C4EDITORAPI void UnselectAllConnectors(Node *exclude = nullptr);

			C4EDITORAPI void UnselectNonGeometryNodes(void);
			C4EDITORAPI void SelectAllCell(const Site *cell);
			C4EDITORAPI void SelectAllMask(Node *root);
			C4EDITORAPI void SelectWithMaterial(void);
			C4EDITORAPI void SelectAllFilter(bool (*filter)(const Node *, const void *), const void *cookie = nullptr);

			C4EDITORAPI void ShowNode(Node *node);
			C4EDITORAPI void HideNode(Node *node);

			C4EDITORAPI void InvalidateAllShaderData(void);

			C4EDITORAPI void ExpandWorld(Instance *instance, int32 depth = 0);
			C4EDITORAPI void ExpandAllWorlds(Node *root = nullptr, int32 depth = 0);
			C4EDITORAPI void CollapseAllWorlds(void);

			C4EDITORAPI void ExpandModel(Model *model);
			C4EDITORAPI void ExpandAllModels(void);
			C4EDITORAPI void CollapseAllModels(void);

			C4EDITORAPI void AddOperation(Operation *operation);
			C4EDITORAPI void DeleteLastOperation(void);

			C4EDITORAPI Zone *GetTargetZone(void) const;
			C4EDITORAPI void SetTargetZone(Zone *zone);

			C4EDITORAPI MaterialContainer *GetSelectedMaterial(void) const;
			C4EDITORAPI void SelectMaterial(MaterialContainer *materialContainer);

			C4EDITORAPI bool NodeSelectable(const Node *node, bool nonpersistent = false) const;
			C4EDITORAPI bool SetTrackPickFilter(EditorTrackData *trackData, PickFilterProc *proc, const void *cookie);

			Node *PickNode(EditorTrackData *trackData, PickData *pickData, PickFilterProc *filterProc, const void *filterCookie = nullptr) const;
			Node *PickHandle(EditorTrackData *trackData, int32 *index) const;
			Node *PickConnector(EditorTrackData *trackData, int32 *index) const;
			Node *PickGizmoArrow(EditorTrackData *trackData) const;
			Node *PickGizmoSquare(EditorTrackData *trackData) const;
			Node *PickGizmoCircle(EditorTrackData *trackData) const;

			EditorTrackData *CalculateTrackData(const ViewportWidget *viewport, const Point3D& position);

			C4EDITORAPI void InitNewNode(EditorTrackData *trackData, Node *node);
			C4EDITORAPI void CommitNewNode(EditorTrackData *trackData, bool commit);

			C4EDITORAPI void DeleteNode(Node *node, bool undoable = false);
			C4EDITORAPI void DeleteSubtree(Node *root);
			C4EDITORAPI void ReparentSelectedNodes(Node *newSuper);

			C4EDITORAPI void ImportScene(Node *root);

			void BuildViewport(int32 index);
			void UpdateViewportStructures(void);
			void InvalidateViewports(EditorViewportType viewportType);
			void ActivateViewportMenu(int32 index, const Point3D& position);
			C4EDITORAPI void FrameSelectionAllViewports(void);

			C4EDITORAPI float SnapToGrid(const float x) const;
			C4EDITORAPI Point2D SnapToGrid(const Point2D& p) const;
			C4EDITORAPI Point3D SnapToGrid(const Point3D& p) const;

			C4EDITORAPI void AdjustPlacement(Transform4D *transform, const Vector3D& normal) const;

			C4EDITORAPI static Point3D GetWorldSpacePosition(const EditorTrackData *trackData, const Point2D& position);
			C4EDITORAPI static Vector3D GetWorldSpaceDirection(const EditorTrackData *trackData, const Vector2D& direction);
			C4EDITORAPI static Vector3D GetWorldSpaceDirection(const EditorTrackData *trackData, const Vector3D& direction);

			C4EDITORAPI Point3D GetTargetSpacePosition(const EditorTrackData *trackData, const Point2D& position) const;
			C4EDITORAPI Vector3D GetTargetSpaceDirection(const EditorTrackData *trackData, const Vector3D& direction) const;

			C4EDITORAPI void AutoScroll(const EditorTrackData *trackData);
			C4EDITORAPI void SetCurrentTool(EditorTool *tool, void *cookie = nullptr);

			C4EDITORAPI void InvalidateGeometry(Geometry *geometry);
			C4EDITORAPI void RebuildGeometry(Geometry *geometry);
			C4EDITORAPI void RebuildGeometry(const List<NodeReference> *nodeList);

			C4EDITORAPI void RegenerateTexcoords(Geometry *geometry);
			C4EDITORAPI void RegenerateTexcoords(const List<NodeReference> *nodeList);

			C4EDITORAPI void OpenMaterialManager(void);

			C4EDITORAPI void DisplayError(const char *string);

			void Invalidate(void) override;

			void SetWidgetSize(const Vector2D& size) override;

			void Preprocess(void) override;
			void Move(void) override;

			void EnterForeground(void) override;
			void EnterBackground(void) override;

			bool HandleKeyboardEvent(const KeyboardEventData *eventData) override;
			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
			void Close(void) override;
	};


	template <class observerType> class EditorObserver : public Observer<observerType, Editor>
	{
		public:

			EditorObserver(observerType *observer, void (observerType::*callback)(Editor *, const EditorEvent&)) : Observer<observerType, Editor>(observer, callback)
			{
			}
	};


	class WorldEditor : public Plugin, public Singleton<WorldEditor>
	{
		private:

			Creator<Object>						objectCreator;
			Creator<Manipulator, Node *>		manipulatorCreator;

			StringTable							stringTable;
			Cursor								*editorCursor[kEditorCursorCount];

			ColorRGBA							sceneGraphColor;
			ColorRGBA							surfaceColor;

			VariableObserver<WorldEditor>		sceneGraphColorObserver;
			VariableObserver<WorldEditor>		surfaceColorObserver;

			CommandObserver<WorldEditor>		worldCommandObserver;
			CommandObserver<WorldEditor>		panelCommandObserver;
			CommandObserver<WorldEditor>		modelCommandObserver;
			CommandObserver<WorldEditor>		waterCommandObserver;
			Command								worldCommand;
			Command								panelCommand;
			Command								modelCommand;
			Command								waterCommand;

			CommandObserver<WorldEditor>		updateWorldsCommandObserver;
			CommandObserver<WorldEditor>		updateAnimsCommandObserver;
			Command								updateWorldsCommand;
			Command								updateAnimsCommand;

			MenuItemWidget						newWorldItem;
			MenuItemWidget						openWorldItem;
			MenuItemWidget						reopenWorldItem;
			MenuItemWidget						dividerLine1Item;
			MenuItemWidget						newPanelItem;
			MenuItemWidget						openPanelItem;
			MenuItemWidget						dividerLine2Item;
			MenuItemWidget						openModelItem;

			ResourceName						playedWorldName;

			Link<FilePicker>					worldPicker;
			Link<FilePicker>					panelPicker;
			Link<FilePicker>					modelPicker;

			List<EditorPlugin>					editorPluginList;
			List<SceneImportPlugin>				sceneImportPluginList;
			List<SceneExportPlugin>				sceneExportPluginList;

			static Object *CreateObject(Unpacker& data, unsigned_int32 unpackFlags);

			void HandleSceneGraphColorEvent(Variable *variable);
			void HandleSurfaceColorEvent(Variable *variable);

			void HandleNewWorldMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleOpenWorldMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleReopenWorldMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleWorldCommand(Command *command, const char *text);
			static void WorldPicked(FilePicker *picker, void *cookie);

			void HandleNewPanelMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleOpenPanelMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandlePanelCommand(Command *command, const char *text);
			static void PanelPicked(FilePicker *picker, void *cookie);

			void HandleOpenModelMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleModelCommand(Command *command, const char *text);
			static void ModelPicked(FilePicker *picker, void *cookie);

			void HandleWaterCommand(Command *command, const char *text);

			void HandleUpdateWorldsCommand(Command *command, const char *text);
			static void UpdateWorldsDirectory(const char *directory);
			static void UpdateWorldResource(const char *name);
			static void UpdateModelsDirectory(const char *directory);
			static void UpdateModelResource(const char *name);

			void HandleUpdateAnimsCommand(Command *command, const char *text);
			static void UpdateAnimsDirectory(const char *directory);
			static void UpdateAnimResource(const char *name);

		public:

			WorldEditor();
			~WorldEditor();

			const StringTable *GetStringTable(void) const
			{
				return (&stringTable);
			}

			Cursor *GetEditorCursor(int32 index) const
			{
				return (editorCursor[index]);
			}

			const ColorRGBA& GetSceneGraphColor(void) const
			{
				return (sceneGraphColor);
			}

			const ColorRGBA& GetSurfaceColor(void) const
			{
				return (surfaceColor);
			}

			void SetPlayedWorldName(const char *name)
			{
				playedWorldName = name;
				reopenWorldItem.Enable();
			}

			void RegisterEditorPlugin(EditorPlugin *plugin)
			{
				editorPluginList.Append(plugin);
			}

			void RegisterSceneImportPlugin(SceneImportPlugin *plugin)
			{
				sceneImportPluginList.Append(plugin);
			}

			void RegisterSceneExportPlugin(SceneExportPlugin *plugin)
			{
				sceneExportPluginList.Append(plugin);
			}

			const List<EditorPlugin> *GetEditorPluginList(void) const
			{
				return (&editorPluginList);
			}

			const List<SceneImportPlugin> *GetSceneImportPluginList(void) const
			{
				return (&sceneImportPluginList);
			}

			const List<SceneExportPlugin> *GetSceneExportPluginList(void) const
			{
				return (&sceneExportPluginList);
			}

			void InitializePlugins(EditorObject *editorObject) const;
			void PreprocessPlugins(Editor *editor) const;
	};


	C4EDITORAPI extern WorldEditor *TheWorldEditor;
}


#endif

// ZYUQURM
