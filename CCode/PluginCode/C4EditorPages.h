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


#ifndef C4EditorPages_h
#define C4EditorPages_h


//# \component	World Editor
//# \prefix		WorldEditor/

//# \import		C4EditorBase.h


#include "C4WorldEditor.h"
#include "C4Paint.h"


namespace C4
{
	typedef Type	PageType;


	const float kEditorPageWidth = 241.0F;


	enum
	{
		kEditorPageGeometries		= 'GEOM',
		kEditorPageLights			= 'LITE',
		kEditorPageSources			= 'SORC',
		kEditorPageZones			= 'ZONE',
		kEditorPagePortals			= 'PORT',
		kEditorPageTriggers			= 'TRIG',
		kEditorPageMarkers			= 'MARK',
		kEditorPagePaths			= 'PATH',
		kEditorPageEffects			= 'EFCT',
		kEditorPageParticles		= 'PART',
		kEditorPageAtmosphere		= 'ATMS',
		kEditorPagePhysics			= 'PHYS',
		kEditorPageMaterial			= 'MATL',
		kEditorPagePaint			= 'PANT',
		kEditorPageTextureMapping	= 'TXTR',
		kEditorPageWorlds			= 'WRLD',
		kEditorPageModels			= 'MODL',
		kEditorPageImpostors		= 'IPST',
		kEditorPagePlacement		= 'PLAC',
		kEditorPageNodeManagement	= 'NDMG',
		kEditorPageGrid				= 'GRID',
		kEditorPageViewports		= 'VIEW',
		kEditorPageTransform		= 'XFRM',
		kEditorPageInfo				= 'INFO',
		kEditorPageFind				= 'FIND'
	};


	enum
	{
		kEditorLayout1,
		kEditorLayout4,
		kEditorLayout2H,
		kEditorLayout2V,
		kEditorLayout3L,
		kEditorLayout3R,
		kEditorLayout3T,
		kEditorLayout3B,
		kEditorLayoutCount
	};


	class ParticleSystemRegistration;
	class ModelRegistration;
	class MaterialBox;
	class MaterialContainer;
	class MaterialWindow;
	class EditorObject;
	class ColorPicker;
	class PathMarker;
	struct EditorToolData;


	class TypeWidget final : public TextWidget
	{
		private:

			Type		itemType;

		public:

			C4EDITORAPI TypeWidget(const Vector2D& size, const char *text, Type type);
			C4EDITORAPI ~TypeWidget();

			Type GetItemType(void) const
			{
				return (itemType);
			}
	};


	class SelectionMask : public Tree<SelectionMask>
	{
		private:

			Type		nodeType;
			int32		maskValue; 

		public:
 
			SelectionMask(Type type = 0);
			~SelectionMask(); 

			Type GetNodeType(void) const
			{ 
				return (nodeType);
			} 
 
			int32 GetMaskValue(void) const
			{
				return (maskValue);
			} 

			void SetMaskValue(int32 value);
	};


	//# \class	EditorPage		Represents a World Editor tool page.
	//
	//# The $EditorPage$ class represents a World Editor tool page.
	//
	//# \def	class EditorPage : public Page, public Packable, public ListElement<EditorPage>, public MapElement<EditorPage>
	//
	//# \ctor	EditorPage(PageType type, const char *panelName, int32 index = kEditorBookObject);
	//
	//# The constructor has protected access. Only instances of $EditorPage$ subclasses can be created.
	//
	//# \param	type		The type of the page.
	//# \param	panelName	The name of the panel resource to load for the page.
	//# \param	index		The index of the book to which the page belongs.
	//
	//# \desc
	//# The $EditorPage$ class is the base class for all World Editor tool pages.
	//#
	//# The $index$ parameter specifies which book the page belongs to. It can be one of the following values.
	//
	//# \table	EditorBook
	//
	//# \privbase	Page								The $EditorPage$ class is an extension of a general tool page.
	//# \base		Utilities/ListElement<EditorPage>	Used internally by the World Editor.
	//# \base		Utilities/MapElement<EditorPage>	Used internally by the World Editor.


	class EditorPage : public Page, public ListElement<EditorPage>, public MapElement<EditorPage>
	{
		friend class EditorObject;

		private:

			PageType			pageType;
			PageType			prevPageType;

			int32				bookIndex;
			unsigned_int32		pageState;

			Editor				*worldEditor;

		protected:

			C4EDITORAPI EditorPage(PageType type, const char *panelName, int32 index = kEditorBookObject);

		public:

			typedef ConstCharKey KeyType;

			C4EDITORAPI virtual ~EditorPage();

			KeyType GetKey(void) const
			{
				return (GetPageTitle());
			}

			PageType GetPageType(void) const
			{
				return (pageType);
			}

			int32 GetBookIndex(void) const
			{
				return (bookIndex);
			}

			unsigned_int32 GetPageState(void) const
			{
				return (pageState);
			}

			Editor *GetEditor(void) const
			{
				return (worldEditor);
			}

			void SetEditor(Editor *editor)
			{
				worldEditor = editor;
			}

			C4EDITORAPI void Pack(Packer& data, unsigned_int32 packFlags) const override;
			C4EDITORAPI void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			C4EDITORAPI void SetWidgetState(unsigned_int32 state) override;
			C4EDITORAPI void Preprocess(void) override;
	};


	class GeometriesPage : public EditorPage, public EditorTool
	{
		private:

