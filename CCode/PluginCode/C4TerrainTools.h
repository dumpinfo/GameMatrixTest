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


#ifndef C4TerrainTools_h
#define C4TerrainTools_h


#include "C4GeometryManipulators.h"
#include "C4EditorPages.h"
#include "C4Terrain.h"


namespace C4
{
	enum
	{
		kTerrainEditLevelCount	= 3
	};


	enum
	{
		kEditorPageTerrain			= 'TERR'
	};


	enum
	{
		kWidgetTerrainTexture		= 'terr'
	};


	enum
	{
		kOperationTerrainPaint		= 'TPNT',
		kOperationTerrainRebuild	= 'TRBD'
	};


	class SphereGeometry;
	class CylinderGeometry;
	class TerrainTextureWindow;


	class TerrainBlockManipulator : public EditorManipulator
	{
		private:

			int32					renderMode;
			int32					renderTool;
			int32					renderBrush;

			SphereGeometry			*sphereBrush;
			CylinderGeometry		*cylinderBrush;
			DiffuseAttribute		*brushDiffuseColor;

			Vector4D				blockSizeVector;
			List<Attribute>			blockAttributeList;
			DiffuseAttribute		blockDiffuseColor;
			Renderable				blockRenderable;

			List<Attribute>			darkAttributeList;
			DiffuseAttribute		darkDiffuseColor;
			Renderable				sphereRenderable;
			Renderable				cylinderRenderable;

			bool CalculateNodeSphere(BoundingSphere *sphere) const;

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

			static void CloneBrushRenderable(const Renderable *brush, Renderable *renderable);

		public:

			TerrainBlockManipulator(TerrainBlock *block);
			~TerrainBlockManipulator();

			TerrainBlock *GetTargetNode(void) const
			{
				return (static_cast<TerrainBlock *>(EditorManipulator::GetTargetNode()));
			}

			TerrainBlockObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			const char *GetDefaultNodeName(void) const override;

			void BuildBrush(float radius, float height);
			void SetBrushTransform(const Point3D& position, const Vector3D& normal);

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

			void SetRenderTool(int32 mode, int32 tool, int32 brush);
	}; 

 
	class TerrainGeometryManipulator : public GeometryManipulator, public ListElement<TerrainGeometryManipulator> 
	{
		private:

			char		*paintMeshStorage; 
			Point3D		*paintMeshVertex;
			Triangle	*paintMeshTriangle;
			int32		paintTriangleCount;

		public:

			TerrainGeometryManipulator(TerrainGeometry *terrain);
			~TerrainGeometryManipulator();

			TerrainGeometry *GetTargetNode(void) const
			{
				return (static_cast<TerrainGeometry *>(EditorManipulator::GetTargetNode()));
			}

			TerrainGeometryObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			const char *GetDefaultNodeName(void) const override;

			bool ReparentedSubnodesAllowed(void) const override;

			bool Pick(const Ray *ray, PickData *data) const override;
			bool RegionPick(const VisibilityRegion *region) const override;

			void CopyTriangleMesh(List<TerrainGeometryManipulator> *terrainPaintList);

			bool GetPaintPosition(const Ray *ray, int32 mode, bool modifiedFlag, GeometryHitData *geometryHitData);
			void CleanupPaintMesh(void);
	};


	class TerrainPaintOperation : public Operation
	{
		private:

			TerrainBlock		*blockNode;
			int32				terrainChannel[2];

			char				*undoStorage;
			char				**drawingChannelData[2];

			int32				terrainCount[kTerrainEditLevelCount];
			MaterialObject		**materialObject[kTerrainEditLevelCount];
			Vector4D			*terrainTextureParam[kTerrainEditLevelCount];
			unsigned_int8		*terrainFlags[kTerrainEditLevelCount];

		public:

			TerrainPaintOperation(TerrainBlock *block, int32 channel1, int32 channel2);
			~TerrainPaintOperation();

			bool AddTerrain(const Integer3D& coord, int32 level);
			bool TerrainModified(const Integer3D& coord, int32 level) const;

			VoxelMap *OpenSavedVoxelMap(const Integer3D& vmin, const Integer3D& vmax) const;
			static void CloseSavedVoxelMap(VoxelMap *voxelMap);

			void Restore(Editor *editor) override;
	};


	class TerrainTextureWidget final : public RenderableWidget
	{
		private:

			struct ImageVertex
			{
				Point2D		position;
				ColorRGB	color;
				Point3D		texcoord;
			};

			int32						textureGroupIndex;
			Color3C						*textureSelector;
			unsigned_int32				paletteIndex[3];
			int32						activeFace;
			int32						hiliteFace;

