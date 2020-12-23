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


#ifndef C4EditorCommands_h
#define C4EditorCommands_h


#include "C4Threads.h"
#include "C4Cameras.h"
#include "C4Spaces.h"
#include "C4EditorBase.h"


namespace C4
{
	enum
	{
		kMaxCombineGeometryCount	= 4
	};


	enum
	{
		kEditorMenuUndo,
		kEditorMenuCut,
		kEditorMenuCopy,
		kEditorMenuPaste,
		kEditorMenuPasteSubnodes,
		kEditorMenuClear,
		kEditorMenuSelectAllZone,
		kEditorMenuSelectSubtree,
		kEditorMenuSelectSuperNode,
		kEditorMenuLockSelection,
		kEditorMenuUnlockSelection,
		kEditorMenuDuplicate,
		kEditorMenuClone,

		kEditorMenuGetInfo,
		kEditorMenuEditController,
		kEditorMenuGroup,
		kEditorMenuResetTransform,
		kEditorMenuAlignToGrid,
		kEditorMenuSetTargetZone,
		kEditorMenuMoveToTargetZone,
		kEditorMenuConnectNode,
		kEditorMenuUnconnectNode,
		kEditorMenuConnectInfiniteZone,
		kEditorMenuSelectConnectedNode,
		kEditorMenuSelectIncomingConnectingNodes,
		kEditorMenuMoveViewportCameraToNode,
		kEditorMenuOpenInstancedWorld,

		kEditorMenuRebuildGeometry,
		kEditorMenuRebuildWithNewPath,
		kEditorMenuRecalculateNormals,
		kEditorMenuBakeTransformIntoVertices,
		kEditorMenuRepositionMeshOrigin,
		kEditorMenuSetMaterial,
		kEditorMenuRemoveMaterial,
		kEditorMenuCombineDetailLevels,
		kEditorMenuSeparateDetailLevels,
		kEditorMenuConvertToGenericMesh,
		kEditorMenuMergeGeometry,
		kEditorMenuInvertGeometry,
		kEditorMenuIntersectGeometry,
		kEditorMenuUnionGeometry,
		kEditorMenuGenerateAmbientOcclusion,
		kEditorMenuRemoveAmbientOcclusion,

		kEditorMenuHideSelection,
		kEditorMenuShowBackfaces,
		kEditorMenuExpandWorlds,
		kEditorMenuExpandModels,
		kEditorMenuRenderLighting,
		kEditorMenuDrawFromCenter,
		kEditorMenuCapGeometry,

		kEditorMenuItemCount
	};


	struct PickData;


	class MeshOriginWindow : public Window
	{
		private:

			Editor					*worldEditor;

			PushButtonWidget		*okayButton;
			PushButtonWidget		*cancelButton;

			RadioWidget				*radioButton[3][3];

			void CommitSettings(void) const;

		public:

			MeshOriginWindow(Editor *editor);
			~MeshOriginWindow();

			void Preprocess(void) override;
			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override; 
	};

 
	class GenerateAmbientOcclusionWindow : public Window
	{ 
		private:

			class AmbientOcclusionJob : public Job 
			{
				private: 
 
					GenerateAmbientOcclusionWindow		*jobWindow;

				public:
 
					AmbientOcclusionJob(GenerateAmbientOcclusionWindow *window, ExecuteProc *execProc, void *cookie);

					GenerateAmbientOcclusionWindow *GetJobWindow(void) const
					{
						return (jobWindow);
					}
			};

			Editor					*worldEditor;
			float					blockageMultiplier;

			int32					jobCount;
			Job						**jobTable;

			Lock					jobLock;

			PushButtonWidget		*startButton;
			PushButtonWidget		*cancelButton;
			EditTextWidget			*intensityBox;
			TextWidget				*inputText;

			PushButtonWidget		*stopButton;
			ProgressWidget			*progressBar;
			BorderWidget			*borderWidget;
			TextWidget				*messageText;

			void StartJob(void);

			static void JobGenerateAmbientOcclusion(Job *job, void *cookie);

		public:

			GenerateAmbientOcclusionWindow(Editor *editor);
			~GenerateAmbientOcclusionWindow();

			void Preprocess(void) override;
			void Move(void) override;

			bool HandleKeyboardEvent(const KeyboardEventData *eventData) override;
			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};


	class GenerateRadiosityDataWindow : public Window
	{
		private:

			enum
			{
				kRenderSize		= 128,
				kRenderArea		= kRenderSize * kRenderSize
			};

			Editor								*worldEditor;

			int32								spaceIndex;
			Array<const RadiositySpace *, 1>	spaceArray;

			CubeCamera							cubeCamera;
			float								nearDistance;
			float								cameraOffset;

			Integer3D							sampleCoord;
			Vector3D							sampleDelta;

			int32								imageVoxelCount;
			Color4C								*imageBuffer[6];
			unsigned_int32						*valueData;
			Color2C								*colorData;

			PushButtonWidget					*stopButton;
			ProgressWidget						*progressBar;
			TextWidget							*countText;

			void BeginRadiositySpace(void);
			void EndRadiositySpace(void);

			static bool DetectInteriorCollision(const World *world, const Point3D& p1, const Point3D& p2, CollisionData *data);

		public:

			GenerateRadiosityDataWindow(Editor *editor);
			~GenerateRadiosityDataWindow();

			void Preprocess(void) override;
			void Move(void) override;

			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};
}


#endif

// ZYUQURM