			enum
			{
				kEditorGeometryPlate,
				kEditorGeometryDisk,
				kEditorGeometryHole,
				kEditorGeometryAnnulus,
				kEditorGeometryBox,
				kEditorGeometryPyramid,
				kEditorGeometryCylinder,
				kEditorGeometryCone,
				kEditorGeometrySphere,
				kEditorGeometryDome,
				kEditorGeometryTorus,
				kEditorGeometryTruncatedCone,
				kEditorGeometryTube,
				kEditorGeometryExtrusion,
				kEditorGeometryRevolution,
				kEditorGeometryRope,
				kEditorGeometryCloth,
				kEditorGeometryCount
			};

			int32								currentTool;

			IconButtonWidget					*geometryButton[kEditorGeometryCount];
			WidgetObserver<GeometriesPage>		geometryButtonObserver;

			void HandleGeometryButtonEvent(Widget *widget, const WidgetEventData *eventData);

		public:

			GeometriesPage();
			~GeometriesPage();

			void Preprocess(void) override;

			void Engage(Editor *editor, void *cookie) override;
			void Disengage(Editor *editor, void *cookie) override;

			bool BeginTool(Editor *editor, EditorTrackData *trackData) override;
			bool TrackTool(Editor *editor, EditorTrackData *trackData) override;
			bool EndTool(Editor *editor, EditorTrackData *trackData) override;
	};


	class LightsPage : public EditorPage, public EditorTool
	{
		private:

			enum
			{
				kEditorLightInfinite,
				kEditorLightPoint,
				kEditorLightCube,
				kEditorLightSpot,
				kEditorLightCount
			};

			enum
			{
				kEditorSpaceShadow,
				kEditorSpaceRadiosity,
				kEditorSpaceCount
			};

			enum
			{
				kEditorLightModeLight,
				kEditorLightModeSpace
			};

			int32							currentMode;
			int32							currentTool;

			IconButtonWidget				*lightButton[kEditorLightCount];
			IconButtonWidget				*spaceButton[kEditorSpaceCount];

			PushButtonWidget				*generateRadiosityButton;

			EditorObserver<LightsPage>		editorObserver;
			WidgetObserver<LightsPage>		lightButtonObserver;
			WidgetObserver<LightsPage>		generateRadiosityButtonObserver;

			void HandleEditorEvent(Editor *editor, const EditorEvent& event);
			void HandleLightButtonEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleGenerateRadiosityButtonEvent(Widget *widget, const WidgetEventData *eventData);

		public:

			LightsPage();
			~LightsPage();

			void Preprocess(void) override;

			void Engage(Editor *editor, void *cookie) override;
			void Disengage(Editor *editor, void *cookie) override;

			bool BeginTool(Editor *editor, EditorTrackData *trackData) override;
			bool TrackTool(Editor *editor, EditorTrackData *trackData) override;
			bool EndTool(Editor *editor, EditorTrackData *trackData) override;
	};


	class SourcesPage : public EditorPage, public EditorTool
	{
		private:

			enum
			{
				kEditorSourceAmbient,
				kEditorSourceOmni,
				kEditorSourceDirected,
				kEditorSourceCount
			};

			enum
			{
				kEditorSourceModeSource,
				kEditorSourceModeSpace
			};

			int32							currentMode;
			int32							currentTool;

			IconButtonWidget				*sourceButton[kEditorSourceCount];
			IconButtonWidget				*acousticsSpaceButton;

			WidgetObserver<SourcesPage>		sourceButtonObserver;

			void HandleSourceButtonEvent(Widget *widget, const WidgetEventData *eventData);

		public:

			SourcesPage();
			~SourcesPage();

			void Preprocess(void) override;

			void Engage(Editor *editor, void *cookie) override;
			void Disengage(Editor *editor, void *cookie) override;

			bool BeginTool(Editor *editor, EditorTrackData *trackData) override;
			bool TrackTool(Editor *editor, EditorTrackData *trackData) override;
			bool EndTool(Editor *editor, EditorTrackData *trackData) override;
	};


	class ZonesPage : public EditorPage, public EditorTool
	{
		private:

			enum
			{
				kEditorZoneBox,
				kEditorZoneCylinder,
				kEditorZonePolygon,
				kEditorZoneCount
			};

			enum
			{
				kEditorZoneModeDraw,
				kEditorZoneModeTool
			};

			enum
			{
				kEditorZoneToolInsert,
				kEditorZoneToolRemove,
				kEditorZoneToolCount
			};

			int32						currentMode;
			int32						currentTool;

			IconButtonWidget			*zoneButton[kEditorZoneCount];
			IconButtonWidget			*toolButton[kEditorZoneToolCount];

			WidgetObserver<ZonesPage>	zoneButtonObserver;

			void HandleZoneButtonEvent(Widget *widget, const WidgetEventData *eventData);

		public:

			ZonesPage();
			~ZonesPage();

			void Preprocess(void) override;

			void Engage(Editor *editor, void *cookie) override;
			void Disengage(Editor *editor, void *cookie) override;

			bool BeginTool(Editor *editor, EditorTrackData *trackData) override;
			bool TrackTool(Editor *editor, EditorTrackData *trackData) override;
			bool EndTool(Editor *editor, EditorTrackData *trackData) override;
	};


	class PortalsPage : public EditorPage, public EditorTool
	{
		private:

			enum
			{
				kEditorPortalDirect,
				kEditorPortalRemote,
				kEditorPortalOcclusion,
				kEditorPortalCount
			};

			enum
			{
				kEditorPortalModePortal,
				kEditorPortalModeSpace,
				kEditorPortalModeTool
			};

			enum
			{
				kEditorPortalToolInsert,
				kEditorPortalToolRemove,
				kEditorPortalToolCount
			};

			int32							currentMode;
			int32							currentTool;

			IconButtonWidget				*portalButton[kEditorPortalCount];
			IconButtonWidget				*toolButton[kEditorPortalToolCount];
			IconButtonWidget				*occlusionSpaceButton;

			PushButtonWidget				*createOpposingButton;

