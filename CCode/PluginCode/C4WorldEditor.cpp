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


#include "C4WorldEditor.h"
#include "C4EditorSupport.h"
#include "C4EditorGizmo.h"
#include "C4InstanceManipulators.h"
#include "C4ModelManipulators.h"
#include "C4ZoneManipulators.h"
#include "C4WorldEditor.h"
#include "C4ModelViewer.h"
#include "C4ScriptEditor.h"
#include "C4ShaderEditor.h"
#include "C4PanelEditor.h"
#include "C4WaterTools.h"
#include "C4Primitives.h"
#include "C4Terrain.h"
#include "C4Water.h"
#include "C4World.h"
#include "C4Dialog.h"
#include "C4Application.h"


using namespace C4;


namespace
{
	enum
	{
		kMaxEditorOperationCount = 64
	};


	const char *cursorTextureName[kEditorCursorCount] =
	{
		"C4/arrow",
		"WorldEditor/cursor/Lock",
		"WorldEditor/cursor/Hand",
		"WorldEditor/cursor/Drag",
		"WorldEditor/cursor/Glass",
		"WorldEditor/cursor/Cross",
		"WorldEditor/cursor/Dropper",
		"WorldEditor/cursor/Insert",
		"WorldEditor/cursor/Remove",
		"WorldEditor/cursor/Free",
		"WorldEditor/cursor/Light",
		"WorldEditor/cursor/Stop"
	};
}


WorldEditor *C4::TheWorldEditor = nullptr;


const float Editor::kFrustumRenderScale = 0.0078125F;
const float Editor::kFrustumLineScaleMultiplier = 0.109375F;


Package				*Editor::editorClipboard = nullptr;
Transform4D			Editor::transformClipboard;
List<Editor>		Editor::windowList;


C4::Plugin *CreatePlugin(void)
{
	return (new WorldEditor);
}


Editor::Editor(const char *name) : Window("WorldEditor/Window")
{
	windowList.Append(this);

	SetMinWindowSize(Vector2D(497.0F + kEditorPageWidth, 558.0F));
	SetStripIcon("WorldEditor/window");

	resourceName[0] = 0;
	rootNode = nullptr;
	editorWorld = nullptr;
	editorObject = nullptr;

	if (name)
	{
		WorldResource *resource = WorldResource::Get(name, kResourceIgnorePackFiles, nullptr, &resourceLocation);
		if (!resource)
		{
			return;
		}

		rootNode = Node::UnpackTree(resource->GetData(), kUnpackEditor);
		resource->Release();

		Object *object = static_cast<InfiniteZone *>(rootNode)->GetAuxiliaryObject();
		if ((object) && (object->GetObjectType() == kObjectEditor))
		{
			editorObject = static_cast<EditorObject *>(object);
		}

		resourceName = name;
		ResourcePath title(name);
		SetWindowTitle(title += WorldResource::GetDescriptor()->GetExtension());
		SetStripTitle(&title[Text::GetDirectoryPathLength(title)]);
	} 
	else
	{
		rootNode = new InfiniteZone; 
		InfiniteZoneObject *object = new InfiniteZoneObject;
		rootNode->SetObject(object); 
		object->Release();
	}
 
	if (!editorObject)
	{ 
		EditorObject *object = new EditorObject; 
		static_cast<InfiniteZone *>(rootNode)->SetAuxiliaryObject(object);
		editorObject = object;
		object->Release();
	} 

	if (!GetTargetZone())
	{
		editorObject->SetTargetZone(GetRootNode());
	}

	EditorManipulator::Install(this, rootNode);
	static_cast<ZoneManipulator *>(GetManipulator(GetTargetZone()))->SetTarget(true);

	editorWorld = new World(rootNode, kWorldViewport | kWorldClearColor | kWorldMotionBlurInhibit | kWorldListenerInhibit);
	editorWorld->Preprocess();

	for (machine a = 0; a < kEditorToolCount; a++)
	{
		standardTool[a] = nullptr;
	}

	nodeReparentTool = nullptr;

	TheEngine->StartMultiaxisMouse();
	TheEngine->StartTablet();
}

Editor::~Editor()
{
	if (editorWorld)
	{
		TheInterfaceMgr->SetCursor(nullptr);

		TheEngine->StopTablet();
		TheEngine->StopMultiaxisMouse();

		PurgeSubwindows();
		operationList.Purge();

		delete editorWorld;

		for (machine a = kEditorViewportCount - 1; a >= 0; a--)
		{
			delete editorViewport[a];
		}

		delete nodeReparentTool;

		for (machine a = kEditorToolCount - 1; a >= 0; a--)
		{
			delete standardTool[a];
		}

		if ((windowList.First() == this) && (windowList.Last() == this))
		{
			delete editorClipboard;
			editorClipboard = nullptr;

			ScriptEditor::ReleaseClipboard();
			ShaderEditor::ReleaseClipboard();
		}
	}
}

Editor *Editor::Open(const char *name)
{
	Editor *window = nullptr;

	if (name)
	{
		window = windowList.First();
		while (window)
		{
			if (window->resourceName == name)
			{
				TheInterfaceMgr->SetActiveWindow(window);
				return (window);
			}

			window = window->ListElement<Editor>::Next();
		}
	}

	Editor *editor = new Editor(name);
	if ((name) && (editor->GetResourceName()[0] == 0))
	{
		delete editor;
		return (nullptr);
	}

	TheInterfaceMgr->AddWidget(editor);
	return (editor);
}

void Editor::SetWidgetSize(const Vector2D& size)
{
	Window::SetWidgetSize(size);
	PositionWidgets();
}

void Editor::Preprocess(void)
{
	static const char *const toolIdentifier[kEditorToolCount] =
	{
		"BoxSelect", "Select", "Move", "Rotate", "Resize", "Connect", "Surface", "Scroll", "Zoom", "BoxZoom", "Orbit", "Camera"
	};

	static const char *const flagIdentifier[kEditorFlagCount] =
	{
		"Backface", "World", "Model", "Light", "Center", "Cap"
	};

	Window::Preprocess();

	bookPane = static_cast<MultipaneWidget *>(FindWidget("Books"));
	toolGroup = FindWidget("Tools");

	for (machine a = 0; a < kEditorToolCount; a++)
	{
		toolButton[a] = static_cast<IconButtonWidget *>(FindWidget(toolIdentifier[a]));
	}

	for (machine a = 0; a < kEditorFlagCount; a++)
	{
		flagButton[a] = static_cast<IconButtonWidget *>(FindWidget(flagIdentifier[a]));
	}

	menuBar = static_cast<MenuBarWidget *>(FindWidget("Menu"));
	BuildMenus();

	editorState = 0;
	gizmoState = 0;
	renderFlags = 0;
	nodeInfoType = 0xFFFFFFFF;
	nodeInfoCategory = 0xFFFFFFFF;

	currentCursor = nullptr;
	gizmoTarget = nullptr;
	pickFilterProc = nullptr;
	placementAdjusterProc = nullptr;
	trackingTool = nullptr;

	standardTool[kEditorToolNodeBoxSelect] = new BoxSelectTool(toolButton[kEditorToolNodeBoxSelect]);
	standardTool[kEditorToolNodeSelect] = new NodeSelectTool(toolButton[kEditorToolNodeSelect]);
	standardTool[kEditorToolNodeMove] = new NodeMoveTool(toolButton[kEditorToolNodeMove]);
	standardTool[kEditorToolNodeRotate] = new NodeRotateTool(toolButton[kEditorToolNodeRotate]);
	standardTool[kEditorToolNodeResize] = new NodeScaleTool(toolButton[kEditorToolNodeResize]);
	standardTool[kEditorToolConnect] = new ConnectTool(toolButton[kEditorToolConnect]);
	standardTool[kEditorToolSurfaceSelect] = new SurfaceSelectTool(toolButton[kEditorToolSurfaceSelect]);
	standardTool[kEditorToolViewportScroll] = new ViewportScrollTool(toolButton[kEditorToolViewportScroll]);
	standardTool[kEditorToolViewportZoom] = new ViewportZoomTool(toolButton[kEditorToolViewportZoom]);
	standardTool[kEditorToolViewportBoxZoom] = new ViewportBoxZoomTool(toolButton[kEditorToolViewportBoxZoom]);
	standardTool[kEditorToolOrbitCamera] = new OrbitCameraTool(toolButton[kEditorToolOrbitCamera]);
	standardTool[kEditorToolFreeCamera] = new FreeCameraTool(toolButton[kEditorToolFreeCamera]);

	nodeReparentTool = new NodeReparentTool;

	currentTool = standardTool[kEditorToolNodeSelect];
	standardTool[kEditorToolNodeSelect]->Engage(this);

	unsigned_int32 editorFlags = editorObject->GetEditorFlags();

	if (editorFlags & kEditorShowBackfaces)
	{
		flagButton[kEditorFlagBackfaces]->SetValue(1);
	}

	if (editorFlags & kEditorRenderLighting)
	{
		flagButton[kEditorFlagLighting]->SetValue(1);
	}

	if (editorFlags & kEditorDrawFromCenter)
	{
		flagButton[kEditorFlagCenter]->SetValue(1);
	}

	if (editorFlags & kEditorCapGeometry)
	{
		flagButton[kEditorFlagCap]->SetValue(1);
	}

	if (editorFlags & kEditorExpandWorlds)
	{
		flagButton[kEditorFlagExpandWorlds]->SetValue(1);
		ExpandAllWorlds();
	}

	if (editorFlags & kEditorExpandModels)
	{
		flagButton[kEditorFlagExpandModels]->SetValue(1);
		ExpandAllModels();
	}

	for (machine a = 0; a < kEditorViewportCount; a++)
	{
		editorViewport[a] = nullptr;
		BuildViewport(a);
	}

	for (machine a = 0; a < kEditorBookCount; a++)
	{
		BookWidget *book = new BookWidget(Vector2D(kEditorPageWidth, bookPane->GetWidgetSize().y), 2.0F, 4.0F);
		book->SetWidgetPosition(Point3D(3.0F, 1.0F, 0.0F));
		bookWidget[a] = book;

		if (a != 0)
		{
			book->Hide();
		}
	}

	editorObject->Preprocess(this);

	EditorPage *page = editorObject->GetFirstEditorPage();
	while (page)
	{
		bookWidget[page->GetBookIndex()]->AppendPage(page);
		page = page->ListElement<EditorPage>::Next();
	}

	for (machine a = 0; a < kEditorBookCount; a++)
	{
		bookPane->AppendNewSubnode(bookWidget[a]);
		bookWidget[a]->OrganizePages();
	}

	PositionWidgets();

	TheWorldEditor->PreprocessPlugins(this);
}

void Editor::PositionWidgets(void)
{
	float menuBarHeight = menuBar->GetWidgetSize().y;
	menuBar->SetWidgetSize(Vector2D(GetWidgetSize().x, menuBarHeight));

	bookPane->SetWidgetSize(Vector2D(kEditorPageWidth + 6.0F, GetWidgetSize().y - menuBarHeight - 27.0F));

	for (machine a = 0; a < kEditorBookCount; a++)
	{
		bookWidget[a]->SetWidgetSize(Vector2D(kEditorPageWidth, GetWidgetSize().y - menuBarHeight - 29.0F));
	}

	UpdateViewportStructures();
}

