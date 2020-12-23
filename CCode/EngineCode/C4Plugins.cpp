 

#include "C4Targa.h"
#include "C4Sequence.h"
#include "C4ToolWindows.h"


using namespace C4;


PluginMgr *C4::ThePluginMgr = nullptr;


namespace C4
{
	template <> PluginMgr Manager<PluginMgr>::managerObject(0);
	template <> PluginMgr **Manager<PluginMgr>::managerPointer = &ThePluginMgr;

	template <> const char *const Manager<PluginMgr>::resultString[] =
	{
		nullptr,
		"Image format unsupported",
		"Unsupported image dimensions",
		"Image index out of range"
	};

	template <> const unsigned_int32 Manager<PluginMgr>::resultIdentifier[] =
	{
		0, 'IFRM', 'ISIZ', 'INDX'
	};

	template class Manager<PluginMgr>;
}


Plugin::Plugin()
{
}

Plugin::~Plugin()
{
}

#if C4DEBUG

	void Plugin::DebugPlugin(void)
	{
	}

#elif C4OPTIMIZED

	void Plugin::OptimizedPlugin(void)
	{
	}

#endif


ImagePlugin::ImagePlugin()
{
}

ImagePlugin::~ImagePlugin()
{
}


ImageImportPlugin::ImageImportPlugin()
{
	ThePluginMgr->RegisterImageImportPlugin(this);
}

ImageImportPlugin::~ImageImportPlugin()
{
}


ImageExportPlugin::ImageExportPlugin()
{
	ThePluginMgr->RegisterImageExportPlugin(this);
}

ImageExportPlugin::~ImageExportPlugin()
{
}


PluginModule::PluginModule() : Module(kModulePlugin)
{
	modulePlugin = nullptr;
}

PluginModule::~PluginModule()
{
}

EngineResult PluginModule::Load(const char *name)
{
	EngineResult result = Module::Load(name);
	if (result != kEngineOkay)
	{
		return (result);
	} 

	#if C4DESKTOP
 
		CreateProc *creator = (CreateProc *) GetFunctionAddress("CreatePlugin");
		if (!creator) 
		{
			return (kEngineModuleCreatorMissing);
		} 

		Plugin *plugin = (*creator)(); 
		if (!plugin) 
		{
			return (kEngineModuleInitFailed);
		}
 
		modulePlugin = plugin;

	#endif

	return (kEngineOkay);
}


PluginMgr::PluginMgr(int)
{
}

PluginMgr::~PluginMgr()
{
}

EngineResult PluginMgr::Construct(void)
{
	new(importCatalog) GenericResourceCatalog("Import/");
	new(exportCatalog) GenericResourceCatalog("Export/");

	BuildToolsMenu();

	targaImageImportPlugin = new TargaImageImportPlugin;
	targaImageExportPlugin = new TargaImageExportPlugin;
	sequenceImageImportPlugin = new SequenceImageImportPlugin;

	Engine::Report("Plugin Manager", kReportLog | kReportHeading);
	Engine::Report("<table class=\"data\" cellspacing=\"0\" cellpadding=\"0\">\r\n", kReportLog);

	LoadPlugins();

	Engine::Report("</table>\r\n", kReportLog);

	return (kEngineOkay);
}

void PluginMgr::Destruct(void)
{
	pluginModuleList.Purge();

	delete sequenceImageImportPlugin;
	delete targaImageExportPlugin;
	delete targaImageImportPlugin;

	exportCatalog->~GenericResourceCatalog();
	importCatalog->~GenericResourceCatalog();
}

void PluginMgr::LoadPlugins(const char *directory)
{
	#if C4DESKTOP

		Map<FileReference>		fileMap;

		TheEngine->GetPluginMap(directory, &fileMap);
		FileReference *reference = fileMap.First();
		while (reference)
		{
			if (!(reference->GetFlags() & kFileDirectory))
			{
				String<kMaxFileNameLength> moduleName(directory);
				if (directory[0] != 0)
				{
					moduleName += '/';
				}

				moduleName += reference->GetName();
				moduleName[Text::GetResourceNameLength(moduleName)] = 0;

				#if C4LOG_FILE

					Engine::Report("<tr><th>", kReportLog);
					Engine::Report(moduleName, kReportLog);
					Engine::Report("</th><td>\r\n", kReportLog);

				#endif

				PluginModule *pluginModule = new PluginModule;
				EngineResult result = pluginModule->Load(moduleName);
				if (result == kEngineOkay)
				{
					pluginModuleList.Append(pluginModule);
				}
				else
				{
					delete pluginModule;
				}

				#if C4LOG_FILE

					Engine::LogResult(result);
					Engine::Report("</td></tr>\r\n", kReportLog);

				#endif
			}

			reference = reference->Next();
		}

		reference = fileMap.First();
		while (reference)
		{
			if (reference->GetFlags() & kFileDirectory)
			{
				String<kMaxFileNameLength> directoryName(directory);
				if (directory[0] != 0)
				{
					directoryName += '/';
				}

				LoadPlugins(directoryName += reference->GetName());
			}

			reference = reference->Next();
		}

	#endif
}

