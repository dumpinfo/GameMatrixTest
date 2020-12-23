 

#ifndef C4Plugins_h
#define C4Plugins_h


//# \component	System Utilities
//# \prefix		System/


#include "C4Engine.h"
#include "C4Menus.h"


namespace C4
{
	typedef EngineResult	PluginResult;
	typedef Type			ImageFormat;


	enum : PluginResult
	{
		kPluginOkay					= kEngineOkay,
		kPluginBadImageFormat		= (kManagerPlugin << 16) | 0x0001,
		kPluginBadImageSize			= (kManagerPlugin << 16) | 0x0002,
		kPluginBadImageIndex		= (kManagerPlugin << 16) | 0x0003
	};


	//# \enum	ImageImportFlags

	enum
	{
		kImageImportAlpha			= 1 << 0		//## The imported image includes an alpha channel.
	};


	//# \enum	ImageExportFlags

	enum
	{
		kImageExportAlpha			= 1 << 0,		//## Include an alpha channel in the exported image.
		kImageExportUncompressed	= 1 << 1,		//## Do not compress the exported image, if possible.
		kImageExportBGRA			= 1 << 2		//## The image data to be exported is stored in BGRA order.
	};


	//# \enum	ImageFormat

	enum : ImageFormat
	{
		kImageFormatRGBA			= 'RGBA',		//## An RGBA image stored as interleaved red, green, blue, and alpha channels in that order. Each channel occupies a single unsigned byte.
		kImageFormatYCbCr			= 'YCBR'		//## A YCbCr 4:2:0 image stored as separate luminance, blue chrominance, and red chrominance channels. The luminance is stored at full resolution with one unsigned byte per pixel. Each chrominance channel is stored separately at one-half the resolution in each dimension. Chrominance channels are stored with one signed byte per low-resolution pixel (which is equivalent to one signed byte per each 2&times;2 group of full-resolution pixels. The luminance channel is stored first, and it is followed by the blue chrominance channel and then the red chrominance channel.
	};


	//# \class	Plugin		The base class for all engine plugins.
	//
	//# The $Plugin$ class represents an engine plugin.
	//
	//# \def	class Plugin
	//
	//# \ctor	Plugin();
	//
	//# The constructor has protected access. Only instances of $Plugin$ subclasses can be created.
	//
	//# \desc
	//# The $Plugin$ class is the base class for all engine plugins. The plugin object defined by
	//# every engine plugin module must inherit, driectly or indirectly, from the $Plugin$ class.
	//#
	//# Every plugin module must contain a $CreatePlugin$ function declared as follows:
	//
	//# \source
	//# extern "C"\n
	//# {\n
	//# \tC4MODULEEXPORT C4::Plugin *CreatePlugin(void);\n
	//# }
	//
	//# \desc
	//# The implementation of this function must return a newly created object that has the
	//# $Plugin$ class as a base class.


	class Plugin
	{
		private:

			#if C4DEBUG

				C4API virtual void DebugPlugin(void);

			#elif C4OPTIMIZED

				C4API virtual void OptimizedPlugin(void);

			#endif

		protected:

			C4API Plugin();

		public:
 
			C4API virtual ~Plugin();
	};
 

	//# \class	ImagePlugin		The base class for all image import plugins and image export plugins. 
	//
	//# The $ImagePlugin$ class represents a generic image import plugin or image export plugin.
	// 
	//# \def	class ImagePlugin : public Plugin
	// 
	//# \ctor	ImagePlugin(); 
	//
	//# The constructor has protected access. Only instances of $ImagePlugin$ subclasses can be created.
	//
	//# \desc 
	//# The $ImagePlugin$ class is the base class for all image import plugins and image export plugins.
	//# An image import plugin contains the functionality needed to read a 2D pixel image from a resource
	//# file in a specific format. An image export plugin performs the opposite function and writes a 2D
	//# pixel image to a resource file. One or both of these types of plugin classes can be defined in a
	//# plugin module and registered with the Plugin Manager using the $@PluginMgr::RegisterImageImportPlugin@$
	//# and $@PluginMgr::RegisterImageExportPlugin@$ functions.
	//#
	//# An implementation of an $ImagePlugin$ subclass must override all of its pure virtual member functions.
	//
	//# \base	Plugin		An $ImagePlugin$ is a specific type of plugin.
	//
	//# \also	$@ImageImportPlugin@$
	//# \also	$@ImageExportPlugin@$