void Editor::BuildMenus(void)
{
	const StringTable *table = TheWorldEditor->GetStringTable();

	// World Menu

	worldMenu = new PulldownMenuWidget(table->GetString(StringID('MENU', 'WRLD')));
	menuBar->AppendMenu(worldMenu);

	worldMenu->AppendMenuItem(new MenuItemWidget(table->GetString(StringID('MENU', 'WRLD', 'CLOS')), WidgetObserver<Editor>(this, &Editor::HandleCloseMenuItem), Shortcut('W')));
	worldMenu->AppendMenuItem(new MenuItemWidget(table->GetString(StringID('MENU', 'WRLD', 'SAVW')), WidgetObserver<Editor>(this, &Editor::HandleSaveWorldMenuItem), Shortcut('S')));
	worldMenu->AppendMenuItem(new MenuItemWidget(table->GetString(StringID('MENU', 'WRLD', 'SVAS')), WidgetObserver<Editor>(this, &Editor::HandleSaveWorldAsMenuItem), Shortcut('S', kShortcutShift)));

	worldMenu->AppendMenuItem(new MenuItemWidget(kLineSolid));
	worldMenu->AppendMenuItem(new MenuItemWidget(table->GetString(StringID('MENU', 'WRLD', 'PLAY')), WidgetObserver<Editor>(this, &Editor::HandleSaveAndPlayWorldMenuItem), Shortcut('P')));
	worldMenu->AppendMenuItem(new MenuItemWidget(kLineSolid));

	MenuItemWidget *widget = new MenuItemWidget(table->GetString(StringID('MENU', 'WRLD', 'ISCN')), WidgetObserver<Editor>(this, &Editor::HandleImportSceneMenuItem));
	if (TheWorldEditor->GetSceneImportPluginList()->Empty())
	{
		widget->Disable();
	}

	worldMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'WRLD', 'ESCN')), WidgetObserver<Editor>(this, &Editor::HandleExportSceneMenuItem));
	if (TheWorldEditor->GetSceneExportPluginList()->Empty())
	{
		widget->Disable();
	}

	worldMenu->AppendMenuItem(widget);

	worldMenu->AppendMenuItem(new MenuItemWidget(kLineSolid));
	worldMenu->AppendMenuItem(new MenuItemWidget(table->GetString(StringID('MENU', 'WRLD', 'IMDL')), WidgetObserver<Editor>(this, &Editor::HandleLoadModelResourceMenuItem)));
	worldMenu->AppendMenuItem(new MenuItemWidget(table->GetString(StringID('MENU', 'WRLD', 'EMDL')), WidgetObserver<Editor>(this, &Editor::HandleSaveModelResourceMenuItem)));

	// Edit Menu

	editMenu = new PulldownMenuWidget(table->GetString(StringID('MENU', 'EDIT')));
	menuBar->AppendMenu(editMenu);

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'EDIT', 'UNDO')), WidgetObserver<Editor>(this, &Editor::HandleUndoMenuItem), Shortcut('Z'));
	editorMenuItem[kEditorMenuUndo] = widget;
	widget->Disable();
	editMenu->AppendMenuItem(widget);

	editMenu->AppendMenuItem(new MenuItemWidget(kLineSolid));

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'EDIT', 'CUT ')), WidgetObserver<Editor>(this, &Editor::HandleCutMenuItem), Shortcut('X'));
	editorMenuItem[kEditorMenuCut] = widget;
	editMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'EDIT', 'COPY')), WidgetObserver<Editor>(this, &Editor::HandleCopyMenuItem), Shortcut('C'));
	editorMenuItem[kEditorMenuCopy] = widget;
	editMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'EDIT', 'PAST')), WidgetObserver<Editor>(this, &Editor::HandlePasteMenuItem), Shortcut('V'));
	editorMenuItem[kEditorMenuPaste] = widget;
	editMenu->AppendMenuItem(widget);

	if (!editorClipboard)
	{
		widget->Disable();
	}

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'EDIT', 'PSUB')), WidgetObserver<Editor>(this, &Editor::HandlePasteSubnodesMenuItem), Shortcut('V', kShortcutShift));
	editorMenuItem[kEditorMenuPasteSubnodes] = widget;
	editMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'EDIT', 'CLER')), WidgetObserver<Editor>(this, &Editor::HandleClearMenuItem), Shortcut(kKeyCodeDelete, kShortcutUnmodified));
	editorMenuItem[kEditorMenuClear] = widget;
	editMenu->AppendMenuItem(widget);

	editMenu->AppendMenuItem(new MenuItemWidget(kLineSolid));
	editMenu->AppendMenuItem(new MenuItemWidget(table->GetString(StringID('MENU', 'EDIT', 'SALL')), WidgetObserver<Editor>(this, &Editor::HandleSelectAllMenuItem), Shortcut('A')));

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'EDIT', 'SZON')), WidgetObserver<Editor>(this, &Editor::HandleSelectAllZoneMenuItem));
	editorMenuItem[kEditorMenuSelectAllZone] = widget;
	editMenu->AppendMenuItem(widget);

	editMenu->AppendMenuItem(new MenuItemWidget(table->GetString(StringID('MENU', 'EDIT', 'SMSK')), WidgetObserver<Editor>(this, &Editor::HandleSelectAllMaskMenuItem), Shortcut('A', kShortcutShift)));
	editMenu->AppendMenuItem(new MenuItemWidget(table->GetString(StringID('MENU', 'EDIT', 'SMAT')), WidgetObserver<Editor>(this, &Editor::HandleSelectMaterialMenuItem)));

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'EDIT', 'SUPR')), WidgetObserver<Editor>(this, &Editor::HandleSelectSuperNodeMenuItem), Shortcut(kKeyCodeUpArrow));
	editorMenuItem[kEditorMenuSelectSuperNode] = widget;
	editMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'EDIT', 'TREE')), WidgetObserver<Editor>(this, &Editor::HandleSelectSubtreeMenuItem), Shortcut(kKeyCodeDownArrow));
	editorMenuItem[kEditorMenuSelectSubtree] = widget;
	editMenu->AppendMenuItem(widget);

	editMenu->AppendMenuItem(new MenuItemWidget(kLineSolid));

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'EDIT', 'LOCK')), WidgetObserver<Editor>(this, &Editor::HandleLockSelectionMenuItem), Shortcut('K'));
	editorMenuItem[kEditorMenuLockSelection] = widget;
	editMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'EDIT', 'ULCK')), WidgetObserver<Editor>(this, &Editor::HandleUnlockSelectionMenuItem), Shortcut('K', kShortcutShift));
	editorMenuItem[kEditorMenuUnlockSelection] = widget;
	editMenu->AppendMenuItem(widget);

	editMenu->AppendMenuItem(new MenuItemWidget(kLineSolid));

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'EDIT', 'DUPL')), WidgetObserver<Editor>(this, &Editor::HandleDuplicateMenuItem), Shortcut('D'));
	editorMenuItem[kEditorMenuDuplicate] = widget;
	editMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'EDIT', 'CLON')), WidgetObserver<Editor>(this, &Editor::HandleCloneMenuItem), Shortcut('D', kShortcutShift));
	editorMenuItem[kEditorMenuClone] = widget;
	editMenu->AppendMenuItem(widget);

	// Node Menu

	nodeMenu = new PulldownMenuWidget(table->GetString(StringID('MENU', 'NODE')));
	menuBar->AppendMenu(nodeMenu);

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'NODE', 'INFO')), WidgetObserver<Editor>(this, &Editor::HandleNodeInfoMenuItem), Shortcut('I'));
	editorMenuItem[kEditorMenuGetInfo] = widget;
	nodeMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'NODE', 'EDIT')), WidgetObserver<Editor>(this, &Editor::HandleEditControllerMenuItem), Shortcut('E'));
	editorMenuItem[kEditorMenuEditController] = widget;
	nodeMenu->AppendMenuItem(widget);

	nodeMenu->AppendMenuItem(new MenuItemWidget(kLineSolid));

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'NODE', 'GRUP')), WidgetObserver<Editor>(this, &Editor::HandleGroupMenuItem), Shortcut('G'));
	editorMenuItem[kEditorMenuGroup] = widget;
	nodeMenu->AppendMenuItem(widget);

	nodeMenu->AppendMenuItem(new MenuItemWidget(kLineSolid));

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'NODE', 'RXFM')), WidgetObserver<Editor>(this, &Editor::HandleResetTransformMenuItem));
	editorMenuItem[kEditorMenuResetTransform] = widget;
	nodeMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'NODE', 'AGRD')), WidgetObserver<Editor>(this, &Editor::HandleAlignToGridMenuItem));
	editorMenuItem[kEditorMenuAlignToGrid] = widget;
	nodeMenu->AppendMenuItem(widget);

	nodeMenu->AppendMenuItem(new MenuItemWidget(kLineSolid));

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'NODE', 'TARG')), WidgetObserver<Editor>(this, &Editor::HandleSetTargetZoneMenuItem), Shortcut('T'));
	editorMenuItem[kEditorMenuSetTargetZone] = widget;
	nodeMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'NODE', 'ITRG')), WidgetObserver<Editor>(this, &Editor::HandleSetInfiniteTargetZoneMenuItem), Shortcut('Y'));
	nodeMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'NODE', 'MZON')), WidgetObserver<Editor>(this, &Editor::HandleMoveToTargetZoneMenuItem), Shortcut('T', kShortcutShift));
	editorMenuItem[kEditorMenuMoveToTargetZone] = widget;
	nodeMenu->AppendMenuItem(widget);

	nodeMenu->AppendMenuItem(new MenuItemWidget(kLineSolid));

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'NODE', 'CONN')), WidgetObserver<Editor>(this, &Editor::HandleConnectNodeMenuItem), Shortcut('L'));
	editorMenuItem[kEditorMenuConnectNode] = widget;
	nodeMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'NODE', 'UCON')), WidgetObserver<Editor>(this, &Editor::HandleUnconnectNodeMenuItem), Shortcut('U'));
	editorMenuItem[kEditorMenuUnconnectNode] = widget;
	nodeMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'NODE', 'CIZN')), WidgetObserver<Editor>(this, &Editor::HandleConnectInfiniteZoneMenuItem));
	editorMenuItem[kEditorMenuConnectInfiniteZone] = widget;
	nodeMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'NODE', 'SCON')), WidgetObserver<Editor>(this, &Editor::HandleSelectConnectedNodeMenuItem));
	editorMenuItem[kEditorMenuSelectConnectedNode] = widget;
	nodeMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'NODE', 'SICN')), WidgetObserver<Editor>(this, &Editor::HandleSelectIncomingConnectingNodesMenuItem));
	editorMenuItem[kEditorMenuSelectIncomingConnectingNodes] = widget;
	nodeMenu->AppendMenuItem(widget);

	nodeMenu->AppendMenuItem(new MenuItemWidget(kLineSolid));

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'NODE', 'MCAM')), WidgetObserver<Editor>(this, &Editor::HandleMoveViewportCameraToNodeMenuItem));
	editorMenuItem[kEditorMenuMoveViewportCameraToNode] = widget;
	nodeMenu->AppendMenuItem(widget);

	nodeMenu->AppendMenuItem(new MenuItemWidget(kLineSolid));

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'NODE', 'OINS')), WidgetObserver<Editor>(this, &Editor::HandleOpenInstancedWorldMenuItem), Shortcut('O', kShortcutShift));
	editorMenuItem[kEditorMenuOpenInstancedWorld] = widget;
	nodeMenu->AppendMenuItem(widget);

	// Geometry Menu

	geometryMenu = new PulldownMenuWidget(table->GetString(StringID('MENU', 'GEOM')));
	menuBar->AppendMenu(geometryMenu);

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'GEOM', 'RBLD')), WidgetObserver<Editor>(this, &Editor::HandleRebuildGeometryMenuItem), Shortcut('R'));
	editorMenuItem[kEditorMenuRebuildGeometry] = widget;
	geometryMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'GEOM', 'RBPA')), WidgetObserver<Editor>(this, &Editor::HandleRebuildWithNewPathMenuItem), Shortcut('R', kShortcutShift));
	editorMenuItem[kEditorMenuRebuildWithNewPath] = widget;
	geometryMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'GEOM', 'NORM')), WidgetObserver<Editor>(this, &Editor::HandleRecalculateNormalsMenuItem));
	editorMenuItem[kEditorMenuRecalculateNormals] = widget;
	geometryMenu->AppendMenuItem(widget);

	geometryMenu->AppendMenuItem(new MenuItemWidget(kLineSolid));

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'GEOM', 'BAKE')), WidgetObserver<Editor>(this, &Editor::HandleBakeTransformIntoVerticesMenuItem), Shortcut('B', kShortcutShift));
	editorMenuItem[kEditorMenuBakeTransformIntoVertices] = widget;
	geometryMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'GEOM', 'ORIG')), WidgetObserver<Editor>(this, &Editor::HandleRepositionMeshOriginMenuItem));
	editorMenuItem[kEditorMenuRepositionMeshOrigin] = widget;
	geometryMenu->AppendMenuItem(widget);

	geometryMenu->AppendMenuItem(new MenuItemWidget(kLineSolid));

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'GEOM', 'SMAT')), WidgetObserver<Editor>(this, &Editor::HandleSetMaterialMenuItem), Shortcut('M'));
	editorMenuItem[kEditorMenuSetMaterial] = widget;
	geometryMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'GEOM', 'RMAT')), WidgetObserver<Editor>(this, &Editor::HandleRemoveMaterialMenuItem));
	editorMenuItem[kEditorMenuRemoveMaterial] = widget;
	geometryMenu->AppendMenuItem(widget);

	geometryMenu->AppendMenuItem(new MenuItemWidget(kLineSolid));

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'GEOM', 'CBLV')), WidgetObserver<Editor>(this, &Editor::HandleCombineDetailLevelsMenuItem));
	editorMenuItem[kEditorMenuCombineDetailLevels] = widget;
	geometryMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'GEOM', 'SPLV')), WidgetObserver<Editor>(this, &Editor::HandleSeparateDetailLevelsMenuItem));
	editorMenuItem[kEditorMenuSeparateDetailLevels] = widget;
	geometryMenu->AppendMenuItem(widget);

	geometryMenu->AppendMenuItem(new MenuItemWidget(kLineSolid));

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'GEOM', 'CONV')), WidgetObserver<Editor>(this, &Editor::HandleConvertToGenericMeshMenuItem), Shortcut('C', kShortcutShift));
	editorMenuItem[kEditorMenuConvertToGenericMesh] = widget;
	geometryMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'GEOM', 'MERG')), WidgetObserver<Editor>(this, &Editor::HandleMergeGeometryMenuItem), Shortcut('M', kShortcutShift));
	editorMenuItem[kEditorMenuMergeGeometry] = widget;
	geometryMenu->AppendMenuItem(widget);

	geometryMenu->AppendMenuItem(new MenuItemWidget(kLineSolid));

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'GEOM', 'IVRT')), WidgetObserver<Editor>(this, &Editor::HandleInvertGeometryMenuItem), Shortcut('I', kShortcutShift));
	editorMenuItem[kEditorMenuInvertGeometry] = widget;
	geometryMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'GEOM', 'SECT')), WidgetObserver<Editor>(this, &Editor::HandleBooleanGeometryMenuItem), Shortcut('J', kShortcutShift));
	editorMenuItem[kEditorMenuIntersectGeometry] = widget;
	geometryMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'GEOM', 'UNON')), WidgetObserver<Editor>(this, &Editor::HandleBooleanGeometryMenuItem), Shortcut('U', kShortcutShift));
	editorMenuItem[kEditorMenuUnionGeometry] = widget;
	geometryMenu->AppendMenuItem(widget);

	geometryMenu->AppendMenuItem(new MenuItemWidget(kLineSolid));

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'GEOM', 'GOCC')), WidgetObserver<Editor>(this, &Editor::HandleGenerateAmbientOcclusionMenuItem));
	editorMenuItem[kEditorMenuGenerateAmbientOcclusion] = widget;
	geometryMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'GEOM', 'ROCC')), WidgetObserver<Editor>(this, &Editor::HandleRemoveAmbientOcclusionMenuItem));
	editorMenuItem[kEditorMenuRemoveAmbientOcclusion] = widget;
	geometryMenu->AppendMenuItem(widget);

	// View Menu

	viewMenu = new PulldownMenuWidget(table->GetString(StringID('MENU', 'VIEW')));
	menuBar->AppendMenu(viewMenu);

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'VIEW', 'HIDE')), WidgetObserver<Editor>(this, &Editor::HandleHideSelectionMenuItem), Shortcut('H'));
	editorMenuItem[kEditorMenuHideSelection] = widget;
	viewMenu->AppendMenuItem(widget);

	viewMenu->AppendMenuItem(new MenuItemWidget(table->GetString(StringID('MENU', 'VIEW', 'UHID')), WidgetObserver<Editor>(this, &Editor::HandleUnhideAllMenuItem), Shortcut('H', kShortcutShift)));

	viewMenu->AppendMenuItem(new MenuItemWidget(kLineSolid));
	viewMenu->AppendMenuItem(new MenuItemWidget(table->GetString(StringID('MENU', 'VIEW', 'UHTZ')), WidgetObserver<Editor>(this, &Editor::HandleUnhideAllInTargetZoneMenuItem)));
	viewMenu->AppendMenuItem(new MenuItemWidget(table->GetString(StringID('MENU', 'VIEW', 'HNTG')), WidgetObserver<Editor>(this, &Editor::HandleHideNonTargetZonesMenuItem)));

	// Layout Menu

	layoutMenu = new PulldownMenuWidget(table->GetString(StringID('MENU', 'LAYO')));
	menuBar->AppendMenu(layoutMenu);

	unsigned_int32 editorFlags = editorObject->GetEditorFlags();

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'LAYO', 'BFAC')), WidgetObserver<Editor>(this, &Editor::HandleShowBackfacesMenuItem));
	editorMenuItem[kEditorMenuShowBackfaces] = widget;
	if (editorFlags & kEditorShowBackfaces)
	{
		widget->ShowBullet();
	}

	layoutMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'LAYO', 'EXWD')), WidgetObserver<Editor>(this, &Editor::HandleExpandWorldsMenuItem));
	editorMenuItem[kEditorMenuExpandWorlds] = widget;
	if (editorFlags & kEditorExpandWorlds)
	{
		widget->ShowBullet();
	}

	layoutMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'LAYO', 'EXMD')), WidgetObserver<Editor>(this, &Editor::HandleExpandModelsMenuItem));
	editorMenuItem[kEditorMenuExpandModels] = widget;
	if (editorFlags & kEditorExpandModels)
	{
		widget->ShowBullet();
	}

	layoutMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'LAYO', 'LGHT')), WidgetObserver<Editor>(this, &Editor::HandleRenderLightingMenuItem));
	editorMenuItem[kEditorMenuRenderLighting] = widget;
	if (editorFlags & kEditorRenderLighting)
	{
		widget->ShowBullet();
	}

	layoutMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'LAYO', 'CENT')), WidgetObserver<Editor>(this, &Editor::HandleDrawFromCenterMenuItem));
	editorMenuItem[kEditorMenuDrawFromCenter] = widget;
	if (editorFlags & kEditorDrawFromCenter)
	{
		widget->ShowBullet();
	}

	layoutMenu->AppendMenuItem(widget);

	widget = new MenuItemWidget(table->GetString(StringID('MENU', 'LAYO', 'CAPS')), WidgetObserver<Editor>(this, &Editor::HandleCapGeometryMenuItem));
	editorMenuItem[kEditorMenuCapGeometry] = widget;
	if (editorFlags & kEditorCapGeometry)
	{
		widget->ShowBullet();
	}

	layoutMenu->AppendMenuItem(widget);
	layoutMenu->AppendMenuItem(new MenuItemWidget(kLineSolid));

	for (machine a = 0; a < kEditorViewportCount; a++)
	{
		widget = new MenuItemWidget(table->GetString(StringID('MENU', 'LAYO', 'PRT1' + a)), WidgetObserver<Editor>(this, &Editor::HandleShowViewportMenuItem), Shortcut('1' + a));
		showViewportItem[a] = widget;
		layoutMenu->AppendMenuItem(widget);
	}

	BuildViewportMenu();

	DisableSelectionItems();
}

void Editor::BuildViewportMenu(void)
{
	const StringTable *table = TheWorldEditor->GetStringTable();

	for (machine a = 0; a < kViewportModeCount; a++)
	{
		viewportMenuItemList.Append(new MenuItemWidget(table->GetString(StringID('MENU', 'VPRT', EditorViewport::viewportIdentifier[a])), WidgetObserver<Editor>(this, &Editor::HandleChangeViewportMenuItem)));
	}

	viewportMenuItemList.Append(new MenuItemWidget(kLineSolid));
	viewportMenuItemList.Append(new MenuItemWidget(table->GetString(StringID('MENU', 'VPRT', 'FALL')), WidgetObserver<Editor>(this, &Editor::HandleFrameAllMenuItem)));
	viewportMenuItemList.Append(new MenuItemWidget(table->GetString(StringID('MENU', 'VPRT', 'FSEL')), WidgetObserver<Editor>(this, &Editor::HandleFrameSelectionMenuItem)));
	viewportMenuItemList.Append(new MenuItemWidget(kLineSolid));
	viewportMenuItemList.Append(new MenuItemWidget(table->GetString(StringID('MENU', 'NODE', 'INFO')), WidgetObserver<Editor>(this, &Editor::HandleNodeInfoMenuItem), Shortcut('I')));
}

void Editor::EnableSelectionItems(void)
{
	editorMenuItem[kEditorMenuCut]->Enable();
	editorMenuItem[kEditorMenuCopy]->Enable();
	editorMenuItem[kEditorMenuClear]->Enable();
	editorMenuItem[kEditorMenuSelectSubtree]->Enable();
	editorMenuItem[kEditorMenuSelectSuperNode]->Enable();
	editorMenuItem[kEditorMenuDuplicate]->Enable();
	editorMenuItem[kEditorMenuClone]->Enable();

	editorMenuItem[kEditorMenuGetInfo]->Enable();
	editorMenuItem[kEditorMenuResetTransform]->Enable();
	editorMenuItem[kEditorMenuAlignToGrid]->Enable();
	editorMenuItem[kEditorMenuSelectIncomingConnectingNodes]->Enable();

	editorMenuItem[kEditorMenuHideSelection]->Enable();

	UpdateConditionalItems();
}

