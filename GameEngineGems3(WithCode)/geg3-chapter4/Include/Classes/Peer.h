#pragma once
#include "../Leadwerks.h"

namespace Leadwerks
{
	class Address;
	class NetworkDriver;

	class Peer
	{
	public:
		NetworkDriver* driver;
		Address address;
		
		Peer();
		virtual ~Peer();
	};
}
