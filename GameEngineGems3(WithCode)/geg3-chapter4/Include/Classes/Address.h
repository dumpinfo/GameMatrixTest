#pragma once
#include "../Leadwerks.h"

namespace Leadwerks
{
	class Address
	{
	public:
		unsigned int host;
		unsigned short port;
		std::string name;
		
		Address();
		Address(const unsigned int ip, const unsigned short port);
		Address(const std::string& name, const unsigned short port);
	};
}
