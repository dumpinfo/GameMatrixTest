#pragma once
#include "../ENetNetworkDriver.h"

namespace Leadwerks
{
	class Bank;
	class Peer;
	class Host;
	class Message;
	
	class ENetClient : public Client
	{
	public:
		ENetHost* enethost;
		
		ENetClient();
		virtual ~ENetClient();
		
		virtual Peer* Connect(const Address& address);
		virtual void Disconnect(const bool force=false);
		virtual bool Send(const int message, const std::string& text, const int channel=0, const int flags=MESSAGE_SEQUENCED);
		virtual bool Send(const int message, Bank* data=NULL, const int channel=0, const int flags=MESSAGE_SEQUENCED);
		virtual Message* Update(const int timeout=0);
	};
}
