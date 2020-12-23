 

#ifndef C4FilePicker_h
#define C4FilePicker_h


//# \component	Interface Manager
//# \prefix		InterfaceMgr/


#include "C4Dialog.h"


namespace C4
{
	//# \enum	FilePickerFlags

	enum
	{
		kFilePickerSave						= 1 << 0,		//## Display an editable text field where the file name can be typed in.
		kFilePickerMultipleSelection		= 1 << 1,		//## Allow multiple files to be selected in the file list simultaneously. This should not be set if the $kFilePickerSave$ flag is set.
		kFilePickerEnableRootDirectory		= 1 << 2,		//## Causes the root directory name to be included in the file name. If the $kFilePickerSave$ flag is set, then this flag also allows a file to be specified in the root directory.
		kFilePickerStayInsideRootDirectory	= 1 << 3		//## Don't allow navigation outside (above) the root directory.
	};


	class Sound;
	class WorldViewportWidget;


	class FilePickerVisit : public MapElement<FilePickerVisit>
	{
		private:

			Type			pickerType;
			ResourcePath	visitPath;

		public:

			typedef Type	KeyType;

			FilePickerVisit(Type type, const char *path);
			~FilePickerVisit();

			KeyType GetKey(void) const
			{
				return (pickerType);
			}

			Type GetVisitType(void) const
			{
				return (pickerType);
			}

			const char *GetVisitPath(void) const
			{
				return (visitPath);
			}

			void SetVisitPath(const char *path)
			{
				visitPath = path;
			}
	};


	//# \class	FilePicker		Encapsulates a file picker box.
	//
	//# A $FilePicker$ class encapsulates a file picker box.
	//
	//# \def	class FilePicker : public Window, public LinkTarget<FilePicker>, public Completable<FilePicker>
	//
	//# \ctor	FilePicker(const char *title, const char *directory,
	//# \ctor2	unsigned_int32 flags = 0, float extraWidth = 0.0F, float extraHeight = 0.0F);
	//
	//# \param	title			The file picker's displayed title.
	//# \param	directory		The path to the directory that is initially displayed by the file picker.
	//# \param	flags			Flags that affect the appearance and behavior of the file picker. See below for a list of possible values.
	//# \param	extraWidth		Additional width that is added to the window for use by subclasses.
	//# \param	extraHeight		Additional height that is added to the window for use by subclasses.
	//
	//# \desc
	//# The $FilePicker$ class provides a standard interface for selecting files. When the user selects a file,
	//# the file picker's completion procedure is called. The completion procedure can then retrieve the path to the
	//# selected file by calling the $@FilePicker::GetFileName@$ function.
	//#
	//# Options that control the appearance and behavior of the file picker can be controlled by specifying a
	//# combination (through logical OR) of the following constants in the $flags$ parameter.
	//
	//# \table	FilePickerFlags
	//
	//# The $directory$ parameter specifies the directory for which a list of files is initially displayed.
	//# If the $kFilePickerNoDirectories$ flag is not specified, then the user can navigate to any directory
	//# beneath the initial directory. If the $kFilePickerSubdirectory$ flag is specified, the user can also
	//# navigate upward one level from the initial directory (and subsequently to any directory beneath that level).
	//#
	//# If the $kFilePickerSave$ flag is specified, then an editable text box is displayed in which the user
	//# can enter a file name. This is useful for allowing new files to be created.
	//#
	//# If the file picker is cancelled, then the completion procedure is not called.
	//
	//# \base	Window								The $FilePicker$ class is a specific type of window.
	//# \base	Utilities/LinkTarget<FilePicker>	File pickers support smart linking with the $@Utilities/Link@$ class. 
	//# \base	Utilities/Completable<FilePicker>	The completion procedure is called when the file picker is dismissed.

 
	//# \function	FilePicker::GetFileNameCount	Returns the number of files that are currently selected.
	// 
	//# \proto	int32 GetFileNameCount(void) const;
	//
	//# \desc 
	//# The $GetFileNameCount$ function returns the number of files that are currently selected in the file list.
	//# The names of the individual files can be retrieved by calling the $@FilePicker::GetFileName@$ repeatedly with 
	//# indexes ranging from 0 to <i>n</i>&nbsp;&minus;&nbsp;1, where <i>n</i> is the number of files returned by 
	//# the $GetFileNameCount$ function.
	//
	//# \also	$@FilePicker::GetFileName@$
 

	//# \function	FilePicker::GetFileName		Returns the name of the file that is currently selected.
	//
	//# \proto	String<kMaxFileNameLength> GetFileName(int32 index = 0) const;
	//
	//# \param	index	The index of the selected file. If the $kFilePickerMultipleSelection$ flag was not specified, then this should be 0.
	//
	//# \desc
	//# The $GetFileName$ function returns the relative path of a file that is currently selected in a file picker.
	//# The base directory for the relative path is the directory that was initially specified when the file picker
	//# was created. In the case that the $kFilePickerSubdirectory$ was specified, the base directory is the
	//# enclosing directory of the initially specified directory.
	//#
	//# The $index$ parameter specifies the index of the selected file for which the name is returned. If the
	//# $kFilePickerMultipleSelection$ flag was not specified, then $index$ should be omitted. Otherwise, $index$ may
	//# be in the range 0 to <i>n</i>&nbsp;&minus;&nbsp;1, where <i>n</i> is the number of files returned by the
	//# $@FilePicker::GetFileNameCount@$ function.
	//#
	//# If no file is selected, the value of the $index$ parameter is out of range, or the requested file name corresponds
	//# to a directory, then the return value is the empty string.
	//
	//# \also	$@FilerPicker::GetFileNameCount@$


