#pragma once
#include "../Leadwerks.h"

namespace Leadwerks
{
	class Peer;
	class Address;
	
	class Server
	{
	public:
		NetworkDriver* driver;
		std::map<Address,Peer*> client;
		std::string password;
		
		Server();
		virtual ~Server();
		
		virtual Message* Update(const int timeout=0)=0;
		virtual void Disconnect(Peer* client, const bool force=false)=0;
		virtual bool Send(Peer* client, const int message, Bank* data=NULL, const int channel=0, const int flags=MESSAGE_SEQUENCED)=0;
		virtual bool Send(Peer* client, const int message, const std::string& text, const int channel=0, const int flags=MESSAGE_SEQUENCED)=0;
		virtual bool Broadcast(const int message, const std::string& text, const int channel=0, const int flags=MESSAGE_SEQUENCED)=0;
		virtual bool Broadcast(const int message, Bank* data=NULL, const int channel=0, const int flags=MESSAGE_SEQUENCED)=0;
	};
}
