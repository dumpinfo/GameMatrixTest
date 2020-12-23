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


#ifndef C4MaterialManager_h
#define C4MaterialManager_h


#include "C4MaterialContainer.h"
#include "C4ShaderEditor.h"


namespace C4
{
	class WorldEditor;


	enum
	{
		kWidgetMaterial		= 'MATL'
	};


	class MaterialWidget final : public Widget
	{
		private:

			MaterialContainer		*materialContainer;

			BorderWidget			borderWidget;
			PreviewWidget			previewWidget;
			TextWidget				textWidget;

			bool CalculateBoundingBox(Box2D *box) const override;

		public:

			MaterialWidget(const Vector2D& size, MaterialContainer *container);
			~MaterialWidget();

			MaterialContainer *GetMaterialContainer(void) const
			{
				return (materialContainer);
			}

			void Preprocess(void) override;
			void HandleMouseEvent(const PanelMouseEventData *eventData) override;

			void SetMaterial(MaterialContainer *container);
			void SetPreviewGeometry(PrimitiveType type);
			void SetMaterialName(const char *name);
			void UpdatePreview(void);
	};


	class MaterialExporter : public FilePicker, public ListElement<MaterialExporter>
	{
		private:

			MaterialContainer	*materialContainer;

		public:

			MaterialExporter(MaterialContainer *container, const char *title);
			~MaterialExporter();

			MaterialContainer *GetMaterialContainer(void) const
			{
				return (materialContainer);
			}

			void Preprocess(void) override;
	};


	class MaterialWindow : public Window, public LinkTarget<MaterialWindow>, public Completable<MaterialWindow>
	{
		private:

			enum
			{
				kMaterialPaneDiffuse,
				kMaterialPaneSpecular,
				kMaterialPaneAmbient,
				kMaterialPaneFlags,
				kMaterialPaneTexcoords,
				kMaterialPaneCount
			};

			Editor									*worldEditor;

			PushButtonWidget						*closeButton;
			PushButtonWidget						*newButton;
			PushButtonWidget						*deleteButton;
			PushButtonWidget						*duplicateButton;
			PushButtonWidget						*importButton;
			PushButtonWidget						*exportButton;
			PushButtonWidget						*cleanupButton;
			PushButtonWidget						*shaderButton;

			TableWidget								*tableWidget;
			MultipaneWidget							*multipaneWidget;
			ConfigurationWidget						*configurationWidget[kMaterialPaneCount];

			PreviewWidget							*previewWidget; 
			PopupMenuWidget							*previewMenuWidget;
			EditTextWidget							*materialNameWidget;
			TextWidget								*referenceCountWidget; 

			List<MaterialContainer>					*materialList; 
			MaterialWidget							*selectedMaterial;

			WidgetObserver<MaterialWindow>			materialTableObserver; 
			ConfigurationObserver<MaterialWindow>	configurationObserver;
 
			Link<FilePicker>						importPicker; 
			List<MaterialExporter>					exporterList;

			void HandleConfigurationEvent(SettingInterface *settingInterface);
 
			void CommitMaterialSettings(MaterialContainer *container);
			void SelectMaterial(MaterialWidget *materialWidget);

			void UpdatePaneVisibility(void);
			void UpdateMaterialConfiguration(const MaterialContainer *container);
			void UpdateMaterialContainerInfo(const MaterialContainer *container);
			void UpdateMaterialList(MaterialContainer *currentMaterial = nullptr);

			void HandleMaterialTableEvent(Widget *widget, const WidgetEventData *eventData);

			static void ImportMaterial(FilePicker *picker, void *cookie);
			static void ExportMaterial(FilePicker *picker, void *cookie);

			static void ShaderEditorComplete(ShaderEditor *shaderEditor, void *cookie);

		public:

			MaterialWindow(Editor *editor);
			~MaterialWindow();

			void Preprocess(void) override;

			void HandleMaterialModification(void);

			bool HandleKeyboardEvent(const KeyboardEventData *eventData) override;
			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;

			void Close(void) override;
	};
}


#endif

// ZYUQURM