void Editor::DisableSelectionItems(void)
{
	editorMenuItem[kEditorMenuCut]->Disable();
	editorMenuItem[kEditorMenuCopy]->Disable();
	editorMenuItem[kEditorMenuPasteSubnodes]->Disable();
	editorMenuItem[kEditorMenuClear]->Disable();
	editorMenuItem[kEditorMenuSelectAllZone]->Disable();
	editorMenuItem[kEditorMenuSelectSubtree]->Disable();
	editorMenuItem[kEditorMenuSelectSuperNode]->Disable();
	editorMenuItem[kEditorMenuLockSelection]->Disable();
	editorMenuItem[kEditorMenuUnlockSelection]->Disable();
	editorMenuItem[kEditorMenuDuplicate]->Disable();
	editorMenuItem[kEditorMenuClone]->Disable();

	editorMenuItem[kEditorMenuGetInfo]->Disable();
	editorMenuItem[kEditorMenuEditController]->Disable();
	editorMenuItem[kEditorMenuGroup]->Disable();
	editorMenuItem[kEditorMenuResetTransform]->Disable();
	editorMenuItem[kEditorMenuAlignToGrid]->Disable();
	editorMenuItem[kEditorMenuSetTargetZone]->Disable();
	editorMenuItem[kEditorMenuMoveToTargetZone]->Disable();
	editorMenuItem[kEditorMenuConnectNode]->Disable();
	editorMenuItem[kEditorMenuConnectInfiniteZone]->Disable();

	if (selectedConnectorList.Empty())
	{
		editorMenuItem[kEditorMenuUnconnectNode]->Disable();
		editorMenuItem[kEditorMenuSelectConnectedNode]->Disable();
	}

	editorMenuItem[kEditorMenuSelectIncomingConnectingNodes]->Disable();
	editorMenuItem[kEditorMenuMoveViewportCameraToNode]->Disable();
	editorMenuItem[kEditorMenuOpenInstancedWorld]->Disable();

	editorMenuItem[kEditorMenuRebuildGeometry]->Disable();
	editorMenuItem[kEditorMenuRebuildWithNewPath]->Disable();
	editorMenuItem[kEditorMenuRecalculateNormals]->Disable();
	editorMenuItem[kEditorMenuBakeTransformIntoVertices]->Disable();
	editorMenuItem[kEditorMenuRepositionMeshOrigin]->Disable();
	editorMenuItem[kEditorMenuSetMaterial]->Disable();
	editorMenuItem[kEditorMenuRemoveMaterial]->Disable();
	editorMenuItem[kEditorMenuCombineDetailLevels]->Disable();
	editorMenuItem[kEditorMenuSeparateDetailLevels]->Disable();
	editorMenuItem[kEditorMenuConvertToGenericMesh]->Disable();
	editorMenuItem[kEditorMenuMergeGeometry]->Disable();
	editorMenuItem[kEditorMenuInvertGeometry]->Disable();
	editorMenuItem[kEditorMenuIntersectGeometry]->Disable();
	editorMenuItem[kEditorMenuUnionGeometry]->Disable();
	editorMenuItem[kEditorMenuGenerateAmbientOcclusion]->Disable();
	editorMenuItem[kEditorMenuRemoveAmbientOcclusion]->Disable();

	editorMenuItem[kEditorMenuHideSelection]->Disable();
}

void Editor::UpdateConditionalItems(void)
{
	editorMenuItem[kEditorMenuPasteSubnodes]->Disable();
	editorMenuItem[kEditorMenuSelectAllZone]->Disable();
	editorMenuItem[kEditorMenuLockSelection]->Disable();
	editorMenuItem[kEditorMenuUnlockSelection]->Disable();

	editorMenuItem[kEditorMenuEditController]->Disable();
	editorMenuItem[kEditorMenuGroup]->Disable();
	editorMenuItem[kEditorMenuSetTargetZone]->Disable();
	editorMenuItem[kEditorMenuMoveToTargetZone]->Disable();
	editorMenuItem[kEditorMenuConnectNode]->Disable();
	editorMenuItem[kEditorMenuUnconnectNode]->Disable();
	editorMenuItem[kEditorMenuConnectInfiniteZone]->Disable();
	editorMenuItem[kEditorMenuSelectConnectedNode]->Disable();
	editorMenuItem[kEditorMenuMoveViewportCameraToNode]->Disable();
	editorMenuItem[kEditorMenuOpenInstancedWorld]->Disable();

	editorMenuItem[kEditorMenuRebuildGeometry]->Disable();
	editorMenuItem[kEditorMenuRebuildWithNewPath]->Disable();
	editorMenuItem[kEditorMenuRecalculateNormals]->Disable();
	editorMenuItem[kEditorMenuBakeTransformIntoVertices]->Disable();
	editorMenuItem[kEditorMenuRepositionMeshOrigin]->Disable();
	editorMenuItem[kEditorMenuSetMaterial]->Disable();
	editorMenuItem[kEditorMenuRemoveMaterial]->Disable();
	editorMenuItem[kEditorMenuCombineDetailLevels]->Disable();
	editorMenuItem[kEditorMenuSeparateDetailLevels]->Disable();
	editorMenuItem[kEditorMenuConvertToGenericMesh]->Disable();
	editorMenuItem[kEditorMenuMergeGeometry]->Disable();
	editorMenuItem[kEditorMenuInvertGeometry]->Disable();
	editorMenuItem[kEditorMenuIntersectGeometry]->Disable();
	editorMenuItem[kEditorMenuUnionGeometry]->Disable();
	editorMenuItem[kEditorMenuGenerateAmbientOcclusion]->Disable();
	editorMenuItem[kEditorMenuRemoveAmbientOcclusion]->Disable();

	int32 totalSelectCount = 0;
	const Node *selectedNode = nullptr;

	int32 targetZoneCount = 0;
	int32 geometryCount = 0;
	int32 primitiveCount = 0;
	int32 terrainCount = 0;
	int32 waterCount = 0;
	int32 pathRebuildableCount = 0;
	int32 genericGeometryCount = 0;
	int32 zoneCount = 0;
	int32 instanceCount = 0;

	bool materialSettable = false;
	bool materialRemovable = false;

	const Zone *targetZone = editorObject->GetTargetZone();

	const NodeReference *reference = selectionList.First();
	while (reference)
	{
		const Node *node = reference->GetNode();
		if (++totalSelectCount == 1)
		{
			selectedNode = node;
		}

		if (node != targetZone)
		{
			targetZoneCount++;
		}

		const EditorManipulator *manipulator = Editor::GetManipulator(node);
		if (manipulator->MaterialSettable())
		{
			materialSettable = true;
			materialRemovable |= manipulator->MaterialRemovable();
		}

		switch (node->GetNodeType())
		{
			case kNodeGeometry:
			{
				geometryCount++;
				const Geometry *geometry = static_cast<const Geometry *>(node);
				GeometryType type = geometry->GetGeometryType();
				if (type == kGeometryPrimitive)
				{
					primitiveCount++;

					if (static_cast<const PrimitiveGeometry *>(geometry)->PathPrimitive())
					{
						pathRebuildableCount++;
					}
				}
				else if (type == kGeometryTerrain)
				{
					terrainCount++;
				}
				else if ((type == kGeometryWater) || (type == kGeometryHorizonWater))
				{
					waterCount++;
				}
				else if (type == kGeometryGeneric)
				{
					genericGeometryCount++;
				}

				break;
			}

			case kNodeEffect:
			{
				if (static_cast<const Effect *>(node)->GetEffectType() == kEffectTube)
				{
					pathRebuildableCount++;
				}

				break;
			}

			case kNodeZone:

				zoneCount++;
				break;

			case kNodeInstance:
			{
				instanceCount++;
				break;
			}
		}

		reference = reference->Next();
	}

	if (totalSelectCount == 0)
	{
		return;
	}

	if (editorState & kEditorSelectionLocked)
	{
		editorMenuItem[kEditorMenuUnlockSelection]->Enable();
	}
	else
	{
		editorMenuItem[kEditorMenuLockSelection]->Enable();
	}

	if (!rootNode->GetManipulator()->Selected())
	{
		editorMenuItem[kEditorMenuGroup]->Enable();
	}

	if (totalSelectCount == 1)
	{
		editorMenuItem[kEditorMenuMoveViewportCameraToNode]->Enable();

		if ((!(GetManipulator(selectedNode)->GetManipulatorFlags() & kManipulatorLockedSubtree)) && (editorClipboard))
		{
			editorMenuItem[kEditorMenuPasteSubnodes]->Enable();
		}

		const Controller *controller = selectedNode->GetController();
		if (controller)
		{
			ControllerType type = controller->GetBaseControllerType();
			if ((type == kControllerScript) || (type == kControllerPanel))
			{
				editorMenuItem[kEditorMenuEditController]->Enable();
			}
		}
	}

	if (zoneCount != 0)
	{
		editorMenuItem[kEditorMenuSelectAllZone]->Enable();

		if (zoneCount == 1)
		{
			editorMenuItem[kEditorMenuSetTargetZone]->Enable();
		}
	}

	if (targetZoneCount != 0)
	{
		editorMenuItem[kEditorMenuMoveToTargetZone]->Enable();
	}

	if (instanceCount != 0)
	{
		editorMenuItem[kEditorMenuOpenInstancedWorld]->Enable();
	}

	if (geometryCount != 0)
	{
		if ((terrainCount == 0) && (waterCount == 0))
		{
			editorMenuItem[kEditorMenuRecalculateNormals]->Enable();
			editorMenuItem[kEditorMenuSeparateDetailLevels]->Enable();
			editorMenuItem[kEditorMenuConvertToGenericMesh]->Enable();
			editorMenuItem[kEditorMenuInvertGeometry]->Enable();

			if (geometryCount != 1)
			{
				editorMenuItem[kEditorMenuMergeGeometry]->Enable();

				if (geometryCount == 2)
				{
					editorMenuItem[kEditorMenuIntersectGeometry]->Enable();
					editorMenuItem[kEditorMenuUnionGeometry]->Enable();
				}
			}
		}

		editorMenuItem[kEditorMenuGenerateAmbientOcclusion]->Enable();
		editorMenuItem[kEditorMenuRemoveAmbientOcclusion]->Enable();
	}

	if (materialSettable)
	{
		editorMenuItem[kEditorMenuSetMaterial]->Enable();

		if (materialRemovable)
		{
			editorMenuItem[kEditorMenuRemoveMaterial]->Enable();
		}
	}

	if ((primitiveCount != 0) || (terrainCount != 0) || (waterCount != 0) || (genericGeometryCount != 0))
	{
		editorMenuItem[kEditorMenuRebuildGeometry]->Enable();
	}

	if (pathRebuildableCount != 0)
	{
		editorMenuItem[kEditorMenuRebuildWithNewPath]->Enable();
	}

	if (genericGeometryCount != 0)
	{
		editorMenuItem[kEditorMenuBakeTransformIntoVertices]->Enable();
		editorMenuItem[kEditorMenuRepositionMeshOrigin]->Enable();
	}

	if ((geometryCount >= 2) && (geometryCount <= kMaxCombineGeometryCount))
	{
		editorMenuItem[kEditorMenuCombineDetailLevels]->Enable();
	}

	int32 connectCount = 0;
	int32 unconnectCount = 0;

	const EditorManipulator *manipulator = selectedConnectorList.First();
	while (manipulator)
	{
		if ((totalSelectCount == 1) && (selectedNode != manipulator->GetTargetNode()))
		{
			connectCount++;
		}

		if (manipulator->GetConnectorSelectionTarget())
		{
			unconnectCount++;
		}

		manipulator = manipulator->Next();
	}

	if (connectCount != 0)
	{
		editorMenuItem[kEditorMenuConnectNode]->Enable();
	}

	if (unconnectCount != 0)
	{
		editorMenuItem[kEditorMenuUnconnectNode]->Enable();
		editorMenuItem[kEditorMenuSelectConnectedNode]->Enable();
	}

	if (!selectedConnectorList.Empty())
	{
		editorMenuItem[kEditorMenuConnectInfiniteZone]->Enable();
	}
}

bool Editor::EntireGroupSelected(const Node *root)
{
	const Node *node = root->GetFirstSubnode();
	while (node)
	{
		if (node->GetNodeType() != kNodeGeneric)
		{
			const Manipulator *manipulator = node->GetManipulator();
			if (!manipulator->Selected())
			{
				return (false);
			}
		}

		if (!EntireGroupSelected(node))
		{
			return (false);
		}

		node = node->Next();
	}

	return (true);
}

bool Editor::SaveWorld(bool strip)
{
	File			file;
	ResourcePath	path;

	TheResourceMgr->GetGenericCatalog()->GetResourcePath(WorldResource::GetDescriptor(), resourceName, &resourceLocation, &path);
	TheResourceMgr->CreateDirectoryPath(path);

	FileResult result = file.Open(path, kFileCreate);
	if (result == kFileOkay)
	{
		if (strip)
		{
			InfiniteZone *zone = static_cast<InfiniteZone *>(rootNode);

			editorObject->Retain();
			zone->SetAuxiliaryObject(nullptr);

			result = rootNode->PackTree(&file, kPackInitialize);

			zone->SetAuxiliaryObject(editorObject);
			editorObject->Release();
		}
		else
		{
			for (machine a = 0; a < kEditorViewportCount; a++)
			{
				const EditorViewport *viewport = editorViewport[a];
				const ViewportWidget *viewportWidget = viewport->GetViewportWidget();
				editorObject->SetViewportTransform(a, viewportWidget->GetViewportCamera()->GetNodeTransform());

				if (viewport->GetEditorViewportType() == kEditorViewportFrustum)
				{
					const FrustumViewportWidget *frustumViewportWidget = static_cast<const FrustumViewportWidget *>(viewportWidget);
					editorObject->SetViewportData(a, Vector2D(frustumViewportWidget->GetCameraAzimuth(), frustumViewportWidget->GetCameraAltitude()));
				}
				else
				{
					const OrthoViewportWidget *orthoViewportWidget = static_cast<const OrthoViewportWidget *>(viewportWidget);
					editorObject->SetViewportData(a, orthoViewportWidget->GetOrthoScale());
				}
			}

			result = rootNode->PackTree(&file, kPackEditor | kPackInitialize);
		}
	}

	if (result == kFileOkay)
	{
		editorState &= ~kEditorWorldUnsaved;
		return (true);
	}

	unsigned_int32 id = 'NSAV';

	if (result == kFileLocked)
	{
		id = 'LOCK';
	}
	else if (result == kFileAccessDenied)
	{
		id = 'ACES';
	}
	else if (result == kFileWriteProtected)
	{
		id = 'PROT';
	}
	else if (result == kFileDiskFull)
	{
		id = 'DFUL';
	}

	const StringTable *table = TheWorldEditor->GetStringTable();
	DisplayError(table->GetString(StringID('ERRR', id)));

	return (false);
}

C4::Zone *Editor::GetTargetZone(void) const
{
	return (editorObject->GetTargetZone());
}

void Editor::SetTargetZone(Zone *zone)
{
	if (!zone)
	{
		zone = GetRootNode();
	}

	Zone *targetZone = editorObject->GetTargetZone();
	if (zone != targetZone)
	{
		if (targetZone)
		{
			static_cast<ZoneManipulator *>(GetManipulator(targetZone))->SetTarget(false);
		}

		if (zone)
		{
			static_cast<ZoneManipulator *>(GetManipulator(zone))->SetTarget(true);
		}

		editorObject->SetTargetZone(zone);
		editorState |= kEditorRedrawViewports;
	}
}

MaterialContainer *Editor::GetSelectedMaterial(void) const
{
	return (editorObject->GetSelectedMaterial());
}

void Editor::SelectMaterial(MaterialContainer *materialContainer)
{
	editorObject->SelectMaterial(materialContainer);
	PostEvent(MaterialEditorEvent(kEditorEventMaterialSelected, materialContainer->GetMaterialObject()));
}

void Editor::PostEvent(const EditorEvent& event)
{
	EditorEventType eventType = event.GetEventType();
	if (eventType == kEditorEventMaterialModified)
	{
		const MaterialObject *materialObject = static_cast<const MaterialEditorEvent *>(&event)->GetEventMaterialObject();

		Node *node = rootNode;
		while (node)
		{
			NodeType type = node->GetNodeType();
			if (type == kNodeGeometry)
			{
				Geometry *geometry = static_cast<Geometry *>(node);
				int32 count = geometry->GetMaterialCount();
				for (machine a = 0; a < count; a++)
				{
					if (geometry->GetMaterialObject(a) == materialObject)
					{
						geometry->Neutralize();
						geometry->Preprocess();
						editorState |= kEditorRedrawViewports;

						RenderSegment *segment = geometry->GetFirstRenderSegment();
						while (segment)
						{
							const MaterialObject *const *pointer = segment->GetMaterialObjectPointer();
							if ((pointer) && (*pointer == materialObject))
							{
								segment->InvalidateShaderData();
							}

							segment = segment->GetNextRenderSegment();
						}

						break;
					}
				}
			}

			node = rootNode->GetNextNode(node);
		}

		if (editorState & kEditorRedrawViewports)
		{
			// Force frustum viewports to update because regular updates are
			// deferred until the material manager is closed.

			InvalidateViewports(kEditorViewportFrustum);
		}
	}

	Observable<Editor, const EditorEvent&>::PostEvent(event);
}

