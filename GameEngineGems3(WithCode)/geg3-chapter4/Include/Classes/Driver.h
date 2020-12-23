#pragma once

#include "../Leadwerks.h"

namespace Leadwerks
{
	class Asset;
	
	//Rename this "Module"
	class Driver : public Object//lua
	{
	public:
		std::string name;
		//std::map<std::string,Asset*> loadedassets;
		std::map<uint64_t, std::map<std::string, Asset*> > workshopfiles;
		virtual std::string GetName();//lua
		
		Driver();
		virtual ~Driver();
		
		virtual Asset* FindAsset(const std::string& path, const uint64_t fileid=0);
	};
}
