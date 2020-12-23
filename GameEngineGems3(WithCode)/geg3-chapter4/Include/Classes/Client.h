#pragma once
#include "../Leadwerks.h"

namespace Leadwerks
{
	class Bank;
	class Peer;
	class Host;
	class Message;
	class NetworkDriver;

	class Client
	{
	public:
		NetworkDriver* driver;
		Peer* server;
		int team;
		
		Client();
		virtual ~Client();
		
		virtual Peer* Connect(const Address& address)=0;
		virtual void Disconnect(const bool force=false)=0;
		virtual bool Send(const int message, const std::string& text, const int channel=0, const int flags=MESSAGE_SEQUENCED)=0;
		virtual bool Send(const int message, Bank* data=NULL, const int channel=0, const int flags=MESSAGE_SEQUENCED)=0;
		virtual Message* Update(const int timeout=0)=0;
		
		/*virtual bool Say(const std::string& text)=0;
		virtual bool TeamSay(const std::string& text)=0;
		virtual bool Join(const int team)=0;
		virtual int GetTeam();*/
	};
}