void Editor::Paste(Node *targetNode)
{
	UnselectAll();

	Node *root = Node::UnpackTree(editorClipboard->GetStorage(), kUnpackEditor);
	Node *node = root->GetFirstSubnode();
	while (node)
	{
		Node *next = node->Next();

		EditorManipulator::Install(this, node);
		targetNode->AppendSubnode(node);

		SelectNode(node);
		GetManipulator(node)->InvalidateGraph();

		node = next;
	}

	delete root;

	// We must preprocess after deleting the root node because there might be
	// connections to the root node that get broken when it's deleted.

	NodeReference *nodeReference = GetFirstSelectedNode();
	while (nodeReference)
	{
		node = nodeReference->GetNode();
		node->Preprocess();

		nodeReference = nodeReference->Next();
	}

	HandleSelectSubtreeMenuItem(nullptr, nullptr);

	AddOperation(new PasteOperation(GetSelectionList()));
	GetRootNode()->Update();

	nodeReference = GetFirstSelectedNode();
	while (nodeReference)
	{
		node = nodeReference->GetNode();
		NodeType type = node->GetNodeType();
		if (type == kNodeGeometry)
		{
			Geometry *geometry = static_cast<Geometry *>(node);

			int32 materialCount = geometry->GetMaterialCount();
			for (machine a = 0; a < materialCount; a++)
			{
				MaterialObject *materialObject = geometry->GetMaterialObject(a);
				if (materialObject)
				{
					MaterialObject *mo = editorObject->FindMatchingMaterial(materialObject);
					if (mo)
					{
						geometry->SetMaterialObject(a, mo);
					}
				}
			}

			RebuildGeometry(geometry);
		}
		else if (type == kNodeInstance)
		{
			ExpandWorld(static_cast<Instance *>(node));
		}
		else if (type == kNodeModel)
		{
			ExpandModel(static_cast<Model *>(node));
		}
		else if (type == kNodeSkybox)
		{
			Skybox *skybox = static_cast<Skybox *>(node);

			MaterialObject *materialObject = skybox->GetMaterialObject();
			if (materialObject)
			{
				MaterialObject *mo = editorObject->FindMatchingMaterial(materialObject);
				if (mo)
				{
					skybox->SetMaterialObject(mo);
				}
			}

			SetProcessPropertiesFlag();
		}

		nodeReference = nodeReference->Next();
	}

	PostEvent(EditorEvent(kEditorEventNodesPasted));
}

Node *Editor::FindNode(const char *name) const
{
	unsigned_int32 hash = Text::Hash(name);

	Node *node = rootNode->GetFirstSubnode();
	while (node)
	{
		if (node->GetNodeHash() == hash)
		{
			return (node);
		}

		node = rootNode->GetNextNode(node);
	}

	return (nullptr);
}

void Editor::SelectNode(Node *node)
{
	if (!(node->GetNodeFlags() & kNodeNonpersistent))
	{
		EditorManipulator *manipulator = GetManipulator(node);
		if (!manipulator->Selected())
		{
			manipulator->Select();

			unsigned_int32 state = (editorState & ~kEditorDisableSelectionItems) | (kEditorSelectionUpdated | kEditorRedrawViewports);

			NodeReference *reference = new NodeReference(node);
			selectionList.Append(reference);

			if (reference->Previous())
			{
				state |= kEditorUpdateConditionalItems;
			}
			else
			{
				state |= kEditorEnableSelectionItems;
				EnableGizmo(reference);
			}

			editorState = state;
		}
	}
}

void Editor::UnselectNode(Node *node)
{
	EditorManipulator *manipulator = GetManipulator(node);
	if (manipulator->Selected())
	{
		manipulator->Unselect();

		NodeReference *reference = selectionList.First();
		while (reference)
		{
			if (reference->GetNode() == node)
			{
				if (reference == gizmoTarget)
				{
					DisableGizmo(reference);
				}

				delete reference;
				break;
			}

			reference = reference->Next();
		}

		unsigned_int32 state = editorState | (kEditorSelectionUpdated | kEditorRedrawViewports);

		reference = selectionList.First();
		if (reference)
		{
			state |= kEditorUpdateConditionalItems;
			if (!gizmoTarget)
			{
				do
				{
					if (!(reference->GetNode()->GetManipulator()->GetManipulatorState() & kManipulatorDeleted))
					{
						EnableGizmo(reference);
						break;
					}

					reference = reference->Next();
				} while (reference);
			}
		}
		else
		{
			state = (state & ~kEditorSelectionLocked) | kEditorDisableSelectionItems;
		}

		editorState = state;
	}
}

void Editor::SelectAll(Node *root)
{
	if (root->GetSuperNode())
	{
		SelectNode(root);
	}

	Node *node = root->GetFirstSubnode();
	while (node)
	{
		SelectNode(node);
		node = root->GetNextNode(node);
	}
}

void Editor::UnselectAll(void)
{
	if (gizmoTarget)
	{
		DisableGizmo(gizmoTarget);
	}

	for (;;)
	{
		NodeReference *reference = selectionList.First();
		if (!reference)
		{
			break;
		}

		GetManipulator(reference->GetNode())->Unselect();
		delete reference;
	}

	editorState = (editorState & ~kEditorSelectionLocked) | (kEditorSelectionUpdated | kEditorRedrawViewports | kEditorDisableSelectionItems);
}

void Editor::UnselectAllTemp(void)
{
	NodeReference *reference = selectionList.First();
	while (reference)
	{
		NodeReference *next = reference->Next();

		Node *node = reference->GetNode();
		EditorManipulator *manipulator = GetManipulator(node);
		if (manipulator->GetManipulatorState() & kManipulatorTempSelected)
		{
			manipulator->Unselect();

			if (reference == gizmoTarget)
			{
				DisableGizmo(reference);
			}

			delete reference;
		}

		reference = next;
	}

	reference = selectionList.First();
	if (reference)
	{
		editorState |= kEditorSelectionUpdated | kEditorRedrawViewports;

		if (!gizmoTarget)
		{
			EnableGizmo(reference);
		}
	}
	else
	{
		editorState = (editorState & ~kEditorSelectionLocked) | (kEditorSelectionUpdated | kEditorRedrawViewports | kEditorDisableSelectionItems);
	}
}

void Editor::SelectConnector(Node *node, int32 index, bool toggle)
{
	EditorManipulator *manipulator = GetManipulator(node);
	manipulator->SelectConnector(index, toggle);

	if (manipulator->GetManipulatorState() & kManipulatorConnectorSelected)
	{
		selectedConnectorList.Append(manipulator);
	}
	else
	{
		manipulator->Detach();
	}

	editorState |= kEditorRedrawViewports | kEditorUpdateConditionalItems;
}

void Editor::UnselectAllConnectors(Node *exclude)
{
	unsigned_int32 state = editorState;

	EditorManipulator *manipulator = selectedConnectorList.First();
	while (manipulator)
	{
		EditorManipulator *next = manipulator->Next();

		if (manipulator->GetTargetNode() != exclude)
		{
			manipulator->UnselectConnector();
			state |= kEditorRedrawViewports | kEditorUpdateConditionalItems;
		}

		manipulator = next;
	}

	editorState = state;
}

void Editor::UnselectNonGeometryNodes(void)
{
	NodeReference *reference = selectionList.First();
	while (reference)
	{
		NodeReference *next = reference->Next();

		Node *node = reference->GetNode();
		if (node->GetNodeType() != kNodeGeometry)
		{
			UnselectNode(node);
		}

		reference = next;
	}
}

void Editor::SelectAllCell(const Site *cell)
{
	const Bond *bond = cell->GetFirstOutgoingEdge();
	while (bond)
	{
		Site *site = bond->GetFinishElement();
		if (site->GetCellIndex() < 0)
		{
			SelectNode(static_cast<Node *>(site));
		}
		else
		{
			SelectAllCell(site);
		}

		bond = bond->GetNextOutgoingEdge();
	}
}

void Editor::SelectAllMask(Node *root)
{
	if ((root->GetSuperNode()) && (NodeSelectable(root)))
	{
		SelectNode(root);
	}

	Node *node = root->GetFirstSubnode();
	while (node)
	{
		if (NodeSelectable(node))
		{
			SelectNode(node);
		}

		node = root->GetNextNode(node);
	}
}

void Editor::SelectWithMaterial(void)
{
	const MaterialObject *materialObject = GetSelectedMaterial()->GetMaterialObject();
	if (materialObject)
	{
		Node *node = rootNode;
		while (node)
		{
			if (!(node->GetNodeFlags() & kNodeNonpersistent))
			{
				const EditorManipulator *manipulator = GetManipulator(node);
				int32 count = manipulator->GetMaterialCount();
				for (machine a = 0; a < count; a++)
				{
					if (manipulator->GetMaterial(a) == materialObject)
					{
						SelectNode(node);

						if (node->GetNodeType() == kNodeGeometry)
						{
							Geometry *geometry = static_cast<Geometry *>(node);
							const GeometryObject *object = geometry->GetObject();

							int32 surfaceCount = object->GetSurfaceCount();
							if (surfaceCount > 0)
							{
								GeometryManipulator *geometryManipulator = static_cast<GeometryManipulator *>(GetManipulator(node));
								for (machine b = 0; b < surfaceCount; b++)
								{
									const SurfaceData *data = object->GetSurfaceData(b);
									if (geometry->GetMaterialObject(data->materialIndex) == materialObject)
									{
										geometryManipulator->SelectSurface(b);
									}
								}
							}
						}

						break;
					}
				}

				node = rootNode->GetNextNode(node);
			}
			else
			{
				node = rootNode->GetNextLevelNode(node);
			}
		}
	}
}

void Editor::SelectAllFilter(bool (*filter)(const Node *, const void *), const void *cookie)
{
	Node *node = rootNode;
	while (node)
	{
		if (!(node->GetNodeFlags() & kNodeNonpersistent))
		{
			if ((*filter)(node, cookie))
			{
				SelectNode(node);
			}

			node = rootNode->GetNextNode(node);
		}
		else
		{
			node = rootNode->GetNextLevelNode(node);
		}
	}
}

void Editor::ShowNode(Node *node)
{
	GetManipulator(node)->Show();
	editorState |= kEditorRedrawViewports;
}

void Editor::HideNode(Node *node)
{
	EditorManipulator *manipulator = GetManipulator(node);
	manipulator->UnselectConnector();

	UnselectNode(node);
	manipulator->Hide();

	editorState |= kEditorRedrawViewports;
}

void Editor::EnableGizmo(const NodeReference *reference)
{
	EditorManipulator *manipulator = GetManipulator(reference->GetNode());
	manipulator->EnableGizmo();
	gizmoTarget = reference;

	PostEvent(GizmoEditorEvent(kEditorEventGizmoMoved, reference->GetNode()));
	editorState |= kEditorRedrawViewports;
}

void Editor::DisableGizmo(const NodeReference *reference)
{
	EditorManipulator *manipulator = GetManipulator(reference->GetNode());
	manipulator->DisableGizmo();
	gizmoTarget = nullptr;

	PostEvent(GizmoEditorEvent(kEditorEventGizmoMoved, nullptr));
	editorState |= kEditorRedrawViewports;
}

void Editor::InvalidateAllShaderData(void)
{
	Node *node = rootNode->GetFirstSubnode();
	while (node)
	{
		EditorManipulator *manipulator = GetManipulator(node);
		if (manipulator)
		{
			manipulator->InvalidateShaderData();
		}

		node = rootNode->GetNextNode(node);
	}

	const Map<ImpostorSystem> *impostorSystemMap = editorWorld->GetImpostorSystemMap();
	ImpostorSystem *impostorSystem = impostorSystemMap->First();
	while (impostorSystem)
	{
		impostorSystem->InvalidateShaderData();
		impostorSystem = impostorSystem->Next();
	}
}

void Editor::AssignGenericControllers(const ScriptObject *scriptObject, const ConnectorKey& connectorKey, Node *target, ConnectOperation *connectOperation)
{
	if (scriptObject)
	{
		const ScriptGraph *scriptGraph = scriptObject->GetFirstScriptGraph();
		while (scriptGraph)
		{
			const Method *method = scriptGraph->GetFirstElement();
			while (method)
			{
				if (method->GetTargetKey() == connectorKey)
				{
					const MethodRegistration *registration = Method::FindRegistration(method->GetMethodType());
					if (!(registration->GetMethodFlags() & kMethodNoMessage))
					{
						target->SetController(new Controller);
						connectOperation->AddGenericControllerNode(target);
						break;
					}
				}

				method = method->GetNextElement();
			}

			scriptGraph = scriptGraph->Next();
		}
	}
}

void Editor::ExpandWorld(Instance *instance, int32 depth)
{
	if (editorObject->GetEditorFlags() & kEditorExpandWorlds)
	{
		static_cast<InstanceManipulator *>(GetManipulator(instance))->ExpandWorld();
		if (depth < kWorldMaxInstanceDepth)
		{
			ExpandAllWorlds(instance, depth + 1);
		}

		editorState |= kEditorRedrawViewports;
	}
}

void Editor::ExpandAllWorlds(Node *root, int32 depth)
{
	if (!root)
	{
		root = GetRootNode();
	}

	Node *node = root->GetFirstSubnode();
	while (node)
	{
		if (node->GetNodeType() == kNodeInstance)
		{
			Node *next = root->GetNextLevelNode(node);
			ExpandWorld(static_cast<Instance *>(node), depth);
			node = next;
			continue;
		}

		node = root->GetNextNode(node);
	}
}

void Editor::CollapseAllWorlds(void)
{
	Node *root = GetRootNode();
	Node *node = root->GetFirstSubnode();
	while (node)
	{
		if (node->GetNodeType() == kNodeInstance)
		{
			static_cast<InstanceManipulator *>(GetManipulator(node))->CollapseWorld();
			editorState |= kEditorRedrawViewports;
		}

		node = root->GetNextNode(node);
	}

	editorWorld->PurgeInstancedWorldData();
}

void Editor::ExpandModel(Model *model)
{
	if (editorObject->GetEditorFlags() & kEditorExpandModels)
	{
		static_cast<ModelManipulator *>(GetManipulator(model))->ExpandModel();
		editorState |= kEditorRedrawViewports;
	}
}

void Editor::ExpandAllModels(void)
{
	Node *root = GetRootNode();
	Node *node = root->GetFirstSubnode();
	while (node)
	{
		if (node->GetNodeType() == kNodeModel)
		{
			ExpandModel(static_cast<Model *>(node));
		}

		node = root->GetNextNode(node);
	}
}

void Editor::CollapseAllModels(void)
{
	Node *root = GetRootNode();
	Node *node = root->GetFirstSubnode();
	while (node)
	{
		if (node->GetNodeType() == kNodeModel)
		{
			static_cast<ModelManipulator *>(GetManipulator(node))->CollapseModel();
			editorState |= kEditorRedrawViewports;
		}

		node = root->GetNextNode(node);
	}
}

void Editor::AddOperation(Operation *operation)
{
	if (operationList.GetElementCount() >= kMaxEditorOperationCount)
	{
		Operation *oldestOperation = operationList.First();
		for (;;)
		{
			Operation *next = oldestOperation->Next();
			if ((!next) || (!next->Coupled()))
			{
				break;
			}

			oldestOperation = next;
		}

		do
		{
			Operation *previous = oldestOperation->Previous();
			delete oldestOperation;
			oldestOperation = previous;
		} while (oldestOperation);
	}

	operationList.Append(operation);

	editorMenuItem[kEditorMenuUndo]->Enable();
	editorState |= kEditorWorldUnsaved | kEditorRedrawViewports;
}

void Editor::DeleteLastOperation(void)
{
	for (;;)
	{
		Operation *operation = operationList.Last();
		if (!operation)
		{
			break;
		}

		bool coupled = operation->Coupled();
		delete operation;

		if (!coupled)
		{
			break;
		}
	}

	if (operationList.Empty())
	{
		editorMenuItem[kEditorMenuUndo]->Disable();
	}
}

