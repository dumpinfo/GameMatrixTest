#pragma once

#include "../Leadwerks.h"

namespace Leadwerks
{
	class Prefab : public Map//lua
	{
	public:
        std::string path;
        
		static const int NoStartCall;//lua

		static Entity* Load(Stream* stream, const int flags=Map::LoadScripts);
		static Entity* Load(const std::string& path, const int flags = Map::LoadScripts, const uint64_t fileid=0);//lua	
	};
}
