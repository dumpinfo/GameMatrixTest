#pragma once

#include "../../../Leadwerks.h"

namespace Leadwerks
{
	class NetworkDriver : public Driver//lua
	{
	public:
		NetworkDriver();
		~NetworkDriver();
		
		virtual Client* CreateClient()=0;
		virtual Peer* CreatePeer()=0;
		virtual Server* CreateServer(const int port)=0;
		
		static NetworkDriver* GetCurrent();//lua
		static void SetCurrent(NetworkDriver* driver);//lua
	};
	extern NetworkDriver* CurrentNetworkDriver;
	//Server* CreateServer(const int port);
	//Client* CreateClient();
}