	//# \function	ImagePlugin::GetImageTypeName		Returns the name of the file type supported by an image plugin.
	//
	//# \proto	virtual const char *GetImageTypeName(void) const = 0;
	//
	//# \desc
	//# The $GetImageTypeName$ function returns the human-readable name of the file type supported by
	//# an image import or image export plugin. This name is displayed to the user when a choice among importers
	//# or exporters can be made.
	//#
	//# The $GetImageTypeName$ function must be overridden by final subclasses of the $ImagePlugin$ class.
	//
	//# \also	$@ImagePlugin::GetImageResourceDescriptor@$


	//# \function	ImagePlugin::GetImageResourceDescriptor		Returns the resource descriptor for the file type supported by an image plugin.
	//
	//# \proto	const ResourceDescriptor *GetImageResourceDescriptor(void) const = 0;
	//
	//# \desc
	//# The $GetImageResourceDescriptor$ function returns a pointer to a permanent resource descriptor
	//# corresponding to the file type supported by an image import or image export plugin.
	//#
	//# The $GetImageResourceDescriptor$ function must be overridden by final subclasses of the $ImagePlugin$ class.
	//
	//# \also	$@ImagePlugin::GetImageTypeName@$
	//# \also	$@ResourceMgr/ResourceDescriptor@$


	class ImagePlugin : public Plugin
	{
		protected:

			ImagePlugin();

		public:

			~ImagePlugin();

			C4API virtual const char *GetImageTypeName(void) const = 0;
			C4API virtual const ResourceDescriptor *GetImageResourceDescriptor(void) const = 0;
	};


	//# \class	ImageImportPlugin		The base class for all image import plugins.
	//
	//# The $ImageImportPlugin$ class represents a generic image import plugin.
	//
	//# \def	class ImageImportPlugin : public ImagePlugin, public ListElement<ImageImportPlugin>
	//
	//# \ctor	ImageImportPlugin();
	//
	//# The constructor has protected access. Only instances of $ImageImportPlugin$ subclasses can be created.
	//
	//# \desc
	//# The $ImageImportPlugin$ class is the base class for all image import plugins. An image import plugin
	//# contains the functionality needed to read a 2D pixel image from a resource file in a specific format.
	//#
	//# An image import plugin should be registered with the Plugin Manager by calling the
	//# $@PluginMgr::RegisterImageImportPlugin@$ function after the plugin has been constructed.
	//#
	//# An implementation of an $ImageImportPlugin$ subclass must override all of its pure virtual member functions
	//# and the pure virtual member functions of the $@ImagePlugin@$ class.
	//
	//# \base	ImagePlugin									An $ImageImportPlugin$ is a specific type of image plugin.
	//# \base	Utilities/ListElement<ImageImportPlugin>	All registered image import plugins are kept in a list.
	//
	//# \also	$@ImageExportPlugin@$
	//# \also	$@PluginMgr::RegisterImageImportPlugin@$


	//# \function	ImageImportPlugin::GetImageFormat		Returns the format of the image data that an image import plugin generates.
	//
	//# \proto	virtual ImageFormat GetImageFormat(void) const = 0;
	//
	//# \desc
	//# The $GetImageFormat$ function returns the format of the image data that an image import plugin generates
	//# when the $@ImageImportPlugin::ImportImageFile@$ function is called. The return value may be one of the
	//# following image formats.
	//
	//# \table	ImageFormat
	//
	//# The $GetImageFormat$ function must be overridden by subclasses of the $ImageImportPlugin$ class.
	//
	//# \also	$@ImageImportPlugin::GetImageFileInfo@$
	//# \also	$@ImageImportPlugin::ImportImageFile@$


