 

#include "C4Commands.h"
#include "C4World.h"
#include "C4Movies.h"
#include "C4ToolWindows.h"
#include "C4Application.h"


using namespace C4;


Command::Command(const char *name, ObserverType *observer)
{
	commandName = name;
	AddObserver(observer);
}

Command::~Command()
{
}


void Engine::HandleTimeCommand(Command *command, const char *text)
{
	TimeWindow::Open();
}

void Engine::HandleShotCommand(Command *command, const char *text)
{
	String<kMaxFileNameLength> name(text);
	int32 p = Text::FindChar(text, '#');
	if (p != -1)
	{
		static int32 shotIndex = 0;

		name[p] = 0;

		int32 i = ++shotIndex;
		if (i < 10)
		{
			name += "000";
		}
		else if (i < 100)
		{
			name += "00";
		}
		else if (i < 1000)
		{
			name += "0";
		}

		name += i;
	}

	name += ".tga";

	String<kMaxFileNameLength> path(TheResourceMgr->GetSaveCatalog()->GetRootPath());
	path += name;

	int32 width = TheDisplayMgr->GetDisplayWidth();
	int32 height = TheDisplayMgr->GetDisplayHeight();

	int32 pixelCount = width * height;
	Color4C *image = new Color4C[pixelCount];

	TheGraphicsMgr->ReadImageBuffer(Rect(0, 0, width, height), image, width);
	ThePluginMgr->GetTargaImageExportPlugin()->ExportImageFile(path, image, Integer2D(width, height));

	delete[] image;
}

void Engine::HandleRecordCommand(Command *command, const char *text)
{
	if ((text[0] != 0) && (!TheMovieMgr->Recording()) && (!TheSoundMgr->Recording()))
	{
		int32 rate = 30;
		bool video = true;
		bool audio = true;
		String<kMaxFileNameLength> name("");

		while (*text != 0)
		{
			String<kMaxCommandLength>	param;

			text += Text::ReadString(text, param, kMaxCommandLength);
			text += Data::GetWhitespaceLength(text);

			if (param == "-rate")
			{
				text += Text::ReadString(text, param, kMaxCommandLength);
				rate = Min(Max(Text::StringToInteger(param), 1), 60);
			}
			else if (param == "-novideo")
			{
				video = false;
			}
			else if (param == "-noaudio")
			{
				audio = false;
			}
			else
			{ 
				name = param;
			}
 
			text += Data::GetWhitespaceLength(text);
		} 

		if (name[0] != 0)
		{ 
			if (video)
			{ 
				TheMovieMgr->StartRecording(rate, name); 
			}

			if (audio)
			{ 
				TheSoundMgr->StartRecording(name);
			}
		}
	}
	else
	{
		TheMovieMgr->StopRecording();
		TheSoundMgr->StopRecording();
	}
}

void Engine::HandleUndefCommand(Command *command, const char *text)
{
	if (*text == '$')
	{
		Variable *variable = GetVariable(text + 1);
		if ((variable) && (!(variable->GetVariableFlags() & kVariablePermanent)))
		{
			delete variable;
		}
	}
}

void Engine::HandleBindCommand(Command *command, const char *text)
{
	if ((TheInputMgr) && (*text != 0))
	{
		Variable *variable = GetVariable("device");
		InputDevice *device = (variable) ? TheInputMgr->FindDevice(variable->GetValue()) : nullptr;
		InputDevice *keyboard = TheInputMgr->FindDevice(kInputDeviceKeyboard);

		if (!device)
		{
			device = keyboard;
		}

		if (device)
		{
			String<kMaxVariableValueLength>		name;

			text += Text::ReadString(text, name, kMaxInputControlNameLength);

			InputControl *control = device->FindControl(name);
			if ((!control) && (keyboard))
			{
				control = keyboard->FindControl(name);
			}

			if (control)
			{
				control->SetControlAction(nullptr);

				text += Data::GetWhitespaceLength(text);
				Text::ReadString(text, name, kMaxVariableValueLength);

				unsigned_int32 k = name[0];
				if (k != 0)
				{
					if (k == '%')
					{
						if (name.Length() == 5)
						{
							unsigned_int32 type = (name[1] << 24) | (name[2] << 16) | (name[3] << 8) | name[4];
							Action *action = TheInputMgr->FindAction(type);
							if (action)
							{
								control->SetControlAction(action);
							}
						}
					}
					else
					{
						CommandAction *action = new CommandAction(name);
						TheInputMgr->AddAction(action);
						control->SetControlAction(action);
					}
				}
			}
		}
	}
}

void Engine::HandleUnbindCommand(Command *command, const char *text)
{
	if (*text != 0)
	{
		Variable *variable = GetVariable("device");
		InputDevice *device = (variable) ? TheInputMgr->FindDevice(variable->GetValue()) : nullptr;
		InputDevice *keyboard = TheInputMgr->FindDevice(kInputDeviceKeyboard);

		if (!device)
		{
			device = keyboard;
		}

		if (device)
		{
			String<kMaxInputControlNameLength>		name;

			Text::ReadString(text, name, kMaxInputControlNameLength);
			InputControl *control = device->FindControl(name);
			if (!control)
			{
				control = keyboard->FindControl(name);
			}

			if (control)
			{
				control->SetControlAction(nullptr);
			}
		}
	}
}

