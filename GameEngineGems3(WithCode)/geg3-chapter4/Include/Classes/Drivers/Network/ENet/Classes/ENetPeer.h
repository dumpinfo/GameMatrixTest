#pragma once
#include "../ENetNetworkDriver.h"

namespace Leadwerks
{
	class ENetPeer : public Peer
	{
	public:
		::ENetPeer* enetpeer;
		
		ENetPeer();
		virtual ~ENetPeer();
	};
}
