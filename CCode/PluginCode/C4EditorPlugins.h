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


#ifndef C4EditorPlugins_h
#define C4EditorPlugins_h


//# \component	World Editor
//# \prefix		WorldEditor/


#include "C4Plugins.h"
#include "C4FilePicker.h"
#include "C4EditorBase.h"


namespace C4
{
	typedef Type	SceneImportType;


	//# \enum	SceneImportType

	enum : SceneImportType
	{
		kSceneImportGeometry					= 'GEOM',		//## Import geometry data.
		kSceneImportAnimation					= 'ANIM'		//## Import animation data.
	};


	//# \enum	GeometryImportFlags

	enum
	{
		kGeometryImportTextures					= 1 << 0,		//## Import texture maps that contain the string "/Import/" in their names.
		kGeometryImportMergeMaterials			= 1 << 1,		//## Reuse existing materials that match imported materials exactly.
		kGeometryImportReuseNamedMaterials		= 1 << 2		//## Reuse existing materials having the same names as imported materials even if the materials are different.
	};


	//# \enum	AnimationImportFlags

	enum
	{
		kAnimationImportAnchorXY				= 1 << 0,		//## Anchor the animation to the origin in the <i>x</i> and <i>y</i> directions.
		kAnimationImportAnchorZ					= 1 << 1,		//## Anchor the animation to the origin in the <i>z</i> direction.
		kAnimationImportFreezeRoot				= 1 << 2,		//## Freeze the root node position and keep all subnode positions relative to it. This flag overrides the $kAnimationImportAnchorXY$ and $kAnimationImportAnchorZ$ flags.
		kAnimationImportPreserveMissing			= 1 << 3		//## Preserve the current transform of any animatable node that is missing in the import file and include it in the generated output.
	};


	class Model;
	class EditorObject;
	class ModelWindow;


	struct GeometryImportData
	{
		unsigned_int32		importFlags;
		float				importScale;
	};


	struct AnimationImportData
	{
		unsigned_int32		importFlags;
		float				importRotation;
	};


	//# \class	EditorPlugin		Represents a World Editor plugin.
	//
	//# The $EditorPlugin$ class represents a World Editor plugin.
	//
	//# \def	class EditorPlugin : public Plugin, public ListElement<EditorPlugin>
	//
	//# \ctor	EditorPlugin();
	//
	//# The constructor has protected access. Only instances of $EditorPlguin$ subclasses can be created.
	//
	//# \desc
	//# The $EditorPlugin$ class is the base class for all World Editor plugins.
	//
	//# \base	System/Plugin							An $EditorPlugin$ is a specific type of plugin.
	//# \base	Utilities/ListElement<EditorPlugin>		Used internally by the World Editor.


	//# \function	EditorPlugin::GetPluginName		Returns the name of a plugin.
	//
	//# \proto	virtual const char *GetPluginName(void) const;
	//
	//# \desc
	//# The $GetPluginName$ function returns a pointer to the name of a World Editor plugin. All plugin
	//# subclasses must implement this method, and the return value cannot be $nullptr$.


	//# \function	EditorPlugin::Initialize		Initializes a plugin for a new editor window.
	//
	//# \proto	virtual void Initialize(EditorObject *editorObject);
	//
	//# \param	editorObject		A pointer to the editor object for a newly created World Editor window.
	//
	//# \desc
	//# The $Initialize$ function is called for each World Editor plugin when a new editor window is created, 
	//# but before the world data is loaded. The plugin can do anything that it needs to do in order to be used
	//# with the window as int32 as it doesn't require access to the world itself or any editor facilities.
	//# Any initialization that does require more access should be implemented in the $@EditorPlugin::Preprocess@$ 
	//# function.
	//# 
	//# A typical use for the $Initialize$ function is to create one or more instances of custom $@EditorPage@$
	//# subclasses and add them to the editor window using the $@EditorObject::AddEditorPage@$ function. Custom
	//# pages should be created at this time so that any data they save is properly reloaded when the world data 
	//# is read from the disk.
	//# 
	//# The default implementation of the $Initialize$ function does nothing. 
	//
	//# \also	$@EditorPlugin::Preprocess@$
	//# \also	$@EditorObject@$
 

	//# \function	EditorPlugin::Preprocess		Preprocesses a plugin for a new editor window.
	//
	//# \proto	virtual void Preprocess(Editor *editor);
	//
	//# \param	editor		A pointer to a fully initialized World Editor window.
	//
	//# \desc
	//# The $Preprocess$ function is called for each World Editor plugin after an editor window has been opened,
	//# the world data has been loaded, and all components of the editor window have been fully initialized.
	//# The plugin can do anything that it needs to do in order to be used with the window, and it may access
	//# any part of the scene structure.
	//#
	//# Note that custom editor pages should not be created in the $Preprocess$ function. Pages should instead
	//# be created and added to the editor window in the $@EditorPlugin::Initialize@$ function.
	//#
	//# The default implementation of the $Preprocess$ function does nothing.
	//
	//# \also	$@EditorPlugin::Initialize@$


