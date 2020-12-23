#include "C4Application.h"
#include "C4Engine.h"
#include "C4World.h"


using namespace C4;


Application *C4::TheApplication = nullptr;


Application::Application() : Singleton<Application>(TheApplication)
{
}

Application::~Application()
{
}

#if C4DEBUG

	void Application::DebugApplication(void)
	{
	}

#elif C4OPTIMIZED

	void Application::OptimizedApplication(void)
	{
	}

#endif

void Application::HandleConnectionEvent(ConnectionEvent event, const NetworkAddress& address, const void *param)
{
	String<63>	text;

	switch (event)
	{
		case kConnectionAttemptFailed:

			text = "[#FFF]Fail: [#8FF]";
			text += MessageMgr::AddressToString(address, true);
			Engine::Report(text);
			break;

		case kConnectionClientOpened:

			text = "[#FFF]Open: [#8FF]";
			text += MessageMgr::AddressToString(address, true);
			Engine::Report(text);
			break;

		case kConnectionServerAccepted:

			text = "[#FFF]Accept: [#8FF]";
			text += MessageMgr::AddressToString(address, true);
			Engine::Report(text);
			break;

		case kConnectionServerClosed:
		case kConnectionClientClosed:

			text = "[#FFF]Close: [#8FF]";
			text += MessageMgr::AddressToString(address, true);
			Engine::Report(text);
			break;

		case kConnectionServerTimedOut:
		case kConnectionClientTimedOut:

			text = "[#FFF]Timeout: [#8FF]";
			text += MessageMgr::AddressToString(address, true);
			Engine::Report(text);
			break;
	}
}

void Application::HandlePlayerEvent(PlayerEvent event, Player *player, const void *param)
{
}

void Application::HandleGameEvent(GameEvent event, const void *param)
{
}

Message *Application::CreateMessage(MessageType type, Decompressor& data) const
{
	return (nullptr);
}

void Application::ReceiveMessage(Player *from, const NetworkAddress& address, const Message *message)
{
}

EngineResult Application::LoadWorld(const char *name)
{
	return (TheWorldMgr->LoadWorld(name));
}

void Application::UnloadWorld(void)
{
	TheWorldMgr->UnloadWorld();
}

void Application::ApplicationTask(void)
{
}

// ZYUQURM