	//# \function	ImageImportPlugin::GetImageFileInfo		Returns information about an image file.
	//
	//# \proto	virtual EngineResult GetImageFileInfo(const char *name, Integer2D *size, unsigned_int32 *flags = nullptr, int32 *count = nullptr) = 0;
	//
	//# \param	name	The file name of the image, relative to the C4 application.
	//# \param	size	A pointer to a location that receives the dimensions of the image. This cannot be $nullptr$.
	//# \param	flags	A pointer to a location that receives flags pertaining to the image. This can be $nullptr$ if this information is not needed.
	//# \param	count	A pointer to a location that receives the number of images stored in the file. This can be $nullptr$ if this information is not needed.
	//
	//# \desc
	//# The $GetImageFileInfo$ function returns information about the image file specified by the $name$ paramater.
	//# If the function succeeds, then the return value is $kEngineOkay$. If an error occurred, then the result code
	//# corresponding to the error is returned.
	//
	//# The horizontal and vertical dimensions of the image are stored in the location specified by the $size$ parameter.
	//# If the $flags$ parameter is not $nullptr$, then flags pertaining to the image are stored in the location it points to.
	//# The flags can be a combination (through logical OR) of the following constants.
	//
	//# \table	ImageImportFlags
	//
	//# If the $count$ parameter is not $nullptr$, then the number of images in the file is stored in the location
	//# it points to. (For most formats, this count is expected to be one.)
	//
	//# The $GetImageFileInfo$ function must be overridden by subclasses of the $ImageImportPlugin$ class.
	//
	//# \also	$@ImageImportPlugin::GetImageFormat@$
	//# \also	$@ImageImportPlugin::ImportImageFile@$


	//# \function	ImageImportPlugin::ImportImageFile		Imports an image from a file.
	//
	//# \proto	EngineResult ImportImageFile(const char *name, void **image, Integer2D *size, unsigned_int32 *flags = nullptr, int32 index = 0) = 0;
	//
	//# \param	name	The file name of the image, relative to the C4 application.
	//# \param	image	A pointer to a location that receives a pointer to the buffer in which the image is stored.
	//# \param	size	A pointer to a location that receives the dimensions of the image. This cannot be $nullptr$.
	//# \param	flags	A pointer to a location that receives flags pertaining to the image. This can be $nullptr$ if this information is not needed.
	//# \param	index	The zero-based index of the image to import.
	//
	//# \desc
	//# The $ImportImageFile$ function imports an image from the file specified by the $name$ paramater.
	//# If the function succeeds, then the return value is $kEngineOkay$. If an error occurred, then the result code
	//# corresponding to the error is returned.
	//
	//# A pointer to a buffer containing the image data is stored in the location specified by the $image$ parameter.
	//# This buffer is allocated by the implementation of the $ImportImageFile$ function. Once the image data is no
	//# longer needed, this buffer should be released by calling the $@ImageImportPlugin::ReleaseImageData@$ function.
	//
	//# The horizontal and vertical dimensions of the image are stored in the location specified by the $size$ parameter.
	//# If the $flags$ parameter is not $nullptr$, then flags pertaining to the image are stored in the location it points to.
	//# The flags can be a combination (through logical OR) of the following constants.
	//
	//# \table	ImageImportFlags
	//
	//# The $index$ parameter specifies which image to import for files containing more than one image. It should be
	//# in the range [0,&nbsp;<i>n</i>&nbsp;&minus;&nbsp;1], where <i>n</i> is the image count returned by the
	//# $@ImageImportPlugin::GetImageFileInfo@$ function.
	//
	//# The $ImportImageFile$ function must be overridden by subclasses of the $ImageImportPlugin$ class.
	//
	//# \also	$@ImageImportPlugin::ReleaseImageData@$
	//# \also	$@ImageImportPlugin::GetImageFormat@$
	//# \also	$@ImageImportPlugin::GetImageFileInfo@$