void Engine::HandleLoadCommand(Command *command, const char *text)
{
	if (*text != 0)
	{
		ResourceName	name;

		Text::ReadString(text, name, kMaxResourceNameLength);
		TheApplication->LoadWorld(name);
	}
}

void Engine::HandleUnloadCommand(Command *command, const char *text)
{
	TheApplication->UnloadWorld();
}

void Engine::HandleCmdCommand(Command *command, const char *text)
{
	TheConsoleWindow->AddText("\n[#FFF]Commands");

	const Command *cmd = GetFirstCommand();
	while (cmd)
	{
		TheConsoleWindow->AddText(cmd->GetName());
		cmd = cmd->Next();
	}
}

void Engine::HandleVarCommand(Command *command, const char *text)
{
	TheConsoleWindow->AddText("\n[#FFF]Variables");

	const Variable *variable = GetFirstVariable();
	while (variable)
	{
		String<kMaxVariableNameLength + kMaxVariableValueLength + 6> string("$");
		string += variable->GetName();
		string += " = \"";
		string += variable->GetValue();
		string += "\"";

		TheConsoleWindow->AddText(string);
		variable = variable->Next();
	}
}

void Engine::HandleExecCommand(Command *command, const char *text)
{
	ExecuteFile(text);
}

void Engine::HandleImportCommand(Command *command, const char *text)
{
	ExecuteFile(text, ThePluginMgr->GetImportCatalog());
}

void Engine::HandleSayCommand(Command *command, const char *text)
{
	TheMessageMgr->SendChatMessage(text);
}

void Engine::HandleAddressCommand(Command *command, const char *text)
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (!TheMessageMgr->Multiplayer())
	{
		Report(table->GetString(StringID('CONS', 'NONT')));
	}

	Report(String<kMaxCommandLength>(table->GetString(StringID('CONS', 'ADDR'))) + MessageMgr::AddressToString(TheNetworkMgr->GetLocalAddress(), true));
}

void Engine::HandleResolveCommand(Command *command, const char *text)
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (!TheMessageMgr->Multiplayer())
	{
		Report(table->GetString(StringID('CONS', 'NONT')));
	}
	else
	{
		const char *message = table->GetString(StringID('CONS', 'RSLV'));
		Report(String<kMaxCommandLength>(message) + text);

		DomainNameResolver *resolver = TheNetworkMgr->ResolveAddress(text);
		resolver->SetCompletionProc(&ResolverComplete);
	}
}

void Engine::ResolverComplete(DomainNameResolver *resolver, void *cookie)
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (resolver->GetResolveResult() == kEngineOkay)
	{
		TheConsoleWindow->AddText(String<kMaxCommandLength>(table->GetString(StringID('CONS', 'RNAM'))) + resolver->GetName());
		TheConsoleWindow->AddText(String<kMaxCommandLength>(table->GetString(StringID('CONS', 'RADR'))) + MessageMgr::AddressToString(resolver->GetAddress()));
	}
	else
	{
		Report(table->GetString(StringID('CONS', 'RERR')));
	}
}

void Engine::HandleDisconnectCommand(Command *command, const char *text)
{
	TheMessageMgr->DisconnectAll();
}

#if C4STATS

	void Engine::HandleStatCommand(Command *command, const char *text)
	{
		StatsWindow::Open();
	}

#endif