void Editor::DeleteNode(Node *node, bool undoable)
{
	if (node != rootNode)
	{
		List<NodeReference>		referenceList;

		UnselectNode(node);

		EditorManipulator *manipulator = GetManipulator(node);
		manipulator->InvalidateGraph();
		manipulator->HandleDelete(undoable);

		Node *super = node->GetSuperNode();
		const Transform4D& transform = node->GetNodeTransform();

		Node *subnode = node->GetFirstSubnode();
		while (subnode)
		{
			Node *nextNode = subnode->Next();

			if (!(subnode->GetNodeFlags() & kNodeNonpersistent))
			{
				subnode->Neutralize();
				subnode->SetNodeTransform(transform * subnode->GetNodeTransform());
				super->AppendNewSubnode(subnode);
			}
			else
			{
				delete subnode;
			}

			subnode = nextNode;
		}

		Hub *hub = node->GetHub();
		if (hub)
		{
			const Connector *connector = hub->GetFirstIncomingEdge();
			while (connector)
			{
				Node *start = connector->GetStartElement()->GetNode();
				if (start != node)
				{
					referenceList.Append(new NodeReference(start));
				}

				connector = connector->GetNextIncomingEdge();
			}
		}

		if (undoable)
		{
			node->Invalidate();
			node->Neutralize();
			node->Detach();

			node->BreakAllLinks();

			hub = node->GetHub();
			if (hub)
			{
				hub->BreakAllOutgoingConnections();

				hub = node->GetHub();
				if (hub)
				{
					hub->BreakAllIncomingConnections();
				}
			}
		}
		else
		{
			delete node;
		}

		const NodeReference *reference = referenceList.First();
		while (reference)
		{
			GetManipulator(reference->GetNode())->HandleConnectorUpdate();
			reference = reference->Next();
		}

		super->Invalidate();
	}
}

void Editor::DeleteSubtree(Node *root)
{
	Node *node = root->GetFirstSubnode();
	while (node)
	{
		Node *next = node->Next();
		DeleteSubtree(node);
		node = next;
	}

	DeleteNode(root);
}

void Editor::ReparentSelectedNodes(Node *newSuper)
{
	AddOperation(new ReparentOperation(&selectionList));

	Zone *newZone = (newSuper->GetNodeType() == kNodeZone) ? static_cast<Zone *>(newSuper) : newSuper->GetOwningZone();

	const NodeReference *reference = GetFirstSelectedNode();
	while (reference)
	{
		Node *node = reference->GetNode();
		if ((node != newSuper) && (!node->Successor(newSuper)) && (node->GetSuperNode() != newSuper))
		{
			EditorManipulator *superManipulator = GetManipulator(node->GetSuperNode());
			if (!superManipulator->Selected())
			{
				superManipulator->InvalidateGraph();

				Zone *oldZone = node->GetOwningZone();
				if (newZone != oldZone)
				{
					node->Neutralize();
				}

				node->SetNodeTransform(newSuper->GetInverseWorldTransform() * node->GetWorldTransform());
				newSuper->AppendSubnode(node);

				if (newZone != oldZone)
				{
					node->Preprocess();
				}

				if (node->GetNodeType() == kNodeGeometry)
				{
					InvalidateGeometry(static_cast<Geometry *>(node));
				}

				Controller *controller = node->GetController();
				if (controller)
				{
					controller->Invalidate();
				}
			}
		}

		reference = reference->Next();
	}

	newSuper->Invalidate();
	GetManipulator(newSuper)->InvalidateGraph();
}

void Editor::InvalidateGeometry(Geometry *geometry)
{
	const Object *object = geometry->GetObject();
	if (object->GetReferenceCount() == 1)
	{
		geometry->InvalidateShaderData();
		geometry->Invalidate();
		geometry->Neutralize();
		geometry->Preprocess();
	}
	else
	{
		Node *node = rootNode->GetFirstSubnode();
		while (node)
		{
			if ((node->GetNodeType() == kNodeGeometry) && (node->GetObject() == object))
			{
				static_cast<Geometry *>(node)->InvalidateShaderData();
				node->Invalidate();
				node->Neutralize();
				node->Preprocess();
			}

			node = rootNode->GetNextNode(node);
		}
	}

	editorState |= kEditorRedrawViewports;

	if ((gizmoTarget) && (gizmoTarget->GetNode() == geometry))
	{
		PostEvent(GizmoEditorEvent(kEditorEventGizmoTargetModified, geometry));
	}
}

void Editor::RebuildGeometry(Geometry *geometry)
{
	GeometryObject *object = geometry->GetObject();

	GeometryType type = object->GetGeometryType();
	if (type == kGeometryPrimitive)
	{
		static_cast<PrimitiveGeometryObject *>(object)->Build(geometry);
	}
	else if (type == kGeometryTerrain)
	{
		TerrainGeometry *terrain = static_cast<TerrainGeometry *>(geometry);
		if (terrain->GetBlockNode())
		{
			TerrainGeometryObject *terrainObject = static_cast<TerrainGeometryObject *>(object);
			terrainObject->Build(terrain);

			if (terrainObject->GetDetailLevel() != 0)
			{
				static_cast<TerrainLevelGeometry *>(terrain)->ProcessStructure();
			}
		}
	}
	else if (type == kGeometryWater)
	{
		static_cast<WaterGeometryObject *>(object)->Build(geometry);
	}
	else if (type == kGeometryHorizonWater)
	{
		static_cast<HorizonWaterGeometryObject *>(object)->Build(geometry);
	}
	else
	{
		if (type == kGeometryGeneric)
		{
			static_cast<GenericGeometryObject *>(object)->Rebuild(geometry);
		}

		RegenerateTexcoords(geometry);
		object->BuildCollisionData();
	}

	static_cast<GeometryManipulator *>(GetManipulator(geometry))->UpdateSurfaceSelection();
	InvalidateGeometry(geometry);
}

void Editor::RebuildGeometry(const List<NodeReference> *nodeList)
{
	rootNode->Update();

	const NodeReference *reference = nodeList->First();
	while (reference)
	{
		Node *node = reference->GetNode();
		if (node->GetNodeType() == kNodeGeometry)
		{
			RebuildGeometry(static_cast<Geometry *>(node));
		}

		reference = reference->Next();
	}

	InvalidateAllViewports();
}

void Editor::RegenerateTexcoords(Geometry *geometry)
{
	const GeometryObject *object = geometry->GetObject();

	int32 levelCount = object->GetGeometryLevelCount();
	for (machine a = 0; a < levelCount; a++)
	{
		Mesh *level = object->GetGeometryLevel(a);
		if (level->GenerateTexcoords(geometry, object))
		{
			if (level->GetArray(kArrayTangent))
			{
				level->CalculateTangentArray();
			}
		}
	}

	geometry->Neutralize();
	geometry->Preprocess();
	editorState |= kEditorRedrawViewports;
}

void Editor::RegenerateTexcoords(const List<NodeReference> *nodeList)
{
	rootNode->Update();

	const NodeReference *reference = nodeList->First();
	while (reference)
	{
		Node *node = reference->GetNode();
		if (node->GetNodeType() == kNodeGeometry)
		{
			RegenerateTexcoords(static_cast<Geometry *>(node));
		}

		reference = reference->Next();
	}

	InvalidateAllViewports();
}

void Editor::OpenMaterialManager(void)
{
	editorState |= kEditorWorldUnsaved;
	AddSubwindow(new MaterialWindow(this));
}

void Editor::DisplayError(const char *string)
{
	const StringTable *table = TheWorldEditor->GetStringTable();

	const char *title = table->GetString(StringID('ERRR'));
	const char *okayText = table->GetString(StringID('BTTN', 'OKAY'));

	Dialog *dialog = new Dialog(Vector2D(342.0F, 120.0F), title, okayText);

	ImageWidget *image = new ImageWidget(Vector2D(64.0F, 64.0F), "C4/error");
	image->SetWidgetPosition(Point3D(12.0F, 12.0F, 0.0F));
	dialog->AppendSubnode(image);

	TextWidget *text = new TextWidget(Vector2D(242.0F, 0.0F), string, "font/Gui");
	text->SetTextFlags(kTextWrapped);
	text->SetWidgetPosition(Point3D(88.0F, 16.0F, 0.0F));
	dialog->AppendSubnode(text);

	AddSubwindow(dialog);
}

bool Editor::NodeSelectable(const Node *node, bool nonpersistent) const
{
	if ((nonpersistent) || (!(node->GetNodeFlags() & kNodeNonpersistent)))
	{
		Type	packType[kMaxNodeTypeSize];

		Package package(packType, sizeof(Type) * kMaxNodeTypeSize);
		Packer packer(&package);

		node->PackType(packer);
		int32 size = package.GetSize() / sizeof(Type) - 1;
		int32 i = 0;

		const SelectionMask *mask = editorObject->GetSelectionMask()->GetFirstSubnode();
		while (mask)
		{
			if (mask->GetNodeType() == packType[i])
			{
				int32 value = mask->GetMaskValue();
				if (value == 1)
				{
					return (true);
				}
				else if ((value == 0) || (i >= size))
				{
					break;
				}

				i++;
				mask = mask->GetFirstSubnode();
				continue;
			}

			mask = mask->Next();
		}
	}

	return (false);
}

Node *Editor::PickNode(Node *root, Ray *ray, PickData *pickData, PickFilterProc *filterProc, const void *filterCookie) const
{
	float			t1, t2;

	Node *hitNode = nullptr;
	if (!filterProc)
	{
		filterProc = [](const Node *node, const PickData *, const void *cookie) -> bool
		{
			return (static_cast<const Editor *>(cookie)->NodeSelectable(node));
		};

		filterCookie = this;
	}

	EditorManipulator *manipulator = GetManipulator(root);
	const BoundingSphere *sphere = manipulator->GetTreeSphere();

	float r = ray->radius * 11.0F;
	if (r == 0.0F)
	{
		r = kFrustumRenderScale * 12.0F;
	}

	if ((sphere) && (Math::IntersectRayAndSphere(ray, sphere->GetCenter(), sphere->GetRadius() + r, &t1, &t2)))
	{
		if (!manipulator->Hidden())
		{
			sphere = manipulator->GetNodeSphere();
			if ((sphere) && (Math::IntersectRayAndSphere(ray, sphere->GetCenter(), sphere->GetRadius() + r, &t1, &t2)))
			{
				Ray			nodeRay;
				PickData	nodePickData;

				const Transform4D& transform = root->GetInverseWorldTransform();
				nodeRay.origin = transform * ray->origin;
				nodeRay.direction = transform * ray->direction;

				float dr = ray->radius;
				nodeRay.radius = dr;

				float s = Sqrt((sphere->GetRadius() + r) * dr * 2.0F + dr * dr);
				nodeRay.tmin = Fmax(ray->tmin, t1) - s;
				nodeRay.tmax = Fmin(ray->tmax, t2) + s;

				// The filter procedure has to be called after the node is picked so that information
				// in the PickData structure can be used to filter out nodes.

				nodePickData.pickPoint.Set(0.0F, 0.0F, 0.0F);
				nodePickData.pickNormal.Set(0.0F, 0.0F, 1.0F);

				if ((manipulator->Pick(&nodeRay, &nodePickData)) && ((*filterProc)(root, &nodePickData, filterCookie)))
				{
					hitNode = root;
					ray->tmax = nodePickData.rayParam;
					*pickData = nodePickData;
				}
			}
		}

		Node *node = root->GetFirstSubnode();
		while (node)
		{
			Node *hit = PickNode(node, ray, pickData, filterProc, filterCookie);
			if (hit)
			{
				hitNode = hit;
			}

			node = node->Next();
		}
	}

	return (hitNode);
}

Node *Editor::PickHandle(int32 viewportIndex, Ray *ray, int32 *index) const
{
	Node *hitNode = nullptr;

	NodeReference *reference = selectionList.First();
	while (reference)
	{
		Node *node = reference->GetNode();
		if (NodeSelectable(node))
		{
			float	t1, t2;

			const EditorManipulator *manipulator = GetManipulator(node);
			const BoundingSphere *sphere = manipulator->GetNodeSphere();

			if ((sphere) && (Math::IntersectRayAndSphere(ray, sphere->GetCenter(), sphere->GetRadius() * K::sqrt_3, &t1, &t2)))
			{
				int32 handleCount = manipulator->GetHandleCount();
				if (handleCount != 0)
				{
					const Transform4D& transform = node->GetInverseWorldTransform();
					Point3D p = transform * ray->origin;
					Vector3D v = transform * ray->direction;

					float r2 = ray->radius * ray->radius * 16.0F;
					for (machine a = 0; a < handleCount; a++)
					{
						const Point3D& q = manipulator->GetHandlePosition(a);
						if (Math::SquaredDistancePointToLine(q, p, v) < r2)
						{
							float t = Magnitude(q - p);
							if (t < ray->tmax - ray->tmin)
							{
								hitNode = node;
								ray->tmax = t + ray->tmin;
								*index = a;
							}
						}
					}
				}
			}
		}

		reference = reference->Next();
	}

	return (hitNode);
}

Node *Editor::PickNode(EditorTrackData *trackData, PickData *pickData, PickFilterProc *filterProc, const void *filterCookie) const
{
	if (trackData->viewportType != kEditorViewportGraph)
	{
		if (filterProc)
		{
			return (PickNode(rootNode, &trackData->worldRay, pickData, filterProc, filterCookie));
		}
		else
		{
			auto filter = [](const Node *node, const PickData *, const void *cookie) -> bool
			{
				return (static_cast<const Editor *>(cookie)->NodeSelectable(node, true));
			};

			Node *node = PickNode(rootNode, &trackData->worldRay, pickData, filter, this);
			if (node)
			{
				while (node->GetNodeFlags() & kNodeNonpersistent)
				{
					node = node->GetSuperNode();
				}

				return (node);
			}
		}
	}

	return (nullptr);
}

Node *Editor::PickHandle(EditorTrackData *trackData, int32 *index) const
{
	if (trackData->viewportType == kEditorViewportOrtho)
	{
		return (PickHandle(trackData->viewportIndex, &trackData->worldRay, index));
	}

	return (nullptr);
}

Node *Editor::PickConnector(EditorTrackData *trackData, int32 *index) const
{
	PickData	pickData;

	NodeReference *reference = selectionList.First();
	while (reference)
	{
		Node *node = reference->GetNode();

		const EditorManipulator *manipulator = GetManipulator(node);
		if (manipulator->PickConnector(trackData, &trackData->worldRay, &pickData))
		{
			*index = pickData.pickIndex[0];
			return (node);
		}

		reference = reference->Next();
	}

	const EditorManipulator *manipulator = selectedConnectorList.First();
	while (manipulator)
	{
		if (!manipulator->Selected())
		{
			if (manipulator->PickConnector(trackData, &trackData->worldRay, &pickData))
			{
				*index = pickData.pickIndex[0];
				return (manipulator->GetTargetNode());
			}
		}

		manipulator = manipulator->Next();
	}

	return (nullptr);
}

Node *Editor::PickGizmoArrow(EditorTrackData *trackData) const
{
	if (gizmoTarget)
	{
		Node *node = gizmoTarget->GetNode();
		EditorGizmo *gizmo = GetManipulator(node)->GetGizmo();
		if (gizmo)
		{
			int32 index = gizmo->PickArrow(trackData->viewportCamera, trackData->viewportType, trackData->lineScale, &trackData->worldRay);
			if (index >= 0)
			{
				trackData->gizmo = gizmo;
				trackData->gizmoIndex = index;
				return (node);
			}
		}
	}

	return (nullptr);
}

Node *Editor::PickGizmoSquare(EditorTrackData *trackData) const
{
	if (gizmoTarget)
	{
		Node *node = gizmoTarget->GetNode();
		EditorGizmo *gizmo = GetManipulator(node)->GetGizmo();
		if (gizmo)
		{
			int32 index = gizmo->PickSquare(trackData->viewportCamera, trackData->viewportType, trackData->lineScale, &trackData->worldRay);
			if (index >= 0)
			{
				trackData->gizmo = gizmo;
				trackData->gizmoIndex = index;
				return (node);
			}
		}
	}

	return (nullptr);
}

Node *Editor::PickGizmoCircle(EditorTrackData *trackData) const
{
	if (gizmoTarget)
	{
		Node *node = gizmoTarget->GetNode();
		EditorGizmo *gizmo = GetManipulator(node)->GetGizmo();
		if (gizmo)
		{
			int32 index = gizmo->PickCircle(trackData->viewportCamera, trackData->viewportType, trackData->lineScale, &trackData->worldRay);
			if (index >= 0)
			{
				trackData->gizmo = gizmo;
				trackData->gizmoIndex = index;
				return (node);
			}
		}
	}

	return (nullptr);
}