void PluginMgr::PurgePlugins(void)
{
	PluginModule *plugin = pluginModuleList.Last();
	while (plugin)
	{
		delete plugin->GetPlugin();
		plugin = plugin->Previous();
	}
}

void PluginMgr::BuildToolsMenu(void)
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	PulldownMenuWidget *toolsMenu = TheInterfaceMgr->GetToolsMenu();

	topMenuItem = nullptr;
	bottomMenuItem = new MenuItemWidget(kLineSolid);
	toolsMenu->AppendMenuItem(bottomMenuItem);

	toolsMenu->AppendMenuItem(new MenuItemWidget(table->GetString(StringID('MENU', 'TOOL', 'CONS')), WidgetObserver<PluginMgr>(this, &PluginMgr::HandleConsoleMenuItem)));
	toolsMenu->AppendMenuItem(new MenuItemWidget(table->GetString(StringID('MENU', 'TOOL', 'TIME')), WidgetObserver<PluginMgr>(this, &PluginMgr::HandleTimeMenuItem)));
	toolsMenu->AppendMenuItem(new MenuItemWidget(table->GetString(StringID('MENU', 'TOOL', 'STAT')), WidgetObserver<PluginMgr>(this, &PluginMgr::HandleStatsMenuItem)));
	toolsMenu->AppendMenuItem(new MenuItemWidget(table->GetString(StringID('MENU', 'TOOL', 'NTWK')), WidgetObserver<PluginMgr>(this, &PluginMgr::HandleNetworkMenuItem)));
	toolsMenu->AppendMenuItem(new MenuItemWidget(kLineSolid));
	toolsMenu->AppendMenuItem(new MenuItemWidget(table->GetString(StringID('MENU', 'TOOL', 'SHOT')), WidgetObserver<PluginMgr>(this, &PluginMgr::HandleScreenshotMenuItem), Shortcut('\\')));
	toolsMenu->AppendMenuItem(new MenuItemWidget(kLineSolid));
	toolsMenu->AppendMenuItem(new MenuItemWidget(table->GetString(StringID('MENU', 'TOOL', 'QUIT')), WidgetObserver<PluginMgr>(this, &PluginMgr::HandleQuitMenuItem), Shortcut('Q')));
}

void PluginMgr::AddToolMenuItem(MenuItemWidget *widget, bool end)
{
	PulldownMenuWidget *toolsMenu = TheInterfaceMgr->GetToolsMenu();

	if (end)
	{
		toolsMenu->InsertMenuItemBefore(widget, bottomMenuItem);
	}
	else if (topMenuItem)
	{
		toolsMenu->InsertMenuItemAfter(widget, topMenuItem);
		topMenuItem = widget;
	}
	else
	{
		topMenuItem = widget;
		toolsMenu->PrependMenuItem(widget);
	}
}

void PluginMgr::HandleConsoleMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	TheInterfaceMgr->SetActiveWindow(TheConsoleWindow);
}

void PluginMgr::HandleTimeMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	TheEngine->HandleTimeCommand(nullptr, nullptr);
}

void PluginMgr::HandleStatsMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	#if C4STATS

		TheEngine->HandleStatCommand(nullptr, nullptr);

	#endif
}

void PluginMgr::HandleNetworkMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	#if C4DIAGS

		TheEngine->HandleNetCommand(nullptr, nullptr);

	#endif
}

void PluginMgr::HandleScreenshotMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	TheEngine->HandleShotCommand(nullptr, "C4_shot_#");
}

void PluginMgr::HandleQuitMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	TheEngine->HandleQuitCommand(nullptr, nullptr);
}

// ZYUQURM