	class FilePicker : public Window, public LinkTarget<FilePicker>, public Completable<FilePicker>
	{
		private:

			unsigned_int32					filePickerFlags;
			Type							filePickerType;

			const ResourceCatalog			*resourceCatalog;
			const ResourceDescriptor		*resourceDescriptor;

			PushButtonWidget				*openButton;
			PushButtonWidget				*saveButton;
			PushButtonWidget				*cancelButton;
			IconButtonWidget				*upButton;
			ListWidget						*resourceList;
			PopupMenuWidget					*directoryMenu;
			EditTextWidget					*nameBox;

			ResourcePath					currentDirectory;
			int32							rootPathLength;
			int32							directoryDepth;
			int32							directoryCount;

			bool							updateFileListFlag;
			bool							updateButtonStateFlag;

			unsigned_int32					matchTime;
			int32							matchLength;
			char							matchText[kMaxFileNameLength];

			static Map<FilePickerVisit>		visitMap;

			void UpdateButtonState(void);
			void BuildDirectoryMenu(void);
			void RememberVisit(void);

			bool OpenFolder(void);
			void MoveUpward(void);

			static const char *GetListItemText(const Widget *widget);

			bool ConfirmReplace(void);
			static void ConfirmComplete(Dialog *dialog, void *cookie);

		protected:

			const ListWidget *GetResourceListWidget(void) const
			{
				return (resourceList);
			}

			const ResourcePath& GetCurrentDirectory(void) const
			{
				return (currentDirectory);
			}

			C4API void SetResourceDescriptor(const ResourceDescriptor *descriptor);
			C4API void SetCurrentDirectory(const char *directory);

			C4API virtual void BuildFileList(void);

		public:

			C4API FilePicker(const char *title, const char *directory, unsigned_int32 flags = 0, const char *panel = nullptr);
			C4API FilePicker(Type pickerType, const char *title, const ResourceCatalog *catalog, const ResourceDescriptor *descriptor, const char *directory = nullptr, unsigned_int32 flags = 0, const char *panel = nullptr);
			C4API ~FilePicker();

			int32 GetFileNameCount(void) const
			{
				return (resourceList->GetSelectedListItemCount());
			}

			static FilePickerVisit *GetFirstVisit(void)
			{
				return (visitMap.First());
			}

			static void PurgeVisits(void)
			{
				visitMap.Purge();
			}

			static void SetVisit(Type type, const char *path);

			C4API String<kMaxFileNameLength> GetFileName(int32 index = 0) const;
			C4API ResourceName GetResourceName(int32 index = 0) const;
			C4API void SetFileName(const char *name);

			C4API static bool FileNameFilter(unsigned_int32 code);

			C4API void Preprocess(void) override;
			C4API void Move(void) override;

			C4API void SetFocusWidget(Widget *widget) override;

			C4API bool HandleKeyboardEvent(const KeyboardEventData *eventData) override;
			C4API void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};


	class PreviewPicker : public FilePicker
	{
		protected:

			PreviewPicker(const char *title, const char *directory, unsigned_int32 flags, const char *panel);
			PreviewPicker(Type pickerType, const char *title, const ResourceCatalog *catalog, const ResourceDescriptor *descriptor, const char *directory, unsigned_int32 flags, const char *panel);

			virtual void UpdatePreview(void) = 0;

			void BuildFileList(void);

		public:

			~PreviewPicker();

			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};


	class TexturePicker : public PreviewPicker
	{
		private:

			BorderWidget	*previewBorder;
			ImageWidget		*previewImage;
			TextWidget		*previewText;

			void UpdatePreview(void);

		public:

			C4API TexturePicker(const char *title, const char *directory, unsigned_int32 flags = 0);
			C4API TexturePicker(Type pickerType, const char *title, const ResourceCatalog *catalog, const ResourceDescriptor *descriptor, const char *directory = nullptr, unsigned_int32 flags = 0);
			C4API ~TexturePicker();

			C4API void Preprocess(void) override;
	};


	class SoundPicker : public PreviewPicker
	{
		private:

			IconButtonWidget	*playButton;
			IconButtonWidget	*stopButton;
			TextWidget			*previewText;

			Sound				*previewSound;

			void UpdatePreview(void);

			static void SoundComplete(Sound *sound, void *cookie);

		public:

			C4API SoundPicker(const char *title, const char *directory, unsigned_int32 flags = 0);
			C4API SoundPicker(Type pickerType, const char *title, const ResourceCatalog *catalog, const ResourceDescriptor *descriptor, const char *directory = nullptr, unsigned_int32 flags = 0);
			C4API ~SoundPicker();

			C4API void Preprocess(void) override;
			C4API void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};


	class WorldPicker : public PreviewPicker
	{
		private:

			WorldViewportWidget		*previewViewport;
			TextWidget				*previewText;

			void UpdatePreview(void);

		public:

			C4API WorldPicker(const char *title, const char *directory, unsigned_int32 flags = 0);
			C4API WorldPicker(Type pickerType, const char *title, const ResourceCatalog *catalog, const ResourceDescriptor *descriptor, const char *directory = nullptr, unsigned_int32 flags = 0);
			C4API ~WorldPicker();

			C4API void Preprocess(void) override;
	};
}


#endif

// ZYUQURM