void Editor::AutoScroll(const EditorTrackData *trackData)
{
	EditorViewport *viewport = editorViewport[trackData->viewportIndex];
	ViewportWidget *viewportWidget = viewport->GetViewportWidget();

	float width = viewportWidget->GetWidgetSize().x;
	float height = viewportWidget->GetWidgetSize().y;

	float x = trackData->currentViewportPosition.x * width;
	float y = trackData->currentViewportPosition.y * height;

	if ((x < 0.0F) || (y < 0.0F) || (x > width) || (y > height))
	{
		float dt = TheTimeMgr->GetSystemFloatDeltaTime() * 0.01F;
		float dx = 0.0F;
		float dy = 0.0F;

		if (x < 0.0F)
		{
			dx = Fmax((x - 15.0F) * dt, width * -0.5F);
		}
		else if (x > width)
		{
			dx = Fmin((x - width + 16.0F) * dt, width * 0.5F);
		}

		if (y < 0.0F)
		{
			dy = Fmax((float) (y - 15.0F) * dt, height * -0.5F);
		}
		else if (y > height)
		{
			dy = Fmin((float) (y - height + 16.0F) * dt, height * 0.5F);
		}

		float scale = trackData->viewportScale;
		Camera *camera = trackData->viewportCamera;
		Vector3D offset = camera->GetWorldTransform() * Vector3D(dx * scale, dy * scale, 0.0F);

		if (trackData->viewportType == kEditorViewportGraph)
		{
			offset.x += scrollFraction.x;
			offset.y += scrollFraction.y;

			float sx = Floor(offset.x);
			float sy = Floor(offset.y);
			scrollFraction.x = offset.x - sx;
			scrollFraction.y = offset.y - sy;

			offset.x = sx;
			offset.y = sy;
		}

		camera->SetNodePosition(camera->GetNodePosition() + offset);
		viewport->Invalidate();
	}
}

void Editor::InitNewNode(EditorTrackData *trackData, Node *node)
{
	UnselectAll();

	trackData->trackNode = node;
	EditorManipulator::Install(this, node);

	Zone *targetZone = GetTargetZone();

	EditorViewportType viewportType = trackData->viewportType;
	if (viewportType == kEditorViewportFrustum)
	{
		Transform4D transform(Identity3D, trackData->currentPickPoint);

		if (GetManipulator(node)->GetManipulatorFlags() & kManipulatorAdjustablePlacement)
		{
			AdjustPlacement(&transform, trackData->currentPickNormal);
		}

		node->SetNodeTransform(targetZone->GetInverseWorldTransform() * transform);
		targetZone->AppendSubnode(node);
	}
	else if (viewportType == kEditorViewportGraph)
	{
		trackData->superNode->AppendSubnode(node);
	}
	else
	{
		Point3D position = GetTargetSpacePosition(trackData, trackData->snappedAnchorPosition);
		const Transform4D& cameraTransform = trackData->viewportCamera->GetWorldTransform();
		node->SetNodeMatrix3D(targetZone->GetInverseWorldTransform() * Matrix3D(cameraTransform[0], -cameraTransform[1], -cameraTransform[2]));
		node->SetNodePosition(position);
		targetZone->AppendSubnode(node);
	}

	rootNode->Update();

	NodeType type = node->GetNodeType();
	if (type == kNodeGeometry)
	{
		Geometry *geometry = static_cast<Geometry *>(node);
		geometry->SetMaterialObject(0, GetSelectedMaterial()->GetMaterialObject());
		RebuildGeometry(geometry);
	}
	else
	{
		if (type == kNodeEffect)
		{
			Effect *effect = static_cast<Effect *>(node);
			if (effect->GetEffectType() == kEffectTube)
			{
				static_cast<TubeEffect *>(effect)->GetObject()->Build();
			}
		}
		else if (type == kNodeImpostor)
		{
			static_cast<Impostor *>(node)->SetMaterialObject(GetSelectedMaterial()->GetMaterialObject());
		}

		node->Preprocess();
	}

	PostEvent(NodeEditorEvent(kEditorEventNodeCreated, node));
	SelectNode(node);

	GetManipulator(node)->InvalidateGraph();
}

void Editor::CommitNewNode(EditorTrackData *trackData, bool commit)
{
	Node *node = trackData->trackNode;

	if (commit)
	{
		AddOperation(new CreateOperation(node));
	}
	else
	{
		if (gizmoTarget)
		{
			DisableGizmo(gizmoTarget);
		}

		selectionList.Purge();
		delete node;

		editorState |= kEditorDisableSelectionItems;
	}
}

EditorTrackData *Editor::CalculateTrackData(const ViewportWidget *viewport, const Point3D& position)
{
	PickData		pickData;

	editorTrackData.previousPosition = editorTrackData.currentPosition;
	editorTrackData.snappedPreviousPosition = editorTrackData.snappedCurrentPosition;
	editorTrackData.previousViewportPosition = editorTrackData.currentViewportPosition;
	editorTrackData.previousModifierKeys = editorTrackData.currentModifierKeys;

	float x = position.x / viewport->GetWidgetSize().x;
	float y = position.y / viewport->GetWidgetSize().y;
	editorTrackData.currentViewportPosition.Set(x, y);

	Ray *ray = &editorTrackData.worldRay;
	const Camera *camera = viewport->GetViewportCamera();
	camera->CastRay(x, y, ray);

	const Transform4D& cameraTransform = camera->GetNodeTransform();
	const Point3D& cameraPosition = cameraTransform.GetTranslation();

	EditorViewportType viewportType = editorTrackData.viewportType;
	if (viewportType != kEditorViewportFrustum)
	{
		x = cameraPosition * cameraTransform[0] + ray->origin.x;
		y = cameraPosition * cameraTransform[1] + ray->origin.y;
	}
	else
	{
		float f = 20.0F / ray->direction.z;
		x = cameraPosition * cameraTransform[0] + ray->direction.x * f;
		y = cameraPosition * cameraTransform[1] + ray->direction.y * f;
	}

	editorTrackData.currentPosition.Set(x, y);
	editorTrackData.snappedCurrentPosition = SnapToGrid(editorTrackData.currentPosition);

	ray->origin = cameraTransform * ray->origin;
	ray->direction = Normalize(cameraTransform * ray->direction);

	if (viewportType != kEditorViewportGraph)
	{
		const BoundingSphere *sphere = GetManipulator(rootNode)->GetTreeSphere();
		if (sphere)
		{
			float centerDistance = ray->direction * (sphere->GetCenter() - ray->origin);
			float sphereRadius = sphere->GetRadius();
			ray->tmax = centerDistance + sphereRadius;

			if (viewportType == kEditorViewportOrtho)
			{
				ray->tmin = centerDistance - sphereRadius;
				ray->radius = static_cast<const OrthoViewportWidget *>(viewport)->GetOrthoScale().x;
			}
			else
			{
				ray->tmax = Fmax(ray->tmax, sphereRadius);
				ray->tmin = 0.0F;
				ray->radius = 0.0F;
			}
		}
		else
		{
			ray->tmin = 0.0F;
			ray->tmax = 1.0F;
			ray->radius = 0.0F;
		}
	}
	else
	{
		ray->tmin = -1.0F;
		ray->tmax = 1.0F;
		ray->radius = 0.0F;
	}

	float tmax = ray->tmax;

	PickFilterProc *filterProc = pickFilterProc;
	const void *filterCookie = pickFilterCookie;

	if (!filterProc)
	{
		filterProc = [](const Node *node, const PickData *, const void *) -> bool
		{
			return (node->GetNodeType() == kNodeGeometry);
		};

		filterCookie = nullptr;
	}

	Node *node = PickNode(rootNode, ray, &pickData, filterProc, filterCookie);
	editorTrackData.currentPickNode = node;
	if (node)
	{
		editorTrackData.currentPickPoint = ray->origin + ray->direction * pickData.rayParam;
		editorTrackData.currentPickNormal = pickData.pickNormal * node->GetInverseWorldTransform();
	}

	ray->tmax = tmax;

	editorTrackData.currentModifierKeys = InterfaceMgr::GetModifierKeys();
	return (&editorTrackData);
}

bool Editor::SetTrackPickFilter(EditorTrackData *trackData, PickFilterProc *proc, const void *cookie)
{
	PickData	pickData;

	pickFilterProc = proc;
	pickFilterCookie = cookie;

	Node *node = PickNode(trackData, &pickData, proc, cookie);
	trackData->currentPickNode = node;
	if (node)
	{
		trackData->currentPickPoint = node->GetWorldTransform() * pickData.pickPoint;
		trackData->currentPickNormal = pickData.pickNormal * node->GetInverseWorldTransform();
		return (true);
	}

	return (false);
}

float Editor::SnapToGrid(const float x) const
{
	if (editorObject->GetEditorFlags() & kEditorSnapToGrid)
	{
		float spacing = editorObject->GetGridLineSpacing();
		return (Floor(x / spacing + 0.5F) * spacing);
	}

	return (x);
}

Point2D Editor::SnapToGrid(const Point2D& p) const
{
	if (editorObject->GetEditorFlags() & kEditorSnapToGrid)
	{
		float spacing = editorObject->GetGridLineSpacing();
		float x = Floor(p.x / spacing + 0.5F) * spacing;
		float y = Floor(p.y / spacing + 0.5F) * spacing;
		return (Point2D(x, y));
	}

	return (p);
}

Point3D Editor::SnapToGrid(const Point3D& p) const
{
	if (editorObject->GetEditorFlags() & kEditorSnapToGrid)
	{
		float spacing = editorObject->GetGridLineSpacing();
		float x = Floor(p.x / spacing + 0.5F) * spacing;
		float y = Floor(p.y / spacing + 0.5F) * spacing;
		float z = Floor(p.z / spacing + 0.5F) * spacing;
		return (Point3D(x, y, z));
	}

	return (p);
}

void Editor::AdjustPlacement(Transform4D *transform, const Vector3D& normal) const
{
	if (placementAdjusterProc)
	{
		(*placementAdjusterProc)(this, transform, normal, placementAdjusterCookie);
	}
}

Point3D Editor::GetWorldSpacePosition(const EditorTrackData *trackData, const Point2D& position)
{
	const Transform4D& cameraTransform = trackData->viewportCamera->GetNodeTransform();
	return (Zero3D + cameraTransform[0] * position.x + cameraTransform[1] * position.y);
}

Vector3D Editor::GetWorldSpaceDirection(const EditorTrackData *trackData, const Vector2D& direction)
{
	const Transform4D& cameraTransform = trackData->viewportCamera->GetNodeTransform();
	return (cameraTransform[0] * direction.x + cameraTransform[1] * direction.y);
}

Vector3D Editor::GetWorldSpaceDirection(const EditorTrackData *trackData, const Vector3D& direction)
{
	return (trackData->viewportCamera->GetNodeTransform() * direction);
}

Point3D Editor::GetTargetSpacePosition(const EditorTrackData *trackData, const Point2D& position) const
{
	Zone *targetZone = GetTargetZone();

	const Transform4D& cameraTransform = trackData->viewportCamera->GetNodeTransform();
	Point3D targetPosition = Zero3D + cameraTransform[2] * (cameraTransform[2] * targetZone->GetWorldPosition());
	targetPosition += cameraTransform[0] * position.x + cameraTransform[1] * position.y;
	return (targetZone->GetInverseWorldTransform() * targetPosition);
}

Vector3D Editor::GetTargetSpaceDirection(const EditorTrackData *trackData, const Vector3D& direction) const
{
	const Transform4D& cameraTransform = trackData->viewportCamera->GetNodeTransform();
	return (GetTargetZone()->GetInverseWorldTransform() * (cameraTransform * direction));
}

void Editor::SetCurrentTool(EditorTool *tool, void *cookie)
{
	currentCursor = nullptr;

	if (currentTool)
	{
		currentTool->Disengage(this, cookie);
	}

	if (tool)
	{
		tool->Engage(this, cookie);
	}

	currentTool = tool;
}

void Editor::ImportScene(Node *root)
{
	UnselectAll();

	Zone *zone = GetTargetZone();

	Node *first = root->GetFirstSubnode();
	Node *node = first;
	while (node)
	{
		node->SetNodeTransform(zone->GetInverseWorldTransform() * node->GetNodeTransform());
		EditorManipulator::Install(this, node);
		zone->AppendSubnode(node);

		node = root->GetFirstSubnode();
	}

	node = first;
	while (node)
	{
		node->Preprocess();
		node = node->Next();
	}

	zone->Update();

	node = first;
	while (node)
	{
		Controller *controller = node->GetController();
		if (controller)
		{
			controller->Update();
		}

		node = zone->GetNextNode(node);
	}

	GetManipulator(zone)->InvalidateGraph();
	editorState |= kEditorWorldUnsaved | kEditorRedrawViewports;
}

void Editor::BuildViewport(int32 index)
{
	EditorViewport *viewport = editorViewport[index];
	delete viewport;

	int32 mode = editorObject->GetViewportMode(index);
	if (mode < kViewportModeOrthoCount)
	{
		OrthoEditorViewport *orthoViewport = new OrthoEditorViewport(this, index, editorObject->GetViewportData(index), mode);
		orthoViewport->SetViewportCameraTransform(editorObject->GetViewportTransform(index));
		viewport = orthoViewport;
	}
	else if (mode == kViewportModeFrustum)
	{
		FrustumEditorViewport *frustumViewport = new FrustumEditorViewport(this, index);
		frustumViewport->SetViewportCameraTransform(editorObject->GetViewportData(index), editorObject->GetViewportTransform(index).GetTranslation());
		viewport = frustumViewport;
	}
	else
	{
		GraphEditorViewport *graphViewport = new GraphEditorViewport(this, index, editorObject->GetViewportData(index));
		graphViewport->SetViewportCameraTransform(editorObject->GetViewportTransform(index));
		viewport = graphViewport;

		GetManipulator(rootNode)->InvalidateGraph();
	}

	editorViewport[index] = viewport;

	ViewportWidget *viewportWidget = viewport->GetViewportWidget();
	AppendNewSubnode(viewportWidget);
}

void Editor::ShowViewport(int32 index)
{
	int32 layout = editorObject->GetCurrentViewportLayout();
	if (layout == kEditorLayout1)
	{
		if (index == editorObject->GetFullViewportIndex())
		{
			editorObject->SetCurrentViewportLayout(editorObject->GetPreviousViewportLayout());
		}
		else
		{
			editorObject->SetFullViewportIndex(index);
		}
	}
	else
	{
		editorObject->SetPreviousViewportLayout(layout);
		editorObject->SetCurrentViewportLayout(kEditorLayout1);
		editorObject->SetFullViewportIndex(index);
	}

	UpdateViewportStructures();
}

