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


#ifndef C4Landscaping_h
#define C4Landscaping_h


#include "C4Viewports.h"
#include "C4Configuration.h"
#include "C4EditorPages.h"


namespace C4
{
	enum
	{
		kEditorPageLandscaping		= 'LAND'
	};


	enum
	{
		kLandscapingEditorModified	= 1 << 0
	};


	class LandscapingGroup : public ListElement<LandscapingGroup>, public Packable, public Configurable
	{
		private:

			String<>					groupName;
			Array<ResourceName, 1>		worldNameArray;

			int32						instanceDensity;
			float						groupOverlapAllowance;
			float						nongroupOverlapAllowance;

			List<Modifier>				modifierList;
			PlacementAdjuster			placementAdjuster;

		public:

			LandscapingGroup();
			LandscapingGroup(const char *name);
			LandscapingGroup(const LandscapingGroup& landscapingGroup);
			~LandscapingGroup();

			const String<>& GetGroupName(void) const
			{
				return (groupName);
			}

			void SetGroupName(const char *name)
			{
				groupName = name;
			}

			int32 GetWorldNameCount(void) const
			{
				return (worldNameArray.GetElementCount());
			}

			const ResourceName& GetWorldName(int32 index) const
			{
				return (worldNameArray[index]);
			}

			int32 GetInstanceDensity(void) const
			{
				return (instanceDensity);
			}

			float GetGroupOverlapAllowance(void) const
			{
				return (groupOverlapAllowance);
			}

			float GetNongroupOverlapAllowance(void) const
			{
				return (nongroupOverlapAllowance);
			}

			Modifier *GetFirstModifier(void) const
			{
				return (modifierList.First());
			}

			void AddModifier(Modifier *modifier)
			{
				modifierList.Append(modifier);
			}

			PlacementAdjuster *GetPlacementAdjuster(void)
			{
				return (&placementAdjuster);
			}

			const PlacementAdjuster *GetPlacementAdjuster(void) const
			{
				return (&placementAdjuster);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override; 
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override; 
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override; 

			bool AddWorldName(const ResourceName& name);
			bool RemoveWorldName(const ResourceName& name); 
	};
 
 
	class LandscapingBrush : public Packable
	{
		private:
 
			List<LandscapingGroup>		groupList;

		public:

			LandscapingBrush(const LandscapingBrush *brush = nullptr);
			~LandscapingBrush();

			LandscapingGroup *GetFirstGroup(void) const
			{
				return (groupList.First());
			}

			void AddGroup(LandscapingGroup *group)
			{
				groupList.Append(group);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void CopyBrush(const LandscapingBrush *brush);
	};


	class LandscapingPage : public EditorPage, public EditorTool
	{
		private:

			enum
			{
				kLandscapingMenuDeleteBrush,
				kLandscapingMenuRenameBrush,
				kLandscapingMenuExportBrush,
				kLandscapingMenuItemCount
			};

			class BrushWidget final : public TextWidget, public MapElement<BrushWidget>
			{
				public:

					typedef StringKey KeyType;

					LandscapingBrush		landscapingBrush;

					BrushWidget();
					BrushWidget(const Vector2D& size, const char *name = nullptr, const LandscapingBrush *brush = nullptr);
					~BrushWidget();

					KeyType GetKey(void) const
					{
						return (GetText());
					}

					void Pack(Packer& data, unsigned_int32 packFlags) const override;
					void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
					bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			};

			Map<BrushWidget>					brushWidgetMap;

			IconButtonWidget					*additiveButton;
			IconButtonWidget					*subtractiveButton;
			SliderWidget						*densitySlider;
			ListWidget							*brushListWidget;

			IconButtonWidget					*menuButton;
			MenuItemWidget						*landscapingMenuItem[kLandscapingMenuItemCount];
			List<MenuItemWidget>				landscapingMenuItemList;

			WidgetObserver<LandscapingPage>		brushListWidgetObserver;
			WidgetObserver<LandscapingPage>		menuButtonObserver;

