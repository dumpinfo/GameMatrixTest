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


#ifndef C4NodeInfo_h
#define C4NodeInfo_h


#include "C4Zones.h"
#include "C4Skybox.h"
#include "C4Panels.h"
#include "C4Forces.h"
#include "C4Particles.h"
#include "C4Viewports.h"
#include "C4Configuration.h"
#include "C4EditorBase.h"


namespace C4
{
	typedef Type	NodeInfoType;


	enum
	{
		kNodeInfoNodeCategory,
		kNodeInfoObjectCategory,
		kNodeInfoCategoryTypeCount,
		kNodeInfoProperties = kNodeInfoCategoryTypeCount,
		kNodeInfoModifiers,
		kNodeInfoConnectors,
		kNodeInfoController,
		kNodeInfoForce
	};


	class Editor;


	class NodeInfoPane : public Board, public ListElement<NodeInfoPane>
	{
		private:

			NodeInfoType	nodeInfoType;
			const char		*paneTitle;
			bool			visitedFlag;

			Widget			*initialFocusWidget;

		protected:

			NodeInfoPane(NodeInfoType type, const char *title, const char *panelName);

			void SetInitialFocusWidget(Widget *widget)
			{
				initialFocusWidget = widget;
			}

			void AddSubwindow(Window *window)
			{
				GetOwningWindow()->AddSubwindow(window);
			}

		public:

			virtual ~NodeInfoPane();

			using ListElement<NodeInfoPane>::Previous;
			using ListElement<NodeInfoPane>::Next;

			NodeInfoType GetNodeInfoType(void) const
			{
				return (nodeInfoType);
			}

			const char *GetPaneTitle(void) const
			{
				return (paneTitle);
			}

			Widget *GetInitialFocusWidget(void) const
			{
				return (initialFocusWidget);
			}

			bool GetVisitedFlag(void) const
			{
				return (visitedFlag);
			}

			void SetVisitedFlag(void)
			{
				visitedFlag = true;
			}

			virtual void BuildSettings(const Configurable *configurable) = 0;
			virtual void CommitSettings(Configurable *configurable) = 0;

