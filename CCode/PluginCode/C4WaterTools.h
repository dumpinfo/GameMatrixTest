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


#ifndef C4WaterTools_h
#define C4WaterTools_h


#include "C4GeometryManipulators.h"
#include "C4EditorPages.h"
#include "C4Water.h"


namespace C4
{
	enum
	{
		kEditorPageWater			= 'WATR'
	};


	enum
	{
		kOperationWaterRebuild		= 'WRBD'
	};


	class WaterResource : public Resource<WaterResource>
	{
		friend class Resource<WaterResource>;

		private:

			static ResourceDescriptor	descriptor;

			~WaterResource();

		public:

			WaterResource(const char *name, ResourceCatalog *catalog);
	};


	class WaterBlockManipulator : public EditorManipulator
	{
		private:

			Vector4D				blockSizeVector;
			List<Attribute>			blockAttributeList;
			DiffuseAttribute		blockDiffuseColor;
			Renderable				blockRenderable;

			bool CalculateNodeSphere(BoundingSphere *sphere) const;

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			WaterBlockManipulator(WaterBlock *block);
			~WaterBlockManipulator();

			WaterBlock *GetTargetNode(void) const
			{
				return (static_cast<WaterBlock *>(EditorManipulator::GetTargetNode()));
			}

			WaterBlockObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			const char *GetDefaultNodeName(void) const override;

			void Select(void) override;
			void Unselect(void) override;

			void HandleSizeUpdate(int32 count, const float *size) override;

			bool ReparentedSubnodesAllowed(void) const override;

			Box3D CalculateNodeBoundingBox(void) const override;

			bool Pick(const Ray *ray, PickData *data) const override;
			bool RegionPick(const VisibilityRegion *region) const override;

			bool Resize(const ManipulatorResizeData *resizeData) override;
			void RescaleBlock(float scale);

			void Render(const ManipulatorRenderData *renderData) override;
	};


	class WaterGeometryManipulator : public GeometryManipulator
	{
		public:

			WaterGeometryManipulator(WaterGeometry *water);
			~WaterGeometryManipulator();

			WaterGeometry *GetTargetNode(void) const
			{
				return (static_cast<WaterGeometry *>(EditorManipulator::GetTargetNode()));
			}
 
			WaterGeometryObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject()); 
			}
 
			const char *GetDefaultNodeName(void) const override;

			bool ReparentedSubnodesAllowed(void) const override; 
	};
 
 
	class HorizonWaterGeometryManipulator : public GeometryManipulator
	{
		public:
 
			HorizonWaterGeometryManipulator(HorizonWaterGeometry *water);
			~HorizonWaterGeometryManipulator();

			HorizonWaterGeometry *GetTargetNode(void) const
			{
				return (static_cast<HorizonWaterGeometry *>(EditorManipulator::GetTargetNode()));
			}

			HorizonWaterGeometryObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			const char *GetDefaultNodeName(void) const override;

			bool ReparentedSubnodesAllowed(void) const override;
	};


	class WaterPage : public EditorPage, public EditorTool
	{
		private:

			enum
			{
				kWaterMenuGenerateLandHeight,
				kWaterMenuRemoveLandHeight,
				kWaterMenuRebuildWaterBlock,
				kWaterMenuImportWaveData,
				kWaterMenuRemoveWaveData,
				kWaterMenuItemCount
			};

			int32						currentTool;

			IconButtonWidget			*waterButton;

			IconButtonWidget			*menuButton;
			MenuItemWidget				*waterMenuItem[kWaterMenuItemCount];
			List<MenuItemWidget>		waterMenuItemList;

			WidgetObserver<WaterPage>	waterButtonObserver;
			WidgetObserver<WaterPage>	menuButtonObserver;

			void HandleWaterButtonEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleMenuButtonEvent(Widget *widget, const WidgetEventData *eventData);

			void HandleGenerateLandHeightMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData);
			void HandleRemoveLandHeightMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData);
			void HandleRebuildWaterBlockMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData);
			void HandleImportWaveDataMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData);
			void HandleRemoveWaveDataMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData);

			static void WaterPicked(FilePicker *picker, void *cookie);

		public:

			WaterPage();
			~WaterPage();

			void Preprocess(void) override;

			void Engage(Editor *editor, void *cookie) override;
			void Disengage(Editor *editor, void *cookie) override;

			bool BeginTool(Editor *editor, EditorTrackData *trackData) override;
			bool TrackTool(Editor *editor, EditorTrackData *trackData) override;
			bool EndTool(Editor *editor, EditorTrackData *trackData) override;

			static void ExportWater(const World *world, const ResourceName& resourceName);
	};


	class WaterRebuildOperation : public Operation
	{
		private:

			WaterBlock		*blockNode;
			Integer2D		blockSize;
			float			waterFieldScale;
			float			waterHorizonDistance;

		public:

			WaterRebuildOperation(WaterBlock *block);
			~WaterRebuildOperation();

			void Restore(Editor *editor) override;
	};


	class WaterBuilder : public Configurable
	{
		private:

			Integer2D		waterSize;

			float			waterHorizonDistance;
			bool			horizonFlag[4];

		public:

			WaterBuilder(const WaterBlock *block);
			~WaterBuilder();

			const bool *GetHorizonFlagArray(void) const
			{
				return (horizonFlag);
			}

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void BuildWater(Job *job, WaterBlock *block);
	};


	class BuildWaterWindow : public Window
	{
		private:

			Editor					*worldEditor;
			WaterBlock				*blockNode;

			WaterBuilder			*waterBuilder;

			PushButtonWidget		*buildButton;
			PushButtonWidget		*cancelButton;

			ConfigurationWidget		*configurationWidget;

		public:

			BuildWaterWindow(Editor *editor, WaterBlock *block);
			~BuildWaterWindow();

			void Preprocess(void) override;
			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};


	class WaterProgressWindow : public Window
	{
		private:

			Editor					*worldEditor;
			WaterBlock				*blockNode;
			WaterBuilder			*waterBuilder;

			Job						waterJob;
			volatile bool			buildSuccess;
			Array<Geometry *>		geometryArray;

			PushButtonWidget		*stopButton;
			ProgressWidget			*progressBar;

			static void JobBuildWater(Job *job, void *cookie);

		public:

			WaterProgressWindow(Editor *editor, WaterBlock *block, WaterBuilder *builder);
			~WaterProgressWindow();

			void Preprocess(void) override;
			void Move(void) override;

			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};


	class GenerateLandElevationWindow : public Window
	{
		private:

			class LandElevationJob : public Job
			{
				private:

					GenerateLandElevationWindow		*jobWindow;

				public:

					LandElevationJob(GenerateLandElevationWindow *window, ExecuteProc *execProc, void *cookie);

					GenerateLandElevationWindow *GetJobWindow(void) const
					{
						return (jobWindow);
					}
			};

			Editor					*worldEditor;

			int32					jobCount;
			Job						**jobTable;

			Lock					jobLock;

			PushButtonWidget		*stopButton;
			ProgressWidget			*progressBar;

			void StartJob(void);

			static bool DetectCollision(const Node *root, Ray *ray, PickData *pickData);
			static void JobGenerateLandElevation(Job *job, void *cookie);

		public:

			GenerateLandElevationWindow(Editor *editor);
			~GenerateLandElevationWindow();

			static bool ValidGeometry(const Geometry *geometry);

			void Preprocess(void) override;
			void Move(void) override;

			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};
}


#endif

// ZYUQURM