			int32						entryCountX;
			int32						entryCountY;

			VertexBuffer				imageVertexBuffer;
			List<Attribute>				imageAttributeList;
			DiffuseTextureAttribute		imageTextureAttribute;

			VertexBuffer				backgroundVertexBuffer;
			List<Attribute>				backgroundAttributeList;
			DiffuseAttribute			backgroundColorAttribute;
			Renderable					backgroundRenderable;

			void SetHilitedFace(int32 face);

			static void SelectPaletteEntry(TerrainTextureWindow *window, TerrainTextureWidget *widget);

		public:

			TerrainTextureWidget(int32 groupIndex);
			~TerrainTextureWidget();

			WidgetPart TestPosition(const Point3D& position) const override;

			void Preprocess(void) override;
			void Build(void) override;
			void Render(List<Renderable> *renderList) override;

			void HandleMouseEvent(const PanelMouseEventData *eventData) override;

			void SetTexture(const char *name);
			void SetTerrainMaterial(TerrainMaterial *material);
	};


	class PaletteWidget final : public ImageWidget
	{
		public:

			PaletteWidget(const Vector2D& size, Texture *texture);
			~PaletteWidget();

			void HandleMouseEvent(const PanelMouseEventData *eventData) override;
	};


	class TerrainTextureWindow : public Window, public Completable<TerrainTextureWindow, TerrainTextureWidget *>
	{
		private:

			Texture				*paletteTexture;

			int32				entryCount;
			unsigned_int32		selectedEntry;

			PaletteWidget		*paletteImage[49];

		public:

			TerrainTextureWindow(Texture *texture, unsigned_int32 selection);
			~TerrainTextureWindow();

			unsigned_int32 GetSelectedEntry(void) const
			{
				return (selectedEntry);
			}

			void Preprocess(void) override;

			bool HandleKeyboardEvent(const KeyboardEventData *eventData) override;
			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};


	class OptimizeTerrainWindow : public Window
	{
		private:

			Editor					*worldEditor;

			int32					jobCount;
			Job						**jobTable;

			PushButtonWidget		*stopButton;
			ProgressWidget			*progressBar;

			static void OptimizeTerrainJob(Job *job, void *cookie);

		public:

			OptimizeTerrainWindow(Editor *editor);
			~OptimizeTerrainWindow();

			void Preprocess(void) override;
			void Move(void) override;

			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};


	class RebuildTerrainBlockWindow : public Window
	{
		private:

			Editor					*worldEditor;
			TerrainBlock			*blockNode;

			PushButtonWidget		*okayButton;
			PushButtonWidget		*cancelButton;

			EditTextWidget			*countBox[3];
			EditTextWidget			*offsetBox[3];

			void CommitSettings(void) const;

		public:

			RebuildTerrainBlockWindow(Editor *editor, TerrainBlock *block);
			~RebuildTerrainBlockWindow();

			void Preprocess(void) override;
			void Move(void) override;

			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};


	class TerrainRebuildOperation : public Operation
	{
		private:

			TerrainBlock		*blockNode;
			Integer3D			blockSize;
			float				voxelScale;

		public:

			TerrainRebuildOperation(TerrainBlock *block);
			~TerrainRebuildOperation();

			void Restore(Editor *editor) override;
	};


	class TerrainPage : public EditorPage, public EditorTool
	{
		public:

			enum
			{
				kEditorTerrainModeBlock,
				kEditorTerrainModeSculpt,
				kEditorTerrainModePaint,
				kEditorTerrainModePickup
			};

			enum
			{
				kEditorTerrainSculptAdd,
				kEditorTerrainSculptSubtract,
				kEditorTerrainSculptMin,
				kEditorTerrainSculptMax,
				kEditorTerrainSculptSmooth,
				kEditorTerrainSculptCount
			};

			enum
			{
				kEditorTerrainPaintBlend1,
				kEditorTerrainPaintBlend2,
				kEditorTerrainPaintMaterial,
				kEditorTerrainPaintHole,
				kEditorTerrainPaintCount
			};

			enum
			{
				kEditorTerrainBrushSphere,
				kEditorTerrainBrushCylinder,
				kEditorTerrainBrushSlope,
				kEditorTerrainBrushCount
			};

			enum
			{
				kEditorTerrainPlaneNone,
				kEditorTerrainPlaneHorizontal,
				kEditorTerrainPlaneTangent,
				kEditorTerrainPlaneCamera,
				kEditorTerrainPlaneCount
			};

			enum
			{
				kEditorTerrainSlopeCount = 4
			};