			void HandleNewBrushMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData);
			void HandleDeleteBrushMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData);
			void HandleRenameBrushMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData);
			void HandleImportBrushMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData);
			void HandleExportBrushMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData);

			void HandleBrushListWidgetEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleMenuButtonEvent(Widget *widget, const WidgetEventData *eventData);

		public:

			LandscapingPage();
			~LandscapingPage();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Preprocess(void) override;

			bool BrushNameAllowed(const char *name) const;
			LandscapingBrush *AddBrush(const char *name, const LandscapingBrush *brush);

			void Engage(Editor *editor, void *cookie) override;
			void Disengage(Editor *editor, void *cookie) override;

			bool BeginTool(Editor *editor, EditorTrackData *trackData) override;
			bool TrackTool(Editor *editor, EditorTrackData *trackData) override;
			bool EndTool(Editor *editor, EditorTrackData *trackData) override;
	};


	class LandscapingEditor : public Window
	{
		private:

			class BrushWindow : public Window, public Completable<BrushWindow>
			{
				private:

					LandscapingPage			*landscapingPage;
					LandscapingEditor		*landscapingEditor;

					PushButtonWidget		*okayButton;
					PushButtonWidget		*cancelButton;
					EditTextWidget			*nameBox;

				public:

					BrushWindow(LandscapingPage *page, LandscapingEditor *editor);
					~BrushWindow();

					const char *GetLandscapingBrushName(void) const
					{
						return (nameBox->GetText());
					}

					void Preprocess(void) override;
					void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
			};

			class GroupWidget final : public TextWidget
			{
				public:

					LandscapingGroup		*landscapingGroup;

					GroupWidget(const Vector2D& size, LandscapingGroup *group);
					~GroupWidget();
			};

			class GroupWindow : public Window, public Completable<GroupWindow>
			{
				private:

					LandscapingEditor		*landscapingEditor;
					String<>				groupName;

					PushButtonWidget		*okayButton;
					PushButtonWidget		*cancelButton;
					EditTextWidget			*nameBox;

				public:

					GroupWindow(LandscapingEditor *editor, const char *name = "");
					~GroupWindow();

					const char *GetLandscapingGroupName(void) const
					{
						return (nameBox->GetText());
					}

					void Preprocess(void) override;
					void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
			};

			class ModifierWidget final : public TextWidget
			{
				public:

					Modifier		*instanceModifier;

					ModifierWidget(const Vector2D& size, Modifier *modifier);
					~ModifierWidget();
			};

			class ModifierWindow : public Window, public Completable<ModifierWindow>
			{
				private:

					LandscapingEditor		*landscapingEditor;

					PushButtonWidget		*okayButton;
					PushButtonWidget		*cancelButton;
					ListWidget				*modifierListWidget;

				public:

					ModifierWindow(LandscapingEditor *editor);
					~ModifierWindow();

					void Preprocess(void) override;
					void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;

					ModifierType GetModifierType(void) const;
			};

			unsigned_int32								editorState;

			LandscapingPage								*landscapingPage;

			LandscapingBrush							*originalBrush;
			LandscapingBrush							landscapingBrush;

			FrustumViewportWidget						*landscapingViewport;
			BorderWidget								*viewportBorder;

			Widget										*settingsGroup;
			MultipaneWidget								*multipaneWidget;
			ConfigurationWidget							*instanceConfigurationWidget;

			ListWidget									*groupListWidget;
			IconButtonWidget							*addGroupButton;
			IconButtonWidget							*removeGroupButton;

			ListWidget									*worldListWidget;
			IconButtonWidget							*addWorldButton;
			IconButtonWidget							*removeWorldButton;

			ListWidget									*modifierListWidget;
			IconButtonWidget							*addModifierButton;
			IconButtonWidget							*removeModifierButton;
			ConfigurationWidget							*modifierConfigurationWidget;

			MenuBarWidget								*menuBar;
			PulldownMenuWidget							*landscapingMenu;

			World										*landscapingWorld;
			Zone										*zoneNode;
			Light										*lightNode;
			Geometry									*groundNode;
			Node										*previewNode;

			bool										toolTracking;
			Point3D										previousPosition;

			float										lightAzimuth;
			float										lightAltitude;

			ConfigurationObserver<LandscapingEditor>	instanceConfigurationObserver;
			ConfigurationObserver<LandscapingEditor>	modifierConfigurationObserver;

			void PositionWidgets(void);
			void BuildMenus(void);

			void HandleInstanceConfigurationEvent(SettingInterface *settingInterface);
			void HandleModifierConfigurationEvent(SettingInterface *settingInterface);

			void HandleCloseMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleSaveBrushMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleRegeneratePreviewMenuItem(Widget *menuItem, const WidgetEventData *eventData);

			void BuildLandscapingWorld(void);
			void GenerateLandscapingPreview(void);

			void ShowSettings(void);
			void HideSettings(void);
			void SelectPane(int32 index);
			void SelectGroup(const LandscapingGroup *group);
			void SelectModifier(const Modifier *modifier);
			void BuildWorldList(const LandscapingGroup *group);
			void BuildModifierList(const LandscapingGroup *group);

			static void CloseDialogComplete(Dialog *dialog, void *cookie);
			static void SaveBrushComplete(BrushWindow *window, void *cookie);
			static void NewGroupComplete(GroupWindow *window, void *cookie);
			static void RenameGroupComplete(GroupWindow *window, void *cookie);
			static void NewWorldComplete(FilePicker *picker, void *cookie);
			static void NewModifierComplete(ModifierWindow *window, void *cookie);

			static void ViewportHandleMouseEvent(const MouseEventData *eventData, ViewportWidget *viewport, void *cookie);
			static void ViewportTrackTask(const Point3D& position, ViewportWidget *viewport, void *cookie);
			static void ViewportRender(List<Renderable> *renderList, ViewportWidget *viewport, void *cookie);

		public:

			LandscapingEditor(LandscapingPage *page, LandscapingBrush *brush = nullptr);
			~LandscapingEditor();

			void SetModifiedFlag(void)
			{
				editorState |= kLandscapingEditorModified;
			}

			void SetWidgetSize(const Vector2D& size) override;
			void Preprocess(void) override;

			bool GroupNameAllowed(const char *name) const;

			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
			void Close(void) override;
	};
}


#endif

// ZYUQURM