	//# \function	ImageImportPlugin::ReleaseImageData		Releases an imported image.
	//
	//# \proto	virtual void ReleaseImageData(void *image) = 0;
	//
	//# \param	image	A pointer to the image data to release.
	//
	//# \desc
	//# The $ReleaseImageData$ function releases the image data buffer specified by the $image$ parameter that was
	//# previously allocated by the $@ImageImportPlugin::ImportImageFile@$ function.
	//
	//# The $ReleaseImageData$ function must be overridden by subclasses of the $ImageImportPlugin$ class.
	//
	//# \also	$@ImageImportPlugin::ImportImageFile@$


	class ImageImportPlugin : public ImagePlugin, public ListElement<ImageImportPlugin>
	{
		protected:

			C4API ImageImportPlugin();

		public:

			C4API ~ImageImportPlugin();

			using ListElement<ImageImportPlugin>::Previous;
			using ListElement<ImageImportPlugin>::Next;

			C4API virtual ImageFormat GetImageFormat(void) const = 0;
			C4API virtual EngineResult GetImageFileInfo(const char *name, Integer2D *size, unsigned_int32 *flags = nullptr, int32 *count = nullptr) = 0;
			C4API virtual EngineResult ImportImageFile(const char *name, void **image, Integer2D *size, unsigned_int32 *flags = nullptr, int32 index = 0) = 0;
			C4API virtual void ReleaseImageData(void *image) = 0;
	};


	//# \class	ImageExportPlugin		The base class for all image export plugins.
	//
	//# The $ImageExportPlugin$ class represents a generic image export plugin.
	//
	//# \def	class ImageExportPlugin : public ImagePlugin, public ListElement<ImageExportPlugin>
	//
	//# \ctor	ImageExportPlugin();
	//
	//# The constructor has protected access. Only instances of $ImageExportPlugin$ subclasses can be created.
	//
	//# \desc
	//# The $ImageExportPlugin$ class is the base class for all image export plugins. An image export plugin
	//# contains the functionality needed to write a 2D pixel image to a file in a specific format.
	//#
	//# An image export plugin should be registered with the Plugin Manager by calling the
	//# $@PluginMgr::RegisterImageExportPlugin@$ function after the plugin has been constructed.
	//#
	//# An implementation of an $ImageExportPlugin$ subclass must override all of its pure virtual member functions
	//# and the pure virtual member functions of the $@ImagePlugin@$ class.
	//
	//# \base	ImagePlugin									An $ImageExportPlugin$ is a specific type of image plugin.
	//# \base	Utilities/ListElement<ImageExportPlugin>	All registered image export plugins are kept in a list.
	//
	//# \also	$@ImageImportPlugin@$
	//# \also	$@PluginMgr::RegisterImageExportPlugin@$


	//# \function	ImageExportPlugin::ExportImageFile		Exports an image to a file.
	//
	//# \proto	virtual EngineResult ExportImageFile(const char *name, const Color4C *image, const Integer2D& size, unsigned_int32 flags = 0) = 0;
	//
	//# \param	name	The file name of the image, relative to the C4 application.
	//# \param	image	A pointer to a buffer in which the image is stored.
	//# \param	size	The dimensions of the image.
	//# \param	flags	Flags pertaining to the export process.
	//
	//# \desc
	//# The $ExportImageFile$ function exports an image to the file specified by the $name$ paramater.
	//# If the function succeeds, then the return value is $kEngineOkay$. If an error occurred, then the result code
	//# corresponding to the error is returned.
	//
	//# The $image$ parameter specifies the buffer containing the image data, and the $size$ parameter specifies the
	//# horizontal and vertical dimensions of the image. Exactly 4<i>wh</i> bytes are read from the image buffer, where
	//# <i>w</i> and <i>h</i> are the width and height of the image. The image data must be stored in RGBA format with
	//# 8 bits per channel.
	//
	//# The $flags$ parameter specifies various options that control the export process, and it can be a combination
	//# (through logical OR) of the following constants.
	//
	//# \table	ImageExportFlags
	//
	//# The $ExportImageFile$ function must be overridden by subclasses of the $ImageExportPlugin$ class.