	class EditorPlugin : public Plugin, public ListElement<EditorPlugin>
	{
		protected:

			C4EDITORAPI EditorPlugin();

		public:

			C4EDITORAPI ~EditorPlugin();

			virtual const char *GetPluginName(void) const = 0;

			C4EDITORAPI virtual void Initialize(EditorObject *editorObject);
			C4EDITORAPI virtual void Preprocess(Editor *editor);
	};


	//# \class	SceneImportPlugin		Represents a World Editor plugin that imports scenes.
	//
	//# The $SceneImportPlugin$ class represents a World Editor plugin that imports scenes.
	//
	//# \def	class SceneImportPlugin : public EditorPlugin, public ListElement<SceneImportPlugin>
	//
	//# \ctor	SceneImportPlugin();
	//
	//# The constructor has protected access. Only instances of $SceneImportPlugin$ subclasses can be created.
	//
	//# \desc
	//# The $SceneImportPlugin$ class is the base class for all World Editor plugins that import scenes.
	//
	//# \base	EditorPlugin								A $SceneImportPlugin$ is a specific type of editor plugin.
	//# \base	Utilities/ListElement<SceneImportPlugin>	Used internally by the World Editor.


	class SceneImportPlugin : public EditorPlugin, public ListElement<SceneImportPlugin>
	{
		protected:

			C4EDITORAPI SceneImportPlugin();

		public:

			C4EDITORAPI ~SceneImportPlugin();

			using ListElement<SceneImportPlugin>::Previous;
			using ListElement<SceneImportPlugin>::Next;

			virtual const ResourceDescriptor *GetImportResourceDescriptor(SceneImportType type) const = 0;

			virtual void ImportGeometry(Editor *editor, const char *importName, const GeometryImportData *importData) = 0;
			virtual bool ImportAnimation(Window *window, const char *importName, const char *resourceName, Model *model, const AnimationImportData *importData) = 0;
	};


	//# \class	SceneExportPlugin		Represents a World Editor plugin that exports scenes.
	//
	//# The $SceneExportPlugin$ class represents a World Editor plugin that exports scenes.
	//
	//# \def	class SceneExportPlugin : public EditorPlugin, public ListElement<SceneExportPlugin>
	//
	//# \ctor	SceneExportPlugin();
	//
	//# The constructor has protected access. Only instances of $SceneExportPlugin$ subclasses can be created.
	//
	//# \desc
	//# The $SceneExportPlugin$ class is the base class for all World Editor plugins that export scenes.
	//
	//# \base	EditorPlugin								A $SceneExportPlugin$ is a specific type of editor plugin.
	//# \base	Utilities/ListElement<SceneExportPlugin>	Used internally by the World Editor.


	class SceneExportPlugin : public EditorPlugin, public ListElement<SceneExportPlugin>
	{
		protected:

			C4EDITORAPI SceneExportPlugin();

		public:

			C4EDITORAPI ~SceneExportPlugin();

			using ListElement<SceneExportPlugin>::Previous;
			using ListElement<SceneExportPlugin>::Next;

			virtual const char *GetPluginName(void) const = 0;
			virtual const ResourceDescriptor *GetExportResourceDescriptor(void) const = 0;

			virtual void ExportScene(const char *name, const World *world) = 0;
	};


	class SceneImportPicker : public FilePicker
	{
		private:

			SceneImportType		importType;
			ResourceName		importSubdir;

			PopupMenuWidget		*importPluginMenu;

			CheckWidget			*importTexturesBox;
			CheckWidget			*mergeMaterialsBox;
			CheckWidget			*reuseNamedMaterialsBox;
			EditTextWidget		*sceneScaleBox;

		public:

			C4EDITORAPI SceneImportPicker(const char *title, SceneImportType type, unsigned_int32 flags = 0, const char *subdir = nullptr, const char *panel = nullptr);
			C4EDITORAPI ~SceneImportPicker();

			bool GetImportTexturesFlag(void) const
			{
				return (importTexturesBox->GetValue() != 0);
			}

			bool GetMergeMaterialsFlag(void) const
			{
				return (mergeMaterialsBox->GetValue() != 0);
			}

			bool GetReuseNamedMaterialsFlag(void) const
			{
				return (reuseNamedMaterialsBox->GetValue() != 0);
			}

			float GetSceneScale(void) const
			{
				return (Text::StringToFloat(sceneScaleBox->GetText()));
			}

			C4EDITORAPI SceneImportPlugin *GetSceneImportPlugin(void) const;

			C4EDITORAPI void Preprocess(void) override;
			C4EDITORAPI void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};


	class SceneExportPicker : public FilePicker
	{
		private:

			ResourcePath		exportSubdir;

			PopupMenuWidget		*exportPluginMenu;

		public:

			C4EDITORAPI SceneExportPicker(const char *title, const char *subdir = nullptr, const char *panel = nullptr);
			C4EDITORAPI ~SceneExportPicker();

			C4EDITORAPI SceneExportPlugin *GetSceneExportPlugin(void) const;

			C4EDITORAPI void Preprocess(void) override;
			C4EDITORAPI void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};
}


#endif

// ZYUQURM
