#pragma once

#include "../Leadwerks.h"
#include <list>

namespace Leadwerks
{
	class Asset;
	class GraphicsDriver;
	
	class AssetReference : public Object
	{
		public:
		std::list<Asset*> instances;
		int instancecount;
		std::string path;
		Driver* driver;
		int time;
		std::list<AssetReference*>::iterator link;
		std::list<Asset*> weakreferences;
		
		AssetReference();
		virtual ~AssetReference();
		
		virtual bool Reload(const int flags=0)=0;
		virtual void Reset()=0;
		virtual AssetReference* Copy()=0;
		virtual Asset* Instance()=0;
		//virtual bool Reload(const int flags)=0;
	};
	
	extern std::list<AssetReference*> AssetReference_list;
}