	class ImageExportPlugin : public ImagePlugin, public ListElement<ImageExportPlugin>
	{
		protected:

			C4API ImageExportPlugin();

		public:

			C4API ~ImageExportPlugin();

			using ListElement<ImageExportPlugin>::Previous;
			using ListElement<ImageExportPlugin>::Next;

			C4API virtual EngineResult ExportImageFile(const char *name, const Color4C *image, const Integer2D& size, unsigned_int32 flags = 0) = 0;
	};


	class PluginModule : public Module, public ListElement<PluginModule>
	{
		private:

			typedef Plugin *CreateProc(void);

			Plugin		*modulePlugin;

		public:

			PluginModule();
			~PluginModule();

			Plugin *GetPlugin(void) const
			{
				return (modulePlugin);
			}

			EngineResult Load(const char *name);
	};


	//# \class	PluginMgr		The Plugin Manager class.
	//
	//# \def	class PluginMgr : public Manager<PluginMgr>
	//
	//# \desc
	//# The $PluginMgr$ class encapsulates the plugin module functionality of the C4 Engine.
	//# The single instance of the Plugin Manager is constructed during an application's initialization
	//# and destroyed at termination.
	//#
	//# The Plugin Manager's member functions are accessed through the global pointer $ThePluginMgr$.
	//
	//# \also	$@ImageImportPlugin@$
	//# \also	$@ImageExportPlugin@$


	//# \function	PluginMgr::RegisterImageImportPlugin	Registers an image import plugin.
	//
	//# \proto	void RegisterImageImportPlugin(ImageImportPlugin *plugin);
	//
	//# \desc
	//# The $RegisterImageImportPlugin$ function registers an image import plugin with the Plugin Manager.
	//# All image import plugins are kept in a list that can be accessed by calling the
	//# $@PluginMgr::GetImageImportPluginList@$ function.
	//
	//# \also	$@PluginMgr::GetImageImportPluginList@$
	//# \also	$@PluginMgr::RegisterImageExportPlugin@$


	//# \function	PluginMgr::RegisterImageExportPlugin	Registers an image export plugin.
	//
	//# \proto	void RegisterImageExportPlugin(ImageExportPlugin *plugin);
	//
	//# \desc
	//# The $RegisterImageExportPlugin$ function registers an image export plugin with the Plugin Manager.
	//# All image export plugins are kept in a list that can be accessed by calling the
	//# $@PluginMgr::GetImageExportPluginList@$ function.
	//
	//# \also	$@PluginMgr::GetImageExportPluginList@$
	//# \also	$@PluginMgr::RegisterImageImportPlugin@$


	//# \function	PluginMgr::GetImageImportPluginList		Returns the list of registered image import plugins.
	//
	//# \proto	const List<ImageImportPlugin> *GetImageImportPluginList(void) const;
	//
	//# \desc
	//# The $GetImageImportPluginList$ function returns a pointer to the Plugin Manager's list of
	//# registered image import plugins.
	//
	//# \also	$@PluginMgr::GetImageExportPluginList@$
	//# \also	$@PluginMgr::RegisterImageImportPlugin@$


	//# \function	PluginMgr::GetImageExportPluginList		Returns the list of registered image export plugins.
	//
	//# \proto	const List<ImageExportPlugin> *GetImageExportPluginList(void) const;
	//
	//# \desc
	//# The $GetImageExportPluginList$ function returns a pointer to the Plugin Manager's list of
	//# registered image export plugins.
	//
	//# \also	$@PluginMgr::GetImageImportPluginList@$
	//# \also	$@PluginMgr::RegisterImageExportPlugin@$