void Editor::UpdateViewportStructures(void)
{
	static const unsigned_int16 visibilityFlags[kEditorLayoutCount - 1] =
	{
		0x000F, 0x00C0, 0x0030, 0x001A, 0x0025, 0x004C, 0x0083
	};

	float width = GetWidgetSize().x;
	float height = GetWidgetSize().y;

	float leftMargin = kEditorPageWidth + 15.0F;
	float rightMargin = 4.0F;
	float menuBarHeight = menuBar->GetWidgetSize().y;
	float topMargin = menuBarHeight + 31.0F;

	toolGroup->SetWidgetPosition(Point3D(leftMargin + 2.0F, toolGroup->GetWidgetPosition().y, 0.0F));
	toolGroup->Invalidate();

	SetBackgroundQuad(0, Point3D(0.0F, 0.0F, 0.0F), Vector2D(leftMargin - 1.0F, height));
	SetBackgroundQuad(1, Point3D(width - rightMargin + 1.0F, 0.0F, 0.0F), Vector2D(rightMargin - 1.0F, height));

	width -= leftMargin + rightMargin;
	SetBackgroundQuad(2, Point3D(leftMargin - 1.0F, 0.0F, 0.0F), Vector2D(width + 2.0F, topMargin - 1.0F));
	SetBackgroundQuad(3, Point3D(leftMargin - 1.0F, height - 4.0F, 0.0F), Vector2D(width + 2.0F, 4.0F));

	height -= topMargin + 5.0F;
	float halfWidth = (width - 6.0F) * 0.5F;
	float halfHeight = (height - 6.0F) * 0.5F;

	viewportPosition[kEditorViewportTopLeft].Set(leftMargin, topMargin, 0.0F);
	viewportSize[kEditorViewportTopLeft].Set(halfWidth, halfHeight);

	viewportPosition[kEditorViewportTopRight].Set(leftMargin + halfWidth + 6.0F, topMargin, 0.0F);
	viewportSize[kEditorViewportTopRight].Set(halfWidth, halfHeight);

	viewportPosition[kEditorViewportBottomLeft].Set(leftMargin, halfHeight + topMargin + 6.0F, 0.0F);
	viewportSize[kEditorViewportBottomLeft].Set(halfWidth, halfHeight);

	viewportPosition[kEditorViewportBottomRight].Set(leftMargin + halfWidth + 6.0F, halfHeight + topMargin + 6.0F, 0.0F);
	viewportSize[kEditorViewportBottomRight].Set(halfWidth, halfHeight);

	viewportPosition[kEditorViewportLeft].Set(leftMargin, topMargin, 0.0F);
	viewportSize[kEditorViewportLeft].Set(halfWidth, height);

	viewportPosition[kEditorViewportRight].Set(leftMargin + halfWidth + 6.0F, topMargin, 0.0F);
	viewportSize[kEditorViewportRight].Set(halfWidth, height);

	viewportPosition[kEditorViewportTop].Set(leftMargin, topMargin, 0.0F);
	viewportSize[kEditorViewportTop].Set(width, halfHeight);

	viewportPosition[kEditorViewportBottom].Set(leftMargin, halfHeight + topMargin + 6.0F, 0.0F);
	viewportSize[kEditorViewportBottom].Set(width, halfHeight);

	int32 layout = editorObject->GetCurrentViewportLayout();
	if (layout == kEditorLayout1)
	{
		int32 index = editorObject->GetFullViewportIndex();
		viewportPosition[index].Set(leftMargin, topMargin, 0.0F);
		viewportSize[index].Set(width, height);

		for (machine a = 0; a < kEditorViewportCount; a++)
		{
			if (a == index)
			{
				editorViewport[a]->Show();
			}
			else
			{
				editorViewport[a]->Hide();
			}
		}

		HideBackgroundQuad(4);
		HideBackgroundQuad(5);
	}
	else
	{
		if (layout != 3)
		{
			if ((layout != 4) && (layout != 5))
			{
				SetBackgroundQuad(4, Point3D(leftMargin - 1.0F, halfHeight + topMargin + 1.0F, 0.0F), Vector2D(width + 2.0F, 4.0F));
			}
			else
			{
				float x = (layout == 4) ? leftMargin + halfWidth + 4.0F : leftMargin - 1.0F;
				SetBackgroundQuad(4, Point3D(x, halfHeight + topMargin + 1.0F, 0.0F), Vector2D(halfWidth + 3.0F, 4.0F));
			}
		}
		else
		{
			HideBackgroundQuad(4);
		}

		if (layout != 2)
		{
			if (layout < 6)
			{
				SetBackgroundQuad(5, Point3D(leftMargin + halfWidth + 1.0F, topMargin - 1.0F, 0.0F), Vector2D(4.0F, height + 2.0F));
			}
			else
			{
				float y = (layout == 6) ? halfHeight + topMargin + 5.0F : topMargin - 1.0F;
				SetBackgroundQuad(5, Point3D(leftMargin + halfWidth + 1.0F, y, 0.0F), Vector2D(4.0F, halfHeight + 3.0F));
			}
		}
		else
		{
			HideBackgroundQuad(5);
		}

		unsigned_int32 flags = visibilityFlags[layout - 1];
		for (machine a = 0; a < kEditorViewportCount; a++)
		{
			if ((flags >> a) & 1)
			{
				editorViewport[a]->Show();
			}
			else
			{
				editorViewport[a]->Hide();
			}
		}
	}

	for (machine a = 0; a < kEditorViewportCount; a++)
	{
		editorViewport[a]->SetViewportPosition(viewportPosition[a], viewportSize[a]);
	}
}

void Editor::InvalidateViewports(EditorViewportType viewportType)
{
	for (machine a = 0; a < kEditorViewportCount; a++)
	{
		EditorViewport *viewport = editorViewport[a];
		if (viewport->GetEditorViewportType() == viewportType)
		{
			viewport->Invalidate();
		}
	}
}

void Editor::FrameSelectionAllViewports(void)
{
	for (machine a = 0; a < kEditorViewportCount; a++)
	{
		viewportMenuIndex = a;
		HandleFrameSelectionMenuItem(nullptr, nullptr);
	}
}

void Editor::ActivateViewportMenu(int32 index, const Point3D& position)
{
	if (!trackingTool)
	{
		viewportMenuIndex = index;

		Menu *menu = new Menu(kMenuContextual, &viewportMenuItemList);
		menu->SetWidgetPosition(editorViewport[index]->GetViewportWidget()->GetWorldPosition() + position + Vector3D(1.0F, 1.0F, 0.0F));
		TheInterfaceMgr->SetActiveMenu(menu);
	}
}

void Editor::Invalidate(void)
{
	Window::Invalidate();
	editorState |= kEditorRedrawViewports;
}

void Editor::Move(void)
{
	if (editorState & kEditorSelectionUpdated)
	{
		PostEvent(EditorEvent(kEditorEventSelectionUpdated));
	}

	unsigned_int32 state = editorState;
	if (state & kEditorProcessProperties)
	{
		editorWorld->ProcessWorldProperties();
	}

	if ((state & kEditorRedrawViewports) && (!GetFirstSubwindow()))
	{
		state &= ~kEditorRedrawViewports;

		for (machine a = 0; a < kEditorViewportCount; a++)
		{
			editorViewport[a]->Invalidate();
		}
	}

	if (state & kEditorDisableSelectionItems)
	{
		DisableSelectionItems();
	}
	else if (state & kEditorEnableSelectionItems)
	{
		EnableSelectionItems();
	}
	else if (state & kEditorUpdateConditionalItems)
	{
		UpdateConditionalItems();
	}

	editorState = state & ~(kEditorWaitUpdate | kEditorSelectionUpdated | kEditorEnableSelectionItems | kEditorDisableSelectionItems | kEditorUpdateConditionalItems);

	if (gizmoTarget)
	{
		GetManipulator(gizmoTarget->GetNode())->GetGizmo()->Reset();
	}

	if (TheInterfaceMgr->GetActiveWindow() == this)
	{
		Cursor *cursor = nullptr;

		if (!TheInterfaceMgr->GetActiveMenu())
		{
			if (trackingTool)
			{
				cursor = currentCursor;
			}

			const Point3D& position = TheInterfaceMgr->GetCursorPosition();

			for (machine a = 0; a < kEditorViewportCount; a++)
			{
				EditorViewport *viewport = editorViewport[a];
				ViewportWidget *viewportWidget = viewport->GetViewportWidget();
				if (viewportWidget->Visible())
				{
					Vector2D p = position.GetVector2D() - viewportWidget->GetWorldPosition().GetVector2D();
					if ((p.x >= 0.0F) && (p.x < viewportWidget->GetWidgetSize().x) && (p.y >= 0.0F) && (p.y < viewportWidget->GetWidgetSize().y))
					{
						if (!trackingTool)
						{
							if (InterfaceMgr::GetOptionKey())
							{
								cursor = TheWorldEditor->GetEditorCursor(kEditorCursorHand);
							}
							else
							{
								cursor = currentCursor;
							}

							if ((!cursor) && (editorState & kEditorSelectionLocked))
							{
								cursor = TheWorldEditor->GetEditorCursor(kEditorCursorLock);
							}
						}

						viewport->Hover(p);
						break;
					}
				}
			}
		}

		TheInterfaceMgr->SetCursor(cursor);
	}

	Window::Move();

	rootNode->Update();
	GetManipulator(rootNode)->Update();
}

void Editor::EnterForeground(void)
{
	Window::EnterForeground();

	if (editorClipboard)
	{
		editorMenuItem[kEditorMenuPaste]->Enable();
	}

	editorState |= kEditorUpdateConditionalItems;
}

void Editor::EnterBackground(void)
{
	Window::EnterBackground();
	TheInterfaceMgr->SetCursor(nullptr);
}

bool Editor::HandleKeyboardEvent(const KeyboardEventData *eventData)
{
	EventType eventType = eventData->eventType;

	if (eventType == kEventKeyDown)
	{
		if (trackingTool)
		{
			FreeCameraTool *tool = static_cast<FreeCameraTool *>(standardTool[kEditorToolFreeCamera]);
			if (trackingTool == tool)
			{
				unsigned_int32 keyCode = eventData->keyCode;
				if (keyCode - 'a' < 26U)
				{
					keyCode -= 0x0020;
				}

				unsigned_int32 flags = tool->GetCameraFlags();

				if (keyCode == 'W')
				{
					flags |= kFreeCameraForward;
				}
				else if (keyCode == 'S')
				{
					flags |= kFreeCameraBackward;
				}
				else if (keyCode == 'A')
				{
					flags |= kFreeCameraLeft;
				}
				else if (keyCode == 'D')
				{
					flags |= kFreeCameraRight;
				}
				else if (keyCode == ' ')
				{
					flags |= kFreeCameraUp;
				}
				else if (keyCode == 'C')
				{
					flags |= kFreeCameraDown;
				}

				tool->SetCameraFlags(flags);
			}

			return (true);
		}
		else
		{
			unsigned_int32 keyCode = eventData->keyCode;

			if (keyCode == kKeyCodeEscape)
			{
				SetFocusWidget(nullptr);
				return (true);
			}

			if (!GetFocusWidget())
			{
				if (keyCode == kKeyCodeTab)
				{
					if (gizmoTarget)
					{
						const NodeReference *first = selectionList.First();
						const NodeReference *last = selectionList.Last();
						if (first != last)
						{
							const NodeReference		*reference;

							if (!(eventData->modifierKeys & kModifierKeyShift))
							{
								reference = gizmoTarget->Next();
								if (!reference)
								{
									reference = first;
								}
							}
							else
							{
								reference = gizmoTarget->Previous();
								if (!reference)
								{
									reference = last;
								}
							}

							DisableGizmo(gizmoTarget);
							EnableGizmo(reference);
						}

						return (true);
					}
				}
				else if ((keyCode == kKeyCodeDelete) || (keyCode == kKeyCodeBackspace))
				{
					editorState |= kEditorWaitUpdate;
					HandleClearMenuItem(nullptr, nullptr);
					return (true);
				}
				else if (keyCode - '0' < 10U)
				{
					if (keyCode == '0')
					{
						toolButton[kEditorToolNodeBoxSelect]->SetValue(1, true);
					}
					else if (keyCode == '1')
					{
						toolButton[kEditorToolNodeSelect]->SetValue(1, true);
					}
					else if (keyCode == '2')
					{
						toolButton[kEditorToolNodeMove]->SetValue(1, true);
					}
					else if (keyCode == '3')
					{
						toolButton[kEditorToolNodeRotate]->SetValue(1, true);
					}
					else if (keyCode == '4')
					{
						toolButton[kEditorToolNodeResize]->SetValue(1, true);
					}
					else if (keyCode == '5')
					{
						toolButton[kEditorToolConnect]->SetValue(1, true);
					}
					else if (keyCode == '6')
					{
						toolButton[kEditorToolSurfaceSelect]->SetValue(1, true);
					}
					else if (keyCode == '8')
					{
						toolButton[kEditorToolOrbitCamera]->SetValue(1, true);
					}
					else if (keyCode == '9')
					{
						toolButton[kEditorToolFreeCamera]->SetValue(1, true);
					}

					return (true);
				}
			}
		}
	}
	else if (eventType == kEventKeyUp)
	{
		if (trackingTool)
		{
			FreeCameraTool *tool = static_cast<FreeCameraTool *>(standardTool[kEditorToolFreeCamera]);
			if (trackingTool == tool)
			{
				unsigned_int32 keyCode = eventData->keyCode;
				if (keyCode - 'a' < 26U)
				{
					keyCode -= 0x0020;
				}

				unsigned_int32 flags = tool->GetCameraFlags();

				if (keyCode == 'W')
				{
					flags &= ~kFreeCameraForward;
				}
				else if (keyCode == 'S')
				{
					flags &= ~kFreeCameraBackward;
				}
				else if (keyCode == 'A')
				{
					flags &= ~kFreeCameraLeft;
				}
				else if (keyCode == 'D')
				{
					flags &= ~kFreeCameraRight;
				}
				else if (keyCode == ' ')
				{
					flags &= ~kFreeCameraUp;
				}
				else if (keyCode == 'C')
				{
					flags &= ~kFreeCameraDown;
				}

				tool->SetCameraFlags(flags);
			}

			return (true);
		}
	}
	else if (eventType == kEventKeyCommand)
	{
		if (trackingTool)
		{
			return (true);
		}
	}

	return (Window::HandleKeyboardEvent(eventData));
}

void Editor::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		if (widget == bookPane)
		{
			int32 index = bookPane->GetSelection();
			for (machine a = 0; a < kEditorBookCount; a++)
			{
				if (a == index)
				{
					bookWidget[a]->Show();
				}
				else
				{
					bookWidget[a]->Hide();
				}
			}
		}
		else if (widget == flagButton[kEditorFlagBackfaces])
		{
			HandleShowBackfacesMenuItem(editorMenuItem[kEditorMenuShowBackfaces], nullptr);
		}
		else if (widget == flagButton[kEditorFlagExpandWorlds])
		{
			HandleExpandWorldsMenuItem(editorMenuItem[kEditorMenuExpandWorlds], nullptr);
		}
		else if (widget == flagButton[kEditorFlagExpandModels])
		{
			HandleExpandModelsMenuItem(editorMenuItem[kEditorMenuExpandModels], nullptr);
		}
		else if (widget == flagButton[kEditorFlagLighting])
		{
			HandleRenderLightingMenuItem(editorMenuItem[kEditorMenuRenderLighting], nullptr);
		}
		else if (widget == flagButton[kEditorFlagCenter])
		{
			HandleDrawFromCenterMenuItem(editorMenuItem[kEditorMenuDrawFromCenter], nullptr);
		}
		else if (widget == flagButton[kEditorFlagCap])
		{
			HandleCapGeometryMenuItem(editorMenuItem[kEditorMenuCapGeometry], nullptr);
		}
	}
}

void Editor::CloseDialogComplete(Dialog *dialog, void *cookie)
{
	Editor *editor = static_cast<Editor *>(cookie);

	int32 status = dialog->GetDialogStatus();
	if (status == kDialogOkay)
	{
		if (editor->resourceName[0] == 0)
		{
			editor->HandleSaveWorldAsMenuItem(nullptr, nullptr);
		}
		else
		{
			editor->SaveWorld();
			editor->Window::Close();
		}
	}
	else if (status == kDialogIgnore)
	{
		editor->Window::Close();
	}
}

void Editor::Close(void)
{
	if (editorState & kEditorWorldUnsaved)
	{
		const StringTable *table = TheWorldEditor->GetStringTable();

		const char *title = table->GetString(StringID('SAVE'));
		const char *okayText = table->GetString(StringID('BTTN', 'SAVE'));
		const char *cancelText = table->GetString(StringID('BTTN', 'CANC'));
		const char *ignoreText = table->GetString(StringID('BTTN', 'DSAV'));

		Dialog *dialog = new Dialog(Vector2D(342.0F, 120.0F), title, okayText, cancelText, ignoreText);
		dialog->SetCompletionProc(&CloseDialogComplete, this);
		dialog->SetIgnoreKeyCode('n');

		PushButtonWidget *button = dialog->GetIgnoreButton();
		const Point3D& position = button->GetWidgetPosition();
		button->SetWidgetPosition(Point3D(position.x + 8.0F, position.y, position.z));
		button->SetWidgetSize(Vector2D(100.0F, button->GetWidgetSize().y));

		ImageWidget *image = new ImageWidget(Vector2D(64.0F, 64.0F), "C4/warning");
		image->SetWidgetPosition(Point3D(12.0F, 12.0F, 0.0F));
		dialog->AppendSubnode(image);

		String<127> confirmText(table->GetString(StringID('CFRM')));
		confirmText += GetWindowTitle();
		confirmText += '?';

		TextWidget *text = new TextWidget(Vector2D(242.0F, 0.0F), confirmText, "font/Gui");
		text->SetTextFlags(kTextWrapped);
		text->SetWidgetPosition(Point3D(88.0F, 16.0F, 0.0F));
		dialog->AppendSubnode(text);

		AddSubwindow(dialog);
	}
	else
	{
		Window::Close();
	}
}