			virtual void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData);
	};


	class CategoryInfoPane : public NodeInfoPane
	{
		private: 

			NodeInfoType			nodeInfoType;
			ConfigurationWidget		*configurationWidget; 

		public: 

			CategoryInfoPane(NodeInfoType type, Type category, const char *title, const char *panelName = "WorldEditor/CategoryInfo");
			~CategoryInfoPane(); 

			NodeInfoType GetNodeInfoType(void) const 
			{ 
				return (nodeInfoType);
			}

			ConfigurationWidget *GetConfigurationWidget(void) 
			{
				return (configurationWidget);
			}

			void Preprocess(void) override;

			void BuildSettings(const Configurable *configurable) override;
			void CommitSettings(Configurable *configurable) override;
	};


	class FireEffectInfoPane : public CategoryInfoPane
	{
		private:

			FrustumViewportWidget	*previewViewport;
			World					*previewWorld;
			FireEffect				*previewFire;

			ConfigurationObserver<FireEffectInfoPane>	configurationObserver;

			void HandleConfigurationEvent(SettingInterface *settingInterface);

			static void RenderPreview(List<Renderable> *renderList, ViewportWidget *viewport, void *cookie);

		public:

			FireEffectInfoPane(Type category, const char *title);
			~FireEffectInfoPane();

			void Preprocess(void) override;

			void BuildSettings(const Configurable *configurable) override;
	};


	class PropertyData : public ListElement<PropertyData>
	{
		private:

			const PropertyRegistration	*propertyRegistration;

			const Property				*originalProperty;
			Property					*newProperty;

		public:

			PropertyData(const PropertyRegistration *registration, const Property *property);
			~PropertyData();

			const PropertyRegistration *GetPropertyRegistration(void) const
			{
				return (propertyRegistration);
			}

			const Property *GetOriginalProperty(void) const
			{
				return (originalProperty);
			}

			Property *GetNewProperty(void) const
			{
				return (newProperty);
			}

			void SetNewProperty(Property *property)
			{
				newProperty = property;
			}
	};


	class PropertyInfoPane : public NodeInfoPane
	{
		private:

			List<PropertyData>		propertyDataList;
			PropertyData			*selectedProperty;

			ListWidget				*availableListWidget;
			ListWidget				*assignedListWidget;

			PushButtonWidget		*assignButton;
			PushButtonWidget		*removeButton;

			ConfigurationWidget						*configurationWidget;
			Array<const PropertyRegistration *>		validPropertyArray;

			void SelectAvailableProperty(int32 index);
			void SelectAssignedProperty(int32 index);

		public:

			PropertyInfoPane();
			~PropertyInfoPane();

			void Preprocess(void) override;

			void BuildSettings(const Configurable *configurable) override;
			void CommitSettings(Configurable *configurable) override;

			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};


	class ModifierInfoPane : public NodeInfoPane
	{
		private:

			List<Modifier>			modifierList;
			Modifier				*selectedModifier;

			ListWidget				*availableListWidget;
			ListWidget				*assignedListWidget;

			PushButtonWidget		*assignButton;
			PushButtonWidget		*removeButton;

			ConfigurationWidget						*configurationWidget;
			Array<const ModifierRegistration *>		validModifierArray;

			void SelectAssignedModifier(int32 index);

		public:

			ModifierInfoPane();
			~ModifierInfoPane();

			void Preprocess(void) override;

			void BuildSettings(const Configurable *configurable) override;
			void CommitSettings(Configurable *configurable) override;

			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};


	class ConnectorInfoPane : public NodeInfoPane
	{
		private:

			struct ConnectorData : ListElement<ConnectorData>
			{
				Widget				*listItem;
				Connector			*connector;

				CheckWidget			*checkWidget;
				EditTextWidget		*textWidget;

				ConnectorData(Widget *item, Connector *conn = nullptr);
				~ConnectorData();
			};

			List<ConnectorData>		connectorList;

			ListWidget				*availableListWidget;
			ListWidget				*assignedListWidget;
			ListWidget				*customListWidget;

			PushButtonWidget		*assignButton;
			PushButtonWidget		*removeButton;
			PushButtonWidget		*newButton;
			PushButtonWidget		*deleteButton;

			bool InternalConnectorAssigned(const char *key) const;

		public:

			ConnectorInfoPane();
			~ConnectorInfoPane();

			void Preprocess(void) override;

			void BuildSettings(const Configurable *configurable) override;
			void CommitSettings(Configurable *configurable) override;

			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};


	class ControllerInfoPane : public NodeInfoPane
	{
		private:

			const Node				*targetNode;

			int32					controllerCount;
			int32					selectionIndex;
			Controller				**controllerTable;

			ListWidget				*controllerListWidget;
			PushButtonWidget		*scriptButton;
			PushButtonWidget		*panelButton;

			ConfigurationWidget						*configurationWidget;
			Array<const ControllerRegistration *>	validControllerArray;

			void SelectController(int32 index);

		public:

			ControllerInfoPane();
			~ControllerInfoPane();

			void Preprocess(void) override;

			void BuildSettings(const Configurable *configurable) override;
			void CommitSettings(Configurable *configurable) override;

			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};


	class ForceInfoPane : public NodeInfoPane
	{
		private:

			const Field				*targetField;

			int32					forceCount;
			int32					selectionIndex;
			Force					**forceTable;

			ListWidget				*forceListWidget;

			ConfigurationWidget					*configurationWidget;
			Array<const ForceRegistration *>	validForceArray;

			void SelectForce(int32 index);

		public:

			ForceInfoPane();
			~ForceInfoPane();

			void Preprocess(void) override;

			void BuildSettings(const Configurable *configurable) override;
			void CommitSettings(Configurable *configurable) override;

			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};


	class NodeInfoWindow : public Window
	{
		private:

			Editor					*worldEditor;
			List<NodeInfoPane>		paneList;

			PushButtonWidget		*okayButton;
			PushButtonWidget		*cancelButton;

			MultipaneWidget			*multipaneWidget;

			void CommitSettings(void);
			void RememberSelectedPane(void) const;

		public:

			C4EDITORAPI NodeInfoWindow(Editor *editor);
			C4EDITORAPI ~NodeInfoWindow();

			void Preprocess(void) override;
			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};
}


#endif

// ZYUQURM