	//# \function	PluginMgr::GetImportCatalog		Returns the resource catalog for importing files.
	//
	//# \proto	GenericResourceCatalog *GetImportCatalog(void);
	//
	//# \desc
	//# The $GetImportCatalog$ function returns the resource catalog that should be used when reading files
	//# for the purpose of importing data into the engine. The import catalog would normally be passed to
	//# the $@ResourceMgr/Resource::Get@$ function.
	//#
	//# The default location of the import catalog is the $Import/$ directory. Since the import catalog is
	//# not a virtual catalog, resource names in this location should include the complete path beneath the
	//# root $Import/$ directory (without omitting the first directory name).
	//
	//# \also	$@PluginMgr::GetExportCatalog@$
	//# \also	$@ResourceMgr/Resource::Get@$
	//# \also	$@ResourceMgr/GenericResourceCatalog@$


	//# \function	PluginMgr::GetExportCatalog		Returns the resource catalog for exporting files.
	//
	//# \proto	GenericResourceCatalog *GetExportCatalog(void);
	//
	//# \desc
	//# The $GetExportCatalog$ function returns the resource catalog that should be used when writing files
	//# for the purpose of exporting data out of the engine. The export catalog would normally be passed to
	//# the $@ResourceMgr/GenericResourceCatalog::GetResourcePath@$ function.
	//#
	//# The default location of the export catalog is the $Export/$ directory. Since the export catalog is
	//# not a virtual catalog, resource names in this location should include the complete path beneath the
	//# root $Export/$ directory (without omitting the first directory name).
	//
	//# \also	$@PluginMgr::GetImportCatalog@$
	//# \also	$@ResourceMgr/GenericResourceCatalog::GetResourcePath@$
	//# \also	$@ResourceMgr/GenericResourceCatalog@$


	class PluginMgr : public Manager<PluginMgr>
	{
		private:

			List<PluginModule>					pluginModuleList;

			List<ImageImportPlugin>				imageImportPluginList;
			List<ImageExportPlugin>				imageExportPluginList;

			ImageImportPlugin					*targaImageImportPlugin;
			ImageExportPlugin					*targaImageExportPlugin;
			ImageImportPlugin					*sequenceImageImportPlugin;

			MenuItemWidget						*topMenuItem;
			MenuItemWidget						*bottomMenuItem;

			Storage<GenericResourceCatalog>		importCatalog;
			Storage<GenericResourceCatalog>		exportCatalog;

			void LoadPlugins(const char *directory = "");

			void BuildToolsMenu(void);

			void HandleConsoleMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleTimeMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleStatsMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleNetworkMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleScreenshotMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleQuitMenuItem(Widget *menuItem, const WidgetEventData *eventData);

		public:

			PluginMgr(int);
			~PluginMgr();

			EngineResult Construct(void);
			void Destruct(void);

			void RegisterImageImportPlugin(ImageImportPlugin *plugin)
			{
				imageImportPluginList.Append(plugin);
			}

			void RegisterImageExportPlugin(ImageExportPlugin *plugin)
			{
				imageExportPluginList.Append(plugin);
			}

			const List<ImageImportPlugin> *GetImageImportPluginList(void) const
			{
				return (&imageImportPluginList);
			}

			const List<ImageExportPlugin> *GetImageExportPluginList(void) const
			{
				return (&imageExportPluginList);
			}

			ImageImportPlugin *GetTargaImageImportPlugin(void)
			{
				return (targaImageImportPlugin);
			}

			ImageExportPlugin *GetTargaImageExportPlugin(void)
			{
				return (targaImageExportPlugin);
			}

			GenericResourceCatalog *GetImportCatalog(void)
			{
				return (importCatalog);
			}

			GenericResourceCatalog *GetExportCatalog(void)
			{
				return (exportCatalog);
			}

			C4API void AddToolMenuItem(MenuItemWidget *widget, bool end = true);

			void PurgePlugins(void);
	};


	C4API extern PluginMgr *ThePluginMgr;
}


#endif

// ZYUQURM