WorldEditor::WorldEditor() :
		Singleton<WorldEditor>(TheWorldEditor),
		objectCreator(&CreateObject),
		manipulatorCreator(&EditorManipulator::Create),
		stringTable("WorldEditor/strings"),

		sceneGraphColorObserver(this, &WorldEditor::HandleSceneGraphColorEvent),
		surfaceColorObserver(this, &WorldEditor::HandleSurfaceColorEvent),

		worldCommandObserver(this, &WorldEditor::HandleWorldCommand),
		panelCommandObserver(this, &WorldEditor::HandlePanelCommand),
		modelCommandObserver(this, &WorldEditor::HandleModelCommand),
		waterCommandObserver(this, &WorldEditor::HandleWaterCommand),
		worldCommand("world", &worldCommandObserver),
		panelCommand("panel", &panelCommandObserver),
		modelCommand("model", &modelCommandObserver),
		waterCommand("water", &waterCommandObserver),

		updateWorldsCommandObserver(this, &WorldEditor::HandleUpdateWorldsCommand),
		updateAnimsCommandObserver(this, &WorldEditor::HandleUpdateAnimsCommand),
		updateWorldsCommand("updateworlds", &updateWorldsCommandObserver),
		updateAnimsCommand("updateanims", &updateAnimsCommandObserver),

		newWorldItem(stringTable.GetString(StringID('WRLD', 'MNEW')), WidgetObserver<WorldEditor>(this, &WorldEditor::HandleNewWorldMenuItem), Shortcut('N')),
		openWorldItem(stringTable.GetString(StringID('WRLD', 'MOPN')), WidgetObserver<WorldEditor>(this, &WorldEditor::HandleOpenWorldMenuItem), Shortcut('O')),
		reopenWorldItem(stringTable.GetString(StringID('WRLD', 'MROP')), WidgetObserver<WorldEditor>(this, &WorldEditor::HandleReopenWorldMenuItem), Shortcut('P', kShortcutShift)),
		dividerLine1Item(kLineSolid),
		newPanelItem(stringTable.GetString(StringID('PANL', 'MNEW')), WidgetObserver<WorldEditor>(this, &WorldEditor::HandleNewPanelMenuItem)),
		openPanelItem(stringTable.GetString(StringID('PANL', 'MOPN')), WidgetObserver<WorldEditor>(this, &WorldEditor::HandleOpenPanelMenuItem)),
		dividerLine2Item(kLineSolid),
		openModelItem(stringTable.GetString(StringID('MODL', 'MOPN')), WidgetObserver<WorldEditor>(this, &WorldEditor::HandleOpenModelMenuItem))
{
	Object::InstallCreator(&objectCreator);
	Manipulator::InstallCreator(&manipulatorCreator);

	for (machine a = 0; a < kEditorCursorCount; a++)
	{
		editorCursor[a] = new Cursor(cursorTextureName[a]);
	}

	TheEngine->AddCommand(&worldCommand);
	TheEngine->AddCommand(&panelCommand);
	TheEngine->AddCommand(&modelCommand);
	TheEngine->AddCommand(&waterCommand);
	TheEngine->AddCommand(&updateWorldsCommand);
	TheEngine->AddCommand(&updateAnimsCommand);

	reopenWorldItem.Disable();

	ThePluginMgr->AddToolMenuItem(&openModelItem);
	ThePluginMgr->AddToolMenuItem(&newWorldItem, false);
	ThePluginMgr->AddToolMenuItem(&openWorldItem, false);
	ThePluginMgr->AddToolMenuItem(&reopenWorldItem, false);
	ThePluginMgr->AddToolMenuItem(&dividerLine1Item, false);
	ThePluginMgr->AddToolMenuItem(&newPanelItem, false);
	ThePluginMgr->AddToolMenuItem(&openPanelItem, false);
	ThePluginMgr->AddToolMenuItem(&dividerLine2Item, false);

	TheEngine->InitVariable("sceneGraphColor", "3E7D6B", kVariablePermanent, &sceneGraphColorObserver);
	TheEngine->InitVariable("surfaceColor", "008000", kVariablePermanent, &surfaceColorObserver);
	TheEngine->InitVariable("sceneImportType", "ogex", kVariablePermanent);
	TheEngine->InitVariable("sceneExportType", "", kVariablePermanent);
	TheEngine->InitVariable("modelEnvironment", "ModelViewer/Floor", kVariablePermanent);

	Editor::transformClipboard.SetIdentity();
}

WorldEditor::~WorldEditor()
{
	delete worldPicker.GetTarget();
	delete panelPicker.GetTarget();
	delete modelPicker.GetTarget();

	Editor::windowList.Purge();
	ModelWindow::PurgeWindowList();
	PanelEditor::PurgeWindowList();

	PanelEditor::ReleaseClipboard();

	for (machine a = kEditorCursorCount - 1; a >= 0; a--)
	{
		delete editorCursor[a];
	}
}

void WorldEditor::HandleSceneGraphColorEvent(Variable *variable)
{
	sceneGraphColor = ColorRGB().SetHexString(variable->GetValue());
}

void WorldEditor::HandleSurfaceColorEvent(Variable *variable)
{
	surfaceColor = ColorRGB().SetHexString(variable->GetValue());
}

Object *WorldEditor::CreateObject(Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (data.GetType())
	{
		case kObjectEditor:

			if ((unpackFlags & (kUnpackEditor | kUnpackNonpersistent)) == kUnpackEditor)
			{
				return (new EditorObject);
			}

			break;
	}

	return (nullptr);
}

void WorldEditor::HandleNewWorldMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	Editor::Open(nullptr);
}

void WorldEditor::HandleOpenWorldMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	FilePicker *picker = worldPicker;
	if (picker)
	{
		TheInterfaceMgr->SetActiveWindow(picker);
	}
	else
	{
		const char *title = GetStringTable()->GetString(StringID('WRLD', 'OPEN'));
		picker = new FilePicker('WRLD', title, TheResourceMgr->GetGenericCatalog(), WorldResource::GetDescriptor());
		picker->SetCompletionProc(&WorldPicked);

		worldPicker = picker;
		TheInterfaceMgr->AddWidget(picker);
	}
}

void WorldEditor::HandleReopenWorldMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	TheApplication->UnloadWorld();

	WorldPicked(nullptr, &playedWorldName[0]);
}

void WorldEditor::HandleWorldCommand(Command *command, const char *text)
{
	if (*text != 0)
	{
		ResourceName	name;

		Text::ReadString(text, name, kMaxResourceNameLength);
		WorldPicked(nullptr, &name[0]);
	}
	else
	{
		HandleOpenWorldMenuItem(nullptr, nullptr);
	}
}

void WorldEditor::WorldPicked(FilePicker *picker, void *cookie)
{
	ResourceName	name;

	if (picker)
	{
		name = picker->GetResourceName();
	}
	else
	{
		name = static_cast<const char *>(cookie);
	}

	Editor *editor = Editor::Open(name);
	if (!editor)
	{
		const StringTable *table = TheWorldEditor->GetStringTable();
		String<kMaxCommandLength> output(table->GetString(StringID('WRLD', 'NRES')));
		output += name;
		Engine::Report(output);
	}
}

void WorldEditor::HandleNewPanelMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	PanelEditor::Open(nullptr);
}

void WorldEditor::HandleOpenPanelMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	FilePicker *picker = panelPicker;
	if (picker)
	{
		TheInterfaceMgr->SetActiveWindow(picker);
	}
	else
	{
		const char *title = GetStringTable()->GetString(StringID('PANL', 'OPEN'));
		picker = new FilePicker('PANL', title, TheResourceMgr->GetGenericCatalog(), PanelResource::GetDescriptor());
		picker->SetCompletionProc(&PanelPicked);

		panelPicker = picker;
		TheInterfaceMgr->AddWidget(picker);
	}
}

void WorldEditor::HandlePanelCommand(Command *command, const char *text)
{
	if (*text != 0)
	{
		ResourceName	name;

		Text::ReadString(text, name, kMaxResourceNameLength);
		PanelPicked(nullptr, &name[0]);
	}
	else
	{
		HandleOpenPanelMenuItem(nullptr, nullptr);
	}
}

void WorldEditor::PanelPicked(FilePicker *picker, void *cookie)
{
	ResourceName	name;

	if (picker)
	{
		name = picker->GetResourceName();
	}
	else
	{
		name = static_cast<const char *>(cookie);
	}

	PanelEditor *editor = PanelEditor::Open(name);
	if (!editor)
	{
		const StringTable *table = TheWorldEditor->GetStringTable();
		String<kMaxCommandLength> output(table->GetString(StringID('PANL', 'NRES')));
		output += name;
		Engine::Report(output);
	}
}

void WorldEditor::HandleOpenModelMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	FilePicker *picker = modelPicker;
	if (picker)
	{
		TheInterfaceMgr->SetActiveWindow(picker);
	}
	else
	{
		const char *title = GetStringTable()->GetString(StringID('MODL', 'OPEN'));
		picker = new FilePicker('MODL', title, TheResourceMgr->GetGenericCatalog(), ModelResource::GetDescriptor());
		picker->SetCompletionProc(&ModelPicked);

		modelPicker = picker;
		TheInterfaceMgr->AddWidget(picker);
	}
}

void WorldEditor::HandleModelCommand(Command *command, const char *text)
{
	if (*text != 0)
	{
		ResourceName	name;

		Text::ReadString(text, name, kMaxResourceNameLength);
		ModelPicked(nullptr, &name[0]);
	}
	else
	{
		HandleOpenModelMenuItem(nullptr, nullptr);
	}
}

void WorldEditor::ModelPicked(FilePicker *picker, void *cookie)
{
	ResourceName	name;

	if (picker)
	{
		name = picker->GetResourceName();
	}
	else
	{
		name = static_cast<const char *>(cookie);
	}

	if (ModelWindow::Open(name) != kResourceOkay)
	{
		const StringTable *table = TheWorldEditor->GetStringTable();
		String<kMaxCommandLength> output(table->GetString(StringID('MODL', 'NRES')));
		output += name;
		Engine::Report(output);
	}
}

void WorldEditor::HandleWaterCommand(Command *command, const char *text)
{
	World *world = TheWorldMgr->GetWorld();
	if ((world) && (*text != 0))
	{
		ResourceName	name;

		Text::ReadString(text, name, kMaxResourceNameLength);
		WaterPage::ExportWater(world, name);
	}
}

void WorldEditor::InitializePlugins(EditorObject *editorObject) const
{
	EditorPlugin *plugin = editorPluginList.First();
	while (plugin)
	{
		plugin->Initialize(editorObject);
		plugin = plugin->ListElement<EditorPlugin>::Next();
	}
}

void WorldEditor::PreprocessPlugins(Editor *editor) const
{
	EditorPlugin *plugin = editorPluginList.First();
	while (plugin)
	{
		plugin->Preprocess(editor);
		plugin = plugin->ListElement<EditorPlugin>::Next();
	}
}

void WorldEditor::HandleUpdateWorldsCommand(Command *command, const char *text)
{
	UpdateWorldsDirectory("");
	UpdateModelsDirectory("");
}

void WorldEditor::UpdateWorldsDirectory(const char *directory)
{
	Map<FileReference>		fileMap;

	TheResourceMgr->GetGenericCatalog()->BuildResourceMap(WorldResource::GetDescriptor(), directory, &fileMap);
	FileReference *reference = fileMap.First();
	while (reference)
	{
		String<> path(directory);
		if (directory[0] != 0)
		{
			path += '/';
		}

		if (!(reference->GetFlags() & kFileDirectory))
		{
			ResourceName name(reference->GetName());
			name[Text::GetResourceNameLength(name)] = 0;
			path += name;
			UpdateWorldResource(&path[Text::GetPrefixDirectoryLength(path)]);
		}
		else
		{
			UpdateWorldsDirectory(path += reference->GetName());
		}

		reference = reference->Next();
	}
}

void WorldEditor::UpdateWorldResource(const char *name)
{
	ResourceLocation	resourceLocation;

	WorldResource *resource = WorldResource::Get(name, kResourceIgnorePackFiles, nullptr, &resourceLocation);
	if (resource)
	{
		File			file;
		ResourcePath	path;

		const void *data = resource->GetData();
		const int32 *format = static_cast<const int32 *>(data);
		int32 version = format[1];

		if (version < kEngineInternalVersion)
		{
			Node *rootNode = Node::UnpackTree(data, kUnpackEditor);

			TheResourceMgr->GetGenericCatalog()->GetResourcePath(WorldResource::GetDescriptor(), name, &resourceLocation, &path);
			if (file.Open(path, kFileCreate) == kFileOkay)
			{
				rootNode->PackTree(&file, kPackEditor | kPackInitialize);
			}

			delete rootNode;
		}

		resource->Release();
	}
}

void WorldEditor::UpdateModelsDirectory(const char *directory)
{
	Map<FileReference>		fileMap;

	TheResourceMgr->GetGenericCatalog()->BuildResourceMap(ModelResource::GetDescriptor(), directory, &fileMap);
	FileReference *reference = fileMap.First();
	while (reference)
	{
		String<> path(directory);
		if (directory[0] != 0)
		{
			path += '/';
		}

		if (!(reference->GetFlags() & kFileDirectory))
		{
			ResourceName name(reference->GetName());
			name[Text::GetResourceNameLength(name)] = 0;
			path += name;
			UpdateModelResource(&path[Text::GetPrefixDirectoryLength(path)]);
		}
		else
		{
			UpdateModelsDirectory(path += reference->GetName());
		}

		reference = reference->Next();
	}
}

void WorldEditor::UpdateModelResource(const char *name)
{
	ResourceLocation	resourceLocation;

	ModelResource *resource = ModelResource::Get(name, kResourceIgnorePackFiles, nullptr, &resourceLocation);
	if (resource)
	{
		File			file;
		ResourcePath	path;

		Node *rootNode = Node::UnpackTree(resource->GetData(), kUnpackEditor);
		resource->Release();

		TheResourceMgr->GetGenericCatalog()->GetResourcePath(ModelResource::GetDescriptor(), name, &resourceLocation, &path);

		if (file.Open(path, kFileCreate) == kFileOkay)
		{
			rootNode->PackTree(&file, kPackInitialize);
		}

		delete rootNode;
	}
}

void WorldEditor::HandleUpdateAnimsCommand(Command *command, const char *text)
{
	UpdateAnimsDirectory("");
}

void WorldEditor::UpdateAnimsDirectory(const char *directory)
{
	Map<FileReference>		fileMap;

	TheResourceMgr->GetGenericCatalog()->BuildResourceMap(AnimationResource::GetDescriptor(), directory, &fileMap);
	FileReference *reference = fileMap.First();
	while (reference)
	{
		String<> path(directory);
		if (directory[0] != 0)
		{
			path += '/';
		}

		if (!(reference->GetFlags() & kFileDirectory))
		{
			ResourceName name(reference->GetName());
			name[Text::GetResourceNameLength(name)] = 0;
			path += name;
			path += ".anm";
			UpdateAnimResource(String<>("Data/") += path);
		}
		else
		{
			UpdateAnimsDirectory(path += reference->GetName());
		}

		reference = reference->Next();
	}
}

void WorldEditor::UpdateAnimResource(const char *name)
{
	File	file;

	if (file.Open(name, kFileReadWrite) == kFileOkay)
	{
		unsigned_int64 size = file.GetSize();
		char *buffer = new char[size];
		file.Read(buffer, size);

		const unsigned_int32 *data = reinterpret_cast<unsigned_int32 *>(buffer);
		if (data[0] != 'C4AN')
		{
			static const unsigned_int32 nullHashTable[3] = {1, 0, 0};

			AnimationHeader			animationHeader;
			const unsigned_int32	*hashTable;
			unsigned_int32			hashTableSize;

			file.SetSize(0);
			file.SetPosition(0);
			file.Write(AnimationResource::resourceSignature, 8);

			int32 nodeCount = data[1];
			unsigned_int32 hashTableOffset = data[2];
			int32 trackCount = data[3];
			unsigned_int32 transformTrackOffset = data[5];
			unsigned_int32 cueTrackOffset = (trackCount == 2) ? data[7] : 0;
			const unsigned_int32 *trackData = data + (1 + transformTrackOffset / 4);

			if (hashTableOffset != 0)
			{
				hashTable = data + (1 + hashTableOffset / 4);
				hashTableSize = (trackData - hashTable) * 4;
			}
			else
			{
				hashTable = nullHashTable;
				hashTableSize = 4 + sizeof(TransformTrackHeader::NodeBucket);
			}

			int32 frameCount = trackData[0];
			float frameDuration = reinterpret_cast<const float *>(trackData)[1];
			unsigned_int32 transformTrackSize = nodeCount * frameCount * sizeof(TransformFrameData);

			animationHeader.frameCount = frameCount;
			animationHeader.frameDuration = frameDuration;
			animationHeader.trackCount = trackCount;
			animationHeader.trackData[0].trackType = kTrackTransform;
			animationHeader.trackData[0].trackOffset = sizeof(AnimationHeader) + (trackCount - 1) * sizeof(AnimationHeader::TrackData);

			file.Write(&animationHeader, sizeof(AnimationHeader));

			if (trackCount == 2)
			{
				animationHeader.trackData[0].trackType = kTrackCue;
				animationHeader.trackData[0].trackOffset += sizeof(TransformTrackHeader) - 4 - sizeof(TransformTrackHeader::NodeBucket) + hashTableSize + transformTrackSize;
				file.Write(&animationHeader.trackData[0], sizeof(AnimationHeader::TrackData));
			}

			unsigned_int32 trackHeaderSize = hashTableSize + 8;

			file.Write(&nodeCount, 4);
			file.Write(&trackHeaderSize, 4);
			file.Write(hashTable, hashTableSize);
			file.Write(&trackData[2], transformTrackSize);

			if (trackCount == 2)
			{
				trackData = data + (1 + cueTrackOffset / 4);
				int32 cueCount = trackData[0];
				file.Write(trackData, 4 + cueCount * sizeof(CueData));
			}
		}

		delete[] buffer;
	}
}

// ZYUQURM