#if C4DIAGS

	void Engine::HandleWireCommand(Command *command, const char *text)
	{
		unsigned_int32 flags = TheGraphicsMgr->GetDiagnosticFlags();

		if (text[0] == 0)
		{
			TheGraphicsMgr->SetDiagnosticFlags(flags ^ kDiagnosticWireframe);
		}
		else
		{
			int32 n = Text::StringToInteger(text);
			flags &= ~(kDiagnosticWireframe | kDiagnosticDepthTest);

			if (n == 0)
			{
				TheGraphicsMgr->SetDiagnosticFlags(flags);
			}
			else if (n == 1)
			{
				TheGraphicsMgr->SetDiagnosticFlags(flags | kDiagnosticWireframe);
			}
			else
			{
				TheGraphicsMgr->SetDiagnosticFlags(flags | (kDiagnosticWireframe | kDiagnosticDepthTest));
			}
		}
	}

	void Engine::HandleNormCommand(Command *command, const char *text)
	{
		TheGraphicsMgr->SetDiagnosticFlags(TheGraphicsMgr->GetDiagnosticFlags() ^ kDiagnosticNormals);
	}

	void Engine::HandleTangCommand(Command *command, const char *text)
	{
		TheGraphicsMgr->SetDiagnosticFlags(TheGraphicsMgr->GetDiagnosticFlags() ^ kDiagnosticTangents);
	}

	void Engine::HandleLrgnCommand(Command *command, const char *text)
	{
		World *world = TheWorldMgr->GetWorld();
		if (world)
		{
			world->SetDiagnosticFlags(world->GetDiagnosticFlags() ^ kDiagnosticLightRegions);
		}
	}

	void Engine::HandleSrgnCommand(Command *command, const char *text)
	{
		World *world = TheWorldMgr->GetWorld();
		if (world)
		{
			world->PurgeShadowDiagnosticData();
			world->SetDiagnosticFlags(world->GetDiagnosticFlags() | kDiagnosticShadowRegions);
		}
	}

	void Engine::HandleDoffCommand(Command *command, const char *text)
	{
		World *world = TheWorldMgr->GetWorld();
		if (world)
		{
			world->SetDiagnosticFlags(world->GetDiagnosticFlags() & ~kDiagnosticShadowRegions);
			world->PurgeShadowDiagnosticData();
		}
	}

	void Engine::HandleSpthCommand(Command *command, const char *text)
	{
		World *world = TheWorldMgr->GetWorld();
		if (world)
		{
			world->SetDiagnosticFlags(world->GetDiagnosticFlags() ^ kDiagnosticSourcePaths);
		}
	}

	void Engine::HandleBodyCommand(Command *command, const char *text)
	{
		World *world = TheWorldMgr->GetWorld();
		if (world)
		{
			unsigned_int32 flags = world->GetDiagnosticFlags();
			if (flags & kDiagnosticRigidBodies)
			{
				world->SetDiagnosticFlags(flags & ~kDiagnosticRigidBodies);
				world->PurgeRigidBodyDiagnosticData();
			}
			else
			{
				world->SetDiagnosticFlags(flags | kDiagnosticRigidBodies);
			}
		}
	}

	void Engine::HandleCtacCommand(Command *command, const char *text)
	{
		World *world = TheWorldMgr->GetWorld();
		if (world)
		{
			unsigned_int32 flags = world->GetDiagnosticFlags();
			if (flags & kDiagnosticContacts)
			{
				world->SetDiagnosticFlags(flags & ~kDiagnosticContacts);
				world->PurgeContactDiagnosticData();
			}
			else
			{
				world->SetDiagnosticFlags(flags | kDiagnosticContacts);
			}
		}
	}

	void Engine::HandleFbufCommand(Command *command, const char *text)
	{
		static const char bufferName[6][5] =
		{
			"refl", "refr", "strc", "velo", "occl", "glbl"
		};

		for (int32 a = 0; a < 6; a++)
		{
			if (Text::CompareText(text, bufferName[a]))
			{
				FrameBufferWindow::Open(a);
				break;
			}
		}
	}

	void Engine::HandleSmapCommand(Command *command, const char *text)
	{
		static const char bufferName[3][5] =
		{
			"inft", "pont", "spot"
		};

		for (int32 a = 0; a < 3; a++)
		{
			if (Text::CompareText(text, bufferName[a]))
			{
				ShadowMapWindow::Open(FrameBufferProcess::kFrameBufferInfiniteShadow + a);
				break;
			}
		}
	}

	void Engine::HandleNetCommand(Command *command, const char *text)
	{
		NetworkWindow::Open();
	}

	void Engine::HandleExtCommand(Command *command, const char *text)
	{
		ExtensionsWindow::Open();
	}

	void Engine::HandleRsrcCommand(Command *command, const char *text)
	{
		TheConsoleWindow->AddText("\n[#FFF]Resource type[RGHT]Cache size");

		const ResourceCatalog *catalog = TheResourceMgr->GetVirtualCatalog();
		const Map<ResourceTracker> *trackerMap = catalog->GetTrackerMap();

		const ResourceTracker *tracker = trackerMap->First();
		while (tracker)
		{
			TheConsoleWindow->AddText((String<kMaxCommandLength>(Text::TypeToString((tracker->GetResourceType() << 8) | 0x20)) += "[RGHT]") += tracker->GetCurrentCacheSize());
			tracker = tracker->Next();
		}
	}

	void Engine::HandleHeapCommand(Command *command, const char *text)
	{
		TheConsoleWindow->AddText("\n[#FFF]Memory heap[RGHT]Bytes used");

		const Heap *heap = MemoryMgr::GetFirstHeap();
		do
		{
			TheConsoleWindow->AddText((String<kMaxCommandLength>(heap->GetHeapName()) += "[RGHT]") += heap->GetTotalSize());
			heap = heap->GetNextHeap();
		} while (heap);

		#if C4DEBUG_MEMORY

			unsigned_int32 systemSize = 0;
			const MemBlockHeader *bh = MemoryMgr::GetFirstSystemBlock();
			while (bh)
			{
				systemSize += bh->physicalSize;
				bh = bh->nextBlock;
			}

			TheConsoleWindow->AddText(String<kMaxCommandLength>("System[RGHT]") += systemSize);

		#endif
	}

#endif

void Engine::HandleQuitCommand(Command *command, const char *text)
{
	if (TheInterfaceMgr->QuitEnabled())
	{
		Quit();
	}
}

// ZYUQURM
