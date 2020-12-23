#pragma once
#include "../../../../Leadwerks.h"
#include "Classes/ENetClient.h"
#include "Classes/ENetServer.h"
#include "Classes/ENetPeer.h"

namespace Leadwerks
{
	class ENetNetworkDriver : public NetworkDriver
	{
	public:
		ENetNetworkDriver();
		~ENetNetworkDriver();
		
		virtual Client* CreateClient();
		virtual Peer* CreatePeer();
		virtual Server* CreateServer(const int port);
	};
	extern bool _ENetInitialized;
}