			EditorObserver<PortalsPage>		editorObserver;
			WidgetObserver<PortalsPage>		portalButtonObserver;
			WidgetObserver<PortalsPage>		createOpposingButtonObserver;

			void HandleEditorEvent(Editor *editor, const EditorEvent& event);
			void HandlePortalButtonEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleCreateOpposingButtonEvent(Widget *widget, const WidgetEventData *eventData);

		public:

			PortalsPage();
			~PortalsPage();

			void Preprocess(void) override;

			void Engage(Editor *editor, void *cookie) override;
			void Disengage(Editor *editor, void *cookie) override;

			bool BeginTool(Editor *editor, EditorTrackData *trackData) override;
			bool TrackTool(Editor *editor, EditorTrackData *trackData) override;
			bool EndTool(Editor *editor, EditorTrackData *trackData) override;
	};


	class TriggersPage : public EditorPage, public EditorTool
	{
		private:

			enum
			{
				kEditorTriggerBox,
				kEditorTriggerCylinder,
				kEditorTriggerSphere,
				kEditorTriggerCount
			};

			int32							currentTool;

			IconButtonWidget				*triggerButton[kEditorTriggerCount];
			WidgetObserver<TriggersPage>	triggerButtonObserver;

			void HandleTriggerButtonEvent(Widget *widget, const WidgetEventData *eventData);

		public:

			TriggersPage();
			~TriggersPage();

			void Preprocess(void) override;

			void Engage(Editor *editor, void *cookie) override;
			void Disengage(Editor *editor, void *cookie) override;

			bool BeginTool(Editor *editor, EditorTrackData *trackData) override;
			bool TrackTool(Editor *editor, EditorTrackData *trackData) override;
			bool EndTool(Editor *editor, EditorTrackData *trackData) override;
	};


	class MarkersPage : public EditorPage, public EditorTool
	{
		private:

			enum
			{
				kEditorMarkerLocator,
				kEditorMarkerConnection,
				kEditorMarkerCube,
				kEditorMarkerShader,
				kEditorMarkerCount
			};

			int32							currentTool;

			ListWidget						*locatorList;
			IconButtonWidget				*markerButton[kEditorMarkerCount];

			WidgetObserver<MarkersPage>		markerButtonObserver;

			void HandleMarkerButtonEvent(Widget *widget, const WidgetEventData *eventData);

		public:

			MarkersPage();
			~MarkersPage();

			void Preprocess(void) override;

			void Engage(Editor *editor, void *cookie) override;
			void Disengage(Editor *editor, void *cookie) override;

			bool BeginTool(Editor *editor, EditorTrackData *trackData) override;
			bool TrackTool(Editor *editor, EditorTrackData *trackData) override;
			bool EndTool(Editor *editor, EditorTrackData *trackData) override;
	};


	class PathsPage : public EditorPage, public EditorTool
	{
		private:

			enum
			{
				kEditorPathLinear,
				kEditorPathElliptical,
				kEditorPathBezier,
				kEditorPathCount
			};

			int32							currentTool;
			PathMarker						*targetPath;

			IconButtonWidget				*pathButton[kEditorPathCount];
			WidgetObserver<PathsPage>		pathButtonObserver;

			void HandlePathButtonEvent(Widget *widget, const WidgetEventData *eventData);

			static bool SnapToBeginning(const PathMarker *marker, const EditorTrackData *trackData);

		public:

			PathsPage();
			~PathsPage();

			void Preprocess(void) override;

			void Engage(Editor *editor, void *cookie) override;
			void Disengage(Editor *editor, void *cookie) override;

			bool BeginTool(Editor *editor, EditorTrackData *trackData) override;
			bool TrackTool(Editor *editor, EditorTrackData *trackData) override;
			bool EndTool(Editor *editor, EditorTrackData *trackData) override;
	};


	class EffectsPage : public EditorPage, public EditorTool
	{
		private:

			enum
			{
				kEditorEffectQuad,
				kEditorEffectFlare,
				kEditorEffectBeam,
				kEditorEffectTube,
				kEditorEffectFire,
				kEditorEffectBoxShaft,
				kEditorEffectCylinderShaft,
				kEditorEffectTruncatedPyramidShaft,
				kEditorEffectTruncatedConeShaft,
				kEditorEffectPanel,
				kEditorEffectCount
			};

			enum
			{
				kEditorEffectModeEffect,
				kEditorEffectModeCamera
			};

			int32							currentMode;
			int32							currentTool;

			IconButtonWidget				*effectButton[kEditorEffectCount];
			IconButtonWidget				*frustumCameraButton;

			WidgetObserver<EffectsPage>		effectButtonObserver;

			void HandleEffectButtonEvent(Widget *widget, const WidgetEventData *eventData);

		public:

			EffectsPage();
			~EffectsPage();

			void Preprocess(void) override;

			void Engage(Editor *editor, void *cookie) override;
			void Disengage(Editor *editor, void *cookie) override;

			bool BeginTool(Editor *editor, EditorTrackData *trackData) override;
			bool TrackTool(Editor *editor, EditorTrackData *trackData) override;
			bool EndTool(Editor *editor, EditorTrackData *trackData) override;
	};


	class ParticlesPage : public EditorPage, public EditorTool
	{
		private:

			enum
			{
				kEditorEmitterBox,
				kEditorEmitterCylinder,
				kEditorEmitterSphere,
				kEditorEmitterCount
			};

			enum
			{
				kParticleMenuSelectAll,
				kParticleMenuGenerateEmitterHeightMap,
				kParticleMenuItemCount
			};

			class GenerateHeightFieldWindow : public Window
			{
				private:

					Editor					*worldEditor;

					int32					jobCount;
					Job						**jobTable;

