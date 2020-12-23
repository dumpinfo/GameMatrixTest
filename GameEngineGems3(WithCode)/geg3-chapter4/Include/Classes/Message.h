#pragma once
#include "../Leadwerks.h"

namespace Leadwerks
{
	class Peer;
	class Bank;

	class Message
	{
	public:
		int id;
		Peer* peer;
		Bank* data;
		
		Message(const int id=0, Peer* peer=NULL, Bank* data=NULL);
		virtual ~Message();
	};
}
