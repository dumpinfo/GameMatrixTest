#pragma once
#include "../ENetNetworkDriver.h"

namespace Leadwerks
{
	class Bank;
	class Peer;
	class Host;
	class Message;
	class NetworkDriver;
	
	class ENetServer : public Server
	{
	public:
		ENetHost* enethost;
		//vector<Packet*> queuedpackets;
		
		ENetServer();
		virtual ~ENetServer();
		
		virtual Message* Update(const int timeout);
		virtual void Disconnect(Peer* client, const bool force=false);
		virtual bool Send(Peer* client, const int message, Bank* data=NULL, const int channel=0, const int flags=MESSAGE_SEQUENCED);
		virtual bool Send(Peer* client, const int message, const std::string& text, const int channel=0, const int flags=MESSAGE_SEQUENCED);
		virtual bool Broadcast(const int message, const std::string& text, const int channel=0, const int flags=MESSAGE_SEQUENCED);
		virtual bool Broadcast(const int message, Bank* data=NULL, const int channel=0, const int flags=MESSAGE_SEQUENCED);
	};
}