					PushButtonWidget		*stopButton;
					ProgressWidget			*progressBar;

					static void JobGenerateHeightField(Job *job, void *cookie);

				public:

					GenerateHeightFieldWindow(Editor *editor);
					~GenerateHeightFieldWindow();

					void Preprocess(void) override;
					void Move(void) override;

					void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
			};

			int32							currentTool;

			IconButtonWidget				*menuButton;
			MenuItemWidget					*particleMenuItem[kParticleMenuItemCount];
			List<MenuItemWidget>			particleMenuItemList;

			IconButtonWidget				*emitterButton[kEditorEmitterCount];
			ListWidget						*particleSystemList;

			WidgetObserver<ParticlesPage>	menuButtonObserver;
			WidgetObserver<ParticlesPage>	emitterButtonObserver;
			WidgetObserver<ParticlesPage>	particleSystemListObserver;

			void HandleSelectAllMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData);
			void HandleGenerateEmitterHeightMapMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData);

			void HandleMenuButtonEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleEmitterButtonEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleParticleSystemListEvent(Widget *widget, const WidgetEventData *eventData);

		public:

			ParticlesPage();
			~ParticlesPage();

			void Preprocess(void) override;

			void Engage(Editor *editor, void *cookie) override;
			void Disengage(Editor *editor, void *cookie) override;

			bool BeginTool(Editor *editor, EditorTrackData *trackData) override;
			bool TrackTool(Editor *editor, EditorTrackData *trackData) override;
			bool EndTool(Editor *editor, EditorTrackData *trackData) override;
	};


	class AtmospherePage : public EditorPage, public EditorTool
	{
		private:

			enum
			{
				kEditorAtmosphereModeSkybox,
				kEditorAtmosphereModeFogSpace
			};

			int32							currentMode;
			int32							currentTool;

			IconButtonWidget				*skyboxButton;
			IconButtonWidget				*fogSpaceButton;

			WidgetObserver<AtmospherePage>	atmosphereButtonObserver;

			void HandleAtmosphereButtonEvent(Widget *widget, const WidgetEventData *eventData);

		public:

			AtmospherePage();
			~AtmospherePage();

			void Preprocess(void) override;

			void Engage(Editor *editor, void *cookie) override;
			void Disengage(Editor *editor, void *cookie) override;

			bool BeginTool(Editor *editor, EditorTrackData *trackData) override;
			bool TrackTool(Editor *editor, EditorTrackData *trackData) override;
			bool EndTool(Editor *editor, EditorTrackData *trackData) override;
	};


	class PhysicsPage : public EditorPage, public EditorTool
	{
		private:

			enum
			{
				kEditorShapeBox,
				kEditorShapePyramid,
				kEditorShapeCylinder,
				kEditorShapeCone,
				kEditorShapeSphere,
				kEditorShapeDome,
				kEditorShapeCapsule,
				kEditorShapeTruncatedPyramid,
				kEditorShapeTruncatedCone,
				kEditorShapeTruncatedDome,
				kEditorShapeCount
			};

			enum
			{
				kEditorJointSpherical,
				kEditorJointUniversal,
				kEditorJointDiscal,
				kEditorJointRevolute,
				kEditorJointCylindrical,
				kEditorJointPrismatic,
				kEditorJointCount
			};

			enum
			{
				kEditorFieldBox,
				kEditorFieldCylinder,
				kEditorFieldSphere,
				kEditorFieldCount
			};

			enum
			{
				kEditorBlockerPlate,
				kEditorBlockerBox,
				kEditorBlockerCylinder,
				kEditorBlockerSphere,
				kEditorBlockerCapsule,
				kEditorBlockerCount
			};

			enum
			{
				kEditorPhysicsModeNode,
				kEditorPhysicsModeSpace,
				kEditorPhysicsModeShape,
				kEditorPhysicsModeJoint,
				kEditorPhysicsModeField,
				kEditorPhysicsModeBlocker
			};

			int32							currentMode;
			int32							currentTool;

			IconButtonWidget				*physicsNodeButton;
			IconButtonWidget				*physicsSpaceButton;
			IconButtonWidget				*shapeButton[kEditorShapeCount];
			IconButtonWidget				*jointButton[kEditorJointCount];
			IconButtonWidget				*fieldButton[kEditorFieldCount];
			IconButtonWidget				*blockerButton[kEditorBlockerCount];

			WidgetObserver<PhysicsPage>		physicsButtonObserver;

			void HandlePhysicsButtonEvent(Widget *widget, const WidgetEventData *eventData);

		public:

			PhysicsPage();
			~PhysicsPage();

			void Preprocess(void) override;

			void Engage(Editor *editor, void *cookie) override;
			void Disengage(Editor *editor, void *cookie) override;

			bool BeginTool(Editor *editor, EditorTrackData *trackData) override;
			bool TrackTool(Editor *editor, EditorTrackData *trackData) override;
			bool EndTool(Editor *editor, EditorTrackData *trackData) override;
	};


	class MaterialPage : public EditorPage, public EditorTool
	{
		private:

			enum
			{
				kEditorMaterialToolPickup,
				kEditorMaterialToolCount
			};

			int32							currentTool;

			Link<Node>						pickupNode;
			int32							pickupIndex;

			IconButtonWidget				*toolButton[kEditorMaterialToolCount];
			MaterialWidget					*materialWidget;

			EditorObserver<MaterialPage>	editorObserver;
			WidgetObserver<MaterialPage>	toolButtonObserver;
			WidgetObserver<MaterialPage>	materialWidgetObserver;

			void HandleEditorEvent(Editor *editor, const EditorEvent& event);
			void HandleToolButtonEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleMaterialWidgetEvent(Widget *widget, const WidgetEventData *eventData);

		public:

			MaterialPage();
			~MaterialPage();

			void Preprocess(void) override;

			void Engage(Editor *editor, void *cookie) override;
			void Disengage(Editor *editor, void *cookie) override;

			bool BeginTool(Editor *editor, EditorTrackData *trackData) override;
	};


	class PaintPage : public EditorPage, public EditorTool
	{
		private:

			enum
			{
				kEditorPaintModeSpace,
				kEditorPaintModeTool
			};

			enum
			{
				kEditorPaintToolBrush,
				kEditorPaintToolCount
			};

			enum
			{
				kPaintMenuAssociatePaintSpace,
				kPaintMenuDissociatePaintSpace,
				kPaintMenuSelectAssociatedPaintSpaces,
				kPaintMenuSelectAssociatedNodes,
				kPaintMenuItemCount
			};

			int32							currentMode;
			int32							currentTool;

			PaintSpace						*targetPaintSpace;
			Painter							*painter;

			PaintState						paintState;
			Point2D							previousPosition;

			bool							channelMask[4];
			ColorRGBA						brushColor;

			int32							invertValue;
			int32							stylusValue;
			int32							strengthValue;
			int32							radiusValue;
			int32							fuzzyValue;

			IconButtonWidget				*paintSpaceButton;
			IconButtonWidget				*toolButton[kEditorPaintToolCount];
			IconButtonWidget				*channelButton[4];
			ColorWidget						*colorWidget;
			CheckWidget						*invertWidget;
			CheckWidget						*stylusWidget;
			ImageWidget						*imageWidget;

			SliderWidget					*strengthSlider;
			TextWidget						*strengthText;

			SliderWidget					*radiusSlider;
			TextWidget						*radiusText;

			SliderWidget					*fuzzySlider;
			TextWidget						*fuzzyText;

			IconButtonWidget				*menuButton;
			MenuItemWidget					*paintMenuItem[kPaintMenuItemCount];
			List<MenuItemWidget>			paintMenuItemList;

			EditorObserver<PaintPage>		editorObserver;
			WidgetObserver<PaintPage>		paintButtonObserver;
			WidgetObserver<PaintPage>		channelButtonObserver;
			WidgetObserver<PaintPage>		colorObserver;
			WidgetObserver<PaintPage>		checkObserver;
			WidgetObserver<PaintPage>		sliderObserver;
			WidgetObserver<PaintPage>		menuButtonObserver;

			float GetBrushStrength(void) const
			{
				return ((float) strengthValue * 0.01F);
			}

			float GetBrushRadius(void) const
			{
				return ((float) (radiusValue + 1));
			}

			float GetBrushFuzziness(void) const
			{
				return ((float) fuzzyValue * 0.01F);
			}

			void UpdateSlider(SliderWidget *widget);
			void UpdateImage(void);

			void HandleEditorEvent(Editor *editor, const EditorEvent& event);
			void HandlePaintButtonEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleChannelButtonEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleColorEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleCheckEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleSliderEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleMenuButtonEvent(Widget *widget, const WidgetEventData *eventData);

			void HandleAssociatePaintSpaceMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData);
			void HandleDissociatePaintSpaceMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData);
			void HandleSelectAssociatedPaintSpacesMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData);
			void HandleSelectAssociatedNodesMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData);

			static bool PaintPickFilter(const Node *node, const PickData *pickData, const void *cookie);

		public:

			PaintPage();
			~PaintPage();

			PaintSpace *GetTargetPaintSpace(void) const
			{
				return (targetPaintSpace);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Preprocess(void) override;

			void Engage(Editor *editor, void *cookie) override;
			void Disengage(Editor *editor, void *cookie) override;

			bool BeginTool(Editor *editor, EditorTrackData *trackData) override;
			bool TrackTool(Editor *editor, EditorTrackData *trackData) override;
			bool EndTool(Editor *editor, EditorTrackData *trackData) override;

			void SetTargetPaintSpace(PaintSpace *paintSpace);
	};


	class TextureMappingPage : public EditorPage, public EditorTool
	{
		private:

			enum
			{
				kEditorTextureToolOffset,
				kEditorTextureToolRotate,
				kEditorTextureToolScale,
				kEditorTextureToolCount
			};

			bool									operationFlag;
			int32									textureOperationType;

			Geometry								*targetGeometry;
			unsigned_int32							targetSurfaceIndex;

			int32									currentTool;
			IconButtonWidget						*toolButton[kEditorTextureToolCount];

			EditTextWidget							*offsetTextWidget[2];
			EditTextWidget							*scaleTextWidget[2];
			EditTextWidget							*rotationTextWidget;
			CheckWidget								*reflectionCheckWidget;
			PopupMenuWidget							*modePopupMenu[2];
			PopupMenuWidget							*planePopupMenu;

			EditorObserver<TextureMappingPage>		editorObserver;
			WidgetObserver<TextureMappingPage>		toolButtonObserver;
			WidgetObserver<TextureMappingPage>		offsetTextObserver;
			WidgetObserver<TextureMappingPage>		scaleTextObserver;
			WidgetObserver<TextureMappingPage>		rotationTextObserver;
			WidgetObserver<TextureMappingPage>		reflectionBoxObserver;
			WidgetObserver<TextureMappingPage>		modeMenuObserver;

			void UpdateTextureAlignData(void);

			void HandleEditorEvent(Editor *editor, const EditorEvent& event);
			void HandleToolButtonEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleOffsetTextEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleScaleTextEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleRotationTextEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleReflectionBoxEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleModeMenuEvent(Widget *widget, const WidgetEventData *eventData);

			static void OffsetTexcoords(const GeometryObject *object, unsigned_int32 index, const Vector2D& offset);
			static void RotateTexcoords(const GeometryObject *object, unsigned_int32 index, const Transform4D& rotation);
			static void ScaleTexcoords(const GeometryObject *object, unsigned_int32 index, const Vector2D& scale);

		public:

			TextureMappingPage();
			~TextureMappingPage();

			void Preprocess(void) override;

			void Engage(Editor *editor, void *cookie) override;
			void Disengage(Editor *editor, void *cookie) override;

			bool BeginTool(Editor *editor, EditorTrackData *trackData) override;
			bool TrackTool(Editor *editor, EditorTrackData *trackData) override;
			bool EndTool(Editor *editor, EditorTrackData *trackData) override;
	};


	class WorldsPage : public EditorPage, public EditorTool
	{
		private:

			enum
			{
				kPlaceDefault			= 0,
				kPlaceIgnoreInstances	= 'IGNR'
			};

			enum
			{
				kWorldMenuSelectAll,
				kWorldMenuSelectSome,
				kWorldMenuExpandAllInSceneGraph,
				kWorldMenuCollapseAllInSceneGraph,
				kWorldMenuReplaceInstances,
				kWorldMenuReplaceModifiers,
				kWorldMenuNewModifierPreset,
				kWorldMenuDeleteModifierPreset,
				kWorldMenuRenameModifierPreset,
				kWorldMenuItemCount
			};

			class WorldWidget final : public TextWidget, public MapElement<WorldWidget>
			{
				public:

					typedef FileNameKey KeyType;

					WorldWidget(const char *text);
					~WorldWidget();

					KeyType GetKey(void) const
					{
						return (GetText());
					}
			};

			class ModifierWidget final : public TextWidget, public MapElement<ModifierWidget>
			{
				public:

					typedef StringKey KeyType;

					List<Modifier>		modifierList;

					ModifierWidget(const char *text = nullptr, const Instance *instance = nullptr);
					~ModifierWidget();

					KeyType GetKey(void) const
					{
						return (GetText());
					}

					void Prepack(List<Object> *linkList) const override;
					void Pack(Packer& data, unsigned_int32 packFlags) const override;
					void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
					bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			};

			class SelectSomeWindow : public Window, public Completable<SelectSomeWindow>
			{
				private:

					unsigned_int32			selectPercentage;

					PushButtonWidget		*okayButton;
					PushButtonWidget		*cancelButton;
					EditTextWidget			*percentBox;

				public:

					SelectSomeWindow(unsigned_int32 percentage);
					~SelectSomeWindow();

					unsigned_int32 GetSelectPercentage(void) const
					{
						return (selectPercentage);
					}

					void Preprocess(void) override;
					void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
			};

			class ModifierPresetWindow : public Window, public Completable<ModifierPresetWindow>
			{
				private:

					WorldsPage				*worldsPage;
					String<>				presetName;

					PushButtonWidget		*okayButton;
					PushButtonWidget		*cancelButton;
					EditTextWidget			*nameBox;

				public:

					ModifierPresetWindow(WorldsPage *page, const char *name = "");
					~ModifierPresetWindow();

					const char *GetModifierPresetName(void) const
					{
						return (nameBox->GetText());
					}

					void Preprocess(void) override;
					void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
			};

			int32							currentTool;

			Map<WorldWidget>				worldWidgetMap;
			Map<ModifierWidget>				modifierWidgetMap;

			int32							modifierIndex;
			unsigned_int32					selectPercentage;
			Type							placeFilter;

			IconButtonWidget				*worldButton;
			ListWidget						*worldList;
			ListWidget						*modifierList;
			CheckWidget						*placeWidget;

			IconButtonWidget				*menuButton;
			MenuItemWidget					*worldMenuItem[kWorldMenuItemCount];
			List<MenuItemWidget>			worldMenuItemList;

			EditorObserver<WorldsPage>		editorObserver;
			WidgetObserver<WorldsPage>		worldButtonObserver;
			WidgetObserver<WorldsPage>		worldListObserver;
			WidgetObserver<WorldsPage>		modifierListObserver;
			WidgetObserver<WorldsPage>		placeWidgetObserver;
			WidgetObserver<WorldsPage>		menuButtonObserver;

			void AddWorldWidget(const char *text);
			void AddInstances(void);

			void BuildWorldList(void);
			void BuildModifierList(void);

			static bool PlacePickFilter(const Node *node, const PickData *pickData, const void *cookie);

			static void SelectAllInstances(Editor *editor, const char *worldName);
			static int32 GatherInstances(Editor *editor, const char *worldName, List<NodeReference> *worldList);
			static bool ModifiersEqual(const Instance *instance, const List<Modifier> *modifierList);

			void HandleCleanupMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData);
			void HandleSelectAllMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData);
			void HandleSelectSomeMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData);
			void HandleExpandAllInSceneGraphMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData);
			void HandleCollapseAllInSceneGraphMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData);
			void HandleReplaceInstancesMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData);
			void HandleReplaceModifiersMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData);
			void HandleNewModifierPresetMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData);
			void HandleDeleteModifierPresetMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData);
			void HandleRenameModifierPresetMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData);

			static void SelectSomeComplete(SelectSomeWindow *window, void *cookie);
			static void NewModifierPresetComplete(ModifierPresetWindow *window, void *cookie);
			static void RenameModifierPresetComplete(ModifierPresetWindow *window, void *cookie);

			void HandleEditorEvent(Editor *editor, const EditorEvent& event);
			void HandleWorldButtonEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleWorldListEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleModifierListEvent(Widget *widget, const WidgetEventData *eventData);
			void HandlePlaceWidgetEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleMenuButtonEvent(Widget *widget, const WidgetEventData *eventData);

		public:

			WorldsPage();
			~WorldsPage();

			void Prepack(List<Object> *linkList) const override;
			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Preprocess(void) override;

			bool ModifierPresetNameAllowed(const char *name) const;

			void Engage(Editor *editor, void *cookie) override;
			void Disengage(Editor *editor, void *cookie) override;

			bool BeginTool(Editor *editor, EditorTrackData *trackData) override;
			bool TrackTool(Editor *editor, EditorTrackData *trackData) override;
			bool EndTool(Editor *editor, EditorTrackData *trackData) override;
	};


	class ModelsPage : public EditorPage, public EditorTool
	{
		private:

			enum
			{
				kModelMenuSelectAll,
				kModelMenuSelectSome,
				kModelMenuExpandAllInSceneGraph,
				kModelMenuCollapseAllInSceneGraph,
				kModelMenuItemCount
			};

			class SelectSomeWindow : public Window, public Completable<SelectSomeWindow>
			{
				private:

					unsigned_int32			selectPercentage;

					PushButtonWidget		*okayButton;
					PushButtonWidget		*cancelButton;
					EditTextWidget			*percentBox;

				public:

					SelectSomeWindow(unsigned_int32 percentage);
					~SelectSomeWindow();

					unsigned_int32 GetSelectPercentage(void) const
					{
						return (selectPercentage);
					}

					void Preprocess(void) override;
					void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
			};

			int32							currentTool;

			unsigned_int32					selectPercentage;

			IconButtonWidget				*modelButton;
			ListWidget						*modelList;

			IconButtonWidget				*menuButton;
			MenuItemWidget					*modelMenuItem[kModelMenuItemCount];
			List<MenuItemWidget>			modelMenuItemList;

			WidgetObserver<ModelsPage>		modelButtonObserver;
			WidgetObserver<ModelsPage>		modelListObserver;
			WidgetObserver<ModelsPage>		menuButtonObserver;

			void BuildModelList(void);

			static void SelectAllModels(Editor *editor, ModelType type);
			static int32 GatherModels(Editor *editor, ModelType type, List<NodeReference> *modelList);

			void HandleSelectAllMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData);
			void HandleSelectSomeMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData);
			void HandleExpandAllInSceneGraphMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData);
			void HandleCollapseAllInSceneGraphMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData);

			static void SelectSomeComplete(SelectSomeWindow *window, void *cookie);

			void HandleModelButtonEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleModelListEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleMenuButtonEvent(Widget *widget, const WidgetEventData *eventData);

		public:

			ModelsPage();
			~ModelsPage();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Preprocess(void) override;

			void Engage(Editor *editor, void *cookie) override;
			void Disengage(Editor *editor, void *cookie) override;

			bool BeginTool(Editor *editor, EditorTrackData *trackData) override;
			bool TrackTool(Editor *editor, EditorTrackData *trackData) override;
			bool EndTool(Editor *editor, EditorTrackData *trackData) override;
	};


	class ImpostorsPage : public EditorPage, public EditorTool
	{
		private:

			int32							currentTool;

			IconButtonWidget				*impostorButton;
			WidgetObserver<ImpostorsPage>	impostorButtonObserver;

			void HandleImpostorButtonEvent(Widget *widget, const WidgetEventData *eventData);

		public:

			ImpostorsPage();
			~ImpostorsPage();

			void Preprocess(void) override;

			void Engage(Editor *editor, void *cookie) override;
			void Disengage(Editor *editor, void *cookie) override;

			bool BeginTool(Editor *editor, EditorTrackData *trackData) override;
			bool TrackTool(Editor *editor, EditorTrackData *trackData) override;
			bool EndTool(Editor *editor, EditorTrackData *trackData) override;
	};


	class PlacementPage : public EditorPage
	{
		private:

			int32							enableValue;
			PlacementAdjuster				placementAdjuster;

			CheckWidget						*enableWidget;
			CheckWidget						*rotateWidget;
			CheckWidget						*tangentWidget;
			CheckWidget						*sinkWidget;
			EditTextWidget					*radiusWidget;
			EditTextWidget					*minOffsetWidget;
			EditTextWidget					*maxOffsetWidget;
			PushButtonWidget				*applyButton;

			WidgetObserver<PlacementPage>	placementWidgetObserver;
			WidgetObserver<PlacementPage>	applyButtonObserver;

			void HandlePlacementWidgetEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleApplyButtonEvent(Widget *widget, const WidgetEventData *eventData);

			static void AdjustPlacement(const Editor *editor, Transform4D *transform, const Vector3D& normal, void *cookie);

		public:

			PlacementPage();
			~PlacementPage();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Preprocess(void) override;
	};


	class NodeManagementPage : public EditorPage
	{
		private:

			class NodeTreeItemWidget final : public TreeItemWidget
			{
				private:

					NodeManagementPage	*nodeManagementPage;
					SelectionMask		*selectionMask;

					WidgetObserver<NodeTreeItemWidget>	checkObserver;

					void HandleCheckEvent(Widget *widget, const WidgetEventData *eventData);

				public:

					NodeTreeItemWidget(NodeManagementPage *page, SelectionMask *mask, const char *text);
					~NodeTreeItemWidget();

					Type GetNodeType(void) const
					{
						return (selectionMask->GetNodeType());
					}

					int32 GetMaskValue(void) const
					{
						return (selectionMask->GetMaskValue());
					}
			};

			TreeWidget								*treeWidget;
			CheckWidget								*allWidget;
			PushButtonWidget						*showButton;
			PushButtonWidget						*hideButton;
			PushButtonWidget						*selectButton;

			EditorObserver<NodeManagementPage>		editorObserver;
			WidgetObserver<NodeManagementPage>		treeWidgetObserver;
			WidgetObserver<NodeManagementPage>		allWidgetObserver;
			WidgetObserver<NodeManagementPage>		showButtonObserver;
			WidgetObserver<NodeManagementPage>		hideButtonObserver;
			WidgetObserver<NodeManagementPage>		selectButtonObserver;

			void BuildNodeTree(const SelectionMask *selectionMask, TreeItemWidget *treeItem, StringID& stringID);
			void UpdateTreeItems(void);

			void HandleEditorEvent(Editor *editor, const EditorEvent& event);

			void HandleTreeWidgetEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleAllWidgetEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleShowButtonEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleHideButtonEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleSelectButtonEvent(Widget *widget, const WidgetEventData *eventData);

		public:

			NodeManagementPage();
			~NodeManagementPage();

			void Preprocess(void) override;
	};


	class GridPage : public EditorPage
	{
		private:

			enum
			{
				kEditorGridButtonShow,
				kEditorGridButtonSnap,
				kEditorGridButtonAxes,
				kEditorGridButtonHalve,
				kEditorGridButtonDouble,
				kEditorGridButtonCount
			};

			IconButtonWidget			*gridButton[kEditorGridButtonCount];
			PopupMenuWidget				*snapAngleWidget;
			EditTextWidget				*gridSpacingWidget;
			EditTextWidget				*majorLineWidget;
			ColorWidget					*gridColorWidget;

			WidgetObserver<GridPage>	gridButtonObserver;
			WidgetObserver<GridPage>	angleMenuObserver;
			WidgetObserver<GridPage>	gridTextObserver;
			WidgetObserver<GridPage>	gridColorObserver;

			void HandleGridButtonEvent(Widget *widget, const WidgetEventData *eventData);
			void HangleAngleMenuEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleGridTextEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleGridColorEvent(Widget *widget, const WidgetEventData *eventData);

		public:

			GridPage();
			~GridPage();

			void Preprocess(void) override;
	};


	class ViewportsPage : public EditorPage
	{
		private:

			IconButtonWidget				*layoutButton[kEditorLayoutCount];

			SliderWidget					*cameraSpeedSlider;
			TextWidget						*cameraSpeedText;

			WidgetObserver<ViewportsPage>	layoutButtonObserver;
			WidgetObserver<ViewportsPage>	cameraSpeedSliderObserver;

			void HandleLayoutButtonEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleCameraSpeedSliderEvent(Widget *widget, const WidgetEventData *eventData);

		public:

			ViewportsPage();
			~ViewportsPage();

			void Preprocess(void) override;

			void SetViewportLayout(int32 layout);
	};


	class TransformPage : public EditorPage
	{
		private:

			OperationType					operationType;
			bool							updateFlag;

			EditTextWidget					*positionTextWidget[3];
			EditTextWidget					*rotationTextWidget[3];
			EditTextWidget					*sizeTextWidget[kMaxObjectSizeCount];

			PushButtonWidget				*copyButton;
			PushButtonWidget				*pasteButton;
			PushButtonWidget				*resetButton;

			RadioWidget						*positionButton;
			RadioWidget						*rotationButton;
			RadioWidget						*bothButton;

			EditorObserver<TransformPage>	editorObserver;
			WidgetObserver<TransformPage>	positionTextObserver;
			WidgetObserver<TransformPage>	rotationTextObserver;
			WidgetObserver<TransformPage>	sizeTextObserver;
			WidgetObserver<TransformPage>	buttonObserver;

			void UpdateTransform(const Node *node);

			void HandleEditorEvent(Editor *editor, const EditorEvent& event);
			void HandlePositionTextEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleRotationTextEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleSizeTextEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleButtonEvent(Widget *widget, const WidgetEventData *eventData);

		public:

			TransformPage();
			~TransformPage();

			void Preprocess(void) override;
	};


	class InfoPage : public EditorPage
	{
		private:

			enum
			{
				kNodeInfoName,
				kNodeInfoController,
				kNodeInfoConnectors,
				kNodeInfoProperties,
				kNodeInfoInstances,
				kNodeInfoCount
			};

			enum
			{
				kGeometryInfoVertices,
				kGeometryInfoPrimitives,
				kGeometryInfoSurfaces,
				kGeometryInfoMaterials,
				kGeometryInfoLevels,
				kGeometryInfoCount
			};

			ImageWidget					*nodeIconWidget;

			Widget						*geometryGroup;
			Widget						*worldGroup;
			Widget						*typeGroup;

			TextWidget					*nodeWidget[kNodeInfoCount];
			TextWidget					*geometryWidget[kGeometryInfoCount];
			TextWidget					*worldWidget;
			TextWidget					*typeWidget;

			EditorObserver<InfoPage>	editorObserver;

			void HandleEditorEvent(Editor *editor, const EditorEvent& event);

			void ClearInfo(void);
			void UpdateInfo(const Node *node);

		public:

			InfoPage();
			~InfoPage();

			void Preprocess(void) override;
	};


	class FindPage : public EditorPage
	{
		private:

			EditTextWidget				*nameWidget;
			CheckWidget					*typeBoxWidget;
			ListWidget					*typeListWidget;
			CheckWidget					*controllerBoxWidget;
			ListWidget					*controllerListWidget;

			WidgetObserver<FindPage>	findAllButtonObserver;
			WidgetObserver<FindPage>	findNextButtonObserver;
			WidgetObserver<FindPage>	findPreviousButtonObserver;

			bool MatchingNode(const Node *node) const;

			void HandleFindAllButtonEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleFindNextButtonEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleFindPreviousButtonEvent(Widget *widget, const WidgetEventData *eventData);

		public:

			FindPage();
			~FindPage();

			void Preprocess(void) override;
	};
}


#endif

// ZYUQURM