			enum
			{
				kTerrainMenuOptimizeTerrain,
				kTerrainMenuRebuildTerrainBlock,
				kTerrainMenuItemCount
			};

		private:

			EditorObserver<TerrainPage>		editorObserver;

			int32							currentMode;
			int32							currentTool;
			int32							currentBrush;
			int32							currentSlope;
			int32							currentPlane;

			int32							radiusValue;
			int32							heightValue;
			int32							offsetValue;
			int32							bulldozerValue;
			int32							blendValue;
			int32							fuzzyValue;
			int32							stylusValue;
			int32							materialValue;

			TerrainBlock					*drawingBlock;
			int32							drawingChannel;

			Antivector4D					drawingPlane;
			Antivector3D					brushNormal;

			float							drawingRadius;
			float							drawingHeight;
			float							drawingOffset;
			Point3D							previousPoint;

			TerrainMaterial					terrainMaterial;

			TerrainPaintOperation			*paintOperation;

			IconButtonWidget				*blockButton;
			IconButtonWidget				*pickupButton;
			IconButtonWidget				*sculptButton[kEditorTerrainSculptCount];
			IconButtonWidget				*paintButton[kEditorTerrainPaintCount];
			IconButtonWidget				*brushButton[kEditorTerrainBrushCount];
			IconButtonWidget				*slopeButton[kEditorTerrainSlopeCount];

			PopupMenuWidget					*planeMenu;
			CheckWidget						*stylusWidget;
			CheckWidget						*materialWidget;

			SliderWidget					*radiusSlider;
			TextWidget						*radiusText;

			SliderWidget					*heightSlider;
			TextWidget						*heightText;

			SliderWidget					*offsetSlider;
			TextWidget						*offsetText;

			SliderWidget					*bulldozerSlider;
			TextWidget						*bulldozerText;

			SliderWidget					*blendSlider;
			TextWidget						*blendText;

			SliderWidget					*fuzzySlider;
			TextWidget						*fuzzyText;

			TerrainTextureWidget			*terrainPaintWidget[2];

			IconButtonWidget				*menuButton;
			MenuItemWidget					*terrainMenuItem[kTerrainMenuItemCount];
			List<MenuItemWidget>			terrainMenuItemList;

			WidgetObserver<TerrainPage>		toolObserver;
			WidgetObserver<TerrainPage>		brushObserver;
			WidgetObserver<TerrainPage>		slopeObserver;
			WidgetObserver<TerrainPage>		planeObserver;
			WidgetObserver<TerrainPage>		sliderObserver;
			WidgetObserver<TerrainPage>		checkObserver;
			WidgetObserver<TerrainPage>		menuButtonObserver;

			static const float				slopeTerm[kEditorTerrainSlopeCount][4];

			List<TerrainGeometryManipulator>	terrainPaintList;

			float GetBrushRadius(void) const
			{
				return ((float) radiusValue * 0.5F + 0.5F);
			}

			float GetBrushHeight(void) const
			{
				return ((float) (heightValue + 10) * 0.01F);
			}

			float GetBrushOffset(void) const
			{
				return ((float) (offsetValue - 50) * 0.02F);
			}

			int32 GetBulldozerBias(void) const
			{
				return (bulldozerValue - 32);
			}

			float GetTextureBlend(void) const
			{
				return ((float) blendValue * 0.01F);
			}

			float GetBlendFuzziness(void) const
			{
				return ((float) fuzzyValue * 0.01F);
			}

			bool GetMaterialUpdateState(void) const
			{
				return (materialValue != 0);
			}

			void UpdateSlider(SliderWidget *widget);
			void UpdateMaterial(const MaterialObject *materialObject);

			void HandleEditorEvent(Editor *editor, const EditorEvent& event);

			void HandleToolButtonEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleBrushEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleSlopeEvent(Widget *widget, const WidgetEventData *eventData);
			void HandlePlaneEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleSliderEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleCheckEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleMenuButtonEvent(Widget *widget, const WidgetEventData *eventData);

			void HandleOptimizeTerrainMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData);
			void HandleRebuildTerrainBlockMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData);

			bool DetectTerrainIntersection(const Node *node, int32 mode, Ray *ray, CollisionPoint *collisionPoint) const;

			void DrawTerrain(Editor *editor, const Point3D& endpoint1, const Point3D& endpoint2);

			void DrawSphericalAdditiveBrush(VoxelMap *voxelMap, VoxelMap *materialMap, const Point3D& p1, const Point3D& p2, UnsignedVoxel material);
			void DrawCylindricalAdditiveBrush(VoxelMap *voxelMap, VoxelMap *materialMap, const Point3D& p1, const Point3D& p2, const Antivector4D& topPlane, const Antivector4D& bottomPlane, UnsignedVoxel material);
			void DrawSlopeAdditiveBrush(VoxelMap *voxelMap, VoxelMap *materialMap, const Point3D& p1, const Point3D& p2, UnsignedVoxel material);

			void DrawSphericalSubtractiveBrush(VoxelMap *voxelMap, VoxelMap *materialMap, const Point3D& p1, const Point3D& p2, UnsignedVoxel material);
			void DrawCylindricalSubtractiveBrush(VoxelMap *voxelMap, VoxelMap *materialMap, const Point3D& p1, const Point3D& p2, const Antivector4D& topPlane, const Antivector4D& bottomPlane, UnsignedVoxel material);
			void DrawSlopeSubtractiveBrush(VoxelMap *voxelMap, VoxelMap *materialMap, const Point3D& p1, const Point3D& p2, UnsignedVoxel material);

			void DrawSphericalMinBrush(VoxelMap *voxelMap, VoxelMap *materialMap, const VoxelMap *savedVoxelMap, const Point3D& p1, const Point3D& p2, UnsignedVoxel material);
			void DrawCylindricalMinBrush(VoxelMap *voxelMap, VoxelMap *materialMap, const VoxelMap *savedVoxelMap, const Point3D& p1, const Point3D& p2, const Antivector4D& topPlane, const Antivector4D& bottomPlane, UnsignedVoxel material);

			void DrawSphericalMaxBrush(VoxelMap *voxelMap, VoxelMap *materialMap, const VoxelMap *savedVoxelMap, const Point3D& p1, const Point3D& p2, UnsignedVoxel material);
			void DrawCylindricalMaxBrush(VoxelMap *voxelMap, VoxelMap *materialMap, const VoxelMap *savedVoxelMap, const Point3D& p1, const Point3D& p2, const Antivector4D& topPlane, const Antivector4D& bottomPlane, UnsignedVoxel material);

			void DrawSphericalSmoothingBrush(VoxelMap *voxelMap, VoxelMap *materialMap, const VoxelMap *savedVoxelMap, const Point3D& p1, const Point3D& p2, UnsignedVoxel material);
			void DrawCylindricalSmoothingBrush(VoxelMap *voxelMap, VoxelMap *materialMap, const VoxelMap *savedVoxelMap, const Point3D& p1, const Point3D& p2, const Antivector4D& topPlane, const Antivector4D& bottomPlane, UnsignedVoxel material);

			void DrawSphericalMinPaintBrush(VoxelMap *drawingMap, VoxelMap *materialMap, const Point3D& p1, const Point3D& p2, float fuzzy, UnsignedVoxel blend, UnsignedVoxel material);
			void DrawCylindricalMinPaintBrush(VoxelMap *drawingMap, VoxelMap *materialMap, const Point3D& p1, const Point3D& p2, const Antivector4D& topPlane, const Antivector4D& bottomPlane, float fuzzy, UnsignedVoxel blend, UnsignedVoxel material);
			void DrawSphericalMaxPaintBrush(VoxelMap *drawingMap, VoxelMap *materialMap, const Point3D& p1, const Point3D& p2, float fuzzy, UnsignedVoxel blend, UnsignedVoxel material);
			void DrawCylindricalMaxPaintBrush(VoxelMap *drawingMap, VoxelMap *materialMap, const Point3D& p1, const Point3D& p2, const Antivector4D& topPlane, const Antivector4D& bottomPlane, float fuzzy, UnsignedVoxel blend, UnsignedVoxel material);

			void DrawSphericalMaterialBrush(VoxelMap *materialMap, const Point3D& p1, const Point3D& p2, UnsignedVoxel material);
			void DrawCylindricalMaterialBrush(VoxelMap *materialMap, const Point3D& p1, const Point3D& p2, const Antivector4D& topPlane, const Antivector4D& bottomPlane, UnsignedVoxel material);

		public:

			TerrainPage();
			~TerrainPage();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Preprocess(void) override;

			void Engage(Editor *editor, void *cookie) override;
			void Disengage(Editor *editor, void *cookie) override;

			bool BeginTool(Editor *editor, EditorTrackData *trackData) override;
			bool TrackTool(Editor *editor, EditorTrackData *trackData) override;
			bool EndTool(Editor *editor, EditorTrackData *trackData) override;

			static void UpdateBlock(Editor *editor, TerrainBlock *block, const Integer3D& coord, int32 level, MaterialObject *materialObject, unsigned_int32 flags = 0);
	};
}


#endif

// ZYUQURM
